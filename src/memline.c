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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK(c, s)	ਕਰੋ { /**/ } ਜਦੋਂ (0)

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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਜੇ defined(SASC) || defined(__amigaos4__)
# include <proto/dos.h>	    // for Open() and Close()
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ block0		ZERO_BL;    // contents of the first block
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਸੂਚਕ_ਖੰਡ	PTR_BL;	    // contents of a pointer block
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਅੰਕੜੇ_ਖੰਡ	ਅੰਕੜੇ_BL;    // contents of a data block
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਸੂਚਕ_ਇੰਦਰਾਜ	PTR_EN;	    // block/line-count pair

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕੜੇ_ID	       (('d' << 8) + 'a')   // data block id
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ID	       (('p' << 8) + 't')   // pointer block id
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLOCK0_ID0     'b'		    // block 0 id 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLOCK0_ID1     '0'		    // block 0 id 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLOCK0_ID1_C0  'c'		    // block 0 id 1 'cm' 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLOCK0_ID1_C1  'C'		    // block 0 id 1 'cm' 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLOCK0_ID1_C2  'd'		    // block 0 id 1 'cm' 2
// BLOCK0_ID1_C3 and BLOCK0_ID1_C4 are for libsodium encryption.  However, for
// these the swapfile is disabled, thus they will not be used.  Added for
// consistency anyway.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLOCK0_ID1_C3  'S'		    // block 0 id 1 'cm' 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLOCK0_ID1_C4  's'		    // block 0 id 1 'cm' 4

#ਜੇ defined(FEAT_CRYPT)
ਸਥਿਰ ਅਚਲ ਪੂਰਨ id1_codes[] = {
    BLOCK0_ID1_C0,  // CRYPT_M_ZIP
    BLOCK0_ID1_C1,  // CRYPT_M_BF
    BLOCK0_ID1_C2,  // CRYPT_M_BF2
    BLOCK0_ID1_C3,  // CRYPT_M_SOD  - Unused!
    BLOCK0_ID1_C4,  // CRYPT_M_SOD2  - Unused!
};
#ਜੇ_ਅੰਤ

/*
 * pointer to a block, used in a pointer block
 */
ਬਣਤਰ ਸੂਚਕ_ਇੰਦਰਾਜ
{
    blocknr_T	pe_bnum;	// block number
    linenr_T	pe_ਸਤਰ_ਗਿਣਤੀ;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    ਪੂਰਨ		pe_ਪੰਨਾ_ਗਿਣਤੀ;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
ਬਣਤਰ ਸੂਚਕ_ਖੰਡ
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_ਗਿਣਤੀ;	// number of pointers in this block
    short_u	pb_ਗਿਣਤੀ_max;	// maximum value for pb_count
    PTR_EN	pb_ਸੂਚਕ[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PB_ਗਿਣਤੀ_MAX(mfp) (short_u)(((mfp)->mf_ਪੰਨਾ_ਮਾਪ - offsetof(PTR_BL, pb_ਸੂਚਕ)) / ਆਕਾਰ(PTR_EN))

/*
 * A data block is a leaf in the tree.
 *
 * The text of the lines is at the end of the block. The text of the first line
 * in the block is put at the end, the text of the second line in front of it,
 * etc. Thus the order of the lines is the opposite of the line number.
 */
ਬਣਤਰ ਅੰਕੜੇ_ਖੰਡ
{
    short_u	db_id;		// ID for data block: DATA_ID
    ਬਿਨਾਨਿਸ਼ਾਨੀ	db_free;	// free space available
    ਬਿਨਾਨਿਸ਼ਾਨੀ	db_txt_ਸ਼ੁਰੂ;	// byte where text starts
    ਬਿਨਾਨਿਸ਼ਾਨੀ	db_txt_ਸਿਰਾ;	// byte just after data block
    linenr_T	db_ਸਤਰ_ਗਿਣਤੀ;	// number of lines in this block
    ਬਿਨਾਨਿਸ਼ਾਨੀ	db_ਸੂਚਕਾਂਕ[1];	// index for start of line (actually bigger)
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DB_MARKED	((ਬਿਨਾਨਿਸ਼ਾਨੀ)1 << ((ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ) * 8) - 1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ	(~DB_MARKED)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੂਚਕਾਂਕ_ਮਾਪ  (ਆਕਾਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ))	    // size of one db_index entry
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_ਮਾਪ (offsetof(ਅੰਕੜੇ_BL, db_ਸੂਚਕਾਂਕ))  // size of data block header

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_FNAME_ਮਾਪ_ORG	900	// what it was in older versions
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_FNAME_ਮਾਪ_NOCRYPT	898	// 2 bytes used for other things
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_FNAME_ਮਾਪ_CRYPT	890	// 10 bytes used for other things
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_UNAME_ਮਾਪ		40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_HNAME_ਮਾਪ		40
/*
 * Restrict the numbers to 32 bits, otherwise most compilers will complain.
 * This won't detect a 64 bit machine that only swaps a byte in the top 32
 * bits, but that is crazy anyway.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_MAGIC_LONG	0x30313233L
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_MAGIC_INT	0x20212223L
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_MAGIC_SHORT	0x10111213L
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_MAGIC_CHAR	0x55

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
ਬਣਤਰ block0
{
    char_u	b0_id[2];	// id for block 0: BLOCK0_ID0 and BLOCK0_ID1,
				// BLOCK0_ID1_C0, BLOCK0_ID1_C1, etc.
    char_u	b0_ਸੰਸਕਰਣ[10];	// Vim version string
    char_u	b0_ਪੰਨਾ_ਮਾਪ[4];// number of bytes per page
    char_u	b0_mtime[4];	// last modification time of file
    char_u	b0_ino[4];	// inode of b0_fname
    char_u	b0_pid[4];	// process id of creator (or 0)
    char_u	b0_uname[B0_UNAME_ਮਾਪ]; // name of user (uid if no name)
    char_u	b0_hname[B0_HNAME_ਮਾਪ]; // host name (if it has a name)
    char_u	b0_fname[B0_FNAME_ਮਾਪ_ORG]; // name of file being edited
    ਲੰਮਾ	b0_magic_long;	// check for byte order of long
    ਪੂਰਨ		b0_magic_int;	// check for byte order of int
    ਛੋਟਾ	b0_magic_short;	// check for byte order of short
    char_u	b0_magic_char;	// check for last char
};

/*
 * Note: b0_dirty and b0_flags are put at the end of the file name.  For very
 * long file names in older versions of Vim they are invalid.
 * The 'fileencoding' comes before b0_flags, with a NUL in front.  But only
 * when there is room, for very long file names it's omitted.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_DIRTY	0x55
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ b0_dirty	b0_fname[B0_FNAME_ਮਾਪ_ORG - 1]

/*
 * The b0_flags field is new in Vim 7.0.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ b0_flags	b0_fname[B0_FNAME_ਮਾਪ_ORG - 2]

/*
 * Crypt seed goes here, 8 bytes.  New in Vim 7.3.
 * Without encryption these bytes may be used for 'fenc'.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ b0_ਬੀਜ		b0_fname[B0_FNAME_ਮਾਪ_ORG - 2 - MF_ਬੀਜ_LEN]

// The lowest two bits contain the fileformat.  Zero means it's not set
// (compatible with Vim 6.x), otherwise it's EOL_UNIX + 1, EOL_DOS + 1 or
// EOL_MAC + 1.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_FF_ਨਕਾਬ	3

// Swap file is in directory of edited file.  Used to find the file from
// different mount points.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_SAME_DIR	4

// The 'fileencoding' is at the end of b0_fname[], with a NUL in front of it.
// When empty there is only the NUL.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ B0_HAS_FENC	8

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_INCR	5	// nr of entries added to ml_stack at a time

/*
 * The line number where the first mark may be is remembered.
 * If it is 0 there are no marks at all.
 * (always used for the current buffer only, no buffer change possible while
 * executing a global command).
 */
ਸਥਿਰ linenr_T	ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked = 0;

/*
 * arguments for ml_find_line()
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_DELETE	0x11	    // delete line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_ਪਾਓ	0x12	    // insert line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_ਲੱਭੋ		0x13	    // just find the line
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_FLUSH	0x02	    // flush locked block
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
      UB_FNAME = 0	// update timestamp and filename
    , UB_SAME_DIR       // update the B0_SAME_DIR flag
    , UB_CRYPT		// update crypt key
} upd_block0_T;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
ਸਥਿਰ ਖਾਲੀ ml_ਸੈੱਟ_b0_crypt(buf_T *buf, ZERO_BL *b0p);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ ml_upd_block0(buf_T *buf, upd_block0_T what);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_b0_fname(ZERO_BL *, buf_T *buf);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_b0_dir_flag(ZERO_BL *b0p, buf_T *buf);
ਸਥਿਰ ਖਾਲੀ ਜੋੜੋ_b0_fenc(ZERO_BL *b0p, buf_T *buf);
ਸਥਿਰ time_t swapfile_info(char_u *);
ਸਥਿਰ ਪੂਰਨ recov_file_ਨਾਂ(char_u **, char_u *, ਪੂਰਨ prepend_dot);
ਸਥਿਰ char_u *findswapname(buf_T *, char_u **, char_u *);
ਸਥਿਰ ਖਾਲੀ ml_flush_ਸਤਰ(buf_T *);
ਸਥਿਰ bhdr_T *ml_new_ਅੰਕੜੇ(memfile_T *, ਪੂਰਨ, ਪੂਰਨ);
ਸਥਿਰ bhdr_T *ml_new_ptr(memfile_T *);
ਸਥਿਰ bhdr_T *ml_ਲੱਭੋ_ਸਤਰ(buf_T *, linenr_T, ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ ml_ਜੋੜੋ_ਢੇਰ(buf_T *);
ਸਥਿਰ ਖਾਲੀ ml_lineadd(buf_T *, ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ b0_magic_wrong(ZERO_BL *);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_INODE
ਸਥਿਰ ਪੂਰਨ fnamecmp_ino(char_u *, char_u *, ਲੰਮਾ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ long_ਮੰਜ਼ਿਲ_char(ਲੰਮਾ, char_u *);
ਸਥਿਰ ਲੰਮਾ char_ਮੰਜ਼ਿਲ_long(char_u *);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
ਸਥਿਰ cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T ਵਿੱਥ, ਪੂਰਨ reading);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
ਸਥਿਰ ਖਾਲੀ ml_updatechunk(buf_T *buf, ਲੰਮਾ ਸਤਰ, ਲੰਮਾ len, ਪੂਰਨ updtype);
#ਜੇ_ਅੰਤ

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
ml_open(buf_T *buf)
{
    memfile_T	*mfp;
    bhdr_T	*hp = ਨਿਰਾ;
    ZERO_BL	*b0p;
    PTR_BL	*pp;
    ਅੰਕੜੇ_BL	*dp;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ਢੇਰ_ਮਾਪ = 0; // no stack yet
    buf->b_ml.ml_ਢੇਰ = ਨਿਰਾ;	// no stack yet
    buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;	// nothing in the stack
    buf->b_ml.ml_locked = ਨਿਰਾ;	// no cached block
    buf->b_ml.ml_ਸਤਰ_lnum = 0;	// no cached line
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = ਨਿਰਾ;
    buf->b_ml.ml_usedchunks = 0;
#ਜੇ_ਅੰਤ

    ਜੇ (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    ਜੇ (p_uc && buf->b_p_swf)
	buf->b_may_ਅਦਲਾ_ਬਦਲੀ = true;
    ਨਹੀਂਤਾਂ
	buf->b_may_ਅਦਲਾ_ਬਦਲੀ = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(ਨਿਰਾ, 0);
    ਜੇ (mfp == ਨਿਰਾ)
	ਜਾਓ error;

    buf->b_ml.ml_mfp = mfp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    mfp->mf_ਬਫਰ = buf;
#ਜੇ_ਅੰਤ
    buf->b_ml.ml_flags = ML_ਖਾਲੀ_ਥਾਵਾਂ;
    buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ = 1;

/*
 * fill block0 struct and write page 0
 */
    ਜੇ ((hp = mf_new(mfp, FALSE, 1)) == ਨਿਰਾ)
	ਜਾਓ error;
    ਜੇ (hp->bh_bnum != 0)
    {
	iemsg(e_didnt_get_ਖੰਡ_nr_zero);
	ਜਾਓ error;
    }
    b0p = (ZERO_BL *)(hp->bh_ਅੰਕੜੇ);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (ਲੰਮਾ)B0_MAGIC_LONG;
    b0p->b0_magic_int = (ਪੂਰਨ)B0_MAGIC_INT;
    b0p->b0_magic_short = (ਛੋਟਾ)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_ਸੰਸਕਰਣ, "VIM ", 4);
    STRNCPY(b0p->b0_ਸੰਸਕਰਣ + 4, ਸੰਸਕਰਣ, 6);
    long_ਮੰਜ਼ਿਲ_char((ਲੰਮਾ)mfp->mf_ਪੰਨਾ_ਮਾਪ, b0p->b0_ਪੰਨਾ_ਮਾਪ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    ਜੇ (!buf->b_spell)
#ਜੇ_ਅੰਤ
    {
	b0p->b0_dirty = buf->b_ਬਦਲਿਆ ? B0_DIRTY : 0;
	b0p->b0_flags = get_fileformat(buf) + 1;
	ਸੈੱਟ_b0_fname(b0p, buf);
	(ਖਾਲੀ)get_user_name(b0p->b0_uname, B0_UNAME_ਮਾਪ);
	b0p->b0_uname[B0_UNAME_ਮਾਪ - 1] = NUL;
	mch_get_host_name(b0p->b0_hname, B0_HNAME_ਮਾਪ);
	b0p->b0_hname[B0_HNAME_ਮਾਪ - 1] = NUL;
	long_ਮੰਜ਼ਿਲ_char(mch_get_pid(), b0p->b0_pid);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	ml_ਸੈੱਟ_b0_crypt(buf, b0p);
#ਜੇ_ਅੰਤ
    }

    /*
     * Always sync block number 0 to disk, so we can check the file name in
     * the swap file in findswapname(). Don't do this for a help files or
     * a spell buffer though.
     * Only works when there's a swapfile, otherwise it's done when the file
     * is created.
     */
    mf_put(mfp, hp, TRUE, FALSE);
    ਜੇ (!buf->b_help && !B_SPELL(buf))
	(ਖਾਲੀ)mf_sync(mfp, 0);

    /*
     * Fill in root pointer block and write page 1.
     */
    ਜੇ ((hp = ml_new_ptr(mfp)) == ਨਿਰਾ)
	ਜਾਓ error;
    ਜੇ (hp->bh_bnum != 1)
    {
	iemsg(e_didnt_get_ਖੰਡ_nr_one);
	ਜਾਓ error;
    }
    pp = (PTR_BL *)(hp->bh_ਅੰਕੜੇ);
    pp->pb_ਗਿਣਤੀ = 1;
    pp->pb_ਸੂਚਕ[0].pe_bnum = 2;
    pp->pb_ਸੂਚਕ[0].pe_ਪੰਨਾ_ਗਿਣਤੀ = 1;
    pp->pb_ਸੂਚਕ[0].pe_old_lnum = 1;
    pp->pb_ਸੂਚਕ[0].pe_ਸਤਰ_ਗਿਣਤੀ = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    ਜੇ ((hp = ml_new_ਅੰਕੜੇ(mfp, FALSE, 1)) == ਨਿਰਾ)
	ਜਾਓ error;
    ਜੇ (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_ਖੰਡ_nr_two);
	ਜਾਓ error;
    }

    dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
    dp->db_ਸੂਚਕਾਂਕ[0] = --dp->db_txt_ਸ਼ੁਰੂ;	// at end of block
    dp->db_free -= 1 + ਸੂਚਕਾਂਕ_ਮਾਪ;
    dp->db_ਸਤਰ_ਗਿਣਤੀ = 1;
    *((char_u *)dp + dp->db_txt_ਸ਼ੁਰੂ) = NUL;	// empty line

    ਵਾਪਸ OK;

error:
    ਜੇ (mfp != ਨਿਰਾ)
    {
	ਜੇ (hp)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, TRUE);	    // will also free(mfp->mf_fname)
    }
    buf->b_ml.ml_mfp = ਨਿਰਾ;
    ਵਾਪਸ FAIL;
}

#ਜੇ defined(FEAT_CRYPT)
/*
 * Swapfile encryption is not supported by XChaCha20.  If this crypt method is
 * used then disable the swapfile, to avoid plain text being written to disk,
 * and return TRUE.
 * Otherwise return FALSE.
 */
    ਸਥਿਰ ਪੂਰਨ
crypt_may_close_swapfile(buf_T *buf, char_u *ਕੁੰਜੀ, ਪੂਰਨ ਵਿਧੀ)
{
    ਜੇ (crypt_ਵਿਧੀ_is_sodium(ਵਿਧੀ) && *ਕੁੰਜੀ != NUL)
    {
	mf_close_file(buf, TRUE);
	buf->b_p_swf = FALSE;
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Prepare encryption for "buf" for the current key and method.
 */
    ਸਥਿਰ ਖਾਲੀ
ml_ਸੈੱਟ_mfp_crypt(buf_T *buf)
{
    ਜੇ (*buf->b_p_ਕੁੰਜੀ == NUL)
	ਵਾਪਸ;

    ਪੂਰਨ ਵਿਧੀ_nr = crypt_get_ਵਿਧੀ_nr(buf);

    ਜੇ (ਵਿਧੀ_nr > CRYPT_M_ZIP && ਵਿਧੀ_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_ਬੀਜ(buf->b_ml.ml_mfp->mf_ਬੀਜ, MF_ਬੀਜ_LEN, ਨਿਰਾ, 0);
    }
# ifdef FEAT_SODIUM
    ਨਹੀਂਤਾਂ ਜੇ (crypt_ਵਿਧੀ_is_sodium(ਵਿਧੀ_nr))
	crypt_sodium_randombytes_buf(buf->b_ml.ml_mfp->mf_ਬੀਜ, MF_ਬੀਜ_LEN);
# endif
}

/*
 * Prepare encryption for "buf" with block 0 "b0p".
 * Note: should not be called with libsodium encryption, since xchacha20 does
 * not support swapfile encryption.
 */
    ਸਥਿਰ ਖਾਲੀ
ml_ਸੈੱਟ_b0_crypt(buf_T *buf, ZERO_BL *b0p)
{
    ਜੇ (*buf->b_p_ਕੁੰਜੀ == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ ਵਿਧੀ_nr = crypt_get_ਵਿਧੀ_nr(buf);

	b0p->b0_id[1] = id1_codes[ਵਿਧੀ_nr];
	ਜੇ (ਵਿਧੀ_nr > CRYPT_M_ZIP && ਵਿਧੀ_nr < CRYPT_M_SOD)
	{
	    // Generate a seed and store it in block 0 and in the memfile.
	    sha2_ਬੀਜ(&b0p->b0_ਬੀਜ, MF_ਬੀਜ_LEN, ਨਿਰਾ, 0);
	    mch_memmove(buf->b_ml.ml_mfp->mf_ਬੀਜ, &b0p->b0_ਬੀਜ, MF_ਬੀਜ_LEN);
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
    ਖਾਲੀ
ml_ਸੈੱਟ_crypt_ਕੁੰਜੀ(
    buf_T	*buf,
    char_u	*old_ਕੁੰਜੀ,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    ਪੂਰਨ		ਪੰਨਾ_ਗਿਣਤੀ;
    ਪੂਰਨ		idx;
    ਲੰਮਾ	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    ਅੰਕੜੇ_BL	*dp;
    blocknr_T	bnum;
    ਪੂਰਨ		ਸਿਖਰ;
    ਪੂਰਨ		old_ਵਿਧੀ;

    ਜੇ (mfp == ਨਿਰਾ || mfp->mf_fd < 0)
	ਵਾਪਸ;  // no memfile yet, nothing to do
    old_ਵਿਧੀ = crypt_ਵਿਧੀ_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    ਜੇ (crypt_may_close_swapfile(buf, buf->b_p_ਕੁੰਜੀ, crypt_get_ਵਿਧੀ_nr(buf)))
	ਵਾਪਸ;
# endif

    // First make sure the swapfile is in a consistent state, using the old
    // key and method.
    {
	char_u *new_ਕੁੰਜੀ = buf->b_p_ਕੁੰਜੀ;
	char_u *new_buf_cm = buf->b_p_cm;

	buf->b_p_ਕੁੰਜੀ = old_ਕੁੰਜੀ;
	buf->b_p_cm = old_cm;
	ml_preserve(buf, FALSE);
	buf->b_p_ਕੁੰਜੀ = new_ਕੁੰਜੀ;
	buf->b_p_cm = new_buf_cm;
    }

    // Set the key, method and seed to be used for reading, these must be the
    // old values.
    mfp->mf_old_ਕੁੰਜੀ = old_ਕੁੰਜੀ;
    mfp->mf_old_cm = old_ਵਿਧੀ;
    ਜੇ (old_ਵਿਧੀ > 0 && *old_ਕੁੰਜੀ != NUL)
	mch_memmove(mfp->mf_old_ਬੀਜ, mfp->mf_ਬੀਜ, MF_ਬੀਜ_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    ਜੇ (mfp->mf_infile_ਗਿਣਤੀ > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_ਸਤਰ(buf);		    // flush buffered line
	(ਖਾਲੀ)ml_ਲੱਭੋ_ਸਤਰ(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = ਨਿਰਾ;
	bnum = 1;		// start with block 1
	ਪੰਨਾ_ਗਿਣਤੀ = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;
	VIM_CLEAR(buf->b_ml.ml_ਢੇਰ);
	buf->b_ml.ml_ਢੇਰ_ਮਾਪ = 0;	// no stack yet

	ਲਈ ( ; !got_int; ਸਤਰ_breakcheck())
	{
	    ਜੇ (hp != ਨਿਰਾ)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    ਜੇ ((hp = mf_get(mfp, bnum, ਪੰਨਾ_ਗਿਣਤੀ)) == ਨਿਰਾ)
	    {
		ਜੇ (bnum == 1)
		    ਰੋਕੋ;
		++error;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		pp = (PTR_BL *)(hp->bh_ਅੰਕੜੇ);
		ਜੇ (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    ਜੇ (pp->pb_ਗਿਣਤੀ == 0)
		    {
			// empty block?
			++error;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (idx < (ਪੂਰਨ)pp->pb_ਗਿਣਤੀ)	// go a block deeper
		    {
			ਜੇ (pp->pb_ਸੂਚਕ[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    ਜਾਰੀ;
			}

			// going one block deeper in the tree, new entry in
			//ਢੇਰ
			ਜੇ ((ਸਿਖਰ = ml_ਜੋੜੋ_ਢੇਰ(buf)) < 0)
			{
			    ++error;
			    ਰੋਕੋ;		    // out of memory
			}
			ip = &(buf->b_ml.ml_ਢੇਰ[ਸਿਖਰ]);
			ip->ip_bnum = bnum;
			ip->ip_ਸੂਚਕਾਂਕ = idx;

			bnum = pp->pb_ਸੂਚਕ[idx].pe_bnum;
			ਪੰਨਾ_ਗਿਣਤੀ = pp->pb_ਸੂਚਕ[idx].pe_ਪੰਨਾ_ਗਿਣਤੀ;
			idx = 0;
			ਜਾਰੀ;
		    }
		}
		ਨਹੀਂਤਾਂ	    // not a pointer block
		{
		    dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
		    ਜੇ (dp->db_id != ਅੰਕੜੇ_ID)	// block id wrong
			++error;
		    ਨਹੀਂਤਾਂ
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = ਨਿਰਾ;
		    }
		}
	    }

	    ਜੇ (buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ == 0)	//ਮੁਕੰਮਲ
		ਰੋਕੋ;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_ਢੇਰ[--(buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_ਸੂਚਕਾਂਕ + 1;	    // go to next index
	    ਪੰਨਾ_ਗਿਣਤੀ = 1;
	}
	ਜੇ (hp != ਨਿਰਾ)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	ਜੇ (error > 0)
	    emsg(_(e_error_while_updating_ਅਦਲਾ_ਬਦਲੀ_file_crypt));
    }

    mfp->mf_old_ਕੁੰਜੀ = ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
    ਖਾਲੀ
ml_setname(buf_T *buf)
{
    ਪੂਰਨ		success = FALSE;
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;
#ਜੇ defined(MSWIN)
    char_u	*p;
#ਜੇ_ਅੰਤ

    mfp = buf->b_ml.ml_mfp;
    ਜੇ (mfp->mf_fd < 0)		    // there is no swap file yet
    {
	/*
	 * When 'updatecount' is 0 and 'noswapfile' there is no swap file.
	 * For help files we will make a swap file now.
	 */
	ਜੇ (p_uc != 0 && (cmdmod.cmod_flags & CMOD_NOSWAPFILE) == 0)
	    ml_open_file(buf);	    // create a swap file
	ਵਾਪਸ;
    }

    /*
     * Try all directories in the 'directory' option.
     */
    dirp = p_dir;
    ਲਈ (;;)
    {
	ਜੇ (*dirp == NUL)	    // tried all directories, fail
	    ਰੋਕੋ;
	fname = findswapname(buf, &dirp, mfp->mf_fname);
	// autocmd may have freed mfp if findswapname creates
	// different swapfile name
	ਜੇ (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    ਵਾਪਸ;
	}
						    // alloc's fname
	ਜੇ (dirp == ਨਿਰਾ)	    // out of memory
	    ਰੋਕੋ;
	ਜੇ (fname == ਨਿਰਾ)	    // no file name found for this dir
	    ਜਾਰੀ;

#ਜੇ defined(MSWIN)
	/*
	 * Set full pathname for swap file now, because a ":!cd dir" may
	 * change directory without us knowing it.
	 */
	p = ਭਰਿਆ_Name_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	ਜੇ (fname == ਨਿਰਾ)
	    ਜਾਰੀ;
#ਜੇ_ਅੰਤ
	// if the file name is the same we don't have to do anything
	ਜੇ (fnamecmp(fname, mfp->mf_fname) == 0)
	{
	    vim_free(fname);
	    success = TRUE;
	    ਰੋਕੋ;
	}
	// need to close the swap file before renaming
	ਜੇ (mfp->mf_fd >= 0)
	{
	    close(mfp->mf_fd);
	    mfp->mf_fd = -1;
	}

	// try to rename the swap file
	ਜੇ (vim_rename(mfp->mf_fname, fname) == 0)
	{
	    success = TRUE;
	    vim_free(mfp->mf_fname);
	    mfp->mf_fname = fname;
	    vim_free(mfp->mf_ffname);
#ਜੇ defined(MSWIN)
	    mfp->mf_ffname = ਨਿਰਾ;  // mf_fname is full pathname already
#ਨਹੀਂਤਾਂ
	    mf_ਸੈੱਟ_ffname(mfp);
#ਜੇ_ਅੰਤ
	    ml_upd_block0(buf, UB_SAME_DIR);
	    ਰੋਕੋ;
	}
	vim_free(fname);	    // this fname didn't work, try another
    }

    ਜੇ (mfp->mf_fd == -1)	    // need to (re)open the swap file
    {
	mfp->mf_fd = mch_open((ਅੱਖਰ *)mfp->mf_fname, O_RDWR | O_EXTRA, 0);
	ਜੇ (mfp->mf_fd < 0)
	{
	    // could not (re)open the swap file, what can we do????
	    emsg(_(e_oops_ਗੁਆਚਾ_the_ਅਦਲਾ_ਬਦਲੀ_file));
	    ਵਾਪਸ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FD_CLOEXEC
	{
	    ਪੂਰਨ fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    ਜੇ (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(ਖਾਲੀ)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#ਜੇ_ਅੰਤ
    }
    ਜੇ (!success)
	emsg(_(e_could_not_rename_ਅਦਲਾ_ਬਦਲੀ_file));
}

/*
 * Open a file for the memfile for all buffers that are not readonly or have
 * been modified.
 * Used when 'updatecount' changes from zero to non-zero.
 */
    ਖਾਲੀ
ml_open_files(ਖਾਲੀ)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ਜੇ (!buf->b_p_ro || buf->b_ਬਦਲਿਆ)
	    ml_open_file(buf);
}

/*
 * Open a swap file for an existing memfile, if there is no swap file yet.
 * If we are unable to find a file name, mf_fname will be NULL
 * and the memfile will be in memory only (no recovery possible).
 */
    ਖਾਲੀ
ml_open_file(buf_T *buf)
{
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;

    mfp = buf->b_ml.ml_mfp;
    ਜੇ (mfp == ਨਿਰਾ || mfp->mf_fd >= 0 || !buf->b_p_swf
				      || (cmdmod.cmod_flags & CMOD_NOSWAPFILE))
	ਵਾਪਸ;		// nothing to do

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    // For a spell buffer use a temp file name.
    ਜੇ (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	ਜੇ (fname != ਨਿਰਾ)
	    (ਖਾਲੀ)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_ਅਦਲਾ_ਬਦਲੀ = false;
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    /*
     * Try all directories in 'directory' option.
     */
    dirp = p_dir;
    ਲਈ (;;)
    {
	ਜੇ (*dirp == NUL)
	    ਰੋਕੋ;
	// There is a small chance that between choosing the swap file name
	// and creating it, another Vim creates the file.  In that case the
	// creation will fail and we will use another directory.
	fname = findswapname(buf, &dirp, ਨਿਰਾ); // allocates fname
	// autocmd may have freed mfp, grr!
	ਜੇ (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    ਵਾਪਸ;
	}
	ਜੇ (dirp == ਨਿਰਾ)
	    ਰੋਕੋ;  // out of memory
	ਜੇ (fname == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਜੇ (mf_open_file(mfp, fname) == OK)	// consumes fname!
	{
	    // don't sync yet in ml_sync_all()
	    mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;
#ਜੇ defined(MSWIN)
	    /*
	     * set full pathname for swap file now, because a ":!cd dir" may
	     * change directory without us knowing it.
	     */
	    mf_fullname(mfp);
#ਜੇ_ਅੰਤ
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    ਜੇ (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_ਸੈੱਟ_dirty(mfp);
		ਰੋਕੋ;
	    }
	    // Writing block 0 failed: close the file and try another dir
	    mf_close_file(buf, FALSE);
	}
    }

    ਜੇ (*p_dir != NUL && mfp->mf_fname == ਨਿਰਾ)
    {
	need_wait_return = TRUE;	// call wait_return() later
	++no_wait_return;
	(ਖਾਲੀ)semsg(_(e_unable_ਮੰਜ਼ਿਲ_open_ਅਦਲਾ_ਬਦਲੀ_file_for_str_recovery_impossible),
		    buf_spname(buf) != ਨਿਰਾ ? buf_spname(buf) : buf->b_fname);
	--no_wait_return;
    }

    // don't try to open a swap file again
    buf->b_may_ਅਦਲਾ_ਬਦਲੀ = false;
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
    ਖਾਲੀ
check_need_ਅਦਲਾ_ਬਦਲੀ(
    ਪੂਰਨ	    newfile)		// reading file into new buffer
{
    ਪੂਰਨ old_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent; // might be reset by an E325 message

    ਜੇ (curbuf->b_may_ਅਦਲਾ_ਬਦਲੀ && (!curbuf->b_p_ro || !newfile))
	ml_open_file(curbuf);
    ਸੁਨੇਹਾ_silent = old_ਸੁਨੇਹਾ_silent;
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
    ਖਾਲੀ
ml_close(buf_T *buf, ਪੂਰਨ del_file)
{
    ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)		// not open
	ਵਾਪਸ;
    mf_close(buf->b_ml.ml_mfp, del_file);	// close the .swp file
    ਜੇ (buf->b_ml.ml_ਸਤਰ_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_ਸਤਰ_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_ਸਤਰ_ptr);
    vim_free(buf->b_ml.ml_ਢੇਰ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
    VIM_CLEAR(buf->b_ml.ml_chunksize);
#ਜੇ_ਅੰਤ
    buf->b_ml.ml_mfp = ਨਿਰਾ;

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
    ਖਾਲੀ
ml_close_all(ਪੂਰਨ del_file)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ml_close(buf, del_file && ((buf->b_flags & BF_PRESERVED) == 0
				 || vim_strchr(p_cpo, CPO_PRESERVE) == ਨਿਰਾ));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    spell_delete_wordlist();	// delete the internal wordlist
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TEMPDIRNAMES
    vim_deltempdir();		// delete created temp directory
#ਜੇ_ਅੰਤ
}

/*
 * Close all memfiles for not modified buffers.
 * Only use just before exiting!
 */
    ਖਾਲੀ
ml_close_notmod(ਖਾਲੀ)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ਜੇ (!buf_Is_ਬਦਲਿਆ(buf))
	    ml_close(buf, TRUE);    // close all not-modified buffers
}

/*
 * Update the timestamp in the .swp file.
 * Used when the file has been written.
 */
    ਖਾਲੀ
ml_timestamp(buf_T *buf)
{
    ml_upd_block0(buf, UB_FNAME);
}

/*
 * Return FAIL when the ID of "b0p" is wrong.
 */
    ਸਥਿਰ ਪੂਰਨ
ml_check_b0_id(ZERO_BL *b0p)
{
    ਜੇ (b0p->b0_id[0] != BLOCK0_ID0
	    || (b0p->b0_id[1] != BLOCK0_ID1
		&& b0p->b0_id[1] != BLOCK0_ID1_C0
		&& b0p->b0_id[1] != BLOCK0_ID1_C1
		&& b0p->b0_id[1] != BLOCK0_ID1_C2
		&& b0p->b0_id[1] != BLOCK0_ID1_C3
		&& b0p->b0_id[1] != BLOCK0_ID1_C4)
	    )
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Update the timestamp or the B0_SAME_DIR flag of the .swp file.
 */
    ਸਥਿਰ ਖਾਲੀ
ml_upd_block0(buf_T *buf, upd_block0_T what)
{
    memfile_T	*mfp;
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    mfp = buf->b_ml.ml_mfp;
    ਜੇ (mfp == ਨਿਰਾ)
	ਵਾਪਸ;
    hp = mf_get(mfp, (blocknr_T)0, 1);
    ਜੇ (hp == ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	ਜੇ (what == UB_CRYPT)
	    ml_ਸੈੱਟ_mfp_crypt(buf);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ;
    }

    b0p = (ZERO_BL *)(hp->bh_ਅੰਕੜੇ);
    ਜੇ (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_ਖੰਡ_zero);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (what == UB_FNAME)
	    ਸੈੱਟ_b0_fname(b0p, buf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	ਨਹੀਂਤਾਂ ਜੇ (what == UB_CRYPT)
	    ml_ਸੈੱਟ_b0_crypt(buf, b0p);
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ // what == UB_SAME_DIR
	    ਸੈੱਟ_b0_dir_flag(b0p, buf);
    }
    mf_put(mfp, hp, TRUE, FALSE);
}

/*
 * Write file name and timestamp into block 0 of a swap file.
 * Also set buf->b_mtime.
 * Don't use NameBuff[]!!!
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_b0_fname(ZERO_BL *b0p, buf_T *buf)
{
    stat_T	st;

    ਜੇ (buf->b_ffname == ਨਿਰਾ)
	b0p->b0_fname[0] = NUL;
    ਨਹੀਂਤਾਂ
    {
#ਜੇ defined(MSWIN) || defined(AMIGA)
	// Systems that cannot translate "~user" back into a path: copy the
	// file name unmodified.  Do use slashes instead of backslashes for
	// portability.
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_ਮਾਪ_CRYPT - 1);
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#ਨਹੀਂਤਾਂ
	size_t	flen;
	char_u	uname[B0_UNAME_ਮਾਪ];

	/*
	 * For a file under the home directory of the current user, we try to
	 * replace the home directory path with "~user". This helps when
	 * editing the same file on different machines over a network.
	 * First replace home dir path with "~/" with home_replace().
	 * Then insert the user name to get "~user/".
	 */
	flen = home_replace(ਨਿਰਾ, buf->b_ffname, b0p->b0_fname,
						   B0_FNAME_ਮਾਪ_CRYPT, TRUE);
	ਜੇ (b0p->b0_fname[0] == '~')
	{
	    size_t  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    ਜੇ (get_user_name(uname, B0_UNAME_ਮਾਪ) == FAIL
		   || (ulen = STRLEN(uname)) + flen > B0_FNAME_ਮਾਪ_CRYPT - 1)
		vim_strncpy(b0p->b0_fname, buf->b_ffname,
						     B0_FNAME_ਮਾਪ_CRYPT - 1);
	    ਨਹੀਂਤਾਂ
	    {
		mch_memmove(b0p->b0_fname + ulen + 1, b0p->b0_fname + 1, flen);
		mch_memmove(b0p->b0_fname + 1, uname, ulen);
	    }
	}
#ਜੇ_ਅੰਤ
	ਜੇ (mch_stat((ਅੱਖਰ *)buf->b_ffname, &st) >= 0)
	{
	    long_ਮੰਜ਼ਿਲ_char((ਲੰਮਾ)st.st_mtime, b0p->b0_mtime);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_INODE
	    long_ਮੰਜ਼ਿਲ_char((ਲੰਮਾ)st.st_ino, b0p->b0_ino);
#ਜੇ_ਅੰਤ
	    buf_store_ਸਮਾਂ(buf, &st, buf->b_ffname);
	    buf->b_mtime_read = buf->b_mtime;
	    buf->b_mtime_read_ns = buf->b_mtime_ns;
	}
	ਨਹੀਂਤਾਂ
	{
	    long_ਮੰਜ਼ਿਲ_char(0L, b0p->b0_mtime);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_INODE
	    long_ਮੰਜ਼ਿਲ_char(0L, b0p->b0_ino);
#ਜੇ_ਅੰਤ
	    buf->b_mtime = 0;
	    buf->b_mtime_ns = 0;
	    buf->b_mtime_read = 0;
	    buf->b_mtime_read_ns = 0;
	    buf->b_orig_ਮਾਪ = 0;
	    buf->b_orig_ਬਹੁਲਕ = 0;
	}
    }

    // Also add the 'fileencoding' if there is room.
    ਜੋੜੋ_b0_fenc(b0p, curbuf);
}

/*
 * Update the B0_SAME_DIR flag of the swap file.  It's set if the file and the
 * swapfile for "buf" are in the same directory.
 * This is fail safe: if we are not sure the directories are equal the flag is
 * not set.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_b0_dir_flag(ZERO_BL *b0p, buf_T *buf)
{
    ਜੇ (same_directory(buf->b_ml.ml_mfp->mf_fname, buf->b_ffname))
	b0p->b0_flags |= B0_SAME_DIR;
    ਨਹੀਂਤਾਂ
	b0p->b0_flags &= ~B0_SAME_DIR;
}

/*
 * When there is room, add the 'fileencoding' to block zero.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_b0_fenc(
    ZERO_BL	*b0p,
    buf_T	*buf)
{
    ਪੂਰਨ		n;
    ਪੂਰਨ		ਮਾਪ = B0_FNAME_ਮਾਪ_NOCRYPT;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    ਜੇ (*buf->b_p_ਕੁੰਜੀ != NUL)
	ਮਾਪ = B0_FNAME_ਮਾਪ_CRYPT;
#ਜੇ_ਅੰਤ

    n = (ਪੂਰਨ)STRLEN(buf->b_p_fenc);
    ਜੇ ((ਪੂਰਨ)STRLEN(b0p->b0_fname) + n + 1 > ਮਾਪ)
	b0p->b0_flags &= ~B0_HAS_FENC;
    ਨਹੀਂਤਾਂ
    {
	mch_memmove((ਅੱਖਰ *)b0p->b0_fname + ਮਾਪ - n,
					    (ਅੱਖਰ *)buf->b_p_fenc, (size_t)n);
	*(b0p->b0_fname + ਮਾਪ - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#ਜੇ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    ਸਥਿਰ ਪੂਰਨ
swapfile_process_running(ZERO_BL *b0p, char_u *ਅਦਲਾ_ਬਦਲੀ_fname ਅਣਵਰਤਿਆ)
{
# ਜੇ defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    ਬਣਤਰ sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    ਜੇ (mch_stat((ਅੱਖਰ *)ਅਦਲਾ_ਬਦਲੀ_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < ਸਮਾਂ(ਨਿਰਾ) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	ਵਾਪਸ FALSE;
# endif
    ਵਾਪਸ mch_process_running(char_ਮੰਜ਼ਿਲ_long(b0p->b0_pid));
}
#ਜੇ_ਅੰਤ

/*
 * Try to recover curbuf from the .swp file.
 * If "checkext" is TRUE, check the extension and detect whether it is
 * a swap file.
 */
    ਖਾਲੀ
ml_recover(ਪੂਰਨ checkext)
{
    buf_T	*buf = ਨਿਰਾ;
    memfile_T	*mfp = ਨਿਰਾ;
    char_u	*fname;
    char_u	*fname_ਵਰਤਿਆ = ਨਿਰਾ;
    bhdr_T	*hp = ਨਿਰਾ;
    ZERO_BL	*b0p;
    ਪੂਰਨ		b0_ff;
    char_u	*b0_fenc = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    ਪੂਰਨ		b0_cm = -1;
#ਜੇ_ਅੰਤ
    PTR_BL	*pp;
    ਅੰਕੜੇ_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    ਪੂਰਨ		ਪੰਨਾ_ਗਿਣਤੀ;
    stat_T	org_stat, swp_stat;
    ਪੂਰਨ		len;
    ਪੂਰਨ		directly;
    linenr_T	lnum;
    char_u	*p;
    ਪੂਰਨ		i;
    ਲੰਮਾ	error;
    ਪੂਰਨ		cannot_open;
    linenr_T	ਸਤਰ_ਗਿਣਤੀ;
    ਪੂਰਨ		has_error;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		ਸਿਖਰ;
    ਪੂਰਨ		txt_ਸ਼ੁਰੂ;
    off_T	ਮਾਪ;
    ਪੂਰਨ		called_from_main;
    ਪੂਰਨ		serious_error = TRUE;
    ਲੰਮਾ	mtime;
    ਪੂਰਨ		attr;
    ਪੂਰਨ		orig_file_ਸਥਿਤੀ = NOTDONE;

    recoverymode = TRUE;
    called_from_main = (curbuf->b_ml.ml_mfp == ਨਿਰਾ);
    attr = HL_ATTR(HLF_E);

    /*
     * If the file name ends in ".s[a-w][a-z]" we assume this is the swap file.
     * Otherwise a search is done to find the swap file(s).
     */
    fname = curbuf->b_fname;
    ਜੇ (fname == ਨਿਰਾ)		    // When there is no file name
	fname = (char_u *)"";
    len = (ਪੂਰਨ)STRLEN(fname);
    ਜੇ (checkext && len >= 4 &&
#ਜੇ defined(VMS)
	    STRNICMP(fname + len - 4, "_s", 2)
#ਨਹੀਂਤਾਂ
	    STRNICMP(fname + len - 4, ".s", 2)
#ਜੇ_ਅੰਤ
						== 0
		&& vim_strchr((char_u *)"abcdefghijklmnopqrstuvw",
					   TOLOWER_ASC(fname[len - 2])) != ਨਿਰਾ
		&& ASCII_ISALPHA(fname[len - 1]))
    {
	directly = TRUE;
	fname_ਵਰਤਿਆ = vim_strsave(fname); // make a copy for mf_open()
    }
    ਨਹੀਂਤਾਂ
    {
	directly = FALSE;

	// count the number of matching swap files
	len = recover_ਨਾਂ(fname, FALSE, ਨਿਰਾ, 0, ਨਿਰਾ);
	ਜੇ (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_ਅਦਲਾ_ਬਦਲੀ_file_ਮਿਲਿਆ_for_str), fname);
	    ਜਾਓ theend;
	}
	ਜੇ (len == 1)		    // one swap file found, use it
	    i = 1;
	ਨਹੀਂਤਾਂ			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (ਖਾਲੀ)recover_ਨਾਂ(fname, TRUE, ਨਿਰਾ, 0, ਨਿਰਾ);
	    ਸੁਨੇਹਾ_putchar('\n');
	    ਸੁਨੇਹਾ_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_ਸੰਖਿਆ(FALSE, ਨਿਰਾ);
	    ਜੇ (i < 1 || i > len)
		ਜਾਓ theend;
	}
	// get the swap file name that will be used
	(ਖਾਲੀ)recover_ਨਾਂ(fname, FALSE, ਨਿਰਾ, i, &fname_ਵਰਤਿਆ);
    }
    ਜੇ (fname_ਵਰਤਿਆ == ਨਿਰਾ)
	ਜਾਓ theend;			// out of memory

    // When called from main() still need to initialize storage structure
    ਜੇ (called_from_main && ml_open(curbuf) == FAIL)
	getout(1);

    /*
     * Allocate a buffer structure for the swap file that is used for recovery.
     * Only the memline and crypt information in it are really used.
     */
    buf = ALLOC_ONE(buf_T);
    ਜੇ (buf == ਨਿਰਾ)
	ਜਾਓ theend;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ਢੇਰ_ਮਾਪ = 0;	// no stack yet
    buf->b_ml.ml_ਢੇਰ = ਨਿਰਾ;		// no stack yet
    buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;		// nothing in the stack
    buf->b_ml.ml_ਸਤਰ_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = ਨਿਰਾ;		// no locked block
    buf->b_ml.ml_flags = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    buf->b_p_ਕੁੰਜੀ = ਖਾਲੀ_ਥਾਵਾਂ_option;
    buf->b_p_cm = ਖਾਲੀ_ਥਾਵਾਂ_option;
#ਜੇ_ਅੰਤ

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_ਵਰਤਿਆ); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_ਵਰਤਿਆ, O_RDONLY);
    fname_ਵਰਤਿਆ = p;
    ਜੇ (mfp == ਨਿਰਾ || mfp->mf_fd < 0)
    {
	ਜੇ (fname_ਵਰਤਿਆ != ਨਿਰਾ)
	    semsg(_(e_cannot_open_str), fname_ਵਰਤਿਆ);
	ਜਾਓ theend;
    }
    buf->b_ml.ml_mfp = mfp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    mfp->mf_ਬਫਰ = buf;
#ਜੇ_ਅੰਤ

    /*
     * The page size set in mf_open() might be different from the page size
     * used in the swap file, we must get it from block 0.  But to read block
     * 0 we need a page size.  Use the minimal size for block 0 here, it will
     * be set to the real value below.
     */
    mfp->mf_ਪੰਨਾ_ਮਾਪ = MIN_ਅਦਲਾ_ਬਦਲੀ_ਪੰਨਾ_ਮਾਪ;

    /*
     * try to read block 0
     */
    ਜੇ ((hp = mf_get(mfp, (blocknr_T)0, 1)) == ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	ਸੁਨੇਹਾ_puts_attr(_("Unable to read block 0 from "), attr | ਸੁਨੇਹਾ_HIST);
	ਸੁਨੇਹਾ_outtrans_attr(mfp->mf_fname, attr | ਸੁਨੇਹਾ_HIST);
	ਸੁਨੇਹਾ_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | ਸੁਨੇਹਾ_HIST);
	ਸੁਨੇਹਾ_ਸਿਰਾ();
	ਜਾਓ theend;
    }
    b0p = (ZERO_BL *)(hp->bh_ਅੰਕੜੇ);
    ਜੇ (STRNCMP(b0p->b0_ਸੰਸਕਰਣ, "VIM 3.0", 7) == 0)
    {
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	ਸੁਨੇਹਾ_outtrans_attr(mfp->mf_fname, ਸੁਨੇਹਾ_HIST);
	ਸੁਨੇਹਾ_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    ਸੁਨੇਹਾ_HIST);
	ਸੁਨੇਹਾ_puts_attr(_("Use Vim version 3.0.\n"), ਸੁਨੇਹਾ_HIST);
	ਸੁਨੇਹਾ_ਸਿਰਾ();
	ਜਾਓ theend;
    }
    ਜੇ (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_ਅਦਲਾ_ਬਦਲੀ_file), mfp->mf_fname);
	ਜਾਓ theend;
    }
    ਜੇ (b0_magic_wrong(b0p))
    {
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	ਸੁਨੇਹਾ_outtrans_attr(mfp->mf_fname, attr | ਸੁਨੇਹਾ_HIST);
#ਜੇ defined(MSWIN)
	ਜੇ (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    ਸੁਨੇਹਾ_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | ਸੁਨੇਹਾ_HIST);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਸੁਨੇਹਾ_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | ਸੁਨੇਹਾ_HIST);
	ਸੁਨੇਹਾ_puts_attr(_("The file was created on "), attr | ਸੁਨੇਹਾ_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	ਸੁਨੇਹਾ_puts_attr((ਅੱਖਰ *)b0p->b0_hname, attr | ਸੁਨੇਹਾ_HIST);
	ਸੁਨੇਹਾ_puts_attr(_(",\nor the file has been damaged."), attr | ਸੁਨੇਹਾ_HIST);
	ਸੁਨੇਹਾ_ਸਿਰਾ();
	ਜਾਓ theend;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(id1_codes); ++i)
	ਜੇ (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    ਜੇ (b0_cm > 0)
	mch_memmove(mfp->mf_ਬੀਜ, &b0p->b0_ਬੀਜ, MF_ਬੀਜ_LEN);
    crypt_ਸੈੱਟ_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#ਨਹੀਂਤਾਂ
    ਜੇ (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_ਸੰਸਕਰਣ_ਦਾ_vim_does_not_support_encryption), mfp->mf_fname);
	ਜਾਓ theend;
    }
#ਜੇ_ਅੰਤ

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    ਜੇ (mfp->mf_ਪੰਨਾ_ਮਾਪ != (ਬਿਨਾਨਿਸ਼ਾਨੀ)char_ਮੰਜ਼ਿਲ_long(b0p->b0_ਪੰਨਾ_ਮਾਪ))
    {
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪਿਛਲਾ_ਪੰਨਾ_ਮਾਪ = mfp->mf_ਪੰਨਾ_ਮਾਪ;

	mf_new_ਪੰਨਾ_ਮਾਪ(mfp, (ਬਿਨਾਨਿਸ਼ਾਨੀ)char_ਮੰਜ਼ਿਲ_long(b0p->b0_ਪੰਨਾ_ਮਾਪ));
	ਜੇ (mfp->mf_ਪੰਨਾ_ਮਾਪ < ਪਿਛਲਾ_ਪੰਨਾ_ਮਾਪ)
	{
	    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	    ਸੁਨੇਹਾ_outtrans_attr(mfp->mf_fname, attr | ਸੁਨੇਹਾ_HIST);
	    ਸੁਨੇਹਾ_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | ਸੁਨੇਹਾ_HIST);
	    ਸੁਨੇਹਾ_ਸਿਰਾ();
	    ਜਾਓ theend;
	}
	ਜੇ ((ਮਾਪ = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	ਨਹੀਂਤਾਂ
	    mfp->mf_blocknr_max = (blocknr_T)(ਮਾਪ / mfp->mf_ਪੰਨਾ_ਮਾਪ);
	mfp->mf_infile_ਗਿਣਤੀ = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_ਪੰਨਾ_ਮਾਪ);
	ਜੇ (p == ਨਿਰਾ)
	    ਜਾਓ theend;
	mch_memmove(p, hp->bh_ਅੰਕੜੇ, ਪਿਛਲਾ_ਪੰਨਾ_ਮਾਪ);
	vim_free(hp->bh_ਅੰਕੜੇ);
	hp->bh_ਅੰਕੜੇ = p;
	b0p = (ZERO_BL *)(hp->bh_ਅੰਕੜੇ);
    }

    /*
     * If .swp file name given directly, use name from swap file for buffer.
     */
    ਜੇ (directly)
    {
	expand_env(b0p->b0_fname, NameBuff, MAXPATHL);
	ਜੇ (setfname(curbuf, NameBuff, ਨਿਰਾ, TRUE) == FAIL)
	    ਜਾਓ theend;
    }

    home_replace(ਨਿਰਾ, mfp->mf_fname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Using swap file \"%s\""), NameBuff);

    ਜੇ (buf_spname(curbuf) != ਨਿਰਾ)
	vim_strncpy(NameBuff, buf_spname(curbuf), MAXPATHL - 1);
    ਨਹੀਂਤਾਂ
	home_replace(ਨਿਰਾ, curbuf->b_ffname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Original file \"%s\""), NameBuff);
    ਸੁਨੇਹਾ_putchar('\n');

    /*
     * check date of swap file and original file
     */
    mtime = char_ਮੰਜ਼ਿਲ_long(b0p->b0_mtime);
    ਜੇ (curbuf->b_ffname != ਨਿਰਾ
	    && mch_stat((ਅੱਖਰ *)curbuf->b_ffname, &org_stat) != -1
	    && ((mch_stat((ਅੱਖਰ *)mfp->mf_fname, &swp_stat) != -1
		    && org_stat.st_mtime > swp_stat.st_mtime)
		|| org_stat.st_mtime != mtime))
	emsg(_(e_warning_original_file_may_have_been_ਬਦਲਿਆ));
    ਬਾਹਰਲਾ_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_ਨਕਾਬ);
    ਜੇ (b0p->b0_flags & B0_HAS_FENC)
    {
	ਪੂਰਨ fnsize = B0_FNAME_ਮਾਪ_NOCRYPT;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	ਜੇ (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_ਮਾਪ_CRYPT;
#ਜੇ_ਅੰਤ
	ਲਈ (p = b0p->b0_fname + fnsize; p > b0p->b0_fname && p[-1] != NUL; --p)
	    ;
	b0_fenc = vim_strnsave(p, b0p->b0_fname + fnsize - p);
    }

    mf_put(mfp, hp, FALSE, FALSE);	// release block 0
    hp = ਨਿਰਾ;

    /*
     * Now that we are sure that the file is going to be recovered, clear the
     * contents of the current buffer.
     */
    ਜਦੋਂ (!(curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    ਜੇ (curbuf->b_ffname != ਨਿਰਾ)
	orig_file_ਸਥਿਤੀ = readfile(curbuf->b_ffname, ਨਿਰਾ, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, ਨਿਰਾ, READ_NEW);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    ਜੇ (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	ਜੇ (*curbuf->b_p_ਕੁੰਜੀ != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_ਵਰਤਿਆ);
	    ਸੁਨੇਹਾ_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    ਸੁਨੇਹਾ_puts(_("\nenter the new crypt key."));
	    ਸੁਨੇਹਾ_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    ਸੁਨੇਹਾ_puts(_("\nto use the same key for text file and swap file"));
	}
	ਨਹੀਂਤਾਂ
	    smsg(_(need_ਕੁੰਜੀ_ਸੁਨੇਹਾ), fname_ਵਰਤਿਆ);
	buf->b_p_ਕੁੰਜੀ = crypt_get_ਕੁੰਜੀ(FALSE, FALSE);
	ਜੇ (buf->b_p_ਕੁੰਜੀ == ਨਿਰਾ)
	    buf->b_p_ਕੁੰਜੀ = curbuf->b_p_ਕੁੰਜੀ;
	ਨਹੀਂਤਾਂ ਜੇ (*buf->b_p_ਕੁੰਜੀ == NUL)
	{
	    vim_free(buf->b_p_ਕੁੰਜੀ);
	    buf->b_p_ਕੁੰਜੀ = curbuf->b_p_ਕੁੰਜੀ;
	}
	ਜੇ (buf->b_p_ਕੁੰਜੀ == ਨਿਰਾ)
	    buf->b_p_ਕੁੰਜੀ = ਖਾਲੀ_ਥਾਵਾਂ_option;
    }
#ਜੇ_ਅੰਤ

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    ਜੇ (b0_ff != 0)
	ਸੈੱਟ_fileformat(b0_ff - 1, OPT_LOCAL);
    ਜੇ (b0_fenc != ਨਿਰਾ)
    {
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    ਪੰਨਾ_ਗਿਣਤੀ = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    ਸਤਰ_ਗਿਣਤੀ = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;
    buf->b_ml.ml_ਢੇਰ = ਨਿਰਾ;
    buf->b_ml.ml_ਢੇਰ_ਮਾਪ = 0;	// no stack yet

    ਜੇ (curbuf->b_ffname == ਨਿਰਾ)
	cannot_open = TRUE;
    ਨਹੀਂਤਾਂ
	cannot_open = FALSE;

    serious_error = FALSE;
    ਲਈ ( ; !got_int; ਸਤਰ_breakcheck())
    {
	ਜੇ (hp != ਨਿਰਾ)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	ਜੇ ((hp = mf_get(mfp, bnum, ਪੰਨਾ_ਗਿਣਤੀ)) == ਨਿਰਾ)
	{
	    ਜੇ (bnum == 1)
	    {
		semsg(_(e_unable_ਮੰਜ਼ਿਲ_read_ਖੰਡ_one_from_str), mfp->mf_fname);
		ਜਾਓ theend;
	    }
	    ++error;
	    ml_append(lnum++, (char_u *)_("???MANY LINES MISSING"),
							    (colnr_T)0, TRUE);
	}
	ਨਹੀਂਤਾਂ		// there is a block
	{
	    pp = (PTR_BL *)(hp->bh_ਅੰਕੜੇ);
	    ਜੇ (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		ਪੂਰਨ ptr_ਖੰਡ_error = FALSE;
		ਜੇ (pp->pb_ਗਿਣਤੀ_max != PB_ਗਿਣਤੀ_MAX(mfp))
		{
		    ptr_ਖੰਡ_error = TRUE;
		    pp->pb_ਗਿਣਤੀ_max = PB_ਗਿਣਤੀ_MAX(mfp);
		}
		ਜੇ (pp->pb_ਗਿਣਤੀ > pp->pb_ਗਿਣਤੀ_max)
		{
		    ptr_ਖੰਡ_error = TRUE;
		    pp->pb_ਗਿਣਤੀ = pp->pb_ਗਿਣਤੀ_max;
		}
		ਜੇ (ptr_ਖੰਡ_error)
		    emsg(_(e_warning_ਸੂਚਕ_ਖੰਡ_corrupted));

		// check line count when using pointer block first time
		ਜੇ (idx == 0 && ਸਤਰ_ਗਿਣਤੀ != 0)
		{
		    ਲਈ (i = 0; i < (ਪੂਰਨ)pp->pb_ਗਿਣਤੀ; ++i)
			ਸਤਰ_ਗਿਣਤੀ -= pp->pb_ਸੂਚਕ[i].pe_ਸਤਰ_ਗਿਣਤੀ;
		    ਜੇ (ਸਤਰ_ਗਿਣਤੀ != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		ਜੇ (pp->pb_ਗਿਣਤੀ == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		ਨਹੀਂਤਾਂ ਜੇ (idx < (ਪੂਰਨ)pp->pb_ਗਿਣਤੀ)	// go a block deeper
		{
		    ਜੇ (pp->pb_ਸੂਚਕ[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			ਜੇ (!cannot_open)
			{
			    ਸਤਰ_ਗਿਣਤੀ = pp->pb_ਸੂਚਕ[idx].pe_ਸਤਰ_ਗਿਣਤੀ;
			    linenr_T pe_old_lnum = pp->pb_ਸੂਚਕ[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    ਜੇ (ਸਤਰ_ਗਿਣਤੀ <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, ਨਿਰਾ, lnum,
					pe_old_lnum - 1,
					ਸਤਰ_ਗਿਣਤੀ, ਨਿਰਾ, 0) != OK)
				cannot_open = TRUE;
			    ਨਹੀਂਤਾਂ
				lnum += ਸਤਰ_ਗਿਣਤੀ;
			}
			ਜੇ (cannot_open)
			{
			    ++error;
			    ml_append(lnum++, (char_u *)_("???LINES MISSING"),
							    (colnr_T)0, TRUE);
			}
			++idx;	    // get same block again for next index
			ਜਾਰੀ;
		    }

		    /*
		     * going one block deeper in the tree
		     */
		    ਜੇ ((ਸਿਖਰ = ml_ਜੋੜੋ_ਢੇਰ(buf)) < 0)	// new entry in stack
		    {
			++error;
			ਰੋਕੋ;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_ਢੇਰ[ਸਿਖਰ]);
		    ip->ip_bnum = bnum;
		    ip->ip_ਸੂਚਕਾਂਕ = idx;

		    bnum = pp->pb_ਸੂਚਕ[idx].pe_bnum;
		    ਸਤਰ_ਗਿਣਤੀ = pp->pb_ਸੂਚਕ[idx].pe_ਸਤਰ_ਗਿਣਤੀ;
		    ਪੰਨਾ_ਗਿਣਤੀ = pp->pb_ਸੂਚਕ[idx].pe_ਪੰਨਾ_ਗਿਣਤੀ;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    ਜੇ (ਪੰਨਾ_ਗਿਣਤੀ < 1
			    || bnum + ਪੰਨਾ_ਗਿਣਤੀ > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_ਸੂਚਕਾਂਕ + 1;
			bnum = ip->ip_bnum;
			ਪੰਨਾ_ਗਿਣਤੀ = 1;
			--buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ;
			ਜਾਰੀ;
		    }
		    idx = 0;
		    ਜਾਰੀ;
		}
	    }
	    ਨਹੀਂਤਾਂ	    // not a pointer block
	    {
		dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
		ਜੇ (dp->db_id != ਅੰਕੜੇ_ID)	// block id wrong
		{
		    ਜੇ (bnum == 1)
		    {
			semsg(_(e_ਖੰਡ_one_id_wrong_str_not_swp_file),
							       mfp->mf_fname);
			ਜਾਓ theend;
		    }
		    ++error;
		    ml_append(lnum++, (char_u *)_("???BLOCK MISSING"),
							    (colnr_T)0, TRUE);
		}
		ਨਹੀਂਤਾਂ
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
		    ਜੇ (hp->bh_ਪੰਨਾ_ਗਿਣਤੀ != ਪੰਨਾ_ਗਿਣਤੀ)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			ਪੰਨਾ_ਗਿਣਤੀ = hp->bh_ਪੰਨਾ_ਗਿਣਤੀ;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    ਜੇ (ਪੰਨਾ_ਗਿਣਤੀ * mfp->mf_ਪੰਨਾ_ਮਾਪ != dp->db_txt_ਸਿਰਾ)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_ਸਿਰਾ = ਪੰਨਾ_ਗਿਣਤੀ * mfp->mf_ਪੰਨਾ_ਮਾਪ;
		    }

		    ਜੇ (dp->db_txt_ਸ਼ੁਰੂ < ਸਿਰਲੇਖ_ਮਾਪ
			    || dp->db_txt_ਸ਼ੁਰੂ > dp->db_txt_ਸਿਰਾ)
		    {
			ml_append(lnum++, (char_u *)_("??? block header corrupted"),
								    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_ਸ਼ੁਰੂ = dp->db_txt_ਸਿਰਾ;
		    }
		    // Make sure there is a NUL at the end of the block so we
		    // don't go over the end when copying text.
		    *((char_u *)dp + dp->db_txt_ਸਿਰਾ - 1) = NUL;

		    // Check the number of lines in the block.
		    // If wrong, use the count in the data block.
		    ਜੇ (ਸਤਰ_ਗਿਣਤੀ != dp->db_ਸਤਰ_ਗਿਣਤੀ)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    ਪੂਰਨ did_questions = FALSE;
		    ਲਈ (i = 0; i < dp->db_ਸਤਰ_ਗਿਣਤੀ; ++i)
		    {
			ਜੇ ((char_u *)&(dp->db_ਸੂਚਕਾਂਕ[i])
					    >= (char_u *)dp + dp->db_txt_ਸ਼ੁਰੂ)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    ਰੋਕੋ;
			}

			txt_ਸ਼ੁਰੂ = (dp->db_ਸੂਚਕਾਂਕ[i] & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
			ਜੇ (txt_ਸ਼ੁਰੂ <= (ਪੂਰਨ)ਸਿਰਲੇਖ_ਮਾਪ
					  || txt_ਸ਼ੁਰੂ >= (ਪੂਰਨ)dp->db_txt_ਸਿਰਾ)
			{
			    ++error;
			    // avoid lots of lines with "???"
			    ਜੇ (did_questions)
				ਜਾਰੀ;
			    did_questions = TRUE;
			    p = (char_u *)"???";
			}
			ਨਹੀਂਤਾਂ
			{
			    did_questions = FALSE;
			    p = (char_u *)dp + txt_ਸ਼ੁਰੂ;
			}
			ml_append(lnum++, p, (colnr_T)0, TRUE);
		    }
		    ਜੇ (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	ਜੇ (buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ == 0)	//ਮੁਕੰਮਲ
	    ਰੋਕੋ;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_ਢੇਰ[--(buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_ਸੂਚਕਾਂਕ + 1;	    // go to next index
	ਪੰਨਾ_ਗਿਣਤੀ = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    ਜੇ (orig_file_ਸਥਿਤੀ != OK || curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	ਜੇ (!(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ == 2 && *ml_get(1) == NUL))
	{
	    ਬਦਲਿਆ_ਅੰਦਰੂਨੀ();
	    ++CHANGEDTICK(curbuf);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (idx = 1; idx <= lnum; ++idx)
	{
	    // Need to copy one line, fetching the other one may flush it.
	    p = vim_strnsave(ml_get(idx), ml_get_len(idx));
	    i = STRCMP(p, ml_get(idx + lnum));
	    vim_free(p);
	    ਜੇ (i != 0)
	    {
		ਬਦਲਿਆ_ਅੰਦਰੂਨੀ();
		++CHANGEDTICK(curbuf);
		ਰੋਕੋ;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    ਜਦੋਂ (curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ > lnum
				       && !(curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ))
	ml_delete(curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    ਜੇ (got_int)
	emsg(_(e_recovery_interrupted));
    ਨਹੀਂਤਾਂ ਜੇ (error)
    {
	++no_wait_return;
	ਸੁਨੇਹਾ(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_ਸਤਰਾਂ_starting_with_questions));
	--no_wait_return;
	ਸੁਨੇਹਾ(_("See \":help E312\" for more information."));
	ਸੁਨੇਹਾ(">>>>>>>>>>>>>");
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (curbuf->b_ਬਦਲਿਆ)
	{
	    ਸੁਨੇਹਾ(_("Recovery completed. You should check if everything is OK."));
	    ਸੁਨੇਹਾ_puts(_("\n(You might want to write out this file under another name\n"));
	    ਸੁਨੇਹਾ_puts(_("and run diff with the original file to check for changes)"));
	}
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ(_("Recovery completed. Buffer contents equals file contents."));
	ਸੁਨੇਹਾ_puts(_("\nYou may want to delete the .swp file now."));
#ਜੇ defined(UNIX) || defined(MSWIN)
	ਜੇ (swapfile_process_running(b0p, fname_ਵਰਤਿਆ))
	{
	    // Warn there could be an active Vim on the same file, the user may
	    // want to kill it.
	    ਸੁਨੇਹਾ_puts(_("\nNote: process STILL RUNNING: "));
	    ਸੁਨੇਹਾ_outnum(char_ਮੰਜ਼ਿਲ_long(b0p->b0_pid));
	}
#ਜੇ_ਅੰਤ
	ਸੁਨੇਹਾ_puts("\n\n");
	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    ਜੇ (*buf->b_p_ਕੁੰਜੀ != NUL && STRCMP(curbuf->b_p_ਕੁੰਜੀ, buf->b_p_ਕੁੰਜੀ) != 0)
    {
	ਸੁਨੇਹਾ_puts(_("Using crypt key from swap file for the text file.\n"));
	ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"key", 0L, buf->b_p_ਕੁੰਜੀ, OPT_LOCAL);
    }
#ਜੇ_ਅੰਤ
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_ਵਰਤਿਆ);
    recoverymode = FALSE;
    ਜੇ (mfp != ਨਿਰਾ)
    {
	ਜੇ (hp != ਨਿਰਾ)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    ਜੇ (buf != ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	ਜੇ (buf->b_p_ਕੁੰਜੀ != curbuf->b_p_ਕੁੰਜੀ)
	    free_string_option(buf->b_p_ਕੁੰਜੀ);
	free_string_option(buf->b_p_cm);
#ਜੇ_ਅੰਤ
	vim_free(buf->b_ml.ml_ਢੇਰ);
	vim_free(buf);
    }
    ਜੇ (serious_error && called_from_main)
	ml_close(curbuf, TRUE);
    ਨਹੀਂਤਾਂ
    {
	apply_autocmds(EVENT_BUFREADPOST, ਨਿਰਾ, curbuf->b_fname, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ਨਿਰਾ, curbuf->b_fname, FALSE, curbuf);
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
    ਪੂਰਨ
recover_ਨਾਂ(
    char_u	*fname,		// base for swap file name
    ਪੂਰਨ		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list ਅਣਵਰਤਿਆ, // when not NULL add file names to it
    ਪੂਰਨ		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_ਬਾਹਰਲਾ)	// result when "nr" > 0
{
    ਪੂਰਨ		ਅੰਕ_ਨਾਂ;
    char_u	*(ਨਾਂ[6]);
    char_u	*p;
    ਪੂਰਨ		ਅੰਕ_files;
    ਪੂਰਨ		file_ਗਿਣਤੀ = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#ਜੇ_ਅੰਤ

    ਜੇ (fname != ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	ਜੇ (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    fname_res = fname;
    }

    ਜੇ (do_list)
    {
	// use msg() to start the scrolling properly
	ਸੁਨੇਹਾ(_("Swap files found:"));
	ਸੁਨੇਹਾ_putchar('\n');
    }

    /*
     * Do the loop for every directory in 'directory'.
     * First allocate some memory to put the directory name in.
     */
    dir_name.string = alloc(STRLEN(p_dir) + 1);
    dirp = p_dir;
    ਜਦੋਂ (dir_name.string != ਨਿਰਾ && *dirp)
    {
	/*
	 * Isolate a directory name from *dirp and put it in dir_name (we know
	 * it is large enough, so use 31000 for length).
	 * Advance dirp to next directory name.
	 */
	dir_name.ਲੰਮਾਈ = (size_t)copy_option_part(&dirp, dir_name.string, 31000, ",");

	ਜੇ (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    ਜੇ (fname == ਨਿਰਾ)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
		ਨਾਂ[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#ਨਹੀਂਤਾਂ
		ਨਾਂ[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#ਜੇ_ਅੰਤ
#ਜੇ defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		ਨਾਂ[1] = vim_strnsave((char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"));
		ਨਾਂ[2] = vim_strnsave((char_u *)".sw?", STRLEN_LITERAL(".sw?"));
		ਅੰਕ_ਨਾਂ = 3;
#ਨਹੀਂਤਾਂ
# ifdef VMS
		ਨਾਂ[1] = vim_strnsave((char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"));
		ਅੰਕ_ਨਾਂ = 2;
# ਨਹੀਂਤਾਂ
		ਅੰਕ_ਨਾਂ = 1;
# endif
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
		ਅੰਕ_ਨਾਂ = recov_file_ਨਾਂ(ਨਾਂ, fname_res, TRUE);
	}
	ਨਹੀਂਤਾਂ			    // check directory dir_name
	{
	    ਜੇ (fname == ਨਿਰਾ)
	    {
		string_T    ret;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
		ਨਾਂ[0] = concat_fnames(dir_name.string, dir_name.ਲੰਮਾਈ,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#ਨਹੀਂਤਾਂ
		ਨਾਂ[0] = concat_fnames(dir_name.string, dir_name.ਲੰਮਾਈ,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#ਜੇ_ਅੰਤ
#ਜੇ defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		ਨਾਂ[1] = concat_fnames(dir_name.string, dir_name.ਲੰਮਾਈ,
		    (char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"), TRUE, &ret);
		ਨਾਂ[2] = concat_fnames(dir_name.string, dir_name.ਲੰਮਾਈ,
		    (char_u *)".sw?", STRLEN_LITERAL(".sw?"), TRUE, &ret);
		ਅੰਕ_ਨਾਂ = 3;
#ਨਹੀਂਤਾਂ
# ifdef VMS
		ਨਾਂ[1] = concat_fnames(dir_name.string, dir_name.ਲੰਮਾਈ,
		    (char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"), TRUE, &ret);
		ਅੰਕ_ਨਾਂ = 2;
# ਨਹੀਂਤਾਂ
		ਅੰਕ_ਨਾਂ = 1;
# endif
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
	    {
		char_u	*ਪੂਛ;

#ਜੇ defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.ਲੰਮਾਈ;
		ਜੇ (after_pathsep(dir_name.string, p) && dir_name.ਲੰਮਾਈ > 1 && p[-1] == p[-2])
		{
		    // Ends with '//', Use Full path for swap name
		    ਪੂਛ = make_percent_swname(dir_name.string, p, fname_res);
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		{
		    string_T	ret;

		    ਪੂਛ = gettail(fname_res);
		    ਪੂਛ = concat_fnames(dir_name.string, dir_name.ਲੰਮਾਈ,
			ਪੂਛ, STRLEN(ਪੂਛ), TRUE, &ret);
		}
		ਜੇ (ਪੂਛ == ਨਿਰਾ)
		    ਅੰਕ_ਨਾਂ = 0;
		ਨਹੀਂਤਾਂ
		{
		    ਅੰਕ_ਨਾਂ = recov_file_ਨਾਂ(ਨਾਂ, ਪੂਛ, FALSE);
		    vim_free(ਪੂਛ);
		}
	    }
	}

	// check for out-of-memory
	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_ਨਾਂ; ++i)
	{
	    ਜੇ (ਨਾਂ[i] == ਨਿਰਾ)
	    {
		ਲਈ (i = 0; i < ਅੰਕ_ਨਾਂ; ++i)
		    vim_free(ਨਾਂ[i]);
		ਅੰਕ_ਨਾਂ = 0;
	    }
	}
	ਜੇ (ਅੰਕ_ਨਾਂ == 0)
	    ਅੰਕ_files = 0;
	ਨਹੀਂਤਾਂ ਜੇ (expand_wildcards(ਅੰਕ_ਨਾਂ, ਨਾਂ, &ਅੰਕ_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    ਅੰਕ_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	ਜੇ (*dirp == NUL && file_ਗਿਣਤੀ + ਅੰਕ_files == 0 && fname != ਨਿਰਾ)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#ਜੇ defined(VMS)
			       (char_u *)"_swp", FALSE
#ਨਹੀਂਤਾਂ
			       (char_u *)".swp", TRUE
#ਜੇ_ਅੰਤ
			      );
	    ਜੇ (swapname != ਨਿਰਾ)
	    {
		ਜੇ (mch_stat((ਅੱਖਰ *)swapname, &st) != -1)    // It exists!
		{
		    files = ALLOC_ONE(char_u *);
		    ਜੇ (files != ਨਿਰਾ)
		    {
			files[0] = swapname;
			swapname = ਨਿਰਾ;
			ਅੰਕ_files = 1;
		    }
		}
		vim_free(swapname);
	    }
	}

	/*
	 * Remove swapfile name of the current buffer, it must be ignored.
	 * But keep it for swapfilelist().
	 */
	ਜੇ (curbuf->b_ml.ml_mfp != ਨਿਰਾ
			       && (p = curbuf->b_ml.ml_mfp->mf_fname) != ਨਿਰਾ
			       && ret_list == ਨਿਰਾ)
	{
	    ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_files; ++i)
		// Do not expand wildcards, on windows would try to expand
		// "%tmp%" in "%tmp%file".
		ਜੇ (fullpathcmp(p, files[i], TRUE, FALSE) & FPC_SAME)
		{
		    // Remove the name from files[i].  Move further entries
		    // down.  When the array becomes empty free it here, since
		    // FreeWild() won't be called below.
		    vim_free(files[i]);
		    ਜੇ (--ਅੰਕ_files == 0)
			vim_free(files);
		    ਨਹੀਂਤਾਂ
			ਲਈ ( ; i < ਅੰਕ_files; ++i)
			    files[i] = files[i + 1];
		}
	}
	ਜੇ (nr > 0)
	{
	    file_ਗਿਣਤੀ += ਅੰਕ_files;
	    ਜੇ (nr <= file_ਗਿਣਤੀ)
	    {
		*fname_ਬਾਹਰਲਾ = vim_strsave(
				      files[nr - 1 + ਅੰਕ_files - file_ਗਿਣਤੀ]);
		dirp = (char_u *)"";		    // stop searching
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (do_list)
	{
	    ਜੇ (dir_name.string[0] == '.' && dir_name.string[1] == NUL)
	    {
		ਜੇ (fname == ਨਿਰਾ)
		    ਸੁਨੇਹਾ_puts(_("   In current directory:\n"));
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ_puts(_("   Using specified name:\n"));
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਸੁਨੇਹਾ_puts(_("   In directory "));
		ਸੁਨੇਹਾ_home_replace(dir_name.string);
		ਸੁਨੇਹਾ_puts(":\n");
	    }

	    ਜੇ (ਅੰਕ_files)
	    {
		ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_files; ++i)
		{
		    // print the swap file name
		    ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)++file_ਗਿਣਤੀ);
		    ਸੁਨੇਹਾ_puts(".    ");
		    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)gettail(files[i]));
		    ਸੁਨੇਹਾ_putchar('\n');
		    (ਖਾਲੀ)swapfile_info(files[i]);
		}
	    }
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ_puts(_("      -- none --\n"));
	    ਬਾਹਰਲਾ_flush();
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਨਹੀਂਤਾਂ ਜੇ (ret_list != ਨਿਰਾ)
	{
	    string_T	name;

	    ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.ਲੰਮਾਈ,
		    files[i], STRLEN(files[i]), TRUE, &name);
		ਜੇ (name.string != ਨਿਰਾ)
		{
		    list_append_string(ret_list, name.string, (ਪੂਰਨ)name.ਲੰਮਾਈ);
		    vim_free(name.string);
		}
	    }
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ
	    file_ਗਿਣਤੀ += ਅੰਕ_files;

	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_ਨਾਂ; ++i)
	    vim_free(ਨਾਂ[i]);
	ਜੇ (ਅੰਕ_files > 0)
	    FreeWild(ਅੰਕ_files, files);
    }
    vim_free(dir_name.string);
    ਵਾਪਸ file_ਗਿਣਤੀ;
}

#ਜੇ defined(UNIX) || defined(MSWIN)
/*
 * Need _very_ long file names.
 * Append the full path to name with path separators made into percent
 * signs, to "dir". An unnamed buffer is handled as "" (<currentdir>/"")
 * The last character in "dir" must be an extra slash or backslash, it is
 * removed.
 */
    char_u *
make_percent_swname(char_u *dir, char_u *dir_ਸਿਰਾ, char_u *name)
{
    string_T	d = {ਨਿਰਾ, 0};
    string_T	fixed_fname;
    char_u	*p;

    fixed_fname.string = fix_fname(name != ਨਿਰਾ ? name : (char_u *)"");
    ਜੇ (fixed_fname.string == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (p = fixed_fname.string; *p != NUL; MB_PTR_ADV(p))
	ਜੇ (vim_ispathsep(*p))
	    *p = '%';
    fixed_fname.ਲੰਮਾਈ = (size_t)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_ਸਿਰਾ[-1];
    *p = NUL;
    concat_fnames(dir, (size_t)(p - dir), fixed_fname.string, fixed_fname.ਲੰਮਾਈ, TRUE, &d);
    vim_free(fixed_fname.string);

    ਵਾਪਸ d.string;
}
#ਜੇ_ਅੰਤ

#ਜੇ (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
ਸਥਿਰ ਪੂਰਨ process_still_running;
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * Return information found in swapfile "fname" in dictionary "d".
 * This is used by the swapinfo() function.
 */
    ਖਾਲੀ
get_b0_dict(char_u *fname, dict_T *d)
{
    ਪੂਰਨ fd;
    ਬਣਤਰ block0 b0;

    ਜੇ ((fd = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
    {
	ਜੇ (read_eintr(fd, &b0, ਆਕਾਰ(b0)) == ਆਕਾਰ(b0))
	{
	    ਜੇ (ml_check_b0_id(&b0) == FAIL)
		dict_ਜੋੜੋ_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    ਨਹੀਂਤਾਂ ਜੇ (b0_magic_wrong(&b0))
		dict_ਜੋੜੋ_string_len(d, "error",
		    (char_u *)"Magic number mismatch", STRLEN_LITERAL("Magic number mismatch"));
	    ਨਹੀਂਤਾਂ
	    {
		// we have swap information
		dict_ਜੋੜੋ_string_len(d, "version", b0.b0_ਸੰਸਕਰਣ, 10);
		dict_ਜੋੜੋ_string_len(d, "user", b0.b0_uname, B0_UNAME_ਮਾਪ);
		dict_ਜੋੜੋ_string_len(d, "host", b0.b0_hname, B0_HNAME_ਮਾਪ);
		dict_ਜੋੜੋ_string_len(d, "fname", b0.b0_fname, B0_FNAME_ਮਾਪ_ORG);

		dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "pid", char_ਮੰਜ਼ਿਲ_long(b0.b0_pid));
		dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "mtime", char_ਮੰਜ਼ਿਲ_long(b0.b0_mtime));
		dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "dirty", b0.b0_dirty ? 1 : 0);
# ifdef CHECK_INODE
		dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "inode", char_ਮੰਜ਼ਿਲ_long(b0.b0_ino));
# endif
	    }
	}
	ਨਹੀਂਤਾਂ
	    dict_ਜੋੜੋ_string_len(d, "error", (char_u *)"Cannot read file", STRLEN_LITERAL("Cannot read file"));
	close(fd);
    }
    ਨਹੀਂਤਾਂ
	dict_ਜੋੜੋ_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#ਜੇ_ਅੰਤ

/*
 * Give information about an existing swap file.
 * Returns timestamp (0 when unknown).
 */
    ਸਥਿਰ time_t
swapfile_info(char_u *fname)
{
    stat_T	    st;
    ਪੂਰਨ		    fd;
    ਬਣਤਰ block0   b0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    char_u	    uname[B0_UNAME_ਮਾਪ];
#ਜੇ_ਅੰਤ

    // print the swap file date
    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) != -1)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	// print name of owner of the file
	ਜੇ (mch_get_uname(st.st_uid, uname, B0_UNAME_ਮਾਪ) == OK)
	{
	    ਸੁਨੇਹਾ_puts(_("          owned by: "));
	    ਸੁਨੇਹਾ_outtrans(uname);
	    ਸੁਨੇਹਾ_puts(_("   dated: "));
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਸੁਨੇਹਾ_puts(_("             dated: "));
	ਸੁਨੇਹਾ_puts(get_ctime(st.st_mtime, TRUE));
    }
    ਨਹੀਂਤਾਂ
	st.st_mtime = 0;

    /*
     * print the original file name
     */
    fd = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_EXTRA, 0);
    ਜੇ (fd >= 0)
    {
	ਜੇ (read_eintr(fd, &b0, ਆਕਾਰ(b0)) == ਆਕਾਰ(b0))
	{
	    ਜੇ (STRNCMP(b0.b0_ਸੰਸਕਰਣ, "VIM 3.0", 7) == 0)
	    {
		ਸੁਨੇਹਾ_puts(_("         [from Vim version 3.0]"));
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ml_check_b0_id(&b0) == FAIL)
	    {
		ਸੁਨੇਹਾ_puts(_("         [does not look like a Vim swap file]"));
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਸੁਨੇਹਾ_puts(_("         file name: "));
		ਜੇ (b0.b0_fname[0] == NUL)
		    ਸੁਨੇਹਾ_puts(_("[No Name]"));
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ_outtrans(b0.b0_fname);

		ਸੁਨੇਹਾ_puts(_("\n          modified: "));
		ਸੁਨੇਹਾ_puts(b0.b0_dirty ? _("YES") : _("no"));

		ਜੇ (*(b0.b0_uname) != NUL)
		{
		    ਸੁਨੇਹਾ_puts(_("\n         user name: "));
		    ਸੁਨੇਹਾ_outtrans(b0.b0_uname);
		}

		ਜੇ (*(b0.b0_hname) != NUL)
		{
		    ਜੇ (*(b0.b0_uname) != NUL)
			ਸੁਨੇਹਾ_puts(_("   host name: "));
		    ਨਹੀਂਤਾਂ
			ਸੁਨੇਹਾ_puts(_("\n         host name: "));
		    ਸੁਨੇਹਾ_outtrans(b0.b0_hname);
		}

		ਜੇ (char_ਮੰਜ਼ਿਲ_long(b0.b0_pid) != 0L)
		{
		    ਸੁਨੇਹਾ_puts(_("\n        process ID: "));
		    ਸੁਨੇਹਾ_outnum(char_ਮੰਜ਼ਿਲ_long(b0.b0_pid));
#ਜੇ defined(UNIX) || defined(MSWIN)
		    ਜੇ (swapfile_process_running(&b0, fname))
		    {
			ਸੁਨੇਹਾ_puts(_(" (STILL RUNNING)"));
# ifdef HAVE_PROCESS_STILL_RUNNING
			process_still_running = TRUE;
# endif
		    }
#ਜੇ_ਅੰਤ
		}

		ਜੇ (b0_magic_wrong(&b0))
		{
#ਜੇ defined(MSWIN)
		    ਜੇ (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			ਸੁਨੇਹਾ_puts(_("\n         [not usable with this version of Vim]"));
		    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
			ਸੁਨੇਹਾ_puts(_("\n         [not usable on this computer]"));
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ_puts(_("         [cannot be read]"));
	close(fd);
    }
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ_puts(_("         [cannot be opened]"));
    ਸੁਨੇਹਾ_putchar('\n');

    ਵਾਪਸ st.st_mtime;
}

/*
 * Return TRUE if the swap file looks OK and there are no changes, thus it can
 * be safely deleted.
 */
    ਸਥਿਰ ਪੂਰਨ
swapfile_unchanged(char_u *fname)
{
    stat_T	    st;
    ਪੂਰਨ		    fd;
    ਬਣਤਰ block0   b0;
    ਪੂਰਨ		    ret = TRUE;

    // must be able to stat the swap file
    ਜੇ (mch_stat((ਅੱਖਰ *)fname, &st) == -1)
	ਵਾਪਸ FALSE;

    // must be able to read the first block
    fd = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_EXTRA, 0);
    ਜੇ (fd < 0)
	ਵਾਪਸ FALSE;
    ਜੇ (read_eintr(fd, &b0, ਆਕਾਰ(b0)) != ਆਕਾਰ(b0))
    {
	close(fd);
	ਵਾਪਸ FALSE;
    }

    // the ID and magic number must be correct
    ਜੇ (ml_check_b0_id(&b0) == FAIL|| b0_magic_wrong(&b0))
	ret = FALSE;

    // must be unchanged
    ਜੇ (b0.b0_dirty)
	ret = FALSE;

#ਜੇ defined(UNIX) || defined(MSWIN)
    // Host name must be known and must equal the current host name, otherwise
    // comparing pid is meaningless.
    ਜੇ (*(b0.b0_hname) == NUL)
    {
	ret = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	char_u	    hostname[B0_HNAME_ਮਾਪ];

	mch_get_host_name(hostname, B0_HNAME_ਮਾਪ);
	hostname[B0_HNAME_ਮਾਪ - 1] = NUL;
	b0.b0_hname[B0_HNAME_ਮਾਪ - 1] = NUL; // in case of corruption
	ਜੇ (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    ਜੇ (char_ਮੰਜ਼ਿਲ_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#ਜੇ_ਅੰਤ

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    close(fd);
    ਵਾਪਸ ret;
}

    ਸਥਿਰ ਪੂਰਨ
recov_file_ਨਾਂ(char_u **ਨਾਂ, char_u *ਰਸਤਾ, ਪੂਰਨ prepend_dot)
{
    ਪੂਰਨ		ਅੰਕ_ਨਾਂ;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    ਪੂਰਨ		i;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
    bool    shortname = curbuf->b_shortname;

    curbuf->b_shortname = false;
#ਜੇ_ਅੰਤ
    string_T	ret;

    ਅੰਕ_ਨਾਂ = 0;

    /*
     * May also add the file name with a dot prepended, for swap file in same
     * dir as original file.
     */
    ਜੇ (prepend_dot)
    {
	ਨਾਂ[ਅੰਕ_ਨਾਂ] = modname(ਰਸਤਾ, (char_u *)".sw?", TRUE);
	ਜੇ (ਨਾਂ[ਅੰਕ_ਨਾਂ] == ਨਿਰਾ)
	    ਜਾਓ ਸਿਰਾ;
	++ਅੰਕ_ਨਾਂ;
    }

    /*
     * Form the normal swap file name pattern by appending ".sw?".
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    ਨਾਂ[ਅੰਕ_ਨਾਂ] = concat_fnames(ਰਸਤਾ, STRLEN(ਰਸਤਾ),
	(char_u *)"_sw%", STRLEN_LITERAL("_sw%"), FALSE, &ret);
#ਨਹੀਂਤਾਂ
    ਨਾਂ[ਅੰਕ_ਨਾਂ] = concat_fnames(ਰਸਤਾ, STRLEN(ਰਸਤਾ),
	(char_u *)".sw?", STRLEN_LITERAL(".sw?"), FALSE, &ret);
#ਜੇ_ਅੰਤ
    ਜੇ (ਨਾਂ[ਅੰਕ_ਨਾਂ] == ਨਿਰਾ)
	ਜਾਓ ਸਿਰਾ;
    ਜੇ (ਅੰਕ_ਨਾਂ >= 1)	    // check if we have the same name twice
    {
	p = ਨਾਂ[ਅੰਕ_ਨਾਂ - 1];
	i = (ਪੂਰਨ)STRLEN(ਨਾਂ[ਅੰਕ_ਨਾਂ - 1]) - (ਪੂਰਨ)STRLEN(ਨਾਂ[ਅੰਕ_ਨਾਂ]);
	ਜੇ (i > 0)
	    p += i;	    // file name has been expanded to full path

	ਜੇ (STRCMP(p, ਨਾਂ[ਅੰਕ_ਨਾਂ]) != 0)
	    ++ਅੰਕ_ਨਾਂ;
	ਨਹੀਂਤਾਂ
	    vim_free(ਨਾਂ[ਅੰਕ_ਨਾਂ]);
    }
    ਨਹੀਂਤਾਂ
	++ਅੰਕ_ਨਾਂ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
    /*
     * Also try with 'shortname' set, in case the file is on a DOS filesystem.
     */
    curbuf->b_shortname = true;
# ifdef VMS
    ਨਾਂ[ਅੰਕ_ਨਾਂ] = modname(ਰਸਤਾ, (char_u *)"_sw%", FALSE);
# ਨਹੀਂਤਾਂ
    ਨਾਂ[ਅੰਕ_ਨਾਂ] = modname(ਰਸਤਾ, (char_u *)".sw?", FALSE);
# endif
    ਜੇ (ਨਾਂ[ਅੰਕ_ਨਾਂ] == ਨਿਰਾ)
	ਜਾਓ ਸਿਰਾ;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = ਨਾਂ[ਅੰਕ_ਨਾਂ];
    i = STRLEN(ਨਾਂ[ਅੰਕ_ਨਾਂ]) - STRLEN(ਨਾਂ[ਅੰਕ_ਨਾਂ - 1]);
    ਜੇ (i > 0)
	p += i;		// file name has been expanded to full path
    ਜੇ (STRCMP(ਨਾਂ[ਅੰਕ_ਨਾਂ - 1], p) == 0)
	vim_free(ਨਾਂ[ਅੰਕ_ਨਾਂ]);
    ਨਹੀਂਤਾਂ
	++ਅੰਕ_ਨਾਂ;
#ਜੇ_ਅੰਤ

ਸਿਰਾ:
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
    curbuf->b_shortname = shortname;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਅੰਕ_ਨਾਂ;
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
    ਖਾਲੀ
ml_sync_all(ਪੂਰਨ check_file, ਪੂਰਨ check_char)
{
    buf_T		*buf;
    stat_T		st;

    FOR_ALL_BUFFERS(buf)
    {
	ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ
		|| buf->b_ml.ml_mfp->mf_fname == ਨਿਰਾ
		|| buf->b_ml.ml_mfp->mf_fd < 0)
	    ਜਾਰੀ;			    // no file

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
	ਜੇ (crypt_may_close_swapfile(buf, buf->b_p_ਕੁੰਜੀ,
						     crypt_get_ਵਿਧੀ_nr(buf)))
	    ਜਾਰੀ;
#ਜੇ_ਅੰਤ

	ml_flush_ਸਤਰ(buf);		    // flush buffered line
					    // flush locked block
	(ਖਾਲੀ)ml_ਲੱਭੋ_ਸਤਰ(buf, (linenr_T)0, ML_FLUSH);
	ਜੇ (buf_Is_ਬਦਲਿਆ(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != ਨਿਰਾ)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    ਜੇ (mch_stat((ਅੱਖਰ *)buf->b_ffname, &st) == -1
		    || st.st_mtime != buf->b_mtime_read
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ST_MTIM_NSEC
		    || st.ST_MTIM_NSEC != buf->b_mtime_read_ns
#ਜੇ_ਅੰਤ
		    || st.st_size != buf->b_orig_ਮਾਪ)
	    {
		ml_preserve(buf, FALSE);
		did_check_timestamps = FALSE;
		need_check_timestamps = TRUE;	// give message later
	    }
	}
	ਜੇ (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (ਖਾਲੀ)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (buf_Is_ਬਦਲਿਆ(buf) ? MFS_FLUSH : 0));
	    ਜੇ (check_char && ui_char_avail())	// character available now
		ਰੋਕੋ;
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
    ਖਾਲੀ
ml_preserve(buf_T *buf, ਪੂਰਨ ਸੁਨੇਹਾ_2)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    ਪੂਰਨ		ਸਥਿਤੀ;
    ਪੂਰਨ		got_int_save = got_int;

    ਜੇ (mfp == ਨਿਰਾ || mfp->mf_fname == ਨਿਰਾ)
    {
	ਜੇ (ਸੁਨੇਹਾ_2)
	    emsg(_(e_cannot_preserve_there_is_no_ਅਦਲਾ_ਬਦਲੀ_file));
	ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    ਜੇ (crypt_may_close_swapfile(buf, buf->b_p_ਕੁੰਜੀ, crypt_get_ਵਿਧੀ_nr(buf)))
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_ਸਤਰ(buf);				    // flush buffered line
    (ਖਾਲੀ)ml_ਲੱਭੋ_ਸਤਰ(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    ਸਥਿਤੀ = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

    // stack is invalid after mf_sync(.., MFS_ALL)
    buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;

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
    ਜੇ (mf_need_trans(mfp) && !got_int)
    {
	lnum = 1;
	ਜਦੋਂ (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    hp = ml_ਲੱਭੋ_ਸਤਰ(buf, lnum, ML_ਲੱਭੋ);
	    ਜੇ (hp == ਨਿਰਾ)
	    {
		ਸਥਿਤੀ = FAIL;
		ਜਾਓ theend;
	    }
	    CHECK(buf->b_ml.ml_locked_ਨੀਵਾਂ != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_ਉੱਚਾ + 1;
	}
	(ਖਾਲੀ)ml_ਲੱਭੋ_ਸਤਰ(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	ਜੇ (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    ਸਥਿਤੀ = FAIL;
	buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    ਜੇ (ਸੁਨੇਹਾ_2)
    {
	ਜੇ (ਸਥਿਤੀ == OK)
	    ਸੁਨੇਹਾ(_("File preserved"));
	ਨਹੀਂਤਾਂ
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
    ਵਾਪਸ ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
    char_u *
ml_get_pos(pos_T *pos)
{
    ਵਾਪਸ (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->ਖਾਨਾ);
}

/*
 * Return pointer to cursor line.
 */
    char_u *
ml_get_curline(ਖਾਲੀ)
{
    ਵਾਪਸ ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE);
}

/*
 * Return pointer to cursor position.
 */
    char_u *
ml_get_cursor(ਖਾਲੀ)
{
    ਵਾਪਸ (ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE) +
							curwin->w_cursor.ਖਾਨਾ);
}

// return length (excluding the NUL) of the given line
    colnr_T
ml_get_len(linenr_T lnum)
{
    ਵਾਪਸ ml_get_buf_len(curbuf, lnum);
}

// return length (excluding the NUL) of the text after position "pos"
    colnr_T
ml_get_pos_len(pos_T *pos)
{
    ਵਾਪਸ ml_get_buf_len(curbuf, pos->lnum) - pos->ਖਾਨਾ;
}

// return length (excluding the NUL) of the cursor line
    colnr_T
ml_get_curline_len(ਖਾਲੀ)
{
    ਵਾਪਸ ml_get_buf_len(curbuf, curwin->w_cursor.lnum);
}

// return length (excluding the NUL) of the cursor position
    colnr_T
ml_get_cursor_len(ਖਾਲੀ)
{
    ਵਾਪਸ ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.ਖਾਨਾ;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*ਸਤਰ;

    ਜੇ (*(ਸਤਰ = ml_get_buf(buf, lnum, FALSE)) == NUL)
	ਵਾਪਸ 0;

    ਜੇ (buf->b_ml.ml_ਸਤਰ_textlen <= 0)
	buf->b_ml.ml_ਸਤਰ_textlen = (ਪੂਰਨ)STRLEN(ਸਤਰ) + 1;
    ਵਾਪਸ buf->b_ml.ml_ਸਤਰ_textlen - 1;
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
    ਪੂਰਨ		will_change)		// line will be changed
{
    bhdr_T	*hp;
    ਅੰਕੜੇ_BL	*dp;
    ਸਥਿਰ ਪੂਰਨ	recursive = 0;
    ਸਥਿਰ char_u questions[4];

    ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)	// invalid line number
    {
	ਜੇ (recursive == 0)
	{
	    // Avoid giving this message for a recursive call, may happen when
	    // the GUI redraws part of the text.
	    ++recursive;
	    siemsg(e_ml_get_invalid_lnum_nr, lnum);
	    --recursive;
	}
	ml_flush_ਸਤਰ(buf);
errorret:
	STRCPY(questions, "???");
	buf->b_ml.ml_ਸਤਰ_len = 4;
	buf->b_ml.ml_ਸਤਰ_textlen = buf->b_ml.ml_ਸਤਰ_len;
	buf->b_ml.ml_ਸਤਰ_lnum = lnum;
	ਵਾਪਸ questions;
    }
    ਜੇ (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)	// there are no lines
    {
	buf->b_ml.ml_ਸਤਰ_len = 1;
	buf->b_ml.ml_ਸਤਰ_textlen = buf->b_ml.ml_ਸਤਰ_len;
	ਵਾਪਸ (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    ਜੇ (buf->b_ml.ml_ਸਤਰ_lnum != lnum || mf_dont_release)
    {
	ਬਿਨਾਨਿਸ਼ਾਨੀ    ਸ਼ੁਰੂ, ਸਿਰਾ;
	ਪੂਰਨ	    idx;

	ml_flush_ਸਤਰ(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	ਜੇ ((hp = ml_ਲੱਭੋ_ਸਤਰ(buf, lnum, ML_ਲੱਭੋ)) == ਨਿਰਾ)
	{
	    ਜੇ (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_ਲੱਭੋ_ਸਤਰ_nr_ਵਿੱਚ_ਬਫਰ_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    ਜਾਓ errorret;
	}

	dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);

	idx = lnum - buf->b_ml.ml_locked_ਨੀਵਾਂ;
	ਸ਼ੁਰੂ = ((dp->db_ਸੂਚਕਾਂਕ[idx]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	ਜੇ (idx == 0)
	    ਸਿਰਾ = dp->db_txt_ਸਿਰਾ;
	ਨਹੀਂਤਾਂ
	    ਸਿਰਾ = ((dp->db_ਸੂਚਕਾਂਕ[idx - 1]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);

	buf->b_ml.ml_ਸਤਰ_ptr = (char_u *)dp + ਸ਼ੁਰੂ;
	buf->b_ml.ml_ਸਤਰ_len = ਸਿਰਾ - ਸ਼ੁਰੂ;
#ਜੇ defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	ਜੇ (buf->b_has_textprop
			 && (size_t)buf->b_ml.ml_ਸਤਰ_len > ਆਕਾਰ(textprop_T))
	    buf->b_ml.ml_ਸਤਰ_textlen = 0;  // call STRLEN() later when needed
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    buf->b_ml.ml_ਸਤਰ_textlen = buf->b_ml.ml_ਸਤਰ_len;
	buf->b_ml.ml_ਸਤਰ_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_ਸਤਰ_DIRTY | ML_ALLOCATED);
    }
    ਜੇ (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (ml_get_alloc_ਸਤਰਾਂ && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_ਸਤਰ_DIRTY;
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (ml_get_alloc_ਸਤਰਾਂ
		 && (buf->b_ml.ml_flags & (ML_ਸਤਰ_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_ਸਤਰ_len);

	// make sure the text is in allocated memory
	ਜੇ (p != ਨਿਰਾ)
	{
	    memmove(p, buf->b_ml.ml_ਸਤਰ_ptr, buf->b_ml.ml_ਸਤਰ_len);
	    buf->b_ml.ml_ਸਤਰ_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    ਜੇ (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_ਸਤਰ_DIRTY;
	}
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ buf->b_ml.ml_ਸਤਰ_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    ਪੂਰਨ
ml_ਸਤਰ_alloced(ਖਾਲੀ)
{
    ਵਾਪਸ (curbuf->b_ml.ml_flags & ML_ਸਤਰ_DIRTY);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_ਪਾਠ_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**ਸਤਰ,
	    ਪੂਰਨ		*len,
	    char_u	**tofree)
{
    ਪੂਰਨ		ਗੇੜਾ;
    ਪੂਰਨ		new_prop_ਗਿਣਤੀ = 0;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    ਪੂਰਨ		n;
    char_u	*props;
    ਪੂਰਨ		new_len = 0;  // init for gcc
    char_u	*new_ਸਤਰ = ਨਿਰਾ;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	ਜੇ (ਗੇੜਾ == 2)
	{
	    uint16_t pc;

	    ਜੇ (new_prop_ਗਿਣਤੀ == 0)
		ਵਾਪਸ;  // nothing to do
	    new_len = *len + (ਪੂਰਨ)PROP_ਗਿਣਤੀ_ਮਾਪ
			     + new_prop_ਗਿਣਤੀ * (ਪੂਰਨ)ਆਕਾਰ(textprop_T);
	    new_ਸਤਰ = alloc(new_len);
	    ਜੇ (new_ਸਤਰ == ਨਿਰਾ)
		ਵਾਪਸ;
	    mch_memmove(new_ਸਤਰ, *ਸਤਰ, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_ਗਿਣਤੀ;
	    mch_memmove(new_ਸਤਰ + *len, &pc, PROP_ਗਿਣਤੀ_ਮਾਪ);
	    new_prop_ਗਿਣਤੀ = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	ਗਿਣਤੀ_2 = get_ਪਾਠ_props(buf, lnum, &props, FALSE);
	ਲਈ (n = 0; n < ਗਿਣਤੀ_2; ++n)
	{
	    mch_memmove(&prop, props + n * ਆਕਾਰ(textprop_T),
							   ਆਕਾਰ(textprop_T));
	    ਜੇ (prop.tp_flags & TP_FLAG_CONT_ਅਗਲਾ)
	    {
		ਜੇ (ਗੇੜਾ == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_PREV;
		    prop.tp_ਖਾਨਾ = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_ਪਾਠ_ਵਿੱਥ = 0;
		    mch_memmove(new_ਸਤਰ + *len + (ਪੂਰਨ)PROP_ਗਿਣਤੀ_ਮਾਪ
			    + new_prop_ਗਿਣਤੀ * ਆਕਾਰ(textprop_T),
			    &prop, ਆਕਾਰ(textprop_T));
		}
		++new_prop_ਗਿਣਤੀ;
	    }
	}
    }
    *ਸਤਰ = new_ਸਤਰ;
    *tofree = new_ਸਤਰ;
    *len = new_len;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ਸਤਰ_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    ਪੂਰਨ		flags)		// ML_APPEND_ flags
{
    char_u	*ਸਤਰ = ਸਤਰ_arg;
    colnr_T	len = len_arg;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ਸਤਰ_ਗਿਣਤੀ;	// number of indexes in current block
    ਪੂਰਨ		ਵਿੱਥ;
    ਪੂਰਨ		from, ਮੰਜ਼ਿਲ;
    ਲੰਮਾ	space_needed;	// space needed for new line
    ਪੂਰਨ		ਪੰਨਾ_ਮਾਪ;
    ਪੂਰਨ		ਪੰਨਾ_ਗਿਣਤੀ;
    ਪੂਰਨ		db_idx;		// index for lnum in data block
    bhdr_T	*hp;
    memfile_T	*mfp;
    ਅੰਕੜੇ_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    char_u	*tofree = ਨਿਰਾ;
# ifdef FEAT_BYTEOFF
    colnr_T	ਪਾਠ_len = 0;	// text len with NUL without text properties
# endif
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		ret = FAIL;

    ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ || buf->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;  // lnum out of range

    ਜੇ (ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked && ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked > lnum)
	ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked = lnum + 1;

    ਜੇ (len == 0)
    {
	len = (colnr_T)STRLEN(ਸਤਰ) + 1;	// space needed for the text
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	ਪਾਠ_len = len;
#ਜੇ_ਅੰਤ
    }
#ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	ਪਾਠ_len = (colnr_T)STRLEN(ਸਤਰ) + 1;
    ਨਹੀਂਤਾਂ
	ਪਾਠ_len = len;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	ਜੋੜੋ_ਪਾਠ_props_for_append(buf, lnum, &ਸਤਰ, &len, &tofree);
#ਜੇ_ਅੰਤ

    space_needed = len + ਸੂਚਕਾਂਕ_ਮਾਪ;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    ਪੰਨਾ_ਮਾਪ = mfp->mf_ਪੰਨਾ_ਮਾਪ;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    ਜੇ ((hp = ml_ਲੱਭੋ_ਸਤਰ(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_ਪਾਓ)) == ਨਿਰਾ)
	ਜਾਓ theend;

    buf->b_ml.ml_flags &= ~ML_ਖਾਲੀ_ਥਾਵਾਂ;

    ਜੇ (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    ਨਹੀਂਤਾਂ
	db_idx = lnum - buf->b_ml.ml_locked_ਨੀਵਾਂ;
		// get line count before the insertion
    ਸਤਰ_ਗਿਣਤੀ = buf->b_ml.ml_locked_ਉੱਚਾ - buf->b_ml.ml_locked_ਨੀਵਾਂ;

    dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    ਜੇ ((ਲੰਮਾ)dp->db_free < space_needed && db_idx == ਸਤਰ_ਗਿਣਤੀ - 1
					    && lnum < buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_ਉੱਚਾ);
	ਜੇ ((hp = ml_ਲੱਭੋ_ਸਤਰ(buf, lnum + 1, ML_ਪਾਓ)) == ਨਿਰਾ)
	    ਜਾਓ theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	ਸਤਰ_ਗਿਣਤੀ = buf->b_ml.ml_locked_ਉੱਚਾ - buf->b_ml.ml_locked_ਨੀਵਾਂ;
	CHECK(buf->b_ml.ml_locked_ਨੀਵਾਂ != lnum + 1, "locked_low != lnum + 1");

	dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
    }

    ++buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਜੇ ((ਲੰਮਾ)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_ਸ਼ੁਰੂ -= len;
	dp->db_free -= space_needed;
	++(dp->db_ਸਤਰ_ਗਿਣਤੀ);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	ਜੇ (ਸਤਰ_ਗਿਣਤੀ > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    ਜੇ (db_idx < 0)
		ਵਿੱਥ = dp->db_txt_ਸਿਰਾ;
	    ਨਹੀਂਤਾਂ
		ਵਿੱਥ = ((dp->db_ਸੂਚਕਾਂਕ[db_idx]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
	    mch_memmove((ਅੱਖਰ *)dp + dp->db_txt_ਸ਼ੁਰੂ,
					  (ਅੱਖਰ *)dp + dp->db_txt_ਸ਼ੁਰੂ + len,
				 (size_t)(ਵਿੱਥ - (dp->db_txt_ਸ਼ੁਰੂ + len)));
	    ਲਈ (i = ਸਤਰ_ਗਿਣਤੀ - 1; i > db_idx; --i)
		dp->db_ਸੂਚਕਾਂਕ[i + 1] = dp->db_ਸੂਚਕਾਂਕ[i] - len;
	    dp->db_ਸੂਚਕਾਂਕ[db_idx + 1] = ਵਿੱਥ - len;
	}
	ਨਹੀਂਤਾਂ
	    // add line at the end (which is the start of the text)
	    dp->db_ਸੂਚਕਾਂਕ[db_idx + 1] = dp->db_txt_ਸ਼ੁਰੂ;

	/*
	 * copy the text into the block
	 */
	mch_memmove((ਅੱਖਰ *)dp + dp->db_ਸੂਚਕਾਂਕ[db_idx + 1], ਸਤਰ, (size_t)len);
	ਜੇ (flags & ML_APPEND_MARK)
	    dp->db_ਸੂਚਕਾਂਕ[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	ਜੇ (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    ਨਹੀਂਤਾਂ	    // not enough space in data block
    {
	ਲੰਮਾ	    ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ, ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ;
	ਪੂਰਨ	    ਪੰਨਾ_ਗਿਣਤੀ_ਖੱਬਾ, ਪੰਨਾ_ਗਿਣਤੀ_ਸੱਜਾ;
	bhdr_T	    *hp_ਖੱਬਾ;
	bhdr_T	    *hp_ਸੱਜਾ;
	bhdr_T	    *hp_new;
	ਪੂਰਨ	    ਸਤਰਾਂ_moved;
	ਪੂਰਨ	    ਅੰਕੜੇ_moved = 0;	    // init to shut up gcc
	ਪੂਰਨ	    ਕੁੱਲ_moved = 0;	    // init to shut up gcc
	ਅੰਕੜੇ_BL	    *dp_ਸੱਜਾ, *dp_ਖੱਬਾ;
	ਪੂਰਨ	    ਢੇਰ_idx;
	ਪੂਰਨ	    ਵਿੱਚ_ਖੱਬਾ;
	ਪੂਰਨ	    lineadd;
	blocknr_T   bnum_ਖੱਬਾ, bnum_ਸੱਜਾ;
	linenr_T    lnum_ਖੱਬਾ, lnum_ਸੱਜਾ;
	ਪੂਰਨ	    pb_idx;
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
	ਜੇ (db_idx < 0)		// left block is new, right block is existing
	{
	    ਸਤਰਾਂ_moved = 0;
	    ਵਿੱਚ_ਖੱਬਾ = TRUE;
	    // space_needed does not change
	}
	ਨਹੀਂਤਾਂ			// left block is existing, right block is new
	{
	    ਸਤਰਾਂ_moved = ਸਤਰ_ਗਿਣਤੀ - db_idx - 1;
	    ਜੇ (ਸਤਰਾਂ_moved == 0)
		ਵਿੱਚ_ਖੱਬਾ = FALSE;	// put new line in right block
					// space_needed does not change
	    ਨਹੀਂਤਾਂ
	    {
		ਅੰਕੜੇ_moved = ((dp->db_ਸੂਚਕਾਂਕ[db_idx]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ) -
							    dp->db_txt_ਸ਼ੁਰੂ;
		ਕੁੱਲ_moved = ਅੰਕੜੇ_moved + ਸਤਰਾਂ_moved * ਸੂਚਕਾਂਕ_ਮਾਪ;
		ਜੇ ((ਲੰਮਾ)dp->db_free + ਕੁੱਲ_moved >= space_needed)
		{
		    ਵਿੱਚ_ਖੱਬਾ = TRUE;	// put new line in left block
		    space_needed = ਕੁੱਲ_moved;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਵਿੱਚ_ਖੱਬਾ = FALSE;	    // put new line in right block
		    space_needed += ਕੁੱਲ_moved;
		}
	    }
	}

	ਪੰਨਾ_ਗਿਣਤੀ = ((space_needed + ਸਿਰਲੇਖ_ਮਾਪ) + ਪੰਨਾ_ਮਾਪ - 1) / ਪੰਨਾ_ਮਾਪ;
	ਜੇ ((hp_new = ml_new_ਅੰਕੜੇ(mfp, flags & ML_APPEND_NEW, ਪੰਨਾ_ਗਿਣਤੀ))
								       == ਨਿਰਾ)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_ਉੱਚਾ);
	    ਜਾਓ theend;
	}
	ਜੇ (db_idx < 0)		// left block is new
	{
	    hp_ਖੱਬਾ = hp_new;
	    hp_ਸੱਜਾ = hp;
	    ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ = 0;
	    ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ = ਸਤਰ_ਗਿਣਤੀ;
	}
	ਨਹੀਂਤਾਂ			// right block is new
	{
	    hp_ਖੱਬਾ = hp;
	    hp_ਸੱਜਾ = hp_new;
	    ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ = ਸਤਰ_ਗਿਣਤੀ;
	    ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ = 0;
	}
	dp_ਸੱਜਾ = (ਅੰਕੜੇ_BL *)(hp_ਸੱਜਾ->bh_ਅੰਕੜੇ);
	dp_ਖੱਬਾ = (ਅੰਕੜੇ_BL *)(hp_ਖੱਬਾ->bh_ਅੰਕੜੇ);
	bnum_ਖੱਬਾ = hp_ਖੱਬਾ->bh_bnum;
	bnum_ਸੱਜਾ = hp_ਸੱਜਾ->bh_bnum;
	ਪੰਨਾ_ਗਿਣਤੀ_ਖੱਬਾ = hp_ਖੱਬਾ->bh_ਪੰਨਾ_ਗਿਣਤੀ;
	ਪੰਨਾ_ਗਿਣਤੀ_ਸੱਜਾ = hp_ਸੱਜਾ->bh_ਪੰਨਾ_ਗਿਣਤੀ;

	/*
	 * May move the new line into the right/new block.
	 */
	ਜੇ (!ਵਿੱਚ_ਖੱਬਾ)
	{
	    dp_ਸੱਜਾ->db_txt_ਸ਼ੁਰੂ -= len;
	    dp_ਸੱਜਾ->db_free -= len + ਸੂਚਕਾਂਕ_ਮਾਪ;
	    dp_ਸੱਜਾ->db_ਸੂਚਕਾਂਕ[0] = dp_ਸੱਜਾ->db_txt_ਸ਼ੁਰੂ;
	    ਜੇ (flags & ML_APPEND_MARK)
		dp_ਸੱਜਾ->db_ਸੂਚਕਾਂਕ[0] |= DB_MARKED;

	    mch_memmove((ਅੱਖਰ *)dp_ਸੱਜਾ + dp_ਸੱਜਾ->db_txt_ਸ਼ੁਰੂ,
							   ਸਤਰ, (size_t)len);
	    ++ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	ਜੇ (ਸਤਰਾਂ_moved)
	{
	    dp_ਸੱਜਾ->db_txt_ਸ਼ੁਰੂ -= ਅੰਕੜੇ_moved;
	    dp_ਸੱਜਾ->db_free -= ਕੁੱਲ_moved;
	    mch_memmove((ਅੱਖਰ *)dp_ਸੱਜਾ + dp_ਸੱਜਾ->db_txt_ਸ਼ੁਰੂ,
			(ਅੱਖਰ *)dp_ਖੱਬਾ + dp_ਖੱਬਾ->db_txt_ਸ਼ੁਰੂ,
			(size_t)ਅੰਕੜੇ_moved);
	    ਵਿੱਥ = dp_ਸੱਜਾ->db_txt_ਸ਼ੁਰੂ - dp_ਖੱਬਾ->db_txt_ਸ਼ੁਰੂ;
	    dp_ਖੱਬਾ->db_txt_ਸ਼ੁਰੂ += ਅੰਕੜੇ_moved;
	    dp_ਖੱਬਾ->db_free += ਕੁੱਲ_moved;

	    /*
	     * update indexes in the new block
	     */
	    ਲਈ (ਮੰਜ਼ਿਲ = ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ, from = db_idx + 1;
					 from < ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ; ++from, ++ਮੰਜ਼ਿਲ)
		dp_ਸੱਜਾ->db_ਸੂਚਕਾਂਕ[ਮੰਜ਼ਿਲ] = dp->db_ਸੂਚਕਾਂਕ[from] + ਵਿੱਥ;
	    ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ += ਸਤਰਾਂ_moved;
	    ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ -= ਸਤਰਾਂ_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	ਜੇ (ਵਿੱਚ_ਖੱਬਾ)
	{
	    dp_ਖੱਬਾ->db_txt_ਸ਼ੁਰੂ -= len;
	    dp_ਖੱਬਾ->db_free -= len + ਸੂਚਕਾਂਕ_ਮਾਪ;
	    dp_ਖੱਬਾ->db_ਸੂਚਕਾਂਕ[ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ] = dp_ਖੱਬਾ->db_txt_ਸ਼ੁਰੂ;
	    ਜੇ (flags & ML_APPEND_MARK)
		dp_ਖੱਬਾ->db_ਸੂਚਕਾਂਕ[ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ] |= DB_MARKED;
	    mch_memmove((ਅੱਖਰ *)dp_ਖੱਬਾ + dp_ਖੱਬਾ->db_txt_ਸ਼ੁਰੂ,
							   ਸਤਰ, (size_t)len);
	    ++ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ;
	}

	ਜੇ (db_idx < 0)		// left block is new
	{
	    lnum_ਖੱਬਾ = lnum + 1;
	    lnum_ਸੱਜਾ = 0;
	}
	ਨਹੀਂਤਾਂ			// right block is new
	{
	    lnum_ਖੱਬਾ = 0;
	    ਜੇ (ਵਿੱਚ_ਖੱਬਾ)
		lnum_ਸੱਜਾ = lnum + 2;
	    ਨਹੀਂਤਾਂ
		lnum_ਸੱਜਾ = lnum + 1;
	}
	dp_ਖੱਬਾ->db_ਸਤਰ_ਗਿਣਤੀ = ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ;
	dp_ਸੱਜਾ->db_ਸਤਰ_ਗਿਣਤੀ = ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ;

	/*
	 * release the two data blocks
	 * The new one (hp_new) already has a correct blocknumber.
	 * The old one (hp, in ml_locked) gets a positive blocknumber if
	 * we changed it and we are not editing a new file.
	 */
	ਜੇ (ਸਤਰਾਂ_moved || ਵਿੱਚ_ਖੱਬਾ)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	ਜੇ (!(flags & ML_APPEND_NEW) && db_idx >= 0 && ਵਿੱਚ_ਖੱਬਾ)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_ਲੱਭੋ_ਸਤਰ(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	ਲਈ (ਢੇਰ_idx = buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ - 1; ਢੇਰ_idx >= 0;
								  --ਢੇਰ_idx)
	{
	    ip = &(buf->b_ml.ml_ਢੇਰ[ਢੇਰ_idx]);
	    pb_idx = ip->ip_ਸੂਚਕਾਂਕ;
	    ਜੇ ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ਨਿਰਾ)
		ਜਾਓ theend;
	    pp = (PTR_BL *)(hp->bh_ਅੰਕੜੇ);   // must be pointer block
	    ਜੇ (pp->pb_id != PTR_ID)
	    {
		iemsg(e_ਸੂਚਕ_ਖੰਡ_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		ਜਾਓ theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    ਜੇ (pp->pb_ਗਿਣਤੀ < pp->pb_ਗਿਣਤੀ_max)
	    {
		ਜੇ (pb_idx + 1 < (ਪੂਰਨ)pp->pb_ਗਿਣਤੀ)
		    mch_memmove(&pp->pb_ਸੂਚਕ[pb_idx + 2],
				&pp->pb_ਸੂਚਕ[pb_idx + 1],
			(size_t)(pp->pb_ਗਿਣਤੀ - pb_idx - 1) * ਆਕਾਰ(PTR_EN));
		++pp->pb_ਗਿਣਤੀ;
		pp->pb_ਸੂਚਕ[pb_idx].pe_ਸਤਰ_ਗਿਣਤੀ = ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ;
		pp->pb_ਸੂਚਕ[pb_idx].pe_bnum = bnum_ਖੱਬਾ;
		pp->pb_ਸੂਚਕ[pb_idx].pe_ਪੰਨਾ_ਗਿਣਤੀ = ਪੰਨਾ_ਗਿਣਤੀ_ਖੱਬਾ;
		pp->pb_ਸੂਚਕ[pb_idx + 1].pe_ਸਤਰ_ਗਿਣਤੀ = ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ;
		pp->pb_ਸੂਚਕ[pb_idx + 1].pe_bnum = bnum_ਸੱਜਾ;
		pp->pb_ਸੂਚਕ[pb_idx + 1].pe_ਪੰਨਾ_ਗਿਣਤੀ = ਪੰਨਾ_ਗਿਣਤੀ_ਸੱਜਾ;

		ਜੇ (lnum_ਖੱਬਾ != 0)
		    pp->pb_ਸੂਚਕ[pb_idx].pe_old_lnum = lnum_ਖੱਬਾ;
		ਜੇ (lnum_ਸੱਜਾ != 0)
		    pp->pb_ਸੂਚਕ[pb_idx + 1].pe_old_lnum = lnum_ਸੱਜਾ;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = ਢੇਰ_idx + 1;	    // truncate stack

		ਜੇ (lineadd)
		{
		    --(buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ);
		    // fix line count for rest of blocks in the stack
		    ml_lineadd(buf, lineadd);
							// fix stack itself
		    buf->b_ml.ml_ਢੇਰ[buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ].ip_ਉੱਚਾ +=
								      lineadd;
		    ++(buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ);
		}

		/*
		 * We are finished, break the loop here.
		 */
		ਰੋਕੋ;
	    }
	    // pointer block full
	    /*
	     * split the pointer block
	     * allocate a new pointer block
	     * move some of the pointer into the new block
	     * prepare for updating the parent block
	     */
	    ਲਈ (;;)	// do this twice when splitting block 1
	    {
		hp_new = ml_new_ptr(mfp);
		ਜੇ (hp_new == ਨਿਰਾ)	    // TODO: try to fix tree
		    ਜਾਓ theend;
		pp_new = (PTR_BL *)(hp_new->bh_ਅੰਕੜੇ);

		ਜੇ (hp->bh_bnum != 1)
		    ਰੋਕੋ;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (size_t)ਪੰਨਾ_ਮਾਪ);
		pp->pb_ਗਿਣਤੀ = 1;
		pp->pb_ਸੂਚਕ[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_ਸੂਚਕ[0].pe_ਸਤਰ_ਗਿਣਤੀ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
		pp->pb_ਸੂਚਕ[0].pe_old_lnum = 1;
		pp->pb_ਸੂਚਕ[0].pe_ਪੰਨਾ_ਗਿਣਤੀ = 1;
		mf_put(mfp, hp, TRUE, FALSE);   // release block 1
		hp = hp_new;		// new block is to be split
		pp = pp_new;
		CHECK(ਢੇਰ_idx != 0, _("stack_idx should be 0"));
		ip->ip_ਸੂਚਕਾਂਕ = 0;
		++ਢੇਰ_idx;	// do block 1 again later
	    }
	    /*
	     * move the pointers after the current one to the new block
	     * If there are none, the new entry will be in the new block.
	     */
	    ਕੁੱਲ_moved = pp->pb_ਗਿਣਤੀ - pb_idx - 1;
	    ਜੇ (ਕੁੱਲ_moved)
	    {
		mch_memmove(&pp_new->pb_ਸੂਚਕ[0],
				&pp->pb_ਸੂਚਕ[pb_idx + 1],
				(size_t)(ਕੁੱਲ_moved) * ਆਕਾਰ(PTR_EN));
		pp_new->pb_ਗਿਣਤੀ = ਕੁੱਲ_moved;
		pp->pb_ਗਿਣਤੀ -= ਕੁੱਲ_moved - 1;
		pp->pb_ਸੂਚਕ[pb_idx + 1].pe_bnum = bnum_ਸੱਜਾ;
		pp->pb_ਸੂਚਕ[pb_idx + 1].pe_ਸਤਰ_ਗਿਣਤੀ = ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ;
		pp->pb_ਸੂਚਕ[pb_idx + 1].pe_ਪੰਨਾ_ਗਿਣਤੀ = ਪੰਨਾ_ਗਿਣਤੀ_ਸੱਜਾ;
		ਜੇ (lnum_ਸੱਜਾ)
		    pp->pb_ਸੂਚਕ[pb_idx + 1].pe_old_lnum = lnum_ਸੱਜਾ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		pp_new->pb_ਗਿਣਤੀ = 1;
		pp_new->pb_ਸੂਚਕ[0].pe_bnum = bnum_ਸੱਜਾ;
		pp_new->pb_ਸੂਚਕ[0].pe_ਸਤਰ_ਗਿਣਤੀ = ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ;
		pp_new->pb_ਸੂਚਕ[0].pe_ਪੰਨਾ_ਗਿਣਤੀ = ਪੰਨਾ_ਗਿਣਤੀ_ਸੱਜਾ;
		pp_new->pb_ਸੂਚਕ[0].pe_old_lnum = lnum_ਸੱਜਾ;
	    }
	    pp->pb_ਸੂਚਕ[pb_idx].pe_bnum = bnum_ਖੱਬਾ;
	    pp->pb_ਸੂਚਕ[pb_idx].pe_ਸਤਰ_ਗਿਣਤੀ = ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ;
	    pp->pb_ਸੂਚਕ[pb_idx].pe_ਪੰਨਾ_ਗਿਣਤੀ = ਪੰਨਾ_ਗਿਣਤੀ_ਖੱਬਾ;
	    ਜੇ (lnum_ਖੱਬਾ)
		pp->pb_ਸੂਚਕ[pb_idx].pe_old_lnum = lnum_ਖੱਬਾ;
	    lnum_ਖੱਬਾ = 0;
	    lnum_ਸੱਜਾ = 0;

	    /*
	     * recompute line counts
	     */
	    ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ = 0;
	    ਲਈ (i = 0; i < (ਪੂਰਨ)pp_new->pb_ਗਿਣਤੀ; ++i)
		ਸਤਰ_ਗਿਣਤੀ_ਸੱਜਾ += pp_new->pb_ਸੂਚਕ[i].pe_ਸਤਰ_ਗਿਣਤੀ;
	    ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ = 0;
	    ਲਈ (i = 0; i < (ਪੂਰਨ)pp->pb_ਗਿਣਤੀ; ++i)
		ਸਤਰ_ਗਿਣਤੀ_ਖੱਬਾ += pp->pb_ਸੂਚਕ[i].pe_ਸਤਰ_ਗਿਣਤੀ;

	    bnum_ਖੱਬਾ = hp->bh_bnum;
	    bnum_ਸੱਜਾ = hp_new->bh_bnum;
	    ਪੰਨਾ_ਗਿਣਤੀ_ਖੱਬਾ = 1;
	    ਪੰਨਾ_ਗਿਣਤੀ_ਸੱਜਾ = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	ਜੇ (ਢੇਰ_idx < 0)
	{
	    iemsg(e_updated_too_many_ਖੰਡਾਂ);
	    buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;	// invalidate stack
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (ਲੰਮਾ)ਪਾਠ_len
# ਨਹੀਂਤਾਂ
	    (ਲੰਮਾ)len
# endif
	    , ML_CHNK_ADDLINE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    ਜੇ (!(flags & ML_APPEND_NEW) && netbeans_ਸਰਗਰਮ())
    {
	ਪੂਰਨ ਸਤਰ_len = (ਪੂਰਨ)STRLEN(ਸਤਰ);
	ਜੇ (ਸਤਰ_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, ਸਤਰ, ਸਤਰ_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)ਸਤਰ_len, (char_u *)"\n", 1);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਜੇ (!(flags & ML_APPEND_NEW) && buf->b_write_ਮੰਜ਼ਿਲ_channel)
	channel_write_new_ਸਤਰਾਂ(buf);
#ਜੇ_ਅੰਤ
    ret = OK;

theend:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    vim_free(tofree);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    ਸਥਿਰ ਪੂਰਨ
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ਸਤਰ,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    ਪੂਰਨ		flags)		// ML_APPEND_ flags
{
    ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ FAIL;  // lnum out of range

    ਜੇ (buf->b_ml.ml_ਸਤਰ_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_ਸਤਰ(buf);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	ਜੇ (buf->b_ml.ml_ਸਤਰ_lnum != 0)
	    ml_flush_ਸਤਰ(buf);
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ml_append_int(buf, lnum, ਸਤਰ, len, flags);
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
    ਪੂਰਨ
ml_append(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ਸਤਰ,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    ਪੂਰਨ		newfile)	// flag, see above
{
    ਵਾਪਸ ml_append_flags(lnum, ਸਤਰ, len, newfile ? ML_APPEND_NEW : 0);
}

    ਪੂਰਨ
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ਸਤਰ,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    ਪੂਰਨ		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    ਜੇ (curbuf->b_ml.ml_mfp == ਨਿਰਾ && open_ਬਫਰ(FALSE, ਨਿਰਾ, 0) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ ml_append_flush(curbuf, lnum, ਸਤਰ, len, flags);
}


#ਜੇ defined(FEAT_SPELL) || defined(FEAT_QUICKFIX) || defined(FEAT_PROP_POPUP)

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
    ਪੂਰਨ
ml_append_buf(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ਸਤਰ,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    ਪੂਰਨ		newfile)	// flag, see above
{
    ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ ml_append_flush(buf, lnum, ਸਤਰ, len, newfile ? ML_APPEND_NEW : 0);
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ
ml_replace(linenr_T lnum, char_u *ਸਤਰ, ਪੂਰਨ copy)
{
    colnr_T len = -1;

    ਜੇ (ਸਤਰ != ਨਿਰਾ)
	len = (colnr_T)STRLEN(ਸਤਰ);
    ਵਾਪਸ ml_replace_len(lnum, ਸਤਰ, len, FALSE, copy);
}

/*
 * Replace a line for the current buffer.  Like ml_replace() with:
 * "len_arg" is the length of the text, excluding NUL.
 * If "has_props" is TRUE then "line_arg" includes the text properties and
 * "len_arg" includes the NUL of the text and text properties.
 * When "copy" is TRUE copy the text into allocated memory, otherwise
 * "line_arg" must be allocated and will be consumed here.
 */
    ਪੂਰਨ
ml_replace_len(
	linenr_T    lnum,
	char_u	    *ਸਤਰ_arg,
	colnr_T	    len_arg,
	ਪੂਰਨ	    has_props,
	ਪੂਰਨ	    copy)
{
    char_u *ਸਤਰ = ਸਤਰ_arg;
    colnr_T len = len_arg;

    ਜੇ (ਸਤਰ == ਨਿਰਾ)		// just checking...
	ਵਾਪਸ FAIL;

    // When starting up, we might still need to create the memfile
    ਜੇ (curbuf->b_ml.ml_mfp == ਨਿਰਾ && open_ਬਫਰ(FALSE, ਨਿਰਾ, 0) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (!has_props)
	++len;  // include the NUL after the text
    ਜੇ (copy)
    {
	// copy the line to allocated memory
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (has_props)
	    ਸਤਰ = vim_memsave(ਸਤਰ, len);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ਸਤਰ = vim_strnsave(ਸਤਰ, len - 1);
	ਜੇ (ਸਤਰ == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    ਜੇ (netbeans_ਸਰਗਰਮ())
    {
	netbeans_removed(curbuf, lnum, 0, (ਲੰਮਾ)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, ਸਤਰ, (ਪੂਰਨ)STRLEN(ਸਤਰ));
    }
#ਜੇ_ਅੰਤ
    ਜੇ (curbuf->b_ml.ml_ਸਤਰ_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_ਸਤਰ(curbuf);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (curbuf->b_has_textprop && !has_props)
    {
	size_t	oldtextlen = STRLEN(curbuf->b_ml.ml_ਸਤਰ_ptr) + 1;

	ਜੇ (oldtextlen < (size_t)curbuf->b_ml.ml_ਸਤਰ_len)
	{
	    char_u *newline;
	    size_t textproplen = curbuf->b_ml.ml_ਸਤਰ_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    ਜੇ (newline != ਨਿਰਾ)
	    {
		mch_memmove(newline, ਸਤਰ, len);
		mch_memmove(newline + len, curbuf->b_ml.ml_ਸਤਰ_ptr
						    + oldtextlen, textproplen);
		vim_free(ਸਤਰ);
		ਸਤਰ = newline;
		len += (colnr_T)textproplen;
	    }
	}
    }
#ਜੇ_ਅੰਤ

    ਜੇ (curbuf->b_ml.ml_flags & (ML_ਸਤਰ_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_ਸਤਰ_ptr);	// free allocated line

    curbuf->b_ml.ml_ਸਤਰ_ptr = ਸਤਰ;
    curbuf->b_ml.ml_ਸਤਰ_len = len;
    curbuf->b_ml.ml_ਸਤਰ_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_ਸਤਰ_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_ਸਤਰ_DIRTY) & ~ML_ਖਾਲੀ_ਥਾਵਾਂ;

    ਵਾਪਸ OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
/*
 * Adjust text properties in line "lnum" for a deleted line.
 * When "above" is true this is the line above the deleted line, otherwise this
 * is the line below the deleted line.
 * "del_props[del_props_len]" are the properties of the deleted line.
 */
    ਸਥਿਰ ਖਾਲੀ
adjust_ਪਾਠ_props_for_delete(
	buf_T	    *buf,
	linenr_T    lnum,
	char_u	    *del_props,
	ਪੂਰਨ	    del_props_len,
	ਪੂਰਨ	    above)
{
    ਪੂਰਨ		did_get_ਸਤਰ = FALSE;
    ਪੂਰਨ		ਪੂਰਾ_del;
    ਪੂਰਨ		ਪੂਰਾ_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    ਅੰਕੜੇ_BL	*dp;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		ਸਤਰ_ਸ਼ੁਰੂ;
    ਲੰਮਾ	ਸਤਰ_ਮਾਪ;
    ਪੂਰਨ		this_props_len = 0;
    char_u	*ਪਾਠ;
    size_t	textlen;
    ਪੂਰਨ		ਮਿਲਿਆ;

    ਲਈ (ਪੂਰਾ_del = 0; ਪੂਰਾ_del < del_props_len; ਪੂਰਾ_del += ਆਕਾਰ(textprop_T))
    {
	mch_memmove(&prop_del, del_props + ਪੂਰਾ_del, ਆਕਾਰ(textprop_T));
	ਜੇ ((above && (prop_del.tp_flags & TP_FLAG_CONT_PREV)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_ਅਗਲਾ))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_ਅਗਲਾ)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_PREV)))
	{
	    ਜੇ (!did_get_ਸਤਰ)
	    {
		did_get_ਸਤਰ = TRUE;
		ਜੇ ((hp = ml_ਲੱਭੋ_ਸਤਰ(buf, lnum, ML_ਲੱਭੋ)) == ਨਿਰਾ)
		    ਵਾਪਸ;

		dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
		idx = lnum - buf->b_ml.ml_locked_ਨੀਵਾਂ;
		ਸਤਰ_ਸ਼ੁਰੂ = ((dp->db_ਸੂਚਕਾਂਕ[idx]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
		ਜੇ (idx == 0)		// first line in block, text at the end
		    ਸਤਰ_ਮਾਪ = dp->db_txt_ਸਿਰਾ - ਸਤਰ_ਸ਼ੁਰੂ;
		ਨਹੀਂਤਾਂ
		    ਸਤਰ_ਮਾਪ = ((dp->db_ਸੂਚਕਾਂਕ[idx - 1]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ)
								  - ਸਤਰ_ਸ਼ੁਰੂ;
		ਪਾਠ = (char_u *)dp + ਸਤਰ_ਸ਼ੁਰੂ;
		textlen = STRLEN(ਪਾਠ) + 1;
		ਜੇ ((ਲੰਮਾ)textlen >= ਸਤਰ_ਮਾਪ)
		{
		    // No properties on this line.
		    ਜੇ (above)
			ਅੰਦਰੂਨੀ_error("no text property above deleted line");
		    ਨਹੀਂਤਾਂ
			ਅੰਦਰੂਨੀ_error("no text property below deleted line");
		    ਵਾਪਸ;
		}
		ਜੇ ((ਲੰਮਾ)textlen + (ਲੰਮਾ)PROP_ਗਿਣਤੀ_ਮਾਪ > ਸਤਰ_ਮਾਪ)
		{
		    ਅੰਦਰੂਨੀ_error("text property data too short");
		    ਵਾਪਸ;
		}

		uint16_t pc;

		mch_memmove(&pc, ਪਾਠ + textlen, PROP_ਗਿਣਤੀ_ਮਾਪ);
		ਜੇ (!ਪਾਠ_prop_ਗਿਣਤੀ_valid(pc, (size_t)(ਸਤਰ_ਮਾਪ - (ਲੰਮਾ)textlen)))
		{
		    ਅੰਦਰੂਨੀ_error("text property count too large");
		    ਵਾਪਸ;
		}
		this_props_len = pc * (ਪੂਰਨ)ਆਕਾਰ(textprop_T);
	    }

	    ਮਿਲਿਆ = FALSE;
	    {
		char_u *props_ਸ਼ੁਰੂ = ਪਾਠ + textlen + PROP_ਗਿਣਤੀ_ਮਾਪ;

		ਲਈ (ਪੂਰਾ_this = 0; ਪੂਰਾ_this < this_props_len;
					       ਪੂਰਾ_this += ਆਕਾਰ(textprop_T))
		{
		    ਪੂਰਨ		flag = above ? TP_FLAG_CONT_ਅਗਲਾ
							   : TP_FLAG_CONT_PREV;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_ਸ਼ੁਰੂ + ਪੂਰਾ_this,
							   ਆਕਾਰ(textprop_T));
		    ਜੇ ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			ਮਿਲਿਆ = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_ਸ਼ੁਰੂ + ਪੂਰਾ_this, &prop_this,
							   ਆਕਾਰ(textprop_T));
			ਰੋਕੋ;
		    }
		}
	    }
	    ਜੇ (!ਮਿਲਿਆ)
	    {
		ਜੇ (above)
		    ਅੰਦਰੂਨੀ_error("text property above deleted line not found");
		ਨਹੀਂਤਾਂ
		    ਅੰਦਰੂਨੀ_error("text property below deleted line not found");
	    }

	    buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
	}
    }
}
#ਜੇ_ਅੰਤ

/*
 * Delete line "lnum" in the current buffer.
 * When "flags" has ML_DEL_MESSAGE may give a "No lines in buffer" message.
 * When "flags" has ML_DEL_UNDO this is called from undo.
 *
 * return FAIL for failure, OK otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
ml_delete_int(buf_T *buf, linenr_T lnum, ਪੂਰਨ flags)
{
    bhdr_T	*hp;
    memfile_T	*mfp;
    ਅੰਕੜੇ_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    ਪੂਰਨ		ਗਿਣਤੀ_2;	    // number of entries in block
    ਪੂਰਨ		idx;
    ਪੂਰਨ		ਢੇਰ_idx;
    ਪੂਰਨ		ਪਾਠ_ਸ਼ੁਰੂ;
    ਪੂਰਨ		ਸਤਰ_ਸ਼ੁਰੂ;
    ਲੰਮਾ	ਸਤਰ_ਮਾਪ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ret = FAIL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    char_u	*textprop_save = ਨਿਰਾ;
    ਲੰਮਾ	textprop_len = 0;
#ਜੇ_ਅੰਤ

    ਜੇ (ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked && ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked > lnum)
	ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    ਜੇ (buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ == 1)	    // file becomes empty
    {
	ਜੇ ((flags & ML_DEL_ਸੁਨੇਹਾ)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_ਸਤਰਾਂ
#ਜੇ_ਅੰਤ
	   )
	    ਸੈੱਟ_keep_ਸੁਨੇਹਾ((char_u *)_(no_ਸਤਰਾਂ_ਸੁਨੇਹਾ), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_ਖਾਲੀ_ਥਾਵਾਂ;

	ਵਾਪਸ i;
    }

/*
 * Find the data block containing the line.
 * This also fills the stack with the blocks from the root to the data block.
 * This also releases any locked block..
 */
    mfp = buf->b_ml.ml_mfp;
    ਜੇ (mfp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ ((hp = ml_ਲੱਭੋ_ਸਤਰ(buf, lnum, ML_DELETE)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
    // compute line count before the delete
    ਗਿਣਤੀ_2 = (ਲੰਮਾ)(buf->b_ml.ml_locked_ਉੱਚਾ)
					- (ਲੰਮਾ)(buf->b_ml.ml_locked_ਨੀਵਾਂ) + 2;
    idx = lnum - buf->b_ml.ml_locked_ਨੀਵਾਂ;

    --buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਸਤਰ_ਸ਼ੁਰੂ = ((dp->db_ਸੂਚਕਾਂਕ[idx]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
    ਜੇ (idx == 0)		// first line in block, text at the end
	ਸਤਰ_ਮਾਪ = dp->db_txt_ਸਿਰਾ - ਸਤਰ_ਸ਼ੁਰੂ;
    ਨਹੀਂਤਾਂ
	ਸਤਰ_ਮਾਪ = ((dp->db_ਸੂਚਕਾਂਕ[idx - 1]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ) - ਸਤਰ_ਸ਼ੁਰੂ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    ਜੇ (netbeans_ਸਰਗਰਮ())
	netbeans_removed(buf, lnum, 0, ਸਤਰ_ਮਾਪ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    ਜੇ (buf->b_has_textprop)
    {
	size_t	textlen = STRLEN((char_u *)dp + ਸਤਰ_ਸ਼ੁਰੂ) + 1;

	textprop_len = ਸਤਰ_ਮਾਪ - (ਲੰਮਾ)textlen;
	ਜੇ (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + ਸਤਰ_ਸ਼ੁਰੂ + textlen,
								 textprop_len);
    }
#ਜੇ_ਅੰਤ

/*
 * special case: If there is only one line in the data block it becomes empty.
 * Then we have to remove the entry, pointing to this data block, from the
 * pointer block. If this pointer block also becomes empty, we go up another
 * block, and so on, up to the root if necessary.
 * The line counts in the pointer blocks have already been adjusted by
 * ml_find_line().
 */
    ਜੇ (ਗਿਣਤੀ_2 == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = ਨਿਰਾ;

	ਲਈ (ਢੇਰ_idx = buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ - 1; ਢੇਰ_idx >= 0;
								  --ਢੇਰ_idx)
	{
	    buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_ਢੇਰ[ਢੇਰ_idx]);
	    idx = ip->ip_ਸੂਚਕਾਂਕ;
	    ਜੇ ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ਨਿਰਾ)
		ਜਾਓ theend;
	    pp = (PTR_BL *)(hp->bh_ਅੰਕੜੇ);   // must be pointer block
	    ਜੇ (pp->pb_id != PTR_ID)
	    {
		iemsg(e_ਸੂਚਕ_ਖੰਡ_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		ਜਾਓ theend;
	    }
	    ਗਿਣਤੀ_2 = --(pp->pb_ਗਿਣਤੀ);
	    ਜੇ (ਗਿਣਤੀ_2 == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (ਗਿਣਤੀ_2 != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_ਸੂਚਕ[idx], &pp->pb_ਸੂਚਕ[idx + 1],
				      (size_t)(ਗਿਣਤੀ_2 - idx) * ਆਕਾਰ(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = ਢੇਰ_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		ਜੇ (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_ਢੇਰ[buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ].ip_ਉੱਚਾ +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ);

		ਰੋਕੋ;
	    }
	}
	CHECK(ਢੇਰ_idx < 0, _("deleted block 1?"));
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	ਪਾਠ_ਸ਼ੁਰੂ = dp->db_txt_ਸ਼ੁਰੂ;
	mch_memmove((ਅੱਖਰ *)dp + ਪਾਠ_ਸ਼ੁਰੂ + ਸਤਰ_ਮਾਪ,
		  (ਅੱਖਰ *)dp + ਪਾਠ_ਸ਼ੁਰੂ, (size_t)(ਸਤਰ_ਸ਼ੁਰੂ - ਪਾਠ_ਸ਼ੁਰੂ));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	ਲਈ (i = idx; i < ਗਿਣਤੀ_2 - 1; ++i)
	    dp->db_ਸੂਚਕਾਂਕ[i] = dp->db_ਸੂਚਕਾਂਕ[i + 1] + ਸਤਰ_ਮਾਪ;

	dp->db_free += ਸਤਰ_ਮਾਪ + ਸੂਚਕਾਂਕ_ਮਾਪ;
	dp->db_txt_ਸ਼ੁਰੂ += ਸਤਰ_ਮਾਪ;
	--(dp->db_ਸਤਰ_ਗਿਣਤੀ);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, ਸਤਰ_ਮਾਪ
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#ਜੇ_ਅੰਤ
    ret = OK;

theend:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (textprop_save != ਨਿਰਾ)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_ਅੰਕੜੇ;
	ਪੂਰਨ	    props_ਬਾਈਟਾਂ;

	mch_memmove(&pc, textprop_save, PROP_ਗਿਣਤੀ_ਮਾਪ);
	props_ਅੰਕੜੇ = textprop_save + PROP_ਗਿਣਤੀ_ਮਾਪ;
	props_ਬਾਈਟਾਂ = pc * (ਪੂਰਨ)ਆਕਾਰ(textprop_T);
	ਜੇ (!ਪਾਠ_prop_ਗਿਣਤੀ_valid(pc, (size_t)textprop_len))
	    props_ਬਾਈਟਾਂ = 0;

	// Adjust text properties in the line above and below.
	ਜੇ (lnum > 1)
	    adjust_ਪਾਠ_props_for_delete(buf, lnum - 1,
					     props_ਅੰਕੜੇ, props_ਬਾਈਟਾਂ, TRUE);
	ਜੇ (lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    adjust_ਪਾਠ_props_for_delete(buf, lnum,
					    props_ਅੰਕੜੇ, props_ਬਾਈਟਾਂ, FALSE);
    }
    vim_free(textprop_save);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ret;
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
    ਪੂਰਨ
ml_delete(linenr_T lnum)
{
    ਵਾਪਸ ml_delete_flags(lnum, 0);
}

/*
 * Like ml_delete() but using flags (see ml_delete_int()).
 */
    ਪੂਰਨ
ml_delete_flags(linenr_T lnum, ਪੂਰਨ flags)
{
    ml_flush_ਸਤਰ(curbuf);
    ਜੇ (lnum < 1 || lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਵਾਪਸ FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ml_delete_int(curbuf, lnum, flags);
}

/*
 * set the DB_MARKED flag for line 'lnum'
 */
    ਖਾਲੀ
ml_setmarked(linenr_T lnum)
{
    bhdr_T    *hp;
    ਅੰਕੜੇ_BL *dp;
				    // invalid line number
    ਜੇ (lnum < 1 || lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
					       || curbuf->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ;			    // give error message?

    ਜੇ (ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked == 0 || ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked > lnum)
	ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    ਜੇ ((hp = ml_ਲੱਭੋ_ਸਤਰ(curbuf, lnum, ML_ਲੱਭੋ)) == ਨਿਰਾ)
	ਵਾਪਸ;		    // give error message?

    dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
    dp->db_ਸੂਚਕਾਂਕ[lnum - curbuf->b_ml.ml_locked_ਨੀਵਾਂ] |= DB_MARKED;
    curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
}

/*
 * find the first line with its DB_MARKED flag set
 */
    linenr_T
ml_firstmarked(ਖਾਲੀ)
{
    bhdr_T	*hp;
    ਅੰਕੜੇ_BL	*dp;
    linenr_T	lnum;
    ਪੂਰਨ		i;

    ਜੇ (curbuf->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    ਲਈ (lnum = ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked; lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	ਜੇ ((hp = ml_ਲੱਭੋ_ਸਤਰ(curbuf, lnum, ML_ਲੱਭੋ)) == ਨਿਰਾ)
	    ਵਾਪਸ (linenr_T)0;		    // give error message?

	dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);

	ਲਈ (i = lnum - curbuf->b_ml.ml_locked_ਨੀਵਾਂ;
			    lnum <= curbuf->b_ml.ml_locked_ਉੱਚਾ; ++i, ++lnum)
	    ਜੇ ((dp->db_ਸੂਚਕਾਂਕ[i]) & DB_MARKED)
	    {
		(dp->db_ਸੂਚਕਾਂਕ[i]) &= DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked = lnum + 1;
		ਵਾਪਸ lnum;
	    }
    }

    ਵਾਪਸ (linenr_T) 0;
}

/*
 * clear all DB_MARKED flags
 */
    ਖਾਲੀ
ml_clearmarked(ਖਾਲੀ)
{
    bhdr_T	*hp;
    ਅੰਕੜੇ_BL	*dp;
    linenr_T	lnum;
    ਪੂਰਨ		i;

    ਜੇ (curbuf->b_ml.ml_mfp == ਨਿਰਾ)	    // nothing to do
	ਵਾਪਸ;

    /*
     * The search starts with line lowest_marked.
     */
    ਲਈ (lnum = ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked; lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	ਜੇ ((hp = ml_ਲੱਭੋ_ਸਤਰ(curbuf, lnum, ML_ਲੱਭੋ)) == ਨਿਰਾ)
	    ਵਾਪਸ;		// give error message?

	dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);

	ਲਈ (i = lnum - curbuf->b_ml.ml_locked_ਨੀਵਾਂ;
			    lnum <= curbuf->b_ml.ml_locked_ਉੱਚਾ; ++i, ++lnum)
	    ਜੇ ((dp->db_ਸੂਚਕਾਂਕ[i]) & DB_MARKED)
	    {
		(dp->db_ਸੂਚਕਾਂਕ[i]) &= DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	    }
    }

    ਸਭ_ਤੋਂ_ਨੀਵਾਂ_marked = 0;
}

/*
 * flush ml_line if necessary
 */
    ਸਥਿਰ ਖਾਲੀ
ml_flush_ਸਤਰ(buf_T *buf)
{
    bhdr_T	*hp;
    ਅੰਕੜੇ_BL	*dp;
    linenr_T	lnum;
    char_u	*new_ਸਤਰ;
    char_u	*old_ਸਤਰ;
    colnr_T	new_len;
    ਪੂਰਨ		old_len;
    ਪੂਰਨ		extra;
    ਪੂਰਨ		idx;
    ਪੂਰਨ		ਸ਼ੁਰੂ;
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    ਪੂਰਨ		i;
    ਸਥਿਰ ਪੂਰਨ  entered = FALSE;

    ਜੇ (buf->b_ml.ml_ਸਤਰ_lnum == 0 || buf->b_ml.ml_mfp == ਨਿਰਾ)
	ਵਾਪਸ;		// nothing to do

    ਜੇ (buf->b_ml.ml_flags & ML_ਸਤਰ_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	ਜੇ (entered)
	    ਵਾਪਸ;
	entered = TRUE;

	lnum = buf->b_ml.ml_ਸਤਰ_lnum;
	new_ਸਤਰ = buf->b_ml.ml_ਸਤਰ_ptr;

	hp = ml_ਲੱਭੋ_ਸਤਰ(buf, lnum, ML_ਲੱਭੋ);
	ਜੇ (hp == ਨਿਰਾ)
	    siemsg(e_cannot_ਲੱਭੋ_ਸਤਰ_nr, lnum);
	ਨਹੀਂਤਾਂ
	{
	    dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
	    idx = lnum - buf->b_ml.ml_locked_ਨੀਵਾਂ;
	    ਸ਼ੁਰੂ = ((dp->db_ਸੂਚਕਾਂਕ[idx]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
	    old_ਸਤਰ = (char_u *)dp + ਸ਼ੁਰੂ;
	    ਜੇ (idx == 0)	// line is last in block
		old_len = dp->db_txt_ਸਿਰਾ - ਸ਼ੁਰੂ;
	    ਨਹੀਂਤਾਂ		// text of previous line follows
		old_len = (dp->db_ਸੂਚਕਾਂਕ[idx - 1] & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ) - ਸ਼ੁਰੂ;
	    new_len = buf->b_ml.ml_ਸਤਰ_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    ਜੇ ((ਪੂਰਨ)dp->db_free >= extra)
	    {
#ਜੇ defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		ਪੂਰਨ old_prop_len = 0;
		ਜੇ (buf->b_has_textprop)
		    old_prop_len = old_len - (ਪੂਰਨ)STRLEN(old_ਸਤਰ) - 1;
#ਜੇ_ਅੰਤ
		// if the length changes and there are following lines
		ਗਿਣਤੀ_2 = buf->b_ml.ml_locked_ਉੱਚਾ - buf->b_ml.ml_locked_ਨੀਵਾਂ + 1;
		ਜੇ (extra != 0 && idx < ਗਿਣਤੀ_2 - 1)
		{
		    // move text of following lines
		    mch_memmove((ਅੱਖਰ *)dp + dp->db_txt_ਸ਼ੁਰੂ - extra,
				(ਅੱਖਰ *)dp + dp->db_txt_ਸ਼ੁਰੂ,
				(size_t)(ਸ਼ੁਰੂ - dp->db_txt_ਸ਼ੁਰੂ));

		    // adjust pointers of this and following lines
		    ਲਈ (i = idx + 1; i < ਗਿਣਤੀ_2; ++i)
			dp->db_ਸੂਚਕਾਂਕ[i] -= extra;
		}
		dp->db_ਸੂਚਕਾਂਕ[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_ਸ਼ੁਰੂ -= extra;

		// copy new line into the data block
		mch_memmove(old_ਸਤਰ - extra, new_ਸਤਰ, (size_t)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ਜੇ defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		ਜੇ (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (ਪੂਰਨ)STRLEN(new_ਸਤਰ) - 1;
		}
		ਜੇ (extra != 0)
		    ml_updatechunk(buf, lnum, (ਲੰਮਾ)extra, ML_CHNK_UPDLINE);
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
	    {
		/*
		 * Cannot do it in one data block: Delete and append.
		 * Append first, because ml_delete_int() cannot delete the
		 * last line in a buffer, which causes trouble for a buffer
		 * that has only one line.
		 * Don't forget to copy the mark!
		 */
		// How about handling errors???
		(ਖਾਲੀ)ml_append_int(buf, lnum, new_ਸਤਰ, new_len,
			 ((dp->db_ਸੂਚਕਾਂਕ[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#ਜੇ_ਅੰਤ
			 );
		(ਖਾਲੀ)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_ਸਤਰ);

	entered = FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_ਸਤਰ_ptr);

    buf->b_ml.ml_flags &= ~(ML_ਸਤਰ_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_ਸਤਰ_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    ਸਥਿਰ bhdr_T *
ml_new_ਅੰਕੜੇ(memfile_T *mfp, ਪੂਰਨ negative, ਪੂਰਨ ਪੰਨਾ_ਗਿਣਤੀ)
{
    bhdr_T	*hp;
    ਅੰਕੜੇ_BL	*dp;

    ਜੇ ((hp = mf_new(mfp, negative, ਪੰਨਾ_ਗਿਣਤੀ)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
    dp->db_id = ਅੰਕੜੇ_ID;
    dp->db_txt_ਸ਼ੁਰੂ = dp->db_txt_ਸਿਰਾ = ਪੰਨਾ_ਗਿਣਤੀ * mfp->mf_ਪੰਨਾ_ਮਾਪ;
    dp->db_free = dp->db_txt_ਸ਼ੁਰੂ - ਸਿਰਲੇਖ_ਮਾਪ;
    dp->db_ਸਤਰ_ਗਿਣਤੀ = 0;

    ਵਾਪਸ hp;
}

/*
 * create a new, empty, pointer block
 */
    ਸਥਿਰ bhdr_T *
ml_new_ptr(memfile_T *mfp)
{
    bhdr_T	*hp;
    PTR_BL	*pp;

    ਜੇ ((hp = mf_new(mfp, FALSE, 1)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    pp = (PTR_BL *)(hp->bh_ਅੰਕੜੇ);
    pp->pb_id = PTR_ID;
    pp->pb_ਗਿਣਤੀ = 0;
    pp->pb_ਗਿਣਤੀ_max = PB_ਗਿਣਤੀ_MAX(mfp);

    ਵਾਪਸ hp;
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
    ਸਥਿਰ bhdr_T *
ml_ਲੱਭੋ_ਸਤਰ(buf_T *buf, linenr_T lnum, ਪੂਰਨ action)
{
    ਅੰਕੜੇ_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    ਪੂਰਨ		dirty;
    linenr_T	ਨੀਵਾਂ, ਉੱਚਾ;
    ਪੂਰਨ		ਸਿਖਰ;
    ਪੂਰਨ		ਪੰਨਾ_ਗਿਣਤੀ;
    ਪੂਰਨ		idx;

    mfp = buf->b_ml.ml_mfp;

    /*
     * If there is a locked block check if the wanted line is in it.
     * If not, flush and release the locked block.
     * Don't do this for ML_INSERT_SAME, because the stack need to be updated.
     * Don't do this for ML_FLUSH, because we want to flush the locked block.
     * Don't do this when 'swapfile' is reset, we want to load all the blocks.
     */
    ਜੇ (buf->b_ml.ml_locked)
    {
	ਜੇ (ML_SIMPLE(action)
		&& buf->b_ml.ml_locked_ਨੀਵਾਂ <= lnum
		&& buf->b_ml.ml_locked_ਉੱਚਾ >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    ਜੇ (action == ML_ਪਾਓ)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_ਉੱਚਾ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_ਉੱਚਾ);
	    }
	    ਵਾਪਸ (buf->b_ml.ml_locked);
	}

	mf_put(mfp, buf->b_ml.ml_locked, buf->b_ml.ml_flags & ML_LOCKED_DIRTY,
					    buf->b_ml.ml_flags & ML_LOCKED_POS);
	buf->b_ml.ml_locked = ਨਿਰਾ;

	/*
	 * If lines have been added or deleted in the locked block, need to
	 * update the line count in pointer blocks.
	 */
	ਜੇ (buf->b_ml.ml_locked_lineadd != 0)
	    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
    }

    ਜੇ (action == ML_FLUSH)	    // nothing else to do
	ਵਾਪਸ ਨਿਰਾ;

    bnum = 1;			    // start at the root of the tree
    ਪੰਨਾ_ਗਿਣਤੀ = 1;
    ਨੀਵਾਂ = 1;
    ਉੱਚਾ = buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਜੇ (action == ML_ਲੱਭੋ)	// first try stack entries
    {
	ਲਈ (ਸਿਖਰ = buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ - 1; ਸਿਖਰ >= 0; --ਸਿਖਰ)
	{
	    ip = &(buf->b_ml.ml_ਢੇਰ[ਸਿਖਰ]);
	    ਜੇ (ip->ip_ਨੀਵਾਂ <= lnum && ip->ip_ਉੱਚਾ >= lnum)
	    {
		bnum = ip->ip_bnum;
		ਨੀਵਾਂ = ip->ip_ਨੀਵਾਂ;
		ਉੱਚਾ = ip->ip_ਉੱਚਾ;
		buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = ਸਿਖਰ;	// truncate stack at prev entry
		ਰੋਕੋ;
	    }
	}
	ਜੇ (ਸਿਖਰ < 0)
	    buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;		// not found, start at the root
    }
    ਨਹੀਂਤਾਂ	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    ਲਈ (;;)
    {
	ਜੇ ((hp = mf_get(mfp, bnum, ਪੰਨਾ_ਗਿਣਤੀ)) == ਨਿਰਾ)
	    ਜਾਓ error_noblock;

	/*
	 * update high for insert/delete
	 */
	ਜੇ (action == ML_ਪਾਓ)
	    ++ਉੱਚਾ;
	ਨਹੀਂਤਾਂ ਜੇ (action == ML_DELETE)
	    --ਉੱਚਾ;

	dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
	ਜੇ (dp->db_id == ਅੰਕੜੇ_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_ਨੀਵਾਂ = ਨੀਵਾਂ;
	    buf->b_ml.ml_locked_ਉੱਚਾ = ਉੱਚਾ;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    ਵਾਪਸ hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	ਜੇ (pp->pb_id != PTR_ID)
	{
	    iemsg(e_ਸੂਚਕ_ਖੰਡ_id_wrong);
	    ਜਾਓ error_ਖੰਡ;
	}

	ਜੇ ((ਸਿਖਰ = ml_ਜੋੜੋ_ਢੇਰ(buf)) < 0)	// add new entry to stack
	    ਜਾਓ error_ਖੰਡ;
	ip = &(buf->b_ml.ml_ਢੇਰ[ਸਿਖਰ]);
	ip->ip_bnum = bnum;
	ip->ip_ਨੀਵਾਂ = ਨੀਵਾਂ;
	ip->ip_ਉੱਚਾ = ਉੱਚਾ;
	ip->ip_ਸੂਚਕਾਂਕ = -1;		// index not known yet

	dirty = FALSE;
	ਲਈ (idx = 0; idx < (ਪੂਰਨ)pp->pb_ਗਿਣਤੀ; ++idx)
	{
	    t = pp->pb_ਸੂਚਕ[idx].pe_ਸਤਰ_ਗਿਣਤੀ;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    ਜੇ ((ਨੀਵਾਂ += t) > lnum)
	    {
		ip->ip_ਸੂਚਕਾਂਕ = idx;
		bnum = pp->pb_ਸੂਚਕ[idx].pe_bnum;
		ਪੰਨਾ_ਗਿਣਤੀ = pp->pb_ਸੂਚਕ[idx].pe_ਪੰਨਾ_ਗਿਣਤੀ;
		ਉੱਚਾ = ਨੀਵਾਂ - 1;
		ਨੀਵਾਂ -= t;

		/*
		 * a negative block number may have been changed
		 */
		ਜੇ (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    ਜੇ (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_ਸੂਚਕ[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		ਰੋਕੋ;
	    }
	}
	ਜੇ (idx >= (ਪੂਰਨ)pp->pb_ਗਿਣਤੀ)	    // past the end: something wrong!
	{
	    ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		siemsg(e_ਸਤਰ_ਸੰਖਿਆ_ਬਾਹਰਲਾ_ਦਾ_range_nr_past_the_ਸਿਰਾ,
					      lnum - buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);

	    ਨਹੀਂਤਾਂ
		siemsg(e_ਸਤਰ_ਗਿਣਤੀ_wrong_ਵਿੱਚ_ਖੰਡ_nr, bnum);
	    ਜਾਓ error_ਖੰਡ;
	}
	ਜੇ (action == ML_DELETE)
	{
	    pp->pb_ਸੂਚਕ[idx].pe_ਸਤਰ_ਗਿਣਤੀ--;
	    dirty = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (action == ML_ਪਾਓ)
	{
	    pp->pb_ਸੂਚਕ[idx].pe_ਸਤਰ_ਗਿਣਤੀ++;
	    dirty = TRUE;
	}
	mf_put(mfp, hp, dirty, FALSE);
    }

error_ਖੰਡ:
    mf_put(mfp, hp, FALSE, FALSE);
error_noblock:
    /*
     * If action is ML_DELETE or ML_INSERT we have to correct the tree for
     * the incremented/decremented line counts, because there won't be a line
     * inserted/deleted after all.
     */
    ਜੇ (action == ML_DELETE)
	ml_lineadd(buf, 1);
    ਨਹੀਂਤਾਂ ਜੇ (action == ML_ਪਾਓ)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ = 0;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
ml_ਜੋੜੋ_ਢੇਰ(buf_T *buf)
{
    ਪੂਰਨ		ਸਿਖਰ;
    infoptr_T	*newstack;

    ਸਿਖਰ = buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ;

    // may have to increase the stack size
    ਜੇ (ਸਿਖਰ == buf->b_ml.ml_ਢੇਰ_ਮਾਪ)
    {
	CHECK(ਸਿਖਰ > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_ਢੇਰ_ਮਾਪ + ਢੇਰ_INCR);
	ਜੇ (newstack == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	ਜੇ (ਸਿਖਰ > 0)
	    mch_memmove(newstack, buf->b_ml.ml_ਢੇਰ,
					     (size_t)ਸਿਖਰ * ਆਕਾਰ(infoptr_T));
	vim_free(buf->b_ml.ml_ਢੇਰ);
	buf->b_ml.ml_ਢੇਰ = newstack;
	buf->b_ml.ml_ਢੇਰ_ਮਾਪ += ਢੇਰ_INCR;
    }

    buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ++;
    ਵਾਪਸ ਸਿਖਰ;
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
    ਸਥਿਰ ਖਾਲੀ
ml_lineadd(buf_T *buf, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਪੂਰਨ		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    ਲਈ (idx = buf->b_ml.ml_ਢੇਰ_ਸਿਖਰ - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_ਢੇਰ[idx]);
	ਜੇ ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ਨਿਰਾ)
	    ਰੋਕੋ;
	pp = (PTR_BL *)(hp->bh_ਅੰਕੜੇ);	// must be pointer block
	ਜੇ (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_ਸੂਚਕ_ਖੰਡ_id_wrong_two);
	    ਰੋਕੋ;
	}
	pp->pb_ਸੂਚਕ[ip->ip_ਸੂਚਕਾਂਕ].pe_ਸਤਰ_ਗਿਣਤੀ += ਗਿਣਤੀ_2;
	ip->ip_ਉੱਚਾ += ਗਿਣਤੀ_2;
	mf_put(mfp, hp, TRUE, FALSE);
    }
}

#ਜੇ defined(HAVE_READLINK)
/*
 * Resolve a symlink in the last component of a file name.
 * Note that f_resolve() does it for every part of the path, we don't do that
 * here.
 * If it worked returns OK and the resolved link in "buf[MAXPATHL]".
 * Otherwise returns FAIL.
 */
    ਪੂਰਨ
resolve_symlink(char_u *fname, char_u *buf)
{
    char_u	tmp[MAXPATHL];
    ਪੂਰਨ		ret;
    ਪੂਰਨ		ਡੂੰਘਾਈ = 0;

    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    ਲਈ (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	ਜੇ (++ਡੂੰਘਾਈ == 100)
	{
	    semsg(_(e_symlink_loop_for_str), fname);
	    ਵਾਪਸ FAIL;
	}

	ret = readlink((ਅੱਖਰ *)tmp, (ਅੱਖਰ *)buf, MAXPATHL - 1);
	ਜੇ (ret <= 0)
	{
	    ਜੇ (errno == EINVAL || errno == ENOENT)
	    {
		// Found non-symlink or not existing file, stop here.
		// When at the first level use the unmodified name, skip the
		// call to vim_FullName().
		ਜੇ (ਡੂੰਘਾਈ == 1)
		    ਵਾਪਸ FAIL;

		// Use the resolved name in tmp[].
		ਰੋਕੋ;
	    }

	    // There must be some error reading links, use original name.
	    ਵਾਪਸ FAIL;
	}
	buf[ret] = NUL;

	/*
	 * Check whether the symlink is relative or absolute.
	 * If it's relative, build a new path based on the directory
	 * portion of the filename (if any) and the path the symlink
	 * points to.
	 */
	ਜੇ (mch_is_ਭਰਿਆ_Name(buf))
	    STRCPY(tmp, buf);
	ਨਹੀਂਤਾਂ
	{
	    char_u *ਪੂਛ;

	    ਪੂਛ = gettail(tmp);
	    ਜੇ (STRLEN(ਪੂਛ) + STRLEN(buf) >= MAXPATHL)
		ਵਾਪਸ FAIL;
	    STRCPY(ਪੂਛ, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    ਵਾਪਸ vim_ਭਰਿਆ_Name(tmp, buf, MAXPATHL, TRUE);
}
#ਜੇ_ਅੰਤ

/*
 * Make swap file name out of the file name and a directory name.
 * Returns pointer to allocated memory or NULL.
 */
    char_u *
makeswapname(
    char_u	*fname,
    char_u	*ffname ਅਣਵਰਤਿਆ,
    buf_T	*buf,
    char_u	*dir_name)
{
    char_u	*r, *s;
    char_u	*fname_res = fname;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_READLINK
    char_u	fname_buf[MAXPATHL];

    // Expand symlink in the file name, so that we put the swap file with the
    // actual file instead of with the symlink.
    ਜੇ (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#ਜੇ_ਅੰਤ

#ਜੇ defined(UNIX) || defined(MSWIN)  // Need _very_ long file names
    ਪੂਰਨ		len = (ਪੂਰਨ)STRLEN(dir_name);

    s = dir_name + len;
    ਜੇ (after_pathsep(dir_name, s) && len > 1 && s[-1] == s[-2])
    {			       // Ends with '//', Use Full path
	r = ਨਿਰਾ;
	ਜੇ ((s = make_percent_swname(dir_name, s, fname_res)) != ਨਿਰਾ)
	{
	    r = modname(s, (char_u *)".swp", FALSE);
	    vim_free(s);
	}
	ਵਾਪਸ r;
    }
#ਜੇ_ਅੰਤ

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#ਜੇ defined(VMS)
	    "_swp",
#ਨਹੀਂਤਾਂ
	    ".swp",
#ਜੇ_ਅੰਤ
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    ਜੇ (r == ਨਿਰਾ)	    // out of memory
	ਵਾਪਸ ਨਿਰਾ;

    s = get_file_ਵਿੱਚ_dir(r, dir_name);
    vim_free(r);
    ਵਾਪਸ s;
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
get_file_ਵਿੱਚ_dir(
    char_u  *fname,
    char_u  *dname)	// don't use "dirname", it is a global for Alpha
{
    string_T	ਪੂਛ;
    string_T	retval;

    ਪੂਛ.string = gettail(fname);
    ਪੂਛ.ਲੰਮਾਈ = STRLEN(ਪੂਛ.string);

    ਜੇ (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (size_t)(ਪੂਛ.string - fname) + ਪੂਛ.ਲੰਮਾਈ);
    ਨਹੀਂਤਾਂ
    {
	size_t	dname_len = STRLEN(dname);

	ਜੇ (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    ਜੇ (ਪੂਛ.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    ਪੂਛ.string, ਪੂਛ.ਲੰਮਾਈ, TRUE, &retval);
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ	    save_char;
		string_T    tmp;

		save_char = *ਪੂਛ.string;
		*ਪੂਛ.string = NUL;
		concat_fnames(fname, (size_t)(ਪੂਛ.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*ਪੂਛ.string = save_char;
		ਜੇ (tmp.string == ਨਿਰਾ)	    // out of memory
		    retval.string = ਨਿਰਾ;
		ਨਹੀਂਤਾਂ
		{
		    concat_fnames(tmp.string, tmp.ਲੰਮਾਈ,
			ਪੂਛ.string, ਪੂਛ.ਲੰਮਾਈ, TRUE, &retval);
		    vim_free(tmp.string);
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    concat_fnames(dname, dname_len, ਪੂਛ.string, ਪੂਛ.ਲੰਮਾਈ,
		TRUE, &retval);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (retval.string != ਨਿਰਾ)
    {
	char_u	*t;

	ਲਈ (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    ਜੇ (*t == ':')
		*t = '%';
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ retval.string;
}

/*
 * Print the ATTENTION message: info about an existing swap file.
 */
    ਸਥਿਰ ਖਾਲੀ
attention_ਸੁਨੇਹਾ(
    buf_T   *buf,	// buffer being edited
    char_u  *fname)	// swap file name
{
    stat_T	st;
    time_t	ਅਦਲਾ_ਬਦਲੀ_mtime;

    ++no_wait_return;
    (ਖਾਲੀ)emsg(_(e_attention));
    ਸੁਨੇਹਾ_puts(_("\nFound a swap file by the name \""));
    ਸੁਨੇਹਾ_home_replace(fname);
    ਸੁਨੇਹਾ_puts("\"\n");
    ਅਦਲਾ_ਬਦਲੀ_mtime = swapfile_info(fname);
    ਸੁਨੇਹਾ_puts(_("While opening file \""));
    ਸੁਨੇਹਾ_outtrans(buf->b_fname);
    ਸੁਨੇਹਾ_puts("\"\n");
    ਜੇ (mch_stat((ਅੱਖਰ *)buf->b_fname, &st) == -1)
    {
	ਸੁਨੇਹਾ_puts(_("      CANNOT BE FOUND"));
    }
    ਨਹੀਂਤਾਂ
    {
	ਸੁਨੇਹਾ_puts(_("             dated: "));
	ਸੁਨੇਹਾ_puts(get_ctime(st.st_mtime, TRUE));
	ਜੇ (ਅਦਲਾ_ਬਦਲੀ_mtime != 0 && st.st_mtime > ਅਦਲਾ_ਬਦਲੀ_mtime)
	    ਸੁਨੇਹਾ_puts(_("      NEWER than swap file!\n"));
    }
    // Some of these messages are long to allow translation to
    // other languages.
    ਸੁਨੇਹਾ_puts(_("\n(1) Another program may be editing the same file.  If this is the case,\n    be careful not to end up with two different instances of the same\n    file when making changes.  Quit, or continue with caution.\n"));
    ਸੁਨੇਹਾ_puts(_("(2) An edit session for this file crashed.\n"));
    ਸੁਨੇਹਾ_puts(_("    If this is the case, use \":recover\" or \"vim -r "));
    ਸੁਨੇਹਾ_outtrans(buf->b_fname);
    ਸੁਨੇਹਾ_puts(_("\"\n    to recover the changes (see \":help recovery\").\n"));
    ਸੁਨੇਹਾ_puts(_("    If you did this already, delete the swap file \""));
    ਸੁਨੇਹਾ_outtrans(fname);
    ਸੁਨੇਹਾ_puts(_("\"\n    to avoid this message.\n"));
    cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
    --no_wait_return;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    SEA_CHOICE_NONE = 0,
    SEA_CHOICE_READONLY = 1,
    SEA_CHOICE_EDIT = 2,
    SEA_CHOICE_RECOVER = 3,
    SEA_CHOICE_DELETE = 4,
    SEA_CHOICE_QUIT = 5,
    SEA_CHOICE_ABORT = 6
} sea_choice_T;

#ਜੇ defined(FEAT_EVAL)
/*
 * Trigger the SwapExists autocommands.
 * Returns a value for equivalent to do_dialog().
 */
    ਸਥਿਰ sea_choice_T
do_swapexists(buf_T *buf, char_u *fname)
{
    ਸੈੱਟ_vim_var_string(VV_SWAPNAME, fname, -1);
    ਸੈੱਟ_vim_var_string(VV_SWAPCHOICE, ਨਿਰਾ, -1);

    // Trigger SwapExists autocommands with <afile> set to the file being
    // edited.  Disallow changing directory here.
    ++allbuf_ਤਾਲਾ;
    apply_autocmds(EVENT_SWAPEXISTS, buf->b_fname, ਨਿਰਾ, FALSE, ਨਿਰਾ);
    --allbuf_ਤਾਲਾ;

    ਸੈੱਟ_vim_var_string(VV_SWAPNAME, ਨਿਰਾ, -1);

    ਚੋਣ (*get_vim_var_str(VV_SWAPCHOICE))
    {
	ਹਾਲਤ 'o': ਵਾਪਸ SEA_CHOICE_READONLY;
	ਹਾਲਤ 'e': ਵਾਪਸ SEA_CHOICE_EDIT;
	ਹਾਲਤ 'r': ਵਾਪਸ SEA_CHOICE_RECOVER;
	ਹਾਲਤ 'd': ਵਾਪਸ SEA_CHOICE_DELETE;
	ਹਾਲਤ 'q': ਵਾਪਸ SEA_CHOICE_QUIT;
	ਹਾਲਤ 'a': ਵਾਪਸ SEA_CHOICE_ABORT;
    }

    ਵਾਪਸ SEA_CHOICE_NONE;
}
#ਜੇ_ਅੰਤ

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
    ਸਥਿਰ char_u *
findswapname(
    buf_T	*buf,
    char_u	**dirp,		// pointer to list of directories
    char_u	*old_fname)	// don't give warning for this file name
{
    char_u	*fname;
    ਪੂਰਨ		n;
    char_u	*dir_name;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AMIGA
    BPTR	fh;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		r;
    char_u	*buf_fname = buf->b_fname;

#ਜੇ !defined(UNIX)
# define CREATE_DUMMY_FILE
    FILE	*dummyfd = ਨਿਰਾ;

# ifdef MSWIN
    ਜੇ (buf_fname != ਨਿਰਾ && !mch_is_ਭਰਿਆ_Name(buf_fname)
				       && vim_strchr(gettail(buf_fname), ':'))
    {
	char_u *t;

	buf_fname = vim_strsave(buf_fname);
	ਜੇ (buf_fname == ਨਿਰਾ)
	    buf_fname = buf->b_fname;
	ਨਹੀਂਤਾਂ
	    ਲਈ (t = gettail(buf_fname); *t != NUL; MB_PTR_ADV(t))
		ਜੇ (*t == ':')
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
    ਜੇ (!(buf->b_p_sn || buf->b_shortname) && buf_fname != ਨਿਰਾ
					     && mch_getperm(buf_fname) < 0)
	dummyfd = mch_fopen((ਅੱਖਰ *)buf_fname, "w");
#ਜੇ_ਅੰਤ

    /*
     * Isolate a directory name from *dirp and put it in dir_name.
     * First allocate some memory to put the directory name in.
     */
    dir_name = alloc(STRLEN(*dirp) + 1);
    ਜੇ (dir_name == ਨਿਰਾ)
	*dirp = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)copy_option_part(dirp, dir_name, 31000, ",");

    /*
     * we try different names until we find one that does not exist yet
     */
    ਜੇ (dir_name == ਨਿਰਾ)	    // out of memory
	fname = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	fname = makeswapname(buf_fname, buf->b_ffname, buf, dir_name);

    ਲਈ (;;)
    {
	ਜੇ (fname == ਨਿਰਾ)	// must be out of memory
	    ਰੋਕੋ;
	ਜੇ ((n = (ਪੂਰਨ)STRLEN(fname)) == 0)	// safety check
	{
	    VIM_CLEAR(fname);
	    ਰੋਕੋ;
	}
#ਜੇ defined(UNIX)
/*
 * Some systems have a MS-DOS compatible filesystem that use 8.3 character
 * file names. If this is the first try and the swap file name does not fit in
 * 8.3, detect if this is the case, set shortname and try again.
 */
	ਜੇ (fname[n - 2] == 'w' && fname[n - 1] == 'p'
					&& !(buf->b_p_sn || buf->b_shortname))
	{
	    char_u	    *ਪੂਛ;
	    char_u	    *fname2;
	    stat_T	    s1, s2;
	    ਪੂਰਨ		    f1, f2;
	    ਪੂਰਨ		    created1 = FALSE, created2 = FALSE;
	    ਪੂਰਨ		    same = FALSE;

	    /*
	     * Check if swapfile name does not fit in 8.3:
	     * It either contains two dots, is longer than 8 chars, or starts
	     * with a dot.
	     */
	    ਪੂਛ = gettail(buf_fname);
	    ਜੇ (       vim_strchr(ਪੂਛ, '.') != ਨਿਰਾ
		    || STRLEN(ਪੂਛ) > (size_t)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		ਜੇ (fname2 != ਨਿਰਾ)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    ਜੇ (vim_strchr(ਪੂਛ, '.') != ਨਿਰਾ)
			fname2[n - 1] = 'x';
		    ਨਹੀਂਤਾਂ ਜੇ (*gettail(fname) == '.')
		    {
			fname2[n] = 'x';
			fname2[n + 1] = NUL;
		    }
		    ਨਹੀਂਤਾਂ
			fname2[n - 5] += 1;
		    /*
		     * may need to create the files to be able to use mch_stat()
		     */
		    f1 = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_EXTRA, 0);
		    ਜੇ (f1 < 0)
		    {
			f1 = mch_open_rw((ਅੱਖਰ *)fname,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			created1 = TRUE;
		    }
		    ਜੇ (f1 >= 0)
		    {
			f2 = mch_open((ਅੱਖਰ *)fname2, O_RDONLY | O_EXTRA, 0);
			ਜੇ (f2 < 0)
			{
			    f2 = mch_open_rw((ਅੱਖਰ *)fname2,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			    created2 = TRUE;
			}
			ਜੇ (f2 >= 0)
			{
			    /*
			     * Both files exist now. If mch_stat() returns the
			     * same device and inode they are the same file.
			     */
			    ਜੇ (mch_fstat(f1, &s1) != -1
				    && mch_fstat(f2, &s2) != -1
				    && s1.st_dev == s2.st_dev
				    && s1.st_ino == s2.st_ino)
				same = TRUE;
			    close(f2);
			    ਜੇ (created2)
				mch_remove(fname2);
			}
			close(f1);
			ਜੇ (created1)
			    mch_remove(fname);
		    }
		    vim_free(fname2);
		    ਜੇ (same)
		    {
			buf->b_shortname = true;
			vim_free(fname);
			fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
			ਜਾਰੀ;	// try again with b_shortname set
		    }
		}
	    }
	}
#ਜੇ_ਅੰਤ
	/*
	 * check if the swapfile already exists
	 */
	ਜੇ (mch_getperm(fname) < 0)	// it does not exist
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    ਜੇ (mch_lstat((ਅੱਖਰ *)fname, &sb) < 0)
#ਨਹੀਂਤਾਂ
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (ਲੰਮਾ)ਬਹੁਲਕ_NEWFILE);
	    /*
	     * on the Amiga mch_getperm() will return -1 when the file exists
	     * but is being used by another program. This happens if you edit
	     * a file twice.
	     */
	    ਜੇ (fh != (BPTR)ਨਿਰਾ)	// can open file, OK
	    {
		Close(fh);
		mch_remove(fname);
		ਰੋਕੋ;
	    }
	    ਜੇ (IoErr() != ERROR_OBJECT_ਵਿੱਚ_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#ਜੇ_ਅੰਤ
		ਰੋਕੋ;
	}

	/*
	 * A file name equal to old_fname is OK to use.
	 */
	ਜੇ (old_fname != ਨਿਰਾ && fnamecmp(fname, old_fname) == 0)
	    ਰੋਕੋ;

	/*
	 * get here when file already exists
	 */
	ਜੇ (fname[n - 2] == 'w' && fname[n - 1] == 'p')	// first try
	{
	    /*
	     * on MS-DOS compatible filesystems (e.g. messydos) file.doc.swp
	     * and file.doc are the same file. To guess if this problem is
	     * present try if file.doc.swx exists. If it does, we set
	     * buf->b_shortname and try file_doc.swp (dots replaced by
	     * underscores for this file), and try again. If it doesn't we
	     * assume that "file.doc.swp" already exists.
	     */
	    ਜੇ (!(buf->b_p_sn || buf->b_shortname))	// not tried yet
	    {
		fname[n - 1] = 'x';
		r = mch_getperm(fname);		// try "file.swx"
		fname[n - 1] = 'p';
		ਜੇ (r >= 0)		    // "file.swx" seems to exist
		{
		    buf->b_shortname = true;
		    vim_free(fname);
		    fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
		    ਜਾਰੀ;	    // try again with '.' replaced with '_'
		}
	    }
	    /*
	     * If we get here the ".swp" file really exists.
	     * Give an error message, unless recovering, no file name, we are
	     * viewing a help file or when the path of the file is different
	     * (happens when all .swp files are in one directory).
	     */
	    ਜੇ (!recoverymode && buf_fname != ਨਿਰਾ
				&& !buf->b_help
				&& !(buf->b_flags & (BF_DUMMY | BF_NO_SEA)))
	    {
		ਪੂਰਨ		fd;
		ਬਣਤਰ block0	b0;
		ਪੂਰਨ		ਫਰਕ = FALSE;

		/*
		 * Try to read block 0 from the swap file to get the original
		 * file name (and inode number).
		 */
		fd = mch_open((ਅੱਖਰ *)fname, O_RDONLY | O_EXTRA, 0);
		ਜੇ (fd >= 0)
		{
		    ਜੇ (read_eintr(fd, &b0, ਆਕਾਰ(b0)) == ਆਕਾਰ(b0))
		    {
			/*
			 * If the swapfile has the same directory as the
			 * buffer don't compare the directory names, they can
			 * have a different mountpoint.
			 */
			ਜੇ (b0.b0_flags & B0_SAME_DIR)
			{
			    ਜੇ (fnamecmp(gettail(buf->b_ffname),
						   gettail(b0.b0_fname)) != 0
				    || !same_directory(fname, buf->b_ffname))
			    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				ਜੇ (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_ਮੰਜ਼ਿਲ_long(b0.b0_ino)))
#ਜੇ_ਅੰਤ
				    ਫਰਕ = TRUE;
			    }
			}
			ਨਹੀਂਤਾਂ
			{
			    /*
			     * The name in the swap file may be
			     * "~user/path/file".  Expand it first.
			     */
			    expand_env(b0.b0_fname, NameBuff, MAXPATHL);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_INODE
			    ਜੇ (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_ਮੰਜ਼ਿਲ_long(b0.b0_ino)))
				ਫਰਕ = TRUE;
#ਨਹੀਂਤਾਂ
			    ਜੇ (fnamecmp(NameBuff, buf->b_ffname) != 0)
				ਫਰਕ = TRUE;
#ਜੇ_ਅੰਤ
			}
		    }
		    close(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		ਜੇ (ਫਰਕ == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == ਨਿਰਾ)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CREATE_DUMMY_FILE
		    ਪੂਰਨ		 did_use_dummy = FALSE;

		    // Avoid getting a warning for the file being created
		    // outside of Vim, it was created at the start of this
		    // function.  Delete the file now, because Vim might exit
		    // here if the window is closed.
		    ਜੇ (dummyfd != ਨਿਰਾ)
		    {
			fclose(dummyfd);
			dummyfd = ਨਿਰਾ;
			mch_remove(buf_fname);
			did_use_dummy = TRUE;
		    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PROCESS_STILL_RUNNING
		    process_still_running = FALSE;
#ਜੇ_ਅੰਤ
		    // It's safe to delete the swap file if all these are true:
		    // - the edited file exists
		    // - the swap file has no changes and looks OK
		    ਜੇ (mch_stat((ਅੱਖਰ *)buf->b_fname, &st) == 0
						  && swapfile_unchanged(fname))
		    {
			choice = SEA_CHOICE_DELETE;
			ਜੇ (p_verbose > 0)
			    verb_ਸੁਨੇਹਾ(_("Found a swap file that is not useful, deleting it"));
		    }

#ਜੇ defined(FEAT_EVAL)
		    /*
		     * If there is an SwapExists autocommand and we can handle
		     * the response, trigger it.  It may return 0 to ask the
		     * user anyway.
		     */
		    ਜੇ (choice == SEA_CHOICE_NONE
			    && ਅਦਲਾ_ਬਦਲੀ_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#ਜੇ_ਅੰਤ

		    ਜੇ (choice == SEA_CHOICE_NONE
					 && ਅਦਲਾ_ਬਦਲੀ_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    ਜੇ (choice == SEA_CHOICE_NONE)
		    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			ਜੇ (gui.starting && !gui.ਵਿੱਚ_use)
			    gui_ਸ਼ੁਰੂ(ਨਿਰਾ);
#ਜੇ_ਅੰਤ
			// Show info about the existing swap file.
			attention_ਸੁਨੇਹਾ(buf, fname);

			// We don't want a 'q' typed at the more-prompt
			// interrupt loading a file.
			got_int = FALSE;

			// If vimrc has "simalt ~x" we don't want it to
			// interfere with the prompt here.
			flush_buffers(FLUSH_TYPEAHEAD);
		    }

#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    ਅਗੇਤਰ = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			size_t	    ਸੁਨੇਹਾ_ਮਾਪ;
			string_T    ਸੁਨੇਹਾ_2;
			char_u	    *tofree;
			ਪੂਰਨ	    dialog_ਨਤੀਜਾ;

			ਅਗੇਤਰ.ਲੰਮਾਈ = STRLEN(ਅਗੇਤਰ.string);
			suffix.ਲੰਮਾਈ = STRLEN(suffix.string);
			ਸੁਨੇਹਾ_ਮਾਪ = ਅਗੇਤਰ.ਲੰਮਾਈ
				+ STRLEN(fname)
				+ suffix.ਲੰਮਾਈ + 5;
			ਸੁਨੇਹਾ_2.string = tofree = alloc(ਸੁਨੇਹਾ_ਮਾਪ);
			ਜੇ (ਸੁਨੇਹਾ_2.string != ਨਿਰਾ)
			{
			    STRCPY(ਸੁਨੇਹਾ_2.string, ਅਗੇਤਰ.string);
			    ਸੁਨੇਹਾ_2.ਲੰਮਾਈ = ਅਗੇਤਰ.ਲੰਮਾਈ;
			    ਸੁਨੇਹਾ_2.ਲੰਮਾਈ += home_replace(ਨਿਰਾ, fname,
				ਸੁਨੇਹਾ_2.string + ਸੁਨੇਹਾ_2.ਲੰਮਾਈ, (ਪੂਰਨ)(ਸੁਨੇਹਾ_ਮਾਪ - ਸੁਨੇਹਾ_2.ਲੰਮਾਈ), TRUE);
			    STRCPY(ਸੁਨੇਹਾ_2.string + ਸੁਨੇਹਾ_2.ਲੰਮਾਈ, suffix.string);
			}
			ਨਹੀਂਤਾਂ
			    ਸੁਨੇਹਾ_2.string = (char_u *)_("Swap file already exists!");
			dialog_ਨਤੀਜਾ = do_dialog(VIM_WARNING,
				    (char_u *)_("VIM - ATTENTION"),
				    ਸੁਨੇਹਾ_2.string,
# ifdef HAVE_PROCESS_STILL_RUNNING
				    process_still_running
					? (char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Quit\n&Abort") :
# endif
					(char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Delete it\n&Quit\n&Abort"), 1, ਨਿਰਾ, FALSE);

# ifdef HAVE_PROCESS_STILL_RUNNING
			ਜੇ (process_still_running && dialog_ਨਤੀਜਾ >= 4)
			    // compensate for missing "Delete it" button
			    dialog_ਨਤੀਜਾ++;
# endif
			choice = dialog_ਨਤੀਜਾ;
			vim_free(tofree);

			// pretend screen didn't scroll, need redraw anyway
			ਸੁਨੇਹਾ_scrolled = 0;
			redraw_all_later(UPD_NOT_VALID);
		    }
#ਜੇ_ਅੰਤ

		    ਚੋਣ (choice)
		    {
			ਹਾਲਤ SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    ਰੋਕੋ;
			ਹਾਲਤ SEA_CHOICE_EDIT:
			    ਰੋਕੋ;
			ਹਾਲਤ SEA_CHOICE_RECOVER:
			    ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_RECOVER;
			    ਰੋਕੋ;
			ਹਾਲਤ SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    ਰੋਕੋ;
			ਹਾਲਤ SEA_CHOICE_QUIT:
			    ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_QUIT;
			    ਰੋਕੋ;
			ਹਾਲਤ SEA_CHOICE_ABORT:
			    ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_QUIT;
			    got_int = TRUE;
			    ਰੋਕੋ;
			ਹਾਲਤ SEA_CHOICE_NONE:
			    ਸੁਨੇਹਾ_puts("\n");
			    ਜੇ (ਸੁਨੇਹਾ_silent == 0)
				// call wait_return() later
				need_wait_return = TRUE;
			    ਰੋਕੋ;
		    }

		    // If the file was deleted this fname can be used.
		    ਜੇ (choice != SEA_CHOICE_NONE && mch_getperm(fname) < 0)
			ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    ਜੇ (did_use_dummy)
			dummyfd = mch_fopen((ਅੱਖਰ *)buf_fname, "w");
#ਜੇ_ਅੰਤ
		}
	    }
	}

	/*
	 * Change the ".swp" extension to find another file that can be used.
	 * First decrement the last char: ".swo", ".swn", etc.
	 * If that still isn't enough decrement the last but one char: ".svz"
	 * Can happen when editing many "No Name" buffers.
	 */
	ਜੇ (fname[n - 1] == 'a')	// ".s?a"
	{
	    ਜੇ (fname[n - 2] == 'a')    // ".saa": tried enough, give up
	    {
		emsg(_(e_too_many_ਅਦਲਾ_ਬਦਲੀ_files_ਮਿਲਿਆ));
		VIM_CLEAR(fname);
		ਰੋਕੋ;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CREATE_DUMMY_FILE
    ਜੇ (dummyfd != ਨਿਰਾ)	// file has been created temporarily
    {
	fclose(dummyfd);
	mch_remove(buf_fname);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ fname;
}

    ਸਥਿਰ ਪੂਰਨ
b0_magic_wrong(ZERO_BL *b0p)
{
    ਵਾਪਸ (b0p->b0_magic_long != (ਲੰਮਾ)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (ਪੂਰਨ)B0_MAGIC_INT
	    || b0p->b0_magic_short != (ਛੋਟਾ)B0_MAGIC_SHORT
	    || b0p->b0_magic_char != B0_MAGIC_CHAR);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_INODE
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

    ਸਥਿਰ ਪੂਰਨ
fnamecmp_ino(
    char_u	*fname_c,	    // current file name
    char_u	*fname_s,	    // file name from swap file
    ਲੰਮਾ	ino_block0)
{
    stat_T	st;
    ino_t	ino_c = 0;	    // ino of current file
    ino_t	ino_s;		    // ino of file from swap file
    char_u	buf_c[MAXPATHL];    // full path of fname_c
    char_u	buf_s[MAXPATHL];    // full path of fname_s
    ਪੂਰਨ		retval_c;	    // flag: buf_c valid
    ਪੂਰਨ		retval_s;	    // flag: buf_s valid

    ਜੇ (mch_stat((ਅੱਖਰ *)fname_c, &st) == 0)
	ino_c = (ino_t)st.st_ino;

    /*
     * First we try to get the inode from the file name, because the inode in
     * the swap file may be outdated.  If that fails (e.g. this path is not
     * valid on this machine), use the inode from block 0.
     */
    ਜੇ (mch_stat((ਅੱਖਰ *)fname_s, &st) == 0)
	ino_s = (ino_t)st.st_ino;
    ਨਹੀਂਤਾਂ
	ino_s = (ino_t)ino_block0;

    ਜੇ (ino_c && ino_s)
	ਵਾਪਸ (ino_c != ino_s);

    /*
     * One of the inode numbers is unknown, try a forced vim_FullName() and
     * compare the file names.
     */
    retval_c = vim_ਭਰਿਆ_Name(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_ਭਰਿਆ_Name(fname_s, buf_s, MAXPATHL, TRUE);
    ਜੇ (retval_c == OK && retval_s == OK)
	ਵਾਪਸ STRCMP(buf_c, buf_s) != 0;

    /*
     * Can't compare inodes or file names, guess that the files are different,
     * unless both appear not to exist at all, then compare with the file name
     * in the swap file.
     */
    ਜੇ (ino_s == 0 && ino_c == 0 && retval_c == FAIL && retval_s == FAIL)
	ਵਾਪਸ STRCMP(fname_c, fname_s) != 0;
    ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    ਸਥਿਰ ਖਾਲੀ
long_ਮੰਜ਼ਿਲ_char(ਲੰਮਾ n, char_u *s)
{
    s[0] = (char_u)(n & 0xff);
    n = (ਬਿਨਾਨਿਸ਼ਾਨੀ)n >> 8;
    s[1] = (char_u)(n & 0xff);
    n = (ਬਿਨਾਨਿਸ਼ਾਨੀ)n >> 8;
    s[2] = (char_u)(n & 0xff);
    n = (ਬਿਨਾਨਿਸ਼ਾਨੀ)n >> 8;
    s[3] = (char_u)(n & 0xff);
}

    ਸਥਿਰ ਲੰਮਾ
char_ਮੰਜ਼ਿਲ_long(char_u *s)
{
    ਲੰਮਾ    retval;

    retval = s[3];
    retval <<= 8;
    retval |= s[2];
    retval <<= 8;
    retval |= s[1];
    retval <<= 8;
    retval |= s[0];

    ਵਾਪਸ retval;
}

/*
 * Set the flags in the first block of the swap file:
 * - file is modified or not: buf->b_changed
 * - 'fileformat'
 * - 'fileencoding'
 */
    ਖਾਲੀ
ml_setflags(buf_T *buf)
{
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    ਜੇ (!buf->b_ml.ml_mfp)
	ਵਾਪਸ;
    ਲਈ (hp = buf->b_ml.ml_mfp->mf_ਵਰਤਿਆ_ਆਖਰੀ; hp != ਨਿਰਾ; hp = hp->bh_prev)
    {
	ਜੇ (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_ਅੰਕੜੇ);
	    b0p->b0_dirty = buf->b_ਬਦਲਿਆ ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_ਨਕਾਬ)
						  | (get_fileformat(buf) + 1);
	    ਜੋੜੋ_b0_fenc(b0p, buf);
	    hp->bh_flags |= BH_DIRTY;
	    mf_sync(buf->b_ml.ml_mfp, MFS_ZERO);
	    ਰੋਕੋ;
	}
    }
}

#ਜੇ defined(FEAT_CRYPT)
/*
 * If "data" points to a data block encrypt the text in it and return a copy
 * in allocated memory.  Return NULL when out of memory.
 * Otherwise return "data".
 */
    char_u *
ml_encrypt_ਅੰਕੜੇ(
    memfile_T	*mfp,
    char_u	*ਅੰਕੜੇ,
    off_T	ਵਿੱਥ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਮਾਪ)
{
    ਅੰਕੜੇ_BL	*dp = (ਅੰਕੜੇ_BL *)ਅੰਕੜੇ;
    char_u	*ਸਿਰ_ਸਿਰਾ;
    char_u	*ਪਾਠ_ਸ਼ੁਰੂ;
    char_u	*new_ਅੰਕੜੇ;
    ਪੂਰਨ		ਪਾਠ_len;
    cryptstate_T *state;

    ਜੇ (dp->db_id != ਅੰਕੜੇ_ID)
	ਵਾਪਸ ਅੰਕੜੇ;

    state = ml_crypt_prepare(mfp, ਵਿੱਥ, FALSE);
    ਜੇ (state == ਨਿਰਾ)
	ਵਾਪਸ ਅੰਕੜੇ;

    new_ਅੰਕੜੇ = alloc(ਮਾਪ);
    ਜੇ (new_ਅੰਕੜੇ == ਨਿਰਾ)
    {
	crypt_free_state(state);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਸਿਰ_ਸਿਰਾ = (char_u *)(&dp->db_ਸੂਚਕਾਂਕ[dp->db_ਸਤਰ_ਗਿਣਤੀ]);
    ਪਾਠ_ਸ਼ੁਰੂ = (char_u *)dp + dp->db_txt_ਸ਼ੁਰੂ;
    ਪਾਠ_len = ਮਾਪ - dp->db_txt_ਸ਼ੁਰੂ;

    // Copy the header and the text.
    mch_memmove(new_ਅੰਕੜੇ, dp, ਸਿਰ_ਸਿਰਾ - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, ਪਾਠ_ਸ਼ੁਰੂ, ਪਾਠ_len, new_ਅੰਕੜੇ + dp->db_txt_ਸ਼ੁਰੂ,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    ਜੇ (ਸਿਰ_ਸਿਰਾ < ਪਾਠ_ਸ਼ੁਰੂ)
	vim_memset(new_ਅੰਕੜੇ + (ਸਿਰ_ਸਿਰਾ - ਅੰਕੜੇ), 0, ਪਾਠ_ਸ਼ੁਰੂ - ਸਿਰ_ਸਿਰਾ);

    ਵਾਪਸ new_ਅੰਕੜੇ;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    ਖਾਲੀ
ml_decrypt_ਅੰਕੜੇ(
    memfile_T	*mfp,
    char_u	*ਅੰਕੜੇ,
    off_T	ਵਿੱਥ,
    ਬਿਨਾਨਿਸ਼ਾਨੀ	ਮਾਪ)
{
    ਅੰਕੜੇ_BL	*dp = (ਅੰਕੜੇ_BL *)ਅੰਕੜੇ;
    char_u	*ਸਿਰ_ਸਿਰਾ;
    char_u	*ਪਾਠ_ਸ਼ੁਰੂ;
    ਪੂਰਨ		ਪਾਠ_len;
    cryptstate_T *state;

    ਜੇ (dp->db_id != ਅੰਕੜੇ_ID)
	ਵਾਪਸ;

    ਸਿਰ_ਸਿਰਾ = (char_u *)(&dp->db_ਸੂਚਕਾਂਕ[dp->db_ਸਤਰ_ਗਿਣਤੀ]);
    ਪਾਠ_ਸ਼ੁਰੂ = (char_u *)dp + dp->db_txt_ਸ਼ੁਰੂ;
    ਪਾਠ_len = dp->db_txt_ਸਿਰਾ - dp->db_txt_ਸ਼ੁਰੂ;

    ਜੇ (ਸਿਰ_ਸਿਰਾ > ਪਾਠ_ਸ਼ੁਰੂ || dp->db_txt_ਸ਼ੁਰੂ > ਮਾਪ
	    || dp->db_txt_ਸਿਰਾ > ਮਾਪ)
	ਵਾਪਸ;  // data was messed up

    state = ml_crypt_prepare(mfp, ਵਿੱਥ, TRUE);
    ਜੇ (state == ਨਿਰਾ)
	ਵਾਪਸ;

    // Decrypt the text in place.
    crypt_decode_inplace(state, ਪਾਠ_ਸ਼ੁਰੂ, ਪਾਠ_len, FALSE);
    crypt_free_state(state);
}

/*
 * Prepare for encryption/decryption, using the key, seed and offset.
 * Return an allocated cryptstate_T *.
 * Note: Encryption not supported for SODIUM
 */
    ਸਥਿਰ cryptstate_T *
ml_crypt_prepare(memfile_T *mfp, off_T ਵਿੱਥ, ਪੂਰਨ reading)
{
    buf_T	*buf = mfp->mf_ਬਫਰ;
    char_u	salt[50];
    ਪੂਰਨ		ਵਿਧੀ_nr;
    char_u	*ਕੁੰਜੀ;
    crypt_arg_T arg;

    CLEAR_ਖੇਤਰ(arg);
    ਜੇ (reading && mfp->mf_old_ਕੁੰਜੀ != ਨਿਰਾ)
    {
	// Reading back blocks with the previous key/method/seed.
	ਵਿਧੀ_nr = mfp->mf_old_cm;
	ਕੁੰਜੀ = mfp->mf_old_ਕੁੰਜੀ;
	arg.cat_ਬੀਜ = mfp->mf_old_ਬੀਜ;
    }
    ਨਹੀਂਤਾਂ
    {
	ਵਿਧੀ_nr = crypt_get_ਵਿਧੀ_nr(buf);
	ਕੁੰਜੀ = buf->b_p_ਕੁੰਜੀ;
	arg.cat_ਬੀਜ = mfp->mf_ਬੀਜ;
    }

    ਜੇ (*ਕੁੰਜੀ == NUL)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (crypt_may_close_swapfile(buf, ਕੁੰਜੀ, ਵਿਧੀ_nr))
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ਵਿਧੀ_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((ਅੱਖਰ *)salt, ਆਕਾਰ(salt), "%s%ld", ਕੁੰਜੀ, (ਲੰਮਾ)ਵਿੱਥ);
	arg.cat_ਬੀਜ = ਨਿਰਾ;
	arg.cat_init_from_file = FALSE;

	ਵਾਪਸ crypt_create(ਵਿਧੀ_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((ਅੱਖਰ *)salt, ਆਕਾਰ(salt), "%ld", (ਲੰਮਾ)ਵਿੱਥ);

    arg.cat_salt = salt;
    arg.cat_salt_len = (ਪੂਰਨ)STRLEN(salt);
    arg.cat_ਬੀਜ_len = MF_ਬੀਜ_LEN;
    arg.cat_ਜੋੜੋ_len = 0;
    arg.cat_ਜੋੜੋ = ਨਿਰਾ;
    arg.cat_init_from_file = FALSE;

    ਵਾਪਸ crypt_create(ਵਿਧੀ_nr, ਕੁੰਜੀ, &arg);
}

#ਜੇ_ਅੰਤ


#ਜੇ defined(FEAT_BYTEOFF)

# define MLCS_MAXL 800	// max no of lines in chunk
# define MLCS_MINL 400   // should be half of MLCS_MAXL

/*
 * Keep information for finding byte offset of a line, updtype may be one of:
 * ML_CHNK_ADDLINE: Add len to parent chunk, possibly splitting it
 *	   Careful: ML_CHNK_ADDLINE may cause ml_find_line() to be called.
 * ML_CHNK_DELLINE: Subtract len from parent chunk, possibly deleting it
 * ML_CHNK_UPDLINE: Add len to parent chunk, as a signed entity.
 */
    ਸਥਿਰ ਖਾਲੀ
ml_updatechunk(
    buf_T	*buf,
    linenr_T	ਸਤਰ,
    ਲੰਮਾ	len,
    ਪੂਰਨ		updtype)
{
    ਸਥਿਰ buf_T	*ml_upd_lastbuf = ਨਿਰਾ;
    ਸਥਿਰ linenr_T	ml_upd_lastcurline;
    ਸਥਿਰ ਪੂਰਨ		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    ਪੂਰਨ			curix = ml_upd_lastcurix;
    ਲੰਮਾ		ਮਾਪ;
    chunksize_T		*curchnk;
    ਪੂਰਨ			rest;
    bhdr_T		*hp;
    ਅੰਕੜੇ_BL		*dp;

    ਜੇ (buf->b_ml.ml_usedchunks == -1 || len == 0)
	ਵਾਪਸ;
    ਜੇ (buf->b_ml.ml_chunksize == ਨਿਰਾ)
    {
	buf->b_ml.ml_chunksize = ALLOC_MULT(chunksize_T, 100);
	ਜੇ (buf->b_ml.ml_chunksize == ਨਿਰਾ)
	{
	    buf->b_ml.ml_usedchunks = -1;
	    ਵਾਪਸ;
	}
	buf->b_ml.ml_numchunks = 100;
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = 1;
	ml_upd_lastbuf = ਨਿਰਾ;		// invalidate resume cache
    }

    ਜੇ (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (ਲੰਮਾ)buf->b_ml.ml_ਸਤਰ_len;
	ml_upd_lastbuf = ਨਿਰਾ;		// invalidate resume cache
	ਵਾਪਸ;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    ਜੇ (buf != ml_upd_lastbuf || ਸਤਰ < curline)
    {
	curline = 1;
	curix = 0;
    }
    ਲਈ (; curix < buf->b_ml.ml_usedchunks - 1
	     && ਸਤਰ >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	     curix++)
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
    curchnk = buf->b_ml.ml_chunksize + curix;

    ਜੇ (updtype == ML_CHNK_DELLINE)
	len = -len;
    curchnk->mlcs_totalsize += len;
    ਜੇ (updtype == ML_CHNK_ADDLINE)
    {
	curchnk->mlcs_numlines++;

	// May resize here so we don't have to do it in both cases below
	ਜੇ (buf->b_ml.ml_usedchunks + 1 >= buf->b_ml.ml_numchunks)
	{
	    chunksize_T *t_chunksize = buf->b_ml.ml_chunksize;

	    buf->b_ml.ml_numchunks = buf->b_ml.ml_numchunks * 3 / 2;
	    buf->b_ml.ml_chunksize = vim_realloc(buf->b_ml.ml_chunksize,
			    ਆਕਾਰ(chunksize_T) * buf->b_ml.ml_numchunks);
	    ਜੇ (buf->b_ml.ml_chunksize == ਨਿਰਾ)
	    {
		// Hmmmm, Give up on offset for this buffer
		vim_free(t_chunksize);
		buf->b_ml.ml_usedchunks = -1;
		ਵਾਪਸ;
	    }
	}

	ਜੇ (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MAXL)
	{
	    ਪੂਰਨ	    ਗਿਣਤੀ_2;	    // number of entries in block
	    ਪੂਰਨ	    idx;
	    ਪੂਰਨ	    ਸਿਰਾ_idx;
	    ਪੂਰਨ	    ਪਾਠ_ਸਿਰਾ;
	    ਪੂਰਨ	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			ਆਕਾਰ(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    ਮਾਪ = 0;
	    linecnt = 0;
	    ਜਦੋਂ (curline < buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
			&& linecnt < MLCS_MINL)
	    {
		ਜੇ ((hp = ml_ਲੱਭੋ_ਸਤਰ(buf, curline, ML_ਲੱਭੋ)) == ਨਿਰਾ)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    ਵਾਪਸ;
		}
		dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
		ਗਿਣਤੀ_2 = (ਲੰਮਾ)(buf->b_ml.ml_locked_ਉੱਚਾ) -
			(ਲੰਮਾ)(buf->b_ml.ml_locked_ਨੀਵਾਂ) + 1;
		idx = curline - buf->b_ml.ml_locked_ਨੀਵਾਂ;
		curline = buf->b_ml.ml_locked_ਉੱਚਾ + 1;

		// compute index of last line to use in this MEMLINE
		rest = ਗਿਣਤੀ_2 - idx;
		ਜੇ (linecnt + rest > MLCS_MINL)
		{
		    ਸਿਰਾ_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਸਿਰਾ_idx = ਗਿਣਤੀ_2 - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		ਜੇ (buf->b_has_textprop)
		{
		    ਪੂਰਨ i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    ਲਈ (i = ਸਿਰਾ_idx; i < idx; ++i)
			ਮਾਪ += (ਪੂਰਨ)STRLEN((char_u *)dp
				      + (dp->db_ਸੂਚਕਾਂਕ[i] & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ)) + 1;
		}
		ਨਹੀਂਤਾਂ
# endif
		{
		    ਜੇ (idx == 0) // first line in block, text at the end
			ਪਾਠ_ਸਿਰਾ = dp->db_txt_ਸਿਰਾ;
		    ਨਹੀਂਤਾਂ
			ਪਾਠ_ਸਿਰਾ = ((dp->db_ਸੂਚਕਾਂਕ[idx - 1]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
		    ਮਾਪ += ਪਾਠ_ਸਿਰਾ
				   - ((dp->db_ਸੂਚਕਾਂਕ[ਸਿਰਾ_idx]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
		}
	    }
	    buf->b_ml.ml_chunksize[curix].mlcs_numlines = linecnt;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_numlines -= linecnt;
	    buf->b_ml.ml_chunksize[curix].mlcs_totalsize = ਮਾਪ;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_totalsize -= ਮਾਪ;
	    buf->b_ml.ml_usedchunks++;
	    ml_upd_lastbuf = ਨਿਰਾ;   // Force recalc of curix & curline
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - ਸਤਰ <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    ਜੇ (ਸਤਰ == buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    {
		curchnk->mlcs_numlines = 0;
		curchnk->mlcs_totalsize = 0;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		/*
		 * Line is just prior to last, move count for last
		 * This is the common case  when loading a new file
		 */
		hp = ml_ਲੱਭੋ_ਸਤਰ(buf, buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, ML_ਲੱਭੋ);
		ਜੇ (hp == ਨਿਰਾ)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    ਵਾਪਸ;
		}
		dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
		ਜੇ (dp->db_ਸਤਰ_ਗਿਣਤੀ == 1)
		    rest = dp->db_txt_ਸਿਰਾ - dp->db_txt_ਸ਼ੁਰੂ;
		ਨਹੀਂਤਾਂ
		    rest =
			((dp->db_ਸੂਚਕਾਂਕ[dp->db_ਸਤਰ_ਗਿਣਤੀ - 2]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ)
			- dp->db_txt_ਸ਼ੁਰੂ;
		curchnk->mlcs_totalsize = rest;
		curchnk->mlcs_numlines = 1;
		curchnk[-1].mlcs_totalsize -= rest;
		curchnk[-1].mlcs_numlines -= 1;
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (updtype == ML_CHNK_DELLINE)
    {
	curchnk->mlcs_numlines--;
	ਜੇ (curix < buf->b_ml.ml_usedchunks - 1
		&& curchnk->mlcs_numlines + curchnk[1].mlcs_numlines
								  <= MLCS_MINL)
	{
	    curix++;
	    curchnk = buf->b_ml.ml_chunksize + curix;
	}
	ਨਹੀਂਤਾਂ ਜੇ (curix == 0 && curchnk->mlcs_numlines <= 0)
	{
	    ml_upd_lastbuf = ਨਿਰਾ;   // Force recalc of curix & curline
	    buf->b_ml.ml_usedchunks--;
	    mch_memmove(buf->b_ml.ml_chunksize, buf->b_ml.ml_chunksize + 1,
			buf->b_ml.ml_usedchunks * ਆਕਾਰ(chunksize_T));
	    ਵਾਪਸ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (curix == 0 || (curchnk->mlcs_numlines > 10
		    && curchnk->mlcs_numlines + curchnk[-1].mlcs_numlines
								  > MLCS_MINL))
	{
	    // The chunks are left as they are, the cached position stays
	    // valid.
	    ml_upd_lastbuf = buf;
	    ml_upd_lastcurline = curline;
	    ml_upd_lastcurix = curix;
	    ਵਾਪਸ;
	}

	// Collapse chunks
	ml_upd_lastbuf = ਨਿਰਾ;   // Force recalc of curix & curline
	curchnk[-1].mlcs_numlines += curchnk->mlcs_numlines;
	curchnk[-1].mlcs_totalsize += curchnk->mlcs_totalsize;
	buf->b_ml.ml_usedchunks--;
	ਜੇ (curix < buf->b_ml.ml_usedchunks)
	    mch_memmove(buf->b_ml.ml_chunksize + curix,
			buf->b_ml.ml_chunksize + curix + 1,
			(buf->b_ml.ml_usedchunks - curix) *
			ਆਕਾਰ(chunksize_T));
	ਵਾਪਸ;
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
    ਲੰਮਾ
ml_ਲੱਭੋ_ਸਤਰ_or_ਵਿੱਥ(buf_T *buf, linenr_T lnum, ਲੰਮਾ *offp)
{
    linenr_T	curline;
    ਪੂਰਨ		curix;
    ਲੰਮਾ	ਮਾਪ;
    bhdr_T	*hp;
    ਅੰਕੜੇ_BL	*dp;
    ਪੂਰਨ		ਗਿਣਤੀ_2;		// number of entries in block
    ਪੂਰਨ		idx;
    ਪੂਰਨ		ਸ਼ੁਰੂ_idx;
    ਪੂਰਨ		ਪਾਠ_ਸਿਰਾ;
    ਲੰਮਾ	ਵਿੱਥ;
    ਪੂਰਨ		len;
    ਪੂਰਨ		ffdos = (get_fileformat(buf) == EOL_DOS);
    ਪੂਰਨ		extra = 0;

    // take care of cached line first
    ml_flush_ਸਤਰ(curbuf);

    ਜੇ (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == ਨਿਰਾ
	    || lnum < 0)
	ਵਾਪਸ -1;

    ਜੇ (offp == ਨਿਰਾ)
	ਵਿੱਥ = 0;
    ਨਹੀਂਤਾਂ
	ਵਿੱਥ = *offp;
    ਜੇ (lnum == 0 && ਵਿੱਥ <= 0)
	ਵਾਪਸ 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = ਮਾਪ = 0;
    ਜਦੋਂ (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (ਵਿੱਥ != 0
	       && ਵਿੱਥ > ਮਾਪ + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (ਲੰਮਾ)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	ਮਾਪ += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	ਜੇ (ਵਿੱਥ && ffdos)
	    ਮਾਪ += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    ਜਦੋਂ ((lnum != 0 && curline < lnum) || (ਵਿੱਥ != 0 && ਮਾਪ < ਵਿੱਥ))
    {
# ifdef FEAT_PROP_POPUP
	size_t textprop_ਕੁੱਲ = 0;
# endif

	ਜੇ (curline > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
		|| (hp = ml_ਲੱਭੋ_ਸਤਰ(buf, curline, ML_ਲੱਭੋ)) == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	dp = (ਅੰਕੜੇ_BL *)(hp->bh_ਅੰਕੜੇ);
	ਗਿਣਤੀ_2 = (ਲੰਮਾ)(buf->b_ml.ml_locked_ਉੱਚਾ) -
		(ਲੰਮਾ)(buf->b_ml.ml_locked_ਨੀਵਾਂ) + 1;
	ਸ਼ੁਰੂ_idx = idx = curline - buf->b_ml.ml_locked_ਨੀਵਾਂ;
	ਜੇ (idx == 0)  // first line in block, text at the end
	    ਪਾਠ_ਸਿਰਾ = dp->db_txt_ਸਿਰਾ;
	ਨਹੀਂਤਾਂ
	    ਪਾਠ_ਸਿਰਾ = ((dp->db_ਸੂਚਕਾਂਕ[idx - 1]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
	// Compute index of last line to use in this MEMLINE
	ਜੇ (lnum != 0)
	{
	    ਜੇ (curline + (ਗਿਣਤੀ_2 - idx) >= lnum)
		idx += lnum - curline - 1;
	    ਨਹੀਂਤਾਂ
		idx = ਗਿਣਤੀ_2 - 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    extra = 0;
	    ਲਈ (;;)
	    {
# ifdef FEAT_PROP_POPUP
		size_t textprop_ਮਾਪ = 0;

		ਜੇ (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_ਸੂਚਕਾਂਕ[idx]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_ਸਿਰਾ
				  : ((dp->db_ਸੂਚਕਾਂਕ[idx - 1]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ));
		    textprop_ਮਾਪ = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		ਜੇ (!(ਵਿੱਥ >= ਮਾਪ
			+ ਪਾਠ_ਸਿਰਾ - (ਪੂਰਨ)((dp->db_ਸੂਚਕਾਂਕ[idx]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ)
# ifdef FEAT_PROP_POPUP
			- (ਲੰਮਾ)(textprop_ਕੁੱਲ + textprop_ਮਾਪ)
# endif
			+ ffdos))
		    ਰੋਕੋ;

		ਜੇ (ffdos)
		    ਮਾਪ++;
# ifdef FEAT_PROP_POPUP
		textprop_ਕੁੱਲ += textprop_ਮਾਪ;
# endif
		ਜੇ (idx == ਗਿਣਤੀ_2 - 1)
		{
		    extra = 1;
		    ਰੋਕੋ;
		}
		idx++;
	    }
	}
# ifdef FEAT_PROP_POPUP
	ਜੇ (buf->b_has_textprop && lnum != 0)
	{
	    ਪੂਰਨ i;

	    // cannot use the db_index pointer, need to get the actual text
	    // lengths.
	    len = 0;
	    ਲਈ (i = ਸ਼ੁਰੂ_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_ਸੂਚਕਾਂਕ[i]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ);
		len += (ਪੂਰਨ)STRLEN(p) + 1;
	    }
	}
	ਨਹੀਂਤਾਂ
# endif
	    len = ਪਾਠ_ਸਿਰਾ - ((dp->db_ਸੂਚਕਾਂਕ[idx]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ)
# ifdef FEAT_PROP_POPUP
				- (ਲੰਮਾ)textprop_ਕੁੱਲ
# endif
				;
	ਮਾਪ += len;
	ਜੇ (ਵਿੱਥ != 0 && ਮਾਪ >= ਵਿੱਥ)
	{
	    ਜੇ (ਮਾਪ + ffdos == ਵਿੱਥ)
		*offp = 0;
	    ਨਹੀਂਤਾਂ ਜੇ (idx == ਸ਼ੁਰੂ_idx)
		*offp = ਵਿੱਥ - ਮਾਪ + len;
	    ਨਹੀਂਤਾਂ
		*offp = ਵਿੱਥ - ਮਾਪ + len
		     - (ਪਾਠ_ਸਿਰਾ - ((dp->db_ਸੂਚਕਾਂਕ[idx - 1]) & DB_ਸੂਚਕਾਂਕ_ਨਕਾਬ))
# ifdef FEAT_PROP_POPUP
		     + (ਲੰਮਾ)textprop_ਕੁੱਲ
# endif
		     ;
	    curline += idx - ਸ਼ੁਰੂ_idx + extra;
	    ਜੇ (curline > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ਵਾਪਸ -1;	// exactly one byte beyond the end
	    ਵਾਪਸ curline;
	}
	curline = buf->b_ml.ml_locked_ਉੱਚਾ + 1;
    }

    ਜੇ (lnum != 0)
    {
	// Count extra CR characters.
	ਜੇ (ffdos)
	    ਮਾਪ += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	ਜੇ ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਮਾਪ -= ffdos + 1;
    }

    ਵਾਪਸ ਮਾਪ;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    ਖਾਲੀ
goto_byte(ਲੰਮਾ cnt)
{
    ਲੰਮਾ	boff = cnt;
    linenr_T	lnum;

    ml_flush_ਸਤਰ(curbuf);	// cached line may be dirty
    setpcmark();
    ਜੇ (boff)
	--boff;
    lnum = ml_ਲੱਭੋ_ਸਤਰ_or_ਵਿੱਥ(curbuf, (linenr_T)0, &boff);
    ਜੇ (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	curwin->w_curswant = MAXCOL;
	coladvance((colnr_T)MAXCOL);
    }
    ਨਹੀਂਤਾਂ
    {
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.ਖਾਨਾ = (colnr_T)boff;
	curwin->w_cursor.coladd = 0;
	curwin->w_ਸੈੱਟ_curswant = true;
    }
    check_cursor();

    // Make sure the cursor is on the first byte of a multi-byte char.
    ਜੇ (has_mbyte)
	mb_adjust_cursor();
}
#ਜੇ_ਅੰਤ
