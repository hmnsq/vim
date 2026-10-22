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

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು NDEBUG
#ಅನ್ತರ್ಭಾವ <assert.h>

// Must include main.c because it contains much more than just main()
#ವ್ಯಾಖ್ಯಾನಿಸು NO_VIM_MAIN
#ಅನ್ತರ್ಭಾವ "main.c"

// This file has to be included because the tested functions are static
#ಅನ್ತರ್ಭಾವ "memfile.c"

#ವ್ಯಾಖ್ಯಾನಿಸು ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಕೀಲಿ(i) ((i) ^ 15167)
#ವ್ಯಾಖ್ಯಾನಿಸು TEST_ಎಣಿಕೆ 50000

/*
 * Test mf_hash_*() functions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
test_mf_hash(ಶೂನ್ಯ)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *ಅಂಶ;
    blocknr_T      ಕೀಲಿ;
    long_u	   i;
    long_u	   ಅಂಕೆ_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    ಗಾಗಿ (i = 0; i < TEST_ಎಣಿಕೆ; i++)
    {
	ಪ್ರತಿಜ್ಞಾ(ht.mht_ಎಣಿಕೆ == i);

	// check that number of buckets is a power of 2
	ಅಂಕೆ_buckets = ht.mht_ಮುಖವಾಡ + 1;
	ಪ್ರತಿಜ್ಞಾ(ಅಂಕೆ_buckets > 0 && (ಅಂಕೆ_buckets & (ಅಂಕೆ_buckets - 1)) == 0);

	// check load factor
	ಪ್ರತಿಜ್ಞಾ(ht.mht_ಎಣಿಕೆ <= (ಅಂಕೆ_buckets << MHT_LOG_ತುಂಬು_ಗುಣಕ));

	ಆದರೆ (i < (MHT_INIT_ಗಾತ್ರ << MHT_LOG_ತುಂಬು_ಗುಣಕ))
	{
	    // first expansion shouldn't have occurred yet
	    ಪ್ರತಿಜ್ಞಾ(ಅಂಕೆ_buckets == MHT_INIT_ಗಾತ್ರ);
	    ಪ್ರತಿಜ್ಞಾ(ht.mht_buckets == ht.mht_small_buckets);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪ್ರತಿಜ್ಞಾ(ಅಂಕೆ_buckets > MHT_INIT_ಗಾತ್ರ);
	    ಪ್ರತಿಜ್ಞಾ(ht.mht_buckets != ht.mht_small_buckets);
	}

	ಕೀಲಿ = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಕೀಲಿ(i);
	ಪ್ರತಿಜ್ಞಾ(mf_hash_ಕಂಡುಹಿಡಿ(&ht, ಕೀಲಿ) == ಖಾಲಿ);

	// allocate and add new item
	ಅಂಶ = LALLOC_CLEAR_ONE(mf_hashitem_T);
	ಪ್ರತಿಜ್ಞಾ(ಅಂಶ != ಖಾಲಿ);
	ಅಂಶ->mhi_ಕೀಲಿ = ಕೀಲಿ;
	mf_hash_ಕೂಡಿಸು_ಅಂಶ(&ht, ಅಂಶ);

	ಪ್ರತಿಜ್ಞಾ(mf_hash_ಕಂಡುಹಿಡಿ(&ht, ಕೀಲಿ) == ಅಂಶ);

	ಆದರೆ (ht.mht_ಮುಖವಾಡ + 1 != ಅಂಕೆ_buckets)
	{
	    // hash table was expanded
	    ಪ್ರತಿಜ್ಞಾ(ht.mht_ಮುಖವಾಡ + 1 == ಅಂಕೆ_buckets * MHT_GROWTH_ಗುಣಕ);
	    ಪ್ರತಿಜ್ಞಾ(i + 1 == (ಅಂಕೆ_buckets << MHT_LOG_ತುಂಬು_ಗುಣಕ));
	}
    }

    // check presence of inserted items
    ಗಾಗಿ (i = 0; i < TEST_ಎಣಿಕೆ; i++)
    {
	ಕೀಲಿ = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಕೀಲಿ(i);
	ಅಂಶ = mf_hash_ಕಂಡುಹಿಡಿ(&ht, ಕೀಲಿ);
	ಪ್ರತಿಜ್ಞಾ(ಅಂಶ != ಖಾಲಿ);
	ಪ್ರತಿಜ್ಞಾ(ಅಂಶ->mhi_ಕೀಲಿ == ಕೀಲಿ);
    }

    // delete some items
    ಗಾಗಿ (i = 0; i < TEST_ಎಣಿಕೆ; i++)
    {
	ಆದರೆ (i % 100 < 70)
	{
	    ಕೀಲಿ = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಕೀಲಿ(i);
	    ಅಂಶ = mf_hash_ಕಂಡುಹಿಡಿ(&ht, ಕೀಲಿ);
	    ಪ್ರತಿಜ್ಞಾ(ಅಂಶ != ಖಾಲಿ);
	    ಪ್ರತಿಜ್ಞಾ(ಅಂಶ->mhi_ಕೀಲಿ == ಕೀಲಿ);

	    mf_hash_rem_ಅಂಶ(&ht, ಅಂಶ);
	    ಪ್ರತಿಜ್ಞಾ(mf_hash_ಕಂಡುಹಿಡಿ(&ht, ಕೀಲಿ) == ಖಾಲಿ);

	    mf_hash_ಕೂಡಿಸು_ಅಂಶ(&ht, ಅಂಶ);
	    ಪ್ರತಿಜ್ಞಾ(mf_hash_ಕಂಡುಹಿಡಿ(&ht, ಕೀಲಿ) == ಅಂಶ);

	    mf_hash_rem_ಅಂಶ(&ht, ಅಂಶ);
	    ಪ್ರತಿಜ್ಞಾ(mf_hash_ಕಂಡುಹಿಡಿ(&ht, ಕೀಲಿ) == ಖಾಲಿ);

	    vim_free(ಅಂಶ);
	}
    }

    // check again
    ಗಾಗಿ (i = 0; i < TEST_ಎಣಿಕೆ; i++)
    {
	ಕೀಲಿ = ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಕೀಲಿ(i);
	ಅಂಶ = mf_hash_ಕಂಡುಹಿಡಿ(&ht, ಕೀಲಿ);

	ಆದರೆ (i % 100 < 70)
	{
	    ಪ್ರತಿಜ್ಞಾ(ಅಂಶ == ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪ್ರತಿಜ್ಞಾ(ಅಂಶ != ಖಾಲಿ);
	    ಪ್ರತಿಜ್ಞಾ(ಅಂಶ->mhi_ಕೀಲಿ == ಕೀಲಿ);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಶೂನ್ಯ)
{
    test_mf_hash();
    ಹಿಂತಿರುಗಿ 0;
}
