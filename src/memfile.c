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

#شامل "vim.h"

/*
 * Some systems have the page size in statfs.f_bsize, some in stat.st_blksize
 */
#اگر_تعریف_شدہ HAVE_ST_BLKSIZE
# define STATFS stat
# define F_BSIZE st_blksize
# define fstatfs(fd, buf, len, nul) mch_fstat((fd), (buf))
#ورنہ
# ifdef HAVE_SYS_STATFS_H
#  include <sys/statfs.h>
#  define STATFS statfs
#  define F_BSIZE f_bsize
# endif
#اگر_ختم

/*
 * for Amiga Dos 2.0x we use Flush
 */
#اگر_تعریف_شدہ AMIGA
# ifdef FEAT_ARP
خارجی صحیح dos2;			// this is in os_amiga.c
# endif
# ifdef SASC
#  include <ios1.h>			// for chkufb()
# endif
#اگر_ختم

#تعریف_کرو MEMFILE_صفحہ_سائز 4096		// default page size

ساکن long_u	کل_mem_مستعمل = 0;	// total memory used for memfiles

ساکن خالی mf_ins_hash(memfile_T *, bhdr_T *);
ساکن خالی mf_rem_hash(memfile_T *, bhdr_T *);
ساکن bhdr_T *mf_ڈھونڈو_hash(memfile_T *, blocknr_T);
ساکن خالی mf_ins_مستعمل(memfile_T *, bhdr_T *);
ساکن خالی mf_rem_مستعمل(memfile_T *, bhdr_T *);
ساکن bhdr_T *mf_release(memfile_T *, صحیح);
ساکن bhdr_T *mf_alloc_bhdr(memfile_T *, صحیح);
ساکن خالی mf_free_bhdr(bhdr_T *);
ساکن خالی mf_ins_free(memfile_T *, bhdr_T *);
ساکن bhdr_T *mf_rem_free(memfile_T *);
ساکن صحیح  mf_read(memfile_T *, bhdr_T *);
ساکن صحیح  mf_write(memfile_T *, bhdr_T *);
ساکن صحیح  mf_write_بلاک(memfile_T *mfp, bhdr_T *hp, off_T انحراف, غیرعلامتی سائز);
ساکن صحیح  mf_trans_جوڑو(memfile_T *, bhdr_T *);
ساکن خالی mf_do_open(memfile_T *, char_u *, صحیح);
ساکن خالی mf_hash_init(mf_hashtab_T *);
ساکن خالی mf_hash_free(mf_hashtab_T *);
ساکن خالی mf_hash_free_all(mf_hashtab_T *);
ساکن mf_hashitem_T *mf_hash_ڈھونڈو(mf_hashtab_T *, blocknr_T);
ساکن خالی mf_hash_جوڑو_شے(mf_hashtab_T *, mf_hashitem_T *);
ساکن خالی mf_hash_rem_شے(mf_hashtab_T *, mf_hashitem_T *);
ساکن صحیح mf_hash_grow(mf_hashtab_T *);

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
mf_open(char_u *fname, صحیح flags)
{
    memfile_T		*mfp;
    off_T		سائز;
#اگر defined(STATFS) && defined(UNIX) && !defined(__QNX__) && !defined(__minix)
# define USE_FSTATFS
    ساخت STATFS	stf;
#اگر_ختم

    اگر ((mfp = ALLOC_ONE(memfile_T)) == تہی)
	واپس تہی;

    اگر (fname == تہی)	    // no file for this memfile, use memory only
    {
	mfp->mf_fname = تہی;
	mfp->mf_ffname = تہی;
	mfp->mf_fd = -1;
    }
    ورنہ
    {
	mf_do_open(mfp, fname, flags);	// try to open the file

	// if the file cannot be opened, return here
	اگر (mfp->mf_fd < 0)
	{
	    vim_free(mfp);
	    واپس تہی;
	}
    }

    mfp->mf_free_پہلا = تہی;		// free list is empty
    mfp->mf_مستعمل_پہلا = تہی;		// used list is empty
    mfp->mf_مستعمل_آخری = تہی;
    mfp->mf_dirty = MF_DIRTY_NO;
    mfp->mf_مستعمل_گنتی = 0;
    mf_hash_init(&mfp->mf_hash);
    mf_hash_init(&mfp->mf_trans);
    mfp->mf_صفحہ_سائز = MEMFILE_صفحہ_سائز;
#اگر_تعریف_شدہ FEAT_CRYPT
    mfp->mf_old_کلید = تہی;
#اگر_ختم

#اگر_تعریف_شدہ USE_FSTATFS
    /*
     * Try to set the page size equal to the block size of the device.
     * Speeds up I/O a lot.
     * When recovering, the actual block size will be retrieved from block 0
     * in ml_recover().  The size used here may be wrong, therefore
     * mf_blocknr_max must be rounded up.
     */
    اگر (mfp->mf_fd >= 0
	    && fstatfs(mfp->mf_fd, &stf, حجم(ساخت statfs), 0) == 0
	    && stf.F_BSIZE >= MIN_تبادلہ_صفحہ_سائز
	    && stf.F_BSIZE <= MAX_تبادلہ_صفحہ_سائز)
	mfp->mf_صفحہ_سائز = stf.F_BSIZE;
#اگر_ختم

    اگر (mfp->mf_fd < 0 || (flags & (O_TRUNC|O_EXCL))
		  || (سائز = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	mfp->mf_blocknr_max = 0;	// no file or empty file
    ورنہ
	mfp->mf_blocknr_max = (blocknr_T)((سائز + mfp->mf_صفحہ_سائز - 1)
							 / mfp->mf_صفحہ_سائز);
    mfp->mf_blocknr_min = -1;
    mfp->mf_neg_گنتی = 0;
    mfp->mf_infile_گنتی = mfp->mf_blocknr_max;

    /*
     * Compute maximum number of pages ('maxmem' is in Kbyte):
     *	'mammem' * 1Kbyte / page-size-in-bytes.
     * Avoid overflow by first reducing page size as much as possible.
     */
    {
	صحیح	    سرکاؤ = 10;
	غیرعلامتی    صفحہ_سائز = mfp->mf_صفحہ_سائز;

	جب (سرکاؤ > 0 && (صفحہ_سائز & 1) == 0)
	{
	    صفحہ_سائز = صفحہ_سائز >> 1;
	    --سرکاؤ;
	}
	mfp->mf_مستعمل_گنتی_max = (p_mm << سرکاؤ) / صفحہ_سائز;
	اگر (mfp->mf_مستعمل_گنتی_max < 10)
	    mfp->mf_مستعمل_گنتی_max = 10;
    }

    واپس mfp;
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
    صحیح
mf_open_file(memfile_T *mfp, char_u *fname)
{
    mf_do_open(mfp, fname, O_RDWR|O_CREAT|O_EXCL); // try to open the file

    اگر (mfp->mf_fd < 0)
	واپس FAIL;

    mfp->mf_dirty = MF_DIRTY_YES;
    واپس OK;
}

/*
 * Close a memory file and delete the associated file if 'del_file' is TRUE.
 */
    خالی
mf_close(memfile_T *mfp, صحیح del_file)
{
    bhdr_T	*hp, *nextp;

    اگر (mfp == تہی)		    // safety check
	واپس;
    اگر (mfp->mf_fd >= 0)
    {
	اگر (close(mfp->mf_fd) < 0)
	    emsg(_(e_close_error_on_تبادلہ_file));
    }
    اگر (del_file && mfp->mf_fname != تہی)
	mch_remove(mfp->mf_fname);
					    // free entries in used list
    برائے (hp = mfp->mf_مستعمل_پہلا; hp != تہی; hp = nextp)
    {
	کل_mem_مستعمل -= (long_u)hp->bh_صفحہ_گنتی * mfp->mf_صفحہ_سائز;
	nextp = hp->bh_اگلا;
	mf_free_bhdr(hp);
    }
    جب (mfp->mf_free_پہلا != تہی)	    // free entries in free list
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
    خالی
mf_close_file(
    buf_T	*buf,
    صحیح		getlines)	// get all lines into memory?
{
    memfile_T	*mfp;
    linenr_T	lnum;

    mfp = buf->b_ml.ml_mfp;
    اگر (mfp == تہی || mfp->mf_fd < 0)		// nothing to close
	واپس;

    اگر (getlines)
    {
	// get all blocks in memory by accessing all lines (clumsy!)
	mf_dont_release = TRUE;
	برائے (lnum = 1; lnum <= buf->b_ml.ml_سطر_گنتی; ++lnum)
	    (خالی)ml_get_buf(buf, lnum, FALSE);
	mf_dont_release = FALSE;
	// TODO: should check if all blocks are really in core
    }

    اگر (close(mfp->mf_fd) < 0)			// close the file
	emsg(_(e_close_error_on_تبادلہ_file));
    mfp->mf_fd = -1;

    اگر (mfp->mf_fname != تہی)
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
    خالی
mf_new_صفحہ_سائز(memfile_T *mfp, غیرعلامتی new_سائز)
{
    // Correct the memory used for block 0 to the new size, because it will be
    // freed with that size later on.
    کل_mem_مستعمل += new_سائز - mfp->mf_صفحہ_سائز;
    mfp->mf_صفحہ_سائز = new_سائز;
}

/*
 * get a new block
 *
 *   negative: TRUE if negative block number desired (data block)
 */
    bhdr_T *
mf_new(memfile_T *mfp, صحیح negative, صحیح صفحہ_گنتی)
{
    bhdr_T	*hp;	// new bhdr_T
    bhdr_T	*freep;	// first block in free list
    char_u	*p;

    /*
     * If we reached the maximum size for the used memory blocks, release one
     * If a bhdr_T is returned, use it and adjust the page_count if necessary.
     */
    hp = mf_release(mfp, صفحہ_گنتی);

/*
 * Decide on the number to use:
 * If there is a free block, use its number.
 * Otherwise use mf_block_min for a negative number, mf_block_max for
 * a positive number.
 */
    freep = mfp->mf_free_پہلا;
    اگر (!negative && freep != تہی && freep->bh_صفحہ_گنتی >= صفحہ_گنتی)
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
	اگر (freep->bh_صفحہ_گنتی > صفحہ_گنتی)
	{
	    اگر (hp == تہی && (hp = mf_alloc_bhdr(mfp, صفحہ_گنتی)) == تہی)
		واپس تہی;
	    hp->bh_bnum = freep->bh_bnum;
	    freep->bh_bnum += صفحہ_گنتی;
	    freep->bh_صفحہ_گنتی -= صفحہ_گنتی;
	}
	ورنہ اگر (hp == تہی)	    // need to allocate memory for this block
	{
	    اگر ((p = alloc((size_t)mfp->mf_صفحہ_سائز * صفحہ_گنتی)) == تہی)
		واپس تہی;
	    hp = mf_rem_free(mfp);
	    hp->bh_ڈیٹا = p;
	}
	ورنہ		    // use the number, remove entry from free list
	{
	    freep = mf_rem_free(mfp);
	    hp->bh_bnum = freep->bh_bnum;
	    vim_free(freep);
	}
    }
    ورنہ	// get a new number
    {
	اگر (hp == تہی && (hp = mf_alloc_bhdr(mfp, صفحہ_گنتی)) == تہی)
	    واپس تہی;
	اگر (negative)
	{
	    hp->bh_bnum = mfp->mf_blocknr_min--;
	    mfp->mf_neg_گنتی++;
	}
	ورنہ
	{
	    hp->bh_bnum = mfp->mf_blocknr_max;
	    mfp->mf_blocknr_max += صفحہ_گنتی;
	}
    }
    hp->bh_flags = BH_LOCKED | BH_DIRTY;	// new block is always dirty
    mfp->mf_dirty = MF_DIRTY_YES;
    hp->bh_صفحہ_گنتی = صفحہ_گنتی;
    mf_ins_مستعمل(mfp, hp);
    mf_ins_hash(mfp, hp);

    /*
     * Init the data to all zero, to avoid reading uninitialized data.
     * This also avoids that the passwd file ends up in the swap file!
     */
    (خالی)vim_memset((حرف *)(hp->bh_ڈیٹا), 0,
				      (size_t)mfp->mf_صفحہ_سائز * صفحہ_گنتی);

    واپس hp;
}

/*
 * Get existing block "nr" with "page_count" pages.
 *
 * Note: The caller should first check a negative nr with mf_trans_del()
 */
    bhdr_T *
mf_get(memfile_T *mfp, blocknr_T nr, صحیح صفحہ_گنتی)
{
    bhdr_T    *hp;
						// doesn't exist
    اگر (nr >= mfp->mf_blocknr_max || nr <= mfp->mf_blocknr_min)
	واپس تہی;

    /*
     * see if it is in the cache
     */
    hp = mf_ڈھونڈو_hash(mfp, nr);
    اگر (hp == تہی)	// not in the hash list
    {
	اگر (nr < 0 || nr >= mfp->mf_infile_گنتی)   // can't be in the file
	    واپس تہی;

	// could check here if the block is in the free list

	/*
	 * Check if we need to flush an existing block.
	 * If so, use that block.
	 * If not, allocate a new block.
	 */
	hp = mf_release(mfp, صفحہ_گنتی);
	اگر (hp == تہی && صفحہ_گنتی > 0)
	    hp = mf_alloc_bhdr(mfp, صفحہ_گنتی);
	اگر (hp == تہی)
	    واپس تہی;

	hp->bh_bnum = nr;
	hp->bh_flags = 0;
	hp->bh_صفحہ_گنتی = صفحہ_گنتی;
	اگر (mf_read(mfp, hp) == FAIL)	    // cannot read the block!
	{
	    mf_free_bhdr(hp);
	    واپس تہی;
	}
    }
    ورنہ
    {
	mf_rem_مستعمل(mfp, hp);	// remove from list, insert in front below
	mf_rem_hash(mfp, hp);
    }

    hp->bh_flags |= BH_LOCKED;
    mf_ins_مستعمل(mfp, hp);	// put in front of used list
    mf_ins_hash(mfp, hp);	// put in front of hash list

    واپس hp;
}

/*
 * release the block *hp
 *
 *   dirty: Block must be written to file later
 *   infile: Block should be in file (needed for recovery)
 *
 *  no return value, function cannot fail
 */
    خالی
mf_put(
    memfile_T	*mfp,
    bhdr_T	*hp,
    صحیح		dirty,
    صحیح		infile)
{
    صحیح		flags;

    flags = hp->bh_flags;

    اگر ((flags & BH_LOCKED) == 0)
	iemsg(e_بلاک_was_not_locked);
    flags &= ~BH_LOCKED;
    اگر (dirty)
    {
	flags |= BH_DIRTY;
	اگر (mfp->mf_dirty != MF_DIRTY_YES_NOSYNC)
	    mfp->mf_dirty = MF_DIRTY_YES;
    }
    hp->bh_flags = flags;
    اگر (infile)
	mf_trans_جوڑو(mfp, hp);	    // may translate negative in positive nr
}

/*
 * block *hp is no longer in used, may put it in the free list of memfile *mfp
 */
    خالی
mf_free(memfile_T *mfp, bhdr_T *hp)
{
    vim_free(hp->bh_ڈیٹا);	// free the memory
    mf_rem_hash(mfp, hp);	// get *hp out of the hash list
    mf_rem_مستعمل(mfp, hp);	// get *hp out of the used list
    اگر (hp->bh_bnum < 0)
    {
	vim_free(hp);		// don't want negative numbers in free list
	mfp->mf_neg_گنتی--;
    }
    ورنہ
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
    صحیح
mf_sync(memfile_T *mfp, صحیح flags)
{
    صحیح		حالت_2;
    bhdr_T	*hp;
    صحیح		got_int_save = got_int;

    اگر (mfp->mf_fd < 0)
    {
	// there is no file, nothing to do
	mfp->mf_dirty = MF_DIRTY_NO;
	واپس FAIL;
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
    حالت_2 = OK;
    برائے (hp = mfp->mf_مستعمل_آخری; hp != تہی; hp = hp->bh_پچھلا)
	اگر (((flags & MFS_ALL) || hp->bh_bnum >= 0)
		&& (hp->bh_flags & BH_DIRTY)
		&& (حالت_2 == OK || (hp->bh_bnum >= 0
		    && hp->bh_bnum < mfp->mf_infile_گنتی)))
	{
	    اگر ((flags & MFS_ZERO) && hp->bh_bnum != 0)
		جاری;
	    اگر (mf_write(mfp, hp) == FAIL)
	    {
		اگر (حالت_2 == FAIL)	// double error: quit syncing
		    توڑو;
		حالت_2 = FAIL;
	    }
	    اگر (flags & MFS_STOP)
	    {
		// Stop when char available now.
		اگر (ui_char_avail())
		    توڑو;
	    }
	    ورنہ
		ui_breakcheck();
	    اگر (got_int)
		توڑو;
	}

    /*
     * If the whole list is flushed, the memfile is not dirty anymore.
     * In case of an error this flag is also set, to avoid trying all the time.
     */
    اگر (hp == تہی || حالت_2 == FAIL)
	mfp->mf_dirty = MF_DIRTY_NO;

    اگر ((flags & MFS_FLUSH) && *p_sws != NUL)
    {
#اگر defined(UNIX)
# ifdef HAVE_FSYNC
	/*
	 * most Unixes have the very useful fsync() function, just what we need.
	 */
	اگر (STRCMP(p_sws, "fsync") == 0)
	{
	    اگر (vim_fsync(mfp->mf_fd))
		حالت_2 = FAIL;
	}
	ورنہ
# endif
# ifdef HAVE_SYNC
	    sync();
# ورنہ
	    fflush(تہی);
# endif
#اگر_ختم
#اگر_تعریف_شدہ VMS
	اگر (STRCMP(p_sws, "fsync") == 0)
	{
	    اگر (vim_fsync(mfp->mf_fd))
		حالت_2 = FAIL;
	}
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
	اگر (_commit(mfp->mf_fd))
	    حالت_2 = FAIL;
#اگر_ختم
#اگر_تعریف_شدہ AMIGA
# اگر defined(__AROS__) || defined(__amigaos4__)
	اگر (vim_fsync(mfp->mf_fd) != 0)
	    حالت_2 = FAIL;
# ورنہ
	/*
	 * Flush() only exists for AmigaDos 2.0.
	 * For 1.3 it should be done with close() + open(), but then the risk
	 * is that the open() may fail and lose the file....
	 */
#  ifdef FEAT_ARP
	اگر (dos2)
#  endif
#  ifdef SASC
	{
	    ساخت UFB *fp = chkufb(mfp->mf_fd);

	    اگر (fp != تہی)
		Flush(fp->ufbfh);
	}
#  ورنہ
#   اگر defined(_DCC) || defined(__GNUC__) || defined(__MORPHOS__)
	{
#    اگر defined(__GNUC__) && !defined(__MORPHOS__) && defined(__libnix__)
	    // Have function (in libnix at least),
	    // but ain't got no prototype anywhere.
	    خارجی غیرعلامتی طویل fdtofh(صحیح filedescriptor);
#    endif
#    اگر !defined(__libnix__)
	    fflush(تہی);
#    ورنہ
	    BPTR fh = (BPTR)fdtofh(mfp->mf_fd);

	    اگر (fh != 0)
		Flush(fh);
#    endif
	}
#   ورنہ // assume Manx
	    Flush(_devtab[mfp->mf_fd].fd);
#   endif
#  endif
# endif
#اگر_ختم // AMIGA
    }

    got_int |= got_int_save;

    واپس حالت_2;
}

/*
 * For all blocks in memory file *mfp that have a positive block number set
 * the dirty flag.  These are blocks that need to be written to a newly
 * created swapfile.
 */
    خالی
mf_سیٹ_dirty(memfile_T *mfp)
{
    bhdr_T	*hp;

    برائے (hp = mfp->mf_مستعمل_آخری; hp != تہی; hp = hp->bh_پچھلا)
	اگر (hp->bh_bnum > 0)
	    hp->bh_flags |= BH_DIRTY;
    mfp->mf_dirty = MF_DIRTY_YES;
}

/*
 * insert block *hp in front of hashlist of memfile *mfp
 */
    ساکن خالی
mf_ins_hash(memfile_T *mfp, bhdr_T *hp)
{
    mf_hash_جوڑو_شے(&mfp->mf_hash, (mf_hashitem_T *)hp);
}

/*
 * remove block *hp from hashlist of memfile list *mfp
 */
    ساکن خالی
mf_rem_hash(memfile_T *mfp, bhdr_T *hp)
{
    mf_hash_rem_شے(&mfp->mf_hash, (mf_hashitem_T *)hp);
}

/*
 * look in hash lists of memfile *mfp for block header with number 'nr'
 */
    ساکن bhdr_T *
mf_ڈھونڈو_hash(memfile_T *mfp, blocknr_T nr)
{
    واپس (bhdr_T *)mf_hash_ڈھونڈو(&mfp->mf_hash, nr);
}

/*
 * insert block *hp in front of used list of memfile *mfp
 */
    ساکن خالی
mf_ins_مستعمل(memfile_T *mfp, bhdr_T *hp)
{
    hp->bh_اگلا = mfp->mf_مستعمل_پہلا;
    mfp->mf_مستعمل_پہلا = hp;
    hp->bh_پچھلا = تہی;
    اگر (hp->bh_اگلا == تہی)	    // list was empty, adjust last pointer
	mfp->mf_مستعمل_آخری = hp;
    ورنہ
	hp->bh_اگلا->bh_پچھلا = hp;
    mfp->mf_مستعمل_گنتی += hp->bh_صفحہ_گنتی;
    کل_mem_مستعمل += (long_u)hp->bh_صفحہ_گنتی * mfp->mf_صفحہ_سائز;
}

/*
 * remove block *hp from used list of memfile *mfp
 */
    ساکن خالی
mf_rem_مستعمل(memfile_T *mfp, bhdr_T *hp)
{
    اگر (hp->bh_اگلا == تہی)	    // last block in used list
	mfp->mf_مستعمل_آخری = hp->bh_پچھلا;
    ورنہ
	hp->bh_اگلا->bh_پچھلا = hp->bh_پچھلا;
    اگر (hp->bh_پچھلا == تہی)	    // first block in used list
	mfp->mf_مستعمل_پہلا = hp->bh_اگلا;
    ورنہ
	hp->bh_پچھلا->bh_اگلا = hp->bh_اگلا;
    mfp->mf_مستعمل_گنتی -= hp->bh_صفحہ_گنتی;
    کل_mem_مستعمل -= (long_u)hp->bh_صفحہ_گنتی * mfp->mf_صفحہ_سائز;
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
    ساکن bhdr_T *
mf_release(memfile_T *mfp, صحیح صفحہ_گنتی)
{
    bhdr_T	*hp;
    صحیح		need_release;
    buf_T	*buf;

    // don't release while in mf_close_file()
    اگر (mf_dont_release)
	واپس تہی;

    /*
     * Need to release a block if the number of blocks for this memfile is
     * higher than the maximum or total memory used is over 'maxmemtot'
     */
    need_release = ((mfp->mf_مستعمل_گنتی >= mfp->mf_مستعمل_گنتی_max)
				  || (کل_mem_مستعمل >> 10) >= (long_u)p_mmt);

    /*
     * Try to create a swap file if the amount of memory used is getting too
     * high.
     */
    اگر (mfp->mf_fd < 0 && need_release && p_uc)
    {
	// find for which buffer this memfile is
	FOR_ALL_BUFFERS(buf)
	    اگر (buf->b_ml.ml_mfp == mfp)
		توڑو;
	اگر (buf != تہی && buf->b_may_تبادلہ)
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
    اگر (mfp->mf_fd < 0 || !need_release)
	واپس تہی;

    برائے (hp = mfp->mf_مستعمل_آخری; hp != تہی; hp = hp->bh_پچھلا)
	اگر (!(hp->bh_flags & BH_LOCKED))
	    توڑو;
    اگر (hp == تہی)	// not a single one that can be released
	واپس تہی;

    /*
     * If the block is dirty, write it.
     * If the write fails we don't free it.
     */
    اگر ((hp->bh_flags & BH_DIRTY) && mf_write(mfp, hp) == FAIL)
	واپس تہی;

    mf_rem_مستعمل(mfp, hp);
    mf_rem_hash(mfp, hp);

    /*
     * If a bhdr_T is returned, make sure that the page_count of bh_data is
     * right
     */
    اگر (hp->bh_صفحہ_گنتی != صفحہ_گنتی)
    {
	VIM_CLEAR(hp->bh_ڈیٹا);
	اگر (صفحہ_گنتی > 0)
	    hp->bh_ڈیٹا = alloc((size_t)mfp->mf_صفحہ_سائز * صفحہ_گنتی);
	اگر (hp->bh_ڈیٹا == تہی)
	{
	    vim_free(hp);
	    واپس تہی;
	}
	hp->bh_صفحہ_گنتی = صفحہ_گنتی;
    }
    واپس hp;
}

/*
 * release as many blocks as possible
 * Used in case of out of memory
 *
 * return TRUE if any memory was released
 */
    صحیح
mf_release_all(خالی)
{
    buf_T	*buf;
    memfile_T	*mfp;
    bhdr_T	*hp;
    صحیح		retval = FALSE;

    FOR_ALL_BUFFERS(buf)
    {
	mfp = buf->b_ml.ml_mfp;
	اگر (mfp != تہی)
	{
	    // If no swap file yet, may open one
	    اگر (mfp->mf_fd < 0 && buf->b_may_تبادلہ)
		ml_open_file(buf);

	    // only if there is a swapfile
	    اگر (mfp->mf_fd >= 0)
	    {
		برائے (hp = mfp->mf_مستعمل_آخری; hp != تہی; )
		{
		    اگر (!(hp->bh_flags & BH_LOCKED)
			    && (!(hp->bh_flags & BH_DIRTY)
				|| mf_write(mfp, hp) != FAIL))
		    {
			mf_rem_مستعمل(mfp, hp);
			mf_rem_hash(mfp, hp);
			mf_free_bhdr(hp);
			hp = mfp->mf_مستعمل_آخری;	// re-start, list was changed
			retval = TRUE;
		    }
		    ورنہ
			hp = hp->bh_پچھلا;
		}
	    }
	}
    }
    واپس retval;
}

/*
 * Allocate a block header and a block of memory for it.
 */
    ساکن bhdr_T *
mf_alloc_bhdr(memfile_T *mfp, صحیح صفحہ_گنتی)
{
    bhdr_T	*hp;

    اگر ((hp = ALLOC_ONE(bhdr_T)) == تہی)
	واپس تہی;

    اگر ((hp->bh_ڈیٹا = alloc((size_t)mfp->mf_صفحہ_سائز * صفحہ_گنتی)) == تہی)
    {
	vim_free(hp);	    // not enough memory
	واپس تہی;
    }
    hp->bh_صفحہ_گنتی = صفحہ_گنتی;
    واپس hp;
}

/*
 * Free a block header and the block of memory for it.
 */
    ساکن خالی
mf_free_bhdr(bhdr_T *hp)
{
    vim_free(hp->bh_ڈیٹا);
    vim_free(hp);
}

/*
 * Insert entry *hp in the free list.
 */
    ساکن خالی
mf_ins_free(memfile_T *mfp, bhdr_T *hp)
{
    hp->bh_اگلا = mfp->mf_free_پہلا;
    mfp->mf_free_پہلا = hp;
}

/*
 * remove the first entry from the free list and return a pointer to it
 * Note: caller must check that mfp->mf_free_first is not NULL!
 */
    ساکن bhdr_T *
mf_rem_free(memfile_T *mfp)
{
    bhdr_T	*hp;

    hp = mfp->mf_free_پہلا;
    mfp->mf_free_پہلا = hp->bh_اگلا;
    واپس hp;
}

/*
 * read a block from disk
 *
 * Return FAIL for failure, OK otherwise
 */
    ساکن صحیح
mf_read(memfile_T *mfp, bhdr_T *hp)
{
    off_T	انحراف;
    غیرعلامتی	صفحہ_سائز;
    غیرعلامتی	سائز;

    اگر (mfp->mf_fd < 0)	    // there is no file, can't read
	واپس FAIL;

    صفحہ_سائز = mfp->mf_صفحہ_سائز;
    انحراف = (off_T)صفحہ_سائز * hp->bh_bnum;
    سائز = صفحہ_سائز * hp->bh_صفحہ_گنتی;
    اگر (vim_lseek(mfp->mf_fd, انحراف, SEEK_SET) != انحراف)
    {
	PERROR(_(e_seek_error_اندر_تبادلہ_file_read));
	واپس FAIL;
    }
    اگر ((غیرعلامتی)read_eintr(mfp->mf_fd, hp->bh_ڈیٹا, سائز) != سائز)
    {
	PERROR(_(e_read_error_اندر_تبادلہ_file));
	واپس FAIL;
    }

#اگر_تعریف_شدہ FEAT_CRYPT
    // Decrypt if 'key' is set and this is a data block. And when changing the
    // key.
    اگر (*mfp->mf_بفر->b_p_کلید != NUL || mfp->mf_old_کلید != تہی)
	ml_decrypt_ڈیٹا(mfp, hp->bh_ڈیٹا, انحراف, سائز);
#اگر_ختم

    واپس OK;
}

/*
 * write a block to disk
 *
 * Return FAIL for failure, OK otherwise
 */
    ساکن صحیح
mf_write(memfile_T *mfp, bhdr_T *hp)
{
    off_T	انحراف;	    // offset in the file
    blocknr_T	nr;	    // block nr which is being written
    bhdr_T	*hp2;
    غیرعلامتی	صفحہ_سائز;  // number of bytes in a page
    غیرعلامتی	صفحہ_گنتی; // number of pages written
    غیرعلامتی	سائز;	    // number of bytes written

    اگر (mfp->mf_fd < 0 && !mfp->mf_reopen)
	// there is no file and there was no file, can't write
	واپس FAIL;

    اگر (hp->bh_bnum < 0)	// must assign file block number
	اگر (mf_trans_جوڑو(mfp, hp) == FAIL)
	    واپس FAIL;

    صفحہ_سائز = mfp->mf_صفحہ_سائز;

    /*
     * We don't want gaps in the file. Write the blocks in front of *hp
     * to extend the file.
     * If block 'mf_infile_count' is not in the hash list, it has been
     * freed. Fill the space in the file with data from the current block.
     */
    برائے (;;)
    {
	صحیح attempt;

	nr = hp->bh_bnum;
	اگر (nr > mfp->mf_infile_گنتی)		// beyond end of file
	{
	    nr = mfp->mf_infile_گنتی;
	    hp2 = mf_ڈھونڈو_hash(mfp, nr);	// NULL caught below
	}
	ورنہ
	    hp2 = hp;

	انحراف = (off_T)صفحہ_سائز * nr;
	اگر (hp2 == تہی)	    // freed block, fill with dummy data
	    صفحہ_گنتی = 1;
	ورنہ
	    صفحہ_گنتی = hp2->bh_صفحہ_گنتی;
	سائز = صفحہ_سائز * صفحہ_گنتی;

	برائے (attempt = 1; attempt <= 2; ++attempt)
	{
	    اگر (mfp->mf_fd >= 0)
	    {
		اگر (vim_lseek(mfp->mf_fd, انحراف, SEEK_SET) != انحراف)
		{
		    PERROR(_(e_seek_error_اندر_تبادلہ_file_write));
		    واپس FAIL;
		}
		اگر (mf_write_بلاک(mfp,
				   hp2 == تہی ? hp : hp2, انحراف, سائز) == OK)
		    توڑو;
	    }

	    اگر (attempt == 1)
	    {
		// If the swap file is on a network drive, and the network
		// gets disconnected and then re-connected, we can maybe fix it
		// by closing and then re-opening the file.
		اگر (mfp->mf_fd >= 0)
		    close(mfp->mf_fd);
		mfp->mf_fd = mch_open_rw((حرف *)mfp->mf_fname, mfp->mf_flags);
		mfp->mf_reopen = (mfp->mf_fd < 0);
	    }
	    اگر (attempt == 2 || mfp->mf_fd < 0)
	    {
		// Avoid repeating the error message, this mostly happens when
		// the disk is full. We give the message again only after a
		// successful write or when hitting a key. We keep on trying,
		// in case some space becomes available.
		اگر (!did_swapwrite_پیغام)
		    emsg(_(e_write_error_اندر_تبادلہ_file));
		did_swapwrite_پیغام = TRUE;
		واپس FAIL;
	    }
	}

	did_swapwrite_پیغام = FALSE;
	اگر (hp2 != تہی)		    // written a non-dummy block
	    hp2->bh_flags &= ~BH_DIRTY;
					    // appended to the file
	اگر (nr + (blocknr_T)صفحہ_گنتی > mfp->mf_infile_گنتی)
	    mfp->mf_infile_گنتی = nr + صفحہ_گنتی;
	اگر (nr == hp->bh_bnum)		    // written the desired block
	    توڑو;
    }
    واپس OK;
}

/*
 * Write block "hp" with data size "size" to file "mfp->mf_fd".
 * Takes care of encryption.
 * Return FAIL or OK.
 */
    ساکن صحیح
mf_write_بلاک(
    memfile_T	*mfp,
    bhdr_T	*hp,
    off_T	انحراف غیر_مستعمل,
    غیرعلامتی	سائز)
{
    char_u	*ڈیٹا = hp->bh_ڈیٹا;
    صحیح		نتیجہ = OK;

#اگر_تعریف_شدہ FEAT_CRYPT
    // Encrypt if 'key' is set and this is a data block.
    اگر (*mfp->mf_بفر->b_p_کلید != NUL)
    {
	ڈیٹا = ml_encrypt_ڈیٹا(mfp, ڈیٹا, انحراف, سائز);
	اگر (ڈیٹا == تہی)
	    واپس FAIL;
    }
#اگر_ختم

    اگر ((غیرعلامتی)write_eintr(mfp->mf_fd, ڈیٹا, سائز) != سائز)
	نتیجہ = FAIL;

#اگر_تعریف_شدہ FEAT_CRYPT
    اگر (ڈیٹا != hp->bh_ڈیٹا)
	vim_free(ڈیٹا);
#اگر_ختم

    واپس نتیجہ;
}

/*
 * Make block number for *hp positive and add it to the translation list
 *
 * Return FAIL for failure, OK otherwise
 */
    ساکن صحیح
mf_trans_جوڑو(memfile_T *mfp, bhdr_T *hp)
{
    bhdr_T	*freep;
    blocknr_T	new_bnum;
    NR_TRANS	*np;
    صحیح		صفحہ_گنتی;

    اگر (hp->bh_bnum >= 0)		    // it's already positive
	واپس OK;

    اگر ((np = ALLOC_ONE(NR_TRANS)) == تہی)
	واپس FAIL;

/*
 * Get a new number for the block.
 * If the first item in the free list has sufficient pages, use its number
 * Otherwise use mf_blocknr_max.
 */
    freep = mfp->mf_free_پہلا;
    صفحہ_گنتی = hp->bh_صفحہ_گنتی;
    اگر (freep != تہی && freep->bh_صفحہ_گنتی >= صفحہ_گنتی)
    {
	new_bnum = freep->bh_bnum;
	/*
	 * If the page count of the free block was larger, reduce it.
	 * If the page count matches, remove the block from the free list
	 */
	اگر (freep->bh_صفحہ_گنتی > صفحہ_گنتی)
	{
	    freep->bh_bnum += صفحہ_گنتی;
	    freep->bh_صفحہ_گنتی -= صفحہ_گنتی;
	}
	ورنہ
	{
	    freep = mf_rem_free(mfp);
	    vim_free(freep);
	}
    }
    ورنہ
    {
	new_bnum = mfp->mf_blocknr_max;
	mfp->mf_blocknr_max += صفحہ_گنتی;
    }

    np->nt_old_bnum = hp->bh_bnum;	    // adjust number
    np->nt_new_bnum = new_bnum;

    mf_rem_hash(mfp, hp);		    // remove from old hash list
    hp->bh_bnum = new_bnum;
    mf_ins_hash(mfp, hp);		    // insert in new hash list

    // Insert "np" into "mf_trans" hashtable with key "np->nt_old_bnum"
    mf_hash_جوڑو_شے(&mfp->mf_trans, (mf_hashitem_T *)np);

    واپس OK;
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

    np = (NR_TRANS *)mf_hash_ڈھونڈو(&mfp->mf_trans, old_nr);

    اگر (np == تہی)		// not found
	واپس old_nr;

    mfp->mf_neg_گنتی--;
    new_bnum = np->nt_new_bnum;

    // remove entry from the trans list
    mf_hash_rem_شے(&mfp->mf_trans, (mf_hashitem_T *)np);

    vim_free(np);

    واپس new_bnum;
}

/*
 * Set mfp->mf_ffname according to mfp->mf_fname and some other things.
 * Only called when creating or renaming the swapfile.	Either way it's a new
 * name so we must work out the full path name.
 */
    خالی
mf_سیٹ_ffname(memfile_T *mfp)
{
    mfp->mf_ffname = بھرا_Name_save(mfp->mf_fname, FALSE);
}

/*
 * Make the name of the file used for the memfile a full path.
 * Used before doing a :cd
 */
    خالی
mf_fullname(memfile_T *mfp)
{
    اگر (mfp == تہی || mfp->mf_fname == تہی || mfp->mf_ffname == تہی)
	واپس;

    vim_free(mfp->mf_fname);
    mfp->mf_fname = mfp->mf_ffname;
    mfp->mf_ffname = تہی;
}

/*
 * return TRUE if there are any translations pending for 'mfp'
 */
    صحیح
mf_need_trans(memfile_T *mfp)
{
    واپس (mfp->mf_fname != تہی && mfp->mf_neg_گنتی > 0);
}

/*
 * Open a swap file for a memfile.
 * The "fname" must be in allocated memory, and is consumed (also when an
 * error occurs).
 */
    ساکن خالی
mf_do_open(
    memfile_T	*mfp,
    char_u	*fname,
    صحیح		flags)		// flags for open()
{
#اگر_تعریف_شدہ HAVE_LSTAT
    stat_T	sb;
#اگر_ختم

    mfp->mf_fname = fname;

    /*
     * Get the full path name before the open, because this is
     * not possible after the open on the Amiga.
     * fname cannot be NameBuff, because it must have been allocated.
     */
    mf_سیٹ_ffname(mfp);
#اگر defined(MSWIN)
    /*
     * A ":!cd e:xxx" may change the directory without us knowing, use the
     * full pathname always.  Careful: This frees fname!
     */
    mf_fullname(mfp);
#اگر_ختم

#اگر_تعریف_شدہ HAVE_LSTAT
    /*
     * Extra security check: When creating a swap file it really shouldn't
     * exist yet.  If there is a symbolic link, this is most likely an attack.
     */
    اگر ((flags & O_CREAT) && mch_lstat((حرف *)mfp->mf_fname, &sb) >= 0)
    {
	mfp->mf_fd = -1;
	emsg(_(e_تبادلہ_file_already_exists_symlink_attack));
    }
    ورنہ
#اگر_ختم
    {
	/*
	 * try to open the file
	 */
	flags |= O_EXTRA | O_NOFOLLOW;
#اگر_تعریف_شدہ MSWIN
	// Prevent handle inheritance that cause problems with Cscope
	// (swap file may not be deleted if cscope connection was open after
	// the file)
	flags |= O_NOINHERIT;
#اگر_ختم
	mfp->mf_flags = flags;
	mfp->mf_fd = mch_open_rw((حرف *)mfp->mf_fname, flags);
    }

    /*
     * If the file cannot be opened, use memory only
     */
    اگر (mfp->mf_fd < 0)
    {
	VIM_CLEAR(mfp->mf_fname);
	VIM_CLEAR(mfp->mf_ffname);
    }
    ورنہ
    {
#اگر_تعریف_شدہ HAVE_FD_CLOEXEC
	صحیح fdflags = fcntl(mfp->mf_fd, F_GETFD);
	اگر (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
	    (خالی)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
#اگر_ختم
#اگر defined(HAVE_SELINUX) || defined(HAVE_SMACK)
	mch_copy_sec(fname, mfp->mf_fname);
#اگر_ختم
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
#تعریف_کرو MHT_LOG_بھرو_ضربی_عدد 6
#تعریف_کرو MHT_GROWTH_ضربی_عدد   2   // must be a power of two

/*
 * Initialize an empty hash table.
 */
    ساکن خالی
mf_hash_init(mf_hashtab_T *mht)
{
    CLEAR_اشارہ_گر(mht);
    mht->mht_buckets = mht->mht_small_buckets;
    mht->mht_نقاب = MHT_INIT_سائز - 1;
}

/*
 * Free the array of a hash table.  Does not free the items it contains!
 * The hash table must not be used again without another mf_hash_init() call.
 */
    ساکن خالی
mf_hash_free(mf_hashtab_T *mht)
{
    اگر (mht->mht_buckets != mht->mht_small_buckets)
	vim_free(mht->mht_buckets);
}

/*
 * Free the array of a hash table and all the items it contains.
 */
    ساکن خالی
mf_hash_free_all(mf_hashtab_T *mht)
{
    long_u	    idx;
    mf_hashitem_T   *mhi;
    mf_hashitem_T   *اگلا;

    برائے (idx = 0; idx <= mht->mht_نقاب; idx++)
	برائے (mhi = mht->mht_buckets[idx]; mhi != تہی; mhi = اگلا)
	{
	    اگلا = mhi->mhi_اگلا;
	    vim_free(mhi);
	}

    mf_hash_free(mht);
}

/*
 * Find "key" in hashtable "mht".
 * Returns a pointer to a mf_hashitem_T or NULL if the item was not found.
 */
    ساکن mf_hashitem_T *
mf_hash_ڈھونڈو(mf_hashtab_T *mht, blocknr_T کلید)
{
    mf_hashitem_T   *mhi;

    mhi = mht->mht_buckets[کلید & mht->mht_نقاب];
    جب (mhi != تہی && mhi->mhi_کلید != کلید)
	mhi = mhi->mhi_اگلا;

    واپس mhi;
}

/*
 * Add item "mhi" to hashtable "mht".
 * "mhi" must not be NULL.
 */
    ساکن خالی
mf_hash_جوڑو_شے(mf_hashtab_T *mht, mf_hashitem_T *mhi)
{
    long_u	    idx;

    idx = mhi->mhi_کلید & mht->mht_نقاب;
    mhi->mhi_اگلا = mht->mht_buckets[idx];
    mhi->mhi_پچھلا = تہی;
    اگر (mhi->mhi_اگلا != تہی)
	mhi->mhi_اگلا->mhi_پچھلا = mhi;
    mht->mht_buckets[idx] = mhi;

    mht->mht_گنتی++;

    /*
     * Grow hashtable when we have more thank 2^MHT_LOG_LOAD_FACTOR
     * items per bucket on average
     */
    اگر (mht->mht_fixed == 0
	&& (mht->mht_گنتی >> MHT_LOG_بھرو_ضربی_عدد) > mht->mht_نقاب)
    {
	اگر (mf_hash_grow(mht) == FAIL)
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
    ساکن خالی
mf_hash_rem_شے(mf_hashtab_T *mht, mf_hashitem_T *mhi)
{
    اگر (mhi->mhi_پچھلا == تہی)
	mht->mht_buckets[mhi->mhi_کلید & mht->mht_نقاب] = mhi->mhi_اگلا;
    ورنہ
	mhi->mhi_پچھلا->mhi_اگلا = mhi->mhi_اگلا;

    اگر (mhi->mhi_اگلا != تہی)
	mhi->mhi_اگلا->mhi_پچھلا = mhi->mhi_پچھلا;

    mht->mht_گنتی--;

    // We could shrink the table here, but it typically takes little memory,
    // so why bother?
}

/*
 * Increase number of buckets in the hashtable by MHT_GROWTH_FACTOR and
 * rehash items.
 * Returns FAIL when out of memory.
 */
    ساکن صحیح
mf_hash_grow(mf_hashtab_T *mht)
{
    long_u	    i, j;
    صحیح		    سرکاؤ;
    mf_hashitem_T   *mhi;
    mf_hashitem_T   *tails[MHT_GROWTH_ضربی_عدد];
    mf_hashitem_T   **buckets;
    size_t	    سائز;

    سائز = (mht->mht_نقاب + 1) * MHT_GROWTH_ضربی_عدد * حجم(خالی *);
    buckets = lalloc_clear(سائز, FALSE);
    اگر (buckets == تہی)
	واپس FAIL;

    سرکاؤ = 0;
    جب ((mht->mht_نقاب >> سرکاؤ) != 0)
	سرکاؤ++;

    برائے (i = 0; i <= mht->mht_نقاب; i++)
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

	CLEAR_میدان(tails);

	برائے (mhi = mht->mht_buckets[i]; mhi != تہی; mhi = mhi->mhi_اگلا)
	{
	    j = (mhi->mhi_کلید >> سرکاؤ) & (MHT_GROWTH_ضربی_عدد - 1);
	    اگر (tails[j] == تہی)
	    {
		buckets[i + (j << سرکاؤ)] = mhi;
		tails[j] = mhi;
		mhi->mhi_پچھلا = تہی;
	    }
	    ورنہ
	    {
		tails[j]->mhi_اگلا = mhi;
		mhi->mhi_پچھلا = tails[j];
		tails[j] = mhi;
	    }
	}

	برائے (j = 0; j < MHT_GROWTH_ضربی_عدد; j++)
	    اگر (tails[j] != تہی)
		tails[j]->mhi_اگلا = تہی;
    }

    اگر (mht->mht_buckets != mht->mht_small_buckets)
	vim_free(mht->mht_buckets);

    mht->mht_buckets = buckets;
    mht->mht_نقاب = (mht->mht_نقاب + 1) * MHT_GROWTH_ضربی_عدد - 1;

    واپس OK;
}
