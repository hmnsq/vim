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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// List heads for garbage collection.
ایستا list_T		*first_list = پوچ;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    برای ((lw) = (l)->lv_watch; (lw) != پوچ; (lw) = (lw)->lw_next)

ایستا تهی list_free_item(list_T *l, listitem_T *item);

/*
 * Add a watcher to a list.
 */
    تهی
list_add_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_next = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    تهی
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	اگر (lw == lwrem)
	{
	    *lwp = lw->lw_next;
	    بشکن;
	}
	lwp = &lw->lw_next;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    ایستا تهی
list_fix_watch(list_T *l, listitem_T *item)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	اگر (lw->lw_item == item)
	    lw->lw_item = item->li_next;
}

    ایستا تهی
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    اگر (first_list != پوچ)
	first_list->lv_used_prev = l;
    l->lv_used_prev = پوچ;
    l->lv_used_next = first_list;
    first_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(تهی)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    اگر (l != پوچ)
	list_init(l);
    بازگشت l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(اندازه(list_T)))
	بازگشت پوچ;
#endif
    بازگشت (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_اقلام(صحیح شمار)
{
    list_T	*l;

    اگر (شمار > 0
	    && (size_t)شمار > (SIZE_MAX - اندازه(list_T)) / اندازه(listitem_T))
	بازگشت پوچ;
    l = (list_T *)alloc_clear(اندازه(list_T) + شمار * اندازه(listitem_T));
    اگر (l == پوچ)
	بازگشت پوچ;

    list_init(l);

    اگر (شمار <= 0)
	بازگشت l;

    listitem_T	*li = (listitem_T *)(l + 1);
    صحیح		i;

    l->lv_len = شمار;
    l->lv_with_اقلام = شمار;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + شمار - 1;
    برای (i = 0; i < شمار; ++i)
    {
	اگر (i == 0)
	    li->li_prev = پوچ;
	وگرنه
	    li->li_prev = li - 1;
	اگر (i == شمار - 1)
	    li->li_next = پوچ;
	وگرنه
	    li->li_next = li + 1;
	++li;
    }

    بازگشت l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    تهی
list_set_item(list_T *l, صحیح idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    صحیح
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    اگر (l == پوچ)
	بازگشت FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    بازگشت OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    صحیح
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(اندازه(list_T)))
	بازگشت FAIL;
#endif
    بازگشت rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    تهی
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    اگر (l != پوچ)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    تهی
list_unref(list_T *l)
{
    اگر (l != پوچ && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    ایستا تهی
list_free_contents(list_T *l)
{
    listitem_T *item;

    اگر (l->lv_first != &range_list_item)
	برای (item = l->lv_first; item != پوچ; item = l->lv_first)
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
    صحیح
list_free_nonref(صحیح copyID)
{
    list_T	*ll;
    صحیح		did_free = FALSE;

    برای (ll = first_list; ll != پوچ; ll = ll->lv_used_next)
	اگر ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == پوچ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    بازگشت did_free;
}

    ایستا تهی
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    اگر (l->lv_used_prev == پوچ)
	first_list = l->lv_used_next;
    وگرنه
	l->lv_used_prev->lv_used_next = l->lv_used_next;
    اگر (l->lv_used_next != پوچ)
	l->lv_used_next->lv_used_prev = l->lv_used_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    تهی
list_free_اقلام(صحیح copyID)
{
    list_T	*ll, *ll_next;

    برای (ll = first_list; ll != پوچ; ll = ll_next)
    {
	ll_next = ll->lv_used_next;
	اگر ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == پوچ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    تهی
list_free(list_T *l)
{
    اگر (in_free_unref_اقلام)
	بازگشت;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(تهی)
{
    بازگشت ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    ایستا تهی
list_free_item(list_T *l, listitem_T *item)
{
    اگر (l->lv_with_اقلام == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_اقلام)
	vim_free(item);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    تهی
listitem_free(list_T *l, listitem_T *item)
{
    clear_tv(&item->li_tv);
    list_free_item(l, item);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    تهی
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
    اگر (l == پوچ)
	بازگشت 0L;
    بازگشت l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    صحیح
list_equal(
    list_T	*l1,
    list_T	*l2,
    صحیح		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    اگر (l1 == l2)
	بازگشت TRUE;
    اگر (list_len(l1) != list_len(l2))
	بازگشت FALSE;
    اگر (list_len(l1) == 0)
	// empty and NULL list are considered equal
	بازگشت TRUE;
    اگر (l1 == پوچ || l2 == پوچ)
	بازگشت FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    برای (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != پوچ && item2 != پوچ;
			       item1 = item1->li_next, item2 = item2->li_next)
	اگر (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    بازگشت FALSE;
    بازگشت item1 == پوچ && item2 == پوچ;
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

    اگر (l == پوچ)
	بازگشت پوچ;

    // Negative index is relative to the end.
    اگر (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    اگر (n < 0 || n >= l->lv_len)
	بازگشت پوچ;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    اگر (n >= l->lv_len)
	بازگشت پوچ;

    // When there is a cached index may start search from there.
    اگر (l->lv_u.mat.lv_idx_item != پوچ)
    {
	اگر (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	وگرنه اگر (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	وگرنه
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    وگرنه
    {
	اگر (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	وگرنه
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    وقتی (n > idx)
    {
	// search forward
	item = item->li_next;
	++idx;
    }
    وقتی (n < idx)
    {
	// search backward
	item = item->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    بازگشت item;
}

/*
 * Get list item "l[idx]" as a number.
 */
    long
list_find_nr(
    list_T	*l,
    long	idx,
    صحیح		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    اگر (l != پوچ && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	اگر (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	اگر (n < 0 || n >= l->lv_len)
	{
	    اگر (errorp != پوچ)
		*errorp = TRUE;
	    بازگشت -1L;
	}

	بازگشت l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    اگر (li == پوچ)
    {
	اگر (errorp != پوچ)
	    *errorp = TRUE;
	بازگشت -1L;
    }
    بازگشت (long)tv_get_عدد_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    اگر (li == پوچ)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	بازگشت پوچ;
    }
    بازگشت tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_index(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    اگر (li != پوچ)
	بازگشت li;

    اگر (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    بازگشت li;
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

    اگر (l == پوچ)
	بازگشت -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    برای (li = l->lv_first; li != پوچ && li != item; li = li->li_next)
	++idx;
    اگر (li == پوچ)
	بازگشت -1;
    بازگشت idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    تهی
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    اگر (l->lv_u.mat.lv_last == پوچ)
    {
	// empty list
	l->lv_first = item;
	item->li_prev = پوچ;
    }
    وگرنه
    {
	l->lv_u.mat.lv_last->li_next = item;
	item->li_prev = l->lv_u.mat.lv_last;
    }
    l->lv_u.mat.lv_last = item;
    ++l->lv_len;
    item->li_next = پوچ;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    صحیح
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    اگر (l->lv_type != پوچ && l->lv_type->tt_member != پوچ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      پوچ, 0) == FAIL)
	بازگشت FAIL;
    li = listitem_alloc();
    اگر (li == پوچ)
	بازگشت FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    بازگشت OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    ایستا صحیح
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    اگر (li == پوچ)
	بازگشت FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    بازگشت OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    صحیح
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    اگر (li == پوچ)
	بازگشت FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    بازگشت OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    صحیح
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    اگر (li == پوچ)
	بازگشت FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    بازگشت OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    صحیح
list_append_string(list_T *l, char_u *str, صحیح len)
{
    listitem_T *li = listitem_alloc();

    اگر (li == پوچ)
	بازگشت FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    اگر (str == پوچ)
	li->li_tv.vval.v_string = پوچ;
    وگرنه اگر ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == پوچ)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    صحیح
list_append_عدد(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    اگر (li == پوچ)
	بازگشت FAIL;
    li->li_tv.v_type = VAR_عدد;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_عدد = n;
    list_append(l, li);
    بازگشت OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    صحیح
list_درج_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    اگر (l->lv_type != پوچ && l->lv_type->tt_member != پوچ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      پوچ, 0) == FAIL)
	بازگشت FAIL;
    ni = listitem_alloc();
    اگر (ni == پوچ)
	بازگشت FAIL;
    copy_tv(tv, &ni->li_tv);
    list_درج(l, ni, item);
    بازگشت OK;
}

    تهی
list_درج(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    اگر (item == پوچ)
	// Append new item at end of list.
	list_append(l, ni);
    وگرنه
    {
	// Insert new item before existing item.
	ni->li_prev = item->li_prev;
	ni->li_next = item;
	اگر (item->li_prev == پوچ)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	وگرنه
	{
	    item->li_prev->li_next = ni;
	    l->lv_u.mat.lv_idx_item = پوچ;
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
    ایستا صحیح
list_append_new_item(list_T *l)
{
    typval_T	tv;

    اگر (l->lv_type != پوچ && l->lv_type->tt_member != پوچ)
	tv.v_type = l->lv_type->tt_member->tt_type;
    وگرنه
	tv.v_type = VAR_عدد;
    tv.v_lock = 0;

    انتخاب (tv.v_type)
    {
	حالت VAR_BOOL: tv.vval.v_عدد = VVAL_FALSE; بشکن;
	حالت VAR_SPECIAL: tv.vval.v_عدد = 0; بشکن;
	حالت VAR_عدد: tv.vval.v_عدد = 0; بشکن;
	حالت VAR_FLOAT: tv.vval.v_float = 0; بشکن;
	حالت VAR_STRING: tv.vval.v_string = پوچ; بشکن;
	حالت VAR_BLOB: tv.vval.v_blob = blob_alloc(); بشکن;
	حالت VAR_FUNC: tv.vval.v_string = پوچ; بشکن;
	حالت VAR_PARTIAL: tv.vval.v_partial = پوچ; بشکن;
	حالت VAR_LIST: tv.vval.v_list = list_alloc(); بشکن;
	حالت VAR_DICT: tv.vval.v_dict = dict_alloc(); بشکن;
#ifdef FEAT_JOB_CHANNEL
	حالت VAR_JOB: tv.vval.v_job = پوچ; بشکن;
	حالت VAR_CHANNEL: tv.vval.v_channel = پوچ; بشکن;
#endif
	حالت VAR_CLASS:
	    اگر (l->lv_type != پوچ && l->lv_type->tt_member != پوچ)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    وگرنه
		tv.vval.v_class = پوچ;
	    بشکن;
	حالت VAR_OBJECT:
	    اگر (l->lv_type != پوچ && l->lv_type->tt_member != پوچ)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    وگرنه
		tv.vval.v_object = پوچ;
	    بشکن;
	حالت VAR_TYPEALIAS: tv.vval.v_typealias = پوچ; بشکن;
	حالت VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); بشکن;
	پیشفرض:
	    tv.v_type = VAR_عدد;
	    tv.vval.v_عدد = 0;
	    بشکن;
    }

    اگر (l->lv_type != پوچ && l->lv_type->tt_member != پوچ)
	set_tv_type(&tv, l->lv_type->tt_member);

    بازگشت list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_index_one(list_T *l, long *n1, صحیح can_append, صحیح quiet)
{
    long	orig_n1 = *n1;
    listitem_T	*li = list_find_index(l, n1);

    اگر (li != پوچ)
	بازگشت li;

    // Vim9: Allow for adding an item at the end.
    اگر (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	اگر (list_append_new_item(l) == FAIL)
	    بازگشت پوچ;
	li = list_find_index(l, n1);
    }
    اگر (li == پوچ)
    {
	اگر (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), orig_n1);
	بازگشت پوچ;
    }
    بازگشت li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    صحیح
check_range_index_two(
	list_T	    *l,
	long	    *n1,
	listitem_T  *li1,
	long	    *n2,
	صحیح	    quiet)
{
    اگر (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	اگر (ni == پوچ)
	{
	    اگر (!quiet)
		semsg(_(e_list_index_out_of_range_nr), *n2);
	    بازگشت FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    اگر (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    اگر (*n2 < *n1)
    {
	اگر (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), *n2);
	بازگشت FAIL;
    }
    بازگشت OK;
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
    صحیح
list_assign_range(
	list_T	    *dest,
	list_T	    *src,
	long	    idx1_arg,
	long	    idx2,
	صحیح	    empty_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    long	idx1 = idx1_arg;
    listitem_T	*first_li = list_find_index(dest, &idx1);
    long	idx;
    type_T	*member_type = پوچ;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = first_li;
    برای (src_li = src->lv_first; src_li != پوچ && dest_li != پوچ; )
    {
	اگر (مقدار_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    بازگشت FAIL;
	src_li = src_li->li_next;
	اگر (src_li == پوچ || (!empty_idx2 && idx2 == idx))
	    بشکن;
	dest_li = dest_li->li_next;
	++idx;
    }

    اگر (in_vim9script() && dest->lv_type != پوچ
					   && dest->lv_type->tt_member != پوچ)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    برای (src_li = src->lv_first; src_li != پوچ; )
    {
	اگر (op != پوچ && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	وگرنه
	{
	    اگر (member_type != پوچ
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      پوچ, 0) == FAIL)
		بازگشت FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_next;
	اگر (src_li == پوچ || (!empty_idx2 && idx2 == idx))
	    بشکن;
	اگر (dest_li->li_next == پوچ)
	{
	    // Need to add an empty item.
	    اگر (list_append_عدد(dest, 0) == FAIL)
	    {
		src_li = پوچ;
		بشکن;
	    }
	}
	dest_li = dest_li->li_next;
	++idx;
    }
    اگر (src_li != پوچ)
    {
	emsg(_(e_list_مقدار_has_more_اقلام_than_targets));
	بازگشت FAIL;
    }
    اگر (empty_idx2
	    ? (dest_li != پوچ && dest_li->li_next != پوچ)
	    : idx != idx2)
    {
	emsg(_(e_list_مقدار_does_not_have_enough_اقلام));
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    ایستا تهی
list_flatten(list_T *list, listitem_T *first, long maxitems, long maxdepth)
{
    listitem_T	*item;
    صحیح		done = 0;

    اگر (maxdepth == 0)
	بازگشت;
    CHECK_LIST_MATERIALIZE(list);
    اگر (first == پوچ)
	item = list->lv_first;
    وگرنه
	item = first;

    وقتی (item != پوچ && done < maxitems)
    {
	listitem_T	*next = item->li_next;

	fast_breakcheck();
	اگر (got_int)
	    بازگشت;

	اگر (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    اگر (list_extend(list, itemlist, next) == FAIL)
	    {
		list_free_item(list, item);
		بازگشت;
	    }

	    اگر (maxdepth > 0)
		list_flatten(list, item->li_prev == پوچ
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
    ایستا تهی
flatten_common(typval_T *argvars, typval_T *rettv, صحیح make_copy)
{
    list_T  *l;
    long    maxdepth;
    صحیح	    error = FALSE;

    اگر (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	بازگشت;
    }

    اگر (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    وگرنه
    {
	maxdepth = (long)tv_get_عدد_chk(&argvars[1], &error);
	اگر (error)
	    بازگشت;
	اگر (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_عدد));
	    بازگشت;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    اگر (l == پوچ)
	بازگشت;

    اگر (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	اگر (l == پوچ)
	    بازگشت;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = پوچ;
    }
    وگرنه
    {
	اگر (مقدار_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    بازگشت;
	++l->lv_refcount;
    }

    list_flatten(l, پوچ, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    تهی
f_flatten(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    وگرنه
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    تهی
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    تهی
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;
    اگر (l == پوچ)
	بازگشت;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    برای (idx = 0, li = l->lv_first; li != پوچ; li = li->li_next, ++idx)
    {
	list_T	    *l2 = list_alloc();

	اگر (l2 == پوچ)
	    بشکن;
	اگر (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    بشکن;
	}
	اگر (list_append_عدد(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    بشکن;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    تهی
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;
    اگر (p == پوچ)
	بازگشت;  // null string behaves like an empty string

    برای (idx = 0; *p != NUL; ++idx)
    {
	صحیح	    len = mb_ptr2len(p);
	list_T	    *l2;

	اگر (len == 0)
	    بشکن;
	l2 = list_alloc();
	اگر (l2 == پوچ)
	    بشکن;
	اگر (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    بشکن;
	}
	اگر (list_append_عدد(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    بشکن;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    صحیح
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*item;
    صحیح		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    اگر (l2 == پوچ || l2->lv_len == 0)
	بازگشت OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == پوچ ? پوچ : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    برای (item = l2->lv_first; item != پوچ && --todo >= 0;
				 item = item == bef_prev ? bef : item->li_next)
	اگر (list_درج_tv(l1, &item->li_tv, bef) == FAIL)
	    بازگشت FAIL;
    بازگشت OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    صحیح
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    صحیح		len1 = l1 == پوچ ? 0 : l1->lv_len;
    صحیح		len2 = l2 == پوچ ? 0 : l2->lv_len;
    long	totallen = (long)len1 + (long)len2;
    صحیح		i;
    listitem_T	*item;

    اگر (totallen == 0)
    {
	l = list_alloc();
	اگر (l == پوچ)
	    بازگشت FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	بازگشت OK;
    }
    اگر (totallen > INT_MAX)
	بازگشت FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_اقلام((صحیح)totallen);
    اگر (l == پوچ)
	بازگشت FAIL;

    i = 0;
    اگر (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	برای (item = l1->lv_first; item != پوچ && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    اگر (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	برای (item = l2->lv_first; item != پوچ && !got_int;
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
    بازگشت OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	شمار;
    long	idx;

    اگر (n2 < n1)
	بازگشت list_alloc();
    اگر (n1 > INT_MAX - (n2 - n1) - 1)
	بازگشت پوچ;
    شمار = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_اقلام((صحیح)شمار);
    اگر (l == پوچ)
	بازگشت پوچ;

    item = list_find(ol, n1);
    برای (idx = 0; idx < شمار; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	اگر (item == پوچ)
	{
	    list_free(l);
	    بازگشت پوچ;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (صحیح)idx, &new_tv);
	item = item->li_next;
    }
    بازگشت l;
}

    صحیح
list_slice_or_index(
	    list_T	*list,
	    صحیح		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    صحیح		exclusive,
	    typval_T	*rettv,
	    صحیح		verbose)
{
    long	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    اگر (n1 < 0)
	n1 = len + n1;
    اگر (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	اگر (!range)
	{
	    اگر (verbose)
		semsg(_(e_list_index_out_of_range_nr), (long)n1_arg);
	    بازگشت FAIL;
	}
	اگر (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	وگرنه
	    n1 = len;
    }
    اگر (range)
    {
	list_T	*l;

	اگر (n2 < 0)
	    n2 = len + n2;
	وگرنه اگر (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	اگر (exclusive)
	    --n2;
	اگر (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	اگر (l == پوچ)
	    بازگشت FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    وگرنه
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long index = n1;
	listitem_T *li = check_range_index_one(list, &index, TRUE, TRUE);
	اگر (li == پوچ)
	    بازگشت FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	اگر (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != پوچ
					&& var1.vval.v_list->lv_type == پوچ)
	{
	    اگر (list->lv_type != پوچ && list->lv_type->tt_member != پوچ)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    بازگشت OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, صحیح deep, صحیح بالاترین, صحیح copyID)
{
    list_T	*copy;
    listitem_T	*item;
    صحیح		idx = 0;

    اگر (orig == پوچ)
	بازگشت پوچ;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_اقلام(orig->lv_len);
    اگر (copy == پوچ)
	بازگشت پوچ;

    اگر (orig->lv_type == پوچ || بالاترین || deep)
	copy->lv_type = پوچ;
    وگرنه
	copy->lv_type = alloc_type(orig->lv_type);
    اگر (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    برای (item = orig->lv_first; item != پوچ && !got_int;
	    item = item->li_next)
    {
	typval_T	new_tv;

	اگر (deep)
	{
	    اگر (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		بشکن;
	}
	وگرنه
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    اگر (item != پوچ)
    {
	list_unref(copy);
	copy = پوچ;
    }

    بازگشت copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    تهی
vimlist_remove(list_T *l, listitem_T *item, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    برای (ip = item; ip != پوچ; ip = ip->li_next)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	اگر (ip == item2)
	    بشکن;
    }

    اگر (item2->li_next == پوچ)
	l->lv_u.mat.lv_last = item->li_prev;
    وگرنه
	item2->li_next->li_prev = item->li_prev;
    اگر (item->li_prev == پوچ)
	l->lv_first = item2->li_next;
    وگرنه
	item->li_prev->li_next = item2->li_next;
    l->lv_u.mat.lv_idx_item = پوچ;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, صحیح copyID, صحیح restore_copyID)
{
    garray_T	ga;

    اگر (tv->vval.v_list == پوچ)
	بازگشت پوچ;
    ga_init2(&ga, اندازه(نویسه), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    اگر (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	بازگشت پوچ;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    بازگشت (char_u *)ga.ga_data;
}

تعریفنوع ساختار join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ایستا صحیح
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    صحیح		echo_style,
    صحیح		restore_copyID,
    صحیح		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    صحیح		i;
    join_T	*p;
    long	sumlen = 0;
    صحیح		first = TRUE;
    صحیح		prev_lock;
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
    اگر (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    برای (item = l->lv_first; item != پوچ && !got_int; item = item->li_next)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	اگر (s.string == پوچ)
	{
	    l->lv_lock = prev_lock;
	    بازگشت FAIL;
	}

	s.length = STRLEN(s.string);
	sumlen += (long)s.length;

	(تهی)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	اگر (tofree != پوچ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	وگرنه
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	سطر_breakcheck();
	اگر (did_echo_string_emsg)  // recursion error, bail out
	    بشکن;
    }
    l->lv_lock = prev_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    اگر (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    اگر (sumlen > INT_MAX - 2 || ga_grow(gap, (صحیح)sumlen + 2) == FAIL)
	بازگشت FAIL;

    برای (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	اگر (first)
	    first = FALSE;
	وگرنه
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	اگر (p->s.string != پوچ)
	    ga_concat_len(gap, p->s.string, p->s.length);
	سطر_breakcheck();
    }

    بازگشت OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    صحیح
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    صحیح		echo_style,
    صحیح		restore_copyID,
    صحیح		copyID)
{
    garray_T	join_ga;
    صحیح		retval;
    join_T	*p;
    صحیح		i;

    اگر (l->lv_len < 1)
	بازگشت OK; // nothing to do
    ga_init2(&join_ga, اندازه(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    اگر (join_ga.ga_data == پوچ)
	بازگشت retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    برای (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    بازگشت retval;
}

/*
 * "join()" function
 */
    تهی
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    اگر (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	بازگشت;

    اگر ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == پوچ)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == پوچ))
	بازگشت;

    اگر (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    وگرنه
	sep = tv_get_string_chk(&argvars[1]);

    اگر (sep != پوچ)
    {
	ga_init2(&ga, اندازه(نویسه), 80);
	اگر (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	وگرنه
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
    وگرنه
	rettv->vval.v_string = پوچ;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    صحیح
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحیح do_error)
{
    صحیح		evaluate = evalarg == پوچ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = پوچ;
    typval_T	tv;
    listitem_T	*item;
    صحیح		vim9script = in_vim9script();
    صحیح		had_comma;

    اگر (evaluate)
    {
	l = list_alloc();
	اگر (l == پوچ)
	    بازگشت FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    وقتی (**arg != ']' && **arg != NUL)
    {
	اگر (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    برو failret;
	اگر (check_typval_is_مقدار(&tv) == FAIL)
	{
	    اگر (evaluate)
		clear_tv(&tv);
	    برو failret;
	}
	اگر (evaluate)
	{
	    item = listitem_alloc();
	    اگر (item != پوچ)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    وگرنه
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	اگر (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	اگر (had_comma)
	{
	    اگر (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		برو failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg == ']')
	    بشکن;

	اگر (!had_comma)
	{
	    اگر (do_error)
	    {
		اگر (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		وگرنه
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    برو failret;
	}
    }

    اگر (**arg != ']')
    {
	اگر (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	اگر (evaluate)
	    list_free(l);
	بازگشت FAIL;
    }

    *arg += 1;
    اگر (evaluate)
	rettv_list_set(rettv, l);

    بازگشت OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    صحیح
write_list(FILE *fd, list_T *list, صحیح دودویی)
{
    listitem_T	*li;
    صحیح		c;
    صحیح		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_اقلام(list, li)
    {
	برای (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    اگر (*s == '\n')
		c = putc(NUL, fd);
	    وگرنه
		c = putc(*s, fd);
	    اگر (c == EOF)
	    {
		ret = FAIL;
		بشکن;
	    }
	}
	اگر (!دودویی || li->li_next != پوچ)
	    اگر (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		بشکن;
	    }
	اگر (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    بشکن;
	}
    }
    بازگشت ret;
}

/*
 * Initialize a static list with 10 items.
 */
    تهی
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    صحیح	    i;

    CLEAR_POINTER(sl);
    l->lv_first = &sl->sl_اقلام[0];
    l->lv_u.mat.lv_last = &sl->sl_اقلام[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    برای (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_اقلام[i];

	اگر (i == 0)
	    li->li_prev = پوچ;
	وگرنه
	    li->li_prev = li - 1;
	اگر (i == 9)
	    li->li_next = پوچ;
	وگرنه
	    li->li_next = li + 1;
    }
}

/*
 * "list2str()" function
 */
    تهی
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    صحیح		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	بازگشت;

    l = argvars[0].vval.v_list;
    اگر (l == پوچ)
	بازگشت;  // empty list results in empty string

    اگر (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (صحیح)tv_get_bool_chk(&argvars[1], پوچ);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    اگر (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	صحیح	(*char2bytes)(صحیح, char_u *);

	اگر (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	وگرنه
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_اقلام(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_عدد(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    وگرنه
    {
	long len = (long)list_len(l) + 1;

	اگر (len <= INT_MAX && ga_grow(&ga, (صحیح)len) == OK)
	{
	    FOR_ALL_LIST_اقلام(l, li)
		ga_append(&ga, tv_get_عدد(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_data;
}

/*
 * "list2tuple()" function
 */
    تهی
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = پوچ;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	بازگشت;

    l = argvars[0].vval.v_list;
    اگر (l == پوچ)
	بازگشت;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    اگر (rettv_tuple_set_with_اقلام(rettv, list_len(l)) == FAIL)
	بازگشت;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_اقلام(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ITEM(tuple, TUPLE_LEN(tuple)));
	tuple->tv_اقلام.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    تهی
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (check_for_tuple_arg(argvars, 0) == FAIL)
	بازگشت;

    tuple = argvars[0].vval.v_tuple;
    اگر (tuple == پوچ)
	بازگشت;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    برای (صحیح i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ITEM(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    ایستا تهی
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*item, *item2;
    listitem_T	*li;
    صحیح		error = FALSE;
    long	idx;
    long	end;
    صحیح		cnt = 0;
    list_T	*rl;

    اگر ((l = argvars[0].vval.v_list) == پوچ
			     || مقدار_check_lock(l->lv_lock, arg_errmsg, TRUE))
	بازگشت;

    idx = (long)tv_get_عدد_chk(&argvars[1], &error);
    اگر (error)
	بازگشت;		// type error: do nothing, errmsg already given

    اگر ((item = list_find(l, idx)) == پوچ)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	بازگشت;
    }

    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	بازگشت;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_عدد_chk(&argvars[2], &error);
    اگر (error)
	بازگشت;		// type error: do nothing

    اگر ((item2 = list_find(l, end)) == پوچ)
    {
	semsg(_(e_list_index_out_of_range_nr), end);
	بازگشت;
    }

    برای (li = item; li != پوچ; li = li->li_next)
    {
	++cnt;
	اگر (li == item2)
	    بشکن;
    }
    اگر (li == پوچ)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	بازگشت;
    }

    vimlist_remove(l, item, item2);
    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    rl = rettv->vval.v_list;

    اگر (l->lv_with_اقلام > 0)
    {
	// need to copy the list items and move the value
	وقتی (item != پوچ)
	{
	    li = listitem_alloc();
	    اگر (li == پوچ)
		بازگشت;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    اگر (item == item2)
		بشکن;
	    item = item->li_next;
	}
    }
    وگرنه
    {
	rl->lv_first = item;
	rl->lv_u.mat.lv_last = item2;
	item->li_prev = پوچ;
	item2->li_next = پوچ;
	rl->lv_len = cnt;
    }
}

ایستا صحیح item_compare(ثابت تهی *s1, ثابت تهی *s2);
ایستا صحیح item_compare2(ثابت تهی *s1, ثابت تهی *s2);

// struct used in the array that's given to qsort()
تعریفنوع ساختار
{
    listitem_T	*item;
    صحیح		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    اجتماع {
	varnumber_T	inum;	// for "N"
	دوگانه		fnum;	// for "n" and "f"
    } کلید;
} sortItem_T;

// struct storing information about current sort
تعریفنوع ساختار
{
    صحیح		item_compare_ic;
    صحیح		item_compare_lc;
    صحیح		item_compare_numeric;
    صحیح		item_compare_numbers;
    صحیح		item_compare_float;
    char_u	*item_compare_func;
    partial_T	*item_compare_partial;
    dict_T	*item_compare_selfdict;
    صحیح		item_compare_func_err;
    صحیح		item_compare_keep_zero;
    صحیح		item_compare_کلیدها_ready;  // ptrs[].key is precomputed
} sortinfo_T;
ایستا sortinfo_T	*sortinfo = پوچ;
#define ITEM_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    ایستا صحیح
item_compare(ثابت تهی *s1, ثابت تهی *s2)
{
    sortItem_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = پوچ, *tofree2 = پوچ;
    صحیح		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;
    tv1 = &si1->item->li_tv;
    tv2 = &si2->item->li_tv;

    اگر (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_کلیدها_ready
					? si1->کلید.inum : tv_to_عدد(tv1);
	varnumber_T	v2 = sortinfo->item_compare_کلیدها_ready
					? si2->کلید.inum : tv_to_عدد(tv2);

	بازگشت v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    اگر (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_کلیدها_ready
					? si1->کلید.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_کلیدها_ready
					? si2->کلید.fnum : tv_get_float(tv2);

	بازگشت v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    اگر (sortinfo->item_compare_numeric && sortinfo->item_compare_کلیدها_ready)
    {
	دوگانه	n1 = si1->کلید.fnum;
	دوگانه	n2 = si2->کلید.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	اگر (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	بازگشت res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    اگر (tv1->v_type == VAR_STRING)
    {
	اگر (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	وگرنه
	    p1 = tv1->vval.v_string;
    }
    وگرنه
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    اگر (tv2->v_type == VAR_STRING)
    {
	اگر (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	وگرنه
	    p2 = tv2->vval.v_string;
    }
    وگرنه
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    اگر (p1 == پوچ)
	p1 = (char_u *)"";
    اگر (p2 == پوچ)
	p2 = (char_u *)"";
    اگر (!sortinfo->item_compare_numeric)
    {
	اگر (sortinfo->item_compare_lc)
	    res = strcoll((نویسه *)p1, (نویسه *)p2);
	وگرنه
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    وگرنه
    {
	دوگانه n1, n2;
	n1 = strtod((نویسه *)p1, (نویسه **)&p1);
	n2 = strtod((نویسه *)p2, (نویسه **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    اگر (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    بازگشت res;
}

    ایستا صحیح
item_compare2(ثابت تهی *s1, ثابت تهی *s2)
{
    sortItem_T  *si1, *si2;
    صحیح		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->item_compare_partial;
    funcexe_T	funcexe;
    صحیح		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    اگر (sortinfo->item_compare_func_err)
	بازگشت 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    اگر (partial == پوچ)
	func_name = sortinfo->item_compare_func;
    وگرنه
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

    اگر (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    وگرنه
    {
	varnumber_T	n;

	n = tv_get_عدد_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    اگر (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    اگر (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    بازگشت res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    ایستا تهی
sort_compute_کلیدها(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    اگر (info->item_compare_numbers)
    {
	برای (i = 0; i < len; ++i)
	    ptrs[i].کلید.inum = tv_to_عدد(&ptrs[i].item->li_tv);
    }
    وگرنه اگر (info->item_compare_float)
    {
	برای (i = 0; i < len; ++i)
	    ptrs[i].کلید.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    وگرنه // info->item_compare_numeric
    {
	برای (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    اگر (tv->v_type == VAR_STRING)
		ptrs[i].کلید.fnum = 0.0;
	    وگرنه
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = پوچ;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].کلید.fnum = p == پوچ ? 0.0 : strtod((نویسه *)p, پوچ);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_کلیدها_ready = TRUE;
}

/*
 * sort() List "l"
 */
    ایستا تهی
do_sort(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    اگر (ptrs == پوچ)
	بازگشت;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_اقلام(l, li)
    {
	ptrs[i].item = li;
	ptrs[i].idx = i;
	++i;
    }

    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = FALSE;
    info->item_compare_کلیدها_ready = FALSE;

    اگر (info->item_compare_func == پوچ && info->item_compare_partial == پوچ
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_کلیدها(ptrs, len, info);
    // test the compare function
    اگر ((info->item_compare_func != پوچ
		|| info->item_compare_partial != پوچ)
	    && item_compare2((تهی *)&ptrs[0], (تهی *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    وگرنه
    {
	// Sort the array with item pointers.
	qsort((تهی *)ptrs, (size_t)len, اندازه(sortItem_T),
		info->item_compare_func == پوچ
		&& info->item_compare_partial == پوچ
		? item_compare : item_compare2);

	اگر (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = پوچ;
	    l->lv_len = 0;
	    برای (i = 0; i < len; ++i)
		list_append(l, ptrs[i].item);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    ایستا تهی
do_uniq(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;
    صحیح	(*item_compare_func_ptr)(ثابت تهی *, ثابت تهی *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    اگر (ptrs == پوچ)
	بازگشت;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != پوچ
	|| info->item_compare_partial != پوچ
	? item_compare2 : item_compare;

    برای (li = l->lv_first; li != پوچ && li->li_next != پوچ;
	    li = li->li_next)
    {
	اگر (item_compare_func_ptr((تهی *)&li, (تهی *)&li->li_next)
		== 0)
	    ptrs[i++].item = li;
	اگر (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    بشکن;
	}
    }

    اگر (!info->item_compare_func_err)
    {
	وقتی (--i >= 0)
	{
	    li = ptrs[i].item->li_next;
	    ptrs[i].item->li_next = li->li_next;
	    اگر (li->li_next != پوچ)
		li->li_next->li_prev = ptrs[i].item;
	    وگرنه
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
    ایستا صحیح
parse_sort_uniq_args(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = پوچ;
    info->item_compare_partial = پوچ;
    info->item_compare_selfdict = پوچ;
    info->item_compare_کلیدها_ready = FALSE;

    اگر (argvars[1].v_type == VAR_UNKNOWN)
	بازگشت OK;

    // optional second argument: {func}
    اگر (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    وگرنه اگر (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    وگرنه
    {
	صحیح	    error = FALSE;
	صحیح	    nr = 0;

	اگر (argvars[1].v_type == VAR_عدد)
	{
	    nr = tv_get_عدد_chk(&argvars[1], &error);
	    اگر (error)
		بازگشت FAIL;
	    اگر (nr == 1)
		info->item_compare_ic = TRUE;
	}
	اگر (nr != 1)
	{
	    اگر (argvars[1].v_type != VAR_عدد)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    وگرنه اگر (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		بازگشت FAIL;
	    }
	}
	اگر (info->item_compare_func != پوچ)
	{
	    اگر (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = پوچ;
	    }
	    وگرنه اگر (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = پوچ;
		info->item_compare_numeric = TRUE;
	    }
	    وگرنه اگر (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = پوچ;
		info->item_compare_numbers = TRUE;
	    }
	    وگرنه اگر (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = پوچ;
		info->item_compare_float = TRUE;
	    }
	    وگرنه اگر (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = پوچ;
		info->item_compare_ic = TRUE;
	    }
	    وگرنه اگر (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = پوچ;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	اگر (check_for_dict_arg(argvars, 2) == FAIL)
	    بازگشت FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    بازگشت OK;
}

/*
 * "sort()" or "uniq()" function
 */
    ایستا تهی
do_sort_uniq(typval_T *argvars, typval_T *rettv, صحیح sort)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    long	len;

    اگر (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	بازگشت;

    اگر (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	بازگشت;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    اگر (l != پوچ && مقدار_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	برو theend;
    rettv_list_set(rettv, l);
    اگر (l == پوچ)
	برو theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    اگر (len <= 1)
	برو theend;	// short list sorts pretty quickly

    اگر (parse_sort_uniq_args(argvars, &info) == FAIL)
	برو theend;

    اگر (sort)
	do_sort(l, &info);
    وگرنه
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    تهی
f_sort(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    تهی
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    صحیح
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*عبارت,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	صحیح		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    صحیح		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    اگر (filtermap == FILTERMAP_FOREACH && عبارت->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(عبارت->vval.v_string);
	اگر (!did_emsg)
	    retval = OK;
	برو theend;
    }

    argv[0] = *get_vim_var_tv(VV_کلید);
    argv[1] = *get_vim_var_tv(VV_VAL);
    اگر (eval_عبارت_typval(عبارت, FALSE, argv, 2, fc, newtv) == FAIL)
	برو theend;
    اگر (filtermap == FILTERMAP_FILTER)
    {
	صحیح	    error = FALSE;

	// filter(): when expr is zero remove the item
	اگر (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	وگرنه
	    *remp = (tv_get_عدد_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	اگر (error)
	    برو theend;
    }
    وگرنه اگر (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    بازگشت retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    ایستا تهی
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	نویسه		*func_name,
	char_u		*arg_errmsg,
	typval_T	*عبارت,
	typval_T	*rettv)
{
    صحیح		prev_lock;
    list_T	*l_ret = پوچ;
    صحیح		idx = 0;
    صحیح		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = پوچ;
    }
    اگر (l == پوچ || (filtermap == FILTERMAP_FILTER
			    && مقدار_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	بازگشت;

    prev_lock = l->lv_lock;

    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	اگر (rettv_list_alloc(rettv) == FAIL)
	    بازگشت;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_کلید, VAR_عدد);

    اگر (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_عبارت_get_funccal(عبارت, &newtv);

    اگر (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	صحیح		len = l->lv_len;
	صحیح		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	اگر (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = پوچ;
	    l->lv_u.mat.lv_last = پوچ;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = پوچ;
	}

	برای (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_عدد;
	    tv.v_lock = 0;
	    tv.vval.v_عدد = val;
	    set_vim_var_nr(VV_کلید, idx);
	    اگر (filter_map_one(&tv, عبارت, filtermap, fc, &newtv, &rem) == FAIL)
		بشکن;
	    اگر (did_emsg)
	    {
		clear_tv(&newtv);
		بشکن;
	    }
	    اگر (filtermap != FILTERMAP_FOREACH)
	    {
		اگر (filtermap != FILTERMAP_FILTER)
		{
		    اگر (filtermap == FILTERMAP_MAP && argtype != پوچ
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			بشکن;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    اگر (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			بشکن;
		}
		وگرنه اگر (!rem)
		{
		    // filter(): append the list item value when not rem
		    اگر (list_append_tv_move(l, &tv) == FAIL)
			بشکن;
		}
	    }

	    val += stride;
	}
    }
    وگرنه
    {
	// Materialized list: loop over the items
	برای (li = l->lv_first; li != پوچ; li = nli)
	{
	    اگر (filtermap == FILTERMAP_MAP && مقدار_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		بشکن;
	    nli = li->li_next;
	    set_vim_var_nr(VV_کلید, idx);
	    اگر (filter_map_one(&li->li_tv, عبارت, filtermap, fc,
							 &newtv, &rem) == FAIL)
		بشکن;
	    اگر (did_emsg)
	    {
		clear_tv(&newtv);
		بشکن;
	    }
	    اگر (filtermap == FILTERMAP_MAP)
	    {
		اگر (argtype != پوچ && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    بشکن;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_lock = 0;
		li->li_tv = newtv;
	    }
	    وگرنه اگر (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		اگر (list_append_tv_move(l_ret, &newtv) == FAIL)
		    بشکن;
	    }
	    وگرنه اگر (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = prev_lock;
    اگر (fc != پوچ)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    ایستا تهی
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*عبارت;
    نویسه	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    صحیح		save_did_emsg;
    type_T	*type = پوچ;

    // map(), filter(), foreach() return the first argument, also on failure.
    اگر (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    اگر (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	بازگشت;

    اگر (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	بازگشت;
    }

    اگر (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	اگر (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != پوچ)
	    type = argvars[0].vval.v_dict->dv_type;
	وگرنه اگر (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != پوچ)
	    type = argvars[0].vval.v_list->lv_type;
    }

    اگر (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	نویسه *پیام;

	اگر (filtermap == FILTERMAP_FOREACH)
	    پیام = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	وگرنه
	    پیام = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(پیام), func_name);
	بازگشت;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    عبارت = &argvars[1];
    اگر (عبارت->v_type == VAR_UNKNOWN)
	بازگشت;

    typval_T	save_val;
    typval_T	save_کلید;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_کلید, &save_کلید);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    اگر (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, عبارت, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, عبارت,
							    arg_errmsg, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, عبارت, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, عبارت);
    وگرنه // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, عبارت, rettv);

    restore_vimvar(VV_کلید, &save_کلید);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    تهی
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    تهی
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    تهی
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    تهی
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    ایستا تهی
list_add(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    اگر (l == پوچ)
    {
	اگر (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    وگرنه اگر (!مقدار_check_lock(l->lv_lock,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    تهی
f_add(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_عدد = 1; // Default: Failed

    اگر (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_عدد_arg(argvars, 1) == FAIL)))
	بازگشت;

    اگر (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    وگرنه
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ایستا long
list_شمار(list_T *l, typval_T *needle, long idx, صحیح ic)
{
    long	n = 0;
    listitem_T	*li;

    اگر (l == پوچ)
	بازگشت 0;

    CHECK_LIST_MATERIALIZE(l);

    اگر (list_len(l) == 0)
	بازگشت 0;

    li = list_find(l, idx);
    اگر (li == پوچ)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	بازگشت 0;
    }

    برای ( ; li != پوچ; li = li->li_next)
	اگر (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    بازگشت n;
}

/*
 * "count()" function
 */
    تهی
f_شمار(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    صحیح		ic = FALSE;
    صحیح		error = FALSE;

    اگر (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 3) == FAIL)))
	بازگشت;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
	ic = (صحیح)tv_get_bool_chk(&argvars[2], &error);

    اگر (!error && argvars[0].v_type == VAR_STRING)
	n = string_شمار(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    وگرنه اگر (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	اگر (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_عدد_chk(&argvars[3], &error);
	اگر (!error)
	    n = list_شمار(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    وگرنه اگر (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	اگر (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_عدد_chk(&argvars[3], &error);
	اگر (!error)
	    n = tuple_شمار(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    وگرنه اگر (!error && argvars[0].v_type == VAR_DICT)
    {
	اگر (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	وگرنه
	    n = dict_شمار(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    وگرنه اگر (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_عدد = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ایستا تهی
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	نویسه		*func_name,
	char_u		*arg_errmsg,
	صحیح		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*item;
    long	before;
    صحیح		error = FALSE;

    l1 = argvars[0].vval.v_list;
    اگر (l1 == پوچ)
    {
	emsg(_(e_cannot_extend_null_list));
	بازگشت;
    }
    اگر (is_new || !مقدار_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	اگر (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    اگر (l1 == پوچ)
		بازگشت;
	}

	l2 = argvars[1].vval.v_list;
	اگر (l2 == پوچ)
	    برو theend;

	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_عدد_chk(&argvars[2], &error);
	    اگر (error)
		برو cleanup;		// type error; errmsg already given
	    اگر (before == l1->lv_len)
		item = پوچ;
	    وگرنه
	    {
		item = list_find(l1, before);
		اگر (item == پوچ)
		{
		    semsg(_(e_list_index_out_of_range_nr), before);
		    برو cleanup;
		}
	    }
	}
	وگرنه
	    item = پوچ;
	اگر (type != پوچ && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    برو cleanup;
	list_extend(l1, l2, item);

theend:
	اگر (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	وگرنه
	    copy_tv(&argvars[0], rettv);
	بازگشت;

cleanup:
	اگر (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    ایستا تهی
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, صحیح is_new)
{
    type_T	*type = پوچ;
    نویسه	*func_name = is_new ? "extendnew()" : "extend()";

    اگر (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	اگر (!is_new && in_vim9script() && argvars[0].vval.v_list != پوچ)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    وگرنه اگر (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	اگر (!is_new && in_vim9script() && argvars[0].vval.v_dict != پوچ)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    وگرنه اگر (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    وگرنه
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    تهی
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
    تهی
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    ایستا تهی
list_درج_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    صحیح		error = FALSE;

    اگر (l == پوچ)
    {
	اگر (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	بازگشت;
    }

    اگر (مقدار_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	بازگشت;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_عدد_chk(&argvars[2], &error);
    اگر (error)
	بازگشت;		// type error; errmsg already given

    اگر (before == l->lv_len)
	item = پوچ;
    وگرنه
    {
	item = list_find(l, before);
	اگر (item == پوچ)
	{
	    semsg(_(e_list_index_out_of_range_nr), before);
	    l = پوچ;
	}
    }
    اگر (l != پوچ)
    {
	(تهی)list_درج_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    تهی
f_درج(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_عدد_arg(argvars, 1) == FAIL)
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL))
	بازگشت;

    اگر (argvars[0].v_type == VAR_BLOB)
	blob_درج_func(argvars, rettv);
    وگرنه اگر (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    وگرنه
	list_درج_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    تهی
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    اگر (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_عدد_arg(argvars, 1) == FAIL
			|| check_for_opt_عدد_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_عدد_arg(argvars, 1) == FAIL)))
	بازگشت;

    اگر (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    وگرنه اگر (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    وگرنه اگر (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    وگرنه
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    ایستا تهی
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    اگر (l != پوچ
	    && !مقدار_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	اگر (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    بازگشت;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = پوچ;
	l->lv_len = 0;
	وقتی (li != پوچ)
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
    تهی
f_reverse(typval_T *argvars, typval_T *rettv)
{
    اگر (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	اگر (argvars[0].vval.v_string != پوچ)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	وگرنه
	    rettv->vval.v_string = پوچ;
    }
    وگرنه اگر (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    وگرنه اگر (argvars[0].v_type == VAR_TUPLE)
	tuple_reverse(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    ایستا تهی
list_reduce(
	typval_T	*argvars,
	typval_T	*عبارت,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = پوچ;
    صحیح		range_list;
    صحیح		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    صحیح		r;
    صحیح		called_emsg_start = called_emsg;
    صحیح		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != پوچ && l->lv_first == &range_list_item;
    اگر (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	اگر (l == پوچ || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_مقدار), "List");
	    بازگشت;
	}
	اگر (range_list)
	{
	    initial.v_type = VAR_عدد;
	    initial.vval.v_عدد = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	وگرنه
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_next;
	}
    }
    وگرنه
    {
	initial = argvars[2];
	اگر (l != پوچ && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    اگر (l == پوچ)
	بازگشت;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_عبارت_get_funccal(عبارت, rettv);

    prev_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    وقتی (range_list ? range_idx < l->lv_len : li != پوچ)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	اگر (range_list)
	{
	    argv[1].v_type = VAR_عدد;
	    argv[1].vval.v_عدد = range_val;
	}
	وگرنه
	    argv[1] = li->li_tv;

	r = eval_عبارت_typval(عبارت, TRUE, argv, 2, fc, rettv);

	اگر (argv[0].v_type != VAR_عدد && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	اگر (r == FAIL || called_emsg != called_emsg_start)
	    بشکن;

	// advance to the next item
	اگر (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	وگرنه
	    li = li->li_next;
    }

    اگر (fc != پوچ)
	remove_funccal();

    l->lv_lock = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    تهی
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    اگر (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    وگرنه اگر (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    وگرنه
	func_name = tv_get_string_strict(&argvars[1]);
    اگر (func_name == پوچ || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	بازگشت;
    }

    اگر (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    وگرنه اگر (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    وگرنه اگر (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    وگرنه
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    تهی
f_slice(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_عدد_arg(argvars, 2) == FAIL))
	بازگشت;

    اگر (check_can_index(&argvars[0], TRUE, FALSE) != OK)
	بازگشت;

    copy_tv(argvars, rettv);
    eval_index_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? پوچ : argvars + 2,
	    TRUE, پوچ, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
