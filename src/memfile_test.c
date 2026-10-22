/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * memfile_test.c: Unittests for memfile.c
 * Mostly by Ivan Krasilnikov.
 */

#undef NDEBUG
#અન્તર્ભાવ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#અન્તર્ભાવ "main.c"

// This file has to be included because the tested functions are static
#અન્તર્ભાવ "memfile.c"

#define index_to_ચાવી(i) ((i) ^ 15167)
#define TEST_ગણતરી 50000

/*
 * Test mf_hash_*() functions.
 */
    સ્થિર શૂન્ય
test_mf_hash(શૂન્ય)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      ચાવી;
    long_u	   i;
    long_u	   અંક_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    માટે (i = 0; i < TEST_ગણતરી; i++)
    {
	પ્રતિજ્ઞા(ht.mht_ગણતરી == i);

	// check that number of buckets is a power of 2
	અંક_buckets = ht.mht_mask + 1;
	પ્રતિજ્ઞા(અંક_buckets > 0 && (અંક_buckets & (અંક_buckets - 1)) == 0);

	// check load factor
	પ્રતિજ્ઞા(ht.mht_ગણતરી <= (અંક_buckets << MHT_LOG_LOAD_FACTOR));

	જો (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    પ્રતિજ્ઞા(અંક_buckets == MHT_INIT_SIZE);
	    પ્રતિજ્ઞા(ht.mht_buckets == ht.mht_small_buckets);
	}
	નહીંતર
	{
	    પ્રતિજ્ઞા(અંક_buckets > MHT_INIT_SIZE);
	    પ્રતિજ્ઞા(ht.mht_buckets != ht.mht_small_buckets);
	}

	ચાવી = index_to_ચાવી(i);
	પ્રતિજ્ઞા(mf_hash_find(&ht, ચાવી) == ખાલી);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	પ્રતિજ્ઞા(item != ખાલી);
	item->mhi_ચાવી = ચાવી;
	mf_hash_add_item(&ht, item);

	પ્રતિજ્ઞા(mf_hash_find(&ht, ચાવી) == item);

	જો (ht.mht_mask + 1 != અંક_buckets)
	{
	    // hash table was expanded
	    પ્રતિજ્ઞા(ht.mht_mask + 1 == અંક_buckets * MHT_GROWTH_FACTOR);
	    પ્રતિજ્ઞા(i + 1 == (અંક_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    માટે (i = 0; i < TEST_ગણતરી; i++)
    {
	ચાવી = index_to_ચાવી(i);
	item = mf_hash_find(&ht, ચાવી);
	પ્રતિજ્ઞા(item != ખાલી);
	પ્રતિજ્ઞા(item->mhi_ચાવી == ચાવી);
    }

    // delete some items
    માટે (i = 0; i < TEST_ગણતરી; i++)
    {
	જો (i % 100 < 70)
	{
	    ચાવી = index_to_ચાવી(i);
	    item = mf_hash_find(&ht, ચાવી);
	    પ્રતિજ્ઞા(item != ખાલી);
	    પ્રતિજ્ઞા(item->mhi_ચાવી == ચાવી);

	    mf_hash_rem_item(&ht, item);
	    પ્રતિજ્ઞા(mf_hash_find(&ht, ચાવી) == ખાલી);

	    mf_hash_add_item(&ht, item);
	    પ્રતિજ્ઞા(mf_hash_find(&ht, ચાવી) == item);

	    mf_hash_rem_item(&ht, item);
	    પ્રતિજ્ઞા(mf_hash_find(&ht, ચાવી) == ખાલી);

	    vim_free(item);
	}
    }

    // check again
    માટે (i = 0; i < TEST_ગણતરી; i++)
    {
	ચાવી = index_to_ચાવી(i);
	item = mf_hash_find(&ht, ચાવી);

	જો (i % 100 < 70)
	{
	    પ્રતિજ્ઞા(item == ખાલી);
	}
	નહીંતર
	{
	    પ્રતિજ્ઞા(item != ખાલી);
	    પ્રતિજ્ઞા(item->mhi_ચાવી == ચાવી);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    પૂર્ણાંક
મુખ્ય(શૂન્ય)
{
    test_mf_hash();
    પાછા 0;
}
