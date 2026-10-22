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

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

// List head for garbage collection. Although there can be a reference loop
// from partial to dict to partial, we don't need to keep track of the partial,
// since it will get freed when the dict is unused and gets freed.
ساكن dict_T		*لومړی_dict = عدم;

/*
 * Allocate an empty header for a dictionary.
 * Caller should take care of the reference count.
 */
    dict_T *
dict_alloc(فراغ)
{
    dict_T *d;

    d = ALLOC_CLEAR_ONE(dict_T);
    إذا (d == عدم)
	أرجع عدم;

    // Add the dict to the list of dicts for garbage collection.
    إذا (لومړی_dict != عدم)
	لومړی_dict->dv_کارول_شوی_prev = d;
    d->dv_کارول_شوی_بل = لومړی_dict;
    d->dv_کارول_شوی_prev = عدم;
    لومړی_dict = d;

    hash_init(&d->dv_hashtab);
    d->dv_قلف = 0;
    d->dv_scope = 0;
    d->dv_refcount = 0;
    d->dv_copyID = 0;
    أرجع d;
}

/*
 * dict_alloc() with an ID for alloc_fail().
 */
    dict_T *
dict_alloc_id(alloc_id_T id ناکارول_شوی)
{
#که_تعریف_شوی FEAT_EVAL
    إذا (alloc_fail_id == id && alloc_does_fail(حجم(list_T)))
	أرجع عدم;
#د_که_پای
    أرجع (dict_alloc());
}

    dict_T *
dict_alloc_قلف(صحيح قلف)
{
    dict_T *d = dict_alloc();

    إذا (d != عدم)
	d->dv_قلف = قلف;
    أرجع d;
}

/*
 * Allocate an empty dict for a return value.
 * Returns OK or FAIL.
 */
    صحيح
rettv_dict_alloc(typval_T *rettv)
{
    dict_T	*d = dict_alloc_قلف(0);

    إذا (d == عدم)
	أرجع FAIL;

    rettv_dict_سېټ(rettv, d);
    أرجع OK;
}

/*
 * Set a dictionary as the return value
 */
    فراغ
rettv_dict_سېټ(typval_T *rettv, dict_T *d)
{
    rettv->v_type = VAR_DICT;
    rettv->vval.v_dict = d;
    إذا (d != عدم)
	++d->dv_refcount;
}

/*
 * Free a Dictionary, including all non-container items it contains.
 * Ignores the reference count.
 */
    فراغ
dict_free_contents(dict_T *d)
{
    hashtab_free_contents(&d->dv_hashtab);
    free_type(d->dv_type);
    d->dv_type = عدم;
}

/*
 * Clear hashtab "ht" and dict items it contains.
 * If "ht" is not freed then you should call hash_init() next!
 */
    فراغ
hashtab_free_contents(hashtab_T *ht)
{
    صحيح		todo;
    hashitem_T	*hi;
    dictitem_T	*di;

    إذا (check_hashtab_frozen(ht, "clear dict"))
	أرجع;

    // Lock the hashtab, we don't want it to resize while freeing items.
    hash_قلف(ht);
    todo = (صحيح)ht->ht_کارول_شوی;
    FOR_ALL_HASHTAB_توکي(ht, hi, todo)
    {
	إذا (!HASHITEM_تشه(hi))
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

    ساكن فراغ
dict_free_dict(dict_T *d)
{
    // Remove the dict from the list of dicts for garbage collection.
    إذا (d->dv_کارول_شوی_prev == عدم)
	لومړی_dict = d->dv_کارول_شوی_بل;
    وإلا
	d->dv_کارول_شوی_prev->dv_کارول_شوی_بل = d->dv_کارول_شوی_بل;
    إذا (d->dv_کارول_شوی_بل != عدم)
	d->dv_کارول_شوی_بل->dv_کارول_شوی_prev = d->dv_کارول_شوی_prev;
    vim_free(d);
}

    ساكن فراغ
dict_free(dict_T *d)
{
    إذا (!دننه_free_unref_توکي)
    {
	dict_free_contents(d);
	dict_free_dict(d);
    }
}

/*
 * Unreference a Dictionary: decrement the reference count and free it when it
 * becomes zero.
 */
    فراغ
dict_unref(dict_T *d)
{
    إذا (d != عدم && --d->dv_refcount <= 0)
	dict_free(d);
}

/*
 * Go through the list of dicts and free items without the copyID.
 * Returns TRUE if something was freed.
 */
    صحيح
dict_free_nonref(صحيح copyID)
{
    dict_T	*dd;
    صحيح		did_free = FALSE;

    لكل (dd = لومړی_dict; dd != عدم; dd = dd->dv_کارول_شوی_بل)
	إذا ((dd->dv_copyID & COPYID_ماسک) != (copyID & COPYID_ماسک))
	{
	    // Free the Dictionary and ordinary items it contains, but don't
	    // recurse into Lists and Dictionaries, they will be in the list
	    // of dicts or list of lists.
	    dict_free_contents(dd);
	    did_free = TRUE;
	}
    أرجع did_free;
}

    فراغ
dict_free_توکي(صحيح copyID)
{
    dict_T	*dd, *dd_بل;

    لكل (dd = لومړی_dict; dd != عدم; dd = dd_بل)
    {
	dd_بل = dd->dv_کارول_شوی_بل;
	إذا ((dd->dv_copyID & COPYID_ماسک) != (copyID & COPYID_ماسک))
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
dictitem_alloc(char_u *کلی)
{
    dictitem_T *di;
    size_t len = STRLEN(کلی);

    di = alloc(offsetof(dictitem_T, di_کلی) + len + 1);
    إذا (di == عدم)
	أرجع عدم;

    mch_memmove(di->di_کلی, کلی, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    di->di_tv.v_قلف = 0;
    di->di_tv.v_type = VAR_UNKNOWN;
    أرجع di;
}

/*
 * Make a copy of a Dictionary item.
 */
    ساكن dictitem_T *
dictitem_copy(dictitem_T *org)
{
    dictitem_T *di;
    size_t	len = STRLEN(org->di_کلی);

    di = alloc(offsetof(dictitem_T, di_کلی) + len + 1);
    إذا (di == عدم)
	أرجع عدم;

    mch_memmove(di->di_کلی, org->di_کلی, len + 1);
    di->di_flags = DI_FLAGS_ALLOC;
    copy_tv(&org->di_tv, &di->di_tv);
    أرجع di;
}

/*
 * Remove item "item" from Dictionary "dict" and free it.
 * "command" is used for the error message when the hashtab if frozen.
 */
    فراغ
dictitem_remove(dict_T *dict, dictitem_T *توکی, حرف *command)
{
    hashitem_T	*hi;

    hi = hash_پیدا_کړه(&dict->dv_hashtab, توکی->di_کلی);
    إذا (HASHITEM_تشه(hi))
	دننی_error("dictitem_remove()");
    وإلا
	hash_remove(&dict->dv_hashtab, hi, command);
    dictitem_free(توکی);
}

/*
 * Free a dict item.  Also clears the value.
 */
    فراغ
dictitem_free(dictitem_T *توکی)
{
    clear_tv(&توکی->di_tv);
    إذا (توکی->di_flags & DI_FLAGS_ALLOC)
	vim_free(توکی);
}

/*
 * Make a copy of dict "d".  Shallow if "deep" is FALSE.
 * The refcount of the new dict is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    dict_T *
dict_copy(dict_T *orig, صحيح deep, صحيح سر_2, صحيح copyID)
{
    dict_T	*copy;
    dictitem_T	*di;
    صحيح		todo;
    hashitem_T	*hi;

    إذا (orig == عدم)
	أرجع عدم;

    copy = dict_alloc();
    إذا (copy == عدم)
	أرجع عدم;

    إذا (copyID != 0)
    {
	orig->dv_copyID = copyID;
	orig->dv_copydict = copy;
    }
    إذا (orig->dv_type == عدم || سر_2 || deep)
	copy->dv_type = عدم;
    وإلا
	copy->dv_type = alloc_type(orig->dv_type);

    todo = (صحيح)orig->dv_hashtab.ht_کارول_شوی;
    لكل (hi = orig->dv_hashtab.ht_array; todo > 0 && !got_int; ++hi)
    {
	إذا (!HASHITEM_تشه(hi))
	{
	    --todo;

	    di = dictitem_alloc(hi->hi_کلی);
	    إذا (di == عدم)
		اكسر;
	    إذا (deep)
	    {
		إذا (توکی_copy(&HI2DI(hi)->di_tv, &di->di_tv,
			    deep, FALSE, copyID) == FAIL)
		{
		    vim_free(di);
		    اكسر;
		}
	    }
	    وإلا
		copy_tv(&HI2DI(hi)->di_tv, &di->di_tv);
	    إذا (dict_جمع_کړه(copy, di) == FAIL)
	    {
		dictitem_free(di);
		اكسر;
	    }
	}
    }

    ++copy->dv_refcount;
    إذا (todo > 0)
    {
	dict_unref(copy);
	copy = عدم;
    }

    أرجع copy;
}

/*
 * Check for adding a function to g: or s: (in Vim9 script) or l:.
 * If the name is wrong give an error message and return TRUE.
 */
    صحيح
dict_wrong_func_name(dict_T *d, typval_T *tv, char_u *name)
{
    أرجع (d == get_globvar_dict()
		|| (دننه_vim9script() && SCRIPT_ID_VALID(current_sctx.sc_sid)
		   && d == &SCRIPT_توکی(current_sctx.sc_sid)->sn_vars->sv_dict)
		|| &d->dv_hashtab == get_funccal_local_ht())
	    && (tv->v_type == VAR_FUNC || tv->v_type == VAR_PARTIAL)
	    && var_wrong_func_name(name, TRUE);
}

/*
 * Add item "item" to Dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_جمع_کړه(dict_T *d, dictitem_T *توکی)
{
    إذا (dict_wrong_func_name(d, &توکی->di_tv, توکی->di_کلی))
	أرجع FAIL;
    أرجع hash_جمع_کړه(&d->dv_hashtab, توکی->di_کلی, "add to dictionary");
}

/*
 * Add a number or special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    ساكن صحيح
dict_جمع_کړه_شمېره_special(
    dict_T	*d,
    حرف	*کلی,
    varnumber_T	nr,
    vartype_T	vartype)
{
    dictitem_T	*توکی;

    توکی = dictitem_alloc((char_u *)کلی);
    إذا (توکی == عدم)
	أرجع FAIL;
    توکی->di_tv.v_type = vartype;
    توکی->di_tv.vval.v_شمېره = nr;
    إذا (dict_جمع_کړه(d, توکی) == FAIL)
    {
	dictitem_free(توکی);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Add a number entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_جمع_کړه_شمېره(dict_T *d, حرف *کلی, varnumber_T nr)
{
    أرجع dict_جمع_کړه_شمېره_special(d, کلی, nr, VAR_شمېره);
}

/*
 * Add a special entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_جمع_کړه_bool(dict_T *d, حرف *کلی, varnumber_T nr)
{
    أرجع dict_جمع_کړه_شمېره_special(d, کلی, nr, VAR_BOOL);
}

/*
 * Add a string entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_جمع_کړه_string(dict_T *d, حرف *کلی, char_u *str)
{
    أرجع dict_جمع_کړه_string_len(d, کلی, str, -1);
}

/*
 * Add a string entry to dictionary "d".
 * "str" will be copied to allocated memory.
 * When "len" is -1 use the whole string, otherwise only this many bytes.
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_جمع_کړه_string_len(dict_T *d, حرف *کلی, char_u *str, صحيح len)
{
    dictitem_T	*توکی;
    char_u	*val = عدم;

    توکی = dictitem_alloc((char_u *)کلی);
    إذا (توکی == عدم)
	أرجع FAIL;
    توکی->di_tv.v_type = VAR_STRING;
    إذا (str != عدم)
    {
	إذا (len == -1)
	    val = vim_strsave(str);
	وإلا
	    val = vim_strnsave(str, len);
    }
    توکی->di_tv.vval.v_string = val;
    إذا (dict_جمع_کړه(d, توکی) == FAIL)
    {
	dictitem_free(توکی);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Add a list entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_جمع_کړه_list(dict_T *d, حرف *کلی, list_T *list)
{
    dictitem_T	*توکی;

    توکی = dictitem_alloc((char_u *)کلی);
    إذا (توکی == عدم)
	أرجع FAIL;
    توکی->di_tv.v_type = VAR_LIST;
    توکی->di_tv.vval.v_list = list;
    إذا (dict_جمع_کړه(d, توکی) == FAIL)
    {
	// Detach "list" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	توکی->di_tv.vval.v_list = عدم;
	dictitem_free(توکی);
	أرجع FAIL;
    }
    ++list->lv_refcount;
    أرجع OK;
}

/*
 * Add a typval_T entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_جمع_کړه_tv(dict_T *d, حرف *کلی, typval_T *tv)
{
    dictitem_T	*توکی;

    توکی = dictitem_alloc((char_u *)کلی);
    إذا (توکی == عدم)
	أرجع FAIL;
    copy_tv(tv, &توکی->di_tv);
    إذا (dict_جمع_کړه(d, توکی) == FAIL)
    {
	dictitem_free(توکی);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Add a callback to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_جمع_کړه_callback(dict_T *d, حرف *کلی, callback_T *cb)
{
    dictitem_T	*توکی;

    توکی = dictitem_alloc((char_u *)کلی);
    إذا (توکی == عدم)
	أرجع FAIL;
    put_callback(cb, &توکی->di_tv);
    إذا (dict_جمع_کړه(d, توکی) == FAIL)
    {
	dictitem_free(توکی);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Add a function entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_جمع_کړه_func(dict_T *d, حرف *کلی, ufunc_T *fp)
{
    dictitem_T	*توکی;

    توکی = dictitem_alloc((char_u *)کلی);
    إذا (توکی == عدم)
	أرجع FAIL;
    توکی->di_tv.v_type = VAR_FUNC;
    توکی->di_tv.vval.v_string = vim_strnsave(fp->uf_name, fp->uf_namelen);
    // Reference before dict_add() so dictitem_free()'s unref stays balanced on failure.
    func_ref(توکی->di_tv.vval.v_string);
    إذا (dict_جمع_کړه(d, توکی) == FAIL)
    {
	dictitem_free(توکی);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Initializes "iter" for iterating over dictionary items with
 * dict_iterate_next().
 * If "var" is not a Dict or an empty Dict then there will be nothing to
 * iterate over, no error is given.
 * NOTE: The dictionary must not change until iterating is finished!
 */
    فراغ
dict_iterate_پیل(typval_T *var, dict_iterator_T *iter)
{
    إذا (var->v_type != VAR_DICT || var->vval.v_dict == عدم)
	iter->dit_todo = 0;
    وإلا
    {
	dict_T	*d = var->vval.v_dict;

	iter->dit_todo = d->dv_hashtab.ht_کارول_شوی;
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
dict_iterate_بل(dict_iterator_T *iter, typval_T **tv_پایله)
{
    dictitem_T	*di;
    char_u      *پایله;

    إذا (iter->dit_todo == 0)
	أرجع عدم;

    طالما (HASHITEM_تشه(iter->dit_hi))
	++iter->dit_hi;

    di = HI2DI(iter->dit_hi);
    پایله = di->di_کلی;
    *tv_پایله = &di->di_tv;

    --iter->dit_todo;
    ++iter->dit_hi;
    أرجع پایله;
}

/*
 * Add a dict entry to dictionary "d".
 * Returns FAIL when out of memory and when key already exists.
 */
    صحيح
dict_جمع_کړه_dict(dict_T *d, حرف *کلی, dict_T *dict)
{
    dictitem_T	*توکی;

    توکی = dictitem_alloc((char_u *)کلی);
    إذا (توکی == عدم)
	أرجع FAIL;
    توکی->di_tv.v_type = VAR_DICT;
    توکی->di_tv.vval.v_dict = dict;
    إذا (dict_جمع_کړه(d, توکی) == FAIL)
    {
	// Detach "dict" so dictitem_free() does not unref it: on failure
	// ownership stays with the caller.
	توکی->di_tv.vval.v_dict = عدم;
	dictitem_free(توکی);
	أرجع FAIL;
    }
    ++dict->dv_refcount;
    أرجع OK;
}

/*
 * Get the number of items in a Dictionary.
 */
    اوږد
dict_len(dict_T *d)
{
    إذا (d == عدم)
	أرجع 0L;
    أرجع (اوږد)d->dv_hashtab.ht_کارول_شوی;
}

/*
 * Find item "key[len]" in Dictionary "d".
 * If "len" is negative use strlen(key).
 * Returns NULL when not found.
 */
    dictitem_T *
dict_پیدا_کړه(dict_T *d, char_u *کلی, صحيح len)
{
#تعریف_کړه AKEYLEN 200
    char_u	buf[AKEYLEN];
    char_u	*akey;
    char_u	*tofree = عدم;
    hashitem_T	*hi;

    إذا (d == عدم)
	أرجع عدم;
    إذا (len < 0)
	akey = کلی;
    وإلا إذا (len >= AKEYLEN)
    {
	tofree = akey = vim_strnsave(کلی, len);
	إذا (akey == عدم)
	    أرجع عدم;
    }
    وإلا
    {
	// Avoid a malloc/free by using buf[].
	mch_memmove(buf, کلی, (size_t)len);
	buf[len] = NUL;
	akey = buf;
    }

    hi = hash_پیدا_کړه(&d->dv_hashtab, akey);
    vim_free(tofree);
    إذا (HASHITEM_تشه(hi))
	أرجع عدم;
    أرجع HI2DI(hi);
}

/*
 * Returns TRUE if "key" is present in Dictionary "d".
 */
    صحيح
dict_has_کلی(dict_T *d, حرف *کلی)
{
    أرجع dict_پیدا_کړه(d, (char_u *)کلی, -1) != عدم;
}

/*
 * Get a typval_T item from a dictionary and copy it into "rettv".
 * Returns FAIL if the entry doesn't exist or out of memory.
 */
    صحيح
dict_get_tv(dict_T *d, حرف *کلی, typval_T *rettv)
{
    dictitem_T	*di;

    di = dict_پیدا_کړه(d, (char_u *)کلی, -1);
    إذا (di == عدم)
	أرجع FAIL;
    copy_tv(&di->di_tv, rettv);
    أرجع OK;
}

/*
 * Get a string item from a dictionary.
 * When "save" is TRUE allocate memory for it.
 * When FALSE a shared buffer is used, can only be used once!
 * Returns NULL if the entry doesn't exist or out of memory.
 */
    char_u *
dict_get_string(dict_T *d, حرف *کلی, صحيح save)
{
    dictitem_T	*di;
    char_u	*s;

    di = dict_پیدا_کړه(d, (char_u *)کلی, -1);
    إذا (di == عدم)
	أرجع عدم;
    s = tv_get_string(&di->di_tv);
    إذا (save && s != عدم)
	s = vim_strsave(s);
    أرجع s;
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 */
    varnumber_T
dict_get_شمېره(dict_T *d, حرف *کلی)
{
    أرجع dict_get_شمېره_def(d, کلی, 0);
}

/*
 * Get a number item from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_شمېره_def(dict_T *d, حرف *کلی, صحيح def)
{
    dictitem_T	*di;

    di = dict_پیدا_کړه(d, (char_u *)کلی, -1);
    إذا (di == عدم)
	أرجع def;
    أرجع tv_get_شمېره(&di->di_tv);
}

/*
 * Get a number item from a dictionary.
 * Returns 0 if the entry doesn't exist.
 * Give an error if the entry is not a number.
 */
    varnumber_T
dict_get_شمېره_check(dict_T *d, char_u *کلی)
{
    dictitem_T	*di;

    di = dict_پیدا_کړه(d, کلی, -1);
    إذا (di == عدم)
	أرجع 0;
    إذا (di->di_tv.v_type != VAR_شمېره)
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(&di->di_tv));
	أرجع 0;
    }
    أرجع tv_get_شمېره(&di->di_tv);
}

/*
 * Get a bool item (number or true/false) from a dictionary.
 * Returns "def" if the entry doesn't exist.
 */
    varnumber_T
dict_get_bool(dict_T *d, حرف *کلی, صحيح def)
{
    dictitem_T	*di;

    di = dict_پیدا_کړه(d, (char_u *)کلی, -1);
    إذا (di == عدم)
	أرجع def;
    أرجع tv_get_bool(&di->di_tv);
}

/*
 * Return an allocated string with the string representation of a Dictionary.
 * May return NULL.
 */
    char_u *
dict2string(typval_T *tv, صحيح copyID, صحيح restore_copyID)
{
    garray_T	ga;
    صحيح		لومړی = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    hashitem_T	*hi;
    char_u	*s;
    dict_T	*d;
    صحيح		todo;
    صحيح		prev_قلف;

    إذا ((d = tv->vval.v_dict) == عدم)
	أرجع عدم;
    ga_init2(&ga, حجم(حرف), 80);
    ga_append(&ga, '{');

    // Lock the dict, the string() method of an object item could remove an
    // item or make the hashtab resize while we iterate over it.
    prev_قلف = d->dv_قلف;
    إذا (d->dv_قلف == 0)
	d->dv_قلف = VAR_LOCKED;
    hash_قلف(&d->dv_hashtab);

    todo = (صحيح)d->dv_hashtab.ht_کارول_شوی;
    FOR_ALL_HASHTAB_توکي(&d->dv_hashtab, hi, todo)
    {
	إذا (!HASHITEM_تشه(hi))
	{
	    --todo;

	    إذا (لومړی)
		لومړی = FALSE;
	    وإلا
		GA_CONCAT_LITERAL(&ga, ", ");

	    tofree = string_quote(hi->hi_کلی, FALSE);
	    إذا (tofree != عدم)
	    {
		ga_concat(&ga, tofree);
		vim_free(tofree);
	    }
	    GA_CONCAT_LITERAL(&ga, ": ");
	    s = echo_string_core(&HI2DI(hi)->di_tv, &tofree, numbuf, copyID,
						 FALSE, restore_copyID, TRUE);
	    إذا (s != عدم)
		ga_concat(&ga, s);
	    vim_free(tofree);
	    إذا (s == عدم || did_echo_string_emsg)
		اكسر;
	    کرښه_breakcheck();

	}
    }
    hash_unlock(&d->dv_hashtab);
    d->dv_قلف = prev_قلف;
    إذا (todo > 0)
    {
	vim_free(ga.ga_معلومات);
	أرجع عدم;
    }

    ga_append(&ga, '}');
    ga_append(&ga, NUL);
    أرجع (char_u *)ga.ga_معلومات;
}

/*
 * Advance over a literal key, including "-".  If the first character is not a
 * literal key character then "key" is returned.
 */
    ساكن char_u *
skip_literal_کلی(char_u *کلی)
{
    char_u *p;

    لكل (p = کلی; ASCII_ISALNUM(*p) || *p == '_' || *p == '-'; ++p)
	;
    أرجع p;
}

/*
 * Get the key for #{key: val} into "tv" and advance "arg".
 * Return FAIL when there is no valid key.
 */
    ساكن صحيح
get_literal_کلی_tv(char_u **arg, typval_T *tv)
{
    char_u *p = skip_literal_کلی(*arg);

    إذا (p == *arg)
	أرجع FAIL;
    tv->v_type = VAR_STRING;
    tv->vval.v_string = vim_strnsave(*arg, p - *arg);

    *arg = p;
    أرجع OK;
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
get_literal_کلی(char_u **arg)
{
    char_u	*کلی;
    char_u	*پای_ټکی;
    typval_T	rettv;

    إذا (**arg == '\'')
    {
	إذا (eval_lit_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    أرجع عدم;
	کلی = rettv.vval.v_string;
    }
    وإلا إذا (**arg == '"')
    {
	إذا (eval_string(arg, &rettv, TRUE, FALSE) == FAIL)
	    أرجع عدم;
	کلی = rettv.vval.v_string;
    }
    وإلا
    {
	پای_ټکی = skip_literal_کلی(*arg);
	إذا (پای_ټکی == *arg)
	{
	    semsg(_(e_invalid_کلی_str), *arg);
	    أرجع عدم;
	}
	کلی = vim_strnsave(*arg, پای_ټکی - *arg);
	*arg = پای_ټکی;
    }
    أرجع کلی;
}

/*
 * Allocate a variable for a Dictionary and fill it from "*arg".
 * "*arg" points to the "{".
 * "literal" is TRUE for #{key: val}
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    صحيح
eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحيح literal)
{
    صحيح		evaluate = evalarg == عدم ? FALSE
				       : (evalarg->eval_flags & EVAL_EVALUATE);
    dict_T	*d = عدم;
    typval_T	tvkey;
    typval_T	tv;
    char_u	*کلی = عدم;
    dictitem_T	*توکی;
    char_u	*curly_بیان = skipwhite(*arg + 1);
    char_u	buf[NUMBUFLEN];
    صحيح		vim9script = دننه_vim9script();
    صحيح		had_comma;

    // First check if it's not a curly-braces expression: {expr}.
    // Must do this without evaluating, otherwise a function may be called
    // twice.  Unfortunately this means we need to call eval1() twice for the
    // first item.
    // "{}" is an empty Dictionary.
    // "#{abc}" is never a curly-braces expression.
    إذا (!vim9script
	    && *curly_بیان != '}'
	    && !literal
	    && eval1(&curly_بیان, &tv, عدم) == OK
	    && *skipwhite(curly_بیان) == '}')
	أرجع NOTDONE;

    إذا (evaluate)
    {
	d = dict_alloc();
	إذا (d == عدم)
	    أرجع FAIL;
    }
    tvkey.v_type = VAR_UNKNOWN;
    tv.v_type = VAR_UNKNOWN;

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    طالما (**arg != '}' && **arg != NUL)
    {
	صحيح	has_bracket = vim9script && **arg == '[';

	إذا (literal)
	{
	    إذا (get_literal_کلی_tv(arg, &tvkey) == FAIL)
		اذهب failret;
	}
	وإلا إذا (vim9script && !has_bracket)
	{
	    tvkey.vval.v_string = get_literal_کلی(arg);
	    إذا (tvkey.vval.v_string == عدم)
		اذهب failret;
	    tvkey.v_type = VAR_STRING;
	}
	وإلا
	{
	    إذا (has_bracket)
		*arg = skipwhite(*arg + 1);
	    إذا (eval1(arg, &tvkey, evalarg) == FAIL)	// recursive!
		اذهب failret;
	    إذا (has_bracket)
	    {
		*arg = skipwhite(*arg);
		إذا (**arg != ']')
		{
		    emsg(_(e_missing_matching_bracket_after_dict_کلی));
		    clear_tv(&tvkey);
		    اذهب failret;
		}
		++*arg;
	    }
	}

	// the colon should come right after the key, but this wasn't checked
	// previously, so only require it in Vim9 script.
	إذا (!vim9script)
	    *arg = skipwhite(*arg);
	إذا (**arg != ':')
	{
	    إذا (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    وإلا
		semsg(_(e_missing_colon_دننه_dictionary_str), *arg);
	    clear_tv(&tvkey);
	    اذهب failret;
	}
	إذا (evaluate)
	{
	    إذا (tvkey.v_type == VAR_FLOAT)
	    {
		tvkey.vval.v_string = typval_tostring(&tvkey, TRUE);
		tvkey.v_type = VAR_STRING;
	    }
	    کلی = tv_get_string_buf_chk(&tvkey, buf);
	    إذا (کلی == عدم)
	    {
		// "key" is NULL when tv_get_string_buf_chk() gave an errmsg
		clear_tv(&tvkey);
		اذهب failret;
	    }
	}
	إذا (vim9script && (*arg)[1] != NUL && !VIM_ISWHITE((*arg)[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    clear_tv(&tvkey);
	    اذهب failret;
	}

	*arg = skipwhite_and_linebreak(*arg + 1, evalarg);
	إذا (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	{
	    clear_tv(&tvkey);
	    اذهب failret;
	}
	إذا (check_typval_is_ارزښت(&tv) == FAIL)
	{
	    clear_tv(&tvkey);
	    clear_tv(&tv);
	    اذهب failret;
	}
	إذا (evaluate)
	{
	    توکی = dict_پیدا_کړه(d, کلی, -1);
	    إذا (توکی != عدم)
	    {
		semsg(_(e_duplicate_کلی_دننه_dictionary_str), کلی);
		clear_tv(&tvkey);
		clear_tv(&tv);
		اذهب failret;
	    }
	    توکی = dictitem_alloc(کلی);
	    إذا (توکی != عدم)
	    {
		توکی->di_tv = tv;
		توکی->di_tv.v_قلف = 0;
		إذا (dict_جمع_کړه(d, توکی) == FAIL)
		    dictitem_free(توکی);
	    }
	}
	clear_tv(&tvkey);

	// the comma should come right after the value, but this wasn't checked
	// previously, so only require it in Vim9 script.
	إذا (!vim9script)
	    *arg = skipwhite(*arg);
	had_comma = **arg == ',';
	إذا (had_comma)
	{
	    إذا (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		اذهب failret;
	    }
	    *arg = skipwhite_and_nl(*arg + 1);
	}

	// the "}" can be on the next line
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	إذا (**arg == '}')
	    اكسر;
	إذا (!had_comma)
	{
	    إذا (**arg == ',')
		semsg(_(e_no_white_space_allowed_before_str_str), ",", *arg);
	    وإلا
		semsg(_(e_missing_comma_دننه_dictionary_str), *arg);
	    اذهب failret;
	}
    }

    إذا (**arg != '}')
    {
	إذا (evalarg != عدم)
	    semsg(_(e_missing_dict_پای_ټکی_str), *arg);
failret:
	إذا (d != عدم)
	    dict_free(d);
	أرجع FAIL;
    }

    *arg = *arg + 1;
    إذا (evaluate)
	rettv_dict_سېټ(rettv, d);

    أرجع OK;
}

/*
 * Evaluate a literal dictionary: #{key: val, key: val}
 * "*arg" points to the "#".
 * On return, "*arg" points to the character after the Dict.
 * Return OK or FAIL.  Returns NOTDONE for {expr}.
 */
    صحيح
eval_lit_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg)
{
    صحيح		vim9script = دننه_vim9script();
    صحيح		ret = OK;

    إذا (vim9script)
    {
	ret = vim9_bad_comment(*arg) ? FAIL : NOTDONE;
    }
    وإلا إذا ((*arg)[1] == '{')
    {
	++*arg;
	ret = eval_dict(arg, rettv, evalarg, TRUE);
    }
    وإلا
	ret = NOTDONE;

    أرجع ret;
}

/*
 * Go over all entries in "d2" and add them to "d1".
 * When "action" is "error" then a duplicate key is an error.
 * When "action" is "force" then a duplicate key is overwritten.
 * When "action" is "move" then move items instead of copying.
 * Otherwise duplicate keys are ignored ("action" is "keep").
 * "func_name" is used for reporting where an error occurred.
 */
    فراغ
dict_extend(dict_T *d1, dict_T *d2, char_u *action, حرف *func_name)
{
    dictitem_T	*di1;
    صحيح		todo;
    char_u	*arg_errmsg = (char_u *)N_("extend() argument");
    type_T	*type;

    إذا (check_hashtab_frozen(&d1->dv_hashtab, "extend"))
	أرجع;

    إذا (*action == 'm')
    {
	إذا (check_hashtab_frozen(&d2->dv_hashtab, "extend"))
	    أرجع;
	hash_قلف(&d2->dv_hashtab);  // don't rehash on hash_remove()
    }

    إذا (d1->dv_type != عدم && d1->dv_type->tt_member != عدم)
	type = d1->dv_type->tt_member;
    وإلا
	type = عدم;

    todo = (صحيح)d2->dv_hashtab.ht_کارول_شوی;
    hashitem_T *hi2;
    FOR_ALL_HASHTAB_توکي(&d2->dv_hashtab, hi2, todo)
    {
	إذا (!HASHITEM_تشه(hi2))
	{
	    --todo;
	    di1 = dict_پیدا_کړه(d1, hi2->hi_کلی, -1);
	    // Check the key to be valid when adding to any scope.
	    إذا (d1->dv_scope != 0 && !valid_varname(hi2->hi_کلی, -1, TRUE))
		اكسر;

	    إذا (type != عدم
		     && check_typval_arg_type(type, &HI2DI(hi2)->di_tv,
							 func_name, 0) == FAIL)
		اكسر;

	    إذا (di1 == عدم)
	    {
		إذا (*action == 'm')
		{
		    // Cheap way to move a dict item from "d2" to "d1".
		    // If dict_add() fails then "d2" won't be empty.
		    di1 = HI2DI(hi2);
		    إذا (dict_جمع_کړه(d1, di1) == OK)
			hash_remove(&d2->dv_hashtab, hi2, "extend");
		}
		وإلا
		{
		    di1 = dictitem_copy(HI2DI(hi2));
		    إذا (di1 != عدم && dict_جمع_کړه(d1, di1) == FAIL)
			dictitem_free(di1);
		}
	    }
	    وإلا إذا (*action == 'e')
	    {
		semsg(_(e_کلی_already_exists_str), hi2->hi_کلی);
		اكسر;
	    }
	    وإلا إذا (*action == 'f' && HI2DI(hi2) != di1)
	    {
		إذا (ارزښت_check_قلف(di1->di_tv.v_قلف, arg_errmsg, TRUE)
			|| var_check_ro(di1->di_flags, arg_errmsg, TRUE))
		    اكسر;
		// Disallow replacing a builtin function.
		إذا (dict_wrong_func_name(d1, &HI2DI(hi2)->di_tv, hi2->hi_کلی))
		    اكسر;
		clear_tv(&di1->di_tv);
		copy_tv(&HI2DI(hi2)->di_tv, &di1->di_tv);
	    }
	}
    }

    إذا (*action == 'm')
	hash_unlock(&d2->dv_hashtab);
}

/*
 * Return the dictitem that an entry in a hashtable points to.
 */
    dictitem_T *
dict_lookup(hashitem_T *hi)
{
    أرجع HI2DI(hi);
}

/*
 * Return TRUE when two dictionaries have exactly the same key/values.
 */
    صحيح
dict_equal(
    dict_T	*d1,
    dict_T	*d2,
    صحيح		ic)	    // ignore case for strings
{
    hashitem_T	*hi;
    dictitem_T	*item2;
    صحيح		todo;

    إذا (d1 == d2)
	أرجع TRUE;
    إذا (dict_len(d1) != dict_len(d2))
	أرجع FALSE;
    إذا (dict_len(d1) == 0)
	// empty and NULL dicts are considered equal
	أرجع TRUE;
    إذا (d1 == عدم || d2 == عدم)
	أرجع FALSE;

    todo = (صحيح)d1->dv_hashtab.ht_کارول_شوی;
    FOR_ALL_HASHTAB_توکي(&d1->dv_hashtab, hi, todo)
    {
	إذا (!HASHITEM_تشه(hi))
	{
	    item2 = dict_پیدا_کړه(d2, hi->hi_کلی, -1);
	    إذا (item2 == عدم)
		أرجع FALSE;
	    إذا (!tv_equal(&HI2DI(hi)->di_tv, &item2->di_tv, ic))
		أرجع FALSE;
	    --todo;
	}
    }
    أرجع TRUE;
}

/*
 * Count the number of times item "needle" occurs in Dict "d". Case is ignored
 * if "ic" is TRUE.
 */
    اوږد
dict_شمېر(dict_T *d, typval_T *needle, صحيح ic)
{
    صحيح		todo;
    hashitem_T	*hi;
    اوږد	n = 0;

    إذا (d == عدم)
	أرجع 0;

    todo = (صحيح)d->dv_hashtab.ht_کارول_شوی;
    FOR_ALL_HASHTAB_توکي(&d->dv_hashtab, hi, todo)
    {
	إذا (!HASHITEM_تشه(hi))
	{
	    --todo;
	    إذا (tv_equal(&HI2DI(hi)->di_tv, needle, ic))
		++n;
	}
    }

    أرجع n;
}

/*
 * extend() a Dict. Append Dict argvars[1] to Dict argvars[0] and return the
 * resulting Dict in "rettv".  "is_new" is TRUE for extendnew().
 */
    فراغ
dict_extend_func(
	typval_T	*argvars,
	type_T		*type,
	حرف		*func_name,
	char_u		*arg_errmsg,
	صحيح		is_new,
	typval_T	*rettv)
{
    dict_T	*d1, *d2;
    char_u	*action;
    صحيح	i;

    d1 = argvars[0].vval.v_dict;
    إذا (d1 == عدم)
    {
	emsg(_(e_cannot_extend_null_dict));
	أرجع;
    }

    إذا (!is_new && ارزښت_check_قلف(d1->dv_قلف, arg_errmsg, TRUE))
	أرجع;

    إذا (is_new)
    {
	d1 = dict_copy(d1, FALSE, TRUE, get_copyID());
	إذا (d1 == عدم)
	    أرجع;
    }

    d2 = argvars[1].vval.v_dict;
    إذا (d2 == عدم)
	اذهب theend;

    // Check the third argument.
    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	ساكن حرف *(av[]) = {"keep", "force", "error"};

	action = tv_get_string_chk(&argvars[2]);
	إذا (action == عدم)
	{
	    إذا (is_new)
		dict_unref(d1);
	    أرجع;
	}
	لكل (i = 0; i < 3; ++i)
	    إذا (STRCMP(action, av[i]) == 0)
		اكسر;
	إذا (i == 3)
	{
	    إذا (is_new)
		dict_unref(d1);
	    semsg(_(e_invalid_دلیل_str), action);
	    أرجع;
	}
    }
    وإلا
	action = (char_u *)"force";

    إذا (type != عدم && check_typval_arg_type(type, &argvars[1],
							 func_name, 2) == FAIL)
    {
	إذا (is_new)
	    dict_unref(d1);
	أرجع;
    }
    dict_extend(d1, d2, action, func_name);

theend:
    إذا (is_new)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = d1;
	rettv->v_قلف = FALSE;
    }
    وإلا
	copy_tv(&argvars[0], rettv);
}

/*
 * Implementation of map(), filter(), foreach() for a Dict.  Apply "expr" to
 * every item in Dict "d" and return the result in "rettv".
 */
    فراغ
dict_filter_map(
	dict_T		*d,
	filtermap_T	filtermap,
	type_T		*argtype,
	حرف		*func_name,
	char_u		*arg_errmsg,
	typval_T	*بیان,
	typval_T	*rettv)
{
    dict_T	*d_ret = عدم;
    hashtab_T	*ht;
    hashitem_T	*hi;
    dictitem_T	*di;
    صحيح		todo;
    صحيح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    إذا (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_DICT;
	rettv->vval.v_dict = عدم;
    }
    إذا (d == عدم
	  || (filtermap == FILTERMAP_FILTER
			&& ارزښت_check_قلف(d->dv_قلف, arg_errmsg, TRUE)))
	أرجع;

    إذا (filtermap == FILTERMAP_MAPNEW)
    {
	إذا (rettv_dict_alloc(rettv) == FAIL)
	    أرجع;
	d_ret = rettv->vval.v_dict;
    }

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_بیان_get_funccal(بیان, &newtv);

    صحيح prev_قلف = d->dv_قلف;
    إذا (d->dv_قلف == 0)
	d->dv_قلف = VAR_LOCKED;
    ht = &d->dv_hashtab;
    hash_قلف(ht);
    todo = (صحيح)ht->ht_کارول_شوی;
    FOR_ALL_HASHTAB_توکي(ht, hi, todo)
    {
	إذا (!HASHITEM_تشه(hi))
	{
	    صحيح		r;

	    --todo;
	    di = HI2DI(hi);
	    إذا (filtermap == FILTERMAP_MAP
		    && (ارزښت_check_قلف(di->di_tv.v_قلف,
			    arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags,
			    arg_errmsg, TRUE)))
		اكسر;
	    سېټ_vim_var_string(VV_کلی, di->di_کلی, -1);
	    r = filter_map_one(&di->di_tv, بیان, filtermap, fc, &newtv, &rem);
	    clear_tv(get_vim_var_tv(VV_کلی));
	    إذا (r == FAIL || did_emsg)
	    {
		clear_tv(&newtv);
		اكسر;
	    }
	    إذا (filtermap == FILTERMAP_MAP)
	    {
		إذا (argtype != عدم && check_typval_arg_type(
			     argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    اكسر;
		}
		// map(): replace the dict item value
		clear_tv(&di->di_tv);
		newtv.v_قلف = 0;
		di->di_tv = newtv;
	    }
	    وإلا إذا (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): add the item value to the new dict
		r = dict_جمع_کړه_tv(d_ret, (حرف *)di->di_کلی, &newtv);
		clear_tv(&newtv);
		إذا (r == FAIL)
		    اكسر;
	    }
	    وإلا إذا (filtermap == FILTERMAP_FILTER && rem)
	    {
		// filter(false): remove the item from the dict
		إذا (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
			|| var_check_ro(di->di_flags, arg_errmsg, TRUE))
		    اكسر;
		dictitem_remove(d, di, "filter");
	    }
	}
    }
    hash_unlock(ht);
    d->dv_قلف = prev_قلف;
    إذا (fc != عدم)
	remove_funccal();
}

/*
 * "remove({dict})" function
 */
    فراغ
dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    dict_T	*d;
    char_u	*کلی;
    dictitem_T	*di;

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	semsg(_(e_too_many_arguments_for_function_str), "remove()");
	أرجع;
    }

    d = argvars[0].vval.v_dict;
    إذا (d == عدم || ارزښت_check_قلف(d->dv_قلف, arg_errmsg, TRUE))
	أرجع;

    کلی = tv_get_string_chk(&argvars[1]);
    إذا (کلی == عدم)
	أرجع;

    di = dict_پیدا_کړه(d, کلی, -1);
    إذا (di == عدم)
    {
	semsg(_(e_کلی_not_present_دننه_dictionary_str), کلی);
	أرجع;
    }

    إذا (var_check_fixed(di->di_flags, arg_errmsg, TRUE)
	    || var_check_ro(di->di_flags, arg_errmsg, TRUE))
	أرجع;

    *rettv = di->di_tv;
    init_tv(&di->di_tv);
    dictitem_remove(d, di, "remove()");
}

تعريفنوع تعداد {
    DICT2_LIST_کلیګانې,
    DICT2_LIST_ارزښتونه,
    DICT2_LIST_توکي,
} dict2list_T;

/*
 * Turn a dict into a list.
 */
    ساكن فراغ
dict2list(typval_T *argvars, typval_T *rettv, dict2list_T what)
{
    list_T	*l;
    list_T	*l2;
    dictitem_T	*di;
    hashitem_T	*hi;
    listitem_T	*li;
    dict_T	*d;
    صحيح		todo;

    إذا (check_for_dict_arg(argvars, 0) == FAIL)
    {
	// invalid argument, return an empty list
	(فراغ)rettv_list_alloc(rettv);
	أرجع;
    }

    // NULL dict behaves like an empty dict
    d = argvars[0].vval.v_dict;
    todo = d == عدم ? 0 : (صحيح)d->dv_hashtab.ht_کارول_شوی;

    // The number of items is known, allocate the list and all its items in
    // one go for efficiency.
    l = list_alloc_with_توکي(todo);
    إذا (l == عدم)
    {
	(فراغ)rettv_list_alloc(rettv);
	أرجع;
    }
    rettv_list_سېټ(rettv, l);
    إذا (todo == 0)
	// NULL or empty dict, return the empty list
	أرجع;

    li = l->lv_لومړی;
    FOR_ALL_HASHTAB_توکي(&d->dv_hashtab, hi, todo)
    {
	إذا (!HASHITEM_تشه(hi))
	{
	    --todo;
	    di = HI2DI(hi);

	    إذا (what == DICT2_LIST_کلیګانې)
	    {
		// keys()
		li->li_tv.v_type = VAR_STRING;
		li->li_tv.v_قلف = 0;
		li->li_tv.vval.v_string = vim_strsave(di->di_کلی);
	    }
	    وإلا إذا (what == DICT2_LIST_ارزښتونه)
	    {
		// values()
		copy_tv(&di->di_tv, &li->li_tv);
	    }
	    وإلا
	    {
		// items()
		l2 = list_alloc_with_توکي(2);
		li->li_tv.v_type = VAR_LIST;
		li->li_tv.v_قلف = 0;
		li->li_tv.vval.v_list = l2;
		إذا (l2 == عدم)
		    اذهب alloc_failed;
		++l2->lv_refcount;

		l2->lv_لومړی->li_tv.v_type = VAR_STRING;
		l2->lv_لومړی->li_tv.vval.v_string = vim_strsave(di->di_کلی);
		إذا (l2->lv_لومړی->li_tv.vval.v_string == عدم)
		    اذهب alloc_failed;
		copy_tv(&di->di_tv, &l2->lv_u.mat.lv_وروستی->li_tv);
	    }
	    li = li->li_بل;
	}
    }
    أرجع;

alloc_failed:
    // On a memory allocation failure some of the pre-allocated items may be
    // left unset; free the whole list and return an empty one instead.
    list_unref(l);
    (فراغ)rettv_list_alloc(rettv);
}

/*
 * "items(dict)" function
 */
    فراغ
dict2items(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_توکي);
}

/*
 * "keys()" function
 */
    فراغ
f_کلیګانې(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_کلیګانې);
}

/*
 * "values(dict)" function
 */
    فراغ
f_ارزښتونه(typval_T *argvars, typval_T *rettv)
{
    dict2list(argvars, rettv, DICT2_LIST_ارزښتونه);
}

/*
 * Make each item in the dict readonly (not the value of the item).
 */
    فراغ
dict_سېټ_توکي_ro(dict_T *di)
{
    صحيح		todo = (صحيح)di->dv_hashtab.ht_کارول_شوی;
    hashitem_T	*hi;

    // Set readonly
    FOR_ALL_HASHTAB_توکي(&di->dv_hashtab, hi, todo)
    {
	إذا (HASHITEM_تشه(hi))
	    استمر;
	--todo;
	HI2DI(hi)->di_flags |= DI_FLAGS_RO | DI_FLAGS_FIX;
    }
}

/*
 * "has_key()" function
 */
    فراغ
f_has_کلی(typval_T *argvars, typval_T *rettv)
{
    إذا (دننه_vim9script()
	    && (check_for_dict_arg(argvars, 0) == FAIL
		|| check_for_string_or_شمېره_arg(argvars, 1) == FAIL))
	أرجع;

    إذا (check_for_dict_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].vval.v_dict == عدم)
	أرجع;

    rettv->vval.v_شمېره = dict_has_کلی(argvars[0].vval.v_dict,
				(حرف *)tv_get_string(&argvars[1]));
}

#د_که_پای // defined(FEAT_EVAL)
