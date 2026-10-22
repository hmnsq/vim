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

#undef न_संशोधन
#अन्तर्भाव <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#अन्तर्भाव "main.c"

// This file has to be included because the tested functions are static
#अन्तर्भाव "memfile.c"

#define index_to_किल्ली(i) ((i) ^ 15167)
#define TEST_मोजणी 50000

/*
 * Test mf_hash_*() functions.
 */
    स्थिर शून्य
test_mf_hash(शून्य)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      किल्ली;
    long_u	   i;
    long_u	   अंक_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    साठी (i = 0; i < TEST_मोजणी; i++)
    {
	प्रतिज्ञा(ht.mht_मोजणी == i);

	// check that number of buckets is a power of 2
	अंक_buckets = ht.mht_mask + 1;
	प्रतिज्ञा(अंक_buckets > 0 && (अंक_buckets & (अंक_buckets - 1)) == 0);

	// check load factor
	प्रतिज्ञा(ht.mht_मोजणी <= (अंक_buckets << MHT_LOG_LOAD_FACTOR));

	जर (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    प्रतिज्ञा(अंक_buckets == MHT_INIT_SIZE);
	    प्रतिज्ञा(ht.mht_buckets == ht.mht_small_buckets);
	}
	अन्यथा
	{
	    प्रतिज्ञा(अंक_buckets > MHT_INIT_SIZE);
	    प्रतिज्ञा(ht.mht_buckets != ht.mht_small_buckets);
	}

	किल्ली = index_to_किल्ली(i);
	प्रतिज्ञा(mf_hash_find(&ht, किल्ली) == रिक्त);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	प्रतिज्ञा(item != रिक्त);
	item->mhi_किल्ली = किल्ली;
	mf_hash_add_item(&ht, item);

	प्रतिज्ञा(mf_hash_find(&ht, किल्ली) == item);

	जर (ht.mht_mask + 1 != अंक_buckets)
	{
	    // hash table was expanded
	    प्रतिज्ञा(ht.mht_mask + 1 == अंक_buckets * MHT_GROWTH_FACTOR);
	    प्रतिज्ञा(i + 1 == (अंक_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    साठी (i = 0; i < TEST_मोजणी; i++)
    {
	किल्ली = index_to_किल्ली(i);
	item = mf_hash_find(&ht, किल्ली);
	प्रतिज्ञा(item != रिक्त);
	प्रतिज्ञा(item->mhi_किल्ली == किल्ली);
    }

    // delete some items
    साठी (i = 0; i < TEST_मोजणी; i++)
    {
	जर (i % 100 < 70)
	{
	    किल्ली = index_to_किल्ली(i);
	    item = mf_hash_find(&ht, किल्ली);
	    प्रतिज्ञा(item != रिक्त);
	    प्रतिज्ञा(item->mhi_किल्ली == किल्ली);

	    mf_hash_rem_item(&ht, item);
	    प्रतिज्ञा(mf_hash_find(&ht, किल्ली) == रिक्त);

	    mf_hash_add_item(&ht, item);
	    प्रतिज्ञा(mf_hash_find(&ht, किल्ली) == item);

	    mf_hash_rem_item(&ht, item);
	    प्रतिज्ञा(mf_hash_find(&ht, किल्ली) == रिक्त);

	    vim_free(item);
	}
    }

    // check again
    साठी (i = 0; i < TEST_मोजणी; i++)
    {
	किल्ली = index_to_किल्ली(i);
	item = mf_hash_find(&ht, किल्ली);

	जर (i % 100 < 70)
	{
	    प्रतिज्ञा(item == रिक्त);
	}
	अन्यथा
	{
	    प्रतिज्ञा(item != रिक्त);
	    प्रतिज्ञा(item->mhi_किल्ली == किल्ली);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    पूर्णांक
main(शून्य)
{
    test_mf_hash();
    परत 0;
}
