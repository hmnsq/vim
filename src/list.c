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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

// List heads for garbage collection.
நிலையான list_T		*first_list = வெற்று;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    க்கு ((lw) = (l)->lv_watch; (lw) != வெற்று; (lw) = (lw)->lw_next)

நிலையான வெறுமை list_free_item(list_T *l, listitem_T *item);

/*
 * Add a watcher to a list.
 */
    வெறுமை
list_add_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_next = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    வெறுமை
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	எனில் (lw == lwrem)
	{
	    *lwp = lw->lw_next;
	    நிறுத்து;
	}
	lwp = &lw->lw_next;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    நிலையான வெறுமை
list_fix_watch(list_T *l, listitem_T *item)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	எனில் (lw->lw_item == item)
	    lw->lw_item = item->li_next;
}

    நிலையான வெறுமை
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    எனில் (first_list != வெற்று)
	first_list->lv_used_prev = l;
    l->lv_used_prev = வெற்று;
    l->lv_used_next = first_list;
    first_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(வெறுமை)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    எனில் (l != வெற்று)
	list_init(l);
    திரும்பு l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    எனில் (alloc_fail_id == id && alloc_does_fail(அளவு(list_T)))
	திரும்பு வெற்று;
#endif
    திரும்பு (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_பொருட்கள்(முழு மொத்தம்)
{
    list_T	*l;

    எனில் (மொத்தம் > 0
	    && (size_t)மொத்தம் > (SIZE_MAX - அளவு(list_T)) / அளவு(listitem_T))
	திரும்பு வெற்று;
    l = (list_T *)alloc_clear(அளவு(list_T) + மொத்தம் * அளவு(listitem_T));
    எனில் (l == வெற்று)
	திரும்பு வெற்று;

    list_init(l);

    எனில் (மொத்தம் <= 0)
	திரும்பு l;

    listitem_T	*li = (listitem_T *)(l + 1);
    முழு		i;

    l->lv_len = மொத்தம்;
    l->lv_with_பொருட்கள் = மொத்தம்;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + மொத்தம் - 1;
    க்கு (i = 0; i < மொத்தம்; ++i)
    {
	எனில் (i == 0)
	    li->li_prev = வெற்று;
	இல்லையெனில்
	    li->li_prev = li - 1;
	எனில் (i == மொத்தம் - 1)
	    li->li_next = வெற்று;
	இல்லையெனில்
	    li->li_next = li + 1;
	++li;
    }

    திரும்பு l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    வெறுமை
list_set_item(list_T *l, முழு idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    முழு
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    எனில் (l == வெற்று)
	திரும்பு FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    திரும்பு OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    முழு
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    எனில் (alloc_fail_id == id && alloc_does_fail(அளவு(list_T)))
	திரும்பு FAIL;
#endif
    திரும்பு rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    வெறுமை
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    எனில் (l != வெற்று)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    வெறுமை
list_unref(list_T *l)
{
    எனில் (l != வெற்று && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    நிலையான வெறுமை
list_free_contents(list_T *l)
{
    listitem_T *item;

    எனில் (l->lv_first != &range_list_item)
	க்கு (item = l->lv_first; item != வெற்று; item = l->lv_first)
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
    முழு
list_free_nonref(முழு copyID)
{
    list_T	*ll;
    முழு		did_free = FALSE;

    க்கு (ll = first_list; ll != வெற்று; ll = ll->lv_used_next)
	எனில் ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == வெற்று)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    திரும்பு did_free;
}

    நிலையான வெறுமை
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    எனில் (l->lv_used_prev == வெற்று)
	first_list = l->lv_used_next;
    இல்லையெனில்
	l->lv_used_prev->lv_used_next = l->lv_used_next;
    எனில் (l->lv_used_next != வெற்று)
	l->lv_used_next->lv_used_prev = l->lv_used_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    வெறுமை
list_free_பொருட்கள்(முழு copyID)
{
    list_T	*ll, *ll_next;

    க்கு (ll = first_list; ll != வெற்று; ll = ll_next)
    {
	ll_next = ll->lv_used_next;
	எனில் ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == வெற்று)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    வெறுமை
list_free(list_T *l)
{
    எனில் (in_free_unref_பொருட்கள்)
	திரும்பு;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(வெறுமை)
{
    திரும்பு ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    நிலையான வெறுமை
list_free_item(list_T *l, listitem_T *item)
{
    எனில் (l->lv_with_பொருட்கள் == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_பொருட்கள்)
	vim_free(item);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    வெறுமை
listitem_free(list_T *l, listitem_T *item)
{
    clear_tv(&item->li_tv);
    list_free_item(l, item);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    வெறுமை
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
    எனில் (l == வெற்று)
	திரும்பு 0L;
    திரும்பு l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    முழு
list_equal(
    list_T	*l1,
    list_T	*l2,
    முழு		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    எனில் (l1 == l2)
	திரும்பு TRUE;
    எனில் (list_len(l1) != list_len(l2))
	திரும்பு FALSE;
    எனில் (list_len(l1) == 0)
	// empty and NULL list are considered equal
	திரும்பு TRUE;
    எனில் (l1 == வெற்று || l2 == வெற்று)
	திரும்பு FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    க்கு (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != வெற்று && item2 != வெற்று;
			       item1 = item1->li_next, item2 = item2->li_next)
	எனில் (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    திரும்பு FALSE;
    திரும்பு item1 == வெற்று && item2 == வெற்று;
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

    எனில் (l == வெற்று)
	திரும்பு வெற்று;

    // Negative index is relative to the end.
    எனில் (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    எனில் (n < 0 || n >= l->lv_len)
	திரும்பு வெற்று;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    எனில் (n >= l->lv_len)
	திரும்பு வெற்று;

    // When there is a cached index may start search from there.
    எனில் (l->lv_u.mat.lv_idx_item != வெற்று)
    {
	எனில் (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	இல்லையெனில் எனில் (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	இல்லையெனில்
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    இல்லையெனில்
    {
	எனில் (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	இல்லையெனில்
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    போது (n > idx)
    {
	// search forward
	item = item->li_next;
	++idx;
    }
    போது (n < idx)
    {
	// search backward
	item = item->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    திரும்பு item;
}

/*
 * Get list item "l[idx]" as a number.
 */
    long
list_find_nr(
    list_T	*l,
    long	idx,
    முழு		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    எனில் (l != வெற்று && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	எனில் (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	எனில் (n < 0 || n >= l->lv_len)
	{
	    எனில் (errorp != வெற்று)
		*errorp = TRUE;
	    திரும்பு -1L;
	}

	திரும்பு l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    எனில் (li == வெற்று)
    {
	எனில் (errorp != வெற்று)
	    *errorp = TRUE;
	திரும்பு -1L;
    }
    திரும்பு (long)tv_get_எண்_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    எனில் (li == வெற்று)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	திரும்பு வெற்று;
    }
    திரும்பு tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_index(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    எனில் (li != வெற்று)
	திரும்பு li;

    எனில் (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    திரும்பு li;
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

    எனில் (l == வெற்று)
	திரும்பு -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    க்கு (li = l->lv_first; li != வெற்று && li != item; li = li->li_next)
	++idx;
    எனில் (li == வெற்று)
	திரும்பு -1;
    திரும்பு idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    வெறுமை
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    எனில் (l->lv_u.mat.lv_last == வெற்று)
    {
	// empty list
	l->lv_first = item;
	item->li_prev = வெற்று;
    }
    இல்லையெனில்
    {
	l->lv_u.mat.lv_last->li_next = item;
	item->li_prev = l->lv_u.mat.lv_last;
    }
    l->lv_u.mat.lv_last = item;
    ++l->lv_len;
    item->li_next = வெற்று;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    முழு
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    எனில் (l->lv_type != வெற்று && l->lv_type->tt_member != வெற்று
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      வெற்று, 0) == FAIL)
	திரும்பு FAIL;
    li = listitem_alloc();
    எனில் (li == வெற்று)
	திரும்பு FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    திரும்பு OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    நிலையான முழு
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    எனில் (li == வெற்று)
	திரும்பு FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    திரும்பு OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    முழு
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    எனில் (li == வெற்று)
	திரும்பு FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    திரும்பு OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    முழு
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    எனில் (li == வெற்று)
	திரும்பு FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    திரும்பு OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    முழு
list_append_string(list_T *l, char_u *str, முழு len)
{
    listitem_T *li = listitem_alloc();

    எனில் (li == வெற்று)
	திரும்பு FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    எனில் (str == வெற்று)
	li->li_tv.vval.v_string = வெற்று;
    இல்லையெனில் எனில் ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == வெற்று)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    முழு
list_append_எண்(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    எனில் (li == வெற்று)
	திரும்பு FAIL;
    li->li_tv.v_type = VAR_எண்;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_எண் = n;
    list_append(l, li);
    திரும்பு OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    முழு
list_செருகு_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    எனில் (l->lv_type != வெற்று && l->lv_type->tt_member != வெற்று
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      வெற்று, 0) == FAIL)
	திரும்பு FAIL;
    ni = listitem_alloc();
    எனில் (ni == வெற்று)
	திரும்பு FAIL;
    copy_tv(tv, &ni->li_tv);
    list_செருகு(l, ni, item);
    திரும்பு OK;
}

    வெறுமை
list_செருகு(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    எனில் (item == வெற்று)
	// Append new item at end of list.
	list_append(l, ni);
    இல்லையெனில்
    {
	// Insert new item before existing item.
	ni->li_prev = item->li_prev;
	ni->li_next = item;
	எனில் (item->li_prev == வெற்று)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	இல்லையெனில்
	{
	    item->li_prev->li_next = ni;
	    l->lv_u.mat.lv_idx_item = வெற்று;
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
    நிலையான முழு
list_append_new_item(list_T *l)
{
    typval_T	tv;

    எனில் (l->lv_type != வெற்று && l->lv_type->tt_member != வெற்று)
	tv.v_type = l->lv_type->tt_member->tt_type;
    இல்லையெனில்
	tv.v_type = VAR_எண்;
    tv.v_lock = 0;

    தேர்வு (tv.v_type)
    {
	நிலை VAR_BOOL: tv.vval.v_எண் = VVAL_FALSE; நிறுத்து;
	நிலை VAR_SPECIAL: tv.vval.v_எண் = 0; நிறுத்து;
	நிலை VAR_எண்: tv.vval.v_எண் = 0; நிறுத்து;
	நிலை VAR_FLOAT: tv.vval.v_float = 0; நிறுத்து;
	நிலை VAR_STRING: tv.vval.v_string = வெற்று; நிறுத்து;
	நிலை VAR_BLOB: tv.vval.v_blob = blob_alloc(); நிறுத்து;
	நிலை VAR_FUNC: tv.vval.v_string = வெற்று; நிறுத்து;
	நிலை VAR_PARTIAL: tv.vval.v_partial = வெற்று; நிறுத்து;
	நிலை VAR_LIST: tv.vval.v_list = list_alloc(); நிறுத்து;
	நிலை VAR_DICT: tv.vval.v_dict = dict_alloc(); நிறுத்து;
#ifdef FEAT_JOB_CHANNEL
	நிலை VAR_JOB: tv.vval.v_job = வெற்று; நிறுத்து;
	நிலை VAR_CHANNEL: tv.vval.v_channel = வெற்று; நிறுத்து;
#endif
	நிலை VAR_CLASS:
	    எனில் (l->lv_type != வெற்று && l->lv_type->tt_member != வெற்று)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    இல்லையெனில்
		tv.vval.v_class = வெற்று;
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    எனில் (l->lv_type != வெற்று && l->lv_type->tt_member != வெற்று)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    இல்லையெனில்
		tv.vval.v_object = வெற்று;
	    நிறுத்து;
	நிலை VAR_TYPEALIAS: tv.vval.v_typealias = வெற்று; நிறுத்து;
	நிலை VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); நிறுத்து;
	இயல்பு:
	    tv.v_type = VAR_எண்;
	    tv.vval.v_எண் = 0;
	    நிறுத்து;
    }

    எனில் (l->lv_type != வெற்று && l->lv_type->tt_member != வெற்று)
	set_tv_type(&tv, l->lv_type->tt_member);

    திரும்பு list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_index_one(list_T *l, long *n1, முழு can_append, முழு quiet)
{
    long	orig_n1 = *n1;
    listitem_T	*li = list_find_index(l, n1);

    எனில் (li != வெற்று)
	திரும்பு li;

    // Vim9: Allow for adding an item at the end.
    எனில் (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	எனில் (list_append_new_item(l) == FAIL)
	    திரும்பு வெற்று;
	li = list_find_index(l, n1);
    }
    எனில் (li == வெற்று)
    {
	எனில் (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), orig_n1);
	திரும்பு வெற்று;
    }
    திரும்பு li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    முழு
check_range_index_two(
	list_T	    *l,
	long	    *n1,
	listitem_T  *li1,
	long	    *n2,
	முழு	    quiet)
{
    எனில் (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	எனில் (ni == வெற்று)
	{
	    எனில் (!quiet)
		semsg(_(e_list_index_out_of_range_nr), *n2);
	    திரும்பு FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    எனில் (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    எனில் (*n2 < *n1)
    {
	எனில் (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), *n2);
	திரும்பு FAIL;
    }
    திரும்பு OK;
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
    முழு
list_assign_range(
	list_T	    *dest,
	list_T	    *src,
	long	    idx1_arg,
	long	    idx2,
	முழு	    empty_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    long	idx1 = idx1_arg;
    listitem_T	*first_li = list_find_index(dest, &idx1);
    long	idx;
    type_T	*member_type = வெற்று;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = first_li;
    க்கு (src_li = src->lv_first; src_li != வெற்று && dest_li != வெற்று; )
    {
	எனில் (மதிப்பு_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    திரும்பு FAIL;
	src_li = src_li->li_next;
	எனில் (src_li == வெற்று || (!empty_idx2 && idx2 == idx))
	    நிறுத்து;
	dest_li = dest_li->li_next;
	++idx;
    }

    எனில் (in_vim9script() && dest->lv_type != வெற்று
					   && dest->lv_type->tt_member != வெற்று)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    க்கு (src_li = src->lv_first; src_li != வெற்று; )
    {
	எனில் (op != வெற்று && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	இல்லையெனில்
	{
	    எனில் (member_type != வெற்று
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      வெற்று, 0) == FAIL)
		திரும்பு FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_next;
	எனில் (src_li == வெற்று || (!empty_idx2 && idx2 == idx))
	    நிறுத்து;
	எனில் (dest_li->li_next == வெற்று)
	{
	    // Need to add an empty item.
	    எனில் (list_append_எண்(dest, 0) == FAIL)
	    {
		src_li = வெற்று;
		நிறுத்து;
	    }
	}
	dest_li = dest_li->li_next;
	++idx;
    }
    எனில் (src_li != வெற்று)
    {
	emsg(_(e_list_மதிப்பு_has_more_பொருட்கள்_than_targets));
	திரும்பு FAIL;
    }
    எனில் (empty_idx2
	    ? (dest_li != வெற்று && dest_li->li_next != வெற்று)
	    : idx != idx2)
    {
	emsg(_(e_list_மதிப்பு_does_not_have_enough_பொருட்கள்));
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    நிலையான வெறுமை
list_flatten(list_T *list, listitem_T *first, long maxitems, long maxdepth)
{
    listitem_T	*item;
    முழு		done = 0;

    எனில் (maxdepth == 0)
	திரும்பு;
    CHECK_LIST_MATERIALIZE(list);
    எனில் (first == வெற்று)
	item = list->lv_first;
    இல்லையெனில்
	item = first;

    போது (item != வெற்று && done < maxitems)
    {
	listitem_T	*next = item->li_next;

	fast_breakcheck();
	எனில் (got_int)
	    திரும்பு;

	எனில் (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    எனில் (list_extend(list, itemlist, next) == FAIL)
	    {
		list_free_item(list, item);
		திரும்பு;
	    }

	    எனில் (maxdepth > 0)
		list_flatten(list, item->li_prev == வெற்று
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
    நிலையான வெறுமை
flatten_common(typval_T *argvars, typval_T *rettv, முழு make_copy)
{
    list_T  *l;
    long    maxdepth;
    முழு	    error = FALSE;

    எனில் (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	திரும்பு;
    }

    எனில் (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    இல்லையெனில்
    {
	maxdepth = (long)tv_get_எண்_chk(&argvars[1], &error);
	எனில் (error)
	    திரும்பு;
	எனில் (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_எண்));
	    திரும்பு;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    எனில் (l == வெற்று)
	திரும்பு;

    எனில் (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	எனில் (l == வெற்று)
	    திரும்பு;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = வெற்று;
    }
    இல்லையெனில்
    {
	எனில் (மதிப்பு_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    திரும்பு;
	++l->lv_refcount;
    }

    list_flatten(l, வெற்று, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    வெறுமை
f_flatten(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    இல்லையெனில்
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    வெறுமை
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    வெறுமை
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;
    எனில் (l == வெற்று)
	திரும்பு;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    க்கு (idx = 0, li = l->lv_first; li != வெற்று; li = li->li_next, ++idx)
    {
	list_T	    *l2 = list_alloc();

	எனில் (l2 == வெற்று)
	    நிறுத்து;
	எனில் (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    நிறுத்து;
	}
	எனில் (list_append_எண்(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    நிறுத்து;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    வெறுமை
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;
    எனில் (p == வெற்று)
	திரும்பு;  // null string behaves like an empty string

    க்கு (idx = 0; *p != NUL; ++idx)
    {
	முழு	    len = mb_ptr2len(p);
	list_T	    *l2;

	எனில் (len == 0)
	    நிறுத்து;
	l2 = list_alloc();
	எனில் (l2 == வெற்று)
	    நிறுத்து;
	எனில் (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    நிறுத்து;
	}
	எனில் (list_append_எண்(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    நிறுத்து;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    முழு
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*item;
    முழு		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    எனில் (l2 == வெற்று || l2->lv_len == 0)
	திரும்பு OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == வெற்று ? வெற்று : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    க்கு (item = l2->lv_first; item != வெற்று && --todo >= 0;
				 item = item == bef_prev ? bef : item->li_next)
	எனில் (list_செருகு_tv(l1, &item->li_tv, bef) == FAIL)
	    திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    முழு
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    முழு		len1 = l1 == வெற்று ? 0 : l1->lv_len;
    முழு		len2 = l2 == வெற்று ? 0 : l2->lv_len;
    long	totallen = (long)len1 + (long)len2;
    முழு		i;
    listitem_T	*item;

    எனில் (totallen == 0)
    {
	l = list_alloc();
	எனில் (l == வெற்று)
	    திரும்பு FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	திரும்பு OK;
    }
    எனில் (totallen > INT_MAX)
	திரும்பு FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_பொருட்கள்((முழு)totallen);
    எனில் (l == வெற்று)
	திரும்பு FAIL;

    i = 0;
    எனில் (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	க்கு (item = l1->lv_first; item != வெற்று && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    எனில் (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	க்கு (item = l2->lv_first; item != வெற்று && !got_int;
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
    திரும்பு OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	மொத்தம்;
    long	idx;

    எனில் (n2 < n1)
	திரும்பு list_alloc();
    எனில் (n1 > INT_MAX - (n2 - n1) - 1)
	திரும்பு வெற்று;
    மொத்தம் = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_பொருட்கள்((முழு)மொத்தம்);
    எனில் (l == வெற்று)
	திரும்பு வெற்று;

    item = list_find(ol, n1);
    க்கு (idx = 0; idx < மொத்தம்; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	எனில் (item == வெற்று)
	{
	    list_free(l);
	    திரும்பு வெற்று;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (முழு)idx, &new_tv);
	item = item->li_next;
    }
    திரும்பு l;
}

    முழு
list_slice_or_index(
	    list_T	*list,
	    முழு		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    முழு		exclusive,
	    typval_T	*rettv,
	    முழு		verbose)
{
    long	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    எனில் (n1 < 0)
	n1 = len + n1;
    எனில் (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	எனில் (!range)
	{
	    எனில் (verbose)
		semsg(_(e_list_index_out_of_range_nr), (long)n1_arg);
	    திரும்பு FAIL;
	}
	எனில் (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	இல்லையெனில்
	    n1 = len;
    }
    எனில் (range)
    {
	list_T	*l;

	எனில் (n2 < 0)
	    n2 = len + n2;
	இல்லையெனில் எனில் (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	எனில் (exclusive)
	    --n2;
	எனில் (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	எனில் (l == வெற்று)
	    திரும்பு FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    இல்லையெனில்
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long index = n1;
	listitem_T *li = check_range_index_one(list, &index, TRUE, TRUE);
	எனில் (li == வெற்று)
	    திரும்பு FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	எனில் (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != வெற்று
					&& var1.vval.v_list->lv_type == வெற்று)
	{
	    எனில் (list->lv_type != வெற்று && list->lv_type->tt_member != வெற்று)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    திரும்பு OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, முழு deep, முழு உச்சி, முழு copyID)
{
    list_T	*copy;
    listitem_T	*item;
    முழு		idx = 0;

    எனில் (orig == வெற்று)
	திரும்பு வெற்று;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_பொருட்கள்(orig->lv_len);
    எனில் (copy == வெற்று)
	திரும்பு வெற்று;

    எனில் (orig->lv_type == வெற்று || உச்சி || deep)
	copy->lv_type = வெற்று;
    இல்லையெனில்
	copy->lv_type = alloc_type(orig->lv_type);
    எனில் (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    க்கு (item = orig->lv_first; item != வெற்று && !got_int;
	    item = item->li_next)
    {
	typval_T	new_tv;

	எனில் (deep)
	{
	    எனில் (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		நிறுத்து;
	}
	இல்லையெனில்
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    எனில் (item != வெற்று)
    {
	list_unref(copy);
	copy = வெற்று;
    }

    திரும்பு copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    வெறுமை
vimlist_remove(list_T *l, listitem_T *item, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    க்கு (ip = item; ip != வெற்று; ip = ip->li_next)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	எனில் (ip == item2)
	    நிறுத்து;
    }

    எனில் (item2->li_next == வெற்று)
	l->lv_u.mat.lv_last = item->li_prev;
    இல்லையெனில்
	item2->li_next->li_prev = item->li_prev;
    எனில் (item->li_prev == வெற்று)
	l->lv_first = item2->li_next;
    இல்லையெனில்
	item->li_prev->li_next = item2->li_next;
    l->lv_u.mat.lv_idx_item = வெற்று;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, முழு copyID, முழு restore_copyID)
{
    garray_T	ga;

    எனில் (tv->vval.v_list == வெற்று)
	திரும்பு வெற்று;
    ga_init2(&ga, அளவு(எழுத்து), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    எனில் (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	திரும்பு வெற்று;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    திரும்பு (char_u *)ga.ga_data;
}

வகைவரை கட்டமைப்பு join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    நிலையான முழு
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    முழு		echo_style,
    முழு		restore_copyID,
    முழு		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    முழு		i;
    join_T	*p;
    long	sumlen = 0;
    முழு		first = TRUE;
    முழு		prev_lock;
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
    எனில் (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    க்கு (item = l->lv_first; item != வெற்று && !got_int; item = item->li_next)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	எனில் (s.string == வெற்று)
	{
	    l->lv_lock = prev_lock;
	    திரும்பு FAIL;
	}

	s.length = STRLEN(s.string);
	sumlen += (long)s.length;

	(வெறுமை)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	எனில் (tofree != வெற்று || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	இல்லையெனில்
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	வரி_breakcheck();
	எனில் (did_echo_string_emsg)  // recursion error, bail out
	    நிறுத்து;
    }
    l->lv_lock = prev_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    எனில் (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    எனில் (sumlen > INT_MAX - 2 || ga_grow(gap, (முழு)sumlen + 2) == FAIL)
	திரும்பு FAIL;

    க்கு (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	எனில் (first)
	    first = FALSE;
	இல்லையெனில்
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	எனில் (p->s.string != வெற்று)
	    ga_concat_len(gap, p->s.string, p->s.length);
	வரி_breakcheck();
    }

    திரும்பு OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    முழு
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    முழு		echo_style,
    முழு		restore_copyID,
    முழு		copyID)
{
    garray_T	join_ga;
    முழு		retval;
    join_T	*p;
    முழு		i;

    எனில் (l->lv_len < 1)
	திரும்பு OK; // nothing to do
    ga_init2(&join_ga, அளவு(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    எனில் (join_ga.ga_data == வெற்று)
	திரும்பு retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    க்கு (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    திரும்பு retval;
}

/*
 * "join()" function
 */
    வெறுமை
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    எனில் (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	திரும்பு;

    எனில் ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == வெற்று)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == வெற்று))
	திரும்பு;

    எனில் (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    இல்லையெனில்
	sep = tv_get_string_chk(&argvars[1]);

    எனில் (sep != வெற்று)
    {
	ga_init2(&ga, அளவு(எழுத்து), 80);
	எனில் (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	இல்லையெனில்
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
    இல்லையெனில்
	rettv->vval.v_string = வெற்று;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    முழு
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, முழு do_error)
{
    முழு		evaluate = evalarg == வெற்று ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = வெற்று;
    typval_T	tv;
    listitem_T	*item;
    முழு		vim9script = in_vim9script();
    முழு		had_comma;

    எனில் (evaluate)
    {
	l = list_alloc();
	எனில் (l == வெற்று)
	    திரும்பு FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    போது (**arg != ']' && **arg != NUL)
    {
	எனில் (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    செல் failret;
	எனில் (check_typval_is_மதிப்பு(&tv) == FAIL)
	{
	    எனில் (evaluate)
		clear_tv(&tv);
	    செல் failret;
	}
	எனில் (evaluate)
	{
	    item = listitem_alloc();
	    எனில் (item != வெற்று)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    இல்லையெனில்
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	எனில் (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	எனில் (had_comma)
	{
	    எனில் (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		செல் failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	எனில் (**arg == ']')
	    நிறுத்து;

	எனில் (!had_comma)
	{
	    எனில் (do_error)
	    {
		எனில் (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		இல்லையெனில்
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    செல் failret;
	}
    }

    எனில் (**arg != ']')
    {
	எனில் (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	எனில் (evaluate)
	    list_free(l);
	திரும்பு FAIL;
    }

    *arg += 1;
    எனில் (evaluate)
	rettv_list_set(rettv, l);

    திரும்பு OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    முழு
write_list(FILE *fd, list_T *list, முழு இருமம்)
{
    listitem_T	*li;
    முழு		c;
    முழு		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_பொருட்கள்(list, li)
    {
	க்கு (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    எனில் (*s == '\n')
		c = putc(NUL, fd);
	    இல்லையெனில்
		c = putc(*s, fd);
	    எனில் (c == EOF)
	    {
		ret = FAIL;
		நிறுத்து;
	    }
	}
	எனில் (!இருமம் || li->li_next != வெற்று)
	    எனில் (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		நிறுத்து;
	    }
	எனில் (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    நிறுத்து;
	}
    }
    திரும்பு ret;
}

/*
 * Initialize a static list with 10 items.
 */
    வெறுமை
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    முழு	    i;

    CLEAR_POINTER(sl);
    l->lv_first = &sl->sl_பொருட்கள்[0];
    l->lv_u.mat.lv_last = &sl->sl_பொருட்கள்[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    க்கு (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_பொருட்கள்[i];

	எனில் (i == 0)
	    li->li_prev = வெற்று;
	இல்லையெனில்
	    li->li_prev = li - 1;
	எனில் (i == 9)
	    li->li_next = வெற்று;
	இல்லையெனில்
	    li->li_next = li + 1;
    }
}

/*
 * "list2str()" function
 */
    வெறுமை
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    முழு		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (check_for_list_arg(argvars, 0) == FAIL)
	திரும்பு;

    l = argvars[0].vval.v_list;
    எனில் (l == வெற்று)
	திரும்பு;  // empty list results in empty string

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (முழு)tv_get_bool_chk(&argvars[1], வெற்று);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    எனில் (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	முழு	(*char2bytes)(முழு, char_u *);

	எனில் (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	இல்லையெனில்
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_பொருட்கள்(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_எண்(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    இல்லையெனில்
    {
	long len = (long)list_len(l) + 1;

	எனில் (len <= INT_MAX && ga_grow(&ga, (முழு)len) == OK)
	{
	    FOR_ALL_LIST_பொருட்கள்(l, li)
		ga_append(&ga, tv_get_எண்(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_data;
}

/*
 * "list2tuple()" function
 */
    வெறுமை
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = வெற்று;

    எனில் (check_for_list_arg(argvars, 0) == FAIL)
	திரும்பு;

    l = argvars[0].vval.v_list;
    எனில் (l == வெற்று)
	திரும்பு;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    எனில் (rettv_tuple_set_with_பொருட்கள்(rettv, list_len(l)) == FAIL)
	திரும்பு;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ITEM(tuple, TUPLE_LEN(tuple)));
	tuple->tv_பொருட்கள்.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    வெறுமை
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (check_for_tuple_arg(argvars, 0) == FAIL)
	திரும்பு;

    tuple = argvars[0].vval.v_tuple;
    எனில் (tuple == வெற்று)
	திரும்பு;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    க்கு (முழு i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ITEM(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    நிலையான வெறுமை
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*item, *item2;
    listitem_T	*li;
    முழு		error = FALSE;
    long	idx;
    long	end;
    முழு		cnt = 0;
    list_T	*rl;

    எனில் ((l = argvars[0].vval.v_list) == வெற்று
			     || மதிப்பு_check_lock(l->lv_lock, arg_errmsg, TRUE))
	திரும்பு;

    idx = (long)tv_get_எண்_chk(&argvars[1], &error);
    எனில் (error)
	திரும்பு;		// type error: do nothing, errmsg already given

    எனில் ((item = list_find(l, idx)) == வெற்று)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	திரும்பு;
    }

    எனில் (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	திரும்பு;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_எண்_chk(&argvars[2], &error);
    எனில் (error)
	திரும்பு;		// type error: do nothing

    எனில் ((item2 = list_find(l, end)) == வெற்று)
    {
	semsg(_(e_list_index_out_of_range_nr), end);
	திரும்பு;
    }

    க்கு (li = item; li != வெற்று; li = li->li_next)
    {
	++cnt;
	எனில் (li == item2)
	    நிறுத்து;
    }
    எனில் (li == வெற்று)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	திரும்பு;
    }

    vimlist_remove(l, item, item2);
    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    rl = rettv->vval.v_list;

    எனில் (l->lv_with_பொருட்கள் > 0)
    {
	// need to copy the list items and move the value
	போது (item != வெற்று)
	{
	    li = listitem_alloc();
	    எனில் (li == வெற்று)
		திரும்பு;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    எனில் (item == item2)
		நிறுத்து;
	    item = item->li_next;
	}
    }
    இல்லையெனில்
    {
	rl->lv_first = item;
	rl->lv_u.mat.lv_last = item2;
	item->li_prev = வெற்று;
	item2->li_next = வெற்று;
	rl->lv_len = cnt;
    }
}

நிலையான முழு item_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2);
நிலையான முழு item_compare2(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2);

// struct used in the array that's given to qsort()
வகைவரை கட்டமைப்பு
{
    listitem_T	*item;
    முழு		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    ஒன்றிப்பு {
	varnumber_T	inum;	// for "N"
	இரட்டை		fnum;	// for "n" and "f"
    } சாவி;
} sortItem_T;

// struct storing information about current sort
வகைவரை கட்டமைப்பு
{
    முழு		item_compare_ic;
    முழு		item_compare_lc;
    முழு		item_compare_numeric;
    முழு		item_compare_numbers;
    முழு		item_compare_float;
    char_u	*item_compare_func;
    partial_T	*item_compare_partial;
    dict_T	*item_compare_selfdict;
    முழு		item_compare_func_err;
    முழு		item_compare_keep_zero;
    முழு		item_compare_சாவிகள்_ready;  // ptrs[].key is precomputed
} sortinfo_T;
நிலையான sortinfo_T	*sortinfo = வெற்று;
#define ITEM_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    நிலையான முழு
item_compare(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    sortItem_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = வெற்று, *tofree2 = வெற்று;
    முழு		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;
    tv1 = &si1->item->li_tv;
    tv2 = &si2->item->li_tv;

    எனில் (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_சாவிகள்_ready
					? si1->சாவி.inum : tv_to_எண்(tv1);
	varnumber_T	v2 = sortinfo->item_compare_சாவிகள்_ready
					? si2->சாவி.inum : tv_to_எண்(tv2);

	திரும்பு v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    எனில் (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_சாவிகள்_ready
					? si1->சாவி.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_சாவிகள்_ready
					? si2->சாவி.fnum : tv_get_float(tv2);

	திரும்பு v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    எனில் (sortinfo->item_compare_numeric && sortinfo->item_compare_சாவிகள்_ready)
    {
	இரட்டை	n1 = si1->சாவி.fnum;
	இரட்டை	n2 = si2->சாவி.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	எனில் (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	திரும்பு res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    எனில் (tv1->v_type == VAR_STRING)
    {
	எனில் (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	இல்லையெனில்
	    p1 = tv1->vval.v_string;
    }
    இல்லையெனில்
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    எனில் (tv2->v_type == VAR_STRING)
    {
	எனில் (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	இல்லையெனில்
	    p2 = tv2->vval.v_string;
    }
    இல்லையெனில்
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    எனில் (p1 == வெற்று)
	p1 = (char_u *)"";
    எனில் (p2 == வெற்று)
	p2 = (char_u *)"";
    எனில் (!sortinfo->item_compare_numeric)
    {
	எனில் (sortinfo->item_compare_lc)
	    res = strcoll((எழுத்து *)p1, (எழுத்து *)p2);
	இல்லையெனில்
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    இல்லையெனில்
    {
	இரட்டை n1, n2;
	n1 = strtod((எழுத்து *)p1, (எழுத்து **)&p1);
	n2 = strtod((எழுத்து *)p2, (எழுத்து **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    எனில் (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    திரும்பு res;
}

    நிலையான முழு
item_compare2(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2)
{
    sortItem_T  *si1, *si2;
    முழு		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->item_compare_partial;
    funcexe_T	funcexe;
    முழு		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    எனில் (sortinfo->item_compare_func_err)
	திரும்பு 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    எனில் (partial == வெற்று)
	func_name = sortinfo->item_compare_func;
    இல்லையெனில்
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

    எனில் (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    இல்லையெனில்
    {
	varnumber_T	n;

	n = tv_get_எண்_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    எனில் (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    எனில் (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    திரும்பு res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    நிலையான வெறுமை
sort_compute_சாவிகள்(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    எனில் (info->item_compare_numbers)
    {
	க்கு (i = 0; i < len; ++i)
	    ptrs[i].சாவி.inum = tv_to_எண்(&ptrs[i].item->li_tv);
    }
    இல்லையெனில் எனில் (info->item_compare_float)
    {
	க்கு (i = 0; i < len; ++i)
	    ptrs[i].சாவி.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    இல்லையெனில் // info->item_compare_numeric
    {
	க்கு (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    எனில் (tv->v_type == VAR_STRING)
		ptrs[i].சாவி.fnum = 0.0;
	    இல்லையெனில்
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = வெற்று;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].சாவி.fnum = p == வெற்று ? 0.0 : strtod((எழுத்து *)p, வெற்று);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_சாவிகள்_ready = TRUE;
}

/*
 * sort() List "l"
 */
    நிலையான வெறுமை
do_sort(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    எனில் (ptrs == வெற்று)
	திரும்பு;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_பொருட்கள்(l, li)
    {
	ptrs[i].item = li;
	ptrs[i].idx = i;
	++i;
    }

    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = FALSE;
    info->item_compare_சாவிகள்_ready = FALSE;

    எனில் (info->item_compare_func == வெற்று && info->item_compare_partial == வெற்று
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_சாவிகள்(ptrs, len, info);
    // test the compare function
    எனில் ((info->item_compare_func != வெற்று
		|| info->item_compare_partial != வெற்று)
	    && item_compare2((வெறுமை *)&ptrs[0], (வெறுமை *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    இல்லையெனில்
    {
	// Sort the array with item pointers.
	qsort((வெறுமை *)ptrs, (size_t)len, அளவு(sortItem_T),
		info->item_compare_func == வெற்று
		&& info->item_compare_partial == வெற்று
		? item_compare : item_compare2);

	எனில் (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = வெற்று;
	    l->lv_len = 0;
	    க்கு (i = 0; i < len; ++i)
		list_append(l, ptrs[i].item);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    நிலையான வெறுமை
do_uniq(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;
    முழு	(*item_compare_func_ptr)(மாறிலி வெறுமை *, மாறிலி வெறுமை *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    எனில் (ptrs == வெற்று)
	திரும்பு;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != வெற்று
	|| info->item_compare_partial != வெற்று
	? item_compare2 : item_compare;

    க்கு (li = l->lv_first; li != வெற்று && li->li_next != வெற்று;
	    li = li->li_next)
    {
	எனில் (item_compare_func_ptr((வெறுமை *)&li, (வெறுமை *)&li->li_next)
		== 0)
	    ptrs[i++].item = li;
	எனில் (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    நிறுத்து;
	}
    }

    எனில் (!info->item_compare_func_err)
    {
	போது (--i >= 0)
	{
	    li = ptrs[i].item->li_next;
	    ptrs[i].item->li_next = li->li_next;
	    எனில் (li->li_next != வெற்று)
		li->li_next->li_prev = ptrs[i].item;
	    இல்லையெனில்
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
    நிலையான முழு
parse_sort_uniq_args(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = வெற்று;
    info->item_compare_partial = வெற்று;
    info->item_compare_selfdict = வெற்று;
    info->item_compare_சாவிகள்_ready = FALSE;

    எனில் (argvars[1].v_type == VAR_UNKNOWN)
	திரும்பு OK;

    // optional second argument: {func}
    எனில் (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    இல்லையெனில் எனில் (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    இல்லையெனில்
    {
	முழு	    error = FALSE;
	முழு	    nr = 0;

	எனில் (argvars[1].v_type == VAR_எண்)
	{
	    nr = tv_get_எண்_chk(&argvars[1], &error);
	    எனில் (error)
		திரும்பு FAIL;
	    எனில் (nr == 1)
		info->item_compare_ic = TRUE;
	}
	எனில் (nr != 1)
	{
	    எனில் (argvars[1].v_type != VAR_எண்)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    இல்லையெனில் எனில் (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		திரும்பு FAIL;
	    }
	}
	எனில் (info->item_compare_func != வெற்று)
	{
	    எனில் (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = வெற்று;
	    }
	    இல்லையெனில் எனில் (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = வெற்று;
		info->item_compare_numeric = TRUE;
	    }
	    இல்லையெனில் எனில் (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = வெற்று;
		info->item_compare_numbers = TRUE;
	    }
	    இல்லையெனில் எனில் (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = வெற்று;
		info->item_compare_float = TRUE;
	    }
	    இல்லையெனில் எனில் (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = வெற்று;
		info->item_compare_ic = TRUE;
	    }
	    இல்லையெனில் எனில் (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = வெற்று;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	எனில் (check_for_dict_arg(argvars, 2) == FAIL)
	    திரும்பு FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    திரும்பு OK;
}

/*
 * "sort()" or "uniq()" function
 */
    நிலையான வெறுமை
do_sort_uniq(typval_T *argvars, typval_T *rettv, முழு sort)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    long	len;

    எனில் (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	திரும்பு;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    எனில் (l != வெற்று && மதிப்பு_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	செல் theend;
    rettv_list_set(rettv, l);
    எனில் (l == வெற்று)
	செல் theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    எனில் (len <= 1)
	செல் theend;	// short list sorts pretty quickly

    எனில் (parse_sort_uniq_args(argvars, &info) == FAIL)
	செல் theend;

    எனில் (sort)
	do_sort(l, &info);
    இல்லையெனில்
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    வெறுமை
f_sort(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    வெறுமை
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    முழு
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*கோவை,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	முழு		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    முழு		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    எனில் (filtermap == FILTERMAP_FOREACH && கோவை->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(கோவை->vval.v_string);
	எனில் (!did_emsg)
	    retval = OK;
	செல் theend;
    }

    argv[0] = *get_vim_var_tv(VV_சாவி);
    argv[1] = *get_vim_var_tv(VV_VAL);
    எனில் (eval_கோவை_typval(கோவை, FALSE, argv, 2, fc, newtv) == FAIL)
	செல் theend;
    எனில் (filtermap == FILTERMAP_FILTER)
    {
	முழு	    error = FALSE;

	// filter(): when expr is zero remove the item
	எனில் (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	இல்லையெனில்
	    *remp = (tv_get_எண்_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	எனில் (error)
	    செல் theend;
    }
    இல்லையெனில் எனில் (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    திரும்பு retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    நிலையான வெறுமை
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	எழுத்து		*func_name,
	char_u		*arg_errmsg,
	typval_T	*கோவை,
	typval_T	*rettv)
{
    முழு		prev_lock;
    list_T	*l_ret = வெற்று;
    முழு		idx = 0;
    முழு		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    எனில் (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = வெற்று;
    }
    எனில் (l == வெற்று || (filtermap == FILTERMAP_FILTER
			    && மதிப்பு_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	திரும்பு;

    prev_lock = l->lv_lock;

    எனில் (filtermap == FILTERMAP_MAPNEW)
    {
	எனில் (rettv_list_alloc(rettv) == FAIL)
	    திரும்பு;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_சாவி, VAR_எண்);

    எனில் (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_கோவை_get_funccal(கோவை, &newtv);

    எனில் (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	முழு		len = l->lv_len;
	முழு		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	எனில் (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = வெற்று;
	    l->lv_u.mat.lv_last = வெற்று;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = வெற்று;
	}

	க்கு (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_எண்;
	    tv.v_lock = 0;
	    tv.vval.v_எண் = val;
	    set_vim_var_nr(VV_சாவி, idx);
	    எனில் (filter_map_one(&tv, கோவை, filtermap, fc, &newtv, &rem) == FAIL)
		நிறுத்து;
	    எனில் (did_emsg)
	    {
		clear_tv(&newtv);
		நிறுத்து;
	    }
	    எனில் (filtermap != FILTERMAP_FOREACH)
	    {
		எனில் (filtermap != FILTERMAP_FILTER)
		{
		    எனில் (filtermap == FILTERMAP_MAP && argtype != வெற்று
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			நிறுத்து;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    எனில் (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			நிறுத்து;
		}
		இல்லையெனில் எனில் (!rem)
		{
		    // filter(): append the list item value when not rem
		    எனில் (list_append_tv_move(l, &tv) == FAIL)
			நிறுத்து;
		}
	    }

	    val += stride;
	}
    }
    இல்லையெனில்
    {
	// Materialized list: loop over the items
	க்கு (li = l->lv_first; li != வெற்று; li = nli)
	{
	    எனில் (filtermap == FILTERMAP_MAP && மதிப்பு_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		நிறுத்து;
	    nli = li->li_next;
	    set_vim_var_nr(VV_சாவி, idx);
	    எனில் (filter_map_one(&li->li_tv, கோவை, filtermap, fc,
							 &newtv, &rem) == FAIL)
		நிறுத்து;
	    எனில் (did_emsg)
	    {
		clear_tv(&newtv);
		நிறுத்து;
	    }
	    எனில் (filtermap == FILTERMAP_MAP)
	    {
		எனில் (argtype != வெற்று && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    நிறுத்து;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_lock = 0;
		li->li_tv = newtv;
	    }
	    இல்லையெனில் எனில் (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		எனில் (list_append_tv_move(l_ret, &newtv) == FAIL)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = prev_lock;
    எனில் (fc != வெற்று)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    நிலையான வெறுமை
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*கோவை;
    எழுத்து	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    முழு		save_did_emsg;
    type_T	*type = வெற்று;

    // map(), filter(), foreach() return the first argument, also on failure.
    எனில் (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    எனில் (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	திரும்பு;
    }

    எனில் (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	எனில் (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != வெற்று)
	    type = argvars[0].vval.v_dict->dv_type;
	இல்லையெனில் எனில் (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != வெற்று)
	    type = argvars[0].vval.v_list->lv_type;
    }

    எனில் (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	எழுத்து *செய்தி;

	எனில் (filtermap == FILTERMAP_FOREACH)
	    செய்தி = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	இல்லையெனில்
	    செய்தி = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(செய்தி), func_name);
	திரும்பு;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    கோவை = &argvars[1];
    எனில் (கோவை->v_type == VAR_UNKNOWN)
	திரும்பு;

    typval_T	save_val;
    typval_T	save_சாவி;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_சாவி, &save_சாவி);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    எனில் (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, கோவை, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, கோவை,
							    arg_errmsg, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, கோவை, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, கோவை);
    இல்லையெனில் // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, கோவை, rettv);

    restore_vimvar(VV_சாவி, &save_சாவி);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    வெறுமை
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    வெறுமை
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    வெறுமை
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    வெறுமை
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    நிலையான வெறுமை
list_add(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    எனில் (l == வெற்று)
    {
	எனில் (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    இல்லையெனில் எனில் (!மதிப்பு_check_lock(l->lv_lock,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    வெறுமை
f_add(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_எண் = 1; // Default: Failed

    எனில் (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_எண்_arg(argvars, 1) == FAIL)))
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    இல்லையெனில்
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    நிலையான long
list_மொத்தம்(list_T *l, typval_T *needle, long idx, முழு ic)
{
    long	n = 0;
    listitem_T	*li;

    எனில் (l == வெற்று)
	திரும்பு 0;

    CHECK_LIST_MATERIALIZE(l);

    எனில் (list_len(l) == 0)
	திரும்பு 0;

    li = list_find(l, idx);
    எனில் (li == வெற்று)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	திரும்பு 0;
    }

    க்கு ( ; li != வெற்று; li = li->li_next)
	எனில் (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    திரும்பு n;
}

/*
 * "count()" function
 */
    வெறுமை
f_மொத்தம்(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    முழு		ic = FALSE;
    முழு		error = FALSE;

    எனில் (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 3) == FAIL)))
	திரும்பு;

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
	ic = (முழு)tv_get_bool_chk(&argvars[2], &error);

    எனில் (!error && argvars[0].v_type == VAR_STRING)
	n = string_மொத்தம்(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    இல்லையெனில் எனில் (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	எனில் (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_எண்_chk(&argvars[3], &error);
	எனில் (!error)
	    n = list_மொத்தம்(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    இல்லையெனில் எனில் (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	எனில் (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_எண்_chk(&argvars[3], &error);
	எனில் (!error)
	    n = tuple_மொத்தம்(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    இல்லையெனில் எனில் (!error && argvars[0].v_type == VAR_DICT)
    {
	எனில் (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	இல்லையெனில்
	    n = dict_மொத்தம்(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    இல்லையெனில் எனில் (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_எண் = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    நிலையான வெறுமை
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	எழுத்து		*func_name,
	char_u		*arg_errmsg,
	முழு		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*item;
    long	before;
    முழு		error = FALSE;

    l1 = argvars[0].vval.v_list;
    எனில் (l1 == வெற்று)
    {
	emsg(_(e_cannot_extend_null_list));
	திரும்பு;
    }
    எனில் (is_new || !மதிப்பு_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	எனில் (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    எனில் (l1 == வெற்று)
		திரும்பு;
	}

	l2 = argvars[1].vval.v_list;
	எனில் (l2 == வெற்று)
	    செல் theend;

	எனில் (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_எண்_chk(&argvars[2], &error);
	    எனில் (error)
		செல் cleanup;		// type error; errmsg already given
	    எனில் (before == l1->lv_len)
		item = வெற்று;
	    இல்லையெனில்
	    {
		item = list_find(l1, before);
		எனில் (item == வெற்று)
		{
		    semsg(_(e_list_index_out_of_range_nr), before);
		    செல் cleanup;
		}
	    }
	}
	இல்லையெனில்
	    item = வெற்று;
	எனில் (type != வெற்று && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    செல் cleanup;
	list_extend(l1, l2, item);

theend:
	எனில் (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	இல்லையெனில்
	    copy_tv(&argvars[0], rettv);
	திரும்பு;

cleanup:
	எனில் (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    நிலையான வெறுமை
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, முழு is_new)
{
    type_T	*type = வெற்று;
    எழுத்து	*func_name = is_new ? "extendnew()" : "extend()";

    எனில் (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	எனில் (!is_new && in_vim9script() && argvars[0].vval.v_list != வெற்று)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	எனில் (!is_new && in_vim9script() && argvars[0].vval.v_dict != வெற்று)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    இல்லையெனில்
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    வெறுமை
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
    வெறுமை
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    நிலையான வெறுமை
list_செருகு_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    முழு		error = FALSE;

    எனில் (l == வெற்று)
    {
	எனில் (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	திரும்பு;
    }

    எனில் (மதிப்பு_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	திரும்பு;

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_எண்_chk(&argvars[2], &error);
    எனில் (error)
	திரும்பு;		// type error; errmsg already given

    எனில் (before == l->lv_len)
	item = வெற்று;
    இல்லையெனில்
    {
	item = list_find(l, before);
	எனில் (item == வெற்று)
	{
	    semsg(_(e_list_index_out_of_range_nr), before);
	    l = வெற்று;
	}
    }
    எனில் (l != வெற்று)
    {
	(வெறுமை)list_செருகு_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    வெறுமை
f_செருகு(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_எண்_arg(argvars, 1) == FAIL)
		|| check_for_opt_எண்_arg(argvars, 2) == FAIL))
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_BLOB)
	blob_செருகு_func(argvars, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    இல்லையெனில்
	list_செருகு_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    வெறுமை
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    எனில் (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_எண்_arg(argvars, 1) == FAIL
			|| check_for_opt_எண்_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_எண்_arg(argvars, 1) == FAIL)))
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    இல்லையெனில்
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    நிலையான வெறுமை
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    எனில் (l != வெற்று
	    && !மதிப்பு_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	எனில் (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    திரும்பு;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = வெற்று;
	l->lv_len = 0;
	போது (li != வெற்று)
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
    வெறுமை
f_reverse(typval_T *argvars, typval_T *rettv)
{
    எனில் (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	எனில் (argvars[0].vval.v_string != வெற்று)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	இல்லையெனில்
	    rettv->vval.v_string = வெற்று;
    }
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_TUPLE)
	tuple_reverse(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    நிலையான வெறுமை
list_reduce(
	typval_T	*argvars,
	typval_T	*கோவை,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = வெற்று;
    முழு		range_list;
    முழு		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    முழு		r;
    முழு		called_emsg_start = called_emsg;
    முழு		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != வெற்று && l->lv_first == &range_list_item;
    எனில் (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    எனில் (argvars[2].v_type == VAR_UNKNOWN)
    {
	எனில் (l == வெற்று || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_மதிப்பு), "List");
	    திரும்பு;
	}
	எனில் (range_list)
	{
	    initial.v_type = VAR_எண்;
	    initial.vval.v_எண் = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	இல்லையெனில்
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_next;
	}
    }
    இல்லையெனில்
    {
	initial = argvars[2];
	எனில் (l != வெற்று && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    எனில் (l == வெற்று)
	திரும்பு;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_கோவை_get_funccal(கோவை, rettv);

    prev_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    போது (range_list ? range_idx < l->lv_len : li != வெற்று)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	எனில் (range_list)
	{
	    argv[1].v_type = VAR_எண்;
	    argv[1].vval.v_எண் = range_val;
	}
	இல்லையெனில்
	    argv[1] = li->li_tv;

	r = eval_கோவை_typval(கோவை, TRUE, argv, 2, fc, rettv);

	எனில் (argv[0].v_type != VAR_எண் && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	எனில் (r == FAIL || called_emsg != called_emsg_start)
	    நிறுத்து;

	// advance to the next item
	எனில் (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	இல்லையெனில்
	    li = li->li_next;
    }

    எனில் (fc != வெற்று)
	remove_funccal();

    l->lv_lock = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    வெறுமை
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    எனில் (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    இல்லையெனில் எனில் (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    இல்லையெனில்
	func_name = tv_get_string_strict(&argvars[1]);
    எனில் (func_name == வெற்று || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	திரும்பு;
    }

    எனில் (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    இல்லையெனில் எனில் (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    இல்லையெனில்
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    வெறுமை
f_slice(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_opt_எண்_arg(argvars, 2) == FAIL))
	திரும்பு;

    எனில் (check_can_index(&argvars[0], TRUE, FALSE) != OK)
	திரும்பு;

    copy_tv(argvars, rettv);
    eval_index_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? வெற்று : argvars + 2,
	    TRUE, வெற்று, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
