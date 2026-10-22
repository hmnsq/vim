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

#وصف_رد NDEBUG
#ضمن <assert.h>

// Must include main.c because it contains much more than just main()
#وصف_ڪريو NO_VIM_MAIN
#ضمن "main.c"

// This file has to be included because the tested functions are static
#ضمن "memfile.c"

#وصف_ڪريو اشاريو_منزل_ڪنجي(i) ((i) ^ 15167)
#وصف_ڪريو TEST_ڳڻپ 50000

/*
 * Test mf_hash_*() functions.
 */
    ساكن فراغ
test_mf_hash(فراغ)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *شي;
    blocknr_T      ڪنجي;
    long_u	   i;
    long_u	   انگ_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    لكل (i = 0; i < TEST_ڳڻپ; i++)
    {
	أكد(ht.mht_ڳڻپ == i);

	// check that number of buckets is a power of 2
	انگ_buckets = ht.mht_نقاب + 1;
	أكد(انگ_buckets > 0 && (انگ_buckets & (انگ_buckets - 1)) == 0);

	// check load factor
	أكد(ht.mht_ڳڻپ <= (انگ_buckets << MHT_LOG_ڀريو_ضرب_ڪندڙ));

	إذا (i < (MHT_INIT_سائيز << MHT_LOG_ڀريو_ضرب_ڪندڙ))
	{
	    // first expansion shouldn't have occurred yet
	    أكد(انگ_buckets == MHT_INIT_سائيز);
	    أكد(ht.mht_buckets == ht.mht_small_buckets);
	}
	وإلا
	{
	    أكد(انگ_buckets > MHT_INIT_سائيز);
	    أكد(ht.mht_buckets != ht.mht_small_buckets);
	}

	ڪنجي = اشاريو_منزل_ڪنجي(i);
	أكد(mf_hash_ڳوليو(&ht, ڪنجي) == عدم);

	// allocate and add new item
	شي = LALLOC_CLEAR_ONE(mf_hashitem_T);
	أكد(شي != عدم);
	شي->mhi_ڪنجي = ڪنجي;
	mf_hash_جوڙيو_شي(&ht, شي);

	أكد(mf_hash_ڳوليو(&ht, ڪنجي) == شي);

	إذا (ht.mht_نقاب + 1 != انگ_buckets)
	{
	    // hash table was expanded
	    أكد(ht.mht_نقاب + 1 == انگ_buckets * MHT_GROWTH_ضرب_ڪندڙ);
	    أكد(i + 1 == (انگ_buckets << MHT_LOG_ڀريو_ضرب_ڪندڙ));
	}
    }

    // check presence of inserted items
    لكل (i = 0; i < TEST_ڳڻپ; i++)
    {
	ڪنجي = اشاريو_منزل_ڪنجي(i);
	شي = mf_hash_ڳوليو(&ht, ڪنجي);
	أكد(شي != عدم);
	أكد(شي->mhi_ڪنجي == ڪنجي);
    }

    // delete some items
    لكل (i = 0; i < TEST_ڳڻپ; i++)
    {
	إذا (i % 100 < 70)
	{
	    ڪنجي = اشاريو_منزل_ڪنجي(i);
	    شي = mf_hash_ڳوليو(&ht, ڪنجي);
	    أكد(شي != عدم);
	    أكد(شي->mhi_ڪنجي == ڪنجي);

	    mf_hash_rem_شي(&ht, شي);
	    أكد(mf_hash_ڳوليو(&ht, ڪنجي) == عدم);

	    mf_hash_جوڙيو_شي(&ht, شي);
	    أكد(mf_hash_ڳوليو(&ht, ڪنجي) == شي);

	    mf_hash_rem_شي(&ht, شي);
	    أكد(mf_hash_ڳوليو(&ht, ڪنجي) == عدم);

	    vim_free(شي);
	}
    }

    // check again
    لكل (i = 0; i < TEST_ڳڻپ; i++)
    {
	ڪنجي = اشاريو_منزل_ڪنجي(i);
	شي = mf_hash_ڳوليو(&ht, ڪنجي);

	إذا (i % 100 < 70)
	{
	    أكد(شي == عدم);
	}
	وإلا
	{
	    أكد(شي != عدم);
	    أكد(شي->mhi_ڪنجي == ڪنجي);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    صحيح
مکيه(فراغ)
{
    test_mf_hash();
    أرجع 0;
}
