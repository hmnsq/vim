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
#define CHECK(c, s)	ಕುರು { /**/ } ಯಾವತ್ (0)

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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <time.h>

#ಯದಿ defined(SASC) || defined(__amigaos4__)
# include <proto/dos.h>	    // for Open() and Close()
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ block0		ZERO_BL;    // contents of the first block
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ pointer_block	PTR_BL;	    // contents of a pointer block
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ data_block	DATA_BL;    // contents of a data block
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ pointer_entry	PTR_EN;	    // block/line-count pair

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

#ಯದಿ defined(FEAT_CRYPT)
ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ id1_codes[] = {
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
ಸಂರಚನಾ pointer_entry
{
    blocknr_T	pe_bnum;	// block number
    linenr_T	pe_ಸಾಲು_ಎಣಿಕೆ;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    ಪೂರ್ಣಾಙ್ಕ		pe_page_ಎಣಿಕೆ;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
ಸಂರಚನಾ pointer_block
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_ಎಣಿಕೆ;	// number of pointers in this block
    short_u	pb_ಎಣಿಕೆ_max;	// maximum value for pb_count
    PTR_EN	pb_pointer[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#define PB_ಎಣಿಕೆ_MAX(mfp) (short_u)(((mfp)->mf_page_size - offsetof(PTR_BL, pb_pointer)) / ಪರಿಮಾಣ(PTR_EN))

/*
 * A data block is a leaf in the tree.
 *
 * The text of the lines is at the end of the block. The text of the first line
 * in the block is put at the end, the text of the second line in front of it,
 * etc. Thus the order of the lines is the opposite of the line number.
 */
ಸಂರಚನಾ data_block
{
    short_u	db_id;		// ID for data block: DATA_ID
    unsigned	db_free;	// free space available
    unsigned	db_txt_start;	// byte where text starts
    unsigned	db_txt_end;	// byte just after data block
    linenr_T	db_ಸಾಲು_ಎಣಿಕೆ;	// number of lines in this block
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
#define DB_MARKED	((unsigned)1 << ((ಪರಿಮಾಣ(unsigned) * 8) - 1))
#define DB_INDEX_MASK	(~DB_MARKED)

#define INDEX_SIZE  (ಪರಿಮಾಣ(unsigned))	    // size of one db_index entry
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
ಸಂರಚನಾ block0
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
    ಪೂರ್ಣಾಙ್ಕ		b0_magic_int;	// check for byte order of int
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

#define ರಾಶಿ_INCR	5	// nr of entries added to ml_stack at a time

/*
 * The line number where the first mark may be is remembered.
 * If it is 0 there are no marks at all.
 * (always used for the current buffer only, no buffer change possible while
 * executing a global command).
 */
ಸ್ಥಿರ linenr_T	lowest_marked = 0;

/*
 * arguments for ml_find_line()
 */
#define ML_DELETE	0x11	    // delete line
#define ML_ಸೇರಿಸು	0x12	    // insert line
#define ML_FIND		0x13	    // just find the line
#define ML_FLUSH	0x02	    // flush locked block
#define ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
      UB_FNAME = 0	// update timestamp and filename
    , UB_SAME_DIR       // update the B0_SAME_DIR flag
    , UB_CRYPT		// update crypt key
} upd_block0_T;

#ifdef FEAT_CRYPT
ಸ್ಥಿರ ಶೂನ್ಯ ml_set_b0_crypt(buf_T *buf, ZERO_BL *b0p);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ ml_upd_block0(buf_T *buf, upd_block0_T what);
ಸ್ಥಿರ ಶೂನ್ಯ set_b0_fname(ZERO_BL *, buf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ set_b0_dir_flag(ZERO_BL *b0p, buf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ add_b0_fenc(ZERO_BL *b0p, buf_T *buf);
ಸ್ಥಿರ time_t swapfile_info(char_u *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ recov_file_names(char_u **, char_u *, ಪೂರ್ಣಾಙ್ಕ prepend_dot);
ಸ್ಥಿರ char_u *findswapname(buf_T *, char_u **, char_u *);
ಸ್ಥಿರ ಶೂನ್ಯ ml_flush_ಸಾಲು(buf_T *);
ಸ್ಥಿರ bhdr_T *ml_new_data(memfile_T *, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ bhdr_T *ml_new_ptr(memfile_T *);
ಸ್ಥಿರ bhdr_T *ml_find_ಸಾಲು(buf_T *, linenr_T, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ml_add_ರಾಶಿ(buf_T *);
ಸ್ಥಿರ ಶೂನ್ಯ ml_lineadd(buf_T *, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ b0_magic_wrong(ZERO_BL *);
#ifdef CHECK_INODE
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fnamecmp_ino(char_u *, char_u *, long);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ long_to_char(long, char_u *);
ಸ್ಥಿರ long char_to_long(char_u *);
#ifdef FEAT_CRYPT
ಸ್ಥಿರ cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T offset, ಪೂರ್ಣಾಙ್ಕ reading);
#endif
#ifdef FEAT_BYTEOFF
ಸ್ಥಿರ ಶೂನ್ಯ ml_updatechunk(buf_T *buf, long ಸಾಲು, long len, ಪೂರ್ಣಾಙ್ಕ updtype);
#endif

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
ml_open(buf_T *buf)
{
    memfile_T	*mfp;
    bhdr_T	*hp = ರಿಕ್ತ;
    ZERO_BL	*b0p;
    PTR_BL	*pp;
    DATA_BL	*dp;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ರಾಶಿ_size = 0; // no stack yet
    buf->b_ml.ml_ರಾಶಿ = ರಿಕ್ತ;	// no stack yet
    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;	// nothing in the stack
    buf->b_ml.ml_locked = ರಿಕ್ತ;	// no cached block
    buf->b_ml.ml_ಸಾಲು_lnum = 0;	// no cached line
#ifdef FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = ರಿಕ್ತ;
    buf->b_ml.ml_usedchunks = 0;
#endif

    ಯದಿ (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    ಯದಿ (p_uc && buf->b_p_swf)
	buf->b_may_swap = true;
    ಅನ್ಯಥಾ
	buf->b_may_swap = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(ರಿಕ್ತ, 0);
    ಯದಿ (mfp == ರಿಕ್ತ)
	ಗಚ್ಛ error;

    buf->b_ml.ml_mfp = mfp;
#ifdef FEAT_CRYPT
    mfp->mf_buffer = buf;
#endif
    buf->b_ml.ml_flags = ML_EMPTY;
    buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ = 1;

/*
 * fill block0 struct and write page 0
 */
    ಯದಿ ((hp = mf_new(mfp, FALSE, 1)) == ರಿಕ್ತ)
	ಗಚ್ಛ error;
    ಯದಿ (hp->bh_bnum != 0)
    {
	iemsg(e_didnt_get_block_nr_zero);
	ಗಚ್ಛ error;
    }
    b0p = (ZERO_BL *)(hp->bh_data);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (long)B0_MAGIC_LONG;
    b0p->b0_magic_int = (ಪೂರ್ಣಾಙ್ಕ)B0_MAGIC_INT;
    b0p->b0_magic_short = (short)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_version, "VIM ", 4);
    STRNCPY(b0p->b0_version + 4, Version, 6);
    long_to_char((long)mfp->mf_page_size, b0p->b0_page_size);

#ifdef FEAT_SPELL
    ಯದಿ (!buf->b_spell)
#endif
    {
	b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	b0p->b0_flags = get_fileformat(buf) + 1;
	set_b0_fname(b0p, buf);
	(ಶೂನ್ಯ)get_user_name(b0p->b0_uname, B0_UNAME_SIZE);
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
    ಯದಿ (!buf->b_help && !B_SPELL(buf))
	(ಶೂನ್ಯ)mf_sync(mfp, 0);

    /*
     * Fill in root pointer block and write page 1.
     */
    ಯದಿ ((hp = ml_new_ptr(mfp)) == ರಿಕ್ತ)
	ಗಚ್ಛ error;
    ಯದಿ (hp->bh_bnum != 1)
    {
	iemsg(e_didnt_get_block_nr_one);
	ಗಚ್ಛ error;
    }
    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_ಎಣಿಕೆ = 1;
    pp->pb_pointer[0].pe_bnum = 2;
    pp->pb_pointer[0].pe_page_ಎಣಿಕೆ = 1;
    pp->pb_pointer[0].pe_old_lnum = 1;
    pp->pb_pointer[0].pe_ಸಾಲು_ಎಣಿಕೆ = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    ಯದಿ ((hp = ml_new_data(mfp, FALSE, 1)) == ರಿಕ್ತ)
	ಗಚ್ಛ error;
    ಯದಿ (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_block_nr_two);
	ಗಚ್ಛ error;
    }

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[0] = --dp->db_txt_start;	// at end of block
    dp->db_free -= 1 + INDEX_SIZE;
    dp->db_ಸಾಲು_ಎಣಿಕೆ = 1;
    *((char_u *)dp + dp->db_txt_start) = NUL;	// empty line

    ಪ್ರತ್ಯಾಗಮ OK;

error:
    ಯದಿ (mfp != ರಿಕ್ತ)
    {
	ಯದಿ (hp)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, TRUE);	    // will also free(mfp->mf_fname)
    }
    buf->b_ml.ml_mfp = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

#ಯದಿ defined(FEAT_CRYPT)
/*
 * Swapfile encryption is not supported by XChaCha20.  If this crypt method is
 * used then disable the swapfile, to avoid plain text being written to disk,
 * and return TRUE.
 * Otherwise return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
crypt_may_close_swapfile(buf_T *buf, char_u *ಕೀಲಿ, ಪೂರ್ಣಾಙ್ಕ method)
{
    ಯದಿ (crypt_method_is_sodium(method) && *ಕೀಲಿ != NUL)
    {
	mf_close_file(buf, TRUE);
	buf->b_p_swf = FALSE;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Prepare encryption for "buf" for the current key and method.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ml_set_mfp_crypt(buf_T *buf)
{
    ಯದಿ (*buf->b_p_ಕೀಲಿ == NUL)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ method_nr = crypt_get_method_nr(buf);

    ಯದಿ (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_seed(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN, ರಿಕ್ತ, 0);
    }
# ifdef FEAT_SODIUM
    ಅನ್ಯಥಾ ಯದಿ (crypt_method_is_sodium(method_nr))
	crypt_sodium_randombytes_buf(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN);
# endif
}

/*
 * Prepare encryption for "buf" with block 0 "b0p".
 * Note: should not be called with libsodium encryption, since xchacha20 does
 * not support swapfile encryption.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ml_set_b0_crypt(buf_T *buf, ZERO_BL *b0p)
{
    ಯದಿ (*buf->b_p_ಕೀಲಿ == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ method_nr = crypt_get_method_nr(buf);

	b0p->b0_id[1] = id1_codes[method_nr];
	ಯದಿ (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
	{
	    // Generate a seed and store it in block 0 and in the memfile.
	    sha2_seed(&b0p->b0_seed, MF_SEED_LEN, ರಿಕ್ತ, 0);
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
    ಶೂನ್ಯ
ml_set_crypt_ಕೀಲಿ(
    buf_T	*buf,
    char_u	*old_ಕೀಲಿ,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    ಪೂರ್ಣಾಙ್ಕ		page_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    long	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    DATA_BL	*dp;
    blocknr_T	bnum;
    ಪೂರ್ಣಾಙ್ಕ		ತುದಿ;
    ಪೂರ್ಣಾಙ್ಕ		old_method;

    ಯದಿ (mfp == ರಿಕ್ತ || mfp->mf_fd < 0)
	ಪ್ರತ್ಯಾಗಮ;  // no memfile yet, nothing to do
    old_method = crypt_method_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    ಯದಿ (crypt_may_close_swapfile(buf, buf->b_p_ಕೀಲಿ, crypt_get_method_nr(buf)))
	ಪ್ರತ್ಯಾಗಮ;
# endif

    // First make sure the swapfile is in a consistent state, using the old
    // key and method.
    {
	char_u *new_ಕೀಲಿ = buf->b_p_ಕೀಲಿ;
	char_u *new_buf_cm = buf->b_p_cm;

	buf->b_p_ಕೀಲಿ = old_ಕೀಲಿ;
	buf->b_p_cm = old_cm;
	ml_preserve(buf, FALSE);
	buf->b_p_ಕೀಲಿ = new_ಕೀಲಿ;
	buf->b_p_cm = new_buf_cm;
    }

    // Set the key, method and seed to be used for reading, these must be the
    // old values.
    mfp->mf_old_ಕೀಲಿ = old_ಕೀಲಿ;
    mfp->mf_old_cm = old_method;
    ಯದಿ (old_method > 0 && *old_ಕೀಲಿ != NUL)
	mch_memmove(mfp->mf_old_seed, mfp->mf_seed, MF_SEED_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    ಯದಿ (mfp->mf_infile_ಎಣಿಕೆ > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_ಸಾಲು(buf);		    // flush buffered line
	(ಶೂನ್ಯ)ml_find_ಸಾಲು(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = ರಿಕ್ತ;
	bnum = 1;		// start with block 1
	page_ಎಣಿಕೆ = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;
	VIM_CLEAR(buf->b_ml.ml_ರಾಶಿ);
	buf->b_ml.ml_ರಾಶಿ_size = 0;	// no stack yet

	ಪ್ರತಿ ( ; !got_int; ಸಾಲು_breakcheck())
	{
	    ಯದಿ (hp != ರಿಕ್ತ)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    ಯದಿ ((hp = mf_get(mfp, bnum, page_ಎಣಿಕೆ)) == ರಿಕ್ತ)
	    {
		ಯದಿ (bnum == 1)
		    ಭಙ್ಗ;
		++error;
	    }
	    ಅನ್ಯಥಾ
	    {
		pp = (PTR_BL *)(hp->bh_data);
		ಯದಿ (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    ಯದಿ (pp->pb_ಎಣಿಕೆ == 0)
		    {
			// empty block?
			++error;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (idx < (ಪೂರ್ಣಾಙ್ಕ)pp->pb_ಎಣಿಕೆ)	// go a block deeper
		    {
			ಯದಿ (pp->pb_pointer[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    ಅನುವರ್ತ;
			}

			// going one block deeper in the tree, new entry in
			//ರಾಶಿ
			ಯದಿ ((ತುದಿ = ml_add_ರಾಶಿ(buf)) < 0)
			{
			    ++error;
			    ಭಙ್ಗ;		    // out of memory
			}
			ip = &(buf->b_ml.ml_ರಾಶಿ[ತುದಿ]);
			ip->ip_bnum = bnum;
			ip->ip_index = idx;

			bnum = pp->pb_pointer[idx].pe_bnum;
			page_ಎಣಿಕೆ = pp->pb_pointer[idx].pe_page_ಎಣಿಕೆ;
			idx = 0;
			ಅನುವರ್ತ;
		    }
		}
		ಅನ್ಯಥಾ	    // not a pointer block
		{
		    dp = (DATA_BL *)(hp->bh_data);
		    ಯದಿ (dp->db_id != DATA_ID)	// block id wrong
			++error;
		    ಅನ್ಯಥಾ
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = ರಿಕ್ತ;
		    }
		}
	    }

	    ಯದಿ (buf->b_ml.ml_ರಾಶಿ_ತುದಿ == 0)	// finished
		ಭಙ್ಗ;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_ರಾಶಿ[--(buf->b_ml.ml_ರಾಶಿ_ತುದಿ)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_index + 1;	    // go to next index
	    page_ಎಣಿಕೆ = 1;
	}
	ಯದಿ (hp != ರಿಕ್ತ)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	ಯದಿ (error > 0)
	    emsg(_(e_error_while_updating_swap_file_crypt));
    }

    mfp->mf_old_ಕೀಲಿ = ರಿಕ್ತ;
}
#endif

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
    ಶೂನ್ಯ
ml_setname(buf_T *buf)
{
    ಪೂರ್ಣಾಙ್ಕ		success = FALSE;
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;
#ಯದಿ defined(MSWIN)
    char_u	*p;
#endif

    mfp = buf->b_ml.ml_mfp;
    ಯದಿ (mfp->mf_fd < 0)		    // there is no swap file yet
    {
	/*
	 * When 'updatecount' is 0 and 'noswapfile' there is no swap file.
	 * For help files we will make a swap file now.
	 */
	ಯದಿ (p_uc != 0 && (cmdmod.cmod_flags & CMOD_NOSWAPFILE) == 0)
	    ml_open_file(buf);	    // create a swap file
	ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * Try all directories in the 'directory' option.
     */
    dirp = p_dir;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (*dirp == NUL)	    // tried all directories, fail
	    ಭಙ್ಗ;
	fname = findswapname(buf, &dirp, mfp->mf_fname);
	// autocmd may have freed mfp if findswapname creates
	// different swapfile name
	ಯದಿ (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    ಪ್ರತ್ಯಾಗಮ;
	}
						    // alloc's fname
	ಯದಿ (dirp == ರಿಕ್ತ)	    // out of memory
	    ಭಙ್ಗ;
	ಯದಿ (fname == ರಿಕ್ತ)	    // no file name found for this dir
	    ಅನುವರ್ತ;

#ಯದಿ defined(MSWIN)
	/*
	 * Set full pathname for swap file now, because a ":!cd dir" may
	 * change directory without us knowing it.
	 */
	p = FullName_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	ಯದಿ (fname == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
#endif
	// if the file name is the same we don't have to do anything
	ಯದಿ (fnamecmp(fname, mfp->mf_fname) == 0)
	{
	    vim_free(fname);
	    success = TRUE;
	    ಭಙ್ಗ;
	}
	// need to close the swap file before renaming
	ಯದಿ (mfp->mf_fd >= 0)
	{
	    close(mfp->mf_fd);
	    mfp->mf_fd = -1;
	}

	// try to rename the swap file
	ಯದಿ (vim_rename(mfp->mf_fname, fname) == 0)
	{
	    success = TRUE;
	    vim_free(mfp->mf_fname);
	    mfp->mf_fname = fname;
	    vim_free(mfp->mf_ffname);
#ಯದಿ defined(MSWIN)
	    mfp->mf_ffname = ರಿಕ್ತ;  // mf_fname is full pathname already
#ಅನ್ಯಥಾ
	    mf_set_ffname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);
	    ಭಙ್ಗ;
	}
	vim_free(fname);	    // this fname didn't work, try another
    }

    ಯದಿ (mfp->mf_fd == -1)	    // need to (re)open the swap file
    {
	mfp->mf_fd = mch_open((ವರ್ಣ *)mfp->mf_fname, O_RDWR | O_EXTRA, 0);
	ಯದಿ (mfp->mf_fd < 0)
	{
	    // could not (re)open the swap file, what can we do????
	    emsg(_(e_oops_lost_the_swap_file));
	    ಪ್ರತ್ಯಾಗಮ;
	}
#ifdef HAVE_FD_CLOEXEC
	{
	    ಪೂರ್ಣಾಙ್ಕ fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    ಯದಿ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(ಶೂನ್ಯ)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#endif
    }
    ಯದಿ (!success)
	emsg(_(e_could_not_rename_swap_file));
}

/*
 * Open a file for the memfile for all buffers that are not readonly or have
 * been modified.
 * Used when 'updatecount' changes from zero to non-zero.
 */
    ಶೂನ್ಯ
ml_open_files(ಶೂನ್ಯ)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ಯದಿ (!buf->b_p_ro || buf->b_changed)
	    ml_open_file(buf);
}

/*
 * Open a swap file for an existing memfile, if there is no swap file yet.
 * If we are unable to find a file name, mf_fname will be NULL
 * and the memfile will be in memory only (no recovery possible).
 */
    ಶೂನ್ಯ
ml_open_file(buf_T *buf)
{
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;

    mfp = buf->b_ml.ml_mfp;
    ಯದಿ (mfp == ರಿಕ್ತ || mfp->mf_fd >= 0 || !buf->b_p_swf
				      || (cmdmod.cmod_flags & CMOD_NOSWAPFILE))
	ಪ್ರತ್ಯಾಗಮ;		// nothing to do

#ifdef FEAT_SPELL
    // For a spell buffer use a temp file name.
    ಯದಿ (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	ಯದಿ (fname != ರಿಕ್ತ)
	    (ಶೂನ್ಯ)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_swap = false;
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    /*
     * Try all directories in 'directory' option.
     */
    dirp = p_dir;
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (*dirp == NUL)
	    ಭಙ್ಗ;
	// There is a small chance that between choosing the swap file name
	// and creating it, another Vim creates the file.  In that case the
	// creation will fail and we will use another directory.
	fname = findswapname(buf, &dirp, ರಿಕ್ತ); // allocates fname
	// autocmd may have freed mfp, grr!
	ಯದಿ (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (dirp == ರಿಕ್ತ)
	    ಭಙ್ಗ;  // out of memory
	ಯದಿ (fname == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	ಯದಿ (mf_open_file(mfp, fname) == OK)	// consumes fname!
	{
	    // don't sync yet in ml_sync_all()
	    mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;
#ಯದಿ defined(MSWIN)
	    /*
	     * set full pathname for swap file now, because a ":!cd dir" may
	     * change directory without us knowing it.
	     */
	    mf_fullname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    ಯದಿ (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_set_dirty(mfp);
		ಭಙ್ಗ;
	    }
	    // Writing block 0 failed: close the file and try another dir
	    mf_close_file(buf, FALSE);
	}
    }

    ಯದಿ (*p_dir != NUL && mfp->mf_fname == ರಿಕ್ತ)
    {
	need_wait_return = TRUE;	// call wait_return() later
	++no_wait_return;
	(ಶೂನ್ಯ)semsg(_(e_unable_to_open_swap_file_for_str_recovery_impossible),
		    buf_spname(buf) != ರಿಕ್ತ ? buf_spname(buf) : buf->b_fname);
	--no_wait_return;
    }

    // don't try to open a swap file again
    buf->b_may_swap = false;
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
    ಶೂನ್ಯ
check_need_swap(
    ಪೂರ್ಣಾಙ್ಕ	    newfile)		// reading file into new buffer
{
    ಪೂರ್ಣಾಙ್ಕ old_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent; // might be reset by an E325 message

    ಯದಿ (curbuf->b_may_swap && (!curbuf->b_p_ro || !newfile))
	ml_open_file(curbuf);
    ಸಂದೇಶ_silent = old_ಸಂದೇಶ_silent;
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
    ಶೂನ್ಯ
ml_close(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ del_file)
{
    ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)		// not open
	ಪ್ರತ್ಯಾಗಮ;
    mf_close(buf->b_ml.ml_mfp, del_file);	// close the .swp file
    ಯದಿ (buf->b_ml.ml_ಸಾಲು_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_ಸಾಲು_ptr);
    vim_free(buf->b_ml.ml_ರಾಶಿ);
#ifdef FEAT_BYTEOFF
    VIM_CLEAR(buf->b_ml.ml_chunksize);
#endif
    buf->b_ml.ml_mfp = ರಿಕ್ತ;

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
    ಶೂನ್ಯ
ml_close_all(ಪೂರ್ಣಾಙ್ಕ del_file)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ml_close(buf, del_file && ((buf->b_flags & BF_PRESERVED) == 0
				 || vim_strchr(p_cpo, CPO_PRESERVE) == ರಿಕ್ತ));
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
    ಶೂನ್ಯ
ml_close_notmod(ಶೂನ್ಯ)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ಯದಿ (!bufIsChanged(buf))
	    ml_close(buf, TRUE);    // close all not-modified buffers
}

/*
 * Update the timestamp in the .swp file.
 * Used when the file has been written.
 */
    ಶೂನ್ಯ
ml_timestamp(buf_T *buf)
{
    ml_upd_block0(buf, UB_FNAME);
}

/*
 * Return FAIL when the ID of "b0p" is wrong.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ml_check_b0_id(ZERO_BL *b0p)
{
    ಯದಿ (b0p->b0_id[0] != BLOCK0_ID0
	    || (b0p->b0_id[1] != BLOCK0_ID1
		&& b0p->b0_id[1] != BLOCK0_ID1_C0
		&& b0p->b0_id[1] != BLOCK0_ID1_C1
		&& b0p->b0_id[1] != BLOCK0_ID1_C2
		&& b0p->b0_id[1] != BLOCK0_ID1_C3
		&& b0p->b0_id[1] != BLOCK0_ID1_C4)
	    )
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Update the timestamp or the B0_SAME_DIR flag of the .swp file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ml_upd_block0(buf_T *buf, upd_block0_T what)
{
    memfile_T	*mfp;
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    mfp = buf->b_ml.ml_mfp;
    ಯದಿ (mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    hp = mf_get(mfp, (blocknr_T)0, 1);
    ಯದಿ (hp == ರಿಕ್ತ)
    {
#ifdef FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	ಯದಿ (what == UB_CRYPT)
	    ml_set_mfp_crypt(buf);
#endif
	ಪ್ರತ್ಯಾಗಮ;
    }

    b0p = (ZERO_BL *)(hp->bh_data);
    ಯದಿ (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_block_zero);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (what == UB_FNAME)
	    set_b0_fname(b0p, buf);
#ifdef FEAT_CRYPT
	ಅನ್ಯಥಾ ಯದಿ (what == UB_CRYPT)
	    ml_set_b0_crypt(buf, b0p);
#endif
	ಅನ್ಯಥಾ // what == UB_SAME_DIR
	    set_b0_dir_flag(b0p, buf);
    }
    mf_put(mfp, hp, TRUE, FALSE);
}

/*
 * Write file name and timestamp into block 0 of a swap file.
 * Also set buf->b_mtime.
 * Don't use NameBuff[]!!!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_b0_fname(ZERO_BL *b0p, buf_T *buf)
{
    stat_T	st;

    ಯದಿ (buf->b_ffname == ರಿಕ್ತ)
	b0p->b0_fname[0] = NUL;
    ಅನ್ಯಥಾ
    {
#ಯದಿ defined(MSWIN) || defined(AMIGA)
	// Systems that cannot translate "~user" back into a path: copy the
	// file name unmodified.  Do use slashes instead of backslashes for
	// portability.
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_SIZE_CRYPT - 1);
# ifdef BACKSLASH_IN_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#ಅನ್ಯಥಾ
	size_t	flen;
	char_u	uname[B0_UNAME_SIZE];

	/*
	 * For a file under the home directory of the current user, we try to
	 * replace the home directory path with "~user". This helps when
	 * editing the same file on different machines over a network.
	 * First replace home dir path with "~/" with home_replace().
	 * Then insert the user name to get "~user/".
	 */
	flen = home_replace(ರಿಕ್ತ, buf->b_ffname, b0p->b0_fname,
						   B0_FNAME_SIZE_CRYPT, TRUE);
	ಯದಿ (b0p->b0_fname[0] == '~')
	{
	    size_t  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    ಯದಿ (get_user_name(uname, B0_UNAME_SIZE) == FAIL
		   || (ulen = STRLEN(uname)) + flen > B0_FNAME_SIZE_CRYPT - 1)
		vim_strncpy(b0p->b0_fname, buf->b_ffname,
						     B0_FNAME_SIZE_CRYPT - 1);
	    ಅನ್ಯಥಾ
	    {
		mch_memmove(b0p->b0_fname + ulen + 1, b0p->b0_fname + 1, flen);
		mch_memmove(b0p->b0_fname + 1, uname, ulen);
	    }
	}
#endif
	ಯದಿ (mch_stat((ವರ್ಣ *)buf->b_ffname, &st) >= 0)
	{
	    long_to_char((long)st.st_mtime, b0p->b0_mtime);
#ifdef CHECK_INODE
	    long_to_char((long)st.st_ino, b0p->b0_ino);
#endif
	    buf_store_time(buf, &st, buf->b_ffname);
	    buf->b_mtime_read = buf->b_mtime;
	    buf->b_mtime_read_ns = buf->b_mtime_ns;
	}
	ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
set_b0_dir_flag(ZERO_BL *b0p, buf_T *buf)
{
    ಯದಿ (same_directory(buf->b_ml.ml_mfp->mf_fname, buf->b_ffname))
	b0p->b0_flags |= B0_SAME_DIR;
    ಅನ್ಯಥಾ
	b0p->b0_flags &= ~B0_SAME_DIR;
}

/*
 * When there is room, add the 'fileencoding' to block zero.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_b0_fenc(
    ZERO_BL	*b0p,
    buf_T	*buf)
{
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		size = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    ಯದಿ (*buf->b_p_ಕೀಲಿ != NUL)
	size = B0_FNAME_SIZE_CRYPT;
#endif

    n = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buf->b_p_fenc);
    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)STRLEN(b0p->b0_fname) + n + 1 > size)
	b0p->b0_flags &= ~B0_HAS_FENC;
    ಅನ್ಯಥಾ
    {
	mch_memmove((ವರ್ಣ *)b0p->b0_fname + size - n,
					    (ವರ್ಣ *)buf->b_p_fenc, (size_t)n);
	*(b0p->b0_fname + size - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#ಯದಿ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#endif

#ಯದಿ defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
swapfile_process_running(ZERO_BL *b0p, char_u *swap_fname UNUSED)
{
# ಯದಿ defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    ಸಂರಚನಾ sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    ಯದಿ (mch_stat((ವರ್ಣ *)swap_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < time(ರಿಕ್ತ) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	ಪ್ರತ್ಯಾಗಮ FALSE;
# endif
    ಪ್ರತ್ಯಾಗಮ mch_process_running(char_to_long(b0p->b0_pid));
}
#endif

/*
 * Try to recover curbuf from the .swp file.
 * If "checkext" is TRUE, check the extension and detect whether it is
 * a swap file.
 */
    ಶೂನ್ಯ
ml_recover(ಪೂರ್ಣಾಙ್ಕ checkext)
{
    buf_T	*buf = ರಿಕ್ತ;
    memfile_T	*mfp = ರಿಕ್ತ;
    char_u	*fname;
    char_u	*fname_used = ರಿಕ್ತ;
    bhdr_T	*hp = ರಿಕ್ತ;
    ZERO_BL	*b0p;
    ಪೂರ್ಣಾಙ್ಕ		b0_ff;
    char_u	*b0_fenc = ರಿಕ್ತ;
#ifdef FEAT_CRYPT
    ಪೂರ್ಣಾಙ್ಕ		b0_cm = -1;
#endif
    PTR_BL	*pp;
    DATA_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    ಪೂರ್ಣಾಙ್ಕ		page_ಎಣಿಕೆ;
    stat_T	org_stat, swp_stat;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		directly;
    linenr_T	lnum;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
    long	error;
    ಪೂರ್ಣಾಙ್ಕ		cannot_open;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		has_error;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		ತುದಿ;
    ಪೂರ್ಣಾಙ್ಕ		txt_start;
    off_T	size;
    ಪೂರ್ಣಾಙ್ಕ		called_from_main;
    ಪೂರ್ಣಾಙ್ಕ		serious_error = TRUE;
    long	mtime;
    ಪೂರ್ಣಾಙ್ಕ		attr;
    ಪೂರ್ಣಾಙ್ಕ		orig_file_status = NOTDONE;

    recoverymode = TRUE;
    called_from_main = (curbuf->b_ml.ml_mfp == ರಿಕ್ತ);
    attr = HL_ATTR(HLF_E);

    /*
     * If the file name ends in ".s[a-w][a-z]" we assume this is the swap file.
     * Otherwise a search is done to find the swap file(s).
     */
    fname = curbuf->b_fname;
    ಯದಿ (fname == ರಿಕ್ತ)		    // When there is no file name
	fname = (char_u *)"";
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(fname);
    ಯದಿ (checkext && len >= 4 &&
#ಯದಿ defined(VMS)
	    STRNICMP(fname + len - 4, "_s", 2)
#ಅನ್ಯಥಾ
	    STRNICMP(fname + len - 4, ".s", 2)
#endif
						== 0
		&& vim_strchr((char_u *)"abcdefghijklmnopqrstuvw",
					   TOLOWER_ASC(fname[len - 2])) != ರಿಕ್ತ
		&& ASCII_ISALPHA(fname[len - 1]))
    {
	directly = TRUE;
	fname_used = vim_strsave(fname); // make a copy for mf_open()
    }
    ಅನ್ಯಥಾ
    {
	directly = FALSE;

	// count the number of matching swap files
	len = recover_names(fname, FALSE, ರಿಕ್ತ, 0, ರಿಕ್ತ);
	ಯದಿ (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_swap_file_found_for_str), fname);
	    ಗಚ್ಛ theend;
	}
	ಯದಿ (len == 1)		    // one swap file found, use it
	    i = 1;
	ಅನ್ಯಥಾ			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (ಶೂನ್ಯ)recover_names(fname, TRUE, ರಿಕ್ತ, 0, ರಿಕ್ತ);
	    ಸಂದೇಶ_putchar('\n');
	    ಸಂದೇಶ_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_ಸಂಖ್ಯೆ(FALSE, ರಿಕ್ತ);
	    ಯದಿ (i < 1 || i > len)
		ಗಚ್ಛ theend;
	}
	// get the swap file name that will be used
	(ಶೂನ್ಯ)recover_names(fname, FALSE, ರಿಕ್ತ, i, &fname_used);
    }
    ಯದಿ (fname_used == ರಿಕ್ತ)
	ಗಚ್ಛ theend;			// out of memory

    // When called from main() still need to initialize storage structure
    ಯದಿ (called_from_main && ml_open(curbuf) == FAIL)
	getout(1);

    /*
     * Allocate a buffer structure for the swap file that is used for recovery.
     * Only the memline and crypt information in it are really used.
     */
    buf = ALLOC_ONE(buf_T);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ರಾಶಿ_size = 0;	// no stack yet
    buf->b_ml.ml_ರಾಶಿ = ರಿಕ್ತ;		// no stack yet
    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;		// nothing in the stack
    buf->b_ml.ml_ಸಾಲು_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = ರಿಕ್ತ;		// no locked block
    buf->b_ml.ml_flags = 0;
#ifdef FEAT_CRYPT
    buf->b_p_ಕೀಲಿ = empty_option;
    buf->b_p_cm = empty_option;
#endif

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_used); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_used, O_RDONLY);
    fname_used = p;
    ಯದಿ (mfp == ರಿಕ್ತ || mfp->mf_fd < 0)
    {
	ಯದಿ (fname_used != ರಿಕ್ತ)
	    semsg(_(e_cannot_open_str), fname_used);
	ಗಚ್ಛ theend;
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
    ಯದಿ ((hp = mf_get(mfp, (blocknr_T)0, 1)) == ರಿಕ್ತ)
    {
	ಸಂದೇಶ_start();
	ಸಂದೇಶ_puts_attr(_("Unable to read block 0 from "), attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_outtrans_attr(mfp->mf_fname, attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_end();
	ಗಚ್ಛ theend;
    }
    b0p = (ZERO_BL *)(hp->bh_data);
    ಯದಿ (STRNCMP(b0p->b0_version, "VIM 3.0", 7) == 0)
    {
	ಸಂದೇಶ_start();
	ಸಂದೇಶ_outtrans_attr(mfp->mf_fname, ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_puts_attr(_("Use Vim version 3.0.\n"), ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_end();
	ಗಚ್ಛ theend;
    }
    ಯದಿ (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_swap_file), mfp->mf_fname);
	ಗಚ್ಛ theend;
    }
    ಯದಿ (b0_magic_wrong(b0p))
    {
	ಸಂದೇಶ_start();
	ಸಂದೇಶ_outtrans_attr(mfp->mf_fname, attr | ಸಂದೇಶ_HIST);
#ಯದಿ defined(MSWIN)
	ಯದಿ (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    ಸಂದೇಶ_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | ಸಂದೇಶ_HIST);
	ಅನ್ಯಥಾ
#endif
	    ಸಂದೇಶ_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_puts_attr(_("The file was created on "), attr | ಸಂದೇಶ_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	ಸಂದೇಶ_puts_attr((ವರ್ಣ *)b0p->b0_hname, attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_puts_attr(_(",\nor the file has been damaged."), attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_end();
	ಗಚ್ಛ theend;
    }

#ifdef FEAT_CRYPT
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(id1_codes); ++i)
	ಯದಿ (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    ಯದಿ (b0_cm > 0)
	mch_memmove(mfp->mf_seed, &b0p->b0_seed, MF_SEED_LEN);
    crypt_set_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#ಅನ್ಯಥಾ
    ಯದಿ (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_version_of_vim_does_not_support_encryption), mfp->mf_fname);
	ಗಚ್ಛ theend;
    }
#endif

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    ಯದಿ (mfp->mf_page_size != (unsigned)char_to_long(b0p->b0_page_size))
    {
	unsigned previous_page_size = mfp->mf_page_size;

	mf_new_page_size(mfp, (unsigned)char_to_long(b0p->b0_page_size));
	ಯದಿ (mfp->mf_page_size < previous_page_size)
	{
	    ಸಂದೇಶ_start();
	    ಸಂದೇಶ_outtrans_attr(mfp->mf_fname, attr | ಸಂದೇಶ_HIST);
	    ಸಂದೇಶ_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | ಸಂದೇಶ_HIST);
	    ಸಂದೇಶ_end();
	    ಗಚ್ಛ theend;
	}
	ಯದಿ ((size = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	ಅನ್ಯಥಾ
	    mfp->mf_blocknr_max = (blocknr_T)(size / mfp->mf_page_size);
	mfp->mf_infile_ಎಣಿಕೆ = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_page_size);
	ಯದಿ (p == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	mch_memmove(p, hp->bh_data, previous_page_size);
	vim_free(hp->bh_data);
	hp->bh_data = p;
	b0p = (ZERO_BL *)(hp->bh_data);
    }

    /*
     * If .swp file name given directly, use name from swap file for buffer.
     */
    ಯದಿ (directly)
    {
	expand_env(b0p->b0_fname, NameBuff, MAXPATHL);
	ಯದಿ (setfname(curbuf, NameBuff, ರಿಕ್ತ, TRUE) == FAIL)
	    ಗಚ್ಛ theend;
    }

    home_replace(ರಿಕ್ತ, mfp->mf_fname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Using swap file \"%s\""), NameBuff);

    ಯದಿ (buf_spname(curbuf) != ರಿಕ್ತ)
	vim_strncpy(NameBuff, buf_spname(curbuf), MAXPATHL - 1);
    ಅನ್ಯಥಾ
	home_replace(ರಿಕ್ತ, curbuf->b_ffname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Original file \"%s\""), NameBuff);
    ಸಂದೇಶ_putchar('\n');

    /*
     * check date of swap file and original file
     */
    mtime = char_to_long(b0p->b0_mtime);
    ಯದಿ (curbuf->b_ffname != ರಿಕ್ತ
	    && mch_stat((ವರ್ಣ *)curbuf->b_ffname, &org_stat) != -1
	    && ((mch_stat((ವರ್ಣ *)mfp->mf_fname, &swp_stat) != -1
		    && org_stat.st_mtime > swp_stat.st_mtime)
		|| org_stat.st_mtime != mtime))
	emsg(_(e_warning_original_file_may_have_been_changed));
    out_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_MASK);
    ಯದಿ (b0p->b0_flags & B0_HAS_FENC)
    {
	ಪೂರ್ಣಾಙ್ಕ fnsize = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	ಯದಿ (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_SIZE_CRYPT;
#endif
	ಪ್ರತಿ (p = b0p->b0_fname + fnsize; p > b0p->b0_fname && p[-1] != NUL; --p)
	    ;
	b0_fenc = vim_strnsave(p, b0p->b0_fname + fnsize - p);
    }

    mf_put(mfp, hp, FALSE, FALSE);	// release block 0
    hp = ರಿಕ್ತ;

    /*
     * Now that we are sure that the file is going to be recovered, clear the
     * contents of the current buffer.
     */
    ಯಾವತ್ (!(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    ಯದಿ (curbuf->b_ffname != ರಿಕ್ತ)
	orig_file_status = readfile(curbuf->b_ffname, ರಿಕ್ತ, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, ರಿಕ್ತ, READ_NEW);

#ifdef FEAT_CRYPT
    ಯದಿ (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	ಯದಿ (*curbuf->b_p_ಕೀಲಿ != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_used);
	    ಸಂದೇಶ_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    ಸಂದೇಶ_puts(_("\nenter the new crypt key."));
	    ಸಂದೇಶ_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    ಸಂದೇಶ_puts(_("\nto use the same key for text file and swap file"));
	}
	ಅನ್ಯಥಾ
	    smsg(_(need_ಕೀಲಿ_ಸಂದೇಶ), fname_used);
	buf->b_p_ಕೀಲಿ = crypt_get_ಕೀಲಿ(FALSE, FALSE);
	ಯದಿ (buf->b_p_ಕೀಲಿ == ರಿಕ್ತ)
	    buf->b_p_ಕೀಲಿ = curbuf->b_p_ಕೀಲಿ;
	ಅನ್ಯಥಾ ಯದಿ (*buf->b_p_ಕೀಲಿ == NUL)
	{
	    vim_free(buf->b_p_ಕೀಲಿ);
	    buf->b_p_ಕೀಲಿ = curbuf->b_p_ಕೀಲಿ;
	}
	ಯದಿ (buf->b_p_ಕೀಲಿ == ರಿಕ್ತ)
	    buf->b_p_ಕೀಲಿ = empty_option;
    }
#endif

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    ಯದಿ (b0_ff != 0)
	set_fileformat(b0_ff - 1, OPT_LOCAL);
    ಯದಿ (b0_fenc != ರಿಕ್ತ)
    {
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    page_ಎಣಿಕೆ = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    ಸಾಲು_ಎಣಿಕೆ = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;
    buf->b_ml.ml_ರಾಶಿ = ರಿಕ್ತ;
    buf->b_ml.ml_ರಾಶಿ_size = 0;	// no stack yet

    ಯದಿ (curbuf->b_ffname == ರಿಕ್ತ)
	cannot_open = TRUE;
    ಅನ್ಯಥಾ
	cannot_open = FALSE;

    serious_error = FALSE;
    ಪ್ರತಿ ( ; !got_int; ಸಾಲು_breakcheck())
    {
	ಯದಿ (hp != ರಿಕ್ತ)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	ಯದಿ ((hp = mf_get(mfp, bnum, page_ಎಣಿಕೆ)) == ರಿಕ್ತ)
	{
	    ಯದಿ (bnum == 1)
	    {
		semsg(_(e_unable_to_read_block_one_from_str), mfp->mf_fname);
		ಗಚ್ಛ theend;
	    }
	    ++error;
	    ml_append(lnum++, (char_u *)_("???MANY LINES MISSING"),
							    (colnr_T)0, TRUE);
	}
	ಅನ್ಯಥಾ		// there is a block
	{
	    pp = (PTR_BL *)(hp->bh_data);
	    ಯದಿ (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		ಪೂರ್ಣಾಙ್ಕ ptr_block_error = FALSE;
		ಯದಿ (pp->pb_ಎಣಿಕೆ_max != PB_ಎಣಿಕೆ_MAX(mfp))
		{
		    ptr_block_error = TRUE;
		    pp->pb_ಎಣಿಕೆ_max = PB_ಎಣಿಕೆ_MAX(mfp);
		}
		ಯದಿ (pp->pb_ಎಣಿಕೆ > pp->pb_ಎಣಿಕೆ_max)
		{
		    ptr_block_error = TRUE;
		    pp->pb_ಎಣಿಕೆ = pp->pb_ಎಣಿಕೆ_max;
		}
		ಯದಿ (ptr_block_error)
		    emsg(_(e_warning_pointer_block_corrupted));

		// check line count when using pointer block first time
		ಯದಿ (idx == 0 && ಸಾಲು_ಎಣಿಕೆ != 0)
		{
		    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)pp->pb_ಎಣಿಕೆ; ++i)
			ಸಾಲು_ಎಣಿಕೆ -= pp->pb_pointer[i].pe_ಸಾಲು_ಎಣಿಕೆ;
		    ಯದಿ (ಸಾಲು_ಎಣಿಕೆ != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		ಯದಿ (pp->pb_ಎಣಿಕೆ == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		ಅನ್ಯಥಾ ಯದಿ (idx < (ಪೂರ್ಣಾಙ್ಕ)pp->pb_ಎಣಿಕೆ)	// go a block deeper
		{
		    ಯದಿ (pp->pb_pointer[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			ಯದಿ (!cannot_open)
			{
			    ಸಾಲು_ಎಣಿಕೆ = pp->pb_pointer[idx].pe_ಸಾಲು_ಎಣಿಕೆ;
			    linenr_T pe_old_lnum = pp->pb_pointer[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    ಯದಿ (ಸಾಲು_ಎಣಿಕೆ <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, ರಿಕ್ತ, lnum,
					pe_old_lnum - 1,
					ಸಾಲು_ಎಣಿಕೆ, ರಿಕ್ತ, 0) != OK)
				cannot_open = TRUE;
			    ಅನ್ಯಥಾ
				lnum += ಸಾಲು_ಎಣಿಕೆ;
			}
			ಯದಿ (cannot_open)
			{
			    ++error;
			    ml_append(lnum++, (char_u *)_("???LINES MISSING"),
							    (colnr_T)0, TRUE);
			}
			++idx;	    // get same block again for next index
			ಅನುವರ್ತ;
		    }

		    /*
		     * going one block deeper in the tree
		     */
		    ಯದಿ ((ತುದಿ = ml_add_ರಾಶಿ(buf)) < 0)	// new entry in stack
		    {
			++error;
			ಭಙ್ಗ;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_ರಾಶಿ[ತುದಿ]);
		    ip->ip_bnum = bnum;
		    ip->ip_index = idx;

		    bnum = pp->pb_pointer[idx].pe_bnum;
		    ಸಾಲು_ಎಣಿಕೆ = pp->pb_pointer[idx].pe_ಸಾಲು_ಎಣಿಕೆ;
		    page_ಎಣಿಕೆ = pp->pb_pointer[idx].pe_page_ಎಣಿಕೆ;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    ಯದಿ (page_ಎಣಿಕೆ < 1
			    || bnum + page_ಎಣಿಕೆ > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_index + 1;
			bnum = ip->ip_bnum;
			page_ಎಣಿಕೆ = 1;
			--buf->b_ml.ml_ರಾಶಿ_ತುದಿ;
			ಅನುವರ್ತ;
		    }
		    idx = 0;
		    ಅನುವರ್ತ;
		}
	    }
	    ಅನ್ಯಥಾ	    // not a pointer block
	    {
		dp = (DATA_BL *)(hp->bh_data);
		ಯದಿ (dp->db_id != DATA_ID)	// block id wrong
		{
		    ಯದಿ (bnum == 1)
		    {
			semsg(_(e_block_one_id_wrong_str_not_swp_file),
							       mfp->mf_fname);
			ಗಚ್ಛ theend;
		    }
		    ++error;
		    ml_append(lnum++, (char_u *)_("???BLOCK MISSING"),
							    (colnr_T)0, TRUE);
		}
		ಅನ್ಯಥಾ
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
		    ಯದಿ (hp->bh_page_ಎಣಿಕೆ != page_ಎಣಿಕೆ)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			page_ಎಣಿಕೆ = hp->bh_page_ಎಣಿಕೆ;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    ಯದಿ (page_ಎಣಿಕೆ * mfp->mf_page_size != dp->db_txt_end)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_end = page_ಎಣಿಕೆ * mfp->mf_page_size;
		    }

		    ಯದಿ (dp->db_txt_start < HEADER_SIZE
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
		    ಯದಿ (ಸಾಲು_ಎಣಿಕೆ != dp->db_ಸಾಲು_ಎಣಿಕೆ)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    ಪೂರ್ಣಾಙ್ಕ did_questions = FALSE;
		    ಪ್ರತಿ (i = 0; i < dp->db_ಸಾಲು_ಎಣಿಕೆ; ++i)
		    {
			ಯದಿ ((char_u *)&(dp->db_index[i])
					    >= (char_u *)dp + dp->db_txt_start)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    ಭಙ್ಗ;
			}

			txt_start = (dp->db_index[i] & DB_INDEX_MASK);
			ಯದಿ (txt_start <= (ಪೂರ್ಣಾಙ್ಕ)HEADER_SIZE
					  || txt_start >= (ಪೂರ್ಣಾಙ್ಕ)dp->db_txt_end)
			{
			    ++error;
			    // avoid lots of lines with "???"
			    ಯದಿ (did_questions)
				ಅನುವರ್ತ;
			    did_questions = TRUE;
			    p = (char_u *)"???";
			}
			ಅನ್ಯಥಾ
			{
			    did_questions = FALSE;
			    p = (char_u *)dp + txt_start;
			}
			ml_append(lnum++, p, (colnr_T)0, TRUE);
		    }
		    ಯದಿ (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	ಯದಿ (buf->b_ml.ml_ರಾಶಿ_ತುದಿ == 0)	// finished
	    ಭಙ್ಗ;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_ರಾಶಿ[--(buf->b_ml.ml_ರಾಶಿ_ತುದಿ)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_index + 1;	    // go to next index
	page_ಎಣಿಕೆ = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    ಯದಿ (orig_file_status != OK || curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	ಯದಿ (!(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 2 && *ml_get(1) == NUL))
	{
	    changed_internal();
	    ++CHANGEDTICK(curbuf);
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (idx = 1; idx <= lnum; ++idx)
	{
	    // Need to copy one line, fetching the other one may flush it.
	    p = vim_strnsave(ml_get(idx), ml_get_len(idx));
	    i = STRCMP(p, ml_get(idx + lnum));
	    vim_free(p);
	    ಯದಿ (i != 0)
	    {
		changed_internal();
		++CHANGEDTICK(curbuf);
		ಭಙ್ಗ;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    ಯಾವತ್ (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ > lnum
				       && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    ಯದಿ (got_int)
	emsg(_(e_recovery_interrupted));
    ಅನ್ಯಥಾ ಯದಿ (error)
    {
	++no_wait_return;
	ಸಂದೇಶ(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_ಸಾಲುಗಳು_starting_with_questions));
	--no_wait_return;
	ಸಂದೇಶ(_("See \":help E312\" for more information."));
	ಸಂದೇಶ(">>>>>>>>>>>>>");
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (curbuf->b_changed)
	{
	    ಸಂದೇಶ(_("Recovery completed. You should check if everything is OK."));
	    ಸಂದೇಶ_puts(_("\n(You might want to write out this file under another name\n"));
	    ಸಂದೇಶ_puts(_("and run diff with the original file to check for changes)"));
	}
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ(_("Recovery completed. Buffer contents equals file contents."));
	ಸಂದೇಶ_puts(_("\nYou may want to delete the .swp file now."));
#ಯದಿ defined(UNIX) || defined(MSWIN)
	ಯದಿ (swapfile_process_running(b0p, fname_used))
	{
	    // Warn there could be an active Vim on the same file, the user may
	    // want to kill it.
	    ಸಂದೇಶ_puts(_("\nNote: process STILL RUNNING: "));
	    ಸಂದೇಶ_outnum(char_to_long(b0p->b0_pid));
	}
#endif
	ಸಂದೇಶ_puts("\n\n");
	cmdline_row = ಸಂದೇಶ_row;
    }
#ifdef FEAT_CRYPT
    ಯದಿ (*buf->b_p_ಕೀಲಿ != NUL && STRCMP(curbuf->b_p_ಕೀಲಿ, buf->b_p_ಕೀಲಿ) != 0)
    {
	ಸಂದೇಶ_puts(_("Using crypt key from swap file for the text file.\n"));
	set_option_ಮೌಲ್ಯ_give_err((char_u *)"key", 0L, buf->b_p_ಕೀಲಿ, OPT_LOCAL);
    }
#endif
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_used);
    recoverymode = FALSE;
    ಯದಿ (mfp != ರಿಕ್ತ)
    {
	ಯದಿ (hp != ರಿಕ್ತ)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    ಯದಿ (buf != ರಿಕ್ತ)
    {
#ifdef FEAT_CRYPT
	ಯದಿ (buf->b_p_ಕೀಲಿ != curbuf->b_p_ಕೀಲಿ)
	    free_string_option(buf->b_p_ಕೀಲಿ);
	free_string_option(buf->b_p_cm);
#endif
	vim_free(buf->b_ml.ml_ರಾಶಿ);
	vim_free(buf);
    }
    ಯದಿ (serious_error && called_from_main)
	ml_close(curbuf, TRUE);
    ಅನ್ಯಥಾ
    {
	apply_autocmds(EVENT_BUFREADPOST, ರಿಕ್ತ, curbuf->b_fname, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ರಿಕ್ತ, curbuf->b_fname, FALSE, curbuf);
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
    ಪೂರ್ಣಾಙ್ಕ
recover_names(
    char_u	*fname,		// base for swap file name
    ಪೂರ್ಣಾಙ್ಕ		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list UNUSED, // when not NULL add file names to it
    ಪೂರ್ಣಾಙ್ಕ		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_out)	// result when "nr" > 0
{
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_names;
    char_u	*(names[6]);
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_files;
    ಪೂರ್ಣಾಙ್ಕ		file_ಎಣಿಕೆ = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = ರಿಕ್ತ;
#ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#endif

    ಯದಿ (fname != ರಿಕ್ತ)
    {
#ifdef HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	ಯದಿ (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	ಅನ್ಯಥಾ
#endif
	    fname_res = fname;
    }

    ಯದಿ (do_list)
    {
	// use msg() to start the scrolling properly
	ಸಂದೇಶ(_("Swap files found:"));
	ಸಂದೇಶ_putchar('\n');
    }

    /*
     * Do the loop for every directory in 'directory'.
     * First allocate some memory to put the directory name in.
     */
    dir_name.string = alloc(STRLEN(p_dir) + 1);
    dirp = p_dir;
    ಯಾವತ್ (dir_name.string != ರಿಕ್ತ && *dirp)
    {
	/*
	 * Isolate a directory name from *dirp and put it in dir_name (we know
	 * it is large enough, so use 31000 for length).
	 * Advance dirp to next directory name.
	 */
	dir_name.length = (size_t)copy_option_part(&dirp, dir_name.string, 31000, ",");

	ಯದಿ (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    ಯದಿ (fname == ರಿಕ್ತ)
	    {
#ifdef VMS
		names[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#ಅನ್ಯಥಾ
		names[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#endif
#ಯದಿ defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		names[1] = vim_strnsave((char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"));
		names[2] = vim_strnsave((char_u *)".sw?", STRLEN_LITERAL(".sw?"));
		ಅಂಕೆ_names = 3;
#ಅನ್ಯಥಾ
# ifdef VMS
		names[1] = vim_strnsave((char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"));
		ಅಂಕೆ_names = 2;
# ಅನ್ಯಥಾ
		ಅಂಕೆ_names = 1;
# endif
#endif
	    }
	    ಅನ್ಯಥಾ
		ಅಂಕೆ_names = recov_file_names(names, fname_res, TRUE);
	}
	ಅನ್ಯಥಾ			    // check directory dir_name
	{
	    ಯದಿ (fname == ರಿಕ್ತ)
	    {
		string_T    ret;

#ifdef VMS
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#ಅನ್ಯಥಾ
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#endif
#ಯದಿ defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		names[1] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"), TRUE, &ret);
		names[2] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".sw?", STRLEN_LITERAL(".sw?"), TRUE, &ret);
		ಅಂಕೆ_names = 3;
#ಅನ್ಯಥಾ
# ifdef VMS
		names[1] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"), TRUE, &ret);
		ಅಂಕೆ_names = 2;
# ಅನ್ಯಥಾ
		ಅಂಕೆ_names = 1;
# endif
#endif
	    }
	    ಅನ್ಯಥಾ
	    {
		char_u	*tail;

#ಯದಿ defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.length;
		ಯದಿ (after_pathsep(dir_name.string, p) && dir_name.length > 1 && p[-1] == p[-2])
		{
		    // Ends with '//', Use Full path for swap name
		    tail = make_percent_swname(dir_name.string, p, fname_res);
		}
		ಅನ್ಯಥಾ
#endif
		{
		    string_T	ret;

		    tail = gettail(fname_res);
		    tail = concat_fnames(dir_name.string, dir_name.length,
			tail, STRLEN(tail), TRUE, &ret);
		}
		ಯದಿ (tail == ರಿಕ್ತ)
		    ಅಂಕೆ_names = 0;
		ಅನ್ಯಥಾ
		{
		    ಅಂಕೆ_names = recov_file_names(names, tail, FALSE);
		    vim_free(tail);
		}
	    }
	}

	// check for out-of-memory
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_names; ++i)
	{
	    ಯದಿ (names[i] == ರಿಕ್ತ)
	    {
		ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_names; ++i)
		    vim_free(names[i]);
		ಅಂಕೆ_names = 0;
	    }
	}
	ಯದಿ (ಅಂಕೆ_names == 0)
	    ಅಂಕೆ_files = 0;
	ಅನ್ಯಥಾ ಯದಿ (expand_wildcards(ಅಂಕೆ_names, names, &ಅಂಕೆ_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    ಅಂಕೆ_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	ಯದಿ (*dirp == NUL && file_ಎಣಿಕೆ + ಅಂಕೆ_files == 0 && fname != ರಿಕ್ತ)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#ಯದಿ defined(VMS)
			       (char_u *)"_swp", FALSE
#ಅನ್ಯಥಾ
			       (char_u *)".swp", TRUE
#endif
			      );
	    ಯದಿ (swapname != ರಿಕ್ತ)
	    {
		ಯದಿ (mch_stat((ವರ್ಣ *)swapname, &st) != -1)    // It exists!
		{
		    files = ALLOC_ONE(char_u *);
		    ಯದಿ (files != ರಿಕ್ತ)
		    {
			files[0] = swapname;
			swapname = ರಿಕ್ತ;
			ಅಂಕೆ_files = 1;
		    }
		}
		vim_free(swapname);
	    }
	}

	/*
	 * Remove swapfile name of the current buffer, it must be ignored.
	 * But keep it for swapfilelist().
	 */
	ಯದಿ (curbuf->b_ml.ml_mfp != ರಿಕ್ತ
			       && (p = curbuf->b_ml.ml_mfp->mf_fname) != ರಿಕ್ತ
			       && ret_list == ರಿಕ್ತ)
	{
	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_files; ++i)
		// Do not expand wildcards, on windows would try to expand
		// "%tmp%" in "%tmp%file".
		ಯದಿ (fullpathcmp(p, files[i], TRUE, FALSE) & FPC_SAME)
		{
		    // Remove the name from files[i].  Move further entries
		    // down.  When the array becomes empty free it here, since
		    // FreeWild() won't be called below.
		    vim_free(files[i]);
		    ಯದಿ (--ಅಂಕೆ_files == 0)
			vim_free(files);
		    ಅನ್ಯಥಾ
			ಪ್ರತಿ ( ; i < ಅಂಕೆ_files; ++i)
			    files[i] = files[i + 1];
		}
	}
	ಯದಿ (nr > 0)
	{
	    file_ಎಣಿಕೆ += ಅಂಕೆ_files;
	    ಯದಿ (nr <= file_ಎಣಿಕೆ)
	    {
		*fname_out = vim_strsave(
				      files[nr - 1 + ಅಂಕೆ_files - file_ಎಣಿಕೆ]);
		dirp = (char_u *)"";		    // stop searching
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (do_list)
	{
	    ಯದಿ (dir_name.string[0] == '.' && dir_name.string[1] == NUL)
	    {
		ಯದಿ (fname == ರಿಕ್ತ)
		    ಸಂದೇಶ_puts(_("   In current directory:\n"));
		ಅನ್ಯಥಾ
		    ಸಂದೇಶ_puts(_("   Using specified name:\n"));
	    }
	    ಅನ್ಯಥಾ
	    {
		ಸಂದೇಶ_puts(_("   In directory "));
		ಸಂದೇಶ_home_replace(dir_name.string);
		ಸಂದೇಶ_puts(":\n");
	    }

	    ಯದಿ (ಅಂಕೆ_files)
	    {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_files; ++i)
		{
		    // print the swap file name
		    ಸಂದೇಶ_outnum((long)++file_ಎಣಿಕೆ);
		    ಸಂದೇಶ_puts(".    ");
		    ಸಂದೇಶ_puts((ವರ್ಣ *)gettail(files[i]));
		    ಸಂದೇಶ_putchar('\n');
		    (ಶೂನ್ಯ)swapfile_info(files[i]);
		}
	    }
	    ಅನ್ಯಥಾ
		ಸಂದೇಶ_puts(_("      -- none --\n"));
	    out_flush();
	}
#ifdef FEAT_EVAL
	ಅನ್ಯಥಾ ಯದಿ (ret_list != ರಿಕ್ತ)
	{
	    string_T	name;

	    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.length,
		    files[i], STRLEN(files[i]), TRUE, &name);
		ಯದಿ (name.string != ರಿಕ್ತ)
		{
		    list_append_string(ret_list, name.string, (ಪೂರ್ಣಾಙ್ಕ)name.length);
		    vim_free(name.string);
		}
	    }
	}
#endif
	ಅನ್ಯಥಾ
	    file_ಎಣಿಕೆ += ಅಂಕೆ_files;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_names; ++i)
	    vim_free(names[i]);
	ಯದಿ (ಅಂಕೆ_files > 0)
	    FreeWild(ಅಂಕೆ_files, files);
    }
    vim_free(dir_name.string);
    ಪ್ರತ್ಯಾಗಮ file_ಎಣಿಕೆ;
}

#ಯದಿ defined(UNIX) || defined(MSWIN)
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
    string_T	d = {ರಿಕ್ತ, 0};
    string_T	fixed_fname;
    char_u	*p;

    fixed_fname.string = fix_fname(name != ರಿಕ್ತ ? name : (char_u *)"");
    ಯದಿ (fixed_fname.string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (p = fixed_fname.string; *p != NUL; MB_PTR_ADV(p))
	ಯದಿ (vim_ispathsep(*p))
	    *p = '%';
    fixed_fname.length = (size_t)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_end[-1];
    *p = NUL;
    concat_fnames(dir, (size_t)(p - dir), fixed_fname.string, fixed_fname.length, TRUE, &d);
    vim_free(fixed_fname.string);

    ಪ್ರತ್ಯಾಗಮ d.string;
}
#endif

#ಯದಿ (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ process_still_running;
#endif

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return information found in swapfile "fname" in dictionary "d".
 * This is used by the swapinfo() function.
 */
    ಶೂನ್ಯ
get_b0_dict(char_u *fname, dict_T *d)
{
    ಪೂರ್ಣಾಙ್ಕ fd;
    ಸಂರಚನಾ block0 b0;

    ಯದಿ ((fd = mch_open((ವರ್ಣ *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
    {
	ಯದಿ (read_eintr(fd, &b0, ಪರಿಮಾಣ(b0)) == ಪರಿಮಾಣ(b0))
	{
	    ಯದಿ (ml_check_b0_id(&b0) == FAIL)
		dict_add_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    ಅನ್ಯಥಾ ಯದಿ (b0_magic_wrong(&b0))
		dict_add_string_len(d, "error",
		    (char_u *)"Magic number mismatch", STRLEN_LITERAL("Magic number mismatch"));
	    ಅನ್ಯಥಾ
	    {
		// we have swap information
		dict_add_string_len(d, "version", b0.b0_version, 10);
		dict_add_string_len(d, "user", b0.b0_uname, B0_UNAME_SIZE);
		dict_add_string_len(d, "host", b0.b0_hname, B0_HNAME_SIZE);
		dict_add_string_len(d, "fname", b0.b0_fname, B0_FNAME_SIZE_ORG);

		dict_add_ಸಂಖ್ಯೆ(d, "pid", char_to_long(b0.b0_pid));
		dict_add_ಸಂಖ್ಯೆ(d, "mtime", char_to_long(b0.b0_mtime));
		dict_add_ಸಂಖ್ಯೆ(d, "dirty", b0.b0_dirty ? 1 : 0);
# ifdef CHECK_INODE
		dict_add_ಸಂಖ್ಯೆ(d, "inode", char_to_long(b0.b0_ino));
# endif
	    }
	}
	ಅನ್ಯಥಾ
	    dict_add_string_len(d, "error", (char_u *)"Cannot read file", STRLEN_LITERAL("Cannot read file"));
	close(fd);
    }
    ಅನ್ಯಥಾ
	dict_add_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#endif

/*
 * Give information about an existing swap file.
 * Returns timestamp (0 when unknown).
 */
    ಸ್ಥಿರ time_t
swapfile_info(char_u *fname)
{
    stat_T	    st;
    ಪೂರ್ಣಾಙ್ಕ		    fd;
    ಸಂರಚನಾ block0   b0;
#ifdef UNIX
    char_u	    uname[B0_UNAME_SIZE];
#endif

    // print the swap file date
    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st) != -1)
    {
#ifdef UNIX
	// print name of owner of the file
	ಯದಿ (mch_get_uname(st.st_uid, uname, B0_UNAME_SIZE) == OK)
	{
	    ಸಂದೇಶ_puts(_("          owned by: "));
	    ಸಂದೇಶ_outtrans(uname);
	    ಸಂದೇಶ_puts(_("   dated: "));
	}
	ಅನ್ಯಥಾ
#endif
	    ಸಂದೇಶ_puts(_("             dated: "));
	ಸಂದೇಶ_puts(get_ctime(st.st_mtime, TRUE));
    }
    ಅನ್ಯಥಾ
	st.st_mtime = 0;

    /*
     * print the original file name
     */
    fd = mch_open((ವರ್ಣ *)fname, O_RDONLY | O_EXTRA, 0);
    ಯದಿ (fd >= 0)
    {
	ಯದಿ (read_eintr(fd, &b0, ಪರಿಮಾಣ(b0)) == ಪರಿಮಾಣ(b0))
	{
	    ಯದಿ (STRNCMP(b0.b0_version, "VIM 3.0", 7) == 0)
	    {
		ಸಂದೇಶ_puts(_("         [from Vim version 3.0]"));
	    }
	    ಅನ್ಯಥಾ ಯದಿ (ml_check_b0_id(&b0) == FAIL)
	    {
		ಸಂದೇಶ_puts(_("         [does not look like a Vim swap file]"));
	    }
	    ಅನ್ಯಥಾ
	    {
		ಸಂದೇಶ_puts(_("         file name: "));
		ಯದಿ (b0.b0_fname[0] == NUL)
		    ಸಂದೇಶ_puts(_("[No Name]"));
		ಅನ್ಯಥಾ
		    ಸಂದೇಶ_outtrans(b0.b0_fname);

		ಸಂದೇಶ_puts(_("\n          modified: "));
		ಸಂದೇಶ_puts(b0.b0_dirty ? _("YES") : _("no"));

		ಯದಿ (*(b0.b0_uname) != NUL)
		{
		    ಸಂದೇಶ_puts(_("\n         user name: "));
		    ಸಂದೇಶ_outtrans(b0.b0_uname);
		}

		ಯದಿ (*(b0.b0_hname) != NUL)
		{
		    ಯದಿ (*(b0.b0_uname) != NUL)
			ಸಂದೇಶ_puts(_("   host name: "));
		    ಅನ್ಯಥಾ
			ಸಂದೇಶ_puts(_("\n         host name: "));
		    ಸಂದೇಶ_outtrans(b0.b0_hname);
		}

		ಯದಿ (char_to_long(b0.b0_pid) != 0L)
		{
		    ಸಂದೇಶ_puts(_("\n        process ID: "));
		    ಸಂದೇಶ_outnum(char_to_long(b0.b0_pid));
#ಯದಿ defined(UNIX) || defined(MSWIN)
		    ಯದಿ (swapfile_process_running(&b0, fname))
		    {
			ಸಂದೇಶ_puts(_(" (STILL RUNNING)"));
# ifdef HAVE_PROCESS_STILL_RUNNING
			process_still_running = TRUE;
# endif
		    }
#endif
		}

		ಯದಿ (b0_magic_wrong(&b0))
		{
#ಯದಿ defined(MSWIN)
		    ಯದಿ (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			ಸಂದೇಶ_puts(_("\n         [not usable with this version of Vim]"));
		    ಅನ್ಯಥಾ
#endif
			ಸಂದೇಶ_puts(_("\n         [not usable on this computer]"));
		}
	    }
	}
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ_puts(_("         [cannot be read]"));
	close(fd);
    }
    ಅನ್ಯಥಾ
	ಸಂದೇಶ_puts(_("         [cannot be opened]"));
    ಸಂದೇಶ_putchar('\n');

    ಪ್ರತ್ಯಾಗಮ st.st_mtime;
}

/*
 * Return TRUE if the swap file looks OK and there are no changes, thus it can
 * be safely deleted.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
swapfile_unchanged(char_u *fname)
{
    stat_T	    st;
    ಪೂರ್ಣಾಙ್ಕ		    fd;
    ಸಂರಚನಾ block0   b0;
    ಪೂರ್ಣಾಙ್ಕ		    ret = TRUE;

    // must be able to stat the swap file
    ಯದಿ (mch_stat((ವರ್ಣ *)fname, &st) == -1)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // must be able to read the first block
    fd = mch_open((ವರ್ಣ *)fname, O_RDONLY | O_EXTRA, 0);
    ಯದಿ (fd < 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (read_eintr(fd, &b0, ಪರಿಮಾಣ(b0)) != ಪರಿಮಾಣ(b0))
    {
	close(fd);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    // the ID and magic number must be correct
    ಯದಿ (ml_check_b0_id(&b0) == FAIL|| b0_magic_wrong(&b0))
	ret = FALSE;

    // must be unchanged
    ಯದಿ (b0.b0_dirty)
	ret = FALSE;

#ಯದಿ defined(UNIX) || defined(MSWIN)
    // Host name must be known and must equal the current host name, otherwise
    // comparing pid is meaningless.
    ಯದಿ (*(b0.b0_hname) == NUL)
    {
	ret = FALSE;
    }
    ಅನ್ಯಥಾ
    {
	char_u	    hostname[B0_HNAME_SIZE];

	mch_get_host_name(hostname, B0_HNAME_SIZE);
	hostname[B0_HNAME_SIZE - 1] = NUL;
	b0.b0_hname[B0_HNAME_SIZE - 1] = NUL; // in case of corruption
	ಯದಿ (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    ಯದಿ (char_to_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#endif

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    close(fd);
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
recov_file_names(char_u **names, char_u *path, ಪೂರ್ಣಾಙ್ಕ prepend_dot)
{
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_names;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;
#ifndef MSWIN
    bool    shortname = curbuf->b_shortname;

    curbuf->b_shortname = false;
#endif
    string_T	ret;

    ಅಂಕೆ_names = 0;

    /*
     * May also add the file name with a dot prepended, for swap file in same
     * dir as original file.
     */
    ಯದಿ (prepend_dot)
    {
	names[ಅಂಕೆ_names] = modname(path, (char_u *)".sw?", TRUE);
	ಯದಿ (names[ಅಂಕೆ_names] == ರಿಕ್ತ)
	    ಗಚ್ಛ end;
	++ಅಂಕೆ_names;
    }

    /*
     * Form the normal swap file name pattern by appending ".sw?".
     */
#ifdef VMS
    names[ಅಂಕೆ_names] = concat_fnames(path, STRLEN(path),
	(char_u *)"_sw%", STRLEN_LITERAL("_sw%"), FALSE, &ret);
#ಅನ್ಯಥಾ
    names[ಅಂಕೆ_names] = concat_fnames(path, STRLEN(path),
	(char_u *)".sw?", STRLEN_LITERAL(".sw?"), FALSE, &ret);
#endif
    ಯದಿ (names[ಅಂಕೆ_names] == ರಿಕ್ತ)
	ಗಚ್ಛ end;
    ಯದಿ (ಅಂಕೆ_names >= 1)	    // check if we have the same name twice
    {
	p = names[ಅಂಕೆ_names - 1];
	i = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(names[ಅಂಕೆ_names - 1]) - (ಪೂರ್ಣಾಙ್ಕ)STRLEN(names[ಅಂಕೆ_names]);
	ಯದಿ (i > 0)
	    p += i;	    // file name has been expanded to full path

	ಯದಿ (STRCMP(p, names[ಅಂಕೆ_names]) != 0)
	    ++ಅಂಕೆ_names;
	ಅನ್ಯಥಾ
	    vim_free(names[ಅಂಕೆ_names]);
    }
    ಅನ್ಯಥಾ
	++ಅಂಕೆ_names;

#ifndef MSWIN
    /*
     * Also try with 'shortname' set, in case the file is on a DOS filesystem.
     */
    curbuf->b_shortname = true;
# ifdef VMS
    names[ಅಂಕೆ_names] = modname(path, (char_u *)"_sw%", FALSE);
# ಅನ್ಯಥಾ
    names[ಅಂಕೆ_names] = modname(path, (char_u *)".sw?", FALSE);
# endif
    ಯದಿ (names[ಅಂಕೆ_names] == ರಿಕ್ತ)
	ಗಚ್ಛ end;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = names[ಅಂಕೆ_names];
    i = STRLEN(names[ಅಂಕೆ_names]) - STRLEN(names[ಅಂಕೆ_names - 1]);
    ಯದಿ (i > 0)
	p += i;		// file name has been expanded to full path
    ಯದಿ (STRCMP(names[ಅಂಕೆ_names - 1], p) == 0)
	vim_free(names[ಅಂಕೆ_names]);
    ಅನ್ಯಥಾ
	++ಅಂಕೆ_names;
#endif

end:
#ifndef MSWIN
    curbuf->b_shortname = shortname;
#endif

    ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_names;
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
    ಶೂನ್ಯ
ml_sync_all(ಪೂರ್ಣಾಙ್ಕ check_file, ಪೂರ್ಣಾಙ್ಕ check_char)
{
    buf_T		*buf;
    stat_T		st;

    FOR_ALL_BUFFERS(buf)
    {
	ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ
		|| buf->b_ml.ml_mfp->mf_fname == ರಿಕ್ತ
		|| buf->b_ml.ml_mfp->mf_fd < 0)
	    ಅನುವರ್ತ;			    // no file

#ifdef FEAT_CRYPT
	ಯದಿ (crypt_may_close_swapfile(buf, buf->b_p_ಕೀಲಿ,
						     crypt_get_method_nr(buf)))
	    ಅನುವರ್ತ;
#endif

	ml_flush_ಸಾಲು(buf);		    // flush buffered line
					    // flush locked block
	(ಶೂನ್ಯ)ml_find_ಸಾಲು(buf, (linenr_T)0, ML_FLUSH);
	ಯದಿ (bufIsChanged(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != ರಿಕ್ತ)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    ಯದಿ (mch_stat((ವರ್ಣ *)buf->b_ffname, &st) == -1
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
	ಯದಿ (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (ಶೂನ್ಯ)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (bufIsChanged(buf) ? MFS_FLUSH : 0));
	    ಯದಿ (check_char && ui_char_avail())	// character available now
		ಭಙ್ಗ;
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
    ಶೂನ್ಯ
ml_preserve(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ message)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    ಪೂರ್ಣಾಙ್ಕ		status;
    ಪೂರ್ಣಾಙ್ಕ		got_int_save = got_int;

    ಯದಿ (mfp == ರಿಕ್ತ || mfp->mf_fname == ರಿಕ್ತ)
    {
	ಯದಿ (message)
	    emsg(_(e_cannot_preserve_there_is_no_swap_file));
	ಪ್ರತ್ಯಾಗಮ;
    }
#ifdef FEAT_CRYPT
    ಯದಿ (crypt_may_close_swapfile(buf, buf->b_p_ಕೀಲಿ, crypt_get_method_nr(buf)))
	ಪ್ರತ್ಯಾಗಮ;
#endif

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_ಸಾಲು(buf);				    // flush buffered line
    (ಶೂನ್ಯ)ml_find_ಸಾಲು(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    status = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

    // stack is invalid after mf_sync(.., MFS_ALL)
    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;

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
    ಯದಿ (mf_need_trans(mfp) && !got_int)
    {
	lnum = 1;
	ಯಾವತ್ (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    hp = ml_find_ಸಾಲು(buf, lnum, ML_FIND);
	    ಯದಿ (hp == ರಿಕ್ತ)
	    {
		status = FAIL;
		ಗಚ್ಛ theend;
	    }
	    CHECK(buf->b_ml.ml_locked_ಕೆಳಗಿನ != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_ಮೇಲಿನ + 1;
	}
	(ಶೂನ್ಯ)ml_find_ಸಾಲು(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	ಯದಿ (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    status = FAIL;
	buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    ಯದಿ (message)
    {
	ಯದಿ (status == OK)
	    ಸಂದೇಶ(_("File preserved"));
	ಅನ್ಯಥಾ
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
    ಪ್ರತ್ಯಾಗಮ ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
    char_u *
ml_get_pos(pos_T *pos)
{
    ಪ್ರತ್ಯಾಗಮ (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->col);
}

/*
 * Return pointer to cursor line.
 */
    char_u *
ml_get_curline(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE);
}

/*
 * Return pointer to cursor position.
 */
    char_u *
ml_get_cursor(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE) +
							curwin->w_cursor.col);
}

// return length (excluding the NUL) of the given line
    colnr_T
ml_get_len(linenr_T lnum)
{
    ಪ್ರತ್ಯಾಗಮ ml_get_buf_len(curbuf, lnum);
}

// return length (excluding the NUL) of the text after position "pos"
    colnr_T
ml_get_pos_len(pos_T *pos)
{
    ಪ್ರತ್ಯಾಗಮ ml_get_buf_len(curbuf, pos->lnum) - pos->col;
}

// return length (excluding the NUL) of the cursor line
    colnr_T
ml_get_curline_len(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ml_get_buf_len(curbuf, curwin->w_cursor.lnum);
}

// return length (excluding the NUL) of the cursor position
    colnr_T
ml_get_cursor_len(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.col;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*ಸಾಲು;

    ಯದಿ (*(ಸಾಲು = ml_get_buf(buf, lnum, FALSE)) == NUL)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (buf->b_ml.ml_ಸಾಲು_textlen <= 0)
	buf->b_ml.ml_ಸಾಲು_textlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಾಲು) + 1;
    ಪ್ರತ್ಯಾಗಮ buf->b_ml.ml_ಸಾಲು_textlen - 1;
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
    ಪೂರ್ಣಾಙ್ಕ		will_change)		// line will be changed
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = 0;
    ಸ್ಥಿರ char_u questions[4];

    ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)	// invalid line number
    {
	ಯದಿ (recursive == 0)
	{
	    // Avoid giving this message for a recursive call, may happen when
	    // the GUI redraws part of the text.
	    ++recursive;
	    siemsg(e_ml_get_invalid_lnum_nr, lnum);
	    --recursive;
	}
	ml_flush_ಸಾಲು(buf);
errorret:
	STRCPY(questions, "???");
	buf->b_ml.ml_ಸಾಲು_len = 4;
	buf->b_ml.ml_ಸಾಲು_textlen = buf->b_ml.ml_ಸಾಲು_len;
	buf->b_ml.ml_ಸಾಲು_lnum = lnum;
	ಪ್ರತ್ಯಾಗಮ questions;
    }
    ಯದಿ (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)	// there are no lines
    {
	buf->b_ml.ml_ಸಾಲು_len = 1;
	buf->b_ml.ml_ಸಾಲು_textlen = buf->b_ml.ml_ಸಾಲು_len;
	ಪ್ರತ್ಯಾಗಮ (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    ಯದಿ (buf->b_ml.ml_ಸಾಲು_lnum != lnum || mf_dont_release)
    {
	unsigned    start, end;
	ಪೂರ್ಣಾಙ್ಕ	    idx;

	ml_flush_ಸಾಲು(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	ಯದಿ ((hp = ml_find_ಸಾಲು(buf, lnum, ML_FIND)) == ರಿಕ್ತ)
	{
	    ಯದಿ (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_find_ಸಾಲು_nr_in_buffer_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    ಗಚ್ಛ errorret;
	}

	dp = (DATA_BL *)(hp->bh_data);

	idx = lnum - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
	start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	ಯದಿ (idx == 0)
	    end = dp->db_txt_end;
	ಅನ್ಯಥಾ
	    end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);

	buf->b_ml.ml_ಸಾಲು_ptr = (char_u *)dp + start;
	buf->b_ml.ml_ಸಾಲು_len = end - start;
#ಯದಿ defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	ಯದಿ (buf->b_has_textprop
			 && (size_t)buf->b_ml.ml_ಸಾಲು_len > ಪರಿಮಾಣ(textprop_T))
	    buf->b_ml.ml_ಸಾಲು_textlen = 0;  // call STRLEN() later when needed
	ಅನ್ಯಥಾ
#endif
	    buf->b_ml.ml_ಸಾಲು_textlen = buf->b_ml.ml_ಸಾಲು_len;
	buf->b_ml.ml_ಸಾಲು_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_ಸಾಲು_DIRTY | ML_ALLOCATED);
    }
    ಯದಿ (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ifdef FEAT_EVAL
	ಯದಿ (ml_get_alloc_ಸಾಲುಗಳು && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;
#endif
    }

#ifdef FEAT_EVAL
    ಯದಿ (ml_get_alloc_ಸಾಲುಗಳು
		 && (buf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_ಸಾಲು_len);

	// make sure the text is in allocated memory
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    memmove(p, buf->b_ml.ml_ಸಾಲು_ptr, buf->b_ml.ml_ಸಾಲು_len);
	    buf->b_ml.ml_ಸಾಲು_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    ಯದಿ (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;
	}
    }
#endif
    ಪ್ರತ್ಯಾಗಮ buf->b_ml.ml_ಸಾಲು_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    ಪೂರ್ಣಾಙ್ಕ
ml_ಸಾಲು_alloced(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (curbuf->b_ml.ml_flags & ML_ಸಾಲು_DIRTY);
}

#ifdef FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_text_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**ಸಾಲು,
	    ಪೂರ್ಣಾಙ್ಕ		*len,
	    char_u	**tofree)
{
    ಪೂರ್ಣಾಙ್ಕ		round;
    ಪೂರ್ಣಾಙ್ಕ		new_prop_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		n;
    char_u	*props;
    ಪೂರ್ಣಾಙ್ಕ		new_len = 0;  // init for gcc
    char_u	*new_ಸಾಲು = ರಿಕ್ತ;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    ಪ್ರತಿ (round = 1; round <= 2; ++round)
    {
	ಯದಿ (round == 2)
	{
	    uint16_t pc;

	    ಯದಿ (new_prop_ಎಣಿಕೆ == 0)
		ಪ್ರತ್ಯಾಗಮ;  // nothing to do
	    new_len = *len + (ಪೂರ್ಣಾಙ್ಕ)PROP_ಎಣಿಕೆ_SIZE
			     + new_prop_ಎಣಿಕೆ * (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(textprop_T);
	    new_ಸಾಲು = alloc(new_len);
	    ಯದಿ (new_ಸಾಲು == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ;
	    mch_memmove(new_ಸಾಲು, *ಸಾಲು, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_ಎಣಿಕೆ;
	    mch_memmove(new_ಸಾಲು + *len, &pc, PROP_ಎಣಿಕೆ_SIZE);
	    new_prop_ಎಣಿಕೆ = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	ಎಣಿಕೆ = get_text_props(buf, lnum, &props, FALSE);
	ಪ್ರತಿ (n = 0; n < ಎಣಿಕೆ; ++n)
	{
	    mch_memmove(&prop, props + n * ಪರಿಮಾಣ(textprop_T),
							   ಪರಿಮಾಣ(textprop_T));
	    ಯದಿ (prop.tp_flags & TP_FLAG_CONT_NEXT)
	    {
		ಯದಿ (round == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_PREV;
		    prop.tp_col = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_text_offset = 0;
		    mch_memmove(new_ಸಾಲು + *len + (ಪೂರ್ಣಾಙ್ಕ)PROP_ಎಣಿಕೆ_SIZE
			    + new_prop_ಎಣಿಕೆ * ಪರಿಮಾಣ(textprop_T),
			    &prop, ಪರಿಮಾಣ(textprop_T));
		}
		++new_prop_ಎಣಿಕೆ;
	    }
	}
    }
    *ಸಾಲು = new_ಸಾಲು;
    *tofree = new_ಸಾಲು;
    *len = new_len;
}
#endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ಸಾಲು_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    ಪೂರ್ಣಾಙ್ಕ		flags)		// ML_APPEND_ flags
{
    char_u	*ಸಾಲು = ಸಾಲು_arg;
    colnr_T	len = len_arg;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_ಎಣಿಕೆ;	// number of indexes in current block
    ಪೂರ್ಣಾಙ್ಕ		offset;
    ಪೂರ್ಣಾಙ್ಕ		from, to;
    long	space_needed;	// space needed for new line
    ಪೂರ್ಣಾಙ್ಕ		page_size;
    ಪೂರ್ಣಾಙ್ಕ		page_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		db_idx;		// index for lnum in data block
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
#ifdef FEAT_PROP_POPUP
    char_u	*tofree = ರಿಕ್ತ;
# ifdef FEAT_BYTEOFF
    colnr_T	text_len = 0;	// text len with NUL without text properties
# endif
#endif
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;

    ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || buf->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;  // lnum out of range

    ಯದಿ (lowest_marked && lowest_marked > lnum)
	lowest_marked = lnum + 1;

    ಯದಿ (len == 0)
    {
	len = (colnr_T)STRLEN(ಸಾಲು) + 1;	// space needed for the text
#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	text_len = len;
#endif
    }
#ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    ಅನ್ಯಥಾ ಯದಿ (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	text_len = (colnr_T)STRLEN(ಸಾಲು) + 1;
    ಅನ್ಯಥಾ
	text_len = len;
#endif

#ifdef FEAT_PROP_POPUP
    ಯದಿ (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	add_text_props_for_append(buf, lnum, &ಸಾಲು, &len, &tofree);
#endif

    space_needed = len + INDEX_SIZE;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    page_size = mfp->mf_page_size;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    ಯದಿ ((hp = ml_find_ಸಾಲು(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_ಸೇರಿಸು)) == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    buf->b_ml.ml_flags &= ~ML_EMPTY;

    ಯದಿ (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    ಅನ್ಯಥಾ
	db_idx = lnum - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
		// get line count before the insertion
    ಸಾಲು_ಎಣಿಕೆ = buf->b_ml.ml_locked_ಮೇಲಿನ - buf->b_ml.ml_locked_ಕೆಳಗಿನ;

    dp = (DATA_BL *)(hp->bh_data);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    ಯದಿ ((long)dp->db_free < space_needed && db_idx == ಸಾಲು_ಎಣಿಕೆ - 1
					    && lnum < buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_ಮೇಲಿನ);
	ಯದಿ ((hp = ml_find_ಸಾಲು(buf, lnum + 1, ML_ಸೇರಿಸು)) == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	ಸಾಲು_ಎಣಿಕೆ = buf->b_ml.ml_locked_ಮೇಲಿನ - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
	CHECK(buf->b_ml.ml_locked_ಕೆಳಗಿನ != lnum + 1, "locked_low != lnum + 1");

	dp = (DATA_BL *)(hp->bh_data);
    }

    ++buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಯದಿ ((long)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_start -= len;
	dp->db_free -= space_needed;
	++(dp->db_ಸಾಲು_ಎಣಿಕೆ);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	ಯದಿ (ಸಾಲು_ಎಣಿಕೆ > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    ಯದಿ (db_idx < 0)
		offset = dp->db_txt_end;
	    ಅನ್ಯಥಾ
		offset = ((dp->db_index[db_idx]) & DB_INDEX_MASK);
	    mch_memmove((ವರ್ಣ *)dp + dp->db_txt_start,
					  (ವರ್ಣ *)dp + dp->db_txt_start + len,
				 (size_t)(offset - (dp->db_txt_start + len)));
	    ಪ್ರತಿ (i = ಸಾಲು_ಎಣಿಕೆ - 1; i > db_idx; --i)
		dp->db_index[i + 1] = dp->db_index[i] - len;
	    dp->db_index[db_idx + 1] = offset - len;
	}
	ಅನ್ಯಥಾ
	    // add line at the end (which is the start of the text)
	    dp->db_index[db_idx + 1] = dp->db_txt_start;

	/*
	 * copy the text into the block
	 */
	mch_memmove((ವರ್ಣ *)dp + dp->db_index[db_idx + 1], ಸಾಲು, (size_t)len);
	ಯದಿ (flags & ML_APPEND_MARK)
	    dp->db_index[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	ಯದಿ (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    ಅನ್ಯಥಾ	    // not enough space in data block
    {
	long	    ಸಾಲು_ಎಣಿಕೆ_ಎಡ, ಸಾಲು_ಎಣಿಕೆ_ಬಲ;
	ಪೂರ್ಣಾಙ್ಕ	    page_ಎಣಿಕೆ_ಎಡ, page_ಎಣಿಕೆ_ಬಲ;
	bhdr_T	    *hp_ಎಡ;
	bhdr_T	    *hp_ಬಲ;
	bhdr_T	    *hp_new;
	ಪೂರ್ಣಾಙ್ಕ	    ಸಾಲುಗಳು_moved;
	ಪೂರ್ಣಾಙ್ಕ	    data_moved = 0;	    // init to shut up gcc
	ಪೂರ್ಣಾಙ್ಕ	    total_moved = 0;	    // init to shut up gcc
	DATA_BL	    *dp_ಬಲ, *dp_ಎಡ;
	ಪೂರ್ಣಾಙ್ಕ	    ರಾಶಿ_idx;
	ಪೂರ್ಣಾಙ್ಕ	    in_ಎಡ;
	ಪೂರ್ಣಾಙ್ಕ	    lineadd;
	blocknr_T   bnum_ಎಡ, bnum_ಬಲ;
	linenr_T    lnum_ಎಡ, lnum_ಬಲ;
	ಪೂರ್ಣಾಙ್ಕ	    pb_idx;
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
	ಯದಿ (db_idx < 0)		// left block is new, right block is existing
	{
	    ಸಾಲುಗಳು_moved = 0;
	    in_ಎಡ = TRUE;
	    // space_needed does not change
	}
	ಅನ್ಯಥಾ			// left block is existing, right block is new
	{
	    ಸಾಲುಗಳು_moved = ಸಾಲು_ಎಣಿಕೆ - db_idx - 1;
	    ಯದಿ (ಸಾಲುಗಳು_moved == 0)
		in_ಎಡ = FALSE;	// put new line in right block
					// space_needed does not change
	    ಅನ್ಯಥಾ
	    {
		data_moved = ((dp->db_index[db_idx]) & DB_INDEX_MASK) -
							    dp->db_txt_start;
		total_moved = data_moved + ಸಾಲುಗಳು_moved * INDEX_SIZE;
		ಯದಿ ((long)dp->db_free + total_moved >= space_needed)
		{
		    in_ಎಡ = TRUE;	// put new line in left block
		    space_needed = total_moved;
		}
		ಅನ್ಯಥಾ
		{
		    in_ಎಡ = FALSE;	    // put new line in right block
		    space_needed += total_moved;
		}
	    }
	}

	page_ಎಣಿಕೆ = ((space_needed + HEADER_SIZE) + page_size - 1) / page_size;
	ಯದಿ ((hp_new = ml_new_data(mfp, flags & ML_APPEND_NEW, page_ಎಣಿಕೆ))
								       == ರಿಕ್ತ)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_ಮೇಲಿನ);
	    ಗಚ್ಛ theend;
	}
	ಯದಿ (db_idx < 0)		// left block is new
	{
	    hp_ಎಡ = hp_new;
	    hp_ಬಲ = hp;
	    ಸಾಲು_ಎಣಿಕೆ_ಎಡ = 0;
	    ಸಾಲು_ಎಣಿಕೆ_ಬಲ = ಸಾಲು_ಎಣಿಕೆ;
	}
	ಅನ್ಯಥಾ			// right block is new
	{
	    hp_ಎಡ = hp;
	    hp_ಬಲ = hp_new;
	    ಸಾಲು_ಎಣಿಕೆ_ಎಡ = ಸಾಲು_ಎಣಿಕೆ;
	    ಸಾಲು_ಎಣಿಕೆ_ಬಲ = 0;
	}
	dp_ಬಲ = (DATA_BL *)(hp_ಬಲ->bh_data);
	dp_ಎಡ = (DATA_BL *)(hp_ಎಡ->bh_data);
	bnum_ಎಡ = hp_ಎಡ->bh_bnum;
	bnum_ಬಲ = hp_ಬಲ->bh_bnum;
	page_ಎಣಿಕೆ_ಎಡ = hp_ಎಡ->bh_page_ಎಣಿಕೆ;
	page_ಎಣಿಕೆ_ಬಲ = hp_ಬಲ->bh_page_ಎಣಿಕೆ;

	/*
	 * May move the new line into the right/new block.
	 */
	ಯದಿ (!in_ಎಡ)
	{
	    dp_ಬಲ->db_txt_start -= len;
	    dp_ಬಲ->db_free -= len + INDEX_SIZE;
	    dp_ಬಲ->db_index[0] = dp_ಬಲ->db_txt_start;
	    ಯದಿ (flags & ML_APPEND_MARK)
		dp_ಬಲ->db_index[0] |= DB_MARKED;

	    mch_memmove((ವರ್ಣ *)dp_ಬಲ + dp_ಬಲ->db_txt_start,
							   ಸಾಲು, (size_t)len);
	    ++ಸಾಲು_ಎಣಿಕೆ_ಬಲ;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	ಯದಿ (ಸಾಲುಗಳು_moved)
	{
	    dp_ಬಲ->db_txt_start -= data_moved;
	    dp_ಬಲ->db_free -= total_moved;
	    mch_memmove((ವರ್ಣ *)dp_ಬಲ + dp_ಬಲ->db_txt_start,
			(ವರ್ಣ *)dp_ಎಡ + dp_ಎಡ->db_txt_start,
			(size_t)data_moved);
	    offset = dp_ಬಲ->db_txt_start - dp_ಎಡ->db_txt_start;
	    dp_ಎಡ->db_txt_start += data_moved;
	    dp_ಎಡ->db_free += total_moved;

	    /*
	     * update indexes in the new block
	     */
	    ಪ್ರತಿ (to = ಸಾಲು_ಎಣಿಕೆ_ಬಲ, from = db_idx + 1;
					 from < ಸಾಲು_ಎಣಿಕೆ_ಎಡ; ++from, ++to)
		dp_ಬಲ->db_index[to] = dp->db_index[from] + offset;
	    ಸಾಲು_ಎಣಿಕೆ_ಬಲ += ಸಾಲುಗಳು_moved;
	    ಸಾಲು_ಎಣಿಕೆ_ಎಡ -= ಸಾಲುಗಳು_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	ಯದಿ (in_ಎಡ)
	{
	    dp_ಎಡ->db_txt_start -= len;
	    dp_ಎಡ->db_free -= len + INDEX_SIZE;
	    dp_ಎಡ->db_index[ಸಾಲು_ಎಣಿಕೆ_ಎಡ] = dp_ಎಡ->db_txt_start;
	    ಯದಿ (flags & ML_APPEND_MARK)
		dp_ಎಡ->db_index[ಸಾಲು_ಎಣಿಕೆ_ಎಡ] |= DB_MARKED;
	    mch_memmove((ವರ್ಣ *)dp_ಎಡ + dp_ಎಡ->db_txt_start,
							   ಸಾಲು, (size_t)len);
	    ++ಸಾಲು_ಎಣಿಕೆ_ಎಡ;
	}

	ಯದಿ (db_idx < 0)		// left block is new
	{
	    lnum_ಎಡ = lnum + 1;
	    lnum_ಬಲ = 0;
	}
	ಅನ್ಯಥಾ			// right block is new
	{
	    lnum_ಎಡ = 0;
	    ಯದಿ (in_ಎಡ)
		lnum_ಬಲ = lnum + 2;
	    ಅನ್ಯಥಾ
		lnum_ಬಲ = lnum + 1;
	}
	dp_ಎಡ->db_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಎಡ;
	dp_ಬಲ->db_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಬಲ;

	/*
	 * release the two data blocks
	 * The new one (hp_new) already has a correct blocknumber.
	 * The old one (hp, in ml_locked) gets a positive blocknumber if
	 * we changed it and we are not editing a new file.
	 */
	ಯದಿ (ಸಾಲುಗಳು_moved || in_ಎಡ)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	ಯದಿ (!(flags & ML_APPEND_NEW) && db_idx >= 0 && in_ಎಡ)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_find_ಸಾಲು(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	ಪ್ರತಿ (ರಾಶಿ_idx = buf->b_ml.ml_ರಾಶಿ_ತುದಿ - 1; ರಾಶಿ_idx >= 0;
								  --ರಾಶಿ_idx)
	{
	    ip = &(buf->b_ml.ml_ರಾಶಿ[ರಾಶಿ_idx]);
	    pb_idx = ip->ip_index;
	    ಯದಿ ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    ಯದಿ (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		ಗಚ್ಛ theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    ಯದಿ (pp->pb_ಎಣಿಕೆ < pp->pb_ಎಣಿಕೆ_max)
	    {
		ಯದಿ (pb_idx + 1 < (ಪೂರ್ಣಾಙ್ಕ)pp->pb_ಎಣಿಕೆ)
		    mch_memmove(&pp->pb_pointer[pb_idx + 2],
				&pp->pb_pointer[pb_idx + 1],
			(size_t)(pp->pb_ಎಣಿಕೆ - pb_idx - 1) * ಪರಿಮಾಣ(PTR_EN));
		++pp->pb_ಎಣಿಕೆ;
		pp->pb_pointer[pb_idx].pe_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಎಡ;
		pp->pb_pointer[pb_idx].pe_bnum = bnum_ಎಡ;
		pp->pb_pointer[pb_idx].pe_page_ಎಣಿಕೆ = page_ಎಣಿಕೆ_ಎಡ;
		pp->pb_pointer[pb_idx + 1].pe_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಬಲ;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_ಬಲ;
		pp->pb_pointer[pb_idx + 1].pe_page_ಎಣಿಕೆ = page_ಎಣಿಕೆ_ಬಲ;

		ಯದಿ (lnum_ಎಡ != 0)
		    pp->pb_pointer[pb_idx].pe_old_lnum = lnum_ಎಡ;
		ಯದಿ (lnum_ಬಲ != 0)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_ಬಲ;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_ರಾಶಿ_ತುದಿ = ರಾಶಿ_idx + 1;	    // truncate stack

		ಯದಿ (lineadd)
		{
		    --(buf->b_ml.ml_ರಾಶಿ_ತುದಿ);
		    // fix line count for rest of blocks in the stack
		    ml_lineadd(buf, lineadd);
							// fix stack itself
		    buf->b_ml.ml_ರಾಶಿ[buf->b_ml.ml_ರಾಶಿ_ತುದಿ].ip_ಮೇಲಿನ +=
								      lineadd;
		    ++(buf->b_ml.ml_ರಾಶಿ_ತುದಿ);
		}

		/*
		 * We are finished, break the loop here.
		 */
		ಭಙ್ಗ;
	    }
	    // pointer block full
	    /*
	     * split the pointer block
	     * allocate a new pointer block
	     * move some of the pointer into the new block
	     * prepare for updating the parent block
	     */
	    ಪ್ರತಿ (;;)	// do this twice when splitting block 1
	    {
		hp_new = ml_new_ptr(mfp);
		ಯದಿ (hp_new == ರಿಕ್ತ)	    // TODO: try to fix tree
		    ಗಚ್ಛ theend;
		pp_new = (PTR_BL *)(hp_new->bh_data);

		ಯದಿ (hp->bh_bnum != 1)
		    ಭಙ್ಗ;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (size_t)page_size);
		pp->pb_ಎಣಿಕೆ = 1;
		pp->pb_pointer[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_pointer[0].pe_ಸಾಲು_ಎಣಿಕೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		pp->pb_pointer[0].pe_old_lnum = 1;
		pp->pb_pointer[0].pe_page_ಎಣಿಕೆ = 1;
		mf_put(mfp, hp, TRUE, FALSE);   // release block 1
		hp = hp_new;		// new block is to be split
		pp = pp_new;
		CHECK(ರಾಶಿ_idx != 0, _("stack_idx should be 0"));
		ip->ip_index = 0;
		++ರಾಶಿ_idx;	// do block 1 again later
	    }
	    /*
	     * move the pointers after the current one to the new block
	     * If there are none, the new entry will be in the new block.
	     */
	    total_moved = pp->pb_ಎಣಿಕೆ - pb_idx - 1;
	    ಯದಿ (total_moved)
	    {
		mch_memmove(&pp_new->pb_pointer[0],
				&pp->pb_pointer[pb_idx + 1],
				(size_t)(total_moved) * ಪರಿಮಾಣ(PTR_EN));
		pp_new->pb_ಎಣಿಕೆ = total_moved;
		pp->pb_ಎಣಿಕೆ -= total_moved - 1;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_ಬಲ;
		pp->pb_pointer[pb_idx + 1].pe_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಬಲ;
		pp->pb_pointer[pb_idx + 1].pe_page_ಎಣಿಕೆ = page_ಎಣಿಕೆ_ಬಲ;
		ಯದಿ (lnum_ಬಲ)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_ಬಲ;
	    }
	    ಅನ್ಯಥಾ
	    {
		pp_new->pb_ಎಣಿಕೆ = 1;
		pp_new->pb_pointer[0].pe_bnum = bnum_ಬಲ;
		pp_new->pb_pointer[0].pe_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಬಲ;
		pp_new->pb_pointer[0].pe_page_ಎಣಿಕೆ = page_ಎಣಿಕೆ_ಬಲ;
		pp_new->pb_pointer[0].pe_old_lnum = lnum_ಬಲ;
	    }
	    pp->pb_pointer[pb_idx].pe_bnum = bnum_ಎಡ;
	    pp->pb_pointer[pb_idx].pe_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಎಡ;
	    pp->pb_pointer[pb_idx].pe_page_ಎಣಿಕೆ = page_ಎಣಿಕೆ_ಎಡ;
	    ಯದಿ (lnum_ಎಡ)
		pp->pb_pointer[pb_idx].pe_old_lnum = lnum_ಎಡ;
	    lnum_ಎಡ = 0;
	    lnum_ಬಲ = 0;

	    /*
	     * recompute line counts
	     */
	    ಸಾಲು_ಎಣಿಕೆ_ಬಲ = 0;
	    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)pp_new->pb_ಎಣಿಕೆ; ++i)
		ಸಾಲು_ಎಣಿಕೆ_ಬಲ += pp_new->pb_pointer[i].pe_ಸಾಲು_ಎಣಿಕೆ;
	    ಸಾಲು_ಎಣಿಕೆ_ಎಡ = 0;
	    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)pp->pb_ಎಣಿಕೆ; ++i)
		ಸಾಲು_ಎಣಿಕೆ_ಎಡ += pp->pb_pointer[i].pe_ಸಾಲು_ಎಣಿಕೆ;

	    bnum_ಎಡ = hp->bh_bnum;
	    bnum_ಬಲ = hp_new->bh_bnum;
	    page_ಎಣಿಕೆ_ಎಡ = 1;
	    page_ಎಣಿಕೆ_ಬಲ = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	ಯದಿ (ರಾಶಿ_idx < 0)
	{
	    iemsg(e_updated_too_many_blocks);
	    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;	// invalidate stack
	}
    }

#ifdef FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (long)text_len
# ಅನ್ಯಥಾ
	    (long)len
# endif
	    , ML_CHNK_ADDLINE);
#endif

#ifdef FEAT_NETBEANS_INTG
    ಯದಿ (!(flags & ML_APPEND_NEW) && netbeans_active())
    {
	ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಾಲು);
	ಯದಿ (ಸಾಲು_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, ಸಾಲು, ಸಾಲು_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)ಸಾಲು_len, (char_u *)"\n", 1);
    }
#endif
#ifdef FEAT_JOB_CHANNEL
    ಯದಿ (!(flags & ML_APPEND_NEW) && buf->b_write_to_channel)
	channel_write_new_ಸಾಲುಗಳು(buf);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    vim_free(tofree);
#endif
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ಸಾಲು,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    ಪೂರ್ಣಾಙ್ಕ		flags)		// ML_APPEND_ flags
{
    ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ FAIL;  // lnum out of range

    ಯದಿ (buf->b_ml.ml_ಸಾಲು_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_ಸಾಲು(buf);

#ifdef FEAT_EVAL
    ಯದಿ (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	ಯದಿ (buf->b_ml.ml_ಸಾಲು_lnum != 0)
	    ml_flush_ಸಾಲು(buf);
    }
#endif

    ಪ್ರತ್ಯಾಗಮ ml_append_int(buf, lnum, ಸಾಲು, len, flags);
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
    ಪೂರ್ಣಾಙ್ಕ
ml_append(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ಸಾಲು,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    ಪೂರ್ಣಾಙ್ಕ		newfile)	// flag, see above
{
    ಪ್ರತ್ಯಾಗಮ ml_append_flags(lnum, ಸಾಲು, len, newfile ? ML_APPEND_NEW : 0);
}

    ಪೂರ್ಣಾಙ್ಕ
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ಸಾಲು,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    ಪೂರ್ಣಾಙ್ಕ		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    ಯದಿ (curbuf->b_ml.ml_mfp == ರಿಕ್ತ && open_buffer(FALSE, ರಿಕ್ತ, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ ml_append_flush(curbuf, lnum, ಸಾಲು, len, flags);
}


#ಯದಿ defined(FEAT_SPELL) || defined(FEAT_QUICKFIX) || defined(FEAT_PROP_POPUP)

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
    ಪೂರ್ಣಾಙ್ಕ
ml_append_buf(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ಸಾಲು,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    ಪೂರ್ಣಾಙ್ಕ		newfile)	// flag, see above
{
    ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ ml_append_flush(buf, lnum, ಸಾಲು, len, newfile ? ML_APPEND_NEW : 0);
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
    ಪೂರ್ಣಾಙ್ಕ
ml_replace(linenr_T lnum, char_u *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ copy)
{
    colnr_T len = -1;

    ಯದಿ (ಸಾಲು != ರಿಕ್ತ)
	len = (colnr_T)STRLEN(ಸಾಲು);
    ಪ್ರತ್ಯಾಗಮ ml_replace_len(lnum, ಸಾಲು, len, FALSE, copy);
}

/*
 * Replace a line for the current buffer.  Like ml_replace() with:
 * "len_arg" is the length of the text, excluding NUL.
 * If "has_props" is TRUE then "line_arg" includes the text properties and
 * "len_arg" includes the NUL of the text and text properties.
 * When "copy" is TRUE copy the text into allocated memory, otherwise
 * "line_arg" must be allocated and will be consumed here.
 */
    ಪೂರ್ಣಾಙ್ಕ
ml_replace_len(
	linenr_T    lnum,
	char_u	    *ಸಾಲು_arg,
	colnr_T	    len_arg,
	ಪೂರ್ಣಾಙ್ಕ	    has_props,
	ಪೂರ್ಣಾಙ್ಕ	    copy)
{
    char_u *ಸಾಲು = ಸಾಲು_arg;
    colnr_T len = len_arg;

    ಯದಿ (ಸಾಲು == ರಿಕ್ತ)		// just checking...
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // When starting up, we might still need to create the memfile
    ಯದಿ (curbuf->b_ml.ml_mfp == ರಿಕ್ತ && open_buffer(FALSE, ರಿಕ್ತ, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (!has_props)
	++len;  // include the NUL after the text
    ಯದಿ (copy)
    {
	// copy the line to allocated memory
#ifdef FEAT_PROP_POPUP
	ಯದಿ (has_props)
	    ಸಾಲು = vim_memsave(ಸಾಲು, len);
	ಅನ್ಯಥಾ
#endif
	    ಸಾಲು = vim_strnsave(ಸಾಲು, len - 1);
	ಯದಿ (ಸಾಲು == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

#ifdef FEAT_NETBEANS_INTG
    ಯದಿ (netbeans_active())
    {
	netbeans_removed(curbuf, lnum, 0, (long)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, ಸಾಲು, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಾಲು));
    }
#endif
    ಯದಿ (curbuf->b_ml.ml_ಸಾಲು_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_ಸಾಲು(curbuf);

#ifdef FEAT_PROP_POPUP
	ಯದಿ (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#endif
    }

#ifdef FEAT_PROP_POPUP
    ಯದಿ (curbuf->b_has_textprop && !has_props)
    {
	size_t	oldtextlen = STRLEN(curbuf->b_ml.ml_ಸಾಲು_ptr) + 1;

	ಯದಿ (oldtextlen < (size_t)curbuf->b_ml.ml_ಸಾಲು_len)
	{
	    char_u *newline;
	    size_t textproplen = curbuf->b_ml.ml_ಸಾಲು_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    ಯದಿ (newline != ರಿಕ್ತ)
	    {
		mch_memmove(newline, ಸಾಲು, len);
		mch_memmove(newline + len, curbuf->b_ml.ml_ಸಾಲು_ptr
						    + oldtextlen, textproplen);
		vim_free(ಸಾಲು);
		ಸಾಲು = newline;
		len += (colnr_T)textproplen;
	    }
	}
    }
#endif

    ಯದಿ (curbuf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_ಸಾಲು_ptr);	// free allocated line

    curbuf->b_ml.ml_ಸಾಲು_ptr = ಸಾಲು;
    curbuf->b_ml.ml_ಸಾಲು_len = len;
    curbuf->b_ml.ml_ಸಾಲು_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_ಸಾಲು_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_ಸಾಲು_DIRTY) & ~ML_EMPTY;

    ಪ್ರತ್ಯಾಗಮ OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * Adjust text properties in line "lnum" for a deleted line.
 * When "above" is true this is the line above the deleted line, otherwise this
 * is the line below the deleted line.
 * "del_props[del_props_len]" are the properties of the deleted line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
adjust_text_props_for_delete(
	buf_T	    *buf,
	linenr_T    lnum,
	char_u	    *del_props,
	ಪೂರ್ಣಾಙ್ಕ	    del_props_len,
	ಪೂರ್ಣಾಙ್ಕ	    above)
{
    ಪೂರ್ಣಾಙ್ಕ		did_get_ಸಾಲು = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		done_del;
    ಪೂರ್ಣಾಙ್ಕ		done_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    DATA_BL	*dp;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_start;
    long	ಸಾಲು_size;
    ಪೂರ್ಣಾಙ್ಕ		this_props_len = 0;
    char_u	*text;
    size_t	textlen;
    ಪೂರ್ಣಾಙ್ಕ		found;

    ಪ್ರತಿ (done_del = 0; done_del < del_props_len; done_del += ಪರಿಮಾಣ(textprop_T))
    {
	mch_memmove(&prop_del, del_props + done_del, ಪರಿಮಾಣ(textprop_T));
	ಯದಿ ((above && (prop_del.tp_flags & TP_FLAG_CONT_PREV)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_NEXT))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_NEXT)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_PREV)))
	{
	    ಯದಿ (!did_get_ಸಾಲು)
	    {
		did_get_ಸಾಲು = TRUE;
		ಯದಿ ((hp = ml_find_ಸಾಲು(buf, lnum, ML_FIND)) == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ;

		dp = (DATA_BL *)(hp->bh_data);
		idx = lnum - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
		ಸಾಲು_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
		ಯದಿ (idx == 0)		// first line in block, text at the end
		    ಸಾಲು_size = dp->db_txt_end - ಸಾಲು_start;
		ಅನ್ಯಥಾ
		    ಸಾಲು_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK)
								  - ಸಾಲು_start;
		text = (char_u *)dp + ಸಾಲು_start;
		textlen = STRLEN(text) + 1;
		ಯದಿ ((long)textlen >= ಸಾಲು_size)
		{
		    // No properties on this line.
		    ಯದಿ (above)
			internal_error("no text property above deleted line");
		    ಅನ್ಯಥಾ
			internal_error("no text property below deleted line");
		    ಪ್ರತ್ಯಾಗಮ;
		}
		ಯದಿ ((long)textlen + (long)PROP_ಎಣಿಕೆ_SIZE > ಸಾಲು_size)
		{
		    internal_error("text property data too short");
		    ಪ್ರತ್ಯಾಗಮ;
		}

		uint16_t pc;

		mch_memmove(&pc, text + textlen, PROP_ಎಣಿಕೆ_SIZE);
		ಯದಿ (!text_prop_ಎಣಿಕೆ_valid(pc, (size_t)(ಸಾಲು_size - (long)textlen)))
		{
		    internal_error("text property count too large");
		    ಪ್ರತ್ಯಾಗಮ;
		}
		this_props_len = pc * (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(textprop_T);
	    }

	    found = FALSE;
	    {
		char_u *props_start = text + textlen + PROP_ಎಣಿಕೆ_SIZE;

		ಪ್ರತಿ (done_this = 0; done_this < this_props_len;
					       done_this += ಪರಿಮಾಣ(textprop_T))
		{
		    ಪೂರ್ಣಾಙ್ಕ		flag = above ? TP_FLAG_CONT_NEXT
							   : TP_FLAG_CONT_PREV;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_start + done_this,
							   ಪರಿಮಾಣ(textprop_T));
		    ಯದಿ ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			found = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_start + done_this, &prop_this,
							   ಪರಿಮಾಣ(textprop_T));
			ಭಙ್ಗ;
		    }
		}
	    }
	    ಯದಿ (!found)
	    {
		ಯದಿ (above)
		    internal_error("text property above deleted line not found");
		ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ml_delete_int(buf_T *buf, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ flags)
{
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;	    // number of entries in block
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		ರಾಶಿ_idx;
    ಪೂರ್ಣಾಙ್ಕ		text_start;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_start;
    long	ಸಾಲು_size;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
#ifdef FEAT_PROP_POPUP
    char_u	*textprop_save = ರಿಕ್ತ;
    long	textprop_len = 0;
#endif

    ಯದಿ (lowest_marked && lowest_marked > lnum)
	lowest_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    ಯದಿ (buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 1)	    // file becomes empty
    {
	ಯದಿ ((flags & ML_DEL_MESSAGE)
#ifdef FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_ಸಾಲುಗಳು
#endif
	   )
	    set_keep_ಸಂದೇಶ((char_u *)_(no_ಸಾಲುಗಳು_ಸಂದೇಶ), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_EMPTY;

	ಪ್ರತ್ಯಾಗಮ i;
    }

/*
 * Find the data block containing the line.
 * This also fills the stack with the blocks from the root to the data block.
 * This also releases any locked block..
 */
    mfp = buf->b_ml.ml_mfp;
    ಯದಿ (mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ ((hp = ml_find_ಸಾಲು(buf, lnum, ML_DELETE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    dp = (DATA_BL *)(hp->bh_data);
    // compute line count before the delete
    ಎಣಿಕೆ = (long)(buf->b_ml.ml_locked_ಮೇಲಿನ)
					- (long)(buf->b_ml.ml_locked_ಕೆಳಗಿನ) + 2;
    idx = lnum - buf->b_ml.ml_locked_ಕೆಳಗಿನ;

    --buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಸಾಲು_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
    ಯದಿ (idx == 0)		// first line in block, text at the end
	ಸಾಲು_size = dp->db_txt_end - ಸಾಲು_start;
    ಅನ್ಯಥಾ
	ಸಾಲು_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK) - ಸಾಲು_start;

#ifdef FEAT_NETBEANS_INTG
    ಯದಿ (netbeans_active())
	netbeans_removed(buf, lnum, 0, ಸಾಲು_size);
#endif
#ifdef FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    ಯದಿ (buf->b_has_textprop)
    {
	size_t	textlen = STRLEN((char_u *)dp + ಸಾಲು_start) + 1;

	textprop_len = ಸಾಲು_size - (long)textlen;
	ಯದಿ (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + ಸಾಲು_start + textlen,
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
    ಯದಿ (ಎಣಿಕೆ == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = ರಿಕ್ತ;

	ಪ್ರತಿ (ರಾಶಿ_idx = buf->b_ml.ml_ರಾಶಿ_ತುದಿ - 1; ರಾಶಿ_idx >= 0;
								  --ರಾಶಿ_idx)
	{
	    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_ರಾಶಿ[ರಾಶಿ_idx]);
	    idx = ip->ip_index;
	    ಯದಿ ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    ಯದಿ (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		ಗಚ್ಛ theend;
	    }
	    ಎಣಿಕೆ = --(pp->pb_ಎಣಿಕೆ);
	    ಯದಿ (ಎಣಿಕೆ == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (ಎಣಿಕೆ != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_pointer[idx], &pp->pb_pointer[idx + 1],
				      (size_t)(ಎಣಿಕೆ - idx) * ಪರಿಮಾಣ(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_ರಾಶಿ_ತುದಿ = ರಾಶಿ_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		ಯದಿ (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_ರಾಶಿ[buf->b_ml.ml_ರಾಶಿ_ತುದಿ].ip_ಮೇಲಿನ +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_ರಾಶಿ_ತುದಿ);

		ಭಙ್ಗ;
	    }
	}
	CHECK(ರಾಶಿ_idx < 0, _("deleted block 1?"));
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	text_start = dp->db_txt_start;
	mch_memmove((ವರ್ಣ *)dp + text_start + ಸಾಲು_size,
		  (ವರ್ಣ *)dp + text_start, (size_t)(ಸಾಲು_start - text_start));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	ಪ್ರತಿ (i = idx; i < ಎಣಿಕೆ - 1; ++i)
	    dp->db_index[i] = dp->db_index[i + 1] + ಸಾಲು_size;

	dp->db_free += ಸಾಲು_size + INDEX_SIZE;
	dp->db_txt_start += ಸಾಲು_size;
	--(dp->db_ಸಾಲು_ಎಣಿಕೆ);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#ifdef FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, ಸಾಲು_size
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    ಯದಿ (textprop_save != ರಿಕ್ತ)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_data;
	ಪೂರ್ಣಾಙ್ಕ	    props_bytes;

	mch_memmove(&pc, textprop_save, PROP_ಎಣಿಕೆ_SIZE);
	props_data = textprop_save + PROP_ಎಣಿಕೆ_SIZE;
	props_bytes = pc * (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(textprop_T);
	ಯದಿ (!text_prop_ಎಣಿಕೆ_valid(pc, (size_t)textprop_len))
	    props_bytes = 0;

	// Adjust text properties in the line above and below.
	ಯದಿ (lnum > 1)
	    adjust_text_props_for_delete(buf, lnum - 1,
					     props_data, props_bytes, TRUE);
	ಯದಿ (lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    adjust_text_props_for_delete(buf, lnum,
					    props_data, props_bytes, FALSE);
    }
    vim_free(textprop_save);
#endif
    ಪ್ರತ್ಯಾಗಮ ret;
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
    ಪೂರ್ಣಾಙ್ಕ
ml_delete(linenr_T lnum)
{
    ಪ್ರತ್ಯಾಗಮ ml_delete_flags(lnum, 0);
}

/*
 * Like ml_delete() but using flags (see ml_delete_int()).
 */
    ಪೂರ್ಣಾಙ್ಕ
ml_delete_flags(linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ml_flush_ಸಾಲು(curbuf);
    ಯದಿ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

#ifdef FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#endif

    ಪ್ರತ್ಯಾಗಮ ml_delete_int(curbuf, lnum, flags);
}

/*
 * set the DB_MARKED flag for line 'lnum'
 */
    ಶೂನ್ಯ
ml_setmarked(linenr_T lnum)
{
    bhdr_T    *hp;
    DATA_BL *dp;
				    // invalid line number
    ಯದಿ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
					       || curbuf->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;			    // give error message?

    ಯದಿ (lowest_marked == 0 || lowest_marked > lnum)
	lowest_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    ಯದಿ ((hp = ml_find_ಸಾಲು(curbuf, lnum, ML_FIND)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;		    // give error message?

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[lnum - curbuf->b_ml.ml_locked_ಕೆಳಗಿನ] |= DB_MARKED;
    curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
}

/*
 * find the first line with its DB_MARKED flag set
 */
    linenr_T
ml_firstmarked(ಶೂನ್ಯ)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (curbuf->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    ಪ್ರತಿ (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	ಯದಿ ((hp = ml_find_ಸಾಲು(curbuf, lnum, ML_FIND)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ (linenr_T)0;		    // give error message?

	dp = (DATA_BL *)(hp->bh_data);

	ಪ್ರತಿ (i = lnum - curbuf->b_ml.ml_locked_ಕೆಳಗಿನ;
			    lnum <= curbuf->b_ml.ml_locked_ಮೇಲಿನ; ++i, ++lnum)
	    ಯದಿ ((dp->db_index[i]) & DB_MARKED)
	    {
		(dp->db_index[i]) &= DB_INDEX_MASK;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		lowest_marked = lnum + 1;
		ಪ್ರತ್ಯಾಗಮ lnum;
	    }
    }

    ಪ್ರತ್ಯಾಗಮ (linenr_T) 0;
}

/*
 * clear all DB_MARKED flags
 */
    ಶೂನ್ಯ
ml_clearmarked(ಶೂನ್ಯ)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (curbuf->b_ml.ml_mfp == ರಿಕ್ತ)	    // nothing to do
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * The search starts with line lowest_marked.
     */
    ಪ್ರತಿ (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	ಯದಿ ((hp = ml_find_ಸಾಲು(curbuf, lnum, ML_FIND)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;		// give error message?

	dp = (DATA_BL *)(hp->bh_data);

	ಪ್ರತಿ (i = lnum - curbuf->b_ml.ml_locked_ಕೆಳಗಿನ;
			    lnum <= curbuf->b_ml.ml_locked_ಮೇಲಿನ; ++i, ++lnum)
	    ಯದಿ ((dp->db_index[i]) & DB_MARKED)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ml_flush_ಸಾಲು(buf_T *buf)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    char_u	*new_ಸಾಲು;
    char_u	*old_ಸಾಲು;
    colnr_T	new_len;
    ಪೂರ್ಣಾಙ್ಕ		old_len;
    ಪೂರ್ಣಾಙ್ಕ		extra;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		start;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  entered = FALSE;

    ಯದಿ (buf->b_ml.ml_ಸಾಲು_lnum == 0 || buf->b_ml.ml_mfp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;		// nothing to do

    ಯದಿ (buf->b_ml.ml_flags & ML_ಸಾಲು_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	ಯದಿ (entered)
	    ಪ್ರತ್ಯಾಗಮ;
	entered = TRUE;

	lnum = buf->b_ml.ml_ಸಾಲು_lnum;
	new_ಸಾಲು = buf->b_ml.ml_ಸಾಲು_ptr;

	hp = ml_find_ಸಾಲು(buf, lnum, ML_FIND);
	ಯದಿ (hp == ರಿಕ್ತ)
	    siemsg(e_cannot_find_ಸಾಲು_nr, lnum);
	ಅನ್ಯಥಾ
	{
	    dp = (DATA_BL *)(hp->bh_data);
	    idx = lnum - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
	    start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	    old_ಸಾಲು = (char_u *)dp + start;
	    ಯದಿ (idx == 0)	// line is last in block
		old_len = dp->db_txt_end - start;
	    ಅನ್ಯಥಾ		// text of previous line follows
		old_len = (dp->db_index[idx - 1] & DB_INDEX_MASK) - start;
	    new_len = buf->b_ml.ml_ಸಾಲು_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)dp->db_free >= extra)
	    {
#ಯದಿ defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		ಪೂರ್ಣಾಙ್ಕ old_prop_len = 0;
		ಯದಿ (buf->b_has_textprop)
		    old_prop_len = old_len - (ಪೂರ್ಣಾಙ್ಕ)STRLEN(old_ಸಾಲು) - 1;
#endif
		// if the length changes and there are following lines
		ಎಣಿಕೆ = buf->b_ml.ml_locked_ಮೇಲಿನ - buf->b_ml.ml_locked_ಕೆಳಗಿನ + 1;
		ಯದಿ (extra != 0 && idx < ಎಣಿಕೆ - 1)
		{
		    // move text of following lines
		    mch_memmove((ವರ್ಣ *)dp + dp->db_txt_start - extra,
				(ವರ್ಣ *)dp + dp->db_txt_start,
				(size_t)(start - dp->db_txt_start));

		    // adjust pointers of this and following lines
		    ಪ್ರತಿ (i = idx + 1; i < ಎಣಿಕೆ; ++i)
			dp->db_index[i] -= extra;
		}
		dp->db_index[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_start -= extra;

		// copy new line into the data block
		mch_memmove(old_ಸಾಲು - extra, new_ಸಾಲು, (size_t)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ಯದಿ defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		ಯದಿ (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (ಪೂರ್ಣಾಙ್ಕ)STRLEN(new_ಸಾಲು) - 1;
		}
		ಯದಿ (extra != 0)
		    ml_updatechunk(buf, lnum, (long)extra, ML_CHNK_UPDLINE);
#endif
	    }
	    ಅನ್ಯಥಾ
	    {
		/*
		 * Cannot do it in one data block: Delete and append.
		 * Append first, because ml_delete_int() cannot delete the
		 * last line in a buffer, which causes trouble for a buffer
		 * that has only one line.
		 * Don't forget to copy the mark!
		 */
		// How about handling errors???
		(ಶೂನ್ಯ)ml_append_int(buf, lnum, new_ಸಾಲು, new_len,
			 ((dp->db_index[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#ifdef FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#endif
			 );
		(ಶೂನ್ಯ)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_ಸಾಲು);

	entered = FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_ಸಾಲು_ptr);

    buf->b_ml.ml_flags &= ~(ML_ಸಾಲು_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_ಸಾಲು_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    ಸ್ಥಿರ bhdr_T *
ml_new_data(memfile_T *mfp, ಪೂರ್ಣಾಙ್ಕ negative, ಪೂರ್ಣಾಙ್ಕ page_ಎಣಿಕೆ)
{
    bhdr_T	*hp;
    DATA_BL	*dp;

    ಯದಿ ((hp = mf_new(mfp, negative, page_ಎಣಿಕೆ)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_id = DATA_ID;
    dp->db_txt_start = dp->db_txt_end = page_ಎಣಿಕೆ * mfp->mf_page_size;
    dp->db_free = dp->db_txt_start - HEADER_SIZE;
    dp->db_ಸಾಲು_ಎಣಿಕೆ = 0;

    ಪ್ರತ್ಯಾಗಮ hp;
}

/*
 * create a new, empty, pointer block
 */
    ಸ್ಥಿರ bhdr_T *
ml_new_ptr(memfile_T *mfp)
{
    bhdr_T	*hp;
    PTR_BL	*pp;

    ಯದಿ ((hp = mf_new(mfp, FALSE, 1)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_id = PTR_ID;
    pp->pb_ಎಣಿಕೆ = 0;
    pp->pb_ಎಣಿಕೆ_max = PB_ಎಣಿಕೆ_MAX(mfp);

    ಪ್ರತ್ಯಾಗಮ hp;
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
    ಸ್ಥಿರ bhdr_T *
ml_find_ಸಾಲು(buf_T *buf, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ action)
{
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    ಪೂರ್ಣಾಙ್ಕ		dirty;
    linenr_T	ಕೆಳಗಿನ, ಮೇಲಿನ;
    ಪೂರ್ಣಾಙ್ಕ		ತುದಿ;
    ಪೂರ್ಣಾಙ್ಕ		page_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    mfp = buf->b_ml.ml_mfp;

    /*
     * If there is a locked block check if the wanted line is in it.
     * If not, flush and release the locked block.
     * Don't do this for ML_INSERT_SAME, because the stack need to be updated.
     * Don't do this for ML_FLUSH, because we want to flush the locked block.
     * Don't do this when 'swapfile' is reset, we want to load all the blocks.
     */
    ಯದಿ (buf->b_ml.ml_locked)
    {
	ಯದಿ (ML_SIMPLE(action)
		&& buf->b_ml.ml_locked_ಕೆಳಗಿನ <= lnum
		&& buf->b_ml.ml_locked_ಮೇಲಿನ >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    ಯದಿ (action == ML_ಸೇರಿಸು)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_ಮೇಲಿನ);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_ಮೇಲಿನ);
	    }
	    ಪ್ರತ್ಯಾಗಮ (buf->b_ml.ml_locked);
	}

	mf_put(mfp, buf->b_ml.ml_locked, buf->b_ml.ml_flags & ML_LOCKED_DIRTY,
					    buf->b_ml.ml_flags & ML_LOCKED_POS);
	buf->b_ml.ml_locked = ರಿಕ್ತ;

	/*
	 * If lines have been added or deleted in the locked block, need to
	 * update the line count in pointer blocks.
	 */
	ಯದಿ (buf->b_ml.ml_locked_lineadd != 0)
	    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
    }

    ಯದಿ (action == ML_FLUSH)	    // nothing else to do
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    bnum = 1;			    // start at the root of the tree
    page_ಎಣಿಕೆ = 1;
    ಕೆಳಗಿನ = 1;
    ಮೇಲಿನ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಯದಿ (action == ML_FIND)	// first try stack entries
    {
	ಪ್ರತಿ (ತುದಿ = buf->b_ml.ml_ರಾಶಿ_ತುದಿ - 1; ತುದಿ >= 0; --ತುದಿ)
	{
	    ip = &(buf->b_ml.ml_ರಾಶಿ[ತುದಿ]);
	    ಯದಿ (ip->ip_ಕೆಳಗಿನ <= lnum && ip->ip_ಮೇಲಿನ >= lnum)
	    {
		bnum = ip->ip_bnum;
		ಕೆಳಗಿನ = ip->ip_ಕೆಳಗಿನ;
		ಮೇಲಿನ = ip->ip_ಮೇಲಿನ;
		buf->b_ml.ml_ರಾಶಿ_ತುದಿ = ತುದಿ;	// truncate stack at prev entry
		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (ತುದಿ < 0)
	    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;		// not found, start at the root
    }
    ಅನ್ಯಥಾ	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    ಪ್ರತಿ (;;)
    {
	ಯದಿ ((hp = mf_get(mfp, bnum, page_ಎಣಿಕೆ)) == ರಿಕ್ತ)
	    ಗಚ್ಛ error_noblock;

	/*
	 * update high for insert/delete
	 */
	ಯದಿ (action == ML_ಸೇರಿಸು)
	    ++ಮೇಲಿನ;
	ಅನ್ಯಥಾ ಯದಿ (action == ML_DELETE)
	    --ಮೇಲಿನ;

	dp = (DATA_BL *)(hp->bh_data);
	ಯದಿ (dp->db_id == DATA_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_ಕೆಳಗಿನ = ಕೆಳಗಿನ;
	    buf->b_ml.ml_locked_ಮೇಲಿನ = ಮೇಲಿನ;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    ಪ್ರತ್ಯಾಗಮ hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	ಯದಿ (pp->pb_id != PTR_ID)
	{
	    iemsg(e_pointer_block_id_wrong);
	    ಗಚ್ಛ error_block;
	}

	ಯದಿ ((ತುದಿ = ml_add_ರಾಶಿ(buf)) < 0)	// add new entry to stack
	    ಗಚ್ಛ error_block;
	ip = &(buf->b_ml.ml_ರಾಶಿ[ತುದಿ]);
	ip->ip_bnum = bnum;
	ip->ip_ಕೆಳಗಿನ = ಕೆಳಗಿನ;
	ip->ip_ಮೇಲಿನ = ಮೇಲಿನ;
	ip->ip_index = -1;		// index not known yet

	dirty = FALSE;
	ಪ್ರತಿ (idx = 0; idx < (ಪೂರ್ಣಾಙ್ಕ)pp->pb_ಎಣಿಕೆ; ++idx)
	{
	    t = pp->pb_pointer[idx].pe_ಸಾಲು_ಎಣಿಕೆ;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    ಯದಿ ((ಕೆಳಗಿನ += t) > lnum)
	    {
		ip->ip_index = idx;
		bnum = pp->pb_pointer[idx].pe_bnum;
		page_ಎಣಿಕೆ = pp->pb_pointer[idx].pe_page_ಎಣಿಕೆ;
		ಮೇಲಿನ = ಕೆಳಗಿನ - 1;
		ಕೆಳಗಿನ -= t;

		/*
		 * a negative block number may have been changed
		 */
		ಯದಿ (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    ಯದಿ (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_pointer[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		ಭಙ್ಗ;
	    }
	}
	ಯದಿ (idx >= (ಪೂರ್ಣಾಙ್ಕ)pp->pb_ಎಣಿಕೆ)	    // past the end: something wrong!
	{
	    ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		siemsg(e_ಸಾಲು_ಸಂಖ್ಯೆ_out_of_range_nr_past_the_end,
					      lnum - buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);

	    ಅನ್ಯಥಾ
		siemsg(e_ಸಾಲು_ಎಣಿಕೆ_wrong_in_block_nr, bnum);
	    ಗಚ್ಛ error_block;
	}
	ಯದಿ (action == ML_DELETE)
	{
	    pp->pb_pointer[idx].pe_ಸಾಲು_ಎಣಿಕೆ--;
	    dirty = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (action == ML_ಸೇರಿಸು)
	{
	    pp->pb_pointer[idx].pe_ಸಾಲು_ಎಣಿಕೆ++;
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
    ಯದಿ (action == ML_DELETE)
	ml_lineadd(buf, 1);
    ಅನ್ಯಥಾ ಯದಿ (action == ML_ಸೇರಿಸು)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ml_add_ರಾಶಿ(buf_T *buf)
{
    ಪೂರ್ಣಾಙ್ಕ		ತುದಿ;
    infoptr_T	*newstack;

    ತುದಿ = buf->b_ml.ml_ರಾಶಿ_ತುದಿ;

    // may have to increase the stack size
    ಯದಿ (ತುದಿ == buf->b_ml.ml_ರಾಶಿ_size)
    {
	CHECK(ತುದಿ > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_ರಾಶಿ_size + ರಾಶಿ_INCR);
	ಯದಿ (newstack == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (ತುದಿ > 0)
	    mch_memmove(newstack, buf->b_ml.ml_ರಾಶಿ,
					     (size_t)ತುದಿ * ಪರಿಮಾಣ(infoptr_T));
	vim_free(buf->b_ml.ml_ರಾಶಿ);
	buf->b_ml.ml_ರಾಶಿ = newstack;
	buf->b_ml.ml_ರಾಶಿ_size += ರಾಶಿ_INCR;
    }

    buf->b_ml.ml_ರಾಶಿ_ತುದಿ++;
    ಪ್ರತ್ಯಾಗಮ ತುದಿ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ml_lineadd(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    ಪ್ರತಿ (idx = buf->b_ml.ml_ರಾಶಿ_ತುದಿ - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_ರಾಶಿ[idx]);
	ಯದಿ ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	pp = (PTR_BL *)(hp->bh_data);	// must be pointer block
	ಯದಿ (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_pointer_block_id_wrong_two);
	    ಭಙ್ಗ;
	}
	pp->pb_pointer[ip->ip_index].pe_ಸಾಲು_ಎಣಿಕೆ += ಎಣಿಕೆ;
	ip->ip_ಮೇಲಿನ += ಎಣಿಕೆ;
	mf_put(mfp, hp, TRUE, FALSE);
    }
}

#ಯದಿ defined(HAVE_READLINK)
/*
 * Resolve a symlink in the last component of a file name.
 * Note that f_resolve() does it for every part of the path, we don't do that
 * here.
 * If it worked returns OK and the resolved link in "buf[MAXPATHL]".
 * Otherwise returns FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
resolve_symlink(char_u *fname, char_u *buf)
{
    char_u	tmp[MAXPATHL];
    ಪೂರ್ಣಾಙ್ಕ		ret;
    ಪೂರ್ಣಾಙ್ಕ		depth = 0;

    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    ಪ್ರತಿ (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	ಯದಿ (++depth == 100)
	{
	    semsg(_(e_symlink_loop_for_str), fname);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ret = readlink((ವರ್ಣ *)tmp, (ವರ್ಣ *)buf, MAXPATHL - 1);
	ಯದಿ (ret <= 0)
	{
	    ಯದಿ (errno == EINVAL || errno == ENOENT)
	    {
		// Found non-symlink or not existing file, stop here.
		// When at the first level use the unmodified name, skip the
		// call to vim_FullName().
		ಯದಿ (depth == 1)
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		// Use the resolved name in tmp[].
		ಭಙ್ಗ;
	    }

	    // There must be some error reading links, use original name.
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	buf[ret] = NUL;

	/*
	 * Check whether the symlink is relative or absolute.
	 * If it's relative, build a new path based on the directory
	 * portion of the filename (if any) and the path the symlink
	 * points to.
	 */
	ಯದಿ (mch_isFullName(buf))
	    STRCPY(tmp, buf);
	ಅನ್ಯಥಾ
	{
	    char_u *tail;

	    tail = gettail(tmp);
	    ಯದಿ (STRLEN(tail) + STRLEN(buf) >= MAXPATHL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    STRCPY(tail, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    ಪ್ರತ್ಯಾಗಮ vim_FullName(tmp, buf, MAXPATHL, TRUE);
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
    ಯದಿ (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#endif

#ಯದಿ defined(UNIX) || defined(MSWIN)  // Need _very_ long file names
    ಪೂರ್ಣಾಙ್ಕ		len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(dir_name);

    s = dir_name + len;
    ಯದಿ (after_pathsep(dir_name, s) && len > 1 && s[-1] == s[-2])
    {			       // Ends with '//', Use Full path
	r = ರಿಕ್ತ;
	ಯದಿ ((s = make_percent_swname(dir_name, s, fname_res)) != ರಿಕ್ತ)
	{
	    r = modname(s, (char_u *)".swp", FALSE);
	    vim_free(s);
	}
	ಪ್ರತ್ಯಾಗಮ r;
    }
#endif

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#ಯದಿ defined(VMS)
	    "_swp",
#ಅನ್ಯಥಾ
	    ".swp",
#endif
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    ಯದಿ (r == ರಿಕ್ತ)	    // out of memory
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    s = get_file_in_dir(r, dir_name);
    vim_free(r);
    ಪ್ರತ್ಯಾಗಮ s;
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

    ಯದಿ (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (size_t)(tail.string - fname) + tail.length);
    ಅನ್ಯಥಾ
    {
	size_t	dname_len = STRLEN(dname);

	ಯದಿ (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    ಯದಿ (tail.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    tail.string, tail.length, TRUE, &retval);
	    ಅನ್ಯಥಾ
	    {
		ಪೂರ್ಣಾಙ್ಕ	    save_char;
		string_T    tmp;

		save_char = *tail.string;
		*tail.string = NUL;
		concat_fnames(fname, (size_t)(tail.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*tail.string = save_char;
		ಯದಿ (tmp.string == ರಿಕ್ತ)	    // out of memory
		    retval.string = ರಿಕ್ತ;
		ಅನ್ಯಥಾ
		{
		    concat_fnames(tmp.string, tmp.length,
			tail.string, tail.length, TRUE, &retval);
		    vim_free(tmp.string);
		}
	    }
	}
	ಅನ್ಯಥಾ
	    concat_fnames(dname, dname_len, tail.string, tail.length,
		TRUE, &retval);
    }

#ifdef MSWIN
    ಯದಿ (retval.string != ರಿಕ್ತ)
    {
	char_u	*t;

	ಪ್ರತಿ (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    ಯದಿ (*t == ':')
		*t = '%';
    }
#endif

    ಪ್ರತ್ಯಾಗಮ retval.string;
}

/*
 * Print the ATTENTION message: info about an existing swap file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
attention_message(
    buf_T   *buf,	// buffer being edited
    char_u  *fname)	// swap file name
{
    stat_T	st;
    time_t	swap_mtime;

    ++no_wait_return;
    (ಶೂನ್ಯ)emsg(_(e_attention));
    ಸಂದೇಶ_puts(_("\nFound a swap file by the name \""));
    ಸಂದೇಶ_home_replace(fname);
    ಸಂದೇಶ_puts("\"\n");
    swap_mtime = swapfile_info(fname);
    ಸಂದೇಶ_puts(_("While opening file \""));
    ಸಂದೇಶ_outtrans(buf->b_fname);
    ಸಂದೇಶ_puts("\"\n");
    ಯದಿ (mch_stat((ವರ್ಣ *)buf->b_fname, &st) == -1)
    {
	ಸಂದೇಶ_puts(_("      CANNOT BE FOUND"));
    }
    ಅನ್ಯಥಾ
    {
	ಸಂದೇಶ_puts(_("             dated: "));
	ಸಂದೇಶ_puts(get_ctime(st.st_mtime, TRUE));
	ಯದಿ (swap_mtime != 0 && st.st_mtime > swap_mtime)
	    ಸಂದೇಶ_puts(_("      NEWER than swap file!\n"));
    }
    // Some of these messages are long to allow translation to
    // other languages.
    ಸಂದೇಶ_puts(_("\n(1) Another program may be editing the same file.  If this is the case,\n    be careful not to end up with two different instances of the same\n    file when making changes.  Quit, or continue with caution.\n"));
    ಸಂದೇಶ_puts(_("(2) An edit session for this file crashed.\n"));
    ಸಂದೇಶ_puts(_("    If this is the case, use \":recover\" or \"vim -r "));
    ಸಂದೇಶ_outtrans(buf->b_fname);
    ಸಂದೇಶ_puts(_("\"\n    to recover the changes (see \":help recovery\").\n"));
    ಸಂದೇಶ_puts(_("    If you did this already, delete the swap file \""));
    ಸಂದೇಶ_outtrans(fname);
    ಸಂದೇಶ_puts(_("\"\n    to avoid this message.\n"));
    cmdline_row = ಸಂದೇಶ_row;
    --no_wait_return;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    SEA_CHOICE_NONE = 0,
    SEA_CHOICE_READONLY = 1,
    SEA_CHOICE_EDIT = 2,
    SEA_CHOICE_RECOVER = 3,
    SEA_CHOICE_DELETE = 4,
    SEA_CHOICE_QUIT = 5,
    SEA_CHOICE_ABORT = 6
} sea_choice_T;

#ಯದಿ defined(FEAT_EVAL)
/*
 * Trigger the SwapExists autocommands.
 * Returns a value for equivalent to do_dialog().
 */
    ಸ್ಥಿರ sea_choice_T
do_swapexists(buf_T *buf, char_u *fname)
{
    set_vim_var_string(VV_SWAPNAME, fname, -1);
    set_vim_var_string(VV_SWAPCHOICE, ರಿಕ್ತ, -1);

    // Trigger SwapExists autocommands with <afile> set to the file being
    // edited.  Disallow changing directory here.
    ++allbuf_lock;
    apply_autocmds(EVENT_SWAPEXISTS, buf->b_fname, ರಿಕ್ತ, FALSE, ರಿಕ್ತ);
    --allbuf_lock;

    set_vim_var_string(VV_SWAPNAME, ರಿಕ್ತ, -1);

    ವಿಕಲ್ಪ (*get_vim_var_str(VV_SWAPCHOICE))
    {
	ಪಕ್ಷ 'o': ಪ್ರತ್ಯಾಗಮ SEA_CHOICE_READONLY;
	ಪಕ್ಷ 'e': ಪ್ರತ್ಯಾಗಮ SEA_CHOICE_EDIT;
	ಪಕ್ಷ 'r': ಪ್ರತ್ಯಾಗಮ SEA_CHOICE_RECOVER;
	ಪಕ್ಷ 'd': ಪ್ರತ್ಯಾಗಮ SEA_CHOICE_DELETE;
	ಪಕ್ಷ 'q': ಪ್ರತ್ಯಾಗಮ SEA_CHOICE_QUIT;
	ಪಕ್ಷ 'a': ಪ್ರತ್ಯಾಗಮ SEA_CHOICE_ABORT;
    }

    ಪ್ರತ್ಯಾಗಮ SEA_CHOICE_NONE;
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
    ಸ್ಥಿರ char_u *
findswapname(
    buf_T	*buf,
    char_u	**dirp,		// pointer to list of directories
    char_u	*old_fname)	// don't give warning for this file name
{
    char_u	*fname;
    ಪೂರ್ಣಾಙ್ಕ		n;
    char_u	*dir_name;
#ifdef AMIGA
    BPTR	fh;
#endif
    ಪೂರ್ಣಾಙ್ಕ		r;
    char_u	*buf_fname = buf->b_fname;

#ಯದಿ !defined(UNIX)
# define CREATE_DUMMY_FILE
    FILE	*dummyfd = ರಿಕ್ತ;

# ifdef MSWIN
    ಯದಿ (buf_fname != ರಿಕ್ತ && !mch_isFullName(buf_fname)
				       && vim_strchr(gettail(buf_fname), ':'))
    {
	char_u *t;

	buf_fname = vim_strsave(buf_fname);
	ಯದಿ (buf_fname == ರಿಕ್ತ)
	    buf_fname = buf->b_fname;
	ಅನ್ಯಥಾ
	    ಪ್ರತಿ (t = gettail(buf_fname); *t != NUL; MB_PTR_ADV(t))
		ಯದಿ (*t == ':')
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
    ಯದಿ (!(buf->b_p_sn || buf->b_shortname) && buf_fname != ರಿಕ್ತ
					     && mch_getperm(buf_fname) < 0)
	dummyfd = mch_fopen((ವರ್ಣ *)buf_fname, "w");
#endif

    /*
     * Isolate a directory name from *dirp and put it in dir_name.
     * First allocate some memory to put the directory name in.
     */
    dir_name = alloc(STRLEN(*dirp) + 1);
    ಯದಿ (dir_name == ರಿಕ್ತ)
	*dirp = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)copy_option_part(dirp, dir_name, 31000, ",");

    /*
     * we try different names until we find one that does not exist yet
     */
    ಯದಿ (dir_name == ರಿಕ್ತ)	    // out of memory
	fname = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	fname = makeswapname(buf_fname, buf->b_ffname, buf, dir_name);

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (fname == ರಿಕ್ತ)	// must be out of memory
	    ಭಙ್ಗ;
	ಯದಿ ((n = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(fname)) == 0)	// safety check
	{
	    VIM_CLEAR(fname);
	    ಭಙ್ಗ;
	}
#ಯದಿ defined(UNIX)
/*
 * Some systems have a MS-DOS compatible filesystem that use 8.3 character
 * file names. If this is the first try and the swap file name does not fit in
 * 8.3, detect if this is the case, set shortname and try again.
 */
	ಯದಿ (fname[n - 2] == 'w' && fname[n - 1] == 'p'
					&& !(buf->b_p_sn || buf->b_shortname))
	{
	    char_u	    *tail;
	    char_u	    *fname2;
	    stat_T	    s1, s2;
	    ಪೂರ್ಣಾಙ್ಕ		    f1, f2;
	    ಪೂರ್ಣಾಙ್ಕ		    created1 = FALSE, created2 = FALSE;
	    ಪೂರ್ಣಾಙ್ಕ		    same = FALSE;

	    /*
	     * Check if swapfile name does not fit in 8.3:
	     * It either contains two dots, is longer than 8 chars, or starts
	     * with a dot.
	     */
	    tail = gettail(buf_fname);
	    ಯದಿ (       vim_strchr(tail, '.') != ರಿಕ್ತ
		    || STRLEN(tail) > (size_t)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		ಯದಿ (fname2 != ರಿಕ್ತ)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    ಯದಿ (vim_strchr(tail, '.') != ರಿಕ್ತ)
			fname2[n - 1] = 'x';
		    ಅನ್ಯಥಾ ಯದಿ (*gettail(fname) == '.')
		    {
			fname2[n] = 'x';
			fname2[n + 1] = NUL;
		    }
		    ಅನ್ಯಥಾ
			fname2[n - 5] += 1;
		    /*
		     * may need to create the files to be able to use mch_stat()
		     */
		    f1 = mch_open((ವರ್ಣ *)fname, O_RDONLY | O_EXTRA, 0);
		    ಯದಿ (f1 < 0)
		    {
			f1 = mch_open_rw((ವರ್ಣ *)fname,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			created1 = TRUE;
		    }
		    ಯದಿ (f1 >= 0)
		    {
			f2 = mch_open((ವರ್ಣ *)fname2, O_RDONLY | O_EXTRA, 0);
			ಯದಿ (f2 < 0)
			{
			    f2 = mch_open_rw((ವರ್ಣ *)fname2,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			    created2 = TRUE;
			}
			ಯದಿ (f2 >= 0)
			{
			    /*
			     * Both files exist now. If mch_stat() returns the
			     * same device and inode they are the same file.
			     */
			    ಯದಿ (mch_fstat(f1, &s1) != -1
				    && mch_fstat(f2, &s2) != -1
				    && s1.st_dev == s2.st_dev
				    && s1.st_ino == s2.st_ino)
				same = TRUE;
			    close(f2);
			    ಯದಿ (created2)
				mch_remove(fname2);
			}
			close(f1);
			ಯದಿ (created1)
			    mch_remove(fname);
		    }
		    vim_free(fname2);
		    ಯದಿ (same)
		    {
			buf->b_shortname = true;
			vim_free(fname);
			fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
			ಅನುವರ್ತ;	// try again with b_shortname set
		    }
		}
	    }
	}
#endif
	/*
	 * check if the swapfile already exists
	 */
	ಯದಿ (mch_getperm(fname) < 0)	// it does not exist
	{
#ifdef HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    ಯದಿ (mch_lstat((ವರ್ಣ *)fname, &sb) < 0)
#ಅನ್ಯಥಾ
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (long)MODE_NEWFILE);
	    /*
	     * on the Amiga mch_getperm() will return -1 when the file exists
	     * but is being used by another program. This happens if you edit
	     * a file twice.
	     */
	    ಯದಿ (fh != (BPTR)ರಿಕ್ತ)	// can open file, OK
	    {
		Close(fh);
		mch_remove(fname);
		ಭಙ್ಗ;
	    }
	    ಯದಿ (IoErr() != ERROR_OBJECT_IN_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#endif
		ಭಙ್ಗ;
	}

	/*
	 * A file name equal to old_fname is OK to use.
	 */
	ಯದಿ (old_fname != ರಿಕ್ತ && fnamecmp(fname, old_fname) == 0)
	    ಭಙ್ಗ;

	/*
	 * get here when file already exists
	 */
	ಯದಿ (fname[n - 2] == 'w' && fname[n - 1] == 'p')	// first try
	{
	    /*
	     * on MS-DOS compatible filesystems (e.g. messydos) file.doc.swp
	     * and file.doc are the same file. To guess if this problem is
	     * present try if file.doc.swx exists. If it does, we set
	     * buf->b_shortname and try file_doc.swp (dots replaced by
	     * underscores for this file), and try again. If it doesn't we
	     * assume that "file.doc.swp" already exists.
	     */
	    ಯದಿ (!(buf->b_p_sn || buf->b_shortname))	// not tried yet
	    {
		fname[n - 1] = 'x';
		r = mch_getperm(fname);		// try "file.swx"
		fname[n - 1] = 'p';
		ಯದಿ (r >= 0)		    // "file.swx" seems to exist
		{
		    buf->b_shortname = true;
		    vim_free(fname);
		    fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
		    ಅನುವರ್ತ;	    // try again with '.' replaced with '_'
		}
	    }
	    /*
	     * If we get here the ".swp" file really exists.
	     * Give an error message, unless recovering, no file name, we are
	     * viewing a help file or when the path of the file is different
	     * (happens when all .swp files are in one directory).
	     */
	    ಯದಿ (!recoverymode && buf_fname != ರಿಕ್ತ
				&& !buf->b_help
				&& !(buf->b_flags & (BF_DUMMY | BF_NO_SEA)))
	    {
		ಪೂರ್ಣಾಙ್ಕ		fd;
		ಸಂರಚನಾ block0	b0;
		ಪೂರ್ಣಾಙ್ಕ		differ = FALSE;

		/*
		 * Try to read block 0 from the swap file to get the original
		 * file name (and inode number).
		 */
		fd = mch_open((ವರ್ಣ *)fname, O_RDONLY | O_EXTRA, 0);
		ಯದಿ (fd >= 0)
		{
		    ಯದಿ (read_eintr(fd, &b0, ಪರಿಮಾಣ(b0)) == ಪರಿಮಾಣ(b0))
		    {
			/*
			 * If the swapfile has the same directory as the
			 * buffer don't compare the directory names, they can
			 * have a different mountpoint.
			 */
			ಯದಿ (b0.b0_flags & B0_SAME_DIR)
			{
			    ಯದಿ (fnamecmp(gettail(buf->b_ffname),
						   gettail(b0.b0_fname)) != 0
				    || !same_directory(fname, buf->b_ffname))
			    {
#ifdef CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				ಯದಿ (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
#endif
				    differ = TRUE;
			    }
			}
			ಅನ್ಯಥಾ
			{
			    /*
			     * The name in the swap file may be
			     * "~user/path/file".  Expand it first.
			     */
			    expand_env(b0.b0_fname, NameBuff, MAXPATHL);
#ifdef CHECK_INODE
			    ಯದಿ (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
				differ = TRUE;
#ಅನ್ಯಥಾ
			    ಯದಿ (fnamecmp(NameBuff, buf->b_ffname) != 0)
				differ = TRUE;
#endif
			}
		    }
		    close(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		ಯದಿ (differ == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == ರಿಕ್ತ)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#ifdef CREATE_DUMMY_FILE
		    ಪೂರ್ಣಾಙ್ಕ		 did_use_dummy = FALSE;

		    // Avoid getting a warning for the file being created
		    // outside of Vim, it was created at the start of this
		    // function.  Delete the file now, because Vim might exit
		    // here if the window is closed.
		    ಯದಿ (dummyfd != ರಿಕ್ತ)
		    {
			fclose(dummyfd);
			dummyfd = ರಿಕ್ತ;
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
		    ಯದಿ (mch_stat((ವರ್ಣ *)buf->b_fname, &st) == 0
						  && swapfile_unchanged(fname))
		    {
			choice = SEA_CHOICE_DELETE;
			ಯದಿ (p_verbose > 0)
			    verb_ಸಂದೇಶ(_("Found a swap file that is not useful, deleting it"));
		    }

#ಯದಿ defined(FEAT_EVAL)
		    /*
		     * If there is an SwapExists autocommand and we can handle
		     * the response, trigger it.  It may return 0 to ask the
		     * user anyway.
		     */
		    ಯದಿ (choice == SEA_CHOICE_NONE
			    && swap_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#endif

		    ಯದಿ (choice == SEA_CHOICE_NONE
					 && swap_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    ಯದಿ (choice == SEA_CHOICE_NONE)
		    {
#ifdef FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			ಯದಿ (gui.starting && !gui.in_use)
			    gui_start(ರಿಕ್ತ);
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

#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    ಯದಿ (swap_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    prefix = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			size_t	    message_size;
			string_T    message;
			char_u	    *tofree;
			ಪೂರ್ಣಾಙ್ಕ	    dialog_result;

			prefix.length = STRLEN(prefix.string);
			suffix.length = STRLEN(suffix.string);
			message_size = prefix.length
				+ STRLEN(fname)
				+ suffix.length + 5;
			message.string = tofree = alloc(message_size);
			ಯದಿ (message.string != ರಿಕ್ತ)
			{
			    STRCPY(message.string, prefix.string);
			    message.length = prefix.length;
			    message.length += home_replace(ರಿಕ್ತ, fname,
				message.string + message.length, (ಪೂರ್ಣಾಙ್ಕ)(message_size - message.length), TRUE);
			    STRCPY(message.string + message.length, suffix.string);
			}
			ಅನ್ಯಥಾ
			    message.string = (char_u *)_("Swap file already exists!");
			dialog_result = do_dialog(VIM_WARNING,
				    (char_u *)_("VIM - ATTENTION"),
				    message.string,
# ifdef HAVE_PROCESS_STILL_RUNNING
				    process_still_running
					? (char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Quit\n&Abort") :
# endif
					(char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Delete it\n&Quit\n&Abort"), 1, ರಿಕ್ತ, FALSE);

# ifdef HAVE_PROCESS_STILL_RUNNING
			ಯದಿ (process_still_running && dialog_result >= 4)
			    // compensate for missing "Delete it" button
			    dialog_result++;
# endif
			choice = dialog_result;
			vim_free(tofree);

			// pretend screen didn't scroll, need redraw anyway
			ಸಂದೇಶ_scrolled = 0;
			redraw_all_later(UPD_NOT_VALID);
		    }
#endif

		    ವಿಕಲ್ಪ (choice)
		    {
			ಪಕ್ಷ SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    ಭಙ್ಗ;
			ಪಕ್ಷ SEA_CHOICE_EDIT:
			    ಭಙ್ಗ;
			ಪಕ್ಷ SEA_CHOICE_RECOVER:
			    swap_exists_action = SEA_RECOVER;
			    ಭಙ್ಗ;
			ಪಕ್ಷ SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    ಭಙ್ಗ;
			ಪಕ್ಷ SEA_CHOICE_QUIT:
			    swap_exists_action = SEA_QUIT;
			    ಭಙ್ಗ;
			ಪಕ್ಷ SEA_CHOICE_ABORT:
			    swap_exists_action = SEA_QUIT;
			    got_int = TRUE;
			    ಭಙ್ಗ;
			ಪಕ್ಷ SEA_CHOICE_NONE:
			    ಸಂದೇಶ_puts("\n");
			    ಯದಿ (ಸಂದೇಶ_silent == 0)
				// call wait_return() later
				need_wait_return = TRUE;
			    ಭಙ್ಗ;
		    }

		    // If the file was deleted this fname can be used.
		    ಯದಿ (choice != SEA_CHOICE_NONE && mch_getperm(fname) < 0)
			ಭಙ್ಗ;

#ifdef CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    ಯದಿ (did_use_dummy)
			dummyfd = mch_fopen((ವರ್ಣ *)buf_fname, "w");
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
	ಯದಿ (fname[n - 1] == 'a')	// ".s?a"
	{
	    ಯದಿ (fname[n - 2] == 'a')    // ".saa": tried enough, give up
	    {
		emsg(_(e_too_many_swap_files_found));
		VIM_CLEAR(fname);
		ಭಙ್ಗ;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#ifdef CREATE_DUMMY_FILE
    ಯದಿ (dummyfd != ರಿಕ್ತ)	// file has been created temporarily
    {
	fclose(dummyfd);
	mch_remove(buf_fname);
    }
#endif
#ifdef MSWIN
    ಯದಿ (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#endif
    ಪ್ರತ್ಯಾಗಮ fname;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
b0_magic_wrong(ZERO_BL *b0p)
{
    ಪ್ರತ್ಯಾಗಮ (b0p->b0_magic_long != (long)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (ಪೂರ್ಣಾಙ್ಕ)B0_MAGIC_INT
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
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
    ಪೂರ್ಣಾಙ್ಕ		retval_c;	    // flag: buf_c valid
    ಪೂರ್ಣಾಙ್ಕ		retval_s;	    // flag: buf_s valid

    ಯದಿ (mch_stat((ವರ್ಣ *)fname_c, &st) == 0)
	ino_c = (ino_t)st.st_ino;

    /*
     * First we try to get the inode from the file name, because the inode in
     * the swap file may be outdated.  If that fails (e.g. this path is not
     * valid on this machine), use the inode from block 0.
     */
    ಯದಿ (mch_stat((ವರ್ಣ *)fname_s, &st) == 0)
	ino_s = (ino_t)st.st_ino;
    ಅನ್ಯಥಾ
	ino_s = (ino_t)ino_block0;

    ಯದಿ (ino_c && ino_s)
	ಪ್ರತ್ಯಾಗಮ (ino_c != ino_s);

    /*
     * One of the inode numbers is unknown, try a forced vim_FullName() and
     * compare the file names.
     */
    retval_c = vim_FullName(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_FullName(fname_s, buf_s, MAXPATHL, TRUE);
    ಯದಿ (retval_c == OK && retval_s == OK)
	ಪ್ರತ್ಯಾಗಮ STRCMP(buf_c, buf_s) != 0;

    /*
     * Can't compare inodes or file names, guess that the files are different,
     * unless both appear not to exist at all, then compare with the file name
     * in the swap file.
     */
    ಯದಿ (ino_s == 0 && ino_c == 0 && retval_c == FAIL && retval_s == FAIL)
	ಪ್ರತ್ಯಾಗಮ STRCMP(fname_c, fname_s) != 0;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}
#endif // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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

    ಸ್ಥಿರ long
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

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Set the flags in the first block of the swap file:
 * - file is modified or not: buf->b_changed
 * - 'fileformat'
 * - 'fileencoding'
 */
    ಶೂನ್ಯ
ml_setflags(buf_T *buf)
{
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    ಯದಿ (!buf->b_ml.ml_mfp)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (hp = buf->b_ml.ml_mfp->mf_used_last; hp != ರಿಕ್ತ; hp = hp->bh_prev)
    {
	ಯದಿ (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_data);
	    b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_MASK)
						  | (get_fileformat(buf) + 1);
	    add_b0_fenc(b0p, buf);
	    hp->bh_flags |= BH_DIRTY;
	    mf_sync(buf->b_ml.ml_mfp, MFS_ZERO);
	    ಭಙ್ಗ;
	}
    }
}

#ಯದಿ defined(FEAT_CRYPT)
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
    ಪೂರ್ಣಾಙ್ಕ		text_len;
    cryptstate_T *state;

    ಯದಿ (dp->db_id != DATA_ID)
	ಪ್ರತ್ಯಾಗಮ data;

    state = ml_crypt_prepare(mfp, offset, FALSE);
    ಯದಿ (state == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ data;

    new_data = alloc(size);
    ಯದಿ (new_data == ರಿಕ್ತ)
    {
	crypt_free_state(state);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    head_end = (char_u *)(&dp->db_index[dp->db_ಸಾಲು_ಎಣಿಕೆ]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = size - dp->db_txt_start;

    // Copy the header and the text.
    mch_memmove(new_data, dp, head_end - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, text_start, text_len, new_data + dp->db_txt_start,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    ಯದಿ (head_end < text_start)
	vim_memset(new_data + (head_end - data), 0, text_start - head_end);

    ಪ್ರತ್ಯಾಗಮ new_data;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    ಶೂನ್ಯ
ml_decrypt_data(
    memfile_T	*mfp,
    char_u	*data,
    off_T	offset,
    unsigned	size)
{
    DATA_BL	*dp = (DATA_BL *)data;
    char_u	*head_end;
    char_u	*text_start;
    ಪೂರ್ಣಾಙ್ಕ		text_len;
    cryptstate_T *state;

    ಯದಿ (dp->db_id != DATA_ID)
	ಪ್ರತ್ಯಾಗಮ;

    head_end = (char_u *)(&dp->db_index[dp->db_ಸಾಲು_ಎಣಿಕೆ]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = dp->db_txt_end - dp->db_txt_start;

    ಯದಿ (head_end > text_start || dp->db_txt_start > size
	    || dp->db_txt_end > size)
	ಪ್ರತ್ಯಾಗಮ;  // data was messed up

    state = ml_crypt_prepare(mfp, offset, TRUE);
    ಯದಿ (state == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Decrypt the text in place.
    crypt_decode_inplace(state, text_start, text_len, FALSE);
    crypt_free_state(state);
}

/*
 * Prepare for encryption/decryption, using the key, seed and offset.
 * Return an allocated cryptstate_T *.
 * Note: Encryption not supported for SODIUM
 */
    ಸ್ಥಿರ cryptstate_T *
ml_crypt_prepare(memfile_T *mfp, off_T offset, ಪೂರ್ಣಾಙ್ಕ reading)
{
    buf_T	*buf = mfp->mf_buffer;
    char_u	salt[50];
    ಪೂರ್ಣಾಙ್ಕ		method_nr;
    char_u	*ಕೀಲಿ;
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    ಯದಿ (reading && mfp->mf_old_ಕೀಲಿ != ರಿಕ್ತ)
    {
	// Reading back blocks with the previous key/method/seed.
	method_nr = mfp->mf_old_cm;
	ಕೀಲಿ = mfp->mf_old_ಕೀಲಿ;
	arg.cat_seed = mfp->mf_old_seed;
    }
    ಅನ್ಯಥಾ
    {
	method_nr = crypt_get_method_nr(buf);
	ಕೀಲಿ = buf->b_p_ಕೀಲಿ;
	arg.cat_seed = mfp->mf_seed;
    }

    ಯದಿ (*ಕೀಲಿ == NUL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (crypt_may_close_swapfile(buf, ಕೀಲಿ, method_nr))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (method_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((ವರ್ಣ *)salt, ಪರಿಮಾಣ(salt), "%s%ld", ಕೀಲಿ, (long)offset);
	arg.cat_seed = ರಿಕ್ತ;
	arg.cat_init_from_file = FALSE;

	ಪ್ರತ್ಯಾಗಮ crypt_create(method_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((ವರ್ಣ *)salt, ಪರಿಮಾಣ(salt), "%ld", (long)offset);

    arg.cat_salt = salt;
    arg.cat_salt_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(salt);
    arg.cat_seed_len = MF_SEED_LEN;
    arg.cat_add_len = 0;
    arg.cat_add = ರಿಕ್ತ;
    arg.cat_init_from_file = FALSE;

    ಪ್ರತ್ಯಾಗಮ crypt_create(method_nr, ಕೀಲಿ, &arg);
}

#endif


#ಯದಿ defined(FEAT_BYTEOFF)

# define MLCS_MAXL 800	// max no of lines in chunk
# define MLCS_MINL 400   // should be half of MLCS_MAXL

/*
 * Keep information for finding byte offset of a line, updtype may be one of:
 * ML_CHNK_ADDLINE: Add len to parent chunk, possibly splitting it
 *	   Careful: ML_CHNK_ADDLINE may cause ml_find_line() to be called.
 * ML_CHNK_DELLINE: Subtract len from parent chunk, possibly deleting it
 * ML_CHNK_UPDLINE: Add len to parent chunk, as a signed entity.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ml_updatechunk(
    buf_T	*buf,
    linenr_T	ಸಾಲು,
    long	len,
    ಪೂರ್ಣಾಙ್ಕ		updtype)
{
    ಸ್ಥಿರ buf_T	*ml_upd_lastbuf = ರಿಕ್ತ;
    ಸ್ಥಿರ linenr_T	ml_upd_lastcurline;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    ಪೂರ್ಣಾಙ್ಕ			curix = ml_upd_lastcurix;
    long		size;
    chunksize_T		*curchnk;
    ಪೂರ್ಣಾಙ್ಕ			rest;
    bhdr_T		*hp;
    DATA_BL		*dp;

    ಯದಿ (buf->b_ml.ml_usedchunks == -1 || len == 0)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (buf->b_ml.ml_chunksize == ರಿಕ್ತ)
    {
	buf->b_ml.ml_chunksize = ALLOC_MULT(chunksize_T, 100);
	ಯದಿ (buf->b_ml.ml_chunksize == ರಿಕ್ತ)
	{
	    buf->b_ml.ml_usedchunks = -1;
	    ಪ್ರತ್ಯಾಗಮ;
	}
	buf->b_ml.ml_numchunks = 100;
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = 1;
	ml_upd_lastbuf = ರಿಕ್ತ;		// invalidate resume cache
    }

    ಯದಿ (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (long)buf->b_ml.ml_ಸಾಲು_len;
	ml_upd_lastbuf = ರಿಕ್ತ;		// invalidate resume cache
	ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    ಯದಿ (buf != ml_upd_lastbuf || ಸಾಲು < curline)
    {
	curline = 1;
	curix = 0;
    }
    ಪ್ರತಿ (; curix < buf->b_ml.ml_usedchunks - 1
	     && ಸಾಲು >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	     curix++)
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
    curchnk = buf->b_ml.ml_chunksize + curix;

    ಯದಿ (updtype == ML_CHNK_DELLINE)
	len = -len;
    curchnk->mlcs_totalsize += len;
    ಯದಿ (updtype == ML_CHNK_ADDLINE)
    {
	curchnk->mlcs_numlines++;

	// May resize here so we don't have to do it in both cases below
	ಯದಿ (buf->b_ml.ml_usedchunks + 1 >= buf->b_ml.ml_numchunks)
	{
	    chunksize_T *t_chunksize = buf->b_ml.ml_chunksize;

	    buf->b_ml.ml_numchunks = buf->b_ml.ml_numchunks * 3 / 2;
	    buf->b_ml.ml_chunksize = vim_realloc(buf->b_ml.ml_chunksize,
			    ಪರಿಮಾಣ(chunksize_T) * buf->b_ml.ml_numchunks);
	    ಯದಿ (buf->b_ml.ml_chunksize == ರಿಕ್ತ)
	    {
		// Hmmmm, Give up on offset for this buffer
		vim_free(t_chunksize);
		buf->b_ml.ml_usedchunks = -1;
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}

	ಯದಿ (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MAXL)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ;	    // number of entries in block
	    ಪೂರ್ಣಾಙ್ಕ	    idx;
	    ಪೂರ್ಣಾಙ್ಕ	    end_idx;
	    ಪೂರ್ಣಾಙ್ಕ	    text_end;
	    ಪೂರ್ಣಾಙ್ಕ	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			ಪರಿಮಾಣ(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    size = 0;
	    linecnt = 0;
	    ಯಾವತ್ (curline < buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
			&& linecnt < MLCS_MINL)
	    {
		ಯದಿ ((hp = ml_find_ಸಾಲು(buf, curline, ML_FIND)) == ರಿಕ್ತ)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    ಪ್ರತ್ಯಾಗಮ;
		}
		dp = (DATA_BL *)(hp->bh_data);
		ಎಣಿಕೆ = (long)(buf->b_ml.ml_locked_ಮೇಲಿನ) -
			(long)(buf->b_ml.ml_locked_ಕೆಳಗಿನ) + 1;
		idx = curline - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
		curline = buf->b_ml.ml_locked_ಮೇಲಿನ + 1;

		// compute index of last line to use in this MEMLINE
		rest = ಎಣಿಕೆ - idx;
		ಯದಿ (linecnt + rest > MLCS_MINL)
		{
		    end_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		ಅನ್ಯಥಾ
		{
		    end_idx = ಎಣಿಕೆ - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		ಯದಿ (buf->b_has_textprop)
		{
		    ಪೂರ್ಣಾಙ್ಕ i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    ಪ್ರತಿ (i = end_idx; i < idx; ++i)
			size += (ಪೂರ್ಣಾಙ್ಕ)STRLEN((char_u *)dp
				      + (dp->db_index[i] & DB_INDEX_MASK)) + 1;
		}
		ಅನ್ಯಥಾ
# endif
		{
		    ಯದಿ (idx == 0) // first line in block, text at the end
			text_end = dp->db_txt_end;
		    ಅನ್ಯಥಾ
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
	    ml_upd_lastbuf = ರಿಕ್ತ;   // Force recalc of curix & curline
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಅನ್ಯಥಾ ಯದಿ (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - ಸಾಲು <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    ಯದಿ (ಸಾಲು == buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		curchnk->mlcs_numlines = 0;
		curchnk->mlcs_totalsize = 0;
	    }
	    ಅನ್ಯಥಾ
	    {
		/*
		 * Line is just prior to last, move count for last
		 * This is the common case  when loading a new file
		 */
		hp = ml_find_ಸಾಲು(buf, buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, ML_FIND);
		ಯದಿ (hp == ರಿಕ್ತ)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    ಪ್ರತ್ಯಾಗಮ;
		}
		dp = (DATA_BL *)(hp->bh_data);
		ಯದಿ (dp->db_ಸಾಲು_ಎಣಿಕೆ == 1)
		    rest = dp->db_txt_end - dp->db_txt_start;
		ಅನ್ಯಥಾ
		    rest =
			((dp->db_index[dp->db_ಸಾಲು_ಎಣಿಕೆ - 2]) & DB_INDEX_MASK)
			- dp->db_txt_start;
		curchnk->mlcs_totalsize = rest;
		curchnk->mlcs_numlines = 1;
		curchnk[-1].mlcs_totalsize -= rest;
		curchnk[-1].mlcs_numlines -= 1;
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (updtype == ML_CHNK_DELLINE)
    {
	curchnk->mlcs_numlines--;
	ಯದಿ (curix < buf->b_ml.ml_usedchunks - 1
		&& curchnk->mlcs_numlines + curchnk[1].mlcs_numlines
								  <= MLCS_MINL)
	{
	    curix++;
	    curchnk = buf->b_ml.ml_chunksize + curix;
	}
	ಅನ್ಯಥಾ ಯದಿ (curix == 0 && curchnk->mlcs_numlines <= 0)
	{
	    ml_upd_lastbuf = ರಿಕ್ತ;   // Force recalc of curix & curline
	    buf->b_ml.ml_usedchunks--;
	    mch_memmove(buf->b_ml.ml_chunksize, buf->b_ml.ml_chunksize + 1,
			buf->b_ml.ml_usedchunks * ಪರಿಮಾಣ(chunksize_T));
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಅನ್ಯಥಾ ಯದಿ (curix == 0 || (curchnk->mlcs_numlines > 10
		    && curchnk->mlcs_numlines + curchnk[-1].mlcs_numlines
								  > MLCS_MINL))
	{
	    // The chunks are left as they are, the cached position stays
	    // valid.
	    ml_upd_lastbuf = buf;
	    ml_upd_lastcurline = curline;
	    ml_upd_lastcurix = curix;
	    ಪ್ರತ್ಯಾಗಮ;
	}

	// Collapse chunks
	ml_upd_lastbuf = ರಿಕ್ತ;   // Force recalc of curix & curline
	curchnk[-1].mlcs_numlines += curchnk->mlcs_numlines;
	curchnk[-1].mlcs_totalsize += curchnk->mlcs_totalsize;
	buf->b_ml.ml_usedchunks--;
	ಯದಿ (curix < buf->b_ml.ml_usedchunks)
	    mch_memmove(buf->b_ml.ml_chunksize + curix,
			buf->b_ml.ml_chunksize + curix + 1,
			(buf->b_ml.ml_usedchunks - curix) *
			ಪರಿಮಾಣ(chunksize_T));
	ಪ್ರತ್ಯಾಗಮ;
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
ml_find_ಸಾಲು_or_offset(buf_T *buf, linenr_T lnum, long *offp)
{
    linenr_T	curline;
    ಪೂರ್ಣಾಙ್ಕ		curix;
    long	size;
    bhdr_T	*hp;
    DATA_BL	*dp;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;		// number of entries in block
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		start_idx;
    ಪೂರ್ಣಾಙ್ಕ		text_end;
    long	offset;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		ffdos = (get_fileformat(buf) == EOL_DOS);
    ಪೂರ್ಣಾಙ್ಕ		extra = 0;

    // take care of cached line first
    ml_flush_ಸಾಲು(curbuf);

    ಯದಿ (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == ರಿಕ್ತ
	    || lnum < 0)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (offp == ರಿಕ್ತ)
	offset = 0;
    ಅನ್ಯಥಾ
	offset = *offp;
    ಯದಿ (lnum == 0 && offset <= 0)
	ಪ್ರತ್ಯಾಗಮ 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = size = 0;
    ಯಾವತ್ (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (offset != 0
	       && offset > size + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (long)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	size += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	ಯದಿ (offset && ffdos)
	    size += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    ಯಾವತ್ ((lnum != 0 && curline < lnum) || (offset != 0 && size < offset))
    {
# ifdef FEAT_PROP_POPUP
	size_t textprop_total = 0;
# endif

	ಯದಿ (curline > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		|| (hp = ml_find_ಸಾಲು(buf, curline, ML_FIND)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
	dp = (DATA_BL *)(hp->bh_data);
	ಎಣಿಕೆ = (long)(buf->b_ml.ml_locked_ಮೇಲಿನ) -
		(long)(buf->b_ml.ml_locked_ಕೆಳಗಿನ) + 1;
	start_idx = idx = curline - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
	ಯದಿ (idx == 0)  // first line in block, text at the end
	    text_end = dp->db_txt_end;
	ಅನ್ಯಥಾ
	    text_end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);
	// Compute index of last line to use in this MEMLINE
	ಯದಿ (lnum != 0)
	{
	    ಯದಿ (curline + (ಎಣಿಕೆ - idx) >= lnum)
		idx += lnum - curline - 1;
	    ಅನ್ಯಥಾ
		idx = ಎಣಿಕೆ - 1;
	}
	ಅನ್ಯಥಾ
	{
	    extra = 0;
	    ಪ್ರತಿ (;;)
	    {
# ifdef FEAT_PROP_POPUP
		size_t textprop_size = 0;

		ಯದಿ (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_index[idx]) & DB_INDEX_MASK);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_end
				  : ((dp->db_index[idx - 1]) & DB_INDEX_MASK));
		    textprop_size = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		ಯದಿ (!(offset >= size
			+ text_end - (ಪೂರ್ಣಾಙ್ಕ)((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
			- (long)(textprop_total + textprop_size)
# endif
			+ ffdos))
		    ಭಙ್ಗ;

		ಯದಿ (ffdos)
		    size++;
# ifdef FEAT_PROP_POPUP
		textprop_total += textprop_size;
# endif
		ಯದಿ (idx == ಎಣಿಕೆ - 1)
		{
		    extra = 1;
		    ಭಙ್ಗ;
		}
		idx++;
	    }
	}
# ifdef FEAT_PROP_POPUP
	ಯದಿ (buf->b_has_textprop && lnum != 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ i;

	    // cannot use the db_index pointer, need to get the actual text
	    // lengths.
	    len = 0;
	    ಪ್ರತಿ (i = start_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_index[i]) & DB_INDEX_MASK);
		len += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p) + 1;
	    }
	}
	ಅನ್ಯಥಾ
# endif
	    len = text_end - ((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
				- (long)textprop_total
# endif
				;
	size += len;
	ಯದಿ (offset != 0 && size >= offset)
	{
	    ಯದಿ (size + ffdos == offset)
		*offp = 0;
	    ಅನ್ಯಥಾ ಯದಿ (idx == start_idx)
		*offp = offset - size + len;
	    ಅನ್ಯಥಾ
		*offp = offset - size + len
		     - (text_end - ((dp->db_index[idx - 1]) & DB_INDEX_MASK))
# ifdef FEAT_PROP_POPUP
		     + (long)textprop_total
# endif
		     ;
	    curline += idx - start_idx + extra;
	    ಯದಿ (curline > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ -1;	// exactly one byte beyond the end
	    ಪ್ರತ್ಯಾಗಮ curline;
	}
	curline = buf->b_ml.ml_locked_ಮೇಲಿನ + 1;
    }

    ಯದಿ (lnum != 0)
    {
	// Count extra CR characters.
	ಯದಿ (ffdos)
	    size += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	ಯದಿ ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    size -= ffdos + 1;
    }

    ಪ್ರತ್ಯಾಗಮ size;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    ಶೂನ್ಯ
goto_byte(long cnt)
{
    long	boff = cnt;
    linenr_T	lnum;

    ml_flush_ಸಾಲು(curbuf);	// cached line may be dirty
    setpcmark();
    ಯದಿ (boff)
	--boff;
    lnum = ml_find_ಸಾಲು_or_offset(curbuf, (linenr_T)0, &boff);
    ಯದಿ (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	curwin->w_curswant = MAXCOL;
	coladvance((colnr_T)MAXCOL);
    }
    ಅನ್ಯಥಾ
    {
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = (colnr_T)boff;
	curwin->w_cursor.coladd = 0;
	curwin->w_set_curswant = true;
    }
    check_cursor();

    // Make sure the cursor is on the first byte of a multi-byte char.
    ಯದಿ (has_mbyte)
	mb_adjust_cursor();
}
#endif
