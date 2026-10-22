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
#define CHECK(c, s)	કરો { /**/ } જ્યારે (0)

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

#અન્તર્ભાવ "vim.h"

#અન્તર્ભાવ <time.h>

#જો defined(SASC) || defined(__amigaos4__)
# include <proto/dos.h>	    // for Open() and Close()
#endif

પ્રકારવ્યાખ્યા સંરચના block0		ZERO_BL;    // contents of the first block
પ્રકારવ્યાખ્યા સંરચના pointer_block	PTR_BL;	    // contents of a pointer block
પ્રકારવ્યાખ્યા સંરચના data_block	DATA_BL;    // contents of a data block
પ્રકારવ્યાખ્યા સંરચના pointer_entry	PTR_EN;	    // block/line-count pair

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

#જો defined(FEAT_CRYPT)
સ્થિર અચલ પૂર્ણાંક id1_codes[] = {
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
સંરચના pointer_entry
{
    blocknr_T	pe_bnum;	// block number
    linenr_T	pe_લીટી_ગણતરી;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    પૂર્ણાંક		pe_page_ગણતરી;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
સંરચના pointer_block
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_ગણતરી;	// number of pointers in this block
    short_u	pb_ગણતરી_max;	// maximum value for pb_count
    PTR_EN	pb_pointer[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#define PB_ગણતરી_MAX(mfp) (short_u)(((mfp)->mf_page_size - offsetof(PTR_BL, pb_pointer)) / કદ(PTR_EN))

/*
 * A data block is a leaf in the tree.
 *
 * The text of the lines is at the end of the block. The text of the first line
 * in the block is put at the end, the text of the second line in front of it,
 * etc. Thus the order of the lines is the opposite of the line number.
 */
સંરચના data_block
{
    short_u	db_id;		// ID for data block: DATA_ID
    unsigned	db_free;	// free space available
    unsigned	db_txt_start;	// byte where text starts
    unsigned	db_txt_end;	// byte just after data block
    linenr_T	db_લીટી_ગણતરી;	// number of lines in this block
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
#define DB_MARKED	((unsigned)1 << ((કદ(unsigned) * 8) - 1))
#define DB_INDEX_MASK	(~DB_MARKED)

#define INDEX_SIZE  (કદ(unsigned))	    // size of one db_index entry
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
સંરચના block0
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
    પૂર્ણાંક		b0_magic_int;	// check for byte order of int
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

#define ઢગલો_INCR	5	// nr of entries added to ml_stack at a time

/*
 * The line number where the first mark may be is remembered.
 * If it is 0 there are no marks at all.
 * (always used for the current buffer only, no buffer change possible while
 * executing a global command).
 */
સ્થિર linenr_T	lowest_marked = 0;

/*
 * arguments for ml_find_line()
 */
#define ML_DELETE	0x11	    // delete line
#define ML_ઉમેરો	0x12	    // insert line
#define ML_FIND		0x13	    // just find the line
#define ML_FLUSH	0x02	    // flush locked block
#define ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
પ્રકારવ્યાખ્યા ગણના {
      UB_FNAME = 0	// update timestamp and filename
    , UB_SAME_DIR       // update the B0_SAME_DIR flag
    , UB_CRYPT		// update crypt key
} upd_block0_T;

#ifdef FEAT_CRYPT
સ્થિર શૂન્ય ml_set_b0_crypt(buf_T *buf, ZERO_BL *b0p);
#endif
સ્થિર શૂન્ય ml_upd_block0(buf_T *buf, upd_block0_T what);
સ્થિર શૂન્ય set_b0_fname(ZERO_BL *, buf_T *buf);
સ્થિર શૂન્ય set_b0_dir_flag(ZERO_BL *b0p, buf_T *buf);
સ્થિર શૂન્ય add_b0_fenc(ZERO_BL *b0p, buf_T *buf);
સ્થિર time_t swapfile_info(char_u *);
સ્થિર પૂર્ણાંક recov_file_names(char_u **, char_u *, પૂર્ણાંક prepend_dot);
સ્થિર char_u *findswapname(buf_T *, char_u **, char_u *);
સ્થિર શૂન્ય ml_flush_લીટી(buf_T *);
સ્થિર bhdr_T *ml_new_data(memfile_T *, પૂર્ણાંક, પૂર્ણાંક);
સ્થિર bhdr_T *ml_new_ptr(memfile_T *);
સ્થિર bhdr_T *ml_find_લીટી(buf_T *, linenr_T, પૂર્ણાંક);
સ્થિર પૂર્ણાંક ml_add_ઢગલો(buf_T *);
સ્થિર શૂન્ય ml_lineadd(buf_T *, પૂર્ણાંક);
સ્થિર પૂર્ણાંક b0_magic_wrong(ZERO_BL *);
#ifdef CHECK_INODE
સ્થિર પૂર્ણાંક fnamecmp_ino(char_u *, char_u *, long);
#endif
સ્થિર શૂન્ય long_to_char(long, char_u *);
સ્થિર long char_to_long(char_u *);
#ifdef FEAT_CRYPT
સ્થિર cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T offset, પૂર્ણાંક reading);
#endif
#ifdef FEAT_BYTEOFF
સ્થિર શૂન્ય ml_updatechunk(buf_T *buf, long લીટી, long len, પૂર્ણાંક updtype);
#endif

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
ml_open(buf_T *buf)
{
    memfile_T	*mfp;
    bhdr_T	*hp = ખાલી;
    ZERO_BL	*b0p;
    PTR_BL	*pp;
    DATA_BL	*dp;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ઢગલો_size = 0; // no stack yet
    buf->b_ml.ml_ઢગલો = ખાલી;	// no stack yet
    buf->b_ml.ml_ઢગલો_ટોચ = 0;	// nothing in the stack
    buf->b_ml.ml_locked = ખાલી;	// no cached block
    buf->b_ml.ml_લીટી_lnum = 0;	// no cached line
#ifdef FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = ખાલી;
    buf->b_ml.ml_usedchunks = 0;
#endif

    જો (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    જો (p_uc && buf->b_p_swf)
	buf->b_may_swap = true;
    નહીંતર
	buf->b_may_swap = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(ખાલી, 0);
    જો (mfp == ખાલી)
	જાઓ error;

    buf->b_ml.ml_mfp = mfp;
#ifdef FEAT_CRYPT
    mfp->mf_buffer = buf;
#endif
    buf->b_ml.ml_flags = ML_EMPTY;
    buf->b_ml.ml_લીટી_ગણતરી = 1;

/*
 * fill block0 struct and write page 0
 */
    જો ((hp = mf_new(mfp, FALSE, 1)) == ખાલી)
	જાઓ error;
    જો (hp->bh_bnum != 0)
    {
	iemsg(e_didnt_get_block_nr_zero);
	જાઓ error;
    }
    b0p = (ZERO_BL *)(hp->bh_data);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (long)B0_MAGIC_LONG;
    b0p->b0_magic_int = (પૂર્ણાંક)B0_MAGIC_INT;
    b0p->b0_magic_short = (short)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_version, "VIM ", 4);
    STRNCPY(b0p->b0_version + 4, Version, 6);
    long_to_char((long)mfp->mf_page_size, b0p->b0_page_size);

#ifdef FEAT_SPELL
    જો (!buf->b_spell)
#endif
    {
	b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	b0p->b0_flags = get_fileformat(buf) + 1;
	set_b0_fname(b0p, buf);
	(શૂન્ય)get_user_name(b0p->b0_uname, B0_UNAME_SIZE);
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
    જો (!buf->b_help && !B_SPELL(buf))
	(શૂન્ય)mf_sync(mfp, 0);

    /*
     * Fill in root pointer block and write page 1.
     */
    જો ((hp = ml_new_ptr(mfp)) == ખાલી)
	જાઓ error;
    જો (hp->bh_bnum != 1)
    {
	iemsg(e_didnt_get_block_nr_one);
	જાઓ error;
    }
    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_ગણતરી = 1;
    pp->pb_pointer[0].pe_bnum = 2;
    pp->pb_pointer[0].pe_page_ગણતરી = 1;
    pp->pb_pointer[0].pe_old_lnum = 1;
    pp->pb_pointer[0].pe_લીટી_ગણતરી = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    જો ((hp = ml_new_data(mfp, FALSE, 1)) == ખાલી)
	જાઓ error;
    જો (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_block_nr_two);
	જાઓ error;
    }

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[0] = --dp->db_txt_start;	// at end of block
    dp->db_free -= 1 + INDEX_SIZE;
    dp->db_લીટી_ગણતરી = 1;
    *((char_u *)dp + dp->db_txt_start) = NUL;	// empty line

    પાછા OK;

error:
    જો (mfp != ખાલી)
    {
	જો (hp)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, TRUE);	    // will also free(mfp->mf_fname)
    }
    buf->b_ml.ml_mfp = ખાલી;
    પાછા FAIL;
}

#જો defined(FEAT_CRYPT)
/*
 * Swapfile encryption is not supported by XChaCha20.  If this crypt method is
 * used then disable the swapfile, to avoid plain text being written to disk,
 * and return TRUE.
 * Otherwise return FALSE.
 */
    સ્થિર પૂર્ણાંક
crypt_may_close_swapfile(buf_T *buf, char_u *ચાવી, પૂર્ણાંક method)
{
    જો (crypt_method_is_sodium(method) && *ચાવી != NUL)
    {
	mf_close_file(buf, TRUE);
	buf->b_p_swf = FALSE;
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Prepare encryption for "buf" for the current key and method.
 */
    સ્થિર શૂન્ય
ml_set_mfp_crypt(buf_T *buf)
{
    જો (*buf->b_p_ચાવી == NUL)
	પાછા;

    પૂર્ણાંક method_nr = crypt_get_method_nr(buf);

    જો (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_seed(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN, ખાલી, 0);
    }
# ifdef FEAT_SODIUM
    નહીંતર જો (crypt_method_is_sodium(method_nr))
	crypt_sodium_randombytes_buf(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN);
# endif
}

/*
 * Prepare encryption for "buf" with block 0 "b0p".
 * Note: should not be called with libsodium encryption, since xchacha20 does
 * not support swapfile encryption.
 */
    સ્થિર શૂન્ય
ml_set_b0_crypt(buf_T *buf, ZERO_BL *b0p)
{
    જો (*buf->b_p_ચાવી == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    નહીંતર
    {
	પૂર્ણાંક method_nr = crypt_get_method_nr(buf);

	b0p->b0_id[1] = id1_codes[method_nr];
	જો (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
	{
	    // Generate a seed and store it in block 0 and in the memfile.
	    sha2_seed(&b0p->b0_seed, MF_SEED_LEN, ખાલી, 0);
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
    શૂન્ય
ml_set_crypt_ચાવી(
    buf_T	*buf,
    char_u	*old_ચાવી,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    પૂર્ણાંક		page_ગણતરી;
    પૂર્ણાંક		idx;
    long	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    DATA_BL	*dp;
    blocknr_T	bnum;
    પૂર્ણાંક		ટોચ;
    પૂર્ણાંક		old_method;

    જો (mfp == ખાલી || mfp->mf_fd < 0)
	પાછા;  // no memfile yet, nothing to do
    old_method = crypt_method_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    જો (crypt_may_close_swapfile(buf, buf->b_p_ચાવી, crypt_get_method_nr(buf)))
	પાછા;
# endif

    // First make sure the swapfile is in a consistent state, using the old
    // key and method.
    {
	char_u *new_ચાવી = buf->b_p_ચાવી;
	char_u *new_buf_cm = buf->b_p_cm;

	buf->b_p_ચાવી = old_ચાવી;
	buf->b_p_cm = old_cm;
	ml_preserve(buf, FALSE);
	buf->b_p_ચાવી = new_ચાવી;
	buf->b_p_cm = new_buf_cm;
    }

    // Set the key, method and seed to be used for reading, these must be the
    // old values.
    mfp->mf_old_ચાવી = old_ચાવી;
    mfp->mf_old_cm = old_method;
    જો (old_method > 0 && *old_ચાવી != NUL)
	mch_memmove(mfp->mf_old_seed, mfp->mf_seed, MF_SEED_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    જો (mfp->mf_infile_ગણતરી > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_લીટી(buf);		    // flush buffered line
	(શૂન્ય)ml_find_લીટી(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = ખાલી;
	bnum = 1;		// start with block 1
	page_ગણતરી = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_ઢગલો_ટોચ = 0;
	VIM_CLEAR(buf->b_ml.ml_ઢગલો);
	buf->b_ml.ml_ઢગલો_size = 0;	// no stack yet

	માટે ( ; !got_int; લીટી_breakcheck())
	{
	    જો (hp != ખાલી)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    જો ((hp = mf_get(mfp, bnum, page_ગણતરી)) == ખાલી)
	    {
		જો (bnum == 1)
		    તોડો;
		++error;
	    }
	    નહીંતર
	    {
		pp = (PTR_BL *)(hp->bh_data);
		જો (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    જો (pp->pb_ગણતરી == 0)
		    {
			// empty block?
			++error;
		    }
		    નહીંતર જો (idx < (પૂર્ણાંક)pp->pb_ગણતરી)	// go a block deeper
		    {
			જો (pp->pb_pointer[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    ચાલુ;
			}

			// going one block deeper in the tree, new entry in
			//ઢગલો
			જો ((ટોચ = ml_add_ઢગલો(buf)) < 0)
			{
			    ++error;
			    તોડો;		    // out of memory
			}
			ip = &(buf->b_ml.ml_ઢગલો[ટોચ]);
			ip->ip_bnum = bnum;
			ip->ip_index = idx;

			bnum = pp->pb_pointer[idx].pe_bnum;
			page_ગણતરી = pp->pb_pointer[idx].pe_page_ગણતરી;
			idx = 0;
			ચાલુ;
		    }
		}
		નહીંતર	    // not a pointer block
		{
		    dp = (DATA_BL *)(hp->bh_data);
		    જો (dp->db_id != DATA_ID)	// block id wrong
			++error;
		    નહીંતર
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = ખાલી;
		    }
		}
	    }

	    જો (buf->b_ml.ml_ઢગલો_ટોચ == 0)	// finished
		તોડો;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_ઢગલો[--(buf->b_ml.ml_ઢગલો_ટોચ)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_index + 1;	    // go to next index
	    page_ગણતરી = 1;
	}
	જો (hp != ખાલી)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	જો (error > 0)
	    emsg(_(e_error_while_updating_swap_file_crypt));
    }

    mfp->mf_old_ચાવી = ખાલી;
}
#endif

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
    શૂન્ય
ml_setname(buf_T *buf)
{
    પૂર્ણાંક		success = FALSE;
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;
#જો defined(MSWIN)
    char_u	*p;
#endif

    mfp = buf->b_ml.ml_mfp;
    જો (mfp->mf_fd < 0)		    // there is no swap file yet
    {
	/*
	 * When 'updatecount' is 0 and 'noswapfile' there is no swap file.
	 * For help files we will make a swap file now.
	 */
	જો (p_uc != 0 && (cmdmod.cmod_flags & CMOD_NOSWAPFILE) == 0)
	    ml_open_file(buf);	    // create a swap file
	પાછા;
    }

    /*
     * Try all directories in the 'directory' option.
     */
    dirp = p_dir;
    માટે (;;)
    {
	જો (*dirp == NUL)	    // tried all directories, fail
	    તોડો;
	fname = findswapname(buf, &dirp, mfp->mf_fname);
	// autocmd may have freed mfp if findswapname creates
	// different swapfile name
	જો (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    પાછા;
	}
						    // alloc's fname
	જો (dirp == ખાલી)	    // out of memory
	    તોડો;
	જો (fname == ખાલી)	    // no file name found for this dir
	    ચાલુ;

#જો defined(MSWIN)
	/*
	 * Set full pathname for swap file now, because a ":!cd dir" may
	 * change directory without us knowing it.
	 */
	p = FullName_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	જો (fname == ખાલી)
	    ચાલુ;
#endif
	// if the file name is the same we don't have to do anything
	જો (fnamecmp(fname, mfp->mf_fname) == 0)
	{
	    vim_free(fname);
	    success = TRUE;
	    તોડો;
	}
	// need to close the swap file before renaming
	જો (mfp->mf_fd >= 0)
	{
	    close(mfp->mf_fd);
	    mfp->mf_fd = -1;
	}

	// try to rename the swap file
	જો (vim_rename(mfp->mf_fname, fname) == 0)
	{
	    success = TRUE;
	    vim_free(mfp->mf_fname);
	    mfp->mf_fname = fname;
	    vim_free(mfp->mf_ffname);
#જો defined(MSWIN)
	    mfp->mf_ffname = ખાલી;  // mf_fname is full pathname already
#નહીંતર
	    mf_set_ffname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);
	    તોડો;
	}
	vim_free(fname);	    // this fname didn't work, try another
    }

    જો (mfp->mf_fd == -1)	    // need to (re)open the swap file
    {
	mfp->mf_fd = mch_open((અક્ષર *)mfp->mf_fname, O_RDWR | O_EXTRA, 0);
	જો (mfp->mf_fd < 0)
	{
	    // could not (re)open the swap file, what can we do????
	    emsg(_(e_oops_lost_the_swap_file));
	    પાછા;
	}
#ifdef HAVE_FD_CLOEXEC
	{
	    પૂર્ણાંક fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    જો (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(શૂન્ય)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#endif
    }
    જો (!success)
	emsg(_(e_could_not_rename_swap_file));
}

/*
 * Open a file for the memfile for all buffers that are not readonly or have
 * been modified.
 * Used when 'updatecount' changes from zero to non-zero.
 */
    શૂન્ય
ml_open_files(શૂન્ય)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	જો (!buf->b_p_ro || buf->b_changed)
	    ml_open_file(buf);
}

/*
 * Open a swap file for an existing memfile, if there is no swap file yet.
 * If we are unable to find a file name, mf_fname will be NULL
 * and the memfile will be in memory only (no recovery possible).
 */
    શૂન્ય
ml_open_file(buf_T *buf)
{
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;

    mfp = buf->b_ml.ml_mfp;
    જો (mfp == ખાલી || mfp->mf_fd >= 0 || !buf->b_p_swf
				      || (cmdmod.cmod_flags & CMOD_NOSWAPFILE))
	પાછા;		// nothing to do

#ifdef FEAT_SPELL
    // For a spell buffer use a temp file name.
    જો (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	જો (fname != ખાલી)
	    (શૂન્ય)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_swap = false;
	પાછા;
    }
#endif

    /*
     * Try all directories in 'directory' option.
     */
    dirp = p_dir;
    માટે (;;)
    {
	જો (*dirp == NUL)
	    તોડો;
	// There is a small chance that between choosing the swap file name
	// and creating it, another Vim creates the file.  In that case the
	// creation will fail and we will use another directory.
	fname = findswapname(buf, &dirp, ખાલી); // allocates fname
	// autocmd may have freed mfp, grr!
	જો (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    પાછા;
	}
	જો (dirp == ખાલી)
	    તોડો;  // out of memory
	જો (fname == ખાલી)
	    ચાલુ;
	જો (mf_open_file(mfp, fname) == OK)	// consumes fname!
	{
	    // don't sync yet in ml_sync_all()
	    mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;
#જો defined(MSWIN)
	    /*
	     * set full pathname for swap file now, because a ":!cd dir" may
	     * change directory without us knowing it.
	     */
	    mf_fullname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    જો (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_set_dirty(mfp);
		તોડો;
	    }
	    // Writing block 0 failed: close the file and try another dir
	    mf_close_file(buf, FALSE);
	}
    }

    જો (*p_dir != NUL && mfp->mf_fname == ખાલી)
    {
	need_wait_return = TRUE;	// call wait_return() later
	++no_wait_return;
	(શૂન્ય)semsg(_(e_unable_to_open_swap_file_for_str_recovery_impossible),
		    buf_spname(buf) != ખાલી ? buf_spname(buf) : buf->b_fname);
	--no_wait_return;
    }

    // don't try to open a swap file again
    buf->b_may_swap = false;
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
    શૂન્ય
check_need_swap(
    પૂર્ણાંક	    newfile)		// reading file into new buffer
{
    પૂર્ણાંક old_સંદેશ_silent = સંદેશ_silent; // might be reset by an E325 message

    જો (curbuf->b_may_swap && (!curbuf->b_p_ro || !newfile))
	ml_open_file(curbuf);
    સંદેશ_silent = old_સંદેશ_silent;
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
    શૂન્ય
ml_close(buf_T *buf, પૂર્ણાંક del_file)
{
    જો (buf->b_ml.ml_mfp == ખાલી)		// not open
	પાછા;
    mf_close(buf->b_ml.ml_mfp, del_file);	// close the .swp file
    જો (buf->b_ml.ml_લીટી_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_લીટી_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_લીટી_ptr);
    vim_free(buf->b_ml.ml_ઢગલો);
#ifdef FEAT_BYTEOFF
    VIM_CLEAR(buf->b_ml.ml_chunksize);
#endif
    buf->b_ml.ml_mfp = ખાલી;

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
    શૂન્ય
ml_close_all(પૂર્ણાંક del_file)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ml_close(buf, del_file && ((buf->b_flags & BF_PRESERVED) == 0
				 || vim_strchr(p_cpo, CPO_PRESERVE) == ખાલી));
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
    શૂન્ય
ml_close_notmod(શૂન્ય)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	જો (!bufIsChanged(buf))
	    ml_close(buf, TRUE);    // close all not-modified buffers
}

/*
 * Update the timestamp in the .swp file.
 * Used when the file has been written.
 */
    શૂન્ય
ml_timestamp(buf_T *buf)
{
    ml_upd_block0(buf, UB_FNAME);
}

/*
 * Return FAIL when the ID of "b0p" is wrong.
 */
    સ્થિર પૂર્ણાંક
ml_check_b0_id(ZERO_BL *b0p)
{
    જો (b0p->b0_id[0] != BLOCK0_ID0
	    || (b0p->b0_id[1] != BLOCK0_ID1
		&& b0p->b0_id[1] != BLOCK0_ID1_C0
		&& b0p->b0_id[1] != BLOCK0_ID1_C1
		&& b0p->b0_id[1] != BLOCK0_ID1_C2
		&& b0p->b0_id[1] != BLOCK0_ID1_C3
		&& b0p->b0_id[1] != BLOCK0_ID1_C4)
	    )
	પાછા FAIL;
    પાછા OK;
}

/*
 * Update the timestamp or the B0_SAME_DIR flag of the .swp file.
 */
    સ્થિર શૂન્ય
ml_upd_block0(buf_T *buf, upd_block0_T what)
{
    memfile_T	*mfp;
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    mfp = buf->b_ml.ml_mfp;
    જો (mfp == ખાલી)
	પાછા;
    hp = mf_get(mfp, (blocknr_T)0, 1);
    જો (hp == ખાલી)
    {
#ifdef FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	જો (what == UB_CRYPT)
	    ml_set_mfp_crypt(buf);
#endif
	પાછા;
    }

    b0p = (ZERO_BL *)(hp->bh_data);
    જો (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_block_zero);
    નહીંતર
    {
	જો (what == UB_FNAME)
	    set_b0_fname(b0p, buf);
#ifdef FEAT_CRYPT
	નહીંતર જો (what == UB_CRYPT)
	    ml_set_b0_crypt(buf, b0p);
#endif
	નહીંતર // what == UB_SAME_DIR
	    set_b0_dir_flag(b0p, buf);
    }
    mf_put(mfp, hp, TRUE, FALSE);
}

/*
 * Write file name and timestamp into block 0 of a swap file.
 * Also set buf->b_mtime.
 * Don't use NameBuff[]!!!
 */
    સ્થિર શૂન્ય
set_b0_fname(ZERO_BL *b0p, buf_T *buf)
{
    stat_T	st;

    જો (buf->b_ffname == ખાલી)
	b0p->b0_fname[0] = NUL;
    નહીંતર
    {
#જો defined(MSWIN) || defined(AMIGA)
	// Systems that cannot translate "~user" back into a path: copy the
	// file name unmodified.  Do use slashes instead of backslashes for
	// portability.
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_SIZE_CRYPT - 1);
# ifdef BACKSLASH_IN_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#નહીંતર
	size_t	flen;
	char_u	uname[B0_UNAME_SIZE];

	/*
	 * For a file under the home directory of the current user, we try to
	 * replace the home directory path with "~user". This helps when
	 * editing the same file on different machines over a network.
	 * First replace home dir path with "~/" with home_replace().
	 * Then insert the user name to get "~user/".
	 */
	flen = home_replace(ખાલી, buf->b_ffname, b0p->b0_fname,
						   B0_FNAME_SIZE_CRYPT, TRUE);
	જો (b0p->b0_fname[0] == '~')
	{
	    size_t  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    જો (get_user_name(uname, B0_UNAME_SIZE) == FAIL
		   || (ulen = STRLEN(uname)) + flen > B0_FNAME_SIZE_CRYPT - 1)
		vim_strncpy(b0p->b0_fname, buf->b_ffname,
						     B0_FNAME_SIZE_CRYPT - 1);
	    નહીંતર
	    {
		mch_memmove(b0p->b0_fname + ulen + 1, b0p->b0_fname + 1, flen);
		mch_memmove(b0p->b0_fname + 1, uname, ulen);
	    }
	}
#endif
	જો (mch_stat((અક્ષર *)buf->b_ffname, &st) >= 0)
	{
	    long_to_char((long)st.st_mtime, b0p->b0_mtime);
#ifdef CHECK_INODE
	    long_to_char((long)st.st_ino, b0p->b0_ino);
#endif
	    buf_store_time(buf, &st, buf->b_ffname);
	    buf->b_mtime_read = buf->b_mtime;
	    buf->b_mtime_read_ns = buf->b_mtime_ns;
	}
	નહીંતર
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
    સ્થિર શૂન્ય
set_b0_dir_flag(ZERO_BL *b0p, buf_T *buf)
{
    જો (same_directory(buf->b_ml.ml_mfp->mf_fname, buf->b_ffname))
	b0p->b0_flags |= B0_SAME_DIR;
    નહીંતર
	b0p->b0_flags &= ~B0_SAME_DIR;
}

/*
 * When there is room, add the 'fileencoding' to block zero.
 */
    સ્થિર શૂન્ય
add_b0_fenc(
    ZERO_BL	*b0p,
    buf_T	*buf)
{
    પૂર્ણાંક		n;
    પૂર્ણાંક		size = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    જો (*buf->b_p_ચાવી != NUL)
	size = B0_FNAME_SIZE_CRYPT;
#endif

    n = (પૂર્ણાંક)STRLEN(buf->b_p_fenc);
    જો ((પૂર્ણાંક)STRLEN(b0p->b0_fname) + n + 1 > size)
	b0p->b0_flags &= ~B0_HAS_FENC;
    નહીંતર
    {
	mch_memmove((અક્ષર *)b0p->b0_fname + size - n,
					    (અક્ષર *)buf->b_p_fenc, (size_t)n);
	*(b0p->b0_fname + size - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#જો defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#endif

#જો defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    સ્થિર પૂર્ણાંક
swapfile_process_running(ZERO_BL *b0p, char_u *swap_fname UNUSED)
{
# જો defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    સંરચના sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    જો (mch_stat((અક્ષર *)swap_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < time(ખાલી) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	પાછા FALSE;
# endif
    પાછા mch_process_running(char_to_long(b0p->b0_pid));
}
#endif

/*
 * Try to recover curbuf from the .swp file.
 * If "checkext" is TRUE, check the extension and detect whether it is
 * a swap file.
 */
    શૂન્ય
ml_recover(પૂર્ણાંક checkext)
{
    buf_T	*buf = ખાલી;
    memfile_T	*mfp = ખાલી;
    char_u	*fname;
    char_u	*fname_used = ખાલી;
    bhdr_T	*hp = ખાલી;
    ZERO_BL	*b0p;
    પૂર્ણાંક		b0_ff;
    char_u	*b0_fenc = ખાલી;
#ifdef FEAT_CRYPT
    પૂર્ણાંક		b0_cm = -1;
#endif
    PTR_BL	*pp;
    DATA_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    પૂર્ણાંક		page_ગણતરી;
    stat_T	org_stat, swp_stat;
    પૂર્ણાંક		len;
    પૂર્ણાંક		directly;
    linenr_T	lnum;
    char_u	*p;
    પૂર્ણાંક		i;
    long	error;
    પૂર્ણાંક		cannot_open;
    linenr_T	લીટી_ગણતરી;
    પૂર્ણાંક		has_error;
    પૂર્ણાંક		idx;
    પૂર્ણાંક		ટોચ;
    પૂર્ણાંક		txt_start;
    off_T	size;
    પૂર્ણાંક		called_from_main;
    પૂર્ણાંક		serious_error = TRUE;
    long	mtime;
    પૂર્ણાંક		attr;
    પૂર્ણાંક		orig_file_status = NOTDONE;

    recoverymode = TRUE;
    called_from_main = (curbuf->b_ml.ml_mfp == ખાલી);
    attr = HL_ATTR(HLF_E);

    /*
     * If the file name ends in ".s[a-w][a-z]" we assume this is the swap file.
     * Otherwise a search is done to find the swap file(s).
     */
    fname = curbuf->b_fname;
    જો (fname == ખાલી)		    // When there is no file name
	fname = (char_u *)"";
    len = (પૂર્ણાંક)STRLEN(fname);
    જો (checkext && len >= 4 &&
#જો defined(VMS)
	    STRNICMP(fname + len - 4, "_s", 2)
#નહીંતર
	    STRNICMP(fname + len - 4, ".s", 2)
#endif
						== 0
		&& vim_strchr((char_u *)"abcdefghijklmnopqrstuvw",
					   TOLOWER_ASC(fname[len - 2])) != ખાલી
		&& ASCII_ISALPHA(fname[len - 1]))
    {
	directly = TRUE;
	fname_used = vim_strsave(fname); // make a copy for mf_open()
    }
    નહીંતર
    {
	directly = FALSE;

	// count the number of matching swap files
	len = recover_names(fname, FALSE, ખાલી, 0, ખાલી);
	જો (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_swap_file_found_for_str), fname);
	    જાઓ theend;
	}
	જો (len == 1)		    // one swap file found, use it
	    i = 1;
	નહીંતર			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (શૂન્ય)recover_names(fname, TRUE, ખાલી, 0, ખાલી);
	    સંદેશ_putchar('\n');
	    સંદેશ_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_સંખ્યા(FALSE, ખાલી);
	    જો (i < 1 || i > len)
		જાઓ theend;
	}
	// get the swap file name that will be used
	(શૂન્ય)recover_names(fname, FALSE, ખાલી, i, &fname_used);
    }
    જો (fname_used == ખાલી)
	જાઓ theend;			// out of memory

    // When called from main() still need to initialize storage structure
    જો (called_from_main && ml_open(curbuf) == FAIL)
	getout(1);

    /*
     * Allocate a buffer structure for the swap file that is used for recovery.
     * Only the memline and crypt information in it are really used.
     */
    buf = ALLOC_ONE(buf_T);
    જો (buf == ખાલી)
	જાઓ theend;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ઢગલો_size = 0;	// no stack yet
    buf->b_ml.ml_ઢગલો = ખાલી;		// no stack yet
    buf->b_ml.ml_ઢગલો_ટોચ = 0;		// nothing in the stack
    buf->b_ml.ml_લીટી_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = ખાલી;		// no locked block
    buf->b_ml.ml_flags = 0;
#ifdef FEAT_CRYPT
    buf->b_p_ચાવી = empty_option;
    buf->b_p_cm = empty_option;
#endif

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_used); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_used, O_RDONLY);
    fname_used = p;
    જો (mfp == ખાલી || mfp->mf_fd < 0)
    {
	જો (fname_used != ખાલી)
	    semsg(_(e_cannot_open_str), fname_used);
	જાઓ theend;
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
    જો ((hp = mf_get(mfp, (blocknr_T)0, 1)) == ખાલી)
    {
	સંદેશ_start();
	સંદેશ_puts_attr(_("Unable to read block 0 from "), attr | સંદેશ_HIST);
	સંદેશ_outtrans_attr(mfp->mf_fname, attr | સંદેશ_HIST);
	સંદેશ_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | સંદેશ_HIST);
	સંદેશ_end();
	જાઓ theend;
    }
    b0p = (ZERO_BL *)(hp->bh_data);
    જો (STRNCMP(b0p->b0_version, "VIM 3.0", 7) == 0)
    {
	સંદેશ_start();
	સંદેશ_outtrans_attr(mfp->mf_fname, સંદેશ_HIST);
	સંદેશ_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    સંદેશ_HIST);
	સંદેશ_puts_attr(_("Use Vim version 3.0.\n"), સંદેશ_HIST);
	સંદેશ_end();
	જાઓ theend;
    }
    જો (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_swap_file), mfp->mf_fname);
	જાઓ theend;
    }
    જો (b0_magic_wrong(b0p))
    {
	સંદેશ_start();
	સંદેશ_outtrans_attr(mfp->mf_fname, attr | સંદેશ_HIST);
#જો defined(MSWIN)
	જો (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    સંદેશ_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | સંદેશ_HIST);
	નહીંતર
#endif
	    સંદેશ_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | સંદેશ_HIST);
	સંદેશ_puts_attr(_("The file was created on "), attr | સંદેશ_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	સંદેશ_puts_attr((અક્ષર *)b0p->b0_hname, attr | સંદેશ_HIST);
	સંદેશ_puts_attr(_(",\nor the file has been damaged."), attr | સંદેશ_HIST);
	સંદેશ_end();
	જાઓ theend;
    }

#ifdef FEAT_CRYPT
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(id1_codes); ++i)
	જો (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    જો (b0_cm > 0)
	mch_memmove(mfp->mf_seed, &b0p->b0_seed, MF_SEED_LEN);
    crypt_set_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#નહીંતર
    જો (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_version_of_vim_does_not_support_encryption), mfp->mf_fname);
	જાઓ theend;
    }
#endif

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    જો (mfp->mf_page_size != (unsigned)char_to_long(b0p->b0_page_size))
    {
	unsigned previous_page_size = mfp->mf_page_size;

	mf_new_page_size(mfp, (unsigned)char_to_long(b0p->b0_page_size));
	જો (mfp->mf_page_size < previous_page_size)
	{
	    સંદેશ_start();
	    સંદેશ_outtrans_attr(mfp->mf_fname, attr | સંદેશ_HIST);
	    સંદેશ_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | સંદેશ_HIST);
	    સંદેશ_end();
	    જાઓ theend;
	}
	જો ((size = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	નહીંતર
	    mfp->mf_blocknr_max = (blocknr_T)(size / mfp->mf_page_size);
	mfp->mf_infile_ગણતરી = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_page_size);
	જો (p == ખાલી)
	    જાઓ theend;
	mch_memmove(p, hp->bh_data, previous_page_size);
	vim_free(hp->bh_data);
	hp->bh_data = p;
	b0p = (ZERO_BL *)(hp->bh_data);
    }

    /*
     * If .swp file name given directly, use name from swap file for buffer.
     */
    જો (directly)
    {
	expand_env(b0p->b0_fname, NameBuff, MAXPATHL);
	જો (setfname(curbuf, NameBuff, ખાલી, TRUE) == FAIL)
	    જાઓ theend;
    }

    home_replace(ખાલી, mfp->mf_fname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Using swap file \"%s\""), NameBuff);

    જો (buf_spname(curbuf) != ખાલી)
	vim_strncpy(NameBuff, buf_spname(curbuf), MAXPATHL - 1);
    નહીંતર
	home_replace(ખાલી, curbuf->b_ffname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Original file \"%s\""), NameBuff);
    સંદેશ_putchar('\n');

    /*
     * check date of swap file and original file
     */
    mtime = char_to_long(b0p->b0_mtime);
    જો (curbuf->b_ffname != ખાલી
	    && mch_stat((અક્ષર *)curbuf->b_ffname, &org_stat) != -1
	    && ((mch_stat((અક્ષર *)mfp->mf_fname, &swp_stat) != -1
		    && org_stat.st_mtime > swp_stat.st_mtime)
		|| org_stat.st_mtime != mtime))
	emsg(_(e_warning_original_file_may_have_been_changed));
    out_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_MASK);
    જો (b0p->b0_flags & B0_HAS_FENC)
    {
	પૂર્ણાંક fnsize = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	જો (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_SIZE_CRYPT;
#endif
	માટે (p = b0p->b0_fname + fnsize; p > b0p->b0_fname && p[-1] != NUL; --p)
	    ;
	b0_fenc = vim_strnsave(p, b0p->b0_fname + fnsize - p);
    }

    mf_put(mfp, hp, FALSE, FALSE);	// release block 0
    hp = ખાલી;

    /*
     * Now that we are sure that the file is going to be recovered, clear the
     * contents of the current buffer.
     */
    જ્યારે (!(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    જો (curbuf->b_ffname != ખાલી)
	orig_file_status = readfile(curbuf->b_ffname, ખાલી, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, ખાલી, READ_NEW);

#ifdef FEAT_CRYPT
    જો (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	જો (*curbuf->b_p_ચાવી != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_used);
	    સંદેશ_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    સંદેશ_puts(_("\nenter the new crypt key."));
	    સંદેશ_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    સંદેશ_puts(_("\nto use the same key for text file and swap file"));
	}
	નહીંતર
	    smsg(_(need_ચાવી_સંદેશ), fname_used);
	buf->b_p_ચાવી = crypt_get_ચાવી(FALSE, FALSE);
	જો (buf->b_p_ચાવી == ખાલી)
	    buf->b_p_ચાવી = curbuf->b_p_ચાવી;
	નહીંતર જો (*buf->b_p_ચાવી == NUL)
	{
	    vim_free(buf->b_p_ચાવી);
	    buf->b_p_ચાવી = curbuf->b_p_ચાવી;
	}
	જો (buf->b_p_ચાવી == ખાલી)
	    buf->b_p_ચાવી = empty_option;
    }
#endif

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    જો (b0_ff != 0)
	set_fileformat(b0_ff - 1, OPT_LOCAL);
    જો (b0_fenc != ખાલી)
    {
	set_option_કિંમત_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    page_ગણતરી = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    લીટી_ગણતરી = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_ઢગલો_ટોચ = 0;
    buf->b_ml.ml_ઢગલો = ખાલી;
    buf->b_ml.ml_ઢગલો_size = 0;	// no stack yet

    જો (curbuf->b_ffname == ખાલી)
	cannot_open = TRUE;
    નહીંતર
	cannot_open = FALSE;

    serious_error = FALSE;
    માટે ( ; !got_int; લીટી_breakcheck())
    {
	જો (hp != ખાલી)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	જો ((hp = mf_get(mfp, bnum, page_ગણતરી)) == ખાલી)
	{
	    જો (bnum == 1)
	    {
		semsg(_(e_unable_to_read_block_one_from_str), mfp->mf_fname);
		જાઓ theend;
	    }
	    ++error;
	    ml_append(lnum++, (char_u *)_("???MANY LINES MISSING"),
							    (colnr_T)0, TRUE);
	}
	નહીંતર		// there is a block
	{
	    pp = (PTR_BL *)(hp->bh_data);
	    જો (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		પૂર્ણાંક ptr_block_error = FALSE;
		જો (pp->pb_ગણતરી_max != PB_ગણતરી_MAX(mfp))
		{
		    ptr_block_error = TRUE;
		    pp->pb_ગણતરી_max = PB_ગણતરી_MAX(mfp);
		}
		જો (pp->pb_ગણતરી > pp->pb_ગણતરી_max)
		{
		    ptr_block_error = TRUE;
		    pp->pb_ગણતરી = pp->pb_ગણતરી_max;
		}
		જો (ptr_block_error)
		    emsg(_(e_warning_pointer_block_corrupted));

		// check line count when using pointer block first time
		જો (idx == 0 && લીટી_ગણતરી != 0)
		{
		    માટે (i = 0; i < (પૂર્ણાંક)pp->pb_ગણતરી; ++i)
			લીટી_ગણતરી -= pp->pb_pointer[i].pe_લીટી_ગણતરી;
		    જો (લીટી_ગણતરી != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		જો (pp->pb_ગણતરી == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		નહીંતર જો (idx < (પૂર્ણાંક)pp->pb_ગણતરી)	// go a block deeper
		{
		    જો (pp->pb_pointer[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			જો (!cannot_open)
			{
			    લીટી_ગણતરી = pp->pb_pointer[idx].pe_લીટી_ગણતરી;
			    linenr_T pe_old_lnum = pp->pb_pointer[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    જો (લીટી_ગણતરી <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, ખાલી, lnum,
					pe_old_lnum - 1,
					લીટી_ગણતરી, ખાલી, 0) != OK)
				cannot_open = TRUE;
			    નહીંતર
				lnum += લીટી_ગણતરી;
			}
			જો (cannot_open)
			{
			    ++error;
			    ml_append(lnum++, (char_u *)_("???LINES MISSING"),
							    (colnr_T)0, TRUE);
			}
			++idx;	    // get same block again for next index
			ચાલુ;
		    }

		    /*
		     * going one block deeper in the tree
		     */
		    જો ((ટોચ = ml_add_ઢગલો(buf)) < 0)	// new entry in stack
		    {
			++error;
			તોડો;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_ઢગલો[ટોચ]);
		    ip->ip_bnum = bnum;
		    ip->ip_index = idx;

		    bnum = pp->pb_pointer[idx].pe_bnum;
		    લીટી_ગણતરી = pp->pb_pointer[idx].pe_લીટી_ગણતરી;
		    page_ગણતરી = pp->pb_pointer[idx].pe_page_ગણતરી;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    જો (page_ગણતરી < 1
			    || bnum + page_ગણતરી > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_index + 1;
			bnum = ip->ip_bnum;
			page_ગણતરી = 1;
			--buf->b_ml.ml_ઢગલો_ટોચ;
			ચાલુ;
		    }
		    idx = 0;
		    ચાલુ;
		}
	    }
	    નહીંતર	    // not a pointer block
	    {
		dp = (DATA_BL *)(hp->bh_data);
		જો (dp->db_id != DATA_ID)	// block id wrong
		{
		    જો (bnum == 1)
		    {
			semsg(_(e_block_one_id_wrong_str_not_swp_file),
							       mfp->mf_fname);
			જાઓ theend;
		    }
		    ++error;
		    ml_append(lnum++, (char_u *)_("???BLOCK MISSING"),
							    (colnr_T)0, TRUE);
		}
		નહીંતર
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
		    જો (hp->bh_page_ગણતરી != page_ગણતરી)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			page_ગણતરી = hp->bh_page_ગણતરી;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    જો (page_ગણતરી * mfp->mf_page_size != dp->db_txt_end)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_end = page_ગણતરી * mfp->mf_page_size;
		    }

		    જો (dp->db_txt_start < HEADER_SIZE
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
		    જો (લીટી_ગણતરી != dp->db_લીટી_ગણતરી)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    પૂર્ણાંક did_questions = FALSE;
		    માટે (i = 0; i < dp->db_લીટી_ગણતરી; ++i)
		    {
			જો ((char_u *)&(dp->db_index[i])
					    >= (char_u *)dp + dp->db_txt_start)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    તોડો;
			}

			txt_start = (dp->db_index[i] & DB_INDEX_MASK);
			જો (txt_start <= (પૂર્ણાંક)HEADER_SIZE
					  || txt_start >= (પૂર્ણાંક)dp->db_txt_end)
			{
			    ++error;
			    // avoid lots of lines with "???"
			    જો (did_questions)
				ચાલુ;
			    did_questions = TRUE;
			    p = (char_u *)"???";
			}
			નહીંતર
			{
			    did_questions = FALSE;
			    p = (char_u *)dp + txt_start;
			}
			ml_append(lnum++, p, (colnr_T)0, TRUE);
		    }
		    જો (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	જો (buf->b_ml.ml_ઢગલો_ટોચ == 0)	// finished
	    તોડો;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_ઢગલો[--(buf->b_ml.ml_ઢગલો_ટોચ)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_index + 1;	    // go to next index
	page_ગણતરી = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    જો (orig_file_status != OK || curbuf->b_ml.ml_લીટી_ગણતરી != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	જો (!(curbuf->b_ml.ml_લીટી_ગણતરી == 2 && *ml_get(1) == NUL))
	{
	    changed_internal();
	    ++CHANGEDTICK(curbuf);
	}
    }
    નહીંતર
    {
	માટે (idx = 1; idx <= lnum; ++idx)
	{
	    // Need to copy one line, fetching the other one may flush it.
	    p = vim_strnsave(ml_get(idx), ml_get_len(idx));
	    i = STRCMP(p, ml_get(idx + lnum));
	    vim_free(p);
	    જો (i != 0)
	    {
		changed_internal();
		++CHANGEDTICK(curbuf);
		તોડો;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    જ્યારે (curbuf->b_ml.ml_લીટી_ગણતરી > lnum
				       && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete(curbuf->b_ml.ml_લીટી_ગણતરી);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    જો (got_int)
	emsg(_(e_recovery_interrupted));
    નહીંતર જો (error)
    {
	++no_wait_return;
	સંદેશ(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_લીટીઓ_starting_with_questions));
	--no_wait_return;
	સંદેશ(_("See \":help E312\" for more information."));
	સંદેશ(">>>>>>>>>>>>>");
    }
    નહીંતર
    {
	જો (curbuf->b_changed)
	{
	    સંદેશ(_("Recovery completed. You should check if everything is OK."));
	    સંદેશ_puts(_("\n(You might want to write out this file under another name\n"));
	    સંદેશ_puts(_("and run diff with the original file to check for changes)"));
	}
	નહીંતર
	    સંદેશ(_("Recovery completed. Buffer contents equals file contents."));
	સંદેશ_puts(_("\nYou may want to delete the .swp file now."));
#જો defined(UNIX) || defined(MSWIN)
	જો (swapfile_process_running(b0p, fname_used))
	{
	    // Warn there could be an active Vim on the same file, the user may
	    // want to kill it.
	    સંદેશ_puts(_("\nNote: process STILL RUNNING: "));
	    સંદેશ_outnum(char_to_long(b0p->b0_pid));
	}
#endif
	સંદેશ_puts("\n\n");
	cmdline_row = સંદેશ_row;
    }
#ifdef FEAT_CRYPT
    જો (*buf->b_p_ચાવી != NUL && STRCMP(curbuf->b_p_ચાવી, buf->b_p_ચાવી) != 0)
    {
	સંદેશ_puts(_("Using crypt key from swap file for the text file.\n"));
	set_option_કિંમત_give_err((char_u *)"key", 0L, buf->b_p_ચાવી, OPT_LOCAL);
    }
#endif
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_used);
    recoverymode = FALSE;
    જો (mfp != ખાલી)
    {
	જો (hp != ખાલી)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    જો (buf != ખાલી)
    {
#ifdef FEAT_CRYPT
	જો (buf->b_p_ચાવી != curbuf->b_p_ચાવી)
	    free_string_option(buf->b_p_ચાવી);
	free_string_option(buf->b_p_cm);
#endif
	vim_free(buf->b_ml.ml_ઢગલો);
	vim_free(buf);
    }
    જો (serious_error && called_from_main)
	ml_close(curbuf, TRUE);
    નહીંતર
    {
	apply_autocmds(EVENT_BUFREADPOST, ખાલી, curbuf->b_fname, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ખાલી, curbuf->b_fname, FALSE, curbuf);
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
    પૂર્ણાંક
recover_names(
    char_u	*fname,		// base for swap file name
    પૂર્ણાંક		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list UNUSED, // when not NULL add file names to it
    પૂર્ણાંક		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_out)	// result when "nr" > 0
{
    પૂર્ણાંક		અંક_names;
    char_u	*(names[6]);
    char_u	*p;
    પૂર્ણાંક		અંક_files;
    પૂર્ણાંક		file_ગણતરી = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = ખાલી;
#ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#endif

    જો (fname != ખાલી)
    {
#ifdef HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	જો (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	નહીંતર
#endif
	    fname_res = fname;
    }

    જો (do_list)
    {
	// use msg() to start the scrolling properly
	સંદેશ(_("Swap files found:"));
	સંદેશ_putchar('\n');
    }

    /*
     * Do the loop for every directory in 'directory'.
     * First allocate some memory to put the directory name in.
     */
    dir_name.string = alloc(STRLEN(p_dir) + 1);
    dirp = p_dir;
    જ્યારે (dir_name.string != ખાલી && *dirp)
    {
	/*
	 * Isolate a directory name from *dirp and put it in dir_name (we know
	 * it is large enough, so use 31000 for length).
	 * Advance dirp to next directory name.
	 */
	dir_name.length = (size_t)copy_option_part(&dirp, dir_name.string, 31000, ",");

	જો (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    જો (fname == ખાલી)
	    {
#ifdef VMS
		names[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#નહીંતર
		names[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#endif
#જો defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		names[1] = vim_strnsave((char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"));
		names[2] = vim_strnsave((char_u *)".sw?", STRLEN_LITERAL(".sw?"));
		અંક_names = 3;
#નહીંતર
# ifdef VMS
		names[1] = vim_strnsave((char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"));
		અંક_names = 2;
# નહીંતર
		અંક_names = 1;
# endif
#endif
	    }
	    નહીંતર
		અંક_names = recov_file_names(names, fname_res, TRUE);
	}
	નહીંતર			    // check directory dir_name
	{
	    જો (fname == ખાલી)
	    {
		string_T    ret;

#ifdef VMS
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#નહીંતર
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#endif
#જો defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		names[1] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"), TRUE, &ret);
		names[2] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".sw?", STRLEN_LITERAL(".sw?"), TRUE, &ret);
		અંક_names = 3;
#નહીંતર
# ifdef VMS
		names[1] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"), TRUE, &ret);
		અંક_names = 2;
# નહીંતર
		અંક_names = 1;
# endif
#endif
	    }
	    નહીંતર
	    {
		char_u	*tail;

#જો defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.length;
		જો (after_pathsep(dir_name.string, p) && dir_name.length > 1 && p[-1] == p[-2])
		{
		    // Ends with '//', Use Full path for swap name
		    tail = make_percent_swname(dir_name.string, p, fname_res);
		}
		નહીંતર
#endif
		{
		    string_T	ret;

		    tail = gettail(fname_res);
		    tail = concat_fnames(dir_name.string, dir_name.length,
			tail, STRLEN(tail), TRUE, &ret);
		}
		જો (tail == ખાલી)
		    અંક_names = 0;
		નહીંતર
		{
		    અંક_names = recov_file_names(names, tail, FALSE);
		    vim_free(tail);
		}
	    }
	}

	// check for out-of-memory
	માટે (પૂર્ણાંક i = 0; i < અંક_names; ++i)
	{
	    જો (names[i] == ખાલી)
	    {
		માટે (i = 0; i < અંક_names; ++i)
		    vim_free(names[i]);
		અંક_names = 0;
	    }
	}
	જો (અંક_names == 0)
	    અંક_files = 0;
	નહીંતર જો (expand_wildcards(અંક_names, names, &અંક_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    અંક_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	જો (*dirp == NUL && file_ગણતરી + અંક_files == 0 && fname != ખાલી)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#જો defined(VMS)
			       (char_u *)"_swp", FALSE
#નહીંતર
			       (char_u *)".swp", TRUE
#endif
			      );
	    જો (swapname != ખાલી)
	    {
		જો (mch_stat((અક્ષર *)swapname, &st) != -1)    // It exists!
		{
		    files = ALLOC_ONE(char_u *);
		    જો (files != ખાલી)
		    {
			files[0] = swapname;
			swapname = ખાલી;
			અંક_files = 1;
		    }
		}
		vim_free(swapname);
	    }
	}

	/*
	 * Remove swapfile name of the current buffer, it must be ignored.
	 * But keep it for swapfilelist().
	 */
	જો (curbuf->b_ml.ml_mfp != ખાલી
			       && (p = curbuf->b_ml.ml_mfp->mf_fname) != ખાલી
			       && ret_list == ખાલી)
	{
	    માટે (પૂર્ણાંક i = 0; i < અંક_files; ++i)
		// Do not expand wildcards, on windows would try to expand
		// "%tmp%" in "%tmp%file".
		જો (fullpathcmp(p, files[i], TRUE, FALSE) & FPC_SAME)
		{
		    // Remove the name from files[i].  Move further entries
		    // down.  When the array becomes empty free it here, since
		    // FreeWild() won't be called below.
		    vim_free(files[i]);
		    જો (--અંક_files == 0)
			vim_free(files);
		    નહીંતર
			માટે ( ; i < અંક_files; ++i)
			    files[i] = files[i + 1];
		}
	}
	જો (nr > 0)
	{
	    file_ગણતરી += અંક_files;
	    જો (nr <= file_ગણતરી)
	    {
		*fname_out = vim_strsave(
				      files[nr - 1 + અંક_files - file_ગણતરી]);
		dirp = (char_u *)"";		    // stop searching
	    }
	}
	નહીંતર જો (do_list)
	{
	    જો (dir_name.string[0] == '.' && dir_name.string[1] == NUL)
	    {
		જો (fname == ખાલી)
		    સંદેશ_puts(_("   In current directory:\n"));
		નહીંતર
		    સંદેશ_puts(_("   Using specified name:\n"));
	    }
	    નહીંતર
	    {
		સંદેશ_puts(_("   In directory "));
		સંદેશ_home_replace(dir_name.string);
		સંદેશ_puts(":\n");
	    }

	    જો (અંક_files)
	    {
		માટે (પૂર્ણાંક i = 0; i < અંક_files; ++i)
		{
		    // print the swap file name
		    સંદેશ_outnum((long)++file_ગણતરી);
		    સંદેશ_puts(".    ");
		    સંદેશ_puts((અક્ષર *)gettail(files[i]));
		    સંદેશ_putchar('\n');
		    (શૂન્ય)swapfile_info(files[i]);
		}
	    }
	    નહીંતર
		સંદેશ_puts(_("      -- none --\n"));
	    out_flush();
	}
#ifdef FEAT_EVAL
	નહીંતર જો (ret_list != ખાલી)
	{
	    string_T	name;

	    માટે (પૂર્ણાંક i = 0; i < અંક_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.length,
		    files[i], STRLEN(files[i]), TRUE, &name);
		જો (name.string != ખાલી)
		{
		    list_append_string(ret_list, name.string, (પૂર્ણાંક)name.length);
		    vim_free(name.string);
		}
	    }
	}
#endif
	નહીંતર
	    file_ગણતરી += અંક_files;

	માટે (પૂર્ણાંક i = 0; i < અંક_names; ++i)
	    vim_free(names[i]);
	જો (અંક_files > 0)
	    FreeWild(અંક_files, files);
    }
    vim_free(dir_name.string);
    પાછા file_ગણતરી;
}

#જો defined(UNIX) || defined(MSWIN)
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
    string_T	d = {ખાલી, 0};
    string_T	fixed_fname;
    char_u	*p;

    fixed_fname.string = fix_fname(name != ખાલી ? name : (char_u *)"");
    જો (fixed_fname.string == ખાલી)
	પાછા ખાલી;

    માટે (p = fixed_fname.string; *p != NUL; MB_PTR_ADV(p))
	જો (vim_ispathsep(*p))
	    *p = '%';
    fixed_fname.length = (size_t)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_end[-1];
    *p = NUL;
    concat_fnames(dir, (size_t)(p - dir), fixed_fname.string, fixed_fname.length, TRUE, &d);
    vim_free(fixed_fname.string);

    પાછા d.string;
}
#endif

#જો (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
સ્થિર પૂર્ણાંક process_still_running;
#endif

#જો defined(FEAT_EVAL)
/*
 * Return information found in swapfile "fname" in dictionary "d".
 * This is used by the swapinfo() function.
 */
    શૂન્ય
get_b0_dict(char_u *fname, dict_T *d)
{
    પૂર્ણાંક fd;
    સંરચના block0 b0;

    જો ((fd = mch_open((અક્ષર *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
    {
	જો (read_eintr(fd, &b0, કદ(b0)) == કદ(b0))
	{
	    જો (ml_check_b0_id(&b0) == FAIL)
		dict_add_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    નહીંતર જો (b0_magic_wrong(&b0))
		dict_add_string_len(d, "error",
		    (char_u *)"Magic number mismatch", STRLEN_LITERAL("Magic number mismatch"));
	    નહીંતર
	    {
		// we have swap information
		dict_add_string_len(d, "version", b0.b0_version, 10);
		dict_add_string_len(d, "user", b0.b0_uname, B0_UNAME_SIZE);
		dict_add_string_len(d, "host", b0.b0_hname, B0_HNAME_SIZE);
		dict_add_string_len(d, "fname", b0.b0_fname, B0_FNAME_SIZE_ORG);

		dict_add_સંખ્યા(d, "pid", char_to_long(b0.b0_pid));
		dict_add_સંખ્યા(d, "mtime", char_to_long(b0.b0_mtime));
		dict_add_સંખ્યા(d, "dirty", b0.b0_dirty ? 1 : 0);
# ifdef CHECK_INODE
		dict_add_સંખ્યા(d, "inode", char_to_long(b0.b0_ino));
# endif
	    }
	}
	નહીંતર
	    dict_add_string_len(d, "error", (char_u *)"Cannot read file", STRLEN_LITERAL("Cannot read file"));
	close(fd);
    }
    નહીંતર
	dict_add_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#endif

/*
 * Give information about an existing swap file.
 * Returns timestamp (0 when unknown).
 */
    સ્થિર time_t
swapfile_info(char_u *fname)
{
    stat_T	    st;
    પૂર્ણાંક		    fd;
    સંરચના block0   b0;
#ifdef UNIX
    char_u	    uname[B0_UNAME_SIZE];
#endif

    // print the swap file date
    જો (mch_stat((અક્ષર *)fname, &st) != -1)
    {
#ifdef UNIX
	// print name of owner of the file
	જો (mch_get_uname(st.st_uid, uname, B0_UNAME_SIZE) == OK)
	{
	    સંદેશ_puts(_("          owned by: "));
	    સંદેશ_outtrans(uname);
	    સંદેશ_puts(_("   dated: "));
	}
	નહીંતર
#endif
	    સંદેશ_puts(_("             dated: "));
	સંદેશ_puts(get_ctime(st.st_mtime, TRUE));
    }
    નહીંતર
	st.st_mtime = 0;

    /*
     * print the original file name
     */
    fd = mch_open((અક્ષર *)fname, O_RDONLY | O_EXTRA, 0);
    જો (fd >= 0)
    {
	જો (read_eintr(fd, &b0, કદ(b0)) == કદ(b0))
	{
	    જો (STRNCMP(b0.b0_version, "VIM 3.0", 7) == 0)
	    {
		સંદેશ_puts(_("         [from Vim version 3.0]"));
	    }
	    નહીંતર જો (ml_check_b0_id(&b0) == FAIL)
	    {
		સંદેશ_puts(_("         [does not look like a Vim swap file]"));
	    }
	    નહીંતર
	    {
		સંદેશ_puts(_("         file name: "));
		જો (b0.b0_fname[0] == NUL)
		    સંદેશ_puts(_("[No Name]"));
		નહીંતર
		    સંદેશ_outtrans(b0.b0_fname);

		સંદેશ_puts(_("\n          modified: "));
		સંદેશ_puts(b0.b0_dirty ? _("YES") : _("no"));

		જો (*(b0.b0_uname) != NUL)
		{
		    સંદેશ_puts(_("\n         user name: "));
		    સંદેશ_outtrans(b0.b0_uname);
		}

		જો (*(b0.b0_hname) != NUL)
		{
		    જો (*(b0.b0_uname) != NUL)
			સંદેશ_puts(_("   host name: "));
		    નહીંતર
			સંદેશ_puts(_("\n         host name: "));
		    સંદેશ_outtrans(b0.b0_hname);
		}

		જો (char_to_long(b0.b0_pid) != 0L)
		{
		    સંદેશ_puts(_("\n        process ID: "));
		    સંદેશ_outnum(char_to_long(b0.b0_pid));
#જો defined(UNIX) || defined(MSWIN)
		    જો (swapfile_process_running(&b0, fname))
		    {
			સંદેશ_puts(_(" (STILL RUNNING)"));
# ifdef HAVE_PROCESS_STILL_RUNNING
			process_still_running = TRUE;
# endif
		    }
#endif
		}

		જો (b0_magic_wrong(&b0))
		{
#જો defined(MSWIN)
		    જો (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			સંદેશ_puts(_("\n         [not usable with this version of Vim]"));
		    નહીંતર
#endif
			સંદેશ_puts(_("\n         [not usable on this computer]"));
		}
	    }
	}
	નહીંતર
	    સંદેશ_puts(_("         [cannot be read]"));
	close(fd);
    }
    નહીંતર
	સંદેશ_puts(_("         [cannot be opened]"));
    સંદેશ_putchar('\n');

    પાછા st.st_mtime;
}

/*
 * Return TRUE if the swap file looks OK and there are no changes, thus it can
 * be safely deleted.
 */
    સ્થિર પૂર્ણાંક
swapfile_unchanged(char_u *fname)
{
    stat_T	    st;
    પૂર્ણાંક		    fd;
    સંરચના block0   b0;
    પૂર્ણાંક		    ret = TRUE;

    // must be able to stat the swap file
    જો (mch_stat((અક્ષર *)fname, &st) == -1)
	પાછા FALSE;

    // must be able to read the first block
    fd = mch_open((અક્ષર *)fname, O_RDONLY | O_EXTRA, 0);
    જો (fd < 0)
	પાછા FALSE;
    જો (read_eintr(fd, &b0, કદ(b0)) != કદ(b0))
    {
	close(fd);
	પાછા FALSE;
    }

    // the ID and magic number must be correct
    જો (ml_check_b0_id(&b0) == FAIL|| b0_magic_wrong(&b0))
	ret = FALSE;

    // must be unchanged
    જો (b0.b0_dirty)
	ret = FALSE;

#જો defined(UNIX) || defined(MSWIN)
    // Host name must be known and must equal the current host name, otherwise
    // comparing pid is meaningless.
    જો (*(b0.b0_hname) == NUL)
    {
	ret = FALSE;
    }
    નહીંતર
    {
	char_u	    hostname[B0_HNAME_SIZE];

	mch_get_host_name(hostname, B0_HNAME_SIZE);
	hostname[B0_HNAME_SIZE - 1] = NUL;
	b0.b0_hname[B0_HNAME_SIZE - 1] = NUL; // in case of corruption
	જો (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    જો (char_to_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#endif

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    close(fd);
    પાછા ret;
}

    સ્થિર પૂર્ણાંક
recov_file_names(char_u **names, char_u *path, પૂર્ણાંક prepend_dot)
{
    પૂર્ણાંક		અંક_names;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    પૂર્ણાંક		i;
#ifndef MSWIN
    bool    shortname = curbuf->b_shortname;

    curbuf->b_shortname = false;
#endif
    string_T	ret;

    અંક_names = 0;

    /*
     * May also add the file name with a dot prepended, for swap file in same
     * dir as original file.
     */
    જો (prepend_dot)
    {
	names[અંક_names] = modname(path, (char_u *)".sw?", TRUE);
	જો (names[અંક_names] == ખાલી)
	    જાઓ end;
	++અંક_names;
    }

    /*
     * Form the normal swap file name pattern by appending ".sw?".
     */
#ifdef VMS
    names[અંક_names] = concat_fnames(path, STRLEN(path),
	(char_u *)"_sw%", STRLEN_LITERAL("_sw%"), FALSE, &ret);
#નહીંતર
    names[અંક_names] = concat_fnames(path, STRLEN(path),
	(char_u *)".sw?", STRLEN_LITERAL(".sw?"), FALSE, &ret);
#endif
    જો (names[અંક_names] == ખાલી)
	જાઓ end;
    જો (અંક_names >= 1)	    // check if we have the same name twice
    {
	p = names[અંક_names - 1];
	i = (પૂર્ણાંક)STRLEN(names[અંક_names - 1]) - (પૂર્ણાંક)STRLEN(names[અંક_names]);
	જો (i > 0)
	    p += i;	    // file name has been expanded to full path

	જો (STRCMP(p, names[અંક_names]) != 0)
	    ++અંક_names;
	નહીંતર
	    vim_free(names[અંક_names]);
    }
    નહીંતર
	++અંક_names;

#ifndef MSWIN
    /*
     * Also try with 'shortname' set, in case the file is on a DOS filesystem.
     */
    curbuf->b_shortname = true;
# ifdef VMS
    names[અંક_names] = modname(path, (char_u *)"_sw%", FALSE);
# નહીંતર
    names[અંક_names] = modname(path, (char_u *)".sw?", FALSE);
# endif
    જો (names[અંક_names] == ખાલી)
	જાઓ end;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = names[અંક_names];
    i = STRLEN(names[અંક_names]) - STRLEN(names[અંક_names - 1]);
    જો (i > 0)
	p += i;		// file name has been expanded to full path
    જો (STRCMP(names[અંક_names - 1], p) == 0)
	vim_free(names[અંક_names]);
    નહીંતર
	++અંક_names;
#endif

end:
#ifndef MSWIN
    curbuf->b_shortname = shortname;
#endif

    પાછા અંક_names;
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
    શૂન્ય
ml_sync_all(પૂર્ણાંક check_file, પૂર્ણાંક check_char)
{
    buf_T		*buf;
    stat_T		st;

    FOR_ALL_BUFFERS(buf)
    {
	જો (buf->b_ml.ml_mfp == ખાલી
		|| buf->b_ml.ml_mfp->mf_fname == ખાલી
		|| buf->b_ml.ml_mfp->mf_fd < 0)
	    ચાલુ;			    // no file

#ifdef FEAT_CRYPT
	જો (crypt_may_close_swapfile(buf, buf->b_p_ચાવી,
						     crypt_get_method_nr(buf)))
	    ચાલુ;
#endif

	ml_flush_લીટી(buf);		    // flush buffered line
					    // flush locked block
	(શૂન્ય)ml_find_લીટી(buf, (linenr_T)0, ML_FLUSH);
	જો (bufIsChanged(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != ખાલી)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    જો (mch_stat((અક્ષર *)buf->b_ffname, &st) == -1
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
	જો (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (શૂન્ય)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (bufIsChanged(buf) ? MFS_FLUSH : 0));
	    જો (check_char && ui_char_avail())	// character available now
		તોડો;
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
    શૂન્ય
ml_preserve(buf_T *buf, પૂર્ણાંક message)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    પૂર્ણાંક		status;
    પૂર્ણાંક		got_int_save = got_int;

    જો (mfp == ખાલી || mfp->mf_fname == ખાલી)
    {
	જો (message)
	    emsg(_(e_cannot_preserve_there_is_no_swap_file));
	પાછા;
    }
#ifdef FEAT_CRYPT
    જો (crypt_may_close_swapfile(buf, buf->b_p_ચાવી, crypt_get_method_nr(buf)))
	પાછા;
#endif

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_લીટી(buf);				    // flush buffered line
    (શૂન્ય)ml_find_લીટી(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    status = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

    // stack is invalid after mf_sync(.., MFS_ALL)
    buf->b_ml.ml_ઢગલો_ટોચ = 0;

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
    જો (mf_need_trans(mfp) && !got_int)
    {
	lnum = 1;
	જ્યારે (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_લીટી_ગણતરી)
	{
	    hp = ml_find_લીટી(buf, lnum, ML_FIND);
	    જો (hp == ખાલી)
	    {
		status = FAIL;
		જાઓ theend;
	    }
	    CHECK(buf->b_ml.ml_locked_નીચલું != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_ઉપલું + 1;
	}
	(શૂન્ય)ml_find_લીટી(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	જો (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    status = FAIL;
	buf->b_ml.ml_ઢગલો_ટોચ = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    જો (message)
    {
	જો (status == OK)
	    સંદેશ(_("File preserved"));
	નહીંતર
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
    પાછા ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
    char_u *
ml_get_pos(pos_T *pos)
{
    પાછા (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->col);
}

/*
 * Return pointer to cursor line.
 */
    char_u *
ml_get_curline(શૂન્ય)
{
    પાછા ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE);
}

/*
 * Return pointer to cursor position.
 */
    char_u *
ml_get_cursor(શૂન્ય)
{
    પાછા (ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE) +
							curwin->w_cursor.col);
}

// return length (excluding the NUL) of the given line
    colnr_T
ml_get_len(linenr_T lnum)
{
    પાછા ml_get_buf_len(curbuf, lnum);
}

// return length (excluding the NUL) of the text after position "pos"
    colnr_T
ml_get_pos_len(pos_T *pos)
{
    પાછા ml_get_buf_len(curbuf, pos->lnum) - pos->col;
}

// return length (excluding the NUL) of the cursor line
    colnr_T
ml_get_curline_len(શૂન્ય)
{
    પાછા ml_get_buf_len(curbuf, curwin->w_cursor.lnum);
}

// return length (excluding the NUL) of the cursor position
    colnr_T
ml_get_cursor_len(શૂન્ય)
{
    પાછા ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.col;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*લીટી;

    જો (*(લીટી = ml_get_buf(buf, lnum, FALSE)) == NUL)
	પાછા 0;

    જો (buf->b_ml.ml_લીટી_textlen <= 0)
	buf->b_ml.ml_લીટી_textlen = (પૂર્ણાંક)STRLEN(લીટી) + 1;
    પાછા buf->b_ml.ml_લીટી_textlen - 1;
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
    પૂર્ણાંક		will_change)		// line will be changed
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    સ્થિર પૂર્ણાંક	recursive = 0;
    સ્થિર char_u questions[4];

    જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)	// invalid line number
    {
	જો (recursive == 0)
	{
	    // Avoid giving this message for a recursive call, may happen when
	    // the GUI redraws part of the text.
	    ++recursive;
	    siemsg(e_ml_get_invalid_lnum_nr, lnum);
	    --recursive;
	}
	ml_flush_લીટી(buf);
errorret:
	STRCPY(questions, "???");
	buf->b_ml.ml_લીટી_len = 4;
	buf->b_ml.ml_લીટી_textlen = buf->b_ml.ml_લીટી_len;
	buf->b_ml.ml_લીટી_lnum = lnum;
	પાછા questions;
    }
    જો (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    જો (buf->b_ml.ml_mfp == ખાલી)	// there are no lines
    {
	buf->b_ml.ml_લીટી_len = 1;
	buf->b_ml.ml_લીટી_textlen = buf->b_ml.ml_લીટી_len;
	પાછા (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    જો (buf->b_ml.ml_લીટી_lnum != lnum || mf_dont_release)
    {
	unsigned    start, end;
	પૂર્ણાંક	    idx;

	ml_flush_લીટી(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	જો ((hp = ml_find_લીટી(buf, lnum, ML_FIND)) == ખાલી)
	{
	    જો (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_find_લીટી_nr_in_buffer_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    જાઓ errorret;
	}

	dp = (DATA_BL *)(hp->bh_data);

	idx = lnum - buf->b_ml.ml_locked_નીચલું;
	start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	જો (idx == 0)
	    end = dp->db_txt_end;
	નહીંતર
	    end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);

	buf->b_ml.ml_લીટી_ptr = (char_u *)dp + start;
	buf->b_ml.ml_લીટી_len = end - start;
#જો defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	જો (buf->b_has_textprop
			 && (size_t)buf->b_ml.ml_લીટી_len > કદ(textprop_T))
	    buf->b_ml.ml_લીટી_textlen = 0;  // call STRLEN() later when needed
	નહીંતર
#endif
	    buf->b_ml.ml_લીટી_textlen = buf->b_ml.ml_લીટી_len;
	buf->b_ml.ml_લીટી_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_લીટી_DIRTY | ML_ALLOCATED);
    }
    જો (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ifdef FEAT_EVAL
	જો (ml_get_alloc_લીટીઓ && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_લીટી_DIRTY;
#endif
    }

#ifdef FEAT_EVAL
    જો (ml_get_alloc_લીટીઓ
		 && (buf->b_ml.ml_flags & (ML_લીટી_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_લીટી_len);

	// make sure the text is in allocated memory
	જો (p != ખાલી)
	{
	    memmove(p, buf->b_ml.ml_લીટી_ptr, buf->b_ml.ml_લીટી_len);
	    buf->b_ml.ml_લીટી_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    જો (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_લીટી_DIRTY;
	}
    }
#endif
    પાછા buf->b_ml.ml_લીટી_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    પૂર્ણાંક
ml_લીટી_alloced(શૂન્ય)
{
    પાછા (curbuf->b_ml.ml_flags & ML_લીટી_DIRTY);
}

#ifdef FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    સ્થિર શૂન્ય
add_text_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**લીટી,
	    પૂર્ણાંક		*len,
	    char_u	**tofree)
{
    પૂર્ણાંક		round;
    પૂર્ણાંક		new_prop_ગણતરી = 0;
    પૂર્ણાંક		ગણતરી;
    પૂર્ણાંક		n;
    char_u	*props;
    પૂર્ણાંક		new_len = 0;  // init for gcc
    char_u	*new_લીટી = ખાલી;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    માટે (round = 1; round <= 2; ++round)
    {
	જો (round == 2)
	{
	    uint16_t pc;

	    જો (new_prop_ગણતરી == 0)
		પાછા;  // nothing to do
	    new_len = *len + (પૂર્ણાંક)PROP_ગણતરી_SIZE
			     + new_prop_ગણતરી * (પૂર્ણાંક)કદ(textprop_T);
	    new_લીટી = alloc(new_len);
	    જો (new_લીટી == ખાલી)
		પાછા;
	    mch_memmove(new_લીટી, *લીટી, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_ગણતરી;
	    mch_memmove(new_લીટી + *len, &pc, PROP_ગણતરી_SIZE);
	    new_prop_ગણતરી = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	ગણતરી = get_text_props(buf, lnum, &props, FALSE);
	માટે (n = 0; n < ગણતરી; ++n)
	{
	    mch_memmove(&prop, props + n * કદ(textprop_T),
							   કદ(textprop_T));
	    જો (prop.tp_flags & TP_FLAG_CONT_NEXT)
	    {
		જો (round == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_PREV;
		    prop.tp_col = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_text_offset = 0;
		    mch_memmove(new_લીટી + *len + (પૂર્ણાંક)PROP_ગણતરી_SIZE
			    + new_prop_ગણતરી * કદ(textprop_T),
			    &prop, કદ(textprop_T));
		}
		++new_prop_ગણતરી;
	    }
	}
    }
    *લીટી = new_લીટી;
    *tofree = new_લીટી;
    *len = new_len;
}
#endif

    સ્થિર પૂર્ણાંક
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*લીટી_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    પૂર્ણાંક		flags)		// ML_APPEND_ flags
{
    char_u	*લીટી = લીટી_arg;
    colnr_T	len = len_arg;
    પૂર્ણાંક		i;
    પૂર્ણાંક		લીટી_ગણતરી;	// number of indexes in current block
    પૂર્ણાંક		offset;
    પૂર્ણાંક		from, to;
    long	space_needed;	// space needed for new line
    પૂર્ણાંક		page_size;
    પૂર્ણાંક		page_ગણતરી;
    પૂર્ણાંક		db_idx;		// index for lnum in data block
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
#ifdef FEAT_PROP_POPUP
    char_u	*tofree = ખાલી;
# ifdef FEAT_BYTEOFF
    colnr_T	text_len = 0;	// text len with NUL without text properties
# endif
#endif
    પૂર્ણાંક		ret = FAIL;

    જો (lnum > buf->b_ml.ml_લીટી_ગણતરી || buf->b_ml.ml_mfp == ખાલી)
	પાછા FAIL;  // lnum out of range

    જો (lowest_marked && lowest_marked > lnum)
	lowest_marked = lnum + 1;

    જો (len == 0)
    {
	len = (colnr_T)STRLEN(લીટી) + 1;	// space needed for the text
#જો defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	text_len = len;
#endif
    }
#જો defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    નહીંતર જો (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	text_len = (colnr_T)STRLEN(લીટી) + 1;
    નહીંતર
	text_len = len;
#endif

#ifdef FEAT_PROP_POPUP
    જો (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	add_text_props_for_append(buf, lnum, &લીટી, &len, &tofree);
#endif

    space_needed = len + INDEX_SIZE;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    page_size = mfp->mf_page_size;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    જો ((hp = ml_find_લીટી(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_ઉમેરો)) == ખાલી)
	જાઓ theend;

    buf->b_ml.ml_flags &= ~ML_EMPTY;

    જો (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    નહીંતર
	db_idx = lnum - buf->b_ml.ml_locked_નીચલું;
		// get line count before the insertion
    લીટી_ગણતરી = buf->b_ml.ml_locked_ઉપલું - buf->b_ml.ml_locked_નીચલું;

    dp = (DATA_BL *)(hp->bh_data);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    જો ((long)dp->db_free < space_needed && db_idx == લીટી_ગણતરી - 1
					    && lnum < buf->b_ml.ml_લીટી_ગણતરી)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_ઉપલું);
	જો ((hp = ml_find_લીટી(buf, lnum + 1, ML_ઉમેરો)) == ખાલી)
	    જાઓ theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	લીટી_ગણતરી = buf->b_ml.ml_locked_ઉપલું - buf->b_ml.ml_locked_નીચલું;
	CHECK(buf->b_ml.ml_locked_નીચલું != lnum + 1, "locked_low != lnum + 1");

	dp = (DATA_BL *)(hp->bh_data);
    }

    ++buf->b_ml.ml_લીટી_ગણતરી;

    જો ((long)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_start -= len;
	dp->db_free -= space_needed;
	++(dp->db_લીટી_ગણતરી);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	જો (લીટી_ગણતરી > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    જો (db_idx < 0)
		offset = dp->db_txt_end;
	    નહીંતર
		offset = ((dp->db_index[db_idx]) & DB_INDEX_MASK);
	    mch_memmove((અક્ષર *)dp + dp->db_txt_start,
					  (અક્ષર *)dp + dp->db_txt_start + len,
				 (size_t)(offset - (dp->db_txt_start + len)));
	    માટે (i = લીટી_ગણતરી - 1; i > db_idx; --i)
		dp->db_index[i + 1] = dp->db_index[i] - len;
	    dp->db_index[db_idx + 1] = offset - len;
	}
	નહીંતર
	    // add line at the end (which is the start of the text)
	    dp->db_index[db_idx + 1] = dp->db_txt_start;

	/*
	 * copy the text into the block
	 */
	mch_memmove((અક્ષર *)dp + dp->db_index[db_idx + 1], લીટી, (size_t)len);
	જો (flags & ML_APPEND_MARK)
	    dp->db_index[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	જો (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    નહીંતર	    // not enough space in data block
    {
	long	    લીટી_ગણતરી_ડાબું, લીટી_ગણતરી_જમણું;
	પૂર્ણાંક	    page_ગણતરી_ડાબું, page_ગણતરી_જમણું;
	bhdr_T	    *hp_ડાબું;
	bhdr_T	    *hp_જમણું;
	bhdr_T	    *hp_new;
	પૂર્ણાંક	    લીટીઓ_moved;
	પૂર્ણાંક	    data_moved = 0;	    // init to shut up gcc
	પૂર્ણાંક	    total_moved = 0;	    // init to shut up gcc
	DATA_BL	    *dp_જમણું, *dp_ડાબું;
	પૂર્ણાંક	    ઢગલો_idx;
	પૂર્ણાંક	    in_ડાબું;
	પૂર્ણાંક	    lineadd;
	blocknr_T   bnum_ડાબું, bnum_જમણું;
	linenr_T    lnum_ડાબું, lnum_જમણું;
	પૂર્ણાંક	    pb_idx;
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
	જો (db_idx < 0)		// left block is new, right block is existing
	{
	    લીટીઓ_moved = 0;
	    in_ડાબું = TRUE;
	    // space_needed does not change
	}
	નહીંતર			// left block is existing, right block is new
	{
	    લીટીઓ_moved = લીટી_ગણતરી - db_idx - 1;
	    જો (લીટીઓ_moved == 0)
		in_ડાબું = FALSE;	// put new line in right block
					// space_needed does not change
	    નહીંતર
	    {
		data_moved = ((dp->db_index[db_idx]) & DB_INDEX_MASK) -
							    dp->db_txt_start;
		total_moved = data_moved + લીટીઓ_moved * INDEX_SIZE;
		જો ((long)dp->db_free + total_moved >= space_needed)
		{
		    in_ડાબું = TRUE;	// put new line in left block
		    space_needed = total_moved;
		}
		નહીંતર
		{
		    in_ડાબું = FALSE;	    // put new line in right block
		    space_needed += total_moved;
		}
	    }
	}

	page_ગણતરી = ((space_needed + HEADER_SIZE) + page_size - 1) / page_size;
	જો ((hp_new = ml_new_data(mfp, flags & ML_APPEND_NEW, page_ગણતરી))
								       == ખાલી)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_ઉપલું);
	    જાઓ theend;
	}
	જો (db_idx < 0)		// left block is new
	{
	    hp_ડાબું = hp_new;
	    hp_જમણું = hp;
	    લીટી_ગણતરી_ડાબું = 0;
	    લીટી_ગણતરી_જમણું = લીટી_ગણતરી;
	}
	નહીંતર			// right block is new
	{
	    hp_ડાબું = hp;
	    hp_જમણું = hp_new;
	    લીટી_ગણતરી_ડાબું = લીટી_ગણતરી;
	    લીટી_ગણતરી_જમણું = 0;
	}
	dp_જમણું = (DATA_BL *)(hp_જમણું->bh_data);
	dp_ડાબું = (DATA_BL *)(hp_ડાબું->bh_data);
	bnum_ડાબું = hp_ડાબું->bh_bnum;
	bnum_જમણું = hp_જમણું->bh_bnum;
	page_ગણતરી_ડાબું = hp_ડાબું->bh_page_ગણતરી;
	page_ગણતરી_જમણું = hp_જમણું->bh_page_ગણતરી;

	/*
	 * May move the new line into the right/new block.
	 */
	જો (!in_ડાબું)
	{
	    dp_જમણું->db_txt_start -= len;
	    dp_જમણું->db_free -= len + INDEX_SIZE;
	    dp_જમણું->db_index[0] = dp_જમણું->db_txt_start;
	    જો (flags & ML_APPEND_MARK)
		dp_જમણું->db_index[0] |= DB_MARKED;

	    mch_memmove((અક્ષર *)dp_જમણું + dp_જમણું->db_txt_start,
							   લીટી, (size_t)len);
	    ++લીટી_ગણતરી_જમણું;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	જો (લીટીઓ_moved)
	{
	    dp_જમણું->db_txt_start -= data_moved;
	    dp_જમણું->db_free -= total_moved;
	    mch_memmove((અક્ષર *)dp_જમણું + dp_જમણું->db_txt_start,
			(અક્ષર *)dp_ડાબું + dp_ડાબું->db_txt_start,
			(size_t)data_moved);
	    offset = dp_જમણું->db_txt_start - dp_ડાબું->db_txt_start;
	    dp_ડાબું->db_txt_start += data_moved;
	    dp_ડાબું->db_free += total_moved;

	    /*
	     * update indexes in the new block
	     */
	    માટે (to = લીટી_ગણતરી_જમણું, from = db_idx + 1;
					 from < લીટી_ગણતરી_ડાબું; ++from, ++to)
		dp_જમણું->db_index[to] = dp->db_index[from] + offset;
	    લીટી_ગણતરી_જમણું += લીટીઓ_moved;
	    લીટી_ગણતરી_ડાબું -= લીટીઓ_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	જો (in_ડાબું)
	{
	    dp_ડાબું->db_txt_start -= len;
	    dp_ડાબું->db_free -= len + INDEX_SIZE;
	    dp_ડાબું->db_index[લીટી_ગણતરી_ડાબું] = dp_ડાબું->db_txt_start;
	    જો (flags & ML_APPEND_MARK)
		dp_ડાબું->db_index[લીટી_ગણતરી_ડાબું] |= DB_MARKED;
	    mch_memmove((અક્ષર *)dp_ડાબું + dp_ડાબું->db_txt_start,
							   લીટી, (size_t)len);
	    ++લીટી_ગણતરી_ડાબું;
	}

	જો (db_idx < 0)		// left block is new
	{
	    lnum_ડાબું = lnum + 1;
	    lnum_જમણું = 0;
	}
	નહીંતર			// right block is new
	{
	    lnum_ડાબું = 0;
	    જો (in_ડાબું)
		lnum_જમણું = lnum + 2;
	    નહીંતર
		lnum_જમણું = lnum + 1;
	}
	dp_ડાબું->db_લીટી_ગણતરી = લીટી_ગણતરી_ડાબું;
	dp_જમણું->db_લીટી_ગણતરી = લીટી_ગણતરી_જમણું;

	/*
	 * release the two data blocks
	 * The new one (hp_new) already has a correct blocknumber.
	 * The old one (hp, in ml_locked) gets a positive blocknumber if
	 * we changed it and we are not editing a new file.
	 */
	જો (લીટીઓ_moved || in_ડાબું)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	જો (!(flags & ML_APPEND_NEW) && db_idx >= 0 && in_ડાબું)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_find_લીટી(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	માટે (ઢગલો_idx = buf->b_ml.ml_ઢગલો_ટોચ - 1; ઢગલો_idx >= 0;
								  --ઢગલો_idx)
	{
	    ip = &(buf->b_ml.ml_ઢગલો[ઢગલો_idx]);
	    pb_idx = ip->ip_index;
	    જો ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ખાલી)
		જાઓ theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    જો (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		જાઓ theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    જો (pp->pb_ગણતરી < pp->pb_ગણતરી_max)
	    {
		જો (pb_idx + 1 < (પૂર્ણાંક)pp->pb_ગણતરી)
		    mch_memmove(&pp->pb_pointer[pb_idx + 2],
				&pp->pb_pointer[pb_idx + 1],
			(size_t)(pp->pb_ગણતરી - pb_idx - 1) * કદ(PTR_EN));
		++pp->pb_ગણતરી;
		pp->pb_pointer[pb_idx].pe_લીટી_ગણતરી = લીટી_ગણતરી_ડાબું;
		pp->pb_pointer[pb_idx].pe_bnum = bnum_ડાબું;
		pp->pb_pointer[pb_idx].pe_page_ગણતરી = page_ગણતરી_ડાબું;
		pp->pb_pointer[pb_idx + 1].pe_લીટી_ગણતરી = લીટી_ગણતરી_જમણું;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_જમણું;
		pp->pb_pointer[pb_idx + 1].pe_page_ગણતરી = page_ગણતરી_જમણું;

		જો (lnum_ડાબું != 0)
		    pp->pb_pointer[pb_idx].pe_old_lnum = lnum_ડાબું;
		જો (lnum_જમણું != 0)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_જમણું;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_ઢગલો_ટોચ = ઢગલો_idx + 1;	    // truncate stack

		જો (lineadd)
		{
		    --(buf->b_ml.ml_ઢગલો_ટોચ);
		    // fix line count for rest of blocks in the stack
		    ml_lineadd(buf, lineadd);
							// fix stack itself
		    buf->b_ml.ml_ઢગલો[buf->b_ml.ml_ઢગલો_ટોચ].ip_ઉપલું +=
								      lineadd;
		    ++(buf->b_ml.ml_ઢગલો_ટોચ);
		}

		/*
		 * We are finished, break the loop here.
		 */
		તોડો;
	    }
	    // pointer block full
	    /*
	     * split the pointer block
	     * allocate a new pointer block
	     * move some of the pointer into the new block
	     * prepare for updating the parent block
	     */
	    માટે (;;)	// do this twice when splitting block 1
	    {
		hp_new = ml_new_ptr(mfp);
		જો (hp_new == ખાલી)	    // TODO: try to fix tree
		    જાઓ theend;
		pp_new = (PTR_BL *)(hp_new->bh_data);

		જો (hp->bh_bnum != 1)
		    તોડો;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (size_t)page_size);
		pp->pb_ગણતરી = 1;
		pp->pb_pointer[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_pointer[0].pe_લીટી_ગણતરી = buf->b_ml.ml_લીટી_ગણતરી;
		pp->pb_pointer[0].pe_old_lnum = 1;
		pp->pb_pointer[0].pe_page_ગણતરી = 1;
		mf_put(mfp, hp, TRUE, FALSE);   // release block 1
		hp = hp_new;		// new block is to be split
		pp = pp_new;
		CHECK(ઢગલો_idx != 0, _("stack_idx should be 0"));
		ip->ip_index = 0;
		++ઢગલો_idx;	// do block 1 again later
	    }
	    /*
	     * move the pointers after the current one to the new block
	     * If there are none, the new entry will be in the new block.
	     */
	    total_moved = pp->pb_ગણતરી - pb_idx - 1;
	    જો (total_moved)
	    {
		mch_memmove(&pp_new->pb_pointer[0],
				&pp->pb_pointer[pb_idx + 1],
				(size_t)(total_moved) * કદ(PTR_EN));
		pp_new->pb_ગણતરી = total_moved;
		pp->pb_ગણતરી -= total_moved - 1;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_જમણું;
		pp->pb_pointer[pb_idx + 1].pe_લીટી_ગણતરી = લીટી_ગણતરી_જમણું;
		pp->pb_pointer[pb_idx + 1].pe_page_ગણતરી = page_ગણતરી_જમણું;
		જો (lnum_જમણું)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_જમણું;
	    }
	    નહીંતર
	    {
		pp_new->pb_ગણતરી = 1;
		pp_new->pb_pointer[0].pe_bnum = bnum_જમણું;
		pp_new->pb_pointer[0].pe_લીટી_ગણતરી = લીટી_ગણતરી_જમણું;
		pp_new->pb_pointer[0].pe_page_ગણતરી = page_ગણતરી_જમણું;
		pp_new->pb_pointer[0].pe_old_lnum = lnum_જમણું;
	    }
	    pp->pb_pointer[pb_idx].pe_bnum = bnum_ડાબું;
	    pp->pb_pointer[pb_idx].pe_લીટી_ગણતરી = લીટી_ગણતરી_ડાબું;
	    pp->pb_pointer[pb_idx].pe_page_ગણતરી = page_ગણતરી_ડાબું;
	    જો (lnum_ડાબું)
		pp->pb_pointer[pb_idx].pe_old_lnum = lnum_ડાબું;
	    lnum_ડાબું = 0;
	    lnum_જમણું = 0;

	    /*
	     * recompute line counts
	     */
	    લીટી_ગણતરી_જમણું = 0;
	    માટે (i = 0; i < (પૂર્ણાંક)pp_new->pb_ગણતરી; ++i)
		લીટી_ગણતરી_જમણું += pp_new->pb_pointer[i].pe_લીટી_ગણતરી;
	    લીટી_ગણતરી_ડાબું = 0;
	    માટે (i = 0; i < (પૂર્ણાંક)pp->pb_ગણતરી; ++i)
		લીટી_ગણતરી_ડાબું += pp->pb_pointer[i].pe_લીટી_ગણતરી;

	    bnum_ડાબું = hp->bh_bnum;
	    bnum_જમણું = hp_new->bh_bnum;
	    page_ગણતરી_ડાબું = 1;
	    page_ગણતરી_જમણું = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	જો (ઢગલો_idx < 0)
	{
	    iemsg(e_updated_too_many_blocks);
	    buf->b_ml.ml_ઢગલો_ટોચ = 0;	// invalidate stack
	}
    }

#ifdef FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (long)text_len
# નહીંતર
	    (long)len
# endif
	    , ML_CHNK_ADDLINE);
#endif

#ifdef FEAT_NETBEANS_INTG
    જો (!(flags & ML_APPEND_NEW) && netbeans_active())
    {
	પૂર્ણાંક લીટી_len = (પૂર્ણાંક)STRLEN(લીટી);
	જો (લીટી_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, લીટી, લીટી_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)લીટી_len, (char_u *)"\n", 1);
    }
#endif
#ifdef FEAT_JOB_CHANNEL
    જો (!(flags & ML_APPEND_NEW) && buf->b_write_to_channel)
	channel_write_new_લીટીઓ(buf);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    vim_free(tofree);
#endif
    પાછા ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    સ્થિર પૂર્ણાંક
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*લીટી,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    પૂર્ણાંક		flags)		// ML_APPEND_ flags
{
    જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)
	પાછા FAIL;  // lnum out of range

    જો (buf->b_ml.ml_લીટી_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_લીટી(buf);

#ifdef FEAT_EVAL
    જો (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	જો (buf->b_ml.ml_લીટી_lnum != 0)
	    ml_flush_લીટી(buf);
    }
#endif

    પાછા ml_append_int(buf, lnum, લીટી, len, flags);
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
    પૂર્ણાંક
ml_append(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*લીટી,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    પૂર્ણાંક		newfile)	// flag, see above
{
    પાછા ml_append_flags(lnum, લીટી, len, newfile ? ML_APPEND_NEW : 0);
}

    પૂર્ણાંક
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*લીટી,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    પૂર્ણાંક		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    જો (curbuf->b_ml.ml_mfp == ખાલી && open_buffer(FALSE, ખાલી, 0) == FAIL)
	પાછા FAIL;
    પાછા ml_append_flush(curbuf, lnum, લીટી, len, flags);
}


#જો defined(FEAT_SPELL) || defined(FEAT_QUICKFIX) || defined(FEAT_PROP_POPUP)

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
    પૂર્ણાંક
ml_append_buf(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*લીટી,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    પૂર્ણાંક		newfile)	// flag, see above
{
    જો (buf->b_ml.ml_mfp == ખાલી)
	પાછા FAIL;
    પાછા ml_append_flush(buf, lnum, લીટી, len, newfile ? ML_APPEND_NEW : 0);
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
    પૂર્ણાંક
ml_replace(linenr_T lnum, char_u *લીટી, પૂર્ણાંક copy)
{
    colnr_T len = -1;

    જો (લીટી != ખાલી)
	len = (colnr_T)STRLEN(લીટી);
    પાછા ml_replace_len(lnum, લીટી, len, FALSE, copy);
}

/*
 * Replace a line for the current buffer.  Like ml_replace() with:
 * "len_arg" is the length of the text, excluding NUL.
 * If "has_props" is TRUE then "line_arg" includes the text properties and
 * "len_arg" includes the NUL of the text and text properties.
 * When "copy" is TRUE copy the text into allocated memory, otherwise
 * "line_arg" must be allocated and will be consumed here.
 */
    પૂર્ણાંક
ml_replace_len(
	linenr_T    lnum,
	char_u	    *લીટી_arg,
	colnr_T	    len_arg,
	પૂર્ણાંક	    has_props,
	પૂર્ણાંક	    copy)
{
    char_u *લીટી = લીટી_arg;
    colnr_T len = len_arg;

    જો (લીટી == ખાલી)		// just checking...
	પાછા FAIL;

    // When starting up, we might still need to create the memfile
    જો (curbuf->b_ml.ml_mfp == ખાલી && open_buffer(FALSE, ખાલી, 0) == FAIL)
	પાછા FAIL;

    જો (!has_props)
	++len;  // include the NUL after the text
    જો (copy)
    {
	// copy the line to allocated memory
#ifdef FEAT_PROP_POPUP
	જો (has_props)
	    લીટી = vim_memsave(લીટી, len);
	નહીંતર
#endif
	    લીટી = vim_strnsave(લીટી, len - 1);
	જો (લીટી == ખાલી)
	    પાછા FAIL;
    }

#ifdef FEAT_NETBEANS_INTG
    જો (netbeans_active())
    {
	netbeans_removed(curbuf, lnum, 0, (long)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, લીટી, (પૂર્ણાંક)STRLEN(લીટી));
    }
#endif
    જો (curbuf->b_ml.ml_લીટી_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_લીટી(curbuf);

#ifdef FEAT_PROP_POPUP
	જો (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#endif
    }

#ifdef FEAT_PROP_POPUP
    જો (curbuf->b_has_textprop && !has_props)
    {
	size_t	oldtextlen = STRLEN(curbuf->b_ml.ml_લીટી_ptr) + 1;

	જો (oldtextlen < (size_t)curbuf->b_ml.ml_લીટી_len)
	{
	    char_u *newline;
	    size_t textproplen = curbuf->b_ml.ml_લીટી_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    જો (newline != ખાલી)
	    {
		mch_memmove(newline, લીટી, len);
		mch_memmove(newline + len, curbuf->b_ml.ml_લીટી_ptr
						    + oldtextlen, textproplen);
		vim_free(લીટી);
		લીટી = newline;
		len += (colnr_T)textproplen;
	    }
	}
    }
#endif

    જો (curbuf->b_ml.ml_flags & (ML_લીટી_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_લીટી_ptr);	// free allocated line

    curbuf->b_ml.ml_લીટી_ptr = લીટી;
    curbuf->b_ml.ml_લીટી_len = len;
    curbuf->b_ml.ml_લીટી_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_લીટી_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_લીટી_DIRTY) & ~ML_EMPTY;

    પાછા OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * Adjust text properties in line "lnum" for a deleted line.
 * When "above" is true this is the line above the deleted line, otherwise this
 * is the line below the deleted line.
 * "del_props[del_props_len]" are the properties of the deleted line.
 */
    સ્થિર શૂન્ય
adjust_text_props_for_delete(
	buf_T	    *buf,
	linenr_T    lnum,
	char_u	    *del_props,
	પૂર્ણાંક	    del_props_len,
	પૂર્ણાંક	    above)
{
    પૂર્ણાંક		did_get_લીટી = FALSE;
    પૂર્ણાંક		done_del;
    પૂર્ણાંક		done_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    DATA_BL	*dp;
    પૂર્ણાંક		idx;
    પૂર્ણાંક		લીટી_start;
    long	લીટી_size;
    પૂર્ણાંક		this_props_len = 0;
    char_u	*text;
    size_t	textlen;
    પૂર્ણાંક		found;

    માટે (done_del = 0; done_del < del_props_len; done_del += કદ(textprop_T))
    {
	mch_memmove(&prop_del, del_props + done_del, કદ(textprop_T));
	જો ((above && (prop_del.tp_flags & TP_FLAG_CONT_PREV)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_NEXT))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_NEXT)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_PREV)))
	{
	    જો (!did_get_લીટી)
	    {
		did_get_લીટી = TRUE;
		જો ((hp = ml_find_લીટી(buf, lnum, ML_FIND)) == ખાલી)
		    પાછા;

		dp = (DATA_BL *)(hp->bh_data);
		idx = lnum - buf->b_ml.ml_locked_નીચલું;
		લીટી_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
		જો (idx == 0)		// first line in block, text at the end
		    લીટી_size = dp->db_txt_end - લીટી_start;
		નહીંતર
		    લીટી_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK)
								  - લીટી_start;
		text = (char_u *)dp + લીટી_start;
		textlen = STRLEN(text) + 1;
		જો ((long)textlen >= લીટી_size)
		{
		    // No properties on this line.
		    જો (above)
			internal_error("no text property above deleted line");
		    નહીંતર
			internal_error("no text property below deleted line");
		    પાછા;
		}
		જો ((long)textlen + (long)PROP_ગણતરી_SIZE > લીટી_size)
		{
		    internal_error("text property data too short");
		    પાછા;
		}

		uint16_t pc;

		mch_memmove(&pc, text + textlen, PROP_ગણતરી_SIZE);
		જો (!text_prop_ગણતરી_valid(pc, (size_t)(લીટી_size - (long)textlen)))
		{
		    internal_error("text property count too large");
		    પાછા;
		}
		this_props_len = pc * (પૂર્ણાંક)કદ(textprop_T);
	    }

	    found = FALSE;
	    {
		char_u *props_start = text + textlen + PROP_ગણતરી_SIZE;

		માટે (done_this = 0; done_this < this_props_len;
					       done_this += કદ(textprop_T))
		{
		    પૂર્ણાંક		flag = above ? TP_FLAG_CONT_NEXT
							   : TP_FLAG_CONT_PREV;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_start + done_this,
							   કદ(textprop_T));
		    જો ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			found = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_start + done_this, &prop_this,
							   કદ(textprop_T));
			તોડો;
		    }
		}
	    }
	    જો (!found)
	    {
		જો (above)
		    internal_error("text property above deleted line not found");
		નહીંતર
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
    સ્થિર પૂર્ણાંક
ml_delete_int(buf_T *buf, linenr_T lnum, પૂર્ણાંક flags)
{
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    પૂર્ણાંક		ગણતરી;	    // number of entries in block
    પૂર્ણાંક		idx;
    પૂર્ણાંક		ઢગલો_idx;
    પૂર્ણાંક		text_start;
    પૂર્ણાંક		લીટી_start;
    long	લીટી_size;
    પૂર્ણાંક		i;
    પૂર્ણાંક		ret = FAIL;
#ifdef FEAT_PROP_POPUP
    char_u	*textprop_save = ખાલી;
    long	textprop_len = 0;
#endif

    જો (lowest_marked && lowest_marked > lnum)
	lowest_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    જો (buf->b_ml.ml_લીટી_ગણતરી == 1)	    // file becomes empty
    {
	જો ((flags & ML_DEL_MESSAGE)
#ifdef FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_લીટીઓ
#endif
	   )
	    set_keep_સંદેશ((char_u *)_(no_લીટીઓ_સંદેશ), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_EMPTY;

	પાછા i;
    }

/*
 * Find the data block containing the line.
 * This also fills the stack with the blocks from the root to the data block.
 * This also releases any locked block..
 */
    mfp = buf->b_ml.ml_mfp;
    જો (mfp == ખાલી)
	પાછા FAIL;

    જો ((hp = ml_find_લીટી(buf, lnum, ML_DELETE)) == ખાલી)
	પાછા FAIL;

    dp = (DATA_BL *)(hp->bh_data);
    // compute line count before the delete
    ગણતરી = (long)(buf->b_ml.ml_locked_ઉપલું)
					- (long)(buf->b_ml.ml_locked_નીચલું) + 2;
    idx = lnum - buf->b_ml.ml_locked_નીચલું;

    --buf->b_ml.ml_લીટી_ગણતરી;

    લીટી_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
    જો (idx == 0)		// first line in block, text at the end
	લીટી_size = dp->db_txt_end - લીટી_start;
    નહીંતર
	લીટી_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK) - લીટી_start;

#ifdef FEAT_NETBEANS_INTG
    જો (netbeans_active())
	netbeans_removed(buf, lnum, 0, લીટી_size);
#endif
#ifdef FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    જો (buf->b_has_textprop)
    {
	size_t	textlen = STRLEN((char_u *)dp + લીટી_start) + 1;

	textprop_len = લીટી_size - (long)textlen;
	જો (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + લીટી_start + textlen,
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
    જો (ગણતરી == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = ખાલી;

	માટે (ઢગલો_idx = buf->b_ml.ml_ઢગલો_ટોચ - 1; ઢગલો_idx >= 0;
								  --ઢગલો_idx)
	{
	    buf->b_ml.ml_ઢગલો_ટોચ = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_ઢગલો[ઢગલો_idx]);
	    idx = ip->ip_index;
	    જો ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ખાલી)
		જાઓ theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    જો (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		જાઓ theend;
	    }
	    ગણતરી = --(pp->pb_ગણતરી);
	    જો (ગણતરી == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    નહીંતર
	    {
		જો (ગણતરી != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_pointer[idx], &pp->pb_pointer[idx + 1],
				      (size_t)(ગણતરી - idx) * કદ(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_ઢગલો_ટોચ = ઢગલો_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		જો (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_ઢગલો[buf->b_ml.ml_ઢગલો_ટોચ].ip_ઉપલું +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_ઢગલો_ટોચ);

		તોડો;
	    }
	}
	CHECK(ઢગલો_idx < 0, _("deleted block 1?"));
    }
    નહીંતર
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	text_start = dp->db_txt_start;
	mch_memmove((અક્ષર *)dp + text_start + લીટી_size,
		  (અક્ષર *)dp + text_start, (size_t)(લીટી_start - text_start));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	માટે (i = idx; i < ગણતરી - 1; ++i)
	    dp->db_index[i] = dp->db_index[i + 1] + લીટી_size;

	dp->db_free += લીટી_size + INDEX_SIZE;
	dp->db_txt_start += લીટી_size;
	--(dp->db_લીટી_ગણતરી);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#ifdef FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, લીટી_size
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    જો (textprop_save != ખાલી)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_data;
	પૂર્ણાંક	    props_bytes;

	mch_memmove(&pc, textprop_save, PROP_ગણતરી_SIZE);
	props_data = textprop_save + PROP_ગણતરી_SIZE;
	props_bytes = pc * (પૂર્ણાંક)કદ(textprop_T);
	જો (!text_prop_ગણતરી_valid(pc, (size_t)textprop_len))
	    props_bytes = 0;

	// Adjust text properties in the line above and below.
	જો (lnum > 1)
	    adjust_text_props_for_delete(buf, lnum - 1,
					     props_data, props_bytes, TRUE);
	જો (lnum <= buf->b_ml.ml_લીટી_ગણતરી)
	    adjust_text_props_for_delete(buf, lnum,
					    props_data, props_bytes, FALSE);
    }
    vim_free(textprop_save);
#endif
    પાછા ret;
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
    પૂર્ણાંક
ml_delete(linenr_T lnum)
{
    પાછા ml_delete_flags(lnum, 0);
}

/*
 * Like ml_delete() but using flags (see ml_delete_int()).
 */
    પૂર્ણાંક
ml_delete_flags(linenr_T lnum, પૂર્ણાંક flags)
{
    ml_flush_લીટી(curbuf);
    જો (lnum < 1 || lnum > curbuf->b_ml.ml_લીટી_ગણતરી)
	પાછા FAIL;

#ifdef FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#endif

    પાછા ml_delete_int(curbuf, lnum, flags);
}

/*
 * set the DB_MARKED flag for line 'lnum'
 */
    શૂન્ય
ml_setmarked(linenr_T lnum)
{
    bhdr_T    *hp;
    DATA_BL *dp;
				    // invalid line number
    જો (lnum < 1 || lnum > curbuf->b_ml.ml_લીટી_ગણતરી
					       || curbuf->b_ml.ml_mfp == ખાલી)
	પાછા;			    // give error message?

    જો (lowest_marked == 0 || lowest_marked > lnum)
	lowest_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    જો ((hp = ml_find_લીટી(curbuf, lnum, ML_FIND)) == ખાલી)
	પાછા;		    // give error message?

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[lnum - curbuf->b_ml.ml_locked_નીચલું] |= DB_MARKED;
    curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
}

/*
 * find the first line with its DB_MARKED flag set
 */
    linenr_T
ml_firstmarked(શૂન્ય)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    પૂર્ણાંક		i;

    જો (curbuf->b_ml.ml_mfp == ખાલી)
	પાછા (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    માટે (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_લીટી_ગણતરી; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	જો ((hp = ml_find_લીટી(curbuf, lnum, ML_FIND)) == ખાલી)
	    પાછા (linenr_T)0;		    // give error message?

	dp = (DATA_BL *)(hp->bh_data);

	માટે (i = lnum - curbuf->b_ml.ml_locked_નીચલું;
			    lnum <= curbuf->b_ml.ml_locked_ઉપલું; ++i, ++lnum)
	    જો ((dp->db_index[i]) & DB_MARKED)
	    {
		(dp->db_index[i]) &= DB_INDEX_MASK;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		lowest_marked = lnum + 1;
		પાછા lnum;
	    }
    }

    પાછા (linenr_T) 0;
}

/*
 * clear all DB_MARKED flags
 */
    શૂન્ય
ml_clearmarked(શૂન્ય)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    પૂર્ણાંક		i;

    જો (curbuf->b_ml.ml_mfp == ખાલી)	    // nothing to do
	પાછા;

    /*
     * The search starts with line lowest_marked.
     */
    માટે (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_લીટી_ગણતરી; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	જો ((hp = ml_find_લીટી(curbuf, lnum, ML_FIND)) == ખાલી)
	    પાછા;		// give error message?

	dp = (DATA_BL *)(hp->bh_data);

	માટે (i = lnum - curbuf->b_ml.ml_locked_નીચલું;
			    lnum <= curbuf->b_ml.ml_locked_ઉપલું; ++i, ++lnum)
	    જો ((dp->db_index[i]) & DB_MARKED)
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
    સ્થિર શૂન્ય
ml_flush_લીટી(buf_T *buf)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    char_u	*new_લીટી;
    char_u	*old_લીટી;
    colnr_T	new_len;
    પૂર્ણાંક		old_len;
    પૂર્ણાંક		extra;
    પૂર્ણાંક		idx;
    પૂર્ણાંક		start;
    પૂર્ણાંક		ગણતરી;
    પૂર્ણાંક		i;
    સ્થિર પૂર્ણાંક  entered = FALSE;

    જો (buf->b_ml.ml_લીટી_lnum == 0 || buf->b_ml.ml_mfp == ખાલી)
	પાછા;		// nothing to do

    જો (buf->b_ml.ml_flags & ML_લીટી_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	જો (entered)
	    પાછા;
	entered = TRUE;

	lnum = buf->b_ml.ml_લીટી_lnum;
	new_લીટી = buf->b_ml.ml_લીટી_ptr;

	hp = ml_find_લીટી(buf, lnum, ML_FIND);
	જો (hp == ખાલી)
	    siemsg(e_cannot_find_લીટી_nr, lnum);
	નહીંતર
	{
	    dp = (DATA_BL *)(hp->bh_data);
	    idx = lnum - buf->b_ml.ml_locked_નીચલું;
	    start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	    old_લીટી = (char_u *)dp + start;
	    જો (idx == 0)	// line is last in block
		old_len = dp->db_txt_end - start;
	    નહીંતર		// text of previous line follows
		old_len = (dp->db_index[idx - 1] & DB_INDEX_MASK) - start;
	    new_len = buf->b_ml.ml_લીટી_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    જો ((પૂર્ણાંક)dp->db_free >= extra)
	    {
#જો defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		પૂર્ણાંક old_prop_len = 0;
		જો (buf->b_has_textprop)
		    old_prop_len = old_len - (પૂર્ણાંક)STRLEN(old_લીટી) - 1;
#endif
		// if the length changes and there are following lines
		ગણતરી = buf->b_ml.ml_locked_ઉપલું - buf->b_ml.ml_locked_નીચલું + 1;
		જો (extra != 0 && idx < ગણતરી - 1)
		{
		    // move text of following lines
		    mch_memmove((અક્ષર *)dp + dp->db_txt_start - extra,
				(અક્ષર *)dp + dp->db_txt_start,
				(size_t)(start - dp->db_txt_start));

		    // adjust pointers of this and following lines
		    માટે (i = idx + 1; i < ગણતરી; ++i)
			dp->db_index[i] -= extra;
		}
		dp->db_index[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_start -= extra;

		// copy new line into the data block
		mch_memmove(old_લીટી - extra, new_લીટી, (size_t)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#જો defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		જો (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (પૂર્ણાંક)STRLEN(new_લીટી) - 1;
		}
		જો (extra != 0)
		    ml_updatechunk(buf, lnum, (long)extra, ML_CHNK_UPDLINE);
#endif
	    }
	    નહીંતર
	    {
		/*
		 * Cannot do it in one data block: Delete and append.
		 * Append first, because ml_delete_int() cannot delete the
		 * last line in a buffer, which causes trouble for a buffer
		 * that has only one line.
		 * Don't forget to copy the mark!
		 */
		// How about handling errors???
		(શૂન્ય)ml_append_int(buf, lnum, new_લીટી, new_len,
			 ((dp->db_index[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#ifdef FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#endif
			 );
		(શૂન્ય)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_લીટી);

	entered = FALSE;
    }
    નહીંતર જો (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_લીટી_ptr);

    buf->b_ml.ml_flags &= ~(ML_લીટી_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_લીટી_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    સ્થિર bhdr_T *
ml_new_data(memfile_T *mfp, પૂર્ણાંક negative, પૂર્ણાંક page_ગણતરી)
{
    bhdr_T	*hp;
    DATA_BL	*dp;

    જો ((hp = mf_new(mfp, negative, page_ગણતરી)) == ખાલી)
	પાછા ખાલી;

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_id = DATA_ID;
    dp->db_txt_start = dp->db_txt_end = page_ગણતરી * mfp->mf_page_size;
    dp->db_free = dp->db_txt_start - HEADER_SIZE;
    dp->db_લીટી_ગણતરી = 0;

    પાછા hp;
}

/*
 * create a new, empty, pointer block
 */
    સ્થિર bhdr_T *
ml_new_ptr(memfile_T *mfp)
{
    bhdr_T	*hp;
    PTR_BL	*pp;

    જો ((hp = mf_new(mfp, FALSE, 1)) == ખાલી)
	પાછા ખાલી;

    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_id = PTR_ID;
    pp->pb_ગણતરી = 0;
    pp->pb_ગણતરી_max = PB_ગણતરી_MAX(mfp);

    પાછા hp;
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
    સ્થિર bhdr_T *
ml_find_લીટી(buf_T *buf, linenr_T lnum, પૂર્ણાંક action)
{
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    પૂર્ણાંક		dirty;
    linenr_T	નીચલું, ઉપલું;
    પૂર્ણાંક		ટોચ;
    પૂર્ણાંક		page_ગણતરી;
    પૂર્ણાંક		idx;

    mfp = buf->b_ml.ml_mfp;

    /*
     * If there is a locked block check if the wanted line is in it.
     * If not, flush and release the locked block.
     * Don't do this for ML_INSERT_SAME, because the stack need to be updated.
     * Don't do this for ML_FLUSH, because we want to flush the locked block.
     * Don't do this when 'swapfile' is reset, we want to load all the blocks.
     */
    જો (buf->b_ml.ml_locked)
    {
	જો (ML_SIMPLE(action)
		&& buf->b_ml.ml_locked_નીચલું <= lnum
		&& buf->b_ml.ml_locked_ઉપલું >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    જો (action == ML_ઉમેરો)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_ઉપલું);
	    }
	    નહીંતર જો (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_ઉપલું);
	    }
	    પાછા (buf->b_ml.ml_locked);
	}

	mf_put(mfp, buf->b_ml.ml_locked, buf->b_ml.ml_flags & ML_LOCKED_DIRTY,
					    buf->b_ml.ml_flags & ML_LOCKED_POS);
	buf->b_ml.ml_locked = ખાલી;

	/*
	 * If lines have been added or deleted in the locked block, need to
	 * update the line count in pointer blocks.
	 */
	જો (buf->b_ml.ml_locked_lineadd != 0)
	    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
    }

    જો (action == ML_FLUSH)	    // nothing else to do
	પાછા ખાલી;

    bnum = 1;			    // start at the root of the tree
    page_ગણતરી = 1;
    નીચલું = 1;
    ઉપલું = buf->b_ml.ml_લીટી_ગણતરી;

    જો (action == ML_FIND)	// first try stack entries
    {
	માટે (ટોચ = buf->b_ml.ml_ઢગલો_ટોચ - 1; ટોચ >= 0; --ટોચ)
	{
	    ip = &(buf->b_ml.ml_ઢગલો[ટોચ]);
	    જો (ip->ip_નીચલું <= lnum && ip->ip_ઉપલું >= lnum)
	    {
		bnum = ip->ip_bnum;
		નીચલું = ip->ip_નીચલું;
		ઉપલું = ip->ip_ઉપલું;
		buf->b_ml.ml_ઢગલો_ટોચ = ટોચ;	// truncate stack at prev entry
		તોડો;
	    }
	}
	જો (ટોચ < 0)
	    buf->b_ml.ml_ઢગલો_ટોચ = 0;		// not found, start at the root
    }
    નહીંતર	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_ઢગલો_ટોચ = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    માટે (;;)
    {
	જો ((hp = mf_get(mfp, bnum, page_ગણતરી)) == ખાલી)
	    જાઓ error_noblock;

	/*
	 * update high for insert/delete
	 */
	જો (action == ML_ઉમેરો)
	    ++ઉપલું;
	નહીંતર જો (action == ML_DELETE)
	    --ઉપલું;

	dp = (DATA_BL *)(hp->bh_data);
	જો (dp->db_id == DATA_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_નીચલું = નીચલું;
	    buf->b_ml.ml_locked_ઉપલું = ઉપલું;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    પાછા hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	જો (pp->pb_id != PTR_ID)
	{
	    iemsg(e_pointer_block_id_wrong);
	    જાઓ error_block;
	}

	જો ((ટોચ = ml_add_ઢગલો(buf)) < 0)	// add new entry to stack
	    જાઓ error_block;
	ip = &(buf->b_ml.ml_ઢગલો[ટોચ]);
	ip->ip_bnum = bnum;
	ip->ip_નીચલું = નીચલું;
	ip->ip_ઉપલું = ઉપલું;
	ip->ip_index = -1;		// index not known yet

	dirty = FALSE;
	માટે (idx = 0; idx < (પૂર્ણાંક)pp->pb_ગણતરી; ++idx)
	{
	    t = pp->pb_pointer[idx].pe_લીટી_ગણતરી;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    જો ((નીચલું += t) > lnum)
	    {
		ip->ip_index = idx;
		bnum = pp->pb_pointer[idx].pe_bnum;
		page_ગણતરી = pp->pb_pointer[idx].pe_page_ગણતરી;
		ઉપલું = નીચલું - 1;
		નીચલું -= t;

		/*
		 * a negative block number may have been changed
		 */
		જો (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    જો (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_pointer[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		તોડો;
	    }
	}
	જો (idx >= (પૂર્ણાંક)pp->pb_ગણતરી)	    // past the end: something wrong!
	{
	    જો (lnum > buf->b_ml.ml_લીટી_ગણતરી)
		siemsg(e_લીટી_સંખ્યા_out_of_range_nr_past_the_end,
					      lnum - buf->b_ml.ml_લીટી_ગણતરી);

	    નહીંતર
		siemsg(e_લીટી_ગણતરી_wrong_in_block_nr, bnum);
	    જાઓ error_block;
	}
	જો (action == ML_DELETE)
	{
	    pp->pb_pointer[idx].pe_લીટી_ગણતરી--;
	    dirty = TRUE;
	}
	નહીંતર જો (action == ML_ઉમેરો)
	{
	    pp->pb_pointer[idx].pe_લીટી_ગણતરી++;
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
    જો (action == ML_DELETE)
	ml_lineadd(buf, 1);
    નહીંતર જો (action == ML_ઉમેરો)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_ઢગલો_ટોચ = 0;
    પાછા ખાલી;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    સ્થિર પૂર્ણાંક
ml_add_ઢગલો(buf_T *buf)
{
    પૂર્ણાંક		ટોચ;
    infoptr_T	*newstack;

    ટોચ = buf->b_ml.ml_ઢગલો_ટોચ;

    // may have to increase the stack size
    જો (ટોચ == buf->b_ml.ml_ઢગલો_size)
    {
	CHECK(ટોચ > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_ઢગલો_size + ઢગલો_INCR);
	જો (newstack == ખાલી)
	    પાછા -1;
	જો (ટોચ > 0)
	    mch_memmove(newstack, buf->b_ml.ml_ઢગલો,
					     (size_t)ટોચ * કદ(infoptr_T));
	vim_free(buf->b_ml.ml_ઢગલો);
	buf->b_ml.ml_ઢગલો = newstack;
	buf->b_ml.ml_ઢગલો_size += ઢગલો_INCR;
    }

    buf->b_ml.ml_ઢગલો_ટોચ++;
    પાછા ટોચ;
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
    સ્થિર શૂન્ય
ml_lineadd(buf_T *buf, પૂર્ણાંક ગણતરી)
{
    પૂર્ણાંક		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    માટે (idx = buf->b_ml.ml_ઢગલો_ટોચ - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_ઢગલો[idx]);
	જો ((hp = mf_get(mfp, ip->ip_bnum, 1)) == ખાલી)
	    તોડો;
	pp = (PTR_BL *)(hp->bh_data);	// must be pointer block
	જો (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_pointer_block_id_wrong_two);
	    તોડો;
	}
	pp->pb_pointer[ip->ip_index].pe_લીટી_ગણતરી += ગણતરી;
	ip->ip_ઉપલું += ગણતરી;
	mf_put(mfp, hp, TRUE, FALSE);
    }
}

#જો defined(HAVE_READLINK)
/*
 * Resolve a symlink in the last component of a file name.
 * Note that f_resolve() does it for every part of the path, we don't do that
 * here.
 * If it worked returns OK and the resolved link in "buf[MAXPATHL]".
 * Otherwise returns FAIL.
 */
    પૂર્ણાંક
resolve_symlink(char_u *fname, char_u *buf)
{
    char_u	tmp[MAXPATHL];
    પૂર્ણાંક		ret;
    પૂર્ણાંક		depth = 0;

    જો (fname == ખાલી)
	પાછા FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    માટે (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	જો (++depth == 100)
	{
	    semsg(_(e_symlink_loop_for_str), fname);
	    પાછા FAIL;
	}

	ret = readlink((અક્ષર *)tmp, (અક્ષર *)buf, MAXPATHL - 1);
	જો (ret <= 0)
	{
	    જો (errno == EINVAL || errno == ENOENT)
	    {
		// Found non-symlink or not existing file, stop here.
		// When at the first level use the unmodified name, skip the
		// call to vim_FullName().
		જો (depth == 1)
		    પાછા FAIL;

		// Use the resolved name in tmp[].
		તોડો;
	    }

	    // There must be some error reading links, use original name.
	    પાછા FAIL;
	}
	buf[ret] = NUL;

	/*
	 * Check whether the symlink is relative or absolute.
	 * If it's relative, build a new path based on the directory
	 * portion of the filename (if any) and the path the symlink
	 * points to.
	 */
	જો (mch_isFullName(buf))
	    STRCPY(tmp, buf);
	નહીંતર
	{
	    char_u *tail;

	    tail = gettail(tmp);
	    જો (STRLEN(tail) + STRLEN(buf) >= MAXPATHL)
		પાછા FAIL;
	    STRCPY(tail, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    પાછા vim_FullName(tmp, buf, MAXPATHL, TRUE);
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
    જો (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#endif

#જો defined(UNIX) || defined(MSWIN)  // Need _very_ long file names
    પૂર્ણાંક		len = (પૂર્ણાંક)STRLEN(dir_name);

    s = dir_name + len;
    જો (after_pathsep(dir_name, s) && len > 1 && s[-1] == s[-2])
    {			       // Ends with '//', Use Full path
	r = ખાલી;
	જો ((s = make_percent_swname(dir_name, s, fname_res)) != ખાલી)
	{
	    r = modname(s, (char_u *)".swp", FALSE);
	    vim_free(s);
	}
	પાછા r;
    }
#endif

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#જો defined(VMS)
	    "_swp",
#નહીંતર
	    ".swp",
#endif
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    જો (r == ખાલી)	    // out of memory
	પાછા ખાલી;

    s = get_file_in_dir(r, dir_name);
    vim_free(r);
    પાછા s;
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

    જો (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (size_t)(tail.string - fname) + tail.length);
    નહીંતર
    {
	size_t	dname_len = STRLEN(dname);

	જો (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    જો (tail.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    tail.string, tail.length, TRUE, &retval);
	    નહીંતર
	    {
		પૂર્ણાંક	    save_char;
		string_T    tmp;

		save_char = *tail.string;
		*tail.string = NUL;
		concat_fnames(fname, (size_t)(tail.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*tail.string = save_char;
		જો (tmp.string == ખાલી)	    // out of memory
		    retval.string = ખાલી;
		નહીંતર
		{
		    concat_fnames(tmp.string, tmp.length,
			tail.string, tail.length, TRUE, &retval);
		    vim_free(tmp.string);
		}
	    }
	}
	નહીંતર
	    concat_fnames(dname, dname_len, tail.string, tail.length,
		TRUE, &retval);
    }

#ifdef MSWIN
    જો (retval.string != ખાલી)
    {
	char_u	*t;

	માટે (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    જો (*t == ':')
		*t = '%';
    }
#endif

    પાછા retval.string;
}

/*
 * Print the ATTENTION message: info about an existing swap file.
 */
    સ્થિર શૂન્ય
attention_message(
    buf_T   *buf,	// buffer being edited
    char_u  *fname)	// swap file name
{
    stat_T	st;
    time_t	swap_mtime;

    ++no_wait_return;
    (શૂન્ય)emsg(_(e_attention));
    સંદેશ_puts(_("\nFound a swap file by the name \""));
    સંદેશ_home_replace(fname);
    સંદેશ_puts("\"\n");
    swap_mtime = swapfile_info(fname);
    સંદેશ_puts(_("While opening file \""));
    સંદેશ_outtrans(buf->b_fname);
    સંદેશ_puts("\"\n");
    જો (mch_stat((અક્ષર *)buf->b_fname, &st) == -1)
    {
	સંદેશ_puts(_("      CANNOT BE FOUND"));
    }
    નહીંતર
    {
	સંદેશ_puts(_("             dated: "));
	સંદેશ_puts(get_ctime(st.st_mtime, TRUE));
	જો (swap_mtime != 0 && st.st_mtime > swap_mtime)
	    સંદેશ_puts(_("      NEWER than swap file!\n"));
    }
    // Some of these messages are long to allow translation to
    // other languages.
    સંદેશ_puts(_("\n(1) Another program may be editing the same file.  If this is the case,\n    be careful not to end up with two different instances of the same\n    file when making changes.  Quit, or continue with caution.\n"));
    સંદેશ_puts(_("(2) An edit session for this file crashed.\n"));
    સંદેશ_puts(_("    If this is the case, use \":recover\" or \"vim -r "));
    સંદેશ_outtrans(buf->b_fname);
    સંદેશ_puts(_("\"\n    to recover the changes (see \":help recovery\").\n"));
    સંદેશ_puts(_("    If you did this already, delete the swap file \""));
    સંદેશ_outtrans(fname);
    સંદેશ_puts(_("\"\n    to avoid this message.\n"));
    cmdline_row = સંદેશ_row;
    --no_wait_return;
}

પ્રકારવ્યાખ્યા ગણના {
    SEA_CHOICE_NONE = 0,
    SEA_CHOICE_READONLY = 1,
    SEA_CHOICE_EDIT = 2,
    SEA_CHOICE_RECOVER = 3,
    SEA_CHOICE_DELETE = 4,
    SEA_CHOICE_QUIT = 5,
    SEA_CHOICE_ABORT = 6
} sea_choice_T;

#જો defined(FEAT_EVAL)
/*
 * Trigger the SwapExists autocommands.
 * Returns a value for equivalent to do_dialog().
 */
    સ્થિર sea_choice_T
do_swapexists(buf_T *buf, char_u *fname)
{
    set_vim_var_string(VV_SWAPNAME, fname, -1);
    set_vim_var_string(VV_SWAPCHOICE, ખાલી, -1);

    // Trigger SwapExists autocommands with <afile> set to the file being
    // edited.  Disallow changing directory here.
    ++allbuf_lock;
    apply_autocmds(EVENT_SWAPEXISTS, buf->b_fname, ખાલી, FALSE, ખાલી);
    --allbuf_lock;

    set_vim_var_string(VV_SWAPNAME, ખાલી, -1);

    પસંદગી (*get_vim_var_str(VV_SWAPCHOICE))
    {
	સ્થિતિ 'o': પાછા SEA_CHOICE_READONLY;
	સ્થિતિ 'e': પાછા SEA_CHOICE_EDIT;
	સ્થિતિ 'r': પાછા SEA_CHOICE_RECOVER;
	સ્થિતિ 'd': પાછા SEA_CHOICE_DELETE;
	સ્થિતિ 'q': પાછા SEA_CHOICE_QUIT;
	સ્થિતિ 'a': પાછા SEA_CHOICE_ABORT;
    }

    પાછા SEA_CHOICE_NONE;
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
    સ્થિર char_u *
findswapname(
    buf_T	*buf,
    char_u	**dirp,		// pointer to list of directories
    char_u	*old_fname)	// don't give warning for this file name
{
    char_u	*fname;
    પૂર્ણાંક		n;
    char_u	*dir_name;
#ifdef AMIGA
    BPTR	fh;
#endif
    પૂર્ણાંક		r;
    char_u	*buf_fname = buf->b_fname;

#જો !defined(UNIX)
# define CREATE_DUMMY_FILE
    FILE	*dummyfd = ખાલી;

# ifdef MSWIN
    જો (buf_fname != ખાલી && !mch_isFullName(buf_fname)
				       && vim_strchr(gettail(buf_fname), ':'))
    {
	char_u *t;

	buf_fname = vim_strsave(buf_fname);
	જો (buf_fname == ખાલી)
	    buf_fname = buf->b_fname;
	નહીંતર
	    માટે (t = gettail(buf_fname); *t != NUL; MB_PTR_ADV(t))
		જો (*t == ':')
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
    જો (!(buf->b_p_sn || buf->b_shortname) && buf_fname != ખાલી
					     && mch_getperm(buf_fname) < 0)
	dummyfd = mch_fopen((અક્ષર *)buf_fname, "w");
#endif

    /*
     * Isolate a directory name from *dirp and put it in dir_name.
     * First allocate some memory to put the directory name in.
     */
    dir_name = alloc(STRLEN(*dirp) + 1);
    જો (dir_name == ખાલી)
	*dirp = ખાલી;
    નહીંતર
	(શૂન્ય)copy_option_part(dirp, dir_name, 31000, ",");

    /*
     * we try different names until we find one that does not exist yet
     */
    જો (dir_name == ખાલી)	    // out of memory
	fname = ખાલી;
    નહીંતર
	fname = makeswapname(buf_fname, buf->b_ffname, buf, dir_name);

    માટે (;;)
    {
	જો (fname == ખાલી)	// must be out of memory
	    તોડો;
	જો ((n = (પૂર્ણાંક)STRLEN(fname)) == 0)	// safety check
	{
	    VIM_CLEAR(fname);
	    તોડો;
	}
#જો defined(UNIX)
/*
 * Some systems have a MS-DOS compatible filesystem that use 8.3 character
 * file names. If this is the first try and the swap file name does not fit in
 * 8.3, detect if this is the case, set shortname and try again.
 */
	જો (fname[n - 2] == 'w' && fname[n - 1] == 'p'
					&& !(buf->b_p_sn || buf->b_shortname))
	{
	    char_u	    *tail;
	    char_u	    *fname2;
	    stat_T	    s1, s2;
	    પૂર્ણાંક		    f1, f2;
	    પૂર્ણાંક		    created1 = FALSE, created2 = FALSE;
	    પૂર્ણાંક		    same = FALSE;

	    /*
	     * Check if swapfile name does not fit in 8.3:
	     * It either contains two dots, is longer than 8 chars, or starts
	     * with a dot.
	     */
	    tail = gettail(buf_fname);
	    જો (       vim_strchr(tail, '.') != ખાલી
		    || STRLEN(tail) > (size_t)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		જો (fname2 != ખાલી)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    જો (vim_strchr(tail, '.') != ખાલી)
			fname2[n - 1] = 'x';
		    નહીંતર જો (*gettail(fname) == '.')
		    {
			fname2[n] = 'x';
			fname2[n + 1] = NUL;
		    }
		    નહીંતર
			fname2[n - 5] += 1;
		    /*
		     * may need to create the files to be able to use mch_stat()
		     */
		    f1 = mch_open((અક્ષર *)fname, O_RDONLY | O_EXTRA, 0);
		    જો (f1 < 0)
		    {
			f1 = mch_open_rw((અક્ષર *)fname,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			created1 = TRUE;
		    }
		    જો (f1 >= 0)
		    {
			f2 = mch_open((અક્ષર *)fname2, O_RDONLY | O_EXTRA, 0);
			જો (f2 < 0)
			{
			    f2 = mch_open_rw((અક્ષર *)fname2,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			    created2 = TRUE;
			}
			જો (f2 >= 0)
			{
			    /*
			     * Both files exist now. If mch_stat() returns the
			     * same device and inode they are the same file.
			     */
			    જો (mch_fstat(f1, &s1) != -1
				    && mch_fstat(f2, &s2) != -1
				    && s1.st_dev == s2.st_dev
				    && s1.st_ino == s2.st_ino)
				same = TRUE;
			    close(f2);
			    જો (created2)
				mch_remove(fname2);
			}
			close(f1);
			જો (created1)
			    mch_remove(fname);
		    }
		    vim_free(fname2);
		    જો (same)
		    {
			buf->b_shortname = true;
			vim_free(fname);
			fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
			ચાલુ;	// try again with b_shortname set
		    }
		}
	    }
	}
#endif
	/*
	 * check if the swapfile already exists
	 */
	જો (mch_getperm(fname) < 0)	// it does not exist
	{
#ifdef HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    જો (mch_lstat((અક્ષર *)fname, &sb) < 0)
#નહીંતર
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (long)MODE_NEWFILE);
	    /*
	     * on the Amiga mch_getperm() will return -1 when the file exists
	     * but is being used by another program. This happens if you edit
	     * a file twice.
	     */
	    જો (fh != (BPTR)ખાલી)	// can open file, OK
	    {
		Close(fh);
		mch_remove(fname);
		તોડો;
	    }
	    જો (IoErr() != ERROR_OBJECT_IN_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#endif
		તોડો;
	}

	/*
	 * A file name equal to old_fname is OK to use.
	 */
	જો (old_fname != ખાલી && fnamecmp(fname, old_fname) == 0)
	    તોડો;

	/*
	 * get here when file already exists
	 */
	જો (fname[n - 2] == 'w' && fname[n - 1] == 'p')	// first try
	{
	    /*
	     * on MS-DOS compatible filesystems (e.g. messydos) file.doc.swp
	     * and file.doc are the same file. To guess if this problem is
	     * present try if file.doc.swx exists. If it does, we set
	     * buf->b_shortname and try file_doc.swp (dots replaced by
	     * underscores for this file), and try again. If it doesn't we
	     * assume that "file.doc.swp" already exists.
	     */
	    જો (!(buf->b_p_sn || buf->b_shortname))	// not tried yet
	    {
		fname[n - 1] = 'x';
		r = mch_getperm(fname);		// try "file.swx"
		fname[n - 1] = 'p';
		જો (r >= 0)		    // "file.swx" seems to exist
		{
		    buf->b_shortname = true;
		    vim_free(fname);
		    fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
		    ચાલુ;	    // try again with '.' replaced with '_'
		}
	    }
	    /*
	     * If we get here the ".swp" file really exists.
	     * Give an error message, unless recovering, no file name, we are
	     * viewing a help file or when the path of the file is different
	     * (happens when all .swp files are in one directory).
	     */
	    જો (!recoverymode && buf_fname != ખાલી
				&& !buf->b_help
				&& !(buf->b_flags & (BF_DUMMY | BF_NO_SEA)))
	    {
		પૂર્ણાંક		fd;
		સંરચના block0	b0;
		પૂર્ણાંક		differ = FALSE;

		/*
		 * Try to read block 0 from the swap file to get the original
		 * file name (and inode number).
		 */
		fd = mch_open((અક્ષર *)fname, O_RDONLY | O_EXTRA, 0);
		જો (fd >= 0)
		{
		    જો (read_eintr(fd, &b0, કદ(b0)) == કદ(b0))
		    {
			/*
			 * If the swapfile has the same directory as the
			 * buffer don't compare the directory names, they can
			 * have a different mountpoint.
			 */
			જો (b0.b0_flags & B0_SAME_DIR)
			{
			    જો (fnamecmp(gettail(buf->b_ffname),
						   gettail(b0.b0_fname)) != 0
				    || !same_directory(fname, buf->b_ffname))
			    {
#ifdef CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				જો (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
#endif
				    differ = TRUE;
			    }
			}
			નહીંતર
			{
			    /*
			     * The name in the swap file may be
			     * "~user/path/file".  Expand it first.
			     */
			    expand_env(b0.b0_fname, NameBuff, MAXPATHL);
#ifdef CHECK_INODE
			    જો (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
				differ = TRUE;
#નહીંતર
			    જો (fnamecmp(NameBuff, buf->b_ffname) != 0)
				differ = TRUE;
#endif
			}
		    }
		    close(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		જો (differ == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == ખાલી)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#ifdef CREATE_DUMMY_FILE
		    પૂર્ણાંક		 did_use_dummy = FALSE;

		    // Avoid getting a warning for the file being created
		    // outside of Vim, it was created at the start of this
		    // function.  Delete the file now, because Vim might exit
		    // here if the window is closed.
		    જો (dummyfd != ખાલી)
		    {
			fclose(dummyfd);
			dummyfd = ખાલી;
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
		    જો (mch_stat((અક્ષર *)buf->b_fname, &st) == 0
						  && swapfile_unchanged(fname))
		    {
			choice = SEA_CHOICE_DELETE;
			જો (p_verbose > 0)
			    verb_સંદેશ(_("Found a swap file that is not useful, deleting it"));
		    }

#જો defined(FEAT_EVAL)
		    /*
		     * If there is an SwapExists autocommand and we can handle
		     * the response, trigger it.  It may return 0 to ask the
		     * user anyway.
		     */
		    જો (choice == SEA_CHOICE_NONE
			    && swap_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#endif

		    જો (choice == SEA_CHOICE_NONE
					 && swap_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    જો (choice == SEA_CHOICE_NONE)
		    {
#ifdef FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			જો (gui.starting && !gui.in_use)
			    gui_start(ખાલી);
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

#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    જો (swap_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    prefix = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			size_t	    message_size;
			string_T    message;
			char_u	    *tofree;
			પૂર્ણાંક	    dialog_result;

			prefix.length = STRLEN(prefix.string);
			suffix.length = STRLEN(suffix.string);
			message_size = prefix.length
				+ STRLEN(fname)
				+ suffix.length + 5;
			message.string = tofree = alloc(message_size);
			જો (message.string != ખાલી)
			{
			    STRCPY(message.string, prefix.string);
			    message.length = prefix.length;
			    message.length += home_replace(ખાલી, fname,
				message.string + message.length, (પૂર્ણાંક)(message_size - message.length), TRUE);
			    STRCPY(message.string + message.length, suffix.string);
			}
			નહીંતર
			    message.string = (char_u *)_("Swap file already exists!");
			dialog_result = do_dialog(VIM_WARNING,
				    (char_u *)_("VIM - ATTENTION"),
				    message.string,
# ifdef HAVE_PROCESS_STILL_RUNNING
				    process_still_running
					? (char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Quit\n&Abort") :
# endif
					(char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Delete it\n&Quit\n&Abort"), 1, ખાલી, FALSE);

# ifdef HAVE_PROCESS_STILL_RUNNING
			જો (process_still_running && dialog_result >= 4)
			    // compensate for missing "Delete it" button
			    dialog_result++;
# endif
			choice = dialog_result;
			vim_free(tofree);

			// pretend screen didn't scroll, need redraw anyway
			સંદેશ_scrolled = 0;
			redraw_all_later(UPD_NOT_VALID);
		    }
#endif

		    પસંદગી (choice)
		    {
			સ્થિતિ SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    તોડો;
			સ્થિતિ SEA_CHOICE_EDIT:
			    તોડો;
			સ્થિતિ SEA_CHOICE_RECOVER:
			    swap_exists_action = SEA_RECOVER;
			    તોડો;
			સ્થિતિ SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    તોડો;
			સ્થિતિ SEA_CHOICE_QUIT:
			    swap_exists_action = SEA_QUIT;
			    તોડો;
			સ્થિતિ SEA_CHOICE_ABORT:
			    swap_exists_action = SEA_QUIT;
			    got_int = TRUE;
			    તોડો;
			સ્થિતિ SEA_CHOICE_NONE:
			    સંદેશ_puts("\n");
			    જો (સંદેશ_silent == 0)
				// call wait_return() later
				need_wait_return = TRUE;
			    તોડો;
		    }

		    // If the file was deleted this fname can be used.
		    જો (choice != SEA_CHOICE_NONE && mch_getperm(fname) < 0)
			તોડો;

#ifdef CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    જો (did_use_dummy)
			dummyfd = mch_fopen((અક્ષર *)buf_fname, "w");
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
	જો (fname[n - 1] == 'a')	// ".s?a"
	{
	    જો (fname[n - 2] == 'a')    // ".saa": tried enough, give up
	    {
		emsg(_(e_too_many_swap_files_found));
		VIM_CLEAR(fname);
		તોડો;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#ifdef CREATE_DUMMY_FILE
    જો (dummyfd != ખાલી)	// file has been created temporarily
    {
	fclose(dummyfd);
	mch_remove(buf_fname);
    }
#endif
#ifdef MSWIN
    જો (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#endif
    પાછા fname;
}

    સ્થિર પૂર્ણાંક
b0_magic_wrong(ZERO_BL *b0p)
{
    પાછા (b0p->b0_magic_long != (long)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (પૂર્ણાંક)B0_MAGIC_INT
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

    સ્થિર પૂર્ણાંક
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
    પૂર્ણાંક		retval_c;	    // flag: buf_c valid
    પૂર્ણાંક		retval_s;	    // flag: buf_s valid

    જો (mch_stat((અક્ષર *)fname_c, &st) == 0)
	ino_c = (ino_t)st.st_ino;

    /*
     * First we try to get the inode from the file name, because the inode in
     * the swap file may be outdated.  If that fails (e.g. this path is not
     * valid on this machine), use the inode from block 0.
     */
    જો (mch_stat((અક્ષર *)fname_s, &st) == 0)
	ino_s = (ino_t)st.st_ino;
    નહીંતર
	ino_s = (ino_t)ino_block0;

    જો (ino_c && ino_s)
	પાછા (ino_c != ino_s);

    /*
     * One of the inode numbers is unknown, try a forced vim_FullName() and
     * compare the file names.
     */
    retval_c = vim_FullName(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_FullName(fname_s, buf_s, MAXPATHL, TRUE);
    જો (retval_c == OK && retval_s == OK)
	પાછા STRCMP(buf_c, buf_s) != 0;

    /*
     * Can't compare inodes or file names, guess that the files are different,
     * unless both appear not to exist at all, then compare with the file name
     * in the swap file.
     */
    જો (ino_s == 0 && ino_c == 0 && retval_c == FAIL && retval_s == FAIL)
	પાછા STRCMP(fname_c, fname_s) != 0;
    પાછા TRUE;
}
#endif // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    સ્થિર શૂન્ય
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

    સ્થિર long
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

    પાછા retval;
}

/*
 * Set the flags in the first block of the swap file:
 * - file is modified or not: buf->b_changed
 * - 'fileformat'
 * - 'fileencoding'
 */
    શૂન્ય
ml_setflags(buf_T *buf)
{
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    જો (!buf->b_ml.ml_mfp)
	પાછા;
    માટે (hp = buf->b_ml.ml_mfp->mf_used_last; hp != ખાલી; hp = hp->bh_prev)
    {
	જો (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_data);
	    b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_MASK)
						  | (get_fileformat(buf) + 1);
	    add_b0_fenc(b0p, buf);
	    hp->bh_flags |= BH_DIRTY;
	    mf_sync(buf->b_ml.ml_mfp, MFS_ZERO);
	    તોડો;
	}
    }
}

#જો defined(FEAT_CRYPT)
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
    પૂર્ણાંક		text_len;
    cryptstate_T *state;

    જો (dp->db_id != DATA_ID)
	પાછા data;

    state = ml_crypt_prepare(mfp, offset, FALSE);
    જો (state == ખાલી)
	પાછા data;

    new_data = alloc(size);
    જો (new_data == ખાલી)
    {
	crypt_free_state(state);
	પાછા ખાલી;
    }
    head_end = (char_u *)(&dp->db_index[dp->db_લીટી_ગણતરી]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = size - dp->db_txt_start;

    // Copy the header and the text.
    mch_memmove(new_data, dp, head_end - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, text_start, text_len, new_data + dp->db_txt_start,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    જો (head_end < text_start)
	vim_memset(new_data + (head_end - data), 0, text_start - head_end);

    પાછા new_data;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    શૂન્ય
ml_decrypt_data(
    memfile_T	*mfp,
    char_u	*data,
    off_T	offset,
    unsigned	size)
{
    DATA_BL	*dp = (DATA_BL *)data;
    char_u	*head_end;
    char_u	*text_start;
    પૂર્ણાંક		text_len;
    cryptstate_T *state;

    જો (dp->db_id != DATA_ID)
	પાછા;

    head_end = (char_u *)(&dp->db_index[dp->db_લીટી_ગણતરી]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = dp->db_txt_end - dp->db_txt_start;

    જો (head_end > text_start || dp->db_txt_start > size
	    || dp->db_txt_end > size)
	પાછા;  // data was messed up

    state = ml_crypt_prepare(mfp, offset, TRUE);
    જો (state == ખાલી)
	પાછા;

    // Decrypt the text in place.
    crypt_decode_inplace(state, text_start, text_len, FALSE);
    crypt_free_state(state);
}

/*
 * Prepare for encryption/decryption, using the key, seed and offset.
 * Return an allocated cryptstate_T *.
 * Note: Encryption not supported for SODIUM
 */
    સ્થિર cryptstate_T *
ml_crypt_prepare(memfile_T *mfp, off_T offset, પૂર્ણાંક reading)
{
    buf_T	*buf = mfp->mf_buffer;
    char_u	salt[50];
    પૂર્ણાંક		method_nr;
    char_u	*ચાવી;
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    જો (reading && mfp->mf_old_ચાવી != ખાલી)
    {
	// Reading back blocks with the previous key/method/seed.
	method_nr = mfp->mf_old_cm;
	ચાવી = mfp->mf_old_ચાવી;
	arg.cat_seed = mfp->mf_old_seed;
    }
    નહીંતર
    {
	method_nr = crypt_get_method_nr(buf);
	ચાવી = buf->b_p_ચાવી;
	arg.cat_seed = mfp->mf_seed;
    }

    જો (*ચાવી == NUL)
	પાછા ખાલી;

    જો (crypt_may_close_swapfile(buf, ચાવી, method_nr))
	પાછા ખાલી;

    જો (method_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((અક્ષર *)salt, કદ(salt), "%s%ld", ચાવી, (long)offset);
	arg.cat_seed = ખાલી;
	arg.cat_init_from_file = FALSE;

	પાછા crypt_create(method_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((અક્ષર *)salt, કદ(salt), "%ld", (long)offset);

    arg.cat_salt = salt;
    arg.cat_salt_len = (પૂર્ણાંક)STRLEN(salt);
    arg.cat_seed_len = MF_SEED_LEN;
    arg.cat_add_len = 0;
    arg.cat_add = ખાલી;
    arg.cat_init_from_file = FALSE;

    પાછા crypt_create(method_nr, ચાવી, &arg);
}

#endif


#જો defined(FEAT_BYTEOFF)

# define MLCS_MAXL 800	// max no of lines in chunk
# define MLCS_MINL 400   // should be half of MLCS_MAXL

/*
 * Keep information for finding byte offset of a line, updtype may be one of:
 * ML_CHNK_ADDLINE: Add len to parent chunk, possibly splitting it
 *	   Careful: ML_CHNK_ADDLINE may cause ml_find_line() to be called.
 * ML_CHNK_DELLINE: Subtract len from parent chunk, possibly deleting it
 * ML_CHNK_UPDLINE: Add len to parent chunk, as a signed entity.
 */
    સ્થિર શૂન્ય
ml_updatechunk(
    buf_T	*buf,
    linenr_T	લીટી,
    long	len,
    પૂર્ણાંક		updtype)
{
    સ્થિર buf_T	*ml_upd_lastbuf = ખાલી;
    સ્થિર linenr_T	ml_upd_lastcurline;
    સ્થિર પૂર્ણાંક		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    પૂર્ણાંક			curix = ml_upd_lastcurix;
    long		size;
    chunksize_T		*curchnk;
    પૂર્ણાંક			rest;
    bhdr_T		*hp;
    DATA_BL		*dp;

    જો (buf->b_ml.ml_usedchunks == -1 || len == 0)
	પાછા;
    જો (buf->b_ml.ml_chunksize == ખાલી)
    {
	buf->b_ml.ml_chunksize = ALLOC_MULT(chunksize_T, 100);
	જો (buf->b_ml.ml_chunksize == ખાલી)
	{
	    buf->b_ml.ml_usedchunks = -1;
	    પાછા;
	}
	buf->b_ml.ml_numchunks = 100;
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = 1;
	ml_upd_lastbuf = ખાલી;		// invalidate resume cache
    }

    જો (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_લીટી_ગણતરી == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (long)buf->b_ml.ml_લીટી_len;
	ml_upd_lastbuf = ખાલી;		// invalidate resume cache
	પાછા;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    જો (buf != ml_upd_lastbuf || લીટી < curline)
    {
	curline = 1;
	curix = 0;
    }
    માટે (; curix < buf->b_ml.ml_usedchunks - 1
	     && લીટી >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	     curix++)
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
    curchnk = buf->b_ml.ml_chunksize + curix;

    જો (updtype == ML_CHNK_DELLINE)
	len = -len;
    curchnk->mlcs_totalsize += len;
    જો (updtype == ML_CHNK_ADDLINE)
    {
	curchnk->mlcs_numlines++;

	// May resize here so we don't have to do it in both cases below
	જો (buf->b_ml.ml_usedchunks + 1 >= buf->b_ml.ml_numchunks)
	{
	    chunksize_T *t_chunksize = buf->b_ml.ml_chunksize;

	    buf->b_ml.ml_numchunks = buf->b_ml.ml_numchunks * 3 / 2;
	    buf->b_ml.ml_chunksize = vim_realloc(buf->b_ml.ml_chunksize,
			    કદ(chunksize_T) * buf->b_ml.ml_numchunks);
	    જો (buf->b_ml.ml_chunksize == ખાલી)
	    {
		// Hmmmm, Give up on offset for this buffer
		vim_free(t_chunksize);
		buf->b_ml.ml_usedchunks = -1;
		પાછા;
	    }
	}

	જો (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MAXL)
	{
	    પૂર્ણાંક	    ગણતરી;	    // number of entries in block
	    પૂર્ણાંક	    idx;
	    પૂર્ણાંક	    end_idx;
	    પૂર્ણાંક	    text_end;
	    પૂર્ણાંક	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			કદ(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    size = 0;
	    linecnt = 0;
	    જ્યારે (curline < buf->b_ml.ml_લીટી_ગણતરી
			&& linecnt < MLCS_MINL)
	    {
		જો ((hp = ml_find_લીટી(buf, curline, ML_FIND)) == ખાલી)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    પાછા;
		}
		dp = (DATA_BL *)(hp->bh_data);
		ગણતરી = (long)(buf->b_ml.ml_locked_ઉપલું) -
			(long)(buf->b_ml.ml_locked_નીચલું) + 1;
		idx = curline - buf->b_ml.ml_locked_નીચલું;
		curline = buf->b_ml.ml_locked_ઉપલું + 1;

		// compute index of last line to use in this MEMLINE
		rest = ગણતરી - idx;
		જો (linecnt + rest > MLCS_MINL)
		{
		    end_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		નહીંતર
		{
		    end_idx = ગણતરી - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		જો (buf->b_has_textprop)
		{
		    પૂર્ણાંક i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    માટે (i = end_idx; i < idx; ++i)
			size += (પૂર્ણાંક)STRLEN((char_u *)dp
				      + (dp->db_index[i] & DB_INDEX_MASK)) + 1;
		}
		નહીંતર
# endif
		{
		    જો (idx == 0) // first line in block, text at the end
			text_end = dp->db_txt_end;
		    નહીંતર
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
	    ml_upd_lastbuf = ખાલી;   // Force recalc of curix & curline
	    પાછા;
	}
	નહીંતર જો (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_લીટી_ગણતરી - લીટી <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    જો (લીટી == buf->b_ml.ml_લીટી_ગણતરી)
	    {
		curchnk->mlcs_numlines = 0;
		curchnk->mlcs_totalsize = 0;
	    }
	    નહીંતર
	    {
		/*
		 * Line is just prior to last, move count for last
		 * This is the common case  when loading a new file
		 */
		hp = ml_find_લીટી(buf, buf->b_ml.ml_લીટી_ગણતરી, ML_FIND);
		જો (hp == ખાલી)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    પાછા;
		}
		dp = (DATA_BL *)(hp->bh_data);
		જો (dp->db_લીટી_ગણતરી == 1)
		    rest = dp->db_txt_end - dp->db_txt_start;
		નહીંતર
		    rest =
			((dp->db_index[dp->db_લીટી_ગણતરી - 2]) & DB_INDEX_MASK)
			- dp->db_txt_start;
		curchnk->mlcs_totalsize = rest;
		curchnk->mlcs_numlines = 1;
		curchnk[-1].mlcs_totalsize -= rest;
		curchnk[-1].mlcs_numlines -= 1;
	    }
	}
    }
    નહીંતર જો (updtype == ML_CHNK_DELLINE)
    {
	curchnk->mlcs_numlines--;
	જો (curix < buf->b_ml.ml_usedchunks - 1
		&& curchnk->mlcs_numlines + curchnk[1].mlcs_numlines
								  <= MLCS_MINL)
	{
	    curix++;
	    curchnk = buf->b_ml.ml_chunksize + curix;
	}
	નહીંતર જો (curix == 0 && curchnk->mlcs_numlines <= 0)
	{
	    ml_upd_lastbuf = ખાલી;   // Force recalc of curix & curline
	    buf->b_ml.ml_usedchunks--;
	    mch_memmove(buf->b_ml.ml_chunksize, buf->b_ml.ml_chunksize + 1,
			buf->b_ml.ml_usedchunks * કદ(chunksize_T));
	    પાછા;
	}
	નહીંતર જો (curix == 0 || (curchnk->mlcs_numlines > 10
		    && curchnk->mlcs_numlines + curchnk[-1].mlcs_numlines
								  > MLCS_MINL))
	{
	    // The chunks are left as they are, the cached position stays
	    // valid.
	    ml_upd_lastbuf = buf;
	    ml_upd_lastcurline = curline;
	    ml_upd_lastcurix = curix;
	    પાછા;
	}

	// Collapse chunks
	ml_upd_lastbuf = ખાલી;   // Force recalc of curix & curline
	curchnk[-1].mlcs_numlines += curchnk->mlcs_numlines;
	curchnk[-1].mlcs_totalsize += curchnk->mlcs_totalsize;
	buf->b_ml.ml_usedchunks--;
	જો (curix < buf->b_ml.ml_usedchunks)
	    mch_memmove(buf->b_ml.ml_chunksize + curix,
			buf->b_ml.ml_chunksize + curix + 1,
			(buf->b_ml.ml_usedchunks - curix) *
			કદ(chunksize_T));
	પાછા;
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
ml_find_લીટી_or_offset(buf_T *buf, linenr_T lnum, long *offp)
{
    linenr_T	curline;
    પૂર્ણાંક		curix;
    long	size;
    bhdr_T	*hp;
    DATA_BL	*dp;
    પૂર્ણાંક		ગણતરી;		// number of entries in block
    પૂર્ણાંક		idx;
    પૂર્ણાંક		start_idx;
    પૂર્ણાંક		text_end;
    long	offset;
    પૂર્ણાંક		len;
    પૂર્ણાંક		ffdos = (get_fileformat(buf) == EOL_DOS);
    પૂર્ણાંક		extra = 0;

    // take care of cached line first
    ml_flush_લીટી(curbuf);

    જો (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == ખાલી
	    || lnum < 0)
	પાછા -1;

    જો (offp == ખાલી)
	offset = 0;
    નહીંતર
	offset = *offp;
    જો (lnum == 0 && offset <= 0)
	પાછા 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = size = 0;
    જ્યારે (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (offset != 0
	       && offset > size + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (long)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	size += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	જો (offset && ffdos)
	    size += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    જ્યારે ((lnum != 0 && curline < lnum) || (offset != 0 && size < offset))
    {
# ifdef FEAT_PROP_POPUP
	size_t textprop_total = 0;
# endif

	જો (curline > buf->b_ml.ml_લીટી_ગણતરી
		|| (hp = ml_find_લીટી(buf, curline, ML_FIND)) == ખાલી)
	    પાછા -1;
	dp = (DATA_BL *)(hp->bh_data);
	ગણતરી = (long)(buf->b_ml.ml_locked_ઉપલું) -
		(long)(buf->b_ml.ml_locked_નીચલું) + 1;
	start_idx = idx = curline - buf->b_ml.ml_locked_નીચલું;
	જો (idx == 0)  // first line in block, text at the end
	    text_end = dp->db_txt_end;
	નહીંતર
	    text_end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);
	// Compute index of last line to use in this MEMLINE
	જો (lnum != 0)
	{
	    જો (curline + (ગણતરી - idx) >= lnum)
		idx += lnum - curline - 1;
	    નહીંતર
		idx = ગણતરી - 1;
	}
	નહીંતર
	{
	    extra = 0;
	    માટે (;;)
	    {
# ifdef FEAT_PROP_POPUP
		size_t textprop_size = 0;

		જો (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_index[idx]) & DB_INDEX_MASK);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_end
				  : ((dp->db_index[idx - 1]) & DB_INDEX_MASK));
		    textprop_size = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		જો (!(offset >= size
			+ text_end - (પૂર્ણાંક)((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
			- (long)(textprop_total + textprop_size)
# endif
			+ ffdos))
		    તોડો;

		જો (ffdos)
		    size++;
# ifdef FEAT_PROP_POPUP
		textprop_total += textprop_size;
# endif
		જો (idx == ગણતરી - 1)
		{
		    extra = 1;
		    તોડો;
		}
		idx++;
	    }
	}
# ifdef FEAT_PROP_POPUP
	જો (buf->b_has_textprop && lnum != 0)
	{
	    પૂર્ણાંક i;

	    // cannot use the db_index pointer, need to get the actual text
	    // lengths.
	    len = 0;
	    માટે (i = start_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_index[i]) & DB_INDEX_MASK);
		len += (પૂર્ણાંક)STRLEN(p) + 1;
	    }
	}
	નહીંતર
# endif
	    len = text_end - ((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
				- (long)textprop_total
# endif
				;
	size += len;
	જો (offset != 0 && size >= offset)
	{
	    જો (size + ffdos == offset)
		*offp = 0;
	    નહીંતર જો (idx == start_idx)
		*offp = offset - size + len;
	    નહીંતર
		*offp = offset - size + len
		     - (text_end - ((dp->db_index[idx - 1]) & DB_INDEX_MASK))
# ifdef FEAT_PROP_POPUP
		     + (long)textprop_total
# endif
		     ;
	    curline += idx - start_idx + extra;
	    જો (curline > buf->b_ml.ml_લીટી_ગણતરી)
		પાછા -1;	// exactly one byte beyond the end
	    પાછા curline;
	}
	curline = buf->b_ml.ml_locked_ઉપલું + 1;
    }

    જો (lnum != 0)
    {
	// Count extra CR characters.
	જો (ffdos)
	    size += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	જો ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_લીટી_ગણતરી)
	    size -= ffdos + 1;
    }

    પાછા size;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    શૂન્ય
goto_byte(long cnt)
{
    long	boff = cnt;
    linenr_T	lnum;

    ml_flush_લીટી(curbuf);	// cached line may be dirty
    setpcmark();
    જો (boff)
	--boff;
    lnum = ml_find_લીટી_or_offset(curbuf, (linenr_T)0, &boff);
    જો (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_લીટી_ગણતરી;
	curwin->w_curswant = MAXCOL;
	coladvance((colnr_T)MAXCOL);
    }
    નહીંતર
    {
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = (colnr_T)boff;
	curwin->w_cursor.coladd = 0;
	curwin->w_set_curswant = true;
    }
    check_cursor();

    // Make sure the cursor is on the first byte of a multi-byte char.
    જો (has_mbyte)
	mb_adjust_cursor();
}
#endif
