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

#define index_to_कुंजी(i) ((i) ^ 15167)
#define TEST_गिनती 50000

/*
 * Test mf_hash_*() functions.
 */
    स्थिर शून्य
test_mf_hash(शून्य)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      कुंजी;
    long_u	   i;
    long_u	   अंक_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    प्रति (i = 0; i < TEST_गिनती; i++)
    {
	प्रतिज्ञा(ht.mht_गिनती == i);

	// check that number of buckets is a power of 2
	अंक_buckets = ht.mht_mask + 1;
	प्रतिज्ञा(अंक_buckets > 0 && (अंक_buckets & (अंक_buckets - 1)) == 0);

	// check load factor
	प्रतिज्ञा(ht.mht_गिनती <= (अंक_buckets << MHT_LOG_LOAD_FACTOR));

	यदि (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
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

	कुंजी = index_to_कुंजी(i);
	प्रतिज्ञा(mf_hash_find(&ht, कुंजी) == रिक्त);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	प्रतिज्ञा(item != रिक्त);
	item->mhi_कुंजी = कुंजी;
	mf_hash_add_item(&ht, item);

	प्रतिज्ञा(mf_hash_find(&ht, कुंजी) == item);

	यदि (ht.mht_mask + 1 != अंक_buckets)
	{
	    // hash table was expanded
	    प्रतिज्ञा(ht.mht_mask + 1 == अंक_buckets * MHT_GROWTH_FACTOR);
	    प्रतिज्ञा(i + 1 == (अंक_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    प्रति (i = 0; i < TEST_गिनती; i++)
    {
	कुंजी = index_to_कुंजी(i);
	item = mf_hash_find(&ht, कुंजी);
	प्रतिज्ञा(item != रिक्त);
	प्रतिज्ञा(item->mhi_कुंजी == कुंजी);
    }

    // delete some items
    प्रति (i = 0; i < TEST_गिनती; i++)
    {
	यदि (i % 100 < 70)
	{
	    कुंजी = index_to_कुंजी(i);
	    item = mf_hash_find(&ht, कुंजी);
	    प्रतिज्ञा(item != रिक्त);
	    प्रतिज्ञा(item->mhi_कुंजी == कुंजी);

	    mf_hash_rem_item(&ht, item);
	    प्रतिज्ञा(mf_hash_find(&ht, कुंजी) == रिक्त);

	    mf_hash_add_item(&ht, item);
	    प्रतिज्ञा(mf_hash_find(&ht, कुंजी) == item);

	    mf_hash_rem_item(&ht, item);
	    प्रतिज्ञा(mf_hash_find(&ht, कुंजी) == रिक्त);

	    vim_free(item);
	}
    }

    // check again
    प्रति (i = 0; i < TEST_गिनती; i++)
    {
	कुंजी = index_to_कुंजी(i);
	item = mf_hash_find(&ht, कुंजी);

	यदि (i % 100 < 70)
	{
	    प्रतिज्ञा(item == रिक्त);
	}
	अन्यथा
	{
	    प्रतिज्ञा(item != रिक्त);
	    प्रतिज्ञा(item->mhi_कुंजी == कुंजी);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    पूर्णाङ्क
main(शून्य)
{
    test_mf_hash();
    प्रत्यागम 0;
}
