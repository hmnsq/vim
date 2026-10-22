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
#ಅನ್ತರ್ಭಾವ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#ಅನ್ತರ್ಭಾವ "main.c"

// This file has to be included because the tested functions are static
#ಅನ್ತರ್ಭಾವ "memfile.c"

#define index_to_ಕೀಲಿ(i) ((i) ^ 15167)
#define TEST_ಎಣಿಕೆ 50000

/*
 * Test mf_hash_*() functions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
test_mf_hash(ಶೂನ್ಯ)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      ಕೀಲಿ;
    long_u	   i;
    long_u	   ಅಂಕೆ_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    ಪ್ರತಿ (i = 0; i < TEST_ಎಣಿಕೆ; i++)
    {
	ಪ್ರತಿಜ್ಞಾ(ht.mht_ಎಣಿಕೆ == i);

	// check that number of buckets is a power of 2
	ಅಂಕೆ_buckets = ht.mht_mask + 1;
	ಪ್ರತಿಜ್ಞಾ(ಅಂಕೆ_buckets > 0 && (ಅಂಕೆ_buckets & (ಅಂಕೆ_buckets - 1)) == 0);

	// check load factor
	ಪ್ರತಿಜ್ಞಾ(ht.mht_ಎಣಿಕೆ <= (ಅಂಕೆ_buckets << MHT_LOG_LOAD_FACTOR));

	ಯದಿ (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    ಪ್ರತಿಜ್ಞಾ(ಅಂಕೆ_buckets == MHT_INIT_SIZE);
	    ಪ್ರತಿಜ್ಞಾ(ht.mht_buckets == ht.mht_small_buckets);
	}
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿಜ್ಞಾ(ಅಂಕೆ_buckets > MHT_INIT_SIZE);
	    ಪ್ರತಿಜ್ಞಾ(ht.mht_buckets != ht.mht_small_buckets);
	}

	ಕೀಲಿ = index_to_ಕೀಲಿ(i);
	ಪ್ರತಿಜ್ಞಾ(mf_hash_find(&ht, ಕೀಲಿ) == ರಿಕ್ತ);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	ಪ್ರತಿಜ್ಞಾ(item != ರಿಕ್ತ);
	item->mhi_ಕೀಲಿ = ಕೀಲಿ;
	mf_hash_add_item(&ht, item);

	ಪ್ರತಿಜ್ಞಾ(mf_hash_find(&ht, ಕೀಲಿ) == item);

	ಯದಿ (ht.mht_mask + 1 != ಅಂಕೆ_buckets)
	{
	    // hash table was expanded
	    ಪ್ರತಿಜ್ಞಾ(ht.mht_mask + 1 == ಅಂಕೆ_buckets * MHT_GROWTH_FACTOR);
	    ಪ್ರತಿಜ್ಞಾ(i + 1 == (ಅಂಕೆ_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    ಪ್ರತಿ (i = 0; i < TEST_ಎಣಿಕೆ; i++)
    {
	ಕೀಲಿ = index_to_ಕೀಲಿ(i);
	item = mf_hash_find(&ht, ಕೀಲಿ);
	ಪ್ರತಿಜ್ಞಾ(item != ರಿಕ್ತ);
	ಪ್ರತಿಜ್ಞಾ(item->mhi_ಕೀಲಿ == ಕೀಲಿ);
    }

    // delete some items
    ಪ್ರತಿ (i = 0; i < TEST_ಎಣಿಕೆ; i++)
    {
	ಯದಿ (i % 100 < 70)
	{
	    ಕೀಲಿ = index_to_ಕೀಲಿ(i);
	    item = mf_hash_find(&ht, ಕೀಲಿ);
	    ಪ್ರತಿಜ್ಞಾ(item != ರಿಕ್ತ);
	    ಪ್ರತಿಜ್ಞಾ(item->mhi_ಕೀಲಿ == ಕೀಲಿ);

	    mf_hash_rem_item(&ht, item);
	    ಪ್ರತಿಜ್ಞಾ(mf_hash_find(&ht, ಕೀಲಿ) == ರಿಕ್ತ);

	    mf_hash_add_item(&ht, item);
	    ಪ್ರತಿಜ್ಞಾ(mf_hash_find(&ht, ಕೀಲಿ) == item);

	    mf_hash_rem_item(&ht, item);
	    ಪ್ರತಿಜ್ಞಾ(mf_hash_find(&ht, ಕೀಲಿ) == ರಿಕ್ತ);

	    vim_free(item);
	}
    }

    // check again
    ಪ್ರತಿ (i = 0; i < TEST_ಎಣಿಕೆ; i++)
    {
	ಕೀಲಿ = index_to_ಕೀಲಿ(i);
	item = mf_hash_find(&ht, ಕೀಲಿ);

	ಯದಿ (i % 100 < 70)
	{
	    ಪ್ರತಿಜ್ಞಾ(item == ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿಜ್ಞಾ(item != ರಿಕ್ತ);
	    ಪ್ರತಿಜ್ಞಾ(item->mhi_ಕೀಲಿ == ಕೀಲಿ);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    ಪೂರ್ಣಾಙ್ಕ
main(ಶೂನ್ಯ)
{
    test_mf_hash();
    ಪ್ರತ್ಯಾಗಮ 0;
}
