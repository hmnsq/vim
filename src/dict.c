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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
സ്ഥിതം dict_T		*first_dict = ശൂന്യ;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(ശൂന്യം)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    എങ്കിൽ (d == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Add the dict to the list of dicts for garbage collection.
    എങ്കിൽ (first_dict != ശൂന്യ)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = ശൂന്യ;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    തിരികെ d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    എങ്കിൽ (alloc_fail_id == id && alloc_does_fail(വലുപ്പം(list_T)))
	തിരികെ ശൂന്യ;
#endif
    തിരികെ (dict_alloc());
}

    dict_T *
dict_alloc_lock(പൂർണ്ണസംഖ്യ lock)
{
    dict_T *d = dict_alloc();

    എങ്കിൽ (d != ശൂന്യ)
	d->dv_lock = lock;
    തിരികെ d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    എങ്കിൽ (d == ശൂന്യ)
	തിരികെ FAIL;

    rettv_dict_set(rettv, d);
    തിരികെ OK;
}

/*
 * Set a dictionary as the return value
 */
    ശൂന്യം
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    എങ്കിൽ (d != ശൂന്യ)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    ശൂന്യം
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = ശൂന്യ;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    ശൂന്യം
hashtab_free_contents(hashtab_T *ht)
{
    പൂർണ്ണസംഖ്യ		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    എങ്കിൽ (check_hashtab_frozen(ht, "clear dict"))
	തിരികെ;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (പൂർണ്ണസംഖ്യ)ht->ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(ht, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
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

    സ്ഥിതം ശൂന്യം
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    എങ്കിൽ (d->dv_used_prev == ശൂന്യ)
	first_dict = d->dv_used_next;
    അല്ലെങ്കിൽ
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    എങ്കിൽ (d->dv_used_next != ശൂന്യ)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    സ്ഥിതം ശൂന്യം
dict_free(dict_T *d)
{
    എങ്കിൽ (!in_free_unref_ഇനങ്ങൾ)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    ശൂന്യം
dict_unref(dict_T *d)
{
    എങ്കിൽ (d != ശൂന്യ && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    പൂർണ്ണസംഖ്യ
dict_free_nonref(പൂർണ്ണസംഖ്യ copyID)
{
    dict_T	*dd;
    പൂർണ്ണസംഖ്യ		did_free = FALSE;

    വേണ്ടി (dd = first_dict; dd != ശൂന്യ; dd = dd->dv_used_next)
	എങ്കിൽ ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    തിരികെ did_free;
}

    ശൂന്യം
dict_free_ഇനങ്ങൾ(പൂർണ്ണസംഖ്യ copyID)
{
    dict_T	*dd, *dd_next;

    വേണ്ടി (dd = first_dict; dd != ശൂന്യ; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	എങ്കിൽ ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
dictitem_alloc(char_u *താക്കോൽ)
{
    dictitem_T *di;
    size_t len = STRLEN(താക്കോൽ);

    di = alloc(offsetof(dictitem_T, di_താക്കോൽ) + len + 1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ ശൂന്യ;

    mch_memmove(di->di_താക്കോൽ, താക്കോൽ, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    തിരികെ di;
}

/*
 * Make a copy of a Dictionary item.
 */
    സ്ഥിതം dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_താക്കോൽ);

    di = alloc(offsetof(dictitem_T, di_താക്കോൽ) + len + 1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ ശൂന്യ;

    mch_memmove(di->di_താക്കോൽ, org->di_താക്കോൽ, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    തിരികെ di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    ശൂന്യം
dictitem_remove(dict_T *dict, dictitem_T *item, അക്ഷരം *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_താക്കോൽ);
    എങ്കിൽ (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    അല്ലെങ്കിൽ
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(item);
}

/*
 * Free a dict item.  Also clears the value.
 */
    ശൂന്യം
dictitem_free(dictitem_T *item)
{
    clear_tv(&item->di_tv);
    എങ്കിൽ (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, പൂർണ്ണസംഖ്യ deep, പൂർണ്ണസംഖ്യ മുകൾ, പൂർണ്ണസംഖ്യ copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    പൂർണ്ണസംഖ്യ		todo;
    hashitem_T	*hi;

    എങ്കിൽ (orig == ശൂന്യ)
	തിരികെ ശൂന്യ;

    copy = dict_alloc();
    എങ്കിൽ (copy == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    എങ്കിൽ (orig->dv_type == ശൂന്യ || മുകൾ || deep)
	copy->dv_type = ശൂന്യ;
    അല്ലെങ്കിൽ
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (പൂർണ്ണസംഖ്യ)orig->dv_hashtab.ht_used;
    വേണ്ടി (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_താക്കോൽ);
	    എങ്കിൽ (di == ശൂന്യ)
		നിർത്തുക;
	    എങ്കിൽ (deep)
	    {
		എങ്കിൽ (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    നിർത്തുക;
		}
	    }
	    അല്ലെങ്കിൽ
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    എങ്കിൽ (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		നിർത്തുക;
	    }
	}
    }

    ++copy->dv_refcount;
    എങ്കിൽ (todo > 0)
    {
	dict_unref(copy);
	copy = ശൂന്യ;
    }

    തിരികെ copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    പൂർണ്ണസംഖ്യ
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    തിരികെ (d == get_globvar_dict()
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
    പൂർണ്ണസംഖ്യ
dict_add(dict_T *d, dictitem_T *item)
{
    എങ്കിൽ (dict_wrong_func_name(d, &item->di_tv, item->di_താക്കോൽ))
	തിരികെ FAIL;
    തിരികെ hash_add(&d->dv_hashtab, item->di_താക്കോൽ, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
dict_add_സംഖ്യ_special(
    dict_T	*d,
    അക്ഷരം	*താക്കോൽ,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)താക്കോൽ);
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_സംഖ്യ = nr;
    എങ്കിൽ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    പൂർണ്ണസംഖ്യ
dict_add_സംഖ്യ(dict_T *d, അക്ഷരം *താക്കോൽ, varnumber_T nr)
{
    തിരികെ dict_add_സംഖ്യ_special(d, താക്കോൽ, nr, VAR_സംഖ്യ);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    പൂർണ്ണസംഖ്യ
dict_add_bool(dict_T *d, അക്ഷരം *താക്കോൽ, varnumber_T nr)
{
    തിരികെ dict_add_സംഖ്യ_special(d, താക്കോൽ, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    പൂർണ്ണസംഖ്യ
dict_add_string(dict_T *d, അക്ഷരം *താക്കോൽ, char_u *str)
{
    തിരികെ dict_add_string_len(d, താക്കോൽ, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    പൂർണ്ണസംഖ്യ
dict_add_string_len(dict_T *d, അക്ഷരം *താക്കോൽ, char_u *str, പൂർണ്ണസംഖ്യ len)
{
    dictitem_T	*item;
    char_u	*val = ശൂന്യ;

    item = dictitem_alloc((char_u *)താക്കോൽ);
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ FAIL;
    item->di_tv.v_type = VAR_STRING;
    എങ്കിൽ (str != ശൂന്യ)
    {
	എങ്കിൽ (len == -1)
	    val = vim_strsave(str);
	അല്ലെങ്കിൽ
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    എങ്കിൽ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    പൂർണ്ണസംഖ്യ
dict_add_list(dict_T *d, അക്ഷരം *താക്കോൽ, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)താക്കോൽ);
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    എങ്കിൽ (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = ശൂന്യ;
	dictitem_free(item);
	തിരികെ FAIL;
    }
    ++list->lv_refcount;
    തിരികെ OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    പൂർണ്ണസംഖ്യ
dict_add_tv(dict_T *d, അക്ഷരം *താക്കോൽ, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)താക്കോൽ);
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ FAIL;
    copy_tv(tv, &item->di_tv);
    എങ്കിൽ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    പൂർണ്ണസംഖ്യ
dict_add_callback(dict_T *d, അക്ഷരം *താക്കോൽ, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)താക്കോൽ);
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ FAIL;
    put_callback(cb, &item->di_tv);
    എങ്കിൽ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    പൂർണ്ണസംഖ്യ
dict_add_func(dict_T *d, അക്ഷരം *താക്കോൽ, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)താക്കോൽ);
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    എങ്കിൽ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    ശൂന്യം
dict_iterate_start(typval_T *var, dict_iterator_T *iter)
{
    എങ്കിൽ (var->v_type != VAR_DICT || var->vval.v_dict == ശൂന്യ)
	iter->dit_todo = 0;
    അല്ലെങ്കിൽ
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

    എങ്കിൽ (iter->dit_todo == 0)
	തിരികെ ശൂന്യ;

    ആകുമ്പോൾ (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_താക്കോൽ;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    തിരികെ result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    പൂർണ്ണസംഖ്യ
dict_add_dict(dict_T *d, അക്ഷരം *താക്കോൽ, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)താക്കോൽ);
    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    എങ്കിൽ (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = ശൂന്യ;
	dictitem_free(item);
	തിരികെ FAIL;
    }
    ++dict->dv_refcount;
    തിരികെ OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    എങ്കിൽ (d == ശൂന്യ)
	തിരികെ 0L;
    തിരികെ (long)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *താക്കോൽ, പൂർണ്ണസംഖ്യ len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = ശൂന്യ;
    hashitem_T	*hi;

    എങ്കിൽ (d == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (len < 0)
	akey = താക്കോൽ;
    അല്ലെങ്കിൽ എങ്കിൽ (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(താക്കോൽ, len);
	എങ്കിൽ (akey == ശൂന്യ)
	    തിരികെ ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, താക്കോൽ, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    എങ്കിൽ (HASHITEM_EMPTY(hi))
	തിരികെ ശൂന്യ;
    തിരികെ HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    പൂർണ്ണസംഖ്യ
dict_has_താക്കോൽ(dict_T *d, അക്ഷരം *താക്കോൽ)
{
    തിരികെ dict_find(d, (char_u *)താക്കോൽ, -1) != ശൂന്യ;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    പൂർണ്ണസംഖ്യ
dict_get_tv(dict_T *d, അക്ഷരം *താക്കോൽ, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)താക്കോൽ, -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ FAIL;
    copy_tv(&di->di_tv, rettv);
    തിരികെ OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, അക്ഷരം *താക്കോൽ, പൂർണ്ണസംഖ്യ save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)താക്കോൽ, -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ ശൂന്യ;
    s = tv_get_string(&di->di_tv);
    എങ്കിൽ (save && s != ശൂന്യ)
	s = vim_strsave(s);
    തിരികെ s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_സംഖ്യ(dict_T *d, അക്ഷരം *താക്കോൽ)
{
    തിരികെ dict_get_സംഖ്യ_def(d, താക്കോൽ, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_സംഖ്യ_def(dict_T *d, അക്ഷരം *താക്കോൽ, പൂർണ്ണസംഖ്യ def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)താക്കോൽ, -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ def;
    തിരികെ tv_get_സംഖ്യ(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_സംഖ്യ_check(dict_T *d, char_u *താക്കോൽ)
{
    dictitem_T	*di;

    di = dict_find(d, താക്കോൽ, -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ 0;
    എങ്കിൽ (di->di_tv.v_type != VAR_സംഖ്യ)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	തിരികെ 0;
    }
    തിരികെ tv_get_സംഖ്യ(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, അക്ഷരം *താക്കോൽ, പൂർണ്ണസംഖ്യ def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)താക്കോൽ, -1);
    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ def;
    തിരികെ tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, പൂർണ്ണസംഖ്യ copyID, പൂർണ്ണസംഖ്യ restore_copyID)
{
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    പൂർണ്ണസംഖ്യ		todo;
    പൂർണ്ണസംഖ്യ		prev_lock;

    എങ്കിൽ ((d = tv->vval.v_dict) == ശൂന്യ)
	തിരികെ ശൂന്യ;
    ga_init2(&ga, വലുപ്പം(അക്ഷരം), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    എങ്കിൽ (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (പൂർണ്ണസംഖ്യ)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&d->dv_hashtab, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    എങ്കിൽ (first)
		first = FALSE;
	    അല്ലെങ്കിൽ
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_താക്കോൽ, FALSE);
	    എങ്കിൽ (tofree != ശൂന്യ)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    എങ്കിൽ (s != ശൂന്യ)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    എങ്കിൽ (s == ശൂന്യ || did_echo_string_emsg)
		നിർത്തുക;
	    വരി_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    എങ്കിൽ (todo > 0)
    {
	vim_free(ga.ga_data);
	തിരികെ ശൂന്യ;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    തിരികെ (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    സ്ഥിതം char_u *
skip_literal_താക്കോൽ(char_u *താക്കോൽ)
{
    char_u *p;

    വേണ്ടി (p = താക്കോൽ; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    തിരികെ p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_literal_താക്കോൽ_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_താക്കോൽ(*arg);

    എങ്കിൽ (p == *arg)
	തിരികെ FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    തിരികെ OK;
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
get_literal_താക്കോൽ(char_u **arg)
{
    char_u	*താക്കോൽ;
    char_u	*end;
    typval_T	rettv;

    എങ്കിൽ (**arg == '\'')
    {
	എങ്കിൽ (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    തിരികെ ശൂന്യ;
	താക്കോൽ = rettv.vval.v_string;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (**arg == '"')
    {
	എങ്കിൽ (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    തിരികെ ശൂന്യ;
	താക്കോൽ = rettv.vval.v_string;
    }
    അല്ലെങ്കിൽ
    {
	end = skip_literal_താക്കോൽ(*arg);
	എങ്കിൽ (end == *arg)
	{
	    semsg(_(e_invalid_താക്കോൽ_str), *arg);
	    തിരികെ ശൂന്യ;
	}
	താക്കോൽ = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    തിരികെ താക്കോൽ;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    പൂർണ്ണസംഖ്യ
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, പൂർണ്ണസംഖ്യ literal)
{
    പൂർണ്ണസംഖ്യ		evaluate = evalarg == ശൂന്യ ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = ശൂന്യ;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*താക്കോൽ = ശൂന്യ;
    dictitem_T	*item;
    char_u	*curly_വ്യഞ്ജകം = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    പൂർണ്ണസംഖ്യ		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    എങ്കിൽ (!vim9script
	    && *curly_വ്യഞ്ജകം != '}'
	    && !literal
	    && eval1(&curly_വ്യഞ്ജകം, &tv, ശൂന്യ) == OK
	    && *skipwhite(curly_വ്യഞ്ജകം) == '}')
	തിരികെ NOTDONE;

    എങ്കിൽ (evaluate)
    {
	d = dict_alloc();
	എങ്കിൽ (d == ശൂന്യ)
	    തിരികെ FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ആകുമ്പോൾ (**arg != '}' && **arg != NUL)
    {
	പൂർണ്ണസംഖ്യ	has_bracket = vim9script && **arg == '[';

	എങ്കിൽ (literal)
	{
	    എങ്കിൽ (get_literal_താക്കോൽ_tv(arg, &tvkey) == FAIL)
		പോകുക failret;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_താക്കോൽ(arg);
	    എങ്കിൽ (tvkey.vval.v_string == ശൂന്യ)
		പോകുക failret;
	    tvkey.v_type = VAR_STRING;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (has_bracket)
		*arg = skipwhite(*arg + 1);
	    എങ്കിൽ (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		പോകുക failret;
	    എങ്കിൽ (has_bracket)
	    {
		*arg = skipwhite(*arg);
		എങ്കിൽ (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_താക്കോൽ));
		    clear_tv(&tvkey);
		    പോകുക failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	എങ്കിൽ (!vim9script)
	    *arg = skipwhite(*arg);
	എങ്കിൽ (**arg != ':')
	{
	    എങ്കിൽ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    അല്ലെങ്കിൽ
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    പോകുക failret;
	}
	എങ്കിൽ (evaluate)
	{
	    എങ്കിൽ (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    താക്കോൽ = tv_get_string_buf_chk(&tvkey, buf);
	    എങ്കിൽ (താക്കോൽ == ശൂന്യ)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		പോകുക failret;
	    }
	}
	എങ്കിൽ (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    പോകുക failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	എങ്കിൽ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    പോകുക failret;
	}
	എങ്കിൽ (check_typval_is_മൂല്യം(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    പോകുക failret;
	}
	എങ്കിൽ (evaluate)
	{
	    item = dict_find(d, താക്കോൽ, -1);
	    എങ്കിൽ (item != ശൂന്യ)
	    {
		semsg(_(e_duplicate_താക്കോൽ_in_dictionary_str), താക്കോൽ);
		clear_tv(&tvkey);
		clear_tv(&tv);
		പോകുക failret;
	    }
	    item = dictitem_alloc(താക്കോൽ);
	    എങ്കിൽ (item != ശൂന്യ)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		എങ്കിൽ (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	എങ്കിൽ (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	എങ്കിൽ (had_comma)
	{
	    എങ്കിൽ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		പോകുക failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	എങ്കിൽ (**arg == '}')
	    നിർത്തുക;
	എങ്കിൽ (!had_comma)
	{
	    എങ്കിൽ (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    അല്ലെങ്കിൽ
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    പോകുക failret;
	}
    }

    എങ്കിൽ (**arg != '}')
    {
	എങ്കിൽ (evalarg != ശൂന്യ)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	എങ്കിൽ (d != ശൂന്യ)
	    dict_free(d);
	തിരികെ FAIL;
    }

    *arg = *arg + 1;
    എങ്കിൽ (evaluate)
	rettv_dict_set(rettv, d);

    തിരികെ OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    പൂർണ്ണസംഖ്യ
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    പൂർണ്ണസംഖ്യ		ret = OK;

    എങ്കിൽ (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    അല്ലെങ്കിൽ
	ret = NOTDONE;

    തിരികെ ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    ശൂന്യം
dict_extend(dict_T *d1, dict_T *d2, char_u *action, അക്ഷരം *func_name)
{
    dictitem_T	*di1;
    പൂർണ്ണസംഖ്യ		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    എങ്കിൽ (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	തിരികെ;

    എങ്കിൽ (*action == 'm')
    {
	എങ്കിൽ (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    തിരികെ;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    എങ്കിൽ (d1->dv_type != ശൂന്യ && d1->dv_type->tt_member != ശൂന്യ)
	type = d1->dv_type->tt_member;
    അല്ലെങ്കിൽ
	type = ശൂന്യ;

    todo = (പൂർണ്ണസംഖ്യ)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&d2->dv_hashtab, hi2, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_താക്കോൽ, -1);
	    // Check the key to be valid when adding to any scope.
	    എങ്കിൽ (d1->dv_scope != 0 && !valid_varname(hi2->hi_താക്കോൽ, -1, TRUE))
		നിർത്തുക;

	    എങ്കിൽ (type != ശൂന്യ
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		നിർത്തുക;

	    എങ്കിൽ (di1 == ശൂന്യ)
	    {
		എങ്കിൽ (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    എങ്കിൽ (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		അല്ലെങ്കിൽ
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    എങ്കിൽ (di1 != ശൂന്യ && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*action == 'e')
	    {
		semsg(_(e_താക്കോൽ_already_exists_str), hi2->hi_താക്കോൽ);
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (*action == 'f' && HI2DI(hi2) != di1)
	    {
		എങ്കിൽ (മൂല്യം_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    നിർത്തുക;
		// Disallow replacing a builtin function.
		എങ്കിൽ (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_താക്കോൽ))
		    നിർത്തുക;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    എങ്കിൽ (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    തിരികെ HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    പൂർണ്ണസംഖ്യ
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    പൂർണ്ണസംഖ്യ		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    പൂർണ്ണസംഖ്യ		todo;

    എങ്കിൽ (d1 == d2)
	തിരികെ TRUE;
    എങ്കിൽ (dict_len(d1) != dict_len(d2))
	തിരികെ FALSE;
    എങ്കിൽ (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	തിരികെ TRUE;
    എങ്കിൽ (d1 == ശൂന്യ || d2 == ശൂന്യ)
	തിരികെ FALSE;

    todo = (പൂർണ്ണസംഖ്യ)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&d1->dv_hashtab, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_താക്കോൽ, -1);
	    എങ്കിൽ (item2 == ശൂന്യ)
		തിരികെ FALSE;
	    എങ്കിൽ (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		തിരികെ FALSE;
	    --todo;
	}
    }
    തിരികെ TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    long
dict_എണ്ണം(dict_T *d, typval_T *needle, പൂർണ്ണസംഖ്യ ic)
{
    പൂർണ്ണസംഖ്യ		todo;
    hashitem_T	*hi;
    long	n = 0;

    എങ്കിൽ (d == ശൂന്യ)
	തിരികെ 0;

    todo = (പൂർണ്ണസംഖ്യ)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&d->dv_hashtab, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    എങ്കിൽ (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    തിരികെ n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    ശൂന്യം
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	അക്ഷരം		*func_name,
	char_u		*arg_errmsg,
	പൂർണ്ണസംഖ്യ		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    പൂർണ്ണസംഖ്യ	i;

    d1 = argvars[0].vval.v_dict;
    എങ്കിൽ (d1 == ശൂന്യ)
    {
	emsg(_(e_cannot_extend_null_dict));
	തിരികെ;
    }

    എങ്കിൽ (!is_new && മൂല്യം_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	തിരികെ;

    എങ്കിൽ (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	എങ്കിൽ (d1 == ശൂന്യ)
	    തിരികെ;
    }

    d2 = argvars[1].vval.v_dict;
    എങ്കിൽ (d2 == ശൂന്യ)
	പോകുക theend;

    // Check the third argument.
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	സ്ഥിതം അക്ഷരം *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	എങ്കിൽ (action == ശൂന്യ)
	{
	    എങ്കിൽ (is_new)
		dict_unref(d1);
	    തിരികെ;
	}
	വേണ്ടി (i = 0; i < 3; ++i)
	    എങ്കിൽ (STRCMP(action, av[i]) == 0)
		നിർത്തുക;
	എങ്കിൽ (i == 3)
	{
	    എങ്കിൽ (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ
	action = (char_u *)"force";

    എങ്കിൽ (type != ശൂന്യ && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	എങ്കിൽ (is_new)
	    dict_unref(d1);
	തിരികെ;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    എങ്കിൽ (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    അല്ലെങ്കിൽ
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    ശൂന്യം
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	അക്ഷരം		*func_name,
	char_u		*arg_errmsg,
	typval_T	*വ്യഞ്ജകം,
	typval_T	*rettv)
{
    dict_T	*d_ret = ശൂന്യ;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    പൂർണ്ണസംഖ്യ		todo;
    പൂർണ്ണസംഖ്യ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    എങ്കിൽ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = ശൂന്യ;
    }
    എങ്കിൽ (d == ശൂന്യ
	  || (filtermap == FILTERMAP_FILTER
			&& മൂല്യം_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	തിരികെ;

    എങ്കിൽ (filtermap == FILTERMAP_MAPNEW)
    {
	എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	    തിരികെ;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_വ്യഞ്ജകം_get_funccal(വ്യഞ്ജകം, &newtv);

    പൂർണ്ണസംഖ്യ prev_lock = d->dv_lock;
    എങ്കിൽ (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (പൂർണ്ണസംഖ്യ)ht->ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(ht, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    പൂർണ്ണസംഖ്യ		r;

	    --todo;
	    di = HI2DI(hi);
	    എങ്കിൽ (filtermap == FILTERMAP_MAP
		    && (മൂല്യം_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		നിർത്തുക;
	    set_vim_var_string(VV_താക്കോൽ, di->di_താക്കോൽ, -1);
	    r = filter_map_one(&di->di_tv, വ്യഞ്ജകം, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_താക്കോൽ));
	    എങ്കിൽ (r == FAIL || did_emsg)
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
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (അക്ഷരം *)di->di_താക്കോൽ, &newtv);
		clear_tv(&newtv);
		എങ്കിൽ (r == FAIL)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		എങ്കിൽ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    നിർത്തുക;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    എങ്കിൽ (fc != ശൂന്യ)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    ശൂന്യം
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*താക്കോൽ;
    dictitem_T	*di;

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	തിരികെ;
    }

    d = argvars[0].vval.v_dict;
    എങ്കിൽ (d == ശൂന്യ || മൂല്യം_check_lock(d->dv_lock, arg_errmsg, TRUE))
	തിരികെ;

    താക്കോൽ = tv_get_string_chk(&argvars[1]);
    എങ്കിൽ (താക്കോൽ == ശൂന്യ)
	തിരികെ;

    di = dict_find(d, താക്കോൽ, -1);
    എങ്കിൽ (di == ശൂന്യ)
    {
	semsg(_(e_താക്കോൽ_not_present_in_dictionary_str), താക്കോൽ);
	തിരികെ;
    }

    എങ്കിൽ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	തിരികെ;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

തരനിർവചനം എണ്ണൽ {
    DICT2_LIST_താക്കോലുകൾ,
    DICT2_LIST_മൂല്യങ്ങൾ,
    DICT2_LIST_ഇനങ്ങൾ,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    സ്ഥിതം ശൂന്യം
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    പൂർണ്ണസംഖ്യ		todo;

    എങ്കിൽ (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(ശൂന്യം)rettv_list_alloc(rettv);
	തിരികെ;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == ശൂന്യ ? 0 : (പൂർണ്ണസംഖ്യ)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_ഇനങ്ങൾ(todo);
    എങ്കിൽ (l == ശൂന്യ)
    {
	(ശൂന്യം)rettv_list_alloc(rettv);
	തിരികെ;
    }
    rettv_list_set(rettv, l);
    എങ്കിൽ (todo == 0)
	// NULL or empty dict, return the empty list
	തിരികെ;

    li = l->lv_first;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&d->dv_hashtab, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    എങ്കിൽ (what == DICT2_LIST_താക്കോലുകൾ)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_താക്കോൽ);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (what == DICT2_LIST_മൂല്യങ്ങൾ)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    അല്ലെങ്കിൽ
	    {
		// items()
		l2 = list_alloc_with_ഇനങ്ങൾ(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		എങ്കിൽ (l2 == ശൂന്യ)
		    പോകുക alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_താക്കോൽ);
		എങ്കിൽ (l2->lv_first->li_tv.vval.v_string == ശൂന്യ)
		    പോകുക alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    തിരികെ;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (ശൂന്യം)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    ശൂന്യം
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ഇനങ്ങൾ);
}

/*
 * "keys()" function
 */
    ശൂന്യം
f_താക്കോലുകൾ(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_താക്കോലുകൾ);
}

/*
 * "values(dict)" function
 */
    ശൂന്യം
f_മൂല്യങ്ങൾ(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_മൂല്യങ്ങൾ);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    ശൂന്യം
dict_set_ഇനങ്ങൾ_ro(dict_T *di)
{
    പൂർണ്ണസംഖ്യ		todo = (പൂർണ്ണസംഖ്യ)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&di->dv_hashtab, hi, todo)
    {
	എങ്കിൽ (HASHITEM_EMPTY(hi))
	    തുടരുക;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    ശൂന്യം
f_has_താക്കോൽ(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    എങ്കിൽ (check_for_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].vval.v_dict == ശൂന്യ)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = dict_has_താക്കോൽ(argvars[0].vval.v_dict,
				(അക്ഷരം *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
