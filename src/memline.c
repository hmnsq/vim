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
#تعریف_کړه CHECK(c, s)	افعل { /**/ } طالما (0)

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

#ضمن "vim.h"

#ضمن <time.h>

#إذا defined(SASC) || defined(__amigaos4__)
# include <proto/dos.h>	    // for Open() and Close()
#د_که_پای

تعريفنوع بنية block0		ZERO_BL;    // contents of the first block
تعريفنوع بنية اشاره_ګر_بلاک	PTR_BL;	    // contents of a pointer block
تعريفنوع بنية معلومات_بلاک	معلومات_BL;    // contents of a data block
تعريفنوع بنية اشاره_ګر_ثبت	PTR_EN;	    // block/line-count pair

#تعریف_کړه معلومات_ID	       (('d' << 8) + 'a')   // data block id
#تعریف_کړه PTR_ID	       (('p' << 8) + 't')   // pointer block id
#تعریف_کړه BLOCK0_ID0     'b'		    // block 0 id 0
#تعریف_کړه BLOCK0_ID1     '0'		    // block 0 id 1
#تعریف_کړه BLOCK0_ID1_C0  'c'		    // block 0 id 1 'cm' 0
#تعریف_کړه BLOCK0_ID1_C1  'C'		    // block 0 id 1 'cm' 1
#تعریف_کړه BLOCK0_ID1_C2  'd'		    // block 0 id 1 'cm' 2
// BLOCK0_ID1_C3 and BLOCK0_ID1_C4 are for libsodium encryption.  However, for
// these the swapfile is disabled, thus they will not be used.  Added for
// consistency anyway.
#تعریف_کړه BLOCK0_ID1_C3  'S'		    // block 0 id 1 'cm' 3
#تعریف_کړه BLOCK0_ID1_C4  's'		    // block 0 id 1 'cm' 4

#إذا defined(FEAT_CRYPT)
ساكن ثابت صحيح id1_codes[] = {
    BLOCK0_ID1_C0,  // CRYPT_M_ZIP
    BLOCK0_ID1_C1,  // CRYPT_M_BF
    BLOCK0_ID1_C2,  // CRYPT_M_BF2
    BLOCK0_ID1_C3,  // CRYPT_M_SOD  - Unused!
    BLOCK0_ID1_C4,  // CRYPT_M_SOD2  - Unused!
};
#د_که_پای

/*
 * pointer to a block, used in a pointer block
 */
بنية اشاره_ګر_ثبت
{
    blocknr_T	pe_bnum;	// block number
    linenr_T	pe_کرښه_شمېر;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    صحيح		pe_پاڼه_شمېر;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
بنية اشاره_ګر_بلاک
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_شمېر;	// number of pointers in this block
    short_u	pb_شمېر_max;	// maximum value for pb_count
    PTR_EN	pb_اشاره_ګر[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#تعریف_کړه PB_شمېر_MAX(mfp) (short_u)(((mfp)->mf_پاڼه_اندازه - offsetof(PTR_BL, pb_اشاره_ګر)) / حجم(PTR_EN))

/*
 * A data block is a leaf in the tree.
 *
 * The text of the lines is at the end of the block. The text of the first line
 * in the block is put at the end, the text of the second line in front of it,
 * etc. Thus the order of the lines is the opposite of the line number.
 */
بنية معلومات_بلاک
{
    short_u	db_id;		// ID for data block: DATA_ID
    بینښه	db_free;	// free space available
    بینښه	db_txt_پیل;	// byte where text starts
    بینښه	db_txt_پای_ټکی;	// byte just after data block
    linenr_T	db_کرښه_شمېر;	// number of lines in this block
    بینښه	db_شاخص[1];	// index for start of line (actually bigger)
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
#تعریف_کړه DB_MARKED	((بینښه)1 << ((حجم(بینښه) * 8) - 1))
#تعریف_کړه DB_شاخص_ماسک	(~DB_MARKED)

#تعریف_کړه شاخص_اندازه  (حجم(بینښه))	    // size of one db_index entry
#تعریف_کړه سرلیک_اندازه (offsetof(معلومات_BL, db_شاخص))  // size of data block header

#تعریف_کړه B0_FNAME_اندازه_ORG	900	// what it was in older versions
#تعریف_کړه B0_FNAME_اندازه_NOCRYPT	898	// 2 bytes used for other things
#تعریف_کړه B0_FNAME_اندازه_CRYPT	890	// 10 bytes used for other things
#تعریف_کړه B0_UNAME_اندازه		40
#تعریف_کړه B0_HNAME_اندازه		40
/*
 * Restrict the numbers to 32 bits, otherwise most compilers will complain.
 * This won't detect a 64 bit machine that only swaps a byte in the top 32
 * bits, but that is crazy anyway.
 */
#تعریف_کړه B0_MAGIC_LONG	0x30313233L
#تعریف_کړه B0_MAGIC_INT	0x20212223L
#تعریف_کړه B0_MAGIC_SHORT	0x10111213L
#تعریف_کړه B0_MAGIC_CHAR	0x55

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
بنية block0
{
    char_u	b0_id[2];	// id for block 0: BLOCK0_ID0 and BLOCK0_ID1,
				// BLOCK0_ID1_C0, BLOCK0_ID1_C1, etc.
    char_u	b0_نسخه[10];	// Vim version string
    char_u	b0_پاڼه_اندازه[4];// number of bytes per page
    char_u	b0_mtime[4];	// last modification time of file
    char_u	b0_ino[4];	// inode of b0_fname
    char_u	b0_pid[4];	// process id of creator (or 0)
    char_u	b0_uname[B0_UNAME_اندازه]; // name of user (uid if no name)
    char_u	b0_hname[B0_HNAME_اندازه]; // host name (if it has a name)
    char_u	b0_fname[B0_FNAME_اندازه_ORG]; // name of file being edited
    اوږد	b0_magic_long;	// check for byte order of long
    صحيح		b0_magic_int;	// check for byte order of int
    لنډ	b0_magic_short;	// check for byte order of short
    char_u	b0_magic_char;	// check for last char
};

/*
 * Note: b0_dirty and b0_flags are put at the end of the file name.  For very
 * long file names in older versions of Vim they are invalid.
 * The 'fileencoding' comes before b0_flags, with a NUL in front.  But only
 * when there is room, for very long file names it's omitted.
 */
#تعریف_کړه B0_DIRTY	0x55
#تعریف_کړه b0_dirty	b0_fname[B0_FNAME_اندازه_ORG - 1]

/*
 * The b0_flags field is new in Vim 7.0.
 */
#تعریف_کړه b0_flags	b0_fname[B0_FNAME_اندازه_ORG - 2]

/*
 * Crypt seed goes here, 8 bytes.  New in Vim 7.3.
 * Without encryption these bytes may be used for 'fenc'.
 */
#تعریف_کړه b0_تخم		b0_fname[B0_FNAME_اندازه_ORG - 2 - MF_تخم_LEN]

// The lowest two bits contain the fileformat.  Zero means it's not set
// (compatible with Vim 6.x), otherwise it's EOL_UNIX + 1, EOL_DOS + 1 or
// EOL_MAC + 1.
#تعریف_کړه B0_FF_ماسک	3

// Swap file is in directory of edited file.  Used to find the file from
// different mount points.
#تعریف_کړه B0_SAME_DIR	4

// The 'fileencoding' is at the end of b0_fname[], with a NUL in front of it.
// When empty there is only the NUL.
#تعریف_کړه B0_HAS_FENC	8

#تعریف_کړه ډېری_INCR	5	// nr of entries added to ml_stack at a time

/*
 * The line number where the first mark may be is remembered.
 * If it is 0 there are no marks at all.
 * (always used for the current buffer only, no buffer change possible while
 * executing a global command).
 */
ساكن linenr_T	ټیټ_ترین_marked = 0;

/*
 * arguments for ml_find_line()
 */
#تعریف_کړه ML_DELETE	0x11	    // delete line
#تعریف_کړه ML_ورننباسه	0x12	    // insert line
#تعریف_کړه ML_پیدا_کړه		0x13	    // just find the line
#تعریف_کړه ML_FLUSH	0x02	    // flush locked block
#تعریف_کړه ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
تعريفنوع تعداد {
      UB_FNAME = 0	// update timestamp and filename
    , UB_SAME_DIR       // update the B0_SAME_DIR flag
    , UB_CRYPT		// update crypt key
} upd_block0_T;

#که_تعریف_شوی FEAT_CRYPT
ساكن فراغ ml_سېټ_b0_crypt(buf_T *buf, ZERO_BL *b0p);
#د_که_پای
ساكن فراغ ml_upd_block0(buf_T *buf, upd_block0_T what);
ساكن فراغ سېټ_b0_fname(ZERO_BL *, buf_T *buf);
ساكن فراغ سېټ_b0_dir_flag(ZERO_BL *b0p, buf_T *buf);
ساكن فراغ جمع_کړه_b0_fenc(ZERO_BL *b0p, buf_T *buf);
ساكن time_t swapfile_info(char_u *);
ساكن صحيح recov_file_نومونه(char_u **, char_u *, صحيح prepend_dot);
ساكن char_u *findswapname(buf_T *, char_u **, char_u *);
ساكن فراغ ml_flush_کرښه(buf_T *);
ساكن bhdr_T *ml_new_معلومات(memfile_T *, صحيح, صحيح);
ساكن bhdr_T *ml_new_ptr(memfile_T *);
ساكن bhdr_T *ml_پیدا_کړه_کرښه(buf_T *, linenr_T, صحيح);
ساكن صحيح ml_جمع_کړه_ډېری(buf_T *);
ساكن فراغ ml_lineadd(buf_T *, صحيح);
ساكن صحيح b0_magic_wrong(ZERO_BL *);
#که_تعریف_شوی CHECK_INODE
ساكن صحيح fnamecmp_ino(char_u *, char_u *, اوږد);
#د_که_پای
ساكن فراغ long_منزل_char(اوږد, char_u *);
ساكن اوږد char_منزل_long(char_u *);
#که_تعریف_شوی FEAT_CRYPT
ساكن cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T دننه_واټن, صحيح reading);
#د_که_پای
#که_تعریف_شوی FEAT_BYTEOFF
ساكن فراغ ml_updatechunk(buf_T *buf, اوږد کرښه, اوږد len, صحيح updtype);
#د_که_پای

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
ml_open(buf_T *buf)
{
    memfile_T	*mfp;
    bhdr_T	*hp = عدم;
    ZERO_BL	*b0p;
    PTR_BL	*pp;
    معلومات_BL	*dp;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ډېری_اندازه = 0; // no stack yet
    buf->b_ml.ml_ډېری = عدم;	// no stack yet
    buf->b_ml.ml_ډېری_سر = 0;	// nothing in the stack
    buf->b_ml.ml_locked = عدم;	// no cached block
    buf->b_ml.ml_کرښه_lnum = 0;	// no cached line
#که_تعریف_شوی FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = عدم;
    buf->b_ml.ml_usedchunks = 0;
#د_که_پای

    إذا (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    إذا (p_uc && buf->b_p_swf)
	buf->b_may_بدلون = true;
    وإلا
	buf->b_may_بدلون = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(عدم, 0);
    إذا (mfp == عدم)
	اذهب error;

    buf->b_ml.ml_mfp = mfp;
#که_تعریف_شوی FEAT_CRYPT
    mfp->mf_بفر = buf;
#د_که_پای
    buf->b_ml.ml_flags = ML_تشه;
    buf->b_ml.ml_کرښه_شمېر = 1;

/*
 * fill block0 struct and write page 0
 */
    إذا ((hp = mf_new(mfp, FALSE, 1)) == عدم)
	اذهب error;
    إذا (hp->bh_bnum != 0)
    {
	iemsg(e_didnt_get_بلاک_nr_zero);
	اذهب error;
    }
    b0p = (ZERO_BL *)(hp->bh_معلومات);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (اوږد)B0_MAGIC_LONG;
    b0p->b0_magic_int = (صحيح)B0_MAGIC_INT;
    b0p->b0_magic_short = (لنډ)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_نسخه, "VIM ", 4);
    STRNCPY(b0p->b0_نسخه + 4, نسخه, 6);
    long_منزل_char((اوږد)mfp->mf_پاڼه_اندازه, b0p->b0_پاڼه_اندازه);

#که_تعریف_شوی FEAT_SPELL
    إذا (!buf->b_spell)
#د_که_پای
    {
	b0p->b0_dirty = buf->b_بدل_شوی ? B0_DIRTY : 0;
	b0p->b0_flags = get_fileformat(buf) + 1;
	سېټ_b0_fname(b0p, buf);
	(فراغ)get_user_name(b0p->b0_uname, B0_UNAME_اندازه);
	b0p->b0_uname[B0_UNAME_اندازه - 1] = NUL;
	mch_get_host_name(b0p->b0_hname, B0_HNAME_اندازه);
	b0p->b0_hname[B0_HNAME_اندازه - 1] = NUL;
	long_منزل_char(mch_get_pid(), b0p->b0_pid);
#که_تعریف_شوی FEAT_CRYPT
	ml_سېټ_b0_crypt(buf, b0p);
#د_که_پای
    }

    /*
     * Always sync block number 0 to disk, so we can check the file name in
     * the swap file in findswapname(). Don't do this for a help files or
     * a spell buffer though.
     * Only works when there's a swapfile, otherwise it's done when the file
     * is created.
     */
    mf_put(mfp, hp, TRUE, FALSE);
    إذا (!buf->b_help && !B_SPELL(buf))
	(فراغ)mf_sync(mfp, 0);

    /*
     * Fill in root pointer block and write page 1.
     */
    إذا ((hp = ml_new_ptr(mfp)) == عدم)
	اذهب error;
    إذا (hp->bh_bnum != 1)
    {
	iemsg(e_didnt_get_بلاک_nr_one);
	اذهب error;
    }
    pp = (PTR_BL *)(hp->bh_معلومات);
    pp->pb_شمېر = 1;
    pp->pb_اشاره_ګر[0].pe_bnum = 2;
    pp->pb_اشاره_ګر[0].pe_پاڼه_شمېر = 1;
    pp->pb_اشاره_ګر[0].pe_old_lnum = 1;
    pp->pb_اشاره_ګر[0].pe_کرښه_شمېر = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    إذا ((hp = ml_new_معلومات(mfp, FALSE, 1)) == عدم)
	اذهب error;
    إذا (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_بلاک_nr_two);
	اذهب error;
    }

    dp = (معلومات_BL *)(hp->bh_معلومات);
    dp->db_شاخص[0] = --dp->db_txt_پیل;	// at end of block
    dp->db_free -= 1 + شاخص_اندازه;
    dp->db_کرښه_شمېر = 1;
    *((char_u *)dp + dp->db_txt_پیل) = NUL;	// empty line

    أرجع OK;

error:
    إذا (mfp != عدم)
    {
	إذا (hp)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, TRUE);	    // will also free(mfp->mf_fname)
    }
    buf->b_ml.ml_mfp = عدم;
    أرجع FAIL;
}

#إذا defined(FEAT_CRYPT)
/*
 * Swapfile encryption is not supported by XChaCha20.  If this crypt method is
 * used then disable the swapfile, to avoid plain text being written to disk,
 * and return TRUE.
 * Otherwise return FALSE.
 */
    ساكن صحيح
crypt_may_close_swapfile(buf_T *buf, char_u *کلی, صحيح لاره)
{
    إذا (crypt_لاره_is_sodium(لاره) && *کلی != NUL)
    {
	mf_close_file(buf, TRUE);
	buf->b_p_swf = FALSE;
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Prepare encryption for "buf" for the current key and method.
 */
    ساكن فراغ
ml_سېټ_mfp_crypt(buf_T *buf)
{
    إذا (*buf->b_p_کلی == NUL)
	أرجع;

    صحيح لاره_nr = crypt_get_لاره_nr(buf);

    إذا (لاره_nr > CRYPT_M_ZIP && لاره_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_تخم(buf->b_ml.ml_mfp->mf_تخم, MF_تخم_LEN, عدم, 0);
    }
# ifdef FEAT_SODIUM
    وإلا إذا (crypt_لاره_is_sodium(لاره_nr))
	crypt_sodium_randombytes_buf(buf->b_ml.ml_mfp->mf_تخم, MF_تخم_LEN);
# endif
}

/*
 * Prepare encryption for "buf" with block 0 "b0p".
 * Note: should not be called with libsodium encryption, since xchacha20 does
 * not support swapfile encryption.
 */
    ساكن فراغ
ml_سېټ_b0_crypt(buf_T *buf, ZERO_BL *b0p)
{
    إذا (*buf->b_p_کلی == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    وإلا
    {
	صحيح لاره_nr = crypt_get_لاره_nr(buf);

	b0p->b0_id[1] = id1_codes[لاره_nr];
	إذا (لاره_nr > CRYPT_M_ZIP && لاره_nr < CRYPT_M_SOD)
	{
	    // Generate a seed and store it in block 0 and in the memfile.
	    sha2_تخم(&b0p->b0_تخم, MF_تخم_LEN, عدم, 0);
	    mch_memmove(buf->b_ml.ml_mfp->mf_تخم, &b0p->b0_تخم, MF_تخم_LEN);
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
    فراغ
ml_سېټ_crypt_کلی(
    buf_T	*buf,
    char_u	*old_کلی,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    صحيح		پاڼه_شمېر;
    صحيح		idx;
    اوږد	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    معلومات_BL	*dp;
    blocknr_T	bnum;
    صحيح		سر_2;
    صحيح		old_لاره;

    إذا (mfp == عدم || mfp->mf_fd < 0)
	أرجع;  // no memfile yet, nothing to do
    old_لاره = crypt_لاره_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    إذا (crypt_may_close_swapfile(buf, buf->b_p_کلی, crypt_get_لاره_nr(buf)))
	أرجع;
# endif

    // First make sure the swapfile is in a consistent state, using the old
    // key and method.
    {
	char_u *new_کلی = buf->b_p_کلی;
	char_u *new_buf_cm = buf->b_p_cm;

	buf->b_p_کلی = old_کلی;
	buf->b_p_cm = old_cm;
	ml_preserve(buf, FALSE);
	buf->b_p_کلی = new_کلی;
	buf->b_p_cm = new_buf_cm;
    }

    // Set the key, method and seed to be used for reading, these must be the
    // old values.
    mfp->mf_old_کلی = old_کلی;
    mfp->mf_old_cm = old_لاره;
    إذا (old_لاره > 0 && *old_کلی != NUL)
	mch_memmove(mfp->mf_old_تخم, mfp->mf_تخم, MF_تخم_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    إذا (mfp->mf_infile_شمېر > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_کرښه(buf);		    // flush buffered line
	(فراغ)ml_پیدا_کړه_کرښه(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = عدم;
	bnum = 1;		// start with block 1
	پاڼه_شمېر = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_ډېری_سر = 0;
	VIM_CLEAR(buf->b_ml.ml_ډېری);
	buf->b_ml.ml_ډېری_اندازه = 0;	// no stack yet

	لكل ( ; !got_int; کرښه_breakcheck())
	{
	    إذا (hp != عدم)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    إذا ((hp = mf_get(mfp, bnum, پاڼه_شمېر)) == عدم)
	    {
		إذا (bnum == 1)
		    اكسر;
		++error;
	    }
	    وإلا
	    {
		pp = (PTR_BL *)(hp->bh_معلومات);
		إذا (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    إذا (pp->pb_شمېر == 0)
		    {
			// empty block?
			++error;
		    }
		    وإلا إذا (idx < (صحيح)pp->pb_شمېر)	// go a block deeper
		    {
			إذا (pp->pb_اشاره_ګر[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    استمر;
			}

			// going one block deeper in the tree, new entry in
			//ډېری
			إذا ((سر_2 = ml_جمع_کړه_ډېری(buf)) < 0)
			{
			    ++error;
			    اكسر;		    // out of memory
			}
			ip = &(buf->b_ml.ml_ډېری[سر_2]);
			ip->ip_bnum = bnum;
			ip->ip_شاخص = idx;

			bnum = pp->pb_اشاره_ګر[idx].pe_bnum;
			پاڼه_شمېر = pp->pb_اشاره_ګر[idx].pe_پاڼه_شمېر;
			idx = 0;
			استمر;
		    }
		}
		وإلا	    // not a pointer block
		{
		    dp = (معلومات_BL *)(hp->bh_معلومات);
		    إذا (dp->db_id != معلومات_ID)	// block id wrong
			++error;
		    وإلا
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = عدم;
		    }
		}
	    }

	    إذا (buf->b_ml.ml_ډېری_سر == 0)	//پای_ته_رسېدلی
		اكسر;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_ډېری[--(buf->b_ml.ml_ډېری_سر)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_شاخص + 1;	    // go to next index
	    پاڼه_شمېر = 1;
	}
	إذا (hp != عدم)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	إذا (error > 0)
	    emsg(_(e_error_while_updating_بدلون_file_crypt));
    }

    mfp->mf_old_کلی = عدم;
}
#د_که_پای

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
    فراغ
ml_setname(buf_T *buf)
{
    صحيح		success = FALSE;
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;
#إذا defined(MSWIN)
    char_u	*p;
#د_که_پای

    mfp = buf->b_ml.ml_mfp;
    إذا (mfp->mf_fd < 0)		    // there is no swap file yet
    {
	/*
	 * When 'updatecount' is 0 and 'noswapfile' there is no swap file.
	 * For help files we will make a swap file now.
	 */
	إذا (p_uc != 0 && (cmdmod.cmod_flags & CMOD_NOSWAPFILE) == 0)
	    ml_open_file(buf);	    // create a swap file
	أرجع;
    }

    /*
     * Try all directories in the 'directory' option.
     */
    dirp = p_dir;
    لكل (;;)
    {
	إذا (*dirp == NUL)	    // tried all directories, fail
	    اكسر;
	fname = findswapname(buf, &dirp, mfp->mf_fname);
	// autocmd may have freed mfp if findswapname creates
	// different swapfile name
	إذا (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    أرجع;
	}
						    // alloc's fname
	إذا (dirp == عدم)	    // out of memory
	    اكسر;
	إذا (fname == عدم)	    // no file name found for this dir
	    استمر;

#إذا defined(MSWIN)
	/*
	 * Set full pathname for swap file now, because a ":!cd dir" may
	 * change directory without us knowing it.
	 */
	p = ډک_Name_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	إذا (fname == عدم)
	    استمر;
#د_که_پای
	// if the file name is the same we don't have to do anything
	إذا (fnamecmp(fname, mfp->mf_fname) == 0)
	{
	    vim_free(fname);
	    success = TRUE;
	    اكسر;
	}
	// need to close the swap file before renaming
	إذا (mfp->mf_fd >= 0)
	{
	    close(mfp->mf_fd);
	    mfp->mf_fd = -1;
	}

	// try to rename the swap file
	إذا (vim_rename(mfp->mf_fname, fname) == 0)
	{
	    success = TRUE;
	    vim_free(mfp->mf_fname);
	    mfp->mf_fname = fname;
	    vim_free(mfp->mf_ffname);
#إذا defined(MSWIN)
	    mfp->mf_ffname = عدم;  // mf_fname is full pathname already
#وإلا
	    mf_سېټ_ffname(mfp);
#د_که_پای
	    ml_upd_block0(buf, UB_SAME_DIR);
	    اكسر;
	}
	vim_free(fname);	    // this fname didn't work, try another
    }

    إذا (mfp->mf_fd == -1)	    // need to (re)open the swap file
    {
	mfp->mf_fd = mch_open((حرف *)mfp->mf_fname, O_RDWR | O_EXTRA, 0);
	إذا (mfp->mf_fd < 0)
	{
	    // could not (re)open the swap file, what can we do????
	    emsg(_(e_oops_ورک_the_بدلون_file));
	    أرجع;
	}
#که_تعریف_شوی HAVE_FD_CLOEXEC
	{
	    صحيح fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    إذا (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(فراغ)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#د_که_پای
    }
    إذا (!success)
	emsg(_(e_could_not_rename_بدلون_file));
}

/*
 * Open a file for the memfile for all buffers that are not readonly or have
 * been modified.
 * Used when 'updatecount' changes from zero to non-zero.
 */
    فراغ
ml_open_files(فراغ)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	إذا (!buf->b_p_ro || buf->b_بدل_شوی)
	    ml_open_file(buf);
}

/*
 * Open a swap file for an existing memfile, if there is no swap file yet.
 * If we are unable to find a file name, mf_fname will be NULL
 * and the memfile will be in memory only (no recovery possible).
 */
    فراغ
ml_open_file(buf_T *buf)
{
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;

    mfp = buf->b_ml.ml_mfp;
    إذا (mfp == عدم || mfp->mf_fd >= 0 || !buf->b_p_swf
				      || (cmdmod.cmod_flags & CMOD_NOSWAPFILE))
	أرجع;		// nothing to do

#که_تعریف_شوی FEAT_SPELL
    // For a spell buffer use a temp file name.
    إذا (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	إذا (fname != عدم)
	    (فراغ)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_بدلون = false;
	أرجع;
    }
#د_که_پای

    /*
     * Try all directories in 'directory' option.
     */
    dirp = p_dir;
    لكل (;;)
    {
	إذا (*dirp == NUL)
	    اكسر;
	// There is a small chance that between choosing the swap file name
	// and creating it, another Vim creates the file.  In that case the
	// creation will fail and we will use another directory.
	fname = findswapname(buf, &dirp, عدم); // allocates fname
	// autocmd may have freed mfp, grr!
	إذا (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    أرجع;
	}
	إذا (dirp == عدم)
	    اكسر;  // out of memory
	إذا (fname == عدم)
	    استمر;
	إذا (mf_open_file(mfp, fname) == OK)	// consumes fname!
	{
	    // don't sync yet in ml_sync_all()
	    mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;
#إذا defined(MSWIN)
	    /*
	     * set full pathname for swap file now, because a ":!cd dir" may
	     * change directory without us knowing it.
	     */
	    mf_fullname(mfp);
#د_که_پای
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    إذا (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_سېټ_dirty(mfp);
		اكسر;
	    }
	    // Writing block 0 failed: close the file and try another dir
	    mf_close_file(buf, FALSE);
	}
    }

    إذا (*p_dir != NUL && mfp->mf_fname == عدم)
    {
	need_wait_return = TRUE;	// call wait_return() later
	++no_wait_return;
	(فراغ)semsg(_(e_unable_منزل_open_بدلون_file_for_str_recovery_impossible),
		    buf_spname(buf) != عدم ? buf_spname(buf) : buf->b_fname);
	--no_wait_return;
    }

    // don't try to open a swap file again
    buf->b_may_بدلون = false;
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
    فراغ
check_need_بدلون(
    صحيح	    newfile)		// reading file into new buffer
{
    صحيح old_پیغام_silent = پیغام_silent; // might be reset by an E325 message

    إذا (curbuf->b_may_بدلون && (!curbuf->b_p_ro || !newfile))
	ml_open_file(curbuf);
    پیغام_silent = old_پیغام_silent;
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
    فراغ
ml_close(buf_T *buf, صحيح del_file)
{
    إذا (buf->b_ml.ml_mfp == عدم)		// not open
	أرجع;
    mf_close(buf->b_ml.ml_mfp, del_file);	// close the .swp file
    إذا (buf->b_ml.ml_کرښه_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_کرښه_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_کرښه_ptr);
    vim_free(buf->b_ml.ml_ډېری);
#که_تعریف_شوی FEAT_BYTEOFF
    VIM_CLEAR(buf->b_ml.ml_chunksize);
#د_که_پای
    buf->b_ml.ml_mfp = عدم;

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
    فراغ
ml_close_all(صحيح del_file)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ml_close(buf, del_file && ((buf->b_flags & BF_PRESERVED) == 0
				 || vim_strchr(p_cpo, CPO_PRESERVE) == عدم));
#که_تعریف_شوی FEAT_SPELL
    spell_delete_wordlist();	// delete the internal wordlist
#د_که_پای
#که_تعریف_شوی TEMPDIRNAMES
    vim_deltempdir();		// delete created temp directory
#د_که_پای
}

/*
 * Close all memfiles for not modified buffers.
 * Only use just before exiting!
 */
    فراغ
ml_close_notmod(فراغ)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	إذا (!buf_Is_بدل_شوی(buf))
	    ml_close(buf, TRUE);    // close all not-modified buffers
}

/*
 * Update the timestamp in the .swp file.
 * Used when the file has been written.
 */
    فراغ
ml_timestamp(buf_T *buf)
{
    ml_upd_block0(buf, UB_FNAME);
}

/*
 * Return FAIL when the ID of "b0p" is wrong.
 */
    ساكن صحيح
ml_check_b0_id(ZERO_BL *b0p)
{
    إذا (b0p->b0_id[0] != BLOCK0_ID0
	    || (b0p->b0_id[1] != BLOCK0_ID1
		&& b0p->b0_id[1] != BLOCK0_ID1_C0
		&& b0p->b0_id[1] != BLOCK0_ID1_C1
		&& b0p->b0_id[1] != BLOCK0_ID1_C2
		&& b0p->b0_id[1] != BLOCK0_ID1_C3
		&& b0p->b0_id[1] != BLOCK0_ID1_C4)
	    )
	أرجع FAIL;
    أرجع OK;
}

/*
 * Update the timestamp or the B0_SAME_DIR flag of the .swp file.
 */
    ساكن فراغ
ml_upd_block0(buf_T *buf, upd_block0_T what)
{
    memfile_T	*mfp;
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    mfp = buf->b_ml.ml_mfp;
    إذا (mfp == عدم)
	أرجع;
    hp = mf_get(mfp, (blocknr_T)0, 1);
    إذا (hp == عدم)
    {
#که_تعریف_شوی FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	إذا (what == UB_CRYPT)
	    ml_سېټ_mfp_crypt(buf);
#د_که_پای
	أرجع;
    }

    b0p = (ZERO_BL *)(hp->bh_معلومات);
    إذا (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_بلاک_zero);
    وإلا
    {
	إذا (what == UB_FNAME)
	    سېټ_b0_fname(b0p, buf);
#که_تعریف_شوی FEAT_CRYPT
	وإلا إذا (what == UB_CRYPT)
	    ml_سېټ_b0_crypt(buf, b0p);
#د_که_پای
	وإلا // what == UB_SAME_DIR
	    سېټ_b0_dir_flag(b0p, buf);
    }
    mf_put(mfp, hp, TRUE, FALSE);
}

/*
 * Write file name and timestamp into block 0 of a swap file.
 * Also set buf->b_mtime.
 * Don't use NameBuff[]!!!
 */
    ساكن فراغ
سېټ_b0_fname(ZERO_BL *b0p, buf_T *buf)
{
    stat_T	st;

    إذا (buf->b_ffname == عدم)
	b0p->b0_fname[0] = NUL;
    وإلا
    {
#إذا defined(MSWIN) || defined(AMIGA)
	// Systems that cannot translate "~user" back into a path: copy the
	// file name unmodified.  Do use slashes instead of backslashes for
	// portability.
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_اندازه_CRYPT - 1);
# ifdef BACKSLASH_دننه_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#وإلا
	size_t	flen;
	char_u	uname[B0_UNAME_اندازه];

	/*
	 * For a file under the home directory of the current user, we try to
	 * replace the home directory path with "~user". This helps when
	 * editing the same file on different machines over a network.
	 * First replace home dir path with "~/" with home_replace().
	 * Then insert the user name to get "~user/".
	 */
	flen = home_replace(عدم, buf->b_ffname, b0p->b0_fname,
						   B0_FNAME_اندازه_CRYPT, TRUE);
	إذا (b0p->b0_fname[0] == '~')
	{
	    size_t  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    إذا (get_user_name(uname, B0_UNAME_اندازه) == FAIL
		   || (ulen = STRLEN(uname)) + flen > B0_FNAME_اندازه_CRYPT - 1)
		vim_strncpy(b0p->b0_fname, buf->b_ffname,
						     B0_FNAME_اندازه_CRYPT - 1);
	    وإلا
	    {
		mch_memmove(b0p->b0_fname + ulen + 1, b0p->b0_fname + 1, flen);
		mch_memmove(b0p->b0_fname + 1, uname, ulen);
	    }
	}
#د_که_پای
	إذا (mch_stat((حرف *)buf->b_ffname, &st) >= 0)
	{
	    long_منزل_char((اوږد)st.st_mtime, b0p->b0_mtime);
#که_تعریف_شوی CHECK_INODE
	    long_منزل_char((اوږد)st.st_ino, b0p->b0_ino);
#د_که_پای
	    buf_store_وخت(buf, &st, buf->b_ffname);
	    buf->b_mtime_read = buf->b_mtime;
	    buf->b_mtime_read_ns = buf->b_mtime_ns;
	}
	وإلا
	{
	    long_منزل_char(0L, b0p->b0_mtime);
#که_تعریف_شوی CHECK_INODE
	    long_منزل_char(0L, b0p->b0_ino);
#د_که_پای
	    buf->b_mtime = 0;
	    buf->b_mtime_ns = 0;
	    buf->b_mtime_read = 0;
	    buf->b_mtime_read_ns = 0;
	    buf->b_orig_اندازه = 0;
	    buf->b_orig_ډېرپېښ = 0;
	}
    }

    // Also add the 'fileencoding' if there is room.
    جمع_کړه_b0_fenc(b0p, curbuf);
}

/*
 * Update the B0_SAME_DIR flag of the swap file.  It's set if the file and the
 * swapfile for "buf" are in the same directory.
 * This is fail safe: if we are not sure the directories are equal the flag is
 * not set.
 */
    ساكن فراغ
سېټ_b0_dir_flag(ZERO_BL *b0p, buf_T *buf)
{
    إذا (same_directory(buf->b_ml.ml_mfp->mf_fname, buf->b_ffname))
	b0p->b0_flags |= B0_SAME_DIR;
    وإلا
	b0p->b0_flags &= ~B0_SAME_DIR;
}

/*
 * When there is room, add the 'fileencoding' to block zero.
 */
    ساكن فراغ
جمع_کړه_b0_fenc(
    ZERO_BL	*b0p,
    buf_T	*buf)
{
    صحيح		n;
    صحيح		اندازه = B0_FNAME_اندازه_NOCRYPT;

#که_تعریف_شوی FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    إذا (*buf->b_p_کلی != NUL)
	اندازه = B0_FNAME_اندازه_CRYPT;
#د_که_پای

    n = (صحيح)STRLEN(buf->b_p_fenc);
    إذا ((صحيح)STRLEN(b0p->b0_fname) + n + 1 > اندازه)
	b0p->b0_flags &= ~B0_HAS_FENC;
    وإلا
    {
	mch_memmove((حرف *)b0p->b0_fname + اندازه - n,
					    (حرف *)buf->b_p_fenc, (size_t)n);
	*(b0p->b0_fname + اندازه - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#إذا defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#د_که_پای

#إذا defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    ساكن صحيح
swapfile_process_running(ZERO_BL *b0p, char_u *بدلون_fname ناکارول_شوی)
{
# إذا defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    بنية sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    إذا (mch_stat((حرف *)بدلون_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < وخت(عدم) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	أرجع FALSE;
# endif
    أرجع mch_process_running(char_منزل_long(b0p->b0_pid));
}
#د_که_پای

/*
 * Try to recover curbuf from the .swp file.
 * If "checkext" is TRUE, check the extension and detect whether it is
 * a swap file.
 */
    فراغ
ml_recover(صحيح checkext)
{
    buf_T	*buf = عدم;
    memfile_T	*mfp = عدم;
    char_u	*fname;
    char_u	*fname_کارول_شوی = عدم;
    bhdr_T	*hp = عدم;
    ZERO_BL	*b0p;
    صحيح		b0_ff;
    char_u	*b0_fenc = عدم;
#که_تعریف_شوی FEAT_CRYPT
    صحيح		b0_cm = -1;
#د_که_پای
    PTR_BL	*pp;
    معلومات_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    صحيح		پاڼه_شمېر;
    stat_T	org_stat, swp_stat;
    صحيح		len;
    صحيح		directly;
    linenr_T	lnum;
    char_u	*p;
    صحيح		i;
    اوږد	error;
    صحيح		cannot_open;
    linenr_T	کرښه_شمېر;
    صحيح		has_error;
    صحيح		idx;
    صحيح		سر_2;
    صحيح		txt_پیل;
    off_T	اندازه;
    صحيح		called_from_main;
    صحيح		serious_error = TRUE;
    اوږد	mtime;
    صحيح		attr;
    صحيح		orig_file_حالت = NOTDONE;

    recoverymode = TRUE;
    called_from_main = (curbuf->b_ml.ml_mfp == عدم);
    attr = HL_ATTR(HLF_E);

    /*
     * If the file name ends in ".s[a-w][a-z]" we assume this is the swap file.
     * Otherwise a search is done to find the swap file(s).
     */
    fname = curbuf->b_fname;
    إذا (fname == عدم)		    // When there is no file name
	fname = (char_u *)"";
    len = (صحيح)STRLEN(fname);
    إذا (checkext && len >= 4 &&
#إذا defined(VMS)
	    STRNICMP(fname + len - 4, "_s", 2)
#وإلا
	    STRNICMP(fname + len - 4, ".s", 2)
#د_که_پای
						== 0
		&& vim_strchr((char_u *)"abcdefghijklmnopqrstuvw",
					   TOLOWER_ASC(fname[len - 2])) != عدم
		&& ASCII_ISALPHA(fname[len - 1]))
    {
	directly = TRUE;
	fname_کارول_شوی = vim_strsave(fname); // make a copy for mf_open()
    }
    وإلا
    {
	directly = FALSE;

	// count the number of matching swap files
	len = recover_نومونه(fname, FALSE, عدم, 0, عدم);
	إذا (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_بدلون_file_وموندل_for_str), fname);
	    اذهب theend;
	}
	إذا (len == 1)		    // one swap file found, use it
	    i = 1;
	وإلا			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (فراغ)recover_نومونه(fname, TRUE, عدم, 0, عدم);
	    پیغام_putchar('\n');
	    پیغام_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_شمېره(FALSE, عدم);
	    إذا (i < 1 || i > len)
		اذهب theend;
	}
	// get the swap file name that will be used
	(فراغ)recover_نومونه(fname, FALSE, عدم, i, &fname_کارول_شوی);
    }
    إذا (fname_کارول_شوی == عدم)
	اذهب theend;			// out of memory

    // When called from main() still need to initialize storage structure
    إذا (called_from_main && ml_open(curbuf) == FAIL)
	getout(1);

    /*
     * Allocate a buffer structure for the swap file that is used for recovery.
     * Only the memline and crypt information in it are really used.
     */
    buf = ALLOC_ONE(buf_T);
    إذا (buf == عدم)
	اذهب theend;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ډېری_اندازه = 0;	// no stack yet
    buf->b_ml.ml_ډېری = عدم;		// no stack yet
    buf->b_ml.ml_ډېری_سر = 0;		// nothing in the stack
    buf->b_ml.ml_کرښه_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = عدم;		// no locked block
    buf->b_ml.ml_flags = 0;
#که_تعریف_شوی FEAT_CRYPT
    buf->b_p_کلی = تشه_option;
    buf->b_p_cm = تشه_option;
#د_که_پای

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_کارول_شوی); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_کارول_شوی, O_RDONLY);
    fname_کارول_شوی = p;
    إذا (mfp == عدم || mfp->mf_fd < 0)
    {
	إذا (fname_کارول_شوی != عدم)
	    semsg(_(e_cannot_open_str), fname_کارول_شوی);
	اذهب theend;
    }
    buf->b_ml.ml_mfp = mfp;
#که_تعریف_شوی FEAT_CRYPT
    mfp->mf_بفر = buf;
#د_که_پای

    /*
     * The page size set in mf_open() might be different from the page size
     * used in the swap file, we must get it from block 0.  But to read block
     * 0 we need a page size.  Use the minimal size for block 0 here, it will
     * be set to the real value below.
     */
    mfp->mf_پاڼه_اندازه = MIN_بدلون_پاڼه_اندازه;

    /*
     * try to read block 0
     */
    إذا ((hp = mf_get(mfp, (blocknr_T)0, 1)) == عدم)
    {
	پیغام_پیل();
	پیغام_puts_attr(_("Unable to read block 0 from "), attr | پیغام_HIST);
	پیغام_outtrans_attr(mfp->mf_fname, attr | پیغام_HIST);
	پیغام_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | پیغام_HIST);
	پیغام_پای_ټکی();
	اذهب theend;
    }
    b0p = (ZERO_BL *)(hp->bh_معلومات);
    إذا (STRNCMP(b0p->b0_نسخه, "VIM 3.0", 7) == 0)
    {
	پیغام_پیل();
	پیغام_outtrans_attr(mfp->mf_fname, پیغام_HIST);
	پیغام_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    پیغام_HIST);
	پیغام_puts_attr(_("Use Vim version 3.0.\n"), پیغام_HIST);
	پیغام_پای_ټکی();
	اذهب theend;
    }
    إذا (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_بدلون_file), mfp->mf_fname);
	اذهب theend;
    }
    إذا (b0_magic_wrong(b0p))
    {
	پیغام_پیل();
	پیغام_outtrans_attr(mfp->mf_fname, attr | پیغام_HIST);
#إذا defined(MSWIN)
	إذا (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    پیغام_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | پیغام_HIST);
	وإلا
#د_که_پای
	    پیغام_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | پیغام_HIST);
	پیغام_puts_attr(_("The file was created on "), attr | پیغام_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	پیغام_puts_attr((حرف *)b0p->b0_hname, attr | پیغام_HIST);
	پیغام_puts_attr(_(",\nor the file has been damaged."), attr | پیغام_HIST);
	پیغام_پای_ټکی();
	اذهب theend;
    }

#که_تعریف_شوی FEAT_CRYPT
    لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(id1_codes); ++i)
	إذا (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    إذا (b0_cm > 0)
	mch_memmove(mfp->mf_تخم, &b0p->b0_تخم, MF_تخم_LEN);
    crypt_سېټ_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#وإلا
    إذا (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_نسخه_د_vim_does_not_support_encryption), mfp->mf_fname);
	اذهب theend;
    }
#د_که_پای

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    إذا (mfp->mf_پاڼه_اندازه != (بینښه)char_منزل_long(b0p->b0_پاڼه_اندازه))
    {
	بینښه پخوانی_پاڼه_اندازه = mfp->mf_پاڼه_اندازه;

	mf_new_پاڼه_اندازه(mfp, (بینښه)char_منزل_long(b0p->b0_پاڼه_اندازه));
	إذا (mfp->mf_پاڼه_اندازه < پخوانی_پاڼه_اندازه)
	{
	    پیغام_پیل();
	    پیغام_outtrans_attr(mfp->mf_fname, attr | پیغام_HIST);
	    پیغام_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | پیغام_HIST);
	    پیغام_پای_ټکی();
	    اذهب theend;
	}
	إذا ((اندازه = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	وإلا
	    mfp->mf_blocknr_max = (blocknr_T)(اندازه / mfp->mf_پاڼه_اندازه);
	mfp->mf_infile_شمېر = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_پاڼه_اندازه);
	إذا (p == عدم)
	    اذهب theend;
	mch_memmove(p, hp->bh_معلومات, پخوانی_پاڼه_اندازه);
	vim_free(hp->bh_معلومات);
	hp->bh_معلومات = p;
	b0p = (ZERO_BL *)(hp->bh_معلومات);
    }

    /*
     * If .swp file name given directly, use name from swap file for buffer.
     */
    إذا (directly)
    {
	expand_env(b0p->b0_fname, NameBuff, MAXPATHL);
	إذا (setfname(curbuf, NameBuff, عدم, TRUE) == FAIL)
	    اذهب theend;
    }

    home_replace(عدم, mfp->mf_fname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Using swap file \"%s\""), NameBuff);

    إذا (buf_spname(curbuf) != عدم)
	vim_strncpy(NameBuff, buf_spname(curbuf), MAXPATHL - 1);
    وإلا
	home_replace(عدم, curbuf->b_ffname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Original file \"%s\""), NameBuff);
    پیغام_putchar('\n');

    /*
     * check date of swap file and original file
     */
    mtime = char_منزل_long(b0p->b0_mtime);
    إذا (curbuf->b_ffname != عدم
	    && mch_stat((حرف *)curbuf->b_ffname, &org_stat) != -1
	    && ((mch_stat((حرف *)mfp->mf_fname, &swp_stat) != -1
		    && org_stat.st_mtime > swp_stat.st_mtime)
		|| org_stat.st_mtime != mtime))
	emsg(_(e_warning_original_file_may_have_been_بدل_شوی));
    بهر_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_ماسک);
    إذا (b0p->b0_flags & B0_HAS_FENC)
    {
	صحيح fnsize = B0_FNAME_اندازه_NOCRYPT;

#که_تعریف_شوی FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	إذا (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_اندازه_CRYPT;
#د_که_پای
	لكل (p = b0p->b0_fname + fnsize; p > b0p->b0_fname && p[-1] != NUL; --p)
	    ;
	b0_fenc = vim_strnsave(p, b0p->b0_fname + fnsize - p);
    }

    mf_put(mfp, hp, FALSE, FALSE);	// release block 0
    hp = عدم;

    /*
     * Now that we are sure that the file is going to be recovered, clear the
     * contents of the current buffer.
     */
    طالما (!(curbuf->b_ml.ml_flags & ML_تشه))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    إذا (curbuf->b_ffname != عدم)
	orig_file_حالت = readfile(curbuf->b_ffname, عدم, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, عدم, READ_NEW);

#که_تعریف_شوی FEAT_CRYPT
    إذا (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	إذا (*curbuf->b_p_کلی != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_کارول_شوی);
	    پیغام_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    پیغام_puts(_("\nenter the new crypt key."));
	    پیغام_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    پیغام_puts(_("\nto use the same key for text file and swap file"));
	}
	وإلا
	    smsg(_(need_کلی_پیغام), fname_کارول_شوی);
	buf->b_p_کلی = crypt_get_کلی(FALSE, FALSE);
	إذا (buf->b_p_کلی == عدم)
	    buf->b_p_کلی = curbuf->b_p_کلی;
	وإلا إذا (*buf->b_p_کلی == NUL)
	{
	    vim_free(buf->b_p_کلی);
	    buf->b_p_کلی = curbuf->b_p_کلی;
	}
	إذا (buf->b_p_کلی == عدم)
	    buf->b_p_کلی = تشه_option;
    }
#د_که_پای

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    إذا (b0_ff != 0)
	سېټ_fileformat(b0_ff - 1, OPT_LOCAL);
    إذا (b0_fenc != عدم)
    {
	سېټ_option_ارزښت_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    پاڼه_شمېر = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    کرښه_شمېر = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_ډېری_سر = 0;
    buf->b_ml.ml_ډېری = عدم;
    buf->b_ml.ml_ډېری_اندازه = 0;	// no stack yet

    إذا (curbuf->b_ffname == عدم)
	cannot_open = TRUE;
    وإلا
	cannot_open = FALSE;

    serious_error = FALSE;
    لكل ( ; !got_int; کرښه_breakcheck())
    {
	إذا (hp != عدم)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	إذا ((hp = mf_get(mfp, bnum, پاڼه_شمېر)) == عدم)
	{
	    إذا (bnum == 1)
	    {
		semsg(_(e_unable_منزل_read_بلاک_one_from_str), mfp->mf_fname);
		اذهب theend;
	    }
	    ++error;
	    ml_append(lnum++, (char_u *)_("???MANY LINES MISSING"),
							    (colnr_T)0, TRUE);
	}
	وإلا		// there is a block
	{
	    pp = (PTR_BL *)(hp->bh_معلومات);
	    إذا (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		صحيح ptr_بلاک_error = FALSE;
		إذا (pp->pb_شمېر_max != PB_شمېر_MAX(mfp))
		{
		    ptr_بلاک_error = TRUE;
		    pp->pb_شمېر_max = PB_شمېر_MAX(mfp);
		}
		إذا (pp->pb_شمېر > pp->pb_شمېر_max)
		{
		    ptr_بلاک_error = TRUE;
		    pp->pb_شمېر = pp->pb_شمېر_max;
		}
		إذا (ptr_بلاک_error)
		    emsg(_(e_warning_اشاره_ګر_بلاک_corrupted));

		// check line count when using pointer block first time
		إذا (idx == 0 && کرښه_شمېر != 0)
		{
		    لكل (i = 0; i < (صحيح)pp->pb_شمېر; ++i)
			کرښه_شمېر -= pp->pb_اشاره_ګر[i].pe_کرښه_شمېر;
		    إذا (کرښه_شمېر != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		إذا (pp->pb_شمېر == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		وإلا إذا (idx < (صحيح)pp->pb_شمېر)	// go a block deeper
		{
		    إذا (pp->pb_اشاره_ګر[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			إذا (!cannot_open)
			{
			    کرښه_شمېر = pp->pb_اشاره_ګر[idx].pe_کرښه_شمېر;
			    linenr_T pe_old_lnum = pp->pb_اشاره_ګر[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    إذا (کرښه_شمېر <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, عدم, lnum,
					pe_old_lnum - 1,
					کرښه_شمېر, عدم, 0) != OK)
				cannot_open = TRUE;
			    وإلا
				lnum += کرښه_شمېر;
			}
			إذا (cannot_open)
			{
			    ++error;
			    ml_append(lnum++, (char_u *)_("???LINES MISSING"),
							    (colnr_T)0, TRUE);
			}
			++idx;	    // get same block again for next index
			استمر;
		    }

		    /*
		     * going one block deeper in the tree
		     */
		    إذا ((سر_2 = ml_جمع_کړه_ډېری(buf)) < 0)	// new entry in stack
		    {
			++error;
			اكسر;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_ډېری[سر_2]);
		    ip->ip_bnum = bnum;
		    ip->ip_شاخص = idx;

		    bnum = pp->pb_اشاره_ګر[idx].pe_bnum;
		    کرښه_شمېر = pp->pb_اشاره_ګر[idx].pe_کرښه_شمېر;
		    پاڼه_شمېر = pp->pb_اشاره_ګر[idx].pe_پاڼه_شمېر;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    إذا (پاڼه_شمېر < 1
			    || bnum + پاڼه_شمېر > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_شاخص + 1;
			bnum = ip->ip_bnum;
			پاڼه_شمېر = 1;
			--buf->b_ml.ml_ډېری_سر;
			استمر;
		    }
		    idx = 0;
		    استمر;
		}
	    }
	    وإلا	    // not a pointer block
	    {
		dp = (معلومات_BL *)(hp->bh_معلومات);
		إذا (dp->db_id != معلومات_ID)	// block id wrong
		{
		    إذا (bnum == 1)
		    {
			semsg(_(e_بلاک_one_id_wrong_str_not_swp_file),
							       mfp->mf_fname);
			اذهب theend;
		    }
		    ++error;
		    ml_append(lnum++, (char_u *)_("???BLOCK MISSING"),
							    (colnr_T)0, TRUE);
		}
		وإلا
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
		    إذا (hp->bh_پاڼه_شمېر != پاڼه_شمېر)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			پاڼه_شمېر = hp->bh_پاڼه_شمېر;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    إذا (پاڼه_شمېر * mfp->mf_پاڼه_اندازه != dp->db_txt_پای_ټکی)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_پای_ټکی = پاڼه_شمېر * mfp->mf_پاڼه_اندازه;
		    }

		    إذا (dp->db_txt_پیل < سرلیک_اندازه
			    || dp->db_txt_پیل > dp->db_txt_پای_ټکی)
		    {
			ml_append(lnum++, (char_u *)_("??? block header corrupted"),
								    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_پیل = dp->db_txt_پای_ټکی;
		    }
		    // Make sure there is a NUL at the end of the block so we
		    // don't go over the end when copying text.
		    *((char_u *)dp + dp->db_txt_پای_ټکی - 1) = NUL;

		    // Check the number of lines in the block.
		    // If wrong, use the count in the data block.
		    إذا (کرښه_شمېر != dp->db_کرښه_شمېر)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    صحيح did_questions = FALSE;
		    لكل (i = 0; i < dp->db_کرښه_شمېر; ++i)
		    {
			إذا ((char_u *)&(dp->db_شاخص[i])
					    >= (char_u *)dp + dp->db_txt_پیل)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    اكسر;
			}

			txt_پیل = (dp->db_شاخص[i] & DB_شاخص_ماسک);
			إذا (txt_پیل <= (صحيح)سرلیک_اندازه
					  || txt_پیل >= (صحيح)dp->db_txt_پای_ټکی)
			{
			    ++error;
			    // avoid lots of lines with "???"
			    إذا (did_questions)
				استمر;
			    did_questions = TRUE;
			    p = (char_u *)"???";
			}
			وإلا
			{
			    did_questions = FALSE;
			    p = (char_u *)dp + txt_پیل;
			}
			ml_append(lnum++, p, (colnr_T)0, TRUE);
		    }
		    إذا (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	إذا (buf->b_ml.ml_ډېری_سر == 0)	//پای_ته_رسېدلی
	    اكسر;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_ډېری[--(buf->b_ml.ml_ډېری_سر)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_شاخص + 1;	    // go to next index
	پاڼه_شمېر = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    إذا (orig_file_حالت != OK || curbuf->b_ml.ml_کرښه_شمېر != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	إذا (!(curbuf->b_ml.ml_کرښه_شمېر == 2 && *ml_get(1) == NUL))
	{
	    بدل_شوی_دننی();
	    ++CHANGEDTICK(curbuf);
	}
    }
    وإلا
    {
	لكل (idx = 1; idx <= lnum; ++idx)
	{
	    // Need to copy one line, fetching the other one may flush it.
	    p = vim_strnsave(ml_get(idx), ml_get_len(idx));
	    i = STRCMP(p, ml_get(idx + lnum));
	    vim_free(p);
	    إذا (i != 0)
	    {
		بدل_شوی_دننی();
		++CHANGEDTICK(curbuf);
		اكسر;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    طالما (curbuf->b_ml.ml_کرښه_شمېر > lnum
				       && !(curbuf->b_ml.ml_flags & ML_تشه))
	ml_delete(curbuf->b_ml.ml_کرښه_شمېر);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    إذا (got_int)
	emsg(_(e_recovery_interrupted));
    وإلا إذا (error)
    {
	++no_wait_return;
	پیغام(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_کرښې_starting_with_questions));
	--no_wait_return;
	پیغام(_("See \":help E312\" for more information."));
	پیغام(">>>>>>>>>>>>>");
    }
    وإلا
    {
	إذا (curbuf->b_بدل_شوی)
	{
	    پیغام(_("Recovery completed. You should check if everything is OK."));
	    پیغام_puts(_("\n(You might want to write out this file under another name\n"));
	    پیغام_puts(_("and run diff with the original file to check for changes)"));
	}
	وإلا
	    پیغام(_("Recovery completed. Buffer contents equals file contents."));
	پیغام_puts(_("\nYou may want to delete the .swp file now."));
#إذا defined(UNIX) || defined(MSWIN)
	إذا (swapfile_process_running(b0p, fname_کارول_شوی))
	{
	    // Warn there could be an active Vim on the same file, the user may
	    // want to kill it.
	    پیغام_puts(_("\nNote: process STILL RUNNING: "));
	    پیغام_outnum(char_منزل_long(b0p->b0_pid));
	}
#د_که_پای
	پیغام_puts("\n\n");
	cmdline_لیکه = پیغام_لیکه;
    }
#که_تعریف_شوی FEAT_CRYPT
    إذا (*buf->b_p_کلی != NUL && STRCMP(curbuf->b_p_کلی, buf->b_p_کلی) != 0)
    {
	پیغام_puts(_("Using crypt key from swap file for the text file.\n"));
	سېټ_option_ارزښت_give_err((char_u *)"key", 0L, buf->b_p_کلی, OPT_LOCAL);
    }
#د_که_پای
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_کارول_شوی);
    recoverymode = FALSE;
    إذا (mfp != عدم)
    {
	إذا (hp != عدم)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    إذا (buf != عدم)
    {
#که_تعریف_شوی FEAT_CRYPT
	إذا (buf->b_p_کلی != curbuf->b_p_کلی)
	    free_string_option(buf->b_p_کلی);
	free_string_option(buf->b_p_cm);
#د_که_پای
	vim_free(buf->b_ml.ml_ډېری);
	vim_free(buf);
    }
    إذا (serious_error && called_from_main)
	ml_close(curbuf, TRUE);
    وإلا
    {
	apply_autocmds(EVENT_BUFREADPOST, عدم, curbuf->b_fname, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, عدم, curbuf->b_fname, FALSE, curbuf);
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
    صحيح
recover_نومونه(
    char_u	*fname,		// base for swap file name
    صحيح		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list ناکارول_شوی, // when not NULL add file names to it
    صحيح		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_بهر)	// result when "nr" > 0
{
    صحيح		عدد_نومونه;
    char_u	*(نومونه[6]);
    char_u	*p;
    صحيح		عدد_files;
    صحيح		file_شمېر = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = عدم;
#که_تعریف_شوی HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#د_که_پای

    إذا (fname != عدم)
    {
#که_تعریف_شوی HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	إذا (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	وإلا
#د_که_پای
	    fname_res = fname;
    }

    إذا (do_list)
    {
	// use msg() to start the scrolling properly
	پیغام(_("Swap files found:"));
	پیغام_putchar('\n');
    }

    /*
     * Do the loop for every directory in 'directory'.
     * First allocate some memory to put the directory name in.
     */
    dir_name.string = alloc(STRLEN(p_dir) + 1);
    dirp = p_dir;
    طالما (dir_name.string != عدم && *dirp)
    {
	/*
	 * Isolate a directory name from *dirp and put it in dir_name (we know
	 * it is large enough, so use 31000 for length).
	 * Advance dirp to next directory name.
	 */
	dir_name.اوږدتیا = (size_t)copy_option_part(&dirp, dir_name.string, 31000, ",");

	إذا (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    إذا (fname == عدم)
	    {
#که_تعریف_شوی VMS
		نومونه[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#وإلا
		نومونه[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#د_که_پای
#إذا defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		نومونه[1] = vim_strnsave((char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"));
		نومونه[2] = vim_strnsave((char_u *)".sw?", STRLEN_LITERAL(".sw?"));
		عدد_نومونه = 3;
#وإلا
# ifdef VMS
		نومونه[1] = vim_strnsave((char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"));
		عدد_نومونه = 2;
# وإلا
		عدد_نومونه = 1;
# endif
#د_که_پای
	    }
	    وإلا
		عدد_نومونه = recov_file_نومونه(نومونه, fname_res, TRUE);
	}
	وإلا			    // check directory dir_name
	{
	    إذا (fname == عدم)
	    {
		string_T    ret;

#که_تعریف_شوی VMS
		نومونه[0] = concat_fnames(dir_name.string, dir_name.اوږدتیا,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#وإلا
		نومونه[0] = concat_fnames(dir_name.string, dir_name.اوږدتیا,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#د_که_پای
#إذا defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		نومونه[1] = concat_fnames(dir_name.string, dir_name.اوږدتیا,
		    (char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"), TRUE, &ret);
		نومونه[2] = concat_fnames(dir_name.string, dir_name.اوږدتیا,
		    (char_u *)".sw?", STRLEN_LITERAL(".sw?"), TRUE, &ret);
		عدد_نومونه = 3;
#وإلا
# ifdef VMS
		نومونه[1] = concat_fnames(dir_name.string, dir_name.اوږدتیا,
		    (char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"), TRUE, &ret);
		عدد_نومونه = 2;
# وإلا
		عدد_نومونه = 1;
# endif
#د_که_پای
	    }
	    وإلا
	    {
		char_u	*لکۍ;

#إذا defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.اوږدتیا;
		إذا (after_pathsep(dir_name.string, p) && dir_name.اوږدتیا > 1 && p[-1] == p[-2])
		{
		    // Ends with '//', Use Full path for swap name
		    لکۍ = make_percent_swname(dir_name.string, p, fname_res);
		}
		وإلا
#د_که_پای
		{
		    string_T	ret;

		    لکۍ = gettail(fname_res);
		    لکۍ = concat_fnames(dir_name.string, dir_name.اوږدتیا,
			لکۍ, STRLEN(لکۍ), TRUE, &ret);
		}
		إذا (لکۍ == عدم)
		    عدد_نومونه = 0;
		وإلا
		{
		    عدد_نومونه = recov_file_نومونه(نومونه, لکۍ, FALSE);
		    vim_free(لکۍ);
		}
	    }
	}

	// check for out-of-memory
	لكل (صحيح i = 0; i < عدد_نومونه; ++i)
	{
	    إذا (نومونه[i] == عدم)
	    {
		لكل (i = 0; i < عدد_نومونه; ++i)
		    vim_free(نومونه[i]);
		عدد_نومونه = 0;
	    }
	}
	إذا (عدد_نومونه == 0)
	    عدد_files = 0;
	وإلا إذا (expand_wildcards(عدد_نومونه, نومونه, &عدد_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    عدد_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	إذا (*dirp == NUL && file_شمېر + عدد_files == 0 && fname != عدم)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#إذا defined(VMS)
			       (char_u *)"_swp", FALSE
#وإلا
			       (char_u *)".swp", TRUE
#د_که_پای
			      );
	    إذا (swapname != عدم)
	    {
		إذا (mch_stat((حرف *)swapname, &st) != -1)    // It exists!
		{
		    files = ALLOC_ONE(char_u *);
		    إذا (files != عدم)
		    {
			files[0] = swapname;
			swapname = عدم;
			عدد_files = 1;
		    }
		}
		vim_free(swapname);
	    }
	}

	/*
	 * Remove swapfile name of the current buffer, it must be ignored.
	 * But keep it for swapfilelist().
	 */
	إذا (curbuf->b_ml.ml_mfp != عدم
			       && (p = curbuf->b_ml.ml_mfp->mf_fname) != عدم
			       && ret_list == عدم)
	{
	    لكل (صحيح i = 0; i < عدد_files; ++i)
		// Do not expand wildcards, on windows would try to expand
		// "%tmp%" in "%tmp%file".
		إذا (fullpathcmp(p, files[i], TRUE, FALSE) & FPC_SAME)
		{
		    // Remove the name from files[i].  Move further entries
		    // down.  When the array becomes empty free it here, since
		    // FreeWild() won't be called below.
		    vim_free(files[i]);
		    إذا (--عدد_files == 0)
			vim_free(files);
		    وإلا
			لكل ( ; i < عدد_files; ++i)
			    files[i] = files[i + 1];
		}
	}
	إذا (nr > 0)
	{
	    file_شمېر += عدد_files;
	    إذا (nr <= file_شمېر)
	    {
		*fname_بهر = vim_strsave(
				      files[nr - 1 + عدد_files - file_شمېر]);
		dirp = (char_u *)"";		    // stop searching
	    }
	}
	وإلا إذا (do_list)
	{
	    إذا (dir_name.string[0] == '.' && dir_name.string[1] == NUL)
	    {
		إذا (fname == عدم)
		    پیغام_puts(_("   In current directory:\n"));
		وإلا
		    پیغام_puts(_("   Using specified name:\n"));
	    }
	    وإلا
	    {
		پیغام_puts(_("   In directory "));
		پیغام_home_replace(dir_name.string);
		پیغام_puts(":\n");
	    }

	    إذا (عدد_files)
	    {
		لكل (صحيح i = 0; i < عدد_files; ++i)
		{
		    // print the swap file name
		    پیغام_outnum((اوږد)++file_شمېر);
		    پیغام_puts(".    ");
		    پیغام_puts((حرف *)gettail(files[i]));
		    پیغام_putchar('\n');
		    (فراغ)swapfile_info(files[i]);
		}
	    }
	    وإلا
		پیغام_puts(_("      -- none --\n"));
	    بهر_flush();
	}
#که_تعریف_شوی FEAT_EVAL
	وإلا إذا (ret_list != عدم)
	{
	    string_T	name;

	    لكل (صحيح i = 0; i < عدد_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.اوږدتیا,
		    files[i], STRLEN(files[i]), TRUE, &name);
		إذا (name.string != عدم)
		{
		    list_append_string(ret_list, name.string, (صحيح)name.اوږدتیا);
		    vim_free(name.string);
		}
	    }
	}
#د_که_پای
	وإلا
	    file_شمېر += عدد_files;

	لكل (صحيح i = 0; i < عدد_نومونه; ++i)
	    vim_free(نومونه[i]);
	إذا (عدد_files > 0)
	    FreeWild(عدد_files, files);
    }
    vim_free(dir_name.string);
    أرجع file_شمېر;
}

#إذا defined(UNIX) || defined(MSWIN)
/*
 * Need _very_ long file names.
 * Append the full path to name with path separators made into percent
 * signs, to "dir". An unnamed buffer is handled as "" (<currentdir>/"")
 * The last character in "dir" must be an extra slash or backslash, it is
 * removed.
 */
    char_u *
make_percent_swname(char_u *dir, char_u *dir_پای_ټکی, char_u *name)
{
    string_T	d = {عدم, 0};
    string_T	fixed_fname;
    char_u	*p;

    fixed_fname.string = fix_fname(name != عدم ? name : (char_u *)"");
    إذا (fixed_fname.string == عدم)
	أرجع عدم;

    لكل (p = fixed_fname.string; *p != NUL; MB_PTR_ADV(p))
	إذا (vim_ispathsep(*p))
	    *p = '%';
    fixed_fname.اوږدتیا = (size_t)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_پای_ټکی[-1];
    *p = NUL;
    concat_fnames(dir, (size_t)(p - dir), fixed_fname.string, fixed_fname.اوږدتیا, TRUE, &d);
    vim_free(fixed_fname.string);

    أرجع d.string;
}
#د_که_پای

#إذا (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
ساكن صحيح process_still_running;
#د_که_پای

#إذا defined(FEAT_EVAL)
/*
 * Return information found in swapfile "fname" in dictionary "d".
 * This is used by the swapinfo() function.
 */
    فراغ
get_b0_dict(char_u *fname, dict_T *d)
{
    صحيح fd;
    بنية block0 b0;

    إذا ((fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
    {
	إذا (read_eintr(fd, &b0, حجم(b0)) == حجم(b0))
	{
	    إذا (ml_check_b0_id(&b0) == FAIL)
		dict_جمع_کړه_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    وإلا إذا (b0_magic_wrong(&b0))
		dict_جمع_کړه_string_len(d, "error",
		    (char_u *)"Magic number mismatch", STRLEN_LITERAL("Magic number mismatch"));
	    وإلا
	    {
		// we have swap information
		dict_جمع_کړه_string_len(d, "version", b0.b0_نسخه, 10);
		dict_جمع_کړه_string_len(d, "user", b0.b0_uname, B0_UNAME_اندازه);
		dict_جمع_کړه_string_len(d, "host", b0.b0_hname, B0_HNAME_اندازه);
		dict_جمع_کړه_string_len(d, "fname", b0.b0_fname, B0_FNAME_اندازه_ORG);

		dict_جمع_کړه_شمېره(d, "pid", char_منزل_long(b0.b0_pid));
		dict_جمع_کړه_شمېره(d, "mtime", char_منزل_long(b0.b0_mtime));
		dict_جمع_کړه_شمېره(d, "dirty", b0.b0_dirty ? 1 : 0);
# ifdef CHECK_INODE
		dict_جمع_کړه_شمېره(d, "inode", char_منزل_long(b0.b0_ino));
# endif
	    }
	}
	وإلا
	    dict_جمع_کړه_string_len(d, "error", (char_u *)"Cannot read file", STRLEN_LITERAL("Cannot read file"));
	close(fd);
    }
    وإلا
	dict_جمع_کړه_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#د_که_پای

/*
 * Give information about an existing swap file.
 * Returns timestamp (0 when unknown).
 */
    ساكن time_t
swapfile_info(char_u *fname)
{
    stat_T	    st;
    صحيح		    fd;
    بنية block0   b0;
#که_تعریف_شوی UNIX
    char_u	    uname[B0_UNAME_اندازه];
#د_که_پای

    // print the swap file date
    إذا (mch_stat((حرف *)fname, &st) != -1)
    {
#که_تعریف_شوی UNIX
	// print name of owner of the file
	إذا (mch_get_uname(st.st_uid, uname, B0_UNAME_اندازه) == OK)
	{
	    پیغام_puts(_("          owned by: "));
	    پیغام_outtrans(uname);
	    پیغام_puts(_("   dated: "));
	}
	وإلا
#د_که_پای
	    پیغام_puts(_("             dated: "));
	پیغام_puts(get_ctime(st.st_mtime, TRUE));
    }
    وإلا
	st.st_mtime = 0;

    /*
     * print the original file name
     */
    fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
    إذا (fd >= 0)
    {
	إذا (read_eintr(fd, &b0, حجم(b0)) == حجم(b0))
	{
	    إذا (STRNCMP(b0.b0_نسخه, "VIM 3.0", 7) == 0)
	    {
		پیغام_puts(_("         [from Vim version 3.0]"));
	    }
	    وإلا إذا (ml_check_b0_id(&b0) == FAIL)
	    {
		پیغام_puts(_("         [does not look like a Vim swap file]"));
	    }
	    وإلا
	    {
		پیغام_puts(_("         file name: "));
		إذا (b0.b0_fname[0] == NUL)
		    پیغام_puts(_("[No Name]"));
		وإلا
		    پیغام_outtrans(b0.b0_fname);

		پیغام_puts(_("\n          modified: "));
		پیغام_puts(b0.b0_dirty ? _("YES") : _("no"));

		إذا (*(b0.b0_uname) != NUL)
		{
		    پیغام_puts(_("\n         user name: "));
		    پیغام_outtrans(b0.b0_uname);
		}

		إذا (*(b0.b0_hname) != NUL)
		{
		    إذا (*(b0.b0_uname) != NUL)
			پیغام_puts(_("   host name: "));
		    وإلا
			پیغام_puts(_("\n         host name: "));
		    پیغام_outtrans(b0.b0_hname);
		}

		إذا (char_منزل_long(b0.b0_pid) != 0L)
		{
		    پیغام_puts(_("\n        process ID: "));
		    پیغام_outnum(char_منزل_long(b0.b0_pid));
#إذا defined(UNIX) || defined(MSWIN)
		    إذا (swapfile_process_running(&b0, fname))
		    {
			پیغام_puts(_(" (STILL RUNNING)"));
# ifdef HAVE_PROCESS_STILL_RUNNING
			process_still_running = TRUE;
# endif
		    }
#د_که_پای
		}

		إذا (b0_magic_wrong(&b0))
		{
#إذا defined(MSWIN)
		    إذا (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			پیغام_puts(_("\n         [not usable with this version of Vim]"));
		    وإلا
#د_که_پای
			پیغام_puts(_("\n         [not usable on this computer]"));
		}
	    }
	}
	وإلا
	    پیغام_puts(_("         [cannot be read]"));
	close(fd);
    }
    وإلا
	پیغام_puts(_("         [cannot be opened]"));
    پیغام_putchar('\n');

    أرجع st.st_mtime;
}

/*
 * Return TRUE if the swap file looks OK and there are no changes, thus it can
 * be safely deleted.
 */
    ساكن صحيح
swapfile_unchanged(char_u *fname)
{
    stat_T	    st;
    صحيح		    fd;
    بنية block0   b0;
    صحيح		    ret = TRUE;

    // must be able to stat the swap file
    إذا (mch_stat((حرف *)fname, &st) == -1)
	أرجع FALSE;

    // must be able to read the first block
    fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
    إذا (fd < 0)
	أرجع FALSE;
    إذا (read_eintr(fd, &b0, حجم(b0)) != حجم(b0))
    {
	close(fd);
	أرجع FALSE;
    }

    // the ID and magic number must be correct
    إذا (ml_check_b0_id(&b0) == FAIL|| b0_magic_wrong(&b0))
	ret = FALSE;

    // must be unchanged
    إذا (b0.b0_dirty)
	ret = FALSE;

#إذا defined(UNIX) || defined(MSWIN)
    // Host name must be known and must equal the current host name, otherwise
    // comparing pid is meaningless.
    إذا (*(b0.b0_hname) == NUL)
    {
	ret = FALSE;
    }
    وإلا
    {
	char_u	    hostname[B0_HNAME_اندازه];

	mch_get_host_name(hostname, B0_HNAME_اندازه);
	hostname[B0_HNAME_اندازه - 1] = NUL;
	b0.b0_hname[B0_HNAME_اندازه - 1] = NUL; // in case of corruption
	إذا (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    إذا (char_منزل_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#د_که_پای

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    close(fd);
    أرجع ret;
}

    ساكن صحيح
recov_file_نومونه(char_u **نومونه, char_u *لار, صحيح prepend_dot)
{
    صحيح		عدد_نومونه;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    صحيح		i;
#که_نه_تعریف_شوی MSWIN
    bool    shortname = curbuf->b_shortname;

    curbuf->b_shortname = false;
#د_که_پای
    string_T	ret;

    عدد_نومونه = 0;

    /*
     * May also add the file name with a dot prepended, for swap file in same
     * dir as original file.
     */
    إذا (prepend_dot)
    {
	نومونه[عدد_نومونه] = modname(لار, (char_u *)".sw?", TRUE);
	إذا (نومونه[عدد_نومونه] == عدم)
	    اذهب پای_ټکی;
	++عدد_نومونه;
    }

    /*
     * Form the normal swap file name pattern by appending ".sw?".
     */
#که_تعریف_شوی VMS
    نومونه[عدد_نومونه] = concat_fnames(لار, STRLEN(لار),
	(char_u *)"_sw%", STRLEN_LITERAL("_sw%"), FALSE, &ret);
#وإلا
    نومونه[عدد_نومونه] = concat_fnames(لار, STRLEN(لار),
	(char_u *)".sw?", STRLEN_LITERAL(".sw?"), FALSE, &ret);
#د_که_پای
    إذا (نومونه[عدد_نومونه] == عدم)
	اذهب پای_ټکی;
    إذا (عدد_نومونه >= 1)	    // check if we have the same name twice
    {
	p = نومونه[عدد_نومونه - 1];
	i = (صحيح)STRLEN(نومونه[عدد_نومونه - 1]) - (صحيح)STRLEN(نومونه[عدد_نومونه]);
	إذا (i > 0)
	    p += i;	    // file name has been expanded to full path

	إذا (STRCMP(p, نومونه[عدد_نومونه]) != 0)
	    ++عدد_نومونه;
	وإلا
	    vim_free(نومونه[عدد_نومونه]);
    }
    وإلا
	++عدد_نومونه;

#که_نه_تعریف_شوی MSWIN
    /*
     * Also try with 'shortname' set, in case the file is on a DOS filesystem.
     */
    curbuf->b_shortname = true;
# ifdef VMS
    نومونه[عدد_نومونه] = modname(لار, (char_u *)"_sw%", FALSE);
# وإلا
    نومونه[عدد_نومونه] = modname(لار, (char_u *)".sw?", FALSE);
# endif
    إذا (نومونه[عدد_نومونه] == عدم)
	اذهب پای_ټکی;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = نومونه[عدد_نومونه];
    i = STRLEN(نومونه[عدد_نومونه]) - STRLEN(نومونه[عدد_نومونه - 1]);
    إذا (i > 0)
	p += i;		// file name has been expanded to full path
    إذا (STRCMP(نومونه[عدد_نومونه - 1], p) == 0)
	vim_free(نومونه[عدد_نومونه]);
    وإلا
	++عدد_نومونه;
#د_که_پای

پای_ټکی:
#که_نه_تعریف_شوی MSWIN
    curbuf->b_shortname = shortname;
#د_که_پای

    أرجع عدد_نومونه;
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
    فراغ
ml_sync_all(صحيح check_file, صحيح check_char)
{
    buf_T		*buf;
    stat_T		st;

    FOR_ALL_BUFFERS(buf)
    {
	إذا (buf->b_ml.ml_mfp == عدم
		|| buf->b_ml.ml_mfp->mf_fname == عدم
		|| buf->b_ml.ml_mfp->mf_fd < 0)
	    استمر;			    // no file

#که_تعریف_شوی FEAT_CRYPT
	إذا (crypt_may_close_swapfile(buf, buf->b_p_کلی,
						     crypt_get_لاره_nr(buf)))
	    استمر;
#د_که_پای

	ml_flush_کرښه(buf);		    // flush buffered line
					    // flush locked block
	(فراغ)ml_پیدا_کړه_کرښه(buf, (linenr_T)0, ML_FLUSH);
	إذا (buf_Is_بدل_شوی(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != عدم)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    إذا (mch_stat((حرف *)buf->b_ffname, &st) == -1
		    || st.st_mtime != buf->b_mtime_read
#که_تعریف_شوی ST_MTIM_NSEC
		    || st.ST_MTIM_NSEC != buf->b_mtime_read_ns
#د_که_پای
		    || st.st_size != buf->b_orig_اندازه)
	    {
		ml_preserve(buf, FALSE);
		did_check_timestamps = FALSE;
		need_check_timestamps = TRUE;	// give message later
	    }
	}
	إذا (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (فراغ)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (buf_Is_بدل_شوی(buf) ? MFS_FLUSH : 0));
	    إذا (check_char && ui_char_avail())	// character available now
		اكسر;
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
    فراغ
ml_preserve(buf_T *buf, صحيح پیغام_2)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    صحيح		حالت;
    صحيح		got_int_save = got_int;

    إذا (mfp == عدم || mfp->mf_fname == عدم)
    {
	إذا (پیغام_2)
	    emsg(_(e_cannot_preserve_there_is_no_بدلون_file));
	أرجع;
    }
#که_تعریف_شوی FEAT_CRYPT
    إذا (crypt_may_close_swapfile(buf, buf->b_p_کلی, crypt_get_لاره_nr(buf)))
	أرجع;
#د_که_پای

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_کرښه(buf);				    // flush buffered line
    (فراغ)ml_پیدا_کړه_کرښه(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    حالت = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

    // stack is invalid after mf_sync(.., MFS_ALL)
    buf->b_ml.ml_ډېری_سر = 0;

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
    إذا (mf_need_trans(mfp) && !got_int)
    {
	lnum = 1;
	طالما (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_کرښه_شمېر)
	{
	    hp = ml_پیدا_کړه_کرښه(buf, lnum, ML_پیدا_کړه);
	    إذا (hp == عدم)
	    {
		حالت = FAIL;
		اذهب theend;
	    }
	    CHECK(buf->b_ml.ml_locked_ټیټ != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_لوړ + 1;
	}
	(فراغ)ml_پیدا_کړه_کرښه(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	إذا (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    حالت = FAIL;
	buf->b_ml.ml_ډېری_سر = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    إذا (پیغام_2)
    {
	إذا (حالت == OK)
	    پیغام(_("File preserved"));
	وإلا
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
    أرجع ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
    char_u *
ml_get_pos(pos_T *pos)
{
    أرجع (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->ستون);
}

/*
 * Return pointer to cursor line.
 */
    char_u *
ml_get_curline(فراغ)
{
    أرجع ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE);
}

/*
 * Return pointer to cursor position.
 */
    char_u *
ml_get_cursor(فراغ)
{
    أرجع (ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE) +
							curwin->w_cursor.ستون);
}

// return length (excluding the NUL) of the given line
    colnr_T
ml_get_len(linenr_T lnum)
{
    أرجع ml_get_buf_len(curbuf, lnum);
}

// return length (excluding the NUL) of the text after position "pos"
    colnr_T
ml_get_pos_len(pos_T *pos)
{
    أرجع ml_get_buf_len(curbuf, pos->lnum) - pos->ستون;
}

// return length (excluding the NUL) of the cursor line
    colnr_T
ml_get_curline_len(فراغ)
{
    أرجع ml_get_buf_len(curbuf, curwin->w_cursor.lnum);
}

// return length (excluding the NUL) of the cursor position
    colnr_T
ml_get_cursor_len(فراغ)
{
    أرجع ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.ستون;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*کرښه;

    إذا (*(کرښه = ml_get_buf(buf, lnum, FALSE)) == NUL)
	أرجع 0;

    إذا (buf->b_ml.ml_کرښه_textlen <= 0)
	buf->b_ml.ml_کرښه_textlen = (صحيح)STRLEN(کرښه) + 1;
    أرجع buf->b_ml.ml_کرښه_textlen - 1;
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
    صحيح		will_change)		// line will be changed
{
    bhdr_T	*hp;
    معلومات_BL	*dp;
    ساكن صحيح	recursive = 0;
    ساكن char_u questions[4];

    إذا (lnum > buf->b_ml.ml_کرښه_شمېر)	// invalid line number
    {
	إذا (recursive == 0)
	{
	    // Avoid giving this message for a recursive call, may happen when
	    // the GUI redraws part of the text.
	    ++recursive;
	    siemsg(e_ml_get_invalid_lnum_nr, lnum);
	    --recursive;
	}
	ml_flush_کرښه(buf);
errorret:
	STRCPY(questions, "???");
	buf->b_ml.ml_کرښه_len = 4;
	buf->b_ml.ml_کرښه_textlen = buf->b_ml.ml_کرښه_len;
	buf->b_ml.ml_کرښه_lnum = lnum;
	أرجع questions;
    }
    إذا (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    إذا (buf->b_ml.ml_mfp == عدم)	// there are no lines
    {
	buf->b_ml.ml_کرښه_len = 1;
	buf->b_ml.ml_کرښه_textlen = buf->b_ml.ml_کرښه_len;
	أرجع (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    إذا (buf->b_ml.ml_کرښه_lnum != lnum || mf_dont_release)
    {
	بینښه    پیل, پای_ټکی;
	صحيح	    idx;

	ml_flush_کرښه(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	إذا ((hp = ml_پیدا_کړه_کرښه(buf, lnum, ML_پیدا_کړه)) == عدم)
	{
	    إذا (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_پیدا_کړه_کرښه_nr_دننه_بفر_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    اذهب errorret;
	}

	dp = (معلومات_BL *)(hp->bh_معلومات);

	idx = lnum - buf->b_ml.ml_locked_ټیټ;
	پیل = ((dp->db_شاخص[idx]) & DB_شاخص_ماسک);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	إذا (idx == 0)
	    پای_ټکی = dp->db_txt_پای_ټکی;
	وإلا
	    پای_ټکی = ((dp->db_شاخص[idx - 1]) & DB_شاخص_ماسک);

	buf->b_ml.ml_کرښه_ptr = (char_u *)dp + پیل;
	buf->b_ml.ml_کرښه_len = پای_ټکی - پیل;
#إذا defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	إذا (buf->b_has_textprop
			 && (size_t)buf->b_ml.ml_کرښه_len > حجم(textprop_T))
	    buf->b_ml.ml_کرښه_textlen = 0;  // call STRLEN() later when needed
	وإلا
#د_که_پای
	    buf->b_ml.ml_کرښه_textlen = buf->b_ml.ml_کرښه_len;
	buf->b_ml.ml_کرښه_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_کرښه_DIRTY | ML_ALLOCATED);
    }
    إذا (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#که_تعریف_شوی FEAT_EVAL
	إذا (ml_get_alloc_کرښې && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_کرښه_DIRTY;
#د_که_پای
    }

#که_تعریف_شوی FEAT_EVAL
    إذا (ml_get_alloc_کرښې
		 && (buf->b_ml.ml_flags & (ML_کرښه_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_کرښه_len);

	// make sure the text is in allocated memory
	إذا (p != عدم)
	{
	    memmove(p, buf->b_ml.ml_کرښه_ptr, buf->b_ml.ml_کرښه_len);
	    buf->b_ml.ml_کرښه_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    إذا (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_کرښه_DIRTY;
	}
    }
#د_که_پای
    أرجع buf->b_ml.ml_کرښه_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    صحيح
ml_کرښه_alloced(فراغ)
{
    أرجع (curbuf->b_ml.ml_flags & ML_کرښه_DIRTY);
}

#که_تعریف_شوی FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    ساكن فراغ
جمع_کړه_متن_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**کرښه,
	    صحيح		*len,
	    char_u	**tofree)
{
    صحيح		پړاو;
    صحيح		new_prop_شمېر = 0;
    صحيح		شمېر;
    صحيح		n;
    char_u	*props;
    صحيح		new_len = 0;  // init for gcc
    char_u	*new_کرښه = عدم;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    لكل (پړاو = 1; پړاو <= 2; ++پړاو)
    {
	إذا (پړاو == 2)
	{
	    uint16_t pc;

	    إذا (new_prop_شمېر == 0)
		أرجع;  // nothing to do
	    new_len = *len + (صحيح)PROP_شمېر_اندازه
			     + new_prop_شمېر * (صحيح)حجم(textprop_T);
	    new_کرښه = alloc(new_len);
	    إذا (new_کرښه == عدم)
		أرجع;
	    mch_memmove(new_کرښه, *کرښه, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_شمېر;
	    mch_memmove(new_کرښه + *len, &pc, PROP_شمېر_اندازه);
	    new_prop_شمېر = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	شمېر = get_متن_props(buf, lnum, &props, FALSE);
	لكل (n = 0; n < شمېر; ++n)
	{
	    mch_memmove(&prop, props + n * حجم(textprop_T),
							   حجم(textprop_T));
	    إذا (prop.tp_flags & TP_FLAG_CONT_بل)
	    {
		إذا (پړاو == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_PREV;
		    prop.tp_ستون = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_متن_دننه_واټن = 0;
		    mch_memmove(new_کرښه + *len + (صحيح)PROP_شمېر_اندازه
			    + new_prop_شمېر * حجم(textprop_T),
			    &prop, حجم(textprop_T));
		}
		++new_prop_شمېر;
	    }
	}
    }
    *کرښه = new_کرښه;
    *tofree = new_کرښه;
    *len = new_len;
}
#د_که_پای

    ساكن صحيح
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*کرښه_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    صحيح		flags)		// ML_APPEND_ flags
{
    char_u	*کرښه = کرښه_arg;
    colnr_T	len = len_arg;
    صحيح		i;
    صحيح		کرښه_شمېر;	// number of indexes in current block
    صحيح		دننه_واټن;
    صحيح		from, منزل;
    اوږد	space_needed;	// space needed for new line
    صحيح		پاڼه_اندازه;
    صحيح		پاڼه_شمېر;
    صحيح		db_idx;		// index for lnum in data block
    bhdr_T	*hp;
    memfile_T	*mfp;
    معلومات_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
#که_تعریف_شوی FEAT_PROP_POPUP
    char_u	*tofree = عدم;
# ifdef FEAT_BYTEOFF
    colnr_T	متن_len = 0;	// text len with NUL without text properties
# endif
#د_که_پای
    صحيح		ret = FAIL;

    إذا (lnum > buf->b_ml.ml_کرښه_شمېر || buf->b_ml.ml_mfp == عدم)
	أرجع FAIL;  // lnum out of range

    إذا (ټیټ_ترین_marked && ټیټ_ترین_marked > lnum)
	ټیټ_ترین_marked = lnum + 1;

    إذا (len == 0)
    {
	len = (colnr_T)STRLEN(کرښه) + 1;	// space needed for the text
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	متن_len = len;
#د_که_پای
    }
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    وإلا إذا (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	متن_len = (colnr_T)STRLEN(کرښه) + 1;
    وإلا
	متن_len = len;
#د_که_پای

#که_تعریف_شوی FEAT_PROP_POPUP
    إذا (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	جمع_کړه_متن_props_for_append(buf, lnum, &کرښه, &len, &tofree);
#د_که_پای

    space_needed = len + شاخص_اندازه;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    پاڼه_اندازه = mfp->mf_پاڼه_اندازه;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    إذا ((hp = ml_پیدا_کړه_کرښه(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_ورننباسه)) == عدم)
	اذهب theend;

    buf->b_ml.ml_flags &= ~ML_تشه;

    إذا (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    وإلا
	db_idx = lnum - buf->b_ml.ml_locked_ټیټ;
		// get line count before the insertion
    کرښه_شمېر = buf->b_ml.ml_locked_لوړ - buf->b_ml.ml_locked_ټیټ;

    dp = (معلومات_BL *)(hp->bh_معلومات);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    إذا ((اوږد)dp->db_free < space_needed && db_idx == کرښه_شمېر - 1
					    && lnum < buf->b_ml.ml_کرښه_شمېر)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_لوړ);
	إذا ((hp = ml_پیدا_کړه_کرښه(buf, lnum + 1, ML_ورننباسه)) == عدم)
	    اذهب theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	کرښه_شمېر = buf->b_ml.ml_locked_لوړ - buf->b_ml.ml_locked_ټیټ;
	CHECK(buf->b_ml.ml_locked_ټیټ != lnum + 1, "locked_low != lnum + 1");

	dp = (معلومات_BL *)(hp->bh_معلومات);
    }

    ++buf->b_ml.ml_کرښه_شمېر;

    إذا ((اوږد)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_پیل -= len;
	dp->db_free -= space_needed;
	++(dp->db_کرښه_شمېر);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	إذا (کرښه_شمېر > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    إذا (db_idx < 0)
		دننه_واټن = dp->db_txt_پای_ټکی;
	    وإلا
		دننه_واټن = ((dp->db_شاخص[db_idx]) & DB_شاخص_ماسک);
	    mch_memmove((حرف *)dp + dp->db_txt_پیل,
					  (حرف *)dp + dp->db_txt_پیل + len,
				 (size_t)(دننه_واټن - (dp->db_txt_پیل + len)));
	    لكل (i = کرښه_شمېر - 1; i > db_idx; --i)
		dp->db_شاخص[i + 1] = dp->db_شاخص[i] - len;
	    dp->db_شاخص[db_idx + 1] = دننه_واټن - len;
	}
	وإلا
	    // add line at the end (which is the start of the text)
	    dp->db_شاخص[db_idx + 1] = dp->db_txt_پیل;

	/*
	 * copy the text into the block
	 */
	mch_memmove((حرف *)dp + dp->db_شاخص[db_idx + 1], کرښه, (size_t)len);
	إذا (flags & ML_APPEND_MARK)
	    dp->db_شاخص[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	إذا (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    وإلا	    // not enough space in data block
    {
	اوږد	    کرښه_شمېر_کیڼ, کرښه_شمېر_ښی;
	صحيح	    پاڼه_شمېر_کیڼ, پاڼه_شمېر_ښی;
	bhdr_T	    *hp_کیڼ;
	bhdr_T	    *hp_ښی;
	bhdr_T	    *hp_new;
	صحيح	    کرښې_moved;
	صحيح	    معلومات_moved = 0;	    // init to shut up gcc
	صحيح	    ټول_moved = 0;	    // init to shut up gcc
	معلومات_BL	    *dp_ښی, *dp_کیڼ;
	صحيح	    ډېری_idx;
	صحيح	    دننه_کیڼ;
	صحيح	    lineadd;
	blocknr_T   bnum_کیڼ, bnum_ښی;
	linenr_T    lnum_کیڼ, lnum_ښی;
	صحيح	    pb_idx;
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
	إذا (db_idx < 0)		// left block is new, right block is existing
	{
	    کرښې_moved = 0;
	    دننه_کیڼ = TRUE;
	    // space_needed does not change
	}
	وإلا			// left block is existing, right block is new
	{
	    کرښې_moved = کرښه_شمېر - db_idx - 1;
	    إذا (کرښې_moved == 0)
		دننه_کیڼ = FALSE;	// put new line in right block
					// space_needed does not change
	    وإلا
	    {
		معلومات_moved = ((dp->db_شاخص[db_idx]) & DB_شاخص_ماسک) -
							    dp->db_txt_پیل;
		ټول_moved = معلومات_moved + کرښې_moved * شاخص_اندازه;
		إذا ((اوږد)dp->db_free + ټول_moved >= space_needed)
		{
		    دننه_کیڼ = TRUE;	// put new line in left block
		    space_needed = ټول_moved;
		}
		وإلا
		{
		    دننه_کیڼ = FALSE;	    // put new line in right block
		    space_needed += ټول_moved;
		}
	    }
	}

	پاڼه_شمېر = ((space_needed + سرلیک_اندازه) + پاڼه_اندازه - 1) / پاڼه_اندازه;
	إذا ((hp_new = ml_new_معلومات(mfp, flags & ML_APPEND_NEW, پاڼه_شمېر))
								       == عدم)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_لوړ);
	    اذهب theend;
	}
	إذا (db_idx < 0)		// left block is new
	{
	    hp_کیڼ = hp_new;
	    hp_ښی = hp;
	    کرښه_شمېر_کیڼ = 0;
	    کرښه_شمېر_ښی = کرښه_شمېر;
	}
	وإلا			// right block is new
	{
	    hp_کیڼ = hp;
	    hp_ښی = hp_new;
	    کرښه_شمېر_کیڼ = کرښه_شمېر;
	    کرښه_شمېر_ښی = 0;
	}
	dp_ښی = (معلومات_BL *)(hp_ښی->bh_معلومات);
	dp_کیڼ = (معلومات_BL *)(hp_کیڼ->bh_معلومات);
	bnum_کیڼ = hp_کیڼ->bh_bnum;
	bnum_ښی = hp_ښی->bh_bnum;
	پاڼه_شمېر_کیڼ = hp_کیڼ->bh_پاڼه_شمېر;
	پاڼه_شمېر_ښی = hp_ښی->bh_پاڼه_شمېر;

	/*
	 * May move the new line into the right/new block.
	 */
	إذا (!دننه_کیڼ)
	{
	    dp_ښی->db_txt_پیل -= len;
	    dp_ښی->db_free -= len + شاخص_اندازه;
	    dp_ښی->db_شاخص[0] = dp_ښی->db_txt_پیل;
	    إذا (flags & ML_APPEND_MARK)
		dp_ښی->db_شاخص[0] |= DB_MARKED;

	    mch_memmove((حرف *)dp_ښی + dp_ښی->db_txt_پیل,
							   کرښه, (size_t)len);
	    ++کرښه_شمېر_ښی;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	إذا (کرښې_moved)
	{
	    dp_ښی->db_txt_پیل -= معلومات_moved;
	    dp_ښی->db_free -= ټول_moved;
	    mch_memmove((حرف *)dp_ښی + dp_ښی->db_txt_پیل,
			(حرف *)dp_کیڼ + dp_کیڼ->db_txt_پیل,
			(size_t)معلومات_moved);
	    دننه_واټن = dp_ښی->db_txt_پیل - dp_کیڼ->db_txt_پیل;
	    dp_کیڼ->db_txt_پیل += معلومات_moved;
	    dp_کیڼ->db_free += ټول_moved;

	    /*
	     * update indexes in the new block
	     */
	    لكل (منزل = کرښه_شمېر_ښی, from = db_idx + 1;
					 from < کرښه_شمېر_کیڼ; ++from, ++منزل)
		dp_ښی->db_شاخص[منزل] = dp->db_شاخص[from] + دننه_واټن;
	    کرښه_شمېر_ښی += کرښې_moved;
	    کرښه_شمېر_کیڼ -= کرښې_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	إذا (دننه_کیڼ)
	{
	    dp_کیڼ->db_txt_پیل -= len;
	    dp_کیڼ->db_free -= len + شاخص_اندازه;
	    dp_کیڼ->db_شاخص[کرښه_شمېر_کیڼ] = dp_کیڼ->db_txt_پیل;
	    إذا (flags & ML_APPEND_MARK)
		dp_کیڼ->db_شاخص[کرښه_شمېر_کیڼ] |= DB_MARKED;
	    mch_memmove((حرف *)dp_کیڼ + dp_کیڼ->db_txt_پیل,
							   کرښه, (size_t)len);
	    ++کرښه_شمېر_کیڼ;
	}

	إذا (db_idx < 0)		// left block is new
	{
	    lnum_کیڼ = lnum + 1;
	    lnum_ښی = 0;
	}
	وإلا			// right block is new
	{
	    lnum_کیڼ = 0;
	    إذا (دننه_کیڼ)
		lnum_ښی = lnum + 2;
	    وإلا
		lnum_ښی = lnum + 1;
	}
	dp_کیڼ->db_کرښه_شمېر = کرښه_شمېر_کیڼ;
	dp_ښی->db_کرښه_شمېر = کرښه_شمېر_ښی;

	/*
	 * release the two data blocks
	 * The new one (hp_new) already has a correct blocknumber.
	 * The old one (hp, in ml_locked) gets a positive blocknumber if
	 * we changed it and we are not editing a new file.
	 */
	إذا (کرښې_moved || دننه_کیڼ)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	إذا (!(flags & ML_APPEND_NEW) && db_idx >= 0 && دننه_کیڼ)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_پیدا_کړه_کرښه(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	لكل (ډېری_idx = buf->b_ml.ml_ډېری_سر - 1; ډېری_idx >= 0;
								  --ډېری_idx)
	{
	    ip = &(buf->b_ml.ml_ډېری[ډېری_idx]);
	    pb_idx = ip->ip_شاخص;
	    إذا ((hp = mf_get(mfp, ip->ip_bnum, 1)) == عدم)
		اذهب theend;
	    pp = (PTR_BL *)(hp->bh_معلومات);   // must be pointer block
	    إذا (pp->pb_id != PTR_ID)
	    {
		iemsg(e_اشاره_ګر_بلاک_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		اذهب theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    إذا (pp->pb_شمېر < pp->pb_شمېر_max)
	    {
		إذا (pb_idx + 1 < (صحيح)pp->pb_شمېر)
		    mch_memmove(&pp->pb_اشاره_ګر[pb_idx + 2],
				&pp->pb_اشاره_ګر[pb_idx + 1],
			(size_t)(pp->pb_شمېر - pb_idx - 1) * حجم(PTR_EN));
		++pp->pb_شمېر;
		pp->pb_اشاره_ګر[pb_idx].pe_کرښه_شمېر = کرښه_شمېر_کیڼ;
		pp->pb_اشاره_ګر[pb_idx].pe_bnum = bnum_کیڼ;
		pp->pb_اشاره_ګر[pb_idx].pe_پاڼه_شمېر = پاڼه_شمېر_کیڼ;
		pp->pb_اشاره_ګر[pb_idx + 1].pe_کرښه_شمېر = کرښه_شمېر_ښی;
		pp->pb_اشاره_ګر[pb_idx + 1].pe_bnum = bnum_ښی;
		pp->pb_اشاره_ګر[pb_idx + 1].pe_پاڼه_شمېر = پاڼه_شمېر_ښی;

		إذا (lnum_کیڼ != 0)
		    pp->pb_اشاره_ګر[pb_idx].pe_old_lnum = lnum_کیڼ;
		إذا (lnum_ښی != 0)
		    pp->pb_اشاره_ګر[pb_idx + 1].pe_old_lnum = lnum_ښی;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_ډېری_سر = ډېری_idx + 1;	    // truncate stack

		إذا (lineadd)
		{
		    --(buf->b_ml.ml_ډېری_سر);
		    // fix line count for rest of blocks in the stack
		    ml_lineadd(buf, lineadd);
							// fix stack itself
		    buf->b_ml.ml_ډېری[buf->b_ml.ml_ډېری_سر].ip_لوړ +=
								      lineadd;
		    ++(buf->b_ml.ml_ډېری_سر);
		}

		/*
		 * We are finished, break the loop here.
		 */
		اكسر;
	    }
	    // pointer block full
	    /*
	     * split the pointer block
	     * allocate a new pointer block
	     * move some of the pointer into the new block
	     * prepare for updating the parent block
	     */
	    لكل (;;)	// do this twice when splitting block 1
	    {
		hp_new = ml_new_ptr(mfp);
		إذا (hp_new == عدم)	    // TODO: try to fix tree
		    اذهب theend;
		pp_new = (PTR_BL *)(hp_new->bh_معلومات);

		إذا (hp->bh_bnum != 1)
		    اكسر;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (size_t)پاڼه_اندازه);
		pp->pb_شمېر = 1;
		pp->pb_اشاره_ګر[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_اشاره_ګر[0].pe_کرښه_شمېر = buf->b_ml.ml_کرښه_شمېر;
		pp->pb_اشاره_ګر[0].pe_old_lnum = 1;
		pp->pb_اشاره_ګر[0].pe_پاڼه_شمېر = 1;
		mf_put(mfp, hp, TRUE, FALSE);   // release block 1
		hp = hp_new;		// new block is to be split
		pp = pp_new;
		CHECK(ډېری_idx != 0, _("stack_idx should be 0"));
		ip->ip_شاخص = 0;
		++ډېری_idx;	// do block 1 again later
	    }
	    /*
	     * move the pointers after the current one to the new block
	     * If there are none, the new entry will be in the new block.
	     */
	    ټول_moved = pp->pb_شمېر - pb_idx - 1;
	    إذا (ټول_moved)
	    {
		mch_memmove(&pp_new->pb_اشاره_ګر[0],
				&pp->pb_اشاره_ګر[pb_idx + 1],
				(size_t)(ټول_moved) * حجم(PTR_EN));
		pp_new->pb_شمېر = ټول_moved;
		pp->pb_شمېر -= ټول_moved - 1;
		pp->pb_اشاره_ګر[pb_idx + 1].pe_bnum = bnum_ښی;
		pp->pb_اشاره_ګر[pb_idx + 1].pe_کرښه_شمېر = کرښه_شمېر_ښی;
		pp->pb_اشاره_ګر[pb_idx + 1].pe_پاڼه_شمېر = پاڼه_شمېر_ښی;
		إذا (lnum_ښی)
		    pp->pb_اشاره_ګر[pb_idx + 1].pe_old_lnum = lnum_ښی;
	    }
	    وإلا
	    {
		pp_new->pb_شمېر = 1;
		pp_new->pb_اشاره_ګر[0].pe_bnum = bnum_ښی;
		pp_new->pb_اشاره_ګر[0].pe_کرښه_شمېر = کرښه_شمېر_ښی;
		pp_new->pb_اشاره_ګر[0].pe_پاڼه_شمېر = پاڼه_شمېر_ښی;
		pp_new->pb_اشاره_ګر[0].pe_old_lnum = lnum_ښی;
	    }
	    pp->pb_اشاره_ګر[pb_idx].pe_bnum = bnum_کیڼ;
	    pp->pb_اشاره_ګر[pb_idx].pe_کرښه_شمېر = کرښه_شمېر_کیڼ;
	    pp->pb_اشاره_ګر[pb_idx].pe_پاڼه_شمېر = پاڼه_شمېر_کیڼ;
	    إذا (lnum_کیڼ)
		pp->pb_اشاره_ګر[pb_idx].pe_old_lnum = lnum_کیڼ;
	    lnum_کیڼ = 0;
	    lnum_ښی = 0;

	    /*
	     * recompute line counts
	     */
	    کرښه_شمېر_ښی = 0;
	    لكل (i = 0; i < (صحيح)pp_new->pb_شمېر; ++i)
		کرښه_شمېر_ښی += pp_new->pb_اشاره_ګر[i].pe_کرښه_شمېر;
	    کرښه_شمېر_کیڼ = 0;
	    لكل (i = 0; i < (صحيح)pp->pb_شمېر; ++i)
		کرښه_شمېر_کیڼ += pp->pb_اشاره_ګر[i].pe_کرښه_شمېر;

	    bnum_کیڼ = hp->bh_bnum;
	    bnum_ښی = hp_new->bh_bnum;
	    پاڼه_شمېر_کیڼ = 1;
	    پاڼه_شمېر_ښی = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	إذا (ډېری_idx < 0)
	{
	    iemsg(e_updated_too_many_بلاکونه);
	    buf->b_ml.ml_ډېری_سر = 0;	// invalidate stack
	}
    }

#که_تعریف_شوی FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (اوږد)متن_len
# وإلا
	    (اوږد)len
# endif
	    , ML_CHNK_ADDLINE);
#د_که_پای

#که_تعریف_شوی FEAT_NETBEANS_INTG
    إذا (!(flags & ML_APPEND_NEW) && netbeans_فعال())
    {
	صحيح کرښه_len = (صحيح)STRLEN(کرښه);
	إذا (کرښه_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, کرښه, کرښه_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)کرښه_len, (char_u *)"\n", 1);
    }
#د_که_پای
#که_تعریف_شوی FEAT_دنده_CHANNEL
    إذا (!(flags & ML_APPEND_NEW) && buf->b_write_منزل_channel)
	channel_write_new_کرښې(buf);
#د_که_پای
    ret = OK;

theend:
#که_تعریف_شوی FEAT_PROP_POPUP
    vim_free(tofree);
#د_که_پای
    أرجع ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    ساكن صحيح
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*کرښه,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    صحيح		flags)		// ML_APPEND_ flags
{
    إذا (lnum > buf->b_ml.ml_کرښه_شمېر)
	أرجع FAIL;  // lnum out of range

    إذا (buf->b_ml.ml_کرښه_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_کرښه(buf);

#که_تعریف_شوی FEAT_EVAL
    إذا (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	إذا (buf->b_ml.ml_کرښه_lnum != 0)
	    ml_flush_کرښه(buf);
    }
#د_که_پای

    أرجع ml_append_int(buf, lnum, کرښه, len, flags);
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
    صحيح
ml_append(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*کرښه,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    صحيح		newfile)	// flag, see above
{
    أرجع ml_append_flags(lnum, کرښه, len, newfile ? ML_APPEND_NEW : 0);
}

    صحيح
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*کرښه,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    صحيح		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    إذا (curbuf->b_ml.ml_mfp == عدم && open_بفر(FALSE, عدم, 0) == FAIL)
	أرجع FAIL;
    أرجع ml_append_flush(curbuf, lnum, کرښه, len, flags);
}


#إذا defined(FEAT_SPELL) || defined(FEAT_QUICKFIX) || defined(FEAT_PROP_POPUP)

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
    صحيح
ml_append_buf(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*کرښه,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    صحيح		newfile)	// flag, see above
{
    إذا (buf->b_ml.ml_mfp == عدم)
	أرجع FAIL;
    أرجع ml_append_flush(buf, lnum, کرښه, len, newfile ? ML_APPEND_NEW : 0);
}
#د_که_پای

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
    صحيح
ml_replace(linenr_T lnum, char_u *کرښه, صحيح copy)
{
    colnr_T len = -1;

    إذا (کرښه != عدم)
	len = (colnr_T)STRLEN(کرښه);
    أرجع ml_replace_len(lnum, کرښه, len, FALSE, copy);
}

/*
 * Replace a line for the current buffer.  Like ml_replace() with:
 * "len_arg" is the length of the text, excluding NUL.
 * If "has_props" is TRUE then "line_arg" includes the text properties and
 * "len_arg" includes the NUL of the text and text properties.
 * When "copy" is TRUE copy the text into allocated memory, otherwise
 * "line_arg" must be allocated and will be consumed here.
 */
    صحيح
ml_replace_len(
	linenr_T    lnum,
	char_u	    *کرښه_arg,
	colnr_T	    len_arg,
	صحيح	    has_props,
	صحيح	    copy)
{
    char_u *کرښه = کرښه_arg;
    colnr_T len = len_arg;

    إذا (کرښه == عدم)		// just checking...
	أرجع FAIL;

    // When starting up, we might still need to create the memfile
    إذا (curbuf->b_ml.ml_mfp == عدم && open_بفر(FALSE, عدم, 0) == FAIL)
	أرجع FAIL;

    إذا (!has_props)
	++len;  // include the NUL after the text
    إذا (copy)
    {
	// copy the line to allocated memory
#که_تعریف_شوی FEAT_PROP_POPUP
	إذا (has_props)
	    کرښه = vim_memsave(کرښه, len);
	وإلا
#د_که_پای
	    کرښه = vim_strnsave(کرښه, len - 1);
	إذا (کرښه == عدم)
	    أرجع FAIL;
    }

#که_تعریف_شوی FEAT_NETBEANS_INTG
    إذا (netbeans_فعال())
    {
	netbeans_removed(curbuf, lnum, 0, (اوږد)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, کرښه, (صحيح)STRLEN(کرښه));
    }
#د_که_پای
    إذا (curbuf->b_ml.ml_کرښه_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_کرښه(curbuf);

#که_تعریف_شوی FEAT_PROP_POPUP
	إذا (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#د_که_پای
    }

#که_تعریف_شوی FEAT_PROP_POPUP
    إذا (curbuf->b_has_textprop && !has_props)
    {
	size_t	oldtextlen = STRLEN(curbuf->b_ml.ml_کرښه_ptr) + 1;

	إذا (oldtextlen < (size_t)curbuf->b_ml.ml_کرښه_len)
	{
	    char_u *newline;
	    size_t textproplen = curbuf->b_ml.ml_کرښه_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    إذا (newline != عدم)
	    {
		mch_memmove(newline, کرښه, len);
		mch_memmove(newline + len, curbuf->b_ml.ml_کرښه_ptr
						    + oldtextlen, textproplen);
		vim_free(کرښه);
		کرښه = newline;
		len += (colnr_T)textproplen;
	    }
	}
    }
#د_که_پای

    إذا (curbuf->b_ml.ml_flags & (ML_کرښه_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_کرښه_ptr);	// free allocated line

    curbuf->b_ml.ml_کرښه_ptr = کرښه;
    curbuf->b_ml.ml_کرښه_len = len;
    curbuf->b_ml.ml_کرښه_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_کرښه_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_کرښه_DIRTY) & ~ML_تشه;

    أرجع OK;
}

#که_تعریف_شوی FEAT_PROP_POPUP
/*
 * Adjust text properties in line "lnum" for a deleted line.
 * When "above" is true this is the line above the deleted line, otherwise this
 * is the line below the deleted line.
 * "del_props[del_props_len]" are the properties of the deleted line.
 */
    ساكن فراغ
adjust_متن_props_for_delete(
	buf_T	    *buf,
	linenr_T    lnum,
	char_u	    *del_props,
	صحيح	    del_props_len,
	صحيح	    above)
{
    صحيح		did_get_کرښه = FALSE;
    صحيح		بشپړ_del;
    صحيح		بشپړ_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    معلومات_BL	*dp;
    صحيح		idx;
    صحيح		کرښه_پیل;
    اوږد	کرښه_اندازه;
    صحيح		this_props_len = 0;
    char_u	*متن;
    size_t	textlen;
    صحيح		وموندل;

    لكل (بشپړ_del = 0; بشپړ_del < del_props_len; بشپړ_del += حجم(textprop_T))
    {
	mch_memmove(&prop_del, del_props + بشپړ_del, حجم(textprop_T));
	إذا ((above && (prop_del.tp_flags & TP_FLAG_CONT_PREV)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_بل))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_بل)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_PREV)))
	{
	    إذا (!did_get_کرښه)
	    {
		did_get_کرښه = TRUE;
		إذا ((hp = ml_پیدا_کړه_کرښه(buf, lnum, ML_پیدا_کړه)) == عدم)
		    أرجع;

		dp = (معلومات_BL *)(hp->bh_معلومات);
		idx = lnum - buf->b_ml.ml_locked_ټیټ;
		کرښه_پیل = ((dp->db_شاخص[idx]) & DB_شاخص_ماسک);
		إذا (idx == 0)		// first line in block, text at the end
		    کرښه_اندازه = dp->db_txt_پای_ټکی - کرښه_پیل;
		وإلا
		    کرښه_اندازه = ((dp->db_شاخص[idx - 1]) & DB_شاخص_ماسک)
								  - کرښه_پیل;
		متن = (char_u *)dp + کرښه_پیل;
		textlen = STRLEN(متن) + 1;
		إذا ((اوږد)textlen >= کرښه_اندازه)
		{
		    // No properties on this line.
		    إذا (above)
			دننی_error("no text property above deleted line");
		    وإلا
			دننی_error("no text property below deleted line");
		    أرجع;
		}
		إذا ((اوږد)textlen + (اوږد)PROP_شمېر_اندازه > کرښه_اندازه)
		{
		    دننی_error("text property data too short");
		    أرجع;
		}

		uint16_t pc;

		mch_memmove(&pc, متن + textlen, PROP_شمېر_اندازه);
		إذا (!متن_prop_شمېر_valid(pc, (size_t)(کرښه_اندازه - (اوږد)textlen)))
		{
		    دننی_error("text property count too large");
		    أرجع;
		}
		this_props_len = pc * (صحيح)حجم(textprop_T);
	    }

	    وموندل = FALSE;
	    {
		char_u *props_پیل = متن + textlen + PROP_شمېر_اندازه;

		لكل (بشپړ_this = 0; بشپړ_this < this_props_len;
					       بشپړ_this += حجم(textprop_T))
		{
		    صحيح		flag = above ? TP_FLAG_CONT_بل
							   : TP_FLAG_CONT_PREV;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_پیل + بشپړ_this,
							   حجم(textprop_T));
		    إذا ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			وموندل = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_پیل + بشپړ_this, &prop_this,
							   حجم(textprop_T));
			اكسر;
		    }
		}
	    }
	    إذا (!وموندل)
	    {
		إذا (above)
		    دننی_error("text property above deleted line not found");
		وإلا
		    دننی_error("text property below deleted line not found");
	    }

	    buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
	}
    }
}
#د_که_پای

/*
 * Delete line "lnum" in the current buffer.
 * When "flags" has ML_DEL_MESSAGE may give a "No lines in buffer" message.
 * When "flags" has ML_DEL_UNDO this is called from undo.
 *
 * return FAIL for failure, OK otherwise
 */
    ساكن صحيح
ml_delete_int(buf_T *buf, linenr_T lnum, صحيح flags)
{
    bhdr_T	*hp;
    memfile_T	*mfp;
    معلومات_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    صحيح		شمېر;	    // number of entries in block
    صحيح		idx;
    صحيح		ډېری_idx;
    صحيح		متن_پیل;
    صحيح		کرښه_پیل;
    اوږد	کرښه_اندازه;
    صحيح		i;
    صحيح		ret = FAIL;
#که_تعریف_شوی FEAT_PROP_POPUP
    char_u	*textprop_save = عدم;
    اوږد	textprop_len = 0;
#د_که_پای

    إذا (ټیټ_ترین_marked && ټیټ_ترین_marked > lnum)
	ټیټ_ترین_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    إذا (buf->b_ml.ml_کرښه_شمېر == 1)	    // file becomes empty
    {
	إذا ((flags & ML_DEL_پیغام)
#که_تعریف_شوی FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_کرښې
#د_که_پای
	   )
	    سېټ_keep_پیغام((char_u *)_(no_کرښې_پیغام), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_تشه;

	أرجع i;
    }

/*
 * Find the data block containing the line.
 * This also fills the stack with the blocks from the root to the data block.
 * This also releases any locked block..
 */
    mfp = buf->b_ml.ml_mfp;
    إذا (mfp == عدم)
	أرجع FAIL;

    إذا ((hp = ml_پیدا_کړه_کرښه(buf, lnum, ML_DELETE)) == عدم)
	أرجع FAIL;

    dp = (معلومات_BL *)(hp->bh_معلومات);
    // compute line count before the delete
    شمېر = (اوږد)(buf->b_ml.ml_locked_لوړ)
					- (اوږد)(buf->b_ml.ml_locked_ټیټ) + 2;
    idx = lnum - buf->b_ml.ml_locked_ټیټ;

    --buf->b_ml.ml_کرښه_شمېر;

    کرښه_پیل = ((dp->db_شاخص[idx]) & DB_شاخص_ماسک);
    إذا (idx == 0)		// first line in block, text at the end
	کرښه_اندازه = dp->db_txt_پای_ټکی - کرښه_پیل;
    وإلا
	کرښه_اندازه = ((dp->db_شاخص[idx - 1]) & DB_شاخص_ماسک) - کرښه_پیل;

#که_تعریف_شوی FEAT_NETBEANS_INTG
    إذا (netbeans_فعال())
	netbeans_removed(buf, lnum, 0, کرښه_اندازه);
#د_که_پای
#که_تعریف_شوی FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    إذا (buf->b_has_textprop)
    {
	size_t	textlen = STRLEN((char_u *)dp + کرښه_پیل) + 1;

	textprop_len = کرښه_اندازه - (اوږد)textlen;
	إذا (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + کرښه_پیل + textlen,
								 textprop_len);
    }
#د_که_پای

/*
 * special case: If there is only one line in the data block it becomes empty.
 * Then we have to remove the entry, pointing to this data block, from the
 * pointer block. If this pointer block also becomes empty, we go up another
 * block, and so on, up to the root if necessary.
 * The line counts in the pointer blocks have already been adjusted by
 * ml_find_line().
 */
    إذا (شمېر == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = عدم;

	لكل (ډېری_idx = buf->b_ml.ml_ډېری_سر - 1; ډېری_idx >= 0;
								  --ډېری_idx)
	{
	    buf->b_ml.ml_ډېری_سر = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_ډېری[ډېری_idx]);
	    idx = ip->ip_شاخص;
	    إذا ((hp = mf_get(mfp, ip->ip_bnum, 1)) == عدم)
		اذهب theend;
	    pp = (PTR_BL *)(hp->bh_معلومات);   // must be pointer block
	    إذا (pp->pb_id != PTR_ID)
	    {
		iemsg(e_اشاره_ګر_بلاک_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		اذهب theend;
	    }
	    شمېر = --(pp->pb_شمېر);
	    إذا (شمېر == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    وإلا
	    {
		إذا (شمېر != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_اشاره_ګر[idx], &pp->pb_اشاره_ګر[idx + 1],
				      (size_t)(شمېر - idx) * حجم(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_ډېری_سر = ډېری_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		إذا (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_ډېری[buf->b_ml.ml_ډېری_سر].ip_لوړ +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_ډېری_سر);

		اكسر;
	    }
	}
	CHECK(ډېری_idx < 0, _("deleted block 1?"));
    }
    وإلا
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	متن_پیل = dp->db_txt_پیل;
	mch_memmove((حرف *)dp + متن_پیل + کرښه_اندازه,
		  (حرف *)dp + متن_پیل, (size_t)(کرښه_پیل - متن_پیل));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	لكل (i = idx; i < شمېر - 1; ++i)
	    dp->db_شاخص[i] = dp->db_شاخص[i + 1] + کرښه_اندازه;

	dp->db_free += کرښه_اندازه + شاخص_اندازه;
	dp->db_txt_پیل += کرښه_اندازه;
	--(dp->db_کرښه_شمېر);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#که_تعریف_شوی FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, کرښه_اندازه
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#د_که_پای
    ret = OK;

theend:
#که_تعریف_شوی FEAT_PROP_POPUP
    إذا (textprop_save != عدم)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_معلومات;
	صحيح	    props_بایټونه;

	mch_memmove(&pc, textprop_save, PROP_شمېر_اندازه);
	props_معلومات = textprop_save + PROP_شمېر_اندازه;
	props_بایټونه = pc * (صحيح)حجم(textprop_T);
	إذا (!متن_prop_شمېر_valid(pc, (size_t)textprop_len))
	    props_بایټونه = 0;

	// Adjust text properties in the line above and below.
	إذا (lnum > 1)
	    adjust_متن_props_for_delete(buf, lnum - 1,
					     props_معلومات, props_بایټونه, TRUE);
	إذا (lnum <= buf->b_ml.ml_کرښه_شمېر)
	    adjust_متن_props_for_delete(buf, lnum,
					    props_معلومات, props_بایټونه, FALSE);
    }
    vim_free(textprop_save);
#د_که_پای
    أرجع ret;
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
    صحيح
ml_delete(linenr_T lnum)
{
    أرجع ml_delete_flags(lnum, 0);
}

/*
 * Like ml_delete() but using flags (see ml_delete_int()).
 */
    صحيح
ml_delete_flags(linenr_T lnum, صحيح flags)
{
    ml_flush_کرښه(curbuf);
    إذا (lnum < 1 || lnum > curbuf->b_ml.ml_کرښه_شمېر)
	أرجع FAIL;

#که_تعریف_شوی FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#د_که_پای

    أرجع ml_delete_int(curbuf, lnum, flags);
}

/*
 * set the DB_MARKED flag for line 'lnum'
 */
    فراغ
ml_setmarked(linenr_T lnum)
{
    bhdr_T    *hp;
    معلومات_BL *dp;
				    // invalid line number
    إذا (lnum < 1 || lnum > curbuf->b_ml.ml_کرښه_شمېر
					       || curbuf->b_ml.ml_mfp == عدم)
	أرجع;			    // give error message?

    إذا (ټیټ_ترین_marked == 0 || ټیټ_ترین_marked > lnum)
	ټیټ_ترین_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    إذا ((hp = ml_پیدا_کړه_کرښه(curbuf, lnum, ML_پیدا_کړه)) == عدم)
	أرجع;		    // give error message?

    dp = (معلومات_BL *)(hp->bh_معلومات);
    dp->db_شاخص[lnum - curbuf->b_ml.ml_locked_ټیټ] |= DB_MARKED;
    curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
}

/*
 * find the first line with its DB_MARKED flag set
 */
    linenr_T
ml_firstmarked(فراغ)
{
    bhdr_T	*hp;
    معلومات_BL	*dp;
    linenr_T	lnum;
    صحيح		i;

    إذا (curbuf->b_ml.ml_mfp == عدم)
	أرجع (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    لكل (lnum = ټیټ_ترین_marked; lnum <= curbuf->b_ml.ml_کرښه_شمېر; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	إذا ((hp = ml_پیدا_کړه_کرښه(curbuf, lnum, ML_پیدا_کړه)) == عدم)
	    أرجع (linenr_T)0;		    // give error message?

	dp = (معلومات_BL *)(hp->bh_معلومات);

	لكل (i = lnum - curbuf->b_ml.ml_locked_ټیټ;
			    lnum <= curbuf->b_ml.ml_locked_لوړ; ++i, ++lnum)
	    إذا ((dp->db_شاخص[i]) & DB_MARKED)
	    {
		(dp->db_شاخص[i]) &= DB_شاخص_ماسک;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		ټیټ_ترین_marked = lnum + 1;
		أرجع lnum;
	    }
    }

    أرجع (linenr_T) 0;
}

/*
 * clear all DB_MARKED flags
 */
    فراغ
ml_clearmarked(فراغ)
{
    bhdr_T	*hp;
    معلومات_BL	*dp;
    linenr_T	lnum;
    صحيح		i;

    إذا (curbuf->b_ml.ml_mfp == عدم)	    // nothing to do
	أرجع;

    /*
     * The search starts with line lowest_marked.
     */
    لكل (lnum = ټیټ_ترین_marked; lnum <= curbuf->b_ml.ml_کرښه_شمېر; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	إذا ((hp = ml_پیدا_کړه_کرښه(curbuf, lnum, ML_پیدا_کړه)) == عدم)
	    أرجع;		// give error message?

	dp = (معلومات_BL *)(hp->bh_معلومات);

	لكل (i = lnum - curbuf->b_ml.ml_locked_ټیټ;
			    lnum <= curbuf->b_ml.ml_locked_لوړ; ++i, ++lnum)
	    إذا ((dp->db_شاخص[i]) & DB_MARKED)
	    {
		(dp->db_شاخص[i]) &= DB_شاخص_ماسک;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	    }
    }

    ټیټ_ترین_marked = 0;
}

/*
 * flush ml_line if necessary
 */
    ساكن فراغ
ml_flush_کرښه(buf_T *buf)
{
    bhdr_T	*hp;
    معلومات_BL	*dp;
    linenr_T	lnum;
    char_u	*new_کرښه;
    char_u	*old_کرښه;
    colnr_T	new_len;
    صحيح		old_len;
    صحيح		extra;
    صحيح		idx;
    صحيح		پیل;
    صحيح		شمېر;
    صحيح		i;
    ساكن صحيح  entered = FALSE;

    إذا (buf->b_ml.ml_کرښه_lnum == 0 || buf->b_ml.ml_mfp == عدم)
	أرجع;		// nothing to do

    إذا (buf->b_ml.ml_flags & ML_کرښه_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	إذا (entered)
	    أرجع;
	entered = TRUE;

	lnum = buf->b_ml.ml_کرښه_lnum;
	new_کرښه = buf->b_ml.ml_کرښه_ptr;

	hp = ml_پیدا_کړه_کرښه(buf, lnum, ML_پیدا_کړه);
	إذا (hp == عدم)
	    siemsg(e_cannot_پیدا_کړه_کرښه_nr, lnum);
	وإلا
	{
	    dp = (معلومات_BL *)(hp->bh_معلومات);
	    idx = lnum - buf->b_ml.ml_locked_ټیټ;
	    پیل = ((dp->db_شاخص[idx]) & DB_شاخص_ماسک);
	    old_کرښه = (char_u *)dp + پیل;
	    إذا (idx == 0)	// line is last in block
		old_len = dp->db_txt_پای_ټکی - پیل;
	    وإلا		// text of previous line follows
		old_len = (dp->db_شاخص[idx - 1] & DB_شاخص_ماسک) - پیل;
	    new_len = buf->b_ml.ml_کرښه_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    إذا ((صحيح)dp->db_free >= extra)
	    {
#إذا defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		صحيح old_prop_len = 0;
		إذا (buf->b_has_textprop)
		    old_prop_len = old_len - (صحيح)STRLEN(old_کرښه) - 1;
#د_که_پای
		// if the length changes and there are following lines
		شمېر = buf->b_ml.ml_locked_لوړ - buf->b_ml.ml_locked_ټیټ + 1;
		إذا (extra != 0 && idx < شمېر - 1)
		{
		    // move text of following lines
		    mch_memmove((حرف *)dp + dp->db_txt_پیل - extra,
				(حرف *)dp + dp->db_txt_پیل,
				(size_t)(پیل - dp->db_txt_پیل));

		    // adjust pointers of this and following lines
		    لكل (i = idx + 1; i < شمېر; ++i)
			dp->db_شاخص[i] -= extra;
		}
		dp->db_شاخص[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_پیل -= extra;

		// copy new line into the data block
		mch_memmove(old_کرښه - extra, new_کرښه, (size_t)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#إذا defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		إذا (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (صحيح)STRLEN(new_کرښه) - 1;
		}
		إذا (extra != 0)
		    ml_updatechunk(buf, lnum, (اوږد)extra, ML_CHNK_UPDLINE);
#د_که_پای
	    }
	    وإلا
	    {
		/*
		 * Cannot do it in one data block: Delete and append.
		 * Append first, because ml_delete_int() cannot delete the
		 * last line in a buffer, which causes trouble for a buffer
		 * that has only one line.
		 * Don't forget to copy the mark!
		 */
		// How about handling errors???
		(فراغ)ml_append_int(buf, lnum, new_کرښه, new_len,
			 ((dp->db_شاخص[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#که_تعریف_شوی FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#د_که_پای
			 );
		(فراغ)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_کرښه);

	entered = FALSE;
    }
    وإلا إذا (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_کرښه_ptr);

    buf->b_ml.ml_flags &= ~(ML_کرښه_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_کرښه_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    ساكن bhdr_T *
ml_new_معلومات(memfile_T *mfp, صحيح negative, صحيح پاڼه_شمېر)
{
    bhdr_T	*hp;
    معلومات_BL	*dp;

    إذا ((hp = mf_new(mfp, negative, پاڼه_شمېر)) == عدم)
	أرجع عدم;

    dp = (معلومات_BL *)(hp->bh_معلومات);
    dp->db_id = معلومات_ID;
    dp->db_txt_پیل = dp->db_txt_پای_ټکی = پاڼه_شمېر * mfp->mf_پاڼه_اندازه;
    dp->db_free = dp->db_txt_پیل - سرلیک_اندازه;
    dp->db_کرښه_شمېر = 0;

    أرجع hp;
}

/*
 * create a new, empty, pointer block
 */
    ساكن bhdr_T *
ml_new_ptr(memfile_T *mfp)
{
    bhdr_T	*hp;
    PTR_BL	*pp;

    إذا ((hp = mf_new(mfp, FALSE, 1)) == عدم)
	أرجع عدم;

    pp = (PTR_BL *)(hp->bh_معلومات);
    pp->pb_id = PTR_ID;
    pp->pb_شمېر = 0;
    pp->pb_شمېر_max = PB_شمېر_MAX(mfp);

    أرجع hp;
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
    ساكن bhdr_T *
ml_پیدا_کړه_کرښه(buf_T *buf, linenr_T lnum, صحيح action)
{
    معلومات_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    صحيح		dirty;
    linenr_T	ټیټ, لوړ;
    صحيح		سر_2;
    صحيح		پاڼه_شمېر;
    صحيح		idx;

    mfp = buf->b_ml.ml_mfp;

    /*
     * If there is a locked block check if the wanted line is in it.
     * If not, flush and release the locked block.
     * Don't do this for ML_INSERT_SAME, because the stack need to be updated.
     * Don't do this for ML_FLUSH, because we want to flush the locked block.
     * Don't do this when 'swapfile' is reset, we want to load all the blocks.
     */
    إذا (buf->b_ml.ml_locked)
    {
	إذا (ML_SIMPLE(action)
		&& buf->b_ml.ml_locked_ټیټ <= lnum
		&& buf->b_ml.ml_locked_لوړ >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    إذا (action == ML_ورننباسه)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_لوړ);
	    }
	    وإلا إذا (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_لوړ);
	    }
	    أرجع (buf->b_ml.ml_locked);
	}

	mf_put(mfp, buf->b_ml.ml_locked, buf->b_ml.ml_flags & ML_LOCKED_DIRTY,
					    buf->b_ml.ml_flags & ML_LOCKED_POS);
	buf->b_ml.ml_locked = عدم;

	/*
	 * If lines have been added or deleted in the locked block, need to
	 * update the line count in pointer blocks.
	 */
	إذا (buf->b_ml.ml_locked_lineadd != 0)
	    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
    }

    إذا (action == ML_FLUSH)	    // nothing else to do
	أرجع عدم;

    bnum = 1;			    // start at the root of the tree
    پاڼه_شمېر = 1;
    ټیټ = 1;
    لوړ = buf->b_ml.ml_کرښه_شمېر;

    إذا (action == ML_پیدا_کړه)	// first try stack entries
    {
	لكل (سر_2 = buf->b_ml.ml_ډېری_سر - 1; سر_2 >= 0; --سر_2)
	{
	    ip = &(buf->b_ml.ml_ډېری[سر_2]);
	    إذا (ip->ip_ټیټ <= lnum && ip->ip_لوړ >= lnum)
	    {
		bnum = ip->ip_bnum;
		ټیټ = ip->ip_ټیټ;
		لوړ = ip->ip_لوړ;
		buf->b_ml.ml_ډېری_سر = سر_2;	// truncate stack at prev entry
		اكسر;
	    }
	}
	إذا (سر_2 < 0)
	    buf->b_ml.ml_ډېری_سر = 0;		// not found, start at the root
    }
    وإلا	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_ډېری_سر = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    لكل (;;)
    {
	إذا ((hp = mf_get(mfp, bnum, پاڼه_شمېر)) == عدم)
	    اذهب error_noblock;

	/*
	 * update high for insert/delete
	 */
	إذا (action == ML_ورننباسه)
	    ++لوړ;
	وإلا إذا (action == ML_DELETE)
	    --لوړ;

	dp = (معلومات_BL *)(hp->bh_معلومات);
	إذا (dp->db_id == معلومات_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_ټیټ = ټیټ;
	    buf->b_ml.ml_locked_لوړ = لوړ;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    أرجع hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	إذا (pp->pb_id != PTR_ID)
	{
	    iemsg(e_اشاره_ګر_بلاک_id_wrong);
	    اذهب error_بلاک;
	}

	إذا ((سر_2 = ml_جمع_کړه_ډېری(buf)) < 0)	// add new entry to stack
	    اذهب error_بلاک;
	ip = &(buf->b_ml.ml_ډېری[سر_2]);
	ip->ip_bnum = bnum;
	ip->ip_ټیټ = ټیټ;
	ip->ip_لوړ = لوړ;
	ip->ip_شاخص = -1;		// index not known yet

	dirty = FALSE;
	لكل (idx = 0; idx < (صحيح)pp->pb_شمېر; ++idx)
	{
	    t = pp->pb_اشاره_ګر[idx].pe_کرښه_شمېر;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    إذا ((ټیټ += t) > lnum)
	    {
		ip->ip_شاخص = idx;
		bnum = pp->pb_اشاره_ګر[idx].pe_bnum;
		پاڼه_شمېر = pp->pb_اشاره_ګر[idx].pe_پاڼه_شمېر;
		لوړ = ټیټ - 1;
		ټیټ -= t;

		/*
		 * a negative block number may have been changed
		 */
		إذا (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    إذا (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_اشاره_ګر[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		اكسر;
	    }
	}
	إذا (idx >= (صحيح)pp->pb_شمېر)	    // past the end: something wrong!
	{
	    إذا (lnum > buf->b_ml.ml_کرښه_شمېر)
		siemsg(e_کرښه_شمېره_بهر_د_range_nr_past_the_پای_ټکی,
					      lnum - buf->b_ml.ml_کرښه_شمېر);

	    وإلا
		siemsg(e_کرښه_شمېر_wrong_دننه_بلاک_nr, bnum);
	    اذهب error_بلاک;
	}
	إذا (action == ML_DELETE)
	{
	    pp->pb_اشاره_ګر[idx].pe_کرښه_شمېر--;
	    dirty = TRUE;
	}
	وإلا إذا (action == ML_ورننباسه)
	{
	    pp->pb_اشاره_ګر[idx].pe_کرښه_شمېر++;
	    dirty = TRUE;
	}
	mf_put(mfp, hp, dirty, FALSE);
    }

error_بلاک:
    mf_put(mfp, hp, FALSE, FALSE);
error_noblock:
    /*
     * If action is ML_DELETE or ML_INSERT we have to correct the tree for
     * the incremented/decremented line counts, because there won't be a line
     * inserted/deleted after all.
     */
    إذا (action == ML_DELETE)
	ml_lineadd(buf, 1);
    وإلا إذا (action == ML_ورننباسه)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_ډېری_سر = 0;
    أرجع عدم;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    ساكن صحيح
ml_جمع_کړه_ډېری(buf_T *buf)
{
    صحيح		سر_2;
    infoptr_T	*newstack;

    سر_2 = buf->b_ml.ml_ډېری_سر;

    // may have to increase the stack size
    إذا (سر_2 == buf->b_ml.ml_ډېری_اندازه)
    {
	CHECK(سر_2 > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_ډېری_اندازه + ډېری_INCR);
	إذا (newstack == عدم)
	    أرجع -1;
	إذا (سر_2 > 0)
	    mch_memmove(newstack, buf->b_ml.ml_ډېری,
					     (size_t)سر_2 * حجم(infoptr_T));
	vim_free(buf->b_ml.ml_ډېری);
	buf->b_ml.ml_ډېری = newstack;
	buf->b_ml.ml_ډېری_اندازه += ډېری_INCR;
    }

    buf->b_ml.ml_ډېری_سر++;
    أرجع سر_2;
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
    ساكن فراغ
ml_lineadd(buf_T *buf, صحيح شمېر)
{
    صحيح		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    لكل (idx = buf->b_ml.ml_ډېری_سر - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_ډېری[idx]);
	إذا ((hp = mf_get(mfp, ip->ip_bnum, 1)) == عدم)
	    اكسر;
	pp = (PTR_BL *)(hp->bh_معلومات);	// must be pointer block
	إذا (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_اشاره_ګر_بلاک_id_wrong_two);
	    اكسر;
	}
	pp->pb_اشاره_ګر[ip->ip_شاخص].pe_کرښه_شمېر += شمېر;
	ip->ip_لوړ += شمېر;
	mf_put(mfp, hp, TRUE, FALSE);
    }
}

#إذا defined(HAVE_READLINK)
/*
 * Resolve a symlink in the last component of a file name.
 * Note that f_resolve() does it for every part of the path, we don't do that
 * here.
 * If it worked returns OK and the resolved link in "buf[MAXPATHL]".
 * Otherwise returns FAIL.
 */
    صحيح
resolve_symlink(char_u *fname, char_u *buf)
{
    char_u	tmp[MAXPATHL];
    صحيح		ret;
    صحيح		ژوروالی = 0;

    إذا (fname == عدم)
	أرجع FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    لكل (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	إذا (++ژوروالی == 100)
	{
	    semsg(_(e_symlink_loop_for_str), fname);
	    أرجع FAIL;
	}

	ret = readlink((حرف *)tmp, (حرف *)buf, MAXPATHL - 1);
	إذا (ret <= 0)
	{
	    إذا (errno == EINVAL || errno == ENOENT)
	    {
		// Found non-symlink or not existing file, stop here.
		// When at the first level use the unmodified name, skip the
		// call to vim_FullName().
		إذا (ژوروالی == 1)
		    أرجع FAIL;

		// Use the resolved name in tmp[].
		اكسر;
	    }

	    // There must be some error reading links, use original name.
	    أرجع FAIL;
	}
	buf[ret] = NUL;

	/*
	 * Check whether the symlink is relative or absolute.
	 * If it's relative, build a new path based on the directory
	 * portion of the filename (if any) and the path the symlink
	 * points to.
	 */
	إذا (mch_is_ډک_Name(buf))
	    STRCPY(tmp, buf);
	وإلا
	{
	    char_u *لکۍ;

	    لکۍ = gettail(tmp);
	    إذا (STRLEN(لکۍ) + STRLEN(buf) >= MAXPATHL)
		أرجع FAIL;
	    STRCPY(لکۍ, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    أرجع vim_ډک_Name(tmp, buf, MAXPATHL, TRUE);
}
#د_که_پای

/*
 * Make swap file name out of the file name and a directory name.
 * Returns pointer to allocated memory or NULL.
 */
    char_u *
makeswapname(
    char_u	*fname,
    char_u	*ffname ناکارول_شوی,
    buf_T	*buf,
    char_u	*dir_name)
{
    char_u	*r, *s;
    char_u	*fname_res = fname;
#که_تعریف_شوی HAVE_READLINK
    char_u	fname_buf[MAXPATHL];

    // Expand symlink in the file name, so that we put the swap file with the
    // actual file instead of with the symlink.
    إذا (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#د_که_پای

#إذا defined(UNIX) || defined(MSWIN)  // Need _very_ long file names
    صحيح		len = (صحيح)STRLEN(dir_name);

    s = dir_name + len;
    إذا (after_pathsep(dir_name, s) && len > 1 && s[-1] == s[-2])
    {			       // Ends with '//', Use Full path
	r = عدم;
	إذا ((s = make_percent_swname(dir_name, s, fname_res)) != عدم)
	{
	    r = modname(s, (char_u *)".swp", FALSE);
	    vim_free(s);
	}
	أرجع r;
    }
#د_که_پای

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#إذا defined(VMS)
	    "_swp",
#وإلا
	    ".swp",
#د_که_پای
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    إذا (r == عدم)	    // out of memory
	أرجع عدم;

    s = get_file_دننه_dir(r, dir_name);
    vim_free(r);
    أرجع s;
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
get_file_دننه_dir(
    char_u  *fname,
    char_u  *dname)	// don't use "dirname", it is a global for Alpha
{
    string_T	لکۍ;
    string_T	retval;

    لکۍ.string = gettail(fname);
    لکۍ.اوږدتیا = STRLEN(لکۍ.string);

    إذا (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (size_t)(لکۍ.string - fname) + لکۍ.اوږدتیا);
    وإلا
    {
	size_t	dname_len = STRLEN(dname);

	إذا (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    إذا (لکۍ.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    لکۍ.string, لکۍ.اوږدتیا, TRUE, &retval);
	    وإلا
	    {
		صحيح	    save_char;
		string_T    tmp;

		save_char = *لکۍ.string;
		*لکۍ.string = NUL;
		concat_fnames(fname, (size_t)(لکۍ.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*لکۍ.string = save_char;
		إذا (tmp.string == عدم)	    // out of memory
		    retval.string = عدم;
		وإلا
		{
		    concat_fnames(tmp.string, tmp.اوږدتیا,
			لکۍ.string, لکۍ.اوږدتیا, TRUE, &retval);
		    vim_free(tmp.string);
		}
	    }
	}
	وإلا
	    concat_fnames(dname, dname_len, لکۍ.string, لکۍ.اوږدتیا,
		TRUE, &retval);
    }

#که_تعریف_شوی MSWIN
    إذا (retval.string != عدم)
    {
	char_u	*t;

	لكل (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    إذا (*t == ':')
		*t = '%';
    }
#د_که_پای

    أرجع retval.string;
}

/*
 * Print the ATTENTION message: info about an existing swap file.
 */
    ساكن فراغ
attention_پیغام(
    buf_T   *buf,	// buffer being edited
    char_u  *fname)	// swap file name
{
    stat_T	st;
    time_t	بدلون_mtime;

    ++no_wait_return;
    (فراغ)emsg(_(e_attention));
    پیغام_puts(_("\nFound a swap file by the name \""));
    پیغام_home_replace(fname);
    پیغام_puts("\"\n");
    بدلون_mtime = swapfile_info(fname);
    پیغام_puts(_("While opening file \""));
    پیغام_outtrans(buf->b_fname);
    پیغام_puts("\"\n");
    إذا (mch_stat((حرف *)buf->b_fname, &st) == -1)
    {
	پیغام_puts(_("      CANNOT BE FOUND"));
    }
    وإلا
    {
	پیغام_puts(_("             dated: "));
	پیغام_puts(get_ctime(st.st_mtime, TRUE));
	إذا (بدلون_mtime != 0 && st.st_mtime > بدلون_mtime)
	    پیغام_puts(_("      NEWER than swap file!\n"));
    }
    // Some of these messages are long to allow translation to
    // other languages.
    پیغام_puts(_("\n(1) Another program may be editing the same file.  If this is the case,\n    be careful not to end up with two different instances of the same\n    file when making changes.  Quit, or continue with caution.\n"));
    پیغام_puts(_("(2) An edit session for this file crashed.\n"));
    پیغام_puts(_("    If this is the case, use \":recover\" or \"vim -r "));
    پیغام_outtrans(buf->b_fname);
    پیغام_puts(_("\"\n    to recover the changes (see \":help recovery\").\n"));
    پیغام_puts(_("    If you did this already, delete the swap file \""));
    پیغام_outtrans(fname);
    پیغام_puts(_("\"\n    to avoid this message.\n"));
    cmdline_لیکه = پیغام_لیکه;
    --no_wait_return;
}

تعريفنوع تعداد {
    SEA_CHOICE_NONE = 0,
    SEA_CHOICE_READONLY = 1,
    SEA_CHOICE_EDIT = 2,
    SEA_CHOICE_RECOVER = 3,
    SEA_CHOICE_DELETE = 4,
    SEA_CHOICE_QUIT = 5,
    SEA_CHOICE_ABORT = 6
} sea_choice_T;

#إذا defined(FEAT_EVAL)
/*
 * Trigger the SwapExists autocommands.
 * Returns a value for equivalent to do_dialog().
 */
    ساكن sea_choice_T
do_swapexists(buf_T *buf, char_u *fname)
{
    سېټ_vim_var_string(VV_SWAPNAME, fname, -1);
    سېټ_vim_var_string(VV_SWAPCHOICE, عدم, -1);

    // Trigger SwapExists autocommands with <afile> set to the file being
    // edited.  Disallow changing directory here.
    ++allbuf_قلف;
    apply_autocmds(EVENT_SWAPEXISTS, buf->b_fname, عدم, FALSE, عدم);
    --allbuf_قلف;

    سېټ_vim_var_string(VV_SWAPNAME, عدم, -1);

    بدل (*get_vim_var_str(VV_SWAPCHOICE))
    {
	حالة 'o': أرجع SEA_CHOICE_READONLY;
	حالة 'e': أرجع SEA_CHOICE_EDIT;
	حالة 'r': أرجع SEA_CHOICE_RECOVER;
	حالة 'd': أرجع SEA_CHOICE_DELETE;
	حالة 'q': أرجع SEA_CHOICE_QUIT;
	حالة 'a': أرجع SEA_CHOICE_ABORT;
    }

    أرجع SEA_CHOICE_NONE;
}
#د_که_پای

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
    ساكن char_u *
findswapname(
    buf_T	*buf,
    char_u	**dirp,		// pointer to list of directories
    char_u	*old_fname)	// don't give warning for this file name
{
    char_u	*fname;
    صحيح		n;
    char_u	*dir_name;
#که_تعریف_شوی AMIGA
    BPTR	fh;
#د_که_پای
    صحيح		r;
    char_u	*buf_fname = buf->b_fname;

#إذا !defined(UNIX)
# define CREATE_DUMMY_FILE
    FILE	*dummyfd = عدم;

# ifdef MSWIN
    إذا (buf_fname != عدم && !mch_is_ډک_Name(buf_fname)
				       && vim_strchr(gettail(buf_fname), ':'))
    {
	char_u *t;

	buf_fname = vim_strsave(buf_fname);
	إذا (buf_fname == عدم)
	    buf_fname = buf->b_fname;
	وإلا
	    لكل (t = gettail(buf_fname); *t != NUL; MB_PTR_ADV(t))
		إذا (*t == ':')
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
    إذا (!(buf->b_p_sn || buf->b_shortname) && buf_fname != عدم
					     && mch_getperm(buf_fname) < 0)
	dummyfd = mch_fopen((حرف *)buf_fname, "w");
#د_که_پای

    /*
     * Isolate a directory name from *dirp and put it in dir_name.
     * First allocate some memory to put the directory name in.
     */
    dir_name = alloc(STRLEN(*dirp) + 1);
    إذا (dir_name == عدم)
	*dirp = عدم;
    وإلا
	(فراغ)copy_option_part(dirp, dir_name, 31000, ",");

    /*
     * we try different names until we find one that does not exist yet
     */
    إذا (dir_name == عدم)	    // out of memory
	fname = عدم;
    وإلا
	fname = makeswapname(buf_fname, buf->b_ffname, buf, dir_name);

    لكل (;;)
    {
	إذا (fname == عدم)	// must be out of memory
	    اكسر;
	إذا ((n = (صحيح)STRLEN(fname)) == 0)	// safety check
	{
	    VIM_CLEAR(fname);
	    اكسر;
	}
#إذا defined(UNIX)
/*
 * Some systems have a MS-DOS compatible filesystem that use 8.3 character
 * file names. If this is the first try and the swap file name does not fit in
 * 8.3, detect if this is the case, set shortname and try again.
 */
	إذا (fname[n - 2] == 'w' && fname[n - 1] == 'p'
					&& !(buf->b_p_sn || buf->b_shortname))
	{
	    char_u	    *لکۍ;
	    char_u	    *fname2;
	    stat_T	    s1, s2;
	    صحيح		    f1, f2;
	    صحيح		    created1 = FALSE, created2 = FALSE;
	    صحيح		    same = FALSE;

	    /*
	     * Check if swapfile name does not fit in 8.3:
	     * It either contains two dots, is longer than 8 chars, or starts
	     * with a dot.
	     */
	    لکۍ = gettail(buf_fname);
	    إذا (       vim_strchr(لکۍ, '.') != عدم
		    || STRLEN(لکۍ) > (size_t)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		إذا (fname2 != عدم)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    إذا (vim_strchr(لکۍ, '.') != عدم)
			fname2[n - 1] = 'x';
		    وإلا إذا (*gettail(fname) == '.')
		    {
			fname2[n] = 'x';
			fname2[n + 1] = NUL;
		    }
		    وإلا
			fname2[n - 5] += 1;
		    /*
		     * may need to create the files to be able to use mch_stat()
		     */
		    f1 = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
		    إذا (f1 < 0)
		    {
			f1 = mch_open_rw((حرف *)fname,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			created1 = TRUE;
		    }
		    إذا (f1 >= 0)
		    {
			f2 = mch_open((حرف *)fname2, O_RDONLY | O_EXTRA, 0);
			إذا (f2 < 0)
			{
			    f2 = mch_open_rw((حرف *)fname2,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			    created2 = TRUE;
			}
			إذا (f2 >= 0)
			{
			    /*
			     * Both files exist now. If mch_stat() returns the
			     * same device and inode they are the same file.
			     */
			    إذا (mch_fstat(f1, &s1) != -1
				    && mch_fstat(f2, &s2) != -1
				    && s1.st_dev == s2.st_dev
				    && s1.st_ino == s2.st_ino)
				same = TRUE;
			    close(f2);
			    إذا (created2)
				mch_remove(fname2);
			}
			close(f1);
			إذا (created1)
			    mch_remove(fname);
		    }
		    vim_free(fname2);
		    إذا (same)
		    {
			buf->b_shortname = true;
			vim_free(fname);
			fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
			استمر;	// try again with b_shortname set
		    }
		}
	    }
	}
#د_که_پای
	/*
	 * check if the swapfile already exists
	 */
	إذا (mch_getperm(fname) < 0)	// it does not exist
	{
#که_تعریف_شوی HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    إذا (mch_lstat((حرف *)fname, &sb) < 0)
#وإلا
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (اوږد)ډېرپېښ_NEWFILE);
	    /*
	     * on the Amiga mch_getperm() will return -1 when the file exists
	     * but is being used by another program. This happens if you edit
	     * a file twice.
	     */
	    إذا (fh != (BPTR)عدم)	// can open file, OK
	    {
		Close(fh);
		mch_remove(fname);
		اكسر;
	    }
	    إذا (IoErr() != ERROR_OBJECT_دننه_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#د_که_پای
		اكسر;
	}

	/*
	 * A file name equal to old_fname is OK to use.
	 */
	إذا (old_fname != عدم && fnamecmp(fname, old_fname) == 0)
	    اكسر;

	/*
	 * get here when file already exists
	 */
	إذا (fname[n - 2] == 'w' && fname[n - 1] == 'p')	// first try
	{
	    /*
	     * on MS-DOS compatible filesystems (e.g. messydos) file.doc.swp
	     * and file.doc are the same file. To guess if this problem is
	     * present try if file.doc.swx exists. If it does, we set
	     * buf->b_shortname and try file_doc.swp (dots replaced by
	     * underscores for this file), and try again. If it doesn't we
	     * assume that "file.doc.swp" already exists.
	     */
	    إذا (!(buf->b_p_sn || buf->b_shortname))	// not tried yet
	    {
		fname[n - 1] = 'x';
		r = mch_getperm(fname);		// try "file.swx"
		fname[n - 1] = 'p';
		إذا (r >= 0)		    // "file.swx" seems to exist
		{
		    buf->b_shortname = true;
		    vim_free(fname);
		    fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
		    استمر;	    // try again with '.' replaced with '_'
		}
	    }
	    /*
	     * If we get here the ".swp" file really exists.
	     * Give an error message, unless recovering, no file name, we are
	     * viewing a help file or when the path of the file is different
	     * (happens when all .swp files are in one directory).
	     */
	    إذا (!recoverymode && buf_fname != عدم
				&& !buf->b_help
				&& !(buf->b_flags & (BF_DUMMY | BF_NO_SEA)))
	    {
		صحيح		fd;
		بنية block0	b0;
		صحيح		توپیر = FALSE;

		/*
		 * Try to read block 0 from the swap file to get the original
		 * file name (and inode number).
		 */
		fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
		إذا (fd >= 0)
		{
		    إذا (read_eintr(fd, &b0, حجم(b0)) == حجم(b0))
		    {
			/*
			 * If the swapfile has the same directory as the
			 * buffer don't compare the directory names, they can
			 * have a different mountpoint.
			 */
			إذا (b0.b0_flags & B0_SAME_DIR)
			{
			    إذا (fnamecmp(gettail(buf->b_ffname),
						   gettail(b0.b0_fname)) != 0
				    || !same_directory(fname, buf->b_ffname))
			    {
#که_تعریف_شوی CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				إذا (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_منزل_long(b0.b0_ino)))
#د_که_پای
				    توپیر = TRUE;
			    }
			}
			وإلا
			{
			    /*
			     * The name in the swap file may be
			     * "~user/path/file".  Expand it first.
			     */
			    expand_env(b0.b0_fname, NameBuff, MAXPATHL);
#که_تعریف_شوی CHECK_INODE
			    إذا (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_منزل_long(b0.b0_ino)))
				توپیر = TRUE;
#وإلا
			    إذا (fnamecmp(NameBuff, buf->b_ffname) != 0)
				توپیر = TRUE;
#د_که_پای
			}
		    }
		    close(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		إذا (توپیر == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == عدم)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#که_تعریف_شوی CREATE_DUMMY_FILE
		    صحيح		 did_use_dummy = FALSE;

		    // Avoid getting a warning for the file being created
		    // outside of Vim, it was created at the start of this
		    // function.  Delete the file now, because Vim might exit
		    // here if the window is closed.
		    إذا (dummyfd != عدم)
		    {
			fclose(dummyfd);
			dummyfd = عدم;
			mch_remove(buf_fname);
			did_use_dummy = TRUE;
		    }
#د_که_پای

#که_تعریف_شوی HAVE_PROCESS_STILL_RUNNING
		    process_still_running = FALSE;
#د_که_پای
		    // It's safe to delete the swap file if all these are true:
		    // - the edited file exists
		    // - the swap file has no changes and looks OK
		    إذا (mch_stat((حرف *)buf->b_fname, &st) == 0
						  && swapfile_unchanged(fname))
		    {
			choice = SEA_CHOICE_DELETE;
			إذا (p_verbose > 0)
			    verb_پیغام(_("Found a swap file that is not useful, deleting it"));
		    }

#إذا defined(FEAT_EVAL)
		    /*
		     * If there is an SwapExists autocommand and we can handle
		     * the response, trigger it.  It may return 0 to ask the
		     * user anyway.
		     */
		    إذا (choice == SEA_CHOICE_NONE
			    && بدلون_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#د_که_پای

		    إذا (choice == SEA_CHOICE_NONE
					 && بدلون_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    إذا (choice == SEA_CHOICE_NONE)
		    {
#که_تعریف_شوی FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			إذا (gui.starting && !gui.دننه_use)
			    gui_پیل(عدم);
#د_که_پای
			// Show info about the existing swap file.
			attention_پیغام(buf, fname);

			// We don't want a 'q' typed at the more-prompt
			// interrupt loading a file.
			got_int = FALSE;

			// If vimrc has "simalt ~x" we don't want it to
			// interfere with the prompt here.
			flush_buffers(FLUSH_TYPEAHEAD);
		    }

#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    إذا (بدلون_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    مختاړی = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			size_t	    پیغام_اندازه;
			string_T    پیغام_2;
			char_u	    *tofree;
			صحيح	    dialog_پایله;

			مختاړی.اوږدتیا = STRLEN(مختاړی.string);
			suffix.اوږدتیا = STRLEN(suffix.string);
			پیغام_اندازه = مختاړی.اوږدتیا
				+ STRLEN(fname)
				+ suffix.اوږدتیا + 5;
			پیغام_2.string = tofree = alloc(پیغام_اندازه);
			إذا (پیغام_2.string != عدم)
			{
			    STRCPY(پیغام_2.string, مختاړی.string);
			    پیغام_2.اوږدتیا = مختاړی.اوږدتیا;
			    پیغام_2.اوږدتیا += home_replace(عدم, fname,
				پیغام_2.string + پیغام_2.اوږدتیا, (صحيح)(پیغام_اندازه - پیغام_2.اوږدتیا), TRUE);
			    STRCPY(پیغام_2.string + پیغام_2.اوږدتیا, suffix.string);
			}
			وإلا
			    پیغام_2.string = (char_u *)_("Swap file already exists!");
			dialog_پایله = do_dialog(VIM_WARNING,
				    (char_u *)_("VIM - ATTENTION"),
				    پیغام_2.string,
# ifdef HAVE_PROCESS_STILL_RUNNING
				    process_still_running
					? (char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Quit\n&Abort") :
# endif
					(char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Delete it\n&Quit\n&Abort"), 1, عدم, FALSE);

# ifdef HAVE_PROCESS_STILL_RUNNING
			إذا (process_still_running && dialog_پایله >= 4)
			    // compensate for missing "Delete it" button
			    dialog_پایله++;
# endif
			choice = dialog_پایله;
			vim_free(tofree);

			// pretend screen didn't scroll, need redraw anyway
			پیغام_scrolled = 0;
			redraw_all_later(UPD_NOT_VALID);
		    }
#د_که_پای

		    بدل (choice)
		    {
			حالة SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    اكسر;
			حالة SEA_CHOICE_EDIT:
			    اكسر;
			حالة SEA_CHOICE_RECOVER:
			    بدلون_exists_action = SEA_RECOVER;
			    اكسر;
			حالة SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    اكسر;
			حالة SEA_CHOICE_QUIT:
			    بدلون_exists_action = SEA_QUIT;
			    اكسر;
			حالة SEA_CHOICE_ABORT:
			    بدلون_exists_action = SEA_QUIT;
			    got_int = TRUE;
			    اكسر;
			حالة SEA_CHOICE_NONE:
			    پیغام_puts("\n");
			    إذا (پیغام_silent == 0)
				// call wait_return() later
				need_wait_return = TRUE;
			    اكسر;
		    }

		    // If the file was deleted this fname can be used.
		    إذا (choice != SEA_CHOICE_NONE && mch_getperm(fname) < 0)
			اكسر;

#که_تعریف_شوی CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    إذا (did_use_dummy)
			dummyfd = mch_fopen((حرف *)buf_fname, "w");
#د_که_پای
		}
	    }
	}

	/*
	 * Change the ".swp" extension to find another file that can be used.
	 * First decrement the last char: ".swo", ".swn", etc.
	 * If that still isn't enough decrement the last but one char: ".svz"
	 * Can happen when editing many "No Name" buffers.
	 */
	إذا (fname[n - 1] == 'a')	// ".s?a"
	{
	    إذا (fname[n - 2] == 'a')    // ".saa": tried enough, give up
	    {
		emsg(_(e_too_many_بدلون_files_وموندل));
		VIM_CLEAR(fname);
		اكسر;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#که_تعریف_شوی CREATE_DUMMY_FILE
    إذا (dummyfd != عدم)	// file has been created temporarily
    {
	fclose(dummyfd);
	mch_remove(buf_fname);
    }
#د_که_پای
#که_تعریف_شوی MSWIN
    إذا (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#د_که_پای
    أرجع fname;
}

    ساكن صحيح
b0_magic_wrong(ZERO_BL *b0p)
{
    أرجع (b0p->b0_magic_long != (اوږد)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (صحيح)B0_MAGIC_INT
	    || b0p->b0_magic_short != (لنډ)B0_MAGIC_SHORT
	    || b0p->b0_magic_char != B0_MAGIC_CHAR);
}

#که_تعریف_شوی CHECK_INODE
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

    ساكن صحيح
fnamecmp_ino(
    char_u	*fname_c,	    // current file name
    char_u	*fname_s,	    // file name from swap file
    اوږد	ino_block0)
{
    stat_T	st;
    ino_t	ino_c = 0;	    // ino of current file
    ino_t	ino_s;		    // ino of file from swap file
    char_u	buf_c[MAXPATHL];    // full path of fname_c
    char_u	buf_s[MAXPATHL];    // full path of fname_s
    صحيح		retval_c;	    // flag: buf_c valid
    صحيح		retval_s;	    // flag: buf_s valid

    إذا (mch_stat((حرف *)fname_c, &st) == 0)
	ino_c = (ino_t)st.st_ino;

    /*
     * First we try to get the inode from the file name, because the inode in
     * the swap file may be outdated.  If that fails (e.g. this path is not
     * valid on this machine), use the inode from block 0.
     */
    إذا (mch_stat((حرف *)fname_s, &st) == 0)
	ino_s = (ino_t)st.st_ino;
    وإلا
	ino_s = (ino_t)ino_block0;

    إذا (ino_c && ino_s)
	أرجع (ino_c != ino_s);

    /*
     * One of the inode numbers is unknown, try a forced vim_FullName() and
     * compare the file names.
     */
    retval_c = vim_ډک_Name(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_ډک_Name(fname_s, buf_s, MAXPATHL, TRUE);
    إذا (retval_c == OK && retval_s == OK)
	أرجع STRCMP(buf_c, buf_s) != 0;

    /*
     * Can't compare inodes or file names, guess that the files are different,
     * unless both appear not to exist at all, then compare with the file name
     * in the swap file.
     */
    إذا (ino_s == 0 && ino_c == 0 && retval_c == FAIL && retval_s == FAIL)
	أرجع STRCMP(fname_c, fname_s) != 0;
    أرجع TRUE;
}
#د_که_پای // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    ساكن فراغ
long_منزل_char(اوږد n, char_u *s)
{
    s[0] = (char_u)(n & 0xff);
    n = (بینښه)n >> 8;
    s[1] = (char_u)(n & 0xff);
    n = (بینښه)n >> 8;
    s[2] = (char_u)(n & 0xff);
    n = (بینښه)n >> 8;
    s[3] = (char_u)(n & 0xff);
}

    ساكن اوږد
char_منزل_long(char_u *s)
{
    اوږد    retval;

    retval = s[3];
    retval <<= 8;
    retval |= s[2];
    retval <<= 8;
    retval |= s[1];
    retval <<= 8;
    retval |= s[0];

    أرجع retval;
}

/*
 * Set the flags in the first block of the swap file:
 * - file is modified or not: buf->b_changed
 * - 'fileformat'
 * - 'fileencoding'
 */
    فراغ
ml_setflags(buf_T *buf)
{
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    إذا (!buf->b_ml.ml_mfp)
	أرجع;
    لكل (hp = buf->b_ml.ml_mfp->mf_کارول_شوی_وروستی; hp != عدم; hp = hp->bh_prev)
    {
	إذا (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_معلومات);
	    b0p->b0_dirty = buf->b_بدل_شوی ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_ماسک)
						  | (get_fileformat(buf) + 1);
	    جمع_کړه_b0_fenc(b0p, buf);
	    hp->bh_flags |= BH_DIRTY;
	    mf_sync(buf->b_ml.ml_mfp, MFS_ZERO);
	    اكسر;
	}
    }
}

#إذا defined(FEAT_CRYPT)
/*
 * If "data" points to a data block encrypt the text in it and return a copy
 * in allocated memory.  Return NULL when out of memory.
 * Otherwise return "data".
 */
    char_u *
ml_encrypt_معلومات(
    memfile_T	*mfp,
    char_u	*معلومات,
    off_T	دننه_واټن,
    بینښه	اندازه)
{
    معلومات_BL	*dp = (معلومات_BL *)معلومات;
    char_u	*سر_پای_ټکی_2;
    char_u	*متن_پیل;
    char_u	*new_معلومات;
    صحيح		متن_len;
    cryptstate_T *state;

    إذا (dp->db_id != معلومات_ID)
	أرجع معلومات;

    state = ml_crypt_prepare(mfp, دننه_واټن, FALSE);
    إذا (state == عدم)
	أرجع معلومات;

    new_معلومات = alloc(اندازه);
    إذا (new_معلومات == عدم)
    {
	crypt_free_state(state);
	أرجع عدم;
    }
    سر_پای_ټکی_2 = (char_u *)(&dp->db_شاخص[dp->db_کرښه_شمېر]);
    متن_پیل = (char_u *)dp + dp->db_txt_پیل;
    متن_len = اندازه - dp->db_txt_پیل;

    // Copy the header and the text.
    mch_memmove(new_معلومات, dp, سر_پای_ټکی_2 - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, متن_پیل, متن_len, new_معلومات + dp->db_txt_پیل,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    إذا (سر_پای_ټکی_2 < متن_پیل)
	vim_memset(new_معلومات + (سر_پای_ټکی_2 - معلومات), 0, متن_پیل - سر_پای_ټکی_2);

    أرجع new_معلومات;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    فراغ
ml_decrypt_معلومات(
    memfile_T	*mfp,
    char_u	*معلومات,
    off_T	دننه_واټن,
    بینښه	اندازه)
{
    معلومات_BL	*dp = (معلومات_BL *)معلومات;
    char_u	*سر_پای_ټکی_2;
    char_u	*متن_پیل;
    صحيح		متن_len;
    cryptstate_T *state;

    إذا (dp->db_id != معلومات_ID)
	أرجع;

    سر_پای_ټکی_2 = (char_u *)(&dp->db_شاخص[dp->db_کرښه_شمېر]);
    متن_پیل = (char_u *)dp + dp->db_txt_پیل;
    متن_len = dp->db_txt_پای_ټکی - dp->db_txt_پیل;

    إذا (سر_پای_ټکی_2 > متن_پیل || dp->db_txt_پیل > اندازه
	    || dp->db_txt_پای_ټکی > اندازه)
	أرجع;  // data was messed up

    state = ml_crypt_prepare(mfp, دننه_واټن, TRUE);
    إذا (state == عدم)
	أرجع;

    // Decrypt the text in place.
    crypt_decode_inplace(state, متن_پیل, متن_len, FALSE);
    crypt_free_state(state);
}

/*
 * Prepare for encryption/decryption, using the key, seed and offset.
 * Return an allocated cryptstate_T *.
 * Note: Encryption not supported for SODIUM
 */
    ساكن cryptstate_T *
ml_crypt_prepare(memfile_T *mfp, off_T دننه_واټن, صحيح reading)
{
    buf_T	*buf = mfp->mf_بفر;
    char_u	salt[50];
    صحيح		لاره_nr;
    char_u	*کلی;
    crypt_arg_T arg;

    CLEAR_ډګر(arg);
    إذا (reading && mfp->mf_old_کلی != عدم)
    {
	// Reading back blocks with the previous key/method/seed.
	لاره_nr = mfp->mf_old_cm;
	کلی = mfp->mf_old_کلی;
	arg.cat_تخم = mfp->mf_old_تخم;
    }
    وإلا
    {
	لاره_nr = crypt_get_لاره_nr(buf);
	کلی = buf->b_p_کلی;
	arg.cat_تخم = mfp->mf_تخم;
    }

    إذا (*کلی == NUL)
	أرجع عدم;

    إذا (crypt_may_close_swapfile(buf, کلی, لاره_nr))
	أرجع عدم;

    إذا (لاره_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((حرف *)salt, حجم(salt), "%s%ld", کلی, (اوږد)دننه_واټن);
	arg.cat_تخم = عدم;
	arg.cat_init_from_file = FALSE;

	أرجع crypt_create(لاره_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((حرف *)salt, حجم(salt), "%ld", (اوږد)دننه_واټن);

    arg.cat_salt = salt;
    arg.cat_salt_len = (صحيح)STRLEN(salt);
    arg.cat_تخم_len = MF_تخم_LEN;
    arg.cat_جمع_کړه_len = 0;
    arg.cat_جمع_کړه = عدم;
    arg.cat_init_from_file = FALSE;

    أرجع crypt_create(لاره_nr, کلی, &arg);
}

#د_که_پای


#إذا defined(FEAT_BYTEOFF)

# define MLCS_MAXL 800	// max no of lines in chunk
# define MLCS_MINL 400   // should be half of MLCS_MAXL

/*
 * Keep information for finding byte offset of a line, updtype may be one of:
 * ML_CHNK_ADDLINE: Add len to parent chunk, possibly splitting it
 *	   Careful: ML_CHNK_ADDLINE may cause ml_find_line() to be called.
 * ML_CHNK_DELLINE: Subtract len from parent chunk, possibly deleting it
 * ML_CHNK_UPDLINE: Add len to parent chunk, as a signed entity.
 */
    ساكن فراغ
ml_updatechunk(
    buf_T	*buf,
    linenr_T	کرښه,
    اوږد	len,
    صحيح		updtype)
{
    ساكن buf_T	*ml_upd_lastbuf = عدم;
    ساكن linenr_T	ml_upd_lastcurline;
    ساكن صحيح		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    صحيح			curix = ml_upd_lastcurix;
    اوږد		اندازه;
    chunksize_T		*curchnk;
    صحيح			rest;
    bhdr_T		*hp;
    معلومات_BL		*dp;

    إذا (buf->b_ml.ml_usedchunks == -1 || len == 0)
	أرجع;
    إذا (buf->b_ml.ml_chunksize == عدم)
    {
	buf->b_ml.ml_chunksize = ALLOC_MULT(chunksize_T, 100);
	إذا (buf->b_ml.ml_chunksize == عدم)
	{
	    buf->b_ml.ml_usedchunks = -1;
	    أرجع;
	}
	buf->b_ml.ml_numchunks = 100;
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = 1;
	ml_upd_lastbuf = عدم;		// invalidate resume cache
    }

    إذا (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_کرښه_شمېر == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (اوږد)buf->b_ml.ml_کرښه_len;
	ml_upd_lastbuf = عدم;		// invalidate resume cache
	أرجع;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    إذا (buf != ml_upd_lastbuf || کرښه < curline)
    {
	curline = 1;
	curix = 0;
    }
    لكل (; curix < buf->b_ml.ml_usedchunks - 1
	     && کرښه >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	     curix++)
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
    curchnk = buf->b_ml.ml_chunksize + curix;

    إذا (updtype == ML_CHNK_DELLINE)
	len = -len;
    curchnk->mlcs_totalsize += len;
    إذا (updtype == ML_CHNK_ADDLINE)
    {
	curchnk->mlcs_numlines++;

	// May resize here so we don't have to do it in both cases below
	إذا (buf->b_ml.ml_usedchunks + 1 >= buf->b_ml.ml_numchunks)
	{
	    chunksize_T *t_chunksize = buf->b_ml.ml_chunksize;

	    buf->b_ml.ml_numchunks = buf->b_ml.ml_numchunks * 3 / 2;
	    buf->b_ml.ml_chunksize = vim_realloc(buf->b_ml.ml_chunksize,
			    حجم(chunksize_T) * buf->b_ml.ml_numchunks);
	    إذا (buf->b_ml.ml_chunksize == عدم)
	    {
		// Hmmmm, Give up on offset for this buffer
		vim_free(t_chunksize);
		buf->b_ml.ml_usedchunks = -1;
		أرجع;
	    }
	}

	إذا (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MAXL)
	{
	    صحيح	    شمېر;	    // number of entries in block
	    صحيح	    idx;
	    صحيح	    پای_ټکی_idx;
	    صحيح	    متن_پای_ټکی;
	    صحيح	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			حجم(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    اندازه = 0;
	    linecnt = 0;
	    طالما (curline < buf->b_ml.ml_کرښه_شمېر
			&& linecnt < MLCS_MINL)
	    {
		إذا ((hp = ml_پیدا_کړه_کرښه(buf, curline, ML_پیدا_کړه)) == عدم)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    أرجع;
		}
		dp = (معلومات_BL *)(hp->bh_معلومات);
		شمېر = (اوږد)(buf->b_ml.ml_locked_لوړ) -
			(اوږد)(buf->b_ml.ml_locked_ټیټ) + 1;
		idx = curline - buf->b_ml.ml_locked_ټیټ;
		curline = buf->b_ml.ml_locked_لوړ + 1;

		// compute index of last line to use in this MEMLINE
		rest = شمېر - idx;
		إذا (linecnt + rest > MLCS_MINL)
		{
		    پای_ټکی_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		وإلا
		{
		    پای_ټکی_idx = شمېر - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		إذا (buf->b_has_textprop)
		{
		    صحيح i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    لكل (i = پای_ټکی_idx; i < idx; ++i)
			اندازه += (صحيح)STRLEN((char_u *)dp
				      + (dp->db_شاخص[i] & DB_شاخص_ماسک)) + 1;
		}
		وإلا
# endif
		{
		    إذا (idx == 0) // first line in block, text at the end
			متن_پای_ټکی = dp->db_txt_پای_ټکی;
		    وإلا
			متن_پای_ټکی = ((dp->db_شاخص[idx - 1]) & DB_شاخص_ماسک);
		    اندازه += متن_پای_ټکی
				   - ((dp->db_شاخص[پای_ټکی_idx]) & DB_شاخص_ماسک);
		}
	    }
	    buf->b_ml.ml_chunksize[curix].mlcs_numlines = linecnt;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_numlines -= linecnt;
	    buf->b_ml.ml_chunksize[curix].mlcs_totalsize = اندازه;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_totalsize -= اندازه;
	    buf->b_ml.ml_usedchunks++;
	    ml_upd_lastbuf = عدم;   // Force recalc of curix & curline
	    أرجع;
	}
	وإلا إذا (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_کرښه_شمېر - کرښه <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    إذا (کرښه == buf->b_ml.ml_کرښه_شمېر)
	    {
		curchnk->mlcs_numlines = 0;
		curchnk->mlcs_totalsize = 0;
	    }
	    وإلا
	    {
		/*
		 * Line is just prior to last, move count for last
		 * This is the common case  when loading a new file
		 */
		hp = ml_پیدا_کړه_کرښه(buf, buf->b_ml.ml_کرښه_شمېر, ML_پیدا_کړه);
		إذا (hp == عدم)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    أرجع;
		}
		dp = (معلومات_BL *)(hp->bh_معلومات);
		إذا (dp->db_کرښه_شمېر == 1)
		    rest = dp->db_txt_پای_ټکی - dp->db_txt_پیل;
		وإلا
		    rest =
			((dp->db_شاخص[dp->db_کرښه_شمېر - 2]) & DB_شاخص_ماسک)
			- dp->db_txt_پیل;
		curchnk->mlcs_totalsize = rest;
		curchnk->mlcs_numlines = 1;
		curchnk[-1].mlcs_totalsize -= rest;
		curchnk[-1].mlcs_numlines -= 1;
	    }
	}
    }
    وإلا إذا (updtype == ML_CHNK_DELLINE)
    {
	curchnk->mlcs_numlines--;
	إذا (curix < buf->b_ml.ml_usedchunks - 1
		&& curchnk->mlcs_numlines + curchnk[1].mlcs_numlines
								  <= MLCS_MINL)
	{
	    curix++;
	    curchnk = buf->b_ml.ml_chunksize + curix;
	}
	وإلا إذا (curix == 0 && curchnk->mlcs_numlines <= 0)
	{
	    ml_upd_lastbuf = عدم;   // Force recalc of curix & curline
	    buf->b_ml.ml_usedchunks--;
	    mch_memmove(buf->b_ml.ml_chunksize, buf->b_ml.ml_chunksize + 1,
			buf->b_ml.ml_usedchunks * حجم(chunksize_T));
	    أرجع;
	}
	وإلا إذا (curix == 0 || (curchnk->mlcs_numlines > 10
		    && curchnk->mlcs_numlines + curchnk[-1].mlcs_numlines
								  > MLCS_MINL))
	{
	    // The chunks are left as they are, the cached position stays
	    // valid.
	    ml_upd_lastbuf = buf;
	    ml_upd_lastcurline = curline;
	    ml_upd_lastcurix = curix;
	    أرجع;
	}

	// Collapse chunks
	ml_upd_lastbuf = عدم;   // Force recalc of curix & curline
	curchnk[-1].mlcs_numlines += curchnk->mlcs_numlines;
	curchnk[-1].mlcs_totalsize += curchnk->mlcs_totalsize;
	buf->b_ml.ml_usedchunks--;
	إذا (curix < buf->b_ml.ml_usedchunks)
	    mch_memmove(buf->b_ml.ml_chunksize + curix,
			buf->b_ml.ml_chunksize + curix + 1,
			(buf->b_ml.ml_usedchunks - curix) *
			حجم(chunksize_T));
	أرجع;
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
    اوږد
ml_پیدا_کړه_کرښه_or_دننه_واټن(buf_T *buf, linenr_T lnum, اوږد *offp)
{
    linenr_T	curline;
    صحيح		curix;
    اوږد	اندازه;
    bhdr_T	*hp;
    معلومات_BL	*dp;
    صحيح		شمېر;		// number of entries in block
    صحيح		idx;
    صحيح		پیل_idx;
    صحيح		متن_پای_ټکی;
    اوږد	دننه_واټن;
    صحيح		len;
    صحيح		ffdos = (get_fileformat(buf) == EOL_DOS);
    صحيح		extra = 0;

    // take care of cached line first
    ml_flush_کرښه(curbuf);

    إذا (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == عدم
	    || lnum < 0)
	أرجع -1;

    إذا (offp == عدم)
	دننه_واټن = 0;
    وإلا
	دننه_واټن = *offp;
    إذا (lnum == 0 && دننه_واټن <= 0)
	أرجع 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = اندازه = 0;
    طالما (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (دننه_واټن != 0
	       && دننه_واټن > اندازه + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (اوږد)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	اندازه += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	إذا (دننه_واټن && ffdos)
	    اندازه += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    طالما ((lnum != 0 && curline < lnum) || (دننه_واټن != 0 && اندازه < دننه_واټن))
    {
# ifdef FEAT_PROP_POPUP
	size_t textprop_ټول = 0;
# endif

	إذا (curline > buf->b_ml.ml_کرښه_شمېر
		|| (hp = ml_پیدا_کړه_کرښه(buf, curline, ML_پیدا_کړه)) == عدم)
	    أرجع -1;
	dp = (معلومات_BL *)(hp->bh_معلومات);
	شمېر = (اوږد)(buf->b_ml.ml_locked_لوړ) -
		(اوږد)(buf->b_ml.ml_locked_ټیټ) + 1;
	پیل_idx = idx = curline - buf->b_ml.ml_locked_ټیټ;
	إذا (idx == 0)  // first line in block, text at the end
	    متن_پای_ټکی = dp->db_txt_پای_ټکی;
	وإلا
	    متن_پای_ټکی = ((dp->db_شاخص[idx - 1]) & DB_شاخص_ماسک);
	// Compute index of last line to use in this MEMLINE
	إذا (lnum != 0)
	{
	    إذا (curline + (شمېر - idx) >= lnum)
		idx += lnum - curline - 1;
	    وإلا
		idx = شمېر - 1;
	}
	وإلا
	{
	    extra = 0;
	    لكل (;;)
	    {
# ifdef FEAT_PROP_POPUP
		size_t textprop_اندازه = 0;

		إذا (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_شاخص[idx]) & DB_شاخص_ماسک);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_پای_ټکی
				  : ((dp->db_شاخص[idx - 1]) & DB_شاخص_ماسک));
		    textprop_اندازه = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		إذا (!(دننه_واټن >= اندازه
			+ متن_پای_ټکی - (صحيح)((dp->db_شاخص[idx]) & DB_شاخص_ماسک)
# ifdef FEAT_PROP_POPUP
			- (اوږد)(textprop_ټول + textprop_اندازه)
# endif
			+ ffdos))
		    اكسر;

		إذا (ffdos)
		    اندازه++;
# ifdef FEAT_PROP_POPUP
		textprop_ټول += textprop_اندازه;
# endif
		إذا (idx == شمېر - 1)
		{
		    extra = 1;
		    اكسر;
		}
		idx++;
	    }
	}
# ifdef FEAT_PROP_POPUP
	إذا (buf->b_has_textprop && lnum != 0)
	{
	    صحيح i;

	    // cannot use the db_index pointer, need to get the actual text
	    // lengths.
	    len = 0;
	    لكل (i = پیل_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_شاخص[i]) & DB_شاخص_ماسک);
		len += (صحيح)STRLEN(p) + 1;
	    }
	}
	وإلا
# endif
	    len = متن_پای_ټکی - ((dp->db_شاخص[idx]) & DB_شاخص_ماسک)
# ifdef FEAT_PROP_POPUP
				- (اوږد)textprop_ټول
# endif
				;
	اندازه += len;
	إذا (دننه_واټن != 0 && اندازه >= دننه_واټن)
	{
	    إذا (اندازه + ffdos == دننه_واټن)
		*offp = 0;
	    وإلا إذا (idx == پیل_idx)
		*offp = دننه_واټن - اندازه + len;
	    وإلا
		*offp = دننه_واټن - اندازه + len
		     - (متن_پای_ټکی - ((dp->db_شاخص[idx - 1]) & DB_شاخص_ماسک))
# ifdef FEAT_PROP_POPUP
		     + (اوږد)textprop_ټول
# endif
		     ;
	    curline += idx - پیل_idx + extra;
	    إذا (curline > buf->b_ml.ml_کرښه_شمېر)
		أرجع -1;	// exactly one byte beyond the end
	    أرجع curline;
	}
	curline = buf->b_ml.ml_locked_لوړ + 1;
    }

    إذا (lnum != 0)
    {
	// Count extra CR characters.
	إذا (ffdos)
	    اندازه += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	إذا ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_کرښه_شمېر)
	    اندازه -= ffdos + 1;
    }

    أرجع اندازه;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    فراغ
goto_byte(اوږد cnt)
{
    اوږد	boff = cnt;
    linenr_T	lnum;

    ml_flush_کرښه(curbuf);	// cached line may be dirty
    setpcmark();
    إذا (boff)
	--boff;
    lnum = ml_پیدا_کړه_کرښه_or_دننه_واټن(curbuf, (linenr_T)0, &boff);
    إذا (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_کرښه_شمېر;
	curwin->w_curswant = MAXCOL;
	coladvance((colnr_T)MAXCOL);
    }
    وإلا
    {
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.ستون = (colnr_T)boff;
	curwin->w_cursor.coladd = 0;
	curwin->w_سېټ_curswant = true;
    }
    check_cursor();

    // Make sure the cursor is on the first byte of a multi-byte char.
    إذا (has_mbyte)
	mb_adjust_cursor();
}
#د_که_پای
