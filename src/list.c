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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

// List heads for garbage collection.
സ്ഥിതം list_T		*first_list = ശൂന്യ;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    വേണ്ടി ((lw) = (l)->lv_watch; (lw) != ശൂന്യ; (lw) = (lw)->lw_next)

സ്ഥിതം ശൂന്യം list_free_item(list_T *l, listitem_T *item);

/*
 * Add a watcher to a list.
 */
    ശൂന്യം
list_add_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_next = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    ശൂന്യം
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	എങ്കിൽ (lw == lwrem)
	{
	    *lwp = lw->lw_next;
	    നിർത്തുക;
	}
	lwp = &lw->lw_next;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    സ്ഥിതം ശൂന്യം
list_fix_watch(list_T *l, listitem_T *item)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	എങ്കിൽ (lw->lw_item == item)
	    lw->lw_item = item->li_next;
}

    സ്ഥിതം ശൂന്യം
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    എങ്കിൽ (first_list != ശൂന്യ)
	first_list->lv_used_prev = l;
    l->lv_used_prev = ശൂന്യ;
    l->lv_used_next = first_list;
    first_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(ശൂന്യം)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    എങ്കിൽ (l != ശൂന്യ)
	list_init(l);
    തിരികെ l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    എങ്കിൽ (alloc_fail_id == id && alloc_does_fail(വലുപ്പം(list_T)))
	തിരികെ ശൂന്യ;
#endif
    തിരികെ (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_ഇനങ്ങൾ(പൂർണ്ണസംഖ്യ എണ്ണം)
{
    list_T	*l;

    എങ്കിൽ (എണ്ണം > 0
	    && (size_t)എണ്ണം > (SIZE_MAX - വലുപ്പം(list_T)) / വലുപ്പം(listitem_T))
	തിരികെ ശൂന്യ;
    l = (list_T *)alloc_clear(വലുപ്പം(list_T) + എണ്ണം * വലുപ്പം(listitem_T));
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ ശൂന്യ;

    list_init(l);

    എങ്കിൽ (എണ്ണം <= 0)
	തിരികെ l;

    listitem_T	*li = (listitem_T *)(l + 1);
    പൂർണ്ണസംഖ്യ		i;

    l->lv_len = എണ്ണം;
    l->lv_with_ഇനങ്ങൾ = എണ്ണം;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + എണ്ണം - 1;
    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
    {
	എങ്കിൽ (i == 0)
	    li->li_prev = ശൂന്യ;
	അല്ലെങ്കിൽ
	    li->li_prev = li - 1;
	എങ്കിൽ (i == എണ്ണം - 1)
	    li->li_next = ശൂന്യ;
	അല്ലെങ്കിൽ
	    li->li_next = li + 1;
	++li;
    }

    തിരികെ l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    ശൂന്യം
list_set_item(list_T *l, പൂർണ്ണസംഖ്യ idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    തിരികെ OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    പൂർണ്ണസംഖ്യ
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    എങ്കിൽ (alloc_fail_id == id && alloc_does_fail(വലുപ്പം(list_T)))
	തിരികെ FAIL;
#endif
    തിരികെ rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    ശൂന്യം
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    എങ്കിൽ (l != ശൂന്യ)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    ശൂന്യം
list_unref(list_T *l)
{
    എങ്കിൽ (l != ശൂന്യ && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    സ്ഥിതം ശൂന്യം
list_free_contents(list_T *l)
{
    listitem_T *item;

    എങ്കിൽ (l->lv_first != &range_list_item)
	വേണ്ടി (item = l->lv_first; item != ശൂന്യ; item = l->lv_first)
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
    പൂർണ്ണസംഖ്യ
list_free_nonref(പൂർണ്ണസംഖ്യ copyID)
{
    list_T	*ll;
    പൂർണ്ണസംഖ്യ		did_free = FALSE;

    വേണ്ടി (ll = first_list; ll != ശൂന്യ; ll = ll->lv_used_next)
	എങ്കിൽ ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == ശൂന്യ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    തിരികെ did_free;
}

    സ്ഥിതം ശൂന്യം
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    എങ്കിൽ (l->lv_used_prev == ശൂന്യ)
	first_list = l->lv_used_next;
    അല്ലെങ്കിൽ
	l->lv_used_prev->lv_used_next = l->lv_used_next;
    എങ്കിൽ (l->lv_used_next != ശൂന്യ)
	l->lv_used_next->lv_used_prev = l->lv_used_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    ശൂന്യം
list_free_ഇനങ്ങൾ(പൂർണ്ണസംഖ്യ copyID)
{
    list_T	*ll, *ll_next;

    വേണ്ടി (ll = first_list; ll != ശൂന്യ; ll = ll_next)
    {
	ll_next = ll->lv_used_next;
	എങ്കിൽ ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == ശൂന്യ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    ശൂന്യം
list_free(list_T *l)
{
    എങ്കിൽ (in_free_unref_ഇനങ്ങൾ)
	തിരികെ;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(ശൂന്യം)
{
    തിരികെ ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    സ്ഥിതം ശൂന്യം
list_free_item(list_T *l, listitem_T *item)
{
    എങ്കിൽ (l->lv_with_ഇനങ്ങൾ == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_ഇനങ്ങൾ)
	vim_free(item);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    ശൂന്യം
listitem_free(list_T *l, listitem_T *item)
{
    clear_tv(&item->li_tv);
    list_free_item(l, item);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    ശൂന്യം
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
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ 0L;
    തിരികെ l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    പൂർണ്ണസംഖ്യ
list_equal(
    list_T	*l1,
    list_T	*l2,
    പൂർണ്ണസംഖ്യ		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    എങ്കിൽ (l1 == l2)
	തിരികെ TRUE;
    എങ്കിൽ (list_len(l1) != list_len(l2))
	തിരികെ FALSE;
    എങ്കിൽ (list_len(l1) == 0)
	// empty and NULL list are considered equal
	തിരികെ TRUE;
    എങ്കിൽ (l1 == ശൂന്യ || l2 == ശൂന്യ)
	തിരികെ FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    വേണ്ടി (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != ശൂന്യ && item2 != ശൂന്യ;
			       item1 = item1->li_next, item2 = item2->li_next)
	എങ്കിൽ (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    തിരികെ FALSE;
    തിരികെ item1 == ശൂന്യ && item2 == ശൂന്യ;
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

    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Negative index is relative to the end.
    എങ്കിൽ (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    എങ്കിൽ (n < 0 || n >= l->lv_len)
	തിരികെ ശൂന്യ;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    എങ്കിൽ (n >= l->lv_len)
	തിരികെ ശൂന്യ;

    // When there is a cached index may start search from there.
    എങ്കിൽ (l->lv_u.mat.lv_idx_item != ശൂന്യ)
    {
	എങ്കിൽ (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	അല്ലെങ്കിൽ
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	അല്ലെങ്കിൽ
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    ആകുമ്പോൾ (n > idx)
    {
	// search forward
	item = item->li_next;
	++idx;
    }
    ആകുമ്പോൾ (n < idx)
    {
	// search backward
	item = item->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    തിരികെ item;
}

/*
 * Get list item "l[idx]" as a number.
 */
    long
list_find_nr(
    list_T	*l,
    long	idx,
    പൂർണ്ണസംഖ്യ		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    എങ്കിൽ (l != ശൂന്യ && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	എങ്കിൽ (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	എങ്കിൽ (n < 0 || n >= l->lv_len)
	{
	    എങ്കിൽ (errorp != ശൂന്യ)
		*errorp = TRUE;
	    തിരികെ -1L;
	}

	തിരികെ l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    എങ്കിൽ (li == ശൂന്യ)
    {
	എങ്കിൽ (errorp != ശൂന്യ)
	    *errorp = TRUE;
	തിരികെ -1L;
    }
    തിരികെ (long)tv_get_സംഖ്യ_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    എങ്കിൽ (li == ശൂന്യ)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	തിരികെ ശൂന്യ;
    }
    തിരികെ tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_index(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    എങ്കിൽ (li != ശൂന്യ)
	തിരികെ li;

    എങ്കിൽ (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    തിരികെ li;
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

    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    വേണ്ടി (li = l->lv_first; li != ശൂന്യ && li != item; li = li->li_next)
	++idx;
    എങ്കിൽ (li == ശൂന്യ)
	തിരികെ -1;
    തിരികെ idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    ശൂന്യം
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    എങ്കിൽ (l->lv_u.mat.lv_last == ശൂന്യ)
    {
	// empty list
	l->lv_first = item;
	item->li_prev = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	l->lv_u.mat.lv_last->li_next = item;
	item->li_prev = l->lv_u.mat.lv_last;
    }
    l->lv_u.mat.lv_last = item;
    ++l->lv_len;
    item->li_next = ശൂന്യ;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    പൂർണ്ണസംഖ്യ
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    എങ്കിൽ (l->lv_type != ശൂന്യ && l->lv_type->tt_member != ശൂന്യ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ശൂന്യ, 0) == FAIL)
	തിരികെ FAIL;
    li = listitem_alloc();
    എങ്കിൽ (li == ശൂന്യ)
	തിരികെ FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    തിരികെ OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    എങ്കിൽ (li == ശൂന്യ)
	തിരികെ FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    തിരികെ OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    എങ്കിൽ (li == ശൂന്യ)
	തിരികെ FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    തിരികെ OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    എങ്കിൽ (li == ശൂന്യ)
	തിരികെ FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    തിരികെ OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
list_append_string(list_T *l, char_u *str, പൂർണ്ണസംഖ്യ len)
{
    listitem_T *li = listitem_alloc();

    എങ്കിൽ (li == ശൂന്യ)
	തിരികെ FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    എങ്കിൽ (str == ശൂന്യ)
	li->li_tv.vval.v_string = ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
list_append_സംഖ്യ(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    എങ്കിൽ (li == ശൂന്യ)
	തിരികെ FAIL;
    li->li_tv.v_type = VAR_സംഖ്യ;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_സംഖ്യ = n;
    list_append(l, li);
    തിരികെ OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    പൂർണ്ണസംഖ്യ
list_തിരുകുക_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    എങ്കിൽ (l->lv_type != ശൂന്യ && l->lv_type->tt_member != ശൂന്യ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ശൂന്യ, 0) == FAIL)
	തിരികെ FAIL;
    ni = listitem_alloc();
    എങ്കിൽ (ni == ശൂന്യ)
	തിരികെ FAIL;
    copy_tv(tv, &ni->li_tv);
    list_തിരുകുക(l, ni, item);
    തിരികെ OK;
}

    ശൂന്യം
list_തിരുകുക(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    എങ്കിൽ (item == ശൂന്യ)
	// Append new item at end of list.
	list_append(l, ni);
    അല്ലെങ്കിൽ
    {
	// Insert new item before existing item.
	ni->li_prev = item->li_prev;
	ni->li_next = item;
	എങ്കിൽ (item->li_prev == ശൂന്യ)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	അല്ലെങ്കിൽ
	{
	    item->li_prev->li_next = ni;
	    l->lv_u.mat.lv_idx_item = ശൂന്യ;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
list_append_new_item(list_T *l)
{
    typval_T	tv;

    എങ്കിൽ (l->lv_type != ശൂന്യ && l->lv_type->tt_member != ശൂന്യ)
	tv.v_type = l->lv_type->tt_member->tt_type;
    അല്ലെങ്കിൽ
	tv.v_type = VAR_സംഖ്യ;
    tv.v_lock = 0;

    തിരഞ്ഞെടുപ്പ് (tv.v_type)
    {
	അവസ്ഥ VAR_BOOL: tv.vval.v_സംഖ്യ = VVAL_FALSE; നിർത്തുക;
	അവസ്ഥ VAR_SPECIAL: tv.vval.v_സംഖ്യ = 0; നിർത്തുക;
	അവസ്ഥ VAR_സംഖ്യ: tv.vval.v_സംഖ്യ = 0; നിർത്തുക;
	അവസ്ഥ VAR_FLOAT: tv.vval.v_float = 0; നിർത്തുക;
	അവസ്ഥ VAR_STRING: tv.vval.v_string = ശൂന്യ; നിർത്തുക;
	അവസ്ഥ VAR_BLOB: tv.vval.v_blob = blob_alloc(); നിർത്തുക;
	അവസ്ഥ VAR_FUNC: tv.vval.v_string = ശൂന്യ; നിർത്തുക;
	അവസ്ഥ VAR_PARTIAL: tv.vval.v_partial = ശൂന്യ; നിർത്തുക;
	അവസ്ഥ VAR_LIST: tv.vval.v_list = list_alloc(); നിർത്തുക;
	അവസ്ഥ VAR_DICT: tv.vval.v_dict = dict_alloc(); നിർത്തുക;
#ifdef FEAT_JOB_CHANNEL
	അവസ്ഥ VAR_JOB: tv.vval.v_job = ശൂന്യ; നിർത്തുക;
	അവസ്ഥ VAR_CHANNEL: tv.vval.v_channel = ശൂന്യ; നിർത്തുക;
#endif
	അവസ്ഥ VAR_CLASS:
	    എങ്കിൽ (l->lv_type != ശൂന്യ && l->lv_type->tt_member != ശൂന്യ)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    അല്ലെങ്കിൽ
		tv.vval.v_class = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    എങ്കിൽ (l->lv_type != ശൂന്യ && l->lv_type->tt_member != ശൂന്യ)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    അല്ലെങ്കിൽ
		tv.vval.v_object = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_TYPEALIAS: tv.vval.v_typealias = ശൂന്യ; നിർത്തുക;
	അവസ്ഥ VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    tv.v_type = VAR_സംഖ്യ;
	    tv.vval.v_സംഖ്യ = 0;
	    നിർത്തുക;
    }

    എങ്കിൽ (l->lv_type != ശൂന്യ && l->lv_type->tt_member != ശൂന്യ)
	set_tv_type(&tv, l->lv_type->tt_member);

    തിരികെ list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_index_one(list_T *l, long *n1, പൂർണ്ണസംഖ്യ can_append, പൂർണ്ണസംഖ്യ quiet)
{
    long	orig_n1 = *n1;
    listitem_T	*li = list_find_index(l, n1);

    എങ്കിൽ (li != ശൂന്യ)
	തിരികെ li;

    // Vim9: Allow for adding an item at the end.
    എങ്കിൽ (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	എങ്കിൽ (list_append_new_item(l) == FAIL)
	    തിരികെ ശൂന്യ;
	li = list_find_index(l, n1);
    }
    എങ്കിൽ (li == ശൂന്യ)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), orig_n1);
	തിരികെ ശൂന്യ;
    }
    തിരികെ li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
check_range_index_two(
	list_T	    *l,
	long	    *n1,
	listitem_T  *li1,
	long	    *n2,
	പൂർണ്ണസംഖ്യ	    quiet)
{
    എങ്കിൽ (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	എങ്കിൽ (ni == ശൂന്യ)
	{
	    എങ്കിൽ (!quiet)
		semsg(_(e_list_index_out_of_range_nr), *n2);
	    തിരികെ FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    എങ്കിൽ (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    എങ്കിൽ (*n2 < *n1)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), *n2);
	തിരികെ FAIL;
    }
    തിരികെ OK;
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
    പൂർണ്ണസംഖ്യ
list_assign_range(
	list_T	    *dest,
	list_T	    *src,
	long	    idx1_arg,
	long	    idx2,
	പൂർണ്ണസംഖ്യ	    empty_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    long	idx1 = idx1_arg;
    listitem_T	*first_li = list_find_index(dest, &idx1);
    long	idx;
    type_T	*member_type = ശൂന്യ;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = first_li;
    വേണ്ടി (src_li = src->lv_first; src_li != ശൂന്യ && dest_li != ശൂന്യ; )
    {
	എങ്കിൽ (മൂല്യം_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    തിരികെ FAIL;
	src_li = src_li->li_next;
	എങ്കിൽ (src_li == ശൂന്യ || (!empty_idx2 && idx2 == idx))
	    നിർത്തുക;
	dest_li = dest_li->li_next;
	++idx;
    }

    എങ്കിൽ (in_vim9script() && dest->lv_type != ശൂന്യ
					   && dest->lv_type->tt_member != ശൂന്യ)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    വേണ്ടി (src_li = src->lv_first; src_li != ശൂന്യ; )
    {
	എങ്കിൽ (op != ശൂന്യ && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (member_type != ശൂന്യ
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      ശൂന്യ, 0) == FAIL)
		തിരികെ FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_next;
	എങ്കിൽ (src_li == ശൂന്യ || (!empty_idx2 && idx2 == idx))
	    നിർത്തുക;
	എങ്കിൽ (dest_li->li_next == ശൂന്യ)
	{
	    // Need to add an empty item.
	    എങ്കിൽ (list_append_സംഖ്യ(dest, 0) == FAIL)
	    {
		src_li = ശൂന്യ;
		നിർത്തുക;
	    }
	}
	dest_li = dest_li->li_next;
	++idx;
    }
    എങ്കിൽ (src_li != ശൂന്യ)
    {
	emsg(_(e_list_മൂല്യം_has_more_ഇനങ്ങൾ_than_targets));
	തിരികെ FAIL;
    }
    എങ്കിൽ (empty_idx2
	    ? (dest_li != ശൂന്യ && dest_li->li_next != ശൂന്യ)
	    : idx != idx2)
    {
	emsg(_(e_list_മൂല്യം_does_not_have_enough_ഇനങ്ങൾ));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    സ്ഥിതം ശൂന്യം
list_flatten(list_T *list, listitem_T *first, long maxitems, long maxdepth)
{
    listitem_T	*item;
    പൂർണ്ണസംഖ്യ		done = 0;

    എങ്കിൽ (maxdepth == 0)
	തിരികെ;
    CHECK_LIST_MATERIALIZE(list);
    എങ്കിൽ (first == ശൂന്യ)
	item = list->lv_first;
    അല്ലെങ്കിൽ
	item = first;

    ആകുമ്പോൾ (item != ശൂന്യ && done < maxitems)
    {
	listitem_T	*next = item->li_next;

	fast_breakcheck();
	എങ്കിൽ (got_int)
	    തിരികെ;

	എങ്കിൽ (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    എങ്കിൽ (list_extend(list, itemlist, next) == FAIL)
	    {
		list_free_item(list, item);
		തിരികെ;
	    }

	    എങ്കിൽ (maxdepth > 0)
		list_flatten(list, item->li_prev == ശൂന്യ
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
    സ്ഥിതം ശൂന്യം
flatten_common(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ make_copy)
{
    list_T  *l;
    long    maxdepth;
    പൂർണ്ണസംഖ്യ	    error = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	തിരികെ;
    }

    എങ്കിൽ (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    അല്ലെങ്കിൽ
    {
	maxdepth = (long)tv_get_സംഖ്യ_chk(&argvars[1], &error);
	എങ്കിൽ (error)
	    തിരികെ;
	എങ്കിൽ (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_സംഖ്യ));
	    തിരികെ;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	എങ്കിൽ (l == ശൂന്യ)
	    തിരികെ;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (മൂല്യം_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    തിരികെ;
	++l->lv_refcount;
    }

    list_flatten(l, ശൂന്യ, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    ശൂന്യം
f_flatten(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    അല്ലെങ്കിൽ
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    ശൂന്യം
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    ശൂന്യം
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    വേണ്ടി (idx = 0, li = l->lv_first; li != ശൂന്യ; li = li->li_next, ++idx)
    {
	list_T	    *l2 = list_alloc();

	എങ്കിൽ (l2 == ശൂന്യ)
	    നിർത്തുക;
	എങ്കിൽ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    നിർത്തുക;
	}
	എങ്കിൽ (list_append_സംഖ്യ(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    നിർത്തുക;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    ശൂന്യം
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;  // null string behaves like an empty string

    വേണ്ടി (idx = 0; *p != NUL; ++idx)
    {
	പൂർണ്ണസംഖ്യ	    len = mb_ptr2len(p);
	list_T	    *l2;

	എങ്കിൽ (len == 0)
	    നിർത്തുക;
	l2 = list_alloc();
	എങ്കിൽ (l2 == ശൂന്യ)
	    നിർത്തുക;
	എങ്കിൽ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    നിർത്തുക;
	}
	എങ്കിൽ (list_append_സംഖ്യ(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    നിർത്തുക;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*item;
    പൂർണ്ണസംഖ്യ		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    എങ്കിൽ (l2 == ശൂന്യ || l2->lv_len == 0)
	തിരികെ OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == ശൂന്യ ? ശൂന്യ : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    വേണ്ടി (item = l2->lv_first; item != ശൂന്യ && --todo >= 0;
				 item = item == bef_prev ? bef : item->li_next)
	എങ്കിൽ (list_തിരുകുക_tv(l1, &item->li_tv, bef) == FAIL)
	    തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    പൂർണ്ണസംഖ്യ		len1 = l1 == ശൂന്യ ? 0 : l1->lv_len;
    പൂർണ്ണസംഖ്യ		len2 = l2 == ശൂന്യ ? 0 : l2->lv_len;
    long	totallen = (long)len1 + (long)len2;
    പൂർണ്ണസംഖ്യ		i;
    listitem_T	*item;

    എങ്കിൽ (totallen == 0)
    {
	l = list_alloc();
	എങ്കിൽ (l == ശൂന്യ)
	    തിരികെ FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	തിരികെ OK;
    }
    എങ്കിൽ (totallen > INT_MAX)
	തിരികെ FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_ഇനങ്ങൾ((പൂർണ്ണസംഖ്യ)totallen);
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ FAIL;

    i = 0;
    എങ്കിൽ (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	വേണ്ടി (item = l1->lv_first; item != ശൂന്യ && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    എങ്കിൽ (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	വേണ്ടി (item = l2->lv_first; item != ശൂന്യ && !got_int;
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
    തിരികെ OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	എണ്ണം;
    long	idx;

    എങ്കിൽ (n2 < n1)
	തിരികെ list_alloc();
    എങ്കിൽ (n1 > INT_MAX - (n2 - n1) - 1)
	തിരികെ ശൂന്യ;
    എണ്ണം = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_ഇനങ്ങൾ((പൂർണ്ണസംഖ്യ)എണ്ണം);
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ ശൂന്യ;

    item = list_find(ol, n1);
    വേണ്ടി (idx = 0; idx < എണ്ണം; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	എങ്കിൽ (item == ശൂന്യ)
	{
	    list_free(l);
	    തിരികെ ശൂന്യ;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (പൂർണ്ണസംഖ്യ)idx, &new_tv);
	item = item->li_next;
    }
    തിരികെ l;
}

    പൂർണ്ണസംഖ്യ
list_slice_or_index(
	    list_T	*list,
	    പൂർണ്ണസംഖ്യ		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    പൂർണ്ണസംഖ്യ		exclusive,
	    typval_T	*rettv,
	    പൂർണ്ണസംഖ്യ		verbose)
{
    long	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    എങ്കിൽ (n1 < 0)
	n1 = len + n1;
    എങ്കിൽ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	എങ്കിൽ (!range)
	{
	    എങ്കിൽ (verbose)
		semsg(_(e_list_index_out_of_range_nr), (long)n1_arg);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	അല്ലെങ്കിൽ
	    n1 = len;
    }
    എങ്കിൽ (range)
    {
	list_T	*l;

	എങ്കിൽ (n2 < 0)
	    n2 = len + n2;
	അല്ലെങ്കിൽ എങ്കിൽ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	എങ്കിൽ (exclusive)
	    --n2;
	എങ്കിൽ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	എങ്കിൽ (l == ശൂന്യ)
	    തിരികെ FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    അല്ലെങ്കിൽ
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long index = n1;
	listitem_T *li = check_range_index_one(list, &index, TRUE, TRUE);
	എങ്കിൽ (li == ശൂന്യ)
	    തിരികെ FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	എങ്കിൽ (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != ശൂന്യ
					&& var1.vval.v_list->lv_type == ശൂന്യ)
	{
	    എങ്കിൽ (list->lv_type != ശൂന്യ && list->lv_type->tt_member != ശൂന്യ)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    തിരികെ OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, പൂർണ്ണസംഖ്യ deep, പൂർണ്ണസംഖ്യ മുകൾ, പൂർണ്ണസംഖ്യ copyID)
{
    list_T	*copy;
    listitem_T	*item;
    പൂർണ്ണസംഖ്യ		idx = 0;

    എങ്കിൽ (orig == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_ഇനങ്ങൾ(orig->lv_len);
    എങ്കിൽ (copy == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (orig->lv_type == ശൂന്യ || മുകൾ || deep)
	copy->lv_type = ശൂന്യ;
    അല്ലെങ്കിൽ
	copy->lv_type = alloc_type(orig->lv_type);
    എങ്കിൽ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    വേണ്ടി (item = orig->lv_first; item != ശൂന്യ && !got_int;
	    item = item->li_next)
    {
	typval_T	new_tv;

	എങ്കിൽ (deep)
	{
	    എങ്കിൽ (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    എങ്കിൽ (item != ശൂന്യ)
    {
	list_unref(copy);
	copy = ശൂന്യ;
    }

    തിരികെ copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    ശൂന്യം
vimlist_remove(list_T *l, listitem_T *item, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    വേണ്ടി (ip = item; ip != ശൂന്യ; ip = ip->li_next)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	എങ്കിൽ (ip == item2)
	    നിർത്തുക;
    }

    എങ്കിൽ (item2->li_next == ശൂന്യ)
	l->lv_u.mat.lv_last = item->li_prev;
    അല്ലെങ്കിൽ
	item2->li_next->li_prev = item->li_prev;
    എങ്കിൽ (item->li_prev == ശൂന്യ)
	l->lv_first = item2->li_next;
    അല്ലെങ്കിൽ
	item->li_prev->li_next = item2->li_next;
    l->lv_u.mat.lv_idx_item = ശൂന്യ;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, പൂർണ്ണസംഖ്യ copyID, പൂർണ്ണസംഖ്യ restore_copyID)
{
    garray_T	ga;

    എങ്കിൽ (tv->vval.v_list == ശൂന്യ)
	തിരികെ ശൂന്യ;
    ga_init2(&ga, വലുപ്പം(അക്ഷരം), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    എങ്കിൽ (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	തിരികെ ശൂന്യ;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    തിരികെ (char_u *)ga.ga_data;
}

തരനിർവചനം ഘടന join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    പൂർണ്ണസംഖ്യ		echo_style,
    പൂർണ്ണസംഖ്യ		restore_copyID,
    പൂർണ്ണസംഖ്യ		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    പൂർണ്ണസംഖ്യ		i;
    join_T	*p;
    long	sumlen = 0;
    പൂർണ്ണസംഖ്യ		first = TRUE;
    പൂർണ്ണസംഖ്യ		prev_lock;
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
    എങ്കിൽ (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    വേണ്ടി (item = l->lv_first; item != ശൂന്യ && !got_int; item = item->li_next)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	എങ്കിൽ (s.string == ശൂന്യ)
	{
	    l->lv_lock = prev_lock;
	    തിരികെ FAIL;
	}

	s.length = STRLEN(s.string);
	sumlen += (long)s.length;

	(ശൂന്യം)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	എങ്കിൽ (tofree != ശൂന്യ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	അല്ലെങ്കിൽ
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	വരി_breakcheck();
	എങ്കിൽ (did_echo_string_emsg)  // recursion error, bail out
	    നിർത്തുക;
    }
    l->lv_lock = prev_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    എങ്കിൽ (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    എങ്കിൽ (sumlen > INT_MAX - 2 || ga_grow(gap, (പൂർണ്ണസംഖ്യ)sumlen + 2) == FAIL)
	തിരികെ FAIL;

    വേണ്ടി (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	എങ്കിൽ (first)
	    first = FALSE;
	അല്ലെങ്കിൽ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	എങ്കിൽ (p->s.string != ശൂന്യ)
	    ga_concat_len(gap, p->s.string, p->s.length);
	വരി_breakcheck();
    }

    തിരികെ OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    പൂർണ്ണസംഖ്യ
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    പൂർണ്ണസംഖ്യ		echo_style,
    പൂർണ്ണസംഖ്യ		restore_copyID,
    പൂർണ്ണസംഖ്യ		copyID)
{
    garray_T	join_ga;
    പൂർണ്ണസംഖ്യ		retval;
    join_T	*p;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (l->lv_len < 1)
	തിരികെ OK; // nothing to do
    ga_init2(&join_ga, വലുപ്പം(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    എങ്കിൽ (join_ga.ga_data == ശൂന്യ)
	തിരികെ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    വേണ്ടി (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    തിരികെ retval;
}

/*
 * "join()" function
 */
    ശൂന്യം
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    എങ്കിൽ (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	തിരികെ;

    എങ്കിൽ ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == ശൂന്യ)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == ശൂന്യ))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    അല്ലെങ്കിൽ
	sep = tv_get_string_chk(&argvars[1]);

    എങ്കിൽ (sep != ശൂന്യ)
    {
	ga_init2(&ga, വലുപ്പം(അക്ഷരം), 80);
	എങ്കിൽ (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	അല്ലെങ്കിൽ
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
    അല്ലെങ്കിൽ
	rettv->vval.v_string = ശൂന്യ;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, പൂർണ്ണസംഖ്യ do_error)
{
    പൂർണ്ണസംഖ്യ		evaluate = evalarg == ശൂന്യ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = ശൂന്യ;
    typval_T	tv;
    listitem_T	*item;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    പൂർണ്ണസംഖ്യ		had_comma;

    എങ്കിൽ (evaluate)
    {
	l = list_alloc();
	എങ്കിൽ (l == ശൂന്യ)
	    തിരികെ FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ആകുമ്പോൾ (**arg != ']' && **arg != NUL)
    {
	എങ്കിൽ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    പോകുക failret;
	എങ്കിൽ (check_typval_is_മൂല്യം(&tv) == FAIL)
	{
	    എങ്കിൽ (evaluate)
		clear_tv(&tv);
	    പോകുക failret;
	}
	എങ്കിൽ (evaluate)
	{
	    item = listitem_alloc();
	    എങ്കിൽ (item != ശൂന്യ)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    അല്ലെങ്കിൽ
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	എങ്കിൽ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	എങ്കിൽ (had_comma)
	{
	    എങ്കിൽ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		പോകുക failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	എങ്കിൽ (**arg == ']')
	    നിർത്തുക;

	എങ്കിൽ (!had_comma)
	{
	    എങ്കിൽ (do_error)
	    {
		എങ്കിൽ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		അല്ലെങ്കിൽ
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    പോകുക failret;
	}
    }

    എങ്കിൽ (**arg != ']')
    {
	എങ്കിൽ (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	എങ്കിൽ (evaluate)
	    list_free(l);
	തിരികെ FAIL;
    }

    *arg += 1;
    എങ്കിൽ (evaluate)
	rettv_list_set(rettv, l);

    തിരികെ OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    പൂർണ്ണസംഖ്യ
write_list(FILE *fd, list_T *list, പൂർണ്ണസംഖ്യ ദ്വിമാന)
{
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ഇനങ്ങൾ(list, li)
    {
	വേണ്ടി (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    എങ്കിൽ (*s == '\n')
		c = putc(NUL, fd);
	    അല്ലെങ്കിൽ
		c = putc(*s, fd);
	    എങ്കിൽ (c == EOF)
	    {
		ret = FAIL;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (!ദ്വിമാന || li->li_next != ശൂന്യ)
	    എങ്കിൽ (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		നിർത്തുക;
	    }
	എങ്കിൽ (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    നിർത്തുക;
	}
    }
    തിരികെ ret;
}

/*
 * Initialize a static list with 10 items.
 */
    ശൂന്യം
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    പൂർണ്ണസംഖ്യ	    i;

    CLEAR_POINTER(sl);
    l->lv_first = &sl->sl_ഇനങ്ങൾ[0];
    l->lv_u.mat.lv_last = &sl->sl_ഇനങ്ങൾ[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    വേണ്ടി (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_ഇനങ്ങൾ[i];

	എങ്കിൽ (i == 0)
	    li->li_prev = ശൂന്യ;
	അല്ലെങ്കിൽ
	    li->li_prev = li - 1;
	എങ്കിൽ (i == 9)
	    li->li_next = ശൂന്യ;
	അല്ലെങ്കിൽ
	    li->li_next = li + 1;
    }
}

/*
 * "list2str()" function
 */
    ശൂന്യം
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL)
	തിരികെ;

    l = argvars[0].vval.v_list;
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;  // empty list results in empty string

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[1], ശൂന്യ);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    എങ്കിൽ (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	പൂർണ്ണസംഖ്യ	(*char2bytes)(പൂർണ്ണസംഖ്യ, char_u *);

	എങ്കിൽ (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	അല്ലെങ്കിൽ
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_സംഖ്യ(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    അല്ലെങ്കിൽ
    {
	long len = (long)list_len(l) + 1;

	എങ്കിൽ (len <= INT_MAX && ga_grow(&ga, (പൂർണ്ണസംഖ്യ)len) == OK)
	{
	    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
		ga_append(&ga, tv_get_സംഖ്യ(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_data;
}

/*
 * "list2tuple()" function
 */
    ശൂന്യം
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ശൂന്യ;

    എങ്കിൽ (check_for_list_arg(argvars, 0) == FAIL)
	തിരികെ;

    l = argvars[0].vval.v_list;
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    എങ്കിൽ (rettv_tuple_set_with_ഇനങ്ങൾ(rettv, list_len(l)) == FAIL)
	തിരികെ;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ITEM(tuple, TUPLE_LEN(tuple)));
	tuple->tv_ഇനങ്ങൾ.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    ശൂന്യം
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (check_for_tuple_arg(argvars, 0) == FAIL)
	തിരികെ;

    tuple = argvars[0].vval.v_tuple;
    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ITEM(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    സ്ഥിതം ശൂന്യം
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*item, *item2;
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    long	idx;
    long	end;
    പൂർണ്ണസംഖ്യ		cnt = 0;
    list_T	*rl;

    എങ്കിൽ ((l = argvars[0].vval.v_list) == ശൂന്യ
			     || മൂല്യം_check_lock(l->lv_lock, arg_errmsg, TRUE))
	തിരികെ;

    idx = (long)tv_get_സംഖ്യ_chk(&argvars[1], &error);
    എങ്കിൽ (error)
	തിരികെ;		// type error: do nothing, errmsg already given

    എങ്കിൽ ((item = list_find(l, idx)) == ശൂന്യ)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	തിരികെ;
    }

    എങ്കിൽ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	തിരികെ;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_സംഖ്യ_chk(&argvars[2], &error);
    എങ്കിൽ (error)
	തിരികെ;		// type error: do nothing

    എങ്കിൽ ((item2 = list_find(l, end)) == ശൂന്യ)
    {
	semsg(_(e_list_index_out_of_range_nr), end);
	തിരികെ;
    }

    വേണ്ടി (li = item; li != ശൂന്യ; li = li->li_next)
    {
	++cnt;
	എങ്കിൽ (li == item2)
	    നിർത്തുക;
    }
    എങ്കിൽ (li == ശൂന്യ)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	തിരികെ;
    }

    vimlist_remove(l, item, item2);
    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    rl = rettv->vval.v_list;

    എങ്കിൽ (l->lv_with_ഇനങ്ങൾ > 0)
    {
	// need to copy the list items and move the value
	ആകുമ്പോൾ (item != ശൂന്യ)
	{
	    li = listitem_alloc();
	    എങ്കിൽ (li == ശൂന്യ)
		തിരികെ;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    എങ്കിൽ (item == item2)
		നിർത്തുക;
	    item = item->li_next;
	}
    }
    അല്ലെങ്കിൽ
    {
	rl->lv_first = item;
	rl->lv_u.mat.lv_last = item2;
	item->li_prev = ശൂന്യ;
	item2->li_next = ശൂന്യ;
	rl->lv_len = cnt;
    }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ item_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2);
സ്ഥിതം പൂർണ്ണസംഖ്യ item_compare2(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2);

// struct used in the array that's given to qsort()
തരനിർവചനം ഘടന
{
    listitem_T	*item;
    പൂർണ്ണസംഖ്യ		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    സംഘം {
	varnumber_T	inum;	// for "N"
	ഇരട്ട		fnum;	// for "n" and "f"
    } താക്കോൽ;
} sortItem_T;

// struct storing information about current sort
തരനിർവചനം ഘടന
{
    പൂർണ്ണസംഖ്യ		item_compare_ic;
    പൂർണ്ണസംഖ്യ		item_compare_lc;
    പൂർണ്ണസംഖ്യ		item_compare_numeric;
    പൂർണ്ണസംഖ്യ		item_compare_numbers;
    പൂർണ്ണസംഖ്യ		item_compare_float;
    char_u	*item_compare_func;
    partial_T	*item_compare_partial;
    dict_T	*item_compare_selfdict;
    പൂർണ്ണസംഖ്യ		item_compare_func_err;
    പൂർണ്ണസംഖ്യ		item_compare_keep_zero;
    പൂർണ്ണസംഖ്യ		item_compare_താക്കോലുകൾ_ready;  // ptrs[].key is precomputed
} sortinfo_T;
സ്ഥിതം sortinfo_T	*sortinfo = ശൂന്യ;
#define ITEM_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
item_compare(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    sortItem_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = ശൂന്യ, *tofree2 = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;
    tv1 = &si1->item->li_tv;
    tv2 = &si2->item->li_tv;

    എങ്കിൽ (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_താക്കോലുകൾ_ready
					? si1->താക്കോൽ.inum : tv_to_സംഖ്യ(tv1);
	varnumber_T	v2 = sortinfo->item_compare_താക്കോലുകൾ_ready
					? si2->താക്കോൽ.inum : tv_to_സംഖ്യ(tv2);

	തിരികെ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    എങ്കിൽ (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_താക്കോലുകൾ_ready
					? si1->താക്കോൽ.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_താക്കോലുകൾ_ready
					? si2->താക്കോൽ.fnum : tv_get_float(tv2);

	തിരികെ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    എങ്കിൽ (sortinfo->item_compare_numeric && sortinfo->item_compare_താക്കോലുകൾ_ready)
    {
	ഇരട്ട	n1 = si1->താക്കോൽ.fnum;
	ഇരട്ട	n2 = si2->താക്കോൽ.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	എങ്കിൽ (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	തിരികെ res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    എങ്കിൽ (tv1->v_type == VAR_STRING)
    {
	എങ്കിൽ (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	അല്ലെങ്കിൽ
	    p1 = tv1->vval.v_string;
    }
    അല്ലെങ്കിൽ
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    എങ്കിൽ (tv2->v_type == VAR_STRING)
    {
	എങ്കിൽ (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	അല്ലെങ്കിൽ
	    p2 = tv2->vval.v_string;
    }
    അല്ലെങ്കിൽ
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    എങ്കിൽ (p1 == ശൂന്യ)
	p1 = (char_u *)"";
    എങ്കിൽ (p2 == ശൂന്യ)
	p2 = (char_u *)"";
    എങ്കിൽ (!sortinfo->item_compare_numeric)
    {
	എങ്കിൽ (sortinfo->item_compare_lc)
	    res = strcoll((അക്ഷരം *)p1, (അക്ഷരം *)p2);
	അല്ലെങ്കിൽ
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    അല്ലെങ്കിൽ
    {
	ഇരട്ട n1, n2;
	n1 = strtod((അക്ഷരം *)p1, (അക്ഷരം **)&p1);
	n2 = strtod((അക്ഷരം *)p2, (അക്ഷരം **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    എങ്കിൽ (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    തിരികെ res;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
item_compare2(സ്ഥിരം ശൂന്യം *s1, സ്ഥിരം ശൂന്യം *s2)
{
    sortItem_T  *si1, *si2;
    പൂർണ്ണസംഖ്യ		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->item_compare_partial;
    funcexe_T	funcexe;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    എങ്കിൽ (sortinfo->item_compare_func_err)
	തിരികെ 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    എങ്കിൽ (partial == ശൂന്യ)
	func_name = sortinfo->item_compare_func;
    അല്ലെങ്കിൽ
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

    എങ്കിൽ (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    അല്ലെങ്കിൽ
    {
	varnumber_T	n;

	n = tv_get_സംഖ്യ_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    എങ്കിൽ (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    എങ്കിൽ (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    തിരികെ res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    സ്ഥിതം ശൂന്യം
sort_compute_താക്കോലുകൾ(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    എങ്കിൽ (info->item_compare_numbers)
    {
	വേണ്ടി (i = 0; i < len; ++i)
	    ptrs[i].താക്കോൽ.inum = tv_to_സംഖ്യ(&ptrs[i].item->li_tv);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (info->item_compare_float)
    {
	വേണ്ടി (i = 0; i < len; ++i)
	    ptrs[i].താക്കോൽ.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    അല്ലെങ്കിൽ // info->item_compare_numeric
    {
	വേണ്ടി (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    എങ്കിൽ (tv->v_type == VAR_STRING)
		ptrs[i].താക്കോൽ.fnum = 0.0;
	    അല്ലെങ്കിൽ
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = ശൂന്യ;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].താക്കോൽ.fnum = p == ശൂന്യ ? 0.0 : strtod((അക്ഷരം *)p, ശൂന്യ);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_താക്കോലുകൾ_ready = TRUE;
}

/*
 * sort() List "l"
 */
    സ്ഥിതം ശൂന്യം
do_sort(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    എങ്കിൽ (ptrs == ശൂന്യ)
	തിരികെ;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_ഇനങ്ങൾ(l, li)
    {
	ptrs[i].item = li;
	ptrs[i].idx = i;
	++i;
    }

    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = FALSE;
    info->item_compare_താക്കോലുകൾ_ready = FALSE;

    എങ്കിൽ (info->item_compare_func == ശൂന്യ && info->item_compare_partial == ശൂന്യ
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_താക്കോലുകൾ(ptrs, len, info);
    // test the compare function
    എങ്കിൽ ((info->item_compare_func != ശൂന്യ
		|| info->item_compare_partial != ശൂന്യ)
	    && item_compare2((ശൂന്യം *)&ptrs[0], (ശൂന്യം *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    അല്ലെങ്കിൽ
    {
	// Sort the array with item pointers.
	qsort((ശൂന്യം *)ptrs, (size_t)len, വലുപ്പം(sortItem_T),
		info->item_compare_func == ശൂന്യ
		&& info->item_compare_partial == ശൂന്യ
		? item_compare : item_compare2);

	എങ്കിൽ (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = ശൂന്യ;
	    l->lv_len = 0;
	    വേണ്ടി (i = 0; i < len; ++i)
		list_append(l, ptrs[i].item);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    സ്ഥിതം ശൂന്യം
do_uniq(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ	(*item_compare_func_ptr)(സ്ഥിരം ശൂന്യം *, സ്ഥിരം ശൂന്യം *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    എങ്കിൽ (ptrs == ശൂന്യ)
	തിരികെ;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != ശൂന്യ
	|| info->item_compare_partial != ശൂന്യ
	? item_compare2 : item_compare;

    വേണ്ടി (li = l->lv_first; li != ശൂന്യ && li->li_next != ശൂന്യ;
	    li = li->li_next)
    {
	എങ്കിൽ (item_compare_func_ptr((ശൂന്യം *)&li, (ശൂന്യം *)&li->li_next)
		== 0)
	    ptrs[i++].item = li;
	എങ്കിൽ (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    നിർത്തുക;
	}
    }

    എങ്കിൽ (!info->item_compare_func_err)
    {
	ആകുമ്പോൾ (--i >= 0)
	{
	    li = ptrs[i].item->li_next;
	    ptrs[i].item->li_next = li->li_next;
	    എങ്കിൽ (li->li_next != ശൂന്യ)
		li->li_next->li_prev = ptrs[i].item;
	    അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
parse_sort_uniq_args(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = ശൂന്യ;
    info->item_compare_partial = ശൂന്യ;
    info->item_compare_selfdict = ശൂന്യ;
    info->item_compare_താക്കോലുകൾ_ready = FALSE;

    എങ്കിൽ (argvars[1].v_type == VAR_UNKNOWN)
	തിരികെ OK;

    // optional second argument: {func}
    എങ്കിൽ (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	    error = FALSE;
	പൂർണ്ണസംഖ്യ	    nr = 0;

	എങ്കിൽ (argvars[1].v_type == VAR_സംഖ്യ)
	{
	    nr = tv_get_സംഖ്യ_chk(&argvars[1], &error);
	    എങ്കിൽ (error)
		തിരികെ FAIL;
	    എങ്കിൽ (nr == 1)
		info->item_compare_ic = TRUE;
	}
	എങ്കിൽ (nr != 1)
	{
	    എങ്കിൽ (argvars[1].v_type != VAR_സംഖ്യ)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    അല്ലെങ്കിൽ എങ്കിൽ (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		തിരികെ FAIL;
	    }
	}
	എങ്കിൽ (info->item_compare_func != ശൂന്യ)
	{
	    എങ്കിൽ (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = ശൂന്യ;
		info->item_compare_numeric = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = ശൂന്യ;
		info->item_compare_numbers = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = ശൂന്യ;
		info->item_compare_float = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = ശൂന്യ;
		info->item_compare_ic = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = ശൂന്യ;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	എങ്കിൽ (check_for_dict_arg(argvars, 2) == FAIL)
	    തിരികെ FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    തിരികെ OK;
}

/*
 * "sort()" or "uniq()" function
 */
    സ്ഥിതം ശൂന്യം
do_sort_uniq(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ sort)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    long	len;

    എങ്കിൽ (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	തിരികെ;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    എങ്കിൽ (l != ശൂന്യ && മൂല്യം_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	പോകുക theend;
    rettv_list_set(rettv, l);
    എങ്കിൽ (l == ശൂന്യ)
	പോകുക theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    എങ്കിൽ (len <= 1)
	പോകുക theend;	// short list sorts pretty quickly

    എങ്കിൽ (parse_sort_uniq_args(argvars, &info) == FAIL)
	പോകുക theend;

    എങ്കിൽ (sort)
	do_sort(l, &info);
    അല്ലെങ്കിൽ
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    ശൂന്യം
f_sort(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    ശൂന്യം
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    പൂർണ്ണസംഖ്യ
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*വ്യഞ്ജകം,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	പൂർണ്ണസംഖ്യ		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    പൂർണ്ണസംഖ്യ		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    എങ്കിൽ (filtermap == FILTERMAP_FOREACH && വ്യഞ്ജകം->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(വ്യഞ്ജകം->vval.v_string);
	എങ്കിൽ (!did_emsg)
	    retval = OK;
	പോകുക theend;
    }

    argv[0] = *get_vim_var_tv(VV_താക്കോൽ);
    argv[1] = *get_vim_var_tv(VV_VAL);
    എങ്കിൽ (eval_വ്യഞ്ജകം_typval(വ്യഞ്ജകം, FALSE, argv, 2, fc, newtv) == FAIL)
	പോകുക theend;
    എങ്കിൽ (filtermap == FILTERMAP_FILTER)
    {
	പൂർണ്ണസംഖ്യ	    error = FALSE;

	// filter(): when expr is zero remove the item
	എങ്കിൽ (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	അല്ലെങ്കിൽ
	    *remp = (tv_get_സംഖ്യ_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	എങ്കിൽ (error)
	    പോകുക theend;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    തിരികെ retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    സ്ഥിതം ശൂന്യം
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	അക്ഷരം		*func_name,
	char_u		*arg_errmsg,
	typval_T	*വ്യഞ്ജകം,
	typval_T	*rettv)
{
    പൂർണ്ണസംഖ്യ		prev_lock;
    list_T	*l_ret = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		idx = 0;
    പൂർണ്ണസംഖ്യ		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    എങ്കിൽ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = ശൂന്യ;
    }
    എങ്കിൽ (l == ശൂന്യ || (filtermap == FILTERMAP_FILTER
			    && മൂല്യം_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	തിരികെ;

    prev_lock = l->lv_lock;

    എങ്കിൽ (filtermap == FILTERMAP_MAPNEW)
    {
	എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	    തിരികെ;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_താക്കോൽ, VAR_സംഖ്യ);

    എങ്കിൽ (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_വ്യഞ്ജകം_get_funccal(വ്യഞ്ജകം, &newtv);

    എങ്കിൽ (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	പൂർണ്ണസംഖ്യ		len = l->lv_len;
	പൂർണ്ണസംഖ്യ		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	എങ്കിൽ (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = ശൂന്യ;
	    l->lv_u.mat.lv_last = ശൂന്യ;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = ശൂന്യ;
	}

	വേണ്ടി (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_സംഖ്യ;
	    tv.v_lock = 0;
	    tv.vval.v_സംഖ്യ = val;
	    set_vim_var_nr(VV_താക്കോൽ, idx);
	    എങ്കിൽ (filter_map_one(&tv, വ്യഞ്ജകം, filtermap, fc, &newtv, &rem) == FAIL)
		നിർത്തുക;
	    എങ്കിൽ (did_emsg)
	    {
		clear_tv(&newtv);
		നിർത്തുക;
	    }
	    എങ്കിൽ (filtermap != FILTERMAP_FOREACH)
	    {
		എങ്കിൽ (filtermap != FILTERMAP_FILTER)
		{
		    എങ്കിൽ (filtermap == FILTERMAP_MAP && argtype != ശൂന്യ
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			നിർത്തുക;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    എങ്കിൽ (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			നിർത്തുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (!rem)
		{
		    // filter(): append the list item value when not rem
		    എങ്കിൽ (list_append_tv_move(l, &tv) == FAIL)
			നിർത്തുക;
		}
	    }

	    val += stride;
	}
    }
    അല്ലെങ്കിൽ
    {
	// Materialized list: loop over the items
	വേണ്ടി (li = l->lv_first; li != ശൂന്യ; li = nli)
	{
	    എങ്കിൽ (filtermap == FILTERMAP_MAP && മൂല്യം_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		നിർത്തുക;
	    nli = li->li_next;
	    set_vim_var_nr(VV_താക്കോൽ, idx);
	    എങ്കിൽ (filter_map_one(&li->li_tv, വ്യഞ്ജകം, filtermap, fc,
							 &newtv, &rem) == FAIL)
		നിർത്തുക;
	    എങ്കിൽ (did_emsg)
	    {
		clear_tv(&newtv);
		നിർത്തുക;
	    }
	    എങ്കിൽ (filtermap == FILTERMAP_MAP)
	    {
		എങ്കിൽ (argtype != ശൂന്യ && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    നിർത്തുക;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_lock = 0;
		li->li_tv = newtv;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		എങ്കിൽ (list_append_tv_move(l_ret, &newtv) == FAIL)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = prev_lock;
    എങ്കിൽ (fc != ശൂന്യ)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    സ്ഥിതം ശൂന്യം
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*വ്യഞ്ജകം;
    അക്ഷരം	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    പൂർണ്ണസംഖ്യ		save_did_emsg;
    type_T	*type = ശൂന്യ;

    // map(), filter(), foreach() return the first argument, also on failure.
    എങ്കിൽ (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    എങ്കിൽ (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	തിരികെ;
    }

    എങ്കിൽ (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	എങ്കിൽ (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != ശൂന്യ)
	    type = argvars[0].vval.v_dict->dv_type;
	അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != ശൂന്യ)
	    type = argvars[0].vval.v_list->lv_type;
    }

    എങ്കിൽ (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	അക്ഷരം *സന്ദേശം;

	എങ്കിൽ (filtermap == FILTERMAP_FOREACH)
	    സന്ദേശം = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	അല്ലെങ്കിൽ
	    സന്ദേശം = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(സന്ദേശം), func_name);
	തിരികെ;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    വ്യഞ്ജകം = &argvars[1];
    എങ്കിൽ (വ്യഞ്ജകം->v_type == VAR_UNKNOWN)
	തിരികെ;

    typval_T	save_val;
    typval_T	save_താക്കോൽ;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_താക്കോൽ, &save_താക്കോൽ);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    എങ്കിൽ (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, വ്യഞ്ജകം, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, വ്യഞ്ജകം,
							    arg_errmsg, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, വ്യഞ്ജകം, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, വ്യഞ്ജകം);
    അല്ലെങ്കിൽ // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, വ്യഞ്ജകം, rettv);

    restore_vimvar(VV_താക്കോൽ, &save_താക്കോൽ);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    ശൂന്യം
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    ശൂന്യം
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    ശൂന്യം
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    ശൂന്യം
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    സ്ഥിതം ശൂന്യം
list_add(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    എങ്കിൽ (l == ശൂന്യ)
    {
	എങ്കിൽ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!മൂല്യം_check_lock(l->lv_lock,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    ശൂന്യം
f_add(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = 1; // Default: Failed

    എങ്കിൽ (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_സംഖ്യ_arg(argvars, 1) == FAIL)))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    അല്ലെങ്കിൽ
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    സ്ഥിതം long
list_എണ്ണം(list_T *l, typval_T *needle, long idx, പൂർണ്ണസംഖ്യ ic)
{
    long	n = 0;
    listitem_T	*li;

    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ 0;

    CHECK_LIST_MATERIALIZE(l);

    എങ്കിൽ (list_len(l) == 0)
	തിരികെ 0;

    li = list_find(l, idx);
    എങ്കിൽ (li == ശൂന്യ)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	തിരികെ 0;
    }

    വേണ്ടി ( ; li != ശൂന്യ; li = li->li_next)
	എങ്കിൽ (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    തിരികെ n;
}

/*
 * "count()" function
 */
    ശൂന്യം
f_എണ്ണം(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    പൂർണ്ണസംഖ്യ		ic = FALSE;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 3) == FAIL)))
	തിരികെ;

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	ic = (പൂർണ്ണസംഖ്യ)tv_get_bool_chk(&argvars[2], &error);

    എങ്കിൽ (!error && argvars[0].v_type == VAR_STRING)
	n = string_എണ്ണം(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    അല്ലെങ്കിൽ എങ്കിൽ (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_സംഖ്യ_chk(&argvars[3], &error);
	എങ്കിൽ (!error)
	    n = list_എണ്ണം(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_സംഖ്യ_chk(&argvars[3], &error);
	എങ്കിൽ (!error)
	    n = tuple_എണ്ണം(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!error && argvars[0].v_type == VAR_DICT)
    {
	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	അല്ലെങ്കിൽ
	    n = dict_എണ്ണം(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_സംഖ്യ = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    സ്ഥിതം ശൂന്യം
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	അക്ഷരം		*func_name,
	char_u		*arg_errmsg,
	പൂർണ്ണസംഖ്യ		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*item;
    long	before;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    l1 = argvars[0].vval.v_list;
    എങ്കിൽ (l1 == ശൂന്യ)
    {
	emsg(_(e_cannot_extend_null_list));
	തിരികെ;
    }
    എങ്കിൽ (is_new || !മൂല്യം_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	എങ്കിൽ (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    എങ്കിൽ (l1 == ശൂന്യ)
		തിരികെ;
	}

	l2 = argvars[1].vval.v_list;
	എങ്കിൽ (l2 == ശൂന്യ)
	    പോകുക theend;

	എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_സംഖ്യ_chk(&argvars[2], &error);
	    എങ്കിൽ (error)
		പോകുക cleanup;		// type error; errmsg already given
	    എങ്കിൽ (before == l1->lv_len)
		item = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		item = list_find(l1, before);
		എങ്കിൽ (item == ശൂന്യ)
		{
		    semsg(_(e_list_index_out_of_range_nr), before);
		    പോകുക cleanup;
		}
	    }
	}
	അല്ലെങ്കിൽ
	    item = ശൂന്യ;
	എങ്കിൽ (type != ശൂന്യ && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    പോകുക cleanup;
	list_extend(l1, l2, item);

theend:
	എങ്കിൽ (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	അല്ലെങ്കിൽ
	    copy_tv(&argvars[0], rettv);
	തിരികെ;

cleanup:
	എങ്കിൽ (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    സ്ഥിതം ശൂന്യം
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, പൂർണ്ണസംഖ്യ is_new)
{
    type_T	*type = ശൂന്യ;
    അക്ഷരം	*func_name = is_new ? "extendnew()" : "extend()";

    എങ്കിൽ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	എങ്കിൽ (!is_new && in_vim9script() && argvars[0].vval.v_list != ശൂന്യ)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	എങ്കിൽ (!is_new && in_vim9script() && argvars[0].vval.v_dict != ശൂന്യ)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    അല്ലെങ്കിൽ
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    ശൂന്യം
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
    ശൂന്യം
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    സ്ഥിതം ശൂന്യം
list_തിരുകുക_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    പൂർണ്ണസംഖ്യ		error = FALSE;

    എങ്കിൽ (l == ശൂന്യ)
    {
	എങ്കിൽ (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	തിരികെ;
    }

    എങ്കിൽ (മൂല്യം_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	തിരികെ;

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_സംഖ്യ_chk(&argvars[2], &error);
    എങ്കിൽ (error)
	തിരികെ;		// type error; errmsg already given

    എങ്കിൽ (before == l->lv_len)
	item = ശൂന്യ;
    അല്ലെങ്കിൽ
    {
	item = list_find(l, before);
	എങ്കിൽ (item == ശൂന്യ)
	{
	    semsg(_(e_list_index_out_of_range_nr), before);
	    l = ശൂന്യ;
	}
    }
    എങ്കിൽ (l != ശൂന്യ)
    {
	(ശൂന്യം)list_തിരുകുക_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    ശൂന്യം
f_തിരുകുക(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_സംഖ്യ_arg(argvars, 1) == FAIL)
		|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
	blob_തിരുകുക_func(argvars, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    അല്ലെങ്കിൽ
	list_തിരുകുക_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    ശൂന്യം
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    എങ്കിൽ (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_സംഖ്യ_arg(argvars, 1) == FAIL
			|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_സംഖ്യ_arg(argvars, 1) == FAIL)))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    അല്ലെങ്കിൽ
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    സ്ഥിതം ശൂന്യം
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    എങ്കിൽ (l != ശൂന്യ
	    && !മൂല്യം_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	എങ്കിൽ (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    തിരികെ;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = ശൂന്യ;
	l->lv_len = 0;
	ആകുമ്പോൾ (li != ശൂന്യ)
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
    ശൂന്യം
f_reverse(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	എങ്കിൽ (argvars[0].vval.v_string != ശൂന്യ)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	അല്ലെങ്കിൽ
	    rettv->vval.v_string = ശൂന്യ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_TUPLE)
	tuple_reverse(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    സ്ഥിതം ശൂന്യം
list_reduce(
	typval_T	*argvars,
	typval_T	*വ്യഞ്ജകം,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		range_list;
    പൂർണ്ണസംഖ്യ		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    പൂർണ്ണസംഖ്യ		r;
    പൂർണ്ണസംഖ്യ		called_emsg_start = called_emsg;
    പൂർണ്ണസംഖ്യ		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != ശൂന്യ && l->lv_first == &range_list_item;
    എങ്കിൽ (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    എങ്കിൽ (argvars[2].v_type == VAR_UNKNOWN)
    {
	എങ്കിൽ (l == ശൂന്യ || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_മൂല്യം), "List");
	    തിരികെ;
	}
	എങ്കിൽ (range_list)
	{
	    initial.v_type = VAR_സംഖ്യ;
	    initial.vval.v_സംഖ്യ = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	അല്ലെങ്കിൽ
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_next;
	}
    }
    അല്ലെങ്കിൽ
    {
	initial = argvars[2];
	എങ്കിൽ (l != ശൂന്യ && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_വ്യഞ്ജകം_get_funccal(വ്യഞ്ജകം, rettv);

    prev_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    ആകുമ്പോൾ (range_list ? range_idx < l->lv_len : li != ശൂന്യ)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	എങ്കിൽ (range_list)
	{
	    argv[1].v_type = VAR_സംഖ്യ;
	    argv[1].vval.v_സംഖ്യ = range_val;
	}
	അല്ലെങ്കിൽ
	    argv[1] = li->li_tv;

	r = eval_വ്യഞ്ജകം_typval(വ്യഞ്ജകം, TRUE, argv, 2, fc, rettv);

	എങ്കിൽ (argv[0].v_type != VAR_സംഖ്യ && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	എങ്കിൽ (r == FAIL || called_emsg != called_emsg_start)
	    നിർത്തുക;

	// advance to the next item
	എങ്കിൽ (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	അല്ലെങ്കിൽ
	    li = li->li_next;
    }

    എങ്കിൽ (fc != ശൂന്യ)
	remove_funccal();

    l->lv_lock = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    ശൂന്യം
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    എങ്കിൽ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    അല്ലെങ്കിൽ
	func_name = tv_get_string_strict(&argvars[1]);
    എങ്കിൽ (func_name == ശൂന്യ || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	തിരികെ;
    }

    എങ്കിൽ (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    അല്ലെങ്കിൽ
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    ശൂന്യം
f_slice(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_opt_സംഖ്യ_arg(argvars, 2) == FAIL))
	തിരികെ;

    എങ്കിൽ (check_can_index(&argvars[0], TRUE, FALSE) != OK)
	തിരികെ;

    copy_tv(argvars, rettv);
    eval_index_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? ശൂന്യ : argvars + 2,
	    TRUE, ശൂന്യ, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
