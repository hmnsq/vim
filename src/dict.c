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

#यदि defined(FEAT_EVAL)

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
    यदि (d == रिक्त)
	फर्काऊ रिक्त;

    // Add the dict to the list of dicts for garbage collection.
    यदि (first_dict != रिक्त)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = रिक्त;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    फर्काऊ d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    यदि (alloc_fail_id == id && alloc_does_fail(परिमाण(list_T)))
	फर्काऊ रिक्त;
#endif
    फर्काऊ (dict_alloc());
}

    dict_T *
dict_alloc_lock(पूर्णाङ्क lock)
{
    dict_T *d = dict_alloc();

    यदि (d != रिक्त)
	d->dv_lock = lock;
    फर्काऊ d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    यदि (d == रिक्त)
	फर्काऊ FAIL;

    rettv_dict_set(rettv, d);
    फर्काऊ OK;
}

/*
 * Set a dictionary as the return value
 */
    शून्य
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    यदि (d != रिक्त)
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

    यदि (check_hashtab_frozen(ht, "clear dict"))
	फर्काऊ;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (पूर्णाङ्क)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तुहरू(ht, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
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
    यदि (d->dv_used_prev == रिक्त)
	first_dict = d->dv_used_next;
    अन्यथा
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    यदि (d->dv_used_next != रिक्त)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    स्थिर शून्य
dict_free(dict_T *d)
{
    यदि (!in_free_unref_वस्तुहरू)
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
    यदि (d != रिक्त && --d->dv_refcount <= 0)
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

    लागि (dd = first_dict; dd != रिक्त; dd = dd->dv_used_next)
	यदि ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    फर्काऊ did_free;
}

    शून्य
dict_free_वस्तुहरू(पूर्णाङ्क copyID)
{
    dict_T	*dd, *dd_next;

    लागि (dd = first_dict; dd != रिक्त; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	यदि ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
dictitem_alloc(char_u *साँचो)
{
    dictitem_T *di;
    माप_प्रकार len = STRLEN(साँचो);

    di = alloc(दुरत्व(dictitem_T, di_साँचो) + len + 1);
    यदि (di == रिक्त)
	फर्काऊ रिक्त;

    mch_memmove(di->di_साँचो, साँचो, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    फर्काऊ di;
}

/*
 * Make a copy of a Dictionary item.
 */
    स्थिर dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    माप_प्रकार	len = STRLEN(org->di_साँचो);

    di = alloc(दुरत्व(dictitem_T, di_साँचो) + len + 1);
    यदि (di == रिक्त)
	फर्काऊ रिक्त;

    mch_memmove(di->di_साँचो, org->di_साँचो, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    फर्काऊ di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    शून्य
dictitem_remove(dict_T *dict, dictitem_T *item, वर्ण *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_साँचो);
    यदि (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    अन्यथा
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
    यदि (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, पूर्णाङ्क deep, पूर्णाङ्क शीर्ष, पूर्णाङ्क copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    पूर्णाङ्क		todo;
    hashitem_T	*hi;

    यदि (orig == रिक्त)
	फर्काऊ रिक्त;

    copy = dict_alloc();
    यदि (copy == रिक्त)
	फर्काऊ रिक्त;

    यदि (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    यदि (orig->dv_type == रिक्त || शीर्ष || deep)
	copy->dv_type = रिक्त;
    अन्यथा
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (पूर्णाङ्क)orig->dv_hashtab.ht_used;
    लागि (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_साँचो);
	    यदि (di == रिक्त)
		रोक;
	    यदि (deep)
	    {
		यदि (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    रोक;
		}
	    }
	    अन्यथा
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    यदि (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		रोक;
	    }
	}
    }

    ++copy->dv_refcount;
    यदि (todo > 0)
    {
	dict_unref(copy);
	copy = रिक्त;
    }

    फर्काऊ copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    पूर्णाङ्क
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    फर्काऊ (d == get_globvar_dict()
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
    यदि (dict_wrong_func_name(d, &item->di_tv, item->di_साँचो))
	फर्काऊ FAIL;
    फर्काऊ hash_add(&d->dv_hashtab, item->di_साँचो, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    स्थिर पूर्णाङ्क
dict_add_सङ्ख्या_special(
    dict_T	*d,
    वर्ण	*साँचो,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)साँचो);
    यदि (item == रिक्त)
	फर्काऊ FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_सङ्ख्या = nr;
    यदि (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_सङ्ख्या(dict_T *d, वर्ण *साँचो, varnumber_T nr)
{
    फर्काऊ dict_add_सङ्ख्या_special(d, साँचो, nr, VAR_सङ्ख्या);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_bool(dict_T *d, वर्ण *साँचो, varnumber_T nr)
{
    फर्काऊ dict_add_सङ्ख्या_special(d, साँचो, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_string(dict_T *d, वर्ण *साँचो, char_u *str)
{
    फर्काऊ dict_add_string_len(d, साँचो, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_string_len(dict_T *d, वर्ण *साँचो, char_u *str, पूर्णाङ्क len)
{
    dictitem_T	*item;
    char_u	*val = रिक्त;

    item = dictitem_alloc((char_u *)साँचो);
    यदि (item == रिक्त)
	फर्काऊ FAIL;
    item->di_tv.v_type = VAR_STRING;
    यदि (str != रिक्त)
    {
	यदि (len == -1)
	    val = vim_strsave(str);
	अन्यथा
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    यदि (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_list(dict_T *d, वर्ण *साँचो, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)साँचो);
    यदि (item == रिक्त)
	फर्काऊ FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    यदि (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = रिक्त;
	dictitem_free(item);
	फर्काऊ FAIL;
    }
    ++list->lv_refcount;
    फर्काऊ OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_tv(dict_T *d, वर्ण *साँचो, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)साँचो);
    यदि (item == रिक्त)
	फर्काऊ FAIL;
    copy_tv(tv, &item->di_tv);
    यदि (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_callback(dict_T *d, वर्ण *साँचो, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)साँचो);
    यदि (item == रिक्त)
	फर्काऊ FAIL;
    put_callback(cb, &item->di_tv);
    यदि (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_func(dict_T *d, वर्ण *साँचो, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)साँचो);
    यदि (item == रिक्त)
	फर्काऊ FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    यदि (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
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
    यदि (var->v_type != VAR_DICT || var->vval.v_dict == रिक्त)
	iter->dit_todo = 0;
    अन्यथा
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

    यदि (iter->dit_todo == 0)
	फर्काऊ रिक्त;

    जबसम्म (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_साँचो;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    फर्काऊ result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    पूर्णाङ्क
dict_add_dict(dict_T *d, वर्ण *साँचो, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)साँचो);
    यदि (item == रिक्त)
	फर्काऊ FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    यदि (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = रिक्त;
	dictitem_free(item);
	फर्काऊ FAIL;
    }
    ++dict->dv_refcount;
    फर्काऊ OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    दीर्घ
dict_len(dict_T *d)
{
    यदि (d == रिक्त)
	फर्काऊ 0L;
    फर्काऊ (दीर्घ)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *साँचो, पूर्णाङ्क len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = रिक्त;
    hashitem_T	*hi;

    यदि (d == रिक्त)
	फर्काऊ रिक्त;
    यदि (len < 0)
	akey = साँचो;
    अन्यथा यदि (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(साँचो, len);
	यदि (akey == रिक्त)
	    फर्काऊ रिक्त;
    }
    अन्यथा
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, साँचो, (माप_प्रकार)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    यदि (HASHITEM_EMPTY(hi))
	फर्काऊ रिक्त;
    फर्काऊ HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    पूर्णाङ्क
dict_has_साँचो(dict_T *d, वर्ण *साँचो)
{
    फर्काऊ dict_find(d, (char_u *)साँचो, -1) != रिक्त;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    पूर्णाङ्क
dict_get_tv(dict_T *d, वर्ण *साँचो, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)साँचो, -1);
    यदि (di == रिक्त)
	फर्काऊ FAIL;
    copy_tv(&di->di_tv, rettv);
    फर्काऊ OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, वर्ण *साँचो, पूर्णाङ्क save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)साँचो, -1);
    यदि (di == रिक्त)
	फर्काऊ रिक्त;
    s = tv_get_string(&di->di_tv);
    यदि (save && s != रिक्त)
	s = vim_strsave(s);
    फर्काऊ s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_सङ्ख्या(dict_T *d, वर्ण *साँचो)
{
    फर्काऊ dict_get_सङ्ख्या_def(d, साँचो, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_सङ्ख्या_def(dict_T *d, वर्ण *साँचो, पूर्णाङ्क def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)साँचो, -1);
    यदि (di == रिक्त)
	फर्काऊ def;
    फर्काऊ tv_get_सङ्ख्या(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_सङ्ख्या_check(dict_T *d, char_u *साँचो)
{
    dictitem_T	*di;

    di = dict_find(d, साँचो, -1);
    यदि (di == रिक्त)
	फर्काऊ 0;
    यदि (di->di_tv.v_type != VAR_सङ्ख्या)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	फर्काऊ 0;
    }
    फर्काऊ tv_get_सङ्ख्या(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, वर्ण *साँचो, पूर्णाङ्क def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)साँचो, -1);
    यदि (di == रिक्त)
	फर्काऊ def;
    फर्काऊ tv_get_bool(&di->di_tv);
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

    यदि ((d = tv->vval.v_dict) == रिक्त)
	फर्काऊ रिक्त;
    ga_init2(&ga, परिमाण(वर्ण), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    यदि (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_वस्तुहरू(&d->dv_hashtab, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    यदि (first)
		first = FALSE;
	    अन्यथा
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_साँचो, FALSE);
	    यदि (tofree != रिक्त)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    यदि (s != रिक्त)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    यदि (s == रिक्त || did_echo_string_emsg)
		रोक;
	    पङ्क्ति_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    यदि (todo > 0)
    {
	vim_free(ga.ga_data);
	फर्काऊ रिक्त;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    फर्काऊ (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    स्थिर char_u *
skip_literal_साँचो(char_u *साँचो)
{
    char_u *p;

    लागि (p = साँचो; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    फर्काऊ p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    स्थिर पूर्णाङ्क
get_literal_साँचो_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_साँचो(*arg);

    यदि (p == *arg)
	फर्काऊ FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    फर्काऊ OK;
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
get_literal_साँचो(char_u **arg)
{
    char_u	*साँचो;
    char_u	*end;
    typval_T	rettv;

    यदि (**arg == '\'')
    {
	यदि (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    फर्काऊ रिक्त;
	साँचो = rettv.vval.v_string;
    }
    अन्यथा यदि (**arg == '"')
    {
	यदि (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    फर्काऊ रिक्त;
	साँचो = rettv.vval.v_string;
    }
    अन्यथा
    {
	end = skip_literal_साँचो(*arg);
	यदि (end == *arg)
	{
	    semsg(_(e_invalid_साँचो_str), *arg);
	    फर्काऊ रिक्त;
	}
	साँचो = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    फर्काऊ साँचो;
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
    char_u	*साँचो = रिक्त;
    dictitem_T	*item;
    char_u	*curly_अभिव्यक्ति = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    पूर्णाङ्क		vim9script = in_vim9script();
    पूर्णाङ्क		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    यदि (!vim9script
	    && *curly_अभिव्यक्ति != '}'
	    && !literal
	    && eval1(&curly_अभिव्यक्ति, &tv, रिक्त) == OK
	    && *skipwhite(curly_अभिव्यक्ति) == '}')
	फर्काऊ NOTDONE;

    यदि (evaluate)
    {
	d = dict_alloc();
	यदि (d == रिक्त)
	    फर्काऊ FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    जबसम्म (**arg != '}' && **arg != NUL)
    {
	पूर्णाङ्क	has_bracket = vim9script && **arg == '[';

	यदि (literal)
	{
	    यदि (get_literal_साँचो_tv(arg, &tvkey) == FAIL)
		गच्छ failret;
	}
	अन्यथा यदि (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_साँचो(arg);
	    यदि (tvkey.vval.v_string == रिक्त)
		गच्छ failret;
	    tvkey.v_type = VAR_STRING;
	}
	अन्यथा
	{
	    यदि (has_bracket)
		*arg = skipwhite(*arg + 1);
	    यदि (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		गच्छ failret;
	    यदि (has_bracket)
	    {
		*arg = skipwhite(*arg);
		यदि (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_साँचो));
		    clear_tv(&tvkey);
		    गच्छ failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	यदि (!vim9script)
	    *arg = skipwhite(*arg);
	यदि (**arg != ':')
	{
	    यदि (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    अन्यथा
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    गच्छ failret;
	}
	यदि (evaluate)
	{
	    यदि (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    साँचो = tv_get_string_buf_chk(&tvkey, buf);
	    यदि (साँचो == रिक्त)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		गच्छ failret;
	    }
	}
	यदि (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    गच्छ failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	यदि (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    गच्छ failret;
	}
	यदि (check_typval_is_मान(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    गच्छ failret;
	}
	यदि (evaluate)
	{
	    item = dict_find(d, साँचो, -1);
	    यदि (item != रिक्त)
	    {
		semsg(_(e_duplicate_साँचो_in_dictionary_str), साँचो);
		clear_tv(&tvkey);
		clear_tv(&tv);
		गच्छ failret;
	    }
	    item = dictitem_alloc(साँचो);
	    यदि (item != रिक्त)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		यदि (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	यदि (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	यदि (had_comma)
	{
	    यदि (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		गच्छ failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	यदि (**arg == '}')
	    रोक;
	यदि (!had_comma)
	{
	    यदि (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    अन्यथा
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    गच्छ failret;
	}
    }

    यदि (**arg != '}')
    {
	यदि (evalarg != रिक्त)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	यदि (d != रिक्त)
	    dict_free(d);
	फर्काऊ FAIL;
    }

    *arg = *arg + 1;
    यदि (evaluate)
	rettv_dict_set(rettv, d);

    फर्काऊ OK;
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

    यदि (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    अन्यथा यदि ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    अन्यथा
	ret = NOTDONE;

    फर्काऊ ret;
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

    यदि (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	फर्काऊ;

    यदि (*action == 'm')
    {
	यदि (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    फर्काऊ;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    यदि (d1->dv_type != रिक्त && d1->dv_type->tt_member != रिक्त)
	type = d1->dv_type->tt_member;
    अन्यथा
	type = रिक्त;

    todo = (पूर्णाङ्क)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_वस्तुहरू(&d2->dv_hashtab, hi2, todo)
    {
	यदि (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_साँचो, -1);
	    // Check the key to be valid when adding to any scope.
	    यदि (d1->dv_scope != 0 && !valid_varname(hi2->hi_साँचो, -1, TRUE))
		रोक;

	    यदि (type != रिक्त
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		रोक;

	    यदि (di1 == रिक्त)
	    {
		यदि (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    यदि (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		अन्यथा
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    यदि (di1 != रिक्त && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    अन्यथा यदि (*action == 'e')
	    {
		semsg(_(e_साँचो_already_exists_str), hi2->hi_साँचो);
		रोक;
	    }
	    अन्यथा यदि (*action == 'f' && HI2DI(hi2) != di1)
	    {
		यदि (मान_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    रोक;
		// Disallow replacing a builtin function.
		यदि (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_साँचो))
		    रोक;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    यदि (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    फर्काऊ HI2DI(hi);
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

    यदि (d1 == d2)
	फर्काऊ TRUE;
    यदि (dict_len(d1) != dict_len(d2))
	फर्काऊ FALSE;
    यदि (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	फर्काऊ TRUE;
    यदि (d1 == रिक्त || d2 == रिक्त)
	फर्काऊ FALSE;

    todo = (पूर्णाङ्क)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_वस्तुहरू(&d1->dv_hashtab, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_साँचो, -1);
	    यदि (item2 == रिक्त)
		फर्काऊ FALSE;
	    यदि (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		फर्काऊ FALSE;
	    --todo;
	}
    }
    फर्काऊ TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    दीर्घ
dict_गन्ती(dict_T *d, typval_T *needle, पूर्णाङ्क ic)
{
    पूर्णाङ्क		todo;
    hashitem_T	*hi;
    दीर्घ	n = 0;

    यदि (d == रिक्त)
	फर्काऊ 0;

    todo = (पूर्णाङ्क)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_वस्तुहरू(&d->dv_hashtab, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    यदि (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    फर्काऊ n;
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
    यदि (d1 == रिक्त)
    {
	emsg(_(e_cannot_extend_null_dict));
	फर्काऊ;
    }

    यदि (!is_new && मान_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	फर्काऊ;

    यदि (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	यदि (d1 == रिक्त)
	    फर्काऊ;
    }

    d2 = argvars[1].vval.v_dict;
    यदि (d2 == रिक्त)
	गच्छ theend;

    // Check the third argument.
    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	स्थिर वर्ण *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	यदि (action == रिक्त)
	{
	    यदि (is_new)
		dict_unref(d1);
	    फर्काऊ;
	}
	लागि (i = 0; i < 3; ++i)
	    यदि (STRCMP(action, av[i]) == 0)
		रोक;
	यदि (i == 3)
	{
	    यदि (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    फर्काऊ;
	}
    }
    अन्यथा
	action = (char_u *)"force";

    यदि (type != रिक्त && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	यदि (is_new)
	    dict_unref(d1);
	फर्काऊ;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    यदि (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    अन्यथा
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
	typval_T	*अभिव्यक्ति,
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

    यदि (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = रिक्त;
    }
    यदि (d == रिक्त
	  || (filtermap == FILTERMAP_FILTER
			&& मान_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	फर्काऊ;

    यदि (filtermap == FILTERMAP_MAPNEW)
    {
	यदि (rettv_dict_alloc(rettv) == FAIL)
	    फर्काऊ;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_अभिव्यक्ति_get_funccal(अभिव्यक्ति, &newtv);

    पूर्णाङ्क prev_lock = d->dv_lock;
    यदि (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (पूर्णाङ्क)ht->ht_used;
    FOR_ALL_HASHTAB_वस्तुहरू(ht, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    पूर्णाङ्क		r;

	    --todo;
	    di = HI2DI(hi);
	    यदि (filtermap == FILTERMAP_MAP
		    && (मान_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		रोक;
	    set_vim_var_string(VV_साँचो, di->di_साँचो, -1);
	    r = filter_map_one(&di->di_tv, अभिव्यक्ति, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_साँचो));
	    यदि (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		रोक;
	    }
	    यदि (filtermap == FILTERMAP_MAP)
	    {
		यदि (argtype != रिक्त && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    रोक;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    अन्यथा यदि (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (वर्ण *)di->di_साँचो, &newtv);
		clear_tv(&newtv);
		यदि (r == FAIL)
		    रोक;
	    }
	    अन्यथा यदि (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		यदि (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    रोक;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    यदि (fc != रिक्त)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    शून्य
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*साँचो;
    dictitem_T	*di;

    यदि (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	फर्काऊ;
    }

    d = argvars[0].vval.v_dict;
    यदि (d == रिक्त || मान_check_lock(d->dv_lock, arg_errmsg, TRUE))
	फर्काऊ;

    साँचो = tv_get_string_chk(&argvars[1]);
    यदि (साँचो == रिक्त)
	फर्काऊ;

    di = dict_find(d, साँचो, -1);
    यदि (di == रिक्त)
    {
	semsg(_(e_साँचो_not_present_in_dictionary_str), साँचो);
	फर्काऊ;
    }

    यदि (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	फर्काऊ;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

प्रकारनिर्देश गणना {
    DICT2_LIST_साँचोहरू,
    DICT2_LIST_मानहरू,
    DICT2_LIST_वस्तुहरू,
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

    यदि (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(शून्य)rettv_list_alloc(rettv);
	फर्काऊ;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == रिक्त ? 0 : (पूर्णाङ्क)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_वस्तुहरू(todo);
    यदि (l == रिक्त)
    {
	(शून्य)rettv_list_alloc(rettv);
	फर्काऊ;
    }
    rettv_list_set(rettv, l);
    यदि (todo == 0)
	// NULL or empty dict, return the empty list
	फर्काऊ;

    li = l->lv_first;
    FOR_ALL_HASHTAB_वस्तुहरू(&d->dv_hashtab, hi, todo)
    {
	यदि (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    यदि (what == DICT2_LIST_साँचोहरू)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_साँचो);
	    }
	    अन्यथा यदि (what == DICT2_LIST_मानहरू)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    अन्यथा
	    {
		// items()
		l2 = list_alloc_with_वस्तुहरू(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		यदि (l2 == रिक्त)
		    गच्छ alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_साँचो);
		यदि (l2->lv_first->li_tv.vval.v_string == रिक्त)
		    गच्छ alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    फर्काऊ;

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
    dict2list(argvars, rettv, DICT2_LIST_वस्तुहरू);
}

/*
 * "keys()" function
 */
    शून्य
f_साँचोहरू(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_साँचोहरू);
}

/*
 * "values(dict)" function
 */
    शून्य
f_मानहरू(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_मानहरू);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    शून्य
dict_set_वस्तुहरू_ro(dict_T *di)
{
    पूर्णाङ्क		todo = (पूर्णाङ्क)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_वस्तुहरू(&di->dv_hashtab, hi, todo)
    {
	यदि (HASHITEM_EMPTY(hi))
	    जारी;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    शून्य
f_has_साँचो(typval_T *argvars, typval_T *rettv)
{
    यदि (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_सङ्ख्या_arg(argvars, 1) == FAIL))
	फर्काऊ;

    यदि (check_for_dict_arg(argvars, 0) == FAIL)
	फर्काऊ;

    यदि (argvars[0].vval.v_dict == रिक्त)
	फर्काऊ;

    rettv->vval.v_सङ्ख्या = dict_has_साँचो(argvars[0].vval.v_dict,
				(वर्ण *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
