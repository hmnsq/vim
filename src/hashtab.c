/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * hashtab.c: Handling of a hashtable with Vim-specific properties.
 *
 * Each item in a hashtable has a NUL terminated string key.  A key can appear
 * only once in the table.
 *
 * A hash number is computed from the key for quick lookup.  When the hashes
 * of two different keys point to the same entry an algorithm is used to
 * iterate over other entries in the table until the right one is found.
 * To make the iteration work removed keys are different from entries where a
 * key was never present.
 *
 * The mechanism has been partly based on how Python Dictionaries are
 * implemented.  The algorithm is from Knuth Vol. 3, Sec. 6.4.
 *
 * The hashtable grows to accommodate more entries when needed.  At least 1/3
 * of the entries is empty to keep the lookup efficient (at the cost of extra
 * memory).
 */

#شامل "vim.h"

#اگر 0
# define HT_DEBUG	// extra checks for table consistency  and statistics

ساکن طویل hash_گنتی_lookup = 0;	// count number of hashtab lookups
ساکن طویل hash_گنتی_perturb = 0;	// count number of "misses"
#اگر_ختم

// Magic value for algorithm that walks through the array.
#تعریف_کرو PERTURB_سرکاؤ 5

ساکن صحیح hash_may_resize(hashtab_T *ht, صحیح minitems);

#اگر 0 // currently not used
/*
 * Create an empty hash table.
 * Returns NULL when out of memory.
 */
    hashtab_T *
hash_create(خالی)
{
    hashtab_T *ht;

    ht = ALLOC_ONE(hashtab_T);
    اگر (ht != تہی)
	hash_init(ht);
    واپس ht;
}
#اگر_ختم

/*
 * Initialize an empty hash table.
 */
    خالی
hash_init(hashtab_T *ht)
{
    // This zeroes all "ht_" entries and all the "hi_key" in "ht_smallarray".
    CLEAR_اشارہ_گر(ht);
    ht->ht_array = ht->ht_smallarray;
    ht->ht_نقاب = HT_INIT_سائز - 1;
}

/*
 * If "ht->ht_flags" has HTFLAGS_FROZEN then give an error message using
 * "command" and return TRUE.
 */
    صحیح
check_hashtab_frozen(hashtab_T *ht, حرف *command)
{
    اگر ((ht->ht_flags & HTFLAGS_FROZEN) == 0)
	واپس FALSE;

    semsg(_(e_not_allowed_مقصد_جوڑو_or_remove_entries_str), command);
    واپس TRUE;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * If "ht" is not freed then you should call hash_init() next!
 */
    خالی
hash_clear(hashtab_T *ht)
{
    اگر (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
}

/*
 * Free the array of a hash table and all the keys it contains.  The keys must
 * have been allocated.  "off" is the offset from the start of the allocate
 * memory to the location of the key (it's always positive).
 */
    خالی
hash_clear_all(hashtab_T *ht, صحیح off)
{
    طویل	todo;
    hashitem_T	*hi;

    todo = (طویل)ht->ht_مستعمل;
    FOR_ALL_HASHTAB_اشیاء(ht, hi, todo)
    {
	اگر (!HASHITEM_خالی_خانے(hi))
	{
	    vim_free(hi->hi_کلید - off);
	    --todo;
	}
    }
    hash_clear(ht);
}

/*
 * Find "key" in hashtable "ht".  "key" must not be NULL.
 * Always returns a pointer to a hashitem.  If the item was not found then
 * HASHITEM_EMPTY() is TRUE.  The pointer is then the place where the key
 * would be added.
 * WARNING: The returned pointer becomes invalid when the hashtable is changed
 * (adding, setting or removing an item)!
 */
    hashitem_T *
hash_ڈھونڈو(hashtab_T *ht, char_u *کلید)
{
    واپس hash_lookup(ht, کلید, hash_hash(کلید));
}

/*
 * Like hash_find(), but caller computes "hash".
 */
    hashitem_T *
hash_lookup(hashtab_T *ht, char_u *کلید, hash_T hash)
{
    hash_T	perturb;
    hashitem_T	*freeitem;
    hashitem_T	*hi;
    غیرعلامتی	idx;

#اگر_تعریف_شدہ HT_DEBUG
    ++hash_گنتی_lookup;
#اگر_ختم

    /*
     * Quickly handle the most common situations:
     * - return if there is no item at all
     * - skip over a removed item
     * - return if the item matches
     */
    idx = (غیرعلامتی)(hash & ht->ht_نقاب);
    hi = &ht->ht_array[idx];

    اگر (hi->hi_کلید == تہی)
	واپس hi;
    اگر (hi->hi_کلید == HI_کلید_REMOVED)
	freeitem = hi;
    ورنہ اگر (hi->hi_hash == hash && STRCMP(hi->hi_کلید, کلید) == 0)
	واپس hi;
    ورنہ
	freeitem = تہی;

    /*
     * Need to search through the table to find the key.  The algorithm
     * to step through the table starts with large steps, gradually becoming
     * smaller down to (1/4 table size + 1).  This means it goes through all
     * table entries in the end.
     * When we run into a NULL key it's clear that the key isn't there.
     * Return the first available slot found (can be a slot of a removed
     * item).
     */
    برائے (perturb = hash; ; perturb >>= PERTURB_سرکاؤ)
    {
#اگر_تعریف_شدہ HT_DEBUG
	++hash_گنتی_perturb;	    // count a "miss" for hashtab lookup
#اگر_ختم
	idx = (غیرعلامتی)((idx << 2U) + idx + perturb + 1U);
	hi = &ht->ht_array[idx & ht->ht_نقاب];
	اگر (hi->hi_کلید == تہی)
	    واپس freeitem == تہی ? hi : freeitem;
	اگر (hi->hi_hash == hash
		&& hi->hi_کلید != HI_کلید_REMOVED
		&& STRCMP(hi->hi_کلید, کلید) == 0)
	    واپس hi;
	اگر (hi->hi_کلید == HI_کلید_REMOVED && freeitem == تہی)
	    freeitem = hi;
    }
}

#اگر defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
/*
 * Print the efficiency of hashtable lookups.
 * Useful when trying different hash algorithms.
 * Called when exiting.
 */
    خالی
hash_debug_results(خالی)
{
# ifdef HT_DEBUG
    fprintf(stderr, "\r\n\r\n\r\n\r\n");
    fprintf(stderr, "Number of hashtable lookups: %ld\r\n", hash_گنتی_lookup);
    fprintf(stderr, "Number of perturb loops: %ld\r\n", hash_گنتی_perturb);
    fprintf(stderr, "Percentage of perturb loops: %ld%%\r\n",
				hash_گنتی_perturb * 100 / hash_گنتی_lookup);
# endif
}
#اگر_ختم

/*
 * Add item with key "key" to hashtable "ht".
 * "command" is used for the error message when the hashtab if frozen.
 * Returns FAIL when out of memory or the key is already present.
 */
    صحیح
hash_جوڑو(hashtab_T *ht, char_u *کلید, حرف *command)
{
    hash_T	hash = hash_hash(کلید);
    hashitem_T	*hi;

    اگر (check_hashtab_frozen(ht, command))
	واپس FAIL;
    hi = hash_lookup(ht, کلید, hash);
    اگر (!HASHITEM_خالی_خانے(hi))
    {
	اندرونی_error("hash_add()");
	واپس FAIL;
    }
    واپس hash_جوڑو_شے(ht, hi, کلید, hash);
}

/*
 * Add item "hi" with "key" to hashtable "ht".  "key" must not be NULL and
 * "hi" must have been obtained with hash_lookup() and point to an empty item.
 * "hi" is invalid after this!
 * Returns OK or FAIL (out of memory).
 */
    صحیح
hash_جوڑو_شے(
    hashtab_T	*ht,
    hashitem_T	*hi,
    char_u	*کلید,
    hash_T	hash)
{
    // If resizing failed before and it fails again we can't add an item.
    اگر (ht->ht_flags & HTFLAGS_ERROR)
	واپس FAIL;

    ++ht->ht_مستعمل;
    ++ht->ht_بدلا;
    اگر (hi->hi_کلید == تہی)
	++ht->ht_filled;
    hi->hi_کلید = کلید;
    hi->hi_hash = hash;

    // When the space gets low may resize the array.
    واپس hash_may_resize(ht, 0);
}

#اگر 0  // not used
/*
 * Overwrite hashtable item "hi" with "key".  "hi" must point to the item that
 * is to be overwritten.  Thus the number of items in the hashtable doesn't
 * change.
 * Although the key must be identical, the pointer may be different, thus it's
 * set anyway (the key is part of an item with that key).
 * The caller must take care of freeing the old item.
 * "hi" is invalid after this!
 */
    خالی
hash_سیٹ(hashitem_T *hi, char_u *کلید)
{
    hi->hi_کلید = کلید;
}
#اگر_ختم

/*
 * Remove item "hi" from  hashtable "ht".  "hi" must have been obtained with
 * hash_lookup().
 * "command" is used for the error message when the hashtab if frozen.
 * The caller must take care of freeing the item itself.
 */
    صحیح
hash_remove(hashtab_T *ht, hashitem_T *hi, حرف *command)
{
    اگر (check_hashtab_frozen(ht, command))
	واپس FAIL;
    --ht->ht_مستعمل;
    ++ht->ht_بدلا;
    hi->hi_کلید = HI_کلید_REMOVED;
    hash_may_resize(ht, 0);
    واپس OK;
}

/*
 * Lock a hashtable: prevent that ht_array changes.
 * Don't use this when items are to be added!
 * Must call hash_unlock() later.
 */
    خالی
hash_تالا(hashtab_T *ht)
{
    ++ht->ht_locked;
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * Lock a hashtable at the specified number of entries.
 * Caller must make sure no more than "size" entries will be added.
 * Must call hash_unlock() later.
 */
    خالی
hash_تالا_سائز(hashtab_T *ht, صحیح سائز)
{
    (خالی)hash_may_resize(ht, سائز);
    ++ht->ht_locked;
}
#اگر_ختم

/*
 * Unlock a hashtable: allow ht_array changes again.
 * Table will be resized (shrink) when necessary.
 * This must balance a call to hash_lock().
 */
    خالی
hash_unlock(hashtab_T *ht)
{
    --ht->ht_locked;
    (خالی)hash_may_resize(ht, 0);
}

/*
 * Shrink a hashtable when there is too much empty space.
 * Grow a hashtable when there is not enough empty space.
 * Returns OK or FAIL (out of memory).
 */
    ساکن صحیح
hash_may_resize(
    hashtab_T	*ht,
    صحیح		minitems)		// minimal number of items
{
    hashitem_T	temparray[HT_INIT_سائز];
    hashitem_T	*oldarray, *newarray;
    hashitem_T	*olditem, *newitem;
    غیرعلامتی	newi;
    صحیح		todo;
    long_u	newsize;
    long_u	minsize;
    long_u	newmask;
    hash_T	perturb;

    // Don't resize a locked table.
    اگر (ht->ht_locked > 0)
	واپس OK;

#اگر_تعریف_شدہ HT_DEBUG
    اگر (ht->ht_مستعمل > ht->ht_filled)
	emsg("hash_may_resize(): more used than filled");
    اگر (ht->ht_filled >= ht->ht_نقاب + 1)
	emsg("hash_may_resize(): table completely filled");
#اگر_ختم

    long_u oldsize = ht->ht_نقاب + 1;
    اگر (minitems == 0)
    {
	// Return quickly for small tables with at least two NULL items.  NULL
	// items are required for the lookup to decide a key isn't there.
	اگر (ht->ht_filled < HT_INIT_سائز - 1
					 && ht->ht_array == ht->ht_smallarray)
	    واپس OK;

	/*
	 * Grow or refill the array when it's more than 2/3 full (including
	 * removed items, so that they get cleaned up).
	 * Shrink the array when it's less than 1/5 full.  When growing it is
	 * at least 1/4 full (avoids repeated grow-shrink operations)
	 */
	اگر (ht->ht_filled * 3 < oldsize * 2 && ht->ht_مستعمل > oldsize / 5)
	    واپس OK;

	اگر (ht->ht_مستعمل > 1000)
	    minsize = ht->ht_مستعمل * 2;  // it's big, don't make too much room
	ورنہ
	    minsize = ht->ht_مستعمل * 4;  // make plenty of room
    }
    ورنہ
    {
	// Use specified size.
	اگر ((long_u)minitems < ht->ht_مستعمل)	// just in case...
	    minitems = (صحیح)ht->ht_مستعمل;
	minsize = (minitems * 3 + 1) / 2;	// array is up to 2/3 full
    }

    newsize = HT_INIT_سائز;
    جب (newsize < minsize)
    {
	newsize <<= 1;		// make sure it's always a power of 2
	اگر (newsize == 0)
	    واپس FAIL;	//حد_سے_تجاوز
    }

    اگر (newsize == HT_INIT_سائز)
    {
	// Use the small array inside the hashdict structure.
	newarray = ht->ht_smallarray;
	اگر (ht->ht_array == newarray)
	{
	    // Moving from ht_smallarray to ht_smallarray!  Happens when there
	    // are many removed items.  Copy the items to be able to clean up
	    // removed items.
	    mch_memmove(temparray, newarray, حجم(temparray));
	    oldarray = temparray;
	}
	ورنہ
	    oldarray = ht->ht_array;
	CLEAR_میدان(ht->ht_smallarray);
    }

    ورنہ اگر (newsize == oldsize && ht->ht_filled * 3 < oldsize * 2)
    {
	// The hashtab is already at the desired size, and there are not too
	// many removed items, bail out.
	واپس OK;
    }

    ورنہ
    {
	// Allocate an array.
	newarray = ALLOC_CLEAR_MULT(hashitem_T, newsize);
	اگر (newarray == تہی)
	{
	    // Out of memory.  When there are NULL items still return OK.
	    // Otherwise set ht_flags to HTFLAGS_ERROR, because lookup may
	    // result in a hang if we add another item.
	    اگر (ht->ht_filled < ht->ht_نقاب)
		واپس OK;
	    ht->ht_flags |= HTFLAGS_ERROR;
	    واپس FAIL;
	}
	oldarray = ht->ht_array;
    }

    /*
     * Move all the items from the old array to the new one, placing them in
     * the right spot.  The new array won't have any removed items, thus this
     * is also a cleanup action.
     */
    newmask = newsize - 1;
    todo = (صحیح)ht->ht_مستعمل;
    برائے (olditem = oldarray; todo > 0; ++olditem)
	اگر (!HASHITEM_خالی_خانے(olditem))
	{
	    /*
	     * The algorithm to find the spot to add the item is identical to
	     * the algorithm to find an item in hash_lookup().  But we only
	     * need to search for a NULL key, thus it's simpler.
	     */
	    newi = (غیرعلامتی)(olditem->hi_hash & newmask);
	    newitem = &newarray[newi];

	    اگر (newitem->hi_کلید != تہی)
		برائے (perturb = olditem->hi_hash; ; perturb >>= PERTURB_سرکاؤ)
		{
		    newi = (غیرعلامتی)((newi << 2U) + newi + perturb + 1U);
		    newitem = &newarray[newi & newmask];
		    اگر (newitem->hi_کلید == تہی)
			توڑو;
		}
	    *newitem = *olditem;
	    --todo;
	}

    اگر (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
    ht->ht_array = newarray;
    ht->ht_نقاب = newmask;
    ht->ht_filled = ht->ht_مستعمل;
    ++ht->ht_بدلا;
    ht->ht_flags &= ~HTFLAGS_ERROR;

    واپس OK;
}

/*
 * Get the hash number for a key.
 * If you think you know a better hash function: Compile with HT_DEBUG set and
 * run a script that uses hashtables a lot.  Vim will then print statistics
 * when exiting.  Try that with the current hash algorithm and yours.  The
 * lower the percentage the better.
 */
    hash_T
hash_hash(char_u *کلید)
{
    hash_T	hash;
    char_u	*p;

    اگر ((hash = *کلید) == 0)
	واپس (hash_T)0;
    p = کلید + 1;

    // A simplistic algorithm that appears to do very well.
    // Suggested by George Reilly.
    جب (*p != NUL)
	hash = hash * 101 + *p++;

    واپس hash;
}
