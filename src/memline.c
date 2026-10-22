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
#ವ್ಯಾಖ್ಯಾನಿಸು CHECK(c, s)	ಮಾಡು { /**/ } ಆಗ (0)

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

#ಆದರೆ defined(SASC) || defined(__amigaos4__)
# include <proto/dos.h>	    // for Open() and Close()
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ block0		ZERO_BL;    // contents of the first block
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಸೂಚಕ_ಖಂಡ	PTR_BL;	    // contents of a pointer block
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ದತ್ತಾಂಶ_ಖಂಡ	ದತ್ತಾಂಶ_BL;    // contents of a data block
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಸೂಚಕ_ನಮೂದು	PTR_EN;	    // block/line-count pair

#ವ್ಯಾಖ್ಯಾನಿಸು ದತ್ತಾಂಶ_ID	       (('d' << 8) + 'a')   // data block id
#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ID	       (('p' << 8) + 't')   // pointer block id
#ವ್ಯಾಖ್ಯಾನಿಸು BLOCK0_ID0     'b'		    // block 0 id 0
#ವ್ಯಾಖ್ಯಾನಿಸು BLOCK0_ID1     '0'		    // block 0 id 1
#ವ್ಯಾಖ್ಯಾನಿಸು BLOCK0_ID1_C0  'c'		    // block 0 id 1 'cm' 0
#ವ್ಯಾಖ್ಯಾನಿಸು BLOCK0_ID1_C1  'C'		    // block 0 id 1 'cm' 1
#ವ್ಯಾಖ್ಯಾನಿಸು BLOCK0_ID1_C2  'd'		    // block 0 id 1 'cm' 2
// BLOCK0_ID1_C3 and BLOCK0_ID1_C4 are for libsodium encryption.  However, for
// these the swapfile is disabled, thus they will not be used.  Added for
// consistency anyway.
#ವ್ಯಾಖ್ಯಾನಿಸು BLOCK0_ID1_C3  'S'		    // block 0 id 1 'cm' 3
#ವ್ಯಾಖ್ಯಾನಿಸು BLOCK0_ID1_C4  's'		    // block 0 id 1 'cm' 4

#ಆದರೆ defined(FEAT_CRYPT)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ id1_codes[] = {
    BLOCK0_ID1_C0,  // CRYPT_M_ZIP
    BLOCK0_ID1_C1,  // CRYPT_M_BF
    BLOCK0_ID1_C2,  // CRYPT_M_BF2
    BLOCK0_ID1_C3,  // CRYPT_M_SOD  - Unused!
    BLOCK0_ID1_C4,  // CRYPT_M_SOD2  - Unused!
};
#ಆದರೆ_ಅಂತ್ಯ

/*
 * pointer to a block, used in a pointer block
 */
ರಚನೆ ಸೂಚಕ_ನಮೂದು
{
    blocknr_T	pe_bnum;	// block number
    linenr_T	pe_ಸಾಲು_ಎಣಿಕೆ;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    ಪೂರ್ಣಾಂಕ		pe_ಪುಟ_ಎಣಿಕೆ;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
ರಚನೆ ಸೂಚಕ_ಖಂಡ
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_ಎಣಿಕೆ;	// number of pointers in this block
    short_u	pb_ಎಣಿಕೆ_max;	// maximum value for pb_count
    PTR_EN	pb_ಸೂಚಕ[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#ವ್ಯಾಖ್ಯಾನಿಸು PB_ಎಣಿಕೆ_MAX(mfp) (short_u)(((mfp)->mf_ಪುಟ_ಗಾತ್ರ - offsetof(PTR_BL, pb_ಸೂಚಕ)) / ಗಾತ್ರ(PTR_EN))

/*
 * A data block is a leaf in the tree.
 *
 * The text of the lines is at the end of the block. The text of the first line
 * in the block is put at the end, the text of the second line in front of it,
 * etc. Thus the order of the lines is the opposite of the line number.
 */
ರಚನೆ ದತ್ತಾಂಶ_ಖಂಡ
{
    short_u	db_id;		// ID for data block: DATA_ID
    ಚಿಹ್ನೆರಹಿತ	db_free;	// free space available
    ಚಿಹ್ನೆರಹಿತ	db_txt_ಆರಂಭ;	// byte where text starts
    ಚಿಹ್ನೆರಹಿತ	db_txt_ಕೊನೆ;	// byte just after data block
    linenr_T	db_ಸಾಲು_ಎಣಿಕೆ;	// number of lines in this block
    ಚಿಹ್ನೆರಹಿತ	db_ಸೂಚ್ಯಂಕ[1];	// index for start of line (actually bigger)
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
#ವ್ಯಾಖ್ಯಾನಿಸು DB_MARKED	((ಚಿಹ್ನೆರಹಿತ)1 << ((ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ) * 8) - 1))
#ವ್ಯಾಖ್ಯಾನಿಸು DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ	(~DB_MARKED)

#ವ್ಯಾಖ್ಯಾನಿಸು ಸೂಚ್ಯಂಕ_ಗಾತ್ರ  (ಗಾತ್ರ(ಚಿಹ್ನೆರಹಿತ))	    // size of one db_index entry
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ (offsetof(ದತ್ತಾಂಶ_BL, db_ಸೂಚ್ಯಂಕ))  // size of data block header

#ವ್ಯಾಖ್ಯಾನಿಸು B0_FNAME_ಗಾತ್ರ_ORG	900	// what it was in older versions
#ವ್ಯಾಖ್ಯಾನಿಸು B0_FNAME_ಗಾತ್ರ_NOCRYPT	898	// 2 bytes used for other things
#ವ್ಯಾಖ್ಯಾನಿಸು B0_FNAME_ಗಾತ್ರ_CRYPT	890	// 10 bytes used for other things
#ವ್ಯಾಖ್ಯಾನಿಸು B0_UNAME_ಗಾತ್ರ		40
#ವ್ಯಾಖ್ಯಾನಿಸು B0_HNAME_ಗಾತ್ರ		40
/*
 * Restrict the numbers to 32 bits, otherwise most compilers will complain.
 * This won't detect a 64 bit machine that only swaps a byte in the top 32
 * bits, but that is crazy anyway.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು B0_MAGIC_LONG	0x30313233L
#ವ್ಯಾಖ್ಯಾನಿಸು B0_MAGIC_INT	0x20212223L
#ವ್ಯಾಖ್ಯಾನಿಸು B0_MAGIC_SHORT	0x10111213L
#ವ್ಯಾಖ್ಯಾನಿಸು B0_MAGIC_CHAR	0x55

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
ರಚನೆ block0
{
    char_u	b0_id[2];	// id for block 0: BLOCK0_ID0 and BLOCK0_ID1,
				// BLOCK0_ID1_C0, BLOCK0_ID1_C1, etc.
    char_u	b0_ಆವೃತ್ತಿ[10];	// Vim version string
    char_u	b0_ಪುಟ_ಗಾತ್ರ[4];// number of bytes per page
    char_u	b0_mtime[4];	// last modification time of file
    char_u	b0_ino[4];	// inode of b0_fname
    char_u	b0_pid[4];	// process id of creator (or 0)
    char_u	b0_uname[B0_UNAME_ಗಾತ್ರ]; // name of user (uid if no name)
    char_u	b0_hname[B0_HNAME_ಗಾತ್ರ]; // host name (if it has a name)
    char_u	b0_fname[B0_FNAME_ಗಾತ್ರ_ORG]; // name of file being edited
    ದೀರ್ಘ	b0_magic_long;	// check for byte order of long
    ಪೂರ್ಣಾಂಕ		b0_magic_int;	// check for byte order of int
    ಗಿಡ್ಡ	b0_magic_short;	// check for byte order of short
    char_u	b0_magic_char;	// check for last char
};

/*
 * Note: b0_dirty and b0_flags are put at the end of the file name.  For very
 * long file names in older versions of Vim they are invalid.
 * The 'fileencoding' comes before b0_flags, with a NUL in front.  But only
 * when there is room, for very long file names it's omitted.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು B0_DIRTY	0x55
#ವ್ಯಾಖ್ಯಾನಿಸು b0_dirty	b0_fname[B0_FNAME_ಗಾತ್ರ_ORG - 1]

/*
 * The b0_flags field is new in Vim 7.0.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು b0_flags	b0_fname[B0_FNAME_ಗಾತ್ರ_ORG - 2]

/*
 * Crypt seed goes here, 8 bytes.  New in Vim 7.3.
 * Without encryption these bytes may be used for 'fenc'.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು b0_ಬೀಜ		b0_fname[B0_FNAME_ಗಾತ್ರ_ORG - 2 - MF_ಬೀಜ_LEN]

// The lowest two bits contain the fileformat.  Zero means it's not set
// (compatible with Vim 6.x), otherwise it's EOL_UNIX + 1, EOL_DOS + 1 or
// EOL_MAC + 1.
#ವ್ಯಾಖ್ಯಾನಿಸು B0_FF_ಮುಖವಾಡ	3

// Swap file is in directory of edited file.  Used to find the file from
// different mount points.
#ವ್ಯಾಖ್ಯಾನಿಸು B0_SAME_DIR	4

// The 'fileencoding' is at the end of b0_fname[], with a NUL in front of it.
// When empty there is only the NUL.
#ವ್ಯಾಖ್ಯಾನಿಸು B0_HAS_FENC	8

#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_INCR	5	// nr of entries added to ml_stack at a time

/*
 * The line number where the first mark may be is remembered.
 * If it is 0 there are no marks at all.
 * (always used for the current buffer only, no buffer change possible while
 * executing a global command).
 */
ಸ್ಥಿರ linenr_T	ಅತಿಕಡಿಮೆ_marked = 0;

/*
 * arguments for ml_find_line()
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ML_DELETE	0x11	    // delete line
#ವ್ಯಾಖ್ಯಾನಿಸು ML_ಸೇರಿಸು	0x12	    // insert line
#ವ್ಯಾಖ್ಯಾನಿಸು ML_ಕಂಡುಹಿಡಿ		0x13	    // just find the line
#ವ್ಯಾಖ್ಯಾನಿಸು ML_FLUSH	0x02	    // flush locked block
#ವ್ಯಾಖ್ಯಾನಿಸು ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
      UB_FNAME = 0	// update timestamp and filename
    , UB_SAME_DIR       // update the B0_SAME_DIR flag
    , UB_CRYPT		// update crypt key
} upd_block0_T;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
ಸ್ಥಿರ ಶೂನ್ಯ ml_ಗಣ_b0_crypt(buf_T *buf, ZERO_BL *b0p);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ ml_upd_block0(buf_T *buf, upd_block0_T what);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_b0_fname(ZERO_BL *, buf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_b0_dir_flag(ZERO_BL *b0p, buf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ ಕೂಡಿಸು_b0_fenc(ZERO_BL *b0p, buf_T *buf);
ಸ್ಥಿರ time_t swapfile_info(char_u *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ recov_file_ಹೆಸರುಗಳು(char_u **, char_u *, ಪೂರ್ಣಾಂಕ prepend_dot);
ಸ್ಥಿರ char_u *findswapname(buf_T *, char_u **, char_u *);
ಸ್ಥಿರ ಶೂನ್ಯ ml_flush_ಸಾಲು(buf_T *);
ಸ್ಥಿರ bhdr_T *ml_new_ದತ್ತಾಂಶ(memfile_T *, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ bhdr_T *ml_new_ptr(memfile_T *);
ಸ್ಥಿರ bhdr_T *ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf_T *, linenr_T, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ml_ಕೂಡಿಸು_ರಾಶಿ(buf_T *);
ಸ್ಥಿರ ಶೂನ್ಯ ml_lineadd(buf_T *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ b0_magic_wrong(ZERO_BL *);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_INODE
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fnamecmp_ino(char_u *, char_u *, ದೀರ್ಘ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ long_ಗಮ್ಯ_char(ದೀರ್ಘ, char_u *);
ಸ್ಥಿರ ದೀರ್ಘ char_ಗಮ್ಯ_long(char_u *);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
ಸ್ಥಿರ cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ reading);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
ಸ್ಥಿರ ಶೂನ್ಯ ml_updatechunk(buf_T *buf, ದೀರ್ಘ ಸಾಲು, ದೀರ್ಘ len, ಪೂರ್ಣಾಂಕ updtype);
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
ml_open(buf_T *buf)
{
    memfile_T	*mfp;
    bhdr_T	*hp = ಖಾಲಿ;
    ZERO_BL	*b0p;
    PTR_BL	*pp;
    ದತ್ತಾಂಶ_BL	*dp;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ರಾಶಿ_ಗಾತ್ರ = 0; // no stack yet
    buf->b_ml.ml_ರಾಶಿ = ಖಾಲಿ;	// no stack yet
    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;	// nothing in the stack
    buf->b_ml.ml_locked = ಖಾಲಿ;	// no cached block
    buf->b_ml.ml_ಸಾಲು_lnum = 0;	// no cached line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = ಖಾಲಿ;
    buf->b_ml.ml_usedchunks = 0;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    ಆದರೆ (p_uc && buf->b_p_swf)
	buf->b_may_ಅದಲುಬದಲು = true;
    ಇಲ್ಲದಿದ್ದರೆ
	buf->b_may_ಅದಲುಬದಲು = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(ಖಾಲಿ, 0);
    ಆದರೆ (mfp == ಖಾಲಿ)
	ಹೋಗು error;

    buf->b_ml.ml_mfp = mfp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    mfp->mf_ಬಫರ್ = buf;
#ಆದರೆ_ಅಂತ್ಯ
    buf->b_ml.ml_flags = ML_ಖಾಲಿಗಳು;
    buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ = 1;

/*
 * fill block0 struct and write page 0
 */
    ಆದರೆ ((hp = mf_new(mfp, FALSE, 1)) == ಖಾಲಿ)
	ಹೋಗು error;
    ಆದರೆ (hp->bh_bnum != 0)
    {
	iemsg(e_didnt_get_ಖಂಡ_nr_zero);
	ಹೋಗು error;
    }
    b0p = (ZERO_BL *)(hp->bh_ದತ್ತಾಂಶ);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (ದೀರ್ಘ)B0_MAGIC_LONG;
    b0p->b0_magic_int = (ಪೂರ್ಣಾಂಕ)B0_MAGIC_INT;
    b0p->b0_magic_short = (ಗಿಡ್ಡ)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_ಆವೃತ್ತಿ, "VIM ", 4);
    STRNCPY(b0p->b0_ಆವೃತ್ತಿ + 4, ಆವೃತ್ತಿ, 6);
    long_ಗಮ್ಯ_char((ದೀರ್ಘ)mfp->mf_ಪುಟ_ಗಾತ್ರ, b0p->b0_ಪುಟ_ಗಾತ್ರ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    ಆದರೆ (!buf->b_spell)
#ಆದರೆ_ಅಂತ್ಯ
    {
	b0p->b0_dirty = buf->b_ಬದಲಾಯಿತು ? B0_DIRTY : 0;
	b0p->b0_flags = get_fileformat(buf) + 1;
	ಗಣ_b0_fname(b0p, buf);
	(ಶೂನ್ಯ)get_user_name(b0p->b0_uname, B0_UNAME_ಗಾತ್ರ);
	b0p->b0_uname[B0_UNAME_ಗಾತ್ರ - 1] = NUL;
	mch_get_host_name(b0p->b0_hname, B0_HNAME_ಗಾತ್ರ);
	b0p->b0_hname[B0_HNAME_ಗಾತ್ರ - 1] = NUL;
	long_ಗಮ್ಯ_char(mch_get_pid(), b0p->b0_pid);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	ml_ಗಣ_b0_crypt(buf, b0p);
#ಆದರೆ_ಅಂತ್ಯ
    }

    /*
     * Always sync block number 0 to disk, so we can check the file name in
     * the swap file in findswapname(). Don't do this for a help files or
     * a spell buffer though.
     * Only works when there's a swapfile, otherwise it's done when the file
     * is created.
     */
    mf_put(mfp, hp, TRUE, FALSE);
    ಆದರೆ (!buf->b_help && !B_SPELL(buf))
	(ಶೂನ್ಯ)mf_sync(mfp, 0);

    /*
     * Fill in root pointer block and write page 1.
     */
    ಆದರೆ ((hp = ml_new_ptr(mfp)) == ಖಾಲಿ)
	ಹೋಗು error;
    ಆದರೆ (hp->bh_bnum != 1)
    {
	iemsg(e_didnt_get_ಖಂಡ_nr_one);
	ಹೋಗು error;
    }
    pp = (PTR_BL *)(hp->bh_ದತ್ತಾಂಶ);
    pp->pb_ಎಣಿಕೆ = 1;
    pp->pb_ಸೂಚಕ[0].pe_bnum = 2;
    pp->pb_ಸೂಚಕ[0].pe_ಪುಟ_ಎಣಿಕೆ = 1;
    pp->pb_ಸೂಚಕ[0].pe_old_lnum = 1;
    pp->pb_ಸೂಚಕ[0].pe_ಸಾಲು_ಎಣಿಕೆ = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    ಆದರೆ ((hp = ml_new_ದತ್ತಾಂಶ(mfp, FALSE, 1)) == ಖಾಲಿ)
	ಹೋಗು error;
    ಆದರೆ (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_ಖಂಡ_nr_two);
	ಹೋಗು error;
    }

    dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
    dp->db_ಸೂಚ್ಯಂಕ[0] = --dp->db_txt_ಆರಂಭ;	// at end of block
    dp->db_free -= 1 + ಸೂಚ್ಯಂಕ_ಗಾತ್ರ;
    dp->db_ಸಾಲು_ಎಣಿಕೆ = 1;
    *((char_u *)dp + dp->db_txt_ಆರಂಭ) = NUL;	// empty line

    ಹಿಂತಿರುಗಿ OK;

error:
    ಆದರೆ (mfp != ಖಾಲಿ)
    {
	ಆದರೆ (hp)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, TRUE);	    // will also free(mfp->mf_fname)
    }
    buf->b_ml.ml_mfp = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ FAIL;
}

#ಆದರೆ defined(FEAT_CRYPT)
/*
 * Swapfile encryption is not supported by XChaCha20.  If this crypt method is
 * used then disable the swapfile, to avoid plain text being written to disk,
 * and return TRUE.
 * Otherwise return FALSE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
crypt_may_close_swapfile(buf_T *buf, char_u *ಕೀಲಿ, ಪೂರ್ಣಾಂಕ ವಿಧಾನ)
{
    ಆದರೆ (crypt_ವಿಧಾನ_is_sodium(ವಿಧಾನ) && *ಕೀಲಿ != NUL)
    {
	mf_close_file(buf, TRUE);
	buf->b_p_swf = FALSE;
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Prepare encryption for "buf" for the current key and method.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ml_ಗಣ_mfp_crypt(buf_T *buf)
{
    ಆದರೆ (*buf->b_p_ಕೀಲಿ == NUL)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ ವಿಧಾನ_nr = crypt_get_ವಿಧಾನ_nr(buf);

    ಆದರೆ (ವಿಧಾನ_nr > CRYPT_M_ZIP && ವಿಧಾನ_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_ಬೀಜ(buf->b_ml.ml_mfp->mf_ಬೀಜ, MF_ಬೀಜ_LEN, ಖಾಲಿ, 0);
    }
# ifdef FEAT_SODIUM
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (crypt_ವಿಧಾನ_is_sodium(ವಿಧಾನ_nr))
	crypt_sodium_randombytes_buf(buf->b_ml.ml_mfp->mf_ಬೀಜ, MF_ಬೀಜ_LEN);
# endif
}

/*
 * Prepare encryption for "buf" with block 0 "b0p".
 * Note: should not be called with libsodium encryption, since xchacha20 does
 * not support swapfile encryption.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ml_ಗಣ_b0_crypt(buf_T *buf, ZERO_BL *b0p)
{
    ಆದರೆ (*buf->b_p_ಕೀಲಿ == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ ವಿಧಾನ_nr = crypt_get_ವಿಧಾನ_nr(buf);

	b0p->b0_id[1] = id1_codes[ವಿಧಾನ_nr];
	ಆದರೆ (ವಿಧಾನ_nr > CRYPT_M_ZIP && ವಿಧಾನ_nr < CRYPT_M_SOD)
	{
	    // Generate a seed and store it in block 0 and in the memfile.
	    sha2_ಬೀಜ(&b0p->b0_ಬೀಜ, MF_ಬೀಜ_LEN, ಖಾಲಿ, 0);
	    mch_memmove(buf->b_ml.ml_mfp->mf_ಬೀಜ, &b0p->b0_ಬೀಜ, MF_ಬೀಜ_LEN);
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
ml_ಗಣ_crypt_ಕೀಲಿ(
    buf_T	*buf,
    char_u	*old_ಕೀಲಿ,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    ಪೂರ್ಣಾಂಕ		ಪುಟ_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		idx;
    ದೀರ್ಘ	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    ದತ್ತಾಂಶ_BL	*dp;
    blocknr_T	bnum;
    ಪೂರ್ಣಾಂಕ		ತುದಿ;
    ಪೂರ್ಣಾಂಕ		old_ವಿಧಾನ;

    ಆದರೆ (mfp == ಖಾಲಿ || mfp->mf_fd < 0)
	ಹಿಂತಿರುಗಿ;  // no memfile yet, nothing to do
    old_ವಿಧಾನ = crypt_ವಿಧಾನ_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    ಆದರೆ (crypt_may_close_swapfile(buf, buf->b_p_ಕೀಲಿ, crypt_get_ವಿಧಾನ_nr(buf)))
	ಹಿಂತಿರುಗಿ;
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
    mfp->mf_old_cm = old_ವಿಧಾನ;
    ಆದರೆ (old_ವಿಧಾನ > 0 && *old_ಕೀಲಿ != NUL)
	mch_memmove(mfp->mf_old_ಬೀಜ, mfp->mf_ಬೀಜ, MF_ಬೀಜ_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    ಆದರೆ (mfp->mf_infile_ಎಣಿಕೆ > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_ಸಾಲು(buf);		    // flush buffered line
	(ಶೂನ್ಯ)ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = ಖಾಲಿ;
	bnum = 1;		// start with block 1
	ಪುಟ_ಎಣಿಕೆ = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;
	VIM_CLEAR(buf->b_ml.ml_ರಾಶಿ);
	buf->b_ml.ml_ರಾಶಿ_ಗಾತ್ರ = 0;	// no stack yet

	ಗಾಗಿ ( ; !got_int; ಸಾಲು_breakcheck())
	{
	    ಆದರೆ (hp != ಖಾಲಿ)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    ಆದರೆ ((hp = mf_get(mfp, bnum, ಪುಟ_ಎಣಿಕೆ)) == ಖಾಲಿ)
	    {
		ಆದರೆ (bnum == 1)
		    ನಿಲ್ಲಿಸು;
		++error;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		pp = (PTR_BL *)(hp->bh_ದತ್ತಾಂಶ);
		ಆದರೆ (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    ಆದರೆ (pp->pb_ಎಣಿಕೆ == 0)
		    {
			// empty block?
			++error;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx < (ಪೂರ್ಣಾಂಕ)pp->pb_ಎಣಿಕೆ)	// go a block deeper
		    {
			ಆದರೆ (pp->pb_ಸೂಚಕ[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    ಮುಂದುವರಿ;
			}

			// going one block deeper in the tree, new entry in
			//ರಾಶಿ
			ಆದರೆ ((ತುದಿ = ml_ಕೂಡಿಸು_ರಾಶಿ(buf)) < 0)
			{
			    ++error;
			    ನಿಲ್ಲಿಸು;		    // out of memory
			}
			ip = &(buf->b_ml.ml_ರಾಶಿ[ತುದಿ]);
			ip->ip_bnum = bnum;
			ip->ip_ಸೂಚ್ಯಂಕ = idx;

			bnum = pp->pb_ಸೂಚಕ[idx].pe_bnum;
			ಪುಟ_ಎಣಿಕೆ = pp->pb_ಸೂಚಕ[idx].pe_ಪುಟ_ಎಣಿಕೆ;
			idx = 0;
			ಮುಂದುವರಿ;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ	    // not a pointer block
		{
		    dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
		    ಆದರೆ (dp->db_id != ದತ್ತಾಂಶ_ID)	// block id wrong
			++error;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = ಖಾಲಿ;
		    }
		}
	    }

	    ಆದರೆ (buf->b_ml.ml_ರಾಶಿ_ತುದಿ == 0)	//ಪೂರ್ಣಗೊಂಡ
		ನಿಲ್ಲಿಸು;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_ರಾಶಿ[--(buf->b_ml.ml_ರಾಶಿ_ತುದಿ)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_ಸೂಚ್ಯಂಕ + 1;	    // go to next index
	    ಪುಟ_ಎಣಿಕೆ = 1;
	}
	ಆದರೆ (hp != ಖಾಲಿ)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	ಆದರೆ (error > 0)
	    emsg(_(e_error_while_updating_ಅದಲುಬದಲು_file_crypt));
    }

    mfp->mf_old_ಕೀಲಿ = ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
    ಶೂನ್ಯ
ml_setname(buf_T *buf)
{
    ಪೂರ್ಣಾಂಕ		success = FALSE;
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;
#ಆದರೆ defined(MSWIN)
    char_u	*p;
#ಆದರೆ_ಅಂತ್ಯ

    mfp = buf->b_ml.ml_mfp;
    ಆದರೆ (mfp->mf_fd < 0)		    // there is no swap file yet
    {
	/*
	 * When 'updatecount' is 0 and 'noswapfile' there is no swap file.
	 * For help files we will make a swap file now.
	 */
	ಆದರೆ (p_uc != 0 && (cmdmod.cmod_flags & CMOD_NOSWAPFILE) == 0)
	    ml_open_file(buf);	    // create a swap file
	ಹಿಂತಿರುಗಿ;
    }

    /*
     * Try all directories in the 'directory' option.
     */
    dirp = p_dir;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (*dirp == NUL)	    // tried all directories, fail
	    ನಿಲ್ಲಿಸು;
	fname = findswapname(buf, &dirp, mfp->mf_fname);
	// autocmd may have freed mfp if findswapname creates
	// different swapfile name
	ಆದರೆ (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    ಹಿಂತಿರುಗಿ;
	}
						    // alloc's fname
	ಆದರೆ (dirp == ಖಾಲಿ)	    // out of memory
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (fname == ಖಾಲಿ)	    // no file name found for this dir
	    ಮುಂದುವರಿ;

#ಆದರೆ defined(MSWIN)
	/*
	 * Set full pathname for swap file now, because a ":!cd dir" may
	 * change directory without us knowing it.
	 */
	p = ತುಂಬಿದ_Name_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	ಆದರೆ (fname == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ
	// if the file name is the same we don't have to do anything
	ಆದರೆ (fnamecmp(fname, mfp->mf_fname) == 0)
	{
	    vim_free(fname);
	    success = TRUE;
	    ನಿಲ್ಲಿಸು;
	}
	// need to close the swap file before renaming
	ಆದರೆ (mfp->mf_fd >= 0)
	{
	    close(mfp->mf_fd);
	    mfp->mf_fd = -1;
	}

	// try to rename the swap file
	ಆದರೆ (vim_rename(mfp->mf_fname, fname) == 0)
	{
	    success = TRUE;
	    vim_free(mfp->mf_fname);
	    mfp->mf_fname = fname;
	    vim_free(mfp->mf_ffname);
#ಆದರೆ defined(MSWIN)
	    mfp->mf_ffname = ಖಾಲಿ;  // mf_fname is full pathname already
#ಇಲ್ಲದಿದ್ದರೆ
	    mf_ಗಣ_ffname(mfp);
#ಆದರೆ_ಅಂತ್ಯ
	    ml_upd_block0(buf, UB_SAME_DIR);
	    ನಿಲ್ಲಿಸು;
	}
	vim_free(fname);	    // this fname didn't work, try another
    }

    ಆದರೆ (mfp->mf_fd == -1)	    // need to (re)open the swap file
    {
	mfp->mf_fd = mch_open((ಅಕ್ಷರ *)mfp->mf_fname, O_RDWR | O_EXTRA, 0);
	ಆದರೆ (mfp->mf_fd < 0)
	{
	    // could not (re)open the swap file, what can we do????
	    emsg(_(e_oops_ಕಳೆದುಹೋದ_the_ಅದಲುಬದಲು_file));
	    ಹಿಂತಿರುಗಿ;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FD_CLOEXEC
	{
	    ಪೂರ್ಣಾಂಕ fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    ಆದರೆ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(ಶೂನ್ಯ)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (!success)
	emsg(_(e_could_not_rename_ಅದಲುಬದಲು_file));
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
	ಆದರೆ (!buf->b_p_ro || buf->b_ಬದಲಾಯಿತು)
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
    ಆದರೆ (mfp == ಖಾಲಿ || mfp->mf_fd >= 0 || !buf->b_p_swf
				      || (cmdmod.cmod_flags & CMOD_NOSWAPFILE))
	ಹಿಂತಿರುಗಿ;		// nothing to do

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    // For a spell buffer use a temp file name.
    ಆದರೆ (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	ಆದರೆ (fname != ಖಾಲಿ)
	    (ಶೂನ್ಯ)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_ಅದಲುಬದಲು = false;
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Try all directories in 'directory' option.
     */
    dirp = p_dir;
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (*dirp == NUL)
	    ನಿಲ್ಲಿಸು;
	// There is a small chance that between choosing the swap file name
	// and creating it, another Vim creates the file.  In that case the
	// creation will fail and we will use another directory.
	fname = findswapname(buf, &dirp, ಖಾಲಿ); // allocates fname
	// autocmd may have freed mfp, grr!
	ಆದರೆ (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (dirp == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;  // out of memory
	ಆದರೆ (fname == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಆದರೆ (mf_open_file(mfp, fname) == OK)	// consumes fname!
	{
	    // don't sync yet in ml_sync_all()
	    mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;
#ಆದರೆ defined(MSWIN)
	    /*
	     * set full pathname for swap file now, because a ":!cd dir" may
	     * change directory without us knowing it.
	     */
	    mf_fullname(mfp);
#ಆದರೆ_ಅಂತ್ಯ
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    ಆದರೆ (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_ಗಣ_dirty(mfp);
		ನಿಲ್ಲಿಸು;
	    }
	    // Writing block 0 failed: close the file and try another dir
	    mf_close_file(buf, FALSE);
	}
    }

    ಆದರೆ (*p_dir != NUL && mfp->mf_fname == ಖಾಲಿ)
    {
	need_wait_return = TRUE;	// call wait_return() later
	++no_wait_return;
	(ಶೂನ್ಯ)semsg(_(e_unable_ಗಮ್ಯ_open_ಅದಲುಬದಲು_file_for_str_recovery_impossible),
		    buf_spname(buf) != ಖಾಲಿ ? buf_spname(buf) : buf->b_fname);
	--no_wait_return;
    }

    // don't try to open a swap file again
    buf->b_may_ಅದಲುಬದಲು = false;
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
    ಶೂನ್ಯ
check_need_ಅದಲುಬದಲು(
    ಪೂರ್ಣಾಂಕ	    newfile)		// reading file into new buffer
{
    ಪೂರ್ಣಾಂಕ old_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent; // might be reset by an E325 message

    ಆದರೆ (curbuf->b_may_ಅದಲುಬದಲು && (!curbuf->b_p_ro || !newfile))
	ml_open_file(curbuf);
    ಸಂದೇಶ_silent = old_ಸಂದೇಶ_silent;
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
    ಶೂನ್ಯ
ml_close(buf_T *buf, ಪೂರ್ಣಾಂಕ del_file)
{
    ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)		// not open
	ಹಿಂತಿರುಗಿ;
    mf_close(buf->b_ml.ml_mfp, del_file);	// close the .swp file
    ಆದರೆ (buf->b_ml.ml_ಸಾಲು_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_ಸಾಲು_ptr);
    vim_free(buf->b_ml.ml_ರಾಶಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
    VIM_CLEAR(buf->b_ml.ml_chunksize);
#ಆದರೆ_ಅಂತ್ಯ
    buf->b_ml.ml_mfp = ಖಾಲಿ;

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
ml_close_all(ಪೂರ್ಣಾಂಕ del_file)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ml_close(buf, del_file && ((buf->b_flags & BF_PRESERVED) == 0
				 || vim_strchr(p_cpo, CPO_PRESERVE) == ಖಾಲಿ));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    spell_delete_wordlist();	// delete the internal wordlist
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TEMPDIRNAMES
    vim_deltempdir();		// delete created temp directory
#ಆದರೆ_ಅಂತ್ಯ
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
	ಆದರೆ (!buf_Is_ಬದಲಾಯಿತು(buf))
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ml_check_b0_id(ZERO_BL *b0p)
{
    ಆದರೆ (b0p->b0_id[0] != BLOCK0_ID0
	    || (b0p->b0_id[1] != BLOCK0_ID1
		&& b0p->b0_id[1] != BLOCK0_ID1_C0
		&& b0p->b0_id[1] != BLOCK0_ID1_C1
		&& b0p->b0_id[1] != BLOCK0_ID1_C2
		&& b0p->b0_id[1] != BLOCK0_ID1_C3
		&& b0p->b0_id[1] != BLOCK0_ID1_C4)
	    )
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
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
    ಆದರೆ (mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    hp = mf_get(mfp, (blocknr_T)0, 1);
    ಆದರೆ (hp == ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	ಆದರೆ (what == UB_CRYPT)
	    ml_ಗಣ_mfp_crypt(buf);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ;
    }

    b0p = (ZERO_BL *)(hp->bh_ದತ್ತಾಂಶ);
    ಆದರೆ (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_ಖಂಡ_zero);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (what == UB_FNAME)
	    ಗಣ_b0_fname(b0p, buf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (what == UB_CRYPT)
	    ml_ಗಣ_b0_crypt(buf, b0p);
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ // what == UB_SAME_DIR
	    ಗಣ_b0_dir_flag(b0p, buf);
    }
    mf_put(mfp, hp, TRUE, FALSE);
}

/*
 * Write file name and timestamp into block 0 of a swap file.
 * Also set buf->b_mtime.
 * Don't use NameBuff[]!!!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_b0_fname(ZERO_BL *b0p, buf_T *buf)
{
    stat_T	st;

    ಆದರೆ (buf->b_ffname == ಖಾಲಿ)
	b0p->b0_fname[0] = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ಆದರೆ defined(MSWIN) || defined(AMIGA)
	// Systems that cannot translate "~user" back into a path: copy the
	// file name unmodified.  Do use slashes instead of backslashes for
	// portability.
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_ಗಾತ್ರ_CRYPT - 1);
# ifdef BACKSLASH_ಒಳ_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#ಇಲ್ಲದಿದ್ದರೆ
	size_t	flen;
	char_u	uname[B0_UNAME_ಗಾತ್ರ];

	/*
	 * For a file under the home directory of the current user, we try to
	 * replace the home directory path with "~user". This helps when
	 * editing the same file on different machines over a network.
	 * First replace home dir path with "~/" with home_replace().
	 * Then insert the user name to get "~user/".
	 */
	flen = home_replace(ಖಾಲಿ, buf->b_ffname, b0p->b0_fname,
						   B0_FNAME_ಗಾತ್ರ_CRYPT, TRUE);
	ಆದರೆ (b0p->b0_fname[0] == '~')
	{
	    size_t  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    ಆದರೆ (get_user_name(uname, B0_UNAME_ಗಾತ್ರ) == FAIL
		   || (ulen = STRLEN(uname)) + flen > B0_FNAME_ಗಾತ್ರ_CRYPT - 1)
		vim_strncpy(b0p->b0_fname, buf->b_ffname,
						     B0_FNAME_ಗಾತ್ರ_CRYPT - 1);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		mch_memmove(b0p->b0_fname + ulen + 1, b0p->b0_fname + 1, flen);
		mch_memmove(b0p->b0_fname + 1, uname, ulen);
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (mch_stat((ಅಕ್ಷರ *)buf->b_ffname, &st) >= 0)
	{
	    long_ಗಮ್ಯ_char((ದೀರ್ಘ)st.st_mtime, b0p->b0_mtime);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_INODE
	    long_ಗಮ್ಯ_char((ದೀರ್ಘ)st.st_ino, b0p->b0_ino);
#ಆದರೆ_ಅಂತ್ಯ
	    buf_store_ಸಮಯ(buf, &st, buf->b_ffname);
	    buf->b_mtime_read = buf->b_mtime;
	    buf->b_mtime_read_ns = buf->b_mtime_ns;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    long_ಗಮ್ಯ_char(0L, b0p->b0_mtime);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_INODE
	    long_ಗಮ್ಯ_char(0L, b0p->b0_ino);
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_mtime = 0;
	    buf->b_mtime_ns = 0;
	    buf->b_mtime_read = 0;
	    buf->b_mtime_read_ns = 0;
	    buf->b_orig_ಗಾತ್ರ = 0;
	    buf->b_orig_ಬಹುಲಕ = 0;
	}
    }

    // Also add the 'fileencoding' if there is room.
    ಕೂಡಿಸು_b0_fenc(b0p, curbuf);
}

/*
 * Update the B0_SAME_DIR flag of the swap file.  It's set if the file and the
 * swapfile for "buf" are in the same directory.
 * This is fail safe: if we are not sure the directories are equal the flag is
 * not set.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_b0_dir_flag(ZERO_BL *b0p, buf_T *buf)
{
    ಆದರೆ (same_directory(buf->b_ml.ml_mfp->mf_fname, buf->b_ffname))
	b0p->b0_flags |= B0_SAME_DIR;
    ಇಲ್ಲದಿದ್ದರೆ
	b0p->b0_flags &= ~B0_SAME_DIR;
}

/*
 * When there is room, add the 'fileencoding' to block zero.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_b0_fenc(
    ZERO_BL	*b0p,
    buf_T	*buf)
{
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		ಗಾತ್ರ_2 = B0_FNAME_ಗಾತ್ರ_NOCRYPT;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    ಆದರೆ (*buf->b_p_ಕೀಲಿ != NUL)
	ಗಾತ್ರ_2 = B0_FNAME_ಗಾತ್ರ_CRYPT;
#ಆದರೆ_ಅಂತ್ಯ

    n = (ಪೂರ್ಣಾಂಕ)STRLEN(buf->b_p_fenc);
    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)STRLEN(b0p->b0_fname) + n + 1 > ಗಾತ್ರ_2)
	b0p->b0_flags &= ~B0_HAS_FENC;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	mch_memmove((ಅಕ್ಷರ *)b0p->b0_fname + ಗಾತ್ರ_2 - n,
					    (ಅಕ್ಷರ *)buf->b_p_fenc, (size_t)n);
	*(b0p->b0_fname + ಗಾತ್ರ_2 - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#ಆದರೆ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
swapfile_process_running(ZERO_BL *b0p, char_u *ಅದಲುಬದಲು_fname ಬಳಸದ)
{
# ಆದರೆ defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    ರಚನೆ sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)ಅದಲುಬದಲು_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < ಸಮಯ(ಖಾಲಿ) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	ಹಿಂತಿರುಗಿ FALSE;
# endif
    ಹಿಂತಿರುಗಿ mch_process_running(char_ಗಮ್ಯ_long(b0p->b0_pid));
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Try to recover curbuf from the .swp file.
 * If "checkext" is TRUE, check the extension and detect whether it is
 * a swap file.
 */
    ಶೂನ್ಯ
ml_recover(ಪೂರ್ಣಾಂಕ checkext)
{
    buf_T	*buf = ಖಾಲಿ;
    memfile_T	*mfp = ಖಾಲಿ;
    char_u	*fname;
    char_u	*fname_ಬಳಸಿದ = ಖಾಲಿ;
    bhdr_T	*hp = ಖಾಲಿ;
    ZERO_BL	*b0p;
    ಪೂರ್ಣಾಂಕ		b0_ff;
    char_u	*b0_fenc = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    ಪೂರ್ಣಾಂಕ		b0_cm = -1;
#ಆದರೆ_ಅಂತ್ಯ
    PTR_BL	*pp;
    ದತ್ತಾಂಶ_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    ಪೂರ್ಣಾಂಕ		ಪುಟ_ಎಣಿಕೆ;
    stat_T	org_stat, swp_stat;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		directly;
    linenr_T	lnum;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
    ದೀರ್ಘ	error;
    ಪೂರ್ಣಾಂಕ		cannot_open;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		has_error;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		ತುದಿ;
    ಪೂರ್ಣಾಂಕ		txt_ಆರಂಭ;
    off_T	ಗಾತ್ರ_2;
    ಪೂರ್ಣಾಂಕ		called_from_main;
    ಪೂರ್ಣಾಂಕ		serious_error = TRUE;
    ದೀರ್ಘ	mtime;
    ಪೂರ್ಣಾಂಕ		attr;
    ಪೂರ್ಣಾಂಕ		orig_file_ಸ್ಥಿತಿ = NOTDONE;

    recoverymode = TRUE;
    called_from_main = (curbuf->b_ml.ml_mfp == ಖಾಲಿ);
    attr = HL_ATTR(HLF_E);

    /*
     * If the file name ends in ".s[a-w][a-z]" we assume this is the swap file.
     * Otherwise a search is done to find the swap file(s).
     */
    fname = curbuf->b_fname;
    ಆದರೆ (fname == ಖಾಲಿ)		    // When there is no file name
	fname = (char_u *)"";
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(fname);
    ಆದರೆ (checkext && len >= 4 &&
#ಆದರೆ defined(VMS)
	    STRNICMP(fname + len - 4, "_s", 2)
#ಇಲ್ಲದಿದ್ದರೆ
	    STRNICMP(fname + len - 4, ".s", 2)
#ಆದರೆ_ಅಂತ್ಯ
						== 0
		&& vim_strchr((char_u *)"abcdefghijklmnopqrstuvw",
					   TOLOWER_ASC(fname[len - 2])) != ಖಾಲಿ
		&& ASCII_ISALPHA(fname[len - 1]))
    {
	directly = TRUE;
	fname_ಬಳಸಿದ = vim_strsave(fname); // make a copy for mf_open()
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	directly = FALSE;

	// count the number of matching swap files
	len = recover_ಹೆಸರುಗಳು(fname, FALSE, ಖಾಲಿ, 0, ಖಾಲಿ);
	ಆದರೆ (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_ಅದಲುಬದಲು_file_ಸಿಕ್ಕಿತು_for_str), fname);
	    ಹೋಗು theend;
	}
	ಆದರೆ (len == 1)		    // one swap file found, use it
	    i = 1;
	ಇಲ್ಲದಿದ್ದರೆ			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (ಶೂನ್ಯ)recover_ಹೆಸರುಗಳು(fname, TRUE, ಖಾಲಿ, 0, ಖಾಲಿ);
	    ಸಂದೇಶ_putchar('\n');
	    ಸಂದೇಶ_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_ಸಂಖ್ಯೆ(FALSE, ಖಾಲಿ);
	    ಆದರೆ (i < 1 || i > len)
		ಹೋಗು theend;
	}
	// get the swap file name that will be used
	(ಶೂನ್ಯ)recover_ಹೆಸರುಗಳು(fname, FALSE, ಖಾಲಿ, i, &fname_ಬಳಸಿದ);
    }
    ಆದರೆ (fname_ಬಳಸಿದ == ಖಾಲಿ)
	ಹೋಗು theend;			// out of memory

    // When called from main() still need to initialize storage structure
    ಆದರೆ (called_from_main && ml_open(curbuf) == FAIL)
	getout(1);

    /*
     * Allocate a buffer structure for the swap file that is used for recovery.
     * Only the memline and crypt information in it are really used.
     */
    buf = ALLOC_ONE(buf_T);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹೋಗು theend;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ರಾಶಿ_ಗಾತ್ರ = 0;	// no stack yet
    buf->b_ml.ml_ರಾಶಿ = ಖಾಲಿ;		// no stack yet
    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;		// nothing in the stack
    buf->b_ml.ml_ಸಾಲು_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = ಖಾಲಿ;		// no locked block
    buf->b_ml.ml_flags = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    buf->b_p_ಕೀಲಿ = ಖಾಲಿಗಳು_option;
    buf->b_p_cm = ಖಾಲಿಗಳು_option;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_ಬಳಸಿದ); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_ಬಳಸಿದ, O_RDONLY);
    fname_ಬಳಸಿದ = p;
    ಆದರೆ (mfp == ಖಾಲಿ || mfp->mf_fd < 0)
    {
	ಆದರೆ (fname_ಬಳಸಿದ != ಖಾಲಿ)
	    semsg(_(e_cannot_open_str), fname_ಬಳಸಿದ);
	ಹೋಗು theend;
    }
    buf->b_ml.ml_mfp = mfp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    mfp->mf_ಬಫರ್ = buf;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * The page size set in mf_open() might be different from the page size
     * used in the swap file, we must get it from block 0.  But to read block
     * 0 we need a page size.  Use the minimal size for block 0 here, it will
     * be set to the real value below.
     */
    mfp->mf_ಪುಟ_ಗಾತ್ರ = MIN_ಅದಲುಬದಲು_ಪುಟ_ಗಾತ್ರ;

    /*
     * try to read block 0
     */
    ಆದರೆ ((hp = mf_get(mfp, (blocknr_T)0, 1)) == ಖಾಲಿ)
    {
	ಸಂದೇಶ_ಆರಂಭ();
	ಸಂದೇಶ_puts_attr(_("Unable to read block 0 from "), attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_outtrans_attr(mfp->mf_fname, attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_ಕೊನೆ();
	ಹೋಗು theend;
    }
    b0p = (ZERO_BL *)(hp->bh_ದತ್ತಾಂಶ);
    ಆದರೆ (STRNCMP(b0p->b0_ಆವೃತ್ತಿ, "VIM 3.0", 7) == 0)
    {
	ಸಂದೇಶ_ಆರಂಭ();
	ಸಂದೇಶ_outtrans_attr(mfp->mf_fname, ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_puts_attr(_("Use Vim version 3.0.\n"), ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_ಕೊನೆ();
	ಹೋಗು theend;
    }
    ಆದರೆ (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_ಅದಲುಬದಲು_file), mfp->mf_fname);
	ಹೋಗು theend;
    }
    ಆದರೆ (b0_magic_wrong(b0p))
    {
	ಸಂದೇಶ_ಆರಂಭ();
	ಸಂದೇಶ_outtrans_attr(mfp->mf_fname, attr | ಸಂದೇಶ_HIST);
#ಆದರೆ defined(MSWIN)
	ಆದರೆ (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    ಸಂದೇಶ_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | ಸಂದೇಶ_HIST);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಸಂದೇಶ_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_puts_attr(_("The file was created on "), attr | ಸಂದೇಶ_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	ಸಂದೇಶ_puts_attr((ಅಕ್ಷರ *)b0p->b0_hname, attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_puts_attr(_(",\nor the file has been damaged."), attr | ಸಂದೇಶ_HIST);
	ಸಂದೇಶ_ಕೊನೆ();
	ಹೋಗು theend;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(id1_codes); ++i)
	ಆದರೆ (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    ಆದರೆ (b0_cm > 0)
	mch_memmove(mfp->mf_ಬೀಜ, &b0p->b0_ಬೀಜ, MF_ಬೀಜ_LEN);
    crypt_ಗಣ_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_ಆವೃತ್ತಿ_ದ_vim_does_not_support_encryption), mfp->mf_fname);
	ಹೋಗು theend;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    ಆದರೆ (mfp->mf_ಪುಟ_ಗಾತ್ರ != (ಚಿಹ್ನೆರಹಿತ)char_ಗಮ್ಯ_long(b0p->b0_ಪುಟ_ಗಾತ್ರ))
    {
	ಚಿಹ್ನೆರಹಿತ ಹಿಂದಿನ_ಪುಟ_ಗಾತ್ರ = mfp->mf_ಪುಟ_ಗಾತ್ರ;

	mf_new_ಪುಟ_ಗಾತ್ರ(mfp, (ಚಿಹ್ನೆರಹಿತ)char_ಗಮ್ಯ_long(b0p->b0_ಪುಟ_ಗಾತ್ರ));
	ಆದರೆ (mfp->mf_ಪುಟ_ಗಾತ್ರ < ಹಿಂದಿನ_ಪುಟ_ಗಾತ್ರ)
	{
	    ಸಂದೇಶ_ಆರಂಭ();
	    ಸಂದೇಶ_outtrans_attr(mfp->mf_fname, attr | ಸಂದೇಶ_HIST);
	    ಸಂದೇಶ_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | ಸಂದೇಶ_HIST);
	    ಸಂದೇಶ_ಕೊನೆ();
	    ಹೋಗು theend;
	}
	ಆದರೆ ((ಗಾತ್ರ_2 = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	ಇಲ್ಲದಿದ್ದರೆ
	    mfp->mf_blocknr_max = (blocknr_T)(ಗಾತ್ರ_2 / mfp->mf_ಪುಟ_ಗಾತ್ರ);
	mfp->mf_infile_ಎಣಿಕೆ = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_ಪುಟ_ಗಾತ್ರ);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹೋಗು theend;
	mch_memmove(p, hp->bh_ದತ್ತಾಂಶ, ಹಿಂದಿನ_ಪುಟ_ಗಾತ್ರ);
	vim_free(hp->bh_ದತ್ತಾಂಶ);
	hp->bh_ದತ್ತಾಂಶ = p;
	b0p = (ZERO_BL *)(hp->bh_ದತ್ತಾಂಶ);
    }

    /*
     * If .swp file name given directly, use name from swap file for buffer.
     */
    ಆದರೆ (directly)
    {
	expand_env(b0p->b0_fname, NameBuff, MAXPATHL);
	ಆದರೆ (setfname(curbuf, NameBuff, ಖಾಲಿ, TRUE) == FAIL)
	    ಹೋಗು theend;
    }

    home_replace(ಖಾಲಿ, mfp->mf_fname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Using swap file \"%s\""), NameBuff);

    ಆದರೆ (buf_spname(curbuf) != ಖಾಲಿ)
	vim_strncpy(NameBuff, buf_spname(curbuf), MAXPATHL - 1);
    ಇಲ್ಲದಿದ್ದರೆ
	home_replace(ಖಾಲಿ, curbuf->b_ffname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Original file \"%s\""), NameBuff);
    ಸಂದೇಶ_putchar('\n');

    /*
     * check date of swap file and original file
     */
    mtime = char_ಗಮ್ಯ_long(b0p->b0_mtime);
    ಆದರೆ (curbuf->b_ffname != ಖಾಲಿ
	    && mch_stat((ಅಕ್ಷರ *)curbuf->b_ffname, &org_stat) != -1
	    && ((mch_stat((ಅಕ್ಷರ *)mfp->mf_fname, &swp_stat) != -1
		    && org_stat.st_mtime > swp_stat.st_mtime)
		|| org_stat.st_mtime != mtime))
	emsg(_(e_warning_original_file_may_have_been_ಬದಲಾಯಿತು));
    ಹೊರಹೋಗು_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_ಮುಖವಾಡ);
    ಆದರೆ (b0p->b0_flags & B0_HAS_FENC)
    {
	ಪೂರ್ಣಾಂಕ fnsize = B0_FNAME_ಗಾತ್ರ_NOCRYPT;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	ಆದರೆ (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_ಗಾತ್ರ_CRYPT;
#ಆದರೆ_ಅಂತ್ಯ
	ಗಾಗಿ (p = b0p->b0_fname + fnsize; p > b0p->b0_fname && p[-1] != NUL; --p)
	    ;
	b0_fenc = vim_strnsave(p, b0p->b0_fname + fnsize - p);
    }

    mf_put(mfp, hp, FALSE, FALSE);	// release block 0
    hp = ಖಾಲಿ;

    /*
     * Now that we are sure that the file is going to be recovered, clear the
     * contents of the current buffer.
     */
    ಆಗ (!(curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    ಆದರೆ (curbuf->b_ffname != ಖಾಲಿ)
	orig_file_ಸ್ಥಿತಿ = readfile(curbuf->b_ffname, ಖಾಲಿ, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, ಖಾಲಿ, READ_NEW);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    ಆದರೆ (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	ಆದರೆ (*curbuf->b_p_ಕೀಲಿ != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_ಬಳಸಿದ);
	    ಸಂದೇಶ_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    ಸಂದೇಶ_puts(_("\nenter the new crypt key."));
	    ಸಂದೇಶ_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    ಸಂದೇಶ_puts(_("\nto use the same key for text file and swap file"));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    smsg(_(need_ಕೀಲಿ_ಸಂದೇಶ), fname_ಬಳಸಿದ);
	buf->b_p_ಕೀಲಿ = crypt_get_ಕೀಲಿ(FALSE, FALSE);
	ಆದರೆ (buf->b_p_ಕೀಲಿ == ಖಾಲಿ)
	    buf->b_p_ಕೀಲಿ = curbuf->b_p_ಕೀಲಿ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*buf->b_p_ಕೀಲಿ == NUL)
	{
	    vim_free(buf->b_p_ಕೀಲಿ);
	    buf->b_p_ಕೀಲಿ = curbuf->b_p_ಕೀಲಿ;
	}
	ಆದರೆ (buf->b_p_ಕೀಲಿ == ಖಾಲಿ)
	    buf->b_p_ಕೀಲಿ = ಖಾಲಿಗಳು_option;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    ಆದರೆ (b0_ff != 0)
	ಗಣ_fileformat(b0_ff - 1, OPT_LOCAL);
    ಆದರೆ (b0_fenc != ಖಾಲಿ)
    {
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    ಪುಟ_ಎಣಿಕೆ = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    ಸಾಲು_ಎಣಿಕೆ = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;
    buf->b_ml.ml_ರಾಶಿ = ಖಾಲಿ;
    buf->b_ml.ml_ರಾಶಿ_ಗಾತ್ರ = 0;	// no stack yet

    ಆದರೆ (curbuf->b_ffname == ಖಾಲಿ)
	cannot_open = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	cannot_open = FALSE;

    serious_error = FALSE;
    ಗಾಗಿ ( ; !got_int; ಸಾಲು_breakcheck())
    {
	ಆದರೆ (hp != ಖಾಲಿ)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	ಆದರೆ ((hp = mf_get(mfp, bnum, ಪುಟ_ಎಣಿಕೆ)) == ಖಾಲಿ)
	{
	    ಆದರೆ (bnum == 1)
	    {
		semsg(_(e_unable_ಗಮ್ಯ_read_ಖಂಡ_one_from_str), mfp->mf_fname);
		ಹೋಗು theend;
	    }
	    ++error;
	    ml_append(lnum++, (char_u *)_("???MANY LINES MISSING"),
							    (colnr_T)0, TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ		// there is a block
	{
	    pp = (PTR_BL *)(hp->bh_ದತ್ತಾಂಶ);
	    ಆದರೆ (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		ಪೂರ್ಣಾಂಕ ptr_ಖಂಡ_error = FALSE;
		ಆದರೆ (pp->pb_ಎಣಿಕೆ_max != PB_ಎಣಿಕೆ_MAX(mfp))
		{
		    ptr_ಖಂಡ_error = TRUE;
		    pp->pb_ಎಣಿಕೆ_max = PB_ಎಣಿಕೆ_MAX(mfp);
		}
		ಆದರೆ (pp->pb_ಎಣಿಕೆ > pp->pb_ಎಣಿಕೆ_max)
		{
		    ptr_ಖಂಡ_error = TRUE;
		    pp->pb_ಎಣಿಕೆ = pp->pb_ಎಣಿಕೆ_max;
		}
		ಆದರೆ (ptr_ಖಂಡ_error)
		    emsg(_(e_warning_ಸೂಚಕ_ಖಂಡ_corrupted));

		// check line count when using pointer block first time
		ಆದರೆ (idx == 0 && ಸಾಲು_ಎಣಿಕೆ != 0)
		{
		    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)pp->pb_ಎಣಿಕೆ; ++i)
			ಸಾಲು_ಎಣಿಕೆ -= pp->pb_ಸೂಚಕ[i].pe_ಸಾಲು_ಎಣಿಕೆ;
		    ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		ಆದರೆ (pp->pb_ಎಣಿಕೆ == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx < (ಪೂರ್ಣಾಂಕ)pp->pb_ಎಣಿಕೆ)	// go a block deeper
		{
		    ಆದರೆ (pp->pb_ಸೂಚಕ[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			ಆದರೆ (!cannot_open)
			{
			    ಸಾಲು_ಎಣಿಕೆ = pp->pb_ಸೂಚಕ[idx].pe_ಸಾಲು_ಎಣಿಕೆ;
			    linenr_T pe_old_lnum = pp->pb_ಸೂಚಕ[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, ಖಾಲಿ, lnum,
					pe_old_lnum - 1,
					ಸಾಲು_ಎಣಿಕೆ, ಖಾಲಿ, 0) != OK)
				cannot_open = TRUE;
			    ಇಲ್ಲದಿದ್ದರೆ
				lnum += ಸಾಲು_ಎಣಿಕೆ;
			}
			ಆದರೆ (cannot_open)
			{
			    ++error;
			    ml_append(lnum++, (char_u *)_("???LINES MISSING"),
							    (colnr_T)0, TRUE);
			}
			++idx;	    // get same block again for next index
			ಮುಂದುವರಿ;
		    }

		    /*
		     * going one block deeper in the tree
		     */
		    ಆದರೆ ((ತುದಿ = ml_ಕೂಡಿಸು_ರಾಶಿ(buf)) < 0)	// new entry in stack
		    {
			++error;
			ನಿಲ್ಲಿಸು;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_ರಾಶಿ[ತುದಿ]);
		    ip->ip_bnum = bnum;
		    ip->ip_ಸೂಚ್ಯಂಕ = idx;

		    bnum = pp->pb_ಸೂಚಕ[idx].pe_bnum;
		    ಸಾಲು_ಎಣಿಕೆ = pp->pb_ಸೂಚಕ[idx].pe_ಸಾಲು_ಎಣಿಕೆ;
		    ಪುಟ_ಎಣಿಕೆ = pp->pb_ಸೂಚಕ[idx].pe_ಪುಟ_ಎಣಿಕೆ;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    ಆದರೆ (ಪುಟ_ಎಣಿಕೆ < 1
			    || bnum + ಪುಟ_ಎಣಿಕೆ > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_ಸೂಚ್ಯಂಕ + 1;
			bnum = ip->ip_bnum;
			ಪುಟ_ಎಣಿಕೆ = 1;
			--buf->b_ml.ml_ರಾಶಿ_ತುದಿ;
			ಮುಂದುವರಿ;
		    }
		    idx = 0;
		    ಮುಂದುವರಿ;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ	    // not a pointer block
	    {
		dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
		ಆದರೆ (dp->db_id != ದತ್ತಾಂಶ_ID)	// block id wrong
		{
		    ಆದರೆ (bnum == 1)
		    {
			semsg(_(e_ಖಂಡ_one_id_wrong_str_not_swp_file),
							       mfp->mf_fname);
			ಹೋಗು theend;
		    }
		    ++error;
		    ml_append(lnum++, (char_u *)_("???BLOCK MISSING"),
							    (colnr_T)0, TRUE);
		}
		ಇಲ್ಲದಿದ್ದರೆ
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
		    ಆದರೆ (hp->bh_ಪುಟ_ಎಣಿಕೆ != ಪುಟ_ಎಣಿಕೆ)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			ಪುಟ_ಎಣಿಕೆ = hp->bh_ಪುಟ_ಎಣಿಕೆ;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    ಆದರೆ (ಪುಟ_ಎಣಿಕೆ * mfp->mf_ಪುಟ_ಗಾತ್ರ != dp->db_txt_ಕೊನೆ)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_ಕೊನೆ = ಪುಟ_ಎಣಿಕೆ * mfp->mf_ಪುಟ_ಗಾತ್ರ;
		    }

		    ಆದರೆ (dp->db_txt_ಆರಂಭ < ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ
			    || dp->db_txt_ಆರಂಭ > dp->db_txt_ಕೊನೆ)
		    {
			ml_append(lnum++, (char_u *)_("??? block header corrupted"),
								    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_ಆರಂಭ = dp->db_txt_ಕೊನೆ;
		    }
		    // Make sure there is a NUL at the end of the block so we
		    // don't go over the end when copying text.
		    *((char_u *)dp + dp->db_txt_ಕೊನೆ - 1) = NUL;

		    // Check the number of lines in the block.
		    // If wrong, use the count in the data block.
		    ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ != dp->db_ಸಾಲು_ಎಣಿಕೆ)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    ಪೂರ್ಣಾಂಕ did_questions = FALSE;
		    ಗಾಗಿ (i = 0; i < dp->db_ಸಾಲು_ಎಣಿಕೆ; ++i)
		    {
			ಆದರೆ ((char_u *)&(dp->db_ಸೂಚ್ಯಂಕ[i])
					    >= (char_u *)dp + dp->db_txt_ಆರಂಭ)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    ನಿಲ್ಲಿಸು;
			}

			txt_ಆರಂಭ = (dp->db_ಸೂಚ್ಯಂಕ[i] & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
			ಆದರೆ (txt_ಆರಂಭ <= (ಪೂರ್ಣಾಂಕ)ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ
					  || txt_ಆರಂಭ >= (ಪೂರ್ಣಾಂಕ)dp->db_txt_ಕೊನೆ)
			{
			    ++error;
			    // avoid lots of lines with "???"
			    ಆದರೆ (did_questions)
				ಮುಂದುವರಿ;
			    did_questions = TRUE;
			    p = (char_u *)"???";
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    did_questions = FALSE;
			    p = (char_u *)dp + txt_ಆರಂಭ;
			}
			ml_append(lnum++, p, (colnr_T)0, TRUE);
		    }
		    ಆದರೆ (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	ಆದರೆ (buf->b_ml.ml_ರಾಶಿ_ತುದಿ == 0)	//ಪೂರ್ಣಗೊಂಡ
	    ನಿಲ್ಲಿಸು;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_ರಾಶಿ[--(buf->b_ml.ml_ರಾಶಿ_ತುದಿ)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_ಸೂಚ್ಯಂಕ + 1;	    // go to next index
	ಪುಟ_ಎಣಿಕೆ = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    ಆದರೆ (orig_file_ಸ್ಥಿತಿ != OK || curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	ಆದರೆ (!(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 2 && *ml_get(1) == NUL))
	{
	    ಬದಲಾಯಿತು_ಆಂತರಿಕ();
	    ++CHANGEDTICK(curbuf);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (idx = 1; idx <= lnum; ++idx)
	{
	    // Need to copy one line, fetching the other one may flush it.
	    p = vim_strnsave(ml_get(idx), ml_get_len(idx));
	    i = STRCMP(p, ml_get(idx + lnum));
	    vim_free(p);
	    ಆದರೆ (i != 0)
	    {
		ಬದಲಾಯಿತು_ಆಂತರಿಕ();
		++CHANGEDTICK(curbuf);
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    ಆಗ (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ > lnum
				       && !(curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು))
	ml_delete(curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    ಆದರೆ (got_int)
	emsg(_(e_recovery_interrupted));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (error)
    {
	++no_wait_return;
	ಸಂದೇಶ(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_ಸಾಲುಗಳು_starting_with_questions));
	--no_wait_return;
	ಸಂದೇಶ(_("See \":help E312\" for more information."));
	ಸಂದೇಶ(">>>>>>>>>>>>>");
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (curbuf->b_ಬದಲಾಯಿತು)
	{
	    ಸಂದೇಶ(_("Recovery completed. You should check if everything is OK."));
	    ಸಂದೇಶ_puts(_("\n(You might want to write out this file under another name\n"));
	    ಸಂದೇಶ_puts(_("and run diff with the original file to check for changes)"));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ(_("Recovery completed. Buffer contents equals file contents."));
	ಸಂದೇಶ_puts(_("\nYou may want to delete the .swp file now."));
#ಆದರೆ defined(UNIX) || defined(MSWIN)
	ಆದರೆ (swapfile_process_running(b0p, fname_ಬಳಸಿದ))
	{
	    // Warn there could be an active Vim on the same file, the user may
	    // want to kill it.
	    ಸಂದೇಶ_puts(_("\nNote: process STILL RUNNING: "));
	    ಸಂದೇಶ_outnum(char_ಗಮ್ಯ_long(b0p->b0_pid));
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಸಂದೇಶ_puts("\n\n");
	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    ಆದರೆ (*buf->b_p_ಕೀಲಿ != NUL && STRCMP(curbuf->b_p_ಕೀಲಿ, buf->b_p_ಕೀಲಿ) != 0)
    {
	ಸಂದೇಶ_puts(_("Using crypt key from swap file for the text file.\n"));
	ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"key", 0L, buf->b_p_ಕೀಲಿ, OPT_LOCAL);
    }
#ಆದರೆ_ಅಂತ್ಯ
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_ಬಳಸಿದ);
    recoverymode = FALSE;
    ಆದರೆ (mfp != ಖಾಲಿ)
    {
	ಆದರೆ (hp != ಖಾಲಿ)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    ಆದರೆ (buf != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	ಆದರೆ (buf->b_p_ಕೀಲಿ != curbuf->b_p_ಕೀಲಿ)
	    free_string_option(buf->b_p_ಕೀಲಿ);
	free_string_option(buf->b_p_cm);
#ಆದರೆ_ಅಂತ್ಯ
	vim_free(buf->b_ml.ml_ರಾಶಿ);
	vim_free(buf);
    }
    ಆದರೆ (serious_error && called_from_main)
	ml_close(curbuf, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	apply_autocmds(EVENT_BUFREADPOST, ಖಾಲಿ, curbuf->b_fname, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ಖಾಲಿ, curbuf->b_fname, FALSE, curbuf);
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
    ಪೂರ್ಣಾಂಕ
recover_ಹೆಸರುಗಳು(
    char_u	*fname,		// base for swap file name
    ಪೂರ್ಣಾಂಕ		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list ಬಳಸದ, // when not NULL add file names to it
    ಪೂರ್ಣಾಂಕ		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_ಹೊರಹೋಗು)	// result when "nr" > 0
{
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಹೆಸರುಗಳು;
    char_u	*(ಹೆಸರುಗಳು[6]);
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_files;
    ಪೂರ್ಣಾಂಕ		file_ಎಣಿಕೆ = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (fname != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	ಆದರೆ (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    fname_res = fname;
    }

    ಆದರೆ (do_list)
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
    ಆಗ (dir_name.string != ಖಾಲಿ && *dirp)
    {
	/*
	 * Isolate a directory name from *dirp and put it in dir_name (we know
	 * it is large enough, so use 31000 for length).
	 * Advance dirp to next directory name.
	 */
	dir_name.ಉದ್ದಳತೆ = (size_t)copy_option_part(&dirp, dir_name.string, 31000, ",");

	ಆದರೆ (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    ಆದರೆ (fname == ಖಾಲಿ)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
		ಹೆಸರುಗಳು[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#ಇಲ್ಲದಿದ್ದರೆ
		ಹೆಸರುಗಳು[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		ಹೆಸರುಗಳು[1] = vim_strnsave((char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"));
		ಹೆಸರುಗಳು[2] = vim_strnsave((char_u *)".sw?", STRLEN_LITERAL(".sw?"));
		ಅಂಕೆ_ಹೆಸರುಗಳು = 3;
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef VMS
		ಹೆಸರುಗಳು[1] = vim_strnsave((char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"));
		ಅಂಕೆ_ಹೆಸರುಗಳು = 2;
# ಇಲ್ಲದಿದ್ದರೆ
		ಅಂಕೆ_ಹೆಸರುಗಳು = 1;
# endif
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಅಂಕೆ_ಹೆಸರುಗಳು = recov_file_ಹೆಸರುಗಳು(ಹೆಸರುಗಳು, fname_res, TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ			    // check directory dir_name
	{
	    ಆದರೆ (fname == ಖಾಲಿ)
	    {
		string_T    ret;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
		ಹೆಸರುಗಳು[0] = concat_fnames(dir_name.string, dir_name.ಉದ್ದಳತೆ,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#ಇಲ್ಲದಿದ್ದರೆ
		ಹೆಸರುಗಳು[0] = concat_fnames(dir_name.string, dir_name.ಉದ್ದಳತೆ,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		ಹೆಸರುಗಳು[1] = concat_fnames(dir_name.string, dir_name.ಉದ್ದಳತೆ,
		    (char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"), TRUE, &ret);
		ಹೆಸರುಗಳು[2] = concat_fnames(dir_name.string, dir_name.ಉದ್ದಳತೆ,
		    (char_u *)".sw?", STRLEN_LITERAL(".sw?"), TRUE, &ret);
		ಅಂಕೆ_ಹೆಸರುಗಳು = 3;
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef VMS
		ಹೆಸರುಗಳು[1] = concat_fnames(dir_name.string, dir_name.ಉದ್ದಳತೆ,
		    (char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"), TRUE, &ret);
		ಅಂಕೆ_ಹೆಸರುಗಳು = 2;
# ಇಲ್ಲದಿದ್ದರೆ
		ಅಂಕೆ_ಹೆಸರುಗಳು = 1;
# endif
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		char_u	*ಬಾಲ;

#ಆದರೆ defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.ಉದ್ದಳತೆ;
		ಆದರೆ (after_pathsep(dir_name.string, p) && dir_name.ಉದ್ದಳತೆ > 1 && p[-1] == p[-2])
		{
		    // Ends with '//', Use Full path for swap name
		    ಬಾಲ = make_percent_swname(dir_name.string, p, fname_res);
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		{
		    string_T	ret;

		    ಬಾಲ = gettail(fname_res);
		    ಬಾಲ = concat_fnames(dir_name.string, dir_name.ಉದ್ದಳತೆ,
			ಬಾಲ, STRLEN(ಬಾಲ), TRUE, &ret);
		}
		ಆದರೆ (ಬಾಲ == ಖಾಲಿ)
		    ಅಂಕೆ_ಹೆಸರುಗಳು = 0;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಅಂಕೆ_ಹೆಸರುಗಳು = recov_file_ಹೆಸರುಗಳು(ಹೆಸರುಗಳು, ಬಾಲ, FALSE);
		    vim_free(ಬಾಲ);
		}
	    }
	}

	// check for out-of-memory
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_ಹೆಸರುಗಳು; ++i)
	{
	    ಆದರೆ (ಹೆಸರುಗಳು[i] == ಖಾಲಿ)
	    {
		ಗಾಗಿ (i = 0; i < ಅಂಕೆ_ಹೆಸರುಗಳು; ++i)
		    vim_free(ಹೆಸರುಗಳು[i]);
		ಅಂಕೆ_ಹೆಸರುಗಳು = 0;
	    }
	}
	ಆದರೆ (ಅಂಕೆ_ಹೆಸರುಗಳು == 0)
	    ಅಂಕೆ_files = 0;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (expand_wildcards(ಅಂಕೆ_ಹೆಸರುಗಳು, ಹೆಸರುಗಳು, &ಅಂಕೆ_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    ಅಂಕೆ_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	ಆದರೆ (*dirp == NUL && file_ಎಣಿಕೆ + ಅಂಕೆ_files == 0 && fname != ಖಾಲಿ)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#ಆದರೆ defined(VMS)
			       (char_u *)"_swp", FALSE
#ಇಲ್ಲದಿದ್ದರೆ
			       (char_u *)".swp", TRUE
#ಆದರೆ_ಅಂತ್ಯ
			      );
	    ಆದರೆ (swapname != ಖಾಲಿ)
	    {
		ಆದರೆ (mch_stat((ಅಕ್ಷರ *)swapname, &st) != -1)    // It exists!
		{
		    files = ALLOC_ONE(char_u *);
		    ಆದರೆ (files != ಖಾಲಿ)
		    {
			files[0] = swapname;
			swapname = ಖಾಲಿ;
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
	ಆದರೆ (curbuf->b_ml.ml_mfp != ಖಾಲಿ
			       && (p = curbuf->b_ml.ml_mfp->mf_fname) != ಖಾಲಿ
			       && ret_list == ಖಾಲಿ)
	{
	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_files; ++i)
		// Do not expand wildcards, on windows would try to expand
		// "%tmp%" in "%tmp%file".
		ಆದರೆ (fullpathcmp(p, files[i], TRUE, FALSE) & FPC_SAME)
		{
		    // Remove the name from files[i].  Move further entries
		    // down.  When the array becomes empty free it here, since
		    // FreeWild() won't be called below.
		    vim_free(files[i]);
		    ಆದರೆ (--ಅಂಕೆ_files == 0)
			vim_free(files);
		    ಇಲ್ಲದಿದ್ದರೆ
			ಗಾಗಿ ( ; i < ಅಂಕೆ_files; ++i)
			    files[i] = files[i + 1];
		}
	}
	ಆದರೆ (nr > 0)
	{
	    file_ಎಣಿಕೆ += ಅಂಕೆ_files;
	    ಆದರೆ (nr <= file_ಎಣಿಕೆ)
	    {
		*fname_ಹೊರಹೋಗು = vim_strsave(
				      files[nr - 1 + ಅಂಕೆ_files - file_ಎಣಿಕೆ]);
		dirp = (char_u *)"";		    // stop searching
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_list)
	{
	    ಆದರೆ (dir_name.string[0] == '.' && dir_name.string[1] == NUL)
	    {
		ಆದರೆ (fname == ಖಾಲಿ)
		    ಸಂದೇಶ_puts(_("   In current directory:\n"));
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ_puts(_("   Using specified name:\n"));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಸಂದೇಶ_puts(_("   In directory "));
		ಸಂದೇಶ_home_replace(dir_name.string);
		ಸಂದೇಶ_puts(":\n");
	    }

	    ಆದರೆ (ಅಂಕೆ_files)
	    {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_files; ++i)
		{
		    // print the swap file name
		    ಸಂದೇಶ_outnum((ದೀರ್ಘ)++file_ಎಣಿಕೆ);
		    ಸಂದೇಶ_puts(".    ");
		    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)gettail(files[i]));
		    ಸಂದೇಶ_putchar('\n');
		    (ಶೂನ್ಯ)swapfile_info(files[i]);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ_puts(_("      -- none --\n"));
	    ಹೊರಹೋಗು_flush();
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret_list != ಖಾಲಿ)
	{
	    string_T	name;

	    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.ಉದ್ದಳತೆ,
		    files[i], STRLEN(files[i]), TRUE, &name);
		ಆದರೆ (name.string != ಖಾಲಿ)
		{
		    list_append_string(ret_list, name.string, (ಪೂರ್ಣಾಂಕ)name.ಉದ್ದಳತೆ);
		    vim_free(name.string);
		}
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ
	    file_ಎಣಿಕೆ += ಅಂಕೆ_files;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_ಹೆಸರುಗಳು; ++i)
	    vim_free(ಹೆಸರುಗಳು[i]);
	ಆದರೆ (ಅಂಕೆ_files > 0)
	    FreeWild(ಅಂಕೆ_files, files);
    }
    vim_free(dir_name.string);
    ಹಿಂತಿರುಗಿ file_ಎಣಿಕೆ;
}

#ಆದರೆ defined(UNIX) || defined(MSWIN)
/*
 * Need _very_ long file names.
 * Append the full path to name with path separators made into percent
 * signs, to "dir". An unnamed buffer is handled as "" (<currentdir>/"")
 * The last character in "dir" must be an extra slash or backslash, it is
 * removed.
 */
    char_u *
make_percent_swname(char_u *dir, char_u *dir_ಕೊನೆ, char_u *name)
{
    string_T	d = {ಖಾಲಿ, 0};
    string_T	fixed_fname;
    char_u	*p;

    fixed_fname.string = fix_fname(name != ಖಾಲಿ ? name : (char_u *)"");
    ಆದರೆ (fixed_fname.string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (p = fixed_fname.string; *p != NUL; MB_PTR_ADV(p))
	ಆದರೆ (vim_ispathsep(*p))
	    *p = '%';
    fixed_fname.ಉದ್ದಳತೆ = (size_t)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_ಕೊನೆ[-1];
    *p = NUL;
    concat_fnames(dir, (size_t)(p - dir), fixed_fname.string, fixed_fname.ಉದ್ದಳತೆ, TRUE, &d);
    vim_free(fixed_fname.string);

    ಹಿಂತಿರುಗಿ d.string;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ process_still_running;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return information found in swapfile "fname" in dictionary "d".
 * This is used by the swapinfo() function.
 */
    ಶೂನ್ಯ
get_b0_dict(char_u *fname, dict_T *d)
{
    ಪೂರ್ಣಾಂಕ fd;
    ರಚನೆ block0 b0;

    ಆದರೆ ((fd = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
    {
	ಆದರೆ (read_eintr(fd, &b0, ಗಾತ್ರ(b0)) == ಗಾತ್ರ(b0))
	{
	    ಆದರೆ (ml_check_b0_id(&b0) == FAIL)
		dict_ಕೂಡಿಸು_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (b0_magic_wrong(&b0))
		dict_ಕೂಡಿಸು_string_len(d, "error",
		    (char_u *)"Magic number mismatch", STRLEN_LITERAL("Magic number mismatch"));
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// we have swap information
		dict_ಕೂಡಿಸು_string_len(d, "version", b0.b0_ಆವೃತ್ತಿ, 10);
		dict_ಕೂಡಿಸು_string_len(d, "user", b0.b0_uname, B0_UNAME_ಗಾತ್ರ);
		dict_ಕೂಡಿಸು_string_len(d, "host", b0.b0_hname, B0_HNAME_ಗಾತ್ರ);
		dict_ಕೂಡಿಸು_string_len(d, "fname", b0.b0_fname, B0_FNAME_ಗಾತ್ರ_ORG);

		dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "pid", char_ಗಮ್ಯ_long(b0.b0_pid));
		dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "mtime", char_ಗಮ್ಯ_long(b0.b0_mtime));
		dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "dirty", b0.b0_dirty ? 1 : 0);
# ifdef CHECK_INODE
		dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "inode", char_ಗಮ್ಯ_long(b0.b0_ino));
# endif
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    dict_ಕೂಡಿಸು_string_len(d, "error", (char_u *)"Cannot read file", STRLEN_LITERAL("Cannot read file"));
	close(fd);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	dict_ಕೂಡಿಸು_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Give information about an existing swap file.
 * Returns timestamp (0 when unknown).
 */
    ಸ್ಥಿರ time_t
swapfile_info(char_u *fname)
{
    stat_T	    st;
    ಪೂರ್ಣಾಂಕ		    fd;
    ರಚನೆ block0   b0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    char_u	    uname[B0_UNAME_ಗಾತ್ರ];
#ಆದರೆ_ಅಂತ್ಯ

    // print the swap file date
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st) != -1)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	// print name of owner of the file
	ಆದರೆ (mch_get_uname(st.st_uid, uname, B0_UNAME_ಗಾತ್ರ) == OK)
	{
	    ಸಂದೇಶ_puts(_("          owned by: "));
	    ಸಂದೇಶ_outtrans(uname);
	    ಸಂದೇಶ_puts(_("   dated: "));
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಸಂದೇಶ_puts(_("             dated: "));
	ಸಂದೇಶ_puts(get_ctime(st.st_mtime, TRUE));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	st.st_mtime = 0;

    /*
     * print the original file name
     */
    fd = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_EXTRA, 0);
    ಆದರೆ (fd >= 0)
    {
	ಆದರೆ (read_eintr(fd, &b0, ಗಾತ್ರ(b0)) == ಗಾತ್ರ(b0))
	{
	    ಆದರೆ (STRNCMP(b0.b0_ಆವೃತ್ತಿ, "VIM 3.0", 7) == 0)
	    {
		ಸಂದೇಶ_puts(_("         [from Vim version 3.0]"));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ml_check_b0_id(&b0) == FAIL)
	    {
		ಸಂದೇಶ_puts(_("         [does not look like a Vim swap file]"));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಸಂದೇಶ_puts(_("         file name: "));
		ಆದರೆ (b0.b0_fname[0] == NUL)
		    ಸಂದೇಶ_puts(_("[No Name]"));
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ_outtrans(b0.b0_fname);

		ಸಂದೇಶ_puts(_("\n          modified: "));
		ಸಂದೇಶ_puts(b0.b0_dirty ? _("YES") : _("no"));

		ಆದರೆ (*(b0.b0_uname) != NUL)
		{
		    ಸಂದೇಶ_puts(_("\n         user name: "));
		    ಸಂದೇಶ_outtrans(b0.b0_uname);
		}

		ಆದರೆ (*(b0.b0_hname) != NUL)
		{
		    ಆದರೆ (*(b0.b0_uname) != NUL)
			ಸಂದೇಶ_puts(_("   host name: "));
		    ಇಲ್ಲದಿದ್ದರೆ
			ಸಂದೇಶ_puts(_("\n         host name: "));
		    ಸಂದೇಶ_outtrans(b0.b0_hname);
		}

		ಆದರೆ (char_ಗಮ್ಯ_long(b0.b0_pid) != 0L)
		{
		    ಸಂದೇಶ_puts(_("\n        process ID: "));
		    ಸಂದೇಶ_outnum(char_ಗಮ್ಯ_long(b0.b0_pid));
#ಆದರೆ defined(UNIX) || defined(MSWIN)
		    ಆದರೆ (swapfile_process_running(&b0, fname))
		    {
			ಸಂದೇಶ_puts(_(" (STILL RUNNING)"));
# ifdef HAVE_PROCESS_STILL_RUNNING
			process_still_running = TRUE;
# endif
		    }
#ಆದರೆ_ಅಂತ್ಯ
		}

		ಆದರೆ (b0_magic_wrong(&b0))
		{
#ಆದರೆ defined(MSWIN)
		    ಆದರೆ (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			ಸಂದೇಶ_puts(_("\n         [not usable with this version of Vim]"));
		    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
			ಸಂದೇಶ_puts(_("\n         [not usable on this computer]"));
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ_puts(_("         [cannot be read]"));
	close(fd);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ_puts(_("         [cannot be opened]"));
    ಸಂದೇಶ_putchar('\n');

    ಹಿಂತಿರುಗಿ st.st_mtime;
}

/*
 * Return TRUE if the swap file looks OK and there are no changes, thus it can
 * be safely deleted.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
swapfile_unchanged(char_u *fname)
{
    stat_T	    st;
    ಪೂರ್ಣಾಂಕ		    fd;
    ರಚನೆ block0   b0;
    ಪೂರ್ಣಾಂಕ		    ret = TRUE;

    // must be able to stat the swap file
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname, &st) == -1)
	ಹಿಂತಿರುಗಿ FALSE;

    // must be able to read the first block
    fd = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_EXTRA, 0);
    ಆದರೆ (fd < 0)
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (read_eintr(fd, &b0, ಗಾತ್ರ(b0)) != ಗಾತ್ರ(b0))
    {
	close(fd);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // the ID and magic number must be correct
    ಆದರೆ (ml_check_b0_id(&b0) == FAIL|| b0_magic_wrong(&b0))
	ret = FALSE;

    // must be unchanged
    ಆದರೆ (b0.b0_dirty)
	ret = FALSE;

#ಆದರೆ defined(UNIX) || defined(MSWIN)
    // Host name must be known and must equal the current host name, otherwise
    // comparing pid is meaningless.
    ಆದರೆ (*(b0.b0_hname) == NUL)
    {
	ret = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	    hostname[B0_HNAME_ಗಾತ್ರ];

	mch_get_host_name(hostname, B0_HNAME_ಗಾತ್ರ);
	hostname[B0_HNAME_ಗಾತ್ರ - 1] = NUL;
	b0.b0_hname[B0_HNAME_ಗಾತ್ರ - 1] = NUL; // in case of corruption
	ಆದರೆ (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    ಆದರೆ (char_ಗಮ್ಯ_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    close(fd);
    ಹಿಂತಿರುಗಿ ret;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
recov_file_ಹೆಸರುಗಳು(char_u **ಹೆಸರುಗಳು, char_u *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ prepend_dot)
{
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಹೆಸರುಗಳು;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		i;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
    bool    shortname = curbuf->b_shortname;

    curbuf->b_shortname = false;
#ಆದರೆ_ಅಂತ್ಯ
    string_T	ret;

    ಅಂಕೆ_ಹೆಸರುಗಳು = 0;

    /*
     * May also add the file name with a dot prepended, for swap file in same
     * dir as original file.
     */
    ಆದರೆ (prepend_dot)
    {
	ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು] = modname(ಮಾರ್ಗ, (char_u *)".sw?", TRUE);
	ಆದರೆ (ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು] == ಖಾಲಿ)
	    ಹೋಗು ಕೊನೆ;
	++ಅಂಕೆ_ಹೆಸರುಗಳು;
    }

    /*
     * Form the normal swap file name pattern by appending ".sw?".
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು] = concat_fnames(ಮಾರ್ಗ, STRLEN(ಮಾರ್ಗ),
	(char_u *)"_sw%", STRLEN_LITERAL("_sw%"), FALSE, &ret);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು] = concat_fnames(ಮಾರ್ಗ, STRLEN(ಮಾರ್ಗ),
	(char_u *)".sw?", STRLEN_LITERAL(".sw?"), FALSE, &ret);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು] == ಖಾಲಿ)
	ಹೋಗು ಕೊನೆ;
    ಆದರೆ (ಅಂಕೆ_ಹೆಸರುಗಳು >= 1)	    // check if we have the same name twice
    {
	p = ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು - 1];
	i = (ಪೂರ್ಣಾಂಕ)STRLEN(ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು - 1]) - (ಪೂರ್ಣಾಂಕ)STRLEN(ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು]);
	ಆದರೆ (i > 0)
	    p += i;	    // file name has been expanded to full path

	ಆದರೆ (STRCMP(p, ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು]) != 0)
	    ++ಅಂಕೆ_ಹೆಸರುಗಳು;
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_free(ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು]);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	++ಅಂಕೆ_ಹೆಸರುಗಳು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
    /*
     * Also try with 'shortname' set, in case the file is on a DOS filesystem.
     */
    curbuf->b_shortname = true;
# ifdef VMS
    ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು] = modname(ಮಾರ್ಗ, (char_u *)"_sw%", FALSE);
# ಇಲ್ಲದಿದ್ದರೆ
    ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು] = modname(ಮಾರ್ಗ, (char_u *)".sw?", FALSE);
# endif
    ಆದರೆ (ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು] == ಖಾಲಿ)
	ಹೋಗು ಕೊನೆ;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು];
    i = STRLEN(ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು]) - STRLEN(ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು - 1]);
    ಆದರೆ (i > 0)
	p += i;		// file name has been expanded to full path
    ಆದರೆ (STRCMP(ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು - 1], p) == 0)
	vim_free(ಹೆಸರುಗಳು[ಅಂಕೆ_ಹೆಸರುಗಳು]);
    ಇಲ್ಲದಿದ್ದರೆ
	++ಅಂಕೆ_ಹೆಸರುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ

ಕೊನೆ:
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
    curbuf->b_shortname = shortname;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಅಂಕೆ_ಹೆಸರುಗಳು;
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
    ಶೂನ್ಯ
ml_sync_all(ಪೂರ್ಣಾಂಕ check_file, ಪೂರ್ಣಾಂಕ check_char)
{
    buf_T		*buf;
    stat_T		st;

    FOR_ALL_BUFFERS(buf)
    {
	ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ
		|| buf->b_ml.ml_mfp->mf_fname == ಖಾಲಿ
		|| buf->b_ml.ml_mfp->mf_fd < 0)
	    ಮುಂದುವರಿ;			    // no file

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
	ಆದರೆ (crypt_may_close_swapfile(buf, buf->b_p_ಕೀಲಿ,
						     crypt_get_ವಿಧಾನ_nr(buf)))
	    ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ

	ml_flush_ಸಾಲು(buf);		    // flush buffered line
					    // flush locked block
	(ಶೂನ್ಯ)ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, (linenr_T)0, ML_FLUSH);
	ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != ಖಾಲಿ)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)buf->b_ffname, &st) == -1
		    || st.st_mtime != buf->b_mtime_read
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ST_MTIM_NSEC
		    || st.ST_MTIM_NSEC != buf->b_mtime_read_ns
#ಆದರೆ_ಅಂತ್ಯ
		    || st.st_size != buf->b_orig_ಗಾತ್ರ)
	    {
		ml_preserve(buf, FALSE);
		did_check_timestamps = FALSE;
		need_check_timestamps = TRUE;	// give message later
	    }
	}
	ಆದರೆ (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (ಶೂನ್ಯ)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (buf_Is_ಬದಲಾಯಿತು(buf) ? MFS_FLUSH : 0));
	    ಆದರೆ (check_char && ui_char_avail())	// character available now
		ನಿಲ್ಲಿಸು;
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
ml_preserve(buf_T *buf, ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_2)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
    ಪೂರ್ಣಾಂಕ		got_int_save = got_int;

    ಆದರೆ (mfp == ಖಾಲಿ || mfp->mf_fname == ಖಾಲಿ)
    {
	ಆದರೆ (ಸಂದೇಶ_2)
	    emsg(_(e_cannot_preserve_there_is_no_ಅದಲುಬದಲು_file));
	ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    ಆದರೆ (crypt_may_close_swapfile(buf, buf->b_p_ಕೀಲಿ, crypt_get_ವಿಧಾನ_nr(buf)))
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_ಸಾಲು(buf);				    // flush buffered line
    (ಶೂನ್ಯ)ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    ಸ್ಥಿತಿ_2 = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

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
    ಆದರೆ (mf_need_trans(mfp) && !got_int)
    {
	lnum = 1;
	ಆಗ (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, lnum, ML_ಕಂಡುಹಿಡಿ);
	    ಆದರೆ (hp == ಖಾಲಿ)
	    {
		ಸ್ಥಿತಿ_2 = FAIL;
		ಹೋಗು theend;
	    }
	    CHECK(buf->b_ml.ml_locked_ಕೆಳಗಿನ != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_ಮೇಲಿನ + 1;
	}
	(ಶೂನ್ಯ)ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	ಆದರೆ (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    ಸ್ಥಿತಿ_2 = FAIL;
	buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    ಆದರೆ (ಸಂದೇಶ_2)
    {
	ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
	    ಸಂದೇಶ(_("File preserved"));
	ಇಲ್ಲದಿದ್ದರೆ
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
    ಹಿಂತಿರುಗಿ ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
    char_u *
ml_get_pos(pos_T *pos)
{
    ಹಿಂತಿರುಗಿ (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->ಲಂಬಸಾಲು);
}

/*
 * Return pointer to cursor line.
 */
    char_u *
ml_get_curline(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE);
}

/*
 * Return pointer to cursor position.
 */
    char_u *
ml_get_cursor(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE) +
							curwin->w_cursor.ಲಂಬಸಾಲು);
}

// return length (excluding the NUL) of the given line
    colnr_T
ml_get_len(linenr_T lnum)
{
    ಹಿಂತಿರುಗಿ ml_get_buf_len(curbuf, lnum);
}

// return length (excluding the NUL) of the text after position "pos"
    colnr_T
ml_get_pos_len(pos_T *pos)
{
    ಹಿಂತಿರುಗಿ ml_get_buf_len(curbuf, pos->lnum) - pos->ಲಂಬಸಾಲು;
}

// return length (excluding the NUL) of the cursor line
    colnr_T
ml_get_curline_len(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ml_get_buf_len(curbuf, curwin->w_cursor.lnum);
}

// return length (excluding the NUL) of the cursor position
    colnr_T
ml_get_cursor_len(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.ಲಂಬಸಾಲು;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*ಸಾಲು;

    ಆದರೆ (*(ಸಾಲು = ml_get_buf(buf, lnum, FALSE)) == NUL)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (buf->b_ml.ml_ಸಾಲು_textlen <= 0)
	buf->b_ml.ml_ಸಾಲು_textlen = (ಪೂರ್ಣಾಂಕ)STRLEN(ಸಾಲು) + 1;
    ಹಿಂತಿರುಗಿ buf->b_ml.ml_ಸಾಲು_textlen - 1;
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
    ಪೂರ್ಣಾಂಕ		will_change)		// line will be changed
{
    bhdr_T	*hp;
    ದತ್ತಾಂಶ_BL	*dp;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = 0;
    ಸ್ಥಿರ char_u questions[4];

    ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)	// invalid line number
    {
	ಆದರೆ (recursive == 0)
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
	ಹಿಂತಿರುಗಿ questions;
    }
    ಆದರೆ (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)	// there are no lines
    {
	buf->b_ml.ml_ಸಾಲು_len = 1;
	buf->b_ml.ml_ಸಾಲು_textlen = buf->b_ml.ml_ಸಾಲು_len;
	ಹಿಂತಿರುಗಿ (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    ಆದರೆ (buf->b_ml.ml_ಸಾಲು_lnum != lnum || mf_dont_release)
    {
	ಚಿಹ್ನೆರಹಿತ    ಆರಂಭ, ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ	    idx;

	ml_flush_ಸಾಲು(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	ಆದರೆ ((hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, lnum, ML_ಕಂಡುಹಿಡಿ)) == ಖಾಲಿ)
	{
	    ಆದರೆ (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_ಕಂಡುಹಿಡಿ_ಸಾಲು_nr_ಒಳ_ಬಫರ್_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    ಹೋಗು errorret;
	}

	dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);

	idx = lnum - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
	ಆರಂಭ = ((dp->db_ಸೂಚ್ಯಂಕ[idx]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	ಆದರೆ (idx == 0)
	    ಕೊನೆ = dp->db_txt_ಕೊನೆ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೊನೆ = ((dp->db_ಸೂಚ್ಯಂಕ[idx - 1]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);

	buf->b_ml.ml_ಸಾಲು_ptr = (char_u *)dp + ಆರಂಭ;
	buf->b_ml.ml_ಸಾಲು_len = ಕೊನೆ - ಆರಂಭ;
#ಆದರೆ defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	ಆದರೆ (buf->b_has_textprop
			 && (size_t)buf->b_ml.ml_ಸಾಲು_len > ಗಾತ್ರ(textprop_T))
	    buf->b_ml.ml_ಸಾಲು_textlen = 0;  // call STRLEN() later when needed
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    buf->b_ml.ml_ಸಾಲು_textlen = buf->b_ml.ml_ಸಾಲು_len;
	buf->b_ml.ml_ಸಾಲು_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_ಸಾಲು_DIRTY | ML_ALLOCATED);
    }
    ಆದರೆ (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (ml_get_alloc_ಸಾಲುಗಳು && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (ml_get_alloc_ಸಾಲುಗಳು
		 && (buf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_ಸಾಲು_len);

	// make sure the text is in allocated memory
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    memmove(p, buf->b_ml.ml_ಸಾಲು_ptr, buf->b_ml.ml_ಸಾಲು_len);
	    buf->b_ml.ml_ಸಾಲು_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    ಆದರೆ (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_ಸಾಲು_DIRTY;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ buf->b_ml.ml_ಸಾಲು_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    ಪೂರ್ಣಾಂಕ
ml_ಸಾಲು_alloced(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (curbuf->b_ml.ml_flags & ML_ಸಾಲು_DIRTY);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_ಪಠ್ಯ_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**ಸಾಲು,
	    ಪೂರ್ಣಾಂಕ		*len,
	    char_u	**tofree)
{
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
    ಪೂರ್ಣಾಂಕ		new_prop_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		n;
    char_u	*props;
    ಪೂರ್ಣಾಂಕ		new_len = 0;  // init for gcc
    char_u	*new_ಸಾಲು = ಖಾಲಿ;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	ಆದರೆ (ಸುತ್ತು == 2)
	{
	    uint16_t pc;

	    ಆದರೆ (new_prop_ಎಣಿಕೆ == 0)
		ಹಿಂತಿರುಗಿ;  // nothing to do
	    new_len = *len + (ಪೂರ್ಣಾಂಕ)PROP_ಎಣಿಕೆ_ಗಾತ್ರ
			     + new_prop_ಎಣಿಕೆ * (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(textprop_T);
	    new_ಸಾಲು = alloc(new_len);
	    ಆದರೆ (new_ಸಾಲು == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    mch_memmove(new_ಸಾಲು, *ಸಾಲು, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_ಎಣಿಕೆ;
	    mch_memmove(new_ಸಾಲು + *len, &pc, PROP_ಎಣಿಕೆ_ಗಾತ್ರ);
	    new_prop_ಎಣಿಕೆ = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	ಎಣಿಕೆ_2 = get_ಪಠ್ಯ_props(buf, lnum, &props, FALSE);
	ಗಾಗಿ (n = 0; n < ಎಣಿಕೆ_2; ++n)
	{
	    mch_memmove(&prop, props + n * ಗಾತ್ರ(textprop_T),
							   ಗಾತ್ರ(textprop_T));
	    ಆದರೆ (prop.tp_flags & TP_FLAG_CONT_ಮುಂದಿನ)
	    {
		ಆದರೆ (ಸುತ್ತು == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_PREV;
		    prop.tp_ಲಂಬಸಾಲು = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_ಪಠ್ಯ_ಸ್ಥಾನಾಂತರ = 0;
		    mch_memmove(new_ಸಾಲು + *len + (ಪೂರ್ಣಾಂಕ)PROP_ಎಣಿಕೆ_ಗಾತ್ರ
			    + new_prop_ಎಣಿಕೆ * ಗಾತ್ರ(textprop_T),
			    &prop, ಗಾತ್ರ(textprop_T));
		}
		++new_prop_ಎಣಿಕೆ;
	    }
	}
    }
    *ಸಾಲು = new_ಸಾಲು;
    *tofree = new_ಸಾಲು;
    *len = new_len;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ಸಾಲು_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    ಪೂರ್ಣಾಂಕ		flags)		// ML_APPEND_ flags
{
    char_u	*ಸಾಲು = ಸಾಲು_arg;
    colnr_T	len = len_arg;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_ಎಣಿಕೆ;	// number of indexes in current block
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ		from, ಗಮ್ಯ;
    ದೀರ್ಘ	space_needed;	// space needed for new line
    ಪೂರ್ಣಾಂಕ		ಪುಟ_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ		ಪುಟ_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		db_idx;		// index for lnum in data block
    bhdr_T	*hp;
    memfile_T	*mfp;
    ದತ್ತಾಂಶ_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    char_u	*tofree = ಖಾಲಿ;
# ifdef FEAT_BYTEOFF
    colnr_T	ಪಠ್ಯ_len = 0;	// text len with NUL without text properties
# endif
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		ret = FAIL;

    ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || buf->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;  // lnum out of range

    ಆದರೆ (ಅತಿಕಡಿಮೆ_marked && ಅತಿಕಡಿಮೆ_marked > lnum)
	ಅತಿಕಡಿಮೆ_marked = lnum + 1;

    ಆದರೆ (len == 0)
    {
	len = (colnr_T)STRLEN(ಸಾಲು) + 1;	// space needed for the text
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	ಪಠ್ಯ_len = len;
#ಆದರೆ_ಅಂತ್ಯ
    }
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	ಪಠ್ಯ_len = (colnr_T)STRLEN(ಸಾಲು) + 1;
    ಇಲ್ಲದಿದ್ದರೆ
	ಪಠ್ಯ_len = len;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	ಕೂಡಿಸು_ಪಠ್ಯ_props_for_append(buf, lnum, &ಸಾಲು, &len, &tofree);
#ಆದರೆ_ಅಂತ್ಯ

    space_needed = len + ಸೂಚ್ಯಂಕ_ಗಾತ್ರ;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    ಪುಟ_ಗಾತ್ರ = mfp->mf_ಪುಟ_ಗಾತ್ರ;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    ಆದರೆ ((hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_ಸೇರಿಸು)) == ಖಾಲಿ)
	ಹೋಗು theend;

    buf->b_ml.ml_flags &= ~ML_ಖಾಲಿಗಳು;

    ಆದರೆ (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    ಇಲ್ಲದಿದ್ದರೆ
	db_idx = lnum - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
		// get line count before the insertion
    ಸಾಲು_ಎಣಿಕೆ = buf->b_ml.ml_locked_ಮೇಲಿನ - buf->b_ml.ml_locked_ಕೆಳಗಿನ;

    dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    ಆದರೆ ((ದೀರ್ಘ)dp->db_free < space_needed && db_idx == ಸಾಲು_ಎಣಿಕೆ - 1
					    && lnum < buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_ಮೇಲಿನ);
	ಆದರೆ ((hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, lnum + 1, ML_ಸೇರಿಸು)) == ಖಾಲಿ)
	    ಹೋಗು theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	ಸಾಲು_ಎಣಿಕೆ = buf->b_ml.ml_locked_ಮೇಲಿನ - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
	CHECK(buf->b_ml.ml_locked_ಕೆಳಗಿನ != lnum + 1, "locked_low != lnum + 1");

	dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
    }

    ++buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಆದರೆ ((ದೀರ್ಘ)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_ಆರಂಭ -= len;
	dp->db_free -= space_needed;
	++(dp->db_ಸಾಲು_ಎಣಿಕೆ);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	ಆದರೆ (ಸಾಲು_ಎಣಿಕೆ > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    ಆದರೆ (db_idx < 0)
		ಸ್ಥಾನಾಂತರ = dp->db_txt_ಕೊನೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸ್ಥಾನಾಂತರ = ((dp->db_ಸೂಚ್ಯಂಕ[db_idx]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
	    mch_memmove((ಅಕ್ಷರ *)dp + dp->db_txt_ಆರಂಭ,
					  (ಅಕ್ಷರ *)dp + dp->db_txt_ಆರಂಭ + len,
				 (size_t)(ಸ್ಥಾನಾಂತರ - (dp->db_txt_ಆರಂಭ + len)));
	    ಗಾಗಿ (i = ಸಾಲು_ಎಣಿಕೆ - 1; i > db_idx; --i)
		dp->db_ಸೂಚ್ಯಂಕ[i + 1] = dp->db_ಸೂಚ್ಯಂಕ[i] - len;
	    dp->db_ಸೂಚ್ಯಂಕ[db_idx + 1] = ಸ್ಥಾನಾಂತರ - len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // add line at the end (which is the start of the text)
	    dp->db_ಸೂಚ್ಯಂಕ[db_idx + 1] = dp->db_txt_ಆರಂಭ;

	/*
	 * copy the text into the block
	 */
	mch_memmove((ಅಕ್ಷರ *)dp + dp->db_ಸೂಚ್ಯಂಕ[db_idx + 1], ಸಾಲು, (size_t)len);
	ಆದರೆ (flags & ML_APPEND_MARK)
	    dp->db_ಸೂಚ್ಯಂಕ[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	ಆದರೆ (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    ಇಲ್ಲದಿದ್ದರೆ	    // not enough space in data block
    {
	ದೀರ್ಘ	    ಸಾಲು_ಎಣಿಕೆ_ಎಡ, ಸಾಲು_ಎಣಿಕೆ_ಬಲ;
	ಪೂರ್ಣಾಂಕ	    ಪುಟ_ಎಣಿಕೆ_ಎಡ, ಪುಟ_ಎಣಿಕೆ_ಬಲ;
	bhdr_T	    *hp_ಎಡ;
	bhdr_T	    *hp_ಬಲ;
	bhdr_T	    *hp_new;
	ಪೂರ್ಣಾಂಕ	    ಸಾಲುಗಳು_moved;
	ಪೂರ್ಣಾಂಕ	    ದತ್ತಾಂಶ_moved = 0;	    // init to shut up gcc
	ಪೂರ್ಣಾಂಕ	    ಒಟ್ಟು_moved = 0;	    // init to shut up gcc
	ದತ್ತಾಂಶ_BL	    *dp_ಬಲ, *dp_ಎಡ;
	ಪೂರ್ಣಾಂಕ	    ರಾಶಿ_idx;
	ಪೂರ್ಣಾಂಕ	    ಒಳ_ಎಡ;
	ಪೂರ್ಣಾಂಕ	    lineadd;
	blocknr_T   bnum_ಎಡ, bnum_ಬಲ;
	linenr_T    lnum_ಎಡ, lnum_ಬಲ;
	ಪೂರ್ಣಾಂಕ	    pb_idx;
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
	ಆದರೆ (db_idx < 0)		// left block is new, right block is existing
	{
	    ಸಾಲುಗಳು_moved = 0;
	    ಒಳ_ಎಡ = TRUE;
	    // space_needed does not change
	}
	ಇಲ್ಲದಿದ್ದರೆ			// left block is existing, right block is new
	{
	    ಸಾಲುಗಳು_moved = ಸಾಲು_ಎಣಿಕೆ - db_idx - 1;
	    ಆದರೆ (ಸಾಲುಗಳು_moved == 0)
		ಒಳ_ಎಡ = FALSE;	// put new line in right block
					// space_needed does not change
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ದತ್ತಾಂಶ_moved = ((dp->db_ಸೂಚ್ಯಂಕ[db_idx]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ) -
							    dp->db_txt_ಆರಂಭ;
		ಒಟ್ಟು_moved = ದತ್ತಾಂಶ_moved + ಸಾಲುಗಳು_moved * ಸೂಚ್ಯಂಕ_ಗಾತ್ರ;
		ಆದರೆ ((ದೀರ್ಘ)dp->db_free + ಒಟ್ಟು_moved >= space_needed)
		{
		    ಒಳ_ಎಡ = TRUE;	// put new line in left block
		    space_needed = ಒಟ್ಟು_moved;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಒಳ_ಎಡ = FALSE;	    // put new line in right block
		    space_needed += ಒಟ್ಟು_moved;
		}
	    }
	}

	ಪುಟ_ಎಣಿಕೆ = ((space_needed + ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ) + ಪುಟ_ಗಾತ್ರ - 1) / ಪುಟ_ಗಾತ್ರ;
	ಆದರೆ ((hp_new = ml_new_ದತ್ತಾಂಶ(mfp, flags & ML_APPEND_NEW, ಪುಟ_ಎಣಿಕೆ))
								       == ಖಾಲಿ)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_ಮೇಲಿನ);
	    ಹೋಗು theend;
	}
	ಆದರೆ (db_idx < 0)		// left block is new
	{
	    hp_ಎಡ = hp_new;
	    hp_ಬಲ = hp;
	    ಸಾಲು_ಎಣಿಕೆ_ಎಡ = 0;
	    ಸಾಲು_ಎಣಿಕೆ_ಬಲ = ಸಾಲು_ಎಣಿಕೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ			// right block is new
	{
	    hp_ಎಡ = hp;
	    hp_ಬಲ = hp_new;
	    ಸಾಲು_ಎಣಿಕೆ_ಎಡ = ಸಾಲು_ಎಣಿಕೆ;
	    ಸಾಲು_ಎಣಿಕೆ_ಬಲ = 0;
	}
	dp_ಬಲ = (ದತ್ತಾಂಶ_BL *)(hp_ಬಲ->bh_ದತ್ತಾಂಶ);
	dp_ಎಡ = (ದತ್ತಾಂಶ_BL *)(hp_ಎಡ->bh_ದತ್ತಾಂಶ);
	bnum_ಎಡ = hp_ಎಡ->bh_bnum;
	bnum_ಬಲ = hp_ಬಲ->bh_bnum;
	ಪುಟ_ಎಣಿಕೆ_ಎಡ = hp_ಎಡ->bh_ಪುಟ_ಎಣಿಕೆ;
	ಪುಟ_ಎಣಿಕೆ_ಬಲ = hp_ಬಲ->bh_ಪುಟ_ಎಣಿಕೆ;

	/*
	 * May move the new line into the right/new block.
	 */
	ಆದರೆ (!ಒಳ_ಎಡ)
	{
	    dp_ಬಲ->db_txt_ಆರಂಭ -= len;
	    dp_ಬಲ->db_free -= len + ಸೂಚ್ಯಂಕ_ಗಾತ್ರ;
	    dp_ಬಲ->db_ಸೂಚ್ಯಂಕ[0] = dp_ಬಲ->db_txt_ಆರಂಭ;
	    ಆದರೆ (flags & ML_APPEND_MARK)
		dp_ಬಲ->db_ಸೂಚ್ಯಂಕ[0] |= DB_MARKED;

	    mch_memmove((ಅಕ್ಷರ *)dp_ಬಲ + dp_ಬಲ->db_txt_ಆರಂಭ,
							   ಸಾಲು, (size_t)len);
	    ++ಸಾಲು_ಎಣಿಕೆ_ಬಲ;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	ಆದರೆ (ಸಾಲುಗಳು_moved)
	{
	    dp_ಬಲ->db_txt_ಆರಂಭ -= ದತ್ತಾಂಶ_moved;
	    dp_ಬಲ->db_free -= ಒಟ್ಟು_moved;
	    mch_memmove((ಅಕ್ಷರ *)dp_ಬಲ + dp_ಬಲ->db_txt_ಆರಂಭ,
			(ಅಕ್ಷರ *)dp_ಎಡ + dp_ಎಡ->db_txt_ಆರಂಭ,
			(size_t)ದತ್ತಾಂಶ_moved);
	    ಸ್ಥಾನಾಂತರ = dp_ಬಲ->db_txt_ಆರಂಭ - dp_ಎಡ->db_txt_ಆರಂಭ;
	    dp_ಎಡ->db_txt_ಆರಂಭ += ದತ್ತಾಂಶ_moved;
	    dp_ಎಡ->db_free += ಒಟ್ಟು_moved;

	    /*
	     * update indexes in the new block
	     */
	    ಗಾಗಿ (ಗಮ್ಯ = ಸಾಲು_ಎಣಿಕೆ_ಬಲ, from = db_idx + 1;
					 from < ಸಾಲು_ಎಣಿಕೆ_ಎಡ; ++from, ++ಗಮ್ಯ)
		dp_ಬಲ->db_ಸೂಚ್ಯಂಕ[ಗಮ್ಯ] = dp->db_ಸೂಚ್ಯಂಕ[from] + ಸ್ಥಾನಾಂತರ;
	    ಸಾಲು_ಎಣಿಕೆ_ಬಲ += ಸಾಲುಗಳು_moved;
	    ಸಾಲು_ಎಣಿಕೆ_ಎಡ -= ಸಾಲುಗಳು_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	ಆದರೆ (ಒಳ_ಎಡ)
	{
	    dp_ಎಡ->db_txt_ಆರಂಭ -= len;
	    dp_ಎಡ->db_free -= len + ಸೂಚ್ಯಂಕ_ಗಾತ್ರ;
	    dp_ಎಡ->db_ಸೂಚ್ಯಂಕ[ಸಾಲು_ಎಣಿಕೆ_ಎಡ] = dp_ಎಡ->db_txt_ಆರಂಭ;
	    ಆದರೆ (flags & ML_APPEND_MARK)
		dp_ಎಡ->db_ಸೂಚ್ಯಂಕ[ಸಾಲು_ಎಣಿಕೆ_ಎಡ] |= DB_MARKED;
	    mch_memmove((ಅಕ್ಷರ *)dp_ಎಡ + dp_ಎಡ->db_txt_ಆರಂಭ,
							   ಸಾಲು, (size_t)len);
	    ++ಸಾಲು_ಎಣಿಕೆ_ಎಡ;
	}

	ಆದರೆ (db_idx < 0)		// left block is new
	{
	    lnum_ಎಡ = lnum + 1;
	    lnum_ಬಲ = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ			// right block is new
	{
	    lnum_ಎಡ = 0;
	    ಆದರೆ (ಒಳ_ಎಡ)
		lnum_ಬಲ = lnum + 2;
	    ಇಲ್ಲದಿದ್ದರೆ
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
	ಆದರೆ (ಸಾಲುಗಳು_moved || ಒಳ_ಎಡ)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	ಆದರೆ (!(flags & ML_APPEND_NEW) && db_idx >= 0 && ಒಳ_ಎಡ)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	ಗಾಗಿ (ರಾಶಿ_idx = buf->b_ml.ml_ರಾಶಿ_ತುದಿ - 1; ರಾಶಿ_idx >= 0;
								  --ರಾಶಿ_idx)
	{
	    ip = &(buf->b_ml.ml_ರಾಶಿ[ರಾಶಿ_idx]);
	    pb_idx = ip->ip_ಸೂಚ್ಯಂಕ;
	    ಆದರೆ ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ಖಾಲಿ)
		ಹೋಗು theend;
	    pp = (PTR_BL *)(hp->bh_ದತ್ತಾಂಶ);   // must be pointer block
	    ಆದರೆ (pp->pb_id != PTR_ID)
	    {
		iemsg(e_ಸೂಚಕ_ಖಂಡ_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		ಹೋಗು theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    ಆದರೆ (pp->pb_ಎಣಿಕೆ < pp->pb_ಎಣಿಕೆ_max)
	    {
		ಆದರೆ (pb_idx + 1 < (ಪೂರ್ಣಾಂಕ)pp->pb_ಎಣಿಕೆ)
		    mch_memmove(&pp->pb_ಸೂಚಕ[pb_idx + 2],
				&pp->pb_ಸೂಚಕ[pb_idx + 1],
			(size_t)(pp->pb_ಎಣಿಕೆ - pb_idx - 1) * ಗಾತ್ರ(PTR_EN));
		++pp->pb_ಎಣಿಕೆ;
		pp->pb_ಸೂಚಕ[pb_idx].pe_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಎಡ;
		pp->pb_ಸೂಚಕ[pb_idx].pe_bnum = bnum_ಎಡ;
		pp->pb_ಸೂಚಕ[pb_idx].pe_ಪುಟ_ಎಣಿಕೆ = ಪುಟ_ಎಣಿಕೆ_ಎಡ;
		pp->pb_ಸೂಚಕ[pb_idx + 1].pe_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಬಲ;
		pp->pb_ಸೂಚಕ[pb_idx + 1].pe_bnum = bnum_ಬಲ;
		pp->pb_ಸೂಚಕ[pb_idx + 1].pe_ಪುಟ_ಎಣಿಕೆ = ಪುಟ_ಎಣಿಕೆ_ಬಲ;

		ಆದರೆ (lnum_ಎಡ != 0)
		    pp->pb_ಸೂಚಕ[pb_idx].pe_old_lnum = lnum_ಎಡ;
		ಆದರೆ (lnum_ಬಲ != 0)
		    pp->pb_ಸೂಚಕ[pb_idx + 1].pe_old_lnum = lnum_ಬಲ;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_ರಾಶಿ_ತುದಿ = ರಾಶಿ_idx + 1;	    // truncate stack

		ಆದರೆ (lineadd)
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
		ನಿಲ್ಲಿಸು;
	    }
	    // pointer block full
	    /*
	     * split the pointer block
	     * allocate a new pointer block
	     * move some of the pointer into the new block
	     * prepare for updating the parent block
	     */
	    ಗಾಗಿ (;;)	// do this twice when splitting block 1
	    {
		hp_new = ml_new_ptr(mfp);
		ಆದರೆ (hp_new == ಖಾಲಿ)	    // TODO: try to fix tree
		    ಹೋಗು theend;
		pp_new = (PTR_BL *)(hp_new->bh_ದತ್ತಾಂಶ);

		ಆದರೆ (hp->bh_bnum != 1)
		    ನಿಲ್ಲಿಸು;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (size_t)ಪುಟ_ಗಾತ್ರ);
		pp->pb_ಎಣಿಕೆ = 1;
		pp->pb_ಸೂಚಕ[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_ಸೂಚಕ[0].pe_ಸಾಲು_ಎಣಿಕೆ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
		pp->pb_ಸೂಚಕ[0].pe_old_lnum = 1;
		pp->pb_ಸೂಚಕ[0].pe_ಪುಟ_ಎಣಿಕೆ = 1;
		mf_put(mfp, hp, TRUE, FALSE);   // release block 1
		hp = hp_new;		// new block is to be split
		pp = pp_new;
		CHECK(ರಾಶಿ_idx != 0, _("stack_idx should be 0"));
		ip->ip_ಸೂಚ್ಯಂಕ = 0;
		++ರಾಶಿ_idx;	// do block 1 again later
	    }
	    /*
	     * move the pointers after the current one to the new block
	     * If there are none, the new entry will be in the new block.
	     */
	    ಒಟ್ಟು_moved = pp->pb_ಎಣಿಕೆ - pb_idx - 1;
	    ಆದರೆ (ಒಟ್ಟು_moved)
	    {
		mch_memmove(&pp_new->pb_ಸೂಚಕ[0],
				&pp->pb_ಸೂಚಕ[pb_idx + 1],
				(size_t)(ಒಟ್ಟು_moved) * ಗಾತ್ರ(PTR_EN));
		pp_new->pb_ಎಣಿಕೆ = ಒಟ್ಟು_moved;
		pp->pb_ಎಣಿಕೆ -= ಒಟ್ಟು_moved - 1;
		pp->pb_ಸೂಚಕ[pb_idx + 1].pe_bnum = bnum_ಬಲ;
		pp->pb_ಸೂಚಕ[pb_idx + 1].pe_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಬಲ;
		pp->pb_ಸೂಚಕ[pb_idx + 1].pe_ಪುಟ_ಎಣಿಕೆ = ಪುಟ_ಎಣಿಕೆ_ಬಲ;
		ಆದರೆ (lnum_ಬಲ)
		    pp->pb_ಸೂಚಕ[pb_idx + 1].pe_old_lnum = lnum_ಬಲ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		pp_new->pb_ಎಣಿಕೆ = 1;
		pp_new->pb_ಸೂಚಕ[0].pe_bnum = bnum_ಬಲ;
		pp_new->pb_ಸೂಚಕ[0].pe_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಬಲ;
		pp_new->pb_ಸೂಚಕ[0].pe_ಪುಟ_ಎಣಿಕೆ = ಪುಟ_ಎಣಿಕೆ_ಬಲ;
		pp_new->pb_ಸೂಚಕ[0].pe_old_lnum = lnum_ಬಲ;
	    }
	    pp->pb_ಸೂಚಕ[pb_idx].pe_bnum = bnum_ಎಡ;
	    pp->pb_ಸೂಚಕ[pb_idx].pe_ಸಾಲು_ಎಣಿಕೆ = ಸಾಲು_ಎಣಿಕೆ_ಎಡ;
	    pp->pb_ಸೂಚಕ[pb_idx].pe_ಪುಟ_ಎಣಿಕೆ = ಪುಟ_ಎಣಿಕೆ_ಎಡ;
	    ಆದರೆ (lnum_ಎಡ)
		pp->pb_ಸೂಚಕ[pb_idx].pe_old_lnum = lnum_ಎಡ;
	    lnum_ಎಡ = 0;
	    lnum_ಬಲ = 0;

	    /*
	     * recompute line counts
	     */
	    ಸಾಲು_ಎಣಿಕೆ_ಬಲ = 0;
	    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)pp_new->pb_ಎಣಿಕೆ; ++i)
		ಸಾಲು_ಎಣಿಕೆ_ಬಲ += pp_new->pb_ಸೂಚಕ[i].pe_ಸಾಲು_ಎಣಿಕೆ;
	    ಸಾಲು_ಎಣಿಕೆ_ಎಡ = 0;
	    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)pp->pb_ಎಣಿಕೆ; ++i)
		ಸಾಲು_ಎಣಿಕೆ_ಎಡ += pp->pb_ಸೂಚಕ[i].pe_ಸಾಲು_ಎಣಿಕೆ;

	    bnum_ಎಡ = hp->bh_bnum;
	    bnum_ಬಲ = hp_new->bh_bnum;
	    ಪುಟ_ಎಣಿಕೆ_ಎಡ = 1;
	    ಪುಟ_ಎಣಿಕೆ_ಬಲ = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	ಆದರೆ (ರಾಶಿ_idx < 0)
	{
	    iemsg(e_updated_too_many_ಖಂಡಗಳು);
	    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;	// invalidate stack
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (ದೀರ್ಘ)ಪಠ್ಯ_len
# ಇಲ್ಲದಿದ್ದರೆ
	    (ದೀರ್ಘ)len
# endif
	    , ML_CHNK_ADDLINE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    ಆದರೆ (!(flags & ML_APPEND_NEW) && netbeans_ಸಕ್ರಿಯ())
    {
	ಪೂರ್ಣಾಂಕ ಸಾಲು_len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಸಾಲು);
	ಆದರೆ (ಸಾಲು_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, ಸಾಲು, ಸಾಲು_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)ಸಾಲು_len, (char_u *)"\n", 1);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (!(flags & ML_APPEND_NEW) && buf->b_write_ಗಮ್ಯ_channel)
	channel_write_new_ಸಾಲುಗಳು(buf);
#ಆದರೆ_ಅಂತ್ಯ
    ret = OK;

theend:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    vim_free(tofree);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ಸಾಲು,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    ಪೂರ್ಣಾಂಕ		flags)		// ML_APPEND_ flags
{
    ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ FAIL;  // lnum out of range

    ಆದರೆ (buf->b_ml.ml_ಸಾಲು_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_ಸಾಲು(buf);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	ಆದರೆ (buf->b_ml.ml_ಸಾಲು_lnum != 0)
	    ml_flush_ಸಾಲು(buf);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ml_append_int(buf, lnum, ಸಾಲು, len, flags);
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
    ಪೂರ್ಣಾಂಕ
ml_append(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ಸಾಲು,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    ಪೂರ್ಣಾಂಕ		newfile)	// flag, see above
{
    ಹಿಂತಿರುಗಿ ml_append_flags(lnum, ಸಾಲು, len, newfile ? ML_APPEND_NEW : 0);
}

    ಪೂರ್ಣಾಂಕ
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ಸಾಲು,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    ಪೂರ್ಣಾಂಕ		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    ಆದರೆ (curbuf->b_ml.ml_mfp == ಖಾಲಿ && open_ಬಫರ್(FALSE, ಖಾಲಿ, 0) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ ml_append_flush(curbuf, lnum, ಸಾಲು, len, flags);
}


#ಆದರೆ defined(FEAT_SPELL) || defined(FEAT_QUICKFIX) || defined(FEAT_PROP_POPUP)

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
    ಪೂರ್ಣಾಂಕ
ml_append_buf(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ಸಾಲು,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    ಪೂರ್ಣಾಂಕ		newfile)	// flag, see above
{
    ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ ml_append_flush(buf, lnum, ಸಾಲು, len, newfile ? ML_APPEND_NEW : 0);
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ
ml_replace(linenr_T lnum, char_u *ಸಾಲು, ಪೂರ್ಣಾಂಕ copy)
{
    colnr_T len = -1;

    ಆದರೆ (ಸಾಲು != ಖಾಲಿ)
	len = (colnr_T)STRLEN(ಸಾಲು);
    ಹಿಂತಿರುಗಿ ml_replace_len(lnum, ಸಾಲು, len, FALSE, copy);
}

/*
 * Replace a line for the current buffer.  Like ml_replace() with:
 * "len_arg" is the length of the text, excluding NUL.
 * If "has_props" is TRUE then "line_arg" includes the text properties and
 * "len_arg" includes the NUL of the text and text properties.
 * When "copy" is TRUE copy the text into allocated memory, otherwise
 * "line_arg" must be allocated and will be consumed here.
 */
    ಪೂರ್ಣಾಂಕ
ml_replace_len(
	linenr_T    lnum,
	char_u	    *ಸಾಲು_arg,
	colnr_T	    len_arg,
	ಪೂರ್ಣಾಂಕ	    has_props,
	ಪೂರ್ಣಾಂಕ	    copy)
{
    char_u *ಸಾಲು = ಸಾಲು_arg;
    colnr_T len = len_arg;

    ಆದರೆ (ಸಾಲು == ಖಾಲಿ)		// just checking...
	ಹಿಂತಿರುಗಿ FAIL;

    // When starting up, we might still need to create the memfile
    ಆದರೆ (curbuf->b_ml.ml_mfp == ಖಾಲಿ && open_ಬಫರ್(FALSE, ಖಾಲಿ, 0) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (!has_props)
	++len;  // include the NUL after the text
    ಆದರೆ (copy)
    {
	// copy the line to allocated memory
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (has_props)
	    ಸಾಲು = vim_memsave(ಸಾಲು, len);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ಸಾಲು = vim_strnsave(ಸಾಲು, len - 1);
	ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    ಆದರೆ (netbeans_ಸಕ್ರಿಯ())
    {
	netbeans_removed(curbuf, lnum, 0, (ದೀರ್ಘ)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, ಸಾಲು, (ಪೂರ್ಣಾಂಕ)STRLEN(ಸಾಲು));
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (curbuf->b_ml.ml_ಸಾಲು_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_ಸಾಲು(curbuf);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (curbuf->b_has_textprop && !has_props)
    {
	size_t	oldtextlen = STRLEN(curbuf->b_ml.ml_ಸಾಲು_ptr) + 1;

	ಆದರೆ (oldtextlen < (size_t)curbuf->b_ml.ml_ಸಾಲು_len)
	{
	    char_u *newline;
	    size_t textproplen = curbuf->b_ml.ml_ಸಾಲು_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    ಆದರೆ (newline != ಖಾಲಿ)
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
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (curbuf->b_ml.ml_flags & (ML_ಸಾಲು_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_ಸಾಲು_ptr);	// free allocated line

    curbuf->b_ml.ml_ಸಾಲು_ptr = ಸಾಲು;
    curbuf->b_ml.ml_ಸಾಲು_len = len;
    curbuf->b_ml.ml_ಸಾಲು_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_ಸಾಲು_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_ಸಾಲು_DIRTY) & ~ML_ಖಾಲಿಗಳು;

    ಹಿಂತಿರುಗಿ OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
/*
 * Adjust text properties in line "lnum" for a deleted line.
 * When "above" is true this is the line above the deleted line, otherwise this
 * is the line below the deleted line.
 * "del_props[del_props_len]" are the properties of the deleted line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
adjust_ಪಠ್ಯ_props_for_delete(
	buf_T	    *buf,
	linenr_T    lnum,
	char_u	    *del_props,
	ಪೂರ್ಣಾಂಕ	    del_props_len,
	ಪೂರ್ಣಾಂಕ	    above)
{
    ಪೂರ್ಣಾಂಕ		did_get_ಸಾಲು = FALSE;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ_del;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    ದತ್ತಾಂಶ_BL	*dp;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_ಆರಂಭ;
    ದೀರ್ಘ	ಸಾಲು_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ		this_props_len = 0;
    char_u	*ಪಠ್ಯ;
    size_t	textlen;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು;

    ಗಾಗಿ (ಮುಗಿದ_del = 0; ಮುಗಿದ_del < del_props_len; ಮುಗಿದ_del += ಗಾತ್ರ(textprop_T))
    {
	mch_memmove(&prop_del, del_props + ಮುಗಿದ_del, ಗಾತ್ರ(textprop_T));
	ಆದರೆ ((above && (prop_del.tp_flags & TP_FLAG_CONT_PREV)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_ಮುಂದಿನ))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_ಮುಂದಿನ)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_PREV)))
	{
	    ಆದರೆ (!did_get_ಸಾಲು)
	    {
		did_get_ಸಾಲು = TRUE;
		ಆದರೆ ((hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, lnum, ML_ಕಂಡುಹಿಡಿ)) == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ;

		dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
		idx = lnum - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
		ಸಾಲು_ಆರಂಭ = ((dp->db_ಸೂಚ್ಯಂಕ[idx]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
		ಆದರೆ (idx == 0)		// first line in block, text at the end
		    ಸಾಲು_ಗಾತ್ರ = dp->db_txt_ಕೊನೆ - ಸಾಲು_ಆರಂಭ;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಾಲು_ಗಾತ್ರ = ((dp->db_ಸೂಚ್ಯಂಕ[idx - 1]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ)
								  - ಸಾಲು_ಆರಂಭ;
		ಪಠ್ಯ = (char_u *)dp + ಸಾಲು_ಆರಂಭ;
		textlen = STRLEN(ಪಠ್ಯ) + 1;
		ಆದರೆ ((ದೀರ್ಘ)textlen >= ಸಾಲು_ಗಾತ್ರ)
		{
		    // No properties on this line.
		    ಆದರೆ (above)
			ಆಂತರಿಕ_error("no text property above deleted line");
		    ಇಲ್ಲದಿದ್ದರೆ
			ಆಂತರಿಕ_error("no text property below deleted line");
		    ಹಿಂತಿರುಗಿ;
		}
		ಆದರೆ ((ದೀರ್ಘ)textlen + (ದೀರ್ಘ)PROP_ಎಣಿಕೆ_ಗಾತ್ರ > ಸಾಲು_ಗಾತ್ರ)
		{
		    ಆಂತರಿಕ_error("text property data too short");
		    ಹಿಂತಿರುಗಿ;
		}

		uint16_t pc;

		mch_memmove(&pc, ಪಠ್ಯ + textlen, PROP_ಎಣಿಕೆ_ಗಾತ್ರ);
		ಆದರೆ (!ಪಠ್ಯ_prop_ಎಣಿಕೆ_valid(pc, (size_t)(ಸಾಲು_ಗಾತ್ರ - (ದೀರ್ಘ)textlen)))
		{
		    ಆಂತರಿಕ_error("text property count too large");
		    ಹಿಂತಿರುಗಿ;
		}
		this_props_len = pc * (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(textprop_T);
	    }

	    ಸಿಕ್ಕಿತು = FALSE;
	    {
		char_u *props_ಆರಂಭ = ಪಠ್ಯ + textlen + PROP_ಎಣಿಕೆ_ಗಾತ್ರ;

		ಗಾಗಿ (ಮುಗಿದ_this = 0; ಮುಗಿದ_this < this_props_len;
					       ಮುಗಿದ_this += ಗಾತ್ರ(textprop_T))
		{
		    ಪೂರ್ಣಾಂಕ		flag = above ? TP_FLAG_CONT_ಮುಂದಿನ
							   : TP_FLAG_CONT_PREV;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_ಆರಂಭ + ಮುಗಿದ_this,
							   ಗಾತ್ರ(textprop_T));
		    ಆದರೆ ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			ಸಿಕ್ಕಿತು = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_ಆರಂಭ + ಮುಗಿದ_this, &prop_this,
							   ಗಾತ್ರ(textprop_T));
			ನಿಲ್ಲಿಸು;
		    }
		}
	    }
	    ಆದರೆ (!ಸಿಕ್ಕಿತು)
	    {
		ಆದರೆ (above)
		    ಆಂತರಿಕ_error("text property above deleted line not found");
		ಇಲ್ಲದಿದ್ದರೆ
		    ಆಂತರಿಕ_error("text property below deleted line not found");
	    }

	    buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Delete line "lnum" in the current buffer.
 * When "flags" has ML_DEL_MESSAGE may give a "No lines in buffer" message.
 * When "flags" has ML_DEL_UNDO this is called from undo.
 *
 * return FAIL for failure, OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ml_delete_int(buf_T *buf, linenr_T lnum, ಪೂರ್ಣಾಂಕ flags)
{
    bhdr_T	*hp;
    memfile_T	*mfp;
    ದತ್ತಾಂಶ_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;	    // number of entries in block
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		ರಾಶಿ_idx;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_ಆರಂಭ;
    ದೀರ್ಘ	ಸಾಲು_ಗಾತ್ರ;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    char_u	*textprop_save = ಖಾಲಿ;
    ದೀರ್ಘ	textprop_len = 0;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಅತಿಕಡಿಮೆ_marked && ಅತಿಕಡಿಮೆ_marked > lnum)
	ಅತಿಕಡಿಮೆ_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    ಆದರೆ (buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 1)	    // file becomes empty
    {
	ಆದರೆ ((flags & ML_DEL_ಸಂದೇಶ)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_ಸಾಲುಗಳು
#ಆದರೆ_ಅಂತ್ಯ
	   )
	    ಗಣ_keep_ಸಂದೇಶ((char_u *)_(no_ಸಾಲುಗಳು_ಸಂದೇಶ), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_ಖಾಲಿಗಳು;

	ಹಿಂತಿರುಗಿ i;
    }

/*
 * Find the data block containing the line.
 * This also fills the stack with the blocks from the root to the data block.
 * This also releases any locked block..
 */
    mfp = buf->b_ml.ml_mfp;
    ಆದರೆ (mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ ((hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, lnum, ML_DELETE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
    // compute line count before the delete
    ಎಣಿಕೆ_2 = (ದೀರ್ಘ)(buf->b_ml.ml_locked_ಮೇಲಿನ)
					- (ದೀರ್ಘ)(buf->b_ml.ml_locked_ಕೆಳಗಿನ) + 2;
    idx = lnum - buf->b_ml.ml_locked_ಕೆಳಗಿನ;

    --buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಸಾಲು_ಆರಂಭ = ((dp->db_ಸೂಚ್ಯಂಕ[idx]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
    ಆದರೆ (idx == 0)		// first line in block, text at the end
	ಸಾಲು_ಗಾತ್ರ = dp->db_txt_ಕೊನೆ - ಸಾಲು_ಆರಂಭ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಾಲು_ಗಾತ್ರ = ((dp->db_ಸೂಚ್ಯಂಕ[idx - 1]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ) - ಸಾಲು_ಆರಂಭ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    ಆದರೆ (netbeans_ಸಕ್ರಿಯ())
	netbeans_removed(buf, lnum, 0, ಸಾಲು_ಗಾತ್ರ);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    ಆದರೆ (buf->b_has_textprop)
    {
	size_t	textlen = STRLEN((char_u *)dp + ಸಾಲು_ಆರಂಭ) + 1;

	textprop_len = ಸಾಲು_ಗಾತ್ರ - (ದೀರ್ಘ)textlen;
	ಆದರೆ (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + ಸಾಲು_ಆರಂಭ + textlen,
								 textprop_len);
    }
#ಆದರೆ_ಅಂತ್ಯ

/*
 * special case: If there is only one line in the data block it becomes empty.
 * Then we have to remove the entry, pointing to this data block, from the
 * pointer block. If this pointer block also becomes empty, we go up another
 * block, and so on, up to the root if necessary.
 * The line counts in the pointer blocks have already been adjusted by
 * ml_find_line().
 */
    ಆದರೆ (ಎಣಿಕೆ_2 == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = ಖಾಲಿ;

	ಗಾಗಿ (ರಾಶಿ_idx = buf->b_ml.ml_ರಾಶಿ_ತುದಿ - 1; ರಾಶಿ_idx >= 0;
								  --ರಾಶಿ_idx)
	{
	    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_ರಾಶಿ[ರಾಶಿ_idx]);
	    idx = ip->ip_ಸೂಚ್ಯಂಕ;
	    ಆದರೆ ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ಖಾಲಿ)
		ಹೋಗು theend;
	    pp = (PTR_BL *)(hp->bh_ದತ್ತಾಂಶ);   // must be pointer block
	    ಆದರೆ (pp->pb_id != PTR_ID)
	    {
		iemsg(e_ಸೂಚಕ_ಖಂಡ_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		ಹೋಗು theend;
	    }
	    ಎಣಿಕೆ_2 = --(pp->pb_ಎಣಿಕೆ);
	    ಆದರೆ (ಎಣಿಕೆ_2 == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (ಎಣಿಕೆ_2 != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_ಸೂಚಕ[idx], &pp->pb_ಸೂಚಕ[idx + 1],
				      (size_t)(ಎಣಿಕೆ_2 - idx) * ಗಾತ್ರ(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_ರಾಶಿ_ತುದಿ = ರಾಶಿ_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		ಆದರೆ (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_ರಾಶಿ[buf->b_ml.ml_ರಾಶಿ_ತುದಿ].ip_ಮೇಲಿನ +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_ರಾಶಿ_ತುದಿ);

		ನಿಲ್ಲಿಸು;
	    }
	}
	CHECK(ರಾಶಿ_idx < 0, _("deleted block 1?"));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	ಪಠ್ಯ_ಆರಂಭ = dp->db_txt_ಆರಂಭ;
	mch_memmove((ಅಕ್ಷರ *)dp + ಪಠ್ಯ_ಆರಂಭ + ಸಾಲು_ಗಾತ್ರ,
		  (ಅಕ್ಷರ *)dp + ಪಠ್ಯ_ಆರಂಭ, (size_t)(ಸಾಲು_ಆರಂಭ - ಪಠ್ಯ_ಆರಂಭ));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	ಗಾಗಿ (i = idx; i < ಎಣಿಕೆ_2 - 1; ++i)
	    dp->db_ಸೂಚ್ಯಂಕ[i] = dp->db_ಸೂಚ್ಯಂಕ[i + 1] + ಸಾಲು_ಗಾತ್ರ;

	dp->db_free += ಸಾಲು_ಗಾತ್ರ + ಸೂಚ್ಯಂಕ_ಗಾತ್ರ;
	dp->db_txt_ಆರಂಭ += ಸಾಲು_ಗಾತ್ರ;
	--(dp->db_ಸಾಲು_ಎಣಿಕೆ);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, ಸಾಲು_ಗಾತ್ರ
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#ಆದರೆ_ಅಂತ್ಯ
    ret = OK;

theend:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (textprop_save != ಖಾಲಿ)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ	    props_ಬೈಟುಗಳು;

	mch_memmove(&pc, textprop_save, PROP_ಎಣಿಕೆ_ಗಾತ್ರ);
	props_ದತ್ತಾಂಶ = textprop_save + PROP_ಎಣಿಕೆ_ಗಾತ್ರ;
	props_ಬೈಟುಗಳು = pc * (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(textprop_T);
	ಆದರೆ (!ಪಠ್ಯ_prop_ಎಣಿಕೆ_valid(pc, (size_t)textprop_len))
	    props_ಬೈಟುಗಳು = 0;

	// Adjust text properties in the line above and below.
	ಆದರೆ (lnum > 1)
	    adjust_ಪಠ್ಯ_props_for_delete(buf, lnum - 1,
					     props_ದತ್ತಾಂಶ, props_ಬೈಟುಗಳು, TRUE);
	ಆದರೆ (lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    adjust_ಪಠ್ಯ_props_for_delete(buf, lnum,
					    props_ದತ್ತಾಂಶ, props_ಬೈಟುಗಳು, FALSE);
    }
    vim_free(textprop_save);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ret;
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
    ಪೂರ್ಣಾಂಕ
ml_delete(linenr_T lnum)
{
    ಹಿಂತಿರುಗಿ ml_delete_flags(lnum, 0);
}

/*
 * Like ml_delete() but using flags (see ml_delete_int()).
 */
    ಪೂರ್ಣಾಂಕ
ml_delete_flags(linenr_T lnum, ಪೂರ್ಣಾಂಕ flags)
{
    ml_flush_ಸಾಲು(curbuf);
    ಆದರೆ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಹಿಂತಿರುಗಿ FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ml_delete_int(curbuf, lnum, flags);
}

/*
 * set the DB_MARKED flag for line 'lnum'
 */
    ಶೂನ್ಯ
ml_setmarked(linenr_T lnum)
{
    bhdr_T    *hp;
    ದತ್ತಾಂಶ_BL *dp;
				    // invalid line number
    ಆದರೆ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
					       || curbuf->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;			    // give error message?

    ಆದರೆ (ಅತಿಕಡಿಮೆ_marked == 0 || ಅತಿಕಡಿಮೆ_marked > lnum)
	ಅತಿಕಡಿಮೆ_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    ಆದರೆ ((hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(curbuf, lnum, ML_ಕಂಡುಹಿಡಿ)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;		    // give error message?

    dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
    dp->db_ಸೂಚ್ಯಂಕ[lnum - curbuf->b_ml.ml_locked_ಕೆಳಗಿನ] |= DB_MARKED;
    curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
}

/*
 * find the first line with its DB_MARKED flag set
 */
    linenr_T
ml_firstmarked(ಶೂನ್ಯ)
{
    bhdr_T	*hp;
    ದತ್ತಾಂಶ_BL	*dp;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (curbuf->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    ಗಾಗಿ (lnum = ಅತಿಕಡಿಮೆ_marked; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	ಆದರೆ ((hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(curbuf, lnum, ML_ಕಂಡುಹಿಡಿ)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ (linenr_T)0;		    // give error message?

	dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);

	ಗಾಗಿ (i = lnum - curbuf->b_ml.ml_locked_ಕೆಳಗಿನ;
			    lnum <= curbuf->b_ml.ml_locked_ಮೇಲಿನ; ++i, ++lnum)
	    ಆದರೆ ((dp->db_ಸೂಚ್ಯಂಕ[i]) & DB_MARKED)
	    {
		(dp->db_ಸೂಚ್ಯಂಕ[i]) &= DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		ಅತಿಕಡಿಮೆ_marked = lnum + 1;
		ಹಿಂತಿರುಗಿ lnum;
	    }
    }

    ಹಿಂತಿರುಗಿ (linenr_T) 0;
}

/*
 * clear all DB_MARKED flags
 */
    ಶೂನ್ಯ
ml_clearmarked(ಶೂನ್ಯ)
{
    bhdr_T	*hp;
    ದತ್ತಾಂಶ_BL	*dp;
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (curbuf->b_ml.ml_mfp == ಖಾಲಿ)	    // nothing to do
	ಹಿಂತಿರುಗಿ;

    /*
     * The search starts with line lowest_marked.
     */
    ಗಾಗಿ (lnum = ಅತಿಕಡಿಮೆ_marked; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	ಆದರೆ ((hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(curbuf, lnum, ML_ಕಂಡುಹಿಡಿ)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;		// give error message?

	dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);

	ಗಾಗಿ (i = lnum - curbuf->b_ml.ml_locked_ಕೆಳಗಿನ;
			    lnum <= curbuf->b_ml.ml_locked_ಮೇಲಿನ; ++i, ++lnum)
	    ಆದರೆ ((dp->db_ಸೂಚ್ಯಂಕ[i]) & DB_MARKED)
	    {
		(dp->db_ಸೂಚ್ಯಂಕ[i]) &= DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	    }
    }

    ಅತಿಕಡಿಮೆ_marked = 0;
}

/*
 * flush ml_line if necessary
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ml_flush_ಸಾಲು(buf_T *buf)
{
    bhdr_T	*hp;
    ದತ್ತಾಂಶ_BL	*dp;
    linenr_T	lnum;
    char_u	*new_ಸಾಲು;
    char_u	*old_ಸಾಲು;
    colnr_T	new_len;
    ಪೂರ್ಣಾಂಕ		old_len;
    ಪೂರ್ಣಾಂಕ		extra;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		i;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  entered = FALSE;

    ಆದರೆ (buf->b_ml.ml_ಸಾಲು_lnum == 0 || buf->b_ml.ml_mfp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;		// nothing to do

    ಆದರೆ (buf->b_ml.ml_flags & ML_ಸಾಲು_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	ಆದರೆ (entered)
	    ಹಿಂತಿರುಗಿ;
	entered = TRUE;

	lnum = buf->b_ml.ml_ಸಾಲು_lnum;
	new_ಸಾಲು = buf->b_ml.ml_ಸಾಲು_ptr;

	hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, lnum, ML_ಕಂಡುಹಿಡಿ);
	ಆದರೆ (hp == ಖಾಲಿ)
	    siemsg(e_cannot_ಕಂಡುಹಿಡಿ_ಸಾಲು_nr, lnum);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
	    idx = lnum - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
	    ಆರಂಭ = ((dp->db_ಸೂಚ್ಯಂಕ[idx]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
	    old_ಸಾಲು = (char_u *)dp + ಆರಂಭ;
	    ಆದರೆ (idx == 0)	// line is last in block
		old_len = dp->db_txt_ಕೊನೆ - ಆರಂಭ;
	    ಇಲ್ಲದಿದ್ದರೆ		// text of previous line follows
		old_len = (dp->db_ಸೂಚ್ಯಂಕ[idx - 1] & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ) - ಆರಂಭ;
	    new_len = buf->b_ml.ml_ಸಾಲು_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)dp->db_free >= extra)
	    {
#ಆದರೆ defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		ಪೂರ್ಣಾಂಕ old_prop_len = 0;
		ಆದರೆ (buf->b_has_textprop)
		    old_prop_len = old_len - (ಪೂರ್ಣಾಂಕ)STRLEN(old_ಸಾಲು) - 1;
#ಆದರೆ_ಅಂತ್ಯ
		// if the length changes and there are following lines
		ಎಣಿಕೆ_2 = buf->b_ml.ml_locked_ಮೇಲಿನ - buf->b_ml.ml_locked_ಕೆಳಗಿನ + 1;
		ಆದರೆ (extra != 0 && idx < ಎಣಿಕೆ_2 - 1)
		{
		    // move text of following lines
		    mch_memmove((ಅಕ್ಷರ *)dp + dp->db_txt_ಆರಂಭ - extra,
				(ಅಕ್ಷರ *)dp + dp->db_txt_ಆರಂಭ,
				(size_t)(ಆರಂಭ - dp->db_txt_ಆರಂಭ));

		    // adjust pointers of this and following lines
		    ಗಾಗಿ (i = idx + 1; i < ಎಣಿಕೆ_2; ++i)
			dp->db_ಸೂಚ್ಯಂಕ[i] -= extra;
		}
		dp->db_ಸೂಚ್ಯಂಕ[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_ಆರಂಭ -= extra;

		// copy new line into the data block
		mch_memmove(old_ಸಾಲು - extra, new_ಸಾಲು, (size_t)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ಆದರೆ defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		ಆದರೆ (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (ಪೂರ್ಣಾಂಕ)STRLEN(new_ಸಾಲು) - 1;
		}
		ಆದರೆ (extra != 0)
		    ml_updatechunk(buf, lnum, (ದೀರ್ಘ)extra, ML_CHNK_UPDLINE);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
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
			 ((dp->db_ಸೂಚ್ಯಂಕ[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#ಆದರೆ_ಅಂತ್ಯ
			 );
		(ಶೂನ್ಯ)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_ಸಾಲು);

	entered = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_ಸಾಲು_ptr);

    buf->b_ml.ml_flags &= ~(ML_ಸಾಲು_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_ಸಾಲು_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    ಸ್ಥಿರ bhdr_T *
ml_new_ದತ್ತಾಂಶ(memfile_T *mfp, ಪೂರ್ಣಾಂಕ negative, ಪೂರ್ಣಾಂಕ ಪುಟ_ಎಣಿಕೆ)
{
    bhdr_T	*hp;
    ದತ್ತಾಂಶ_BL	*dp;

    ಆದರೆ ((hp = mf_new(mfp, negative, ಪುಟ_ಎಣಿಕೆ)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
    dp->db_id = ದತ್ತಾಂಶ_ID;
    dp->db_txt_ಆರಂಭ = dp->db_txt_ಕೊನೆ = ಪುಟ_ಎಣಿಕೆ * mfp->mf_ಪುಟ_ಗಾತ್ರ;
    dp->db_free = dp->db_txt_ಆರಂಭ - ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    dp->db_ಸಾಲು_ಎಣಿಕೆ = 0;

    ಹಿಂತಿರುಗಿ hp;
}

/*
 * create a new, empty, pointer block
 */
    ಸ್ಥಿರ bhdr_T *
ml_new_ptr(memfile_T *mfp)
{
    bhdr_T	*hp;
    PTR_BL	*pp;

    ಆದರೆ ((hp = mf_new(mfp, FALSE, 1)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    pp = (PTR_BL *)(hp->bh_ದತ್ತಾಂಶ);
    pp->pb_id = PTR_ID;
    pp->pb_ಎಣಿಕೆ = 0;
    pp->pb_ಎಣಿಕೆ_max = PB_ಎಣಿಕೆ_MAX(mfp);

    ಹಿಂತಿರುಗಿ hp;
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
ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf_T *buf, linenr_T lnum, ಪೂರ್ಣಾಂಕ action)
{
    ದತ್ತಾಂಶ_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    ಪೂರ್ಣಾಂಕ		dirty;
    linenr_T	ಕೆಳಗಿನ, ಮೇಲಿನ;
    ಪೂರ್ಣಾಂಕ		ತುದಿ;
    ಪೂರ್ಣಾಂಕ		ಪುಟ_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		idx;

    mfp = buf->b_ml.ml_mfp;

    /*
     * If there is a locked block check if the wanted line is in it.
     * If not, flush and release the locked block.
     * Don't do this for ML_INSERT_SAME, because the stack need to be updated.
     * Don't do this for ML_FLUSH, because we want to flush the locked block.
     * Don't do this when 'swapfile' is reset, we want to load all the blocks.
     */
    ಆದರೆ (buf->b_ml.ml_locked)
    {
	ಆದರೆ (ML_SIMPLE(action)
		&& buf->b_ml.ml_locked_ಕೆಳಗಿನ <= lnum
		&& buf->b_ml.ml_locked_ಮೇಲಿನ >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    ಆದರೆ (action == ML_ಸೇರಿಸು)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_ಮೇಲಿನ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_ಮೇಲಿನ);
	    }
	    ಹಿಂತಿರುಗಿ (buf->b_ml.ml_locked);
	}

	mf_put(mfp, buf->b_ml.ml_locked, buf->b_ml.ml_flags & ML_LOCKED_DIRTY,
					    buf->b_ml.ml_flags & ML_LOCKED_POS);
	buf->b_ml.ml_locked = ಖಾಲಿ;

	/*
	 * If lines have been added or deleted in the locked block, need to
	 * update the line count in pointer blocks.
	 */
	ಆದರೆ (buf->b_ml.ml_locked_lineadd != 0)
	    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
    }

    ಆದರೆ (action == ML_FLUSH)	    // nothing else to do
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    bnum = 1;			    // start at the root of the tree
    ಪುಟ_ಎಣಿಕೆ = 1;
    ಕೆಳಗಿನ = 1;
    ಮೇಲಿನ = buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಆದರೆ (action == ML_ಕಂಡುಹಿಡಿ)	// first try stack entries
    {
	ಗಾಗಿ (ತುದಿ = buf->b_ml.ml_ರಾಶಿ_ತುದಿ - 1; ತುದಿ >= 0; --ತುದಿ)
	{
	    ip = &(buf->b_ml.ml_ರಾಶಿ[ತುದಿ]);
	    ಆದರೆ (ip->ip_ಕೆಳಗಿನ <= lnum && ip->ip_ಮೇಲಿನ >= lnum)
	    {
		bnum = ip->ip_bnum;
		ಕೆಳಗಿನ = ip->ip_ಕೆಳಗಿನ;
		ಮೇಲಿನ = ip->ip_ಮೇಲಿನ;
		buf->b_ml.ml_ರಾಶಿ_ತುದಿ = ತುದಿ;	// truncate stack at prev entry
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (ತುದಿ < 0)
	    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;		// not found, start at the root
    }
    ಇಲ್ಲದಿದ್ದರೆ	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    ಗಾಗಿ (;;)
    {
	ಆದರೆ ((hp = mf_get(mfp, bnum, ಪುಟ_ಎಣಿಕೆ)) == ಖಾಲಿ)
	    ಹೋಗು error_noblock;

	/*
	 * update high for insert/delete
	 */
	ಆದರೆ (action == ML_ಸೇರಿಸು)
	    ++ಮೇಲಿನ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action == ML_DELETE)
	    --ಮೇಲಿನ;

	dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
	ಆದರೆ (dp->db_id == ದತ್ತಾಂಶ_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_ಕೆಳಗಿನ = ಕೆಳಗಿನ;
	    buf->b_ml.ml_locked_ಮೇಲಿನ = ಮೇಲಿನ;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    ಹಿಂತಿರುಗಿ hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	ಆದರೆ (pp->pb_id != PTR_ID)
	{
	    iemsg(e_ಸೂಚಕ_ಖಂಡ_id_wrong);
	    ಹೋಗು error_ಖಂಡ;
	}

	ಆದರೆ ((ತುದಿ = ml_ಕೂಡಿಸು_ರಾಶಿ(buf)) < 0)	// add new entry to stack
	    ಹೋಗು error_ಖಂಡ;
	ip = &(buf->b_ml.ml_ರಾಶಿ[ತುದಿ]);
	ip->ip_bnum = bnum;
	ip->ip_ಕೆಳಗಿನ = ಕೆಳಗಿನ;
	ip->ip_ಮೇಲಿನ = ಮೇಲಿನ;
	ip->ip_ಸೂಚ್ಯಂಕ = -1;		// index not known yet

	dirty = FALSE;
	ಗಾಗಿ (idx = 0; idx < (ಪೂರ್ಣಾಂಕ)pp->pb_ಎಣಿಕೆ; ++idx)
	{
	    t = pp->pb_ಸೂಚಕ[idx].pe_ಸಾಲು_ಎಣಿಕೆ;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    ಆದರೆ ((ಕೆಳಗಿನ += t) > lnum)
	    {
		ip->ip_ಸೂಚ್ಯಂಕ = idx;
		bnum = pp->pb_ಸೂಚಕ[idx].pe_bnum;
		ಪುಟ_ಎಣಿಕೆ = pp->pb_ಸೂಚಕ[idx].pe_ಪುಟ_ಎಣಿಕೆ;
		ಮೇಲಿನ = ಕೆಳಗಿನ - 1;
		ಕೆಳಗಿನ -= t;

		/*
		 * a negative block number may have been changed
		 */
		ಆದರೆ (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    ಆದರೆ (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_ಸೂಚಕ[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (idx >= (ಪೂರ್ಣಾಂಕ)pp->pb_ಎಣಿಕೆ)	    // past the end: something wrong!
	{
	    ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		siemsg(e_ಸಾಲು_ಸಂಖ್ಯೆ_ಹೊರಹೋಗು_ದ_range_nr_past_the_ಕೊನೆ,
					      lnum - buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);

	    ಇಲ್ಲದಿದ್ದರೆ
		siemsg(e_ಸಾಲು_ಎಣಿಕೆ_wrong_ಒಳ_ಖಂಡ_nr, bnum);
	    ಹೋಗು error_ಖಂಡ;
	}
	ಆದರೆ (action == ML_DELETE)
	{
	    pp->pb_ಸೂಚಕ[idx].pe_ಸಾಲು_ಎಣಿಕೆ--;
	    dirty = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action == ML_ಸೇರಿಸು)
	{
	    pp->pb_ಸೂಚಕ[idx].pe_ಸಾಲು_ಎಣಿಕೆ++;
	    dirty = TRUE;
	}
	mf_put(mfp, hp, dirty, FALSE);
    }

error_ಖಂಡ:
    mf_put(mfp, hp, FALSE, FALSE);
error_noblock:
    /*
     * If action is ML_DELETE or ML_INSERT we have to correct the tree for
     * the incremented/decremented line counts, because there won't be a line
     * inserted/deleted after all.
     */
    ಆದರೆ (action == ML_DELETE)
	ml_lineadd(buf, 1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (action == ML_ಸೇರಿಸು)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_ರಾಶಿ_ತುದಿ = 0;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ml_ಕೂಡಿಸು_ರಾಶಿ(buf_T *buf)
{
    ಪೂರ್ಣಾಂಕ		ತುದಿ;
    infoptr_T	*newstack;

    ತುದಿ = buf->b_ml.ml_ರಾಶಿ_ತುದಿ;

    // may have to increase the stack size
    ಆದರೆ (ತುದಿ == buf->b_ml.ml_ರಾಶಿ_ಗಾತ್ರ)
    {
	CHECK(ತುದಿ > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_ರಾಶಿ_ಗಾತ್ರ + ರಾಶಿ_INCR);
	ಆದರೆ (newstack == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (ತುದಿ > 0)
	    mch_memmove(newstack, buf->b_ml.ml_ರಾಶಿ,
					     (size_t)ತುದಿ * ಗಾತ್ರ(infoptr_T));
	vim_free(buf->b_ml.ml_ರಾಶಿ);
	buf->b_ml.ml_ರಾಶಿ = newstack;
	buf->b_ml.ml_ರಾಶಿ_ಗಾತ್ರ += ರಾಶಿ_INCR;
    }

    buf->b_ml.ml_ರಾಶಿ_ತುದಿ++;
    ಹಿಂತಿರುಗಿ ತುದಿ;
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
ml_lineadd(buf_T *buf, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಪೂರ್ಣಾಂಕ		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    ಗಾಗಿ (idx = buf->b_ml.ml_ರಾಶಿ_ತುದಿ - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_ರಾಶಿ[idx]);
	ಆದರೆ ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	pp = (PTR_BL *)(hp->bh_ದತ್ತಾಂಶ);	// must be pointer block
	ಆದರೆ (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_ಸೂಚಕ_ಖಂಡ_id_wrong_two);
	    ನಿಲ್ಲಿಸು;
	}
	pp->pb_ಸೂಚಕ[ip->ip_ಸೂಚ್ಯಂಕ].pe_ಸಾಲು_ಎಣಿಕೆ += ಎಣಿಕೆ_2;
	ip->ip_ಮೇಲಿನ += ಎಣಿಕೆ_2;
	mf_put(mfp, hp, TRUE, FALSE);
    }
}

#ಆದರೆ defined(HAVE_READLINK)
/*
 * Resolve a symlink in the last component of a file name.
 * Note that f_resolve() does it for every part of the path, we don't do that
 * here.
 * If it worked returns OK and the resolved link in "buf[MAXPATHL]".
 * Otherwise returns FAIL.
 */
    ಪೂರ್ಣಾಂಕ
resolve_symlink(char_u *fname, char_u *buf)
{
    char_u	tmp[MAXPATHL];
    ಪೂರ್ಣಾಂಕ		ret;
    ಪೂರ್ಣಾಂಕ		ಆಳ = 0;

    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    ಗಾಗಿ (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	ಆದರೆ (++ಆಳ == 100)
	{
	    semsg(_(e_symlink_loop_for_str), fname);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ret = readlink((ಅಕ್ಷರ *)tmp, (ಅಕ್ಷರ *)buf, MAXPATHL - 1);
	ಆದರೆ (ret <= 0)
	{
	    ಆದರೆ (errno == EINVAL || errno == ENOENT)
	    {
		// Found non-symlink or not existing file, stop here.
		// When at the first level use the unmodified name, skip the
		// call to vim_FullName().
		ಆದರೆ (ಆಳ == 1)
		    ಹಿಂತಿರುಗಿ FAIL;

		// Use the resolved name in tmp[].
		ನಿಲ್ಲಿಸು;
	    }

	    // There must be some error reading links, use original name.
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	buf[ret] = NUL;

	/*
	 * Check whether the symlink is relative or absolute.
	 * If it's relative, build a new path based on the directory
	 * portion of the filename (if any) and the path the symlink
	 * points to.
	 */
	ಆದರೆ (mch_is_ತುಂಬಿದ_Name(buf))
	    STRCPY(tmp, buf);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u *ಬಾಲ;

	    ಬಾಲ = gettail(tmp);
	    ಆದರೆ (STRLEN(ಬಾಲ) + STRLEN(buf) >= MAXPATHL)
		ಹಿಂತಿರುಗಿ FAIL;
	    STRCPY(ಬಾಲ, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    ಹಿಂತಿರುಗಿ vim_ತುಂಬಿದ_Name(tmp, buf, MAXPATHL, TRUE);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Make swap file name out of the file name and a directory name.
 * Returns pointer to allocated memory or NULL.
 */
    char_u *
makeswapname(
    char_u	*fname,
    char_u	*ffname ಬಳಸದ,
    buf_T	*buf,
    char_u	*dir_name)
{
    char_u	*r, *s;
    char_u	*fname_res = fname;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_READLINK
    char_u	fname_buf[MAXPATHL];

    // Expand symlink in the file name, so that we put the swap file with the
    // actual file instead of with the symlink.
    ಆದರೆ (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(UNIX) || defined(MSWIN)  // Need _very_ long file names
    ಪೂರ್ಣಾಂಕ		len = (ಪೂರ್ಣಾಂಕ)STRLEN(dir_name);

    s = dir_name + len;
    ಆದರೆ (after_pathsep(dir_name, s) && len > 1 && s[-1] == s[-2])
    {			       // Ends with '//', Use Full path
	r = ಖಾಲಿ;
	ಆದರೆ ((s = make_percent_swname(dir_name, s, fname_res)) != ಖಾಲಿ)
	{
	    r = modname(s, (char_u *)".swp", FALSE);
	    vim_free(s);
	}
	ಹಿಂತಿರುಗಿ r;
    }
#ಆದರೆ_ಅಂತ್ಯ

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#ಆದರೆ defined(VMS)
	    "_swp",
#ಇಲ್ಲದಿದ್ದರೆ
	    ".swp",
#ಆದರೆ_ಅಂತ್ಯ
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    ಆದರೆ (r == ಖಾಲಿ)	    // out of memory
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    s = get_file_ಒಳ_dir(r, dir_name);
    vim_free(r);
    ಹಿಂತಿರುಗಿ s;
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
get_file_ಒಳ_dir(
    char_u  *fname,
    char_u  *dname)	// don't use "dirname", it is a global for Alpha
{
    string_T	ಬಾಲ;
    string_T	retval;

    ಬಾಲ.string = gettail(fname);
    ಬಾಲ.ಉದ್ದಳತೆ = STRLEN(ಬಾಲ.string);

    ಆದರೆ (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (size_t)(ಬಾಲ.string - fname) + ಬಾಲ.ಉದ್ದಳತೆ);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	size_t	dname_len = STRLEN(dname);

	ಆದರೆ (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    ಆದರೆ (ಬಾಲ.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    ಬಾಲ.string, ಬಾಲ.ಉದ್ದಳತೆ, TRUE, &retval);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ	    save_char;
		string_T    tmp;

		save_char = *ಬಾಲ.string;
		*ಬಾಲ.string = NUL;
		concat_fnames(fname, (size_t)(ಬಾಲ.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*ಬಾಲ.string = save_char;
		ಆದರೆ (tmp.string == ಖಾಲಿ)	    // out of memory
		    retval.string = ಖಾಲಿ;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    concat_fnames(tmp.string, tmp.ಉದ್ದಳತೆ,
			ಬಾಲ.string, ಬಾಲ.ಉದ್ದಳತೆ, TRUE, &retval);
		    vim_free(tmp.string);
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    concat_fnames(dname, dname_len, ಬಾಲ.string, ಬಾಲ.ಉದ್ದಳತೆ,
		TRUE, &retval);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (retval.string != ಖಾಲಿ)
    {
	char_u	*t;

	ಗಾಗಿ (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    ಆದರೆ (*t == ':')
		*t = '%';
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ retval.string;
}

/*
 * Print the ATTENTION message: info about an existing swap file.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
attention_ಸಂದೇಶ(
    buf_T   *buf,	// buffer being edited
    char_u  *fname)	// swap file name
{
    stat_T	st;
    time_t	ಅದಲುಬದಲು_mtime;

    ++no_wait_return;
    (ಶೂನ್ಯ)emsg(_(e_attention));
    ಸಂದೇಶ_puts(_("\nFound a swap file by the name \""));
    ಸಂದೇಶ_home_replace(fname);
    ಸಂದೇಶ_puts("\"\n");
    ಅದಲುಬದಲು_mtime = swapfile_info(fname);
    ಸಂದೇಶ_puts(_("While opening file \""));
    ಸಂದೇಶ_outtrans(buf->b_fname);
    ಸಂದೇಶ_puts("\"\n");
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)buf->b_fname, &st) == -1)
    {
	ಸಂದೇಶ_puts(_("      CANNOT BE FOUND"));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಂದೇಶ_puts(_("             dated: "));
	ಸಂದೇಶ_puts(get_ctime(st.st_mtime, TRUE));
	ಆದರೆ (ಅದಲುಬದಲು_mtime != 0 && st.st_mtime > ಅದಲುಬದಲು_mtime)
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
    cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
    --no_wait_return;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    SEA_CHOICE_NONE = 0,
    SEA_CHOICE_READONLY = 1,
    SEA_CHOICE_EDIT = 2,
    SEA_CHOICE_RECOVER = 3,
    SEA_CHOICE_DELETE = 4,
    SEA_CHOICE_QUIT = 5,
    SEA_CHOICE_ABORT = 6
} sea_choice_T;

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Trigger the SwapExists autocommands.
 * Returns a value for equivalent to do_dialog().
 */
    ಸ್ಥಿರ sea_choice_T
do_swapexists(buf_T *buf, char_u *fname)
{
    ಗಣ_vim_var_string(VV_SWAPNAME, fname, -1);
    ಗಣ_vim_var_string(VV_SWAPCHOICE, ಖಾಲಿ, -1);

    // Trigger SwapExists autocommands with <afile> set to the file being
    // edited.  Disallow changing directory here.
    ++allbuf_ಬೀಗ;
    apply_autocmds(EVENT_SWAPEXISTS, buf->b_fname, ಖಾಲಿ, FALSE, ಖಾಲಿ);
    --allbuf_ಬೀಗ;

    ಗಣ_vim_var_string(VV_SWAPNAME, ಖಾಲಿ, -1);

    ಆಯ್ಕೆ (*get_vim_var_str(VV_SWAPCHOICE))
    {
	ಸ್ಥಿತಿ 'o': ಹಿಂತಿರುಗಿ SEA_CHOICE_READONLY;
	ಸ್ಥಿತಿ 'e': ಹಿಂತಿರುಗಿ SEA_CHOICE_EDIT;
	ಸ್ಥಿತಿ 'r': ಹಿಂತಿರುಗಿ SEA_CHOICE_RECOVER;
	ಸ್ಥಿತಿ 'd': ಹಿಂತಿರುಗಿ SEA_CHOICE_DELETE;
	ಸ್ಥಿತಿ 'q': ಹಿಂತಿರುಗಿ SEA_CHOICE_QUIT;
	ಸ್ಥಿತಿ 'a': ಹಿಂತಿರುಗಿ SEA_CHOICE_ABORT;
    }

    ಹಿಂತಿರುಗಿ SEA_CHOICE_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ		n;
    char_u	*dir_name;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AMIGA
    BPTR	fh;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		r;
    char_u	*buf_fname = buf->b_fname;

#ಆದರೆ !defined(UNIX)
# define CREATE_DUMMY_FILE
    FILE	*dummyfd = ಖಾಲಿ;

# ifdef MSWIN
    ಆದರೆ (buf_fname != ಖಾಲಿ && !mch_is_ತುಂಬಿದ_Name(buf_fname)
				       && vim_strchr(gettail(buf_fname), ':'))
    {
	char_u *t;

	buf_fname = vim_strsave(buf_fname);
	ಆದರೆ (buf_fname == ಖಾಲಿ)
	    buf_fname = buf->b_fname;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಾಗಿ (t = gettail(buf_fname); *t != NUL; MB_PTR_ADV(t))
		ಆದರೆ (*t == ':')
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
    ಆದರೆ (!(buf->b_p_sn || buf->b_shortname) && buf_fname != ಖಾಲಿ
					     && mch_getperm(buf_fname) < 0)
	dummyfd = mch_fopen((ಅಕ್ಷರ *)buf_fname, "w");
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Isolate a directory name from *dirp and put it in dir_name.
     * First allocate some memory to put the directory name in.
     */
    dir_name = alloc(STRLEN(*dirp) + 1);
    ಆದರೆ (dir_name == ಖಾಲಿ)
	*dirp = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)copy_option_part(dirp, dir_name, 31000, ",");

    /*
     * we try different names until we find one that does not exist yet
     */
    ಆದರೆ (dir_name == ಖಾಲಿ)	    // out of memory
	fname = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	fname = makeswapname(buf_fname, buf->b_ffname, buf, dir_name);

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (fname == ಖಾಲಿ)	// must be out of memory
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ ((n = (ಪೂರ್ಣಾಂಕ)STRLEN(fname)) == 0)	// safety check
	{
	    VIM_CLEAR(fname);
	    ನಿಲ್ಲಿಸು;
	}
#ಆದರೆ defined(UNIX)
/*
 * Some systems have a MS-DOS compatible filesystem that use 8.3 character
 * file names. If this is the first try and the swap file name does not fit in
 * 8.3, detect if this is the case, set shortname and try again.
 */
	ಆದರೆ (fname[n - 2] == 'w' && fname[n - 1] == 'p'
					&& !(buf->b_p_sn || buf->b_shortname))
	{
	    char_u	    *ಬಾಲ;
	    char_u	    *fname2;
	    stat_T	    s1, s2;
	    ಪೂರ್ಣಾಂಕ		    f1, f2;
	    ಪೂರ್ಣಾಂಕ		    created1 = FALSE, created2 = FALSE;
	    ಪೂರ್ಣಾಂಕ		    same = FALSE;

	    /*
	     * Check if swapfile name does not fit in 8.3:
	     * It either contains two dots, is longer than 8 chars, or starts
	     * with a dot.
	     */
	    ಬಾಲ = gettail(buf_fname);
	    ಆದರೆ (       vim_strchr(ಬಾಲ, '.') != ಖಾಲಿ
		    || STRLEN(ಬಾಲ) > (size_t)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		ಆದರೆ (fname2 != ಖಾಲಿ)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    ಆದರೆ (vim_strchr(ಬಾಲ, '.') != ಖಾಲಿ)
			fname2[n - 1] = 'x';
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*gettail(fname) == '.')
		    {
			fname2[n] = 'x';
			fname2[n + 1] = NUL;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			fname2[n - 5] += 1;
		    /*
		     * may need to create the files to be able to use mch_stat()
		     */
		    f1 = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_EXTRA, 0);
		    ಆದರೆ (f1 < 0)
		    {
			f1 = mch_open_rw((ಅಕ್ಷರ *)fname,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			created1 = TRUE;
		    }
		    ಆದರೆ (f1 >= 0)
		    {
			f2 = mch_open((ಅಕ್ಷರ *)fname2, O_RDONLY | O_EXTRA, 0);
			ಆದರೆ (f2 < 0)
			{
			    f2 = mch_open_rw((ಅಕ್ಷರ *)fname2,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			    created2 = TRUE;
			}
			ಆದರೆ (f2 >= 0)
			{
			    /*
			     * Both files exist now. If mch_stat() returns the
			     * same device and inode they are the same file.
			     */
			    ಆದರೆ (mch_fstat(f1, &s1) != -1
				    && mch_fstat(f2, &s2) != -1
				    && s1.st_dev == s2.st_dev
				    && s1.st_ino == s2.st_ino)
				same = TRUE;
			    close(f2);
			    ಆದರೆ (created2)
				mch_remove(fname2);
			}
			close(f1);
			ಆದರೆ (created1)
			    mch_remove(fname);
		    }
		    vim_free(fname2);
		    ಆದರೆ (same)
		    {
			buf->b_shortname = true;
			vim_free(fname);
			fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
			ಮುಂದುವರಿ;	// try again with b_shortname set
		    }
		}
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	/*
	 * check if the swapfile already exists
	 */
	ಆದರೆ (mch_getperm(fname) < 0)	// it does not exist
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    ಆದರೆ (mch_lstat((ಅಕ್ಷರ *)fname, &sb) < 0)
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (ದೀರ್ಘ)ಬಹುಲಕ_NEWFILE);
	    /*
	     * on the Amiga mch_getperm() will return -1 when the file exists
	     * but is being used by another program. This happens if you edit
	     * a file twice.
	     */
	    ಆದರೆ (fh != (BPTR)ಖಾಲಿ)	// can open file, OK
	    {
		Close(fh);
		mch_remove(fname);
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (IoErr() != ERROR_OBJECT_ಒಳ_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	}

	/*
	 * A file name equal to old_fname is OK to use.
	 */
	ಆದರೆ (old_fname != ಖಾಲಿ && fnamecmp(fname, old_fname) == 0)
	    ನಿಲ್ಲಿಸು;

	/*
	 * get here when file already exists
	 */
	ಆದರೆ (fname[n - 2] == 'w' && fname[n - 1] == 'p')	// first try
	{
	    /*
	     * on MS-DOS compatible filesystems (e.g. messydos) file.doc.swp
	     * and file.doc are the same file. To guess if this problem is
	     * present try if file.doc.swx exists. If it does, we set
	     * buf->b_shortname and try file_doc.swp (dots replaced by
	     * underscores for this file), and try again. If it doesn't we
	     * assume that "file.doc.swp" already exists.
	     */
	    ಆದರೆ (!(buf->b_p_sn || buf->b_shortname))	// not tried yet
	    {
		fname[n - 1] = 'x';
		r = mch_getperm(fname);		// try "file.swx"
		fname[n - 1] = 'p';
		ಆದರೆ (r >= 0)		    // "file.swx" seems to exist
		{
		    buf->b_shortname = true;
		    vim_free(fname);
		    fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
		    ಮುಂದುವರಿ;	    // try again with '.' replaced with '_'
		}
	    }
	    /*
	     * If we get here the ".swp" file really exists.
	     * Give an error message, unless recovering, no file name, we are
	     * viewing a help file or when the path of the file is different
	     * (happens when all .swp files are in one directory).
	     */
	    ಆದರೆ (!recoverymode && buf_fname != ಖಾಲಿ
				&& !buf->b_help
				&& !(buf->b_flags & (BF_DUMMY | BF_NO_SEA)))
	    {
		ಪೂರ್ಣಾಂಕ		fd;
		ರಚನೆ block0	b0;
		ಪೂರ್ಣಾಂಕ		ವ್ಯತ್ಯಾಸ = FALSE;

		/*
		 * Try to read block 0 from the swap file to get the original
		 * file name (and inode number).
		 */
		fd = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY | O_EXTRA, 0);
		ಆದರೆ (fd >= 0)
		{
		    ಆದರೆ (read_eintr(fd, &b0, ಗಾತ್ರ(b0)) == ಗಾತ್ರ(b0))
		    {
			/*
			 * If the swapfile has the same directory as the
			 * buffer don't compare the directory names, they can
			 * have a different mountpoint.
			 */
			ಆದರೆ (b0.b0_flags & B0_SAME_DIR)
			{
			    ಆದರೆ (fnamecmp(gettail(buf->b_ffname),
						   gettail(b0.b0_fname)) != 0
				    || !same_directory(fname, buf->b_ffname))
			    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				ಆದರೆ (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_ಗಮ್ಯ_long(b0.b0_ino)))
#ಆದರೆ_ಅಂತ್ಯ
				    ವ್ಯತ್ಯಾಸ = TRUE;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    /*
			     * The name in the swap file may be
			     * "~user/path/file".  Expand it first.
			     */
			    expand_env(b0.b0_fname, NameBuff, MAXPATHL);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_INODE
			    ಆದರೆ (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_ಗಮ್ಯ_long(b0.b0_ino)))
				ವ್ಯತ್ಯಾಸ = TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
			    ಆದರೆ (fnamecmp(NameBuff, buf->b_ffname) != 0)
				ವ್ಯತ್ಯಾಸ = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
			}
		    }
		    close(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		ಆದರೆ (ವ್ಯತ್ಯಾಸ == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == ಖಾಲಿ)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CREATE_DUMMY_FILE
		    ಪೂರ್ಣಾಂಕ		 did_use_dummy = FALSE;

		    // Avoid getting a warning for the file being created
		    // outside of Vim, it was created at the start of this
		    // function.  Delete the file now, because Vim might exit
		    // here if the window is closed.
		    ಆದರೆ (dummyfd != ಖಾಲಿ)
		    {
			fclose(dummyfd);
			dummyfd = ಖಾಲಿ;
			mch_remove(buf_fname);
			did_use_dummy = TRUE;
		    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PROCESS_STILL_RUNNING
		    process_still_running = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
		    // It's safe to delete the swap file if all these are true:
		    // - the edited file exists
		    // - the swap file has no changes and looks OK
		    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)buf->b_fname, &st) == 0
						  && swapfile_unchanged(fname))
		    {
			choice = SEA_CHOICE_DELETE;
			ಆದರೆ (p_verbose > 0)
			    verb_ಸಂದೇಶ(_("Found a swap file that is not useful, deleting it"));
		    }

#ಆದರೆ defined(FEAT_EVAL)
		    /*
		     * If there is an SwapExists autocommand and we can handle
		     * the response, trigger it.  It may return 0 to ask the
		     * user anyway.
		     */
		    ಆದರೆ (choice == SEA_CHOICE_NONE
			    && ಅದಲುಬದಲು_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#ಆದರೆ_ಅಂತ್ಯ

		    ಆದರೆ (choice == SEA_CHOICE_NONE
					 && ಅದಲುಬದಲು_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    ಆದರೆ (choice == SEA_CHOICE_NONE)
		    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			ಆದರೆ (gui.starting && !gui.ಒಳ_use)
			    gui_ಆರಂಭ(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
			// Show info about the existing swap file.
			attention_ಸಂದೇಶ(buf, fname);

			// We don't want a 'q' typed at the more-prompt
			// interrupt loading a file.
			got_int = FALSE;

			// If vimrc has "simalt ~x" we don't want it to
			// interfere with the prompt here.
			flush_buffers(FLUSH_TYPEAHEAD);
		    }

#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    ಆದರೆ (ಅದಲುಬದಲು_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    ಪೂರ್ವಪ್ರತ್ಯಯ = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			size_t	    ಸಂದೇಶ_ಗಾತ್ರ;
			string_T    ಸಂದೇಶ_2;
			char_u	    *tofree;
			ಪೂರ್ಣಾಂಕ	    dialog_ಫಲಿತಾಂಶ;

			ಪೂರ್ವಪ್ರತ್ಯಯ.ಉದ್ದಳತೆ = STRLEN(ಪೂರ್ವಪ್ರತ್ಯಯ.string);
			suffix.ಉದ್ದಳತೆ = STRLEN(suffix.string);
			ಸಂದೇಶ_ಗಾತ್ರ = ಪೂರ್ವಪ್ರತ್ಯಯ.ಉದ್ದಳತೆ
				+ STRLEN(fname)
				+ suffix.ಉದ್ದಳತೆ + 5;
			ಸಂದೇಶ_2.string = tofree = alloc(ಸಂದೇಶ_ಗಾತ್ರ);
			ಆದರೆ (ಸಂದೇಶ_2.string != ಖಾಲಿ)
			{
			    STRCPY(ಸಂದೇಶ_2.string, ಪೂರ್ವಪ್ರತ್ಯಯ.string);
			    ಸಂದೇಶ_2.ಉದ್ದಳತೆ = ಪೂರ್ವಪ್ರತ್ಯಯ.ಉದ್ದಳತೆ;
			    ಸಂದೇಶ_2.ಉದ್ದಳತೆ += home_replace(ಖಾಲಿ, fname,
				ಸಂದೇಶ_2.string + ಸಂದೇಶ_2.ಉದ್ದಳತೆ, (ಪೂರ್ಣಾಂಕ)(ಸಂದೇಶ_ಗಾತ್ರ - ಸಂದೇಶ_2.ಉದ್ದಳತೆ), TRUE);
			    STRCPY(ಸಂದೇಶ_2.string + ಸಂದೇಶ_2.ಉದ್ದಳತೆ, suffix.string);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    ಸಂದೇಶ_2.string = (char_u *)_("Swap file already exists!");
			dialog_ಫಲಿತಾಂಶ = do_dialog(VIM_WARNING,
				    (char_u *)_("VIM - ATTENTION"),
				    ಸಂದೇಶ_2.string,
# ifdef HAVE_PROCESS_STILL_RUNNING
				    process_still_running
					? (char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Quit\n&Abort") :
# endif
					(char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Delete it\n&Quit\n&Abort"), 1, ಖಾಲಿ, FALSE);

# ifdef HAVE_PROCESS_STILL_RUNNING
			ಆದರೆ (process_still_running && dialog_ಫಲಿತಾಂಶ >= 4)
			    // compensate for missing "Delete it" button
			    dialog_ಫಲಿತಾಂಶ++;
# endif
			choice = dialog_ಫಲಿತಾಂಶ;
			vim_free(tofree);

			// pretend screen didn't scroll, need redraw anyway
			ಸಂದೇಶ_scrolled = 0;
			redraw_all_later(UPD_NOT_VALID);
		    }
#ಆದರೆ_ಅಂತ್ಯ

		    ಆಯ್ಕೆ (choice)
		    {
			ಸ್ಥಿತಿ SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ SEA_CHOICE_EDIT:
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ SEA_CHOICE_RECOVER:
			    ಅದಲುಬದಲು_exists_action = SEA_RECOVER;
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ SEA_CHOICE_QUIT:
			    ಅದಲುಬದಲು_exists_action = SEA_QUIT;
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ SEA_CHOICE_ABORT:
			    ಅದಲುಬದಲು_exists_action = SEA_QUIT;
			    got_int = TRUE;
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ SEA_CHOICE_NONE:
			    ಸಂದೇಶ_puts("\n");
			    ಆದರೆ (ಸಂದೇಶ_silent == 0)
				// call wait_return() later
				need_wait_return = TRUE;
			    ನಿಲ್ಲಿಸು;
		    }

		    // If the file was deleted this fname can be used.
		    ಆದರೆ (choice != SEA_CHOICE_NONE && mch_getperm(fname) < 0)
			ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    ಆದರೆ (did_use_dummy)
			dummyfd = mch_fopen((ಅಕ್ಷರ *)buf_fname, "w");
#ಆದರೆ_ಅಂತ್ಯ
		}
	    }
	}

	/*
	 * Change the ".swp" extension to find another file that can be used.
	 * First decrement the last char: ".swo", ".swn", etc.
	 * If that still isn't enough decrement the last but one char: ".svz"
	 * Can happen when editing many "No Name" buffers.
	 */
	ಆದರೆ (fname[n - 1] == 'a')	// ".s?a"
	{
	    ಆದರೆ (fname[n - 2] == 'a')    // ".saa": tried enough, give up
	    {
		emsg(_(e_too_many_ಅದಲುಬದಲು_files_ಸಿಕ್ಕಿತು));
		VIM_CLEAR(fname);
		ನಿಲ್ಲಿಸು;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CREATE_DUMMY_FILE
    ಆದರೆ (dummyfd != ಖಾಲಿ)	// file has been created temporarily
    {
	fclose(dummyfd);
	mch_remove(buf_fname);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ fname;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
b0_magic_wrong(ZERO_BL *b0p)
{
    ಹಿಂತಿರುಗಿ (b0p->b0_magic_long != (ದೀರ್ಘ)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (ಪೂರ್ಣಾಂಕ)B0_MAGIC_INT
	    || b0p->b0_magic_short != (ಗಿಡ್ಡ)B0_MAGIC_SHORT
	    || b0p->b0_magic_char != B0_MAGIC_CHAR);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_INODE
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fnamecmp_ino(
    char_u	*fname_c,	    // current file name
    char_u	*fname_s,	    // file name from swap file
    ದೀರ್ಘ	ino_block0)
{
    stat_T	st;
    ino_t	ino_c = 0;	    // ino of current file
    ino_t	ino_s;		    // ino of file from swap file
    char_u	buf_c[MAXPATHL];    // full path of fname_c
    char_u	buf_s[MAXPATHL];    // full path of fname_s
    ಪೂರ್ಣಾಂಕ		retval_c;	    // flag: buf_c valid
    ಪೂರ್ಣಾಂಕ		retval_s;	    // flag: buf_s valid

    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname_c, &st) == 0)
	ino_c = (ino_t)st.st_ino;

    /*
     * First we try to get the inode from the file name, because the inode in
     * the swap file may be outdated.  If that fails (e.g. this path is not
     * valid on this machine), use the inode from block 0.
     */
    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)fname_s, &st) == 0)
	ino_s = (ino_t)st.st_ino;
    ಇಲ್ಲದಿದ್ದರೆ
	ino_s = (ino_t)ino_block0;

    ಆದರೆ (ino_c && ino_s)
	ಹಿಂತಿರುಗಿ (ino_c != ino_s);

    /*
     * One of the inode numbers is unknown, try a forced vim_FullName() and
     * compare the file names.
     */
    retval_c = vim_ತುಂಬಿದ_Name(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_ತುಂಬಿದ_Name(fname_s, buf_s, MAXPATHL, TRUE);
    ಆದರೆ (retval_c == OK && retval_s == OK)
	ಹಿಂತಿರುಗಿ STRCMP(buf_c, buf_s) != 0;

    /*
     * Can't compare inodes or file names, guess that the files are different,
     * unless both appear not to exist at all, then compare with the file name
     * in the swap file.
     */
    ಆದರೆ (ino_s == 0 && ino_c == 0 && retval_c == FAIL && retval_s == FAIL)
	ಹಿಂತಿರುಗಿ STRCMP(fname_c, fname_s) != 0;
    ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
long_ಗಮ್ಯ_char(ದೀರ್ಘ n, char_u *s)
{
    s[0] = (char_u)(n & 0xff);
    n = (ಚಿಹ್ನೆರಹಿತ)n >> 8;
    s[1] = (char_u)(n & 0xff);
    n = (ಚಿಹ್ನೆರಹಿತ)n >> 8;
    s[2] = (char_u)(n & 0xff);
    n = (ಚಿಹ್ನೆರಹಿತ)n >> 8;
    s[3] = (char_u)(n & 0xff);
}

    ಸ್ಥಿರ ದೀರ್ಘ
char_ಗಮ್ಯ_long(char_u *s)
{
    ದೀರ್ಘ    retval;

    retval = s[3];
    retval <<= 8;
    retval |= s[2];
    retval <<= 8;
    retval |= s[1];
    retval <<= 8;
    retval |= s[0];

    ಹಿಂತಿರುಗಿ retval;
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

    ಆದರೆ (!buf->b_ml.ml_mfp)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (hp = buf->b_ml.ml_mfp->mf_ಬಳಸಿದ_ಕೊನೆಯ; hp != ಖಾಲಿ; hp = hp->bh_prev)
    {
	ಆದರೆ (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_ದತ್ತಾಂಶ);
	    b0p->b0_dirty = buf->b_ಬದಲಾಯಿತು ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_ಮುಖವಾಡ)
						  | (get_fileformat(buf) + 1);
	    ಕೂಡಿಸು_b0_fenc(b0p, buf);
	    hp->bh_flags |= BH_DIRTY;
	    mf_sync(buf->b_ml.ml_mfp, MFS_ZERO);
	    ನಿಲ್ಲಿಸು;
	}
    }
}

#ಆದರೆ defined(FEAT_CRYPT)
/*
 * If "data" points to a data block encrypt the text in it and return a copy
 * in allocated memory.  Return NULL when out of memory.
 * Otherwise return "data".
 */
    char_u *
ml_encrypt_ದತ್ತಾಂಶ(
    memfile_T	*mfp,
    char_u	*ದತ್ತಾಂಶ,
    off_T	ಸ್ಥಾನಾಂತರ,
    ಚಿಹ್ನೆರಹಿತ	ಗಾತ್ರ_2)
{
    ದತ್ತಾಂಶ_BL	*dp = (ದತ್ತಾಂಶ_BL *)ದತ್ತಾಂಶ;
    char_u	*ತಲೆ_ಕೊನೆ;
    char_u	*ಪಠ್ಯ_ಆರಂಭ;
    char_u	*new_ದತ್ತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_len;
    cryptstate_T *state;

    ಆದರೆ (dp->db_id != ದತ್ತಾಂಶ_ID)
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;

    state = ml_crypt_prepare(mfp, ಸ್ಥಾನಾಂತರ, FALSE);
    ಆದರೆ (state == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;

    new_ದತ್ತಾಂಶ = alloc(ಗಾತ್ರ_2);
    ಆದರೆ (new_ದತ್ತಾಂಶ == ಖಾಲಿ)
    {
	crypt_free_state(state);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ತಲೆ_ಕೊನೆ = (char_u *)(&dp->db_ಸೂಚ್ಯಂಕ[dp->db_ಸಾಲು_ಎಣಿಕೆ]);
    ಪಠ್ಯ_ಆರಂಭ = (char_u *)dp + dp->db_txt_ಆರಂಭ;
    ಪಠ್ಯ_len = ಗಾತ್ರ_2 - dp->db_txt_ಆರಂಭ;

    // Copy the header and the text.
    mch_memmove(new_ದತ್ತಾಂಶ, dp, ತಲೆ_ಕೊನೆ - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, ಪಠ್ಯ_ಆರಂಭ, ಪಠ್ಯ_len, new_ದತ್ತಾಂಶ + dp->db_txt_ಆರಂಭ,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    ಆದರೆ (ತಲೆ_ಕೊನೆ < ಪಠ್ಯ_ಆರಂಭ)
	vim_memset(new_ದತ್ತಾಂಶ + (ತಲೆ_ಕೊನೆ - ದತ್ತಾಂಶ), 0, ಪಠ್ಯ_ಆರಂಭ - ತಲೆ_ಕೊನೆ);

    ಹಿಂತಿರುಗಿ new_ದತ್ತಾಂಶ;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    ಶೂನ್ಯ
ml_decrypt_ದತ್ತಾಂಶ(
    memfile_T	*mfp,
    char_u	*ದತ್ತಾಂಶ,
    off_T	ಸ್ಥಾನಾಂತರ,
    ಚಿಹ್ನೆರಹಿತ	ಗಾತ್ರ_2)
{
    ದತ್ತಾಂಶ_BL	*dp = (ದತ್ತಾಂಶ_BL *)ದತ್ತಾಂಶ;
    char_u	*ತಲೆ_ಕೊನೆ;
    char_u	*ಪಠ್ಯ_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_len;
    cryptstate_T *state;

    ಆದರೆ (dp->db_id != ದತ್ತಾಂಶ_ID)
	ಹಿಂತಿರುಗಿ;

    ತಲೆ_ಕೊನೆ = (char_u *)(&dp->db_ಸೂಚ್ಯಂಕ[dp->db_ಸಾಲು_ಎಣಿಕೆ]);
    ಪಠ್ಯ_ಆರಂಭ = (char_u *)dp + dp->db_txt_ಆರಂಭ;
    ಪಠ್ಯ_len = dp->db_txt_ಕೊನೆ - dp->db_txt_ಆರಂಭ;

    ಆದರೆ (ತಲೆ_ಕೊನೆ > ಪಠ್ಯ_ಆರಂಭ || dp->db_txt_ಆರಂಭ > ಗಾತ್ರ_2
	    || dp->db_txt_ಕೊನೆ > ಗಾತ್ರ_2)
	ಹಿಂತಿರುಗಿ;  // data was messed up

    state = ml_crypt_prepare(mfp, ಸ್ಥಾನಾಂತರ, TRUE);
    ಆದರೆ (state == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Decrypt the text in place.
    crypt_decode_inplace(state, ಪಠ್ಯ_ಆರಂಭ, ಪಠ್ಯ_len, FALSE);
    crypt_free_state(state);
}

/*
 * Prepare for encryption/decryption, using the key, seed and offset.
 * Return an allocated cryptstate_T *.
 * Note: Encryption not supported for SODIUM
 */
    ಸ್ಥಿರ cryptstate_T *
ml_crypt_prepare(memfile_T *mfp, off_T ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ reading)
{
    buf_T	*buf = mfp->mf_ಬಫರ್;
    char_u	salt[50];
    ಪೂರ್ಣಾಂಕ		ವಿಧಾನ_nr;
    char_u	*ಕೀಲಿ;
    crypt_arg_T arg;

    CLEAR_ಕ್ಷೇತ್ರ(arg);
    ಆದರೆ (reading && mfp->mf_old_ಕೀಲಿ != ಖಾಲಿ)
    {
	// Reading back blocks with the previous key/method/seed.
	ವಿಧಾನ_nr = mfp->mf_old_cm;
	ಕೀಲಿ = mfp->mf_old_ಕೀಲಿ;
	arg.cat_ಬೀಜ = mfp->mf_old_ಬೀಜ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ವಿಧಾನ_nr = crypt_get_ವಿಧಾನ_nr(buf);
	ಕೀಲಿ = buf->b_p_ಕೀಲಿ;
	arg.cat_ಬೀಜ = mfp->mf_ಬೀಜ;
    }

    ಆದರೆ (*ಕೀಲಿ == NUL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (crypt_may_close_swapfile(buf, ಕೀಲಿ, ವಿಧಾನ_nr))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ವಿಧಾನ_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((ಅಕ್ಷರ *)salt, ಗಾತ್ರ(salt), "%s%ld", ಕೀಲಿ, (ದೀರ್ಘ)ಸ್ಥಾನಾಂತರ);
	arg.cat_ಬೀಜ = ಖಾಲಿ;
	arg.cat_init_from_file = FALSE;

	ಹಿಂತಿರುಗಿ crypt_create(ವಿಧಾನ_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((ಅಕ್ಷರ *)salt, ಗಾತ್ರ(salt), "%ld", (ದೀರ್ಘ)ಸ್ಥಾನಾಂತರ);

    arg.cat_salt = salt;
    arg.cat_salt_len = (ಪೂರ್ಣಾಂಕ)STRLEN(salt);
    arg.cat_ಬೀಜ_len = MF_ಬೀಜ_LEN;
    arg.cat_ಕೂಡಿಸು_len = 0;
    arg.cat_ಕೂಡಿಸು = ಖಾಲಿ;
    arg.cat_init_from_file = FALSE;

    ಹಿಂತಿರುಗಿ crypt_create(ವಿಧಾನ_nr, ಕೀಲಿ, &arg);
}

#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ defined(FEAT_BYTEOFF)

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
    ದೀರ್ಘ	len,
    ಪೂರ್ಣಾಂಕ		updtype)
{
    ಸ್ಥಿರ buf_T	*ml_upd_lastbuf = ಖಾಲಿ;
    ಸ್ಥಿರ linenr_T	ml_upd_lastcurline;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    ಪೂರ್ಣಾಂಕ			curix = ml_upd_lastcurix;
    ದೀರ್ಘ		ಗಾತ್ರ_2;
    chunksize_T		*curchnk;
    ಪೂರ್ಣಾಂಕ			rest;
    bhdr_T		*hp;
    ದತ್ತಾಂಶ_BL		*dp;

    ಆದರೆ (buf->b_ml.ml_usedchunks == -1 || len == 0)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (buf->b_ml.ml_chunksize == ಖಾಲಿ)
    {
	buf->b_ml.ml_chunksize = ALLOC_MULT(chunksize_T, 100);
	ಆದರೆ (buf->b_ml.ml_chunksize == ಖಾಲಿ)
	{
	    buf->b_ml.ml_usedchunks = -1;
	    ಹಿಂತಿರುಗಿ;
	}
	buf->b_ml.ml_numchunks = 100;
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = 1;
	ml_upd_lastbuf = ಖಾಲಿ;		// invalidate resume cache
    }

    ಆದರೆ (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (ದೀರ್ಘ)buf->b_ml.ml_ಸಾಲು_len;
	ml_upd_lastbuf = ಖಾಲಿ;		// invalidate resume cache
	ಹಿಂತಿರುಗಿ;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    ಆದರೆ (buf != ml_upd_lastbuf || ಸಾಲು < curline)
    {
	curline = 1;
	curix = 0;
    }
    ಗಾಗಿ (; curix < buf->b_ml.ml_usedchunks - 1
	     && ಸಾಲು >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	     curix++)
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
    curchnk = buf->b_ml.ml_chunksize + curix;

    ಆದರೆ (updtype == ML_CHNK_DELLINE)
	len = -len;
    curchnk->mlcs_totalsize += len;
    ಆದರೆ (updtype == ML_CHNK_ADDLINE)
    {
	curchnk->mlcs_numlines++;

	// May resize here so we don't have to do it in both cases below
	ಆದರೆ (buf->b_ml.ml_usedchunks + 1 >= buf->b_ml.ml_numchunks)
	{
	    chunksize_T *t_chunksize = buf->b_ml.ml_chunksize;

	    buf->b_ml.ml_numchunks = buf->b_ml.ml_numchunks * 3 / 2;
	    buf->b_ml.ml_chunksize = vim_realloc(buf->b_ml.ml_chunksize,
			    ಗಾತ್ರ(chunksize_T) * buf->b_ml.ml_numchunks);
	    ಆದರೆ (buf->b_ml.ml_chunksize == ಖಾಲಿ)
	    {
		// Hmmmm, Give up on offset for this buffer
		vim_free(t_chunksize);
		buf->b_ml.ml_usedchunks = -1;
		ಹಿಂತಿರುಗಿ;
	    }
	}

	ಆದರೆ (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MAXL)
	{
	    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2;	    // number of entries in block
	    ಪೂರ್ಣಾಂಕ	    idx;
	    ಪೂರ್ಣಾಂಕ	    ಕೊನೆ_idx;
	    ಪೂರ್ಣಾಂಕ	    ಪಠ್ಯ_ಕೊನೆ;
	    ಪೂರ್ಣಾಂಕ	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			ಗಾತ್ರ(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    ಗಾತ್ರ_2 = 0;
	    linecnt = 0;
	    ಆಗ (curline < buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
			&& linecnt < MLCS_MINL)
	    {
		ಆದರೆ ((hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, curline, ML_ಕಂಡುಹಿಡಿ)) == ಖಾಲಿ)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    ಹಿಂತಿರುಗಿ;
		}
		dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
		ಎಣಿಕೆ_2 = (ದೀರ್ಘ)(buf->b_ml.ml_locked_ಮೇಲಿನ) -
			(ದೀರ್ಘ)(buf->b_ml.ml_locked_ಕೆಳಗಿನ) + 1;
		idx = curline - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
		curline = buf->b_ml.ml_locked_ಮೇಲಿನ + 1;

		// compute index of last line to use in this MEMLINE
		rest = ಎಣಿಕೆ_2 - idx;
		ಆದರೆ (linecnt + rest > MLCS_MINL)
		{
		    ಕೊನೆ_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಕೊನೆ_idx = ಎಣಿಕೆ_2 - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		ಆದರೆ (buf->b_has_textprop)
		{
		    ಪೂರ್ಣಾಂಕ i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    ಗಾಗಿ (i = ಕೊನೆ_idx; i < idx; ++i)
			ಗಾತ್ರ_2 += (ಪೂರ್ಣಾಂಕ)STRLEN((char_u *)dp
				      + (dp->db_ಸೂಚ್ಯಂಕ[i] & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ)) + 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
# endif
		{
		    ಆದರೆ (idx == 0) // first line in block, text at the end
			ಪಠ್ಯ_ಕೊನೆ = dp->db_txt_ಕೊನೆ;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಪಠ್ಯ_ಕೊನೆ = ((dp->db_ಸೂಚ್ಯಂಕ[idx - 1]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
		    ಗಾತ್ರ_2 += ಪಠ್ಯ_ಕೊನೆ
				   - ((dp->db_ಸೂಚ್ಯಂಕ[ಕೊನೆ_idx]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
		}
	    }
	    buf->b_ml.ml_chunksize[curix].mlcs_numlines = linecnt;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_numlines -= linecnt;
	    buf->b_ml.ml_chunksize[curix].mlcs_totalsize = ಗಾತ್ರ_2;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_totalsize -= ಗಾತ್ರ_2;
	    buf->b_ml.ml_usedchunks++;
	    ml_upd_lastbuf = ಖಾಲಿ;   // Force recalc of curix & curline
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - ಸಾಲು <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    ಆದರೆ (ಸಾಲು == buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    {
		curchnk->mlcs_numlines = 0;
		curchnk->mlcs_totalsize = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		/*
		 * Line is just prior to last, move count for last
		 * This is the common case  when loading a new file
		 */
		hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, ML_ಕಂಡುಹಿಡಿ);
		ಆದರೆ (hp == ಖಾಲಿ)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    ಹಿಂತಿರುಗಿ;
		}
		dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
		ಆದರೆ (dp->db_ಸಾಲು_ಎಣಿಕೆ == 1)
		    rest = dp->db_txt_ಕೊನೆ - dp->db_txt_ಆರಂಭ;
		ಇಲ್ಲದಿದ್ದರೆ
		    rest =
			((dp->db_ಸೂಚ್ಯಂಕ[dp->db_ಸಾಲು_ಎಣಿಕೆ - 2]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ)
			- dp->db_txt_ಆರಂಭ;
		curchnk->mlcs_totalsize = rest;
		curchnk->mlcs_numlines = 1;
		curchnk[-1].mlcs_totalsize -= rest;
		curchnk[-1].mlcs_numlines -= 1;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (updtype == ML_CHNK_DELLINE)
    {
	curchnk->mlcs_numlines--;
	ಆದರೆ (curix < buf->b_ml.ml_usedchunks - 1
		&& curchnk->mlcs_numlines + curchnk[1].mlcs_numlines
								  <= MLCS_MINL)
	{
	    curix++;
	    curchnk = buf->b_ml.ml_chunksize + curix;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curix == 0 && curchnk->mlcs_numlines <= 0)
	{
	    ml_upd_lastbuf = ಖಾಲಿ;   // Force recalc of curix & curline
	    buf->b_ml.ml_usedchunks--;
	    mch_memmove(buf->b_ml.ml_chunksize, buf->b_ml.ml_chunksize + 1,
			buf->b_ml.ml_usedchunks * ಗಾತ್ರ(chunksize_T));
	    ಹಿಂತಿರುಗಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curix == 0 || (curchnk->mlcs_numlines > 10
		    && curchnk->mlcs_numlines + curchnk[-1].mlcs_numlines
								  > MLCS_MINL))
	{
	    // The chunks are left as they are, the cached position stays
	    // valid.
	    ml_upd_lastbuf = buf;
	    ml_upd_lastcurline = curline;
	    ml_upd_lastcurix = curix;
	    ಹಿಂತಿರುಗಿ;
	}

	// Collapse chunks
	ml_upd_lastbuf = ಖಾಲಿ;   // Force recalc of curix & curline
	curchnk[-1].mlcs_numlines += curchnk->mlcs_numlines;
	curchnk[-1].mlcs_totalsize += curchnk->mlcs_totalsize;
	buf->b_ml.ml_usedchunks--;
	ಆದರೆ (curix < buf->b_ml.ml_usedchunks)
	    mch_memmove(buf->b_ml.ml_chunksize + curix,
			buf->b_ml.ml_chunksize + curix + 1,
			(buf->b_ml.ml_usedchunks - curix) *
			ಗಾತ್ರ(chunksize_T));
	ಹಿಂತಿರುಗಿ;
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
    ದೀರ್ಘ
ml_ಕಂಡುಹಿಡಿ_ಸಾಲು_or_ಸ್ಥಾನಾಂತರ(buf_T *buf, linenr_T lnum, ದೀರ್ಘ *offp)
{
    linenr_T	curline;
    ಪೂರ್ಣಾಂಕ		curix;
    ದೀರ್ಘ	ಗಾತ್ರ_2;
    bhdr_T	*hp;
    ದತ್ತಾಂಶ_BL	*dp;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;		// number of entries in block
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_idx;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_ಕೊನೆ;
    ದೀರ್ಘ	ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		ffdos = (get_fileformat(buf) == EOL_DOS);
    ಪೂರ್ಣಾಂಕ		extra = 0;

    // take care of cached line first
    ml_flush_ಸಾಲು(curbuf);

    ಆದರೆ (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == ಖಾಲಿ
	    || lnum < 0)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (offp == ಖಾಲಿ)
	ಸ್ಥಾನಾಂತರ = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಾನಾಂತರ = *offp;
    ಆದರೆ (lnum == 0 && ಸ್ಥಾನಾಂತರ <= 0)
	ಹಿಂತಿರುಗಿ 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = ಗಾತ್ರ_2 = 0;
    ಆಗ (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (ಸ್ಥಾನಾಂತರ != 0
	       && ಸ್ಥಾನಾಂತರ > ಗಾತ್ರ_2 + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (ದೀರ್ಘ)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	ಗಾತ್ರ_2 += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	ಆದರೆ (ಸ್ಥಾನಾಂತರ && ffdos)
	    ಗಾತ್ರ_2 += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    ಆಗ ((lnum != 0 && curline < lnum) || (ಸ್ಥಾನಾಂತರ != 0 && ಗಾತ್ರ_2 < ಸ್ಥಾನಾಂತರ))
    {
# ifdef FEAT_PROP_POPUP
	size_t textprop_ಒಟ್ಟು = 0;
# endif

	ಆದರೆ (curline > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
		|| (hp = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು(buf, curline, ML_ಕಂಡುಹಿಡಿ)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	dp = (ದತ್ತಾಂಶ_BL *)(hp->bh_ದತ್ತಾಂಶ);
	ಎಣಿಕೆ_2 = (ದೀರ್ಘ)(buf->b_ml.ml_locked_ಮೇಲಿನ) -
		(ದೀರ್ಘ)(buf->b_ml.ml_locked_ಕೆಳಗಿನ) + 1;
	ಆರಂಭ_idx = idx = curline - buf->b_ml.ml_locked_ಕೆಳಗಿನ;
	ಆದರೆ (idx == 0)  // first line in block, text at the end
	    ಪಠ್ಯ_ಕೊನೆ = dp->db_txt_ಕೊನೆ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪಠ್ಯ_ಕೊನೆ = ((dp->db_ಸೂಚ್ಯಂಕ[idx - 1]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
	// Compute index of last line to use in this MEMLINE
	ಆದರೆ (lnum != 0)
	{
	    ಆದರೆ (curline + (ಎಣಿಕೆ_2 - idx) >= lnum)
		idx += lnum - curline - 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		idx = ಎಣಿಕೆ_2 - 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    extra = 0;
	    ಗಾಗಿ (;;)
	    {
# ifdef FEAT_PROP_POPUP
		size_t textprop_ಗಾತ್ರ = 0;

		ಆದರೆ (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_ಸೂಚ್ಯಂಕ[idx]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_ಕೊನೆ
				  : ((dp->db_ಸೂಚ್ಯಂಕ[idx - 1]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ));
		    textprop_ಗಾತ್ರ = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		ಆದರೆ (!(ಸ್ಥಾನಾಂತರ >= ಗಾತ್ರ_2
			+ ಪಠ್ಯ_ಕೊನೆ - (ಪೂರ್ಣಾಂಕ)((dp->db_ಸೂಚ್ಯಂಕ[idx]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ)
# ifdef FEAT_PROP_POPUP
			- (ದೀರ್ಘ)(textprop_ಒಟ್ಟು + textprop_ಗಾತ್ರ)
# endif
			+ ffdos))
		    ನಿಲ್ಲಿಸು;

		ಆದರೆ (ffdos)
		    ಗಾತ್ರ_2++;
# ifdef FEAT_PROP_POPUP
		textprop_ಒಟ್ಟು += textprop_ಗಾತ್ರ;
# endif
		ಆದರೆ (idx == ಎಣಿಕೆ_2 - 1)
		{
		    extra = 1;
		    ನಿಲ್ಲಿಸು;
		}
		idx++;
	    }
	}
# ifdef FEAT_PROP_POPUP
	ಆದರೆ (buf->b_has_textprop && lnum != 0)
	{
	    ಪೂರ್ಣಾಂಕ i;

	    // cannot use the db_index pointer, need to get the actual text
	    // lengths.
	    len = 0;
	    ಗಾಗಿ (i = ಆರಂಭ_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_ಸೂಚ್ಯಂಕ[i]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ);
		len += (ಪೂರ್ಣಾಂಕ)STRLEN(p) + 1;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    len = ಪಠ್ಯ_ಕೊನೆ - ((dp->db_ಸೂಚ್ಯಂಕ[idx]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ)
# ifdef FEAT_PROP_POPUP
				- (ದೀರ್ಘ)textprop_ಒಟ್ಟು
# endif
				;
	ಗಾತ್ರ_2 += len;
	ಆದರೆ (ಸ್ಥಾನಾಂತರ != 0 && ಗಾತ್ರ_2 >= ಸ್ಥಾನಾಂತರ)
	{
	    ಆದರೆ (ಗಾತ್ರ_2 + ffdos == ಸ್ಥಾನಾಂತರ)
		*offp = 0;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx == ಆರಂಭ_idx)
		*offp = ಸ್ಥಾನಾಂತರ - ಗಾತ್ರ_2 + len;
	    ಇಲ್ಲದಿದ್ದರೆ
		*offp = ಸ್ಥಾನಾಂತರ - ಗಾತ್ರ_2 + len
		     - (ಪಠ್ಯ_ಕೊನೆ - ((dp->db_ಸೂಚ್ಯಂಕ[idx - 1]) & DB_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ))
# ifdef FEAT_PROP_POPUP
		     + (ದೀರ್ಘ)textprop_ಒಟ್ಟು
# endif
		     ;
	    curline += idx - ಆರಂಭ_idx + extra;
	    ಆದರೆ (curline > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ -1;	// exactly one byte beyond the end
	    ಹಿಂತಿರುಗಿ curline;
	}
	curline = buf->b_ml.ml_locked_ಮೇಲಿನ + 1;
    }

    ಆದರೆ (lnum != 0)
    {
	// Count extra CR characters.
	ಆದರೆ (ffdos)
	    ಗಾತ್ರ_2 += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	ಆದರೆ ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಗಾತ್ರ_2 -= ffdos + 1;
    }

    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    ಶೂನ್ಯ
goto_byte(ದೀರ್ಘ cnt)
{
    ದೀರ್ಘ	boff = cnt;
    linenr_T	lnum;

    ml_flush_ಸಾಲು(curbuf);	// cached line may be dirty
    setpcmark();
    ಆದರೆ (boff)
	--boff;
    lnum = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು_or_ಸ್ಥಾನಾಂತರ(curbuf, (linenr_T)0, &boff);
    ಆದರೆ (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	curwin->w_curswant = MAXCOL;
	coladvance((colnr_T)MAXCOL);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.ಲಂಬಸಾಲು = (colnr_T)boff;
	curwin->w_cursor.coladd = 0;
	curwin->w_ಗಣ_curswant = true;
    }
    check_cursor();

    // Make sure the cursor is on the first byte of a multi-byte char.
    ಆದರೆ (has_mbyte)
	mb_adjust_cursor();
}
#ಆದರೆ_ಅಂತ್ಯ
