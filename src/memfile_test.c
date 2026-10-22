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

#تعریف_لغوه NDEBUG
#ضمن <assert.h>

// Must include main.c because it contains much more than just main()
#تعریف_کړه NO_VIM_MAIN
#ضمن "main.c"

// This file has to be included because the tested functions are static
#ضمن "memfile.c"

#تعریف_کړه شاخص_منزل_کلی(i) ((i) ^ 15167)
#تعریف_کړه TEST_شمېر 50000

/*
 * Test mf_hash_*() functions.
 */
    ساكن فراغ
test_mf_hash(فراغ)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *توکی;
    blocknr_T      کلی;
    long_u	   i;
    long_u	   عدد_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    لكل (i = 0; i < TEST_شمېر; i++)
    {
	أكد(ht.mht_شمېر == i);

	// check that number of buckets is a power of 2
	عدد_buckets = ht.mht_ماسک + 1;
	أكد(عدد_buckets > 0 && (عدد_buckets & (عدد_buckets - 1)) == 0);

	// check load factor
	أكد(ht.mht_شمېر <= (عدد_buckets << MHT_LOG_بارول_ضریب));

	إذا (i < (MHT_INIT_اندازه << MHT_LOG_بارول_ضریب))
	{
	    // first expansion shouldn't have occurred yet
	    أكد(عدد_buckets == MHT_INIT_اندازه);
	    أكد(ht.mht_buckets == ht.mht_small_buckets);
	}
	وإلا
	{
	    أكد(عدد_buckets > MHT_INIT_اندازه);
	    أكد(ht.mht_buckets != ht.mht_small_buckets);
	}

	کلی = شاخص_منزل_کلی(i);
	أكد(mf_hash_پیدا_کړه(&ht, کلی) == عدم);

	// allocate and add new item
	توکی = LALLOC_CLEAR_ONE(mf_hashitem_T);
	أكد(توکی != عدم);
	توکی->mhi_کلی = کلی;
	mf_hash_جمع_کړه_توکی(&ht, توکی);

	أكد(mf_hash_پیدا_کړه(&ht, کلی) == توکی);

	إذا (ht.mht_ماسک + 1 != عدد_buckets)
	{
	    // hash table was expanded
	    أكد(ht.mht_ماسک + 1 == عدد_buckets * MHT_GROWTH_ضریب);
	    أكد(i + 1 == (عدد_buckets << MHT_LOG_بارول_ضریب));
	}
    }

    // check presence of inserted items
    لكل (i = 0; i < TEST_شمېر; i++)
    {
	کلی = شاخص_منزل_کلی(i);
	توکی = mf_hash_پیدا_کړه(&ht, کلی);
	أكد(توکی != عدم);
	أكد(توکی->mhi_کلی == کلی);
    }

    // delete some items
    لكل (i = 0; i < TEST_شمېر; i++)
    {
	إذا (i % 100 < 70)
	{
	    کلی = شاخص_منزل_کلی(i);
	    توکی = mf_hash_پیدا_کړه(&ht, کلی);
	    أكد(توکی != عدم);
	    أكد(توکی->mhi_کلی == کلی);

	    mf_hash_rem_توکی(&ht, توکی);
	    أكد(mf_hash_پیدا_کړه(&ht, کلی) == عدم);

	    mf_hash_جمع_کړه_توکی(&ht, توکی);
	    أكد(mf_hash_پیدا_کړه(&ht, کلی) == توکی);

	    mf_hash_rem_توکی(&ht, توکی);
	    أكد(mf_hash_پیدا_کړه(&ht, کلی) == عدم);

	    vim_free(توکی);
	}
    }

    // check again
    لكل (i = 0; i < TEST_شمېر; i++)
    {
	کلی = شاخص_منزل_کلی(i);
	توکی = mf_hash_پیدا_کړه(&ht, کلی);

	إذا (i % 100 < 70)
	{
	    أكد(توکی == عدم);
	}
	وإلا
	{
	    أكد(توکی != عدم);
	    أكد(توکی->mhi_کلی == کلی);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    صحيح
اصلي(فراغ)
{
    test_mf_hash();
    أرجع 0;
}
