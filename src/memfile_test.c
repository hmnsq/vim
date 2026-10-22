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
#include <assert.h>

// Must include main.c because it contains much more than just main()
#define NO_VIM_MAIN
#include "main.c"

// This file has to be included because the tested functions are static
#include "memfile.c"

#define index_to_सुसि(i) ((i) ^ 15167)
#define TEST_बिलांग 50000

/*
 * Test mf_hash_*() functions.
 */
    static void
test_mf_hash(void)
{
    mf_hashtab_T   ht;
    mf_hashitem_T  *item;
    blocknr_T      सुसि;
    long_u	   i;
    long_u	   नुमेरो_buckets;

    mf_hash_init(&ht);

    // insert some items and check invariants
    for (i = 0; i < TEST_बिलांग; i++)
    {
	assert(ht.mht_बिलांग == i);

	// check that number of buckets is a power of 2
	नुमेरो_buckets = ht.mht_mask + 1;
	assert(नुमेरो_buckets > 0 && (नुमेरो_buckets & (नुमेरो_buckets - 1)) == 0);

	// check load factor
	assert(ht.mht_बिलांग <= (नुमेरो_buckets << MHT_LOG_LOAD_FACTOR));

	if (i < (MHT_INIT_SIZE << MHT_LOG_LOAD_FACTOR))
	{
	    // first expansion shouldn't have occurred yet
	    assert(नुमेरो_buckets == MHT_INIT_SIZE);
	    assert(ht.mht_buckets == ht.mht_small_buckets);
	}
	else
	{
	    assert(नुमेरो_buckets > MHT_INIT_SIZE);
	    assert(ht.mht_buckets != ht.mht_small_buckets);
	}

	सुसि = index_to_सुसि(i);
	assert(mf_hash_find(&ht, सुसि) == NULL);

	// allocate and add new item
	item = LALLOC_CLEAR_ONE(mf_hashitem_T);
	assert(item != NULL);
	item->mhi_सुसि = सुसि;
	mf_hash_add_item(&ht, item);

	assert(mf_hash_find(&ht, सुसि) == item);

	if (ht.mht_mask + 1 != नुमेरो_buckets)
	{
	    // hash table was expanded
	    assert(ht.mht_mask + 1 == नुमेरो_buckets * MHT_GROWTH_FACTOR);
	    assert(i + 1 == (नुमेरो_buckets << MHT_LOG_LOAD_FACTOR));
	}
    }

    // check presence of inserted items
    for (i = 0; i < TEST_बिलांग; i++)
    {
	सुसि = index_to_सुसि(i);
	item = mf_hash_find(&ht, सुसि);
	assert(item != NULL);
	assert(item->mhi_सुसि == सुसि);
    }

    // delete some items
    for (i = 0; i < TEST_बिलांग; i++)
    {
	if (i % 100 < 70)
	{
	    सुसि = index_to_सुसि(i);
	    item = mf_hash_find(&ht, सुसि);
	    assert(item != NULL);
	    assert(item->mhi_सुसि == सुसि);

	    mf_hash_rem_item(&ht, item);
	    assert(mf_hash_find(&ht, सुसि) == NULL);

	    mf_hash_add_item(&ht, item);
	    assert(mf_hash_find(&ht, सुसि) == item);

	    mf_hash_rem_item(&ht, item);
	    assert(mf_hash_find(&ht, सुसि) == NULL);

	    vim_free(item);
	}
    }

    // check again
    for (i = 0; i < TEST_बिलांग; i++)
    {
	सुसि = index_to_सुसि(i);
	item = mf_hash_find(&ht, सुसि);

	if (i % 100 < 70)
	{
	    assert(item == NULL);
	}
	else
	{
	    assert(item != NULL);
	    assert(item->mhi_सुसि == सुसि);
	}
    }

    // free hash table and all remaining items
    mf_hash_free_all(&ht);
}

    int
main(void)
{
    test_mf_hash();
    return 0;
}
