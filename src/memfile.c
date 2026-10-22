/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * memfile.c: Contains the functions for handling blocks of memory which can
 * be stored in a file. This is the implementation of a sort of virtual memory.
 *
 * A memfile consists of a sequence of blocks. The blocks numbered from 0
 * upwards have been assigned a place in the actual file. The block number
 * is equal to the page number in the file. The
 * blocks with negative numbers are currently in memory only. They can be
 * assigned a place in the file when too much memory is being used. At that
 * moment they get a new, positive, number. A list is used for translation of
 * negative to positive numbers.
 *
 * The size of a block is a multiple of a page size, normally the page size of
 * the device the file is on. Most blocks are 1 page long. A Block of multiple
 * pages is used for a line that does not fit in a single page.
 *
 * Each block can be in memory and/or in a file. The block stays in memory
 * as long as it is locked. If it is no longer locked it can be swapped out to
 * the file. It is only written to the file if it has been changed.
 *
 * Under normal operation the file is created when opening the memory file and
 * deleted when closing the memory file. Only with recovery an existing memory
 * file is opened.
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"

/*
 * Some systems have the page size in statfs.f_bsize, some in stat.st_blksize
 */
#ifdef HAVE_ST_BLKSIZE
# define STATFS stat
# define F_BSIZE st_blksize
# define fstatfs(fd, buf, len, nul) mch_fstat((fd), (buf))
#ଅନ୍ୟଥା
# ifdef HAVE_SYS_STATFS_H
#  include <sys/statfs.h>
#  define STATFS statfs
#  define F_BSIZE f_bsize
# endif
#endif

/*
 * for Amiga Dos 2.0x we use Flush
 */
#ifdef AMIGA
# ifdef FEAT_ARP
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dos2;			// this is in os_amiga.c
# endif
# ifdef SASC
#  include <ios1.h>			// for chkufb()
# endif
#endif

#define MEMFILE_PAGE_SIZE 4096		// default page size

ସ୍ଥିର long_u	total_mem_used = 0;	// total memory used for memfiles

ସ୍ଥିର ଶୂନ୍ୟ mf_ins_hash(memfile_T *, bhdr_T *);
ସ୍ଥିର ଶୂନ୍ୟ mf_rem_hash(memfile_T *, bhdr_T *);
ସ୍ଥିର bhdr_T *mf_find_hash(memfile_T *, blocknr_T);
ସ୍ଥିର ଶୂନ୍ୟ mf_ins_used(memfile_T *, bhdr_T *);
ସ୍ଥିର ଶୂନ୍ୟ mf_rem_used(memfile_T *, bhdr_T *);
ସ୍ଥିର bhdr_T *mf_release(memfile_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର bhdr_T *mf_alloc_bhdr(memfile_T *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ mf_free_bhdr(bhdr_T *);
ସ୍ଥିର ଶୂନ୍ୟ mf_ins_free(memfile_T *, bhdr_T *);
ସ୍ଥିର bhdr_T *mf_rem_free(memfile_T *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  mf_read(memfile_T *, bhdr_T *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  mf_write(memfile_T *, bhdr_T *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  mf_write_block(memfile_T *mfp, bhdr_T *hp, off_T offset, unsigned size);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  mf_trans_add(memfile_T *, bhdr_T *);
ସ୍ଥିର ଶୂନ୍ୟ mf_do_open(memfile_T *, char_u *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ଶୂନ୍ୟ mf_hash_init(mf_hashtab_T *);
ସ୍ଥିର ଶୂନ୍ୟ mf_hash_free(mf_hashtab_T *);
ସ୍ଥିର ଶୂନ୍ୟ mf_hash_free_all(mf_hashtab_T *);
ସ୍ଥିର mf_hashitem_T *mf_hash_find(mf_hashtab_T *, blocknr_T);
ସ୍ଥିର ଶୂନ୍ୟ mf_hash_add_item(mf_hashtab_T *, mf_hashitem_T *);
ସ୍ଥିର ଶୂନ୍ୟ mf_hash_rem_item(mf_hashtab_T *, mf_hashitem_T *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mf_hash_grow(mf_hashtab_T *);

/*
 * The functions for using a memfile:
 *
 * mf_open()	    open a new or existing memfile
 * mf_open_file()   open a swap file for an existing memfile
 * mf_close()	    close (and delete) a memfile
 * mf_new()	    create a new block in a memfile and lock it
 * mf_get()	    get an existing block and lock it
 * mf_put()	    unlock a block, may be marked for writing
 * mf_free()	    remove a block
 * mf_sync()	    sync changed parts of memfile to disk
 * mf_release_all() release as much memory as possible
 * mf_trans_del()   may translate negative to positive block number
 * mf_fullname()    make file name full path (use before first :cd)
 */

/*
 * Open an existing or new memory block file.
 *
 *  fname:	name of file to use (NULL means no file at all)
 *		Note: fname must have been allocated, it is not copied!
 *			If opening the file fails, fname is freed.
 *  flags:	flags for open() call
 *
 *  If fname != NULL and file cannot be opened, fail.
 *
 * return value: identifier for this memory block file.
 */
    memfile_T *
mf_open(char_u *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    memfile_T		*mfp;
    off_T		size;
#ଯଦି defined(STATFS) && defined(UNIX) && !defined(__QNX__) && !defined(__minix)
# define USE_FSTATFS
    ସଂରଚନା STATFS	stf;
#endif

    ଯଦି ((mfp = ALLOC_ONE(memfile_T)) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (fname == ଖାଲି)	    // no file for this memfile, use memory only
    {
	mfp->mf_fname = ଖାଲି;
	mfp->mf_ffname = ଖାଲି;
	mfp->mf_fd = -1;
    }
    ଅନ୍ୟଥା
    {
	mf_do_open(mfp, fname, flags);	// try to open the file

	// if the file cannot be opened, return here
	ଯଦି (mfp->mf_fd < 0)
	{
	    vim_free(mfp);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    mfp->mf_free_first = ଖାଲି;		// free list is empty
    mfp->mf_used_first = ଖାଲି;		// used list is empty
    mfp->mf_used_last = ଖାଲି;
    mfp->mf_dirty = MF_DIRTY_NO;
    mfp->mf_used_ଗଣନା = 0;
    mf_hash_init(&mfp->mf_hash);
    mf_hash_init(&mfp->mf_trans);
    mfp->mf_page_size = MEMFILE_PAGE_SIZE;
#ifdef FEAT_CRYPT
    mfp->mf_old_ଚାବି = ଖାଲି;
#endif

#ifdef USE_FSTATFS
    /*
     * Try to set the page size equal to the block size of the device.
     * Speeds up I/O a lot.
     * When recovering, the actual block size will be retrieved from block 0
     * in ml_recover().  The size used here may be wrong, therefore
     * mf_blocknr_max must be rounded up.
     */
    ଯଦି (mfp->mf_fd >= 0
	    && fstatfs(mfp->mf_fd, &stf, ଆକାର(ସଂରଚନା statfs), 0) == 0
	    && stf.F_BSIZE >= MIN_SWAP_PAGE_SIZE
	    && stf.F_BSIZE <= MAX_SWAP_PAGE_SIZE)
	mfp->mf_page_size = stf.F_BSIZE;
#endif

    ଯଦି (mfp->mf_fd < 0 || (flags & (O_TRUNC|O_EXCL))
		  || (size = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	mfp->mf_blocknr_max = 0;	// no file or empty file
    ଅନ୍ୟଥା
	mfp->mf_blocknr_max = (blocknr_T)((size + mfp->mf_page_size - 1)
							 / mfp->mf_page_size);
    mfp->mf_blocknr_min = -1;
    mfp->mf_neg_ଗଣନା = 0;
    mfp->mf_infile_ଗଣନା = mfp->mf_blocknr_max;

    /*
     * Compute maximum number of pages ('maxmem' is in Kbyte):
     *	'mammem' * 1Kbyte / page-size-in-bytes.
     * Avoid overflow by first reducing page size as much as possible.
     */
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    shift = 10;
	unsigned    page_size = mfp->mf_page_size;

	ଯେତେବେଳେ (shift > 0 && (page_size & 1) == 0)
	{
	    page_size = page_size >> 1;
	    --shift;
	}
	mfp->mf_used_ଗଣନା_max = (p_mm << shift) / page_size;
	ଯଦି (mfp->mf_used_ଗଣନା_max < 10)
	    mfp->mf_used_ଗଣନା_max = 10;
    }

    ଫେରନ୍ତୁ mfp;
}

/*
 * Open a file for an existing memfile.  Used when updatecount set from 0 to
 * some value.
 * If the file already exists, this fails.
 * "fname" is the name of file to use (NULL means no file at all)
 * Note: "fname" must have been allocated, it is not copied!  If opening the
 * file fails, "fname" is freed.
 *
 * return value: FAIL if file could not be opened, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mf_open_file(memfile_T *mfp, char_u *fname)
{
    mf_do_open(mfp, fname, O_RDWR|O_CREAT|O_EXCL); // try to open the file

    ଯଦି (mfp->mf_fd < 0)
	ଫେରନ୍ତୁ FAIL;

    mfp->mf_dirty = MF_DIRTY_YES;
    ଫେରନ୍ତୁ OK;
}

/*
 * Close a memory file and delete the associated file if 'del_file' is TRUE.
 */
    ଶୂନ୍ୟ
mf_close(memfile_T *mfp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା del_file)
{
    bhdr_T	*hp, *nextp;

    ଯଦି (mfp == ଖାଲି)		    // safety check
	ଫେରନ୍ତୁ;
    ଯଦି (mfp->mf_fd >= 0)
    {
	ଯଦି (close(mfp->mf_fd) < 0)
	    emsg(_(e_close_error_on_swap_file));
    }
    ଯଦି (del_file && mfp->mf_fname != ଖାଲି)
	mch_remove(mfp->mf_fname);
					    // free entries in used list
    ପାଇଁ (hp = mfp->mf_used_first; hp != ଖାଲି; hp = nextp)
    {
	total_mem_used -= (long_u)hp->bh_page_ଗଣନା * mfp->mf_page_size;
	nextp = hp->bh_next;
	mf_free_bhdr(hp);
    }
    ଯେତେବେଳେ (mfp->mf_free_first != ଖାଲି)	    // free entries in free list
	vim_free(mf_rem_free(mfp));
    mf_hash_free(&mfp->mf_hash);
    mf_hash_free_all(&mfp->mf_trans);	    // free hashtable and its items
    vim_free(mfp->mf_fname);
    vim_free(mfp->mf_ffname);
    vim_free(mfp);
}

/*
 * Close the swap file for a memfile.  Used when 'swapfile' is reset.
 */
    ଶୂନ୍ୟ
mf_close_file(
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		getlines)	// get all lines into memory?
{
    memfile_T	*mfp;
    linenr_T	lnum;

    mfp = buf->b_ml.ml_mfp;
    ଯଦି (mfp == ଖାଲି || mfp->mf_fd < 0)		// nothing to close
	ଫେରନ୍ତୁ;

    ଯଦି (getlines)
    {
	// get all blocks in memory by accessing all lines (clumsy!)
	mf_dont_release = TRUE;
	ପାଇଁ (lnum = 1; lnum <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା; ++lnum)
	    (ଶୂନ୍ୟ)ml_get_buf(buf, lnum, FALSE);
	mf_dont_release = FALSE;
	// TODO: should check if all blocks are really in core
    }

    ଯଦି (close(mfp->mf_fd) < 0)			// close the file
	emsg(_(e_close_error_on_swap_file));
    mfp->mf_fd = -1;

    ଯଦି (mfp->mf_fname != ଖାଲି)
    {
	mch_remove(mfp->mf_fname);		// delete the swap file
	VIM_CLEAR(mfp->mf_fname);
	VIM_CLEAR(mfp->mf_ffname);
    }
}

/*
 * Set new size for a memfile.  Used when block 0 of a swapfile has been read
 * and the size it indicates differs from what was guessed.
 */
    ଶୂନ୍ୟ
mf_new_page_size(memfile_T *mfp, unsigned new_size)
{
    // Correct the memory used for block 0 to the new size, because it will be
    // freed with that size later on.
    total_mem_used += new_size - mfp->mf_page_size;
    mfp->mf_page_size = new_size;
}

/*
 * get a new block
 *
 *   negative: TRUE if negative block number desired (data block)
 */
    bhdr_T *
mf_new(memfile_T *mfp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା negative, ପୂର୍ଣ୍ଣସଂଖ୍ୟା page_ଗଣନା)
{
    bhdr_T	*hp;	// new bhdr_T
    bhdr_T	*freep;	// first block in free list
    char_u	*p;

    /*
     * If we reached the maximum size for the used memory blocks, release one
     * If a bhdr_T is returned, use it and adjust the page_count if necessary.
     */
    hp = mf_release(mfp, page_ଗଣନା);

/*
 * Decide on the number to use:
 * If there is a free block, use its number.
 * Otherwise use mf_block_min for a negative number, mf_block_max for
 * a positive number.
 */
    freep = mfp->mf_free_first;
    ଯଦି (!negative && freep != ଖାଲି && freep->bh_page_ଗଣନା >= page_ଗଣନା)
    {
	/*
	 * If the block in the free list has more pages, take only the number
	 * of pages needed and allocate a new bhdr_T with data
	 *
	 * If the number of pages matches and mf_release() did not return a
	 * bhdr_T, use the bhdr_T from the free list and allocate the data
	 *
	 * If the number of pages matches and mf_release() returned a bhdr_T,
	 * just use the number and free the bhdr_T from the free list
	 */
	ଯଦି (freep->bh_page_ଗଣନା > page_ଗଣନା)
	{
	    ଯଦି (hp == ଖାଲି && (hp = mf_alloc_bhdr(mfp, page_ଗଣନା)) == ଖାଲି)
		ଫେରନ୍ତୁ ଖାଲି;
	    hp->bh_bnum = freep->bh_bnum;
	    freep->bh_bnum += page_ଗଣନା;
	    freep->bh_page_ଗଣନା -= page_ଗଣନା;
	}
	ଅନ୍ୟଥା ଯଦି (hp == ଖାଲି)	    // need to allocate memory for this block
	{
	    ଯଦି ((p = alloc((size_t)mfp->mf_page_size * page_ଗଣନା)) == ଖାଲି)
		ଫେରନ୍ତୁ ଖାଲି;
	    hp = mf_rem_free(mfp);
	    hp->bh_data = p;
	}
	ଅନ୍ୟଥା		    // use the number, remove entry from free list
	{
	    freep = mf_rem_free(mfp);
	    hp->bh_bnum = freep->bh_bnum;
	    vim_free(freep);
	}
    }
    ଅନ୍ୟଥା	// get a new number
    {
	ଯଦି (hp == ଖାଲି && (hp = mf_alloc_bhdr(mfp, page_ଗଣନା)) == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (negative)
	{
	    hp->bh_bnum = mfp->mf_blocknr_min--;
	    mfp->mf_neg_ଗଣନା++;
	}
	ଅନ୍ୟଥା
	{
	    hp->bh_bnum = mfp->mf_blocknr_max;
	    mfp->mf_blocknr_max += page_ଗଣନା;
	}
    }
    hp->bh_flags = BH_LOCKED | BH_DIRTY;	// new block is always dirty
    mfp->mf_dirty = MF_DIRTY_YES;
    hp->bh_page_ଗଣନା = page_ଗଣନା;
    mf_ins_used(mfp, hp);
    mf_ins_hash(mfp, hp);

    /*
     * Init the data to all zero, to avoid reading uninitialized data.
     * This also avoids that the passwd file ends up in the swap file!
     */
    (ଶୂନ୍ୟ)vim_memset((ଅକ୍ଷର *)(hp->bh_data), 0,
				      (size_t)mfp->mf_page_size * page_ଗଣନା);

    ଫେରନ୍ତୁ hp;
}

/*
 * Get existing block "nr" with "page_count" pages.
 *
 * Note: The caller should first check a negative nr with mf_trans_del()
 */
    bhdr_T *
mf_get(memfile_T *mfp, blocknr_T nr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା page_ଗଣନା)
{
    bhdr_T    *hp;
						// doesn't exist
    ଯଦି (nr >= mfp->mf_blocknr_max || nr <= mfp->mf_blocknr_min)
	ଫେରନ୍ତୁ ଖାଲି;

    /*
     * see if it is in the cache
     */
    hp = mf_find_hash(mfp, nr);
    ଯଦି (hp == ଖାଲି)	// not in the hash list
    {
	ଯଦି (nr < 0 || nr >= mfp->mf_infile_ଗଣନା)   // can't be in the file
	    ଫେରନ୍ତୁ ଖାଲି;

	// could check here if the block is in the free list

	/*
	 * Check if we need to flush an existing block.
	 * If so, use that block.
	 * If not, allocate a new block.
	 */
	hp = mf_release(mfp, page_ଗଣନା);
	ଯଦି (hp == ଖାଲି && page_ଗଣନା > 0)
	    hp = mf_alloc_bhdr(mfp, page_ଗଣନା);
	ଯଦି (hp == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	hp->bh_bnum = nr;
	hp->bh_flags = 0;
	hp->bh_page_ଗଣନା = page_ଗଣନା;
	ଯଦି (mf_read(mfp, hp) == FAIL)	    // cannot read the block!
	{
	    mf_free_bhdr(hp);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଅନ୍ୟଥା
    {
	mf_rem_used(mfp, hp);	// remove from list, insert in front below
	mf_rem_hash(mfp, hp);
    }

    hp->bh_flags |= BH_LOCKED;
    mf_ins_used(mfp, hp);	// put in front of used list
    mf_ins_hash(mfp, hp);	// put in front of hash list

    ଫେରନ୍ତୁ hp;
}

/*
 * release the block *hp
 *
 *   dirty: Block must be written to file later
 *   infile: Block should be in file (needed for recovery)
 *
 *  no return value, function cannot fail
 */
    ଶୂନ୍ୟ
mf_put(
    memfile_T	*mfp,
    bhdr_T	*hp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dirty,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		infile)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags;

    flags = hp->bh_flags;

    ଯଦି ((flags & BH_LOCKED) == 0)
	iemsg(e_block_was_not_locked);
    flags &= ~BH_LOCKED;
    ଯଦି (dirty)
    {
	flags |= BH_DIRTY;
	ଯଦି (mfp->mf_dirty != MF_DIRTY_YES_NOSYNC)
	    mfp->mf_dirty = MF_DIRTY_YES;
    }
    hp->bh_flags = flags;
    ଯଦି (infile)
	mf_trans_add(mfp, hp);	    // may translate negative in positive nr
}

/*
 * block *hp is no longer in used, may put it in the free list of memfile *mfp
 */
    ଶୂନ୍ୟ
mf_free(memfile_T *mfp, bhdr_T *hp)
{
    vim_free(hp->bh_data);	// free the memory
    mf_rem_hash(mfp, hp);	// get *hp out of the hash list
    mf_rem_used(mfp, hp);	// get *hp out of the used list
    ଯଦି (hp->bh_bnum < 0)
    {
	vim_free(hp);		// don't want negative numbers in free list
	mfp->mf_neg_ଗଣନା--;
    }
    ଅନ୍ୟଥା
	mf_ins_free(mfp, hp);	// put *hp in the free list
}

/*
 * Sync the memory file *mfp to disk.
 * Flags:
 *  MFS_ALL	If not given, blocks with negative numbers are not synced,
 *		even when they are dirty!
 *  MFS_STOP	Stop syncing when a character becomes available, but sync at
 *		least one block.
 *  MFS_FLUSH	Make sure buffers are flushed to disk, so they will survive a
 *		system crash.
 *  MFS_ZERO	Only write block 0.
 *
 * Return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mf_sync(memfile_T *mfp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;
    bhdr_T	*hp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		got_int_save = got_int;

    ଯଦି (mfp->mf_fd < 0)
    {
	// there is no file, nothing to do
	mfp->mf_dirty = MF_DIRTY_NO;
	ଫେରନ୍ତୁ FAIL;
    }

    // Only a CTRL-C while writing will break us here, not one typed
    // previously.
    got_int = FALSE;

    /*
     * sync from last to first (may reduce the probability of an inconsistent
     * file) If a write fails, it is very likely caused by a full filesystem.
     * Then we only try to write blocks within the existing file. If that also
     * fails then we give up.
     */
    status = OK;
    ପାଇଁ (hp = mfp->mf_used_last; hp != ଖାଲି; hp = hp->bh_prev)
	ଯଦି (((flags & MFS_ALL) || hp->bh_bnum >= 0)
		&& (hp->bh_flags & BH_DIRTY)
		&& (status == OK || (hp->bh_bnum >= 0
		    && hp->bh_bnum < mfp->mf_infile_ଗଣନା)))
	{
	    ଯଦି ((flags & MFS_ZERO) && hp->bh_bnum != 0)
		ଜାରି;
	    ଯଦି (mf_write(mfp, hp) == FAIL)
	    {
		ଯଦି (status == FAIL)	// double error: quit syncing
		    ଭାଙ୍ଗ;
		status = FAIL;
	    }
	    ଯଦି (flags & MFS_STOP)
	    {
		// Stop when char available now.
		ଯଦି (ui_char_avail())
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା
		ui_breakcheck();
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
	}

    /*
     * If the whole list is flushed, the memfile is not dirty anymore.
     * In case of an error this flag is also set, to avoid trying all the time.
     */
    ଯଦି (hp == ଖାଲି || status == FAIL)
	mfp->mf_dirty = MF_DIRTY_NO;

    ଯଦି ((flags & MFS_FLUSH) && *p_sws != NUL)
    {
#ଯଦି defined(UNIX)
# ifdef HAVE_FSYNC
	/*
	 * most Unixes have the very useful fsync() function, just what we need.
	 */
	ଯଦି (STRCMP(p_sws, "fsync") == 0)
	{
	    ଯଦି (vim_fsync(mfp->mf_fd))
		status = FAIL;
	}
	ଅନ୍ୟଥା
# endif
# ifdef HAVE_SYNC
	    sync();
# ଅନ୍ୟଥା
	    fflush(ଖାଲି);
# endif
#endif
#ifdef VMS
	ଯଦି (STRCMP(p_sws, "fsync") == 0)
	{
	    ଯଦି (vim_fsync(mfp->mf_fd))
		status = FAIL;
	}
#endif
#ifdef MSWIN
	ଯଦି (_commit(mfp->mf_fd))
	    status = FAIL;
#endif
#ifdef AMIGA
# ଯଦି defined(__AROS__) || defined(__amigaos4__)
	ଯଦି (vim_fsync(mfp->mf_fd) != 0)
	    status = FAIL;
# ଅନ୍ୟଥା
	/*
	 * Flush() only exists for AmigaDos 2.0.
	 * For 1.3 it should be done with close() + open(), but then the risk
	 * is that the open() may fail and lose the file....
	 */
#  ifdef FEAT_ARP
	ଯଦି (dos2)
#  endif
#  ifdef SASC
	{
	    ସଂରଚନା UFB *fp = chkufb(mfp->mf_fd);

	    ଯଦି (fp != ଖାଲି)
		Flush(fp->ufbfh);
	}
#  ଅନ୍ୟଥା
#   ଯଦି defined(_DCC) || defined(__GNUC__) || defined(__MORPHOS__)
	{
#    ଯଦି defined(__GNUC__) && !defined(__MORPHOS__) && defined(__libnix__)
	    // Have function (in libnix at least),
	    // but ain't got no prototype anywhere.
	    ବାହ୍ୟ unsigned long fdtofh(ପୂର୍ଣ୍ଣସଂଖ୍ୟା filedescriptor);
#    endif
#    ଯଦି !defined(__libnix__)
	    fflush(ଖାଲି);
#    ଅନ୍ୟଥା
	    BPTR fh = (BPTR)fdtofh(mfp->mf_fd);

	    ଯଦି (fh != 0)
		Flush(fh);
#    endif
	}
#   ଅନ୍ୟଥା // assume Manx
	    Flush(_devtab[mfp->mf_fd].fd);
#   endif
#  endif
# endif
#endif // AMIGA
    }

    got_int |= got_int_save;

    ଫେରନ୍ତୁ status;
}

/*
 * For all blocks in memory file *mfp that have a positive block number set
 * the dirty flag.  These are blocks that need to be written to a newly
 * created swapfile.
 */
    ଶୂନ୍ୟ
mf_set_dirty(memfile_T *mfp)
{
    bhdr_T	*hp;

    ପାଇଁ (hp = mfp->mf_used_last; hp != ଖାଲି; hp = hp->bh_prev)
	ଯଦି (hp->bh_bnum > 0)
	    hp->bh_flags |= BH_DIRTY;
    mfp->mf_dirty = MF_DIRTY_YES;
}

/*
 * insert block *hp in front of hashlist of memfile *mfp
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_ins_hash(memfile_T *mfp, bhdr_T *hp)
{
    mf_hash_add_item(&mfp->mf_hash, (mf_hashitem_T *)hp);
}

/*
 * remove block *hp from hashlist of memfile list *mfp
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_rem_hash(memfile_T *mfp, bhdr_T *hp)
{
    mf_hash_rem_item(&mfp->mf_hash, (mf_hashitem_T *)hp);
}

/*
 * look in hash lists of memfile *mfp for block header with number 'nr'
 */
    ସ୍ଥିର bhdr_T *
mf_find_hash(memfile_T *mfp, blocknr_T nr)
{
    ଫେରନ୍ତୁ (bhdr_T *)mf_hash_find(&mfp->mf_hash, nr);
}

/*
 * insert block *hp in front of used list of memfile *mfp
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_ins_used(memfile_T *mfp, bhdr_T *hp)
{
    hp->bh_next = mfp->mf_used_first;
    mfp->mf_used_first = hp;
    hp->bh_prev = ଖାଲି;
    ଯଦି (hp->bh_next == ଖାଲି)	    // list was empty, adjust last pointer
	mfp->mf_used_last = hp;
    ଅନ୍ୟଥା
	hp->bh_next->bh_prev = hp;
    mfp->mf_used_ଗଣନା += hp->bh_page_ଗଣନା;
    total_mem_used += (long_u)hp->bh_page_ଗଣନା * mfp->mf_page_size;
}

/*
 * remove block *hp from used list of memfile *mfp
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_rem_used(memfile_T *mfp, bhdr_T *hp)
{
    ଯଦି (hp->bh_next == ଖାଲି)	    // last block in used list
	mfp->mf_used_last = hp->bh_prev;
    ଅନ୍ୟଥା
	hp->bh_next->bh_prev = hp->bh_prev;
    ଯଦି (hp->bh_prev == ଖାଲି)	    // first block in used list
	mfp->mf_used_first = hp->bh_next;
    ଅନ୍ୟଥା
	hp->bh_prev->bh_next = hp->bh_next;
    mfp->mf_used_ଗଣନା -= hp->bh_page_ଗଣନା;
    total_mem_used -= (long_u)hp->bh_page_ଗଣନା * mfp->mf_page_size;
}

/*
 * Release the least recently used block from the used list if the number
 * of used memory blocks gets to big.
 *
 * Return the block header to the caller, including the memory block, so
 * it can be re-used. Make sure the page_count is right.
 *
 * Returns NULL if no block is released.
 */
    ସ୍ଥିର bhdr_T *
mf_release(memfile_T *mfp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା page_ଗଣନା)
{
    bhdr_T	*hp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_release;
    buf_T	*buf;

    // don't release while in mf_close_file()
    ଯଦି (mf_dont_release)
	ଫେରନ୍ତୁ ଖାଲି;

    /*
     * Need to release a block if the number of blocks for this memfile is
     * higher than the maximum or total memory used is over 'maxmemtot'
     */
    need_release = ((mfp->mf_used_ଗଣନା >= mfp->mf_used_ଗଣନା_max)
				  || (total_mem_used >> 10) >= (long_u)p_mmt);

    /*
     * Try to create a swap file if the amount of memory used is getting too
     * high.
     */
    ଯଦି (mfp->mf_fd < 0 && need_release && p_uc)
    {
	// find for which buffer this memfile is
	FOR_ALL_BUFFERS(buf)
	    ଯଦି (buf->b_ml.ml_mfp == mfp)
		ଭାଙ୍ଗ;
	ଯଦି (buf != ଖାଲି && buf->b_may_swap)
	    ml_open_file(buf);
    }

    /*
     * don't release a block if
     *	there is no file for this memfile
     * or
     *	the number of blocks for this memfile is lower than the maximum
     *	  and
     *	total memory used is not up to 'maxmemtot'
     */
    ଯଦି (mfp->mf_fd < 0 || !need_release)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (hp = mfp->mf_used_last; hp != ଖାଲି; hp = hp->bh_prev)
	ଯଦି (!(hp->bh_flags & BH_LOCKED))
	    ଭାଙ୍ଗ;
    ଯଦି (hp == ଖାଲି)	// not a single one that can be released
	ଫେରନ୍ତୁ ଖାଲି;

    /*
     * If the block is dirty, write it.
     * If the write fails we don't free it.
     */
    ଯଦି ((hp->bh_flags & BH_DIRTY) && mf_write(mfp, hp) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    mf_rem_used(mfp, hp);
    mf_rem_hash(mfp, hp);

    /*
     * If a bhdr_T is returned, make sure that the page_count of bh_data is
     * right
     */
    ଯଦି (hp->bh_page_ଗଣନା != page_ଗଣନା)
    {
	VIM_CLEAR(hp->bh_data);
	ଯଦି (page_ଗଣନା > 0)
	    hp->bh_data = alloc((size_t)mfp->mf_page_size * page_ଗଣନା);
	ଯଦି (hp->bh_data == ଖାଲି)
	{
	    vim_free(hp);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	hp->bh_page_ଗଣନା = page_ଗଣନା;
    }
    ଫେରନ୍ତୁ hp;
}

/*
 * release as many blocks as possible
 * Used in case of out of memory
 *
 * return TRUE if any memory was released
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mf_release_all(ଶୂନ୍ୟ)
{
    buf_T	*buf;
    memfile_T	*mfp;
    bhdr_T	*hp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;

    FOR_ALL_BUFFERS(buf)
    {
	mfp = buf->b_ml.ml_mfp;
	ଯଦି (mfp != ଖାଲି)
	{
	    // If no swap file yet, may open one
	    ଯଦି (mfp->mf_fd < 0 && buf->b_may_swap)
		ml_open_file(buf);

	    // only if there is a swapfile
	    ଯଦି (mfp->mf_fd >= 0)
	    {
		ପାଇଁ (hp = mfp->mf_used_last; hp != ଖାଲି; )
		{
		    ଯଦି (!(hp->bh_flags & BH_LOCKED)
			    && (!(hp->bh_flags & BH_DIRTY)
				|| mf_write(mfp, hp) != FAIL))
		    {
			mf_rem_used(mfp, hp);
			mf_rem_hash(mfp, hp);
			mf_free_bhdr(hp);
			hp = mfp->mf_used_last;	// re-start, list was changed
			retval = TRUE;
		    }
		    ଅନ୍ୟଥା
			hp = hp->bh_prev;
		}
	    }
	}
    }
    ଫେରନ୍ତୁ retval;
}

/*
 * Allocate a block header and a block of memory for it.
 */
    ସ୍ଥିର bhdr_T *
mf_alloc_bhdr(memfile_T *mfp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା page_ଗଣନା)
{
    bhdr_T	*hp;

    ଯଦି ((hp = ALLOC_ONE(bhdr_T)) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି ((hp->bh_data = alloc((size_t)mfp->mf_page_size * page_ଗଣନା)) == ଖାଲି)
    {
	vim_free(hp);	    // not enough memory
	ଫେରନ୍ତୁ ଖାଲି;
    }
    hp->bh_page_ଗଣନା = page_ଗଣନା;
    ଫେରନ୍ତୁ hp;
}

/*
 * Free a block header and the block of memory for it.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_free_bhdr(bhdr_T *hp)
{
    vim_free(hp->bh_data);
    vim_free(hp);
}

/*
 * Insert entry *hp in the free list.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_ins_free(memfile_T *mfp, bhdr_T *hp)
{
    hp->bh_next = mfp->mf_free_first;
    mfp->mf_free_first = hp;
}

/*
 * remove the first entry from the free list and return a pointer to it
 * Note: caller must check that mfp->mf_free_first is not NULL!
 */
    ସ୍ଥିର bhdr_T *
mf_rem_free(memfile_T *mfp)
{
    bhdr_T	*hp;

    hp = mfp->mf_free_first;
    mfp->mf_free_first = hp->bh_next;
    ଫେରନ୍ତୁ hp;
}

/*
 * read a block from disk
 *
 * Return FAIL for failure, OK otherwise
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mf_read(memfile_T *mfp, bhdr_T *hp)
{
    off_T	offset;
    unsigned	page_size;
    unsigned	size;

    ଯଦି (mfp->mf_fd < 0)	    // there is no file, can't read
	ଫେରନ୍ତୁ FAIL;

    page_size = mfp->mf_page_size;
    offset = (off_T)page_size * hp->bh_bnum;
    size = page_size * hp->bh_page_ଗଣନା;
    ଯଦି (vim_lseek(mfp->mf_fd, offset, SEEK_SET) != offset)
    {
	PERROR(_(e_seek_error_in_swap_file_read));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି ((unsigned)read_eintr(mfp->mf_fd, hp->bh_data, size) != size)
    {
	PERROR(_(e_read_error_in_swap_file));
	ଫେରନ୍ତୁ FAIL;
    }

#ifdef FEAT_CRYPT
    // Decrypt if 'key' is set and this is a data block. And when changing the
    // key.
    ଯଦି (*mfp->mf_buffer->b_p_ଚାବି != NUL || mfp->mf_old_ଚାବି != ଖାଲି)
	ml_decrypt_data(mfp, hp->bh_data, offset, size);
#endif

    ଫେରନ୍ତୁ OK;
}

/*
 * write a block to disk
 *
 * Return FAIL for failure, OK otherwise
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mf_write(memfile_T *mfp, bhdr_T *hp)
{
    off_T	offset;	    // offset in the file
    blocknr_T	nr;	    // block nr which is being written
    bhdr_T	*hp2;
    unsigned	page_size;  // number of bytes in a page
    unsigned	page_ଗଣନା; // number of pages written
    unsigned	size;	    // number of bytes written

    ଯଦି (mfp->mf_fd < 0 && !mfp->mf_reopen)
	// there is no file and there was no file, can't write
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (hp->bh_bnum < 0)	// must assign file block number
	ଯଦି (mf_trans_add(mfp, hp) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

    page_size = mfp->mf_page_size;

    /*
     * We don't want gaps in the file. Write the blocks in front of *hp
     * to extend the file.
     * If block 'mf_infile_count' is not in the hash list, it has been
     * freed. Fill the space in the file with data from the current block.
     */
    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା attempt;

	nr = hp->bh_bnum;
	ଯଦି (nr > mfp->mf_infile_ଗଣନା)		// beyond end of file
	{
	    nr = mfp->mf_infile_ଗଣନା;
	    hp2 = mf_find_hash(mfp, nr);	// NULL caught below
	}
	ଅନ୍ୟଥା
	    hp2 = hp;

	offset = (off_T)page_size * nr;
	ଯଦି (hp2 == ଖାଲି)	    // freed block, fill with dummy data
	    page_ଗଣନା = 1;
	ଅନ୍ୟଥା
	    page_ଗଣନା = hp2->bh_page_ଗଣନା;
	size = page_size * page_ଗଣନା;

	ପାଇଁ (attempt = 1; attempt <= 2; ++attempt)
	{
	    ଯଦି (mfp->mf_fd >= 0)
	    {
		ଯଦି (vim_lseek(mfp->mf_fd, offset, SEEK_SET) != offset)
		{
		    PERROR(_(e_seek_error_in_swap_file_write));
		    ଫେରନ୍ତୁ FAIL;
		}
		ଯଦି (mf_write_block(mfp,
				   hp2 == ଖାଲି ? hp : hp2, offset, size) == OK)
		    ଭାଙ୍ଗ;
	    }

	    ଯଦି (attempt == 1)
	    {
		// If the swap file is on a network drive, and the network
		// gets disconnected and then re-connected, we can maybe fix it
		// by closing and then re-opening the file.
		ଯଦି (mfp->mf_fd >= 0)
		    close(mfp->mf_fd);
		mfp->mf_fd = mch_open_rw((ଅକ୍ଷର *)mfp->mf_fname, mfp->mf_flags);
		mfp->mf_reopen = (mfp->mf_fd < 0);
	    }
	    ଯଦି (attempt == 2 || mfp->mf_fd < 0)
	    {
		// Avoid repeating the error message, this mostly happens when
		// the disk is full. We give the message again only after a
		// successful write or when hitting a key. We keep on trying,
		// in case some space becomes available.
		ଯଦି (!did_swapwrite_ସନ୍ଦେଶ)
		    emsg(_(e_write_error_in_swap_file));
		did_swapwrite_ସନ୍ଦେଶ = TRUE;
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	did_swapwrite_ସନ୍ଦେଶ = FALSE;
	ଯଦି (hp2 != ଖାଲି)		    // written a non-dummy block
	    hp2->bh_flags &= ~BH_DIRTY;
					    // appended to the file
	ଯଦି (nr + (blocknr_T)page_ଗଣନା > mfp->mf_infile_ଗଣନା)
	    mfp->mf_infile_ଗଣନା = nr + page_ଗଣନା;
	ଯଦି (nr == hp->bh_bnum)		    // written the desired block
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Write block "hp" with data size "size" to file "mfp->mf_fd".
 * Takes care of encryption.
 * Return FAIL or OK.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mf_write_block(
    memfile_T	*mfp,
    bhdr_T	*hp,
    off_T	offset UNUSED,
    unsigned	size)
{
    char_u	*data = hp->bh_data;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result = OK;

#ifdef FEAT_CRYPT
    // Encrypt if 'key' is set and this is a data block.
    ଯଦି (*mfp->mf_buffer->b_p_ଚାବି != NUL)
    {
	data = ml_encrypt_data(mfp, data, offset, size);
	ଯଦି (data == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }
#endif

    ଯଦି ((unsigned)write_eintr(mfp->mf_fd, data, size) != size)
	result = FAIL;

#ifdef FEAT_CRYPT
    ଯଦି (data != hp->bh_data)
	vim_free(data);
#endif

    ଫେରନ୍ତୁ result;
}

/*
 * Make block number for *hp positive and add it to the translation list
 *
 * Return FAIL for failure, OK otherwise
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mf_trans_add(memfile_T *mfp, bhdr_T *hp)
{
    bhdr_T	*freep;
    blocknr_T	new_bnum;
    NR_TRANS	*np;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		page_ଗଣନା;

    ଯଦି (hp->bh_bnum >= 0)		    // it's already positive
	ଫେରନ୍ତୁ OK;

    ଯଦି ((np = ALLOC_ONE(NR_TRANS)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

/*
 * Get a new number for the block.
 * If the first item in the free list has sufficient pages, use its number
 * Otherwise use mf_blocknr_max.
 */
    freep = mfp->mf_free_first;
    page_ଗଣନା = hp->bh_page_ଗଣନା;
    ଯଦି (freep != ଖାଲି && freep->bh_page_ଗଣନା >= page_ଗଣନା)
    {
	new_bnum = freep->bh_bnum;
	/*
	 * If the page count of the free block was larger, reduce it.
	 * If the page count matches, remove the block from the free list
	 */
	ଯଦି (freep->bh_page_ଗଣନା > page_ଗଣନା)
	{
	    freep->bh_bnum += page_ଗଣନା;
	    freep->bh_page_ଗଣନା -= page_ଗଣନା;
	}
	ଅନ୍ୟଥା
	{
	    freep = mf_rem_free(mfp);
	    vim_free(freep);
	}
    }
    ଅନ୍ୟଥା
    {
	new_bnum = mfp->mf_blocknr_max;
	mfp->mf_blocknr_max += page_ଗଣନା;
    }

    np->nt_old_bnum = hp->bh_bnum;	    // adjust number
    np->nt_new_bnum = new_bnum;

    mf_rem_hash(mfp, hp);		    // remove from old hash list
    hp->bh_bnum = new_bnum;
    mf_ins_hash(mfp, hp);		    // insert in new hash list

    // Insert "np" into "mf_trans" hashtable with key "np->nt_old_bnum"
    mf_hash_add_item(&mfp->mf_trans, (mf_hashitem_T *)np);

    ଫେରନ୍ତୁ OK;
}

/*
 * Lookup a translation from the trans lists and delete the entry.
 *
 * Return the positive new number when found, the old number when not found
 */
    blocknr_T
mf_trans_del(memfile_T *mfp, blocknr_T old_nr)
{
    NR_TRANS	*np;
    blocknr_T	new_bnum;

    np = (NR_TRANS *)mf_hash_find(&mfp->mf_trans, old_nr);

    ଯଦି (np == ଖାଲି)		// not found
	ଫେରନ୍ତୁ old_nr;

    mfp->mf_neg_ଗଣନା--;
    new_bnum = np->nt_new_bnum;

    // remove entry from the trans list
    mf_hash_rem_item(&mfp->mf_trans, (mf_hashitem_T *)np);

    vim_free(np);

    ଫେରନ୍ତୁ new_bnum;
}

/*
 * Set mfp->mf_ffname according to mfp->mf_fname and some other things.
 * Only called when creating or renaming the swapfile.	Either way it's a new
 * name so we must work out the full path name.
 */
    ଶୂନ୍ୟ
mf_set_ffname(memfile_T *mfp)
{
    mfp->mf_ffname = FullName_save(mfp->mf_fname, FALSE);
}

/*
 * Make the name of the file used for the memfile a full path.
 * Used before doing a :cd
 */
    ଶୂନ୍ୟ
mf_fullname(memfile_T *mfp)
{
    ଯଦି (mfp == ଖାଲି || mfp->mf_fname == ଖାଲି || mfp->mf_ffname == ଖାଲି)
	ଫେରନ୍ତୁ;

    vim_free(mfp->mf_fname);
    mfp->mf_fname = mfp->mf_ffname;
    mfp->mf_ffname = ଖାଲି;
}

/*
 * return TRUE if there are any translations pending for 'mfp'
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mf_need_trans(memfile_T *mfp)
{
    ଫେରନ୍ତୁ (mfp->mf_fname != ଖାଲି && mfp->mf_neg_ଗଣନା > 0);
}

/*
 * Open a swap file for a memfile.
 * The "fname" must be in allocated memory, and is consumed (also when an
 * error occurs).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_do_open(
    memfile_T	*mfp,
    char_u	*fname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		// flags for open()
{
#ifdef HAVE_LSTAT
    stat_T	sb;
#endif

    mfp->mf_fname = fname;

    /*
     * Get the full path name before the open, because this is
     * not possible after the open on the Amiga.
     * fname cannot be NameBuff, because it must have been allocated.
     */
    mf_set_ffname(mfp);
#ଯଦି defined(MSWIN)
    /*
     * A ":!cd e:xxx" may change the directory without us knowing, use the
     * full pathname always.  Careful: This frees fname!
     */
    mf_fullname(mfp);
#endif

#ifdef HAVE_LSTAT
    /*
     * Extra security check: When creating a swap file it really shouldn't
     * exist yet.  If there is a symbolic link, this is most likely an attack.
     */
    ଯଦି ((flags & O_CREAT) && mch_lstat((ଅକ୍ଷର *)mfp->mf_fname, &sb) >= 0)
    {
	mfp->mf_fd = -1;
	emsg(_(e_swap_file_already_exists_symlink_attack));
    }
    ଅନ୍ୟଥା
#endif
    {
	/*
	 * try to open the file
	 */
	flags |= O_EXTRA | O_NOFOLLOW;
#ifdef MSWIN
	// Prevent handle inheritance that cause problems with Cscope
	// (swap file may not be deleted if cscope connection was open after
	// the file)
	flags |= O_NOINHERIT;
#endif
	mfp->mf_flags = flags;
	mfp->mf_fd = mch_open_rw((ଅକ୍ଷର *)mfp->mf_fname, flags);
    }

    /*
     * If the file cannot be opened, use memory only
     */
    ଯଦି (mfp->mf_fd < 0)
    {
	VIM_CLEAR(mfp->mf_fname);
	VIM_CLEAR(mfp->mf_ffname);
    }
    ଅନ୍ୟଥା
    {
#ifdef HAVE_FD_CLOEXEC
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdflags = fcntl(mfp->mf_fd, F_GETFD);
	ଯଦି (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (ଶୂନ୍ୟ)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
#endif
#ଯଦି defined(HAVE_SELINUX) || defined(HAVE_SMACK)
	mch_copy_sec(fname, mfp->mf_fname);
#endif
	mch_hide(mfp->mf_fname);    // try setting the 'hidden' flag
    }
}

/*
 * Implementation of mf_hashtab_T follows.
 */

/*
 * The number of buckets in the hashtable is increased by a factor of
 * MHT_GROWTH_FACTOR when the average number of items per bucket
 * exceeds 2 ^ MHT_LOG_LOAD_FACTOR.
 */
#define MHT_LOG_LOAD_FACTOR 6
#define MHT_GROWTH_FACTOR   2   // must be a power of two

/*
 * Initialize an empty hash table.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_hash_init(mf_hashtab_T *mht)
{
    CLEAR_POINTER(mht);
    mht->mht_buckets = mht->mht_small_buckets;
    mht->mht_mask = MHT_INIT_SIZE - 1;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * The hash table must not be used again without another mf_hash_init() call.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_hash_free(mf_hashtab_T *mht)
{
    ଯଦି (mht->mht_buckets != mht->mht_small_buckets)
	vim_free(mht->mht_buckets);
}

/*
 * Free the array of a hash table and all the items it contains.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_hash_free_all(mf_hashtab_T *mht)
{
    long_u	    idx;
    mf_hashitem_T   *mhi;
    mf_hashitem_T   *next;

    ପାଇଁ (idx = 0; idx <= mht->mht_mask; idx++)
	ପାଇଁ (mhi = mht->mht_buckets[idx]; mhi != ଖାଲି; mhi = next)
	{
	    next = mhi->mhi_next;
	    vim_free(mhi);
	}

    mf_hash_free(mht);
}

/*
 * Find "key" in hashtable "mht".
 * Returns a pointer to a mf_hashitem_T or NULL if the item was not found.
 */
    ସ୍ଥିର mf_hashitem_T *
mf_hash_find(mf_hashtab_T *mht, blocknr_T ଚାବି)
{
    mf_hashitem_T   *mhi;

    mhi = mht->mht_buckets[ଚାବି & mht->mht_mask];
    ଯେତେବେଳେ (mhi != ଖାଲି && mhi->mhi_ଚାବି != ଚାବି)
	mhi = mhi->mhi_next;

    ଫେରନ୍ତୁ mhi;
}

/*
 * Add item "mhi" to hashtable "mht".
 * "mhi" must not be NULL.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_hash_add_item(mf_hashtab_T *mht, mf_hashitem_T *mhi)
{
    long_u	    idx;

    idx = mhi->mhi_ଚାବି & mht->mht_mask;
    mhi->mhi_next = mht->mht_buckets[idx];
    mhi->mhi_prev = ଖାଲି;
    ଯଦି (mhi->mhi_next != ଖାଲି)
	mhi->mhi_next->mhi_prev = mhi;
    mht->mht_buckets[idx] = mhi;

    mht->mht_ଗଣନା++;

    /*
     * Grow hashtable when we have more thank 2^MHT_LOG_LOAD_FACTOR
     * items per bucket on average
     */
    ଯଦି (mht->mht_fixed == 0
	&& (mht->mht_ଗଣନା >> MHT_LOG_LOAD_FACTOR) > mht->mht_mask)
    {
	ଯଦି (mf_hash_grow(mht) == FAIL)
	{
	    // stop trying to grow after first failure to allocate memory
	    mht->mht_fixed = 1;
	}
    }
}

/*
 * Remove item "mhi" from hashtable "mht".
 * "mhi" must not be NULL and must have been inserted into "mht".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
mf_hash_rem_item(mf_hashtab_T *mht, mf_hashitem_T *mhi)
{
    ଯଦି (mhi->mhi_prev == ଖାଲି)
	mht->mht_buckets[mhi->mhi_ଚାବି & mht->mht_mask] = mhi->mhi_next;
    ଅନ୍ୟଥା
	mhi->mhi_prev->mhi_next = mhi->mhi_next;

    ଯଦି (mhi->mhi_next != ଖାଲି)
	mhi->mhi_next->mhi_prev = mhi->mhi_prev;

    mht->mht_ଗଣନା--;

    // We could shrink the table here, but it typically takes little memory,
    // so why bother?
}

/*
 * Increase number of buckets in the hashtable by MHT_GROWTH_FACTOR and
 * rehash items.
 * Returns FAIL when out of memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mf_hash_grow(mf_hashtab_T *mht)
{
    long_u	    i, j;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    shift;
    mf_hashitem_T   *mhi;
    mf_hashitem_T   *tails[MHT_GROWTH_FACTOR];
    mf_hashitem_T   **buckets;
    size_t	    size;

    size = (mht->mht_mask + 1) * MHT_GROWTH_FACTOR * ଆକାର(ଶୂନ୍ୟ *);
    buckets = lalloc_clear(size, FALSE);
    ଯଦି (buckets == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    shift = 0;
    ଯେତେବେଳେ ((mht->mht_mask >> shift) != 0)
	shift++;

    ପାଇଁ (i = 0; i <= mht->mht_mask; i++)
    {
	/*
	 * Traverse the items in the i-th original bucket and move them into
	 * MHT_GROWTH_FACTOR new buckets, preserving their relative order
	 * within each new bucket.  Preserving the order is important because
	 * mf_get() tries to keep most recently used items at the front of
	 * each bucket.
	 *
	 * Here we strongly rely on the fact the hashes are computed modulo
	 * a power of two.
	 */

	CLEAR_FIELD(tails);

	ପାଇଁ (mhi = mht->mht_buckets[i]; mhi != ଖାଲି; mhi = mhi->mhi_next)
	{
	    j = (mhi->mhi_ଚାବି >> shift) & (MHT_GROWTH_FACTOR - 1);
	    ଯଦି (tails[j] == ଖାଲି)
	    {
		buckets[i + (j << shift)] = mhi;
		tails[j] = mhi;
		mhi->mhi_prev = ଖାଲି;
	    }
	    ଅନ୍ୟଥା
	    {
		tails[j]->mhi_next = mhi;
		mhi->mhi_prev = tails[j];
		tails[j] = mhi;
	    }
	}

	ପାଇଁ (j = 0; j < MHT_GROWTH_FACTOR; j++)
	    ଯଦି (tails[j] != ଖାଲି)
		tails[j]->mhi_next = ଖାଲି;
    }

    ଯଦି (mht->mht_buckets != mht->mht_small_buckets)
	vim_free(mht->mht_buckets);

    mht->mht_buckets = buckets;
    mht->mht_mask = (mht->mht_mask + 1) * MHT_GROWTH_FACTOR - 1;

    ଫେରନ୍ତୁ OK;
}
