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

#include "vim.h"

#ᰀᰧ defined(FEAT_EVAL)

// List heads for garbage collection.
static list_T		*first_list = NULL;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    for ((lw) = (l)->lv_watch; (lw) != NULL; (lw) = (lw)->lw_next)

static void list_free_item(list_T *l, listitem_T *item);

/*
 * Add a watcher to a list.
 */
    void
list_add_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_next = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    void
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	ᰀᰧ (lw == lwrem)
	{
	    *lwp = lw->lw_next;
	    bha_nga;
	}
	lwp = &lw->lw_next;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    static void
list_fix_watch(list_T *l, listitem_T *item)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	ᰀᰧ (lw->lw_item == item)
	    lw->lw_item = item->li_next;
}

    static void
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    ᰀᰧ (first_list != NULL)
	first_list->lv_used_prev = l;
    l->lv_used_prev = NULL;
    l->lv_used_next = first_list;
    first_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(void)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    ᰀᰧ (l != NULL)
	list_init(l);
    ᰀᰧᰌ l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ᰀᰧ (alloc_fail_id == id && alloc_does_fail(parimaa_na(list_T)))
	ᰀᰧᰌ NULL;
#endif
    ᰀᰧᰌ (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_items(puur_naa_nka count)
{
    list_T	*l;

    ᰀᰧ (count > 0
	    && (size_t)count > (SIZE_MAX - parimaa_na(list_T)) / parimaa_na(listitem_T))
	ᰀᰧᰌ NULL;
    l = (list_T *)alloc_clear(parimaa_na(list_T) + count * parimaa_na(listitem_T));
    ᰀᰧ (l == NULL)
	ᰀᰧᰌ NULL;

    list_init(l);

    ᰀᰧ (count <= 0)
	ᰀᰧᰌ l;

    listitem_T	*li = (listitem_T *)(l + 1);
    puur_naa_nka		i;

    l->lv_len = count;
    l->lv_with_items = count;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + count - 1;
    for (i = 0; i < count; ++i)
    {
	ᰀᰧ (i == 0)
	    li->li_prev = NULL;
	ᰀᰧᰀ
	    li->li_prev = li - 1;
	ᰀᰧ (i == count - 1)
	    li->li_next = NULL;
	ᰀᰧᰀ
	    li->li_next = li + 1;
	++li;
    }

    ᰀᰧᰌ l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    void
list_set_item(list_T *l, puur_naa_nka idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    puur_naa_nka
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    ᰀᰧ (l == NULL)
	ᰀᰧᰌ FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    ᰀᰧᰌ OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    puur_naa_nka
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ᰀᰧ (alloc_fail_id == id && alloc_does_fail(parimaa_na(list_T)))
	ᰀᰧᰌ FAIL;
#endif
    ᰀᰧᰌ rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    void
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ᰀᰧ (l != NULL)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    void
list_unref(list_T *l)
{
    ᰀᰧ (l != NULL && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    static void
list_free_contents(list_T *l)
{
    listitem_T *item;

    ᰀᰧ (l->lv_first != &range_list_item)
	for (item = l->lv_first; item != NULL; item = l->lv_first)
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
    puur_naa_nka
list_free_nonref(puur_naa_nka copyID)
{
    list_T	*ll;
    puur_naa_nka		did_free = FALSE;

    for (ll = first_list; ll != NULL; ll = ll->lv_used_next)
	ᰀᰧ ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == NULL)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    ᰀᰧᰌ did_free;
}

    static void
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    ᰀᰧ (l->lv_used_prev == NULL)
	first_list = l->lv_used_next;
    ᰀᰧᰀ
	l->lv_used_prev->lv_used_next = l->lv_used_next;
    ᰀᰧ (l->lv_used_next != NULL)
	l->lv_used_next->lv_used_prev = l->lv_used_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    void
list_free_items(puur_naa_nka copyID)
{
    list_T	*ll, *ll_next;

    for (ll = first_list; ll != NULL; ll = ll_next)
    {
	ll_next = ll->lv_used_next;
	ᰀᰧ ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == NULL)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    void
list_free(list_T *l)
{
    ᰀᰧ (in_free_unref_items)
	ᰀᰧᰌ;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(void)
{
    ᰀᰧᰌ ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    static void
list_free_item(list_T *l, listitem_T *item)
{
    ᰀᰧ (l->lv_with_items == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_items)
	vim_free(item);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    void
listitem_free(list_T *l, listitem_T *item)
{
    clear_tv(&item->li_tv);
    list_free_item(l, item);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    void
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
    ᰀᰧ (l == NULL)
	ᰀᰧᰌ 0L;
    ᰀᰧᰌ l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    puur_naa_nka
list_equal(
    list_T	*l1,
    list_T	*l2,
    puur_naa_nka		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    ᰀᰧ (l1 == l2)
	ᰀᰧᰌ TRUE;
    ᰀᰧ (list_len(l1) != list_len(l2))
	ᰀᰧᰌ FALSE;
    ᰀᰧ (list_len(l1) == 0)
	// empty and NULL list are considered equal
	ᰀᰧᰌ TRUE;
    ᰀᰧ (l1 == NULL || l2 == NULL)
	ᰀᰧᰌ FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    for (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != NULL && item2 != NULL;
			       item1 = item1->li_next, item2 = item2->li_next)
	ᰀᰧ (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    ᰀᰧᰌ FALSE;
    ᰀᰧᰌ item1 == NULL && item2 == NULL;
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

    ᰀᰧ (l == NULL)
	ᰀᰧᰌ NULL;

    // Negative index is relative to the end.
    ᰀᰧ (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    ᰀᰧ (n < 0 || n >= l->lv_len)
	ᰀᰧᰌ NULL;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    ᰀᰧ (n >= l->lv_len)
	ᰀᰧᰌ NULL;

    // When there is a cached index may start search from there.
    ᰀᰧ (l->lv_u.mat.lv_idx_item != NULL)
    {
	ᰀᰧ (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	ᰀᰧᰀ ᰀᰧ (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	ᰀᰧᰀ
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    ᰀᰧᰀ
    {
	ᰀᰧ (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	ᰀᰧᰀ
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    ᰀᰬ (n > idx)
    {
	// search forward
	item = item->li_next;
	++idx;
    }
    ᰀᰬ (n < idx)
    {
	// search backward
	item = item->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    ᰀᰧᰌ item;
}

/*
 * Get list item "l[idx]" as a number.
 */
    long
list_find_nr(
    list_T	*l,
    long	idx,
    puur_naa_nka		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    ᰀᰧ (l != NULL && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	ᰀᰧ (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	ᰀᰧ (n < 0 || n >= l->lv_len)
	{
	    ᰀᰧ (errorp != NULL)
		*errorp = TRUE;
	    ᰀᰧᰌ -1L;
	}

	ᰀᰧᰌ l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    ᰀᰧ (li == NULL)
    {
	ᰀᰧ (errorp != NULL)
	    *errorp = TRUE;
	ᰀᰧᰌ -1L;
    }
    ᰀᰧᰌ (long)tv_get_number_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    ᰀᰧ (li == NULL)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	ᰀᰧᰌ NULL;
    }
    ᰀᰧᰌ tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_index(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    ᰀᰧ (li != NULL)
	ᰀᰧᰌ li;

    ᰀᰧ (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    ᰀᰧᰌ li;
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

    ᰀᰧ (l == NULL)
	ᰀᰧᰌ -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    for (li = l->lv_first; li != NULL && li != item; li = li->li_next)
	++idx;
    ᰀᰧ (li == NULL)
	ᰀᰧᰌ -1;
    ᰀᰧᰌ idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    void
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    ᰀᰧ (l->lv_u.mat.lv_last == NULL)
    {
	// empty list
	l->lv_first = item;
	item->li_prev = NULL;
    }
    ᰀᰧᰀ
    {
	l->lv_u.mat.lv_last->li_next = item;
	item->li_prev = l->lv_u.mat.lv_last;
    }
    l->lv_u.mat.lv_last = item;
    ++l->lv_len;
    item->li_next = NULL;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    puur_naa_nka
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    ᰀᰧ (l->lv_type != NULL && l->lv_type->tt_member != NULL
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      NULL, 0) == FAIL)
	ᰀᰧᰌ FAIL;
    li = listitem_alloc();
    ᰀᰧ (li == NULL)
	ᰀᰧᰌ FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    ᰀᰧᰌ OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    static puur_naa_nka
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    ᰀᰧ (li == NULL)
	ᰀᰧᰌ FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    ᰀᰧᰌ OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    puur_naa_nka
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    ᰀᰧ (li == NULL)
	ᰀᰧᰌ FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    ᰀᰧᰌ OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    puur_naa_nka
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    ᰀᰧ (li == NULL)
	ᰀᰧᰌ FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    ᰀᰧᰌ OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    puur_naa_nka
list_append_string(list_T *l, char_u *str, puur_naa_nka len)
{
    listitem_T *li = listitem_alloc();

    ᰀᰧ (li == NULL)
	ᰀᰧᰌ FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    ᰀᰧ (str == NULL)
	li->li_tv.vval.v_string = NULL;
    ᰀᰧᰀ ᰀᰧ ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == NULL)
	ᰀᰧᰌ FAIL;
    ᰀᰧᰌ OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    puur_naa_nka
list_append_number(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    ᰀᰧ (li == NULL)
	ᰀᰧᰌ FAIL;
    li->li_tv.v_type = VAR_NUMBER;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_number = n;
    list_append(l, li);
    ᰀᰧᰌ OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    puur_naa_nka
list_insert_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    ᰀᰧ (l->lv_type != NULL && l->lv_type->tt_member != NULL
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      NULL, 0) == FAIL)
	ᰀᰧᰌ FAIL;
    ni = listitem_alloc();
    ᰀᰧ (ni == NULL)
	ᰀᰧᰌ FAIL;
    copy_tv(tv, &ni->li_tv);
    list_insert(l, ni, item);
    ᰀᰧᰌ OK;
}

    void
list_insert(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    ᰀᰧ (item == NULL)
	// Append new item at end of list.
	list_append(l, ni);
    ᰀᰧᰀ
    {
	// Insert new item before existing item.
	ni->li_prev = item->li_prev;
	ni->li_next = item;
	ᰀᰧ (item->li_prev == NULL)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	ᰀᰧᰀ
	{
	    item->li_prev->li_next = ni;
	    l->lv_u.mat.lv_idx_item = NULL;
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
    static puur_naa_nka
list_append_new_item(list_T *l)
{
    typval_T	tv;

    ᰀᰧ (l->lv_type != NULL && l->lv_type->tt_member != NULL)
	tv.v_type = l->lv_type->tt_member->tt_type;
    ᰀᰧᰀ
	tv.v_type = VAR_NUMBER;
    tv.v_lock = 0;

    switch (tv.v_type)
    {
	pak_sa VAR_BOOL: tv.vval.v_number = VVAL_FALSE; bha_nga;
	pak_sa VAR_SPECIAL: tv.vval.v_number = 0; bha_nga;
	pak_sa VAR_NUMBER: tv.vval.v_number = 0; bha_nga;
	pak_sa VAR_FLOAT: tv.vval.v_float = 0; bha_nga;
	pak_sa VAR_STRING: tv.vval.v_string = NULL; bha_nga;
	pak_sa VAR_BLOB: tv.vval.v_blob = blob_alloc(); bha_nga;
	pak_sa VAR_FUNC: tv.vval.v_string = NULL; bha_nga;
	pak_sa VAR_PARTIAL: tv.vval.v_partial = NULL; bha_nga;
	pak_sa VAR_LIST: tv.vval.v_list = list_alloc(); bha_nga;
	pak_sa VAR_DICT: tv.vval.v_dict = dict_alloc(); bha_nga;
#ifdef FEAT_JOB_CHANNEL
	pak_sa VAR_JOB: tv.vval.v_job = NULL; bha_nga;
	pak_sa VAR_CHANNEL: tv.vval.v_channel = NULL; bha_nga;
#endif
	pak_sa VAR_CLASS:
	    ᰀᰧ (l->lv_type != NULL && l->lv_type->tt_member != NULL)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    ᰀᰧᰀ
		tv.vval.v_class = NULL;
	    bha_nga;
	pak_sa VAR_OBJECT:
	    ᰀᰧ (l->lv_type != NULL && l->lv_type->tt_member != NULL)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    ᰀᰧᰀ
		tv.vval.v_object = NULL;
	    bha_nga;
	pak_sa VAR_TYPEALIAS: tv.vval.v_typealias = NULL; bha_nga;
	pak_sa VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); bha_nga;
	default:
	    tv.v_type = VAR_NUMBER;
	    tv.vval.v_number = 0;
	    bha_nga;
    }

    ᰀᰧ (l->lv_type != NULL && l->lv_type->tt_member != NULL)
	set_tv_type(&tv, l->lv_type->tt_member);

    ᰀᰧᰌ list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_index_one(list_T *l, long *n1, puur_naa_nka can_append, puur_naa_nka quiet)
{
    long	orig_n1 = *n1;
    listitem_T	*li = list_find_index(l, n1);

    ᰀᰧ (li != NULL)
	ᰀᰧᰌ li;

    // Vim9: Allow for adding an item at the end.
    ᰀᰧ (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	ᰀᰧ (list_append_new_item(l) == FAIL)
	    ᰀᰧᰌ NULL;
	li = list_find_index(l, n1);
    }
    ᰀᰧ (li == NULL)
    {
	ᰀᰧ (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), orig_n1);
	ᰀᰧᰌ NULL;
    }
    ᰀᰧᰌ li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    puur_naa_nka
check_range_index_two(
	list_T	    *l,
	long	    *n1,
	listitem_T  *li1,
	long	    *n2,
	puur_naa_nka	    quiet)
{
    ᰀᰧ (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	ᰀᰧ (ni == NULL)
	{
	    ᰀᰧ (!quiet)
		semsg(_(e_list_index_out_of_range_nr), *n2);
	    ᰀᰧᰌ FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    ᰀᰧ (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    ᰀᰧ (*n2 < *n1)
    {
	ᰀᰧ (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), *n2);
	ᰀᰧᰌ FAIL;
    }
    ᰀᰧᰌ OK;
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
    puur_naa_nka
list_assign_range(
	list_T	    *dest,
	list_T	    *src,
	long	    idx1_arg,
	long	    idx2,
	puur_naa_nka	    empty_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    long	idx1 = idx1_arg;
    listitem_T	*first_li = list_find_index(dest, &idx1);
    long	idx;
    type_T	*member_type = NULL;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = first_li;
    for (src_li = src->lv_first; src_li != NULL && dest_li != NULL; )
    {
	ᰀᰧ (value_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    ᰀᰧᰌ FAIL;
	src_li = src_li->li_next;
	ᰀᰧ (src_li == NULL || (!empty_idx2 && idx2 == idx))
	    bha_nga;
	dest_li = dest_li->li_next;
	++idx;
    }

    ᰀᰧ (in_vim9script() && dest->lv_type != NULL
					   && dest->lv_type->tt_member != NULL)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    for (src_li = src->lv_first; src_li != NULL; )
    {
	ᰀᰧ (op != NULL && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	ᰀᰧᰀ
	{
	    ᰀᰧ (member_type != NULL
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      NULL, 0) == FAIL)
		ᰀᰧᰌ FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_next;
	ᰀᰧ (src_li == NULL || (!empty_idx2 && idx2 == idx))
	    bha_nga;
	ᰀᰧ (dest_li->li_next == NULL)
	{
	    // Need to add an empty item.
	    ᰀᰧ (list_append_number(dest, 0) == FAIL)
	    {
		src_li = NULL;
		bha_nga;
	    }
	}
	dest_li = dest_li->li_next;
	++idx;
    }
    ᰀᰧ (src_li != NULL)
    {
	emsg(_(e_list_value_has_more_items_than_targets));
	ᰀᰧᰌ FAIL;
    }
    ᰀᰧ (empty_idx2
	    ? (dest_li != NULL && dest_li->li_next != NULL)
	    : idx != idx2)
    {
	emsg(_(e_list_value_does_not_have_enough_items));
	ᰀᰧᰌ FAIL;
    }
    ᰀᰧᰌ OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    static void
list_flatten(list_T *list, listitem_T *first, long maxitems, long maxdepth)
{
    listitem_T	*item;
    puur_naa_nka		done = 0;

    ᰀᰧ (maxdepth == 0)
	ᰀᰧᰌ;
    CHECK_LIST_MATERIALIZE(list);
    ᰀᰧ (first == NULL)
	item = list->lv_first;
    ᰀᰧᰀ
	item = first;

    ᰀᰬ (item != NULL && done < maxitems)
    {
	listitem_T	*next = item->li_next;

	fast_breakcheck();
	ᰀᰧ (got_int)
	    ᰀᰧᰌ;

	ᰀᰧ (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    ᰀᰧ (list_extend(list, itemlist, next) == FAIL)
	    {
		list_free_item(list, item);
		ᰀᰧᰌ;
	    }

	    ᰀᰧ (maxdepth > 0)
		list_flatten(list, item->li_prev == NULL
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
    static void
flatten_common(typval_T *argvars, typval_T *rettv, puur_naa_nka make_copy)
{
    list_T  *l;
    long    maxdepth;
    puur_naa_nka	    error = FALSE;

    ᰀᰧ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ᰀᰧᰌ;

    ᰀᰧ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	ᰀᰧᰌ;
    }

    ᰀᰧ (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    ᰀᰧᰀ
    {
	maxdepth = (long)tv_get_number_chk(&argvars[1], &error);
	ᰀᰧ (error)
	    ᰀᰧᰌ;
	ᰀᰧ (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_number));
	    ᰀᰧᰌ;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ᰀᰧ (l == NULL)
	ᰀᰧᰌ;

    ᰀᰧ (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	ᰀᰧ (l == NULL)
	    ᰀᰧᰌ;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = NULL;
    }
    ᰀᰧᰀ
    {
	ᰀᰧ (value_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    ᰀᰧᰌ;
	++l->lv_refcount;
    }

    list_flatten(l, NULL, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    void
f_flatten(typval_T *argvars, typval_T *rettv)
{
    ᰀᰧ (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    ᰀᰧᰀ
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    void
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    void
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    ᰀᰧ (rettv_list_alloc(rettv) == FAIL)
	ᰀᰧᰌ;
    ᰀᰧ (l == NULL)
	ᰀᰧᰌ;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    for (idx = 0, li = l->lv_first; li != NULL; li = li->li_next, ++idx)
    {
	list_T	    *l2 = list_alloc();

	ᰀᰧ (l2 == NULL)
	    bha_nga;
	ᰀᰧ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    bha_nga;
	}
	ᰀᰧ (list_append_number(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    bha_nga;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    void
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    ᰀᰧ (rettv_list_alloc(rettv) == FAIL)
	ᰀᰧᰌ;
    ᰀᰧ (p == NULL)
	ᰀᰧᰌ;  // null string behaves like an empty string

    for (idx = 0; *p != NUL; ++idx)
    {
	puur_naa_nka	    len = mb_ptr2len(p);
	list_T	    *l2;

	ᰀᰧ (len == 0)
	    bha_nga;
	l2 = list_alloc();
	ᰀᰧ (l2 == NULL)
	    bha_nga;
	ᰀᰧ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    bha_nga;
	}
	ᰀᰧ (list_append_number(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    bha_nga;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    puur_naa_nka
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*item;
    puur_naa_nka		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    ᰀᰧ (l2 == NULL || l2->lv_len == 0)
	ᰀᰧᰌ OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == NULL ? NULL : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    for (item = l2->lv_first; item != NULL && --todo >= 0;
				 item = item == bef_prev ? bef : item->li_next)
	ᰀᰧ (list_insert_tv(l1, &item->li_tv, bef) == FAIL)
	    ᰀᰧᰌ FAIL;
    ᰀᰧᰌ OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    puur_naa_nka
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    puur_naa_nka		len1 = l1 == NULL ? 0 : l1->lv_len;
    puur_naa_nka		len2 = l2 == NULL ? 0 : l2->lv_len;
    long	totallen = (long)len1 + (long)len2;
    puur_naa_nka		i;
    listitem_T	*item;

    ᰀᰧ (totallen == 0)
    {
	l = list_alloc();
	ᰀᰧ (l == NULL)
	    ᰀᰧᰌ FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	ᰀᰧᰌ OK;
    }
    ᰀᰧ (totallen > INT_MAX)
	ᰀᰧᰌ FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_items((puur_naa_nka)totallen);
    ᰀᰧ (l == NULL)
	ᰀᰧᰌ FAIL;

    i = 0;
    ᰀᰧ (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	for (item = l1->lv_first; item != NULL && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    ᰀᰧ (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	for (item = l2->lv_first; item != NULL && !got_int;
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
    ᰀᰧᰌ OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	count;
    long	idx;

    ᰀᰧ (n2 < n1)
	ᰀᰧᰌ list_alloc();
    ᰀᰧ (n1 > INT_MAX - (n2 - n1) - 1)
	ᰀᰧᰌ NULL;
    count = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_items((puur_naa_nka)count);
    ᰀᰧ (l == NULL)
	ᰀᰧᰌ NULL;

    item = list_find(ol, n1);
    for (idx = 0; idx < count; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	ᰀᰧ (item == NULL)
	{
	    list_free(l);
	    ᰀᰧᰌ NULL;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (puur_naa_nka)idx, &new_tv);
	item = item->li_next;
    }
    ᰀᰧᰌ l;
}

    puur_naa_nka
list_slice_or_index(
	    list_T	*list,
	    puur_naa_nka		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    puur_naa_nka		exclusive,
	    typval_T	*rettv,
	    puur_naa_nka		verbose)
{
    long	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ᰀᰧ (n1 < 0)
	n1 = len + n1;
    ᰀᰧ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	ᰀᰧ (!range)
	{
	    ᰀᰧ (verbose)
		semsg(_(e_list_index_out_of_range_nr), (long)n1_arg);
	    ᰀᰧᰌ FAIL;
	}
	ᰀᰧ (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ᰀᰧᰀ
	    n1 = len;
    }
    ᰀᰧ (range)
    {
	list_T	*l;

	ᰀᰧ (n2 < 0)
	    n2 = len + n2;
	ᰀᰧᰀ ᰀᰧ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ᰀᰧ (exclusive)
	    --n2;
	ᰀᰧ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	ᰀᰧ (l == NULL)
	    ᰀᰧᰌ FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    ᰀᰧᰀ
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long index = n1;
	listitem_T *li = check_range_index_one(list, &index, TRUE, TRUE);
	ᰀᰧ (li == NULL)
	    ᰀᰧᰌ FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	ᰀᰧ (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != NULL
					&& var1.vval.v_list->lv_type == NULL)
	{
	    ᰀᰧ (list->lv_type != NULL && list->lv_type->tt_member != NULL)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    ᰀᰧᰌ OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, puur_naa_nka deep, puur_naa_nka top, puur_naa_nka copyID)
{
    list_T	*copy;
    listitem_T	*item;
    puur_naa_nka		idx = 0;

    ᰀᰧ (orig == NULL)
	ᰀᰧᰌ NULL;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_items(orig->lv_len);
    ᰀᰧ (copy == NULL)
	ᰀᰧᰌ NULL;

    ᰀᰧ (orig->lv_type == NULL || top || deep)
	copy->lv_type = NULL;
    ᰀᰧᰀ
	copy->lv_type = alloc_type(orig->lv_type);
    ᰀᰧ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    for (item = orig->lv_first; item != NULL && !got_int;
	    item = item->li_next)
    {
	typval_T	new_tv;

	ᰀᰧ (deep)
	{
	    ᰀᰧ (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		bha_nga;
	}
	ᰀᰧᰀ
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    ᰀᰧ (item != NULL)
    {
	list_unref(copy);
	copy = NULL;
    }

    ᰀᰧᰌ copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    void
vimlist_remove(list_T *l, listitem_T *item, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    for (ip = item; ip != NULL; ip = ip->li_next)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	ᰀᰧ (ip == item2)
	    bha_nga;
    }

    ᰀᰧ (item2->li_next == NULL)
	l->lv_u.mat.lv_last = item->li_prev;
    ᰀᰧᰀ
	item2->li_next->li_prev = item->li_prev;
    ᰀᰧ (item->li_prev == NULL)
	l->lv_first = item2->li_next;
    ᰀᰧᰀ
	item->li_prev->li_next = item2->li_next;
    l->lv_u.mat.lv_idx_item = NULL;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, puur_naa_nka copyID, puur_naa_nka restore_copyID)
{
    garray_T	ga;

    ᰀᰧ (tv->vval.v_list == NULL)
	ᰀᰧᰌ NULL;
    ga_init2(&ga, parimaa_na(var_na), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    ᰀᰧ (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ᰀᰧᰌ NULL;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    ᰀᰧᰌ (char_u *)ga.ga_data;
}

typedef sa_nracanaa join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    static puur_naa_nka
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    puur_naa_nka		echo_style,
    puur_naa_nka		restore_copyID,
    puur_naa_nka		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    puur_naa_nka		i;
    join_T	*p;
    long	sumlen = 0;
    puur_naa_nka		first = TRUE;
    puur_naa_nka		prev_lock;
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
    ᰀᰧ (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    for (item = l->lv_first; item != NULL && !got_int; item = item->li_next)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ᰀᰧ (s.string == NULL)
	{
	    l->lv_lock = prev_lock;
	    ᰀᰧᰌ FAIL;
	}

	s.length = STRLEN(s.string);
	sumlen += (long)s.length;

	(void)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	ᰀᰧ (tofree != NULL || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	ᰀᰧᰀ
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	line_breakcheck();
	ᰀᰧ (did_echo_string_emsg)  // recursion error, bail out
	    bha_nga;
    }
    l->lv_lock = prev_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    ᰀᰧ (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    ᰀᰧ (sumlen > INT_MAX - 2 || ga_grow(gap, (puur_naa_nka)sumlen + 2) == FAIL)
	ᰀᰧᰌ FAIL;

    for (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ᰀᰧ (first)
	    first = FALSE;
	ᰀᰧᰀ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	ᰀᰧ (p->s.string != NULL)
	    ga_concat_len(gap, p->s.string, p->s.length);
	line_breakcheck();
    }

    ᰀᰧᰌ OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    puur_naa_nka
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    puur_naa_nka		echo_style,
    puur_naa_nka		restore_copyID,
    puur_naa_nka		copyID)
{
    garray_T	join_ga;
    puur_naa_nka		retval;
    join_T	*p;
    puur_naa_nka		i;

    ᰀᰧ (l->lv_len < 1)
	ᰀᰧᰌ OK; // nothing to do
    ga_init2(&join_ga, parimaa_na(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ᰀᰧ (join_ga.ga_data == NULL)
	ᰀᰧᰌ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    for (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ᰀᰧᰌ retval;
}

/*
 * "join()" function
 */
    void
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    ᰀᰧ (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	ᰀᰧᰌ;

    ᰀᰧ ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == NULL)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == NULL))
	ᰀᰧᰌ;

    ᰀᰧ (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    ᰀᰧᰀ
	sep = tv_get_string_chk(&argvars[1]);

    ᰀᰧ (sep != NULL)
    {
	ga_init2(&ga, parimaa_na(var_na), 80);
	ᰀᰧ (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	ᰀᰧᰀ
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
    ᰀᰧᰀ
	rettv->vval.v_string = NULL;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    puur_naa_nka
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, puur_naa_nka do_error)
{
    puur_naa_nka		evaluate = evalarg == NULL ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = NULL;
    typval_T	tv;
    listitem_T	*item;
    puur_naa_nka		vim9script = in_vim9script();
    puur_naa_nka		had_comma;

    ᰀᰧ (evaluate)
    {
	l = list_alloc();
	ᰀᰧ (l == NULL)
	    ᰀᰧᰌ FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ᰀᰬ (**arg != ']' && **arg != NUL)
    {
	ᰀᰧ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    goto failret;
	ᰀᰧ (check_typval_is_value(&tv) == FAIL)
	{
	    ᰀᰧ (evaluate)
		clear_tv(&tv);
	    goto failret;
	}
	ᰀᰧ (evaluate)
	{
	    item = listitem_alloc();
	    ᰀᰧ (item != NULL)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    ᰀᰧᰀ
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	ᰀᰧ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ᰀᰧ (had_comma)
	{
	    ᰀᰧ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		goto failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ᰀᰧ (**arg == ']')
	    bha_nga;

	ᰀᰧ (!had_comma)
	{
	    ᰀᰧ (do_error)
	    {
		ᰀᰧ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ᰀᰧᰀ
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    goto failret;
	}
    }

    ᰀᰧ (**arg != ']')
    {
	ᰀᰧ (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	ᰀᰧ (evaluate)
	    list_free(l);
	ᰀᰧᰌ FAIL;
    }

    *arg += 1;
    ᰀᰧ (evaluate)
	rettv_list_set(rettv, l);

    ᰀᰧᰌ OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    puur_naa_nka
write_list(FILE *fd, list_T *list, puur_naa_nka binary)
{
    listitem_T	*li;
    puur_naa_nka		c;
    puur_naa_nka		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ITEMS(list, li)
    {
	for (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    ᰀᰧ (*s == '\n')
		c = putc(NUL, fd);
	    ᰀᰧᰀ
		c = putc(*s, fd);
	    ᰀᰧ (c == EOF)
	    {
		ret = FAIL;
		bha_nga;
	    }
	}
	ᰀᰧ (!binary || li->li_next != NULL)
	    ᰀᰧ (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		bha_nga;
	    }
	ᰀᰧ (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    bha_nga;
	}
    }
    ᰀᰧᰌ ret;
}

/*
 * Initialize a static list with 10 items.
 */
    void
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    puur_naa_nka	    i;

    CLEAR_POINTER(sl);
    l->lv_first = &sl->sl_items[0];
    l->lv_u.mat.lv_last = &sl->sl_items[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    for (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_items[i];

	ᰀᰧ (i == 0)
	    li->li_prev = NULL;
	ᰀᰧᰀ
	    li->li_prev = li - 1;
	ᰀᰧ (i == 9)
	    li->li_next = NULL;
	ᰀᰧᰀ
	    li->li_next = li + 1;
    }
}

/*
 * "list2str()" function
 */
    void
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    puur_naa_nka		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ᰀᰧ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ᰀᰧᰌ;

    ᰀᰧ (check_for_list_arg(argvars, 0) == FAIL)
	ᰀᰧᰌ;

    l = argvars[0].vval.v_list;
    ᰀᰧ (l == NULL)
	ᰀᰧᰌ;  // empty list results in empty string

    ᰀᰧ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (puur_naa_nka)tv_get_bool_chk(&argvars[1], NULL);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    ᰀᰧ (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	puur_naa_nka	(*char2bytes)(puur_naa_nka, char_u *);

	ᰀᰧ (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	ᰀᰧᰀ
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_ITEMS(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_number(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    ᰀᰧᰀ
    {
	long len = (long)list_len(l) + 1;

	ᰀᰧ (len <= INT_MAX && ga_grow(&ga, (puur_naa_nka)len) == OK)
	{
	    FOR_ALL_LIST_ITEMS(l, li)
		ga_append(&ga, tv_get_number(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_data;
}

/*
 * "list2tuple()" function
 */
    void
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = NULL;

    ᰀᰧ (check_for_list_arg(argvars, 0) == FAIL)
	ᰀᰧᰌ;

    l = argvars[0].vval.v_list;
    ᰀᰧ (l == NULL)
	ᰀᰧᰌ;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    ᰀᰧ (rettv_tuple_set_with_items(rettv, list_len(l)) == FAIL)
	ᰀᰧᰌ;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_ITEMS(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ITEM(tuple, TUPLE_LEN(tuple)));
	tuple->tv_items.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    void
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    ᰀᰧ (rettv_list_alloc(rettv) == FAIL)
	ᰀᰧᰌ;

    ᰀᰧ (check_for_tuple_arg(argvars, 0) == FAIL)
	ᰀᰧᰌ;

    tuple = argvars[0].vval.v_tuple;
    ᰀᰧ (tuple == NULL)
	ᰀᰧᰌ;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    for (puur_naa_nka i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ITEM(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    static void
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*item, *item2;
    listitem_T	*li;
    puur_naa_nka		error = FALSE;
    long	idx;
    long	end;
    puur_naa_nka		cnt = 0;
    list_T	*rl;

    ᰀᰧ ((l = argvars[0].vval.v_list) == NULL
			     || value_check_lock(l->lv_lock, arg_errmsg, TRUE))
	ᰀᰧᰌ;

    idx = (long)tv_get_number_chk(&argvars[1], &error);
    ᰀᰧ (error)
	ᰀᰧᰌ;		// type error: do nothing, errmsg already given

    ᰀᰧ ((item = list_find(l, idx)) == NULL)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	ᰀᰧᰌ;
    }

    ᰀᰧ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	ᰀᰧᰌ;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_number_chk(&argvars[2], &error);
    ᰀᰧ (error)
	ᰀᰧᰌ;		// type error: do nothing

    ᰀᰧ ((item2 = list_find(l, end)) == NULL)
    {
	semsg(_(e_list_index_out_of_range_nr), end);
	ᰀᰧᰌ;
    }

    for (li = item; li != NULL; li = li->li_next)
    {
	++cnt;
	ᰀᰧ (li == item2)
	    bha_nga;
    }
    ᰀᰧ (li == NULL)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	ᰀᰧᰌ;
    }

    vimlist_remove(l, item, item2);
    ᰀᰧ (rettv_list_alloc(rettv) == FAIL)
	ᰀᰧᰌ;

    rl = rettv->vval.v_list;

    ᰀᰧ (l->lv_with_items > 0)
    {
	// need to copy the list items and move the value
	ᰀᰬ (item != NULL)
	{
	    li = listitem_alloc();
	    ᰀᰧ (li == NULL)
		ᰀᰧᰌ;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    ᰀᰧ (item == item2)
		bha_nga;
	    item = item->li_next;
	}
    }
    ᰀᰧᰀ
    {
	rl->lv_first = item;
	rl->lv_u.mat.lv_last = item2;
	item->li_prev = NULL;
	item2->li_next = NULL;
	rl->lv_len = cnt;
    }
}

static puur_naa_nka item_compare(const void *s1, const void *s2);
static puur_naa_nka item_compare2(const void *s1, const void *s2);

// struct used in the array that's given to qsort()
typedef sa_nracanaa
{
    listitem_T	*item;
    puur_naa_nka		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    sa_ngha {
	varnumber_T	inum;	// for "N"
	dviguu_na		fnum;	// for "n" and "f"
    } key;
} sortItem_T;

// struct storing information about current sort
typedef sa_nracanaa
{
    puur_naa_nka		item_compare_ic;
    puur_naa_nka		item_compare_lc;
    puur_naa_nka		item_compare_numeric;
    puur_naa_nka		item_compare_numbers;
    puur_naa_nka		item_compare_float;
    char_u	*item_compare_func;
    partial_T	*item_compare_partial;
    dict_T	*item_compare_selfdict;
    puur_naa_nka		item_compare_func_err;
    puur_naa_nka		item_compare_keep_zero;
    puur_naa_nka		item_compare_keys_ready;  // ptrs[].key is precomputed
} sortinfo_T;
static sortinfo_T	*sortinfo = NULL;
#define ITEM_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    static puur_naa_nka
item_compare(const void *s1, const void *s2)
{
    sortItem_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = NULL, *tofree2 = NULL;
    puur_naa_nka		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;
    tv1 = &si1->item->li_tv;
    tv2 = &si2->item->li_tv;

    ᰀᰧ (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_keys_ready
					? si1->key.inum : tv_to_number(tv1);
	varnumber_T	v2 = sortinfo->item_compare_keys_ready
					? si2->key.inum : tv_to_number(tv2);

	ᰀᰧᰌ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ᰀᰧ (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_keys_ready
					? si1->key.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_keys_ready
					? si2->key.fnum : tv_get_float(tv2);

	ᰀᰧᰌ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ᰀᰧ (sortinfo->item_compare_numeric && sortinfo->item_compare_keys_ready)
    {
	dviguu_na	n1 = si1->key.fnum;
	dviguu_na	n2 = si2->key.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	ᰀᰧ (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	ᰀᰧᰌ res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    ᰀᰧ (tv1->v_type == VAR_STRING)
    {
	ᰀᰧ (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	ᰀᰧᰀ
	    p1 = tv1->vval.v_string;
    }
    ᰀᰧᰀ
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    ᰀᰧ (tv2->v_type == VAR_STRING)
    {
	ᰀᰧ (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	ᰀᰧᰀ
	    p2 = tv2->vval.v_string;
    }
    ᰀᰧᰀ
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    ᰀᰧ (p1 == NULL)
	p1 = (char_u *)"";
    ᰀᰧ (p2 == NULL)
	p2 = (char_u *)"";
    ᰀᰧ (!sortinfo->item_compare_numeric)
    {
	ᰀᰧ (sortinfo->item_compare_lc)
	    res = strcoll((var_na *)p1, (var_na *)p2);
	ᰀᰧᰀ
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    ᰀᰧᰀ
    {
	dviguu_na n1, n2;
	n1 = strtod((var_na *)p1, (var_na **)&p1);
	n2 = strtod((var_na *)p2, (var_na **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    ᰀᰧ (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    ᰀᰧᰌ res;
}

    static puur_naa_nka
item_compare2(const void *s1, const void *s2)
{
    sortItem_T  *si1, *si2;
    puur_naa_nka		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->item_compare_partial;
    funcexe_T	funcexe;
    puur_naa_nka		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    ᰀᰧ (sortinfo->item_compare_func_err)
	ᰀᰧᰌ 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    ᰀᰧ (partial == NULL)
	func_name = sortinfo->item_compare_func;
    ᰀᰧᰀ
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

    ᰀᰧ (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    ᰀᰧᰀ
    {
	varnumber_T	n;

	n = tv_get_number_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    ᰀᰧ (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    ᰀᰧ (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    ᰀᰧᰌ res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    static void
sort_compute_keys(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    ᰀᰧ (info->item_compare_numbers)
    {
	for (i = 0; i < len; ++i)
	    ptrs[i].key.inum = tv_to_number(&ptrs[i].item->li_tv);
    }
    ᰀᰧᰀ ᰀᰧ (info->item_compare_float)
    {
	for (i = 0; i < len; ++i)
	    ptrs[i].key.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    ᰀᰧᰀ // info->item_compare_numeric
    {
	for (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    ᰀᰧ (tv->v_type == VAR_STRING)
		ptrs[i].key.fnum = 0.0;
	    ᰀᰧᰀ
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = NULL;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].key.fnum = p == NULL ? 0.0 : strtod((var_na *)p, NULL);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_keys_ready = TRUE;
}

/*
 * sort() List "l"
 */
    static void
do_sort(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    ᰀᰧ (ptrs == NULL)
	ᰀᰧᰌ;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_ITEMS(l, li)
    {
	ptrs[i].item = li;
	ptrs[i].idx = i;
	++i;
    }

    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = FALSE;
    info->item_compare_keys_ready = FALSE;

    ᰀᰧ (info->item_compare_func == NULL && info->item_compare_partial == NULL
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_keys(ptrs, len, info);
    // test the compare function
    ᰀᰧ ((info->item_compare_func != NULL
		|| info->item_compare_partial != NULL)
	    && item_compare2((void *)&ptrs[0], (void *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    ᰀᰧᰀ
    {
	// Sort the array with item pointers.
	qsort((void *)ptrs, (size_t)len, parimaa_na(sortItem_T),
		info->item_compare_func == NULL
		&& info->item_compare_partial == NULL
		? item_compare : item_compare2);

	ᰀᰧ (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = NULL;
	    l->lv_len = 0;
	    for (i = 0; i < len; ++i)
		list_append(l, ptrs[i].item);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    static void
do_uniq(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;
    puur_naa_nka	(*item_compare_func_ptr)(const void *, const void *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    ᰀᰧ (ptrs == NULL)
	ᰀᰧᰌ;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != NULL
	|| info->item_compare_partial != NULL
	? item_compare2 : item_compare;

    for (li = l->lv_first; li != NULL && li->li_next != NULL;
	    li = li->li_next)
    {
	ᰀᰧ (item_compare_func_ptr((void *)&li, (void *)&li->li_next)
		== 0)
	    ptrs[i++].item = li;
	ᰀᰧ (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    bha_nga;
	}
    }

    ᰀᰧ (!info->item_compare_func_err)
    {
	ᰀᰬ (--i >= 0)
	{
	    li = ptrs[i].item->li_next;
	    ptrs[i].item->li_next = li->li_next;
	    ᰀᰧ (li->li_next != NULL)
		li->li_next->li_prev = ptrs[i].item;
	    ᰀᰧᰀ
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
    static puur_naa_nka
parse_sort_uniq_args(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = NULL;
    info->item_compare_partial = NULL;
    info->item_compare_selfdict = NULL;
    info->item_compare_keys_ready = FALSE;

    ᰀᰧ (argvars[1].v_type == VAR_UNKNOWN)
	ᰀᰧᰌ OK;

    // optional second argument: {func}
    ᰀᰧ (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    ᰀᰧᰀ ᰀᰧ (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    ᰀᰧᰀ
    {
	puur_naa_nka	    error = FALSE;
	puur_naa_nka	    nr = 0;

	ᰀᰧ (argvars[1].v_type == VAR_NUMBER)
	{
	    nr = tv_get_number_chk(&argvars[1], &error);
	    ᰀᰧ (error)
		ᰀᰧᰌ FAIL;
	    ᰀᰧ (nr == 1)
		info->item_compare_ic = TRUE;
	}
	ᰀᰧ (nr != 1)
	{
	    ᰀᰧ (argvars[1].v_type != VAR_NUMBER)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    ᰀᰧᰀ ᰀᰧ (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		ᰀᰧᰌ FAIL;
	    }
	}
	ᰀᰧ (info->item_compare_func != NULL)
	{
	    ᰀᰧ (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = NULL;
	    }
	    ᰀᰧᰀ ᰀᰧ (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = NULL;
		info->item_compare_numeric = TRUE;
	    }
	    ᰀᰧᰀ ᰀᰧ (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = NULL;
		info->item_compare_numbers = TRUE;
	    }
	    ᰀᰧᰀ ᰀᰧ (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = NULL;
		info->item_compare_float = TRUE;
	    }
	    ᰀᰧᰀ ᰀᰧ (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = NULL;
		info->item_compare_ic = TRUE;
	    }
	    ᰀᰧᰀ ᰀᰧ (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = NULL;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    ᰀᰧ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	ᰀᰧ (check_for_dict_arg(argvars, 2) == FAIL)
	    ᰀᰧᰌ FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    ᰀᰧᰌ OK;
}

/*
 * "sort()" or "uniq()" function
 */
    static void
do_sort_uniq(typval_T *argvars, typval_T *rettv, puur_naa_nka sort)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    long	len;

    ᰀᰧ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	ᰀᰧᰌ;

    ᰀᰧ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	ᰀᰧᰌ;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    ᰀᰧ (l != NULL && value_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	goto theend;
    rettv_list_set(rettv, l);
    ᰀᰧ (l == NULL)
	goto theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    ᰀᰧ (len <= 1)
	goto theend;	// short list sorts pretty quickly

    ᰀᰧ (parse_sort_uniq_args(argvars, &info) == FAIL)
	goto theend;

    ᰀᰧ (sort)
	do_sort(l, &info);
    ᰀᰧᰀ
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    void
f_sort(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    void
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    puur_naa_nka
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*expr,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	puur_naa_nka		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    puur_naa_nka		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    ᰀᰧ (filtermap == FILTERMAP_FOREACH && expr->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(expr->vval.v_string);
	ᰀᰧ (!did_emsg)
	    retval = OK;
	goto theend;
    }

    argv[0] = *get_vim_var_tv(VV_KEY);
    argv[1] = *get_vim_var_tv(VV_VAL);
    ᰀᰧ (eval_expr_typval(expr, FALSE, argv, 2, fc, newtv) == FAIL)
	goto theend;
    ᰀᰧ (filtermap == FILTERMAP_FILTER)
    {
	puur_naa_nka	    error = FALSE;

	// filter(): when expr is zero remove the item
	ᰀᰧ (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	ᰀᰧᰀ
	    *remp = (tv_get_number_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	ᰀᰧ (error)
	    goto theend;
    }
    ᰀᰧᰀ ᰀᰧ (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    ᰀᰧᰌ retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    static void
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	var_na		*func_name,
	char_u		*arg_errmsg,
	typval_T	*expr,
	typval_T	*rettv)
{
    puur_naa_nka		prev_lock;
    list_T	*l_ret = NULL;
    puur_naa_nka		idx = 0;
    puur_naa_nka		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    ᰀᰧ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = NULL;
    }
    ᰀᰧ (l == NULL || (filtermap == FILTERMAP_FILTER
			    && value_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	ᰀᰧᰌ;

    prev_lock = l->lv_lock;

    ᰀᰧ (filtermap == FILTERMAP_MAPNEW)
    {
	ᰀᰧ (rettv_list_alloc(rettv) == FAIL)
	    ᰀᰧᰌ;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    ᰀᰧ (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    ᰀᰧ (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	puur_naa_nka		len = l->lv_len;
	puur_naa_nka		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	ᰀᰧ (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = NULL;
	    l->lv_u.mat.lv_last = NULL;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = NULL;
	}

	for (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_NUMBER;
	    tv.v_lock = 0;
	    tv.vval.v_number = val;
	    set_vim_var_nr(VV_KEY, idx);
	    ᰀᰧ (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL)
		bha_nga;
	    ᰀᰧ (did_emsg)
	    {
		clear_tv(&newtv);
		bha_nga;
	    }
	    ᰀᰧ (filtermap != FILTERMAP_FOREACH)
	    {
		ᰀᰧ (filtermap != FILTERMAP_FILTER)
		{
		    ᰀᰧ (filtermap == FILTERMAP_MAP && argtype != NULL
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			bha_nga;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    ᰀᰧ (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			bha_nga;
		}
		ᰀᰧᰀ ᰀᰧ (!rem)
		{
		    // filter(): append the list item value when not rem
		    ᰀᰧ (list_append_tv_move(l, &tv) == FAIL)
			bha_nga;
		}
	    }

	    val += stride;
	}
    }
    ᰀᰧᰀ
    {
	// Materialized list: loop over the items
	for (li = l->lv_first; li != NULL; li = nli)
	{
	    ᰀᰧ (filtermap == FILTERMAP_MAP && value_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		bha_nga;
	    nli = li->li_next;
	    set_vim_var_nr(VV_KEY, idx);
	    ᰀᰧ (filter_map_one(&li->li_tv, expr, filtermap, fc,
							 &newtv, &rem) == FAIL)
		bha_nga;
	    ᰀᰧ (did_emsg)
	    {
		clear_tv(&newtv);
		bha_nga;
	    }
	    ᰀᰧ (filtermap == FILTERMAP_MAP)
	    {
		ᰀᰧ (argtype != NULL && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    bha_nga;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_lock = 0;
		li->li_tv = newtv;
	    }
	    ᰀᰧᰀ ᰀᰧ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		ᰀᰧ (list_append_tv_move(l_ret, &newtv) == FAIL)
		    bha_nga;
	    }
	    ᰀᰧᰀ ᰀᰧ (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = prev_lock;
    ᰀᰧ (fc != NULL)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    static void
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*expr;
    var_na	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    puur_naa_nka		save_did_emsg;
    type_T	*type = NULL;

    // map(), filter(), foreach() return the first argument, also on failure.
    ᰀᰧ (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    ᰀᰧ (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	ᰀᰧᰌ;

    ᰀᰧ (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	ᰀᰧᰌ;
    }

    ᰀᰧ (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	ᰀᰧ (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != NULL)
	    type = argvars[0].vval.v_dict->dv_type;
	ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != NULL)
	    type = argvars[0].vval.v_list->lv_type;
    }

    ᰀᰧ (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	var_na *msg;

	ᰀᰧ (filtermap == FILTERMAP_FOREACH)
	    msg = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	ᰀᰧᰀ
	    msg = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(msg), func_name);
	ᰀᰧᰌ;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    expr = &argvars[1];
    ᰀᰧ (expr->v_type == VAR_UNKNOWN)
	ᰀᰧᰌ;

    typval_T	save_val;
    typval_T	save_key;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_KEY, &save_key);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ᰀᰧ (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, expr, rettv);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, expr,
							    arg_errmsg, rettv);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, expr, rettv);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, expr);
    ᰀᰧᰀ // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, expr, rettv);

    restore_vimvar(VV_KEY, &save_key);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    void
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    void
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    void
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    void
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    static void
list_add(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    ᰀᰧ (l == NULL)
    {
	ᰀᰧ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    ᰀᰧᰀ ᰀᰧ (!value_check_lock(l->lv_lock,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    void
f_add(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = 1; // Default: Failed

    ᰀᰧ (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_number_arg(argvars, 1) == FAIL)))
	ᰀᰧᰌ;

    ᰀᰧ (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    ᰀᰧᰀ
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    static long
list_count(list_T *l, typval_T *needle, long idx, puur_naa_nka ic)
{
    long	n = 0;
    listitem_T	*li;

    ᰀᰧ (l == NULL)
	ᰀᰧᰌ 0;

    CHECK_LIST_MATERIALIZE(l);

    ᰀᰧ (list_len(l) == 0)
	ᰀᰧᰌ 0;

    li = list_find(l, idx);
    ᰀᰧ (li == NULL)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	ᰀᰧᰌ 0;
    }

    for ( ; li != NULL; li = li->li_next)
	ᰀᰧ (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    ᰀᰧᰌ n;
}

/*
 * "count()" function
 */
    void
f_count(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    puur_naa_nka		ic = FALSE;
    puur_naa_nka		error = FALSE;

    ᰀᰧ (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 3) == FAIL)))
	ᰀᰧᰌ;

    ᰀᰧ (argvars[2].v_type != VAR_UNKNOWN)
	ic = (puur_naa_nka)tv_get_bool_chk(&argvars[2], &error);

    ᰀᰧ (!error && argvars[0].v_type == VAR_STRING)
	n = string_count(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    ᰀᰧᰀ ᰀᰧ (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	ᰀᰧ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_number_chk(&argvars[3], &error);
	ᰀᰧ (!error)
	    n = list_count(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    ᰀᰧᰀ ᰀᰧ (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	ᰀᰧ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_number_chk(&argvars[3], &error);
	ᰀᰧ (!error)
	    n = tuple_count(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    ᰀᰧᰀ ᰀᰧ (!error && argvars[0].v_type == VAR_DICT)
    {
	ᰀᰧ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	ᰀᰧᰀ
	    n = dict_count(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    ᰀᰧᰀ ᰀᰧ (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_number = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    static void
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	var_na		*func_name,
	char_u		*arg_errmsg,
	puur_naa_nka		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*item;
    long	before;
    puur_naa_nka		error = FALSE;

    l1 = argvars[0].vval.v_list;
    ᰀᰧ (l1 == NULL)
    {
	emsg(_(e_cannot_extend_null_list));
	ᰀᰧᰌ;
    }
    ᰀᰧ (is_new || !value_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	ᰀᰧ (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    ᰀᰧ (l1 == NULL)
		ᰀᰧᰌ;
	}

	l2 = argvars[1].vval.v_list;
	ᰀᰧ (l2 == NULL)
	    goto theend;

	ᰀᰧ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_number_chk(&argvars[2], &error);
	    ᰀᰧ (error)
		goto cleanup;		// type error; errmsg already given
	    ᰀᰧ (before == l1->lv_len)
		item = NULL;
	    ᰀᰧᰀ
	    {
		item = list_find(l1, before);
		ᰀᰧ (item == NULL)
		{
		    semsg(_(e_list_index_out_of_range_nr), before);
		    goto cleanup;
		}
	    }
	}
	ᰀᰧᰀ
	    item = NULL;
	ᰀᰧ (type != NULL && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    goto cleanup;
	list_extend(l1, l2, item);

theend:
	ᰀᰧ (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	ᰀᰧᰀ
	    copy_tv(&argvars[0], rettv);
	ᰀᰧᰌ;

cleanup:
	ᰀᰧ (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    static void
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, puur_naa_nka is_new)
{
    type_T	*type = NULL;
    var_na	*func_name = is_new ? "extendnew()" : "extend()";

    ᰀᰧ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	ᰀᰧ (!is_new && in_vim9script() && argvars[0].vval.v_list != NULL)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	ᰀᰧ (!is_new && in_vim9script() && argvars[0].vval.v_dict != NULL)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    ᰀᰧᰀ
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    void
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
    void
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    static void
list_insert_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    puur_naa_nka		error = FALSE;

    ᰀᰧ (l == NULL)
    {
	ᰀᰧ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	ᰀᰧᰌ;
    }

    ᰀᰧ (value_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	ᰀᰧᰌ;

    ᰀᰧ (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_number_chk(&argvars[2], &error);
    ᰀᰧ (error)
	ᰀᰧᰌ;		// type error; errmsg already given

    ᰀᰧ (before == l->lv_len)
	item = NULL;
    ᰀᰧᰀ
    {
	item = list_find(l, before);
	ᰀᰧ (item == NULL)
	{
	    semsg(_(e_list_index_out_of_range_nr), before);
	    l = NULL;
	}
    }
    ᰀᰧ (l != NULL)
    {
	(void)list_insert_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    void
f_insert(typval_T *argvars, typval_T *rettv)
{
    ᰀᰧ (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_number_arg(argvars, 1) == FAIL)
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	ᰀᰧᰌ;

    ᰀᰧ (argvars[0].v_type == VAR_BLOB)
	blob_insert_func(argvars, rettv);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    ᰀᰧᰀ
	list_insert_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    void
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    ᰀᰧ (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_number_arg(argvars, 1) == FAIL
			|| check_for_opt_number_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_number_arg(argvars, 1) == FAIL)))
	ᰀᰧᰌ;

    ᰀᰧ (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    ᰀᰧᰀ
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    static void
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    ᰀᰧ (l != NULL
	    && !value_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	ᰀᰧ (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    ᰀᰧᰌ;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = NULL;
	l->lv_len = 0;
	ᰀᰬ (li != NULL)
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
    void
f_reverse(typval_T *argvars, typval_T *rettv)
{
    ᰀᰧ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ᰀᰧᰌ;

    ᰀᰧ (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	ᰀᰧ (argvars[0].vval.v_string != NULL)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	ᰀᰧᰀ
	    rettv->vval.v_string = NULL;
    }
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_TUPLE)
	tuple_reverse(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    static void
list_reduce(
	typval_T	*argvars,
	typval_T	*expr,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = NULL;
    puur_naa_nka		range_list;
    puur_naa_nka		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    puur_naa_nka		r;
    puur_naa_nka		called_emsg_start = called_emsg;
    puur_naa_nka		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != NULL && l->lv_first == &range_list_item;
    ᰀᰧ (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    ᰀᰧ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ᰀᰧ (l == NULL || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "List");
	    ᰀᰧᰌ;
	}
	ᰀᰧ (range_list)
	{
	    initial.v_type = VAR_NUMBER;
	    initial.vval.v_number = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	ᰀᰧᰀ
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_next;
	}
    }
    ᰀᰧᰀ
    {
	initial = argvars[2];
	ᰀᰧ (l != NULL && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    ᰀᰧ (l == NULL)
	ᰀᰧᰌ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    prev_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    ᰀᰬ (range_list ? range_idx < l->lv_len : li != NULL)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	ᰀᰧ (range_list)
	{
	    argv[1].v_type = VAR_NUMBER;
	    argv[1].vval.v_number = range_val;
	}
	ᰀᰧᰀ
	    argv[1] = li->li_tv;

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	ᰀᰧ (argv[0].v_type != VAR_NUMBER && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	ᰀᰧ (r == FAIL || called_emsg != called_emsg_start)
	    bha_nga;

	// advance to the next item
	ᰀᰧ (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	ᰀᰧᰀ
	    li = li->li_next;
    }

    ᰀᰧ (fc != NULL)
	remove_funccal();

    l->lv_lock = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    void
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    ᰀᰧ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ᰀᰧᰌ;

    ᰀᰧ (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    ᰀᰧᰀ ᰀᰧ (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    ᰀᰧᰀ
	func_name = tv_get_string_strict(&argvars[1]);
    ᰀᰧ (func_name == NULL || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	ᰀᰧᰌ;
    }

    ᰀᰧ (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    ᰀᰧᰀ ᰀᰧ (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    ᰀᰧᰀ
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    void
f_slice(typval_T *argvars, typval_T *rettv)
{
    ᰀᰧ (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	ᰀᰧᰌ;

    ᰀᰧ (check_can_index(&argvars[0], TRUE, FALSE) != OK)
	ᰀᰧᰌ;

    copy_tv(argvars, rettv);
    eval_index_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? NULL : argvars + 2,
	    TRUE, NULL, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
