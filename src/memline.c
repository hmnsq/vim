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
#define CHECK(c, s)	करा { /**/ } पर्यंत (0)

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

#जर defined(SASC) || defined(__amigaos4__)
# include <proto/dos.h>	    // for Open() and Close()
#endif

प्रकारव्याख्या संरचना block0		ZERO_BL;    // contents of the first block
प्रकारव्याख्या संरचना pointer_block	PTR_BL;	    // contents of a pointer block
प्रकारव्याख्या संरचना data_block	DATA_BL;    // contents of a data block
प्रकारव्याख्या संरचना pointer_entry	PTR_EN;	    // block/line-count pair

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

#जर defined(FEAT_CRYPT)
स्थिर स्थिरांक पूर्णांक id1_codes[] = {
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
    linenr_T	pe_ओळ_मोजणी;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    पूर्णांक		pe_page_मोजणी;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
संरचना pointer_block
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_मोजणी;	// number of pointers in this block
    short_u	pb_मोजणी_max;	// maximum value for pb_count
    PTR_EN	pb_pointer[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#define PB_मोजणी_MAX(mfp) (short_u)(((mfp)->mf_page_size - दुरत्व(PTR_BL, pb_pointer)) / आकार(PTR_EN))

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
    अचिन्हित	db_free;	// free space available
    अचिन्हित	db_txt_start;	// byte where text starts
    अचिन्हित	db_txt_end;	// byte just after data block
    linenr_T	db_ओळ_मोजणी;	// number of lines in this block
    अचिन्हित	db_index[1];	// index for start of line (actually bigger)
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
#define DB_MARKED	((अचिन्हित)1 << ((आकार(अचिन्हित) * 8) - 1))
#define DB_INDEX_MASK	(~DB_MARKED)

#define INDEX_SIZE  (आकार(अचिन्हित))	    // size of one db_index entry
#define HEADER_SIZE (दुरत्व(DATA_BL, db_index))  // size of data block header

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
    दीर्घ	b0_magic_long;	// check for byte order of long
    पूर्णांक		b0_magic_int;	// check for byte order of int
    लघु	b0_magic_short;	// check for byte order of short
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

#define चळत_INCR	5	// nr of entries added to ml_stack at a time

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
#define ML_घाला	0x12	    // insert line
#define ML_FIND		0x13	    // just find the line
#define ML_FLUSH	0x02	    // flush locked block
#define ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
प्रकारव्याख्या गणना {
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
स्थिर समय_प्रकार swapfile_info(char_u *);
स्थिर पूर्णांक recov_file_names(char_u **, char_u *, पूर्णांक prepend_dot);
स्थिर char_u *findswapname(buf_T *, char_u **, char_u *);
स्थिर शून्य ml_flush_ओळ(buf_T *);
स्थिर bhdr_T *ml_new_data(memfile_T *, पूर्णांक, पूर्णांक);
स्थिर bhdr_T *ml_new_ptr(memfile_T *);
स्थिर bhdr_T *ml_find_ओळ(buf_T *, linenr_T, पूर्णांक);
स्थिर पूर्णांक ml_add_चळत(buf_T *);
स्थिर शून्य ml_lineadd(buf_T *, पूर्णांक);
स्थिर पूर्णांक b0_magic_wrong(ZERO_BL *);
#ifdef CHECK_INODE
स्थिर पूर्णांक fnamecmp_ino(char_u *, char_u *, दीर्घ);
#endif
स्थिर शून्य long_to_char(दीर्घ, char_u *);
स्थिर दीर्घ char_to_long(char_u *);
#ifdef FEAT_CRYPT
स्थिर cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T offset, पूर्णांक reading);
#endif
#ifdef FEAT_BYTEOFF
स्थिर शून्य ml_updatechunk(buf_T *buf, दीर्घ ओळ, दीर्घ len, पूर्णांक updtype);
#endif

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णांक
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
    buf->b_ml.ml_चळत_size = 0; // no stack yet
    buf->b_ml.ml_चळत = रिक्त;	// no stack yet
    buf->b_ml.ml_चळत_शीर्ष = 0;	// nothing in the stack
    buf->b_ml.ml_locked = रिक्त;	// no cached block
    buf->b_ml.ml_ओळ_lnum = 0;	// no cached line
#ifdef FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = रिक्त;
    buf->b_ml.ml_usedchunks = 0;
#endif

    जर (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    जर (p_uc && buf->b_p_swf)
	buf->b_may_swap = true;
    अन्यथा
	buf->b_may_swap = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(रिक्त, 0);
    जर (mfp == रिक्त)
	जा error;

    buf->b_ml.ml_mfp = mfp;
#ifdef FEAT_CRYPT
    mfp->mf_buffer = buf;
#endif
    buf->b_ml.ml_flags = ML_EMPTY;
    buf->b_ml.ml_ओळ_मोजणी = 1;

/*
 * fill block0 struct and write page 0
 */
    जर ((hp = mf_new(mfp, FALSE, 1)) == रिक्त)
	जा error;
    जर (hp->bh_bnum != 0)
    {
	iemsg(e_didnt_get_block_nr_zero);
	जा error;
    }
    b0p = (ZERO_BL *)(hp->bh_data);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (दीर्घ)B0_MAGIC_LONG;
    b0p->b0_magic_int = (पूर्णांक)B0_MAGIC_INT;
    b0p->b0_magic_short = (लघु)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_version, "VIM ", 4);
    STRNCPY(b0p->b0_version + 4, Version, 6);
    long_to_char((दीर्घ)mfp->mf_page_size, b0p->b0_page_size);

#ifdef FEAT_SPELL
    जर (!buf->b_spell)
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
    जर (!buf->b_help && !B_SPELL(buf))
	(शून्य)mf_sync(mfp, 0);

    /*
     * Fill in root pointer block and write page 1.
     */
    जर ((hp = ml_new_ptr(mfp)) == रिक्त)
	जा error;
    जर (hp->bh_bnum != 1)
    {
	iemsg(e_didnt_get_block_nr_one);
	जा error;
    }
    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_मोजणी = 1;
    pp->pb_pointer[0].pe_bnum = 2;
    pp->pb_pointer[0].pe_page_मोजणी = 1;
    pp->pb_pointer[0].pe_old_lnum = 1;
    pp->pb_pointer[0].pe_ओळ_मोजणी = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    जर ((hp = ml_new_data(mfp, FALSE, 1)) == रिक्त)
	जा error;
    जर (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_block_nr_two);
	जा error;
    }

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[0] = --dp->db_txt_start;	// at end of block
    dp->db_free -= 1 + INDEX_SIZE;
    dp->db_ओळ_मोजणी = 1;
    *((char_u *)dp + dp->db_txt_start) = NUL;	// empty line

    परत OK;

error:
    जर (mfp != रिक्त)
    {
	जर (hp)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, TRUE);	    // will also free(mfp->mf_fname)
    }
    buf->b_ml.ml_mfp = रिक्त;
    परत FAIL;
}

#जर defined(FEAT_CRYPT)
/*
 * Swapfile encryption is not supported by XChaCha20.  If this crypt method is
 * used then disable the swapfile, to avoid plain text being written to disk,
 * and return TRUE.
 * Otherwise return FALSE.
 */
    स्थिर पूर्णांक
crypt_may_close_swapfile(buf_T *buf, char_u *किल्ली, पूर्णांक method)
{
    जर (crypt_method_is_sodium(method) && *किल्ली != NUL)
    {
	mf_close_file(buf, TRUE);
	buf->b_p_swf = FALSE;
	परत TRUE;
    }
    परत FALSE;
}

/*
 * Prepare encryption for "buf" for the current key and method.
 */
    स्थिर शून्य
ml_set_mfp_crypt(buf_T *buf)
{
    जर (*buf->b_p_किल्ली == NUL)
	परत;

    पूर्णांक method_nr = crypt_get_method_nr(buf);

    जर (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_seed(buf->b_ml.ml_mfp->mf_seed, MF_SEED_LEN, रिक्त, 0);
    }
# ifdef FEAT_SODIUM
    अन्यथा जर (crypt_method_is_sodium(method_nr))
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
    जर (*buf->b_p_किल्ली == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    अन्यथा
    {
	पूर्णांक method_nr = crypt_get_method_nr(buf);

	b0p->b0_id[1] = id1_codes[method_nr];
	जर (method_nr > CRYPT_M_ZIP && method_nr < CRYPT_M_SOD)
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
ml_set_crypt_किल्ली(
    buf_T	*buf,
    char_u	*old_किल्ली,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    पूर्णांक		page_मोजणी;
    पूर्णांक		idx;
    दीर्घ	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    DATA_BL	*dp;
    blocknr_T	bnum;
    पूर्णांक		शीर्ष;
    पूर्णांक		old_method;

    जर (mfp == रिक्त || mfp->mf_fd < 0)
	परत;  // no memfile yet, nothing to do
    old_method = crypt_method_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    जर (crypt_may_close_swapfile(buf, buf->b_p_किल्ली, crypt_get_method_nr(buf)))
	परत;
# endif

    // First make sure the swapfile is in a consistent state, using the old
    // key and method.
    {
	char_u *new_किल्ली = buf->b_p_किल्ली;
	char_u *new_buf_cm = buf->b_p_cm;

	buf->b_p_किल्ली = old_किल्ली;
	buf->b_p_cm = old_cm;
	ml_preserve(buf, FALSE);
	buf->b_p_किल्ली = new_किल्ली;
	buf->b_p_cm = new_buf_cm;
    }

    // Set the key, method and seed to be used for reading, these must be the
    // old values.
    mfp->mf_old_किल्ली = old_किल्ली;
    mfp->mf_old_cm = old_method;
    जर (old_method > 0 && *old_किल्ली != NUL)
	mch_memmove(mfp->mf_old_seed, mfp->mf_seed, MF_SEED_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    जर (mfp->mf_infile_मोजणी > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_ओळ(buf);		    // flush buffered line
	(शून्य)ml_find_ओळ(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = रिक्त;
	bnum = 1;		// start with block 1
	page_मोजणी = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_चळत_शीर्ष = 0;
	VIM_CLEAR(buf->b_ml.ml_चळत);
	buf->b_ml.ml_चळत_size = 0;	// no stack yet

	साठी ( ; !got_int; ओळ_breakcheck())
	{
	    जर (hp != रिक्त)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    जर ((hp = mf_get(mfp, bnum, page_मोजणी)) == रिक्त)
	    {
		जर (bnum == 1)
		    थांबा;
		++error;
	    }
	    अन्यथा
	    {
		pp = (PTR_BL *)(hp->bh_data);
		जर (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    जर (pp->pb_मोजणी == 0)
		    {
			// empty block?
			++error;
		    }
		    अन्यथा जर (idx < (पूर्णांक)pp->pb_मोजणी)	// go a block deeper
		    {
			जर (pp->pb_pointer[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    सुरूठेवा;
			}

			// going one block deeper in the tree, new entry in
			//चळत
			जर ((शीर्ष = ml_add_चळत(buf)) < 0)
			{
			    ++error;
			    थांबा;		    // out of memory
			}
			ip = &(buf->b_ml.ml_चळत[शीर्ष]);
			ip->ip_bnum = bnum;
			ip->ip_index = idx;

			bnum = pp->pb_pointer[idx].pe_bnum;
			page_मोजणी = pp->pb_pointer[idx].pe_page_मोजणी;
			idx = 0;
			सुरूठेवा;
		    }
		}
		अन्यथा	    // not a pointer block
		{
		    dp = (DATA_BL *)(hp->bh_data);
		    जर (dp->db_id != DATA_ID)	// block id wrong
			++error;
		    अन्यथा
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = रिक्त;
		    }
		}
	    }

	    जर (buf->b_ml.ml_चळत_शीर्ष == 0)	// finished
		थांबा;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_चळत[--(buf->b_ml.ml_चळत_शीर्ष)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_index + 1;	    // go to next index
	    page_मोजणी = 1;
	}
	जर (hp != रिक्त)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	जर (error > 0)
	    emsg(_(e_error_while_updating_swap_file_crypt));
    }

    mfp->mf_old_किल्ली = रिक्त;
}
#endif

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
    शून्य
ml_setname(buf_T *buf)
{
    पूर्णांक		success = FALSE;
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;
#जर defined(MSWIN)
    char_u	*p;
#endif

    mfp = buf->b_ml.ml_mfp;
    जर (mfp->mf_fd < 0)		    // there is no swap file yet
    {
	/*
	 * When 'updatecount' is 0 and 'noswapfile' there is no swap file.
	 * For help files we will make a swap file now.
	 */
	जर (p_uc != 0 && (cmdmod.cmod_flags & CMOD_NOSWAPFILE) == 0)
	    ml_open_file(buf);	    // create a swap file
	परत;
    }

    /*
     * Try all directories in the 'directory' option.
     */
    dirp = p_dir;
    साठी (;;)
    {
	जर (*dirp == NUL)	    // tried all directories, fail
	    थांबा;
	fname = findswapname(buf, &dirp, mfp->mf_fname);
	// autocmd may have freed mfp if findswapname creates
	// different swapfile name
	जर (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    परत;
	}
						    // alloc's fname
	जर (dirp == रिक्त)	    // out of memory
	    थांबा;
	जर (fname == रिक्त)	    // no file name found for this dir
	    सुरूठेवा;

#जर defined(MSWIN)
	/*
	 * Set full pathname for swap file now, because a ":!cd dir" may
	 * change directory without us knowing it.
	 */
	p = FullName_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	जर (fname == रिक्त)
	    सुरूठेवा;
#endif
	// if the file name is the same we don't have to do anything
	जर (fnamecmp(fname, mfp->mf_fname) == 0)
	{
	    vim_free(fname);
	    success = TRUE;
	    थांबा;
	}
	// need to close the swap file before renaming
	जर (mfp->mf_fd >= 0)
	{
	    बंद(mfp->mf_fd);
	    mfp->mf_fd = -1;
	}

	// try to rename the swap file
	जर (vim_rename(mfp->mf_fname, fname) == 0)
	{
	    success = TRUE;
	    vim_free(mfp->mf_fname);
	    mfp->mf_fname = fname;
	    vim_free(mfp->mf_ffname);
#जर defined(MSWIN)
	    mfp->mf_ffname = रिक्त;  // mf_fname is full pathname already
#अन्यथा
	    mf_set_ffname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);
	    थांबा;
	}
	vim_free(fname);	    // this fname didn't work, try another
    }

    जर (mfp->mf_fd == -1)	    // need to (re)open the swap file
    {
	mfp->mf_fd = mch_open((अक्षर *)mfp->mf_fname, O_RDWR | O_EXTRA, 0);
	जर (mfp->mf_fd < 0)
	{
	    // could not (re)open the swap file, what can we do????
	    emsg(_(e_oops_lost_the_swap_file));
	    परत;
	}
#ifdef HAVE_FD_CLOEXEC
	{
	    पूर्णांक fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    जर (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(शून्य)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#endif
    }
    जर (!success)
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
	जर (!buf->b_p_ro || buf->b_changed)
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
    जर (mfp == रिक्त || mfp->mf_fd >= 0 || !buf->b_p_swf
				      || (cmdmod.cmod_flags & CMOD_NOSWAPFILE))
	परत;		// nothing to do

#ifdef FEAT_SPELL
    // For a spell buffer use a temp file name.
    जर (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	जर (fname != रिक्त)
	    (शून्य)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_swap = false;
	परत;
    }
#endif

    /*
     * Try all directories in 'directory' option.
     */
    dirp = p_dir;
    साठी (;;)
    {
	जर (*dirp == NUL)
	    थांबा;
	// There is a small chance that between choosing the swap file name
	// and creating it, another Vim creates the file.  In that case the
	// creation will fail and we will use another directory.
	fname = findswapname(buf, &dirp, रिक्त); // allocates fname
	// autocmd may have freed mfp, grr!
	जर (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    परत;
	}
	जर (dirp == रिक्त)
	    थांबा;  // out of memory
	जर (fname == रिक्त)
	    सुरूठेवा;
	जर (mf_open_file(mfp, fname) == OK)	// consumes fname!
	{
	    // don't sync yet in ml_sync_all()
	    mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;
#जर defined(MSWIN)
	    /*
	     * set full pathname for swap file now, because a ":!cd dir" may
	     * change directory without us knowing it.
	     */
	    mf_fullname(mfp);
#endif
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    जर (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_set_dirty(mfp);
		थांबा;
	    }
	    // Writing block 0 failed: close the file and try another dir
	    mf_close_file(buf, FALSE);
	}
    }

    जर (*p_dir != NUL && mfp->mf_fname == रिक्त)
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
    पूर्णांक	    newfile)		// reading file into new buffer
{
    पूर्णांक old_संदेश_silent = संदेश_silent; // might be reset by an E325 message

    जर (curbuf->b_may_swap && (!curbuf->b_p_ro || !newfile))
	ml_open_file(curbuf);
    संदेश_silent = old_संदेश_silent;
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
    शून्य
ml_close(buf_T *buf, पूर्णांक del_file)
{
    जर (buf->b_ml.ml_mfp == रिक्त)		// not open
	परत;
    mf_close(buf->b_ml.ml_mfp, del_file);	// close the .swp file
    जर (buf->b_ml.ml_ओळ_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_ओळ_ptr);
    vim_free(buf->b_ml.ml_चळत);
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
ml_close_all(पूर्णांक del_file)
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
	जर (!bufIsChanged(buf))
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
    स्थिर पूर्णांक
ml_check_b0_id(ZERO_BL *b0p)
{
    जर (b0p->b0_id[0] != BLOCK0_ID0
	    || (b0p->b0_id[1] != BLOCK0_ID1
		&& b0p->b0_id[1] != BLOCK0_ID1_C0
		&& b0p->b0_id[1] != BLOCK0_ID1_C1
		&& b0p->b0_id[1] != BLOCK0_ID1_C2
		&& b0p->b0_id[1] != BLOCK0_ID1_C3
		&& b0p->b0_id[1] != BLOCK0_ID1_C4)
	    )
	परत FAIL;
    परत OK;
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
    जर (mfp == रिक्त)
	परत;
    hp = mf_get(mfp, (blocknr_T)0, 1);
    जर (hp == रिक्त)
    {
#ifdef FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	जर (what == UB_CRYPT)
	    ml_set_mfp_crypt(buf);
#endif
	परत;
    }

    b0p = (ZERO_BL *)(hp->bh_data);
    जर (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_block_zero);
    अन्यथा
    {
	जर (what == UB_FNAME)
	    set_b0_fname(b0p, buf);
#ifdef FEAT_CRYPT
	अन्यथा जर (what == UB_CRYPT)
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

    जर (buf->b_ffname == रिक्त)
	b0p->b0_fname[0] = NUL;
    अन्यथा
    {
#जर defined(MSWIN) || defined(AMIGA)
	// Systems that cannot translate "~user" back into a path: copy the
	// file name unmodified.  Do use slashes instead of backslashes for
	// portability.
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_SIZE_CRYPT - 1);
# ifdef BACKSLASH_IN_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#अन्यथा
	माप_प्रकार	flen;
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
	जर (b0p->b0_fname[0] == '~')
	{
	    माप_प्रकार  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    जर (get_user_name(uname, B0_UNAME_SIZE) == FAIL
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
	जर (mch_stat((अक्षर *)buf->b_ffname, &st) >= 0)
	{
	    long_to_char((दीर्घ)st.st_mtime, b0p->b0_mtime);
#ifdef CHECK_INODE
	    long_to_char((दीर्घ)st.st_ino, b0p->b0_ino);
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
    जर (same_directory(buf->b_ml.ml_mfp->mf_fname, buf->b_ffname))
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
    पूर्णांक		n;
    पूर्णांक		size = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    जर (*buf->b_p_किल्ली != NUL)
	size = B0_FNAME_SIZE_CRYPT;
#endif

    n = (पूर्णांक)STRLEN(buf->b_p_fenc);
    जर ((पूर्णांक)STRLEN(b0p->b0_fname) + n + 1 > size)
	b0p->b0_flags &= ~B0_HAS_FENC;
    अन्यथा
    {
	mch_memmove((अक्षर *)b0p->b0_fname + size - n,
					    (अक्षर *)buf->b_p_fenc, (माप_प्रकार)n);
	*(b0p->b0_fname + size - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#जर defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#endif

#जर defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    स्थिर पूर्णांक
swapfile_process_running(ZERO_BL *b0p, char_u *swap_fname UNUSED)
{
# जर defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    संरचना sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    जर (mch_stat((अक्षर *)swap_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < समय(रिक्त) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	परत FALSE;
# endif
    परत mch_process_running(char_to_long(b0p->b0_pid));
}
#endif

/*
 * Try to recover curbuf from the .swp file.
 * If "checkext" is TRUE, check the extension and detect whether it is
 * a swap file.
 */
    शून्य
ml_recover(पूर्णांक checkext)
{
    buf_T	*buf = रिक्त;
    memfile_T	*mfp = रिक्त;
    char_u	*fname;
    char_u	*fname_used = रिक्त;
    bhdr_T	*hp = रिक्त;
    ZERO_BL	*b0p;
    पूर्णांक		b0_ff;
    char_u	*b0_fenc = रिक्त;
#ifdef FEAT_CRYPT
    पूर्णांक		b0_cm = -1;
#endif
    PTR_BL	*pp;
    DATA_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    पूर्णांक		page_मोजणी;
    stat_T	org_stat, swp_stat;
    पूर्णांक		len;
    पूर्णांक		directly;
    linenr_T	lnum;
    char_u	*p;
    पूर्णांक		i;
    दीर्घ	error;
    पूर्णांक		cannot_open;
    linenr_T	ओळ_मोजणी;
    पूर्णांक		has_error;
    पूर्णांक		idx;
    पूर्णांक		शीर्ष;
    पूर्णांक		txt_start;
    off_T	size;
    पूर्णांक		called_from_main;
    पूर्णांक		serious_error = TRUE;
    दीर्घ	mtime;
    पूर्णांक		attr;
    पूर्णांक		orig_file_status = NOTDONE;

    recoverymode = TRUE;
    called_from_main = (curbuf->b_ml.ml_mfp == रिक्त);
    attr = HL_ATTR(HLF_E);

    /*
     * If the file name ends in ".s[a-w][a-z]" we assume this is the swap file.
     * Otherwise a search is done to find the swap file(s).
     */
    fname = curbuf->b_fname;
    जर (fname == रिक्त)		    // When there is no file name
	fname = (char_u *)"";
    len = (पूर्णांक)STRLEN(fname);
    जर (checkext && len >= 4 &&
#जर defined(VMS)
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
	जर (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_swap_file_found_for_str), fname);
	    जा theend;
	}
	जर (len == 1)		    // one swap file found, use it
	    i = 1;
	अन्यथा			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (शून्य)recover_names(fname, TRUE, रिक्त, 0, रिक्त);
	    संदेश_putchar('\n');
	    संदेश_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_संख्या(FALSE, रिक्त);
	    जर (i < 1 || i > len)
		जा theend;
	}
	// get the swap file name that will be used
	(शून्य)recover_names(fname, FALSE, रिक्त, i, &fname_used);
    }
    जर (fname_used == रिक्त)
	जा theend;			// out of memory

    // When called from main() still need to initialize storage structure
    जर (called_from_main && ml_open(curbuf) == FAIL)
	getout(1);

    /*
     * Allocate a buffer structure for the swap file that is used for recovery.
     * Only the memline and crypt information in it are really used.
     */
    buf = ALLOC_ONE(buf_T);
    जर (buf == रिक्त)
	जा theend;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_चळत_size = 0;	// no stack yet
    buf->b_ml.ml_चळत = रिक्त;		// no stack yet
    buf->b_ml.ml_चळत_शीर्ष = 0;		// nothing in the stack
    buf->b_ml.ml_ओळ_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = रिक्त;		// no locked block
    buf->b_ml.ml_flags = 0;
#ifdef FEAT_CRYPT
    buf->b_p_किल्ली = empty_option;
    buf->b_p_cm = empty_option;
#endif

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_used); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_used, O_RDONLY);
    fname_used = p;
    जर (mfp == रिक्त || mfp->mf_fd < 0)
    {
	जर (fname_used != रिक्त)
	    semsg(_(e_cannot_open_str), fname_used);
	जा theend;
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
    जर ((hp = mf_get(mfp, (blocknr_T)0, 1)) == रिक्त)
    {
	संदेश_start();
	संदेश_puts_attr(_("Unable to read block 0 from "), attr | संदेश_HIST);
	संदेश_outtrans_attr(mfp->mf_fname, attr | संदेश_HIST);
	संदेश_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | संदेश_HIST);
	संदेश_end();
	जा theend;
    }
    b0p = (ZERO_BL *)(hp->bh_data);
    जर (STRNCMP(b0p->b0_version, "VIM 3.0", 7) == 0)
    {
	संदेश_start();
	संदेश_outtrans_attr(mfp->mf_fname, संदेश_HIST);
	संदेश_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    संदेश_HIST);
	संदेश_puts_attr(_("Use Vim version 3.0.\n"), संदेश_HIST);
	संदेश_end();
	जा theend;
    }
    जर (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_swap_file), mfp->mf_fname);
	जा theend;
    }
    जर (b0_magic_wrong(b0p))
    {
	संदेश_start();
	संदेश_outtrans_attr(mfp->mf_fname, attr | संदेश_HIST);
#जर defined(MSWIN)
	जर (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    संदेश_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | संदेश_HIST);
	अन्यथा
#endif
	    संदेश_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | संदेश_HIST);
	संदेश_puts_attr(_("The file was created on "), attr | संदेश_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	संदेश_puts_attr((अक्षर *)b0p->b0_hname, attr | संदेश_HIST);
	संदेश_puts_attr(_(",\nor the file has been damaged."), attr | संदेश_HIST);
	संदेश_end();
	जा theend;
    }

#ifdef FEAT_CRYPT
    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(id1_codes); ++i)
	जर (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    जर (b0_cm > 0)
	mch_memmove(mfp->mf_seed, &b0p->b0_seed, MF_SEED_LEN);
    crypt_set_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#अन्यथा
    जर (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_version_of_vim_does_not_support_encryption), mfp->mf_fname);
	जा theend;
    }
#endif

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    जर (mfp->mf_page_size != (अचिन्हित)char_to_long(b0p->b0_page_size))
    {
	अचिन्हित previous_page_size = mfp->mf_page_size;

	mf_new_page_size(mfp, (अचिन्हित)char_to_long(b0p->b0_page_size));
	जर (mfp->mf_page_size < previous_page_size)
	{
	    संदेश_start();
	    संदेश_outtrans_attr(mfp->mf_fname, attr | संदेश_HIST);
	    संदेश_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | संदेश_HIST);
	    संदेश_end();
	    जा theend;
	}
	जर ((size = vim_lseek(mfp->mf_fd, (off_T)0L, अंत_से)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	अन्यथा
	    mfp->mf_blocknr_max = (blocknr_T)(size / mfp->mf_page_size);
	mfp->mf_infile_मोजणी = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_page_size);
	जर (p == रिक्त)
	    जा theend;
	mch_memmove(p, hp->bh_data, previous_page_size);
	vim_free(hp->bh_data);
	hp->bh_data = p;
	b0p = (ZERO_BL *)(hp->bh_data);
    }

    /*
     * If .swp file name given directly, use name from swap file for buffer.
     */
    जर (directly)
    {
	expand_env(b0p->b0_fname, NameBuff, MAXPATHL);
	जर (setfname(curbuf, NameBuff, रिक्त, TRUE) == FAIL)
	    जा theend;
    }

    home_replace(रिक्त, mfp->mf_fname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Using swap file \"%s\""), NameBuff);

    जर (buf_spname(curbuf) != रिक्त)
	vim_strncpy(NameBuff, buf_spname(curbuf), MAXPATHL - 1);
    अन्यथा
	home_replace(रिक्त, curbuf->b_ffname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Original file \"%s\""), NameBuff);
    संदेश_putchar('\n');

    /*
     * check date of swap file and original file
     */
    mtime = char_to_long(b0p->b0_mtime);
    जर (curbuf->b_ffname != रिक्त
	    && mch_stat((अक्षर *)curbuf->b_ffname, &org_stat) != -1
	    && ((mch_stat((अक्षर *)mfp->mf_fname, &swp_stat) != -1
		    && org_stat.st_mtime > swp_stat.st_mtime)
		|| org_stat.st_mtime != mtime))
	emsg(_(e_warning_original_file_may_have_been_changed));
    out_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_MASK);
    जर (b0p->b0_flags & B0_HAS_FENC)
    {
	पूर्णांक fnsize = B0_FNAME_SIZE_NOCRYPT;

#ifdef FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	जर (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_SIZE_CRYPT;
#endif
	साठी (p = b0p->b0_fname + fnsize; p > b0p->b0_fname && p[-1] != NUL; --p)
	    ;
	b0_fenc = vim_strnsave(p, b0p->b0_fname + fnsize - p);
    }

    mf_put(mfp, hp, FALSE, FALSE);	// release block 0
    hp = रिक्त;

    /*
     * Now that we are sure that the file is going to be recovered, clear the
     * contents of the current buffer.
     */
    पर्यंत (!(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    जर (curbuf->b_ffname != रिक्त)
	orig_file_status = readfile(curbuf->b_ffname, रिक्त, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, रिक्त, READ_NEW);

#ifdef FEAT_CRYPT
    जर (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	जर (*curbuf->b_p_किल्ली != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_used);
	    संदेश_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    संदेश_puts(_("\nenter the new crypt key."));
	    संदेश_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    संदेश_puts(_("\nto use the same key for text file and swap file"));
	}
	अन्यथा
	    smsg(_(need_किल्ली_संदेश), fname_used);
	buf->b_p_किल्ली = crypt_get_किल्ली(FALSE, FALSE);
	जर (buf->b_p_किल्ली == रिक्त)
	    buf->b_p_किल्ली = curbuf->b_p_किल्ली;
	अन्यथा जर (*buf->b_p_किल्ली == NUL)
	{
	    vim_free(buf->b_p_किल्ली);
	    buf->b_p_किल्ली = curbuf->b_p_किल्ली;
	}
	जर (buf->b_p_किल्ली == रिक्त)
	    buf->b_p_किल्ली = empty_option;
    }
#endif

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    जर (b0_ff != 0)
	set_fileformat(b0_ff - 1, OPT_LOCAL);
    जर (b0_fenc != रिक्त)
    {
	set_option_मूल्य_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    page_मोजणी = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    ओळ_मोजणी = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_चळत_शीर्ष = 0;
    buf->b_ml.ml_चळत = रिक्त;
    buf->b_ml.ml_चळत_size = 0;	// no stack yet

    जर (curbuf->b_ffname == रिक्त)
	cannot_open = TRUE;
    अन्यथा
	cannot_open = FALSE;

    serious_error = FALSE;
    साठी ( ; !got_int; ओळ_breakcheck())
    {
	जर (hp != रिक्त)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	जर ((hp = mf_get(mfp, bnum, page_मोजणी)) == रिक्त)
	{
	    जर (bnum == 1)
	    {
		semsg(_(e_unable_to_read_block_one_from_str), mfp->mf_fname);
		जा theend;
	    }
	    ++error;
	    ml_append(lnum++, (char_u *)_("???MANY LINES MISSING"),
							    (colnr_T)0, TRUE);
	}
	अन्यथा		// there is a block
	{
	    pp = (PTR_BL *)(hp->bh_data);
	    जर (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		पूर्णांक ptr_block_error = FALSE;
		जर (pp->pb_मोजणी_max != PB_मोजणी_MAX(mfp))
		{
		    ptr_block_error = TRUE;
		    pp->pb_मोजणी_max = PB_मोजणी_MAX(mfp);
		}
		जर (pp->pb_मोजणी > pp->pb_मोजणी_max)
		{
		    ptr_block_error = TRUE;
		    pp->pb_मोजणी = pp->pb_मोजणी_max;
		}
		जर (ptr_block_error)
		    emsg(_(e_warning_pointer_block_corrupted));

		// check line count when using pointer block first time
		जर (idx == 0 && ओळ_मोजणी != 0)
		{
		    साठी (i = 0; i < (पूर्णांक)pp->pb_मोजणी; ++i)
			ओळ_मोजणी -= pp->pb_pointer[i].pe_ओळ_मोजणी;
		    जर (ओळ_मोजणी != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		जर (pp->pb_मोजणी == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		अन्यथा जर (idx < (पूर्णांक)pp->pb_मोजणी)	// go a block deeper
		{
		    जर (pp->pb_pointer[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			जर (!cannot_open)
			{
			    ओळ_मोजणी = pp->pb_pointer[idx].pe_ओळ_मोजणी;
			    linenr_T pe_old_lnum = pp->pb_pointer[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    जर (ओळ_मोजणी <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, रिक्त, lnum,
					pe_old_lnum - 1,
					ओळ_मोजणी, रिक्त, 0) != OK)
				cannot_open = TRUE;
			    अन्यथा
				lnum += ओळ_मोजणी;
			}
			जर (cannot_open)
			{
			    ++error;
			    ml_append(lnum++, (char_u *)_("???LINES MISSING"),
							    (colnr_T)0, TRUE);
			}
			++idx;	    // get same block again for next index
			सुरूठेवा;
		    }

		    /*
		     * going one block deeper in the tree
		     */
		    जर ((शीर्ष = ml_add_चळत(buf)) < 0)	// new entry in stack
		    {
			++error;
			थांबा;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_चळत[शीर्ष]);
		    ip->ip_bnum = bnum;
		    ip->ip_index = idx;

		    bnum = pp->pb_pointer[idx].pe_bnum;
		    ओळ_मोजणी = pp->pb_pointer[idx].pe_ओळ_मोजणी;
		    page_मोजणी = pp->pb_pointer[idx].pe_page_मोजणी;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    जर (page_मोजणी < 1
			    || bnum + page_मोजणी > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_index + 1;
			bnum = ip->ip_bnum;
			page_मोजणी = 1;
			--buf->b_ml.ml_चळत_शीर्ष;
			सुरूठेवा;
		    }
		    idx = 0;
		    सुरूठेवा;
		}
	    }
	    अन्यथा	    // not a pointer block
	    {
		dp = (DATA_BL *)(hp->bh_data);
		जर (dp->db_id != DATA_ID)	// block id wrong
		{
		    जर (bnum == 1)
		    {
			semsg(_(e_block_one_id_wrong_str_not_swp_file),
							       mfp->mf_fname);
			जा theend;
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
		    जर (hp->bh_page_मोजणी != page_मोजणी)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			page_मोजणी = hp->bh_page_मोजणी;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    जर (page_मोजणी * mfp->mf_page_size != dp->db_txt_end)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_end = page_मोजणी * mfp->mf_page_size;
		    }

		    जर (dp->db_txt_start < HEADER_SIZE
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
		    जर (ओळ_मोजणी != dp->db_ओळ_मोजणी)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    पूर्णांक did_questions = FALSE;
		    साठी (i = 0; i < dp->db_ओळ_मोजणी; ++i)
		    {
			जर ((char_u *)&(dp->db_index[i])
					    >= (char_u *)dp + dp->db_txt_start)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    थांबा;
			}

			txt_start = (dp->db_index[i] & DB_INDEX_MASK);
			जर (txt_start <= (पूर्णांक)HEADER_SIZE
					  || txt_start >= (पूर्णांक)dp->db_txt_end)
			{
			    ++error;
			    // avoid lots of lines with "???"
			    जर (did_questions)
				सुरूठेवा;
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
		    जर (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	जर (buf->b_ml.ml_चळत_शीर्ष == 0)	// finished
	    थांबा;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_चळत[--(buf->b_ml.ml_चळत_शीर्ष)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_index + 1;	    // go to next index
	page_मोजणी = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    जर (orig_file_status != OK || curbuf->b_ml.ml_ओळ_मोजणी != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	जर (!(curbuf->b_ml.ml_ओळ_मोजणी == 2 && *ml_get(1) == NUL))
	{
	    changed_internal();
	    ++CHANGEDTICK(curbuf);
	}
    }
    अन्यथा
    {
	साठी (idx = 1; idx <= lnum; ++idx)
	{
	    // Need to copy one line, fetching the other one may flush it.
	    p = vim_strnsave(ml_get(idx), ml_get_len(idx));
	    i = STRCMP(p, ml_get(idx + lnum));
	    vim_free(p);
	    जर (i != 0)
	    {
		changed_internal();
		++CHANGEDTICK(curbuf);
		थांबा;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    पर्यंत (curbuf->b_ml.ml_ओळ_मोजणी > lnum
				       && !(curbuf->b_ml.ml_flags & ML_EMPTY))
	ml_delete(curbuf->b_ml.ml_ओळ_मोजणी);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    जर (got_int)
	emsg(_(e_recovery_interrupted));
    अन्यथा जर (error)
    {
	++no_wait_return;
	संदेश(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_ओळी_starting_with_questions));
	--no_wait_return;
	संदेश(_("See \":help E312\" for more information."));
	संदेश(">>>>>>>>>>>>>");
    }
    अन्यथा
    {
	जर (curbuf->b_changed)
	{
	    संदेश(_("Recovery completed. You should check if everything is OK."));
	    संदेश_puts(_("\n(You might want to write out this file under another name\n"));
	    संदेश_puts(_("and run diff with the original file to check for changes)"));
	}
	अन्यथा
	    संदेश(_("Recovery completed. Buffer contents equals file contents."));
	संदेश_puts(_("\nYou may want to delete the .swp file now."));
#जर defined(UNIX) || defined(MSWIN)
	जर (swapfile_process_running(b0p, fname_used))
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
    जर (*buf->b_p_किल्ली != NUL && STRCMP(curbuf->b_p_किल्ली, buf->b_p_किल्ली) != 0)
    {
	संदेश_puts(_("Using crypt key from swap file for the text file.\n"));
	set_option_मूल्य_give_err((char_u *)"key", 0L, buf->b_p_किल्ली, OPT_LOCAL);
    }
#endif
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_used);
    recoverymode = FALSE;
    जर (mfp != रिक्त)
    {
	जर (hp != रिक्त)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    जर (buf != रिक्त)
    {
#ifdef FEAT_CRYPT
	जर (buf->b_p_किल्ली != curbuf->b_p_किल्ली)
	    free_string_option(buf->b_p_किल्ली);
	free_string_option(buf->b_p_cm);
#endif
	vim_free(buf->b_ml.ml_चळत);
	vim_free(buf);
    }
    जर (serious_error && called_from_main)
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
    पूर्णांक
recover_names(
    char_u	*fname,		// base for swap file name
    पूर्णांक		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list UNUSED, // when not NULL add file names to it
    पूर्णांक		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_out)	// result when "nr" > 0
{
    पूर्णांक		अंक_names;
    char_u	*(names[6]);
    char_u	*p;
    पूर्णांक		अंक_files;
    पूर्णांक		file_मोजणी = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = रिक्त;
#ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#endif

    जर (fname != रिक्त)
    {
#ifdef HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	जर (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	अन्यथा
#endif
	    fname_res = fname;
    }

    जर (do_list)
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
    पर्यंत (dir_name.string != रिक्त && *dirp)
    {
	/*
	 * Isolate a directory name from *dirp and put it in dir_name (we know
	 * it is large enough, so use 31000 for length).
	 * Advance dirp to next directory name.
	 */
	dir_name.length = (माप_प्रकार)copy_option_part(&dirp, dir_name.string, 31000, ",");

	जर (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    जर (fname == रिक्त)
	    {
#ifdef VMS
		names[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#अन्यथा
		names[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#endif
#जर defined(UNIX) || defined(MSWIN)
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
	    जर (fname == रिक्त)
	    {
		string_T    ret;

#ifdef VMS
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#अन्यथा
		names[0] = concat_fnames(dir_name.string, dir_name.length,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#endif
#जर defined(UNIX) || defined(MSWIN)
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

#जर defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.length;
		जर (after_pathsep(dir_name.string, p) && dir_name.length > 1 && p[-1] == p[-2])
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
		जर (tail == रिक्त)
		    अंक_names = 0;
		अन्यथा
		{
		    अंक_names = recov_file_names(names, tail, FALSE);
		    vim_free(tail);
		}
	    }
	}

	// check for out-of-memory
	साठी (पूर्णांक i = 0; i < अंक_names; ++i)
	{
	    जर (names[i] == रिक्त)
	    {
		साठी (i = 0; i < अंक_names; ++i)
		    vim_free(names[i]);
		अंक_names = 0;
	    }
	}
	जर (अंक_names == 0)
	    अंक_files = 0;
	अन्यथा जर (expand_wildcards(अंक_names, names, &अंक_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    अंक_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	जर (*dirp == NUL && file_मोजणी + अंक_files == 0 && fname != रिक्त)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#जर defined(VMS)
			       (char_u *)"_swp", FALSE
#अन्यथा
			       (char_u *)".swp", TRUE
#endif
			      );
	    जर (swapname != रिक्त)
	    {
		जर (mch_stat((अक्षर *)swapname, &st) != -1)    // It exists!
		{
		    files = ALLOC_ONE(char_u *);
		    जर (files != रिक्त)
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
	जर (curbuf->b_ml.ml_mfp != रिक्त
			       && (p = curbuf->b_ml.ml_mfp->mf_fname) != रिक्त
			       && ret_list == रिक्त)
	{
	    साठी (पूर्णांक i = 0; i < अंक_files; ++i)
		// Do not expand wildcards, on windows would try to expand
		// "%tmp%" in "%tmp%file".
		जर (fullpathcmp(p, files[i], TRUE, FALSE) & FPC_SAME)
		{
		    // Remove the name from files[i].  Move further entries
		    // down.  When the array becomes empty free it here, since
		    // FreeWild() won't be called below.
		    vim_free(files[i]);
		    जर (--अंक_files == 0)
			vim_free(files);
		    अन्यथा
			साठी ( ; i < अंक_files; ++i)
			    files[i] = files[i + 1];
		}
	}
	जर (nr > 0)
	{
	    file_मोजणी += अंक_files;
	    जर (nr <= file_मोजणी)
	    {
		*fname_out = vim_strsave(
				      files[nr - 1 + अंक_files - file_मोजणी]);
		dirp = (char_u *)"";		    // stop searching
	    }
	}
	अन्यथा जर (do_list)
	{
	    जर (dir_name.string[0] == '.' && dir_name.string[1] == NUL)
	    {
		जर (fname == रिक्त)
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

	    जर (अंक_files)
	    {
		साठी (पूर्णांक i = 0; i < अंक_files; ++i)
		{
		    // print the swap file name
		    संदेश_outnum((दीर्घ)++file_मोजणी);
		    संदेश_puts(".    ");
		    संदेश_puts((अक्षर *)gettail(files[i]));
		    संदेश_putchar('\n');
		    (शून्य)swapfile_info(files[i]);
		}
	    }
	    अन्यथा
		संदेश_puts(_("      -- none --\n"));
	    out_flush();
	}
#ifdef FEAT_EVAL
	अन्यथा जर (ret_list != रिक्त)
	{
	    string_T	name;

	    साठी (पूर्णांक i = 0; i < अंक_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.length,
		    files[i], STRLEN(files[i]), TRUE, &name);
		जर (name.string != रिक्त)
		{
		    list_append_string(ret_list, name.string, (पूर्णांक)name.length);
		    vim_free(name.string);
		}
	    }
	}
#endif
	अन्यथा
	    file_मोजणी += अंक_files;

	साठी (पूर्णांक i = 0; i < अंक_names; ++i)
	    vim_free(names[i]);
	जर (अंक_files > 0)
	    FreeWild(अंक_files, files);
    }
    vim_free(dir_name.string);
    परत file_मोजणी;
}

#जर defined(UNIX) || defined(MSWIN)
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
    जर (fixed_fname.string == रिक्त)
	परत रिक्त;

    साठी (p = fixed_fname.string; *p != NUL; MB_PTR_ADV(p))
	जर (vim_ispathsep(*p))
	    *p = '%';
    fixed_fname.length = (माप_प्रकार)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_end[-1];
    *p = NUL;
    concat_fnames(dir, (माप_प्रकार)(p - dir), fixed_fname.string, fixed_fname.length, TRUE, &d);
    vim_free(fixed_fname.string);

    परत d.string;
}
#endif

#जर (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
स्थिर पूर्णांक process_still_running;
#endif

#जर defined(FEAT_EVAL)
/*
 * Return information found in swapfile "fname" in dictionary "d".
 * This is used by the swapinfo() function.
 */
    शून्य
get_b0_dict(char_u *fname, dict_T *d)
{
    पूर्णांक fd;
    संरचना block0 b0;

    जर ((fd = mch_open((अक्षर *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
    {
	जर (read_eintr(fd, &b0, आकार(b0)) == आकार(b0))
	{
	    जर (ml_check_b0_id(&b0) == FAIL)
		dict_add_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    अन्यथा जर (b0_magic_wrong(&b0))
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
	बंद(fd);
    }
    अन्यथा
	dict_add_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#endif

/*
 * Give information about an existing swap file.
 * Returns timestamp (0 when unknown).
 */
    स्थिर समय_प्रकार
swapfile_info(char_u *fname)
{
    stat_T	    st;
    पूर्णांक		    fd;
    संरचना block0   b0;
#ifdef UNIX
    char_u	    uname[B0_UNAME_SIZE];
#endif

    // print the swap file date
    जर (mch_stat((अक्षर *)fname, &st) != -1)
    {
#ifdef UNIX
	// print name of owner of the file
	जर (mch_get_uname(st.st_uid, uname, B0_UNAME_SIZE) == OK)
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
    fd = mch_open((अक्षर *)fname, O_RDONLY | O_EXTRA, 0);
    जर (fd >= 0)
    {
	जर (read_eintr(fd, &b0, आकार(b0)) == आकार(b0))
	{
	    जर (STRNCMP(b0.b0_version, "VIM 3.0", 7) == 0)
	    {
		संदेश_puts(_("         [from Vim version 3.0]"));
	    }
	    अन्यथा जर (ml_check_b0_id(&b0) == FAIL)
	    {
		संदेश_puts(_("         [does not look like a Vim swap file]"));
	    }
	    अन्यथा
	    {
		संदेश_puts(_("         file name: "));
		जर (b0.b0_fname[0] == NUL)
		    संदेश_puts(_("[No Name]"));
		अन्यथा
		    संदेश_outtrans(b0.b0_fname);

		संदेश_puts(_("\n          modified: "));
		संदेश_puts(b0.b0_dirty ? _("YES") : _("no"));

		जर (*(b0.b0_uname) != NUL)
		{
		    संदेश_puts(_("\n         user name: "));
		    संदेश_outtrans(b0.b0_uname);
		}

		जर (*(b0.b0_hname) != NUL)
		{
		    जर (*(b0.b0_uname) != NUL)
			संदेश_puts(_("   host name: "));
		    अन्यथा
			संदेश_puts(_("\n         host name: "));
		    संदेश_outtrans(b0.b0_hname);
		}

		जर (char_to_long(b0.b0_pid) != 0L)
		{
		    संदेश_puts(_("\n        process ID: "));
		    संदेश_outnum(char_to_long(b0.b0_pid));
#जर defined(UNIX) || defined(MSWIN)
		    जर (swapfile_process_running(&b0, fname))
		    {
			संदेश_puts(_(" (STILL RUNNING)"));
# ifdef HAVE_PROCESS_STILL_RUNNING
			process_still_running = TRUE;
# endif
		    }
#endif
		}

		जर (b0_magic_wrong(&b0))
		{
#जर defined(MSWIN)
		    जर (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			संदेश_puts(_("\n         [not usable with this version of Vim]"));
		    अन्यथा
#endif
			संदेश_puts(_("\n         [not usable on this computer]"));
		}
	    }
	}
	अन्यथा
	    संदेश_puts(_("         [cannot be read]"));
	बंद(fd);
    }
    अन्यथा
	संदेश_puts(_("         [cannot be opened]"));
    संदेश_putchar('\n');

    परत st.st_mtime;
}

/*
 * Return TRUE if the swap file looks OK and there are no changes, thus it can
 * be safely deleted.
 */
    स्थिर पूर्णांक
swapfile_unchanged(char_u *fname)
{
    stat_T	    st;
    पूर्णांक		    fd;
    संरचना block0   b0;
    पूर्णांक		    ret = TRUE;

    // must be able to stat the swap file
    जर (mch_stat((अक्षर *)fname, &st) == -1)
	परत FALSE;

    // must be able to read the first block
    fd = mch_open((अक्षर *)fname, O_RDONLY | O_EXTRA, 0);
    जर (fd < 0)
	परत FALSE;
    जर (read_eintr(fd, &b0, आकार(b0)) != आकार(b0))
    {
	बंद(fd);
	परत FALSE;
    }

    // the ID and magic number must be correct
    जर (ml_check_b0_id(&b0) == FAIL|| b0_magic_wrong(&b0))
	ret = FALSE;

    // must be unchanged
    जर (b0.b0_dirty)
	ret = FALSE;

#जर defined(UNIX) || defined(MSWIN)
    // Host name must be known and must equal the current host name, otherwise
    // comparing pid is meaningless.
    जर (*(b0.b0_hname) == NUL)
    {
	ret = FALSE;
    }
    अन्यथा
    {
	char_u	    hostname[B0_HNAME_SIZE];

	mch_get_host_name(hostname, B0_HNAME_SIZE);
	hostname[B0_HNAME_SIZE - 1] = NUL;
	b0.b0_hname[B0_HNAME_SIZE - 1] = NUL; // in case of corruption
	जर (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    जर (char_to_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#endif

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    बंद(fd);
    परत ret;
}

    स्थिर पूर्णांक
recov_file_names(char_u **names, char_u *path, पूर्णांक prepend_dot)
{
    पूर्णांक		अंक_names;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    पूर्णांक		i;
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
    जर (prepend_dot)
    {
	names[अंक_names] = modname(path, (char_u *)".sw?", TRUE);
	जर (names[अंक_names] == रिक्त)
	    जा end;
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
    जर (names[अंक_names] == रिक्त)
	जा end;
    जर (अंक_names >= 1)	    // check if we have the same name twice
    {
	p = names[अंक_names - 1];
	i = (पूर्णांक)STRLEN(names[अंक_names - 1]) - (पूर्णांक)STRLEN(names[अंक_names]);
	जर (i > 0)
	    p += i;	    // file name has been expanded to full path

	जर (STRCMP(p, names[अंक_names]) != 0)
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
    जर (names[अंक_names] == रिक्त)
	जा end;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = names[अंक_names];
    i = STRLEN(names[अंक_names]) - STRLEN(names[अंक_names - 1]);
    जर (i > 0)
	p += i;		// file name has been expanded to full path
    जर (STRCMP(names[अंक_names - 1], p) == 0)
	vim_free(names[अंक_names]);
    अन्यथा
	++अंक_names;
#endif

end:
#ifndef MSWIN
    curbuf->b_shortname = shortname;
#endif

    परत अंक_names;
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
    शून्य
ml_sync_all(पूर्णांक check_file, पूर्णांक check_char)
{
    buf_T		*buf;
    stat_T		st;

    FOR_ALL_BUFFERS(buf)
    {
	जर (buf->b_ml.ml_mfp == रिक्त
		|| buf->b_ml.ml_mfp->mf_fname == रिक्त
		|| buf->b_ml.ml_mfp->mf_fd < 0)
	    सुरूठेवा;			    // no file

#ifdef FEAT_CRYPT
	जर (crypt_may_close_swapfile(buf, buf->b_p_किल्ली,
						     crypt_get_method_nr(buf)))
	    सुरूठेवा;
#endif

	ml_flush_ओळ(buf);		    // flush buffered line
					    // flush locked block
	(शून्य)ml_find_ओळ(buf, (linenr_T)0, ML_FLUSH);
	जर (bufIsChanged(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != रिक्त)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    जर (mch_stat((अक्षर *)buf->b_ffname, &st) == -1
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
	जर (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (शून्य)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (bufIsChanged(buf) ? MFS_FLUSH : 0));
	    जर (check_char && ui_char_avail())	// character available now
		थांबा;
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
ml_preserve(buf_T *buf, पूर्णांक message)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    पूर्णांक		status;
    पूर्णांक		got_int_save = got_int;

    जर (mfp == रिक्त || mfp->mf_fname == रिक्त)
    {
	जर (message)
	    emsg(_(e_cannot_preserve_there_is_no_swap_file));
	परत;
    }
#ifdef FEAT_CRYPT
    जर (crypt_may_close_swapfile(buf, buf->b_p_किल्ली, crypt_get_method_nr(buf)))
	परत;
#endif

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_ओळ(buf);				    // flush buffered line
    (शून्य)ml_find_ओळ(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    status = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

    // stack is invalid after mf_sync(.., MFS_ALL)
    buf->b_ml.ml_चळत_शीर्ष = 0;

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
    जर (mf_need_trans(mfp) && !got_int)
    {
	lnum = 1;
	पर्यंत (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_ओळ_मोजणी)
	{
	    hp = ml_find_ओळ(buf, lnum, ML_FIND);
	    जर (hp == रिक्त)
	    {
		status = FAIL;
		जा theend;
	    }
	    CHECK(buf->b_ml.ml_locked_खालचा != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_वरचा + 1;
	}
	(शून्य)ml_find_ओळ(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	जर (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    status = FAIL;
	buf->b_ml.ml_चळत_शीर्ष = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    जर (message)
    {
	जर (status == OK)
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
    परत ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
    char_u *
ml_get_pos(pos_T *pos)
{
    परत (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->col);
}

/*
 * Return pointer to cursor line.
 */
    char_u *
ml_get_curline(शून्य)
{
    परत ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE);
}

/*
 * Return pointer to cursor position.
 */
    char_u *
ml_get_cursor(शून्य)
{
    परत (ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE) +
							curwin->w_cursor.col);
}

// return length (excluding the NUL) of the given line
    colnr_T
ml_get_len(linenr_T lnum)
{
    परत ml_get_buf_len(curbuf, lnum);
}

// return length (excluding the NUL) of the text after position "pos"
    colnr_T
ml_get_pos_len(pos_T *pos)
{
    परत ml_get_buf_len(curbuf, pos->lnum) - pos->col;
}

// return length (excluding the NUL) of the cursor line
    colnr_T
ml_get_curline_len(शून्य)
{
    परत ml_get_buf_len(curbuf, curwin->w_cursor.lnum);
}

// return length (excluding the NUL) of the cursor position
    colnr_T
ml_get_cursor_len(शून्य)
{
    परत ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.col;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*ओळ;

    जर (*(ओळ = ml_get_buf(buf, lnum, FALSE)) == NUL)
	परत 0;

    जर (buf->b_ml.ml_ओळ_textlen <= 0)
	buf->b_ml.ml_ओळ_textlen = (पूर्णांक)STRLEN(ओळ) + 1;
    परत buf->b_ml.ml_ओळ_textlen - 1;
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
    पूर्णांक		will_change)		// line will be changed
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    स्थिर पूर्णांक	recursive = 0;
    स्थिर char_u questions[4];

    जर (lnum > buf->b_ml.ml_ओळ_मोजणी)	// invalid line number
    {
	जर (recursive == 0)
	{
	    // Avoid giving this message for a recursive call, may happen when
	    // the GUI redraws part of the text.
	    ++recursive;
	    siemsg(e_ml_get_invalid_lnum_nr, lnum);
	    --recursive;
	}
	ml_flush_ओळ(buf);
errorret:
	STRCPY(questions, "???");
	buf->b_ml.ml_ओळ_len = 4;
	buf->b_ml.ml_ओळ_textlen = buf->b_ml.ml_ओळ_len;
	buf->b_ml.ml_ओळ_lnum = lnum;
	परत questions;
    }
    जर (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    जर (buf->b_ml.ml_mfp == रिक्त)	// there are no lines
    {
	buf->b_ml.ml_ओळ_len = 1;
	buf->b_ml.ml_ओळ_textlen = buf->b_ml.ml_ओळ_len;
	परत (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    जर (buf->b_ml.ml_ओळ_lnum != lnum || mf_dont_release)
    {
	अचिन्हित    start, end;
	पूर्णांक	    idx;

	ml_flush_ओळ(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	जर ((hp = ml_find_ओळ(buf, lnum, ML_FIND)) == रिक्त)
	{
	    जर (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_find_ओळ_nr_in_buffer_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    जा errorret;
	}

	dp = (DATA_BL *)(hp->bh_data);

	idx = lnum - buf->b_ml.ml_locked_खालचा;
	start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	जर (idx == 0)
	    end = dp->db_txt_end;
	अन्यथा
	    end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);

	buf->b_ml.ml_ओळ_ptr = (char_u *)dp + start;
	buf->b_ml.ml_ओळ_len = end - start;
#जर defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	जर (buf->b_has_textprop
			 && (माप_प्रकार)buf->b_ml.ml_ओळ_len > आकार(textprop_T))
	    buf->b_ml.ml_ओळ_textlen = 0;  // call STRLEN() later when needed
	अन्यथा
#endif
	    buf->b_ml.ml_ओळ_textlen = buf->b_ml.ml_ओळ_len;
	buf->b_ml.ml_ओळ_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_ओळ_DIRTY | ML_ALLOCATED);
    }
    जर (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#ifdef FEAT_EVAL
	जर (ml_get_alloc_ओळी && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_ओळ_DIRTY;
#endif
    }

#ifdef FEAT_EVAL
    जर (ml_get_alloc_ओळी
		 && (buf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_ओळ_len);

	// make sure the text is in allocated memory
	जर (p != रिक्त)
	{
	    स_हटाओ(p, buf->b_ml.ml_ओळ_ptr, buf->b_ml.ml_ओळ_len);
	    buf->b_ml.ml_ओळ_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    जर (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_ओळ_DIRTY;
	}
    }
#endif
    परत buf->b_ml.ml_ओळ_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    पूर्णांक
ml_ओळ_alloced(शून्य)
{
    परत (curbuf->b_ml.ml_flags & ML_ओळ_DIRTY);
}

#ifdef FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    स्थिर शून्य
add_text_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**ओळ,
	    पूर्णांक		*len,
	    char_u	**tofree)
{
    पूर्णांक		round;
    पूर्णांक		new_prop_मोजणी = 0;
    पूर्णांक		मोजणी;
    पूर्णांक		n;
    char_u	*props;
    पूर्णांक		new_len = 0;  // init for gcc
    char_u	*new_ओळ = रिक्त;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    साठी (round = 1; round <= 2; ++round)
    {
	जर (round == 2)
	{
	    uint16_t pc;

	    जर (new_prop_मोजणी == 0)
		परत;  // nothing to do
	    new_len = *len + (पूर्णांक)PROP_मोजणी_SIZE
			     + new_prop_मोजणी * (पूर्णांक)आकार(textprop_T);
	    new_ओळ = alloc(new_len);
	    जर (new_ओळ == रिक्त)
		परत;
	    mch_memmove(new_ओळ, *ओळ, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_मोजणी;
	    mch_memmove(new_ओळ + *len, &pc, PROP_मोजणी_SIZE);
	    new_prop_मोजणी = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	मोजणी = get_text_props(buf, lnum, &props, FALSE);
	साठी (n = 0; n < मोजणी; ++n)
	{
	    mch_memmove(&prop, props + n * आकार(textprop_T),
							   आकार(textprop_T));
	    जर (prop.tp_flags & TP_FLAG_CONT_NEXT)
	    {
		जर (round == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_PREV;
		    prop.tp_col = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_text_offset = 0;
		    mch_memmove(new_ओळ + *len + (पूर्णांक)PROP_मोजणी_SIZE
			    + new_prop_मोजणी * आकार(textprop_T),
			    &prop, आकार(textprop_T));
		}
		++new_prop_मोजणी;
	    }
	}
    }
    *ओळ = new_ओळ;
    *tofree = new_ओळ;
    *len = new_len;
}
#endif

    स्थिर पूर्णांक
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ओळ_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    पूर्णांक		flags)		// ML_APPEND_ flags
{
    char_u	*ओळ = ओळ_arg;
    colnr_T	len = len_arg;
    पूर्णांक		i;
    पूर्णांक		ओळ_मोजणी;	// number of indexes in current block
    पूर्णांक		offset;
    पूर्णांक		from, to;
    दीर्घ	space_needed;	// space needed for new line
    पूर्णांक		page_size;
    पूर्णांक		page_मोजणी;
    पूर्णांक		db_idx;		// index for lnum in data block
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
    पूर्णांक		ret = FAIL;

    जर (lnum > buf->b_ml.ml_ओळ_मोजणी || buf->b_ml.ml_mfp == रिक्त)
	परत FAIL;  // lnum out of range

    जर (lowest_marked && lowest_marked > lnum)
	lowest_marked = lnum + 1;

    जर (len == 0)
    {
	len = (colnr_T)STRLEN(ओळ) + 1;	// space needed for the text
#जर defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	text_len = len;
#endif
    }
#जर defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    अन्यथा जर (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	text_len = (colnr_T)STRLEN(ओळ) + 1;
    अन्यथा
	text_len = len;
#endif

#ifdef FEAT_PROP_POPUP
    जर (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	add_text_props_for_append(buf, lnum, &ओळ, &len, &tofree);
#endif

    space_needed = len + INDEX_SIZE;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    page_size = mfp->mf_page_size;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    जर ((hp = ml_find_ओळ(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_घाला)) == रिक्त)
	जा theend;

    buf->b_ml.ml_flags &= ~ML_EMPTY;

    जर (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    अन्यथा
	db_idx = lnum - buf->b_ml.ml_locked_खालचा;
		// get line count before the insertion
    ओळ_मोजणी = buf->b_ml.ml_locked_वरचा - buf->b_ml.ml_locked_खालचा;

    dp = (DATA_BL *)(hp->bh_data);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    जर ((दीर्घ)dp->db_free < space_needed && db_idx == ओळ_मोजणी - 1
					    && lnum < buf->b_ml.ml_ओळ_मोजणी)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_वरचा);
	जर ((hp = ml_find_ओळ(buf, lnum + 1, ML_घाला)) == रिक्त)
	    जा theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	ओळ_मोजणी = buf->b_ml.ml_locked_वरचा - buf->b_ml.ml_locked_खालचा;
	CHECK(buf->b_ml.ml_locked_खालचा != lnum + 1, "locked_low != lnum + 1");

	dp = (DATA_BL *)(hp->bh_data);
    }

    ++buf->b_ml.ml_ओळ_मोजणी;

    जर ((दीर्घ)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_start -= len;
	dp->db_free -= space_needed;
	++(dp->db_ओळ_मोजणी);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	जर (ओळ_मोजणी > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    जर (db_idx < 0)
		offset = dp->db_txt_end;
	    अन्यथा
		offset = ((dp->db_index[db_idx]) & DB_INDEX_MASK);
	    mch_memmove((अक्षर *)dp + dp->db_txt_start,
					  (अक्षर *)dp + dp->db_txt_start + len,
				 (माप_प्रकार)(offset - (dp->db_txt_start + len)));
	    साठी (i = ओळ_मोजणी - 1; i > db_idx; --i)
		dp->db_index[i + 1] = dp->db_index[i] - len;
	    dp->db_index[db_idx + 1] = offset - len;
	}
	अन्यथा
	    // add line at the end (which is the start of the text)
	    dp->db_index[db_idx + 1] = dp->db_txt_start;

	/*
	 * copy the text into the block
	 */
	mch_memmove((अक्षर *)dp + dp->db_index[db_idx + 1], ओळ, (माप_प्रकार)len);
	जर (flags & ML_APPEND_MARK)
	    dp->db_index[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	जर (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    अन्यथा	    // not enough space in data block
    {
	दीर्घ	    ओळ_मोजणी_डावा, ओळ_मोजणी_उजवा;
	पूर्णांक	    page_मोजणी_डावा, page_मोजणी_उजवा;
	bhdr_T	    *hp_डावा;
	bhdr_T	    *hp_उजवा;
	bhdr_T	    *hp_new;
	पूर्णांक	    ओळी_moved;
	पूर्णांक	    data_moved = 0;	    // init to shut up gcc
	पूर्णांक	    total_moved = 0;	    // init to shut up gcc
	DATA_BL	    *dp_उजवा, *dp_डावा;
	पूर्णांक	    चळत_idx;
	पूर्णांक	    in_डावा;
	पूर्णांक	    lineadd;
	blocknr_T   bnum_डावा, bnum_उजवा;
	linenr_T    lnum_डावा, lnum_उजवा;
	पूर्णांक	    pb_idx;
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
	जर (db_idx < 0)		// left block is new, right block is existing
	{
	    ओळी_moved = 0;
	    in_डावा = TRUE;
	    // space_needed does not change
	}
	अन्यथा			// left block is existing, right block is new
	{
	    ओळी_moved = ओळ_मोजणी - db_idx - 1;
	    जर (ओळी_moved == 0)
		in_डावा = FALSE;	// put new line in right block
					// space_needed does not change
	    अन्यथा
	    {
		data_moved = ((dp->db_index[db_idx]) & DB_INDEX_MASK) -
							    dp->db_txt_start;
		total_moved = data_moved + ओळी_moved * INDEX_SIZE;
		जर ((दीर्घ)dp->db_free + total_moved >= space_needed)
		{
		    in_डावा = TRUE;	// put new line in left block
		    space_needed = total_moved;
		}
		अन्यथा
		{
		    in_डावा = FALSE;	    // put new line in right block
		    space_needed += total_moved;
		}
	    }
	}

	page_मोजणी = ((space_needed + HEADER_SIZE) + page_size - 1) / page_size;
	जर ((hp_new = ml_new_data(mfp, flags & ML_APPEND_NEW, page_मोजणी))
								       == रिक्त)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_वरचा);
	    जा theend;
	}
	जर (db_idx < 0)		// left block is new
	{
	    hp_डावा = hp_new;
	    hp_उजवा = hp;
	    ओळ_मोजणी_डावा = 0;
	    ओळ_मोजणी_उजवा = ओळ_मोजणी;
	}
	अन्यथा			// right block is new
	{
	    hp_डावा = hp;
	    hp_उजवा = hp_new;
	    ओळ_मोजणी_डावा = ओळ_मोजणी;
	    ओळ_मोजणी_उजवा = 0;
	}
	dp_उजवा = (DATA_BL *)(hp_उजवा->bh_data);
	dp_डावा = (DATA_BL *)(hp_डावा->bh_data);
	bnum_डावा = hp_डावा->bh_bnum;
	bnum_उजवा = hp_उजवा->bh_bnum;
	page_मोजणी_डावा = hp_डावा->bh_page_मोजणी;
	page_मोजणी_उजवा = hp_उजवा->bh_page_मोजणी;

	/*
	 * May move the new line into the right/new block.
	 */
	जर (!in_डावा)
	{
	    dp_उजवा->db_txt_start -= len;
	    dp_उजवा->db_free -= len + INDEX_SIZE;
	    dp_उजवा->db_index[0] = dp_उजवा->db_txt_start;
	    जर (flags & ML_APPEND_MARK)
		dp_उजवा->db_index[0] |= DB_MARKED;

	    mch_memmove((अक्षर *)dp_उजवा + dp_उजवा->db_txt_start,
							   ओळ, (माप_प्रकार)len);
	    ++ओळ_मोजणी_उजवा;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	जर (ओळी_moved)
	{
	    dp_उजवा->db_txt_start -= data_moved;
	    dp_उजवा->db_free -= total_moved;
	    mch_memmove((अक्षर *)dp_उजवा + dp_उजवा->db_txt_start,
			(अक्षर *)dp_डावा + dp_डावा->db_txt_start,
			(माप_प्रकार)data_moved);
	    offset = dp_उजवा->db_txt_start - dp_डावा->db_txt_start;
	    dp_डावा->db_txt_start += data_moved;
	    dp_डावा->db_free += total_moved;

	    /*
	     * update indexes in the new block
	     */
	    साठी (to = ओळ_मोजणी_उजवा, from = db_idx + 1;
					 from < ओळ_मोजणी_डावा; ++from, ++to)
		dp_उजवा->db_index[to] = dp->db_index[from] + offset;
	    ओळ_मोजणी_उजवा += ओळी_moved;
	    ओळ_मोजणी_डावा -= ओळी_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	जर (in_डावा)
	{
	    dp_डावा->db_txt_start -= len;
	    dp_डावा->db_free -= len + INDEX_SIZE;
	    dp_डावा->db_index[ओळ_मोजणी_डावा] = dp_डावा->db_txt_start;
	    जर (flags & ML_APPEND_MARK)
		dp_डावा->db_index[ओळ_मोजणी_डावा] |= DB_MARKED;
	    mch_memmove((अक्षर *)dp_डावा + dp_डावा->db_txt_start,
							   ओळ, (माप_प्रकार)len);
	    ++ओळ_मोजणी_डावा;
	}

	जर (db_idx < 0)		// left block is new
	{
	    lnum_डावा = lnum + 1;
	    lnum_उजवा = 0;
	}
	अन्यथा			// right block is new
	{
	    lnum_डावा = 0;
	    जर (in_डावा)
		lnum_उजवा = lnum + 2;
	    अन्यथा
		lnum_उजवा = lnum + 1;
	}
	dp_डावा->db_ओळ_मोजणी = ओळ_मोजणी_डावा;
	dp_उजवा->db_ओळ_मोजणी = ओळ_मोजणी_उजवा;

	/*
	 * release the two data blocks
	 * The new one (hp_new) already has a correct blocknumber.
	 * The old one (hp, in ml_locked) gets a positive blocknumber if
	 * we changed it and we are not editing a new file.
	 */
	जर (ओळी_moved || in_डावा)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	जर (!(flags & ML_APPEND_NEW) && db_idx >= 0 && in_डावा)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_find_ओळ(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	साठी (चळत_idx = buf->b_ml.ml_चळत_शीर्ष - 1; चळत_idx >= 0;
								  --चळत_idx)
	{
	    ip = &(buf->b_ml.ml_चळत[चळत_idx]);
	    pb_idx = ip->ip_index;
	    जर ((hp = mf_get(mfp, ip->ip_bnum, 1)) == रिक्त)
		जा theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    जर (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		जा theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    जर (pp->pb_मोजणी < pp->pb_मोजणी_max)
	    {
		जर (pb_idx + 1 < (पूर्णांक)pp->pb_मोजणी)
		    mch_memmove(&pp->pb_pointer[pb_idx + 2],
				&pp->pb_pointer[pb_idx + 1],
			(माप_प्रकार)(pp->pb_मोजणी - pb_idx - 1) * आकार(PTR_EN));
		++pp->pb_मोजणी;
		pp->pb_pointer[pb_idx].pe_ओळ_मोजणी = ओळ_मोजणी_डावा;
		pp->pb_pointer[pb_idx].pe_bnum = bnum_डावा;
		pp->pb_pointer[pb_idx].pe_page_मोजणी = page_मोजणी_डावा;
		pp->pb_pointer[pb_idx + 1].pe_ओळ_मोजणी = ओळ_मोजणी_उजवा;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_उजवा;
		pp->pb_pointer[pb_idx + 1].pe_page_मोजणी = page_मोजणी_उजवा;

		जर (lnum_डावा != 0)
		    pp->pb_pointer[pb_idx].pe_old_lnum = lnum_डावा;
		जर (lnum_उजवा != 0)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_उजवा;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_चळत_शीर्ष = चळत_idx + 1;	    // truncate stack

		जर (lineadd)
		{
		    --(buf->b_ml.ml_चळत_शीर्ष);
		    // fix line count for rest of blocks in the stack
		    ml_lineadd(buf, lineadd);
							// fix stack itself
		    buf->b_ml.ml_चळत[buf->b_ml.ml_चळत_शीर्ष].ip_वरचा +=
								      lineadd;
		    ++(buf->b_ml.ml_चळत_शीर्ष);
		}

		/*
		 * We are finished, break the loop here.
		 */
		थांबा;
	    }
	    // pointer block full
	    /*
	     * split the pointer block
	     * allocate a new pointer block
	     * move some of the pointer into the new block
	     * prepare for updating the parent block
	     */
	    साठी (;;)	// do this twice when splitting block 1
	    {
		hp_new = ml_new_ptr(mfp);
		जर (hp_new == रिक्त)	    // TODO: try to fix tree
		    जा theend;
		pp_new = (PTR_BL *)(hp_new->bh_data);

		जर (hp->bh_bnum != 1)
		    थांबा;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (माप_प्रकार)page_size);
		pp->pb_मोजणी = 1;
		pp->pb_pointer[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_pointer[0].pe_ओळ_मोजणी = buf->b_ml.ml_ओळ_मोजणी;
		pp->pb_pointer[0].pe_old_lnum = 1;
		pp->pb_pointer[0].pe_page_मोजणी = 1;
		mf_put(mfp, hp, TRUE, FALSE);   // release block 1
		hp = hp_new;		// new block is to be split
		pp = pp_new;
		CHECK(चळत_idx != 0, _("stack_idx should be 0"));
		ip->ip_index = 0;
		++चळत_idx;	// do block 1 again later
	    }
	    /*
	     * move the pointers after the current one to the new block
	     * If there are none, the new entry will be in the new block.
	     */
	    total_moved = pp->pb_मोजणी - pb_idx - 1;
	    जर (total_moved)
	    {
		mch_memmove(&pp_new->pb_pointer[0],
				&pp->pb_pointer[pb_idx + 1],
				(माप_प्रकार)(total_moved) * आकार(PTR_EN));
		pp_new->pb_मोजणी = total_moved;
		pp->pb_मोजणी -= total_moved - 1;
		pp->pb_pointer[pb_idx + 1].pe_bnum = bnum_उजवा;
		pp->pb_pointer[pb_idx + 1].pe_ओळ_मोजणी = ओळ_मोजणी_उजवा;
		pp->pb_pointer[pb_idx + 1].pe_page_मोजणी = page_मोजणी_उजवा;
		जर (lnum_उजवा)
		    pp->pb_pointer[pb_idx + 1].pe_old_lnum = lnum_उजवा;
	    }
	    अन्यथा
	    {
		pp_new->pb_मोजणी = 1;
		pp_new->pb_pointer[0].pe_bnum = bnum_उजवा;
		pp_new->pb_pointer[0].pe_ओळ_मोजणी = ओळ_मोजणी_उजवा;
		pp_new->pb_pointer[0].pe_page_मोजणी = page_मोजणी_उजवा;
		pp_new->pb_pointer[0].pe_old_lnum = lnum_उजवा;
	    }
	    pp->pb_pointer[pb_idx].pe_bnum = bnum_डावा;
	    pp->pb_pointer[pb_idx].pe_ओळ_मोजणी = ओळ_मोजणी_डावा;
	    pp->pb_pointer[pb_idx].pe_page_मोजणी = page_मोजणी_डावा;
	    जर (lnum_डावा)
		pp->pb_pointer[pb_idx].pe_old_lnum = lnum_डावा;
	    lnum_डावा = 0;
	    lnum_उजवा = 0;

	    /*
	     * recompute line counts
	     */
	    ओळ_मोजणी_उजवा = 0;
	    साठी (i = 0; i < (पूर्णांक)pp_new->pb_मोजणी; ++i)
		ओळ_मोजणी_उजवा += pp_new->pb_pointer[i].pe_ओळ_मोजणी;
	    ओळ_मोजणी_डावा = 0;
	    साठी (i = 0; i < (पूर्णांक)pp->pb_मोजणी; ++i)
		ओळ_मोजणी_डावा += pp->pb_pointer[i].pe_ओळ_मोजणी;

	    bnum_डावा = hp->bh_bnum;
	    bnum_उजवा = hp_new->bh_bnum;
	    page_मोजणी_डावा = 1;
	    page_मोजणी_उजवा = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	जर (चळत_idx < 0)
	{
	    iemsg(e_updated_too_many_blocks);
	    buf->b_ml.ml_चळत_शीर्ष = 0;	// invalidate stack
	}
    }

#ifdef FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (दीर्घ)text_len
# अन्यथा
	    (दीर्घ)len
# endif
	    , ML_CHNK_ADDLINE);
#endif

#ifdef FEAT_NETBEANS_INTG
    जर (!(flags & ML_APPEND_NEW) && netbeans_active())
    {
	पूर्णांक ओळ_len = (पूर्णांक)STRLEN(ओळ);
	जर (ओळ_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, ओळ, ओळ_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)ओळ_len, (char_u *)"\n", 1);
    }
#endif
#ifdef FEAT_JOB_CHANNEL
    जर (!(flags & ML_APPEND_NEW) && buf->b_write_to_channel)
	channel_write_new_ओळी(buf);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    vim_free(tofree);
#endif
    परत ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    स्थिर पूर्णांक
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ओळ,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    पूर्णांक		flags)		// ML_APPEND_ flags
{
    जर (lnum > buf->b_ml.ml_ओळ_मोजणी)
	परत FAIL;  // lnum out of range

    जर (buf->b_ml.ml_ओळ_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_ओळ(buf);

#ifdef FEAT_EVAL
    जर (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	जर (buf->b_ml.ml_ओळ_lnum != 0)
	    ml_flush_ओळ(buf);
    }
#endif

    परत ml_append_int(buf, lnum, ओळ, len, flags);
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
    पूर्णांक
ml_append(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ओळ,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    पूर्णांक		newfile)	// flag, see above
{
    परत ml_append_flags(lnum, ओळ, len, newfile ? ML_APPEND_NEW : 0);
}

    पूर्णांक
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ओळ,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    पूर्णांक		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    जर (curbuf->b_ml.ml_mfp == रिक्त && open_buffer(FALSE, रिक्त, 0) == FAIL)
	परत FAIL;
    परत ml_append_flush(curbuf, lnum, ओळ, len, flags);
}


#जर defined(FEAT_SPELL) || defined(FEAT_QUICKFIX) || defined(FEAT_PROP_POPUP)

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
    पूर्णांक
ml_append_buf(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*ओळ,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    पूर्णांक		newfile)	// flag, see above
{
    जर (buf->b_ml.ml_mfp == रिक्त)
	परत FAIL;
    परत ml_append_flush(buf, lnum, ओळ, len, newfile ? ML_APPEND_NEW : 0);
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
    पूर्णांक
ml_replace(linenr_T lnum, char_u *ओळ, पूर्णांक copy)
{
    colnr_T len = -1;

    जर (ओळ != रिक्त)
	len = (colnr_T)STRLEN(ओळ);
    परत ml_replace_len(lnum, ओळ, len, FALSE, copy);
}

/*
 * Replace a line for the current buffer.  Like ml_replace() with:
 * "len_arg" is the length of the text, excluding NUL.
 * If "has_props" is TRUE then "line_arg" includes the text properties and
 * "len_arg" includes the NUL of the text and text properties.
 * When "copy" is TRUE copy the text into allocated memory, otherwise
 * "line_arg" must be allocated and will be consumed here.
 */
    पूर्णांक
ml_replace_len(
	linenr_T    lnum,
	char_u	    *ओळ_arg,
	colnr_T	    len_arg,
	पूर्णांक	    has_props,
	पूर्णांक	    copy)
{
    char_u *ओळ = ओळ_arg;
    colnr_T len = len_arg;

    जर (ओळ == रिक्त)		// just checking...
	परत FAIL;

    // When starting up, we might still need to create the memfile
    जर (curbuf->b_ml.ml_mfp == रिक्त && open_buffer(FALSE, रिक्त, 0) == FAIL)
	परत FAIL;

    जर (!has_props)
	++len;  // include the NUL after the text
    जर (copy)
    {
	// copy the line to allocated memory
#ifdef FEAT_PROP_POPUP
	जर (has_props)
	    ओळ = vim_memsave(ओळ, len);
	अन्यथा
#endif
	    ओळ = vim_strnsave(ओळ, len - 1);
	जर (ओळ == रिक्त)
	    परत FAIL;
    }

#ifdef FEAT_NETBEANS_INTG
    जर (netbeans_active())
    {
	netbeans_removed(curbuf, lnum, 0, (दीर्घ)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, ओळ, (पूर्णांक)STRLEN(ओळ));
    }
#endif
    जर (curbuf->b_ml.ml_ओळ_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_ओळ(curbuf);

#ifdef FEAT_PROP_POPUP
	जर (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#endif
    }

#ifdef FEAT_PROP_POPUP
    जर (curbuf->b_has_textprop && !has_props)
    {
	माप_प्रकार	oldtextlen = STRLEN(curbuf->b_ml.ml_ओळ_ptr) + 1;

	जर (oldtextlen < (माप_प्रकार)curbuf->b_ml.ml_ओळ_len)
	{
	    char_u *newline;
	    माप_प्रकार textproplen = curbuf->b_ml.ml_ओळ_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    जर (newline != रिक्त)
	    {
		mch_memmove(newline, ओळ, len);
		mch_memmove(newline + len, curbuf->b_ml.ml_ओळ_ptr
						    + oldtextlen, textproplen);
		vim_free(ओळ);
		ओळ = newline;
		len += (colnr_T)textproplen;
	    }
	}
    }
#endif

    जर (curbuf->b_ml.ml_flags & (ML_ओळ_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_ओळ_ptr);	// free allocated line

    curbuf->b_ml.ml_ओळ_ptr = ओळ;
    curbuf->b_ml.ml_ओळ_len = len;
    curbuf->b_ml.ml_ओळ_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_ओळ_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_ओळ_DIRTY) & ~ML_EMPTY;

    परत OK;
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
	पूर्णांक	    del_props_len,
	पूर्णांक	    above)
{
    पूर्णांक		did_get_ओळ = FALSE;
    पूर्णांक		done_del;
    पूर्णांक		done_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    DATA_BL	*dp;
    पूर्णांक		idx;
    पूर्णांक		ओळ_start;
    दीर्घ	ओळ_size;
    पूर्णांक		this_props_len = 0;
    char_u	*text;
    माप_प्रकार	textlen;
    पूर्णांक		found;

    साठी (done_del = 0; done_del < del_props_len; done_del += आकार(textprop_T))
    {
	mch_memmove(&prop_del, del_props + done_del, आकार(textprop_T));
	जर ((above && (prop_del.tp_flags & TP_FLAG_CONT_PREV)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_NEXT))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_NEXT)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_PREV)))
	{
	    जर (!did_get_ओळ)
	    {
		did_get_ओळ = TRUE;
		जर ((hp = ml_find_ओळ(buf, lnum, ML_FIND)) == रिक्त)
		    परत;

		dp = (DATA_BL *)(hp->bh_data);
		idx = lnum - buf->b_ml.ml_locked_खालचा;
		ओळ_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
		जर (idx == 0)		// first line in block, text at the end
		    ओळ_size = dp->db_txt_end - ओळ_start;
		अन्यथा
		    ओळ_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK)
								  - ओळ_start;
		text = (char_u *)dp + ओळ_start;
		textlen = STRLEN(text) + 1;
		जर ((दीर्घ)textlen >= ओळ_size)
		{
		    // No properties on this line.
		    जर (above)
			internal_error("no text property above deleted line");
		    अन्यथा
			internal_error("no text property below deleted line");
		    परत;
		}
		जर ((दीर्घ)textlen + (दीर्घ)PROP_मोजणी_SIZE > ओळ_size)
		{
		    internal_error("text property data too short");
		    परत;
		}

		uint16_t pc;

		mch_memmove(&pc, text + textlen, PROP_मोजणी_SIZE);
		जर (!text_prop_मोजणी_valid(pc, (माप_प्रकार)(ओळ_size - (दीर्घ)textlen)))
		{
		    internal_error("text property count too large");
		    परत;
		}
		this_props_len = pc * (पूर्णांक)आकार(textprop_T);
	    }

	    found = FALSE;
	    {
		char_u *props_start = text + textlen + PROP_मोजणी_SIZE;

		साठी (done_this = 0; done_this < this_props_len;
					       done_this += आकार(textprop_T))
		{
		    पूर्णांक		flag = above ? TP_FLAG_CONT_NEXT
							   : TP_FLAG_CONT_PREV;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_start + done_this,
							   आकार(textprop_T));
		    जर ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			found = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_start + done_this, &prop_this,
							   आकार(textprop_T));
			थांबा;
		    }
		}
	    }
	    जर (!found)
	    {
		जर (above)
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
    स्थिर पूर्णांक
ml_delete_int(buf_T *buf, linenr_T lnum, पूर्णांक flags)
{
    bhdr_T	*hp;
    memfile_T	*mfp;
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    पूर्णांक		मोजणी;	    // number of entries in block
    पूर्णांक		idx;
    पूर्णांक		चळत_idx;
    पूर्णांक		text_start;
    पूर्णांक		ओळ_start;
    दीर्घ	ओळ_size;
    पूर्णांक		i;
    पूर्णांक		ret = FAIL;
#ifdef FEAT_PROP_POPUP
    char_u	*textprop_save = रिक्त;
    दीर्घ	textprop_len = 0;
#endif

    जर (lowest_marked && lowest_marked > lnum)
	lowest_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    जर (buf->b_ml.ml_ओळ_मोजणी == 1)	    // file becomes empty
    {
	जर ((flags & ML_DEL_MESSAGE)
#ifdef FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_ओळी
#endif
	   )
	    set_keep_संदेश((char_u *)_(no_ओळी_संदेश), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_EMPTY;

	परत i;
    }

/*
 * Find the data block containing the line.
 * This also fills the stack with the blocks from the root to the data block.
 * This also releases any locked block..
 */
    mfp = buf->b_ml.ml_mfp;
    जर (mfp == रिक्त)
	परत FAIL;

    जर ((hp = ml_find_ओळ(buf, lnum, ML_DELETE)) == रिक्त)
	परत FAIL;

    dp = (DATA_BL *)(hp->bh_data);
    // compute line count before the delete
    मोजणी = (दीर्घ)(buf->b_ml.ml_locked_वरचा)
					- (दीर्घ)(buf->b_ml.ml_locked_खालचा) + 2;
    idx = lnum - buf->b_ml.ml_locked_खालचा;

    --buf->b_ml.ml_ओळ_मोजणी;

    ओळ_start = ((dp->db_index[idx]) & DB_INDEX_MASK);
    जर (idx == 0)		// first line in block, text at the end
	ओळ_size = dp->db_txt_end - ओळ_start;
    अन्यथा
	ओळ_size = ((dp->db_index[idx - 1]) & DB_INDEX_MASK) - ओळ_start;

#ifdef FEAT_NETBEANS_INTG
    जर (netbeans_active())
	netbeans_removed(buf, lnum, 0, ओळ_size);
#endif
#ifdef FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    जर (buf->b_has_textprop)
    {
	माप_प्रकार	textlen = STRLEN((char_u *)dp + ओळ_start) + 1;

	textprop_len = ओळ_size - (दीर्घ)textlen;
	जर (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + ओळ_start + textlen,
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
    जर (मोजणी == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = रिक्त;

	साठी (चळत_idx = buf->b_ml.ml_चळत_शीर्ष - 1; चळत_idx >= 0;
								  --चळत_idx)
	{
	    buf->b_ml.ml_चळत_शीर्ष = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_चळत[चळत_idx]);
	    idx = ip->ip_index;
	    जर ((hp = mf_get(mfp, ip->ip_bnum, 1)) == रिक्त)
		जा theend;
	    pp = (PTR_BL *)(hp->bh_data);   // must be pointer block
	    जर (pp->pb_id != PTR_ID)
	    {
		iemsg(e_pointer_block_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		जा theend;
	    }
	    मोजणी = --(pp->pb_मोजणी);
	    जर (मोजणी == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    अन्यथा
	    {
		जर (मोजणी != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_pointer[idx], &pp->pb_pointer[idx + 1],
				      (माप_प्रकार)(मोजणी - idx) * आकार(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_चळत_शीर्ष = चळत_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		जर (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_चळत[buf->b_ml.ml_चळत_शीर्ष].ip_वरचा +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_चळत_शीर्ष);

		थांबा;
	    }
	}
	CHECK(चळत_idx < 0, _("deleted block 1?"));
    }
    अन्यथा
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	text_start = dp->db_txt_start;
	mch_memmove((अक्षर *)dp + text_start + ओळ_size,
		  (अक्षर *)dp + text_start, (माप_प्रकार)(ओळ_start - text_start));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	साठी (i = idx; i < मोजणी - 1; ++i)
	    dp->db_index[i] = dp->db_index[i + 1] + ओळ_size;

	dp->db_free += ओळ_size + INDEX_SIZE;
	dp->db_txt_start += ओळ_size;
	--(dp->db_ओळ_मोजणी);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#ifdef FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, ओळ_size
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#endif
    ret = OK;

theend:
#ifdef FEAT_PROP_POPUP
    जर (textprop_save != रिक्त)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_data;
	पूर्णांक	    props_bytes;

	mch_memmove(&pc, textprop_save, PROP_मोजणी_SIZE);
	props_data = textprop_save + PROP_मोजणी_SIZE;
	props_bytes = pc * (पूर्णांक)आकार(textprop_T);
	जर (!text_prop_मोजणी_valid(pc, (माप_प्रकार)textprop_len))
	    props_bytes = 0;

	// Adjust text properties in the line above and below.
	जर (lnum > 1)
	    adjust_text_props_for_delete(buf, lnum - 1,
					     props_data, props_bytes, TRUE);
	जर (lnum <= buf->b_ml.ml_ओळ_मोजणी)
	    adjust_text_props_for_delete(buf, lnum,
					    props_data, props_bytes, FALSE);
    }
    vim_free(textprop_save);
#endif
    परत ret;
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
    पूर्णांक
ml_delete(linenr_T lnum)
{
    परत ml_delete_flags(lnum, 0);
}

/*
 * Like ml_delete() but using flags (see ml_delete_int()).
 */
    पूर्णांक
ml_delete_flags(linenr_T lnum, पूर्णांक flags)
{
    ml_flush_ओळ(curbuf);
    जर (lnum < 1 || lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	परत FAIL;

#ifdef FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#endif

    परत ml_delete_int(curbuf, lnum, flags);
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
    जर (lnum < 1 || lnum > curbuf->b_ml.ml_ओळ_मोजणी
					       || curbuf->b_ml.ml_mfp == रिक्त)
	परत;			    // give error message?

    जर (lowest_marked == 0 || lowest_marked > lnum)
	lowest_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    जर ((hp = ml_find_ओळ(curbuf, lnum, ML_FIND)) == रिक्त)
	परत;		    // give error message?

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_index[lnum - curbuf->b_ml.ml_locked_खालचा] |= DB_MARKED;
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
    पूर्णांक		i;

    जर (curbuf->b_ml.ml_mfp == रिक्त)
	परत (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    साठी (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_ओळ_मोजणी; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	जर ((hp = ml_find_ओळ(curbuf, lnum, ML_FIND)) == रिक्त)
	    परत (linenr_T)0;		    // give error message?

	dp = (DATA_BL *)(hp->bh_data);

	साठी (i = lnum - curbuf->b_ml.ml_locked_खालचा;
			    lnum <= curbuf->b_ml.ml_locked_वरचा; ++i, ++lnum)
	    जर ((dp->db_index[i]) & DB_MARKED)
	    {
		(dp->db_index[i]) &= DB_INDEX_MASK;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		lowest_marked = lnum + 1;
		परत lnum;
	    }
    }

    परत (linenr_T) 0;
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
    पूर्णांक		i;

    जर (curbuf->b_ml.ml_mfp == रिक्त)	    // nothing to do
	परत;

    /*
     * The search starts with line lowest_marked.
     */
    साठी (lnum = lowest_marked; lnum <= curbuf->b_ml.ml_ओळ_मोजणी; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	जर ((hp = ml_find_ओळ(curbuf, lnum, ML_FIND)) == रिक्त)
	    परत;		// give error message?

	dp = (DATA_BL *)(hp->bh_data);

	साठी (i = lnum - curbuf->b_ml.ml_locked_खालचा;
			    lnum <= curbuf->b_ml.ml_locked_वरचा; ++i, ++lnum)
	    जर ((dp->db_index[i]) & DB_MARKED)
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
ml_flush_ओळ(buf_T *buf)
{
    bhdr_T	*hp;
    DATA_BL	*dp;
    linenr_T	lnum;
    char_u	*new_ओळ;
    char_u	*old_ओळ;
    colnr_T	new_len;
    पूर्णांक		old_len;
    पूर्णांक		extra;
    पूर्णांक		idx;
    पूर्णांक		start;
    पूर्णांक		मोजणी;
    पूर्णांक		i;
    स्थिर पूर्णांक  entered = FALSE;

    जर (buf->b_ml.ml_ओळ_lnum == 0 || buf->b_ml.ml_mfp == रिक्त)
	परत;		// nothing to do

    जर (buf->b_ml.ml_flags & ML_ओळ_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	जर (entered)
	    परत;
	entered = TRUE;

	lnum = buf->b_ml.ml_ओळ_lnum;
	new_ओळ = buf->b_ml.ml_ओळ_ptr;

	hp = ml_find_ओळ(buf, lnum, ML_FIND);
	जर (hp == रिक्त)
	    siemsg(e_cannot_find_ओळ_nr, lnum);
	अन्यथा
	{
	    dp = (DATA_BL *)(hp->bh_data);
	    idx = lnum - buf->b_ml.ml_locked_खालचा;
	    start = ((dp->db_index[idx]) & DB_INDEX_MASK);
	    old_ओळ = (char_u *)dp + start;
	    जर (idx == 0)	// line is last in block
		old_len = dp->db_txt_end - start;
	    अन्यथा		// text of previous line follows
		old_len = (dp->db_index[idx - 1] & DB_INDEX_MASK) - start;
	    new_len = buf->b_ml.ml_ओळ_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    जर ((पूर्णांक)dp->db_free >= extra)
	    {
#जर defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		पूर्णांक old_prop_len = 0;
		जर (buf->b_has_textprop)
		    old_prop_len = old_len - (पूर्णांक)STRLEN(old_ओळ) - 1;
#endif
		// if the length changes and there are following lines
		मोजणी = buf->b_ml.ml_locked_वरचा - buf->b_ml.ml_locked_खालचा + 1;
		जर (extra != 0 && idx < मोजणी - 1)
		{
		    // move text of following lines
		    mch_memmove((अक्षर *)dp + dp->db_txt_start - extra,
				(अक्षर *)dp + dp->db_txt_start,
				(माप_प्रकार)(start - dp->db_txt_start));

		    // adjust pointers of this and following lines
		    साठी (i = idx + 1; i < मोजणी; ++i)
			dp->db_index[i] -= extra;
		}
		dp->db_index[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_start -= extra;

		// copy new line into the data block
		mch_memmove(old_ओळ - extra, new_ओळ, (माप_प्रकार)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#जर defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		जर (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (पूर्णांक)STRLEN(new_ओळ) - 1;
		}
		जर (extra != 0)
		    ml_updatechunk(buf, lnum, (दीर्घ)extra, ML_CHNK_UPDLINE);
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
		(शून्य)ml_append_int(buf, lnum, new_ओळ, new_len,
			 ((dp->db_index[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#ifdef FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#endif
			 );
		(शून्य)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_ओळ);

	entered = FALSE;
    }
    अन्यथा जर (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_ओळ_ptr);

    buf->b_ml.ml_flags &= ~(ML_ओळ_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_ओळ_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    स्थिर bhdr_T *
ml_new_data(memfile_T *mfp, पूर्णांक negative, पूर्णांक page_मोजणी)
{
    bhdr_T	*hp;
    DATA_BL	*dp;

    जर ((hp = mf_new(mfp, negative, page_मोजणी)) == रिक्त)
	परत रिक्त;

    dp = (DATA_BL *)(hp->bh_data);
    dp->db_id = DATA_ID;
    dp->db_txt_start = dp->db_txt_end = page_मोजणी * mfp->mf_page_size;
    dp->db_free = dp->db_txt_start - HEADER_SIZE;
    dp->db_ओळ_मोजणी = 0;

    परत hp;
}

/*
 * create a new, empty, pointer block
 */
    स्थिर bhdr_T *
ml_new_ptr(memfile_T *mfp)
{
    bhdr_T	*hp;
    PTR_BL	*pp;

    जर ((hp = mf_new(mfp, FALSE, 1)) == रिक्त)
	परत रिक्त;

    pp = (PTR_BL *)(hp->bh_data);
    pp->pb_id = PTR_ID;
    pp->pb_मोजणी = 0;
    pp->pb_मोजणी_max = PB_मोजणी_MAX(mfp);

    परत hp;
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
ml_find_ओळ(buf_T *buf, linenr_T lnum, पूर्णांक action)
{
    DATA_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    पूर्णांक		dirty;
    linenr_T	खालचा, वरचा;
    पूर्णांक		शीर्ष;
    पूर्णांक		page_मोजणी;
    पूर्णांक		idx;

    mfp = buf->b_ml.ml_mfp;

    /*
     * If there is a locked block check if the wanted line is in it.
     * If not, flush and release the locked block.
     * Don't do this for ML_INSERT_SAME, because the stack need to be updated.
     * Don't do this for ML_FLUSH, because we want to flush the locked block.
     * Don't do this when 'swapfile' is reset, we want to load all the blocks.
     */
    जर (buf->b_ml.ml_locked)
    {
	जर (ML_SIMPLE(action)
		&& buf->b_ml.ml_locked_खालचा <= lnum
		&& buf->b_ml.ml_locked_वरचा >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    जर (action == ML_घाला)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_वरचा);
	    }
	    अन्यथा जर (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_वरचा);
	    }
	    परत (buf->b_ml.ml_locked);
	}

	mf_put(mfp, buf->b_ml.ml_locked, buf->b_ml.ml_flags & ML_LOCKED_DIRTY,
					    buf->b_ml.ml_flags & ML_LOCKED_POS);
	buf->b_ml.ml_locked = रिक्त;

	/*
	 * If lines have been added or deleted in the locked block, need to
	 * update the line count in pointer blocks.
	 */
	जर (buf->b_ml.ml_locked_lineadd != 0)
	    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
    }

    जर (action == ML_FLUSH)	    // nothing else to do
	परत रिक्त;

    bnum = 1;			    // start at the root of the tree
    page_मोजणी = 1;
    खालचा = 1;
    वरचा = buf->b_ml.ml_ओळ_मोजणी;

    जर (action == ML_FIND)	// first try stack entries
    {
	साठी (शीर्ष = buf->b_ml.ml_चळत_शीर्ष - 1; शीर्ष >= 0; --शीर्ष)
	{
	    ip = &(buf->b_ml.ml_चळत[शीर्ष]);
	    जर (ip->ip_खालचा <= lnum && ip->ip_वरचा >= lnum)
	    {
		bnum = ip->ip_bnum;
		खालचा = ip->ip_खालचा;
		वरचा = ip->ip_वरचा;
		buf->b_ml.ml_चळत_शीर्ष = शीर्ष;	// truncate stack at prev entry
		थांबा;
	    }
	}
	जर (शीर्ष < 0)
	    buf->b_ml.ml_चळत_शीर्ष = 0;		// not found, start at the root
    }
    अन्यथा	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_चळत_शीर्ष = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    साठी (;;)
    {
	जर ((hp = mf_get(mfp, bnum, page_मोजणी)) == रिक्त)
	    जा error_noblock;

	/*
	 * update high for insert/delete
	 */
	जर (action == ML_घाला)
	    ++वरचा;
	अन्यथा जर (action == ML_DELETE)
	    --वरचा;

	dp = (DATA_BL *)(hp->bh_data);
	जर (dp->db_id == DATA_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_खालचा = खालचा;
	    buf->b_ml.ml_locked_वरचा = वरचा;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    परत hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	जर (pp->pb_id != PTR_ID)
	{
	    iemsg(e_pointer_block_id_wrong);
	    जा error_block;
	}

	जर ((शीर्ष = ml_add_चळत(buf)) < 0)	// add new entry to stack
	    जा error_block;
	ip = &(buf->b_ml.ml_चळत[शीर्ष]);
	ip->ip_bnum = bnum;
	ip->ip_खालचा = खालचा;
	ip->ip_वरचा = वरचा;
	ip->ip_index = -1;		// index not known yet

	dirty = FALSE;
	साठी (idx = 0; idx < (पूर्णांक)pp->pb_मोजणी; ++idx)
	{
	    t = pp->pb_pointer[idx].pe_ओळ_मोजणी;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    जर ((खालचा += t) > lnum)
	    {
		ip->ip_index = idx;
		bnum = pp->pb_pointer[idx].pe_bnum;
		page_मोजणी = pp->pb_pointer[idx].pe_page_मोजणी;
		वरचा = खालचा - 1;
		खालचा -= t;

		/*
		 * a negative block number may have been changed
		 */
		जर (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    जर (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_pointer[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		थांबा;
	    }
	}
	जर (idx >= (पूर्णांक)pp->pb_मोजणी)	    // past the end: something wrong!
	{
	    जर (lnum > buf->b_ml.ml_ओळ_मोजणी)
		siemsg(e_ओळ_संख्या_out_of_range_nr_past_the_end,
					      lnum - buf->b_ml.ml_ओळ_मोजणी);

	    अन्यथा
		siemsg(e_ओळ_मोजणी_wrong_in_block_nr, bnum);
	    जा error_block;
	}
	जर (action == ML_DELETE)
	{
	    pp->pb_pointer[idx].pe_ओळ_मोजणी--;
	    dirty = TRUE;
	}
	अन्यथा जर (action == ML_घाला)
	{
	    pp->pb_pointer[idx].pe_ओळ_मोजणी++;
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
    जर (action == ML_DELETE)
	ml_lineadd(buf, 1);
    अन्यथा जर (action == ML_घाला)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_चळत_शीर्ष = 0;
    परत रिक्त;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    स्थिर पूर्णांक
ml_add_चळत(buf_T *buf)
{
    पूर्णांक		शीर्ष;
    infoptr_T	*newstack;

    शीर्ष = buf->b_ml.ml_चळत_शीर्ष;

    // may have to increase the stack size
    जर (शीर्ष == buf->b_ml.ml_चळत_size)
    {
	CHECK(शीर्ष > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_चळत_size + चळत_INCR);
	जर (newstack == रिक्त)
	    परत -1;
	जर (शीर्ष > 0)
	    mch_memmove(newstack, buf->b_ml.ml_चळत,
					     (माप_प्रकार)शीर्ष * आकार(infoptr_T));
	vim_free(buf->b_ml.ml_चळत);
	buf->b_ml.ml_चळत = newstack;
	buf->b_ml.ml_चळत_size += चळत_INCR;
    }

    buf->b_ml.ml_चळत_शीर्ष++;
    परत शीर्ष;
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
ml_lineadd(buf_T *buf, पूर्णांक मोजणी)
{
    पूर्णांक		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    साठी (idx = buf->b_ml.ml_चळत_शीर्ष - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_चळत[idx]);
	जर ((hp = mf_get(mfp, ip->ip_bnum, 1)) == रिक्त)
	    थांबा;
	pp = (PTR_BL *)(hp->bh_data);	// must be pointer block
	जर (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_pointer_block_id_wrong_two);
	    थांबा;
	}
	pp->pb_pointer[ip->ip_index].pe_ओळ_मोजणी += मोजणी;
	ip->ip_वरचा += मोजणी;
	mf_put(mfp, hp, TRUE, FALSE);
    }
}

#जर defined(HAVE_READLINK)
/*
 * Resolve a symlink in the last component of a file name.
 * Note that f_resolve() does it for every part of the path, we don't do that
 * here.
 * If it worked returns OK and the resolved link in "buf[MAXPATHL]".
 * Otherwise returns FAIL.
 */
    पूर्णांक
resolve_symlink(char_u *fname, char_u *buf)
{
    char_u	tmp[MAXPATHL];
    पूर्णांक		ret;
    पूर्णांक		depth = 0;

    जर (fname == रिक्त)
	परत FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    साठी (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	जर (++depth == 100)
	{
	    semsg(_(e_symlink_loop_for_str), fname);
	    परत FAIL;
	}

	ret = readlink((अक्षर *)tmp, (अक्षर *)buf, MAXPATHL - 1);
	जर (ret <= 0)
	{
	    जर (त्रुटि_सं == EINVAL || त्रुटि_सं == ENOENT)
	    {
		// Found non-symlink or not existing file, stop here.
		// When at the first level use the unmodified name, skip the
		// call to vim_FullName().
		जर (depth == 1)
		    परत FAIL;

		// Use the resolved name in tmp[].
		थांबा;
	    }

	    // There must be some error reading links, use original name.
	    परत FAIL;
	}
	buf[ret] = NUL;

	/*
	 * Check whether the symlink is relative or absolute.
	 * If it's relative, build a new path based on the directory
	 * portion of the filename (if any) and the path the symlink
	 * points to.
	 */
	जर (mch_isFullName(buf))
	    STRCPY(tmp, buf);
	अन्यथा
	{
	    char_u *tail;

	    tail = gettail(tmp);
	    जर (STRLEN(tail) + STRLEN(buf) >= MAXPATHL)
		परत FAIL;
	    STRCPY(tail, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    परत vim_FullName(tmp, buf, MAXPATHL, TRUE);
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
    जर (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#endif

#जर defined(UNIX) || defined(MSWIN)  // Need _very_ long file names
    पूर्णांक		len = (पूर्णांक)STRLEN(dir_name);

    s = dir_name + len;
    जर (after_pathsep(dir_name, s) && len > 1 && s[-1] == s[-2])
    {			       // Ends with '//', Use Full path
	r = रिक्त;
	जर ((s = make_percent_swname(dir_name, s, fname_res)) != रिक्त)
	{
	    r = modname(s, (char_u *)".swp", FALSE);
	    vim_free(s);
	}
	परत r;
    }
#endif

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#जर defined(VMS)
	    "_swp",
#अन्यथा
	    ".swp",
#endif
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    जर (r == रिक्त)	    // out of memory
	परत रिक्त;

    s = get_file_in_dir(r, dir_name);
    vim_free(r);
    परत s;
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

    जर (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (माप_प्रकार)(tail.string - fname) + tail.length);
    अन्यथा
    {
	माप_प्रकार	dname_len = STRLEN(dname);

	जर (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    जर (tail.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    tail.string, tail.length, TRUE, &retval);
	    अन्यथा
	    {
		पूर्णांक	    save_char;
		string_T    tmp;

		save_char = *tail.string;
		*tail.string = NUL;
		concat_fnames(fname, (माप_प्रकार)(tail.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*tail.string = save_char;
		जर (tmp.string == रिक्त)	    // out of memory
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
    जर (retval.string != रिक्त)
    {
	char_u	*t;

	साठी (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    जर (*t == ':')
		*t = '%';
    }
#endif

    परत retval.string;
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
    समय_प्रकार	swap_mtime;

    ++no_wait_return;
    (शून्य)emsg(_(e_attention));
    संदेश_puts(_("\nFound a swap file by the name \""));
    संदेश_home_replace(fname);
    संदेश_puts("\"\n");
    swap_mtime = swapfile_info(fname);
    संदेश_puts(_("While opening file \""));
    संदेश_outtrans(buf->b_fname);
    संदेश_puts("\"\n");
    जर (mch_stat((अक्षर *)buf->b_fname, &st) == -1)
    {
	संदेश_puts(_("      CANNOT BE FOUND"));
    }
    अन्यथा
    {
	संदेश_puts(_("             dated: "));
	संदेश_puts(get_ctime(st.st_mtime, TRUE));
	जर (swap_mtime != 0 && st.st_mtime > swap_mtime)
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

प्रकारव्याख्या गणना {
    SEA_CHOICE_NONE = 0,
    SEA_CHOICE_READONLY = 1,
    SEA_CHOICE_EDIT = 2,
    SEA_CHOICE_RECOVER = 3,
    SEA_CHOICE_DELETE = 4,
    SEA_CHOICE_QUIT = 5,
    SEA_CHOICE_ABORT = 6
} sea_choice_T;

#जर defined(FEAT_EVAL)
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

    निवड (*get_vim_var_str(VV_SWAPCHOICE))
    {
	स्थिती 'o': परत SEA_CHOICE_READONLY;
	स्थिती 'e': परत SEA_CHOICE_EDIT;
	स्थिती 'r': परत SEA_CHOICE_RECOVER;
	स्थिती 'd': परत SEA_CHOICE_DELETE;
	स्थिती 'q': परत SEA_CHOICE_QUIT;
	स्थिती 'a': परत SEA_CHOICE_ABORT;
    }

    परत SEA_CHOICE_NONE;
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
    पूर्णांक		n;
    char_u	*dir_name;
#ifdef AMIGA
    BPTR	fh;
#endif
    पूर्णांक		r;
    char_u	*buf_fname = buf->b_fname;

#जर !defined(UNIX)
# define CREATE_DUMMY_FILE
    खाता	*dummyfd = रिक्त;

# ifdef MSWIN
    जर (buf_fname != रिक्त && !mch_isFullName(buf_fname)
				       && vim_strchr(gettail(buf_fname), ':'))
    {
	char_u *t;

	buf_fname = vim_strsave(buf_fname);
	जर (buf_fname == रिक्त)
	    buf_fname = buf->b_fname;
	अन्यथा
	    साठी (t = gettail(buf_fname); *t != NUL; MB_PTR_ADV(t))
		जर (*t == ':')
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
    जर (!(buf->b_p_sn || buf->b_shortname) && buf_fname != रिक्त
					     && mch_getperm(buf_fname) < 0)
	dummyfd = mch_fopen((अक्षर *)buf_fname, "w");
#endif

    /*
     * Isolate a directory name from *dirp and put it in dir_name.
     * First allocate some memory to put the directory name in.
     */
    dir_name = alloc(STRLEN(*dirp) + 1);
    जर (dir_name == रिक्त)
	*dirp = रिक्त;
    अन्यथा
	(शून्य)copy_option_part(dirp, dir_name, 31000, ",");

    /*
     * we try different names until we find one that does not exist yet
     */
    जर (dir_name == रिक्त)	    // out of memory
	fname = रिक्त;
    अन्यथा
	fname = makeswapname(buf_fname, buf->b_ffname, buf, dir_name);

    साठी (;;)
    {
	जर (fname == रिक्त)	// must be out of memory
	    थांबा;
	जर ((n = (पूर्णांक)STRLEN(fname)) == 0)	// safety check
	{
	    VIM_CLEAR(fname);
	    थांबा;
	}
#जर defined(UNIX)
/*
 * Some systems have a MS-DOS compatible filesystem that use 8.3 character
 * file names. If this is the first try and the swap file name does not fit in
 * 8.3, detect if this is the case, set shortname and try again.
 */
	जर (fname[n - 2] == 'w' && fname[n - 1] == 'p'
					&& !(buf->b_p_sn || buf->b_shortname))
	{
	    char_u	    *tail;
	    char_u	    *fname2;
	    stat_T	    s1, s2;
	    पूर्णांक		    f1, f2;
	    पूर्णांक		    created1 = FALSE, created2 = FALSE;
	    पूर्णांक		    same = FALSE;

	    /*
	     * Check if swapfile name does not fit in 8.3:
	     * It either contains two dots, is longer than 8 chars, or starts
	     * with a dot.
	     */
	    tail = gettail(buf_fname);
	    जर (       vim_strchr(tail, '.') != रिक्त
		    || STRLEN(tail) > (माप_प्रकार)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		जर (fname2 != रिक्त)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    जर (vim_strchr(tail, '.') != रिक्त)
			fname2[n - 1] = 'x';
		    अन्यथा जर (*gettail(fname) == '.')
		    {
			fname2[n] = 'x';
			fname2[n + 1] = NUL;
		    }
		    अन्यथा
			fname2[n - 5] += 1;
		    /*
		     * may need to create the files to be able to use mch_stat()
		     */
		    f1 = mch_open((अक्षर *)fname, O_RDONLY | O_EXTRA, 0);
		    जर (f1 < 0)
		    {
			f1 = mch_open_rw((अक्षर *)fname,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			created1 = TRUE;
		    }
		    जर (f1 >= 0)
		    {
			f2 = mch_open((अक्षर *)fname2, O_RDONLY | O_EXTRA, 0);
			जर (f2 < 0)
			{
			    f2 = mch_open_rw((अक्षर *)fname2,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			    created2 = TRUE;
			}
			जर (f2 >= 0)
			{
			    /*
			     * Both files exist now. If mch_stat() returns the
			     * same device and inode they are the same file.
			     */
			    जर (mch_fstat(f1, &s1) != -1
				    && mch_fstat(f2, &s2) != -1
				    && s1.st_dev == s2.st_dev
				    && s1.st_ino == s2.st_ino)
				same = TRUE;
			    बंद(f2);
			    जर (created2)
				mch_remove(fname2);
			}
			बंद(f1);
			जर (created1)
			    mch_remove(fname);
		    }
		    vim_free(fname2);
		    जर (same)
		    {
			buf->b_shortname = true;
			vim_free(fname);
			fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
			सुरूठेवा;	// try again with b_shortname set
		    }
		}
	    }
	}
#endif
	/*
	 * check if the swapfile already exists
	 */
	जर (mch_getperm(fname) < 0)	// it does not exist
	{
#ifdef HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    जर (mch_lstat((अक्षर *)fname, &sb) < 0)
#अन्यथा
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (दीर्घ)MODE_NEWFILE);
	    /*
	     * on the Amiga mch_getperm() will return -1 when the file exists
	     * but is being used by another program. This happens if you edit
	     * a file twice.
	     */
	    जर (fh != (BPTR)रिक्त)	// can open file, OK
	    {
		Close(fh);
		mch_remove(fname);
		थांबा;
	    }
	    जर (IoErr() != ERROR_OBJECT_IN_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#endif
		थांबा;
	}

	/*
	 * A file name equal to old_fname is OK to use.
	 */
	जर (old_fname != रिक्त && fnamecmp(fname, old_fname) == 0)
	    थांबा;

	/*
	 * get here when file already exists
	 */
	जर (fname[n - 2] == 'w' && fname[n - 1] == 'p')	// first try
	{
	    /*
	     * on MS-DOS compatible filesystems (e.g. messydos) file.doc.swp
	     * and file.doc are the same file. To guess if this problem is
	     * present try if file.doc.swx exists. If it does, we set
	     * buf->b_shortname and try file_doc.swp (dots replaced by
	     * underscores for this file), and try again. If it doesn't we
	     * assume that "file.doc.swp" already exists.
	     */
	    जर (!(buf->b_p_sn || buf->b_shortname))	// not tried yet
	    {
		fname[n - 1] = 'x';
		r = mch_getperm(fname);		// try "file.swx"
		fname[n - 1] = 'p';
		जर (r >= 0)		    // "file.swx" seems to exist
		{
		    buf->b_shortname = true;
		    vim_free(fname);
		    fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
		    सुरूठेवा;	    // try again with '.' replaced with '_'
		}
	    }
	    /*
	     * If we get here the ".swp" file really exists.
	     * Give an error message, unless recovering, no file name, we are
	     * viewing a help file or when the path of the file is different
	     * (happens when all .swp files are in one directory).
	     */
	    जर (!recoverymode && buf_fname != रिक्त
				&& !buf->b_help
				&& !(buf->b_flags & (BF_DUMMY | BF_NO_SEA)))
	    {
		पूर्णांक		fd;
		संरचना block0	b0;
		पूर्णांक		differ = FALSE;

		/*
		 * Try to read block 0 from the swap file to get the original
		 * file name (and inode number).
		 */
		fd = mch_open((अक्षर *)fname, O_RDONLY | O_EXTRA, 0);
		जर (fd >= 0)
		{
		    जर (read_eintr(fd, &b0, आकार(b0)) == आकार(b0))
		    {
			/*
			 * If the swapfile has the same directory as the
			 * buffer don't compare the directory names, they can
			 * have a different mountpoint.
			 */
			जर (b0.b0_flags & B0_SAME_DIR)
			{
			    जर (fnamecmp(gettail(buf->b_ffname),
						   gettail(b0.b0_fname)) != 0
				    || !same_directory(fname, buf->b_ffname))
			    {
#ifdef CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				जर (fnamecmp_ino(buf->b_ffname, NameBuff,
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
			    जर (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_to_long(b0.b0_ino)))
				differ = TRUE;
#अन्यथा
			    जर (fnamecmp(NameBuff, buf->b_ffname) != 0)
				differ = TRUE;
#endif
			}
		    }
		    बंद(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		जर (differ == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == रिक्त)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#ifdef CREATE_DUMMY_FILE
		    पूर्णांक		 did_use_dummy = FALSE;

		    // Avoid getting a warning for the file being created
		    // outside of Vim, it was created at the start of this
		    // function.  Delete the file now, because Vim might exit
		    // here if the window is closed.
		    जर (dummyfd != रिक्त)
		    {
			ख_बंद(dummyfd);
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
		    जर (mch_stat((अक्षर *)buf->b_fname, &st) == 0
						  && swapfile_unchanged(fname))
		    {
			choice = SEA_CHOICE_DELETE;
			जर (p_verbose > 0)
			    verb_संदेश(_("Found a swap file that is not useful, deleting it"));
		    }

#जर defined(FEAT_EVAL)
		    /*
		     * If there is an SwapExists autocommand and we can handle
		     * the response, trigger it.  It may return 0 to ask the
		     * user anyway.
		     */
		    जर (choice == SEA_CHOICE_NONE
			    && swap_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#endif

		    जर (choice == SEA_CHOICE_NONE
					 && swap_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    जर (choice == SEA_CHOICE_NONE)
		    {
#ifdef FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			जर (gui.starting && !gui.in_use)
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

#जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    जर (swap_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    prefix = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			माप_प्रकार	    message_size;
			string_T    message;
			char_u	    *tofree;
			पूर्णांक	    dialog_result;

			prefix.length = STRLEN(prefix.string);
			suffix.length = STRLEN(suffix.string);
			message_size = prefix.length
				+ STRLEN(fname)
				+ suffix.length + 5;
			message.string = tofree = alloc(message_size);
			जर (message.string != रिक्त)
			{
			    STRCPY(message.string, prefix.string);
			    message.length = prefix.length;
			    message.length += home_replace(रिक्त, fname,
				message.string + message.length, (पूर्णांक)(message_size - message.length), TRUE);
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
			जर (process_still_running && dialog_result >= 4)
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

		    निवड (choice)
		    {
			स्थिती SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    थांबा;
			स्थिती SEA_CHOICE_EDIT:
			    थांबा;
			स्थिती SEA_CHOICE_RECOVER:
			    swap_exists_action = SEA_RECOVER;
			    थांबा;
			स्थिती SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    थांबा;
			स्थिती SEA_CHOICE_QUIT:
			    swap_exists_action = SEA_QUIT;
			    थांबा;
			स्थिती SEA_CHOICE_ABORT:
			    swap_exists_action = SEA_QUIT;
			    got_int = TRUE;
			    थांबा;
			स्थिती SEA_CHOICE_NONE:
			    संदेश_puts("\n");
			    जर (संदेश_silent == 0)
				// call wait_return() later
				need_wait_return = TRUE;
			    थांबा;
		    }

		    // If the file was deleted this fname can be used.
		    जर (choice != SEA_CHOICE_NONE && mch_getperm(fname) < 0)
			थांबा;

#ifdef CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    जर (did_use_dummy)
			dummyfd = mch_fopen((अक्षर *)buf_fname, "w");
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
	जर (fname[n - 1] == 'a')	// ".s?a"
	{
	    जर (fname[n - 2] == 'a')    // ".saa": tried enough, give up
	    {
		emsg(_(e_too_many_swap_files_found));
		VIM_CLEAR(fname);
		थांबा;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#ifdef CREATE_DUMMY_FILE
    जर (dummyfd != रिक्त)	// file has been created temporarily
    {
	ख_बंद(dummyfd);
	mch_remove(buf_fname);
    }
#endif
#ifdef MSWIN
    जर (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#endif
    परत fname;
}

    स्थिर पूर्णांक
b0_magic_wrong(ZERO_BL *b0p)
{
    परत (b0p->b0_magic_long != (दीर्घ)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (पूर्णांक)B0_MAGIC_INT
	    || b0p->b0_magic_short != (लघु)B0_MAGIC_SHORT
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

    स्थिर पूर्णांक
fnamecmp_ino(
    char_u	*fname_c,	    // current file name
    char_u	*fname_s,	    // file name from swap file
    दीर्घ	ino_block0)
{
    stat_T	st;
    ino_t	ino_c = 0;	    // ino of current file
    ino_t	ino_s;		    // ino of file from swap file
    char_u	buf_c[MAXPATHL];    // full path of fname_c
    char_u	buf_s[MAXPATHL];    // full path of fname_s
    पूर्णांक		retval_c;	    // flag: buf_c valid
    पूर्णांक		retval_s;	    // flag: buf_s valid

    जर (mch_stat((अक्षर *)fname_c, &st) == 0)
	ino_c = (ino_t)st.st_ino;

    /*
     * First we try to get the inode from the file name, because the inode in
     * the swap file may be outdated.  If that fails (e.g. this path is not
     * valid on this machine), use the inode from block 0.
     */
    जर (mch_stat((अक्षर *)fname_s, &st) == 0)
	ino_s = (ino_t)st.st_ino;
    अन्यथा
	ino_s = (ino_t)ino_block0;

    जर (ino_c && ino_s)
	परत (ino_c != ino_s);

    /*
     * One of the inode numbers is unknown, try a forced vim_FullName() and
     * compare the file names.
     */
    retval_c = vim_FullName(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_FullName(fname_s, buf_s, MAXPATHL, TRUE);
    जर (retval_c == OK && retval_s == OK)
	परत STRCMP(buf_c, buf_s) != 0;

    /*
     * Can't compare inodes or file names, guess that the files are different,
     * unless both appear not to exist at all, then compare with the file name
     * in the swap file.
     */
    जर (ino_s == 0 && ino_c == 0 && retval_c == FAIL && retval_s == FAIL)
	परत STRCMP(fname_c, fname_s) != 0;
    परत TRUE;
}
#endif // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    स्थिर शून्य
long_to_char(दीर्घ n, char_u *s)
{
    s[0] = (char_u)(n & 0xff);
    n = (अचिन्हित)n >> 8;
    s[1] = (char_u)(n & 0xff);
    n = (अचिन्हित)n >> 8;
    s[2] = (char_u)(n & 0xff);
    n = (अचिन्हित)n >> 8;
    s[3] = (char_u)(n & 0xff);
}

    स्थिर दीर्घ
char_to_long(char_u *s)
{
    दीर्घ    retval;

    retval = s[3];
    retval <<= 8;
    retval |= s[2];
    retval <<= 8;
    retval |= s[1];
    retval <<= 8;
    retval |= s[0];

    परत retval;
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

    जर (!buf->b_ml.ml_mfp)
	परत;
    साठी (hp = buf->b_ml.ml_mfp->mf_used_last; hp != रिक्त; hp = hp->bh_prev)
    {
	जर (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_data);
	    b0p->b0_dirty = buf->b_changed ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_MASK)
						  | (get_fileformat(buf) + 1);
	    add_b0_fenc(b0p, buf);
	    hp->bh_flags |= BH_DIRTY;
	    mf_sync(buf->b_ml.ml_mfp, MFS_ZERO);
	    थांबा;
	}
    }
}

#जर defined(FEAT_CRYPT)
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
    अचिन्हित	size)
{
    DATA_BL	*dp = (DATA_BL *)data;
    char_u	*head_end;
    char_u	*text_start;
    char_u	*new_data;
    पूर्णांक		text_len;
    cryptstate_T *state;

    जर (dp->db_id != DATA_ID)
	परत data;

    state = ml_crypt_prepare(mfp, offset, FALSE);
    जर (state == रिक्त)
	परत data;

    new_data = alloc(size);
    जर (new_data == रिक्त)
    {
	crypt_free_state(state);
	परत रिक्त;
    }
    head_end = (char_u *)(&dp->db_index[dp->db_ओळ_मोजणी]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = size - dp->db_txt_start;

    // Copy the header and the text.
    mch_memmove(new_data, dp, head_end - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, text_start, text_len, new_data + dp->db_txt_start,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    जर (head_end < text_start)
	vim_memset(new_data + (head_end - data), 0, text_start - head_end);

    परत new_data;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    शून्य
ml_decrypt_data(
    memfile_T	*mfp,
    char_u	*data,
    off_T	offset,
    अचिन्हित	size)
{
    DATA_BL	*dp = (DATA_BL *)data;
    char_u	*head_end;
    char_u	*text_start;
    पूर्णांक		text_len;
    cryptstate_T *state;

    जर (dp->db_id != DATA_ID)
	परत;

    head_end = (char_u *)(&dp->db_index[dp->db_ओळ_मोजणी]);
    text_start = (char_u *)dp + dp->db_txt_start;
    text_len = dp->db_txt_end - dp->db_txt_start;

    जर (head_end > text_start || dp->db_txt_start > size
	    || dp->db_txt_end > size)
	परत;  // data was messed up

    state = ml_crypt_prepare(mfp, offset, TRUE);
    जर (state == रिक्त)
	परत;

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
ml_crypt_prepare(memfile_T *mfp, off_T offset, पूर्णांक reading)
{
    buf_T	*buf = mfp->mf_buffer;
    char_u	salt[50];
    पूर्णांक		method_nr;
    char_u	*किल्ली;
    crypt_arg_T arg;

    CLEAR_FIELD(arg);
    जर (reading && mfp->mf_old_किल्ली != रिक्त)
    {
	// Reading back blocks with the previous key/method/seed.
	method_nr = mfp->mf_old_cm;
	किल्ली = mfp->mf_old_किल्ली;
	arg.cat_seed = mfp->mf_old_seed;
    }
    अन्यथा
    {
	method_nr = crypt_get_method_nr(buf);
	किल्ली = buf->b_p_किल्ली;
	arg.cat_seed = mfp->mf_seed;
    }

    जर (*किल्ली == NUL)
	परत रिक्त;

    जर (crypt_may_close_swapfile(buf, किल्ली, method_nr))
	परत रिक्त;

    जर (method_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((अक्षर *)salt, आकार(salt), "%s%ld", किल्ली, (दीर्घ)offset);
	arg.cat_seed = रिक्त;
	arg.cat_init_from_file = FALSE;

	परत crypt_create(method_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((अक्षर *)salt, आकार(salt), "%ld", (दीर्घ)offset);

    arg.cat_salt = salt;
    arg.cat_salt_len = (पूर्णांक)STRLEN(salt);
    arg.cat_seed_len = MF_SEED_LEN;
    arg.cat_add_len = 0;
    arg.cat_add = रिक्त;
    arg.cat_init_from_file = FALSE;

    परत crypt_create(method_nr, किल्ली, &arg);
}

#endif


#जर defined(FEAT_BYTEOFF)

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
    linenr_T	ओळ,
    दीर्घ	len,
    पूर्णांक		updtype)
{
    स्थिर buf_T	*ml_upd_lastbuf = रिक्त;
    स्थिर linenr_T	ml_upd_lastcurline;
    स्थिर पूर्णांक		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    पूर्णांक			curix = ml_upd_lastcurix;
    दीर्घ		size;
    chunksize_T		*curchnk;
    पूर्णांक			rest;
    bhdr_T		*hp;
    DATA_BL		*dp;

    जर (buf->b_ml.ml_usedchunks == -1 || len == 0)
	परत;
    जर (buf->b_ml.ml_chunksize == रिक्त)
    {
	buf->b_ml.ml_chunksize = ALLOC_MULT(chunksize_T, 100);
	जर (buf->b_ml.ml_chunksize == रिक्त)
	{
	    buf->b_ml.ml_usedchunks = -1;
	    परत;
	}
	buf->b_ml.ml_numchunks = 100;
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = 1;
	ml_upd_lastbuf = रिक्त;		// invalidate resume cache
    }

    जर (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_ओळ_मोजणी == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (दीर्घ)buf->b_ml.ml_ओळ_len;
	ml_upd_lastbuf = रिक्त;		// invalidate resume cache
	परत;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    जर (buf != ml_upd_lastbuf || ओळ < curline)
    {
	curline = 1;
	curix = 0;
    }
    साठी (; curix < buf->b_ml.ml_usedchunks - 1
	     && ओळ >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	     curix++)
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
    curchnk = buf->b_ml.ml_chunksize + curix;

    जर (updtype == ML_CHNK_DELLINE)
	len = -len;
    curchnk->mlcs_totalsize += len;
    जर (updtype == ML_CHNK_ADDLINE)
    {
	curchnk->mlcs_numlines++;

	// May resize here so we don't have to do it in both cases below
	जर (buf->b_ml.ml_usedchunks + 1 >= buf->b_ml.ml_numchunks)
	{
	    chunksize_T *t_chunksize = buf->b_ml.ml_chunksize;

	    buf->b_ml.ml_numchunks = buf->b_ml.ml_numchunks * 3 / 2;
	    buf->b_ml.ml_chunksize = vim_realloc(buf->b_ml.ml_chunksize,
			    आकार(chunksize_T) * buf->b_ml.ml_numchunks);
	    जर (buf->b_ml.ml_chunksize == रिक्त)
	    {
		// Hmmmm, Give up on offset for this buffer
		vim_free(t_chunksize);
		buf->b_ml.ml_usedchunks = -1;
		परत;
	    }
	}

	जर (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MAXL)
	{
	    पूर्णांक	    मोजणी;	    // number of entries in block
	    पूर्णांक	    idx;
	    पूर्णांक	    end_idx;
	    पूर्णांक	    text_end;
	    पूर्णांक	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			आकार(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    size = 0;
	    linecnt = 0;
	    पर्यंत (curline < buf->b_ml.ml_ओळ_मोजणी
			&& linecnt < MLCS_MINL)
	    {
		जर ((hp = ml_find_ओळ(buf, curline, ML_FIND)) == रिक्त)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    परत;
		}
		dp = (DATA_BL *)(hp->bh_data);
		मोजणी = (दीर्घ)(buf->b_ml.ml_locked_वरचा) -
			(दीर्घ)(buf->b_ml.ml_locked_खालचा) + 1;
		idx = curline - buf->b_ml.ml_locked_खालचा;
		curline = buf->b_ml.ml_locked_वरचा + 1;

		// compute index of last line to use in this MEMLINE
		rest = मोजणी - idx;
		जर (linecnt + rest > MLCS_MINL)
		{
		    end_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		अन्यथा
		{
		    end_idx = मोजणी - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		जर (buf->b_has_textprop)
		{
		    पूर्णांक i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    साठी (i = end_idx; i < idx; ++i)
			size += (पूर्णांक)STRLEN((char_u *)dp
				      + (dp->db_index[i] & DB_INDEX_MASK)) + 1;
		}
		अन्यथा
# endif
		{
		    जर (idx == 0) // first line in block, text at the end
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
	    परत;
	}
	अन्यथा जर (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_ओळ_मोजणी - ओळ <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    जर (ओळ == buf->b_ml.ml_ओळ_मोजणी)
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
		hp = ml_find_ओळ(buf, buf->b_ml.ml_ओळ_मोजणी, ML_FIND);
		जर (hp == रिक्त)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    परत;
		}
		dp = (DATA_BL *)(hp->bh_data);
		जर (dp->db_ओळ_मोजणी == 1)
		    rest = dp->db_txt_end - dp->db_txt_start;
		अन्यथा
		    rest =
			((dp->db_index[dp->db_ओळ_मोजणी - 2]) & DB_INDEX_MASK)
			- dp->db_txt_start;
		curchnk->mlcs_totalsize = rest;
		curchnk->mlcs_numlines = 1;
		curchnk[-1].mlcs_totalsize -= rest;
		curchnk[-1].mlcs_numlines -= 1;
	    }
	}
    }
    अन्यथा जर (updtype == ML_CHNK_DELLINE)
    {
	curchnk->mlcs_numlines--;
	जर (curix < buf->b_ml.ml_usedchunks - 1
		&& curchnk->mlcs_numlines + curchnk[1].mlcs_numlines
								  <= MLCS_MINL)
	{
	    curix++;
	    curchnk = buf->b_ml.ml_chunksize + curix;
	}
	अन्यथा जर (curix == 0 && curchnk->mlcs_numlines <= 0)
	{
	    ml_upd_lastbuf = रिक्त;   // Force recalc of curix & curline
	    buf->b_ml.ml_usedchunks--;
	    mch_memmove(buf->b_ml.ml_chunksize, buf->b_ml.ml_chunksize + 1,
			buf->b_ml.ml_usedchunks * आकार(chunksize_T));
	    परत;
	}
	अन्यथा जर (curix == 0 || (curchnk->mlcs_numlines > 10
		    && curchnk->mlcs_numlines + curchnk[-1].mlcs_numlines
								  > MLCS_MINL))
	{
	    // The chunks are left as they are, the cached position stays
	    // valid.
	    ml_upd_lastbuf = buf;
	    ml_upd_lastcurline = curline;
	    ml_upd_lastcurix = curix;
	    परत;
	}

	// Collapse chunks
	ml_upd_lastbuf = रिक्त;   // Force recalc of curix & curline
	curchnk[-1].mlcs_numlines += curchnk->mlcs_numlines;
	curchnk[-1].mlcs_totalsize += curchnk->mlcs_totalsize;
	buf->b_ml.ml_usedchunks--;
	जर (curix < buf->b_ml.ml_usedchunks)
	    mch_memmove(buf->b_ml.ml_chunksize + curix,
			buf->b_ml.ml_chunksize + curix + 1,
			(buf->b_ml.ml_usedchunks - curix) *
			आकार(chunksize_T));
	परत;
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
    दीर्घ
ml_find_ओळ_or_offset(buf_T *buf, linenr_T lnum, दीर्घ *offp)
{
    linenr_T	curline;
    पूर्णांक		curix;
    दीर्घ	size;
    bhdr_T	*hp;
    DATA_BL	*dp;
    पूर्णांक		मोजणी;		// number of entries in block
    पूर्णांक		idx;
    पूर्णांक		start_idx;
    पूर्णांक		text_end;
    दीर्घ	offset;
    पूर्णांक		len;
    पूर्णांक		ffdos = (get_fileformat(buf) == EOL_DOS);
    पूर्णांक		extra = 0;

    // take care of cached line first
    ml_flush_ओळ(curbuf);

    जर (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == रिक्त
	    || lnum < 0)
	परत -1;

    जर (offp == रिक्त)
	offset = 0;
    अन्यथा
	offset = *offp;
    जर (lnum == 0 && offset <= 0)
	परत 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = size = 0;
    पर्यंत (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (offset != 0
	       && offset > size + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (दीर्घ)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	size += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	जर (offset && ffdos)
	    size += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    पर्यंत ((lnum != 0 && curline < lnum) || (offset != 0 && size < offset))
    {
# ifdef FEAT_PROP_POPUP
	माप_प्रकार textprop_total = 0;
# endif

	जर (curline > buf->b_ml.ml_ओळ_मोजणी
		|| (hp = ml_find_ओळ(buf, curline, ML_FIND)) == रिक्त)
	    परत -1;
	dp = (DATA_BL *)(hp->bh_data);
	मोजणी = (दीर्घ)(buf->b_ml.ml_locked_वरचा) -
		(दीर्घ)(buf->b_ml.ml_locked_खालचा) + 1;
	start_idx = idx = curline - buf->b_ml.ml_locked_खालचा;
	जर (idx == 0)  // first line in block, text at the end
	    text_end = dp->db_txt_end;
	अन्यथा
	    text_end = ((dp->db_index[idx - 1]) & DB_INDEX_MASK);
	// Compute index of last line to use in this MEMLINE
	जर (lnum != 0)
	{
	    जर (curline + (मोजणी - idx) >= lnum)
		idx += lnum - curline - 1;
	    अन्यथा
		idx = मोजणी - 1;
	}
	अन्यथा
	{
	    extra = 0;
	    साठी (;;)
	    {
# ifdef FEAT_PROP_POPUP
		माप_प्रकार textprop_size = 0;

		जर (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_index[idx]) & DB_INDEX_MASK);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_end
				  : ((dp->db_index[idx - 1]) & DB_INDEX_MASK));
		    textprop_size = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		जर (!(offset >= size
			+ text_end - (पूर्णांक)((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
			- (दीर्घ)(textprop_total + textprop_size)
# endif
			+ ffdos))
		    थांबा;

		जर (ffdos)
		    size++;
# ifdef FEAT_PROP_POPUP
		textprop_total += textprop_size;
# endif
		जर (idx == मोजणी - 1)
		{
		    extra = 1;
		    थांबा;
		}
		idx++;
	    }
	}
# ifdef FEAT_PROP_POPUP
	जर (buf->b_has_textprop && lnum != 0)
	{
	    पूर्णांक i;

	    // cannot use the db_index pointer, need to get the actual text
	    // lengths.
	    len = 0;
	    साठी (i = start_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_index[i]) & DB_INDEX_MASK);
		len += (पूर्णांक)STRLEN(p) + 1;
	    }
	}
	अन्यथा
# endif
	    len = text_end - ((dp->db_index[idx]) & DB_INDEX_MASK)
# ifdef FEAT_PROP_POPUP
				- (दीर्घ)textprop_total
# endif
				;
	size += len;
	जर (offset != 0 && size >= offset)
	{
	    जर (size + ffdos == offset)
		*offp = 0;
	    अन्यथा जर (idx == start_idx)
		*offp = offset - size + len;
	    अन्यथा
		*offp = offset - size + len
		     - (text_end - ((dp->db_index[idx - 1]) & DB_INDEX_MASK))
# ifdef FEAT_PROP_POPUP
		     + (दीर्घ)textprop_total
# endif
		     ;
	    curline += idx - start_idx + extra;
	    जर (curline > buf->b_ml.ml_ओळ_मोजणी)
		परत -1;	// exactly one byte beyond the end
	    परत curline;
	}
	curline = buf->b_ml.ml_locked_वरचा + 1;
    }

    जर (lnum != 0)
    {
	// Count extra CR characters.
	जर (ffdos)
	    size += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	जर ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_ओळ_मोजणी)
	    size -= ffdos + 1;
    }

    परत size;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    शून्य
goto_byte(दीर्घ cnt)
{
    दीर्घ	boff = cnt;
    linenr_T	lnum;

    ml_flush_ओळ(curbuf);	// cached line may be dirty
    setpcmark();
    जर (boff)
	--boff;
    lnum = ml_find_ओळ_or_offset(curbuf, (linenr_T)0, &boff);
    जर (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
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
    जर (has_mbyte)
	mb_adjust_cursor();
}
#endif
