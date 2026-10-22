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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

// List heads for garbage collection.
ಸ್ಥಿರ list_T		*ಮೊದಲ_list = ಖಾಲಿ;	// list of all lists

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_WATCHERS(l, lw) \
    ಗಾಗಿ ((lw) = (l)->lv_watch; (lw) != ಖಾಲಿ; (lw) = (lw)->lw_ಮುಂದಿನ)

ಸ್ಥಿರ ಶೂನ್ಯ list_free_ಅಂಶ(list_T *l, listitem_T *ಅಂಶ);

/*
 * Add a watcher to a list.
 */
    ಶೂನ್ಯ
list_ಕೂಡಿಸು_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_ಮುಂದಿನ = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    ಶೂನ್ಯ
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	ಆದರೆ (lw == lwrem)
	{
	    *lwp = lw->lw_ಮುಂದಿನ;
	    ನಿಲ್ಲಿಸು;
	}
	lwp = &lw->lw_ಮುಂದಿನ;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_fix_watch(list_T *l, listitem_T *ಅಂಶ)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	ಆದರೆ (lw->lw_ಅಂಶ == ಅಂಶ)
	    lw->lw_ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    ಆದರೆ (ಮೊದಲ_list != ಖಾಲಿ)
	ಮೊದಲ_list->lv_ಬಳಸಿದ_prev = l;
    l->lv_ಬಳಸಿದ_prev = ಖಾಲಿ;
    l->lv_ಬಳಸಿದ_ಮುಂದಿನ = ಮೊದಲ_list;
    ಮೊದಲ_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(ಶೂನ್ಯ)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    ಆದರೆ (l != ಖಾಲಿ)
	list_init(l);
    ಹಿಂತಿರುಗಿ l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (alloc_fail_id == id && alloc_does_fail(ಗಾತ್ರ(list_T)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    list_T	*l;

    ಆದರೆ (ಎಣಿಕೆ_2 > 0
	    && (size_t)ಎಣಿಕೆ_2 > (ಗಾತ್ರ_MAX - ಗಾತ್ರ(list_T)) / ಗಾತ್ರ(listitem_T))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    l = (list_T *)alloc_clear(ಗಾತ್ರ(list_T) + ಎಣಿಕೆ_2 * ಗಾತ್ರ(listitem_T));
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    list_init(l);

    ಆದರೆ (ಎಣಿಕೆ_2 <= 0)
	ಹಿಂತಿರುಗಿ l;

    listitem_T	*li = (listitem_T *)(l + 1);
    ಪೂರ್ಣಾಂಕ		i;

    l->lv_len = ಎಣಿಕೆ_2;
    l->lv_with_ವಸ್ತುಗಳು = ಎಣಿಕೆ_2;
    l->lv_ಮೊದಲ = li;
    l->lv_u.mat.lv_ಕೊನೆಯ = li + ಎಣಿಕೆ_2 - 1;
    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	ಆದರೆ (i == 0)
	    li->li_prev = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    li->li_prev = li - 1;
	ಆದರೆ (i == ಎಣಿಕೆ_2 - 1)
	    li->li_ಮುಂದಿನ = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    li->li_ಮುಂದಿನ = li + 1;
	++li;
    }

    ಹಿಂತಿರುಗಿ l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    ಶೂನ್ಯ
list_ಗಣ_ಅಂಶ(list_T *l, ಪೂರ್ಣಾಂಕ idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    rettv->v_ಬೀಗ = 0;
    rettv_list_ಗಣ(rettv, l);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    ಪೂರ್ಣಾಂಕ
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (alloc_fail_id == id && alloc_does_fail(ಗಾತ್ರ(list_T)))
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    ಶೂನ್ಯ
rettv_list_ಗಣ(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ಆದರೆ (l != ಖಾಲಿ)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    ಶೂನ್ಯ
list_unref(list_T *l)
{
    ಆದರೆ (l != ಖಾಲಿ && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_free_contents(list_T *l)
{
    listitem_T *ಅಂಶ;

    ಆದರೆ (l->lv_ಮೊದಲ != &range_list_ಅಂಶ)
	ಗಾಗಿ (ಅಂಶ = l->lv_ಮೊದಲ; ಅಂಶ != ಖಾಲಿ; ಅಂಶ = l->lv_ಮೊದಲ)
	{
	    // Remove the item before deleting it.
	    l->lv_ಮೊದಲ = ಅಂಶ->li_ಮುಂದಿನ;
	    clear_tv(&ಅಂಶ->li_tv);
	    list_free_ಅಂಶ(l, ಅಂಶ);
	}
}

/*
 * Go through the list of lists and free items without the copyID.
 * But don't free a list that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    ಪೂರ್ಣಾಂಕ
list_free_nonref(ಪೂರ್ಣಾಂಕ copyID)
{
    list_T	*ll;
    ಪೂರ್ಣಾಂಕ		did_free = FALSE;

    ಗಾಗಿ (ll = ಮೊದಲ_list; ll != ಖಾಲಿ; ll = ll->lv_ಬಳಸಿದ_ಮುಂದಿನ)
	ಆದರೆ ((ll->lv_copyID & COPYID_ಮುಖವಾಡ) != (copyID & COPYID_ಮುಖವಾಡ)
						      && ll->lv_watch == ಖಾಲಿ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    ಹಿಂತಿರುಗಿ did_free;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    ಆದರೆ (l->lv_ಬಳಸಿದ_prev == ಖಾಲಿ)
	ಮೊದಲ_list = l->lv_ಬಳಸಿದ_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	l->lv_ಬಳಸಿದ_prev->lv_ಬಳಸಿದ_ಮುಂದಿನ = l->lv_ಬಳಸಿದ_ಮುಂದಿನ;
    ಆದರೆ (l->lv_ಬಳಸಿದ_ಮುಂದಿನ != ಖಾಲಿ)
	l->lv_ಬಳಸಿದ_ಮುಂದಿನ->lv_ಬಳಸಿದ_prev = l->lv_ಬಳಸಿದ_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    ಶೂನ್ಯ
list_free_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಂಕ copyID)
{
    list_T	*ll, *ll_ಮುಂದಿನ;

    ಗಾಗಿ (ll = ಮೊದಲ_list; ll != ಖಾಲಿ; ll = ll_ಮುಂದಿನ)
    {
	ll_ಮುಂದಿನ = ll->lv_ಬಳಸಿದ_ಮುಂದಿನ;
	ಆದರೆ ((ll->lv_copyID & COPYID_ಮುಖವಾಡ) != (copyID & COPYID_ಮುಖವಾಡ)
						      && ll->lv_watch == ಖಾಲಿ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    ಶೂನ್ಯ
list_free(list_T *l)
{
    ಆದರೆ (ಒಳ_free_unref_ವಸ್ತುಗಳು)
	ಹಿಂತಿರುಗಿ;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_free_ಅಂಶ(list_T *l, listitem_T *ಅಂಶ)
{
    ಆದರೆ (l->lv_with_ವಸ್ತುಗಳು == 0 || ಅಂಶ < (listitem_T *)l
			   || ಅಂಶ >= (listitem_T *)(l + 1) + l->lv_with_ವಸ್ತುಗಳು)
	vim_free(ಅಂಶ);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    ಶೂನ್ಯ
listitem_free(list_T *l, listitem_T *ಅಂಶ)
{
    clear_tv(&ಅಂಶ->li_tv);
    list_free_ಅಂಶ(l, ಅಂಶ);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    ಶೂನ್ಯ
listitem_remove(list_T *l, listitem_T *ಅಂಶ)
{
    vimlist_remove(l, ಅಂಶ, ಅಂಶ);
    listitem_free(l, ಅಂಶ);
}

/*
 * Get the number of items in a list.
 */
    ದೀರ್ಘ
list_len(list_T *l)
{
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0L;
    ಹಿಂತಿರುಗಿ l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    ಪೂರ್ಣಾಂಕ
list_equal(
    list_T	*l1,
    list_T	*l2,
    ಪೂರ್ಣಾಂಕ		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    ಆದರೆ (l1 == l2)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (list_len(l1) != list_len(l2))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (list_len(l1) == 0)
	// empty and NULL list are considered equal
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (l1 == ಖಾಲಿ || l2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    ಗಾಗಿ (item1 = l1->lv_ಮೊದಲ, item2 = l2->lv_ಮೊದಲ;
	    item1 != ಖಾಲಿ && item2 != ಖಾಲಿ;
			       item1 = item1->li_ಮುಂದಿನ, item2 = item2->li_ಮುಂದಿನ)
	ಆದರೆ (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ item1 == ಖಾಲಿ && item2 == ಖಾಲಿ;
}

/*
 * Locate item with index "n" in list "l" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    listitem_T *
list_ಕಂಡುಹಿಡಿ(list_T *l, ದೀರ್ಘ n)
{
    listitem_T	*ಅಂಶ;
    ದೀರ್ಘ	idx;

    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Negative index is relative to the end.
    ಆದರೆ (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    ಆದರೆ (n < 0 || n >= l->lv_len)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    ಆದರೆ (n >= l->lv_len)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // When there is a cached index may start search from there.
    ಆದರೆ (l->lv_u.mat.lv_idx_ಅಂಶ != ಖಾಲಿ)
    {
	ಆದರೆ (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    ಅಂಶ = l->lv_ಮೊದಲ;
	    idx = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    ಅಂಶ = l->lv_u.mat.lv_ಕೊನೆಯ;
	    idx = l->lv_len - 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // closest to the cached index
	    ಅಂಶ = l->lv_u.mat.lv_idx_ಅಂಶ;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    ಅಂಶ = l->lv_ಮೊದಲ;
	    idx = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // closest to the end of the list
	    ಅಂಶ = l->lv_u.mat.lv_ಕೊನೆಯ;
	    idx = l->lv_len - 1;
	}
    }

    ಆಗ (n > idx)
    {
	// search forward
	ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ;
	++idx;
    }
    ಆಗ (n < idx)
    {
	// search backward
	ಅಂಶ = ಅಂಶ->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_ಅಂಶ = ಅಂಶ;

    ಹಿಂತಿರುಗಿ ಅಂಶ;
}

/*
 * Get list item "l[idx]" as a number.
 */
    ದೀರ್ಘ
list_ಕಂಡುಹಿಡಿ_nr(
    list_T	*l,
    ದೀರ್ಘ	idx,
    ಪೂರ್ಣಾಂಕ		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    ಆದರೆ (l != ಖಾಲಿ && l->lv_ಮೊದಲ == &range_list_ಅಂಶ)
    {
	ದೀರ್ಘ	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	ಆದರೆ (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	ಆದರೆ (n < 0 || n >= l->lv_len)
	{
	    ಆದರೆ (errorp != ಖಾಲಿ)
		*errorp = TRUE;
	    ಹಿಂತಿರುಗಿ -1L;
	}

	ಹಿಂತಿರುಗಿ l->lv_u.nonmat.lv_ಆರಂಭ + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_ಕಂಡುಹಿಡಿ(l, idx);
    ಆದರೆ (li == ಖಾಲಿ)
    {
	ಆದರೆ (errorp != ಖಾಲಿ)
	    *errorp = TRUE;
	ಹಿಂತಿರುಗಿ -1L;
    }
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_ಕಂಡುಹಿಡಿ_str(list_T *l, ದೀರ್ಘ idx)
{
    listitem_T	*li;

    li = list_ಕಂಡುಹಿಡಿ(l, idx - 1);
    ಆದರೆ (li == ಖಾಲಿ)
    {
	semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), idx);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ(list_T *l, ದೀರ್ಘ *idx)
{
    listitem_T *li = list_ಕಂಡುಹಿಡಿ(l, *idx);

    ಆದರೆ (li != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ li;

    ಆದರೆ (*idx < 0)
    {
	*idx = 0;
	li = list_ಕಂಡುಹಿಡಿ(l, *idx);
    }
    ಹಿಂತಿರುಗಿ li;
}

/*
 * Locate "item" list "l" and return its index.
 * Returns -1 when "item" is not in the list.
 */
    ದೀರ್ಘ
list_idx_ದ_ಅಂಶ(list_T *l, listitem_T *ಅಂಶ)
{
    ದೀರ್ಘ	idx = 0;
    listitem_T	*li;

    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    ಗಾಗಿ (li = l->lv_ಮೊದಲ; li != ಖಾಲಿ && li != ಅಂಶ; li = li->li_ಮುಂದಿನ)
	++idx;
    ಆದರೆ (li == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    ಶೂನ್ಯ
list_append(list_T *l, listitem_T *ಅಂಶ)
{
    CHECK_LIST_MATERIALIZE(l);
    ಆದರೆ (l->lv_u.mat.lv_ಕೊನೆಯ == ಖಾಲಿ)
    {
	// empty list
	l->lv_ಮೊದಲ = ಅಂಶ;
	ಅಂಶ->li_prev = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	l->lv_u.mat.lv_ಕೊನೆಯ->li_ಮುಂದಿನ = ಅಂಶ;
	ಅಂಶ->li_prev = l->lv_u.mat.lv_ಕೊನೆಯ;
    }
    l->lv_u.mat.lv_ಕೊನೆಯ = ಅಂಶ;
    ++l->lv_len;
    ಅಂಶ->li_ಮುಂದಿನ = ಖಾಲಿ;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    ಪೂರ್ಣಾಂಕ
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    ಆದರೆ (l->lv_type != ಖಾಲಿ && l->lv_type->tt_member != ಖಾಲಿ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ಖಾಲಿ, 0) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    li = listitem_alloc();
    ಆದರೆ (li == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    ಆದರೆ (li == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    ಆದರೆ (li == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_ಬೀಗ = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    ಆದರೆ (li == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_ಬೀಗ = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
list_append_string(list_T *l, char_u *str, ಪೂರ್ಣಾಂಕ len)
{
    listitem_T *li = listitem_alloc();

    ಆದರೆ (li == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_ಬೀಗ = 0;
    ಆದರೆ (str == ಖಾಲಿ)
	li->li_tv.vval.v_string = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
list_append_ಸಂಖ್ಯೆ(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    ಆದರೆ (li == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    li->li_tv.v_type = VAR_ಸಂಖ್ಯೆ;
    li->li_tv.v_ಬೀಗ = 0;
    li->li_tv.vval.v_ಸಂಖ್ಯೆ = n;
    list_append(l, li);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    ಪೂರ್ಣಾಂಕ
list_ಸೇರಿಸು_tv(list_T *l, typval_T *tv, listitem_T *ಅಂಶ)
{
    listitem_T	*ni;

    ಆದರೆ (l->lv_type != ಖಾಲಿ && l->lv_type->tt_member != ಖಾಲಿ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ಖಾಲಿ, 0) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ni = listitem_alloc();
    ಆದರೆ (ni == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    copy_tv(tv, &ni->li_tv);
    list_ಸೇರಿಸು(l, ni, ಅಂಶ);
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
list_ಸೇರಿಸು(list_T *l, listitem_T *ni, listitem_T *ಅಂಶ)
{
    CHECK_LIST_MATERIALIZE(l);
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	// Append new item at end of list.
	list_append(l, ni);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Insert new item before existing item.
	ni->li_prev = ಅಂಶ->li_prev;
	ni->li_ಮುಂದಿನ = ಅಂಶ;
	ಆದರೆ (ಅಂಶ->li_prev == ಖಾಲಿ)
	{
	    l->lv_ಮೊದಲ = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಅಂಶ->li_prev->li_ಮುಂದಿನ = ni;
	    l->lv_u.mat.lv_idx_ಅಂಶ = ಖಾಲಿ;
	}
	ಅಂಶ->li_prev = ni;
	++l->lv_len;
    }
}

/*
 * Append a new empty item into "l" based on the list item type.  Used when
 * adding a new element to a List in Vim9script by using the current list
 * length as the index.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
list_append_new_ಅಂಶ(list_T *l)
{
    typval_T	tv;

    ಆದರೆ (l->lv_type != ಖಾಲಿ && l->lv_type->tt_member != ಖಾಲಿ)
	tv.v_type = l->lv_type->tt_member->tt_type;
    ಇಲ್ಲದಿದ್ದರೆ
	tv.v_type = VAR_ಸಂಖ್ಯೆ;
    tv.v_ಬೀಗ = 0;

    ಆಯ್ಕೆ (tv.v_type)
    {
	ಸ್ಥಿತಿ VAR_BOOL: tv.vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_SPECIAL: tv.vval.v_ಸಂಖ್ಯೆ = 0; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ: tv.vval.v_ಸಂಖ್ಯೆ = 0; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FLOAT: tv.vval.v_float = 0; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_STRING: tv.vval.v_string = ಖಾಲಿ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB: tv.vval.v_blob = blob_alloc(); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FUNC: tv.vval.v_string = ಖಾಲಿ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_PARTIAL: tv.vval.v_partial = ಖಾಲಿ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST: tv.vval.v_list = list_alloc(); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT: tv.vval.v_dict = dict_alloc(); ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ: tv.vval.v_ಕೆಲಸ = ಖಾಲಿ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CHANNEL: tv.vval.v_channel = ಖಾಲಿ; ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_CLASS:
	    ಆದರೆ (l->lv_type != ಖಾಲಿ && l->lv_type->tt_member != ಖಾಲಿ)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    ಇಲ್ಲದಿದ್ದರೆ
		tv.vval.v_class = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    ಆದರೆ (l->lv_type != ಖಾಲಿ && l->lv_type->tt_member != ಖಾಲಿ)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    ಇಲ್ಲದಿದ್ದರೆ
		tv.vval.v_object = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TYPEALIAS: tv.vval.v_typealias = ಖಾಲಿ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    tv.v_type = VAR_ಸಂಖ್ಯೆ;
	    tv.vval.v_ಸಂಖ್ಯೆ = 0;
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (l->lv_type != ಖಾಲಿ && l->lv_type->tt_member != ಖಾಲಿ)
	ಗಣ_tv_type(&tv, l->lv_type->tt_member);

    ಹಿಂತಿರುಗಿ list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_ಸೂಚ್ಯಂಕ_one(list_T *l, ದೀರ್ಘ *n1, ಪೂರ್ಣಾಂಕ can_append, ಪೂರ್ಣಾಂಕ quiet)
{
    ದೀರ್ಘ	orig_n1 = *n1;
    listitem_T	*li = list_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ(l, n1);

    ಆದರೆ (li != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ li;

    // Vim9: Allow for adding an item at the end.
    ಆದರೆ (can_append && ಒಳ_vim9script()
	    && *n1 == l->lv_len && l->lv_ಬೀಗ == 0)
    {
	ಆದರೆ (list_append_new_ಅಂಶ(l) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	li = list_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ(l, n1);
    }
    ಆದರೆ (li == ಖಾಲಿ)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), orig_n1);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
check_range_ಸೂಚ್ಯಂಕ_two(
	list_T	    *l,
	ದೀರ್ಘ	    *n1,
	listitem_T  *li1,
	ದೀರ್ಘ	    *n2,
	ಪೂರ್ಣಾಂಕ	    quiet)
{
    ಆದರೆ (*n2 < 0)
    {
	listitem_T	*ni = list_ಕಂಡುಹಿಡಿ(l, *n2);

	ಆದರೆ (ni == ಖಾಲಿ)
	{
	    ಆದರೆ (!quiet)
		semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), *n2);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	*n2 = list_idx_ದ_ಅಂಶ(l, ni);
    }

    // Check that n2 isn't before n1.
    ಆದರೆ (*n1 < 0)
	*n1 = list_idx_ದ_ಅಂಶ(l, li1);
    ಆದರೆ (*n2 < *n1)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), *n2);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
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
    ಪೂರ್ಣಾಂಕ
list_ನಿಯೋಜಿಸು_range(
	list_T	    *dest,
	list_T	    *src,
	ದೀರ್ಘ	    idx1_arg,
	ದೀರ್ಘ	    idx2,
	ಪೂರ್ಣಾಂಕ	    ಖಾಲಿಗಳು_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    ದೀರ್ಘ	idx1 = idx1_arg;
    listitem_T	*ಮೊದಲ_li = list_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ(dest, &idx1);
    ದೀರ್ಘ	idx;
    type_T	*member_type = ಖಾಲಿ;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = ಮೊದಲ_li;
    ಗಾಗಿ (src_li = src->lv_ಮೊದಲ; src_li != ಖಾಲಿ && dest_li != ಖಾಲಿ; )
    {
	ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(dest_li->li_tv.v_ಬೀಗ, varname, FALSE))
	    ಹಿಂತಿರುಗಿ FAIL;
	src_li = src_li->li_ಮುಂದಿನ;
	ಆದರೆ (src_li == ಖಾಲಿ || (!ಖಾಲಿಗಳು_idx2 && idx2 == idx))
	    ನಿಲ್ಲಿಸು;
	dest_li = dest_li->li_ಮುಂದಿನ;
	++idx;
    }

    ಆದರೆ (ಒಳ_vim9script() && dest->lv_type != ಖಾಲಿ
					   && dest->lv_type->tt_member != ಖಾಲಿ)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = ಮೊದಲ_li;
    ಗಾಗಿ (src_li = src->lv_ಮೊದಲ; src_li != ಖಾಲಿ; )
    {
	ಆದರೆ (op != ಖಾಲಿ && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (member_type != ಖಾಲಿ
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      ಖಾಲಿ, 0) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_ಮುಂದಿನ;
	ಆದರೆ (src_li == ಖಾಲಿ || (!ಖಾಲಿಗಳು_idx2 && idx2 == idx))
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (dest_li->li_ಮುಂದಿನ == ಖಾಲಿ)
	{
	    // Need to add an empty item.
	    ಆದರೆ (list_append_ಸಂಖ್ಯೆ(dest, 0) == FAIL)
	    {
		src_li = ಖಾಲಿ;
		ನಿಲ್ಲಿಸು;
	    }
	}
	dest_li = dest_li->li_ಮುಂದಿನ;
	++idx;
    }
    ಆದರೆ (src_li != ಖಾಲಿ)
    {
	emsg(_(e_list_ಮೌಲ್ಯ_has_ಇನ್ನಷ್ಟು_ವಸ್ತುಗಳು_than_targets));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (ಖಾಲಿಗಳು_idx2
	    ? (dest_li != ಖಾಲಿ && dest_li->li_ಮುಂದಿನ != ಖಾಲಿ)
	    : idx != idx2)
    {
	emsg(_(e_list_ಮೌಲ್ಯ_does_not_have_enough_ವಸ್ತುಗಳು));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_flatten(list_T *list, listitem_T *ಮೊದಲ, ದೀರ್ಘ maxitems, ದೀರ್ಘ maxdepth)
{
    listitem_T	*ಅಂಶ;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ = 0;

    ಆದರೆ (maxdepth == 0)
	ಹಿಂತಿರುಗಿ;
    CHECK_LIST_MATERIALIZE(list);
    ಆದರೆ (ಮೊದಲ == ಖಾಲಿ)
	ಅಂಶ = list->lv_ಮೊದಲ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಅಂಶ = ಮೊದಲ;

    ಆಗ (ಅಂಶ != ಖಾಲಿ && ಮುಗಿದ < maxitems)
    {
	listitem_T	*ಮುಂದಿನ = ಅಂಶ->li_ಮುಂದಿನ;

	fast_breakcheck();
	ಆದರೆ (got_int)
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (ಅಂಶ->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = ಅಂಶ->li_tv.vval.v_list;

	    vimlist_remove(list, ಅಂಶ, ಅಂಶ);
	    ಆದರೆ (list_extend(list, itemlist, ಮುಂದಿನ) == FAIL)
	    {
		list_free_ಅಂಶ(list, ಅಂಶ);
		ಹಿಂತಿರುಗಿ;
	    }

	    ಆದರೆ (maxdepth > 0)
		list_flatten(list, ಅಂಶ->li_prev == ಖಾಲಿ
				     ? list->lv_ಮೊದಲ : ಅಂಶ->li_prev->li_ಮುಂದಿನ,
				itemlist->lv_len, maxdepth - 1);
	    clear_tv(&ಅಂಶ->li_tv);
	    list_free_ಅಂಶ(list, ಅಂಶ);
	}

	++ಮುಗಿದ;
	ಅಂಶ = ಮುಂದಿನ;
    }
}

/*
 * "flatten()" and "flattennew()" functions
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
flatten_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ make_copy)
{
    list_T  *l;
    ದೀರ್ಘ    maxdepth;
    ಪೂರ್ಣಾಂಕ	    error = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_ವಾದ_ದ_str_must_be_list), "flatten()");
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	maxdepth = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
	ಆದರೆ (error)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_ಸಂಖ್ಯೆ));
	    ಹಿಂತಿರುಗಿ;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(l->lv_ಬೀಗ,
				     (char_u *)N_("flatten() argument"), TRUE))
	    ಹಿಂತಿರುಗಿ;
	++l->lv_refcount;
    }

    list_flatten(l, ಖಾಲಿ, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    ಶೂನ್ಯ
f_flatten(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script())
	emsg(_(e_cannot_use_flatten_ಒಳ_vim9_script));
    ಇಲ್ಲದಿದ್ದರೆ
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    ಶೂನ್ಯ
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    ಶೂನ್ಯ
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    ಗಾಗಿ (idx = 0, li = l->lv_ಮೊದಲ; li != ಖಾಲಿ; li = li->li_ಮುಂದಿನ, ++idx)
    {
	list_T	    *l2 = list_alloc();

	ಆದರೆ (l2 == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (list_append_ಸಂಖ್ಯೆ(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    ಶೂನ್ಯ
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // null string behaves like an empty string

    ಗಾಗಿ (idx = 0; *p != NUL; ++idx)
    {
	ಪೂರ್ಣಾಂಕ	    len = mb_ptr2len(p);
	list_T	    *l2;

	ಆದರೆ (len == 0)
	    ನಿಲ್ಲಿಸು;
	l2 = list_alloc();
	ಆದರೆ (l2 == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (list_append_ಸಂಖ್ಯೆ(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    ನಿಲ್ಲಿಸು;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*ಅಂಶ;
    ಪೂರ್ಣಾಂಕ		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    ಆದರೆ (l2 == ಖಾಲಿ || l2->lv_len == 0)
	ಹಿಂತಿರುಗಿ OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == ಖಾಲಿ ? ಖಾಲಿ : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    ಗಾಗಿ (ಅಂಶ = l2->lv_ಮೊದಲ; ಅಂಶ != ಖಾಲಿ && --todo >= 0;
				 ಅಂಶ = ಅಂಶ == bef_prev ? bef : ಅಂಶ->li_ಮುಂದಿನ)
	ಆದರೆ (list_ಸೇರಿಸು_tv(l1, &ಅಂಶ->li_tv, bef) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    ಪೂರ್ಣಾಂಕ		len1 = l1 == ಖಾಲಿ ? 0 : l1->lv_len;
    ಪೂರ್ಣಾಂಕ		len2 = l2 == ಖಾಲಿ ? 0 : l2->lv_len;
    ದೀರ್ಘ	totallen = (ದೀರ್ಘ)len1 + (ದೀರ್ಘ)len2;
    ಪೂರ್ಣಾಂಕ		i;
    listitem_T	*ಅಂಶ;

    ಆದರೆ (totallen == 0)
    {
	l = list_alloc();
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_ಬೀಗ = 0;
	tv->vval.v_list = l;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಆದರೆ (totallen > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
	ಹಿಂತಿರುಗಿ FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_ವಸ್ತುಗಳು((ಪೂರ್ಣಾಂಕ)totallen);
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    i = 0;
    ಆದರೆ (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	ಗಾಗಿ (ಅಂಶ = l1->lv_ಮೊದಲ; ಅಂಶ != ಖಾಲಿ && !got_int;
		ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ)
	{
	    typval_T new_tv;

	    copy_tv(&ಅಂಶ->li_tv, &new_tv);
	    list_ಗಣ_ಅಂಶ(l, i++, &new_tv);
	}
    }
    ಆದರೆ (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	ಗಾಗಿ (ಅಂಶ = l2->lv_ಮೊದಲ; ಅಂಶ != ಖಾಲಿ && !got_int;
		ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ)
	{
	    typval_T new_tv;

	    copy_tv(&ಅಂಶ->li_tv, &new_tv);
	    list_ಗಣ_ಅಂಶ(l, i++, &new_tv);
	}
    }

    ++l->lv_refcount;
    tv->v_type = VAR_LIST;
    tv->v_ಬೀಗ = 0;
    tv->vval.v_list = l;
    ಹಿಂತಿರುಗಿ OK;
}

    list_T *
list_ತುಂಡು(list_T *ol, ದೀರ್ಘ n1, ದೀರ್ಘ n2)
{
    listitem_T	*ಅಂಶ;
    list_T	*l;
    ದೀರ್ಘ	ಎಣಿಕೆ_2;
    ದೀರ್ಘ	idx;

    ಆದರೆ (n2 < n1)
	ಹಿಂತಿರುಗಿ list_alloc();
    ಆದರೆ (n1 > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - (n2 - n1) - 1)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಎಣಿಕೆ_2 = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_ವಸ್ತುಗಳು((ಪೂರ್ಣಾಂಕ)ಎಣಿಕೆ_2);
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಅಂಶ = list_ಕಂಡುಹಿಡಿ(ol, n1);
    ಗಾಗಿ (idx = 0; idx < ಎಣಿಕೆ_2; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	{
	    list_free(l);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	copy_tv(&ಅಂಶ->li_tv, &new_tv);
	list_ಗಣ_ಅಂಶ(l, (ಪೂರ್ಣಾಂಕ)idx, &new_tv);
	ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ;
    }
    ಹಿಂತಿರುಗಿ l;
}

    ಪೂರ್ಣಾಂಕ
list_ತುಂಡು_or_ಸೂಚ್ಯಂಕ(
	    list_T	*list,
	    ಪೂರ್ಣಾಂಕ		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    ಪೂರ್ಣಾಂಕ		exclusive,
	    typval_T	*rettv,
	    ಪೂರ್ಣಾಂಕ		verbose)
{
    ದೀರ್ಘ	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ಆದರೆ (n1 < 0)
	n1 = len + n1;
    ಆದರೆ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	ಆದರೆ (!range)
	{
	    ಆದರೆ (verbose)
		semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), (ದೀರ್ಘ)n1_arg);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (ಒಳ_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ಇಲ್ಲದಿದ್ದರೆ
	    n1 = len;
    }
    ಆದರೆ (range)
    {
	list_T	*l;

	ಆದರೆ (n2 < 0)
	    n2 = len + n2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ಆದರೆ (exclusive)
	    --n2;
	ಆದರೆ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_ತುಂಡು(list, n1, n2);
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	clear_tv(rettv);
	rettv_list_ಗಣ(rettv, l);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	ದೀರ್ಘ ಸೂಚ್ಯಂಕ = n1;
	listitem_T *li = check_range_ಸೂಚ್ಯಂಕ_one(list, &ಸೂಚ್ಯಂಕ, TRUE, TRUE);
	ಆದರೆ (li == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	ಆದರೆ (ಒಳ_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != ಖಾಲಿ
					&& var1.vval.v_list->lv_type == ಖಾಲಿ)
	{
	    ಆದರೆ (list->lv_type != ಖಾಲಿ && list->lv_type->tt_member != ಖಾಲಿ)
		ಗಣ_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, ಪೂರ್ಣಾಂಕ deep, ಪೂರ್ಣಾಂಕ ತುದಿ, ಪೂರ್ಣಾಂಕ copyID)
{
    list_T	*copy;
    listitem_T	*ಅಂಶ;
    ಪೂರ್ಣಾಂಕ		idx = 0;

    ಆದರೆ (orig == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_ವಸ್ತುಗಳು(orig->lv_len);
    ಆದರೆ (copy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (orig->lv_type == ಖಾಲಿ || ತುದಿ || deep)
	copy->lv_type = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	copy->lv_type = alloc_type(orig->lv_type);
    ಆದರೆ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    ಗಾಗಿ (ಅಂಶ = orig->lv_ಮೊದಲ; ಅಂಶ != ಖಾಲಿ && !got_int;
	    ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ)
    {
	typval_T	new_tv;

	ಆದರೆ (deep)
	{
	    ಆದರೆ (ಅಂಶ_copy(&ಅಂಶ->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    copy_tv(&ಅಂಶ->li_tv, &new_tv);
	list_ಗಣ_ಅಂಶ(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
    {
	list_unref(copy);
	copy = ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    ಶೂನ್ಯ
vimlist_remove(list_T *l, listitem_T *ಅಂಶ, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    ಗಾಗಿ (ip = ಅಂಶ; ip != ಖಾಲಿ; ip = ip->li_ಮುಂದಿನ)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	ಆದರೆ (ip == item2)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (item2->li_ಮುಂದಿನ == ಖಾಲಿ)
	l->lv_u.mat.lv_ಕೊನೆಯ = ಅಂಶ->li_prev;
    ಇಲ್ಲದಿದ್ದರೆ
	item2->li_ಮುಂದಿನ->li_prev = ಅಂಶ->li_prev;
    ಆದರೆ (ಅಂಶ->li_prev == ಖಾಲಿ)
	l->lv_ಮೊದಲ = item2->li_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಅಂಶ->li_prev->li_ಮುಂದಿನ = item2->li_ಮುಂದಿನ;
    l->lv_u.mat.lv_idx_ಅಂಶ = ಖಾಲಿ;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, ಪೂರ್ಣಾಂಕ copyID, ಪೂರ್ಣಾಂಕ restore_copyID)
{
    garray_T	ga;

    ಆದರೆ (tv->vval.v_list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    ಆದರೆ (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    ಹಿಂತಿರುಗಿ (char_u *)ga.ga_ದತ್ತಾಂಶ;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    ಪೂರ್ಣಾಂಕ		echo_style,
    ಪೂರ್ಣಾಂಕ		restore_copyID,
    ಪೂರ್ಣಾಂಕ		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    ಪೂರ್ಣಾಂಕ		i;
    join_T	*p;
    ದೀರ್ಘ	sumlen = 0;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = TRUE;
    ಪೂರ್ಣಾಂಕ		prev_ಬೀಗ;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    listitem_T	*ಅಂಶ;
    string_T	s;
    size_t	seplen;

    // Stringify each item in the list.
    CHECK_LIST_MATERIALIZE(l);
    // Lock the list, the string() method of an object item could remove the
    // item the loop is standing on.
    prev_ಬೀಗ = l->lv_ಬೀಗ;
    ಆದರೆ (l->lv_ಬೀಗ == 0)
	l->lv_ಬೀಗ = VAR_LOCKED;
    ಗಾಗಿ (ಅಂಶ = l->lv_ಮೊದಲ; ಅಂಶ != ಖಾಲಿ && !got_int; ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ)
    {
	s.string = echo_string_core(&ಅಂಶ->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ಆದರೆ (s.string == ಖಾಲಿ)
	{
	    l->lv_ಬೀಗ = prev_ಬೀಗ;
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	s.ಉದ್ದಳತೆ = STRLEN(s.string);
	sumlen += (ದೀರ್ಘ)s.ಉದ್ದಳತೆ;

	(ಶೂನ್ಯ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_ದತ್ತಾಂಶ) + (join_gap->ga_len++);
	ಆದರೆ (tofree != ಖಾಲಿ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.ಉದ್ದಳತೆ = s.ಉದ್ದಳತೆ;
	    p->tofree = tofree;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    p->s.string = vim_strnsave(s.string, s.ಉದ್ದಳತೆ);
	    p->s.ಉದ್ದಳತೆ = s.ಉದ್ದಳತೆ;
	    p->tofree = p->s.string;
	}

	ಸಾಲು_breakcheck();
	ಆದರೆ (did_echo_string_emsg)  // recursion error, bail out
	    ನಿಲ್ಲಿಸು;
    }
    l->lv_ಬೀಗ = prev_ಬೀಗ;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    ಆದರೆ (join_gap->ga_len >= 2)
	sumlen += (ದೀರ್ಘ)seplen * (join_gap->ga_len - 1);
    ಆದರೆ (sumlen > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - 2 || ga_grow(gap, (ಪೂರ್ಣಾಂಕ)sumlen + 2) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಗಾಗಿ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ಆದರೆ (ಮೊದಲ)
	    ಮೊದಲ = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_ದತ್ತಾಂಶ) + i;

	ಆದರೆ (p->s.string != ಖಾಲಿ)
	    ga_concat_len(gap, p->s.string, p->s.ಉದ್ದಳತೆ);
	ಸಾಲು_breakcheck();
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    ಪೂರ್ಣಾಂಕ
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    ಪೂರ್ಣಾಂಕ		echo_style,
    ಪೂರ್ಣಾಂಕ		restore_copyID,
    ಪೂರ್ಣಾಂಕ		copyID)
{
    garray_T	join_ga;
    ಪೂರ್ಣಾಂಕ		retval;
    join_T	*p;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (l->lv_len < 1)
	ಹಿಂತಿರುಗಿ OK; // nothing to do
    ga_init2(&join_ga, ಗಾತ್ರ(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ಆದರೆ (join_ga.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * "join()" function
 */
    ಶೂನ್ಯ
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    ಆದರೆ (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == ಖಾಲಿ)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    ಇಲ್ಲದಿದ್ದರೆ
	sep = tv_get_string_chk(&argvars[1]);

    ಆದರೆ (sep != ಖಾಲಿ)
    {
	ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 80);
	ಆದರೆ (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	ಇಲ್ಲದಿದ್ದರೆ
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_ದತ್ತಾಂಶ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = ಖಾಲಿ;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಂಕ do_error)
{
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg == ಖಾಲಿ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = ಖಾಲಿ;
    typval_T	tv;
    listitem_T	*ಅಂಶ;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    ಪೂರ್ಣಾಂಕ		had_comma;

    ಆದರೆ (evaluate)
    {
	l = list_alloc();
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ಆಗ (**arg != ']' && **arg != NUL)
    {
	ಆದರೆ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    ಹೋಗು failret;
	ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(&tv) == FAIL)
	{
	    ಆದರೆ (evaluate)
		clear_tv(&tv);
	    ಹೋಗು failret;
	}
	ಆದರೆ (evaluate)
	{
	    ಅಂಶ = listitem_alloc();
	    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
	    {
		ಅಂಶ->li_tv = tv;
		ಅಂಶ->li_tv.v_ಬೀಗ = 0;
		list_append(l, ಅಂಶ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	ಆದರೆ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ಆದರೆ (had_comma)
	{
	    ಆದರೆ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ಹೋಗು failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಆದರೆ (**arg == ']')
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (!had_comma)
	{
	    ಆದರೆ (do_error)
	    {
		ಆದರೆ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ಇಲ್ಲದಿದ್ದರೆ
		    semsg(_(e_missing_comma_ಒಳ_list_str), *arg);
	    }
	    ಹೋಗು failret;
	}
    }

    ಆದರೆ (**arg != ']')
    {
	ಆದರೆ (do_error)
	    semsg(_(e_missing_ಕೊನೆ_ದ_list_rsb_str), *arg);
failret:
	ಆದರೆ (evaluate)
	    list_free(l);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    *arg += 1;
    ಆದರೆ (evaluate)
	rettv_list_ಗಣ(rettv, l);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    ಪೂರ್ಣಾಂಕ
write_list(FILE *fd, list_T *list, ಪೂರ್ಣಾಂಕ ದ್ವಿಮಾನ)
{
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ವಸ್ತುಗಳು(list, li)
    {
	ಗಾಗಿ (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    ಆದರೆ (*s == '\n')
		c = putc(NUL, fd);
	    ಇಲ್ಲದಿದ್ದರೆ
		c = putc(*s, fd);
	    ಆದರೆ (c == EOF)
	    {
		ret = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (!ದ್ವಿಮಾನ || li->li_ಮುಂದಿನ != ಖಾಲಿ)
	    ಆದರೆ (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		ನಿಲ್ಲಿಸು;
	    }
	ಆದರೆ (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    ನಿಲ್ಲಿಸು;
	}
    }
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Initialize a static list with 10 items.
 */
    ಶೂನ್ಯ
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    ಪೂರ್ಣಾಂಕ	    i;

    CLEAR_ಸೂಚಕ(sl);
    l->lv_ಮೊದಲ = &sl->sl_ವಸ್ತುಗಳು[0];
    l->lv_u.mat.lv_ಕೊನೆಯ = &sl->sl_ವಸ್ತುಗಳು[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_ಬೀಗ = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    ಗಾಗಿ (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_ವಸ್ತುಗಳು[i];

	ಆದರೆ (i == 0)
	    li->li_prev = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    li->li_prev = li - 1;
	ಆದರೆ (i == 9)
	    li->li_ಮುಂದಿನ = ಖಾಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ
	    li->li_ಮುಂದಿನ = li + 1;
    }
}

/*
 * "list2str()" function
 */
    ಶೂನ್ಯ
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    l = argvars[0].vval.v_list;
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // empty list results in empty string

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[1], ಖಾಲಿ);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    ಆದರೆ (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	ಪೂರ್ಣಾಂಕ	(*char2bytes)(ಪೂರ್ಣಾಂಕ, char_u *);

	ಆದರೆ (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	ಇಲ್ಲದಿದ್ದರೆ
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_ಸಂಖ್ಯೆ(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ದೀರ್ಘ len = (ದೀರ್ಘ)list_len(l) + 1;

	ಆದರೆ (len <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ && ga_grow(&ga, (ಪೂರ್ಣಾಂಕ)len) == OK)
	{
	    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
		ga_append(&ga, tv_get_ಸಂಖ್ಯೆ(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_ದತ್ತಾಂಶ;
}

/*
 * "list2tuple()" function
 */
    ಶೂನ್ಯ
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ಖಾಲಿ;

    ಆದರೆ (check_for_list_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    l = argvars[0].vval.v_list;
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    ಆದರೆ (rettv_tuple_ಗಣ_with_ವಸ್ತುಗಳು(rettv, list_len(l)) == FAIL)
	ಹಿಂತಿರುಗಿ;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ಅಂಶ(tuple, TUPLE_LEN(tuple)));
	tuple->tv_ವಸ್ತುಗಳು.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    ಶೂನ್ಯ
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_tuple_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    tuple = argvars[0].vval.v_tuple;
    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ಅಂಶ(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*ಅಂಶ, *item2;
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    ದೀರ್ಘ	idx;
    ದೀರ್ಘ	ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		cnt = 0;
    list_T	*rl;

    ಆದರೆ ((l = argvars[0].vval.v_list) == ಖಾಲಿ
			     || ಮೌಲ್ಯ_check_ಬೀಗ(l->lv_ಬೀಗ, arg_errmsg, TRUE))
	ಹಿಂತಿರುಗಿ;

    idx = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;		// type error: do nothing, errmsg already given

    ಆದರೆ ((ಅಂಶ = list_ಕಂಡುಹಿಡಿ(l, idx)) == ಖಾಲಿ)
    {
	semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), idx);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, ಅಂಶ, ಅಂಶ);
	*rettv = ಅಂಶ->li_tv;
	list_free_ಅಂಶ(l, ಅಂಶ);
	ಹಿಂತಿರುಗಿ;
    }

    // Remove range of items, return list with values.
    ಕೊನೆ = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;		// type error: do nothing

    ಆದರೆ ((item2 = list_ಕಂಡುಹಿಡಿ(l, ಕೊನೆ)) == ಖಾಲಿ)
    {
	semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), ಕೊನೆ);
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (li = ಅಂಶ; li != ಖಾಲಿ; li = li->li_ಮುಂದಿನ)
    {
	++cnt;
	ಆದರೆ (li == item2)
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (li == ಖಾಲಿ)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	ಹಿಂತಿರುಗಿ;
    }

    vimlist_remove(l, ಅಂಶ, item2);
    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rl = rettv->vval.v_list;

    ಆದರೆ (l->lv_with_ವಸ್ತುಗಳು > 0)
    {
	// need to copy the list items and move the value
	ಆಗ (ಅಂಶ != ಖಾಲಿ)
	{
	    li = listitem_alloc();
	    ಆದರೆ (li == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    li->li_tv = ಅಂಶ->li_tv;
	    init_tv(&ಅಂಶ->li_tv);
	    list_append(rl, li);
	    ಆದರೆ (ಅಂಶ == item2)
		ನಿಲ್ಲಿಸು;
	    ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	rl->lv_ಮೊದಲ = ಅಂಶ;
	rl->lv_u.mat.lv_ಕೊನೆಯ = item2;
	ಅಂಶ->li_prev = ಖಾಲಿ;
	item2->li_ಮುಂದಿನ = ಖಾಲಿ;
	rl->lv_len = cnt;
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಅಂಶ_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಅಂಶ_compare2(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2);

// struct used in the array that's given to qsort()
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    listitem_T	*ಅಂಶ;
    ಪೂರ್ಣಾಂಕ		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    ಒಕ್ಕೂಟ {
	varnumber_T	inum;	// for "N"
	ದ್ವಿಗುಣ		fnum;	// for "n" and "f"
    } ಕೀಲಿ;
} ವಿಂಗಡಿಸು_ಅಂಶ_T;

// struct storing information about current sort
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_compare_ic;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_compare_lc;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_compare_numeric;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_compare_numbers;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_compare_float;
    char_u	*ಅಂಶ_compare_func;
    partial_T	*ಅಂಶ_compare_partial;
    dict_T	*ಅಂಶ_compare_selfdict;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_compare_func_err;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_compare_keep_zero;
    ಪೂರ್ಣಾಂಕ		ಅಂಶ_compare_ಕೀಲಿಗಳು_ready;  // ptrs[].key is precomputed
} sortinfo_T;
ಸ್ಥಿರ sortinfo_T	*sortinfo = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿಸು ಅಂಶ_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಅಂಶ_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ವಿಂಗಡಿಸು_ಅಂಶ_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = ಖಾಲಿ, *tofree2 = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (ವಿಂಗಡಿಸು_ಅಂಶ_T *)s1;
    si2 = (ವಿಂಗಡಿಸು_ಅಂಶ_T *)s2;
    tv1 = &si1->ಅಂಶ->li_tv;
    tv2 = &si2->ಅಂಶ->li_tv;

    ಆದರೆ (sortinfo->ಅಂಶ_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->ಅಂಶ_compare_ಕೀಲಿಗಳು_ready
					? si1->ಕೀಲಿ.inum : tv_ಗಮ್ಯ_ಸಂಖ್ಯೆ(tv1);
	varnumber_T	v2 = sortinfo->ಅಂಶ_compare_ಕೀಲಿಗಳು_ready
					? si2->ಕೀಲಿ.inum : tv_ಗಮ್ಯ_ಸಂಖ್ಯೆ(tv2);

	ಹಿಂತಿರುಗಿ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ಆದರೆ (sortinfo->ಅಂಶ_compare_float)
    {
	float_T	v1 = sortinfo->ಅಂಶ_compare_ಕೀಲಿಗಳು_ready
					? si1->ಕೀಲಿ.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->ಅಂಶ_compare_ಕೀಲಿಗಳು_ready
					? si2->ಕೀಲಿ.fnum : tv_get_float(tv2);

	ಹಿಂತಿರುಗಿ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ಆದರೆ (sortinfo->ಅಂಶ_compare_numeric && sortinfo->ಅಂಶ_compare_ಕೀಲಿಗಳು_ready)
    {
	ದ್ವಿಗುಣ	n1 = si1->ಕೀಲಿ.fnum;
	ದ್ವಿಗುಣ	n2 = si2->ಕೀಲಿ.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	ಆದರೆ (res == 0 && !sortinfo->ಅಂಶ_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	ಹಿಂತಿರುಗಿ res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    ಆದರೆ (tv1->v_type == VAR_STRING)
    {
	ಆದರೆ (tv2->v_type != VAR_STRING || sortinfo->ಅಂಶ_compare_numeric)
	    p1 = (char_u *)"'";
	ಇಲ್ಲದಿದ್ದರೆ
	    p1 = tv1->vval.v_string;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    ಆದರೆ (tv2->v_type == VAR_STRING)
    {
	ಆದರೆ (tv1->v_type != VAR_STRING || sortinfo->ಅಂಶ_compare_numeric)
	    p2 = (char_u *)"'";
	ಇಲ್ಲದಿದ್ದರೆ
	    p2 = tv2->vval.v_string;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    ಆದರೆ (p1 == ಖಾಲಿ)
	p1 = (char_u *)"";
    ಆದರೆ (p2 == ಖಾಲಿ)
	p2 = (char_u *)"";
    ಆದರೆ (!sortinfo->ಅಂಶ_compare_numeric)
    {
	ಆದರೆ (sortinfo->ಅಂಶ_compare_lc)
	    res = strcoll((ಅಕ್ಷರ *)p1, (ಅಕ್ಷರ *)p2);
	ಇಲ್ಲದಿದ್ದರೆ
	    res = sortinfo->ಅಂಶ_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ದ್ವಿಗುಣ n1, n2;
	n1 = strtod((ಅಕ್ಷರ *)p1, (ಅಕ್ಷರ **)&p1);
	n2 = strtod((ಅಕ್ಷರ *)p2, (ಅಕ್ಷರ **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    ಆದರೆ (res == 0 && !sortinfo->ಅಂಶ_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    ಹಿಂತಿರುಗಿ res;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಅಂಶ_compare2(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *s2)
{
    ವಿಂಗಡಿಸು_ಅಂಶ_T  *si1, *si2;
    ಪೂರ್ಣಾಂಕ		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->ಅಂಶ_compare_partial;
    funcexe_T	funcexe;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    ಆದರೆ (sortinfo->ಅಂಶ_compare_func_err)
	ಹಿಂತಿರುಗಿ 0;

    si1 = (ವಿಂಗಡಿಸು_ಅಂಶ_T *)s1;
    si2 = (ವಿಂಗಡಿಸು_ಅಂಶ_T *)s2;

    ಆದರೆ (partial == ಖಾಲಿ)
	func_name = sortinfo->ಅಂಶ_compare_func;
    ಇಲ್ಲದಿದ್ದರೆ
	func_name = partial_name(partial);

    // Copy the values.  This is needed to be able to set v_lock to VAR_FIXED
    // in the copy without changing the original list items.
    copy_tv(&si1->ಅಂಶ->li_tv, &argv[0]);
    copy_tv(&si2->ಅಂಶ->li_tv, &argv[1]);

    rettv.v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_ಕ್ಷೇತ್ರ(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = partial;
    funcexe.fe_selfdict = sortinfo->ಅಂಶ_compare_selfdict;
    res = call_func(func_name, -1, &rettv, 2, argv, &funcexe);
    clear_tv(&argv[0]);
    clear_tv(&argv[1]);

    ಆದರೆ (res == FAIL || did_emsg > did_emsg_before)
	res = ಅಂಶ_COMPARE_FAIL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	varnumber_T	n;

	n = tv_get_ಸಂಖ್ಯೆ_chk(&rettv, &sortinfo->ಅಂಶ_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    ಆದರೆ (sortinfo->ಅಂಶ_compare_func_err)
	res = ಅಂಶ_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    ಆದರೆ (res == 0 && !sortinfo->ಅಂಶ_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    ಹಿಂತಿರುಗಿ res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ವಿಂಗಡಿಸು_compute_ಕೀಲಿಗಳು(ವಿಂಗಡಿಸು_ಅಂಶ_T *ptrs, ದೀರ್ಘ len, sortinfo_T *info)
{
    ದೀರ್ಘ	i;

    ಆದರೆ (info->ಅಂಶ_compare_numbers)
    {
	ಗಾಗಿ (i = 0; i < len; ++i)
	    ptrs[i].ಕೀಲಿ.inum = tv_ಗಮ್ಯ_ಸಂಖ್ಯೆ(&ptrs[i].ಅಂಶ->li_tv);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (info->ಅಂಶ_compare_float)
    {
	ಗಾಗಿ (i = 0; i < len; ++i)
	    ptrs[i].ಕೀಲಿ.fnum = tv_get_float(&ptrs[i].ಅಂಶ->li_tv);
    }
    ಇಲ್ಲದಿದ್ದರೆ // info->item_compare_numeric
    {
	ಗಾಗಿ (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].ಅಂಶ->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    ಆದರೆ (tv->v_type == VAR_STRING)
		ptrs[i].ಕೀಲಿ.fnum = 0.0;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = ಖಾಲಿ;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].ಕೀಲಿ.fnum = p == ಖಾಲಿ ? 0.0 : strtod((ಅಕ್ಷರ *)p, ಖಾಲಿ);
		vim_free(tofree);
	    }
	}
    }
    info->ಅಂಶ_compare_ಕೀಲಿಗಳು_ready = TRUE;
}

/*
 * sort() List "l"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_ವಿಂಗಡಿಸು(list_T *l, sortinfo_T *info)
{
    ದೀರ್ಘ	len;
    ವಿಂಗಡಿಸು_ಅಂಶ_T	*ptrs;
    ದೀರ್ಘ	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(ವಿಂಗಡಿಸು_ಅಂಶ_T, len);
    ಆದರೆ (ptrs == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_ವಸ್ತುಗಳು(l, li)
    {
	ptrs[i].ಅಂಶ = li;
	ptrs[i].idx = i;
	++i;
    }

    info->ಅಂಶ_compare_func_err = FALSE;
    info->ಅಂಶ_compare_keep_zero = FALSE;
    info->ಅಂಶ_compare_ಕೀಲಿಗಳು_ready = FALSE;

    ಆದರೆ (info->ಅಂಶ_compare_func == ಖಾಲಿ && info->ಅಂಶ_compare_partial == ಖಾಲಿ
	    && (info->ಅಂಶ_compare_numbers || info->ಅಂಶ_compare_float
						|| info->ಅಂಶ_compare_numeric))
	ವಿಂಗಡಿಸು_compute_ಕೀಲಿಗಳು(ptrs, len, info);
    // test the compare function
    ಆದರೆ ((info->ಅಂಶ_compare_func != ಖಾಲಿ
		|| info->ಅಂಶ_compare_partial != ಖಾಲಿ)
	    && ಅಂಶ_compare2((ಶೂನ್ಯ *)&ptrs[0], (ಶೂನ್ಯ *)&ptrs[1])
	    == ಅಂಶ_COMPARE_FAIL)
	emsg(_(e_ವಿಂಗಡಿಸು_compare_function_failed));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Sort the array with item pointers.
	qsort((ಶೂನ್ಯ *)ptrs, (size_t)len, ಗಾತ್ರ(ವಿಂಗಡಿಸು_ಅಂಶ_T),
		info->ಅಂಶ_compare_func == ಖಾಲಿ
		&& info->ಅಂಶ_compare_partial == ಖಾಲಿ
		? ಅಂಶ_compare : ಅಂಶ_compare2);

	ಆದರೆ (!info->ಅಂಶ_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_ಮೊದಲ = l->lv_u.mat.lv_ಕೊನೆಯ
		= l->lv_u.mat.lv_idx_ಅಂಶ = ಖಾಲಿ;
	    l->lv_len = 0;
	    ಗಾಗಿ (i = 0; i < len; ++i)
		list_append(l, ptrs[i].ಅಂಶ);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_uniq(list_T *l, sortinfo_T *info)
{
    ದೀರ್ಘ	len;
    ವಿಂಗಡಿಸು_ಅಂಶ_T	*ptrs;
    ದೀರ್ಘ	i = 0;
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ	(*ಅಂಶ_compare_func_ptr)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(ವಿಂಗಡಿಸು_ಅಂಶ_T, len);
    ಆದರೆ (ptrs == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // f_uniq(): ptrs will be a stack of items to remove
    info->ಅಂಶ_compare_func_err = FALSE;
    info->ಅಂಶ_compare_keep_zero = TRUE;
    ಅಂಶ_compare_func_ptr = info->ಅಂಶ_compare_func != ಖಾಲಿ
	|| info->ಅಂಶ_compare_partial != ಖಾಲಿ
	? ಅಂಶ_compare2 : ಅಂಶ_compare;

    ಗಾಗಿ (li = l->lv_ಮೊದಲ; li != ಖಾಲಿ && li->li_ಮುಂದಿನ != ಖಾಲಿ;
	    li = li->li_ಮುಂದಿನ)
    {
	ಆದರೆ (ಅಂಶ_compare_func_ptr((ಶೂನ್ಯ *)&li, (ಶೂನ್ಯ *)&li->li_ಮುಂದಿನ)
		== 0)
	    ptrs[i++].ಅಂಶ = li;
	ಆದರೆ (info->ಅಂಶ_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (!info->ಅಂಶ_compare_func_err)
    {
	ಆಗ (--i >= 0)
	{
	    li = ptrs[i].ಅಂಶ->li_ಮುಂದಿನ;
	    ptrs[i].ಅಂಶ->li_ಮುಂದಿನ = li->li_ಮುಂದಿನ;
	    ಆದರೆ (li->li_ಮುಂದಿನ != ಖಾಲಿ)
		li->li_ಮುಂದಿನ->li_prev = ptrs[i].ಅಂಶ;
	    ಇಲ್ಲದಿದ್ದರೆ
		l->lv_u.mat.lv_ಕೊನೆಯ = ptrs[i].ಅಂಶ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
parse_ವಿಂಗಡಿಸು_uniq_ವಾದಗಳು(typval_T *argvars, sortinfo_T *info)
{
    info->ಅಂಶ_compare_ic = FALSE;
    info->ಅಂಶ_compare_lc = FALSE;
    info->ಅಂಶ_compare_numeric = FALSE;
    info->ಅಂಶ_compare_numbers = FALSE;
    info->ಅಂಶ_compare_float = FALSE;
    info->ಅಂಶ_compare_func = ಖಾಲಿ;
    info->ಅಂಶ_compare_partial = ಖಾಲಿ;
    info->ಅಂಶ_compare_selfdict = ಖಾಲಿ;
    info->ಅಂಶ_compare_ಕೀಲಿಗಳು_ready = FALSE;

    ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ OK;

    // optional second argument: {func}
    ಆದರೆ (argvars[1].v_type == VAR_FUNC)
	info->ಅಂಶ_compare_func = argvars[1].vval.v_string;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type == VAR_PARTIAL)
	info->ಅಂಶ_compare_partial = argvars[1].vval.v_partial;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	    error = FALSE;
	ಪೂರ್ಣಾಂಕ	    nr = 0;

	ಆದರೆ (argvars[1].v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    nr = tv_get_ಸಂಖ್ಯೆ_chk(&argvars[1], &error);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (nr == 1)
		info->ಅಂಶ_compare_ic = TRUE;
	}
	ಆದರೆ (nr != 1)
	{
	    ಆದರೆ (argvars[1].v_type != VAR_ಸಂಖ್ಯೆ)
		info->ಅಂಶ_compare_func = tv_get_string(&argvars[1]);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nr != 0)
	    {
		emsg(_(e_invalid_ವಾದ));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಆದರೆ (info->ಅಂಶ_compare_func != ಖಾಲಿ)
	{
	    ಆದರೆ (*info->ಅಂಶ_compare_func == NUL)
	    {
		// empty string means default sort
		info->ಅಂಶ_compare_func = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(info->ಅಂಶ_compare_func, "n") == 0)
	    {
		info->ಅಂಶ_compare_func = ಖಾಲಿ;
		info->ಅಂಶ_compare_numeric = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(info->ಅಂಶ_compare_func, "N") == 0)
	    {
		info->ಅಂಶ_compare_func = ಖಾಲಿ;
		info->ಅಂಶ_compare_numbers = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(info->ಅಂಶ_compare_func, "f") == 0)
	    {
		info->ಅಂಶ_compare_func = ಖಾಲಿ;
		info->ಅಂಶ_compare_float = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(info->ಅಂಶ_compare_func, "i") == 0)
	    {
		info->ಅಂಶ_compare_func = ಖಾಲಿ;
		info->ಅಂಶ_compare_ic = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(info->ಅಂಶ_compare_func, "l") == 0)
	    {
		info->ಅಂಶ_compare_func = ಖಾಲಿ;
		info->ಅಂಶ_compare_lc = TRUE;
	    }
	}
    }

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	ಆದರೆ (check_for_dict_arg(argvars, 2) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	info->ಅಂಶ_compare_selfdict = argvars[2].vval.v_dict;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * "sort()" or "uniq()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_ವಿಂಗಡಿಸು_uniq(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ ವಿಂಗಡಿಸು)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    ದೀರ್ಘ	len;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_ವಾದ_ದ_str_must_be_list), ವಿಂಗಡಿಸು ? "sort()" : "uniq()");
	ಹಿಂತಿರುಗಿ;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    ಆದರೆ (l != ಖಾಲಿ && ಮೌಲ್ಯ_check_ಬೀಗ(l->lv_ಬೀಗ,
		(char_u *)(ವಿಂಗಡಿಸು ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	ಹೋಗು theend;
    rettv_list_ಗಣ(rettv, l);
    ಆದರೆ (l == ಖಾಲಿ)
	ಹೋಗು theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    ಆದರೆ (len <= 1)
	ಹೋಗು theend;	// short list sorts pretty quickly

    ಆದರೆ (parse_ವಿಂಗಡಿಸು_uniq_ವಾದಗಳು(argvars, &info) == FAIL)
	ಹೋಗು theend;

    ಆದರೆ (ವಿಂಗಡಿಸು)
	do_ವಿಂಗಡಿಸು(l, &info);
    ಇಲ್ಲದಿದ್ದರೆ
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    ಶೂನ್ಯ
f_ವಿಂಗಡಿಸು(typval_T *argvars, typval_T *rettv)
{
    do_ವಿಂಗಡಿಸು_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    ಶೂನ್ಯ
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_ವಿಂಗಡಿಸು_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    ಪೂರ್ಣಾಂಕ
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	ಪೂರ್ಣಾಂಕ		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    ಪೂರ್ಣಾಂಕ		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    ಆದರೆ (filtermap == FILTERMAP_FOREACH && ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(ಅಭಿವ್ಯಕ್ತಿ->vval.v_string);
	ಆದರೆ (!did_emsg)
	    retval = OK;
	ಹೋಗು theend;
    }

    argv[0] = *get_vim_var_tv(VV_ಕೀಲಿ);
    argv[1] = *get_vim_var_tv(VV_VAL);
    ಆದರೆ (eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, FALSE, argv, 2, fc, newtv) == FAIL)
	ಹೋಗು theend;
    ಆದರೆ (filtermap == FILTERMAP_FILTER)
    {
	ಪೂರ್ಣಾಂಕ	    error = FALSE;

	// filter(): when expr is zero remove the item
	ಆದರೆ (ಒಳ_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	ಇಲ್ಲದಿದ್ದರೆ
	    *remp = (tv_get_ಸಂಖ್ಯೆ_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	ಆದರೆ (error)
	    ಹೋಗು theend;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	ಅಕ್ಷರ		*func_name,
	char_u		*arg_errmsg,
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
	typval_T	*rettv)
{
    ಪೂರ್ಣಾಂಕ		prev_ಬೀಗ;
    list_T	*l_ret = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		idx = 0;
    ಪೂರ್ಣಾಂಕ		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    ಆದರೆ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = ಖಾಲಿ;
    }
    ಆದರೆ (l == ಖಾಲಿ || (filtermap == FILTERMAP_FILTER
			    && ಮೌಲ್ಯ_check_ಬೀಗ(l->lv_ಬೀಗ, arg_errmsg, TRUE)))
	ಹಿಂತಿರುಗಿ;

    prev_ಬೀಗ = l->lv_ಬೀಗ;

    ಆದರೆ (filtermap == FILTERMAP_MAPNEW)
    {
	ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    ಗಣ_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    ಆದರೆ (l->lv_ಬೀಗ == 0)
	l->lv_ಬೀಗ = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, &newtv);

    ಆದರೆ (l->lv_ಮೊದಲ == &range_list_ಅಂಶ)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_ಆರಂಭ;
	ಪೂರ್ಣಾಂಕ		len = l->lv_len;
	ಪೂರ್ಣಾಂಕ		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	ಆದರೆ (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_ಮೊದಲ = ಖಾಲಿ;
	    l->lv_u.mat.lv_ಕೊನೆಯ = ಖಾಲಿ;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_ಅಂಶ = ಖಾಲಿ;
	}

	ಗಾಗಿ (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_ಸಂಖ್ಯೆ;
	    tv.v_ಬೀಗ = 0;
	    tv.vval.v_ಸಂಖ್ಯೆ = val;
	    ಗಣ_vim_var_nr(VV_ಕೀಲಿ, idx);
	    ಆದರೆ (filter_map_one(&tv, ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc, &newtv, &rem) == FAIL)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (did_emsg)
	    {
		clear_tv(&newtv);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (filtermap != FILTERMAP_FOREACH)
	    {
		ಆದರೆ (filtermap != FILTERMAP_FILTER)
		{
		    ಆದರೆ (filtermap == FILTERMAP_MAP && argtype != ಖಾಲಿ
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			ನಿಲ್ಲಿಸು;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    ಆದರೆ (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!rem)
		{
		    // filter(): append the list item value when not rem
		    ಆದರೆ (list_append_tv_move(l, &tv) == FAIL)
			ನಿಲ್ಲಿಸು;
		}
	    }

	    val += stride;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Materialized list: loop over the items
	ಗಾಗಿ (li = l->lv_ಮೊದಲ; li != ಖಾಲಿ; li = nli)
	{
	    ಆದರೆ (filtermap == FILTERMAP_MAP && ಮೌಲ್ಯ_check_ಬೀಗ(
			li->li_tv.v_ಬೀಗ, arg_errmsg, TRUE))
		ನಿಲ್ಲಿಸು;
	    nli = li->li_ಮುಂದಿನ;
	    ಗಣ_vim_var_nr(VV_ಕೀಲಿ, idx);
	    ಆದರೆ (filter_map_one(&li->li_tv, ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc,
							 &newtv, &rem) == FAIL)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (did_emsg)
	    {
		clear_tv(&newtv);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (filtermap == FILTERMAP_MAP)
	    {
		ಆದರೆ (argtype != ಖಾಲಿ && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ನಿಲ್ಲಿಸು;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_ಬೀಗ = 0;
		li->li_tv = newtv;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		ಆದರೆ (list_append_tv_move(l_ret, &newtv) == FAIL)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_ಬೀಗ = prev_ಬೀಗ;
    ಆದರೆ (fc != ಖಾಲಿ)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ;
    ಅಕ್ಷರ	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    ಪೂರ್ಣಾಂಕ		save_did_emsg;
    type_T	*type = ಖಾಲಿ;

    // map(), filter(), foreach() return the first argument, also on failure.
    ಆದರೆ (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (filtermap == FILTERMAP_MAP && ಒಳ_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	ಆದರೆ (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != ಖಾಲಿ)
	    type = argvars[0].vval.v_dict->dv_type;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != ಖಾಲಿ)
	    type = argvars[0].vval.v_list->lv_type;
    }

    ಆದರೆ (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	ಅಕ್ಷರ *ಸಂದೇಶ;

	ಆದರೆ (filtermap == FILTERMAP_FOREACH)
	    ಸಂದೇಶ = e_ವಾದ_ದ_str_must_be_list_tuple_string_dictionary_or_blob;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ = e_ವಾದ_ದ_str_must_be_list_string_dictionary_or_blob;
	semsg(_(ಸಂದೇಶ), func_name);
	ಹಿಂತಿರುಗಿ;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    ಅಭಿವ್ಯಕ್ತಿ = &argvars[1];
    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ->v_type == VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ;

    typval_T	save_val;
    typval_T	save_ಕೀಲಿ;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_ಕೀಲಿ, &save_ಕೀಲಿ);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ಆದರೆ (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, ಅಭಿವ್ಯಕ್ತಿ, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, ಅಭಿವ್ಯಕ್ತಿ,
							    arg_errmsg, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, ಅಭಿವ್ಯಕ್ತಿ, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, ಅಭಿವ್ಯಕ್ತಿ);
    ಇಲ್ಲದಿದ್ದರೆ // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, ಅಭಿವ್ಯಕ್ತಿ, rettv);

    restore_vimvar(VV_ಕೀಲಿ, &save_ಕೀಲಿ);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    ಶೂನ್ಯ
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    ಶೂನ್ಯ
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    ಶೂನ್ಯ
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    ಶೂನ್ಯ
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_ಕೂಡಿಸು(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    ಆದರೆ (l == ಖಾಲಿ)
    {
	ಆದರೆ (ಒಳ_vim9script())
	    emsg(_(e_cannot_ಕೂಡಿಸು_ಗಮ್ಯ_null_list));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಮೌಲ್ಯ_check_ಬೀಗ(l->lv_ಬೀಗ,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    ಶೂನ್ಯ
f_ಕೂಡಿಸು(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = 1; // Default: Failed

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_LIST)
	list_ಕೂಡಿಸು(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_BLOB)
	blob_ಕೂಡಿಸು(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
list_ಎಣಿಕೆ(list_T *l, typval_T *needle, ದೀರ್ಘ idx, ಪೂರ್ಣಾಂಕ ic)
{
    ದೀರ್ಘ	n = 0;
    listitem_T	*li;

    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    CHECK_LIST_MATERIALIZE(l);

    ಆದರೆ (list_len(l) == 0)
	ಹಿಂತಿರುಗಿ 0;

    li = list_ಕಂಡುಹಿಡಿ(l, idx);
    ಆದರೆ (li == ಖಾಲಿ)
    {
	semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), idx);
	ಹಿಂತಿರುಗಿ 0;
    }

    ಗಾಗಿ ( ; li != ಖಾಲಿ; li = li->li_ಮುಂದಿನ)
	ಆದರೆ (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    ಹಿಂತಿರುಗಿ n;
}

/*
 * "count()" function
 */
    ಶೂನ್ಯ
f_ಎಣಿಕೆ(typval_T *argvars, typval_T *rettv)
{
    ದೀರ್ಘ	n = 0;
    ಪೂರ್ಣಾಂಕ		ic = FALSE;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	ic = (ಪೂರ್ಣಾಂಕ)tv_get_bool_chk(&argvars[2], &error);

    ಆದರೆ (!error && argvars[0].v_type == VAR_STRING)
	n = string_ಎಣಿಕೆ(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!error && argvars[0].v_type == VAR_LIST)
    {
	ದೀರ್ಘ idx = 0;

	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], &error);
	ಆದರೆ (!error)
	    n = list_ಎಣಿಕೆ(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!error && argvars[0].v_type == VAR_TUPLE)
    {
	ದೀರ್ಘ idx = 0;

	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[3], &error);
	ಆದರೆ (!error)
	    n = tuple_ಎಣಿಕೆ(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!error && argvars[0].v_type == VAR_DICT)
    {
	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_ವಾದ));
	ಇಲ್ಲದಿದ್ದರೆ
	    n = dict_ಎಣಿಕೆ(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!error)
	semsg(_(e_ವಾದ_ದ_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_ಸಂಖ್ಯೆ = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	ಅಕ್ಷರ		*func_name,
	char_u		*arg_errmsg,
	ಪೂರ್ಣಾಂಕ		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*ಅಂಶ;
    ದೀರ್ಘ	before;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    l1 = argvars[0].vval.v_list;
    ಆದರೆ (l1 == ಖಾಲಿ)
    {
	emsg(_(e_cannot_extend_null_list));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (is_new || !ಮೌಲ್ಯ_check_ಬೀಗ(l1->lv_ಬೀಗ, arg_errmsg, TRUE))
    {
	ಆದರೆ (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    ಆದರೆ (l1 == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	}

	l2 = argvars[1].vval.v_list;
	ಆದರೆ (l2 == ಖಾಲಿ)
	    ಹೋಗು theend;

	ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
	    ಆದರೆ (error)
		ಹೋಗು cleanup;		// type error; errmsg already given
	    ಆದರೆ (before == l1->lv_len)
		ಅಂಶ = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಅಂಶ = list_ಕಂಡುಹಿಡಿ(l1, before);
		ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
		{
		    semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), before);
		    ಹೋಗು cleanup;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಅಂಶ = ಖಾಲಿ;
	ಆದರೆ (type != ಖಾಲಿ && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    ಹೋಗು cleanup;
	list_extend(l1, l2, ಅಂಶ);

theend:
	ಆದರೆ (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_ಬೀಗ = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    copy_tv(&argvars[0], rettv);
	ಹಿಂತಿರುಗಿ;

cleanup:
	ಆದರೆ (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, ಪೂರ್ಣಾಂಕ is_new)
{
    type_T	*type = ಖಾಲಿ;
    ಅಕ್ಷರ	*func_name = is_new ? "extendnew()" : "extend()";

    ಆದರೆ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	ಆದರೆ (!is_new && ಒಳ_vim9script() && argvars[0].vval.v_list != ಖಾಲಿ)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	ಆದರೆ (!is_new && ಒಳ_vim9script() && argvars[0].vval.v_dict != ಖಾಲಿ)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_ವಾದ_ದ_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    ಶೂನ್ಯ
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
    ಶೂನ್ಯ
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_ಸೇರಿಸು_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    ದೀರ್ಘ	before = 0;
    listitem_T	*ಅಂಶ;
    ಪೂರ್ಣಾಂಕ		error = FALSE;

    ಆದರೆ (l == ಖಾಲಿ)
    {
	ಆದರೆ (ಒಳ_vim9script())
	    emsg(_(e_cannot_ಕೂಡಿಸು_ಗಮ್ಯ_null_list));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(l->lv_ಬೀಗ, (char_u *)N_("insert() argument"), TRUE))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	before = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[2], &error);
    ಆದರೆ (error)
	ಹಿಂತಿರುಗಿ;		// type error; errmsg already given

    ಆದರೆ (before == l->lv_len)
	ಅಂಶ = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಂಶ = list_ಕಂಡುಹಿಡಿ(l, before);
	ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	{
	    semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), before);
	    l = ಖಾಲಿ;
	}
    }
    ಆದರೆ (l != ಖಾಲಿ)
    {
	(ಶೂನ್ಯ)list_ಸೇರಿಸು_tv(l, &argvars[1], ಅಂಶ);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    ಶೂನ್ಯ
f_ಸೇರಿಸು(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_BLOB)
	blob_ಸೇರಿಸು_func(argvars, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type != VAR_LIST)
	semsg(_(e_ವಾದ_ದ_str_must_be_list_or_blob), "insert()");
    ಇಲ್ಲದಿದ್ದರೆ
	list_ಸೇರಿಸು_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    ಶೂನ್ಯ
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
			|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_ವಾದ_ದ_str_must_be_list_dictionary_or_blob), "remove()");
}

    ಸ್ಥಿರ ಶೂನ್ಯ
list_ಹಿಮ್ಮುಖ(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_ಗಣ(rettv, l);
    ಆದರೆ (l != ಖಾಲಿ
	    && !ಮೌಲ್ಯ_check_ಬೀಗ(l->lv_ಬೀಗ,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	ಆದರೆ (l->lv_ಮೊದಲ == &range_list_ಅಂಶ)
	{
	    varnumber_T new_ಆರಂಭ = l->lv_u.nonmat.lv_ಆರಂಭ
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_ಕೊನೆ = new_ಆರಂಭ
		- (l->lv_u.nonmat.lv_ಕೊನೆ - l->lv_u.nonmat.lv_ಆರಂಭ);
	    l->lv_u.nonmat.lv_ಆರಂಭ = new_ಆರಂಭ;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    ಹಿಂತಿರುಗಿ;
	}
	li = l->lv_u.mat.lv_ಕೊನೆಯ;
	l->lv_ಮೊದಲ = l->lv_u.mat.lv_ಕೊನೆಯ = ಖಾಲಿ;
	l->lv_len = 0;
	ಆಗ (li != ಖಾಲಿ)
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
    ಶೂನ್ಯ
f_ಹಿಮ್ಮುಖ(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_BLOB)
	blob_ಹಿಮ್ಮುಖ(argvars[0].vval.v_blob, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	ಆದರೆ (argvars[0].vval.v_string != ಖಾಲಿ)
	    rettv->vval.v_string = ಹಿಮ್ಮುಖ_ಪಠ್ಯ(argvars[0].vval.v_string);
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_string = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_LIST)
	list_ಹಿಮ್ಮುಖ(argvars[0].vval.v_list, rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_TUPLE)
	tuple_ಹಿಮ್ಮುಖ(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_reduce(
	typval_T	*argvars,
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		range_list;
    ಪೂರ್ಣಾಂಕ		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    ಪೂರ್ಣಾಂಕ		r;
    ಪೂರ್ಣಾಂಕ		called_emsg_ಆರಂಭ = called_emsg;
    ಪೂರ್ಣಾಂಕ		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != ಖಾಲಿ && l->lv_ಮೊದಲ == &range_list_ಅಂಶ;
    ಆದರೆ (range_list)
	range_val = l->lv_u.nonmat.lv_ಆರಂಭ;

    ಆದರೆ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ಆದರೆ (l == ಖಾಲಿ || l->lv_len == 0)
	{
	    semsg(_(e_reduce_ದ_an_ಖಾಲಿಗಳು_str_with_no_initial_ಮೌಲ್ಯ), "List");
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (range_list)
	{
	    initial.v_type = VAR_ಸಂಖ್ಯೆ;
	    initial.vval.v_ಸಂಖ್ಯೆ = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    initial = l->lv_ಮೊದಲ->li_tv;
	    li = l->lv_ಮೊದಲ->li_ಮುಂದಿನ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	initial = argvars[2];
	ಆದರೆ (l != ಖಾಲಿ && !range_list)
	    li = l->lv_ಮೊದಲ;
    }
    copy_tv(&initial, rettv);

    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, rettv);

    prev_locked = l->lv_ಬೀಗ;
    l->lv_ಬೀಗ = VAR_FIXED;  // disallow the list changing here

    ಆಗ (range_list ? range_idx < l->lv_len : li != ಖಾಲಿ)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	ಆದರೆ (range_list)
	{
	    argv[1].v_type = VAR_ಸಂಖ್ಯೆ;
	    argv[1].vval.v_ಸಂಖ್ಯೆ = range_val;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    argv[1] = li->li_tv;

	r = eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, TRUE, argv, 2, fc, rettv);

	ಆದರೆ (argv[0].v_type != VAR_ಸಂಖ್ಯೆ && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	ಆದರೆ (r == FAIL || called_emsg != called_emsg_ಆರಂಭ)
	    ನಿಲ್ಲಿಸು;

	// advance to the next item
	ಆದರೆ (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    li = li->li_ಮುಂದಿನ;
    }

    ಆದರೆ (fc != ಖಾಲಿ)
	remove_funccal();

    l->lv_ಬೀಗ = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    ಶೂನ್ಯ
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    ಆದರೆ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    ಇಲ್ಲದಿದ್ದರೆ
	func_name = tv_get_string_strict(&argvars[1]);
    ಆದರೆ (func_name == ಖಾಲಿ || *func_name == NUL)
    {
	emsg(_(e_missing_function_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    ಇಲ್ಲದಿದ್ದರೆ
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    ಶೂನ್ಯ
f_ತುಂಡು(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_can_ಸೂಚ್ಯಂಕ(&argvars[0], TRUE, FALSE) != OK)
	ಹಿಂತಿರುಗಿ;

    copy_tv(argvars, rettv);
    eval_ಸೂಚ್ಯಂಕ_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? ಖಾಲಿ : argvars + 2,
	    TRUE, ಖಾಲಿ, 0, FALSE);
}

#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_EVAL)
