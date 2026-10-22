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

#அந்தர்பாவ "vim.h"

#எனில் 0
# define HT_DEBUG	// extra checks for table consistency  and statistics

நிலையான long hash_மொத்தம்_lookup = 0;	// count number of hashtab lookups
நிலையான long hash_மொத்தம்_perturb = 0;	// count number of "misses"
#endif

// Magic value for algorithm that walks through the array.
#define PERTURB_SHIFT 5

நிலையான முழு hash_may_resize(hashtab_T *ht, முழு minitems);

#எனில் 0 // currently not used
/*
 * Create an empty hash table.
 * Returns NULL when out of memory.
 */
    hashtab_T *
hash_create(வெறுமை)
{
    hashtab_T *ht;

    ht = ALLOC_ONE(hashtab_T);
    எனில் (ht != வெற்று)
	hash_init(ht);
    திரும்பு ht;
}
#endif

/*
 * Initialize an empty hash table.
 */
    வெறுமை
hash_init(hashtab_T *ht)
{
    // This zeroes all "ht_" entries and all the "hi_key" in "ht_smallarray".
    CLEAR_POINTER(ht);
    ht->ht_array = ht->ht_smallarray;
    ht->ht_mask = HT_INIT_SIZE - 1;
}

/*
 * If "ht->ht_flags" has HTFLAGS_FROZEN then give an error message using
 * "command" and return TRUE.
 */
    முழு
check_hashtab_frozen(hashtab_T *ht, எழுத்து *command)
{
    எனில் ((ht->ht_flags & HTFLAGS_FROZEN) == 0)
	திரும்பு FALSE;

    semsg(_(e_not_allowed_to_add_or_remove_entries_str), command);
    திரும்பு TRUE;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * If "ht" is not freed then you should call hash_init() next!
 */
    வெறுமை
hash_clear(hashtab_T *ht)
{
    எனில் (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
}

/*
 * Free the array of a hash table and all the keys it contains.  The keys must
 * have been allocated.  "off" is the offset from the start of the allocate
 * memory to the location of the key (it's always positive).
 */
    வெறுமை
hash_clear_all(hashtab_T *ht, முழு off)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_பொருட்கள்(ht, hi, todo)
    {
	எனில் (!HASHITEM_EMPTY(hi))
	{
	    vim_free(hi->hi_சாவி - off);
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
hash_find(hashtab_T *ht, char_u *சாவி)
{
    திரும்பு hash_lookup(ht, சாவி, hash_hash(சாவி));
}

/*
 * Like hash_find(), but caller computes "hash".
 */
    hashitem_T *
hash_lookup(hashtab_T *ht, char_u *சாவி, hash_T hash)
{
    hash_T	perturb;
    hashitem_T	*freeitem;
    hashitem_T	*hi;
    unsigned	idx;

#ifdef HT_DEBUG
    ++hash_மொத்தம்_lookup;
#endif

    /*
     * Quickly handle the most common situations:
     * - return if there is no item at all
     * - skip over a removed item
     * - return if the item matches
     */
    idx = (unsigned)(hash & ht->ht_mask);
    hi = &ht->ht_array[idx];

    எனில் (hi->hi_சாவி == வெற்று)
	திரும்பு hi;
    எனில் (hi->hi_சாவி == HI_சாவி_REMOVED)
	freeitem = hi;
    இல்லையெனில் எனில் (hi->hi_hash == hash && STRCMP(hi->hi_சாவி, சாவி) == 0)
	திரும்பு hi;
    இல்லையெனில்
	freeitem = வெற்று;

    /*
     * Need to search through the table to find the key.  The algorithm
     * to step through the table starts with large steps, gradually becoming
     * smaller down to (1/4 table size + 1).  This means it goes through all
     * table entries in the end.
     * When we run into a NULL key it's clear that the key isn't there.
     * Return the first available slot found (can be a slot of a removed
     * item).
     */
    க்கு (perturb = hash; ; perturb >>= PERTURB_SHIFT)
    {
#ifdef HT_DEBUG
	++hash_மொத்தம்_perturb;	    // count a "miss" for hashtab lookup
#endif
	idx = (unsigned)((idx << 2U) + idx + perturb + 1U);
	hi = &ht->ht_array[idx & ht->ht_mask];
	எனில் (hi->hi_சாவி == வெற்று)
	    திரும்பு freeitem == வெற்று ? hi : freeitem;
	எனில் (hi->hi_hash == hash
		&& hi->hi_சாவி != HI_சாவி_REMOVED
		&& STRCMP(hi->hi_சாவி, சாவி) == 0)
	    திரும்பு hi;
	எனில் (hi->hi_சாவி == HI_சாவி_REMOVED && freeitem == வெற்று)
	    freeitem = hi;
    }
}

#எனில் defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
/*
 * Print the efficiency of hashtable lookups.
 * Useful when trying different hash algorithms.
 * Called when exiting.
 */
    வெறுமை
hash_debug_results(வெறுமை)
{
# ifdef HT_DEBUG
    fprintf(stderr, "\r\n\r\n\r\n\r\n");
    fprintf(stderr, "Number of hashtable lookups: %ld\r\n", hash_மொத்தம்_lookup);
    fprintf(stderr, "Number of perturb loops: %ld\r\n", hash_மொத்தம்_perturb);
    fprintf(stderr, "Percentage of perturb loops: %ld%%\r\n",
				hash_மொத்தம்_perturb * 100 / hash_மொத்தம்_lookup);
# endif
}
#endif

/*
 * Add item with key "key" to hashtable "ht".
 * "command" is used for the error message when the hashtab if frozen.
 * Returns FAIL when out of memory or the key is already present.
 */
    முழு
hash_add(hashtab_T *ht, char_u *சாவி, எழுத்து *command)
{
    hash_T	hash = hash_hash(சாவி);
    hashitem_T	*hi;

    எனில் (check_hashtab_frozen(ht, command))
	திரும்பு FAIL;
    hi = hash_lookup(ht, சாவி, hash);
    எனில் (!HASHITEM_EMPTY(hi))
    {
	internal_error("hash_add()");
	திரும்பு FAIL;
    }
    திரும்பு hash_add_item(ht, hi, சாவி, hash);
}

/*
 * Add item "hi" with "key" to hashtable "ht".  "key" must not be NULL and
 * "hi" must have been obtained with hash_lookup() and point to an empty item.
 * "hi" is invalid after this!
 * Returns OK or FAIL (out of memory).
 */
    முழு
hash_add_item(
    hashtab_T	*ht,
    hashitem_T	*hi,
    char_u	*சாவி,
    hash_T	hash)
{
    // If resizing failed before and it fails again we can't add an item.
    எனில் (ht->ht_flags & HTFLAGS_ERROR)
	திரும்பு FAIL;

    ++ht->ht_used;
    ++ht->ht_changed;
    எனில் (hi->hi_சாவி == வெற்று)
	++ht->ht_filled;
    hi->hi_சாவி = சாவி;
    hi->hi_hash = hash;

    // When the space gets low may resize the array.
    திரும்பு hash_may_resize(ht, 0);
}

#எனில் 0  // not used
/*
 * Overwrite hashtable item "hi" with "key".  "hi" must point to the item that
 * is to be overwritten.  Thus the number of items in the hashtable doesn't
 * change.
 * Although the key must be identical, the pointer may be different, thus it's
 * set anyway (the key is part of an item with that key).
 * The caller must take care of freeing the old item.
 * "hi" is invalid after this!
 */
    வெறுமை
hash_set(hashitem_T *hi, char_u *சாவி)
{
    hi->hi_சாவி = சாவி;
}
#endif

/*
 * Remove item "hi" from  hashtable "ht".  "hi" must have been obtained with
 * hash_lookup().
 * "command" is used for the error message when the hashtab if frozen.
 * The caller must take care of freeing the item itself.
 */
    முழு
hash_remove(hashtab_T *ht, hashitem_T *hi, எழுத்து *command)
{
    எனில் (check_hashtab_frozen(ht, command))
	திரும்பு FAIL;
    --ht->ht_used;
    ++ht->ht_changed;
    hi->hi_சாவி = HI_சாவி_REMOVED;
    hash_may_resize(ht, 0);
    திரும்பு OK;
}

/*
 * Lock a hashtable: prevent that ht_array changes.
 * Don't use this when items are to be added!
 * Must call hash_unlock() later.
 */
    வெறுமை
hash_lock(hashtab_T *ht)
{
    ++ht->ht_locked;
}

#எனில் defined(FEAT_PROP_POPUP)
/*
 * Lock a hashtable at the specified number of entries.
 * Caller must make sure no more than "size" entries will be added.
 * Must call hash_unlock() later.
 */
    வெறுமை
hash_lock_size(hashtab_T *ht, முழு size)
{
    (வெறுமை)hash_may_resize(ht, size);
    ++ht->ht_locked;
}
#endif

/*
 * Unlock a hashtable: allow ht_array changes again.
 * Table will be resized (shrink) when necessary.
 * This must balance a call to hash_lock().
 */
    வெறுமை
hash_unlock(hashtab_T *ht)
{
    --ht->ht_locked;
    (வெறுமை)hash_may_resize(ht, 0);
}

/*
 * Shrink a hashtable when there is too much empty space.
 * Grow a hashtable when there is not enough empty space.
 * Returns OK or FAIL (out of memory).
 */
    நிலையான முழு
hash_may_resize(
    hashtab_T	*ht,
    முழு		minitems)		// minimal number of items
{
    hashitem_T	temparray[HT_INIT_SIZE];
    hashitem_T	*oldarray, *newarray;
    hashitem_T	*olditem, *newitem;
    unsigned	newi;
    முழு		todo;
    long_u	newsize;
    long_u	minsize;
    long_u	newmask;
    hash_T	perturb;

    // Don't resize a locked table.
    எனில் (ht->ht_locked > 0)
	திரும்பு OK;

#ifdef HT_DEBUG
    எனில் (ht->ht_used > ht->ht_filled)
	emsg("hash_may_resize(): more used than filled");
    எனில் (ht->ht_filled >= ht->ht_mask + 1)
	emsg("hash_may_resize(): table completely filled");
#endif

    long_u oldsize = ht->ht_mask + 1;
    எனில் (minitems == 0)
    {
	// Return quickly for small tables with at least two NULL items.  NULL
	// items are required for the lookup to decide a key isn't there.
	எனில் (ht->ht_filled < HT_INIT_SIZE - 1
					 && ht->ht_array == ht->ht_smallarray)
	    திரும்பு OK;

	/*
	 * Grow or refill the array when it's more than 2/3 full (including
	 * removed items, so that they get cleaned up).
	 * Shrink the array when it's less than 1/5 full.  When growing it is
	 * at least 1/4 full (avoids repeated grow-shrink operations)
	 */
	எனில் (ht->ht_filled * 3 < oldsize * 2 && ht->ht_used > oldsize / 5)
	    திரும்பு OK;

	எனில் (ht->ht_used > 1000)
	    minsize = ht->ht_used * 2;  // it's big, don't make too much room
	இல்லையெனில்
	    minsize = ht->ht_used * 4;  // make plenty of room
    }
    இல்லையெனில்
    {
	// Use specified size.
	எனில் ((long_u)minitems < ht->ht_used)	// just in case...
	    minitems = (முழு)ht->ht_used;
	minsize = (minitems * 3 + 1) / 2;	// array is up to 2/3 full
    }

    newsize = HT_INIT_SIZE;
    போது (newsize < minsize)
    {
	newsize <<= 1;		// make sure it's always a power of 2
	எனில் (newsize == 0)
	    திரும்பு FAIL;	// overflow
    }

    எனில் (newsize == HT_INIT_SIZE)
    {
	// Use the small array inside the hashdict structure.
	newarray = ht->ht_smallarray;
	எனில் (ht->ht_array == newarray)
	{
	    // Moving from ht_smallarray to ht_smallarray!  Happens when there
	    // are many removed items.  Copy the items to be able to clean up
	    // removed items.
	    mch_memmove(temparray, newarray, அளவு(temparray));
	    oldarray = temparray;
	}
	இல்லையெனில்
	    oldarray = ht->ht_array;
	CLEAR_FIELD(ht->ht_smallarray);
    }

    இல்லையெனில் எனில் (newsize == oldsize && ht->ht_filled * 3 < oldsize * 2)
    {
	// The hashtab is already at the desired size, and there are not too
	// many removed items, bail out.
	திரும்பு OK;
    }

    இல்லையெனில்
    {
	// Allocate an array.
	newarray = ALLOC_CLEAR_MULT(hashitem_T, newsize);
	எனில் (newarray == வெற்று)
	{
	    // Out of memory.  When there are NULL items still return OK.
	    // Otherwise set ht_flags to HTFLAGS_ERROR, because lookup may
	    // result in a hang if we add another item.
	    எனில் (ht->ht_filled < ht->ht_mask)
		திரும்பு OK;
	    ht->ht_flags |= HTFLAGS_ERROR;
	    திரும்பு FAIL;
	}
	oldarray = ht->ht_array;
    }

    /*
     * Move all the items from the old array to the new one, placing them in
     * the right spot.  The new array won't have any removed items, thus this
     * is also a cleanup action.
     */
    newmask = newsize - 1;
    todo = (முழு)ht->ht_used;
    க்கு (olditem = oldarray; todo > 0; ++olditem)
	எனில் (!HASHITEM_EMPTY(olditem))
	{
	    /*
	     * The algorithm to find the spot to add the item is identical to
	     * the algorithm to find an item in hash_lookup().  But we only
	     * need to search for a NULL key, thus it's simpler.
	     */
	    newi = (unsigned)(olditem->hi_hash & newmask);
	    newitem = &newarray[newi];

	    எனில் (newitem->hi_சாவி != வெற்று)
		க்கு (perturb = olditem->hi_hash; ; perturb >>= PERTURB_SHIFT)
		{
		    newi = (unsigned)((newi << 2U) + newi + perturb + 1U);
		    newitem = &newarray[newi & newmask];
		    எனில் (newitem->hi_சாவி == வெற்று)
			நிறுத்து;
		}
	    *newitem = *olditem;
	    --todo;
	}

    எனில் (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
    ht->ht_array = newarray;
    ht->ht_mask = newmask;
    ht->ht_filled = ht->ht_used;
    ++ht->ht_changed;
    ht->ht_flags &= ~HTFLAGS_ERROR;

    திரும்பு OK;
}

/*
 * Get the hash number for a key.
 * If you think you know a better hash function: Compile with HT_DEBUG set and
 * run a script that uses hashtables a lot.  Vim will then print statistics
 * when exiting.  Try that with the current hash algorithm and yours.  The
 * lower the percentage the better.
 */
    hash_T
hash_hash(char_u *சாவி)
{
    hash_T	hash;
    char_u	*p;

    எனில் ((hash = *சாவி) == 0)
	திரும்பு (hash_T)0;
    p = சாவி + 1;

    // A simplistic algorithm that appears to do very well.
    // Suggested by George Reilly.
    போது (*p != NUL)
	hash = hash * 101 + *p++;

    திரும்பு hash;
}
