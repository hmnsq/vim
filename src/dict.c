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

#ضمن "vim.h"

#ܐܢ defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
ساكن dict_T		*first_dict = عدم;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(فراغ)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    ܐܢ (d == عدم)
	ܦܢܐ عدم;

    // Add the dict to the list of dicts for garbage collection.
    ܐܢ (first_dict != عدم)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = عدم;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    ܦܢܐ d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ܐܢ (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	ܦܢܐ عدم;
#endif
    ܦܢܐ (dict_alloc());
}

    dict_T *
dict_alloc_lock(صحيح lock)
{
    dict_T *d = dict_alloc();

    ܐܢ (d != عدم)
	d->dv_lock = lock;
    ܦܢܐ d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    صحيح
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    ܐܢ (d == عدم)
	ܦܢܐ FAIL;

    rettv_dict_set(rettv, d);
    ܦܢܐ OK;
}

/*
 * Set a dictionary as the return value
 */
    فراغ
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    ܐܢ (d != عدم)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    فراغ
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = عدم;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    فراغ
hashtab_free_contents(hashtab_T *ht)
{
    صحيح		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    ܐܢ (check_hashtab_frozen(ht, "clear dict"))
	ܦܢܐ;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (صحيح)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	ܐܢ (!HASHITEM_EMPTY(hi))
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

    ساكن فراغ
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    ܐܢ (d->dv_used_prev == عدم)
	first_dict = d->dv_used_next;
    ܐܚܪܢܐ
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    ܐܢ (d->dv_used_next != عدم)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    ساكن فراغ
dict_free(dict_T *d)
{
    ܐܢ (!in_free_unref_items)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    فراغ
dict_unref(dict_T *d)
{
    ܐܢ (d != عدم && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    صحيح
dict_free_nonref(صحيح copyID)
{
    dict_T	*dd;
    صحيح		did_free = FALSE;

    لكل (dd = first_dict; dd != عدم; dd = dd->dv_used_next)
	ܐܢ ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    ܦܢܐ did_free;
}

    فراغ
dict_free_items(صحيح copyID)
{
    dict_T	*dd, *dd_next;

    لكل (dd = first_dict; dd != عدم; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	ܐܢ ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
dictitem_alloc(char_u *key)
{
    dictitem_T *di;
    size_t len = STRLEN(key);

    di = alloc(offsetof(dictitem_T, di_key) + len + 1);
    ܐܢ (di == عدم)
	ܦܢܐ عدم;

    mch_memmove(di->di_key, key, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    ܦܢܐ di;
}

/*
 * Make a copy of a Dictionary item.
 */
    ساكن dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_key);

    di = alloc(offsetof(dictitem_T, di_key) + len + 1);
    ܐܢ (di == عدم)
	ܦܢܐ عدم;

    mch_memmove(di->di_key, org->di_key, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    ܦܢܐ di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    فراغ
dictitem_remove(dict_T *dict, dictitem_T *item, حرف *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_key);
    ܐܢ (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    ܐܚܪܢܐ
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(item);
}

/*
 * Free a dict item.  Also clears the value.
 */
    فراغ
dictitem_free(dictitem_T *item)
{
    clear_tv(&item->di_tv);
    ܐܢ (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, صحيح deep, صحيح top, صحيح copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    صحيح		todo;
    hashitem_T	*hi;

    ܐܢ (orig == عدم)
	ܦܢܐ عدم;

    copy = dict_alloc();
    ܐܢ (copy == عدم)
	ܦܢܐ عدم;

    ܐܢ (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    ܐܢ (orig->dv_type == عدم || top || deep)
	copy->dv_type = عدم;
    ܐܚܪܢܐ
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (صحيح)orig->dv_hashtab.ht_used;
    لكل (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ܐܢ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_key);
	    ܐܢ (di == عدم)
		اكسر;
	    ܐܢ (deep)
	    {
		ܐܢ (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    اكسر;
		}
	    }
	    ܐܚܪܢܐ
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    ܐܢ (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		اكسر;
	    }
	}
    }

    ++copy->dv_refcount;
    ܐܢ (todo > 0)
    {
	dict_unref(copy);
	copy = عدم;
    }

    ܦܢܐ copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    صحيح
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    ܦܢܐ (d == get_globvar_dict()
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
    صحيح
dict_add(dict_T *d, dictitem_T *item)
{
    ܐܢ (dict_wrong_func_name(d, &item->di_tv, item->di_key))
	ܦܢܐ FAIL;
    ܦܢܐ hash_add(&d->dv_hashtab, item->di_key, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ساكن صحيح
dict_add_number_special(
    dict_T	*d,
    حرف	*key,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    ܐܢ (item == عدم)
	ܦܢܐ FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_number = nr;
    ܐܢ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ܦܢܐ FAIL;
    }
    ܦܢܐ OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_add_number(dict_T *d, حرف *key, varnumber_T nr)
{
    ܦܢܐ dict_add_number_special(d, key, nr, VAR_NUMBER);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_add_bool(dict_T *d, حرف *key, varnumber_T nr)
{
    ܦܢܐ dict_add_number_special(d, key, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_add_string(dict_T *d, حرف *key, char_u *str)
{
    ܦܢܐ dict_add_string_len(d, key, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_add_string_len(dict_T *d, حرف *key, char_u *str, صحيح len)
{
    dictitem_T	*item;
    char_u	*val = عدم;

    item = dictitem_alloc((char_u *)key);
    ܐܢ (item == عدم)
	ܦܢܐ FAIL;
    item->di_tv.v_type = VAR_STRING;
    ܐܢ (str != عدم)
    {
	ܐܢ (len == -1)
	    val = vim_strsave(str);
	ܐܚܪܢܐ
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    ܐܢ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ܦܢܐ FAIL;
    }
    ܦܢܐ OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_add_list(dict_T *d, حرف *key, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    ܐܢ (item == عدم)
	ܦܢܐ FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    ܐܢ (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = عدم;
	dictitem_free(item);
	ܦܢܐ FAIL;
    }
    ++list->lv_refcount;
    ܦܢܐ OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_add_tv(dict_T *d, حرف *key, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    ܐܢ (item == عدم)
	ܦܢܐ FAIL;
    copy_tv(tv, &item->di_tv);
    ܐܢ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ܦܢܐ FAIL;
    }
    ܦܢܐ OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_add_callback(dict_T *d, حرف *key, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    ܐܢ (item == عدم)
	ܦܢܐ FAIL;
    put_callback(cb, &item->di_tv);
    ܐܢ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ܦܢܐ FAIL;
    }
    ܦܢܐ OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_add_func(dict_T *d, حرف *key, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    ܐܢ (item == عدم)
	ܦܢܐ FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    ܐܢ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ܦܢܐ FAIL;
    }
    ܦܢܐ OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    فراغ
dict_iterate_start(typval_T *var, dict_iterator_T *iter)
{
    ܐܢ (var->v_type != VAR_DICT || var->vval.v_dict == عدم)
	iter->dit_todo = 0;
    ܐܚܪܢܐ
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

    ܐܢ (iter->dit_todo == 0)
	ܦܢܐ عدم;

    ܟܡܐ (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_key;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    ܦܢܐ result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_add_dict(dict_T *d, حرف *key, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    ܐܢ (item == عدم)
	ܦܢܐ FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    ܐܢ (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = عدم;
	dictitem_free(item);
	ܦܢܐ FAIL;
    }
    ++dict->dv_refcount;
    ܦܢܐ OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    ܐܢ (d == عدم)
	ܦܢܐ 0L;
    ܦܢܐ (long)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *key, صحيح len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = عدم;
    hashitem_T	*hi;

    ܐܢ (d == عدم)
	ܦܢܐ عدم;
    ܐܢ (len < 0)
	akey = key;
    ܐܚܪܢܐ ܐܢ (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(key, len);
	ܐܢ (akey == عدم)
	    ܦܢܐ عدم;
    }
    ܐܚܪܢܐ
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, key, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    ܐܢ (HASHITEM_EMPTY(hi))
	ܦܢܐ عدم;
    ܦܢܐ HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    صحيح
dict_has_key(dict_T *d, حرف *key)
{
    ܦܢܐ dict_find(d, (char_u *)key, -1) != عدم;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    صحيح
dict_get_tv(dict_T *d, حرف *key, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)key, -1);
    ܐܢ (di == عدم)
	ܦܢܐ FAIL;
    copy_tv(&di->di_tv, rettv);
    ܦܢܐ OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, حرف *key, صحيح save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)key, -1);
    ܐܢ (di == عدم)
	ܦܢܐ عدم;
    s = tv_get_string(&di->di_tv);
    ܐܢ (save && s != عدم)
	s = vim_strsave(s);
    ܦܢܐ s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_number(dict_T *d, حرف *key)
{
    ܦܢܐ dict_get_number_def(d, key, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_number_def(dict_T *d, حرف *key, صحيح def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)key, -1);
    ܐܢ (di == عدم)
	ܦܢܐ def;
    ܦܢܐ tv_get_number(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_number_check(dict_T *d, char_u *key)
{
    dictitem_T	*di;

    di = dict_find(d, key, -1);
    ܐܢ (di == عدم)
	ܦܢܐ 0;
    ܐܢ (di->di_tv.v_type != VAR_NUMBER)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	ܦܢܐ 0;
    }
    ܦܢܐ tv_get_number(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, حرف *key, صحيح def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)key, -1);
    ܐܢ (di == عدم)
	ܦܢܐ def;
    ܦܢܐ tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, صحيح copyID, صحيح restore_copyID)
{
    garray_T	ga;
    صحيح		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    صحيح		todo;
    صحيح		prev_lock;

    ܐܢ ((d = tv->vval.v_dict) == عدم)
	ܦܢܐ عدم;
    ga_init2(&ga, حجم(حرف), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    ܐܢ (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (صحيح)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	ܐܢ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    ܐܢ (first)
		first = FALSE;
	    ܐܚܪܢܐ
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_key, FALSE);
	    ܐܢ (tofree != عدم)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    ܐܢ (s != عدم)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    ܐܢ (s == عدم || did_echo_string_emsg)
		اكسر;
	    line_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    ܐܢ (todo > 0)
    {
	vim_free(ga.ga_data);
	ܦܢܐ عدم;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    ܦܢܐ (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    ساكن char_u *
skip_literal_key(char_u *key)
{
    char_u *p;

    لكل (p = key; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    ܦܢܐ p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    ساكن صحيح
get_literal_key_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_key(*arg);

    ܐܢ (p == *arg)
	ܦܢܐ FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    ܦܢܐ OK;
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
get_literal_key(char_u **arg)
{
    char_u	*key;
    char_u	*end;
    typval_T	rettv;

    ܐܢ (**arg == '\'')
    {
	ܐܢ (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ܦܢܐ عدم;
	key = rettv.vval.v_string;
    }
    ܐܚܪܢܐ ܐܢ (**arg == '"')
    {
	ܐܢ (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ܦܢܐ عدم;
	key = rettv.vval.v_string;
    }
    ܐܚܪܢܐ
    {
	end = skip_literal_key(*arg);
	ܐܢ (end == *arg)
	{
	    semsg(_(e_invalid_key_str), *arg);
	    ܦܢܐ عدم;
	}
	key = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    ܦܢܐ key;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    صحيح
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحيح literal)
{
    صحيح		evaluate = evalarg == عدم ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = عدم;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*key = عدم;
    dictitem_T	*item;
    char_u	*curly_expr = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    صحيح		vim9script = in_vim9script();
    صحيح		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    ܐܢ (!vim9script
	    && *curly_expr != '}'
	    && !literal
	    && eval1(&curly_expr, &tv, عدم) == OK
	    && *skipwhite(curly_expr) == '}')
	ܦܢܐ NOTDONE;

    ܐܢ (evaluate)
    {
	d = dict_alloc();
	ܐܢ (d == عدم)
	    ܦܢܐ FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ܟܡܐ (**arg != '}' && **arg != NUL)
    {
	صحيح	has_bracket = vim9script && **arg == '[';

	ܐܢ (literal)
	{
	    ܐܢ (get_literal_key_tv(arg, &tvkey) == FAIL)
		اذهب failret;
	}
	ܐܚܪܢܐ ܐܢ (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_key(arg);
	    ܐܢ (tvkey.vval.v_string == عدم)
		اذهب failret;
	    tvkey.v_type = VAR_STRING;
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (has_bracket)
		*arg = skipwhite(*arg + 1);
	    ܐܢ (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		اذهب failret;
	    ܐܢ (has_bracket)
	    {
		*arg = skipwhite(*arg);
		ܐܢ (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_key));
		    clear_tv(&tvkey);
		    اذهب failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ܐܢ (!vim9script)
	    *arg = skipwhite(*arg);
	ܐܢ (**arg != ':')
	{
	    ܐܢ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ܐܚܪܢܐ
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    اذهب failret;
	}
	ܐܢ (evaluate)
	{
	    ܐܢ (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    key = tv_get_string_buf_chk(&tvkey, buf);
	    ܐܢ (key == عدم)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		اذهب failret;
	    }
	}
	ܐܢ (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    اذهب failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ܐܢ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    اذهب failret;
	}
	ܐܢ (check_typval_is_value(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    اذهب failret;
	}
	ܐܢ (evaluate)
	{
	    item = dict_find(d, key, -1);
	    ܐܢ (item != عدم)
	    {
		semsg(_(e_duplicate_key_in_dictionary_str), key);
		clear_tv(&tvkey);
		clear_tv(&tv);
		اذهب failret;
	    }
	    item = dictitem_alloc(key);
	    ܐܢ (item != عدم)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		ܐܢ (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ܐܢ (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	ܐܢ (had_comma)
	{
	    ܐܢ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		اذهب failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ܐܢ (**arg == '}')
	    اكسر;
	ܐܢ (!had_comma)
	{
	    ܐܢ (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    ܐܚܪܢܐ
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    اذهب failret;
	}
    }

    ܐܢ (**arg != '}')
    {
	ܐܢ (evalarg != عدم)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	ܐܢ (d != عدم)
	    dict_free(d);
	ܦܢܐ FAIL;
    }

    *arg = *arg + 1;
    ܐܢ (evaluate)
	rettv_dict_set(rettv, d);

    ܦܢܐ OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    صحيح
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    صحيح		vim9script = in_vim9script();
    صحيح		ret = OK;

    ܐܢ (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    ܐܚܪܢܐ ܐܢ ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    ܐܚܪܢܐ
	ret = NOTDONE;

    ܦܢܐ ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    فراغ
dict_extend(dict_T *d1, dict_T *d2, char_u *action, حرف *func_name)
{
    dictitem_T	*di1;
    صحيح		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    ܐܢ (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	ܦܢܐ;

    ܐܢ (*action == 'm')
    {
	ܐܢ (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    ܦܢܐ;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    ܐܢ (d1->dv_type != عدم && d1->dv_type->tt_member != عدم)
	type = d1->dv_type->tt_member;
    ܐܚܪܢܐ
	type = عدم;

    todo = (صحيح)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_ITEMS(&d2->dv_hashtab, hi2, todo)
    {
	ܐܢ (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_key, -1);
	    // Check the key to be valid when adding to any scope.
	    ܐܢ (d1->dv_scope != 0 && !valid_varname(hi2->hi_key, -1, TRUE))
		اكسر;

	    ܐܢ (type != عدم
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		اكسر;

	    ܐܢ (di1 == عدم)
	    {
		ܐܢ (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    ܐܢ (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		ܐܚܪܢܐ
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    ܐܢ (di1 != عدم && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    ܐܚܪܢܐ ܐܢ (*action == 'e')
	    {
		semsg(_(e_key_already_exists_str), hi2->hi_key);
		اكسر;
	    }
	    ܐܚܪܢܐ ܐܢ (*action == 'f' && HI2DI(hi2) != di1)
	    {
		ܐܢ (value_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    اكسر;
		// Disallow replacing a builtin function.
		ܐܢ (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_key))
		    اكسر;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    ܐܢ (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    ܦܢܐ HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    صحيح
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    صحيح		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    صحيح		todo;

    ܐܢ (d1 == d2)
	ܦܢܐ TRUE;
    ܐܢ (dict_len(d1) != dict_len(d2))
	ܦܢܐ FALSE;
    ܐܢ (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	ܦܢܐ TRUE;
    ܐܢ (d1 == عدم || d2 == عدم)
	ܦܢܐ FALSE;

    todo = (صحيح)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ITEMS(&d1->dv_hashtab, hi, todo)
    {
	ܐܢ (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_key, -1);
	    ܐܢ (item2 == عدم)
		ܦܢܐ FALSE;
	    ܐܢ (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		ܦܢܐ FALSE;
	    --todo;
	}
    }
    ܦܢܐ TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    long
dict_count(dict_T *d, typval_T *needle, صحيح ic)
{
    صحيح		todo;
    hashitem_T	*hi;
    long	n = 0;

    ܐܢ (d == عدم)
	ܦܢܐ 0;

    todo = (صحيح)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	ܐܢ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ܐܢ (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    ܦܢܐ n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    فراغ
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	حرف		*func_name,
	char_u		*arg_errmsg,
	صحيح		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    صحيح	i;

    d1 = argvars[0].vval.v_dict;
    ܐܢ (d1 == عدم)
    {
	emsg(_(e_cannot_extend_null_dict));
	ܦܢܐ;
    }

    ܐܢ (!is_new && value_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	ܦܢܐ;

    ܐܢ (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	ܐܢ (d1 == عدم)
	    ܦܢܐ;
    }

    d2 = argvars[1].vval.v_dict;
    ܐܢ (d2 == عدم)
	اذهب theend;

    // Check the third argument.
    ܐܢ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ساكن حرف *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	ܐܢ (action == عدم)
	{
	    ܐܢ (is_new)
		dict_unref(d1);
	    ܦܢܐ;
	}
	لكل (i = 0; i < 3; ++i)
	    ܐܢ (STRCMP(action, av[i]) == 0)
		اكسر;
	ܐܢ (i == 3)
	{
	    ܐܢ (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    ܦܢܐ;
	}
    }
    ܐܚܪܢܐ
	action = (char_u *)"force";

    ܐܢ (type != عدم && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	ܐܢ (is_new)
	    dict_unref(d1);
	ܦܢܐ;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    ܐܢ (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    ܐܚܪܢܐ
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    فراغ
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	حرف		*func_name,
	char_u		*arg_errmsg,
	typval_T	*expr,
	typval_T	*rettv)
{
    dict_T	*d_ret = عدم;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    صحيح		todo;
    صحيح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ܐܢ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = عدم;
    }
    ܐܢ (d == عدم
	  || (filtermap == FILTERMAP_FILTER
			&& value_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	ܦܢܐ;

    ܐܢ (filtermap == FILTERMAP_MAPNEW)
    {
	ܐܢ (rettv_dict_alloc(rettv) == FAIL)
	    ܦܢܐ;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    صحيح prev_lock = d->dv_lock;
    ܐܢ (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (صحيح)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	ܐܢ (!HASHITEM_EMPTY(hi))
	{
	    صحيح		r;

	    --todo;
	    di = HI2DI(hi);
	    ܐܢ (filtermap == FILTERMAP_MAP
		    && (value_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		اكسر;
	    set_vim_var_string(VV_KEY, di->di_key, -1);
	    r = filter_map_one(&di->di_tv, expr, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_KEY));
	    ܐܢ (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		اكسر;
	    }
	    ܐܢ (filtermap == FILTERMAP_MAP)
	    {
		ܐܢ (argtype != عدم && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    اكسر;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    ܐܚܪܢܐ ܐܢ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (حرف *)di->di_key, &newtv);
		clear_tv(&newtv);
		ܐܢ (r == FAIL)
		    اكسر;
	    }
	    ܐܚܪܢܐ ܐܢ (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		ܐܢ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    اكسر;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    ܐܢ (fc != عدم)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    فراغ
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*key;
    dictitem_T	*di;

    ܐܢ (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	ܦܢܐ;
    }

    d = argvars[0].vval.v_dict;
    ܐܢ (d == عدم || value_check_lock(d->dv_lock, arg_errmsg, TRUE))
	ܦܢܐ;

    key = tv_get_string_chk(&argvars[1]);
    ܐܢ (key == عدم)
	ܦܢܐ;

    di = dict_find(d, key, -1);
    ܐܢ (di == عدم)
    {
	semsg(_(e_key_not_present_in_dictionary_str), key);
	ܦܢܐ;
    }

    ܐܢ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	ܦܢܐ;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

تعريفنوع تعداد {
    DICT2LIST_KEYS,
    DICT2LIST_VALUES,
    DICT2LIST_ITEMS,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    ساكن فراغ
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    صحيح		todo;

    ܐܢ (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(فراغ)rettv_list_alloc(rettv);
	ܦܢܐ;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == عدم ? 0 : (صحيح)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_items(todo);
    ܐܢ (l == عدم)
    {
	(فراغ)rettv_list_alloc(rettv);
	ܦܢܐ;
    }
    rettv_list_set(rettv, l);
    ܐܢ (todo == 0)
	// NULL or empty dict, return the empty list
	ܦܢܐ;

    li = l->lv_first;
    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	ܐܢ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    ܐܢ (what == DICT2LIST_KEYS)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_key);
	    }
	    ܐܚܪܢܐ ܐܢ (what == DICT2LIST_VALUES)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    ܐܚܪܢܐ
	    {
		// items()
		l2 = list_alloc_with_items(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		ܐܢ (l2 == عدم)
		    اذهب alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_key);
		ܐܢ (l2->lv_first->li_tv.vval.v_string == عدم)
		    اذهب alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    ܦܢܐ;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (فراغ)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    فراغ
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2LIST_ITEMS);
}

/*
 * "keys()" function
 */
    فراغ
f_keys(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2LIST_KEYS);
}

/*
 * "values(dict)" function
 */
    فراغ
f_values(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2LIST_VALUES);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    فراغ
dict_set_items_ro(dict_T *di)
{
    صحيح		todo = (صحيح)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_ITEMS(&di->dv_hashtab, hi, todo)
    {
	ܐܢ (HASHITEM_EMPTY(hi))
	    استمر;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    فراغ
f_has_key(typval_T *argvars, typval_T *rettv)
{
    ܐܢ (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_number_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    ܐܢ (check_for_dict_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    ܐܢ (argvars[0].vval.v_dict == عدم)
	ܦܢܐ;

    rettv->vval.v_number = dict_has_key(argvars[0].vval.v_dict,
				(حرف *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
