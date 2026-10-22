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

#ضمن "vim.h"

#אם defined(FEAT_EVAL)

// List heads for garbage collection.
ساكن list_T		*first_list = عدم;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    עבור ((lw) = (l)->lv_watch; (lw) != عدم; (lw) = (lw)->lw_next)

ساكن فراغ list_free_item(list_T *l, listitem_T *item);

/*
 * Add a watcher to a list.
 */
    فراغ
list_add_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_next = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    فراغ
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	אם (lw == lwrem)
	{
	    *lwp = lw->lw_next;
	    اكسر;
	}
	lwp = &lw->lw_next;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    ساكن فراغ
list_fix_watch(list_T *l, listitem_T *item)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	אם (lw->lw_item == item)
	    lw->lw_item = item->li_next;
}

    ساكن فراغ
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    אם (first_list != عدم)
	first_list->lv_used_prev = l;
    l->lv_used_prev = عدم;
    l->lv_used_next = first_list;
    first_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(فراغ)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    אם (l != عدم)
	list_init(l);
    החזר l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    אם (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	החזר عدم;
#endif
    החזר (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_items(صحيح count)
{
    list_T	*l;

    אם (count > 0
	    && (size_t)count > (SIZE_MAX - حجم(list_T)) / حجم(listitem_T))
	החזר عدم;
    l = (list_T *)alloc_clear(حجم(list_T) + count * حجم(listitem_T));
    אם (l == عدم)
	החזר عدم;

    list_init(l);

    אם (count <= 0)
	החזר l;

    listitem_T	*li = (listitem_T *)(l + 1);
    صحيح		i;

    l->lv_len = count;
    l->lv_with_items = count;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + count - 1;
    עבור (i = 0; i < count; ++i)
    {
	אם (i == 0)
	    li->li_prev = عدم;
	אחרת
	    li->li_prev = li - 1;
	אם (i == count - 1)
	    li->li_next = عدم;
	אחרת
	    li->li_next = li + 1;
	++li;
    }

    החזר l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    فراغ
list_set_item(list_T *l, صحيح idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    صحيح
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    אם (l == عدم)
	החזר FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    החזר OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    صحيح
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    אם (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	החזר FAIL;
#endif
    החזר rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    فراغ
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    אם (l != عدم)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    فراغ
list_unref(list_T *l)
{
    אם (l != عدم && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    ساكن فراغ
list_free_contents(list_T *l)
{
    listitem_T *item;

    אם (l->lv_first != &range_list_item)
	עבור (item = l->lv_first; item != عدم; item = l->lv_first)
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
    صحيح
list_free_nonref(صحيح copyID)
{
    list_T	*ll;
    صحيح		did_free = FALSE;

    עבור (ll = first_list; ll != عدم; ll = ll->lv_used_next)
	אם ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == عدم)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    החזר did_free;
}

    ساكن فراغ
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    אם (l->lv_used_prev == عدم)
	first_list = l->lv_used_next;
    אחרת
	l->lv_used_prev->lv_used_next = l->lv_used_next;
    אם (l->lv_used_next != عدم)
	l->lv_used_next->lv_used_prev = l->lv_used_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    فراغ
list_free_items(صحيح copyID)
{
    list_T	*ll, *ll_next;

    עבור (ll = first_list; ll != عدم; ll = ll_next)
    {
	ll_next = ll->lv_used_next;
	אם ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == عدم)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    فراغ
list_free(list_T *l)
{
    אם (in_free_unref_items)
	החזר;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(فراغ)
{
    החזר ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    ساكن فراغ
list_free_item(list_T *l, listitem_T *item)
{
    אם (l->lv_with_items == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_items)
	vim_free(item);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    فراغ
listitem_free(list_T *l, listitem_T *item)
{
    clear_tv(&item->li_tv);
    list_free_item(l, item);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    فراغ
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
    אם (l == عدم)
	החזר 0L;
    החזר l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    صحيح
list_equal(
    list_T	*l1,
    list_T	*l2,
    صحيح		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    אם (l1 == l2)
	החזר TRUE;
    אם (list_len(l1) != list_len(l2))
	החזר FALSE;
    אם (list_len(l1) == 0)
	// empty and NULL list are considered equal
	החזר TRUE;
    אם (l1 == عدم || l2 == عدم)
	החזר FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    עבור (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != عدم && item2 != عدم;
			       item1 = item1->li_next, item2 = item2->li_next)
	אם (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    החזר FALSE;
    החזר item1 == عدم && item2 == عدم;
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

    אם (l == عدم)
	החזר عدم;

    // Negative index is relative to the end.
    אם (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    אם (n < 0 || n >= l->lv_len)
	החזר عدم;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    אם (n >= l->lv_len)
	החזר عدم;

    // When there is a cached index may start search from there.
    אם (l->lv_u.mat.lv_idx_item != عدم)
    {
	אם (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	אחרת אם (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	אחרת
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    אחרת
    {
	אם (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	אחרת
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    כלעוד (n > idx)
    {
	// search forward
	item = item->li_next;
	++idx;
    }
    כלעוד (n < idx)
    {
	// search backward
	item = item->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    החזר item;
}

/*
 * Get list item "l[idx]" as a number.
 */
    long
list_find_nr(
    list_T	*l,
    long	idx,
    صحيح		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    אם (l != عدم && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	אם (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	אם (n < 0 || n >= l->lv_len)
	{
	    אם (errorp != عدم)
		*errorp = TRUE;
	    החזר -1L;
	}

	החזר l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    אם (li == عدم)
    {
	אם (errorp != عدم)
	    *errorp = TRUE;
	החזר -1L;
    }
    החזר (long)tv_get_number_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    אם (li == عدم)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	החזר عدم;
    }
    החזר tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_index(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    אם (li != عدم)
	החזר li;

    אם (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    החזר li;
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

    אם (l == عدم)
	החזר -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    עבור (li = l->lv_first; li != عدم && li != item; li = li->li_next)
	++idx;
    אם (li == عدم)
	החזר -1;
    החזר idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    فراغ
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    אם (l->lv_u.mat.lv_last == عدم)
    {
	// empty list
	l->lv_first = item;
	item->li_prev = عدم;
    }
    אחרת
    {
	l->lv_u.mat.lv_last->li_next = item;
	item->li_prev = l->lv_u.mat.lv_last;
    }
    l->lv_u.mat.lv_last = item;
    ++l->lv_len;
    item->li_next = عدم;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    صحيح
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    אם (l->lv_type != عدم && l->lv_type->tt_member != عدم
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      عدم, 0) == FAIL)
	החזר FAIL;
    li = listitem_alloc();
    אם (li == عدم)
	החזר FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    החזר OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    ساكن صحيح
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    אם (li == عدم)
	החזר FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    החזר OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    صحيح
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    אם (li == عدم)
	החזר FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    החזר OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    صحيح
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    אם (li == عدم)
	החזר FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    החזר OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    صحيح
list_append_string(list_T *l, char_u *str, صحيح len)
{
    listitem_T *li = listitem_alloc();

    אם (li == عدم)
	החזר FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    אם (str == عدم)
	li->li_tv.vval.v_string = عدم;
    אחרת אם ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == عدم)
	החזר FAIL;
    החזר OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    صحيح
list_append_number(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    אם (li == عدم)
	החזר FAIL;
    li->li_tv.v_type = VAR_NUMBER;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_number = n;
    list_append(l, li);
    החזר OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    صحيح
list_insert_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    אם (l->lv_type != عدم && l->lv_type->tt_member != عدم
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      عدم, 0) == FAIL)
	החזר FAIL;
    ni = listitem_alloc();
    אם (ni == عدم)
	החזר FAIL;
    copy_tv(tv, &ni->li_tv);
    list_insert(l, ni, item);
    החזר OK;
}

    فراغ
list_insert(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    אם (item == عدم)
	// Append new item at end of list.
	list_append(l, ni);
    אחרת
    {
	// Insert new item before existing item.
	ni->li_prev = item->li_prev;
	ni->li_next = item;
	אם (item->li_prev == عدم)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	אחרת
	{
	    item->li_prev->li_next = ni;
	    l->lv_u.mat.lv_idx_item = عدم;
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
    ساكن صحيح
list_append_new_item(list_T *l)
{
    typval_T	tv;

    אם (l->lv_type != عدم && l->lv_type->tt_member != عدم)
	tv.v_type = l->lv_type->tt_member->tt_type;
    אחרת
	tv.v_type = VAR_NUMBER;
    tv.v_lock = 0;

    بدل (tv.v_type)
    {
	حالة VAR_BOOL: tv.vval.v_number = VVAL_FALSE; اكسر;
	حالة VAR_SPECIAL: tv.vval.v_number = 0; اكسر;
	حالة VAR_NUMBER: tv.vval.v_number = 0; اكسر;
	حالة VAR_FLOAT: tv.vval.v_float = 0; اكسر;
	حالة VAR_STRING: tv.vval.v_string = عدم; اكسر;
	حالة VAR_BLOB: tv.vval.v_blob = blob_alloc(); اكسر;
	حالة VAR_FUNC: tv.vval.v_string = عدم; اكسر;
	حالة VAR_PARTIAL: tv.vval.v_partial = عدم; اكسر;
	حالة VAR_LIST: tv.vval.v_list = list_alloc(); اكسر;
	حالة VAR_DICT: tv.vval.v_dict = dict_alloc(); اكسر;
#ifdef FEAT_JOB_CHANNEL
	حالة VAR_JOB: tv.vval.v_job = عدم; اكسر;
	حالة VAR_CHANNEL: tv.vval.v_channel = عدم; اكسر;
#endif
	حالة VAR_CLASS:
	    אם (l->lv_type != عدم && l->lv_type->tt_member != عدم)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    אחרת
		tv.vval.v_class = عدم;
	    اكسر;
	حالة VAR_OBJECT:
	    אם (l->lv_type != عدم && l->lv_type->tt_member != عدم)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    אחרת
		tv.vval.v_object = عدم;
	    اكسر;
	حالة VAR_TYPEALIAS: tv.vval.v_typealias = عدم; اكسر;
	حالة VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); اكسر;
	افتراضي:
	    tv.v_type = VAR_NUMBER;
	    tv.vval.v_number = 0;
	    اكسر;
    }

    אם (l->lv_type != عدم && l->lv_type->tt_member != عدم)
	set_tv_type(&tv, l->lv_type->tt_member);

    החזר list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_index_one(list_T *l, long *n1, صحيح can_append, صحيح quiet)
{
    long	orig_n1 = *n1;
    listitem_T	*li = list_find_index(l, n1);

    אם (li != عدم)
	החזר li;

    // Vim9: Allow for adding an item at the end.
    אם (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	אם (list_append_new_item(l) == FAIL)
	    החזר عدم;
	li = list_find_index(l, n1);
    }
    אם (li == عدم)
    {
	אם (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), orig_n1);
	החזר عدم;
    }
    החזר li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    صحيح
check_range_index_two(
	list_T	    *l,
	long	    *n1,
	listitem_T  *li1,
	long	    *n2,
	صحيح	    quiet)
{
    אם (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	אם (ni == عدم)
	{
	    אם (!quiet)
		semsg(_(e_list_index_out_of_range_nr), *n2);
	    החזר FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    אם (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    אם (*n2 < *n1)
    {
	אם (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), *n2);
	החזר FAIL;
    }
    החזר OK;
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
    صحيح
list_assign_range(
	list_T	    *dest,
	list_T	    *src,
	long	    idx1_arg,
	long	    idx2,
	صحيح	    empty_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    long	idx1 = idx1_arg;
    listitem_T	*first_li = list_find_index(dest, &idx1);
    long	idx;
    type_T	*member_type = عدم;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = first_li;
    עבור (src_li = src->lv_first; src_li != عدم && dest_li != عدم; )
    {
	אם (value_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    החזר FAIL;
	src_li = src_li->li_next;
	אם (src_li == عدم || (!empty_idx2 && idx2 == idx))
	    اكسر;
	dest_li = dest_li->li_next;
	++idx;
    }

    אם (in_vim9script() && dest->lv_type != عدم
					   && dest->lv_type->tt_member != عدم)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    עבור (src_li = src->lv_first; src_li != عدم; )
    {
	אם (op != عدم && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	אחרת
	{
	    אם (member_type != عدم
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      عدم, 0) == FAIL)
		החזר FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_next;
	אם (src_li == عدم || (!empty_idx2 && idx2 == idx))
	    اكسر;
	אם (dest_li->li_next == عدم)
	{
	    // Need to add an empty item.
	    אם (list_append_number(dest, 0) == FAIL)
	    {
		src_li = عدم;
		اكسر;
	    }
	}
	dest_li = dest_li->li_next;
	++idx;
    }
    אם (src_li != عدم)
    {
	emsg(_(e_list_value_has_more_items_than_targets));
	החזר FAIL;
    }
    אם (empty_idx2
	    ? (dest_li != عدم && dest_li->li_next != عدم)
	    : idx != idx2)
    {
	emsg(_(e_list_value_does_not_have_enough_items));
	החזר FAIL;
    }
    החזר OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    ساكن فراغ
list_flatten(list_T *list, listitem_T *first, long maxitems, long maxdepth)
{
    listitem_T	*item;
    صحيح		done = 0;

    אם (maxdepth == 0)
	החזר;
    CHECK_LIST_MATERIALIZE(list);
    אם (first == عدم)
	item = list->lv_first;
    אחרת
	item = first;

    כלעוד (item != عدم && done < maxitems)
    {
	listitem_T	*next = item->li_next;

	fast_breakcheck();
	אם (got_int)
	    החזר;

	אם (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    אם (list_extend(list, itemlist, next) == FAIL)
	    {
		list_free_item(list, item);
		החזר;
	    }

	    אם (maxdepth > 0)
		list_flatten(list, item->li_prev == عدم
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
    ساكن فراغ
flatten_common(typval_T *argvars, typval_T *rettv, صحيح make_copy)
{
    list_T  *l;
    long    maxdepth;
    صحيح	    error = FALSE;

    אם (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	החזר;

    אם (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	החזר;
    }

    אם (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    אחרת
    {
	maxdepth = (long)tv_get_number_chk(&argvars[1], &error);
	אם (error)
	    החזר;
	אם (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_number));
	    החזר;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    אם (l == عدم)
	החזר;

    אם (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	אם (l == عدم)
	    החזר;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = عدم;
    }
    אחרת
    {
	אם (value_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    החזר;
	++l->lv_refcount;
    }

    list_flatten(l, عدم, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    فراغ
f_flatten(typval_T *argvars, typval_T *rettv)
{
    אם (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    אחרת
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    فراغ
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    فراغ
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    אם (rettv_list_alloc(rettv) == FAIL)
	החזר;
    אם (l == عدم)
	החזר;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    עבור (idx = 0, li = l->lv_first; li != عدم; li = li->li_next, ++idx)
    {
	list_T	    *l2 = list_alloc();

	אם (l2 == عدم)
	    اكسر;
	אם (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    اكسر;
	}
	אם (list_append_number(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    اكسر;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    فراغ
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    אם (rettv_list_alloc(rettv) == FAIL)
	החזר;
    אם (p == عدم)
	החזר;  // null string behaves like an empty string

    עבור (idx = 0; *p != NUL; ++idx)
    {
	صحيح	    len = mb_ptr2len(p);
	list_T	    *l2;

	אם (len == 0)
	    اكسر;
	l2 = list_alloc();
	אם (l2 == عدم)
	    اكسر;
	אם (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    اكسر;
	}
	אם (list_append_number(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    اكسر;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    صحيح
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*item;
    صحيح		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    אם (l2 == عدم || l2->lv_len == 0)
	החזר OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == عدم ? عدم : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    עבור (item = l2->lv_first; item != عدم && --todo >= 0;
				 item = item == bef_prev ? bef : item->li_next)
	אם (list_insert_tv(l1, &item->li_tv, bef) == FAIL)
	    החזר FAIL;
    החזר OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    صحيح
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    صحيح		len1 = l1 == عدم ? 0 : l1->lv_len;
    صحيح		len2 = l2 == عدم ? 0 : l2->lv_len;
    long	totallen = (long)len1 + (long)len2;
    صحيح		i;
    listitem_T	*item;

    אם (totallen == 0)
    {
	l = list_alloc();
	אם (l == عدم)
	    החזר FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	החזר OK;
    }
    אם (totallen > INT_MAX)
	החזר FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_items((صحيح)totallen);
    אם (l == عدم)
	החזר FAIL;

    i = 0;
    אם (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	עבור (item = l1->lv_first; item != عدم && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    אם (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	עבור (item = l2->lv_first; item != عدم && !got_int;
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
    החזר OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	count;
    long	idx;

    אם (n2 < n1)
	החזר list_alloc();
    אם (n1 > INT_MAX - (n2 - n1) - 1)
	החזר عدم;
    count = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_items((صحيح)count);
    אם (l == عدم)
	החזר عدم;

    item = list_find(ol, n1);
    עבור (idx = 0; idx < count; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	אם (item == عدم)
	{
	    list_free(l);
	    החזר عدم;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (صحيح)idx, &new_tv);
	item = item->li_next;
    }
    החזר l;
}

    صحيح
list_slice_or_index(
	    list_T	*list,
	    صحيح		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    صحيح		exclusive,
	    typval_T	*rettv,
	    صحيح		verbose)
{
    long	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    אם (n1 < 0)
	n1 = len + n1;
    אם (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	אם (!range)
	{
	    אם (verbose)
		semsg(_(e_list_index_out_of_range_nr), (long)n1_arg);
	    החזר FAIL;
	}
	אם (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	אחרת
	    n1 = len;
    }
    אם (range)
    {
	list_T	*l;

	אם (n2 < 0)
	    n2 = len + n2;
	אחרת אם (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	אם (exclusive)
	    --n2;
	אם (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	אם (l == عدم)
	    החזר FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    אחרת
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long index = n1;
	listitem_T *li = check_range_index_one(list, &index, TRUE, TRUE);
	אם (li == عدم)
	    החזר FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	אם (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != عدم
					&& var1.vval.v_list->lv_type == عدم)
	{
	    אם (list->lv_type != عدم && list->lv_type->tt_member != عدم)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    החזר OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, صحيح deep, صحيح top, صحيح copyID)
{
    list_T	*copy;
    listitem_T	*item;
    صحيح		idx = 0;

    אם (orig == عدم)
	החזר عدم;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_items(orig->lv_len);
    אם (copy == عدم)
	החזר عدم;

    אם (orig->lv_type == عدم || top || deep)
	copy->lv_type = عدم;
    אחרת
	copy->lv_type = alloc_type(orig->lv_type);
    אם (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    עבור (item = orig->lv_first; item != عدم && !got_int;
	    item = item->li_next)
    {
	typval_T	new_tv;

	אם (deep)
	{
	    אם (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		اكسر;
	}
	אחרת
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    אם (item != عدم)
    {
	list_unref(copy);
	copy = عدم;
    }

    החזר copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    فراغ
vimlist_remove(list_T *l, listitem_T *item, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    עבור (ip = item; ip != عدم; ip = ip->li_next)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	אם (ip == item2)
	    اكسر;
    }

    אם (item2->li_next == عدم)
	l->lv_u.mat.lv_last = item->li_prev;
    אחרת
	item2->li_next->li_prev = item->li_prev;
    אם (item->li_prev == عدم)
	l->lv_first = item2->li_next;
    אחרת
	item->li_prev->li_next = item2->li_next;
    l->lv_u.mat.lv_idx_item = عدم;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, صحيح copyID, صحيح restore_copyID)
{
    garray_T	ga;

    אם (tv->vval.v_list == عدم)
	החזר عدم;
    ga_init2(&ga, حجم(حرف), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    אם (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	החזר عدم;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    החזר (char_u *)ga.ga_data;
}

تعريفنوع بنية join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ساكن صحيح
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    صحيح		echo_style,
    صحيح		restore_copyID,
    صحيح		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    صحيح		i;
    join_T	*p;
    long	sumlen = 0;
    صحيح		first = TRUE;
    صحيح		prev_lock;
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
    אם (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    עבור (item = l->lv_first; item != عدم && !got_int; item = item->li_next)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	אם (s.string == عدم)
	{
	    l->lv_lock = prev_lock;
	    החזר FAIL;
	}

	s.length = STRLEN(s.string);
	sumlen += (long)s.length;

	(فراغ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	אם (tofree != عدم || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	אחרת
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	line_breakcheck();
	אם (did_echo_string_emsg)  // recursion error, bail out
	    اكسر;
    }
    l->lv_lock = prev_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    אם (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    אם (sumlen > INT_MAX - 2 || ga_grow(gap, (صحيح)sumlen + 2) == FAIL)
	החזר FAIL;

    עבור (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	אם (first)
	    first = FALSE;
	אחרת
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	אם (p->s.string != عدم)
	    ga_concat_len(gap, p->s.string, p->s.length);
	line_breakcheck();
    }

    החזר OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    صحيح
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    صحيح		echo_style,
    صحيح		restore_copyID,
    صحيح		copyID)
{
    garray_T	join_ga;
    صحيح		retval;
    join_T	*p;
    صحيح		i;

    אם (l->lv_len < 1)
	החזר OK; // nothing to do
    ga_init2(&join_ga, حجم(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    אם (join_ga.ga_data == عدم)
	החזר retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    עבור (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    החזר retval;
}

/*
 * "join()" function
 */
    فراغ
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    אם (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	החזר;

    אם ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == عدم)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == عدم))
	החזר;

    אם (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    אחרת
	sep = tv_get_string_chk(&argvars[1]);

    אם (sep != عدم)
    {
	ga_init2(&ga, حجم(حرف), 80);
	אם (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	אחרת
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
    אחרת
	rettv->vval.v_string = عدم;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    صحيح
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحيح do_error)
{
    صحيح		evaluate = evalarg == عدم ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = عدم;
    typval_T	tv;
    listitem_T	*item;
    صحيح		vim9script = in_vim9script();
    صحيح		had_comma;

    אם (evaluate)
    {
	l = list_alloc();
	אם (l == عدم)
	    החזר FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    כלעוד (**arg != ']' && **arg != NUL)
    {
	אם (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    اذهب failret;
	אם (check_typval_is_value(&tv) == FAIL)
	{
	    אם (evaluate)
		clear_tv(&tv);
	    اذهب failret;
	}
	אם (evaluate)
	{
	    item = listitem_alloc();
	    אם (item != عدم)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    אחרת
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	אם (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	אם (had_comma)
	{
	    אם (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		اذهب failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	אם (**arg == ']')
	    اكسر;

	אם (!had_comma)
	{
	    אם (do_error)
	    {
		אם (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		אחרת
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    اذهب failret;
	}
    }

    אם (**arg != ']')
    {
	אם (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	אם (evaluate)
	    list_free(l);
	החזר FAIL;
    }

    *arg += 1;
    אם (evaluate)
	rettv_list_set(rettv, l);

    החזר OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    صحيح
write_list(FILE *fd, list_T *list, صحيح binary)
{
    listitem_T	*li;
    صحيح		c;
    صحيح		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ITEMS(list, li)
    {
	עבור (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    אם (*s == '\n')
		c = putc(NUL, fd);
	    אחרת
		c = putc(*s, fd);
	    אם (c == EOF)
	    {
		ret = FAIL;
		اكسر;
	    }
	}
	אם (!binary || li->li_next != عدم)
	    אם (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		اكسر;
	    }
	אם (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    اكسر;
	}
    }
    החזר ret;
}

/*
 * Initialize a static list with 10 items.
 */
    فراغ
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    صحيح	    i;

    CLEAR_POINTER(sl);
    l->lv_first = &sl->sl_items[0];
    l->lv_u.mat.lv_last = &sl->sl_items[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    עבור (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_items[i];

	אם (i == 0)
	    li->li_prev = عدم;
	אחרת
	    li->li_prev = li - 1;
	אם (i == 9)
	    li->li_next = عدم;
	אחרת
	    li->li_next = li + 1;
    }
}

/*
 * "list2str()" function
 */
    فراغ
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    صحيح		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    אם (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	החזר;

    אם (check_for_list_arg(argvars, 0) == FAIL)
	החזר;

    l = argvars[0].vval.v_list;
    אם (l == عدم)
	החזר;  // empty list results in empty string

    אם (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (صحيح)tv_get_bool_chk(&argvars[1], عدم);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    אם (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	صحيح	(*char2bytes)(صحيح, char_u *);

	אם (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	אחרת
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
    אחרת
    {
	long len = (long)list_len(l) + 1;

	אם (len <= INT_MAX && ga_grow(&ga, (صحيح)len) == OK)
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
    فراغ
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = عدم;

    אם (check_for_list_arg(argvars, 0) == FAIL)
	החזר;

    l = argvars[0].vval.v_list;
    אם (l == عدم)
	החזר;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    אם (rettv_tuple_set_with_items(rettv, list_len(l)) == FAIL)
	החזר;

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
    فراغ
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    אם (rettv_list_alloc(rettv) == FAIL)
	החזר;

    אם (check_for_tuple_arg(argvars, 0) == FAIL)
	החזר;

    tuple = argvars[0].vval.v_tuple;
    אם (tuple == عدم)
	החזר;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    עבור (صحيح i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ITEM(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    ساكن فراغ
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*item, *item2;
    listitem_T	*li;
    صحيح		error = FALSE;
    long	idx;
    long	end;
    صحيح		cnt = 0;
    list_T	*rl;

    אם ((l = argvars[0].vval.v_list) == عدم
			     || value_check_lock(l->lv_lock, arg_errmsg, TRUE))
	החזר;

    idx = (long)tv_get_number_chk(&argvars[1], &error);
    אם (error)
	החזר;		// type error: do nothing, errmsg already given

    אם ((item = list_find(l, idx)) == عدم)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	החזר;
    }

    אם (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	החזר;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_number_chk(&argvars[2], &error);
    אם (error)
	החזר;		// type error: do nothing

    אם ((item2 = list_find(l, end)) == عدم)
    {
	semsg(_(e_list_index_out_of_range_nr), end);
	החזר;
    }

    עבור (li = item; li != عدم; li = li->li_next)
    {
	++cnt;
	אם (li == item2)
	    اكسر;
    }
    אם (li == عدم)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	החזר;
    }

    vimlist_remove(l, item, item2);
    אם (rettv_list_alloc(rettv) == FAIL)
	החזר;

    rl = rettv->vval.v_list;

    אם (l->lv_with_items > 0)
    {
	// need to copy the list items and move the value
	כלעוד (item != عدم)
	{
	    li = listitem_alloc();
	    אם (li == عدم)
		החזר;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    אם (item == item2)
		اكسر;
	    item = item->li_next;
	}
    }
    אחרת
    {
	rl->lv_first = item;
	rl->lv_u.mat.lv_last = item2;
	item->li_prev = عدم;
	item2->li_next = عدم;
	rl->lv_len = cnt;
    }
}

ساكن صحيح item_compare(ثابت فراغ *s1, ثابت فراغ *s2);
ساكن صحيح item_compare2(ثابت فراغ *s1, ثابت فراغ *s2);

// struct used in the array that's given to qsort()
تعريفنوع بنية
{
    listitem_T	*item;
    صحيح		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    اتحاد {
	varnumber_T	inum;	// for "N"
	مزدوج		fnum;	// for "n" and "f"
    } key;
} sortItem_T;

// struct storing information about current sort
تعريفنوع بنية
{
    صحيح		item_compare_ic;
    صحيح		item_compare_lc;
    صحيح		item_compare_numeric;
    صحيح		item_compare_numbers;
    صحيح		item_compare_float;
    char_u	*item_compare_func;
    partial_T	*item_compare_partial;
    dict_T	*item_compare_selfdict;
    صحيح		item_compare_func_err;
    صحيح		item_compare_keep_zero;
    صحيح		item_compare_keys_ready;  // ptrs[].key is precomputed
} sortinfo_T;
ساكن sortinfo_T	*sortinfo = عدم;
#define ITEM_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    ساكن صحيح
item_compare(ثابت فراغ *s1, ثابت فراغ *s2)
{
    sortItem_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = عدم, *tofree2 = عدم;
    صحيح		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;
    tv1 = &si1->item->li_tv;
    tv2 = &si2->item->li_tv;

    אם (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_keys_ready
					? si1->key.inum : tv_to_number(tv1);
	varnumber_T	v2 = sortinfo->item_compare_keys_ready
					? si2->key.inum : tv_to_number(tv2);

	החזר v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    אם (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_keys_ready
					? si1->key.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_keys_ready
					? si2->key.fnum : tv_get_float(tv2);

	החזר v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    אם (sortinfo->item_compare_numeric && sortinfo->item_compare_keys_ready)
    {
	مزدوج	n1 = si1->key.fnum;
	مزدوج	n2 = si2->key.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	אם (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	החזר res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    אם (tv1->v_type == VAR_STRING)
    {
	אם (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	אחרת
	    p1 = tv1->vval.v_string;
    }
    אחרת
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    אם (tv2->v_type == VAR_STRING)
    {
	אם (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	אחרת
	    p2 = tv2->vval.v_string;
    }
    אחרת
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    אם (p1 == عدم)
	p1 = (char_u *)"";
    אם (p2 == عدم)
	p2 = (char_u *)"";
    אם (!sortinfo->item_compare_numeric)
    {
	אם (sortinfo->item_compare_lc)
	    res = strcoll((حرف *)p1, (حرف *)p2);
	אחרת
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    אחרת
    {
	مزدوج n1, n2;
	n1 = strtod((حرف *)p1, (حرف **)&p1);
	n2 = strtod((حرف *)p2, (حرف **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    אם (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    החזר res;
}

    ساكن صحيح
item_compare2(ثابت فراغ *s1, ثابت فراغ *s2)
{
    sortItem_T  *si1, *si2;
    صحيح		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->item_compare_partial;
    funcexe_T	funcexe;
    صحيح		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    אם (sortinfo->item_compare_func_err)
	החזר 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    אם (partial == عدم)
	func_name = sortinfo->item_compare_func;
    אחרת
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

    אם (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    אחרת
    {
	varnumber_T	n;

	n = tv_get_number_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    אם (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    אם (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    החזר res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    ساكن فراغ
sort_compute_keys(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    אם (info->item_compare_numbers)
    {
	עבור (i = 0; i < len; ++i)
	    ptrs[i].key.inum = tv_to_number(&ptrs[i].item->li_tv);
    }
    אחרת אם (info->item_compare_float)
    {
	עבור (i = 0; i < len; ++i)
	    ptrs[i].key.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    אחרת // info->item_compare_numeric
    {
	עבור (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    אם (tv->v_type == VAR_STRING)
		ptrs[i].key.fnum = 0.0;
	    אחרת
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = عدم;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].key.fnum = p == عدم ? 0.0 : strtod((حرف *)p, عدم);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_keys_ready = TRUE;
}

/*
 * sort() List "l"
 */
    ساكن فراغ
do_sort(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    אם (ptrs == عدم)
	החזר;

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

    אם (info->item_compare_func == عدم && info->item_compare_partial == عدم
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_keys(ptrs, len, info);
    // test the compare function
    אם ((info->item_compare_func != عدم
		|| info->item_compare_partial != عدم)
	    && item_compare2((فراغ *)&ptrs[0], (فراغ *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    אחרת
    {
	// Sort the array with item pointers.
	qsort((فراغ *)ptrs, (size_t)len, حجم(sortItem_T),
		info->item_compare_func == عدم
		&& info->item_compare_partial == عدم
		? item_compare : item_compare2);

	אם (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = عدم;
	    l->lv_len = 0;
	    עבור (i = 0; i < len; ++i)
		list_append(l, ptrs[i].item);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    ساكن فراغ
do_uniq(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;
    صحيح	(*item_compare_func_ptr)(ثابت فراغ *, ثابت فراغ *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    אם (ptrs == عدم)
	החזר;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != عدم
	|| info->item_compare_partial != عدم
	? item_compare2 : item_compare;

    עבור (li = l->lv_first; li != عدم && li->li_next != عدم;
	    li = li->li_next)
    {
	אם (item_compare_func_ptr((فراغ *)&li, (فراغ *)&li->li_next)
		== 0)
	    ptrs[i++].item = li;
	אם (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    اكسر;
	}
    }

    אם (!info->item_compare_func_err)
    {
	כלעוד (--i >= 0)
	{
	    li = ptrs[i].item->li_next;
	    ptrs[i].item->li_next = li->li_next;
	    אם (li->li_next != عدم)
		li->li_next->li_prev = ptrs[i].item;
	    אחרת
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
    ساكن صحيح
parse_sort_uniq_args(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = عدم;
    info->item_compare_partial = عدم;
    info->item_compare_selfdict = عدم;
    info->item_compare_keys_ready = FALSE;

    אם (argvars[1].v_type == VAR_UNKNOWN)
	החזר OK;

    // optional second argument: {func}
    אם (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    אחרת אם (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    אחרת
    {
	صحيح	    error = FALSE;
	صحيح	    nr = 0;

	אם (argvars[1].v_type == VAR_NUMBER)
	{
	    nr = tv_get_number_chk(&argvars[1], &error);
	    אם (error)
		החזר FAIL;
	    אם (nr == 1)
		info->item_compare_ic = TRUE;
	}
	אם (nr != 1)
	{
	    אם (argvars[1].v_type != VAR_NUMBER)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    אחרת אם (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		החזר FAIL;
	    }
	}
	אם (info->item_compare_func != عدم)
	{
	    אם (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = عدم;
	    }
	    אחרת אם (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = عدم;
		info->item_compare_numeric = TRUE;
	    }
	    אחרת אם (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = عدم;
		info->item_compare_numbers = TRUE;
	    }
	    אחרת אם (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = عدم;
		info->item_compare_float = TRUE;
	    }
	    אחרת אם (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = عدم;
		info->item_compare_ic = TRUE;
	    }
	    אחרת אם (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = عدم;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    אם (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	אם (check_for_dict_arg(argvars, 2) == FAIL)
	    החזר FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    החזר OK;
}

/*
 * "sort()" or "uniq()" function
 */
    ساكن فراغ
do_sort_uniq(typval_T *argvars, typval_T *rettv, صحيح sort)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    long	len;

    אם (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	החזר;

    אם (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	החזר;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    אם (l != عدم && value_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	اذهب theend;
    rettv_list_set(rettv, l);
    אם (l == عدم)
	اذهب theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    אם (len <= 1)
	اذهب theend;	// short list sorts pretty quickly

    אם (parse_sort_uniq_args(argvars, &info) == FAIL)
	اذهب theend;

    אם (sort)
	do_sort(l, &info);
    אחרת
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    فراغ
f_sort(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    فراغ
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    صحيح
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*expr,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	صحيح		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    صحيح		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    אם (filtermap == FILTERMAP_FOREACH && expr->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(expr->vval.v_string);
	אם (!did_emsg)
	    retval = OK;
	اذهب theend;
    }

    argv[0] = *get_vim_var_tv(VV_KEY);
    argv[1] = *get_vim_var_tv(VV_VAL);
    אם (eval_expr_typval(expr, FALSE, argv, 2, fc, newtv) == FAIL)
	اذهب theend;
    אם (filtermap == FILTERMAP_FILTER)
    {
	صحيح	    error = FALSE;

	// filter(): when expr is zero remove the item
	אם (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	אחרת
	    *remp = (tv_get_number_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	אם (error)
	    اذهب theend;
    }
    אחרת אם (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    החזר retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    ساكن فراغ
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	حرف		*func_name,
	char_u		*arg_errmsg,
	typval_T	*expr,
	typval_T	*rettv)
{
    صحيح		prev_lock;
    list_T	*l_ret = عدم;
    صحيح		idx = 0;
    صحيح		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    אם (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = عدم;
    }
    אם (l == عدم || (filtermap == FILTERMAP_FILTER
			    && value_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	החזר;

    prev_lock = l->lv_lock;

    אם (filtermap == FILTERMAP_MAPNEW)
    {
	אם (rettv_list_alloc(rettv) == FAIL)
	    החזר;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    אם (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    אם (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	صحيح		len = l->lv_len;
	صحيح		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	אם (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = عدم;
	    l->lv_u.mat.lv_last = عدم;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = عدم;
	}

	עבור (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_NUMBER;
	    tv.v_lock = 0;
	    tv.vval.v_number = val;
	    set_vim_var_nr(VV_KEY, idx);
	    אם (filter_map_one(&tv, expr, filtermap, fc, &newtv, &rem) == FAIL)
		اكسر;
	    אם (did_emsg)
	    {
		clear_tv(&newtv);
		اكسر;
	    }
	    אם (filtermap != FILTERMAP_FOREACH)
	    {
		אם (filtermap != FILTERMAP_FILTER)
		{
		    אם (filtermap == FILTERMAP_MAP && argtype != عدم
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			اكسر;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    אם (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			اكسر;
		}
		אחרת אם (!rem)
		{
		    // filter(): append the list item value when not rem
		    אם (list_append_tv_move(l, &tv) == FAIL)
			اكسر;
		}
	    }

	    val += stride;
	}
    }
    אחרת
    {
	// Materialized list: loop over the items
	עבור (li = l->lv_first; li != عدم; li = nli)
	{
	    אם (filtermap == FILTERMAP_MAP && value_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		اكسر;
	    nli = li->li_next;
	    set_vim_var_nr(VV_KEY, idx);
	    אם (filter_map_one(&li->li_tv, expr, filtermap, fc,
							 &newtv, &rem) == FAIL)
		اكسر;
	    אם (did_emsg)
	    {
		clear_tv(&newtv);
		اكسر;
	    }
	    אם (filtermap == FILTERMAP_MAP)
	    {
		אם (argtype != عدم && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    اكسر;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_lock = 0;
		li->li_tv = newtv;
	    }
	    אחרת אם (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		אם (list_append_tv_move(l_ret, &newtv) == FAIL)
		    اكسر;
	    }
	    אחרת אם (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = prev_lock;
    אם (fc != عدم)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    ساكن فراغ
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*expr;
    حرف	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    صحيح		save_did_emsg;
    type_T	*type = عدم;

    // map(), filter(), foreach() return the first argument, also on failure.
    אם (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    אם (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	החזר;

    אם (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	החזר;
    }

    אם (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	אם (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != عدم)
	    type = argvars[0].vval.v_dict->dv_type;
	אחרת אם (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != عدم)
	    type = argvars[0].vval.v_list->lv_type;
    }

    אם (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	حرف *msg;

	אם (filtermap == FILTERMAP_FOREACH)
	    msg = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	אחרת
	    msg = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(msg), func_name);
	החזר;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    expr = &argvars[1];
    אם (expr->v_type == VAR_UNKNOWN)
	החזר;

    typval_T	save_val;
    typval_T	save_key;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_KEY, &save_key);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    אם (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, expr, rettv);
    אחרת אם (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, expr,
							    arg_errmsg, rettv);
    אחרת אם (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, expr, rettv);
    אחרת אם (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, expr);
    אחרת // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, expr, rettv);

    restore_vimvar(VV_KEY, &save_key);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    فراغ
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    فراغ
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    فراغ
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    فراغ
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    ساكن فراغ
list_add(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    אם (l == عدم)
    {
	אם (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    אחרת אם (!value_check_lock(l->lv_lock,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    فراغ
f_add(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_number = 1; // Default: Failed

    אם (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_number_arg(argvars, 1) == FAIL)))
	החזר;

    אם (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    אחרת אם (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    אחרת
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ساكن long
list_count(list_T *l, typval_T *needle, long idx, صحيح ic)
{
    long	n = 0;
    listitem_T	*li;

    אם (l == عدم)
	החזר 0;

    CHECK_LIST_MATERIALIZE(l);

    אם (list_len(l) == 0)
	החזר 0;

    li = list_find(l, idx);
    אם (li == عدم)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	החזר 0;
    }

    עבור ( ; li != عدم; li = li->li_next)
	אם (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    החזר n;
}

/*
 * "count()" function
 */
    فراغ
f_count(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    صحيح		ic = FALSE;
    صحيح		error = FALSE;

    אם (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 3) == FAIL)))
	החזר;

    אם (argvars[2].v_type != VAR_UNKNOWN)
	ic = (صحيح)tv_get_bool_chk(&argvars[2], &error);

    אם (!error && argvars[0].v_type == VAR_STRING)
	n = string_count(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    אחרת אם (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	אם (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_number_chk(&argvars[3], &error);
	אם (!error)
	    n = list_count(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    אחרת אם (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	אם (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_number_chk(&argvars[3], &error);
	אם (!error)
	    n = tuple_count(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    אחרת אם (!error && argvars[0].v_type == VAR_DICT)
    {
	אם (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	אחרת
	    n = dict_count(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    אחרת אם (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_number = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ساكن فراغ
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	حرف		*func_name,
	char_u		*arg_errmsg,
	صحيح		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*item;
    long	before;
    صحيح		error = FALSE;

    l1 = argvars[0].vval.v_list;
    אם (l1 == عدم)
    {
	emsg(_(e_cannot_extend_null_list));
	החזר;
    }
    אם (is_new || !value_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	אם (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    אם (l1 == عدم)
		החזר;
	}

	l2 = argvars[1].vval.v_list;
	אם (l2 == عدم)
	    اذهب theend;

	אם (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_number_chk(&argvars[2], &error);
	    אם (error)
		اذهب cleanup;		// type error; errmsg already given
	    אם (before == l1->lv_len)
		item = عدم;
	    אחרת
	    {
		item = list_find(l1, before);
		אם (item == عدم)
		{
		    semsg(_(e_list_index_out_of_range_nr), before);
		    اذهب cleanup;
		}
	    }
	}
	אחרת
	    item = عدم;
	אם (type != عدم && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    اذهب cleanup;
	list_extend(l1, l2, item);

theend:
	אם (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	אחרת
	    copy_tv(&argvars[0], rettv);
	החזר;

cleanup:
	אם (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    ساكن فراغ
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, صحيح is_new)
{
    type_T	*type = عدم;
    حرف	*func_name = is_new ? "extendnew()" : "extend()";

    אם (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	אם (!is_new && in_vim9script() && argvars[0].vval.v_list != عدم)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    אחרת אם (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	אם (!is_new && in_vim9script() && argvars[0].vval.v_dict != عدم)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    אחרת אם (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    אחרת
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    فراغ
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
    فراغ
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    ساكن فراغ
list_insert_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    صحيح		error = FALSE;

    אם (l == عدم)
    {
	אם (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	החזר;
    }

    אם (value_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	החזר;

    אם (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_number_chk(&argvars[2], &error);
    אם (error)
	החזר;		// type error; errmsg already given

    אם (before == l->lv_len)
	item = عدم;
    אחרת
    {
	item = list_find(l, before);
	אם (item == عدم)
	{
	    semsg(_(e_list_index_out_of_range_nr), before);
	    l = عدم;
	}
    }
    אם (l != عدم)
    {
	(فراغ)list_insert_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    فراغ
f_insert(typval_T *argvars, typval_T *rettv)
{
    אם (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_number_arg(argvars, 1) == FAIL)
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	החזר;

    אם (argvars[0].v_type == VAR_BLOB)
	blob_insert_func(argvars, rettv);
    אחרת אם (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    אחרת
	list_insert_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    فراغ
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    אם (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_number_arg(argvars, 1) == FAIL
			|| check_for_opt_number_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_number_arg(argvars, 1) == FAIL)))
	החזר;

    אם (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    אחרת אם (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    אחרת אם (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    אחרת
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    ساكن فراغ
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    אם (l != عدم
	    && !value_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	אם (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    החזר;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = عدم;
	l->lv_len = 0;
	כלעוד (li != عدم)
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
    فراغ
f_reverse(typval_T *argvars, typval_T *rettv)
{
    אם (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	החזר;

    אם (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    אחרת אם (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	אם (argvars[0].vval.v_string != عدم)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	אחרת
	    rettv->vval.v_string = عدم;
    }
    אחרת אם (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    אחרת אם (argvars[0].v_type == VAR_TUPLE)
	tuple_reverse(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    ساكن فراغ
list_reduce(
	typval_T	*argvars,
	typval_T	*expr,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = عدم;
    صحيح		range_list;
    صحيح		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    صحيح		r;
    صحيح		called_emsg_start = called_emsg;
    صحيح		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != عدم && l->lv_first == &range_list_item;
    אם (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    אם (argvars[2].v_type == VAR_UNKNOWN)
    {
	אם (l == عدم || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "List");
	    החזר;
	}
	אם (range_list)
	{
	    initial.v_type = VAR_NUMBER;
	    initial.vval.v_number = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	אחרת
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_next;
	}
    }
    אחרת
    {
	initial = argvars[2];
	אם (l != عدم && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    אם (l == عدم)
	החזר;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    prev_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    כלעוד (range_list ? range_idx < l->lv_len : li != عدم)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	אם (range_list)
	{
	    argv[1].v_type = VAR_NUMBER;
	    argv[1].vval.v_number = range_val;
	}
	אחרת
	    argv[1] = li->li_tv;

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	אם (argv[0].v_type != VAR_NUMBER && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	אם (r == FAIL || called_emsg != called_emsg_start)
	    اكسر;

	// advance to the next item
	אם (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	אחרת
	    li = li->li_next;
    }

    אם (fc != عدم)
	remove_funccal();

    l->lv_lock = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    فراغ
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    אם (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	החזר;

    אם (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    אחרת אם (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    אחרת
	func_name = tv_get_string_strict(&argvars[1]);
    אם (func_name == عدم || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	החזר;
    }

    אם (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    אחרת אם (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    אחרת אם (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    אחרת
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    فراغ
f_slice(typval_T *argvars, typval_T *rettv)
{
    אם (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_number_arg(argvars, 2) == FAIL))
	החזר;

    אם (check_can_index(&argvars[0], TRUE, FALSE) != OK)
	החזר;

    copy_tv(argvars, rettv);
    eval_index_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? عدم : argvars + 2,
	    TRUE, عدم, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
