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
ساکن list_T		*پہلا_list = تہی;	// list of all lists

#تعریف_کرو FOR_ALL_WATCHERS(l, lw) \
    برائے ((lw) = (l)->lv_watch; (lw) != تہی; (lw) = (lw)->lw_اگلا)

ساکن خالی list_free_شے(list_T *l, listitem_T *شے);

/*
 * Add a watcher to a list.
 */
    خالی
list_جوڑو_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_اگلا = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    خالی
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	اگر (lw == lwrem)
	{
	    *lwp = lw->lw_اگلا;
	    توڑو;
	}
	lwp = &lw->lw_اگلا;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    ساکن خالی
list_fix_watch(list_T *l, listitem_T *شے)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	اگر (lw->lw_شے == شے)
	    lw->lw_شے = شے->li_اگلا;
}

    ساکن خالی
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    اگر (پہلا_list != تہی)
	پہلا_list->lv_مستعمل_پچھلا = l;
    l->lv_مستعمل_پچھلا = تہی;
    l->lv_مستعمل_اگلا = پہلا_list;
    پہلا_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(خالی)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    اگر (l != تہی)
	list_init(l);
    واپس l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	واپس تہی;
#اگر_ختم
    واپس (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_اشیاء(صحیح گنتی)
{
    list_T	*l;

    اگر (گنتی > 0
	    && (size_t)گنتی > (سائز_MAX - حجم(list_T)) / حجم(listitem_T))
	واپس تہی;
    l = (list_T *)alloc_clear(حجم(list_T) + گنتی * حجم(listitem_T));
    اگر (l == تہی)
	واپس تہی;

    list_init(l);

    اگر (گنتی <= 0)
	واپس l;

    listitem_T	*li = (listitem_T *)(l + 1);
    صحیح		i;

    l->lv_len = گنتی;
    l->lv_with_اشیاء = گنتی;
    l->lv_پہلا = li;
    l->lv_u.mat.lv_آخری = li + گنتی - 1;
    برائے (i = 0; i < گنتی; ++i)
    {
	اگر (i == 0)
	    li->li_پچھلا = تہی;
	ورنہ
	    li->li_پچھلا = li - 1;
	اگر (i == گنتی - 1)
	    li->li_اگلا = تہی;
	ورنہ
	    li->li_اگلا = li + 1;
	++li;
    }

    واپس l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    خالی
list_سیٹ_شے(list_T *l, صحیح idx, typval_T *tv)
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

    اگر (l == تہی)
	واپس FAIL;

    rettv->v_تالا = 0;
    rettv_list_سیٹ(rettv, l);
    واپس OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    صحیح
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	واپس FAIL;
#اگر_ختم
    واپس rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    خالی
rettv_list_سیٹ(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    اگر (l != تہی)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    خالی
list_unref(list_T *l)
{
    اگر (l != تہی && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    ساکن خالی
list_free_contents(list_T *l)
{
    listitem_T *شے;

    اگر (l->lv_پہلا != &range_list_شے)
	برائے (شے = l->lv_پہلا; شے != تہی; شے = l->lv_پہلا)
	{
	    // Remove the item before deleting it.
	    l->lv_پہلا = شے->li_اگلا;
	    clear_tv(&شے->li_tv);
	    list_free_شے(l, شے);
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

    برائے (ll = پہلا_list; ll != تہی; ll = ll->lv_مستعمل_اگلا)
	اگر ((ll->lv_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب)
						      && ll->lv_watch == تہی)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    واپس did_free;
}

    ساکن خالی
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    اگر (l->lv_مستعمل_پچھلا == تہی)
	پہلا_list = l->lv_مستعمل_اگلا;
    ورنہ
	l->lv_مستعمل_پچھلا->lv_مستعمل_اگلا = l->lv_مستعمل_اگلا;
    اگر (l->lv_مستعمل_اگلا != تہی)
	l->lv_مستعمل_اگلا->lv_مستعمل_پچھلا = l->lv_مستعمل_پچھلا;

    free_type(l->lv_type);
    vim_free(l);
}

    خالی
list_free_اشیاء(صحیح copyID)
{
    list_T	*ll, *ll_اگلا;

    برائے (ll = پہلا_list; ll != تہی; ll = ll_اگلا)
    {
	ll_اگلا = ll->lv_مستعمل_اگلا;
	اگر ((ll->lv_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب)
						      && ll->lv_watch == تہی)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    خالی
list_free(list_T *l)
{
    اگر (اندر_free_unref_اشیاء)
	واپس;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(خالی)
{
    واپس ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    ساکن خالی
list_free_شے(list_T *l, listitem_T *شے)
{
    اگر (l->lv_with_اشیاء == 0 || شے < (listitem_T *)l
			   || شے >= (listitem_T *)(l + 1) + l->lv_with_اشیاء)
	vim_free(شے);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    خالی
listitem_free(list_T *l, listitem_T *شے)
{
    clear_tv(&شے->li_tv);
    list_free_شے(l, شے);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    خالی
listitem_remove(list_T *l, listitem_T *شے)
{
    vimlist_remove(l, شے, شے);
    listitem_free(l, شے);
}

/*
 * Get the number of items in a list.
 */
    طویل
list_len(list_T *l)
{
    اگر (l == تہی)
	واپس 0L;
    واپس l->lv_len;
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
	واپس TRUE;
    اگر (list_len(l1) != list_len(l2))
	واپس FALSE;
    اگر (list_len(l1) == 0)
	// empty and NULL list are considered equal
	واپس TRUE;
    اگر (l1 == تہی || l2 == تہی)
	واپس FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    برائے (item1 = l1->lv_پہلا, item2 = l2->lv_پہلا;
	    item1 != تہی && item2 != تہی;
			       item1 = item1->li_اگلا, item2 = item2->li_اگلا)
	اگر (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    واپس FALSE;
    واپس item1 == تہی && item2 == تہی;
}

/*
 * Locate item with index "n" in list "l" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    listitem_T *
list_ڈھونڈو(list_T *l, طویل n)
{
    listitem_T	*شے;
    طویل	idx;

    اگر (l == تہی)
	واپس تہی;

    // Negative index is relative to the end.
    اگر (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    اگر (n < 0 || n >= l->lv_len)
	واپس تہی;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    اگر (n >= l->lv_len)
	واپس تہی;

    // When there is a cached index may start search from there.
    اگر (l->lv_u.mat.lv_idx_شے != تہی)
    {
	اگر (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    شے = l->lv_پہلا;
	    idx = 0;
	}
	ورنہ اگر (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    شے = l->lv_u.mat.lv_آخری;
	    idx = l->lv_len - 1;
	}
	ورنہ
	{
	    // closest to the cached index
	    شے = l->lv_u.mat.lv_idx_شے;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    ورنہ
    {
	اگر (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    شے = l->lv_پہلا;
	    idx = 0;
	}
	ورنہ
	{
	    // closest to the end of the list
	    شے = l->lv_u.mat.lv_آخری;
	    idx = l->lv_len - 1;
	}
    }

    جب (n > idx)
    {
	// search forward
	شے = شے->li_اگلا;
	++idx;
    }
    جب (n < idx)
    {
	// search backward
	شے = شے->li_پچھلا;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_شے = شے;

    واپس شے;
}

/*
 * Get list item "l[idx]" as a number.
 */
    طویل
list_ڈھونڈو_nr(
    list_T	*l,
    طویل	idx,
    صحیح		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    اگر (l != تہی && l->lv_پہلا == &range_list_شے)
    {
	طویل	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	اگر (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	اگر (n < 0 || n >= l->lv_len)
	{
	    اگر (errorp != تہی)
		*errorp = TRUE;
	    واپس -1L;
	}

	واپس l->lv_u.nonmat.lv_آغاز + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_ڈھونڈو(l, idx);
    اگر (li == تہی)
    {
	اگر (errorp != تہی)
	    *errorp = TRUE;
	واپس -1L;
    }
    واپس (طویل)tv_get_نمبر_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_ڈھونڈو_str(list_T *l, طویل idx)
{
    listitem_T	*li;

    li = list_ڈھونڈو(l, idx - 1);
    اگر (li == تہی)
    {
	semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), idx);
	واپس تہی;
    }
    واپس tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_ڈھونڈو_اشاریہ(list_T *l, طویل *idx)
{
    listitem_T *li = list_ڈھونڈو(l, *idx);

    اگر (li != تہی)
	واپس li;

    اگر (*idx < 0)
    {
	*idx = 0;
	li = list_ڈھونڈو(l, *idx);
    }
    واپس li;
}

/*
 * Locate "item" list "l" and return its index.
 * Returns -1 when "item" is not in the list.
 */
    طویل
list_idx_کا_شے(list_T *l, listitem_T *شے)
{
    طویل	idx = 0;
    listitem_T	*li;

    اگر (l == تہی)
	واپس -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    برائے (li = l->lv_پہلا; li != تہی && li != شے; li = li->li_اگلا)
	++idx;
    اگر (li == تہی)
	واپس -1;
    واپس idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    خالی
list_append(list_T *l, listitem_T *شے)
{
    CHECK_LIST_MATERIALIZE(l);
    اگر (l->lv_u.mat.lv_آخری == تہی)
    {
	// empty list
	l->lv_پہلا = شے;
	شے->li_پچھلا = تہی;
    }
    ورنہ
    {
	l->lv_u.mat.lv_آخری->li_اگلا = شے;
	شے->li_پچھلا = l->lv_u.mat.lv_آخری;
    }
    l->lv_u.mat.lv_آخری = شے;
    ++l->lv_len;
    شے->li_اگلا = تہی;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    صحیح
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    اگر (l->lv_type != تہی && l->lv_type->tt_member != تہی
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      تہی, 0) == FAIL)
	واپس FAIL;
    li = listitem_alloc();
    اگر (li == تہی)
	واپس FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    واپس OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    ساکن صحیح
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    اگر (li == تہی)
	واپس FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    واپس OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    صحیح
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    اگر (li == تہی)
	واپس FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_تالا = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    واپس OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    صحیح
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    اگر (li == تہی)
	واپس FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_تالا = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    واپس OK;
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

    اگر (li == تہی)
	واپس FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_تالا = 0;
    اگر (str == تہی)
	li->li_tv.vval.v_string = تہی;
    ورنہ اگر ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == تہی)
	واپس FAIL;
    واپس OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    صحیح
list_append_نمبر(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    اگر (li == تہی)
	واپس FAIL;
    li->li_tv.v_type = VAR_نمبر;
    li->li_tv.v_تالا = 0;
    li->li_tv.vval.v_نمبر = n;
    list_append(l, li);
    واپس OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    صحیح
list_داخل_کرو_tv(list_T *l, typval_T *tv, listitem_T *شے)
{
    listitem_T	*ni;

    اگر (l->lv_type != تہی && l->lv_type->tt_member != تہی
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      تہی, 0) == FAIL)
	واپس FAIL;
    ni = listitem_alloc();
    اگر (ni == تہی)
	واپس FAIL;
    copy_tv(tv, &ni->li_tv);
    list_داخل_کرو(l, ni, شے);
    واپس OK;
}

    خالی
list_داخل_کرو(list_T *l, listitem_T *ni, listitem_T *شے)
{
    CHECK_LIST_MATERIALIZE(l);
    اگر (شے == تہی)
	// Append new item at end of list.
	list_append(l, ni);
    ورنہ
    {
	// Insert new item before existing item.
	ni->li_پچھلا = شے->li_پچھلا;
	ni->li_اگلا = شے;
	اگر (شے->li_پچھلا == تہی)
	{
	    l->lv_پہلا = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	ورنہ
	{
	    شے->li_پچھلا->li_اگلا = ni;
	    l->lv_u.mat.lv_idx_شے = تہی;
	}
	شے->li_پچھلا = ni;
	++l->lv_len;
    }
}

/*
 * Append a new empty item into "l" based on the list item type.  Used when
 * adding a new element to a List in Vim9script by using the current list
 * length as the index.
 */
    ساکن صحیح
list_append_new_شے(list_T *l)
{
    typval_T	tv;

    اگر (l->lv_type != تہی && l->lv_type->tt_member != تہی)
	tv.v_type = l->lv_type->tt_member->tt_type;
    ورنہ
	tv.v_type = VAR_نمبر;
    tv.v_تالا = 0;

    انتخاب (tv.v_type)
    {
	حالت VAR_BOOL: tv.vval.v_نمبر = VVAL_FALSE; توڑو;
	حالت VAR_SPECIAL: tv.vval.v_نمبر = 0; توڑو;
	حالت VAR_نمبر: tv.vval.v_نمبر = 0; توڑو;
	حالت VAR_FLOAT: tv.vval.v_float = 0; توڑو;
	حالت VAR_STRING: tv.vval.v_string = تہی; توڑو;
	حالت VAR_BLOB: tv.vval.v_blob = blob_alloc(); توڑو;
	حالت VAR_FUNC: tv.vval.v_string = تہی; توڑو;
	حالت VAR_PARTIAL: tv.vval.v_partial = تہی; توڑو;
	حالت VAR_LIST: tv.vval.v_list = list_alloc(); توڑو;
	حالت VAR_DICT: tv.vval.v_dict = dict_alloc(); توڑو;
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	حالت VAR_کام: tv.vval.v_کام = تہی; توڑو;
	حالت VAR_CHANNEL: tv.vval.v_channel = تہی; توڑو;
#اگر_ختم
	حالت VAR_CLASS:
	    اگر (l->lv_type != تہی && l->lv_type->tt_member != تہی)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    ورنہ
		tv.vval.v_class = تہی;
	    توڑو;
	حالت VAR_OBJECT:
	    اگر (l->lv_type != تہی && l->lv_type->tt_member != تہی)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    ورنہ
		tv.vval.v_object = تہی;
	    توڑو;
	حالت VAR_TYPEALIAS: tv.vval.v_typealias = تہی; توڑو;
	حالت VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); توڑو;
	طےشدہ:
	    tv.v_type = VAR_نمبر;
	    tv.vval.v_نمبر = 0;
	    توڑو;
    }

    اگر (l->lv_type != تہی && l->lv_type->tt_member != تہی)
	سیٹ_tv_type(&tv, l->lv_type->tt_member);

    واپس list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_اشاریہ_one(list_T *l, طویل *n1, صحیح can_append, صحیح quiet)
{
    طویل	orig_n1 = *n1;
    listitem_T	*li = list_ڈھونڈو_اشاریہ(l, n1);

    اگر (li != تہی)
	واپس li;

    // Vim9: Allow for adding an item at the end.
    اگر (can_append && اندر_vim9script()
	    && *n1 == l->lv_len && l->lv_تالا == 0)
    {
	اگر (list_append_new_شے(l) == FAIL)
	    واپس تہی;
	li = list_ڈھونڈو_اشاریہ(l, n1);
    }
    اگر (li == تہی)
    {
	اگر (!quiet)
	    semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), orig_n1);
	واپس تہی;
    }
    واپس li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    صحیح
check_range_اشاریہ_two(
	list_T	    *l,
	طویل	    *n1,
	listitem_T  *li1,
	طویل	    *n2,
	صحیح	    quiet)
{
    اگر (*n2 < 0)
    {
	listitem_T	*ni = list_ڈھونڈو(l, *n2);

	اگر (ni == تہی)
	{
	    اگر (!quiet)
		semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), *n2);
	    واپس FAIL;
	}
	*n2 = list_idx_کا_شے(l, ni);
    }

    // Check that n2 isn't before n1.
    اگر (*n1 < 0)
	*n1 = list_idx_کا_شے(l, li1);
    اگر (*n2 < *n1)
    {
	اگر (!quiet)
	    semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), *n2);
	واپس FAIL;
    }
    واپس OK;
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
list_تفویض_range(
	list_T	    *dest,
	list_T	    *src,
	طویل	    idx1_arg,
	طویل	    idx2,
	صحیح	    خالی_خانے_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    طویل	idx1 = idx1_arg;
    listitem_T	*پہلا_li = list_ڈھونڈو_اشاریہ(dest, &idx1);
    طویل	idx;
    type_T	*member_type = تہی;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = پہلا_li;
    برائے (src_li = src->lv_پہلا; src_li != تہی && dest_li != تہی; )
    {
	اگر (قدر_check_تالا(dest_li->li_tv.v_تالا, varname, FALSE))
	    واپس FAIL;
	src_li = src_li->li_اگلا;
	اگر (src_li == تہی || (!خالی_خانے_idx2 && idx2 == idx))
	    توڑو;
	dest_li = dest_li->li_اگلا;
	++idx;
    }

    اگر (اندر_vim9script() && dest->lv_type != تہی
					   && dest->lv_type->tt_member != تہی)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = پہلا_li;
    برائے (src_li = src->lv_پہلا; src_li != تہی; )
    {
	اگر (op != تہی && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	ورنہ
	{
	    اگر (member_type != تہی
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      تہی, 0) == FAIL)
		واپس FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_اگلا;
	اگر (src_li == تہی || (!خالی_خانے_idx2 && idx2 == idx))
	    توڑو;
	اگر (dest_li->li_اگلا == تہی)
	{
	    // Need to add an empty item.
	    اگر (list_append_نمبر(dest, 0) == FAIL)
	    {
		src_li = تہی;
		توڑو;
	    }
	}
	dest_li = dest_li->li_اگلا;
	++idx;
    }
    اگر (src_li != تہی)
    {
	emsg(_(e_list_قدر_has_مزید_اشیاء_than_targets));
	واپس FAIL;
    }
    اگر (خالی_خانے_idx2
	    ? (dest_li != تہی && dest_li->li_اگلا != تہی)
	    : idx != idx2)
    {
	emsg(_(e_list_قدر_does_not_have_enough_اشیاء));
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    ساکن خالی
list_flatten(list_T *list, listitem_T *پہلا, طویل maxitems, طویل maxdepth)
{
    listitem_T	*شے;
    صحیح		مکمل = 0;

    اگر (maxdepth == 0)
	واپس;
    CHECK_LIST_MATERIALIZE(list);
    اگر (پہلا == تہی)
	شے = list->lv_پہلا;
    ورنہ
	شے = پہلا;

    جب (شے != تہی && مکمل < maxitems)
    {
	listitem_T	*اگلا = شے->li_اگلا;

	fast_breakcheck();
	اگر (got_int)
	    واپس;

	اگر (شے->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = شے->li_tv.vval.v_list;

	    vimlist_remove(list, شے, شے);
	    اگر (list_extend(list, itemlist, اگلا) == FAIL)
	    {
		list_free_شے(list, شے);
		واپس;
	    }

	    اگر (maxdepth > 0)
		list_flatten(list, شے->li_پچھلا == تہی
				     ? list->lv_پہلا : شے->li_پچھلا->li_اگلا,
				itemlist->lv_len, maxdepth - 1);
	    clear_tv(&شے->li_tv);
	    list_free_شے(list, شے);
	}

	++مکمل;
	شے = اگلا;
    }
}

/*
 * "flatten()" and "flattennew()" functions
 */
    ساکن خالی
flatten_common(typval_T *argvars, typval_T *rettv, صحیح make_copy)
{
    list_T  *l;
    طویل    maxdepth;
    صحیح	    error = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    اگر (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_دلیل_کا_str_must_be_list), "flatten()");
	واپس;
    }

    اگر (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    ورنہ
    {
	maxdepth = (طویل)tv_get_نمبر_chk(&argvars[1], &error);
	اگر (error)
	    واپس;
	اگر (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_نمبر));
	    واپس;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    اگر (l == تہی)
	واپس;

    اگر (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	اگر (l == تہی)
	    واپس;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = تہی;
    }
    ورنہ
    {
	اگر (قدر_check_تالا(l->lv_تالا,
				     (char_u *)N_("flatten() argument"), TRUE))
	    واپس;
	++l->lv_refcount;
    }

    list_flatten(l, تہی, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    خالی
f_flatten(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script())
	emsg(_(e_cannot_use_flatten_اندر_vim9_script));
    ورنہ
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    خالی
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    خالی
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;
    اگر (l == تہی)
	واپس;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    برائے (idx = 0, li = l->lv_پہلا; li != تہی; li = li->li_اگلا, ++idx)
    {
	list_T	    *l2 = list_alloc();

	اگر (l2 == تہی)
	    توڑو;
	اگر (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    توڑو;
	}
	اگر (list_append_نمبر(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    توڑو;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    خالی
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;
    اگر (p == تہی)
	واپس;  // null string behaves like an empty string

    برائے (idx = 0; *p != NUL; ++idx)
    {
	صحیح	    len = mb_ptr2len(p);
	list_T	    *l2;

	اگر (len == 0)
	    توڑو;
	l2 = list_alloc();
	اگر (l2 == تہی)
	    توڑو;
	اگر (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    توڑو;
	}
	اگر (list_append_نمبر(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    توڑو;
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
    listitem_T	*شے;
    صحیح		todo;
    listitem_T	*bef_پچھلا;

    // NULL list is equivalent to an empty list: nothing to do.
    اگر (l2 == تہی || l2->lv_len == 0)
	واپس OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_پچھلا = bef == تہی ? تہی : bef->li_پچھلا;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    برائے (شے = l2->lv_پہلا; شے != تہی && --todo >= 0;
				 شے = شے == bef_پچھلا ? bef : شے->li_اگلا)
	اگر (list_داخل_کرو_tv(l1, &شے->li_tv, bef) == FAIL)
	    واپس FAIL;
    واپس OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    صحیح
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    صحیح		len1 = l1 == تہی ? 0 : l1->lv_len;
    صحیح		len2 = l2 == تہی ? 0 : l2->lv_len;
    طویل	totallen = (طویل)len1 + (طویل)len2;
    صحیح		i;
    listitem_T	*شے;

    اگر (totallen == 0)
    {
	l = list_alloc();
	اگر (l == تہی)
	    واپس FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_تالا = 0;
	tv->vval.v_list = l;
	واپس OK;
    }
    اگر (totallen > زیادہ_سے_زیادہ_عدد)
	واپس FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_اشیاء((صحیح)totallen);
    اگر (l == تہی)
	واپس FAIL;

    i = 0;
    اگر (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	برائے (شے = l1->lv_پہلا; شے != تہی && !got_int;
		شے = شے->li_اگلا)
	{
	    typval_T new_tv;

	    copy_tv(&شے->li_tv, &new_tv);
	    list_سیٹ_شے(l, i++, &new_tv);
	}
    }
    اگر (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	برائے (شے = l2->lv_پہلا; شے != تہی && !got_int;
		شے = شے->li_اگلا)
	{
	    typval_T new_tv;

	    copy_tv(&شے->li_tv, &new_tv);
	    list_سیٹ_شے(l, i++, &new_tv);
	}
    }

    ++l->lv_refcount;
    tv->v_type = VAR_LIST;
    tv->v_تالا = 0;
    tv->vval.v_list = l;
    واپس OK;
}

    list_T *
list_قاش(list_T *ol, طویل n1, طویل n2)
{
    listitem_T	*شے;
    list_T	*l;
    طویل	گنتی;
    طویل	idx;

    اگر (n2 < n1)
	واپس list_alloc();
    اگر (n1 > زیادہ_سے_زیادہ_عدد - (n2 - n1) - 1)
	واپس تہی;
    گنتی = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_اشیاء((صحیح)گنتی);
    اگر (l == تہی)
	واپس تہی;

    شے = list_ڈھونڈو(ol, n1);
    برائے (idx = 0; idx < گنتی; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	اگر (شے == تہی)
	{
	    list_free(l);
	    واپس تہی;
	}

	copy_tv(&شے->li_tv, &new_tv);
	list_سیٹ_شے(l, (صحیح)idx, &new_tv);
	شے = شے->li_اگلا;
    }
    واپس l;
}

    صحیح
list_قاش_or_اشاریہ(
	    list_T	*list,
	    صحیح		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    صحیح		exclusive,
	    typval_T	*rettv,
	    صحیح		verbose)
{
    طویل	len = list_len(list);
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
		semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), (طویل)n1_arg);
	    واپس FAIL;
	}
	اگر (اندر_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ورنہ
	    n1 = len;
    }
    اگر (range)
    {
	list_T	*l;

	اگر (n2 < 0)
	    n2 = len + n2;
	ورنہ اگر (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	اگر (exclusive)
	    --n2;
	اگر (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_قاش(list, n1, n2);
	اگر (l == تہی)
	    واپس FAIL;
	clear_tv(rettv);
	rettv_list_سیٹ(rettv, l);
    }
    ورنہ
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	طویل اشاریہ = n1;
	listitem_T *li = check_range_اشاریہ_one(list, &اشاریہ, TRUE, TRUE);
	اگر (li == تہی)
	    واپس FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	اگر (اندر_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != تہی
					&& var1.vval.v_list->lv_type == تہی)
	{
	    اگر (list->lv_type != تہی && list->lv_type->tt_member != تہی)
		سیٹ_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    واپس OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, صحیح deep, صحیح چوٹی, صحیح copyID)
{
    list_T	*copy;
    listitem_T	*شے;
    صحیح		idx = 0;

    اگر (orig == تہی)
	واپس تہی;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_اشیاء(orig->lv_len);
    اگر (copy == تہی)
	واپس تہی;

    اگر (orig->lv_type == تہی || چوٹی || deep)
	copy->lv_type = تہی;
    ورنہ
	copy->lv_type = alloc_type(orig->lv_type);
    اگر (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    برائے (شے = orig->lv_پہلا; شے != تہی && !got_int;
	    شے = شے->li_اگلا)
    {
	typval_T	new_tv;

	اگر (deep)
	{
	    اگر (شے_copy(&شے->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		توڑو;
	}
	ورنہ
	    copy_tv(&شے->li_tv, &new_tv);
	list_سیٹ_شے(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    اگر (شے != تہی)
    {
	list_unref(copy);
	copy = تہی;
    }

    واپس copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    خالی
vimlist_remove(list_T *l, listitem_T *شے, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    برائے (ip = شے; ip != تہی; ip = ip->li_اگلا)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	اگر (ip == item2)
	    توڑو;
    }

    اگر (item2->li_اگلا == تہی)
	l->lv_u.mat.lv_آخری = شے->li_پچھلا;
    ورنہ
	item2->li_اگلا->li_پچھلا = شے->li_پچھلا;
    اگر (شے->li_پچھلا == تہی)
	l->lv_پہلا = item2->li_اگلا;
    ورنہ
	شے->li_پچھلا->li_اگلا = item2->li_اگلا;
    l->lv_u.mat.lv_idx_شے = تہی;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, صحیح copyID, صحیح restore_copyID)
{
    garray_T	ga;

    اگر (tv->vval.v_list == تہی)
	واپس تہی;
    ga_init2(&ga, حجم(حرف), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    اگر (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_ڈیٹا);
	واپس تہی;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    واپس (char_u *)ga.ga_ڈیٹا;
}

قسمتعریف ساخت join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ساکن صحیح
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
    طویل	sumlen = 0;
    صحیح		پہلا = TRUE;
    صحیح		پچھلا_تالا;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    listitem_T	*شے;
    string_T	s;
    size_t	seplen;

    // Stringify each item in the list.
    CHECK_LIST_MATERIALIZE(l);
    // Lock the list, the string() method of an object item could remove the
    // item the loop is standing on.
    پچھلا_تالا = l->lv_تالا;
    اگر (l->lv_تالا == 0)
	l->lv_تالا = VAR_LOCKED;
    برائے (شے = l->lv_پہلا; شے != تہی && !got_int; شے = شے->li_اگلا)
    {
	s.string = echo_string_core(&شے->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	اگر (s.string == تہی)
	{
	    l->lv_تالا = پچھلا_تالا;
	    واپس FAIL;
	}

	s.لمبائی = STRLEN(s.string);
	sumlen += (طویل)s.لمبائی;

	(خالی)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_ڈیٹا) + (join_gap->ga_len++);
	اگر (tofree != تہی || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.لمبائی = s.لمبائی;
	    p->tofree = tofree;
	}
	ورنہ
	{
	    p->s.string = vim_strnsave(s.string, s.لمبائی);
	    p->s.لمبائی = s.لمبائی;
	    p->tofree = p->s.string;
	}

	سطر_breakcheck();
	اگر (did_echo_string_emsg)  // recursion error, bail out
	    توڑو;
    }
    l->lv_تالا = پچھلا_تالا;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    اگر (join_gap->ga_len >= 2)
	sumlen += (طویل)seplen * (join_gap->ga_len - 1);
    اگر (sumlen > زیادہ_سے_زیادہ_عدد - 2 || ga_grow(gap, (صحیح)sumlen + 2) == FAIL)
	واپس FAIL;

    برائے (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	اگر (پہلا)
	    پہلا = FALSE;
	ورنہ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_ڈیٹا) + i;

	اگر (p->s.string != تہی)
	    ga_concat_len(gap, p->s.string, p->s.لمبائی);
	سطر_breakcheck();
    }

    واپس OK;
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
	واپس OK; // nothing to do
    ga_init2(&join_ga, حجم(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    اگر (join_ga.ga_ڈیٹا == تہی)
	واپس retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_ڈیٹا;
    برائے (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    واپس retval;
}

/*
 * "join()" function
 */
    خالی
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    اگر (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	واپس;

    اگر ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == تہی)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == تہی))
	واپس;

    اگر (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    ورنہ
	sep = tv_get_string_chk(&argvars[1]);

    اگر (sep != تہی)
    {
	ga_init2(&ga, حجم(حرف), 80);
	اگر (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	ورنہ
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_ڈیٹا;
    }
    ورنہ
	rettv->vval.v_string = تہی;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    صحیح
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحیح do_error)
{
    صحیح		evaluate = evalarg == تہی ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = تہی;
    typval_T	tv;
    listitem_T	*شے;
    صحیح		vim9script = اندر_vim9script();
    صحیح		had_comma;

    اگر (evaluate)
    {
	l = list_alloc();
	اگر (l == تہی)
	    واپس FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    جب (**arg != ']' && **arg != NUL)
    {
	اگر (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    جاؤ failret;
	اگر (check_typval_is_قدر(&tv) == FAIL)
	{
	    اگر (evaluate)
		clear_tv(&tv);
	    جاؤ failret;
	}
	اگر (evaluate)
	{
	    شے = listitem_alloc();
	    اگر (شے != تہی)
	    {
		شے->li_tv = tv;
		شے->li_tv.v_تالا = 0;
		list_append(l, شے);
	    }
	    ورنہ
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
		جاؤ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg == ']')
	    توڑو;

	اگر (!had_comma)
	{
	    اگر (do_error)
	    {
		اگر (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ورنہ
		    semsg(_(e_missing_comma_اندر_list_str), *arg);
	    }
	    جاؤ failret;
	}
    }

    اگر (**arg != ']')
    {
	اگر (do_error)
	    semsg(_(e_missing_انتہا_کا_list_rsb_str), *arg);
failret:
	اگر (evaluate)
	    list_free(l);
	واپس FAIL;
    }

    *arg += 1;
    اگر (evaluate)
	rettv_list_سیٹ(rettv, l);

    واپس OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    صحیح
write_list(FILE *fd, list_T *list, صحیح ثنائی)
{
    listitem_T	*li;
    صحیح		c;
    صحیح		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_اشیاء(list, li)
    {
	برائے (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    اگر (*s == '\n')
		c = putc(NUL, fd);
	    ورنہ
		c = putc(*s, fd);
	    اگر (c == EOF)
	    {
		ret = FAIL;
		توڑو;
	    }
	}
	اگر (!ثنائی || li->li_اگلا != تہی)
	    اگر (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		توڑو;
	    }
	اگر (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    توڑو;
	}
    }
    واپس ret;
}

/*
 * Initialize a static list with 10 items.
 */
    خالی
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    صحیح	    i;

    CLEAR_اشارہ_گر(sl);
    l->lv_پہلا = &sl->sl_اشیاء[0];
    l->lv_u.mat.lv_آخری = &sl->sl_اشیاء[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_تالا = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    برائے (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_اشیاء[i];

	اگر (i == 0)
	    li->li_پچھلا = تہی;
	ورنہ
	    li->li_پچھلا = li - 1;
	اگر (i == 9)
	    li->li_اگلا = تہی;
	ورنہ
	    li->li_اگلا = li + 1;
    }
}

/*
 * "list2str()" function
 */
    خالی
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    صحیح		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	واپس;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	واپس;

    l = argvars[0].vval.v_list;
    اگر (l == تہی)
	واپس;  // empty list results in empty string

    اگر (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (صحیح)tv_get_bool_chk(&argvars[1], تہی);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    اگر (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	صحیح	(*char2bytes)(صحیح, char_u *);

	اگر (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	ورنہ
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_اشیاء(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_نمبر(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    ورنہ
    {
	طویل len = (طویل)list_len(l) + 1;

	اگر (len <= زیادہ_سے_زیادہ_عدد && ga_grow(&ga, (صحیح)len) == OK)
	{
	    FOR_ALL_LIST_اشیاء(l, li)
		ga_append(&ga, tv_get_نمبر(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_ڈیٹا;
}

/*
 * "list2tuple()" function
 */
    خالی
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = تہی;

    اگر (check_for_list_arg(argvars, 0) == FAIL)
	واپس;

    l = argvars[0].vval.v_list;
    اگر (l == تہی)
	واپس;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    اگر (rettv_tuple_سیٹ_with_اشیاء(rettv, list_len(l)) == FAIL)
	واپس;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_اشیاء(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_شے(tuple, TUPLE_LEN(tuple)));
	tuple->tv_اشیاء.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    خالی
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (check_for_tuple_arg(argvars, 0) == FAIL)
	واپس;

    tuple = argvars[0].vval.v_tuple;
    اگر (tuple == تہی)
	واپس;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    برائے (صحیح i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_شے(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    ساکن خالی
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*شے, *item2;
    listitem_T	*li;
    صحیح		error = FALSE;
    طویل	idx;
    طویل	انتہا;
    صحیح		cnt = 0;
    list_T	*rl;

    اگر ((l = argvars[0].vval.v_list) == تہی
			     || قدر_check_تالا(l->lv_تالا, arg_errmsg, TRUE))
	واپس;

    idx = (طویل)tv_get_نمبر_chk(&argvars[1], &error);
    اگر (error)
	واپس;		// type error: do nothing, errmsg already given

    اگر ((شے = list_ڈھونڈو(l, idx)) == تہی)
    {
	semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), idx);
	واپس;
    }

    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, شے, شے);
	*rettv = شے->li_tv;
	list_free_شے(l, شے);
	واپس;
    }

    // Remove range of items, return list with values.
    انتہا = (طویل)tv_get_نمبر_chk(&argvars[2], &error);
    اگر (error)
	واپس;		// type error: do nothing

    اگر ((item2 = list_ڈھونڈو(l, انتہا)) == تہی)
    {
	semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), انتہا);
	واپس;
    }

    برائے (li = شے; li != تہی; li = li->li_اگلا)
    {
	++cnt;
	اگر (li == item2)
	    توڑو;
    }
    اگر (li == تہی)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	واپس;
    }

    vimlist_remove(l, شے, item2);
    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    rl = rettv->vval.v_list;

    اگر (l->lv_with_اشیاء > 0)
    {
	// need to copy the list items and move the value
	جب (شے != تہی)
	{
	    li = listitem_alloc();
	    اگر (li == تہی)
		واپس;
	    li->li_tv = شے->li_tv;
	    init_tv(&شے->li_tv);
	    list_append(rl, li);
	    اگر (شے == item2)
		توڑو;
	    شے = شے->li_اگلا;
	}
    }
    ورنہ
    {
	rl->lv_پہلا = شے;
	rl->lv_u.mat.lv_آخری = item2;
	شے->li_پچھلا = تہی;
	item2->li_اگلا = تہی;
	rl->lv_len = cnt;
    }
}

ساکن صحیح شے_compare(ثابت خالی *s1, ثابت خالی *s2);
ساکن صحیح شے_compare2(ثابت خالی *s1, ثابت خالی *s2);

// struct used in the array that's given to qsort()
قسمتعریف ساخت
{
    listitem_T	*شے;
    صحیح		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    اتحاد {
	varnumber_T	inum;	// for "N"
	دوہرا		fnum;	// for "n" and "f"
    } کلید;
} ترتیب_دو_شے_T;

// struct storing information about current sort
قسمتعریف ساخت
{
    صحیح		شے_compare_ic;
    صحیح		شے_compare_lc;
    صحیح		شے_compare_numeric;
    صحیح		شے_compare_numbers;
    صحیح		شے_compare_float;
    char_u	*شے_compare_func;
    partial_T	*شے_compare_partial;
    dict_T	*شے_compare_selfdict;
    صحیح		شے_compare_func_err;
    صحیح		شے_compare_keep_zero;
    صحیح		شے_compare_کلیدیں_ready;  // ptrs[].key is precomputed
} sortinfo_T;
ساکن sortinfo_T	*sortinfo = تہی;
#تعریف_کرو شے_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    ساکن صحیح
شے_compare(ثابت خالی *s1, ثابت خالی *s2)
{
    ترتیب_دو_شے_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = تہی, *tofree2 = تہی;
    صحیح		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (ترتیب_دو_شے_T *)s1;
    si2 = (ترتیب_دو_شے_T *)s2;
    tv1 = &si1->شے->li_tv;
    tv2 = &si2->شے->li_tv;

    اگر (sortinfo->شے_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->شے_compare_کلیدیں_ready
					? si1->کلید.inum : tv_مقصد_نمبر(tv1);
	varnumber_T	v2 = sortinfo->شے_compare_کلیدیں_ready
					? si2->کلید.inum : tv_مقصد_نمبر(tv2);

	واپس v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    اگر (sortinfo->شے_compare_float)
    {
	float_T	v1 = sortinfo->شے_compare_کلیدیں_ready
					? si1->کلید.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->شے_compare_کلیدیں_ready
					? si2->کلید.fnum : tv_get_float(tv2);

	واپس v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    اگر (sortinfo->شے_compare_numeric && sortinfo->شے_compare_کلیدیں_ready)
    {
	دوہرا	n1 = si1->کلید.fnum;
	دوہرا	n2 = si2->کلید.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	اگر (res == 0 && !sortinfo->شے_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	واپس res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    اگر (tv1->v_type == VAR_STRING)
    {
	اگر (tv2->v_type != VAR_STRING || sortinfo->شے_compare_numeric)
	    p1 = (char_u *)"'";
	ورنہ
	    p1 = tv1->vval.v_string;
    }
    ورنہ
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    اگر (tv2->v_type == VAR_STRING)
    {
	اگر (tv1->v_type != VAR_STRING || sortinfo->شے_compare_numeric)
	    p2 = (char_u *)"'";
	ورنہ
	    p2 = tv2->vval.v_string;
    }
    ورنہ
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    اگر (p1 == تہی)
	p1 = (char_u *)"";
    اگر (p2 == تہی)
	p2 = (char_u *)"";
    اگر (!sortinfo->شے_compare_numeric)
    {
	اگر (sortinfo->شے_compare_lc)
	    res = strcoll((حرف *)p1, (حرف *)p2);
	ورنہ
	    res = sortinfo->شے_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    ورنہ
    {
	دوہرا n1, n2;
	n1 = strtod((حرف *)p1, (حرف **)&p1);
	n2 = strtod((حرف *)p2, (حرف **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    اگر (res == 0 && !sortinfo->شے_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    واپس res;
}

    ساکن صحیح
شے_compare2(ثابت خالی *s1, ثابت خالی *s2)
{
    ترتیب_دو_شے_T  *si1, *si2;
    صحیح		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->شے_compare_partial;
    funcexe_T	funcexe;
    صحیح		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    اگر (sortinfo->شے_compare_func_err)
	واپس 0;

    si1 = (ترتیب_دو_شے_T *)s1;
    si2 = (ترتیب_دو_شے_T *)s2;

    اگر (partial == تہی)
	func_name = sortinfo->شے_compare_func;
    ورنہ
	func_name = partial_name(partial);

    // Copy the values.  This is needed to be able to set v_lock to VAR_FIXED
    // in the copy without changing the original list items.
    copy_tv(&si1->شے->li_tv, &argv[0]);
    copy_tv(&si2->شے->li_tv, &argv[1]);

    rettv.v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_میدان(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = partial;
    funcexe.fe_selfdict = sortinfo->شے_compare_selfdict;
    res = call_func(func_name, -1, &rettv, 2, argv, &funcexe);
    clear_tv(&argv[0]);
    clear_tv(&argv[1]);

    اگر (res == FAIL || did_emsg > did_emsg_before)
	res = شے_COMPARE_FAIL;
    ورنہ
    {
	varnumber_T	n;

	n = tv_get_نمبر_chk(&rettv, &sortinfo->شے_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    اگر (sortinfo->شے_compare_func_err)
	res = شے_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    اگر (res == 0 && !sortinfo->شے_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    واپس res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    ساکن خالی
ترتیب_دو_compute_کلیدیں(ترتیب_دو_شے_T *ptrs, طویل len, sortinfo_T *info)
{
    طویل	i;

    اگر (info->شے_compare_numbers)
    {
	برائے (i = 0; i < len; ++i)
	    ptrs[i].کلید.inum = tv_مقصد_نمبر(&ptrs[i].شے->li_tv);
    }
    ورنہ اگر (info->شے_compare_float)
    {
	برائے (i = 0; i < len; ++i)
	    ptrs[i].کلید.fnum = tv_get_float(&ptrs[i].شے->li_tv);
    }
    ورنہ // info->item_compare_numeric
    {
	برائے (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].شے->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    اگر (tv->v_type == VAR_STRING)
		ptrs[i].کلید.fnum = 0.0;
	    ورنہ
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = تہی;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].کلید.fnum = p == تہی ? 0.0 : strtod((حرف *)p, تہی);
		vim_free(tofree);
	    }
	}
    }
    info->شے_compare_کلیدیں_ready = TRUE;
}

/*
 * sort() List "l"
 */
    ساکن خالی
do_ترتیب_دو(list_T *l, sortinfo_T *info)
{
    طویل	len;
    ترتیب_دو_شے_T	*ptrs;
    طویل	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(ترتیب_دو_شے_T, len);
    اگر (ptrs == تہی)
	واپس;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_اشیاء(l, li)
    {
	ptrs[i].شے = li;
	ptrs[i].idx = i;
	++i;
    }

    info->شے_compare_func_err = FALSE;
    info->شے_compare_keep_zero = FALSE;
    info->شے_compare_کلیدیں_ready = FALSE;

    اگر (info->شے_compare_func == تہی && info->شے_compare_partial == تہی
	    && (info->شے_compare_numbers || info->شے_compare_float
						|| info->شے_compare_numeric))
	ترتیب_دو_compute_کلیدیں(ptrs, len, info);
    // test the compare function
    اگر ((info->شے_compare_func != تہی
		|| info->شے_compare_partial != تہی)
	    && شے_compare2((خالی *)&ptrs[0], (خالی *)&ptrs[1])
	    == شے_COMPARE_FAIL)
	emsg(_(e_ترتیب_دو_compare_function_failed));
    ورنہ
    {
	// Sort the array with item pointers.
	qsort((خالی *)ptrs, (size_t)len, حجم(ترتیب_دو_شے_T),
		info->شے_compare_func == تہی
		&& info->شے_compare_partial == تہی
		? شے_compare : شے_compare2);

	اگر (!info->شے_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_پہلا = l->lv_u.mat.lv_آخری
		= l->lv_u.mat.lv_idx_شے = تہی;
	    l->lv_len = 0;
	    برائے (i = 0; i < len; ++i)
		list_append(l, ptrs[i].شے);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    ساکن خالی
do_uniq(list_T *l, sortinfo_T *info)
{
    طویل	len;
    ترتیب_دو_شے_T	*ptrs;
    طویل	i = 0;
    listitem_T	*li;
    صحیح	(*شے_compare_func_ptr)(ثابت خالی *, ثابت خالی *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(ترتیب_دو_شے_T, len);
    اگر (ptrs == تہی)
	واپس;

    // f_uniq(): ptrs will be a stack of items to remove
    info->شے_compare_func_err = FALSE;
    info->شے_compare_keep_zero = TRUE;
    شے_compare_func_ptr = info->شے_compare_func != تہی
	|| info->شے_compare_partial != تہی
	? شے_compare2 : شے_compare;

    برائے (li = l->lv_پہلا; li != تہی && li->li_اگلا != تہی;
	    li = li->li_اگلا)
    {
	اگر (شے_compare_func_ptr((خالی *)&li, (خالی *)&li->li_اگلا)
		== 0)
	    ptrs[i++].شے = li;
	اگر (info->شے_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    توڑو;
	}
    }

    اگر (!info->شے_compare_func_err)
    {
	جب (--i >= 0)
	{
	    li = ptrs[i].شے->li_اگلا;
	    ptrs[i].شے->li_اگلا = li->li_اگلا;
	    اگر (li->li_اگلا != تہی)
		li->li_اگلا->li_پچھلا = ptrs[i].شے;
	    ورنہ
		l->lv_u.mat.lv_آخری = ptrs[i].شے;
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
    ساکن صحیح
parse_ترتیب_دو_uniq_دلائل(typval_T *argvars, sortinfo_T *info)
{
    info->شے_compare_ic = FALSE;
    info->شے_compare_lc = FALSE;
    info->شے_compare_numeric = FALSE;
    info->شے_compare_numbers = FALSE;
    info->شے_compare_float = FALSE;
    info->شے_compare_func = تہی;
    info->شے_compare_partial = تہی;
    info->شے_compare_selfdict = تہی;
    info->شے_compare_کلیدیں_ready = FALSE;

    اگر (argvars[1].v_type == VAR_UNKNOWN)
	واپس OK;

    // optional second argument: {func}
    اگر (argvars[1].v_type == VAR_FUNC)
	info->شے_compare_func = argvars[1].vval.v_string;
    ورنہ اگر (argvars[1].v_type == VAR_PARTIAL)
	info->شے_compare_partial = argvars[1].vval.v_partial;
    ورنہ
    {
	صحیح	    error = FALSE;
	صحیح	    nr = 0;

	اگر (argvars[1].v_type == VAR_نمبر)
	{
	    nr = tv_get_نمبر_chk(&argvars[1], &error);
	    اگر (error)
		واپس FAIL;
	    اگر (nr == 1)
		info->شے_compare_ic = TRUE;
	}
	اگر (nr != 1)
	{
	    اگر (argvars[1].v_type != VAR_نمبر)
		info->شے_compare_func = tv_get_string(&argvars[1]);
	    ورنہ اگر (nr != 0)
	    {
		emsg(_(e_invalid_دلیل));
		واپس FAIL;
	    }
	}
	اگر (info->شے_compare_func != تہی)
	{
	    اگر (*info->شے_compare_func == NUL)
	    {
		// empty string means default sort
		info->شے_compare_func = تہی;
	    }
	    ورنہ اگر (STRCMP(info->شے_compare_func, "n") == 0)
	    {
		info->شے_compare_func = تہی;
		info->شے_compare_numeric = TRUE;
	    }
	    ورنہ اگر (STRCMP(info->شے_compare_func, "N") == 0)
	    {
		info->شے_compare_func = تہی;
		info->شے_compare_numbers = TRUE;
	    }
	    ورنہ اگر (STRCMP(info->شے_compare_func, "f") == 0)
	    {
		info->شے_compare_func = تہی;
		info->شے_compare_float = TRUE;
	    }
	    ورنہ اگر (STRCMP(info->شے_compare_func, "i") == 0)
	    {
		info->شے_compare_func = تہی;
		info->شے_compare_ic = TRUE;
	    }
	    ورنہ اگر (STRCMP(info->شے_compare_func, "l") == 0)
	    {
		info->شے_compare_func = تہی;
		info->شے_compare_lc = TRUE;
	    }
	}
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	اگر (check_for_dict_arg(argvars, 2) == FAIL)
	    واپس FAIL;
	info->شے_compare_selfdict = argvars[2].vval.v_dict;
    }

    واپس OK;
}

/*
 * "sort()" or "uniq()" function
 */
    ساکن خالی
do_ترتیب_دو_uniq(typval_T *argvars, typval_T *rettv, صحیح ترتیب_دو)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    طویل	len;

    اگر (اندر_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	واپس;

    اگر (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_دلیل_کا_str_must_be_list), ترتیب_دو ? "sort()" : "uniq()");
	واپس;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    اگر (l != تہی && قدر_check_تالا(l->lv_تالا,
		(char_u *)(ترتیب_دو ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	جاؤ theend;
    rettv_list_سیٹ(rettv, l);
    اگر (l == تہی)
	جاؤ theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    اگر (len <= 1)
	جاؤ theend;	// short list sorts pretty quickly

    اگر (parse_ترتیب_دو_uniq_دلائل(argvars, &info) == FAIL)
	جاؤ theend;

    اگر (ترتیب_دو)
	do_ترتیب_دو(l, &info);
    ورنہ
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    خالی
f_ترتیب_دو(typval_T *argvars, typval_T *rettv)
{
    do_ترتیب_دو_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    خالی
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_ترتیب_دو_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    صحیح
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*اظہاریہ,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	صحیح		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    صحیح		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    اگر (filtermap == FILTERMAP_FOREACH && اظہاریہ->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(اظہاریہ->vval.v_string);
	اگر (!did_emsg)
	    retval = OK;
	جاؤ theend;
    }

    argv[0] = *get_vim_var_tv(VV_کلید);
    argv[1] = *get_vim_var_tv(VV_VAL);
    اگر (eval_اظہاریہ_typval(اظہاریہ, FALSE, argv, 2, fc, newtv) == FAIL)
	جاؤ theend;
    اگر (filtermap == FILTERMAP_FILTER)
    {
	صحیح	    error = FALSE;

	// filter(): when expr is zero remove the item
	اگر (اندر_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	ورنہ
	    *remp = (tv_get_نمبر_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	اگر (error)
	    جاؤ theend;
    }
    ورنہ اگر (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    واپس retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    ساکن خالی
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	حرف		*func_name,
	char_u		*arg_errmsg,
	typval_T	*اظہاریہ,
	typval_T	*rettv)
{
    صحیح		پچھلا_تالا;
    list_T	*l_ret = تہی;
    صحیح		idx = 0;
    صحیح		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = تہی;
    }
    اگر (l == تہی || (filtermap == FILTERMAP_FILTER
			    && قدر_check_تالا(l->lv_تالا, arg_errmsg, TRUE)))
	واپس;

    پچھلا_تالا = l->lv_تالا;

    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	اگر (rettv_list_alloc(rettv) == FAIL)
	    واپس;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    سیٹ_vim_var_type(VV_کلید, VAR_نمبر);

    اگر (l->lv_تالا == 0)
	l->lv_تالا = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اظہاریہ_get_funccal(اظہاریہ, &newtv);

    اگر (l->lv_پہلا == &range_list_شے)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_آغاز;
	صحیح		len = l->lv_len;
	صحیح		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	اگر (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_پہلا = تہی;
	    l->lv_u.mat.lv_آخری = تہی;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_شے = تہی;
	}

	برائے (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_نمبر;
	    tv.v_تالا = 0;
	    tv.vval.v_نمبر = val;
	    سیٹ_vim_var_nr(VV_کلید, idx);
	    اگر (filter_map_one(&tv, اظہاریہ, filtermap, fc, &newtv, &rem) == FAIL)
		توڑو;
	    اگر (did_emsg)
	    {
		clear_tv(&newtv);
		توڑو;
	    }
	    اگر (filtermap != FILTERMAP_FOREACH)
	    {
		اگر (filtermap != FILTERMAP_FILTER)
		{
		    اگر (filtermap == FILTERMAP_MAP && argtype != تہی
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			توڑو;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    اگر (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			توڑو;
		}
		ورنہ اگر (!rem)
		{
		    // filter(): append the list item value when not rem
		    اگر (list_append_tv_move(l, &tv) == FAIL)
			توڑو;
		}
	    }

	    val += stride;
	}
    }
    ورنہ
    {
	// Materialized list: loop over the items
	برائے (li = l->lv_پہلا; li != تہی; li = nli)
	{
	    اگر (filtermap == FILTERMAP_MAP && قدر_check_تالا(
			li->li_tv.v_تالا, arg_errmsg, TRUE))
		توڑو;
	    nli = li->li_اگلا;
	    سیٹ_vim_var_nr(VV_کلید, idx);
	    اگر (filter_map_one(&li->li_tv, اظہاریہ, filtermap, fc,
							 &newtv, &rem) == FAIL)
		توڑو;
	    اگر (did_emsg)
	    {
		clear_tv(&newtv);
		توڑو;
	    }
	    اگر (filtermap == FILTERMAP_MAP)
	    {
		اگر (argtype != تہی && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    توڑو;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_تالا = 0;
		li->li_tv = newtv;
	    }
	    ورنہ اگر (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		اگر (list_append_tv_move(l_ret, &newtv) == FAIL)
		    توڑو;
	    }
	    ورنہ اگر (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_تالا = پچھلا_تالا;
    اگر (fc != تہی)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    ساکن خالی
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*اظہاریہ;
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
    صحیح		save_did_emsg;
    type_T	*type = تہی;

    // map(), filter(), foreach() return the first argument, also on failure.
    اگر (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    اگر (اندر_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	واپس;

    اگر (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	واپس;
    }

    اگر (filtermap == FILTERMAP_MAP && اندر_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	اگر (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != تہی)
	    type = argvars[0].vval.v_dict->dv_type;
	ورنہ اگر (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != تہی)
	    type = argvars[0].vval.v_list->lv_type;
    }

    اگر (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	حرف *پیغام;

	اگر (filtermap == FILTERMAP_FOREACH)
	    پیغام = e_دلیل_کا_str_must_be_list_tuple_string_dictionary_or_blob;
	ورنہ
	    پیغام = e_دلیل_کا_str_must_be_list_string_dictionary_or_blob;
	semsg(_(پیغام), func_name);
	واپس;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    اظہاریہ = &argvars[1];
    اگر (اظہاریہ->v_type == VAR_UNKNOWN)
	واپس;

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
						      arg_errmsg, اظہاریہ, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, اظہاریہ,
							    arg_errmsg, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, اظہاریہ, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, اظہاریہ);
    ورنہ // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, اظہاریہ, rettv);

    restore_vimvar(VV_کلید, &save_کلید);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    خالی
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    خالی
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    خالی
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    خالی
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    ساکن خالی
list_جوڑو(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    اگر (l == تہی)
    {
	اگر (اندر_vim9script())
	    emsg(_(e_cannot_جوڑو_مقصد_null_list));
    }
    ورنہ اگر (!قدر_check_تالا(l->lv_تالا,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    خالی
f_جوڑو(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_نمبر = 1; // Default: Failed

    اگر (اندر_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_نمبر_arg(argvars, 1) == FAIL)))
	واپس;

    اگر (argvars[0].v_type == VAR_LIST)
	list_جوڑو(argvars, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_BLOB)
	blob_جوڑو(argvars, rettv);
    ورنہ
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ساکن طویل
list_گنتی(list_T *l, typval_T *needle, طویل idx, صحیح ic)
{
    طویل	n = 0;
    listitem_T	*li;

    اگر (l == تہی)
	واپس 0;

    CHECK_LIST_MATERIALIZE(l);

    اگر (list_len(l) == 0)
	واپس 0;

    li = list_ڈھونڈو(l, idx);
    اگر (li == تہی)
    {
	semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), idx);
	واپس 0;
    }

    برائے ( ; li != تہی; li = li->li_اگلا)
	اگر (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    واپس n;
}

/*
 * "count()" function
 */
    خالی
f_گنتی(typval_T *argvars, typval_T *rettv)
{
    طویل	n = 0;
    صحیح		ic = FALSE;
    صحیح		error = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 3) == FAIL)))
	واپس;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
	ic = (صحیح)tv_get_bool_chk(&argvars[2], &error);

    اگر (!error && argvars[0].v_type == VAR_STRING)
	n = string_گنتی(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    ورنہ اگر (!error && argvars[0].v_type == VAR_LIST)
    {
	طویل idx = 0;

	اگر (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (طویل)tv_get_نمبر_chk(&argvars[3], &error);
	اگر (!error)
	    n = list_گنتی(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    ورنہ اگر (!error && argvars[0].v_type == VAR_TUPLE)
    {
	طویل idx = 0;

	اگر (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (طویل)tv_get_نمبر_chk(&argvars[3], &error);
	اگر (!error)
	    n = tuple_گنتی(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    ورنہ اگر (!error && argvars[0].v_type == VAR_DICT)
    {
	اگر (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_دلیل));
	ورنہ
	    n = dict_گنتی(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    ورنہ اگر (!error)
	semsg(_(e_دلیل_کا_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_نمبر = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ساکن خالی
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	حرف		*func_name,
	char_u		*arg_errmsg,
	صحیح		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*شے;
    طویل	before;
    صحیح		error = FALSE;

    l1 = argvars[0].vval.v_list;
    اگر (l1 == تہی)
    {
	emsg(_(e_cannot_extend_null_list));
	واپس;
    }
    اگر (is_new || !قدر_check_تالا(l1->lv_تالا, arg_errmsg, TRUE))
    {
	اگر (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    اگر (l1 == تہی)
		واپس;
	}

	l2 = argvars[1].vval.v_list;
	اگر (l2 == تہی)
	    جاؤ theend;

	اگر (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (طویل)tv_get_نمبر_chk(&argvars[2], &error);
	    اگر (error)
		جاؤ cleanup;		// type error; errmsg already given
	    اگر (before == l1->lv_len)
		شے = تہی;
	    ورنہ
	    {
		شے = list_ڈھونڈو(l1, before);
		اگر (شے == تہی)
		{
		    semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), before);
		    جاؤ cleanup;
		}
	    }
	}
	ورنہ
	    شے = تہی;
	اگر (type != تہی && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    جاؤ cleanup;
	list_extend(l1, l2, شے);

theend:
	اگر (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_تالا = FALSE;
	}
	ورنہ
	    copy_tv(&argvars[0], rettv);
	واپس;

cleanup:
	اگر (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    ساکن خالی
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, صحیح is_new)
{
    type_T	*type = تہی;
    حرف	*func_name = is_new ? "extendnew()" : "extend()";

    اگر (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	اگر (!is_new && اندر_vim9script() && argvars[0].vval.v_list != تہی)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ورنہ اگر (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	اگر (!is_new && اندر_vim9script() && argvars[0].vval.v_dict != تہی)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ورنہ اگر (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    ورنہ
	semsg(_(e_دلیل_کا_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    خالی
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
    خالی
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    ساکن خالی
list_داخل_کرو_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    طویل	before = 0;
    listitem_T	*شے;
    صحیح		error = FALSE;

    اگر (l == تہی)
    {
	اگر (اندر_vim9script())
	    emsg(_(e_cannot_جوڑو_مقصد_null_list));
	واپس;
    }

    اگر (قدر_check_تالا(l->lv_تالا, (char_u *)N_("insert() argument"), TRUE))
	واپس;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
	before = (طویل)tv_get_نمبر_chk(&argvars[2], &error);
    اگر (error)
	واپس;		// type error; errmsg already given

    اگر (before == l->lv_len)
	شے = تہی;
    ورنہ
    {
	شے = list_ڈھونڈو(l, before);
	اگر (شے == تہی)
	{
	    semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), before);
	    l = تہی;
	}
    }
    اگر (l != تہی)
    {
	(خالی)list_داخل_کرو_tv(l, &argvars[1], شے);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    خالی
f_داخل_کرو(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_نمبر_arg(argvars, 1) == FAIL)
		|| check_for_opt_نمبر_arg(argvars, 2) == FAIL))
	واپس;

    اگر (argvars[0].v_type == VAR_BLOB)
	blob_داخل_کرو_func(argvars, rettv);
    ورنہ اگر (argvars[0].v_type != VAR_LIST)
	semsg(_(e_دلیل_کا_str_must_be_list_or_blob), "insert()");
    ورنہ
	list_داخل_کرو_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    خالی
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    اگر (اندر_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_نمبر_arg(argvars, 1) == FAIL
			|| check_for_opt_نمبر_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_نمبر_arg(argvars, 1) == FAIL)))
	واپس;

    اگر (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    ورنہ اگر (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    ورنہ اگر (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    ورنہ
	semsg(_(e_دلیل_کا_str_must_be_list_dictionary_or_blob), "remove()");
}

    ساکن خالی
list_الٹ(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_سیٹ(rettv, l);
    اگر (l != تہی
	    && !قدر_check_تالا(l->lv_تالا,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	اگر (l->lv_پہلا == &range_list_شے)
	{
	    varnumber_T new_آغاز = l->lv_u.nonmat.lv_آغاز
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_انتہا = new_آغاز
		- (l->lv_u.nonmat.lv_انتہا - l->lv_u.nonmat.lv_آغاز);
	    l->lv_u.nonmat.lv_آغاز = new_آغاز;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    واپس;
	}
	li = l->lv_u.mat.lv_آخری;
	l->lv_پہلا = l->lv_u.mat.lv_آخری = تہی;
	l->lv_len = 0;
	جب (li != تہی)
	{
	    ni = li->li_پچھلا;
	    list_append(l, li);
	    li = ni;
	}
	l->lv_u.mat.lv_idx = l->lv_len - l->lv_u.mat.lv_idx - 1;
    }
}

/*
 * "reverse({list})" function
 */
    خالی
f_الٹ(typval_T *argvars, typval_T *rettv)
{
    اگر (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_BLOB)
	blob_الٹ(argvars[0].vval.v_blob, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	اگر (argvars[0].vval.v_string != تہی)
	    rettv->vval.v_string = الٹ_متن(argvars[0].vval.v_string);
	ورنہ
	    rettv->vval.v_string = تہی;
    }
    ورنہ اگر (argvars[0].v_type == VAR_LIST)
	list_الٹ(argvars[0].vval.v_list, rettv);
    ورنہ اگر (argvars[0].v_type == VAR_TUPLE)
	tuple_الٹ(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    ساکن خالی
list_reduce(
	typval_T	*argvars,
	typval_T	*اظہاریہ,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = تہی;
    صحیح		range_list;
    صحیح		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    صحیح		r;
    صحیح		called_emsg_آغاز = called_emsg;
    صحیح		پچھلا_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != تہی && l->lv_پہلا == &range_list_شے;
    اگر (range_list)
	range_val = l->lv_u.nonmat.lv_آغاز;

    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	اگر (l == تہی || l->lv_len == 0)
	{
	    semsg(_(e_reduce_کا_an_خالی_خانے_str_with_no_initial_قدر), "List");
	    واپس;
	}
	اگر (range_list)
	{
	    initial.v_type = VAR_نمبر;
	    initial.vval.v_نمبر = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	ورنہ
	{
	    initial = l->lv_پہلا->li_tv;
	    li = l->lv_پہلا->li_اگلا;
	}
    }
    ورنہ
    {
	initial = argvars[2];
	اگر (l != تہی && !range_list)
	    li = l->lv_پہلا;
    }
    copy_tv(&initial, rettv);

    اگر (l == تہی)
	واپس;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اظہاریہ_get_funccal(اظہاریہ, rettv);

    پچھلا_locked = l->lv_تالا;
    l->lv_تالا = VAR_FIXED;  // disallow the list changing here

    جب (range_list ? range_idx < l->lv_len : li != تہی)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	اگر (range_list)
	{
	    argv[1].v_type = VAR_نمبر;
	    argv[1].vval.v_نمبر = range_val;
	}
	ورنہ
	    argv[1] = li->li_tv;

	r = eval_اظہاریہ_typval(اظہاریہ, TRUE, argv, 2, fc, rettv);

	اگر (argv[0].v_type != VAR_نمبر && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	اگر (r == FAIL || called_emsg != called_emsg_آغاز)
	    توڑو;

	// advance to the next item
	اگر (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	ورنہ
	    li = li->li_اگلا;
    }

    اگر (fc != تہی)
	remove_funccal();

    l->lv_تالا = پچھلا_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    خالی
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    اگر (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    ورنہ اگر (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    ورنہ
	func_name = tv_get_string_strict(&argvars[1]);
    اگر (func_name == تہی || *func_name == NUL)
    {
	emsg(_(e_missing_function_دلیل));
	واپس;
    }

    اگر (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    ورنہ اگر (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    ورنہ اگر (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    ورنہ
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    خالی
f_قاش(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 2) == FAIL))
	واپس;

    اگر (check_can_اشاریہ(&argvars[0], TRUE, FALSE) != OK)
	واپس;

    copy_tv(argvars, rettv);
    eval_اشاریہ_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? تہی : argvars + 2,
	    TRUE, تہی, 0, FALSE);
}

#اگر_ختم // defined(FEAT_EVAL)
