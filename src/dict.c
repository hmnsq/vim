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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
ساکن dict_T		*پہلا_dict = تہی;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(خالی)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    اگر (d == تہی)
	واپس تہی;

    // Add the dict to the list of dicts for garbage collection.
    اگر (پہلا_dict != تہی)
	پہلا_dict->dv_مستعمل_پچھلا = d;
    d->dv_مستعمل_اگلا = پہلا_dict;
    d->dv_مستعمل_پچھلا = تہی;
    پہلا_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_تالا = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    واپس d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	واپس تہی;
#اگر_ختم
    واپس (dict_alloc());
}

    dict_T *
dict_alloc_تالا(صحیح تالا)
{
    dict_T *d = dict_alloc();

    اگر (d != تہی)
	d->dv_تالا = تالا;
    واپس d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    صحیح
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_تالا(0);

    اگر (d == تہی)
	واپس FAIL;

    rettv_dict_سیٹ(rettv, d);
    واپس OK;
}

/*
 * Set a dictionary as the return value
 */
    خالی
rettv_dict_سیٹ(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    اگر (d != تہی)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    خالی
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = تہی;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    خالی
hashtab_free_contents(hashtab_T *ht)
{
    صحیح		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    اگر (check_hashtab_frozen(ht, "clear dict"))
	واپس;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_تالا(ht);
    todo = (صحیح)ht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
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

    ساکن خالی
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    اگر (d->dv_مستعمل_پچھلا == تہی)
	پہلا_dict = d->dv_مستعمل_اگلا;
    ورنہ
	d->dv_مستعمل_پچھلا->dv_مستعمل_اگلا = d->dv_مستعمل_اگلا;
    اگر (d->dv_مستعمل_اگلا != تہی)
	d->dv_مستعمل_اگلا->dv_مستعمل_پچھلا = d->dv_مستعمل_پچھلا;
    vim_free(d);
}

    ساکن خالی
dict_free(dict_T *d)
{
    اگر (!اندر_free_unref_اشیاء)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    خالی
dict_unref(dict_T *d)
{
    اگر (d != تہی && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    صحیح
dict_free_nonref(صحیح copyID)
{
    dict_T	*dd;
    صحیح		did_free = FALSE;

    برائے (dd = پہلا_dict; dd != تہی; dd = dd->dv_مستعمل_اگلا)
	اگر ((dd->dv_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    واپس did_free;
}

    خالی
dict_free_اشیاء(صحیح copyID)
{
    dict_T	*dd, *dd_اگلا;

    برائے (dd = پہلا_dict; dd != تہی; dd = dd_اگلا)
    {
	dd_اگلا = dd->dv_مستعمل_اگلا;
	اگر ((dd->dv_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب))
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
dictitem_alloc(char_u *کلید)
{
    dictitem_T *di;
    size_t len = STRLEN(کلید);

    di = alloc(offsetof(dictitem_T, di_کلید) + len + 1);
    اگر (di == تہی)
	واپس تہی;

    mch_memmove(di->di_کلید, کلید, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_تالا = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    واپس di;
}

/*
 * Make a copy of a Dictionary item.
 */
    ساکن dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_کلید);

    di = alloc(offsetof(dictitem_T, di_کلید) + len + 1);
    اگر (di == تہی)
	واپس تہی;

    mch_memmove(di->di_کلید, org->di_کلید, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    واپس di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    خالی
dictitem_remove(dict_T *dict, dictitem_T *شے, حرف *command)
{
    hashitem_T	*hi;

    hi = hash_ڈھونڈو(&dict->dv_hashtab, شے->di_کلید);
    اگر (HASHITEM_خالی_خانے(hi))
	اندرونی_error("dictitem_remove()");
    ورنہ
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(شے);
}

/*
 * Free a dict item.  Also clears the value.
 */
    خالی
dictitem_free(dictitem_T *شے)
{
    clear_tv(&شے->di_tv);
    اگر (شے->di_flags & DI_FLAGS_ALLOC)
	vim_free(شے);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, صحیح deep, صحیح چوٹی, صحیح copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    صحیح		todo;
    hashitem_T	*hi;

    اگر (orig == تہی)
	واپس تہی;

    copy = dict_alloc();
    اگر (copy == تہی)
	واپس تہی;

    اگر (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    اگر (orig->dv_type == تہی || چوٹی || deep)
	copy->dv_type = تہی;
    ورنہ
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (صحیح)orig->dv_hashtab.ht_مستعمل;
    برائے (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_کلید);
	    اگر (di == تہی)
		توڑو;
	    اگر (deep)
	    {
		اگر (شے_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    توڑو;
		}
	    }
	    ورنہ
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    اگر (dict_جوڑو(copy, di) == FAIL)
	    {
		dictitem_free(di);
		توڑو;
	    }
	}
    }

    ++copy->dv_refcount;
    اگر (todo > 0)
    {
	dict_unref(copy);
	copy = تہی;
    }

    واپس copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    صحیح
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    واپس (d == get_globvar_dict()
		|| (اندر_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid)
		   && d == &SCRIPT_شے(current_sctx.sc_sid)->sn_vars->sv_dict)
		|| &d->dv_hashtab == get_funccal_local_ht())
	    && (tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
	    && var_wrong_func_name(name, TRUE);
}

/*
 * Add item "item" to Dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_جوڑو(dict_T *d, dictitem_T *شے)
{
    اگر (dict_wrong_func_name(d, &شے->di_tv, شے->di_کلید))
	واپس FAIL;
    واپس hash_جوڑو(&d->dv_hashtab, شے->di_کلید, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ساکن صحیح
dict_جوڑو_نمبر_special(
    dict_T	*d,
    حرف	*کلید,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*شے;

    شے = dictitem_alloc((char_u *)کلید);
    اگر (شے == تہی)
	واپس FAIL;
    شے->di_tv.v_type = vartype;
    شے->di_tv.vval.v_نمبر = nr;
    اگر (dict_جوڑو(d, شے) == FAIL)
    {
	dictitem_free(شے);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_جوڑو_نمبر(dict_T *d, حرف *کلید, varnumber_T nr)
{
    واپس dict_جوڑو_نمبر_special(d, کلید, nr, VAR_نمبر);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_جوڑو_bool(dict_T *d, حرف *کلید, varnumber_T nr)
{
    واپس dict_جوڑو_نمبر_special(d, کلید, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_جوڑو_string(dict_T *d, حرف *کلید, char_u *str)
{
    واپس dict_جوڑو_string_len(d, کلید, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_جوڑو_string_len(dict_T *d, حرف *کلید, char_u *str, صحیح len)
{
    dictitem_T	*شے;
    char_u	*val = تہی;

    شے = dictitem_alloc((char_u *)کلید);
    اگر (شے == تہی)
	واپس FAIL;
    شے->di_tv.v_type = VAR_STRING;
    اگر (str != تہی)
    {
	اگر (len == -1)
	    val = vim_strsave(str);
	ورنہ
	    val = vim_strnsave(str, len);
    }
    شے->di_tv.vval.v_string = val;
    اگر (dict_جوڑو(d, شے) == FAIL)
    {
	dictitem_free(شے);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_جوڑو_list(dict_T *d, حرف *کلید, list_T *list)
{
    dictitem_T	*شے;

    شے = dictitem_alloc((char_u *)کلید);
    اگر (شے == تہی)
	واپس FAIL;
    شے->di_tv.v_type = VAR_LIST;
    شے->di_tv.vval.v_list = list;
    اگر (dict_جوڑو(d, شے) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	شے->di_tv.vval.v_list = تہی;
	dictitem_free(شے);
	واپس FAIL;
    }
    ++list->lv_refcount;
    واپس OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_جوڑو_tv(dict_T *d, حرف *کلید, typval_T *tv)
{
    dictitem_T	*شے;

    شے = dictitem_alloc((char_u *)کلید);
    اگر (شے == تہی)
	واپس FAIL;
    copy_tv(tv, &شے->di_tv);
    اگر (dict_جوڑو(d, شے) == FAIL)
    {
	dictitem_free(شے);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_جوڑو_callback(dict_T *d, حرف *کلید, callback_T *cb)
{
    dictitem_T	*شے;

    شے = dictitem_alloc((char_u *)کلید);
    اگر (شے == تہی)
	واپس FAIL;
    put_callback(cb, &شے->di_tv);
    اگر (dict_جوڑو(d, شے) == FAIL)
    {
	dictitem_free(شے);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_جوڑو_func(dict_T *d, حرف *کلید, ufunc_T *fp)
{
    dictitem_T	*شے;

    شے = dictitem_alloc((char_u *)کلید);
    اگر (شے == تہی)
	واپس FAIL;
    شے->di_tv.v_type = VAR_FUNC;
    شے->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(شے->di_tv.vval.v_string);
    اگر (dict_جوڑو(d, شے) == FAIL)
    {
	dictitem_free(شے);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    خالی
dict_iterate_آغاز(typval_T *var, dict_iterator_T *iter)
{
    اگر (var->v_type != VAR_DICT || var->vval.v_dict == تہی)
	iter->dit_todo = 0;
    ورنہ
    {
	dict_T	*d = var->vval.v_dict;

	iter->dit_todo = d->dv_hashtab.ht_مستعمل;
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
dict_iterate_اگلا(dict_iterator_T *iter, typval_T **tv_نتیجہ)
{
    dictitem_T	*di;
    char_u      *نتیجہ;

    اگر (iter->dit_todo == 0)
	واپس تہی;

    جب (HASHITEM_خالی_خانے(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    نتیجہ = di->di_کلید;
    *tv_نتیجہ = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    واپس نتیجہ;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحیح
dict_جوڑو_dict(dict_T *d, حرف *کلید, dict_T *dict)
{
    dictitem_T	*شے;

    شے = dictitem_alloc((char_u *)کلید);
    اگر (شے == تہی)
	واپس FAIL;
    شے->di_tv.v_type = VAR_DICT;
    شے->di_tv.vval.v_dict = dict;
    اگر (dict_جوڑو(d, شے) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	شے->di_tv.vval.v_dict = تہی;
	dictitem_free(شے);
	واپس FAIL;
    }
    ++dict->dv_refcount;
    واپس OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    طویل
dict_len(dict_T *d)
{
    اگر (d == تہی)
	واپس 0L;
    واپس (طویل)d->dv_hashtab.ht_مستعمل;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_ڈھونڈو(dict_T *d, char_u *کلید, صحیح len)
{
#تعریف_کرو AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = تہی;
    hashitem_T	*hi;

    اگر (d == تہی)
	واپس تہی;
    اگر (len < 0)
	akey = کلید;
    ورنہ اگر (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(کلید, len);
	اگر (akey == تہی)
	    واپس تہی;
    }
    ورنہ
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, کلید, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_ڈھونڈو(&d->dv_hashtab, akey);
    vim_free(tofree);
    اگر (HASHITEM_خالی_خانے(hi))
	واپس تہی;
    واپس HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    صحیح
dict_has_کلید(dict_T *d, حرف *کلید)
{
    واپس dict_ڈھونڈو(d, (char_u *)کلید, -1) != تہی;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    صحیح
dict_get_tv(dict_T *d, حرف *کلید, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_ڈھونڈو(d, (char_u *)کلید, -1);
    اگر (di == تہی)
	واپس FAIL;
    copy_tv(&di->di_tv, rettv);
    واپس OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, حرف *کلید, صحیح save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_ڈھونڈو(d, (char_u *)کلید, -1);
    اگر (di == تہی)
	واپس تہی;
    s = tv_get_string(&di->di_tv);
    اگر (save && s != تہی)
	s = vim_strsave(s);
    واپس s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_نمبر(dict_T *d, حرف *کلید)
{
    واپس dict_get_نمبر_def(d, کلید, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_نمبر_def(dict_T *d, حرف *کلید, صحیح def)
{
    dictitem_T	*di;

    di = dict_ڈھونڈو(d, (char_u *)کلید, -1);
    اگر (di == تہی)
	واپس def;
    واپس tv_get_نمبر(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_نمبر_check(dict_T *d, char_u *کلید)
{
    dictitem_T	*di;

    di = dict_ڈھونڈو(d, کلید, -1);
    اگر (di == تہی)
	واپس 0;
    اگر (di->di_tv.v_type != VAR_نمبر)
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(&di->di_tv));
	واپس 0;
    }
    واپس tv_get_نمبر(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, حرف *کلید, صحیح def)
{
    dictitem_T	*di;

    di = dict_ڈھونڈو(d, (char_u *)کلید, -1);
    اگر (di == تہی)
	واپس def;
    واپس tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, صحیح copyID, صحیح restore_copyID)
{
    garray_T	ga;
    صحیح		پہلا = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    صحیح		todo;
    صحیح		پچھلا_تالا;

    اگر ((d = tv->vval.v_dict) == تہی)
	واپس تہی;
    ga_init2(&ga, حجم(حرف), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    پچھلا_تالا = d->dv_تالا;
    اگر (d->dv_تالا == 0)
	d->dv_تالا = VAR_LOCKED;
    hash_تالا(&d->dv_hashtab);

    todo = (صحیح)d->dv_hashtab.ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(&d->dv_hashtab, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;

	    اگر (پہلا)
		پہلا = FALSE;
	    ورنہ
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_کلید, FALSE);
	    اگر (tofree != تہی)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    اگر (s != تہی)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    اگر (s == تہی || did_echo_string_emsg)
		توڑو;
	    سطر_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_تالا = پچھلا_تالا;
    اگر (todo > 0)
    {
	vim_free(ga.ga_ڈیٹا);
	واپس تہی;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    واپس (char_u *)ga.ga_ڈیٹا;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    ساکن char_u *
skip_literal_کلید(char_u *کلید)
{
    char_u *p;

    برائے (p = کلید; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    واپس p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    ساکن صحیح
get_literal_کلید_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_کلید(*arg);

    اگر (p == *arg)
	واپس FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    واپس OK;
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
get_literal_کلید(char_u **arg)
{
    char_u	*کلید;
    char_u	*انتہا;
    typval_T	rettv;

    اگر (**arg == '\'')
    {
	اگر (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    واپس تہی;
	کلید = rettv.vval.v_string;
    }
    ورنہ اگر (**arg == '"')
    {
	اگر (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    واپس تہی;
	کلید = rettv.vval.v_string;
    }
    ورنہ
    {
	انتہا = skip_literal_کلید(*arg);
	اگر (انتہا == *arg)
	{
	    semsg(_(e_invalid_کلید_str), *arg);
	    واپس تہی;
	}
	کلید = vim_strnsave(*arg, انتہا - *arg);
	*arg = انتہا;
    }
    واپس کلید;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    صحیح
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحیح literal)
{
    صحیح		evaluate = evalarg == تہی ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = تہی;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*کلید = تہی;
    dictitem_T	*شے;
    char_u	*curly_اظہاریہ = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    صحیح		vim9script = اندر_vim9script();
    صحیح		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    اگر (!vim9script
	    && *curly_اظہاریہ != '}'
	    && !literal
	    && eval1(&curly_اظہاریہ, &tv, تہی) == OK
	    && *skipwhite(curly_اظہاریہ) == '}')
	واپس NOTDONE;

    اگر (evaluate)
    {
	d = dict_alloc();
	اگر (d == تہی)
	    واپس FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    جب (**arg != '}' && **arg != NUL)
    {
	صحیح	has_bracket = vim9script && **arg == '[';

	اگر (literal)
	{
	    اگر (get_literal_کلید_tv(arg, &tvkey) == FAIL)
		جاؤ failret;
	}
	ورنہ اگر (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_کلید(arg);
	    اگر (tvkey.vval.v_string == تہی)
		جاؤ failret;
	    tvkey.v_type = VAR_STRING;
	}
	ورنہ
	{
	    اگر (has_bracket)
		*arg = skipwhite(*arg + 1);
	    اگر (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		جاؤ failret;
	    اگر (has_bracket)
	    {
		*arg = skipwhite(*arg);
		اگر (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_کلید));
		    clear_tv(&tvkey);
		    جاؤ failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	اگر (!vim9script)
	    *arg = skipwhite(*arg);
	اگر (**arg != ':')
	{
	    اگر (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ورنہ
		semsg(_(e_missing_colon_اندر_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    جاؤ failret;
	}
	اگر (evaluate)
	{
	    اگر (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    کلید = tv_get_string_buf_chk(&tvkey, buf);
	    اگر (کلید == تہی)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		جاؤ failret;
	    }
	}
	اگر (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    جاؤ failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	اگر (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    جاؤ failret;
	}
	اگر (check_typval_is_قدر(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    جاؤ failret;
	}
	اگر (evaluate)
	{
	    شے = dict_ڈھونڈو(d, کلید, -1);
	    اگر (شے != تہی)
	    {
		semsg(_(e_duplicate_کلید_اندر_dictionary_str), کلید);
		clear_tv(&tvkey);
		clear_tv(&tv);
		جاؤ failret;
	    }
	    شے = dictitem_alloc(کلید);
	    اگر (شے != تہی)
	    {
		شے->di_tv = tv;
		شے->di_tv.v_تالا = 0;
		اگر (dict_جوڑو(d, شے) == FAIL)
		    dictitem_free(شے);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	اگر (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	اگر (had_comma)
	{
	    اگر (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		جاؤ failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg == '}')
	    توڑو;
	اگر (!had_comma)
	{
	    اگر (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    ورنہ
		semsg(_(e_missing_comma_اندر_dictionary_str), *arg);
	    جاؤ failret;
	}
    }

    اگر (**arg != '}')
    {
	اگر (evalarg != تہی)
	    semsg(_(e_missing_dict_انتہا_str), *arg);
failret:
	اگر (d != تہی)
	    dict_free(d);
	واپس FAIL;
    }

    *arg = *arg + 1;
    اگر (evaluate)
	rettv_dict_سیٹ(rettv, d);

    واپس OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    صحیح
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    صحیح		vim9script = اندر_vim9script();
    صحیح		ret = OK;

    اگر (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    ورنہ اگر ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    ورنہ
	ret = NOTDONE;

    واپس ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    خالی
dict_extend(dict_T *d1, dict_T *d2, char_u *action, حرف *func_name)
{
    dictitem_T	*di1;
    صحیح		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    اگر (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	واپس;

    اگر (*action == 'm')
    {
	اگر (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    واپس;
	hash_تالا(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    اگر (d1->dv_type != تہی && d1->dv_type->tt_member != تہی)
	type = d1->dv_type->tt_member;
    ورنہ
	type = تہی;

    todo = (صحیح)d2->dv_hashtab.ht_مستعمل;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_اشیاء(&d2->dv_hashtab, hi2, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi2))
	{
	    --todo;
	    di1 = dict_ڈھونڈو(d1, hi2->hi_کلید, -1);
	    // Check the key to be valid when adding to any scope.
	    اگر (d1->dv_scope != 0 && !valid_varname(hi2->hi_کلید, -1, TRUE))
		توڑو;

	    اگر (type != تہی
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		توڑو;

	    اگر (di1 == تہی)
	    {
		اگر (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    اگر (dict_جوڑو(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		ورنہ
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    اگر (di1 != تہی && dict_جوڑو(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    ورنہ اگر (*action == 'e')
	    {
		semsg(_(e_کلید_already_exists_str), hi2->hi_کلید);
		توڑو;
	    }
	    ورنہ اگر (*action == 'f' && HI2DI(hi2) != di1)
	    {
		اگر (قدر_check_تالا(di1->di_tv.v_تالا, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    توڑو;
		// Disallow replacing a builtin function.
		اگر (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_کلید))
		    توڑو;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    اگر (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    واپس HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    صحیح
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    صحیح		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    صحیح		todo;

    اگر (d1 == d2)
	واپس TRUE;
    اگر (dict_len(d1) != dict_len(d2))
	واپس FALSE;
    اگر (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	واپس TRUE;
    اگر (d1 == تہی || d2 == تہی)
	واپس FALSE;

    todo = (صحیح)d1->dv_hashtab.ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(&d1->dv_hashtab, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    item2 = dict_ڈھونڈو(d2, hi->hi_کلید, -1);
	    اگر (item2 == تہی)
		واپس FALSE;
	    اگر (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		واپس FALSE;
	    --todo;
	}
    }
    واپس TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    طویل
dict_گنتی(dict_T *d, typval_T *needle, صحیح ic)
{
    صحیح		todo;
    hashitem_T	*hi;
    طویل	n = 0;

    اگر (d == تہی)
	واپس 0;

    todo = (صحیح)d->dv_hashtab.ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(&d->dv_hashtab, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    اگر (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    واپس n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    خالی
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	حرف		*func_name,
	char_u		*arg_errmsg,
	صحیح		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    صحیح	i;

    d1 = argvars[0].vval.v_dict;
    اگر (d1 == تہی)
    {
	emsg(_(e_cannot_extend_null_dict));
	واپس;
    }

    اگر (!is_new && قدر_check_تالا(d1->dv_تالا, arg_errmsg, TRUE))
	واپس;

    اگر (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	اگر (d1 == تہی)
	    واپس;
    }

    d2 = argvars[1].vval.v_dict;
    اگر (d2 == تہی)
	جاؤ theend;

    // Check the third argument.
    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	ساکن حرف *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	اگر (action == تہی)
	{
	    اگر (is_new)
		dict_unref(d1);
	    واپس;
	}
	برائے (i = 0; i < 3; ++i)
	    اگر (STRCMP(action, av[i]) == 0)
		توڑو;
	اگر (i == 3)
	{
	    اگر (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_دلیل_str), action);
	    واپس;
	}
    }
    ورنہ
	action = (char_u *)"force";

    اگر (type != تہی && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	اگر (is_new)
	    dict_unref(d1);
	واپس;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    اگر (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_تالا = FALSE;
    }
    ورنہ
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    خالی
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	حرف		*func_name,
	char_u		*arg_errmsg,
	typval_T	*اظہاریہ,
	typval_T	*rettv)
{
    dict_T	*d_ret = تہی;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    صحیح		todo;
    صحیح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = تہی;
    }
    اگر (d == تہی
	  || (filtermap == FILTERMAP_FILTER
			&& قدر_check_تالا(d->dv_تالا, arg_errmsg, TRUE)))
	واپس;

    اگر (filtermap == FILTERMAP_MAPNEW)
    {
	اگر (rettv_dict_alloc(rettv) == FAIL)
	    واپس;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اظہاریہ_get_funccal(اظہاریہ, &newtv);

    صحیح پچھلا_تالا = d->dv_تالا;
    اگر (d->dv_تالا == 0)
	d->dv_تالا = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_تالا(ht);
    todo = (صحیح)ht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    صحیح		r;

	    --todo;
	    di = HI2DI(hi);
	    اگر (filtermap == FILTERMAP_MAP
		    && (قدر_check_تالا(di->di_tv.v_تالا,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		توڑو;
	    سیٹ_vim_var_string(VV_کلید, di->di_کلید, -1);
	    r = filter_map_one(&di->di_tv, اظہاریہ, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_کلید));
	    اگر (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		توڑو;
	    }
	    اگر (filtermap == FILTERMAP_MAP)
	    {
		اگر (argtype != تہی && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    توڑو;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_تالا = 0;
		di->di_tv = newtv;
	    }
	    ورنہ اگر (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_جوڑو_tv(d_ret, (حرف *)di->di_کلید, &newtv);
		clear_tv(&newtv);
		اگر (r == FAIL)
		    توڑو;
	    }
	    ورنہ اگر (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		اگر (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    توڑو;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_تالا = پچھلا_تالا;
    اگر (fc != تہی)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    خالی
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*کلید;
    dictitem_T	*di;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	واپس;
    }

    d = argvars[0].vval.v_dict;
    اگر (d == تہی || قدر_check_تالا(d->dv_تالا, arg_errmsg, TRUE))
	واپس;

    کلید = tv_get_string_chk(&argvars[1]);
    اگر (کلید == تہی)
	واپس;

    di = dict_ڈھونڈو(d, کلید, -1);
    اگر (di == تہی)
    {
	semsg(_(e_کلید_not_present_اندر_dictionary_str), کلید);
	واپس;
    }

    اگر (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	واپس;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

قسمتعریف شمار {
    DICT2_LIST_کلیدیں,
    DICT2_LIST_اقدار,
    DICT2_LIST_اشیاء,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    ساکن خالی
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    صحیح		todo;

    اگر (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(خالی)rettv_list_alloc(rettv);
	واپس;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == تہی ? 0 : (صحیح)d->dv_hashtab.ht_مستعمل;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_اشیاء(todo);
    اگر (l == تہی)
    {
	(خالی)rettv_list_alloc(rettv);
	واپس;
    }
    rettv_list_سیٹ(rettv, l);
    اگر (todo == 0)
	// NULL or empty dict, return the empty list
	واپس;

    li = l->lv_پہلا;
    FOR_ALL_HASHTAB_اشیاء(&d->dv_hashtab, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    اگر (what == DICT2_LIST_کلیدیں)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_تالا = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_کلید);
	    }
	    ورنہ اگر (what == DICT2_LIST_اقدار)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    ورنہ
	    {
		// items()
		l2 = list_alloc_with_اشیاء(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_تالا = 0;
		li->li_tv.vval.v_list = l2;
		اگر (l2 == تہی)
		    جاؤ alloc_failed;
		++l2->lv_refcount;

		l2->lv_پہلا->li_tv.v_type = VAR_STRING;
		l2->lv_پہلا->li_tv.vval.v_string = vim_strsave(di->di_کلید);
		اگر (l2->lv_پہلا->li_tv.vval.v_string == تہی)
		    جاؤ alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_آخری->li_tv);
	    }
	    li = li->li_اگلا;
	}
    }
    واپس;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (خالی)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    خالی
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_اشیاء);
}

/*
 * "keys()" function
 */
    خالی
f_کلیدیں(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_کلیدیں);
}

/*
 * "values(dict)" function
 */
    خالی
f_اقدار(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_اقدار);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    خالی
dict_سیٹ_اشیاء_ro(dict_T *di)
{
    صحیح		todo = (صحیح)di->dv_hashtab.ht_مستعمل;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_اشیاء(&di->dv_hashtab, hi, todo)
    {
	اگر (HASHITEM_خالی_خانے(hi))
	    جاری;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    خالی
f_has_کلید(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    اگر (check_for_dict_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].vval.v_dict == تہی)
	واپس;

    rettv->vval.v_نمبر = dict_has_کلید(argvars[0].vval.v_dict,
				(حرف *)tv_get_string(&argvars[1]));
}

#اگر_ختم // defined(FEAT_EVAL)
