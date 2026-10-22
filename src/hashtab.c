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

#ضمن "vim.h"

#إذا 0
# define HT_DEBUG	// extra checks for table consistency  and statistics

ساكن اوږد hash_شمېر_lookup = 0;	// count number of hashtab lookups
ساكن اوږد hash_شمېر_perturb = 0;	// count number of "misses"
#د_که_پای

// Magic value for algorithm that walks through the array.
#تعریف_کړه PERTURB_ښويدنه 5

ساكن صحيح hash_may_resize(hashtab_T *ht, صحيح minitems);

#إذا 0 // currently not used
/*
 * Create an empty hash table.
 * Returns NULL when out of memory.
 */
    hashtab_T *
hash_create(فراغ)
{
    hashtab_T *ht;

    ht = ALLOC_ONE(hashtab_T);
    إذا (ht != عدم)
	hash_init(ht);
    أرجع ht;
}
#د_که_پای

/*
 * Initialize an empty hash table.
 */
    فراغ
hash_init(hashtab_T *ht)
{
    // This zeroes all "ht_" entries and all the "hi_key" in "ht_smallarray".
    CLEAR_اشاره_ګر(ht);
    ht->ht_array = ht->ht_smallarray;
    ht->ht_ماسک = HT_INIT_اندازه - 1;
}

/*
 * If "ht->ht_flags" has HTFLAGS_FROZEN then give an error message using
 * "command" and return TRUE.
 */
    صحيح
check_hashtab_frozen(hashtab_T *ht, حرف *command)
{
    إذا ((ht->ht_flags & HTFLAGS_FROZEN) == 0)
	أرجع FALSE;

    semsg(_(e_not_allowed_منزل_جمع_کړه_or_remove_entries_str), command);
    أرجع TRUE;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * If "ht" is not freed then you should call hash_init() next!
 */
    فراغ
hash_clear(hashtab_T *ht)
{
    إذا (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
}

/*
 * Free the array of a hash table and all the keys it contains.  The keys must
 * have been allocated.  "off" is the offset from the start of the allocate
 * memory to the location of the key (it's always positive).
 */
    فراغ
hash_clear_all(hashtab_T *ht, صحيح off)
{
    اوږد	todo;
    hashitem_T	*hi;

    todo = (اوږد)ht->ht_کارول_شوی;
    FOR_ALL_HASHTAB_توکي(ht, hi, todo)
    {
	إذا (!HASHITEM_تشه(hi))
	{
	    vim_free(hi->hi_کلی - off);
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
hash_پیدا_کړه(hashtab_T *ht, char_u *کلی)
{
    أرجع hash_lookup(ht, کلی, hash_hash(کلی));
}

/*
 * Like hash_find(), but caller computes "hash".
 */
    hashitem_T *
hash_lookup(hashtab_T *ht, char_u *کلی, hash_T hash)
{
    hash_T	perturb;
    hashitem_T	*freeitem;
    hashitem_T	*hi;
    بینښه	idx;

#که_تعریف_شوی HT_DEBUG
    ++hash_شمېر_lookup;
#د_که_پای

    /*
     * Quickly handle the most common situations:
     * - return if there is no item at all
     * - skip over a removed item
     * - return if the item matches
     */
    idx = (بینښه)(hash & ht->ht_ماسک);
    hi = &ht->ht_array[idx];

    إذا (hi->hi_کلی == عدم)
	أرجع hi;
    إذا (hi->hi_کلی == HI_کلی_REMOVED)
	freeitem = hi;
    وإلا إذا (hi->hi_hash == hash && STRCMP(hi->hi_کلی, کلی) == 0)
	أرجع hi;
    وإلا
	freeitem = عدم;

    /*
     * Need to search through the table to find the key.  The algorithm
     * to step through the table starts with large steps, gradually becoming
     * smaller down to (1/4 table size + 1).  This means it goes through all
     * table entries in the end.
     * When we run into a NULL key it's clear that the key isn't there.
     * Return the first available slot found (can be a slot of a removed
     * item).
     */
    لكل (perturb = hash; ; perturb >>= PERTURB_ښويدنه)
    {
#که_تعریف_شوی HT_DEBUG
	++hash_شمېر_perturb;	    // count a "miss" for hashtab lookup
#د_که_پای
	idx = (بینښه)((idx << 2U) + idx + perturb + 1U);
	hi = &ht->ht_array[idx & ht->ht_ماسک];
	إذا (hi->hi_کلی == عدم)
	    أرجع freeitem == عدم ? hi : freeitem;
	إذا (hi->hi_hash == hash
		&& hi->hi_کلی != HI_کلی_REMOVED
		&& STRCMP(hi->hi_کلی, کلی) == 0)
	    أرجع hi;
	إذا (hi->hi_کلی == HI_کلی_REMOVED && freeitem == عدم)
	    freeitem = hi;
    }
}

#إذا defined(FEAT_EVAL) || defined(FEAT_SYN_HL)
/*
 * Print the efficiency of hashtable lookups.
 * Useful when trying different hash algorithms.
 * Called when exiting.
 */
    فراغ
hash_debug_results(فراغ)
{
# ifdef HT_DEBUG
    fprintf(stderr, "\r\n\r\n\r\n\r\n");
    fprintf(stderr, "Number of hashtable lookups: %ld\r\n", hash_شمېر_lookup);
    fprintf(stderr, "Number of perturb loops: %ld\r\n", hash_شمېر_perturb);
    fprintf(stderr, "Percentage of perturb loops: %ld%%\r\n",
				hash_شمېر_perturb * 100 / hash_شمېر_lookup);
# endif
}
#د_که_پای

/*
 * Add item with key "key" to hashtable "ht".
 * "command" is used for the error message when the hashtab if frozen.
 * Returns FAIL when out of memory or the key is already present.
 */
    صحيح
hash_جمع_کړه(hashtab_T *ht, char_u *کلی, حرف *command)
{
    hash_T	hash = hash_hash(کلی);
    hashitem_T	*hi;

    إذا (check_hashtab_frozen(ht, command))
	أرجع FAIL;
    hi = hash_lookup(ht, کلی, hash);
    إذا (!HASHITEM_تشه(hi))
    {
	دننی_error("hash_add()");
	أرجع FAIL;
    }
    أرجع hash_جمع_کړه_توکی(ht, hi, کلی, hash);
}

/*
 * Add item "hi" with "key" to hashtable "ht".  "key" must not be NULL and
 * "hi" must have been obtained with hash_lookup() and point to an empty item.
 * "hi" is invalid after this!
 * Returns OK or FAIL (out of memory).
 */
    صحيح
hash_جمع_کړه_توکی(
    hashtab_T	*ht,
    hashitem_T	*hi,
    char_u	*کلی,
    hash_T	hash)
{
    // If resizing failed before and it fails again we can't add an item.
    إذا (ht->ht_flags & HTFLAGS_ERROR)
	أرجع FAIL;

    ++ht->ht_کارول_شوی;
    ++ht->ht_بدل_شوی;
    إذا (hi->hi_کلی == عدم)
	++ht->ht_filled;
    hi->hi_کلی = کلی;
    hi->hi_hash = hash;

    // When the space gets low may resize the array.
    أرجع hash_may_resize(ht, 0);
}

#إذا 0  // not used
/*
 * Overwrite hashtable item "hi" with "key".  "hi" must point to the item that
 * is to be overwritten.  Thus the number of items in the hashtable doesn't
 * change.
 * Although the key must be identical, the pointer may be different, thus it's
 * set anyway (the key is part of an item with that key).
 * The caller must take care of freeing the old item.
 * "hi" is invalid after this!
 */
    فراغ
hash_سېټ(hashitem_T *hi, char_u *کلی)
{
    hi->hi_کلی = کلی;
}
#د_که_پای

/*
 * Remove item "hi" from  hashtable "ht".  "hi" must have been obtained with
 * hash_lookup().
 * "command" is used for the error message when the hashtab if frozen.
 * The caller must take care of freeing the item itself.
 */
    صحيح
hash_remove(hashtab_T *ht, hashitem_T *hi, حرف *command)
{
    إذا (check_hashtab_frozen(ht, command))
	أرجع FAIL;
    --ht->ht_کارول_شوی;
    ++ht->ht_بدل_شوی;
    hi->hi_کلی = HI_کلی_REMOVED;
    hash_may_resize(ht, 0);
    أرجع OK;
}

/*
 * Lock a hashtable: prevent that ht_array changes.
 * Don't use this when items are to be added!
 * Must call hash_unlock() later.
 */
    فراغ
hash_قلف(hashtab_T *ht)
{
    ++ht->ht_locked;
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Lock a hashtable at the specified number of entries.
 * Caller must make sure no more than "size" entries will be added.
 * Must call hash_unlock() later.
 */
    فراغ
hash_قلف_اندازه(hashtab_T *ht, صحيح اندازه)
{
    (فراغ)hash_may_resize(ht, اندازه);
    ++ht->ht_locked;
}
#د_که_پای

/*
 * Unlock a hashtable: allow ht_array changes again.
 * Table will be resized (shrink) when necessary.
 * This must balance a call to hash_lock().
 */
    فراغ
hash_unlock(hashtab_T *ht)
{
    --ht->ht_locked;
    (فراغ)hash_may_resize(ht, 0);
}

/*
 * Shrink a hashtable when there is too much empty space.
 * Grow a hashtable when there is not enough empty space.
 * Returns OK or FAIL (out of memory).
 */
    ساكن صحيح
hash_may_resize(
    hashtab_T	*ht,
    صحيح		minitems)		// minimal number of items
{
    hashitem_T	temparray[HT_INIT_اندازه];
    hashitem_T	*oldarray, *newarray;
    hashitem_T	*olditem, *newitem;
    بینښه	newi;
    صحيح		todo;
    long_u	newsize;
    long_u	minsize;
    long_u	newmask;
    hash_T	perturb;

    // Don't resize a locked table.
    إذا (ht->ht_locked > 0)
	أرجع OK;

#که_تعریف_شوی HT_DEBUG
    إذا (ht->ht_کارول_شوی > ht->ht_filled)
	emsg("hash_may_resize(): more used than filled");
    إذا (ht->ht_filled >= ht->ht_ماسک + 1)
	emsg("hash_may_resize(): table completely filled");
#د_که_پای

    long_u oldsize = ht->ht_ماسک + 1;
    إذا (minitems == 0)
    {
	// Return quickly for small tables with at least two NULL items.  NULL
	// items are required for the lookup to decide a key isn't there.
	إذا (ht->ht_filled < HT_INIT_اندازه - 1
					 && ht->ht_array == ht->ht_smallarray)
	    أرجع OK;

	/*
	 * Grow or refill the array when it's more than 2/3 full (including
	 * removed items, so that they get cleaned up).
	 * Shrink the array when it's less than 1/5 full.  When growing it is
	 * at least 1/4 full (avoids repeated grow-shrink operations)
	 */
	إذا (ht->ht_filled * 3 < oldsize * 2 && ht->ht_کارول_شوی > oldsize / 5)
	    أرجع OK;

	إذا (ht->ht_کارول_شوی > 1000)
	    minsize = ht->ht_کارول_شوی * 2;  // it's big, don't make too much room
	وإلا
	    minsize = ht->ht_کارول_شوی * 4;  // make plenty of room
    }
    وإلا
    {
	// Use specified size.
	إذا ((long_u)minitems < ht->ht_کارول_شوی)	// just in case...
	    minitems = (صحيح)ht->ht_کارول_شوی;
	minsize = (minitems * 3 + 1) / 2;	// array is up to 2/3 full
    }

    newsize = HT_INIT_اندازه;
    طالما (newsize < minsize)
    {
	newsize <<= 1;		// make sure it's always a power of 2
	إذا (newsize == 0)
	    أرجع FAIL;	//ډکېدنه
    }

    إذا (newsize == HT_INIT_اندازه)
    {
	// Use the small array inside the hashdict structure.
	newarray = ht->ht_smallarray;
	إذا (ht->ht_array == newarray)
	{
	    // Moving from ht_smallarray to ht_smallarray!  Happens when there
	    // are many removed items.  Copy the items to be able to clean up
	    // removed items.
	    mch_memmove(temparray, newarray, حجم(temparray));
	    oldarray = temparray;
	}
	وإلا
	    oldarray = ht->ht_array;
	CLEAR_ډګر(ht->ht_smallarray);
    }

    وإلا إذا (newsize == oldsize && ht->ht_filled * 3 < oldsize * 2)
    {
	// The hashtab is already at the desired size, and there are not too
	// many removed items, bail out.
	أرجع OK;
    }

    وإلا
    {
	// Allocate an array.
	newarray = ALLOC_CLEAR_MULT(hashitem_T, newsize);
	إذا (newarray == عدم)
	{
	    // Out of memory.  When there are NULL items still return OK.
	    // Otherwise set ht_flags to HTFLAGS_ERROR, because lookup may
	    // result in a hang if we add another item.
	    إذا (ht->ht_filled < ht->ht_ماسک)
		أرجع OK;
	    ht->ht_flags |= HTFLAGS_ERROR;
	    أرجع FAIL;
	}
	oldarray = ht->ht_array;
    }

    /*
     * Move all the items from the old array to the new one, placing them in
     * the right spot.  The new array won't have any removed items, thus this
     * is also a cleanup action.
     */
    newmask = newsize - 1;
    todo = (صحيح)ht->ht_کارول_شوی;
    لكل (olditem = oldarray; todo > 0; ++olditem)
	إذا (!HASHITEM_تشه(olditem))
	{
	    /*
	     * The algorithm to find the spot to add the item is identical to
	     * the algorithm to find an item in hash_lookup().  But we only
	     * need to search for a NULL key, thus it's simpler.
	     */
	    newi = (بینښه)(olditem->hi_hash & newmask);
	    newitem = &newarray[newi];

	    إذا (newitem->hi_کلی != عدم)
		لكل (perturb = olditem->hi_hash; ; perturb >>= PERTURB_ښويدنه)
		{
		    newi = (بینښه)((newi << 2U) + newi + perturb + 1U);
		    newitem = &newarray[newi & newmask];
		    إذا (newitem->hi_کلی == عدم)
			اكسر;
		}
	    *newitem = *olditem;
	    --todo;
	}

    إذا (ht->ht_array != ht->ht_smallarray)
	vim_free(ht->ht_array);
    ht->ht_array = newarray;
    ht->ht_ماسک = newmask;
    ht->ht_filled = ht->ht_کارول_شوی;
    ++ht->ht_بدل_شوی;
    ht->ht_flags &= ~HTFLAGS_ERROR;

    أرجع OK;
}

/*
 * Get the hash number for a key.
 * If you think you know a better hash function: Compile with HT_DEBUG set and
 * run a script that uses hashtables a lot.  Vim will then print statistics
 * when exiting.  Try that with the current hash algorithm and yours.  The
 * lower the percentage the better.
 */
    hash_T
hash_hash(char_u *کلی)
{
    hash_T	hash;
    char_u	*p;

    إذا ((hash = *کلی) == 0)
	أرجع (hash_T)0;
    p = کلی + 1;

    // A simplistic algorithm that appears to do very well.
    // Suggested by George Reilly.
    طالما (*p != NUL)
	hash = hash * 101 + *p++;

    أرجع hash;
}
