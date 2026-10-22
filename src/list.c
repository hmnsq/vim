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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

// List heads for garbage collection.
સ્થિર list_T		*first_list = ખાલી;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    માટે ((lw) = (l)->lv_watch; (lw) != ખાલી; (lw) = (lw)->lw_next)

સ્થિર શૂન્ય list_free_item(list_T *l, listitem_T *item);

/*
 * Add a watcher to a list.
 */
    શૂન્ય
list_add_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_next = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    શૂન્ય
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	જો (lw == lwrem)
	{
	    *lwp = lw->lw_next;
	    તોડો;
	}
	lwp = &lw->lw_next;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    સ્થિર શૂન્ય
list_fix_watch(list_T *l, listitem_T *item)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	જો (lw->lw_item == item)
	    lw->lw_item = item->li_next;
}

    સ્થિર શૂન્ય
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    જો (first_list != ખાલી)
	first_list->lv_used_prev = l;
    l->lv_used_prev = ખાલી;
    l->lv_used_next = first_list;
    first_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(શૂન્ય)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    જો (l != ખાલી)
	list_init(l);
    પાછા l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    જો (alloc_fail_id == id && alloc_does_fail(કદ(list_T)))
	પાછા ખાલી;
#endif
    પાછા (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_વસ્તુઓ(પૂર્ણાંક ગણતરી)
{
    list_T	*l;

    જો (ગણતરી > 0
	    && (size_t)ગણતરી > (SIZE_MAX - કદ(list_T)) / કદ(listitem_T))
	પાછા ખાલી;
    l = (list_T *)alloc_clear(કદ(list_T) + ગણતરી * કદ(listitem_T));
    જો (l == ખાલી)
	પાછા ખાલી;

    list_init(l);

    જો (ગણતરી <= 0)
	પાછા l;

    listitem_T	*li = (listitem_T *)(l + 1);
    પૂર્ણાંક		i;

    l->lv_len = ગણતરી;
    l->lv_with_વસ્તુઓ = ગણતરી;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + ગણતરી - 1;
    માટે (i = 0; i < ગણતરી; ++i)
    {
	જો (i == 0)
	    li->li_prev = ખાલી;
	નહીંતર
	    li->li_prev = li - 1;
	જો (i == ગણતરી - 1)
	    li->li_next = ખાલી;
	નહીંતર
	    li->li_next = li + 1;
	++li;
    }

    પાછા l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    શૂન્ય
list_set_item(list_T *l, પૂર્ણાંક idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    જો (l == ખાલી)
	પાછા FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    પાછા OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    પૂર્ણાંક
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    જો (alloc_fail_id == id && alloc_does_fail(કદ(list_T)))
	પાછા FAIL;
#endif
    પાછા rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    શૂન્ય
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    જો (l != ખાલી)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    શૂન્ય
list_unref(list_T *l)
{
    જો (l != ખાલી && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    સ્થિર શૂન્ય
list_free_contents(list_T *l)
{
    listitem_T *item;

    જો (l->lv_first != &range_list_item)
	માટે (item = l->lv_first; item != ખાલી; item = l->lv_first)
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
    પૂર્ણાંક
list_free_nonref(પૂર્ણાંક copyID)
{
    list_T	*ll;
    પૂર્ણાંક		did_free = FALSE;

    માટે (ll = first_list; ll != ખાલી; ll = ll->lv_used_next)
	જો ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == ખાલી)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    પાછા did_free;
}

    સ્થિર શૂન્ય
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    જો (l->lv_used_prev == ખાલી)
	first_list = l->lv_used_next;
    નહીંતર
	l->lv_used_prev->lv_used_next = l->lv_used_next;
    જો (l->lv_used_next != ખાલી)
	l->lv_used_next->lv_used_prev = l->lv_used_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    શૂન્ય
list_free_વસ્તુઓ(પૂર્ણાંક copyID)
{
    list_T	*ll, *ll_next;

    માટે (ll = first_list; ll != ખાલી; ll = ll_next)
    {
	ll_next = ll->lv_used_next;
	જો ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == ખાલી)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    શૂન્ય
list_free(list_T *l)
{
    જો (in_free_unref_વસ્તુઓ)
	પાછા;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(શૂન્ય)
{
    પાછા ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    સ્થિર શૂન્ય
list_free_item(list_T *l, listitem_T *item)
{
    જો (l->lv_with_વસ્તુઓ == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_વસ્તુઓ)
	vim_free(item);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    શૂન્ય
listitem_free(list_T *l, listitem_T *item)
{
    clear_tv(&item->li_tv);
    list_free_item(l, item);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    શૂન્ય
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
    જો (l == ખાલી)
	પાછા 0L;
    પાછા l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    પૂર્ણાંક
list_equal(
    list_T	*l1,
    list_T	*l2,
    પૂર્ણાંક		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    જો (l1 == l2)
	પાછા TRUE;
    જો (list_len(l1) != list_len(l2))
	પાછા FALSE;
    જો (list_len(l1) == 0)
	// empty and NULL list are considered equal
	પાછા TRUE;
    જો (l1 == ખાલી || l2 == ખાલી)
	પાછા FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    માટે (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != ખાલી && item2 != ખાલી;
			       item1 = item1->li_next, item2 = item2->li_next)
	જો (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    પાછા FALSE;
    પાછા item1 == ખાલી && item2 == ખાલી;
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

    જો (l == ખાલી)
	પાછા ખાલી;

    // Negative index is relative to the end.
    જો (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    જો (n < 0 || n >= l->lv_len)
	પાછા ખાલી;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    જો (n >= l->lv_len)
	પાછા ખાલી;

    // When there is a cached index may start search from there.
    જો (l->lv_u.mat.lv_idx_item != ખાલી)
    {
	જો (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	નહીંતર જો (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	નહીંતર
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    નહીંતર
    {
	જો (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	નહીંતર
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    જ્યારે (n > idx)
    {
	// search forward
	item = item->li_next;
	++idx;
    }
    જ્યારે (n < idx)
    {
	// search backward
	item = item->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    પાછા item;
}

/*
 * Get list item "l[idx]" as a number.
 */
    long
list_find_nr(
    list_T	*l,
    long	idx,
    પૂર્ણાંક		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    જો (l != ખાલી && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	જો (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	જો (n < 0 || n >= l->lv_len)
	{
	    જો (errorp != ખાલી)
		*errorp = TRUE;
	    પાછા -1L;
	}

	પાછા l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    જો (li == ખાલી)
    {
	જો (errorp != ખાલી)
	    *errorp = TRUE;
	પાછા -1L;
    }
    પાછા (long)tv_get_સંખ્યા_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    જો (li == ખાલી)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	પાછા ખાલી;
    }
    પાછા tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_index(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    જો (li != ખાલી)
	પાછા li;

    જો (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    પાછા li;
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

    જો (l == ખાલી)
	પાછા -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    માટે (li = l->lv_first; li != ખાલી && li != item; li = li->li_next)
	++idx;
    જો (li == ખાલી)
	પાછા -1;
    પાછા idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    શૂન્ય
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    જો (l->lv_u.mat.lv_last == ખાલી)
    {
	// empty list
	l->lv_first = item;
	item->li_prev = ખાલી;
    }
    નહીંતર
    {
	l->lv_u.mat.lv_last->li_next = item;
	item->li_prev = l->lv_u.mat.lv_last;
    }
    l->lv_u.mat.lv_last = item;
    ++l->lv_len;
    item->li_next = ખાલી;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    પૂર્ણાંક
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    જો (l->lv_type != ખાલી && l->lv_type->tt_member != ખાલી
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ખાલી, 0) == FAIL)
	પાછા FAIL;
    li = listitem_alloc();
    જો (li == ખાલી)
	પાછા FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    પાછા OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    સ્થિર પૂર્ણાંક
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    જો (li == ખાલી)
	પાછા FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    પાછા OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    પૂર્ણાંક
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    જો (li == ખાલી)
	પાછા FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    પાછા OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    પૂર્ણાંક
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    જો (li == ખાલી)
	પાછા FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    પાછા OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    પૂર્ણાંક
list_append_string(list_T *l, char_u *str, પૂર્ણાંક len)
{
    listitem_T *li = listitem_alloc();

    જો (li == ખાલી)
	પાછા FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    જો (str == ખાલી)
	li->li_tv.vval.v_string = ખાલી;
    નહીંતર જો ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == ખાલી)
	પાછા FAIL;
    પાછા OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    પૂર્ણાંક
list_append_સંખ્યા(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    જો (li == ખાલી)
	પાછા FAIL;
    li->li_tv.v_type = VAR_સંખ્યા;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_સંખ્યા = n;
    list_append(l, li);
    પાછા OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    પૂર્ણાંક
list_ઉમેરો_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    જો (l->lv_type != ખાલી && l->lv_type->tt_member != ખાલી
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ખાલી, 0) == FAIL)
	પાછા FAIL;
    ni = listitem_alloc();
    જો (ni == ખાલી)
	પાછા FAIL;
    copy_tv(tv, &ni->li_tv);
    list_ઉમેરો(l, ni, item);
    પાછા OK;
}

    શૂન્ય
list_ઉમેરો(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    જો (item == ખાલી)
	// Append new item at end of list.
	list_append(l, ni);
    નહીંતર
    {
	// Insert new item before existing item.
	ni->li_prev = item->li_prev;
	ni->li_next = item;
	જો (item->li_prev == ખાલી)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	નહીંતર
	{
	    item->li_prev->li_next = ni;
	    l->lv_u.mat.lv_idx_item = ખાલી;
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
    સ્થિર પૂર્ણાંક
list_append_new_item(list_T *l)
{
    typval_T	tv;

    જો (l->lv_type != ખાલી && l->lv_type->tt_member != ખાલી)
	tv.v_type = l->lv_type->tt_member->tt_type;
    નહીંતર
	tv.v_type = VAR_સંખ્યા;
    tv.v_lock = 0;

    પસંદગી (tv.v_type)
    {
	સ્થિતિ VAR_BOOL: tv.vval.v_સંખ્યા = VVAL_FALSE; તોડો;
	સ્થિતિ VAR_SPECIAL: tv.vval.v_સંખ્યા = 0; તોડો;
	સ્થિતિ VAR_સંખ્યા: tv.vval.v_સંખ્યા = 0; તોડો;
	સ્થિતિ VAR_FLOAT: tv.vval.v_float = 0; તોડો;
	સ્થિતિ VAR_STRING: tv.vval.v_string = ખાલી; તોડો;
	સ્થિતિ VAR_BLOB: tv.vval.v_blob = blob_alloc(); તોડો;
	સ્થિતિ VAR_FUNC: tv.vval.v_string = ખાલી; તોડો;
	સ્થિતિ VAR_PARTIAL: tv.vval.v_partial = ખાલી; તોડો;
	સ્થિતિ VAR_LIST: tv.vval.v_list = list_alloc(); તોડો;
	સ્થિતિ VAR_DICT: tv.vval.v_dict = dict_alloc(); તોડો;
#ifdef FEAT_JOB_CHANNEL
	સ્થિતિ VAR_JOB: tv.vval.v_job = ખાલી; તોડો;
	સ્થિતિ VAR_CHANNEL: tv.vval.v_channel = ખાલી; તોડો;
#endif
	સ્થિતિ VAR_CLASS:
	    જો (l->lv_type != ખાલી && l->lv_type->tt_member != ખાલી)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    નહીંતર
		tv.vval.v_class = ખાલી;
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    જો (l->lv_type != ખાલી && l->lv_type->tt_member != ખાલી)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    નહીંતર
		tv.vval.v_object = ખાલી;
	    તોડો;
	સ્થિતિ VAR_TYPEALIAS: tv.vval.v_typealias = ખાલી; તોડો;
	સ્થિતિ VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); તોડો;
	મૂળભૂત:
	    tv.v_type = VAR_સંખ્યા;
	    tv.vval.v_સંખ્યા = 0;
	    તોડો;
    }

    જો (l->lv_type != ખાલી && l->lv_type->tt_member != ખાલી)
	set_tv_type(&tv, l->lv_type->tt_member);

    પાછા list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_index_one(list_T *l, long *n1, પૂર્ણાંક can_append, પૂર્ણાંક quiet)
{
    long	orig_n1 = *n1;
    listitem_T	*li = list_find_index(l, n1);

    જો (li != ખાલી)
	પાછા li;

    // Vim9: Allow for adding an item at the end.
    જો (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	જો (list_append_new_item(l) == FAIL)
	    પાછા ખાલી;
	li = list_find_index(l, n1);
    }
    જો (li == ખાલી)
    {
	જો (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), orig_n1);
	પાછા ખાલી;
    }
    પાછા li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    પૂર્ણાંક
check_range_index_two(
	list_T	    *l,
	long	    *n1,
	listitem_T  *li1,
	long	    *n2,
	પૂર્ણાંક	    quiet)
{
    જો (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	જો (ni == ખાલી)
	{
	    જો (!quiet)
		semsg(_(e_list_index_out_of_range_nr), *n2);
	    પાછા FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    જો (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    જો (*n2 < *n1)
    {
	જો (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), *n2);
	પાછા FAIL;
    }
    પાછા OK;
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
    પૂર્ણાંક
list_assign_range(
	list_T	    *dest,
	list_T	    *src,
	long	    idx1_arg,
	long	    idx2,
	પૂર્ણાંક	    empty_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    long	idx1 = idx1_arg;
    listitem_T	*first_li = list_find_index(dest, &idx1);
    long	idx;
    type_T	*member_type = ખાલી;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = first_li;
    માટે (src_li = src->lv_first; src_li != ખાલી && dest_li != ખાલી; )
    {
	જો (કિંમત_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    પાછા FAIL;
	src_li = src_li->li_next;
	જો (src_li == ખાલી || (!empty_idx2 && idx2 == idx))
	    તોડો;
	dest_li = dest_li->li_next;
	++idx;
    }

    જો (in_vim9script() && dest->lv_type != ખાલી
					   && dest->lv_type->tt_member != ખાલી)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    માટે (src_li = src->lv_first; src_li != ખાલી; )
    {
	જો (op != ખાલી && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	નહીંતર
	{
	    જો (member_type != ખાલી
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      ખાલી, 0) == FAIL)
		પાછા FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_next;
	જો (src_li == ખાલી || (!empty_idx2 && idx2 == idx))
	    તોડો;
	જો (dest_li->li_next == ખાલી)
	{
	    // Need to add an empty item.
	    જો (list_append_સંખ્યા(dest, 0) == FAIL)
	    {
		src_li = ખાલી;
		તોડો;
	    }
	}
	dest_li = dest_li->li_next;
	++idx;
    }
    જો (src_li != ખાલી)
    {
	emsg(_(e_list_કિંમત_has_more_વસ્તુઓ_than_targets));
	પાછા FAIL;
    }
    જો (empty_idx2
	    ? (dest_li != ખાલી && dest_li->li_next != ખાલી)
	    : idx != idx2)
    {
	emsg(_(e_list_કિંમત_does_not_have_enough_વસ્તુઓ));
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    સ્થિર શૂન્ય
list_flatten(list_T *list, listitem_T *first, long maxitems, long maxdepth)
{
    listitem_T	*item;
    પૂર્ણાંક		done = 0;

    જો (maxdepth == 0)
	પાછા;
    CHECK_LIST_MATERIALIZE(list);
    જો (first == ખાલી)
	item = list->lv_first;
    નહીંતર
	item = first;

    જ્યારે (item != ખાલી && done < maxitems)
    {
	listitem_T	*next = item->li_next;

	fast_breakcheck();
	જો (got_int)
	    પાછા;

	જો (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    જો (list_extend(list, itemlist, next) == FAIL)
	    {
		list_free_item(list, item);
		પાછા;
	    }

	    જો (maxdepth > 0)
		list_flatten(list, item->li_prev == ખાલી
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
    સ્થિર શૂન્ય
flatten_common(typval_T *argvars, typval_T *rettv, પૂર્ણાંક make_copy)
{
    list_T  *l;
    long    maxdepth;
    પૂર્ણાંક	    error = FALSE;

    જો (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    જો (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	પાછા;
    }

    જો (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    નહીંતર
    {
	maxdepth = (long)tv_get_સંખ્યા_chk(&argvars[1], &error);
	જો (error)
	    પાછા;
	જો (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_સંખ્યા));
	    પાછા;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    જો (l == ખાલી)
	પાછા;

    જો (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	જો (l == ખાલી)
	    પાછા;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = ખાલી;
    }
    નહીંતર
    {
	જો (કિંમત_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    પાછા;
	++l->lv_refcount;
    }

    list_flatten(l, ખાલી, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    શૂન્ય
f_flatten(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    નહીંતર
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    શૂન્ય
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    શૂન્ય
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;
    જો (l == ખાલી)
	પાછા;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    માટે (idx = 0, li = l->lv_first; li != ખાલી; li = li->li_next, ++idx)
    {
	list_T	    *l2 = list_alloc();

	જો (l2 == ખાલી)
	    તોડો;
	જો (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    તોડો;
	}
	જો (list_append_સંખ્યા(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    તોડો;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    શૂન્ય
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;
    જો (p == ખાલી)
	પાછા;  // null string behaves like an empty string

    માટે (idx = 0; *p != NUL; ++idx)
    {
	પૂર્ણાંક	    len = mb_ptr2len(p);
	list_T	    *l2;

	જો (len == 0)
	    તોડો;
	l2 = list_alloc();
	જો (l2 == ખાલી)
	    તોડો;
	જો (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    તોડો;
	}
	જો (list_append_સંખ્યા(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    તોડો;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    પૂર્ણાંક
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*item;
    પૂર્ણાંક		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    જો (l2 == ખાલી || l2->lv_len == 0)
	પાછા OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == ખાલી ? ખાલી : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    માટે (item = l2->lv_first; item != ખાલી && --todo >= 0;
				 item = item == bef_prev ? bef : item->li_next)
	જો (list_ઉમેરો_tv(l1, &item->li_tv, bef) == FAIL)
	    પાછા FAIL;
    પાછા OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    પૂર્ણાંક
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    પૂર્ણાંક		len1 = l1 == ખાલી ? 0 : l1->lv_len;
    પૂર્ણાંક		len2 = l2 == ખાલી ? 0 : l2->lv_len;
    long	totallen = (long)len1 + (long)len2;
    પૂર્ણાંક		i;
    listitem_T	*item;

    જો (totallen == 0)
    {
	l = list_alloc();
	જો (l == ખાલી)
	    પાછા FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	પાછા OK;
    }
    જો (totallen > INT_MAX)
	પાછા FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_વસ્તુઓ((પૂર્ણાંક)totallen);
    જો (l == ખાલી)
	પાછા FAIL;

    i = 0;
    જો (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	માટે (item = l1->lv_first; item != ખાલી && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    જો (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	માટે (item = l2->lv_first; item != ખાલી && !got_int;
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
    પાછા OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	ગણતરી;
    long	idx;

    જો (n2 < n1)
	પાછા list_alloc();
    જો (n1 > INT_MAX - (n2 - n1) - 1)
	પાછા ખાલી;
    ગણતરી = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_વસ્તુઓ((પૂર્ણાંક)ગણતરી);
    જો (l == ખાલી)
	પાછા ખાલી;

    item = list_find(ol, n1);
    માટે (idx = 0; idx < ગણતરી; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	જો (item == ખાલી)
	{
	    list_free(l);
	    પાછા ખાલી;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (પૂર્ણાંક)idx, &new_tv);
	item = item->li_next;
    }
    પાછા l;
}

    પૂર્ણાંક
list_slice_or_index(
	    list_T	*list,
	    પૂર્ણાંક		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    પૂર્ણાંક		exclusive,
	    typval_T	*rettv,
	    પૂર્ણાંક		verbose)
{
    long	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    જો (n1 < 0)
	n1 = len + n1;
    જો (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	જો (!range)
	{
	    જો (verbose)
		semsg(_(e_list_index_out_of_range_nr), (long)n1_arg);
	    પાછા FAIL;
	}
	જો (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	નહીંતર
	    n1 = len;
    }
    જો (range)
    {
	list_T	*l;

	જો (n2 < 0)
	    n2 = len + n2;
	નહીંતર જો (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	જો (exclusive)
	    --n2;
	જો (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	જો (l == ખાલી)
	    પાછા FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    નહીંતર
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long index = n1;
	listitem_T *li = check_range_index_one(list, &index, TRUE, TRUE);
	જો (li == ખાલી)
	    પાછા FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	જો (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != ખાલી
					&& var1.vval.v_list->lv_type == ખાલી)
	{
	    જો (list->lv_type != ખાલી && list->lv_type->tt_member != ખાલી)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    પાછા OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, પૂર્ણાંક deep, પૂર્ણાંક ટોચ, પૂર્ણાંક copyID)
{
    list_T	*copy;
    listitem_T	*item;
    પૂર્ણાંક		idx = 0;

    જો (orig == ખાલી)
	પાછા ખાલી;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_વસ્તુઓ(orig->lv_len);
    જો (copy == ખાલી)
	પાછા ખાલી;

    જો (orig->lv_type == ખાલી || ટોચ || deep)
	copy->lv_type = ખાલી;
    નહીંતર
	copy->lv_type = alloc_type(orig->lv_type);
    જો (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    માટે (item = orig->lv_first; item != ખાલી && !got_int;
	    item = item->li_next)
    {
	typval_T	new_tv;

	જો (deep)
	{
	    જો (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		તોડો;
	}
	નહીંતર
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    જો (item != ખાલી)
    {
	list_unref(copy);
	copy = ખાલી;
    }

    પાછા copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    શૂન્ય
vimlist_remove(list_T *l, listitem_T *item, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    માટે (ip = item; ip != ખાલી; ip = ip->li_next)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	જો (ip == item2)
	    તોડો;
    }

    જો (item2->li_next == ખાલી)
	l->lv_u.mat.lv_last = item->li_prev;
    નહીંતર
	item2->li_next->li_prev = item->li_prev;
    જો (item->li_prev == ખાલી)
	l->lv_first = item2->li_next;
    નહીંતર
	item->li_prev->li_next = item2->li_next;
    l->lv_u.mat.lv_idx_item = ખાલી;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, પૂર્ણાંક copyID, પૂર્ણાંક restore_copyID)
{
    garray_T	ga;

    જો (tv->vval.v_list == ખાલી)
	પાછા ખાલી;
    ga_init2(&ga, કદ(અક્ષર), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    જો (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	પાછા ખાલી;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    પાછા (char_u *)ga.ga_data;
}

પ્રકારવ્યાખ્યા સંરચના join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    સ્થિર પૂર્ણાંક
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    પૂર્ણાંક		echo_style,
    પૂર્ણાંક		restore_copyID,
    પૂર્ણાંક		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    પૂર્ણાંક		i;
    join_T	*p;
    long	sumlen = 0;
    પૂર્ણાંક		first = TRUE;
    પૂર્ણાંક		prev_lock;
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
    જો (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    માટે (item = l->lv_first; item != ખાલી && !got_int; item = item->li_next)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	જો (s.string == ખાલી)
	{
	    l->lv_lock = prev_lock;
	    પાછા FAIL;
	}

	s.length = STRLEN(s.string);
	sumlen += (long)s.length;

	(શૂન્ય)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	જો (tofree != ખાલી || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	નહીંતર
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	લીટી_breakcheck();
	જો (did_echo_string_emsg)  // recursion error, bail out
	    તોડો;
    }
    l->lv_lock = prev_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    જો (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    જો (sumlen > INT_MAX - 2 || ga_grow(gap, (પૂર્ણાંક)sumlen + 2) == FAIL)
	પાછા FAIL;

    માટે (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	જો (first)
	    first = FALSE;
	નહીંતર
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	જો (p->s.string != ખાલી)
	    ga_concat_len(gap, p->s.string, p->s.length);
	લીટી_breakcheck();
    }

    પાછા OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    પૂર્ણાંક
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    પૂર્ણાંક		echo_style,
    પૂર્ણાંક		restore_copyID,
    પૂર્ણાંક		copyID)
{
    garray_T	join_ga;
    પૂર્ણાંક		retval;
    join_T	*p;
    પૂર્ણાંક		i;

    જો (l->lv_len < 1)
	પાછા OK; // nothing to do
    ga_init2(&join_ga, કદ(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    જો (join_ga.ga_data == ખાલી)
	પાછા retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    માટે (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    પાછા retval;
}

/*
 * "join()" function
 */
    શૂન્ય
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    જો (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	પાછા;

    જો ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == ખાલી)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == ખાલી))
	પાછા;

    જો (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    નહીંતર
	sep = tv_get_string_chk(&argvars[1]);

    જો (sep != ખાલી)
    {
	ga_init2(&ga, કદ(અક્ષર), 80);
	જો (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	નહીંતર
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
    નહીંતર
	rettv->vval.v_string = ખાલી;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    પૂર્ણાંક
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, પૂર્ણાંક do_error)
{
    પૂર્ણાંક		evaluate = evalarg == ખાલી ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = ખાલી;
    typval_T	tv;
    listitem_T	*item;
    પૂર્ણાંક		vim9script = in_vim9script();
    પૂર્ણાંક		had_comma;

    જો (evaluate)
    {
	l = list_alloc();
	જો (l == ખાલી)
	    પાછા FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    જ્યારે (**arg != ']' && **arg != NUL)
    {
	જો (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    જાઓ failret;
	જો (check_typval_is_કિંમત(&tv) == FAIL)
	{
	    જો (evaluate)
		clear_tv(&tv);
	    જાઓ failret;
	}
	જો (evaluate)
	{
	    item = listitem_alloc();
	    જો (item != ખાલી)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    નહીંતર
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	જો (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	જો (had_comma)
	{
	    જો (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		જાઓ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	જો (**arg == ']')
	    તોડો;

	જો (!had_comma)
	{
	    જો (do_error)
	    {
		જો (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		નહીંતર
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    જાઓ failret;
	}
    }

    જો (**arg != ']')
    {
	જો (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	જો (evaluate)
	    list_free(l);
	પાછા FAIL;
    }

    *arg += 1;
    જો (evaluate)
	rettv_list_set(rettv, l);

    પાછા OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    પૂર્ણાંક
write_list(FILE *fd, list_T *list, પૂર્ણાંક દ્વિઅંકી)
{
    listitem_T	*li;
    પૂર્ણાંક		c;
    પૂર્ણાંક		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_વસ્તુઓ(list, li)
    {
	માટે (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    જો (*s == '\n')
		c = putc(NUL, fd);
	    નહીંતર
		c = putc(*s, fd);
	    જો (c == EOF)
	    {
		ret = FAIL;
		તોડો;
	    }
	}
	જો (!દ્વિઅંકી || li->li_next != ખાલી)
	    જો (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		તોડો;
	    }
	જો (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    તોડો;
	}
    }
    પાછા ret;
}

/*
 * Initialize a static list with 10 items.
 */
    શૂન્ય
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    પૂર્ણાંક	    i;

    CLEAR_POINTER(sl);
    l->lv_first = &sl->sl_વસ્તુઓ[0];
    l->lv_u.mat.lv_last = &sl->sl_વસ્તુઓ[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    માટે (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_વસ્તુઓ[i];

	જો (i == 0)
	    li->li_prev = ખાલી;
	નહીંતર
	    li->li_prev = li - 1;
	જો (i == 9)
	    li->li_next = ખાલી;
	નહીંતર
	    li->li_next = li + 1;
    }
}

/*
 * "list2str()" function
 */
    શૂન્ય
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    પૂર્ણાંક		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	પાછા;

    જો (check_for_list_arg(argvars, 0) == FAIL)
	પાછા;

    l = argvars[0].vval.v_list;
    જો (l == ખાલી)
	પાછા;  // empty list results in empty string

    જો (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (પૂર્ણાંક)tv_get_bool_chk(&argvars[1], ખાલી);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    જો (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	પૂર્ણાંક	(*char2bytes)(પૂર્ણાંક, char_u *);

	જો (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	નહીંતર
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_વસ્તુઓ(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_સંખ્યા(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    નહીંતર
    {
	long len = (long)list_len(l) + 1;

	જો (len <= INT_MAX && ga_grow(&ga, (પૂર્ણાંક)len) == OK)
	{
	    FOR_ALL_LIST_વસ્તુઓ(l, li)
		ga_append(&ga, tv_get_સંખ્યા(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_data;
}

/*
 * "list2tuple()" function
 */
    શૂન્ય
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ખાલી;

    જો (check_for_list_arg(argvars, 0) == FAIL)
	પાછા;

    l = argvars[0].vval.v_list;
    જો (l == ખાલી)
	પાછા;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    જો (rettv_tuple_set_with_વસ્તુઓ(rettv, list_len(l)) == FAIL)
	પાછા;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ITEM(tuple, TUPLE_LEN(tuple)));
	tuple->tv_વસ્તુઓ.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    શૂન્ય
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (check_for_tuple_arg(argvars, 0) == FAIL)
	પાછા;

    tuple = argvars[0].vval.v_tuple;
    જો (tuple == ખાલી)
	પાછા;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    માટે (પૂર્ણાંક i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ITEM(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    સ્થિર શૂન્ય
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*item, *item2;
    listitem_T	*li;
    પૂર્ણાંક		error = FALSE;
    long	idx;
    long	end;
    પૂર્ણાંક		cnt = 0;
    list_T	*rl;

    જો ((l = argvars[0].vval.v_list) == ખાલી
			     || કિંમત_check_lock(l->lv_lock, arg_errmsg, TRUE))
	પાછા;

    idx = (long)tv_get_સંખ્યા_chk(&argvars[1], &error);
    જો (error)
	પાછા;		// type error: do nothing, errmsg already given

    જો ((item = list_find(l, idx)) == ખાલી)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	પાછા;
    }

    જો (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	પાછા;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_સંખ્યા_chk(&argvars[2], &error);
    જો (error)
	પાછા;		// type error: do nothing

    જો ((item2 = list_find(l, end)) == ખાલી)
    {
	semsg(_(e_list_index_out_of_range_nr), end);
	પાછા;
    }

    માટે (li = item; li != ખાલી; li = li->li_next)
    {
	++cnt;
	જો (li == item2)
	    તોડો;
    }
    જો (li == ખાલી)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	પાછા;
    }

    vimlist_remove(l, item, item2);
    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    rl = rettv->vval.v_list;

    જો (l->lv_with_વસ્તુઓ > 0)
    {
	// need to copy the list items and move the value
	જ્યારે (item != ખાલી)
	{
	    li = listitem_alloc();
	    જો (li == ખાલી)
		પાછા;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    જો (item == item2)
		તોડો;
	    item = item->li_next;
	}
    }
    નહીંતર
    {
	rl->lv_first = item;
	rl->lv_u.mat.lv_last = item2;
	item->li_prev = ખાલી;
	item2->li_next = ખાલી;
	rl->lv_len = cnt;
    }
}

સ્થિર પૂર્ણાંક item_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2);
સ્થિર પૂર્ણાંક item_compare2(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2);

// struct used in the array that's given to qsort()
પ્રકારવ્યાખ્યા સંરચના
{
    listitem_T	*item;
    પૂર્ણાંક		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    સંઘ {
	varnumber_T	inum;	// for "N"
	દ્વિગુણ		fnum;	// for "n" and "f"
    } ચાવી;
} sortItem_T;

// struct storing information about current sort
પ્રકારવ્યાખ્યા સંરચના
{
    પૂર્ણાંક		item_compare_ic;
    પૂર્ણાંક		item_compare_lc;
    પૂર્ણાંક		item_compare_numeric;
    પૂર્ણાંક		item_compare_numbers;
    પૂર્ણાંક		item_compare_float;
    char_u	*item_compare_func;
    partial_T	*item_compare_partial;
    dict_T	*item_compare_selfdict;
    પૂર્ણાંક		item_compare_func_err;
    પૂર્ણાંક		item_compare_keep_zero;
    પૂર્ણાંક		item_compare_ચાવીઓ_ready;  // ptrs[].key is precomputed
} sortinfo_T;
સ્થિર sortinfo_T	*sortinfo = ખાલી;
#define ITEM_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    સ્થિર પૂર્ણાંક
item_compare(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    sortItem_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = ખાલી, *tofree2 = ખાલી;
    પૂર્ણાંક		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;
    tv1 = &si1->item->li_tv;
    tv2 = &si2->item->li_tv;

    જો (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_ચાવીઓ_ready
					? si1->ચાવી.inum : tv_to_સંખ્યા(tv1);
	varnumber_T	v2 = sortinfo->item_compare_ચાવીઓ_ready
					? si2->ચાવી.inum : tv_to_સંખ્યા(tv2);

	પાછા v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    જો (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_ચાવીઓ_ready
					? si1->ચાવી.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_ચાવીઓ_ready
					? si2->ચાવી.fnum : tv_get_float(tv2);

	પાછા v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    જો (sortinfo->item_compare_numeric && sortinfo->item_compare_ચાવીઓ_ready)
    {
	દ્વિગુણ	n1 = si1->ચાવી.fnum;
	દ્વિગુણ	n2 = si2->ચાવી.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	જો (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	પાછા res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    જો (tv1->v_type == VAR_STRING)
    {
	જો (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	નહીંતર
	    p1 = tv1->vval.v_string;
    }
    નહીંતર
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    જો (tv2->v_type == VAR_STRING)
    {
	જો (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	નહીંતર
	    p2 = tv2->vval.v_string;
    }
    નહીંતર
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    જો (p1 == ખાલી)
	p1 = (char_u *)"";
    જો (p2 == ખાલી)
	p2 = (char_u *)"";
    જો (!sortinfo->item_compare_numeric)
    {
	જો (sortinfo->item_compare_lc)
	    res = strcoll((અક્ષર *)p1, (અક્ષર *)p2);
	નહીંતર
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    નહીંતર
    {
	દ્વિગુણ n1, n2;
	n1 = strtod((અક્ષર *)p1, (અક્ષર **)&p1);
	n2 = strtod((અક્ષર *)p2, (અક્ષર **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    જો (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    પાછા res;
}

    સ્થિર પૂર્ણાંક
item_compare2(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2)
{
    sortItem_T  *si1, *si2;
    પૂર્ણાંક		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->item_compare_partial;
    funcexe_T	funcexe;
    પૂર્ણાંક		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    જો (sortinfo->item_compare_func_err)
	પાછા 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    જો (partial == ખાલી)
	func_name = sortinfo->item_compare_func;
    નહીંતર
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

    જો (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    નહીંતર
    {
	varnumber_T	n;

	n = tv_get_સંખ્યા_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    જો (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    જો (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    પાછા res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    સ્થિર શૂન્ય
sort_compute_ચાવીઓ(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    જો (info->item_compare_numbers)
    {
	માટે (i = 0; i < len; ++i)
	    ptrs[i].ચાવી.inum = tv_to_સંખ્યા(&ptrs[i].item->li_tv);
    }
    નહીંતર જો (info->item_compare_float)
    {
	માટે (i = 0; i < len; ++i)
	    ptrs[i].ચાવી.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    નહીંતર // info->item_compare_numeric
    {
	માટે (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    જો (tv->v_type == VAR_STRING)
		ptrs[i].ચાવી.fnum = 0.0;
	    નહીંતર
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = ખાલી;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].ચાવી.fnum = p == ખાલી ? 0.0 : strtod((અક્ષર *)p, ખાલી);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_ચાવીઓ_ready = TRUE;
}

/*
 * sort() List "l"
 */
    સ્થિર શૂન્ય
do_sort(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    જો (ptrs == ખાલી)
	પાછા;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_વસ્તુઓ(l, li)
    {
	ptrs[i].item = li;
	ptrs[i].idx = i;
	++i;
    }

    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = FALSE;
    info->item_compare_ચાવીઓ_ready = FALSE;

    જો (info->item_compare_func == ખાલી && info->item_compare_partial == ખાલી
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_ચાવીઓ(ptrs, len, info);
    // test the compare function
    જો ((info->item_compare_func != ખાલી
		|| info->item_compare_partial != ખાલી)
	    && item_compare2((શૂન્ય *)&ptrs[0], (શૂન્ય *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    નહીંતર
    {
	// Sort the array with item pointers.
	qsort((શૂન્ય *)ptrs, (size_t)len, કદ(sortItem_T),
		info->item_compare_func == ખાલી
		&& info->item_compare_partial == ખાલી
		? item_compare : item_compare2);

	જો (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = ખાલી;
	    l->lv_len = 0;
	    માટે (i = 0; i < len; ++i)
		list_append(l, ptrs[i].item);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    સ્થિર શૂન્ય
do_uniq(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;
    પૂર્ણાંક	(*item_compare_func_ptr)(અચલ શૂન્ય *, અચલ શૂન્ય *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    જો (ptrs == ખાલી)
	પાછા;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != ખાલી
	|| info->item_compare_partial != ખાલી
	? item_compare2 : item_compare;

    માટે (li = l->lv_first; li != ખાલી && li->li_next != ખાલી;
	    li = li->li_next)
    {
	જો (item_compare_func_ptr((શૂન્ય *)&li, (શૂન્ય *)&li->li_next)
		== 0)
	    ptrs[i++].item = li;
	જો (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    તોડો;
	}
    }

    જો (!info->item_compare_func_err)
    {
	જ્યારે (--i >= 0)
	{
	    li = ptrs[i].item->li_next;
	    ptrs[i].item->li_next = li->li_next;
	    જો (li->li_next != ખાલી)
		li->li_next->li_prev = ptrs[i].item;
	    નહીંતર
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
    સ્થિર પૂર્ણાંક
parse_sort_uniq_args(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = ખાલી;
    info->item_compare_partial = ખાલી;
    info->item_compare_selfdict = ખાલી;
    info->item_compare_ચાવીઓ_ready = FALSE;

    જો (argvars[1].v_type == VAR_UNKNOWN)
	પાછા OK;

    // optional second argument: {func}
    જો (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    નહીંતર જો (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    નહીંતર
    {
	પૂર્ણાંક	    error = FALSE;
	પૂર્ણાંક	    nr = 0;

	જો (argvars[1].v_type == VAR_સંખ્યા)
	{
	    nr = tv_get_સંખ્યા_chk(&argvars[1], &error);
	    જો (error)
		પાછા FAIL;
	    જો (nr == 1)
		info->item_compare_ic = TRUE;
	}
	જો (nr != 1)
	{
	    જો (argvars[1].v_type != VAR_સંખ્યા)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    નહીંતર જો (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		પાછા FAIL;
	    }
	}
	જો (info->item_compare_func != ખાલી)
	{
	    જો (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = ખાલી;
	    }
	    નહીંતર જો (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = ખાલી;
		info->item_compare_numeric = TRUE;
	    }
	    નહીંતર જો (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = ખાલી;
		info->item_compare_numbers = TRUE;
	    }
	    નહીંતર જો (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = ખાલી;
		info->item_compare_float = TRUE;
	    }
	    નહીંતર જો (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = ખાલી;
		info->item_compare_ic = TRUE;
	    }
	    નહીંતર જો (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = ખાલી;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	જો (check_for_dict_arg(argvars, 2) == FAIL)
	    પાછા FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    પાછા OK;
}

/*
 * "sort()" or "uniq()" function
 */
    સ્થિર શૂન્ય
do_sort_uniq(typval_T *argvars, typval_T *rettv, પૂર્ણાંક sort)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    long	len;

    જો (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	પાછા;

    જો (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	પાછા;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    જો (l != ખાલી && કિંમત_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	જાઓ theend;
    rettv_list_set(rettv, l);
    જો (l == ખાલી)
	જાઓ theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    જો (len <= 1)
	જાઓ theend;	// short list sorts pretty quickly

    જો (parse_sort_uniq_args(argvars, &info) == FAIL)
	જાઓ theend;

    જો (sort)
	do_sort(l, &info);
    નહીંતર
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    શૂન્ય
f_sort(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    શૂન્ય
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    પૂર્ણાંક
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*પદાવલિ,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	પૂર્ણાંક		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    પૂર્ણાંક		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    જો (filtermap == FILTERMAP_FOREACH && પદાવલિ->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(પદાવલિ->vval.v_string);
	જો (!did_emsg)
	    retval = OK;
	જાઓ theend;
    }

    argv[0] = *get_vim_var_tv(VV_ચાવી);
    argv[1] = *get_vim_var_tv(VV_VAL);
    જો (eval_પદાવલિ_typval(પદાવલિ, FALSE, argv, 2, fc, newtv) == FAIL)
	જાઓ theend;
    જો (filtermap == FILTERMAP_FILTER)
    {
	પૂર્ણાંક	    error = FALSE;

	// filter(): when expr is zero remove the item
	જો (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	નહીંતર
	    *remp = (tv_get_સંખ્યા_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	જો (error)
	    જાઓ theend;
    }
    નહીંતર જો (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    પાછા retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    સ્થિર શૂન્ય
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	અક્ષર		*func_name,
	char_u		*arg_errmsg,
	typval_T	*પદાવલિ,
	typval_T	*rettv)
{
    પૂર્ણાંક		prev_lock;
    list_T	*l_ret = ખાલી;
    પૂર્ણાંક		idx = 0;
    પૂર્ણાંક		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    જો (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = ખાલી;
    }
    જો (l == ખાલી || (filtermap == FILTERMAP_FILTER
			    && કિંમત_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	પાછા;

    prev_lock = l->lv_lock;

    જો (filtermap == FILTERMAP_MAPNEW)
    {
	જો (rettv_list_alloc(rettv) == FAIL)
	    પાછા;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ચાવી, VAR_સંખ્યા);

    જો (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_પદાવલિ_get_funccal(પદાવલિ, &newtv);

    જો (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	પૂર્ણાંક		len = l->lv_len;
	પૂર્ણાંક		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	જો (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = ખાલી;
	    l->lv_u.mat.lv_last = ખાલી;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = ખાલી;
	}

	માટે (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_સંખ્યા;
	    tv.v_lock = 0;
	    tv.vval.v_સંખ્યા = val;
	    set_vim_var_nr(VV_ચાવી, idx);
	    જો (filter_map_one(&tv, પદાવલિ, filtermap, fc, &newtv, &rem) == FAIL)
		તોડો;
	    જો (did_emsg)
	    {
		clear_tv(&newtv);
		તોડો;
	    }
	    જો (filtermap != FILTERMAP_FOREACH)
	    {
		જો (filtermap != FILTERMAP_FILTER)
		{
		    જો (filtermap == FILTERMAP_MAP && argtype != ખાલી
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			તોડો;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    જો (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			તોડો;
		}
		નહીંતર જો (!rem)
		{
		    // filter(): append the list item value when not rem
		    જો (list_append_tv_move(l, &tv) == FAIL)
			તોડો;
		}
	    }

	    val += stride;
	}
    }
    નહીંતર
    {
	// Materialized list: loop over the items
	માટે (li = l->lv_first; li != ખાલી; li = nli)
	{
	    જો (filtermap == FILTERMAP_MAP && કિંમત_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		તોડો;
	    nli = li->li_next;
	    set_vim_var_nr(VV_ચાવી, idx);
	    જો (filter_map_one(&li->li_tv, પદાવલિ, filtermap, fc,
							 &newtv, &rem) == FAIL)
		તોડો;
	    જો (did_emsg)
	    {
		clear_tv(&newtv);
		તોડો;
	    }
	    જો (filtermap == FILTERMAP_MAP)
	    {
		જો (argtype != ખાલી && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    તોડો;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_lock = 0;
		li->li_tv = newtv;
	    }
	    નહીંતર જો (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		જો (list_append_tv_move(l_ret, &newtv) == FAIL)
		    તોડો;
	    }
	    નહીંતર જો (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = prev_lock;
    જો (fc != ખાલી)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    સ્થિર શૂન્ય
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*પદાવલિ;
    અક્ષર	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    પૂર્ણાંક		save_did_emsg;
    type_T	*type = ખાલી;

    // map(), filter(), foreach() return the first argument, also on failure.
    જો (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    જો (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	પાછા;

    જો (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	પાછા;
    }

    જો (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	જો (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != ખાલી)
	    type = argvars[0].vval.v_dict->dv_type;
	નહીંતર જો (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != ખાલી)
	    type = argvars[0].vval.v_list->lv_type;
    }

    જો (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	અક્ષર *સંદેશ;

	જો (filtermap == FILTERMAP_FOREACH)
	    સંદેશ = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	નહીંતર
	    સંદેશ = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(સંદેશ), func_name);
	પાછા;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    પદાવલિ = &argvars[1];
    જો (પદાવલિ->v_type == VAR_UNKNOWN)
	પાછા;

    typval_T	save_val;
    typval_T	save_ચાવી;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_ચાવી, &save_ચાવી);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    જો (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, પદાવલિ, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, પદાવલિ,
							    arg_errmsg, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, પદાવલિ, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, પદાવલિ);
    નહીંતર // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, પદાવલિ, rettv);

    restore_vimvar(VV_ચાવી, &save_ચાવી);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    શૂન્ય
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    શૂન્ય
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    શૂન્ય
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    શૂન્ય
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    સ્થિર શૂન્ય
list_add(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    જો (l == ખાલી)
    {
	જો (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    નહીંતર જો (!કિંમત_check_lock(l->lv_lock,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    શૂન્ય
f_add(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = 1; // Default: Failed

    જો (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_સંખ્યા_arg(argvars, 1) == FAIL)))
	પાછા;

    જો (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    નહીંતર
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    સ્થિર long
list_ગણતરી(list_T *l, typval_T *needle, long idx, પૂર્ણાંક ic)
{
    long	n = 0;
    listitem_T	*li;

    જો (l == ખાલી)
	પાછા 0;

    CHECK_LIST_MATERIALIZE(l);

    જો (list_len(l) == 0)
	પાછા 0;

    li = list_find(l, idx);
    જો (li == ખાલી)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	પાછા 0;
    }

    માટે ( ; li != ખાલી; li = li->li_next)
	જો (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    પાછા n;
}

/*
 * "count()" function
 */
    શૂન્ય
f_ગણતરી(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    પૂર્ણાંક		ic = FALSE;
    પૂર્ણાંક		error = FALSE;

    જો (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 3) == FAIL)))
	પાછા;

    જો (argvars[2].v_type != VAR_UNKNOWN)
	ic = (પૂર્ણાંક)tv_get_bool_chk(&argvars[2], &error);

    જો (!error && argvars[0].v_type == VAR_STRING)
	n = string_ગણતરી(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    નહીંતર જો (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	જો (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_સંખ્યા_chk(&argvars[3], &error);
	જો (!error)
	    n = list_ગણતરી(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    નહીંતર જો (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	જો (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_સંખ્યા_chk(&argvars[3], &error);
	જો (!error)
	    n = tuple_ગણતરી(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    નહીંતર જો (!error && argvars[0].v_type == VAR_DICT)
    {
	જો (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	નહીંતર
	    n = dict_ગણતરી(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    નહીંતર જો (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_સંખ્યા = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    સ્થિર શૂન્ય
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	અક્ષર		*func_name,
	char_u		*arg_errmsg,
	પૂર્ણાંક		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*item;
    long	before;
    પૂર્ણાંક		error = FALSE;

    l1 = argvars[0].vval.v_list;
    જો (l1 == ખાલી)
    {
	emsg(_(e_cannot_extend_null_list));
	પાછા;
    }
    જો (is_new || !કિંમત_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	જો (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    જો (l1 == ખાલી)
		પાછા;
	}

	l2 = argvars[1].vval.v_list;
	જો (l2 == ખાલી)
	    જાઓ theend;

	જો (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_સંખ્યા_chk(&argvars[2], &error);
	    જો (error)
		જાઓ cleanup;		// type error; errmsg already given
	    જો (before == l1->lv_len)
		item = ખાલી;
	    નહીંતર
	    {
		item = list_find(l1, before);
		જો (item == ખાલી)
		{
		    semsg(_(e_list_index_out_of_range_nr), before);
		    જાઓ cleanup;
		}
	    }
	}
	નહીંતર
	    item = ખાલી;
	જો (type != ખાલી && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    જાઓ cleanup;
	list_extend(l1, l2, item);

theend:
	જો (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	નહીંતર
	    copy_tv(&argvars[0], rettv);
	પાછા;

cleanup:
	જો (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    સ્થિર શૂન્ય
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, પૂર્ણાંક is_new)
{
    type_T	*type = ખાલી;
    અક્ષર	*func_name = is_new ? "extendnew()" : "extend()";

    જો (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	જો (!is_new && in_vim9script() && argvars[0].vval.v_list != ખાલી)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    નહીંતર જો (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	જો (!is_new && in_vim9script() && argvars[0].vval.v_dict != ખાલી)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    નહીંતર જો (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    નહીંતર
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    શૂન્ય
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
    શૂન્ય
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    સ્થિર શૂન્ય
list_ઉમેરો_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    પૂર્ણાંક		error = FALSE;

    જો (l == ખાલી)
    {
	જો (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	પાછા;
    }

    જો (કિંમત_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	પાછા;

    જો (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_સંખ્યા_chk(&argvars[2], &error);
    જો (error)
	પાછા;		// type error; errmsg already given

    જો (before == l->lv_len)
	item = ખાલી;
    નહીંતર
    {
	item = list_find(l, before);
	જો (item == ખાલી)
	{
	    semsg(_(e_list_index_out_of_range_nr), before);
	    l = ખાલી;
	}
    }
    જો (l != ખાલી)
    {
	(શૂન્ય)list_ઉમેરો_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    શૂન્ય
f_ઉમેરો(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_સંખ્યા_arg(argvars, 1) == FAIL)
		|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL))
	પાછા;

    જો (argvars[0].v_type == VAR_BLOB)
	blob_ઉમેરો_func(argvars, rettv);
    નહીંતર જો (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    નહીંતર
	list_ઉમેરો_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    શૂન્ય
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    જો (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_સંખ્યા_arg(argvars, 1) == FAIL
			|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_સંખ્યા_arg(argvars, 1) == FAIL)))
	પાછા;

    જો (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    નહીંતર જો (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    નહીંતર જો (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    નહીંતર
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    સ્થિર શૂન્ય
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    જો (l != ખાલી
	    && !કિંમત_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	જો (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    પાછા;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = ખાલી;
	l->lv_len = 0;
	જ્યારે (li != ખાલી)
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
    શૂન્ય
f_reverse(typval_T *argvars, typval_T *rettv)
{
    જો (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	જો (argvars[0].vval.v_string != ખાલી)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	નહીંતર
	    rettv->vval.v_string = ખાલી;
    }
    નહીંતર જો (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    નહીંતર જો (argvars[0].v_type == VAR_TUPLE)
	tuple_reverse(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    સ્થિર શૂન્ય
list_reduce(
	typval_T	*argvars,
	typval_T	*પદાવલિ,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = ખાલી;
    પૂર્ણાંક		range_list;
    પૂર્ણાંક		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    પૂર્ણાંક		r;
    પૂર્ણાંક		called_emsg_start = called_emsg;
    પૂર્ણાંક		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != ખાલી && l->lv_first == &range_list_item;
    જો (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    જો (argvars[2].v_type == VAR_UNKNOWN)
    {
	જો (l == ખાલી || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_કિંમત), "List");
	    પાછા;
	}
	જો (range_list)
	{
	    initial.v_type = VAR_સંખ્યા;
	    initial.vval.v_સંખ્યા = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	નહીંતર
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_next;
	}
    }
    નહીંતર
    {
	initial = argvars[2];
	જો (l != ખાલી && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    જો (l == ખાલી)
	પાછા;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_પદાવલિ_get_funccal(પદાવલિ, rettv);

    prev_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    જ્યારે (range_list ? range_idx < l->lv_len : li != ખાલી)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	જો (range_list)
	{
	    argv[1].v_type = VAR_સંખ્યા;
	    argv[1].vval.v_સંખ્યા = range_val;
	}
	નહીંતર
	    argv[1] = li->li_tv;

	r = eval_પદાવલિ_typval(પદાવલિ, TRUE, argv, 2, fc, rettv);

	જો (argv[0].v_type != VAR_સંખ્યા && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	જો (r == FAIL || called_emsg != called_emsg_start)
	    તોડો;

	// advance to the next item
	જો (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	નહીંતર
	    li = li->li_next;
    }

    જો (fc != ખાલી)
	remove_funccal();

    l->lv_lock = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    શૂન્ય
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    જો (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    નહીંતર જો (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    નહીંતર
	func_name = tv_get_string_strict(&argvars[1]);
    જો (func_name == ખાલી || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	પાછા;
    }

    જો (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    નહીંતર જો (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    નહીંતર જો (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    નહીંતર
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    શૂન્ય
f_slice(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_opt_સંખ્યા_arg(argvars, 2) == FAIL))
	પાછા;

    જો (check_can_index(&argvars[0], TRUE, FALSE) != OK)
	પાછા;

    copy_tv(argvars, rettv);
    eval_index_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? ખાલી : argvars + 2,
	    TRUE, ખાલી, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
