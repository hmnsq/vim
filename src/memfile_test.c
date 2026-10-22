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

#నిర్వచనం_రద్దు NDEBUG
#అన్తర్భావ <assert.h>

// Must include main.c because it contains much more than just main()
#నిర్వచించు NO_VIM_MAIN
#అన్తర్భావ "main.c"

// This file has to be included because the tested functions are static
#అన్తర్భావ "memfile.c"

#నిర్వచించు సూచీ_గమ్యం_కీలకం(i) ((i) ^ 15167)
#నిర్వచించు TEST_లెక్క 50000

/*
 * Test mf_hash_*() functions.
 */
    స్థిర శూన్య
test_mf_hash(శూన్య)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *అంశం;
    blocknr_T      కీలకం;
    long_u	   i;
    long_u	   అంకె_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    కొరకు (i = 0; i < TEST_లెక్క; i++)
    {
	ప్రతిజ్ఞా(ht.mht_లెక్క == i);

	// check that number of buckets is a power of 2
	అంకె_buckets = ht.mht_ముసుగు + 1;
	ప్రతిజ్ఞా(అంకె_buckets > 0 && (అంకె_buckets & (అంకె_buckets - 1)) == 0);

	// check load factor
	ప్రతిజ్ఞా(ht.mht_లెక్క <= (అంకె_buckets << MHT_LOG_ఎక్కించు_గుణకం));

	ఒకవేళ (i < (MHT_INIT_కొలత << MHT_LOG_ఎక్కించు_గుణకం))
	{
	    // first expansion shouldn't have occurred yet
	    ప్రతిజ్ఞా(అంకె_buckets == MHT_INIT_కొలత);
	    ప్రతిజ్ఞా(ht.mht_buckets == ht.mht_small_buckets);
	}
	లేకపోతే
	{
	    ప్రతిజ్ఞా(అంకె_buckets > MHT_INIT_కొలత);
	    ప్రతిజ్ఞా(ht.mht_buckets != ht.mht_small_buckets);
	}

	కీలకం = సూచీ_గమ్యం_కీలకం(i);
	ప్రతిజ్ఞా(mf_hash_కనుగొను(&ht, కీలకం) == ఖాళీ);

	// allocate and add new item
	అంశం = LALLOC_CLEAR_ONE(mf_hashitem_T);
	ప్రతిజ్ఞా(అంశం != ఖాళీ);
	అంశం->mhi_కీలకం = కీలకం;
	mf_hash_కూడు_అంశం(&ht, అంశం);

	ప్రతిజ్ఞా(mf_hash_కనుగొను(&ht, కీలకం) == అంశం);

	ఒకవేళ (ht.mht_ముసుగు + 1 != అంకె_buckets)
	{
	    // hash table was expanded
	    ప్రతిజ్ఞా(ht.mht_ముసుగు + 1 == అంకె_buckets * MHT_GROWTH_గుణకం);
	    ప్రతిజ్ఞా(i + 1 == (అంకె_buckets << MHT_LOG_ఎక్కించు_గుణకం));
	}
    }

    // check presence of inserted items
    కొరకు (i = 0; i < TEST_లెక్క; i++)
    {
	కీలకం = సూచీ_గమ్యం_కీలకం(i);
	అంశం = mf_hash_కనుగొను(&ht, కీలకం);
	ప్రతిజ్ఞా(అంశం != ఖాళీ);
	ప్రతిజ్ఞా(అంశం->mhi_కీలకం == కీలకం);
    }

    // delete some items
    కొరకు (i = 0; i < TEST_లెక్క; i++)
    {
	ఒకవేళ (i % 100 < 70)
	{
	    కీలకం = సూచీ_గమ్యం_కీలకం(i);
	    అంశం = mf_hash_కనుగొను(&ht, కీలకం);
	    ప్రతిజ్ఞా(అంశం != ఖాళీ);
	    ప్రతిజ్ఞా(అంశం->mhi_కీలకం == కీలకం);

	    mf_hash_rem_అంశం(&ht, అంశం);
	    ప్రతిజ్ఞా(mf_hash_కనుగొను(&ht, కీలకం) == ఖాళీ);

	    mf_hash_కూడు_అంశం(&ht, అంశం);
	    ప్రతిజ్ఞా(mf_hash_కనుగొను(&ht, కీలకం) == అంశం);

	    mf_hash_rem_అంశం(&ht, అంశం);
	    ప్రతిజ్ఞా(mf_hash_కనుగొను(&ht, కీలకం) == ఖాళీ);

	    vim_free(అంశం);
	}
    }

    // check again
    కొరకు (i = 0; i < TEST_లెక్క; i++)
    {
	కీలకం = సూచీ_గమ్యం_కీలకం(i);
	అంశం = mf_hash_కనుగొను(&ht, కీలకం);

	ఒకవేళ (i % 100 < 70)
	{
	    ప్రతిజ్ఞా(అంశం == ఖాళీ);
	}
	లేకపోతే
	{
	    ప్రతిజ్ఞా(అంశం != ఖాళీ);
	    ప్రతిజ్ఞా(అంశం->mhi_కీలకం == కీలకం);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    పూర్ణాంకం
ప్రధాన(శూన్య)
{
    test_mf_hash();
    తిరిగి 0;
}
