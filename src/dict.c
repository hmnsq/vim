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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
સ્થિર dict_T		*first_dict = ખાલી;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(શૂન્ય)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    જો (d == ખાલી)
	પાછા ખાલી;

    // Add the dict to the list of dicts for garbage collection.
    જો (first_dict != ખાલી)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = ખાલી;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    પાછા d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    જો (alloc_fail_id == id && alloc_does_fail(કદ(list_T)))
	પાછા ખાલી;
#endif
    પાછા (dict_alloc());
}

    dict_T *
dict_alloc_lock(પૂર્ણાંક lock)
{
    dict_T *d = dict_alloc();

    જો (d != ખાલી)
	d->dv_lock = lock;
    પાછા d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    જો (d == ખાલી)
	પાછા FAIL;

    rettv_dict_set(rettv, d);
    પાછા OK;
}

/*
 * Set a dictionary as the return value
 */
    શૂન્ય
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    જો (d != ખાલી)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    શૂન્ય
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = ખાલી;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    શૂન્ય
hashtab_free_contents(hashtab_T *ht)
{
    પૂર્ણાંક		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    જો (check_hashtab_frozen(ht, "clear dict"))
	પાછા;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (પૂર્ણાંક)ht->ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
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

    સ્થિર શૂન્ય
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    જો (d->dv_used_prev == ખાલી)
	first_dict = d->dv_used_next;
    નહીંતર
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    જો (d->dv_used_next != ખાલી)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    સ્થિર શૂન્ય
dict_free(dict_T *d)
{
    જો (!in_free_unref_વસ્તુઓ)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    શૂન્ય
dict_unref(dict_T *d)
{
    જો (d != ખાલી && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    પૂર્ણાંક
dict_free_nonref(પૂર્ણાંક copyID)
{
    dict_T	*dd;
    પૂર્ણાંક		did_free = FALSE;

    માટે (dd = first_dict; dd != ખાલી; dd = dd->dv_used_next)
	જો ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    પાછા did_free;
}

    શૂન્ય
dict_free_વસ્તુઓ(પૂર્ણાંક copyID)
{
    dict_T	*dd, *dd_next;

    માટે (dd = first_dict; dd != ખાલી; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	જો ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
dictitem_alloc(char_u *ચાવી)
{
    dictitem_T *di;
    size_t len = STRLEN(ચાવી);

    di = alloc(offsetof(dictitem_T, di_ચાવી) + len + 1);
    જો (di == ખાલી)
	પાછા ખાલી;

    mch_memmove(di->di_ચાવી, ચાવી, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    પાછા di;
}

/*
 * Make a copy of a Dictionary item.
 */
    સ્થિર dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_ચાવી);

    di = alloc(offsetof(dictitem_T, di_ચાવી) + len + 1);
    જો (di == ખાલી)
	પાછા ખાલી;

    mch_memmove(di->di_ચાવી, org->di_ચાવી, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    પાછા di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    શૂન્ય
dictitem_remove(dict_T *dict, dictitem_T *item, અક્ષર *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_ચાવી);
    જો (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    નહીંતર
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(item);
}

/*
 * Free a dict item.  Also clears the value.
 */
    શૂન્ય
dictitem_free(dictitem_T *item)
{
    clear_tv(&item->di_tv);
    જો (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, પૂર્ણાંક deep, પૂર્ણાંક ટોચ, પૂર્ણાંક copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    પૂર્ણાંક		todo;
    hashitem_T	*hi;

    જો (orig == ખાલી)
	પાછા ખાલી;

    copy = dict_alloc();
    જો (copy == ખાલી)
	પાછા ખાલી;

    જો (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    જો (orig->dv_type == ખાલી || ટોચ || deep)
	copy->dv_type = ખાલી;
    નહીંતર
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (પૂર્ણાંક)orig->dv_hashtab.ht_used;
    માટે (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_ચાવી);
	    જો (di == ખાલી)
		તોડો;
	    જો (deep)
	    {
		જો (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    તોડો;
		}
	    }
	    નહીંતર
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    જો (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		તોડો;
	    }
	}
    }

    ++copy->dv_refcount;
    જો (todo > 0)
    {
	dict_unref(copy);
	copy = ખાલી;
    }

    પાછા copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    પૂર્ણાંક
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    પાછા (d == get_globvar_dict()
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
    પૂર્ણાંક
dict_add(dict_T *d, dictitem_T *item)
{
    જો (dict_wrong_func_name(d, &item->di_tv, item->di_ચાવી))
	પાછા FAIL;
    પાછા hash_add(&d->dv_hashtab, item->di_ચાવી, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    સ્થિર પૂર્ણાંક
dict_add_સંખ્યા_special(
    dict_T	*d,
    અક્ષર	*ચાવી,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ચાવી);
    જો (item == ખાલી)
	પાછા FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_સંખ્યા = nr;
    જો (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    પૂર્ણાંક
dict_add_સંખ્યા(dict_T *d, અક્ષર *ચાવી, varnumber_T nr)
{
    પાછા dict_add_સંખ્યા_special(d, ચાવી, nr, VAR_સંખ્યા);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    પૂર્ણાંક
dict_add_bool(dict_T *d, અક્ષર *ચાવી, varnumber_T nr)
{
    પાછા dict_add_સંખ્યા_special(d, ચાવી, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    પૂર્ણાંક
dict_add_string(dict_T *d, અક્ષર *ચાવી, char_u *str)
{
    પાછા dict_add_string_len(d, ચાવી, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    પૂર્ણાંક
dict_add_string_len(dict_T *d, અક્ષર *ચાવી, char_u *str, પૂર્ણાંક len)
{
    dictitem_T	*item;
    char_u	*val = ખાલી;

    item = dictitem_alloc((char_u *)ચાવી);
    જો (item == ખાલી)
	પાછા FAIL;
    item->di_tv.v_type = VAR_STRING;
    જો (str != ખાલી)
    {
	જો (len == -1)
	    val = vim_strsave(str);
	નહીંતર
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    જો (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    પૂર્ણાંક
dict_add_list(dict_T *d, અક્ષર *ચાવી, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ચાવી);
    જો (item == ખાલી)
	પાછા FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    જો (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = ખાલી;
	dictitem_free(item);
	પાછા FAIL;
    }
    ++list->lv_refcount;
    પાછા OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    પૂર્ણાંક
dict_add_tv(dict_T *d, અક્ષર *ચાવી, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ચાવી);
    જો (item == ખાલી)
	પાછા FAIL;
    copy_tv(tv, &item->di_tv);
    જો (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    પૂર્ણાંક
dict_add_callback(dict_T *d, અક્ષર *ચાવી, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ચાવી);
    જો (item == ખાલી)
	પાછા FAIL;
    put_callback(cb, &item->di_tv);
    જો (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    પૂર્ણાંક
dict_add_func(dict_T *d, અક્ષર *ચાવી, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ચાવી);
    જો (item == ખાલી)
	પાછા FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    જો (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    શૂન્ય
dict_iterate_start(typval_T *var, dict_iterator_T *iter)
{
    જો (var->v_type != VAR_DICT || var->vval.v_dict == ખાલી)
	iter->dit_todo = 0;
    નહીંતર
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

    જો (iter->dit_todo == 0)
	પાછા ખાલી;

    જ્યારે (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_ચાવી;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    પાછા result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    પૂર્ણાંક
dict_add_dict(dict_T *d, અક્ષર *ચાવી, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)ચાવી);
    જો (item == ખાલી)
	પાછા FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    જો (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = ખાલી;
	dictitem_free(item);
	પાછા FAIL;
    }
    ++dict->dv_refcount;
    પાછા OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    જો (d == ખાલી)
	પાછા 0L;
    પાછા (long)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *ચાવી, પૂર્ણાંક len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = ખાલી;
    hashitem_T	*hi;

    જો (d == ખાલી)
	પાછા ખાલી;
    જો (len < 0)
	akey = ચાવી;
    નહીંતર જો (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(ચાવી, len);
	જો (akey == ખાલી)
	    પાછા ખાલી;
    }
    નહીંતર
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, ચાવી, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    જો (HASHITEM_EMPTY(hi))
	પાછા ખાલી;
    પાછા HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    પૂર્ણાંક
dict_has_ચાવી(dict_T *d, અક્ષર *ચાવી)
{
    પાછા dict_find(d, (char_u *)ચાવી, -1) != ખાલી;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    પૂર્ણાંક
dict_get_tv(dict_T *d, અક્ષર *ચાવી, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)ચાવી, -1);
    જો (di == ખાલી)
	પાછા FAIL;
    copy_tv(&di->di_tv, rettv);
    પાછા OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, અક્ષર *ચાવી, પૂર્ણાંક save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)ચાવી, -1);
    જો (di == ખાલી)
	પાછા ખાલી;
    s = tv_get_string(&di->di_tv);
    જો (save && s != ખાલી)
	s = vim_strsave(s);
    પાછા s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_સંખ્યા(dict_T *d, અક્ષર *ચાવી)
{
    પાછા dict_get_સંખ્યા_def(d, ચાવી, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_સંખ્યા_def(dict_T *d, અક્ષર *ચાવી, પૂર્ણાંક def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)ચાવી, -1);
    જો (di == ખાલી)
	પાછા def;
    પાછા tv_get_સંખ્યા(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_સંખ્યા_check(dict_T *d, char_u *ચાવી)
{
    dictitem_T	*di;

    di = dict_find(d, ચાવી, -1);
    જો (di == ખાલી)
	પાછા 0;
    જો (di->di_tv.v_type != VAR_સંખ્યા)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	પાછા 0;
    }
    પાછા tv_get_સંખ્યા(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, અક્ષર *ચાવી, પૂર્ણાંક def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)ચાવી, -1);
    જો (di == ખાલી)
	પાછા def;
    પાછા tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, પૂર્ણાંક copyID, પૂર્ણાંક restore_copyID)
{
    garray_T	ga;
    પૂર્ણાંક		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    પૂર્ણાંક		todo;
    પૂર્ણાંક		prev_lock;

    જો ((d = tv->vval.v_dict) == ખાલી)
	પાછા ખાલી;
    ga_init2(&ga, કદ(અક્ષર), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    જો (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (પૂર્ણાંક)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(&d->dv_hashtab, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    જો (first)
		first = FALSE;
	    નહીંતર
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_ચાવી, FALSE);
	    જો (tofree != ખાલી)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    જો (s != ખાલી)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    જો (s == ખાલી || did_echo_string_emsg)
		તોડો;
	    લીટી_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    જો (todo > 0)
    {
	vim_free(ga.ga_data);
	પાછા ખાલી;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    પાછા (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    સ્થિર char_u *
skip_literal_ચાવી(char_u *ચાવી)
{
    char_u *p;

    માટે (p = ચાવી; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    પાછા p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    સ્થિર પૂર્ણાંક
get_literal_ચાવી_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_ચાવી(*arg);

    જો (p == *arg)
	પાછા FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    પાછા OK;
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
get_literal_ચાવી(char_u **arg)
{
    char_u	*ચાવી;
    char_u	*end;
    typval_T	rettv;

    જો (**arg == '\'')
    {
	જો (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    પાછા ખાલી;
	ચાવી = rettv.vval.v_string;
    }
    નહીંતર જો (**arg == '"')
    {
	જો (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    પાછા ખાલી;
	ચાવી = rettv.vval.v_string;
    }
    નહીંતર
    {
	end = skip_literal_ચાવી(*arg);
	જો (end == *arg)
	{
	    semsg(_(e_invalid_ચાવી_str), *arg);
	    પાછા ખાલી;
	}
	ચાવી = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    પાછા ચાવી;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    પૂર્ણાંક
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, પૂર્ણાંક literal)
{
    પૂર્ણાંક		evaluate = evalarg == ખાલી ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = ખાલી;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*ચાવી = ખાલી;
    dictitem_T	*item;
    char_u	*curly_પદાવલિ = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    પૂર્ણાંક		vim9script = in_vim9script();
    પૂર્ણાંક		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    જો (!vim9script
	    && *curly_પદાવલિ != '}'
	    && !literal
	    && eval1(&curly_પદાવલિ, &tv, ખાલી) == OK
	    && *skipwhite(curly_પદાવલિ) == '}')
	પાછા NOTDONE;

    જો (evaluate)
    {
	d = dict_alloc();
	જો (d == ખાલી)
	    પાછા FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    જ્યારે (**arg != '}' && **arg != NUL)
    {
	પૂર્ણાંક	has_bracket = vim9script && **arg == '[';

	જો (literal)
	{
	    જો (get_literal_ચાવી_tv(arg, &tvkey) == FAIL)
		જાઓ failret;
	}
	નહીંતર જો (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_ચાવી(arg);
	    જો (tvkey.vval.v_string == ખાલી)
		જાઓ failret;
	    tvkey.v_type = VAR_STRING;
	}
	નહીંતર
	{
	    જો (has_bracket)
		*arg = skipwhite(*arg + 1);
	    જો (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		જાઓ failret;
	    જો (has_bracket)
	    {
		*arg = skipwhite(*arg);
		જો (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_ચાવી));
		    clear_tv(&tvkey);
		    જાઓ failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	જો (!vim9script)
	    *arg = skipwhite(*arg);
	જો (**arg != ':')
	{
	    જો (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    નહીંતર
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    જાઓ failret;
	}
	જો (evaluate)
	{
	    જો (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    ચાવી = tv_get_string_buf_chk(&tvkey, buf);
	    જો (ચાવી == ખાલી)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		જાઓ failret;
	    }
	}
	જો (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    જાઓ failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	જો (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    જાઓ failret;
	}
	જો (check_typval_is_કિંમત(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    જાઓ failret;
	}
	જો (evaluate)
	{
	    item = dict_find(d, ચાવી, -1);
	    જો (item != ખાલી)
	    {
		semsg(_(e_duplicate_ચાવી_in_dictionary_str), ચાવી);
		clear_tv(&tvkey);
		clear_tv(&tv);
		જાઓ failret;
	    }
	    item = dictitem_alloc(ચાવી);
	    જો (item != ખાલી)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		જો (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	જો (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	જો (had_comma)
	{
	    જો (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		જાઓ failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	જો (**arg == '}')
	    તોડો;
	જો (!had_comma)
	{
	    જો (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    નહીંતર
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    જાઓ failret;
	}
    }

    જો (**arg != '}')
    {
	જો (evalarg != ખાલી)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	જો (d != ખાલી)
	    dict_free(d);
	પાછા FAIL;
    }

    *arg = *arg + 1;
    જો (evaluate)
	rettv_dict_set(rettv, d);

    પાછા OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    પૂર્ણાંક
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    પૂર્ણાંક		vim9script = in_vim9script();
    પૂર્ણાંક		ret = OK;

    જો (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    નહીંતર જો ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    નહીંતર
	ret = NOTDONE;

    પાછા ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    શૂન્ય
dict_extend(dict_T *d1, dict_T *d2, char_u *action, અક્ષર *func_name)
{
    dictitem_T	*di1;
    પૂર્ણાંક		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    જો (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	પાછા;

    જો (*action == 'm')
    {
	જો (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    પાછા;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    જો (d1->dv_type != ખાલી && d1->dv_type->tt_member != ખાલી)
	type = d1->dv_type->tt_member;
    નહીંતર
	type = ખાલી;

    todo = (પૂર્ણાંક)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_વસ્તુઓ(&d2->dv_hashtab, hi2, todo)
    {
	જો (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_ચાવી, -1);
	    // Check the key to be valid when adding to any scope.
	    જો (d1->dv_scope != 0 && !valid_varname(hi2->hi_ચાવી, -1, TRUE))
		તોડો;

	    જો (type != ખાલી
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		તોડો;

	    જો (di1 == ખાલી)
	    {
		જો (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    જો (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		નહીંતર
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    જો (di1 != ખાલી && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    નહીંતર જો (*action == 'e')
	    {
		semsg(_(e_ચાવી_already_exists_str), hi2->hi_ચાવી);
		તોડો;
	    }
	    નહીંતર જો (*action == 'f' && HI2DI(hi2) != di1)
	    {
		જો (કિંમત_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    તોડો;
		// Disallow replacing a builtin function.
		જો (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_ચાવી))
		    તોડો;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    જો (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    પાછા HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    પૂર્ણાંક
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    પૂર્ણાંક		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    પૂર્ણાંક		todo;

    જો (d1 == d2)
	પાછા TRUE;
    જો (dict_len(d1) != dict_len(d2))
	પાછા FALSE;
    જો (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	પાછા TRUE;
    જો (d1 == ખાલી || d2 == ખાલી)
	પાછા FALSE;

    todo = (પૂર્ણાંક)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(&d1->dv_hashtab, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_ચાવી, -1);
	    જો (item2 == ખાલી)
		પાછા FALSE;
	    જો (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		પાછા FALSE;
	    --todo;
	}
    }
    પાછા TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    long
dict_ગણતરી(dict_T *d, typval_T *needle, પૂર્ણાંક ic)
{
    પૂર્ણાંક		todo;
    hashitem_T	*hi;
    long	n = 0;

    જો (d == ખાલી)
	પાછા 0;

    todo = (પૂર્ણાંક)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(&d->dv_hashtab, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    જો (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    પાછા n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    શૂન્ય
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	અક્ષર		*func_name,
	char_u		*arg_errmsg,
	પૂર્ણાંક		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    પૂર્ણાંક	i;

    d1 = argvars[0].vval.v_dict;
    જો (d1 == ખાલી)
    {
	emsg(_(e_cannot_extend_null_dict));
	પાછા;
    }

    જો (!is_new && કિંમત_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	પાછા;

    જો (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	જો (d1 == ખાલી)
	    પાછા;
    }

    d2 = argvars[1].vval.v_dict;
    જો (d2 == ખાલી)
	જાઓ theend;

    // Check the third argument.
    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	સ્થિર અક્ષર *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	જો (action == ખાલી)
	{
	    જો (is_new)
		dict_unref(d1);
	    પાછા;
	}
	માટે (i = 0; i < 3; ++i)
	    જો (STRCMP(action, av[i]) == 0)
		તોડો;
	જો (i == 3)
	{
	    જો (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    પાછા;
	}
    }
    નહીંતર
	action = (char_u *)"force";

    જો (type != ખાલી && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	જો (is_new)
	    dict_unref(d1);
	પાછા;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    જો (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    નહીંતર
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    શૂન્ય
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	અક્ષર		*func_name,
	char_u		*arg_errmsg,
	typval_T	*પદાવલિ,
	typval_T	*rettv)
{
    dict_T	*d_ret = ખાલી;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    પૂર્ણાંક		todo;
    પૂર્ણાંક		rem;
    typval_T	newtv;
    funccall_T	*fc;

    જો (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = ખાલી;
    }
    જો (d == ખાલી
	  || (filtermap == FILTERMAP_FILTER
			&& કિંમત_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	પાછા;

    જો (filtermap == FILTERMAP_MAPNEW)
    {
	જો (rettv_dict_alloc(rettv) == FAIL)
	    પાછા;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_પદાવલિ_get_funccal(પદાવલિ, &newtv);

    પૂર્ણાંક prev_lock = d->dv_lock;
    જો (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (પૂર્ણાંક)ht->ht_used;
    FOR_ALL_HASHTAB_વસ્તુઓ(ht, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    પૂર્ણાંક		r;

	    --todo;
	    di = HI2DI(hi);
	    જો (filtermap == FILTERMAP_MAP
		    && (કિંમત_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		તોડો;
	    set_vim_var_string(VV_ચાવી, di->di_ચાવી, -1);
	    r = filter_map_one(&di->di_tv, પદાવલિ, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_ચાવી));
	    જો (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		તોડો;
	    }
	    જો (filtermap == FILTERMAP_MAP)
	    {
		જો (argtype != ખાલી && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    તોડો;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    નહીંતર જો (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (અક્ષર *)di->di_ચાવી, &newtv);
		clear_tv(&newtv);
		જો (r == FAIL)
		    તોડો;
	    }
	    નહીંતર જો (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		જો (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    તોડો;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    જો (fc != ખાલી)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    શૂન્ય
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*ચાવી;
    dictitem_T	*di;

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	પાછા;
    }

    d = argvars[0].vval.v_dict;
    જો (d == ખાલી || કિંમત_check_lock(d->dv_lock, arg_errmsg, TRUE))
	પાછા;

    ચાવી = tv_get_string_chk(&argvars[1]);
    જો (ચાવી == ખાલી)
	પાછા;

    di = dict_find(d, ચાવી, -1);
    જો (di == ખાલી)
    {
	semsg(_(e_ચાવી_not_present_in_dictionary_str), ચાવી);
	પાછા;
    }

    જો (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	પાછા;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

પ્રકારવ્યાખ્યા ગણના {
    DICT2_LIST_ચાવીઓ,
    DICT2_LIST_કિંમતો,
    DICT2_LIST_વસ્તુઓ,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    સ્થિર શૂન્ય
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    પૂર્ણાંક		todo;

    જો (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(શૂન્ય)rettv_list_alloc(rettv);
	પાછા;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == ખાલી ? 0 : (પૂર્ણાંક)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_વસ્તુઓ(todo);
    જો (l == ખાલી)
    {
	(શૂન્ય)rettv_list_alloc(rettv);
	પાછા;
    }
    rettv_list_set(rettv, l);
    જો (todo == 0)
	// NULL or empty dict, return the empty list
	પાછા;

    li = l->lv_first;
    FOR_ALL_HASHTAB_વસ્તુઓ(&d->dv_hashtab, hi, todo)
    {
	જો (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    જો (what == DICT2_LIST_ચાવીઓ)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_ચાવી);
	    }
	    નહીંતર જો (what == DICT2_LIST_કિંમતો)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    નહીંતર
	    {
		// items()
		l2 = list_alloc_with_વસ્તુઓ(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		જો (l2 == ખાલી)
		    જાઓ alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_ચાવી);
		જો (l2->lv_first->li_tv.vval.v_string == ખાલી)
		    જાઓ alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    પાછા;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (શૂન્ય)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    શૂન્ય
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_વસ્તુઓ);
}

/*
 * "keys()" function
 */
    શૂન્ય
f_ચાવીઓ(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ચાવીઓ);
}

/*
 * "values(dict)" function
 */
    શૂન્ય
f_કિંમતો(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_કિંમતો);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    શૂન્ય
dict_set_વસ્તુઓ_ro(dict_T *di)
{
    પૂર્ણાંક		todo = (પૂર્ણાંક)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_વસ્તુઓ(&di->dv_hashtab, hi, todo)
    {
	જો (HASHITEM_EMPTY(hi))
	    ચાલુ;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    શૂન્ય
f_has_ચાવી(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    જો (check_for_dict_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].vval.v_dict == ખાલી)
	પાછા;

    rettv->vval.v_સંખ્યા = dict_has_ચાવી(argvars[0].vval.v_dict,
				(અક્ષર *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
