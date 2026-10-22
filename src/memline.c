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
#define CHECK(c, s)	कुरु { /**/ } यावत् (0)

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

#अन्तर्भाव "vim.h"

#अन्तर्भाव <time.h>

#यदि defined(SASC) || defined(__amigaos4__)
# include <proto/dos.h>	    // for Open() and Close()
#endif

प्रकारनिर्देश संरचना block0		ZERO_BL;    // contents of the first block
प्रकारनिर्देश संरचना pointer_block	PTR_BL;	    // contents of a pointer block
प्रकारनिर्देश संरचना data_block	DATA_BL;    // contents of a data block
प्रकारनिर्देश संरचना pointer_entry	PTR_EN;	    // block/line-count pair

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

#यदि defined(FEAT_CRYPT)
स्थिर ध्रुव पूर्णाङ्क id1_codes[] = {
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
संरचना pointer_entry
{
    blocknr_T	pe_bnum;	// block number
    linenr_T	pe_पंक्ति_गिनती;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    पूर्णाङ्क		pe_page_गिनती;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
संरचना pointer_block
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_गिनती;	// number of pointers in this block
    short_u	pb_गिनती_max;	// maximum value for pb_count
    PTR_EN	pb_pointer[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#define PB_गिनती_MAX(mfp) (short_u)(((mfp)->mf_page_size - offsetof(PTR_BL, pb_pointer)) / परिमाण(PTR_EN))

/*
 * A data block is a leaf in the tree.
 *
 * The text of the lines is at the end of the block. The text of the first line
 * in the block is put at the end, the text of the second line in front of it,
 * etc. Thus the order of the lines is the opposite of the line number.
 */
संरचना data_block
{
    short_u	db_id;		// ID for data block: DATA_ID
    unsigned	db_free;	// free space available
    unsigned	db_txt_start;	// byte where text starts
    unsigned	db_txt_end;	// byte just after data block
    linenr_T	db_पंक्ति_गिनती;	// number of lines in this block
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
#define DB_MARKED	((unsigned)1 << ((परिमाण(unsigned) * 8) - 1))
#define DB_INDEX_MASK	(~DB_MARKED)

#define INDEX_SIZE  (परिमाण(unsigned))	    // size of one db_index entry
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
संरचना block0
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
    पूर्णाङ्क		b0_magic_int;	// check for byte order of int
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

#define ढेर_INCR	5	// nr of entries added to ml_stack at a time

/*
 * The line number where the first mark may be is remembered.
 * If it is 0 there are no marks at all.
 * (always used for the current buffer only, no buffer change possible while
 * executing a global command).
 */
स्थिर linenr_T	lowest_marked = 0;

/*
 * arguments for ml_find_line()
 */
#define ML_DELETE	0x11	    // delete line
#define ML_जोड़ो	0x12	    // insert line
#define ML_FIND		0x13	    // just find the line
#define ML_FLUSH	0x02	    // flush locked block
#define ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
प्रकारनिर्देश गणना {
      UB_FNAME = 0	// update timestamp and filename
    , UB_SAME_DIR       // update the B0_SAME_DIR flag
    , UB_CRYPT		// update crypt key
} upd_block0_T;

#ifdef FEAT_CRYPT
स्थिर शून्य ml_set_b0_crypt(buf_T *buf, ZERO_BL *b0p);
#endif
स्थिर शून्य ml_upd_block0(buf_T *buf, upd_block0_T what);
स्थिर शून्य set_b0_fname(ZERO_BL *, buf_T *buf);
स्थिर शून्य set_b0_dir_flag(ZERO_BL *b0p, buf_T *buf);
स्थिर शून्य add_b0_fenc(ZERO_BL *b0p, buf_T *buf);
स्थिर time_t swapfile_info(char_u *);
स्थिर पूर्णाङ्क recov_file_names(char_u **, char_u *, पूर्णाङ्क prepend_dot);
स्थिर char_u *findswapname(buf_T *, char_u **, char_u *);
स्थिर शून्य ml_flush_पंक्ति(buf_T *);
स्थिर bhdr_T *ml_new_data(memfile_T *, पूर्णाङ्क, पूर्णाङ्क);
स्थिर bhdr_T *ml_new_ptr(memfile_T *);
स्थिर bhdr_T *ml_find_पंक्ति(buf_T *, linenr_T, पूर्णाङ्क);
स्थिर पूर्णाङ्क ml_add_ढेर(buf_T *);
स्थिर शून्य ml_lineadd(buf_T *, पूर्णाङ्क);
स्थिर पूर्णाङ्क b0_magic_wrong(ZERO_BL *);
#ifdef CHECK_INODE
स्थिर पूर्णाङ्क fnamecmp_ino(char_u *, char_u *, long);
#endif
स्थिर शून्य long_to_char(long, char_u *);
स्थिर long char_to_long(char_u *);
#ifdef FEAT_CRYPT
स्थिर cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T offset, पूर्णाङ्क reading);
#endif
#ifdef FEAT_BYTEOFF
स्थिर शून्य ml_updatechunk(buf_T *buf, long पंक्ति, long len, पूर्णाङ्क updtype);
#endif

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
ml_open(buf_T *buf)
{
    memfile_T	*mfp;
    bhdr_T	*hp = रिक्त;
    ZERO_BL	*b0p;
    PTR_BL	*pp;
    DATA_BL	*dp;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ढेर_size = 0; // no stack yet
    buf->b_ml.ml_ढेर = रिक्त;	// no stack yet
    buf->b_ml.ml_ढेर_शीर्ष = 0;	// nothing in the stack
    buf->b_ml.ml_locked = रिक्त;	// no cached block
    buf->b_ml.ml_पंक्ति_lnum = 0;	// no cached line
#ifdef FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = रिक्त;
    buf->b_ml.ml_usedchunks = 0;
#endif

    यदि (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    यदि (p_uc && buf->b_p_swf)
	buf->b_may_swap = true;
    अन्यथा
	buf->b_may_swap = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(रिक्त, 0);
    यदि (mfp == रिक्त)
	गच्छ error;

    buf->b_ml.ml_mfp = mfp;
#ifdef FEAT_CRYPT
    mfp->mf_buffer = buf;
#endif
    buf->b_ml.ml_flags = ML_EMPTY;
    buf->b_ml.ml_पंक्ति_गिनती = 1;

/*
 * fill block0 struct and write page 0
 */
    यदि ((hp = mf_new(mfp, FALSE, 1)) == रिक्त)
	गच्छ error;
    यदि (hp->bh_bnum != 0)
    {
	iemsg(e_didnt_get_block_nr_zero);
	गच्छ error;
    }
    b0p = (ZERO_BL *)(hp->bh_data);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (long)B0_MAGIC_LONG;
    b0p->b0_magic_int = (पूर्णाङ्क)B0_MAGIC_INT;
    b0p->b0_magic_short = (short)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_version, "VIM ", 4);
    STRNCPY(b0p->b0_version + 4, Version, 6);
    long_to_char((long)mfp->mf_page_size, b0p->b0_page_size);

#ifdef FEAT_SPELL
    यदि (!buf->b_spell)
#endif
    {
	b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	b0p->b0_flags = get_fileformat(buf) + 1;
	set_b0_fname(b0p, buf);
	(शून्य)get_user_name(b0p->b0_uname, B0_UNAME_SIZE);
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
    यदि (!buf->b_help && !B_SPELL(buf))
	(शून्य)mf_sync(mfp, 0);

    /*
     * Fill in root pointer block and write page 1.
     */
    यदि ((hp = ml_new_ptr(mfp)) == रिक्त)
	गच्छ error;
    यदि (hp->bh_bnum != 1)
    {
	iemsg(e_didnt_get_block_nr_one);
	गच्छ error;
    }
    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_गिनती = 1;
    pp->pb_pointer[0].pe_bnum = 2;
    pp->pb_pointer[0].pe_page_गिनती = 1;
    pp->pb_pointer[0].pe_old_lnum = 1;
    pp->pb_pointer[0].pe_पंक्ति_गिनती = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    यदि ((hp = ml_new_data(mfp, FALSE, 1)) == रिक्त)
	गच्छ error;
    यदि (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_block_nr_two);
	गच्छ error;
    }

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[0] = --dp->db_txt_start;	// at end of block
    dp->db_free -= 1 + INDEX_SIZE;
    dp->db_पंक्ति_गिनती = 1;
    *((char_u *)dp + dp->db_txt_start) = NUL;	// empty line

    प्रत्यागम OK;

error:
    यदि (mfp != रिक्त)
    {
	यदि (hp)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, TRUE);	    // will also free(mfp->mf_fname)
    }
    buf->b_ml.ml_mfp = रिक्त;
    प्रत्यागम FAIL;
}

#यदि defined(FEAT_CRYPT)
/*
 * Swapfile encryption is not supported by XChaCha20.  If this crypt method is
 * used then disable the swapfile, to avoid plain text being written to disk,
 * and return TRUE.
 * Otherwise return FALSE.
 */
    स्थिर पूर्णाङ्क
crypt_may_close_swapfile(buf_T *buf, char_u *कुंजी, पूर्णाङ्क method)
{
    यदि (crypt_method_is_sodium(method) && *कुंजी != NUL)
    {
	mf_close_file(buf, TRUE);
	buf->b_p_swf = FALSE;
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Prepare encryption for "buf" for the current key and method.
 */
    स्थिर शून्य
ml_set_mfp_crypt(buf_T *buf)
{
    यदि (*buf->b_p_कुंजी == NUL)
	प्रत्यागम;

    पूर्णाङ्क method_nr = crypt_get_method_nr(buf);

    यदि (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_seed(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN, रिक्त, 0);
    }
# ifdef FEAT_SODIUM
    अन्यथा यदि (crypt_method_is_sodium(method_nr))
	crypt_sodium_randombytes_buf(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN);
# endif
}

/*
 * Prepare encryption for "buf" with block 0 "b0p".
 * Note: should not be called with libsodium encryption, since xchacha20 does
 * not support swapfile encryption.
 */
    स्थिर शून्य
ml_set_b0_crypt(buf_T *buf, ZERO_BL *b0p)
{
    यदि (*buf->b_p_कुंजी == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    अन्यथा
    {
	पूर्णाङ्क method_nr = crypt_get_method_nr(buf);

	b0p->b0_id[1] = id1_codes[method_nr];
	यदि (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
	{
	    // Generate a seed and store it in block 0 and in the memfile.
	    sha2_seed(&b0p->b0_seed, MF_SEED_LEN, रिक्त, 0);
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
    शून्य
ml_set_crypt_कुंजी(
    buf_T	*buf,
    char_u	*old_कुंजी,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    पूर्णाङ्क		page_गिनती;
    पूर्णाङ्क		idx;
    long	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    DATA_BL	*dp;
    blocknr_T	bnum;
    पूर्णाङ्क		शीर्ष;
    पूर्णाङ्क		old_method;

    यदि (mfp == रिक्त || mfp->mf_fd < 0)
	प्रत्यागम;  // no memfile yet, nothing to do
    old_method = crypt_method_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    यदि (crypt_may_close_swapfile(buf, buf->b_p_कुंजी, crypt_get_method_nr(buf)))
	प्रत्यागम;
# endif

    // First make sure the swapfile is in a consistent state, using the old
    // key and method.
    {
	char_u *new_कुंजी = buf->b_p_कुंजी;
	char_u *new_buf_cm = buf->b_p_cm;

	buf->b_p_कुंजी = old_कुंजी;
	buf->b_p_cm = old_cm;
	ml_preserve(buf, FALSE);
	buf->b_p_कुंजी = new_कुंजी;
	buf->b_p_cm = new_buf_cm;
    }

    // Set the key, method and seed to be used for reading, these must be the
    // old values.
    mfp->mf_old_कुंजी = old_कुंजी;
    mfp->mf_old_cm = old_method;
    यदि (old_method > 0 && *old_कुंजी != NUL)
	mch_memmove(mfp->mf_old_seed, mfp->mf_seed, MF_SEED_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    यदि (mfp->mf_infile_गिनती > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_पंक्ति(buf);		    // flush buffered line
	(शून्य)ml_find_पंक्ति(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = रिक्त;
	bnum = 1;		// start with block 1
	page_गिनती = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_ढेर_शीर्ष = 0;
	VIM_CLEAR(buf->b_ml.ml_ढेर);
	buf->b_ml.ml_ढेर_size = 0;	// no stack yet

	प्रति ( ; !got_int; पंक्ति_breakcheck())
	{
	    यदि (hp != रिक्त)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    यदि ((hp = mf_get(mfp, bnum, page_गिनती)) == रिक्त)
	    {
		यदि (bnum == 1)
		    भङ्ग;
		++error;
	    }
	    अन्यथा
	    {
		pp = (PTR_BL *)(hp->bh_data);
		यदि (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    यदि (pp->pb_गिनती == 0)
		    {
			// empty block?
			++error;
		    }
		    अन्यथा यदि (idx < (पूर्णाङ्क)pp->pb_गिनती)	// go a block deeper
		    {
			यदि (pp->pb_pointer[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    अनुवर्त;
			}

			// going one block deeper in the tree, new entry in
			//ढेर
			यदि ((शीर्ष = ml_add_ढेर(buf)) < 0)
			{
			    ++error;
			    भङ्ग;		    // out of memory
			}
			ip = &(buf->b_ml.ml_ढेर[शीर्ष]);
			ip->ip_bnum = bnum;
			ip->ip_index = idx;

			bnum = pp->pb_pointer[idx].pe_bnum;
			page_गिनती = pp->pb_pointer[idx].pe_page_गिनती;
			idx = 0;
			अनुवर्त;
		    }
		}
		अन्यथा	    // not a pointer block
		{
		    dp = (DATA_BL *)(hp->bh_data);
		    यदि (dp->db_id != DATA_ID)	// block id wrong
			++error;
		    अन्यथा
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = रिक्त;
		    }
		}
	    }

	    यदि (buf->b_ml.ml_ढेर_शीर्ष == 0)	// finished
		भङ्ग;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_ढेर[--(buf->b_ml.ml_ढेर_शीर्ष)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_index + 1;	    // go to next index
	    page_गिनती = 1;
	}
	यदि (hp != रिक्त)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	यदि (error > 0)
	    emsg(_(e_error_while_updating_swap_file_crypt));
    }

    mfp->mf_old_कुंजी = रिक्त;
}
#endif

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
    शून्य
ml_setname(buf_T *buf)
{
    पूर्णाङ्क		success = FALSE;
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;
#यदि defined(MSWIN)
    char_u	*p;
#endif

    mfp = buf->b_ml.ml_mfp;
    यदि (mfp->mf_fd < 0)		    // there is no swap file yet
    {
	/*
	 * When 'updatecount' is 0 and 'noswapfile' there is no swap file.
	 * For help files we will make a swap file now.
	 */
	यदि (p_uc != 0 && (cmdmod.cmod_flags & CMOD_NOSWAPFILE) == 0)
	    ml_open_file(buf);	    // create a swap file
	प्रत्यागम;
    }

    /*
     * Try all directories in the 'directory' option.
     */
    dirp = p_dir;
    प्रति (;;)
    {
	यदि (*dirp == NUL)	    // tried all directories, fail
	    भङ्ग;
	fname = findswapname(buf, &dirp, mfp->mf_fname);
	// autocmd may have freed mfp if findswapname creates
	// different swapfile name
	यदि (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    प्रत्यागम;
	}
						    // alloc's fname
	यदि (dirp == रिक्त)	    // out of memory
	    भङ्ग;
	यदि (fname == रिक्त)	    // no file name found for this dir
	    अनुवर्त;

#यदि defined(MSWIN)
	/*
	 * Set full pathname for swap file now, because a ":!cd dir" may
	 * change directory without us knowing it.
	 */
	p = FullName_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	यदि (fname == रिक्त)
	    अनुवर्त;
#endif
	// if the file name is the same we don't have to do anything
	यदि (fnamecmp(fname, mfp->mf_fname) == 0)
	{
	    vim_free(fname);
	    success = TRUE;
	    भङ्ग;
	}
	// need to close the swap file before renaming
	यदि (mfp->mf_fd >= 0)
	{
	    close(mfp->mf_fd);
	    mfp->mf_fd = -1;
	}

	// try to rename the swap file
	यदि (vim_rename(mfp->mf_fname, fname) == 0)
	{
	    success = TRUE;
	    vim_free(mfp->mf_fname);
	    mfp->mf_fname = fname;
	    vim_free(mfp->mf_ffname);
#यदि defined(MSWIN)
	    mfp->mf_ffname = रिक्त;  // mf_fname is full pathname already
#अन्यथा
	    mf_set_ffname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);
	    भङ्ग;
	}
	vim_free(fname);	    // this fname didn't work, try another
    }

    यदि (mfp->mf_fd == -1)	    // need to (re)open the swap file
    {
	mfp->mf_fd = mch_open((वर्ण *)mfp->mf_fname, O_RDWR | O_EXTRA, 0);
	यदि (mfp->mf_fd < 0)
	{
	    // could not (re)open the swap file, what can we do????
	    emsg(_(e_oops_lost_the_swap_file));
	    प्रत्यागम;
	}
#ifdef HAVE_FD_CLOEXEC
	{
	    पूर्णाङ्क fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    यदि (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(शून्य)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#endif
    }
    यदि (!success)
	emsg(_(e_could_not_rename_swap_file));
}

/*
 * Open a file for the memfile for all buffers that are not readonly or have
 * been modified.
 * Used when 'updatecount' changes from zero to non-zero.
 */
    शून्य
ml_open_files(शून्य)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	यदि (!buf->b_p_ro || buf->b_changed)
	    ml_open_file(buf);
}

/*
 * Open a swap file for an existing memfile, if there is no swap file yet.
 * If we are unable to find a file name, mf_fname will be NULL
 * and the memfile will be in memory only (no recovery possible).
 */
    शून्य
ml_open_file(buf_T *buf)
{
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;

    mfp = buf->b_ml.ml_mfp;
    यदि (mfp == रिक्त || mfp->mf_fd >= 0 || !buf->b_p_swf
				      || (cmdmod.cmod_flags & CMOD_NOSWAPFILE))
	प्रत्यागम;		// nothing to do

#ifdef FEAT_SPELL
    // For a spell buffer use a temp file name.
    यदि (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	यदि (fname != रिक्त)
	    (शून्य)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_swap = false;
	प्रत्यागम;
    }
#endif

    /*
     * Try all directories in 'directory' option.
     */
    dirp = p_dir;
    प्रति (;;)
    {
	यदि (*dirp == NUL)
	    भङ्ग;
	// There is a small chance that between choosing the swap file name
	// and creating it, another Vim creates the file.  In that case the
	// creation will fail and we will use another directory.
	fname = findswapname(buf, &dirp, रिक्त); // allocates fname
	// autocmd may have freed mfp, grr!
	यदि (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    प्रत्यागम;
	}
	यदि (dirp == रिक्त)
	    भङ्ग;  // out of memory
	यदि (fname == रिक्त)
	    अनुवर्त;
	यदि (mf_open_file(mfp, fname) == OK)	// consumes fname!
	{
	    // don't sync yet in ml_sync_all()
	    mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;
#यदि defined(MSWIN)
	    /*
	     * set full pathname for swap file now, because a ":!cd dir" may
	     * change directory without us knowing it.
	     */
	    mf_fullname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    यदि (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_set_dirty(mfp);
		भङ्ग;
	    }
	    // Writing block 0 failed: close the file and try another dir
	    mf_close_file(buf, FALSE);
	}
    }

    यदि (*p_dir != NUL && mfp->mf_fname == रिक्त)
    {
	need_wait_return = TRUE;	// call wait_return() later
	++no_wait_return;
	(शून्य)semsg(_(e_unable_to_open_swap_file_for_str_recovery_impossible),
		    buf_spname(buf) != रिक्त ? buf_spname(buf) : buf->b_fname);
	--no_wait_return;
    }

    // don't try to open a swap file again
    buf->b_may_swap = false;
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
    शून्य
check_need_swap(
    पूर्णाङ्क	    newfile)		// reading file into new buffer
{
    पूर्णाङ्क old_संदेश_silent = संदेश_silent; // might be reset by an E325 message

    यदि (curbuf->b_may_swap && (!curbuf->b_p_ro || !newfile))
	ml_open_file(curbuf);
    संदेश_silent = old_संदेश_silent;
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
    शून्य
ml_close(buf_T *buf, पूर्णाङ्क del_file)
{
    यदि (buf->b_ml.ml_mfp == रिक्त)		// not open
	प्रत्यागम;
    mf_close(buf->b_ml.ml_mfp, del_file);	// close the .swp file
    यदि (buf->b_ml.ml_पंक्ति_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_पंक्ति_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_पंक्ति_ptr);
    vim_free(buf->b_ml.ml_ढेर);
#ifdef FEAT_BYTEOFF
    VIM_CLEAR(buf->b_ml.ml_chunksize);
#endif
    buf->b_ml.ml_mfp = रिक्त;

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
    शून्य
ml_close_all(पूर्णाङ्क del_file)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ml_close(buf, del_file && ((buf->b_flags & BF_PRESERVED) == 0
				 || vim_strchr(p_cpo, CPO_PRESERVE) == रिक्त));
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
    शून्य
ml_close_notmod(शून्य)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	यदि (!bufIsChanged(buf))
	    ml_close(buf, TRUE);    // close all not-modified buffers
}

/*
 * Update the timestamp in the .swp file.
 * Used when the file has been written.
 */
    शून्य
ml_timestamp(buf_T *buf)
{
    ml_upd_block0(buf, UB_FNAME);
}

/*
 * Return FAIL when the ID of "b0p" is wrong.
 */
    स्थिर पूर्णाङ्क
ml_check_b0_id(ZERO_BL *b0p)
{
    यदि (b0p->b0_id[0] != BLOCK0_ID0
	    || (b0p->b0_id[1] != BLOCK0_ID1
		&& b0p->b0_id[1] != BLOCK0_ID1_C0
		&& b0p->b0_id[1] != BLOCK0_ID1_C1
		&& b0p->b0_id[1] != BLOCK0_ID1_C2
		&& b0p->b0_id[1] != BLOCK0_ID1_C3
		&& b0p->b0_id[1] != BLOCK0_ID1_C4)
	    )
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Update the timestamp or the B0_SAME_DIR flag of the .swp file.
 */
    स्थिर शून्य
ml_upd_block0(buf_T *buf, upd_block0_T what)
{
    memfile_T	*mfp;
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    mfp = buf->b_ml.ml_mfp;
    यदि (mfp == रिक्त)
	प्रत्यागम;
    hp = mf_get(mfp, (blocknr_T)0, 1);
    यदि (hp == रिक्त)
    {
#ifdef FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	यदि (what == UB_CRYPT)
	    ml_set_mfp_crypt(buf);
#endif
	प्रत्यागम;
    }

    b0p = (ZERO_BL *)(hp->bh_data);
    यदि (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_block_zero);
    अन्यथा
    {
	यदि (what == UB_FNAME)
	    set_b0_fname(b0p, buf);
#ifdef FEAT_CRYPT
	अन्यथा यदि (what == UB_CRYPT)
	    ml_set_b0_crypt(buf, b0p);
#endif
	अन्यथा // what == UB_SAME_DIR
	    set_b0_dir_flag(b0p, buf);
    }
    mf_put(mfp, hp, TRUE, FALSE);
}

/*
 * Write file name and timestamp into block 0 of a swap file.
 * Also set buf->b_mtime.
 * Don't use NameBuff[]!!!
 */
    स्थिर शून्य
set_b0_fname(ZERO_BL *b0p, buf_T *buf)
{
    stat_T	st;

    यदि (buf->b_ffname == रिक्त)
	b0p->b0_fname[0] = NUL;
    अन्यथा
    {
#यदि defined(MSWIN) || defined(AMIGA)
	// Systems that cannot translate "~user" back into a path: copy the
	// file name unmodified.  Do use slashes instead of backslashes for
	// portability.
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_SIZE_CRYPT - 1);
# ifdef BACKSLASH_IN_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#अन्यथा
	size_t	flen;
	char_u	uname[B0_UNAME_SIZE];

	/*
	 * For a file under the home directory of the current user, we try to
	 * replace the home directory path with "~user". This helps when
	 * editing the same file on different machines over a network.
	 * First replace home dir path with "~/" with home_replace().
	 * Then insert the user name to get "~user/".
	 */
	flen = home_replace(रिक्त, buf->b_ffname, b0p->b0_fname,
						   B0_FNAME_SIZE_CRYPT, TRUE);
	यदि (b0p->b0_fname[0] == '~')
	{
	    size_t  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    यदि (get_user_name(uname, B0_UNAME_SIZE) == FAIL
		   || (ulen = STRLEN(uname)) + flen > B0_FNAME_SIZE_CRYPT - 1)
		vim_strncpy(b0p->b0_fname, buf->b_ffname,
						     B0_FNAME_SIZE_CRYPT - 1);
	    अन्यथा
	    {
		mch_memmove(b0p->b0_fname + ulen + 1, b0p->b0_fname + 1, flen);
		mch_memmove(b0p->b0_fname + 1, uname, ulen);
	    }
	}
#endif
	यदि (mch_stat((वर्ण *)buf->b_ffname, &st) >= 0)
	{
	    long_to_char((long)st.st_mtime, b0p->b0_mtime);
#ifdef CHECK_INODE
	    long_to_char((long)st.st_ino, b0p->b0_ino);
#endif
	    buf_store_time(buf, &st, buf->b_ffname);
	    buf->b_mtime_read = buf->b_mtime;
	    buf->b_mtime_read_ns = buf->b_mtime_ns;
	}
	अन्यथा
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
    स्थिर शून्य
set_b0_dir_flag(ZERO_BL *b0p, buf_T *buf)
{
    यदि (same_directory(buf->b_ml.ml_mfp->mf_fname, buf->b_ffname))
	b0p->b0_flags |= B0_SAME_DIR;
    अन्यथा
	b0p->b0_flags &= ~B0_SAME_DIR;
}

/*
 * When there is room, add the 'fileencoding' to block zero.
 */
    स्थिर शून्य
add_b0_fenc(
    ZERO_BL	*b0p,
    buf_T	*buf)
{
    पूर्णाङ्क		n;
    पूर्णाङ्क		size = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    यदि (*buf->b_p_कुंजी != NUL)
	size = B0_FNAME_SIZE_CRYPT;
#endif

    n = (पूर्णाङ्क)STRLEN(buf->b_p_fenc);
    यदि ((पूर्णाङ्क)STRLEN(b0p->b0_fname) + n + 1 > size)
	b0p->b0_flags &= ~B0_HAS_FENC;
    अन्यथा
    {
	mch_memmove((वर्ण *)b0p->b0_fname + size - n,
					    (वर्ण *)buf->b_p_fenc, (size_t)n);
	*(b0p->b0_fname + size - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#यदि defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#endif

#यदि defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    स्थिर पूर्णाङ्क
swapfile_process_running(ZERO_BL *b0p, char_u *swap_fname UNUSED)
{
# यदि defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    संरचना sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    यदि (mch_stat((वर्ण *)swap_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < time(रिक्त) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	प्रत्यागम FALSE;
# endif
    प्रत्यागम mch_process_running(char_to_long(b0p->b0_pid));
}
#endif

/*
 * Try to recover curbuf from the .swp file.
 * If "checkext" is TRUE, check the extension and detect whether it is
 * a swap file.
 */
    शून्य
ml_recover(पूर्णाङ्क checkext)
{
    buf_T	*buf = रिक्त;
    memfile_T	*mfp = रिक्त;
    char_u	*fname;
    char_u	*fname_used = रिक्त;
    bhdr_T	*hp = रिक्त;
    ZERO_BL	*b0p;
    पूर्णाङ्क		b0_ff;
    char_u	*b0_fenc = रिक्त;
#ifdef FEAT_CRYPT
    पूर्णाङ्क		b0_cm = -1;
#endif
    PTR_BL	*pp;
    DATA_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    पूर्णाङ्क		page_गिनती;
    stat_T	org_stat, swp_stat;
    पूर्णाङ्क		len;
    पूर्णाङ्क		directly;
    linenr_T	lnum;
    char_u	*p;
    पूर्णाङ्क		i;
    long	error;
    पूर्णाङ्क		cannot_open;
    linenr_T	पंक्ति_गिनती;
    पूर्णाङ्क		has_error;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		शीर्ष;
    पूर्णाङ्क		txt_start;
    off_T	size;
    पूर्णाङ्क		called_from_main;
    पूर्णाङ्क		serious_error = TRUE;
    long	mtime;
    पूर्णाङ्क		attr;
    पूर्णाङ्क		orig_file_status = NOTDONE;

    recoverymode = TRUE;
    called_from_main = (curbuf->b_ml.ml_mfp == रिक्त);
    attr = HL_ATTR(HLF_E);

    /*
     * If the file name ends in ".s[a-w][a-z]" we assume this is the swap file.
     * Otherwise a search is done to find the swap file(s).
     */
    fname = curbuf->b_fname;
    यदि (fname == रिक्त)		    // When there is no file name
	fname = (char_u *)"";
    len = (पूर्णाङ्क)STRLEN(fname);
    यदि (checkext && len >= 4 &&
#यदि defined(VMS)
	    STRNICMP(fname + len - 4, "_s", 2)
#अन्यथा
	    STRNICMP(fname + len - 4, ".s", 2)
#endif
						== 0
		&& vim_strchr((char_u *)"abcdefghijklmnopqrstuvw",
					   TOLOWER_ASC(fname[len - 2])) != रिक्त
		&& ASCII_ISALPHA(fname[len - 1]))
    {
	directly = TRUE;
	fname_used = vim_strsave(fname); // make a copy for mf_open()
    }
    अन्यथा
    {
	directly = FALSE;

	// count the number of matching swap files
	len = recover_names(fname, FALSE, रिक्त, 0, रिक्त);
	यदि (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_swap_file_found_for_str), fname);
	    गच्छ theend;
	}
	यदि (len == 1)		    // one swap file found, use it
	    i = 1;
	अन्यथा			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (शून्य)recover_names(fname, TRUE, रिक्त, 0, रिक्त);
	    संदेश_putchar('\n');
	    संदेश_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_संख्या(FALSE, रिक्त);
	    यदि (i < 1 || i > len)
		गच्छ theend;
	}
	// get the swap file name that will be used
	(शून्य)recover_names(fname, FALSE, रिक्त, i, &fname_used);
    }
    यदि (fname_used == रिक्त)
	गच्छ theend;			// out of memory

    // When called from main() still need to initialize storage structure
    यदि (called_from_main && ml_open(curbuf) == FAIL)
	getout(1);

    /*
     * Allocate a buffer structure for the swap file that is used for recovery.
     * Only the memline and crypt information in it are really used.
     */
    buf = ALLOC_ONE(buf_T);
    यदि (buf == रिक्त)
	गच्छ theend;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ढेर_size = 0;	// no stack yet
    buf->b_ml.ml_ढेर = रिक्त;		// no stack yet
    buf->b_ml.ml_ढेर_शीर्ष = 0;		// nothing in the stack
    buf->b_ml.ml_पंक्ति_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = रिक्त;		// no locked block
    buf->b_ml.ml_flags = 0;
#ifdef FEAT_CRYPT
    buf->b_p_कुंजी = empty_option;
    buf->b_p_cm = empty_option;
#endif

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_used); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_used, O_RDONLY);
    fname_used = p;
    यदि (mfp == रिक्त || mfp->mf_fd < 0)
    {
	यदि (fname_used != रिक्त)
	    semsg(_(e_cannot_open_str), fname_used);
	गच्छ theend;
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
    यदि ((hp = mf_get(mfp, (blocknr_T)0, 1)) == रिक्त)
    {
	संदेश_start();
	संदेश_puts_attr(_("Unable to read block 0 from "), attr | संदेश_HIST);
	संदेश_outtrans_attr(mfp->mf_fname, attr | संदेश_HIST);
	संदेश_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | संदेश_HIST);
	संदेश_end();
	गच्छ theend;
    }
    b0p = (ZERO_BL *)(hp->bh_data);
    यदि (STRNCMP(b0p->b0_version, "VIM 3.0", 7) == 0)
    {
	संदेश_start();
	संदेश_outtrans_attr(mfp->mf_fname, संदेश_HIST);
	संदेश_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    संदेश_HIST);
	संदेश_puts_attr(_("Use Vim version 3.0.\n"), संदेश_HIST);
	संदेश_end();
	गच्छ theend;
    }
    यदि (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_swap_file), mfp->mf_fname);
	गच्छ theend;
    }
    यदि (b0_magic_wrong(b0p))
    {
	संदेश_start();
	संदेश_outtrans_attr(mfp->mf_fname, attr | संदेश_HIST);
#यदि defined(MSWIN)
	यदि (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    संदेश_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | संदेश_HIST);
	अन्यथा
#endif
	    संदेश_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | संदेश_HIST);
	संदेश_puts_attr(_("The file was created on "), attr | संदेश_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	संदेश_puts_attr((वर्ण *)b0p->b0_hname, attr | संदेश_HIST);
	संदेश_puts_attr(_(",\nor the file has been damaged."), attr | संदेश_HIST);
	संदेश_end();
	गच्छ theend;
    }

#ifdef FEAT_CRYPT
    प्रति (i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(id1_codes); ++i)
	यदि (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    यदि (b0_cm > 0)
	mch_memmove(mfp->mf_seed, &b0p->b0_seed, MF_SEED_LEN);
    crypt_set_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#अन्यथा
    यदि (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_version_of_vim_does_not_support_encryption), mfp->mf_fname);
	गच्छ theend;
    }
#endif

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    यदि (mfp->mf_page_size != (unsigned)char_to_long(b0p->b0_page_size))
    {
	unsigned previous_page_size = mfp->mf_page_size;

	mf_new_page_size(mfp, (unsigned)char_to_long(b0p->b0_page_size));
	यदि (mfp->mf_page_size < previous_page_size)
	{
	    संदेश_start();
	    संदेश_outtrans_attr(mfp->mf_fname, attr | संदेश_HIST);
	    संदेश_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | संदेश_HIST);
	    संदेश_end();
	    गच्छ theend;
	}
	यदि ((size = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	अन्यथा
	    mfp->mf_blocknr_max = (blocknr_T)(size / mfp->mf_page_size);
	mfp->mf_infile_गिनती = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_page_size);
	यदि (p == रिक्त)
	    गच्छ theend;
	mch_memmove(p, hp->bh_data, previous_page_size);
	vim_free(hp->bh_data);
	hp->bh_data = p;
	b0p = (ZERO_BL *)(hp->bh_data);
    }

    /*
     * If .swp file name given directly, use name from swap file for buffer.
     */
    यदि (directly)
    {
	expand_env(b0p->b0_fname, NameBuff, MAXPATHL);
	यदि (setfname(curbuf, NameBuff, रिक्त, TRUE) == FAIL)
	    गच्छ theend;
    }

    home_replace(रिक्त, mfp->mf_fname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Using swap file \"%s\""), NameBuff);

    यदि (buf_spname(curbuf) != रिक्त)
	vim_strncpy(NameBuff, buf_spname(curbuf), MAXPATHL - 1);
    अन्यथा
	home_replace(रिक्त, curbuf->b_ffname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Original file \"%s\""), NameBuff);
    संदेश_putchar('\n');

    /*
     * check date of swap file and original file
     */
    mtime = char_to_long(b0p->b0_mtime);
    यदि (curbuf->b_ffname != रिक्त
	    && mch_stat((वर्ण *)curbuf->b_ffname, &org_stat) != -1
	    && ((mch_stat((वर्ण *)mfp->mf_fname, &swp_stat) != -1
		    && org_stat.st_mtime > swp_stat.st_mtime)
		|| org_stat.st_mtime != mtime))
	emsg(_(e_warning_original_file_may_have_been_changed));
    out_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_MASK);
    यदि (b0p->b0_flags & B0_HAS_FENC)
    {
	पूर्णाङ्क fnsize = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	यदि (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_SIZE_CRYPT;
#endif
	प्रति (p = b0p->b0_fname + fnsize; p > b0p->b0_fname && p[-1] != NUL; --p)
	    ;
	b0_fenc = vim_strnsave(p, b0p->b0_fname + fnsize - p);
    }

    mf_put(mfp, hp, FALSE, FALSE);	// release block 0
    hp = रिक्त;

    /*
     * Now that we are sure that the file is going to be recovered, clear the
     * contents of the current buffer.
     */
    यावत् (!(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    यदि (curbuf->b_ffname != रिक्त)
	orig_file_status = readfile(curbuf->b_ffname, रिक्त, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, रिक्त, READ_NEW);

#ifdef FEAT_CRYPT
    यदि (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	यदि (*curbuf->b_p_कुंजी != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_used);
	    संदेश_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    संदेश_puts(_("\nenter the new crypt key."));
	    संदेश_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    संदेश_puts(_("\nto use the same key for text file and swap file"));
	}
	अन्यथा
	    smsg(_(need_कुंजी_संदेश), fname_used);
	buf->b_p_कुंजी = crypt_get_कुंजी(FALSE, FALSE);
	यदि (buf->b_p_कुंजी == रिक्त)
	    buf->b_p_कुंजी = curbuf->b_p_कुंजी;
	अन्यथा यदि (*buf->b_p_कुंजी == NUL)
	{
	    vim_free(buf->b_p_कुंजी);
	    buf->b_p_कुंजी = curbuf->b_p_कुंजी;
	}
	यदि (buf->b_p_कुंजी == रिक्त)
	    buf->b_p_कुंजी = empty_option;
    }
#endif

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    यदि (b0_ff != 0)
	set_fileformat(b0_ff - 1, OPT_LOCAL);
    यदि (b0_fenc != रिक्त)
    {
	set_option_मूल्य_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    page_गिनती = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    पंक्ति_गिनती = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_ढेर_शीर्ष = 0;
    buf->b_ml.ml_ढेर = रिक्त;
    buf->b_ml.ml_ढेर_size = 0;	// no stack yet

    यदि (curbuf->b_ffname == रिक्त)
	cannot_open = TRUE;
    अन्यथा
	cannot_open = FALSE;

    serious_error = FALSE;
    प्रति ( ; !got_int; पंक्ति_breakcheck())
    {
	यदि (hp != रिक्त)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	यदि ((hp = mf_get(mfp, bnum, page_गिनती)) == रिक्त)
	{
	    यदि (bnum == 1)
	    {
		semsg(_(e_unable_to_read_block_one_from_str), mfp->mf_fname);
		गच्छ theend;
	    }
	    ++error;
	    ml_append(lnum++, (char_u *)_("???MANY LINES MISSING"),
							    (colnr_T)0, TRUE);
	}
	अन्यथा		// there is a block
	{
	    pp = (PTR_BL *)(hp->bh_data);
	    यदि (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		पूर्णाङ्क ptr_block_error = FALSE;
		यदि (pp->pb_गिनती_max != PB_गिनती_MAX(mfp))
		{
		    ptr_block_error = TRUE;
		    pp->pb_गिनती_max = PB_गिनती_MAX(mfp);
		}
		यदि (pp->pb_गिनती > pp->pb_गिनती_max)
		{
		    ptr_block_error = TRUE;
		    pp->pb_गिनती = pp->pb_गिनती_max;
		}
		यदि (ptr_block_error)
		    emsg(_(e_warning_pointer_block_corrupted));

		// check line count when using pointer block first time
		यदि (idx == 0 && पंक्ति_गिनती != 0)
		{
		    प्रति (i = 0; i < (पूर्णाङ्क)pp->pb_गिनती; ++i)
			पंक्ति_गिनती -= pp->pb_pointer[i].pe_पंक्ति_गिनती;
		    यदि (पंक्ति_गिनती != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		यदि (pp->pb_गिनती == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		अन्यथा यदि (idx < (पूर्णाङ्क)pp->pb_गिनती)	// go a block deeper
		{
		    यदि (pp->pb_pointer[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			यदि (!cannot_open)
			{
			    पंक्ति_गिनती = pp->pb_pointer[idx].pe_पंक्ति_गिनती;
			    linenr_T pe_old_lnum = pp->pb_pointer[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    यदि (पंक्ति_गिनती <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, रिक्त, lnum,
					pe_old_lnum - 1,
					पंक्ति_गिनती, रिक्त, 0) != OK)
				cannot_open = TRUE;
			    अन्यथा
				lnum += पंक्ति_गिनती;
			}
			यदि (cannot_open)
			{
			    ++error;
			    ml_append(lnum++, (char_u *)_("???LINES MISSING"),
							    (colnr_T)0, TRUE);
			}
			++idx;	    // get same block again for next index
			अनुवर्त;
		    }

		    /*
		     * going one block deeper in the tree
		     */
		    यदि ((शीर्ष = ml_add_ढेर(buf)) < 0)	// new entry in stack
		    {
			++error;
			भङ्ग;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_ढेर[शीर्ष]);
		    ip->ip_bnum = bnum;
		    ip->ip_index = idx;

		    bnum = pp->pb_pointer[idx].pe_bnum;
		    पंक्ति_गिनती = pp->pb_pointer[idx].pe_पंक्ति_गिनती;
		    page_गिनती = pp->pb_pointer[idx].pe_page_गिनती;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    यदि (page_गिनती < 1
			    || bnum + page_गिनती > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_index + 1;
			bnum = ip->ip_bnum;
			page_गिनती = 1;
			--buf->b_ml.ml_ढेर_शीर्ष;
			अनुवर्त;
		    }
		    idx = 0;
		    अनुवर्त;
		}
	    }
	    अन्यथा	    // not a pointer block
	    {
		dp = (DATA_BL *)(hp->bh_data);
		यदि (dp->db_id != DATA_ID)	// block id wrong
		{
		    यदि (bnum == 1)
		    {
			semsg(_(e_block_one_id_wrong_str_not_swp_file),
							       mfp->mf_fname);
			गच्छ theend;
		    }
		    ++error;
		    ml_append(lnum++, (char_u *)_("???BLOCK MISSING"),
							    (colnr_T)0, TRUE);
		}
		अन्यथा
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
		    यदि (hp->bh_page_गिनती != page_गिनती)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			page_गिनती = hp->bh_page_गिनती;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    यदि (page_गिनती * mfp->mf_page_size != dp->db_txt_end)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_end = page_गिनती * mfp->mf_page_size;
		    }

		    यदि (dp->db_txt_start < HEADER_SIZE
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
		    यदि (पंक्ति_गिनती != dp->db_पंक्ति_गिनती)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    पूर्णाङ्क did_questions = FALSE;
		    प्रति (i = 0; i < dp->db_पंक्ति_गिनती; ++i)
		    {
			यदि ((char_u *)&(dp->db_index[i])
					    >= (char_u *)dp + dp->db_txt_start)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    भङ्ग;
			}

			txt_start = (dp->db_index[i] & DB_INDEX_MASK);
			यदि (txt_start <= (पूर्णाङ्क)HEADER_SIZE
					  || txt_start >= (पूर्णाङ्क)dp->db_txt_end)
			{
			    ++error;
			    // avoid lots of lines with "???"
			    यदि (did_questions)
				अनुवर्त;
			    did_questions = TRUE;
			    p = (char_u *)"???";
			}
			अन्यथा
			{
			    did_questions = FALSE;
			    p = (char_u *)dp + txt_start;
			}
			ml_append(lnum++, p, (colnr_T)0, TRUE);
		    }
		    यदि (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	यदि (buf->b_ml.ml_ढेर_शीर्ष == 0)	// finished
	    भङ्ग;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_ढेर[--(buf->b_ml.ml_ढेर_शीर्ष)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_index + 1;	    // go to next index
	page_गिनती = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    यदि (orig_file_status != OK || curbuf->b_ml.ml_पंक्ति_गिनती != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	यदि (!(curbuf->b_ml.ml_पंक्ति_गिनती == 2 && *ml_get(1) == NUL))
	{
	    changed_internal();
	    ++CHANGEDTICK(curbuf);
	}
    }
    अन्यथा
    {
	प्रति (idx = 1; idx <= lnum; ++idx)
	{
	    // Need to copy one line, fetching the other one may flush it.
	    p = vim_strnsave(ml_get(idx), ml_get_len(idx));
	    i = STRCMP(p, ml_get(idx + lnum));
	    vim_free(p);
	    यदि (i != 0)
	    {
		changed_internal();
		++CHANGEDTICK(curbuf);
		भङ्ग;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    यावत् (curbuf->b_ml.ml_पंक्ति_गिनती > lnum
				       && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete(curbuf->b_ml.ml_पंक्ति_गिनती);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    यदि (got_int)
	emsg(_(e_recovery_interrupted));
    अन्यथा यदि (error)
    {
	++no_wait_return;
	संदेश(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_पंक्तियाँ_starting_with_questions));
	--no_wait_return;
	संदेश(_("See \":help E312\" for more information."));
	संदेश(">>>>>>>>>>>>>");
    }
    अन्यथा
    {
	यदि (curbuf->b_changed)
	{
	    संदेश(_("Recovery completed. You should check if everything is OK."));
	    संदेश_puts(_("\n(You might want to write out this file under another name\n"));
	    संदेश_puts(_("and run diff with the original file to check for changes)"));
	}
	अन्यथा
	    संदेश(_("Recovery completed. Buffer contents equals file contents."));
	संदेश_puts(_("\nYou may want to delete the .swp file now."));
#यदि defined(UNIX) || defined(MSWIN)
	यदि (swapfile_process_running(b0p, fname_used))
	{
	    // Warn there could be an active Vim on the same file, the user may
	    // want to kill it.
	    संदेश_puts(_("\nNote: process STILL RUNNING: "));
	    संदेश_outnum(char_to_long(b0p->b0_pid));
	}
#endif
	संदेश_puts("\n\n");
	cmdline_row = संदेश_row;
    }
#ifdef FEAT_CRYPT
    यदि (*buf->b_p_कुंजी != NUL && STRCMP(curbuf->b_p_कुंजी, buf->b_p_कुंजी) != 0)
    {
	संदेश_puts(_("Using crypt key from swap file for the text file.\n"));
	set_option_मूल्य_give_err((char_u *)"key", 0L, buf->b_p_कुंजी, OPT_LOCAL);
    }
#endif
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_used);
    recoverymode = FALSE;
    यदि (mfp != रिक्त)
    {
	यदि (hp != रिक्त)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    यदि (buf != रिक्त)
    {
#ifdef FEAT_CRYPT
	यदि (buf->b_p_कुंजी != curbuf->b_p_कुंजी)
	    free_string_option(buf->b_p_कुंजी);
	free_string_option(buf->b_p_cm);
#endif
	vim_free(buf->b_ml.ml_ढेर);
	vim_free(buf);
    }
    यदि (serious_error && called_from_main)
	ml_close(curbuf, TRUE);
    अन्यथा
    {
	apply_autocmds(EVENT_BUFREADPOST, रिक्त, curbuf->b_fname, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, रिक्त, curbuf->b_fname, FALSE, curbuf);
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
    पूर्णाङ्क
recover_names(
    char_u	*fname,		// base for swap file name
    पूर्णाङ्क		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list UNUSED, // when not NULL add file names to it
    पूर्णाङ्क		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_out)	// result when "nr" > 0
{
    पूर्णाङ्क		अंक_names;
    char_u	*(names[6]);
    char_u	*p;
    पूर्णाङ्क		अंक_files;
    पूर्णाङ्क		file_गिनती = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = रिक्त;
#ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#endif

    यदि (fname != रिक्त)
    {
#ifdef HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	यदि (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	अन्यथा
#endif
	    fname_res = fname;
    }

    यदि (do_list)
    {
	// use msg() to start the scrolling properly
	संदेश(_("Swap files found:"));
	संदेश_putchar('\n');
    }

    /*
     * Do the loop for every directory in 'directory'.
     * First allocate some memory to put the directory name in.
     */
    dir_name.string = alloc(STRLEN(p_dir) + 1);
    dirp = p_dir;
    यावत् (dir_name.string != रिक्त && *dirp)
    {
	/*
	 * Isolate a directory name from *dirp and put it in dir_name (we know
	 * it is large enough, so use 31000 for length).
	 * Advance dirp to next directory name.
	 */
	dir_name.length = (size_t)copy_option_part(&dirp, dir_name.string, 31000, ",");

	यदि (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    यदि (fname == रिक्त)
	    {
#ifdef VMS
		names[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#अन्यथा
		names[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#endif
#यदि defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		names[1] = vim_strnsave((char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"));
		names[2] = vim_strnsave((char_u *)".sw?", STRLEN_LITERAL(".sw?"));
		अंक_names = 3;
#अन्यथा
# ifdef VMS
		names[1] = vim_strnsave((char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"));
		अंक_names = 2;
# अन्यथा
		अंक_names = 1;
# endif
#endif
	    }
	    अन्यथा
		अंक_names = recov_file_names(names, fname_res, TRUE);
	}
	अन्यथा			    // check directory dir_name
	{
	    यदि (fname == रिक्त)
	    {
		string_T    ret;

#ifdef VMS
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#अन्यथा
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#endif
#यदि defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		names[1] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"), TRUE, &ret);
		names[2] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".sw?", STRLEN_LITERAL(".sw?"), TRUE, &ret);
		अंक_names = 3;
#अन्यथा
# ifdef VMS
		names[1] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"), TRUE, &ret);
		अंक_names = 2;
# अन्यथा
		अंक_names = 1;
# endif
#endif
	    }
	    अन्यथा
	    {
		char_u	*tail;

#यदि defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.length;
		यदि (after_pathsep(dir_name.string, p) && dir_name.length > 1 && p[-1] == p[-2])
		{
		    // Ends with '//', Use Full path for swap name
		    tail = make_percent_swname(dir_name.string, p, fname_res);
		}
		अन्यथा
#endif
		{
		    string_T	ret;

		    tail = gettail(fname_res);
		    tail = concat_fnames(dir_name.string, dir_name.length,
			tail, STRLEN(tail), TRUE, &ret);
		}
		यदि (tail == रिक्त)
		    अंक_names = 0;
		अन्यथा
		{
		    अंक_names = recov_file_names(names, tail, FALSE);
		    vim_free(tail);
		}
	    }
	}

	// check for out-of-memory
	प्रति (पूर्णाङ्क i = 0; i < अंक_names; ++i)
	{
	    यदि (names[i] == रिक्त)
	    {
		प्रति (i = 0; i < अंक_names; ++i)
		    vim_free(names[i]);
		अंक_names = 0;
	    }
	}
	यदि (अंक_names == 0)
	    अंक_files = 0;
	अन्यथा यदि (expand_wildcards(अंक_names, names, &अंक_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    अंक_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	यदि (*dirp == NUL && file_गिनती + अंक_files == 0 && fname != रिक्त)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#यदि defined(VMS)
			       (char_u *)"_swp", FALSE
#अन्यथा
			       (char_u *)".swp", TRUE
#endif
			      );
	    यदि (swapname != रिक्त)
	    {
		यदि (mch_stat((वर्ण *)swapname, &st) != -1)    // It exists!
		{
		    files = ALLOC_ONE(char_u *);
		    यदि (files != रिक्त)
		    {
			files[0] = swapname;
			swapname = रिक्त;
			अंक_files = 1;
		    }
		}
		vim_free(swapname);
	    }
	}

	/*
	 * Remove swapfile name of the current buffer, it must be ignored.
	 * But keep it for swapfilelist().
	 */
	यदि (curbuf->b_ml.ml_mfp != रिक्त
			       && (p = curbuf->b_ml.ml_mfp->mf_fname) != रिक्त
			       && ret_list == रिक्त)
	{
	    प्रति (पूर्णाङ्क i = 0; i < अंक_files; ++i)
		// Do not expand wildcards, on windows would try to expand
		// "%tmp%" in "%tmp%file".
		यदि (fullpathcmp(p, files[i], TRUE, FALSE) & FPC_SAME)
		{
		    // Remove the name from files[i].  Move further entries
		    // down.  When the array becomes empty free it here, since
		    // FreeWild() won't be called below.
		    vim_free(files[i]);
		    यदि (--अंक_files == 0)
			vim_free(files);
		    अन्यथा
			प्रति ( ; i < अंक_files; ++i)
			    files[i] = files[i + 1];
		}
	}
	यदि (nr > 0)
	{
	    file_गिनती += अंक_files;
	    यदि (nr <= file_गिनती)
	    {
		*fname_out = vim_strsave(
				      files[nr - 1 + अंक_files - file_गिनती]);
		dirp = (char_u *)"";		    // stop searching
	    }
	}
	अन्यथा यदि (do_list)
	{
	    यदि (dir_name.string[0] == '.' && dir_name.string[1] == NUL)
	    {
		यदि (fname == रिक्त)
		    संदेश_puts(_("   In current directory:\n"));
		अन्यथा
		    संदेश_puts(_("   Using specified name:\n"));
	    }
	    अन्यथा
	    {
		संदेश_puts(_("   In directory "));
		संदेश_home_replace(dir_name.string);
		संदेश_puts(":\n");
	    }

	    यदि (अंक_files)
	    {
		प्रति (पूर्णाङ्क i = 0; i < अंक_files; ++i)
		{
		    // print the swap file name
		    संदेश_outnum((long)++file_गिनती);
		    संदेश_puts(".    ");
		    संदेश_puts((वर्ण *)gettail(files[i]));
		    संदेश_putchar('\n');
		    (शून्य)swapfile_info(files[i]);
		}
	    }
	    अन्यथा
		संदेश_puts(_("      -- none --\n"));
	    out_flush();
	}
#ifdef FEAT_EVAL
	अन्यथा यदि (ret_list != रिक्त)
	{
	    string_T	name;

	    प्रति (पूर्णाङ्क i = 0; i < अंक_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.length,
		    files[i], STRLEN(files[i]), TRUE, &name);
		यदि (name.string != रिक्त)
		{
		    list_append_string(ret_list, name.string, (पूर्णाङ्क)name.length);
		    vim_free(name.string);
		}
	    }
	}
#endif
	अन्यथा
	    file_गिनती += अंक_files;

	प्रति (पूर्णाङ्क i = 0; i < अंक_names; ++i)
	    vim_free(names[i]);
	यदि (अंक_files > 0)
	    FreeWild(अंक_files, files);
    }
    vim_free(dir_name.string);
    प्रत्यागम file_गिनती;
}

#यदि defined(UNIX) || defined(MSWIN)
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
    string_T	d = {रिक्त, 0};
    string_T	fixed_fname;
    char_u	*p;

    fixed_fname.string = fix_fname(name != रिक्त ? name : (char_u *)"");
    यदि (fixed_fname.string == रिक्त)
	प्रत्यागम रिक्त;

    प्रति (p = fixed_fname.string; *p != NUL; MB_PTR_ADV(p))
	यदि (vim_ispathsep(*p))
	    *p = '%';
    fixed_fname.length = (size_t)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_end[-1];
    *p = NUL;
    concat_fnames(dir, (size_t)(p - dir), fixed_fname.string, fixed_fname.length, TRUE, &d);
    vim_free(fixed_fname.string);

    प्रत्यागम d.string;
}
#endif

#यदि (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
स्थिर पूर्णाङ्क process_still_running;
#endif

#यदि defined(FEAT_EVAL)
/*
 * Return information found in swapfile "fname" in dictionary "d".
 * This is used by the swapinfo() function.
 */
    शून्य
get_b0_dict(char_u *fname, dict_T *d)
{
    पूर्णाङ्क fd;
    संरचना block0 b0;

    यदि ((fd = mch_open((वर्ण *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
    {
	यदि (read_eintr(fd, &b0, परिमाण(b0)) == परिमाण(b0))
	{
	    यदि (ml_check_b0_id(&b0) == FAIL)
		dict_add_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    अन्यथा यदि (b0_magic_wrong(&b0))
		dict_add_string_len(d, "error",
		    (char_u *)"Magic number mismatch", STRLEN_LITERAL("Magic number mismatch"));
	    अन्यथा
	    {
		// we have swap information
		dict_add_string_len(d, "version", b0.b0_version, 10);
		dict_add_string_len(d, "user", b0.b0_uname, B0_UNAME_SIZE);
		dict_add_string_len(d, "host", b0.b0_hname, B0_HNAME_SIZE);
		dict_add_string_len(d, "fname", b0.b0_fname, B0_FNAME_SIZE_ORG);

		dict_add_संख्या(d, "pid", char_to_long(b0.b0_pid));
		dict_add_संख्या(d, "mtime", char_to_long(b0.b0_mtime));
		dict_add_संख्या(d, "dirty", b0.b0_dirty ? 1 : 0);
# ifdef CHECK_INODE
		dict_add_संख्या(d, "inode", char_to_long(b0.b0_ino));
# endif
	    }
	}
	अन्यथा
	    dict_add_string_len(d, "error", (char_u *)"Cannot read file", STRLEN_LITERAL("Cannot read file"));
	close(fd);
    }
    अन्यथा
	dict_add_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#endif

/*
 * Give information about an existing swap file.
 * Returns timestamp (0 when unknown).
 */
    स्थिर time_t
swapfile_info(char_u *fname)
{
    stat_T	    st;
    पूर्णाङ्क		    fd;
    संरचना block0   b0;
#ifdef UNIX
    char_u	    uname[B0_UNAME_SIZE];
#endif

    // print the swap file date
    यदि (mch_stat((वर्ण *)fname, &st) != -1)
    {
#ifdef UNIX
	// print name of owner of the file
	यदि (mch_get_uname(st.st_uid, uname, B0_UNAME_SIZE) == OK)
	{
	    संदेश_puts(_("          owned by: "));
	    संदेश_outtrans(uname);
	    संदेश_puts(_("   dated: "));
	}
	अन्यथा
#endif
	    संदेश_puts(_("             dated: "));
	संदेश_puts(get_ctime(st.st_mtime, TRUE));
    }
    अन्यथा
	st.st_mtime = 0;

    /*
     * print the original file name
     */
    fd = mch_open((वर्ण *)fname, O_RDONLY | O_EXTRA, 0);
    यदि (fd >= 0)
    {
	यदि (read_eintr(fd, &b0, परिमाण(b0)) == परिमाण(b0))
	{
	    यदि (STRNCMP(b0.b0_version, "VIM 3.0", 7) == 0)
	    {
		संदेश_puts(_("         [from Vim version 3.0]"));
	    }
	    अन्यथा यदि (ml_check_b0_id(&b0) == FAIL)
	    {
		संदेश_puts(_("         [does not look like a Vim swap file]"));
	    }
	    अन्यथा
	    {
		संदेश_puts(_("         file name: "));
		यदि (b0.b0_fname[0] == NUL)
		    संदेश_puts(_("[No Name]"));
		अन्यथा
		    संदेश_outtrans(b0.b0_fname);

		संदेश_puts(_("\n          modified: "));
		संदेश_puts(b0.b0_dirty ? _("YES") : _("no"));

		यदि (*(b0.b0_uname) != NUL)
		{
		    संदेश_puts(_("\n         user name: "));
		    संदेश_outtrans(b0.b0_uname);
		}

		यदि (*(b0.b0_hname) != NUL)
		{
		    यदि (*(b0.b0_uname) != NUL)
			संदेश_puts(_("   host name: "));
		    अन्यथा
			संदेश_puts(_("\n         host name: "));
		    संदेश_outtrans(b0.b0_hname);
		}

		यदि (char_to_long(b0.b0_pid) != 0L)
		{
		    संदेश_puts(_("\n        process ID: "));
		    संदेश_outnum(char_to_long(b0.b0_pid));
#यदि defined(UNIX) || defined(MSWIN)
		    यदि (swapfile_process_running(&b0, fname))
		    {
			संदेश_puts(_(" (STILL RUNNING)"));
# ifdef HAVE_PROCESS_STILL_RUNNING
			process_still_running = TRUE;
# endif
		    }
#endif
		}

		यदि (b0_magic_wrong(&b0))
		{
#यदि defined(MSWIN)
		    यदि (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			संदेश_puts(_("\n         [not usable with this version of Vim]"));
		    अन्यथा
#endif
			संदेश_puts(_("\n         [not usable on this computer]"));
		}
	    }
	}
	अन्यथा
	    संदेश_puts(_("         [cannot be read]"));
	close(fd);
    }
    अन्यथा
	संदेश_puts(_("         [cannot be opened]"));
    संदेश_putchar('\n');

    प्रत्यागम st.st_mtime;
}

/*
 * Return TRUE if the swap file looks OK and there are no changes, thus it can
 * be safely deleted.
 */
    स्थिर पूर्णाङ्क
swapfile_unchanged(char_u *fname)
{
    stat_T	    st;
    पूर्णाङ्क		    fd;
    संरचना block0   b0;
    पूर्णाङ्क		    ret = TRUE;

    // must be able to stat the swap file
    यदि (mch_stat((वर्ण *)fname, &st) == -1)
	प्रत्यागम FALSE;

    // must be able to read the first block
    fd = mch_open((वर्ण *)fname, O_RDONLY | O_EXTRA, 0);
    यदि (fd < 0)
	प्रत्यागम FALSE;
    यदि (read_eintr(fd, &b0, परिमाण(b0)) != परिमाण(b0))
    {
	close(fd);
	प्रत्यागम FALSE;
    }

    // the ID and magic number must be correct
    यदि (ml_check_b0_id(&b0) == FAIL|| b0_magic_wrong(&b0))
	ret = FALSE;

    // must be unchanged
    यदि (b0.b0_dirty)
	ret = FALSE;

#यदि defined(UNIX) || defined(MSWIN)
    // Host name must be known and must equal the current host name, otherwise
    // comparing pid is meaningless.
    यदि (*(b0.b0_hname) == NUL)
    {
	ret = FALSE;
    }
    अन्यथा
    {
	char_u	    hostname[B0_HNAME_SIZE];

	mch_get_host_name(hostname, B0_HNAME_SIZE);
	hostname[B0_HNAME_SIZE - 1] = NUL;
	b0.b0_hname[B0_HNAME_SIZE - 1] = NUL; // in case of corruption
	यदि (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    यदि (char_to_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#endif

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    close(fd);
    प्रत्यागम ret;
}

    स्थिर पूर्णाङ्क
recov_file_names(char_u **names, char_u *path, पूर्णाङ्क prepend_dot)
{
    पूर्णाङ्क		अंक_names;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    पूर्णाङ्क		i;
#ifndef MSWIN
    bool    shortname = curbuf->b_shortname;

    curbuf->b_shortname = false;
#endif
    string_T	ret;

    अंक_names = 0;

    /*
     * May also add the file name with a dot prepended, for swap file in same
     * dir as original file.
     */
    यदि (prepend_dot)
    {
	names[अंक_names] = modname(path, (char_u *)".sw?", TRUE);
	यदि (names[अंक_names] == रिक्त)
	    गच्छ end;
	++अंक_names;
    }

    /*
     * Form the normal swap file name pattern by appending ".sw?".
     */
#ifdef VMS
    names[अंक_names] = concat_fnames(path, STRLEN(path),
	(char_u *)"_sw%", STRLEN_LITERAL("_sw%"), FALSE, &ret);
#अन्यथा
    names[अंक_names] = concat_fnames(path, STRLEN(path),
	(char_u *)".sw?", STRLEN_LITERAL(".sw?"), FALSE, &ret);
#endif
    यदि (names[अंक_names] == रिक्त)
	गच्छ end;
    यदि (अंक_names >= 1)	    // check if we have the same name twice
    {
	p = names[अंक_names - 1];
	i = (पूर्णाङ्क)STRLEN(names[अंक_names - 1]) - (पूर्णाङ्क)STRLEN(names[अंक_names]);
	यदि (i > 0)
	    p += i;	    // file name has been expanded to full path

	यदि (STRCMP(p, names[अंक_names]) != 0)
	    ++अंक_names;
	अन्यथा
	    vim_free(names[अंक_names]);
    }
    अन्यथा
	++अंक_names;

#ifndef MSWIN
    /*
     * Also try with 'shortname' set, in case the file is on a DOS filesystem.
     */
    curbuf->b_shortname = true;
# ifdef VMS
    names[अंक_names] = modname(path, (char_u *)"_sw%", FALSE);
# अन्यथा
    names[अंक_names] = modname(path, (char_u *)".sw?", FALSE);
# endif
    यदि (names[अंक_names] == रिक्त)
	गच्छ end;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = names[अंक_names];
    i = STRLEN(names[अंक_names]) - STRLEN(names[अंक_names - 1]);
    यदि (i > 0)
	p += i;		// file name has been expanded to full path
    यदि (STRCMP(names[अंक_names - 1], p) == 0)
	vim_free(names[अंक_names]);
    अन्यथा
	++अंक_names;
#endif

end:
#ifndef MSWIN
    curbuf->b_shortname = shortname;
#endif

    प्रत्यागम अंक_names;
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
    शून्य
ml_sync_all(पूर्णाङ्क check_file, पूर्णाङ्क check_char)
{
    buf_T		*buf;
    stat_T		st;

    FOR_ALL_BUFFERS(buf)
    {
	यदि (buf->b_ml.ml_mfp == रिक्त
		|| buf->b_ml.ml_mfp->mf_fname == रिक्त
		|| buf->b_ml.ml_mfp->mf_fd < 0)
	    अनुवर्त;			    // no file

#ifdef FEAT_CRYPT
	यदि (crypt_may_close_swapfile(buf, buf->b_p_कुंजी,
						     crypt_get_method_nr(buf)))
	    अनुवर्त;
#endif

	ml_flush_पंक्ति(buf);		    // flush buffered line
					    // flush locked block
	(शून्य)ml_find_पंक्ति(buf, (linenr_T)0, ML_FLUSH);
	यदि (bufIsChanged(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != रिक्त)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    यदि (mch_stat((वर्ण *)buf->b_ffname, &st) == -1
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
	यदि (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (शून्य)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (bufIsChanged(buf) ? MFS_FLUSH : 0));
	    यदि (check_char && ui_char_avail())	// character available now
		भङ्ग;
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
    शून्य
ml_preserve(buf_T *buf, पूर्णाङ्क message)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    पूर्णाङ्क		status;
    पूर्णाङ्क		got_int_save = got_int;

    यदि (mfp == रिक्त || mfp->mf_fname == रिक्त)
    {
	यदि (message)
	    emsg(_(e_cannot_preserve_there_is_no_swap_file));
	प्रत्यागम;
    }
#ifdef FEAT_CRYPT
    यदि (crypt_may_close_swapfile(buf, buf->b_p_कुंजी, crypt_get_method_nr(buf)))
	प्रत्यागम;
#endif

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_पंक्ति(buf);				    // flush buffered line
    (शून्य)ml_find_पंक्ति(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    status = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

    // stack is invalid after mf_sync(.., MFS_ALL)
    buf->b_ml.ml_ढेर_शीर्ष = 0;

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
    यदि (mf_need_trans(mfp) && !got_int)
    {
	lnum = 1;
	यावत् (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_पंक्ति_गिनती)
	{
	    hp = ml_find_पंक्ति(buf, lnum, ML_FIND);
	    यदि (hp == रिक्त)
	    {
		status = FAIL;
		गच्छ theend;
	    }
	    CHECK(buf->b_ml.ml_locked_निचला != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_ऊपरी + 1;
	}
	(शून्य)ml_find_पंक्ति(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	यदि (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    status = FAIL;
	buf->b_ml.ml_ढेर_शीर्ष = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    यदि (message)
    {
	यदि (status == OK)
	    संदेश(_("File preserved"));
	अन्यथा
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
    प्रत्यागम ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
    char_u *
ml_get_pos(pos_T *pos)
{
    प्रत्यागम (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->col);
}

/*
 * Return pointer to cursor line.
 */
    char_u *
ml_get_curline(शून्य)
{
    प्रत्यागम ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE);
}

/*
 * Return pointer to cursor position.
 */
    char_u *
ml_get_cursor(शून्य)
{
    प्रत्यागम (ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE) +
							curwin->w_cursor.col);
}

// return length (excluding the NUL) of the given line
    colnr_T
ml_get_len(linenr_T lnum)
{
    प्रत्यागम ml_get_buf_len(curbuf, lnum);
}

// return length (excluding the NUL) of the text after position "pos"
    colnr_T
ml_get_pos_len(pos_T *pos)
{
    प्रत्यागम ml_get_buf_len(curbuf, pos->lnum) - pos->col;
}

// return length (excluding the NUL) of the cursor line
    colnr_T
ml_get_curline_len(शून्य)
{
    प्रत्यागम ml_get_buf_len(curbuf, curwin->w_cursor.lnum);
}

// return length (excluding the NUL) of the cursor position
    colnr_T
ml_get_cursor_len(शून्य)
{
    प्रत्यागम ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.col;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*पंक्ति;

    यदि (*(पंक्ति = ml_get_buf(buf, lnum, FALSE)) == NUL)
	प्रत्यागम 0;

    यदि (buf->b_ml.ml_पंक्ति_textlen <= 0)
	buf->b_ml.ml_पंक्ति_textlen = (पूर्णाङ्क)STRLEN(पंक्ति) + 1;
    प्रत्यागम buf->b_ml.ml_पंक्ति_textlen - 1;
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
    पूर्णाङ्क		will_change)		// line will be changed
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    स्थिर पूर्णाङ्क	recursive = 0;
    स्थिर char_u questions[4];

    यदि (lnum > buf->b_ml.ml_पंक्ति_गिनती)	// invalid line number
    {
	यदि (recursive == 0)
	{
	    // Avoid giving this message for a recursive call, may happen when
	    // the GUI redraws part of the text.
	    ++recursive;
	    siemsg(e_ml_get_invalid_lnum_nr, lnum);
	    --recursive;
	}
	ml_flush_पंक्ति(buf);
errorret:
	STRCPY(questions, "???");
	buf->b_ml.ml_पंक्ति_len = 4;
	buf->b_ml.ml_पंक्ति_textlen = buf->b_ml.ml_पंक्ति_len;
	buf->b_ml.ml_पंक्ति_lnum = lnum;
	प्रत्यागम questions;
    }
    यदि (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    यदि (buf->b_ml.ml_mfp == रिक्त)	// there are no lines
    {
	buf->b_ml.ml_पंक्ति_len = 1;
	buf->b_ml.ml_पंक्ति_textlen = buf->b_ml.ml_पंक्ति_len;
	प्रत्यागम (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    यदि (buf->b_ml.ml_पंक्ति_lnum != lnum || mf_dont_release)
    {
	unsigned    start, end;
	पूर्णाङ्क	    idx;

	ml_flush_पंक्ति(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	यदि ((hp = ml_find_पंक्ति(buf, lnum, ML_FIND)) == रिक्त)
	{
	    यदि (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_find_पंक्ति_nr_in_buffer_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    गच्छ errorret;
	}

	dp = (DATA_BL *)(hp->bh_data);

	idx = lnum - buf->b_ml.ml_locked_निचला;
	start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	यदि (idx == 0)
	    end = dp->db_txt_end;
	अन्यथा
	    end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);

	buf->b_ml.ml_पंक्ति_ptr = (char_u *)dp + start;
	buf->b_ml.ml_पंक्ति_len = end - start;
#यदि defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	यदि (buf->b_has_textprop
			 && (size_t)buf->b_ml.ml_पंक्ति_len > परिमाण(textprop_T))
	    buf->b_ml.ml_पंक्ति_textlen = 0;  // call STRLEN() later when needed
	अन्यथा
#endif
	    buf->b_ml.ml_पंक्ति_textlen = buf->b_ml.ml_पंक्ति_len;
	buf->b_ml.ml_पंक्ति_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_पंक्ति_DIRTY | ML_ALLOCATED);
    }
    यदि (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ifdef FEAT_EVAL
	यदि (ml_get_alloc_पंक्तियाँ && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_पंक्ति_DIRTY;
#endif
    }

#ifdef FEAT_EVAL
    यदि (ml_get_alloc_पंक्तियाँ
		 && (buf->b_ml.ml_flags & (ML_पंक्ति_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_पंक्ति_len);

	// make sure the text is in allocated memory
	यदि (p != रिक्त)
	{
	    memmove(p, buf->b_ml.ml_पंक्ति_ptr, buf->b_ml.ml_पंक्ति_len);
	    buf->b_ml.ml_पंक्ति_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    यदि (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_पंक्ति_DIRTY;
	}
    }
#endif
    प्रत्यागम buf->b_ml.ml_पंक्ति_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    पूर्णाङ्क
ml_पंक्ति_alloced(शून्य)
{
    प्रत्यागम (curbuf->b_ml.ml_flags & ML_पंक्ति_DIRTY);
}

#ifdef FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    स्थिर शून्य
add_text_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**पंक्ति,
	    पूर्णाङ्क		*len,
	    char_u	**tofree)
{
    पूर्णाङ्क		round;
    पूर्णाङ्क		new_prop_गिनती = 0;
    पूर्णाङ्क		गिनती;
    पूर्णाङ्क		n;
    char_u	*props;
    पूर्णाङ्क		new_len = 0;  // init for gcc
    char_u	*new_पंक्ति = रिक्त;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    प्रति (round = 1; round <= 2; ++round)
    {
	यदि (round == 2)
	{
	    uint16_t pc;

	    यदि (new_prop_गिनती == 0)
		प्रत्यागम;  // nothing to do
	    new_len = *len + (पूर्णाङ्क)PROP_गिनती_SIZE
			     + new_prop_गिनती * (पूर्णाङ्क)परिमाण(textprop_T);
	    new_पंक्ति = alloc(new_len);
	    यदि (new_पंक्ति == रिक्त)
		प्रत्यागम;
	    mch_memmove(new_पंक्ति, *पंक्ति, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_गिनती;
	    mch_memmove(new_पंक्ति + *len, &pc, PROP_गिनती_SIZE);
	    new_prop_गिनती = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	गिनती = get_text_props(buf, lnum, &props, FALSE);
	प्रति (n = 0; n < गिनती; ++n)
	{
	    mch_memmove(&prop, props + n * परिमाण(textprop_T),
							   परिमाण(textprop_T));
	    यदि (prop.tp_flags & TP_FLAG_CONT_NEXT)
	    {
		यदि (round == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_PREV;
		    prop.tp_col = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_text_offset = 0;
		    mch_memmove(new_पंक्ति + *len + (पूर्णाङ्क)PROP_गिनती_SIZE
			    + new_prop_गिनती * परिमाण(textprop_T),
			    &prop, परिमाण(textprop_T));
		}
		++new_prop_गिनती;
	    }
	}
    }
    *पंक्ति = new_पंक्ति;
    *tofree = new_पंक्ति;
    *len = new_len;
}
#endif

    स्थिर पूर्णाङ्क
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*पंक्ति_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    पूर्णाङ्क		flags)		// ML_APPEND_ flags
{
    char_u	*पंक्ति = पंक्ति_arg;
    colnr_T	len = len_arg;
    पूर्णाङ्क		i;
    पूर्णाङ्क		पंक्ति_गिनती;	// number of indexes in current block
    पूर्णाङ्क		offset;
    पूर्णाङ्क		from, to;
    long	space_needed;	// space needed for new line
    पूर्णाङ्क		page_size;
    पूर्णाङ्क		page_गिनती;
    पूर्णाङ्क		db_idx;		// index for lnum in data block
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
#ifdef FEAT_PROP_POPUP
    char_u	*tofree = रिक्त;
# ifdef FEAT_BYTEOFF
    colnr_T	text_len = 0;	// text len with NUL without text properties
# endif
#endif
    पूर्णाङ्क		ret = FAIL;

    यदि (lnum > buf->b_ml.ml_पंक्ति_गिनती || buf->b_ml.ml_mfp == रिक्त)
	प्रत्यागम FAIL;  // lnum out of range

    यदि (lowest_marked && lowest_marked > lnum)
	lowest_marked = lnum + 1;

    यदि (len == 0)
    {
	len = (colnr_T)STRLEN(पंक्ति) + 1;	// space needed for the text
#यदि defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	text_len = len;
#endif
    }
#यदि defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    अन्यथा यदि (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	text_len = (colnr_T)STRLEN(पंक्ति) + 1;
    अन्यथा
	text_len = len;
#endif

#ifdef FEAT_PROP_POPUP
    यदि (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	add_text_props_for_append(buf, lnum, &पंक्ति, &len, &tofree);
#endif

    space_needed = len + INDEX_SIZE;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    page_size = mfp->mf_page_size;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    यदि ((hp = ml_find_पंक्ति(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_जोड़ो)) == रिक्त)
	गच्छ theend;

    buf->b_ml.ml_flags &= ~ML_EMPTY;

    यदि (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    अन्यथा
	db_idx = lnum - buf->b_ml.ml_locked_निचला;
		// get line count before the insertion
    पंक्ति_गिनती = buf->b_ml.ml_locked_ऊपरी - buf->b_ml.ml_locked_निचला;

    dp = (DATA_BL *)(hp->bh_data);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    यदि ((long)dp->db_free < space_needed && db_idx == पंक्ति_गिनती - 1
					    && lnum < buf->b_ml.ml_पंक्ति_गिनती)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_ऊपरी);
	यदि ((hp = ml_find_पंक्ति(buf, lnum + 1, ML_जोड़ो)) == रिक्त)
	    गच्छ theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	पंक्ति_गिनती = buf->b_ml.ml_locked_ऊपरी - buf->b_ml.ml_locked_निचला;
	CHECK(buf->b_ml.ml_locked_निचला != lnum + 1, "locked_low != lnum + 1");

	dp = (DATA_BL *)(hp->bh_data);
    }

    ++buf->b_ml.ml_पंक्ति_गिनती;

    यदि ((long)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_start -= len;
	dp->db_free -= space_needed;
	++(dp->db_पंक्ति_गिनती);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	यदि (पंक्ति_गिनती > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    यदि (db_idx < 0)
		offset = dp->db_txt_end;
	    अन्यथा
		offset = ((dp->db_index[db_idx]) & DB_INDEX_MASK);
	    mch_memmove((वर्ण *)dp + dp->db_txt_start,
					  (वर्ण *)dp + dp->db_txt_start + len,
				 (size_t)(offset - (dp->db_txt_start + len)));
	    प्रति (i = पंक्ति_गिनती - 1; i > db_idx; --i)
		dp->db_index[i + 1] = dp->db_index[i] - len;
	    dp->db_index[db_idx + 1] = offset - len;
	}
	अन्यथा
	    // add line at the end (which is the start of the text)
	    dp->db_index[db_idx + 1] = dp->db_txt_start;

	/*
	 * copy the text into the block
	 */
	mch_memmove((वर्ण *)dp + dp->db_index[db_idx + 1], पंक्ति, (size_t)len);
	यदि (flags & ML_APPEND_MARK)
	    dp->db_index[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	यदि (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    अन्यथा	    // not enough space in data block
    {
	long	    पंक्ति_गिनती_बायाँ, पंक्ति_गिनती_दायाँ;
	पूर्णाङ्क	    page_गिनती_बायाँ, page_गिनती_दायाँ;
	bhdr_T	    *hp_बायाँ;
	bhdr_T	    *hp_दायाँ;
	bhdr_T	    *hp_new;
	पूर्णाङ्क	    पंक्तियाँ_moved;
	पूर्णाङ्क	    data_moved = 0;	    // init to shut up gcc
	पूर्णाङ्क	    total_moved = 0;	    // init to shut up gcc
	DATA_BL	    *dp_दायाँ, *dp_बायाँ;
	पूर्णाङ्क	    ढेर_idx;
	पूर्णाङ्क	    in_बायाँ;
	पूर्णाङ्क	    lineadd;
	blocknr_T   bnum_बायाँ, bnum_दायाँ;
	linenr_T    lnum_बायाँ, lnum_दायाँ;
	पूर्णाङ्क	    pb_idx;
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
	यदि (db_idx < 0)		// left block is new, right block is existing
	{
	    पंक्तियाँ_moved = 0;
	    in_बायाँ = TRUE;
	    // space_needed does not change
	}
	अन्यथा			// left block is existing, right block is new
	{
	    पंक्तियाँ_moved = पंक्ति_गिनती - db_idx - 1;
	    यदि (पंक्तियाँ_moved == 0)
		in_बायाँ = FALSE;	// put new line in right block
					// space_needed does not change
	    अन्यथा
	    {
		data_moved = ((dp->db_index[db_idx]) & DB_INDEX_MASK) -
							    dp->db_txt_start;
		total_moved = data_moved + पंक्तियाँ_moved * INDEX_SIZE;
		यदि ((long)dp->db_free + total_moved >= space_needed)
		{
		    in_बायाँ = TRUE;	// put new line in left block
		    space_needed = total_moved;
		}
		अन्यथा
		{
		    in_बायाँ = FALSE;	    // put new line in right block
		    space_needed += total_moved;
		}
	    }
	}

	page_गिनती = ((space_needed + HEADER_SIZE) + page_size - 1) / page_size;
	यदि ((hp_new = ml_new_data(mfp, flags & ML_APPEND_NEW, page_गिनती))
								       == रिक्त)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_ऊपरी);
	    गच्छ theend;
	}
	यदि (db_idx < 0)		// left block is new
	{
	    hp_बायाँ = hp_new;
	    hp_दायाँ = hp;
	    पंक्ति_गिनती_बायाँ = 0;
	    पंक्ति_गिनती_दायाँ = पंक्ति_गिनती;
	}
	अन्यथा			// right block is new
	{
	    hp_बायाँ = hp;
	    hp_दायाँ = hp_new;
	    पंक्ति_गिनती_बायाँ = पंक्ति_गिनती;
	    पंक्ति_गिनती_दायाँ = 0;
	}
	dp_दायाँ = (DATA_BL *)(hp_दायाँ->bh_data);
	dp_बायाँ = (DATA_BL *)(hp_बायाँ->bh_data);
	bnum_बायाँ = hp_बायाँ->bh_bnum;
	bnum_दायाँ = hp_दायाँ->bh_bnum;
	page_गिनती_बायाँ = hp_बायाँ->bh_page_गिनती;
	page_गिनती_दायाँ = hp_दायाँ->bh_page_गिनती;

	/*
	 * May move the new line into the right/new block.
	 */
	यदि (!in_बायाँ)
	{
	    dp_दायाँ->db_txt_start -= len;
	    dp_दायाँ->db_free -= len + INDEX_SIZE;
	    dp_दायाँ->db_index[0] = dp_दायाँ->db_txt_start;
	    यदि (flags & ML_APPEND_MARK)
		dp_दायाँ->db_index[0] |= DB_MARKED;

	    mch_memmove((वर्ण *)dp_दायाँ + dp_दायाँ->db_txt_start,
							   पंक्ति, (size_t)len);
	    ++पंक्ति_गिनती_दायाँ;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	यदि (पंक्तियाँ_moved)
	{
	    dp_दायाँ->db_txt_start -= data_moved;
	    dp_दायाँ->db_free -= total_moved;
	    mch_memmove((वर्ण *)dp_दायाँ + dp_दायाँ->db_txt_start,
			(वर्ण *)dp_बायाँ + dp_बायाँ->db_txt_start,
			(size_t)data_moved);
	    offset = dp_दायाँ->db_txt_start - dp_बायाँ->db_txt_start;
	    dp_बायाँ->db_txt_start += data_moved;
	    dp_बायाँ->db_free += total_moved;

	    /*
	     * update indexes in the new block
	     */
	    प्रति (to = पंक्ति_गिनती_दायाँ, from = db_idx + 1;
					 from < पंक्ति_गिनती_बायाँ; ++from, ++to)
		dp_दायाँ->db_index[to] = dp->db_index[from] + offset;
	    पंक्ति_गिनती_दायाँ += पंक्तियाँ_moved;
	    पंक्ति_गिनती_बायाँ -= पंक्तियाँ_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	यदि (in_बायाँ)
	{
	    dp_बायाँ->db_txt_start -= len;
	    dp_बायाँ->db_free -= len + INDEX_SIZE;
	    dp_बायाँ->db_index[पंक्ति_गिनती_बायाँ] = dp_बायाँ->db_txt_start;
	    यदि (flags & ML_APPEND_MARK)
		dp_बायाँ->db_index[पंक्ति_गिनती_बायाँ] |= DB_MARKED;
	    mch_memmove((वर्ण *)dp_बायाँ + dp_बायाँ->db_txt_start,
							   पंक्ति, (size_t)len);
	    ++पंक्ति_गिनती_बायाँ;
	}

	यदि (db_idx < 0)		// left block is new
	{
	    lnum_बायाँ = lnum + 1;
	    lnum_दायाँ = 0;
	}
	अन्यथा			// right block is new
	{
	    lnum_बायाँ = 0;
	    यदि (in_बायाँ)
		lnum_दायाँ = lnum + 2;
	    अन्यथा
		lnum_दायाँ = lnum + 1;
	}
	dp_बायाँ->db_पंक्ति_गिनती = पंक्ति_गिनती_बायाँ;
	dp_दायाँ->db_पंक्ति_गिनती = पंक्ति_गिनती_दायाँ;

	/*
	 * release the two data blocks
	 * The new one (hp_new) already has a correct blocknumber.
	 * The old one (hp, in ml_locked) gets a positive blocknumber if
	 * we changed it and we are not editing a new file.
	 */
	यदि (पंक्तियाँ_moved || in_बायाँ)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	यदि (!(flags & ML_APPEND_NEW) && db_idx >= 0 && in_बायाँ)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_find_पंक्ति(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	प्रति (ढेर_idx = buf->b_ml.ml_ढेर_शीर्ष - 1; ढेर_idx >= 0;
								  --ढेर_idx)
	{
	    ip = &(buf->b_ml.ml_ढेर[ढेर_idx]);
	    pb_idx = ip->ip_index;
	    यदि ((hp = mf_get(mfp, ip->ip_bnum, 1)) == रिक्त)
		गच्छ theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    यदि (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		गच्छ theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    यदि (pp->pb_गिनती < pp->pb_गिनती_max)
	    {
		यदि (pb_idx + 1 < (पूर्णाङ्क)pp->pb_गिनती)
		    mch_memmove(&pp->pb_pointer[pb_idx + 2],
				&pp->pb_pointer[pb_idx + 1],
			(size_t)(pp->pb_गिनती - pb_idx - 1) * परिमाण(PTR_EN));
		++pp->pb_गिनती;
		pp->pb_pointer[pb_idx].pe_पंक्ति_गिनती = पंक्ति_गिनती_बायाँ;
		pp->pb_pointer[pb_idx].pe_bnum = bnum_बायाँ;
		pp->pb_pointer[pb_idx].pe_page_गिनती = page_गिनती_बायाँ;
		pp->pb_pointer[pb_idx + 1].pe_पंक्ति_गिनती = पंक्ति_गिनती_दायाँ;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_दायाँ;
		pp->pb_pointer[pb_idx + 1].pe_page_गिनती = page_गिनती_दायाँ;

		यदि (lnum_बायाँ != 0)
		    pp->pb_pointer[pb_idx].pe_old_lnum = lnum_बायाँ;
		यदि (lnum_दायाँ != 0)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_दायाँ;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_ढेर_शीर्ष = ढेर_idx + 1;	    // truncate stack

		यदि (lineadd)
		{
		    --(buf->b_ml.ml_ढेर_शीर्ष);
		    // fix line count for rest of blocks in the stack
		    ml_lineadd(buf, lineadd);
							// fix stack itself
		    buf->b_ml.ml_ढेर[buf->b_ml.ml_ढेर_शीर्ष].ip_ऊपरी +=
								      lineadd;
		    ++(buf->b_ml.ml_ढेर_शीर्ष);
		}

		/*
		 * We are finished, break the loop here.
		 */
		भङ्ग;
	    }
	    // pointer block full
	    /*
	     * split the pointer block
	     * allocate a new pointer block
	     * move some of the pointer into the new block
	     * prepare for updating the parent block
	     */
	    प्रति (;;)	// do this twice when splitting block 1
	    {
		hp_new = ml_new_ptr(mfp);
		यदि (hp_new == रिक्त)	    // TODO: try to fix tree
		    गच्छ theend;
		pp_new = (PTR_BL *)(hp_new->bh_data);

		यदि (hp->bh_bnum != 1)
		    भङ्ग;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (size_t)page_size);
		pp->pb_गिनती = 1;
		pp->pb_pointer[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_pointer[0].pe_पंक्ति_गिनती = buf->b_ml.ml_पंक्ति_गिनती;
		pp->pb_pointer[0].pe_old_lnum = 1;
		pp->pb_pointer[0].pe_page_गिनती = 1;
		mf_put(mfp, hp, TRUE, FALSE);   // release block 1
		hp = hp_new;		// new block is to be split
		pp = pp_new;
		CHECK(ढेर_idx != 0, _("stack_idx should be 0"));
		ip->ip_index = 0;
		++ढेर_idx;	// do block 1 again later
	    }
	    /*
	     * move the pointers after the current one to the new block
	     * If there are none, the new entry will be in the new block.
	     */
	    total_moved = pp->pb_गिनती - pb_idx - 1;
	    यदि (total_moved)
	    {
		mch_memmove(&pp_new->pb_pointer[0],
				&pp->pb_pointer[pb_idx + 1],
				(size_t)(total_moved) * परिमाण(PTR_EN));
		pp_new->pb_गिनती = total_moved;
		pp->pb_गिनती -= total_moved - 1;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_दायाँ;
		pp->pb_pointer[pb_idx + 1].pe_पंक्ति_गिनती = पंक्ति_गिनती_दायाँ;
		pp->pb_pointer[pb_idx + 1].pe_page_गिनती = page_गिनती_दायाँ;
		यदि (lnum_दायाँ)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_दायाँ;
	    }
	    अन्यथा
	    {
		pp_new->pb_गिनती = 1;
		pp_new->pb_pointer[0].pe_bnum = bnum_दायाँ;
		pp_new->pb_pointer[0].pe_पंक्ति_गिनती = पंक्ति_गिनती_दायाँ;
		pp_new->pb_pointer[0].pe_page_गिनती = page_गिनती_दायाँ;
		pp_new->pb_pointer[0].pe_old_lnum = lnum_दायाँ;
	    }
	    pp->pb_pointer[pb_idx].pe_bnum = bnum_बायाँ;
	    pp->pb_pointer[pb_idx].pe_पंक्ति_गिनती = पंक्ति_गिनती_बायाँ;
	    pp->pb_pointer[pb_idx].pe_page_गिनती = page_गिनती_बायाँ;
	    यदि (lnum_बायाँ)
		pp->pb_pointer[pb_idx].pe_old_lnum = lnum_बायाँ;
	    lnum_बायाँ = 0;
	    lnum_दायाँ = 0;

	    /*
	     * recompute line counts
	     */
	    पंक्ति_गिनती_दायाँ = 0;
	    प्रति (i = 0; i < (पूर्णाङ्क)pp_new->pb_गिनती; ++i)
		पंक्ति_गिनती_दायाँ += pp_new->pb_pointer[i].pe_पंक्ति_गिनती;
	    पंक्ति_गिनती_बायाँ = 0;
	    प्रति (i = 0; i < (पूर्णाङ्क)pp->pb_गिनती; ++i)
		पंक्ति_गिनती_बायाँ += pp->pb_pointer[i].pe_पंक्ति_गिनती;

	    bnum_बायाँ = hp->bh_bnum;
	    bnum_दायाँ = hp_new->bh_bnum;
	    page_गिनती_बायाँ = 1;
	    page_गिनती_दायाँ = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	यदि (ढेर_idx < 0)
	{
	    iemsg(e_updated_too_many_blocks);
	    buf->b_ml.ml_ढेर_शीर्ष = 0;	// invalidate stack
	}
    }

#ifdef FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (long)text_len
# अन्यथा
	    (long)len
# endif
	    , ML_CHNK_ADDLINE);
#endif

#ifdef FEAT_NETBEANS_INTG
    यदि (!(flags & ML_APPEND_NEW) && netbeans_active())
    {
	पूर्णाङ्क पंक्ति_len = (पूर्णाङ्क)STRLEN(पंक्ति);
	यदि (पंक्ति_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, पंक्ति, पंक्ति_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)पंक्ति_len, (char_u *)"\n", 1);
    }
#endif
#ifdef FEAT_JOB_CHANNEL
    यदि (!(flags & ML_APPEND_NEW) && buf->b_write_to_channel)
	channel_write_new_पंक्तियाँ(buf);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    vim_free(tofree);
#endif
    प्रत्यागम ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    स्थिर पूर्णाङ्क
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*पंक्ति,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    पूर्णाङ्क		flags)		// ML_APPEND_ flags
{
    यदि (lnum > buf->b_ml.ml_पंक्ति_गिनती)
	प्रत्यागम FAIL;  // lnum out of range

    यदि (buf->b_ml.ml_पंक्ति_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_पंक्ति(buf);

#ifdef FEAT_EVAL
    यदि (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	यदि (buf->b_ml.ml_पंक्ति_lnum != 0)
	    ml_flush_पंक्ति(buf);
    }
#endif

    प्रत्यागम ml_append_int(buf, lnum, पंक्ति, len, flags);
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
    पूर्णाङ्क
ml_append(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*पंक्ति,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    पूर्णाङ्क		newfile)	// flag, see above
{
    प्रत्यागम ml_append_flags(lnum, पंक्ति, len, newfile ? ML_APPEND_NEW : 0);
}

    पूर्णाङ्क
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*पंक्ति,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    पूर्णाङ्क		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    यदि (curbuf->b_ml.ml_mfp == रिक्त && open_buffer(FALSE, रिक्त, 0) == FAIL)
	प्रत्यागम FAIL;
    प्रत्यागम ml_append_flush(curbuf, lnum, पंक्ति, len, flags);
}


#यदि defined(FEAT_SPELL) || defined(FEAT_QUICKFIX) || defined(FEAT_PROP_POPUP)

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
    पूर्णाङ्क
ml_append_buf(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*पंक्ति,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    पूर्णाङ्क		newfile)	// flag, see above
{
    यदि (buf->b_ml.ml_mfp == रिक्त)
	प्रत्यागम FAIL;
    प्रत्यागम ml_append_flush(buf, lnum, पंक्ति, len, newfile ? ML_APPEND_NEW : 0);
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
    पूर्णाङ्क
ml_replace(linenr_T lnum, char_u *पंक्ति, पूर्णाङ्क copy)
{
    colnr_T len = -1;

    यदि (पंक्ति != रिक्त)
	len = (colnr_T)STRLEN(पंक्ति);
    प्रत्यागम ml_replace_len(lnum, पंक्ति, len, FALSE, copy);
}

/*
 * Replace a line for the current buffer.  Like ml_replace() with:
 * "len_arg" is the length of the text, excluding NUL.
 * If "has_props" is TRUE then "line_arg" includes the text properties and
 * "len_arg" includes the NUL of the text and text properties.
 * When "copy" is TRUE copy the text into allocated memory, otherwise
 * "line_arg" must be allocated and will be consumed here.
 */
    पूर्णाङ्क
ml_replace_len(
	linenr_T    lnum,
	char_u	    *पंक्ति_arg,
	colnr_T	    len_arg,
	पूर्णाङ्क	    has_props,
	पूर्णाङ्क	    copy)
{
    char_u *पंक्ति = पंक्ति_arg;
    colnr_T len = len_arg;

    यदि (पंक्ति == रिक्त)		// just checking...
	प्रत्यागम FAIL;

    // When starting up, we might still need to create the memfile
    यदि (curbuf->b_ml.ml_mfp == रिक्त && open_buffer(FALSE, रिक्त, 0) == FAIL)
	प्रत्यागम FAIL;

    यदि (!has_props)
	++len;  // include the NUL after the text
    यदि (copy)
    {
	// copy the line to allocated memory
#ifdef FEAT_PROP_POPUP
	यदि (has_props)
	    पंक्ति = vim_memsave(पंक्ति, len);
	अन्यथा
#endif
	    पंक्ति = vim_strnsave(पंक्ति, len - 1);
	यदि (पंक्ति == रिक्त)
	    प्रत्यागम FAIL;
    }

#ifdef FEAT_NETBEANS_INTG
    यदि (netbeans_active())
    {
	netbeans_removed(curbuf, lnum, 0, (long)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, पंक्ति, (पूर्णाङ्क)STRLEN(पंक्ति));
    }
#endif
    यदि (curbuf->b_ml.ml_पंक्ति_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_पंक्ति(curbuf);

#ifdef FEAT_PROP_POPUP
	यदि (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#endif
    }

#ifdef FEAT_PROP_POPUP
    यदि (curbuf->b_has_textprop && !has_props)
    {
	size_t	oldtextlen = STRLEN(curbuf->b_ml.ml_पंक्ति_ptr) + 1;

	यदि (oldtextlen < (size_t)curbuf->b_ml.ml_पंक्ति_len)
	{
	    char_u *newline;
	    size_t textproplen = curbuf->b_ml.ml_पंक्ति_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    यदि (newline != रिक्त)
	    {
		mch_memmove(newline, पंक्ति, len);
		mch_memmove(newline + len, curbuf->b_ml.ml_पंक्ति_ptr
						    + oldtextlen, textproplen);
		vim_free(पंक्ति);
		पंक्ति = newline;
		len += (colnr_T)textproplen;
	    }
	}
    }
#endif

    यदि (curbuf->b_ml.ml_flags & (ML_पंक्ति_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_पंक्ति_ptr);	// free allocated line

    curbuf->b_ml.ml_पंक्ति_ptr = पंक्ति;
    curbuf->b_ml.ml_पंक्ति_len = len;
    curbuf->b_ml.ml_पंक्ति_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_पंक्ति_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_पंक्ति_DIRTY) & ~ML_EMPTY;

    प्रत्यागम OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * Adjust text properties in line "lnum" for a deleted line.
 * When "above" is true this is the line above the deleted line, otherwise this
 * is the line below the deleted line.
 * "del_props[del_props_len]" are the properties of the deleted line.
 */
    स्थिर शून्य
adjust_text_props_for_delete(
	buf_T	    *buf,
	linenr_T    lnum,
	char_u	    *del_props,
	पूर्णाङ्क	    del_props_len,
	पूर्णाङ्क	    above)
{
    पूर्णाङ्क		did_get_पंक्ति = FALSE;
    पूर्णाङ्क		done_del;
    पूर्णाङ्क		done_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    DATA_BL	*dp;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		पंक्ति_start;
    long	पंक्ति_size;
    पूर्णाङ्क		this_props_len = 0;
    char_u	*text;
    size_t	textlen;
    पूर्णाङ्क		found;

    प्रति (done_del = 0; done_del < del_props_len; done_del += परिमाण(textprop_T))
    {
	mch_memmove(&prop_del, del_props + done_del, परिमाण(textprop_T));
	यदि ((above && (prop_del.tp_flags & TP_FLAG_CONT_PREV)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_NEXT))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_NEXT)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_PREV)))
	{
	    यदि (!did_get_पंक्ति)
	    {
		did_get_पंक्ति = TRUE;
		यदि ((hp = ml_find_पंक्ति(buf, lnum, ML_FIND)) == रिक्त)
		    प्रत्यागम;

		dp = (DATA_BL *)(hp->bh_data);
		idx = lnum - buf->b_ml.ml_locked_निचला;
		पंक्ति_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
		यदि (idx == 0)		// first line in block, text at the end
		    पंक्ति_size = dp->db_txt_end - पंक्ति_start;
		अन्यथा
		    पंक्ति_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK)
								  - पंक्ति_start;
		text = (char_u *)dp + पंक्ति_start;
		textlen = STRLEN(text) + 1;
		यदि ((long)textlen >= पंक्ति_size)
		{
		    // No properties on this line.
		    यदि (above)
			internal_error("no text property above deleted line");
		    अन्यथा
			internal_error("no text property below deleted line");
		    प्रत्यागम;
		}
		यदि ((long)textlen + (long)PROP_गिनती_SIZE > पंक्ति_size)
		{
		    internal_error("text property data too short");
		    प्रत्यागम;
		}

		uint16_t pc;

		mch_memmove(&pc, text + textlen, PROP_गिनती_SIZE);
		यदि (!text_prop_गिनती_valid(pc, (size_t)(पंक्ति_size - (long)textlen)))
		{
		    internal_error("text property count too large");
		    प्रत्यागम;
		}
		this_props_len = pc * (पूर्णाङ्क)परिमाण(textprop_T);
	    }

	    found = FALSE;
	    {
		char_u *props_start = text + textlen + PROP_गिनती_SIZE;

		प्रति (done_this = 0; done_this < this_props_len;
					       done_this += परिमाण(textprop_T))
		{
		    पूर्णाङ्क		flag = above ? TP_FLAG_CONT_NEXT
							   : TP_FLAG_CONT_PREV;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_start + done_this,
							   परिमाण(textprop_T));
		    यदि ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			found = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_start + done_this, &prop_this,
							   परिमाण(textprop_T));
			भङ्ग;
		    }
		}
	    }
	    यदि (!found)
	    {
		यदि (above)
		    internal_error("text property above deleted line not found");
		अन्यथा
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
    स्थिर पूर्णाङ्क
ml_delete_int(buf_T *buf, linenr_T lnum, पूर्णाङ्क flags)
{
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    पूर्णाङ्क		गिनती;	    // number of entries in block
    पूर्णाङ्क		idx;
    पूर्णाङ्क		ढेर_idx;
    पूर्णाङ्क		text_start;
    पूर्णाङ्क		पंक्ति_start;
    long	पंक्ति_size;
    पूर्णाङ्क		i;
    पूर्णाङ्क		ret = FAIL;
#ifdef FEAT_PROP_POPUP
    char_u	*textprop_save = रिक्त;
    long	textprop_len = 0;
#endif

    यदि (lowest_marked && lowest_marked > lnum)
	lowest_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    यदि (buf->b_ml.ml_पंक्ति_गिनती == 1)	    // file becomes empty
    {
	यदि ((flags & ML_DEL_MESSAGE)
#ifdef FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_पंक्तियाँ
#endif
	   )
	    set_keep_संदेश((char_u *)_(no_पंक्तियाँ_संदेश), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_EMPTY;

	प्रत्यागम i;
    }

/*
 * Find the data block containing the line.
 * This also fills the stack with the blocks from the root to the data block.
 * This also releases any locked block..
 */
    mfp = buf->b_ml.ml_mfp;
    यदि (mfp == रिक्त)
	प्रत्यागम FAIL;

    यदि ((hp = ml_find_पंक्ति(buf, lnum, ML_DELETE)) == रिक्त)
	प्रत्यागम FAIL;

    dp = (DATA_BL *)(hp->bh_data);
    // compute line count before the delete
    गिनती = (long)(buf->b_ml.ml_locked_ऊपरी)
					- (long)(buf->b_ml.ml_locked_निचला) + 2;
    idx = lnum - buf->b_ml.ml_locked_निचला;

    --buf->b_ml.ml_पंक्ति_गिनती;

    पंक्ति_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
    यदि (idx == 0)		// first line in block, text at the end
	पंक्ति_size = dp->db_txt_end - पंक्ति_start;
    अन्यथा
	पंक्ति_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK) - पंक्ति_start;

#ifdef FEAT_NETBEANS_INTG
    यदि (netbeans_active())
	netbeans_removed(buf, lnum, 0, पंक्ति_size);
#endif
#ifdef FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    यदि (buf->b_has_textprop)
    {
	size_t	textlen = STRLEN((char_u *)dp + पंक्ति_start) + 1;

	textprop_len = पंक्ति_size - (long)textlen;
	यदि (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + पंक्ति_start + textlen,
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
    यदि (गिनती == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = रिक्त;

	प्रति (ढेर_idx = buf->b_ml.ml_ढेर_शीर्ष - 1; ढेर_idx >= 0;
								  --ढेर_idx)
	{
	    buf->b_ml.ml_ढेर_शीर्ष = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_ढेर[ढेर_idx]);
	    idx = ip->ip_index;
	    यदि ((hp = mf_get(mfp, ip->ip_bnum, 1)) == रिक्त)
		गच्छ theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    यदि (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		गच्छ theend;
	    }
	    गिनती = --(pp->pb_गिनती);
	    यदि (गिनती == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    अन्यथा
	    {
		यदि (गिनती != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_pointer[idx], &pp->pb_pointer[idx + 1],
				      (size_t)(गिनती - idx) * परिमाण(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_ढेर_शीर्ष = ढेर_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		यदि (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_ढेर[buf->b_ml.ml_ढेर_शीर्ष].ip_ऊपरी +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_ढेर_शीर्ष);

		भङ्ग;
	    }
	}
	CHECK(ढेर_idx < 0, _("deleted block 1?"));
    }
    अन्यथा
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	text_start = dp->db_txt_start;
	mch_memmove((वर्ण *)dp + text_start + पंक्ति_size,
		  (वर्ण *)dp + text_start, (size_t)(पंक्ति_start - text_start));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	प्रति (i = idx; i < गिनती - 1; ++i)
	    dp->db_index[i] = dp->db_index[i + 1] + पंक्ति_size;

	dp->db_free += पंक्ति_size + INDEX_SIZE;
	dp->db_txt_start += पंक्ति_size;
	--(dp->db_पंक्ति_गिनती);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#ifdef FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, पंक्ति_size
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    यदि (textprop_save != रिक्त)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_data;
	पूर्णाङ्क	    props_bytes;

	mch_memmove(&pc, textprop_save, PROP_गिनती_SIZE);
	props_data = textprop_save + PROP_गिनती_SIZE;
	props_bytes = pc * (पूर्णाङ्क)परिमाण(textprop_T);
	यदि (!text_prop_गिनती_valid(pc, (size_t)textprop_len))
	    props_bytes = 0;

	// Adjust text properties in the line above and below.
	यदि (lnum > 1)
	    adjust_text_props_for_delete(buf, lnum - 1,
					     props_data, props_bytes, TRUE);
	यदि (lnum <= buf->b_ml.ml_पंक्ति_गिनती)
	    adjust_text_props_for_delete(buf, lnum,
					    props_data, props_bytes, FALSE);
    }
    vim_free(textprop_save);
#endif
    प्रत्यागम ret;
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
    पूर्णाङ्क
ml_delete(linenr_T lnum)
{
    प्रत्यागम ml_delete_flags(lnum, 0);
}

/*
 * Like ml_delete() but using flags (see ml_delete_int()).
 */
    पूर्णाङ्क
ml_delete_flags(linenr_T lnum, पूर्णाङ्क flags)
{
    ml_flush_पंक्ति(curbuf);
    यदि (lnum < 1 || lnum > curbuf->b_ml.ml_पंक्ति_गिनती)
	प्रत्यागम FAIL;

#ifdef FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#endif

    प्रत्यागम ml_delete_int(curbuf, lnum, flags);
}

/*
 * set the DB_MARKED flag for line 'lnum'
 */
    शून्य
ml_setmarked(linenr_T lnum)
{
    bhdr_T    *hp;
    DATA_BL *dp;
				    // invalid line number
    यदि (lnum < 1 || lnum > curbuf->b_ml.ml_पंक्ति_गिनती
					       || curbuf->b_ml.ml_mfp == रिक्त)
	प्रत्यागम;			    // give error message?

    यदि (lowest_marked == 0 || lowest_marked > lnum)
	lowest_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    यदि ((hp = ml_find_पंक्ति(curbuf, lnum, ML_FIND)) == रिक्त)
	प्रत्यागम;		    // give error message?

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[lnum - curbuf->b_ml.ml_locked_निचला] |= DB_MARKED;
    curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
}

/*
 * find the first line with its DB_MARKED flag set
 */
    linenr_T
ml_firstmarked(शून्य)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    पूर्णाङ्क		i;

    यदि (curbuf->b_ml.ml_mfp == रिक्त)
	प्रत्यागम (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    प्रति (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_पंक्ति_गिनती; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	यदि ((hp = ml_find_पंक्ति(curbuf, lnum, ML_FIND)) == रिक्त)
	    प्रत्यागम (linenr_T)0;		    // give error message?

	dp = (DATA_BL *)(hp->bh_data);

	प्रति (i = lnum - curbuf->b_ml.ml_locked_निचला;
			    lnum <= curbuf->b_ml.ml_locked_ऊपरी; ++i, ++lnum)
	    यदि ((dp->db_index[i]) & DB_MARKED)
	    {
		(dp->db_index[i]) &= DB_INDEX_MASK;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		lowest_marked = lnum + 1;
		प्रत्यागम lnum;
	    }
    }

    प्रत्यागम (linenr_T) 0;
}

/*
 * clear all DB_MARKED flags
 */
    शून्य
ml_clearmarked(शून्य)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    पूर्णाङ्क		i;

    यदि (curbuf->b_ml.ml_mfp == रिक्त)	    // nothing to do
	प्रत्यागम;

    /*
     * The search starts with line lowest_marked.
     */
    प्रति (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_पंक्ति_गिनती; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	यदि ((hp = ml_find_पंक्ति(curbuf, lnum, ML_FIND)) == रिक्त)
	    प्रत्यागम;		// give error message?

	dp = (DATA_BL *)(hp->bh_data);

	प्रति (i = lnum - curbuf->b_ml.ml_locked_निचला;
			    lnum <= curbuf->b_ml.ml_locked_ऊपरी; ++i, ++lnum)
	    यदि ((dp->db_index[i]) & DB_MARKED)
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
    स्थिर शून्य
ml_flush_पंक्ति(buf_T *buf)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    char_u	*new_पंक्ति;
    char_u	*old_पंक्ति;
    colnr_T	new_len;
    पूर्णाङ्क		old_len;
    पूर्णाङ्क		extra;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		start;
    पूर्णाङ्क		गिनती;
    पूर्णाङ्क		i;
    स्थिर पूर्णाङ्क  entered = FALSE;

    यदि (buf->b_ml.ml_पंक्ति_lnum == 0 || buf->b_ml.ml_mfp == रिक्त)
	प्रत्यागम;		// nothing to do

    यदि (buf->b_ml.ml_flags & ML_पंक्ति_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	यदि (entered)
	    प्रत्यागम;
	entered = TRUE;

	lnum = buf->b_ml.ml_पंक्ति_lnum;
	new_पंक्ति = buf->b_ml.ml_पंक्ति_ptr;

	hp = ml_find_पंक्ति(buf, lnum, ML_FIND);
	यदि (hp == रिक्त)
	    siemsg(e_cannot_find_पंक्ति_nr, lnum);
	अन्यथा
	{
	    dp = (DATA_BL *)(hp->bh_data);
	    idx = lnum - buf->b_ml.ml_locked_निचला;
	    start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	    old_पंक्ति = (char_u *)dp + start;
	    यदि (idx == 0)	// line is last in block
		old_len = dp->db_txt_end - start;
	    अन्यथा		// text of previous line follows
		old_len = (dp->db_index[idx - 1] & DB_INDEX_MASK) - start;
	    new_len = buf->b_ml.ml_पंक्ति_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    यदि ((पूर्णाङ्क)dp->db_free >= extra)
	    {
#यदि defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		पूर्णाङ्क old_prop_len = 0;
		यदि (buf->b_has_textprop)
		    old_prop_len = old_len - (पूर्णाङ्क)STRLEN(old_पंक्ति) - 1;
#endif
		// if the length changes and there are following lines
		गिनती = buf->b_ml.ml_locked_ऊपरी - buf->b_ml.ml_locked_निचला + 1;
		यदि (extra != 0 && idx < गिनती - 1)
		{
		    // move text of following lines
		    mch_memmove((वर्ण *)dp + dp->db_txt_start - extra,
				(वर्ण *)dp + dp->db_txt_start,
				(size_t)(start - dp->db_txt_start));

		    // adjust pointers of this and following lines
		    प्रति (i = idx + 1; i < गिनती; ++i)
			dp->db_index[i] -= extra;
		}
		dp->db_index[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_start -= extra;

		// copy new line into the data block
		mch_memmove(old_पंक्ति - extra, new_पंक्ति, (size_t)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#यदि defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		यदि (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (पूर्णाङ्क)STRLEN(new_पंक्ति) - 1;
		}
		यदि (extra != 0)
		    ml_updatechunk(buf, lnum, (long)extra, ML_CHNK_UPDLINE);
#endif
	    }
	    अन्यथा
	    {
		/*
		 * Cannot do it in one data block: Delete and append.
		 * Append first, because ml_delete_int() cannot delete the
		 * last line in a buffer, which causes trouble for a buffer
		 * that has only one line.
		 * Don't forget to copy the mark!
		 */
		// How about handling errors???
		(शून्य)ml_append_int(buf, lnum, new_पंक्ति, new_len,
			 ((dp->db_index[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#ifdef FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#endif
			 );
		(शून्य)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_पंक्ति);

	entered = FALSE;
    }
    अन्यथा यदि (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_पंक्ति_ptr);

    buf->b_ml.ml_flags &= ~(ML_पंक्ति_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_पंक्ति_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    स्थिर bhdr_T *
ml_new_data(memfile_T *mfp, पूर्णाङ्क negative, पूर्णाङ्क page_गिनती)
{
    bhdr_T	*hp;
    DATA_BL	*dp;

    यदि ((hp = mf_new(mfp, negative, page_गिनती)) == रिक्त)
	प्रत्यागम रिक्त;

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_id = DATA_ID;
    dp->db_txt_start = dp->db_txt_end = page_गिनती * mfp->mf_page_size;
    dp->db_free = dp->db_txt_start - HEADER_SIZE;
    dp->db_पंक्ति_गिनती = 0;

    प्रत्यागम hp;
}

/*
 * create a new, empty, pointer block
 */
    स्थिर bhdr_T *
ml_new_ptr(memfile_T *mfp)
{
    bhdr_T	*hp;
    PTR_BL	*pp;

    यदि ((hp = mf_new(mfp, FALSE, 1)) == रिक्त)
	प्रत्यागम रिक्त;

    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_id = PTR_ID;
    pp->pb_गिनती = 0;
    pp->pb_गिनती_max = PB_गिनती_MAX(mfp);

    प्रत्यागम hp;
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
    स्थिर bhdr_T *
ml_find_पंक्ति(buf_T *buf, linenr_T lnum, पूर्णाङ्क action)
{
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    पूर्णाङ्क		dirty;
    linenr_T	निचला, ऊपरी;
    पूर्णाङ्क		शीर्ष;
    पूर्णाङ्क		page_गिनती;
    पूर्णाङ्क		idx;

    mfp = buf->b_ml.ml_mfp;

    /*
     * If there is a locked block check if the wanted line is in it.
     * If not, flush and release the locked block.
     * Don't do this for ML_INSERT_SAME, because the stack need to be updated.
     * Don't do this for ML_FLUSH, because we want to flush the locked block.
     * Don't do this when 'swapfile' is reset, we want to load all the blocks.
     */
    यदि (buf->b_ml.ml_locked)
    {
	यदि (ML_SIMPLE(action)
		&& buf->b_ml.ml_locked_निचला <= lnum
		&& buf->b_ml.ml_locked_ऊपरी >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    यदि (action == ML_जोड़ो)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_ऊपरी);
	    }
	    अन्यथा यदि (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_ऊपरी);
	    }
	    प्रत्यागम (buf->b_ml.ml_locked);
	}

	mf_put(mfp, buf->b_ml.ml_locked, buf->b_ml.ml_flags & ML_LOCKED_DIRTY,
					    buf->b_ml.ml_flags & ML_LOCKED_POS);
	buf->b_ml.ml_locked = रिक्त;

	/*
	 * If lines have been added or deleted in the locked block, need to
	 * update the line count in pointer blocks.
	 */
	यदि (buf->b_ml.ml_locked_lineadd != 0)
	    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
    }

    यदि (action == ML_FLUSH)	    // nothing else to do
	प्रत्यागम रिक्त;

    bnum = 1;			    // start at the root of the tree
    page_गिनती = 1;
    निचला = 1;
    ऊपरी = buf->b_ml.ml_पंक्ति_गिनती;

    यदि (action == ML_FIND)	// first try stack entries
    {
	प्रति (शीर्ष = buf->b_ml.ml_ढेर_शीर्ष - 1; शीर्ष >= 0; --शीर्ष)
	{
	    ip = &(buf->b_ml.ml_ढेर[शीर्ष]);
	    यदि (ip->ip_निचला <= lnum && ip->ip_ऊपरी >= lnum)
	    {
		bnum = ip->ip_bnum;
		निचला = ip->ip_निचला;
		ऊपरी = ip->ip_ऊपरी;
		buf->b_ml.ml_ढेर_शीर्ष = शीर्ष;	// truncate stack at prev entry
		भङ्ग;
	    }
	}
	यदि (शीर्ष < 0)
	    buf->b_ml.ml_ढेर_शीर्ष = 0;		// not found, start at the root
    }
    अन्यथा	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_ढेर_शीर्ष = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    प्रति (;;)
    {
	यदि ((hp = mf_get(mfp, bnum, page_गिनती)) == रिक्त)
	    गच्छ error_noblock;

	/*
	 * update high for insert/delete
	 */
	यदि (action == ML_जोड़ो)
	    ++ऊपरी;
	अन्यथा यदि (action == ML_DELETE)
	    --ऊपरी;

	dp = (DATA_BL *)(hp->bh_data);
	यदि (dp->db_id == DATA_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_निचला = निचला;
	    buf->b_ml.ml_locked_ऊपरी = ऊपरी;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    प्रत्यागम hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	यदि (pp->pb_id != PTR_ID)
	{
	    iemsg(e_pointer_block_id_wrong);
	    गच्छ error_block;
	}

	यदि ((शीर्ष = ml_add_ढेर(buf)) < 0)	// add new entry to stack
	    गच्छ error_block;
	ip = &(buf->b_ml.ml_ढेर[शीर्ष]);
	ip->ip_bnum = bnum;
	ip->ip_निचला = निचला;
	ip->ip_ऊपरी = ऊपरी;
	ip->ip_index = -1;		// index not known yet

	dirty = FALSE;
	प्रति (idx = 0; idx < (पूर्णाङ्क)pp->pb_गिनती; ++idx)
	{
	    t = pp->pb_pointer[idx].pe_पंक्ति_गिनती;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    यदि ((निचला += t) > lnum)
	    {
		ip->ip_index = idx;
		bnum = pp->pb_pointer[idx].pe_bnum;
		page_गिनती = pp->pb_pointer[idx].pe_page_गिनती;
		ऊपरी = निचला - 1;
		निचला -= t;

		/*
		 * a negative block number may have been changed
		 */
		यदि (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    यदि (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_pointer[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		भङ्ग;
	    }
	}
	यदि (idx >= (पूर्णाङ्क)pp->pb_गिनती)	    // past the end: something wrong!
	{
	    यदि (lnum > buf->b_ml.ml_पंक्ति_गिनती)
		siemsg(e_पंक्ति_संख्या_out_of_range_nr_past_the_end,
					      lnum - buf->b_ml.ml_पंक्ति_गिनती);

	    अन्यथा
		siemsg(e_पंक्ति_गिनती_wrong_in_block_nr, bnum);
	    गच्छ error_block;
	}
	यदि (action == ML_DELETE)
	{
	    pp->pb_pointer[idx].pe_पंक्ति_गिनती--;
	    dirty = TRUE;
	}
	अन्यथा यदि (action == ML_जोड़ो)
	{
	    pp->pb_pointer[idx].pe_पंक्ति_गिनती++;
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
    यदि (action == ML_DELETE)
	ml_lineadd(buf, 1);
    अन्यथा यदि (action == ML_जोड़ो)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_ढेर_शीर्ष = 0;
    प्रत्यागम रिक्त;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    स्थिर पूर्णाङ्क
ml_add_ढेर(buf_T *buf)
{
    पूर्णाङ्क		शीर्ष;
    infoptr_T	*newstack;

    शीर्ष = buf->b_ml.ml_ढेर_शीर्ष;

    // may have to increase the stack size
    यदि (शीर्ष == buf->b_ml.ml_ढेर_size)
    {
	CHECK(शीर्ष > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_ढेर_size + ढेर_INCR);
	यदि (newstack == रिक्त)
	    प्रत्यागम -1;
	यदि (शीर्ष > 0)
	    mch_memmove(newstack, buf->b_ml.ml_ढेर,
					     (size_t)शीर्ष * परिमाण(infoptr_T));
	vim_free(buf->b_ml.ml_ढेर);
	buf->b_ml.ml_ढेर = newstack;
	buf->b_ml.ml_ढेर_size += ढेर_INCR;
    }

    buf->b_ml.ml_ढेर_शीर्ष++;
    प्रत्यागम शीर्ष;
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
    स्थिर शून्य
ml_lineadd(buf_T *buf, पूर्णाङ्क गिनती)
{
    पूर्णाङ्क		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    प्रति (idx = buf->b_ml.ml_ढेर_शीर्ष - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_ढेर[idx]);
	यदि ((hp = mf_get(mfp, ip->ip_bnum, 1)) == रिक्त)
	    भङ्ग;
	pp = (PTR_BL *)(hp->bh_data);	// must be pointer block
	यदि (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_pointer_block_id_wrong_two);
	    भङ्ग;
	}
	pp->pb_pointer[ip->ip_index].pe_पंक्ति_गिनती += गिनती;
	ip->ip_ऊपरी += गिनती;
	mf_put(mfp, hp, TRUE, FALSE);
    }
}

#यदि defined(HAVE_READLINK)
/*
 * Resolve a symlink in the last component of a file name.
 * Note that f_resolve() does it for every part of the path, we don't do that
 * here.
 * If it worked returns OK and the resolved link in "buf[MAXPATHL]".
 * Otherwise returns FAIL.
 */
    पूर्णाङ्क
resolve_symlink(char_u *fname, char_u *buf)
{
    char_u	tmp[MAXPATHL];
    पूर्णाङ्क		ret;
    पूर्णाङ्क		depth = 0;

    यदि (fname == रिक्त)
	प्रत्यागम FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    प्रति (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	यदि (++depth == 100)
	{
	    semsg(_(e_symlink_loop_for_str), fname);
	    प्रत्यागम FAIL;
	}

	ret = readlink((वर्ण *)tmp, (वर्ण *)buf, MAXPATHL - 1);
	यदि (ret <= 0)
	{
	    यदि (errno == EINVAL || errno == ENOENT)
	    {
		// Found non-symlink or not existing file, stop here.
		// When at the first level use the unmodified name, skip the
		// call to vim_FullName().
		यदि (depth == 1)
		    प्रत्यागम FAIL;

		// Use the resolved name in tmp[].
		भङ्ग;
	    }

	    // There must be some error reading links, use original name.
	    प्रत्यागम FAIL;
	}
	buf[ret] = NUL;

	/*
	 * Check whether the symlink is relative or absolute.
	 * If it's relative, build a new path based on the directory
	 * portion of the filename (if any) and the path the symlink
	 * points to.
	 */
	यदि (mch_isFullName(buf))
	    STRCPY(tmp, buf);
	अन्यथा
	{
	    char_u *tail;

	    tail = gettail(tmp);
	    यदि (STRLEN(tail) + STRLEN(buf) >= MAXPATHL)
		प्रत्यागम FAIL;
	    STRCPY(tail, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    प्रत्यागम vim_FullName(tmp, buf, MAXPATHL, TRUE);
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
    यदि (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#endif

#यदि defined(UNIX) || defined(MSWIN)  // Need _very_ long file names
    पूर्णाङ्क		len = (पूर्णाङ्क)STRLEN(dir_name);

    s = dir_name + len;
    यदि (after_pathsep(dir_name, s) && len > 1 && s[-1] == s[-2])
    {			       // Ends with '//', Use Full path
	r = रिक्त;
	यदि ((s = make_percent_swname(dir_name, s, fname_res)) != रिक्त)
	{
	    r = modname(s, (char_u *)".swp", FALSE);
	    vim_free(s);
	}
	प्रत्यागम r;
    }
#endif

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#यदि defined(VMS)
	    "_swp",
#अन्यथा
	    ".swp",
#endif
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    यदि (r == रिक्त)	    // out of memory
	प्रत्यागम रिक्त;

    s = get_file_in_dir(r, dir_name);
    vim_free(r);
    प्रत्यागम s;
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

    यदि (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (size_t)(tail.string - fname) + tail.length);
    अन्यथा
    {
	size_t	dname_len = STRLEN(dname);

	यदि (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    यदि (tail.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    tail.string, tail.length, TRUE, &retval);
	    अन्यथा
	    {
		पूर्णाङ्क	    save_char;
		string_T    tmp;

		save_char = *tail.string;
		*tail.string = NUL;
		concat_fnames(fname, (size_t)(tail.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*tail.string = save_char;
		यदि (tmp.string == रिक्त)	    // out of memory
		    retval.string = रिक्त;
		अन्यथा
		{
		    concat_fnames(tmp.string, tmp.length,
			tail.string, tail.length, TRUE, &retval);
		    vim_free(tmp.string);
		}
	    }
	}
	अन्यथा
	    concat_fnames(dname, dname_len, tail.string, tail.length,
		TRUE, &retval);
    }

#ifdef MSWIN
    यदि (retval.string != रिक्त)
    {
	char_u	*t;

	प्रति (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    यदि (*t == ':')
		*t = '%';
    }
#endif

    प्रत्यागम retval.string;
}

/*
 * Print the ATTENTION message: info about an existing swap file.
 */
    स्थिर शून्य
attention_message(
    buf_T   *buf,	// buffer being edited
    char_u  *fname)	// swap file name
{
    stat_T	st;
    time_t	swap_mtime;

    ++no_wait_return;
    (शून्य)emsg(_(e_attention));
    संदेश_puts(_("\nFound a swap file by the name \""));
    संदेश_home_replace(fname);
    संदेश_puts("\"\n");
    swap_mtime = swapfile_info(fname);
    संदेश_puts(_("While opening file \""));
    संदेश_outtrans(buf->b_fname);
    संदेश_puts("\"\n");
    यदि (mch_stat((वर्ण *)buf->b_fname, &st) == -1)
    {
	संदेश_puts(_("      CANNOT BE FOUND"));
    }
    अन्यथा
    {
	संदेश_puts(_("             dated: "));
	संदेश_puts(get_ctime(st.st_mtime, TRUE));
	यदि (swap_mtime != 0 && st.st_mtime > swap_mtime)
	    संदेश_puts(_("      NEWER than swap file!\n"));
    }
    // Some of these messages are long to allow translation to
    // other languages.
    संदेश_puts(_("\n(1) Another program may be editing the same file.  If this is the case,\n    be careful not to end up with two different instances of the same\n    file when making changes.  Quit, or continue with caution.\n"));
    संदेश_puts(_("(2) An edit session for this file crashed.\n"));
    संदेश_puts(_("    If this is the case, use \":recover\" or \"vim -r "));
    संदेश_outtrans(buf->b_fname);
    संदेश_puts(_("\"\n    to recover the changes (see \":help recovery\").\n"));
    संदेश_puts(_("    If you did this already, delete the swap file \""));
    संदेश_outtrans(fname);
    संदेश_puts(_("\"\n    to avoid this message.\n"));
    cmdline_row = संदेश_row;
    --no_wait_return;
}

प्रकारनिर्देश गणना {
    SEA_CHOICE_NONE = 0,
    SEA_CHOICE_READONLY = 1,
    SEA_CHOICE_EDIT = 2,
    SEA_CHOICE_RECOVER = 3,
    SEA_CHOICE_DELETE = 4,
    SEA_CHOICE_QUIT = 5,
    SEA_CHOICE_ABORT = 6
} sea_choice_T;

#यदि defined(FEAT_EVAL)
/*
 * Trigger the SwapExists autocommands.
 * Returns a value for equivalent to do_dialog().
 */
    स्थिर sea_choice_T
do_swapexists(buf_T *buf, char_u *fname)
{
    set_vim_var_string(VV_SWAPNAME, fname, -1);
    set_vim_var_string(VV_SWAPCHOICE, रिक्त, -1);

    // Trigger SwapExists autocommands with <afile> set to the file being
    // edited.  Disallow changing directory here.
    ++allbuf_lock;
    apply_autocmds(EVENT_SWAPEXISTS, buf->b_fname, रिक्त, FALSE, रिक्त);
    --allbuf_lock;

    set_vim_var_string(VV_SWAPNAME, रिक्त, -1);

    विकल्प (*get_vim_var_str(VV_SWAPCHOICE))
    {
	पक्ष 'o': प्रत्यागम SEA_CHOICE_READONLY;
	पक्ष 'e': प्रत्यागम SEA_CHOICE_EDIT;
	पक्ष 'r': प्रत्यागम SEA_CHOICE_RECOVER;
	पक्ष 'd': प्रत्यागम SEA_CHOICE_DELETE;
	पक्ष 'q': प्रत्यागम SEA_CHOICE_QUIT;
	पक्ष 'a': प्रत्यागम SEA_CHOICE_ABORT;
    }

    प्रत्यागम SEA_CHOICE_NONE;
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
    स्थिर char_u *
findswapname(
    buf_T	*buf,
    char_u	**dirp,		// pointer to list of directories
    char_u	*old_fname)	// don't give warning for this file name
{
    char_u	*fname;
    पूर्णाङ्क		n;
    char_u	*dir_name;
#ifdef AMIGA
    BPTR	fh;
#endif
    पूर्णाङ्क		r;
    char_u	*buf_fname = buf->b_fname;

#यदि !defined(UNIX)
# define CREATE_DUMMY_FILE
    FILE	*dummyfd = रिक्त;

# ifdef MSWIN
    यदि (buf_fname != रिक्त && !mch_isFullName(buf_fname)
				       && vim_strchr(gettail(buf_fname), ':'))
    {
	char_u *t;

	buf_fname = vim_strsave(buf_fname);
	यदि (buf_fname == रिक्त)
	    buf_fname = buf->b_fname;
	अन्यथा
	    प्रति (t = gettail(buf_fname); *t != NUL; MB_PTR_ADV(t))
		यदि (*t == ':')
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
    यदि (!(buf->b_p_sn || buf->b_shortname) && buf_fname != रिक्त
					     && mch_getperm(buf_fname) < 0)
	dummyfd = mch_fopen((वर्ण *)buf_fname, "w");
#endif

    /*
     * Isolate a directory name from *dirp and put it in dir_name.
     * First allocate some memory to put the directory name in.
     */
    dir_name = alloc(STRLEN(*dirp) + 1);
    यदि (dir_name == रिक्त)
	*dirp = रिक्त;
    अन्यथा
	(शून्य)copy_option_part(dirp, dir_name, 31000, ",");

    /*
     * we try different names until we find one that does not exist yet
     */
    यदि (dir_name == रिक्त)	    // out of memory
	fname = रिक्त;
    अन्यथा
	fname = makeswapname(buf_fname, buf->b_ffname, buf, dir_name);

    प्रति (;;)
    {
	यदि (fname == रिक्त)	// must be out of memory
	    भङ्ग;
	यदि ((n = (पूर्णाङ्क)STRLEN(fname)) == 0)	// safety check
	{
	    VIM_CLEAR(fname);
	    भङ्ग;
	}
#यदि defined(UNIX)
/*
 * Some systems have a MS-DOS compatible filesystem that use 8.3 character
 * file names. If this is the first try and the swap file name does not fit in
 * 8.3, detect if this is the case, set shortname and try again.
 */
	यदि (fname[n - 2] == 'w' && fname[n - 1] == 'p'
					&& !(buf->b_p_sn || buf->b_shortname))
	{
	    char_u	    *tail;
	    char_u	    *fname2;
	    stat_T	    s1, s2;
	    पूर्णाङ्क		    f1, f2;
	    पूर्णाङ्क		    created1 = FALSE, created2 = FALSE;
	    पूर्णाङ्क		    same = FALSE;

	    /*
	     * Check if swapfile name does not fit in 8.3:
	     * It either contains two dots, is longer than 8 chars, or starts
	     * with a dot.
	     */
	    tail = gettail(buf_fname);
	    यदि (       vim_strchr(tail, '.') != रिक्त
		    || STRLEN(tail) > (size_t)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		यदि (fname2 != रिक्त)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    यदि (vim_strchr(tail, '.') != रिक्त)
			fname2[n - 1] = 'x';
		    अन्यथा यदि (*gettail(fname) == '.')
		    {
			fname2[n] = 'x';
			fname2[n + 1] = NUL;
		    }
		    अन्यथा
			fname2[n - 5] += 1;
		    /*
		     * may need to create the files to be able to use mch_stat()
		     */
		    f1 = mch_open((वर्ण *)fname, O_RDONLY | O_EXTRA, 0);
		    यदि (f1 < 0)
		    {
			f1 = mch_open_rw((वर्ण *)fname,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			created1 = TRUE;
		    }
		    यदि (f1 >= 0)
		    {
			f2 = mch_open((वर्ण *)fname2, O_RDONLY | O_EXTRA, 0);
			यदि (f2 < 0)
			{
			    f2 = mch_open_rw((वर्ण *)fname2,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			    created2 = TRUE;
			}
			यदि (f2 >= 0)
			{
			    /*
			     * Both files exist now. If mch_stat() returns the
			     * same device and inode they are the same file.
			     */
			    यदि (mch_fstat(f1, &s1) != -1
				    && mch_fstat(f2, &s2) != -1
				    && s1.st_dev == s2.st_dev
				    && s1.st_ino == s2.st_ino)
				same = TRUE;
			    close(f2);
			    यदि (created2)
				mch_remove(fname2);
			}
			close(f1);
			यदि (created1)
			    mch_remove(fname);
		    }
		    vim_free(fname2);
		    यदि (same)
		    {
			buf->b_shortname = true;
			vim_free(fname);
			fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
			अनुवर्त;	// try again with b_shortname set
		    }
		}
	    }
	}
#endif
	/*
	 * check if the swapfile already exists
	 */
	यदि (mch_getperm(fname) < 0)	// it does not exist
	{
#ifdef HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    यदि (mch_lstat((वर्ण *)fname, &sb) < 0)
#अन्यथा
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (long)MODE_NEWFILE);
	    /*
	     * on the Amiga mch_getperm() will return -1 when the file exists
	     * but is being used by another program. This happens if you edit
	     * a file twice.
	     */
	    यदि (fh != (BPTR)रिक्त)	// can open file, OK
	    {
		Close(fh);
		mch_remove(fname);
		भङ्ग;
	    }
	    यदि (IoErr() != ERROR_OBJECT_IN_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#endif
		भङ्ग;
	}

	/*
	 * A file name equal to old_fname is OK to use.
	 */
	यदि (old_fname != रिक्त && fnamecmp(fname, old_fname) == 0)
	    भङ्ग;

	/*
	 * get here when file already exists
	 */
	यदि (fname[n - 2] == 'w' && fname[n - 1] == 'p')	// first try
	{
	    /*
	     * on MS-DOS compatible filesystems (e.g. messydos) file.doc.swp
	     * and file.doc are the same file. To guess if this problem is
	     * present try if file.doc.swx exists. If it does, we set
	     * buf->b_shortname and try file_doc.swp (dots replaced by
	     * underscores for this file), and try again. If it doesn't we
	     * assume that "file.doc.swp" already exists.
	     */
	    यदि (!(buf->b_p_sn || buf->b_shortname))	// not tried yet
	    {
		fname[n - 1] = 'x';
		r = mch_getperm(fname);		// try "file.swx"
		fname[n - 1] = 'p';
		यदि (r >= 0)		    // "file.swx" seems to exist
		{
		    buf->b_shortname = true;
		    vim_free(fname);
		    fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
		    अनुवर्त;	    // try again with '.' replaced with '_'
		}
	    }
	    /*
	     * If we get here the ".swp" file really exists.
	     * Give an error message, unless recovering, no file name, we are
	     * viewing a help file or when the path of the file is different
	     * (happens when all .swp files are in one directory).
	     */
	    यदि (!recoverymode && buf_fname != रिक्त
				&& !buf->b_help
				&& !(buf->b_flags & (BF_DUMMY | BF_NO_SEA)))
	    {
		पूर्णाङ्क		fd;
		संरचना block0	b0;
		पूर्णाङ्क		differ = FALSE;

		/*
		 * Try to read block 0 from the swap file to get the original
		 * file name (and inode number).
		 */
		fd = mch_open((वर्ण *)fname, O_RDONLY | O_EXTRA, 0);
		यदि (fd >= 0)
		{
		    यदि (read_eintr(fd, &b0, परिमाण(b0)) == परिमाण(b0))
		    {
			/*
			 * If the swapfile has the same directory as the
			 * buffer don't compare the directory names, they can
			 * have a different mountpoint.
			 */
			यदि (b0.b0_flags & B0_SAME_DIR)
			{
			    यदि (fnamecmp(gettail(buf->b_ffname),
						   gettail(b0.b0_fname)) != 0
				    || !same_directory(fname, buf->b_ffname))
			    {
#ifdef CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				यदि (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
#endif
				    differ = TRUE;
			    }
			}
			अन्यथा
			{
			    /*
			     * The name in the swap file may be
			     * "~user/path/file".  Expand it first.
			     */
			    expand_env(b0.b0_fname, NameBuff, MAXPATHL);
#ifdef CHECK_INODE
			    यदि (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
				differ = TRUE;
#अन्यथा
			    यदि (fnamecmp(NameBuff, buf->b_ffname) != 0)
				differ = TRUE;
#endif
			}
		    }
		    close(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		यदि (differ == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == रिक्त)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#ifdef CREATE_DUMMY_FILE
		    पूर्णाङ्क		 did_use_dummy = FALSE;

		    // Avoid getting a warning for the file being created
		    // outside of Vim, it was created at the start of this
		    // function.  Delete the file now, because Vim might exit
		    // here if the window is closed.
		    यदि (dummyfd != रिक्त)
		    {
			fclose(dummyfd);
			dummyfd = रिक्त;
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
		    यदि (mch_stat((वर्ण *)buf->b_fname, &st) == 0
						  && swapfile_unchanged(fname))
		    {
			choice = SEA_CHOICE_DELETE;
			यदि (p_verbose > 0)
			    verb_संदेश(_("Found a swap file that is not useful, deleting it"));
		    }

#यदि defined(FEAT_EVAL)
		    /*
		     * If there is an SwapExists autocommand and we can handle
		     * the response, trigger it.  It may return 0 to ask the
		     * user anyway.
		     */
		    यदि (choice == SEA_CHOICE_NONE
			    && swap_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#endif

		    यदि (choice == SEA_CHOICE_NONE
					 && swap_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    यदि (choice == SEA_CHOICE_NONE)
		    {
#ifdef FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			यदि (gui.starting && !gui.in_use)
			    gui_start(रिक्त);
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

#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    यदि (swap_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    prefix = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			size_t	    message_size;
			string_T    message;
			char_u	    *tofree;
			पूर्णाङ्क	    dialog_result;

			prefix.length = STRLEN(prefix.string);
			suffix.length = STRLEN(suffix.string);
			message_size = prefix.length
				+ STRLEN(fname)
				+ suffix.length + 5;
			message.string = tofree = alloc(message_size);
			यदि (message.string != रिक्त)
			{
			    STRCPY(message.string, prefix.string);
			    message.length = prefix.length;
			    message.length += home_replace(रिक्त, fname,
				message.string + message.length, (पूर्णाङ्क)(message_size - message.length), TRUE);
			    STRCPY(message.string + message.length, suffix.string);
			}
			अन्यथा
			    message.string = (char_u *)_("Swap file already exists!");
			dialog_result = do_dialog(VIM_WARNING,
				    (char_u *)_("VIM - ATTENTION"),
				    message.string,
# ifdef HAVE_PROCESS_STILL_RUNNING
				    process_still_running
					? (char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Quit\n&Abort") :
# endif
					(char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Delete it\n&Quit\n&Abort"), 1, रिक्त, FALSE);

# ifdef HAVE_PROCESS_STILL_RUNNING
			यदि (process_still_running && dialog_result >= 4)
			    // compensate for missing "Delete it" button
			    dialog_result++;
# endif
			choice = dialog_result;
			vim_free(tofree);

			// pretend screen didn't scroll, need redraw anyway
			संदेश_scrolled = 0;
			redraw_all_later(UPD_NOT_VALID);
		    }
#endif

		    विकल्प (choice)
		    {
			पक्ष SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    भङ्ग;
			पक्ष SEA_CHOICE_EDIT:
			    भङ्ग;
			पक्ष SEA_CHOICE_RECOVER:
			    swap_exists_action = SEA_RECOVER;
			    भङ्ग;
			पक्ष SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    भङ्ग;
			पक्ष SEA_CHOICE_QUIT:
			    swap_exists_action = SEA_QUIT;
			    भङ्ग;
			पक्ष SEA_CHOICE_ABORT:
			    swap_exists_action = SEA_QUIT;
			    got_int = TRUE;
			    भङ्ग;
			पक्ष SEA_CHOICE_NONE:
			    संदेश_puts("\n");
			    यदि (संदेश_silent == 0)
				// call wait_return() later
				need_wait_return = TRUE;
			    भङ्ग;
		    }

		    // If the file was deleted this fname can be used.
		    यदि (choice != SEA_CHOICE_NONE && mch_getperm(fname) < 0)
			भङ्ग;

#ifdef CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    यदि (did_use_dummy)
			dummyfd = mch_fopen((वर्ण *)buf_fname, "w");
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
	यदि (fname[n - 1] == 'a')	// ".s?a"
	{
	    यदि (fname[n - 2] == 'a')    // ".saa": tried enough, give up
	    {
		emsg(_(e_too_many_swap_files_found));
		VIM_CLEAR(fname);
		भङ्ग;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#ifdef CREATE_DUMMY_FILE
    यदि (dummyfd != रिक्त)	// file has been created temporarily
    {
	fclose(dummyfd);
	mch_remove(buf_fname);
    }
#endif
#ifdef MSWIN
    यदि (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#endif
    प्रत्यागम fname;
}

    स्थिर पूर्णाङ्क
b0_magic_wrong(ZERO_BL *b0p)
{
    प्रत्यागम (b0p->b0_magic_long != (long)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (पूर्णाङ्क)B0_MAGIC_INT
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

    स्थिर पूर्णाङ्क
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
    पूर्णाङ्क		retval_c;	    // flag: buf_c valid
    पूर्णाङ्क		retval_s;	    // flag: buf_s valid

    यदि (mch_stat((वर्ण *)fname_c, &st) == 0)
	ino_c = (ino_t)st.st_ino;

    /*
     * First we try to get the inode from the file name, because the inode in
     * the swap file may be outdated.  If that fails (e.g. this path is not
     * valid on this machine), use the inode from block 0.
     */
    यदि (mch_stat((वर्ण *)fname_s, &st) == 0)
	ino_s = (ino_t)st.st_ino;
    अन्यथा
	ino_s = (ino_t)ino_block0;

    यदि (ino_c && ino_s)
	प्रत्यागम (ino_c != ino_s);

    /*
     * One of the inode numbers is unknown, try a forced vim_FullName() and
     * compare the file names.
     */
    retval_c = vim_FullName(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_FullName(fname_s, buf_s, MAXPATHL, TRUE);
    यदि (retval_c == OK && retval_s == OK)
	प्रत्यागम STRCMP(buf_c, buf_s) != 0;

    /*
     * Can't compare inodes or file names, guess that the files are different,
     * unless both appear not to exist at all, then compare with the file name
     * in the swap file.
     */
    यदि (ino_s == 0 && ino_c == 0 && retval_c == FAIL && retval_s == FAIL)
	प्रत्यागम STRCMP(fname_c, fname_s) != 0;
    प्रत्यागम TRUE;
}
#endif // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    स्थिर शून्य
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

    स्थिर long
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

    प्रत्यागम retval;
}

/*
 * Set the flags in the first block of the swap file:
 * - file is modified or not: buf->b_changed
 * - 'fileformat'
 * - 'fileencoding'
 */
    शून्य
ml_setflags(buf_T *buf)
{
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    यदि (!buf->b_ml.ml_mfp)
	प्रत्यागम;
    प्रति (hp = buf->b_ml.ml_mfp->mf_used_last; hp != रिक्त; hp = hp->bh_prev)
    {
	यदि (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_data);
	    b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_MASK)
						  | (get_fileformat(buf) + 1);
	    add_b0_fenc(b0p, buf);
	    hp->bh_flags |= BH_DIRTY;
	    mf_sync(buf->b_ml.ml_mfp, MFS_ZERO);
	    भङ्ग;
	}
    }
}

#यदि defined(FEAT_CRYPT)
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
    पूर्णाङ्क		text_len;
    cryptstate_T *state;

    यदि (dp->db_id != DATA_ID)
	प्रत्यागम data;

    state = ml_crypt_prepare(mfp, offset, FALSE);
    यदि (state == रिक्त)
	प्रत्यागम data;

    new_data = alloc(size);
    यदि (new_data == रिक्त)
    {
	crypt_free_state(state);
	प्रत्यागम रिक्त;
    }
    head_end = (char_u *)(&dp->db_index[dp->db_पंक्ति_गिनती]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = size - dp->db_txt_start;

    // Copy the header and the text.
    mch_memmove(new_data, dp, head_end - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, text_start, text_len, new_data + dp->db_txt_start,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    यदि (head_end < text_start)
	vim_memset(new_data + (head_end - data), 0, text_start - head_end);

    प्रत्यागम new_data;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    शून्य
ml_decrypt_data(
    memfile_T	*mfp,
    char_u	*data,
    off_T	offset,
    unsigned	size)
{
    DATA_BL	*dp = (DATA_BL *)data;
    char_u	*head_end;
    char_u	*text_start;
    पूर्णाङ्क		text_len;
    cryptstate_T *state;

    यदि (dp->db_id != DATA_ID)
	प्रत्यागम;

    head_end = (char_u *)(&dp->db_index[dp->db_पंक्ति_गिनती]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = dp->db_txt_end - dp->db_txt_start;

    यदि (head_end > text_start || dp->db_txt_start > size
	    || dp->db_txt_end > size)
	प्रत्यागम;  // data was messed up

    state = ml_crypt_prepare(mfp, offset, TRUE);
    यदि (state == रिक्त)
	प्रत्यागम;

    // Decrypt the text in place.
    crypt_decode_inplace(state, text_start, text_len, FALSE);
    crypt_free_state(state);
}

/*
 * Prepare for encryption/decryption, using the key, seed and offset.
 * Return an allocated cryptstate_T *.
 * Note: Encryption not supported for SODIUM
 */
    स्थिर cryptstate_T *
ml_crypt_prepare(memfile_T *mfp, off_T offset, पूर्णाङ्क reading)
{
    buf_T	*buf = mfp->mf_buffer;
    char_u	salt[50];
    पूर्णाङ्क		method_nr;
    char_u	*कुंजी;
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    यदि (reading && mfp->mf_old_कुंजी != रिक्त)
    {
	// Reading back blocks with the previous key/method/seed.
	method_nr = mfp->mf_old_cm;
	कुंजी = mfp->mf_old_कुंजी;
	arg.cat_seed = mfp->mf_old_seed;
    }
    अन्यथा
    {
	method_nr = crypt_get_method_nr(buf);
	कुंजी = buf->b_p_कुंजी;
	arg.cat_seed = mfp->mf_seed;
    }

    यदि (*कुंजी == NUL)
	प्रत्यागम रिक्त;

    यदि (crypt_may_close_swapfile(buf, कुंजी, method_nr))
	प्रत्यागम रिक्त;

    यदि (method_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((वर्ण *)salt, परिमाण(salt), "%s%ld", कुंजी, (long)offset);
	arg.cat_seed = रिक्त;
	arg.cat_init_from_file = FALSE;

	प्रत्यागम crypt_create(method_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((वर्ण *)salt, परिमाण(salt), "%ld", (long)offset);

    arg.cat_salt = salt;
    arg.cat_salt_len = (पूर्णाङ्क)STRLEN(salt);
    arg.cat_seed_len = MF_SEED_LEN;
    arg.cat_add_len = 0;
    arg.cat_add = रिक्त;
    arg.cat_init_from_file = FALSE;

    प्रत्यागम crypt_create(method_nr, कुंजी, &arg);
}

#endif


#यदि defined(FEAT_BYTEOFF)

# define MLCS_MAXL 800	// max no of lines in chunk
# define MLCS_MINL 400   // should be half of MLCS_MAXL

/*
 * Keep information for finding byte offset of a line, updtype may be one of:
 * ML_CHNK_ADDLINE: Add len to parent chunk, possibly splitting it
 *	   Careful: ML_CHNK_ADDLINE may cause ml_find_line() to be called.
 * ML_CHNK_DELLINE: Subtract len from parent chunk, possibly deleting it
 * ML_CHNK_UPDLINE: Add len to parent chunk, as a signed entity.
 */
    स्थिर शून्य
ml_updatechunk(
    buf_T	*buf,
    linenr_T	पंक्ति,
    long	len,
    पूर्णाङ्क		updtype)
{
    स्थिर buf_T	*ml_upd_lastbuf = रिक्त;
    स्थिर linenr_T	ml_upd_lastcurline;
    स्थिर पूर्णाङ्क		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    पूर्णाङ्क			curix = ml_upd_lastcurix;
    long		size;
    chunksize_T		*curchnk;
    पूर्णाङ्क			rest;
    bhdr_T		*hp;
    DATA_BL		*dp;

    यदि (buf->b_ml.ml_usedchunks == -1 || len == 0)
	प्रत्यागम;
    यदि (buf->b_ml.ml_chunksize == रिक्त)
    {
	buf->b_ml.ml_chunksize = ALLOC_MULT(chunksize_T, 100);
	यदि (buf->b_ml.ml_chunksize == रिक्त)
	{
	    buf->b_ml.ml_usedchunks = -1;
	    प्रत्यागम;
	}
	buf->b_ml.ml_numchunks = 100;
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = 1;
	ml_upd_lastbuf = रिक्त;		// invalidate resume cache
    }

    यदि (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_पंक्ति_गिनती == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (long)buf->b_ml.ml_पंक्ति_len;
	ml_upd_lastbuf = रिक्त;		// invalidate resume cache
	प्रत्यागम;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    यदि (buf != ml_upd_lastbuf || पंक्ति < curline)
    {
	curline = 1;
	curix = 0;
    }
    प्रति (; curix < buf->b_ml.ml_usedchunks - 1
	     && पंक्ति >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	     curix++)
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
    curchnk = buf->b_ml.ml_chunksize + curix;

    यदि (updtype == ML_CHNK_DELLINE)
	len = -len;
    curchnk->mlcs_totalsize += len;
    यदि (updtype == ML_CHNK_ADDLINE)
    {
	curchnk->mlcs_numlines++;

	// May resize here so we don't have to do it in both cases below
	यदि (buf->b_ml.ml_usedchunks + 1 >= buf->b_ml.ml_numchunks)
	{
	    chunksize_T *t_chunksize = buf->b_ml.ml_chunksize;

	    buf->b_ml.ml_numchunks = buf->b_ml.ml_numchunks * 3 / 2;
	    buf->b_ml.ml_chunksize = vim_realloc(buf->b_ml.ml_chunksize,
			    परिमाण(chunksize_T) * buf->b_ml.ml_numchunks);
	    यदि (buf->b_ml.ml_chunksize == रिक्त)
	    {
		// Hmmmm, Give up on offset for this buffer
		vim_free(t_chunksize);
		buf->b_ml.ml_usedchunks = -1;
		प्रत्यागम;
	    }
	}

	यदि (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MAXL)
	{
	    पूर्णाङ्क	    गिनती;	    // number of entries in block
	    पूर्णाङ्क	    idx;
	    पूर्णाङ्क	    end_idx;
	    पूर्णाङ्क	    text_end;
	    पूर्णाङ्क	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			परिमाण(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    size = 0;
	    linecnt = 0;
	    यावत् (curline < buf->b_ml.ml_पंक्ति_गिनती
			&& linecnt < MLCS_MINL)
	    {
		यदि ((hp = ml_find_पंक्ति(buf, curline, ML_FIND)) == रिक्त)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    प्रत्यागम;
		}
		dp = (DATA_BL *)(hp->bh_data);
		गिनती = (long)(buf->b_ml.ml_locked_ऊपरी) -
			(long)(buf->b_ml.ml_locked_निचला) + 1;
		idx = curline - buf->b_ml.ml_locked_निचला;
		curline = buf->b_ml.ml_locked_ऊपरी + 1;

		// compute index of last line to use in this MEMLINE
		rest = गिनती - idx;
		यदि (linecnt + rest > MLCS_MINL)
		{
		    end_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		अन्यथा
		{
		    end_idx = गिनती - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		यदि (buf->b_has_textprop)
		{
		    पूर्णाङ्क i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    प्रति (i = end_idx; i < idx; ++i)
			size += (पूर्णाङ्क)STRLEN((char_u *)dp
				      + (dp->db_index[i] & DB_INDEX_MASK)) + 1;
		}
		अन्यथा
# endif
		{
		    यदि (idx == 0) // first line in block, text at the end
			text_end = dp->db_txt_end;
		    अन्यथा
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
	    ml_upd_lastbuf = रिक्त;   // Force recalc of curix & curline
	    प्रत्यागम;
	}
	अन्यथा यदि (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_पंक्ति_गिनती - पंक्ति <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    यदि (पंक्ति == buf->b_ml.ml_पंक्ति_गिनती)
	    {
		curchnk->mlcs_numlines = 0;
		curchnk->mlcs_totalsize = 0;
	    }
	    अन्यथा
	    {
		/*
		 * Line is just prior to last, move count for last
		 * This is the common case  when loading a new file
		 */
		hp = ml_find_पंक्ति(buf, buf->b_ml.ml_पंक्ति_गिनती, ML_FIND);
		यदि (hp == रिक्त)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    प्रत्यागम;
		}
		dp = (DATA_BL *)(hp->bh_data);
		यदि (dp->db_पंक्ति_गिनती == 1)
		    rest = dp->db_txt_end - dp->db_txt_start;
		अन्यथा
		    rest =
			((dp->db_index[dp->db_पंक्ति_गिनती - 2]) & DB_INDEX_MASK)
			- dp->db_txt_start;
		curchnk->mlcs_totalsize = rest;
		curchnk->mlcs_numlines = 1;
		curchnk[-1].mlcs_totalsize -= rest;
		curchnk[-1].mlcs_numlines -= 1;
	    }
	}
    }
    अन्यथा यदि (updtype == ML_CHNK_DELLINE)
    {
	curchnk->mlcs_numlines--;
	यदि (curix < buf->b_ml.ml_usedchunks - 1
		&& curchnk->mlcs_numlines + curchnk[1].mlcs_numlines
								  <= MLCS_MINL)
	{
	    curix++;
	    curchnk = buf->b_ml.ml_chunksize + curix;
	}
	अन्यथा यदि (curix == 0 && curchnk->mlcs_numlines <= 0)
	{
	    ml_upd_lastbuf = रिक्त;   // Force recalc of curix & curline
	    buf->b_ml.ml_usedchunks--;
	    mch_memmove(buf->b_ml.ml_chunksize, buf->b_ml.ml_chunksize + 1,
			buf->b_ml.ml_usedchunks * परिमाण(chunksize_T));
	    प्रत्यागम;
	}
	अन्यथा यदि (curix == 0 || (curchnk->mlcs_numlines > 10
		    && curchnk->mlcs_numlines + curchnk[-1].mlcs_numlines
								  > MLCS_MINL))
	{
	    // The chunks are left as they are, the cached position stays
	    // valid.
	    ml_upd_lastbuf = buf;
	    ml_upd_lastcurline = curline;
	    ml_upd_lastcurix = curix;
	    प्रत्यागम;
	}

	// Collapse chunks
	ml_upd_lastbuf = रिक्त;   // Force recalc of curix & curline
	curchnk[-1].mlcs_numlines += curchnk->mlcs_numlines;
	curchnk[-1].mlcs_totalsize += curchnk->mlcs_totalsize;
	buf->b_ml.ml_usedchunks--;
	यदि (curix < buf->b_ml.ml_usedchunks)
	    mch_memmove(buf->b_ml.ml_chunksize + curix,
			buf->b_ml.ml_chunksize + curix + 1,
			(buf->b_ml.ml_usedchunks - curix) *
			परिमाण(chunksize_T));
	प्रत्यागम;
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
ml_find_पंक्ति_or_offset(buf_T *buf, linenr_T lnum, long *offp)
{
    linenr_T	curline;
    पूर्णाङ्क		curix;
    long	size;
    bhdr_T	*hp;
    DATA_BL	*dp;
    पूर्णाङ्क		गिनती;		// number of entries in block
    पूर्णाङ्क		idx;
    पूर्णाङ्क		start_idx;
    पूर्णाङ्क		text_end;
    long	offset;
    पूर्णाङ्क		len;
    पूर्णाङ्क		ffdos = (get_fileformat(buf) == EOL_DOS);
    पूर्णाङ्क		extra = 0;

    // take care of cached line first
    ml_flush_पंक्ति(curbuf);

    यदि (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == रिक्त
	    || lnum < 0)
	प्रत्यागम -1;

    यदि (offp == रिक्त)
	offset = 0;
    अन्यथा
	offset = *offp;
    यदि (lnum == 0 && offset <= 0)
	प्रत्यागम 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = size = 0;
    यावत् (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (offset != 0
	       && offset > size + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (long)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	size += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	यदि (offset && ffdos)
	    size += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    यावत् ((lnum != 0 && curline < lnum) || (offset != 0 && size < offset))
    {
# ifdef FEAT_PROP_POPUP
	size_t textprop_total = 0;
# endif

	यदि (curline > buf->b_ml.ml_पंक्ति_गिनती
		|| (hp = ml_find_पंक्ति(buf, curline, ML_FIND)) == रिक्त)
	    प्रत्यागम -1;
	dp = (DATA_BL *)(hp->bh_data);
	गिनती = (long)(buf->b_ml.ml_locked_ऊपरी) -
		(long)(buf->b_ml.ml_locked_निचला) + 1;
	start_idx = idx = curline - buf->b_ml.ml_locked_निचला;
	यदि (idx == 0)  // first line in block, text at the end
	    text_end = dp->db_txt_end;
	अन्यथा
	    text_end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);
	// Compute index of last line to use in this MEMLINE
	यदि (lnum != 0)
	{
	    यदि (curline + (गिनती - idx) >= lnum)
		idx += lnum - curline - 1;
	    अन्यथा
		idx = गिनती - 1;
	}
	अन्यथा
	{
	    extra = 0;
	    प्रति (;;)
	    {
# ifdef FEAT_PROP_POPUP
		size_t textprop_size = 0;

		यदि (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_index[idx]) & DB_INDEX_MASK);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_end
				  : ((dp->db_index[idx - 1]) & DB_INDEX_MASK));
		    textprop_size = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		यदि (!(offset >= size
			+ text_end - (पूर्णाङ्क)((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
			- (long)(textprop_total + textprop_size)
# endif
			+ ffdos))
		    भङ्ग;

		यदि (ffdos)
		    size++;
# ifdef FEAT_PROP_POPUP
		textprop_total += textprop_size;
# endif
		यदि (idx == गिनती - 1)
		{
		    extra = 1;
		    भङ्ग;
		}
		idx++;
	    }
	}
# ifdef FEAT_PROP_POPUP
	यदि (buf->b_has_textprop && lnum != 0)
	{
	    पूर्णाङ्क i;

	    // cannot use the db_index pointer, need to get the actual text
	    // lengths.
	    len = 0;
	    प्रति (i = start_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_index[i]) & DB_INDEX_MASK);
		len += (पूर्णाङ्क)STRLEN(p) + 1;
	    }
	}
	अन्यथा
# endif
	    len = text_end - ((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
				- (long)textprop_total
# endif
				;
	size += len;
	यदि (offset != 0 && size >= offset)
	{
	    यदि (size + ffdos == offset)
		*offp = 0;
	    अन्यथा यदि (idx == start_idx)
		*offp = offset - size + len;
	    अन्यथा
		*offp = offset - size + len
		     - (text_end - ((dp->db_index[idx - 1]) & DB_INDEX_MASK))
# ifdef FEAT_PROP_POPUP
		     + (long)textprop_total
# endif
		     ;
	    curline += idx - start_idx + extra;
	    यदि (curline > buf->b_ml.ml_पंक्ति_गिनती)
		प्रत्यागम -1;	// exactly one byte beyond the end
	    प्रत्यागम curline;
	}
	curline = buf->b_ml.ml_locked_ऊपरी + 1;
    }

    यदि (lnum != 0)
    {
	// Count extra CR characters.
	यदि (ffdos)
	    size += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	यदि ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_पंक्ति_गिनती)
	    size -= ffdos + 1;
    }

    प्रत्यागम size;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    शून्य
goto_byte(long cnt)
{
    long	boff = cnt;
    linenr_T	lnum;

    ml_flush_पंक्ति(curbuf);	// cached line may be dirty
    setpcmark();
    यदि (boff)
	--boff;
    lnum = ml_find_पंक्ति_or_offset(curbuf, (linenr_T)0, &boff);
    यदि (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_पंक्ति_गिनती;
	curwin->w_curswant = MAXCOL;
	coladvance((colnr_T)MAXCOL);
    }
    अन्यथा
    {
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.col = (colnr_T)boff;
	curwin->w_cursor.coladd = 0;
	curwin->w_set_curswant = true;
    }
    check_cursor();

    // Make sure the cursor is on the first byte of a multi-byte char.
    यदि (has_mbyte)
	mb_adjust_cursor();
}
#endif
