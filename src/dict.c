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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
ਸਥਿਰ dict_T		*ਪਹਿਲਾ_dict = ਨਿਰਾ;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(ਖਾਲੀ)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Add the dict to the list of dicts for garbage collection.
    ਜੇ (ਪਹਿਲਾ_dict != ਨਿਰਾ)
	ਪਹਿਲਾ_dict->dv_ਵਰਤਿਆ_prev = d;
    d->dv_ਵਰਤਿਆ_ਅਗਲਾ = ਪਹਿਲਾ_dict;
    d->dv_ਵਰਤਿਆ_prev = ਨਿਰਾ;
    ਪਹਿਲਾ_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_ਤਾਲਾ = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    ਵਾਪਸ d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (alloc_fail_id == id && alloc_does_fail(ਆਕਾਰ(list_T)))
	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ (dict_alloc());
}

    dict_T *
dict_alloc_ਤਾਲਾ(ਪੂਰਨ ਤਾਲਾ)
{
    dict_T *d = dict_alloc();

    ਜੇ (d != ਨਿਰਾ)
	d->dv_ਤਾਲਾ = ਤਾਲਾ;
    ਵਾਪਸ d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_ਤਾਲਾ(0);

    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    rettv_dict_ਸੈੱਟ(rettv, d);
    ਵਾਪਸ OK;
}

/*
 * Set a dictionary as the return value
 */
    ਖਾਲੀ
rettv_dict_ਸੈੱਟ(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    ਜੇ (d != ਨਿਰਾ)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    ਖਾਲੀ
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = ਨਿਰਾ;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    ਖਾਲੀ
hashtab_free_contents(hashtab_T *ht)
{
    ਪੂਰਨ		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    ਜੇ (check_hashtab_frozen(ht, "clear dict"))
	ਵਾਪਸ;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_ਤਾਲਾ(ht);
    todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
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

    ਸਥਿਰ ਖਾਲੀ
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    ਜੇ (d->dv_ਵਰਤਿਆ_prev == ਨਿਰਾ)
	ਪਹਿਲਾ_dict = d->dv_ਵਰਤਿਆ_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	d->dv_ਵਰਤਿਆ_prev->dv_ਵਰਤਿਆ_ਅਗਲਾ = d->dv_ਵਰਤਿਆ_ਅਗਲਾ;
    ਜੇ (d->dv_ਵਰਤਿਆ_ਅਗਲਾ != ਨਿਰਾ)
	d->dv_ਵਰਤਿਆ_ਅਗਲਾ->dv_ਵਰਤਿਆ_prev = d->dv_ਵਰਤਿਆ_prev;
    vim_free(d);
}

    ਸਥਿਰ ਖਾਲੀ
dict_free(dict_T *d)
{
    ਜੇ (!ਵਿੱਚ_free_unref_ਚੀਜ਼ਾਂ)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    ਖਾਲੀ
dict_unref(dict_T *d)
{
    ਜੇ (d != ਨਿਰਾ && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    ਪੂਰਨ
dict_free_nonref(ਪੂਰਨ copyID)
{
    dict_T	*dd;
    ਪੂਰਨ		did_free = FALSE;

    ਲਈ (dd = ਪਹਿਲਾ_dict; dd != ਨਿਰਾ; dd = dd->dv_ਵਰਤਿਆ_ਅਗਲਾ)
	ਜੇ ((dd->dv_copyID & COPYID_ਨਕਾਬ) != (copyID & COPYID_ਨਕਾਬ))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    ਵਾਪਸ did_free;
}

    ਖਾਲੀ
dict_free_ਚੀਜ਼ਾਂ(ਪੂਰਨ copyID)
{
    dict_T	*dd, *dd_ਅਗਲਾ;

    ਲਈ (dd = ਪਹਿਲਾ_dict; dd != ਨਿਰਾ; dd = dd_ਅਗਲਾ)
    {
	dd_ਅਗਲਾ = dd->dv_ਵਰਤਿਆ_ਅਗਲਾ;
	ਜੇ ((dd->dv_copyID & COPYID_ਨਕਾਬ) != (copyID & COPYID_ਨਕਾਬ))
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
dictitem_alloc(char_u *ਕੁੰਜੀ)
{
    dictitem_T *di;
    size_t len = STRLEN(ਕੁੰਜੀ);

    di = alloc(offsetof(dictitem_T, di_ਕੁੰਜੀ) + len + 1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    mch_memmove(di->di_ਕੁੰਜੀ, ਕੁੰਜੀ, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_ਤਾਲਾ = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    ਵਾਪਸ di;
}

/*
 * Make a copy of a Dictionary item.
 */
    ਸਥਿਰ dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_ਕੁੰਜੀ);

    di = alloc(offsetof(dictitem_T, di_ਕੁੰਜੀ) + len + 1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    mch_memmove(di->di_ਕੁੰਜੀ, org->di_ਕੁੰਜੀ, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    ਵਾਪਸ di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    ਖਾਲੀ
dictitem_remove(dict_T *dict, dictitem_T *ਵਸਤੂ, ਅੱਖਰ *command)
{
    hashitem_T	*hi;

    hi = hash_ਲੱਭੋ(&dict->dv_hashtab, ਵਸਤੂ->di_ਕੁੰਜੀ);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	ਅੰਦਰੂਨੀ_error("dictitem_remove()");
    ਨਹੀਂਤਾਂ
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(ਵਸਤੂ);
}

/*
 * Free a dict item.  Also clears the value.
 */
    ਖਾਲੀ
dictitem_free(dictitem_T *ਵਸਤੂ)
{
    clear_tv(&ਵਸਤੂ->di_tv);
    ਜੇ (ਵਸਤੂ->di_flags & DI_FLAGS_ALLOC)
	vim_free(ਵਸਤੂ);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, ਪੂਰਨ deep, ਪੂਰਨ ਸਿਖਰ, ਪੂਰਨ copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    ਪੂਰਨ		todo;
    hashitem_T	*hi;

    ਜੇ (orig == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    copy = dict_alloc();
    ਜੇ (copy == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    ਜੇ (orig->dv_type == ਨਿਰਾ || ਸਿਖਰ || deep)
	copy->dv_type = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (ਪੂਰਨ)orig->dv_hashtab.ht_ਵਰਤਿਆ;
    ਲਈ (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_ਕੁੰਜੀ);
	    ਜੇ (di == ਨਿਰਾ)
		ਰੋਕੋ;
	    ਜੇ (deep)
	    {
		ਜੇ (ਵਸਤੂ_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    ਰੋਕੋ;
		}
	    }
	    ਨਹੀਂਤਾਂ
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    ਜੇ (dict_ਜੋੜੋ(copy, di) == FAIL)
	    {
		dictitem_free(di);
		ਰੋਕੋ;
	    }
	}
    }

    ++copy->dv_refcount;
    ਜੇ (todo > 0)
    {
	dict_unref(copy);
	copy = ਨਿਰਾ;
    }

    ਵਾਪਸ copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    ਪੂਰਨ
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    ਵਾਪਸ (d == get_globvar_dict()
		|| (ਵਿੱਚ_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid)
		   && d == &SCRIPT_ਵਸਤੂ(current_sctx.sc_sid)->sn_vars->sv_dict)
		|| &d->dv_hashtab == get_funccal_local_ht())
	    && (tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
	    && var_wrong_func_name(name, TRUE);
}

/*
 * Add item "item" to Dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ਪੂਰਨ
dict_ਜੋੜੋ(dict_T *d, dictitem_T *ਵਸਤੂ)
{
    ਜੇ (dict_wrong_func_name(d, &ਵਸਤੂ->di_tv, ਵਸਤੂ->di_ਕੁੰਜੀ))
	ਵਾਪਸ FAIL;
    ਵਾਪਸ hash_ਜੋੜੋ(&d->dv_hashtab, ਵਸਤੂ->di_ਕੁੰਜੀ, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ਸਥਿਰ ਪੂਰਨ
dict_ਜੋੜੋ_ਸੰਖਿਆ_special(
    dict_T	*d,
    ਅੱਖਰ	*ਕੁੰਜੀ,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*ਵਸਤੂ;

    ਵਸਤੂ = dictitem_alloc((char_u *)ਕੁੰਜੀ);
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਸਤੂ->di_tv.v_type = vartype;
    ਵਸਤੂ->di_tv.vval.v_ਸੰਖਿਆ = nr;
    ਜੇ (dict_ਜੋੜੋ(d, ਵਸਤੂ) == FAIL)
    {
	dictitem_free(ਵਸਤੂ);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ਪੂਰਨ
dict_ਜੋੜੋ_ਸੰਖਿਆ(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, varnumber_T nr)
{
    ਵਾਪਸ dict_ਜੋੜੋ_ਸੰਖਿਆ_special(d, ਕੁੰਜੀ, nr, VAR_ਸੰਖਿਆ);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ਪੂਰਨ
dict_ਜੋੜੋ_bool(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, varnumber_T nr)
{
    ਵਾਪਸ dict_ਜੋੜੋ_ਸੰਖਿਆ_special(d, ਕੁੰਜੀ, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ਪੂਰਨ
dict_ਜੋੜੋ_string(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, char_u *str)
{
    ਵਾਪਸ dict_ਜੋੜੋ_string_len(d, ਕੁੰਜੀ, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    ਪੂਰਨ
dict_ਜੋੜੋ_string_len(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, char_u *str, ਪੂਰਨ len)
{
    dictitem_T	*ਵਸਤੂ;
    char_u	*val = ਨਿਰਾ;

    ਵਸਤੂ = dictitem_alloc((char_u *)ਕੁੰਜੀ);
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਸਤੂ->di_tv.v_type = VAR_STRING;
    ਜੇ (str != ਨਿਰਾ)
    {
	ਜੇ (len == -1)
	    val = vim_strsave(str);
	ਨਹੀਂਤਾਂ
	    val = vim_strnsave(str, len);
    }
    ਵਸਤੂ->di_tv.vval.v_string = val;
    ਜੇ (dict_ਜੋੜੋ(d, ਵਸਤੂ) == FAIL)
    {
	dictitem_free(ਵਸਤੂ);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ਪੂਰਨ
dict_ਜੋੜੋ_list(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, list_T *list)
{
    dictitem_T	*ਵਸਤੂ;

    ਵਸਤੂ = dictitem_alloc((char_u *)ਕੁੰਜੀ);
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਸਤੂ->di_tv.v_type = VAR_LIST;
    ਵਸਤੂ->di_tv.vval.v_list = list;
    ਜੇ (dict_ਜੋੜੋ(d, ਵਸਤੂ) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	ਵਸਤੂ->di_tv.vval.v_list = ਨਿਰਾ;
	dictitem_free(ਵਸਤੂ);
	ਵਾਪਸ FAIL;
    }
    ++list->lv_refcount;
    ਵਾਪਸ OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ਪੂਰਨ
dict_ਜੋੜੋ_tv(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, typval_T *tv)
{
    dictitem_T	*ਵਸਤੂ;

    ਵਸਤੂ = dictitem_alloc((char_u *)ਕੁੰਜੀ);
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    copy_tv(tv, &ਵਸਤੂ->di_tv);
    ਜੇ (dict_ਜੋੜੋ(d, ਵਸਤੂ) == FAIL)
    {
	dictitem_free(ਵਸਤੂ);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ਪੂਰਨ
dict_ਜੋੜੋ_callback(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, callback_T *cb)
{
    dictitem_T	*ਵਸਤੂ;

    ਵਸਤੂ = dictitem_alloc((char_u *)ਕੁੰਜੀ);
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    put_callback(cb, &ਵਸਤੂ->di_tv);
    ਜੇ (dict_ਜੋੜੋ(d, ਵਸਤੂ) == FAIL)
    {
	dictitem_free(ਵਸਤੂ);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ਪੂਰਨ
dict_ਜੋੜੋ_func(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, ufunc_T *fp)
{
    dictitem_T	*ਵਸਤੂ;

    ਵਸਤੂ = dictitem_alloc((char_u *)ਕੁੰਜੀ);
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਸਤੂ->di_tv.v_type = VAR_FUNC;
    ਵਸਤੂ->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(ਵਸਤੂ->di_tv.vval.v_string);
    ਜੇ (dict_ਜੋੜੋ(d, ਵਸਤੂ) == FAIL)
    {
	dictitem_free(ਵਸਤੂ);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    ਖਾਲੀ
dict_iterate_ਸ਼ੁਰੂ(typval_T *var, dict_iterator_T *iter)
{
    ਜੇ (var->v_type != VAR_DICT || var->vval.v_dict == ਨਿਰਾ)
	iter->dit_todo = 0;
    ਨਹੀਂਤਾਂ
    {
	dict_T	*d = var->vval.v_dict;

	iter->dit_todo = d->dv_hashtab.ht_ਵਰਤਿਆ;
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
dict_iterate_ਅਗਲਾ(dict_iterator_T *iter, typval_T **tv_ਨਤੀਜਾ)
{
    dictitem_T	*di;
    char_u      *ਨਤੀਜਾ;

    ਜੇ (iter->dit_todo == 0)
	ਵਾਪਸ ਨਿਰਾ;

    ਜਦੋਂ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    ਨਤੀਜਾ = di->di_ਕੁੰਜੀ;
    *tv_ਨਤੀਜਾ = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ਪੂਰਨ
dict_ਜੋੜੋ_dict(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, dict_T *dict)
{
    dictitem_T	*ਵਸਤੂ;

    ਵਸਤੂ = dictitem_alloc((char_u *)ਕੁੰਜੀ);
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਸਤੂ->di_tv.v_type = VAR_DICT;
    ਵਸਤੂ->di_tv.vval.v_dict = dict;
    ਜੇ (dict_ਜੋੜੋ(d, ਵਸਤੂ) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	ਵਸਤੂ->di_tv.vval.v_dict = ਨਿਰਾ;
	dictitem_free(ਵਸਤੂ);
	ਵਾਪਸ FAIL;
    }
    ++dict->dv_refcount;
    ਵਾਪਸ OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    ਲੰਮਾ
dict_len(dict_T *d)
{
    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ 0L;
    ਵਾਪਸ (ਲੰਮਾ)d->dv_hashtab.ht_ਵਰਤਿਆ;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_ਲੱਭੋ(dict_T *d, char_u *ਕੁੰਜੀ, ਪੂਰਨ len)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = ਨਿਰਾ;
    hashitem_T	*hi;

    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (len < 0)
	akey = ਕੁੰਜੀ;
    ਨਹੀਂਤਾਂ ਜੇ (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(ਕੁੰਜੀ, len);
	ਜੇ (akey == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, ਕੁੰਜੀ, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_ਲੱਭੋ(&d->dv_hashtab, akey);
    vim_free(tofree);
    ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    ਪੂਰਨ
dict_has_ਕੁੰਜੀ(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ)
{
    ਵਾਪਸ dict_ਲੱਭੋ(d, (char_u *)ਕੁੰਜੀ, -1) != ਨਿਰਾ;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    ਪੂਰਨ
dict_get_tv(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_ਲੱਭੋ(d, (char_u *)ਕੁੰਜੀ, -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    copy_tv(&di->di_tv, rettv);
    ਵਾਪਸ OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, ਪੂਰਨ save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_ਲੱਭੋ(d, (char_u *)ਕੁੰਜੀ, -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    s = tv_get_string(&di->di_tv);
    ਜੇ (save && s != ਨਿਰਾ)
	s = vim_strsave(s);
    ਵਾਪਸ s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_ਸੰਖਿਆ(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ)
{
    ਵਾਪਸ dict_get_ਸੰਖਿਆ_def(d, ਕੁੰਜੀ, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_ਸੰਖਿਆ_def(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, ਪੂਰਨ def)
{
    dictitem_T	*di;

    di = dict_ਲੱਭੋ(d, (char_u *)ਕੁੰਜੀ, -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ def;
    ਵਾਪਸ tv_get_ਸੰਖਿਆ(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_ਸੰਖਿਆ_check(dict_T *d, char_u *ਕੁੰਜੀ)
{
    dictitem_T	*di;

    di = dict_ਲੱਭੋ(d, ਕੁੰਜੀ, -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ 0;
    ਜੇ (di->di_tv.v_type != VAR_ਸੰਖਿਆ)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(&di->di_tv));
	ਵਾਪਸ 0;
    }
    ਵਾਪਸ tv_get_ਸੰਖਿਆ(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, ਅੱਖਰ *ਕੁੰਜੀ, ਪੂਰਨ def)
{
    dictitem_T	*di;

    di = dict_ਲੱਭੋ(d, (char_u *)ਕੁੰਜੀ, -1);
    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ def;
    ਵਾਪਸ tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, ਪੂਰਨ copyID, ਪੂਰਨ restore_copyID)
{
    garray_T	ga;
    ਪੂਰਨ		ਪਹਿਲਾ = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    ਪੂਰਨ		todo;
    ਪੂਰਨ		prev_ਤਾਲਾ;

    ਜੇ ((d = tv->vval.v_dict) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_ਤਾਲਾ = d->dv_ਤਾਲਾ;
    ਜੇ (d->dv_ਤਾਲਾ == 0)
	d->dv_ਤਾਲਾ = VAR_LOCKED;
    hash_ਤਾਲਾ(&d->dv_hashtab);

    todo = (ਪੂਰਨ)d->dv_hashtab.ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&d->dv_hashtab, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;

	    ਜੇ (ਪਹਿਲਾ)
		ਪਹਿਲਾ = FALSE;
	    ਨਹੀਂਤਾਂ
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_ਕੁੰਜੀ, FALSE);
	    ਜੇ (tofree != ਨਿਰਾ)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    ਜੇ (s != ਨਿਰਾ)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    ਜੇ (s == ਨਿਰਾ || did_echo_string_emsg)
		ਰੋਕੋ;
	    ਸਤਰ_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_ਤਾਲਾ = prev_ਤਾਲਾ;
    ਜੇ (todo > 0)
    {
	vim_free(ga.ga_ਅੰਕੜੇ);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    ਵਾਪਸ (char_u *)ga.ga_ਅੰਕੜੇ;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    ਸਥਿਰ char_u *
skip_literal_ਕੁੰਜੀ(char_u *ਕੁੰਜੀ)
{
    char_u *p;

    ਲਈ (p = ਕੁੰਜੀ; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    ਵਾਪਸ p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    ਸਥਿਰ ਪੂਰਨ
get_literal_ਕੁੰਜੀ_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_ਕੁੰਜੀ(*arg);

    ਜੇ (p == *arg)
	ਵਾਪਸ FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    ਵਾਪਸ OK;
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
get_literal_ਕੁੰਜੀ(char_u **arg)
{
    char_u	*ਕੁੰਜੀ;
    char_u	*ਸਿਰਾ;
    typval_T	rettv;

    ਜੇ (**arg == '\'')
    {
	ਜੇ (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	ਕੁੰਜੀ = rettv.vval.v_string;
    }
    ਨਹੀਂਤਾਂ ਜੇ (**arg == '"')
    {
	ਜੇ (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	ਕੁੰਜੀ = rettv.vval.v_string;
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਿਰਾ = skip_literal_ਕੁੰਜੀ(*arg);
	ਜੇ (ਸਿਰਾ == *arg)
	{
	    semsg(_(e_invalid_ਕੁੰਜੀ_str), *arg);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ਕੁੰਜੀ = vim_strnsave(*arg, ਸਿਰਾ - *arg);
	*arg = ਸਿਰਾ;
    }
    ਵਾਪਸ ਕੁੰਜੀ;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    ਪੂਰਨ
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ਪੂਰਨ literal)
{
    ਪੂਰਨ		evaluate = evalarg == ਨਿਰਾ ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = ਨਿਰਾ;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*ਕੁੰਜੀ = ਨਿਰਾ;
    dictitem_T	*ਵਸਤੂ;
    char_u	*curly_ਵਿਅੰਜਕ = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    ਪੂਰਨ		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    ਜੇ (!vim9script
	    && *curly_ਵਿਅੰਜਕ != '}'
	    && !literal
	    && eval1(&curly_ਵਿਅੰਜਕ, &tv, ਨਿਰਾ) == OK
	    && *skipwhite(curly_ਵਿਅੰਜਕ) == '}')
	ਵਾਪਸ NOTDONE;

    ਜੇ (evaluate)
    {
	d = dict_alloc();
	ਜੇ (d == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ਜਦੋਂ (**arg != '}' && **arg != NUL)
    {
	ਪੂਰਨ	has_bracket = vim9script && **arg == '[';

	ਜੇ (literal)
	{
	    ਜੇ (get_literal_ਕੁੰਜੀ_tv(arg, &tvkey) == FAIL)
		ਜਾਓ failret;
	}
	ਨਹੀਂਤਾਂ ਜੇ (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_ਕੁੰਜੀ(arg);
	    ਜੇ (tvkey.vval.v_string == ਨਿਰਾ)
		ਜਾਓ failret;
	    tvkey.v_type = VAR_STRING;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (has_bracket)
		*arg = skipwhite(*arg + 1);
	    ਜੇ (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		ਜਾਓ failret;
	    ਜੇ (has_bracket)
	    {
		*arg = skipwhite(*arg);
		ਜੇ (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_ਕੁੰਜੀ));
		    clear_tv(&tvkey);
		    ਜਾਓ failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ਜੇ (!vim9script)
	    *arg = skipwhite(*arg);
	ਜੇ (**arg != ':')
	{
	    ਜੇ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_missing_colon_ਵਿੱਚ_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    ਜਾਓ failret;
	}
	ਜੇ (evaluate)
	{
	    ਜੇ (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    ਕੁੰਜੀ = tv_get_string_buf_chk(&tvkey, buf);
	    ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		ਜਾਓ failret;
	    }
	}
	ਜੇ (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    ਜਾਓ failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ਜੇ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    ਜਾਓ failret;
	}
	ਜੇ (check_typval_is_ਮੁੱਲ(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    ਜਾਓ failret;
	}
	ਜੇ (evaluate)
	{
	    ਵਸਤੂ = dict_ਲੱਭੋ(d, ਕੁੰਜੀ, -1);
	    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
	    {
		semsg(_(e_duplicate_ਕੁੰਜੀ_ਵਿੱਚ_dictionary_str), ਕੁੰਜੀ);
		clear_tv(&tvkey);
		clear_tv(&tv);
		ਜਾਓ failret;
	    }
	    ਵਸਤੂ = dictitem_alloc(ਕੁੰਜੀ);
	    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
	    {
		ਵਸਤੂ->di_tv = tv;
		ਵਸਤੂ->di_tv.v_ਤਾਲਾ = 0;
		ਜੇ (dict_ਜੋੜੋ(d, ਵਸਤੂ) == FAIL)
		    dictitem_free(ਵਸਤੂ);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ਜੇ (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	ਜੇ (had_comma)
	{
	    ਜੇ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ਜਾਓ failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ਜੇ (**arg == '}')
	    ਰੋਕੋ;
	ਜੇ (!had_comma)
	{
	    ਜੇ (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_missing_comma_ਵਿੱਚ_dictionary_str), *arg);
	    ਜਾਓ failret;
	}
    }

    ਜੇ (**arg != '}')
    {
	ਜੇ (evalarg != ਨਿਰਾ)
	    semsg(_(e_missing_dict_ਸਿਰਾ_str), *arg);
failret:
	ਜੇ (d != ਨਿਰਾ)
	    dict_free(d);
	ਵਾਪਸ FAIL;
    }

    *arg = *arg + 1;
    ਜੇ (evaluate)
	rettv_dict_ਸੈੱਟ(rettv, d);

    ਵਾਪਸ OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    ਪੂਰਨ
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    ਪੂਰਨ		ret = OK;

    ਜੇ (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    ਨਹੀਂਤਾਂ ਜੇ ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    ਨਹੀਂਤਾਂ
	ret = NOTDONE;

    ਵਾਪਸ ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    ਖਾਲੀ
dict_extend(dict_T *d1, dict_T *d2, char_u *action, ਅੱਖਰ *func_name)
{
    dictitem_T	*di1;
    ਪੂਰਨ		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    ਜੇ (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	ਵਾਪਸ;

    ਜੇ (*action == 'm')
    {
	ਜੇ (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    ਵਾਪਸ;
	hash_ਤਾਲਾ(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    ਜੇ (d1->dv_type != ਨਿਰਾ && d1->dv_type->tt_member != ਨਿਰਾ)
	type = d1->dv_type->tt_member;
    ਨਹੀਂਤਾਂ
	type = ਨਿਰਾ;

    todo = (ਪੂਰਨ)d2->dv_hashtab.ht_ਵਰਤਿਆ;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&d2->dv_hashtab, hi2, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi2))
	{
	    --todo;
	    di1 = dict_ਲੱਭੋ(d1, hi2->hi_ਕੁੰਜੀ, -1);
	    // Check the key to be valid when adding to any scope.
	    ਜੇ (d1->dv_scope != 0 && !valid_varname(hi2->hi_ਕੁੰਜੀ, -1, TRUE))
		ਰੋਕੋ;

	    ਜੇ (type != ਨਿਰਾ
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		ਰੋਕੋ;

	    ਜੇ (di1 == ਨਿਰਾ)
	    {
		ਜੇ (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    ਜੇ (dict_ਜੋੜੋ(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		ਨਹੀਂਤਾਂ
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    ਜੇ (di1 != ਨਿਰਾ && dict_ਜੋੜੋ(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*action == 'e')
	    {
		semsg(_(e_ਕੁੰਜੀ_already_exists_str), hi2->hi_ਕੁੰਜੀ);
		ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (*action == 'f' && HI2DI(hi2) != di1)
	    {
		ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(di1->di_tv.v_ਤਾਲਾ, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    ਰੋਕੋ;
		// Disallow replacing a builtin function.
		ਜੇ (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_ਕੁੰਜੀ))
		    ਰੋਕੋ;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    ਜੇ (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    ਵਾਪਸ HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    ਪੂਰਨ
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    ਪੂਰਨ		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    ਪੂਰਨ		todo;

    ਜੇ (d1 == d2)
	ਵਾਪਸ TRUE;
    ਜੇ (dict_len(d1) != dict_len(d2))
	ਵਾਪਸ FALSE;
    ਜੇ (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	ਵਾਪਸ TRUE;
    ਜੇ (d1 == ਨਿਰਾ || d2 == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    todo = (ਪੂਰਨ)d1->dv_hashtab.ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&d1->dv_hashtab, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    item2 = dict_ਲੱਭੋ(d2, hi->hi_ਕੁੰਜੀ, -1);
	    ਜੇ (item2 == ਨਿਰਾ)
		ਵਾਪਸ FALSE;
	    ਜੇ (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		ਵਾਪਸ FALSE;
	    --todo;
	}
    }
    ਵਾਪਸ TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    ਲੰਮਾ
dict_ਗਿਣਤੀ(dict_T *d, typval_T *needle, ਪੂਰਨ ic)
{
    ਪੂਰਨ		todo;
    hashitem_T	*hi;
    ਲੰਮਾ	n = 0;

    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ 0;

    todo = (ਪੂਰਨ)d->dv_hashtab.ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&d->dv_hashtab, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    ਜੇ (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    ਵਾਪਸ n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    ਖਾਲੀ
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	ਅੱਖਰ		*func_name,
	char_u		*arg_errmsg,
	ਪੂਰਨ		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    ਪੂਰਨ	i;

    d1 = argvars[0].vval.v_dict;
    ਜੇ (d1 == ਨਿਰਾ)
    {
	emsg(_(e_cannot_extend_null_dict));
	ਵਾਪਸ;
    }

    ਜੇ (!is_new && ਮੁੱਲ_check_ਤਾਲਾ(d1->dv_ਤਾਲਾ, arg_errmsg, TRUE))
	ਵਾਪਸ;

    ਜੇ (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	ਜੇ (d1 == ਨਿਰਾ)
	    ਵਾਪਸ;
    }

    d2 = argvars[1].vval.v_dict;
    ਜੇ (d2 == ਨਿਰਾ)
	ਜਾਓ theend;

    // Check the third argument.
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ਸਥਿਰ ਅੱਖਰ *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	ਜੇ (action == ਨਿਰਾ)
	{
	    ਜੇ (is_new)
		dict_unref(d1);
	    ਵਾਪਸ;
	}
	ਲਈ (i = 0; i < 3; ++i)
	    ਜੇ (STRCMP(action, av[i]) == 0)
		ਰੋਕੋ;
	ਜੇ (i == 3)
	{
	    ਜੇ (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_ਦਲੀਲ_str), action);
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ
	action = (char_u *)"force";

    ਜੇ (type != ਨਿਰਾ && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	ਜੇ (is_new)
	    dict_unref(d1);
	ਵਾਪਸ;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    ਜੇ (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_ਤਾਲਾ = FALSE;
    }
    ਨਹੀਂਤਾਂ
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    ਖਾਲੀ
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	ਅੱਖਰ		*func_name,
	char_u		*arg_errmsg,
	typval_T	*ਵਿਅੰਜਕ,
	typval_T	*rettv)
{
    dict_T	*d_ret = ਨਿਰਾ;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    ਪੂਰਨ		todo;
    ਪੂਰਨ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ਜੇ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = ਨਿਰਾ;
    }
    ਜੇ (d == ਨਿਰਾ
	  || (filtermap == FILTERMAP_FILTER
			&& ਮੁੱਲ_check_ਤਾਲਾ(d->dv_ਤਾਲਾ, arg_errmsg, TRUE)))
	ਵਾਪਸ;

    ਜੇ (filtermap == FILTERMAP_MAPNEW)
    {
	ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	    ਵਾਪਸ;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ਵਿਅੰਜਕ_get_funccal(ਵਿਅੰਜਕ, &newtv);

    ਪੂਰਨ prev_ਤਾਲਾ = d->dv_ਤਾਲਾ;
    ਜੇ (d->dv_ਤਾਲਾ == 0)
	d->dv_ਤਾਲਾ = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_ਤਾਲਾ(ht);
    todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    ਪੂਰਨ		r;

	    --todo;
	    di = HI2DI(hi);
	    ਜੇ (filtermap == FILTERMAP_MAP
		    && (ਮੁੱਲ_check_ਤਾਲਾ(di->di_tv.v_ਤਾਲਾ,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		ਰੋਕੋ;
	    ਸੈੱਟ_vim_var_string(VV_ਕੁੰਜੀ, di->di_ਕੁੰਜੀ, -1);
	    r = filter_map_one(&di->di_tv, ਵਿਅੰਜਕ, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_ਕੁੰਜੀ));
	    ਜੇ (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		ਰੋਕੋ;
	    }
	    ਜੇ (filtermap == FILTERMAP_MAP)
	    {
		ਜੇ (argtype != ਨਿਰਾ && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ਰੋਕੋ;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_ਤਾਲਾ = 0;
		di->di_tv = newtv;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_ਜੋੜੋ_tv(d_ret, (ਅੱਖਰ *)di->di_ਕੁੰਜੀ, &newtv);
		clear_tv(&newtv);
		ਜੇ (r == FAIL)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		ਜੇ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    ਰੋਕੋ;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_ਤਾਲਾ = prev_ਤਾਲਾ;
    ਜੇ (fc != ਨਿਰਾ)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    ਖਾਲੀ
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*ਕੁੰਜੀ;
    dictitem_T	*di;

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	ਵਾਪਸ;
    }

    d = argvars[0].vval.v_dict;
    ਜੇ (d == ਨਿਰਾ || ਮੁੱਲ_check_ਤਾਲਾ(d->dv_ਤਾਲਾ, arg_errmsg, TRUE))
	ਵਾਪਸ;

    ਕੁੰਜੀ = tv_get_string_chk(&argvars[1]);
    ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
	ਵਾਪਸ;

    di = dict_ਲੱਭੋ(d, ਕੁੰਜੀ, -1);
    ਜੇ (di == ਨਿਰਾ)
    {
	semsg(_(e_ਕੁੰਜੀ_not_present_ਵਿੱਚ_dictionary_str), ਕੁੰਜੀ);
	ਵਾਪਸ;
    }

    ਜੇ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	ਵਾਪਸ;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    DICT2_LIST_ਕੁੰਜੀਆਂ,
    DICT2_LIST_ਮੁੱਲਾਂ,
    DICT2_LIST_ਚੀਜ਼ਾਂ,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    ਸਥਿਰ ਖਾਲੀ
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    ਪੂਰਨ		todo;

    ਜੇ (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(ਖਾਲੀ)rettv_list_alloc(rettv);
	ਵਾਪਸ;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == ਨਿਰਾ ? 0 : (ਪੂਰਨ)d->dv_hashtab.ht_ਵਰਤਿਆ;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_ਚੀਜ਼ਾਂ(todo);
    ਜੇ (l == ਨਿਰਾ)
    {
	(ਖਾਲੀ)rettv_list_alloc(rettv);
	ਵਾਪਸ;
    }
    rettv_list_ਸੈੱਟ(rettv, l);
    ਜੇ (todo == 0)
	// NULL or empty dict, return the empty list
	ਵਾਪਸ;

    li = l->lv_ਪਹਿਲਾ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&d->dv_hashtab, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    ਜੇ (what == DICT2_LIST_ਕੁੰਜੀਆਂ)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_ਤਾਲਾ = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_ਕੁੰਜੀ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (what == DICT2_LIST_ਮੁੱਲਾਂ)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// items()
		l2 = list_alloc_with_ਚੀਜ਼ਾਂ(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_ਤਾਲਾ = 0;
		li->li_tv.vval.v_list = l2;
		ਜੇ (l2 == ਨਿਰਾ)
		    ਜਾਓ alloc_failed;
		++l2->lv_refcount;

		l2->lv_ਪਹਿਲਾ->li_tv.v_type = VAR_STRING;
		l2->lv_ਪਹਿਲਾ->li_tv.vval.v_string = vim_strsave(di->di_ਕੁੰਜੀ);
		ਜੇ (l2->lv_ਪਹਿਲਾ->li_tv.vval.v_string == ਨਿਰਾ)
		    ਜਾਓ alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_ਆਖਰੀ->li_tv);
	    }
	    li = li->li_ਅਗਲਾ;
	}
    }
    ਵਾਪਸ;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (ਖਾਲੀ)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    ਖਾਲੀ
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ਚੀਜ਼ਾਂ);
}

/*
 * "keys()" function
 */
    ਖਾਲੀ
f_ਕੁੰਜੀਆਂ(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ਕੁੰਜੀਆਂ);
}

/*
 * "values(dict)" function
 */
    ਖਾਲੀ
f_ਮੁੱਲਾਂ(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ਮੁੱਲਾਂ);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    ਖਾਲੀ
dict_ਸੈੱਟ_ਚੀਜ਼ਾਂ_ro(dict_T *di)
{
    ਪੂਰਨ		todo = (ਪੂਰਨ)di->dv_hashtab.ht_ਵਰਤਿਆ;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&di->dv_hashtab, hi, todo)
    {
	ਜੇ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	    ਜਾਰੀ;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    ਖਾਲੀ
f_has_ਕੁੰਜੀ(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (check_for_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].vval.v_dict == ਨਿਰਾ)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = dict_has_ਕੁੰਜੀ(argvars[0].vval.v_dict,
				(ਅੱਖਰ *)tv_get_string(&argvars[1]));
}

#ਜੇ_ਅੰਤ // defined(FEAT_EVAL)
