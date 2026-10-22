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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

/*
 * Some systems have the page size in statfs.f_bsize, some in stat.st_blksize
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ST_BLKSIZE
# define STATFS stat
# define F_BSIZE st_blksize
# define fstatfs(fd, buf, len, nul) mch_fstat((fd), (buf))
#ਨਹੀਂਤਾਂ
# ifdef HAVE_SYS_STATFS_H
#  include <sys/statfs.h>
#  define STATFS statfs
#  define F_BSIZE f_bsize
# endif
#ਜੇ_ਅੰਤ

/*
 * for Amiga Dos 2.0x we use Flush
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
# ifdef FEAT_ARP
ਬਾਹਰੀ ਪੂਰਨ dos2;			// this is in os_amiga.c
# endif
# ifdef SASC
#  include <ios1.h>			// for chkufb()
# endif
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MEMFILE_ਪੰਨਾ_ਮਾਪ 4096		// default page size

ਸਥਿਰ long_u	ਕੁੱਲ_mem_ਵਰਤਿਆ = 0;	// total memory used for memfiles

ਸਥਿਰ ਖਾਲੀ mf_ins_hash(memfile_T *, bhdr_T *);
ਸਥਿਰ ਖਾਲੀ mf_rem_hash(memfile_T *, bhdr_T *);
ਸਥਿਰ bhdr_T *mf_ਲੱਭੋ_hash(memfile_T *, blocknr_T);
ਸਥਿਰ ਖਾਲੀ mf_ins_ਵਰਤਿਆ(memfile_T *, bhdr_T *);
ਸਥਿਰ ਖਾਲੀ mf_rem_ਵਰਤਿਆ(memfile_T *, bhdr_T *);
ਸਥਿਰ bhdr_T *mf_release(memfile_T *, ਪੂਰਨ);
ਸਥਿਰ bhdr_T *mf_alloc_bhdr(memfile_T *, ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ mf_free_bhdr(bhdr_T *);
ਸਥਿਰ ਖਾਲੀ mf_ins_free(memfile_T *, bhdr_T *);
ਸਥਿਰ bhdr_T *mf_rem_free(memfile_T *);
ਸਥਿਰ ਪੂਰਨ  mf_read(memfile_T *, bhdr_T *);
ਸਥਿਰ ਪੂਰਨ  mf_write(memfile_T *, bhdr_T *);
ਸਥਿਰ ਪੂਰਨ  mf_write_ਖੰਡ(memfile_T *mfp, bhdr_T *hp, off_T ਵਿੱਥ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ);
ਸਥਿਰ ਪੂਰਨ  mf_trans_ਜੋੜੋ(memfile_T *, bhdr_T *);
ਸਥਿਰ ਖਾਲੀ mf_do_open(memfile_T *, char_u *, ਪੂਰਨ);
ਸਥਿਰ ਖਾਲੀ mf_hash_init(mf_hashtab_T *);
ਸਥਿਰ ਖਾਲੀ mf_hash_free(mf_hashtab_T *);
ਸਥਿਰ ਖਾਲੀ mf_hash_free_all(mf_hashtab_T *);
ਸਥਿਰ mf_hashitem_T *mf_hash_ਲੱਭੋ(mf_hashtab_T *, blocknr_T);
ਸਥਿਰ ਖਾਲੀ mf_hash_ਜੋੜੋ_ਵਸਤੂ(mf_hashtab_T *, mf_hashitem_T *);
ਸਥਿਰ ਖਾਲੀ mf_hash_rem_ਵਸਤੂ(mf_hashtab_T *, mf_hashitem_T *);
ਸਥਿਰ ਪੂਰਨ mf_hash_grow(mf_hashtab_T *);

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
mf_open(char_u *fname, ਪੂਰਨ flags)
{
    memfile_T		*mfp;
    off_T		ਮਾਪ;
#ਜੇ defined(STATFS) && defined(UNIX) && !defined(__QNX__) && !defined(__minix)
# define USE_FSTATFS
    ਬਣਤਰ STATFS	stf;
#ਜੇ_ਅੰਤ

    ਜੇ ((mfp = ALLOC_ONE(memfile_T)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (fname == ਨਿਰਾ)	    // no file for this memfile, use memory only
    {
	mfp->mf_fname = ਨਿਰਾ;
	mfp->mf_ffname = ਨਿਰਾ;
	mfp->mf_fd = -1;
    }
    ਨਹੀਂਤਾਂ
    {
	mf_do_open(mfp, fname, flags);	// try to open the file

	// if the file cannot be opened, return here
	ਜੇ (mfp->mf_fd < 0)
	{
	    vim_free(mfp);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

    mfp->mf_free_ਪਹਿਲਾ = ਨਿਰਾ;		// free list is empty
    mfp->mf_ਵਰਤਿਆ_ਪਹਿਲਾ = ਨਿਰਾ;		// used list is empty
    mfp->mf_ਵਰਤਿਆ_ਆਖਰੀ = ਨਿਰਾ;
    mfp->mf_dirty = MF_DIRTY_NO;
    mfp->mf_ਵਰਤਿਆ_ਗਿਣਤੀ = 0;
    mf_hash_init(&mfp->mf_hash);
    mf_hash_init(&mfp->mf_trans);
    mfp->mf_ਪੰਨਾ_ਮਾਪ = MEMFILE_ਪੰਨਾ_ਮਾਪ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    mfp->mf_old_ਕੁੰਜੀ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_FSTATFS
    /*
     * Try to set the page size equal to the block size of the device.
     * Speeds up I/O a lot.
     * When recovering, the actual block size will be retrieved from block 0
     * in ml_recover().  The size used here may be wrong, therefore
     * mf_blocknr_max must be rounded up.
     */
    ਜੇ (mfp->mf_fd >= 0
	    && fstatfs(mfp->mf_fd, &stf, ਆਕਾਰ(ਬਣਤਰ statfs), 0) == 0
	    && stf.F_BSIZE >= MIN_ਅਦਲਾ_ਬਦਲੀ_ਪੰਨਾ_ਮਾਪ
	    && stf.F_BSIZE <= MAX_ਅਦਲਾ_ਬਦਲੀ_ਪੰਨਾ_ਮਾਪ)
	mfp->mf_ਪੰਨਾ_ਮਾਪ = stf.F_BSIZE;
#ਜੇ_ਅੰਤ

    ਜੇ (mfp->mf_fd < 0 || (flags & (O_TRUNC|O_EXCL))
		  || (ਮਾਪ = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	mfp->mf_blocknr_max = 0;	// no file or empty file
    ਨਹੀਂਤਾਂ
	mfp->mf_blocknr_max = (blocknr_T)((ਮਾਪ + mfp->mf_ਪੰਨਾ_ਮਾਪ - 1)
							 / mfp->mf_ਪੰਨਾ_ਮਾਪ);
    mfp->mf_blocknr_min = -1;
    mfp->mf_neg_ਗਿਣਤੀ = 0;
    mfp->mf_infile_ਗਿਣਤੀ = mfp->mf_blocknr_max;

    /*
     * Compute maximum number of pages ('maxmem' is in Kbyte):
     *	'mammem' * 1Kbyte / page-size-in-bytes.
     * Avoid overflow by first reducing page size as much as possible.
     */
    {
	ਪੂਰਨ	    ਖਿਸਕਾਓ = 10;
	ਬਿਨਾਨਿਸ਼ਾਨੀ    ਪੰਨਾ_ਮਾਪ = mfp->mf_ਪੰਨਾ_ਮਾਪ;

	ਜਦੋਂ (ਖਿਸਕਾਓ > 0 && (ਪੰਨਾ_ਮਾਪ & 1) == 0)
	{
	    ਪੰਨਾ_ਮਾਪ = ਪੰਨਾ_ਮਾਪ >> 1;
	    --ਖਿਸਕਾਓ;
	}
	mfp->mf_ਵਰਤਿਆ_ਗਿਣਤੀ_max = (p_mm << ਖਿਸਕਾਓ) / ਪੰਨਾ_ਮਾਪ;
	ਜੇ (mfp->mf_ਵਰਤਿਆ_ਗਿਣਤੀ_max < 10)
	    mfp->mf_ਵਰਤਿਆ_ਗਿਣਤੀ_max = 10;
    }

    ਵਾਪਸ mfp;
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
    ਪੂਰਨ
mf_open_file(memfile_T *mfp, char_u *fname)
{
    mf_do_open(mfp, fname, O_RDWR|O_CREAT|O_EXCL); // try to open the file

    ਜੇ (mfp->mf_fd < 0)
	ਵਾਪਸ FAIL;

    mfp->mf_dirty = MF_DIRTY_YES;
    ਵਾਪਸ OK;
}

/*
 * Close a memory file and delete the associated file if 'del_file' is TRUE.
 */
    ਖਾਲੀ
mf_close(memfile_T *mfp, ਪੂਰਨ del_file)
{
    bhdr_T	*hp, *nextp;

    ਜੇ (mfp == ਨਿਰਾ)		    // safety check
	ਵਾਪਸ;
    ਜੇ (mfp->mf_fd >= 0)
    {
	ਜੇ (close(mfp->mf_fd) < 0)
	    emsg(_(e_close_error_on_ਅਦਲਾ_ਬਦਲੀ_file));
    }
    ਜੇ (del_file && mfp->mf_fname != ਨਿਰਾ)
	mch_remove(mfp->mf_fname);
					    // free entries in used list
    ਲਈ (hp = mfp->mf_ਵਰਤਿਆ_ਪਹਿਲਾ; hp != ਨਿਰਾ; hp = nextp)
    {
	ਕੁੱਲ_mem_ਵਰਤਿਆ -= (long_u)hp->bh_ਪੰਨਾ_ਗਿਣਤੀ * mfp->mf_ਪੰਨਾ_ਮਾਪ;
	nextp = hp->bh_ਅਗਲਾ;
	mf_free_bhdr(hp);
    }
    ਜਦੋਂ (mfp->mf_free_ਪਹਿਲਾ != ਨਿਰਾ)	    // free entries in free list
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
    ਖਾਲੀ
mf_close_file(
    buf_T	*buf,
    ਪੂਰਨ		getlines)	// get all lines into memory?
{
    memfile_T	*mfp;
    linenr_T	lnum;

    mfp = buf->b_ml.ml_mfp;
    ਜੇ (mfp == ਨਿਰਾ || mfp->mf_fd < 0)		// nothing to close
	ਵਾਪਸ;

    ਜੇ (getlines)
    {
	// get all blocks in memory by accessing all lines (clumsy!)
	mf_dont_release = TRUE;
	ਲਈ (lnum = 1; lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++lnum)
	    (ਖਾਲੀ)ml_get_buf(buf, lnum, FALSE);
	mf_dont_release = FALSE;
	// TODO: should check if all blocks are really in core
    }

    ਜੇ (close(mfp->mf_fd) < 0)			// close the file
	emsg(_(e_close_error_on_ਅਦਲਾ_ਬਦਲੀ_file));
    mfp->mf_fd = -1;

    ਜੇ (mfp->mf_fname != ਨਿਰਾ)
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
    ਖਾਲੀ
mf_new_ਪੰਨਾ_ਮਾਪ(memfile_T *mfp, ਬਿਨਾਨਿਸ਼ਾਨੀ new_ਮਾਪ)
{
    // Correct the memory used for block 0 to the new size, because it will be
    // freed with that size later on.
    ਕੁੱਲ_mem_ਵਰਤਿਆ += new_ਮਾਪ - mfp->mf_ਪੰਨਾ_ਮਾਪ;
    mfp->mf_ਪੰਨਾ_ਮਾਪ = new_ਮਾਪ;
}

/*
 * get a new block
 *
 *   negative: TRUE if negative block number desired (data block)
 */
    bhdr_T *
mf_new(memfile_T *mfp, ਪੂਰਨ negative, ਪੂਰਨ ਪੰਨਾ_ਗਿਣਤੀ)
{
    bhdr_T	*hp;	// new bhdr_T
    bhdr_T	*freep;	// first block in free list
    char_u	*p;

    /*
     * If we reached the maximum size for the used memory blocks, release one
     * If a bhdr_T is returned, use it and adjust the page_count if necessary.
     */
    hp = mf_release(mfp, ਪੰਨਾ_ਗਿਣਤੀ);

/*
 * Decide on the number to use:
 * If there is a free block, use its number.
 * Otherwise use mf_block_min for a negative number, mf_block_max for
 * a positive number.
 */
    freep = mfp->mf_free_ਪਹਿਲਾ;
    ਜੇ (!negative && freep != ਨਿਰਾ && freep->bh_ਪੰਨਾ_ਗਿਣਤੀ >= ਪੰਨਾ_ਗਿਣਤੀ)
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
	ਜੇ (freep->bh_ਪੰਨਾ_ਗਿਣਤੀ > ਪੰਨਾ_ਗਿਣਤੀ)
	{
	    ਜੇ (hp == ਨਿਰਾ && (hp = mf_alloc_bhdr(mfp, ਪੰਨਾ_ਗਿਣਤੀ)) == ਨਿਰਾ)
		ਵਾਪਸ ਨਿਰਾ;
	    hp->bh_bnum = freep->bh_bnum;
	    freep->bh_bnum += ਪੰਨਾ_ਗਿਣਤੀ;
	    freep->bh_ਪੰਨਾ_ਗਿਣਤੀ -= ਪੰਨਾ_ਗਿਣਤੀ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (hp == ਨਿਰਾ)	    // need to allocate memory for this block
	{
	    ਜੇ ((p = alloc((size_t)mfp->mf_ਪੰਨਾ_ਮਾਪ * ਪੰਨਾ_ਗਿਣਤੀ)) == ਨਿਰਾ)
		ਵਾਪਸ ਨਿਰਾ;
	    hp = mf_rem_free(mfp);
	    hp->bh_ਅੰਕੜੇ = p;
	}
	ਨਹੀਂਤਾਂ		    // use the number, remove entry from free list
	{
	    freep = mf_rem_free(mfp);
	    hp->bh_bnum = freep->bh_bnum;
	    vim_free(freep);
	}
    }
    ਨਹੀਂਤਾਂ	// get a new number
    {
	ਜੇ (hp == ਨਿਰਾ && (hp = mf_alloc_bhdr(mfp, ਪੰਨਾ_ਗਿਣਤੀ)) == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (negative)
	{
	    hp->bh_bnum = mfp->mf_blocknr_min--;
	    mfp->mf_neg_ਗਿਣਤੀ++;
	}
	ਨਹੀਂਤਾਂ
	{
	    hp->bh_bnum = mfp->mf_blocknr_max;
	    mfp->mf_blocknr_max += ਪੰਨਾ_ਗਿਣਤੀ;
	}
    }
    hp->bh_flags = BH_LOCKED | BH_DIRTY;	// new block is always dirty
    mfp->mf_dirty = MF_DIRTY_YES;
    hp->bh_ਪੰਨਾ_ਗਿਣਤੀ = ਪੰਨਾ_ਗਿਣਤੀ;
    mf_ins_ਵਰਤਿਆ(mfp, hp);
    mf_ins_hash(mfp, hp);

    /*
     * Init the data to all zero, to avoid reading uninitialized data.
     * This also avoids that the passwd file ends up in the swap file!
     */
    (ਖਾਲੀ)vim_memset((ਅੱਖਰ *)(hp->bh_ਅੰਕੜੇ), 0,
				      (size_t)mfp->mf_ਪੰਨਾ_ਮਾਪ * ਪੰਨਾ_ਗਿਣਤੀ);

    ਵਾਪਸ hp;
}

/*
 * Get existing block "nr" with "page_count" pages.
 *
 * Note: The caller should first check a negative nr with mf_trans_del()
 */
    bhdr_T *
mf_get(memfile_T *mfp, blocknr_T nr, ਪੂਰਨ ਪੰਨਾ_ਗਿਣਤੀ)
{
    bhdr_T    *hp;
						// doesn't exist
    ਜੇ (nr >= mfp->mf_blocknr_max || nr <= mfp->mf_blocknr_min)
	ਵਾਪਸ ਨਿਰਾ;

    /*
     * see if it is in the cache
     */
    hp = mf_ਲੱਭੋ_hash(mfp, nr);
    ਜੇ (hp == ਨਿਰਾ)	// not in the hash list
    {
	ਜੇ (nr < 0 || nr >= mfp->mf_infile_ਗਿਣਤੀ)   // can't be in the file
	    ਵਾਪਸ ਨਿਰਾ;

	// could check here if the block is in the free list

	/*
	 * Check if we need to flush an existing block.
	 * If so, use that block.
	 * If not, allocate a new block.
	 */
	hp = mf_release(mfp, ਪੰਨਾ_ਗਿਣਤੀ);
	ਜੇ (hp == ਨਿਰਾ && ਪੰਨਾ_ਗਿਣਤੀ > 0)
	    hp = mf_alloc_bhdr(mfp, ਪੰਨਾ_ਗਿਣਤੀ);
	ਜੇ (hp == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	hp->bh_bnum = nr;
	hp->bh_flags = 0;
	hp->bh_ਪੰਨਾ_ਗਿਣਤੀ = ਪੰਨਾ_ਗਿਣਤੀ;
	ਜੇ (mf_read(mfp, hp) == FAIL)	    // cannot read the block!
	{
	    mf_free_bhdr(hp);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	mf_rem_ਵਰਤਿਆ(mfp, hp);	// remove from list, insert in front below
	mf_rem_hash(mfp, hp);
    }

    hp->bh_flags |= BH_LOCKED;
    mf_ins_ਵਰਤਿਆ(mfp, hp);	// put in front of used list
    mf_ins_hash(mfp, hp);	// put in front of hash list

    ਵਾਪਸ hp;
}

/*
 * release the block *hp
 *
 *   dirty: Block must be written to file later
 *   infile: Block should be in file (needed for recovery)
 *
 *  no return value, function cannot fail
 */
    ਖਾਲੀ
mf_put(
    memfile_T	*mfp,
    bhdr_T	*hp,
    ਪੂਰਨ		dirty,
    ਪੂਰਨ		infile)
{
    ਪੂਰਨ		flags;

    flags = hp->bh_flags;

    ਜੇ ((flags & BH_LOCKED) == 0)
	iemsg(e_ਖੰਡ_was_not_locked);
    flags &= ~BH_LOCKED;
    ਜੇ (dirty)
    {
	flags |= BH_DIRTY;
	ਜੇ (mfp->mf_dirty != MF_DIRTY_YES_NOSYNC)
	    mfp->mf_dirty = MF_DIRTY_YES;
    }
    hp->bh_flags = flags;
    ਜੇ (infile)
	mf_trans_ਜੋੜੋ(mfp, hp);	    // may translate negative in positive nr
}

/*
 * block *hp is no longer in used, may put it in the free list of memfile *mfp
 */
    ਖਾਲੀ
mf_free(memfile_T *mfp, bhdr_T *hp)
{
    vim_free(hp->bh_ਅੰਕੜੇ);	// free the memory
    mf_rem_hash(mfp, hp);	// get *hp out of the hash list
    mf_rem_ਵਰਤਿਆ(mfp, hp);	// get *hp out of the used list
    ਜੇ (hp->bh_bnum < 0)
    {
	vim_free(hp);		// don't want negative numbers in free list
	mfp->mf_neg_ਗਿਣਤੀ--;
    }
    ਨਹੀਂਤਾਂ
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
    ਪੂਰਨ
mf_sync(memfile_T *mfp, ਪੂਰਨ flags)
{
    ਪੂਰਨ		ਸਥਿਤੀ;
    bhdr_T	*hp;
    ਪੂਰਨ		got_int_save = got_int;

    ਜੇ (mfp->mf_fd < 0)
    {
	// there is no file, nothing to do
	mfp->mf_dirty = MF_DIRTY_NO;
	ਵਾਪਸ FAIL;
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
    ਸਥਿਤੀ = OK;
    ਲਈ (hp = mfp->mf_ਵਰਤਿਆ_ਆਖਰੀ; hp != ਨਿਰਾ; hp = hp->bh_prev)
	ਜੇ (((flags & MFS_ALL) || hp->bh_bnum >= 0)
		&& (hp->bh_flags & BH_DIRTY)
		&& (ਸਥਿਤੀ == OK || (hp->bh_bnum >= 0
		    && hp->bh_bnum < mfp->mf_infile_ਗਿਣਤੀ)))
	{
	    ਜੇ ((flags & MFS_ZERO) && hp->bh_bnum != 0)
		ਜਾਰੀ;
	    ਜੇ (mf_write(mfp, hp) == FAIL)
	    {
		ਜੇ (ਸਥਿਤੀ == FAIL)	// double error: quit syncing
		    ਰੋਕੋ;
		ਸਥਿਤੀ = FAIL;
	    }
	    ਜੇ (flags & MFS_STOP)
	    {
		// Stop when char available now.
		ਜੇ (ui_char_avail())
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ
		ui_breakcheck();
	    ਜੇ (got_int)
		ਰੋਕੋ;
	}

    /*
     * If the whole list is flushed, the memfile is not dirty anymore.
     * In case of an error this flag is also set, to avoid trying all the time.
     */
    ਜੇ (hp == ਨਿਰਾ || ਸਥਿਤੀ == FAIL)
	mfp->mf_dirty = MF_DIRTY_NO;

    ਜੇ ((flags & MFS_FLUSH) && *p_sws != NUL)
    {
#ਜੇ defined(UNIX)
# ifdef HAVE_FSYNC
	/*
	 * most Unixes have the very useful fsync() function, just what we need.
	 */
	ਜੇ (STRCMP(p_sws, "fsync") == 0)
	{
	    ਜੇ (vim_fsync(mfp->mf_fd))
		ਸਥਿਤੀ = FAIL;
	}
	ਨਹੀਂਤਾਂ
# endif
# ifdef HAVE_SYNC
	    sync();
# ਨਹੀਂਤਾਂ
	    fflush(ਨਿਰਾ);
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
	ਜੇ (STRCMP(p_sws, "fsync") == 0)
	{
	    ਜੇ (vim_fsync(mfp->mf_fd))
		ਸਥਿਤੀ = FAIL;
	}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	ਜੇ (_commit(mfp->mf_fd))
	    ਸਥਿਤੀ = FAIL;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
# ਜੇ defined(__AROS__) || defined(__amigaos4__)
	ਜੇ (vim_fsync(mfp->mf_fd) != 0)
	    ਸਥਿਤੀ = FAIL;
# ਨਹੀਂਤਾਂ
	/*
	 * Flush() only exists for AmigaDos 2.0.
	 * For 1.3 it should be done with close() + open(), but then the risk
	 * is that the open() may fail and lose the file....
	 */
#  ifdef FEAT_ARP
	ਜੇ (dos2)
#  endif
#  ifdef SASC
	{
	    ਬਣਤਰ UFB *fp = chkufb(mfp->mf_fd);

	    ਜੇ (fp != ਨਿਰਾ)
		Flush(fp->ufbfh);
	}
#  ਨਹੀਂਤਾਂ
#   ਜੇ defined(_DCC) || defined(__GNUC__) || defined(__MORPHOS__)
	{
#    ਜੇ defined(__GNUC__) && !defined(__MORPHOS__) && defined(__libnix__)
	    // Have function (in libnix at least),
	    // but ain't got no prototype anywhere.
	    ਬਾਹਰੀ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ fdtofh(ਪੂਰਨ filedescriptor);
#    endif
#    ਜੇ !defined(__libnix__)
	    fflush(ਨਿਰਾ);
#    ਨਹੀਂਤਾਂ
	    BPTR fh = (BPTR)fdtofh(mfp->mf_fd);

	    ਜੇ (fh != 0)
		Flush(fh);
#    endif
	}
#   ਨਹੀਂਤਾਂ // assume Manx
	    Flush(_devtab[mfp->mf_fd].fd);
#   endif
#  endif
# endif
#ਜੇ_ਅੰਤ // AMIGA
    }

    got_int |= got_int_save;

    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * For all blocks in memory file *mfp that have a positive block number set
 * the dirty flag.  These are blocks that need to be written to a newly
 * created swapfile.
 */
    ਖਾਲੀ
mf_ਸੈੱਟ_dirty(memfile_T *mfp)
{
    bhdr_T	*hp;

    ਲਈ (hp = mfp->mf_ਵਰਤਿਆ_ਆਖਰੀ; hp != ਨਿਰਾ; hp = hp->bh_prev)
	ਜੇ (hp->bh_bnum > 0)
	    hp->bh_flags |= BH_DIRTY;
    mfp->mf_dirty = MF_DIRTY_YES;
}

/*
 * insert block *hp in front of hashlist of memfile *mfp
 */
    ਸਥਿਰ ਖਾਲੀ
mf_ins_hash(memfile_T *mfp, bhdr_T *hp)
{
    mf_hash_ਜੋੜੋ_ਵਸਤੂ(&mfp->mf_hash, (mf_hashitem_T *)hp);
}

/*
 * remove block *hp from hashlist of memfile list *mfp
 */
    ਸਥਿਰ ਖਾਲੀ
mf_rem_hash(memfile_T *mfp, bhdr_T *hp)
{
    mf_hash_rem_ਵਸਤੂ(&mfp->mf_hash, (mf_hashitem_T *)hp);
}

/*
 * look in hash lists of memfile *mfp for block header with number 'nr'
 */
    ਸਥਿਰ bhdr_T *
mf_ਲੱਭੋ_hash(memfile_T *mfp, blocknr_T nr)
{
    ਵਾਪਸ (bhdr_T *)mf_hash_ਲੱਭੋ(&mfp->mf_hash, nr);
}

/*
 * insert block *hp in front of used list of memfile *mfp
 */
    ਸਥਿਰ ਖਾਲੀ
mf_ins_ਵਰਤਿਆ(memfile_T *mfp, bhdr_T *hp)
{
    hp->bh_ਅਗਲਾ = mfp->mf_ਵਰਤਿਆ_ਪਹਿਲਾ;
    mfp->mf_ਵਰਤਿਆ_ਪਹਿਲਾ = hp;
    hp->bh_prev = ਨਿਰਾ;
    ਜੇ (hp->bh_ਅਗਲਾ == ਨਿਰਾ)	    // list was empty, adjust last pointer
	mfp->mf_ਵਰਤਿਆ_ਆਖਰੀ = hp;
    ਨਹੀਂਤਾਂ
	hp->bh_ਅਗਲਾ->bh_prev = hp;
    mfp->mf_ਵਰਤਿਆ_ਗਿਣਤੀ += hp->bh_ਪੰਨਾ_ਗਿਣਤੀ;
    ਕੁੱਲ_mem_ਵਰਤਿਆ += (long_u)hp->bh_ਪੰਨਾ_ਗਿਣਤੀ * mfp->mf_ਪੰਨਾ_ਮਾਪ;
}

/*
 * remove block *hp from used list of memfile *mfp
 */
    ਸਥਿਰ ਖਾਲੀ
mf_rem_ਵਰਤਿਆ(memfile_T *mfp, bhdr_T *hp)
{
    ਜੇ (hp->bh_ਅਗਲਾ == ਨਿਰਾ)	    // last block in used list
	mfp->mf_ਵਰਤਿਆ_ਆਖਰੀ = hp->bh_prev;
    ਨਹੀਂਤਾਂ
	hp->bh_ਅਗਲਾ->bh_prev = hp->bh_prev;
    ਜੇ (hp->bh_prev == ਨਿਰਾ)	    // first block in used list
	mfp->mf_ਵਰਤਿਆ_ਪਹਿਲਾ = hp->bh_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	hp->bh_prev->bh_ਅਗਲਾ = hp->bh_ਅਗਲਾ;
    mfp->mf_ਵਰਤਿਆ_ਗਿਣਤੀ -= hp->bh_ਪੰਨਾ_ਗਿਣਤੀ;
    ਕੁੱਲ_mem_ਵਰਤਿਆ -= (long_u)hp->bh_ਪੰਨਾ_ਗਿਣਤੀ * mfp->mf_ਪੰਨਾ_ਮਾਪ;
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
    ਸਥਿਰ bhdr_T *
mf_release(memfile_T *mfp, ਪੂਰਨ ਪੰਨਾ_ਗਿਣਤੀ)
{
    bhdr_T	*hp;
    ਪੂਰਨ		need_release;
    buf_T	*buf;

    // don't release while in mf_close_file()
    ਜੇ (mf_dont_release)
	ਵਾਪਸ ਨਿਰਾ;

    /*
     * Need to release a block if the number of blocks for this memfile is
     * higher than the maximum or total memory used is over 'maxmemtot'
     */
    need_release = ((mfp->mf_ਵਰਤਿਆ_ਗਿਣਤੀ >= mfp->mf_ਵਰਤਿਆ_ਗਿਣਤੀ_max)
				  || (ਕੁੱਲ_mem_ਵਰਤਿਆ >> 10) >= (long_u)p_mmt);

    /*
     * Try to create a swap file if the amount of memory used is getting too
     * high.
     */
    ਜੇ (mfp->mf_fd < 0 && need_release && p_uc)
    {
	// find for which buffer this memfile is
	FOR_ALL_BUFFERS(buf)
	    ਜੇ (buf->b_ml.ml_mfp == mfp)
		ਰੋਕੋ;
	ਜੇ (buf != ਨਿਰਾ && buf->b_may_ਅਦਲਾ_ਬਦਲੀ)
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
    ਜੇ (mfp->mf_fd < 0 || !need_release)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (hp = mfp->mf_ਵਰਤਿਆ_ਆਖਰੀ; hp != ਨਿਰਾ; hp = hp->bh_prev)
	ਜੇ (!(hp->bh_flags & BH_LOCKED))
	    ਰੋਕੋ;
    ਜੇ (hp == ਨਿਰਾ)	// not a single one that can be released
	ਵਾਪਸ ਨਿਰਾ;

    /*
     * If the block is dirty, write it.
     * If the write fails we don't free it.
     */
    ਜੇ ((hp->bh_flags & BH_DIRTY) && mf_write(mfp, hp) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    mf_rem_ਵਰਤਿਆ(mfp, hp);
    mf_rem_hash(mfp, hp);

    /*
     * If a bhdr_T is returned, make sure that the page_count of bh_data is
     * right
     */
    ਜੇ (hp->bh_ਪੰਨਾ_ਗਿਣਤੀ != ਪੰਨਾ_ਗਿਣਤੀ)
    {
	VIM_CLEAR(hp->bh_ਅੰਕੜੇ);
	ਜੇ (ਪੰਨਾ_ਗਿਣਤੀ > 0)
	    hp->bh_ਅੰਕੜੇ = alloc((size_t)mfp->mf_ਪੰਨਾ_ਮਾਪ * ਪੰਨਾ_ਗਿਣਤੀ);
	ਜੇ (hp->bh_ਅੰਕੜੇ == ਨਿਰਾ)
	{
	    vim_free(hp);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	hp->bh_ਪੰਨਾ_ਗਿਣਤੀ = ਪੰਨਾ_ਗਿਣਤੀ;
    }
    ਵਾਪਸ hp;
}

/*
 * release as many blocks as possible
 * Used in case of out of memory
 *
 * return TRUE if any memory was released
 */
    ਪੂਰਨ
mf_release_all(ਖਾਲੀ)
{
    buf_T	*buf;
    memfile_T	*mfp;
    bhdr_T	*hp;
    ਪੂਰਨ		retval = FALSE;

    FOR_ALL_BUFFERS(buf)
    {
	mfp = buf->b_ml.ml_mfp;
	ਜੇ (mfp != ਨਿਰਾ)
	{
	    // If no swap file yet, may open one
	    ਜੇ (mfp->mf_fd < 0 && buf->b_may_ਅਦਲਾ_ਬਦਲੀ)
		ml_open_file(buf);

	    // only if there is a swapfile
	    ਜੇ (mfp->mf_fd >= 0)
	    {
		ਲਈ (hp = mfp->mf_ਵਰਤਿਆ_ਆਖਰੀ; hp != ਨਿਰਾ; )
		{
		    ਜੇ (!(hp->bh_flags & BH_LOCKED)
			    && (!(hp->bh_flags & BH_DIRTY)
				|| mf_write(mfp, hp) != FAIL))
		    {
			mf_rem_ਵਰਤਿਆ(mfp, hp);
			mf_rem_hash(mfp, hp);
			mf_free_bhdr(hp);
			hp = mfp->mf_ਵਰਤਿਆ_ਆਖਰੀ;	// re-start, list was changed
			retval = TRUE;
		    }
		    ਨਹੀਂਤਾਂ
			hp = hp->bh_prev;
		}
	    }
	}
    }
    ਵਾਪਸ retval;
}

/*
 * Allocate a block header and a block of memory for it.
 */
    ਸਥਿਰ bhdr_T *
mf_alloc_bhdr(memfile_T *mfp, ਪੂਰਨ ਪੰਨਾ_ਗਿਣਤੀ)
{
    bhdr_T	*hp;

    ਜੇ ((hp = ALLOC_ONE(bhdr_T)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ ((hp->bh_ਅੰਕੜੇ = alloc((size_t)mfp->mf_ਪੰਨਾ_ਮਾਪ * ਪੰਨਾ_ਗਿਣਤੀ)) == ਨਿਰਾ)
    {
	vim_free(hp);	    // not enough memory
	ਵਾਪਸ ਨਿਰਾ;
    }
    hp->bh_ਪੰਨਾ_ਗਿਣਤੀ = ਪੰਨਾ_ਗਿਣਤੀ;
    ਵਾਪਸ hp;
}

/*
 * Free a block header and the block of memory for it.
 */
    ਸਥਿਰ ਖਾਲੀ
mf_free_bhdr(bhdr_T *hp)
{
    vim_free(hp->bh_ਅੰਕੜੇ);
    vim_free(hp);
}

/*
 * Insert entry *hp in the free list.
 */
    ਸਥਿਰ ਖਾਲੀ
mf_ins_free(memfile_T *mfp, bhdr_T *hp)
{
    hp->bh_ਅਗਲਾ = mfp->mf_free_ਪਹਿਲਾ;
    mfp->mf_free_ਪਹਿਲਾ = hp;
}

/*
 * remove the first entry from the free list and return a pointer to it
 * Note: caller must check that mfp->mf_free_first is not NULL!
 */
    ਸਥਿਰ bhdr_T *
mf_rem_free(memfile_T *mfp)
{
    bhdr_T	*hp;

    hp = mfp->mf_free_ਪਹਿਲਾ;
    mfp->mf_free_ਪਹਿਲਾ = hp->bh_ਅਗਲਾ;
    ਵਾਪਸ hp;
}

/*
 * read a block from disk
 *
 * Return FAIL for failure, OK otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
mf_read(memfile_T *mfp, bhdr_T *hp)
{
    off_T	ਵਿੱਥ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਪੰਨਾ_ਮਾਪ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਮਾਪ;

    ਜੇ (mfp->mf_fd < 0)	    // there is no file, can't read
	ਵਾਪਸ FAIL;

    ਪੰਨਾ_ਮਾਪ = mfp->mf_ਪੰਨਾ_ਮਾਪ;
    ਵਿੱਥ = (off_T)ਪੰਨਾ_ਮਾਪ * hp->bh_bnum;
    ਮਾਪ = ਪੰਨਾ_ਮਾਪ * hp->bh_ਪੰਨਾ_ਗਿਣਤੀ;
    ਜੇ (vim_lseek(mfp->mf_fd, ਵਿੱਥ, SEEK_SET) != ਵਿੱਥ)
    {
	PERROR(_(e_seek_error_ਵਿੱਚ_ਅਦਲਾ_ਬਦਲੀ_file_read));
	ਵਾਪਸ FAIL;
    }
    ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ)read_eintr(mfp->mf_fd, hp->bh_ਅੰਕੜੇ, ਮਾਪ) != ਮਾਪ)
    {
	PERROR(_(e_read_error_ਵਿੱਚ_ਅਦਲਾ_ਬਦਲੀ_file));
	ਵਾਪਸ FAIL;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    // Decrypt if 'key' is set and this is a data block. And when changing the
    // key.
    ਜੇ (*mfp->mf_ਬਫਰ->b_p_ਕੁੰਜੀ != NUL || mfp->mf_old_ਕੁੰਜੀ != ਨਿਰਾ)
	ml_decrypt_ਅੰਕੜੇ(mfp, hp->bh_ਅੰਕੜੇ, ਵਿੱਥ, ਮਾਪ);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * write a block to disk
 *
 * Return FAIL for failure, OK otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
mf_write(memfile_T *mfp, bhdr_T *hp)
{
    off_T	ਵਿੱਥ;	    // offset in the file
    blocknr_T	nr;	    // block nr which is being written
    bhdr_T	*hp2;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਪੰਨਾ_ਮਾਪ;  // number of bytes in a page
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਪੰਨਾ_ਗਿਣਤੀ; // number of pages written
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਮਾਪ;	    // number of bytes written

    ਜੇ (mfp->mf_fd < 0 && !mfp->mf_reopen)
	// there is no file and there was no file, can't write
	ਵਾਪਸ FAIL;

    ਜੇ (hp->bh_bnum < 0)	// must assign file block number
	ਜੇ (mf_trans_ਜੋੜੋ(mfp, hp) == FAIL)
	    ਵਾਪਸ FAIL;

    ਪੰਨਾ_ਮਾਪ = mfp->mf_ਪੰਨਾ_ਮਾਪ;

    /*
     * We don't want gaps in the file. Write the blocks in front of *hp
     * to extend the file.
     * If block 'mf_infile_count' is not in the hash list, it has been
     * freed. Fill the space in the file with data from the current block.
     */
    ਲਈ (;;)
    {
	ਪੂਰਨ attempt;

	nr = hp->bh_bnum;
	ਜੇ (nr > mfp->mf_infile_ਗਿਣਤੀ)		// beyond end of file
	{
	    nr = mfp->mf_infile_ਗਿਣਤੀ;
	    hp2 = mf_ਲੱਭੋ_hash(mfp, nr);	// NULL caught below
	}
	ਨਹੀਂਤਾਂ
	    hp2 = hp;

	ਵਿੱਥ = (off_T)ਪੰਨਾ_ਮਾਪ * nr;
	ਜੇ (hp2 == ਨਿਰਾ)	    // freed block, fill with dummy data
	    ਪੰਨਾ_ਗਿਣਤੀ = 1;
	ਨਹੀਂਤਾਂ
	    ਪੰਨਾ_ਗਿਣਤੀ = hp2->bh_ਪੰਨਾ_ਗਿਣਤੀ;
	ਮਾਪ = ਪੰਨਾ_ਮਾਪ * ਪੰਨਾ_ਗਿਣਤੀ;

	ਲਈ (attempt = 1; attempt <= 2; ++attempt)
	{
	    ਜੇ (mfp->mf_fd >= 0)
	    {
		ਜੇ (vim_lseek(mfp->mf_fd, ਵਿੱਥ, SEEK_SET) != ਵਿੱਥ)
		{
		    PERROR(_(e_seek_error_ਵਿੱਚ_ਅਦਲਾ_ਬਦਲੀ_file_write));
		    ਵਾਪਸ FAIL;
		}
		ਜੇ (mf_write_ਖੰਡ(mfp,
				   hp2 == ਨਿਰਾ ? hp : hp2, ਵਿੱਥ, ਮਾਪ) == OK)
		    ਰੋਕੋ;
	    }

	    ਜੇ (attempt == 1)
	    {
		// If the swap file is on a network drive, and the network
		// gets disconnected and then re-connected, we can maybe fix it
		// by closing and then re-opening the file.
		ਜੇ (mfp->mf_fd >= 0)
		    close(mfp->mf_fd);
		mfp->mf_fd = mch_open_rw((ਅੱਖਰ *)mfp->mf_fname, mfp->mf_flags);
		mfp->mf_reopen = (mfp->mf_fd < 0);
	    }
	    ਜੇ (attempt == 2 || mfp->mf_fd < 0)
	    {
		// Avoid repeating the error message, this mostly happens when
		// the disk is full. We give the message again only after a
		// successful write or when hitting a key. We keep on trying,
		// in case some space becomes available.
		ਜੇ (!did_swapwrite_ਸੁਨੇਹਾ)
		    emsg(_(e_write_error_ਵਿੱਚ_ਅਦਲਾ_ਬਦਲੀ_file));
		did_swapwrite_ਸੁਨੇਹਾ = TRUE;
		ਵਾਪਸ FAIL;
	    }
	}

	did_swapwrite_ਸੁਨੇਹਾ = FALSE;
	ਜੇ (hp2 != ਨਿਰਾ)		    // written a non-dummy block
	    hp2->bh_flags &= ~BH_DIRTY;
					    // appended to the file
	ਜੇ (nr + (blocknr_T)ਪੰਨਾ_ਗਿਣਤੀ > mfp->mf_infile_ਗਿਣਤੀ)
	    mfp->mf_infile_ਗਿਣਤੀ = nr + ਪੰਨਾ_ਗਿਣਤੀ;
	ਜੇ (nr == hp->bh_bnum)		    // written the desired block
	    ਰੋਕੋ;
    }
    ਵਾਪਸ OK;
}

/*
 * Write block "hp" with data size "size" to file "mfp->mf_fd".
 * Takes care of encryption.
 * Return FAIL or OK.
 */
    ਸਥਿਰ ਪੂਰਨ
mf_write_ਖੰਡ(
    memfile_T	*mfp,
    bhdr_T	*hp,
    off_T	ਵਿੱਥ ਅਣਵਰਤਿਆ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਮਾਪ)
{
    char_u	*ਅੰਕੜੇ = hp->bh_ਅੰਕੜੇ;
    ਪੂਰਨ		ਨਤੀਜਾ = OK;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    // Encrypt if 'key' is set and this is a data block.
    ਜੇ (*mfp->mf_ਬਫਰ->b_p_ਕੁੰਜੀ != NUL)
    {
	ਅੰਕੜੇ = ml_encrypt_ਅੰਕੜੇ(mfp, ਅੰਕੜੇ, ਵਿੱਥ, ਮਾਪ);
	ਜੇ (ਅੰਕੜੇ == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }
#ਜੇ_ਅੰਤ

    ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ)write_eintr(mfp->mf_fd, ਅੰਕੜੇ, ਮਾਪ) != ਮਾਪ)
	ਨਤੀਜਾ = FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    ਜੇ (ਅੰਕੜੇ != hp->bh_ਅੰਕੜੇ)
	vim_free(ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Make block number for *hp positive and add it to the translation list
 *
 * Return FAIL for failure, OK otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
mf_trans_ਜੋੜੋ(memfile_T *mfp, bhdr_T *hp)
{
    bhdr_T	*freep;
    blocknr_T	new_bnum;
    NR_TRANS	*np;
    ਪੂਰਨ		ਪੰਨਾ_ਗਿਣਤੀ;

    ਜੇ (hp->bh_bnum >= 0)		    // it's already positive
	ਵਾਪਸ OK;

    ਜੇ ((np = ALLOC_ONE(NR_TRANS)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

/*
 * Get a new number for the block.
 * If the first item in the free list has sufficient pages, use its number
 * Otherwise use mf_blocknr_max.
 */
    freep = mfp->mf_free_ਪਹਿਲਾ;
    ਪੰਨਾ_ਗਿਣਤੀ = hp->bh_ਪੰਨਾ_ਗਿਣਤੀ;
    ਜੇ (freep != ਨਿਰਾ && freep->bh_ਪੰਨਾ_ਗਿਣਤੀ >= ਪੰਨਾ_ਗਿਣਤੀ)
    {
	new_bnum = freep->bh_bnum;
	/*
	 * If the page count of the free block was larger, reduce it.
	 * If the page count matches, remove the block from the free list
	 */
	ਜੇ (freep->bh_ਪੰਨਾ_ਗਿਣਤੀ > ਪੰਨਾ_ਗਿਣਤੀ)
	{
	    freep->bh_bnum += ਪੰਨਾ_ਗਿਣਤੀ;
	    freep->bh_ਪੰਨਾ_ਗਿਣਤੀ -= ਪੰਨਾ_ਗਿਣਤੀ;
	}
	ਨਹੀਂਤਾਂ
	{
	    freep = mf_rem_free(mfp);
	    vim_free(freep);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	new_bnum = mfp->mf_blocknr_max;
	mfp->mf_blocknr_max += ਪੰਨਾ_ਗਿਣਤੀ;
    }

    np->nt_old_bnum = hp->bh_bnum;	    // adjust number
    np->nt_new_bnum = new_bnum;

    mf_rem_hash(mfp, hp);		    // remove from old hash list
    hp->bh_bnum = new_bnum;
    mf_ins_hash(mfp, hp);		    // insert in new hash list

    // Insert "np" into "mf_trans" hashtable with key "np->nt_old_bnum"
    mf_hash_ਜੋੜੋ_ਵਸਤੂ(&mfp->mf_trans, (mf_hashitem_T *)np);

    ਵਾਪਸ OK;
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

    np = (NR_TRANS *)mf_hash_ਲੱਭੋ(&mfp->mf_trans, old_nr);

    ਜੇ (np == ਨਿਰਾ)		// not found
	ਵਾਪਸ old_nr;

    mfp->mf_neg_ਗਿਣਤੀ--;
    new_bnum = np->nt_new_bnum;

    // remove entry from the trans list
    mf_hash_rem_ਵਸਤੂ(&mfp->mf_trans, (mf_hashitem_T *)np);

    vim_free(np);

    ਵਾਪਸ new_bnum;
}

/*
 * Set mfp->mf_ffname according to mfp->mf_fname and some other things.
 * Only called when creating or renaming the swapfile.	Either way it's a new
 * name so we must work out the full path name.
 */
    ਖਾਲੀ
mf_ਸੈੱਟ_ffname(memfile_T *mfp)
{
    mfp->mf_ffname = ਭਰਿਆ_Name_save(mfp->mf_fname, FALSE);
}

/*
 * Make the name of the file used for the memfile a full path.
 * Used before doing a :cd
 */
    ਖਾਲੀ
mf_fullname(memfile_T *mfp)
{
    ਜੇ (mfp == ਨਿਰਾ || mfp->mf_fname == ਨਿਰਾ || mfp->mf_ffname == ਨਿਰਾ)
	ਵਾਪਸ;

    vim_free(mfp->mf_fname);
    mfp->mf_fname = mfp->mf_ffname;
    mfp->mf_ffname = ਨਿਰਾ;
}

/*
 * return TRUE if there are any translations pending for 'mfp'
 */
    ਪੂਰਨ
mf_need_trans(memfile_T *mfp)
{
    ਵਾਪਸ (mfp->mf_fname != ਨਿਰਾ && mfp->mf_neg_ਗਿਣਤੀ > 0);
}

/*
 * Open a swap file for a memfile.
 * The "fname" must be in allocated memory, and is consumed (also when an
 * error occurs).
 */
    ਸਥਿਰ ਖਾਲੀ
mf_do_open(
    memfile_T	*mfp,
    char_u	*fname,
    ਪੂਰਨ		flags)		// flags for open()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LSTAT
    stat_T	sb;
#ਜੇ_ਅੰਤ

    mfp->mf_fname = fname;

    /*
     * Get the full path name before the open, because this is
     * not possible after the open on the Amiga.
     * fname cannot be NameBuff, because it must have been allocated.
     */
    mf_ਸੈੱਟ_ffname(mfp);
#ਜੇ defined(MSWIN)
    /*
     * A ":!cd e:xxx" may change the directory without us knowing, use the
     * full pathname always.  Careful: This frees fname!
     */
    mf_fullname(mfp);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LSTAT
    /*
     * Extra security check: When creating a swap file it really shouldn't
     * exist yet.  If there is a symbolic link, this is most likely an attack.
     */
    ਜੇ ((flags & O_CREAT) && mch_lstat((ਅੱਖਰ *)mfp->mf_fname, &sb) >= 0)
    {
	mfp->mf_fd = -1;
	emsg(_(e_ਅਦਲਾ_ਬਦਲੀ_file_already_exists_symlink_attack));
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	/*
	 * try to open the file
	 */
	flags |= O_EXTRA | O_NOFOLLOW;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	// Prevent handle inheritance that cause problems with Cscope
	// (swap file may not be deleted if cscope connection was open after
	// the file)
	flags |= O_NOINHERIT;
#ਜੇ_ਅੰਤ
	mfp->mf_flags = flags;
	mfp->mf_fd = mch_open_rw((ਅੱਖਰ *)mfp->mf_fname, flags);
    }

    /*
     * If the file cannot be opened, use memory only
     */
    ਜੇ (mfp->mf_fd < 0)
    {
	VIM_CLEAR(mfp->mf_fname);
	VIM_CLEAR(mfp->mf_ffname);
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FD_CLOEXEC
	ਪੂਰਨ fdflags = fcntl(mfp->mf_fd, F_GETFD);
	ਜੇ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (ਖਾਲੀ)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
#ਜੇ_ਅੰਤ
#ਜੇ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
	mch_copy_sec(fname, mfp->mf_fname);
#ਜੇ_ਅੰਤ
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MHT_LOG_ਲੱਦੋ_ਗੁਣਕ 6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MHT_GROWTH_ਗੁਣਕ   2   // must be a power of two

/*
 * Initialize an empty hash table.
 */
    ਸਥਿਰ ਖਾਲੀ
mf_hash_init(mf_hashtab_T *mht)
{
    CLEAR_ਸੂਚਕ(mht);
    mht->mht_buckets = mht->mht_small_buckets;
    mht->mht_ਨਕਾਬ = MHT_INIT_ਮਾਪ - 1;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * The hash table must not be used again without another mf_hash_init() call.
 */
    ਸਥਿਰ ਖਾਲੀ
mf_hash_free(mf_hashtab_T *mht)
{
    ਜੇ (mht->mht_buckets != mht->mht_small_buckets)
	vim_free(mht->mht_buckets);
}

/*
 * Free the array of a hash table and all the items it contains.
 */
    ਸਥਿਰ ਖਾਲੀ
mf_hash_free_all(mf_hashtab_T *mht)
{
    long_u	    idx;
    mf_hashitem_T   *mhi;
    mf_hashitem_T   *ਅਗਲਾ;

    ਲਈ (idx = 0; idx <= mht->mht_ਨਕਾਬ; idx++)
	ਲਈ (mhi = mht->mht_buckets[idx]; mhi != ਨਿਰਾ; mhi = ਅਗਲਾ)
	{
	    ਅਗਲਾ = mhi->mhi_ਅਗਲਾ;
	    vim_free(mhi);
	}

    mf_hash_free(mht);
}

/*
 * Find "key" in hashtable "mht".
 * Returns a pointer to a mf_hashitem_T or NULL if the item was not found.
 */
    ਸਥਿਰ mf_hashitem_T *
mf_hash_ਲੱਭੋ(mf_hashtab_T *mht, blocknr_T ਕੁੰਜੀ)
{
    mf_hashitem_T   *mhi;

    mhi = mht->mht_buckets[ਕੁੰਜੀ & mht->mht_ਨਕਾਬ];
    ਜਦੋਂ (mhi != ਨਿਰਾ && mhi->mhi_ਕੁੰਜੀ != ਕੁੰਜੀ)
	mhi = mhi->mhi_ਅਗਲਾ;

    ਵਾਪਸ mhi;
}

/*
 * Add item "mhi" to hashtable "mht".
 * "mhi" must not be NULL.
 */
    ਸਥਿਰ ਖਾਲੀ
mf_hash_ਜੋੜੋ_ਵਸਤੂ(mf_hashtab_T *mht, mf_hashitem_T *mhi)
{
    long_u	    idx;

    idx = mhi->mhi_ਕੁੰਜੀ & mht->mht_ਨਕਾਬ;
    mhi->mhi_ਅਗਲਾ = mht->mht_buckets[idx];
    mhi->mhi_prev = ਨਿਰਾ;
    ਜੇ (mhi->mhi_ਅਗਲਾ != ਨਿਰਾ)
	mhi->mhi_ਅਗਲਾ->mhi_prev = mhi;
    mht->mht_buckets[idx] = mhi;

    mht->mht_ਗਿਣਤੀ++;

    /*
     * Grow hashtable when we have more thank 2^MHT_LOG_LOAD_FACTOR
     * items per bucket on average
     */
    ਜੇ (mht->mht_fixed == 0
	&& (mht->mht_ਗਿਣਤੀ >> MHT_LOG_ਲੱਦੋ_ਗੁਣਕ) > mht->mht_ਨਕਾਬ)
    {
	ਜੇ (mf_hash_grow(mht) == FAIL)
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
    ਸਥਿਰ ਖਾਲੀ
mf_hash_rem_ਵਸਤੂ(mf_hashtab_T *mht, mf_hashitem_T *mhi)
{
    ਜੇ (mhi->mhi_prev == ਨਿਰਾ)
	mht->mht_buckets[mhi->mhi_ਕੁੰਜੀ & mht->mht_ਨਕਾਬ] = mhi->mhi_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	mhi->mhi_prev->mhi_ਅਗਲਾ = mhi->mhi_ਅਗਲਾ;

    ਜੇ (mhi->mhi_ਅਗਲਾ != ਨਿਰਾ)
	mhi->mhi_ਅਗਲਾ->mhi_prev = mhi->mhi_prev;

    mht->mht_ਗਿਣਤੀ--;

    // We could shrink the table here, but it typically takes little memory,
    // so why bother?
}

/*
 * Increase number of buckets in the hashtable by MHT_GROWTH_FACTOR and
 * rehash items.
 * Returns FAIL when out of memory.
 */
    ਸਥਿਰ ਪੂਰਨ
mf_hash_grow(mf_hashtab_T *mht)
{
    long_u	    i, j;
    ਪੂਰਨ		    ਖਿਸਕਾਓ;
    mf_hashitem_T   *mhi;
    mf_hashitem_T   *tails[MHT_GROWTH_ਗੁਣਕ];
    mf_hashitem_T   **buckets;
    size_t	    ਮਾਪ;

    ਮਾਪ = (mht->mht_ਨਕਾਬ + 1) * MHT_GROWTH_ਗੁਣਕ * ਆਕਾਰ(ਖਾਲੀ *);
    buckets = lalloc_clear(ਮਾਪ, FALSE);
    ਜੇ (buckets == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਖਿਸਕਾਓ = 0;
    ਜਦੋਂ ((mht->mht_ਨਕਾਬ >> ਖਿਸਕਾਓ) != 0)
	ਖਿਸਕਾਓ++;

    ਲਈ (i = 0; i <= mht->mht_ਨਕਾਬ; i++)
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

	CLEAR_ਖੇਤਰ(tails);

	ਲਈ (mhi = mht->mht_buckets[i]; mhi != ਨਿਰਾ; mhi = mhi->mhi_ਅਗਲਾ)
	{
	    j = (mhi->mhi_ਕੁੰਜੀ >> ਖਿਸਕਾਓ) & (MHT_GROWTH_ਗੁਣਕ - 1);
	    ਜੇ (tails[j] == ਨਿਰਾ)
	    {
		buckets[i + (j << ਖਿਸਕਾਓ)] = mhi;
		tails[j] = mhi;
		mhi->mhi_prev = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		tails[j]->mhi_ਅਗਲਾ = mhi;
		mhi->mhi_prev = tails[j];
		tails[j] = mhi;
	    }
	}

	ਲਈ (j = 0; j < MHT_GROWTH_ਗੁਣਕ; j++)
	    ਜੇ (tails[j] != ਨਿਰਾ)
		tails[j]->mhi_ਅਗਲਾ = ਨਿਰਾ;
    }

    ਜੇ (mht->mht_buckets != mht->mht_small_buckets)
	vim_free(mht->mht_buckets);

    mht->mht_buckets = buckets;
    mht->mht_ਨਕਾਬ = (mht->mht_ਨਕਾਬ + 1) * MHT_GROWTH_ਗੁਣਕ - 1;

    ਵਾਪਸ OK;
}
