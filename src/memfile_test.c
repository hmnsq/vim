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
#अन्तर्भाव <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#अन्तर्भाव "main.c"

// This file has to be included because the tested functions are static
#अन्तर्भाव "memfile.c"

#define index_to_कुञ्चिका(i) ((i) ^ 15167)
#define TEST_गणनम् 50000

/*
 * Test mf_hash_*() functions.
 */
    स्थिर शून्य
test_mf_hash(शून्य)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      कुञ्चिका;
    long_u	   i;
    long_u	   अङ्कः_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    अत्थाय (i = 0; i < TEST_गणनम्; i++)
    {
	प्रतिज्ञा(ht.mht_गणनम् == i);

	// check that number of buckets is a power of 2
	अङ्कः_buckets = ht.mht_mask + 1;
	प्रतिज्ञा(अङ्कः_buckets > 0 && (अङ्कः_buckets & (अङ्कः_buckets - 1)) == 0);

	// check load factor
	प्रतिज्ञा(ht.mht_गणनम् <= (अङ्कः_buckets << MHT_LOG_LOAD_FACTOR));

	सचे (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    प्रतिज्ञा(अङ्कः_buckets == MHT_INIT_SIZE);
	    प्रतिज्ञा(ht.mht_buckets == ht.mht_small_buckets);
	}
	अञ्ञथा
	{
	    प्रतिज्ञा(अङ्कः_buckets > MHT_INIT_SIZE);
	    प्रतिज्ञा(ht.mht_buckets != ht.mht_small_buckets);
	}

	कुञ्चिका = index_to_कुञ्चिका(i);
	प्रतिज्ञा(mf_hash_find(&ht, कुञ्चिका) == रिक्त);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	प्रतिज्ञा(item != रिक्त);
	item->mhi_कुञ्चिका = कुञ्चिका;
	mf_hash_add_item(&ht, item);

	प्रतिज्ञा(mf_hash_find(&ht, कुञ्चिका) == item);

	सचे (ht.mht_mask + 1 != अङ्कः_buckets)
	{
	    // hash table was expanded
	    प्रतिज्ञा(ht.mht_mask + 1 == अङ्कः_buckets * MHT_GROWTH_FACTOR);
	    प्रतिज्ञा(i + 1 == (अङ्कः_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    अत्थाय (i = 0; i < TEST_गणनम्; i++)
    {
	कुञ्चिका = index_to_कुञ्चिका(i);
	item = mf_hash_find(&ht, कुञ्चिका);
	प्रतिज्ञा(item != रिक्त);
	प्रतिज्ञा(item->mhi_कुञ्चिका == कुञ्चिका);
    }

    // delete some items
    अत्थाय (i = 0; i < TEST_गणनम्; i++)
    {
	सचे (i % 100 < 70)
	{
	    कुञ्चिका = index_to_कुञ्चिका(i);
	    item = mf_hash_find(&ht, कुञ्चिका);
	    प्रतिज्ञा(item != रिक्त);
	    प्रतिज्ञा(item->mhi_कुञ्चिका == कुञ्चिका);

	    mf_hash_rem_item(&ht, item);
	    प्रतिज्ञा(mf_hash_find(&ht, कुञ्चिका) == रिक्त);

	    mf_hash_add_item(&ht, item);
	    प्रतिज्ञा(mf_hash_find(&ht, कुञ्चिका) == item);

	    mf_hash_rem_item(&ht, item);
	    प्रतिज्ञा(mf_hash_find(&ht, कुञ्चिका) == रिक्त);

	    vim_free(item);
	}
    }

    // check again
    अत्थाय (i = 0; i < TEST_गणनम्; i++)
    {
	कुञ्चिका = index_to_कुञ्चिका(i);
	item = mf_hash_find(&ht, कुञ्चिका);

	सचे (i % 100 < 70)
	{
	    प्रतिज्ञा(item == रिक्त);
	}
	अञ्ञथा
	{
	    प्रतिज्ञा(item != रिक्त);
	    प्रतिज्ञा(item->mhi_कुञ्चिका == कुञ्चिका);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    पूर्णाङ्क
main(शून्य)
{
    test_mf_hash();
    पटिनिवत्त 0;
}
