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
#ضمن <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#ضمن "main.c"

// This file has to be included because the tested functions are static
#ضمن "memfile.c"

#define index_to_key(i) ((i) ^ 15167)
#define TEST_COUNT 50000

/*
 * Test mf_hash_*() functions.
 */
    ساكن فراغ
test_mf_hash(فراغ)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      key;
    long_u	   i;
    long_u	   num_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    עבור (i = 0; i < TEST_COUNT; i++)
    {
	أكد(ht.mht_count == i);

	// check that number of buckets is a power of 2
	num_buckets = ht.mht_mask + 1;
	أكد(num_buckets > 0 && (num_buckets & (num_buckets - 1)) == 0);

	// check load factor
	أكد(ht.mht_count <= (num_buckets << MHT_LOG_LOAD_FACTOR));

	אם (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    أكد(num_buckets == MHT_INIT_SIZE);
	    أكد(ht.mht_buckets == ht.mht_small_buckets);
	}
	אחרת
	{
	    أكد(num_buckets > MHT_INIT_SIZE);
	    أكد(ht.mht_buckets != ht.mht_small_buckets);
	}

	key = index_to_key(i);
	أكد(mf_hash_find(&ht, key) == عدم);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	أكد(item != عدم);
	item->mhi_key = key;
	mf_hash_add_item(&ht, item);

	أكد(mf_hash_find(&ht, key) == item);

	אם (ht.mht_mask + 1 != num_buckets)
	{
	    // hash table was expanded
	    أكد(ht.mht_mask + 1 == num_buckets * MHT_GROWTH_FACTOR);
	    أكد(i + 1 == (num_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    עבור (i = 0; i < TEST_COUNT; i++)
    {
	key = index_to_key(i);
	item = mf_hash_find(&ht, key);
	أكد(item != عدم);
	أكد(item->mhi_key == key);
    }

    // delete some items
    עבור (i = 0; i < TEST_COUNT; i++)
    {
	אם (i % 100 < 70)
	{
	    key = index_to_key(i);
	    item = mf_hash_find(&ht, key);
	    أكد(item != عدم);
	    أكد(item->mhi_key == key);

	    mf_hash_rem_item(&ht, item);
	    أكد(mf_hash_find(&ht, key) == عدم);

	    mf_hash_add_item(&ht, item);
	    أكد(mf_hash_find(&ht, key) == item);

	    mf_hash_rem_item(&ht, item);
	    أكد(mf_hash_find(&ht, key) == عدم);

	    vim_free(item);
	}
    }

    // check again
    עבור (i = 0; i < TEST_COUNT; i++)
    {
	key = index_to_key(i);
	item = mf_hash_find(&ht, key);

	אם (i % 100 < 70)
	{
	    أكد(item == عدم);
	}
	אחרת
	{
	    أكد(item != عدم);
	    أكد(item->mhi_key == key);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    صحيح
main(فراغ)
{
    test_mf_hash();
    החזר 0;
}
