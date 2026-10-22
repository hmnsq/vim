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
#ଅନ୍ତର୍ଭାଵ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#ଅନ୍ତର୍ଭାଵ "main.c"

// This file has to be included because the tested functions are static
#ଅନ୍ତର୍ଭାଵ "memfile.c"

#define index_to_ଚାବି(i) ((i) ^ 15167)
#define TEST_ଗଣନା 50000

/*
 * Test mf_hash_*() functions.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
test_mf_hash(ଶୂନ୍ୟ)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      ଚାବି;
    long_u	   i;
    long_u	   ଅଙ୍କ_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    ପାଇଁ (i = 0; i < TEST_ଗଣନା; i++)
    {
	ପ୍ରତିଜ୍ଞା(ht.mht_ଗଣନା == i);

	// check that number of buckets is a power of 2
	ଅଙ୍କ_buckets = ht.mht_mask + 1;
	ପ୍ରତିଜ୍ଞା(ଅଙ୍କ_buckets > 0 && (ଅଙ୍କ_buckets & (ଅଙ୍କ_buckets - 1)) == 0);

	// check load factor
	ପ୍ରତିଜ୍ଞା(ht.mht_ଗଣନା <= (ଅଙ୍କ_buckets << MHT_LOG_LOAD_FACTOR));

	ଯଦି (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    ପ୍ରତିଜ୍ଞା(ଅଙ୍କ_buckets == MHT_INIT_SIZE);
	    ପ୍ରତିଜ୍ଞା(ht.mht_buckets == ht.mht_small_buckets);
	}
	ଅନ୍ୟଥା
	{
	    ପ୍ରତିଜ୍ଞା(ଅଙ୍କ_buckets > MHT_INIT_SIZE);
	    ପ୍ରତିଜ୍ଞା(ht.mht_buckets != ht.mht_small_buckets);
	}

	ଚାବି = index_to_ଚାବି(i);
	ପ୍ରତିଜ୍ଞା(mf_hash_find(&ht, ଚାବି) == ଖାଲି);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	ପ୍ରତିଜ୍ଞା(item != ଖାଲି);
	item->mhi_ଚାବି = ଚାବି;
	mf_hash_add_item(&ht, item);

	ପ୍ରତିଜ୍ଞା(mf_hash_find(&ht, ଚାବି) == item);

	ଯଦି (ht.mht_mask + 1 != ଅଙ୍କ_buckets)
	{
	    // hash table was expanded
	    ପ୍ରତିଜ୍ଞା(ht.mht_mask + 1 == ଅଙ୍କ_buckets * MHT_GROWTH_FACTOR);
	    ପ୍ରତିଜ୍ଞା(i + 1 == (ଅଙ୍କ_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    ପାଇଁ (i = 0; i < TEST_ଗଣନା; i++)
    {
	ଚାବି = index_to_ଚାବି(i);
	item = mf_hash_find(&ht, ଚାବି);
	ପ୍ରତିଜ୍ଞା(item != ଖାଲି);
	ପ୍ରତିଜ୍ଞା(item->mhi_ଚାବି == ଚାବି);
    }

    // delete some items
    ପାଇଁ (i = 0; i < TEST_ଗଣନା; i++)
    {
	ଯଦି (i % 100 < 70)
	{
	    ଚାବି = index_to_ଚାବି(i);
	    item = mf_hash_find(&ht, ଚାବି);
	    ପ୍ରତିଜ୍ଞା(item != ଖାଲି);
	    ପ୍ରତିଜ୍ଞା(item->mhi_ଚାବି == ଚାବି);

	    mf_hash_rem_item(&ht, item);
	    ପ୍ରତିଜ୍ଞା(mf_hash_find(&ht, ଚାବି) == ଖାଲି);

	    mf_hash_add_item(&ht, item);
	    ପ୍ରତିଜ୍ଞା(mf_hash_find(&ht, ଚାବି) == item);

	    mf_hash_rem_item(&ht, item);
	    ପ୍ରତିଜ୍ଞା(mf_hash_find(&ht, ଚାବି) == ଖାଲି);

	    vim_free(item);
	}
    }

    // check again
    ପାଇଁ (i = 0; i < TEST_ଗଣନା; i++)
    {
	ଚାବି = index_to_ଚାବି(i);
	item = mf_hash_find(&ht, ଚାବି);

	ଯଦି (i % 100 < 70)
	{
	    ପ୍ରତିଜ୍ଞା(item == ଖାଲି);
	}
	ଅନ୍ୟଥା
	{
	    ପ୍ରତିଜ୍ଞା(item != ଖାଲି);
	    ପ୍ରତିଜ୍ଞା(item->mhi_ଚାବି == ଚାବି);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ଶୂନ୍ୟ)
{
    test_mf_hash();
    ଫେରନ୍ତୁ 0;
}
