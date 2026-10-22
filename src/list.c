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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

// List heads for garbage collection.
ಸ್ಥಿರ list_T		*first_list = ರಿಕ್ತ;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    ಪ್ರತಿ ((lw) = (l)->lv_watch; (lw) != ರಿಕ್ತ; (lw) = (lw)->lw_next)

ಸ್ಥಿರ ಶೂನ್ಯ list_free_item(list_T *l, listitem_T *item);

/*
 * Add a watcher to a list.
 */
    ಶೂನ್ಯ
list_add_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_next = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    ಶೂನ್ಯ
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	ಯದಿ (lw == lwrem)
	{
	    *lwp = lw->lw_next;
	    ಭಙ್ಗ;
	}
	lwp = &lw->lw_next;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_fix_watch(list_T *l, listitem_T *item)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	ಯದಿ (lw->lw_item == item)
	    lw->lw_item = item->li_next;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    ಯದಿ (first_list != ರಿಕ್ತ)
	first_list->lv_used_prev = l;
    l->lv_used_prev = ರಿಕ್ತ;
    l->lv_used_next = first_list;
    first_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(ಶೂನ್ಯ)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    ಯದಿ (l != ರಿಕ್ತ)
	list_init(l);
    ಪ್ರತ್ಯಾಗಮ l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ಯದಿ (alloc_fail_id == id && alloc_does_fail(ಪರಿಮಾಣ(list_T)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
    ಪ್ರತ್ಯಾಗಮ (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    list_T	*l;

    ಯದಿ (ಎಣಿಕೆ > 0
	    && (size_t)ಎಣಿಕೆ > (SIZE_MAX - ಪರಿಮಾಣ(list_T)) / ಪರಿಮಾಣ(listitem_T))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    l = (list_T *)alloc_clear(ಪರಿಮಾಣ(list_T) + ಎಣಿಕೆ * ಪರಿಮಾಣ(listitem_T));
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    list_init(l);

    ಯದಿ (ಎಣಿಕೆ <= 0)
	ಪ್ರತ್ಯಾಗಮ l;

    listitem_T	*li = (listitem_T *)(l + 1);
    ಪೂರ್ಣಾಙ್ಕ		i;

    l->lv_len = ಎಣಿಕೆ;
    l->lv_with_ವಸ್ತುಗಳು = ಎಣಿಕೆ;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + ಎಣಿಕೆ - 1;
    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
    {
	ಯದಿ (i == 0)
	    li->li_prev = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    li->li_prev = li - 1;
	ಯದಿ (i == ಎಣಿಕೆ - 1)
	    li->li_next = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    li->li_next = li + 1;
	++li;
    }

    ಪ್ರತ್ಯಾಗಮ l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    ಶೂನ್ಯ
list_set_item(list_T *l, ಪೂರ್ಣಾಙ್ಕ idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    ಪೂರ್ಣಾಙ್ಕ
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ಯದಿ (alloc_fail_id == id && alloc_does_fail(ಪರಿಮಾಣ(list_T)))
	ಪ್ರತ್ಯಾಗಮ FAIL;
#endif
    ಪ್ರತ್ಯಾಗಮ rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    ಶೂನ್ಯ
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ಯದಿ (l != ರಿಕ್ತ)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    ಶೂನ್ಯ
list_unref(list_T *l)
{
    ಯದಿ (l != ರಿಕ್ತ && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_free_contents(list_T *l)
{
    listitem_T *item;

    ಯದಿ (l->lv_first != &range_list_item)
	ಪ್ರತಿ (item = l->lv_first; item != ರಿಕ್ತ; item = l->lv_first)
	{
	    // Remove the item before deleting it.
	    l->lv_first = item->li_next;
	    clear_tv(&item->li_tv);
	    list_free_item(l, item);
	}
}

/*
 * Go through the list of lists and free items without the copyID.
 * But don't free a list that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_free_nonref(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    list_T	*ll;
    ಪೂರ್ಣಾಙ್ಕ		did_free = FALSE;

    ಪ್ರತಿ (ll = first_list; ll != ರಿಕ್ತ; ll = ll->lv_used_next)
	ಯದಿ ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == ರಿಕ್ತ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    ಪ್ರತ್ಯಾಗಮ did_free;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    ಯದಿ (l->lv_used_prev == ರಿಕ್ತ)
	first_list = l->lv_used_next;
    ಅನ್ಯಥಾ
	l->lv_used_prev->lv_used_next = l->lv_used_next;
    ಯದಿ (l->lv_used_next != ರಿಕ್ತ)
	l->lv_used_next->lv_used_prev = l->lv_used_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    ಶೂನ್ಯ
list_free_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    list_T	*ll, *ll_next;

    ಪ್ರತಿ (ll = first_list; ll != ರಿಕ್ತ; ll = ll_next)
    {
	ll_next = ll->lv_used_next;
	ಯದಿ ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == ರಿಕ್ತ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    ಶೂನ್ಯ
list_free(list_T *l)
{
    ಯದಿ (in_free_unref_ವಸ್ತುಗಳು)
	ಪ್ರತ್ಯಾಗಮ;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_free_item(list_T *l, listitem_T *item)
{
    ಯದಿ (l->lv_with_ವಸ್ತುಗಳು == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_ವಸ್ತುಗಳು)
	vim_free(item);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    ಶೂನ್ಯ
listitem_free(list_T *l, listitem_T *item)
{
    clear_tv(&item->li_tv);
    list_free_item(l, item);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    ಶೂನ್ಯ
listitem_remove(list_T *l, listitem_T *item)
{
    vimlist_remove(l, item, item);
    listitem_free(l, item);
}

/*
 * Get the number of items in a list.
 */
    long
list_len(list_T *l)
{
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0L;
    ಪ್ರತ್ಯಾಗಮ l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_equal(
    list_T	*l1,
    list_T	*l2,
    ಪೂರ್ಣಾಙ್ಕ		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    ಯದಿ (l1 == l2)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (list_len(l1) != list_len(l2))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (list_len(l1) == 0)
	// empty and NULL list are considered equal
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (l1 == ರಿಕ್ತ || l2 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    ಪ್ರತಿ (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != ರಿಕ್ತ && item2 != ರಿಕ್ತ;
			       item1 = item1->li_next, item2 = item2->li_next)
	ಯದಿ (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ item1 == ರಿಕ್ತ && item2 == ರಿಕ್ತ;
}

/*
 * Locate item with index "n" in list "l" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    listitem_T *
list_find(list_T *l, long n)
{
    listitem_T	*item;
    long	idx;

    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Negative index is relative to the end.
    ಯದಿ (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    ಯದಿ (n < 0 || n >= l->lv_len)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    ಯದಿ (n >= l->lv_len)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // When there is a cached index may start search from there.
    ಯದಿ (l->lv_u.mat.lv_idx_item != ರಿಕ್ತ)
    {
	ಯದಿ (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	ಅನ್ಯಥಾ ಯದಿ (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	ಅನ್ಯಥಾ
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	ಅನ್ಯಥಾ
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    ಯಾವತ್ (n > idx)
    {
	// search forward
	item = item->li_next;
	++idx;
    }
    ಯಾವತ್ (n < idx)
    {
	// search backward
	item = item->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    ಪ್ರತ್ಯಾಗಮ item;
}

/*
 * Get list item "l[idx]" as a number.
 */
    long
list_find_nr(
    list_T	*l,
    long	idx,
    ಪೂರ್ಣಾಙ್ಕ		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    ಯದಿ (l != ರಿಕ್ತ && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	ಯದಿ (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	ಯದಿ (n < 0 || n >= l->lv_len)
	{
	    ಯದಿ (errorp != ರಿಕ್ತ)
		*errorp = TRUE;
	    ಪ್ರತ್ಯಾಗಮ -1L;
	}

	ಪ್ರತ್ಯಾಗಮ l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    ಯದಿ (li == ರಿಕ್ತ)
    {
	ಯದಿ (errorp != ರಿಕ್ತ)
	    *errorp = TRUE;
	ಪ್ರತ್ಯಾಗಮ -1L;
    }
    ಪ್ರತ್ಯಾಗಮ (long)tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    ಯದಿ (li == ರಿಕ್ತ)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_index(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    ಯದಿ (li != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ li;

    ಯದಿ (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    ಪ್ರತ್ಯಾಗಮ li;
}

/*
 * Locate "item" list "l" and return its index.
 * Returns -1 when "item" is not in the list.
 */
    long
list_idx_of_item(list_T *l, listitem_T *item)
{
    long	idx = 0;
    listitem_T	*li;

    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    ಪ್ರತಿ (li = l->lv_first; li != ರಿಕ್ತ && li != item; li = li->li_next)
	++idx;
    ಯದಿ (li == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    ಶೂನ್ಯ
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    ಯದಿ (l->lv_u.mat.lv_last == ರಿಕ್ತ)
    {
	// empty list
	l->lv_first = item;
	item->li_prev = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	l->lv_u.mat.lv_last->li_next = item;
	item->li_prev = l->lv_u.mat.lv_last;
    }
    l->lv_u.mat.lv_last = item;
    ++l->lv_len;
    item->li_next = ರಿಕ್ತ;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    ಯದಿ (l->lv_type != ರಿಕ್ತ && l->lv_type->tt_member != ರಿಕ್ತ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ರಿಕ್ತ, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    li = listitem_alloc();
    ಯದಿ (li == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    ಯದಿ (li == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    ಯದಿ (li == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    ಯದಿ (li == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_append_string(list_T *l, char_u *str, ಪೂರ್ಣಾಙ್ಕ len)
{
    listitem_T *li = listitem_alloc();

    ಯದಿ (li == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    ಯದಿ (str == ರಿಕ್ತ)
	li->li_tv.vval.v_string = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_append_ಸಂಖ್ಯೆ(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    ಯದಿ (li == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    li->li_tv.v_type = VAR_ಸಂಖ್ಯೆ;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_ಸಂಖ್ಯೆ = n;
    list_append(l, li);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_ಸೇರಿಸು_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    ಯದಿ (l->lv_type != ರಿಕ್ತ && l->lv_type->tt_member != ರಿಕ್ತ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ರಿಕ್ತ, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ni = listitem_alloc();
    ಯದಿ (ni == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    copy_tv(tv, &ni->li_tv);
    list_ಸೇರಿಸು(l, ni, item);
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
list_ಸೇರಿಸು(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    ಯದಿ (item == ರಿಕ್ತ)
	// Append new item at end of list.
	list_append(l, ni);
    ಅನ್ಯಥಾ
    {
	// Insert new item before existing item.
	ni->li_prev = item->li_prev;
	ni->li_next = item;
	ಯದಿ (item->li_prev == ರಿಕ್ತ)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	ಅನ್ಯಥಾ
	{
	    item->li_prev->li_next = ni;
	    l->lv_u.mat.lv_idx_item = ರಿಕ್ತ;
	}
	item->li_prev = ni;
	++l->lv_len;
    }
}

/*
 * Append a new empty item into "l" based on the list item type.  Used when
 * adding a new element to a List in Vim9script by using the current list
 * length as the index.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
list_append_new_item(list_T *l)
{
    typval_T	tv;

    ಯದಿ (l->lv_type != ರಿಕ್ತ && l->lv_type->tt_member != ರಿಕ್ತ)
	tv.v_type = l->lv_type->tt_member->tt_type;
    ಅನ್ಯಥಾ
	tv.v_type = VAR_ಸಂಖ್ಯೆ;
    tv.v_lock = 0;

    ವಿಕಲ್ಪ (tv.v_type)
    {
	ಪಕ್ಷ VAR_BOOL: tv.vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_SPECIAL: tv.vval.v_ಸಂಖ್ಯೆ = 0; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ: tv.vval.v_ಸಂಖ್ಯೆ = 0; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FLOAT: tv.vval.v_float = 0; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_STRING: tv.vval.v_string = ರಿಕ್ತ; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BLOB: tv.vval.v_blob = blob_alloc(); ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FUNC: tv.vval.v_string = ರಿಕ್ತ; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_PARTIAL: tv.vval.v_partial = ರಿಕ್ತ; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST: tv.vval.v_list = list_alloc(); ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT: tv.vval.v_dict = dict_alloc(); ಭಙ್ಗ;
#ifdef FEAT_JOB_CHANNEL
	ಪಕ್ಷ VAR_JOB: tv.vval.v_job = ರಿಕ್ತ; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CHANNEL: tv.vval.v_channel = ರಿಕ್ತ; ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_CLASS:
	    ಯದಿ (l->lv_type != ರಿಕ್ತ && l->lv_type->tt_member != ರಿಕ್ತ)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    ಅನ್ಯಥಾ
		tv.vval.v_class = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_OBJECT:
	    ಯದಿ (l->lv_type != ರಿಕ್ತ && l->lv_type->tt_member != ರಿಕ್ತ)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    ಅನ್ಯಥಾ
		tv.vval.v_object = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TYPEALIAS: tv.vval.v_typealias = ರಿಕ್ತ; ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    tv.v_type = VAR_ಸಂಖ್ಯೆ;
	    tv.vval.v_ಸಂಖ್ಯೆ = 0;
	    ಭಙ್ಗ;
    }

    ಯದಿ (l->lv_type != ರಿಕ್ತ && l->lv_type->tt_member != ರಿಕ್ತ)
	set_tv_type(&tv, l->lv_type->tt_member);

    ಪ್ರತ್ಯಾಗಮ list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_index_one(list_T *l, long *n1, ಪೂರ್ಣಾಙ್ಕ can_append, ಪೂರ್ಣಾಙ್ಕ quiet)
{
    long	orig_n1 = *n1;
    listitem_T	*li = list_find_index(l, n1);

    ಯದಿ (li != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ li;

    // Vim9: Allow for adding an item at the end.
    ಯದಿ (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	ಯದಿ (list_append_new_item(l) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	li = list_find_index(l, n1);
    }
    ಯದಿ (li == ರಿಕ್ತ)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), orig_n1);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_range_index_two(
	list_T	    *l,
	long	    *n1,
	listitem_T  *li1,
	long	    *n2,
	ಪೂರ್ಣಾಙ್ಕ	    quiet)
{
    ಯದಿ (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	ಯದಿ (ni == ರಿಕ್ತ)
	{
	    ಯದಿ (!quiet)
		semsg(_(e_list_index_out_of_range_nr), *n2);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    ಯದಿ (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    ಯದಿ (*n2 < *n1)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), *n2);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಪೂರ್ಣಾಙ್ಕ
list_assign_range(
	list_T	    *dest,
	list_T	    *src,
	long	    idx1_arg,
	long	    idx2,
	ಪೂರ್ಣಾಙ್ಕ	    empty_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    long	idx1 = idx1_arg;
    listitem_T	*first_li = list_find_index(dest, &idx1);
    long	idx;
    type_T	*member_type = ರಿಕ್ತ;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = first_li;
    ಪ್ರತಿ (src_li = src->lv_first; src_li != ರಿಕ್ತ && dest_li != ರಿಕ್ತ; )
    {
	ಯದಿ (ಮೌಲ್ಯ_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	src_li = src_li->li_next;
	ಯದಿ (src_li == ರಿಕ್ತ || (!empty_idx2 && idx2 == idx))
	    ಭಙ್ಗ;
	dest_li = dest_li->li_next;
	++idx;
    }

    ಯದಿ (in_vim9script() && dest->lv_type != ರಿಕ್ತ
					   && dest->lv_type->tt_member != ರಿಕ್ತ)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    ಪ್ರತಿ (src_li = src->lv_first; src_li != ರಿಕ್ತ; )
    {
	ಯದಿ (op != ರಿಕ್ತ && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (member_type != ರಿಕ್ತ
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      ರಿಕ್ತ, 0) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_next;
	ಯದಿ (src_li == ರಿಕ್ತ || (!empty_idx2 && idx2 == idx))
	    ಭಙ್ಗ;
	ಯದಿ (dest_li->li_next == ರಿಕ್ತ)
	{
	    // Need to add an empty item.
	    ಯದಿ (list_append_ಸಂಖ್ಯೆ(dest, 0) == FAIL)
	    {
		src_li = ರಿಕ್ತ;
		ಭಙ್ಗ;
	    }
	}
	dest_li = dest_li->li_next;
	++idx;
    }
    ಯದಿ (src_li != ರಿಕ್ತ)
    {
	emsg(_(e_list_ಮೌಲ್ಯ_has_more_ವಸ್ತುಗಳು_than_targets));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (empty_idx2
	    ? (dest_li != ರಿಕ್ತ && dest_li->li_next != ರಿಕ್ತ)
	    : idx != idx2)
    {
	emsg(_(e_list_ಮೌಲ್ಯ_does_not_have_enough_ವಸ್ತುಗಳು));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_flatten(list_T *list, listitem_T *first, long maxitems, long maxdepth)
{
    listitem_T	*item;
    ಪೂರ್ಣಾಙ್ಕ		done = 0;

    ಯದಿ (maxdepth == 0)
	ಪ್ರತ್ಯಾಗಮ;
    CHECK_LIST_MATERIALIZE(list);
    ಯದಿ (first == ರಿಕ್ತ)
	item = list->lv_first;
    ಅನ್ಯಥಾ
	item = first;

    ಯಾವತ್ (item != ರಿಕ್ತ && done < maxitems)
    {
	listitem_T	*next = item->li_next;

	fast_breakcheck();
	ಯದಿ (got_int)
	    ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    ಯದಿ (list_extend(list, itemlist, next) == FAIL)
	    {
		list_free_item(list, item);
		ಪ್ರತ್ಯಾಗಮ;
	    }

	    ಯದಿ (maxdepth > 0)
		list_flatten(list, item->li_prev == ರಿಕ್ತ
				     ? list->lv_first : item->li_prev->li_next,
				itemlist->lv_len, maxdepth - 1);
	    clear_tv(&item->li_tv);
	    list_free_item(list, item);
	}

	++done;
	item = next;
    }
}

/*
 * "flatten()" and "flattennew()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
flatten_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ make_copy)
{
    list_T  *l;
    long    maxdepth;
    ಪೂರ್ಣಾಙ್ಕ	    error = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    ಅನ್ಯಥಾ
    {
	maxdepth = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
	ಯದಿ (error)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_ಸಂಖ್ಯೆ));
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	ಯದಿ (l == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (ಮೌಲ್ಯ_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    ಪ್ರತ್ಯಾಗಮ;
	++l->lv_refcount;
    }

    list_flatten(l, ರಿಕ್ತ, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    ಶೂನ್ಯ
f_flatten(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    ಅನ್ಯಥಾ
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    ಶೂನ್ಯ
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    ಶೂನ್ಯ
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    ಪ್ರತಿ (idx = 0, li = l->lv_first; li != ರಿಕ್ತ; li = li->li_next, ++idx)
    {
	list_T	    *l2 = list_alloc();

	ಯದಿ (l2 == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯದಿ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ಭಙ್ಗ;
	}
	ಯದಿ (list_append_ಸಂಖ್ಯೆ(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    ಭಙ್ಗ;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    ಶೂನ್ಯ
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // null string behaves like an empty string

    ಪ್ರತಿ (idx = 0; *p != NUL; ++idx)
    {
	ಪೂರ್ಣಾಙ್ಕ	    len = mb_ptr2len(p);
	list_T	    *l2;

	ಯದಿ (len == 0)
	    ಭಙ್ಗ;
	l2 = list_alloc();
	ಯದಿ (l2 == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯದಿ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ಭಙ್ಗ;
	}
	ಯದಿ (list_append_ಸಂಖ್ಯೆ(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    ಭಙ್ಗ;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*item;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    ಯದಿ (l2 == ರಿಕ್ತ || l2->lv_len == 0)
	ಪ್ರತ್ಯಾಗಮ OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == ರಿಕ್ತ ? ರಿಕ್ತ : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    ಪ್ರತಿ (item = l2->lv_first; item != ರಿಕ್ತ && --todo >= 0;
				 item = item == bef_prev ? bef : item->li_next)
	ಯದಿ (list_ಸೇರಿಸು_tv(l1, &item->li_tv, bef) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    ಪೂರ್ಣಾಙ್ಕ		len1 = l1 == ರಿಕ್ತ ? 0 : l1->lv_len;
    ಪೂರ್ಣಾಙ್ಕ		len2 = l2 == ರಿಕ್ತ ? 0 : l2->lv_len;
    long	totallen = (long)len1 + (long)len2;
    ಪೂರ್ಣಾಙ್ಕ		i;
    listitem_T	*item;

    ಯದಿ (totallen == 0)
    {
	l = list_alloc();
	ಯದಿ (l == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಯದಿ (totallen > INT_MAX)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_ವಸ್ತುಗಳು((ಪೂರ್ಣಾಙ್ಕ)totallen);
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    i = 0;
    ಯದಿ (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	ಪ್ರತಿ (item = l1->lv_first; item != ರಿಕ್ತ && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    ಯದಿ (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	ಪ್ರತಿ (item = l2->lv_first; item != ರಿಕ್ತ && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }

    ++l->lv_refcount;
    tv->v_type = VAR_LIST;
    tv->v_lock = 0;
    tv->vval.v_list = l;
    ಪ್ರತ್ಯಾಗಮ OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	ಎಣಿಕೆ;
    long	idx;

    ಯದಿ (n2 < n1)
	ಪ್ರತ್ಯಾಗಮ list_alloc();
    ಯದಿ (n1 > INT_MAX - (n2 - n1) - 1)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಎಣಿಕೆ = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_ವಸ್ತುಗಳು((ಪೂರ್ಣಾಙ್ಕ)ಎಣಿಕೆ);
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    item = list_find(ol, n1);
    ಪ್ರತಿ (idx = 0; idx < ಎಣಿಕೆ; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	ಯದಿ (item == ರಿಕ್ತ)
	{
	    list_free(l);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (ಪೂರ್ಣಾಙ್ಕ)idx, &new_tv);
	item = item->li_next;
    }
    ಪ್ರತ್ಯಾಗಮ l;
}

    ಪೂರ್ಣಾಙ್ಕ
list_slice_or_index(
	    list_T	*list,
	    ಪೂರ್ಣಾಙ್ಕ		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    ಪೂರ್ಣಾಙ್ಕ		exclusive,
	    typval_T	*rettv,
	    ಪೂರ್ಣಾಙ್ಕ		verbose)
{
    long	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ಯದಿ (n1 < 0)
	n1 = len + n1;
    ಯದಿ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	ಯದಿ (!range)
	{
	    ಯದಿ (verbose)
		semsg(_(e_list_index_out_of_range_nr), (long)n1_arg);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ಅನ್ಯಥಾ
	    n1 = len;
    }
    ಯದಿ (range)
    {
	list_T	*l;

	ಯದಿ (n2 < 0)
	    n2 = len + n2;
	ಅನ್ಯಥಾ ಯದಿ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ಯದಿ (exclusive)
	    --n2;
	ಯದಿ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	ಯದಿ (l == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    ಅನ್ಯಥಾ
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long index = n1;
	listitem_T *li = check_range_index_one(list, &index, TRUE, TRUE);
	ಯದಿ (li == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	ಯದಿ (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != ರಿಕ್ತ
					&& var1.vval.v_list->lv_type == ರಿಕ್ತ)
	{
	    ಯದಿ (list->lv_type != ರಿಕ್ತ && list->lv_type->tt_member != ರಿಕ್ತ)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, ಪೂರ್ಣಾಙ್ಕ deep, ಪೂರ್ಣಾಙ್ಕ ತುದಿ, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    list_T	*copy;
    listitem_T	*item;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;

    ಯದಿ (orig == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_ವಸ್ತುಗಳು(orig->lv_len);
    ಯದಿ (copy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (orig->lv_type == ರಿಕ್ತ || ತುದಿ || deep)
	copy->lv_type = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	copy->lv_type = alloc_type(orig->lv_type);
    ಯದಿ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    ಪ್ರತಿ (item = orig->lv_first; item != ರಿಕ್ತ && !got_int;
	    item = item->li_next)
    {
	typval_T	new_tv;

	ಯದಿ (deep)
	{
	    ಯದಿ (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    ಯದಿ (item != ರಿಕ್ತ)
    {
	list_unref(copy);
	copy = ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    ಶೂನ್ಯ
vimlist_remove(list_T *l, listitem_T *item, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    ಪ್ರತಿ (ip = item; ip != ರಿಕ್ತ; ip = ip->li_next)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	ಯದಿ (ip == item2)
	    ಭಙ್ಗ;
    }

    ಯದಿ (item2->li_next == ರಿಕ್ತ)
	l->lv_u.mat.lv_last = item->li_prev;
    ಅನ್ಯಥಾ
	item2->li_next->li_prev = item->li_prev;
    ಯದಿ (item->li_prev == ರಿಕ್ತ)
	l->lv_first = item2->li_next;
    ಅನ್ಯಥಾ
	item->li_prev->li_next = item2->li_next;
    l->lv_u.mat.lv_idx_item = ರಿಕ್ತ;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ copyID, ಪೂರ್ಣಾಙ್ಕ restore_copyID)
{
    garray_T	ga;

    ಯದಿ (tv->vval.v_list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    ಯದಿ (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    ಪ್ರತ್ಯಾಗಮ (char_u *)ga.ga_data;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    ಪೂರ್ಣಾಙ್ಕ		echo_style,
    ಪೂರ್ಣಾಙ್ಕ		restore_copyID,
    ಪೂರ್ಣಾಙ್ಕ		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    join_T	*p;
    long	sumlen = 0;
    ಪೂರ್ಣಾಙ್ಕ		first = TRUE;
    ಪೂರ್ಣಾಙ್ಕ		prev_lock;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    listitem_T	*item;
    string_T	s;
    size_t	seplen;

    // Stringify each item in the list.
    CHECK_LIST_MATERIALIZE(l);
    // Lock the list, the string() method of an object item could remove the
    // item the loop is standing on.
    prev_lock = l->lv_lock;
    ಯದಿ (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    ಪ್ರತಿ (item = l->lv_first; item != ರಿಕ್ತ && !got_int; item = item->li_next)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ಯದಿ (s.string == ರಿಕ್ತ)
	{
	    l->lv_lock = prev_lock;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	s.length = STRLEN(s.string);
	sumlen += (long)s.length;

	(ಶೂನ್ಯ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	ಯದಿ (tofree != ರಿಕ್ತ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	ಅನ್ಯಥಾ
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	ಸಾಲು_breakcheck();
	ಯದಿ (did_echo_string_emsg)  // recursion error, bail out
	    ಭಙ್ಗ;
    }
    l->lv_lock = prev_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    ಯದಿ (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    ಯದಿ (sumlen > INT_MAX - 2 || ga_grow(gap, (ಪೂರ್ಣಾಙ್ಕ)sumlen + 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತಿ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ಯದಿ (first)
	    first = FALSE;
	ಅನ್ಯಥಾ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	ಯದಿ (p->s.string != ರಿಕ್ತ)
	    ga_concat_len(gap, p->s.string, p->s.length);
	ಸಾಲು_breakcheck();
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    ಪೂರ್ಣಾಙ್ಕ		echo_style,
    ಪೂರ್ಣಾಙ್ಕ		restore_copyID,
    ಪೂರ್ಣಾಙ್ಕ		copyID)
{
    garray_T	join_ga;
    ಪೂರ್ಣಾಙ್ಕ		retval;
    join_T	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (l->lv_len < 1)
	ಪ್ರತ್ಯಾಗಮ OK; // nothing to do
    ga_init2(&join_ga, ಪರಿಮಾಣ(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ಯದಿ (join_ga.ga_data == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    ಪ್ರತಿ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * "join()" function
 */
    ಶೂನ್ಯ
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    ಯದಿ (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == ರಿಕ್ತ)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    ಅನ್ಯಥಾ
	sep = tv_get_string_chk(&argvars[1]);

    ಯದಿ (sep != ರಿಕ್ತ)
    {
	ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 80);
	ಯದಿ (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	ಅನ್ಯಥಾ
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
    ಅನ್ಯಥಾ
	rettv->vval.v_string = ರಿಕ್ತ;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಙ್ಕ do_error)
{
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg == ರಿಕ್ತ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = ರಿಕ್ತ;
    typval_T	tv;
    listitem_T	*item;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    ಪೂರ್ಣಾಙ್ಕ		had_comma;

    ಯದಿ (evaluate)
    {
	l = list_alloc();
	ಯದಿ (l == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ಯಾವತ್ (**arg != ']' && **arg != NUL)
    {
	ಯದಿ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    ಗಚ್ಛ failret;
	ಯದಿ (check_typval_is_ಮೌಲ್ಯ(&tv) == FAIL)
	{
	    ಯದಿ (evaluate)
		clear_tv(&tv);
	    ಗಚ್ಛ failret;
	}
	ಯದಿ (evaluate)
	{
	    item = listitem_alloc();
	    ಯದಿ (item != ರಿಕ್ತ)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    ಅನ್ಯಥಾ
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	ಯದಿ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ಯದಿ (had_comma)
	{
	    ಯದಿ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ಗಚ್ಛ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಯದಿ (**arg == ']')
	    ಭಙ್ಗ;

	ಯದಿ (!had_comma)
	{
	    ಯದಿ (do_error)
	    {
		ಯದಿ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ಅನ್ಯಥಾ
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    ಗಚ್ಛ failret;
	}
    }

    ಯದಿ (**arg != ']')
    {
	ಯದಿ (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	ಯದಿ (evaluate)
	    list_free(l);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    *arg += 1;
    ಯದಿ (evaluate)
	rettv_list_set(rettv, l);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    ಪೂರ್ಣಾಙ್ಕ
write_list(FILE *fd, list_T *list, ಪೂರ್ಣಾಙ್ಕ ದ್ವಿಮಾನ)
{
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
    {
	ಪ್ರತಿ (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    ಯದಿ (*s == '\n')
		c = putc(NUL, fd);
	    ಅನ್ಯಥಾ
		c = putc(*s, fd);
	    ಯದಿ (c == EOF)
	    {
		ret = FAIL;
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (!ದ್ವಿಮಾನ || li->li_next != ರಿಕ್ತ)
	    ಯದಿ (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		ಭಙ್ಗ;
	    }
	ಯದಿ (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    ಭಙ್ಗ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Initialize a static list with 10 items.
 */
    ಶೂನ್ಯ
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    ಪೂರ್ಣಾಙ್ಕ	    i;

    CLEAR_POINTER(sl);
    l->lv_first = &sl->sl_ವಸ್ತುಗಳು[0];
    l->lv_u.mat.lv_last = &sl->sl_ವಸ್ತುಗಳು[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    ಪ್ರತಿ (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_ವಸ್ತುಗಳು[i];

	ಯದಿ (i == 0)
	    li->li_prev = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    li->li_prev = li - 1;
	ಯದಿ (i == 9)
	    li->li_next = ರಿಕ್ತ;
	ಅನ್ಯಥಾ
	    li->li_next = li + 1;
    }
}

/*
 * "list2str()" function
 */
    ಶೂನ್ಯ
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    l = argvars[0].vval.v_list;
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // empty list results in empty string

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[1], ರಿಕ್ತ);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    ಯದಿ (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	ಪೂರ್ಣಾಙ್ಕ	(*char2bytes)(ಪೂರ್ಣಾಙ್ಕ, char_u *);

	ಯದಿ (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	ಅನ್ಯಥಾ
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_ಸಂಖ್ಯೆ(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    ಅನ್ಯಥಾ
    {
	long len = (long)list_len(l) + 1;

	ಯದಿ (len <= INT_MAX && ga_grow(&ga, (ಪೂರ್ಣಾಙ್ಕ)len) == OK)
	{
	    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
		ga_append(&ga, tv_get_ಸಂಖ್ಯೆ(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_data;
}

/*
 * "list2tuple()" function
 */
    ಶೂನ್ಯ
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ರಿಕ್ತ;

    ಯದಿ (check_for_list_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    l = argvars[0].vval.v_list;
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    ಯದಿ (rettv_tuple_set_with_ವಸ್ತುಗಳು(rettv, list_len(l)) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ITEM(tuple, TUPLE_LEN(tuple)));
	tuple->tv_ವಸ್ತುಗಳು.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    ಶೂನ್ಯ
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_tuple_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    tuple = argvars[0].vval.v_tuple;
    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ITEM(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*item, *item2;
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    long	idx;
    long	end;
    ಪೂರ್ಣಾಙ್ಕ		cnt = 0;
    list_T	*rl;

    ಯದಿ ((l = argvars[0].vval.v_list) == ರಿಕ್ತ
			     || ಮೌಲ್ಯ_check_lock(l->lv_lock, arg_errmsg, TRUE))
	ಪ್ರತ್ಯಾಗಮ;

    idx = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;		// type error: do nothing, errmsg already given

    ಯದಿ ((item = list_find(l, idx)) == ರಿಕ್ತ)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;		// type error: do nothing

    ಯದಿ ((item2 = list_find(l, end)) == ರಿಕ್ತ)
    {
	semsg(_(e_list_index_out_of_range_nr), end);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (li = item; li != ರಿಕ್ತ; li = li->li_next)
    {
	++cnt;
	ಯದಿ (li == item2)
	    ಭಙ್ಗ;
    }
    ಯದಿ (li == ರಿಕ್ತ)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	ಪ್ರತ್ಯಾಗಮ;
    }

    vimlist_remove(l, item, item2);
    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rl = rettv->vval.v_list;

    ಯದಿ (l->lv_with_ವಸ್ತುಗಳು > 0)
    {
	// need to copy the list items and move the value
	ಯಾವತ್ (item != ರಿಕ್ತ)
	{
	    li = listitem_alloc();
	    ಯದಿ (li == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    ಯದಿ (item == item2)
		ಭಙ್ಗ;
	    item = item->li_next;
	}
    }
    ಅನ್ಯಥಾ
    {
	rl->lv_first = item;
	rl->lv_u.mat.lv_last = item2;
	item->li_prev = ರಿಕ್ತ;
	item2->li_next = ರಿಕ್ತ;
	rl->lv_len = cnt;
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ item_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ item_compare2(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2);

// struct used in the array that's given to qsort()
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    listitem_T	*item;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    ಸಙ್ಘ {
	varnumber_T	inum;	// for "N"
	ದ್ವಿಗುಣ		fnum;	// for "n" and "f"
    } ಕೀಲಿ;
} sortItem_T;

// struct storing information about current sort
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ		item_compare_ic;
    ಪೂರ್ಣಾಙ್ಕ		item_compare_lc;
    ಪೂರ್ಣಾಙ್ಕ		item_compare_numeric;
    ಪೂರ್ಣಾಙ್ಕ		item_compare_numbers;
    ಪೂರ್ಣಾಙ್ಕ		item_compare_float;
    char_u	*item_compare_func;
    partial_T	*item_compare_partial;
    dict_T	*item_compare_selfdict;
    ಪೂರ್ಣಾಙ್ಕ		item_compare_func_err;
    ಪೂರ್ಣಾಙ್ಕ		item_compare_keep_zero;
    ಪೂರ್ಣಾಙ್ಕ		item_compare_ಕೀಲಿಗಳು_ready;  // ptrs[].key is precomputed
} sortinfo_T;
ಸ್ಥಿರ sortinfo_T	*sortinfo = ರಿಕ್ತ;
#define ITEM_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
item_compare(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    sortItem_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = ರಿಕ್ತ, *tofree2 = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;
    tv1 = &si1->item->li_tv;
    tv2 = &si2->item->li_tv;

    ಯದಿ (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_ಕೀಲಿಗಳು_ready
					? si1->ಕೀಲಿ.inum : tv_to_ಸಂಖ್ಯೆ(tv1);
	varnumber_T	v2 = sortinfo->item_compare_ಕೀಲಿಗಳು_ready
					? si2->ಕೀಲಿ.inum : tv_to_ಸಂಖ್ಯೆ(tv2);

	ಪ್ರತ್ಯಾಗಮ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ಯದಿ (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_ಕೀಲಿಗಳು_ready
					? si1->ಕೀಲಿ.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_ಕೀಲಿಗಳು_ready
					? si2->ಕೀಲಿ.fnum : tv_get_float(tv2);

	ಪ್ರತ್ಯಾಗಮ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ಯದಿ (sortinfo->item_compare_numeric && sortinfo->item_compare_ಕೀಲಿಗಳು_ready)
    {
	ದ್ವಿಗುಣ	n1 = si1->ಕೀಲಿ.fnum;
	ದ್ವಿಗುಣ	n2 = si2->ಕೀಲಿ.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	ಯದಿ (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	ಪ್ರತ್ಯಾಗಮ res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    ಯದಿ (tv1->v_type == VAR_STRING)
    {
	ಯದಿ (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	ಅನ್ಯಥಾ
	    p1 = tv1->vval.v_string;
    }
    ಅನ್ಯಥಾ
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    ಯದಿ (tv2->v_type == VAR_STRING)
    {
	ಯದಿ (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	ಅನ್ಯಥಾ
	    p2 = tv2->vval.v_string;
    }
    ಅನ್ಯಥಾ
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    ಯದಿ (p1 == ರಿಕ್ತ)
	p1 = (char_u *)"";
    ಯದಿ (p2 == ರಿಕ್ತ)
	p2 = (char_u *)"";
    ಯದಿ (!sortinfo->item_compare_numeric)
    {
	ಯದಿ (sortinfo->item_compare_lc)
	    res = strcoll((ವರ್ಣ *)p1, (ವರ್ಣ *)p2);
	ಅನ್ಯಥಾ
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    ಅನ್ಯಥಾ
    {
	ದ್ವಿಗುಣ n1, n2;
	n1 = strtod((ವರ್ಣ *)p1, (ವರ್ಣ **)&p1);
	n2 = strtod((ವರ್ಣ *)p2, (ವರ್ಣ **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    ಯದಿ (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    ಪ್ರತ್ಯಾಗಮ res;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
item_compare2(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2)
{
    sortItem_T  *si1, *si2;
    ಪೂರ್ಣಾಙ್ಕ		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->item_compare_partial;
    funcexe_T	funcexe;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    ಯದಿ (sortinfo->item_compare_func_err)
	ಪ್ರತ್ಯಾಗಮ 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    ಯದಿ (partial == ರಿಕ್ತ)
	func_name = sortinfo->item_compare_func;
    ಅನ್ಯಥಾ
	func_name = partial_name(partial);

    // Copy the values.  This is needed to be able to set v_lock to VAR_FIXED
    // in the copy without changing the original list items.
    copy_tv(&si1->item->li_tv, &argv[0]);
    copy_tv(&si2->item->li_tv, &argv[1]);

    rettv.v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = partial;
    funcexe.fe_selfdict = sortinfo->item_compare_selfdict;
    res = call_func(func_name, -1, &rettv, 2, argv, &funcexe);
    clear_tv(&argv[0]);
    clear_tv(&argv[1]);

    ಯದಿ (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    ಅನ್ಯಥಾ
    {
	varnumber_T	n;

	n = tv_get_ಸಂಖ್ಯೆ_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    ಯದಿ (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    ಯದಿ (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sort_compute_ಕೀಲಿಗಳು(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    ಯದಿ (info->item_compare_numbers)
    {
	ಪ್ರತಿ (i = 0; i < len; ++i)
	    ptrs[i].ಕೀಲಿ.inum = tv_to_ಸಂಖ್ಯೆ(&ptrs[i].item->li_tv);
    }
    ಅನ್ಯಥಾ ಯದಿ (info->item_compare_float)
    {
	ಪ್ರತಿ (i = 0; i < len; ++i)
	    ptrs[i].ಕೀಲಿ.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    ಅನ್ಯಥಾ // info->item_compare_numeric
    {
	ಪ್ರತಿ (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    ಯದಿ (tv->v_type == VAR_STRING)
		ptrs[i].ಕೀಲಿ.fnum = 0.0;
	    ಅನ್ಯಥಾ
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = ರಿಕ್ತ;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].ಕೀಲಿ.fnum = p == ರಿಕ್ತ ? 0.0 : strtod((ವರ್ಣ *)p, ರಿಕ್ತ);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_ಕೀಲಿಗಳು_ready = TRUE;
}

/*
 * sort() List "l"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_sort(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    ಯದಿ (ptrs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ptrs[i].item = li;
	ptrs[i].idx = i;
	++i;
    }

    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = FALSE;
    info->item_compare_ಕೀಲಿಗಳು_ready = FALSE;

    ಯದಿ (info->item_compare_func == ರಿಕ್ತ && info->item_compare_partial == ರಿಕ್ತ
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_ಕೀಲಿಗಳು(ptrs, len, info);
    // test the compare function
    ಯದಿ ((info->item_compare_func != ರಿಕ್ತ
		|| info->item_compare_partial != ರಿಕ್ತ)
	    && item_compare2((ಶೂನ್ಯ *)&ptrs[0], (ಶೂನ್ಯ *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    ಅನ್ಯಥಾ
    {
	// Sort the array with item pointers.
	qsort((ಶೂನ್ಯ *)ptrs, (size_t)len, ಪರಿಮಾಣ(sortItem_T),
		info->item_compare_func == ರಿಕ್ತ
		&& info->item_compare_partial == ರಿಕ್ತ
		? item_compare : item_compare2);

	ಯದಿ (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = ರಿಕ್ತ;
	    l->lv_len = 0;
	    ಪ್ರತಿ (i = 0; i < len; ++i)
		list_append(l, ptrs[i].item);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_uniq(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ	(*item_compare_func_ptr)(ಧ್ರುವ ಶೂನ್ಯ *, ಧ್ರುವ ಶೂನ್ಯ *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    ಯದಿ (ptrs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != ರಿಕ್ತ
	|| info->item_compare_partial != ರಿಕ್ತ
	? item_compare2 : item_compare;

    ಪ್ರತಿ (li = l->lv_first; li != ರಿಕ್ತ && li->li_next != ರಿಕ್ತ;
	    li = li->li_next)
    {
	ಯದಿ (item_compare_func_ptr((ಶೂನ್ಯ *)&li, (ಶೂನ್ಯ *)&li->li_next)
		== 0)
	    ptrs[i++].item = li;
	ಯದಿ (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    ಭಙ್ಗ;
	}
    }

    ಯದಿ (!info->item_compare_func_err)
    {
	ಯಾವತ್ (--i >= 0)
	{
	    li = ptrs[i].item->li_next;
	    ptrs[i].item->li_next = li->li_next;
	    ಯದಿ (li->li_next != ರಿಕ್ತ)
		li->li_next->li_prev = ptrs[i].item;
	    ಅನ್ಯಥಾ
		l->lv_u.mat.lv_last = ptrs[i].item;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
parse_sort_uniq_args(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = ರಿಕ್ತ;
    info->item_compare_partial = ರಿಕ್ತ;
    info->item_compare_selfdict = ರಿಕ್ತ;
    info->item_compare_ಕೀಲಿಗಳು_ready = FALSE;

    ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ OK;

    // optional second argument: {func}
    ಯದಿ (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	    error = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	    nr = 0;

	ಯದಿ (argvars[1].v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    nr = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (nr == 1)
		info->item_compare_ic = TRUE;
	}
	ಯದಿ (nr != 1)
	{
	    ಯದಿ (argvars[1].v_type != VAR_ಸಂಖ್ಯೆ)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    ಅನ್ಯಥಾ ಯದಿ (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಯದಿ (info->item_compare_func != ರಿಕ್ತ)
	{
	    ಯದಿ (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = ರಿಕ್ತ;
		info->item_compare_numeric = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = ರಿಕ್ತ;
		info->item_compare_numbers = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = ರಿಕ್ತ;
		info->item_compare_float = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = ರಿಕ್ತ;
		info->item_compare_ic = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = ರಿಕ್ತ;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	ಯದಿ (check_for_dict_arg(argvars, 2) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * "sort()" or "uniq()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_sort_uniq(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ sort)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    long	len;

    ಯದಿ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    ಯದಿ (l != ರಿಕ್ತ && ಮೌಲ್ಯ_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	ಗಚ್ಛ theend;
    rettv_list_set(rettv, l);
    ಯದಿ (l == ರಿಕ್ತ)
	ಗಚ್ಛ theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    ಯದಿ (len <= 1)
	ಗಚ್ಛ theend;	// short list sorts pretty quickly

    ಯದಿ (parse_sort_uniq_args(argvars, &info) == FAIL)
	ಗಚ್ಛ theend;

    ಯದಿ (sort)
	do_sort(l, &info);
    ಅನ್ಯಥಾ
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    ಶೂನ್ಯ
f_sort(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    ಶೂನ್ಯ
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    ಪೂರ್ಣಾಙ್ಕ
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	ಪೂರ್ಣಾಙ್ಕ		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    ಯದಿ (filtermap == FILTERMAP_FOREACH && ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(ಅಭಿವ್ಯಕ್ತಿ->vval.v_string);
	ಯದಿ (!did_emsg)
	    retval = OK;
	ಗಚ್ಛ theend;
    }

    argv[0] = *get_vim_var_tv(VV_ಕೀಲಿ);
    argv[1] = *get_vim_var_tv(VV_VAL);
    ಯದಿ (eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, FALSE, argv, 2, fc, newtv) == FAIL)
	ಗಚ್ಛ theend;
    ಯದಿ (filtermap == FILTERMAP_FILTER)
    {
	ಪೂರ್ಣಾಙ್ಕ	    error = FALSE;

	// filter(): when expr is zero remove the item
	ಯದಿ (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	ಅನ್ಯಥಾ
	    *remp = (tv_get_ಸಂಖ್ಯೆ_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	ಯದಿ (error)
	    ಗಚ್ಛ theend;
    }
    ಅನ್ಯಥಾ ಯದಿ (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	ವರ್ಣ		*func_name,
	char_u		*arg_errmsg,
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
	typval_T	*rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		prev_lock;
    list_T	*l_ret = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    ಯದಿ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = ರಿಕ್ತ;
    }
    ಯದಿ (l == ರಿಕ್ತ || (filtermap == FILTERMAP_FILTER
			    && ಮೌಲ್ಯ_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	ಪ್ರತ್ಯಾಗಮ;

    prev_lock = l->lv_lock;

    ಯದಿ (filtermap == FILTERMAP_MAPNEW)
    {
	ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    ಯದಿ (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, &newtv);

    ಯದಿ (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	ಪೂರ್ಣಾಙ್ಕ		len = l->lv_len;
	ಪೂರ್ಣಾಙ್ಕ		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	ಯದಿ (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = ರಿಕ್ತ;
	    l->lv_u.mat.lv_last = ರಿಕ್ತ;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = ರಿಕ್ತ;
	}

	ಪ್ರತಿ (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_ಸಂಖ್ಯೆ;
	    tv.v_lock = 0;
	    tv.vval.v_ಸಂಖ್ಯೆ = val;
	    set_vim_var_nr(VV_ಕೀಲಿ, idx);
	    ಯದಿ (filter_map_one(&tv, ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc, &newtv, &rem) == FAIL)
		ಭಙ್ಗ;
	    ಯದಿ (did_emsg)
	    {
		clear_tv(&newtv);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (filtermap != FILTERMAP_FOREACH)
	    {
		ಯದಿ (filtermap != FILTERMAP_FILTER)
		{
		    ಯದಿ (filtermap == FILTERMAP_MAP && argtype != ರಿಕ್ತ
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			ಭಙ್ಗ;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    ಯದಿ (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ ಯದಿ (!rem)
		{
		    // filter(): append the list item value when not rem
		    ಯದಿ (list_append_tv_move(l, &tv) == FAIL)
			ಭಙ್ಗ;
		}
	    }

	    val += stride;
	}
    }
    ಅನ್ಯಥಾ
    {
	// Materialized list: loop over the items
	ಪ್ರತಿ (li = l->lv_first; li != ರಿಕ್ತ; li = nli)
	{
	    ಯದಿ (filtermap == FILTERMAP_MAP && ಮೌಲ್ಯ_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		ಭಙ್ಗ;
	    nli = li->li_next;
	    set_vim_var_nr(VV_ಕೀಲಿ, idx);
	    ಯದಿ (filter_map_one(&li->li_tv, ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc,
							 &newtv, &rem) == FAIL)
		ಭಙ್ಗ;
	    ಯದಿ (did_emsg)
	    {
		clear_tv(&newtv);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (filtermap == FILTERMAP_MAP)
	    {
		ಯದಿ (argtype != ರಿಕ್ತ && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ಭಙ್ಗ;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_lock = 0;
		li->li_tv = newtv;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		ಯದಿ (list_append_tv_move(l_ret, &newtv) == FAIL)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = prev_lock;
    ಯದಿ (fc != ರಿಕ್ತ)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ;
    ವರ್ಣ	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    ಪೂರ್ಣಾಙ್ಕ		save_did_emsg;
    type_T	*type = ರಿಕ್ತ;

    // map(), filter(), foreach() return the first argument, also on failure.
    ಯದಿ (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    ಯದಿ (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	ಯದಿ (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != ರಿಕ್ತ)
	    type = argvars[0].vval.v_dict->dv_type;
	ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != ರಿಕ್ತ)
	    type = argvars[0].vval.v_list->lv_type;
    }

    ಯದಿ (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	ವರ್ಣ *ಸಂದೇಶ;

	ಯದಿ (filtermap == FILTERMAP_FOREACH)
	    ಸಂದೇಶ = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(ಸಂದೇಶ), func_name);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    ಅಭಿವ್ಯಕ್ತಿ = &argvars[1];
    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ;

    typval_T	save_val;
    typval_T	save_ಕೀಲಿ;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_ಕೀಲಿ, &save_ಕೀಲಿ);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ಯದಿ (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, ಅಭಿವ್ಯಕ್ತಿ, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, ಅಭಿವ್ಯಕ್ತಿ,
							    arg_errmsg, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, ಅಭಿವ್ಯಕ್ತಿ, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, ಅಭಿವ್ಯಕ್ತಿ);
    ಅನ್ಯಥಾ // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, ಅಭಿವ್ಯಕ್ತಿ, rettv);

    restore_vimvar(VV_ಕೀಲಿ, &save_ಕೀಲಿ);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    ಶೂನ್ಯ
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    ಶೂನ್ಯ
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    ಶೂನ್ಯ
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    ಶೂನ್ಯ
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_add(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    ಯದಿ (l == ರಿಕ್ತ)
    {
	ಯದಿ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    ಅನ್ಯಥಾ ಯದಿ (!ಮೌಲ್ಯ_check_lock(l->lv_lock,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    ಶೂನ್ಯ
f_add(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = 1; // Default: Failed

    ಯದಿ (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    ಅನ್ಯಥಾ
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ಸ್ಥಿರ long
list_ಎಣಿಕೆ(list_T *l, typval_T *needle, long idx, ಪೂರ್ಣಾಙ್ಕ ic)
{
    long	n = 0;
    listitem_T	*li;

    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    CHECK_LIST_MATERIALIZE(l);

    ಯದಿ (list_len(l) == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    li = list_find(l, idx);
    ಯದಿ (li == ರಿಕ್ತ)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪ್ರತಿ ( ; li != ರಿಕ್ತ; li = li->li_next)
	ಯದಿ (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * "count()" function
 */
    ಶೂನ್ಯ
f_ಎಣಿಕೆ(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    ಪೂರ್ಣಾಙ್ಕ		ic = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	ic = (ಪೂರ್ಣಾಙ್ಕ)tv_get_bool_chk(&argvars[2], &error);

    ಯದಿ (!error && argvars[0].v_type == VAR_STRING)
	n = string_ಎಣಿಕೆ(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    ಅನ್ಯಥಾ ಯದಿ (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], &error);
	ಯದಿ (!error)
	    n = list_ಎಣಿಕೆ(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    ಅನ್ಯಥಾ ಯದಿ (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], &error);
	ಯದಿ (!error)
	    n = tuple_ಎಣಿಕೆ(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    ಅನ್ಯಥಾ ಯದಿ (!error && argvars[0].v_type == VAR_DICT)
    {
	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	ಅನ್ಯಥಾ
	    n = dict_ಎಣಿಕೆ(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    ಅನ್ಯಥಾ ಯದಿ (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	ವರ್ಣ		*func_name,
	char_u		*arg_errmsg,
	ಪೂರ್ಣಾಙ್ಕ		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*item;
    long	before;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    l1 = argvars[0].vval.v_list;
    ಯದಿ (l1 == ರಿಕ್ತ)
    {
	emsg(_(e_cannot_extend_null_list));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (is_new || !ಮೌಲ್ಯ_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	ಯದಿ (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    ಯದಿ (l1 == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	}

	l2 = argvars[1].vval.v_list;
	ಯದಿ (l2 == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	    ಯದಿ (error)
		ಗಚ್ಛ cleanup;		// type error; errmsg already given
	    ಯದಿ (before == l1->lv_len)
		item = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		item = list_find(l1, before);
		ಯದಿ (item == ರಿಕ್ತ)
		{
		    semsg(_(e_list_index_out_of_range_nr), before);
		    ಗಚ್ಛ cleanup;
		}
	    }
	}
	ಅನ್ಯಥಾ
	    item = ರಿಕ್ತ;
	ಯದಿ (type != ರಿಕ್ತ && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    ಗಚ್ಛ cleanup;
	list_extend(l1, l2, item);

theend:
	ಯದಿ (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	ಅನ್ಯಥಾ
	    copy_tv(&argvars[0], rettv);
	ಪ್ರತ್ಯಾಗಮ;

cleanup:
	ಯದಿ (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, ಪೂರ್ಣಾಙ್ಕ is_new)
{
    type_T	*type = ರಿಕ್ತ;
    ವರ್ಣ	*func_name = is_new ? "extendnew()" : "extend()";

    ಯದಿ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	ಯದಿ (!is_new && in_vim9script() && argvars[0].vval.v_list != ರಿಕ್ತ)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	ಯದಿ (!is_new && in_vim9script() && argvars[0].vval.v_dict != ರಿಕ್ತ)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    ಅನ್ಯಥಾ
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    ಶೂನ್ಯ
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
    ಶೂನ್ಯ
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_ಸೇರಿಸು_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

    ಯದಿ (l == ರಿಕ್ತ)
    {
	ಯದಿ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (ಮೌಲ್ಯ_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
    ಯದಿ (error)
	ಪ್ರತ್ಯಾಗಮ;		// type error; errmsg already given

    ಯದಿ (before == l->lv_len)
	item = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
    {
	item = list_find(l, before);
	ಯದಿ (item == ರಿಕ್ತ)
	{
	    semsg(_(e_list_index_out_of_range_nr), before);
	    l = ರಿಕ್ತ;
	}
    }
    ಯದಿ (l != ರಿಕ್ತ)
    {
	(ಶೂನ್ಯ)list_ಸೇರಿಸು_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    ಶೂನ್ಯ
f_ಸೇರಿಸು(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_BLOB)
	blob_ಸೇರಿಸು_func(argvars, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    ಅನ್ಯಥಾ
	list_ಸೇರಿಸು_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    ಶೂನ್ಯ
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    ಯದಿ (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
			|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    ಅನ್ಯಥಾ
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    ಯದಿ (l != ರಿಕ್ತ
	    && !ಮೌಲ್ಯ_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	ಯದಿ (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = ರಿಕ್ತ;
	l->lv_len = 0;
	ಯಾವತ್ (li != ರಿಕ್ತ)
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
    ಶೂನ್ಯ
f_reverse(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	ಯದಿ (argvars[0].vval.v_string != ರಿಕ್ತ)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	ಅನ್ಯಥಾ
	    rettv->vval.v_string = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_TUPLE)
	tuple_reverse(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_reduce(
	typval_T	*argvars,
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		range_list;
    ಪೂರ್ಣಾಙ್ಕ		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    ಪೂರ್ಣಾಙ್ಕ		r;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_start = called_emsg;
    ಪೂರ್ಣಾಙ್ಕ		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != ರಿಕ್ತ && l->lv_first == &range_list_item;
    ಯದಿ (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    ಯದಿ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ಯದಿ (l == ರಿಕ್ತ || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_ಮೌಲ್ಯ), "List");
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (range_list)
	{
	    initial.v_type = VAR_ಸಂಖ್ಯೆ;
	    initial.vval.v_ಸಂಖ್ಯೆ = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	ಅನ್ಯಥಾ
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_next;
	}
    }
    ಅನ್ಯಥಾ
    {
	initial = argvars[2];
	ಯದಿ (l != ರಿಕ್ತ && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, rettv);

    prev_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    ಯಾವತ್ (range_list ? range_idx < l->lv_len : li != ರಿಕ್ತ)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	ಯದಿ (range_list)
	{
	    argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
	    argv[1].vval.v_ಸಂಖ್ಯೆ = range_val;
	}
	ಅನ್ಯಥಾ
	    argv[1] = li->li_tv;

	r = eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, TRUE, argv, 2, fc, rettv);

	ಯದಿ (argv[0].v_type != VAR_ಸಂಖ್ಯೆ && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	ಯದಿ (r == FAIL || called_emsg != called_emsg_start)
	    ಭಙ್ಗ;

	// advance to the next item
	ಯದಿ (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	ಅನ್ಯಥಾ
	    li = li->li_next;
    }

    ಯದಿ (fc != ರಿಕ್ತ)
	remove_funccal();

    l->lv_lock = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    ಶೂನ್ಯ
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    ಯದಿ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    ಅನ್ಯಥಾ ಯದಿ (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    ಅನ್ಯಥಾ
	func_name = tv_get_string_strict(&argvars[1]);
    ಯದಿ (func_name == ರಿಕ್ತ || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    ಅನ್ಯಥಾ
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    ಶೂನ್ಯ
f_slice(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_can_index(&argvars[0], TRUE, FALSE) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    copy_tv(argvars, rettv);
    eval_index_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? ರಿಕ್ತ : argvars + 2,
	    TRUE, ರಿಕ್ತ, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
