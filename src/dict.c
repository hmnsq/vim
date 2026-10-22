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

#include "vim.h"

#නම් defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
static dict_T		*first_dict = NULL;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(void)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    නම් (d == NULL)
	ආපසු NULL;

    // Add the dict to the list of dicts for garbage collection.
    නම් (first_dict != NULL)
	first_dict->dv_used_prev = d;
    d->dv_used_next = first_dict;
    d->dv_used_prev = NULL;
    first_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_lock = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    ආපසු d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id UNUSED)
{
#ifdef FEAT_EVAL
    නම් (alloc_fail_id == id && alloc_does_fail(parimaa_na(list_T)))
	ආපසු NULL;
#endif
    ආපසු (dict_alloc());
}

    dict_T *
dict_alloc_lock(පූර්ණ_සංඛ්‍යා lock)
{
    dict_T *d = dict_alloc();

    නම් (d != NULL)
	d->dv_lock = lock;
    ආපසු d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    පූර්ණ_සංඛ්‍යා
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_lock(0);

    නම් (d == NULL)
	ආපසු FAIL;

    rettv_dict_set(rettv, d);
    ආපසු OK;
}

/*
 * Set a dictionary as the return value
 */
    void
rettv_dict_set(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    නම් (d != NULL)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    void
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = NULL;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    void
hashtab_free_contents(hashtab_T *ht)
{
    පූර්ණ_සංඛ්‍යා		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    නම් (check_hashtab_frozen(ht, "clear dict"))
	ආපසු;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_lock(ht);
    todo = (පූර්ණ_සංඛ්‍යා)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	නම් (!HASHITEM_EMPTY(hi))
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

    static void
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    නම් (d->dv_used_prev == NULL)
	first_dict = d->dv_used_next;
    නැතහොත්
	d->dv_used_prev->dv_used_next = d->dv_used_next;
    නම් (d->dv_used_next != NULL)
	d->dv_used_next->dv_used_prev = d->dv_used_prev;
    vim_free(d);
}

    static void
dict_free(dict_T *d)
{
    නම් (!in_free_unref_items)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    void
dict_unref(dict_T *d)
{
    නම් (d != NULL && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    පූර්ණ_සංඛ්‍යා
dict_free_nonref(පූර්ණ_සංඛ්‍යා copyID)
{
    dict_T	*dd;
    පූර්ණ_සංඛ්‍යා		did_free = FALSE;

    සඳහා (dd = first_dict; dd != NULL; dd = dd->dv_used_next)
	නම් ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    ආපසු did_free;
}

    void
dict_free_items(පූර්ණ_සංඛ්‍යා copyID)
{
    dict_T	*dd, *dd_next;

    සඳහා (dd = first_dict; dd != NULL; dd = dd_next)
    {
	dd_next = dd->dv_used_next;
	නම් ((dd->dv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
dictitem_alloc(char_u *key)
{
    dictitem_T *di;
    size_t len = STRLEN(key);

    di = alloc(offsetof(dictitem_T, di_key) + len + 1);
    නම් (di == NULL)
	ආපසු NULL;

    mch_memmove(di->di_key, key, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_lock = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    ආපසු di;
}

/*
 * Make a copy of a Dictionary item.
 */
    static dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_key);

    di = alloc(offsetof(dictitem_T, di_key) + len + 1);
    නම් (di == NULL)
	ආපසු NULL;

    mch_memmove(di->di_key, org->di_key, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    ආපසු di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    void
dictitem_remove(dict_T *dict, dictitem_T *item, var_na *command)
{
    hashitem_T	*hi;

    hi = hash_find(&dict->dv_hashtab, item->di_key);
    නම් (HASHITEM_EMPTY(hi))
	internal_error("dictitem_remove()");
    නැතහොත්
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(item);
}

/*
 * Free a dict item.  Also clears the value.
 */
    void
dictitem_free(dictitem_T *item)
{
    clear_tv(&item->di_tv);
    නම් (item->di_flags & DI_FLAGS_ALLOC)
	vim_free(item);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, පූර්ණ_සංඛ්‍යා deep, පූර්ණ_සංඛ්‍යා top, පූර්ණ_සංඛ්‍යා copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    පූර්ණ_සංඛ්‍යා		todo;
    hashitem_T	*hi;

    නම් (orig == NULL)
	ආපසු NULL;

    copy = dict_alloc();
    නම් (copy == NULL)
	ආපසු NULL;

    නම් (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    නම් (orig->dv_type == NULL || top || deep)
	copy->dv_type = NULL;
    නැතහොත්
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (පූර්ණ_සංඛ්‍යා)orig->dv_hashtab.ht_used;
    සඳහා (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	නම් (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_key);
	    නම් (di == NULL)
		bha_nga;
	    නම් (deep)
	    {
		නම් (item_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    bha_nga;
		}
	    }
	    නැතහොත්
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    නම් (dict_add(copy, di) == FAIL)
	    {
		dictitem_free(di);
		bha_nga;
	    }
	}
    }

    ++copy->dv_refcount;
    නම් (todo > 0)
    {
	dict_unref(copy);
	copy = NULL;
    }

    ආපසු copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    පූර්ණ_සංඛ්‍යා
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    ආපසු (d == get_globvar_dict()
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
    පූර්ණ_සංඛ්‍යා
dict_add(dict_T *d, dictitem_T *item)
{
    නම් (dict_wrong_func_name(d, &item->di_tv, item->di_key))
	ආපසු FAIL;
    ආපසු hash_add(&d->dv_hashtab, item->di_key, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    static පූර්ණ_සංඛ්‍යා
dict_add_number_special(
    dict_T	*d,
    var_na	*key,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    නම් (item == NULL)
	ආපසු FAIL;
    item->di_tv.v_type = vartype;
    item->di_tv.vval.v_number = nr;
    නම් (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ආපසු FAIL;
    }
    ආපසු OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    පූර්ණ_සංඛ්‍යා
dict_add_number(dict_T *d, var_na *key, varnumber_T nr)
{
    ආපසු dict_add_number_special(d, key, nr, VAR_NUMBER);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    පූර්ණ_සංඛ්‍යා
dict_add_bool(dict_T *d, var_na *key, varnumber_T nr)
{
    ආපසු dict_add_number_special(d, key, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    පූර්ණ_සංඛ්‍යා
dict_add_string(dict_T *d, var_na *key, char_u *str)
{
    ආපසු dict_add_string_len(d, key, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    පූර්ණ_සංඛ්‍යා
dict_add_string_len(dict_T *d, var_na *key, char_u *str, පූර්ණ_සංඛ්‍යා len)
{
    dictitem_T	*item;
    char_u	*val = NULL;

    item = dictitem_alloc((char_u *)key);
    නම් (item == NULL)
	ආපසු FAIL;
    item->di_tv.v_type = VAR_STRING;
    නම් (str != NULL)
    {
	නම් (len == -1)
	    val = vim_strsave(str);
	නැතහොත්
	    val = vim_strnsave(str, len);
    }
    item->di_tv.vval.v_string = val;
    නම් (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ආපසු FAIL;
    }
    ආපසු OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    පූර්ණ_සංඛ්‍යා
dict_add_list(dict_T *d, var_na *key, list_T *list)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    නම් (item == NULL)
	ආපසු FAIL;
    item->di_tv.v_type = VAR_LIST;
    item->di_tv.vval.v_list = list;
    නම් (dict_add(d, item) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_list = NULL;
	dictitem_free(item);
	ආපසු FAIL;
    }
    ++list->lv_refcount;
    ආපසු OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    පූර්ණ_සංඛ්‍යා
dict_add_tv(dict_T *d, var_na *key, typval_T *tv)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    නම් (item == NULL)
	ආපසු FAIL;
    copy_tv(tv, &item->di_tv);
    නම් (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ආපසු FAIL;
    }
    ආපසු OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    පූර්ණ_සංඛ්‍යා
dict_add_callback(dict_T *d, var_na *key, callback_T *cb)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    නම් (item == NULL)
	ආපසු FAIL;
    put_callback(cb, &item->di_tv);
    නම් (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ආපසු FAIL;
    }
    ආපසු OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    පූර්ණ_සංඛ්‍යා
dict_add_func(dict_T *d, var_na *key, ufunc_T *fp)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    නම් (item == NULL)
	ආපසු FAIL;
    item->di_tv.v_type = VAR_FUNC;
    item->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(item->di_tv.vval.v_string);
    නම් (dict_add(d, item) == FAIL)
    {
	dictitem_free(item);
	ආපසු FAIL;
    }
    ආපසු OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    void
dict_iterate_start(typval_T *var, dict_iterator_T *iter)
{
    නම් (var->v_type != VAR_DICT || var->vval.v_dict == NULL)
	iter->dit_todo = 0;
    නැතහොත්
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

    නම් (iter->dit_todo == 0)
	ආපසු NULL;

    අතරතුර (HASHITEM_EMPTY(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    result = di->di_key;
    *tv_result = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    ආපසු result;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    පූර්ණ_සංඛ්‍යා
dict_add_dict(dict_T *d, var_na *key, dict_T *dict)
{
    dictitem_T	*item;

    item = dictitem_alloc((char_u *)key);
    නම් (item == NULL)
	ආපසු FAIL;
    item->di_tv.v_type = VAR_DICT;
    item->di_tv.vval.v_dict = dict;
    නම් (dict_add(d, item) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	item->di_tv.vval.v_dict = NULL;
	dictitem_free(item);
	ආපසු FAIL;
    }
    ++dict->dv_refcount;
    ආපසු OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    long
dict_len(dict_T *d)
{
    නම් (d == NULL)
	ආපසු 0L;
    ආපසු (long)d->dv_hashtab.ht_used;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_find(dict_T *d, char_u *key, පූර්ණ_සංඛ්‍යා len)
{
#define AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = NULL;
    hashitem_T	*hi;

    නම් (d == NULL)
	ආපසු NULL;
    නම් (len < 0)
	akey = key;
    නැතහොත් නම් (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(key, len);
	නම් (akey == NULL)
	    ආපසු NULL;
    }
    නැතහොත්
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, key, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_find(&d->dv_hashtab, akey);
    vim_free(tofree);
    නම් (HASHITEM_EMPTY(hi))
	ආපසු NULL;
    ආපසු HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    පූර්ණ_සංඛ්‍යා
dict_has_key(dict_T *d, var_na *key)
{
    ආපසු dict_find(d, (char_u *)key, -1) != NULL;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    පූර්ණ_සංඛ්‍යා
dict_get_tv(dict_T *d, var_na *key, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)key, -1);
    නම් (di == NULL)
	ආපසු FAIL;
    copy_tv(&di->di_tv, rettv);
    ආපසු OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, var_na *key, පූර්ණ_සංඛ්‍යා save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_find(d, (char_u *)key, -1);
    නම් (di == NULL)
	ආපසු NULL;
    s = tv_get_string(&di->di_tv);
    නම් (save && s != NULL)
	s = vim_strsave(s);
    ආපසු s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_number(dict_T *d, var_na *key)
{
    ආපසු dict_get_number_def(d, key, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_number_def(dict_T *d, var_na *key, පූර්ණ_සංඛ්‍යා def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)key, -1);
    නම් (di == NULL)
	ආපසු def;
    ආපසු tv_get_number(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_number_check(dict_T *d, char_u *key)
{
    dictitem_T	*di;

    di = dict_find(d, key, -1);
    නම් (di == NULL)
	ආපසු 0;
    නම් (di->di_tv.v_type != VAR_NUMBER)
    {
	semsg(_(e_invalid_argument_str), tv_get_string(&di->di_tv));
	ආපසු 0;
    }
    ආපසු tv_get_number(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, var_na *key, පූර්ණ_සංඛ්‍යා def)
{
    dictitem_T	*di;

    di = dict_find(d, (char_u *)key, -1);
    නම් (di == NULL)
	ආපසු def;
    ආපසු tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, පූර්ණ_සංඛ්‍යා copyID, පූර්ණ_සංඛ්‍යා restore_copyID)
{
    garray_T	ga;
    පූර්ණ_සංඛ්‍යා		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    පූර්ණ_සංඛ්‍යා		todo;
    පූර්ණ_සංඛ්‍යා		prev_lock;

    නම් ((d = tv->vval.v_dict) == NULL)
	ආපසු NULL;
    ga_init2(&ga, parimaa_na(var_na), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_lock = d->dv_lock;
    නම් (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    hash_lock(&d->dv_hashtab);

    todo = (පූර්ණ_සංඛ්‍යා)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	නම් (!HASHITEM_EMPTY(hi))
	{
	    --todo;

	    නම් (first)
		first = FALSE;
	    නැතහොත්
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_key, FALSE);
	    නම් (tofree != NULL)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    නම් (s != NULL)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    නම් (s == NULL || did_echo_string_emsg)
		bha_nga;
	    line_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_lock = prev_lock;
    නම් (todo > 0)
    {
	vim_free(ga.ga_data);
	ආපසු NULL;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    ආපසු (char_u *)ga.ga_data;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    static char_u *
skip_literal_key(char_u *key)
{
    char_u *p;

    සඳහා (p = key; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    ආපසු p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    static පූර්ණ_සංඛ්‍යා
get_literal_key_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_key(*arg);

    නම් (p == *arg)
	ආපසු FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    ආපසු OK;
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
get_literal_key(char_u **arg)
{
    char_u	*key;
    char_u	*end;
    typval_T	rettv;

    නම් (**arg == '\'')
    {
	නම් (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ආපසු NULL;
	key = rettv.vval.v_string;
    }
    නැතහොත් නම් (**arg == '"')
    {
	නම් (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    ආපසු NULL;
	key = rettv.vval.v_string;
    }
    නැතහොත්
    {
	end = skip_literal_key(*arg);
	නම් (end == *arg)
	{
	    semsg(_(e_invalid_key_str), *arg);
	    ආපසු NULL;
	}
	key = vim_strnsave(*arg, end - *arg);
	*arg = end;
    }
    ආපසු key;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    පූර්ණ_සංඛ්‍යා
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, පූර්ණ_සංඛ්‍යා literal)
{
    පූර්ණ_සංඛ්‍යා		evaluate = evalarg == NULL ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = NULL;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*key = NULL;
    dictitem_T	*item;
    char_u	*curly_expr = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();
    පූර්ණ_සංඛ්‍යා		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    නම් (!vim9script
	    && *curly_expr != '}'
	    && !literal
	    && eval1(&curly_expr, &tv, NULL) == OK
	    && *skipwhite(curly_expr) == '}')
	ආපසු NOTDONE;

    නම් (evaluate)
    {
	d = dict_alloc();
	නම් (d == NULL)
	    ආපසු FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    අතරතුර (**arg != '}' && **arg != NUL)
    {
	පූර්ණ_සංඛ්‍යා	has_bracket = vim9script && **arg == '[';

	නම් (literal)
	{
	    නම් (get_literal_key_tv(arg, &tvkey) == FAIL)
		goto failret;
	}
	නැතහොත් නම් (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_key(arg);
	    නම් (tvkey.vval.v_string == NULL)
		goto failret;
	    tvkey.v_type = VAR_STRING;
	}
	නැතහොත්
	{
	    නම් (has_bracket)
		*arg = skipwhite(*arg + 1);
	    නම් (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		goto failret;
	    නම් (has_bracket)
	    {
		*arg = skipwhite(*arg);
		නම් (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_key));
		    clear_tv(&tvkey);
		    goto failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	නම් (!vim9script)
	    *arg = skipwhite(*arg);
	නම් (**arg != ':')
	{
	    නම් (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    නැතහොත්
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    goto failret;
	}
	නම් (evaluate)
	{
	    නම් (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    key = tv_get_string_buf_chk(&tvkey, buf);
	    නම් (key == NULL)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		goto failret;
	    }
	}
	නම් (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    goto failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	නම් (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    goto failret;
	}
	නම් (check_typval_is_value(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    goto failret;
	}
	නම් (evaluate)
	{
	    item = dict_find(d, key, -1);
	    නම් (item != NULL)
	    {
		semsg(_(e_duplicate_key_in_dictionary_str), key);
		clear_tv(&tvkey);
		clear_tv(&tv);
		goto failret;
	    }
	    item = dictitem_alloc(key);
	    නම් (item != NULL)
	    {
		item->di_tv = tv;
		item->di_tv.v_lock = 0;
		නම් (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	නම් (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	නම් (had_comma)
	{
	    නම් (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		goto failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	නම් (**arg == '}')
	    bha_nga;
	නම් (!had_comma)
	{
	    නම් (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    නැතහොත්
		semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    goto failret;
	}
    }

    නම් (**arg != '}')
    {
	නම් (evalarg != NULL)
	    semsg(_(e_missing_dict_end_str), *arg);
failret:
	නම් (d != NULL)
	    dict_free(d);
	ආපසු FAIL;
    }

    *arg = *arg + 1;
    නම් (evaluate)
	rettv_dict_set(rettv, d);

    ආපසු OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    පූර්ණ_සංඛ්‍යා
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();
    පූර්ණ_සංඛ්‍යා		ret = OK;

    නම් (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    නැතහොත් නම් ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    නැතහොත්
	ret = NOTDONE;

    ආපසු ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    void
dict_extend(dict_T *d1, dict_T *d2, char_u *action, var_na *func_name)
{
    dictitem_T	*di1;
    පූර්ණ_සංඛ්‍යා		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    නම් (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	ආපසු;

    නම් (*action == 'm')
    {
	නම් (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    ආපසු;
	hash_lock(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    නම් (d1->dv_type != NULL && d1->dv_type->tt_member != NULL)
	type = d1->dv_type->tt_member;
    නැතහොත්
	type = NULL;

    todo = (පූර්ණ_සංඛ්‍යා)d2->dv_hashtab.ht_used;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_ITEMS(&d2->dv_hashtab, hi2, todo)
    {
	නම් (!HASHITEM_EMPTY(hi2))
	{
	    --todo;
	    di1 = dict_find(d1, hi2->hi_key, -1);
	    // Check the key to be valid when adding to any scope.
	    නම් (d1->dv_scope != 0 && !valid_varname(hi2->hi_key, -1, TRUE))
		bha_nga;

	    නම් (type != NULL
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		bha_nga;

	    නම් (di1 == NULL)
	    {
		නම් (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    නම් (dict_add(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		නැතහොත්
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    නම් (di1 != NULL && dict_add(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    නැතහොත් නම් (*action == 'e')
	    {
		semsg(_(e_key_already_exists_str), hi2->hi_key);
		bha_nga;
	    }
	    නැතහොත් නම් (*action == 'f' && HI2DI(hi2) != di1)
	    {
		නම් (value_check_lock(di1->di_tv.v_lock, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    bha_nga;
		// Disallow replacing a builtin function.
		නම් (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_key))
		    bha_nga;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    නම් (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    ආපසු HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    පූර්ණ_සංඛ්‍යා
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    පූර්ණ_සංඛ්‍යා		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    පූර්ණ_සංඛ්‍යා		todo;

    නම් (d1 == d2)
	ආපසු TRUE;
    නම් (dict_len(d1) != dict_len(d2))
	ආපසු FALSE;
    නම් (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	ආපසු TRUE;
    නම් (d1 == NULL || d2 == NULL)
	ආපසු FALSE;

    todo = (පූර්ණ_සංඛ්‍යා)d1->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ITEMS(&d1->dv_hashtab, hi, todo)
    {
	නම් (!HASHITEM_EMPTY(hi))
	{
	    item2 = dict_find(d2, hi->hi_key, -1);
	    නම් (item2 == NULL)
		ආපසු FALSE;
	    නම් (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		ආපසු FALSE;
	    --todo;
	}
    }
    ආපසු TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    long
dict_count(dict_T *d, typval_T *needle, පූර්ණ_සංඛ්‍යා ic)
{
    පූර්ණ_සංඛ්‍යා		todo;
    hashitem_T	*hi;
    long	n = 0;

    නම් (d == NULL)
	ආපසු 0;

    todo = (පූර්ණ_සංඛ්‍යා)d->dv_hashtab.ht_used;
    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	නම් (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    නම් (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    ආපසු n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    void
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	var_na		*func_name,
	char_u		*arg_errmsg,
	පූර්ණ_සංඛ්‍යා		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    පූර්ණ_සංඛ්‍යා	i;

    d1 = argvars[0].vval.v_dict;
    නම් (d1 == NULL)
    {
	emsg(_(e_cannot_extend_null_dict));
	ආපසු;
    }

    නම් (!is_new && value_check_lock(d1->dv_lock, arg_errmsg, TRUE))
	ආපසු;

    නම් (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	නම් (d1 == NULL)
	    ආපසු;
    }

    d2 = argvars[1].vval.v_dict;
    නම් (d2 == NULL)
	goto theend;

    // Check the third argument.
    නම් (argvars[2].v_type != VAR_UNKNOWN)
    {
	static var_na *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	නම් (action == NULL)
	{
	    නම් (is_new)
		dict_unref(d1);
	    ආපසු;
	}
	සඳහා (i = 0; i < 3; ++i)
	    නම් (STRCMP(action, av[i]) == 0)
		bha_nga;
	නම් (i == 3)
	{
	    නම් (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_argument_str), action);
	    ආපසු;
	}
    }
    නැතහොත්
	action = (char_u *)"force";

    නම් (type != NULL && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	නම් (is_new)
	    dict_unref(d1);
	ආපසු;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    නම් (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_lock = FALSE;
    }
    නැතහොත්
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    void
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	var_na		*func_name,
	char_u		*arg_errmsg,
	typval_T	*expr,
	typval_T	*rettv)
{
    dict_T	*d_ret = NULL;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    පූර්ණ_සංඛ්‍යා		todo;
    පූර්ණ_සංඛ්‍යා		rem;
    typval_T	newtv;
    funccall_T	*fc;

    නම් (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = NULL;
    }
    නම් (d == NULL
	  || (filtermap == FILTERMAP_FILTER
			&& value_check_lock(d->dv_lock, arg_errmsg, TRUE)))
	ආපසු;

    නම් (filtermap == FILTERMAP_MAPNEW)
    {
	නම් (rettv_dict_alloc(rettv) == FAIL)
	    ආපසු;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    පූර්ණ_සංඛ්‍යා prev_lock = d->dv_lock;
    නම් (d->dv_lock == 0)
	d->dv_lock = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_lock(ht);
    todo = (පූර්ණ_සංඛ්‍යා)ht->ht_used;
    FOR_ALL_HASHTAB_ITEMS(ht, hi, todo)
    {
	නම් (!HASHITEM_EMPTY(hi))
	{
	    පූර්ණ_සංඛ්‍යා		r;

	    --todo;
	    di = HI2DI(hi);
	    නම් (filtermap == FILTERMAP_MAP
		    && (value_check_lock(di->di_tv.v_lock,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		bha_nga;
	    set_vim_var_string(VV_KEY, di->di_key, -1);
	    r = filter_map_one(&di->di_tv, expr, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_KEY));
	    නම් (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		bha_nga;
	    }
	    නම් (filtermap == FILTERMAP_MAP)
	    {
		නම් (argtype != NULL && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    bha_nga;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_lock = 0;
		di->di_tv = newtv;
	    }
	    නැතහොත් නම් (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_add_tv(d_ret, (var_na *)di->di_key, &newtv);
		clear_tv(&newtv);
		නම් (r == FAIL)
		    bha_nga;
	    }
	    නැතහොත් නම් (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		නම් (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    bha_nga;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_lock = prev_lock;
    නම් (fc != NULL)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    void
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*key;
    dictitem_T	*di;

    නම් (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	ආපසු;
    }

    d = argvars[0].vval.v_dict;
    නම් (d == NULL || value_check_lock(d->dv_lock, arg_errmsg, TRUE))
	ආපසු;

    key = tv_get_string_chk(&argvars[1]);
    නම් (key == NULL)
	ආපසු;

    di = dict_find(d, key, -1);
    නම් (di == NULL)
    {
	semsg(_(e_key_not_present_in_dictionary_str), key);
	ආපසු;
    }

    නම් (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	ආපසු;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

typedef ga_nanaa {
    DICT2LIST_KEYS,
    DICT2LIST_VALUES,
    DICT2LIST_ITEMS,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    static void
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    පූර්ණ_සංඛ්‍යා		todo;

    නම් (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(void)rettv_list_alloc(rettv);
	ආපසු;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == NULL ? 0 : (පූර්ණ_සංඛ්‍යා)d->dv_hashtab.ht_used;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_items(todo);
    නම් (l == NULL)
    {
	(void)rettv_list_alloc(rettv);
	ආපසු;
    }
    rettv_list_set(rettv, l);
    නම් (todo == 0)
	// NULL or empty dict, return the empty list
	ආපසු;

    li = l->lv_first;
    FOR_ALL_HASHTAB_ITEMS(&d->dv_hashtab, hi, todo)
    {
	නම් (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    නම් (what == DICT2LIST_KEYS)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_key);
	    }
	    නැතහොත් නම් (what == DICT2LIST_VALUES)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    නැතහොත්
	    {
		// items()
		l2 = list_alloc_with_items(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_lock = 0;
		li->li_tv.vval.v_list = l2;
		නම් (l2 == NULL)
		    goto alloc_failed;
		++l2->lv_refcount;

		l2->lv_first->li_tv.v_type = VAR_STRING;
		l2->lv_first->li_tv.vval.v_string = vim_strsave(di->di_key);
		නම් (l2->lv_first->li_tv.vval.v_string == NULL)
		    goto alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_last->li_tv);
	    }
	    li = li->li_next;
	}
    }
    ආපසු;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (void)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    void
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2LIST_ITEMS);
}

/*
 * "keys()" function
 */
    void
f_keys(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2LIST_KEYS);
}

/*
 * "values(dict)" function
 */
    void
f_values(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2LIST_VALUES);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    void
dict_set_items_ro(dict_T *di)
{
    පූර්ණ_සංඛ්‍යා		todo = (පූර්ණ_සංඛ්‍යා)di->dv_hashtab.ht_used;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_ITEMS(&di->dv_hashtab, hi, todo)
    {
	නම් (HASHITEM_EMPTY(hi))
	    continue;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    void
f_has_key(typval_T *argvars, typval_T *rettv)
{
    නම් (in_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_number_arg(argvars, 1) == FAIL))
	ආපසු;

    නම් (check_for_dict_arg(argvars, 0) == FAIL)
	ආපසු;

    නම් (argvars[0].vval.v_dict == NULL)
	ආපසු;

    rettv->vval.v_number = dict_has_key(argvars[0].vval.v_dict,
				(var_na *)tv_get_string(&argvars[1]));
}

#endif // defined(FEAT_EVAL)
