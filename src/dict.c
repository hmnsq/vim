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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
ସ୍ଥିର dict_T		*first_dict = ଖାଲି;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(ଶୂନ୍ୟ)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Add the dict to the list of dicts for garbage collection.
    ଯଦି (first_dict != ଖାଲି)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = ଖାଲି;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    ଫେରନ୍ତୁ d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    ଯଦି (alloc_fail_id == id && alloc_does_fail(ଆକାର(list_T)))
	ଫେରନ୍ତୁ ଖାଲି;
#endif
    ଫେରନ୍ତୁ (dict_alloc());
}

    dict_T *
dict_alloc_lock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock)
{
    dict_T *d = dict_alloc();

    ଯଦି (d != ଖାଲି)
	d->dv_lock = lock;
    ଫେରନ୍ତୁ d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    rettv_dict_set(rettv, d);
    ଫେରନ୍ତୁ OK;
}

/*
 * Set a dictionary as the return value
 */
    ଶୂନ୍ୟ
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    ଯଦି (d != ଖାଲି)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    ଶୂନ୍ୟ
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = ଖାଲି;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    ଶୂନ୍ୟ
hashtab_free_contents(hashtab_T *ht)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    ଯଦି (check_hashtab_frozen(ht, "clear dict"))
	ଫେରନ୍ତୁ;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
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

    ସ୍ଥିର ଶୂନ୍ୟ
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    ଯଦି (d->dv_used_prev == ଖାଲି)
	first_dict = d->dv_used_next;
    ଅନ୍ୟଥା
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    ଯଦି (d->dv_used_next != ଖାଲି)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    ସ୍ଥିର ଶୂନ୍ୟ
dict_free(dict_T *d)
{
    ଯଦି (!in_free_unref_ବସ୍ତୁଗୁଡ଼ିକ)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    ଶୂନ୍ୟ
dict_unref(dict_T *d)
{
    ଯଦି (d != ଖାଲି && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_free_nonref(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    dict_T	*dd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_free = FALSE;

    ପାଇଁ (dd = first_dict; dd != ଖାଲି; dd = dd->dv_used_next)
	ଯଦି ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    ଫେରନ୍ତୁ did_free;
}

    ଶୂନ୍ୟ
dict_free_ବସ୍ତୁଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    dict_T	*dd, *dd_next;

    ପାଇଁ (dd = first_dict; dd != ଖାଲି; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	ଯଦି ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
dictitem_alloc(char_u *ଚାବି)
{
    dictitem_T *di;
    size_t len = STRLEN(ଚାବି);

    di = alloc(offsetof(dictitem_T, di_ଚାବି) + len + 1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    mch_memmove(di->di_ଚାବି, ଚାବି, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    ଫେରନ୍ତୁ di;
}

/*
 * Make a copy of a Dictionary item.
 */
    ସ୍ଥିର dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_ଚାବି);

    di = alloc(offsetof(dictitem_T, di_ଚାବି) + len + 1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    mch_memmove(di->di_ଚାବି, org->di_ଚାବି, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    ଫେରନ୍ତୁ di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    ଶୂନ୍ୟ
dictitem_remove(dict_T *dict, dictitem_T *item, ଅକ୍ଷର *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_ଚାବି);
    ଯଦି (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    ଅନ୍ୟଥା
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(item);
}

/*
 * Free a dict item.  Also clears the value.
 */
    ଶୂନ୍ୟ
dictitem_free(dictitem_T *item)
{
    clear_tv(&item->di_tv);
    ଯଦି (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା deep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    hashitem_T	*hi;

    ଯଦି (orig == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    copy = dict_alloc();
    ଯଦି (copy == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    ଯଦି (orig->dv_type == ଖାଲି || ଶୀର୍ଷ || deep)
	copy->dv_type = ଖାଲି;
    ଅନ୍ୟଥା
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)orig->dv_hashtab.ht_used;
    ପାଇଁ (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_ଚାବି);
	    ଯଦି (di == ଖାଲି)
		ଭାଙ୍ଗ;
	    ଯଦି (deep)
	    {
		ଯଦି (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    ଭାଙ୍ଗ;
		}
	    }
	    ଅନ୍ୟଥା
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    ଯଦି (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		ଭାଙ୍ଗ;
	    }
	}
    }

    ++copy->dv_refcount;
    ଯଦି (todo > 0)
    {
	dict_unref(copy);
	copy = ଖାଲି;
    }

    ଫେରନ୍ତୁ copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    ଫେରନ୍ତୁ (d == get_globvar_dict()
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add(dict_T *d, dictitem_T *item)
{
    ଯଦି (dict_wrong_func_name(d, &item->di_tv, item->di_ଚାବି))
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ hash_add(&d->dv_hashtab, item->di_ଚାବି, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add_ସଂଖ୍ୟା_special(
    dict_T	*d,
    ଅକ୍ଷର	*ଚାବି,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ଚାବି);
    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_ସଂଖ୍ୟା = nr;
    ଯଦି (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add_ସଂଖ୍ୟା(dict_T *d, ଅକ୍ଷର *ଚାବି, varnumber_T nr)
{
    ଫେରନ୍ତୁ dict_add_ସଂଖ୍ୟା_special(d, ଚାବି, nr, VAR_ସଂଖ୍ୟା);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add_bool(dict_T *d, ଅକ୍ଷର *ଚାବି, varnumber_T nr)
{
    ଫେରନ୍ତୁ dict_add_ସଂଖ୍ୟା_special(d, ଚାବି, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add_string(dict_T *d, ଅକ୍ଷର *ଚାବି, char_u *str)
{
    ଫେରନ୍ତୁ dict_add_string_len(d, ଚାବି, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add_string_len(dict_T *d, ଅକ୍ଷର *ଚାବି, char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    dictitem_T	*item;
    char_u	*val = ଖାଲି;

    item = dictitem_alloc((char_u *)ଚାବି);
    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    item->di_tv.v_type = VAR_STRING;
    ଯଦି (str != ଖାଲି)
    {
	ଯଦି (len == -1)
	    val = vim_strsave(str);
	ଅନ୍ୟଥା
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    ଯଦି (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add_list(dict_T *d, ଅକ୍ଷର *ଚାବି, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ଚାବି);
    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    ଯଦି (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = ଖାଲି;
	dictitem_free(item);
	ଫେରନ୍ତୁ FAIL;
    }
    ++list->lv_refcount;
    ଫେରନ୍ତୁ OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add_tv(dict_T *d, ଅକ୍ଷର *ଚାବି, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ଚାବି);
    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    copy_tv(tv, &item->di_tv);
    ଯଦି (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add_callback(dict_T *d, ଅକ୍ଷର *ଚାବି, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ଚାବି);
    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    put_callback(cb, &item->di_tv);
    ଯଦି (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add_func(dict_T *d, ଅକ୍ଷର *ଚାବି, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ଚାବି);
    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    ଯଦି (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    ଶୂନ୍ୟ
dict_iterate_start(typval_T *var, dict_iterator_T *iter)
{
    ଯଦି (var->v_type != VAR_DICT || var->vval.v_dict == ଖାଲି)
	iter->dit_todo = 0;
    ଅନ୍ୟଥା
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

    ଯଦି (iter->dit_todo == 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯେତେବେଳେ (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_ଚାବି;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    ଫେରନ୍ତୁ result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_add_dict(dict_T *d, ଅକ୍ଷର *ଚାବି, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ଚାବି);
    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    ଯଦି (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = ଖାଲି;
	dictitem_free(item);
	ଫେରନ୍ତୁ FAIL;
    }
    ++dict->dv_refcount;
    ଫେରନ୍ତୁ OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ 0L;
    ଫେରନ୍ତୁ (long)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = ଖାଲି;
    hashitem_T	*hi;

    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (len < 0)
	akey = ଚାବି;
    ଅନ୍ୟଥା ଯଦି (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(ଚାବି, len);
	ଯଦି (akey == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, ଚାବି, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    ଯଦି (HASHITEM_EMPTY(hi))
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_has_ଚାବି(dict_T *d, ଅକ୍ଷର *ଚାବି)
{
    ଫେରନ୍ତୁ dict_find(d, (char_u *)ଚାବି, -1) != ଖାଲି;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_get_tv(dict_T *d, ଅକ୍ଷର *ଚାବି, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)ଚାବି, -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    copy_tv(&di->di_tv, rettv);
    ଫେରନ୍ତୁ OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, ଅକ୍ଷର *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)ଚାବି, -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    s = tv_get_string(&di->di_tv);
    ଯଦି (save && s != ଖାଲି)
	s = vim_strsave(s);
    ଫେରନ୍ତୁ s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_ସଂଖ୍ୟା(dict_T *d, ଅକ୍ଷର *ଚାବି)
{
    ଫେରନ୍ତୁ dict_get_ସଂଖ୍ୟା_def(d, ଚାବି, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_ସଂଖ୍ୟା_def(dict_T *d, ଅକ୍ଷର *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)ଚାବି, -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ def;
    ଫେରନ୍ତୁ tv_get_ସଂଖ୍ୟା(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_ସଂଖ୍ୟା_check(dict_T *d, char_u *ଚାବି)
{
    dictitem_T	*di;

    di = dict_find(d, ଚାବି, -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    ଯଦି (di->di_tv.v_type != VAR_ସଂଖ୍ୟା)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ tv_get_ସଂଖ୍ୟା(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, ଅକ୍ଷର *ଚାବି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)ଚାବି, -1);
    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ def;
    ଫେରନ୍ତୁ tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା restore_copyID)
{
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_lock;

    ଯଦି ((d = tv->vval.v_dict) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    ଯଦି (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&d->dv_hashtab, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    ଯଦି (first)
		first = FALSE;
	    ଅନ୍ୟଥା
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_ଚାବି, FALSE);
	    ଯଦି (tofree != ଖାଲି)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    ଯଦି (s != ଖାଲି)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    ଯଦି (s == ଖାଲି || did_echo_string_emsg)
		ଭାଙ୍ଗ;
	    ଧାଡ଼ି_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    ଯଦି (todo > 0)
    {
	vim_free(ga.ga_data);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    ଫେରନ୍ତୁ (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    ସ୍ଥିର char_u *
skip_literal_ଚାବି(char_u *ଚାବି)
{
    char_u *p;

    ପାଇଁ (p = ଚାବି; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    ଫେରନ୍ତୁ p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_literal_ଚାବି_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_ଚାବି(*arg);

    ଯଦି (p == *arg)
	ଫେରନ୍ତୁ FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    ଫେରନ୍ତୁ OK;
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
get_literal_ଚାବି(char_u **arg)
{
    char_u	*ଚାବି;
    char_u	*end;
    typval_T	rettv;

    ଯଦି (**arg == '\'')
    {
	ଯଦି (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଚାବି = rettv.vval.v_string;
    }
    ଅନ୍ୟଥା ଯଦି (**arg == '"')
    {
	ଯଦି (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଚାବି = rettv.vval.v_string;
    }
    ଅନ୍ୟଥା
    {
	end = skip_literal_ଚାବି(*arg);
	ଯଦି (end == *arg)
	{
	    semsg(_(e_invalid_ଚାବି_str), *arg);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	ଚାବି = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    ଫେରନ୍ତୁ ଚାବି;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା literal)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg == ଖାଲି ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = ଖାଲି;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*ଚାବି = ଖାଲି;
    dictitem_T	*item;
    char_u	*curly_ପଦସମଷ୍ଟି = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    ଯଦି (!vim9script
	    && *curly_ପଦସମଷ୍ଟି != '}'
	    && !literal
	    && eval1(&curly_ପଦସମଷ୍ଟି, &tv, ଖାଲି) == OK
	    && *skipwhite(curly_ପଦସମଷ୍ଟି) == '}')
	ଫେରନ୍ତୁ NOTDONE;

    ଯଦି (evaluate)
    {
	d = dict_alloc();
	ଯଦି (d == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ଯେତେବେଳେ (**arg != '}' && **arg != NUL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_bracket = vim9script && **arg == '[';

	ଯଦି (literal)
	{
	    ଯଦି (get_literal_ଚାବି_tv(arg, &tvkey) == FAIL)
		ଯାଅ failret;
	}
	ଅନ୍ୟଥା ଯଦି (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_ଚାବି(arg);
	    ଯଦି (tvkey.vval.v_string == ଖାଲି)
		ଯାଅ failret;
	    tvkey.v_type = VAR_STRING;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (has_bracket)
		*arg = skipwhite(*arg + 1);
	    ଯଦି (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		ଯାଅ failret;
	    ଯଦି (has_bracket)
	    {
		*arg = skipwhite(*arg);
		ଯଦି (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_ଚାବି));
		    clear_tv(&tvkey);
		    ଯାଅ failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ଯଦି (!vim9script)
	    *arg = skipwhite(*arg);
	ଯଦି (**arg != ':')
	{
	    ଯଦି (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ଅନ୍ୟଥା
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    ଯାଅ failret;
	}
	ଯଦି (evaluate)
	{
	    ଯଦି (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    ଚାବି = tv_get_string_buf_chk(&tvkey, buf);
	    ଯଦି (ଚାବି == ଖାଲି)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		ଯାଅ failret;
	    }
	}
	ଯଦି (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    ଯାଅ failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	ଯଦି (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    ଯାଅ failret;
	}
	ଯଦି (check_typval_is_ମୂଲ୍ୟ(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    ଯାଅ failret;
	}
	ଯଦି (evaluate)
	{
	    item = dict_find(d, ଚାବି, -1);
	    ଯଦି (item != ଖାଲି)
	    {
		semsg(_(e_duplicate_ଚାବି_in_dictionary_str), ଚାବି);
		clear_tv(&tvkey);
		clear_tv(&tv);
		ଯାଅ failret;
	    }
	    item = dictitem_alloc(ଚାବି);
	    ଯଦି (item != ଖାଲି)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		ଯଦି (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	ଯଦି (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	ଯଦି (had_comma)
	{
	    ଯଦି (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ଯାଅ failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ଯଦି (**arg == '}')
	    ଭାଙ୍ଗ;
	ଯଦି (!had_comma)
	{
	    ଯଦି (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    ଅନ୍ୟଥା
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    ଯାଅ failret;
	}
    }

    ଯଦି (**arg != '}')
    {
	ଯଦି (evalarg != ଖାଲି)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	ଯଦି (d != ଖାଲି)
	    dict_free(d);
	ଫେରନ୍ତୁ FAIL;
    }

    *arg = *arg + 1;
    ଯଦି (evaluate)
	rettv_dict_set(rettv, d);

    ଫେରନ୍ତୁ OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;

    ଯଦି (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    ଅନ୍ୟଥା ଯଦି ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    ଅନ୍ୟଥା
	ret = NOTDONE;

    ଫେରନ୍ତୁ ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    ଶୂନ୍ୟ
dict_extend(dict_T *d1, dict_T *d2, char_u *action, ଅକ୍ଷର *func_name)
{
    dictitem_T	*di1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    ଯଦି (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	ଫେରନ୍ତୁ;

    ଯଦି (*action == 'm')
    {
	ଯଦି (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    ଫେରନ୍ତୁ;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    ଯଦି (d1->dv_type != ଖାଲି && d1->dv_type->tt_member != ଖାଲି)
	type = d1->dv_type->tt_member;
    ଅନ୍ୟଥା
	type = ଖାଲି;

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&d2->dv_hashtab, hi2, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_ଚାବି, -1);
	    // Check the key to be valid when adding to any scope.
	    ଯଦି (d1->dv_scope != 0 && !valid_varname(hi2->hi_ଚାବି, -1, TRUE))
		ଭାଙ୍ଗ;

	    ଯଦି (type != ଖାଲି
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		ଭାଙ୍ଗ;

	    ଯଦି (di1 == ଖାଲି)
	    {
		ଯଦି (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    ଯଦି (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		ଅନ୍ୟଥା
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    ଯଦି (di1 != ଖାଲି && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (*action == 'e')
	    {
		semsg(_(e_ଚାବି_already_exists_str), hi2->hi_ଚାବି);
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (*action == 'f' && HI2DI(hi2) != di1)
	    {
		ଯଦି (ମୂଲ୍ୟ_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    ଭାଙ୍ଗ;
		// Disallow replacing a builtin function.
		ଯଦି (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_ଚାବି))
		    ଭାଙ୍ଗ;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    ଯଦି (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    ଫେରନ୍ତୁ HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;

    ଯଦି (d1 == d2)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (dict_len(d1) != dict_len(d2))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (d1 == ଖାଲି || d2 == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&d1->dv_hashtab, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_ଚାବି, -1);
	    ଯଦି (item2 == ଖାଲି)
		ଫେରନ୍ତୁ FALSE;
	    ଯଦି (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		ଫେରନ୍ତୁ FALSE;
	    --todo;
	}
    }
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    long
dict_ଗଣନା(dict_T *d, typval_T *needle, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ic)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    hashitem_T	*hi;
    long	n = 0;

    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&d->dv_hashtab, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ଯଦି (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    ଫେରନ୍ତୁ n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    ଶୂନ୍ୟ
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	ଅକ୍ଷର		*func_name,
	char_u		*arg_errmsg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

    d1 = argvars[0].vval.v_dict;
    ଯଦି (d1 == ଖାଲି)
    {
	emsg(_(e_cannot_extend_null_dict));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (!is_new && ମୂଲ୍ୟ_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	ଫେରନ୍ତୁ;

    ଯଦି (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	ଯଦି (d1 == ଖାଲି)
	    ଫେରନ୍ତୁ;
    }

    d2 = argvars[1].vval.v_dict;
    ଯଦି (d2 == ଖାଲି)
	ଯାଅ theend;

    // Check the third argument.
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	ସ୍ଥିର ଅକ୍ଷର *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	ଯଦି (action == ଖାଲି)
	{
	    ଯଦି (is_new)
		dict_unref(d1);
	    ଫେରନ୍ତୁ;
	}
	ପାଇଁ (i = 0; i < 3; ++i)
	    ଯଦି (STRCMP(action, av[i]) == 0)
		ଭାଙ୍ଗ;
	ଯଦି (i == 3)
	{
	    ଯଦି (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା
	action = (char_u *)"force";

    ଯଦି (type != ଖାଲି && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	ଯଦି (is_new)
	    dict_unref(d1);
	ଫେରନ୍ତୁ;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    ଯଦି (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    ଅନ୍ୟଥା
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    ଶୂନ୍ୟ
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	ଅକ୍ଷର		*func_name,
	char_u		*arg_errmsg,
	typval_T	*ପଦସମଷ୍ଟି,
	typval_T	*rettv)
{
    dict_T	*d_ret = ଖାଲି;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rem;
    typval_T	newtv;
    funccall_T	*fc;

    ଯଦି (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = ଖାଲି;
    }
    ଯଦି (d == ଖାଲି
	  || (filtermap == FILTERMAP_FILTER
			&& ମୂଲ୍ୟ_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	ଫେରନ୍ତୁ;

    ଯଦି (filtermap == FILTERMAP_MAPNEW)
    {
	ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	    ଫେରନ୍ତୁ;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ପଦସମଷ୍ଟି_get_funccal(ପଦସମଷ୍ଟି, &newtv);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_lock = d->dv_lock;
    ଯଦି (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ht->ht_used;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(ht, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

	    --todo;
	    di = HI2DI(hi);
	    ଯଦି (filtermap == FILTERMAP_MAP
		    && (ମୂଲ୍ୟ_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		ଭାଙ୍ଗ;
	    set_vim_var_string(VV_ଚାବି, di->di_ଚାବି, -1);
	    r = filter_map_one(&di->di_tv, ପଦସମଷ୍ଟି, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_ଚାବି));
	    ଯଦି (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (filtermap == FILTERMAP_MAP)
	    {
		ଯଦି (argtype != ଖାଲି && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ଭାଙ୍ଗ;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    ଅନ୍ୟଥା ଯଦି (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (ଅକ୍ଷର *)di->di_ଚାବି, &newtv);
		clear_tv(&newtv);
		ଯଦି (r == FAIL)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		ଯଦି (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    ଭାଙ୍ଗ;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    ଯଦି (fc != ଖାଲି)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    ଶୂନ୍ୟ
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*ଚାବି;
    dictitem_T	*di;

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	ଫେରନ୍ତୁ;
    }

    d = argvars[0].vval.v_dict;
    ଯଦି (d == ଖାଲି || ମୂଲ୍ୟ_check_lock(d->dv_lock, arg_errmsg, TRUE))
	ଫେରନ୍ତୁ;

    ଚାବି = tv_get_string_chk(&argvars[1]);
    ଯଦି (ଚାବି == ଖାଲି)
	ଫେରନ୍ତୁ;

    di = dict_find(d, ଚାବି, -1);
    ଯଦି (di == ଖାଲି)
    {
	semsg(_(e_ଚାବି_not_present_in_dictionary_str), ଚାବି);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	ଫେରନ୍ତୁ;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    DICT2_LIST_ଚାବିଗୁଡ଼ିକ,
    DICT2_LIST_ମୂଲ୍ୟଗୁଡ଼ିକ,
    DICT2_LIST_ବସ୍ତୁଗୁଡ଼ିକ,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo;

    ଯଦି (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(ଶୂନ୍ୟ)rettv_list_alloc(rettv);
	ଫେରନ୍ତୁ;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == ଖାଲି ? 0 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(todo);
    ଯଦି (l == ଖାଲି)
    {
	(ଶୂନ୍ୟ)rettv_list_alloc(rettv);
	ଫେରନ୍ତୁ;
    }
    rettv_list_set(rettv, l);
    ଯଦି (todo == 0)
	// NULL or empty dict, return the empty list
	ଫେରନ୍ତୁ;

    li = l->lv_first;
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&d->dv_hashtab, hi, todo)
    {
	ଯଦି (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    ଯଦି (what == DICT2_LIST_ଚାବିଗୁଡ଼ିକ)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_ଚାବି);
	    }
	    ଅନ୍ୟଥା ଯଦି (what == DICT2_LIST_ମୂଲ୍ୟଗୁଡ଼ିକ)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    ଅନ୍ୟଥା
	    {
		// items()
		l2 = list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		ଯଦି (l2 == ଖାଲି)
		    ଯାଅ alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_ଚାବି);
		ଯଦି (l2->lv_first->li_tv.vval.v_string == ଖାଲି)
		    ଯାଅ alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    ଫେରନ୍ତୁ;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (ଶୂନ୍ୟ)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    ଶୂନ୍ୟ
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ବସ୍ତୁଗୁଡ଼ିକ);
}

/*
 * "keys()" function
 */
    ଶୂନ୍ୟ
f_ଚାବିଗୁଡ଼ିକ(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ଚାବିଗୁଡ଼ିକ);
}

/*
 * "values(dict)" function
 */
    ଶୂନ୍ୟ
f_ମୂଲ୍ୟଗୁଡ଼ିକ(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ମୂଲ୍ୟଗୁଡ଼ିକ);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    ଶୂନ୍ୟ
dict_set_ବସ୍ତୁଗୁଡ଼ିକ_ro(dict_T *di)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&di->dv_hashtab, hi, todo)
    {
	ଯଦି (HASHITEM_EMPTY(hi))
	    ଜାରି;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    ଶୂନ୍ୟ
f_has_ଚାବି(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (check_for_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].vval.v_dict == ଖାଲି)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = dict_has_ଚାବି(argvars[0].vval.v_dict,
				(ଅକ୍ଷର *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
