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

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ NDEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

// Must include main.c because it contains much more than just main()
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NO_VIM_MAIN
#ਅਨ੍ਤਰ੍ਭਾਵ "main.c"

// This file has to be included because the tested functions are static
#ਅਨ੍ਤਰ੍ਭਾਵ "memfile.c"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ(i) ((i) ^ 15167)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEST_ਗਿਣਤੀ 50000

/*
 * Test mf_hash_*() functions.
 */
    ਸਥਿਰ ਖਾਲੀ
test_mf_hash(ਖਾਲੀ)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *ਵਸਤੂ;
    blocknr_T      ਕੁੰਜੀ;
    long_u	   i;
    long_u	   ਅੰਕ_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    ਲਈ (i = 0; i < TEST_ਗਿਣਤੀ; i++)
    {
	ਪ੍ਰਤਿਜ੍ਞਾ(ht.mht_ਗਿਣਤੀ == i);

	// check that number of buckets is a power of 2
	ਅੰਕ_buckets = ht.mht_ਨਕਾਬ + 1;
	ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕ_buckets > 0 && (ਅੰਕ_buckets & (ਅੰਕ_buckets - 1)) == 0);

	// check load factor
	ਪ੍ਰਤਿਜ੍ਞਾ(ht.mht_ਗਿਣਤੀ <= (ਅੰਕ_buckets << MHT_LOG_ਲੱਦੋ_ਗੁਣਕ));

	ਜੇ (i < (MHT_INIT_ਮਾਪ << MHT_LOG_ਲੱਦੋ_ਗੁਣਕ))
	{
	    // first expansion shouldn't have occurred yet
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕ_buckets == MHT_INIT_ਮਾਪ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(ht.mht_buckets == ht.mht_small_buckets);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕ_buckets > MHT_INIT_ਮਾਪ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(ht.mht_buckets != ht.mht_small_buckets);
	}

	ਕੁੰਜੀ = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ(i);
	ਪ੍ਰਤਿਜ੍ਞਾ(mf_hash_ਲੱਭੋ(&ht, ਕੁੰਜੀ) == ਨਿਰਾ);

	// allocate and add new item
	ਵਸਤੂ = LALLOC_CLEAR_ONE(mf_hashitem_T);
	ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ != ਨਿਰਾ);
	ਵਸਤੂ->mhi_ਕੁੰਜੀ = ਕੁੰਜੀ;
	mf_hash_ਜੋੜੋ_ਵਸਤੂ(&ht, ਵਸਤੂ);

	ਪ੍ਰਤਿਜ੍ਞਾ(mf_hash_ਲੱਭੋ(&ht, ਕੁੰਜੀ) == ਵਸਤੂ);

	ਜੇ (ht.mht_ਨਕਾਬ + 1 != ਅੰਕ_buckets)
	{
	    // hash table was expanded
	    ਪ੍ਰਤਿਜ੍ਞਾ(ht.mht_ਨਕਾਬ + 1 == ਅੰਕ_buckets * MHT_GROWTH_ਗੁਣਕ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(i + 1 == (ਅੰਕ_buckets << MHT_LOG_ਲੱਦੋ_ਗੁਣਕ));
	}
    }

    // check presence of inserted items
    ਲਈ (i = 0; i < TEST_ਗਿਣਤੀ; i++)
    {
	ਕੁੰਜੀ = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ(i);
	ਵਸਤੂ = mf_hash_ਲੱਭੋ(&ht, ਕੁੰਜੀ);
	ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ != ਨਿਰਾ);
	ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ->mhi_ਕੁੰਜੀ == ਕੁੰਜੀ);
    }

    // delete some items
    ਲਈ (i = 0; i < TEST_ਗਿਣਤੀ; i++)
    {
	ਜੇ (i % 100 < 70)
	{
	    ਕੁੰਜੀ = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ(i);
	    ਵਸਤੂ = mf_hash_ਲੱਭੋ(&ht, ਕੁੰਜੀ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ != ਨਿਰਾ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ->mhi_ਕੁੰਜੀ == ਕੁੰਜੀ);

	    mf_hash_rem_ਵਸਤੂ(&ht, ਵਸਤੂ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(mf_hash_ਲੱਭੋ(&ht, ਕੁੰਜੀ) == ਨਿਰਾ);

	    mf_hash_ਜੋੜੋ_ਵਸਤੂ(&ht, ਵਸਤੂ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(mf_hash_ਲੱਭੋ(&ht, ਕੁੰਜੀ) == ਵਸਤੂ);

	    mf_hash_rem_ਵਸਤੂ(&ht, ਵਸਤੂ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(mf_hash_ਲੱਭੋ(&ht, ਕੁੰਜੀ) == ਨਿਰਾ);

	    vim_free(ਵਸਤੂ);
	}
    }

    // check again
    ਲਈ (i = 0; i < TEST_ਗਿਣਤੀ; i++)
    {
	ਕੁੰਜੀ = ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਕੁੰਜੀ(i);
	ਵਸਤੂ = mf_hash_ਲੱਭੋ(&ht, ਕੁੰਜੀ);

	ਜੇ (i % 100 < 70)
	{
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ == ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ != ਨਿਰਾ);
	    ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ->mhi_ਕੁੰਜੀ == ਕੁੰਜੀ);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    ਪੂਰਨ
ਮੁੱਖ(ਖਾਲੀ)
{
    test_mf_hash();
    ਵਾਪਸ 0;
}
