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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
ایستا dict_T		*first_dict = پوچ;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(تهی)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    اگر (d == پوچ)
	بازگشت پوچ;

    // Add the dict to the list of dicts for garbage collection.
    اگر (first_dict != پوچ)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = پوچ;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    بازگشت d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(اندازه(list_T)))
	بازگشت پوچ;
#endif
    بازگشت (dict_alloc());
}

    dict_T *
dict_alloc_lock(صحیح lock)
{
    dict_T *d = dict_alloc();

    اگر (d != پوچ)
	d->dv_lock = lock;
    بازگشت d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    صحیح
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    اگر (d == پوچ)
	بازگشت FAIL;

    rettv_dict_set(rettv, d);
    بازگشت OK;
}

/*
 * Set a dictionary as the return value
 */
    تهی
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    اگر (d != پوچ)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    تهی
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = پوچ;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    تهی
hashtab_free_contents(hashtab_T *ht)
{
    صحیح		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    اگر (check_hashtab_frozen(ht, "clear dict"))
	بازگشت;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (صحیح)ht->ht_used;
    FOR_ALL_HASHTAB_اقلام(ht, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
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

    ایستا تهی
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    اگر (d->dv_used_prev == پوچ)
	first_dict = d->dv_used_next;
    وگرنه
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    اگر (d->dv_used_next != پوچ)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    ایستا تهی
dict_free(dict_T *d)
{
    اگر (!in_free_unref_اقلام)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    تهی
dict_unref(dict_T *d)
{
    اگر (d != پوچ && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    صحیح
dict_free_nonref(صحیح copyID)
{
    dict_T	*dd;
    صحیح		did_free = FALSE;

    برای (dd = first_dict; dd != پوچ; dd = dd->dv_used_next)
	اگر ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    بازگشت did_free;
}

    تهی
dict_free_اقلام(صحیح copyID)
{
    dict_T	*dd, *dd_next;

    برای (dd = first_dict; dd != پوچ; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	اگر ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
dictitem_alloc(char_u *کلید)
{
    dictitem_T *di;
    size_t len = STRLEN(کلید);

    di = alloc(offsetof(dictitem_T, di_کلید) + len + 1);
    اگر (di == پوچ)
	بازگشت پوچ;

    mch_memmove(di->di_کلید, کلید, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    بازگشت di;
}

/*
 * Make a copy of a Dictionary item.
 */
    ایستا dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_کلید);

    di = alloc(offsetof(dictitem_T, di_کلید) + len + 1);
    اگر (di == پوچ)
	بازگشت پوچ;

    mch_memmove(di->di_کلید, org->di_کلید, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    بازگشت di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    تهی
dictitem_remove(dict_T *dict, dictitem_T *item, نویسه *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_کلید);
    اگر (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    وگرنه
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(item);
}

/*
 * Free a dict item.  Also clears the value.
 */
    تهی
dictitem_free(dictitem_T *item)
{
    clear_tv(&item->di_tv);
    اگر (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, صحیح deep, صحیح بالاترین, صحیح copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    صحیح		todo;
    hashitem_T	*hi;

    اگر (orig == پوچ)
	بازگشت پوچ;

    copy = dict_alloc();
    اگر (copy == پوچ)
	بازگشت پوچ;

    اگر (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    اگر (orig->dv_type == پوچ || بالاترین || deep)
	copy->dv_type = پوچ;
    وگرنه
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (صحیح)orig->dv_hashtab.ht_used;
    برای (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_کلید);
	    اگر (di == پوچ)
		بشکن;
	    اگر (deep)
	    {
		اگر (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    بشکن;
		}
	    }
	    وگرنه
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    اگر (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		بشکن;
	    }
	}
    }

    ++copy->dv_refcount;
    اگر (todo > 0)
    {
	dict_unref(copy);
	copy = پوچ;
    }

    بازگشت copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    صحیح
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    بازگشت (d == get_globvar_dict()
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
    صحیح
dict_add(dict_T *d, dictitem_T *item)
{
    اگر (dict_wrong_func_name(d, &item->di_tv, item->di_کلید))
	بازگشت FAIL;
    بازگشت hash_add(&d->dv_hashtab, item->di_کلید, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ایستا صحیح
dict_add_عدد_special(
    dict_T	*d,
    نویسه	*کلید,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)کلید);
    اگر (item == پوچ)
	بازگشت FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_عدد = nr;
    اگر (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_add_عدد(dict_T *d, نویسه *کلید, varnumber_T nr)
{
    بازگشت dict_add_عدد_special(d, کلید, nr, VAR_عدد);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_add_bool(dict_T *d, نویسه *کلید, varnumber_T nr)
{
    بازگشت dict_add_عدد_special(d, کلید, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_add_string(dict_T *d, نویسه *کلید, char_u *str)
{
    بازگشت dict_add_string_len(d, کلید, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_add_string_len(dict_T *d, نویسه *کلید, char_u *str, صحیح len)
{
    dictitem_T	*item;
    char_u	*val = پوچ;

    item = dictitem_alloc((char_u *)کلید);
    اگر (item == پوچ)
	بازگشت FAIL;
    item->di_tv.v_type = VAR_STRING;
    اگر (str != پوچ)
    {
	اگر (len == -1)
	    val = vim_strsave(str);
	وگرنه
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    اگر (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_add_list(dict_T *d, نویسه *کلید, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)کلید);
    اگر (item == پوچ)
	بازگشت FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    اگر (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = پوچ;
	dictitem_free(item);
	بازگشت FAIL;
    }
    ++list->lv_refcount;
    بازگشت OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_add_tv(dict_T *d, نویسه *کلید, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)کلید);
    اگر (item == پوچ)
	بازگشت FAIL;
    copy_tv(tv, &item->di_tv);
    اگر (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_add_callback(dict_T *d, نویسه *کلید, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)کلید);
    اگر (item == پوچ)
	بازگشت FAIL;
    put_callback(cb, &item->di_tv);
    اگر (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_add_func(dict_T *d, نویسه *کلید, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)کلید);
    اگر (item == پوچ)
	بازگشت FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    اگر (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    تهی
dict_iterate_start(typval_T *var, dict_iterator_T *iter)
{
    اگر (var->v_type != VAR_DICT || var->vval.v_dict == پوچ)
	iter->dit_todo = 0;
    وگرنه
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

    اگر (iter->dit_todo == 0)
	بازگشت پوچ;

    وقتی (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_کلید;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    بازگشت result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_add_dict(dict_T *d, نویسه *کلید, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)کلید);
    اگر (item == پوچ)
	بازگشت FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    اگر (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = پوچ;
	dictitem_free(item);
	بازگشت FAIL;
    }
    ++dict->dv_refcount;
    بازگشت OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    اگر (d == پوچ)
	بازگشت 0L;
    بازگشت (long)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *کلید, صحیح len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = پوچ;
    hashitem_T	*hi;

    اگر (d == پوچ)
	بازگشت پوچ;
    اگر (len < 0)
	akey = کلید;
    وگرنه اگر (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(کلید, len);
	اگر (akey == پوچ)
	    بازگشت پوچ;
    }
    وگرنه
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, کلید, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    اگر (HASHITEM_EMPTY(hi))
	بازگشت پوچ;
    بازگشت HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    صحیح
dict_has_کلید(dict_T *d, نویسه *کلید)
{
    بازگشت dict_find(d, (char_u *)کلید, -1) != پوچ;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    صحیح
dict_get_tv(dict_T *d, نویسه *کلید, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)کلید, -1);
    اگر (di == پوچ)
	بازگشت FAIL;
    copy_tv(&di->di_tv, rettv);
    بازگشت OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, نویسه *کلید, صحیح save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)کلید, -1);
    اگر (di == پوچ)
	بازگشت پوچ;
    s = tv_get_string(&di->di_tv);
    اگر (save && s != پوچ)
	s = vim_strsave(s);
    بازگشت s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_عدد(dict_T *d, نویسه *کلید)
{
    بازگشت dict_get_عدد_def(d, کلید, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_عدد_def(dict_T *d, نویسه *کلید, صحیح def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)کلید, -1);
    اگر (di == پوچ)
	بازگشت def;
    بازگشت tv_get_عدد(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_عدد_check(dict_T *d, char_u *کلید)
{
    dictitem_T	*di;

    di = dict_find(d, کلید, -1);
    اگر (di == پوچ)
	بازگشت 0;
    اگر (di->di_tv.v_type != VAR_عدد)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	بازگشت 0;
    }
    بازگشت tv_get_عدد(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, نویسه *کلید, صحیح def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)کلید, -1);
    اگر (di == پوچ)
	بازگشت def;
    بازگشت tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, صحیح copyID, صحیح restore_copyID)
{
    garray_T	ga;
    صحیح		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    صحیح		todo;
    صحیح		prev_lock;

    اگر ((d = tv->vval.v_dict) == پوچ)
	بازگشت پوچ;
    ga_init2(&ga, اندازه(نویسه), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    اگر (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (صحیح)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_اقلام(&d->dv_hashtab, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    اگر (first)
		first = FALSE;
	    وگرنه
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_کلید, FALSE);
	    اگر (tofree != پوچ)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    اگر (s != پوچ)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    اگر (s == پوچ || did_echo_string_emsg)
		بشکن;
	    سطر_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    اگر (todo > 0)
    {
	vim_free(ga.ga_data);
	بازگشت پوچ;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    بازگشت (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    ایستا char_u *
skip_literal_کلید(char_u *کلید)
{
    char_u *p;

    برای (p = کلید; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    بازگشت p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    ایستا صحیح
get_literal_کلید_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_کلید(*arg);

    اگر (p == *arg)
	بازگشت FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    بازگشت OK;
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
get_literal_کلید(char_u **arg)
{
    char_u	*کلید;
    char_u	*end;
    typval_T	rettv;

    اگر (**arg == '\'')
    {
	اگر (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    بازگشت پوچ;
	کلید = rettv.vval.v_string;
    }
    وگرنه اگر (**arg == '"')
    {
	اگر (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    بازگشت پوچ;
	کلید = rettv.vval.v_string;
    }
    وگرنه
    {
	end = skip_literal_کلید(*arg);
	اگر (end == *arg)
	{
	    semsg(_(e_invalid_کلید_str), *arg);
	    بازگشت پوچ;
	}
	کلید = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    بازگشت کلید;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    صحیح
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحیح literal)
{
    صحیح		evaluate = evalarg == پوچ ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = پوچ;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*کلید = پوچ;
    dictitem_T	*item;
    char_u	*curly_عبارت = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    صحیح		vim9script = in_vim9script();
    صحیح		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    اگر (!vim9script
	    && *curly_عبارت != '}'
	    && !literal
	    && eval1(&curly_عبارت, &tv, پوچ) == OK
	    && *skipwhite(curly_عبارت) == '}')
	بازگشت NOTDONE;

    اگر (evaluate)
    {
	d = dict_alloc();
	اگر (d == پوچ)
	    بازگشت FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    وقتی (**arg != '}' && **arg != NUL)
    {
	صحیح	has_bracket = vim9script && **arg == '[';

	اگر (literal)
	{
	    اگر (get_literal_کلید_tv(arg, &tvkey) == FAIL)
		برو failret;
	}
	وگرنه اگر (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_کلید(arg);
	    اگر (tvkey.vval.v_string == پوچ)
		برو failret;
	    tvkey.v_type = VAR_STRING;
	}
	وگرنه
	{
	    اگر (has_bracket)
		*arg = skipwhite(*arg + 1);
	    اگر (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		برو failret;
	    اگر (has_bracket)
	    {
		*arg = skipwhite(*arg);
		اگر (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_کلید));
		    clear_tv(&tvkey);
		    برو failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	اگر (!vim9script)
	    *arg = skipwhite(*arg);
	اگر (**arg != ':')
	{
	    اگر (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    وگرنه
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    برو failret;
	}
	اگر (evaluate)
	{
	    اگر (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    کلید = tv_get_string_buf_chk(&tvkey, buf);
	    اگر (کلید == پوچ)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		برو failret;
	    }
	}
	اگر (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    برو failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	اگر (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    برو failret;
	}
	اگر (check_typval_is_مقدار(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    برو failret;
	}
	اگر (evaluate)
	{
	    item = dict_find(d, کلید, -1);
	    اگر (item != پوچ)
	    {
		semsg(_(e_duplicate_کلید_in_dictionary_str), کلید);
		clear_tv(&tvkey);
		clear_tv(&tv);
		برو failret;
	    }
	    item = dictitem_alloc(کلید);
	    اگر (item != پوچ)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		اگر (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	اگر (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	اگر (had_comma)
	{
	    اگر (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		برو failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg == '}')
	    بشکن;
	اگر (!had_comma)
	{
	    اگر (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    وگرنه
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    برو failret;
	}
    }

    اگر (**arg != '}')
    {
	اگر (evalarg != پوچ)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	اگر (d != پوچ)
	    dict_free(d);
	بازگشت FAIL;
    }

    *arg = *arg + 1;
    اگر (evaluate)
	rettv_dict_set(rettv, d);

    بازگشت OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    صحیح
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    صحیح		vim9script = in_vim9script();
    صحیح		ret = OK;

    اگر (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    وگرنه اگر ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    وگرنه
	ret = NOTDONE;

    بازگشت ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    تهی
dict_extend(dict_T *d1, dict_T *d2, char_u *action, نویسه *func_name)
{
    dictitem_T	*di1;
    صحیح		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    اگر (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	بازگشت;

    اگر (*action == 'm')
    {
	اگر (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    بازگشت;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    اگر (d1->dv_type != پوچ && d1->dv_type->tt_member != پوچ)
	type = d1->dv_type->tt_member;
    وگرنه
	type = پوچ;

    todo = (صحیح)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_اقلام(&d2->dv_hashtab, hi2, todo)
    {
	اگر (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_کلید, -1);
	    // Check the key to be valid when adding to any scope.
	    اگر (d1->dv_scope != 0 && !valid_varname(hi2->hi_کلید, -1, TRUE))
		بشکن;

	    اگر (type != پوچ
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		بشکن;

	    اگر (di1 == پوچ)
	    {
		اگر (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    اگر (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		وگرنه
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    اگر (di1 != پوچ && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    وگرنه اگر (*action == 'e')
	    {
		semsg(_(e_کلید_already_exists_str), hi2->hi_کلید);
		بشکن;
	    }
	    وگرنه اگر (*action == 'f' && HI2DI(hi2) != di1)
	    {
		اگر (مقدار_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    بشکن;
		// Disallow replacing a builtin function.
		اگر (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_کلید))
		    بشکن;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    اگر (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    بازگشت HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    صحیح
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    صحیح		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    صحیح		todo;

    اگر (d1 == d2)
	بازگشت TRUE;
    اگر (dict_len(d1) != dict_len(d2))
	بازگشت FALSE;
    اگر (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	بازگشت TRUE;
    اگر (d1 == پوچ || d2 == پوچ)
	بازگشت FALSE;

    todo = (صحیح)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_اقلام(&d1->dv_hashtab, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_کلید, -1);
	    اگر (item2 == پوچ)
		بازگشت FALSE;
	    اگر (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		بازگشت FALSE;
	    --todo;
	}
    }
    بازگشت TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    long
dict_شمار(dict_T *d, typval_T *needle, صحیح ic)
{
    صحیح		todo;
    hashitem_T	*hi;
    long	n = 0;

    اگر (d == پوچ)
	بازگشت 0;

    todo = (صحیح)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_اقلام(&d->dv_hashtab, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    اگر (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    بازگشت n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    تهی
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	نویسه		*func_name,
	char_u		*arg_errmsg,
	صحیح		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    صحیح	i;

    d1 = argvars[0].vval.v_dict;
    اگر (d1 == پوچ)
    {
	emsg(_(e_cannot_extend_null_dict));
	بازگشت;
    }

    اگر (!is_new && مقدار_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	بازگشت;

    اگر (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	اگر (d1 == پوچ)
	    بازگشت;
    }

    d2 = argvars[1].vval.v_dict;
    اگر (d2 == پوچ)
	برو theend;

    // Check the third argument.
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	ایستا نویسه *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	اگر (action == پوچ)
	{
	    اگر (is_new)
		dict_unref(d1);
	    بازگشت;
	}
	برای (i = 0; i < 3; ++i)
	    اگر (STRCMP(action, av[i]) == 0)
		بشکن;
	اگر (i == 3)
	{
	    اگر (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    بازگشت;
	}
    }
    وگرنه
	action = (char_u *)"force";

    اگر (type != پوچ && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	اگر (is_new)
	    dict_unref(d1);
	بازگشت;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    اگر (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    وگرنه
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    تهی
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	نویسه		*func_name,
	char_u		*arg_errmsg,
	typval_T	*عبارت,
	typval_T	*rettv)
{
    dict_T	*d_ret = پوچ;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    صحیح		todo;
    صحیح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = پوچ;
    }
    اگر (d == پوچ
	  || (filtermap == FILTERMAP_FILTER
			&& مقدار_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	بازگشت;

    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	اگر (rettv_dict_alloc(rettv) == FAIL)
	    بازگشت;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_عبارت_get_funccal(عبارت, &newtv);

    صحیح prev_lock = d->dv_lock;
    اگر (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (صحیح)ht->ht_used;
    FOR_ALL_HASHTAB_اقلام(ht, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    صحیح		r;

	    --todo;
	    di = HI2DI(hi);
	    اگر (filtermap == FILTERMAP_MAP
		    && (مقدار_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		بشکن;
	    set_vim_var_string(VV_کلید, di->di_کلید, -1);
	    r = filter_map_one(&di->di_tv, عبارت, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_کلید));
	    اگر (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		بشکن;
	    }
	    اگر (filtermap == FILTERMAP_MAP)
	    {
		اگر (argtype != پوچ && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    بشکن;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    وگرنه اگر (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (نویسه *)di->di_کلید, &newtv);
		clear_tv(&newtv);
		اگر (r == FAIL)
		    بشکن;
	    }
	    وگرنه اگر (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		اگر (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    بشکن;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    اگر (fc != پوچ)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    تهی
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*کلید;
    dictitem_T	*di;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	بازگشت;
    }

    d = argvars[0].vval.v_dict;
    اگر (d == پوچ || مقدار_check_lock(d->dv_lock, arg_errmsg, TRUE))
	بازگشت;

    کلید = tv_get_string_chk(&argvars[1]);
    اگر (کلید == پوچ)
	بازگشت;

    di = dict_find(d, کلید, -1);
    اگر (di == پوچ)
    {
	semsg(_(e_کلید_not_present_in_dictionary_str), کلید);
	بازگشت;
    }

    اگر (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	بازگشت;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

تعریفنوع شمارش {
    DICT2_LIST_کلیدها,
    DICT2_LIST_مقادیر,
    DICT2_LIST_اقلام,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    ایستا تهی
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    صحیح		todo;

    اگر (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(تهی)rettv_list_alloc(rettv);
	بازگشت;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == پوچ ? 0 : (صحیح)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_اقلام(todo);
    اگر (l == پوچ)
    {
	(تهی)rettv_list_alloc(rettv);
	بازگشت;
    }
    rettv_list_set(rettv, l);
    اگر (todo == 0)
	// NULL or empty dict, return the empty list
	بازگشت;

    li = l->lv_first;
    FOR_ALL_HASHTAB_اقلام(&d->dv_hashtab, hi, todo)
    {
	اگر (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    اگر (what == DICT2_LIST_کلیدها)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_کلید);
	    }
	    وگرنه اگر (what == DICT2_LIST_مقادیر)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    وگرنه
	    {
		// items()
		l2 = list_alloc_with_اقلام(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		اگر (l2 == پوچ)
		    برو alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_کلید);
		اگر (l2->lv_first->li_tv.vval.v_string == پوچ)
		    برو alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    بازگشت;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (تهی)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    تهی
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_اقلام);
}

/*
 * "keys()" function
 */
    تهی
f_کلیدها(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_کلیدها);
}

/*
 * "values(dict)" function
 */
    تهی
f_مقادیر(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_مقادیر);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    تهی
dict_set_اقلام_ro(dict_T *di)
{
    صحیح		todo = (صحیح)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_اقلام(&di->dv_hashtab, hi, todo)
    {
	اگر (HASHITEM_EMPTY(hi))
	    ادامه;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    تهی
f_has_کلید(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    اگر (check_for_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].vval.v_dict == پوچ)
	بازگشت;

    rettv->vval.v_عدد = dict_has_کلید(argvars[0].vval.v_dict,
				(نویسه *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
