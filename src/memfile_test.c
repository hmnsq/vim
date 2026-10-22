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

#تعریف_منسوخ NDEBUG
#شامل <assert.h>

// Must include main.c because it contains much more than just main()
#تعریف_کرو NO_VIM_MAIN
#شامل "main.c"

// This file has to be included because the tested functions are static
#شامل "memfile.c"

#تعریف_کرو اشاریہ_مقصد_کلید(i) ((i) ^ 15167)
#تعریف_کرو TEST_گنتی 50000

/*
 * Test mf_hash_*() functions.
 */
    ساکن خالی
test_mf_hash(خالی)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *شے;
    blocknr_T      کلید;
    long_u	   i;
    long_u	   عدد_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    برائے (i = 0; i < TEST_گنتی; i++)
    {
	أكد(ht.mht_گنتی == i);

	// check that number of buckets is a power of 2
	عدد_buckets = ht.mht_نقاب + 1;
	أكد(عدد_buckets > 0 && (عدد_buckets & (عدد_buckets - 1)) == 0);

	// check load factor
	أكد(ht.mht_گنتی <= (عدد_buckets << MHT_LOG_بھرو_ضربی_عدد));

	اگر (i < (MHT_INIT_سائز << MHT_LOG_بھرو_ضربی_عدد))
	{
	    // first expansion shouldn't have occurred yet
	    أكد(عدد_buckets == MHT_INIT_سائز);
	    أكد(ht.mht_buckets == ht.mht_small_buckets);
	}
	ورنہ
	{
	    أكد(عدد_buckets > MHT_INIT_سائز);
	    أكد(ht.mht_buckets != ht.mht_small_buckets);
	}

	کلید = اشاریہ_مقصد_کلید(i);
	أكد(mf_hash_ڈھونڈو(&ht, کلید) == تہی);

	// allocate and add new item
	شے = LALLOC_CLEAR_ONE(mf_hashitem_T);
	أكد(شے != تہی);
	شے->mhi_کلید = کلید;
	mf_hash_جوڑو_شے(&ht, شے);

	أكد(mf_hash_ڈھونڈو(&ht, کلید) == شے);

	اگر (ht.mht_نقاب + 1 != عدد_buckets)
	{
	    // hash table was expanded
	    أكد(ht.mht_نقاب + 1 == عدد_buckets * MHT_GROWTH_ضربی_عدد);
	    أكد(i + 1 == (عدد_buckets << MHT_LOG_بھرو_ضربی_عدد));
	}
    }

    // check presence of inserted items
    برائے (i = 0; i < TEST_گنتی; i++)
    {
	کلید = اشاریہ_مقصد_کلید(i);
	شے = mf_hash_ڈھونڈو(&ht, کلید);
	أكد(شے != تہی);
	أكد(شے->mhi_کلید == کلید);
    }

    // delete some items
    برائے (i = 0; i < TEST_گنتی; i++)
    {
	اگر (i % 100 < 70)
	{
	    کلید = اشاریہ_مقصد_کلید(i);
	    شے = mf_hash_ڈھونڈو(&ht, کلید);
	    أكد(شے != تہی);
	    أكد(شے->mhi_کلید == کلید);

	    mf_hash_rem_شے(&ht, شے);
	    أكد(mf_hash_ڈھونڈو(&ht, کلید) == تہی);

	    mf_hash_جوڑو_شے(&ht, شے);
	    أكد(mf_hash_ڈھونڈو(&ht, کلید) == شے);

	    mf_hash_rem_شے(&ht, شے);
	    أكد(mf_hash_ڈھونڈو(&ht, کلید) == تہی);

	    vim_free(شے);
	}
    }

    // check again
    برائے (i = 0; i < TEST_گنتی; i++)
    {
	کلید = اشاریہ_مقصد_کلید(i);
	شے = mf_hash_ڈھونڈو(&ht, کلید);

	اگر (i % 100 < 70)
	{
	    أكد(شے == تہی);
	}
	ورنہ
	{
	    أكد(شے != تہی);
	    أكد(شے->mhi_کلید == کلید);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    صحیح
مین(خالی)
{
    test_mf_hash();
    واپس 0;
}
