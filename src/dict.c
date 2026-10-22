/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * dict.c: Dictionary support
 */

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
স্থির dict_T		*first_dict = রিক্ত;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(শূন্য)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    যদি (d == রিক্ত)
	প্রত্যাগম রিক্ত;

    // Add the dict to the list of dicts for garbage collection.
    যদি (first_dict != রিক্ত)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = রিক্ত;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    প্রত্যাগম d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    যদি (alloc_fail_id == id && alloc_does_fail(পরিমাণ(list_T)))
	প্রত্যাগম রিক্ত;
#endif
    প্রত্যাগম (dict_alloc());
}

    dict_T *
dict_alloc_lock(পূর্ণাঙ্ক lock)
{
    dict_T *d = dict_alloc();

    যদি (d != রিক্ত)
	d->dv_lock = lock;
    প্রত্যাগম d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    যদি (d == রিক্ত)
	প্রত্যাগম FAIL;

    rettv_dict_set(rettv, d);
    প্রত্যাগম OK;
}

/*
 * Set a dictionary as the return value
 */
    শূন্য
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    যদি (d != রিক্ত)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    শূন্য
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = রিক্ত;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    শূন্য
hashtab_free_contents(hashtab_T *ht)
{
    পূর্ণাঙ্ক		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    যদি (check_hashtab_frozen(ht, "clear dict"))
	প্রত্যাগম;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (পূর্ণাঙ্ক)ht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    // Remove the item before deleting it, just in case there is
	    // something recursive causing trouble.
	    di = HI2DI(hi);
	    hash_remove(ht, hi, "clear dict");
	    dictitem_free(di);
	    --todo;
	}
    }

    // The hashtab is still locked, it has to be re-initialized anyway.
    hash_clear(ht);
}

    স্থির শূন্য
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    যদি (d->dv_used_prev == রিক্ত)
	first_dict = d->dv_used_next;
    অন্যথা
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    যদি (d->dv_used_next != রিক্ত)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    স্থির শূন্য
dict_free(dict_T *d)
{
    যদি (!in_free_unref_বস্তুসমূহ)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    শূন্য
dict_unref(dict_T *d)
{
    যদি (d != রিক্ত && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    পূর্ণাঙ্ক
dict_free_nonref(পূর্ণাঙ্ক copyID)
{
    dict_T	*dd;
    পূর্ণাঙ্ক		did_free = FALSE;

    প্রতি (dd = first_dict; dd != রিক্ত; dd = dd->dv_used_next)
	যদি ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    প্রত্যাগম did_free;
}

    শূন্য
dict_free_বস্তুসমূহ(পূর্ণাঙ্ক copyID)
{
    dict_T	*dd, *dd_next;

    প্রতি (dd = first_dict; dd != রিক্ত; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	যদি ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	    dict_free_dict(dd);
    }
}

/*
 * Allocate a Dictionary item.
 * The "key" is copied to the new item.
 * Note that the type and value of the item "di_tv" still needs to be
 * initialized!
 * Returns NULL when out of memory.
 */
    dictitem_T *
dictitem_alloc(char_u *চাবি)
{
    dictitem_T *di;
    size_t len = STRLEN(চাবি);

    di = alloc(offsetof(dictitem_T, di_চাবি) + len + 1);
    যদি (di == রিক্ত)
	প্রত্যাগম রিক্ত;

    mch_memmove(di->di_চাবি, চাবি, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    প্রত্যাগম di;
}

/*
 * Make a copy of a Dictionary item.
 */
    স্থির dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_চাবি);

    di = alloc(offsetof(dictitem_T, di_চাবি) + len + 1);
    যদি (di == রিক্ত)
	প্রত্যাগম রিক্ত;

    mch_memmove(di->di_চাবি, org->di_চাবি, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    প্রত্যাগম di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    শূন্য
dictitem_remove(dict_T *dict, dictitem_T *item, বর্ণ *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_চাবি);
    যদি (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    অন্যথা
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(item);
}

/*
 * Free a dict item.  Also clears the value.
 */
    শূন্য
dictitem_free(dictitem_T *item)
{
    clear_tv(&item->di_tv);
    যদি (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, পূর্ণাঙ্ক deep, পূর্ণাঙ্ক শীর্ষ, পূর্ণাঙ্ক copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    পূর্ণাঙ্ক		todo;
    hashitem_T	*hi;

    যদি (orig == রিক্ত)
	প্রত্যাগম রিক্ত;

    copy = dict_alloc();
    যদি (copy == রিক্ত)
	প্রত্যাগম রিক্ত;

    যদি (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    যদি (orig->dv_type == রিক্ত || শীর্ষ || deep)
	copy->dv_type = রিক্ত;
    অন্যথা
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (পূর্ণাঙ্ক)orig->dv_hashtab.ht_used;
    প্রতি (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_চাবি);
	    যদি (di == রিক্ত)
		ভঙ্গ;
	    যদি (deep)
	    {
		যদি (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    ভঙ্গ;
		}
	    }
	    অন্যথা
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    যদি (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		ভঙ্গ;
	    }
	}
    }

    ++copy->dv_refcount;
    যদি (todo > 0)
    {
	dict_unref(copy);
	copy = রিক্ত;
    }

    প্রত্যাগম copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    পূর্ণাঙ্ক
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    প্রত্যাগম (d == get_globvar_dict()
		|| (in_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid)
		   && d == &SCRIPT_ITEM(current_sctx.sc_sid)->sn_vars->sv_dict)
		|| &d->dv_hashtab == get_funccal_local_ht())
	    && (tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
	    && var_wrong_func_name(name, TRUE);
}

/*
 * Add item "item" to Dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    পূর্ণাঙ্ক
dict_add(dict_T *d, dictitem_T *item)
{
    যদি (dict_wrong_func_name(d, &item->di_tv, item->di_চাবি))
	প্রত্যাগম FAIL;
    প্রত্যাগম hash_add(&d->dv_hashtab, item->di_চাবি, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    স্থির পূর্ণাঙ্ক
dict_add_সংখ্যা_special(
    dict_T	*d,
    বর্ণ	*চাবি,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)চাবি);
    যদি (item == রিক্ত)
	প্রত্যাগম FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_সংখ্যা = nr;
    যদি (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    পূর্ণাঙ্ক
dict_add_সংখ্যা(dict_T *d, বর্ণ *চাবি, varnumber_T nr)
{
    প্রত্যাগম dict_add_সংখ্যা_special(d, চাবি, nr, VAR_সংখ্যা);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    পূর্ণাঙ্ক
dict_add_bool(dict_T *d, বর্ণ *চাবি, varnumber_T nr)
{
    প্রত্যাগম dict_add_সংখ্যা_special(d, চাবি, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    পূর্ণাঙ্ক
dict_add_string(dict_T *d, বর্ণ *চাবি, char_u *str)
{
    প্রত্যাগম dict_add_string_len(d, চাবি, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    পূর্ণাঙ্ক
dict_add_string_len(dict_T *d, বর্ণ *চাবি, char_u *str, পূর্ণাঙ্ক len)
{
    dictitem_T	*item;
    char_u	*val = রিক্ত;

    item = dictitem_alloc((char_u *)চাবি);
    যদি (item == রিক্ত)
	প্রত্যাগম FAIL;
    item->di_tv.v_type = VAR_STRING;
    যদি (str != রিক্ত)
    {
	যদি (len == -1)
	    val = vim_strsave(str);
	অন্যথা
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    যদি (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    পূর্ণাঙ্ক
dict_add_list(dict_T *d, বর্ণ *চাবি, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)চাবি);
    যদি (item == রিক্ত)
	প্রত্যাগম FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    যদি (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = রিক্ত;
	dictitem_free(item);
	প্রত্যাগম FAIL;
    }
    ++list->lv_refcount;
    প্রত্যাগম OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    পূর্ণাঙ্ক
dict_add_tv(dict_T *d, বর্ণ *চাবি, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)চাবি);
    যদি (item == রিক্ত)
	প্রত্যাগম FAIL;
    copy_tv(tv, &item->di_tv);
    যদি (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    পূর্ণাঙ্ক
dict_add_callback(dict_T *d, বর্ণ *চাবি, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)চাবি);
    যদি (item == রিক্ত)
	প্রত্যাগম FAIL;
    put_callback(cb, &item->di_tv);
    যদি (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    পূর্ণাঙ্ক
dict_add_func(dict_T *d, বর্ণ *চাবি, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)চাবি);
    যদি (item == রিক্ত)
	প্রত্যাগম FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    যদি (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    শূন্য
dict_iterate_start(typval_T *var, dict_iterator_T *iter)
{
    যদি (var->v_type != VAR_DICT || var->vval.v_dict == রিক্ত)
	iter->dit_todo = 0;
    অন্যথা
    {
	dict_T	*d = var->vval.v_dict;

	iter->dit_todo = d->dv_hashtab.ht_used;
	iter->dit_hi = d->dv_hashtab.ht_array;
    }
}

/*
 * Iterate over the items referred to by "iter".  It should be initialized with
 * dict_iterate_start().
 * Returns a pointer to the key.
 * "*tv_result" is set to point to the value for that key.
 * If there are no more items, NULL is returned.
 */
    char_u *
dict_iterate_next(dict_iterator_T *iter, typval_T **tv_result)
{
    dictitem_T	*di;
    char_u      *result;

    যদি (iter->dit_todo == 0)
	প্রত্যাগম রিক্ত;

    যাবত্ (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_চাবি;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    প্রত্যাগম result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    পূর্ণাঙ্ক
dict_add_dict(dict_T *d, বর্ণ *চাবি, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)চাবি);
    যদি (item == রিক্ত)
	প্রত্যাগম FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    যদি (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = রিক্ত;
	dictitem_free(item);
	প্রত্যাগম FAIL;
    }
    ++dict->dv_refcount;
    প্রত্যাগম OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    যদি (d == রিক্ত)
	প্রত্যাগম 0L;
    প্রত্যাগম (long)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *চাবি, পূর্ণাঙ্ক len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = রিক্ত;
    hashitem_T	*hi;

    যদি (d == রিক্ত)
	প্রত্যাগম রিক্ত;
    যদি (len < 0)
	akey = চাবি;
    অন্যথা যদি (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(চাবি, len);
	যদি (akey == রিক্ত)
	    প্রত্যাগম রিক্ত;
    }
    অন্যথা
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, চাবি, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    যদি (HASHITEM_EMPTY(hi))
	প্রত্যাগম রিক্ত;
    প্রত্যাগম HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    পূর্ণাঙ্ক
dict_has_চাবি(dict_T *d, বর্ণ *চাবি)
{
    প্রত্যাগম dict_find(d, (char_u *)চাবি, -1) != রিক্ত;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    পূর্ণাঙ্ক
dict_get_tv(dict_T *d, বর্ণ *চাবি, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)চাবি, -1);
    যদি (di == রিক্ত)
	প্রত্যাগম FAIL;
    copy_tv(&di->di_tv, rettv);
    প্রত্যাগম OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, বর্ণ *চাবি, পূর্ণাঙ্ক save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)চাবি, -1);
    যদি (di == রিক্ত)
	প্রত্যাগম রিক্ত;
    s = tv_get_string(&di->di_tv);
    যদি (save && s != রিক্ত)
	s = vim_strsave(s);
    প্রত্যাগম s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_সংখ্যা(dict_T *d, বর্ণ *চাবি)
{
    প্রত্যাগম dict_get_সংখ্যা_def(d, চাবি, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_সংখ্যা_def(dict_T *d, বর্ণ *চাবি, পূর্ণাঙ্ক def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)চাবি, -1);
    যদি (di == রিক্ত)
	প্রত্যাগম def;
    প্রত্যাগম tv_get_সংখ্যা(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_সংখ্যা_check(dict_T *d, char_u *চাবি)
{
    dictitem_T	*di;

    di = dict_find(d, চাবি, -1);
    যদি (di == রিক্ত)
	প্রত্যাগম 0;
    যদি (di->di_tv.v_type != VAR_সংখ্যা)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	প্রত্যাগম 0;
    }
    প্রত্যাগম tv_get_সংখ্যা(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, বর্ণ *চাবি, পূর্ণাঙ্ক def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)চাবি, -1);
    যদি (di == রিক্ত)
	প্রত্যাগম def;
    প্রত্যাগম tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, পূর্ণাঙ্ক copyID, পূর্ণাঙ্ক restore_copyID)
{
    garray_T	ga;
    পূর্ণাঙ্ক		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    পূর্ণাঙ্ক		todo;
    পূর্ণাঙ্ক		prev_lock;

    যদি ((d = tv->vval.v_dict) == রিক্ত)
	প্রত্যাগম রিক্ত;
    ga_init2(&ga, পরিমাণ(বর্ণ), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    যদি (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (পূর্ণাঙ্ক)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(&d->dv_hashtab, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    যদি (first)
		first = FALSE;
	    অন্যথা
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_চাবি, FALSE);
	    যদি (tofree != রিক্ত)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    যদি (s != রিক্ত)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    যদি (s == রিক্ত || did_echo_string_emsg)
		ভঙ্গ;
	    সারি_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    যদি (todo > 0)
    {
	vim_free(ga.ga_data);
	প্রত্যাগম রিক্ত;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    প্রত্যাগম (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    স্থির char_u *
skip_literal_চাবি(char_u *চাবি)
{
    char_u *p;

    প্রতি (p = চাবি; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    প্রত্যাগম p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    স্থির পূর্ণাঙ্ক
get_literal_চাবি_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_চাবি(*arg);

    যদি (p == *arg)
	প্রত্যাগম FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    প্রত্যাগম OK;
}

/*
 * Get a literal key for a Vim9 dict:
 * {"name": value},
 * {'name': value},
 * {name: value} use "name" as a literal key
 * Return the key in allocated memory or NULL in the case of an error.
 * "arg" is advanced to just after the key.
 */
    char_u *
get_literal_চাবি(char_u **arg)
{
    char_u	*চাবি;
    char_u	*end;
    typval_T	rettv;

    যদি (**arg == '\'')
    {
	যদি (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    প্রত্যাগম রিক্ত;
	চাবি = rettv.vval.v_string;
    }
    অন্যথা যদি (**arg == '"')
    {
	যদি (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    প্রত্যাগম রিক্ত;
	চাবি = rettv.vval.v_string;
    }
    অন্যথা
    {
	end = skip_literal_চাবি(*arg);
	যদি (end == *arg)
	{
	    semsg(_(e_invalid_চাবি_str), *arg);
	    প্রত্যাগম রিক্ত;
	}
	চাবি = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    প্রত্যাগম চাবি;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    পূর্ণাঙ্ক
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, পূর্ণাঙ্ক literal)
{
    পূর্ণাঙ্ক		evaluate = evalarg == রিক্ত ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = রিক্ত;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*চাবি = রিক্ত;
    dictitem_T	*item;
    char_u	*curly_রাশিমালা = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    পূর্ণাঙ্ক		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    যদি (!vim9script
	    && *curly_রাশিমালা != '}'
	    && !literal
	    && eval1(&curly_রাশিমালা, &tv, রিক্ত) == OK
	    && *skipwhite(curly_রাশিমালা) == '}')
	প্রত্যাগম NOTDONE;

    যদি (evaluate)
    {
	d = dict_alloc();
	যদি (d == রিক্ত)
	    প্রত্যাগম FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    যাবত্ (**arg != '}' && **arg != NUL)
    {
	পূর্ণাঙ্ক	has_bracket = vim9script && **arg == '[';

	যদি (literal)
	{
	    যদি (get_literal_চাবি_tv(arg, &tvkey) == FAIL)
		গচ্ছ failret;
	}
	অন্যথা যদি (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_চাবি(arg);
	    যদি (tvkey.vval.v_string == রিক্ত)
		গচ্ছ failret;
	    tvkey.v_type = VAR_STRING;
	}
	অন্যথা
	{
	    যদি (has_bracket)
		*arg = skipwhite(*arg + 1);
	    যদি (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		গচ্ছ failret;
	    যদি (has_bracket)
	    {
		*arg = skipwhite(*arg);
		যদি (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_চাবি));
		    clear_tv(&tvkey);
		    গচ্ছ failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	যদি (!vim9script)
	    *arg = skipwhite(*arg);
	যদি (**arg != ':')
	{
	    যদি (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    অন্যথা
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    গচ্ছ failret;
	}
	যদি (evaluate)
	{
	    যদি (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    চাবি = tv_get_string_buf_chk(&tvkey, buf);
	    যদি (চাবি == রিক্ত)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		গচ্ছ failret;
	    }
	}
	যদি (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    গচ্ছ failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	যদি (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    গচ্ছ failret;
	}
	যদি (check_typval_is_মান(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    গচ্ছ failret;
	}
	যদি (evaluate)
	{
	    item = dict_find(d, চাবি, -1);
	    যদি (item != রিক্ত)
	    {
		semsg(_(e_duplicate_চাবি_in_dictionary_str), চাবি);
		clear_tv(&tvkey);
		clear_tv(&tv);
		গচ্ছ failret;
	    }
	    item = dictitem_alloc(চাবি);
	    যদি (item != রিক্ত)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		যদি (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	যদি (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	যদি (had_comma)
	{
	    যদি (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		গচ্ছ failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	যদি (**arg == '}')
	    ভঙ্গ;
	যদি (!had_comma)
	{
	    যদি (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    অন্যথা
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    গচ্ছ failret;
	}
    }

    যদি (**arg != '}')
    {
	যদি (evalarg != রিক্ত)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	যদি (d != রিক্ত)
	    dict_free(d);
	প্রত্যাগম FAIL;
    }

    *arg = *arg + 1;
    যদি (evaluate)
	rettv_dict_set(rettv, d);

    প্রত্যাগম OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    পূর্ণাঙ্ক
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    পূর্ণাঙ্ক		ret = OK;

    যদি (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    অন্যথা যদি ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    অন্যথা
	ret = NOTDONE;

    প্রত্যাগম ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    শূন্য
dict_extend(dict_T *d1, dict_T *d2, char_u *action, বর্ণ *func_name)
{
    dictitem_T	*di1;
    পূর্ণাঙ্ক		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    যদি (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	প্রত্যাগম;

    যদি (*action == 'm')
    {
	যদি (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    প্রত্যাগম;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    যদি (d1->dv_type != রিক্ত && d1->dv_type->tt_member != রিক্ত)
	type = d1->dv_type->tt_member;
    অন্যথা
	type = রিক্ত;

    todo = (পূর্ণাঙ্ক)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_বস্তুসমূহ(&d2->dv_hashtab, hi2, todo)
    {
	যদি (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_চাবি, -1);
	    // Check the key to be valid when adding to any scope.
	    যদি (d1->dv_scope != 0 && !valid_varname(hi2->hi_চাবি, -1, TRUE))
		ভঙ্গ;

	    যদি (type != রিক্ত
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		ভঙ্গ;

	    যদি (di1 == রিক্ত)
	    {
		যদি (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    যদি (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		অন্যথা
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    যদি (di1 != রিক্ত && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    অন্যথা যদি (*action == 'e')
	    {
		semsg(_(e_চাবি_already_exists_str), hi2->hi_চাবি);
		ভঙ্গ;
	    }
	    অন্যথা যদি (*action == 'f' && HI2DI(hi2) != di1)
	    {
		যদি (মান_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    ভঙ্গ;
		// Disallow replacing a builtin function.
		যদি (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_চাবি))
		    ভঙ্গ;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    যদি (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    প্রত্যাগম HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    পূর্ণাঙ্ক
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    পূর্ণাঙ্ক		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    পূর্ণাঙ্ক		todo;

    যদি (d1 == d2)
	প্রত্যাগম TRUE;
    যদি (dict_len(d1) != dict_len(d2))
	প্রত্যাগম FALSE;
    যদি (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	প্রত্যাগম TRUE;
    যদি (d1 == রিক্ত || d2 == রিক্ত)
	প্রত্যাগম FALSE;

    todo = (পূর্ণাঙ্ক)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(&d1->dv_hashtab, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_চাবি, -1);
	    যদি (item2 == রিক্ত)
		প্রত্যাগম FALSE;
	    যদি (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		প্রত্যাগম FALSE;
	    --todo;
	}
    }
    প্রত্যাগম TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    long
dict_গুনতি(dict_T *d, typval_T *needle, পূর্ণাঙ্ক ic)
{
    পূর্ণাঙ্ক		todo;
    hashitem_T	*hi;
    long	n = 0;

    যদি (d == রিক্ত)
	প্রত্যাগম 0;

    todo = (পূর্ণাঙ্ক)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(&d->dv_hashtab, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    যদি (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    প্রত্যাগম n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    শূন্য
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	বর্ণ		*func_name,
	char_u		*arg_errmsg,
	পূর্ণাঙ্ক		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    পূর্ণাঙ্ক	i;

    d1 = argvars[0].vval.v_dict;
    যদি (d1 == রিক্ত)
    {
	emsg(_(e_cannot_extend_null_dict));
	প্রত্যাগম;
    }

    যদি (!is_new && মান_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	প্রত্যাগম;

    যদি (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	যদি (d1 == রিক্ত)
	    প্রত্যাগম;
    }

    d2 = argvars[1].vval.v_dict;
    যদি (d2 == রিক্ত)
	গচ্ছ theend;

    // Check the third argument.
    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	স্থির বর্ণ *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	যদি (action == রিক্ত)
	{
	    যদি (is_new)
		dict_unref(d1);
	    প্রত্যাগম;
	}
	প্রতি (i = 0; i < 3; ++i)
	    যদি (STRCMP(action, av[i]) == 0)
		ভঙ্গ;
	যদি (i == 3)
	{
	    যদি (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    প্রত্যাগম;
	}
    }
    অন্যথা
	action = (char_u *)"force";

    যদি (type != রিক্ত && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	যদি (is_new)
	    dict_unref(d1);
	প্রত্যাগম;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    যদি (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    অন্যথা
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    শূন্য
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	বর্ণ		*func_name,
	char_u		*arg_errmsg,
	typval_T	*রাশিমালা,
	typval_T	*rettv)
{
    dict_T	*d_ret = রিক্ত;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    পূর্ণাঙ্ক		todo;
    পূর্ণাঙ্ক		rem;
    typval_T	newtv;
    funccall_T	*fc;

    যদি (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = রিক্ত;
    }
    যদি (d == রিক্ত
	  || (filtermap == FILTERMAP_FILTER
			&& মান_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	প্রত্যাগম;

    যদি (filtermap == FILTERMAP_MAPNEW)
    {
	যদি (rettv_dict_alloc(rettv) == FAIL)
	    প্রত্যাগম;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_রাশিমালা_get_funccal(রাশিমালা, &newtv);

    পূর্ণাঙ্ক prev_lock = d->dv_lock;
    যদি (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (পূর্ণাঙ্ক)ht->ht_used;
    FOR_ALL_HASHTAB_বস্তুসমূহ(ht, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    পূর্ণাঙ্ক		r;

	    --todo;
	    di = HI2DI(hi);
	    যদি (filtermap == FILTERMAP_MAP
		    && (মান_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		ভঙ্গ;
	    set_vim_var_string(VV_চাবি, di->di_চাবি, -1);
	    r = filter_map_one(&di->di_tv, রাশিমালা, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_চাবি));
	    যদি (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		ভঙ্গ;
	    }
	    যদি (filtermap == FILTERMAP_MAP)
	    {
		যদি (argtype != রিক্ত && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ভঙ্গ;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    অন্যথা যদি (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (বর্ণ *)di->di_চাবি, &newtv);
		clear_tv(&newtv);
		যদি (r == FAIL)
		    ভঙ্গ;
	    }
	    অন্যথা যদি (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		যদি (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    ভঙ্গ;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    যদি (fc != রিক্ত)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    শূন্য
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*চাবি;
    dictitem_T	*di;

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	প্রত্যাগম;
    }

    d = argvars[0].vval.v_dict;
    যদি (d == রিক্ত || মান_check_lock(d->dv_lock, arg_errmsg, TRUE))
	প্রত্যাগম;

    চাবি = tv_get_string_chk(&argvars[1]);
    যদি (চাবি == রিক্ত)
	প্রত্যাগম;

    di = dict_find(d, চাবি, -1);
    যদি (di == রিক্ত)
    {
	semsg(_(e_চাবি_not_present_in_dictionary_str), চাবি);
	প্রত্যাগম;
    }

    যদি (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	প্রত্যাগম;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

প্রকারনির্দেশ গণনা {
    DICT2_LIST_চাবিগুলি,
    DICT2_LIST_মানসমূহ,
    DICT2_LIST_বস্তুসমূহ,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    স্থির শূন্য
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    পূর্ণাঙ্ক		todo;

    যদি (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(শূন্য)rettv_list_alloc(rettv);
	প্রত্যাগম;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == রিক্ত ? 0 : (পূর্ণাঙ্ক)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_বস্তুসমূহ(todo);
    যদি (l == রিক্ত)
    {
	(শূন্য)rettv_list_alloc(rettv);
	প্রত্যাগম;
    }
    rettv_list_set(rettv, l);
    যদি (todo == 0)
	// NULL or empty dict, return the empty list
	প্রত্যাগম;

    li = l->lv_first;
    FOR_ALL_HASHTAB_বস্তুসমূহ(&d->dv_hashtab, hi, todo)
    {
	যদি (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    যদি (what == DICT2_LIST_চাবিগুলি)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_চাবি);
	    }
	    অন্যথা যদি (what == DICT2_LIST_মানসমূহ)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    অন্যথা
	    {
		// items()
		l2 = list_alloc_with_বস্তুসমূহ(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		যদি (l2 == রিক্ত)
		    গচ্ছ alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_চাবি);
		যদি (l2->lv_first->li_tv.vval.v_string == রিক্ত)
		    গচ্ছ alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    প্রত্যাগম;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (শূন্য)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    শূন্য
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_বস্তুসমূহ);
}

/*
 * "keys()" function
 */
    শূন্য
f_চাবিগুলি(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_চাবিগুলি);
}

/*
 * "values(dict)" function
 */
    শূন্য
f_মানসমূহ(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_মানসমূহ);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    শূন্য
dict_set_বস্তুসমূহ_ro(dict_T *di)
{
    পূর্ণাঙ্ক		todo = (পূর্ণাঙ্ক)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_বস্তুসমূহ(&di->dv_hashtab, hi, todo)
    {
	যদি (HASHITEM_EMPTY(hi))
	    অনুবর্ত;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    শূন্য
f_has_চাবি(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_সংখ্যা_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    যদি (check_for_dict_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    যদি (argvars[0].vval.v_dict == রিক্ত)
	প্রত্যাগম;

    rettv->vval.v_সংখ্যা = dict_has_চাবি(argvars[0].vval.v_dict,
				(বর্ণ *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
