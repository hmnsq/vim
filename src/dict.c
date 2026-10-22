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

#ಆದರೆ defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
ಸ್ಥಿರ dict_T		*ಮೊದಲ_dict = ಖಾಲಿ;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(ಶೂನ್ಯ)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Add the dict to the list of dicts for garbage collection.
    ಆದರೆ (ಮೊದಲ_dict != ಖಾಲಿ)
	ಮೊದಲ_dict->dv_ಬಳಸಿದ_prev = d;
    d->dv_ಬಳಸಿದ_ಮುಂದಿನ = ಮೊದಲ_dict;
    d->dv_ಬಳಸಿದ_prev = ಖಾಲಿ;
    ಮೊದಲ_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_ಬೀಗ = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    ಹಿಂತಿರುಗಿ d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (alloc_fail_id == id && alloc_does_fail(ಗಾತ್ರ(list_T)))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ (dict_alloc());
}

    dict_T *
dict_alloc_ಬೀಗ(ಪೂರ್ಣಾಂಕ ಬೀಗ)
{
    dict_T *d = dict_alloc();

    ಆದರೆ (d != ಖಾಲಿ)
	d->dv_ಬೀಗ = ಬೀಗ;
    ಹಿಂತಿರುಗಿ d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_ಬೀಗ(0);

    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    rettv_dict_ಗಣ(rettv, d);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set a dictionary as the return value
 */
    ಶೂನ್ಯ
rettv_dict_ಗಣ(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    ಆದರೆ (d != ಖಾಲಿ)
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
    d->dv_type = ಖಾಲಿ;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    ಶೂನ್ಯ
hashtab_free_contents(hashtab_T *ht)
{
    ಪೂರ್ಣಾಂಕ		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    ಆದರೆ (check_hashtab_frozen(ht, "clear dict"))
	ಹಿಂತಿರುಗಿ;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_ಬೀಗ(ht);
    todo = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
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
    ಆದರೆ (d->dv_ಬಳಸಿದ_prev == ಖಾಲಿ)
	ಮೊದಲ_dict = d->dv_ಬಳಸಿದ_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	d->dv_ಬಳಸಿದ_prev->dv_ಬಳಸಿದ_ಮುಂದಿನ = d->dv_ಬಳಸಿದ_ಮುಂದಿನ;
    ಆದರೆ (d->dv_ಬಳಸಿದ_ಮುಂದಿನ != ಖಾಲಿ)
	d->dv_ಬಳಸಿದ_ಮುಂದಿನ->dv_ಬಳಸಿದ_prev = d->dv_ಬಳಸಿದ_prev;
    vim_free(d);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dict_free(dict_T *d)
{
    ಆದರೆ (!ಒಳ_free_unref_ವಸ್ತುಗಳು)
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
    ಆದರೆ (d != ಖಾಲಿ && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    ಪೂರ್ಣಾಂಕ
dict_free_nonref(ಪೂರ್ಣಾಂಕ copyID)
{
    dict_T	*dd;
    ಪೂರ್ಣಾಂಕ		did_free = FALSE;

    ಗಾಗಿ (dd = ಮೊದಲ_dict; dd != ಖಾಲಿ; dd = dd->dv_ಬಳಸಿದ_ಮುಂದಿನ)
	ಆದರೆ ((dd->dv_copyID & COPYID_ಮುಖವಾಡ) != (copyID & COPYID_ಮುಖವಾಡ))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    ಹಿಂತಿರುಗಿ did_free;
}

    ಶೂನ್ಯ
dict_free_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಂಕ copyID)
{
    dict_T	*dd, *dd_ಮುಂದಿನ;

    ಗಾಗಿ (dd = ಮೊದಲ_dict; dd != ಖಾಲಿ; dd = dd_ಮುಂದಿನ)
    {
	dd_ಮುಂದಿನ = dd->dv_ಬಳಸಿದ_ಮುಂದಿನ;
	ಆದರೆ ((dd->dv_copyID & COPYID_ಮುಖವಾಡ) != (copyID & COPYID_ಮುಖವಾಡ))
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
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    mch_memmove(di->di_ಕೀಲಿ, ಕೀಲಿ, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_ಬೀಗ = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    ಹಿಂತಿರುಗಿ di;
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
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    mch_memmove(di->di_ಕೀಲಿ, org->di_ಕೀಲಿ, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    ಹಿಂತಿರುಗಿ di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    ಶೂನ್ಯ
dictitem_remove(dict_T *dict, dictitem_T *ಅಂಶ, ಅಕ್ಷರ *command)
{
    hashitem_T	*hi;

    hi = hash_ಕಂಡುಹಿಡಿ(&dict->dv_hashtab, ಅಂಶ->di_ಕೀಲಿ);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	ಆಂತರಿಕ_error("dictitem_remove()");
    ಇಲ್ಲದಿದ್ದರೆ
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(ಅಂಶ);
}

/*
 * Free a dict item.  Also clears the value.
 */
    ಶೂನ್ಯ
dictitem_free(dictitem_T *ಅಂಶ)
{
    clear_tv(&ಅಂಶ->di_tv);
    ಆದರೆ (ಅಂಶ->di_flags & DI_FLAGS_ALLOC)
	vim_free(ಅಂಶ);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, ಪೂರ್ಣಾಂಕ deep, ಪೂರ್ಣಾಂಕ ತುದಿ, ಪೂರ್ಣಾಂಕ copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    ಪೂರ್ಣಾಂಕ		todo;
    hashitem_T	*hi;

    ಆದರೆ (orig == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    copy = dict_alloc();
    ಆದರೆ (copy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    ಆದರೆ (orig->dv_type == ಖಾಲಿ || ತುದಿ || deep)
	copy->dv_type = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (ಪೂರ್ಣಾಂಕ)orig->dv_hashtab.ht_ಬಳಸಿದ;
    ಗಾಗಿ (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_ಕೀಲಿ);
	    ಆದರೆ (di == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (deep)
	    {
		ಆದರೆ (ಅಂಶ_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    ಆದರೆ (dict_ಕೂಡಿಸು(copy, di) == FAIL)
	    {
		dictitem_free(di);
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ++copy->dv_refcount;
    ಆದರೆ (todo > 0)
    {
	dict_unref(copy);
	copy = ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    ಪೂರ್ಣಾಂಕ
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    ಹಿಂತಿರುಗಿ (d == get_globvar_dict()
		|| (ಒಳ_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid)
		   && d == &SCRIPT_ಅಂಶ(current_sctx.sc_sid)->sn_vars->sv_dict)
		|| &d->dv_hashtab == get_funccal_local_ht())
	    && (tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
	    && var_wrong_func_name(name, TRUE);
}

/*
 * Add item "item" to Dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು(dict_T *d, dictitem_T *ಅಂಶ)
{
    ಆದರೆ (dict_wrong_func_name(d, &ಅಂಶ->di_tv, ಅಂಶ->di_ಕೀಲಿ))
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ hash_ಕೂಡಿಸು(&d->dv_hashtab, ಅಂಶ->di_ಕೀಲಿ, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ_special(
    dict_T	*d,
    ಅಕ್ಷರ	*ಕೀಲಿ,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*ಅಂಶ;

    ಅಂಶ = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಅಂಶ->di_tv.v_type = vartype;
    ಅಂಶ->di_tv.vval.v_ಸಂಖ್ಯೆ = nr;
    ಆದರೆ (dict_ಕೂಡಿಸು(d, ಅಂಶ) == FAIL)
    {
	dictitem_free(ಅಂಶ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, varnumber_T nr)
{
    ಹಿಂತಿರುಗಿ dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ_special(d, ಕೀಲಿ, nr, VAR_ಸಂಖ್ಯೆ);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು_bool(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, varnumber_T nr)
{
    ಹಿಂತಿರುಗಿ dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ_special(d, ಕೀಲಿ, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು_string(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, char_u *str)
{
    ಹಿಂತಿರುಗಿ dict_ಕೂಡಿಸು_string_len(d, ಕೀಲಿ, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು_string_len(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, char_u *str, ಪೂರ್ಣಾಂಕ len)
{
    dictitem_T	*ಅಂಶ;
    char_u	*val = ಖಾಲಿ;

    ಅಂಶ = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಅಂಶ->di_tv.v_type = VAR_STRING;
    ಆದರೆ (str != ಖಾಲಿ)
    {
	ಆದರೆ (len == -1)
	    val = vim_strsave(str);
	ಇಲ್ಲದಿದ್ದರೆ
	    val = vim_strnsave(str, len);
    }
    ಅಂಶ->di_tv.vval.v_string = val;
    ಆದರೆ (dict_ಕೂಡಿಸು(d, ಅಂಶ) == FAIL)
    {
	dictitem_free(ಅಂಶ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು_list(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, list_T *list)
{
    dictitem_T	*ಅಂಶ;

    ಅಂಶ = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಅಂಶ->di_tv.v_type = VAR_LIST;
    ಅಂಶ->di_tv.vval.v_list = list;
    ಆದರೆ (dict_ಕೂಡಿಸು(d, ಅಂಶ) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	ಅಂಶ->di_tv.vval.v_list = ಖಾಲಿ;
	dictitem_free(ಅಂಶ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ++list->lv_refcount;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು_tv(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, typval_T *tv)
{
    dictitem_T	*ಅಂಶ;

    ಅಂಶ = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    copy_tv(tv, &ಅಂಶ->di_tv);
    ಆದರೆ (dict_ಕೂಡಿಸು(d, ಅಂಶ) == FAIL)
    {
	dictitem_free(ಅಂಶ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು_callback(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, callback_T *cb)
{
    dictitem_T	*ಅಂಶ;

    ಅಂಶ = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    put_callback(cb, &ಅಂಶ->di_tv);
    ಆದರೆ (dict_ಕೂಡಿಸು(d, ಅಂಶ) == FAIL)
    {
	dictitem_free(ಅಂಶ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು_func(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, ufunc_T *fp)
{
    dictitem_T	*ಅಂಶ;

    ಅಂಶ = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಅಂಶ->di_tv.v_type = VAR_FUNC;
    ಅಂಶ->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(ಅಂಶ->di_tv.vval.v_string);
    ಆದರೆ (dict_ಕೂಡಿಸು(d, ಅಂಶ) == FAIL)
    {
	dictitem_free(ಅಂಶ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    ಶೂನ್ಯ
dict_iterate_ಆರಂಭ(typval_T *var, dict_iterator_T *iter)
{
    ಆದರೆ (var->v_type != VAR_DICT || var->vval.v_dict == ಖಾಲಿ)
	iter->dit_todo = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	dict_T	*d = var->vval.v_dict;

	iter->dit_todo = d->dv_hashtab.ht_ಬಳಸಿದ;
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
dict_iterate_ಮುಂದಿನ(dict_iterator_T *iter, typval_T **tv_ಫಲಿತಾಂಶ)
{
    dictitem_T	*di;
    char_u      *ಫಲಿತಾಂಶ;

    ಆದರೆ (iter->dit_todo == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆಗ (HASHITEM_ಖಾಲಿಗಳು(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    ಫಲಿತಾಂಶ = di->di_ಕೀಲಿ;
    *tv_ಫಲಿತಾಂಶ = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ಪೂರ್ಣಾಂಕ
dict_ಕೂಡಿಸು_dict(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, dict_T *dict)
{
    dictitem_T	*ಅಂಶ;

    ಅಂಶ = dictitem_alloc((char_u *)ಕೀಲಿ);
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಅಂಶ->di_tv.v_type = VAR_DICT;
    ಅಂಶ->di_tv.vval.v_dict = dict;
    ಆದರೆ (dict_ಕೂಡಿಸು(d, ಅಂಶ) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	ಅಂಶ->di_tv.vval.v_dict = ಖಾಲಿ;
	dictitem_free(ಅಂಶ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ++dict->dv_refcount;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    ದೀರ್ಘ
dict_len(dict_T *d)
{
    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0L;
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)d->dv_hashtab.ht_ಬಳಸಿದ;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_ಕಂಡುಹಿಡಿ(dict_T *d, char_u *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ len)
{
#ವ್ಯಾಖ್ಯಾನಿಸು AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = ಖಾಲಿ;
    hashitem_T	*hi;

    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (len < 0)
	akey = ಕೀಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(ಕೀಲಿ, len);
	ಆದರೆ (akey == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, ಕೀಲಿ, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_ಕಂಡುಹಿಡಿ(&d->dv_hashtab, akey);
    vim_free(tofree);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    ಪೂರ್ಣಾಂಕ
dict_has_ಕೀಲಿ(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ)
{
    ಹಿಂತಿರುಗಿ dict_ಕಂಡುಹಿಡಿ(d, (char_u *)ಕೀಲಿ, -1) != ಖಾಲಿ;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    ಪೂರ್ಣಾಂಕ
dict_get_tv(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)ಕೀಲಿ, -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    copy_tv(&di->di_tv, rettv);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)ಕೀಲಿ, -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    s = tv_get_string(&di->di_tv);
    ಆದರೆ (save && s != ಖಾಲಿ)
	s = vim_strsave(s);
    ಹಿಂತಿರುಗಿ s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_ಸಂಖ್ಯೆ(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ)
{
    ಹಿಂತಿರುಗಿ dict_get_ಸಂಖ್ಯೆ_def(d, ಕೀಲಿ, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_ಸಂಖ್ಯೆ_def(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ def)
{
    dictitem_T	*di;

    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)ಕೀಲಿ, -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ def;
    ಹಿಂತಿರುಗಿ tv_get_ಸಂಖ್ಯೆ(&di->di_tv);
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

    di = dict_ಕಂಡುಹಿಡಿ(d, ಕೀಲಿ, -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_invalid_ವಾದ_str), tv_get_string(&di->di_tv));
	ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ tv_get_ಸಂಖ್ಯೆ(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, ಅಕ್ಷರ *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ def)
{
    dictitem_T	*di;

    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)ಕೀಲಿ, -1);
    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ def;
    ಹಿಂತಿರುಗಿ tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, ಪೂರ್ಣಾಂಕ copyID, ಪೂರ್ಣಾಂಕ restore_copyID)
{
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    ಪೂರ್ಣಾಂಕ		todo;
    ಪೂರ್ಣಾಂಕ		prev_ಬೀಗ;

    ಆದರೆ ((d = tv->vval.v_dict) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_ಬೀಗ = d->dv_ಬೀಗ;
    ಆದರೆ (d->dv_ಬೀಗ == 0)
	d->dv_ಬೀಗ = VAR_LOCKED;
    hash_ಬೀಗ(&d->dv_hashtab);

    todo = (ಪೂರ್ಣಾಂಕ)d->dv_hashtab.ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;

	    ಆದರೆ (ಮೊದಲ)
		ಮೊದಲ = FALSE;
	    ಇಲ್ಲದಿದ್ದರೆ
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_ಕೀಲಿ, FALSE);
	    ಆದರೆ (tofree != ಖಾಲಿ)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    ಆದರೆ (s != ಖಾಲಿ)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    ಆದರೆ (s == ಖಾಲಿ || did_echo_string_emsg)
		ನಿಲ್ಲಿಸು;
	    ಸಾಲು_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_ಬೀಗ = prev_ಬೀಗ;
    ಆದರೆ (todo > 0)
    {
	vim_free(ga.ga_ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    ಹಿಂತಿರುಗಿ (char_u *)ga.ga_ದತ್ತಾಂಶ;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    ಸ್ಥಿರ char_u *
skip_literal_ಕೀಲಿ(char_u *ಕೀಲಿ)
{
    char_u *p;

    ಗಾಗಿ (p = ಕೀಲಿ; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_literal_ಕೀಲಿ_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_ಕೀಲಿ(*arg);

    ಆದರೆ (p == *arg)
	ಹಿಂತಿರುಗಿ FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    ಹಿಂತಿರುಗಿ OK;
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
    char_u	*ಕೊನೆ;
    typval_T	rettv;

    ಆದರೆ (**arg == '\'')
    {
	ಆದರೆ (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಕೀಲಿ = rettv.vval.v_string;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (**arg == '"')
    {
	ಆದರೆ (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಕೀಲಿ = rettv.vval.v_string;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಕೊನೆ = skip_literal_ಕೀಲಿ(*arg);
	ಆದರೆ (ಕೊನೆ == *arg)
	{
	    semsg(_(e_invalid_ಕೀಲಿ_str), *arg);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಕೀಲಿ = vim_strnsave(*arg, ಕೊನೆ - *arg);
	*arg = ಕೊನೆ;
    }
    ಹಿಂತಿರುಗಿ ಕೀಲಿ;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    ಪೂರ್ಣಾಂಕ
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಂಕ literal)
{
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg == ಖಾಲಿ ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = ಖಾಲಿ;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*ಕೀಲಿ = ಖಾಲಿ;
    dictitem_T	*ಅಂಶ;
    char_u	*curly_ಅಭಿವ್ಯಕ್ತಿ = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    ಪೂರ್ಣಾಂಕ		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    ಆದರೆ (!vim9script
	    && *curly_ಅಭಿವ್ಯಕ್ತಿ != '}'
	    && !literal
	    && eval1(&curly_ಅಭಿವ್ಯಕ್ತಿ, &tv, ಖಾಲಿ) == OK
	    && *skipwhite(curly_ಅಭಿವ್ಯಕ್ತಿ) == '}')
	ಹಿಂತಿರುಗಿ NOTDONE;

    ಆದರೆ (evaluate)
    {
	d = dict_alloc();
	ಆದರೆ (d == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ಆಗ (**arg != '}' && **arg != NUL)
    {
	ಪೂರ್ಣಾಂಕ	has_bracket = vim9script && **arg == '[';

	ಆದರೆ (literal)
	{
	    ಆದರೆ (get_literal_ಕೀಲಿ_tv(arg, &tvkey) == FAIL)
		ಹೋಗು failret;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_ಕೀಲಿ(arg);
	    ಆದರೆ (tvkey.vval.v_string == ಖಾಲಿ)
		ಹೋಗು failret;
	    tvkey.v_type = VAR_STRING;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (has_bracket)
		*arg = skipwhite(*arg + 1);
	    ಆದರೆ (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		ಹೋಗು failret;
	    ಆದರೆ (has_bracket)
	    {
		*arg = skipwhite(*arg);
		ಆದರೆ (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_ಕೀಲಿ));
		    clear_tv(&tvkey);
		    ಹೋಗು failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ಆದರೆ (!vim9script)
	    *arg = skipwhite(*arg);
	ಆದರೆ (**arg != ':')
	{
	    ಆದರೆ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_missing_colon_ಒಳ_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    ಹೋಗು failret;
	}
	ಆದರೆ (evaluate)
	{
	    ಆದರೆ (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    ಕೀಲಿ = tv_get_string_buf_chk(&tvkey, buf);
	    ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		ಹೋಗು failret;
	    }
	}
	ಆದರೆ (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    ಹೋಗು failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ಆದರೆ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    ಹೋಗು failret;
	}
	ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    ಹೋಗು failret;
	}
	ಆದರೆ (evaluate)
	{
	    ಅಂಶ = dict_ಕಂಡುಹಿಡಿ(d, ಕೀಲಿ, -1);
	    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
	    {
		semsg(_(e_duplicate_ಕೀಲಿ_ಒಳ_dictionary_str), ಕೀಲಿ);
		clear_tv(&tvkey);
		clear_tv(&tv);
		ಹೋಗು failret;
	    }
	    ಅಂಶ = dictitem_alloc(ಕೀಲಿ);
	    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
	    {
		ಅಂಶ->di_tv = tv;
		ಅಂಶ->di_tv.v_ಬೀಗ = 0;
		ಆದರೆ (dict_ಕೂಡಿಸು(d, ಅಂಶ) == FAIL)
		    dictitem_free(ಅಂಶ);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ಆದರೆ (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	ಆದರೆ (had_comma)
	{
	    ಆದರೆ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ಹೋಗು failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಆದರೆ (**arg == '}')
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (!had_comma)
	{
	    ಆದರೆ (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_missing_comma_ಒಳ_dictionary_str), *arg);
	    ಹೋಗು failret;
	}
    }

    ಆದರೆ (**arg != '}')
    {
	ಆದರೆ (evalarg != ಖಾಲಿ)
	    semsg(_(e_missing_dict_ಕೊನೆ_str), *arg);
failret:
	ಆದರೆ (d != ಖಾಲಿ)
	    dict_free(d);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    *arg = *arg + 1;
    ಆದರೆ (evaluate)
	rettv_dict_ಗಣ(rettv, d);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    ಪೂರ್ಣಾಂಕ
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    ಪೂರ್ಣಾಂಕ		ret = OK;

    ಆದರೆ (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ret = NOTDONE;

    ಹಿಂತಿರುಗಿ ret;
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
dict_extend(dict_T *d1, dict_T *d2, char_u *action, ಅಕ್ಷರ *func_name)
{
    dictitem_T	*di1;
    ಪೂರ್ಣಾಂಕ		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    ಆದರೆ (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (*action == 'm')
    {
	ಆದರೆ (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    ಹಿಂತಿರುಗಿ;
	hash_ಬೀಗ(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    ಆದರೆ (d1->dv_type != ಖಾಲಿ && d1->dv_type->tt_member != ಖಾಲಿ)
	type = d1->dv_type->tt_member;
    ಇಲ್ಲದಿದ್ದರೆ
	type = ಖಾಲಿ;

    todo = (ಪೂರ್ಣಾಂಕ)d2->dv_hashtab.ht_ಬಳಸಿದ;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d2->dv_hashtab, hi2, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi2))
	{
	    --todo;
	    di1 = dict_ಕಂಡುಹಿಡಿ(d1, hi2->hi_ಕೀಲಿ, -1);
	    // Check the key to be valid when adding to any scope.
	    ಆದರೆ (d1->dv_scope != 0 && !valid_varname(hi2->hi_ಕೀಲಿ, -1, TRUE))
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (type != ಖಾಲಿ
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		ನಿಲ್ಲಿಸು;

	    ಆದರೆ (di1 == ಖಾಲಿ)
	    {
		ಆದರೆ (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    ಆದರೆ (dict_ಕೂಡಿಸು(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    ಆದರೆ (di1 != ಖಾಲಿ && dict_ಕೂಡಿಸು(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*action == 'e')
	    {
		semsg(_(e_ಕೀಲಿ_already_exists_str), hi2->hi_ಕೀಲಿ);
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*action == 'f' && HI2DI(hi2) != di1)
	    {
		ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(di1->di_tv.v_ಬೀಗ, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    ನಿಲ್ಲಿಸು;
		// Disallow replacing a builtin function.
		ಆದರೆ (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_ಕೀಲಿ))
		    ನಿಲ್ಲಿಸು;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    ಆದರೆ (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    ಹಿಂತಿರುಗಿ HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    ಪೂರ್ಣಾಂಕ
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    ಪೂರ್ಣಾಂಕ		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    ಪೂರ್ಣಾಂಕ		todo;

    ಆದರೆ (d1 == d2)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (dict_len(d1) != dict_len(d2))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (d1 == ಖಾಲಿ || d2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    todo = (ಪೂರ್ಣಾಂಕ)d1->dv_hashtab.ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d1->dv_hashtab, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    item2 = dict_ಕಂಡುಹಿಡಿ(d2, hi->hi_ಕೀಲಿ, -1);
	    ಆದರೆ (item2 == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FALSE;
	    ಆದರೆ (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		ಹಿಂತಿರುಗಿ FALSE;
	    --todo;
	}
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    ದೀರ್ಘ
dict_ಎಣಿಕೆ(dict_T *d, typval_T *needle, ಪೂರ್ಣಾಂಕ ic)
{
    ಪೂರ್ಣಾಂಕ		todo;
    hashitem_T	*hi;
    ದೀರ್ಘ	n = 0;

    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    todo = (ಪೂರ್ಣಾಂಕ)d->dv_hashtab.ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    ಆದರೆ (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    ಹಿಂತಿರುಗಿ n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    ಶೂನ್ಯ
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	ಅಕ್ಷರ		*func_name,
	char_u		*arg_errmsg,
	ಪೂರ್ಣಾಂಕ		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    ಪೂರ್ಣಾಂಕ	i;

    d1 = argvars[0].vval.v_dict;
    ಆದರೆ (d1 == ಖಾಲಿ)
    {
	emsg(_(e_cannot_extend_null_dict));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!is_new && ಮೌಲ್ಯ_check_ಬೀಗ(d1->dv_ಬೀಗ, arg_errmsg, TRUE))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	ಆದರೆ (d1 == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }

    d2 = argvars[1].vval.v_dict;
    ಆದರೆ (d2 == ಖಾಲಿ)
	ಹೋಗು theend;

    // Check the third argument.
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	ಸ್ಥಿರ ಅಕ್ಷರ *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	ಆದರೆ (action == ಖಾಲಿ)
	{
	    ಆದರೆ (is_new)
		dict_unref(d1);
	    ಹಿಂತಿರುಗಿ;
	}
	ಗಾಗಿ (i = 0; i < 3; ++i)
	    ಆದರೆ (STRCMP(action, av[i]) == 0)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (i == 3)
	{
	    ಆದರೆ (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_ವಾದ_str), action);
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	action = (char_u *)"force";

    ಆದರೆ (type != ಖಾಲಿ && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	ಆದರೆ (is_new)
	    dict_unref(d1);
	ಹಿಂತಿರುಗಿ;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    ಆದರೆ (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_ಬೀಗ = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
	ಅಕ್ಷರ		*func_name,
	char_u		*arg_errmsg,
	typval_T	*ಅಭಿವ್ಯಕ್ತಿ,
	typval_T	*rettv)
{
    dict_T	*d_ret = ಖಾಲಿ;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    ಪೂರ್ಣಾಂಕ		todo;
    ಪೂರ್ಣಾಂಕ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ಆದರೆ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = ಖಾಲಿ;
    }
    ಆದರೆ (d == ಖಾಲಿ
	  || (filtermap == FILTERMAP_FILTER
			&& ಮೌಲ್ಯ_check_ಬೀಗ(d->dv_ಬೀಗ, arg_errmsg, TRUE)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (filtermap == FILTERMAP_MAPNEW)
    {
	ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, &newtv);

    ಪೂರ್ಣಾಂಕ prev_ಬೀಗ = d->dv_ಬೀಗ;
    ಆದರೆ (d->dv_ಬೀಗ == 0)
	d->dv_ಬೀಗ = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_ಬೀಗ(ht);
    todo = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    ಪೂರ್ಣಾಂಕ		r;

	    --todo;
	    di = HI2DI(hi);
	    ಆದರೆ (filtermap == FILTERMAP_MAP
		    && (ಮೌಲ್ಯ_check_ಬೀಗ(di->di_tv.v_ಬೀಗ,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		ನಿಲ್ಲಿಸು;
	    ಗಣ_vim_var_string(VV_ಕೀಲಿ, di->di_ಕೀಲಿ, -1);
	    r = filter_map_one(&di->di_tv, ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_ಕೀಲಿ));
	    ಆದರೆ (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (filtermap == FILTERMAP_MAP)
	    {
		ಆದರೆ (argtype != ಖಾಲಿ && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ನಿಲ್ಲಿಸು;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_ಬೀಗ = 0;
		di->di_tv = newtv;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_ಕೂಡಿಸು_tv(d_ret, (ಅಕ್ಷರ *)di->di_ಕೀಲಿ, &newtv);
		clear_tv(&newtv);
		ಆದರೆ (r == FAIL)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		ಆದರೆ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    ನಿಲ್ಲಿಸು;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_ಬೀಗ = prev_ಬೀಗ;
    ಆದರೆ (fc != ಖಾಲಿ)
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

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	ಹಿಂತಿರುಗಿ;
    }

    d = argvars[0].vval.v_dict;
    ಆದರೆ (d == ಖಾಲಿ || ಮೌಲ್ಯ_check_ಬೀಗ(d->dv_ಬೀಗ, arg_errmsg, TRUE))
	ಹಿಂತಿರುಗಿ;

    ಕೀಲಿ = tv_get_string_chk(&argvars[1]);
    ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    di = dict_ಕಂಡುಹಿಡಿ(d, ಕೀಲಿ, -1);
    ಆದರೆ (di == ಖಾಲಿ)
    {
	semsg(_(e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str), ಕೀಲಿ);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	ಹಿಂತಿರುಗಿ;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
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
    ಪೂರ್ಣಾಂಕ		todo;

    ಆದರೆ (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(ಶೂನ್ಯ)rettv_list_alloc(rettv);
	ಹಿಂತಿರುಗಿ;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == ಖಾಲಿ ? 0 : (ಪೂರ್ಣಾಂಕ)d->dv_hashtab.ht_ಬಳಸಿದ;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_ವಸ್ತುಗಳು(todo);
    ಆದರೆ (l == ಖಾಲಿ)
    {
	(ಶೂನ್ಯ)rettv_list_alloc(rettv);
	ಹಿಂತಿರುಗಿ;
    }
    rettv_list_ಗಣ(rettv, l);
    ಆದರೆ (todo == 0)
	// NULL or empty dict, return the empty list
	ಹಿಂತಿರುಗಿ;

    li = l->lv_ಮೊದಲ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&d->dv_hashtab, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    ಆದರೆ (what == DICT2_LIST_ಕೀಲಿಗಳು)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_ಬೀಗ = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_ಕೀಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == DICT2_LIST_ಮೌಲ್ಯಗಳು)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// items()
		l2 = list_alloc_with_ವಸ್ತುಗಳು(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_ಬೀಗ = 0;
		li->li_tv.vval.v_list = l2;
		ಆದರೆ (l2 == ಖಾಲಿ)
		    ಹೋಗು alloc_failed;
		++l2->lv_refcount;

		l2->lv_ಮೊದಲ->li_tv.v_type = VAR_STRING;
		l2->lv_ಮೊದಲ->li_tv.vval.v_string = vim_strsave(di->di_ಕೀಲಿ);
		ಆದರೆ (l2->lv_ಮೊದಲ->li_tv.vval.v_string == ಖಾಲಿ)
		    ಹೋಗು alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_ಕೊನೆಯ->li_tv);
	    }
	    li = li->li_ಮುಂದಿನ;
	}
    }
    ಹಿಂತಿರುಗಿ;

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
dict_ಗಣ_ವಸ್ತುಗಳು_ro(dict_T *di)
{
    ಪೂರ್ಣಾಂಕ		todo = (ಪೂರ್ಣಾಂಕ)di->dv_hashtab.ht_ಬಳಸಿದ;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&di->dv_hashtab, hi, todo)
    {
	ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	    ಮುಂದುವರಿ;
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
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_for_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].vval.v_dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = dict_has_ಕೀಲಿ(argvars[0].vval.v_dict,
				(ಅಕ್ಷರ *)tv_get_string(&argvars[1]));
}

#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_EVAL)
