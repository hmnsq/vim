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

#अन्तर्भाव "vim.h"

#यदि defined(FEAT_EVAL)

// List heads for garbage collection.
स्थिर list_T		*first_list = रिक्त;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    प्रति ((lw) = (l)->lv_watch; (lw) != रिक्त; (lw) = (lw)->lw_next)

स्थिर शून्य list_free_item(list_T *l, listitem_T *item);

/*
 * Add a watcher to a list.
 */
    शून्य
list_add_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_next = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    शून्य
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	यदि (lw == lwrem)
	{
	    *lwp = lw->lw_next;
	    भङ्ग;
	}
	lwp = &lw->lw_next;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    स्थिर शून्य
list_fix_watch(list_T *l, listitem_T *item)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	यदि (lw->lw_item == item)
	    lw->lw_item = item->li_next;
}

    स्थिर शून्य
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    यदि (first_list != रिक्त)
	first_list->lv_used_prev = l;
    l->lv_used_prev = रिक्त;
    l->lv_used_next = first_list;
    first_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(शून्य)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    यदि (l != रिक्त)
	list_init(l);
    प्रत्यागम l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    यदि (alloc_fail_id == id && alloc_does_fail(परिमाण(list_T)))
	प्रत्यागम रिक्त;
#endif
    प्रत्यागम (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_वस्तुसंच(पूर्णाङ्क मोजणी)
{
    list_T	*l;

    यदि (मोजणी > 0
	    && (size_t)मोजणी > (SIZE_MAX - परिमाण(list_T)) / परिमाण(listitem_T))
	प्रत्यागम रिक्त;
    l = (list_T *)alloc_clear(परिमाण(list_T) + मोजणी * परिमाण(listitem_T));
    यदि (l == रिक्त)
	प्रत्यागम रिक्त;

    list_init(l);

    यदि (मोजणी <= 0)
	प्रत्यागम l;

    listitem_T	*li = (listitem_T *)(l + 1);
    पूर्णाङ्क		i;

    l->lv_len = मोजणी;
    l->lv_with_वस्तुसंच = मोजणी;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + मोजणी - 1;
    प्रति (i = 0; i < मोजणी; ++i)
    {
	यदि (i == 0)
	    li->li_prev = रिक्त;
	अन्यथा
	    li->li_prev = li - 1;
	यदि (i == मोजणी - 1)
	    li->li_next = रिक्त;
	अन्यथा
	    li->li_next = li + 1;
	++li;
    }

    प्रत्यागम l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    शून्य
list_set_item(list_T *l, पूर्णाङ्क idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    यदि (l == रिक्त)
	प्रत्यागम FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    प्रत्यागम OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    पूर्णाङ्क
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    यदि (alloc_fail_id == id && alloc_does_fail(परिमाण(list_T)))
	प्रत्यागम FAIL;
#endif
    प्रत्यागम rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    शून्य
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    यदि (l != रिक्त)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    शून्य
list_unref(list_T *l)
{
    यदि (l != रिक्त && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    स्थिर शून्य
list_free_contents(list_T *l)
{
    listitem_T *item;

    यदि (l->lv_first != &range_list_item)
	प्रति (item = l->lv_first; item != रिक्त; item = l->lv_first)
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
    पूर्णाङ्क
list_free_nonref(पूर्णाङ्क copyID)
{
    list_T	*ll;
    पूर्णाङ्क		did_free = FALSE;

    प्रति (ll = first_list; ll != रिक्त; ll = ll->lv_used_next)
	यदि ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == रिक्त)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    प्रत्यागम did_free;
}

    स्थिर शून्य
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    यदि (l->lv_used_prev == रिक्त)
	first_list = l->lv_used_next;
    अन्यथा
	l->lv_used_prev->lv_used_next = l->lv_used_next;
    यदि (l->lv_used_next != रिक्त)
	l->lv_used_next->lv_used_prev = l->lv_used_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    शून्य
list_free_वस्तुसंच(पूर्णाङ्क copyID)
{
    list_T	*ll, *ll_next;

    प्रति (ll = first_list; ll != रिक्त; ll = ll_next)
    {
	ll_next = ll->lv_used_next;
	यदि ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == रिक्त)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    शून्य
list_free(list_T *l)
{
    यदि (in_free_unref_वस्तुसंच)
	प्रत्यागम;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(शून्य)
{
    प्रत्यागम ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    स्थिर शून्य
list_free_item(list_T *l, listitem_T *item)
{
    यदि (l->lv_with_वस्तुसंच == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_वस्तुसंच)
	vim_free(item);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    शून्य
listitem_free(list_T *l, listitem_T *item)
{
    clear_tv(&item->li_tv);
    list_free_item(l, item);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    शून्य
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
    यदि (l == रिक्त)
	प्रत्यागम 0L;
    प्रत्यागम l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    पूर्णाङ्क
list_equal(
    list_T	*l1,
    list_T	*l2,
    पूर्णाङ्क		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    यदि (l1 == l2)
	प्रत्यागम TRUE;
    यदि (list_len(l1) != list_len(l2))
	प्रत्यागम FALSE;
    यदि (list_len(l1) == 0)
	// empty and NULL list are considered equal
	प्रत्यागम TRUE;
    यदि (l1 == रिक्त || l2 == रिक्त)
	प्रत्यागम FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    प्रति (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != रिक्त && item2 != रिक्त;
			       item1 = item1->li_next, item2 = item2->li_next)
	यदि (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    प्रत्यागम FALSE;
    प्रत्यागम item1 == रिक्त && item2 == रिक्त;
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

    यदि (l == रिक्त)
	प्रत्यागम रिक्त;

    // Negative index is relative to the end.
    यदि (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    यदि (n < 0 || n >= l->lv_len)
	प्रत्यागम रिक्त;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    यदि (n >= l->lv_len)
	प्रत्यागम रिक्त;

    // When there is a cached index may start search from there.
    यदि (l->lv_u.mat.lv_idx_item != रिक्त)
    {
	यदि (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	अन्यथा यदि (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	अन्यथा
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    अन्यथा
    {
	यदि (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	अन्यथा
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    यावत् (n > idx)
    {
	// search forward
	item = item->li_next;
	++idx;
    }
    यावत् (n < idx)
    {
	// search backward
	item = item->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    प्रत्यागम item;
}

/*
 * Get list item "l[idx]" as a number.
 */
    long
list_find_nr(
    list_T	*l,
    long	idx,
    पूर्णाङ्क		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    यदि (l != रिक्त && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	यदि (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	यदि (n < 0 || n >= l->lv_len)
	{
	    यदि (errorp != रिक्त)
		*errorp = TRUE;
	    प्रत्यागम -1L;
	}

	प्रत्यागम l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    यदि (li == रिक्त)
    {
	यदि (errorp != रिक्त)
	    *errorp = TRUE;
	प्रत्यागम -1L;
    }
    प्रत्यागम (long)tv_get_संख्या_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    यदि (li == रिक्त)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	प्रत्यागम रिक्त;
    }
    प्रत्यागम tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_index(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    यदि (li != रिक्त)
	प्रत्यागम li;

    यदि (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    प्रत्यागम li;
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

    यदि (l == रिक्त)
	प्रत्यागम -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    प्रति (li = l->lv_first; li != रिक्त && li != item; li = li->li_next)
	++idx;
    यदि (li == रिक्त)
	प्रत्यागम -1;
    प्रत्यागम idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    शून्य
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    यदि (l->lv_u.mat.lv_last == रिक्त)
    {
	// empty list
	l->lv_first = item;
	item->li_prev = रिक्त;
    }
    अन्यथा
    {
	l->lv_u.mat.lv_last->li_next = item;
	item->li_prev = l->lv_u.mat.lv_last;
    }
    l->lv_u.mat.lv_last = item;
    ++l->lv_len;
    item->li_next = रिक्त;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    पूर्णाङ्क
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    यदि (l->lv_type != रिक्त && l->lv_type->tt_member != रिक्त
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      रिक्त, 0) == FAIL)
	प्रत्यागम FAIL;
    li = listitem_alloc();
    यदि (li == रिक्त)
	प्रत्यागम FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    प्रत्यागम OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    स्थिर पूर्णाङ्क
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    यदि (li == रिक्त)
	प्रत्यागम FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    प्रत्यागम OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    पूर्णाङ्क
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    यदि (li == रिक्त)
	प्रत्यागम FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    प्रत्यागम OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    पूर्णाङ्क
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    यदि (li == रिक्त)
	प्रत्यागम FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    प्रत्यागम OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    पूर्णाङ्क
list_append_string(list_T *l, char_u *str, पूर्णाङ्क len)
{
    listitem_T *li = listitem_alloc();

    यदि (li == रिक्त)
	प्रत्यागम FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    यदि (str == रिक्त)
	li->li_tv.vval.v_string = रिक्त;
    अन्यथा यदि ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == रिक्त)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    पूर्णाङ्क
list_append_संख्या(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    यदि (li == रिक्त)
	प्रत्यागम FAIL;
    li->li_tv.v_type = VAR_संख्या;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_संख्या = n;
    list_append(l, li);
    प्रत्यागम OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    पूर्णाङ्क
list_घाला_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    यदि (l->lv_type != रिक्त && l->lv_type->tt_member != रिक्त
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      रिक्त, 0) == FAIL)
	प्रत्यागम FAIL;
    ni = listitem_alloc();
    यदि (ni == रिक्त)
	प्रत्यागम FAIL;
    copy_tv(tv, &ni->li_tv);
    list_घाला(l, ni, item);
    प्रत्यागम OK;
}

    शून्य
list_घाला(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    यदि (item == रिक्त)
	// Append new item at end of list.
	list_append(l, ni);
    अन्यथा
    {
	// Insert new item before existing item.
	ni->li_prev = item->li_prev;
	ni->li_next = item;
	यदि (item->li_prev == रिक्त)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	अन्यथा
	{
	    item->li_prev->li_next = ni;
	    l->lv_u.mat.lv_idx_item = रिक्त;
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
    स्थिर पूर्णाङ्क
list_append_new_item(list_T *l)
{
    typval_T	tv;

    यदि (l->lv_type != रिक्त && l->lv_type->tt_member != रिक्त)
	tv.v_type = l->lv_type->tt_member->tt_type;
    अन्यथा
	tv.v_type = VAR_संख्या;
    tv.v_lock = 0;

    विकल्प (tv.v_type)
    {
	पक्ष VAR_BOOL: tv.vval.v_संख्या = VVAL_FALSE; भङ्ग;
	पक्ष VAR_SPECIAL: tv.vval.v_संख्या = 0; भङ्ग;
	पक्ष VAR_संख्या: tv.vval.v_संख्या = 0; भङ्ग;
	पक्ष VAR_FLOAT: tv.vval.v_float = 0; भङ्ग;
	पक्ष VAR_STRING: tv.vval.v_string = रिक्त; भङ्ग;
	पक्ष VAR_BLOB: tv.vval.v_blob = blob_alloc(); भङ्ग;
	पक्ष VAR_FUNC: tv.vval.v_string = रिक्त; भङ्ग;
	पक्ष VAR_PARTIAL: tv.vval.v_partial = रिक्त; भङ्ग;
	पक्ष VAR_LIST: tv.vval.v_list = list_alloc(); भङ्ग;
	पक्ष VAR_DICT: tv.vval.v_dict = dict_alloc(); भङ्ग;
#ifdef FEAT_JOB_CHANNEL
	पक्ष VAR_JOB: tv.vval.v_job = रिक्त; भङ्ग;
	पक्ष VAR_CHANNEL: tv.vval.v_channel = रिक्त; भङ्ग;
#endif
	पक्ष VAR_CLASS:
	    यदि (l->lv_type != रिक्त && l->lv_type->tt_member != रिक्त)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    अन्यथा
		tv.vval.v_class = रिक्त;
	    भङ्ग;
	पक्ष VAR_OBJECT:
	    यदि (l->lv_type != रिक्त && l->lv_type->tt_member != रिक्त)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    अन्यथा
		tv.vval.v_object = रिक्त;
	    भङ्ग;
	पक्ष VAR_TYPEALIAS: tv.vval.v_typealias = रिक्त; भङ्ग;
	पक्ष VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); भङ्ग;
	स्वभाव:
	    tv.v_type = VAR_संख्या;
	    tv.vval.v_संख्या = 0;
	    भङ्ग;
    }

    यदि (l->lv_type != रिक्त && l->lv_type->tt_member != रिक्त)
	set_tv_type(&tv, l->lv_type->tt_member);

    प्रत्यागम list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_index_one(list_T *l, long *n1, पूर्णाङ्क can_append, पूर्णाङ्क quiet)
{
    long	orig_n1 = *n1;
    listitem_T	*li = list_find_index(l, n1);

    यदि (li != रिक्त)
	प्रत्यागम li;

    // Vim9: Allow for adding an item at the end.
    यदि (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	यदि (list_append_new_item(l) == FAIL)
	    प्रत्यागम रिक्त;
	li = list_find_index(l, n1);
    }
    यदि (li == रिक्त)
    {
	यदि (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), orig_n1);
	प्रत्यागम रिक्त;
    }
    प्रत्यागम li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    पूर्णाङ्क
check_range_index_two(
	list_T	    *l,
	long	    *n1,
	listitem_T  *li1,
	long	    *n2,
	पूर्णाङ्क	    quiet)
{
    यदि (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	यदि (ni == रिक्त)
	{
	    यदि (!quiet)
		semsg(_(e_list_index_out_of_range_nr), *n2);
	    प्रत्यागम FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    यदि (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    यदि (*n2 < *n1)
    {
	यदि (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), *n2);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
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
    पूर्णाङ्क
list_assign_range(
	list_T	    *dest,
	list_T	    *src,
	long	    idx1_arg,
	long	    idx2,
	पूर्णाङ्क	    empty_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    long	idx1 = idx1_arg;
    listitem_T	*first_li = list_find_index(dest, &idx1);
    long	idx;
    type_T	*member_type = रिक्त;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = first_li;
    प्रति (src_li = src->lv_first; src_li != रिक्त && dest_li != रिक्त; )
    {
	यदि (मूल्य_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    प्रत्यागम FAIL;
	src_li = src_li->li_next;
	यदि (src_li == रिक्त || (!empty_idx2 && idx2 == idx))
	    भङ्ग;
	dest_li = dest_li->li_next;
	++idx;
    }

    यदि (in_vim9script() && dest->lv_type != रिक्त
					   && dest->lv_type->tt_member != रिक्त)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    प्रति (src_li = src->lv_first; src_li != रिक्त; )
    {
	यदि (op != रिक्त && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	अन्यथा
	{
	    यदि (member_type != रिक्त
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      रिक्त, 0) == FAIL)
		प्रत्यागम FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_next;
	यदि (src_li == रिक्त || (!empty_idx2 && idx2 == idx))
	    भङ्ग;
	यदि (dest_li->li_next == रिक्त)
	{
	    // Need to add an empty item.
	    यदि (list_append_संख्या(dest, 0) == FAIL)
	    {
		src_li = रिक्त;
		भङ्ग;
	    }
	}
	dest_li = dest_li->li_next;
	++idx;
    }
    यदि (src_li != रिक्त)
    {
	emsg(_(e_list_मूल्य_has_more_वस्तुसंच_than_targets));
	प्रत्यागम FAIL;
    }
    यदि (empty_idx2
	    ? (dest_li != रिक्त && dest_li->li_next != रिक्त)
	    : idx != idx2)
    {
	emsg(_(e_list_मूल्य_does_not_have_enough_वस्तुसंच));
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    स्थिर शून्य
list_flatten(list_T *list, listitem_T *first, long maxitems, long maxdepth)
{
    listitem_T	*item;
    पूर्णाङ्क		done = 0;

    यदि (maxdepth == 0)
	प्रत्यागम;
    CHECK_LIST_MATERIALIZE(list);
    यदि (first == रिक्त)
	item = list->lv_first;
    अन्यथा
	item = first;

    यावत् (item != रिक्त && done < maxitems)
    {
	listitem_T	*next = item->li_next;

	fast_breakcheck();
	यदि (got_int)
	    प्रत्यागम;

	यदि (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    यदि (list_extend(list, itemlist, next) == FAIL)
	    {
		list_free_item(list, item);
		प्रत्यागम;
	    }

	    यदि (maxdepth > 0)
		list_flatten(list, item->li_prev == रिक्त
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
    स्थिर शून्य
flatten_common(typval_T *argvars, typval_T *rettv, पूर्णाङ्क make_copy)
{
    list_T  *l;
    long    maxdepth;
    पूर्णाङ्क	    error = FALSE;

    यदि (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    यदि (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	प्रत्यागम;
    }

    यदि (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    अन्यथा
    {
	maxdepth = (long)tv_get_संख्या_chk(&argvars[1], &error);
	यदि (error)
	    प्रत्यागम;
	यदि (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_संख्या));
	    प्रत्यागम;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    यदि (l == रिक्त)
	प्रत्यागम;

    यदि (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	यदि (l == रिक्त)
	    प्रत्यागम;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = रिक्त;
    }
    अन्यथा
    {
	यदि (मूल्य_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    प्रत्यागम;
	++l->lv_refcount;
    }

    list_flatten(l, रिक्त, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    शून्य
f_flatten(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    अन्यथा
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    शून्य
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    शून्य
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;
    यदि (l == रिक्त)
	प्रत्यागम;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    प्रति (idx = 0, li = l->lv_first; li != रिक्त; li = li->li_next, ++idx)
    {
	list_T	    *l2 = list_alloc();

	यदि (l2 == रिक्त)
	    भङ्ग;
	यदि (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    भङ्ग;
	}
	यदि (list_append_संख्या(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    भङ्ग;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    शून्य
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;
    यदि (p == रिक्त)
	प्रत्यागम;  // null string behaves like an empty string

    प्रति (idx = 0; *p != NUL; ++idx)
    {
	पूर्णाङ्क	    len = mb_ptr2len(p);
	list_T	    *l2;

	यदि (len == 0)
	    भङ्ग;
	l2 = list_alloc();
	यदि (l2 == रिक्त)
	    भङ्ग;
	यदि (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    भङ्ग;
	}
	यदि (list_append_संख्या(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    भङ्ग;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    पूर्णाङ्क
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*item;
    पूर्णाङ्क		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    यदि (l2 == रिक्त || l2->lv_len == 0)
	प्रत्यागम OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == रिक्त ? रिक्त : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    प्रति (item = l2->lv_first; item != रिक्त && --todo >= 0;
				 item = item == bef_prev ? bef : item->li_next)
	यदि (list_घाला_tv(l1, &item->li_tv, bef) == FAIL)
	    प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    पूर्णाङ्क
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    पूर्णाङ्क		len1 = l1 == रिक्त ? 0 : l1->lv_len;
    पूर्णाङ्क		len2 = l2 == रिक्त ? 0 : l2->lv_len;
    long	totallen = (long)len1 + (long)len2;
    पूर्णाङ्क		i;
    listitem_T	*item;

    यदि (totallen == 0)
    {
	l = list_alloc();
	यदि (l == रिक्त)
	    प्रत्यागम FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	प्रत्यागम OK;
    }
    यदि (totallen > INT_MAX)
	प्रत्यागम FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_वस्तुसंच((पूर्णाङ्क)totallen);
    यदि (l == रिक्त)
	प्रत्यागम FAIL;

    i = 0;
    यदि (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	प्रति (item = l1->lv_first; item != रिक्त && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    यदि (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	प्रति (item = l2->lv_first; item != रिक्त && !got_int;
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
    प्रत्यागम OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	मोजणी;
    long	idx;

    यदि (n2 < n1)
	प्रत्यागम list_alloc();
    यदि (n1 > INT_MAX - (n2 - n1) - 1)
	प्रत्यागम रिक्त;
    मोजणी = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_वस्तुसंच((पूर्णाङ्क)मोजणी);
    यदि (l == रिक्त)
	प्रत्यागम रिक्त;

    item = list_find(ol, n1);
    प्रति (idx = 0; idx < मोजणी; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	यदि (item == रिक्त)
	{
	    list_free(l);
	    प्रत्यागम रिक्त;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (पूर्णाङ्क)idx, &new_tv);
	item = item->li_next;
    }
    प्रत्यागम l;
}

    पूर्णाङ्क
list_slice_or_index(
	    list_T	*list,
	    पूर्णाङ्क		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    पूर्णाङ्क		exclusive,
	    typval_T	*rettv,
	    पूर्णाङ्क		verbose)
{
    long	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    यदि (n1 < 0)
	n1 = len + n1;
    यदि (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	यदि (!range)
	{
	    यदि (verbose)
		semsg(_(e_list_index_out_of_range_nr), (long)n1_arg);
	    प्रत्यागम FAIL;
	}
	यदि (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	अन्यथा
	    n1 = len;
    }
    यदि (range)
    {
	list_T	*l;

	यदि (n2 < 0)
	    n2 = len + n2;
	अन्यथा यदि (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	यदि (exclusive)
	    --n2;
	यदि (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	यदि (l == रिक्त)
	    प्रत्यागम FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    अन्यथा
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long index = n1;
	listitem_T *li = check_range_index_one(list, &index, TRUE, TRUE);
	यदि (li == रिक्त)
	    प्रत्यागम FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	यदि (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != रिक्त
					&& var1.vval.v_list->lv_type == रिक्त)
	{
	    यदि (list->lv_type != रिक्त && list->lv_type->tt_member != रिक्त)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    प्रत्यागम OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, पूर्णाङ्क deep, पूर्णाङ्क शीर्ष, पूर्णाङ्क copyID)
{
    list_T	*copy;
    listitem_T	*item;
    पूर्णाङ्क		idx = 0;

    यदि (orig == रिक्त)
	प्रत्यागम रिक्त;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_वस्तुसंच(orig->lv_len);
    यदि (copy == रिक्त)
	प्रत्यागम रिक्त;

    यदि (orig->lv_type == रिक्त || शीर्ष || deep)
	copy->lv_type = रिक्त;
    अन्यथा
	copy->lv_type = alloc_type(orig->lv_type);
    यदि (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    प्रति (item = orig->lv_first; item != रिक्त && !got_int;
	    item = item->li_next)
    {
	typval_T	new_tv;

	यदि (deep)
	{
	    यदि (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		भङ्ग;
	}
	अन्यथा
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    यदि (item != रिक्त)
    {
	list_unref(copy);
	copy = रिक्त;
    }

    प्रत्यागम copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    शून्य
vimlist_remove(list_T *l, listitem_T *item, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    प्रति (ip = item; ip != रिक्त; ip = ip->li_next)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	यदि (ip == item2)
	    भङ्ग;
    }

    यदि (item2->li_next == रिक्त)
	l->lv_u.mat.lv_last = item->li_prev;
    अन्यथा
	item2->li_next->li_prev = item->li_prev;
    यदि (item->li_prev == रिक्त)
	l->lv_first = item2->li_next;
    अन्यथा
	item->li_prev->li_next = item2->li_next;
    l->lv_u.mat.lv_idx_item = रिक्त;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, पूर्णाङ्क copyID, पूर्णाङ्क restore_copyID)
{
    garray_T	ga;

    यदि (tv->vval.v_list == रिक्त)
	प्रत्यागम रिक्त;
    ga_init2(&ga, परिमाण(वर्ण), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    यदि (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	प्रत्यागम रिक्त;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    प्रत्यागम (char_u *)ga.ga_data;
}

प्रकारनिर्देश संरचना join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    स्थिर पूर्णाङ्क
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    पूर्णाङ्क		echo_style,
    पूर्णाङ्क		restore_copyID,
    पूर्णाङ्क		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    पूर्णाङ्क		i;
    join_T	*p;
    long	sumlen = 0;
    पूर्णाङ्क		first = TRUE;
    पूर्णाङ्क		prev_lock;
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
    यदि (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    प्रति (item = l->lv_first; item != रिक्त && !got_int; item = item->li_next)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	यदि (s.string == रिक्त)
	{
	    l->lv_lock = prev_lock;
	    प्रत्यागम FAIL;
	}

	s.length = STRLEN(s.string);
	sumlen += (long)s.length;

	(शून्य)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	यदि (tofree != रिक्त || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	अन्यथा
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	ओळ_breakcheck();
	यदि (did_echo_string_emsg)  // recursion error, bail out
	    भङ्ग;
    }
    l->lv_lock = prev_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    यदि (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    यदि (sumlen > INT_MAX - 2 || ga_grow(gap, (पूर्णाङ्क)sumlen + 2) == FAIL)
	प्रत्यागम FAIL;

    प्रति (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	यदि (first)
	    first = FALSE;
	अन्यथा
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	यदि (p->s.string != रिक्त)
	    ga_concat_len(gap, p->s.string, p->s.length);
	ओळ_breakcheck();
    }

    प्रत्यागम OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    पूर्णाङ्क
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    पूर्णाङ्क		echo_style,
    पूर्णाङ्क		restore_copyID,
    पूर्णाङ्क		copyID)
{
    garray_T	join_ga;
    पूर्णाङ्क		retval;
    join_T	*p;
    पूर्णाङ्क		i;

    यदि (l->lv_len < 1)
	प्रत्यागम OK; // nothing to do
    ga_init2(&join_ga, परिमाण(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    यदि (join_ga.ga_data == रिक्त)
	प्रत्यागम retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    प्रति (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    प्रत्यागम retval;
}

/*
 * "join()" function
 */
    शून्य
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    यदि (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	प्रत्यागम;

    यदि ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == रिक्त)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == रिक्त))
	प्रत्यागम;

    यदि (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    अन्यथा
	sep = tv_get_string_chk(&argvars[1]);

    यदि (sep != रिक्त)
    {
	ga_init2(&ga, परिमाण(वर्ण), 80);
	यदि (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	अन्यथा
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
    अन्यथा
	rettv->vval.v_string = रिक्त;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    पूर्णाङ्क
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, पूर्णाङ्क do_error)
{
    पूर्णाङ्क		evaluate = evalarg == रिक्त ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = रिक्त;
    typval_T	tv;
    listitem_T	*item;
    पूर्णाङ्क		vim9script = in_vim9script();
    पूर्णाङ्क		had_comma;

    यदि (evaluate)
    {
	l = list_alloc();
	यदि (l == रिक्त)
	    प्रत्यागम FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    यावत् (**arg != ']' && **arg != NUL)
    {
	यदि (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    गच्छ failret;
	यदि (check_typval_is_मूल्य(&tv) == FAIL)
	{
	    यदि (evaluate)
		clear_tv(&tv);
	    गच्छ failret;
	}
	यदि (evaluate)
	{
	    item = listitem_alloc();
	    यदि (item != रिक्त)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    अन्यथा
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	यदि (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	यदि (had_comma)
	{
	    यदि (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		गच्छ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	यदि (**arg == ']')
	    भङ्ग;

	यदि (!had_comma)
	{
	    यदि (do_error)
	    {
		यदि (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		अन्यथा
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    गच्छ failret;
	}
    }

    यदि (**arg != ']')
    {
	यदि (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	यदि (evaluate)
	    list_free(l);
	प्रत्यागम FAIL;
    }

    *arg += 1;
    यदि (evaluate)
	rettv_list_set(rettv, l);

    प्रत्यागम OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    पूर्णाङ्क
write_list(FILE *fd, list_T *list, पूर्णाङ्क द्विमान)
{
    listitem_T	*li;
    पूर्णाङ्क		c;
    पूर्णाङ्क		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_वस्तुसंच(list, li)
    {
	प्रति (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    यदि (*s == '\n')
		c = putc(NUL, fd);
	    अन्यथा
		c = putc(*s, fd);
	    यदि (c == EOF)
	    {
		ret = FAIL;
		भङ्ग;
	    }
	}
	यदि (!द्विमान || li->li_next != रिक्त)
	    यदि (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		भङ्ग;
	    }
	यदि (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    भङ्ग;
	}
    }
    प्रत्यागम ret;
}

/*
 * Initialize a static list with 10 items.
 */
    शून्य
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    पूर्णाङ्क	    i;

    CLEAR_POINTER(sl);
    l->lv_first = &sl->sl_वस्तुसंच[0];
    l->lv_u.mat.lv_last = &sl->sl_वस्तुसंच[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    प्रति (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_वस्तुसंच[i];

	यदि (i == 0)
	    li->li_prev = रिक्त;
	अन्यथा
	    li->li_prev = li - 1;
	यदि (i == 9)
	    li->li_next = रिक्त;
	अन्यथा
	    li->li_next = li + 1;
    }
}

/*
 * "list2str()" function
 */
    शून्य
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    पूर्णाङ्क		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    यदि (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    यदि (check_for_list_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    l = argvars[0].vval.v_list;
    यदि (l == रिक्त)
	प्रत्यागम;  // empty list results in empty string

    यदि (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (पूर्णाङ्क)tv_get_bool_chk(&argvars[1], रिक्त);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    यदि (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	पूर्णाङ्क	(*char2bytes)(पूर्णाङ्क, char_u *);

	यदि (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	अन्यथा
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_वस्तुसंच(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_संख्या(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    अन्यथा
    {
	long len = (long)list_len(l) + 1;

	यदि (len <= INT_MAX && ga_grow(&ga, (पूर्णाङ्क)len) == OK)
	{
	    FOR_ALL_LIST_वस्तुसंच(l, li)
		ga_append(&ga, tv_get_संख्या(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_data;
}

/*
 * "list2tuple()" function
 */
    शून्य
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = रिक्त;

    यदि (check_for_list_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    l = argvars[0].vval.v_list;
    यदि (l == रिक्त)
	प्रत्यागम;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    यदि (rettv_tuple_set_with_वस्तुसंच(rettv, list_len(l)) == FAIL)
	प्रत्यागम;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ITEM(tuple, TUPLE_LEN(tuple)));
	tuple->tv_वस्तुसंच.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    शून्य
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (check_for_tuple_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    tuple = argvars[0].vval.v_tuple;
    यदि (tuple == रिक्त)
	प्रत्यागम;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    प्रति (पूर्णाङ्क i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ITEM(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    स्थिर शून्य
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*item, *item2;
    listitem_T	*li;
    पूर्णाङ्क		error = FALSE;
    long	idx;
    long	end;
    पूर्णाङ्क		cnt = 0;
    list_T	*rl;

    यदि ((l = argvars[0].vval.v_list) == रिक्त
			     || मूल्य_check_lock(l->lv_lock, arg_errmsg, TRUE))
	प्रत्यागम;

    idx = (long)tv_get_संख्या_chk(&argvars[1], &error);
    यदि (error)
	प्रत्यागम;		// type error: do nothing, errmsg already given

    यदि ((item = list_find(l, idx)) == रिक्त)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	प्रत्यागम;
    }

    यदि (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	प्रत्यागम;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_संख्या_chk(&argvars[2], &error);
    यदि (error)
	प्रत्यागम;		// type error: do nothing

    यदि ((item2 = list_find(l, end)) == रिक्त)
    {
	semsg(_(e_list_index_out_of_range_nr), end);
	प्रत्यागम;
    }

    प्रति (li = item; li != रिक्त; li = li->li_next)
    {
	++cnt;
	यदि (li == item2)
	    भङ्ग;
    }
    यदि (li == रिक्त)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	प्रत्यागम;
    }

    vimlist_remove(l, item, item2);
    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    rl = rettv->vval.v_list;

    यदि (l->lv_with_वस्तुसंच > 0)
    {
	// need to copy the list items and move the value
	यावत् (item != रिक्त)
	{
	    li = listitem_alloc();
	    यदि (li == रिक्त)
		प्रत्यागम;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    यदि (item == item2)
		भङ्ग;
	    item = item->li_next;
	}
    }
    अन्यथा
    {
	rl->lv_first = item;
	rl->lv_u.mat.lv_last = item2;
	item->li_prev = रिक्त;
	item2->li_next = रिक्त;
	rl->lv_len = cnt;
    }
}

स्थिर पूर्णाङ्क item_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2);
स्थिर पूर्णाङ्क item_compare2(ध्रुव शून्य *s1, ध्रुव शून्य *s2);

// struct used in the array that's given to qsort()
प्रकारनिर्देश संरचना
{
    listitem_T	*item;
    पूर्णाङ्क		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    सङ्घ {
	varnumber_T	inum;	// for "N"
	द्विगुण		fnum;	// for "n" and "f"
    } किल्ली;
} sortItem_T;

// struct storing information about current sort
प्रकारनिर्देश संरचना
{
    पूर्णाङ्क		item_compare_ic;
    पूर्णाङ्क		item_compare_lc;
    पूर्णाङ्क		item_compare_numeric;
    पूर्णाङ्क		item_compare_numbers;
    पूर्णाङ्क		item_compare_float;
    char_u	*item_compare_func;
    partial_T	*item_compare_partial;
    dict_T	*item_compare_selfdict;
    पूर्णाङ्क		item_compare_func_err;
    पूर्णाङ्क		item_compare_keep_zero;
    पूर्णाङ्क		item_compare_किल्ल्या_ready;  // ptrs[].key is precomputed
} sortinfo_T;
स्थिर sortinfo_T	*sortinfo = रिक्त;
#define ITEM_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    स्थिर पूर्णाङ्क
item_compare(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    sortItem_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = रिक्त, *tofree2 = रिक्त;
    पूर्णाङ्क		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;
    tv1 = &si1->item->li_tv;
    tv2 = &si2->item->li_tv;

    यदि (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_किल्ल्या_ready
					? si1->किल्ली.inum : tv_to_संख्या(tv1);
	varnumber_T	v2 = sortinfo->item_compare_किल्ल्या_ready
					? si2->किल्ली.inum : tv_to_संख्या(tv2);

	प्रत्यागम v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    यदि (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_किल्ल्या_ready
					? si1->किल्ली.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_किल्ल्या_ready
					? si2->किल्ली.fnum : tv_get_float(tv2);

	प्रत्यागम v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    यदि (sortinfo->item_compare_numeric && sortinfo->item_compare_किल्ल्या_ready)
    {
	द्विगुण	n1 = si1->किल्ली.fnum;
	द्विगुण	n2 = si2->किल्ली.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	यदि (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	प्रत्यागम res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    यदि (tv1->v_type == VAR_STRING)
    {
	यदि (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	अन्यथा
	    p1 = tv1->vval.v_string;
    }
    अन्यथा
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    यदि (tv2->v_type == VAR_STRING)
    {
	यदि (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	अन्यथा
	    p2 = tv2->vval.v_string;
    }
    अन्यथा
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    यदि (p1 == रिक्त)
	p1 = (char_u *)"";
    यदि (p2 == रिक्त)
	p2 = (char_u *)"";
    यदि (!sortinfo->item_compare_numeric)
    {
	यदि (sortinfo->item_compare_lc)
	    res = strcoll((वर्ण *)p1, (वर्ण *)p2);
	अन्यथा
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    अन्यथा
    {
	द्विगुण n1, n2;
	n1 = strtod((वर्ण *)p1, (वर्ण **)&p1);
	n2 = strtod((वर्ण *)p2, (वर्ण **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    यदि (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    प्रत्यागम res;
}

    स्थिर पूर्णाङ्क
item_compare2(ध्रुव शून्य *s1, ध्रुव शून्य *s2)
{
    sortItem_T  *si1, *si2;
    पूर्णाङ्क		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->item_compare_partial;
    funcexe_T	funcexe;
    पूर्णाङ्क		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    यदि (sortinfo->item_compare_func_err)
	प्रत्यागम 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    यदि (partial == रिक्त)
	func_name = sortinfo->item_compare_func;
    अन्यथा
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

    यदि (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    अन्यथा
    {
	varnumber_T	n;

	n = tv_get_संख्या_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    यदि (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    यदि (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    प्रत्यागम res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    स्थिर शून्य
sort_compute_किल्ल्या(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    यदि (info->item_compare_numbers)
    {
	प्रति (i = 0; i < len; ++i)
	    ptrs[i].किल्ली.inum = tv_to_संख्या(&ptrs[i].item->li_tv);
    }
    अन्यथा यदि (info->item_compare_float)
    {
	प्रति (i = 0; i < len; ++i)
	    ptrs[i].किल्ली.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    अन्यथा // info->item_compare_numeric
    {
	प्रति (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    यदि (tv->v_type == VAR_STRING)
		ptrs[i].किल्ली.fnum = 0.0;
	    अन्यथा
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = रिक्त;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].किल्ली.fnum = p == रिक्त ? 0.0 : strtod((वर्ण *)p, रिक्त);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_किल्ल्या_ready = TRUE;
}

/*
 * sort() List "l"
 */
    स्थिर शून्य
do_sort(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    यदि (ptrs == रिक्त)
	प्रत्यागम;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_वस्तुसंच(l, li)
    {
	ptrs[i].item = li;
	ptrs[i].idx = i;
	++i;
    }

    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = FALSE;
    info->item_compare_किल्ल्या_ready = FALSE;

    यदि (info->item_compare_func == रिक्त && info->item_compare_partial == रिक्त
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_किल्ल्या(ptrs, len, info);
    // test the compare function
    यदि ((info->item_compare_func != रिक्त
		|| info->item_compare_partial != रिक्त)
	    && item_compare2((शून्य *)&ptrs[0], (शून्य *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    अन्यथा
    {
	// Sort the array with item pointers.
	qsort((शून्य *)ptrs, (size_t)len, परिमाण(sortItem_T),
		info->item_compare_func == रिक्त
		&& info->item_compare_partial == रिक्त
		? item_compare : item_compare2);

	यदि (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = रिक्त;
	    l->lv_len = 0;
	    प्रति (i = 0; i < len; ++i)
		list_append(l, ptrs[i].item);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    स्थिर शून्य
do_uniq(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;
    पूर्णाङ्क	(*item_compare_func_ptr)(ध्रुव शून्य *, ध्रुव शून्य *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    यदि (ptrs == रिक्त)
	प्रत्यागम;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != रिक्त
	|| info->item_compare_partial != रिक्त
	? item_compare2 : item_compare;

    प्रति (li = l->lv_first; li != रिक्त && li->li_next != रिक्त;
	    li = li->li_next)
    {
	यदि (item_compare_func_ptr((शून्य *)&li, (शून्य *)&li->li_next)
		== 0)
	    ptrs[i++].item = li;
	यदि (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    भङ्ग;
	}
    }

    यदि (!info->item_compare_func_err)
    {
	यावत् (--i >= 0)
	{
	    li = ptrs[i].item->li_next;
	    ptrs[i].item->li_next = li->li_next;
	    यदि (li->li_next != रिक्त)
		li->li_next->li_prev = ptrs[i].item;
	    अन्यथा
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
    स्थिर पूर्णाङ्क
parse_sort_uniq_args(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = रिक्त;
    info->item_compare_partial = रिक्त;
    info->item_compare_selfdict = रिक्त;
    info->item_compare_किल्ल्या_ready = FALSE;

    यदि (argvars[1].v_type == VAR_UNKNOWN)
	प्रत्यागम OK;

    // optional second argument: {func}
    यदि (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    अन्यथा यदि (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    अन्यथा
    {
	पूर्णाङ्क	    error = FALSE;
	पूर्णाङ्क	    nr = 0;

	यदि (argvars[1].v_type == VAR_संख्या)
	{
	    nr = tv_get_संख्या_chk(&argvars[1], &error);
	    यदि (error)
		प्रत्यागम FAIL;
	    यदि (nr == 1)
		info->item_compare_ic = TRUE;
	}
	यदि (nr != 1)
	{
	    यदि (argvars[1].v_type != VAR_संख्या)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    अन्यथा यदि (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		प्रत्यागम FAIL;
	    }
	}
	यदि (info->item_compare_func != रिक्त)
	{
	    यदि (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = रिक्त;
	    }
	    अन्यथा यदि (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = रिक्त;
		info->item_compare_numeric = TRUE;
	    }
	    अन्यथा यदि (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = रिक्त;
		info->item_compare_numbers = TRUE;
	    }
	    अन्यथा यदि (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = रिक्त;
		info->item_compare_float = TRUE;
	    }
	    अन्यथा यदि (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = रिक्त;
		info->item_compare_ic = TRUE;
	    }
	    अन्यथा यदि (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = रिक्त;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	यदि (check_for_dict_arg(argvars, 2) == FAIL)
	    प्रत्यागम FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    प्रत्यागम OK;
}

/*
 * "sort()" or "uniq()" function
 */
    स्थिर शून्य
do_sort_uniq(typval_T *argvars, typval_T *rettv, पूर्णाङ्क sort)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    long	len;

    यदि (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	प्रत्यागम;

    यदि (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	प्रत्यागम;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    यदि (l != रिक्त && मूल्य_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	गच्छ theend;
    rettv_list_set(rettv, l);
    यदि (l == रिक्त)
	गच्छ theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    यदि (len <= 1)
	गच्छ theend;	// short list sorts pretty quickly

    यदि (parse_sort_uniq_args(argvars, &info) == FAIL)
	गच्छ theend;

    यदि (sort)
	do_sort(l, &info);
    अन्यथा
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    शून्य
f_sort(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    शून्य
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    पूर्णाङ्क
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*अभिव्यक्ती,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	पूर्णाङ्क		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    पूर्णाङ्क		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    यदि (filtermap == FILTERMAP_FOREACH && अभिव्यक्ती->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(अभिव्यक्ती->vval.v_string);
	यदि (!did_emsg)
	    retval = OK;
	गच्छ theend;
    }

    argv[0] = *get_vim_var_tv(VV_किल्ली);
    argv[1] = *get_vim_var_tv(VV_VAL);
    यदि (eval_अभिव्यक्ती_typval(अभिव्यक्ती, FALSE, argv, 2, fc, newtv) == FAIL)
	गच्छ theend;
    यदि (filtermap == FILTERMAP_FILTER)
    {
	पूर्णाङ्क	    error = FALSE;

	// filter(): when expr is zero remove the item
	यदि (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	अन्यथा
	    *remp = (tv_get_संख्या_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	यदि (error)
	    गच्छ theend;
    }
    अन्यथा यदि (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    प्रत्यागम retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    स्थिर शून्य
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	वर्ण		*func_name,
	char_u		*arg_errmsg,
	typval_T	*अभिव्यक्ती,
	typval_T	*rettv)
{
    पूर्णाङ्क		prev_lock;
    list_T	*l_ret = रिक्त;
    पूर्णाङ्क		idx = 0;
    पूर्णाङ्क		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    यदि (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = रिक्त;
    }
    यदि (l == रिक्त || (filtermap == FILTERMAP_FILTER
			    && मूल्य_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	प्रत्यागम;

    prev_lock = l->lv_lock;

    यदि (filtermap == FILTERMAP_MAPNEW)
    {
	यदि (rettv_list_alloc(rettv) == FAIL)
	    प्रत्यागम;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_किल्ली, VAR_संख्या);

    यदि (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_अभिव्यक्ती_get_funccal(अभिव्यक्ती, &newtv);

    यदि (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	पूर्णाङ्क		len = l->lv_len;
	पूर्णाङ्क		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	यदि (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = रिक्त;
	    l->lv_u.mat.lv_last = रिक्त;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = रिक्त;
	}

	प्रति (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_संख्या;
	    tv.v_lock = 0;
	    tv.vval.v_संख्या = val;
	    set_vim_var_nr(VV_किल्ली, idx);
	    यदि (filter_map_one(&tv, अभिव्यक्ती, filtermap, fc, &newtv, &rem) == FAIL)
		भङ्ग;
	    यदि (did_emsg)
	    {
		clear_tv(&newtv);
		भङ्ग;
	    }
	    यदि (filtermap != FILTERMAP_FOREACH)
	    {
		यदि (filtermap != FILTERMAP_FILTER)
		{
		    यदि (filtermap == FILTERMAP_MAP && argtype != रिक्त
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			भङ्ग;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    यदि (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			भङ्ग;
		}
		अन्यथा यदि (!rem)
		{
		    // filter(): append the list item value when not rem
		    यदि (list_append_tv_move(l, &tv) == FAIL)
			भङ्ग;
		}
	    }

	    val += stride;
	}
    }
    अन्यथा
    {
	// Materialized list: loop over the items
	प्रति (li = l->lv_first; li != रिक्त; li = nli)
	{
	    यदि (filtermap == FILTERMAP_MAP && मूल्य_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		भङ्ग;
	    nli = li->li_next;
	    set_vim_var_nr(VV_किल्ली, idx);
	    यदि (filter_map_one(&li->li_tv, अभिव्यक्ती, filtermap, fc,
							 &newtv, &rem) == FAIL)
		भङ्ग;
	    यदि (did_emsg)
	    {
		clear_tv(&newtv);
		भङ्ग;
	    }
	    यदि (filtermap == FILTERMAP_MAP)
	    {
		यदि (argtype != रिक्त && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    भङ्ग;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_lock = 0;
		li->li_tv = newtv;
	    }
	    अन्यथा यदि (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		यदि (list_append_tv_move(l_ret, &newtv) == FAIL)
		    भङ्ग;
	    }
	    अन्यथा यदि (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = prev_lock;
    यदि (fc != रिक्त)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    स्थिर शून्य
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*अभिव्यक्ती;
    वर्ण	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    पूर्णाङ्क		save_did_emsg;
    type_T	*type = रिक्त;

    // map(), filter(), foreach() return the first argument, also on failure.
    यदि (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    यदि (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	प्रत्यागम;
    }

    यदि (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	यदि (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != रिक्त)
	    type = argvars[0].vval.v_dict->dv_type;
	अन्यथा यदि (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != रिक्त)
	    type = argvars[0].vval.v_list->lv_type;
    }

    यदि (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	वर्ण *संदेश;

	यदि (filtermap == FILTERMAP_FOREACH)
	    संदेश = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	अन्यथा
	    संदेश = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(संदेश), func_name);
	प्रत्यागम;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    अभिव्यक्ती = &argvars[1];
    यदि (अभिव्यक्ती->v_type == VAR_UNKNOWN)
	प्रत्यागम;

    typval_T	save_val;
    typval_T	save_किल्ली;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_किल्ली, &save_किल्ली);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    यदि (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, अभिव्यक्ती, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, अभिव्यक्ती,
							    arg_errmsg, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, अभिव्यक्ती, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, अभिव्यक्ती);
    अन्यथा // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, अभिव्यक्ती, rettv);

    restore_vimvar(VV_किल्ली, &save_किल्ली);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    शून्य
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    शून्य
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    शून्य
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    शून्य
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    स्थिर शून्य
list_add(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    यदि (l == रिक्त)
    {
	यदि (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    अन्यथा यदि (!मूल्य_check_lock(l->lv_lock,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    शून्य
f_add(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_संख्या = 1; // Default: Failed

    यदि (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_संख्या_arg(argvars, 1) == FAIL)))
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    अन्यथा
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    स्थिर long
list_मोजणी(list_T *l, typval_T *needle, long idx, पूर्णाङ्क ic)
{
    long	n = 0;
    listitem_T	*li;

    यदि (l == रिक्त)
	प्रत्यागम 0;

    CHECK_LIST_MATERIALIZE(l);

    यदि (list_len(l) == 0)
	प्रत्यागम 0;

    li = list_find(l, idx);
    यदि (li == रिक्त)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	प्रत्यागम 0;
    }

    प्रति ( ; li != रिक्त; li = li->li_next)
	यदि (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    प्रत्यागम n;
}

/*
 * "count()" function
 */
    शून्य
f_मोजणी(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    पूर्णाङ्क		ic = FALSE;
    पूर्णाङ्क		error = FALSE;

    यदि (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_संख्या_arg(argvars, 3) == FAIL)))
	प्रत्यागम;

    यदि (argvars[2].v_type != VAR_UNKNOWN)
	ic = (पूर्णाङ्क)tv_get_bool_chk(&argvars[2], &error);

    यदि (!error && argvars[0].v_type == VAR_STRING)
	n = string_मोजणी(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    अन्यथा यदि (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	यदि (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_संख्या_chk(&argvars[3], &error);
	यदि (!error)
	    n = list_मोजणी(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    अन्यथा यदि (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	यदि (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_संख्या_chk(&argvars[3], &error);
	यदि (!error)
	    n = tuple_मोजणी(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    अन्यथा यदि (!error && argvars[0].v_type == VAR_DICT)
    {
	यदि (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	अन्यथा
	    n = dict_मोजणी(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    अन्यथा यदि (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_संख्या = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    स्थिर शून्य
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	वर्ण		*func_name,
	char_u		*arg_errmsg,
	पूर्णाङ्क		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*item;
    long	before;
    पूर्णाङ्क		error = FALSE;

    l1 = argvars[0].vval.v_list;
    यदि (l1 == रिक्त)
    {
	emsg(_(e_cannot_extend_null_list));
	प्रत्यागम;
    }
    यदि (is_new || !मूल्य_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	यदि (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    यदि (l1 == रिक्त)
		प्रत्यागम;
	}

	l2 = argvars[1].vval.v_list;
	यदि (l2 == रिक्त)
	    गच्छ theend;

	यदि (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_संख्या_chk(&argvars[2], &error);
	    यदि (error)
		गच्छ cleanup;		// type error; errmsg already given
	    यदि (before == l1->lv_len)
		item = रिक्त;
	    अन्यथा
	    {
		item = list_find(l1, before);
		यदि (item == रिक्त)
		{
		    semsg(_(e_list_index_out_of_range_nr), before);
		    गच्छ cleanup;
		}
	    }
	}
	अन्यथा
	    item = रिक्त;
	यदि (type != रिक्त && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    गच्छ cleanup;
	list_extend(l1, l2, item);

theend:
	यदि (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	अन्यथा
	    copy_tv(&argvars[0], rettv);
	प्रत्यागम;

cleanup:
	यदि (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    स्थिर शून्य
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, पूर्णाङ्क is_new)
{
    type_T	*type = रिक्त;
    वर्ण	*func_name = is_new ? "extendnew()" : "extend()";

    यदि (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	यदि (!is_new && in_vim9script() && argvars[0].vval.v_list != रिक्त)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    अन्यथा यदि (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	यदि (!is_new && in_vim9script() && argvars[0].vval.v_dict != रिक्त)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    अन्यथा यदि (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    अन्यथा
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    शून्य
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
    शून्य
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    स्थिर शून्य
list_घाला_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    पूर्णाङ्क		error = FALSE;

    यदि (l == रिक्त)
    {
	यदि (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	प्रत्यागम;
    }

    यदि (मूल्य_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	प्रत्यागम;

    यदि (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_संख्या_chk(&argvars[2], &error);
    यदि (error)
	प्रत्यागम;		// type error; errmsg already given

    यदि (before == l->lv_len)
	item = रिक्त;
    अन्यथा
    {
	item = list_find(l, before);
	यदि (item == रिक्त)
	{
	    semsg(_(e_list_index_out_of_range_nr), before);
	    l = रिक्त;
	}
    }
    यदि (l != रिक्त)
    {
	(शून्य)list_घाला_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    शून्य
f_घाला(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_संख्या_arg(argvars, 1) == FAIL)
		|| check_for_opt_संख्या_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_BLOB)
	blob_घाला_func(argvars, rettv);
    अन्यथा यदि (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    अन्यथा
	list_घाला_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    शून्य
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    यदि (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_संख्या_arg(argvars, 1) == FAIL
			|| check_for_opt_संख्या_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_संख्या_arg(argvars, 1) == FAIL)))
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    अन्यथा यदि (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    अन्यथा यदि (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    अन्यथा
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    स्थिर शून्य
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    यदि (l != रिक्त
	    && !मूल्य_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	यदि (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    प्रत्यागम;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = रिक्त;
	l->lv_len = 0;
	यावत् (li != रिक्त)
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
    शून्य
f_reverse(typval_T *argvars, typval_T *rettv)
{
    यदि (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	यदि (argvars[0].vval.v_string != रिक्त)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	अन्यथा
	    rettv->vval.v_string = रिक्त;
    }
    अन्यथा यदि (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_TUPLE)
	tuple_reverse(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    स्थिर शून्य
list_reduce(
	typval_T	*argvars,
	typval_T	*अभिव्यक्ती,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = रिक्त;
    पूर्णाङ्क		range_list;
    पूर्णाङ्क		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    पूर्णाङ्क		r;
    पूर्णाङ्क		called_emsg_start = called_emsg;
    पूर्णाङ्क		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != रिक्त && l->lv_first == &range_list_item;
    यदि (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    यदि (argvars[2].v_type == VAR_UNKNOWN)
    {
	यदि (l == रिक्त || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_मूल्य), "List");
	    प्रत्यागम;
	}
	यदि (range_list)
	{
	    initial.v_type = VAR_संख्या;
	    initial.vval.v_संख्या = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	अन्यथा
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_next;
	}
    }
    अन्यथा
    {
	initial = argvars[2];
	यदि (l != रिक्त && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    यदि (l == रिक्त)
	प्रत्यागम;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_अभिव्यक्ती_get_funccal(अभिव्यक्ती, rettv);

    prev_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    यावत् (range_list ? range_idx < l->lv_len : li != रिक्त)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	यदि (range_list)
	{
	    argv[1].v_type = VAR_संख्या;
	    argv[1].vval.v_संख्या = range_val;
	}
	अन्यथा
	    argv[1] = li->li_tv;

	r = eval_अभिव्यक्ती_typval(अभिव्यक्ती, TRUE, argv, 2, fc, rettv);

	यदि (argv[0].v_type != VAR_संख्या && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	यदि (r == FAIL || called_emsg != called_emsg_start)
	    भङ्ग;

	// advance to the next item
	यदि (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	अन्यथा
	    li = li->li_next;
    }

    यदि (fc != रिक्त)
	remove_funccal();

    l->lv_lock = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    शून्य
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    यदि (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    यदि (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    अन्यथा यदि (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    अन्यथा
	func_name = tv_get_string_strict(&argvars[1]);
    यदि (func_name == रिक्त || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	प्रत्यागम;
    }

    यदि (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    अन्यथा यदि (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    अन्यथा
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    शून्य
f_slice(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_संख्या_arg(argvars, 1) == FAIL
		|| check_for_opt_संख्या_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    यदि (check_can_index(&argvars[0], TRUE, FALSE) != OK)
	प्रत्यागम;

    copy_tv(argvars, rettv);
    eval_index_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? रिक्त : argvars + 2,
	    TRUE, रिक्त, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
