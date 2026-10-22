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
#அந்தர்பாவ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#அந்தர்பாவ "main.c"

// This file has to be included because the tested functions are static
#அந்தர்பாவ "memfile.c"

#define index_to_சாவி(i) ((i) ^ 15167)
#define TEST_மொத்தம் 50000

/*
 * Test mf_hash_*() functions.
 */
    நிலையான வெறுமை
test_mf_hash(வெறுமை)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      சாவி;
    long_u	   i;
    long_u	   இலக்கம்_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    க்கு (i = 0; i < TEST_மொத்தம்; i++)
    {
	ப்ரதிஜ்ஞா(ht.mht_மொத்தம் == i);

	// check that number of buckets is a power of 2
	இலக்கம்_buckets = ht.mht_mask + 1;
	ப்ரதிஜ்ஞா(இலக்கம்_buckets > 0 && (இலக்கம்_buckets & (இலக்கம்_buckets - 1)) == 0);

	// check load factor
	ப்ரதிஜ்ஞா(ht.mht_மொத்தம் <= (இலக்கம்_buckets << MHT_LOG_LOAD_FACTOR));

	எனில் (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    ப்ரதிஜ்ஞா(இலக்கம்_buckets == MHT_INIT_SIZE);
	    ப்ரதிஜ்ஞா(ht.mht_buckets == ht.mht_small_buckets);
	}
	இல்லையெனில்
	{
	    ப்ரதிஜ்ஞா(இலக்கம்_buckets > MHT_INIT_SIZE);
	    ப்ரதிஜ்ஞா(ht.mht_buckets != ht.mht_small_buckets);
	}

	சாவி = index_to_சாவி(i);
	ப்ரதிஜ்ஞா(mf_hash_find(&ht, சாவி) == வெற்று);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	ப்ரதிஜ்ஞா(item != வெற்று);
	item->mhi_சாவி = சாவி;
	mf_hash_add_item(&ht, item);

	ப்ரதிஜ்ஞா(mf_hash_find(&ht, சாவி) == item);

	எனில் (ht.mht_mask + 1 != இலக்கம்_buckets)
	{
	    // hash table was expanded
	    ப்ரதிஜ்ஞா(ht.mht_mask + 1 == இலக்கம்_buckets * MHT_GROWTH_FACTOR);
	    ப்ரதிஜ்ஞா(i + 1 == (இலக்கம்_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    க்கு (i = 0; i < TEST_மொத்தம்; i++)
    {
	சாவி = index_to_சாவி(i);
	item = mf_hash_find(&ht, சாவி);
	ப்ரதிஜ்ஞா(item != வெற்று);
	ப்ரதிஜ்ஞா(item->mhi_சாவி == சாவி);
    }

    // delete some items
    க்கு (i = 0; i < TEST_மொத்தம்; i++)
    {
	எனில் (i % 100 < 70)
	{
	    சாவி = index_to_சாவி(i);
	    item = mf_hash_find(&ht, சாவி);
	    ப்ரதிஜ்ஞா(item != வெற்று);
	    ப்ரதிஜ்ஞா(item->mhi_சாவி == சாவி);

	    mf_hash_rem_item(&ht, item);
	    ப்ரதிஜ்ஞா(mf_hash_find(&ht, சாவி) == வெற்று);

	    mf_hash_add_item(&ht, item);
	    ப்ரதிஜ்ஞா(mf_hash_find(&ht, சாவி) == item);

	    mf_hash_rem_item(&ht, item);
	    ப்ரதிஜ்ஞா(mf_hash_find(&ht, சாவி) == வெற்று);

	    vim_free(item);
	}
    }

    // check again
    க்கு (i = 0; i < TEST_மொத்தம்; i++)
    {
	சாவி = index_to_சாவி(i);
	item = mf_hash_find(&ht, சாவி);

	எனில் (i % 100 < 70)
	{
	    ப்ரதிஜ்ஞா(item == வெற்று);
	}
	இல்லையெனில்
	{
	    ப்ரதிஜ்ஞா(item != வெற்று);
	    ப்ரதிஜ்ஞா(item->mhi_சாவி == சாவி);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    முழு
முதன்மை(வெறுமை)
{
    test_mf_hash();
    திரும்பு 0;
}
