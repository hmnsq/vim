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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

// List heads for garbage collection.
স্থির list_T		*first_list = ফাঁকা;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    জন্য ((lw) = (l)->lv_watch; (lw) != ফাঁকা; (lw) = (lw)->lw_পরবর্তী)

স্থির শূন্য list_free_item(list_T *l, listitem_T *item);

/*
 * Add a watcher to a list.
 */
    শূন্য
list_add_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_পরবর্তী = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    শূন্য
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	যদি (lw == lwrem)
	{
	    *lwp = lw->lw_পরবর্তী;
	    বিরতি;
	}
	lwp = &lw->lw_পরবর্তী;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    স্থির শূন্য
list_fix_watch(list_T *l, listitem_T *item)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	যদি (lw->lw_item == item)
	    lw->lw_item = item->li_পরবর্তী;
}

    স্থির শূন্য
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    যদি (first_list != ফাঁকা)
	first_list->lv_used_পূর্ব = l;
    l->lv_used_পূর্ব = ফাঁকা;
    l->lv_used_পরবর্তী = first_list;
    first_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(শূন্য)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    যদি (l != ফাঁকা)
	list_init(l);
    ফেরত l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    যদি (alloc_fail_id == id && alloc_does_fail(আকার(list_T)))
	ফেরত ফাঁকা;
#endif
    ফেরত (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_বস্তুসমূহ(পূর্ণসংখ্যা গুনতি)
{
    list_T	*l;

    যদি (গুনতি > 0
	    && (size_t)গুনতি > (আকার_MAX - আকার(list_T)) / আকার(listitem_T))
	ফেরত ফাঁকা;
    l = (list_T *)alloc_clear(আকার(list_T) + গুনতি * আকার(listitem_T));
    যদি (l == ফাঁকা)
	ফেরত ফাঁকা;

    list_init(l);

    যদি (গুনতি <= 0)
	ফেরত l;

    listitem_T	*li = (listitem_T *)(l + 1);
    পূর্ণসংখ্যা		i;

    l->lv_len = গুনতি;
    l->lv_with_বস্তুসমূহ = গুনতি;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + গুনতি - 1;
    জন্য (i = 0; i < গুনতি; ++i)
    {
	যদি (i == 0)
	    li->li_পূর্ব = ফাঁকা;
	অন্যথা
	    li->li_পূর্ব = li - 1;
	যদি (i == গুনতি - 1)
	    li->li_পরবর্তী = ফাঁকা;
	অন্যথা
	    li->li_পরবর্তী = li + 1;
	++li;
    }

    ফেরত l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    শূন্য
list_set_item(list_T *l, পূর্ণসংখ্যা idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    যদি (l == ফাঁকা)
	ফেরত FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    ফেরত OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    পূর্ণসংখ্যা
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    যদি (alloc_fail_id == id && alloc_does_fail(আকার(list_T)))
	ফেরত FAIL;
#endif
    ফেরত rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    শূন্য
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    যদি (l != ফাঁকা)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    শূন্য
list_unref(list_T *l)
{
    যদি (l != ফাঁকা && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    স্থির শূন্য
list_free_contents(list_T *l)
{
    listitem_T *item;

    যদি (l->lv_first != &range_list_item)
	জন্য (item = l->lv_first; item != ফাঁকা; item = l->lv_first)
	{
	    // Remove the item before deleting it.
	    l->lv_first = item->li_পরবর্তী;
	    clear_tv(&item->li_tv);
	    list_free_item(l, item);
	}
}

/*
 * Go through the list of lists and free items without the copyID.
 * But don't free a list that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    পূর্ণসংখ্যা
list_free_nonref(পূর্ণসংখ্যা copyID)
{
    list_T	*ll;
    পূর্ণসংখ্যা		did_free = FALSE;

    জন্য (ll = first_list; ll != ফাঁকা; ll = ll->lv_used_পরবর্তী)
	যদি ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == ফাঁকা)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    ফেরত did_free;
}

    স্থির শূন্য
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    যদি (l->lv_used_পূর্ব == ফাঁকা)
	first_list = l->lv_used_পরবর্তী;
    অন্যথা
	l->lv_used_পূর্ব->lv_used_পরবর্তী = l->lv_used_পরবর্তী;
    যদি (l->lv_used_পরবর্তী != ফাঁকা)
	l->lv_used_পরবর্তী->lv_used_পূর্ব = l->lv_used_পূর্ব;

    free_type(l->lv_type);
    vim_free(l);
}

    শূন্য
list_free_বস্তুসমূহ(পূর্ণসংখ্যা copyID)
{
    list_T	*ll, *ll_পরবর্তী;

    জন্য (ll = first_list; ll != ফাঁকা; ll = ll_পরবর্তী)
    {
	ll_পরবর্তী = ll->lv_used_পরবর্তী;
	যদি ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == ফাঁকা)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    শূন্য
list_free(list_T *l)
{
    যদি (in_free_unref_বস্তুসমূহ)
	ফেরত;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(শূন্য)
{
    ফেরত ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    স্থির শূন্য
list_free_item(list_T *l, listitem_T *item)
{
    যদি (l->lv_with_বস্তুসমূহ == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_বস্তুসমূহ)
	vim_free(item);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    শূন্য
listitem_free(list_T *l, listitem_T *item)
{
    clear_tv(&item->li_tv);
    list_free_item(l, item);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    শূন্য
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
    যদি (l == ফাঁকা)
	ফেরত 0L;
    ফেরত l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    পূর্ণসংখ্যা
list_equal(
    list_T	*l1,
    list_T	*l2,
    পূর্ণসংখ্যা		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    যদি (l1 == l2)
	ফেরত TRUE;
    যদি (list_len(l1) != list_len(l2))
	ফেরত FALSE;
    যদি (list_len(l1) == 0)
	// empty and NULL list are considered equal
	ফেরত TRUE;
    যদি (l1 == ফাঁকা || l2 == ফাঁকা)
	ফেরত FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    জন্য (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != ফাঁকা && item2 != ফাঁকা;
			       item1 = item1->li_পরবর্তী, item2 = item2->li_পরবর্তী)
	যদি (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    ফেরত FALSE;
    ফেরত item1 == ফাঁকা && item2 == ফাঁকা;
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

    যদি (l == ফাঁকা)
	ফেরত ফাঁকা;

    // Negative index is relative to the end.
    যদি (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    যদি (n < 0 || n >= l->lv_len)
	ফেরত ফাঁকা;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    যদি (n >= l->lv_len)
	ফেরত ফাঁকা;

    // When there is a cached index may start search from there.
    যদি (l->lv_u.mat.lv_idx_item != ফাঁকা)
    {
	যদি (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	অন্যথা যদি (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	অন্যথা
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    অন্যথা
    {
	যদি (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	অন্যথা
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    যখন (n > idx)
    {
	// search forward
	item = item->li_পরবর্তী;
	++idx;
    }
    যখন (n < idx)
    {
	// search backward
	item = item->li_পূর্ব;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    ফেরত item;
}

/*
 * Get list item "l[idx]" as a number.
 */
    long
list_find_nr(
    list_T	*l,
    long	idx,
    পূর্ণসংখ্যা		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    যদি (l != ফাঁকা && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	যদি (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	যদি (n < 0 || n >= l->lv_len)
	{
	    যদি (errorp != ফাঁকা)
		*errorp = TRUE;
	    ফেরত -1L;
	}

	ফেরত l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    যদি (li == ফাঁকা)
    {
	যদি (errorp != ফাঁকা)
	    *errorp = TRUE;
	ফেরত -1L;
    }
    ফেরত (long)tv_get_সংখ্যা_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    যদি (li == ফাঁকা)
    {
	semsg(_(e_list_সূচক_out_of_range_nr), idx);
	ফেরত ফাঁকা;
    }
    ফেরত tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_সূচক(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    যদি (li != ফাঁকা)
	ফেরত li;

    যদি (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    ফেরত li;
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

    যদি (l == ফাঁকা)
	ফেরত -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    জন্য (li = l->lv_first; li != ফাঁকা && li != item; li = li->li_পরবর্তী)
	++idx;
    যদি (li == ফাঁকা)
	ফেরত -1;
    ফেরত idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    শূন্য
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    যদি (l->lv_u.mat.lv_last == ফাঁকা)
    {
	// empty list
	l->lv_first = item;
	item->li_পূর্ব = ফাঁকা;
    }
    অন্যথা
    {
	l->lv_u.mat.lv_last->li_পরবর্তী = item;
	item->li_পূর্ব = l->lv_u.mat.lv_last;
    }
    l->lv_u.mat.lv_last = item;
    ++l->lv_len;
    item->li_পরবর্তী = ফাঁকা;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    পূর্ণসংখ্যা
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    যদি (l->lv_type != ফাঁকা && l->lv_type->tt_member != ফাঁকা
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ফাঁকা, 0) == FAIL)
	ফেরত FAIL;
    li = listitem_alloc();
    যদি (li == ফাঁকা)
	ফেরত FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    ফেরত OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    স্থির পূর্ণসংখ্যা
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    যদি (li == ফাঁকা)
	ফেরত FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    ফেরত OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    পূর্ণসংখ্যা
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    যদি (li == ফাঁকা)
	ফেরত FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    ফেরত OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    পূর্ণসংখ্যা
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    যদি (li == ফাঁকা)
	ফেরত FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    ফেরত OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    পূর্ণসংখ্যা
list_append_string(list_T *l, char_u *str, পূর্ণসংখ্যা len)
{
    listitem_T *li = listitem_alloc();

    যদি (li == ফাঁকা)
	ফেরত FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    যদি (str == ফাঁকা)
	li->li_tv.vval.v_string = ফাঁকা;
    অন্যথা যদি ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == ফাঁকা)
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    পূর্ণসংখ্যা
list_append_সংখ্যা(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    যদি (li == ফাঁকা)
	ফেরত FAIL;
    li->li_tv.v_type = VAR_সংখ্যা;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_সংখ্যা = n;
    list_append(l, li);
    ফেরত OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    পূর্ণসংখ্যা
list_ঢোকাও_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    যদি (l->lv_type != ফাঁকা && l->lv_type->tt_member != ফাঁকা
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ফাঁকা, 0) == FAIL)
	ফেরত FAIL;
    ni = listitem_alloc();
    যদি (ni == ফাঁকা)
	ফেরত FAIL;
    copy_tv(tv, &ni->li_tv);
    list_ঢোকাও(l, ni, item);
    ফেরত OK;
}

    শূন্য
list_ঢোকাও(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    যদি (item == ফাঁকা)
	// Append new item at end of list.
	list_append(l, ni);
    অন্যথা
    {
	// Insert new item before existing item.
	ni->li_পূর্ব = item->li_পূর্ব;
	ni->li_পরবর্তী = item;
	যদি (item->li_পূর্ব == ফাঁকা)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	অন্যথা
	{
	    item->li_পূর্ব->li_পরবর্তী = ni;
	    l->lv_u.mat.lv_idx_item = ফাঁকা;
	}
	item->li_পূর্ব = ni;
	++l->lv_len;
    }
}

/*
 * Append a new empty item into "l" based on the list item type.  Used when
 * adding a new element to a List in Vim9script by using the current list
 * length as the index.
 */
    স্থির পূর্ণসংখ্যা
list_append_new_item(list_T *l)
{
    typval_T	tv;

    যদি (l->lv_type != ফাঁকা && l->lv_type->tt_member != ফাঁকা)
	tv.v_type = l->lv_type->tt_member->tt_type;
    অন্যথা
	tv.v_type = VAR_সংখ্যা;
    tv.v_lock = 0;

    নির্বাচন (tv.v_type)
    {
	ক্ষেত্র VAR_BOOL: tv.vval.v_সংখ্যা = VVAL_FALSE; বিরতি;
	ক্ষেত্র VAR_SPECIAL: tv.vval.v_সংখ্যা = 0; বিরতি;
	ক্ষেত্র VAR_সংখ্যা: tv.vval.v_সংখ্যা = 0; বিরতি;
	ক্ষেত্র VAR_FLOAT: tv.vval.v_float = 0; বিরতি;
	ক্ষেত্র VAR_STRING: tv.vval.v_string = ফাঁকা; বিরতি;
	ক্ষেত্র VAR_BLOB: tv.vval.v_blob = blob_alloc(); বিরতি;
	ক্ষেত্র VAR_FUNC: tv.vval.v_string = ফাঁকা; বিরতি;
	ক্ষেত্র VAR_PARTIAL: tv.vval.v_partial = ফাঁকা; বিরতি;
	ক্ষেত্র VAR_LIST: tv.vval.v_list = list_alloc(); বিরতি;
	ক্ষেত্র VAR_DICT: tv.vval.v_dict = dict_alloc(); বিরতি;
#ifdef FEAT_JOB_CHANNEL
	ক্ষেত্র VAR_JOB: tv.vval.v_job = ফাঁকা; বিরতি;
	ক্ষেত্র VAR_CHANNEL: tv.vval.v_channel = ফাঁকা; বিরতি;
#endif
	ক্ষেত্র VAR_CLASS:
	    যদি (l->lv_type != ফাঁকা && l->lv_type->tt_member != ফাঁকা)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    অন্যথা
		tv.vval.v_class = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_OBJECT:
	    যদি (l->lv_type != ফাঁকা && l->lv_type->tt_member != ফাঁকা)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    অন্যথা
		tv.vval.v_object = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_TYPEALIAS: tv.vval.v_typealias = ফাঁকা; বিরতি;
	ক্ষেত্র VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); বিরতি;
	পূর্বনির্ধারিত:
	    tv.v_type = VAR_সংখ্যা;
	    tv.vval.v_সংখ্যা = 0;
	    বিরতি;
    }

    যদি (l->lv_type != ফাঁকা && l->lv_type->tt_member != ফাঁকা)
	set_tv_type(&tv, l->lv_type->tt_member);

    ফেরত list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_সূচক_one(list_T *l, long *n1, পূর্ণসংখ্যা can_append, পূর্ণসংখ্যা quiet)
{
    long	orig_n1 = *n1;
    listitem_T	*li = list_find_সূচক(l, n1);

    যদি (li != ফাঁকা)
	ফেরত li;

    // Vim9: Allow for adding an item at the end.
    যদি (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	যদি (list_append_new_item(l) == FAIL)
	    ফেরত ফাঁকা;
	li = list_find_সূচক(l, n1);
    }
    যদি (li == ফাঁকা)
    {
	যদি (!quiet)
	    semsg(_(e_list_সূচক_out_of_range_nr), orig_n1);
	ফেরত ফাঁকা;
    }
    ফেরত li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    পূর্ণসংখ্যা
check_range_সূচক_two(
	list_T	    *l,
	long	    *n1,
	listitem_T  *li1,
	long	    *n2,
	পূর্ণসংখ্যা	    quiet)
{
    যদি (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	যদি (ni == ফাঁকা)
	{
	    যদি (!quiet)
		semsg(_(e_list_সূচক_out_of_range_nr), *n2);
	    ফেরত FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    যদি (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    যদি (*n2 < *n1)
    {
	যদি (!quiet)
	    semsg(_(e_list_সূচক_out_of_range_nr), *n2);
	ফেরত FAIL;
    }
    ফেরত OK;
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
    পূর্ণসংখ্যা
list_assign_range(
	list_T	    *dest,
	list_T	    *src,
	long	    idx1_arg,
	long	    idx2,
	পূর্ণসংখ্যা	    empty_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    long	idx1 = idx1_arg;
    listitem_T	*first_li = list_find_সূচক(dest, &idx1);
    long	idx;
    type_T	*member_type = ফাঁকা;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = first_li;
    জন্য (src_li = src->lv_first; src_li != ফাঁকা && dest_li != ফাঁকা; )
    {
	যদি (মান_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    ফেরত FAIL;
	src_li = src_li->li_পরবর্তী;
	যদি (src_li == ফাঁকা || (!empty_idx2 && idx2 == idx))
	    বিরতি;
	dest_li = dest_li->li_পরবর্তী;
	++idx;
    }

    যদি (in_vim9script() && dest->lv_type != ফাঁকা
					   && dest->lv_type->tt_member != ফাঁকা)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    জন্য (src_li = src->lv_first; src_li != ফাঁকা; )
    {
	যদি (op != ফাঁকা && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	অন্যথা
	{
	    যদি (member_type != ফাঁকা
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      ফাঁকা, 0) == FAIL)
		ফেরত FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_পরবর্তী;
	যদি (src_li == ফাঁকা || (!empty_idx2 && idx2 == idx))
	    বিরতি;
	যদি (dest_li->li_পরবর্তী == ফাঁকা)
	{
	    // Need to add an empty item.
	    যদি (list_append_সংখ্যা(dest, 0) == FAIL)
	    {
		src_li = ফাঁকা;
		বিরতি;
	    }
	}
	dest_li = dest_li->li_পরবর্তী;
	++idx;
    }
    যদি (src_li != ফাঁকা)
    {
	emsg(_(e_list_মান_has_more_বস্তুসমূহ_than_targets));
	ফেরত FAIL;
    }
    যদি (empty_idx2
	    ? (dest_li != ফাঁকা && dest_li->li_পরবর্তী != ফাঁকা)
	    : idx != idx2)
    {
	emsg(_(e_list_মান_does_not_have_enough_বস্তুসমূহ));
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    স্থির শূন্য
list_flatten(list_T *list, listitem_T *first, long maxitems, long maxdepth)
{
    listitem_T	*item;
    পূর্ণসংখ্যা		done = 0;

    যদি (maxdepth == 0)
	ফেরত;
    CHECK_LIST_MATERIALIZE(list);
    যদি (first == ফাঁকা)
	item = list->lv_first;
    অন্যথা
	item = first;

    যখন (item != ফাঁকা && done < maxitems)
    {
	listitem_T	*পরবর্তী = item->li_পরবর্তী;

	fast_breakcheck();
	যদি (got_int)
	    ফেরত;

	যদি (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    যদি (list_extend(list, itemlist, পরবর্তী) == FAIL)
	    {
		list_free_item(list, item);
		ফেরত;
	    }

	    যদি (maxdepth > 0)
		list_flatten(list, item->li_পূর্ব == ফাঁকা
				     ? list->lv_first : item->li_পূর্ব->li_পরবর্তী,
				itemlist->lv_len, maxdepth - 1);
	    clear_tv(&item->li_tv);
	    list_free_item(list, item);
	}

	++done;
	item = পরবর্তী;
    }
}

/*
 * "flatten()" and "flattennew()" functions
 */
    স্থির শূন্য
flatten_common(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা make_copy)
{
    list_T  *l;
    long    maxdepth;
    পূর্ণসংখ্যা	    error = FALSE;

    যদি (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	ফেরত;
    }

    যদি (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    অন্যথা
    {
	maxdepth = (long)tv_get_সংখ্যা_chk(&argvars[1], &error);
	যদি (error)
	    ফেরত;
	যদি (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_সংখ্যা));
	    ফেরত;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    যদি (l == ফাঁকা)
	ফেরত;

    যদি (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	যদি (l == ফাঁকা)
	    ফেরত;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = ফাঁকা;
    }
    অন্যথা
    {
	যদি (মান_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    ফেরত;
	++l->lv_refcount;
    }

    list_flatten(l, ফাঁকা, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    শূন্য
f_flatten(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    অন্যথা
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    শূন্য
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    শূন্য
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;
    যদি (l == ফাঁকা)
	ফেরত;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    জন্য (idx = 0, li = l->lv_first; li != ফাঁকা; li = li->li_পরবর্তী, ++idx)
    {
	list_T	    *l2 = list_alloc();

	যদি (l2 == ফাঁকা)
	    বিরতি;
	যদি (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    বিরতি;
	}
	যদি (list_append_সংখ্যা(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    বিরতি;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    শূন্য
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;
    যদি (p == ফাঁকা)
	ফেরত;  // null string behaves like an empty string

    জন্য (idx = 0; *p != NUL; ++idx)
    {
	পূর্ণসংখ্যা	    len = mb_ptr2len(p);
	list_T	    *l2;

	যদি (len == 0)
	    বিরতি;
	l2 = list_alloc();
	যদি (l2 == ফাঁকা)
	    বিরতি;
	যদি (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    বিরতি;
	}
	যদি (list_append_সংখ্যা(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    বিরতি;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    পূর্ণসংখ্যা
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*item;
    পূর্ণসংখ্যা		todo;
    listitem_T	*bef_পূর্ব;

    // NULL list is equivalent to an empty list: nothing to do.
    যদি (l2 == ফাঁকা || l2->lv_len == 0)
	ফেরত OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_পূর্ব = bef == ফাঁকা ? ফাঁকা : bef->li_পূর্ব;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    জন্য (item = l2->lv_first; item != ফাঁকা && --todo >= 0;
				 item = item == bef_পূর্ব ? bef : item->li_পরবর্তী)
	যদি (list_ঢোকাও_tv(l1, &item->li_tv, bef) == FAIL)
	    ফেরত FAIL;
    ফেরত OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    পূর্ণসংখ্যা
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    পূর্ণসংখ্যা		len1 = l1 == ফাঁকা ? 0 : l1->lv_len;
    পূর্ণসংখ্যা		len2 = l2 == ফাঁকা ? 0 : l2->lv_len;
    long	totallen = (long)len1 + (long)len2;
    পূর্ণসংখ্যা		i;
    listitem_T	*item;

    যদি (totallen == 0)
    {
	l = list_alloc();
	যদি (l == ফাঁকা)
	    ফেরত FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	ফেরত OK;
    }
    যদি (totallen > INT_MAX)
	ফেরত FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_বস্তুসমূহ((পূর্ণসংখ্যা)totallen);
    যদি (l == ফাঁকা)
	ফেরত FAIL;

    i = 0;
    যদি (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	জন্য (item = l1->lv_first; item != ফাঁকা && !got_int;
		item = item->li_পরবর্তী)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    যদি (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	জন্য (item = l2->lv_first; item != ফাঁকা && !got_int;
		item = item->li_পরবর্তী)
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
    ফেরত OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	গুনতি;
    long	idx;

    যদি (n2 < n1)
	ফেরত list_alloc();
    যদি (n1 > INT_MAX - (n2 - n1) - 1)
	ফেরত ফাঁকা;
    গুনতি = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_বস্তুসমূহ((পূর্ণসংখ্যা)গুনতি);
    যদি (l == ফাঁকা)
	ফেরত ফাঁকা;

    item = list_find(ol, n1);
    জন্য (idx = 0; idx < গুনতি; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	যদি (item == ফাঁকা)
	{
	    list_free(l);
	    ফেরত ফাঁকা;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (পূর্ণসংখ্যা)idx, &new_tv);
	item = item->li_পরবর্তী;
    }
    ফেরত l;
}

    পূর্ণসংখ্যা
list_slice_or_সূচক(
	    list_T	*list,
	    পূর্ণসংখ্যা		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    পূর্ণসংখ্যা		exclusive,
	    typval_T	*rettv,
	    পূর্ণসংখ্যা		verbose)
{
    long	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    যদি (n1 < 0)
	n1 = len + n1;
    যদি (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	যদি (!range)
	{
	    যদি (verbose)
		semsg(_(e_list_সূচক_out_of_range_nr), (long)n1_arg);
	    ফেরত FAIL;
	}
	যদি (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	অন্যথা
	    n1 = len;
    }
    যদি (range)
    {
	list_T	*l;

	যদি (n2 < 0)
	    n2 = len + n2;
	অন্যথা যদি (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	যদি (exclusive)
	    --n2;
	যদি (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	যদি (l == ফাঁকা)
	    ফেরত FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    অন্যথা
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long সূচক = n1;
	listitem_T *li = check_range_সূচক_one(list, &সূচক, TRUE, TRUE);
	যদি (li == ফাঁকা)
	    ফেরত FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	যদি (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != ফাঁকা
					&& var1.vval.v_list->lv_type == ফাঁকা)
	{
	    যদি (list->lv_type != ফাঁকা && list->lv_type->tt_member != ফাঁকা)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    ফেরত OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, পূর্ণসংখ্যা deep, পূর্ণসংখ্যা শীর্ষ_2, পূর্ণসংখ্যা copyID)
{
    list_T	*copy;
    listitem_T	*item;
    পূর্ণসংখ্যা		idx = 0;

    যদি (orig == ফাঁকা)
	ফেরত ফাঁকা;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_বস্তুসমূহ(orig->lv_len);
    যদি (copy == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (orig->lv_type == ফাঁকা || শীর্ষ_2 || deep)
	copy->lv_type = ফাঁকা;
    অন্যথা
	copy->lv_type = alloc_type(orig->lv_type);
    যদি (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    জন্য (item = orig->lv_first; item != ফাঁকা && !got_int;
	    item = item->li_পরবর্তী)
    {
	typval_T	new_tv;

	যদি (deep)
	{
	    যদি (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		বিরতি;
	}
	অন্যথা
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    যদি (item != ফাঁকা)
    {
	list_unref(copy);
	copy = ফাঁকা;
    }

    ফেরত copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    শূন্য
vimlist_remove(list_T *l, listitem_T *item, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    জন্য (ip = item; ip != ফাঁকা; ip = ip->li_পরবর্তী)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	যদি (ip == item2)
	    বিরতি;
    }

    যদি (item2->li_পরবর্তী == ফাঁকা)
	l->lv_u.mat.lv_last = item->li_পূর্ব;
    অন্যথা
	item2->li_পরবর্তী->li_পূর্ব = item->li_পূর্ব;
    যদি (item->li_পূর্ব == ফাঁকা)
	l->lv_first = item2->li_পরবর্তী;
    অন্যথা
	item->li_পূর্ব->li_পরবর্তী = item2->li_পরবর্তী;
    l->lv_u.mat.lv_idx_item = ফাঁকা;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, পূর্ণসংখ্যা copyID, পূর্ণসংখ্যা restore_copyID)
{
    garray_T	ga;

    যদি (tv->vval.v_list == ফাঁকা)
	ফেরত ফাঁকা;
    ga_init2(&ga, আকার(অক্ষর), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    যদি (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_তথ্য);
	ফেরত ফাঁকা;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    ফেরত (char_u *)ga.ga_তথ্য;
}

প্রকারলেখ কাঠামো join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    স্থির পূর্ণসংখ্যা
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    পূর্ণসংখ্যা		echo_style,
    পূর্ণসংখ্যা		restore_copyID,
    পূর্ণসংখ্যা		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    পূর্ণসংখ্যা		i;
    join_T	*p;
    long	sumlen = 0;
    পূর্ণসংখ্যা		first = TRUE;
    পূর্ণসংখ্যা		পূর্ব_lock;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    listitem_T	*item;
    string_T	s;
    size_t	seplen;

    // Stringify each item in the list.
    CHECK_LIST_MATERIALIZE(l);
    // Lock the list, the string() method of an object item could remove the
    // item the loop is standing on.
    পূর্ব_lock = l->lv_lock;
    যদি (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    জন্য (item = l->lv_first; item != ফাঁকা && !got_int; item = item->li_পরবর্তী)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	যদি (s.string == ফাঁকা)
	{
	    l->lv_lock = পূর্ব_lock;
	    ফেরত FAIL;
	}

	s.দৈর্ঘ্য = STRLEN(s.string);
	sumlen += (long)s.দৈর্ঘ্য;

	(শূন্য)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_তথ্য) + (join_gap->ga_len++);
	যদি (tofree != ফাঁকা || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.দৈর্ঘ্য = s.দৈর্ঘ্য;
	    p->tofree = tofree;
	}
	অন্যথা
	{
	    p->s.string = vim_strnsave(s.string, s.দৈর্ঘ্য);
	    p->s.দৈর্ঘ্য = s.দৈর্ঘ্য;
	    p->tofree = p->s.string;
	}

	সারি_breakcheck();
	যদি (did_echo_string_emsg)  // recursion error, bail out
	    বিরতি;
    }
    l->lv_lock = পূর্ব_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    যদি (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    যদি (sumlen > INT_MAX - 2 || ga_grow(gap, (পূর্ণসংখ্যা)sumlen + 2) == FAIL)
	ফেরত FAIL;

    জন্য (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	যদি (first)
	    first = FALSE;
	অন্যথা
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_তথ্য) + i;

	যদি (p->s.string != ফাঁকা)
	    ga_concat_len(gap, p->s.string, p->s.দৈর্ঘ্য);
	সারি_breakcheck();
    }

    ফেরত OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    পূর্ণসংখ্যা
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    পূর্ণসংখ্যা		echo_style,
    পূর্ণসংখ্যা		restore_copyID,
    পূর্ণসংখ্যা		copyID)
{
    garray_T	join_ga;
    পূর্ণসংখ্যা		retval;
    join_T	*p;
    পূর্ণসংখ্যা		i;

    যদি (l->lv_len < 1)
	ফেরত OK; // nothing to do
    ga_init2(&join_ga, আকার(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    যদি (join_ga.ga_তথ্য == ফাঁকা)
	ফেরত retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_তথ্য;
    জন্য (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ফেরত retval;
}

/*
 * "join()" function
 */
    শূন্য
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    যদি (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	ফেরত;

    যদি ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == ফাঁকা)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == ফাঁকা))
	ফেরত;

    যদি (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    অন্যথা
	sep = tv_get_string_chk(&argvars[1]);

    যদি (sep != ফাঁকা)
    {
	ga_init2(&ga, আকার(অক্ষর), 80);
	যদি (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	অন্যথা
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_তথ্য;
    }
    অন্যথা
	rettv->vval.v_string = ফাঁকা;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    পূর্ণসংখ্যা
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, পূর্ণসংখ্যা do_error)
{
    পূর্ণসংখ্যা		evaluate = evalarg == ফাঁকা ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = ফাঁকা;
    typval_T	tv;
    listitem_T	*item;
    পূর্ণসংখ্যা		vim9script = in_vim9script();
    পূর্ণসংখ্যা		had_comma;

    যদি (evaluate)
    {
	l = list_alloc();
	যদি (l == ফাঁকা)
	    ফেরত FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    যখন (**arg != ']' && **arg != NUL)
    {
	যদি (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    যাও failret;
	যদি (check_typval_is_মান(&tv) == FAIL)
	{
	    যদি (evaluate)
		clear_tv(&tv);
	    যাও failret;
	}
	যদি (evaluate)
	{
	    item = listitem_alloc();
	    যদি (item != ফাঁকা)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    অন্যথা
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	যদি (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	যদি (had_comma)
	{
	    যদি (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		যাও failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	যদি (**arg == ']')
	    বিরতি;

	যদি (!had_comma)
	{
	    যদি (do_error)
	    {
		যদি (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		অন্যথা
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    যাও failret;
	}
    }

    যদি (**arg != ']')
    {
	যদি (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	যদি (evaluate)
	    list_free(l);
	ফেরত FAIL;
    }

    *arg += 1;
    যদি (evaluate)
	rettv_list_set(rettv, l);

    ফেরত OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    পূর্ণসংখ্যা
write_list(FILE *fd, list_T *list, পূর্ণসংখ্যা দ্বিমিক)
{
    listitem_T	*li;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_বস্তুসমূহ(list, li)
    {
	জন্য (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    যদি (*s == '\n')
		c = putc(NUL, fd);
	    অন্যথা
		c = putc(*s, fd);
	    যদি (c == EOF)
	    {
		ret = FAIL;
		বিরতি;
	    }
	}
	যদি (!দ্বিমিক || li->li_পরবর্তী != ফাঁকা)
	    যদি (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		বিরতি;
	    }
	যদি (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    বিরতি;
	}
    }
    ফেরত ret;
}

/*
 * Initialize a static list with 10 items.
 */
    শূন্য
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    পূর্ণসংখ্যা	    i;

    CLEAR_POINTER(sl);
    l->lv_first = &sl->sl_বস্তুসমূহ[0];
    l->lv_u.mat.lv_last = &sl->sl_বস্তুসমূহ[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    জন্য (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_বস্তুসমূহ[i];

	যদি (i == 0)
	    li->li_পূর্ব = ফাঁকা;
	অন্যথা
	    li->li_পূর্ব = li - 1;
	যদি (i == 9)
	    li->li_পরবর্তী = ফাঁকা;
	অন্যথা
	    li->li_পরবর্তী = li + 1;
    }
}

/*
 * "list2str()" function
 */
    শূন্য
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    পূর্ণসংখ্যা		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ফেরত;

    যদি (check_for_list_arg(argvars, 0) == FAIL)
	ফেরত;

    l = argvars[0].vval.v_list;
    যদি (l == ফাঁকা)
	ফেরত;  // empty list results in empty string

    যদি (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[1], ফাঁকা);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    যদি (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	পূর্ণসংখ্যা	(*char2bytes)(পূর্ণসংখ্যা, char_u *);

	যদি (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	অন্যথা
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_বস্তুসমূহ(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_সংখ্যা(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    অন্যথা
    {
	long len = (long)list_len(l) + 1;

	যদি (len <= INT_MAX && ga_grow(&ga, (পূর্ণসংখ্যা)len) == OK)
	{
	    FOR_ALL_LIST_বস্তুসমূহ(l, li)
		ga_append(&ga, tv_get_সংখ্যা(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_তথ্য;
}

/*
 * "list2tuple()" function
 */
    শূন্য
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ফাঁকা;

    যদি (check_for_list_arg(argvars, 0) == FAIL)
	ফেরত;

    l = argvars[0].vval.v_list;
    যদি (l == ফাঁকা)
	ফেরত;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    যদি (rettv_tuple_set_with_বস্তুসমূহ(rettv, list_len(l)) == FAIL)
	ফেরত;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ITEM(tuple, TUPLE_LEN(tuple)));
	tuple->tv_বস্তুসমূহ.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    শূন্য
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (check_for_tuple_arg(argvars, 0) == FAIL)
	ফেরত;

    tuple = argvars[0].vval.v_tuple;
    যদি (tuple == ফাঁকা)
	ফেরত;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    জন্য (পূর্ণসংখ্যা i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ITEM(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    স্থির শূন্য
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*item, *item2;
    listitem_T	*li;
    পূর্ণসংখ্যা		error = FALSE;
    long	idx;
    long	end;
    পূর্ণসংখ্যা		cnt = 0;
    list_T	*rl;

    যদি ((l = argvars[0].vval.v_list) == ফাঁকা
			     || মান_check_lock(l->lv_lock, arg_errmsg, TRUE))
	ফেরত;

    idx = (long)tv_get_সংখ্যা_chk(&argvars[1], &error);
    যদি (error)
	ফেরত;		// type error: do nothing, errmsg already given

    যদি ((item = list_find(l, idx)) == ফাঁকা)
    {
	semsg(_(e_list_সূচক_out_of_range_nr), idx);
	ফেরত;
    }

    যদি (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	ফেরত;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_সংখ্যা_chk(&argvars[2], &error);
    যদি (error)
	ফেরত;		// type error: do nothing

    যদি ((item2 = list_find(l, end)) == ফাঁকা)
    {
	semsg(_(e_list_সূচক_out_of_range_nr), end);
	ফেরত;
    }

    জন্য (li = item; li != ফাঁকা; li = li->li_পরবর্তী)
    {
	++cnt;
	যদি (li == item2)
	    বিরতি;
    }
    যদি (li == ফাঁকা)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	ফেরত;
    }

    vimlist_remove(l, item, item2);
    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    rl = rettv->vval.v_list;

    যদি (l->lv_with_বস্তুসমূহ > 0)
    {
	// need to copy the list items and move the value
	যখন (item != ফাঁকা)
	{
	    li = listitem_alloc();
	    যদি (li == ফাঁকা)
		ফেরত;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    যদি (item == item2)
		বিরতি;
	    item = item->li_পরবর্তী;
	}
    }
    অন্যথা
    {
	rl->lv_first = item;
	rl->lv_u.mat.lv_last = item2;
	item->li_পূর্ব = ফাঁকা;
	item2->li_পরবর্তী = ফাঁকা;
	rl->lv_len = cnt;
    }
}

স্থির পূর্ণসংখ্যা item_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2);
স্থির পূর্ণসংখ্যা item_compare2(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2);

// struct used in the array that's given to qsort()
প্রকারলেখ কাঠামো
{
    listitem_T	*item;
    পূর্ণসংখ্যা		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    সংঘ {
	varnumber_T	inum;	// for "N"
	দ্বিগুণ		fnum;	// for "n" and "f"
    } চাবি;
} sortItem_T;

// struct storing information about current sort
প্রকারলেখ কাঠামো
{
    পূর্ণসংখ্যা		item_compare_ic;
    পূর্ণসংখ্যা		item_compare_lc;
    পূর্ণসংখ্যা		item_compare_numeric;
    পূর্ণসংখ্যা		item_compare_numbers;
    পূর্ণসংখ্যা		item_compare_float;
    char_u	*item_compare_func;
    partial_T	*item_compare_partial;
    dict_T	*item_compare_selfdict;
    পূর্ণসংখ্যা		item_compare_func_err;
    পূর্ণসংখ্যা		item_compare_keep_zero;
    পূর্ণসংখ্যা		item_compare_চাবিগুলি_ready;  // ptrs[].key is precomputed
} sortinfo_T;
স্থির sortinfo_T	*sortinfo = ফাঁকা;
#define ITEM_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    স্থির পূর্ণসংখ্যা
item_compare(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    sortItem_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = ফাঁকা, *tofree2 = ফাঁকা;
    পূর্ণসংখ্যা		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;
    tv1 = &si1->item->li_tv;
    tv2 = &si2->item->li_tv;

    যদি (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_চাবিগুলি_ready
					? si1->চাবি.inum : tv_to_সংখ্যা(tv1);
	varnumber_T	v2 = sortinfo->item_compare_চাবিগুলি_ready
					? si2->চাবি.inum : tv_to_সংখ্যা(tv2);

	ফেরত v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    যদি (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_চাবিগুলি_ready
					? si1->চাবি.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_চাবিগুলি_ready
					? si2->চাবি.fnum : tv_get_float(tv2);

	ফেরত v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    যদি (sortinfo->item_compare_numeric && sortinfo->item_compare_চাবিগুলি_ready)
    {
	দ্বিগুণ	n1 = si1->চাবি.fnum;
	দ্বিগুণ	n2 = si2->চাবি.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	যদি (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	ফেরত res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    যদি (tv1->v_type == VAR_STRING)
    {
	যদি (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	অন্যথা
	    p1 = tv1->vval.v_string;
    }
    অন্যথা
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    যদি (tv2->v_type == VAR_STRING)
    {
	যদি (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	অন্যথা
	    p2 = tv2->vval.v_string;
    }
    অন্যথা
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    যদি (p1 == ফাঁকা)
	p1 = (char_u *)"";
    যদি (p2 == ফাঁকা)
	p2 = (char_u *)"";
    যদি (!sortinfo->item_compare_numeric)
    {
	যদি (sortinfo->item_compare_lc)
	    res = strcoll((অক্ষর *)p1, (অক্ষর *)p2);
	অন্যথা
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    অন্যথা
    {
	দ্বিগুণ n1, n2;
	n1 = strtod((অক্ষর *)p1, (অক্ষর **)&p1);
	n2 = strtod((অক্ষর *)p2, (অক্ষর **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    যদি (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    ফেরত res;
}

    স্থির পূর্ণসংখ্যা
item_compare2(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2)
{
    sortItem_T  *si1, *si2;
    পূর্ণসংখ্যা		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->item_compare_partial;
    funcexe_T	funcexe;
    পূর্ণসংখ্যা		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    যদি (sortinfo->item_compare_func_err)
	ফেরত 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    যদি (partial == ফাঁকা)
	func_name = sortinfo->item_compare_func;
    অন্যথা
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

    যদি (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    অন্যথা
    {
	varnumber_T	n;

	n = tv_get_সংখ্যা_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    যদি (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    যদি (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    ফেরত res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    স্থির শূন্য
sort_compute_চাবিগুলি(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    যদি (info->item_compare_numbers)
    {
	জন্য (i = 0; i < len; ++i)
	    ptrs[i].চাবি.inum = tv_to_সংখ্যা(&ptrs[i].item->li_tv);
    }
    অন্যথা যদি (info->item_compare_float)
    {
	জন্য (i = 0; i < len; ++i)
	    ptrs[i].চাবি.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    অন্যথা // info->item_compare_numeric
    {
	জন্য (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    যদি (tv->v_type == VAR_STRING)
		ptrs[i].চাবি.fnum = 0.0;
	    অন্যথা
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = ফাঁকা;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].চাবি.fnum = p == ফাঁকা ? 0.0 : strtod((অক্ষর *)p, ফাঁকা);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_চাবিগুলি_ready = TRUE;
}

/*
 * sort() List "l"
 */
    স্থির শূন্য
do_sort(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    যদি (ptrs == ফাঁকা)
	ফেরত;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_বস্তুসমূহ(l, li)
    {
	ptrs[i].item = li;
	ptrs[i].idx = i;
	++i;
    }

    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = FALSE;
    info->item_compare_চাবিগুলি_ready = FALSE;

    যদি (info->item_compare_func == ফাঁকা && info->item_compare_partial == ফাঁকা
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_চাবিগুলি(ptrs, len, info);
    // test the compare function
    যদি ((info->item_compare_func != ফাঁকা
		|| info->item_compare_partial != ফাঁকা)
	    && item_compare2((শূন্য *)&ptrs[0], (শূন্য *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    অন্যথা
    {
	// Sort the array with item pointers.
	qsort((শূন্য *)ptrs, (size_t)len, আকার(sortItem_T),
		info->item_compare_func == ফাঁকা
		&& info->item_compare_partial == ফাঁকা
		? item_compare : item_compare2);

	যদি (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = ফাঁকা;
	    l->lv_len = 0;
	    জন্য (i = 0; i < len; ++i)
		list_append(l, ptrs[i].item);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    স্থির শূন্য
do_uniq(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;
    পূর্ণসংখ্যা	(*item_compare_func_ptr)(ধ্রুবক শূন্য *, ধ্রুবক শূন্য *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    যদি (ptrs == ফাঁকা)
	ফেরত;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != ফাঁকা
	|| info->item_compare_partial != ফাঁকা
	? item_compare2 : item_compare;

    জন্য (li = l->lv_first; li != ফাঁকা && li->li_পরবর্তী != ফাঁকা;
	    li = li->li_পরবর্তী)
    {
	যদি (item_compare_func_ptr((শূন্য *)&li, (শূন্য *)&li->li_পরবর্তী)
		== 0)
	    ptrs[i++].item = li;
	যদি (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    বিরতি;
	}
    }

    যদি (!info->item_compare_func_err)
    {
	যখন (--i >= 0)
	{
	    li = ptrs[i].item->li_পরবর্তী;
	    ptrs[i].item->li_পরবর্তী = li->li_পরবর্তী;
	    যদি (li->li_পরবর্তী != ফাঁকা)
		li->li_পরবর্তী->li_পূর্ব = ptrs[i].item;
	    অন্যথা
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
    স্থির পূর্ণসংখ্যা
parse_sort_uniq_args(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = ফাঁকা;
    info->item_compare_partial = ফাঁকা;
    info->item_compare_selfdict = ফাঁকা;
    info->item_compare_চাবিগুলি_ready = FALSE;

    যদি (argvars[1].v_type == VAR_UNKNOWN)
	ফেরত OK;

    // optional second argument: {func}
    যদি (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    অন্যথা যদি (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    অন্যথা
    {
	পূর্ণসংখ্যা	    error = FALSE;
	পূর্ণসংখ্যা	    nr = 0;

	যদি (argvars[1].v_type == VAR_সংখ্যা)
	{
	    nr = tv_get_সংখ্যা_chk(&argvars[1], &error);
	    যদি (error)
		ফেরত FAIL;
	    যদি (nr == 1)
		info->item_compare_ic = TRUE;
	}
	যদি (nr != 1)
	{
	    যদি (argvars[1].v_type != VAR_সংখ্যা)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    অন্যথা যদি (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		ফেরত FAIL;
	    }
	}
	যদি (info->item_compare_func != ফাঁকা)
	{
	    যদি (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = ফাঁকা;
	    }
	    অন্যথা যদি (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = ফাঁকা;
		info->item_compare_numeric = TRUE;
	    }
	    অন্যথা যদি (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = ফাঁকা;
		info->item_compare_numbers = TRUE;
	    }
	    অন্যথা যদি (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = ফাঁকা;
		info->item_compare_float = TRUE;
	    }
	    অন্যথা যদি (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = ফাঁকা;
		info->item_compare_ic = TRUE;
	    }
	    অন্যথা যদি (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = ফাঁকা;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	যদি (check_for_dict_arg(argvars, 2) == FAIL)
	    ফেরত FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    ফেরত OK;
}

/*
 * "sort()" or "uniq()" function
 */
    স্থির শূন্য
do_sort_uniq(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা sort)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    long	len;

    যদি (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	ফেরত;

    যদি (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	ফেরত;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    যদি (l != ফাঁকা && মান_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	যাও theend;
    rettv_list_set(rettv, l);
    যদি (l == ফাঁকা)
	যাও theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    যদি (len <= 1)
	যাও theend;	// short list sorts pretty quickly

    যদি (parse_sort_uniq_args(argvars, &info) == FAIL)
	যাও theend;

    যদি (sort)
	do_sort(l, &info);
    অন্যথা
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    শূন্য
f_sort(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    শূন্য
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    পূর্ণসংখ্যা
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*রাশিমালা,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	পূর্ণসংখ্যা		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    পূর্ণসংখ্যা		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    যদি (filtermap == FILTERMAP_FOREACH && রাশিমালা->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(রাশিমালা->vval.v_string);
	যদি (!did_emsg)
	    retval = OK;
	যাও theend;
    }

    argv[0] = *get_vim_var_tv(VV_চাবি);
    argv[1] = *get_vim_var_tv(VV_VAL);
    যদি (eval_রাশিমালা_typval(রাশিমালা, FALSE, argv, 2, fc, newtv) == FAIL)
	যাও theend;
    যদি (filtermap == FILTERMAP_FILTER)
    {
	পূর্ণসংখ্যা	    error = FALSE;

	// filter(): when expr is zero remove the item
	যদি (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	অন্যথা
	    *remp = (tv_get_সংখ্যা_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	যদি (error)
	    যাও theend;
    }
    অন্যথা যদি (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    ফেরত retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    স্থির শূন্য
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	অক্ষর		*func_name,
	char_u		*arg_errmsg,
	typval_T	*রাশিমালা,
	typval_T	*rettv)
{
    পূর্ণসংখ্যা		পূর্ব_lock;
    list_T	*l_ret = ফাঁকা;
    পূর্ণসংখ্যা		idx = 0;
    পূর্ণসংখ্যা		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    যদি (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = ফাঁকা;
    }
    যদি (l == ফাঁকা || (filtermap == FILTERMAP_FILTER
			    && মান_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	ফেরত;

    পূর্ব_lock = l->lv_lock;

    যদি (filtermap == FILTERMAP_MAPNEW)
    {
	যদি (rettv_list_alloc(rettv) == FAIL)
	    ফেরত;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_চাবি, VAR_সংখ্যা);

    যদি (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_রাশিমালা_get_funccal(রাশিমালা, &newtv);

    যদি (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	পূর্ণসংখ্যা		len = l->lv_len;
	পূর্ণসংখ্যা		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	যদি (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = ফাঁকা;
	    l->lv_u.mat.lv_last = ফাঁকা;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = ফাঁকা;
	}

	জন্য (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_সংখ্যা;
	    tv.v_lock = 0;
	    tv.vval.v_সংখ্যা = val;
	    set_vim_var_nr(VV_চাবি, idx);
	    যদি (filter_map_one(&tv, রাশিমালা, filtermap, fc, &newtv, &rem) == FAIL)
		বিরতি;
	    যদি (did_emsg)
	    {
		clear_tv(&newtv);
		বিরতি;
	    }
	    যদি (filtermap != FILTERMAP_FOREACH)
	    {
		যদি (filtermap != FILTERMAP_FILTER)
		{
		    যদি (filtermap == FILTERMAP_MAP && argtype != ফাঁকা
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			বিরতি;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    যদি (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			বিরতি;
		}
		অন্যথা যদি (!rem)
		{
		    // filter(): append the list item value when not rem
		    যদি (list_append_tv_move(l, &tv) == FAIL)
			বিরতি;
		}
	    }

	    val += stride;
	}
    }
    অন্যথা
    {
	// Materialized list: loop over the items
	জন্য (li = l->lv_first; li != ফাঁকা; li = nli)
	{
	    যদি (filtermap == FILTERMAP_MAP && মান_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		বিরতি;
	    nli = li->li_পরবর্তী;
	    set_vim_var_nr(VV_চাবি, idx);
	    যদি (filter_map_one(&li->li_tv, রাশিমালা, filtermap, fc,
							 &newtv, &rem) == FAIL)
		বিরতি;
	    যদি (did_emsg)
	    {
		clear_tv(&newtv);
		বিরতি;
	    }
	    যদি (filtermap == FILTERMAP_MAP)
	    {
		যদি (argtype != ফাঁকা && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    বিরতি;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_lock = 0;
		li->li_tv = newtv;
	    }
	    অন্যথা যদি (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		যদি (list_append_tv_move(l_ret, &newtv) == FAIL)
		    বিরতি;
	    }
	    অন্যথা যদি (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = পূর্ব_lock;
    যদি (fc != ফাঁকা)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    স্থির শূন্য
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*রাশিমালা;
    অক্ষর	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    পূর্ণসংখ্যা		save_did_emsg;
    type_T	*type = ফাঁকা;

    // map(), filter(), foreach() return the first argument, also on failure.
    যদি (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    যদি (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	ফেরত;

    যদি (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	ফেরত;
    }

    যদি (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	যদি (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != ফাঁকা)
	    type = argvars[0].vval.v_dict->dv_type;
	অন্যথা যদি (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != ফাঁকা)
	    type = argvars[0].vval.v_list->lv_type;
    }

    যদি (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	অক্ষর *বার্তা;

	যদি (filtermap == FILTERMAP_FOREACH)
	    বার্তা = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	অন্যথা
	    বার্তা = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(বার্তা), func_name);
	ফেরত;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    রাশিমালা = &argvars[1];
    যদি (রাশিমালা->v_type == VAR_UNKNOWN)
	ফেরত;

    typval_T	save_val;
    typval_T	save_চাবি;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_চাবি, &save_চাবি);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    যদি (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, রাশিমালা, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, রাশিমালা,
							    arg_errmsg, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, রাশিমালা, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, রাশিমালা);
    অন্যথা // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, রাশিমালা, rettv);

    restore_vimvar(VV_চাবি, &save_চাবি);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    শূন্য
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    শূন্য
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    শূন্য
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    শূন্য
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    স্থির শূন্য
list_add(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    যদি (l == ফাঁকা)
    {
	যদি (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    অন্যথা যদি (!মান_check_lock(l->lv_lock,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    শূন্য
f_add(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = 1; // Default: Failed

    যদি (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_সংখ্যা_arg(argvars, 1) == FAIL)))
	ফেরত;

    যদি (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    অন্যথা
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    স্থির long
list_গুনতি(list_T *l, typval_T *needle, long idx, পূর্ণসংখ্যা ic)
{
    long	n = 0;
    listitem_T	*li;

    যদি (l == ফাঁকা)
	ফেরত 0;

    CHECK_LIST_MATERIALIZE(l);

    যদি (list_len(l) == 0)
	ফেরত 0;

    li = list_find(l, idx);
    যদি (li == ফাঁকা)
    {
	semsg(_(e_list_সূচক_out_of_range_nr), idx);
	ফেরত 0;
    }

    জন্য ( ; li != ফাঁকা; li = li->li_পরবর্তী)
	যদি (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    ফেরত n;
}

/*
 * "count()" function
 */
    শূন্য
f_গুনতি(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    পূর্ণসংখ্যা		ic = FALSE;
    পূর্ণসংখ্যা		error = FALSE;

    যদি (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 3) == FAIL)))
	ফেরত;

    যদি (argvars[2].v_type != VAR_UNKNOWN)
	ic = (পূর্ণসংখ্যা)tv_get_bool_chk(&argvars[2], &error);

    যদি (!error && argvars[0].v_type == VAR_STRING)
	n = string_গুনতি(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    অন্যথা যদি (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	যদি (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_সংখ্যা_chk(&argvars[3], &error);
	যদি (!error)
	    n = list_গুনতি(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    অন্যথা যদি (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	যদি (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_সংখ্যা_chk(&argvars[3], &error);
	যদি (!error)
	    n = tuple_গুনতি(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    অন্যথা যদি (!error && argvars[0].v_type == VAR_DICT)
    {
	যদি (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	অন্যথা
	    n = dict_গুনতি(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    অন্যথা যদি (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_সংখ্যা = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    স্থির শূন্য
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	অক্ষর		*func_name,
	char_u		*arg_errmsg,
	পূর্ণসংখ্যা		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*item;
    long	before;
    পূর্ণসংখ্যা		error = FALSE;

    l1 = argvars[0].vval.v_list;
    যদি (l1 == ফাঁকা)
    {
	emsg(_(e_cannot_extend_null_list));
	ফেরত;
    }
    যদি (is_new || !মান_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	যদি (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    যদি (l1 == ফাঁকা)
		ফেরত;
	}

	l2 = argvars[1].vval.v_list;
	যদি (l2 == ফাঁকা)
	    যাও theend;

	যদি (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_সংখ্যা_chk(&argvars[2], &error);
	    যদি (error)
		যাও cleanup;		// type error; errmsg already given
	    যদি (before == l1->lv_len)
		item = ফাঁকা;
	    অন্যথা
	    {
		item = list_find(l1, before);
		যদি (item == ফাঁকা)
		{
		    semsg(_(e_list_সূচক_out_of_range_nr), before);
		    যাও cleanup;
		}
	    }
	}
	অন্যথা
	    item = ফাঁকা;
	যদি (type != ফাঁকা && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    যাও cleanup;
	list_extend(l1, l2, item);

theend:
	যদি (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	অন্যথা
	    copy_tv(&argvars[0], rettv);
	ফেরত;

cleanup:
	যদি (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    স্থির শূন্য
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, পূর্ণসংখ্যা is_new)
{
    type_T	*type = ফাঁকা;
    অক্ষর	*func_name = is_new ? "extendnew()" : "extend()";

    যদি (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	যদি (!is_new && in_vim9script() && argvars[0].vval.v_list != ফাঁকা)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    অন্যথা যদি (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	যদি (!is_new && in_vim9script() && argvars[0].vval.v_dict != ফাঁকা)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    অন্যথা যদি (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    অন্যথা
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    শূন্য
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
    শূন্য
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    স্থির শূন্য
list_ঢোকাও_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    পূর্ণসংখ্যা		error = FALSE;

    যদি (l == ফাঁকা)
    {
	যদি (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	ফেরত;
    }

    যদি (মান_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	ফেরত;

    যদি (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_সংখ্যা_chk(&argvars[2], &error);
    যদি (error)
	ফেরত;		// type error; errmsg already given

    যদি (before == l->lv_len)
	item = ফাঁকা;
    অন্যথা
    {
	item = list_find(l, before);
	যদি (item == ফাঁকা)
	{
	    semsg(_(e_list_সূচক_out_of_range_nr), before);
	    l = ফাঁকা;
	}
    }
    যদি (l != ফাঁকা)
    {
	(শূন্য)list_ঢোকাও_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    শূন্য
f_ঢোকাও(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_সংখ্যা_arg(argvars, 1) == FAIL)
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL))
	ফেরত;

    যদি (argvars[0].v_type == VAR_BLOB)
	blob_ঢোকাও_func(argvars, rettv);
    অন্যথা যদি (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    অন্যথা
	list_ঢোকাও_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    শূন্য
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    যদি (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_সংখ্যা_arg(argvars, 1) == FAIL
			|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_সংখ্যা_arg(argvars, 1) == FAIL)))
	ফেরত;

    যদি (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    অন্যথা যদি (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    অন্যথা যদি (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    অন্যথা
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    স্থির শূন্য
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    যদি (l != ফাঁকা
	    && !মান_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	যদি (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    ফেরত;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = ফাঁকা;
	l->lv_len = 0;
	যখন (li != ফাঁকা)
	{
	    ni = li->li_পূর্ব;
	    list_append(l, li);
	    li = ni;
	}
	l->lv_u.mat.lv_idx = l->lv_len - l->lv_u.mat.lv_idx - 1;
    }
}

/*
 * "reverse({list})" function
 */
    শূন্য
f_reverse(typval_T *argvars, typval_T *rettv)
{
    যদি (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	যদি (argvars[0].vval.v_string != ফাঁকা)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	অন্যথা
	    rettv->vval.v_string = ফাঁকা;
    }
    অন্যথা যদি (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_TUPLE)
	tuple_reverse(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    স্থির শূন্য
list_reduce(
	typval_T	*argvars,
	typval_T	*রাশিমালা,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = ফাঁকা;
    পূর্ণসংখ্যা		range_list;
    পূর্ণসংখ্যা		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    পূর্ণসংখ্যা		r;
    পূর্ণসংখ্যা		called_emsg_start = called_emsg;
    পূর্ণসংখ্যা		পূর্ব_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != ফাঁকা && l->lv_first == &range_list_item;
    যদি (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    যদি (argvars[2].v_type == VAR_UNKNOWN)
    {
	যদি (l == ফাঁকা || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_মান), "List");
	    ফেরত;
	}
	যদি (range_list)
	{
	    initial.v_type = VAR_সংখ্যা;
	    initial.vval.v_সংখ্যা = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	অন্যথা
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_পরবর্তী;
	}
    }
    অন্যথা
    {
	initial = argvars[2];
	যদি (l != ফাঁকা && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    যদি (l == ফাঁকা)
	ফেরত;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_রাশিমালা_get_funccal(রাশিমালা, rettv);

    পূর্ব_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    যখন (range_list ? range_idx < l->lv_len : li != ফাঁকা)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	যদি (range_list)
	{
	    argv[1].v_type = VAR_সংখ্যা;
	    argv[1].vval.v_সংখ্যা = range_val;
	}
	অন্যথা
	    argv[1] = li->li_tv;

	r = eval_রাশিমালা_typval(রাশিমালা, TRUE, argv, 2, fc, rettv);

	যদি (argv[0].v_type != VAR_সংখ্যা && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	যদি (r == FAIL || called_emsg != called_emsg_start)
	    বিরতি;

	// advance to the next item
	যদি (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	অন্যথা
	    li = li->li_পরবর্তী;
    }

    যদি (fc != ফাঁকা)
	remove_funccal();

    l->lv_lock = পূর্ব_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    শূন্য
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    যদি (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    অন্যথা যদি (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    অন্যথা
	func_name = tv_get_string_strict(&argvars[1]);
    যদি (func_name == ফাঁকা || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	ফেরত;
    }

    যদি (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    অন্যথা যদি (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    অন্যথা
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    শূন্য
f_slice(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_opt_সংখ্যা_arg(argvars, 2) == FAIL))
	ফেরত;

    যদি (check_can_সূচক(&argvars[0], TRUE, FALSE) != OK)
	ফেরত;

    copy_tv(argvars, rettv);
    eval_সূচক_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? ফাঁকা : argvars + 2,
	    TRUE, ফাঁকা, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
