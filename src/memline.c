/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

// for debugging
// #define CHECK(c, s)	do { if (c) emsg((s)); } while (0)
#define CHECK(c, s)	செய் { /**/ } போது (0)

/*
 * memline.c: Contains the functions for appending, deleting and changing the
 * text lines. The memfile functions are used to store the information in
 * blocks of memory, backed up by a file. The structure of the information is
 * a tree.  The root of the tree is a pointer block. The leaves of the tree
 * are data blocks. In between may be several layers of pointer blocks,
 * forming branches.
 *
 * Three types of blocks are used:
 * - Block nr 0 contains information for recovery
 * - Pointer blocks contain list of pointers to other blocks.
 * - Data blocks contain the actual text.
 *
 * Block nr 0 contains the block0 structure (see below).
 *
 * Block nr 1 is the first pointer block. It is the root of the tree.
 * Other pointer blocks are branches.
 *
 *  If a line is too big to fit in a single page, the block containing that
 *  line is made big enough to hold the line. It may span several pages.
 *  Otherwise all blocks are one page.
 *
 *  A data block that was filled when starting to edit a file and was not
 *  changed since then, can have a negative block number. This means that it
 *  has not yet been assigned a place in the file. When recovering, the lines
 *  in this data block can be read from the original file. When the block is
 *  changed (lines appended/deleted/changed) or when it is flushed it gets a
 *  positive number. Use mf_trans_del() to get the new number, before calling
 *  mf_get().
 */

#அந்தர்பாவ "vim.h"

#அந்தர்பாவ <time.h>

#எனில் defined(SASC) || defined(__amigaos4__)
# include <proto/dos.h>	    // for Open() and Close()
#endif

வகைவரை கட்டமைப்பு block0		ZERO_BL;    // contents of the first block
வகைவரை கட்டமைப்பு pointer_block	PTR_BL;	    // contents of a pointer block
வகைவரை கட்டமைப்பு data_block	DATA_BL;    // contents of a data block
வகைவரை கட்டமைப்பு pointer_entry	PTR_EN;	    // block/line-count pair

#define DATA_ID	       (('d' << 8) + 'a')   // data block id
#define PTR_ID	       (('p' << 8) + 't')   // pointer block id
#define BLOCK0_ID0     'b'		    // block 0 id 0
#define BLOCK0_ID1     '0'		    // block 0 id 1
#define BLOCK0_ID1_C0  'c'		    // block 0 id 1 'cm' 0
#define BLOCK0_ID1_C1  'C'		    // block 0 id 1 'cm' 1
#define BLOCK0_ID1_C2  'd'		    // block 0 id 1 'cm' 2
// BLOCK0_ID1_C3 and BLOCK0_ID1_C4 are for libsodium encryption.  However, for
// these the swapfile is disabled, thus they will not be used.  Added for
// consistency anyway.
#define BLOCK0_ID1_C3  'S'		    // block 0 id 1 'cm' 3
#define BLOCK0_ID1_C4  's'		    // block 0 id 1 'cm' 4

#எனில் defined(FEAT_CRYPT)
நிலையான மாறிலி முழு id1_codes[] = {
    BLOCK0_ID1_C0,  // CRYPT_M_ZIP
    BLOCK0_ID1_C1,  // CRYPT_M_BF
    BLOCK0_ID1_C2,  // CRYPT_M_BF2
    BLOCK0_ID1_C3,  // CRYPT_M_SOD  - Unused!
    BLOCK0_ID1_C4,  // CRYPT_M_SOD2  - Unused!
};
#endif

/*
 * pointer to a block, used in a pointer block
 */
கட்டமைப்பு pointer_entry
{
    blocknr_T	pe_bnum;	// block number
    linenr_T	pe_வரி_மொத்தம்;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    முழு		pe_page_மொத்தம்;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
கட்டமைப்பு pointer_block
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_மொத்தம்;	// number of pointers in this block
    short_u	pb_மொத்தம்_max;	// maximum value for pb_count
    PTR_EN	pb_pointer[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#define PB_மொத்தம்_MAX(mfp) (short_u)(((mfp)->mf_page_size - offsetof(PTR_BL, pb_pointer)) / அளவு(PTR_EN))

/*
 * A data block is a leaf in the tree.
 *
 * The text of the lines is at the end of the block. The text of the first line
 * in the block is put at the end, the text of the second line in front of it,
 * etc. Thus the order of the lines is the opposite of the line number.
 */
கட்டமைப்பு data_block
{
    short_u	db_id;		// ID for data block: DATA_ID
    unsigned	db_free;	// free space available
    unsigned	db_txt_start;	// byte where text starts
    unsigned	db_txt_end;	// byte just after data block
    linenr_T	db_வரி_மொத்தம்;	// number of lines in this block
    unsigned	db_index[1];	// index for start of line (actually bigger)
				// followed by empty space up to db_txt_start
				// followed by the text in the lines until
				// end of page
};

/*
 * The low bits of db_index hold the actual index. The topmost bit is
 * used for the global command to be able to mark a line.
 * This method is not clean, but otherwise there would be at least one extra
 * byte used for each line.
 * The mark has to be in this place to keep it with the correct line when other
 * lines are inserted or deleted.
 */
#define DB_MARKED	((unsigned)1 << ((அளவு(unsigned) * 8) - 1))
#define DB_INDEX_MASK	(~DB_MARKED)

#define INDEX_SIZE  (அளவு(unsigned))	    // size of one db_index entry
#define HEADER_SIZE (offsetof(DATA_BL, db_index))  // size of data block header

#define B0_FNAME_SIZE_ORG	900	// what it was in older versions
#define B0_FNAME_SIZE_NOCRYPT	898	// 2 bytes used for other things
#define B0_FNAME_SIZE_CRYPT	890	// 10 bytes used for other things
#define B0_UNAME_SIZE		40
#define B0_HNAME_SIZE		40
/*
 * Restrict the numbers to 32 bits, otherwise most compilers will complain.
 * This won't detect a 64 bit machine that only swaps a byte in the top 32
 * bits, but that is crazy anyway.
 */
#define B0_MAGIC_LONG	0x30313233L
#define B0_MAGIC_INT	0x20212223L
#define B0_MAGIC_SHORT	0x10111213L
#define B0_MAGIC_CHAR	0x55

/*
 * Block zero holds all info about the swap file.
 *
 * NOTE: DEFINITION OF BLOCK 0 SHOULD NOT CHANGE! It would make all existing
 * swap files unusable!
 *
 * If size of block0 changes anyway, adjust MIN_SWAP_PAGE_SIZE in vim.h!!
 *
 * This block is built up of single bytes, to make it portable across
 * different machines. b0_magic_* is used to check the byte order and size of
 * variables, because the rest of the swap file is not portable.
 */
கட்டமைப்பு block0
{
    char_u	b0_id[2];	// id for block 0: BLOCK0_ID0 and BLOCK0_ID1,
				// BLOCK0_ID1_C0, BLOCK0_ID1_C1, etc.
    char_u	b0_version[10];	// Vim version string
    char_u	b0_page_size[4];// number of bytes per page
    char_u	b0_mtime[4];	// last modification time of file
    char_u	b0_ino[4];	// inode of b0_fname
    char_u	b0_pid[4];	// process id of creator (or 0)
    char_u	b0_uname[B0_UNAME_SIZE]; // name of user (uid if no name)
    char_u	b0_hname[B0_HNAME_SIZE]; // host name (if it has a name)
    char_u	b0_fname[B0_FNAME_SIZE_ORG]; // name of file being edited
    long	b0_magic_long;	// check for byte order of long
    முழு		b0_magic_int;	// check for byte order of int
    short	b0_magic_short;	// check for byte order of short
    char_u	b0_magic_char;	// check for last char
};

/*
 * Note: b0_dirty and b0_flags are put at the end of the file name.  For very
 * long file names in older versions of Vim they are invalid.
 * The 'fileencoding' comes before b0_flags, with a NUL in front.  But only
 * when there is room, for very long file names it's omitted.
 */
#define B0_DIRTY	0x55
#define b0_dirty	b0_fname[B0_FNAME_SIZE_ORG - 1]

/*
 * The b0_flags field is new in Vim 7.0.
 */
#define b0_flags	b0_fname[B0_FNAME_SIZE_ORG - 2]

/*
 * Crypt seed goes here, 8 bytes.  New in Vim 7.3.
 * Without encryption these bytes may be used for 'fenc'.
 */
#define b0_seed		b0_fname[B0_FNAME_SIZE_ORG - 2 - MF_SEED_LEN]

// The lowest two bits contain the fileformat.  Zero means it's not set
// (compatible with Vim 6.x), otherwise it's EOL_UNIX + 1, EOL_DOS + 1 or
// EOL_MAC + 1.
#define B0_FF_MASK	3

// Swap file is in directory of edited file.  Used to find the file from
// different mount points.
#define B0_SAME_DIR	4

// The 'fileencoding' is at the end of b0_fname[], with a NUL in front of it.
// When empty there is only the NUL.
#define B0_HAS_FENC	8

#define அடுக்கு_INCR	5	// nr of entries added to ml_stack at a time

/*
 * The line number where the first mark may be is remembered.
 * If it is 0 there are no marks at all.
 * (always used for the current buffer only, no buffer change possible while
 * executing a global command).
 */
நிலையான linenr_T	lowest_marked = 0;

/*
 * arguments for ml_find_line()
 */
#define ML_DELETE	0x11	    // delete line
#define ML_செருகு	0x12	    // insert line
#define ML_FIND		0x13	    // just find the line
#define ML_FLUSH	0x02	    // flush locked block
#define ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
வகைவரை எண்ணிக்கை {
      UB_FNAME = 0	// update timestamp and filename
    , UB_SAME_DIR       // update the B0_SAME_DIR flag
    , UB_CRYPT		// update crypt key
} upd_block0_T;

#ifdef FEAT_CRYPT
நிலையான வெறுமை ml_set_b0_crypt(buf_T *buf, ZERO_BL *b0p);
#endif
நிலையான வெறுமை ml_upd_block0(buf_T *buf, upd_block0_T what);
நிலையான வெறுமை set_b0_fname(ZERO_BL *, buf_T *buf);
நிலையான வெறுமை set_b0_dir_flag(ZERO_BL *b0p, buf_T *buf);
நிலையான வெறுமை add_b0_fenc(ZERO_BL *b0p, buf_T *buf);
நிலையான time_t swapfile_info(char_u *);
நிலையான முழு recov_file_names(char_u **, char_u *, முழு prepend_dot);
நிலையான char_u *findswapname(buf_T *, char_u **, char_u *);
நிலையான வெறுமை ml_flush_வரி(buf_T *);
நிலையான bhdr_T *ml_new_data(memfile_T *, முழு, முழு);
நிலையான bhdr_T *ml_new_ptr(memfile_T *);
நிலையான bhdr_T *ml_find_வரி(buf_T *, linenr_T, முழு);
நிலையான முழு ml_add_அடுக்கு(buf_T *);
நிலையான வெறுமை ml_lineadd(buf_T *, முழு);
நிலையான முழு b0_magic_wrong(ZERO_BL *);
#ifdef CHECK_INODE
நிலையான முழு fnamecmp_ino(char_u *, char_u *, long);
#endif
நிலையான வெறுமை long_to_char(long, char_u *);
நிலையான long char_to_long(char_u *);
#ifdef FEAT_CRYPT
நிலையான cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T offset, முழு reading);
#endif
#ifdef FEAT_BYTEOFF
நிலையான வெறுமை ml_updatechunk(buf_T *buf, long வரி, long len, முழு updtype);
#endif

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
    முழு
ml_open(buf_T *buf)
{
    memfile_T	*mfp;
    bhdr_T	*hp = வெற்று;
    ZERO_BL	*b0p;
    PTR_BL	*pp;
    DATA_BL	*dp;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_அடுக்கு_size = 0; // no stack yet
    buf->b_ml.ml_அடுக்கு = வெற்று;	// no stack yet
    buf->b_ml.ml_அடுக்கு_உச்சி = 0;	// nothing in the stack
    buf->b_ml.ml_locked = வெற்று;	// no cached block
    buf->b_ml.ml_வரி_lnum = 0;	// no cached line
#ifdef FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = வெற்று;
    buf->b_ml.ml_usedchunks = 0;
#endif

    எனில் (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    எனில் (p_uc && buf->b_p_swf)
	buf->b_may_swap = true;
    இல்லையெனில்
	buf->b_may_swap = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(வெற்று, 0);
    எனில் (mfp == வெற்று)
	செல் error;

    buf->b_ml.ml_mfp = mfp;
#ifdef FEAT_CRYPT
    mfp->mf_buffer = buf;
#endif
    buf->b_ml.ml_flags = ML_EMPTY;
    buf->b_ml.ml_வரி_மொத்தம் = 1;

/*
 * fill block0 struct and write page 0
 */
    எனில் ((hp = mf_new(mfp, FALSE, 1)) == வெற்று)
	செல் error;
    எனில் (hp->bh_bnum != 0)
    {
	iemsg(e_didnt_get_block_nr_zero);
	செல் error;
    }
    b0p = (ZERO_BL *)(hp->bh_data);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (long)B0_MAGIC_LONG;
    b0p->b0_magic_int = (முழு)B0_MAGIC_INT;
    b0p->b0_magic_short = (short)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_version, "VIM ", 4);
    STRNCPY(b0p->b0_version + 4, Version, 6);
    long_to_char((long)mfp->mf_page_size, b0p->b0_page_size);

#ifdef FEAT_SPELL
    எனில் (!buf->b_spell)
#endif
    {
	b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	b0p->b0_flags = get_fileformat(buf) + 1;
	set_b0_fname(b0p, buf);
	(வெறுமை)get_user_name(b0p->b0_uname, B0_UNAME_SIZE);
	b0p->b0_uname[B0_UNAME_SIZE - 1] = NUL;
	mch_get_host_name(b0p->b0_hname, B0_HNAME_SIZE);
	b0p->b0_hname[B0_HNAME_SIZE - 1] = NUL;
	long_to_char(mch_get_pid(), b0p->b0_pid);
#ifdef FEAT_CRYPT
	ml_set_b0_crypt(buf, b0p);
#endif
    }

    /*
     * Always sync block number 0 to disk, so we can check the file name in
     * the swap file in findswapname(). Don't do this for a help files or
     * a spell buffer though.
     * Only works when there's a swapfile, otherwise it's done when the file
     * is created.
     */
    mf_put(mfp, hp, TRUE, FALSE);
    எனில் (!buf->b_help && !B_SPELL(buf))
	(வெறுமை)mf_sync(mfp, 0);

    /*
     * Fill in root pointer block and write page 1.
     */
    எனில் ((hp = ml_new_ptr(mfp)) == வெற்று)
	செல் error;
    எனில் (hp->bh_bnum != 1)
    {
	iemsg(e_didnt_get_block_nr_one);
	செல் error;
    }
    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_மொத்தம் = 1;
    pp->pb_pointer[0].pe_bnum = 2;
    pp->pb_pointer[0].pe_page_மொத்தம் = 1;
    pp->pb_pointer[0].pe_old_lnum = 1;
    pp->pb_pointer[0].pe_வரி_மொத்தம் = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    எனில் ((hp = ml_new_data(mfp, FALSE, 1)) == வெற்று)
	செல் error;
    எனில் (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_block_nr_two);
	செல் error;
    }

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[0] = --dp->db_txt_start;	// at end of block
    dp->db_free -= 1 + INDEX_SIZE;
    dp->db_வரி_மொத்தம் = 1;
    *((char_u *)dp + dp->db_txt_start) = NUL;	// empty line

    திரும்பு OK;

error:
    எனில் (mfp != வெற்று)
    {
	எனில் (hp)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, TRUE);	    // will also free(mfp->mf_fname)
    }
    buf->b_ml.ml_mfp = வெற்று;
    திரும்பு FAIL;
}

#எனில் defined(FEAT_CRYPT)
/*
 * Swapfile encryption is not supported by XChaCha20.  If this crypt method is
 * used then disable the swapfile, to avoid plain text being written to disk,
 * and return TRUE.
 * Otherwise return FALSE.
 */
    நிலையான முழு
crypt_may_close_swapfile(buf_T *buf, char_u *சாவி, முழு method)
{
    எனில் (crypt_method_is_sodium(method) && *சாவி != NUL)
    {
	mf_close_file(buf, TRUE);
	buf->b_p_swf = FALSE;
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Prepare encryption for "buf" for the current key and method.
 */
    நிலையான வெறுமை
ml_set_mfp_crypt(buf_T *buf)
{
    எனில் (*buf->b_p_சாவி == NUL)
	திரும்பு;

    முழு method_nr = crypt_get_method_nr(buf);

    எனில் (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_seed(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN, வெற்று, 0);
    }
# ifdef FEAT_SODIUM
    இல்லையெனில் எனில் (crypt_method_is_sodium(method_nr))
	crypt_sodium_randombytes_buf(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN);
# endif
}

/*
 * Prepare encryption for "buf" with block 0 "b0p".
 * Note: should not be called with libsodium encryption, since xchacha20 does
 * not support swapfile encryption.
 */
    நிலையான வெறுமை
ml_set_b0_crypt(buf_T *buf, ZERO_BL *b0p)
{
    எனில் (*buf->b_p_சாவி == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    இல்லையெனில்
    {
	முழு method_nr = crypt_get_method_nr(buf);

	b0p->b0_id[1] = id1_codes[method_nr];
	எனில் (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
	{
	    // Generate a seed and store it in block 0 and in the memfile.
	    sha2_seed(&b0p->b0_seed, MF_SEED_LEN, வெற்று, 0);
	    mch_memmove(buf->b_ml.ml_mfp->mf_seed, &b0p->b0_seed, MF_SEED_LEN);
	}
    }
}

/*
 * Called after the crypt key or 'cryptmethod' was changed for "buf".
 * Will apply this to the swapfile.
 * "old_key" is the previous key.  It is equal to buf->b_p_key when
 * 'cryptmethod' is changed.
 * "old_cm" is the previous 'cryptmethod'.  It is equal to the current
 * 'cryptmethod' when 'key' is changed.
 */
    வெறுமை
ml_set_crypt_சாவி(
    buf_T	*buf,
    char_u	*old_சாவி,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    முழு		page_மொத்தம்;
    முழு		idx;
    long	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    DATA_BL	*dp;
    blocknr_T	bnum;
    முழு		உச்சி;
    முழு		old_method;

    எனில் (mfp == வெற்று || mfp->mf_fd < 0)
	திரும்பு;  // no memfile yet, nothing to do
    old_method = crypt_method_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    எனில் (crypt_may_close_swapfile(buf, buf->b_p_சாவி, crypt_get_method_nr(buf)))
	திரும்பு;
# endif

    // First make sure the swapfile is in a consistent state, using the old
    // key and method.
    {
	char_u *new_சாவி = buf->b_p_சாவி;
	char_u *new_buf_cm = buf->b_p_cm;

	buf->b_p_சாவி = old_சாவி;
	buf->b_p_cm = old_cm;
	ml_preserve(buf, FALSE);
	buf->b_p_சாவி = new_சாவி;
	buf->b_p_cm = new_buf_cm;
    }

    // Set the key, method and seed to be used for reading, these must be the
    // old values.
    mfp->mf_old_சாவி = old_சாவி;
    mfp->mf_old_cm = old_method;
    எனில் (old_method > 0 && *old_சாவி != NUL)
	mch_memmove(mfp->mf_old_seed, mfp->mf_seed, MF_SEED_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    எனில் (mfp->mf_infile_மொத்தம் > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_வரி(buf);		    // flush buffered line
	(வெறுமை)ml_find_வரி(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = வெற்று;
	bnum = 1;		// start with block 1
	page_மொத்தம் = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_அடுக்கு_உச்சி = 0;
	VIM_CLEAR(buf->b_ml.ml_அடுக்கு);
	buf->b_ml.ml_அடுக்கு_size = 0;	// no stack yet

	க்கு ( ; !got_int; வரி_breakcheck())
	{
	    எனில் (hp != வெற்று)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    எனில் ((hp = mf_get(mfp, bnum, page_மொத்தம்)) == வெற்று)
	    {
		எனில் (bnum == 1)
		    நிறுத்து;
		++error;
	    }
	    இல்லையெனில்
	    {
		pp = (PTR_BL *)(hp->bh_data);
		எனில் (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    எனில் (pp->pb_மொத்தம் == 0)
		    {
			// empty block?
			++error;
		    }
		    இல்லையெனில் எனில் (idx < (முழு)pp->pb_மொத்தம்)	// go a block deeper
		    {
			எனில் (pp->pb_pointer[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    தொடர்;
			}

			// going one block deeper in the tree, new entry in
			//அடுக்கு
			எனில் ((உச்சி = ml_add_அடுக்கு(buf)) < 0)
			{
			    ++error;
			    நிறுத்து;		    // out of memory
			}
			ip = &(buf->b_ml.ml_அடுக்கு[உச்சி]);
			ip->ip_bnum = bnum;
			ip->ip_index = idx;

			bnum = pp->pb_pointer[idx].pe_bnum;
			page_மொத்தம் = pp->pb_pointer[idx].pe_page_மொத்தம்;
			idx = 0;
			தொடர்;
		    }
		}
		இல்லையெனில்	    // not a pointer block
		{
		    dp = (DATA_BL *)(hp->bh_data);
		    எனில் (dp->db_id != DATA_ID)	// block id wrong
			++error;
		    இல்லையெனில்
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = வெற்று;
		    }
		}
	    }

	    எனில் (buf->b_ml.ml_அடுக்கு_உச்சி == 0)	// finished
		நிறுத்து;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_அடுக்கு[--(buf->b_ml.ml_அடுக்கு_உச்சி)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_index + 1;	    // go to next index
	    page_மொத்தம் = 1;
	}
	எனில் (hp != வெற்று)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	எனில் (error > 0)
	    emsg(_(e_error_while_updating_swap_file_crypt));
    }

    mfp->mf_old_சாவி = வெற்று;
}
#endif

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
    வெறுமை
ml_setname(buf_T *buf)
{
    முழு		success = FALSE;
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;
#எனில் defined(MSWIN)
    char_u	*p;
#endif

    mfp = buf->b_ml.ml_mfp;
    எனில் (mfp->mf_fd < 0)		    // there is no swap file yet
    {
	/*
	 * When 'updatecount' is 0 and 'noswapfile' there is no swap file.
	 * For help files we will make a swap file now.
	 */
	எனில் (p_uc != 0 && (cmdmod.cmod_flags & CMOD_NOSWAPFILE) == 0)
	    ml_open_file(buf);	    // create a swap file
	திரும்பு;
    }

    /*
     * Try all directories in the 'directory' option.
     */
    dirp = p_dir;
    க்கு (;;)
    {
	எனில் (*dirp == NUL)	    // tried all directories, fail
	    நிறுத்து;
	fname = findswapname(buf, &dirp, mfp->mf_fname);
	// autocmd may have freed mfp if findswapname creates
	// different swapfile name
	எனில் (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    திரும்பு;
	}
						    // alloc's fname
	எனில் (dirp == வெற்று)	    // out of memory
	    நிறுத்து;
	எனில் (fname == வெற்று)	    // no file name found for this dir
	    தொடர்;

#எனில் defined(MSWIN)
	/*
	 * Set full pathname for swap file now, because a ":!cd dir" may
	 * change directory without us knowing it.
	 */
	p = FullName_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	எனில் (fname == வெற்று)
	    தொடர்;
#endif
	// if the file name is the same we don't have to do anything
	எனில் (fnamecmp(fname, mfp->mf_fname) == 0)
	{
	    vim_free(fname);
	    success = TRUE;
	    நிறுத்து;
	}
	// need to close the swap file before renaming
	எனில் (mfp->mf_fd >= 0)
	{
	    close(mfp->mf_fd);
	    mfp->mf_fd = -1;
	}

	// try to rename the swap file
	எனில் (vim_rename(mfp->mf_fname, fname) == 0)
	{
	    success = TRUE;
	    vim_free(mfp->mf_fname);
	    mfp->mf_fname = fname;
	    vim_free(mfp->mf_ffname);
#எனில் defined(MSWIN)
	    mfp->mf_ffname = வெற்று;  // mf_fname is full pathname already
#இல்லையெனில்
	    mf_set_ffname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);
	    நிறுத்து;
	}
	vim_free(fname);	    // this fname didn't work, try another
    }

    எனில் (mfp->mf_fd == -1)	    // need to (re)open the swap file
    {
	mfp->mf_fd = mch_open((எழுத்து *)mfp->mf_fname, O_RDWR | O_EXTRA, 0);
	எனில் (mfp->mf_fd < 0)
	{
	    // could not (re)open the swap file, what can we do????
	    emsg(_(e_oops_lost_the_swap_file));
	    திரும்பு;
	}
#ifdef HAVE_FD_CLOEXEC
	{
	    முழு fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    எனில் (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(வெறுமை)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#endif
    }
    எனில் (!success)
	emsg(_(e_could_not_rename_swap_file));
}

/*
 * Open a file for the memfile for all buffers that are not readonly or have
 * been modified.
 * Used when 'updatecount' changes from zero to non-zero.
 */
    வெறுமை
ml_open_files(வெறுமை)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	எனில் (!buf->b_p_ro || buf->b_changed)
	    ml_open_file(buf);
}

/*
 * Open a swap file for an existing memfile, if there is no swap file yet.
 * If we are unable to find a file name, mf_fname will be NULL
 * and the memfile will be in memory only (no recovery possible).
 */
    வெறுமை
ml_open_file(buf_T *buf)
{
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;

    mfp = buf->b_ml.ml_mfp;
    எனில் (mfp == வெற்று || mfp->mf_fd >= 0 || !buf->b_p_swf
				      || (cmdmod.cmod_flags & CMOD_NOSWAPFILE))
	திரும்பு;		// nothing to do

#ifdef FEAT_SPELL
    // For a spell buffer use a temp file name.
    எனில் (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	எனில் (fname != வெற்று)
	    (வெறுமை)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_swap = false;
	திரும்பு;
    }
#endif

    /*
     * Try all directories in 'directory' option.
     */
    dirp = p_dir;
    க்கு (;;)
    {
	எனில் (*dirp == NUL)
	    நிறுத்து;
	// There is a small chance that between choosing the swap file name
	// and creating it, another Vim creates the file.  In that case the
	// creation will fail and we will use another directory.
	fname = findswapname(buf, &dirp, வெற்று); // allocates fname
	// autocmd may have freed mfp, grr!
	எனில் (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    திரும்பு;
	}
	எனில் (dirp == வெற்று)
	    நிறுத்து;  // out of memory
	எனில் (fname == வெற்று)
	    தொடர்;
	எனில் (mf_open_file(mfp, fname) == OK)	// consumes fname!
	{
	    // don't sync yet in ml_sync_all()
	    mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;
#எனில் defined(MSWIN)
	    /*
	     * set full pathname for swap file now, because a ":!cd dir" may
	     * change directory without us knowing it.
	     */
	    mf_fullname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    எனில் (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_set_dirty(mfp);
		நிறுத்து;
	    }
	    // Writing block 0 failed: close the file and try another dir
	    mf_close_file(buf, FALSE);
	}
    }

    எனில் (*p_dir != NUL && mfp->mf_fname == வெற்று)
    {
	need_wait_return = TRUE;	// call wait_return() later
	++no_wait_return;
	(வெறுமை)semsg(_(e_unable_to_open_swap_file_for_str_recovery_impossible),
		    buf_spname(buf) != வெற்று ? buf_spname(buf) : buf->b_fname);
	--no_wait_return;
    }

    // don't try to open a swap file again
    buf->b_may_swap = false;
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
    வெறுமை
check_need_swap(
    முழு	    newfile)		// reading file into new buffer
{
    முழு old_செய்தி_silent = செய்தி_silent; // might be reset by an E325 message

    எனில் (curbuf->b_may_swap && (!curbuf->b_p_ro || !newfile))
	ml_open_file(curbuf);
    செய்தி_silent = old_செய்தி_silent;
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
    வெறுமை
ml_close(buf_T *buf, முழு del_file)
{
    எனில் (buf->b_ml.ml_mfp == வெற்று)		// not open
	திரும்பு;
    mf_close(buf->b_ml.ml_mfp, del_file);	// close the .swp file
    எனில் (buf->b_ml.ml_வரி_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_வரி_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_வரி_ptr);
    vim_free(buf->b_ml.ml_அடுக்கு);
#ifdef FEAT_BYTEOFF
    VIM_CLEAR(buf->b_ml.ml_chunksize);
#endif
    buf->b_ml.ml_mfp = வெற்று;

    // Reset the "recovered" flag, give the ATTENTION prompt the next time
    // this buffer is loaded.
    buf->b_flags &= ~BF_RECOVERED;
}

/*
 * Close all existing memlines and memfiles.
 * Only used when exiting.
 * When 'del_file' is TRUE, delete the memfiles.
 * But don't delete files that were ":preserve"d when we are POSIX compatible.
 */
    வெறுமை
ml_close_all(முழு del_file)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ml_close(buf, del_file && ((buf->b_flags & BF_PRESERVED) == 0
				 || vim_strchr(p_cpo, CPO_PRESERVE) == வெற்று));
#ifdef FEAT_SPELL
    spell_delete_wordlist();	// delete the internal wordlist
#endif
#ifdef TEMPDIRNAMES
    vim_deltempdir();		// delete created temp directory
#endif
}

/*
 * Close all memfiles for not modified buffers.
 * Only use just before exiting!
 */
    வெறுமை
ml_close_notmod(வெறுமை)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	எனில் (!bufIsChanged(buf))
	    ml_close(buf, TRUE);    // close all not-modified buffers
}

/*
 * Update the timestamp in the .swp file.
 * Used when the file has been written.
 */
    வெறுமை
ml_timestamp(buf_T *buf)
{
    ml_upd_block0(buf, UB_FNAME);
}

/*
 * Return FAIL when the ID of "b0p" is wrong.
 */
    நிலையான முழு
ml_check_b0_id(ZERO_BL *b0p)
{
    எனில் (b0p->b0_id[0] != BLOCK0_ID0
	    || (b0p->b0_id[1] != BLOCK0_ID1
		&& b0p->b0_id[1] != BLOCK0_ID1_C0
		&& b0p->b0_id[1] != BLOCK0_ID1_C1
		&& b0p->b0_id[1] != BLOCK0_ID1_C2
		&& b0p->b0_id[1] != BLOCK0_ID1_C3
		&& b0p->b0_id[1] != BLOCK0_ID1_C4)
	    )
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Update the timestamp or the B0_SAME_DIR flag of the .swp file.
 */
    நிலையான வெறுமை
ml_upd_block0(buf_T *buf, upd_block0_T what)
{
    memfile_T	*mfp;
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    mfp = buf->b_ml.ml_mfp;
    எனில் (mfp == வெற்று)
	திரும்பு;
    hp = mf_get(mfp, (blocknr_T)0, 1);
    எனில் (hp == வெற்று)
    {
#ifdef FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	எனில் (what == UB_CRYPT)
	    ml_set_mfp_crypt(buf);
#endif
	திரும்பு;
    }

    b0p = (ZERO_BL *)(hp->bh_data);
    எனில் (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_block_zero);
    இல்லையெனில்
    {
	எனில் (what == UB_FNAME)
	    set_b0_fname(b0p, buf);
#ifdef FEAT_CRYPT
	இல்லையெனில் எனில் (what == UB_CRYPT)
	    ml_set_b0_crypt(buf, b0p);
#endif
	இல்லையெனில் // what == UB_SAME_DIR
	    set_b0_dir_flag(b0p, buf);
    }
    mf_put(mfp, hp, TRUE, FALSE);
}

/*
 * Write file name and timestamp into block 0 of a swap file.
 * Also set buf->b_mtime.
 * Don't use NameBuff[]!!!
 */
    நிலையான வெறுமை
set_b0_fname(ZERO_BL *b0p, buf_T *buf)
{
    stat_T	st;

    எனில் (buf->b_ffname == வெற்று)
	b0p->b0_fname[0] = NUL;
    இல்லையெனில்
    {
#எனில் defined(MSWIN) || defined(AMIGA)
	// Systems that cannot translate "~user" back into a path: copy the
	// file name unmodified.  Do use slashes instead of backslashes for
	// portability.
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_SIZE_CRYPT - 1);
# ifdef BACKSLASH_IN_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#இல்லையெனில்
	size_t	flen;
	char_u	uname[B0_UNAME_SIZE];

	/*
	 * For a file under the home directory of the current user, we try to
	 * replace the home directory path with "~user". This helps when
	 * editing the same file on different machines over a network.
	 * First replace home dir path with "~/" with home_replace().
	 * Then insert the user name to get "~user/".
	 */
	flen = home_replace(வெற்று, buf->b_ffname, b0p->b0_fname,
						   B0_FNAME_SIZE_CRYPT, TRUE);
	எனில் (b0p->b0_fname[0] == '~')
	{
	    size_t  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    எனில் (get_user_name(uname, B0_UNAME_SIZE) == FAIL
		   || (ulen = STRLEN(uname)) + flen > B0_FNAME_SIZE_CRYPT - 1)
		vim_strncpy(b0p->b0_fname, buf->b_ffname,
						     B0_FNAME_SIZE_CRYPT - 1);
	    இல்லையெனில்
	    {
		mch_memmove(b0p->b0_fname + ulen + 1, b0p->b0_fname + 1, flen);
		mch_memmove(b0p->b0_fname + 1, uname, ulen);
	    }
	}
#endif
	எனில் (mch_stat((எழுத்து *)buf->b_ffname, &st) >= 0)
	{
	    long_to_char((long)st.st_mtime, b0p->b0_mtime);
#ifdef CHECK_INODE
	    long_to_char((long)st.st_ino, b0p->b0_ino);
#endif
	    buf_store_time(buf, &st, buf->b_ffname);
	    buf->b_mtime_read = buf->b_mtime;
	    buf->b_mtime_read_ns = buf->b_mtime_ns;
	}
	இல்லையெனில்
	{
	    long_to_char(0L, b0p->b0_mtime);
#ifdef CHECK_INODE
	    long_to_char(0L, b0p->b0_ino);
#endif
	    buf->b_mtime = 0;
	    buf->b_mtime_ns = 0;
	    buf->b_mtime_read = 0;
	    buf->b_mtime_read_ns = 0;
	    buf->b_orig_size = 0;
	    buf->b_orig_mode = 0;
	}
    }

    // Also add the 'fileencoding' if there is room.
    add_b0_fenc(b0p, curbuf);
}

/*
 * Update the B0_SAME_DIR flag of the swap file.  It's set if the file and the
 * swapfile for "buf" are in the same directory.
 * This is fail safe: if we are not sure the directories are equal the flag is
 * not set.
 */
    நிலையான வெறுமை
set_b0_dir_flag(ZERO_BL *b0p, buf_T *buf)
{
    எனில் (same_directory(buf->b_ml.ml_mfp->mf_fname, buf->b_ffname))
	b0p->b0_flags |= B0_SAME_DIR;
    இல்லையெனில்
	b0p->b0_flags &= ~B0_SAME_DIR;
}

/*
 * When there is room, add the 'fileencoding' to block zero.
 */
    நிலையான வெறுமை
add_b0_fenc(
    ZERO_BL	*b0p,
    buf_T	*buf)
{
    முழு		n;
    முழு		size = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    எனில் (*buf->b_p_சாவி != NUL)
	size = B0_FNAME_SIZE_CRYPT;
#endif

    n = (முழு)STRLEN(buf->b_p_fenc);
    எனில் ((முழு)STRLEN(b0p->b0_fname) + n + 1 > size)
	b0p->b0_flags &= ~B0_HAS_FENC;
    இல்லையெனில்
    {
	mch_memmove((எழுத்து *)b0p->b0_fname + size - n,
					    (எழுத்து *)buf->b_p_fenc, (size_t)n);
	*(b0p->b0_fname + size - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#எனில் defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#endif

#எனில் defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    நிலையான முழு
swapfile_process_running(ZERO_BL *b0p, char_u *swap_fname UNUSED)
{
# எனில் defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    கட்டமைப்பு sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    எனில் (mch_stat((எழுத்து *)swap_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < time(வெற்று) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	திரும்பு FALSE;
# endif
    திரும்பு mch_process_running(char_to_long(b0p->b0_pid));
}
#endif

/*
 * Try to recover curbuf from the .swp file.
 * If "checkext" is TRUE, check the extension and detect whether it is
 * a swap file.
 */
    வெறுமை
ml_recover(முழு checkext)
{
    buf_T	*buf = வெற்று;
    memfile_T	*mfp = வெற்று;
    char_u	*fname;
    char_u	*fname_used = வெற்று;
    bhdr_T	*hp = வெற்று;
    ZERO_BL	*b0p;
    முழு		b0_ff;
    char_u	*b0_fenc = வெற்று;
#ifdef FEAT_CRYPT
    முழு		b0_cm = -1;
#endif
    PTR_BL	*pp;
    DATA_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    முழு		page_மொத்தம்;
    stat_T	org_stat, swp_stat;
    முழு		len;
    முழு		directly;
    linenr_T	lnum;
    char_u	*p;
    முழு		i;
    long	error;
    முழு		cannot_open;
    linenr_T	வரி_மொத்தம்;
    முழு		has_error;
    முழு		idx;
    முழு		உச்சி;
    முழு		txt_start;
    off_T	size;
    முழு		called_from_main;
    முழு		serious_error = TRUE;
    long	mtime;
    முழு		attr;
    முழு		orig_file_status = NOTDONE;

    recoverymode = TRUE;
    called_from_main = (curbuf->b_ml.ml_mfp == வெற்று);
    attr = HL_ATTR(HLF_E);

    /*
     * If the file name ends in ".s[a-w][a-z]" we assume this is the swap file.
     * Otherwise a search is done to find the swap file(s).
     */
    fname = curbuf->b_fname;
    எனில் (fname == வெற்று)		    // When there is no file name
	fname = (char_u *)"";
    len = (முழு)STRLEN(fname);
    எனில் (checkext && len >= 4 &&
#எனில் defined(VMS)
	    STRNICMP(fname + len - 4, "_s", 2)
#இல்லையெனில்
	    STRNICMP(fname + len - 4, ".s", 2)
#endif
						== 0
		&& vim_strchr((char_u *)"abcdefghijklmnopqrstuvw",
					   TOLOWER_ASC(fname[len - 2])) != வெற்று
		&& ASCII_ISALPHA(fname[len - 1]))
    {
	directly = TRUE;
	fname_used = vim_strsave(fname); // make a copy for mf_open()
    }
    இல்லையெனில்
    {
	directly = FALSE;

	// count the number of matching swap files
	len = recover_names(fname, FALSE, வெற்று, 0, வெற்று);
	எனில் (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_swap_file_found_for_str), fname);
	    செல் theend;
	}
	எனில் (len == 1)		    // one swap file found, use it
	    i = 1;
	இல்லையெனில்			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (வெறுமை)recover_names(fname, TRUE, வெற்று, 0, வெற்று);
	    செய்தி_putchar('\n');
	    செய்தி_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_எண்(FALSE, வெற்று);
	    எனில் (i < 1 || i > len)
		செல் theend;
	}
	// get the swap file name that will be used
	(வெறுமை)recover_names(fname, FALSE, வெற்று, i, &fname_used);
    }
    எனில் (fname_used == வெற்று)
	செல் theend;			// out of memory

    // When called from main() still need to initialize storage structure
    எனில் (called_from_main && ml_open(curbuf) == FAIL)
	getout(1);

    /*
     * Allocate a buffer structure for the swap file that is used for recovery.
     * Only the memline and crypt information in it are really used.
     */
    buf = ALLOC_ONE(buf_T);
    எனில் (buf == வெற்று)
	செல் theend;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_அடுக்கு_size = 0;	// no stack yet
    buf->b_ml.ml_அடுக்கு = வெற்று;		// no stack yet
    buf->b_ml.ml_அடுக்கு_உச்சி = 0;		// nothing in the stack
    buf->b_ml.ml_வரி_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = வெற்று;		// no locked block
    buf->b_ml.ml_flags = 0;
#ifdef FEAT_CRYPT
    buf->b_p_சாவி = empty_option;
    buf->b_p_cm = empty_option;
#endif

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_used); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_used, O_RDONLY);
    fname_used = p;
    எனில் (mfp == வெற்று || mfp->mf_fd < 0)
    {
	எனில் (fname_used != வெற்று)
	    semsg(_(e_cannot_open_str), fname_used);
	செல் theend;
    }
    buf->b_ml.ml_mfp = mfp;
#ifdef FEAT_CRYPT
    mfp->mf_buffer = buf;
#endif

    /*
     * The page size set in mf_open() might be different from the page size
     * used in the swap file, we must get it from block 0.  But to read block
     * 0 we need a page size.  Use the minimal size for block 0 here, it will
     * be set to the real value below.
     */
    mfp->mf_page_size = MIN_SWAP_PAGE_SIZE;

    /*
     * try to read block 0
     */
    எனில் ((hp = mf_get(mfp, (blocknr_T)0, 1)) == வெற்று)
    {
	செய்தி_start();
	செய்தி_puts_attr(_("Unable to read block 0 from "), attr | செய்தி_HIST);
	செய்தி_outtrans_attr(mfp->mf_fname, attr | செய்தி_HIST);
	செய்தி_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | செய்தி_HIST);
	செய்தி_end();
	செல் theend;
    }
    b0p = (ZERO_BL *)(hp->bh_data);
    எனில் (STRNCMP(b0p->b0_version, "VIM 3.0", 7) == 0)
    {
	செய்தி_start();
	செய்தி_outtrans_attr(mfp->mf_fname, செய்தி_HIST);
	செய்தி_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    செய்தி_HIST);
	செய்தி_puts_attr(_("Use Vim version 3.0.\n"), செய்தி_HIST);
	செய்தி_end();
	செல் theend;
    }
    எனில் (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_swap_file), mfp->mf_fname);
	செல் theend;
    }
    எனில் (b0_magic_wrong(b0p))
    {
	செய்தி_start();
	செய்தி_outtrans_attr(mfp->mf_fname, attr | செய்தி_HIST);
#எனில் defined(MSWIN)
	எனில் (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    செய்தி_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | செய்தி_HIST);
	இல்லையெனில்
#endif
	    செய்தி_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | செய்தி_HIST);
	செய்தி_puts_attr(_("The file was created on "), attr | செய்தி_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	செய்தி_puts_attr((எழுத்து *)b0p->b0_hname, attr | செய்தி_HIST);
	செய்தி_puts_attr(_(",\nor the file has been damaged."), attr | செய்தி_HIST);
	செய்தி_end();
	செல் theend;
    }

#ifdef FEAT_CRYPT
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(id1_codes); ++i)
	எனில் (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    எனில் (b0_cm > 0)
	mch_memmove(mfp->mf_seed, &b0p->b0_seed, MF_SEED_LEN);
    crypt_set_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#இல்லையெனில்
    எனில் (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_version_of_vim_does_not_support_encryption), mfp->mf_fname);
	செல் theend;
    }
#endif

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    எனில் (mfp->mf_page_size != (unsigned)char_to_long(b0p->b0_page_size))
    {
	unsigned previous_page_size = mfp->mf_page_size;

	mf_new_page_size(mfp, (unsigned)char_to_long(b0p->b0_page_size));
	எனில் (mfp->mf_page_size < previous_page_size)
	{
	    செய்தி_start();
	    செய்தி_outtrans_attr(mfp->mf_fname, attr | செய்தி_HIST);
	    செய்தி_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | செய்தி_HIST);
	    செய்தி_end();
	    செல் theend;
	}
	எனில் ((size = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	இல்லையெனில்
	    mfp->mf_blocknr_max = (blocknr_T)(size / mfp->mf_page_size);
	mfp->mf_infile_மொத்தம் = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_page_size);
	எனில் (p == வெற்று)
	    செல் theend;
	mch_memmove(p, hp->bh_data, previous_page_size);
	vim_free(hp->bh_data);
	hp->bh_data = p;
	b0p = (ZERO_BL *)(hp->bh_data);
    }

    /*
     * If .swp file name given directly, use name from swap file for buffer.
     */
    எனில் (directly)
    {
	expand_env(b0p->b0_fname, NameBuff, MAXPATHL);
	எனில் (setfname(curbuf, NameBuff, வெற்று, TRUE) == FAIL)
	    செல் theend;
    }

    home_replace(வெற்று, mfp->mf_fname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Using swap file \"%s\""), NameBuff);

    எனில் (buf_spname(curbuf) != வெற்று)
	vim_strncpy(NameBuff, buf_spname(curbuf), MAXPATHL - 1);
    இல்லையெனில்
	home_replace(வெற்று, curbuf->b_ffname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Original file \"%s\""), NameBuff);
    செய்தி_putchar('\n');

    /*
     * check date of swap file and original file
     */
    mtime = char_to_long(b0p->b0_mtime);
    எனில் (curbuf->b_ffname != வெற்று
	    && mch_stat((எழுத்து *)curbuf->b_ffname, &org_stat) != -1
	    && ((mch_stat((எழுத்து *)mfp->mf_fname, &swp_stat) != -1
		    && org_stat.st_mtime > swp_stat.st_mtime)
		|| org_stat.st_mtime != mtime))
	emsg(_(e_warning_original_file_may_have_been_changed));
    out_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_MASK);
    எனில் (b0p->b0_flags & B0_HAS_FENC)
    {
	முழு fnsize = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	எனில் (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_SIZE_CRYPT;
#endif
	க்கு (p = b0p->b0_fname + fnsize; p > b0p->b0_fname && p[-1] != NUL; --p)
	    ;
	b0_fenc = vim_strnsave(p, b0p->b0_fname + fnsize - p);
    }

    mf_put(mfp, hp, FALSE, FALSE);	// release block 0
    hp = வெற்று;

    /*
     * Now that we are sure that the file is going to be recovered, clear the
     * contents of the current buffer.
     */
    போது (!(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    எனில் (curbuf->b_ffname != வெற்று)
	orig_file_status = readfile(curbuf->b_ffname, வெற்று, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, வெற்று, READ_NEW);

#ifdef FEAT_CRYPT
    எனில் (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	எனில் (*curbuf->b_p_சாவி != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_used);
	    செய்தி_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    செய்தி_puts(_("\nenter the new crypt key."));
	    செய்தி_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    செய்தி_puts(_("\nto use the same key for text file and swap file"));
	}
	இல்லையெனில்
	    smsg(_(need_சாவி_செய்தி), fname_used);
	buf->b_p_சாவி = crypt_get_சாவி(FALSE, FALSE);
	எனில் (buf->b_p_சாவி == வெற்று)
	    buf->b_p_சாவி = curbuf->b_p_சாவி;
	இல்லையெனில் எனில் (*buf->b_p_சாவி == NUL)
	{
	    vim_free(buf->b_p_சாவி);
	    buf->b_p_சாவி = curbuf->b_p_சாவி;
	}
	எனில் (buf->b_p_சாவி == வெற்று)
	    buf->b_p_சாவி = empty_option;
    }
#endif

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    எனில் (b0_ff != 0)
	set_fileformat(b0_ff - 1, OPT_LOCAL);
    எனில் (b0_fenc != வெற்று)
    {
	set_option_மதிப்பு_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    page_மொத்தம் = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    வரி_மொத்தம் = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_அடுக்கு_உச்சி = 0;
    buf->b_ml.ml_அடுக்கு = வெற்று;
    buf->b_ml.ml_அடுக்கு_size = 0;	// no stack yet

    எனில் (curbuf->b_ffname == வெற்று)
	cannot_open = TRUE;
    இல்லையெனில்
	cannot_open = FALSE;

    serious_error = FALSE;
    க்கு ( ; !got_int; வரி_breakcheck())
    {
	எனில் (hp != வெற்று)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	எனில் ((hp = mf_get(mfp, bnum, page_மொத்தம்)) == வெற்று)
	{
	    எனில் (bnum == 1)
	    {
		semsg(_(e_unable_to_read_block_one_from_str), mfp->mf_fname);
		செல் theend;
	    }
	    ++error;
	    ml_append(lnum++, (char_u *)_("???MANY LINES MISSING"),
							    (colnr_T)0, TRUE);
	}
	இல்லையெனில்		// there is a block
	{
	    pp = (PTR_BL *)(hp->bh_data);
	    எனில் (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		முழு ptr_block_error = FALSE;
		எனில் (pp->pb_மொத்தம்_max != PB_மொத்தம்_MAX(mfp))
		{
		    ptr_block_error = TRUE;
		    pp->pb_மொத்தம்_max = PB_மொத்தம்_MAX(mfp);
		}
		எனில் (pp->pb_மொத்தம் > pp->pb_மொத்தம்_max)
		{
		    ptr_block_error = TRUE;
		    pp->pb_மொத்தம் = pp->pb_மொத்தம்_max;
		}
		எனில் (ptr_block_error)
		    emsg(_(e_warning_pointer_block_corrupted));

		// check line count when using pointer block first time
		எனில் (idx == 0 && வரி_மொத்தம் != 0)
		{
		    க்கு (i = 0; i < (முழு)pp->pb_மொத்தம்; ++i)
			வரி_மொத்தம் -= pp->pb_pointer[i].pe_வரி_மொத்தம்;
		    எனில் (வரி_மொத்தம் != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		எனில் (pp->pb_மொத்தம் == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		இல்லையெனில் எனில் (idx < (முழு)pp->pb_மொத்தம்)	// go a block deeper
		{
		    எனில் (pp->pb_pointer[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			எனில் (!cannot_open)
			{
			    வரி_மொத்தம் = pp->pb_pointer[idx].pe_வரி_மொத்தம்;
			    linenr_T pe_old_lnum = pp->pb_pointer[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    எனில் (வரி_மொத்தம் <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, வெற்று, lnum,
					pe_old_lnum - 1,
					வரி_மொத்தம், வெற்று, 0) != OK)
				cannot_open = TRUE;
			    இல்லையெனில்
				lnum += வரி_மொத்தம்;
			}
			எனில் (cannot_open)
			{
			    ++error;
			    ml_append(lnum++, (char_u *)_("???LINES MISSING"),
							    (colnr_T)0, TRUE);
			}
			++idx;	    // get same block again for next index
			தொடர்;
		    }

		    /*
		     * going one block deeper in the tree
		     */
		    எனில் ((உச்சி = ml_add_அடுக்கு(buf)) < 0)	// new entry in stack
		    {
			++error;
			நிறுத்து;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_அடுக்கு[உச்சி]);
		    ip->ip_bnum = bnum;
		    ip->ip_index = idx;

		    bnum = pp->pb_pointer[idx].pe_bnum;
		    வரி_மொத்தம் = pp->pb_pointer[idx].pe_வரி_மொத்தம்;
		    page_மொத்தம் = pp->pb_pointer[idx].pe_page_மொத்தம்;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    எனில் (page_மொத்தம் < 1
			    || bnum + page_மொத்தம் > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_index + 1;
			bnum = ip->ip_bnum;
			page_மொத்தம் = 1;
			--buf->b_ml.ml_அடுக்கு_உச்சி;
			தொடர்;
		    }
		    idx = 0;
		    தொடர்;
		}
	    }
	    இல்லையெனில்	    // not a pointer block
	    {
		dp = (DATA_BL *)(hp->bh_data);
		எனில் (dp->db_id != DATA_ID)	// block id wrong
		{
		    எனில் (bnum == 1)
		    {
			semsg(_(e_block_one_id_wrong_str_not_swp_file),
							       mfp->mf_fname);
			செல் theend;
		    }
		    ++error;
		    ml_append(lnum++, (char_u *)_("???BLOCK MISSING"),
							    (colnr_T)0, TRUE);
		}
		இல்லையெனில்
		{
		    /*
		     * It is a data block.
		     * Append all the lines in this block.
		     */
		    has_error = FALSE;

		    // Verify the cached block's actual size matches the
		    // pointer entry's pe_page_count.  mf_get() cache hits
		    // return the original block without resizing, so a
		    // crafted swap file referencing the same block twice
		    // with different pe_page_count values would cause an
		    // OOB write below.
		    எனில் (hp->bh_page_மொத்தம் != page_மொத்தம்)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			page_மொத்தம் = hp->bh_page_மொத்தம்;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    எனில் (page_மொத்தம் * mfp->mf_page_size != dp->db_txt_end)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_end = page_மொத்தம் * mfp->mf_page_size;
		    }

		    எனில் (dp->db_txt_start < HEADER_SIZE
			    || dp->db_txt_start > dp->db_txt_end)
		    {
			ml_append(lnum++, (char_u *)_("??? block header corrupted"),
								    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_start = dp->db_txt_end;
		    }
		    // Make sure there is a NUL at the end of the block so we
		    // don't go over the end when copying text.
		    *((char_u *)dp + dp->db_txt_end - 1) = NUL;

		    // Check the number of lines in the block.
		    // If wrong, use the count in the data block.
		    எனில் (வரி_மொத்தம் != dp->db_வரி_மொத்தம்)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    முழு did_questions = FALSE;
		    க்கு (i = 0; i < dp->db_வரி_மொத்தம்; ++i)
		    {
			எனில் ((char_u *)&(dp->db_index[i])
					    >= (char_u *)dp + dp->db_txt_start)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    நிறுத்து;
			}

			txt_start = (dp->db_index[i] & DB_INDEX_MASK);
			எனில் (txt_start <= (முழு)HEADER_SIZE
					  || txt_start >= (முழு)dp->db_txt_end)
			{
			    ++error;
			    // avoid lots of lines with "???"
			    எனில் (did_questions)
				தொடர்;
			    did_questions = TRUE;
			    p = (char_u *)"???";
			}
			இல்லையெனில்
			{
			    did_questions = FALSE;
			    p = (char_u *)dp + txt_start;
			}
			ml_append(lnum++, p, (colnr_T)0, TRUE);
		    }
		    எனில் (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	எனில் (buf->b_ml.ml_அடுக்கு_உச்சி == 0)	// finished
	    நிறுத்து;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_அடுக்கு[--(buf->b_ml.ml_அடுக்கு_உச்சி)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_index + 1;	    // go to next index
	page_மொத்தம் = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    எனில் (orig_file_status != OK || curbuf->b_ml.ml_வரி_மொத்தம் != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	எனில் (!(curbuf->b_ml.ml_வரி_மொத்தம் == 2 && *ml_get(1) == NUL))
	{
	    changed_internal();
	    ++CHANGEDTICK(curbuf);
	}
    }
    இல்லையெனில்
    {
	க்கு (idx = 1; idx <= lnum; ++idx)
	{
	    // Need to copy one line, fetching the other one may flush it.
	    p = vim_strnsave(ml_get(idx), ml_get_len(idx));
	    i = STRCMP(p, ml_get(idx + lnum));
	    vim_free(p);
	    எனில் (i != 0)
	    {
		changed_internal();
		++CHANGEDTICK(curbuf);
		நிறுத்து;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    போது (curbuf->b_ml.ml_வரி_மொத்தம் > lnum
				       && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete(curbuf->b_ml.ml_வரி_மொத்தம்);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    எனில் (got_int)
	emsg(_(e_recovery_interrupted));
    இல்லையெனில் எனில் (error)
    {
	++no_wait_return;
	செய்தி(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_வரிகள்_starting_with_questions));
	--no_wait_return;
	செய்தி(_("See \":help E312\" for more information."));
	செய்தி(">>>>>>>>>>>>>");
    }
    இல்லையெனில்
    {
	எனில் (curbuf->b_changed)
	{
	    செய்தி(_("Recovery completed. You should check if everything is OK."));
	    செய்தி_puts(_("\n(You might want to write out this file under another name\n"));
	    செய்தி_puts(_("and run diff with the original file to check for changes)"));
	}
	இல்லையெனில்
	    செய்தி(_("Recovery completed. Buffer contents equals file contents."));
	செய்தி_puts(_("\nYou may want to delete the .swp file now."));
#எனில் defined(UNIX) || defined(MSWIN)
	எனில் (swapfile_process_running(b0p, fname_used))
	{
	    // Warn there could be an active Vim on the same file, the user may
	    // want to kill it.
	    செய்தி_puts(_("\nNote: process STILL RUNNING: "));
	    செய்தி_outnum(char_to_long(b0p->b0_pid));
	}
#endif
	செய்தி_puts("\n\n");
	cmdline_row = செய்தி_row;
    }
#ifdef FEAT_CRYPT
    எனில் (*buf->b_p_சாவி != NUL && STRCMP(curbuf->b_p_சாவி, buf->b_p_சாவி) != 0)
    {
	செய்தி_puts(_("Using crypt key from swap file for the text file.\n"));
	set_option_மதிப்பு_give_err((char_u *)"key", 0L, buf->b_p_சாவி, OPT_LOCAL);
    }
#endif
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_used);
    recoverymode = FALSE;
    எனில் (mfp != வெற்று)
    {
	எனில் (hp != வெற்று)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    எனில் (buf != வெற்று)
    {
#ifdef FEAT_CRYPT
	எனில் (buf->b_p_சாவி != curbuf->b_p_சாவி)
	    free_string_option(buf->b_p_சாவி);
	free_string_option(buf->b_p_cm);
#endif
	vim_free(buf->b_ml.ml_அடுக்கு);
	vim_free(buf);
    }
    எனில் (serious_error && called_from_main)
	ml_close(curbuf, TRUE);
    இல்லையெனில்
    {
	apply_autocmds(EVENT_BUFREADPOST, வெற்று, curbuf->b_fname, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, வெற்று, curbuf->b_fname, FALSE, curbuf);
    }
}

/*
 * Find the names of swap files in current directory and the directory given
 * with the 'directory' option.
 *
 * Used to:
 * - list the swap files for "vim -r"
 * - count the number of swap files when recovering
 * - list the swap files when recovering
 * - list the swap files for swapfilelist()
 * - find the name of the n'th swap file when recovering
 */
    முழு
recover_names(
    char_u	*fname,		// base for swap file name
    முழு		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list UNUSED, // when not NULL add file names to it
    முழு		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_out)	// result when "nr" > 0
{
    முழு		இலக்கம்_names;
    char_u	*(names[6]);
    char_u	*p;
    முழு		இலக்கம்_files;
    முழு		file_மொத்தம் = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = வெற்று;
#ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#endif

    எனில் (fname != வெற்று)
    {
#ifdef HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	எனில் (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	இல்லையெனில்
#endif
	    fname_res = fname;
    }

    எனில் (do_list)
    {
	// use msg() to start the scrolling properly
	செய்தி(_("Swap files found:"));
	செய்தி_putchar('\n');
    }

    /*
     * Do the loop for every directory in 'directory'.
     * First allocate some memory to put the directory name in.
     */
    dir_name.string = alloc(STRLEN(p_dir) + 1);
    dirp = p_dir;
    போது (dir_name.string != வெற்று && *dirp)
    {
	/*
	 * Isolate a directory name from *dirp and put it in dir_name (we know
	 * it is large enough, so use 31000 for length).
	 * Advance dirp to next directory name.
	 */
	dir_name.length = (size_t)copy_option_part(&dirp, dir_name.string, 31000, ",");

	எனில் (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    எனில் (fname == வெற்று)
	    {
#ifdef VMS
		names[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#இல்லையெனில்
		names[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#endif
#எனில் defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		names[1] = vim_strnsave((char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"));
		names[2] = vim_strnsave((char_u *)".sw?", STRLEN_LITERAL(".sw?"));
		இலக்கம்_names = 3;
#இல்லையெனில்
# ifdef VMS
		names[1] = vim_strnsave((char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"));
		இலக்கம்_names = 2;
# இல்லையெனில்
		இலக்கம்_names = 1;
# endif
#endif
	    }
	    இல்லையெனில்
		இலக்கம்_names = recov_file_names(names, fname_res, TRUE);
	}
	இல்லையெனில்			    // check directory dir_name
	{
	    எனில் (fname == வெற்று)
	    {
		string_T    ret;

#ifdef VMS
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#இல்லையெனில்
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#endif
#எனில் defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		names[1] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"), TRUE, &ret);
		names[2] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".sw?", STRLEN_LITERAL(".sw?"), TRUE, &ret);
		இலக்கம்_names = 3;
#இல்லையெனில்
# ifdef VMS
		names[1] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"), TRUE, &ret);
		இலக்கம்_names = 2;
# இல்லையெனில்
		இலக்கம்_names = 1;
# endif
#endif
	    }
	    இல்லையெனில்
	    {
		char_u	*tail;

#எனில் defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.length;
		எனில் (after_pathsep(dir_name.string, p) && dir_name.length > 1 && p[-1] == p[-2])
		{
		    // Ends with '//', Use Full path for swap name
		    tail = make_percent_swname(dir_name.string, p, fname_res);
		}
		இல்லையெனில்
#endif
		{
		    string_T	ret;

		    tail = gettail(fname_res);
		    tail = concat_fnames(dir_name.string, dir_name.length,
			tail, STRLEN(tail), TRUE, &ret);
		}
		எனில் (tail == வெற்று)
		    இலக்கம்_names = 0;
		இல்லையெனில்
		{
		    இலக்கம்_names = recov_file_names(names, tail, FALSE);
		    vim_free(tail);
		}
	    }
	}

	// check for out-of-memory
	க்கு (முழு i = 0; i < இலக்கம்_names; ++i)
	{
	    எனில் (names[i] == வெற்று)
	    {
		க்கு (i = 0; i < இலக்கம்_names; ++i)
		    vim_free(names[i]);
		இலக்கம்_names = 0;
	    }
	}
	எனில் (இலக்கம்_names == 0)
	    இலக்கம்_files = 0;
	இல்லையெனில் எனில் (expand_wildcards(இலக்கம்_names, names, &இலக்கம்_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    இலக்கம்_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	எனில் (*dirp == NUL && file_மொத்தம் + இலக்கம்_files == 0 && fname != வெற்று)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#எனில் defined(VMS)
			       (char_u *)"_swp", FALSE
#இல்லையெனில்
			       (char_u *)".swp", TRUE
#endif
			      );
	    எனில் (swapname != வெற்று)
	    {
		எனில் (mch_stat((எழுத்து *)swapname, &st) != -1)    // It exists!
		{
		    files = ALLOC_ONE(char_u *);
		    எனில் (files != வெற்று)
		    {
			files[0] = swapname;
			swapname = வெற்று;
			இலக்கம்_files = 1;
		    }
		}
		vim_free(swapname);
	    }
	}

	/*
	 * Remove swapfile name of the current buffer, it must be ignored.
	 * But keep it for swapfilelist().
	 */
	எனில் (curbuf->b_ml.ml_mfp != வெற்று
			       && (p = curbuf->b_ml.ml_mfp->mf_fname) != வெற்று
			       && ret_list == வெற்று)
	{
	    க்கு (முழு i = 0; i < இலக்கம்_files; ++i)
		// Do not expand wildcards, on windows would try to expand
		// "%tmp%" in "%tmp%file".
		எனில் (fullpathcmp(p, files[i], TRUE, FALSE) & FPC_SAME)
		{
		    // Remove the name from files[i].  Move further entries
		    // down.  When the array becomes empty free it here, since
		    // FreeWild() won't be called below.
		    vim_free(files[i]);
		    எனில் (--இலக்கம்_files == 0)
			vim_free(files);
		    இல்லையெனில்
			க்கு ( ; i < இலக்கம்_files; ++i)
			    files[i] = files[i + 1];
		}
	}
	எனில் (nr > 0)
	{
	    file_மொத்தம் += இலக்கம்_files;
	    எனில் (nr <= file_மொத்தம்)
	    {
		*fname_out = vim_strsave(
				      files[nr - 1 + இலக்கம்_files - file_மொத்தம்]);
		dirp = (char_u *)"";		    // stop searching
	    }
	}
	இல்லையெனில் எனில் (do_list)
	{
	    எனில் (dir_name.string[0] == '.' && dir_name.string[1] == NUL)
	    {
		எனில் (fname == வெற்று)
		    செய்தி_puts(_("   In current directory:\n"));
		இல்லையெனில்
		    செய்தி_puts(_("   Using specified name:\n"));
	    }
	    இல்லையெனில்
	    {
		செய்தி_puts(_("   In directory "));
		செய்தி_home_replace(dir_name.string);
		செய்தி_puts(":\n");
	    }

	    எனில் (இலக்கம்_files)
	    {
		க்கு (முழு i = 0; i < இலக்கம்_files; ++i)
		{
		    // print the swap file name
		    செய்தி_outnum((long)++file_மொத்தம்);
		    செய்தி_puts(".    ");
		    செய்தி_puts((எழுத்து *)gettail(files[i]));
		    செய்தி_putchar('\n');
		    (வெறுமை)swapfile_info(files[i]);
		}
	    }
	    இல்லையெனில்
		செய்தி_puts(_("      -- none --\n"));
	    out_flush();
	}
#ifdef FEAT_EVAL
	இல்லையெனில் எனில் (ret_list != வெற்று)
	{
	    string_T	name;

	    க்கு (முழு i = 0; i < இலக்கம்_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.length,
		    files[i], STRLEN(files[i]), TRUE, &name);
		எனில் (name.string != வெற்று)
		{
		    list_append_string(ret_list, name.string, (முழு)name.length);
		    vim_free(name.string);
		}
	    }
	}
#endif
	இல்லையெனில்
	    file_மொத்தம் += இலக்கம்_files;

	க்கு (முழு i = 0; i < இலக்கம்_names; ++i)
	    vim_free(names[i]);
	எனில் (இலக்கம்_files > 0)
	    FreeWild(இலக்கம்_files, files);
    }
    vim_free(dir_name.string);
    திரும்பு file_மொத்தம்;
}

#எனில் defined(UNIX) || defined(MSWIN)
/*
 * Need _very_ long file names.
 * Append the full path to name with path separators made into percent
 * signs, to "dir". An unnamed buffer is handled as "" (<currentdir>/"")
 * The last character in "dir" must be an extra slash or backslash, it is
 * removed.
 */
    char_u *
make_percent_swname(char_u *dir, char_u *dir_end, char_u *name)
{
    string_T	d = {வெற்று, 0};
    string_T	fixed_fname;
    char_u	*p;

    fixed_fname.string = fix_fname(name != வெற்று ? name : (char_u *)"");
    எனில் (fixed_fname.string == வெற்று)
	திரும்பு வெற்று;

    க்கு (p = fixed_fname.string; *p != NUL; MB_PTR_ADV(p))
	எனில் (vim_ispathsep(*p))
	    *p = '%';
    fixed_fname.length = (size_t)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_end[-1];
    *p = NUL;
    concat_fnames(dir, (size_t)(p - dir), fixed_fname.string, fixed_fname.length, TRUE, &d);
    vim_free(fixed_fname.string);

    திரும்பு d.string;
}
#endif

#எனில் (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
நிலையான முழு process_still_running;
#endif

#எனில் defined(FEAT_EVAL)
/*
 * Return information found in swapfile "fname" in dictionary "d".
 * This is used by the swapinfo() function.
 */
    வெறுமை
get_b0_dict(char_u *fname, dict_T *d)
{
    முழு fd;
    கட்டமைப்பு block0 b0;

    எனில் ((fd = mch_open((எழுத்து *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
    {
	எனில் (read_eintr(fd, &b0, அளவு(b0)) == அளவு(b0))
	{
	    எனில் (ml_check_b0_id(&b0) == FAIL)
		dict_add_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    இல்லையெனில் எனில் (b0_magic_wrong(&b0))
		dict_add_string_len(d, "error",
		    (char_u *)"Magic number mismatch", STRLEN_LITERAL("Magic number mismatch"));
	    இல்லையெனில்
	    {
		// we have swap information
		dict_add_string_len(d, "version", b0.b0_version, 10);
		dict_add_string_len(d, "user", b0.b0_uname, B0_UNAME_SIZE);
		dict_add_string_len(d, "host", b0.b0_hname, B0_HNAME_SIZE);
		dict_add_string_len(d, "fname", b0.b0_fname, B0_FNAME_SIZE_ORG);

		dict_add_எண்(d, "pid", char_to_long(b0.b0_pid));
		dict_add_எண்(d, "mtime", char_to_long(b0.b0_mtime));
		dict_add_எண்(d, "dirty", b0.b0_dirty ? 1 : 0);
# ifdef CHECK_INODE
		dict_add_எண்(d, "inode", char_to_long(b0.b0_ino));
# endif
	    }
	}
	இல்லையெனில்
	    dict_add_string_len(d, "error", (char_u *)"Cannot read file", STRLEN_LITERAL("Cannot read file"));
	close(fd);
    }
    இல்லையெனில்
	dict_add_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#endif

/*
 * Give information about an existing swap file.
 * Returns timestamp (0 when unknown).
 */
    நிலையான time_t
swapfile_info(char_u *fname)
{
    stat_T	    st;
    முழு		    fd;
    கட்டமைப்பு block0   b0;
#ifdef UNIX
    char_u	    uname[B0_UNAME_SIZE];
#endif

    // print the swap file date
    எனில் (mch_stat((எழுத்து *)fname, &st) != -1)
    {
#ifdef UNIX
	// print name of owner of the file
	எனில் (mch_get_uname(st.st_uid, uname, B0_UNAME_SIZE) == OK)
	{
	    செய்தி_puts(_("          owned by: "));
	    செய்தி_outtrans(uname);
	    செய்தி_puts(_("   dated: "));
	}
	இல்லையெனில்
#endif
	    செய்தி_puts(_("             dated: "));
	செய்தி_puts(get_ctime(st.st_mtime, TRUE));
    }
    இல்லையெனில்
	st.st_mtime = 0;

    /*
     * print the original file name
     */
    fd = mch_open((எழுத்து *)fname, O_RDONLY | O_EXTRA, 0);
    எனில் (fd >= 0)
    {
	எனில் (read_eintr(fd, &b0, அளவு(b0)) == அளவு(b0))
	{
	    எனில் (STRNCMP(b0.b0_version, "VIM 3.0", 7) == 0)
	    {
		செய்தி_puts(_("         [from Vim version 3.0]"));
	    }
	    இல்லையெனில் எனில் (ml_check_b0_id(&b0) == FAIL)
	    {
		செய்தி_puts(_("         [does not look like a Vim swap file]"));
	    }
	    இல்லையெனில்
	    {
		செய்தி_puts(_("         file name: "));
		எனில் (b0.b0_fname[0] == NUL)
		    செய்தி_puts(_("[No Name]"));
		இல்லையெனில்
		    செய்தி_outtrans(b0.b0_fname);

		செய்தி_puts(_("\n          modified: "));
		செய்தி_puts(b0.b0_dirty ? _("YES") : _("no"));

		எனில் (*(b0.b0_uname) != NUL)
		{
		    செய்தி_puts(_("\n         user name: "));
		    செய்தி_outtrans(b0.b0_uname);
		}

		எனில் (*(b0.b0_hname) != NUL)
		{
		    எனில் (*(b0.b0_uname) != NUL)
			செய்தி_puts(_("   host name: "));
		    இல்லையெனில்
			செய்தி_puts(_("\n         host name: "));
		    செய்தி_outtrans(b0.b0_hname);
		}

		எனில் (char_to_long(b0.b0_pid) != 0L)
		{
		    செய்தி_puts(_("\n        process ID: "));
		    செய்தி_outnum(char_to_long(b0.b0_pid));
#எனில் defined(UNIX) || defined(MSWIN)
		    எனில் (swapfile_process_running(&b0, fname))
		    {
			செய்தி_puts(_(" (STILL RUNNING)"));
# ifdef HAVE_PROCESS_STILL_RUNNING
			process_still_running = TRUE;
# endif
		    }
#endif
		}

		எனில் (b0_magic_wrong(&b0))
		{
#எனில் defined(MSWIN)
		    எனில் (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			செய்தி_puts(_("\n         [not usable with this version of Vim]"));
		    இல்லையெனில்
#endif
			செய்தி_puts(_("\n         [not usable on this computer]"));
		}
	    }
	}
	இல்லையெனில்
	    செய்தி_puts(_("         [cannot be read]"));
	close(fd);
    }
    இல்லையெனில்
	செய்தி_puts(_("         [cannot be opened]"));
    செய்தி_putchar('\n');

    திரும்பு st.st_mtime;
}

/*
 * Return TRUE if the swap file looks OK and there are no changes, thus it can
 * be safely deleted.
 */
    நிலையான முழு
swapfile_unchanged(char_u *fname)
{
    stat_T	    st;
    முழு		    fd;
    கட்டமைப்பு block0   b0;
    முழு		    ret = TRUE;

    // must be able to stat the swap file
    எனில் (mch_stat((எழுத்து *)fname, &st) == -1)
	திரும்பு FALSE;

    // must be able to read the first block
    fd = mch_open((எழுத்து *)fname, O_RDONLY | O_EXTRA, 0);
    எனில் (fd < 0)
	திரும்பு FALSE;
    எனில் (read_eintr(fd, &b0, அளவு(b0)) != அளவு(b0))
    {
	close(fd);
	திரும்பு FALSE;
    }

    // the ID and magic number must be correct
    எனில் (ml_check_b0_id(&b0) == FAIL|| b0_magic_wrong(&b0))
	ret = FALSE;

    // must be unchanged
    எனில் (b0.b0_dirty)
	ret = FALSE;

#எனில் defined(UNIX) || defined(MSWIN)
    // Host name must be known and must equal the current host name, otherwise
    // comparing pid is meaningless.
    எனில் (*(b0.b0_hname) == NUL)
    {
	ret = FALSE;
    }
    இல்லையெனில்
    {
	char_u	    hostname[B0_HNAME_SIZE];

	mch_get_host_name(hostname, B0_HNAME_SIZE);
	hostname[B0_HNAME_SIZE - 1] = NUL;
	b0.b0_hname[B0_HNAME_SIZE - 1] = NUL; // in case of corruption
	எனில் (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    எனில் (char_to_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#endif

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    close(fd);
    திரும்பு ret;
}

    நிலையான முழு
recov_file_names(char_u **names, char_u *path, முழு prepend_dot)
{
    முழு		இலக்கம்_names;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    முழு		i;
#ifndef MSWIN
    bool    shortname = curbuf->b_shortname;

    curbuf->b_shortname = false;
#endif
    string_T	ret;

    இலக்கம்_names = 0;

    /*
     * May also add the file name with a dot prepended, for swap file in same
     * dir as original file.
     */
    எனில் (prepend_dot)
    {
	names[இலக்கம்_names] = modname(path, (char_u *)".sw?", TRUE);
	எனில் (names[இலக்கம்_names] == வெற்று)
	    செல் end;
	++இலக்கம்_names;
    }

    /*
     * Form the normal swap file name pattern by appending ".sw?".
     */
#ifdef VMS
    names[இலக்கம்_names] = concat_fnames(path, STRLEN(path),
	(char_u *)"_sw%", STRLEN_LITERAL("_sw%"), FALSE, &ret);
#இல்லையெனில்
    names[இலக்கம்_names] = concat_fnames(path, STRLEN(path),
	(char_u *)".sw?", STRLEN_LITERAL(".sw?"), FALSE, &ret);
#endif
    எனில் (names[இலக்கம்_names] == வெற்று)
	செல் end;
    எனில் (இலக்கம்_names >= 1)	    // check if we have the same name twice
    {
	p = names[இலக்கம்_names - 1];
	i = (முழு)STRLEN(names[இலக்கம்_names - 1]) - (முழு)STRLEN(names[இலக்கம்_names]);
	எனில் (i > 0)
	    p += i;	    // file name has been expanded to full path

	எனில் (STRCMP(p, names[இலக்கம்_names]) != 0)
	    ++இலக்கம்_names;
	இல்லையெனில்
	    vim_free(names[இலக்கம்_names]);
    }
    இல்லையெனில்
	++இலக்கம்_names;

#ifndef MSWIN
    /*
     * Also try with 'shortname' set, in case the file is on a DOS filesystem.
     */
    curbuf->b_shortname = true;
# ifdef VMS
    names[இலக்கம்_names] = modname(path, (char_u *)"_sw%", FALSE);
# இல்லையெனில்
    names[இலக்கம்_names] = modname(path, (char_u *)".sw?", FALSE);
# endif
    எனில் (names[இலக்கம்_names] == வெற்று)
	செல் end;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = names[இலக்கம்_names];
    i = STRLEN(names[இலக்கம்_names]) - STRLEN(names[இலக்கம்_names - 1]);
    எனில் (i > 0)
	p += i;		// file name has been expanded to full path
    எனில் (STRCMP(names[இலக்கம்_names - 1], p) == 0)
	vim_free(names[இலக்கம்_names]);
    இல்லையெனில்
	++இலக்கம்_names;
#endif

end:
#ifndef MSWIN
    curbuf->b_shortname = shortname;
#endif

    திரும்பு இலக்கம்_names;
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
    வெறுமை
ml_sync_all(முழு check_file, முழு check_char)
{
    buf_T		*buf;
    stat_T		st;

    FOR_ALL_BUFFERS(buf)
    {
	எனில் (buf->b_ml.ml_mfp == வெற்று
		|| buf->b_ml.ml_mfp->mf_fname == வெற்று
		|| buf->b_ml.ml_mfp->mf_fd < 0)
	    தொடர்;			    // no file

#ifdef FEAT_CRYPT
	எனில் (crypt_may_close_swapfile(buf, buf->b_p_சாவி,
						     crypt_get_method_nr(buf)))
	    தொடர்;
#endif

	ml_flush_வரி(buf);		    // flush buffered line
					    // flush locked block
	(வெறுமை)ml_find_வரி(buf, (linenr_T)0, ML_FLUSH);
	எனில் (bufIsChanged(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != வெற்று)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    எனில் (mch_stat((எழுத்து *)buf->b_ffname, &st) == -1
		    || st.st_mtime != buf->b_mtime_read
#ifdef ST_MTIM_NSEC
		    || st.ST_MTIM_NSEC != buf->b_mtime_read_ns
#endif
		    || st.st_size != buf->b_orig_size)
	    {
		ml_preserve(buf, FALSE);
		did_check_timestamps = FALSE;
		need_check_timestamps = TRUE;	// give message later
	    }
	}
	எனில் (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (வெறுமை)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (bufIsChanged(buf) ? MFS_FLUSH : 0));
	    எனில் (check_char && ui_char_avail())	// character available now
		நிறுத்து;
	}
    }
}

/*
 * sync one buffer, including negative blocks
 *
 * after this all the blocks are in the swap file
 *
 * Used for the :preserve command and when the original file has been
 * changed or deleted.
 *
 * when message is TRUE the success of preserving is reported
 */
    வெறுமை
ml_preserve(buf_T *buf, முழு message)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    முழு		status;
    முழு		got_int_save = got_int;

    எனில் (mfp == வெற்று || mfp->mf_fname == வெற்று)
    {
	எனில் (message)
	    emsg(_(e_cannot_preserve_there_is_no_swap_file));
	திரும்பு;
    }
#ifdef FEAT_CRYPT
    எனில் (crypt_may_close_swapfile(buf, buf->b_p_சாவி, crypt_get_method_nr(buf)))
	திரும்பு;
#endif

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_வரி(buf);				    // flush buffered line
    (வெறுமை)ml_find_வரி(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    status = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

    // stack is invalid after mf_sync(.., MFS_ALL)
    buf->b_ml.ml_அடுக்கு_உச்சி = 0;

    /*
     * Some of the data blocks may have been changed from negative to
     * positive block number. In that case the pointer blocks need to be
     * updated.
     *
     * We don't know in which pointer block the references are, so we visit
     * all data blocks until there are no more translations to be done (or
     * we hit the end of the file, which can only happen in case a write fails,
     * e.g. when file system if full).
     * ml_find_line() does the work by translating the negative block numbers
     * when getting the first line of each data block.
     */
    எனில் (mf_need_trans(mfp) && !got_int)
    {
	lnum = 1;
	போது (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_வரி_மொத்தம்)
	{
	    hp = ml_find_வரி(buf, lnum, ML_FIND);
	    எனில் (hp == வெற்று)
	    {
		status = FAIL;
		செல் theend;
	    }
	    CHECK(buf->b_ml.ml_locked_கீழ் != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_உயர்வு + 1;
	}
	(வெறுமை)ml_find_வரி(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	எனில் (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    status = FAIL;
	buf->b_ml.ml_அடுக்கு_உச்சி = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    எனில் (message)
    {
	எனில் (status == OK)
	    செய்தி(_("File preserved"));
	இல்லையெனில்
	    emsg(_(e_preserve_failed));
    }
}

/*
 * NOTE: The pointer returned by the ml_get_*() functions only remains valid
 * until the next call!
 *  line1 = ml_get(1);
 *  line2 = ml_get(2);	// line1 is now invalid!
 * Make a copy of the line if necessary.
 */
/*
 * Return a pointer to a (read-only copy of a) line.
 *
 * On failure an error message is given and IObuff is returned (to avoid
 * having to check for error everywhere).
 */
    char_u  *
ml_get(linenr_T lnum)
{
    திரும்பு ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
    char_u *
ml_get_pos(pos_T *pos)
{
    திரும்பு (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->col);
}

/*
 * Return pointer to cursor line.
 */
    char_u *
ml_get_curline(வெறுமை)
{
    திரும்பு ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE);
}

/*
 * Return pointer to cursor position.
 */
    char_u *
ml_get_cursor(வெறுமை)
{
    திரும்பு (ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE) +
							curwin->w_cursor.col);
}

// return length (excluding the NUL) of the given line
    colnr_T
ml_get_len(linenr_T lnum)
{
    திரும்பு ml_get_buf_len(curbuf, lnum);
}

// return length (excluding the NUL) of the text after position "pos"
    colnr_T
ml_get_pos_len(pos_T *pos)
{
    திரும்பு ml_get_buf_len(curbuf, pos->lnum) - pos->col;
}

// return length (excluding the NUL) of the cursor line
    colnr_T
ml_get_curline_len(வெறுமை)
{
    திரும்பு ml_get_buf_len(curbuf, curwin->w_cursor.lnum);
}

// return length (excluding the NUL) of the cursor position
    colnr_T
ml_get_cursor_len(வெறுமை)
{
    திரும்பு ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.col;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*வரி;

    எனில் (*(வரி = ml_get_buf(buf, lnum, FALSE)) == NUL)
	திரும்பு 0;

    எனில் (buf->b_ml.ml_வரி_textlen <= 0)
	buf->b_ml.ml_வரி_textlen = (முழு)STRLEN(வரி) + 1;
    திரும்பு buf->b_ml.ml_வரி_textlen - 1;
}

/*
 * Return a pointer to a line in a specific buffer
 *
 * "will_change": if TRUE mark the buffer dirty (chars in the line will be
 * changed)
 */
    char_u  *
ml_get_buf(
    buf_T	*buf,
    linenr_T	lnum,
    முழு		will_change)		// line will be changed
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    நிலையான முழு	recursive = 0;
    நிலையான char_u questions[4];

    எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம்)	// invalid line number
    {
	எனில் (recursive == 0)
	{
	    // Avoid giving this message for a recursive call, may happen when
	    // the GUI redraws part of the text.
	    ++recursive;
	    siemsg(e_ml_get_invalid_lnum_nr, lnum);
	    --recursive;
	}
	ml_flush_வரி(buf);
errorret:
	STRCPY(questions, "???");
	buf->b_ml.ml_வரி_len = 4;
	buf->b_ml.ml_வரி_textlen = buf->b_ml.ml_வரி_len;
	buf->b_ml.ml_வரி_lnum = lnum;
	திரும்பு questions;
    }
    எனில் (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    எனில் (buf->b_ml.ml_mfp == வெற்று)	// there are no lines
    {
	buf->b_ml.ml_வரி_len = 1;
	buf->b_ml.ml_வரி_textlen = buf->b_ml.ml_வரி_len;
	திரும்பு (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    எனில் (buf->b_ml.ml_வரி_lnum != lnum || mf_dont_release)
    {
	unsigned    start, end;
	முழு	    idx;

	ml_flush_வரி(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	எனில் ((hp = ml_find_வரி(buf, lnum, ML_FIND)) == வெற்று)
	{
	    எனில் (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_find_வரி_nr_in_buffer_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    செல் errorret;
	}

	dp = (DATA_BL *)(hp->bh_data);

	idx = lnum - buf->b_ml.ml_locked_கீழ்;
	start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	எனில் (idx == 0)
	    end = dp->db_txt_end;
	இல்லையெனில்
	    end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);

	buf->b_ml.ml_வரி_ptr = (char_u *)dp + start;
	buf->b_ml.ml_வரி_len = end - start;
#எனில் defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	எனில் (buf->b_has_textprop
			 && (size_t)buf->b_ml.ml_வரி_len > அளவு(textprop_T))
	    buf->b_ml.ml_வரி_textlen = 0;  // call STRLEN() later when needed
	இல்லையெனில்
#endif
	    buf->b_ml.ml_வரி_textlen = buf->b_ml.ml_வரி_len;
	buf->b_ml.ml_வரி_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_வரி_DIRTY | ML_ALLOCATED);
    }
    எனில் (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ifdef FEAT_EVAL
	எனில் (ml_get_alloc_வரிகள் && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_வரி_DIRTY;
#endif
    }

#ifdef FEAT_EVAL
    எனில் (ml_get_alloc_வரிகள்
		 && (buf->b_ml.ml_flags & (ML_வரி_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_வரி_len);

	// make sure the text is in allocated memory
	எனில் (p != வெற்று)
	{
	    memmove(p, buf->b_ml.ml_வரி_ptr, buf->b_ml.ml_வரி_len);
	    buf->b_ml.ml_வரி_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    எனில் (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_வரி_DIRTY;
	}
    }
#endif
    திரும்பு buf->b_ml.ml_வரி_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    முழு
ml_வரி_alloced(வெறுமை)
{
    திரும்பு (curbuf->b_ml.ml_flags & ML_வரி_DIRTY);
}

#ifdef FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    நிலையான வெறுமை
add_text_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**வரி,
	    முழு		*len,
	    char_u	**tofree)
{
    முழு		round;
    முழு		new_prop_மொத்தம் = 0;
    முழு		மொத்தம்;
    முழு		n;
    char_u	*props;
    முழு		new_len = 0;  // init for gcc
    char_u	*new_வரி = வெற்று;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    க்கு (round = 1; round <= 2; ++round)
    {
	எனில் (round == 2)
	{
	    uint16_t pc;

	    எனில் (new_prop_மொத்தம் == 0)
		திரும்பு;  // nothing to do
	    new_len = *len + (முழு)PROP_மொத்தம்_SIZE
			     + new_prop_மொத்தம் * (முழு)அளவு(textprop_T);
	    new_வரி = alloc(new_len);
	    எனில் (new_வரி == வெற்று)
		திரும்பு;
	    mch_memmove(new_வரி, *வரி, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_மொத்தம்;
	    mch_memmove(new_வரி + *len, &pc, PROP_மொத்தம்_SIZE);
	    new_prop_மொத்தம் = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	மொத்தம் = get_text_props(buf, lnum, &props, FALSE);
	க்கு (n = 0; n < மொத்தம்; ++n)
	{
	    mch_memmove(&prop, props + n * அளவு(textprop_T),
							   அளவு(textprop_T));
	    எனில் (prop.tp_flags & TP_FLAG_CONT_NEXT)
	    {
		எனில் (round == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_PREV;
		    prop.tp_col = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_text_offset = 0;
		    mch_memmove(new_வரி + *len + (முழு)PROP_மொத்தம்_SIZE
			    + new_prop_மொத்தம் * அளவு(textprop_T),
			    &prop, அளவு(textprop_T));
		}
		++new_prop_மொத்தம்;
	    }
	}
    }
    *வரி = new_வரி;
    *tofree = new_வரி;
    *len = new_len;
}
#endif

    நிலையான முழு
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*வரி_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    முழு		flags)		// ML_APPEND_ flags
{
    char_u	*வரி = வரி_arg;
    colnr_T	len = len_arg;
    முழு		i;
    முழு		வரி_மொத்தம்;	// number of indexes in current block
    முழு		offset;
    முழு		from, to;
    long	space_needed;	// space needed for new line
    முழு		page_size;
    முழு		page_மொத்தம்;
    முழு		db_idx;		// index for lnum in data block
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
#ifdef FEAT_PROP_POPUP
    char_u	*tofree = வெற்று;
# ifdef FEAT_BYTEOFF
    colnr_T	text_len = 0;	// text len with NUL without text properties
# endif
#endif
    முழு		ret = FAIL;

    எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம் || buf->b_ml.ml_mfp == வெற்று)
	திரும்பு FAIL;  // lnum out of range

    எனில் (lowest_marked && lowest_marked > lnum)
	lowest_marked = lnum + 1;

    எனில் (len == 0)
    {
	len = (colnr_T)STRLEN(வரி) + 1;	// space needed for the text
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	text_len = len;
#endif
    }
#எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    இல்லையெனில் எனில் (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	text_len = (colnr_T)STRLEN(வரி) + 1;
    இல்லையெனில்
	text_len = len;
#endif

#ifdef FEAT_PROP_POPUP
    எனில் (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	add_text_props_for_append(buf, lnum, &வரி, &len, &tofree);
#endif

    space_needed = len + INDEX_SIZE;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    page_size = mfp->mf_page_size;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    எனில் ((hp = ml_find_வரி(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_செருகு)) == வெற்று)
	செல் theend;

    buf->b_ml.ml_flags &= ~ML_EMPTY;

    எனில் (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    இல்லையெனில்
	db_idx = lnum - buf->b_ml.ml_locked_கீழ்;
		// get line count before the insertion
    வரி_மொத்தம் = buf->b_ml.ml_locked_உயர்வு - buf->b_ml.ml_locked_கீழ்;

    dp = (DATA_BL *)(hp->bh_data);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    எனில் ((long)dp->db_free < space_needed && db_idx == வரி_மொத்தம் - 1
					    && lnum < buf->b_ml.ml_வரி_மொத்தம்)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_உயர்வு);
	எனில் ((hp = ml_find_வரி(buf, lnum + 1, ML_செருகு)) == வெற்று)
	    செல் theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	வரி_மொத்தம் = buf->b_ml.ml_locked_உயர்வு - buf->b_ml.ml_locked_கீழ்;
	CHECK(buf->b_ml.ml_locked_கீழ் != lnum + 1, "locked_low != lnum + 1");

	dp = (DATA_BL *)(hp->bh_data);
    }

    ++buf->b_ml.ml_வரி_மொத்தம்;

    எனில் ((long)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_start -= len;
	dp->db_free -= space_needed;
	++(dp->db_வரி_மொத்தம்);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	எனில் (வரி_மொத்தம் > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    எனில் (db_idx < 0)
		offset = dp->db_txt_end;
	    இல்லையெனில்
		offset = ((dp->db_index[db_idx]) & DB_INDEX_MASK);
	    mch_memmove((எழுத்து *)dp + dp->db_txt_start,
					  (எழுத்து *)dp + dp->db_txt_start + len,
				 (size_t)(offset - (dp->db_txt_start + len)));
	    க்கு (i = வரி_மொத்தம் - 1; i > db_idx; --i)
		dp->db_index[i + 1] = dp->db_index[i] - len;
	    dp->db_index[db_idx + 1] = offset - len;
	}
	இல்லையெனில்
	    // add line at the end (which is the start of the text)
	    dp->db_index[db_idx + 1] = dp->db_txt_start;

	/*
	 * copy the text into the block
	 */
	mch_memmove((எழுத்து *)dp + dp->db_index[db_idx + 1], வரி, (size_t)len);
	எனில் (flags & ML_APPEND_MARK)
	    dp->db_index[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	எனில் (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    இல்லையெனில்	    // not enough space in data block
    {
	long	    வரி_மொத்தம்_இடது, வரி_மொத்தம்_வலது;
	முழு	    page_மொத்தம்_இடது, page_மொத்தம்_வலது;
	bhdr_T	    *hp_இடது;
	bhdr_T	    *hp_வலது;
	bhdr_T	    *hp_new;
	முழு	    வரிகள்_moved;
	முழு	    data_moved = 0;	    // init to shut up gcc
	முழு	    total_moved = 0;	    // init to shut up gcc
	DATA_BL	    *dp_வலது, *dp_இடது;
	முழு	    அடுக்கு_idx;
	முழு	    in_இடது;
	முழு	    lineadd;
	blocknr_T   bnum_இடது, bnum_வலது;
	linenr_T    lnum_இடது, lnum_வலது;
	முழு	    pb_idx;
	PTR_BL	    *pp_new;

	/*
	 * There is not enough room, we have to create a new data block and
	 * copy some lines into it.
	 * Then we have to insert an entry in the pointer block.
	 * If this pointer block also is full, we go up another block, and so
	 * on, up to the root if necessary.
	 * The line counts in the pointer blocks have already been adjusted by
	 * ml_find_line().
	 *
	 * We are going to allocate a new data block. Depending on the
	 * situation it will be put to the left or right of the existing
	 * block.  If possible we put the new line in the left block and move
	 * the lines after it to the right block. Otherwise the new line is
	 * also put in the right block. This method is more efficient when
	 * inserting a lot of lines at one place.
	 */
	எனில் (db_idx < 0)		// left block is new, right block is existing
	{
	    வரிகள்_moved = 0;
	    in_இடது = TRUE;
	    // space_needed does not change
	}
	இல்லையெனில்			// left block is existing, right block is new
	{
	    வரிகள்_moved = வரி_மொத்தம் - db_idx - 1;
	    எனில் (வரிகள்_moved == 0)
		in_இடது = FALSE;	// put new line in right block
					// space_needed does not change
	    இல்லையெனில்
	    {
		data_moved = ((dp->db_index[db_idx]) & DB_INDEX_MASK) -
							    dp->db_txt_start;
		total_moved = data_moved + வரிகள்_moved * INDEX_SIZE;
		எனில் ((long)dp->db_free + total_moved >= space_needed)
		{
		    in_இடது = TRUE;	// put new line in left block
		    space_needed = total_moved;
		}
		இல்லையெனில்
		{
		    in_இடது = FALSE;	    // put new line in right block
		    space_needed += total_moved;
		}
	    }
	}

	page_மொத்தம் = ((space_needed + HEADER_SIZE) + page_size - 1) / page_size;
	எனில் ((hp_new = ml_new_data(mfp, flags & ML_APPEND_NEW, page_மொத்தம்))
								       == வெற்று)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_உயர்வு);
	    செல் theend;
	}
	எனில் (db_idx < 0)		// left block is new
	{
	    hp_இடது = hp_new;
	    hp_வலது = hp;
	    வரி_மொத்தம்_இடது = 0;
	    வரி_மொத்தம்_வலது = வரி_மொத்தம்;
	}
	இல்லையெனில்			// right block is new
	{
	    hp_இடது = hp;
	    hp_வலது = hp_new;
	    வரி_மொத்தம்_இடது = வரி_மொத்தம்;
	    வரி_மொத்தம்_வலது = 0;
	}
	dp_வலது = (DATA_BL *)(hp_வலது->bh_data);
	dp_இடது = (DATA_BL *)(hp_இடது->bh_data);
	bnum_இடது = hp_இடது->bh_bnum;
	bnum_வலது = hp_வலது->bh_bnum;
	page_மொத்தம்_இடது = hp_இடது->bh_page_மொத்தம்;
	page_மொத்தம்_வலது = hp_வலது->bh_page_மொத்தம்;

	/*
	 * May move the new line into the right/new block.
	 */
	எனில் (!in_இடது)
	{
	    dp_வலது->db_txt_start -= len;
	    dp_வலது->db_free -= len + INDEX_SIZE;
	    dp_வலது->db_index[0] = dp_வலது->db_txt_start;
	    எனில் (flags & ML_APPEND_MARK)
		dp_வலது->db_index[0] |= DB_MARKED;

	    mch_memmove((எழுத்து *)dp_வலது + dp_வலது->db_txt_start,
							   வரி, (size_t)len);
	    ++வரி_மொத்தம்_வலது;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	எனில் (வரிகள்_moved)
	{
	    dp_வலது->db_txt_start -= data_moved;
	    dp_வலது->db_free -= total_moved;
	    mch_memmove((எழுத்து *)dp_வலது + dp_வலது->db_txt_start,
			(எழுத்து *)dp_இடது + dp_இடது->db_txt_start,
			(size_t)data_moved);
	    offset = dp_வலது->db_txt_start - dp_இடது->db_txt_start;
	    dp_இடது->db_txt_start += data_moved;
	    dp_இடது->db_free += total_moved;

	    /*
	     * update indexes in the new block
	     */
	    க்கு (to = வரி_மொத்தம்_வலது, from = db_idx + 1;
					 from < வரி_மொத்தம்_இடது; ++from, ++to)
		dp_வலது->db_index[to] = dp->db_index[from] + offset;
	    வரி_மொத்தம்_வலது += வரிகள்_moved;
	    வரி_மொத்தம்_இடது -= வரிகள்_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	எனில் (in_இடது)
	{
	    dp_இடது->db_txt_start -= len;
	    dp_இடது->db_free -= len + INDEX_SIZE;
	    dp_இடது->db_index[வரி_மொத்தம்_இடது] = dp_இடது->db_txt_start;
	    எனில் (flags & ML_APPEND_MARK)
		dp_இடது->db_index[வரி_மொத்தம்_இடது] |= DB_MARKED;
	    mch_memmove((எழுத்து *)dp_இடது + dp_இடது->db_txt_start,
							   வரி, (size_t)len);
	    ++வரி_மொத்தம்_இடது;
	}

	எனில் (db_idx < 0)		// left block is new
	{
	    lnum_இடது = lnum + 1;
	    lnum_வலது = 0;
	}
	இல்லையெனில்			// right block is new
	{
	    lnum_இடது = 0;
	    எனில் (in_இடது)
		lnum_வலது = lnum + 2;
	    இல்லையெனில்
		lnum_வலது = lnum + 1;
	}
	dp_இடது->db_வரி_மொத்தம் = வரி_மொத்தம்_இடது;
	dp_வலது->db_வரி_மொத்தம் = வரி_மொத்தம்_வலது;

	/*
	 * release the two data blocks
	 * The new one (hp_new) already has a correct blocknumber.
	 * The old one (hp, in ml_locked) gets a positive blocknumber if
	 * we changed it and we are not editing a new file.
	 */
	எனில் (வரிகள்_moved || in_இடது)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	எனில் (!(flags & ML_APPEND_NEW) && db_idx >= 0 && in_இடது)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_find_வரி(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	க்கு (அடுக்கு_idx = buf->b_ml.ml_அடுக்கு_உச்சி - 1; அடுக்கு_idx >= 0;
								  --அடுக்கு_idx)
	{
	    ip = &(buf->b_ml.ml_அடுக்கு[அடுக்கு_idx]);
	    pb_idx = ip->ip_index;
	    எனில் ((hp = mf_get(mfp, ip->ip_bnum, 1)) == வெற்று)
		செல் theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    எனில் (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		செல் theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    எனில் (pp->pb_மொத்தம் < pp->pb_மொத்தம்_max)
	    {
		எனில் (pb_idx + 1 < (முழு)pp->pb_மொத்தம்)
		    mch_memmove(&pp->pb_pointer[pb_idx + 2],
				&pp->pb_pointer[pb_idx + 1],
			(size_t)(pp->pb_மொத்தம் - pb_idx - 1) * அளவு(PTR_EN));
		++pp->pb_மொத்தம்;
		pp->pb_pointer[pb_idx].pe_வரி_மொத்தம் = வரி_மொத்தம்_இடது;
		pp->pb_pointer[pb_idx].pe_bnum = bnum_இடது;
		pp->pb_pointer[pb_idx].pe_page_மொத்தம் = page_மொத்தம்_இடது;
		pp->pb_pointer[pb_idx + 1].pe_வரி_மொத்தம் = வரி_மொத்தம்_வலது;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_வலது;
		pp->pb_pointer[pb_idx + 1].pe_page_மொத்தம் = page_மொத்தம்_வலது;

		எனில் (lnum_இடது != 0)
		    pp->pb_pointer[pb_idx].pe_old_lnum = lnum_இடது;
		எனில் (lnum_வலது != 0)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_வலது;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_அடுக்கு_உச்சி = அடுக்கு_idx + 1;	    // truncate stack

		எனில் (lineadd)
		{
		    --(buf->b_ml.ml_அடுக்கு_உச்சி);
		    // fix line count for rest of blocks in the stack
		    ml_lineadd(buf, lineadd);
							// fix stack itself
		    buf->b_ml.ml_அடுக்கு[buf->b_ml.ml_அடுக்கு_உச்சி].ip_உயர்வு +=
								      lineadd;
		    ++(buf->b_ml.ml_அடுக்கு_உச்சி);
		}

		/*
		 * We are finished, break the loop here.
		 */
		நிறுத்து;
	    }
	    // pointer block full
	    /*
	     * split the pointer block
	     * allocate a new pointer block
	     * move some of the pointer into the new block
	     * prepare for updating the parent block
	     */
	    க்கு (;;)	// do this twice when splitting block 1
	    {
		hp_new = ml_new_ptr(mfp);
		எனில் (hp_new == வெற்று)	    // TODO: try to fix tree
		    செல் theend;
		pp_new = (PTR_BL *)(hp_new->bh_data);

		எனில் (hp->bh_bnum != 1)
		    நிறுத்து;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (size_t)page_size);
		pp->pb_மொத்தம் = 1;
		pp->pb_pointer[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_pointer[0].pe_வரி_மொத்தம் = buf->b_ml.ml_வரி_மொத்தம்;
		pp->pb_pointer[0].pe_old_lnum = 1;
		pp->pb_pointer[0].pe_page_மொத்தம் = 1;
		mf_put(mfp, hp, TRUE, FALSE);   // release block 1
		hp = hp_new;		// new block is to be split
		pp = pp_new;
		CHECK(அடுக்கு_idx != 0, _("stack_idx should be 0"));
		ip->ip_index = 0;
		++அடுக்கு_idx;	// do block 1 again later
	    }
	    /*
	     * move the pointers after the current one to the new block
	     * If there are none, the new entry will be in the new block.
	     */
	    total_moved = pp->pb_மொத்தம் - pb_idx - 1;
	    எனில் (total_moved)
	    {
		mch_memmove(&pp_new->pb_pointer[0],
				&pp->pb_pointer[pb_idx + 1],
				(size_t)(total_moved) * அளவு(PTR_EN));
		pp_new->pb_மொத்தம் = total_moved;
		pp->pb_மொத்தம் -= total_moved - 1;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_வலது;
		pp->pb_pointer[pb_idx + 1].pe_வரி_மொத்தம் = வரி_மொத்தம்_வலது;
		pp->pb_pointer[pb_idx + 1].pe_page_மொத்தம் = page_மொத்தம்_வலது;
		எனில் (lnum_வலது)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_வலது;
	    }
	    இல்லையெனில்
	    {
		pp_new->pb_மொத்தம் = 1;
		pp_new->pb_pointer[0].pe_bnum = bnum_வலது;
		pp_new->pb_pointer[0].pe_வரி_மொத்தம் = வரி_மொத்தம்_வலது;
		pp_new->pb_pointer[0].pe_page_மொத்தம் = page_மொத்தம்_வலது;
		pp_new->pb_pointer[0].pe_old_lnum = lnum_வலது;
	    }
	    pp->pb_pointer[pb_idx].pe_bnum = bnum_இடது;
	    pp->pb_pointer[pb_idx].pe_வரி_மொத்தம் = வரி_மொத்தம்_இடது;
	    pp->pb_pointer[pb_idx].pe_page_மொத்தம் = page_மொத்தம்_இடது;
	    எனில் (lnum_இடது)
		pp->pb_pointer[pb_idx].pe_old_lnum = lnum_இடது;
	    lnum_இடது = 0;
	    lnum_வலது = 0;

	    /*
	     * recompute line counts
	     */
	    வரி_மொத்தம்_வலது = 0;
	    க்கு (i = 0; i < (முழு)pp_new->pb_மொத்தம்; ++i)
		வரி_மொத்தம்_வலது += pp_new->pb_pointer[i].pe_வரி_மொத்தம்;
	    வரி_மொத்தம்_இடது = 0;
	    க்கு (i = 0; i < (முழு)pp->pb_மொத்தம்; ++i)
		வரி_மொத்தம்_இடது += pp->pb_pointer[i].pe_வரி_மொத்தம்;

	    bnum_இடது = hp->bh_bnum;
	    bnum_வலது = hp_new->bh_bnum;
	    page_மொத்தம்_இடது = 1;
	    page_மொத்தம்_வலது = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	எனில் (அடுக்கு_idx < 0)
	{
	    iemsg(e_updated_too_many_blocks);
	    buf->b_ml.ml_அடுக்கு_உச்சி = 0;	// invalidate stack
	}
    }

#ifdef FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (long)text_len
# இல்லையெனில்
	    (long)len
# endif
	    , ML_CHNK_ADDLINE);
#endif

#ifdef FEAT_NETBEANS_INTG
    எனில் (!(flags & ML_APPEND_NEW) && netbeans_active())
    {
	முழு வரி_len = (முழு)STRLEN(வரி);
	எனில் (வரி_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, வரி, வரி_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)வரி_len, (char_u *)"\n", 1);
    }
#endif
#ifdef FEAT_JOB_CHANNEL
    எனில் (!(flags & ML_APPEND_NEW) && buf->b_write_to_channel)
	channel_write_new_வரிகள்(buf);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    vim_free(tofree);
#endif
    திரும்பு ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    நிலையான முழு
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*வரி,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    முழு		flags)		// ML_APPEND_ flags
{
    எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம்)
	திரும்பு FAIL;  // lnum out of range

    எனில் (buf->b_ml.ml_வரி_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_வரி(buf);

#ifdef FEAT_EVAL
    எனில் (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	எனில் (buf->b_ml.ml_வரி_lnum != 0)
	    ml_flush_வரி(buf);
    }
#endif

    திரும்பு ml_append_int(buf, lnum, வரி, len, flags);
}

/*
 * Append a line after lnum (may be 0 to insert a line in front of the file).
 * "line" does not need to be allocated, but can't be another line in a
 * buffer, unlocking may make it invalid.
 *
 * "newfile": TRUE when starting to edit a new file, meaning that pe_old_lnum
 *		will be set for recovery
 * Check: The caller of this function should probably also call
 * appended_lines().
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
ml_append(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*வரி,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    முழு		newfile)	// flag, see above
{
    திரும்பு ml_append_flags(lnum, வரி, len, newfile ? ML_APPEND_NEW : 0);
}

    முழு
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*வரி,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    முழு		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    எனில் (curbuf->b_ml.ml_mfp == வெற்று && open_buffer(FALSE, வெற்று, 0) == FAIL)
	திரும்பு FAIL;
    திரும்பு ml_append_flush(curbuf, lnum, வரி, len, flags);
}


#எனில் defined(FEAT_SPELL) || defined(FEAT_QUICKFIX) || defined(FEAT_PROP_POPUP)

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
    முழு
ml_append_buf(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*வரி,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    முழு		newfile)	// flag, see above
{
    எனில் (buf->b_ml.ml_mfp == வெற்று)
	திரும்பு FAIL;
    திரும்பு ml_append_flush(buf, lnum, வரி, len, newfile ? ML_APPEND_NEW : 0);
}
#endif

/*
 * Replace line "lnum", with buffering, in current buffer.
 *
 * If "copy" is TRUE, make a copy of the line, otherwise the line has been
 * copied to allocated memory already.
 * If "copy" is FALSE the "line" may be freed to add text properties!
 * Do not use it after calling ml_replace().
 *
 * Check: The caller of this function should probably also call
 * changed_lines(), unless update_screen(UPD_NOT_VALID) is used.
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
ml_replace(linenr_T lnum, char_u *வரி, முழு copy)
{
    colnr_T len = -1;

    எனில் (வரி != வெற்று)
	len = (colnr_T)STRLEN(வரி);
    திரும்பு ml_replace_len(lnum, வரி, len, FALSE, copy);
}

/*
 * Replace a line for the current buffer.  Like ml_replace() with:
 * "len_arg" is the length of the text, excluding NUL.
 * If "has_props" is TRUE then "line_arg" includes the text properties and
 * "len_arg" includes the NUL of the text and text properties.
 * When "copy" is TRUE copy the text into allocated memory, otherwise
 * "line_arg" must be allocated and will be consumed here.
 */
    முழு
ml_replace_len(
	linenr_T    lnum,
	char_u	    *வரி_arg,
	colnr_T	    len_arg,
	முழு	    has_props,
	முழு	    copy)
{
    char_u *வரி = வரி_arg;
    colnr_T len = len_arg;

    எனில் (வரி == வெற்று)		// just checking...
	திரும்பு FAIL;

    // When starting up, we might still need to create the memfile
    எனில் (curbuf->b_ml.ml_mfp == வெற்று && open_buffer(FALSE, வெற்று, 0) == FAIL)
	திரும்பு FAIL;

    எனில் (!has_props)
	++len;  // include the NUL after the text
    எனில் (copy)
    {
	// copy the line to allocated memory
#ifdef FEAT_PROP_POPUP
	எனில் (has_props)
	    வரி = vim_memsave(வரி, len);
	இல்லையெனில்
#endif
	    வரி = vim_strnsave(வரி, len - 1);
	எனில் (வரி == வெற்று)
	    திரும்பு FAIL;
    }

#ifdef FEAT_NETBEANS_INTG
    எனில் (netbeans_active())
    {
	netbeans_removed(curbuf, lnum, 0, (long)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, வரி, (முழு)STRLEN(வரி));
    }
#endif
    எனில் (curbuf->b_ml.ml_வரி_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_வரி(curbuf);

#ifdef FEAT_PROP_POPUP
	எனில் (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#endif
    }

#ifdef FEAT_PROP_POPUP
    எனில் (curbuf->b_has_textprop && !has_props)
    {
	size_t	oldtextlen = STRLEN(curbuf->b_ml.ml_வரி_ptr) + 1;

	எனில் (oldtextlen < (size_t)curbuf->b_ml.ml_வரி_len)
	{
	    char_u *newline;
	    size_t textproplen = curbuf->b_ml.ml_வரி_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    எனில் (newline != வெற்று)
	    {
		mch_memmove(newline, வரி, len);
		mch_memmove(newline + len, curbuf->b_ml.ml_வரி_ptr
						    + oldtextlen, textproplen);
		vim_free(வரி);
		வரி = newline;
		len += (colnr_T)textproplen;
	    }
	}
    }
#endif

    எனில் (curbuf->b_ml.ml_flags & (ML_வரி_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_வரி_ptr);	// free allocated line

    curbuf->b_ml.ml_வரி_ptr = வரி;
    curbuf->b_ml.ml_வரி_len = len;
    curbuf->b_ml.ml_வரி_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_வரி_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_வரி_DIRTY) & ~ML_EMPTY;

    திரும்பு OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * Adjust text properties in line "lnum" for a deleted line.
 * When "above" is true this is the line above the deleted line, otherwise this
 * is the line below the deleted line.
 * "del_props[del_props_len]" are the properties of the deleted line.
 */
    நிலையான வெறுமை
adjust_text_props_for_delete(
	buf_T	    *buf,
	linenr_T    lnum,
	char_u	    *del_props,
	முழு	    del_props_len,
	முழு	    above)
{
    முழு		did_get_வரி = FALSE;
    முழு		done_del;
    முழு		done_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    DATA_BL	*dp;
    முழு		idx;
    முழு		வரி_start;
    long	வரி_size;
    முழு		this_props_len = 0;
    char_u	*text;
    size_t	textlen;
    முழு		found;

    க்கு (done_del = 0; done_del < del_props_len; done_del += அளவு(textprop_T))
    {
	mch_memmove(&prop_del, del_props + done_del, அளவு(textprop_T));
	எனில் ((above && (prop_del.tp_flags & TP_FLAG_CONT_PREV)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_NEXT))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_NEXT)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_PREV)))
	{
	    எனில் (!did_get_வரி)
	    {
		did_get_வரி = TRUE;
		எனில் ((hp = ml_find_வரி(buf, lnum, ML_FIND)) == வெற்று)
		    திரும்பு;

		dp = (DATA_BL *)(hp->bh_data);
		idx = lnum - buf->b_ml.ml_locked_கீழ்;
		வரி_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
		எனில் (idx == 0)		// first line in block, text at the end
		    வரி_size = dp->db_txt_end - வரி_start;
		இல்லையெனில்
		    வரி_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK)
								  - வரி_start;
		text = (char_u *)dp + வரி_start;
		textlen = STRLEN(text) + 1;
		எனில் ((long)textlen >= வரி_size)
		{
		    // No properties on this line.
		    எனில் (above)
			internal_error("no text property above deleted line");
		    இல்லையெனில்
			internal_error("no text property below deleted line");
		    திரும்பு;
		}
		எனில் ((long)textlen + (long)PROP_மொத்தம்_SIZE > வரி_size)
		{
		    internal_error("text property data too short");
		    திரும்பு;
		}

		uint16_t pc;

		mch_memmove(&pc, text + textlen, PROP_மொத்தம்_SIZE);
		எனில் (!text_prop_மொத்தம்_valid(pc, (size_t)(வரி_size - (long)textlen)))
		{
		    internal_error("text property count too large");
		    திரும்பு;
		}
		this_props_len = pc * (முழு)அளவு(textprop_T);
	    }

	    found = FALSE;
	    {
		char_u *props_start = text + textlen + PROP_மொத்தம்_SIZE;

		க்கு (done_this = 0; done_this < this_props_len;
					       done_this += அளவு(textprop_T))
		{
		    முழு		flag = above ? TP_FLAG_CONT_NEXT
							   : TP_FLAG_CONT_PREV;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_start + done_this,
							   அளவு(textprop_T));
		    எனில் ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			found = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_start + done_this, &prop_this,
							   அளவு(textprop_T));
			நிறுத்து;
		    }
		}
	    }
	    எனில் (!found)
	    {
		எனில் (above)
		    internal_error("text property above deleted line not found");
		இல்லையெனில்
		    internal_error("text property below deleted line not found");
	    }

	    buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
	}
    }
}
#endif

/*
 * Delete line "lnum" in the current buffer.
 * When "flags" has ML_DEL_MESSAGE may give a "No lines in buffer" message.
 * When "flags" has ML_DEL_UNDO this is called from undo.
 *
 * return FAIL for failure, OK otherwise
 */
    நிலையான முழு
ml_delete_int(buf_T *buf, linenr_T lnum, முழு flags)
{
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    முழு		மொத்தம்;	    // number of entries in block
    முழு		idx;
    முழு		அடுக்கு_idx;
    முழு		text_start;
    முழு		வரி_start;
    long	வரி_size;
    முழு		i;
    முழு		ret = FAIL;
#ifdef FEAT_PROP_POPUP
    char_u	*textprop_save = வெற்று;
    long	textprop_len = 0;
#endif

    எனில் (lowest_marked && lowest_marked > lnum)
	lowest_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    எனில் (buf->b_ml.ml_வரி_மொத்தம் == 1)	    // file becomes empty
    {
	எனில் ((flags & ML_DEL_MESSAGE)
#ifdef FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_வரிகள்
#endif
	   )
	    set_keep_செய்தி((char_u *)_(no_வரிகள்_செய்தி), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_EMPTY;

	திரும்பு i;
    }

/*
 * Find the data block containing the line.
 * This also fills the stack with the blocks from the root to the data block.
 * This also releases any locked block..
 */
    mfp = buf->b_ml.ml_mfp;
    எனில் (mfp == வெற்று)
	திரும்பு FAIL;

    எனில் ((hp = ml_find_வரி(buf, lnum, ML_DELETE)) == வெற்று)
	திரும்பு FAIL;

    dp = (DATA_BL *)(hp->bh_data);
    // compute line count before the delete
    மொத்தம் = (long)(buf->b_ml.ml_locked_உயர்வு)
					- (long)(buf->b_ml.ml_locked_கீழ்) + 2;
    idx = lnum - buf->b_ml.ml_locked_கீழ்;

    --buf->b_ml.ml_வரி_மொத்தம்;

    வரி_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
    எனில் (idx == 0)		// first line in block, text at the end
	வரி_size = dp->db_txt_end - வரி_start;
    இல்லையெனில்
	வரி_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK) - வரி_start;

#ifdef FEAT_NETBEANS_INTG
    எனில் (netbeans_active())
	netbeans_removed(buf, lnum, 0, வரி_size);
#endif
#ifdef FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    எனில் (buf->b_has_textprop)
    {
	size_t	textlen = STRLEN((char_u *)dp + வரி_start) + 1;

	textprop_len = வரி_size - (long)textlen;
	எனில் (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + வரி_start + textlen,
								 textprop_len);
    }
#endif

/*
 * special case: If there is only one line in the data block it becomes empty.
 * Then we have to remove the entry, pointing to this data block, from the
 * pointer block. If this pointer block also becomes empty, we go up another
 * block, and so on, up to the root if necessary.
 * The line counts in the pointer blocks have already been adjusted by
 * ml_find_line().
 */
    எனில் (மொத்தம் == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = வெற்று;

	க்கு (அடுக்கு_idx = buf->b_ml.ml_அடுக்கு_உச்சி - 1; அடுக்கு_idx >= 0;
								  --அடுக்கு_idx)
	{
	    buf->b_ml.ml_அடுக்கு_உச்சி = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_அடுக்கு[அடுக்கு_idx]);
	    idx = ip->ip_index;
	    எனில் ((hp = mf_get(mfp, ip->ip_bnum, 1)) == வெற்று)
		செல் theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    எனில் (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		செல் theend;
	    }
	    மொத்தம் = --(pp->pb_மொத்தம்);
	    எனில் (மொத்தம் == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    இல்லையெனில்
	    {
		எனில் (மொத்தம் != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_pointer[idx], &pp->pb_pointer[idx + 1],
				      (size_t)(மொத்தம் - idx) * அளவு(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_அடுக்கு_உச்சி = அடுக்கு_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		எனில் (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_அடுக்கு[buf->b_ml.ml_அடுக்கு_உச்சி].ip_உயர்வு +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_அடுக்கு_உச்சி);

		நிறுத்து;
	    }
	}
	CHECK(அடுக்கு_idx < 0, _("deleted block 1?"));
    }
    இல்லையெனில்
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	text_start = dp->db_txt_start;
	mch_memmove((எழுத்து *)dp + text_start + வரி_size,
		  (எழுத்து *)dp + text_start, (size_t)(வரி_start - text_start));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	க்கு (i = idx; i < மொத்தம் - 1; ++i)
	    dp->db_index[i] = dp->db_index[i + 1] + வரி_size;

	dp->db_free += வரி_size + INDEX_SIZE;
	dp->db_txt_start += வரி_size;
	--(dp->db_வரி_மொத்தம்);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#ifdef FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, வரி_size
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    எனில் (textprop_save != வெற்று)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_data;
	முழு	    props_bytes;

	mch_memmove(&pc, textprop_save, PROP_மொத்தம்_SIZE);
	props_data = textprop_save + PROP_மொத்தம்_SIZE;
	props_bytes = pc * (முழு)அளவு(textprop_T);
	எனில் (!text_prop_மொத்தம்_valid(pc, (size_t)textprop_len))
	    props_bytes = 0;

	// Adjust text properties in the line above and below.
	எனில் (lnum > 1)
	    adjust_text_props_for_delete(buf, lnum - 1,
					     props_data, props_bytes, TRUE);
	எனில் (lnum <= buf->b_ml.ml_வரி_மொத்தம்)
	    adjust_text_props_for_delete(buf, lnum,
					    props_data, props_bytes, FALSE);
    }
    vim_free(textprop_save);
#endif
    திரும்பு ret;
}

/*
 * Delete line "lnum" in the current buffer.
 * When "message" is TRUE may give a "No lines in buffer" message.
 *
 * Check: The caller of this function should probably also call
 * deleted_lines() after this.
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
ml_delete(linenr_T lnum)
{
    திரும்பு ml_delete_flags(lnum, 0);
}

/*
 * Like ml_delete() but using flags (see ml_delete_int()).
 */
    முழு
ml_delete_flags(linenr_T lnum, முழு flags)
{
    ml_flush_வரி(curbuf);
    எனில் (lnum < 1 || lnum > curbuf->b_ml.ml_வரி_மொத்தம்)
	திரும்பு FAIL;

#ifdef FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#endif

    திரும்பு ml_delete_int(curbuf, lnum, flags);
}

/*
 * set the DB_MARKED flag for line 'lnum'
 */
    வெறுமை
ml_setmarked(linenr_T lnum)
{
    bhdr_T    *hp;
    DATA_BL *dp;
				    // invalid line number
    எனில் (lnum < 1 || lnum > curbuf->b_ml.ml_வரி_மொத்தம்
					       || curbuf->b_ml.ml_mfp == வெற்று)
	திரும்பு;			    // give error message?

    எனில் (lowest_marked == 0 || lowest_marked > lnum)
	lowest_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    எனில் ((hp = ml_find_வரி(curbuf, lnum, ML_FIND)) == வெற்று)
	திரும்பு;		    // give error message?

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[lnum - curbuf->b_ml.ml_locked_கீழ்] |= DB_MARKED;
    curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
}

/*
 * find the first line with its DB_MARKED flag set
 */
    linenr_T
ml_firstmarked(வெறுமை)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    முழு		i;

    எனில் (curbuf->b_ml.ml_mfp == வெற்று)
	திரும்பு (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    க்கு (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_வரி_மொத்தம்; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	எனில் ((hp = ml_find_வரி(curbuf, lnum, ML_FIND)) == வெற்று)
	    திரும்பு (linenr_T)0;		    // give error message?

	dp = (DATA_BL *)(hp->bh_data);

	க்கு (i = lnum - curbuf->b_ml.ml_locked_கீழ்;
			    lnum <= curbuf->b_ml.ml_locked_உயர்வு; ++i, ++lnum)
	    எனில் ((dp->db_index[i]) & DB_MARKED)
	    {
		(dp->db_index[i]) &= DB_INDEX_MASK;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		lowest_marked = lnum + 1;
		திரும்பு lnum;
	    }
    }

    திரும்பு (linenr_T) 0;
}

/*
 * clear all DB_MARKED flags
 */
    வெறுமை
ml_clearmarked(வெறுமை)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    முழு		i;

    எனில் (curbuf->b_ml.ml_mfp == வெற்று)	    // nothing to do
	திரும்பு;

    /*
     * The search starts with line lowest_marked.
     */
    க்கு (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_வரி_மொத்தம்; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	எனில் ((hp = ml_find_வரி(curbuf, lnum, ML_FIND)) == வெற்று)
	    திரும்பு;		// give error message?

	dp = (DATA_BL *)(hp->bh_data);

	க்கு (i = lnum - curbuf->b_ml.ml_locked_கீழ்;
			    lnum <= curbuf->b_ml.ml_locked_உயர்வு; ++i, ++lnum)
	    எனில் ((dp->db_index[i]) & DB_MARKED)
	    {
		(dp->db_index[i]) &= DB_INDEX_MASK;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	    }
    }

    lowest_marked = 0;
}

/*
 * flush ml_line if necessary
 */
    நிலையான வெறுமை
ml_flush_வரி(buf_T *buf)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    char_u	*new_வரி;
    char_u	*old_வரி;
    colnr_T	new_len;
    முழு		old_len;
    முழு		extra;
    முழு		idx;
    முழு		start;
    முழு		மொத்தம்;
    முழு		i;
    நிலையான முழு  entered = FALSE;

    எனில் (buf->b_ml.ml_வரி_lnum == 0 || buf->b_ml.ml_mfp == வெற்று)
	திரும்பு;		// nothing to do

    எனில் (buf->b_ml.ml_flags & ML_வரி_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	எனில் (entered)
	    திரும்பு;
	entered = TRUE;

	lnum = buf->b_ml.ml_வரி_lnum;
	new_வரி = buf->b_ml.ml_வரி_ptr;

	hp = ml_find_வரி(buf, lnum, ML_FIND);
	எனில் (hp == வெற்று)
	    siemsg(e_cannot_find_வரி_nr, lnum);
	இல்லையெனில்
	{
	    dp = (DATA_BL *)(hp->bh_data);
	    idx = lnum - buf->b_ml.ml_locked_கீழ்;
	    start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	    old_வரி = (char_u *)dp + start;
	    எனில் (idx == 0)	// line is last in block
		old_len = dp->db_txt_end - start;
	    இல்லையெனில்		// text of previous line follows
		old_len = (dp->db_index[idx - 1] & DB_INDEX_MASK) - start;
	    new_len = buf->b_ml.ml_வரி_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    எனில் ((முழு)dp->db_free >= extra)
	    {
#எனில் defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		முழு old_prop_len = 0;
		எனில் (buf->b_has_textprop)
		    old_prop_len = old_len - (முழு)STRLEN(old_வரி) - 1;
#endif
		// if the length changes and there are following lines
		மொத்தம் = buf->b_ml.ml_locked_உயர்வு - buf->b_ml.ml_locked_கீழ் + 1;
		எனில் (extra != 0 && idx < மொத்தம் - 1)
		{
		    // move text of following lines
		    mch_memmove((எழுத்து *)dp + dp->db_txt_start - extra,
				(எழுத்து *)dp + dp->db_txt_start,
				(size_t)(start - dp->db_txt_start));

		    // adjust pointers of this and following lines
		    க்கு (i = idx + 1; i < மொத்தம்; ++i)
			dp->db_index[i] -= extra;
		}
		dp->db_index[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_start -= extra;

		// copy new line into the data block
		mch_memmove(old_வரி - extra, new_வரி, (size_t)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#எனில் defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		எனில் (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (முழு)STRLEN(new_வரி) - 1;
		}
		எனில் (extra != 0)
		    ml_updatechunk(buf, lnum, (long)extra, ML_CHNK_UPDLINE);
#endif
	    }
	    இல்லையெனில்
	    {
		/*
		 * Cannot do it in one data block: Delete and append.
		 * Append first, because ml_delete_int() cannot delete the
		 * last line in a buffer, which causes trouble for a buffer
		 * that has only one line.
		 * Don't forget to copy the mark!
		 */
		// How about handling errors???
		(வெறுமை)ml_append_int(buf, lnum, new_வரி, new_len,
			 ((dp->db_index[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#ifdef FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#endif
			 );
		(வெறுமை)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_வரி);

	entered = FALSE;
    }
    இல்லையெனில் எனில் (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_வரி_ptr);

    buf->b_ml.ml_flags &= ~(ML_வரி_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_வரி_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    நிலையான bhdr_T *
ml_new_data(memfile_T *mfp, முழு negative, முழு page_மொத்தம்)
{
    bhdr_T	*hp;
    DATA_BL	*dp;

    எனில் ((hp = mf_new(mfp, negative, page_மொத்தம்)) == வெற்று)
	திரும்பு வெற்று;

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_id = DATA_ID;
    dp->db_txt_start = dp->db_txt_end = page_மொத்தம் * mfp->mf_page_size;
    dp->db_free = dp->db_txt_start - HEADER_SIZE;
    dp->db_வரி_மொத்தம் = 0;

    திரும்பு hp;
}

/*
 * create a new, empty, pointer block
 */
    நிலையான bhdr_T *
ml_new_ptr(memfile_T *mfp)
{
    bhdr_T	*hp;
    PTR_BL	*pp;

    எனில் ((hp = mf_new(mfp, FALSE, 1)) == வெற்று)
	திரும்பு வெற்று;

    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_id = PTR_ID;
    pp->pb_மொத்தம் = 0;
    pp->pb_மொத்தம்_max = PB_மொத்தம்_MAX(mfp);

    திரும்பு hp;
}

/*
 * Lookup line 'lnum' in a memline.
 *
 *   action: if ML_DELETE or ML_INSERT the line count is updated while searching
 *	     if ML_FLUSH only flush a locked block
 *	     if ML_FIND just find the line
 *
 * If the block was found it is locked and put in ml_locked.
 * The stack is updated to lead to the locked block. The ip_high field in
 * the stack is updated to reflect the last line in the block AFTER the
 * insert or delete, also if the pointer block has not been updated yet. But
 * if ml_locked != NULL ml_locked_lineadd must be added to ip_high.
 *
 * return: NULL for failure, pointer to block header otherwise
 */
    நிலையான bhdr_T *
ml_find_வரி(buf_T *buf, linenr_T lnum, முழு action)
{
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    முழு		dirty;
    linenr_T	கீழ், உயர்வு;
    முழு		உச்சி;
    முழு		page_மொத்தம்;
    முழு		idx;

    mfp = buf->b_ml.ml_mfp;

    /*
     * If there is a locked block check if the wanted line is in it.
     * If not, flush and release the locked block.
     * Don't do this for ML_INSERT_SAME, because the stack need to be updated.
     * Don't do this for ML_FLUSH, because we want to flush the locked block.
     * Don't do this when 'swapfile' is reset, we want to load all the blocks.
     */
    எனில் (buf->b_ml.ml_locked)
    {
	எனில் (ML_SIMPLE(action)
		&& buf->b_ml.ml_locked_கீழ் <= lnum
		&& buf->b_ml.ml_locked_உயர்வு >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    எனில் (action == ML_செருகு)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_உயர்வு);
	    }
	    இல்லையெனில் எனில் (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_உயர்வு);
	    }
	    திரும்பு (buf->b_ml.ml_locked);
	}

	mf_put(mfp, buf->b_ml.ml_locked, buf->b_ml.ml_flags & ML_LOCKED_DIRTY,
					    buf->b_ml.ml_flags & ML_LOCKED_POS);
	buf->b_ml.ml_locked = வெற்று;

	/*
	 * If lines have been added or deleted in the locked block, need to
	 * update the line count in pointer blocks.
	 */
	எனில் (buf->b_ml.ml_locked_lineadd != 0)
	    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
    }

    எனில் (action == ML_FLUSH)	    // nothing else to do
	திரும்பு வெற்று;

    bnum = 1;			    // start at the root of the tree
    page_மொத்தம் = 1;
    கீழ் = 1;
    உயர்வு = buf->b_ml.ml_வரி_மொத்தம்;

    எனில் (action == ML_FIND)	// first try stack entries
    {
	க்கு (உச்சி = buf->b_ml.ml_அடுக்கு_உச்சி - 1; உச்சி >= 0; --உச்சி)
	{
	    ip = &(buf->b_ml.ml_அடுக்கு[உச்சி]);
	    எனில் (ip->ip_கீழ் <= lnum && ip->ip_உயர்வு >= lnum)
	    {
		bnum = ip->ip_bnum;
		கீழ் = ip->ip_கீழ்;
		உயர்வு = ip->ip_உயர்வு;
		buf->b_ml.ml_அடுக்கு_உச்சி = உச்சி;	// truncate stack at prev entry
		நிறுத்து;
	    }
	}
	எனில் (உச்சி < 0)
	    buf->b_ml.ml_அடுக்கு_உச்சி = 0;		// not found, start at the root
    }
    இல்லையெனில்	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_அடுக்கு_உச்சி = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    க்கு (;;)
    {
	எனில் ((hp = mf_get(mfp, bnum, page_மொத்தம்)) == வெற்று)
	    செல் error_noblock;

	/*
	 * update high for insert/delete
	 */
	எனில் (action == ML_செருகு)
	    ++உயர்வு;
	இல்லையெனில் எனில் (action == ML_DELETE)
	    --உயர்வு;

	dp = (DATA_BL *)(hp->bh_data);
	எனில் (dp->db_id == DATA_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_கீழ் = கீழ்;
	    buf->b_ml.ml_locked_உயர்வு = உயர்வு;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    திரும்பு hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	எனில் (pp->pb_id != PTR_ID)
	{
	    iemsg(e_pointer_block_id_wrong);
	    செல் error_block;
	}

	எனில் ((உச்சி = ml_add_அடுக்கு(buf)) < 0)	// add new entry to stack
	    செல் error_block;
	ip = &(buf->b_ml.ml_அடுக்கு[உச்சி]);
	ip->ip_bnum = bnum;
	ip->ip_கீழ் = கீழ்;
	ip->ip_உயர்வு = உயர்வு;
	ip->ip_index = -1;		// index not known yet

	dirty = FALSE;
	க்கு (idx = 0; idx < (முழு)pp->pb_மொத்தம்; ++idx)
	{
	    t = pp->pb_pointer[idx].pe_வரி_மொத்தம்;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    எனில் ((கீழ் += t) > lnum)
	    {
		ip->ip_index = idx;
		bnum = pp->pb_pointer[idx].pe_bnum;
		page_மொத்தம் = pp->pb_pointer[idx].pe_page_மொத்தம்;
		உயர்வு = கீழ் - 1;
		கீழ் -= t;

		/*
		 * a negative block number may have been changed
		 */
		எனில் (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    எனில் (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_pointer[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		நிறுத்து;
	    }
	}
	எனில் (idx >= (முழு)pp->pb_மொத்தம்)	    // past the end: something wrong!
	{
	    எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம்)
		siemsg(e_வரி_எண்_out_of_range_nr_past_the_end,
					      lnum - buf->b_ml.ml_வரி_மொத்தம்);

	    இல்லையெனில்
		siemsg(e_வரி_மொத்தம்_wrong_in_block_nr, bnum);
	    செல் error_block;
	}
	எனில் (action == ML_DELETE)
	{
	    pp->pb_pointer[idx].pe_வரி_மொத்தம்--;
	    dirty = TRUE;
	}
	இல்லையெனில் எனில் (action == ML_செருகு)
	{
	    pp->pb_pointer[idx].pe_வரி_மொத்தம்++;
	    dirty = TRUE;
	}
	mf_put(mfp, hp, dirty, FALSE);
    }

error_block:
    mf_put(mfp, hp, FALSE, FALSE);
error_noblock:
    /*
     * If action is ML_DELETE or ML_INSERT we have to correct the tree for
     * the incremented/decremented line counts, because there won't be a line
     * inserted/deleted after all.
     */
    எனில் (action == ML_DELETE)
	ml_lineadd(buf, 1);
    இல்லையெனில் எனில் (action == ML_செருகு)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_அடுக்கு_உச்சி = 0;
    திரும்பு வெற்று;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    நிலையான முழு
ml_add_அடுக்கு(buf_T *buf)
{
    முழு		உச்சி;
    infoptr_T	*newstack;

    உச்சி = buf->b_ml.ml_அடுக்கு_உச்சி;

    // may have to increase the stack size
    எனில் (உச்சி == buf->b_ml.ml_அடுக்கு_size)
    {
	CHECK(உச்சி > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_அடுக்கு_size + அடுக்கு_INCR);
	எனில் (newstack == வெற்று)
	    திரும்பு -1;
	எனில் (உச்சி > 0)
	    mch_memmove(newstack, buf->b_ml.ml_அடுக்கு,
					     (size_t)உச்சி * அளவு(infoptr_T));
	vim_free(buf->b_ml.ml_அடுக்கு);
	buf->b_ml.ml_அடுக்கு = newstack;
	buf->b_ml.ml_அடுக்கு_size += அடுக்கு_INCR;
    }

    buf->b_ml.ml_அடுக்கு_உச்சி++;
    திரும்பு உச்சி;
}

/*
 * Update the pointer blocks on the stack for inserted/deleted lines.
 * The stack itself is also updated.
 *
 * When a insert/delete line action fails, the line is not inserted/deleted,
 * but the pointer blocks have already been updated. That is fixed here by
 * walking through the stack.
 *
 * Count is the number of lines added, negative if lines have been deleted.
 */
    நிலையான வெறுமை
ml_lineadd(buf_T *buf, முழு மொத்தம்)
{
    முழு		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    க்கு (idx = buf->b_ml.ml_அடுக்கு_உச்சி - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_அடுக்கு[idx]);
	எனில் ((hp = mf_get(mfp, ip->ip_bnum, 1)) == வெற்று)
	    நிறுத்து;
	pp = (PTR_BL *)(hp->bh_data);	// must be pointer block
	எனில் (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_pointer_block_id_wrong_two);
	    நிறுத்து;
	}
	pp->pb_pointer[ip->ip_index].pe_வரி_மொத்தம் += மொத்தம்;
	ip->ip_உயர்வு += மொத்தம்;
	mf_put(mfp, hp, TRUE, FALSE);
    }
}

#எனில் defined(HAVE_READLINK)
/*
 * Resolve a symlink in the last component of a file name.
 * Note that f_resolve() does it for every part of the path, we don't do that
 * here.
 * If it worked returns OK and the resolved link in "buf[MAXPATHL]".
 * Otherwise returns FAIL.
 */
    முழு
resolve_symlink(char_u *fname, char_u *buf)
{
    char_u	tmp[MAXPATHL];
    முழு		ret;
    முழு		depth = 0;

    எனில் (fname == வெற்று)
	திரும்பு FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    க்கு (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	எனில் (++depth == 100)
	{
	    semsg(_(e_symlink_loop_for_str), fname);
	    திரும்பு FAIL;
	}

	ret = readlink((எழுத்து *)tmp, (எழுத்து *)buf, MAXPATHL - 1);
	எனில் (ret <= 0)
	{
	    எனில் (errno == EINVAL || errno == ENOENT)
	    {
		// Found non-symlink or not existing file, stop here.
		// When at the first level use the unmodified name, skip the
		// call to vim_FullName().
		எனில் (depth == 1)
		    திரும்பு FAIL;

		// Use the resolved name in tmp[].
		நிறுத்து;
	    }

	    // There must be some error reading links, use original name.
	    திரும்பு FAIL;
	}
	buf[ret] = NUL;

	/*
	 * Check whether the symlink is relative or absolute.
	 * If it's relative, build a new path based on the directory
	 * portion of the filename (if any) and the path the symlink
	 * points to.
	 */
	எனில் (mch_isFullName(buf))
	    STRCPY(tmp, buf);
	இல்லையெனில்
	{
	    char_u *tail;

	    tail = gettail(tmp);
	    எனில் (STRLEN(tail) + STRLEN(buf) >= MAXPATHL)
		திரும்பு FAIL;
	    STRCPY(tail, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    திரும்பு vim_FullName(tmp, buf, MAXPATHL, TRUE);
}
#endif

/*
 * Make swap file name out of the file name and a directory name.
 * Returns pointer to allocated memory or NULL.
 */
    char_u *
makeswapname(
    char_u	*fname,
    char_u	*ffname UNUSED,
    buf_T	*buf,
    char_u	*dir_name)
{
    char_u	*r, *s;
    char_u	*fname_res = fname;
#ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];

    // Expand symlink in the file name, so that we put the swap file with the
    // actual file instead of with the symlink.
    எனில் (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#endif

#எனில் defined(UNIX) || defined(MSWIN)  // Need _very_ long file names
    முழு		len = (முழு)STRLEN(dir_name);

    s = dir_name + len;
    எனில் (after_pathsep(dir_name, s) && len > 1 && s[-1] == s[-2])
    {			       // Ends with '//', Use Full path
	r = வெற்று;
	எனில் ((s = make_percent_swname(dir_name, s, fname_res)) != வெற்று)
	{
	    r = modname(s, (char_u *)".swp", FALSE);
	    vim_free(s);
	}
	திரும்பு r;
    }
#endif

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#எனில் defined(VMS)
	    "_swp",
#இல்லையெனில்
	    ".swp",
#endif
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    எனில் (r == வெற்று)	    // out of memory
	திரும்பு வெற்று;

    s = get_file_in_dir(r, dir_name);
    vim_free(r);
    திரும்பு s;
}

/*
 * Get file name to use for swap file or backup file.
 * Use the name of the edited file "fname" and an entry in the 'dir' or 'bdir'
 * option "dname".
 * - If "dname" is ".", return "fname" (swap file in dir of file).
 * - If "dname" starts with "./", insert "dname" in "fname" (swap file
 *   relative to dir of file).
 * - Otherwise, prepend "dname" to the tail of "fname" (swap file in specific
 *   dir).
 *
 * The return value is an allocated string and can be NULL.
 */
    char_u *
get_file_in_dir(
    char_u  *fname,
    char_u  *dname)	// don't use "dirname", it is a global for Alpha
{
    string_T	tail;
    string_T	retval;

    tail.string = gettail(fname);
    tail.length = STRLEN(tail.string);

    எனில் (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (size_t)(tail.string - fname) + tail.length);
    இல்லையெனில்
    {
	size_t	dname_len = STRLEN(dname);

	எனில் (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    எனில் (tail.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    tail.string, tail.length, TRUE, &retval);
	    இல்லையெனில்
	    {
		முழு	    save_char;
		string_T    tmp;

		save_char = *tail.string;
		*tail.string = NUL;
		concat_fnames(fname, (size_t)(tail.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*tail.string = save_char;
		எனில் (tmp.string == வெற்று)	    // out of memory
		    retval.string = வெற்று;
		இல்லையெனில்
		{
		    concat_fnames(tmp.string, tmp.length,
			tail.string, tail.length, TRUE, &retval);
		    vim_free(tmp.string);
		}
	    }
	}
	இல்லையெனில்
	    concat_fnames(dname, dname_len, tail.string, tail.length,
		TRUE, &retval);
    }

#ifdef MSWIN
    எனில் (retval.string != வெற்று)
    {
	char_u	*t;

	க்கு (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    எனில் (*t == ':')
		*t = '%';
    }
#endif

    திரும்பு retval.string;
}

/*
 * Print the ATTENTION message: info about an existing swap file.
 */
    நிலையான வெறுமை
attention_message(
    buf_T   *buf,	// buffer being edited
    char_u  *fname)	// swap file name
{
    stat_T	st;
    time_t	swap_mtime;

    ++no_wait_return;
    (வெறுமை)emsg(_(e_attention));
    செய்தி_puts(_("\nFound a swap file by the name \""));
    செய்தி_home_replace(fname);
    செய்தி_puts("\"\n");
    swap_mtime = swapfile_info(fname);
    செய்தி_puts(_("While opening file \""));
    செய்தி_outtrans(buf->b_fname);
    செய்தி_puts("\"\n");
    எனில் (mch_stat((எழுத்து *)buf->b_fname, &st) == -1)
    {
	செய்தி_puts(_("      CANNOT BE FOUND"));
    }
    இல்லையெனில்
    {
	செய்தி_puts(_("             dated: "));
	செய்தி_puts(get_ctime(st.st_mtime, TRUE));
	எனில் (swap_mtime != 0 && st.st_mtime > swap_mtime)
	    செய்தி_puts(_("      NEWER than swap file!\n"));
    }
    // Some of these messages are long to allow translation to
    // other languages.
    செய்தி_puts(_("\n(1) Another program may be editing the same file.  If this is the case,\n    be careful not to end up with two different instances of the same\n    file when making changes.  Quit, or continue with caution.\n"));
    செய்தி_puts(_("(2) An edit session for this file crashed.\n"));
    செய்தி_puts(_("    If this is the case, use \":recover\" or \"vim -r "));
    செய்தி_outtrans(buf->b_fname);
    செய்தி_puts(_("\"\n    to recover the changes (see \":help recovery\").\n"));
    செய்தி_puts(_("    If you did this already, delete the swap file \""));
    செய்தி_outtrans(fname);
    செய்தி_puts(_("\"\n    to avoid this message.\n"));
    cmdline_row = செய்தி_row;
    --no_wait_return;
}

வகைவரை எண்ணிக்கை {
    SEA_CHOICE_NONE = 0,
    SEA_CHOICE_READONLY = 1,
    SEA_CHOICE_EDIT = 2,
    SEA_CHOICE_RECOVER = 3,
    SEA_CHOICE_DELETE = 4,
    SEA_CHOICE_QUIT = 5,
    SEA_CHOICE_ABORT = 6
} sea_choice_T;

#எனில் defined(FEAT_EVAL)
/*
 * Trigger the SwapExists autocommands.
 * Returns a value for equivalent to do_dialog().
 */
    நிலையான sea_choice_T
do_swapexists(buf_T *buf, char_u *fname)
{
    set_vim_var_string(VV_SWAPNAME, fname, -1);
    set_vim_var_string(VV_SWAPCHOICE, வெற்று, -1);

    // Trigger SwapExists autocommands with <afile> set to the file being
    // edited.  Disallow changing directory here.
    ++allbuf_lock;
    apply_autocmds(EVENT_SWAPEXISTS, buf->b_fname, வெற்று, FALSE, வெற்று);
    --allbuf_lock;

    set_vim_var_string(VV_SWAPNAME, வெற்று, -1);

    தேர்வு (*get_vim_var_str(VV_SWAPCHOICE))
    {
	நிலை 'o': திரும்பு SEA_CHOICE_READONLY;
	நிலை 'e': திரும்பு SEA_CHOICE_EDIT;
	நிலை 'r': திரும்பு SEA_CHOICE_RECOVER;
	நிலை 'd': திரும்பு SEA_CHOICE_DELETE;
	நிலை 'q': திரும்பு SEA_CHOICE_QUIT;
	நிலை 'a': திரும்பு SEA_CHOICE_ABORT;
    }

    திரும்பு SEA_CHOICE_NONE;
}
#endif

/*
 * Find out what name to use for the swap file for buffer 'buf'.
 *
 * Several names are tried to find one that does not exist
 * Returns the name in allocated memory or NULL.
 * When out of memory "dirp" is set to NULL.
 *
 * Note: If BASENAMELEN is not correct, you will get error messages for
 *	 not being able to open the swap or undo file
 * Note: May trigger SwapExists autocmd, pointers may change!
 */
    நிலையான char_u *
findswapname(
    buf_T	*buf,
    char_u	**dirp,		// pointer to list of directories
    char_u	*old_fname)	// don't give warning for this file name
{
    char_u	*fname;
    முழு		n;
    char_u	*dir_name;
#ifdef AMIGA
    BPTR	fh;
#endif
    முழு		r;
    char_u	*buf_fname = buf->b_fname;

#எனில் !defined(UNIX)
# define CREATE_DUMMY_FILE
    FILE	*dummyfd = வெற்று;

# ifdef MSWIN
    எனில் (buf_fname != வெற்று && !mch_isFullName(buf_fname)
				       && vim_strchr(gettail(buf_fname), ':'))
    {
	char_u *t;

	buf_fname = vim_strsave(buf_fname);
	எனில் (buf_fname == வெற்று)
	    buf_fname = buf->b_fname;
	இல்லையெனில்
	    க்கு (t = gettail(buf_fname); *t != NUL; MB_PTR_ADV(t))
		எனில் (*t == ':')
		    *t = '%';
    }
# endif

    /*
     * If we start editing a new file, e.g. "test.doc", which resides on an
     * MSDOS compatible filesystem, it is possible that the file
     * "test.doc.swp" which we create will be exactly the same file. To avoid
     * this problem we temporarily create "test.doc".  Don't do this when the
     * check below for an 8.3 file name is used.
     */
    எனில் (!(buf->b_p_sn || buf->b_shortname) && buf_fname != வெற்று
					     && mch_getperm(buf_fname) < 0)
	dummyfd = mch_fopen((எழுத்து *)buf_fname, "w");
#endif

    /*
     * Isolate a directory name from *dirp and put it in dir_name.
     * First allocate some memory to put the directory name in.
     */
    dir_name = alloc(STRLEN(*dirp) + 1);
    எனில் (dir_name == வெற்று)
	*dirp = வெற்று;
    இல்லையெனில்
	(வெறுமை)copy_option_part(dirp, dir_name, 31000, ",");

    /*
     * we try different names until we find one that does not exist yet
     */
    எனில் (dir_name == வெற்று)	    // out of memory
	fname = வெற்று;
    இல்லையெனில்
	fname = makeswapname(buf_fname, buf->b_ffname, buf, dir_name);

    க்கு (;;)
    {
	எனில் (fname == வெற்று)	// must be out of memory
	    நிறுத்து;
	எனில் ((n = (முழு)STRLEN(fname)) == 0)	// safety check
	{
	    VIM_CLEAR(fname);
	    நிறுத்து;
	}
#எனில் defined(UNIX)
/*
 * Some systems have a MS-DOS compatible filesystem that use 8.3 character
 * file names. If this is the first try and the swap file name does not fit in
 * 8.3, detect if this is the case, set shortname and try again.
 */
	எனில் (fname[n - 2] == 'w' && fname[n - 1] == 'p'
					&& !(buf->b_p_sn || buf->b_shortname))
	{
	    char_u	    *tail;
	    char_u	    *fname2;
	    stat_T	    s1, s2;
	    முழு		    f1, f2;
	    முழு		    created1 = FALSE, created2 = FALSE;
	    முழு		    same = FALSE;

	    /*
	     * Check if swapfile name does not fit in 8.3:
	     * It either contains two dots, is longer than 8 chars, or starts
	     * with a dot.
	     */
	    tail = gettail(buf_fname);
	    எனில் (       vim_strchr(tail, '.') != வெற்று
		    || STRLEN(tail) > (size_t)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		எனில் (fname2 != வெற்று)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    எனில் (vim_strchr(tail, '.') != வெற்று)
			fname2[n - 1] = 'x';
		    இல்லையெனில் எனில் (*gettail(fname) == '.')
		    {
			fname2[n] = 'x';
			fname2[n + 1] = NUL;
		    }
		    இல்லையெனில்
			fname2[n - 5] += 1;
		    /*
		     * may need to create the files to be able to use mch_stat()
		     */
		    f1 = mch_open((எழுத்து *)fname, O_RDONLY | O_EXTRA, 0);
		    எனில் (f1 < 0)
		    {
			f1 = mch_open_rw((எழுத்து *)fname,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			created1 = TRUE;
		    }
		    எனில் (f1 >= 0)
		    {
			f2 = mch_open((எழுத்து *)fname2, O_RDONLY | O_EXTRA, 0);
			எனில் (f2 < 0)
			{
			    f2 = mch_open_rw((எழுத்து *)fname2,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			    created2 = TRUE;
			}
			எனில் (f2 >= 0)
			{
			    /*
			     * Both files exist now. If mch_stat() returns the
			     * same device and inode they are the same file.
			     */
			    எனில் (mch_fstat(f1, &s1) != -1
				    && mch_fstat(f2, &s2) != -1
				    && s1.st_dev == s2.st_dev
				    && s1.st_ino == s2.st_ino)
				same = TRUE;
			    close(f2);
			    எனில் (created2)
				mch_remove(fname2);
			}
			close(f1);
			எனில் (created1)
			    mch_remove(fname);
		    }
		    vim_free(fname2);
		    எனில் (same)
		    {
			buf->b_shortname = true;
			vim_free(fname);
			fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
			தொடர்;	// try again with b_shortname set
		    }
		}
	    }
	}
#endif
	/*
	 * check if the swapfile already exists
	 */
	எனில் (mch_getperm(fname) < 0)	// it does not exist
	{
#ifdef HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    எனில் (mch_lstat((எழுத்து *)fname, &sb) < 0)
#இல்லையெனில்
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (long)MODE_NEWFILE);
	    /*
	     * on the Amiga mch_getperm() will return -1 when the file exists
	     * but is being used by another program. This happens if you edit
	     * a file twice.
	     */
	    எனில் (fh != (BPTR)வெற்று)	// can open file, OK
	    {
		Close(fh);
		mch_remove(fname);
		நிறுத்து;
	    }
	    எனில் (IoErr() != ERROR_OBJECT_IN_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#endif
		நிறுத்து;
	}

	/*
	 * A file name equal to old_fname is OK to use.
	 */
	எனில் (old_fname != வெற்று && fnamecmp(fname, old_fname) == 0)
	    நிறுத்து;

	/*
	 * get here when file already exists
	 */
	எனில் (fname[n - 2] == 'w' && fname[n - 1] == 'p')	// first try
	{
	    /*
	     * on MS-DOS compatible filesystems (e.g. messydos) file.doc.swp
	     * and file.doc are the same file. To guess if this problem is
	     * present try if file.doc.swx exists. If it does, we set
	     * buf->b_shortname and try file_doc.swp (dots replaced by
	     * underscores for this file), and try again. If it doesn't we
	     * assume that "file.doc.swp" already exists.
	     */
	    எனில் (!(buf->b_p_sn || buf->b_shortname))	// not tried yet
	    {
		fname[n - 1] = 'x';
		r = mch_getperm(fname);		// try "file.swx"
		fname[n - 1] = 'p';
		எனில் (r >= 0)		    // "file.swx" seems to exist
		{
		    buf->b_shortname = true;
		    vim_free(fname);
		    fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
		    தொடர்;	    // try again with '.' replaced with '_'
		}
	    }
	    /*
	     * If we get here the ".swp" file really exists.
	     * Give an error message, unless recovering, no file name, we are
	     * viewing a help file or when the path of the file is different
	     * (happens when all .swp files are in one directory).
	     */
	    எனில் (!recoverymode && buf_fname != வெற்று
				&& !buf->b_help
				&& !(buf->b_flags & (BF_DUMMY | BF_NO_SEA)))
	    {
		முழு		fd;
		கட்டமைப்பு block0	b0;
		முழு		differ = FALSE;

		/*
		 * Try to read block 0 from the swap file to get the original
		 * file name (and inode number).
		 */
		fd = mch_open((எழுத்து *)fname, O_RDONLY | O_EXTRA, 0);
		எனில் (fd >= 0)
		{
		    எனில் (read_eintr(fd, &b0, அளவு(b0)) == அளவு(b0))
		    {
			/*
			 * If the swapfile has the same directory as the
			 * buffer don't compare the directory names, they can
			 * have a different mountpoint.
			 */
			எனில் (b0.b0_flags & B0_SAME_DIR)
			{
			    எனில் (fnamecmp(gettail(buf->b_ffname),
						   gettail(b0.b0_fname)) != 0
				    || !same_directory(fname, buf->b_ffname))
			    {
#ifdef CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				எனில் (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
#endif
				    differ = TRUE;
			    }
			}
			இல்லையெனில்
			{
			    /*
			     * The name in the swap file may be
			     * "~user/path/file".  Expand it first.
			     */
			    expand_env(b0.b0_fname, NameBuff, MAXPATHL);
#ifdef CHECK_INODE
			    எனில் (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
				differ = TRUE;
#இல்லையெனில்
			    எனில் (fnamecmp(NameBuff, buf->b_ffname) != 0)
				differ = TRUE;
#endif
			}
		    }
		    close(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		எனில் (differ == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == வெற்று)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#ifdef CREATE_DUMMY_FILE
		    முழு		 did_use_dummy = FALSE;

		    // Avoid getting a warning for the file being created
		    // outside of Vim, it was created at the start of this
		    // function.  Delete the file now, because Vim might exit
		    // here if the window is closed.
		    எனில் (dummyfd != வெற்று)
		    {
			fclose(dummyfd);
			dummyfd = வெற்று;
			mch_remove(buf_fname);
			did_use_dummy = TRUE;
		    }
#endif

#ifdef HAVE_PROCESS_STILL_RUNNING
		    process_still_running = FALSE;
#endif
		    // It's safe to delete the swap file if all these are true:
		    // - the edited file exists
		    // - the swap file has no changes and looks OK
		    எனில் (mch_stat((எழுத்து *)buf->b_fname, &st) == 0
						  && swapfile_unchanged(fname))
		    {
			choice = SEA_CHOICE_DELETE;
			எனில் (p_verbose > 0)
			    verb_செய்தி(_("Found a swap file that is not useful, deleting it"));
		    }

#எனில் defined(FEAT_EVAL)
		    /*
		     * If there is an SwapExists autocommand and we can handle
		     * the response, trigger it.  It may return 0 to ask the
		     * user anyway.
		     */
		    எனில் (choice == SEA_CHOICE_NONE
			    && swap_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#endif

		    எனில் (choice == SEA_CHOICE_NONE
					 && swap_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    எனில் (choice == SEA_CHOICE_NONE)
		    {
#ifdef FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			எனில் (gui.starting && !gui.in_use)
			    gui_start(வெற்று);
#endif
			// Show info about the existing swap file.
			attention_message(buf, fname);

			// We don't want a 'q' typed at the more-prompt
			// interrupt loading a file.
			got_int = FALSE;

			// If vimrc has "simalt ~x" we don't want it to
			// interfere with the prompt here.
			flush_buffers(FLUSH_TYPEAHEAD);
		    }

#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    எனில் (swap_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    prefix = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			size_t	    message_size;
			string_T    message;
			char_u	    *tofree;
			முழு	    dialog_result;

			prefix.length = STRLEN(prefix.string);
			suffix.length = STRLEN(suffix.string);
			message_size = prefix.length
				+ STRLEN(fname)
				+ suffix.length + 5;
			message.string = tofree = alloc(message_size);
			எனில் (message.string != வெற்று)
			{
			    STRCPY(message.string, prefix.string);
			    message.length = prefix.length;
			    message.length += home_replace(வெற்று, fname,
				message.string + message.length, (முழு)(message_size - message.length), TRUE);
			    STRCPY(message.string + message.length, suffix.string);
			}
			இல்லையெனில்
			    message.string = (char_u *)_("Swap file already exists!");
			dialog_result = do_dialog(VIM_WARNING,
				    (char_u *)_("VIM - ATTENTION"),
				    message.string,
# ifdef HAVE_PROCESS_STILL_RUNNING
				    process_still_running
					? (char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Quit\n&Abort") :
# endif
					(char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Delete it\n&Quit\n&Abort"), 1, வெற்று, FALSE);

# ifdef HAVE_PROCESS_STILL_RUNNING
			எனில் (process_still_running && dialog_result >= 4)
			    // compensate for missing "Delete it" button
			    dialog_result++;
# endif
			choice = dialog_result;
			vim_free(tofree);

			// pretend screen didn't scroll, need redraw anyway
			செய்தி_scrolled = 0;
			redraw_all_later(UPD_NOT_VALID);
		    }
#endif

		    தேர்வு (choice)
		    {
			நிலை SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    நிறுத்து;
			நிலை SEA_CHOICE_EDIT:
			    நிறுத்து;
			நிலை SEA_CHOICE_RECOVER:
			    swap_exists_action = SEA_RECOVER;
			    நிறுத்து;
			நிலை SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    நிறுத்து;
			நிலை SEA_CHOICE_QUIT:
			    swap_exists_action = SEA_QUIT;
			    நிறுத்து;
			நிலை SEA_CHOICE_ABORT:
			    swap_exists_action = SEA_QUIT;
			    got_int = TRUE;
			    நிறுத்து;
			நிலை SEA_CHOICE_NONE:
			    செய்தி_puts("\n");
			    எனில் (செய்தி_silent == 0)
				// call wait_return() later
				need_wait_return = TRUE;
			    நிறுத்து;
		    }

		    // If the file was deleted this fname can be used.
		    எனில் (choice != SEA_CHOICE_NONE && mch_getperm(fname) < 0)
			நிறுத்து;

#ifdef CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    எனில் (did_use_dummy)
			dummyfd = mch_fopen((எழுத்து *)buf_fname, "w");
#endif
		}
	    }
	}

	/*
	 * Change the ".swp" extension to find another file that can be used.
	 * First decrement the last char: ".swo", ".swn", etc.
	 * If that still isn't enough decrement the last but one char: ".svz"
	 * Can happen when editing many "No Name" buffers.
	 */
	எனில் (fname[n - 1] == 'a')	// ".s?a"
	{
	    எனில் (fname[n - 2] == 'a')    // ".saa": tried enough, give up
	    {
		emsg(_(e_too_many_swap_files_found));
		VIM_CLEAR(fname);
		நிறுத்து;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#ifdef CREATE_DUMMY_FILE
    எனில் (dummyfd != வெற்று)	// file has been created temporarily
    {
	fclose(dummyfd);
	mch_remove(buf_fname);
    }
#endif
#ifdef MSWIN
    எனில் (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#endif
    திரும்பு fname;
}

    நிலையான முழு
b0_magic_wrong(ZERO_BL *b0p)
{
    திரும்பு (b0p->b0_magic_long != (long)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (முழு)B0_MAGIC_INT
	    || b0p->b0_magic_short != (short)B0_MAGIC_SHORT
	    || b0p->b0_magic_char != B0_MAGIC_CHAR);
}

#ifdef CHECK_INODE
/*
 * Compare current file name with file name from swap file.
 * Try to use inode numbers when possible.
 * Return non-zero when files are different.
 *
 * When comparing file names a few things have to be taken into consideration:
 * - When working over a network the full path of a file depends on the host.
 *   We check the inode number if possible.  It is not 100% reliable though,
 *   because the device number cannot be used over a network.
 * - When a file does not exist yet (editing a new file) there is no inode
 *   number.
 * - The file name in a swap file may not be valid on the current host.  The
 *   "~user" form is used whenever possible to avoid this.
 *
 * This is getting complicated, let's make a table:
 *
 *		ino_c  ino_s  fname_c  fname_s	differ =
 *
 * both files exist -> compare inode numbers:
 *		!= 0   != 0	X	 X	ino_c != ino_s
 *
 * inode number(s) unknown, file names available -> compare file names
 *		== 0	X	OK	 OK	fname_c != fname_s
 *		 X     == 0	OK	 OK	fname_c != fname_s
 *
 * current file doesn't exist, file for swap file exist, file name(s) not
 * available -> probably different
 *		== 0   != 0    FAIL	 X	TRUE
 *		== 0   != 0	X	FAIL	TRUE
 *
 * current file exists, inode for swap unknown, file name(s) not
 * available -> probably different
 *		!= 0   == 0    FAIL	 X	TRUE
 *		!= 0   == 0	X	FAIL	TRUE
 *
 * current file doesn't exist, inode for swap unknown, one file name not
 * available -> probably different
 *		== 0   == 0    FAIL	 OK	TRUE
 *		== 0   == 0	OK	FAIL	TRUE
 *
 * current file doesn't exist, inode for swap unknown, both file names not
 * available -> compare file names
 *		== 0   == 0    FAIL	FAIL	fname_c != fname_s
 *
 * Note that when the ino_t is 64 bits, only the last 32 will be used.  This
 * can't be changed without making the block 0 incompatible with 32 bit
 * versions.
 */

    நிலையான முழு
fnamecmp_ino(
    char_u	*fname_c,	    // current file name
    char_u	*fname_s,	    // file name from swap file
    long	ino_block0)
{
    stat_T	st;
    ino_t	ino_c = 0;	    // ino of current file
    ino_t	ino_s;		    // ino of file from swap file
    char_u	buf_c[MAXPATHL];    // full path of fname_c
    char_u	buf_s[MAXPATHL];    // full path of fname_s
    முழு		retval_c;	    // flag: buf_c valid
    முழு		retval_s;	    // flag: buf_s valid

    எனில் (mch_stat((எழுத்து *)fname_c, &st) == 0)
	ino_c = (ino_t)st.st_ino;

    /*
     * First we try to get the inode from the file name, because the inode in
     * the swap file may be outdated.  If that fails (e.g. this path is not
     * valid on this machine), use the inode from block 0.
     */
    எனில் (mch_stat((எழுத்து *)fname_s, &st) == 0)
	ino_s = (ino_t)st.st_ino;
    இல்லையெனில்
	ino_s = (ino_t)ino_block0;

    எனில் (ino_c && ino_s)
	திரும்பு (ino_c != ino_s);

    /*
     * One of the inode numbers is unknown, try a forced vim_FullName() and
     * compare the file names.
     */
    retval_c = vim_FullName(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_FullName(fname_s, buf_s, MAXPATHL, TRUE);
    எனில் (retval_c == OK && retval_s == OK)
	திரும்பு STRCMP(buf_c, buf_s) != 0;

    /*
     * Can't compare inodes or file names, guess that the files are different,
     * unless both appear not to exist at all, then compare with the file name
     * in the swap file.
     */
    எனில் (ino_s == 0 && ino_c == 0 && retval_c == FAIL && retval_s == FAIL)
	திரும்பு STRCMP(fname_c, fname_s) != 0;
    திரும்பு TRUE;
}
#endif // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    நிலையான வெறுமை
long_to_char(long n, char_u *s)
{
    s[0] = (char_u)(n & 0xff);
    n = (unsigned)n >> 8;
    s[1] = (char_u)(n & 0xff);
    n = (unsigned)n >> 8;
    s[2] = (char_u)(n & 0xff);
    n = (unsigned)n >> 8;
    s[3] = (char_u)(n & 0xff);
}

    நிலையான long
char_to_long(char_u *s)
{
    long    retval;

    retval = s[3];
    retval <<= 8;
    retval |= s[2];
    retval <<= 8;
    retval |= s[1];
    retval <<= 8;
    retval |= s[0];

    திரும்பு retval;
}

/*
 * Set the flags in the first block of the swap file:
 * - file is modified or not: buf->b_changed
 * - 'fileformat'
 * - 'fileencoding'
 */
    வெறுமை
ml_setflags(buf_T *buf)
{
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    எனில் (!buf->b_ml.ml_mfp)
	திரும்பு;
    க்கு (hp = buf->b_ml.ml_mfp->mf_used_last; hp != வெற்று; hp = hp->bh_prev)
    {
	எனில் (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_data);
	    b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_MASK)
						  | (get_fileformat(buf) + 1);
	    add_b0_fenc(b0p, buf);
	    hp->bh_flags |= BH_DIRTY;
	    mf_sync(buf->b_ml.ml_mfp, MFS_ZERO);
	    நிறுத்து;
	}
    }
}

#எனில் defined(FEAT_CRYPT)
/*
 * If "data" points to a data block encrypt the text in it and return a copy
 * in allocated memory.  Return NULL when out of memory.
 * Otherwise return "data".
 */
    char_u *
ml_encrypt_data(
    memfile_T	*mfp,
    char_u	*data,
    off_T	offset,
    unsigned	size)
{
    DATA_BL	*dp = (DATA_BL *)data;
    char_u	*head_end;
    char_u	*text_start;
    char_u	*new_data;
    முழு		text_len;
    cryptstate_T *state;

    எனில் (dp->db_id != DATA_ID)
	திரும்பு data;

    state = ml_crypt_prepare(mfp, offset, FALSE);
    எனில் (state == வெற்று)
	திரும்பு data;

    new_data = alloc(size);
    எனில் (new_data == வெற்று)
    {
	crypt_free_state(state);
	திரும்பு வெற்று;
    }
    head_end = (char_u *)(&dp->db_index[dp->db_வரி_மொத்தம்]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = size - dp->db_txt_start;

    // Copy the header and the text.
    mch_memmove(new_data, dp, head_end - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, text_start, text_len, new_data + dp->db_txt_start,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    எனில் (head_end < text_start)
	vim_memset(new_data + (head_end - data), 0, text_start - head_end);

    திரும்பு new_data;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    வெறுமை
ml_decrypt_data(
    memfile_T	*mfp,
    char_u	*data,
    off_T	offset,
    unsigned	size)
{
    DATA_BL	*dp = (DATA_BL *)data;
    char_u	*head_end;
    char_u	*text_start;
    முழு		text_len;
    cryptstate_T *state;

    எனில் (dp->db_id != DATA_ID)
	திரும்பு;

    head_end = (char_u *)(&dp->db_index[dp->db_வரி_மொத்தம்]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = dp->db_txt_end - dp->db_txt_start;

    எனில் (head_end > text_start || dp->db_txt_start > size
	    || dp->db_txt_end > size)
	திரும்பு;  // data was messed up

    state = ml_crypt_prepare(mfp, offset, TRUE);
    எனில் (state == வெற்று)
	திரும்பு;

    // Decrypt the text in place.
    crypt_decode_inplace(state, text_start, text_len, FALSE);
    crypt_free_state(state);
}

/*
 * Prepare for encryption/decryption, using the key, seed and offset.
 * Return an allocated cryptstate_T *.
 * Note: Encryption not supported for SODIUM
 */
    நிலையான cryptstate_T *
ml_crypt_prepare(memfile_T *mfp, off_T offset, முழு reading)
{
    buf_T	*buf = mfp->mf_buffer;
    char_u	salt[50];
    முழு		method_nr;
    char_u	*சாவி;
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    எனில் (reading && mfp->mf_old_சாவி != வெற்று)
    {
	// Reading back blocks with the previous key/method/seed.
	method_nr = mfp->mf_old_cm;
	சாவி = mfp->mf_old_சாவி;
	arg.cat_seed = mfp->mf_old_seed;
    }
    இல்லையெனில்
    {
	method_nr = crypt_get_method_nr(buf);
	சாவி = buf->b_p_சாவி;
	arg.cat_seed = mfp->mf_seed;
    }

    எனில் (*சாவி == NUL)
	திரும்பு வெற்று;

    எனில் (crypt_may_close_swapfile(buf, சாவி, method_nr))
	திரும்பு வெற்று;

    எனில் (method_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((எழுத்து *)salt, அளவு(salt), "%s%ld", சாவி, (long)offset);
	arg.cat_seed = வெற்று;
	arg.cat_init_from_file = FALSE;

	திரும்பு crypt_create(method_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((எழுத்து *)salt, அளவு(salt), "%ld", (long)offset);

    arg.cat_salt = salt;
    arg.cat_salt_len = (முழு)STRLEN(salt);
    arg.cat_seed_len = MF_SEED_LEN;
    arg.cat_add_len = 0;
    arg.cat_add = வெற்று;
    arg.cat_init_from_file = FALSE;

    திரும்பு crypt_create(method_nr, சாவி, &arg);
}

#endif


#எனில் defined(FEAT_BYTEOFF)

# define MLCS_MAXL 800	// max no of lines in chunk
# define MLCS_MINL 400   // should be half of MLCS_MAXL

/*
 * Keep information for finding byte offset of a line, updtype may be one of:
 * ML_CHNK_ADDLINE: Add len to parent chunk, possibly splitting it
 *	   Careful: ML_CHNK_ADDLINE may cause ml_find_line() to be called.
 * ML_CHNK_DELLINE: Subtract len from parent chunk, possibly deleting it
 * ML_CHNK_UPDLINE: Add len to parent chunk, as a signed entity.
 */
    நிலையான வெறுமை
ml_updatechunk(
    buf_T	*buf,
    linenr_T	வரி,
    long	len,
    முழு		updtype)
{
    நிலையான buf_T	*ml_upd_lastbuf = வெற்று;
    நிலையான linenr_T	ml_upd_lastcurline;
    நிலையான முழு		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    முழு			curix = ml_upd_lastcurix;
    long		size;
    chunksize_T		*curchnk;
    முழு			rest;
    bhdr_T		*hp;
    DATA_BL		*dp;

    எனில் (buf->b_ml.ml_usedchunks == -1 || len == 0)
	திரும்பு;
    எனில் (buf->b_ml.ml_chunksize == வெற்று)
    {
	buf->b_ml.ml_chunksize = ALLOC_MULT(chunksize_T, 100);
	எனில் (buf->b_ml.ml_chunksize == வெற்று)
	{
	    buf->b_ml.ml_usedchunks = -1;
	    திரும்பு;
	}
	buf->b_ml.ml_numchunks = 100;
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = 1;
	ml_upd_lastbuf = வெற்று;		// invalidate resume cache
    }

    எனில் (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_வரி_மொத்தம் == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (long)buf->b_ml.ml_வரி_len;
	ml_upd_lastbuf = வெற்று;		// invalidate resume cache
	திரும்பு;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    எனில் (buf != ml_upd_lastbuf || வரி < curline)
    {
	curline = 1;
	curix = 0;
    }
    க்கு (; curix < buf->b_ml.ml_usedchunks - 1
	     && வரி >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	     curix++)
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
    curchnk = buf->b_ml.ml_chunksize + curix;

    எனில் (updtype == ML_CHNK_DELLINE)
	len = -len;
    curchnk->mlcs_totalsize += len;
    எனில் (updtype == ML_CHNK_ADDLINE)
    {
	curchnk->mlcs_numlines++;

	// May resize here so we don't have to do it in both cases below
	எனில் (buf->b_ml.ml_usedchunks + 1 >= buf->b_ml.ml_numchunks)
	{
	    chunksize_T *t_chunksize = buf->b_ml.ml_chunksize;

	    buf->b_ml.ml_numchunks = buf->b_ml.ml_numchunks * 3 / 2;
	    buf->b_ml.ml_chunksize = vim_realloc(buf->b_ml.ml_chunksize,
			    அளவு(chunksize_T) * buf->b_ml.ml_numchunks);
	    எனில் (buf->b_ml.ml_chunksize == வெற்று)
	    {
		// Hmmmm, Give up on offset for this buffer
		vim_free(t_chunksize);
		buf->b_ml.ml_usedchunks = -1;
		திரும்பு;
	    }
	}

	எனில் (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MAXL)
	{
	    முழு	    மொத்தம்;	    // number of entries in block
	    முழு	    idx;
	    முழு	    end_idx;
	    முழு	    text_end;
	    முழு	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			அளவு(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    size = 0;
	    linecnt = 0;
	    போது (curline < buf->b_ml.ml_வரி_மொத்தம்
			&& linecnt < MLCS_MINL)
	    {
		எனில் ((hp = ml_find_வரி(buf, curline, ML_FIND)) == வெற்று)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    திரும்பு;
		}
		dp = (DATA_BL *)(hp->bh_data);
		மொத்தம் = (long)(buf->b_ml.ml_locked_உயர்வு) -
			(long)(buf->b_ml.ml_locked_கீழ்) + 1;
		idx = curline - buf->b_ml.ml_locked_கீழ்;
		curline = buf->b_ml.ml_locked_உயர்வு + 1;

		// compute index of last line to use in this MEMLINE
		rest = மொத்தம் - idx;
		எனில் (linecnt + rest > MLCS_MINL)
		{
		    end_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		இல்லையெனில்
		{
		    end_idx = மொத்தம் - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		எனில் (buf->b_has_textprop)
		{
		    முழு i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    க்கு (i = end_idx; i < idx; ++i)
			size += (முழு)STRLEN((char_u *)dp
				      + (dp->db_index[i] & DB_INDEX_MASK)) + 1;
		}
		இல்லையெனில்
# endif
		{
		    எனில் (idx == 0) // first line in block, text at the end
			text_end = dp->db_txt_end;
		    இல்லையெனில்
			text_end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);
		    size += text_end
				   - ((dp->db_index[end_idx]) & DB_INDEX_MASK);
		}
	    }
	    buf->b_ml.ml_chunksize[curix].mlcs_numlines = linecnt;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_numlines -= linecnt;
	    buf->b_ml.ml_chunksize[curix].mlcs_totalsize = size;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_totalsize -= size;
	    buf->b_ml.ml_usedchunks++;
	    ml_upd_lastbuf = வெற்று;   // Force recalc of curix & curline
	    திரும்பு;
	}
	இல்லையெனில் எனில் (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_வரி_மொத்தம் - வரி <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    எனில் (வரி == buf->b_ml.ml_வரி_மொத்தம்)
	    {
		curchnk->mlcs_numlines = 0;
		curchnk->mlcs_totalsize = 0;
	    }
	    இல்லையெனில்
	    {
		/*
		 * Line is just prior to last, move count for last
		 * This is the common case  when loading a new file
		 */
		hp = ml_find_வரி(buf, buf->b_ml.ml_வரி_மொத்தம், ML_FIND);
		எனில் (hp == வெற்று)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    திரும்பு;
		}
		dp = (DATA_BL *)(hp->bh_data);
		எனில் (dp->db_வரி_மொத்தம் == 1)
		    rest = dp->db_txt_end - dp->db_txt_start;
		இல்லையெனில்
		    rest =
			((dp->db_index[dp->db_வரி_மொத்தம் - 2]) & DB_INDEX_MASK)
			- dp->db_txt_start;
		curchnk->mlcs_totalsize = rest;
		curchnk->mlcs_numlines = 1;
		curchnk[-1].mlcs_totalsize -= rest;
		curchnk[-1].mlcs_numlines -= 1;
	    }
	}
    }
    இல்லையெனில் எனில் (updtype == ML_CHNK_DELLINE)
    {
	curchnk->mlcs_numlines--;
	எனில் (curix < buf->b_ml.ml_usedchunks - 1
		&& curchnk->mlcs_numlines + curchnk[1].mlcs_numlines
								  <= MLCS_MINL)
	{
	    curix++;
	    curchnk = buf->b_ml.ml_chunksize + curix;
	}
	இல்லையெனில் எனில் (curix == 0 && curchnk->mlcs_numlines <= 0)
	{
	    ml_upd_lastbuf = வெற்று;   // Force recalc of curix & curline
	    buf->b_ml.ml_usedchunks--;
	    mch_memmove(buf->b_ml.ml_chunksize, buf->b_ml.ml_chunksize + 1,
			buf->b_ml.ml_usedchunks * அளவு(chunksize_T));
	    திரும்பு;
	}
	இல்லையெனில் எனில் (curix == 0 || (curchnk->mlcs_numlines > 10
		    && curchnk->mlcs_numlines + curchnk[-1].mlcs_numlines
								  > MLCS_MINL))
	{
	    // The chunks are left as they are, the cached position stays
	    // valid.
	    ml_upd_lastbuf = buf;
	    ml_upd_lastcurline = curline;
	    ml_upd_lastcurix = curix;
	    திரும்பு;
	}

	// Collapse chunks
	ml_upd_lastbuf = வெற்று;   // Force recalc of curix & curline
	curchnk[-1].mlcs_numlines += curchnk->mlcs_numlines;
	curchnk[-1].mlcs_totalsize += curchnk->mlcs_totalsize;
	buf->b_ml.ml_usedchunks--;
	எனில் (curix < buf->b_ml.ml_usedchunks)
	    mch_memmove(buf->b_ml.ml_chunksize + curix,
			buf->b_ml.ml_chunksize + curix + 1,
			(buf->b_ml.ml_usedchunks - curix) *
			அளவு(chunksize_T));
	திரும்பு;
    }
    ml_upd_lastbuf = buf;
    ml_upd_lastcurline = curline;
    ml_upd_lastcurix = curix;
}

/*
 * Find offset for line or line with offset.
 * Find line with offset if "lnum" is 0; return remaining offset in offp
 * Find offset of line if "lnum" > 0
 * return -1 if information is not available
 */
    long
ml_find_வரி_or_offset(buf_T *buf, linenr_T lnum, long *offp)
{
    linenr_T	curline;
    முழு		curix;
    long	size;
    bhdr_T	*hp;
    DATA_BL	*dp;
    முழு		மொத்தம்;		// number of entries in block
    முழு		idx;
    முழு		start_idx;
    முழு		text_end;
    long	offset;
    முழு		len;
    முழு		ffdos = (get_fileformat(buf) == EOL_DOS);
    முழு		extra = 0;

    // take care of cached line first
    ml_flush_வரி(curbuf);

    எனில் (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == வெற்று
	    || lnum < 0)
	திரும்பு -1;

    எனில் (offp == வெற்று)
	offset = 0;
    இல்லையெனில்
	offset = *offp;
    எனில் (lnum == 0 && offset <= 0)
	திரும்பு 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = size = 0;
    போது (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (offset != 0
	       && offset > size + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (long)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	size += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	எனில் (offset && ffdos)
	    size += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    போது ((lnum != 0 && curline < lnum) || (offset != 0 && size < offset))
    {
# ifdef FEAT_PROP_POPUP
	size_t textprop_total = 0;
# endif

	எனில் (curline > buf->b_ml.ml_வரி_மொத்தம்
		|| (hp = ml_find_வரி(buf, curline, ML_FIND)) == வெற்று)
	    திரும்பு -1;
	dp = (DATA_BL *)(hp->bh_data);
	மொத்தம் = (long)(buf->b_ml.ml_locked_உயர்வு) -
		(long)(buf->b_ml.ml_locked_கீழ்) + 1;
	start_idx = idx = curline - buf->b_ml.ml_locked_கீழ்;
	எனில் (idx == 0)  // first line in block, text at the end
	    text_end = dp->db_txt_end;
	இல்லையெனில்
	    text_end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);
	// Compute index of last line to use in this MEMLINE
	எனில் (lnum != 0)
	{
	    எனில் (curline + (மொத்தம் - idx) >= lnum)
		idx += lnum - curline - 1;
	    இல்லையெனில்
		idx = மொத்தம் - 1;
	}
	இல்லையெனில்
	{
	    extra = 0;
	    க்கு (;;)
	    {
# ifdef FEAT_PROP_POPUP
		size_t textprop_size = 0;

		எனில் (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_index[idx]) & DB_INDEX_MASK);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_end
				  : ((dp->db_index[idx - 1]) & DB_INDEX_MASK));
		    textprop_size = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		எனில் (!(offset >= size
			+ text_end - (முழு)((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
			- (long)(textprop_total + textprop_size)
# endif
			+ ffdos))
		    நிறுத்து;

		எனில் (ffdos)
		    size++;
# ifdef FEAT_PROP_POPUP
		textprop_total += textprop_size;
# endif
		எனில் (idx == மொத்தம் - 1)
		{
		    extra = 1;
		    நிறுத்து;
		}
		idx++;
	    }
	}
# ifdef FEAT_PROP_POPUP
	எனில் (buf->b_has_textprop && lnum != 0)
	{
	    முழு i;

	    // cannot use the db_index pointer, need to get the actual text
	    // lengths.
	    len = 0;
	    க்கு (i = start_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_index[i]) & DB_INDEX_MASK);
		len += (முழு)STRLEN(p) + 1;
	    }
	}
	இல்லையெனில்
# endif
	    len = text_end - ((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
				- (long)textprop_total
# endif
				;
	size += len;
	எனில் (offset != 0 && size >= offset)
	{
	    எனில் (size + ffdos == offset)
		*offp = 0;
	    இல்லையெனில் எனில் (idx == start_idx)
		*offp = offset - size + len;
	    இல்லையெனில்
		*offp = offset - size + len
		     - (text_end - ((dp->db_index[idx - 1]) & DB_INDEX_MASK))
# ifdef FEAT_PROP_POPUP
		     + (long)textprop_total
# endif
		     ;
	    curline += idx - start_idx + extra;
	    எனில் (curline > buf->b_ml.ml_வரி_மொத்தம்)
		திரும்பு -1;	// exactly one byte beyond the end
	    திரும்பு curline;
	}
	curline = buf->b_ml.ml_locked_உயர்வு + 1;
    }

    எனில் (lnum != 0)
    {
	// Count extra CR characters.
	எனில் (ffdos)
	    size += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	எனில் ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_வரி_மொத்தம்)
	    size -= ffdos + 1;
    }

    திரும்பு size;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    வெறுமை
goto_byte(long cnt)
{
    long	boff = cnt;
    linenr_T	lnum;

    ml_flush_வரி(curbuf);	// cached line may be dirty
    setpcmark();
    எனில் (boff)
	--boff;
    lnum = ml_find_வரி_or_offset(curbuf, (linenr_T)0, &boff);
    எனில் (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_வரி_மொத்தம்;
	curwin->w_curswant = MAXCOL;
	coladvance((colnr_T)MAXCOL);
    }
    இல்லையெனில்
    {
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = (colnr_T)boff;
	curwin->w_cursor.coladd = 0;
	curwin->w_set_curswant = true;
    }
    check_cursor();

    // Make sure the cursor is on the first byte of a multi-byte char.
    எனில் (has_mbyte)
	mb_adjust_cursor();
}
#endif
