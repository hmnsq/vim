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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
స్థిర dict_T		*మొదటి_dict = ఖాళీ;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(శూన్య)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    ఒకవేళ (d == ఖాళీ)
	తిరిగి ఖాళీ;

    // Add the dict to the list of dicts for garbage collection.
    ఒకవేళ (మొదటి_dict != ఖాళీ)
	మొదటి_dict->dv_వాడిన_prev = d;
    d->dv_వాడిన_తదుపరి = మొదటి_dict;
    d->dv_వాడిన_prev = ఖాళీ;
    మొదటి_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_తాళం = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    తిరిగి d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (alloc_fail_id == id && alloc_does_fail(పరిమాణం(list_T)))
	తిరిగి ఖాళీ;
#అయితే_ముగింపు
    తిరిగి (dict_alloc());
}

    dict_T *
dict_alloc_తాళం(పూర్ణాంకం తాళం)
{
    dict_T *d = dict_alloc();

    ఒకవేళ (d != ఖాళీ)
	d->dv_తాళం = తాళం;
    తిరిగి d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_తాళం(0);

    ఒకవేళ (d == ఖాళీ)
	తిరిగి FAIL;

    rettv_dict_సమితి(rettv, d);
    తిరిగి OK;
}

/*
 * Set a dictionary as the return value
 */
    శూన్య
rettv_dict_సమితి(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    ఒకవేళ (d != ఖాళీ)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    శూన్య
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = ఖాళీ;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    శూన్య
hashtab_free_contents(hashtab_T *ht)
{
    పూర్ణాంకం		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    ఒకవేళ (check_hashtab_frozen(ht, "clear dict"))
	తిరిగి;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_తాళం(ht);
    todo = (పూర్ణాంకం)ht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
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

    స్థిర శూన్య
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    ఒకవేళ (d->dv_వాడిన_prev == ఖాళీ)
	మొదటి_dict = d->dv_వాడిన_తదుపరి;
    లేకపోతే
	d->dv_వాడిన_prev->dv_వాడిన_తదుపరి = d->dv_వాడిన_తదుపరి;
    ఒకవేళ (d->dv_వాడిన_తదుపరి != ఖాళీ)
	d->dv_వాడిన_తదుపరి->dv_వాడిన_prev = d->dv_వాడిన_prev;
    vim_free(d);
}

    స్థిర శూన్య
dict_free(dict_T *d)
{
    ఒకవేళ (!లోపల_free_unref_వస్తువులు)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    శూన్య
dict_unref(dict_T *d)
{
    ఒకవేళ (d != ఖాళీ && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    పూర్ణాంకం
dict_free_nonref(పూర్ణాంకం copyID)
{
    dict_T	*dd;
    పూర్ణాంకం		did_free = FALSE;

    కొరకు (dd = మొదటి_dict; dd != ఖాళీ; dd = dd->dv_వాడిన_తదుపరి)
	ఒకవేళ ((dd->dv_copyID & COPYID_ముసుగు) != (copyID & COPYID_ముసుగు))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    తిరిగి did_free;
}

    శూన్య
dict_free_వస్తువులు(పూర్ణాంకం copyID)
{
    dict_T	*dd, *dd_తదుపరి;

    కొరకు (dd = మొదటి_dict; dd != ఖాళీ; dd = dd_తదుపరి)
    {
	dd_తదుపరి = dd->dv_వాడిన_తదుపరి;
	ఒకవేళ ((dd->dv_copyID & COPYID_ముసుగు) != (copyID & COPYID_ముసుగు))
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
dictitem_alloc(char_u *కీలకం)
{
    dictitem_T *di;
    size_t len = STRLEN(కీలకం);

    di = alloc(offsetof(dictitem_T, di_కీలకం) + len + 1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి ఖాళీ;

    mch_memmove(di->di_కీలకం, కీలకం, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_తాళం = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    తిరిగి di;
}

/*
 * Make a copy of a Dictionary item.
 */
    స్థిర dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_కీలకం);

    di = alloc(offsetof(dictitem_T, di_కీలకం) + len + 1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి ఖాళీ;

    mch_memmove(di->di_కీలకం, org->di_కీలకం, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    తిరిగి di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    శూన్య
dictitem_remove(dict_T *dict, dictitem_T *అంశం, అక్షరం *command)
{
    hashitem_T	*hi;

    hi = hash_కనుగొను(&dict->dv_hashtab, అంశం->di_కీలకం);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	అంతర్గత_error("dictitem_remove()");
    లేకపోతే
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(అంశం);
}

/*
 * Free a dict item.  Also clears the value.
 */
    శూన్య
dictitem_free(dictitem_T *అంశం)
{
    clear_tv(&అంశం->di_tv);
    ఒకవేళ (అంశం->di_flags & DI_FLAGS_ALLOC)
	vim_free(అంశం);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, పూర్ణాంకం deep, పూర్ణాంకం శిఖరం, పూర్ణాంకం copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    పూర్ణాంకం		todo;
    hashitem_T	*hi;

    ఒకవేళ (orig == ఖాళీ)
	తిరిగి ఖాళీ;

    copy = dict_alloc();
    ఒకవేళ (copy == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    ఒకవేళ (orig->dv_type == ఖాళీ || శిఖరం || deep)
	copy->dv_type = ఖాళీ;
    లేకపోతే
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (పూర్ణాంకం)orig->dv_hashtab.ht_వాడిన;
    కొరకు (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_కీలకం);
	    ఒకవేళ (di == ఖాళీ)
		ఆపు;
	    ఒకవేళ (deep)
	    {
		ఒకవేళ (అంశం_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    ఆపు;
		}
	    }
	    లేకపోతే
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    ఒకవేళ (dict_కూడు(copy, di) == FAIL)
	    {
		dictitem_free(di);
		ఆపు;
	    }
	}
    }

    ++copy->dv_refcount;
    ఒకవేళ (todo > 0)
    {
	dict_unref(copy);
	copy = ఖాళీ;
    }

    తిరిగి copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    పూర్ణాంకం
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    తిరిగి (d == get_globvar_dict()
		|| (లోపల_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid)
		   && d == &SCRIPT_అంశం(current_sctx.sc_sid)->sn_vars->sv_dict)
		|| &d->dv_hashtab == get_funccal_local_ht())
	    && (tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
	    && var_wrong_func_name(name, TRUE);
}

/*
 * Add item "item" to Dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    పూర్ణాంకం
dict_కూడు(dict_T *d, dictitem_T *అంశం)
{
    ఒకవేళ (dict_wrong_func_name(d, &అంశం->di_tv, అంశం->di_కీలకం))
	తిరిగి FAIL;
    తిరిగి hash_కూడు(&d->dv_hashtab, అంశం->di_కీలకం, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    స్థిర పూర్ణాంకం
dict_కూడు_సంఖ్య_special(
    dict_T	*d,
    అక్షరం	*కీలకం,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*అంశం;

    అంశం = dictitem_alloc((char_u *)కీలకం);
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి FAIL;
    అంశం->di_tv.v_type = vartype;
    అంశం->di_tv.vval.v_సంఖ్య = nr;
    ఒకవేళ (dict_కూడు(d, అంశం) == FAIL)
    {
	dictitem_free(అంశం);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    పూర్ణాంకం
dict_కూడు_సంఖ్య(dict_T *d, అక్షరం *కీలకం, varnumber_T nr)
{
    తిరిగి dict_కూడు_సంఖ్య_special(d, కీలకం, nr, VAR_సంఖ్య);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    పూర్ణాంకం
dict_కూడు_bool(dict_T *d, అక్షరం *కీలకం, varnumber_T nr)
{
    తిరిగి dict_కూడు_సంఖ్య_special(d, కీలకం, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    పూర్ణాంకం
dict_కూడు_string(dict_T *d, అక్షరం *కీలకం, char_u *str)
{
    తిరిగి dict_కూడు_string_len(d, కీలకం, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    పూర్ణాంకం
dict_కూడు_string_len(dict_T *d, అక్షరం *కీలకం, char_u *str, పూర్ణాంకం len)
{
    dictitem_T	*అంశం;
    char_u	*val = ఖాళీ;

    అంశం = dictitem_alloc((char_u *)కీలకం);
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి FAIL;
    అంశం->di_tv.v_type = VAR_STRING;
    ఒకవేళ (str != ఖాళీ)
    {
	ఒకవేళ (len == -1)
	    val = vim_strsave(str);
	లేకపోతే
	    val = vim_strnsave(str, len);
    }
    అంశం->di_tv.vval.v_string = val;
    ఒకవేళ (dict_కూడు(d, అంశం) == FAIL)
    {
	dictitem_free(అంశం);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    పూర్ణాంకం
dict_కూడు_list(dict_T *d, అక్షరం *కీలకం, list_T *list)
{
    dictitem_T	*అంశం;

    అంశం = dictitem_alloc((char_u *)కీలకం);
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి FAIL;
    అంశం->di_tv.v_type = VAR_LIST;
    అంశం->di_tv.vval.v_list = list;
    ఒకవేళ (dict_కూడు(d, అంశం) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	అంశం->di_tv.vval.v_list = ఖాళీ;
	dictitem_free(అంశం);
	తిరిగి FAIL;
    }
    ++list->lv_refcount;
    తిరిగి OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    పూర్ణాంకం
dict_కూడు_tv(dict_T *d, అక్షరం *కీలకం, typval_T *tv)
{
    dictitem_T	*అంశం;

    అంశం = dictitem_alloc((char_u *)కీలకం);
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి FAIL;
    copy_tv(tv, &అంశం->di_tv);
    ఒకవేళ (dict_కూడు(d, అంశం) == FAIL)
    {
	dictitem_free(అంశం);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    పూర్ణాంకం
dict_కూడు_callback(dict_T *d, అక్షరం *కీలకం, callback_T *cb)
{
    dictitem_T	*అంశం;

    అంశం = dictitem_alloc((char_u *)కీలకం);
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి FAIL;
    put_callback(cb, &అంశం->di_tv);
    ఒకవేళ (dict_కూడు(d, అంశం) == FAIL)
    {
	dictitem_free(అంశం);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    పూర్ణాంకం
dict_కూడు_func(dict_T *d, అక్షరం *కీలకం, ufunc_T *fp)
{
    dictitem_T	*అంశం;

    అంశం = dictitem_alloc((char_u *)కీలకం);
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి FAIL;
    అంశం->di_tv.v_type = VAR_FUNC;
    అంశం->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(అంశం->di_tv.vval.v_string);
    ఒకవేళ (dict_కూడు(d, అంశం) == FAIL)
    {
	dictitem_free(అంశం);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    శూన్య
dict_iterate_ప్రారంభం(typval_T *var, dict_iterator_T *iter)
{
    ఒకవేళ (var->v_type != VAR_DICT || var->vval.v_dict == ఖాళీ)
	iter->dit_todo = 0;
    లేకపోతే
    {
	dict_T	*d = var->vval.v_dict;

	iter->dit_todo = d->dv_hashtab.ht_వాడిన;
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
dict_iterate_తదుపరి(dict_iterator_T *iter, typval_T **tv_ఫలితం)
{
    dictitem_T	*di;
    char_u      *ఫలితం;

    ఒకవేళ (iter->dit_todo == 0)
	తిరిగి ఖాళీ;

    అయితే (HASHITEM_ఖాళీలు(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    ఫలితం = di->di_కీలకం;
    *tv_ఫలితం = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    తిరిగి ఫలితం;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    పూర్ణాంకం
dict_కూడు_dict(dict_T *d, అక్షరం *కీలకం, dict_T *dict)
{
    dictitem_T	*అంశం;

    అంశం = dictitem_alloc((char_u *)కీలకం);
    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి FAIL;
    అంశం->di_tv.v_type = VAR_DICT;
    అంశం->di_tv.vval.v_dict = dict;
    ఒకవేళ (dict_కూడు(d, అంశం) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	అంశం->di_tv.vval.v_dict = ఖాళీ;
	dictitem_free(అంశం);
	తిరిగి FAIL;
    }
    ++dict->dv_refcount;
    తిరిగి OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    దీర్ఘ
dict_len(dict_T *d)
{
    ఒకవేళ (d == ఖాళీ)
	తిరిగి 0L;
    తిరిగి (దీర్ఘ)d->dv_hashtab.ht_వాడిన;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_కనుగొను(dict_T *d, char_u *కీలకం, పూర్ణాంకం len)
{
#నిర్వచించు AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = ఖాళీ;
    hashitem_T	*hi;

    ఒకవేళ (d == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (len < 0)
	akey = కీలకం;
    లేకపోతే ఒకవేళ (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(కీలకం, len);
	ఒకవేళ (akey == ఖాళీ)
	    తిరిగి ఖాళీ;
    }
    లేకపోతే
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, కీలకం, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_కనుగొను(&d->dv_hashtab, akey);
    vim_free(tofree);
    ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	తిరిగి ఖాళీ;
    తిరిగి HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    పూర్ణాంకం
dict_has_కీలకం(dict_T *d, అక్షరం *కీలకం)
{
    తిరిగి dict_కనుగొను(d, (char_u *)కీలకం, -1) != ఖాళీ;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    పూర్ణాంకం
dict_get_tv(dict_T *d, అక్షరం *కీలకం, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_కనుగొను(d, (char_u *)కీలకం, -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి FAIL;
    copy_tv(&di->di_tv, rettv);
    తిరిగి OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, అక్షరం *కీలకం, పూర్ణాంకం save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_కనుగొను(d, (char_u *)కీలకం, -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి ఖాళీ;
    s = tv_get_string(&di->di_tv);
    ఒకవేళ (save && s != ఖాళీ)
	s = vim_strsave(s);
    తిరిగి s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_సంఖ్య(dict_T *d, అక్షరం *కీలకం)
{
    తిరిగి dict_get_సంఖ్య_def(d, కీలకం, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_సంఖ్య_def(dict_T *d, అక్షరం *కీలకం, పూర్ణాంకం def)
{
    dictitem_T	*di;

    di = dict_కనుగొను(d, (char_u *)కీలకం, -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి def;
    తిరిగి tv_get_సంఖ్య(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_సంఖ్య_check(dict_T *d, char_u *కీలకం)
{
    dictitem_T	*di;

    di = dict_కనుగొను(d, కీలకం, -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి 0;
    ఒకవేళ (di->di_tv.v_type != VAR_సంఖ్య)
    {
	semsg(_(e_invalid_వాదం_str), tv_get_string(&di->di_tv));
	తిరిగి 0;
    }
    తిరిగి tv_get_సంఖ్య(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, అక్షరం *కీలకం, పూర్ణాంకం def)
{
    dictitem_T	*di;

    di = dict_కనుగొను(d, (char_u *)కీలకం, -1);
    ఒకవేళ (di == ఖాళీ)
	తిరిగి def;
    తిరిగి tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, పూర్ణాంకం copyID, పూర్ణాంకం restore_copyID)
{
    garray_T	ga;
    పూర్ణాంకం		మొదటి = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    పూర్ణాంకం		todo;
    పూర్ణాంకం		prev_తాళం;

    ఒకవేళ ((d = tv->vval.v_dict) == ఖాళీ)
	తిరిగి ఖాళీ;
    ga_init2(&ga, పరిమాణం(అక్షరం), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_తాళం = d->dv_తాళం;
    ఒకవేళ (d->dv_తాళం == 0)
	d->dv_తాళం = VAR_LOCKED;
    hash_తాళం(&d->dv_hashtab);

    todo = (పూర్ణాంకం)d->dv_hashtab.ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(&d->dv_hashtab, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;

	    ఒకవేళ (మొదటి)
		మొదటి = FALSE;
	    లేకపోతే
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_కీలకం, FALSE);
	    ఒకవేళ (tofree != ఖాళీ)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    ఒకవేళ (s != ఖాళీ)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    ఒకవేళ (s == ఖాళీ || did_echo_string_emsg)
		ఆపు;
	    పంక్తి_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_తాళం = prev_తాళం;
    ఒకవేళ (todo > 0)
    {
	vim_free(ga.ga_దత్తాంశం);
	తిరిగి ఖాళీ;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    తిరిగి (char_u *)ga.ga_దత్తాంశం;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    స్థిర char_u *
skip_literal_కీలకం(char_u *కీలకం)
{
    char_u *p;

    కొరకు (p = కీలకం; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    తిరిగి p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    స్థిర పూర్ణాంకం
get_literal_కీలకం_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_కీలకం(*arg);

    ఒకవేళ (p == *arg)
	తిరిగి FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    తిరిగి OK;
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
get_literal_కీలకం(char_u **arg)
{
    char_u	*కీలకం;
    char_u	*చివర;
    typval_T	rettv;

    ఒకవేళ (**arg == '\'')
    {
	ఒకవేళ (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    తిరిగి ఖాళీ;
	కీలకం = rettv.vval.v_string;
    }
    లేకపోతే ఒకవేళ (**arg == '"')
    {
	ఒకవేళ (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    తిరిగి ఖాళీ;
	కీలకం = rettv.vval.v_string;
    }
    లేకపోతే
    {
	చివర = skip_literal_కీలకం(*arg);
	ఒకవేళ (చివర == *arg)
	{
	    semsg(_(e_invalid_కీలకం_str), *arg);
	    తిరిగి ఖాళీ;
	}
	కీలకం = vim_strnsave(*arg, చివర - *arg);
	*arg = చివర;
    }
    తిరిగి కీలకం;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    పూర్ణాంకం
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, పూర్ణాంకం literal)
{
    పూర్ణాంకం		evaluate = evalarg == ఖాళీ ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = ఖాళీ;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*కీలకం = ఖాళీ;
    dictitem_T	*అంశం;
    char_u	*curly_సమాసం = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    పూర్ణాంకం		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    ఒకవేళ (!vim9script
	    && *curly_సమాసం != '}'
	    && !literal
	    && eval1(&curly_సమాసం, &tv, ఖాళీ) == OK
	    && *skipwhite(curly_సమాసం) == '}')
	తిరిగి NOTDONE;

    ఒకవేళ (evaluate)
    {
	d = dict_alloc();
	ఒకవేళ (d == ఖాళీ)
	    తిరిగి FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    అయితే (**arg != '}' && **arg != NUL)
    {
	పూర్ణాంకం	has_bracket = vim9script && **arg == '[';

	ఒకవేళ (literal)
	{
	    ఒకవేళ (get_literal_కీలకం_tv(arg, &tvkey) == FAIL)
		వెళ్ళు failret;
	}
	లేకపోతే ఒకవేళ (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_కీలకం(arg);
	    ఒకవేళ (tvkey.vval.v_string == ఖాళీ)
		వెళ్ళు failret;
	    tvkey.v_type = VAR_STRING;
	}
	లేకపోతే
	{
	    ఒకవేళ (has_bracket)
		*arg = skipwhite(*arg + 1);
	    ఒకవేళ (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		వెళ్ళు failret;
	    ఒకవేళ (has_bracket)
	    {
		*arg = skipwhite(*arg);
		ఒకవేళ (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_కీలకం));
		    clear_tv(&tvkey);
		    వెళ్ళు failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ఒకవేళ (!vim9script)
	    *arg = skipwhite(*arg);
	ఒకవేళ (**arg != ':')
	{
	    ఒకవేళ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    లేకపోతే
		semsg(_(e_missing_colon_లోపల_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    వెళ్ళు failret;
	}
	ఒకవేళ (evaluate)
	{
	    ఒకవేళ (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    కీలకం = tv_get_string_buf_chk(&tvkey, buf);
	    ఒకవేళ (కీలకం == ఖాళీ)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		వెళ్ళు failret;
	    }
	}
	ఒకవేళ (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    వెళ్ళు failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ఒకవేళ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    వెళ్ళు failret;
	}
	ఒకవేళ (check_typval_is_విలువ(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    వెళ్ళు failret;
	}
	ఒకవేళ (evaluate)
	{
	    అంశం = dict_కనుగొను(d, కీలకం, -1);
	    ఒకవేళ (అంశం != ఖాళీ)
	    {
		semsg(_(e_duplicate_కీలకం_లోపల_dictionary_str), కీలకం);
		clear_tv(&tvkey);
		clear_tv(&tv);
		వెళ్ళు failret;
	    }
	    అంశం = dictitem_alloc(కీలకం);
	    ఒకవేళ (అంశం != ఖాళీ)
	    {
		అంశం->di_tv = tv;
		అంశం->di_tv.v_తాళం = 0;
		ఒకవేళ (dict_కూడు(d, అంశం) == FAIL)
		    dictitem_free(అంశం);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ఒకవేళ (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	ఒకవేళ (had_comma)
	{
	    ఒకవేళ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		వెళ్ళు failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ఒకవేళ (**arg == '}')
	    ఆపు;
	ఒకవేళ (!had_comma)
	{
	    ఒకవేళ (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    లేకపోతే
		semsg(_(e_missing_comma_లోపల_dictionary_str), *arg);
	    వెళ్ళు failret;
	}
    }

    ఒకవేళ (**arg != '}')
    {
	ఒకవేళ (evalarg != ఖాళీ)
	    semsg(_(e_missing_dict_చివర_str), *arg);
failret:
	ఒకవేళ (d != ఖాళీ)
	    dict_free(d);
	తిరిగి FAIL;
    }

    *arg = *arg + 1;
    ఒకవేళ (evaluate)
	rettv_dict_సమితి(rettv, d);

    తిరిగి OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    పూర్ణాంకం
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    పూర్ణాంకం		ret = OK;

    ఒకవేళ (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    లేకపోతే ఒకవేళ ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    లేకపోతే
	ret = NOTDONE;

    తిరిగి ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    శూన్య
dict_extend(dict_T *d1, dict_T *d2, char_u *action, అక్షరం *func_name)
{
    dictitem_T	*di1;
    పూర్ణాంకం		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    ఒకవేళ (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	తిరిగి;

    ఒకవేళ (*action == 'm')
    {
	ఒకవేళ (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    తిరిగి;
	hash_తాళం(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    ఒకవేళ (d1->dv_type != ఖాళీ && d1->dv_type->tt_member != ఖాళీ)
	type = d1->dv_type->tt_member;
    లేకపోతే
	type = ఖాళీ;

    todo = (పూర్ణాంకం)d2->dv_hashtab.ht_వాడిన;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_వస్తువులు(&d2->dv_hashtab, hi2, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi2))
	{
	    --todo;
	    di1 = dict_కనుగొను(d1, hi2->hi_కీలకం, -1);
	    // Check the key to be valid when adding to any scope.
	    ఒకవేళ (d1->dv_scope != 0 && !valid_varname(hi2->hi_కీలకం, -1, TRUE))
		ఆపు;

	    ఒకవేళ (type != ఖాళీ
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		ఆపు;

	    ఒకవేళ (di1 == ఖాళీ)
	    {
		ఒకవేళ (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    ఒకవేళ (dict_కూడు(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		లేకపోతే
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    ఒకవేళ (di1 != ఖాళీ && dict_కూడు(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    లేకపోతే ఒకవేళ (*action == 'e')
	    {
		semsg(_(e_కీలకం_already_exists_str), hi2->hi_కీలకం);
		ఆపు;
	    }
	    లేకపోతే ఒకవేళ (*action == 'f' && HI2DI(hi2) != di1)
	    {
		ఒకవేళ (విలువ_check_తాళం(di1->di_tv.v_తాళం, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    ఆపు;
		// Disallow replacing a builtin function.
		ఒకవేళ (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_కీలకం))
		    ఆపు;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    ఒకవేళ (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    తిరిగి HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    పూర్ణాంకం
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    పూర్ణాంకం		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    పూర్ణాంకం		todo;

    ఒకవేళ (d1 == d2)
	తిరిగి TRUE;
    ఒకవేళ (dict_len(d1) != dict_len(d2))
	తిరిగి FALSE;
    ఒకవేళ (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	తిరిగి TRUE;
    ఒకవేళ (d1 == ఖాళీ || d2 == ఖాళీ)
	తిరిగి FALSE;

    todo = (పూర్ణాంకం)d1->dv_hashtab.ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(&d1->dv_hashtab, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    item2 = dict_కనుగొను(d2, hi->hi_కీలకం, -1);
	    ఒకవేళ (item2 == ఖాళీ)
		తిరిగి FALSE;
	    ఒకవేళ (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		తిరిగి FALSE;
	    --todo;
	}
    }
    తిరిగి TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    దీర్ఘ
dict_లెక్క(dict_T *d, typval_T *needle, పూర్ణాంకం ic)
{
    పూర్ణాంకం		todo;
    hashitem_T	*hi;
    దీర్ఘ	n = 0;

    ఒకవేళ (d == ఖాళీ)
	తిరిగి 0;

    todo = (పూర్ణాంకం)d->dv_hashtab.ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(&d->dv_hashtab, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    ఒకవేళ (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    తిరిగి n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    శూన్య
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	అక్షరం		*func_name,
	char_u		*arg_errmsg,
	పూర్ణాంకం		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    పూర్ణాంకం	i;

    d1 = argvars[0].vval.v_dict;
    ఒకవేళ (d1 == ఖాళీ)
    {
	emsg(_(e_cannot_extend_null_dict));
	తిరిగి;
    }

    ఒకవేళ (!is_new && విలువ_check_తాళం(d1->dv_తాళం, arg_errmsg, TRUE))
	తిరిగి;

    ఒకవేళ (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	ఒకవేళ (d1 == ఖాళీ)
	    తిరిగి;
    }

    d2 = argvars[1].vval.v_dict;
    ఒకవేళ (d2 == ఖాళీ)
	వెళ్ళు theend;

    // Check the third argument.
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	స్థిర అక్షరం *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	ఒకవేళ (action == ఖాళీ)
	{
	    ఒకవేళ (is_new)
		dict_unref(d1);
	    తిరిగి;
	}
	కొరకు (i = 0; i < 3; ++i)
	    ఒకవేళ (STRCMP(action, av[i]) == 0)
		ఆపు;
	ఒకవేళ (i == 3)
	{
	    ఒకవేళ (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_వాదం_str), action);
	    తిరిగి;
	}
    }
    లేకపోతే
	action = (char_u *)"force";

    ఒకవేళ (type != ఖాళీ && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	ఒకవేళ (is_new)
	    dict_unref(d1);
	తిరిగి;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    ఒకవేళ (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_తాళం = FALSE;
    }
    లేకపోతే
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    శూన్య
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	అక్షరం		*func_name,
	char_u		*arg_errmsg,
	typval_T	*సమాసం,
	typval_T	*rettv)
{
    dict_T	*d_ret = ఖాళీ;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    పూర్ణాంకం		todo;
    పూర్ణాంకం		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ఒకవేళ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = ఖాళీ;
    }
    ఒకవేళ (d == ఖాళీ
	  || (filtermap == FILTERMAP_FILTER
			&& విలువ_check_తాళం(d->dv_తాళం, arg_errmsg, TRUE)))
	తిరిగి;

    ఒకవేళ (filtermap == FILTERMAP_MAPNEW)
    {
	ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	    తిరిగి;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_సమాసం_get_funccal(సమాసం, &newtv);

    పూర్ణాంకం prev_తాళం = d->dv_తాళం;
    ఒకవేళ (d->dv_తాళం == 0)
	d->dv_తాళం = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_తాళం(ht);
    todo = (పూర్ణాంకం)ht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    పూర్ణాంకం		r;

	    --todo;
	    di = HI2DI(hi);
	    ఒకవేళ (filtermap == FILTERMAP_MAP
		    && (విలువ_check_తాళం(di->di_tv.v_తాళం,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		ఆపు;
	    సమితి_vim_var_string(VV_కీలకం, di->di_కీలకం, -1);
	    r = filter_map_one(&di->di_tv, సమాసం, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_కీలకం));
	    ఒకవేళ (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		ఆపు;
	    }
	    ఒకవేళ (filtermap == FILTERMAP_MAP)
	    {
		ఒకవేళ (argtype != ఖాళీ && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ఆపు;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_తాళం = 0;
		di->di_tv = newtv;
	    }
	    లేకపోతే ఒకవేళ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_కూడు_tv(d_ret, (అక్షరం *)di->di_కీలకం, &newtv);
		clear_tv(&newtv);
		ఒకవేళ (r == FAIL)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		ఒకవేళ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    ఆపు;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_తాళం = prev_తాళం;
    ఒకవేళ (fc != ఖాళీ)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    శూన్య
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*కీలకం;
    dictitem_T	*di;

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	తిరిగి;
    }

    d = argvars[0].vval.v_dict;
    ఒకవేళ (d == ఖాళీ || విలువ_check_తాళం(d->dv_తాళం, arg_errmsg, TRUE))
	తిరిగి;

    కీలకం = tv_get_string_chk(&argvars[1]);
    ఒకవేళ (కీలకం == ఖాళీ)
	తిరిగి;

    di = dict_కనుగొను(d, కీలకం, -1);
    ఒకవేళ (di == ఖాళీ)
    {
	semsg(_(e_కీలకం_not_present_లోపల_dictionary_str), కీలకం);
	తిరిగి;
    }

    ఒకవేళ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	తిరిగి;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

రకనిర్వచనం గణన {
    DICT2_LIST_కీలకాలు,
    DICT2_LIST_విలువలు,
    DICT2_LIST_వస్తువులు,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    స్థిర శూన్య
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    పూర్ణాంకం		todo;

    ఒకవేళ (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(శూన్య)rettv_list_alloc(rettv);
	తిరిగి;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == ఖాళీ ? 0 : (పూర్ణాంకం)d->dv_hashtab.ht_వాడిన;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_వస్తువులు(todo);
    ఒకవేళ (l == ఖాళీ)
    {
	(శూన్య)rettv_list_alloc(rettv);
	తిరిగి;
    }
    rettv_list_సమితి(rettv, l);
    ఒకవేళ (todo == 0)
	// NULL or empty dict, return the empty list
	తిరిగి;

    li = l->lv_మొదటి;
    FOR_ALL_HASHTAB_వస్తువులు(&d->dv_hashtab, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    ఒకవేళ (what == DICT2_LIST_కీలకాలు)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_తాళం = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_కీలకం);
	    }
	    లేకపోతే ఒకవేళ (what == DICT2_LIST_విలువలు)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    లేకపోతే
	    {
		// items()
		l2 = list_alloc_with_వస్తువులు(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_తాళం = 0;
		li->li_tv.vval.v_list = l2;
		ఒకవేళ (l2 == ఖాళీ)
		    వెళ్ళు alloc_failed;
		++l2->lv_refcount;

		l2->lv_మొదటి->li_tv.v_type = VAR_STRING;
		l2->lv_మొదటి->li_tv.vval.v_string = vim_strsave(di->di_కీలకం);
		ఒకవేళ (l2->lv_మొదటి->li_tv.vval.v_string == ఖాళీ)
		    వెళ్ళు alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_చివరి->li_tv);
	    }
	    li = li->li_తదుపరి;
	}
    }
    తిరిగి;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (శూన్య)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    శూన్య
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_వస్తువులు);
}

/*
 * "keys()" function
 */
    శూన్య
f_కీలకాలు(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_కీలకాలు);
}

/*
 * "values(dict)" function
 */
    శూన్య
f_విలువలు(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_విలువలు);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    శూన్య
dict_సమితి_వస్తువులు_ro(dict_T *di)
{
    పూర్ణాంకం		todo = (పూర్ణాంకం)di->dv_hashtab.ht_వాడిన;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_వస్తువులు(&di->dv_hashtab, hi, todo)
    {
	ఒకవేళ (HASHITEM_ఖాళీలు(hi))
	    కొనసాగు;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    శూన్య
f_has_కీలకం(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (check_for_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].vval.v_dict == ఖాళీ)
	తిరిగి;

    rettv->vval.v_సంఖ్య = dict_has_కీలకం(argvars[0].vval.v_dict,
				(అక్షరం *)tv_get_string(&argvars[1]));
}

#అయితే_ముగింపు // defined(FEAT_EVAL)
