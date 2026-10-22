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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ 0
# define HT_DEBUG	// extra checks for table consistency  and statistics

ಸ್ಥಿರ ದೀರ್ಘ hash_ಎಣಿಕೆ_lookup = 0;	// count number of hashtab lookups
ಸ್ಥಿರ ದೀರ್ಘ hash_ಎಣಿಕೆ_perturb = 0;	// count number of "misses"
#ಆದರೆ_ಅಂತ್ಯ

// Magic value for algorithm that walks through the array.
#ವ್ಯಾಖ್ಯಾನಿಸು PERTURB_ಸರಿಸು 5

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hash_may_resize(hashtab_T *ht, ಪೂರ್ಣಾಂಕ minitems);

#ಆದರೆ 0 // currently not used
/*
 * Create an empty hash table.
 * Returns NULL when out of memory.
 */
    hashtab_T *
hash_create(ಶೂನ್ಯ)
{
    hashtab_T *ht;

    ht = ALLOC_ONE(hashtab_T);
    ಆದರೆ (ht != ಖಾಲಿ)
	hash_init(ht);
    ಹಿಂತಿರುಗಿ ht;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Initialize an empty hash table.
 */
    ಶೂನ್ಯ
hash_init(hashtab_T *ht)
{
    // This zeroes all "ht_" entries and all the "hi_key" in "ht_smallarray".
    CLEAR_ಸೂಚಕ(ht);
    ht->ht_array = ht->ht_smallarray;
    ht->ht_ಮುಖವಾಡ = HT_INIT_ಗಾತ್ರ - 1;
}

/*
 * If "ht->ht_flags" has HTFLAGS_FROZEN then give an error message using
 * "command" and return TRUE.
 */
    ಪೂರ್ಣಾಂಕ
check_hashtab_frozen(hashtab_T *ht, ಅಕ್ಷರ *command)
{
    ಆದರೆ ((ht->ht_flags & HTFLAGS_FROZEN) == 0)
	ಹಿಂತಿರುಗಿ FALSE;

    semsg(_(e_not_allowed_ಗಮ್ಯ_ಕೂಡಿಸು_or_remove_entries_str), command);
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * If "ht" is not freed then you should call hash_init() next!
 */
    ಶೂನ್ಯ
hash_clear(hashtab_T *ht)
{
    ಆದರೆ (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
}

/*
 * Free the array of a hash table and all the keys it contains.  The keys must
 * have been allocated.  "off" is the offset from the start of the allocate
 * memory to the location of the key (it's always positive).
 */
    ಶೂನ್ಯ
hash_clear_all(hashtab_T *ht, ಪೂರ್ಣಾಂಕ off)
{
    ದೀರ್ಘ	todo;
    hashitem_T	*hi;

    todo = (ದೀರ್ಘ)ht->ht_ಬಳಸಿದ;
    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(ht, hi, todo)
    {
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	{
	    vim_free(hi->hi_ಕೀಲಿ - off);
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
hash_ಕಂಡುಹಿಡಿ(hashtab_T *ht, char_u *ಕೀಲಿ)
{
    ಹಿಂತಿರುಗಿ hash_lookup(ht, ಕೀಲಿ, hash_hash(ಕೀಲಿ));
}

/*
 * Like hash_find(), but caller computes "hash".
 */
    hashitem_T *
hash_lookup(hashtab_T *ht, char_u *ಕೀಲಿ, hash_T hash)
{
    hash_T	perturb;
    hashitem_T	*freeitem;
    hashitem_T	*hi;
    ಚಿಹ್ನೆರಹಿತ	idx;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HT_DEBUG
    ++hash_ಎಣಿಕೆ_lookup;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Quickly handle the most common situations:
     * - return if there is no item at all
     * - skip over a removed item
     * - return if the item matches
     */
    idx = (ಚಿಹ್ನೆರಹಿತ)(hash & ht->ht_ಮುಖವಾಡ);
    hi = &ht->ht_array[idx];

    ಆದರೆ (hi->hi_ಕೀಲಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ hi;
    ಆದರೆ (hi->hi_ಕೀಲಿ == HI_ಕೀಲಿ_REMOVED)
	freeitem = hi;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hi->hi_hash == hash && STRCMP(hi->hi_ಕೀಲಿ, ಕೀಲಿ) == 0)
	ಹಿಂತಿರುಗಿ hi;
    ಇಲ್ಲದಿದ್ದರೆ
	freeitem = ಖಾಲಿ;

    /*
     * Need to search through the table to find the key.  The algorithm
     * to step through the table starts with large steps, gradually becoming
     * smaller down to (1/4 table size + 1).  This means it goes through all
     * table entries in the end.
     * When we run into a NULL key it's clear that the key isn't there.
     * Return the first available slot found (can be a slot of a removed
     * item).
     */
    ಗಾಗಿ (perturb = hash; ; perturb >>= PERTURB_ಸರಿಸು)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HT_DEBUG
	++hash_ಎಣಿಕೆ_perturb;	    // count a "miss" for hashtab lookup
#ಆದರೆ_ಅಂತ್ಯ
	idx = (ಚಿಹ್ನೆರಹಿತ)((idx << 2U) + idx + perturb + 1U);
	hi = &ht->ht_array[idx & ht->ht_ಮುಖವಾಡ];
	ಆದರೆ (hi->hi_ಕೀಲಿ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ freeitem == ಖಾಲಿ ? hi : freeitem;
	ಆದರೆ (hi->hi_hash == hash
		&& hi->hi_ಕೀಲಿ != HI_ಕೀಲಿ_REMOVED
		&& STRCMP(hi->hi_ಕೀಲಿ, ಕೀಲಿ) == 0)
	    ಹಿಂತಿರುಗಿ hi;
	ಆದರೆ (hi->hi_ಕೀಲಿ == HI_ಕೀಲಿ_REMOVED && freeitem == ಖಾಲಿ)
	    freeitem = hi;
    }
}

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
/*
 * Print the efficiency of hashtable lookups.
 * Useful when trying different hash algorithms.
 * Called when exiting.
 */
    ಶೂನ್ಯ
hash_debug_results(ಶೂನ್ಯ)
{
# ifdef HT_DEBUG
    fprintf(stderr, "\r\n\r\n\r\n\r\n");
    fprintf(stderr, "Number of hashtable lookups: %ld\r\n", hash_ಎಣಿಕೆ_lookup);
    fprintf(stderr, "Number of perturb loops: %ld\r\n", hash_ಎಣಿಕೆ_perturb);
    fprintf(stderr, "Percentage of perturb loops: %ld%%\r\n",
				hash_ಎಣಿಕೆ_perturb * 100 / hash_ಎಣಿಕೆ_lookup);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Add item with key "key" to hashtable "ht".
 * "command" is used for the error message when the hashtab if frozen.
 * Returns FAIL when out of memory or the key is already present.
 */
    ಪೂರ್ಣಾಂಕ
hash_ಕೂಡಿಸು(hashtab_T *ht, char_u *ಕೀಲಿ, ಅಕ್ಷರ *command)
{
    hash_T	hash = hash_hash(ಕೀಲಿ);
    hashitem_T	*hi;

    ಆದರೆ (check_hashtab_frozen(ht, command))
	ಹಿಂತಿರುಗಿ FAIL;
    hi = hash_lookup(ht, ಕೀಲಿ, hash);
    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
    {
	ಆಂತರಿಕ_error("hash_add()");
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ hash_ಕೂಡಿಸು_ಅಂಶ(ht, hi, ಕೀಲಿ, hash);
}

/*
 * Add item "hi" with "key" to hashtable "ht".  "key" must not be NULL and
 * "hi" must have been obtained with hash_lookup() and point to an empty item.
 * "hi" is invalid after this!
 * Returns OK or FAIL (out of memory).
 */
    ಪೂರ್ಣಾಂಕ
hash_ಕೂಡಿಸು_ಅಂಶ(
    hashtab_T	*ht,
    hashitem_T	*hi,
    char_u	*ಕೀಲಿ,
    hash_T	hash)
{
    // If resizing failed before and it fails again we can't add an item.
    ಆದರೆ (ht->ht_flags & HTFLAGS_ERROR)
	ಹಿಂತಿರುಗಿ FAIL;

    ++ht->ht_ಬಳಸಿದ;
    ++ht->ht_ಬದಲಾಯಿತು;
    ಆದರೆ (hi->hi_ಕೀಲಿ == ಖಾಲಿ)
	++ht->ht_filled;
    hi->hi_ಕೀಲಿ = ಕೀಲಿ;
    hi->hi_hash = hash;

    // When the space gets low may resize the array.
    ಹಿಂತಿರುಗಿ hash_may_resize(ht, 0);
}

#ಆದರೆ 0  // not used
/*
 * Overwrite hashtable item "hi" with "key".  "hi" must point to the item that
 * is to be overwritten.  Thus the number of items in the hashtable doesn't
 * change.
 * Although the key must be identical, the pointer may be different, thus it's
 * set anyway (the key is part of an item with that key).
 * The caller must take care of freeing the old item.
 * "hi" is invalid after this!
 */
    ಶೂನ್ಯ
hash_ಗಣ(hashitem_T *hi, char_u *ಕೀಲಿ)
{
    hi->hi_ಕೀಲಿ = ಕೀಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Remove item "hi" from  hashtable "ht".  "hi" must have been obtained with
 * hash_lookup().
 * "command" is used for the error message when the hashtab if frozen.
 * The caller must take care of freeing the item itself.
 */
    ಪೂರ್ಣಾಂಕ
hash_remove(hashtab_T *ht, hashitem_T *hi, ಅಕ್ಷರ *command)
{
    ಆದರೆ (check_hashtab_frozen(ht, command))
	ಹಿಂತಿರುಗಿ FAIL;
    --ht->ht_ಬಳಸಿದ;
    ++ht->ht_ಬದಲಾಯಿತು;
    hi->hi_ಕೀಲಿ = HI_ಕೀಲಿ_REMOVED;
    hash_may_resize(ht, 0);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Lock a hashtable: prevent that ht_array changes.
 * Don't use this when items are to be added!
 * Must call hash_unlock() later.
 */
    ಶೂನ್ಯ
hash_ಬೀಗ(hashtab_T *ht)
{
    ++ht->ht_locked;
}

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * Lock a hashtable at the specified number of entries.
 * Caller must make sure no more than "size" entries will be added.
 * Must call hash_unlock() later.
 */
    ಶೂನ್ಯ
hash_ಬೀಗ_ಗಾತ್ರ(hashtab_T *ht, ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2)
{
    (ಶೂನ್ಯ)hash_may_resize(ht, ಗಾತ್ರ_2);
    ++ht->ht_locked;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Unlock a hashtable: allow ht_array changes again.
 * Table will be resized (shrink) when necessary.
 * This must balance a call to hash_lock().
 */
    ಶೂನ್ಯ
hash_unlock(hashtab_T *ht)
{
    --ht->ht_locked;
    (ಶೂನ್ಯ)hash_may_resize(ht, 0);
}

/*
 * Shrink a hashtable when there is too much empty space.
 * Grow a hashtable when there is not enough empty space.
 * Returns OK or FAIL (out of memory).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
hash_may_resize(
    hashtab_T	*ht,
    ಪೂರ್ಣಾಂಕ		minitems)		// minimal number of items
{
    hashitem_T	temparray[HT_INIT_ಗಾತ್ರ];
    hashitem_T	*oldarray, *newarray;
    hashitem_T	*olditem, *newitem;
    ಚಿಹ್ನೆರಹಿತ	newi;
    ಪೂರ್ಣಾಂಕ		todo;
    long_u	newsize;
    long_u	minsize;
    long_u	newmask;
    hash_T	perturb;

    // Don't resize a locked table.
    ಆದರೆ (ht->ht_locked > 0)
	ಹಿಂತಿರುಗಿ OK;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HT_DEBUG
    ಆದರೆ (ht->ht_ಬಳಸಿದ > ht->ht_filled)
	emsg("hash_may_resize(): more used than filled");
    ಆದರೆ (ht->ht_filled >= ht->ht_ಮುಖವಾಡ + 1)
	emsg("hash_may_resize(): table completely filled");
#ಆದರೆ_ಅಂತ್ಯ

    long_u oldsize = ht->ht_ಮುಖವಾಡ + 1;
    ಆದರೆ (minitems == 0)
    {
	// Return quickly for small tables with at least two NULL items.  NULL
	// items are required for the lookup to decide a key isn't there.
	ಆದರೆ (ht->ht_filled < HT_INIT_ಗಾತ್ರ - 1
					 && ht->ht_array == ht->ht_smallarray)
	    ಹಿಂತಿರುಗಿ OK;

	/*
	 * Grow or refill the array when it's more than 2/3 full (including
	 * removed items, so that they get cleaned up).
	 * Shrink the array when it's less than 1/5 full.  When growing it is
	 * at least 1/4 full (avoids repeated grow-shrink operations)
	 */
	ಆದರೆ (ht->ht_filled * 3 < oldsize * 2 && ht->ht_ಬಳಸಿದ > oldsize / 5)
	    ಹಿಂತಿರುಗಿ OK;

	ಆದರೆ (ht->ht_ಬಳಸಿದ > 1000)
	    minsize = ht->ht_ಬಳಸಿದ * 2;  // it's big, don't make too much room
	ಇಲ್ಲದಿದ್ದರೆ
	    minsize = ht->ht_ಬಳಸಿದ * 4;  // make plenty of room
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Use specified size.
	ಆದರೆ ((long_u)minitems < ht->ht_ಬಳಸಿದ)	// just in case...
	    minitems = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
	minsize = (minitems * 3 + 1) / 2;	// array is up to 2/3 full
    }

    newsize = HT_INIT_ಗಾತ್ರ;
    ಆಗ (newsize < minsize)
    {
	newsize <<= 1;		// make sure it's always a power of 2
	ಆದರೆ (newsize == 0)
	    ಹಿಂತಿರುಗಿ FAIL;	//ಅತಿಪ್ರವಾಹ
    }

    ಆದರೆ (newsize == HT_INIT_ಗಾತ್ರ)
    {
	// Use the small array inside the hashdict structure.
	newarray = ht->ht_smallarray;
	ಆದರೆ (ht->ht_array == newarray)
	{
	    // Moving from ht_smallarray to ht_smallarray!  Happens when there
	    // are many removed items.  Copy the items to be able to clean up
	    // removed items.
	    mch_memmove(temparray, newarray, ಗಾತ್ರ(temparray));
	    oldarray = temparray;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    oldarray = ht->ht_array;
	CLEAR_ಕ್ಷೇತ್ರ(ht->ht_smallarray);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (newsize == oldsize && ht->ht_filled * 3 < oldsize * 2)
    {
	// The hashtab is already at the desired size, and there are not too
	// many removed items, bail out.
	ಹಿಂತಿರುಗಿ OK;
    }

    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Allocate an array.
	newarray = ALLOC_CLEAR_MULT(hashitem_T, newsize);
	ಆದರೆ (newarray == ಖಾಲಿ)
	{
	    // Out of memory.  When there are NULL items still return OK.
	    // Otherwise set ht_flags to HTFLAGS_ERROR, because lookup may
	    // result in a hang if we add another item.
	    ಆದರೆ (ht->ht_filled < ht->ht_ಮುಖವಾಡ)
		ಹಿಂತಿರುಗಿ OK;
	    ht->ht_flags |= HTFLAGS_ERROR;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	oldarray = ht->ht_array;
    }

    /*
     * Move all the items from the old array to the new one, placing them in
     * the right spot.  The new array won't have any removed items, thus this
     * is also a cleanup action.
     */
    newmask = newsize - 1;
    todo = (ಪೂರ್ಣಾಂಕ)ht->ht_ಬಳಸಿದ;
    ಗಾಗಿ (olditem = oldarray; todo > 0; ++olditem)
	ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(olditem))
	{
	    /*
	     * The algorithm to find the spot to add the item is identical to
	     * the algorithm to find an item in hash_lookup().  But we only
	     * need to search for a NULL key, thus it's simpler.
	     */
	    newi = (ಚಿಹ್ನೆರಹಿತ)(olditem->hi_hash & newmask);
	    newitem = &newarray[newi];

	    ಆದರೆ (newitem->hi_ಕೀಲಿ != ಖಾಲಿ)
		ಗಾಗಿ (perturb = olditem->hi_hash; ; perturb >>= PERTURB_ಸರಿಸು)
		{
		    newi = (ಚಿಹ್ನೆರಹಿತ)((newi << 2U) + newi + perturb + 1U);
		    newitem = &newarray[newi & newmask];
		    ಆದರೆ (newitem->hi_ಕೀಲಿ == ಖಾಲಿ)
			ನಿಲ್ಲಿಸು;
		}
	    *newitem = *olditem;
	    --todo;
	}

    ಆದರೆ (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
    ht->ht_array = newarray;
    ht->ht_ಮುಖವಾಡ = newmask;
    ht->ht_filled = ht->ht_ಬಳಸಿದ;
    ++ht->ht_ಬದಲಾಯಿತು;
    ht->ht_flags &= ~HTFLAGS_ERROR;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the hash number for a key.
 * If you think you know a better hash function: Compile with HT_DEBUG set and
 * run a script that uses hashtables a lot.  Vim will then print statistics
 * when exiting.  Try that with the current hash algorithm and yours.  The
 * lower the percentage the better.
 */
    hash_T
hash_hash(char_u *ಕೀಲಿ)
{
    hash_T	hash;
    char_u	*p;

    ಆದರೆ ((hash = *ಕೀಲಿ) == 0)
	ಹಿಂತಿರುಗಿ (hash_T)0;
    p = ಕೀಲಿ + 1;

    // A simplistic algorithm that appears to do very well.
    // Suggested by George Reilly.
    ಆಗ (*p != NUL)
	hash = hash * 101 + *p++;

    ಹಿಂತಿರುಗಿ hash;
}
