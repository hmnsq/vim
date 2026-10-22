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

#అన్తర్భావ "vim.h"

#ఒకవేళ 0
# define HT_DEBUG	// extra checks for table consistency  and statistics

స్థిర దీర్ఘ hash_లెక్క_lookup = 0;	// count number of hashtab lookups
స్థిర దీర్ఘ hash_లెక్క_perturb = 0;	// count number of "misses"
#అయితే_ముగింపు

// Magic value for algorithm that walks through the array.
#నిర్వచించు PERTURB_జరుపు 5

స్థిర పూర్ణాంకం hash_may_resize(hashtab_T *ht, పూర్ణాంకం minitems);

#ఒకవేళ 0 // currently not used
/*
 * Create an empty hash table.
 * Returns NULL when out of memory.
 */
    hashtab_T *
hash_create(శూన్య)
{
    hashtab_T *ht;

    ht = ALLOC_ONE(hashtab_T);
    ఒకవేళ (ht != ఖాళీ)
	hash_init(ht);
    తిరిగి ht;
}
#అయితే_ముగింపు

/*
 * Initialize an empty hash table.
 */
    శూన్య
hash_init(hashtab_T *ht)
{
    // This zeroes all "ht_" entries and all the "hi_key" in "ht_smallarray".
    CLEAR_సూచిక(ht);
    ht->ht_array = ht->ht_smallarray;
    ht->ht_ముసుగు = HT_INIT_కొలత - 1;
}

/*
 * If "ht->ht_flags" has HTFLAGS_FROZEN then give an error message using
 * "command" and return TRUE.
 */
    పూర్ణాంకం
check_hashtab_frozen(hashtab_T *ht, అక్షరం *command)
{
    ఒకవేళ ((ht->ht_flags & HTFLAGS_FROZEN) == 0)
	తిరిగి FALSE;

    semsg(_(e_not_allowed_గమ్యం_కూడు_or_remove_entries_str), command);
    తిరిగి TRUE;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * If "ht" is not freed then you should call hash_init() next!
 */
    శూన్య
hash_clear(hashtab_T *ht)
{
    ఒకవేళ (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
}

/*
 * Free the array of a hash table and all the keys it contains.  The keys must
 * have been allocated.  "off" is the offset from the start of the allocate
 * memory to the location of the key (it's always positive).
 */
    శూన్య
hash_clear_all(hashtab_T *ht, పూర్ణాంకం off)
{
    దీర్ఘ	todo;
    hashitem_T	*hi;

    todo = (దీర్ఘ)ht->ht_వాడిన;
    FOR_ALL_HASHTAB_వస్తువులు(ht, hi, todo)
    {
	ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	{
	    vim_free(hi->hi_కీలకం - off);
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
hash_కనుగొను(hashtab_T *ht, char_u *కీలకం)
{
    తిరిగి hash_lookup(ht, కీలకం, hash_hash(కీలకం));
}

/*
 * Like hash_find(), but caller computes "hash".
 */
    hashitem_T *
hash_lookup(hashtab_T *ht, char_u *కీలకం, hash_T hash)
{
    hash_T	perturb;
    hashitem_T	*freeitem;
    hashitem_T	*hi;
    గుర్తులేని	idx;

#నిర్వచితమైతే HT_DEBUG
    ++hash_లెక్క_lookup;
#అయితే_ముగింపు

    /*
     * Quickly handle the most common situations:
     * - return if there is no item at all
     * - skip over a removed item
     * - return if the item matches
     */
    idx = (గుర్తులేని)(hash & ht->ht_ముసుగు);
    hi = &ht->ht_array[idx];

    ఒకవేళ (hi->hi_కీలకం == ఖాళీ)
	తిరిగి hi;
    ఒకవేళ (hi->hi_కీలకం == HI_కీలకం_REMOVED)
	freeitem = hi;
    లేకపోతే ఒకవేళ (hi->hi_hash == hash && STRCMP(hi->hi_కీలకం, కీలకం) == 0)
	తిరిగి hi;
    లేకపోతే
	freeitem = ఖాళీ;

    /*
     * Need to search through the table to find the key.  The algorithm
     * to step through the table starts with large steps, gradually becoming
     * smaller down to (1/4 table size + 1).  This means it goes through all
     * table entries in the end.
     * When we run into a NULL key it's clear that the key isn't there.
     * Return the first available slot found (can be a slot of a removed
     * item).
     */
    కొరకు (perturb = hash; ; perturb >>= PERTURB_జరుపు)
    {
#నిర్వచితమైతే HT_DEBUG
	++hash_లెక్క_perturb;	    // count a "miss" for hashtab lookup
#అయితే_ముగింపు
	idx = (గుర్తులేని)((idx << 2U) + idx + perturb + 1U);
	hi = &ht->ht_array[idx & ht->ht_ముసుగు];
	ఒకవేళ (hi->hi_కీలకం == ఖాళీ)
	    తిరిగి freeitem == ఖాళీ ? hi : freeitem;
	ఒకవేళ (hi->hi_hash == hash
		&& hi->hi_కీలకం != HI_కీలకం_REMOVED
		&& STRCMP(hi->hi_కీలకం, కీలకం) == 0)
	    తిరిగి hi;
	ఒకవేళ (hi->hi_కీలకం == HI_కీలకం_REMOVED && freeitem == ఖాళీ)
	    freeitem = hi;
    }
}

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
/*
 * Print the efficiency of hashtable lookups.
 * Useful when trying different hash algorithms.
 * Called when exiting.
 */
    శూన్య
hash_debug_results(శూన్య)
{
# ifdef HT_DEBUG
    fprintf(stderr, "\r\n\r\n\r\n\r\n");
    fprintf(stderr, "Number of hashtable lookups: %ld\r\n", hash_లెక్క_lookup);
    fprintf(stderr, "Number of perturb loops: %ld\r\n", hash_లెక్క_perturb);
    fprintf(stderr, "Percentage of perturb loops: %ld%%\r\n",
				hash_లెక్క_perturb * 100 / hash_లెక్క_lookup);
# endif
}
#అయితే_ముగింపు

/*
 * Add item with key "key" to hashtable "ht".
 * "command" is used for the error message when the hashtab if frozen.
 * Returns FAIL when out of memory or the key is already present.
 */
    పూర్ణాంకం
hash_కూడు(hashtab_T *ht, char_u *కీలకం, అక్షరం *command)
{
    hash_T	hash = hash_hash(కీలకం);
    hashitem_T	*hi;

    ఒకవేళ (check_hashtab_frozen(ht, command))
	తిరిగి FAIL;
    hi = hash_lookup(ht, కీలకం, hash);
    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
    {
	అంతర్గత_error("hash_add()");
	తిరిగి FAIL;
    }
    తిరిగి hash_కూడు_అంశం(ht, hi, కీలకం, hash);
}

/*
 * Add item "hi" with "key" to hashtable "ht".  "key" must not be NULL and
 * "hi" must have been obtained with hash_lookup() and point to an empty item.
 * "hi" is invalid after this!
 * Returns OK or FAIL (out of memory).
 */
    పూర్ణాంకం
hash_కూడు_అంశం(
    hashtab_T	*ht,
    hashitem_T	*hi,
    char_u	*కీలకం,
    hash_T	hash)
{
    // If resizing failed before and it fails again we can't add an item.
    ఒకవేళ (ht->ht_flags & HTFLAGS_ERROR)
	తిరిగి FAIL;

    ++ht->ht_వాడిన;
    ++ht->ht_మారింది;
    ఒకవేళ (hi->hi_కీలకం == ఖాళీ)
	++ht->ht_filled;
    hi->hi_కీలకం = కీలకం;
    hi->hi_hash = hash;

    // When the space gets low may resize the array.
    తిరిగి hash_may_resize(ht, 0);
}

#ఒకవేళ 0  // not used
/*
 * Overwrite hashtable item "hi" with "key".  "hi" must point to the item that
 * is to be overwritten.  Thus the number of items in the hashtable doesn't
 * change.
 * Although the key must be identical, the pointer may be different, thus it's
 * set anyway (the key is part of an item with that key).
 * The caller must take care of freeing the old item.
 * "hi" is invalid after this!
 */
    శూన్య
hash_సమితి(hashitem_T *hi, char_u *కీలకం)
{
    hi->hi_కీలకం = కీలకం;
}
#అయితే_ముగింపు

/*
 * Remove item "hi" from  hashtable "ht".  "hi" must have been obtained with
 * hash_lookup().
 * "command" is used for the error message when the hashtab if frozen.
 * The caller must take care of freeing the item itself.
 */
    పూర్ణాంకం
hash_remove(hashtab_T *ht, hashitem_T *hi, అక్షరం *command)
{
    ఒకవేళ (check_hashtab_frozen(ht, command))
	తిరిగి FAIL;
    --ht->ht_వాడిన;
    ++ht->ht_మారింది;
    hi->hi_కీలకం = HI_కీలకం_REMOVED;
    hash_may_resize(ht, 0);
    తిరిగి OK;
}

/*
 * Lock a hashtable: prevent that ht_array changes.
 * Don't use this when items are to be added!
 * Must call hash_unlock() later.
 */
    శూన్య
hash_తాళం(hashtab_T *ht)
{
    ++ht->ht_locked;
}

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * Lock a hashtable at the specified number of entries.
 * Caller must make sure no more than "size" entries will be added.
 * Must call hash_unlock() later.
 */
    శూన్య
hash_తాళం_కొలత(hashtab_T *ht, పూర్ణాంకం కొలత)
{
    (శూన్య)hash_may_resize(ht, కొలత);
    ++ht->ht_locked;
}
#అయితే_ముగింపు

/*
 * Unlock a hashtable: allow ht_array changes again.
 * Table will be resized (shrink) when necessary.
 * This must balance a call to hash_lock().
 */
    శూన్య
hash_unlock(hashtab_T *ht)
{
    --ht->ht_locked;
    (శూన్య)hash_may_resize(ht, 0);
}

/*
 * Shrink a hashtable when there is too much empty space.
 * Grow a hashtable when there is not enough empty space.
 * Returns OK or FAIL (out of memory).
 */
    స్థిర పూర్ణాంకం
hash_may_resize(
    hashtab_T	*ht,
    పూర్ణాంకం		minitems)		// minimal number of items
{
    hashitem_T	temparray[HT_INIT_కొలత];
    hashitem_T	*oldarray, *newarray;
    hashitem_T	*olditem, *newitem;
    గుర్తులేని	newi;
    పూర్ణాంకం		todo;
    long_u	newsize;
    long_u	minsize;
    long_u	newmask;
    hash_T	perturb;

    // Don't resize a locked table.
    ఒకవేళ (ht->ht_locked > 0)
	తిరిగి OK;

#నిర్వచితమైతే HT_DEBUG
    ఒకవేళ (ht->ht_వాడిన > ht->ht_filled)
	emsg("hash_may_resize(): more used than filled");
    ఒకవేళ (ht->ht_filled >= ht->ht_ముసుగు + 1)
	emsg("hash_may_resize(): table completely filled");
#అయితే_ముగింపు

    long_u oldsize = ht->ht_ముసుగు + 1;
    ఒకవేళ (minitems == 0)
    {
	// Return quickly for small tables with at least two NULL items.  NULL
	// items are required for the lookup to decide a key isn't there.
	ఒకవేళ (ht->ht_filled < HT_INIT_కొలత - 1
					 && ht->ht_array == ht->ht_smallarray)
	    తిరిగి OK;

	/*
	 * Grow or refill the array when it's more than 2/3 full (including
	 * removed items, so that they get cleaned up).
	 * Shrink the array when it's less than 1/5 full.  When growing it is
	 * at least 1/4 full (avoids repeated grow-shrink operations)
	 */
	ఒకవేళ (ht->ht_filled * 3 < oldsize * 2 && ht->ht_వాడిన > oldsize / 5)
	    తిరిగి OK;

	ఒకవేళ (ht->ht_వాడిన > 1000)
	    minsize = ht->ht_వాడిన * 2;  // it's big, don't make too much room
	లేకపోతే
	    minsize = ht->ht_వాడిన * 4;  // make plenty of room
    }
    లేకపోతే
    {
	// Use specified size.
	ఒకవేళ ((long_u)minitems < ht->ht_వాడిన)	// just in case...
	    minitems = (పూర్ణాంకం)ht->ht_వాడిన;
	minsize = (minitems * 3 + 1) / 2;	// array is up to 2/3 full
    }

    newsize = HT_INIT_కొలత;
    అయితే (newsize < minsize)
    {
	newsize <<= 1;		// make sure it's always a power of 2
	ఒకవేళ (newsize == 0)
	    తిరిగి FAIL;	//అధికప్రవాహం
    }

    ఒకవేళ (newsize == HT_INIT_కొలత)
    {
	// Use the small array inside the hashdict structure.
	newarray = ht->ht_smallarray;
	ఒకవేళ (ht->ht_array == newarray)
	{
	    // Moving from ht_smallarray to ht_smallarray!  Happens when there
	    // are many removed items.  Copy the items to be able to clean up
	    // removed items.
	    mch_memmove(temparray, newarray, పరిమాణం(temparray));
	    oldarray = temparray;
	}
	లేకపోతే
	    oldarray = ht->ht_array;
	CLEAR_క్షేత్రం(ht->ht_smallarray);
    }

    లేకపోతే ఒకవేళ (newsize == oldsize && ht->ht_filled * 3 < oldsize * 2)
    {
	// The hashtab is already at the desired size, and there are not too
	// many removed items, bail out.
	తిరిగి OK;
    }

    లేకపోతే
    {
	// Allocate an array.
	newarray = ALLOC_CLEAR_MULT(hashitem_T, newsize);
	ఒకవేళ (newarray == ఖాళీ)
	{
	    // Out of memory.  When there are NULL items still return OK.
	    // Otherwise set ht_flags to HTFLAGS_ERROR, because lookup may
	    // result in a hang if we add another item.
	    ఒకవేళ (ht->ht_filled < ht->ht_ముసుగు)
		తిరిగి OK;
	    ht->ht_flags |= HTFLAGS_ERROR;
	    తిరిగి FAIL;
	}
	oldarray = ht->ht_array;
    }

    /*
     * Move all the items from the old array to the new one, placing them in
     * the right spot.  The new array won't have any removed items, thus this
     * is also a cleanup action.
     */
    newmask = newsize - 1;
    todo = (పూర్ణాంకం)ht->ht_వాడిన;
    కొరకు (olditem = oldarray; todo > 0; ++olditem)
	ఒకవేళ (!HASHITEM_ఖాళీలు(olditem))
	{
	    /*
	     * The algorithm to find the spot to add the item is identical to
	     * the algorithm to find an item in hash_lookup().  But we only
	     * need to search for a NULL key, thus it's simpler.
	     */
	    newi = (గుర్తులేని)(olditem->hi_hash & newmask);
	    newitem = &newarray[newi];

	    ఒకవేళ (newitem->hi_కీలకం != ఖాళీ)
		కొరకు (perturb = olditem->hi_hash; ; perturb >>= PERTURB_జరుపు)
		{
		    newi = (గుర్తులేని)((newi << 2U) + newi + perturb + 1U);
		    newitem = &newarray[newi & newmask];
		    ఒకవేళ (newitem->hi_కీలకం == ఖాళీ)
			ఆపు;
		}
	    *newitem = *olditem;
	    --todo;
	}

    ఒకవేళ (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
    ht->ht_array = newarray;
    ht->ht_ముసుగు = newmask;
    ht->ht_filled = ht->ht_వాడిన;
    ++ht->ht_మారింది;
    ht->ht_flags &= ~HTFLAGS_ERROR;

    తిరిగి OK;
}

/*
 * Get the hash number for a key.
 * If you think you know a better hash function: Compile with HT_DEBUG set and
 * run a script that uses hashtables a lot.  Vim will then print statistics
 * when exiting.  Try that with the current hash algorithm and yours.  The
 * lower the percentage the better.
 */
    hash_T
hash_hash(char_u *కీలకం)
{
    hash_T	hash;
    char_u	*p;

    ఒకవేళ ((hash = *కీలకం) == 0)
	తిరిగి (hash_T)0;
    p = కీలకం + 1;

    // A simplistic algorithm that appears to do very well.
    // Suggested by George Reilly.
    అయితే (*p != NUL)
	hash = hash * 101 + *p++;

    తిరిగి hash;
}
