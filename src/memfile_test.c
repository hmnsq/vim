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
#অন্তর্ভাব <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#অন্তর্ভাব "main.c"

// This file has to be included because the tested functions are static
#অন্তর্ভাব "memfile.c"

#define index_to_চাবি(i) ((i) ^ 15167)
#define TEST_গুনতি 50000

/*
 * Test mf_hash_*() functions.
 */
    স্থির শূন্য
test_mf_hash(শূন্য)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      চাবি;
    long_u	   i;
    long_u	   অঙ্ক_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    প্রতি (i = 0; i < TEST_গুনতি; i++)
    {
	প্রতিজ্ঞা(ht.mht_গুনতি == i);

	// check that number of buckets is a power of 2
	অঙ্ক_buckets = ht.mht_mask + 1;
	প্রতিজ্ঞা(অঙ্ক_buckets > 0 && (অঙ্ক_buckets & (অঙ্ক_buckets - 1)) == 0);

	// check load factor
	প্রতিজ্ঞা(ht.mht_গুনতি <= (অঙ্ক_buckets << MHT_LOG_LOAD_FACTOR));

	যদি (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    প্রতিজ্ঞা(অঙ্ক_buckets == MHT_INIT_SIZE);
	    প্রতিজ্ঞা(ht.mht_buckets == ht.mht_small_buckets);
	}
	অন্যথা
	{
	    প্রতিজ্ঞা(অঙ্ক_buckets > MHT_INIT_SIZE);
	    প্রতিজ্ঞা(ht.mht_buckets != ht.mht_small_buckets);
	}

	চাবি = index_to_চাবি(i);
	প্রতিজ্ঞা(mf_hash_find(&ht, চাবি) == রিক্ত);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	প্রতিজ্ঞা(item != রিক্ত);
	item->mhi_চাবি = চাবি;
	mf_hash_add_item(&ht, item);

	প্রতিজ্ঞা(mf_hash_find(&ht, চাবি) == item);

	যদি (ht.mht_mask + 1 != অঙ্ক_buckets)
	{
	    // hash table was expanded
	    প্রতিজ্ঞা(ht.mht_mask + 1 == অঙ্ক_buckets * MHT_GROWTH_FACTOR);
	    প্রতিজ্ঞা(i + 1 == (অঙ্ক_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    প্রতি (i = 0; i < TEST_গুনতি; i++)
    {
	চাবি = index_to_চাবি(i);
	item = mf_hash_find(&ht, চাবি);
	প্রতিজ্ঞা(item != রিক্ত);
	প্রতিজ্ঞা(item->mhi_চাবি == চাবি);
    }

    // delete some items
    প্রতি (i = 0; i < TEST_গুনতি; i++)
    {
	যদি (i % 100 < 70)
	{
	    চাবি = index_to_চাবি(i);
	    item = mf_hash_find(&ht, চাবি);
	    প্রতিজ্ঞা(item != রিক্ত);
	    প্রতিজ্ঞা(item->mhi_চাবি == চাবি);

	    mf_hash_rem_item(&ht, item);
	    প্রতিজ্ঞা(mf_hash_find(&ht, চাবি) == রিক্ত);

	    mf_hash_add_item(&ht, item);
	    প্রতিজ্ঞা(mf_hash_find(&ht, চাবি) == item);

	    mf_hash_rem_item(&ht, item);
	    প্রতিজ্ঞা(mf_hash_find(&ht, চাবি) == রিক্ত);

	    vim_free(item);
	}
    }

    // check again
    প্রতি (i = 0; i < TEST_গুনতি; i++)
    {
	চাবি = index_to_চাবি(i);
	item = mf_hash_find(&ht, চাবি);

	যদি (i % 100 < 70)
	{
	    প্রতিজ্ঞা(item == রিক্ত);
	}
	অন্যথা
	{
	    প্রতিজ্ঞা(item != রিক্ত);
	    প্রতিজ্ঞা(item->mhi_চাবি == চাবি);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    পূর্ণাঙ্ক
main(শূন্য)
{
    test_mf_hash();
    প্রত্যাগম 0;
}
