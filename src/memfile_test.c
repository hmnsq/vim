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

#define index_to_مفتاح(i) ((i) ^ 15167)
#define TEST_عدد 50000

/*
 * Test mf_hash_*() functions.
 */
    ساكن فراغ
test_mf_hash(فراغ)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      مفتاح;
    long_u	   i;
    long_u	   رقم_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    لكل (i = 0; i < TEST_عدد; i++)
    {
	أكد(ht.mht_عدد == i);

	// check that number of buckets is a power of 2
	رقم_buckets = ht.mht_mask + 1;
	أكد(رقم_buckets > 0 && (رقم_buckets & (رقم_buckets - 1)) == 0);

	// check load factor
	أكد(ht.mht_عدد <= (رقم_buckets << MHT_LOG_LOAD_FACTOR));

	إذا (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    أكد(رقم_buckets == MHT_INIT_SIZE);
	    أكد(ht.mht_buckets == ht.mht_small_buckets);
	}
	وإلا
	{
	    أكد(رقم_buckets > MHT_INIT_SIZE);
	    أكد(ht.mht_buckets != ht.mht_small_buckets);
	}

	مفتاح = index_to_مفتاح(i);
	أكد(mf_hash_find(&ht, مفتاح) == عدم);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	أكد(item != عدم);
	item->mhi_مفتاح = مفتاح;
	mf_hash_add_item(&ht, item);

	أكد(mf_hash_find(&ht, مفتاح) == item);

	إذا (ht.mht_mask + 1 != رقم_buckets)
	{
	    // hash table was expanded
	    أكد(ht.mht_mask + 1 == رقم_buckets * MHT_GROWTH_FACTOR);
	    أكد(i + 1 == (رقم_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    لكل (i = 0; i < TEST_عدد; i++)
    {
	مفتاح = index_to_مفتاح(i);
	item = mf_hash_find(&ht, مفتاح);
	أكد(item != عدم);
	أكد(item->mhi_مفتاح == مفتاح);
    }

    // delete some items
    لكل (i = 0; i < TEST_عدد; i++)
    {
	إذا (i % 100 < 70)
	{
	    مفتاح = index_to_مفتاح(i);
	    item = mf_hash_find(&ht, مفتاح);
	    أكد(item != عدم);
	    أكد(item->mhi_مفتاح == مفتاح);

	    mf_hash_rem_item(&ht, item);
	    أكد(mf_hash_find(&ht, مفتاح) == عدم);

	    mf_hash_add_item(&ht, item);
	    أكد(mf_hash_find(&ht, مفتاح) == item);

	    mf_hash_rem_item(&ht, item);
	    أكد(mf_hash_find(&ht, مفتاح) == عدم);

	    vim_free(item);
	}
    }

    // check again
    لكل (i = 0; i < TEST_عدد; i++)
    {
	مفتاح = index_to_مفتاح(i);
	item = mf_hash_find(&ht, مفتاح);

	إذا (i % 100 < 70)
	{
	    أكد(item == عدم);
	}
	وإلا
	{
	    أكد(item != عدم);
	    أكد(item->mhi_مفتاح == مفتاح);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    صحيح
main(فراغ)
{
    test_mf_hash();
    أرجع 0;
}
