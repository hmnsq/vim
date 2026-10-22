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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ 0
# define HT_DEBUG	// extra checks for table consistency  and statistics

സ്ഥിതം long hash_എണ്ണം_lookup = 0;	// count number of hashtab lookups
സ്ഥിതം long hash_എണ്ണം_perturb = 0;	// count number of "misses"
#endif

// Magic value for algorithm that walks through the array.
#define PERTURB_SHIFT 5

സ്ഥിതം പൂർണ്ണസംഖ്യ hash_may_resize(hashtab_T *ht, പൂർണ്ണസംഖ്യ minitems);

#എങ്കിൽ 0 // currently not used
/*
 * Create an empty hash table.
 * Returns NULL when out of memory.
 */
    hashtab_T *
hash_create(ശൂന്യം)
{
    hashtab_T *ht;

    ht = ALLOC_ONE(hashtab_T);
    എങ്കിൽ (ht != ശൂന്യ)
	hash_init(ht);
    തിരികെ ht;
}
#endif

/*
 * Initialize an empty hash table.
 */
    ശൂന്യം
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
    പൂർണ്ണസംഖ്യ
check_hashtab_frozen(hashtab_T *ht, അക്ഷരം *command)
{
    എങ്കിൽ ((ht->ht_flags & HTFLAGS_FROZEN) == 0)
	തിരികെ FALSE;

    semsg(_(e_not_allowed_to_add_or_remove_entries_str), command);
    തിരികെ TRUE;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * If "ht" is not freed then you should call hash_init() next!
 */
    ശൂന്യം
hash_clear(hashtab_T *ht)
{
    എങ്കിൽ (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
}

/*
 * Free the array of a hash table and all the keys it contains.  The keys must
 * have been allocated.  "off" is the offset from the start of the allocate
 * memory to the location of the key (it's always positive).
 */
    ശൂന്യം
hash_clear_all(hashtab_T *ht, പൂർണ്ണസംഖ്യ off)
{
    long	todo;
    hashitem_T	*hi;

    todo = (long)ht->ht_used;
    FOR_ALL_HASHTAB_ഇനങ്ങൾ(ht, hi, todo)
    {
	എങ്കിൽ (!HASHITEM_EMPTY(hi))
	{
	    vim_free(hi->hi_താക്കോൽ - off);
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
hash_find(hashtab_T *ht, char_u *താക്കോൽ)
{
    തിരികെ hash_lookup(ht, താക്കോൽ, hash_hash(താക്കോൽ));
}

/*
 * Like hash_find(), but caller computes "hash".
 */
    hashitem_T *
hash_lookup(hashtab_T *ht, char_u *താക്കോൽ, hash_T hash)
{
    hash_T	perturb;
    hashitem_T	*freeitem;
    hashitem_T	*hi;
    unsigned	idx;

#ifdef HT_DEBUG
    ++hash_എണ്ണം_lookup;
#endif

    /*
     * Quickly handle the most common situations:
     * - return if there is no item at all
     * - skip over a removed item
     * - return if the item matches
     */
    idx = (unsigned)(hash & ht->ht_mask);
    hi = &ht->ht_array[idx];

    എങ്കിൽ (hi->hi_താക്കോൽ == ശൂന്യ)
	തിരികെ hi;
    എങ്കിൽ (hi->hi_താക്കോൽ == HI_താക്കോൽ_REMOVED)
	freeitem = hi;
    അല്ലെങ്കിൽ എങ്കിൽ (hi->hi_hash == hash && STRCMP(hi->hi_താക്കോൽ, താക്കോൽ) == 0)
	തിരികെ hi;
    അല്ലെങ്കിൽ
	freeitem = ശൂന്യ;

    /*
     * Need to search through the table to find the key.  The algorithm
     * to step through the table starts with large steps, gradually becoming
     * smaller down to (1/4 table size + 1).  This means it goes through all
     * table entries in the end.
     * When we run into a NULL key it's clear that the key isn't there.
     * Return the first available slot found (can be a slot of a removed
     * item).
     */
    വേണ്ടി (perturb = hash; ; perturb >>= PERTURB_SHIFT)
    {
#ifdef HT_DEBUG
	++hash_എണ്ണം_perturb;	    // count a "miss" for hashtab lookup
#endif
	idx = (unsigned)((idx << 2U) + idx + perturb + 1U);
	hi = &ht->ht_array[idx & ht->ht_mask];
	എങ്കിൽ (hi->hi_താക്കോൽ == ശൂന്യ)
	    തിരികെ freeitem == ശൂന്യ ? hi : freeitem;
	എങ്കിൽ (hi->hi_hash == hash
		&& hi->hi_താക്കോൽ != HI_താക്കോൽ_REMOVED
		&& STRCMP(hi->hi_താക്കോൽ, താക്കോൽ) == 0)
	    തിരികെ hi;
	എങ്കിൽ (hi->hi_താക്കോൽ == HI_താക്കോൽ_REMOVED && freeitem == ശൂന്യ)
	    freeitem = hi;
    }
}

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
/*
 * Print the efficiency of hashtable lookups.
 * Useful when trying different hash algorithms.
 * Called when exiting.
 */
    ശൂന്യം
hash_debug_results(ശൂന്യം)
{
# ifdef HT_DEBUG
    fprintf(stderr, "\r\n\r\n\r\n\r\n");
    fprintf(stderr, "Number of hashtable lookups: %ld\r\n", hash_എണ്ണം_lookup);
    fprintf(stderr, "Number of perturb loops: %ld\r\n", hash_എണ്ണം_perturb);
    fprintf(stderr, "Percentage of perturb loops: %ld%%\r\n",
				hash_എണ്ണം_perturb * 100 / hash_എണ്ണം_lookup);
# endif
}
#endif

/*
 * Add item with key "key" to hashtable "ht".
 * "command" is used for the error message when the hashtab if frozen.
 * Returns FAIL when out of memory or the key is already present.
 */
    പൂർണ്ണസംഖ്യ
hash_add(hashtab_T *ht, char_u *താക്കോൽ, അക്ഷരം *command)
{
    hash_T	hash = hash_hash(താക്കോൽ);
    hashitem_T	*hi;

    എങ്കിൽ (check_hashtab_frozen(ht, command))
	തിരികെ FAIL;
    hi = hash_lookup(ht, താക്കോൽ, hash);
    എങ്കിൽ (!HASHITEM_EMPTY(hi))
    {
	internal_error("hash_add()");
	തിരികെ FAIL;
    }
    തിരികെ hash_add_item(ht, hi, താക്കോൽ, hash);
}

/*
 * Add item "hi" with "key" to hashtable "ht".  "key" must not be NULL and
 * "hi" must have been obtained with hash_lookup() and point to an empty item.
 * "hi" is invalid after this!
 * Returns OK or FAIL (out of memory).
 */
    പൂർണ്ണസംഖ്യ
hash_add_item(
    hashtab_T	*ht,
    hashitem_T	*hi,
    char_u	*താക്കോൽ,
    hash_T	hash)
{
    // If resizing failed before and it fails again we can't add an item.
    എങ്കിൽ (ht->ht_flags & HTFLAGS_ERROR)
	തിരികെ FAIL;

    ++ht->ht_used;
    ++ht->ht_changed;
    എങ്കിൽ (hi->hi_താക്കോൽ == ശൂന്യ)
	++ht->ht_filled;
    hi->hi_താക്കോൽ = താക്കോൽ;
    hi->hi_hash = hash;

    // When the space gets low may resize the array.
    തിരികെ hash_may_resize(ht, 0);
}

#എങ്കിൽ 0  // not used
/*
 * Overwrite hashtable item "hi" with "key".  "hi" must point to the item that
 * is to be overwritten.  Thus the number of items in the hashtable doesn't
 * change.
 * Although the key must be identical, the pointer may be different, thus it's
 * set anyway (the key is part of an item with that key).
 * The caller must take care of freeing the old item.
 * "hi" is invalid after this!
 */
    ശൂന്യം
hash_set(hashitem_T *hi, char_u *താക്കോൽ)
{
    hi->hi_താക്കോൽ = താക്കോൽ;
}
#endif

/*
 * Remove item "hi" from  hashtable "ht".  "hi" must have been obtained with
 * hash_lookup().
 * "command" is used for the error message when the hashtab if frozen.
 * The caller must take care of freeing the item itself.
 */
    പൂർണ്ണസംഖ്യ
hash_remove(hashtab_T *ht, hashitem_T *hi, അക്ഷരം *command)
{
    എങ്കിൽ (check_hashtab_frozen(ht, command))
	തിരികെ FAIL;
    --ht->ht_used;
    ++ht->ht_changed;
    hi->hi_താക്കോൽ = HI_താക്കോൽ_REMOVED;
    hash_may_resize(ht, 0);
    തിരികെ OK;
}

/*
 * Lock a hashtable: prevent that ht_array changes.
 * Don't use this when items are to be added!
 * Must call hash_unlock() later.
 */
    ശൂന്യം
hash_lock(hashtab_T *ht)
{
    ++ht->ht_locked;
}

#എങ്കിൽ defined(FEAT_PROP_POPUP)
/*
 * Lock a hashtable at the specified number of entries.
 * Caller must make sure no more than "size" entries will be added.
 * Must call hash_unlock() later.
 */
    ശൂന്യം
hash_lock_size(hashtab_T *ht, പൂർണ്ണസംഖ്യ size)
{
    (ശൂന്യം)hash_may_resize(ht, size);
    ++ht->ht_locked;
}
#endif

/*
 * Unlock a hashtable: allow ht_array changes again.
 * Table will be resized (shrink) when necessary.
 * This must balance a call to hash_lock().
 */
    ശൂന്യം
hash_unlock(hashtab_T *ht)
{
    --ht->ht_locked;
    (ശൂന്യം)hash_may_resize(ht, 0);
}

/*
 * Shrink a hashtable when there is too much empty space.
 * Grow a hashtable when there is not enough empty space.
 * Returns OK or FAIL (out of memory).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
hash_may_resize(
    hashtab_T	*ht,
    പൂർണ്ണസംഖ്യ		minitems)		// minimal number of items
{
    hashitem_T	temparray[HT_INIT_SIZE];
    hashitem_T	*oldarray, *newarray;
    hashitem_T	*olditem, *newitem;
    unsigned	newi;
    പൂർണ്ണസംഖ്യ		todo;
    long_u	newsize;
    long_u	minsize;
    long_u	newmask;
    hash_T	perturb;

    // Don't resize a locked table.
    എങ്കിൽ (ht->ht_locked > 0)
	തിരികെ OK;

#ifdef HT_DEBUG
    എങ്കിൽ (ht->ht_used > ht->ht_filled)
	emsg("hash_may_resize(): more used than filled");
    എങ്കിൽ (ht->ht_filled >= ht->ht_mask + 1)
	emsg("hash_may_resize(): table completely filled");
#endif

    long_u oldsize = ht->ht_mask + 1;
    എങ്കിൽ (minitems == 0)
    {
	// Return quickly for small tables with at least two NULL items.  NULL
	// items are required for the lookup to decide a key isn't there.
	എങ്കിൽ (ht->ht_filled < HT_INIT_SIZE - 1
					 && ht->ht_array == ht->ht_smallarray)
	    തിരികെ OK;

	/*
	 * Grow or refill the array when it's more than 2/3 full (including
	 * removed items, so that they get cleaned up).
	 * Shrink the array when it's less than 1/5 full.  When growing it is
	 * at least 1/4 full (avoids repeated grow-shrink operations)
	 */
	എങ്കിൽ (ht->ht_filled * 3 < oldsize * 2 && ht->ht_used > oldsize / 5)
	    തിരികെ OK;

	എങ്കിൽ (ht->ht_used > 1000)
	    minsize = ht->ht_used * 2;  // it's big, don't make too much room
	അല്ലെങ്കിൽ
	    minsize = ht->ht_used * 4;  // make plenty of room
    }
    അല്ലെങ്കിൽ
    {
	// Use specified size.
	എങ്കിൽ ((long_u)minitems < ht->ht_used)	// just in case...
	    minitems = (പൂർണ്ണസംഖ്യ)ht->ht_used;
	minsize = (minitems * 3 + 1) / 2;	// array is up to 2/3 full
    }

    newsize = HT_INIT_SIZE;
    ആകുമ്പോൾ (newsize < minsize)
    {
	newsize <<= 1;		// make sure it's always a power of 2
	എങ്കിൽ (newsize == 0)
	    തിരികെ FAIL;	// overflow
    }

    എങ്കിൽ (newsize == HT_INIT_SIZE)
    {
	// Use the small array inside the hashdict structure.
	newarray = ht->ht_smallarray;
	എങ്കിൽ (ht->ht_array == newarray)
	{
	    // Moving from ht_smallarray to ht_smallarray!  Happens when there
	    // are many removed items.  Copy the items to be able to clean up
	    // removed items.
	    mch_memmove(temparray, newarray, വലുപ്പം(temparray));
	    oldarray = temparray;
	}
	അല്ലെങ്കിൽ
	    oldarray = ht->ht_array;
	CLEAR_FIELD(ht->ht_smallarray);
    }

    അല്ലെങ്കിൽ എങ്കിൽ (newsize == oldsize && ht->ht_filled * 3 < oldsize * 2)
    {
	// The hashtab is already at the desired size, and there are not too
	// many removed items, bail out.
	തിരികെ OK;
    }

    അല്ലെങ്കിൽ
    {
	// Allocate an array.
	newarray = ALLOC_CLEAR_MULT(hashitem_T, newsize);
	എങ്കിൽ (newarray == ശൂന്യ)
	{
	    // Out of memory.  When there are NULL items still return OK.
	    // Otherwise set ht_flags to HTFLAGS_ERROR, because lookup may
	    // result in a hang if we add another item.
	    എങ്കിൽ (ht->ht_filled < ht->ht_mask)
		തിരികെ OK;
	    ht->ht_flags |= HTFLAGS_ERROR;
	    തിരികെ FAIL;
	}
	oldarray = ht->ht_array;
    }

    /*
     * Move all the items from the old array to the new one, placing them in
     * the right spot.  The new array won't have any removed items, thus this
     * is also a cleanup action.
     */
    newmask = newsize - 1;
    todo = (പൂർണ്ണസംഖ്യ)ht->ht_used;
    വേണ്ടി (olditem = oldarray; todo > 0; ++olditem)
	എങ്കിൽ (!HASHITEM_EMPTY(olditem))
	{
	    /*
	     * The algorithm to find the spot to add the item is identical to
	     * the algorithm to find an item in hash_lookup().  But we only
	     * need to search for a NULL key, thus it's simpler.
	     */
	    newi = (unsigned)(olditem->hi_hash & newmask);
	    newitem = &newarray[newi];

	    എങ്കിൽ (newitem->hi_താക്കോൽ != ശൂന്യ)
		വേണ്ടി (perturb = olditem->hi_hash; ; perturb >>= PERTURB_SHIFT)
		{
		    newi = (unsigned)((newi << 2U) + newi + perturb + 1U);
		    newitem = &newarray[newi & newmask];
		    എങ്കിൽ (newitem->hi_താക്കോൽ == ശൂന്യ)
			നിർത്തുക;
		}
	    *newitem = *olditem;
	    --todo;
	}

    എങ്കിൽ (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
    ht->ht_array = newarray;
    ht->ht_mask = newmask;
    ht->ht_filled = ht->ht_used;
    ++ht->ht_changed;
    ht->ht_flags &= ~HTFLAGS_ERROR;

    തിരികെ OK;
}

/*
 * Get the hash number for a key.
 * If you think you know a better hash function: Compile with HT_DEBUG set and
 * run a script that uses hashtables a lot.  Vim will then print statistics
 * when exiting.  Try that with the current hash algorithm and yours.  The
 * lower the percentage the better.
 */
    hash_T
hash_hash(char_u *താക്കോൽ)
{
    hash_T	hash;
    char_u	*p;

    എങ്കിൽ ((hash = *താക്കോൽ) == 0)
	തിരികെ (hash_T)0;
    p = താക്കോൽ + 1;

    // A simplistic algorithm that appears to do very well.
    // Suggested by George Reilly.
    ആകുമ്പോൾ (*p != NUL)
	hash = hash * 101 + *p++;

    തിരികെ hash;
}
