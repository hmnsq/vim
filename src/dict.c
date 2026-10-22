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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
நிலையான dict_T		*first_dict = வெற்று;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(வெறுமை)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    எனில் (d == வெற்று)
	திரும்பு வெற்று;

    // Add the dict to the list of dicts for garbage collection.
    எனில் (first_dict != வெற்று)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = வெற்று;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    திரும்பு d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    எனில் (alloc_fail_id == id && alloc_does_fail(அளவு(list_T)))
	திரும்பு வெற்று;
#endif
    திரும்பு (dict_alloc());
}

    dict_T *
dict_alloc_lock(முழு lock)
{
    dict_T *d = dict_alloc();

    எனில் (d != வெற்று)
	d->dv_lock = lock;
    திரும்பு d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    முழு
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    எனில் (d == வெற்று)
	திரும்பு FAIL;

    rettv_dict_set(rettv, d);
    திரும்பு OK;
}

/*
 * Set a dictionary as the return value
 */
    வெறுமை
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    எனில் (d != வெற்று)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    வெறுமை
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = வெற்று;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    வெறுமை
hashtab_free_contents(hashtab_T *ht)
{
    முழு		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    எனில் (check_hashtab_frozen(ht, "clear dict"))
	திரும்பு;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (முழு)ht->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
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

    நிலையான வெறுமை
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    எனில் (d->dv_used_prev == வெற்று)
	first_dict = d->dv_used_next;
    இல்லையெனில்
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    எனில் (d->dv_used_next != வெற்று)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    நிலையான வெறுமை
dict_free(dict_T *d)
{
    எனில் (!in_free_unref_பொருட்கள்)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    வெறுமை
dict_unref(dict_T *d)
{
    எனில் (d != வெற்று && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    முழு
dict_free_nonref(முழு copyID)
{
    dict_T	*dd;
    முழு		did_free = FALSE;

    க்கு (dd = first_dict; dd != வெற்று; dd = dd->dv_used_next)
	எனில் ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    திரும்பு did_free;
}

    வெறுமை
dict_free_பொருட்கள்(முழு copyID)
{
    dict_T	*dd, *dd_next;

    க்கு (dd = first_dict; dd != வெற்று; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	எனில் ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
dictitem_alloc(char_u *சாவி)
{
    dictitem_T *di;
    size_t len = STRLEN(சாவி);

    di = alloc(offsetof(dictitem_T, di_சாவி) + len + 1);
    எனில் (di == வெற்று)
	திரும்பு வெற்று;

    mch_memmove(di->di_சாவி, சாவி, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    திரும்பு di;
}

/*
 * Make a copy of a Dictionary item.
 */
    நிலையான dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_சாவி);

    di = alloc(offsetof(dictitem_T, di_சாவி) + len + 1);
    எனில் (di == வெற்று)
	திரும்பு வெற்று;

    mch_memmove(di->di_சாவி, org->di_சாவி, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    திரும்பு di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    வெறுமை
dictitem_remove(dict_T *dict, dictitem_T *item, எழுத்து *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_சாவி);
    எனில் (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    இல்லையெனில்
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(item);
}

/*
 * Free a dict item.  Also clears the value.
 */
    வெறுமை
dictitem_free(dictitem_T *item)
{
    clear_tv(&item->di_tv);
    எனில் (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, முழு deep, முழு உச்சி, முழு copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    முழு		todo;
    hashitem_T	*hi;

    எனில் (orig == வெற்று)
	திரும்பு வெற்று;

    copy = dict_alloc();
    எனில் (copy == வெற்று)
	திரும்பு வெற்று;

    எனில் (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    எனில் (orig->dv_type == வெற்று || உச்சி || deep)
	copy->dv_type = வெற்று;
    இல்லையெனில்
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (முழு)orig->dv_hashtab.ht_used;
    க்கு (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_சாவி);
	    எனில் (di == வெற்று)
		நிறுத்து;
	    எனில் (deep)
	    {
		எனில் (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    நிறுத்து;
		}
	    }
	    இல்லையெனில்
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    எனில் (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		நிறுத்து;
	    }
	}
    }

    ++copy->dv_refcount;
    எனில் (todo > 0)
    {
	dict_unref(copy);
	copy = வெற்று;
    }

    திரும்பு copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    முழு
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    திரும்பு (d == get_globvar_dict()
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
    முழு
dict_add(dict_T *d, dictitem_T *item)
{
    எனில் (dict_wrong_func_name(d, &item->di_tv, item->di_சாவி))
	திரும்பு FAIL;
    திரும்பு hash_add(&d->dv_hashtab, item->di_சாவி, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    நிலையான முழு
dict_add_எண்_special(
    dict_T	*d,
    எழுத்து	*சாவி,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)சாவி);
    எனில் (item == வெற்று)
	திரும்பு FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_எண் = nr;
    எனில் (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    முழு
dict_add_எண்(dict_T *d, எழுத்து *சாவி, varnumber_T nr)
{
    திரும்பு dict_add_எண்_special(d, சாவி, nr, VAR_எண்);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    முழு
dict_add_bool(dict_T *d, எழுத்து *சாவி, varnumber_T nr)
{
    திரும்பு dict_add_எண்_special(d, சாவி, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    முழு
dict_add_string(dict_T *d, எழுத்து *சாவி, char_u *str)
{
    திரும்பு dict_add_string_len(d, சாவி, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    முழு
dict_add_string_len(dict_T *d, எழுத்து *சாவி, char_u *str, முழு len)
{
    dictitem_T	*item;
    char_u	*val = வெற்று;

    item = dictitem_alloc((char_u *)சாவி);
    எனில் (item == வெற்று)
	திரும்பு FAIL;
    item->di_tv.v_type = VAR_STRING;
    எனில் (str != வெற்று)
    {
	எனில் (len == -1)
	    val = vim_strsave(str);
	இல்லையெனில்
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    எனில் (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    முழு
dict_add_list(dict_T *d, எழுத்து *சாவி, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)சாவி);
    எனில் (item == வெற்று)
	திரும்பு FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    எனில் (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = வெற்று;
	dictitem_free(item);
	திரும்பு FAIL;
    }
    ++list->lv_refcount;
    திரும்பு OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    முழு
dict_add_tv(dict_T *d, எழுத்து *சாவி, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)சாவி);
    எனில் (item == வெற்று)
	திரும்பு FAIL;
    copy_tv(tv, &item->di_tv);
    எனில் (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    முழு
dict_add_callback(dict_T *d, எழுத்து *சாவி, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)சாவி);
    எனில் (item == வெற்று)
	திரும்பு FAIL;
    put_callback(cb, &item->di_tv);
    எனில் (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    முழு
dict_add_func(dict_T *d, எழுத்து *சாவி, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)சாவி);
    எனில் (item == வெற்று)
	திரும்பு FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    எனில் (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    வெறுமை
dict_iterate_start(typval_T *var, dict_iterator_T *iter)
{
    எனில் (var->v_type != VAR_DICT || var->vval.v_dict == வெற்று)
	iter->dit_todo = 0;
    இல்லையெனில்
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

    எனில் (iter->dit_todo == 0)
	திரும்பு வெற்று;

    போது (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_சாவி;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    திரும்பு result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    முழு
dict_add_dict(dict_T *d, எழுத்து *சாவி, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)சாவி);
    எனில் (item == வெற்று)
	திரும்பு FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    எனில் (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = வெற்று;
	dictitem_free(item);
	திரும்பு FAIL;
    }
    ++dict->dv_refcount;
    திரும்பு OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    எனில் (d == வெற்று)
	திரும்பு 0L;
    திரும்பு (long)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *சாவி, முழு len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = வெற்று;
    hashitem_T	*hi;

    எனில் (d == வெற்று)
	திரும்பு வெற்று;
    எனில் (len < 0)
	akey = சாவி;
    இல்லையெனில் எனில் (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(சாவி, len);
	எனில் (akey == வெற்று)
	    திரும்பு வெற்று;
    }
    இல்லையெனில்
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, சாவி, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    எனில் (HASHITEM_EMPTY(hi))
	திரும்பு வெற்று;
    திரும்பு HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    முழு
dict_has_சாவி(dict_T *d, எழுத்து *சாவி)
{
    திரும்பு dict_find(d, (char_u *)சாவி, -1) != வெற்று;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    முழு
dict_get_tv(dict_T *d, எழுத்து *சாவி, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)சாவி, -1);
    எனில் (di == வெற்று)
	திரும்பு FAIL;
    copy_tv(&di->di_tv, rettv);
    திரும்பு OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, எழுத்து *சாவி, முழு save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)சாவி, -1);
    எனில் (di == வெற்று)
	திரும்பு வெற்று;
    s = tv_get_string(&di->di_tv);
    எனில் (save && s != வெற்று)
	s = vim_strsave(s);
    திரும்பு s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_எண்(dict_T *d, எழுத்து *சாவி)
{
    திரும்பு dict_get_எண்_def(d, சாவி, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_எண்_def(dict_T *d, எழுத்து *சாவி, முழு def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)சாவி, -1);
    எனில் (di == வெற்று)
	திரும்பு def;
    திரும்பு tv_get_எண்(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_எண்_check(dict_T *d, char_u *சாவி)
{
    dictitem_T	*di;

    di = dict_find(d, சாவி, -1);
    எனில் (di == வெற்று)
	திரும்பு 0;
    எனில் (di->di_tv.v_type != VAR_எண்)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	திரும்பு 0;
    }
    திரும்பு tv_get_எண்(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, எழுத்து *சாவி, முழு def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)சாவி, -1);
    எனில் (di == வெற்று)
	திரும்பு def;
    திரும்பு tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, முழு copyID, முழு restore_copyID)
{
    garray_T	ga;
    முழு		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    முழு		todo;
    முழு		prev_lock;

    எனில் ((d = tv->vval.v_dict) == வெற்று)
	திரும்பு வெற்று;
    ga_init2(&ga, அளவு(எழுத்து), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    எனில் (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (முழு)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(&d->dv_hashtab, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    எனில் (first)
		first = FALSE;
	    இல்லையெனில்
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_சாவி, FALSE);
	    எனில் (tofree != வெற்று)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    எனில் (s != வெற்று)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    எனில் (s == வெற்று || did_echo_string_emsg)
		நிறுத்து;
	    வரி_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    எனில் (todo > 0)
    {
	vim_free(ga.ga_data);
	திரும்பு வெற்று;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    திரும்பு (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    நிலையான char_u *
skip_literal_சாவி(char_u *சாவி)
{
    char_u *p;

    க்கு (p = சாவி; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    திரும்பு p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    நிலையான முழு
get_literal_சாவி_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_சாவி(*arg);

    எனில் (p == *arg)
	திரும்பு FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    திரும்பு OK;
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
get_literal_சாவி(char_u **arg)
{
    char_u	*சாவி;
    char_u	*end;
    typval_T	rettv;

    எனில் (**arg == '\'')
    {
	எனில் (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    திரும்பு வெற்று;
	சாவி = rettv.vval.v_string;
    }
    இல்லையெனில் எனில் (**arg == '"')
    {
	எனில் (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    திரும்பு வெற்று;
	சாவி = rettv.vval.v_string;
    }
    இல்லையெனில்
    {
	end = skip_literal_சாவி(*arg);
	எனில் (end == *arg)
	{
	    semsg(_(e_invalid_சாவி_str), *arg);
	    திரும்பு வெற்று;
	}
	சாவி = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    திரும்பு சாவி;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    முழு
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, முழு literal)
{
    முழு		evaluate = evalarg == வெற்று ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = வெற்று;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*சாவி = வெற்று;
    dictitem_T	*item;
    char_u	*curly_கோவை = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    முழு		vim9script = in_vim9script();
    முழு		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    எனில் (!vim9script
	    && *curly_கோவை != '}'
	    && !literal
	    && eval1(&curly_கோவை, &tv, வெற்று) == OK
	    && *skipwhite(curly_கோவை) == '}')
	திரும்பு NOTDONE;

    எனில் (evaluate)
    {
	d = dict_alloc();
	எனில் (d == வெற்று)
	    திரும்பு FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    போது (**arg != '}' && **arg != NUL)
    {
	முழு	has_bracket = vim9script && **arg == '[';

	எனில் (literal)
	{
	    எனில் (get_literal_சாவி_tv(arg, &tvkey) == FAIL)
		செல் failret;
	}
	இல்லையெனில் எனில் (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_சாவி(arg);
	    எனில் (tvkey.vval.v_string == வெற்று)
		செல் failret;
	    tvkey.v_type = VAR_STRING;
	}
	இல்லையெனில்
	{
	    எனில் (has_bracket)
		*arg = skipwhite(*arg + 1);
	    எனில் (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		செல் failret;
	    எனில் (has_bracket)
	    {
		*arg = skipwhite(*arg);
		எனில் (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_சாவி));
		    clear_tv(&tvkey);
		    செல் failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	எனில் (!vim9script)
	    *arg = skipwhite(*arg);
	எனில் (**arg != ':')
	{
	    எனில் (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    இல்லையெனில்
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    செல் failret;
	}
	எனில் (evaluate)
	{
	    எனில் (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    சாவி = tv_get_string_buf_chk(&tvkey, buf);
	    எனில் (சாவி == வெற்று)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		செல் failret;
	    }
	}
	எனில் (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    செல் failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	எனில் (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    செல் failret;
	}
	எனில் (check_typval_is_மதிப்பு(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    செல் failret;
	}
	எனில் (evaluate)
	{
	    item = dict_find(d, சாவி, -1);
	    எனில் (item != வெற்று)
	    {
		semsg(_(e_duplicate_சாவி_in_dictionary_str), சாவி);
		clear_tv(&tvkey);
		clear_tv(&tv);
		செல் failret;
	    }
	    item = dictitem_alloc(சாவி);
	    எனில் (item != வெற்று)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		எனில் (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	எனில் (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	எனில் (had_comma)
	{
	    எனில் (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		செல் failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	எனில் (**arg == '}')
	    நிறுத்து;
	எனில் (!had_comma)
	{
	    எனில் (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    இல்லையெனில்
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    செல் failret;
	}
    }

    எனில் (**arg != '}')
    {
	எனில் (evalarg != வெற்று)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	எனில் (d != வெற்று)
	    dict_free(d);
	திரும்பு FAIL;
    }

    *arg = *arg + 1;
    எனில் (evaluate)
	rettv_dict_set(rettv, d);

    திரும்பு OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    முழு
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    முழு		vim9script = in_vim9script();
    முழு		ret = OK;

    எனில் (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    இல்லையெனில் எனில் ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    இல்லையெனில்
	ret = NOTDONE;

    திரும்பு ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    வெறுமை
dict_extend(dict_T *d1, dict_T *d2, char_u *action, எழுத்து *func_name)
{
    dictitem_T	*di1;
    முழு		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    எனில் (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	திரும்பு;

    எனில் (*action == 'm')
    {
	எனில் (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    திரும்பு;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    எனில் (d1->dv_type != வெற்று && d1->dv_type->tt_member != வெற்று)
	type = d1->dv_type->tt_member;
    இல்லையெனில்
	type = வெற்று;

    todo = (முழு)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_பொருட்கள்(&d2->dv_hashtab, hi2, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_சாவி, -1);
	    // Check the key to be valid when adding to any scope.
	    எனில் (d1->dv_scope != 0 && !valid_varname(hi2->hi_சாவி, -1, TRUE))
		நிறுத்து;

	    எனில் (type != வெற்று
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		நிறுத்து;

	    எனில் (di1 == வெற்று)
	    {
		எனில் (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    எனில் (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		இல்லையெனில்
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    எனில் (di1 != வெற்று && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    இல்லையெனில் எனில் (*action == 'e')
	    {
		semsg(_(e_சாவி_already_exists_str), hi2->hi_சாவி);
		நிறுத்து;
	    }
	    இல்லையெனில் எனில் (*action == 'f' && HI2DI(hi2) != di1)
	    {
		எனில் (மதிப்பு_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    நிறுத்து;
		// Disallow replacing a builtin function.
		எனில் (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_சாவி))
		    நிறுத்து;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    எனில் (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    திரும்பு HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    முழு
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    முழு		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    முழு		todo;

    எனில் (d1 == d2)
	திரும்பு TRUE;
    எனில் (dict_len(d1) != dict_len(d2))
	திரும்பு FALSE;
    எனில் (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	திரும்பு TRUE;
    எனில் (d1 == வெற்று || d2 == வெற்று)
	திரும்பு FALSE;

    todo = (முழு)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(&d1->dv_hashtab, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_சாவி, -1);
	    எனில் (item2 == வெற்று)
		திரும்பு FALSE;
	    எனில் (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		திரும்பு FALSE;
	    --todo;
	}
    }
    திரும்பு TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    long
dict_மொத்தம்(dict_T *d, typval_T *needle, முழு ic)
{
    முழு		todo;
    hashitem_T	*hi;
    long	n = 0;

    எனில் (d == வெற்று)
	திரும்பு 0;

    todo = (முழு)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(&d->dv_hashtab, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    எனில் (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    திரும்பு n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    வெறுமை
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	எழுத்து		*func_name,
	char_u		*arg_errmsg,
	முழு		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    முழு	i;

    d1 = argvars[0].vval.v_dict;
    எனில் (d1 == வெற்று)
    {
	emsg(_(e_cannot_extend_null_dict));
	திரும்பு;
    }

    எனில் (!is_new && மதிப்பு_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	திரும்பு;

    எனில் (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	எனில் (d1 == வெற்று)
	    திரும்பு;
    }

    d2 = argvars[1].vval.v_dict;
    எனில் (d2 == வெற்று)
	செல் theend;

    // Check the third argument.
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	நிலையான எழுத்து *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	எனில் (action == வெற்று)
	{
	    எனில் (is_new)
		dict_unref(d1);
	    திரும்பு;
	}
	க்கு (i = 0; i < 3; ++i)
	    எனில் (STRCMP(action, av[i]) == 0)
		நிறுத்து;
	எனில் (i == 3)
	{
	    எனில் (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    திரும்பு;
	}
    }
    இல்லையெனில்
	action = (char_u *)"force";

    எனில் (type != வெற்று && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	எனில் (is_new)
	    dict_unref(d1);
	திரும்பு;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    எனில் (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    இல்லையெனில்
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    வெறுமை
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	எழுத்து		*func_name,
	char_u		*arg_errmsg,
	typval_T	*கோவை,
	typval_T	*rettv)
{
    dict_T	*d_ret = வெற்று;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    முழு		todo;
    முழு		rem;
    typval_T	newtv;
    funccall_T	*fc;

    எனில் (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = வெற்று;
    }
    எனில் (d == வெற்று
	  || (filtermap == FILTERMAP_FILTER
			&& மதிப்பு_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	திரும்பு;

    எனில் (filtermap == FILTERMAP_MAPNEW)
    {
	எனில் (rettv_dict_alloc(rettv) == FAIL)
	    திரும்பு;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_கோவை_get_funccal(கோவை, &newtv);

    முழு prev_lock = d->dv_lock;
    எனில் (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (முழு)ht->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    முழு		r;

	    --todo;
	    di = HI2DI(hi);
	    எனில் (filtermap == FILTERMAP_MAP
		    && (மதிப்பு_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		நிறுத்து;
	    set_vim_var_string(VV_சாவி, di->di_சாவி, -1);
	    r = filter_map_one(&di->di_tv, கோவை, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_சாவி));
	    எனில் (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		நிறுத்து;
	    }
	    எனில் (filtermap == FILTERMAP_MAP)
	    {
		எனில் (argtype != வெற்று && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    நிறுத்து;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    இல்லையெனில் எனில் (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (எழுத்து *)di->di_சாவி, &newtv);
		clear_tv(&newtv);
		எனில் (r == FAIL)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		எனில் (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    நிறுத்து;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    எனில் (fc != வெற்று)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    வெறுமை
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*சாவி;
    dictitem_T	*di;

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	திரும்பு;
    }

    d = argvars[0].vval.v_dict;
    எனில் (d == வெற்று || மதிப்பு_check_lock(d->dv_lock, arg_errmsg, TRUE))
	திரும்பு;

    சாவி = tv_get_string_chk(&argvars[1]);
    எனில் (சாவி == வெற்று)
	திரும்பு;

    di = dict_find(d, சாவி, -1);
    எனில் (di == வெற்று)
    {
	semsg(_(e_சாவி_not_present_in_dictionary_str), சாவி);
	திரும்பு;
    }

    எனில் (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	திரும்பு;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

வகைவரை எண்ணிக்கை {
    DICT2_LIST_சாவிகள்,
    DICT2_LIST_மதிப்புகள்,
    DICT2_LIST_பொருட்கள்,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    நிலையான வெறுமை
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    முழு		todo;

    எனில் (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(வெறுமை)rettv_list_alloc(rettv);
	திரும்பு;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == வெற்று ? 0 : (முழு)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_பொருட்கள்(todo);
    எனில் (l == வெற்று)
    {
	(வெறுமை)rettv_list_alloc(rettv);
	திரும்பு;
    }
    rettv_list_set(rettv, l);
    எனில் (todo == 0)
	// NULL or empty dict, return the empty list
	திரும்பு;

    li = l->lv_first;
    FOR_ALL_HASHTAB_பொருட்கள்(&d->dv_hashtab, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    எனில் (what == DICT2_LIST_சாவிகள்)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_சாவி);
	    }
	    இல்லையெனில் எனில் (what == DICT2_LIST_மதிப்புகள்)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    இல்லையெனில்
	    {
		// items()
		l2 = list_alloc_with_பொருட்கள்(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		எனில் (l2 == வெற்று)
		    செல் alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_சாவி);
		எனில் (l2->lv_first->li_tv.vval.v_string == வெற்று)
		    செல் alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    திரும்பு;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (வெறுமை)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    வெறுமை
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_பொருட்கள்);
}

/*
 * "keys()" function
 */
    வெறுமை
f_சாவிகள்(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_சாவிகள்);
}

/*
 * "values(dict)" function
 */
    வெறுமை
f_மதிப்புகள்(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_மதிப்புகள்);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    வெறுமை
dict_set_பொருட்கள்_ro(dict_T *di)
{
    முழு		todo = (முழு)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_பொருட்கள்(&di->dv_hashtab, hi, todo)
    {
	எனில் (HASHITEM_EMPTY(hi))
	    தொடர்;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    வெறுமை
f_has_சாவி(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    எனில் (check_for_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].vval.v_dict == வெற்று)
	திரும்பு;

    rettv->vval.v_எண் = dict_has_சாவி(argvars[0].vval.v_dict,
				(எழுத்து *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
