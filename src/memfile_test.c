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
#അന്തര്ഭാവ <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#അന്തര്ഭാവ "main.c"

// This file has to be included because the tested functions are static
#അന്തര്ഭാവ "memfile.c"

#define index_to_താക്കോൽ(i) ((i) ^ 15167)
#define TEST_എണ്ണം 50000

/*
 * Test mf_hash_*() functions.
 */
    സ്ഥിതം ശൂന്യം
test_mf_hash(ശൂന്യം)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      താക്കോൽ;
    long_u	   i;
    long_u	   അക്കം_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    വേണ്ടി (i = 0; i < TEST_എണ്ണം; i++)
    {
	പ്രതിജ്ഞാ(ht.mht_എണ്ണം == i);

	// check that number of buckets is a power of 2
	അക്കം_buckets = ht.mht_mask + 1;
	പ്രതിജ്ഞാ(അക്കം_buckets > 0 && (അക്കം_buckets & (അക്കം_buckets - 1)) == 0);

	// check load factor
	പ്രതിജ്ഞാ(ht.mht_എണ്ണം <= (അക്കം_buckets << MHT_LOG_LOAD_FACTOR));

	എങ്കിൽ (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    പ്രതിജ്ഞാ(അക്കം_buckets == MHT_INIT_SIZE);
	    പ്രതിജ്ഞാ(ht.mht_buckets == ht.mht_small_buckets);
	}
	അല്ലെങ്കിൽ
	{
	    പ്രതിജ്ഞാ(അക്കം_buckets > MHT_INIT_SIZE);
	    പ്രതിജ്ഞാ(ht.mht_buckets != ht.mht_small_buckets);
	}

	താക്കോൽ = index_to_താക്കോൽ(i);
	പ്രതിജ്ഞാ(mf_hash_find(&ht, താക്കോൽ) == ശൂന്യ);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	പ്രതിജ്ഞാ(item != ശൂന്യ);
	item->mhi_താക്കോൽ = താക്കോൽ;
	mf_hash_add_item(&ht, item);

	പ്രതിജ്ഞാ(mf_hash_find(&ht, താക്കോൽ) == item);

	എങ്കിൽ (ht.mht_mask + 1 != അക്കം_buckets)
	{
	    // hash table was expanded
	    പ്രതിജ്ഞാ(ht.mht_mask + 1 == അക്കം_buckets * MHT_GROWTH_FACTOR);
	    പ്രതിജ്ഞാ(i + 1 == (അക്കം_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    വേണ്ടി (i = 0; i < TEST_എണ്ണം; i++)
    {
	താക്കോൽ = index_to_താക്കോൽ(i);
	item = mf_hash_find(&ht, താക്കോൽ);
	പ്രതിജ്ഞാ(item != ശൂന്യ);
	പ്രതിജ്ഞാ(item->mhi_താക്കോൽ == താക്കോൽ);
    }

    // delete some items
    വേണ്ടി (i = 0; i < TEST_എണ്ണം; i++)
    {
	എങ്കിൽ (i % 100 < 70)
	{
	    താക്കോൽ = index_to_താക്കോൽ(i);
	    item = mf_hash_find(&ht, താക്കോൽ);
	    പ്രതിജ്ഞാ(item != ശൂന്യ);
	    പ്രതിജ്ഞാ(item->mhi_താക്കോൽ == താക്കോൽ);

	    mf_hash_rem_item(&ht, item);
	    പ്രതിജ്ഞാ(mf_hash_find(&ht, താക്കോൽ) == ശൂന്യ);

	    mf_hash_add_item(&ht, item);
	    പ്രതിജ്ഞാ(mf_hash_find(&ht, താക്കോൽ) == item);

	    mf_hash_rem_item(&ht, item);
	    പ്രതിജ്ഞാ(mf_hash_find(&ht, താക്കോൽ) == ശൂന്യ);

	    vim_free(item);
	}
    }

    // check again
    വേണ്ടി (i = 0; i < TEST_എണ്ണം; i++)
    {
	താക്കോൽ = index_to_താക്കോൽ(i);
	item = mf_hash_find(&ht, താക്കോൽ);

	എങ്കിൽ (i % 100 < 70)
	{
	    പ്രതിജ്ഞാ(item == ശൂന്യ);
	}
	അല്ലെങ്കിൽ
	{
	    പ്രതിജ്ഞാ(item != ശൂന്യ);
	    പ്രതിജ്ഞാ(item->mhi_താക്കോൽ == താക്കോൽ);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    പൂർണ്ണസംഖ്യ
പ്രധാന(ശൂന്യം)
{
    test_mf_hash();
    തിരികെ 0;
}
