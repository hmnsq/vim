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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

// List heads for garbage collection.
ସ୍ଥିର list_T		*first_list = ଖାଲି;	// list of all lists

#define FOR_ALL_WATCHERS(l, lw) \
    ପାଇଁ ((lw) = (l)->lv_watch; (lw) != ଖାଲି; (lw) = (lw)->lw_next)

ସ୍ଥିର ଶୂନ୍ୟ list_free_item(list_T *l, listitem_T *item);

/*
 * Add a watcher to a list.
 */
    ଶୂନ୍ୟ
list_add_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_next = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    ଶୂନ୍ୟ
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	ଯଦି (lw == lwrem)
	{
	    *lwp = lw->lw_next;
	    ଭାଙ୍ଗ;
	}
	lwp = &lw->lw_next;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_fix_watch(list_T *l, listitem_T *item)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	ଯଦି (lw->lw_item == item)
	    lw->lw_item = item->li_next;
}

    ସ୍ଥିର ଶୂନ୍ୟ
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    ଯଦି (first_list != ଖାଲି)
	first_list->lv_used_prev = l;
    l->lv_used_prev = ଖାଲି;
    l->lv_used_next = first_list;
    first_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(ଶୂନ୍ୟ)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    ଯଦି (l != ଖାଲି)
	list_init(l);
    ଫେରନ୍ତୁ l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ଯଦି (alloc_fail_id == id && alloc_does_fail(ଆକାର(list_T)))
	ଫେରନ୍ତୁ ଖାଲି;
#endif
    ଫେରନ୍ତୁ (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    list_T	*l;

    ଯଦି (ଗଣନା_2 > 0
	    && (size_t)ଗଣନା_2 > (SIZE_MAX - ଆକାର(list_T)) / ଆକାର(listitem_T))
	ଫେରନ୍ତୁ ଖାଲି;
    l = (list_T *)alloc_clear(ଆକାର(list_T) + ଗଣନା_2 * ଆକାର(listitem_T));
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    list_init(l);

    ଯଦି (ଗଣନା_2 <= 0)
	ଫେରନ୍ତୁ l;

    listitem_T	*li = (listitem_T *)(l + 1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    l->lv_len = ଗଣନା_2;
    l->lv_with_ବସ୍ତୁଗୁଡ଼ିକ = ଗଣନା_2;
    l->lv_first = li;
    l->lv_u.mat.lv_last = li + ଗଣନା_2 - 1;
    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
    {
	ଯଦି (i == 0)
	    li->li_prev = ଖାଲି;
	ଅନ୍ୟଥା
	    li->li_prev = li - 1;
	ଯଦି (i == ଗଣନା_2 - 1)
	    li->li_next = ଖାଲି;
	ଅନ୍ୟଥା
	    li->li_next = li + 1;
	++li;
    }

    ଫେରନ୍ତୁ l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    ଶୂନ୍ୟ
list_set_item(list_T *l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    rettv->v_lock = 0;
    rettv_list_set(rettv, l);
    ଫେରନ୍ତୁ OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ଯଦି (alloc_fail_id == id && alloc_does_fail(ଆକାର(list_T)))
	ଫେରନ୍ତୁ FAIL;
#endif
    ଫେରନ୍ତୁ rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    ଶୂନ୍ୟ
rettv_list_set(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ଯଦି (l != ଖାଲି)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    ଶୂନ୍ୟ
list_unref(list_T *l)
{
    ଯଦି (l != ଖାଲି && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_free_contents(list_T *l)
{
    listitem_T *item;

    ଯଦି (l->lv_first != &range_list_item)
	ପାଇଁ (item = l->lv_first; item != ଖାଲି; item = l->lv_first)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_free_nonref(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    list_T	*ll;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_free = FALSE;

    ପାଇଁ (ll = first_list; ll != ଖାଲି; ll = ll->lv_used_next)
	ଯଦି ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == ଖାଲି)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    ଫେରନ୍ତୁ did_free;
}

    ସ୍ଥିର ଶୂନ୍ୟ
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    ଯଦି (l->lv_used_prev == ଖାଲି)
	first_list = l->lv_used_next;
    ଅନ୍ୟଥା
	l->lv_used_prev->lv_used_next = l->lv_used_next;
    ଯଦି (l->lv_used_next != ଖାଲି)
	l->lv_used_next->lv_used_prev = l->lv_used_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    ଶୂନ୍ୟ
list_free_ବସ୍ତୁଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    list_T	*ll, *ll_next;

    ପାଇଁ (ll = first_list; ll != ଖାଲି; ll = ll_next)
    {
	ll_next = ll->lv_used_next;
	ଯଦି ((ll->lv_copyID & COPYID_MASK) != (copyID & COPYID_MASK)
						      && ll->lv_watch == ଖାଲି)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    ଶୂନ୍ୟ
list_free(list_T *l)
{
    ଯଦି (in_free_unref_ବସ୍ତୁଗୁଡ଼ିକ)
	ଫେରନ୍ତୁ;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_free_item(list_T *l, listitem_T *item)
{
    ଯଦି (l->lv_with_ବସ୍ତୁଗୁଡ଼ିକ == 0 || item < (listitem_T *)l
			   || item >= (listitem_T *)(l + 1) + l->lv_with_ବସ୍ତୁଗୁଡ଼ିକ)
	vim_free(item);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    ଶୂନ୍ୟ
listitem_free(list_T *l, listitem_T *item)
{
    clear_tv(&item->li_tv);
    list_free_item(l, item);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    ଶୂନ୍ୟ
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
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ 0L;
    ଫେରନ୍ତୁ l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_equal(
    list_T	*l1,
    list_T	*l2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    ଯଦି (l1 == l2)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (list_len(l1) != list_len(l2))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (list_len(l1) == 0)
	// empty and NULL list are considered equal
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (l1 == ଖାଲି || l2 == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    ପାଇଁ (item1 = l1->lv_first, item2 = l2->lv_first;
	    item1 != ଖାଲି && item2 != ଖାଲି;
			       item1 = item1->li_next, item2 = item2->li_next)
	ଯଦି (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ item1 == ଖାଲି && item2 == ଖାଲି;
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

    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Negative index is relative to the end.
    ଯଦି (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    ଯଦି (n < 0 || n >= l->lv_len)
	ଫେରନ୍ତୁ ଖାଲି;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    ଯଦି (n >= l->lv_len)
	ଫେରନ୍ତୁ ଖାଲି;

    // When there is a cached index may start search from there.
    ଯଦି (l->lv_u.mat.lv_idx_item != ଖାଲି)
    {
	ଯଦି (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	ଅନ୍ୟଥା ଯଦି (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
	ଅନ୍ୟଥା
	{
	    // closest to the cached index
	    item = l->lv_u.mat.lv_idx_item;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    item = l->lv_first;
	    idx = 0;
	}
	ଅନ୍ୟଥା
	{
	    // closest to the end of the list
	    item = l->lv_u.mat.lv_last;
	    idx = l->lv_len - 1;
	}
    }

    ଯେତେବେଳେ (n > idx)
    {
	// search forward
	item = item->li_next;
	++idx;
    }
    ଯେତେବେଳେ (n < idx)
    {
	// search backward
	item = item->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_item = item;

    ଫେରନ୍ତୁ item;
}

/*
 * Get list item "l[idx]" as a number.
 */
    long
list_find_nr(
    list_T	*l,
    long	idx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    ଯଦି (l != ଖାଲି && l->lv_first == &range_list_item)
    {
	long	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	ଯଦି (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	ଯଦି (n < 0 || n >= l->lv_len)
	{
	    ଯଦି (errorp != ଖାଲି)
		*errorp = TRUE;
	    ଫେରନ୍ତୁ -1L;
	}

	ଫେରନ୍ତୁ l->lv_u.nonmat.lv_start + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_find(l, idx);
    ଯଦି (li == ଖାଲି)
    {
	ଯଦି (errorp != ଖାଲି)
	    *errorp = TRUE;
	ଫେରନ୍ତୁ -1L;
    }
    ଫେରନ୍ତୁ (long)tv_get_ସଂଖ୍ୟା_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_find_str(list_T *l, long idx)
{
    listitem_T	*li;

    li = list_find(l, idx - 1);
    ଯଦି (li == ଖାଲି)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_find_index(list_T *l, long *idx)
{
    listitem_T *li = list_find(l, *idx);

    ଯଦି (li != ଖାଲି)
	ଫେରନ୍ତୁ li;

    ଯଦି (*idx < 0)
    {
	*idx = 0;
	li = list_find(l, *idx);
    }
    ଫେରନ୍ତୁ li;
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

    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    ପାଇଁ (li = l->lv_first; li != ଖାଲି && li != item; li = li->li_next)
	++idx;
    ଯଦି (li == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    ଶୂନ୍ୟ
list_append(list_T *l, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    ଯଦି (l->lv_u.mat.lv_last == ଖାଲି)
    {
	// empty list
	l->lv_first = item;
	item->li_prev = ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	l->lv_u.mat.lv_last->li_next = item;
	item->li_prev = l->lv_u.mat.lv_last;
    }
    l->lv_u.mat.lv_last = item;
    ++l->lv_len;
    item->li_next = ଖାଲି;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    ଯଦି (l->lv_type != ଖାଲି && l->lv_type->tt_member != ଖାଲି
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ଖାଲି, 0) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    li = listitem_alloc();
    ଯଦି (li == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    ଫେରନ୍ତୁ OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    ଯଦି (li == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    ଫେରନ୍ତୁ OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    ଯଦି (li == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    ଫେରନ୍ତୁ OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    ଯଦି (li == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    ଫେରନ୍ତୁ OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_append_string(list_T *l, char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    listitem_T *li = listitem_alloc();

    ଯଦି (li == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_lock = 0;
    ଯଦି (str == ଖାଲି)
	li->li_tv.vval.v_string = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_append_ସଂଖ୍ୟା(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    ଯଦି (li == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    li->li_tv.v_type = VAR_ସଂଖ୍ୟା;
    li->li_tv.v_lock = 0;
    li->li_tv.vval.v_ସଂଖ୍ୟା = n;
    list_append(l, li);
    ଫେରନ୍ତୁ OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_ଭର୍ତ୍ତି_tv(list_T *l, typval_T *tv, listitem_T *item)
{
    listitem_T	*ni;

    ଯଦି (l->lv_type != ଖାଲି && l->lv_type->tt_member != ଖାଲି
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ଖାଲି, 0) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ni = listitem_alloc();
    ଯଦି (ni == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    copy_tv(tv, &ni->li_tv);
    list_ଭର୍ତ୍ତି(l, ni, item);
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
list_ଭର୍ତ୍ତି(list_T *l, listitem_T *ni, listitem_T *item)
{
    CHECK_LIST_MATERIALIZE(l);
    ଯଦି (item == ଖାଲି)
	// Append new item at end of list.
	list_append(l, ni);
    ଅନ୍ୟଥା
    {
	// Insert new item before existing item.
	ni->li_prev = item->li_prev;
	ni->li_next = item;
	ଯଦି (item->li_prev == ଖାଲି)
	{
	    l->lv_first = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	ଅନ୍ୟଥା
	{
	    item->li_prev->li_next = ni;
	    l->lv_u.mat.lv_idx_item = ଖାଲି;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_append_new_item(list_T *l)
{
    typval_T	tv;

    ଯଦି (l->lv_type != ଖାଲି && l->lv_type->tt_member != ଖାଲି)
	tv.v_type = l->lv_type->tt_member->tt_type;
    ଅନ୍ୟଥା
	tv.v_type = VAR_ସଂଖ୍ୟା;
    tv.v_lock = 0;

    ବାଛ (tv.v_type)
    {
	ସ୍ଥିତି VAR_BOOL: tv.vval.v_ସଂଖ୍ୟା = VVAL_FALSE; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_SPECIAL: tv.vval.v_ସଂଖ୍ୟା = 0; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା: tv.vval.v_ସଂଖ୍ୟା = 0; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FLOAT: tv.vval.v_float = 0; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_STRING: tv.vval.v_string = ଖାଲି; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB: tv.vval.v_blob = blob_alloc(); ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FUNC: tv.vval.v_string = ଖାଲି; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_PARTIAL: tv.vval.v_partial = ଖାଲି; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST: tv.vval.v_list = list_alloc(); ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT: tv.vval.v_dict = dict_alloc(); ଭାଙ୍ଗ;
#ifdef FEAT_JOB_CHANNEL
	ସ୍ଥିତି VAR_JOB: tv.vval.v_job = ଖାଲି; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CHANNEL: tv.vval.v_channel = ଖାଲି; ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_CLASS:
	    ଯଦି (l->lv_type != ଖାଲି && l->lv_type->tt_member != ଖାଲି)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    ଅନ୍ୟଥା
		tv.vval.v_class = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    ଯଦି (l->lv_type != ଖାଲି && l->lv_type->tt_member != ଖାଲି)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    ଅନ୍ୟଥା
		tv.vval.v_object = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TYPEALIAS: tv.vval.v_typealias = ଖାଲି; ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    tv.v_type = VAR_ସଂଖ୍ୟା;
	    tv.vval.v_ସଂଖ୍ୟା = 0;
	    ଭାଙ୍ଗ;
    }

    ଯଦି (l->lv_type != ଖାଲି && l->lv_type->tt_member != ଖାଲି)
	set_tv_type(&tv, l->lv_type->tt_member);

    ଫେରନ୍ତୁ list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_index_one(list_T *l, long *n1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା can_append, ପୂର୍ଣ୍ଣସଂଖ୍ୟା quiet)
{
    long	orig_n1 = *n1;
    listitem_T	*li = list_find_index(l, n1);

    ଯଦି (li != ଖାଲି)
	ଫେରନ୍ତୁ li;

    // Vim9: Allow for adding an item at the end.
    ଯଦି (can_append && in_vim9script()
	    && *n1 == l->lv_len && l->lv_lock == 0)
    {
	ଯଦି (list_append_new_item(l) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	li = list_find_index(l, n1);
    }
    ଯଦି (li == ଖାଲି)
    {
	ଯଦି (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), orig_n1);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_range_index_two(
	list_T	    *l,
	long	    *n1,
	listitem_T  *li1,
	long	    *n2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    quiet)
{
    ଯଦି (*n2 < 0)
    {
	listitem_T	*ni = list_find(l, *n2);

	ଯଦି (ni == ଖାଲି)
	{
	    ଯଦି (!quiet)
		semsg(_(e_list_index_out_of_range_nr), *n2);
	    ଫେରନ୍ତୁ FAIL;
	}
	*n2 = list_idx_of_item(l, ni);
    }

    // Check that n2 isn't before n1.
    ଯଦି (*n1 < 0)
	*n1 = list_idx_of_item(l, li1);
    ଯଦି (*n2 < *n1)
    {
	ଯଦି (!quiet)
	    semsg(_(e_list_index_out_of_range_nr), *n2);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_assign_range(
	list_T	    *dest,
	list_T	    *src,
	long	    idx1_arg,
	long	    idx2,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    empty_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    long	idx1 = idx1_arg;
    listitem_T	*first_li = list_find_index(dest, &idx1);
    long	idx;
    type_T	*member_type = ଖାଲି;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = first_li;
    ପାଇଁ (src_li = src->lv_first; src_li != ଖାଲି && dest_li != ଖାଲି; )
    {
	ଯଦି (ମୂଲ୍ୟ_check_lock(dest_li->li_tv.v_lock, varname, FALSE))
	    ଫେରନ୍ତୁ FAIL;
	src_li = src_li->li_next;
	ଯଦି (src_li == ଖାଲି || (!empty_idx2 && idx2 == idx))
	    ଭାଙ୍ଗ;
	dest_li = dest_li->li_next;
	++idx;
    }

    ଯଦି (in_vim9script() && dest->lv_type != ଖାଲି
					   && dest->lv_type->tt_member != ଖାଲି)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = first_li;
    ପାଇଁ (src_li = src->lv_first; src_li != ଖାଲି; )
    {
	ଯଦି (op != ଖାଲି && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	ଅନ୍ୟଥା
	{
	    ଯଦି (member_type != ଖାଲି
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      ଖାଲି, 0) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_next;
	ଯଦି (src_li == ଖାଲି || (!empty_idx2 && idx2 == idx))
	    ଭାଙ୍ଗ;
	ଯଦି (dest_li->li_next == ଖାଲି)
	{
	    // Need to add an empty item.
	    ଯଦି (list_append_ସଂଖ୍ୟା(dest, 0) == FAIL)
	    {
		src_li = ଖାଲି;
		ଭାଙ୍ଗ;
	    }
	}
	dest_li = dest_li->li_next;
	++idx;
    }
    ଯଦି (src_li != ଖାଲି)
    {
	emsg(_(e_list_ମୂଲ୍ୟ_has_more_ବସ୍ତୁଗୁଡ଼ିକ_than_targets));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (empty_idx2
	    ? (dest_li != ଖାଲି && dest_li->li_next != ଖାଲି)
	    : idx != idx2)
    {
	emsg(_(e_list_ମୂଲ୍ୟ_does_not_have_enough_ବସ୍ତୁଗୁଡ଼ିକ));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_flatten(list_T *list, listitem_T *first, long maxitems, long maxdepth)
{
    listitem_T	*item;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done = 0;

    ଯଦି (maxdepth == 0)
	ଫେରନ୍ତୁ;
    CHECK_LIST_MATERIALIZE(list);
    ଯଦି (first == ଖାଲି)
	item = list->lv_first;
    ଅନ୍ୟଥା
	item = first;

    ଯେତେବେଳେ (item != ଖାଲି && done < maxitems)
    {
	listitem_T	*next = item->li_next;

	fast_breakcheck();
	ଯଦି (got_int)
	    ଫେରନ୍ତୁ;

	ଯଦି (item->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = item->li_tv.vval.v_list;

	    vimlist_remove(list, item, item);
	    ଯଦି (list_extend(list, itemlist, next) == FAIL)
	    {
		list_free_item(list, item);
		ଫେରନ୍ତୁ;
	    }

	    ଯଦି (maxdepth > 0)
		list_flatten(list, item->li_prev == ଖାଲି
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
    ସ୍ଥିର ଶୂନ୍ୟ
flatten_common(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_copy)
{
    list_T  *l;
    long    maxdepth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    error = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), "flatten()");
	ଫେରନ୍ତୁ;
    }

    ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    ଅନ୍ୟଥା
    {
	maxdepth = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
	ଯଦି (error)
	    ଫେରନ୍ତୁ;
	ଯଦି (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_ସଂଖ୍ୟା));
	    ଫେରନ୍ତୁ;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	ଯଦି (l == ଖାଲି)
	    ଫେରନ୍ତୁ;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (ମୂଲ୍ୟ_check_lock(l->lv_lock,
				     (char_u *)N_("flatten() argument"), TRUE))
	    ଫେରନ୍ତୁ;
	++l->lv_refcount;
    }

    list_flatten(l, ଖାଲି, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    ଶୂନ୍ୟ
f_flatten(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script())
	emsg(_(e_cannot_use_flatten_in_vim9_script));
    ଅନ୍ୟଥା
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    ଶୂନ୍ୟ
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    ଶୂନ୍ୟ
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    ପାଇଁ (idx = 0, li = l->lv_first; li != ଖାଲି; li = li->li_next, ++idx)
    {
	list_T	    *l2 = list_alloc();

	ଯଦି (l2 == ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯଦି (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (list_append_ସଂଖ୍ୟା(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    ଭାଙ୍ଗ;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    ଶୂନ୍ୟ
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;  // null string behaves like an empty string

    ପାଇଁ (idx = 0; *p != NUL; ++idx)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = mb_ptr2len(p);
	list_T	    *l2;

	ଯଦି (len == 0)
	    ଭାଙ୍ଗ;
	l2 = list_alloc();
	ଯଦି (l2 == ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯଦି (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (list_append_ସଂଖ୍ୟା(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    ଭାଙ୍ଗ;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*item;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    ଯଦି (l2 == ଖାଲି || l2->lv_len == 0)
	ଫେରନ୍ତୁ OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == ଖାଲି ? ଖାଲି : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    ପାଇଁ (item = l2->lv_first; item != ଖାଲି && --todo >= 0;
				 item = item == bef_prev ? bef : item->li_next)
	ଯଦି (list_ଭର୍ତ୍ତି_tv(l1, &item->li_tv, bef) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len1 = l1 == ଖାଲି ? 0 : l1->lv_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len2 = l2 == ଖାଲି ? 0 : l2->lv_len;
    long	totallen = (long)len1 + (long)len2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    listitem_T	*item;

    ଯଦି (totallen == 0)
    {
	l = list_alloc();
	ଯଦି (l == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_lock = 0;
	tv->vval.v_list = l;
	ଫେରନ୍ତୁ OK;
    }
    ଯଦି (totallen > INT_MAX)
	ଫେରନ୍ତୁ FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)totallen);
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    i = 0;
    ଯଦି (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	ପାଇଁ (item = l1->lv_first; item != ଖାଲି && !got_int;
		item = item->li_next)
	{
	    typval_T new_tv;

	    copy_tv(&item->li_tv, &new_tv);
	    list_set_item(l, i++, &new_tv);
	}
    }
    ଯଦି (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	ପାଇଁ (item = l2->lv_first; item != ଖାଲି && !got_int;
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
    ଫେରନ୍ତୁ OK;
}

    list_T *
list_slice(list_T *ol, long n1, long n2)
{
    listitem_T	*item;
    list_T	*l;
    long	ଗଣନା_2;
    long	idx;

    ଯଦି (n2 < n1)
	ଫେରନ୍ତୁ list_alloc();
    ଯଦି (n1 > INT_MAX - (n2 - n1) - 1)
	ଫେରନ୍ତୁ ଖାଲି;
    ଗଣନା_2 = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଗଣନା_2);
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    item = list_find(ol, n1);
    ପାଇଁ (idx = 0; idx < ଗଣନା_2; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	ଯଦି (item == ଖାଲି)
	{
	    list_free(l);
	    ଫେରନ୍ତୁ ଖାଲି;
	}

	copy_tv(&item->li_tv, &new_tv);
	list_set_item(l, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)idx, &new_tv);
	item = item->li_next;
    }
    ଫେରନ୍ତୁ l;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_slice_or_index(
	    list_T	*list,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		exclusive,
	    typval_T	*rettv,
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose)
{
    long	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ଯଦି (n1 < 0)
	n1 = len + n1;
    ଯଦି (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	ଯଦି (!range)
	{
	    ଯଦି (verbose)
		semsg(_(e_list_index_out_of_range_nr), (long)n1_arg);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ଅନ୍ୟଥା
	    n1 = len;
    }
    ଯଦି (range)
    {
	list_T	*l;

	ଯଦି (n2 < 0)
	    n2 = len + n2;
	ଅନ୍ୟଥା ଯଦି (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ଯଦି (exclusive)
	    --n2;
	ଯଦି (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_slice(list, n1, n2);
	ଯଦି (l == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	clear_tv(rettv);
	rettv_list_set(rettv, l);
    }
    ଅନ୍ୟଥା
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	long index = n1;
	listitem_T *li = check_range_index_one(list, &index, TRUE, TRUE);
	ଯଦି (li == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	ଯଦି (in_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != ଖାଲି
					&& var1.vval.v_list->lv_type == ଖାଲି)
	{
	    ଯଦି (list->lv_type != ଖାଲି && list->lv_type->tt_member != ଖାଲି)
		set_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା deep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    list_T	*copy;
    listitem_T	*item;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;

    ଯଦି (orig == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(orig->lv_len);
    ଯଦି (copy == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (orig->lv_type == ଖାଲି || ଶୀର୍ଷ || deep)
	copy->lv_type = ଖାଲି;
    ଅନ୍ୟଥା
	copy->lv_type = alloc_type(orig->lv_type);
    ଯଦି (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    ପାଇଁ (item = orig->lv_first; item != ଖାଲି && !got_int;
	    item = item->li_next)
    {
	typval_T	new_tv;

	ଯଦି (deep)
	{
	    ଯଦି (item_copy(&item->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	    copy_tv(&item->li_tv, &new_tv);
	list_set_item(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    ଯଦି (item != ଖାଲି)
    {
	list_unref(copy);
	copy = ଖାଲି;
    }

    ଫେରନ୍ତୁ copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    ଶୂନ୍ୟ
vimlist_remove(list_T *l, listitem_T *item, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    ପାଇଁ (ip = item; ip != ଖାଲି; ip = ip->li_next)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	ଯଦି (ip == item2)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (item2->li_next == ଖାଲି)
	l->lv_u.mat.lv_last = item->li_prev;
    ଅନ୍ୟଥା
	item2->li_next->li_prev = item->li_prev;
    ଯଦି (item->li_prev == ଖାଲି)
	l->lv_first = item2->li_next;
    ଅନ୍ୟଥା
	item->li_prev->li_next = item2->li_next;
    l->lv_u.mat.lv_idx_item = ଖାଲି;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା restore_copyID)
{
    garray_T	ga;

    ଯଦି (tv->vval.v_list == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    ଯଦି (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    ଫେରନ୍ତୁ (char_u *)ga.ga_data;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		echo_style,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    join_T	*p;
    long	sumlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = TRUE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_lock;
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
    ଯଦି (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;
    ପାଇଁ (item = l->lv_first; item != ଖାଲି && !got_int; item = item->li_next)
    {
	s.string = echo_string_core(&item->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ଯଦି (s.string == ଖାଲି)
	{
	    l->lv_lock = prev_lock;
	    ଫେରନ୍ତୁ FAIL;
	}

	s.length = STRLEN(s.string);
	sumlen += (long)s.length;

	(ଶୂନ୍ୟ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	ଯଦି (tofree != ଖାଲି || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	ଅନ୍ୟଥା
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	ଧାଡ଼ି_breakcheck();
	ଯଦି (did_echo_string_emsg)  // recursion error, bail out
	    ଭାଙ୍ଗ;
    }
    l->lv_lock = prev_lock;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    ଯଦି (join_gap->ga_len >= 2)
	sumlen += (long)seplen * (join_gap->ga_len - 1);
    ଯଦି (sumlen > INT_MAX - 2 || ga_grow(gap, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sumlen + 2) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ପାଇଁ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ଯଦି (first)
	    first = FALSE;
	ଅନ୍ୟଥା
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	ଯଦି (p->s.string != ଖାଲି)
	    ga_concat_len(gap, p->s.string, p->s.length);
	ଧାଡ଼ି_breakcheck();
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		echo_style,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID)
{
    garray_T	join_ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    join_T	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (l->lv_len < 1)
	ଫେରନ୍ତୁ OK; // nothing to do
    ga_init2(&join_ga, ଆକାର(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ଯଦି (join_ga.ga_data == ଖାଲି)
	ଫେରନ୍ତୁ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    ପାଇଁ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ଫେରନ୍ତୁ retval;
}

/*
 * "join()" function
 */
    ଶୂନ୍ୟ
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    ଯଦି (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == ଖାଲି)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == ଖାଲି))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    ଅନ୍ୟଥା
	sep = tv_get_string_chk(&argvars[1]);

    ଯଦି (sep != ଖାଲି)
    {
	ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 80);
	ଯଦି (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	ଅନ୍ୟଥା
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_data;
    }
    ଅନ୍ୟଥା
	rettv->vval.v_string = ଖାଲି;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_error)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg == ଖାଲି ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = ଖାଲି;
    typval_T	tv;
    listitem_T	*item;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_comma;

    ଯଦି (evaluate)
    {
	l = list_alloc();
	ଯଦି (l == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ଯେତେବେଳେ (**arg != ']' && **arg != NUL)
    {
	ଯଦି (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    ଯାଅ failret;
	ଯଦି (check_typval_is_ମୂଲ୍ୟ(&tv) == FAIL)
	{
	    ଯଦି (evaluate)
		clear_tv(&tv);
	    ଯାଅ failret;
	}
	ଯଦି (evaluate)
	{
	    item = listitem_alloc();
	    ଯଦି (item != ଖାଲି)
	    {
		item->li_tv = tv;
		item->li_tv.v_lock = 0;
		list_append(l, item);
	    }
	    ଅନ୍ୟଥା
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	ଯଦି (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ଯଦି (had_comma)
	{
	    ଯଦି (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ଯାଅ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ଯଦି (**arg == ']')
	    ଭାଙ୍ଗ;

	ଯଦି (!had_comma)
	{
	    ଯଦି (do_error)
	    {
		ଯଦି (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ଅନ୍ୟଥା
		    semsg(_(e_missing_comma_in_list_str), *arg);
	    }
	    ଯାଅ failret;
	}
    }

    ଯଦି (**arg != ']')
    {
	ଯଦି (do_error)
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
failret:
	ଯଦି (evaluate)
	    list_free(l);
	ଫେରନ୍ତୁ FAIL;
    }

    *arg += 1;
    ଯଦି (evaluate)
	rettv_list_set(rettv, l);

    ଫେରନ୍ତୁ OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
write_list(FILE *fd, list_T *list, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଦ୍ୱିମିକ)
{
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(list, li)
    {
	ପାଇଁ (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    ଯଦି (*s == '\n')
		c = putc(NUL, fd);
	    ଅନ୍ୟଥା
		c = putc(*s, fd);
	    ଯଦି (c == EOF)
	    {
		ret = FAIL;
		ଭାଙ୍ଗ;
	    }
	}
	ଯଦି (!ଦ୍ୱିମିକ || li->li_next != ଖାଲି)
	    ଯଦି (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		ଭାଙ୍ଗ;
	    }
	ଯଦି (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    ଭାଙ୍ଗ;
	}
    }
    ଫେରନ୍ତୁ ret;
}

/*
 * Initialize a static list with 10 items.
 */
    ଶୂନ୍ୟ
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    CLEAR_POINTER(sl);
    l->lv_first = &sl->sl_ବସ୍ତୁଗୁଡ଼ିକ[0];
    l->lv_u.mat.lv_last = &sl->sl_ବସ୍ତୁଗୁଡ଼ିକ[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_lock = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    ପାଇଁ (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_ବସ୍ତୁଗୁଡ଼ିକ[i];

	ଯଦି (i == 0)
	    li->li_prev = ଖାଲି;
	ଅନ୍ୟଥା
	    li->li_prev = li - 1;
	ଯଦି (i == 9)
	    li->li_next = ଖାଲି;
	ଅନ୍ୟଥା
	    li->li_next = li + 1;
    }
}

/*
 * "list2str()" function
 */
    ଶୂନ୍ୟ
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    l = argvars[0].vval.v_list;
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;  // empty list results in empty string

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[1], ଖାଲି);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    ଯଦି (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	(*char2bytes)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_u *);

	ଯଦି (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	ଅନ୍ୟଥା
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_ସଂଖ୍ୟା(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    ଅନ୍ୟଥା
    {
	long len = (long)list_len(l) + 1;

	ଯଦି (len <= INT_MAX && ga_grow(&ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len) == OK)
	{
	    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
		ga_append(&ga, tv_get_ସଂଖ୍ୟା(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_data;
}

/*
 * "list2tuple()" function
 */
    ଶୂନ୍ୟ
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ଖାଲି;

    ଯଦି (check_for_list_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    l = argvars[0].vval.v_list;
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    ଯଦି (rettv_tuple_set_with_ବସ୍ତୁଗୁଡ଼ିକ(rettv, list_len(l)) == FAIL)
	ଫେରନ୍ତୁ;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ITEM(tuple, TUPLE_LEN(tuple)));
	tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    ଶୂନ୍ୟ
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_tuple_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    tuple = argvars[0].vval.v_tuple;
    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ITEM(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*item, *item2;
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    long	idx;
    long	end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cnt = 0;
    list_T	*rl;

    ଯଦି ((l = argvars[0].vval.v_list) == ଖାଲି
			     || ମୂଲ୍ୟ_check_lock(l->lv_lock, arg_errmsg, TRUE))
	ଫେରନ୍ତୁ;

    idx = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ;		// type error: do nothing, errmsg already given

    ଯଦି ((item = list_find(l, idx)) == ଖାଲି)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, item, item);
	*rettv = item->li_tv;
	list_free_item(l, item);
	ଫେରନ୍ତୁ;
    }

    // Remove range of items, return list with values.
    end = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ;		// type error: do nothing

    ଯଦି ((item2 = list_find(l, end)) == ଖାଲି)
    {
	semsg(_(e_list_index_out_of_range_nr), end);
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (li = item; li != ଖାଲି; li = li->li_next)
    {
	++cnt;
	ଯଦି (li == item2)
	    ଭାଙ୍ଗ;
    }
    ଯଦି (li == ଖାଲି)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	ଫେରନ୍ତୁ;
    }

    vimlist_remove(l, item, item2);
    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    rl = rettv->vval.v_list;

    ଯଦି (l->lv_with_ବସ୍ତୁଗୁଡ଼ିକ > 0)
    {
	// need to copy the list items and move the value
	ଯେତେବେଳେ (item != ଖାଲି)
	{
	    li = listitem_alloc();
	    ଯଦି (li == ଖାଲି)
		ଫେରନ୍ତୁ;
	    li->li_tv = item->li_tv;
	    init_tv(&item->li_tv);
	    list_append(rl, li);
	    ଯଦି (item == item2)
		ଭାଙ୍ଗ;
	    item = item->li_next;
	}
    }
    ଅନ୍ୟଥା
    {
	rl->lv_first = item;
	rl->lv_u.mat.lv_last = item2;
	item->li_prev = ଖାଲି;
	item2->li_next = ଖାଲି;
	rl->lv_len = cnt;
    }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା item_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା item_compare2(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2);

// struct used in the array that's given to qsort()
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    listitem_T	*item;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    ସଂଘ {
	varnumber_T	inum;	// for "N"
	ଦ୍ୱିଗୁଣ		fnum;	// for "n" and "f"
    } ଚାବି;
} sortItem_T;

// struct storing information about current sort
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_compare_ic;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_compare_lc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_compare_numeric;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_compare_numbers;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_compare_float;
    char_u	*item_compare_func;
    partial_T	*item_compare_partial;
    dict_T	*item_compare_selfdict;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_compare_func_err;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_compare_keep_zero;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item_compare_ଚାବିଗୁଡ଼ିକ_ready;  // ptrs[].key is precomputed
} sortinfo_T;
ସ୍ଥିର sortinfo_T	*sortinfo = ଖାଲି;
#define ITEM_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
item_compare(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    sortItem_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = ଖାଲି, *tofree2 = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;
    tv1 = &si1->item->li_tv;
    tv2 = &si2->item->li_tv;

    ଯଦି (sortinfo->item_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->item_compare_ଚାବିଗୁଡ଼ିକ_ready
					? si1->ଚାବି.inum : tv_to_ସଂଖ୍ୟା(tv1);
	varnumber_T	v2 = sortinfo->item_compare_ଚାବିଗୁଡ଼ିକ_ready
					? si2->ଚାବି.inum : tv_to_ସଂଖ୍ୟା(tv2);

	ଫେରନ୍ତୁ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ଯଦି (sortinfo->item_compare_float)
    {
	float_T	v1 = sortinfo->item_compare_ଚାବିଗୁଡ଼ିକ_ready
					? si1->ଚାବି.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->item_compare_ଚାବିଗୁଡ଼ିକ_ready
					? si2->ଚାବି.fnum : tv_get_float(tv2);

	ଫେରନ୍ତୁ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ଯଦି (sortinfo->item_compare_numeric && sortinfo->item_compare_ଚାବିଗୁଡ଼ିକ_ready)
    {
	ଦ୍ୱିଗୁଣ	n1 = si1->ଚାବି.fnum;
	ଦ୍ୱିଗୁଣ	n2 = si2->ଚାବି.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	ଯଦି (res == 0 && !sortinfo->item_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	ଫେରନ୍ତୁ res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    ଯଦି (tv1->v_type == VAR_STRING)
    {
	ଯଦି (tv2->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p1 = (char_u *)"'";
	ଅନ୍ୟଥା
	    p1 = tv1->vval.v_string;
    }
    ଅନ୍ୟଥା
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    ଯଦି (tv2->v_type == VAR_STRING)
    {
	ଯଦି (tv1->v_type != VAR_STRING || sortinfo->item_compare_numeric)
	    p2 = (char_u *)"'";
	ଅନ୍ୟଥା
	    p2 = tv2->vval.v_string;
    }
    ଅନ୍ୟଥା
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    ଯଦି (p1 == ଖାଲି)
	p1 = (char_u *)"";
    ଯଦି (p2 == ଖାଲି)
	p2 = (char_u *)"";
    ଯଦି (!sortinfo->item_compare_numeric)
    {
	ଯଦି (sortinfo->item_compare_lc)
	    res = strcoll((ଅକ୍ଷର *)p1, (ଅକ୍ଷର *)p2);
	ଅନ୍ୟଥା
	    res = sortinfo->item_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    ଅନ୍ୟଥା
    {
	ଦ୍ୱିଗୁଣ n1, n2;
	n1 = strtod((ଅକ୍ଷର *)p1, (ଅକ୍ଷର **)&p1);
	n2 = strtod((ଅକ୍ଷର *)p2, (ଅକ୍ଷର **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    ଯଦି (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    ଫେରନ୍ତୁ res;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
item_compare2(ଅଚଳ ଶୂନ୍ୟ *s1, ଅଚଳ ଶୂନ୍ୟ *s2)
{
    sortItem_T  *si1, *si2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->item_compare_partial;
    funcexe_T	funcexe;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    ଯଦି (sortinfo->item_compare_func_err)
	ଫେରନ୍ତୁ 0;

    si1 = (sortItem_T *)s1;
    si2 = (sortItem_T *)s2;

    ଯଦି (partial == ଖାଲି)
	func_name = sortinfo->item_compare_func;
    ଅନ୍ୟଥା
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

    ଯଦି (res == FAIL || did_emsg > did_emsg_before)
	res = ITEM_COMPARE_FAIL;
    ଅନ୍ୟଥା
    {
	varnumber_T	n;

	n = tv_get_ସଂଖ୍ୟା_chk(&rettv, &sortinfo->item_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    ଯଦି (sortinfo->item_compare_func_err)
	res = ITEM_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    ଯଦି (res == 0 && !sortinfo->item_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    ଫେରନ୍ତୁ res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
sort_compute_ଚାବିଗୁଡ଼ିକ(sortItem_T *ptrs, long len, sortinfo_T *info)
{
    long	i;

    ଯଦି (info->item_compare_numbers)
    {
	ପାଇଁ (i = 0; i < len; ++i)
	    ptrs[i].ଚାବି.inum = tv_to_ସଂଖ୍ୟା(&ptrs[i].item->li_tv);
    }
    ଅନ୍ୟଥା ଯଦି (info->item_compare_float)
    {
	ପାଇଁ (i = 0; i < len; ++i)
	    ptrs[i].ଚାବି.fnum = tv_get_float(&ptrs[i].item->li_tv);
    }
    ଅନ୍ୟଥା // info->item_compare_numeric
    {
	ପାଇଁ (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].item->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    ଯଦି (tv->v_type == VAR_STRING)
		ptrs[i].ଚାବି.fnum = 0.0;
	    ଅନ୍ୟଥା
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = ଖାଲି;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].ଚାବି.fnum = p == ଖାଲି ? 0.0 : strtod((ଅକ୍ଷର *)p, ଖାଲି);
		vim_free(tofree);
	    }
	}
    }
    info->item_compare_ଚାବିଗୁଡ଼ିକ_ready = TRUE;
}

/*
 * sort() List "l"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_sort(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    ଯଦି (ptrs == ଖାଲି)
	ଫେରନ୍ତୁ;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(l, li)
    {
	ptrs[i].item = li;
	ptrs[i].idx = i;
	++i;
    }

    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = FALSE;
    info->item_compare_ଚାବିଗୁଡ଼ିକ_ready = FALSE;

    ଯଦି (info->item_compare_func == ଖାଲି && info->item_compare_partial == ଖାଲି
	    && (info->item_compare_numbers || info->item_compare_float
						|| info->item_compare_numeric))
	sort_compute_ଚାବିଗୁଡ଼ିକ(ptrs, len, info);
    // test the compare function
    ଯଦି ((info->item_compare_func != ଖାଲି
		|| info->item_compare_partial != ଖାଲି)
	    && item_compare2((ଶୂନ୍ୟ *)&ptrs[0], (ଶୂନ୍ୟ *)&ptrs[1])
	    == ITEM_COMPARE_FAIL)
	emsg(_(e_sort_compare_function_failed));
    ଅନ୍ୟଥା
    {
	// Sort the array with item pointers.
	qsort((ଶୂନ୍ୟ *)ptrs, (size_t)len, ଆକାର(sortItem_T),
		info->item_compare_func == ଖାଲି
		&& info->item_compare_partial == ଖାଲି
		? item_compare : item_compare2);

	ଯଦି (!info->item_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_first = l->lv_u.mat.lv_last
		= l->lv_u.mat.lv_idx_item = ଖାଲି;
	    l->lv_len = 0;
	    ପାଇଁ (i = 0; i < len; ++i)
		list_append(l, ptrs[i].item);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_uniq(list_T *l, sortinfo_T *info)
{
    long	len;
    sortItem_T	*ptrs;
    long	i = 0;
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	(*item_compare_func_ptr)(ଅଚଳ ଶୂନ୍ୟ *, ଅଚଳ ଶୂନ୍ୟ *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(sortItem_T, len);
    ଯଦି (ptrs == ଖାଲି)
	ଫେରନ୍ତୁ;

    // f_uniq(): ptrs will be a stack of items to remove
    info->item_compare_func_err = FALSE;
    info->item_compare_keep_zero = TRUE;
    item_compare_func_ptr = info->item_compare_func != ଖାଲି
	|| info->item_compare_partial != ଖାଲି
	? item_compare2 : item_compare;

    ପାଇଁ (li = l->lv_first; li != ଖାଲି && li->li_next != ଖାଲି;
	    li = li->li_next)
    {
	ଯଦି (item_compare_func_ptr((ଶୂନ୍ୟ *)&li, (ଶୂନ୍ୟ *)&li->li_next)
		== 0)
	    ptrs[i++].item = li;
	ଯଦି (info->item_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    ଭାଙ୍ଗ;
	}
    }

    ଯଦି (!info->item_compare_func_err)
    {
	ଯେତେବେଳେ (--i >= 0)
	{
	    li = ptrs[i].item->li_next;
	    ptrs[i].item->li_next = li->li_next;
	    ଯଦି (li->li_next != ଖାଲି)
		li->li_next->li_prev = ptrs[i].item;
	    ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
parse_sort_uniq_args(typval_T *argvars, sortinfo_T *info)
{
    info->item_compare_ic = FALSE;
    info->item_compare_lc = FALSE;
    info->item_compare_numeric = FALSE;
    info->item_compare_numbers = FALSE;
    info->item_compare_float = FALSE;
    info->item_compare_func = ଖାଲି;
    info->item_compare_partial = ଖାଲି;
    info->item_compare_selfdict = ଖାଲି;
    info->item_compare_ଚାବିଗୁଡ଼ିକ_ready = FALSE;

    ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
	ଫେରନ୍ତୁ OK;

    // optional second argument: {func}
    ଯଦି (argvars[1].v_type == VAR_FUNC)
	info->item_compare_func = argvars[1].vval.v_string;
    ଅନ୍ୟଥା ଯଦି (argvars[1].v_type == VAR_PARTIAL)
	info->item_compare_partial = argvars[1].vval.v_partial;
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    error = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nr = 0;

	ଯଦି (argvars[1].v_type == VAR_ସଂଖ୍ୟା)
	{
	    nr = tv_get_ସଂଖ୍ୟା_chk(&argvars[1], &error);
	    ଯଦି (error)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (nr == 1)
		info->item_compare_ic = TRUE;
	}
	ଯଦି (nr != 1)
	{
	    ଯଦି (argvars[1].v_type != VAR_ସଂଖ୍ୟା)
		info->item_compare_func = tv_get_string(&argvars[1]);
	    ଅନ୍ୟଥା ଯଦି (nr != 0)
	    {
		emsg(_(e_invalid_argument));
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଯଦି (info->item_compare_func != ଖାଲି)
	{
	    ଯଦି (*info->item_compare_func == NUL)
	    {
		// empty string means default sort
		info->item_compare_func = ଖାଲି;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(info->item_compare_func, "n") == 0)
	    {
		info->item_compare_func = ଖାଲି;
		info->item_compare_numeric = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(info->item_compare_func, "N") == 0)
	    {
		info->item_compare_func = ଖାଲି;
		info->item_compare_numbers = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(info->item_compare_func, "f") == 0)
	    {
		info->item_compare_func = ଖାଲି;
		info->item_compare_float = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(info->item_compare_func, "i") == 0)
	    {
		info->item_compare_func = ଖାଲି;
		info->item_compare_ic = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (STRCMP(info->item_compare_func, "l") == 0)
	    {
		info->item_compare_func = ଖାଲି;
		info->item_compare_lc = TRUE;
	    }
	}
    }

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	ଯଦି (check_for_dict_arg(argvars, 2) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	info->item_compare_selfdict = argvars[2].vval.v_dict;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * "sort()" or "uniq()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_sort_uniq(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sort)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    long	len;

    ଯଦି (in_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_argument_of_str_must_be_list), sort ? "sort()" : "uniq()");
	ଫେରନ୍ତୁ;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    ଯଦି (l != ଖାଲି && ମୂଲ୍ୟ_check_lock(l->lv_lock,
		(char_u *)(sort ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	ଯାଅ theend;
    rettv_list_set(rettv, l);
    ଯଦି (l == ଖାଲି)
	ଯାଅ theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    ଯଦି (len <= 1)
	ଯାଅ theend;	// short list sorts pretty quickly

    ଯଦି (parse_sort_uniq_args(argvars, &info) == FAIL)
	ଯାଅ theend;

    ଯଦି (sort)
	do_sort(l, &info);
    ଅନ୍ୟଥା
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    ଶୂନ୍ୟ
f_sort(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    ଶୂନ୍ୟ
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_sort_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*ପଦସମଷ୍ଟି,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    ଯଦି (filtermap == FILTERMAP_FOREACH && ପଦସମଷ୍ଟି->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(ପଦସମଷ୍ଟି->vval.v_string);
	ଯଦି (!did_emsg)
	    retval = OK;
	ଯାଅ theend;
    }

    argv[0] = *get_vim_var_tv(VV_ଚାବି);
    argv[1] = *get_vim_var_tv(VV_VAL);
    ଯଦି (eval_ପଦସମଷ୍ଟି_typval(ପଦସମଷ୍ଟି, FALSE, argv, 2, fc, newtv) == FAIL)
	ଯାଅ theend;
    ଯଦି (filtermap == FILTERMAP_FILTER)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    error = FALSE;

	// filter(): when expr is zero remove the item
	ଯଦି (in_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	ଅନ୍ୟଥା
	    *remp = (tv_get_ସଂଖ୍ୟା_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	ଯଦି (error)
	    ଯାଅ theend;
    }
    ଅନ୍ୟଥା ଯଦି (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    ଫେରନ୍ତୁ retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	ଅକ୍ଷର		*func_name,
	char_u		*arg_errmsg,
	typval_T	*ପଦସମଷ୍ଟି,
	typval_T	*rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_lock;
    list_T	*l_ret = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    ଯଦି (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = ଖାଲି;
    }
    ଯଦି (l == ଖାଲି || (filtermap == FILTERMAP_FILTER
			    && ମୂଲ୍ୟ_check_lock(l->lv_lock, arg_errmsg, TRUE)))
	ଫେରନ୍ତୁ;

    prev_lock = l->lv_lock;

    ଯଦି (filtermap == FILTERMAP_MAPNEW)
    {
	ଯଦି (rettv_list_alloc(rettv) == FAIL)
	    ଫେରନ୍ତୁ;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ଚାବି, VAR_ସଂଖ୍ୟା);

    ଯଦି (l->lv_lock == 0)
	l->lv_lock = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ପଦସମଷ୍ଟି_get_funccal(ପଦସମଷ୍ଟି, &newtv);

    ଯଦି (l->lv_first == &range_list_item)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_start;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = l->lv_len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	ଯଦି (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_first = ଖାଲି;
	    l->lv_u.mat.lv_last = ଖାଲି;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_item = ଖାଲି;
	}

	ପାଇଁ (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_ସଂଖ୍ୟା;
	    tv.v_lock = 0;
	    tv.vval.v_ସଂଖ୍ୟା = val;
	    set_vim_var_nr(VV_ଚାବି, idx);
	    ଯଦି (filter_map_one(&tv, ପଦସମଷ୍ଟି, filtermap, fc, &newtv, &rem) == FAIL)
		ଭାଙ୍ଗ;
	    ଯଦି (did_emsg)
	    {
		clear_tv(&newtv);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (filtermap != FILTERMAP_FOREACH)
	    {
		ଯଦି (filtermap != FILTERMAP_FILTER)
		{
		    ଯଦି (filtermap == FILTERMAP_MAP && argtype != ଖାଲି
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			ଭାଙ୍ଗ;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    ଯଦି (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା ଯଦି (!rem)
		{
		    // filter(): append the list item value when not rem
		    ଯଦି (list_append_tv_move(l, &tv) == FAIL)
			ଭାଙ୍ଗ;
		}
	    }

	    val += stride;
	}
    }
    ଅନ୍ୟଥା
    {
	// Materialized list: loop over the items
	ପାଇଁ (li = l->lv_first; li != ଖାଲି; li = nli)
	{
	    ଯଦି (filtermap == FILTERMAP_MAP && ମୂଲ୍ୟ_check_lock(
			li->li_tv.v_lock, arg_errmsg, TRUE))
		ଭାଙ୍ଗ;
	    nli = li->li_next;
	    set_vim_var_nr(VV_ଚାବି, idx);
	    ଯଦି (filter_map_one(&li->li_tv, ପଦସମଷ୍ଟି, filtermap, fc,
							 &newtv, &rem) == FAIL)
		ଭାଙ୍ଗ;
	    ଯଦି (did_emsg)
	    {
		clear_tv(&newtv);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (filtermap == FILTERMAP_MAP)
	    {
		ଯଦି (argtype != ଖାଲି && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ଭାଙ୍ଗ;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_lock = 0;
		li->li_tv = newtv;
	    }
	    ଅନ୍ୟଥା ଯଦି (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		ଯଦି (list_append_tv_move(l_ret, &newtv) == FAIL)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_lock = prev_lock;
    ଯଦି (fc != ଖାଲି)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*ପଦସମଷ୍ଟି;
    ଅକ୍ଷର	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_did_emsg;
    type_T	*type = ଖାଲି;

    // map(), filter(), foreach() return the first argument, also on failure.
    ଯଦି (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    ଯଦି (in_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (filtermap == FILTERMAP_MAP && in_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	ଯଦି (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != ଖାଲି)
	    type = argvars[0].vval.v_dict->dv_type;
	ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != ଖାଲି)
	    type = argvars[0].vval.v_list->lv_type;
    }

    ଯଦି (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	ଅକ୍ଷର *ସନ୍ଦେଶ;

	ଯଦି (filtermap == FILTERMAP_FOREACH)
	    ସନ୍ଦେଶ = e_argument_of_str_must_be_list_tuple_string_dictionary_or_blob;
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ = e_argument_of_str_must_be_list_string_dictionary_or_blob;
	semsg(_(ସନ୍ଦେଶ), func_name);
	ଫେରନ୍ତୁ;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    ପଦସମଷ୍ଟି = &argvars[1];
    ଯଦି (ପଦସମଷ୍ଟି->v_type == VAR_UNKNOWN)
	ଫେରନ୍ତୁ;

    typval_T	save_val;
    typval_T	save_ଚାବି;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_ଚାବି, &save_ଚାବି);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ଯଦି (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, ପଦସମଷ୍ଟି, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, ପଦସମଷ୍ଟି,
							    arg_errmsg, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, ପଦସମଷ୍ଟି, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, ପଦସମଷ୍ଟି);
    ଅନ୍ୟଥା // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, ପଦସମଷ୍ଟି, rettv);

    restore_vimvar(VV_ଚାବି, &save_ଚାବି);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    ଶୂନ୍ୟ
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    ଶୂନ୍ୟ
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    ଶୂନ୍ୟ
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    ଶୂନ୍ୟ
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_add(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    ଯଦି (l == ଖାଲି)
    {
	ଯଦି (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
    }
    ଅନ୍ୟଥା ଯଦି (!ମୂଲ୍ୟ_check_lock(l->lv_lock,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    ଶୂନ୍ୟ
f_add(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = 1; // Default: Failed

    ଯଦି (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_LIST)
	list_add(argvars, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_BLOB)
	blob_add(argvars, rettv);
    ଅନ୍ୟଥା
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ସ୍ଥିର long
list_ଗଣନା(list_T *l, typval_T *needle, long idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ic)
{
    long	n = 0;
    listitem_T	*li;

    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    CHECK_LIST_MATERIALIZE(l);

    ଯଦି (list_len(l) == 0)
	ଫେରନ୍ତୁ 0;

    li = list_find(l, idx);
    ଯଦି (li == ଖାଲି)
    {
	semsg(_(e_list_index_out_of_range_nr), idx);
	ଫେରନ୍ତୁ 0;
    }

    ପାଇଁ ( ; li != ଖାଲି; li = li->li_next)
	ଯଦି (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    ଫେରନ୍ତୁ n;
}

/*
 * "count()" function
 */
    ଶୂନ୍ୟ
f_ଗଣନା(typval_T *argvars, typval_T *rettv)
{
    long	n = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 3) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	ic = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_bool_chk(&argvars[2], &error);

    ଯଦି (!error && argvars[0].v_type == VAR_STRING)
	n = string_ଗଣନା(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    ଅନ୍ୟଥା ଯଦି (!error && argvars[0].v_type == VAR_LIST)
    {
	long idx = 0;

	ଯଦି (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[3], &error);
	ଯଦି (!error)
	    n = list_ଗଣନା(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    ଅନ୍ୟଥା ଯଦି (!error && argvars[0].v_type == VAR_TUPLE)
    {
	long idx = 0;

	ଯଦି (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[3], &error);
	ଯଦି (!error)
	    n = tuple_ଗଣନା(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    ଅନ୍ୟଥା ଯଦି (!error && argvars[0].v_type == VAR_DICT)
    {
	ଯଦି (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_argument));
	ଅନ୍ୟଥା
	    n = dict_ଗଣନା(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    ଅନ୍ୟଥା ଯଦି (!error)
	semsg(_(e_argument_of_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_ସଂଖ୍ୟା = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	ଅକ୍ଷର		*func_name,
	char_u		*arg_errmsg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*item;
    long	before;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    l1 = argvars[0].vval.v_list;
    ଯଦି (l1 == ଖାଲି)
    {
	emsg(_(e_cannot_extend_null_list));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (is_new || !ମୂଲ୍ୟ_check_lock(l1->lv_lock, arg_errmsg, TRUE))
    {
	ଯଦି (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    ଯଦି (l1 == ଖାଲି)
		ଫେରନ୍ତୁ;
	}

	l2 = argvars[1].vval.v_list;
	ଯଦି (l2 == ଖାଲି)
	    ଯାଅ theend;

	ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
	    ଯଦି (error)
		ଯାଅ cleanup;		// type error; errmsg already given
	    ଯଦି (before == l1->lv_len)
		item = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		item = list_find(l1, before);
		ଯଦି (item == ଖାଲି)
		{
		    semsg(_(e_list_index_out_of_range_nr), before);
		    ଯାଅ cleanup;
		}
	    }
	}
	ଅନ୍ୟଥା
	    item = ଖାଲି;
	ଯଦି (type != ଖାଲି && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    ଯାଅ cleanup;
	list_extend(l1, l2, item);

theend:
	ଯଦି (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_lock = FALSE;
	}
	ଅନ୍ୟଥା
	    copy_tv(&argvars[0], rettv);
	ଫେରନ୍ତୁ;

cleanup:
	ଯଦି (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_new)
{
    type_T	*type = ଖାଲି;
    ଅକ୍ଷର	*func_name = is_new ? "extendnew()" : "extend()";

    ଯଦି (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	ଯଦି (!is_new && in_vim9script() && argvars[0].vval.v_list != ଖାଲି)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	ଯଦି (!is_new && in_vim9script() && argvars[0].vval.v_dict != ଖାଲି)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    ଅନ୍ୟଥା
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
list_ଭର୍ତ୍ତି_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    long	before = 0;
    listitem_T	*item;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

    ଯଦି (l == ଖାଲି)
    {
	ଯଦି (in_vim9script())
	    emsg(_(e_cannot_add_to_null_list));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (ମୂଲ୍ୟ_check_lock(l->lv_lock, (char_u *)N_("insert() argument"), TRUE))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	before = (long)tv_get_ସଂଖ୍ୟା_chk(&argvars[2], &error);
    ଯଦି (error)
	ଫେରନ୍ତୁ;		// type error; errmsg already given

    ଯଦି (before == l->lv_len)
	item = ଖାଲି;
    ଅନ୍ୟଥା
    {
	item = list_find(l, before);
	ଯଦି (item == ଖାଲି)
	{
	    semsg(_(e_list_index_out_of_range_nr), before);
	    l = ଖାଲି;
	}
    }
    ଯଦି (l != ଖାଲି)
    {
	(ଶୂନ୍ୟ)list_ଭର୍ତ୍ତି_tv(l, &argvars[1], item);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    ଶୂନ୍ୟ
f_ଭର୍ତ୍ତି(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_BLOB)
	blob_ଭର୍ତ୍ତି_func(argvars, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type != VAR_LIST)
	semsg(_(e_argument_of_str_must_be_list_or_blob), "insert()");
    ଅନ୍ୟଥା
	list_ଭର୍ତ୍ତି_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    ଶୂନ୍ୟ
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    ଯଦି (in_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
			|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    ଅନ୍ୟଥା
	semsg(_(e_argument_of_str_must_be_list_dictionary_or_blob), "remove()");
}

    ସ୍ଥିର ଶୂନ୍ୟ
list_reverse(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_set(rettv, l);
    ଯଦି (l != ଖାଲି
	    && !ମୂଲ୍ୟ_check_lock(l->lv_lock,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	ଯଦି (l->lv_first == &range_list_item)
	{
	    varnumber_T new_start = l->lv_u.nonmat.lv_start
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_end = new_start
		- (l->lv_u.nonmat.lv_end - l->lv_u.nonmat.lv_start);
	    l->lv_u.nonmat.lv_start = new_start;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    ଫେରନ୍ତୁ;
	}
	li = l->lv_u.mat.lv_last;
	l->lv_first = l->lv_u.mat.lv_last = ଖାଲି;
	l->lv_len = 0;
	ଯେତେବେଳେ (li != ଖାଲି)
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
    ଶୂନ୍ୟ
f_reverse(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_BLOB)
	blob_reverse(argvars[0].vval.v_blob, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	ଯଦି (argvars[0].vval.v_string != ଖାଲି)
	    rettv->vval.v_string = reverse_text(argvars[0].vval.v_string);
	ଅନ୍ୟଥା
	    rettv->vval.v_string = ଖାଲି;
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_LIST)
	list_reverse(argvars[0].vval.v_list, rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_TUPLE)
	tuple_reverse(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_reduce(
	typval_T	*argvars,
	typval_T	*ପଦସମଷ୍ଟି,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		range_list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_start = called_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != ଖାଲି && l->lv_first == &range_list_item;
    ଯଦି (range_list)
	range_val = l->lv_u.nonmat.lv_start;

    ଯଦି (argvars[2].v_type == VAR_UNKNOWN)
    {
	ଯଦି (l == ଖାଲି || l->lv_len == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_ମୂଲ୍ୟ), "List");
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (range_list)
	{
	    initial.v_type = VAR_ସଂଖ୍ୟା;
	    initial.vval.v_ସଂଖ୍ୟା = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	ଅନ୍ୟଥା
	{
	    initial = l->lv_first->li_tv;
	    li = l->lv_first->li_next;
	}
    }
    ଅନ୍ୟଥା
    {
	initial = argvars[2];
	ଯଦି (l != ଖାଲି && !range_list)
	    li = l->lv_first;
    }
    copy_tv(&initial, rettv);

    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ପଦସମଷ୍ଟି_get_funccal(ପଦସମଷ୍ଟି, rettv);

    prev_locked = l->lv_lock;
    l->lv_lock = VAR_FIXED;  // disallow the list changing here

    ଯେତେବେଳେ (range_list ? range_idx < l->lv_len : li != ଖାଲି)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	ଯଦି (range_list)
	{
	    argv[1].v_type = VAR_ସଂଖ୍ୟା;
	    argv[1].vval.v_ସଂଖ୍ୟା = range_val;
	}
	ଅନ୍ୟଥା
	    argv[1] = li->li_tv;

	r = eval_ପଦସମଷ୍ଟି_typval(ପଦସମଷ୍ଟି, TRUE, argv, 2, fc, rettv);

	ଯଦି (argv[0].v_type != VAR_ସଂଖ୍ୟା && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	ଯଦି (r == FAIL || called_emsg != called_emsg_start)
	    ଭାଙ୍ଗ;

	// advance to the next item
	ଯଦି (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	ଅନ୍ୟଥା
	    li = li->li_next;
    }

    ଯଦି (fc != ଖାଲି)
	remove_funccal();

    l->lv_lock = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    ଶୂନ୍ୟ
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    ଯଦି (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    ଅନ୍ୟଥା ଯଦି (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    ଅନ୍ୟଥା
	func_name = tv_get_string_strict(&argvars[1]);
    ଯଦି (func_name == ଖାଲି || *func_name == NUL)
    {
	emsg(_(e_missing_function_argument));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    ଅନ୍ୟଥା
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    ଶୂନ୍ୟ
f_slice(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (check_can_index(&argvars[0], TRUE, FALSE) != OK)
	ଫେରନ୍ତୁ;

    copy_tv(argvars, rettv);
    eval_index_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? ଖାଲି : argvars + 2,
	    TRUE, ଖାଲି, 0, FALSE);
}

#endif // defined(FEAT_EVAL)
