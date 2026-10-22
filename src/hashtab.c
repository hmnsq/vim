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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ 0
# define HT_DEBUG	// extra checks for table consistency  and statistics

ਸਥਿਰ ਲੰਮਾ hash_ਗਿਣਤੀ_lookup = 0;	// count number of hashtab lookups
ਸਥਿਰ ਲੰਮਾ hash_ਗਿਣਤੀ_perturb = 0;	// count number of "misses"
#ਜੇ_ਅੰਤ

// Magic value for algorithm that walks through the array.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERTURB_ਖਿਸਕਾਓ 5

ਸਥਿਰ ਪੂਰਨ hash_may_resize(hashtab_T *ht, ਪੂਰਨ minitems);

#ਜੇ 0 // currently not used
/*
 * Create an empty hash table.
 * Returns NULL when out of memory.
 */
    hashtab_T *
hash_create(ਖਾਲੀ)
{
    hashtab_T *ht;

    ht = ALLOC_ONE(hashtab_T);
    ਜੇ (ht != ਨਿਰਾ)
	hash_init(ht);
    ਵਾਪਸ ht;
}
#ਜੇ_ਅੰਤ

/*
 * Initialize an empty hash table.
 */
    ਖਾਲੀ
hash_init(hashtab_T *ht)
{
    // This zeroes all "ht_" entries and all the "hi_key" in "ht_smallarray".
    CLEAR_ਸੂਚਕ(ht);
    ht->ht_array = ht->ht_smallarray;
    ht->ht_ਨਕਾਬ = HT_INIT_ਮਾਪ - 1;
}

/*
 * If "ht->ht_flags" has HTFLAGS_FROZEN then give an error message using
 * "command" and return TRUE.
 */
    ਪੂਰਨ
check_hashtab_frozen(hashtab_T *ht, ਅੱਖਰ *command)
{
    ਜੇ ((ht->ht_flags & HTFLAGS_FROZEN) == 0)
	ਵਾਪਸ FALSE;

    semsg(_(e_not_allowed_ਮੰਜ਼ਿਲ_ਜੋੜੋ_or_remove_entries_str), command);
    ਵਾਪਸ TRUE;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * If "ht" is not freed then you should call hash_init() next!
 */
    ਖਾਲੀ
hash_clear(hashtab_T *ht)
{
    ਜੇ (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
}

/*
 * Free the array of a hash table and all the keys it contains.  The keys must
 * have been allocated.  "off" is the offset from the start of the allocate
 * memory to the location of the key (it's always positive).
 */
    ਖਾਲੀ
hash_clear_all(hashtab_T *ht, ਪੂਰਨ off)
{
    ਲੰਮਾ	todo;
    hashitem_T	*hi;

    todo = (ਲੰਮਾ)ht->ht_ਵਰਤਿਆ;
    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(ht, hi, todo)
    {
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	{
	    vim_free(hi->hi_ਕੁੰਜੀ - off);
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
hash_ਲੱਭੋ(hashtab_T *ht, char_u *ਕੁੰਜੀ)
{
    ਵਾਪਸ hash_lookup(ht, ਕੁੰਜੀ, hash_hash(ਕੁੰਜੀ));
}

/*
 * Like hash_find(), but caller computes "hash".
 */
    hashitem_T *
hash_lookup(hashtab_T *ht, char_u *ਕੁੰਜੀ, hash_T hash)
{
    hash_T	perturb;
    hashitem_T	*freeitem;
    hashitem_T	*hi;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	idx;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HT_DEBUG
    ++hash_ਗਿਣਤੀ_lookup;
#ਜੇ_ਅੰਤ

    /*
     * Quickly handle the most common situations:
     * - return if there is no item at all
     * - skip over a removed item
     * - return if the item matches
     */
    idx = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(hash & ht->ht_ਨਕਾਬ);
    hi = &ht->ht_array[idx];

    ਜੇ (hi->hi_ਕੁੰਜੀ == ਨਿਰਾ)
	ਵਾਪਸ hi;
    ਜੇ (hi->hi_ਕੁੰਜੀ == HI_ਕੁੰਜੀ_REMOVED)
	freeitem = hi;
    ਨਹੀਂਤਾਂ ਜੇ (hi->hi_hash == hash && STRCMP(hi->hi_ਕੁੰਜੀ, ਕੁੰਜੀ) == 0)
	ਵਾਪਸ hi;
    ਨਹੀਂਤਾਂ
	freeitem = ਨਿਰਾ;

    /*
     * Need to search through the table to find the key.  The algorithm
     * to step through the table starts with large steps, gradually becoming
     * smaller down to (1/4 table size + 1).  This means it goes through all
     * table entries in the end.
     * When we run into a NULL key it's clear that the key isn't there.
     * Return the first available slot found (can be a slot of a removed
     * item).
     */
    ਲਈ (perturb = hash; ; perturb >>= PERTURB_ਖਿਸਕਾਓ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HT_DEBUG
	++hash_ਗਿਣਤੀ_perturb;	    // count a "miss" for hashtab lookup
#ਜੇ_ਅੰਤ
	idx = (ਬਿਨਾਨਿਸ਼ਾਨੀ)((idx << 2U) + idx + perturb + 1U);
	hi = &ht->ht_array[idx & ht->ht_ਨਕਾਬ];
	ਜੇ (hi->hi_ਕੁੰਜੀ == ਨਿਰਾ)
	    ਵਾਪਸ freeitem == ਨਿਰਾ ? hi : freeitem;
	ਜੇ (hi->hi_hash == hash
		&& hi->hi_ਕੁੰਜੀ != HI_ਕੁੰਜੀ_REMOVED
		&& STRCMP(hi->hi_ਕੁੰਜੀ, ਕੁੰਜੀ) == 0)
	    ਵਾਪਸ hi;
	ਜੇ (hi->hi_ਕੁੰਜੀ == HI_ਕੁੰਜੀ_REMOVED && freeitem == ਨਿਰਾ)
	    freeitem = hi;
    }
}

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
/*
 * Print the efficiency of hashtable lookups.
 * Useful when trying different hash algorithms.
 * Called when exiting.
 */
    ਖਾਲੀ
hash_debug_results(ਖਾਲੀ)
{
# ifdef HT_DEBUG
    fprintf(stderr, "\r\n\r\n\r\n\r\n");
    fprintf(stderr, "Number of hashtable lookups: %ld\r\n", hash_ਗਿਣਤੀ_lookup);
    fprintf(stderr, "Number of perturb loops: %ld\r\n", hash_ਗਿਣਤੀ_perturb);
    fprintf(stderr, "Percentage of perturb loops: %ld%%\r\n",
				hash_ਗਿਣਤੀ_perturb * 100 / hash_ਗਿਣਤੀ_lookup);
# endif
}
#ਜੇ_ਅੰਤ

/*
 * Add item with key "key" to hashtable "ht".
 * "command" is used for the error message when the hashtab if frozen.
 * Returns FAIL when out of memory or the key is already present.
 */
    ਪੂਰਨ
hash_ਜੋੜੋ(hashtab_T *ht, char_u *ਕੁੰਜੀ, ਅੱਖਰ *command)
{
    hash_T	hash = hash_hash(ਕੁੰਜੀ);
    hashitem_T	*hi;

    ਜੇ (check_hashtab_frozen(ht, command))
	ਵਾਪਸ FAIL;
    hi = hash_lookup(ht, ਕੁੰਜੀ, hash);
    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
    {
	ਅੰਦਰੂਨੀ_error("hash_add()");
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ hash_ਜੋੜੋ_ਵਸਤੂ(ht, hi, ਕੁੰਜੀ, hash);
}

/*
 * Add item "hi" with "key" to hashtable "ht".  "key" must not be NULL and
 * "hi" must have been obtained with hash_lookup() and point to an empty item.
 * "hi" is invalid after this!
 * Returns OK or FAIL (out of memory).
 */
    ਪੂਰਨ
hash_ਜੋੜੋ_ਵਸਤੂ(
    hashtab_T	*ht,
    hashitem_T	*hi,
    char_u	*ਕੁੰਜੀ,
    hash_T	hash)
{
    // If resizing failed before and it fails again we can't add an item.
    ਜੇ (ht->ht_flags & HTFLAGS_ERROR)
	ਵਾਪਸ FAIL;

    ++ht->ht_ਵਰਤਿਆ;
    ++ht->ht_ਬਦਲਿਆ;
    ਜੇ (hi->hi_ਕੁੰਜੀ == ਨਿਰਾ)
	++ht->ht_filled;
    hi->hi_ਕੁੰਜੀ = ਕੁੰਜੀ;
    hi->hi_hash = hash;

    // When the space gets low may resize the array.
    ਵਾਪਸ hash_may_resize(ht, 0);
}

#ਜੇ 0  // not used
/*
 * Overwrite hashtable item "hi" with "key".  "hi" must point to the item that
 * is to be overwritten.  Thus the number of items in the hashtable doesn't
 * change.
 * Although the key must be identical, the pointer may be different, thus it's
 * set anyway (the key is part of an item with that key).
 * The caller must take care of freeing the old item.
 * "hi" is invalid after this!
 */
    ਖਾਲੀ
hash_ਸੈੱਟ(hashitem_T *hi, char_u *ਕੁੰਜੀ)
{
    hi->hi_ਕੁੰਜੀ = ਕੁੰਜੀ;
}
#ਜੇ_ਅੰਤ

/*
 * Remove item "hi" from  hashtable "ht".  "hi" must have been obtained with
 * hash_lookup().
 * "command" is used for the error message when the hashtab if frozen.
 * The caller must take care of freeing the item itself.
 */
    ਪੂਰਨ
hash_remove(hashtab_T *ht, hashitem_T *hi, ਅੱਖਰ *command)
{
    ਜੇ (check_hashtab_frozen(ht, command))
	ਵਾਪਸ FAIL;
    --ht->ht_ਵਰਤਿਆ;
    ++ht->ht_ਬਦਲਿਆ;
    hi->hi_ਕੁੰਜੀ = HI_ਕੁੰਜੀ_REMOVED;
    hash_may_resize(ht, 0);
    ਵਾਪਸ OK;
}

/*
 * Lock a hashtable: prevent that ht_array changes.
 * Don't use this when items are to be added!
 * Must call hash_unlock() later.
 */
    ਖਾਲੀ
hash_ਤਾਲਾ(hashtab_T *ht)
{
    ++ht->ht_locked;
}

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * Lock a hashtable at the specified number of entries.
 * Caller must make sure no more than "size" entries will be added.
 * Must call hash_unlock() later.
 */
    ਖਾਲੀ
hash_ਤਾਲਾ_ਮਾਪ(hashtab_T *ht, ਪੂਰਨ ਮਾਪ)
{
    (ਖਾਲੀ)hash_may_resize(ht, ਮਾਪ);
    ++ht->ht_locked;
}
#ਜੇ_ਅੰਤ

/*
 * Unlock a hashtable: allow ht_array changes again.
 * Table will be resized (shrink) when necessary.
 * This must balance a call to hash_lock().
 */
    ਖਾਲੀ
hash_unlock(hashtab_T *ht)
{
    --ht->ht_locked;
    (ਖਾਲੀ)hash_may_resize(ht, 0);
}

/*
 * Shrink a hashtable when there is too much empty space.
 * Grow a hashtable when there is not enough empty space.
 * Returns OK or FAIL (out of memory).
 */
    ਸਥਿਰ ਪੂਰਨ
hash_may_resize(
    hashtab_T	*ht,
    ਪੂਰਨ		minitems)		// minimal number of items
{
    hashitem_T	temparray[HT_INIT_ਮਾਪ];
    hashitem_T	*oldarray, *newarray;
    hashitem_T	*olditem, *newitem;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	newi;
    ਪੂਰਨ		todo;
    long_u	newsize;
    long_u	minsize;
    long_u	newmask;
    hash_T	perturb;

    // Don't resize a locked table.
    ਜੇ (ht->ht_locked > 0)
	ਵਾਪਸ OK;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HT_DEBUG
    ਜੇ (ht->ht_ਵਰਤਿਆ > ht->ht_filled)
	emsg("hash_may_resize(): more used than filled");
    ਜੇ (ht->ht_filled >= ht->ht_ਨਕਾਬ + 1)
	emsg("hash_may_resize(): table completely filled");
#ਜੇ_ਅੰਤ

    long_u oldsize = ht->ht_ਨਕਾਬ + 1;
    ਜੇ (minitems == 0)
    {
	// Return quickly for small tables with at least two NULL items.  NULL
	// items are required for the lookup to decide a key isn't there.
	ਜੇ (ht->ht_filled < HT_INIT_ਮਾਪ - 1
					 && ht->ht_array == ht->ht_smallarray)
	    ਵਾਪਸ OK;

	/*
	 * Grow or refill the array when it's more than 2/3 full (including
	 * removed items, so that they get cleaned up).
	 * Shrink the array when it's less than 1/5 full.  When growing it is
	 * at least 1/4 full (avoids repeated grow-shrink operations)
	 */
	ਜੇ (ht->ht_filled * 3 < oldsize * 2 && ht->ht_ਵਰਤਿਆ > oldsize / 5)
	    ਵਾਪਸ OK;

	ਜੇ (ht->ht_ਵਰਤਿਆ > 1000)
	    minsize = ht->ht_ਵਰਤਿਆ * 2;  // it's big, don't make too much room
	ਨਹੀਂਤਾਂ
	    minsize = ht->ht_ਵਰਤਿਆ * 4;  // make plenty of room
    }
    ਨਹੀਂਤਾਂ
    {
	// Use specified size.
	ਜੇ ((long_u)minitems < ht->ht_ਵਰਤਿਆ)	// just in case...
	    minitems = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
	minsize = (minitems * 3 + 1) / 2;	// array is up to 2/3 full
    }

    newsize = HT_INIT_ਮਾਪ;
    ਜਦੋਂ (newsize < minsize)
    {
	newsize <<= 1;		// make sure it's always a power of 2
	ਜੇ (newsize == 0)
	    ਵਾਪਸ FAIL;	//ਉਛਾਲ
    }

    ਜੇ (newsize == HT_INIT_ਮਾਪ)
    {
	// Use the small array inside the hashdict structure.
	newarray = ht->ht_smallarray;
	ਜੇ (ht->ht_array == newarray)
	{
	    // Moving from ht_smallarray to ht_smallarray!  Happens when there
	    // are many removed items.  Copy the items to be able to clean up
	    // removed items.
	    mch_memmove(temparray, newarray, ਆਕਾਰ(temparray));
	    oldarray = temparray;
	}
	ਨਹੀਂਤਾਂ
	    oldarray = ht->ht_array;
	CLEAR_ਖੇਤਰ(ht->ht_smallarray);
    }

    ਨਹੀਂਤਾਂ ਜੇ (newsize == oldsize && ht->ht_filled * 3 < oldsize * 2)
    {
	// The hashtab is already at the desired size, and there are not too
	// many removed items, bail out.
	ਵਾਪਸ OK;
    }

    ਨਹੀਂਤਾਂ
    {
	// Allocate an array.
	newarray = ALLOC_CLEAR_MULT(hashitem_T, newsize);
	ਜੇ (newarray == ਨਿਰਾ)
	{
	    // Out of memory.  When there are NULL items still return OK.
	    // Otherwise set ht_flags to HTFLAGS_ERROR, because lookup may
	    // result in a hang if we add another item.
	    ਜੇ (ht->ht_filled < ht->ht_ਨਕਾਬ)
		ਵਾਪਸ OK;
	    ht->ht_flags |= HTFLAGS_ERROR;
	    ਵਾਪਸ FAIL;
	}
	oldarray = ht->ht_array;
    }

    /*
     * Move all the items from the old array to the new one, placing them in
     * the right spot.  The new array won't have any removed items, thus this
     * is also a cleanup action.
     */
    newmask = newsize - 1;
    todo = (ਪੂਰਨ)ht->ht_ਵਰਤਿਆ;
    ਲਈ (olditem = oldarray; todo > 0; ++olditem)
	ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(olditem))
	{
	    /*
	     * The algorithm to find the spot to add the item is identical to
	     * the algorithm to find an item in hash_lookup().  But we only
	     * need to search for a NULL key, thus it's simpler.
	     */
	    newi = (ਬਿਨਾਨਿਸ਼ਾਨੀ)(olditem->hi_hash & newmask);
	    newitem = &newarray[newi];

	    ਜੇ (newitem->hi_ਕੁੰਜੀ != ਨਿਰਾ)
		ਲਈ (perturb = olditem->hi_hash; ; perturb >>= PERTURB_ਖਿਸਕਾਓ)
		{
		    newi = (ਬਿਨਾਨਿਸ਼ਾਨੀ)((newi << 2U) + newi + perturb + 1U);
		    newitem = &newarray[newi & newmask];
		    ਜੇ (newitem->hi_ਕੁੰਜੀ == ਨਿਰਾ)
			ਰੋਕੋ;
		}
	    *newitem = *olditem;
	    --todo;
	}

    ਜੇ (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
    ht->ht_array = newarray;
    ht->ht_ਨਕਾਬ = newmask;
    ht->ht_filled = ht->ht_ਵਰਤਿਆ;
    ++ht->ht_ਬਦਲਿਆ;
    ht->ht_flags &= ~HTFLAGS_ERROR;

    ਵਾਪਸ OK;
}

/*
 * Get the hash number for a key.
 * If you think you know a better hash function: Compile with HT_DEBUG set and
 * run a script that uses hashtables a lot.  Vim will then print statistics
 * when exiting.  Try that with the current hash algorithm and yours.  The
 * lower the percentage the better.
 */
    hash_T
hash_hash(char_u *ਕੁੰਜੀ)
{
    hash_T	hash;
    char_u	*p;

    ਜੇ ((hash = *ਕੁੰਜੀ) == 0)
	ਵਾਪਸ (hash_T)0;
    p = ਕੁੰਜੀ + 1;

    // A simplistic algorithm that appears to do very well.
    // Suggested by George Reilly.
    ਜਦੋਂ (*p != NUL)
	hash = hash * 101 + *p++;

    ਵਾਪਸ hash;
}
