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

#सचे defined(FEAT_EVAL)

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
    सचे (d == रिक्त)
	पटिनिवत्त रिक्त;

    // Add the dict to the list of dicts for garbage collection.
    सचे (first_dict != रिक्त)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = रिक्त;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    पटिनिवत्त d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    सचे (alloc_fail_id == id && alloc_does_fail(परिमाण(list_T)))
	पटिनिवत्त रिक्त;
#endif
    पटिनिवत्त (dict_alloc());
}

    dict_T *
dict_alloc_lock(पूर्णाङ्क lock)
{
    dict_T *d = dict_alloc();

    सचे (d != रिक्त)
	d->dv_lock = lock;
    पटिनिवत्त d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    सचे (d == रिक्त)
	पटिनिवत्त FAIL;

    rettv_dict_set(rettv, d);
    पटिनिवत्त OK;
}

/*
 * Set a dictionary as the return value
 */
    शून्य
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    सचे (d != रिक्त)
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

    सचे (check_hashtab_frozen(ht, "clear dict"))
	पटिनिवत्त;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (पूर्णाङ्क)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(ht, hi, todo)
    {
	सचे (!HASHITEM_EMPTY(hi))
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
    सचे (d->dv_used_prev == रिक्त)
	first_dict = d->dv_used_next;
    अञ्ञथा
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    सचे (d->dv_used_next != रिक्त)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    स्थिर शून्य
dict_free(dict_T *d)
{
    सचे (!in_free_unref_वस्तूनि)
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
    सचे (d != रिक्त && --d->dv_refcount <= 0)
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

    अत्थाय (dd = first_dict; dd != रिक्त; dd = dd->dv_used_next)
	सचे ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    पटिनिवत्त did_free;
}

    शून्य
dict_free_वस्तूनि(पूर्णाङ्क copyID)
{
    dict_T	*dd, *dd_next;

    अत्थाय (dd = first_dict; dd != रिक्त; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	सचे ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
    सचे (di == रिक्त)
	पटिनिवत्त रिक्त;

    mch_memmove(di->di_कुञ्चिका, कुञ्चिका, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    पटिनिवत्त di;
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
    सचे (di == रिक्त)
	पटिनिवत्त रिक्त;

    mch_memmove(di->di_कुञ्चिका, org->di_कुञ्चिका, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    पटिनिवत्त di;
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
    सचे (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    अञ्ञथा
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
    सचे (item->di_flags & DI_FLAGS_ALLOC)
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

    सचे (orig == रिक्त)
	पटिनिवत्त रिक्त;

    copy = dict_alloc();
    सचे (copy == रिक्त)
	पटिनिवत्त रिक्त;

    सचे (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    सचे (orig->dv_type == रिक्त || शिखरम् || deep)
	copy->dv_type = रिक्त;
    अञ्ञथा
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (पूर्णाङ्क)orig->dv_hashtab.ht_used;
    अत्थाय (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	सचे (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_कुञ्चिका);
	    सचे (di == रिक्त)
		भङ्ग;
	    सचे (deep)
	    {
		सचे (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    भङ्ग;
		}
	    }
	    अञ्ञथा
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    सचे (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		भङ्ग;
	    }
	}
    }

    ++copy->dv_refcount;
    सचे (todo > 0)
    {
	dict_unref(copy);
	copy = रिक्त;
    }

    पटिनिवत्त copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    पूर्णाङ्क
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    पटिनिवत्त (d == get_globvar_dict()
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
    सचे (dict_wrong_func_name(d, &item->di_tv, item->di_कुञ्चिका))
	पटिनिवत्त FAIL;
    पटिनिवत्त hash_add(&d->dv_hashtab, item->di_कुञ्चिका, "add to dictionary");
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
    सचे (item == रिक्त)
	पटिनिवत्त FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_सङ्ख्या = nr;
    सचे (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_सङ्ख्या(dict_T *d, वर्ण *कुञ्चिका, varnumber_T nr)
{
    पटिनिवत्त dict_add_सङ्ख्या_special(d, कुञ्चिका, nr, VAR_सङ्ख्या);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_bool(dict_T *d, वर्ण *कुञ्चिका, varnumber_T nr)
{
    पटिनिवत्त dict_add_सङ्ख्या_special(d, कुञ्चिका, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_string(dict_T *d, वर्ण *कुञ्चिका, char_u *str)
{
    पटिनिवत्त dict_add_string_len(d, कुञ्चिका, str, -1);
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
    सचे (item == रिक्त)
	पटिनिवत्त FAIL;
    item->di_tv.v_type = VAR_STRING;
    सचे (str != रिक्त)
    {
	सचे (len == -1)
	    val = vim_strsave(str);
	अञ्ञथा
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    सचे (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
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
    सचे (item == रिक्त)
	पटिनिवत्त FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    सचे (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = रिक्त;
	dictitem_free(item);
	पटिनिवत्त FAIL;
    }
    ++list->lv_refcount;
    पटिनिवत्त OK;
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
    सचे (item == रिक्त)
	पटिनिवत्त FAIL;
    copy_tv(tv, &item->di_tv);
    सचे (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
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
    सचे (item == रिक्त)
	पटिनिवत्त FAIL;
    put_callback(cb, &item->di_tv);
    सचे (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
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
    सचे (item == रिक्त)
	पटिनिवत्त FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    सचे (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
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
    सचे (var->v_type != VAR_DICT || var->vval.v_dict == रिक्त)
	iter->dit_todo = 0;
    अञ्ञथा
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

    सचे (iter->dit_todo == 0)
	पटिनिवत्त रिक्त;

    याव (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_कुञ्चिका;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    पटिनिवत्त result;
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
    सचे (item == रिक्त)
	पटिनिवत्त FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    सचे (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = रिक्त;
	dictitem_free(item);
	पटिनिवत्त FAIL;
    }
    ++dict->dv_refcount;
    पटिनिवत्त OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    सचे (d == रिक्त)
	पटिनिवत्त 0L;
    पटिनिवत्त (long)d->dv_hashtab.ht_used;
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

    सचे (d == रिक्त)
	पटिनिवत्त रिक्त;
    सचे (len < 0)
	akey = कुञ्चिका;
    अञ्ञथा सचे (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(कुञ्चिका, len);
	सचे (akey == रिक्त)
	    पटिनिवत्त रिक्त;
    }
    अञ्ञथा
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, कुञ्चिका, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    सचे (HASHITEM_EMPTY(hi))
	पटिनिवत्त रिक्त;
    पटिनिवत्त HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    पूर्णाङ्क
dict_has_कुञ्चिका(dict_T *d, वर्ण *कुञ्चिका)
{
    पटिनिवत्त dict_find(d, (char_u *)कुञ्चिका, -1) != रिक्त;
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
    सचे (di == रिक्त)
	पटिनिवत्त FAIL;
    copy_tv(&di->di_tv, rettv);
    पटिनिवत्त OK;
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
    सचे (di == रिक्त)
	पटिनिवत्त रिक्त;
    s = tv_get_string(&di->di_tv);
    सचे (save && s != रिक्त)
	s = vim_strsave(s);
    पटिनिवत्त s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_सङ्ख्या(dict_T *d, वर्ण *कुञ्चिका)
{
    पटिनिवत्त dict_get_सङ्ख्या_def(d, कुञ्चिका, 0);
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
    सचे (di == रिक्त)
	पटिनिवत्त def;
    पटिनिवत्त tv_get_सङ्ख्या(&di->di_tv);
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
    सचे (di == रिक्त)
	पटिनिवत्त 0;
    सचे (di->di_tv.v_type != VAR_सङ्ख्या)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	पटिनिवत्त 0;
    }
    पटिनिवत्त tv_get_सङ्ख्या(&di->di_tv);
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
    सचे (di == रिक्त)
	पटिनिवत्त def;
    पटिनिवत्त tv_get_bool(&di->di_tv);
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

    सचे ((d = tv->vval.v_dict) == रिक्त)
	पटिनिवत्त रिक्त;
    ga_init2(&ga, परिमाण(वर्ण), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    सचे (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(&d->dv_hashtab, hi, todo)
    {
	सचे (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    सचे (first)
		first = FALSE;
	    अञ्ञथा
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_कुञ्चिका, FALSE);
	    सचे (tofree != रिक्त)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    सचे (s != रिक्त)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    सचे (s == रिक्त || did_echo_string_emsg)
		भङ्ग;
	    पङ्क्तिः_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    सचे (todo > 0)
    {
	vim_free(ga.ga_data);
	पटिनिवत्त रिक्त;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    पटिनिवत्त (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    स्थिर char_u *
skip_literal_कुञ्चिका(char_u *कुञ्चिका)
{
    char_u *p;

    अत्थाय (p = कुञ्चिका; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    पटिनिवत्त p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    स्थिर पूर्णाङ्क
get_literal_कुञ्चिका_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_कुञ्चिका(*arg);

    सचे (p == *arg)
	पटिनिवत्त FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    पटिनिवत्त OK;
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

    सचे (**arg == '\'')
    {
	सचे (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    पटिनिवत्त रिक्त;
	कुञ्चिका = rettv.vval.v_string;
    }
    अञ्ञथा सचे (**arg == '"')
    {
	सचे (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    पटिनिवत्त रिक्त;
	कुञ्चिका = rettv.vval.v_string;
    }
    अञ्ञथा
    {
	end = skip_literal_कुञ्चिका(*arg);
	सचे (end == *arg)
	{
	    semsg(_(e_invalid_कुञ्चिका_str), *arg);
	    पटिनिवत्त रिक्त;
	}
	कुञ्चिका = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    पटिनिवत्त कुञ्चिका;
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
    सचे (!vim9script
	    && *curly_व्यञ्जकम् != '}'
	    && !literal
	    && eval1(&curly_व्यञ्जकम्, &tv, रिक्त) == OK
	    && *skipwhite(curly_व्यञ्जकम्) == '}')
	पटिनिवत्त NOTDONE;

    सचे (evaluate)
    {
	d = dict_alloc();
	सचे (d == रिक्त)
	    पटिनिवत्त FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    याव (**arg != '}' && **arg != NUL)
    {
	पूर्णाङ्क	has_bracket = vim9script && **arg == '[';

	सचे (literal)
	{
	    सचे (get_literal_कुञ्चिका_tv(arg, &tvkey) == FAIL)
		गच्छ failret;
	}
	अञ्ञथा सचे (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_कुञ्चिका(arg);
	    सचे (tvkey.vval.v_string == रिक्त)
		गच्छ failret;
	    tvkey.v_type = VAR_STRING;
	}
	अञ्ञथा
	{
	    सचे (has_bracket)
		*arg = skipwhite(*arg + 1);
	    सचे (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		गच्छ failret;
	    सचे (has_bracket)
	    {
		*arg = skipwhite(*arg);
		सचे (**arg != ']')
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
	सचे (!vim9script)
	    *arg = skipwhite(*arg);
	सचे (**arg != ':')
	{
	    सचे (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    अञ्ञथा
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    गच्छ failret;
	}
	सचे (evaluate)
	{
	    सचे (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    कुञ्चिका = tv_get_string_buf_chk(&tvkey, buf);
	    सचे (कुञ्चिका == रिक्त)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		गच्छ failret;
	    }
	}
	सचे (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    गच्छ failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	सचे (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    गच्छ failret;
	}
	सचे (check_typval_is_मानम्(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    गच्छ failret;
	}
	सचे (evaluate)
	{
	    item = dict_find(d, कुञ्चिका, -1);
	    सचे (item != रिक्त)
	    {
		semsg(_(e_duplicate_कुञ्चिका_in_dictionary_str), कुञ्चिका);
		clear_tv(&tvkey);
		clear_tv(&tv);
		गच्छ failret;
	    }
	    item = dictitem_alloc(कुञ्चिका);
	    सचे (item != रिक्त)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		सचे (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	सचे (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	सचे (had_comma)
	{
	    सचे (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		गच्छ failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	सचे (**arg == '}')
	    भङ्ग;
	सचे (!had_comma)
	{
	    सचे (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    अञ्ञथा
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    गच्छ failret;
	}
    }

    सचे (**arg != '}')
    {
	सचे (evalarg != रिक्त)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	सचे (d != रिक्त)
	    dict_free(d);
	पटिनिवत्त FAIL;
    }

    *arg = *arg + 1;
    सचे (evaluate)
	rettv_dict_set(rettv, d);

    पटिनिवत्त OK;
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

    सचे (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    अञ्ञथा सचे ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    अञ्ञथा
	ret = NOTDONE;

    पटिनिवत्त ret;
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

    सचे (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	पटिनिवत्त;

    सचे (*action == 'm')
    {
	सचे (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    पटिनिवत्त;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    सचे (d1->dv_type != रिक्त && d1->dv_type->tt_member != रिक्त)
	type = d1->dv_type->tt_member;
    अञ्ञथा
	type = रिक्त;

    todo = (पूर्णाङ्क)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_वस्तूनि(&d2->dv_hashtab, hi2, todo)
    {
	सचे (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_कुञ्चिका, -1);
	    // Check the key to be valid when adding to any scope.
	    सचे (d1->dv_scope != 0 && !valid_varname(hi2->hi_कुञ्चिका, -1, TRUE))
		भङ्ग;

	    सचे (type != रिक्त
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		भङ्ग;

	    सचे (di1 == रिक्त)
	    {
		सचे (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    सचे (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		अञ्ञथा
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    सचे (di1 != रिक्त && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    अञ्ञथा सचे (*action == 'e')
	    {
		semsg(_(e_कुञ्चिका_already_exists_str), hi2->hi_कुञ्चिका);
		भङ्ग;
	    }
	    अञ्ञथा सचे (*action == 'f' && HI2DI(hi2) != di1)
	    {
		सचे (मानम्_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    भङ्ग;
		// Disallow replacing a builtin function.
		सचे (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_कुञ्चिका))
		    भङ्ग;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    सचे (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    पटिनिवत्त HI2DI(hi);
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

    सचे (d1 == d2)
	पटिनिवत्त TRUE;
    सचे (dict_len(d1) != dict_len(d2))
	पटिनिवत्त FALSE;
    सचे (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	पटिनिवत्त TRUE;
    सचे (d1 == रिक्त || d2 == रिक्त)
	पटिनिवत्त FALSE;

    todo = (पूर्णाङ्क)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(&d1->dv_hashtab, hi, todo)
    {
	सचे (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_कुञ्चिका, -1);
	    सचे (item2 == रिक्त)
		पटिनिवत्त FALSE;
	    सचे (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		पटिनिवत्त FALSE;
	    --todo;
	}
    }
    पटिनिवत्त TRUE;
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

    सचे (d == रिक्त)
	पटिनिवत्त 0;

    todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(&d->dv_hashtab, hi, todo)
    {
	सचे (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    सचे (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    पटिनिवत्त n;
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
    सचे (d1 == रिक्त)
    {
	emsg(_(e_cannot_extend_null_dict));
	पटिनिवत्त;
    }

    सचे (!is_new && मानम्_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	पटिनिवत्त;

    सचे (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	सचे (d1 == रिक्त)
	    पटिनिवत्त;
    }

    d2 = argvars[1].vval.v_dict;
    सचे (d2 == रिक्त)
	गच्छ theend;

    // Check the third argument.
    सचे (argvars[2].v_type != VAR_UNKNOWN)
    {
	स्थिर वर्ण *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	सचे (action == रिक्त)
	{
	    सचे (is_new)
		dict_unref(d1);
	    पटिनिवत्त;
	}
	अत्थाय (i = 0; i < 3; ++i)
	    सचे (STRCMP(action, av[i]) == 0)
		भङ्ग;
	सचे (i == 3)
	{
	    सचे (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    पटिनिवत्त;
	}
    }
    अञ्ञथा
	action = (char_u *)"force";

    सचे (type != रिक्त && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	सचे (is_new)
	    dict_unref(d1);
	पटिनिवत्त;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    सचे (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    अञ्ञथा
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

    सचे (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = रिक्त;
    }
    सचे (d == रिक्त
	  || (filtermap == FILTERMAP_FILTER
			&& मानम्_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	पटिनिवत्त;

    सचे (filtermap == FILTERMAP_MAPNEW)
    {
	सचे (rettv_dict_alloc(rettv) == FAIL)
	    पटिनिवत्त;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_व्यञ्जकम्_get_funccal(व्यञ्जकम्, &newtv);

    पूर्णाङ्क prev_lock = d->dv_lock;
    सचे (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (पूर्णाङ्क)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तूनि(ht, hi, todo)
    {
	सचे (!HASHITEM_EMPTY(hi))
	{
	    पूर्णाङ्क		r;

	    --todo;
	    di = HI2DI(hi);
	    सचे (filtermap == FILTERMAP_MAP
		    && (मानम्_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		भङ्ग;
	    set_vim_var_string(VV_कुञ्चिका, di->di_कुञ्चिका, -1);
	    r = filter_map_one(&di->di_tv, व्यञ्जकम्, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_कुञ्चिका));
	    सचे (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		भङ्ग;
	    }
	    सचे (filtermap == FILTERMAP_MAP)
	    {
		सचे (argtype != रिक्त && check_typval_arg_type(
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
	    अञ्ञथा सचे (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (वर्ण *)di->di_कुञ्चिका, &newtv);
		clear_tv(&newtv);
		सचे (r == FAIL)
		    भङ्ग;
	    }
	    अञ्ञथा सचे (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		सचे (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    भङ्ग;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    सचे (fc != रिक्त)
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

    सचे (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	पटिनिवत्त;
    }

    d = argvars[0].vval.v_dict;
    सचे (d == रिक्त || मानम्_check_lock(d->dv_lock, arg_errmsg, TRUE))
	पटिनिवत्त;

    कुञ्चिका = tv_get_string_chk(&argvars[1]);
    सचे (कुञ्चिका == रिक्त)
	पटिनिवत्त;

    di = dict_find(d, कुञ्चिका, -1);
    सचे (di == रिक्त)
    {
	semsg(_(e_कुञ्चिका_not_present_in_dictionary_str), कुञ्चिका);
	पटिनिवत्त;
    }

    सचे (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	पटिनिवत्त;

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

    सचे (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(शून्य)rettv_list_alloc(rettv);
	पटिनिवत्त;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == रिक्त ? 0 : (पूर्णाङ्क)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_वस्तूनि(todo);
    सचे (l == रिक्त)
    {
	(शून्य)rettv_list_alloc(rettv);
	पटिनिवत्त;
    }
    rettv_list_set(rettv, l);
    सचे (todo == 0)
	// NULL or empty dict, return the empty list
	पटिनिवत्त;

    li = l->lv_first;
    FOR_ALL_HASHTAB_वस्तूनि(&d->dv_hashtab, hi, todo)
    {
	सचे (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    सचे (what == DICT2_LIST_कुञ्चिकाः)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_कुञ्चिका);
	    }
	    अञ्ञथा सचे (what == DICT2_LIST_मानानि)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    अञ्ञथा
	    {
		// items()
		l2 = list_alloc_with_वस्तूनि(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		सचे (l2 == रिक्त)
		    गच्छ alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_कुञ्चिका);
		सचे (l2->lv_first->li_tv.vval.v_string == रिक्त)
		    गच्छ alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    पटिनिवत्त;

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
	सचे (HASHITEM_EMPTY(hi))
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
    सचे (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    सचे (check_for_dict_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (argvars[0].vval.v_dict == रिक्त)
	पटिनिवत्त;

    rettv->vval.v_सङ्ख्या = dict_has_कुञ्चिका(argvars[0].vval.v_dict,
				(वर्ण *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
