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

#إذا defined(FEAT_EVAL)

// List heads for garbage collection.
ساكن list_T		*first_list = عدم;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    لكل ((lw) = (l)->lv_watch; (lw) != عدم; (lw) = (lw)->lw_next)

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
	إذا (lw == lwrem)
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
	إذا (lw->lw_item == item)
	    lw->lw_item = item->li_next;
}

    ساكن فراغ
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    إذا (first_list != عدم)
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
    إذا (l != عدم)
	list_init(l);
    أرجع l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    إذا (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	أرجع عدم;
#endif
    أرجع (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_ابوبو(صحيح كدي)
{
    list_T	*l;

    إذا (كدي > 0
	    && (size_t)كدي > (SIZE_MAX - حجم(list_T)) / حجم(listitem_T))
	أرجع عدم;
    l = (list_T *)alloc_clear(حجم(list_T) + كدي * حجم(listitem_T));
    إذا (l == عدم)
	أرجع عدم;

    list_init(l);

    إذا (كدي <= 0)
	أرجع l;

    listitem_T	*li = (listitem_T *)(l + 1);
    صحيح		i;

    l->lv_len = كدي;
    l->lv_with_ابوبو = كدي;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + كدي - 1;
    لكل (i = 0; i < كدي; ++i)
    {
	إذا (i == 0)
	    li->li_prev = عدم;
	وإلا
	    li->li_prev = li - 1;
	إذا (i == كدي - 1)
	    li->li_next = عدم;
	وإلا
	    li->li_next = li + 1;
	++li;
    }

    أرجع l;
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

    إذا (l == عدم)
	أرجع FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    أرجع OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    صحيح
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    إذا (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	أرجع FAIL;
#endif
    أرجع rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    فراغ
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    إذا (l != عدم)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    فراغ
list_unref(list_T *l)
{
    إذا (l != عدم && --l->lv_refcount <= 0)
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

    إذا (l->lv_first != &range_list_item)
	لكل (item = l->lv_first; item != عدم; item = l->lv_first)
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

    لكل (ll = first_list; ll != عدم; ll = ll->lv_used_next)
	إذا ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == عدم)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    أرجع did_free;
}

    ساكن فراغ
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    إذا (l->lv_used_prev == عدم)
	first_list = l->lv_used_next;
    وإلا
	l->lv_used_prev->lv_used_next = l->lv_used_next;
    إذا (l->lv_used_next != عدم)
	l->lv_used_next->lv_used_prev = l->lv_used_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    فراغ
list_free_ابوبو(صحيح copyID)
{
    list_T	*ll, *ll_next;

    لكل (ll = first_list; ll != عدم; ll = ll_next)
    {
	ll_next = ll->lv_used_next;
	إذا ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
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
    إذا (in_free_unref_ابوبو)
	أرجع;

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
    أرجع ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    ساكن فراغ
list_free_item(list_T *l, listitem_T *item)
{
    إذا (l->lv_with_ابوبو == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_ابوبو)
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
    إذا (l == عدم)
	أرجع 0L;
    أرجع l->lv_len;
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

    إذا (l1 == l2)
	أرجع TRUE;
    إذا (list_len(l1) != list_len(l2))
	أرجع FALSE;
    إذا (list_len(l1) == 0)
	// empty and NULL list are considered equal
	أرجع TRUE;
    إذا (l1 == عدم || l2 == عدم)
	أرجع FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    لكل (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != عدم && item2 != عدم;
			       item1 = item1->li_next, item2 = item2->li_next)
	إذا (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    أرجع FALSE;
    أرجع item1 == عدم && item2 == عدم;
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

    إذا (l == عدم)
	أرجع عدم;

    // Negative index is relative to the end.
    إذا (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    إذا (n < 0 || n >= l->lv_len)
	أرجع عدم;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    إذا (n >= l->lv_len)
	أرجع عدم;

    // When there is a cached index may start search from there.
    إذا (l->lv_u.mat.lv_idx_item != عدم)
    {
	إذا (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	وإلا إذا (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	وإلا
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    وإلا
    {
	إذا (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	وإلا
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    طالما (n > idx)
    {
	// search forward
	item = item->li_next;
	++idx;
    }
    طالما (n < idx)
    {
	// search backward
	item = item->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    أرجع item;
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

    إذا (l != عدم && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	إذا (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	إذا (n < 0 || n >= l->lv_len)
	{
	    إذا (errorp != عدم)
		*errorp = TRUE;
	    أرجع -1L;
	}

	أرجع l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    إذا (li == عدم)
    {
	إذا (errorp != عدم)
	    *errorp = TRUE;
	أرجع -1L;
    }
    أرجع (long)tv_get_لمب_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    إذا (li == عدم)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	أرجع عدم;
    }
    أرجع tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_index(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    إذا (li != عدم)
	أرجع li;

    إذا (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    أرجع li;
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

    إذا (l == عدم)
	أرجع -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    لكل (li = l->lv_first; li != عدم && li != item; li = li->li_next)
	++idx;
    إذا (li == عدم)
	أرجع -1;
    أرجع idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    فراغ
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    إذا (l->lv_u.mat.lv_last == عدم)
    {
	// empty list
	l->lv_first = item;
	item->li_prev = عدم;
    }
    وإلا
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

    إذا (l->lv_type != عدم && l->lv_type->tt_member != عدم
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      عدم, 0) == FAIL)
	أرجع FAIL;
    li = listitem_alloc();
    إذا (li == عدم)
	أرجع FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    أرجع OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    ساكن صحيح
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    إذا (li == عدم)
	أرجع FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    أرجع OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    صحيح
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    إذا (li == عدم)
	أرجع FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    أرجع OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    صحيح
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    إذا (li == عدم)
	أرجع FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    أرجع OK;
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

    إذا (li == عدم)
	أرجع FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    إذا (str == عدم)
	li->li_tv.vval.v_string = عدم;
    وإلا إذا ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == عدم)
	أرجع FAIL;
    أرجع OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    صحيح
list_append_لمب(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    إذا (li == عدم)
	أرجع FAIL;
    li->li_tv.v_type = VAR_لمب;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_لمب = n;
    list_append(l, li);
    أرجع OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    صحيح
list_سك_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    إذا (l->lv_type != عدم && l->lv_type->tt_member != عدم
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      عدم, 0) == FAIL)
	أرجع FAIL;
    ni = listitem_alloc();
    إذا (ni == عدم)
	أرجع FAIL;
    copy_tv(tv, &ni->li_tv);
    list_سك(l, ni, item);
    أرجع OK;
}

    فراغ
list_سك(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    إذا (item == عدم)
	// Append new item at end of list.
	list_append(l, ni);
    وإلا
    {
	// Insert new item before existing item.
	ni->li_prev = item->li_prev;
	ni->li_next = item;
	إذا (item->li_prev == عدم)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	وإلا
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

    إذا (l->lv_type != عدم && l->lv_type->tt_member != عدم)
	tv.v_type = l->lv_type->tt_member->tt_type;
    وإلا
	tv.v_type = VAR_لمب;
    tv.v_lock = 0;

    بدل (tv.v_type)
    {
	حالة VAR_BOOL: tv.vval.v_لمب = VVAL_FALSE; اكسر;
	حالة VAR_SPECIAL: tv.vval.v_لمب = 0; اكسر;
	حالة VAR_لمب: tv.vval.v_لمب = 0; اكسر;
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
	    إذا (l->lv_type != عدم && l->lv_type->tt_member != عدم)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    وإلا
		tv.vval.v_class = عدم;
	    اكسر;
	حالة VAR_OBJECT:
	    إذا (l->lv_type != عدم && l->lv_type->tt_member != عدم)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    وإلا
		tv.vval.v_object = عدم;
	    اكسر;
	حالة VAR_TYPEALIAS: tv.vval.v_typealias = عدم; اكسر;
	حالة VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); اكسر;
	افتراضي:
	    tv.v_type = VAR_لمب;
	    tv.vval.v_لمب = 0;
	    اكسر;
    }

    إذا (l->lv_type != عدم && l->lv_type->tt_member != عدم)
	set_tv_type(&tv, l->lv_type->tt_member);

    أرجع list_append_tv(l, &tv);
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

    إذا (li != عدم)
	أرجع li;

    // Vim9: Allow for adding an item at the end.
    إذا (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	إذا (list_append_new_item(l) == FAIL)
	    أرجع عدم;
	li = list_find_index(l, n1);
    }
    إذا (li == عدم)
    {
	إذا (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), orig_n1);
	أرجع عدم;
    }
    أرجع li;
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
    إذا (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	إذا (ni == عدم)
	{
	    إذا (!quiet)
		semsg(_(e_list_index_out_of_range_nr), *n2);
	    أرجع FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    إذا (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    إذا (*n2 < *n1)
    {
	إذا (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), *n2);
	أرجع FAIL;
    }
    أرجع OK;
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
    لكل (src_li = src->lv_first; src_li != عدم && dest_li != عدم; )
    {
	إذا (درج_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    أرجع FAIL;
	src_li = src_li->li_next;
	إذا (src_li == عدم || (!empty_idx2 && idx2 == idx))
	    اكسر;
	dest_li = dest_li->li_next;
	++idx;
    }

    إذا (in_vim9script() && dest->lv_type != عدم
					   && dest->lv_type->tt_member != عدم)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    لكل (src_li = src->lv_first; src_li != عدم; )
    {
	إذا (op != عدم && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	وإلا
	{
	    إذا (member_type != عدم
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      عدم, 0) == FAIL)
		أرجع FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_next;
	إذا (src_li == عدم || (!empty_idx2 && idx2 == idx))
	    اكسر;
	إذا (dest_li->li_next == عدم)
	{
	    // Need to add an empty item.
	    إذا (list_append_لمب(dest, 0) == FAIL)
	    {
		src_li = عدم;
		اكسر;
	    }
	}
	dest_li = dest_li->li_next;
	++idx;
    }
    إذا (src_li != عدم)
    {
	emsg(_(e_list_درج_has_more_ابوبو_than_targets));
	أرجع FAIL;
    }
    إذا (empty_idx2
	    ? (dest_li != عدم && dest_li->li_next != عدم)
	    : idx != idx2)
    {
	emsg(_(e_list_درج_does_not_have_enough_ابوبو));
	أرجع FAIL;
    }
    أرجع OK;
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

    إذا (maxdepth == 0)
	أرجع;
    CHECK_LIST_MATERIALIZE(list);
    إذا (first == عدم)
	item = list->lv_first;
    وإلا
	item = first;

    طالما (item != عدم && done < maxitems)
    {
	listitem_T	*next = item->li_next;

	fast_breakcheck();
	إذا (got_int)
	    أرجع;

	إذا (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    إذا (list_extend(list, itemlist, next) == FAIL)
	    {
		list_free_item(list, item);
		أرجع;
	    }

	    إذا (maxdepth > 0)
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

    إذا (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_لمب_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	أرجع;
    }

    إذا (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    وإلا
    {
	maxdepth = (long)tv_get_لمب_chk(&argvars[1], &error);
	إذا (error)
	    أرجع;
	إذا (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_لمب));
	    أرجع;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    إذا (l == عدم)
	أرجع;

    إذا (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	إذا (l == عدم)
	    أرجع;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = عدم;
    }
    وإلا
    {
	إذا (درج_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    أرجع;
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
    إذا (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    وإلا
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

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;
    إذا (l == عدم)
	أرجع;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    لكل (idx = 0, li = l->lv_first; li != عدم; li = li->li_next, ++idx)
    {
	list_T	    *l2 = list_alloc();

	إذا (l2 == عدم)
	    اكسر;
	إذا (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    اكسر;
	}
	إذا (list_append_لمب(l2, idx) == FAIL
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

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;
    إذا (p == عدم)
	أرجع;  // null string behaves like an empty string

    لكل (idx = 0; *p != NUL; ++idx)
    {
	صحيح	    len = mb_ptr2len(p);
	list_T	    *l2;

	إذا (len == 0)
	    اكسر;
	l2 = list_alloc();
	إذا (l2 == عدم)
	    اكسر;
	إذا (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    اكسر;
	}
	إذا (list_append_لمب(l2, idx) == FAIL
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
    إذا (l2 == عدم || l2->lv_len == 0)
	أرجع OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == عدم ? عدم : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    لكل (item = l2->lv_first; item != عدم && --todo >= 0;
				 item = item == bef_prev ? bef : item->li_next)
	إذا (list_سك_tv(l1, &item->li_tv, bef) == FAIL)
	    أرجع FAIL;
    أرجع OK;
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

    إذا (totallen == 0)
    {
	l = list_alloc();
	إذا (l == عدم)
	    أرجع FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	أرجع OK;
    }
    إذا (totallen > INT_MAX)
	أرجع FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_ابوبو((صحيح)totallen);
    إذا (l == عدم)
	أرجع FAIL;

    i = 0;
    إذا (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	لكل (item = l1->lv_first; item != عدم && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    إذا (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	لكل (item = l2->lv_first; item != عدم && !got_int;
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
    أرجع OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	كدي;
    long	idx;

    إذا (n2 < n1)
	أرجع list_alloc();
    إذا (n1 > INT_MAX - (n2 - n1) - 1)
	أرجع عدم;
    كدي = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_ابوبو((صحيح)كدي);
    إذا (l == عدم)
	أرجع عدم;

    item = list_find(ol, n1);
    لكل (idx = 0; idx < كدي; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	إذا (item == عدم)
	{
	    list_free(l);
	    أرجع عدم;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (صحيح)idx, &new_tv);
	item = item->li_next;
    }
    أرجع l;
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

    إذا (n1 < 0)
	n1 = len + n1;
    إذا (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	إذا (!range)
	{
	    إذا (verbose)
		semsg(_(e_list_index_out_of_range_nr), (long)n1_arg);
	    أرجع FAIL;
	}
	إذا (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	وإلا
	    n1 = len;
    }
    إذا (range)
    {
	list_T	*l;

	إذا (n2 < 0)
	    n2 = len + n2;
	وإلا إذا (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	إذا (exclusive)
	    --n2;
	إذا (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	إذا (l == عدم)
	    أرجع FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    وإلا
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long index = n1;
	listitem_T *li = check_range_index_one(list, &index, TRUE, TRUE);
	إذا (li == عدم)
	    أرجع FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	إذا (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != عدم
					&& var1.vval.v_list->lv_type == عدم)
	{
	    إذا (list->lv_type != عدم && list->lv_type->tt_member != عدم)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    أرجع OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, صحيح deep, صحيح كولي, صحيح copyID)
{
    list_T	*copy;
    listitem_T	*item;
    صحيح		idx = 0;

    إذا (orig == عدم)
	أرجع عدم;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_ابوبو(orig->lv_len);
    إذا (copy == عدم)
	أرجع عدم;

    إذا (orig->lv_type == عدم || كولي || deep)
	copy->lv_type = عدم;
    وإلا
	copy->lv_type = alloc_type(orig->lv_type);
    إذا (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    لكل (item = orig->lv_first; item != عدم && !got_int;
	    item = item->li_next)
    {
	typval_T	new_tv;

	إذا (deep)
	{
	    إذا (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		اكسر;
	}
	وإلا
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    إذا (item != عدم)
    {
	list_unref(copy);
	copy = عدم;
    }

    أرجع copy;
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
    لكل (ip = item; ip != عدم; ip = ip->li_next)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	إذا (ip == item2)
	    اكسر;
    }

    إذا (item2->li_next == عدم)
	l->lv_u.mat.lv_last = item->li_prev;
    وإلا
	item2->li_next->li_prev = item->li_prev;
    إذا (item->li_prev == عدم)
	l->lv_first = item2->li_next;
    وإلا
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

    إذا (tv->vval.v_list == عدم)
	أرجع عدم;
    ga_init2(&ga, حجم(حرف), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    إذا (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	أرجع عدم;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    أرجع (char_u *)ga.ga_data;
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
    إذا (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    لكل (item = l->lv_first; item != عدم && !got_int; item = item->li_next)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	إذا (s.string == عدم)
	{
	    l->lv_lock = prev_lock;
	    أرجع FAIL;
	}

	s.length = STRLEN(s.string);
	sumlen += (long)s.length;

	(فراغ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	إذا (tofree != عدم || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	وإلا
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	لي_breakcheck();
	إذا (did_echo_string_emsg)  // recursion error, bail out
	    اكسر;
    }
    l->lv_lock = prev_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    إذا (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    إذا (sumlen > INT_MAX - 2 || ga_grow(gap, (صحيح)sumlen + 2) == FAIL)
	أرجع FAIL;

    لكل (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	إذا (first)
	    first = FALSE;
	وإلا
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	إذا (p->s.string != عدم)
	    ga_concat_len(gap, p->s.string, p->s.length);
	لي_breakcheck();
    }

    أرجع OK;
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

    إذا (l->lv_len < 1)
	أرجع OK; // nothing to do
    ga_init2(&join_ga, حجم(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    إذا (join_ga.ga_data == عدم)
	أرجع retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    لكل (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    أرجع retval;
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

    إذا (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	أرجع;

    إذا ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == عدم)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == عدم))
	أرجع;

    إذا (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    وإلا
	sep = tv_get_string_chk(&argvars[1]);

    إذا (sep != عدم)
    {
	ga_init2(&ga, حجم(حرف), 80);
	إذا (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	وإلا
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
    وإلا
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

    إذا (evaluate)
    {
	l = list_alloc();
	إذا (l == عدم)
	    أرجع FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    طالما (**arg != ']' && **arg != NUL)
    {
	إذا (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    اذهب failret;
	إذا (check_typval_is_درج(&tv) == FAIL)
	{
	    إذا (evaluate)
		clear_tv(&tv);
	    اذهب failret;
	}
	إذا (evaluate)
	{
	    item = listitem_alloc();
	    إذا (item != عدم)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    وإلا
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	إذا (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	إذا (had_comma)
	{
	    إذا (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		اذهب failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	إذا (**arg == ']')
	    اكسر;

	إذا (!had_comma)
	{
	    إذا (do_error)
	    {
		إذا (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		وإلا
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    اذهب failret;
	}
    }

    إذا (**arg != ']')
    {
	إذا (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	إذا (evaluate)
	    list_free(l);
	أرجع FAIL;
    }

    *arg += 1;
    إذا (evaluate)
	rettv_list_set(rettv, l);

    أرجع OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    صحيح
write_list(FILE *fd, list_T *list, صحيح نبيو)
{
    listitem_T	*li;
    صحيح		c;
    صحيح		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ابوبو(list, li)
    {
	لكل (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    إذا (*s == '\n')
		c = putc(NUL, fd);
	    وإلا
		c = putc(*s, fd);
	    إذا (c == EOF)
	    {
		ret = FAIL;
		اكسر;
	    }
	}
	إذا (!نبيو || li->li_next != عدم)
	    إذا (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		اكسر;
	    }
	إذا (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    اكسر;
	}
    }
    أرجع ret;
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
    l->lv_first = &sl->sl_ابوبو[0];
    l->lv_u.mat.lv_last = &sl->sl_ابوبو[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    لكل (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_ابوبو[i];

	إذا (i == 0)
	    li->li_prev = عدم;
	وإلا
	    li->li_prev = li - 1;
	إذا (i == 9)
	    li->li_next = عدم;
	وإلا
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

    إذا (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (check_for_list_arg(argvars, 0) == FAIL)
	أرجع;

    l = argvars[0].vval.v_list;
    إذا (l == عدم)
	أرجع;  // empty list results in empty string

    إذا (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (صحيح)tv_get_bool_chk(&argvars[1], عدم);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    إذا (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	صحيح	(*char2bytes)(صحيح, char_u *);

	إذا (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	وإلا
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_ابوبو(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_لمب(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    وإلا
    {
	long len = (long)list_len(l) + 1;

	إذا (len <= INT_MAX && ga_grow(&ga, (صحيح)len) == OK)
	{
	    FOR_ALL_LIST_ابوبو(l, li)
		ga_append(&ga, tv_get_لمب(&li->li_tv));
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

    إذا (check_for_list_arg(argvars, 0) == FAIL)
	أرجع;

    l = argvars[0].vval.v_list;
    إذا (l == عدم)
	أرجع;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    إذا (rettv_tuple_set_with_ابوبو(rettv, list_len(l)) == FAIL)
	أرجع;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_ابوبو(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ITEM(tuple, TUPLE_LEN(tuple)));
	tuple->tv_ابوبو.ga_len++;
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

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (check_for_tuple_arg(argvars, 0) == FAIL)
	أرجع;

    tuple = argvars[0].vval.v_tuple;
    إذا (tuple == عدم)
	أرجع;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    لكل (صحيح i = 0; i < tuple_len(tuple); i++)
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

    إذا ((l = argvars[0].vval.v_list) == عدم
			     || درج_check_lock(l->lv_lock, arg_errmsg, TRUE))
	أرجع;

    idx = (long)tv_get_لمب_chk(&argvars[1], &error);
    إذا (error)
	أرجع;		// type error: do nothing, errmsg already given

    إذا ((item = list_find(l, idx)) == عدم)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	أرجع;
    }

    إذا (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	أرجع;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_لمب_chk(&argvars[2], &error);
    إذا (error)
	أرجع;		// type error: do nothing

    إذا ((item2 = list_find(l, end)) == عدم)
    {
	semsg(_(e_list_index_out_of_range_nr), end);
	أرجع;
    }

    لكل (li = item; li != عدم; li = li->li_next)
    {
	++cnt;
	إذا (li == item2)
	    اكسر;
    }
    إذا (li == عدم)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	أرجع;
    }

    vimlist_remove(l, item, item2);
    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    rl = rettv->vval.v_list;

    إذا (l->lv_with_ابوبو > 0)
    {
	// need to copy the list items and move the value
	طالما (item != عدم)
	{
	    li = listitem_alloc();
	    إذا (li == عدم)
		أرجع;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    إذا (item == item2)
		اكسر;
	    item = item->li_next;
	}
    }
    وإلا
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
    } مبودي;
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
    صحيح		item_compare_مبوداي_ready;  // ptrs[].key is precomputed
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

    إذا (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_مبوداي_ready
					? si1->مبودي.inum : tv_to_لمب(tv1);
	varnumber_T	v2 = sortinfo->item_compare_مبوداي_ready
					? si2->مبودي.inum : tv_to_لمب(tv2);

	أرجع v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    إذا (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_مبوداي_ready
					? si1->مبودي.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_مبوداي_ready
					? si2->مبودي.fnum : tv_get_float(tv2);

	أرجع v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    إذا (sortinfo->item_compare_numeric && sortinfo->item_compare_مبوداي_ready)
    {
	مزدوج	n1 = si1->مبودي.fnum;
	مزدوج	n2 = si2->مبودي.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	إذا (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	أرجع res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    إذا (tv1->v_type == VAR_STRING)
    {
	إذا (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	وإلا
	    p1 = tv1->vval.v_string;
    }
    وإلا
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    إذا (tv2->v_type == VAR_STRING)
    {
	إذا (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	وإلا
	    p2 = tv2->vval.v_string;
    }
    وإلا
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    إذا (p1 == عدم)
	p1 = (char_u *)"";
    إذا (p2 == عدم)
	p2 = (char_u *)"";
    إذا (!sortinfo->item_compare_numeric)
    {
	إذا (sortinfo->item_compare_lc)
	    res = strcoll((حرف *)p1, (حرف *)p2);
	وإلا
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    وإلا
    {
	مزدوج n1, n2;
	n1 = strtod((حرف *)p1, (حرف **)&p1);
	n2 = strtod((حرف *)p2, (حرف **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    إذا (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    أرجع res;
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
    إذا (sortinfo->item_compare_func_err)
	أرجع 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    إذا (partial == عدم)
	func_name = sortinfo->item_compare_func;
    وإلا
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

    إذا (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    وإلا
    {
	varnumber_T	n;

	n = tv_get_لمب_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    إذا (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    إذا (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    أرجع res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    ساكن فراغ
sort_compute_مبوداي(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    إذا (info->item_compare_numbers)
    {
	لكل (i = 0; i < len; ++i)
	    ptrs[i].مبودي.inum = tv_to_لمب(&ptrs[i].item->li_tv);
    }
    وإلا إذا (info->item_compare_float)
    {
	لكل (i = 0; i < len; ++i)
	    ptrs[i].مبودي.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    وإلا // info->item_compare_numeric
    {
	لكل (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    إذا (tv->v_type == VAR_STRING)
		ptrs[i].مبودي.fnum = 0.0;
	    وإلا
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = عدم;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].مبودي.fnum = p == عدم ? 0.0 : strtod((حرف *)p, عدم);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_مبوداي_ready = TRUE;
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
    إذا (ptrs == عدم)
	أرجع;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_ابوبو(l, li)
    {
	ptrs[i].item = li;
	ptrs[i].idx = i;
	++i;
    }

    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = FALSE;
    info->item_compare_مبوداي_ready = FALSE;

    إذا (info->item_compare_func == عدم && info->item_compare_partial == عدم
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_مبوداي(ptrs, len, info);
    // test the compare function
    إذا ((info->item_compare_func != عدم
		|| info->item_compare_partial != عدم)
	    && item_compare2((فراغ *)&ptrs[0], (فراغ *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    وإلا
    {
	// Sort the array with item pointers.
	qsort((فراغ *)ptrs, (size_t)len, حجم(sortItem_T),
		info->item_compare_func == عدم
		&& info->item_compare_partial == عدم
		? item_compare : item_compare2);

	إذا (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = عدم;
	    l->lv_len = 0;
	    لكل (i = 0; i < len; ++i)
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
    إذا (ptrs == عدم)
	أرجع;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != عدم
	|| info->item_compare_partial != عدم
	? item_compare2 : item_compare;

    لكل (li = l->lv_first; li != عدم && li->li_next != عدم;
	    li = li->li_next)
    {
	إذا (item_compare_func_ptr((فراغ *)&li, (فراغ *)&li->li_next)
		== 0)
	    ptrs[i++].item = li;
	إذا (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    اكسر;
	}
    }

    إذا (!info->item_compare_func_err)
    {
	طالما (--i >= 0)
	{
	    li = ptrs[i].item->li_next;
	    ptrs[i].item->li_next = li->li_next;
	    إذا (li->li_next != عدم)
		li->li_next->li_prev = ptrs[i].item;
	    وإلا
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
parse_sort_uniq_حجوجي(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = عدم;
    info->item_compare_partial = عدم;
    info->item_compare_selfdict = عدم;
    info->item_compare_مبوداي_ready = FALSE;

    إذا (argvars[1].v_type == VAR_UNKNOWN)
	أرجع OK;

    // optional second argument: {func}
    إذا (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    وإلا إذا (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    وإلا
    {
	صحيح	    error = FALSE;
	صحيح	    nr = 0;

	إذا (argvars[1].v_type == VAR_لمب)
	{
	    nr = tv_get_لمب_chk(&argvars[1], &error);
	    إذا (error)
		أرجع FAIL;
	    إذا (nr == 1)
		info->item_compare_ic = TRUE;
	}
	إذا (nr != 1)
	{
	    إذا (argvars[1].v_type != VAR_لمب)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    وإلا إذا (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		أرجع FAIL;
	    }
	}
	إذا (info->item_compare_func != عدم)
	{
	    إذا (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = عدم;
	    }
	    وإلا إذا (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = عدم;
		info->item_compare_numeric = TRUE;
	    }
	    وإلا إذا (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = عدم;
		info->item_compare_numbers = TRUE;
	    }
	    وإلا إذا (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = عدم;
		info->item_compare_float = TRUE;
	    }
	    وإلا إذا (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = عدم;
		info->item_compare_ic = TRUE;
	    }
	    وإلا إذا (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = عدم;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	إذا (check_for_dict_arg(argvars, 2) == FAIL)
	    أرجع FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    أرجع OK;
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

    إذا (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	أرجع;

    إذا (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	أرجع;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    إذا (l != عدم && درج_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	اذهب theend;
    rettv_list_set(rettv, l);
    إذا (l == عدم)
	اذهب theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    إذا (len <= 1)
	اذهب theend;	// short list sorts pretty quickly

    إذا (parse_sort_uniq_حجوجي(argvars, &info) == FAIL)
	اذهب theend;

    إذا (sort)
	do_sort(l, &info);
    وإلا
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
	typval_T	*فرچ,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	صحيح		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    صحيح		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    إذا (filtermap == FILTERMAP_FOREACH && فرچ->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(فرچ->vval.v_string);
	إذا (!did_emsg)
	    retval = OK;
	اذهب theend;
    }

    argv[0] = *get_vim_var_tv(VV_مبودي);
    argv[1] = *get_vim_var_tv(VV_VAL);
    إذا (eval_فرچ_typval(فرچ, FALSE, argv, 2, fc, newtv) == FAIL)
	اذهب theend;
    إذا (filtermap == FILTERMAP_FILTER)
    {
	صحيح	    error = FALSE;

	// filter(): when expr is zero remove the item
	إذا (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	وإلا
	    *remp = (tv_get_لمب_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	إذا (error)
	    اذهب theend;
    }
    وإلا إذا (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    أرجع retval;
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
	typval_T	*فرچ,
	typval_T	*rettv)
{
    صحيح		prev_lock;
    list_T	*l_ret = عدم;
    صحيح		idx = 0;
    صحيح		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    إذا (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = عدم;
    }
    إذا (l == عدم || (filtermap == FILTERMAP_FILTER
			    && درج_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	أرجع;

    prev_lock = l->lv_lock;

    إذا (filtermap == FILTERMAP_MAPNEW)
    {
	إذا (rettv_list_alloc(rettv) == FAIL)
	    أرجع;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_مبودي, VAR_لمب);

    إذا (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_فرچ_get_funccal(فرچ, &newtv);

    إذا (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	صحيح		len = l->lv_len;
	صحيح		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	إذا (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = عدم;
	    l->lv_u.mat.lv_last = عدم;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = عدم;
	}

	لكل (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_لمب;
	    tv.v_lock = 0;
	    tv.vval.v_لمب = val;
	    set_vim_var_nr(VV_مبودي, idx);
	    إذا (filter_map_one(&tv, فرچ, filtermap, fc, &newtv, &rem) == FAIL)
		اكسر;
	    إذا (did_emsg)
	    {
		clear_tv(&newtv);
		اكسر;
	    }
	    إذا (filtermap != FILTERMAP_FOREACH)
	    {
		إذا (filtermap != FILTERMAP_FILTER)
		{
		    إذا (filtermap == FILTERMAP_MAP && argtype != عدم
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			اكسر;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    إذا (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			اكسر;
		}
		وإلا إذا (!rem)
		{
		    // filter(): append the list item value when not rem
		    إذا (list_append_tv_move(l, &tv) == FAIL)
			اكسر;
		}
	    }

	    val += stride;
	}
    }
    وإلا
    {
	// Materialized list: loop over the items
	لكل (li = l->lv_first; li != عدم; li = nli)
	{
	    إذا (filtermap == FILTERMAP_MAP && درج_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		اكسر;
	    nli = li->li_next;
	    set_vim_var_nr(VV_مبودي, idx);
	    إذا (filter_map_one(&li->li_tv, فرچ, filtermap, fc,
							 &newtv, &rem) == FAIL)
		اكسر;
	    إذا (did_emsg)
	    {
		clear_tv(&newtv);
		اكسر;
	    }
	    إذا (filtermap == FILTERMAP_MAP)
	    {
		إذا (argtype != عدم && check_typval_arg_type(
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
	    وإلا إذا (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		إذا (list_append_tv_move(l_ret, &newtv) == FAIL)
		    اكسر;
	    }
	    وإلا إذا (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = prev_lock;
    إذا (fc != عدم)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    ساكن فراغ
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*فرچ;
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
    إذا (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    إذا (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	أرجع;

    إذا (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	أرجع;
    }

    إذا (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	إذا (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != عدم)
	    type = argvars[0].vval.v_dict->dv_type;
	وإلا إذا (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != عدم)
	    type = argvars[0].vval.v_list->lv_type;
    }

    إذا (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	حرف *سكو;

	إذا (filtermap == FILTERMAP_FOREACH)
	    سكو = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	وإلا
	    سكو = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(سكو), func_name);
	أرجع;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    فرچ = &argvars[1];
    إذا (فرچ->v_type == VAR_UNKNOWN)
	أرجع;

    typval_T	save_val;
    typval_T	save_مبودي;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_مبودي, &save_مبودي);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    إذا (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, فرچ, rettv);
    وإلا إذا (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, فرچ,
							    arg_errmsg, rettv);
    وإلا إذا (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, فرچ, rettv);
    وإلا إذا (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, فرچ);
    وإلا // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, فرچ, rettv);

    restore_vimvar(VV_مبودي, &save_مبودي);
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

    إذا (l == عدم)
    {
	إذا (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    وإلا إذا (!درج_check_lock(l->lv_lock,
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
    rettv->vval.v_لمب = 1; // Default: Failed

    إذا (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_لمب_arg(argvars, 1) == FAIL)))
	أرجع;

    إذا (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    وإلا إذا (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    وإلا
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ساكن long
list_كدي(list_T *l, typval_T *needle, long idx, صحيح ic)
{
    long	n = 0;
    listitem_T	*li;

    إذا (l == عدم)
	أرجع 0;

    CHECK_LIST_MATERIALIZE(l);

    إذا (list_len(l) == 0)
	أرجع 0;

    li = list_find(l, idx);
    إذا (li == عدم)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	أرجع 0;
    }

    لكل ( ; li != عدم; li = li->li_next)
	إذا (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    أرجع n;
}

/*
 * "count()" function
 */
    فراغ
f_كدي(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    صحيح		ic = FALSE;
    صحيح		error = FALSE;

    إذا (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_لمب_arg(argvars, 3) == FAIL)))
	أرجع;

    إذا (argvars[2].v_type != VAR_UNKNOWN)
	ic = (صحيح)tv_get_bool_chk(&argvars[2], &error);

    إذا (!error && argvars[0].v_type == VAR_STRING)
	n = string_كدي(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    وإلا إذا (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	إذا (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_لمب_chk(&argvars[3], &error);
	إذا (!error)
	    n = list_كدي(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    وإلا إذا (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	إذا (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_لمب_chk(&argvars[3], &error);
	إذا (!error)
	    n = tuple_كدي(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    وإلا إذا (!error && argvars[0].v_type == VAR_DICT)
    {
	إذا (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	وإلا
	    n = dict_كدي(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    وإلا إذا (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_لمب = n;
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
    إذا (l1 == عدم)
    {
	emsg(_(e_cannot_extend_null_list));
	أرجع;
    }
    إذا (is_new || !درج_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	إذا (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    إذا (l1 == عدم)
		أرجع;
	}

	l2 = argvars[1].vval.v_list;
	إذا (l2 == عدم)
	    اذهب theend;

	إذا (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_لمب_chk(&argvars[2], &error);
	    إذا (error)
		اذهب cleanup;		// type error; errmsg already given
	    إذا (before == l1->lv_len)
		item = عدم;
	    وإلا
	    {
		item = list_find(l1, before);
		إذا (item == عدم)
		{
		    semsg(_(e_list_index_out_of_range_nr), before);
		    اذهب cleanup;
		}
	    }
	}
	وإلا
	    item = عدم;
	إذا (type != عدم && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    اذهب cleanup;
	list_extend(l1, l2, item);

theend:
	إذا (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	وإلا
	    copy_tv(&argvars[0], rettv);
	أرجع;

cleanup:
	إذا (is_new)
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

    إذا (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	إذا (!is_new && in_vim9script() && argvars[0].vval.v_list != عدم)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    وإلا إذا (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	إذا (!is_new && in_vim9script() && argvars[0].vval.v_dict != عدم)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    وإلا إذا (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    وإلا
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
list_سك_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    صحيح		error = FALSE;

    إذا (l == عدم)
    {
	إذا (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	أرجع;
    }

    إذا (درج_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	أرجع;

    إذا (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_لمب_chk(&argvars[2], &error);
    إذا (error)
	أرجع;		// type error; errmsg already given

    إذا (before == l->lv_len)
	item = عدم;
    وإلا
    {
	item = list_find(l, before);
	إذا (item == عدم)
	{
	    semsg(_(e_list_index_out_of_range_nr), before);
	    l = عدم;
	}
    }
    إذا (l != عدم)
    {
	(فراغ)list_سك_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    فراغ
f_سك(typval_T *argvars, typval_T *rettv)
{
    إذا (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_لمب_arg(argvars, 1) == FAIL)
		|| check_for_opt_لمب_arg(argvars, 2) == FAIL))
	أرجع;

    إذا (argvars[0].v_type == VAR_BLOB)
	blob_سك_func(argvars, rettv);
    وإلا إذا (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    وإلا
	list_سك_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    فراغ
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    إذا (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_لمب_arg(argvars, 1) == FAIL
			|| check_for_opt_لمب_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_لمب_arg(argvars, 1) == FAIL)))
	أرجع;

    إذا (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    وإلا إذا (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    وإلا إذا (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    وإلا
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    ساكن فراغ
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    إذا (l != عدم
	    && !درج_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	إذا (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    أرجع;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = عدم;
	l->lv_len = 0;
	طالما (li != عدم)
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
    إذا (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    وإلا إذا (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	إذا (argvars[0].vval.v_string != عدم)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	وإلا
	    rettv->vval.v_string = عدم;
    }
    وإلا إذا (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    وإلا إذا (argvars[0].v_type == VAR_TUPLE)
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
	typval_T	*فرچ,
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
    إذا (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    إذا (argvars[2].v_type == VAR_UNKNOWN)
    {
	إذا (l == عدم || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_درج), "List");
	    أرجع;
	}
	إذا (range_list)
	{
	    initial.v_type = VAR_لمب;
	    initial.vval.v_لمب = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	وإلا
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_next;
	}
    }
    وإلا
    {
	initial = argvars[2];
	إذا (l != عدم && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    إذا (l == عدم)
	أرجع;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_فرچ_get_funccal(فرچ, rettv);

    prev_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    طالما (range_list ? range_idx < l->lv_len : li != عدم)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	إذا (range_list)
	{
	    argv[1].v_type = VAR_لمب;
	    argv[1].vval.v_لمب = range_val;
	}
	وإلا
	    argv[1] = li->li_tv;

	r = eval_فرچ_typval(فرچ, TRUE, argv, 2, fc, rettv);

	إذا (argv[0].v_type != VAR_لمب && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	إذا (r == FAIL || called_emsg != called_emsg_start)
	    اكسر;

	// advance to the next item
	إذا (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	وإلا
	    li = li->li_next;
    }

    إذا (fc != عدم)
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

    إذا (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    وإلا إذا (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    وإلا
	func_name = tv_get_string_strict(&argvars[1]);
    إذا (func_name == عدم || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	أرجع;
    }

    إذا (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    وإلا إذا (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    وإلا إذا (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    وإلا
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    فراغ
f_slice(typval_T *argvars, typval_T *rettv)
{
    إذا (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_لمب_arg(argvars, 1) == FAIL
		|| check_for_opt_لمب_arg(argvars, 2) == FAIL))
	أرجع;

    إذا (check_can_index(&argvars[0], TRUE, FALSE) != OK)
	أرجع;

    copy_tv(argvars, rettv);
    eval_index_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? عدم : argvars + 2,
	    TRUE, عدم, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
