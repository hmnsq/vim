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

#define index_to_ئاچقۇچ(i) ((i) ^ 15167)
#define TEST_سانى 50000

/*
 * Test mf_hash_*() functions.
 */
    ساكن فراغ
test_mf_hash(فراغ)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      ئاچقۇچ;
    long_u	   i;
    long_u	   رەقەم_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    لكل (i = 0; i < TEST_سانى; i++)
    {
	أكد(ht.mht_سانى == i);

	// check that number of buckets is a power of 2
	رەقەم_buckets = ht.mht_mask + 1;
	أكد(رەقەم_buckets > 0 && (رەقەم_buckets & (رەقەم_buckets - 1)) == 0);

	// check load factor
	أكد(ht.mht_سانى <= (رەقەم_buckets << MHT_LOG_LOAD_FACTOR));

	إذا (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    أكد(رەقەم_buckets == MHT_INIT_SIZE);
	    أكد(ht.mht_buckets == ht.mht_small_buckets);
	}
	وإلا
	{
	    أكد(رەقەم_buckets > MHT_INIT_SIZE);
	    أكد(ht.mht_buckets != ht.mht_small_buckets);
	}

	ئاچقۇچ = index_to_ئاچقۇچ(i);
	أكد(mf_hash_find(&ht, ئاچقۇچ) == عدم);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	أكد(item != عدم);
	item->mhi_ئاچقۇچ = ئاچقۇچ;
	mf_hash_add_item(&ht, item);

	أكد(mf_hash_find(&ht, ئاچقۇچ) == item);

	إذا (ht.mht_mask + 1 != رەقەم_buckets)
	{
	    // hash table was expanded
	    أكد(ht.mht_mask + 1 == رەقەم_buckets * MHT_GROWTH_FACTOR);
	    أكد(i + 1 == (رەقەم_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    لكل (i = 0; i < TEST_سانى; i++)
    {
	ئاچقۇچ = index_to_ئاچقۇچ(i);
	item = mf_hash_find(&ht, ئاچقۇچ);
	أكد(item != عدم);
	أكد(item->mhi_ئاچقۇچ == ئاچقۇچ);
    }

    // delete some items
    لكل (i = 0; i < TEST_سانى; i++)
    {
	إذا (i % 100 < 70)
	{
	    ئاچقۇچ = index_to_ئاچقۇچ(i);
	    item = mf_hash_find(&ht, ئاچقۇچ);
	    أكد(item != عدم);
	    أكد(item->mhi_ئاچقۇچ == ئاچقۇچ);

	    mf_hash_rem_item(&ht, item);
	    أكد(mf_hash_find(&ht, ئاچقۇچ) == عدم);

	    mf_hash_add_item(&ht, item);
	    أكد(mf_hash_find(&ht, ئاچقۇچ) == item);

	    mf_hash_rem_item(&ht, item);
	    أكد(mf_hash_find(&ht, ئاچقۇچ) == عدم);

	    vim_free(item);
	}
    }

    // check again
    لكل (i = 0; i < TEST_سانى; i++)
    {
	ئاچقۇچ = index_to_ئاچقۇچ(i);
	item = mf_hash_find(&ht, ئاچقۇچ);

	إذا (i % 100 < 70)
	{
	    أكد(item == عدم);
	}
	وإلا
	{
	    أكد(item != عدم);
	    أكد(item->mhi_ئاچقۇچ == ئاچقۇچ);
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
