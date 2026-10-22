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

#అన్తర్భావ "vim.h"

/*
 * Some systems have the page size in statfs.f_bsize, some in stat.st_blksize
 */
#నిర్వచితమైతే HAVE_ST_BLKSIZE
# define STATFS stat
# define F_BSIZE st_blksize
# define fstatfs(fd, buf, len, nul) mch_fstat((fd), (buf))
#లేకపోతే
# ifdef HAVE_SYS_STATFS_H
#  include <sys/statfs.h>
#  define STATFS statfs
#  define F_BSIZE f_bsize
# endif
#అయితే_ముగింపు

/*
 * for Amiga Dos 2.0x we use Flush
 */
#నిర్వచితమైతే AMIGA
# ifdef FEAT_ARP
బాహ్య పూర్ణాంకం dos2;			// this is in os_amiga.c
# endif
# ifdef SASC
#  include <ios1.h>			// for chkufb()
# endif
#అయితే_ముగింపు

#నిర్వచించు MEMFILE_పుట_కొలత 4096		// default page size

స్థిర long_u	మొత్తం_mem_వాడిన = 0;	// total memory used for memfiles

స్థిర శూన్య mf_ins_hash(memfile_T *, bhdr_T *);
స్థిర శూన్య mf_rem_hash(memfile_T *, bhdr_T *);
స్థిర bhdr_T *mf_కనుగొను_hash(memfile_T *, blocknr_T);
స్థిర శూన్య mf_ins_వాడిన(memfile_T *, bhdr_T *);
స్థిర శూన్య mf_rem_వాడిన(memfile_T *, bhdr_T *);
స్థిర bhdr_T *mf_release(memfile_T *, పూర్ణాంకం);
స్థిర bhdr_T *mf_alloc_bhdr(memfile_T *, పూర్ణాంకం);
స్థిర శూన్య mf_free_bhdr(bhdr_T *);
స్థిర శూన్య mf_ins_free(memfile_T *, bhdr_T *);
స్థిర bhdr_T *mf_rem_free(memfile_T *);
స్థిర పూర్ణాంకం  mf_read(memfile_T *, bhdr_T *);
స్థిర పూర్ణాంకం  mf_write(memfile_T *, bhdr_T *);
స్థిర పూర్ణాంకం  mf_write_ఖండం(memfile_T *mfp, bhdr_T *hp, off_T స్థానభ్రంశం, గుర్తులేని కొలత);
స్థిర పూర్ణాంకం  mf_trans_కూడు(memfile_T *, bhdr_T *);
స్థిర శూన్య mf_do_open(memfile_T *, char_u *, పూర్ణాంకం);
స్థిర శూన్య mf_hash_init(mf_hashtab_T *);
స్థిర శూన్య mf_hash_free(mf_hashtab_T *);
స్థిర శూన్య mf_hash_free_all(mf_hashtab_T *);
స్థిర mf_hashitem_T *mf_hash_కనుగొను(mf_hashtab_T *, blocknr_T);
స్థిర శూన్య mf_hash_కూడు_అంశం(mf_hashtab_T *, mf_hashitem_T *);
స్థిర శూన్య mf_hash_rem_అంశం(mf_hashtab_T *, mf_hashitem_T *);
స్థిర పూర్ణాంకం mf_hash_grow(mf_hashtab_T *);

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
mf_open(char_u *fname, పూర్ణాంకం flags)
{
    memfile_T		*mfp;
    off_T		కొలత;
#ఒకవేళ defined(STATFS) && defined(UNIX) && !defined(__QNX__) && !defined(__minix)
# define USE_FSTATFS
    నిర్మాణం STATFS	stf;
#అయితే_ముగింపు

    ఒకవేళ ((mfp = ALLOC_ONE(memfile_T)) == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (fname == ఖాళీ)	    // no file for this memfile, use memory only
    {
	mfp->mf_fname = ఖాళీ;
	mfp->mf_ffname = ఖాళీ;
	mfp->mf_fd = -1;
    }
    లేకపోతే
    {
	mf_do_open(mfp, fname, flags);	// try to open the file

	// if the file cannot be opened, return here
	ఒకవేళ (mfp->mf_fd < 0)
	{
	    vim_free(mfp);
	    తిరిగి ఖాళీ;
	}
    }

    mfp->mf_free_మొదటి = ఖాళీ;		// free list is empty
    mfp->mf_వాడిన_మొదటి = ఖాళీ;		// used list is empty
    mfp->mf_వాడిన_చివరి = ఖాళీ;
    mfp->mf_dirty = MF_DIRTY_NO;
    mfp->mf_వాడిన_లెక్క = 0;
    mf_hash_init(&mfp->mf_hash);
    mf_hash_init(&mfp->mf_trans);
    mfp->mf_పుట_కొలత = MEMFILE_పుట_కొలత;
#నిర్వచితమైతే FEAT_CRYPT
    mfp->mf_old_కీలకం = ఖాళీ;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_FSTATFS
    /*
     * Try to set the page size equal to the block size of the device.
     * Speeds up I/O a lot.
     * When recovering, the actual block size will be retrieved from block 0
     * in ml_recover().  The size used here may be wrong, therefore
     * mf_blocknr_max must be rounded up.
     */
    ఒకవేళ (mfp->mf_fd >= 0
	    && fstatfs(mfp->mf_fd, &stf, పరిమాణం(నిర్మాణం statfs), 0) == 0
	    && stf.F_BSIZE >= MIN_మార్పిడి_పుట_కొలత
	    && stf.F_BSIZE <= MAX_మార్పిడి_పుట_కొలత)
	mfp->mf_పుట_కొలత = stf.F_BSIZE;
#అయితే_ముగింపు

    ఒకవేళ (mfp->mf_fd < 0 || (flags & (O_TRUNC|O_EXCL))
		  || (కొలత = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	mfp->mf_blocknr_max = 0;	// no file or empty file
    లేకపోతే
	mfp->mf_blocknr_max = (blocknr_T)((కొలత + mfp->mf_పుట_కొలత - 1)
							 / mfp->mf_పుట_కొలత);
    mfp->mf_blocknr_min = -1;
    mfp->mf_neg_లెక్క = 0;
    mfp->mf_infile_లెక్క = mfp->mf_blocknr_max;

    /*
     * Compute maximum number of pages ('maxmem' is in Kbyte):
     *	'mammem' * 1Kbyte / page-size-in-bytes.
     * Avoid overflow by first reducing page size as much as possible.
     */
    {
	పూర్ణాంకం	    జరుపు = 10;
	గుర్తులేని    పుట_కొలత = mfp->mf_పుట_కొలత;

	అయితే (జరుపు > 0 && (పుట_కొలత & 1) == 0)
	{
	    పుట_కొలత = పుట_కొలత >> 1;
	    --జరుపు;
	}
	mfp->mf_వాడిన_లెక్క_max = (p_mm << జరుపు) / పుట_కొలత;
	ఒకవేళ (mfp->mf_వాడిన_లెక్క_max < 10)
	    mfp->mf_వాడిన_లెక్క_max = 10;
    }

    తిరిగి mfp;
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
    పూర్ణాంకం
mf_open_file(memfile_T *mfp, char_u *fname)
{
    mf_do_open(mfp, fname, O_RDWR|O_CREAT|O_EXCL); // try to open the file

    ఒకవేళ (mfp->mf_fd < 0)
	తిరిగి FAIL;

    mfp->mf_dirty = MF_DIRTY_YES;
    తిరిగి OK;
}

/*
 * Close a memory file and delete the associated file if 'del_file' is TRUE.
 */
    శూన్య
mf_close(memfile_T *mfp, పూర్ణాంకం del_file)
{
    bhdr_T	*hp, *nextp;

    ఒకవేళ (mfp == ఖాళీ)		    // safety check
	తిరిగి;
    ఒకవేళ (mfp->mf_fd >= 0)
    {
	ఒకవేళ (close(mfp->mf_fd) < 0)
	    emsg(_(e_close_error_on_మార్పిడి_file));
    }
    ఒకవేళ (del_file && mfp->mf_fname != ఖాళీ)
	mch_remove(mfp->mf_fname);
					    // free entries in used list
    కొరకు (hp = mfp->mf_వాడిన_మొదటి; hp != ఖాళీ; hp = nextp)
    {
	మొత్తం_mem_వాడిన -= (long_u)hp->bh_పుట_లెక్క * mfp->mf_పుట_కొలత;
	nextp = hp->bh_తదుపరి;
	mf_free_bhdr(hp);
    }
    అయితే (mfp->mf_free_మొదటి != ఖాళీ)	    // free entries in free list
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
    శూన్య
mf_close_file(
    buf_T	*buf,
    పూర్ణాంకం		getlines)	// get all lines into memory?
{
    memfile_T	*mfp;
    linenr_T	lnum;

    mfp = buf->b_ml.ml_mfp;
    ఒకవేళ (mfp == ఖాళీ || mfp->mf_fd < 0)		// nothing to close
	తిరిగి;

    ఒకవేళ (getlines)
    {
	// get all blocks in memory by accessing all lines (clumsy!)
	mf_dont_release = TRUE;
	కొరకు (lnum = 1; lnum <= buf->b_ml.ml_పంక్తి_లెక్క; ++lnum)
	    (శూన్య)ml_get_buf(buf, lnum, FALSE);
	mf_dont_release = FALSE;
	// TODO: should check if all blocks are really in core
    }

    ఒకవేళ (close(mfp->mf_fd) < 0)			// close the file
	emsg(_(e_close_error_on_మార్పిడి_file));
    mfp->mf_fd = -1;

    ఒకవేళ (mfp->mf_fname != ఖాళీ)
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
    శూన్య
mf_new_పుట_కొలత(memfile_T *mfp, గుర్తులేని new_కొలత)
{
    // Correct the memory used for block 0 to the new size, because it will be
    // freed with that size later on.
    మొత్తం_mem_వాడిన += new_కొలత - mfp->mf_పుట_కొలత;
    mfp->mf_పుట_కొలత = new_కొలత;
}

/*
 * get a new block
 *
 *   negative: TRUE if negative block number desired (data block)
 */
    bhdr_T *
mf_new(memfile_T *mfp, పూర్ణాంకం negative, పూర్ణాంకం పుట_లెక్క)
{
    bhdr_T	*hp;	// new bhdr_T
    bhdr_T	*freep;	// first block in free list
    char_u	*p;

    /*
     * If we reached the maximum size for the used memory blocks, release one
     * If a bhdr_T is returned, use it and adjust the page_count if necessary.
     */
    hp = mf_release(mfp, పుట_లెక్క);

/*
 * Decide on the number to use:
 * If there is a free block, use its number.
 * Otherwise use mf_block_min for a negative number, mf_block_max for
 * a positive number.
 */
    freep = mfp->mf_free_మొదటి;
    ఒకవేళ (!negative && freep != ఖాళీ && freep->bh_పుట_లెక్క >= పుట_లెక్క)
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
	ఒకవేళ (freep->bh_పుట_లెక్క > పుట_లెక్క)
	{
	    ఒకవేళ (hp == ఖాళీ && (hp = mf_alloc_bhdr(mfp, పుట_లెక్క)) == ఖాళీ)
		తిరిగి ఖాళీ;
	    hp->bh_bnum = freep->bh_bnum;
	    freep->bh_bnum += పుట_లెక్క;
	    freep->bh_పుట_లెక్క -= పుట_లెక్క;
	}
	లేకపోతే ఒకవేళ (hp == ఖాళీ)	    // need to allocate memory for this block
	{
	    ఒకవేళ ((p = alloc((size_t)mfp->mf_పుట_కొలత * పుట_లెక్క)) == ఖాళీ)
		తిరిగి ఖాళీ;
	    hp = mf_rem_free(mfp);
	    hp->bh_దత్తాంశం = p;
	}
	లేకపోతే		    // use the number, remove entry from free list
	{
	    freep = mf_rem_free(mfp);
	    hp->bh_bnum = freep->bh_bnum;
	    vim_free(freep);
	}
    }
    లేకపోతే	// get a new number
    {
	ఒకవేళ (hp == ఖాళీ && (hp = mf_alloc_bhdr(mfp, పుట_లెక్క)) == ఖాళీ)
	    తిరిగి ఖాళీ;
	ఒకవేళ (negative)
	{
	    hp->bh_bnum = mfp->mf_blocknr_min--;
	    mfp->mf_neg_లెక్క++;
	}
	లేకపోతే
	{
	    hp->bh_bnum = mfp->mf_blocknr_max;
	    mfp->mf_blocknr_max += పుట_లెక్క;
	}
    }
    hp->bh_flags = BH_LOCKED | BH_DIRTY;	// new block is always dirty
    mfp->mf_dirty = MF_DIRTY_YES;
    hp->bh_పుట_లెక్క = పుట_లెక్క;
    mf_ins_వాడిన(mfp, hp);
    mf_ins_hash(mfp, hp);

    /*
     * Init the data to all zero, to avoid reading uninitialized data.
     * This also avoids that the passwd file ends up in the swap file!
     */
    (శూన్య)vim_memset((అక్షరం *)(hp->bh_దత్తాంశం), 0,
				      (size_t)mfp->mf_పుట_కొలత * పుట_లెక్క);

    తిరిగి hp;
}

/*
 * Get existing block "nr" with "page_count" pages.
 *
 * Note: The caller should first check a negative nr with mf_trans_del()
 */
    bhdr_T *
mf_get(memfile_T *mfp, blocknr_T nr, పూర్ణాంకం పుట_లెక్క)
{
    bhdr_T    *hp;
						// doesn't exist
    ఒకవేళ (nr >= mfp->mf_blocknr_max || nr <= mfp->mf_blocknr_min)
	తిరిగి ఖాళీ;

    /*
     * see if it is in the cache
     */
    hp = mf_కనుగొను_hash(mfp, nr);
    ఒకవేళ (hp == ఖాళీ)	// not in the hash list
    {
	ఒకవేళ (nr < 0 || nr >= mfp->mf_infile_లెక్క)   // can't be in the file
	    తిరిగి ఖాళీ;

	// could check here if the block is in the free list

	/*
	 * Check if we need to flush an existing block.
	 * If so, use that block.
	 * If not, allocate a new block.
	 */
	hp = mf_release(mfp, పుట_లెక్క);
	ఒకవేళ (hp == ఖాళీ && పుట_లెక్క > 0)
	    hp = mf_alloc_bhdr(mfp, పుట_లెక్క);
	ఒకవేళ (hp == ఖాళీ)
	    తిరిగి ఖాళీ;

	hp->bh_bnum = nr;
	hp->bh_flags = 0;
	hp->bh_పుట_లెక్క = పుట_లెక్క;
	ఒకవేళ (mf_read(mfp, hp) == FAIL)	    // cannot read the block!
	{
	    mf_free_bhdr(hp);
	    తిరిగి ఖాళీ;
	}
    }
    లేకపోతే
    {
	mf_rem_వాడిన(mfp, hp);	// remove from list, insert in front below
	mf_rem_hash(mfp, hp);
    }

    hp->bh_flags |= BH_LOCKED;
    mf_ins_వాడిన(mfp, hp);	// put in front of used list
    mf_ins_hash(mfp, hp);	// put in front of hash list

    తిరిగి hp;
}

/*
 * release the block *hp
 *
 *   dirty: Block must be written to file later
 *   infile: Block should be in file (needed for recovery)
 *
 *  no return value, function cannot fail
 */
    శూన్య
mf_put(
    memfile_T	*mfp,
    bhdr_T	*hp,
    పూర్ణాంకం		dirty,
    పూర్ణాంకం		infile)
{
    పూర్ణాంకం		flags;

    flags = hp->bh_flags;

    ఒకవేళ ((flags & BH_LOCKED) == 0)
	iemsg(e_ఖండం_was_not_locked);
    flags &= ~BH_LOCKED;
    ఒకవేళ (dirty)
    {
	flags |= BH_DIRTY;
	ఒకవేళ (mfp->mf_dirty != MF_DIRTY_YES_NOSYNC)
	    mfp->mf_dirty = MF_DIRTY_YES;
    }
    hp->bh_flags = flags;
    ఒకవేళ (infile)
	mf_trans_కూడు(mfp, hp);	    // may translate negative in positive nr
}

/*
 * block *hp is no longer in used, may put it in the free list of memfile *mfp
 */
    శూన్య
mf_free(memfile_T *mfp, bhdr_T *hp)
{
    vim_free(hp->bh_దత్తాంశం);	// free the memory
    mf_rem_hash(mfp, hp);	// get *hp out of the hash list
    mf_rem_వాడిన(mfp, hp);	// get *hp out of the used list
    ఒకవేళ (hp->bh_bnum < 0)
    {
	vim_free(hp);		// don't want negative numbers in free list
	mfp->mf_neg_లెక్క--;
    }
    లేకపోతే
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
    పూర్ణాంకం
mf_sync(memfile_T *mfp, పూర్ణాంకం flags)
{
    పూర్ణాంకం		స్థితి_2;
    bhdr_T	*hp;
    పూర్ణాంకం		got_int_save = got_int;

    ఒకవేళ (mfp->mf_fd < 0)
    {
	// there is no file, nothing to do
	mfp->mf_dirty = MF_DIRTY_NO;
	తిరిగి FAIL;
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
    స్థితి_2 = OK;
    కొరకు (hp = mfp->mf_వాడిన_చివరి; hp != ఖాళీ; hp = hp->bh_prev)
	ఒకవేళ (((flags & MFS_ALL) || hp->bh_bnum >= 0)
		&& (hp->bh_flags & BH_DIRTY)
		&& (స్థితి_2 == OK || (hp->bh_bnum >= 0
		    && hp->bh_bnum < mfp->mf_infile_లెక్క)))
	{
	    ఒకవేళ ((flags & MFS_ZERO) && hp->bh_bnum != 0)
		కొనసాగు;
	    ఒకవేళ (mf_write(mfp, hp) == FAIL)
	    {
		ఒకవేళ (స్థితి_2 == FAIL)	// double error: quit syncing
		    ఆపు;
		స్థితి_2 = FAIL;
	    }
	    ఒకవేళ (flags & MFS_STOP)
	    {
		// Stop when char available now.
		ఒకవేళ (ui_char_avail())
		    ఆపు;
	    }
	    లేకపోతే
		ui_breakcheck();
	    ఒకవేళ (got_int)
		ఆపు;
	}

    /*
     * If the whole list is flushed, the memfile is not dirty anymore.
     * In case of an error this flag is also set, to avoid trying all the time.
     */
    ఒకవేళ (hp == ఖాళీ || స్థితి_2 == FAIL)
	mfp->mf_dirty = MF_DIRTY_NO;

    ఒకవేళ ((flags & MFS_FLUSH) && *p_sws != NUL)
    {
#ఒకవేళ defined(UNIX)
# ifdef HAVE_FSYNC
	/*
	 * most Unixes have the very useful fsync() function, just what we need.
	 */
	ఒకవేళ (STRCMP(p_sws, "fsync") == 0)
	{
	    ఒకవేళ (vim_fsync(mfp->mf_fd))
		స్థితి_2 = FAIL;
	}
	లేకపోతే
# endif
# ifdef HAVE_SYNC
	    sync();
# లేకపోతే
	    fflush(ఖాళీ);
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే VMS
	ఒకవేళ (STRCMP(p_sws, "fsync") == 0)
	{
	    ఒకవేళ (vim_fsync(mfp->mf_fd))
		స్థితి_2 = FAIL;
	}
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN
	ఒకవేళ (_commit(mfp->mf_fd))
	    స్థితి_2 = FAIL;
#అయితే_ముగింపు
#నిర్వచితమైతే AMIGA
# ఒకవేళ defined(__AROS__) || defined(__amigaos4__)
	ఒకవేళ (vim_fsync(mfp->mf_fd) != 0)
	    స్థితి_2 = FAIL;
# లేకపోతే
	/*
	 * Flush() only exists for AmigaDos 2.0.
	 * For 1.3 it should be done with close() + open(), but then the risk
	 * is that the open() may fail and lose the file....
	 */
#  ifdef FEAT_ARP
	ఒకవేళ (dos2)
#  endif
#  ifdef SASC
	{
	    నిర్మాణం UFB *fp = chkufb(mfp->mf_fd);

	    ఒకవేళ (fp != ఖాళీ)
		Flush(fp->ufbfh);
	}
#  లేకపోతే
#   ఒకవేళ defined(_DCC) || defined(__GNUC__) || defined(__MORPHOS__)
	{
#    ఒకవేళ defined(__GNUC__) && !defined(__MORPHOS__) && defined(__libnix__)
	    // Have function (in libnix at least),
	    // but ain't got no prototype anywhere.
	    బాహ్య గుర్తులేని దీర్ఘ fdtofh(పూర్ణాంకం filedescriptor);
#    endif
#    ఒకవేళ !defined(__libnix__)
	    fflush(ఖాళీ);
#    లేకపోతే
	    BPTR fh = (BPTR)fdtofh(mfp->mf_fd);

	    ఒకవేళ (fh != 0)
		Flush(fh);
#    endif
	}
#   లేకపోతే // assume Manx
	    Flush(_devtab[mfp->mf_fd].fd);
#   endif
#  endif
# endif
#అయితే_ముగింపు // AMIGA
    }

    got_int |= got_int_save;

    తిరిగి స్థితి_2;
}

/*
 * For all blocks in memory file *mfp that have a positive block number set
 * the dirty flag.  These are blocks that need to be written to a newly
 * created swapfile.
 */
    శూన్య
mf_సమితి_dirty(memfile_T *mfp)
{
    bhdr_T	*hp;

    కొరకు (hp = mfp->mf_వాడిన_చివరి; hp != ఖాళీ; hp = hp->bh_prev)
	ఒకవేళ (hp->bh_bnum > 0)
	    hp->bh_flags |= BH_DIRTY;
    mfp->mf_dirty = MF_DIRTY_YES;
}

/*
 * insert block *hp in front of hashlist of memfile *mfp
 */
    స్థిర శూన్య
mf_ins_hash(memfile_T *mfp, bhdr_T *hp)
{
    mf_hash_కూడు_అంశం(&mfp->mf_hash, (mf_hashitem_T *)hp);
}

/*
 * remove block *hp from hashlist of memfile list *mfp
 */
    స్థిర శూన్య
mf_rem_hash(memfile_T *mfp, bhdr_T *hp)
{
    mf_hash_rem_అంశం(&mfp->mf_hash, (mf_hashitem_T *)hp);
}

/*
 * look in hash lists of memfile *mfp for block header with number 'nr'
 */
    స్థిర bhdr_T *
mf_కనుగొను_hash(memfile_T *mfp, blocknr_T nr)
{
    తిరిగి (bhdr_T *)mf_hash_కనుగొను(&mfp->mf_hash, nr);
}

/*
 * insert block *hp in front of used list of memfile *mfp
 */
    స్థిర శూన్య
mf_ins_వాడిన(memfile_T *mfp, bhdr_T *hp)
{
    hp->bh_తదుపరి = mfp->mf_వాడిన_మొదటి;
    mfp->mf_వాడిన_మొదటి = hp;
    hp->bh_prev = ఖాళీ;
    ఒకవేళ (hp->bh_తదుపరి == ఖాళీ)	    // list was empty, adjust last pointer
	mfp->mf_వాడిన_చివరి = hp;
    లేకపోతే
	hp->bh_తదుపరి->bh_prev = hp;
    mfp->mf_వాడిన_లెక్క += hp->bh_పుట_లెక్క;
    మొత్తం_mem_వాడిన += (long_u)hp->bh_పుట_లెక్క * mfp->mf_పుట_కొలత;
}

/*
 * remove block *hp from used list of memfile *mfp
 */
    స్థిర శూన్య
mf_rem_వాడిన(memfile_T *mfp, bhdr_T *hp)
{
    ఒకవేళ (hp->bh_తదుపరి == ఖాళీ)	    // last block in used list
	mfp->mf_వాడిన_చివరి = hp->bh_prev;
    లేకపోతే
	hp->bh_తదుపరి->bh_prev = hp->bh_prev;
    ఒకవేళ (hp->bh_prev == ఖాళీ)	    // first block in used list
	mfp->mf_వాడిన_మొదటి = hp->bh_తదుపరి;
    లేకపోతే
	hp->bh_prev->bh_తదుపరి = hp->bh_తదుపరి;
    mfp->mf_వాడిన_లెక్క -= hp->bh_పుట_లెక్క;
    మొత్తం_mem_వాడిన -= (long_u)hp->bh_పుట_లెక్క * mfp->mf_పుట_కొలత;
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
    స్థిర bhdr_T *
mf_release(memfile_T *mfp, పూర్ణాంకం పుట_లెక్క)
{
    bhdr_T	*hp;
    పూర్ణాంకం		need_release;
    buf_T	*buf;

    // don't release while in mf_close_file()
    ఒకవేళ (mf_dont_release)
	తిరిగి ఖాళీ;

    /*
     * Need to release a block if the number of blocks for this memfile is
     * higher than the maximum or total memory used is over 'maxmemtot'
     */
    need_release = ((mfp->mf_వాడిన_లెక్క >= mfp->mf_వాడిన_లెక్క_max)
				  || (మొత్తం_mem_వాడిన >> 10) >= (long_u)p_mmt);

    /*
     * Try to create a swap file if the amount of memory used is getting too
     * high.
     */
    ఒకవేళ (mfp->mf_fd < 0 && need_release && p_uc)
    {
	// find for which buffer this memfile is
	FOR_ALL_BUFFERS(buf)
	    ఒకవేళ (buf->b_ml.ml_mfp == mfp)
		ఆపు;
	ఒకవేళ (buf != ఖాళీ && buf->b_may_మార్పిడి)
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
    ఒకవేళ (mfp->mf_fd < 0 || !need_release)
	తిరిగి ఖాళీ;

    కొరకు (hp = mfp->mf_వాడిన_చివరి; hp != ఖాళీ; hp = hp->bh_prev)
	ఒకవేళ (!(hp->bh_flags & BH_LOCKED))
	    ఆపు;
    ఒకవేళ (hp == ఖాళీ)	// not a single one that can be released
	తిరిగి ఖాళీ;

    /*
     * If the block is dirty, write it.
     * If the write fails we don't free it.
     */
    ఒకవేళ ((hp->bh_flags & BH_DIRTY) && mf_write(mfp, hp) == FAIL)
	తిరిగి ఖాళీ;

    mf_rem_వాడిన(mfp, hp);
    mf_rem_hash(mfp, hp);

    /*
     * If a bhdr_T is returned, make sure that the page_count of bh_data is
     * right
     */
    ఒకవేళ (hp->bh_పుట_లెక్క != పుట_లెక్క)
    {
	VIM_CLEAR(hp->bh_దత్తాంశం);
	ఒకవేళ (పుట_లెక్క > 0)
	    hp->bh_దత్తాంశం = alloc((size_t)mfp->mf_పుట_కొలత * పుట_లెక్క);
	ఒకవేళ (hp->bh_దత్తాంశం == ఖాళీ)
	{
	    vim_free(hp);
	    తిరిగి ఖాళీ;
	}
	hp->bh_పుట_లెక్క = పుట_లెక్క;
    }
    తిరిగి hp;
}

/*
 * release as many blocks as possible
 * Used in case of out of memory
 *
 * return TRUE if any memory was released
 */
    పూర్ణాంకం
mf_release_all(శూన్య)
{
    buf_T	*buf;
    memfile_T	*mfp;
    bhdr_T	*hp;
    పూర్ణాంకం		retval = FALSE;

    FOR_ALL_BUFFERS(buf)
    {
	mfp = buf->b_ml.ml_mfp;
	ఒకవేళ (mfp != ఖాళీ)
	{
	    // If no swap file yet, may open one
	    ఒకవేళ (mfp->mf_fd < 0 && buf->b_may_మార్పిడి)
		ml_open_file(buf);

	    // only if there is a swapfile
	    ఒకవేళ (mfp->mf_fd >= 0)
	    {
		కొరకు (hp = mfp->mf_వాడిన_చివరి; hp != ఖాళీ; )
		{
		    ఒకవేళ (!(hp->bh_flags & BH_LOCKED)
			    && (!(hp->bh_flags & BH_DIRTY)
				|| mf_write(mfp, hp) != FAIL))
		    {
			mf_rem_వాడిన(mfp, hp);
			mf_rem_hash(mfp, hp);
			mf_free_bhdr(hp);
			hp = mfp->mf_వాడిన_చివరి;	// re-start, list was changed
			retval = TRUE;
		    }
		    లేకపోతే
			hp = hp->bh_prev;
		}
	    }
	}
    }
    తిరిగి retval;
}

/*
 * Allocate a block header and a block of memory for it.
 */
    స్థిర bhdr_T *
mf_alloc_bhdr(memfile_T *mfp, పూర్ణాంకం పుట_లెక్క)
{
    bhdr_T	*hp;

    ఒకవేళ ((hp = ALLOC_ONE(bhdr_T)) == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ ((hp->bh_దత్తాంశం = alloc((size_t)mfp->mf_పుట_కొలత * పుట_లెక్క)) == ఖాళీ)
    {
	vim_free(hp);	    // not enough memory
	తిరిగి ఖాళీ;
    }
    hp->bh_పుట_లెక్క = పుట_లెక్క;
    తిరిగి hp;
}

/*
 * Free a block header and the block of memory for it.
 */
    స్థిర శూన్య
mf_free_bhdr(bhdr_T *hp)
{
    vim_free(hp->bh_దత్తాంశం);
    vim_free(hp);
}

/*
 * Insert entry *hp in the free list.
 */
    స్థిర శూన్య
mf_ins_free(memfile_T *mfp, bhdr_T *hp)
{
    hp->bh_తదుపరి = mfp->mf_free_మొదటి;
    mfp->mf_free_మొదటి = hp;
}

/*
 * remove the first entry from the free list and return a pointer to it
 * Note: caller must check that mfp->mf_free_first is not NULL!
 */
    స్థిర bhdr_T *
mf_rem_free(memfile_T *mfp)
{
    bhdr_T	*hp;

    hp = mfp->mf_free_మొదటి;
    mfp->mf_free_మొదటి = hp->bh_తదుపరి;
    తిరిగి hp;
}

/*
 * read a block from disk
 *
 * Return FAIL for failure, OK otherwise
 */
    స్థిర పూర్ణాంకం
mf_read(memfile_T *mfp, bhdr_T *hp)
{
    off_T	స్థానభ్రంశం;
    గుర్తులేని	పుట_కొలత;
    గుర్తులేని	కొలత;

    ఒకవేళ (mfp->mf_fd < 0)	    // there is no file, can't read
	తిరిగి FAIL;

    పుట_కొలత = mfp->mf_పుట_కొలత;
    స్థానభ్రంశం = (off_T)పుట_కొలత * hp->bh_bnum;
    కొలత = పుట_కొలత * hp->bh_పుట_లెక్క;
    ఒకవేళ (vim_lseek(mfp->mf_fd, స్థానభ్రంశం, SEEK_SET) != స్థానభ్రంశం)
    {
	PERROR(_(e_seek_error_లోపల_మార్పిడి_file_read));
	తిరిగి FAIL;
    }
    ఒకవేళ ((గుర్తులేని)read_eintr(mfp->mf_fd, hp->bh_దత్తాంశం, కొలత) != కొలత)
    {
	PERROR(_(e_read_error_లోపల_మార్పిడి_file));
	తిరిగి FAIL;
    }

#నిర్వచితమైతే FEAT_CRYPT
    // Decrypt if 'key' is set and this is a data block. And when changing the
    // key.
    ఒకవేళ (*mfp->mf_బఫరు->b_p_కీలకం != NUL || mfp->mf_old_కీలకం != ఖాళీ)
	ml_decrypt_దత్తాంశం(mfp, hp->bh_దత్తాంశం, స్థానభ్రంశం, కొలత);
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * write a block to disk
 *
 * Return FAIL for failure, OK otherwise
 */
    స్థిర పూర్ణాంకం
mf_write(memfile_T *mfp, bhdr_T *hp)
{
    off_T	స్థానభ్రంశం;	    // offset in the file
    blocknr_T	nr;	    // block nr which is being written
    bhdr_T	*hp2;
    గుర్తులేని	పుట_కొలత;  // number of bytes in a page
    గుర్తులేని	పుట_లెక్క; // number of pages written
    గుర్తులేని	కొలత;	    // number of bytes written

    ఒకవేళ (mfp->mf_fd < 0 && !mfp->mf_reopen)
	// there is no file and there was no file, can't write
	తిరిగి FAIL;

    ఒకవేళ (hp->bh_bnum < 0)	// must assign file block number
	ఒకవేళ (mf_trans_కూడు(mfp, hp) == FAIL)
	    తిరిగి FAIL;

    పుట_కొలత = mfp->mf_పుట_కొలత;

    /*
     * We don't want gaps in the file. Write the blocks in front of *hp
     * to extend the file.
     * If block 'mf_infile_count' is not in the hash list, it has been
     * freed. Fill the space in the file with data from the current block.
     */
    కొరకు (;;)
    {
	పూర్ణాంకం attempt;

	nr = hp->bh_bnum;
	ఒకవేళ (nr > mfp->mf_infile_లెక్క)		// beyond end of file
	{
	    nr = mfp->mf_infile_లెక్క;
	    hp2 = mf_కనుగొను_hash(mfp, nr);	// NULL caught below
	}
	లేకపోతే
	    hp2 = hp;

	స్థానభ్రంశం = (off_T)పుట_కొలత * nr;
	ఒకవేళ (hp2 == ఖాళీ)	    // freed block, fill with dummy data
	    పుట_లెక్క = 1;
	లేకపోతే
	    పుట_లెక్క = hp2->bh_పుట_లెక్క;
	కొలత = పుట_కొలత * పుట_లెక్క;

	కొరకు (attempt = 1; attempt <= 2; ++attempt)
	{
	    ఒకవేళ (mfp->mf_fd >= 0)
	    {
		ఒకవేళ (vim_lseek(mfp->mf_fd, స్థానభ్రంశం, SEEK_SET) != స్థానభ్రంశం)
		{
		    PERROR(_(e_seek_error_లోపల_మార్పిడి_file_write));
		    తిరిగి FAIL;
		}
		ఒకవేళ (mf_write_ఖండం(mfp,
				   hp2 == ఖాళీ ? hp : hp2, స్థానభ్రంశం, కొలత) == OK)
		    ఆపు;
	    }

	    ఒకవేళ (attempt == 1)
	    {
		// If the swap file is on a network drive, and the network
		// gets disconnected and then re-connected, we can maybe fix it
		// by closing and then re-opening the file.
		ఒకవేళ (mfp->mf_fd >= 0)
		    close(mfp->mf_fd);
		mfp->mf_fd = mch_open_rw((అక్షరం *)mfp->mf_fname, mfp->mf_flags);
		mfp->mf_reopen = (mfp->mf_fd < 0);
	    }
	    ఒకవేళ (attempt == 2 || mfp->mf_fd < 0)
	    {
		// Avoid repeating the error message, this mostly happens when
		// the disk is full. We give the message again only after a
		// successful write or when hitting a key. We keep on trying,
		// in case some space becomes available.
		ఒకవేళ (!did_swapwrite_సందేశం)
		    emsg(_(e_write_error_లోపల_మార్పిడి_file));
		did_swapwrite_సందేశం = TRUE;
		తిరిగి FAIL;
	    }
	}

	did_swapwrite_సందేశం = FALSE;
	ఒకవేళ (hp2 != ఖాళీ)		    // written a non-dummy block
	    hp2->bh_flags &= ~BH_DIRTY;
					    // appended to the file
	ఒకవేళ (nr + (blocknr_T)పుట_లెక్క > mfp->mf_infile_లెక్క)
	    mfp->mf_infile_లెక్క = nr + పుట_లెక్క;
	ఒకవేళ (nr == hp->bh_bnum)		    // written the desired block
	    ఆపు;
    }
    తిరిగి OK;
}

/*
 * Write block "hp" with data size "size" to file "mfp->mf_fd".
 * Takes care of encryption.
 * Return FAIL or OK.
 */
    స్థిర పూర్ణాంకం
mf_write_ఖండం(
    memfile_T	*mfp,
    bhdr_T	*hp,
    off_T	స్థానభ్రంశం వాడని,
    గుర్తులేని	కొలత)
{
    char_u	*దత్తాంశం = hp->bh_దత్తాంశం;
    పూర్ణాంకం		ఫలితం = OK;

#నిర్వచితమైతే FEAT_CRYPT
    // Encrypt if 'key' is set and this is a data block.
    ఒకవేళ (*mfp->mf_బఫరు->b_p_కీలకం != NUL)
    {
	దత్తాంశం = ml_encrypt_దత్తాంశం(mfp, దత్తాంశం, స్థానభ్రంశం, కొలత);
	ఒకవేళ (దత్తాంశం == ఖాళీ)
	    తిరిగి FAIL;
    }
#అయితే_ముగింపు

    ఒకవేళ ((గుర్తులేని)write_eintr(mfp->mf_fd, దత్తాంశం, కొలత) != కొలత)
	ఫలితం = FAIL;

#నిర్వచితమైతే FEAT_CRYPT
    ఒకవేళ (దత్తాంశం != hp->bh_దత్తాంశం)
	vim_free(దత్తాంశం);
#అయితే_ముగింపు

    తిరిగి ఫలితం;
}

/*
 * Make block number for *hp positive and add it to the translation list
 *
 * Return FAIL for failure, OK otherwise
 */
    స్థిర పూర్ణాంకం
mf_trans_కూడు(memfile_T *mfp, bhdr_T *hp)
{
    bhdr_T	*freep;
    blocknr_T	new_bnum;
    NR_TRANS	*np;
    పూర్ణాంకం		పుట_లెక్క;

    ఒకవేళ (hp->bh_bnum >= 0)		    // it's already positive
	తిరిగి OK;

    ఒకవేళ ((np = ALLOC_ONE(NR_TRANS)) == ఖాళీ)
	తిరిగి FAIL;

/*
 * Get a new number for the block.
 * If the first item in the free list has sufficient pages, use its number
 * Otherwise use mf_blocknr_max.
 */
    freep = mfp->mf_free_మొదటి;
    పుట_లెక్క = hp->bh_పుట_లెక్క;
    ఒకవేళ (freep != ఖాళీ && freep->bh_పుట_లెక్క >= పుట_లెక్క)
    {
	new_bnum = freep->bh_bnum;
	/*
	 * If the page count of the free block was larger, reduce it.
	 * If the page count matches, remove the block from the free list
	 */
	ఒకవేళ (freep->bh_పుట_లెక్క > పుట_లెక్క)
	{
	    freep->bh_bnum += పుట_లెక్క;
	    freep->bh_పుట_లెక్క -= పుట_లెక్క;
	}
	లేకపోతే
	{
	    freep = mf_rem_free(mfp);
	    vim_free(freep);
	}
    }
    లేకపోతే
    {
	new_bnum = mfp->mf_blocknr_max;
	mfp->mf_blocknr_max += పుట_లెక్క;
    }

    np->nt_old_bnum = hp->bh_bnum;	    // adjust number
    np->nt_new_bnum = new_bnum;

    mf_rem_hash(mfp, hp);		    // remove from old hash list
    hp->bh_bnum = new_bnum;
    mf_ins_hash(mfp, hp);		    // insert in new hash list

    // Insert "np" into "mf_trans" hashtable with key "np->nt_old_bnum"
    mf_hash_కూడు_అంశం(&mfp->mf_trans, (mf_hashitem_T *)np);

    తిరిగి OK;
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

    np = (NR_TRANS *)mf_hash_కనుగొను(&mfp->mf_trans, old_nr);

    ఒకవేళ (np == ఖాళీ)		// not found
	తిరిగి old_nr;

    mfp->mf_neg_లెక్క--;
    new_bnum = np->nt_new_bnum;

    // remove entry from the trans list
    mf_hash_rem_అంశం(&mfp->mf_trans, (mf_hashitem_T *)np);

    vim_free(np);

    తిరిగి new_bnum;
}

/*
 * Set mfp->mf_ffname according to mfp->mf_fname and some other things.
 * Only called when creating or renaming the swapfile.	Either way it's a new
 * name so we must work out the full path name.
 */
    శూన్య
mf_సమితి_ffname(memfile_T *mfp)
{
    mfp->mf_ffname = నిండిన_Name_save(mfp->mf_fname, FALSE);
}

/*
 * Make the name of the file used for the memfile a full path.
 * Used before doing a :cd
 */
    శూన్య
mf_fullname(memfile_T *mfp)
{
    ఒకవేళ (mfp == ఖాళీ || mfp->mf_fname == ఖాళీ || mfp->mf_ffname == ఖాళీ)
	తిరిగి;

    vim_free(mfp->mf_fname);
    mfp->mf_fname = mfp->mf_ffname;
    mfp->mf_ffname = ఖాళీ;
}

/*
 * return TRUE if there are any translations pending for 'mfp'
 */
    పూర్ణాంకం
mf_need_trans(memfile_T *mfp)
{
    తిరిగి (mfp->mf_fname != ఖాళీ && mfp->mf_neg_లెక్క > 0);
}

/*
 * Open a swap file for a memfile.
 * The "fname" must be in allocated memory, and is consumed (also when an
 * error occurs).
 */
    స్థిర శూన్య
mf_do_open(
    memfile_T	*mfp,
    char_u	*fname,
    పూర్ణాంకం		flags)		// flags for open()
{
#నిర్వచితమైతే HAVE_LSTAT
    stat_T	sb;
#అయితే_ముగింపు

    mfp->mf_fname = fname;

    /*
     * Get the full path name before the open, because this is
     * not possible after the open on the Amiga.
     * fname cannot be NameBuff, because it must have been allocated.
     */
    mf_సమితి_ffname(mfp);
#ఒకవేళ defined(MSWIN)
    /*
     * A ":!cd e:xxx" may change the directory without us knowing, use the
     * full pathname always.  Careful: This frees fname!
     */
    mf_fullname(mfp);
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_LSTAT
    /*
     * Extra security check: When creating a swap file it really shouldn't
     * exist yet.  If there is a symbolic link, this is most likely an attack.
     */
    ఒకవేళ ((flags & O_CREAT) && mch_lstat((అక్షరం *)mfp->mf_fname, &sb) >= 0)
    {
	mfp->mf_fd = -1;
	emsg(_(e_మార్పిడి_file_already_exists_symlink_attack));
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	/*
	 * try to open the file
	 */
	flags |= O_EXTRA | O_NOFOLLOW;
#నిర్వచితమైతే MSWIN
	// Prevent handle inheritance that cause problems with Cscope
	// (swap file may not be deleted if cscope connection was open after
	// the file)
	flags |= O_NOINHERIT;
#అయితే_ముగింపు
	mfp->mf_flags = flags;
	mfp->mf_fd = mch_open_rw((అక్షరం *)mfp->mf_fname, flags);
    }

    /*
     * If the file cannot be opened, use memory only
     */
    ఒకవేళ (mfp->mf_fd < 0)
    {
	VIM_CLEAR(mfp->mf_fname);
	VIM_CLEAR(mfp->mf_ffname);
    }
    లేకపోతే
    {
#నిర్వచితమైతే HAVE_FD_CLOEXEC
	పూర్ణాంకం fdflags = fcntl(mfp->mf_fd, F_GETFD);
	ఒకవేళ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (శూన్య)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
#అయితే_ముగింపు
#ఒకవేళ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
	mch_copy_sec(fname, mfp->mf_fname);
#అయితే_ముగింపు
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
#నిర్వచించు MHT_LOG_ఎక్కించు_గుణకం 6
#నిర్వచించు MHT_GROWTH_గుణకం   2   // must be a power of two

/*
 * Initialize an empty hash table.
 */
    స్థిర శూన్య
mf_hash_init(mf_hashtab_T *mht)
{
    CLEAR_సూచిక(mht);
    mht->mht_buckets = mht->mht_small_buckets;
    mht->mht_ముసుగు = MHT_INIT_కొలత - 1;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * The hash table must not be used again without another mf_hash_init() call.
 */
    స్థిర శూన్య
mf_hash_free(mf_hashtab_T *mht)
{
    ఒకవేళ (mht->mht_buckets != mht->mht_small_buckets)
	vim_free(mht->mht_buckets);
}

/*
 * Free the array of a hash table and all the items it contains.
 */
    స్థిర శూన్య
mf_hash_free_all(mf_hashtab_T *mht)
{
    long_u	    idx;
    mf_hashitem_T   *mhi;
    mf_hashitem_T   *తదుపరి;

    కొరకు (idx = 0; idx <= mht->mht_ముసుగు; idx++)
	కొరకు (mhi = mht->mht_buckets[idx]; mhi != ఖాళీ; mhi = తదుపరి)
	{
	    తదుపరి = mhi->mhi_తదుపరి;
	    vim_free(mhi);
	}

    mf_hash_free(mht);
}

/*
 * Find "key" in hashtable "mht".
 * Returns a pointer to a mf_hashitem_T or NULL if the item was not found.
 */
    స్థిర mf_hashitem_T *
mf_hash_కనుగొను(mf_hashtab_T *mht, blocknr_T కీలకం)
{
    mf_hashitem_T   *mhi;

    mhi = mht->mht_buckets[కీలకం & mht->mht_ముసుగు];
    అయితే (mhi != ఖాళీ && mhi->mhi_కీలకం != కీలకం)
	mhi = mhi->mhi_తదుపరి;

    తిరిగి mhi;
}

/*
 * Add item "mhi" to hashtable "mht".
 * "mhi" must not be NULL.
 */
    స్థిర శూన్య
mf_hash_కూడు_అంశం(mf_hashtab_T *mht, mf_hashitem_T *mhi)
{
    long_u	    idx;

    idx = mhi->mhi_కీలకం & mht->mht_ముసుగు;
    mhi->mhi_తదుపరి = mht->mht_buckets[idx];
    mhi->mhi_prev = ఖాళీ;
    ఒకవేళ (mhi->mhi_తదుపరి != ఖాళీ)
	mhi->mhi_తదుపరి->mhi_prev = mhi;
    mht->mht_buckets[idx] = mhi;

    mht->mht_లెక్క++;

    /*
     * Grow hashtable when we have more thank 2^MHT_LOG_LOAD_FACTOR
     * items per bucket on average
     */
    ఒకవేళ (mht->mht_fixed == 0
	&& (mht->mht_లెక్క >> MHT_LOG_ఎక్కించు_గుణకం) > mht->mht_ముసుగు)
    {
	ఒకవేళ (mf_hash_grow(mht) == FAIL)
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
    స్థిర శూన్య
mf_hash_rem_అంశం(mf_hashtab_T *mht, mf_hashitem_T *mhi)
{
    ఒకవేళ (mhi->mhi_prev == ఖాళీ)
	mht->mht_buckets[mhi->mhi_కీలకం & mht->mht_ముసుగు] = mhi->mhi_తదుపరి;
    లేకపోతే
	mhi->mhi_prev->mhi_తదుపరి = mhi->mhi_తదుపరి;

    ఒకవేళ (mhi->mhi_తదుపరి != ఖాళీ)
	mhi->mhi_తదుపరి->mhi_prev = mhi->mhi_prev;

    mht->mht_లెక్క--;

    // We could shrink the table here, but it typically takes little memory,
    // so why bother?
}

/*
 * Increase number of buckets in the hashtable by MHT_GROWTH_FACTOR and
 * rehash items.
 * Returns FAIL when out of memory.
 */
    స్థిర పూర్ణాంకం
mf_hash_grow(mf_hashtab_T *mht)
{
    long_u	    i, j;
    పూర్ణాంకం		    జరుపు;
    mf_hashitem_T   *mhi;
    mf_hashitem_T   *tails[MHT_GROWTH_గుణకం];
    mf_hashitem_T   **buckets;
    size_t	    కొలత;

    కొలత = (mht->mht_ముసుగు + 1) * MHT_GROWTH_గుణకం * పరిమాణం(శూన్య *);
    buckets = lalloc_clear(కొలత, FALSE);
    ఒకవేళ (buckets == ఖాళీ)
	తిరిగి FAIL;

    జరుపు = 0;
    అయితే ((mht->mht_ముసుగు >> జరుపు) != 0)
	జరుపు++;

    కొరకు (i = 0; i <= mht->mht_ముసుగు; i++)
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

	CLEAR_క్షేత్రం(tails);

	కొరకు (mhi = mht->mht_buckets[i]; mhi != ఖాళీ; mhi = mhi->mhi_తదుపరి)
	{
	    j = (mhi->mhi_కీలకం >> జరుపు) & (MHT_GROWTH_గుణకం - 1);
	    ఒకవేళ (tails[j] == ఖాళీ)
	    {
		buckets[i + (j << జరుపు)] = mhi;
		tails[j] = mhi;
		mhi->mhi_prev = ఖాళీ;
	    }
	    లేకపోతే
	    {
		tails[j]->mhi_తదుపరి = mhi;
		mhi->mhi_prev = tails[j];
		tails[j] = mhi;
	    }
	}

	కొరకు (j = 0; j < MHT_GROWTH_గుణకం; j++)
	    ఒకవేళ (tails[j] != ఖాళీ)
		tails[j]->mhi_తదుపరి = ఖాళీ;
    }

    ఒకవేళ (mht->mht_buckets != mht->mht_small_buckets)
	vim_free(mht->mht_buckets);

    mht->mht_buckets = buckets;
    mht->mht_ముసుగు = (mht->mht_ముసుగు + 1) * MHT_GROWTH_గుణకం - 1;

    తిరిగి OK;
}
