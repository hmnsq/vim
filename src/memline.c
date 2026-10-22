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
#define CHECK(c, s)	কুরু { /**/ } যাবত্ (0)

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

#অন্তর্ভাব "vim.h"

#অন্তর্ভাব <time.h>

#যদি defined(SASC) || defined(__amigaos4__)
# include <proto/dos.h>	    // for Open() and Close()
#endif

প্রকারনির্দেশ সংরচনা block0		ZERO_BL;    // contents of the first block
প্রকারনির্দেশ সংরচনা pointer_block	PTR_BL;	    // contents of a pointer block
প্রকারনির্দেশ সংরচনা data_block	DATA_BL;    // contents of a data block
প্রকারনির্দেশ সংরচনা pointer_entry	PTR_EN;	    // block/line-count pair

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

#যদি defined(FEAT_CRYPT)
স্থির ধ্রুব পূর্ণাঙ্ক id1_codes[] = {
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
সংরচনা pointer_entry
{
    blocknr_T	pe_bnum;	// block number
    linenr_T	pe_সারি_গুনতি;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    পূর্ণাঙ্ক		pe_page_গুনতি;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
সংরচনা pointer_block
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_গুনতি;	// number of pointers in this block
    short_u	pb_গুনতি_max;	// maximum value for pb_count
    PTR_EN	pb_pointer[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#define PB_গুনতি_MAX(mfp) (short_u)(((mfp)->mf_page_size - offsetof(PTR_BL, pb_pointer)) / পরিমাণ(PTR_EN))

/*
 * A data block is a leaf in the tree.
 *
 * The text of the lines is at the end of the block. The text of the first line
 * in the block is put at the end, the text of the second line in front of it,
 * etc. Thus the order of the lines is the opposite of the line number.
 */
সংরচনা data_block
{
    short_u	db_id;		// ID for data block: DATA_ID
    unsigned	db_free;	// free space available
    unsigned	db_txt_start;	// byte where text starts
    unsigned	db_txt_end;	// byte just after data block
    linenr_T	db_সারি_গুনতি;	// number of lines in this block
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
#define DB_MARKED	((unsigned)1 << ((পরিমাণ(unsigned) * 8) - 1))
#define DB_INDEX_MASK	(~DB_MARKED)

#define INDEX_SIZE  (পরিমাণ(unsigned))	    // size of one db_index entry
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
সংরচনা block0
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
    পূর্ণাঙ্ক		b0_magic_int;	// check for byte order of int
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

#define স্তূপ_INCR	5	// nr of entries added to ml_stack at a time

/*
 * The line number where the first mark may be is remembered.
 * If it is 0 there are no marks at all.
 * (always used for the current buffer only, no buffer change possible while
 * executing a global command).
 */
স্থির linenr_T	lowest_marked = 0;

/*
 * arguments for ml_find_line()
 */
#define ML_DELETE	0x11	    // delete line
#define ML_ঢোকাও	0x12	    // insert line
#define ML_FIND		0x13	    // just find the line
#define ML_FLUSH	0x02	    // flush locked block
#define ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
প্রকারনির্দেশ গণনা {
      UB_FNAME = 0	// update timestamp and filename
    , UB_SAME_DIR       // update the B0_SAME_DIR flag
    , UB_CRYPT		// update crypt key
} upd_block0_T;

#ifdef FEAT_CRYPT
স্থির শূন্য ml_set_b0_crypt(buf_T *buf, ZERO_BL *b0p);
#endif
স্থির শূন্য ml_upd_block0(buf_T *buf, upd_block0_T what);
স্থির শূন্য set_b0_fname(ZERO_BL *, buf_T *buf);
স্থির শূন্য set_b0_dir_flag(ZERO_BL *b0p, buf_T *buf);
স্থির শূন্য add_b0_fenc(ZERO_BL *b0p, buf_T *buf);
স্থির time_t swapfile_info(char_u *);
স্থির পূর্ণাঙ্ক recov_file_names(char_u **, char_u *, পূর্ণাঙ্ক prepend_dot);
স্থির char_u *findswapname(buf_T *, char_u **, char_u *);
স্থির শূন্য ml_flush_সারি(buf_T *);
স্থির bhdr_T *ml_new_data(memfile_T *, পূর্ণাঙ্ক, পূর্ণাঙ্ক);
স্থির bhdr_T *ml_new_ptr(memfile_T *);
স্থির bhdr_T *ml_find_সারি(buf_T *, linenr_T, পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক ml_add_স্তূপ(buf_T *);
স্থির শূন্য ml_lineadd(buf_T *, পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক b0_magic_wrong(ZERO_BL *);
#ifdef CHECK_INODE
স্থির পূর্ণাঙ্ক fnamecmp_ino(char_u *, char_u *, long);
#endif
স্থির শূন্য long_to_char(long, char_u *);
স্থির long char_to_long(char_u *);
#ifdef FEAT_CRYPT
স্থির cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T offset, পূর্ণাঙ্ক reading);
#endif
#ifdef FEAT_BYTEOFF
স্থির শূন্য ml_updatechunk(buf_T *buf, long সারি, long len, পূর্ণাঙ্ক updtype);
#endif

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
    পূর্ণাঙ্ক
ml_open(buf_T *buf)
{
    memfile_T	*mfp;
    bhdr_T	*hp = রিক্ত;
    ZERO_BL	*b0p;
    PTR_BL	*pp;
    DATA_BL	*dp;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_স্তূপ_size = 0; // no stack yet
    buf->b_ml.ml_স্তূপ = রিক্ত;	// no stack yet
    buf->b_ml.ml_স্তূপ_শীর্ষ = 0;	// nothing in the stack
    buf->b_ml.ml_locked = রিক্ত;	// no cached block
    buf->b_ml.ml_সারি_lnum = 0;	// no cached line
#ifdef FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = রিক্ত;
    buf->b_ml.ml_usedchunks = 0;
#endif

    যদি (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    যদি (p_uc && buf->b_p_swf)
	buf->b_may_swap = true;
    অন্যথা
	buf->b_may_swap = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(রিক্ত, 0);
    যদি (mfp == রিক্ত)
	গচ্ছ error;

    buf->b_ml.ml_mfp = mfp;
#ifdef FEAT_CRYPT
    mfp->mf_buffer = buf;
#endif
    buf->b_ml.ml_flags = ML_EMPTY;
    buf->b_ml.ml_সারি_গুনতি = 1;

/*
 * fill block0 struct and write page 0
 */
    যদি ((hp = mf_new(mfp, FALSE, 1)) == রিক্ত)
	গচ্ছ error;
    যদি (hp->bh_bnum != 0)
    {
	iemsg(e_didnt_get_block_nr_zero);
	গচ্ছ error;
    }
    b0p = (ZERO_BL *)(hp->bh_data);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (long)B0_MAGIC_LONG;
    b0p->b0_magic_int = (পূর্ণাঙ্ক)B0_MAGIC_INT;
    b0p->b0_magic_short = (short)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_version, "VIM ", 4);
    STRNCPY(b0p->b0_version + 4, Version, 6);
    long_to_char((long)mfp->mf_page_size, b0p->b0_page_size);

#ifdef FEAT_SPELL
    যদি (!buf->b_spell)
#endif
    {
	b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	b0p->b0_flags = get_fileformat(buf) + 1;
	set_b0_fname(b0p, buf);
	(শূন্য)get_user_name(b0p->b0_uname, B0_UNAME_SIZE);
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
    যদি (!buf->b_help && !B_SPELL(buf))
	(শূন্য)mf_sync(mfp, 0);

    /*
     * Fill in root pointer block and write page 1.
     */
    যদি ((hp = ml_new_ptr(mfp)) == রিক্ত)
	গচ্ছ error;
    যদি (hp->bh_bnum != 1)
    {
	iemsg(e_didnt_get_block_nr_one);
	গচ্ছ error;
    }
    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_গুনতি = 1;
    pp->pb_pointer[0].pe_bnum = 2;
    pp->pb_pointer[0].pe_page_গুনতি = 1;
    pp->pb_pointer[0].pe_old_lnum = 1;
    pp->pb_pointer[0].pe_সারি_গুনতি = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    যদি ((hp = ml_new_data(mfp, FALSE, 1)) == রিক্ত)
	গচ্ছ error;
    যদি (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_block_nr_two);
	গচ্ছ error;
    }

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[0] = --dp->db_txt_start;	// at end of block
    dp->db_free -= 1 + INDEX_SIZE;
    dp->db_সারি_গুনতি = 1;
    *((char_u *)dp + dp->db_txt_start) = NUL;	// empty line

    প্রত্যাগম OK;

error:
    যদি (mfp != রিক্ত)
    {
	যদি (hp)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, TRUE);	    // will also free(mfp->mf_fname)
    }
    buf->b_ml.ml_mfp = রিক্ত;
    প্রত্যাগম FAIL;
}

#যদি defined(FEAT_CRYPT)
/*
 * Swapfile encryption is not supported by XChaCha20.  If this crypt method is
 * used then disable the swapfile, to avoid plain text being written to disk,
 * and return TRUE.
 * Otherwise return FALSE.
 */
    স্থির পূর্ণাঙ্ক
crypt_may_close_swapfile(buf_T *buf, char_u *চাবি, পূর্ণাঙ্ক method)
{
    যদি (crypt_method_is_sodium(method) && *চাবি != NUL)
    {
	mf_close_file(buf, TRUE);
	buf->b_p_swf = FALSE;
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Prepare encryption for "buf" for the current key and method.
 */
    স্থির শূন্য
ml_set_mfp_crypt(buf_T *buf)
{
    যদি (*buf->b_p_চাবি == NUL)
	প্রত্যাগম;

    পূর্ণাঙ্ক method_nr = crypt_get_method_nr(buf);

    যদি (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_seed(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN, রিক্ত, 0);
    }
# ifdef FEAT_SODIUM
    অন্যথা যদি (crypt_method_is_sodium(method_nr))
	crypt_sodium_randombytes_buf(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN);
# endif
}

/*
 * Prepare encryption for "buf" with block 0 "b0p".
 * Note: should not be called with libsodium encryption, since xchacha20 does
 * not support swapfile encryption.
 */
    স্থির শূন্য
ml_set_b0_crypt(buf_T *buf, ZERO_BL *b0p)
{
    যদি (*buf->b_p_চাবি == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    অন্যথা
    {
	পূর্ণাঙ্ক method_nr = crypt_get_method_nr(buf);

	b0p->b0_id[1] = id1_codes[method_nr];
	যদি (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
	{
	    // Generate a seed and store it in block 0 and in the memfile.
	    sha2_seed(&b0p->b0_seed, MF_SEED_LEN, রিক্ত, 0);
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
    শূন্য
ml_set_crypt_চাবি(
    buf_T	*buf,
    char_u	*old_চাবি,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    পূর্ণাঙ্ক		page_গুনতি;
    পূর্ণাঙ্ক		idx;
    long	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    DATA_BL	*dp;
    blocknr_T	bnum;
    পূর্ণাঙ্ক		শীর্ষ;
    পূর্ণাঙ্ক		old_method;

    যদি (mfp == রিক্ত || mfp->mf_fd < 0)
	প্রত্যাগম;  // no memfile yet, nothing to do
    old_method = crypt_method_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    যদি (crypt_may_close_swapfile(buf, buf->b_p_চাবি, crypt_get_method_nr(buf)))
	প্রত্যাগম;
# endif

    // First make sure the swapfile is in a consistent state, using the old
    // key and method.
    {
	char_u *new_চাবি = buf->b_p_চাবি;
	char_u *new_buf_cm = buf->b_p_cm;

	buf->b_p_চাবি = old_চাবি;
	buf->b_p_cm = old_cm;
	ml_preserve(buf, FALSE);
	buf->b_p_চাবি = new_চাবি;
	buf->b_p_cm = new_buf_cm;
    }

    // Set the key, method and seed to be used for reading, these must be the
    // old values.
    mfp->mf_old_চাবি = old_চাবি;
    mfp->mf_old_cm = old_method;
    যদি (old_method > 0 && *old_চাবি != NUL)
	mch_memmove(mfp->mf_old_seed, mfp->mf_seed, MF_SEED_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    যদি (mfp->mf_infile_গুনতি > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_সারি(buf);		    // flush buffered line
	(শূন্য)ml_find_সারি(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = রিক্ত;
	bnum = 1;		// start with block 1
	page_গুনতি = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_স্তূপ_শীর্ষ = 0;
	VIM_CLEAR(buf->b_ml.ml_স্তূপ);
	buf->b_ml.ml_স্তূপ_size = 0;	// no stack yet

	প্রতি ( ; !got_int; সারি_breakcheck())
	{
	    যদি (hp != রিক্ত)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    যদি ((hp = mf_get(mfp, bnum, page_গুনতি)) == রিক্ত)
	    {
		যদি (bnum == 1)
		    ভঙ্গ;
		++error;
	    }
	    অন্যথা
	    {
		pp = (PTR_BL *)(hp->bh_data);
		যদি (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    যদি (pp->pb_গুনতি == 0)
		    {
			// empty block?
			++error;
		    }
		    অন্যথা যদি (idx < (পূর্ণাঙ্ক)pp->pb_গুনতি)	// go a block deeper
		    {
			যদি (pp->pb_pointer[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    অনুবর্ত;
			}

			// going one block deeper in the tree, new entry in
			//স্তূপ
			যদি ((শীর্ষ = ml_add_স্তূপ(buf)) < 0)
			{
			    ++error;
			    ভঙ্গ;		    // out of memory
			}
			ip = &(buf->b_ml.ml_স্তূপ[শীর্ষ]);
			ip->ip_bnum = bnum;
			ip->ip_index = idx;

			bnum = pp->pb_pointer[idx].pe_bnum;
			page_গুনতি = pp->pb_pointer[idx].pe_page_গুনতি;
			idx = 0;
			অনুবর্ত;
		    }
		}
		অন্যথা	    // not a pointer block
		{
		    dp = (DATA_BL *)(hp->bh_data);
		    যদি (dp->db_id != DATA_ID)	// block id wrong
			++error;
		    অন্যথা
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = রিক্ত;
		    }
		}
	    }

	    যদি (buf->b_ml.ml_স্তূপ_শীর্ষ == 0)	// finished
		ভঙ্গ;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_স্তূপ[--(buf->b_ml.ml_স্তূপ_শীর্ষ)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_index + 1;	    // go to next index
	    page_গুনতি = 1;
	}
	যদি (hp != রিক্ত)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	যদি (error > 0)
	    emsg(_(e_error_while_updating_swap_file_crypt));
    }

    mfp->mf_old_চাবি = রিক্ত;
}
#endif

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
    শূন্য
ml_setname(buf_T *buf)
{
    পূর্ণাঙ্ক		success = FALSE;
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;
#যদি defined(MSWIN)
    char_u	*p;
#endif

    mfp = buf->b_ml.ml_mfp;
    যদি (mfp->mf_fd < 0)		    // there is no swap file yet
    {
	/*
	 * When 'updatecount' is 0 and 'noswapfile' there is no swap file.
	 * For help files we will make a swap file now.
	 */
	যদি (p_uc != 0 && (cmdmod.cmod_flags & CMOD_NOSWAPFILE) == 0)
	    ml_open_file(buf);	    // create a swap file
	প্রত্যাগম;
    }

    /*
     * Try all directories in the 'directory' option.
     */
    dirp = p_dir;
    প্রতি (;;)
    {
	যদি (*dirp == NUL)	    // tried all directories, fail
	    ভঙ্গ;
	fname = findswapname(buf, &dirp, mfp->mf_fname);
	// autocmd may have freed mfp if findswapname creates
	// different swapfile name
	যদি (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    প্রত্যাগম;
	}
						    // alloc's fname
	যদি (dirp == রিক্ত)	    // out of memory
	    ভঙ্গ;
	যদি (fname == রিক্ত)	    // no file name found for this dir
	    অনুবর্ত;

#যদি defined(MSWIN)
	/*
	 * Set full pathname for swap file now, because a ":!cd dir" may
	 * change directory without us knowing it.
	 */
	p = FullName_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	যদি (fname == রিক্ত)
	    অনুবর্ত;
#endif
	// if the file name is the same we don't have to do anything
	যদি (fnamecmp(fname, mfp->mf_fname) == 0)
	{
	    vim_free(fname);
	    success = TRUE;
	    ভঙ্গ;
	}
	// need to close the swap file before renaming
	যদি (mfp->mf_fd >= 0)
	{
	    close(mfp->mf_fd);
	    mfp->mf_fd = -1;
	}

	// try to rename the swap file
	যদি (vim_rename(mfp->mf_fname, fname) == 0)
	{
	    success = TRUE;
	    vim_free(mfp->mf_fname);
	    mfp->mf_fname = fname;
	    vim_free(mfp->mf_ffname);
#যদি defined(MSWIN)
	    mfp->mf_ffname = রিক্ত;  // mf_fname is full pathname already
#অন্যথা
	    mf_set_ffname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);
	    ভঙ্গ;
	}
	vim_free(fname);	    // this fname didn't work, try another
    }

    যদি (mfp->mf_fd == -1)	    // need to (re)open the swap file
    {
	mfp->mf_fd = mch_open((বর্ণ *)mfp->mf_fname, O_RDWR | O_EXTRA, 0);
	যদি (mfp->mf_fd < 0)
	{
	    // could not (re)open the swap file, what can we do????
	    emsg(_(e_oops_lost_the_swap_file));
	    প্রত্যাগম;
	}
#ifdef HAVE_FD_CLOEXEC
	{
	    পূর্ণাঙ্ক fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    যদি (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(শূন্য)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#endif
    }
    যদি (!success)
	emsg(_(e_could_not_rename_swap_file));
}

/*
 * Open a file for the memfile for all buffers that are not readonly or have
 * been modified.
 * Used when 'updatecount' changes from zero to non-zero.
 */
    শূন্য
ml_open_files(শূন্য)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	যদি (!buf->b_p_ro || buf->b_changed)
	    ml_open_file(buf);
}

/*
 * Open a swap file for an existing memfile, if there is no swap file yet.
 * If we are unable to find a file name, mf_fname will be NULL
 * and the memfile will be in memory only (no recovery possible).
 */
    শূন্য
ml_open_file(buf_T *buf)
{
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;

    mfp = buf->b_ml.ml_mfp;
    যদি (mfp == রিক্ত || mfp->mf_fd >= 0 || !buf->b_p_swf
				      || (cmdmod.cmod_flags & CMOD_NOSWAPFILE))
	প্রত্যাগম;		// nothing to do

#ifdef FEAT_SPELL
    // For a spell buffer use a temp file name.
    যদি (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	যদি (fname != রিক্ত)
	    (শূন্য)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_swap = false;
	প্রত্যাগম;
    }
#endif

    /*
     * Try all directories in 'directory' option.
     */
    dirp = p_dir;
    প্রতি (;;)
    {
	যদি (*dirp == NUL)
	    ভঙ্গ;
	// There is a small chance that between choosing the swap file name
	// and creating it, another Vim creates the file.  In that case the
	// creation will fail and we will use another directory.
	fname = findswapname(buf, &dirp, রিক্ত); // allocates fname
	// autocmd may have freed mfp, grr!
	যদি (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    প্রত্যাগম;
	}
	যদি (dirp == রিক্ত)
	    ভঙ্গ;  // out of memory
	যদি (fname == রিক্ত)
	    অনুবর্ত;
	যদি (mf_open_file(mfp, fname) == OK)	// consumes fname!
	{
	    // don't sync yet in ml_sync_all()
	    mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;
#যদি defined(MSWIN)
	    /*
	     * set full pathname for swap file now, because a ":!cd dir" may
	     * change directory without us knowing it.
	     */
	    mf_fullname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    যদি (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_set_dirty(mfp);
		ভঙ্গ;
	    }
	    // Writing block 0 failed: close the file and try another dir
	    mf_close_file(buf, FALSE);
	}
    }

    যদি (*p_dir != NUL && mfp->mf_fname == রিক্ত)
    {
	need_wait_return = TRUE;	// call wait_return() later
	++no_wait_return;
	(শূন্য)semsg(_(e_unable_to_open_swap_file_for_str_recovery_impossible),
		    buf_spname(buf) != রিক্ত ? buf_spname(buf) : buf->b_fname);
	--no_wait_return;
    }

    // don't try to open a swap file again
    buf->b_may_swap = false;
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
    শূন্য
check_need_swap(
    পূর্ণাঙ্ক	    newfile)		// reading file into new buffer
{
    পূর্ণাঙ্ক old_বার্তা_silent = বার্তা_silent; // might be reset by an E325 message

    যদি (curbuf->b_may_swap && (!curbuf->b_p_ro || !newfile))
	ml_open_file(curbuf);
    বার্তা_silent = old_বার্তা_silent;
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
    শূন্য
ml_close(buf_T *buf, পূর্ণাঙ্ক del_file)
{
    যদি (buf->b_ml.ml_mfp == রিক্ত)		// not open
	প্রত্যাগম;
    mf_close(buf->b_ml.ml_mfp, del_file);	// close the .swp file
    যদি (buf->b_ml.ml_সারি_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_সারি_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_সারি_ptr);
    vim_free(buf->b_ml.ml_স্তূপ);
#ifdef FEAT_BYTEOFF
    VIM_CLEAR(buf->b_ml.ml_chunksize);
#endif
    buf->b_ml.ml_mfp = রিক্ত;

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
    শূন্য
ml_close_all(পূর্ণাঙ্ক del_file)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ml_close(buf, del_file && ((buf->b_flags & BF_PRESERVED) == 0
				 || vim_strchr(p_cpo, CPO_PRESERVE) == রিক্ত));
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
    শূন্য
ml_close_notmod(শূন্য)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	যদি (!bufIsChanged(buf))
	    ml_close(buf, TRUE);    // close all not-modified buffers
}

/*
 * Update the timestamp in the .swp file.
 * Used when the file has been written.
 */
    শূন্য
ml_timestamp(buf_T *buf)
{
    ml_upd_block0(buf, UB_FNAME);
}

/*
 * Return FAIL when the ID of "b0p" is wrong.
 */
    স্থির পূর্ণাঙ্ক
ml_check_b0_id(ZERO_BL *b0p)
{
    যদি (b0p->b0_id[0] != BLOCK0_ID0
	    || (b0p->b0_id[1] != BLOCK0_ID1
		&& b0p->b0_id[1] != BLOCK0_ID1_C0
		&& b0p->b0_id[1] != BLOCK0_ID1_C1
		&& b0p->b0_id[1] != BLOCK0_ID1_C2
		&& b0p->b0_id[1] != BLOCK0_ID1_C3
		&& b0p->b0_id[1] != BLOCK0_ID1_C4)
	    )
	প্রত্যাগম FAIL;
    প্রত্যাগম OK;
}

/*
 * Update the timestamp or the B0_SAME_DIR flag of the .swp file.
 */
    স্থির শূন্য
ml_upd_block0(buf_T *buf, upd_block0_T what)
{
    memfile_T	*mfp;
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    mfp = buf->b_ml.ml_mfp;
    যদি (mfp == রিক্ত)
	প্রত্যাগম;
    hp = mf_get(mfp, (blocknr_T)0, 1);
    যদি (hp == রিক্ত)
    {
#ifdef FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	যদি (what == UB_CRYPT)
	    ml_set_mfp_crypt(buf);
#endif
	প্রত্যাগম;
    }

    b0p = (ZERO_BL *)(hp->bh_data);
    যদি (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_block_zero);
    অন্যথা
    {
	যদি (what == UB_FNAME)
	    set_b0_fname(b0p, buf);
#ifdef FEAT_CRYPT
	অন্যথা যদি (what == UB_CRYPT)
	    ml_set_b0_crypt(buf, b0p);
#endif
	অন্যথা // what == UB_SAME_DIR
	    set_b0_dir_flag(b0p, buf);
    }
    mf_put(mfp, hp, TRUE, FALSE);
}

/*
 * Write file name and timestamp into block 0 of a swap file.
 * Also set buf->b_mtime.
 * Don't use NameBuff[]!!!
 */
    স্থির শূন্য
set_b0_fname(ZERO_BL *b0p, buf_T *buf)
{
    stat_T	st;

    যদি (buf->b_ffname == রিক্ত)
	b0p->b0_fname[0] = NUL;
    অন্যথা
    {
#যদি defined(MSWIN) || defined(AMIGA)
	// Systems that cannot translate "~user" back into a path: copy the
	// file name unmodified.  Do use slashes instead of backslashes for
	// portability.
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_SIZE_CRYPT - 1);
# ifdef BACKSLASH_IN_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#অন্যথা
	size_t	flen;
	char_u	uname[B0_UNAME_SIZE];

	/*
	 * For a file under the home directory of the current user, we try to
	 * replace the home directory path with "~user". This helps when
	 * editing the same file on different machines over a network.
	 * First replace home dir path with "~/" with home_replace().
	 * Then insert the user name to get "~user/".
	 */
	flen = home_replace(রিক্ত, buf->b_ffname, b0p->b0_fname,
						   B0_FNAME_SIZE_CRYPT, TRUE);
	যদি (b0p->b0_fname[0] == '~')
	{
	    size_t  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    যদি (get_user_name(uname, B0_UNAME_SIZE) == FAIL
		   || (ulen = STRLEN(uname)) + flen > B0_FNAME_SIZE_CRYPT - 1)
		vim_strncpy(b0p->b0_fname, buf->b_ffname,
						     B0_FNAME_SIZE_CRYPT - 1);
	    অন্যথা
	    {
		mch_memmove(b0p->b0_fname + ulen + 1, b0p->b0_fname + 1, flen);
		mch_memmove(b0p->b0_fname + 1, uname, ulen);
	    }
	}
#endif
	যদি (mch_stat((বর্ণ *)buf->b_ffname, &st) >= 0)
	{
	    long_to_char((long)st.st_mtime, b0p->b0_mtime);
#ifdef CHECK_INODE
	    long_to_char((long)st.st_ino, b0p->b0_ino);
#endif
	    buf_store_time(buf, &st, buf->b_ffname);
	    buf->b_mtime_read = buf->b_mtime;
	    buf->b_mtime_read_ns = buf->b_mtime_ns;
	}
	অন্যথা
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
    স্থির শূন্য
set_b0_dir_flag(ZERO_BL *b0p, buf_T *buf)
{
    যদি (same_directory(buf->b_ml.ml_mfp->mf_fname, buf->b_ffname))
	b0p->b0_flags |= B0_SAME_DIR;
    অন্যথা
	b0p->b0_flags &= ~B0_SAME_DIR;
}

/*
 * When there is room, add the 'fileencoding' to block zero.
 */
    স্থির শূন্য
add_b0_fenc(
    ZERO_BL	*b0p,
    buf_T	*buf)
{
    পূর্ণাঙ্ক		n;
    পূর্ণাঙ্ক		size = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    যদি (*buf->b_p_চাবি != NUL)
	size = B0_FNAME_SIZE_CRYPT;
#endif

    n = (পূর্ণাঙ্ক)STRLEN(buf->b_p_fenc);
    যদি ((পূর্ণাঙ্ক)STRLEN(b0p->b0_fname) + n + 1 > size)
	b0p->b0_flags &= ~B0_HAS_FENC;
    অন্যথা
    {
	mch_memmove((বর্ণ *)b0p->b0_fname + size - n,
					    (বর্ণ *)buf->b_p_fenc, (size_t)n);
	*(b0p->b0_fname + size - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#যদি defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#endif

#যদি defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    স্থির পূর্ণাঙ্ক
swapfile_process_running(ZERO_BL *b0p, char_u *swap_fname UNUSED)
{
# যদি defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    সংরচনা sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    যদি (mch_stat((বর্ণ *)swap_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < time(রিক্ত) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	প্রত্যাগম FALSE;
# endif
    প্রত্যাগম mch_process_running(char_to_long(b0p->b0_pid));
}
#endif

/*
 * Try to recover curbuf from the .swp file.
 * If "checkext" is TRUE, check the extension and detect whether it is
 * a swap file.
 */
    শূন্য
ml_recover(পূর্ণাঙ্ক checkext)
{
    buf_T	*buf = রিক্ত;
    memfile_T	*mfp = রিক্ত;
    char_u	*fname;
    char_u	*fname_used = রিক্ত;
    bhdr_T	*hp = রিক্ত;
    ZERO_BL	*b0p;
    পূর্ণাঙ্ক		b0_ff;
    char_u	*b0_fenc = রিক্ত;
#ifdef FEAT_CRYPT
    পূর্ণাঙ্ক		b0_cm = -1;
#endif
    PTR_BL	*pp;
    DATA_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    পূর্ণাঙ্ক		page_গুনতি;
    stat_T	org_stat, swp_stat;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		directly;
    linenr_T	lnum;
    char_u	*p;
    পূর্ণাঙ্ক		i;
    long	error;
    পূর্ণাঙ্ক		cannot_open;
    linenr_T	সারি_গুনতি;
    পূর্ণাঙ্ক		has_error;
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		শীর্ষ;
    পূর্ণাঙ্ক		txt_start;
    off_T	size;
    পূর্ণাঙ্ক		called_from_main;
    পূর্ণাঙ্ক		serious_error = TRUE;
    long	mtime;
    পূর্ণাঙ্ক		attr;
    পূর্ণাঙ্ক		orig_file_status = NOTDONE;

    recoverymode = TRUE;
    called_from_main = (curbuf->b_ml.ml_mfp == রিক্ত);
    attr = HL_ATTR(HLF_E);

    /*
     * If the file name ends in ".s[a-w][a-z]" we assume this is the swap file.
     * Otherwise a search is done to find the swap file(s).
     */
    fname = curbuf->b_fname;
    যদি (fname == রিক্ত)		    // When there is no file name
	fname = (char_u *)"";
    len = (পূর্ণাঙ্ক)STRLEN(fname);
    যদি (checkext && len >= 4 &&
#যদি defined(VMS)
	    STRNICMP(fname + len - 4, "_s", 2)
#অন্যথা
	    STRNICMP(fname + len - 4, ".s", 2)
#endif
						== 0
		&& vim_strchr((char_u *)"abcdefghijklmnopqrstuvw",
					   TOLOWER_ASC(fname[len - 2])) != রিক্ত
		&& ASCII_ISALPHA(fname[len - 1]))
    {
	directly = TRUE;
	fname_used = vim_strsave(fname); // make a copy for mf_open()
    }
    অন্যথা
    {
	directly = FALSE;

	// count the number of matching swap files
	len = recover_names(fname, FALSE, রিক্ত, 0, রিক্ত);
	যদি (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_swap_file_found_for_str), fname);
	    গচ্ছ theend;
	}
	যদি (len == 1)		    // one swap file found, use it
	    i = 1;
	অন্যথা			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (শূন্য)recover_names(fname, TRUE, রিক্ত, 0, রিক্ত);
	    বার্তা_putchar('\n');
	    বার্তা_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_সংখ্যা(FALSE, রিক্ত);
	    যদি (i < 1 || i > len)
		গচ্ছ theend;
	}
	// get the swap file name that will be used
	(শূন্য)recover_names(fname, FALSE, রিক্ত, i, &fname_used);
    }
    যদি (fname_used == রিক্ত)
	গচ্ছ theend;			// out of memory

    // When called from main() still need to initialize storage structure
    যদি (called_from_main && ml_open(curbuf) == FAIL)
	getout(1);

    /*
     * Allocate a buffer structure for the swap file that is used for recovery.
     * Only the memline and crypt information in it are really used.
     */
    buf = ALLOC_ONE(buf_T);
    যদি (buf == রিক্ত)
	গচ্ছ theend;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_স্তূপ_size = 0;	// no stack yet
    buf->b_ml.ml_স্তূপ = রিক্ত;		// no stack yet
    buf->b_ml.ml_স্তূপ_শীর্ষ = 0;		// nothing in the stack
    buf->b_ml.ml_সারি_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = রিক্ত;		// no locked block
    buf->b_ml.ml_flags = 0;
#ifdef FEAT_CRYPT
    buf->b_p_চাবি = empty_option;
    buf->b_p_cm = empty_option;
#endif

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_used); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_used, O_RDONLY);
    fname_used = p;
    যদি (mfp == রিক্ত || mfp->mf_fd < 0)
    {
	যদি (fname_used != রিক্ত)
	    semsg(_(e_cannot_open_str), fname_used);
	গচ্ছ theend;
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
    যদি ((hp = mf_get(mfp, (blocknr_T)0, 1)) == রিক্ত)
    {
	বার্তা_start();
	বার্তা_puts_attr(_("Unable to read block 0 from "), attr | বার্তা_HIST);
	বার্তা_outtrans_attr(mfp->mf_fname, attr | বার্তা_HIST);
	বার্তা_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | বার্তা_HIST);
	বার্তা_end();
	গচ্ছ theend;
    }
    b0p = (ZERO_BL *)(hp->bh_data);
    যদি (STRNCMP(b0p->b0_version, "VIM 3.0", 7) == 0)
    {
	বার্তা_start();
	বার্তা_outtrans_attr(mfp->mf_fname, বার্তা_HIST);
	বার্তা_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    বার্তা_HIST);
	বার্তা_puts_attr(_("Use Vim version 3.0.\n"), বার্তা_HIST);
	বার্তা_end();
	গচ্ছ theend;
    }
    যদি (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_swap_file), mfp->mf_fname);
	গচ্ছ theend;
    }
    যদি (b0_magic_wrong(b0p))
    {
	বার্তা_start();
	বার্তা_outtrans_attr(mfp->mf_fname, attr | বার্তা_HIST);
#যদি defined(MSWIN)
	যদি (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    বার্তা_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | বার্তা_HIST);
	অন্যথা
#endif
	    বার্তা_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | বার্তা_HIST);
	বার্তা_puts_attr(_("The file was created on "), attr | বার্তা_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	বার্তা_puts_attr((বর্ণ *)b0p->b0_hname, attr | বার্তা_HIST);
	বার্তা_puts_attr(_(",\nor the file has been damaged."), attr | বার্তা_HIST);
	বার্তা_end();
	গচ্ছ theend;
    }

#ifdef FEAT_CRYPT
    প্রতি (i = 0; i < (পূর্ণাঙ্ক)ARRAY_LENGTH(id1_codes); ++i)
	যদি (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    যদি (b0_cm > 0)
	mch_memmove(mfp->mf_seed, &b0p->b0_seed, MF_SEED_LEN);
    crypt_set_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#অন্যথা
    যদি (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_version_of_vim_does_not_support_encryption), mfp->mf_fname);
	গচ্ছ theend;
    }
#endif

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    যদি (mfp->mf_page_size != (unsigned)char_to_long(b0p->b0_page_size))
    {
	unsigned previous_page_size = mfp->mf_page_size;

	mf_new_page_size(mfp, (unsigned)char_to_long(b0p->b0_page_size));
	যদি (mfp->mf_page_size < previous_page_size)
	{
	    বার্তা_start();
	    বার্তা_outtrans_attr(mfp->mf_fname, attr | বার্তা_HIST);
	    বার্তা_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | বার্তা_HIST);
	    বার্তা_end();
	    গচ্ছ theend;
	}
	যদি ((size = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	অন্যথা
	    mfp->mf_blocknr_max = (blocknr_T)(size / mfp->mf_page_size);
	mfp->mf_infile_গুনতি = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_page_size);
	যদি (p == রিক্ত)
	    গচ্ছ theend;
	mch_memmove(p, hp->bh_data, previous_page_size);
	vim_free(hp->bh_data);
	hp->bh_data = p;
	b0p = (ZERO_BL *)(hp->bh_data);
    }

    /*
     * If .swp file name given directly, use name from swap file for buffer.
     */
    যদি (directly)
    {
	expand_env(b0p->b0_fname, NameBuff, MAXPATHL);
	যদি (setfname(curbuf, NameBuff, রিক্ত, TRUE) == FAIL)
	    গচ্ছ theend;
    }

    home_replace(রিক্ত, mfp->mf_fname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Using swap file \"%s\""), NameBuff);

    যদি (buf_spname(curbuf) != রিক্ত)
	vim_strncpy(NameBuff, buf_spname(curbuf), MAXPATHL - 1);
    অন্যথা
	home_replace(রিক্ত, curbuf->b_ffname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Original file \"%s\""), NameBuff);
    বার্তা_putchar('\n');

    /*
     * check date of swap file and original file
     */
    mtime = char_to_long(b0p->b0_mtime);
    যদি (curbuf->b_ffname != রিক্ত
	    && mch_stat((বর্ণ *)curbuf->b_ffname, &org_stat) != -1
	    && ((mch_stat((বর্ণ *)mfp->mf_fname, &swp_stat) != -1
		    && org_stat.st_mtime > swp_stat.st_mtime)
		|| org_stat.st_mtime != mtime))
	emsg(_(e_warning_original_file_may_have_been_changed));
    out_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_MASK);
    যদি (b0p->b0_flags & B0_HAS_FENC)
    {
	পূর্ণাঙ্ক fnsize = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	যদি (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_SIZE_CRYPT;
#endif
	প্রতি (p = b0p->b0_fname + fnsize; p > b0p->b0_fname && p[-1] != NUL; --p)
	    ;
	b0_fenc = vim_strnsave(p, b0p->b0_fname + fnsize - p);
    }

    mf_put(mfp, hp, FALSE, FALSE);	// release block 0
    hp = রিক্ত;

    /*
     * Now that we are sure that the file is going to be recovered, clear the
     * contents of the current buffer.
     */
    যাবত্ (!(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    যদি (curbuf->b_ffname != রিক্ত)
	orig_file_status = readfile(curbuf->b_ffname, রিক্ত, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, রিক্ত, READ_NEW);

#ifdef FEAT_CRYPT
    যদি (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	যদি (*curbuf->b_p_চাবি != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_used);
	    বার্তা_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    বার্তা_puts(_("\nenter the new crypt key."));
	    বার্তা_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    বার্তা_puts(_("\nto use the same key for text file and swap file"));
	}
	অন্যথা
	    smsg(_(need_চাবি_বার্তা), fname_used);
	buf->b_p_চাবি = crypt_get_চাবি(FALSE, FALSE);
	যদি (buf->b_p_চাবি == রিক্ত)
	    buf->b_p_চাবি = curbuf->b_p_চাবি;
	অন্যথা যদি (*buf->b_p_চাবি == NUL)
	{
	    vim_free(buf->b_p_চাবি);
	    buf->b_p_চাবি = curbuf->b_p_চাবি;
	}
	যদি (buf->b_p_চাবি == রিক্ত)
	    buf->b_p_চাবি = empty_option;
    }
#endif

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    যদি (b0_ff != 0)
	set_fileformat(b0_ff - 1, OPT_LOCAL);
    যদি (b0_fenc != রিক্ত)
    {
	set_option_মান_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    page_গুনতি = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    সারি_গুনতি = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_স্তূপ_শীর্ষ = 0;
    buf->b_ml.ml_স্তূপ = রিক্ত;
    buf->b_ml.ml_স্তূপ_size = 0;	// no stack yet

    যদি (curbuf->b_ffname == রিক্ত)
	cannot_open = TRUE;
    অন্যথা
	cannot_open = FALSE;

    serious_error = FALSE;
    প্রতি ( ; !got_int; সারি_breakcheck())
    {
	যদি (hp != রিক্ত)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	যদি ((hp = mf_get(mfp, bnum, page_গুনতি)) == রিক্ত)
	{
	    যদি (bnum == 1)
	    {
		semsg(_(e_unable_to_read_block_one_from_str), mfp->mf_fname);
		গচ্ছ theend;
	    }
	    ++error;
	    ml_append(lnum++, (char_u *)_("???MANY LINES MISSING"),
							    (colnr_T)0, TRUE);
	}
	অন্যথা		// there is a block
	{
	    pp = (PTR_BL *)(hp->bh_data);
	    যদি (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		পূর্ণাঙ্ক ptr_block_error = FALSE;
		যদি (pp->pb_গুনতি_max != PB_গুনতি_MAX(mfp))
		{
		    ptr_block_error = TRUE;
		    pp->pb_গুনতি_max = PB_গুনতি_MAX(mfp);
		}
		যদি (pp->pb_গুনতি > pp->pb_গুনতি_max)
		{
		    ptr_block_error = TRUE;
		    pp->pb_গুনতি = pp->pb_গুনতি_max;
		}
		যদি (ptr_block_error)
		    emsg(_(e_warning_pointer_block_corrupted));

		// check line count when using pointer block first time
		যদি (idx == 0 && সারি_গুনতি != 0)
		{
		    প্রতি (i = 0; i < (পূর্ণাঙ্ক)pp->pb_গুনতি; ++i)
			সারি_গুনতি -= pp->pb_pointer[i].pe_সারি_গুনতি;
		    যদি (সারি_গুনতি != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		যদি (pp->pb_গুনতি == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		অন্যথা যদি (idx < (পূর্ণাঙ্ক)pp->pb_গুনতি)	// go a block deeper
		{
		    যদি (pp->pb_pointer[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			যদি (!cannot_open)
			{
			    সারি_গুনতি = pp->pb_pointer[idx].pe_সারি_গুনতি;
			    linenr_T pe_old_lnum = pp->pb_pointer[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    যদি (সারি_গুনতি <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, রিক্ত, lnum,
					pe_old_lnum - 1,
					সারি_গুনতি, রিক্ত, 0) != OK)
				cannot_open = TRUE;
			    অন্যথা
				lnum += সারি_গুনতি;
			}
			যদি (cannot_open)
			{
			    ++error;
			    ml_append(lnum++, (char_u *)_("???LINES MISSING"),
							    (colnr_T)0, TRUE);
			}
			++idx;	    // get same block again for next index
			অনুবর্ত;
		    }

		    /*
		     * going one block deeper in the tree
		     */
		    যদি ((শীর্ষ = ml_add_স্তূপ(buf)) < 0)	// new entry in stack
		    {
			++error;
			ভঙ্গ;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_স্তূপ[শীর্ষ]);
		    ip->ip_bnum = bnum;
		    ip->ip_index = idx;

		    bnum = pp->pb_pointer[idx].pe_bnum;
		    সারি_গুনতি = pp->pb_pointer[idx].pe_সারি_গুনতি;
		    page_গুনতি = pp->pb_pointer[idx].pe_page_গুনতি;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    যদি (page_গুনতি < 1
			    || bnum + page_গুনতি > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_index + 1;
			bnum = ip->ip_bnum;
			page_গুনতি = 1;
			--buf->b_ml.ml_স্তূপ_শীর্ষ;
			অনুবর্ত;
		    }
		    idx = 0;
		    অনুবর্ত;
		}
	    }
	    অন্যথা	    // not a pointer block
	    {
		dp = (DATA_BL *)(hp->bh_data);
		যদি (dp->db_id != DATA_ID)	// block id wrong
		{
		    যদি (bnum == 1)
		    {
			semsg(_(e_block_one_id_wrong_str_not_swp_file),
							       mfp->mf_fname);
			গচ্ছ theend;
		    }
		    ++error;
		    ml_append(lnum++, (char_u *)_("???BLOCK MISSING"),
							    (colnr_T)0, TRUE);
		}
		অন্যথা
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
		    যদি (hp->bh_page_গুনতি != page_গুনতি)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			page_গুনতি = hp->bh_page_গুনতি;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    যদি (page_গুনতি * mfp->mf_page_size != dp->db_txt_end)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_end = page_গুনতি * mfp->mf_page_size;
		    }

		    যদি (dp->db_txt_start < HEADER_SIZE
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
		    যদি (সারি_গুনতি != dp->db_সারি_গুনতি)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    পূর্ণাঙ্ক did_questions = FALSE;
		    প্রতি (i = 0; i < dp->db_সারি_গুনতি; ++i)
		    {
			যদি ((char_u *)&(dp->db_index[i])
					    >= (char_u *)dp + dp->db_txt_start)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    ভঙ্গ;
			}

			txt_start = (dp->db_index[i] & DB_INDEX_MASK);
			যদি (txt_start <= (পূর্ণাঙ্ক)HEADER_SIZE
					  || txt_start >= (পূর্ণাঙ্ক)dp->db_txt_end)
			{
			    ++error;
			    // avoid lots of lines with "???"
			    যদি (did_questions)
				অনুবর্ত;
			    did_questions = TRUE;
			    p = (char_u *)"???";
			}
			অন্যথা
			{
			    did_questions = FALSE;
			    p = (char_u *)dp + txt_start;
			}
			ml_append(lnum++, p, (colnr_T)0, TRUE);
		    }
		    যদি (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	যদি (buf->b_ml.ml_স্তূপ_শীর্ষ == 0)	// finished
	    ভঙ্গ;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_স্তূপ[--(buf->b_ml.ml_স্তূপ_শীর্ষ)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_index + 1;	    // go to next index
	page_গুনতি = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    যদি (orig_file_status != OK || curbuf->b_ml.ml_সারি_গুনতি != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	যদি (!(curbuf->b_ml.ml_সারি_গুনতি == 2 && *ml_get(1) == NUL))
	{
	    changed_internal();
	    ++CHANGEDTICK(curbuf);
	}
    }
    অন্যথা
    {
	প্রতি (idx = 1; idx <= lnum; ++idx)
	{
	    // Need to copy one line, fetching the other one may flush it.
	    p = vim_strnsave(ml_get(idx), ml_get_len(idx));
	    i = STRCMP(p, ml_get(idx + lnum));
	    vim_free(p);
	    যদি (i != 0)
	    {
		changed_internal();
		++CHANGEDTICK(curbuf);
		ভঙ্গ;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    যাবত্ (curbuf->b_ml.ml_সারি_গুনতি > lnum
				       && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete(curbuf->b_ml.ml_সারি_গুনতি);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    যদি (got_int)
	emsg(_(e_recovery_interrupted));
    অন্যথা যদি (error)
    {
	++no_wait_return;
	বার্তা(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_সারিগুলি_starting_with_questions));
	--no_wait_return;
	বার্তা(_("See \":help E312\" for more information."));
	বার্তা(">>>>>>>>>>>>>");
    }
    অন্যথা
    {
	যদি (curbuf->b_changed)
	{
	    বার্তা(_("Recovery completed. You should check if everything is OK."));
	    বার্তা_puts(_("\n(You might want to write out this file under another name\n"));
	    বার্তা_puts(_("and run diff with the original file to check for changes)"));
	}
	অন্যথা
	    বার্তা(_("Recovery completed. Buffer contents equals file contents."));
	বার্তা_puts(_("\nYou may want to delete the .swp file now."));
#যদি defined(UNIX) || defined(MSWIN)
	যদি (swapfile_process_running(b0p, fname_used))
	{
	    // Warn there could be an active Vim on the same file, the user may
	    // want to kill it.
	    বার্তা_puts(_("\nNote: process STILL RUNNING: "));
	    বার্তা_outnum(char_to_long(b0p->b0_pid));
	}
#endif
	বার্তা_puts("\n\n");
	cmdline_row = বার্তা_row;
    }
#ifdef FEAT_CRYPT
    যদি (*buf->b_p_চাবি != NUL && STRCMP(curbuf->b_p_চাবি, buf->b_p_চাবি) != 0)
    {
	বার্তা_puts(_("Using crypt key from swap file for the text file.\n"));
	set_option_মান_give_err((char_u *)"key", 0L, buf->b_p_চাবি, OPT_LOCAL);
    }
#endif
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_used);
    recoverymode = FALSE;
    যদি (mfp != রিক্ত)
    {
	যদি (hp != রিক্ত)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    যদি (buf != রিক্ত)
    {
#ifdef FEAT_CRYPT
	যদি (buf->b_p_চাবি != curbuf->b_p_চাবি)
	    free_string_option(buf->b_p_চাবি);
	free_string_option(buf->b_p_cm);
#endif
	vim_free(buf->b_ml.ml_স্তূপ);
	vim_free(buf);
    }
    যদি (serious_error && called_from_main)
	ml_close(curbuf, TRUE);
    অন্যথা
    {
	apply_autocmds(EVENT_BUFREADPOST, রিক্ত, curbuf->b_fname, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, রিক্ত, curbuf->b_fname, FALSE, curbuf);
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
    পূর্ণাঙ্ক
recover_names(
    char_u	*fname,		// base for swap file name
    পূর্ণাঙ্ক		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list UNUSED, // when not NULL add file names to it
    পূর্ণাঙ্ক		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_out)	// result when "nr" > 0
{
    পূর্ণাঙ্ক		অঙ্ক_names;
    char_u	*(names[6]);
    char_u	*p;
    পূর্ণাঙ্ক		অঙ্ক_files;
    পূর্ণাঙ্ক		file_গুনতি = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = রিক্ত;
#ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#endif

    যদি (fname != রিক্ত)
    {
#ifdef HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	যদি (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	অন্যথা
#endif
	    fname_res = fname;
    }

    যদি (do_list)
    {
	// use msg() to start the scrolling properly
	বার্তা(_("Swap files found:"));
	বার্তা_putchar('\n');
    }

    /*
     * Do the loop for every directory in 'directory'.
     * First allocate some memory to put the directory name in.
     */
    dir_name.string = alloc(STRLEN(p_dir) + 1);
    dirp = p_dir;
    যাবত্ (dir_name.string != রিক্ত && *dirp)
    {
	/*
	 * Isolate a directory name from *dirp and put it in dir_name (we know
	 * it is large enough, so use 31000 for length).
	 * Advance dirp to next directory name.
	 */
	dir_name.length = (size_t)copy_option_part(&dirp, dir_name.string, 31000, ",");

	যদি (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    যদি (fname == রিক্ত)
	    {
#ifdef VMS
		names[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#অন্যথা
		names[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#endif
#যদি defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		names[1] = vim_strnsave((char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"));
		names[2] = vim_strnsave((char_u *)".sw?", STRLEN_LITERAL(".sw?"));
		অঙ্ক_names = 3;
#অন্যথা
# ifdef VMS
		names[1] = vim_strnsave((char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"));
		অঙ্ক_names = 2;
# অন্যথা
		অঙ্ক_names = 1;
# endif
#endif
	    }
	    অন্যথা
		অঙ্ক_names = recov_file_names(names, fname_res, TRUE);
	}
	অন্যথা			    // check directory dir_name
	{
	    যদি (fname == রিক্ত)
	    {
		string_T    ret;

#ifdef VMS
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#অন্যথা
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#endif
#যদি defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		names[1] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"), TRUE, &ret);
		names[2] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".sw?", STRLEN_LITERAL(".sw?"), TRUE, &ret);
		অঙ্ক_names = 3;
#অন্যথা
# ifdef VMS
		names[1] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"), TRUE, &ret);
		অঙ্ক_names = 2;
# অন্যথা
		অঙ্ক_names = 1;
# endif
#endif
	    }
	    অন্যথা
	    {
		char_u	*tail;

#যদি defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.length;
		যদি (after_pathsep(dir_name.string, p) && dir_name.length > 1 && p[-1] == p[-2])
		{
		    // Ends with '//', Use Full path for swap name
		    tail = make_percent_swname(dir_name.string, p, fname_res);
		}
		অন্যথা
#endif
		{
		    string_T	ret;

		    tail = gettail(fname_res);
		    tail = concat_fnames(dir_name.string, dir_name.length,
			tail, STRLEN(tail), TRUE, &ret);
		}
		যদি (tail == রিক্ত)
		    অঙ্ক_names = 0;
		অন্যথা
		{
		    অঙ্ক_names = recov_file_names(names, tail, FALSE);
		    vim_free(tail);
		}
	    }
	}

	// check for out-of-memory
	প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক_names; ++i)
	{
	    যদি (names[i] == রিক্ত)
	    {
		প্রতি (i = 0; i < অঙ্ক_names; ++i)
		    vim_free(names[i]);
		অঙ্ক_names = 0;
	    }
	}
	যদি (অঙ্ক_names == 0)
	    অঙ্ক_files = 0;
	অন্যথা যদি (expand_wildcards(অঙ্ক_names, names, &অঙ্ক_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    অঙ্ক_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	যদি (*dirp == NUL && file_গুনতি + অঙ্ক_files == 0 && fname != রিক্ত)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#যদি defined(VMS)
			       (char_u *)"_swp", FALSE
#অন্যথা
			       (char_u *)".swp", TRUE
#endif
			      );
	    যদি (swapname != রিক্ত)
	    {
		যদি (mch_stat((বর্ণ *)swapname, &st) != -1)    // It exists!
		{
		    files = ALLOC_ONE(char_u *);
		    যদি (files != রিক্ত)
		    {
			files[0] = swapname;
			swapname = রিক্ত;
			অঙ্ক_files = 1;
		    }
		}
		vim_free(swapname);
	    }
	}

	/*
	 * Remove swapfile name of the current buffer, it must be ignored.
	 * But keep it for swapfilelist().
	 */
	যদি (curbuf->b_ml.ml_mfp != রিক্ত
			       && (p = curbuf->b_ml.ml_mfp->mf_fname) != রিক্ত
			       && ret_list == রিক্ত)
	{
	    প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক_files; ++i)
		// Do not expand wildcards, on windows would try to expand
		// "%tmp%" in "%tmp%file".
		যদি (fullpathcmp(p, files[i], TRUE, FALSE) & FPC_SAME)
		{
		    // Remove the name from files[i].  Move further entries
		    // down.  When the array becomes empty free it here, since
		    // FreeWild() won't be called below.
		    vim_free(files[i]);
		    যদি (--অঙ্ক_files == 0)
			vim_free(files);
		    অন্যথা
			প্রতি ( ; i < অঙ্ক_files; ++i)
			    files[i] = files[i + 1];
		}
	}
	যদি (nr > 0)
	{
	    file_গুনতি += অঙ্ক_files;
	    যদি (nr <= file_গুনতি)
	    {
		*fname_out = vim_strsave(
				      files[nr - 1 + অঙ্ক_files - file_গুনতি]);
		dirp = (char_u *)"";		    // stop searching
	    }
	}
	অন্যথা যদি (do_list)
	{
	    যদি (dir_name.string[0] == '.' && dir_name.string[1] == NUL)
	    {
		যদি (fname == রিক্ত)
		    বার্তা_puts(_("   In current directory:\n"));
		অন্যথা
		    বার্তা_puts(_("   Using specified name:\n"));
	    }
	    অন্যথা
	    {
		বার্তা_puts(_("   In directory "));
		বার্তা_home_replace(dir_name.string);
		বার্তা_puts(":\n");
	    }

	    যদি (অঙ্ক_files)
	    {
		প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক_files; ++i)
		{
		    // print the swap file name
		    বার্তা_outnum((long)++file_গুনতি);
		    বার্তা_puts(".    ");
		    বার্তা_puts((বর্ণ *)gettail(files[i]));
		    বার্তা_putchar('\n');
		    (শূন্য)swapfile_info(files[i]);
		}
	    }
	    অন্যথা
		বার্তা_puts(_("      -- none --\n"));
	    out_flush();
	}
#ifdef FEAT_EVAL
	অন্যথা যদি (ret_list != রিক্ত)
	{
	    string_T	name;

	    প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.length,
		    files[i], STRLEN(files[i]), TRUE, &name);
		যদি (name.string != রিক্ত)
		{
		    list_append_string(ret_list, name.string, (পূর্ণাঙ্ক)name.length);
		    vim_free(name.string);
		}
	    }
	}
#endif
	অন্যথা
	    file_গুনতি += অঙ্ক_files;

	প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক_names; ++i)
	    vim_free(names[i]);
	যদি (অঙ্ক_files > 0)
	    FreeWild(অঙ্ক_files, files);
    }
    vim_free(dir_name.string);
    প্রত্যাগম file_গুনতি;
}

#যদি defined(UNIX) || defined(MSWIN)
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
    string_T	d = {রিক্ত, 0};
    string_T	fixed_fname;
    char_u	*p;

    fixed_fname.string = fix_fname(name != রিক্ত ? name : (char_u *)"");
    যদি (fixed_fname.string == রিক্ত)
	প্রত্যাগম রিক্ত;

    প্রতি (p = fixed_fname.string; *p != NUL; MB_PTR_ADV(p))
	যদি (vim_ispathsep(*p))
	    *p = '%';
    fixed_fname.length = (size_t)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_end[-1];
    *p = NUL;
    concat_fnames(dir, (size_t)(p - dir), fixed_fname.string, fixed_fname.length, TRUE, &d);
    vim_free(fixed_fname.string);

    প্রত্যাগম d.string;
}
#endif

#যদি (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
স্থির পূর্ণাঙ্ক process_still_running;
#endif

#যদি defined(FEAT_EVAL)
/*
 * Return information found in swapfile "fname" in dictionary "d".
 * This is used by the swapinfo() function.
 */
    শূন্য
get_b0_dict(char_u *fname, dict_T *d)
{
    পূর্ণাঙ্ক fd;
    সংরচনা block0 b0;

    যদি ((fd = mch_open((বর্ণ *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
    {
	যদি (read_eintr(fd, &b0, পরিমাণ(b0)) == পরিমাণ(b0))
	{
	    যদি (ml_check_b0_id(&b0) == FAIL)
		dict_add_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    অন্যথা যদি (b0_magic_wrong(&b0))
		dict_add_string_len(d, "error",
		    (char_u *)"Magic number mismatch", STRLEN_LITERAL("Magic number mismatch"));
	    অন্যথা
	    {
		// we have swap information
		dict_add_string_len(d, "version", b0.b0_version, 10);
		dict_add_string_len(d, "user", b0.b0_uname, B0_UNAME_SIZE);
		dict_add_string_len(d, "host", b0.b0_hname, B0_HNAME_SIZE);
		dict_add_string_len(d, "fname", b0.b0_fname, B0_FNAME_SIZE_ORG);

		dict_add_সংখ্যা(d, "pid", char_to_long(b0.b0_pid));
		dict_add_সংখ্যা(d, "mtime", char_to_long(b0.b0_mtime));
		dict_add_সংখ্যা(d, "dirty", b0.b0_dirty ? 1 : 0);
# ifdef CHECK_INODE
		dict_add_সংখ্যা(d, "inode", char_to_long(b0.b0_ino));
# endif
	    }
	}
	অন্যথা
	    dict_add_string_len(d, "error", (char_u *)"Cannot read file", STRLEN_LITERAL("Cannot read file"));
	close(fd);
    }
    অন্যথা
	dict_add_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#endif

/*
 * Give information about an existing swap file.
 * Returns timestamp (0 when unknown).
 */
    স্থির time_t
swapfile_info(char_u *fname)
{
    stat_T	    st;
    পূর্ণাঙ্ক		    fd;
    সংরচনা block0   b0;
#ifdef UNIX
    char_u	    uname[B0_UNAME_SIZE];
#endif

    // print the swap file date
    যদি (mch_stat((বর্ণ *)fname, &st) != -1)
    {
#ifdef UNIX
	// print name of owner of the file
	যদি (mch_get_uname(st.st_uid, uname, B0_UNAME_SIZE) == OK)
	{
	    বার্তা_puts(_("          owned by: "));
	    বার্তা_outtrans(uname);
	    বার্তা_puts(_("   dated: "));
	}
	অন্যথা
#endif
	    বার্তা_puts(_("             dated: "));
	বার্তা_puts(get_ctime(st.st_mtime, TRUE));
    }
    অন্যথা
	st.st_mtime = 0;

    /*
     * print the original file name
     */
    fd = mch_open((বর্ণ *)fname, O_RDONLY | O_EXTRA, 0);
    যদি (fd >= 0)
    {
	যদি (read_eintr(fd, &b0, পরিমাণ(b0)) == পরিমাণ(b0))
	{
	    যদি (STRNCMP(b0.b0_version, "VIM 3.0", 7) == 0)
	    {
		বার্তা_puts(_("         [from Vim version 3.0]"));
	    }
	    অন্যথা যদি (ml_check_b0_id(&b0) == FAIL)
	    {
		বার্তা_puts(_("         [does not look like a Vim swap file]"));
	    }
	    অন্যথা
	    {
		বার্তা_puts(_("         file name: "));
		যদি (b0.b0_fname[0] == NUL)
		    বার্তা_puts(_("[No Name]"));
		অন্যথা
		    বার্তা_outtrans(b0.b0_fname);

		বার্তা_puts(_("\n          modified: "));
		বার্তা_puts(b0.b0_dirty ? _("YES") : _("no"));

		যদি (*(b0.b0_uname) != NUL)
		{
		    বার্তা_puts(_("\n         user name: "));
		    বার্তা_outtrans(b0.b0_uname);
		}

		যদি (*(b0.b0_hname) != NUL)
		{
		    যদি (*(b0.b0_uname) != NUL)
			বার্তা_puts(_("   host name: "));
		    অন্যথা
			বার্তা_puts(_("\n         host name: "));
		    বার্তা_outtrans(b0.b0_hname);
		}

		যদি (char_to_long(b0.b0_pid) != 0L)
		{
		    বার্তা_puts(_("\n        process ID: "));
		    বার্তা_outnum(char_to_long(b0.b0_pid));
#যদি defined(UNIX) || defined(MSWIN)
		    যদি (swapfile_process_running(&b0, fname))
		    {
			বার্তা_puts(_(" (STILL RUNNING)"));
# ifdef HAVE_PROCESS_STILL_RUNNING
			process_still_running = TRUE;
# endif
		    }
#endif
		}

		যদি (b0_magic_wrong(&b0))
		{
#যদি defined(MSWIN)
		    যদি (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			বার্তা_puts(_("\n         [not usable with this version of Vim]"));
		    অন্যথা
#endif
			বার্তা_puts(_("\n         [not usable on this computer]"));
		}
	    }
	}
	অন্যথা
	    বার্তা_puts(_("         [cannot be read]"));
	close(fd);
    }
    অন্যথা
	বার্তা_puts(_("         [cannot be opened]"));
    বার্তা_putchar('\n');

    প্রত্যাগম st.st_mtime;
}

/*
 * Return TRUE if the swap file looks OK and there are no changes, thus it can
 * be safely deleted.
 */
    স্থির পূর্ণাঙ্ক
swapfile_unchanged(char_u *fname)
{
    stat_T	    st;
    পূর্ণাঙ্ক		    fd;
    সংরচনা block0   b0;
    পূর্ণাঙ্ক		    ret = TRUE;

    // must be able to stat the swap file
    যদি (mch_stat((বর্ণ *)fname, &st) == -1)
	প্রত্যাগম FALSE;

    // must be able to read the first block
    fd = mch_open((বর্ণ *)fname, O_RDONLY | O_EXTRA, 0);
    যদি (fd < 0)
	প্রত্যাগম FALSE;
    যদি (read_eintr(fd, &b0, পরিমাণ(b0)) != পরিমাণ(b0))
    {
	close(fd);
	প্রত্যাগম FALSE;
    }

    // the ID and magic number must be correct
    যদি (ml_check_b0_id(&b0) == FAIL|| b0_magic_wrong(&b0))
	ret = FALSE;

    // must be unchanged
    যদি (b0.b0_dirty)
	ret = FALSE;

#যদি defined(UNIX) || defined(MSWIN)
    // Host name must be known and must equal the current host name, otherwise
    // comparing pid is meaningless.
    যদি (*(b0.b0_hname) == NUL)
    {
	ret = FALSE;
    }
    অন্যথা
    {
	char_u	    hostname[B0_HNAME_SIZE];

	mch_get_host_name(hostname, B0_HNAME_SIZE);
	hostname[B0_HNAME_SIZE - 1] = NUL;
	b0.b0_hname[B0_HNAME_SIZE - 1] = NUL; // in case of corruption
	যদি (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    যদি (char_to_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#endif

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    close(fd);
    প্রত্যাগম ret;
}

    স্থির পূর্ণাঙ্ক
recov_file_names(char_u **names, char_u *path, পূর্ণাঙ্ক prepend_dot)
{
    পূর্ণাঙ্ক		অঙ্ক_names;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    পূর্ণাঙ্ক		i;
#ifndef MSWIN
    bool    shortname = curbuf->b_shortname;

    curbuf->b_shortname = false;
#endif
    string_T	ret;

    অঙ্ক_names = 0;

    /*
     * May also add the file name with a dot prepended, for swap file in same
     * dir as original file.
     */
    যদি (prepend_dot)
    {
	names[অঙ্ক_names] = modname(path, (char_u *)".sw?", TRUE);
	যদি (names[অঙ্ক_names] == রিক্ত)
	    গচ্ছ end;
	++অঙ্ক_names;
    }

    /*
     * Form the normal swap file name pattern by appending ".sw?".
     */
#ifdef VMS
    names[অঙ্ক_names] = concat_fnames(path, STRLEN(path),
	(char_u *)"_sw%", STRLEN_LITERAL("_sw%"), FALSE, &ret);
#অন্যথা
    names[অঙ্ক_names] = concat_fnames(path, STRLEN(path),
	(char_u *)".sw?", STRLEN_LITERAL(".sw?"), FALSE, &ret);
#endif
    যদি (names[অঙ্ক_names] == রিক্ত)
	গচ্ছ end;
    যদি (অঙ্ক_names >= 1)	    // check if we have the same name twice
    {
	p = names[অঙ্ক_names - 1];
	i = (পূর্ণাঙ্ক)STRLEN(names[অঙ্ক_names - 1]) - (পূর্ণাঙ্ক)STRLEN(names[অঙ্ক_names]);
	যদি (i > 0)
	    p += i;	    // file name has been expanded to full path

	যদি (STRCMP(p, names[অঙ্ক_names]) != 0)
	    ++অঙ্ক_names;
	অন্যথা
	    vim_free(names[অঙ্ক_names]);
    }
    অন্যথা
	++অঙ্ক_names;

#ifndef MSWIN
    /*
     * Also try with 'shortname' set, in case the file is on a DOS filesystem.
     */
    curbuf->b_shortname = true;
# ifdef VMS
    names[অঙ্ক_names] = modname(path, (char_u *)"_sw%", FALSE);
# অন্যথা
    names[অঙ্ক_names] = modname(path, (char_u *)".sw?", FALSE);
# endif
    যদি (names[অঙ্ক_names] == রিক্ত)
	গচ্ছ end;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = names[অঙ্ক_names];
    i = STRLEN(names[অঙ্ক_names]) - STRLEN(names[অঙ্ক_names - 1]);
    যদি (i > 0)
	p += i;		// file name has been expanded to full path
    যদি (STRCMP(names[অঙ্ক_names - 1], p) == 0)
	vim_free(names[অঙ্ক_names]);
    অন্যথা
	++অঙ্ক_names;
#endif

end:
#ifndef MSWIN
    curbuf->b_shortname = shortname;
#endif

    প্রত্যাগম অঙ্ক_names;
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
    শূন্য
ml_sync_all(পূর্ণাঙ্ক check_file, পূর্ণাঙ্ক check_char)
{
    buf_T		*buf;
    stat_T		st;

    FOR_ALL_BUFFERS(buf)
    {
	যদি (buf->b_ml.ml_mfp == রিক্ত
		|| buf->b_ml.ml_mfp->mf_fname == রিক্ত
		|| buf->b_ml.ml_mfp->mf_fd < 0)
	    অনুবর্ত;			    // no file

#ifdef FEAT_CRYPT
	যদি (crypt_may_close_swapfile(buf, buf->b_p_চাবি,
						     crypt_get_method_nr(buf)))
	    অনুবর্ত;
#endif

	ml_flush_সারি(buf);		    // flush buffered line
					    // flush locked block
	(শূন্য)ml_find_সারি(buf, (linenr_T)0, ML_FLUSH);
	যদি (bufIsChanged(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != রিক্ত)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    যদি (mch_stat((বর্ণ *)buf->b_ffname, &st) == -1
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
	যদি (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (শূন্য)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (bufIsChanged(buf) ? MFS_FLUSH : 0));
	    যদি (check_char && ui_char_avail())	// character available now
		ভঙ্গ;
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
    শূন্য
ml_preserve(buf_T *buf, পূর্ণাঙ্ক message)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    পূর্ণাঙ্ক		status;
    পূর্ণাঙ্ক		got_int_save = got_int;

    যদি (mfp == রিক্ত || mfp->mf_fname == রিক্ত)
    {
	যদি (message)
	    emsg(_(e_cannot_preserve_there_is_no_swap_file));
	প্রত্যাগম;
    }
#ifdef FEAT_CRYPT
    যদি (crypt_may_close_swapfile(buf, buf->b_p_চাবি, crypt_get_method_nr(buf)))
	প্রত্যাগম;
#endif

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_সারি(buf);				    // flush buffered line
    (শূন্য)ml_find_সারি(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    status = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

    // stack is invalid after mf_sync(.., MFS_ALL)
    buf->b_ml.ml_স্তূপ_শীর্ষ = 0;

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
    যদি (mf_need_trans(mfp) && !got_int)
    {
	lnum = 1;
	যাবত্ (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_সারি_গুনতি)
	{
	    hp = ml_find_সারি(buf, lnum, ML_FIND);
	    যদি (hp == রিক্ত)
	    {
		status = FAIL;
		গচ্ছ theend;
	    }
	    CHECK(buf->b_ml.ml_locked_নিচের != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_উপরের + 1;
	}
	(শূন্য)ml_find_সারি(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	যদি (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    status = FAIL;
	buf->b_ml.ml_স্তূপ_শীর্ষ = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    যদি (message)
    {
	যদি (status == OK)
	    বার্তা(_("File preserved"));
	অন্যথা
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
    প্রত্যাগম ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
    char_u *
ml_get_pos(pos_T *pos)
{
    প্রত্যাগম (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->col);
}

/*
 * Return pointer to cursor line.
 */
    char_u *
ml_get_curline(শূন্য)
{
    প্রত্যাগম ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE);
}

/*
 * Return pointer to cursor position.
 */
    char_u *
ml_get_cursor(শূন্য)
{
    প্রত্যাগম (ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE) +
							curwin->w_cursor.col);
}

// return length (excluding the NUL) of the given line
    colnr_T
ml_get_len(linenr_T lnum)
{
    প্রত্যাগম ml_get_buf_len(curbuf, lnum);
}

// return length (excluding the NUL) of the text after position "pos"
    colnr_T
ml_get_pos_len(pos_T *pos)
{
    প্রত্যাগম ml_get_buf_len(curbuf, pos->lnum) - pos->col;
}

// return length (excluding the NUL) of the cursor line
    colnr_T
ml_get_curline_len(শূন্য)
{
    প্রত্যাগম ml_get_buf_len(curbuf, curwin->w_cursor.lnum);
}

// return length (excluding the NUL) of the cursor position
    colnr_T
ml_get_cursor_len(শূন্য)
{
    প্রত্যাগম ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.col;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*সারি;

    যদি (*(সারি = ml_get_buf(buf, lnum, FALSE)) == NUL)
	প্রত্যাগম 0;

    যদি (buf->b_ml.ml_সারি_textlen <= 0)
	buf->b_ml.ml_সারি_textlen = (পূর্ণাঙ্ক)STRLEN(সারি) + 1;
    প্রত্যাগম buf->b_ml.ml_সারি_textlen - 1;
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
    পূর্ণাঙ্ক		will_change)		// line will be changed
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    স্থির পূর্ণাঙ্ক	recursive = 0;
    স্থির char_u questions[4];

    যদি (lnum > buf->b_ml.ml_সারি_গুনতি)	// invalid line number
    {
	যদি (recursive == 0)
	{
	    // Avoid giving this message for a recursive call, may happen when
	    // the GUI redraws part of the text.
	    ++recursive;
	    siemsg(e_ml_get_invalid_lnum_nr, lnum);
	    --recursive;
	}
	ml_flush_সারি(buf);
errorret:
	STRCPY(questions, "???");
	buf->b_ml.ml_সারি_len = 4;
	buf->b_ml.ml_সারি_textlen = buf->b_ml.ml_সারি_len;
	buf->b_ml.ml_সারি_lnum = lnum;
	প্রত্যাগম questions;
    }
    যদি (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    যদি (buf->b_ml.ml_mfp == রিক্ত)	// there are no lines
    {
	buf->b_ml.ml_সারি_len = 1;
	buf->b_ml.ml_সারি_textlen = buf->b_ml.ml_সারি_len;
	প্রত্যাগম (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    যদি (buf->b_ml.ml_সারি_lnum != lnum || mf_dont_release)
    {
	unsigned    start, end;
	পূর্ণাঙ্ক	    idx;

	ml_flush_সারি(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	যদি ((hp = ml_find_সারি(buf, lnum, ML_FIND)) == রিক্ত)
	{
	    যদি (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_find_সারি_nr_in_buffer_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    গচ্ছ errorret;
	}

	dp = (DATA_BL *)(hp->bh_data);

	idx = lnum - buf->b_ml.ml_locked_নিচের;
	start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	যদি (idx == 0)
	    end = dp->db_txt_end;
	অন্যথা
	    end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);

	buf->b_ml.ml_সারি_ptr = (char_u *)dp + start;
	buf->b_ml.ml_সারি_len = end - start;
#যদি defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	যদি (buf->b_has_textprop
			 && (size_t)buf->b_ml.ml_সারি_len > পরিমাণ(textprop_T))
	    buf->b_ml.ml_সারি_textlen = 0;  // call STRLEN() later when needed
	অন্যথা
#endif
	    buf->b_ml.ml_সারি_textlen = buf->b_ml.ml_সারি_len;
	buf->b_ml.ml_সারি_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_সারি_DIRTY | ML_ALLOCATED);
    }
    যদি (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ifdef FEAT_EVAL
	যদি (ml_get_alloc_সারিগুলি && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_সারি_DIRTY;
#endif
    }

#ifdef FEAT_EVAL
    যদি (ml_get_alloc_সারিগুলি
		 && (buf->b_ml.ml_flags & (ML_সারি_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_সারি_len);

	// make sure the text is in allocated memory
	যদি (p != রিক্ত)
	{
	    memmove(p, buf->b_ml.ml_সারি_ptr, buf->b_ml.ml_সারি_len);
	    buf->b_ml.ml_সারি_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    যদি (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_সারি_DIRTY;
	}
    }
#endif
    প্রত্যাগম buf->b_ml.ml_সারি_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    পূর্ণাঙ্ক
ml_সারি_alloced(শূন্য)
{
    প্রত্যাগম (curbuf->b_ml.ml_flags & ML_সারি_DIRTY);
}

#ifdef FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    স্থির শূন্য
add_text_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**সারি,
	    পূর্ণাঙ্ক		*len,
	    char_u	**tofree)
{
    পূর্ণাঙ্ক		round;
    পূর্ণাঙ্ক		new_prop_গুনতি = 0;
    পূর্ণাঙ্ক		গুনতি;
    পূর্ণাঙ্ক		n;
    char_u	*props;
    পূর্ণাঙ্ক		new_len = 0;  // init for gcc
    char_u	*new_সারি = রিক্ত;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    প্রতি (round = 1; round <= 2; ++round)
    {
	যদি (round == 2)
	{
	    uint16_t pc;

	    যদি (new_prop_গুনতি == 0)
		প্রত্যাগম;  // nothing to do
	    new_len = *len + (পূর্ণাঙ্ক)PROP_গুনতি_SIZE
			     + new_prop_গুনতি * (পূর্ণাঙ্ক)পরিমাণ(textprop_T);
	    new_সারি = alloc(new_len);
	    যদি (new_সারি == রিক্ত)
		প্রত্যাগম;
	    mch_memmove(new_সারি, *সারি, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_গুনতি;
	    mch_memmove(new_সারি + *len, &pc, PROP_গুনতি_SIZE);
	    new_prop_গুনতি = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	গুনতি = get_text_props(buf, lnum, &props, FALSE);
	প্রতি (n = 0; n < গুনতি; ++n)
	{
	    mch_memmove(&prop, props + n * পরিমাণ(textprop_T),
							   পরিমাণ(textprop_T));
	    যদি (prop.tp_flags & TP_FLAG_CONT_NEXT)
	    {
		যদি (round == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_PREV;
		    prop.tp_col = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_text_offset = 0;
		    mch_memmove(new_সারি + *len + (পূর্ণাঙ্ক)PROP_গুনতি_SIZE
			    + new_prop_গুনতি * পরিমাণ(textprop_T),
			    &prop, পরিমাণ(textprop_T));
		}
		++new_prop_গুনতি;
	    }
	}
    }
    *সারি = new_সারি;
    *tofree = new_সারি;
    *len = new_len;
}
#endif

    স্থির পূর্ণাঙ্ক
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*সারি_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    পূর্ণাঙ্ক		flags)		// ML_APPEND_ flags
{
    char_u	*সারি = সারি_arg;
    colnr_T	len = len_arg;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		সারি_গুনতি;	// number of indexes in current block
    পূর্ণাঙ্ক		offset;
    পূর্ণাঙ্ক		from, to;
    long	space_needed;	// space needed for new line
    পূর্ণাঙ্ক		page_size;
    পূর্ণাঙ্ক		page_গুনতি;
    পূর্ণাঙ্ক		db_idx;		// index for lnum in data block
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
#ifdef FEAT_PROP_POPUP
    char_u	*tofree = রিক্ত;
# ifdef FEAT_BYTEOFF
    colnr_T	text_len = 0;	// text len with NUL without text properties
# endif
#endif
    পূর্ণাঙ্ক		ret = FAIL;

    যদি (lnum > buf->b_ml.ml_সারি_গুনতি || buf->b_ml.ml_mfp == রিক্ত)
	প্রত্যাগম FAIL;  // lnum out of range

    যদি (lowest_marked && lowest_marked > lnum)
	lowest_marked = lnum + 1;

    যদি (len == 0)
    {
	len = (colnr_T)STRLEN(সারি) + 1;	// space needed for the text
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	text_len = len;
#endif
    }
#যদি defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    অন্যথা যদি (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	text_len = (colnr_T)STRLEN(সারি) + 1;
    অন্যথা
	text_len = len;
#endif

#ifdef FEAT_PROP_POPUP
    যদি (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	add_text_props_for_append(buf, lnum, &সারি, &len, &tofree);
#endif

    space_needed = len + INDEX_SIZE;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    page_size = mfp->mf_page_size;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    যদি ((hp = ml_find_সারি(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_ঢোকাও)) == রিক্ত)
	গচ্ছ theend;

    buf->b_ml.ml_flags &= ~ML_EMPTY;

    যদি (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    অন্যথা
	db_idx = lnum - buf->b_ml.ml_locked_নিচের;
		// get line count before the insertion
    সারি_গুনতি = buf->b_ml.ml_locked_উপরের - buf->b_ml.ml_locked_নিচের;

    dp = (DATA_BL *)(hp->bh_data);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    যদি ((long)dp->db_free < space_needed && db_idx == সারি_গুনতি - 1
					    && lnum < buf->b_ml.ml_সারি_গুনতি)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_উপরের);
	যদি ((hp = ml_find_সারি(buf, lnum + 1, ML_ঢোকাও)) == রিক্ত)
	    গচ্ছ theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	সারি_গুনতি = buf->b_ml.ml_locked_উপরের - buf->b_ml.ml_locked_নিচের;
	CHECK(buf->b_ml.ml_locked_নিচের != lnum + 1, "locked_low != lnum + 1");

	dp = (DATA_BL *)(hp->bh_data);
    }

    ++buf->b_ml.ml_সারি_গুনতি;

    যদি ((long)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_start -= len;
	dp->db_free -= space_needed;
	++(dp->db_সারি_গুনতি);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	যদি (সারি_গুনতি > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    যদি (db_idx < 0)
		offset = dp->db_txt_end;
	    অন্যথা
		offset = ((dp->db_index[db_idx]) & DB_INDEX_MASK);
	    mch_memmove((বর্ণ *)dp + dp->db_txt_start,
					  (বর্ণ *)dp + dp->db_txt_start + len,
				 (size_t)(offset - (dp->db_txt_start + len)));
	    প্রতি (i = সারি_গুনতি - 1; i > db_idx; --i)
		dp->db_index[i + 1] = dp->db_index[i] - len;
	    dp->db_index[db_idx + 1] = offset - len;
	}
	অন্যথা
	    // add line at the end (which is the start of the text)
	    dp->db_index[db_idx + 1] = dp->db_txt_start;

	/*
	 * copy the text into the block
	 */
	mch_memmove((বর্ণ *)dp + dp->db_index[db_idx + 1], সারি, (size_t)len);
	যদি (flags & ML_APPEND_MARK)
	    dp->db_index[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	যদি (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    অন্যথা	    // not enough space in data block
    {
	long	    সারি_গুনতি_বাম, সারি_গুনতি_ডান;
	পূর্ণাঙ্ক	    page_গুনতি_বাম, page_গুনতি_ডান;
	bhdr_T	    *hp_বাম;
	bhdr_T	    *hp_ডান;
	bhdr_T	    *hp_new;
	পূর্ণাঙ্ক	    সারিগুলি_moved;
	পূর্ণাঙ্ক	    data_moved = 0;	    // init to shut up gcc
	পূর্ণাঙ্ক	    total_moved = 0;	    // init to shut up gcc
	DATA_BL	    *dp_ডান, *dp_বাম;
	পূর্ণাঙ্ক	    স্তূপ_idx;
	পূর্ণাঙ্ক	    in_বাম;
	পূর্ণাঙ্ক	    lineadd;
	blocknr_T   bnum_বাম, bnum_ডান;
	linenr_T    lnum_বাম, lnum_ডান;
	পূর্ণাঙ্ক	    pb_idx;
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
	যদি (db_idx < 0)		// left block is new, right block is existing
	{
	    সারিগুলি_moved = 0;
	    in_বাম = TRUE;
	    // space_needed does not change
	}
	অন্যথা			// left block is existing, right block is new
	{
	    সারিগুলি_moved = সারি_গুনতি - db_idx - 1;
	    যদি (সারিগুলি_moved == 0)
		in_বাম = FALSE;	// put new line in right block
					// space_needed does not change
	    অন্যথা
	    {
		data_moved = ((dp->db_index[db_idx]) & DB_INDEX_MASK) -
							    dp->db_txt_start;
		total_moved = data_moved + সারিগুলি_moved * INDEX_SIZE;
		যদি ((long)dp->db_free + total_moved >= space_needed)
		{
		    in_বাম = TRUE;	// put new line in left block
		    space_needed = total_moved;
		}
		অন্যথা
		{
		    in_বাম = FALSE;	    // put new line in right block
		    space_needed += total_moved;
		}
	    }
	}

	page_গুনতি = ((space_needed + HEADER_SIZE) + page_size - 1) / page_size;
	যদি ((hp_new = ml_new_data(mfp, flags & ML_APPEND_NEW, page_গুনতি))
								       == রিক্ত)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_উপরের);
	    গচ্ছ theend;
	}
	যদি (db_idx < 0)		// left block is new
	{
	    hp_বাম = hp_new;
	    hp_ডান = hp;
	    সারি_গুনতি_বাম = 0;
	    সারি_গুনতি_ডান = সারি_গুনতি;
	}
	অন্যথা			// right block is new
	{
	    hp_বাম = hp;
	    hp_ডান = hp_new;
	    সারি_গুনতি_বাম = সারি_গুনতি;
	    সারি_গুনতি_ডান = 0;
	}
	dp_ডান = (DATA_BL *)(hp_ডান->bh_data);
	dp_বাম = (DATA_BL *)(hp_বাম->bh_data);
	bnum_বাম = hp_বাম->bh_bnum;
	bnum_ডান = hp_ডান->bh_bnum;
	page_গুনতি_বাম = hp_বাম->bh_page_গুনতি;
	page_গুনতি_ডান = hp_ডান->bh_page_গুনতি;

	/*
	 * May move the new line into the right/new block.
	 */
	যদি (!in_বাম)
	{
	    dp_ডান->db_txt_start -= len;
	    dp_ডান->db_free -= len + INDEX_SIZE;
	    dp_ডান->db_index[0] = dp_ডান->db_txt_start;
	    যদি (flags & ML_APPEND_MARK)
		dp_ডান->db_index[0] |= DB_MARKED;

	    mch_memmove((বর্ণ *)dp_ডান + dp_ডান->db_txt_start,
							   সারি, (size_t)len);
	    ++সারি_গুনতি_ডান;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	যদি (সারিগুলি_moved)
	{
	    dp_ডান->db_txt_start -= data_moved;
	    dp_ডান->db_free -= total_moved;
	    mch_memmove((বর্ণ *)dp_ডান + dp_ডান->db_txt_start,
			(বর্ণ *)dp_বাম + dp_বাম->db_txt_start,
			(size_t)data_moved);
	    offset = dp_ডান->db_txt_start - dp_বাম->db_txt_start;
	    dp_বাম->db_txt_start += data_moved;
	    dp_বাম->db_free += total_moved;

	    /*
	     * update indexes in the new block
	     */
	    প্রতি (to = সারি_গুনতি_ডান, from = db_idx + 1;
					 from < সারি_গুনতি_বাম; ++from, ++to)
		dp_ডান->db_index[to] = dp->db_index[from] + offset;
	    সারি_গুনতি_ডান += সারিগুলি_moved;
	    সারি_গুনতি_বাম -= সারিগুলি_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	যদি (in_বাম)
	{
	    dp_বাম->db_txt_start -= len;
	    dp_বাম->db_free -= len + INDEX_SIZE;
	    dp_বাম->db_index[সারি_গুনতি_বাম] = dp_বাম->db_txt_start;
	    যদি (flags & ML_APPEND_MARK)
		dp_বাম->db_index[সারি_গুনতি_বাম] |= DB_MARKED;
	    mch_memmove((বর্ণ *)dp_বাম + dp_বাম->db_txt_start,
							   সারি, (size_t)len);
	    ++সারি_গুনতি_বাম;
	}

	যদি (db_idx < 0)		// left block is new
	{
	    lnum_বাম = lnum + 1;
	    lnum_ডান = 0;
	}
	অন্যথা			// right block is new
	{
	    lnum_বাম = 0;
	    যদি (in_বাম)
		lnum_ডান = lnum + 2;
	    অন্যথা
		lnum_ডান = lnum + 1;
	}
	dp_বাম->db_সারি_গুনতি = সারি_গুনতি_বাম;
	dp_ডান->db_সারি_গুনতি = সারি_গুনতি_ডান;

	/*
	 * release the two data blocks
	 * The new one (hp_new) already has a correct blocknumber.
	 * The old one (hp, in ml_locked) gets a positive blocknumber if
	 * we changed it and we are not editing a new file.
	 */
	যদি (সারিগুলি_moved || in_বাম)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	যদি (!(flags & ML_APPEND_NEW) && db_idx >= 0 && in_বাম)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_find_সারি(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	প্রতি (স্তূপ_idx = buf->b_ml.ml_স্তূপ_শীর্ষ - 1; স্তূপ_idx >= 0;
								  --স্তূপ_idx)
	{
	    ip = &(buf->b_ml.ml_স্তূপ[স্তূপ_idx]);
	    pb_idx = ip->ip_index;
	    যদি ((hp = mf_get(mfp, ip->ip_bnum, 1)) == রিক্ত)
		গচ্ছ theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    যদি (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		গচ্ছ theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    যদি (pp->pb_গুনতি < pp->pb_গুনতি_max)
	    {
		যদি (pb_idx + 1 < (পূর্ণাঙ্ক)pp->pb_গুনতি)
		    mch_memmove(&pp->pb_pointer[pb_idx + 2],
				&pp->pb_pointer[pb_idx + 1],
			(size_t)(pp->pb_গুনতি - pb_idx - 1) * পরিমাণ(PTR_EN));
		++pp->pb_গুনতি;
		pp->pb_pointer[pb_idx].pe_সারি_গুনতি = সারি_গুনতি_বাম;
		pp->pb_pointer[pb_idx].pe_bnum = bnum_বাম;
		pp->pb_pointer[pb_idx].pe_page_গুনতি = page_গুনতি_বাম;
		pp->pb_pointer[pb_idx + 1].pe_সারি_গুনতি = সারি_গুনতি_ডান;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_ডান;
		pp->pb_pointer[pb_idx + 1].pe_page_গুনতি = page_গুনতি_ডান;

		যদি (lnum_বাম != 0)
		    pp->pb_pointer[pb_idx].pe_old_lnum = lnum_বাম;
		যদি (lnum_ডান != 0)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_ডান;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_স্তূপ_শীর্ষ = স্তূপ_idx + 1;	    // truncate stack

		যদি (lineadd)
		{
		    --(buf->b_ml.ml_স্তূপ_শীর্ষ);
		    // fix line count for rest of blocks in the stack
		    ml_lineadd(buf, lineadd);
							// fix stack itself
		    buf->b_ml.ml_স্তূপ[buf->b_ml.ml_স্তূপ_শীর্ষ].ip_উপরের +=
								      lineadd;
		    ++(buf->b_ml.ml_স্তূপ_শীর্ষ);
		}

		/*
		 * We are finished, break the loop here.
		 */
		ভঙ্গ;
	    }
	    // pointer block full
	    /*
	     * split the pointer block
	     * allocate a new pointer block
	     * move some of the pointer into the new block
	     * prepare for updating the parent block
	     */
	    প্রতি (;;)	// do this twice when splitting block 1
	    {
		hp_new = ml_new_ptr(mfp);
		যদি (hp_new == রিক্ত)	    // TODO: try to fix tree
		    গচ্ছ theend;
		pp_new = (PTR_BL *)(hp_new->bh_data);

		যদি (hp->bh_bnum != 1)
		    ভঙ্গ;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (size_t)page_size);
		pp->pb_গুনতি = 1;
		pp->pb_pointer[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_pointer[0].pe_সারি_গুনতি = buf->b_ml.ml_সারি_গুনতি;
		pp->pb_pointer[0].pe_old_lnum = 1;
		pp->pb_pointer[0].pe_page_গুনতি = 1;
		mf_put(mfp, hp, TRUE, FALSE);   // release block 1
		hp = hp_new;		// new block is to be split
		pp = pp_new;
		CHECK(স্তূপ_idx != 0, _("stack_idx should be 0"));
		ip->ip_index = 0;
		++স্তূপ_idx;	// do block 1 again later
	    }
	    /*
	     * move the pointers after the current one to the new block
	     * If there are none, the new entry will be in the new block.
	     */
	    total_moved = pp->pb_গুনতি - pb_idx - 1;
	    যদি (total_moved)
	    {
		mch_memmove(&pp_new->pb_pointer[0],
				&pp->pb_pointer[pb_idx + 1],
				(size_t)(total_moved) * পরিমাণ(PTR_EN));
		pp_new->pb_গুনতি = total_moved;
		pp->pb_গুনতি -= total_moved - 1;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_ডান;
		pp->pb_pointer[pb_idx + 1].pe_সারি_গুনতি = সারি_গুনতি_ডান;
		pp->pb_pointer[pb_idx + 1].pe_page_গুনতি = page_গুনতি_ডান;
		যদি (lnum_ডান)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_ডান;
	    }
	    অন্যথা
	    {
		pp_new->pb_গুনতি = 1;
		pp_new->pb_pointer[0].pe_bnum = bnum_ডান;
		pp_new->pb_pointer[0].pe_সারি_গুনতি = সারি_গুনতি_ডান;
		pp_new->pb_pointer[0].pe_page_গুনতি = page_গুনতি_ডান;
		pp_new->pb_pointer[0].pe_old_lnum = lnum_ডান;
	    }
	    pp->pb_pointer[pb_idx].pe_bnum = bnum_বাম;
	    pp->pb_pointer[pb_idx].pe_সারি_গুনতি = সারি_গুনতি_বাম;
	    pp->pb_pointer[pb_idx].pe_page_গুনতি = page_গুনতি_বাম;
	    যদি (lnum_বাম)
		pp->pb_pointer[pb_idx].pe_old_lnum = lnum_বাম;
	    lnum_বাম = 0;
	    lnum_ডান = 0;

	    /*
	     * recompute line counts
	     */
	    সারি_গুনতি_ডান = 0;
	    প্রতি (i = 0; i < (পূর্ণাঙ্ক)pp_new->pb_গুনতি; ++i)
		সারি_গুনতি_ডান += pp_new->pb_pointer[i].pe_সারি_গুনতি;
	    সারি_গুনতি_বাম = 0;
	    প্রতি (i = 0; i < (পূর্ণাঙ্ক)pp->pb_গুনতি; ++i)
		সারি_গুনতি_বাম += pp->pb_pointer[i].pe_সারি_গুনতি;

	    bnum_বাম = hp->bh_bnum;
	    bnum_ডান = hp_new->bh_bnum;
	    page_গুনতি_বাম = 1;
	    page_গুনতি_ডান = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	যদি (স্তূপ_idx < 0)
	{
	    iemsg(e_updated_too_many_blocks);
	    buf->b_ml.ml_স্তূপ_শীর্ষ = 0;	// invalidate stack
	}
    }

#ifdef FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (long)text_len
# অন্যথা
	    (long)len
# endif
	    , ML_CHNK_ADDLINE);
#endif

#ifdef FEAT_NETBEANS_INTG
    যদি (!(flags & ML_APPEND_NEW) && netbeans_active())
    {
	পূর্ণাঙ্ক সারি_len = (পূর্ণাঙ্ক)STRLEN(সারি);
	যদি (সারি_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, সারি, সারি_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)সারি_len, (char_u *)"\n", 1);
    }
#endif
#ifdef FEAT_JOB_CHANNEL
    যদি (!(flags & ML_APPEND_NEW) && buf->b_write_to_channel)
	channel_write_new_সারিগুলি(buf);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    vim_free(tofree);
#endif
    প্রত্যাগম ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    স্থির পূর্ণাঙ্ক
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*সারি,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    পূর্ণাঙ্ক		flags)		// ML_APPEND_ flags
{
    যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
	প্রত্যাগম FAIL;  // lnum out of range

    যদি (buf->b_ml.ml_সারি_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_সারি(buf);

#ifdef FEAT_EVAL
    যদি (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	যদি (buf->b_ml.ml_সারি_lnum != 0)
	    ml_flush_সারি(buf);
    }
#endif

    প্রত্যাগম ml_append_int(buf, lnum, সারি, len, flags);
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
    পূর্ণাঙ্ক
ml_append(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*সারি,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    পূর্ণাঙ্ক		newfile)	// flag, see above
{
    প্রত্যাগম ml_append_flags(lnum, সারি, len, newfile ? ML_APPEND_NEW : 0);
}

    পূর্ণাঙ্ক
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*সারি,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    পূর্ণাঙ্ক		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    যদি (curbuf->b_ml.ml_mfp == রিক্ত && open_buffer(FALSE, রিক্ত, 0) == FAIL)
	প্রত্যাগম FAIL;
    প্রত্যাগম ml_append_flush(curbuf, lnum, সারি, len, flags);
}


#যদি defined(FEAT_SPELL) || defined(FEAT_QUICKFIX) || defined(FEAT_PROP_POPUP)

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
    পূর্ণাঙ্ক
ml_append_buf(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*সারি,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    পূর্ণাঙ্ক		newfile)	// flag, see above
{
    যদি (buf->b_ml.ml_mfp == রিক্ত)
	প্রত্যাগম FAIL;
    প্রত্যাগম ml_append_flush(buf, lnum, সারি, len, newfile ? ML_APPEND_NEW : 0);
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
    পূর্ণাঙ্ক
ml_replace(linenr_T lnum, char_u *সারি, পূর্ণাঙ্ক copy)
{
    colnr_T len = -1;

    যদি (সারি != রিক্ত)
	len = (colnr_T)STRLEN(সারি);
    প্রত্যাগম ml_replace_len(lnum, সারি, len, FALSE, copy);
}

/*
 * Replace a line for the current buffer.  Like ml_replace() with:
 * "len_arg" is the length of the text, excluding NUL.
 * If "has_props" is TRUE then "line_arg" includes the text properties and
 * "len_arg" includes the NUL of the text and text properties.
 * When "copy" is TRUE copy the text into allocated memory, otherwise
 * "line_arg" must be allocated and will be consumed here.
 */
    পূর্ণাঙ্ক
ml_replace_len(
	linenr_T    lnum,
	char_u	    *সারি_arg,
	colnr_T	    len_arg,
	পূর্ণাঙ্ক	    has_props,
	পূর্ণাঙ্ক	    copy)
{
    char_u *সারি = সারি_arg;
    colnr_T len = len_arg;

    যদি (সারি == রিক্ত)		// just checking...
	প্রত্যাগম FAIL;

    // When starting up, we might still need to create the memfile
    যদি (curbuf->b_ml.ml_mfp == রিক্ত && open_buffer(FALSE, রিক্ত, 0) == FAIL)
	প্রত্যাগম FAIL;

    যদি (!has_props)
	++len;  // include the NUL after the text
    যদি (copy)
    {
	// copy the line to allocated memory
#ifdef FEAT_PROP_POPUP
	যদি (has_props)
	    সারি = vim_memsave(সারি, len);
	অন্যথা
#endif
	    সারি = vim_strnsave(সারি, len - 1);
	যদি (সারি == রিক্ত)
	    প্রত্যাগম FAIL;
    }

#ifdef FEAT_NETBEANS_INTG
    যদি (netbeans_active())
    {
	netbeans_removed(curbuf, lnum, 0, (long)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, সারি, (পূর্ণাঙ্ক)STRLEN(সারি));
    }
#endif
    যদি (curbuf->b_ml.ml_সারি_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_সারি(curbuf);

#ifdef FEAT_PROP_POPUP
	যদি (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#endif
    }

#ifdef FEAT_PROP_POPUP
    যদি (curbuf->b_has_textprop && !has_props)
    {
	size_t	oldtextlen = STRLEN(curbuf->b_ml.ml_সারি_ptr) + 1;

	যদি (oldtextlen < (size_t)curbuf->b_ml.ml_সারি_len)
	{
	    char_u *newline;
	    size_t textproplen = curbuf->b_ml.ml_সারি_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    যদি (newline != রিক্ত)
	    {
		mch_memmove(newline, সারি, len);
		mch_memmove(newline + len, curbuf->b_ml.ml_সারি_ptr
						    + oldtextlen, textproplen);
		vim_free(সারি);
		সারি = newline;
		len += (colnr_T)textproplen;
	    }
	}
    }
#endif

    যদি (curbuf->b_ml.ml_flags & (ML_সারি_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_সারি_ptr);	// free allocated line

    curbuf->b_ml.ml_সারি_ptr = সারি;
    curbuf->b_ml.ml_সারি_len = len;
    curbuf->b_ml.ml_সারি_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_সারি_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_সারি_DIRTY) & ~ML_EMPTY;

    প্রত্যাগম OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * Adjust text properties in line "lnum" for a deleted line.
 * When "above" is true this is the line above the deleted line, otherwise this
 * is the line below the deleted line.
 * "del_props[del_props_len]" are the properties of the deleted line.
 */
    স্থির শূন্য
adjust_text_props_for_delete(
	buf_T	    *buf,
	linenr_T    lnum,
	char_u	    *del_props,
	পূর্ণাঙ্ক	    del_props_len,
	পূর্ণাঙ্ক	    above)
{
    পূর্ণাঙ্ক		did_get_সারি = FALSE;
    পূর্ণাঙ্ক		done_del;
    পূর্ণাঙ্ক		done_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    DATA_BL	*dp;
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		সারি_start;
    long	সারি_size;
    পূর্ণাঙ্ক		this_props_len = 0;
    char_u	*text;
    size_t	textlen;
    পূর্ণাঙ্ক		found;

    প্রতি (done_del = 0; done_del < del_props_len; done_del += পরিমাণ(textprop_T))
    {
	mch_memmove(&prop_del, del_props + done_del, পরিমাণ(textprop_T));
	যদি ((above && (prop_del.tp_flags & TP_FLAG_CONT_PREV)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_NEXT))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_NEXT)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_PREV)))
	{
	    যদি (!did_get_সারি)
	    {
		did_get_সারি = TRUE;
		যদি ((hp = ml_find_সারি(buf, lnum, ML_FIND)) == রিক্ত)
		    প্রত্যাগম;

		dp = (DATA_BL *)(hp->bh_data);
		idx = lnum - buf->b_ml.ml_locked_নিচের;
		সারি_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
		যদি (idx == 0)		// first line in block, text at the end
		    সারি_size = dp->db_txt_end - সারি_start;
		অন্যথা
		    সারি_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK)
								  - সারি_start;
		text = (char_u *)dp + সারি_start;
		textlen = STRLEN(text) + 1;
		যদি ((long)textlen >= সারি_size)
		{
		    // No properties on this line.
		    যদি (above)
			internal_error("no text property above deleted line");
		    অন্যথা
			internal_error("no text property below deleted line");
		    প্রত্যাগম;
		}
		যদি ((long)textlen + (long)PROP_গুনতি_SIZE > সারি_size)
		{
		    internal_error("text property data too short");
		    প্রত্যাগম;
		}

		uint16_t pc;

		mch_memmove(&pc, text + textlen, PROP_গুনতি_SIZE);
		যদি (!text_prop_গুনতি_valid(pc, (size_t)(সারি_size - (long)textlen)))
		{
		    internal_error("text property count too large");
		    প্রত্যাগম;
		}
		this_props_len = pc * (পূর্ণাঙ্ক)পরিমাণ(textprop_T);
	    }

	    found = FALSE;
	    {
		char_u *props_start = text + textlen + PROP_গুনতি_SIZE;

		প্রতি (done_this = 0; done_this < this_props_len;
					       done_this += পরিমাণ(textprop_T))
		{
		    পূর্ণাঙ্ক		flag = above ? TP_FLAG_CONT_NEXT
							   : TP_FLAG_CONT_PREV;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_start + done_this,
							   পরিমাণ(textprop_T));
		    যদি ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			found = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_start + done_this, &prop_this,
							   পরিমাণ(textprop_T));
			ভঙ্গ;
		    }
		}
	    }
	    যদি (!found)
	    {
		যদি (above)
		    internal_error("text property above deleted line not found");
		অন্যথা
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
    স্থির পূর্ণাঙ্ক
ml_delete_int(buf_T *buf, linenr_T lnum, পূর্ণাঙ্ক flags)
{
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    পূর্ণাঙ্ক		গুনতি;	    // number of entries in block
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		স্তূপ_idx;
    পূর্ণাঙ্ক		text_start;
    পূর্ণাঙ্ক		সারি_start;
    long	সারি_size;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		ret = FAIL;
#ifdef FEAT_PROP_POPUP
    char_u	*textprop_save = রিক্ত;
    long	textprop_len = 0;
#endif

    যদি (lowest_marked && lowest_marked > lnum)
	lowest_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    যদি (buf->b_ml.ml_সারি_গুনতি == 1)	    // file becomes empty
    {
	যদি ((flags & ML_DEL_MESSAGE)
#ifdef FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_সারিগুলি
#endif
	   )
	    set_keep_বার্তা((char_u *)_(no_সারিগুলি_বার্তা), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_EMPTY;

	প্রত্যাগম i;
    }

/*
 * Find the data block containing the line.
 * This also fills the stack with the blocks from the root to the data block.
 * This also releases any locked block..
 */
    mfp = buf->b_ml.ml_mfp;
    যদি (mfp == রিক্ত)
	প্রত্যাগম FAIL;

    যদি ((hp = ml_find_সারি(buf, lnum, ML_DELETE)) == রিক্ত)
	প্রত্যাগম FAIL;

    dp = (DATA_BL *)(hp->bh_data);
    // compute line count before the delete
    গুনতি = (long)(buf->b_ml.ml_locked_উপরের)
					- (long)(buf->b_ml.ml_locked_নিচের) + 2;
    idx = lnum - buf->b_ml.ml_locked_নিচের;

    --buf->b_ml.ml_সারি_গুনতি;

    সারি_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
    যদি (idx == 0)		// first line in block, text at the end
	সারি_size = dp->db_txt_end - সারি_start;
    অন্যথা
	সারি_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK) - সারি_start;

#ifdef FEAT_NETBEANS_INTG
    যদি (netbeans_active())
	netbeans_removed(buf, lnum, 0, সারি_size);
#endif
#ifdef FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    যদি (buf->b_has_textprop)
    {
	size_t	textlen = STRLEN((char_u *)dp + সারি_start) + 1;

	textprop_len = সারি_size - (long)textlen;
	যদি (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + সারি_start + textlen,
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
    যদি (গুনতি == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = রিক্ত;

	প্রতি (স্তূপ_idx = buf->b_ml.ml_স্তূপ_শীর্ষ - 1; স্তূপ_idx >= 0;
								  --স্তূপ_idx)
	{
	    buf->b_ml.ml_স্তূপ_শীর্ষ = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_স্তূপ[স্তূপ_idx]);
	    idx = ip->ip_index;
	    যদি ((hp = mf_get(mfp, ip->ip_bnum, 1)) == রিক্ত)
		গচ্ছ theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    যদি (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		গচ্ছ theend;
	    }
	    গুনতি = --(pp->pb_গুনতি);
	    যদি (গুনতি == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    অন্যথা
	    {
		যদি (গুনতি != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_pointer[idx], &pp->pb_pointer[idx + 1],
				      (size_t)(গুনতি - idx) * পরিমাণ(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_স্তূপ_শীর্ষ = স্তূপ_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		যদি (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_স্তূপ[buf->b_ml.ml_স্তূপ_শীর্ষ].ip_উপরের +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_স্তূপ_শীর্ষ);

		ভঙ্গ;
	    }
	}
	CHECK(স্তূপ_idx < 0, _("deleted block 1?"));
    }
    অন্যথা
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	text_start = dp->db_txt_start;
	mch_memmove((বর্ণ *)dp + text_start + সারি_size,
		  (বর্ণ *)dp + text_start, (size_t)(সারি_start - text_start));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	প্রতি (i = idx; i < গুনতি - 1; ++i)
	    dp->db_index[i] = dp->db_index[i + 1] + সারি_size;

	dp->db_free += সারি_size + INDEX_SIZE;
	dp->db_txt_start += সারি_size;
	--(dp->db_সারি_গুনতি);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#ifdef FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, সারি_size
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    যদি (textprop_save != রিক্ত)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_data;
	পূর্ণাঙ্ক	    props_bytes;

	mch_memmove(&pc, textprop_save, PROP_গুনতি_SIZE);
	props_data = textprop_save + PROP_গুনতি_SIZE;
	props_bytes = pc * (পূর্ণাঙ্ক)পরিমাণ(textprop_T);
	যদি (!text_prop_গুনতি_valid(pc, (size_t)textprop_len))
	    props_bytes = 0;

	// Adjust text properties in the line above and below.
	যদি (lnum > 1)
	    adjust_text_props_for_delete(buf, lnum - 1,
					     props_data, props_bytes, TRUE);
	যদি (lnum <= buf->b_ml.ml_সারি_গুনতি)
	    adjust_text_props_for_delete(buf, lnum,
					    props_data, props_bytes, FALSE);
    }
    vim_free(textprop_save);
#endif
    প্রত্যাগম ret;
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
    পূর্ণাঙ্ক
ml_delete(linenr_T lnum)
{
    প্রত্যাগম ml_delete_flags(lnum, 0);
}

/*
 * Like ml_delete() but using flags (see ml_delete_int()).
 */
    পূর্ণাঙ্ক
ml_delete_flags(linenr_T lnum, পূর্ণাঙ্ক flags)
{
    ml_flush_সারি(curbuf);
    যদি (lnum < 1 || lnum > curbuf->b_ml.ml_সারি_গুনতি)
	প্রত্যাগম FAIL;

#ifdef FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#endif

    প্রত্যাগম ml_delete_int(curbuf, lnum, flags);
}

/*
 * set the DB_MARKED flag for line 'lnum'
 */
    শূন্য
ml_setmarked(linenr_T lnum)
{
    bhdr_T    *hp;
    DATA_BL *dp;
				    // invalid line number
    যদি (lnum < 1 || lnum > curbuf->b_ml.ml_সারি_গুনতি
					       || curbuf->b_ml.ml_mfp == রিক্ত)
	প্রত্যাগম;			    // give error message?

    যদি (lowest_marked == 0 || lowest_marked > lnum)
	lowest_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    যদি ((hp = ml_find_সারি(curbuf, lnum, ML_FIND)) == রিক্ত)
	প্রত্যাগম;		    // give error message?

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[lnum - curbuf->b_ml.ml_locked_নিচের] |= DB_MARKED;
    curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
}

/*
 * find the first line with its DB_MARKED flag set
 */
    linenr_T
ml_firstmarked(শূন্য)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    পূর্ণাঙ্ক		i;

    যদি (curbuf->b_ml.ml_mfp == রিক্ত)
	প্রত্যাগম (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    প্রতি (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_সারি_গুনতি; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	যদি ((hp = ml_find_সারি(curbuf, lnum, ML_FIND)) == রিক্ত)
	    প্রত্যাগম (linenr_T)0;		    // give error message?

	dp = (DATA_BL *)(hp->bh_data);

	প্রতি (i = lnum - curbuf->b_ml.ml_locked_নিচের;
			    lnum <= curbuf->b_ml.ml_locked_উপরের; ++i, ++lnum)
	    যদি ((dp->db_index[i]) & DB_MARKED)
	    {
		(dp->db_index[i]) &= DB_INDEX_MASK;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		lowest_marked = lnum + 1;
		প্রত্যাগম lnum;
	    }
    }

    প্রত্যাগম (linenr_T) 0;
}

/*
 * clear all DB_MARKED flags
 */
    শূন্য
ml_clearmarked(শূন্য)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    পূর্ণাঙ্ক		i;

    যদি (curbuf->b_ml.ml_mfp == রিক্ত)	    // nothing to do
	প্রত্যাগম;

    /*
     * The search starts with line lowest_marked.
     */
    প্রতি (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_সারি_গুনতি; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	যদি ((hp = ml_find_সারি(curbuf, lnum, ML_FIND)) == রিক্ত)
	    প্রত্যাগম;		// give error message?

	dp = (DATA_BL *)(hp->bh_data);

	প্রতি (i = lnum - curbuf->b_ml.ml_locked_নিচের;
			    lnum <= curbuf->b_ml.ml_locked_উপরের; ++i, ++lnum)
	    যদি ((dp->db_index[i]) & DB_MARKED)
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
    স্থির শূন্য
ml_flush_সারি(buf_T *buf)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    char_u	*new_সারি;
    char_u	*old_সারি;
    colnr_T	new_len;
    পূর্ণাঙ্ক		old_len;
    পূর্ণাঙ্ক		extra;
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		start;
    পূর্ণাঙ্ক		গুনতি;
    পূর্ণাঙ্ক		i;
    স্থির পূর্ণাঙ্ক  entered = FALSE;

    যদি (buf->b_ml.ml_সারি_lnum == 0 || buf->b_ml.ml_mfp == রিক্ত)
	প্রত্যাগম;		// nothing to do

    যদি (buf->b_ml.ml_flags & ML_সারি_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	যদি (entered)
	    প্রত্যাগম;
	entered = TRUE;

	lnum = buf->b_ml.ml_সারি_lnum;
	new_সারি = buf->b_ml.ml_সারি_ptr;

	hp = ml_find_সারি(buf, lnum, ML_FIND);
	যদি (hp == রিক্ত)
	    siemsg(e_cannot_find_সারি_nr, lnum);
	অন্যথা
	{
	    dp = (DATA_BL *)(hp->bh_data);
	    idx = lnum - buf->b_ml.ml_locked_নিচের;
	    start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	    old_সারি = (char_u *)dp + start;
	    যদি (idx == 0)	// line is last in block
		old_len = dp->db_txt_end - start;
	    অন্যথা		// text of previous line follows
		old_len = (dp->db_index[idx - 1] & DB_INDEX_MASK) - start;
	    new_len = buf->b_ml.ml_সারি_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    যদি ((পূর্ণাঙ্ক)dp->db_free >= extra)
	    {
#যদি defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		পূর্ণাঙ্ক old_prop_len = 0;
		যদি (buf->b_has_textprop)
		    old_prop_len = old_len - (পূর্ণাঙ্ক)STRLEN(old_সারি) - 1;
#endif
		// if the length changes and there are following lines
		গুনতি = buf->b_ml.ml_locked_উপরের - buf->b_ml.ml_locked_নিচের + 1;
		যদি (extra != 0 && idx < গুনতি - 1)
		{
		    // move text of following lines
		    mch_memmove((বর্ণ *)dp + dp->db_txt_start - extra,
				(বর্ণ *)dp + dp->db_txt_start,
				(size_t)(start - dp->db_txt_start));

		    // adjust pointers of this and following lines
		    প্রতি (i = idx + 1; i < গুনতি; ++i)
			dp->db_index[i] -= extra;
		}
		dp->db_index[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_start -= extra;

		// copy new line into the data block
		mch_memmove(old_সারি - extra, new_সারি, (size_t)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#যদি defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		যদি (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (পূর্ণাঙ্ক)STRLEN(new_সারি) - 1;
		}
		যদি (extra != 0)
		    ml_updatechunk(buf, lnum, (long)extra, ML_CHNK_UPDLINE);
#endif
	    }
	    অন্যথা
	    {
		/*
		 * Cannot do it in one data block: Delete and append.
		 * Append first, because ml_delete_int() cannot delete the
		 * last line in a buffer, which causes trouble for a buffer
		 * that has only one line.
		 * Don't forget to copy the mark!
		 */
		// How about handling errors???
		(শূন্য)ml_append_int(buf, lnum, new_সারি, new_len,
			 ((dp->db_index[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#ifdef FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#endif
			 );
		(শূন্য)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_সারি);

	entered = FALSE;
    }
    অন্যথা যদি (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_সারি_ptr);

    buf->b_ml.ml_flags &= ~(ML_সারি_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_সারি_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    স্থির bhdr_T *
ml_new_data(memfile_T *mfp, পূর্ণাঙ্ক negative, পূর্ণাঙ্ক page_গুনতি)
{
    bhdr_T	*hp;
    DATA_BL	*dp;

    যদি ((hp = mf_new(mfp, negative, page_গুনতি)) == রিক্ত)
	প্রত্যাগম রিক্ত;

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_id = DATA_ID;
    dp->db_txt_start = dp->db_txt_end = page_গুনতি * mfp->mf_page_size;
    dp->db_free = dp->db_txt_start - HEADER_SIZE;
    dp->db_সারি_গুনতি = 0;

    প্রত্যাগম hp;
}

/*
 * create a new, empty, pointer block
 */
    স্থির bhdr_T *
ml_new_ptr(memfile_T *mfp)
{
    bhdr_T	*hp;
    PTR_BL	*pp;

    যদি ((hp = mf_new(mfp, FALSE, 1)) == রিক্ত)
	প্রত্যাগম রিক্ত;

    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_id = PTR_ID;
    pp->pb_গুনতি = 0;
    pp->pb_গুনতি_max = PB_গুনতি_MAX(mfp);

    প্রত্যাগম hp;
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
    স্থির bhdr_T *
ml_find_সারি(buf_T *buf, linenr_T lnum, পূর্ণাঙ্ক action)
{
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    পূর্ণাঙ্ক		dirty;
    linenr_T	নিচের, উপরের;
    পূর্ণাঙ্ক		শীর্ষ;
    পূর্ণাঙ্ক		page_গুনতি;
    পূর্ণাঙ্ক		idx;

    mfp = buf->b_ml.ml_mfp;

    /*
     * If there is a locked block check if the wanted line is in it.
     * If not, flush and release the locked block.
     * Don't do this for ML_INSERT_SAME, because the stack need to be updated.
     * Don't do this for ML_FLUSH, because we want to flush the locked block.
     * Don't do this when 'swapfile' is reset, we want to load all the blocks.
     */
    যদি (buf->b_ml.ml_locked)
    {
	যদি (ML_SIMPLE(action)
		&& buf->b_ml.ml_locked_নিচের <= lnum
		&& buf->b_ml.ml_locked_উপরের >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    যদি (action == ML_ঢোকাও)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_উপরের);
	    }
	    অন্যথা যদি (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_উপরের);
	    }
	    প্রত্যাগম (buf->b_ml.ml_locked);
	}

	mf_put(mfp, buf->b_ml.ml_locked, buf->b_ml.ml_flags & ML_LOCKED_DIRTY,
					    buf->b_ml.ml_flags & ML_LOCKED_POS);
	buf->b_ml.ml_locked = রিক্ত;

	/*
	 * If lines have been added or deleted in the locked block, need to
	 * update the line count in pointer blocks.
	 */
	যদি (buf->b_ml.ml_locked_lineadd != 0)
	    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
    }

    যদি (action == ML_FLUSH)	    // nothing else to do
	প্রত্যাগম রিক্ত;

    bnum = 1;			    // start at the root of the tree
    page_গুনতি = 1;
    নিচের = 1;
    উপরের = buf->b_ml.ml_সারি_গুনতি;

    যদি (action == ML_FIND)	// first try stack entries
    {
	প্রতি (শীর্ষ = buf->b_ml.ml_স্তূপ_শীর্ষ - 1; শীর্ষ >= 0; --শীর্ষ)
	{
	    ip = &(buf->b_ml.ml_স্তূপ[শীর্ষ]);
	    যদি (ip->ip_নিচের <= lnum && ip->ip_উপরের >= lnum)
	    {
		bnum = ip->ip_bnum;
		নিচের = ip->ip_নিচের;
		উপরের = ip->ip_উপরের;
		buf->b_ml.ml_স্তূপ_শীর্ষ = শীর্ষ;	// truncate stack at prev entry
		ভঙ্গ;
	    }
	}
	যদি (শীর্ষ < 0)
	    buf->b_ml.ml_স্তূপ_শীর্ষ = 0;		// not found, start at the root
    }
    অন্যথা	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_স্তূপ_শীর্ষ = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    প্রতি (;;)
    {
	যদি ((hp = mf_get(mfp, bnum, page_গুনতি)) == রিক্ত)
	    গচ্ছ error_noblock;

	/*
	 * update high for insert/delete
	 */
	যদি (action == ML_ঢোকাও)
	    ++উপরের;
	অন্যথা যদি (action == ML_DELETE)
	    --উপরের;

	dp = (DATA_BL *)(hp->bh_data);
	যদি (dp->db_id == DATA_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_নিচের = নিচের;
	    buf->b_ml.ml_locked_উপরের = উপরের;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    প্রত্যাগম hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	যদি (pp->pb_id != PTR_ID)
	{
	    iemsg(e_pointer_block_id_wrong);
	    গচ্ছ error_block;
	}

	যদি ((শীর্ষ = ml_add_স্তূপ(buf)) < 0)	// add new entry to stack
	    গচ্ছ error_block;
	ip = &(buf->b_ml.ml_স্তূপ[শীর্ষ]);
	ip->ip_bnum = bnum;
	ip->ip_নিচের = নিচের;
	ip->ip_উপরের = উপরের;
	ip->ip_index = -1;		// index not known yet

	dirty = FALSE;
	প্রতি (idx = 0; idx < (পূর্ণাঙ্ক)pp->pb_গুনতি; ++idx)
	{
	    t = pp->pb_pointer[idx].pe_সারি_গুনতি;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    যদি ((নিচের += t) > lnum)
	    {
		ip->ip_index = idx;
		bnum = pp->pb_pointer[idx].pe_bnum;
		page_গুনতি = pp->pb_pointer[idx].pe_page_গুনতি;
		উপরের = নিচের - 1;
		নিচের -= t;

		/*
		 * a negative block number may have been changed
		 */
		যদি (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    যদি (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_pointer[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		ভঙ্গ;
	    }
	}
	যদি (idx >= (পূর্ণাঙ্ক)pp->pb_গুনতি)	    // past the end: something wrong!
	{
	    যদি (lnum > buf->b_ml.ml_সারি_গুনতি)
		siemsg(e_সারি_সংখ্যা_out_of_range_nr_past_the_end,
					      lnum - buf->b_ml.ml_সারি_গুনতি);

	    অন্যথা
		siemsg(e_সারি_গুনতি_wrong_in_block_nr, bnum);
	    গচ্ছ error_block;
	}
	যদি (action == ML_DELETE)
	{
	    pp->pb_pointer[idx].pe_সারি_গুনতি--;
	    dirty = TRUE;
	}
	অন্যথা যদি (action == ML_ঢোকাও)
	{
	    pp->pb_pointer[idx].pe_সারি_গুনতি++;
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
    যদি (action == ML_DELETE)
	ml_lineadd(buf, 1);
    অন্যথা যদি (action == ML_ঢোকাও)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_স্তূপ_শীর্ষ = 0;
    প্রত্যাগম রিক্ত;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    স্থির পূর্ণাঙ্ক
ml_add_স্তূপ(buf_T *buf)
{
    পূর্ণাঙ্ক		শীর্ষ;
    infoptr_T	*newstack;

    শীর্ষ = buf->b_ml.ml_স্তূপ_শীর্ষ;

    // may have to increase the stack size
    যদি (শীর্ষ == buf->b_ml.ml_স্তূপ_size)
    {
	CHECK(শীর্ষ > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_স্তূপ_size + স্তূপ_INCR);
	যদি (newstack == রিক্ত)
	    প্রত্যাগম -1;
	যদি (শীর্ষ > 0)
	    mch_memmove(newstack, buf->b_ml.ml_স্তূপ,
					     (size_t)শীর্ষ * পরিমাণ(infoptr_T));
	vim_free(buf->b_ml.ml_স্তূপ);
	buf->b_ml.ml_স্তূপ = newstack;
	buf->b_ml.ml_স্তূপ_size += স্তূপ_INCR;
    }

    buf->b_ml.ml_স্তূপ_শীর্ষ++;
    প্রত্যাগম শীর্ষ;
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
    স্থির শূন্য
ml_lineadd(buf_T *buf, পূর্ণাঙ্ক গুনতি)
{
    পূর্ণাঙ্ক		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    প্রতি (idx = buf->b_ml.ml_স্তূপ_শীর্ষ - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_স্তূপ[idx]);
	যদি ((hp = mf_get(mfp, ip->ip_bnum, 1)) == রিক্ত)
	    ভঙ্গ;
	pp = (PTR_BL *)(hp->bh_data);	// must be pointer block
	যদি (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_pointer_block_id_wrong_two);
	    ভঙ্গ;
	}
	pp->pb_pointer[ip->ip_index].pe_সারি_গুনতি += গুনতি;
	ip->ip_উপরের += গুনতি;
	mf_put(mfp, hp, TRUE, FALSE);
    }
}

#যদি defined(HAVE_READLINK)
/*
 * Resolve a symlink in the last component of a file name.
 * Note that f_resolve() does it for every part of the path, we don't do that
 * here.
 * If it worked returns OK and the resolved link in "buf[MAXPATHL]".
 * Otherwise returns FAIL.
 */
    পূর্ণাঙ্ক
resolve_symlink(char_u *fname, char_u *buf)
{
    char_u	tmp[MAXPATHL];
    পূর্ণাঙ্ক		ret;
    পূর্ণাঙ্ক		depth = 0;

    যদি (fname == রিক্ত)
	প্রত্যাগম FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    প্রতি (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	যদি (++depth == 100)
	{
	    semsg(_(e_symlink_loop_for_str), fname);
	    প্রত্যাগম FAIL;
	}

	ret = readlink((বর্ণ *)tmp, (বর্ণ *)buf, MAXPATHL - 1);
	যদি (ret <= 0)
	{
	    যদি (errno == EINVAL || errno == ENOENT)
	    {
		// Found non-symlink or not existing file, stop here.
		// When at the first level use the unmodified name, skip the
		// call to vim_FullName().
		যদি (depth == 1)
		    প্রত্যাগম FAIL;

		// Use the resolved name in tmp[].
		ভঙ্গ;
	    }

	    // There must be some error reading links, use original name.
	    প্রত্যাগম FAIL;
	}
	buf[ret] = NUL;

	/*
	 * Check whether the symlink is relative or absolute.
	 * If it's relative, build a new path based on the directory
	 * portion of the filename (if any) and the path the symlink
	 * points to.
	 */
	যদি (mch_isFullName(buf))
	    STRCPY(tmp, buf);
	অন্যথা
	{
	    char_u *tail;

	    tail = gettail(tmp);
	    যদি (STRLEN(tail) + STRLEN(buf) >= MAXPATHL)
		প্রত্যাগম FAIL;
	    STRCPY(tail, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    প্রত্যাগম vim_FullName(tmp, buf, MAXPATHL, TRUE);
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
    যদি (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#endif

#যদি defined(UNIX) || defined(MSWIN)  // Need _very_ long file names
    পূর্ণাঙ্ক		len = (পূর্ণাঙ্ক)STRLEN(dir_name);

    s = dir_name + len;
    যদি (after_pathsep(dir_name, s) && len > 1 && s[-1] == s[-2])
    {			       // Ends with '//', Use Full path
	r = রিক্ত;
	যদি ((s = make_percent_swname(dir_name, s, fname_res)) != রিক্ত)
	{
	    r = modname(s, (char_u *)".swp", FALSE);
	    vim_free(s);
	}
	প্রত্যাগম r;
    }
#endif

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#যদি defined(VMS)
	    "_swp",
#অন্যথা
	    ".swp",
#endif
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    যদি (r == রিক্ত)	    // out of memory
	প্রত্যাগম রিক্ত;

    s = get_file_in_dir(r, dir_name);
    vim_free(r);
    প্রত্যাগম s;
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

    যদি (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (size_t)(tail.string - fname) + tail.length);
    অন্যথা
    {
	size_t	dname_len = STRLEN(dname);

	যদি (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    যদি (tail.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    tail.string, tail.length, TRUE, &retval);
	    অন্যথা
	    {
		পূর্ণাঙ্ক	    save_char;
		string_T    tmp;

		save_char = *tail.string;
		*tail.string = NUL;
		concat_fnames(fname, (size_t)(tail.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*tail.string = save_char;
		যদি (tmp.string == রিক্ত)	    // out of memory
		    retval.string = রিক্ত;
		অন্যথা
		{
		    concat_fnames(tmp.string, tmp.length,
			tail.string, tail.length, TRUE, &retval);
		    vim_free(tmp.string);
		}
	    }
	}
	অন্যথা
	    concat_fnames(dname, dname_len, tail.string, tail.length,
		TRUE, &retval);
    }

#ifdef MSWIN
    যদি (retval.string != রিক্ত)
    {
	char_u	*t;

	প্রতি (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    যদি (*t == ':')
		*t = '%';
    }
#endif

    প্রত্যাগম retval.string;
}

/*
 * Print the ATTENTION message: info about an existing swap file.
 */
    স্থির শূন্য
attention_message(
    buf_T   *buf,	// buffer being edited
    char_u  *fname)	// swap file name
{
    stat_T	st;
    time_t	swap_mtime;

    ++no_wait_return;
    (শূন্য)emsg(_(e_attention));
    বার্তা_puts(_("\nFound a swap file by the name \""));
    বার্তা_home_replace(fname);
    বার্তা_puts("\"\n");
    swap_mtime = swapfile_info(fname);
    বার্তা_puts(_("While opening file \""));
    বার্তা_outtrans(buf->b_fname);
    বার্তা_puts("\"\n");
    যদি (mch_stat((বর্ণ *)buf->b_fname, &st) == -1)
    {
	বার্তা_puts(_("      CANNOT BE FOUND"));
    }
    অন্যথা
    {
	বার্তা_puts(_("             dated: "));
	বার্তা_puts(get_ctime(st.st_mtime, TRUE));
	যদি (swap_mtime != 0 && st.st_mtime > swap_mtime)
	    বার্তা_puts(_("      NEWER than swap file!\n"));
    }
    // Some of these messages are long to allow translation to
    // other languages.
    বার্তা_puts(_("\n(1) Another program may be editing the same file.  If this is the case,\n    be careful not to end up with two different instances of the same\n    file when making changes.  Quit, or continue with caution.\n"));
    বার্তা_puts(_("(2) An edit session for this file crashed.\n"));
    বার্তা_puts(_("    If this is the case, use \":recover\" or \"vim -r "));
    বার্তা_outtrans(buf->b_fname);
    বার্তা_puts(_("\"\n    to recover the changes (see \":help recovery\").\n"));
    বার্তা_puts(_("    If you did this already, delete the swap file \""));
    বার্তা_outtrans(fname);
    বার্তা_puts(_("\"\n    to avoid this message.\n"));
    cmdline_row = বার্তা_row;
    --no_wait_return;
}

প্রকারনির্দেশ গণনা {
    SEA_CHOICE_NONE = 0,
    SEA_CHOICE_READONLY = 1,
    SEA_CHOICE_EDIT = 2,
    SEA_CHOICE_RECOVER = 3,
    SEA_CHOICE_DELETE = 4,
    SEA_CHOICE_QUIT = 5,
    SEA_CHOICE_ABORT = 6
} sea_choice_T;

#যদি defined(FEAT_EVAL)
/*
 * Trigger the SwapExists autocommands.
 * Returns a value for equivalent to do_dialog().
 */
    স্থির sea_choice_T
do_swapexists(buf_T *buf, char_u *fname)
{
    set_vim_var_string(VV_SWAPNAME, fname, -1);
    set_vim_var_string(VV_SWAPCHOICE, রিক্ত, -1);

    // Trigger SwapExists autocommands with <afile> set to the file being
    // edited.  Disallow changing directory here.
    ++allbuf_lock;
    apply_autocmds(EVENT_SWAPEXISTS, buf->b_fname, রিক্ত, FALSE, রিক্ত);
    --allbuf_lock;

    set_vim_var_string(VV_SWAPNAME, রিক্ত, -1);

    বিকল্প (*get_vim_var_str(VV_SWAPCHOICE))
    {
	পক্ষ 'o': প্রত্যাগম SEA_CHOICE_READONLY;
	পক্ষ 'e': প্রত্যাগম SEA_CHOICE_EDIT;
	পক্ষ 'r': প্রত্যাগম SEA_CHOICE_RECOVER;
	পক্ষ 'd': প্রত্যাগম SEA_CHOICE_DELETE;
	পক্ষ 'q': প্রত্যাগম SEA_CHOICE_QUIT;
	পক্ষ 'a': প্রত্যাগম SEA_CHOICE_ABORT;
    }

    প্রত্যাগম SEA_CHOICE_NONE;
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
    স্থির char_u *
findswapname(
    buf_T	*buf,
    char_u	**dirp,		// pointer to list of directories
    char_u	*old_fname)	// don't give warning for this file name
{
    char_u	*fname;
    পূর্ণাঙ্ক		n;
    char_u	*dir_name;
#ifdef AMIGA
    BPTR	fh;
#endif
    পূর্ণাঙ্ক		r;
    char_u	*buf_fname = buf->b_fname;

#যদি !defined(UNIX)
# define CREATE_DUMMY_FILE
    FILE	*dummyfd = রিক্ত;

# ifdef MSWIN
    যদি (buf_fname != রিক্ত && !mch_isFullName(buf_fname)
				       && vim_strchr(gettail(buf_fname), ':'))
    {
	char_u *t;

	buf_fname = vim_strsave(buf_fname);
	যদি (buf_fname == রিক্ত)
	    buf_fname = buf->b_fname;
	অন্যথা
	    প্রতি (t = gettail(buf_fname); *t != NUL; MB_PTR_ADV(t))
		যদি (*t == ':')
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
    যদি (!(buf->b_p_sn || buf->b_shortname) && buf_fname != রিক্ত
					     && mch_getperm(buf_fname) < 0)
	dummyfd = mch_fopen((বর্ণ *)buf_fname, "w");
#endif

    /*
     * Isolate a directory name from *dirp and put it in dir_name.
     * First allocate some memory to put the directory name in.
     */
    dir_name = alloc(STRLEN(*dirp) + 1);
    যদি (dir_name == রিক্ত)
	*dirp = রিক্ত;
    অন্যথা
	(শূন্য)copy_option_part(dirp, dir_name, 31000, ",");

    /*
     * we try different names until we find one that does not exist yet
     */
    যদি (dir_name == রিক্ত)	    // out of memory
	fname = রিক্ত;
    অন্যথা
	fname = makeswapname(buf_fname, buf->b_ffname, buf, dir_name);

    প্রতি (;;)
    {
	যদি (fname == রিক্ত)	// must be out of memory
	    ভঙ্গ;
	যদি ((n = (পূর্ণাঙ্ক)STRLEN(fname)) == 0)	// safety check
	{
	    VIM_CLEAR(fname);
	    ভঙ্গ;
	}
#যদি defined(UNIX)
/*
 * Some systems have a MS-DOS compatible filesystem that use 8.3 character
 * file names. If this is the first try and the swap file name does not fit in
 * 8.3, detect if this is the case, set shortname and try again.
 */
	যদি (fname[n - 2] == 'w' && fname[n - 1] == 'p'
					&& !(buf->b_p_sn || buf->b_shortname))
	{
	    char_u	    *tail;
	    char_u	    *fname2;
	    stat_T	    s1, s2;
	    পূর্ণাঙ্ক		    f1, f2;
	    পূর্ণাঙ্ক		    created1 = FALSE, created2 = FALSE;
	    পূর্ণাঙ্ক		    same = FALSE;

	    /*
	     * Check if swapfile name does not fit in 8.3:
	     * It either contains two dots, is longer than 8 chars, or starts
	     * with a dot.
	     */
	    tail = gettail(buf_fname);
	    যদি (       vim_strchr(tail, '.') != রিক্ত
		    || STRLEN(tail) > (size_t)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		যদি (fname2 != রিক্ত)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    যদি (vim_strchr(tail, '.') != রিক্ত)
			fname2[n - 1] = 'x';
		    অন্যথা যদি (*gettail(fname) == '.')
		    {
			fname2[n] = 'x';
			fname2[n + 1] = NUL;
		    }
		    অন্যথা
			fname2[n - 5] += 1;
		    /*
		     * may need to create the files to be able to use mch_stat()
		     */
		    f1 = mch_open((বর্ণ *)fname, O_RDONLY | O_EXTRA, 0);
		    যদি (f1 < 0)
		    {
			f1 = mch_open_rw((বর্ণ *)fname,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			created1 = TRUE;
		    }
		    যদি (f1 >= 0)
		    {
			f2 = mch_open((বর্ণ *)fname2, O_RDONLY | O_EXTRA, 0);
			যদি (f2 < 0)
			{
			    f2 = mch_open_rw((বর্ণ *)fname2,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			    created2 = TRUE;
			}
			যদি (f2 >= 0)
			{
			    /*
			     * Both files exist now. If mch_stat() returns the
			     * same device and inode they are the same file.
			     */
			    যদি (mch_fstat(f1, &s1) != -1
				    && mch_fstat(f2, &s2) != -1
				    && s1.st_dev == s2.st_dev
				    && s1.st_ino == s2.st_ino)
				same = TRUE;
			    close(f2);
			    যদি (created2)
				mch_remove(fname2);
			}
			close(f1);
			যদি (created1)
			    mch_remove(fname);
		    }
		    vim_free(fname2);
		    যদি (same)
		    {
			buf->b_shortname = true;
			vim_free(fname);
			fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
			অনুবর্ত;	// try again with b_shortname set
		    }
		}
	    }
	}
#endif
	/*
	 * check if the swapfile already exists
	 */
	যদি (mch_getperm(fname) < 0)	// it does not exist
	{
#ifdef HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    যদি (mch_lstat((বর্ণ *)fname, &sb) < 0)
#অন্যথা
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (long)MODE_NEWFILE);
	    /*
	     * on the Amiga mch_getperm() will return -1 when the file exists
	     * but is being used by another program. This happens if you edit
	     * a file twice.
	     */
	    যদি (fh != (BPTR)রিক্ত)	// can open file, OK
	    {
		Close(fh);
		mch_remove(fname);
		ভঙ্গ;
	    }
	    যদি (IoErr() != ERROR_OBJECT_IN_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#endif
		ভঙ্গ;
	}

	/*
	 * A file name equal to old_fname is OK to use.
	 */
	যদি (old_fname != রিক্ত && fnamecmp(fname, old_fname) == 0)
	    ভঙ্গ;

	/*
	 * get here when file already exists
	 */
	যদি (fname[n - 2] == 'w' && fname[n - 1] == 'p')	// first try
	{
	    /*
	     * on MS-DOS compatible filesystems (e.g. messydos) file.doc.swp
	     * and file.doc are the same file. To guess if this problem is
	     * present try if file.doc.swx exists. If it does, we set
	     * buf->b_shortname and try file_doc.swp (dots replaced by
	     * underscores for this file), and try again. If it doesn't we
	     * assume that "file.doc.swp" already exists.
	     */
	    যদি (!(buf->b_p_sn || buf->b_shortname))	// not tried yet
	    {
		fname[n - 1] = 'x';
		r = mch_getperm(fname);		// try "file.swx"
		fname[n - 1] = 'p';
		যদি (r >= 0)		    // "file.swx" seems to exist
		{
		    buf->b_shortname = true;
		    vim_free(fname);
		    fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
		    অনুবর্ত;	    // try again with '.' replaced with '_'
		}
	    }
	    /*
	     * If we get here the ".swp" file really exists.
	     * Give an error message, unless recovering, no file name, we are
	     * viewing a help file or when the path of the file is different
	     * (happens when all .swp files are in one directory).
	     */
	    যদি (!recoverymode && buf_fname != রিক্ত
				&& !buf->b_help
				&& !(buf->b_flags & (BF_DUMMY | BF_NO_SEA)))
	    {
		পূর্ণাঙ্ক		fd;
		সংরচনা block0	b0;
		পূর্ণাঙ্ক		differ = FALSE;

		/*
		 * Try to read block 0 from the swap file to get the original
		 * file name (and inode number).
		 */
		fd = mch_open((বর্ণ *)fname, O_RDONLY | O_EXTRA, 0);
		যদি (fd >= 0)
		{
		    যদি (read_eintr(fd, &b0, পরিমাণ(b0)) == পরিমাণ(b0))
		    {
			/*
			 * If the swapfile has the same directory as the
			 * buffer don't compare the directory names, they can
			 * have a different mountpoint.
			 */
			যদি (b0.b0_flags & B0_SAME_DIR)
			{
			    যদি (fnamecmp(gettail(buf->b_ffname),
						   gettail(b0.b0_fname)) != 0
				    || !same_directory(fname, buf->b_ffname))
			    {
#ifdef CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				যদি (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
#endif
				    differ = TRUE;
			    }
			}
			অন্যথা
			{
			    /*
			     * The name in the swap file may be
			     * "~user/path/file".  Expand it first.
			     */
			    expand_env(b0.b0_fname, NameBuff, MAXPATHL);
#ifdef CHECK_INODE
			    যদি (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
				differ = TRUE;
#অন্যথা
			    যদি (fnamecmp(NameBuff, buf->b_ffname) != 0)
				differ = TRUE;
#endif
			}
		    }
		    close(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		যদি (differ == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == রিক্ত)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#ifdef CREATE_DUMMY_FILE
		    পূর্ণাঙ্ক		 did_use_dummy = FALSE;

		    // Avoid getting a warning for the file being created
		    // outside of Vim, it was created at the start of this
		    // function.  Delete the file now, because Vim might exit
		    // here if the window is closed.
		    যদি (dummyfd != রিক্ত)
		    {
			fclose(dummyfd);
			dummyfd = রিক্ত;
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
		    যদি (mch_stat((বর্ণ *)buf->b_fname, &st) == 0
						  && swapfile_unchanged(fname))
		    {
			choice = SEA_CHOICE_DELETE;
			যদি (p_verbose > 0)
			    verb_বার্তা(_("Found a swap file that is not useful, deleting it"));
		    }

#যদি defined(FEAT_EVAL)
		    /*
		     * If there is an SwapExists autocommand and we can handle
		     * the response, trigger it.  It may return 0 to ask the
		     * user anyway.
		     */
		    যদি (choice == SEA_CHOICE_NONE
			    && swap_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#endif

		    যদি (choice == SEA_CHOICE_NONE
					 && swap_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    যদি (choice == SEA_CHOICE_NONE)
		    {
#ifdef FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			যদি (gui.starting && !gui.in_use)
			    gui_start(রিক্ত);
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

#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    যদি (swap_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    prefix = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			size_t	    message_size;
			string_T    message;
			char_u	    *tofree;
			পূর্ণাঙ্ক	    dialog_result;

			prefix.length = STRLEN(prefix.string);
			suffix.length = STRLEN(suffix.string);
			message_size = prefix.length
				+ STRLEN(fname)
				+ suffix.length + 5;
			message.string = tofree = alloc(message_size);
			যদি (message.string != রিক্ত)
			{
			    STRCPY(message.string, prefix.string);
			    message.length = prefix.length;
			    message.length += home_replace(রিক্ত, fname,
				message.string + message.length, (পূর্ণাঙ্ক)(message_size - message.length), TRUE);
			    STRCPY(message.string + message.length, suffix.string);
			}
			অন্যথা
			    message.string = (char_u *)_("Swap file already exists!");
			dialog_result = do_dialog(VIM_WARNING,
				    (char_u *)_("VIM - ATTENTION"),
				    message.string,
# ifdef HAVE_PROCESS_STILL_RUNNING
				    process_still_running
					? (char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Quit\n&Abort") :
# endif
					(char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Delete it\n&Quit\n&Abort"), 1, রিক্ত, FALSE);

# ifdef HAVE_PROCESS_STILL_RUNNING
			যদি (process_still_running && dialog_result >= 4)
			    // compensate for missing "Delete it" button
			    dialog_result++;
# endif
			choice = dialog_result;
			vim_free(tofree);

			// pretend screen didn't scroll, need redraw anyway
			বার্তা_scrolled = 0;
			redraw_all_later(UPD_NOT_VALID);
		    }
#endif

		    বিকল্প (choice)
		    {
			পক্ষ SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    ভঙ্গ;
			পক্ষ SEA_CHOICE_EDIT:
			    ভঙ্গ;
			পক্ষ SEA_CHOICE_RECOVER:
			    swap_exists_action = SEA_RECOVER;
			    ভঙ্গ;
			পক্ষ SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    ভঙ্গ;
			পক্ষ SEA_CHOICE_QUIT:
			    swap_exists_action = SEA_QUIT;
			    ভঙ্গ;
			পক্ষ SEA_CHOICE_ABORT:
			    swap_exists_action = SEA_QUIT;
			    got_int = TRUE;
			    ভঙ্গ;
			পক্ষ SEA_CHOICE_NONE:
			    বার্তা_puts("\n");
			    যদি (বার্তা_silent == 0)
				// call wait_return() later
				need_wait_return = TRUE;
			    ভঙ্গ;
		    }

		    // If the file was deleted this fname can be used.
		    যদি (choice != SEA_CHOICE_NONE && mch_getperm(fname) < 0)
			ভঙ্গ;

#ifdef CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    যদি (did_use_dummy)
			dummyfd = mch_fopen((বর্ণ *)buf_fname, "w");
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
	যদি (fname[n - 1] == 'a')	// ".s?a"
	{
	    যদি (fname[n - 2] == 'a')    // ".saa": tried enough, give up
	    {
		emsg(_(e_too_many_swap_files_found));
		VIM_CLEAR(fname);
		ভঙ্গ;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#ifdef CREATE_DUMMY_FILE
    যদি (dummyfd != রিক্ত)	// file has been created temporarily
    {
	fclose(dummyfd);
	mch_remove(buf_fname);
    }
#endif
#ifdef MSWIN
    যদি (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#endif
    প্রত্যাগম fname;
}

    স্থির পূর্ণাঙ্ক
b0_magic_wrong(ZERO_BL *b0p)
{
    প্রত্যাগম (b0p->b0_magic_long != (long)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (পূর্ণাঙ্ক)B0_MAGIC_INT
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

    স্থির পূর্ণাঙ্ক
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
    পূর্ণাঙ্ক		retval_c;	    // flag: buf_c valid
    পূর্ণাঙ্ক		retval_s;	    // flag: buf_s valid

    যদি (mch_stat((বর্ণ *)fname_c, &st) == 0)
	ino_c = (ino_t)st.st_ino;

    /*
     * First we try to get the inode from the file name, because the inode in
     * the swap file may be outdated.  If that fails (e.g. this path is not
     * valid on this machine), use the inode from block 0.
     */
    যদি (mch_stat((বর্ণ *)fname_s, &st) == 0)
	ino_s = (ino_t)st.st_ino;
    অন্যথা
	ino_s = (ino_t)ino_block0;

    যদি (ino_c && ino_s)
	প্রত্যাগম (ino_c != ino_s);

    /*
     * One of the inode numbers is unknown, try a forced vim_FullName() and
     * compare the file names.
     */
    retval_c = vim_FullName(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_FullName(fname_s, buf_s, MAXPATHL, TRUE);
    যদি (retval_c == OK && retval_s == OK)
	প্রত্যাগম STRCMP(buf_c, buf_s) != 0;

    /*
     * Can't compare inodes or file names, guess that the files are different,
     * unless both appear not to exist at all, then compare with the file name
     * in the swap file.
     */
    যদি (ino_s == 0 && ino_c == 0 && retval_c == FAIL && retval_s == FAIL)
	প্রত্যাগম STRCMP(fname_c, fname_s) != 0;
    প্রত্যাগম TRUE;
}
#endif // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    স্থির শূন্য
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

    স্থির long
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

    প্রত্যাগম retval;
}

/*
 * Set the flags in the first block of the swap file:
 * - file is modified or not: buf->b_changed
 * - 'fileformat'
 * - 'fileencoding'
 */
    শূন্য
ml_setflags(buf_T *buf)
{
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    যদি (!buf->b_ml.ml_mfp)
	প্রত্যাগম;
    প্রতি (hp = buf->b_ml.ml_mfp->mf_used_last; hp != রিক্ত; hp = hp->bh_prev)
    {
	যদি (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_data);
	    b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_MASK)
						  | (get_fileformat(buf) + 1);
	    add_b0_fenc(b0p, buf);
	    hp->bh_flags |= BH_DIRTY;
	    mf_sync(buf->b_ml.ml_mfp, MFS_ZERO);
	    ভঙ্গ;
	}
    }
}

#যদি defined(FEAT_CRYPT)
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
    পূর্ণাঙ্ক		text_len;
    cryptstate_T *state;

    যদি (dp->db_id != DATA_ID)
	প্রত্যাগম data;

    state = ml_crypt_prepare(mfp, offset, FALSE);
    যদি (state == রিক্ত)
	প্রত্যাগম data;

    new_data = alloc(size);
    যদি (new_data == রিক্ত)
    {
	crypt_free_state(state);
	প্রত্যাগম রিক্ত;
    }
    head_end = (char_u *)(&dp->db_index[dp->db_সারি_গুনতি]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = size - dp->db_txt_start;

    // Copy the header and the text.
    mch_memmove(new_data, dp, head_end - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, text_start, text_len, new_data + dp->db_txt_start,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    যদি (head_end < text_start)
	vim_memset(new_data + (head_end - data), 0, text_start - head_end);

    প্রত্যাগম new_data;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    শূন্য
ml_decrypt_data(
    memfile_T	*mfp,
    char_u	*data,
    off_T	offset,
    unsigned	size)
{
    DATA_BL	*dp = (DATA_BL *)data;
    char_u	*head_end;
    char_u	*text_start;
    পূর্ণাঙ্ক		text_len;
    cryptstate_T *state;

    যদি (dp->db_id != DATA_ID)
	প্রত্যাগম;

    head_end = (char_u *)(&dp->db_index[dp->db_সারি_গুনতি]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = dp->db_txt_end - dp->db_txt_start;

    যদি (head_end > text_start || dp->db_txt_start > size
	    || dp->db_txt_end > size)
	প্রত্যাগম;  // data was messed up

    state = ml_crypt_prepare(mfp, offset, TRUE);
    যদি (state == রিক্ত)
	প্রত্যাগম;

    // Decrypt the text in place.
    crypt_decode_inplace(state, text_start, text_len, FALSE);
    crypt_free_state(state);
}

/*
 * Prepare for encryption/decryption, using the key, seed and offset.
 * Return an allocated cryptstate_T *.
 * Note: Encryption not supported for SODIUM
 */
    স্থির cryptstate_T *
ml_crypt_prepare(memfile_T *mfp, off_T offset, পূর্ণাঙ্ক reading)
{
    buf_T	*buf = mfp->mf_buffer;
    char_u	salt[50];
    পূর্ণাঙ্ক		method_nr;
    char_u	*চাবি;
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    যদি (reading && mfp->mf_old_চাবি != রিক্ত)
    {
	// Reading back blocks with the previous key/method/seed.
	method_nr = mfp->mf_old_cm;
	চাবি = mfp->mf_old_চাবি;
	arg.cat_seed = mfp->mf_old_seed;
    }
    অন্যথা
    {
	method_nr = crypt_get_method_nr(buf);
	চাবি = buf->b_p_চাবি;
	arg.cat_seed = mfp->mf_seed;
    }

    যদি (*চাবি == NUL)
	প্রত্যাগম রিক্ত;

    যদি (crypt_may_close_swapfile(buf, চাবি, method_nr))
	প্রত্যাগম রিক্ত;

    যদি (method_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((বর্ণ *)salt, পরিমাণ(salt), "%s%ld", চাবি, (long)offset);
	arg.cat_seed = রিক্ত;
	arg.cat_init_from_file = FALSE;

	প্রত্যাগম crypt_create(method_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((বর্ণ *)salt, পরিমাণ(salt), "%ld", (long)offset);

    arg.cat_salt = salt;
    arg.cat_salt_len = (পূর্ণাঙ্ক)STRLEN(salt);
    arg.cat_seed_len = MF_SEED_LEN;
    arg.cat_add_len = 0;
    arg.cat_add = রিক্ত;
    arg.cat_init_from_file = FALSE;

    প্রত্যাগম crypt_create(method_nr, চাবি, &arg);
}

#endif


#যদি defined(FEAT_BYTEOFF)

# define MLCS_MAXL 800	// max no of lines in chunk
# define MLCS_MINL 400   // should be half of MLCS_MAXL

/*
 * Keep information for finding byte offset of a line, updtype may be one of:
 * ML_CHNK_ADDLINE: Add len to parent chunk, possibly splitting it
 *	   Careful: ML_CHNK_ADDLINE may cause ml_find_line() to be called.
 * ML_CHNK_DELLINE: Subtract len from parent chunk, possibly deleting it
 * ML_CHNK_UPDLINE: Add len to parent chunk, as a signed entity.
 */
    স্থির শূন্য
ml_updatechunk(
    buf_T	*buf,
    linenr_T	সারি,
    long	len,
    পূর্ণাঙ্ক		updtype)
{
    স্থির buf_T	*ml_upd_lastbuf = রিক্ত;
    স্থির linenr_T	ml_upd_lastcurline;
    স্থির পূর্ণাঙ্ক		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    পূর্ণাঙ্ক			curix = ml_upd_lastcurix;
    long		size;
    chunksize_T		*curchnk;
    পূর্ণাঙ্ক			rest;
    bhdr_T		*hp;
    DATA_BL		*dp;

    যদি (buf->b_ml.ml_usedchunks == -1 || len == 0)
	প্রত্যাগম;
    যদি (buf->b_ml.ml_chunksize == রিক্ত)
    {
	buf->b_ml.ml_chunksize = ALLOC_MULT(chunksize_T, 100);
	যদি (buf->b_ml.ml_chunksize == রিক্ত)
	{
	    buf->b_ml.ml_usedchunks = -1;
	    প্রত্যাগম;
	}
	buf->b_ml.ml_numchunks = 100;
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = 1;
	ml_upd_lastbuf = রিক্ত;		// invalidate resume cache
    }

    যদি (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_সারি_গুনতি == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (long)buf->b_ml.ml_সারি_len;
	ml_upd_lastbuf = রিক্ত;		// invalidate resume cache
	প্রত্যাগম;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    যদি (buf != ml_upd_lastbuf || সারি < curline)
    {
	curline = 1;
	curix = 0;
    }
    প্রতি (; curix < buf->b_ml.ml_usedchunks - 1
	     && সারি >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	     curix++)
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
    curchnk = buf->b_ml.ml_chunksize + curix;

    যদি (updtype == ML_CHNK_DELLINE)
	len = -len;
    curchnk->mlcs_totalsize += len;
    যদি (updtype == ML_CHNK_ADDLINE)
    {
	curchnk->mlcs_numlines++;

	// May resize here so we don't have to do it in both cases below
	যদি (buf->b_ml.ml_usedchunks + 1 >= buf->b_ml.ml_numchunks)
	{
	    chunksize_T *t_chunksize = buf->b_ml.ml_chunksize;

	    buf->b_ml.ml_numchunks = buf->b_ml.ml_numchunks * 3 / 2;
	    buf->b_ml.ml_chunksize = vim_realloc(buf->b_ml.ml_chunksize,
			    পরিমাণ(chunksize_T) * buf->b_ml.ml_numchunks);
	    যদি (buf->b_ml.ml_chunksize == রিক্ত)
	    {
		// Hmmmm, Give up on offset for this buffer
		vim_free(t_chunksize);
		buf->b_ml.ml_usedchunks = -1;
		প্রত্যাগম;
	    }
	}

	যদি (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MAXL)
	{
	    পূর্ণাঙ্ক	    গুনতি;	    // number of entries in block
	    পূর্ণাঙ্ক	    idx;
	    পূর্ণাঙ্ক	    end_idx;
	    পূর্ণাঙ্ক	    text_end;
	    পূর্ণাঙ্ক	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			পরিমাণ(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    size = 0;
	    linecnt = 0;
	    যাবত্ (curline < buf->b_ml.ml_সারি_গুনতি
			&& linecnt < MLCS_MINL)
	    {
		যদি ((hp = ml_find_সারি(buf, curline, ML_FIND)) == রিক্ত)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    প্রত্যাগম;
		}
		dp = (DATA_BL *)(hp->bh_data);
		গুনতি = (long)(buf->b_ml.ml_locked_উপরের) -
			(long)(buf->b_ml.ml_locked_নিচের) + 1;
		idx = curline - buf->b_ml.ml_locked_নিচের;
		curline = buf->b_ml.ml_locked_উপরের + 1;

		// compute index of last line to use in this MEMLINE
		rest = গুনতি - idx;
		যদি (linecnt + rest > MLCS_MINL)
		{
		    end_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		অন্যথা
		{
		    end_idx = গুনতি - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		যদি (buf->b_has_textprop)
		{
		    পূর্ণাঙ্ক i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    প্রতি (i = end_idx; i < idx; ++i)
			size += (পূর্ণাঙ্ক)STRLEN((char_u *)dp
				      + (dp->db_index[i] & DB_INDEX_MASK)) + 1;
		}
		অন্যথা
# endif
		{
		    যদি (idx == 0) // first line in block, text at the end
			text_end = dp->db_txt_end;
		    অন্যথা
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
	    ml_upd_lastbuf = রিক্ত;   // Force recalc of curix & curline
	    প্রত্যাগম;
	}
	অন্যথা যদি (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_সারি_গুনতি - সারি <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    যদি (সারি == buf->b_ml.ml_সারি_গুনতি)
	    {
		curchnk->mlcs_numlines = 0;
		curchnk->mlcs_totalsize = 0;
	    }
	    অন্যথা
	    {
		/*
		 * Line is just prior to last, move count for last
		 * This is the common case  when loading a new file
		 */
		hp = ml_find_সারি(buf, buf->b_ml.ml_সারি_গুনতি, ML_FIND);
		যদি (hp == রিক্ত)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    প্রত্যাগম;
		}
		dp = (DATA_BL *)(hp->bh_data);
		যদি (dp->db_সারি_গুনতি == 1)
		    rest = dp->db_txt_end - dp->db_txt_start;
		অন্যথা
		    rest =
			((dp->db_index[dp->db_সারি_গুনতি - 2]) & DB_INDEX_MASK)
			- dp->db_txt_start;
		curchnk->mlcs_totalsize = rest;
		curchnk->mlcs_numlines = 1;
		curchnk[-1].mlcs_totalsize -= rest;
		curchnk[-1].mlcs_numlines -= 1;
	    }
	}
    }
    অন্যথা যদি (updtype == ML_CHNK_DELLINE)
    {
	curchnk->mlcs_numlines--;
	যদি (curix < buf->b_ml.ml_usedchunks - 1
		&& curchnk->mlcs_numlines + curchnk[1].mlcs_numlines
								  <= MLCS_MINL)
	{
	    curix++;
	    curchnk = buf->b_ml.ml_chunksize + curix;
	}
	অন্যথা যদি (curix == 0 && curchnk->mlcs_numlines <= 0)
	{
	    ml_upd_lastbuf = রিক্ত;   // Force recalc of curix & curline
	    buf->b_ml.ml_usedchunks--;
	    mch_memmove(buf->b_ml.ml_chunksize, buf->b_ml.ml_chunksize + 1,
			buf->b_ml.ml_usedchunks * পরিমাণ(chunksize_T));
	    প্রত্যাগম;
	}
	অন্যথা যদি (curix == 0 || (curchnk->mlcs_numlines > 10
		    && curchnk->mlcs_numlines + curchnk[-1].mlcs_numlines
								  > MLCS_MINL))
	{
	    // The chunks are left as they are, the cached position stays
	    // valid.
	    ml_upd_lastbuf = buf;
	    ml_upd_lastcurline = curline;
	    ml_upd_lastcurix = curix;
	    প্রত্যাগম;
	}

	// Collapse chunks
	ml_upd_lastbuf = রিক্ত;   // Force recalc of curix & curline
	curchnk[-1].mlcs_numlines += curchnk->mlcs_numlines;
	curchnk[-1].mlcs_totalsize += curchnk->mlcs_totalsize;
	buf->b_ml.ml_usedchunks--;
	যদি (curix < buf->b_ml.ml_usedchunks)
	    mch_memmove(buf->b_ml.ml_chunksize + curix,
			buf->b_ml.ml_chunksize + curix + 1,
			(buf->b_ml.ml_usedchunks - curix) *
			পরিমাণ(chunksize_T));
	প্রত্যাগম;
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
ml_find_সারি_or_offset(buf_T *buf, linenr_T lnum, long *offp)
{
    linenr_T	curline;
    পূর্ণাঙ্ক		curix;
    long	size;
    bhdr_T	*hp;
    DATA_BL	*dp;
    পূর্ণাঙ্ক		গুনতি;		// number of entries in block
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		start_idx;
    পূর্ণাঙ্ক		text_end;
    long	offset;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		ffdos = (get_fileformat(buf) == EOL_DOS);
    পূর্ণাঙ্ক		extra = 0;

    // take care of cached line first
    ml_flush_সারি(curbuf);

    যদি (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == রিক্ত
	    || lnum < 0)
	প্রত্যাগম -1;

    যদি (offp == রিক্ত)
	offset = 0;
    অন্যথা
	offset = *offp;
    যদি (lnum == 0 && offset <= 0)
	প্রত্যাগম 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = size = 0;
    যাবত্ (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (offset != 0
	       && offset > size + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (long)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	size += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	যদি (offset && ffdos)
	    size += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    যাবত্ ((lnum != 0 && curline < lnum) || (offset != 0 && size < offset))
    {
# ifdef FEAT_PROP_POPUP
	size_t textprop_total = 0;
# endif

	যদি (curline > buf->b_ml.ml_সারি_গুনতি
		|| (hp = ml_find_সারি(buf, curline, ML_FIND)) == রিক্ত)
	    প্রত্যাগম -1;
	dp = (DATA_BL *)(hp->bh_data);
	গুনতি = (long)(buf->b_ml.ml_locked_উপরের) -
		(long)(buf->b_ml.ml_locked_নিচের) + 1;
	start_idx = idx = curline - buf->b_ml.ml_locked_নিচের;
	যদি (idx == 0)  // first line in block, text at the end
	    text_end = dp->db_txt_end;
	অন্যথা
	    text_end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);
	// Compute index of last line to use in this MEMLINE
	যদি (lnum != 0)
	{
	    যদি (curline + (গুনতি - idx) >= lnum)
		idx += lnum - curline - 1;
	    অন্যথা
		idx = গুনতি - 1;
	}
	অন্যথা
	{
	    extra = 0;
	    প্রতি (;;)
	    {
# ifdef FEAT_PROP_POPUP
		size_t textprop_size = 0;

		যদি (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_index[idx]) & DB_INDEX_MASK);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_end
				  : ((dp->db_index[idx - 1]) & DB_INDEX_MASK));
		    textprop_size = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		যদি (!(offset >= size
			+ text_end - (পূর্ণাঙ্ক)((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
			- (long)(textprop_total + textprop_size)
# endif
			+ ffdos))
		    ভঙ্গ;

		যদি (ffdos)
		    size++;
# ifdef FEAT_PROP_POPUP
		textprop_total += textprop_size;
# endif
		যদি (idx == গুনতি - 1)
		{
		    extra = 1;
		    ভঙ্গ;
		}
		idx++;
	    }
	}
# ifdef FEAT_PROP_POPUP
	যদি (buf->b_has_textprop && lnum != 0)
	{
	    পূর্ণাঙ্ক i;

	    // cannot use the db_index pointer, need to get the actual text
	    // lengths.
	    len = 0;
	    প্রতি (i = start_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_index[i]) & DB_INDEX_MASK);
		len += (পূর্ণাঙ্ক)STRLEN(p) + 1;
	    }
	}
	অন্যথা
# endif
	    len = text_end - ((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
				- (long)textprop_total
# endif
				;
	size += len;
	যদি (offset != 0 && size >= offset)
	{
	    যদি (size + ffdos == offset)
		*offp = 0;
	    অন্যথা যদি (idx == start_idx)
		*offp = offset - size + len;
	    অন্যথা
		*offp = offset - size + len
		     - (text_end - ((dp->db_index[idx - 1]) & DB_INDEX_MASK))
# ifdef FEAT_PROP_POPUP
		     + (long)textprop_total
# endif
		     ;
	    curline += idx - start_idx + extra;
	    যদি (curline > buf->b_ml.ml_সারি_গুনতি)
		প্রত্যাগম -1;	// exactly one byte beyond the end
	    প্রত্যাগম curline;
	}
	curline = buf->b_ml.ml_locked_উপরের + 1;
    }

    যদি (lnum != 0)
    {
	// Count extra CR characters.
	যদি (ffdos)
	    size += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	যদি ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_সারি_গুনতি)
	    size -= ffdos + 1;
    }

    প্রত্যাগম size;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    শূন্য
goto_byte(long cnt)
{
    long	boff = cnt;
    linenr_T	lnum;

    ml_flush_সারি(curbuf);	// cached line may be dirty
    setpcmark();
    যদি (boff)
	--boff;
    lnum = ml_find_সারি_or_offset(curbuf, (linenr_T)0, &boff);
    যদি (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_সারি_গুনতি;
	curwin->w_curswant = MAXCOL;
	coladvance((colnr_T)MAXCOL);
    }
    অন্যথা
    {
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = (colnr_T)boff;
	curwin->w_cursor.coladd = 0;
	curwin->w_set_curswant = true;
    }
    check_cursor();

    // Make sure the cursor is on the first byte of a multi-byte char.
    যদি (has_mbyte)
	mb_adjust_cursor();
}
#endif
