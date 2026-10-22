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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
ಸ್ಥಿರ dict_T		*first_dict = ರಿಕ್ತ;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(ಶೂನ್ಯ)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Add the dict to the list of dicts for garbage collection.
    ಯದಿ (first_dict != ರಿಕ್ತ)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = ರಿಕ್ತ;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    ಪ್ರತ್ಯಾಗಮ d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ಯದಿ (alloc_fail_id == id && alloc_does_fail(ಪರಿಮಾಣ(list_T)))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
    ಪ್ರತ್ಯಾಗಮ (dict_alloc());
}

    dict_T *
dict_alloc_lock(ಪೂರ್ಣಾಙ್ಕ lock)
{
    dict_T *d = dict_alloc();

    ಯದಿ (d != ರಿಕ್ತ)
	d->dv_lock = lock;
    ಪ್ರತ್ಯಾಗಮ d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    rettv_dict_set(rettv, d);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set a dictionary as the return value
 */
    ಶೂನ್ಯ
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    ಯದಿ (d != ರಿಕ್ತ)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    ಶೂನ್ಯ
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = ರಿಕ್ತ;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    ಶೂನ್ಯ
hashtab_free_contents(hashtab_T *ht)
{
    ಪೂರ್ಣಾಙ್ಕ		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    ಯದಿ (check_hashtab_frozen(ht, "clear dict"))
	ಪ್ರತ್ಯಾಗಮ;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (ಪೂರ್ಣಾಙ್ಕ)ht->ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
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

    ಸ್ಥಿರ ಶೂನ್ಯ
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    ಯದಿ (d->dv_used_prev == ರಿಕ್ತ)
	first_dict = d->dv_used_next;
    ಅನ್ಯಥಾ
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    ಯದಿ (d->dv_used_next != ರಿಕ್ತ)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dict_free(dict_T *d)
{
    ಯದಿ (!in_free_unref_ವಸ್ತುಗಳು)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    ಶೂನ್ಯ
dict_unref(dict_T *d)
{
    ಯದಿ (d != ರಿಕ್ತ && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_free_nonref(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    dict_T	*dd;
    ಪೂರ್ಣಾಙ್ಕ		did_free = FALSE;

    ಪ್ರತಿ (dd = first_dict; dd != ರಿಕ್ತ; dd = dd->dv_used_next)
	ಯದಿ ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    ಪ್ರತ್ಯಾಗಮ did_free;
}

    ಶೂನ್ಯ
dict_free_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    dict_T	*dd, *dd_next;

    ಪ್ರತಿ (dd = first_dict; dd != ರಿಕ್ತ; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	ಯದಿ ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
dictitem_alloc(char_u *ಕೀಲಿ)
{
    dictitem_T *di;
    size_t len = STRLEN(ಕೀಲಿ);

    di = alloc(offsetof(dictitem_T, di_ಕೀಲಿ) + len + 1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    mch_memmove(di->di_ಕೀಲಿ, ಕೀಲಿ, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    ಪ್ರತ್ಯಾಗಮ di;
}

/*
 * Make a copy of a Dictionary item.
 */
    ಸ್ಥಿರ dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_ಕೀಲಿ);

    di = alloc(offsetof(dictitem_T, di_ಕೀಲಿ) + len + 1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    mch_memmove(di->di_ಕೀಲಿ, org->di_ಕೀಲಿ, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    ಪ್ರತ್ಯಾಗಮ di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    ಶೂನ್ಯ
dictitem_remove(dict_T *dict, dictitem_T *item, ವರ್ಣ *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_ಕೀಲಿ);
    ಯದಿ (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    ಅನ್ಯಥಾ
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(item);
}

/*
 * Free a dict item.  Also clears the value.
 */
    ಶೂನ್ಯ
dictitem_free(dictitem_T *item)
{
    clear_tv(&item->di_tv);
    ಯದಿ (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, ಪೂರ್ಣಾಙ್ಕ deep, ಪೂರ್ಣಾಙ್ಕ ತುದಿ, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    hashitem_T	*hi;

    ಯದಿ (orig == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    copy = dict_alloc();
    ಯದಿ (copy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    ಯದಿ (orig->dv_type == ರಿಕ್ತ || ತುದಿ || deep)
	copy->dv_type = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (ಪೂರ್ಣಾಙ್ಕ)orig->dv_hashtab.ht_used;
    ಪ್ರತಿ (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_ಕೀಲಿ);
	    ಯದಿ (di == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    ಯದಿ (deep)
	    {
		ಯದಿ (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    ಭಙ್ಗ;
		}
	    }
	    ಅನ್ಯಥಾ
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    ಯದಿ (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		ಭಙ್ಗ;
	    }
	}
    }

    ++copy->dv_refcount;
    ಯದಿ (todo > 0)
    {
	dict_unref(copy);
	copy = ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ (d == get_globvar_dict()
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
    ಪೂರ್ಣಾಙ್ಕ
dict_add(dict_T *d, dictitem_T *item)
{
    ಯದಿ (dict_wrong_func_name(d, &item->di_tv, item->di_ಕೀಲಿ))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ hash_add(&d->dv_hashtab, item->di_ಕೀಲಿ, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dict_add_ಸಂಖ್ಯೆ_special(
    dict_T	*d,
    ವರ್ಣ	*ಕೀಲಿ,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_ಸಂಖ್ಯೆ = nr;
    ಯದಿ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_add_ಸಂಖ್ಯೆ(dict_T *d, ವರ್ಣ *ಕೀಲಿ, varnumber_T nr)
{
    ಪ್ರತ್ಯಾಗಮ dict_add_ಸಂಖ್ಯೆ_special(d, ಕೀಲಿ, nr, VAR_ಸಂಖ್ಯೆ);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_add_bool(dict_T *d, ವರ್ಣ *ಕೀಲಿ, varnumber_T nr)
{
    ಪ್ರತ್ಯಾಗಮ dict_add_ಸಂಖ್ಯೆ_special(d, ಕೀಲಿ, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_add_string(dict_T *d, ವರ್ಣ *ಕೀಲಿ, char_u *str)
{
    ಪ್ರತ್ಯಾಗಮ dict_add_string_len(d, ಕೀಲಿ, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_add_string_len(dict_T *d, ವರ್ಣ *ಕೀಲಿ, char_u *str, ಪೂರ್ಣಾಙ್ಕ len)
{
    dictitem_T	*item;
    char_u	*val = ರಿಕ್ತ;

    item = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    item->di_tv.v_type = VAR_STRING;
    ಯದಿ (str != ರಿಕ್ತ)
    {
	ಯದಿ (len == -1)
	    val = vim_strsave(str);
	ಅನ್ಯಥಾ
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    ಯದಿ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_add_list(dict_T *d, ವರ್ಣ *ಕೀಲಿ, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    ಯದಿ (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = ರಿಕ್ತ;
	dictitem_free(item);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ++list->lv_refcount;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_add_tv(dict_T *d, ವರ್ಣ *ಕೀಲಿ, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    copy_tv(tv, &item->di_tv);
    ಯದಿ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_add_callback(dict_T *d, ವರ್ಣ *ಕೀಲಿ, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    put_callback(cb, &item->di_tv);
    ಯದಿ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_add_func(dict_T *d, ವರ್ಣ *ಕೀಲಿ, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    ಯದಿ (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    ಶೂನ್ಯ
dict_iterate_start(typval_T *var, dict_iterator_T *iter)
{
    ಯದಿ (var->v_type != VAR_DICT || var->vval.v_dict == ರಿಕ್ತ)
	iter->dit_todo = 0;
    ಅನ್ಯಥಾ
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

    ಯದಿ (iter->dit_todo == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯಾವತ್ (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_ಕೀಲಿ;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_add_dict(dict_T *d, ವರ್ಣ *ಕೀಲಿ, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    ಯದಿ (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = ರಿಕ್ತ;
	dictitem_free(item);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ++dict->dv_refcount;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0L;
    ಪ್ರತ್ಯಾಗಮ (long)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = ರಿಕ್ತ;
    hashitem_T	*hi;

    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (len < 0)
	akey = ಕೀಲಿ;
    ಅನ್ಯಥಾ ಯದಿ (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(ಕೀಲಿ, len);
	ಯದಿ (akey == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, ಕೀಲಿ, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    ಯದಿ (HASHITEM_EMPTY(hi))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_has_ಕೀಲಿ(dict_T *d, ವರ್ಣ *ಕೀಲಿ)
{
    ಪ್ರತ್ಯಾಗಮ dict_find(d, (char_u *)ಕೀಲಿ, -1) != ರಿಕ್ತ;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_get_tv(dict_T *d, ವರ್ಣ *ಕೀಲಿ, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)ಕೀಲಿ, -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    copy_tv(&di->di_tv, rettv);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, ವರ್ಣ *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)ಕೀಲಿ, -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    s = tv_get_string(&di->di_tv);
    ಯದಿ (save && s != ರಿಕ್ತ)
	s = vim_strsave(s);
    ಪ್ರತ್ಯಾಗಮ s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_ಸಂಖ್ಯೆ(dict_T *d, ವರ್ಣ *ಕೀಲಿ)
{
    ಪ್ರತ್ಯಾಗಮ dict_get_ಸಂಖ್ಯೆ_def(d, ಕೀಲಿ, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_ಸಂಖ್ಯೆ_def(dict_T *d, ವರ್ಣ *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)ಕೀಲಿ, -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ def;
    ಪ್ರತ್ಯಾಗಮ tv_get_ಸಂಖ್ಯೆ(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_ಸಂಖ್ಯೆ_check(dict_T *d, char_u *ಕೀಲಿ)
{
    dictitem_T	*di;

    di = dict_find(d, ಕೀಲಿ, -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ tv_get_ಸಂಖ್ಯೆ(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, ವರ್ಣ *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)ಕೀಲಿ, -1);
    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ def;
    ಪ್ರತ್ಯಾಗಮ tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ copyID, ಪೂರ್ಣಾಙ್ಕ restore_copyID)
{
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    ಪೂರ್ಣಾಙ್ಕ		prev_lock;

    ಯದಿ ((d = tv->vval.v_dict) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    ಯದಿ (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (ಪೂರ್ಣಾಙ್ಕ)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    ಯದಿ (first)
		first = FALSE;
	    ಅನ್ಯಥಾ
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_ಕೀಲಿ, FALSE);
	    ಯದಿ (tofree != ರಿಕ್ತ)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    ಯದಿ (s != ರಿಕ್ತ)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    ಯದಿ (s == ರಿಕ್ತ || did_echo_string_emsg)
		ಭಙ್ಗ;
	    ಸಾಲು_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    ಯದಿ (todo > 0)
    {
	vim_free(ga.ga_data);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    ಪ್ರತ್ಯಾಗಮ (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    ಸ್ಥಿರ char_u *
skip_literal_ಕೀಲಿ(char_u *ಕೀಲಿ)
{
    char_u *p;

    ಪ್ರತಿ (p = ಕೀಲಿ; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_literal_ಕೀಲಿ_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_ಕೀಲಿ(*arg);

    ಯದಿ (p == *arg)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    ಪ್ರತ್ಯಾಗಮ OK;
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
get_literal_ಕೀಲಿ(char_u **arg)
{
    char_u	*ಕೀಲಿ;
    char_u	*end;
    typval_T	rettv;

    ಯದಿ (**arg == '\'')
    {
	ಯದಿ (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಕೀಲಿ = rettv.vval.v_string;
    }
    ಅನ್ಯಥಾ ಯದಿ (**arg == '"')
    {
	ಯದಿ (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಕೀಲಿ = rettv.vval.v_string;
    }
    ಅನ್ಯಥಾ
    {
	end = skip_literal_ಕೀಲಿ(*arg);
	ಯದಿ (end == *arg)
	{
	    semsg(_(e_invalid_ಕೀಲಿ_str), *arg);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಕೀಲಿ = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಙ್ಕ literal)
{
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg == ರಿಕ್ತ ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = ರಿಕ್ತ;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*ಕೀಲಿ = ರಿಕ್ತ;
    dictitem_T	*item;
    char_u	*curly_ಅಭಿವ್ಯಕ್ತಿ = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    ಪೂರ್ಣಾಙ್ಕ		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    ಯದಿ (!vim9script
	    && *curly_ಅಭಿವ್ಯಕ್ತಿ != '}'
	    && !literal
	    && eval1(&curly_ಅಭಿವ್ಯಕ್ತಿ, &tv, ರಿಕ್ತ) == OK
	    && *skipwhite(curly_ಅಭಿವ್ಯಕ್ತಿ) == '}')
	ಪ್ರತ್ಯಾಗಮ NOTDONE;

    ಯದಿ (evaluate)
    {
	d = dict_alloc();
	ಯದಿ (d == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ಯಾವತ್ (**arg != '}' && **arg != NUL)
    {
	ಪೂರ್ಣಾಙ್ಕ	has_bracket = vim9script && **arg == '[';

	ಯದಿ (literal)
	{
	    ಯದಿ (get_literal_ಕೀಲಿ_tv(arg, &tvkey) == FAIL)
		ಗಚ್ಛ failret;
	}
	ಅನ್ಯಥಾ ಯದಿ (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_ಕೀಲಿ(arg);
	    ಯದಿ (tvkey.vval.v_string == ರಿಕ್ತ)
		ಗಚ್ಛ failret;
	    tvkey.v_type = VAR_STRING;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (has_bracket)
		*arg = skipwhite(*arg + 1);
	    ಯದಿ (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		ಗಚ್ಛ failret;
	    ಯದಿ (has_bracket)
	    {
		*arg = skipwhite(*arg);
		ಯದಿ (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_ಕೀಲಿ));
		    clear_tv(&tvkey);
		    ಗಚ್ಛ failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ಯದಿ (!vim9script)
	    *arg = skipwhite(*arg);
	ಯದಿ (**arg != ':')
	{
	    ಯದಿ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ಅನ್ಯಥಾ
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    ಗಚ್ಛ failret;
	}
	ಯದಿ (evaluate)
	{
	    ಯದಿ (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    ಕೀಲಿ = tv_get_string_buf_chk(&tvkey, buf);
	    ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		ಗಚ್ಛ failret;
	    }
	}
	ಯದಿ (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    ಗಚ್ಛ failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ಯದಿ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    ಗಚ್ಛ failret;
	}
	ಯದಿ (check_typval_is_ಮೌಲ್ಯ(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    ಗಚ್ಛ failret;
	}
	ಯದಿ (evaluate)
	{
	    item = dict_find(d, ಕೀಲಿ, -1);
	    ಯದಿ (item != ರಿಕ್ತ)
	    {
		semsg(_(e_duplicate_ಕೀಲಿ_in_dictionary_str), ಕೀಲಿ);
		clear_tv(&tvkey);
		clear_tv(&tv);
		ಗಚ್ಛ failret;
	    }
	    item = dictitem_alloc(ಕೀಲಿ);
	    ಯದಿ (item != ರಿಕ್ತ)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		ಯದಿ (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ಯದಿ (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	ಯದಿ (had_comma)
	{
	    ಯದಿ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ಗಚ್ಛ failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಯದಿ (**arg == '}')
	    ಭಙ್ಗ;
	ಯದಿ (!had_comma)
	{
	    ಯದಿ (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    ಅನ್ಯಥಾ
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    ಗಚ್ಛ failret;
	}
    }

    ಯದಿ (**arg != '}')
    {
	ಯದಿ (evalarg != ರಿಕ್ತ)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	ಯದಿ (d != ರಿಕ್ತ)
	    dict_free(d);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    *arg = *arg + 1;
    ಯದಿ (evaluate)
	rettv_dict_set(rettv, d);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;

    ಯದಿ (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    ಅನ್ಯಥಾ ಯದಿ ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    ಅನ್ಯಥಾ
	ret = NOTDONE;

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    ಶೂನ್ಯ
dict_extend(dict_T *d1, dict_T *d2, char_u *action, ವರ್ಣ *func_name)
{
    dictitem_T	*di1;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    ಯದಿ (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (*action == 'm')
    {
	ಯದಿ (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    ಪ್ರತ್ಯಾಗಮ;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    ಯದಿ (d1->dv_type != ರಿಕ್ತ && d1->dv_type->tt_member != ರಿಕ್ತ)
	type = d1->dv_type->tt_member;
    ಅನ್ಯಥಾ
	type = ರಿಕ್ತ;

    todo = (ಪೂರ್ಣಾಙ್ಕ)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d2->dv_hashtab, hi2, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_ಕೀಲಿ, -1);
	    // Check the key to be valid when adding to any scope.
	    ಯದಿ (d1->dv_scope != 0 && !valid_varname(hi2->hi_ಕೀಲಿ, -1, TRUE))
		ಭಙ್ಗ;

	    ಯದಿ (type != ರಿಕ್ತ
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		ಭಙ್ಗ;

	    ಯದಿ (di1 == ರಿಕ್ತ)
	    {
		ಯದಿ (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    ಯದಿ (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		ಅನ್ಯಥಾ
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    ಯದಿ (di1 != ರಿಕ್ತ && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*action == 'e')
	    {
		semsg(_(e_ಕೀಲಿ_already_exists_str), hi2->hi_ಕೀಲಿ);
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*action == 'f' && HI2DI(hi2) != di1)
	    {
		ಯದಿ (ಮೌಲ್ಯ_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    ಭಙ್ಗ;
		// Disallow replacing a builtin function.
		ಯದಿ (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_ಕೀಲಿ))
		    ಭಙ್ಗ;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    ಯದಿ (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    ಪ್ರತ್ಯಾಗಮ HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    ಪೂರ್ಣಾಙ್ಕ
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    ಪೂರ್ಣಾಙ್ಕ		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    ಪೂರ್ಣಾಙ್ಕ		todo;

    ಯದಿ (d1 == d2)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (dict_len(d1) != dict_len(d2))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (d1 == ರಿಕ್ತ || d2 == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    todo = (ಪೂರ್ಣಾಙ್ಕ)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d1->dv_hashtab, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_ಕೀಲಿ, -1);
	    ಯದಿ (item2 == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಯದಿ (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    --todo;
	}
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    long
dict_ಎಣಿಕೆ(dict_T *d, typval_T *needle, ಪೂರ್ಣಾಙ್ಕ ic)
{
    ಪೂರ್ಣಾಙ್ಕ		todo;
    hashitem_T	*hi;
    long	n = 0;

    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    todo = (ಪೂರ್ಣಾಙ್ಕ)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ಯದಿ (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    ಶೂನ್ಯ
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	ವರ್ಣ		*func_name,
	char_u		*arg_errmsg,
	ಪೂರ್ಣಾಙ್ಕ		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    ಪೂರ್ಣಾಙ್ಕ	i;

    d1 = argvars[0].vval.v_dict;
    ಯದಿ (d1 == ರಿಕ್ತ)
    {
	emsg(_(e_cannot_extend_null_dict));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!is_new && ಮೌಲ್ಯ_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	ಯದಿ (d1 == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    d2 = argvars[1].vval.v_dict;
    ಯದಿ (d2 == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    // Check the third argument.
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಸ್ಥಿರ ವರ್ಣ *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	ಯದಿ (action == ರಿಕ್ತ)
	{
	    ಯದಿ (is_new)
		dict_unref(d1);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಪ್ರತಿ (i = 0; i < 3; ++i)
	    ಯದಿ (STRCMP(action, av[i]) == 0)
		ಭಙ್ಗ;
	ಯದಿ (i == 3)
	{
	    ಯದಿ (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ
	action = (char_u *)"force";

    ಯದಿ (type != ರಿಕ್ತ && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	ಯದಿ (is_new)
	    dict_unref(d1);
	ಪ್ರತ್ಯಾಗಮ;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    ಯದಿ (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    ಅನ್ಯಥಾ
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    ಶೂನ್ಯ
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	ವರ್ಣ		*func_name,
	char_u		*arg_errmsg,
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
	typval_T	*rettv)
{
    dict_T	*d_ret = ರಿಕ್ತ;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    ಪೂರ್ಣಾಙ್ಕ		todo;
    ಪೂರ್ಣಾಙ್ಕ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ಯದಿ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = ರಿಕ್ತ;
    }
    ಯದಿ (d == ರಿಕ್ತ
	  || (filtermap == FILTERMAP_FILTER
			&& ಮೌಲ್ಯ_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (filtermap == FILTERMAP_MAPNEW)
    {
	ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, &newtv);

    ಪೂರ್ಣಾಙ್ಕ prev_lock = d->dv_lock;
    ಯದಿ (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (ಪೂರ್ಣಾಙ್ಕ)ht->ht_used;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    ಪೂರ್ಣಾಙ್ಕ		r;

	    --todo;
	    di = HI2DI(hi);
	    ಯದಿ (filtermap == FILTERMAP_MAP
		    && (ಮೌಲ್ಯ_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		ಭಙ್ಗ;
	    set_vim_var_string(VV_ಕೀಲಿ, di->di_ಕೀಲಿ, -1);
	    r = filter_map_one(&di->di_tv, ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_ಕೀಲಿ));
	    ಯದಿ (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (filtermap == FILTERMAP_MAP)
	    {
		ಯದಿ (argtype != ರಿಕ್ತ && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ಭಙ್ಗ;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (ವರ್ಣ *)di->di_ಕೀಲಿ, &newtv);
		clear_tv(&newtv);
		ಯದಿ (r == FAIL)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		ಯದಿ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    ಭಙ್ಗ;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    ಯದಿ (fc != ರಿಕ್ತ)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    ಶೂನ್ಯ
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*ಕೀಲಿ;
    dictitem_T	*di;

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	ಪ್ರತ್ಯಾಗಮ;
    }

    d = argvars[0].vval.v_dict;
    ಯದಿ (d == ರಿಕ್ತ || ಮೌಲ್ಯ_check_lock(d->dv_lock, arg_errmsg, TRUE))
	ಪ್ರತ್ಯಾಗಮ;

    ಕೀಲಿ = tv_get_string_chk(&argvars[1]);
    ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    di = dict_find(d, ಕೀಲಿ, -1);
    ಯದಿ (di == ರಿಕ್ತ)
    {
	semsg(_(e_ಕೀಲಿ_not_present_in_dictionary_str), ಕೀಲಿ);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	ಪ್ರತ್ಯಾಗಮ;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    DICT2_LIST_ಕೀಲಿಗಳು,
    DICT2_LIST_ಮೌಲ್ಯಗಳು,
    DICT2_LIST_ವಸ್ತುಗಳು,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    ಪೂರ್ಣಾಙ್ಕ		todo;

    ಯದಿ (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(ಶೂನ್ಯ)rettv_list_alloc(rettv);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == ರಿಕ್ತ ? 0 : (ಪೂರ್ಣಾಙ್ಕ)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_ವಸ್ತುಗಳು(todo);
    ಯದಿ (l == ರಿಕ್ತ)
    {
	(ಶೂನ್ಯ)rettv_list_alloc(rettv);
	ಪ್ರತ್ಯಾಗಮ;
    }
    rettv_list_set(rettv, l);
    ಯದಿ (todo == 0)
	// NULL or empty dict, return the empty list
	ಪ್ರತ್ಯಾಗಮ;

    li = l->lv_first;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
    {
	ಯದಿ (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    ಯದಿ (what == DICT2_LIST_ಕೀಲಿಗಳು)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_ಕೀಲಿ);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (what == DICT2_LIST_ಮೌಲ್ಯಗಳು)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    ಅನ್ಯಥಾ
	    {
		// items()
		l2 = list_alloc_with_ವಸ್ತುಗಳು(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		ಯದಿ (l2 == ರಿಕ್ತ)
		    ಗಚ್ಛ alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_ಕೀಲಿ);
		ಯದಿ (l2->lv_first->li_tv.vval.v_string == ರಿಕ್ತ)
		    ಗಚ್ಛ alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    ಪ್ರತ್ಯಾಗಮ;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    ಶೂನ್ಯ
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ವಸ್ತುಗಳು);
}

/*
 * "keys()" function
 */
    ಶೂನ್ಯ
f_ಕೀಲಿಗಳು(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ಕೀಲಿಗಳು);
}

/*
 * "values(dict)" function
 */
    ಶೂನ್ಯ
f_ಮೌಲ್ಯಗಳು(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ಮೌಲ್ಯಗಳು);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    ಶೂನ್ಯ
dict_set_ವಸ್ತುಗಳು_ro(dict_T *di)
{
    ಪೂರ್ಣಾಙ್ಕ		todo = (ಪೂರ್ಣಾಙ್ಕ)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&di->dv_hashtab, hi, todo)
    {
	ಯದಿ (HASHITEM_EMPTY(hi))
	    ಅನುವರ್ತ;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    ಶೂನ್ಯ
f_has_ಕೀಲಿ(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_for_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].vval.v_dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = dict_has_ಕೀಲಿ(argvars[0].vval.v_dict,
				(ವರ್ಣ *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
