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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
स्थिर dict_T		*first_dict = रिक्त;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(शून्य)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    जइ (d == रिक्त)
	प्रत्यागम रिक्त;

    // Add the dict to the list of dicts for garbage collection.
    जइ (first_dict != रिक्त)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = रिक्त;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    प्रत्यागम d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    जइ (alloc_fail_id == id && alloc_does_fail(परिमाण(list_T)))
	प्रत्यागम रिक्त;
#endif
    प्रत्यागम (dict_alloc());
}

    dict_T *
dict_alloc_lock(पूर्णाङ्क lock)
{
    dict_T *d = dict_alloc();

    जइ (d != रिक्त)
	d->dv_lock = lock;
    प्रत्यागम d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    जइ (d == रिक्त)
	प्रत्यागम FAIL;

    rettv_dict_set(rettv, d);
    प्रत्यागम OK;
}

/*
 * Set a dictionary as the return value
 */
    शून्य
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    जइ (d != रिक्त)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    शून्य
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = रिक्त;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    शून्य
hashtab_free_contents(hashtab_T *ht)
{
    पूर्णाङ्क		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    जइ (check_hashtab_frozen(ht, "clear dict"))
	प्रत्यागम;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (पूर्णाङ्क)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(ht, hi, todo)
    {
	जइ (!HASHITEM_EMPTY(hi))
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

    स्थिर शून्य
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    जइ (d->dv_used_prev == रिक्त)
	first_dict = d->dv_used_next;
    अण्णहा
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    जइ (d->dv_used_next != रिक्त)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    स्थिर शून्य
dict_free(dict_T *d)
{
    जइ (!in_free_unref_वस्तूनि)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    शून्य
dict_unref(dict_T *d)
{
    जइ (d != रिक्त && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    पूर्णाङ्क
dict_free_nonref(पूर्णाङ्क copyID)
{
    dict_T	*dd;
    पूर्णाङ्क		did_free = FALSE;

    प्रति (dd = first_dict; dd != रिक्त; dd = dd->dv_used_next)
	जइ ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    प्रत्यागम did_free;
}

    शून्य
dict_free_वस्तूनि(पूर्णाङ्क copyID)
{
    dict_T	*dd, *dd_next;

    प्रति (dd = first_dict; dd != रिक्त; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	जइ ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
dictitem_alloc(char_u *कुञ्चिका)
{
    dictitem_T *di;
    size_t len = STRLEN(कुञ्चिका);

    di = alloc(offsetof(dictitem_T, di_कुञ्चिका) + len + 1);
    जइ (di == रिक्त)
	प्रत्यागम रिक्त;

    mch_memmove(di->di_कुञ्चिका, कुञ्चिका, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    प्रत्यागम di;
}

/*
 * Make a copy of a Dictionary item.
 */
    स्थिर dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_कुञ्चिका);

    di = alloc(offsetof(dictitem_T, di_कुञ्चिका) + len + 1);
    जइ (di == रिक्त)
	प्रत्यागम रिक्त;

    mch_memmove(di->di_कुञ्चिका, org->di_कुञ्चिका, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    प्रत्यागम di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    शून्य
dictitem_remove(dict_T *dict, dictitem_T *item, वर्ण *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_कुञ्चिका);
    जइ (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    अण्णहा
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(item);
}

/*
 * Free a dict item.  Also clears the value.
 */
    शून्य
dictitem_free(dictitem_T *item)
{
    clear_tv(&item->di_tv);
    जइ (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, पूर्णाङ्क deep, पूर्णाङ्क शिखरम्, पूर्णाङ्क copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    पूर्णाङ्क		todo;
    hashitem_T	*hi;

    जइ (orig == रिक्त)
	प्रत्यागम रिक्त;

    copy = dict_alloc();
    जइ (copy == रिक्त)
	प्रत्यागम रिक्त;

    जइ (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    जइ (orig->dv_type == रिक्त || शिखरम् || deep)
	copy->dv_type = रिक्त;
    अण्णहा
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (पूर्णाङ्क)orig->dv_hashtab.ht_used;
    प्रति (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_कुञ्चिका);
	    जइ (di == रिक्त)
		भङ्ग;
	    जइ (deep)
	    {
		जइ (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    भङ्ग;
		}
	    }
	    अण्णहा
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    जइ (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		भङ्ग;
	    }
	}
    }

    ++copy->dv_refcount;
    जइ (todo > 0)
    {
	dict_unref(copy);
	copy = रिक्त;
    }

    प्रत्यागम copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    पूर्णाङ्क
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    प्रत्यागम (d == get_globvar_dict()
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
    पूर्णाङ्क
dict_add(dict_T *d, dictitem_T *item)
{
    जइ (dict_wrong_func_name(d, &item->di_tv, item->di_कुञ्चिका))
	प्रत्यागम FAIL;
    प्रत्यागम hash_add(&d->dv_hashtab, item->di_कुञ्चिका, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    स्थिर पूर्णाङ्क
dict_add_सङ्ख्या_special(
    dict_T	*d,
    वर्ण	*कुञ्चिका,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)कुञ्चिका);
    जइ (item == रिक्त)
	प्रत्यागम FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_सङ्ख्या = nr;
    जइ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_सङ्ख्या(dict_T *d, वर्ण *कुञ्चिका, varnumber_T nr)
{
    प्रत्यागम dict_add_सङ्ख्या_special(d, कुञ्चिका, nr, VAR_सङ्ख्या);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_bool(dict_T *d, वर्ण *कुञ्चिका, varnumber_T nr)
{
    प्रत्यागम dict_add_सङ्ख्या_special(d, कुञ्चिका, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_string(dict_T *d, वर्ण *कुञ्चिका, char_u *str)
{
    प्रत्यागम dict_add_string_len(d, कुञ्चिका, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_string_len(dict_T *d, वर्ण *कुञ्चिका, char_u *str, पूर्णाङ्क len)
{
    dictitem_T	*item;
    char_u	*val = रिक्त;

    item = dictitem_alloc((char_u *)कुञ्चिका);
    जइ (item == रिक्त)
	प्रत्यागम FAIL;
    item->di_tv.v_type = VAR_STRING;
    जइ (str != रिक्त)
    {
	जइ (len == -1)
	    val = vim_strsave(str);
	अण्णहा
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    जइ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_list(dict_T *d, वर्ण *कुञ्चिका, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)कुञ्चिका);
    जइ (item == रिक्त)
	प्रत्यागम FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    जइ (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = रिक्त;
	dictitem_free(item);
	प्रत्यागम FAIL;
    }
    ++list->lv_refcount;
    प्रत्यागम OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_tv(dict_T *d, वर्ण *कुञ्चिका, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)कुञ्चिका);
    जइ (item == रिक्त)
	प्रत्यागम FAIL;
    copy_tv(tv, &item->di_tv);
    जइ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_callback(dict_T *d, वर्ण *कुञ्चिका, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)कुञ्चिका);
    जइ (item == रिक्त)
	प्रत्यागम FAIL;
    put_callback(cb, &item->di_tv);
    जइ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_func(dict_T *d, वर्ण *कुञ्चिका, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)कुञ्चिका);
    जइ (item == रिक्त)
	प्रत्यागम FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    जइ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    शून्य
dict_iterate_start(typval_T *var, dict_iterator_T *iter)
{
    जइ (var->v_type != VAR_DICT || var->vval.v_dict == रिक्त)
	iter->dit_todo = 0;
    अण्णहा
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

    जइ (iter->dit_todo == 0)
	प्रत्यागम रिक्त;

    जाव (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_कुञ्चिका;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    प्रत्यागम result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_dict(dict_T *d, वर्ण *कुञ्चिका, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)कुञ्चिका);
    जइ (item == रिक्त)
	प्रत्यागम FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    जइ (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = रिक्त;
	dictitem_free(item);
	प्रत्यागम FAIL;
    }
    ++dict->dv_refcount;
    प्रत्यागम OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    जइ (d == रिक्त)
	प्रत्यागम 0L;
    प्रत्यागम (long)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *कुञ्चिका, पूर्णाङ्क len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = रिक्त;
    hashitem_T	*hi;

    जइ (d == रिक्त)
	प्रत्यागम रिक्त;
    जइ (len < 0)
	akey = कुञ्चिका;
    अण्णहा जइ (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(कुञ्चिका, len);
	जइ (akey == रिक्त)
	    प्रत्यागम रिक्त;
    }
    अण्णहा
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, कुञ्चिका, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    जइ (HASHITEM_EMPTY(hi))
	प्रत्यागम रिक्त;
    प्रत्यागम HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    पूर्णाङ्क
dict_has_कुञ्चिका(dict_T *d, वर्ण *कुञ्चिका)
{
    प्रत्यागम dict_find(d, (char_u *)कुञ्चिका, -1) != रिक्त;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    पूर्णाङ्क
dict_get_tv(dict_T *d, वर्ण *कुञ्चिका, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)कुञ्चिका, -1);
    जइ (di == रिक्त)
	प्रत्यागम FAIL;
    copy_tv(&di->di_tv, rettv);
    प्रत्यागम OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, वर्ण *कुञ्चिका, पूर्णाङ्क save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)कुञ्चिका, -1);
    जइ (di == रिक्त)
	प्रत्यागम रिक्त;
    s = tv_get_string(&di->di_tv);
    जइ (save && s != रिक्त)
	s = vim_strsave(s);
    प्रत्यागम s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_सङ्ख्या(dict_T *d, वर्ण *कुञ्चिका)
{
    प्रत्यागम dict_get_सङ्ख्या_def(d, कुञ्चिका, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_सङ्ख्या_def(dict_T *d, वर्ण *कुञ्चिका, पूर्णाङ्क def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)कुञ्चिका, -1);
    जइ (di == रिक्त)
	प्रत्यागम def;
    प्रत्यागम tv_get_सङ्ख्या(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_सङ्ख्या_check(dict_T *d, char_u *कुञ्चिका)
{
    dictitem_T	*di;

    di = dict_find(d, कुञ्चिका, -1);
    जइ (di == रिक्त)
	प्रत्यागम 0;
    जइ (di->di_tv.v_type != VAR_सङ्ख्या)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	प्रत्यागम 0;
    }
    प्रत्यागम tv_get_सङ्ख्या(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, वर्ण *कुञ्चिका, पूर्णाङ्क def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)कुञ्चिका, -1);
    जइ (di == रिक्त)
	प्रत्यागम def;
    प्रत्यागम tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, पूर्णाङ्क copyID, पूर्णाङ्क restore_copyID)
{
    garray_T	ga;
    पूर्णाङ्क		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    पूर्णाङ्क		todo;
    पूर्णाङ्क		prev_lock;

    जइ ((d = tv->vval.v_dict) == रिक्त)
	प्रत्यागम रिक्त;
    ga_init2(&ga, परिमाण(वर्ण), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    जइ (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(&d->dv_hashtab, hi, todo)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    जइ (first)
		first = FALSE;
	    अण्णहा
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_कुञ्चिका, FALSE);
	    जइ (tofree != रिक्त)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    जइ (s != रिक्त)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    जइ (s == रिक्त || did_echo_string_emsg)
		भङ्ग;
	    पङ्क्तिः_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    जइ (todo > 0)
    {
	vim_free(ga.ga_data);
	प्रत्यागम रिक्त;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    प्रत्यागम (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    स्थिर char_u *
skip_literal_कुञ्चिका(char_u *कुञ्चिका)
{
    char_u *p;

    प्रति (p = कुञ्चिका; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    प्रत्यागम p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    स्थिर पूर्णाङ्क
get_literal_कुञ्चिका_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_कुञ्चिका(*arg);

    जइ (p == *arg)
	प्रत्यागम FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    प्रत्यागम OK;
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
get_literal_कुञ्चिका(char_u **arg)
{
    char_u	*कुञ्चिका;
    char_u	*end;
    typval_T	rettv;

    जइ (**arg == '\'')
    {
	जइ (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    प्रत्यागम रिक्त;
	कुञ्चिका = rettv.vval.v_string;
    }
    अण्णहा जइ (**arg == '"')
    {
	जइ (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    प्रत्यागम रिक्त;
	कुञ्चिका = rettv.vval.v_string;
    }
    अण्णहा
    {
	end = skip_literal_कुञ्चिका(*arg);
	जइ (end == *arg)
	{
	    semsg(_(e_invalid_कुञ्चिका_str), *arg);
	    प्रत्यागम रिक्त;
	}
	कुञ्चिका = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    प्रत्यागम कुञ्चिका;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    पूर्णाङ्क
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, पूर्णाङ्क literal)
{
    पूर्णाङ्क		evaluate = evalarg == रिक्त ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = रिक्त;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*कुञ्चिका = रिक्त;
    dictitem_T	*item;
    char_u	*curly_व्यञ्जकम् = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    पूर्णाङ्क		vim9script = in_vim9script();
    पूर्णाङ्क		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    जइ (!vim9script
	    && *curly_व्यञ्जकम् != '}'
	    && !literal
	    && eval1(&curly_व्यञ्जकम्, &tv, रिक्त) == OK
	    && *skipwhite(curly_व्यञ्जकम्) == '}')
	प्रत्यागम NOTDONE;

    जइ (evaluate)
    {
	d = dict_alloc();
	जइ (d == रिक्त)
	    प्रत्यागम FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    जाव (**arg != '}' && **arg != NUL)
    {
	पूर्णाङ्क	has_bracket = vim9script && **arg == '[';

	जइ (literal)
	{
	    जइ (get_literal_कुञ्चिका_tv(arg, &tvkey) == FAIL)
		गच्छ failret;
	}
	अण्णहा जइ (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_कुञ्चिका(arg);
	    जइ (tvkey.vval.v_string == रिक्त)
		गच्छ failret;
	    tvkey.v_type = VAR_STRING;
	}
	अण्णहा
	{
	    जइ (has_bracket)
		*arg = skipwhite(*arg + 1);
	    जइ (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		गच्छ failret;
	    जइ (has_bracket)
	    {
		*arg = skipwhite(*arg);
		जइ (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_कुञ्चिका));
		    clear_tv(&tvkey);
		    गच्छ failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	जइ (!vim9script)
	    *arg = skipwhite(*arg);
	जइ (**arg != ':')
	{
	    जइ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    अण्णहा
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    गच्छ failret;
	}
	जइ (evaluate)
	{
	    जइ (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    कुञ्चिका = tv_get_string_buf_chk(&tvkey, buf);
	    जइ (कुञ्चिका == रिक्त)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		गच्छ failret;
	    }
	}
	जइ (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    गच्छ failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	जइ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    गच्छ failret;
	}
	जइ (check_typval_is_मानम्(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    गच्छ failret;
	}
	जइ (evaluate)
	{
	    item = dict_find(d, कुञ्चिका, -1);
	    जइ (item != रिक्त)
	    {
		semsg(_(e_duplicate_कुञ्चिका_in_dictionary_str), कुञ्चिका);
		clear_tv(&tvkey);
		clear_tv(&tv);
		गच्छ failret;
	    }
	    item = dictitem_alloc(कुञ्चिका);
	    जइ (item != रिक्त)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		जइ (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	जइ (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	जइ (had_comma)
	{
	    जइ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		गच्छ failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	जइ (**arg == '}')
	    भङ्ग;
	जइ (!had_comma)
	{
	    जइ (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    अण्णहा
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    गच्छ failret;
	}
    }

    जइ (**arg != '}')
    {
	जइ (evalarg != रिक्त)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	जइ (d != रिक्त)
	    dict_free(d);
	प्रत्यागम FAIL;
    }

    *arg = *arg + 1;
    जइ (evaluate)
	rettv_dict_set(rettv, d);

    प्रत्यागम OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    पूर्णाङ्क
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    पूर्णाङ्क		vim9script = in_vim9script();
    पूर्णाङ्क		ret = OK;

    जइ (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    अण्णहा जइ ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    अण्णहा
	ret = NOTDONE;

    प्रत्यागम ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    शून्य
dict_extend(dict_T *d1, dict_T *d2, char_u *action, वर्ण *func_name)
{
    dictitem_T	*di1;
    पूर्णाङ्क		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    जइ (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	प्रत्यागम;

    जइ (*action == 'm')
    {
	जइ (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    प्रत्यागम;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    जइ (d1->dv_type != रिक्त && d1->dv_type->tt_member != रिक्त)
	type = d1->dv_type->tt_member;
    अण्णहा
	type = रिक्त;

    todo = (पूर्णाङ्क)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_वस्तूनि(&d2->dv_hashtab, hi2, todo)
    {
	जइ (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_कुञ्चिका, -1);
	    // Check the key to be valid when adding to any scope.
	    जइ (d1->dv_scope != 0 && !valid_varname(hi2->hi_कुञ्चिका, -1, TRUE))
		भङ्ग;

	    जइ (type != रिक्त
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		भङ्ग;

	    जइ (di1 == रिक्त)
	    {
		जइ (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    जइ (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		अण्णहा
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    जइ (di1 != रिक्त && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    अण्णहा जइ (*action == 'e')
	    {
		semsg(_(e_कुञ्चिका_already_exists_str), hi2->hi_कुञ्चिका);
		भङ्ग;
	    }
	    अण्णहा जइ (*action == 'f' && HI2DI(hi2) != di1)
	    {
		जइ (मानम्_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    भङ्ग;
		// Disallow replacing a builtin function.
		जइ (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_कुञ्चिका))
		    भङ्ग;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    जइ (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    प्रत्यागम HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    पूर्णाङ्क
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    पूर्णाङ्क		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    पूर्णाङ्क		todo;

    जइ (d1 == d2)
	प्रत्यागम TRUE;
    जइ (dict_len(d1) != dict_len(d2))
	प्रत्यागम FALSE;
    जइ (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	प्रत्यागम TRUE;
    जइ (d1 == रिक्त || d2 == रिक्त)
	प्रत्यागम FALSE;

    todo = (पूर्णाङ्क)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(&d1->dv_hashtab, hi, todo)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_कुञ्चिका, -1);
	    जइ (item2 == रिक्त)
		प्रत्यागम FALSE;
	    जइ (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		प्रत्यागम FALSE;
	    --todo;
	}
    }
    प्रत्यागम TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    long
dict_गणनम्(dict_T *d, typval_T *needle, पूर्णाङ्क ic)
{
    पूर्णाङ्क		todo;
    hashitem_T	*hi;
    long	n = 0;

    जइ (d == रिक्त)
	प्रत्यागम 0;

    todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(&d->dv_hashtab, hi, todo)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    जइ (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    प्रत्यागम n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    शून्य
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	वर्ण		*func_name,
	char_u		*arg_errmsg,
	पूर्णाङ्क		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    पूर्णाङ्क	i;

    d1 = argvars[0].vval.v_dict;
    जइ (d1 == रिक्त)
    {
	emsg(_(e_cannot_extend_null_dict));
	प्रत्यागम;
    }

    जइ (!is_new && मानम्_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	प्रत्यागम;

    जइ (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	जइ (d1 == रिक्त)
	    प्रत्यागम;
    }

    d2 = argvars[1].vval.v_dict;
    जइ (d2 == रिक्त)
	गच्छ theend;

    // Check the third argument.
    जइ (argvars[2].v_type != VAR_UNKNOWN)
    {
	स्थिर वर्ण *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	जइ (action == रिक्त)
	{
	    जइ (is_new)
		dict_unref(d1);
	    प्रत्यागम;
	}
	प्रति (i = 0; i < 3; ++i)
	    जइ (STRCMP(action, av[i]) == 0)
		भङ्ग;
	जइ (i == 3)
	{
	    जइ (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    प्रत्यागम;
	}
    }
    अण्णहा
	action = (char_u *)"force";

    जइ (type != रिक्त && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	जइ (is_new)
	    dict_unref(d1);
	प्रत्यागम;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    जइ (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    अण्णहा
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    शून्य
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	वर्ण		*func_name,
	char_u		*arg_errmsg,
	typval_T	*व्यञ्जकम्,
	typval_T	*rettv)
{
    dict_T	*d_ret = रिक्त;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    पूर्णाङ्क		todo;
    पूर्णाङ्क		rem;
    typval_T	newtv;
    funccall_T	*fc;

    जइ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = रिक्त;
    }
    जइ (d == रिक्त
	  || (filtermap == FILTERMAP_FILTER
			&& मानम्_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	प्रत्यागम;

    जइ (filtermap == FILTERMAP_MAPNEW)
    {
	जइ (rettv_dict_alloc(rettv) == FAIL)
	    प्रत्यागम;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_व्यञ्जकम्_get_funccal(व्यञ्जकम्, &newtv);

    पूर्णाङ्क prev_lock = d->dv_lock;
    जइ (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (पूर्णाङ्क)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(ht, hi, todo)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    पूर्णाङ्क		r;

	    --todo;
	    di = HI2DI(hi);
	    जइ (filtermap == FILTERMAP_MAP
		    && (मानम्_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		भङ्ग;
	    set_vim_var_string(VV_कुञ्चिका, di->di_कुञ्चिका, -1);
	    r = filter_map_one(&di->di_tv, व्यञ्जकम्, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_कुञ्चिका));
	    जइ (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		भङ्ग;
	    }
	    जइ (filtermap == FILTERMAP_MAP)
	    {
		जइ (argtype != रिक्त && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    भङ्ग;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    अण्णहा जइ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (वर्ण *)di->di_कुञ्चिका, &newtv);
		clear_tv(&newtv);
		जइ (r == FAIL)
		    भङ्ग;
	    }
	    अण्णहा जइ (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		जइ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    भङ्ग;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    जइ (fc != रिक्त)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    शून्य
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*कुञ्चिका;
    dictitem_T	*di;

    जइ (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	प्रत्यागम;
    }

    d = argvars[0].vval.v_dict;
    जइ (d == रिक्त || मानम्_check_lock(d->dv_lock, arg_errmsg, TRUE))
	प्रत्यागम;

    कुञ्चिका = tv_get_string_chk(&argvars[1]);
    जइ (कुञ्चिका == रिक्त)
	प्रत्यागम;

    di = dict_find(d, कुञ्चिका, -1);
    जइ (di == रिक्त)
    {
	semsg(_(e_कुञ्चिका_not_present_in_dictionary_str), कुञ्चिका);
	प्रत्यागम;
    }

    जइ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	प्रत्यागम;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

प्रकारनिर्देश गणना {
    DICT2_LIST_कुञ्चिकाः,
    DICT2_LIST_मानानि,
    DICT2_LIST_वस्तूनि,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    स्थिर शून्य
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    पूर्णाङ्क		todo;

    जइ (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(शून्य)rettv_list_alloc(rettv);
	प्रत्यागम;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == रिक्त ? 0 : (पूर्णाङ्क)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_वस्तूनि(todo);
    जइ (l == रिक्त)
    {
	(शून्य)rettv_list_alloc(rettv);
	प्रत्यागम;
    }
    rettv_list_set(rettv, l);
    जइ (todo == 0)
	// NULL or empty dict, return the empty list
	प्रत्यागम;

    li = l->lv_first;
    FOR_ALL_HASHTAB_वस्तूनि(&d->dv_hashtab, hi, todo)
    {
	जइ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    जइ (what == DICT2_LIST_कुञ्चिकाः)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_कुञ्चिका);
	    }
	    अण्णहा जइ (what == DICT2_LIST_मानानि)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    अण्णहा
	    {
		// items()
		l2 = list_alloc_with_वस्तूनि(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		जइ (l2 == रिक्त)
		    गच्छ alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_कुञ्चिका);
		जइ (l2->lv_first->li_tv.vval.v_string == रिक्त)
		    गच्छ alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    प्रत्यागम;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (शून्य)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    शून्य
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_वस्तूनि);
}

/*
 * "keys()" function
 */
    शून्य
f_कुञ्चिकाः(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_कुञ्चिकाः);
}

/*
 * "values(dict)" function
 */
    शून्य
f_मानानि(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_मानानि);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    शून्य
dict_set_वस्तूनि_ro(dict_T *di)
{
    पूर्णाङ्क		todo = (पूर्णाङ्क)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_वस्तूनि(&di->dv_hashtab, hi, todo)
    {
	जइ (HASHITEM_EMPTY(hi))
	    अनुवर्त;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    शून्य
f_has_कुञ्चिका(typval_T *argvars, typval_T *rettv)
{
    जइ (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_सङ्ख्या_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    जइ (check_for_dict_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    जइ (argvars[0].vval.v_dict == रिक्त)
	प्रत्यागम;

    rettv->vval.v_सङ्ख्या = dict_has_कुञ्चिका(argvars[0].vval.v_dict,
				(वर्ण *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
