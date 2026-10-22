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
#شامل <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#شامل "main.c"

// This file has to be included because the tested functions are static
#شامل "memfile.c"

#define index_to_کلید(i) ((i) ^ 15167)
#define TEST_شمار 50000

/*
 * Test mf_hash_*() functions.
 */
    ایستا تهی
test_mf_hash(تهی)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      کلید;
    long_u	   i;
    long_u	   رقم_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    برای (i = 0; i < TEST_شمار; i++)
    {
	أكد(ht.mht_شمار == i);

	// check that number of buckets is a power of 2
	رقم_buckets = ht.mht_mask + 1;
	أكد(رقم_buckets > 0 && (رقم_buckets & (رقم_buckets - 1)) == 0);

	// check load factor
	أكد(ht.mht_شمار <= (رقم_buckets << MHT_LOG_LOAD_FACTOR));

	اگر (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    أكد(رقم_buckets == MHT_INIT_SIZE);
	    أكد(ht.mht_buckets == ht.mht_small_buckets);
	}
	وگرنه
	{
	    أكد(رقم_buckets > MHT_INIT_SIZE);
	    أكد(ht.mht_buckets != ht.mht_small_buckets);
	}

	کلید = index_to_کلید(i);
	أكد(mf_hash_find(&ht, کلید) == پوچ);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	أكد(item != پوچ);
	item->mhi_کلید = کلید;
	mf_hash_add_item(&ht, item);

	أكد(mf_hash_find(&ht, کلید) == item);

	اگر (ht.mht_mask + 1 != رقم_buckets)
	{
	    // hash table was expanded
	    أكد(ht.mht_mask + 1 == رقم_buckets * MHT_GROWTH_FACTOR);
	    أكد(i + 1 == (رقم_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    برای (i = 0; i < TEST_شمار; i++)
    {
	کلید = index_to_کلید(i);
	item = mf_hash_find(&ht, کلید);
	أكد(item != پوچ);
	أكد(item->mhi_کلید == کلید);
    }

    // delete some items
    برای (i = 0; i < TEST_شمار; i++)
    {
	اگر (i % 100 < 70)
	{
	    کلید = index_to_کلید(i);
	    item = mf_hash_find(&ht, کلید);
	    أكد(item != پوچ);
	    أكد(item->mhi_کلید == کلید);

	    mf_hash_rem_item(&ht, item);
	    أكد(mf_hash_find(&ht, کلید) == پوچ);

	    mf_hash_add_item(&ht, item);
	    أكد(mf_hash_find(&ht, کلید) == item);

	    mf_hash_rem_item(&ht, item);
	    أكد(mf_hash_find(&ht, کلید) == پوچ);

	    vim_free(item);
	}
    }

    // check again
    برای (i = 0; i < TEST_شمار; i++)
    {
	کلید = index_to_کلید(i);
	item = mf_hash_find(&ht, کلید);

	اگر (i % 100 < 70)
	{
	    أكد(item == پوچ);
	}
	وگرنه
	{
	    أكد(item != پوچ);
	    أكد(item->mhi_کلید == کلید);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    صحیح
اصلی(تهی)
{
    test_mf_hash();
    بازگشت 0;
}
