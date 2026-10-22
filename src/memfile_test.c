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

#لغو_تعریف NDEBUG
#ضمن <assert.h>

// Must include main.c because it contains much more than just main()
#تعریف_کن NO_VIM_MAIN
#ضمن "main.c"

// This file has to be included because the tested functions are static
#ضمن "memfile.c"

#تعریف_کن شاخص_مقصد_کلید(i) ((i) ^ 15167)
#تعریف_کن TEST_شمار 50000

/*
 * Test mf_hash_*() functions.
 */
    ساكن فراغ
test_mf_hash(فراغ)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *قلم;
    blocknr_T      کلید;
    long_u	   i;
    long_u	   رقم_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    لكل (i = 0; i < TEST_شمار; i++)
    {
	أكد(ht.mht_شمار == i);

	// check that number of buckets is a power of 2
	رقم_buckets = ht.mht_ماسک + 1;
	أكد(رقم_buckets > 0 && (رقم_buckets & (رقم_buckets - 1)) == 0);

	// check load factor
	أكد(ht.mht_شمار <= (رقم_buckets << MHT_LOG_بارگیری_ضریب));

	إذا (i < (MHT_INIT_اندازه << MHT_LOG_بارگیری_ضریب))
	{
	    // first expansion shouldn't have occurred yet
	    أكد(رقم_buckets == MHT_INIT_اندازه);
	    أكد(ht.mht_buckets == ht.mht_small_buckets);
	}
	وإلا
	{
	    أكد(رقم_buckets > MHT_INIT_اندازه);
	    أكد(ht.mht_buckets != ht.mht_small_buckets);
	}

	کلید = شاخص_مقصد_کلید(i);
	أكد(mf_hash_پیدا_کن(&ht, کلید) == عدم);

	// allocate and add new item
	قلم = LALLOC_CLEAR_ONE(mf_hashitem_T);
	أكد(قلم != عدم);
	قلم->mhi_کلید = کلید;
	mf_hash_جمع_کن_قلم(&ht, قلم);

	أكد(mf_hash_پیدا_کن(&ht, کلید) == قلم);

	إذا (ht.mht_ماسک + 1 != رقم_buckets)
	{
	    // hash table was expanded
	    أكد(ht.mht_ماسک + 1 == رقم_buckets * MHT_GROWTH_ضریب);
	    أكد(i + 1 == (رقم_buckets << MHT_LOG_بارگیری_ضریب));
	}
    }

    // check presence of inserted items
    لكل (i = 0; i < TEST_شمار; i++)
    {
	کلید = شاخص_مقصد_کلید(i);
	قلم = mf_hash_پیدا_کن(&ht, کلید);
	أكد(قلم != عدم);
	أكد(قلم->mhi_کلید == کلید);
    }

    // delete some items
    لكل (i = 0; i < TEST_شمار; i++)
    {
	إذا (i % 100 < 70)
	{
	    کلید = شاخص_مقصد_کلید(i);
	    قلم = mf_hash_پیدا_کن(&ht, کلید);
	    أكد(قلم != عدم);
	    أكد(قلم->mhi_کلید == کلید);

	    mf_hash_rem_قلم(&ht, قلم);
	    أكد(mf_hash_پیدا_کن(&ht, کلید) == عدم);

	    mf_hash_جمع_کن_قلم(&ht, قلم);
	    أكد(mf_hash_پیدا_کن(&ht, کلید) == قلم);

	    mf_hash_rem_قلم(&ht, قلم);
	    أكد(mf_hash_پیدا_کن(&ht, کلید) == عدم);

	    vim_free(قلم);
	}
    }

    // check again
    لكل (i = 0; i < TEST_شمار; i++)
    {
	کلید = شاخص_مقصد_کلید(i);
	قلم = mf_hash_پیدا_کن(&ht, کلید);

	إذا (i % 100 < 70)
	{
	    أكد(قلم == عدم);
	}
	وإلا
	{
	    أكد(قلم != عدم);
	    أكد(قلم->mhi_کلید == کلید);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    صحيح
اصلی(فراغ)
{
    test_mf_hash();
    أرجع 0;
}
