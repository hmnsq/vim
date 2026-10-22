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

#ಅನ್ತರ್ಭಾವ "vim.h"

/*
 * Some systems have the page size in statfs.f_bsize, some in stat.st_blksize
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ST_BLKSIZE
# define STATFS stat
# define F_BSIZE st_blksize
# define fstatfs(fd, buf, len, nul) mch_fstat((fd), (buf))
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef HAVE_SYS_STATFS_H
#  include <sys/statfs.h>
#  define STATFS statfs
#  define F_BSIZE f_bsize
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * for Amiga Dos 2.0x we use Flush
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
# ifdef FEAT_ARP
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ dos2;			// this is in os_amiga.c
# endif
# ifdef SASC
#  include <ios1.h>			// for chkufb()
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು MEMFILE_ಪುಟ_ಗಾತ್ರ 4096		// default page size

ಸ್ಥಿರ long_u	ಒಟ್ಟು_mem_ಬಳಸಿದ = 0;	// total memory used for memfiles

ಸ್ಥಿರ ಶೂನ್ಯ mf_ins_hash(memfile_T *, bhdr_T *);
ಸ್ಥಿರ ಶೂನ್ಯ mf_rem_hash(memfile_T *, bhdr_T *);
ಸ್ಥಿರ bhdr_T *mf_ಕಂಡುಹಿಡಿ_hash(memfile_T *, blocknr_T);
ಸ್ಥಿರ ಶೂನ್ಯ mf_ins_ಬಳಸಿದ(memfile_T *, bhdr_T *);
ಸ್ಥಿರ ಶೂನ್ಯ mf_rem_ಬಳಸಿದ(memfile_T *, bhdr_T *);
ಸ್ಥಿರ bhdr_T *mf_release(memfile_T *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ bhdr_T *mf_alloc_bhdr(memfile_T *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ mf_free_bhdr(bhdr_T *);
ಸ್ಥಿರ ಶೂನ್ಯ mf_ins_free(memfile_T *, bhdr_T *);
ಸ್ಥಿರ bhdr_T *mf_rem_free(memfile_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  mf_read(memfile_T *, bhdr_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  mf_write(memfile_T *, bhdr_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  mf_write_ಖಂಡ(memfile_T *mfp, bhdr_T *hp, off_T ಸ್ಥಾನಾಂತರ, ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  mf_trans_ಕೂಡಿಸು(memfile_T *, bhdr_T *);
ಸ್ಥಿರ ಶೂನ್ಯ mf_do_open(memfile_T *, char_u *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಶೂನ್ಯ mf_hash_init(mf_hashtab_T *);
ಸ್ಥಿರ ಶೂನ್ಯ mf_hash_free(mf_hashtab_T *);
ಸ್ಥಿರ ಶೂನ್ಯ mf_hash_free_all(mf_hashtab_T *);
ಸ್ಥಿರ mf_hashitem_T *mf_hash_ಕಂಡುಹಿಡಿ(mf_hashtab_T *, blocknr_T);
ಸ್ಥಿರ ಶೂನ್ಯ mf_hash_ಕೂಡಿಸು_ಅಂಶ(mf_hashtab_T *, mf_hashitem_T *);
ಸ್ಥಿರ ಶೂನ್ಯ mf_hash_rem_ಅಂಶ(mf_hashtab_T *, mf_hashitem_T *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mf_hash_grow(mf_hashtab_T *);

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
mf_open(char_u *fname, ಪೂರ್ಣಾಂಕ flags)
{
    memfile_T		*mfp;
    off_T		ಗಾತ್ರ_2;
#ಆದರೆ defined(STATFS) && defined(UNIX) && !defined(__QNX__) && !defined(__minix)
# define USE_FSTATFS
    ರಚನೆ STATFS	stf;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ ((mfp = ALLOC_ONE(memfile_T)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (fname == ಖಾಲಿ)	    // no file for this memfile, use memory only
    {
	mfp->mf_fname = ಖಾಲಿ;
	mfp->mf_ffname = ಖಾಲಿ;
	mfp->mf_fd = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	mf_do_open(mfp, fname, flags);	// try to open the file

	// if the file cannot be opened, return here
	ಆದರೆ (mfp->mf_fd < 0)
	{
	    vim_free(mfp);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    mfp->mf_free_ಮೊದಲ = ಖಾಲಿ;		// free list is empty
    mfp->mf_ಬಳಸಿದ_ಮೊದಲ = ಖಾಲಿ;		// used list is empty
    mfp->mf_ಬಳಸಿದ_ಕೊನೆಯ = ಖಾಲಿ;
    mfp->mf_dirty = MF_DIRTY_NO;
    mfp->mf_ಬಳಸಿದ_ಎಣಿಕೆ = 0;
    mf_hash_init(&mfp->mf_hash);
    mf_hash_init(&mfp->mf_trans);
    mfp->mf_ಪುಟ_ಗಾತ್ರ = MEMFILE_ಪುಟ_ಗಾತ್ರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    mfp->mf_old_ಕೀಲಿ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_FSTATFS
    /*
     * Try to set the page size equal to the block size of the device.
     * Speeds up I/O a lot.
     * When recovering, the actual block size will be retrieved from block 0
     * in ml_recover().  The size used here may be wrong, therefore
     * mf_blocknr_max must be rounded up.
     */
    ಆದರೆ (mfp->mf_fd >= 0
	    && fstatfs(mfp->mf_fd, &stf, ಗಾತ್ರ(ರಚನೆ statfs), 0) == 0
	    && stf.F_BSIZE >= MIN_ಅದಲುಬದಲು_ಪುಟ_ಗಾತ್ರ
	    && stf.F_BSIZE <= MAX_ಅದಲುಬದಲು_ಪುಟ_ಗಾತ್ರ)
	mfp->mf_ಪುಟ_ಗಾತ್ರ = stf.F_BSIZE;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (mfp->mf_fd < 0 || (flags & (O_TRUNC|O_EXCL))
		  || (ಗಾತ್ರ_2 = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	mfp->mf_blocknr_max = 0;	// no file or empty file
    ಇಲ್ಲದಿದ್ದರೆ
	mfp->mf_blocknr_max = (blocknr_T)((ಗಾತ್ರ_2 + mfp->mf_ಪುಟ_ಗಾತ್ರ - 1)
							 / mfp->mf_ಪುಟ_ಗಾತ್ರ);
    mfp->mf_blocknr_min = -1;
    mfp->mf_neg_ಎಣಿಕೆ = 0;
    mfp->mf_infile_ಎಣಿಕೆ = mfp->mf_blocknr_max;

    /*
     * Compute maximum number of pages ('maxmem' is in Kbyte):
     *	'mammem' * 1Kbyte / page-size-in-bytes.
     * Avoid overflow by first reducing page size as much as possible.
     */
    {
	ಪೂರ್ಣಾಂಕ	    ಸರಿಸು = 10;
	ಚಿಹ್ನೆರಹಿತ    ಪುಟ_ಗಾತ್ರ = mfp->mf_ಪುಟ_ಗಾತ್ರ;

	ಆಗ (ಸರಿಸು > 0 && (ಪುಟ_ಗಾತ್ರ & 1) == 0)
	{
	    ಪುಟ_ಗಾತ್ರ = ಪುಟ_ಗಾತ್ರ >> 1;
	    --ಸರಿಸು;
	}
	mfp->mf_ಬಳಸಿದ_ಎಣಿಕೆ_max = (p_mm << ಸರಿಸು) / ಪುಟ_ಗಾತ್ರ;
	ಆದರೆ (mfp->mf_ಬಳಸಿದ_ಎಣಿಕೆ_max < 10)
	    mfp->mf_ಬಳಸಿದ_ಎಣಿಕೆ_max = 10;
    }

    ಹಿಂತಿರುಗಿ mfp;
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
    ಪೂರ್ಣಾಂಕ
mf_open_file(memfile_T *mfp, char_u *fname)
{
    mf_do_open(mfp, fname, O_RDWR|O_CREAT|O_EXCL); // try to open the file

    ಆದರೆ (mfp->mf_fd < 0)
	ಹಿಂತಿರುಗಿ FAIL;

    mfp->mf_dirty = MF_DIRTY_YES;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Close a memory file and delete the associated file if 'del_file' is TRUE.
 */
    ಶೂನ್ಯ
mf_close(memfile_T *mfp, ಪೂರ್ಣಾಂಕ del_file)
{
    bhdr_T	*hp, *nextp;

    ಆದರೆ (mfp == ಖಾಲಿ)		    // safety check
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (mfp->mf_fd >= 0)
    {
	ಆದರೆ (close(mfp->mf_fd) < 0)
	    emsg(_(e_close_error_on_ಅದಲುಬದಲು_file));
    }
    ಆದರೆ (del_file && mfp->mf_fname != ಖಾಲಿ)
	mch_remove(mfp->mf_fname);
					    // free entries in used list
    ಗಾಗಿ (hp = mfp->mf_ಬಳಸಿದ_ಮೊದಲ; hp != ಖಾಲಿ; hp = nextp)
    {
	ಒಟ್ಟು_mem_ಬಳಸಿದ -= (long_u)hp->bh_ಪುಟ_ಎಣಿಕೆ * mfp->mf_ಪುಟ_ಗಾತ್ರ;
	nextp = hp->bh_ಮುಂದಿನ;
	mf_free_bhdr(hp);
    }
    ಆಗ (mfp->mf_free_ಮೊದಲ != ಖಾಲಿ)	    // free entries in free list
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
    ಶೂನ್ಯ
mf_close_file(
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		getlines)	// get all lines into memory?
{
    memfile_T	*mfp;
    linenr_T	lnum;

    mfp = buf->b_ml.ml_mfp;
    ಆದರೆ (mfp == ಖಾಲಿ || mfp->mf_fd < 0)		// nothing to close
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (getlines)
    {
	// get all blocks in memory by accessing all lines (clumsy!)
	mf_dont_release = TRUE;
	ಗಾಗಿ (lnum = 1; lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
	    (ಶೂನ್ಯ)ml_get_buf(buf, lnum, FALSE);
	mf_dont_release = FALSE;
	// TODO: should check if all blocks are really in core
    }

    ಆದರೆ (close(mfp->mf_fd) < 0)			// close the file
	emsg(_(e_close_error_on_ಅದಲುಬದಲು_file));
    mfp->mf_fd = -1;

    ಆದರೆ (mfp->mf_fname != ಖಾಲಿ)
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
    ಶೂನ್ಯ
mf_new_ಪುಟ_ಗಾತ್ರ(memfile_T *mfp, ಚಿಹ್ನೆರಹಿತ new_ಗಾತ್ರ)
{
    // Correct the memory used for block 0 to the new size, because it will be
    // freed with that size later on.
    ಒಟ್ಟು_mem_ಬಳಸಿದ += new_ಗಾತ್ರ - mfp->mf_ಪುಟ_ಗಾತ್ರ;
    mfp->mf_ಪುಟ_ಗಾತ್ರ = new_ಗಾತ್ರ;
}

/*
 * get a new block
 *
 *   negative: TRUE if negative block number desired (data block)
 */
    bhdr_T *
mf_new(memfile_T *mfp, ಪೂರ್ಣಾಂಕ negative, ಪೂರ್ಣಾಂಕ ಪುಟ_ಎಣಿಕೆ)
{
    bhdr_T	*hp;	// new bhdr_T
    bhdr_T	*freep;	// first block in free list
    char_u	*p;

    /*
     * If we reached the maximum size for the used memory blocks, release one
     * If a bhdr_T is returned, use it and adjust the page_count if necessary.
     */
    hp = mf_release(mfp, ಪುಟ_ಎಣಿಕೆ);

/*
 * Decide on the number to use:
 * If there is a free block, use its number.
 * Otherwise use mf_block_min for a negative number, mf_block_max for
 * a positive number.
 */
    freep = mfp->mf_free_ಮೊದಲ;
    ಆದರೆ (!negative && freep != ಖಾಲಿ && freep->bh_ಪುಟ_ಎಣಿಕೆ >= ಪುಟ_ಎಣಿಕೆ)
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
	ಆದರೆ (freep->bh_ಪುಟ_ಎಣಿಕೆ > ಪುಟ_ಎಣಿಕೆ)
	{
	    ಆದರೆ (hp == ಖಾಲಿ && (hp = mf_alloc_bhdr(mfp, ಪುಟ_ಎಣಿಕೆ)) == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    hp->bh_bnum = freep->bh_bnum;
	    freep->bh_bnum += ಪುಟ_ಎಣಿಕೆ;
	    freep->bh_ಪುಟ_ಎಣಿಕೆ -= ಪುಟ_ಎಣಿಕೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hp == ಖಾಲಿ)	    // need to allocate memory for this block
	{
	    ಆದರೆ ((p = alloc((size_t)mfp->mf_ಪುಟ_ಗಾತ್ರ * ಪುಟ_ಎಣಿಕೆ)) == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    hp = mf_rem_free(mfp);
	    hp->bh_ದತ್ತಾಂಶ = p;
	}
	ಇಲ್ಲದಿದ್ದರೆ		    // use the number, remove entry from free list
	{
	    freep = mf_rem_free(mfp);
	    hp->bh_bnum = freep->bh_bnum;
	    vim_free(freep);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ	// get a new number
    {
	ಆದರೆ (hp == ಖಾಲಿ && (hp = mf_alloc_bhdr(mfp, ಪುಟ_ಎಣಿಕೆ)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (negative)
	{
	    hp->bh_bnum = mfp->mf_blocknr_min--;
	    mfp->mf_neg_ಎಣಿಕೆ++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    hp->bh_bnum = mfp->mf_blocknr_max;
	    mfp->mf_blocknr_max += ಪುಟ_ಎಣಿಕೆ;
	}
    }
    hp->bh_flags = BH_LOCKED | BH_DIRTY;	// new block is always dirty
    mfp->mf_dirty = MF_DIRTY_YES;
    hp->bh_ಪುಟ_ಎಣಿಕೆ = ಪುಟ_ಎಣಿಕೆ;
    mf_ins_ಬಳಸಿದ(mfp, hp);
    mf_ins_hash(mfp, hp);

    /*
     * Init the data to all zero, to avoid reading uninitialized data.
     * This also avoids that the passwd file ends up in the swap file!
     */
    (ಶೂನ್ಯ)vim_memset((ಅಕ್ಷರ *)(hp->bh_ದತ್ತಾಂಶ), 0,
				      (size_t)mfp->mf_ಪುಟ_ಗಾತ್ರ * ಪುಟ_ಎಣಿಕೆ);

    ಹಿಂತಿರುಗಿ hp;
}

/*
 * Get existing block "nr" with "page_count" pages.
 *
 * Note: The caller should first check a negative nr with mf_trans_del()
 */
    bhdr_T *
mf_get(memfile_T *mfp, blocknr_T nr, ಪೂರ್ಣಾಂಕ ಪುಟ_ಎಣಿಕೆ)
{
    bhdr_T    *hp;
						// doesn't exist
    ಆದರೆ (nr >= mfp->mf_blocknr_max || nr <= mfp->mf_blocknr_min)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    /*
     * see if it is in the cache
     */
    hp = mf_ಕಂಡುಹಿಡಿ_hash(mfp, nr);
    ಆದರೆ (hp == ಖಾಲಿ)	// not in the hash list
    {
	ಆದರೆ (nr < 0 || nr >= mfp->mf_infile_ಎಣಿಕೆ)   // can't be in the file
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	// could check here if the block is in the free list

	/*
	 * Check if we need to flush an existing block.
	 * If so, use that block.
	 * If not, allocate a new block.
	 */
	hp = mf_release(mfp, ಪುಟ_ಎಣಿಕೆ);
	ಆದರೆ (hp == ಖಾಲಿ && ಪುಟ_ಎಣಿಕೆ > 0)
	    hp = mf_alloc_bhdr(mfp, ಪುಟ_ಎಣಿಕೆ);
	ಆದರೆ (hp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	hp->bh_bnum = nr;
	hp->bh_flags = 0;
	hp->bh_ಪುಟ_ಎಣಿಕೆ = ಪುಟ_ಎಣಿಕೆ;
	ಆದರೆ (mf_read(mfp, hp) == FAIL)	    // cannot read the block!
	{
	    mf_free_bhdr(hp);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	mf_rem_ಬಳಸಿದ(mfp, hp);	// remove from list, insert in front below
	mf_rem_hash(mfp, hp);
    }

    hp->bh_flags |= BH_LOCKED;
    mf_ins_ಬಳಸಿದ(mfp, hp);	// put in front of used list
    mf_ins_hash(mfp, hp);	// put in front of hash list

    ಹಿಂತಿರುಗಿ hp;
}

/*
 * release the block *hp
 *
 *   dirty: Block must be written to file later
 *   infile: Block should be in file (needed for recovery)
 *
 *  no return value, function cannot fail
 */
    ಶೂನ್ಯ
mf_put(
    memfile_T	*mfp,
    bhdr_T	*hp,
    ಪೂರ್ಣಾಂಕ		dirty,
    ಪೂರ್ಣಾಂಕ		infile)
{
    ಪೂರ್ಣಾಂಕ		flags;

    flags = hp->bh_flags;

    ಆದರೆ ((flags & BH_LOCKED) == 0)
	iemsg(e_ಖಂಡ_was_not_locked);
    flags &= ~BH_LOCKED;
    ಆದರೆ (dirty)
    {
	flags |= BH_DIRTY;
	ಆದರೆ (mfp->mf_dirty != MF_DIRTY_YES_NOSYNC)
	    mfp->mf_dirty = MF_DIRTY_YES;
    }
    hp->bh_flags = flags;
    ಆದರೆ (infile)
	mf_trans_ಕೂಡಿಸು(mfp, hp);	    // may translate negative in positive nr
}

/*
 * block *hp is no longer in used, may put it in the free list of memfile *mfp
 */
    ಶೂನ್ಯ
mf_free(memfile_T *mfp, bhdr_T *hp)
{
    vim_free(hp->bh_ದತ್ತಾಂಶ);	// free the memory
    mf_rem_hash(mfp, hp);	// get *hp out of the hash list
    mf_rem_ಬಳಸಿದ(mfp, hp);	// get *hp out of the used list
    ಆದರೆ (hp->bh_bnum < 0)
    {
	vim_free(hp);		// don't want negative numbers in free list
	mfp->mf_neg_ಎಣಿಕೆ--;
    }
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಪೂರ್ಣಾಂಕ
mf_sync(memfile_T *mfp, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
    bhdr_T	*hp;
    ಪೂರ್ಣಾಂಕ		got_int_save = got_int;

    ಆದರೆ (mfp->mf_fd < 0)
    {
	// there is no file, nothing to do
	mfp->mf_dirty = MF_DIRTY_NO;
	ಹಿಂತಿರುಗಿ FAIL;
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
    ಸ್ಥಿತಿ_2 = OK;
    ಗಾಗಿ (hp = mfp->mf_ಬಳಸಿದ_ಕೊನೆಯ; hp != ಖಾಲಿ; hp = hp->bh_prev)
	ಆದರೆ (((flags & MFS_ALL) || hp->bh_bnum >= 0)
		&& (hp->bh_flags & BH_DIRTY)
		&& (ಸ್ಥಿತಿ_2 == OK || (hp->bh_bnum >= 0
		    && hp->bh_bnum < mfp->mf_infile_ಎಣಿಕೆ)))
	{
	    ಆದರೆ ((flags & MFS_ZERO) && hp->bh_bnum != 0)
		ಮುಂದುವರಿ;
	    ಆದರೆ (mf_write(mfp, hp) == FAIL)
	    {
		ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)	// double error: quit syncing
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ_2 = FAIL;
	    }
	    ಆದರೆ (flags & MFS_STOP)
	    {
		// Stop when char available now.
		ಆದರೆ (ui_char_avail())
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ui_breakcheck();
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	}

    /*
     * If the whole list is flushed, the memfile is not dirty anymore.
     * In case of an error this flag is also set, to avoid trying all the time.
     */
    ಆದರೆ (hp == ಖಾಲಿ || ಸ್ಥಿತಿ_2 == FAIL)
	mfp->mf_dirty = MF_DIRTY_NO;

    ಆದರೆ ((flags & MFS_FLUSH) && *p_sws != NUL)
    {
#ಆದರೆ defined(UNIX)
# ifdef HAVE_FSYNC
	/*
	 * most Unixes have the very useful fsync() function, just what we need.
	 */
	ಆದರೆ (STRCMP(p_sws, "fsync") == 0)
	{
	    ಆದರೆ (vim_fsync(mfp->mf_fd))
		ಸ್ಥಿತಿ_2 = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
# ifdef HAVE_SYNC
	    sync();
# ಇಲ್ಲದಿದ್ದರೆ
	    fflush(ಖಾಲಿ);
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
	ಆದರೆ (STRCMP(p_sws, "fsync") == 0)
	{
	    ಆದರೆ (vim_fsync(mfp->mf_fd))
		ಸ್ಥಿತಿ_2 = FAIL;
	}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಆದರೆ (_commit(mfp->mf_fd))
	    ಸ್ಥಿತಿ_2 = FAIL;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
# ಆದರೆ defined(__AROS__) || defined(__amigaos4__)
	ಆದರೆ (vim_fsync(mfp->mf_fd) != 0)
	    ಸ್ಥಿತಿ_2 = FAIL;
# ಇಲ್ಲದಿದ್ದರೆ
	/*
	 * Flush() only exists for AmigaDos 2.0.
	 * For 1.3 it should be done with close() + open(), but then the risk
	 * is that the open() may fail and lose the file....
	 */
#  ifdef FEAT_ARP
	ಆದರೆ (dos2)
#  endif
#  ifdef SASC
	{
	    ರಚನೆ UFB *fp = chkufb(mfp->mf_fd);

	    ಆದರೆ (fp != ಖಾಲಿ)
		Flush(fp->ufbfh);
	}
#  ಇಲ್ಲದಿದ್ದರೆ
#   ಆದರೆ defined(_DCC) || defined(__GNUC__) || defined(__MORPHOS__)
	{
#    ಆದರೆ defined(__GNUC__) && !defined(__MORPHOS__) && defined(__libnix__)
	    // Have function (in libnix at least),
	    // but ain't got no prototype anywhere.
	    ಬಾಹ್ಯ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ fdtofh(ಪೂರ್ಣಾಂಕ filedescriptor);
#    endif
#    ಆದರೆ !defined(__libnix__)
	    fflush(ಖಾಲಿ);
#    ಇಲ್ಲದಿದ್ದರೆ
	    BPTR fh = (BPTR)fdtofh(mfp->mf_fd);

	    ಆದರೆ (fh != 0)
		Flush(fh);
#    endif
	}
#   ಇಲ್ಲದಿದ್ದರೆ // assume Manx
	    Flush(_devtab[mfp->mf_fd].fd);
#   endif
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ // AMIGA
    }

    got_int |= got_int_save;

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * For all blocks in memory file *mfp that have a positive block number set
 * the dirty flag.  These are blocks that need to be written to a newly
 * created swapfile.
 */
    ಶೂನ್ಯ
mf_ಗಣ_dirty(memfile_T *mfp)
{
    bhdr_T	*hp;

    ಗಾಗಿ (hp = mfp->mf_ಬಳಸಿದ_ಕೊನೆಯ; hp != ಖಾಲಿ; hp = hp->bh_prev)
	ಆದರೆ (hp->bh_bnum > 0)
	    hp->bh_flags |= BH_DIRTY;
    mfp->mf_dirty = MF_DIRTY_YES;
}

/*
 * insert block *hp in front of hashlist of memfile *mfp
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_ins_hash(memfile_T *mfp, bhdr_T *hp)
{
    mf_hash_ಕೂಡಿಸು_ಅಂಶ(&mfp->mf_hash, (mf_hashitem_T *)hp);
}

/*
 * remove block *hp from hashlist of memfile list *mfp
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_rem_hash(memfile_T *mfp, bhdr_T *hp)
{
    mf_hash_rem_ಅಂಶ(&mfp->mf_hash, (mf_hashitem_T *)hp);
}

/*
 * look in hash lists of memfile *mfp for block header with number 'nr'
 */
    ಸ್ಥಿರ bhdr_T *
mf_ಕಂಡುಹಿಡಿ_hash(memfile_T *mfp, blocknr_T nr)
{
    ಹಿಂತಿರುಗಿ (bhdr_T *)mf_hash_ಕಂಡುಹಿಡಿ(&mfp->mf_hash, nr);
}

/*
 * insert block *hp in front of used list of memfile *mfp
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_ins_ಬಳಸಿದ(memfile_T *mfp, bhdr_T *hp)
{
    hp->bh_ಮುಂದಿನ = mfp->mf_ಬಳಸಿದ_ಮೊದಲ;
    mfp->mf_ಬಳಸಿದ_ಮೊದಲ = hp;
    hp->bh_prev = ಖಾಲಿ;
    ಆದರೆ (hp->bh_ಮುಂದಿನ == ಖಾಲಿ)	    // list was empty, adjust last pointer
	mfp->mf_ಬಳಸಿದ_ಕೊನೆಯ = hp;
    ಇಲ್ಲದಿದ್ದರೆ
	hp->bh_ಮುಂದಿನ->bh_prev = hp;
    mfp->mf_ಬಳಸಿದ_ಎಣಿಕೆ += hp->bh_ಪುಟ_ಎಣಿಕೆ;
    ಒಟ್ಟು_mem_ಬಳಸಿದ += (long_u)hp->bh_ಪುಟ_ಎಣಿಕೆ * mfp->mf_ಪುಟ_ಗಾತ್ರ;
}

/*
 * remove block *hp from used list of memfile *mfp
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_rem_ಬಳಸಿದ(memfile_T *mfp, bhdr_T *hp)
{
    ಆದರೆ (hp->bh_ಮುಂದಿನ == ಖಾಲಿ)	    // last block in used list
	mfp->mf_ಬಳಸಿದ_ಕೊನೆಯ = hp->bh_prev;
    ಇಲ್ಲದಿದ್ದರೆ
	hp->bh_ಮುಂದಿನ->bh_prev = hp->bh_prev;
    ಆದರೆ (hp->bh_prev == ಖಾಲಿ)	    // first block in used list
	mfp->mf_ಬಳಸಿದ_ಮೊದಲ = hp->bh_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	hp->bh_prev->bh_ಮುಂದಿನ = hp->bh_ಮುಂದಿನ;
    mfp->mf_ಬಳಸಿದ_ಎಣಿಕೆ -= hp->bh_ಪುಟ_ಎಣಿಕೆ;
    ಒಟ್ಟು_mem_ಬಳಸಿದ -= (long_u)hp->bh_ಪುಟ_ಎಣಿಕೆ * mfp->mf_ಪುಟ_ಗಾತ್ರ;
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
    ಸ್ಥಿರ bhdr_T *
mf_release(memfile_T *mfp, ಪೂರ್ಣಾಂಕ ಪುಟ_ಎಣಿಕೆ)
{
    bhdr_T	*hp;
    ಪೂರ್ಣಾಂಕ		need_release;
    buf_T	*buf;

    // don't release while in mf_close_file()
    ಆದರೆ (mf_dont_release)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    /*
     * Need to release a block if the number of blocks for this memfile is
     * higher than the maximum or total memory used is over 'maxmemtot'
     */
    need_release = ((mfp->mf_ಬಳಸಿದ_ಎಣಿಕೆ >= mfp->mf_ಬಳಸಿದ_ಎಣಿಕೆ_max)
				  || (ಒಟ್ಟು_mem_ಬಳಸಿದ >> 10) >= (long_u)p_mmt);

    /*
     * Try to create a swap file if the amount of memory used is getting too
     * high.
     */
    ಆದರೆ (mfp->mf_fd < 0 && need_release && p_uc)
    {
	// find for which buffer this memfile is
	FOR_ALL_BUFFERS(buf)
	    ಆದರೆ (buf->b_ml.ml_mfp == mfp)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (buf != ಖಾಲಿ && buf->b_may_ಅದಲುಬದಲು)
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
    ಆದರೆ (mfp->mf_fd < 0 || !need_release)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (hp = mfp->mf_ಬಳಸಿದ_ಕೊನೆಯ; hp != ಖಾಲಿ; hp = hp->bh_prev)
	ಆದರೆ (!(hp->bh_flags & BH_LOCKED))
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (hp == ಖಾಲಿ)	// not a single one that can be released
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    /*
     * If the block is dirty, write it.
     * If the write fails we don't free it.
     */
    ಆದರೆ ((hp->bh_flags & BH_DIRTY) && mf_write(mfp, hp) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    mf_rem_ಬಳಸಿದ(mfp, hp);
    mf_rem_hash(mfp, hp);

    /*
     * If a bhdr_T is returned, make sure that the page_count of bh_data is
     * right
     */
    ಆದರೆ (hp->bh_ಪುಟ_ಎಣಿಕೆ != ಪುಟ_ಎಣಿಕೆ)
    {
	VIM_CLEAR(hp->bh_ದತ್ತಾಂಶ);
	ಆದರೆ (ಪುಟ_ಎಣಿಕೆ > 0)
	    hp->bh_ದತ್ತಾಂಶ = alloc((size_t)mfp->mf_ಪುಟ_ಗಾತ್ರ * ಪುಟ_ಎಣಿಕೆ);
	ಆದರೆ (hp->bh_ದತ್ತಾಂಶ == ಖಾಲಿ)
	{
	    vim_free(hp);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	hp->bh_ಪುಟ_ಎಣಿಕೆ = ಪುಟ_ಎಣಿಕೆ;
    }
    ಹಿಂತಿರುಗಿ hp;
}

/*
 * release as many blocks as possible
 * Used in case of out of memory
 *
 * return TRUE if any memory was released
 */
    ಪೂರ್ಣಾಂಕ
mf_release_all(ಶೂನ್ಯ)
{
    buf_T	*buf;
    memfile_T	*mfp;
    bhdr_T	*hp;
    ಪೂರ್ಣಾಂಕ		retval = FALSE;

    FOR_ALL_BUFFERS(buf)
    {
	mfp = buf->b_ml.ml_mfp;
	ಆದರೆ (mfp != ಖಾಲಿ)
	{
	    // If no swap file yet, may open one
	    ಆದರೆ (mfp->mf_fd < 0 && buf->b_may_ಅದಲುಬದಲು)
		ml_open_file(buf);

	    // only if there is a swapfile
	    ಆದರೆ (mfp->mf_fd >= 0)
	    {
		ಗಾಗಿ (hp = mfp->mf_ಬಳಸಿದ_ಕೊನೆಯ; hp != ಖಾಲಿ; )
		{
		    ಆದರೆ (!(hp->bh_flags & BH_LOCKED)
			    && (!(hp->bh_flags & BH_DIRTY)
				|| mf_write(mfp, hp) != FAIL))
		    {
			mf_rem_ಬಳಸಿದ(mfp, hp);
			mf_rem_hash(mfp, hp);
			mf_free_bhdr(hp);
			hp = mfp->mf_ಬಳಸಿದ_ಕೊನೆಯ;	// re-start, list was changed
			retval = TRUE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			hp = hp->bh_prev;
		}
	    }
	}
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Allocate a block header and a block of memory for it.
 */
    ಸ್ಥಿರ bhdr_T *
mf_alloc_bhdr(memfile_T *mfp, ಪೂರ್ಣಾಂಕ ಪುಟ_ಎಣಿಕೆ)
{
    bhdr_T	*hp;

    ಆದರೆ ((hp = ALLOC_ONE(bhdr_T)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ ((hp->bh_ದತ್ತಾಂಶ = alloc((size_t)mfp->mf_ಪುಟ_ಗಾತ್ರ * ಪುಟ_ಎಣಿಕೆ)) == ಖಾಲಿ)
    {
	vim_free(hp);	    // not enough memory
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    hp->bh_ಪುಟ_ಎಣಿಕೆ = ಪುಟ_ಎಣಿಕೆ;
    ಹಿಂತಿರುಗಿ hp;
}

/*
 * Free a block header and the block of memory for it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_free_bhdr(bhdr_T *hp)
{
    vim_free(hp->bh_ದತ್ತಾಂಶ);
    vim_free(hp);
}

/*
 * Insert entry *hp in the free list.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_ins_free(memfile_T *mfp, bhdr_T *hp)
{
    hp->bh_ಮುಂದಿನ = mfp->mf_free_ಮೊದಲ;
    mfp->mf_free_ಮೊದಲ = hp;
}

/*
 * remove the first entry from the free list and return a pointer to it
 * Note: caller must check that mfp->mf_free_first is not NULL!
 */
    ಸ್ಥಿರ bhdr_T *
mf_rem_free(memfile_T *mfp)
{
    bhdr_T	*hp;

    hp = mfp->mf_free_ಮೊದಲ;
    mfp->mf_free_ಮೊದಲ = hp->bh_ಮುಂದಿನ;
    ಹಿಂತಿರುಗಿ hp;
}

/*
 * read a block from disk
 *
 * Return FAIL for failure, OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mf_read(memfile_T *mfp, bhdr_T *hp)
{
    off_T	ಸ್ಥಾನಾಂತರ;
    ಚಿಹ್ನೆರಹಿತ	ಪುಟ_ಗಾತ್ರ;
    ಚಿಹ್ನೆರಹಿತ	ಗಾತ್ರ_2;

    ಆದರೆ (mfp->mf_fd < 0)	    // there is no file, can't read
	ಹಿಂತಿರುಗಿ FAIL;

    ಪುಟ_ಗಾತ್ರ = mfp->mf_ಪುಟ_ಗಾತ್ರ;
    ಸ್ಥಾನಾಂತರ = (off_T)ಪುಟ_ಗಾತ್ರ * hp->bh_bnum;
    ಗಾತ್ರ_2 = ಪುಟ_ಗಾತ್ರ * hp->bh_ಪುಟ_ಎಣಿಕೆ;
    ಆದರೆ (vim_lseek(mfp->mf_fd, ಸ್ಥಾನಾಂತರ, SEEK_SET) != ಸ್ಥಾನಾಂತರ)
    {
	PERROR(_(e_seek_error_ಒಳ_ಅದಲುಬದಲು_file_read));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ)read_eintr(mfp->mf_fd, hp->bh_ದತ್ತಾಂಶ, ಗಾತ್ರ_2) != ಗಾತ್ರ_2)
    {
	PERROR(_(e_read_error_ಒಳ_ಅದಲುಬದಲು_file));
	ಹಿಂತಿರುಗಿ FAIL;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    // Decrypt if 'key' is set and this is a data block. And when changing the
    // key.
    ಆದರೆ (*mfp->mf_ಬಫರ್->b_p_ಕೀಲಿ != NUL || mfp->mf_old_ಕೀಲಿ != ಖಾಲಿ)
	ml_decrypt_ದತ್ತಾಂಶ(mfp, hp->bh_ದತ್ತಾಂಶ, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * write a block to disk
 *
 * Return FAIL for failure, OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mf_write(memfile_T *mfp, bhdr_T *hp)
{
    off_T	ಸ್ಥಾನಾಂತರ;	    // offset in the file
    blocknr_T	nr;	    // block nr which is being written
    bhdr_T	*hp2;
    ಚಿಹ್ನೆರಹಿತ	ಪುಟ_ಗಾತ್ರ;  // number of bytes in a page
    ಚಿಹ್ನೆರಹಿತ	ಪುಟ_ಎಣಿಕೆ; // number of pages written
    ಚಿಹ್ನೆರಹಿತ	ಗಾತ್ರ_2;	    // number of bytes written

    ಆದರೆ (mfp->mf_fd < 0 && !mfp->mf_reopen)
	// there is no file and there was no file, can't write
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (hp->bh_bnum < 0)	// must assign file block number
	ಆದರೆ (mf_trans_ಕೂಡಿಸು(mfp, hp) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

    ಪುಟ_ಗಾತ್ರ = mfp->mf_ಪುಟ_ಗಾತ್ರ;

    /*
     * We don't want gaps in the file. Write the blocks in front of *hp
     * to extend the file.
     * If block 'mf_infile_count' is not in the hash list, it has been
     * freed. Fill the space in the file with data from the current block.
     */
    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ attempt;

	nr = hp->bh_bnum;
	ಆದರೆ (nr > mfp->mf_infile_ಎಣಿಕೆ)		// beyond end of file
	{
	    nr = mfp->mf_infile_ಎಣಿಕೆ;
	    hp2 = mf_ಕಂಡುಹಿಡಿ_hash(mfp, nr);	// NULL caught below
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    hp2 = hp;

	ಸ್ಥಾನಾಂತರ = (off_T)ಪುಟ_ಗಾತ್ರ * nr;
	ಆದರೆ (hp2 == ಖಾಲಿ)	    // freed block, fill with dummy data
	    ಪುಟ_ಎಣಿಕೆ = 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪುಟ_ಎಣಿಕೆ = hp2->bh_ಪುಟ_ಎಣಿಕೆ;
	ಗಾತ್ರ_2 = ಪುಟ_ಗಾತ್ರ * ಪುಟ_ಎಣಿಕೆ;

	ಗಾಗಿ (attempt = 1; attempt <= 2; ++attempt)
	{
	    ಆದರೆ (mfp->mf_fd >= 0)
	    {
		ಆದರೆ (vim_lseek(mfp->mf_fd, ಸ್ಥಾನಾಂತರ, SEEK_SET) != ಸ್ಥಾನಾಂತರ)
		{
		    PERROR(_(e_seek_error_ಒಳ_ಅದಲುಬದಲು_file_write));
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಆದರೆ (mf_write_ಖಂಡ(mfp,
				   hp2 == ಖಾಲಿ ? hp : hp2, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2) == OK)
		    ನಿಲ್ಲಿಸು;
	    }

	    ಆದರೆ (attempt == 1)
	    {
		// If the swap file is on a network drive, and the network
		// gets disconnected and then re-connected, we can maybe fix it
		// by closing and then re-opening the file.
		ಆದರೆ (mfp->mf_fd >= 0)
		    close(mfp->mf_fd);
		mfp->mf_fd = mch_open_rw((ಅಕ್ಷರ *)mfp->mf_fname, mfp->mf_flags);
		mfp->mf_reopen = (mfp->mf_fd < 0);
	    }
	    ಆದರೆ (attempt == 2 || mfp->mf_fd < 0)
	    {
		// Avoid repeating the error message, this mostly happens when
		// the disk is full. We give the message again only after a
		// successful write or when hitting a key. We keep on trying,
		// in case some space becomes available.
		ಆದರೆ (!did_swapwrite_ಸಂದೇಶ)
		    emsg(_(e_write_error_ಒಳ_ಅದಲುಬದಲು_file));
		did_swapwrite_ಸಂದೇಶ = TRUE;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	did_swapwrite_ಸಂದೇಶ = FALSE;
	ಆದರೆ (hp2 != ಖಾಲಿ)		    // written a non-dummy block
	    hp2->bh_flags &= ~BH_DIRTY;
					    // appended to the file
	ಆದರೆ (nr + (blocknr_T)ಪುಟ_ಎಣಿಕೆ > mfp->mf_infile_ಎಣಿಕೆ)
	    mfp->mf_infile_ಎಣಿಕೆ = nr + ಪುಟ_ಎಣಿಕೆ;
	ಆದರೆ (nr == hp->bh_bnum)		    // written the desired block
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Write block "hp" with data size "size" to file "mfp->mf_fd".
 * Takes care of encryption.
 * Return FAIL or OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mf_write_ಖಂಡ(
    memfile_T	*mfp,
    bhdr_T	*hp,
    off_T	ಸ್ಥಾನಾಂತರ ಬಳಸದ,
    ಚಿಹ್ನೆರಹಿತ	ಗಾತ್ರ_2)
{
    char_u	*ದತ್ತಾಂಶ = hp->bh_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ = OK;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    // Encrypt if 'key' is set and this is a data block.
    ಆದರೆ (*mfp->mf_ಬಫರ್->b_p_ಕೀಲಿ != NUL)
    {
	ದತ್ತಾಂಶ = ml_encrypt_ದತ್ತಾಂಶ(mfp, ದತ್ತಾಂಶ, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2);
	ಆದರೆ (ದತ್ತಾಂಶ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ)write_eintr(mfp->mf_fd, ದತ್ತಾಂಶ, ಗಾತ್ರ_2) != ಗಾತ್ರ_2)
	ಫಲಿತಾಂಶ = FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    ಆದರೆ (ದತ್ತಾಂಶ != hp->bh_ದತ್ತಾಂಶ)
	vim_free(ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Make block number for *hp positive and add it to the translation list
 *
 * Return FAIL for failure, OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mf_trans_ಕೂಡಿಸು(memfile_T *mfp, bhdr_T *hp)
{
    bhdr_T	*freep;
    blocknr_T	new_bnum;
    NR_TRANS	*np;
    ಪೂರ್ಣಾಂಕ		ಪುಟ_ಎಣಿಕೆ;

    ಆದರೆ (hp->bh_bnum >= 0)		    // it's already positive
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ ((np = ALLOC_ONE(NR_TRANS)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

/*
 * Get a new number for the block.
 * If the first item in the free list has sufficient pages, use its number
 * Otherwise use mf_blocknr_max.
 */
    freep = mfp->mf_free_ಮೊದಲ;
    ಪುಟ_ಎಣಿಕೆ = hp->bh_ಪುಟ_ಎಣಿಕೆ;
    ಆದರೆ (freep != ಖಾಲಿ && freep->bh_ಪುಟ_ಎಣಿಕೆ >= ಪುಟ_ಎಣಿಕೆ)
    {
	new_bnum = freep->bh_bnum;
	/*
	 * If the page count of the free block was larger, reduce it.
	 * If the page count matches, remove the block from the free list
	 */
	ಆದರೆ (freep->bh_ಪುಟ_ಎಣಿಕೆ > ಪುಟ_ಎಣಿಕೆ)
	{
	    freep->bh_bnum += ಪುಟ_ಎಣಿಕೆ;
	    freep->bh_ಪುಟ_ಎಣಿಕೆ -= ಪುಟ_ಎಣಿಕೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    freep = mf_rem_free(mfp);
	    vim_free(freep);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	new_bnum = mfp->mf_blocknr_max;
	mfp->mf_blocknr_max += ಪುಟ_ಎಣಿಕೆ;
    }

    np->nt_old_bnum = hp->bh_bnum;	    // adjust number
    np->nt_new_bnum = new_bnum;

    mf_rem_hash(mfp, hp);		    // remove from old hash list
    hp->bh_bnum = new_bnum;
    mf_ins_hash(mfp, hp);		    // insert in new hash list

    // Insert "np" into "mf_trans" hashtable with key "np->nt_old_bnum"
    mf_hash_ಕೂಡಿಸು_ಅಂಶ(&mfp->mf_trans, (mf_hashitem_T *)np);

    ಹಿಂತಿರುಗಿ OK;
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

    np = (NR_TRANS *)mf_hash_ಕಂಡುಹಿಡಿ(&mfp->mf_trans, old_nr);

    ಆದರೆ (np == ಖಾಲಿ)		// not found
	ಹಿಂತಿರುಗಿ old_nr;

    mfp->mf_neg_ಎಣಿಕೆ--;
    new_bnum = np->nt_new_bnum;

    // remove entry from the trans list
    mf_hash_rem_ಅಂಶ(&mfp->mf_trans, (mf_hashitem_T *)np);

    vim_free(np);

    ಹಿಂತಿರುಗಿ new_bnum;
}

/*
 * Set mfp->mf_ffname according to mfp->mf_fname and some other things.
 * Only called when creating or renaming the swapfile.	Either way it's a new
 * name so we must work out the full path name.
 */
    ಶೂನ್ಯ
mf_ಗಣ_ffname(memfile_T *mfp)
{
    mfp->mf_ffname = ತುಂಬಿದ_Name_save(mfp->mf_fname, FALSE);
}

/*
 * Make the name of the file used for the memfile a full path.
 * Used before doing a :cd
 */
    ಶೂನ್ಯ
mf_fullname(memfile_T *mfp)
{
    ಆದರೆ (mfp == ಖಾಲಿ || mfp->mf_fname == ಖಾಲಿ || mfp->mf_ffname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    vim_free(mfp->mf_fname);
    mfp->mf_fname = mfp->mf_ffname;
    mfp->mf_ffname = ಖಾಲಿ;
}

/*
 * return TRUE if there are any translations pending for 'mfp'
 */
    ಪೂರ್ಣಾಂಕ
mf_need_trans(memfile_T *mfp)
{
    ಹಿಂತಿರುಗಿ (mfp->mf_fname != ಖಾಲಿ && mfp->mf_neg_ಎಣಿಕೆ > 0);
}

/*
 * Open a swap file for a memfile.
 * The "fname" must be in allocated memory, and is consumed (also when an
 * error occurs).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_do_open(
    memfile_T	*mfp,
    char_u	*fname,
    ಪೂರ್ಣಾಂಕ		flags)		// flags for open()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LSTAT
    stat_T	sb;
#ಆದರೆ_ಅಂತ್ಯ

    mfp->mf_fname = fname;

    /*
     * Get the full path name before the open, because this is
     * not possible after the open on the Amiga.
     * fname cannot be NameBuff, because it must have been allocated.
     */
    mf_ಗಣ_ffname(mfp);
#ಆದರೆ defined(MSWIN)
    /*
     * A ":!cd e:xxx" may change the directory without us knowing, use the
     * full pathname always.  Careful: This frees fname!
     */
    mf_fullname(mfp);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LSTAT
    /*
     * Extra security check: When creating a swap file it really shouldn't
     * exist yet.  If there is a symbolic link, this is most likely an attack.
     */
    ಆದರೆ ((flags & O_CREAT) && mch_lstat((ಅಕ್ಷರ *)mfp->mf_fname, &sb) >= 0)
    {
	mfp->mf_fd = -1;
	emsg(_(e_ಅದಲುಬದಲು_file_already_exists_symlink_attack));
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	/*
	 * try to open the file
	 */
	flags |= O_EXTRA | O_NOFOLLOW;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	// Prevent handle inheritance that cause problems with Cscope
	// (swap file may not be deleted if cscope connection was open after
	// the file)
	flags |= O_NOINHERIT;
#ಆದರೆ_ಅಂತ್ಯ
	mfp->mf_flags = flags;
	mfp->mf_fd = mch_open_rw((ಅಕ್ಷರ *)mfp->mf_fname, flags);
    }

    /*
     * If the file cannot be opened, use memory only
     */
    ಆದರೆ (mfp->mf_fd < 0)
    {
	VIM_CLEAR(mfp->mf_fname);
	VIM_CLEAR(mfp->mf_ffname);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FD_CLOEXEC
	ಪೂರ್ಣಾಂಕ fdflags = fcntl(mfp->mf_fd, F_GETFD);
	ಆದರೆ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (ಶೂನ್ಯ)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
	mch_copy_sec(fname, mfp->mf_fname);
#ಆದರೆ_ಅಂತ್ಯ
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
#ವ್ಯಾಖ್ಯಾನಿಸು MHT_LOG_ತುಂಬು_ಗುಣಕ 6
#ವ್ಯಾಖ್ಯಾನಿಸು MHT_GROWTH_ಗುಣಕ   2   // must be a power of two

/*
 * Initialize an empty hash table.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_hash_init(mf_hashtab_T *mht)
{
    CLEAR_ಸೂಚಕ(mht);
    mht->mht_buckets = mht->mht_small_buckets;
    mht->mht_ಮುಖವಾಡ = MHT_INIT_ಗಾತ್ರ - 1;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * The hash table must not be used again without another mf_hash_init() call.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_hash_free(mf_hashtab_T *mht)
{
    ಆದರೆ (mht->mht_buckets != mht->mht_small_buckets)
	vim_free(mht->mht_buckets);
}

/*
 * Free the array of a hash table and all the items it contains.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_hash_free_all(mf_hashtab_T *mht)
{
    long_u	    idx;
    mf_hashitem_T   *mhi;
    mf_hashitem_T   *ಮುಂದಿನ;

    ಗಾಗಿ (idx = 0; idx <= mht->mht_ಮುಖವಾಡ; idx++)
	ಗಾಗಿ (mhi = mht->mht_buckets[idx]; mhi != ಖಾಲಿ; mhi = ಮುಂದಿನ)
	{
	    ಮುಂದಿನ = mhi->mhi_ಮುಂದಿನ;
	    vim_free(mhi);
	}

    mf_hash_free(mht);
}

/*
 * Find "key" in hashtable "mht".
 * Returns a pointer to a mf_hashitem_T or NULL if the item was not found.
 */
    ಸ್ಥಿರ mf_hashitem_T *
mf_hash_ಕಂಡುಹಿಡಿ(mf_hashtab_T *mht, blocknr_T ಕೀಲಿ)
{
    mf_hashitem_T   *mhi;

    mhi = mht->mht_buckets[ಕೀಲಿ & mht->mht_ಮುಖವಾಡ];
    ಆಗ (mhi != ಖಾಲಿ && mhi->mhi_ಕೀಲಿ != ಕೀಲಿ)
	mhi = mhi->mhi_ಮುಂದಿನ;

    ಹಿಂತಿರುಗಿ mhi;
}

/*
 * Add item "mhi" to hashtable "mht".
 * "mhi" must not be NULL.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_hash_ಕೂಡಿಸು_ಅಂಶ(mf_hashtab_T *mht, mf_hashitem_T *mhi)
{
    long_u	    idx;

    idx = mhi->mhi_ಕೀಲಿ & mht->mht_ಮುಖವಾಡ;
    mhi->mhi_ಮುಂದಿನ = mht->mht_buckets[idx];
    mhi->mhi_prev = ಖಾಲಿ;
    ಆದರೆ (mhi->mhi_ಮುಂದಿನ != ಖಾಲಿ)
	mhi->mhi_ಮುಂದಿನ->mhi_prev = mhi;
    mht->mht_buckets[idx] = mhi;

    mht->mht_ಎಣಿಕೆ++;

    /*
     * Grow hashtable when we have more thank 2^MHT_LOG_LOAD_FACTOR
     * items per bucket on average
     */
    ಆದರೆ (mht->mht_fixed == 0
	&& (mht->mht_ಎಣಿಕೆ >> MHT_LOG_ತುಂಬು_ಗುಣಕ) > mht->mht_ಮುಖವಾಡ)
    {
	ಆದರೆ (mf_hash_grow(mht) == FAIL)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
mf_hash_rem_ಅಂಶ(mf_hashtab_T *mht, mf_hashitem_T *mhi)
{
    ಆದರೆ (mhi->mhi_prev == ಖಾಲಿ)
	mht->mht_buckets[mhi->mhi_ಕೀಲಿ & mht->mht_ಮುಖವಾಡ] = mhi->mhi_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	mhi->mhi_prev->mhi_ಮುಂದಿನ = mhi->mhi_ಮುಂದಿನ;

    ಆದರೆ (mhi->mhi_ಮುಂದಿನ != ಖಾಲಿ)
	mhi->mhi_ಮುಂದಿನ->mhi_prev = mhi->mhi_prev;

    mht->mht_ಎಣಿಕೆ--;

    // We could shrink the table here, but it typically takes little memory,
    // so why bother?
}

/*
 * Increase number of buckets in the hashtable by MHT_GROWTH_FACTOR and
 * rehash items.
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mf_hash_grow(mf_hashtab_T *mht)
{
    long_u	    i, j;
    ಪೂರ್ಣಾಂಕ		    ಸರಿಸು;
    mf_hashitem_T   *mhi;
    mf_hashitem_T   *tails[MHT_GROWTH_ಗುಣಕ];
    mf_hashitem_T   **buckets;
    size_t	    ಗಾತ್ರ_2;

    ಗಾತ್ರ_2 = (mht->mht_ಮುಖವಾಡ + 1) * MHT_GROWTH_ಗುಣಕ * ಗಾತ್ರ(ಶೂನ್ಯ *);
    buckets = lalloc_clear(ಗಾತ್ರ_2, FALSE);
    ಆದರೆ (buckets == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಸರಿಸು = 0;
    ಆಗ ((mht->mht_ಮುಖವಾಡ >> ಸರಿಸು) != 0)
	ಸರಿಸು++;

    ಗಾಗಿ (i = 0; i <= mht->mht_ಮುಖವಾಡ; i++)
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

	CLEAR_ಕ್ಷೇತ್ರ(tails);

	ಗಾಗಿ (mhi = mht->mht_buckets[i]; mhi != ಖಾಲಿ; mhi = mhi->mhi_ಮುಂದಿನ)
	{
	    j = (mhi->mhi_ಕೀಲಿ >> ಸರಿಸು) & (MHT_GROWTH_ಗುಣಕ - 1);
	    ಆದರೆ (tails[j] == ಖಾಲಿ)
	    {
		buckets[i + (j << ಸರಿಸು)] = mhi;
		tails[j] = mhi;
		mhi->mhi_prev = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		tails[j]->mhi_ಮುಂದಿನ = mhi;
		mhi->mhi_prev = tails[j];
		tails[j] = mhi;
	    }
	}

	ಗಾಗಿ (j = 0; j < MHT_GROWTH_ಗುಣಕ; j++)
	    ಆದರೆ (tails[j] != ಖಾಲಿ)
		tails[j]->mhi_ಮುಂದಿನ = ಖಾಲಿ;
    }

    ಆದರೆ (mht->mht_buckets != mht->mht_small_buckets)
	vim_free(mht->mht_buckets);

    mht->mht_buckets = buckets;
    mht->mht_ಮುಖವಾಡ = (mht->mht_ಮುಖವಾಡ + 1) * MHT_GROWTH_ಗುಣಕ - 1;

    ಹಿಂತಿರುಗಿ OK;
}
