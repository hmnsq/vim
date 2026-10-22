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
#تعریف_کرو CHECK(c, s)	افعل { /**/ } طالما (0)

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
#جے_انت

تعريفنوع بنية block0		ZERO_BL;    // contents of the first block
تعريفنوع بنية اشارہ_گر_بلاک	PTR_BL;	    // contents of a pointer block
تعريفنوع بنية انکڑے_بلاک	انکڑے_BL;    // contents of a data block
تعريفنوع بنية اشارہ_گر_اندراج	PTR_EN;	    // block/line-count pair

#تعریف_کرو انکڑے_ID	       (('d' << 8) + 'a')   // data block id
#تعریف_کرو PTR_ID	       (('p' << 8) + 't')   // pointer block id
#تعریف_کرو BLOCK0_ID0     'b'		    // block 0 id 0
#تعریف_کرو BLOCK0_ID1     '0'		    // block 0 id 1
#تعریف_کرو BLOCK0_ID1_C0  'c'		    // block 0 id 1 'cm' 0
#تعریف_کرو BLOCK0_ID1_C1  'C'		    // block 0 id 1 'cm' 1
#تعریف_کرو BLOCK0_ID1_C2  'd'		    // block 0 id 1 'cm' 2
// BLOCK0_ID1_C3 and BLOCK0_ID1_C4 are for libsodium encryption.  However, for
// these the swapfile is disabled, thus they will not be used.  Added for
// consistency anyway.
#تعریف_کرو BLOCK0_ID1_C3  'S'		    // block 0 id 1 'cm' 3
#تعریف_کرو BLOCK0_ID1_C4  's'		    // block 0 id 1 'cm' 4

#إذا defined(FEAT_CRYPT)
ساكن ثابت صحيح id1_codes[] = {
    BLOCK0_ID1_C0,  // CRYPT_M_ZIP
    BLOCK0_ID1_C1,  // CRYPT_M_BF
    BLOCK0_ID1_C2,  // CRYPT_M_BF2
    BLOCK0_ID1_C3,  // CRYPT_M_SOD  - Unused!
    BLOCK0_ID1_C4,  // CRYPT_M_SOD2  - Unused!
};
#جے_انت

/*
 * pointer to a block, used in a pointer block
 */
بنية اشارہ_گر_اندراج
{
    blocknr_T	pe_bnum;	// block number
    linenr_T	pe_سطر_گنتی;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    صحيح		pe_ورقہ_گنتی;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
بنية اشارہ_گر_بلاک
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_گنتی;	// number of pointers in this block
    short_u	pb_گنتی_max;	// maximum value for pb_count
    PTR_EN	pb_اشارہ_گر[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#تعریف_کرو PB_گنتی_MAX(mfp) (short_u)(((mfp)->mf_ورقہ_ناپ - offsetof(PTR_BL, pb_اشارہ_گر)) / حجم(PTR_EN))

/*
 * A data block is a leaf in the tree.
 *
 * The text of the lines is at the end of the block. The text of the first line
 * in the block is put at the end, the text of the second line in front of it,
 * etc. Thus the order of the lines is the opposite of the line number.
 */
بنية انکڑے_بلاک
{
    short_u	db_id;		// ID for data block: DATA_ID
    بنانشانی	db_free;	// free space available
    بنانشانی	db_txt_شروع;	// byte where text starts
    بنانشانی	db_txt_سرا;	// byte just after data block
    linenr_T	db_سطر_گنتی;	// number of lines in this block
    بنانشانی	db_اشاریہ[1];	// index for start of line (actually bigger)
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
#تعریف_کرو DB_MARKED	((بنانشانی)1 << ((حجم(بنانشانی) * 8) - 1))
#تعریف_کرو DB_اشاریہ_نقاب	(~DB_MARKED)

#تعریف_کرو اشاریہ_ناپ  (حجم(بنانشانی))	    // size of one db_index entry
#تعریف_کرو سرخی_ناپ (offsetof(انکڑے_BL, db_اشاریہ))  // size of data block header

#تعریف_کرو B0_FNAME_ناپ_ORG	900	// what it was in older versions
#تعریف_کرو B0_FNAME_ناپ_NOCRYPT	898	// 2 bytes used for other things
#تعریف_کرو B0_FNAME_ناپ_CRYPT	890	// 10 bytes used for other things
#تعریف_کرو B0_UNAME_ناپ		40
#تعریف_کرو B0_HNAME_ناپ		40
/*
 * Restrict the numbers to 32 bits, otherwise most compilers will complain.
 * This won't detect a 64 bit machine that only swaps a byte in the top 32
 * bits, but that is crazy anyway.
 */
#تعریف_کرو B0_MAGIC_LONG	0x30313233L
#تعریف_کرو B0_MAGIC_INT	0x20212223L
#تعریف_کرو B0_MAGIC_SHORT	0x10111213L
#تعریف_کرو B0_MAGIC_CHAR	0x55

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
    char_u	b0_نسخہ[10];	// Vim version string
    char_u	b0_ورقہ_ناپ[4];// number of bytes per page
    char_u	b0_mtime[4];	// last modification time of file
    char_u	b0_ino[4];	// inode of b0_fname
    char_u	b0_pid[4];	// process id of creator (or 0)
    char_u	b0_uname[B0_UNAME_ناپ]; // name of user (uid if no name)
    char_u	b0_hname[B0_HNAME_ناپ]; // host name (if it has a name)
    char_u	b0_fname[B0_FNAME_ناپ_ORG]; // name of file being edited
    لما	b0_magic_long;	// check for byte order of long
    صحيح		b0_magic_int;	// check for byte order of int
    چھوٹا	b0_magic_short;	// check for byte order of short
    char_u	b0_magic_char;	// check for last char
};

/*
 * Note: b0_dirty and b0_flags are put at the end of the file name.  For very
 * long file names in older versions of Vim they are invalid.
 * The 'fileencoding' comes before b0_flags, with a NUL in front.  But only
 * when there is room, for very long file names it's omitted.
 */
#تعریف_کرو B0_DIRTY	0x55
#تعریف_کرو b0_dirty	b0_fname[B0_FNAME_ناپ_ORG - 1]

/*
 * The b0_flags field is new in Vim 7.0.
 */
#تعریف_کرو b0_flags	b0_fname[B0_FNAME_ناپ_ORG - 2]

/*
 * Crypt seed goes here, 8 bytes.  New in Vim 7.3.
 * Without encryption these bytes may be used for 'fenc'.
 */
#تعریف_کرو b0_بیج		b0_fname[B0_FNAME_ناپ_ORG - 2 - MF_بیج_LEN]

// The lowest two bits contain the fileformat.  Zero means it's not set
// (compatible with Vim 6.x), otherwise it's EOL_UNIX + 1, EOL_DOS + 1 or
// EOL_MAC + 1.
#تعریف_کرو B0_FF_نقاب	3

// Swap file is in directory of edited file.  Used to find the file from
// different mount points.
#تعریف_کرو B0_SAME_DIR	4

// The 'fileencoding' is at the end of b0_fname[], with a NUL in front of it.
// When empty there is only the NUL.
#تعریف_کرو B0_HAS_FENC	8

#تعریف_کرو ڈھیر_INCR	5	// nr of entries added to ml_stack at a time

/*
 * The line number where the first mark may be is remembered.
 * If it is 0 there are no marks at all.
 * (always used for the current buffer only, no buffer change possible while
 * executing a global command).
 */
ساكن linenr_T	سبھ_توں_نیواں_marked = 0;

/*
 * arguments for ml_find_line()
 */
#تعریف_کرو ML_DELETE	0x11	    // delete line
#تعریف_کرو ML_داخل_کرو	0x12	    // insert line
#تعریف_کرو ML_لبھو		0x13	    // just find the line
#تعریف_کرو ML_FLUSH	0x02	    // flush locked block
#تعریف_کرو ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
تعريفنوع تعداد {
      UB_FNAME = 0	// update timestamp and filename
    , UB_SAME_DIR       // update the B0_SAME_DIR flag
    , UB_CRYPT		// update crypt key
} upd_block0_T;

#جے_تعریف_شدہ FEAT_CRYPT
ساكن فراغ ml_سیٹ_b0_crypt(buf_T *buf, ZERO_BL *b0p);
#جے_انت
ساكن فراغ ml_upd_block0(buf_T *buf, upd_block0_T what);
ساكن فراغ سیٹ_b0_fname(ZERO_BL *, buf_T *buf);
ساكن فراغ سیٹ_b0_dir_flag(ZERO_BL *b0p, buf_T *buf);
ساكن فراغ جوڑو_b0_fenc(ZERO_BL *b0p, buf_T *buf);
ساكن time_t swapfile_info(char_u *);
ساكن صحيح recov_file_ناں(char_u **, char_u *, صحيح prepend_dot);
ساكن char_u *findswapname(buf_T *, char_u **, char_u *);
ساكن فراغ ml_flush_سطر(buf_T *);
ساكن bhdr_T *ml_new_انکڑے(memfile_T *, صحيح, صحيح);
ساكن bhdr_T *ml_new_ptr(memfile_T *);
ساكن bhdr_T *ml_لبھو_سطر(buf_T *, linenr_T, صحيح);
ساكن صحيح ml_جوڑو_ڈھیر(buf_T *);
ساكن فراغ ml_lineadd(buf_T *, صحيح);
ساكن صحيح b0_magic_wrong(ZERO_BL *);
#جے_تعریف_شدہ CHECK_INODE
ساكن صحيح fnamecmp_ino(char_u *, char_u *, لما);
#جے_انت
ساكن فراغ long_منزل_char(لما, char_u *);
ساكن لما char_منزل_long(char_u *);
#جے_تعریف_شدہ FEAT_CRYPT
ساكن cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T وتھ, صحيح reading);
#جے_انت
#جے_تعریف_شدہ FEAT_BYTEOFF
ساكن فراغ ml_updatechunk(buf_T *buf, لما سطر, لما len, صحيح updtype);
#جے_انت

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
    انکڑے_BL	*dp;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ڈھیر_ناپ = 0; // no stack yet
    buf->b_ml.ml_ڈھیر = عدم;	// no stack yet
    buf->b_ml.ml_ڈھیر_چوٹی = 0;	// nothing in the stack
    buf->b_ml.ml_locked = عدم;	// no cached block
    buf->b_ml.ml_سطر_lnum = 0;	// no cached line
#جے_تعریف_شدہ FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = عدم;
    buf->b_ml.ml_usedchunks = 0;
#جے_انت

    إذا (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    إذا (p_uc && buf->b_p_swf)
	buf->b_may_وٹاندرا = true;
    وإلا
	buf->b_may_وٹاندرا = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(عدم, 0);
    إذا (mfp == عدم)
	اذهب error;

    buf->b_ml.ml_mfp = mfp;
#جے_تعریف_شدہ FEAT_CRYPT
    mfp->mf_بفر = buf;
#جے_انت
    buf->b_ml.ml_flags = ML_خالی_تھاواں;
    buf->b_ml.ml_سطر_گنتی = 1;

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
    b0p = (ZERO_BL *)(hp->bh_انکڑے);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (لما)B0_MAGIC_LONG;
    b0p->b0_magic_int = (صحيح)B0_MAGIC_INT;
    b0p->b0_magic_short = (چھوٹا)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_نسخہ, "VIM ", 4);
    STRNCPY(b0p->b0_نسخہ + 4, نسخہ, 6);
    long_منزل_char((لما)mfp->mf_ورقہ_ناپ, b0p->b0_ورقہ_ناپ);

#جے_تعریف_شدہ FEAT_SPELL
    إذا (!buf->b_spell)
#جے_انت
    {
	b0p->b0_dirty = buf->b_بدلیا ? B0_DIRTY : 0;
	b0p->b0_flags = get_fileformat(buf) + 1;
	سیٹ_b0_fname(b0p, buf);
	(فراغ)get_user_name(b0p->b0_uname, B0_UNAME_ناپ);
	b0p->b0_uname[B0_UNAME_ناپ - 1] = NUL;
	mch_get_host_name(b0p->b0_hname, B0_HNAME_ناپ);
	b0p->b0_hname[B0_HNAME_ناپ - 1] = NUL;
	long_منزل_char(mch_get_pid(), b0p->b0_pid);
#جے_تعریف_شدہ FEAT_CRYPT
	ml_سیٹ_b0_crypt(buf, b0p);
#جے_انت
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
    pp = (PTR_BL *)(hp->bh_انکڑے);
    pp->pb_گنتی = 1;
    pp->pb_اشارہ_گر[0].pe_bnum = 2;
    pp->pb_اشارہ_گر[0].pe_ورقہ_گنتی = 1;
    pp->pb_اشارہ_گر[0].pe_old_lnum = 1;
    pp->pb_اشارہ_گر[0].pe_سطر_گنتی = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    إذا ((hp = ml_new_انکڑے(mfp, FALSE, 1)) == عدم)
	اذهب error;
    إذا (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_بلاک_nr_two);
	اذهب error;
    }

    dp = (انکڑے_BL *)(hp->bh_انکڑے);
    dp->db_اشاریہ[0] = --dp->db_txt_شروع;	// at end of block
    dp->db_free -= 1 + اشاریہ_ناپ;
    dp->db_سطر_گنتی = 1;
    *((char_u *)dp + dp->db_txt_شروع) = NUL;	// empty line

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
crypt_may_close_swapfile(buf_T *buf, char_u *کلید, صحيح طریقہ)
{
    إذا (crypt_طریقہ_is_sodium(طریقہ) && *کلید != NUL)
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
ml_سیٹ_mfp_crypt(buf_T *buf)
{
    إذا (*buf->b_p_کلید == NUL)
	أرجع;

    صحيح طریقہ_nr = crypt_get_طریقہ_nr(buf);

    إذا (طریقہ_nr > CRYPT_M_ZIP && طریقہ_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_بیج(buf->b_ml.ml_mfp->mf_بیج, MF_بیج_LEN, عدم, 0);
    }
# ifdef FEAT_SODIUM
    وإلا إذا (crypt_طریقہ_is_sodium(طریقہ_nr))
	crypt_sodium_randombytes_buf(buf->b_ml.ml_mfp->mf_بیج, MF_بیج_LEN);
# endif
}

/*
 * Prepare encryption for "buf" with block 0 "b0p".
 * Note: should not be called with libsodium encryption, since xchacha20 does
 * not support swapfile encryption.
 */
    ساكن فراغ
ml_سیٹ_b0_crypt(buf_T *buf, ZERO_BL *b0p)
{
    إذا (*buf->b_p_کلید == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    وإلا
    {
	صحيح طریقہ_nr = crypt_get_طریقہ_nr(buf);

	b0p->b0_id[1] = id1_codes[طریقہ_nr];
	إذا (طریقہ_nr > CRYPT_M_ZIP && طریقہ_nr < CRYPT_M_SOD)
	{
	    // Generate a seed and store it in block 0 and in the memfile.
	    sha2_بیج(&b0p->b0_بیج, MF_بیج_LEN, عدم, 0);
	    mch_memmove(buf->b_ml.ml_mfp->mf_بیج, &b0p->b0_بیج, MF_بیج_LEN);
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
ml_سیٹ_crypt_کلید(
    buf_T	*buf,
    char_u	*old_کلید,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    صحيح		ورقہ_گنتی;
    صحيح		idx;
    لما	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    انکڑے_BL	*dp;
    blocknr_T	bnum;
    صحيح		چوٹی;
    صحيح		old_طریقہ;

    إذا (mfp == عدم || mfp->mf_fd < 0)
	أرجع;  // no memfile yet, nothing to do
    old_طریقہ = crypt_طریقہ_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    إذا (crypt_may_close_swapfile(buf, buf->b_p_کلید, crypt_get_طریقہ_nr(buf)))
	أرجع;
# endif

    // First make sure the swapfile is in a consistent state, using the old
    // key and method.
    {
	char_u *new_کلید = buf->b_p_کلید;
	char_u *new_buf_cm = buf->b_p_cm;

	buf->b_p_کلید = old_کلید;
	buf->b_p_cm = old_cm;
	ml_preserve(buf, FALSE);
	buf->b_p_کلید = new_کلید;
	buf->b_p_cm = new_buf_cm;
    }

    // Set the key, method and seed to be used for reading, these must be the
    // old values.
    mfp->mf_old_کلید = old_کلید;
    mfp->mf_old_cm = old_طریقہ;
    إذا (old_طریقہ > 0 && *old_کلید != NUL)
	mch_memmove(mfp->mf_old_بیج, mfp->mf_بیج, MF_بیج_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    إذا (mfp->mf_infile_گنتی > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_سطر(buf);		    // flush buffered line
	(فراغ)ml_لبھو_سطر(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = عدم;
	bnum = 1;		// start with block 1
	ورقہ_گنتی = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_ڈھیر_چوٹی = 0;
	VIM_CLEAR(buf->b_ml.ml_ڈھیر);
	buf->b_ml.ml_ڈھیر_ناپ = 0;	// no stack yet

	لكل ( ; !got_int; سطر_breakcheck())
	{
	    إذا (hp != عدم)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    إذا ((hp = mf_get(mfp, bnum, ورقہ_گنتی)) == عدم)
	    {
		إذا (bnum == 1)
		    اكسر;
		++error;
	    }
	    وإلا
	    {
		pp = (PTR_BL *)(hp->bh_انکڑے);
		إذا (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    إذا (pp->pb_گنتی == 0)
		    {
			// empty block?
			++error;
		    }
		    وإلا إذا (idx < (صحيح)pp->pb_گنتی)	// go a block deeper
		    {
			إذا (pp->pb_اشارہ_گر[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    استمر;
			}

			// going one block deeper in the tree, new entry in
			//ڈھیر
			إذا ((چوٹی = ml_جوڑو_ڈھیر(buf)) < 0)
			{
			    ++error;
			    اكسر;		    // out of memory
			}
			ip = &(buf->b_ml.ml_ڈھیر[چوٹی]);
			ip->ip_bnum = bnum;
			ip->ip_اشاریہ = idx;

			bnum = pp->pb_اشارہ_گر[idx].pe_bnum;
			ورقہ_گنتی = pp->pb_اشارہ_گر[idx].pe_ورقہ_گنتی;
			idx = 0;
			استمر;
		    }
		}
		وإلا	    // not a pointer block
		{
		    dp = (انکڑے_BL *)(hp->bh_انکڑے);
		    إذا (dp->db_id != انکڑے_ID)	// block id wrong
			++error;
		    وإلا
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = عدم;
		    }
		}
	    }

	    إذا (buf->b_ml.ml_ڈھیر_چوٹی == 0)	//مکمل
		اكسر;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_ڈھیر[--(buf->b_ml.ml_ڈھیر_چوٹی)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_اشاریہ + 1;	    // go to next index
	    ورقہ_گنتی = 1;
	}
	إذا (hp != عدم)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	إذا (error > 0)
	    emsg(_(e_error_while_updating_وٹاندرا_file_crypt));
    }

    mfp->mf_old_کلید = عدم;
}
#جے_انت

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
#جے_انت

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
	p = بھریا_Name_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	إذا (fname == عدم)
	    استمر;
#جے_انت
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
	    mf_سیٹ_ffname(mfp);
#جے_انت
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
	    emsg(_(e_oops_گواچا_the_وٹاندرا_file));
	    أرجع;
	}
#جے_تعریف_شدہ HAVE_FD_CLOEXEC
	{
	    صحيح fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    إذا (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(فراغ)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#جے_انت
    }
    إذا (!success)
	emsg(_(e_could_not_rename_وٹاندرا_file));
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
	إذا (!buf->b_p_ro || buf->b_بدلیا)
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

#جے_تعریف_شدہ FEAT_SPELL
    // For a spell buffer use a temp file name.
    إذا (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	إذا (fname != عدم)
	    (فراغ)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_وٹاندرا = false;
	أرجع;
    }
#جے_انت

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
#جے_انت
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    إذا (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_سیٹ_dirty(mfp);
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
	(فراغ)semsg(_(e_unable_منزل_open_وٹاندرا_file_for_str_recovery_impossible),
		    buf_spname(buf) != عدم ? buf_spname(buf) : buf->b_fname);
	--no_wait_return;
    }

    // don't try to open a swap file again
    buf->b_may_وٹاندرا = false;
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
    فراغ
check_need_وٹاندرا(
    صحيح	    newfile)		// reading file into new buffer
{
    صحيح old_پیغام_silent = پیغام_silent; // might be reset by an E325 message

    إذا (curbuf->b_may_وٹاندرا && (!curbuf->b_p_ro || !newfile))
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
    إذا (buf->b_ml.ml_سطر_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_سطر_ptr);
    vim_free(buf->b_ml.ml_ڈھیر);
#جے_تعریف_شدہ FEAT_BYTEOFF
    VIM_CLEAR(buf->b_ml.ml_chunksize);
#جے_انت
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
#جے_تعریف_شدہ FEAT_SPELL
    spell_delete_wordlist();	// delete the internal wordlist
#جے_انت
#جے_تعریف_شدہ TEMPDIRNAMES
    vim_deltempdir();		// delete created temp directory
#جے_انت
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
	إذا (!buf_Is_بدلیا(buf))
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
#جے_تعریف_شدہ FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	إذا (what == UB_CRYPT)
	    ml_سیٹ_mfp_crypt(buf);
#جے_انت
	أرجع;
    }

    b0p = (ZERO_BL *)(hp->bh_انکڑے);
    إذا (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_بلاک_zero);
    وإلا
    {
	إذا (what == UB_FNAME)
	    سیٹ_b0_fname(b0p, buf);
#جے_تعریف_شدہ FEAT_CRYPT
	وإلا إذا (what == UB_CRYPT)
	    ml_سیٹ_b0_crypt(buf, b0p);
#جے_انت
	وإلا // what == UB_SAME_DIR
	    سیٹ_b0_dir_flag(b0p, buf);
    }
    mf_put(mfp, hp, TRUE, FALSE);
}

/*
 * Write file name and timestamp into block 0 of a swap file.
 * Also set buf->b_mtime.
 * Don't use NameBuff[]!!!
 */
    ساكن فراغ
سیٹ_b0_fname(ZERO_BL *b0p, buf_T *buf)
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
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_ناپ_CRYPT - 1);
# ifdef BACKSLASH_وچ_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#وإلا
	size_t	flen;
	char_u	uname[B0_UNAME_ناپ];

	/*
	 * For a file under the home directory of the current user, we try to
	 * replace the home directory path with "~user". This helps when
	 * editing the same file on different machines over a network.
	 * First replace home dir path with "~/" with home_replace().
	 * Then insert the user name to get "~user/".
	 */
	flen = home_replace(عدم, buf->b_ffname, b0p->b0_fname,
						   B0_FNAME_ناپ_CRYPT, TRUE);
	إذا (b0p->b0_fname[0] == '~')
	{
	    size_t  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    إذا (get_user_name(uname, B0_UNAME_ناپ) == FAIL
		   || (ulen = STRLEN(uname)) + flen > B0_FNAME_ناپ_CRYPT - 1)
		vim_strncpy(b0p->b0_fname, buf->b_ffname,
						     B0_FNAME_ناپ_CRYPT - 1);
	    وإلا
	    {
		mch_memmove(b0p->b0_fname + ulen + 1, b0p->b0_fname + 1, flen);
		mch_memmove(b0p->b0_fname + 1, uname, ulen);
	    }
	}
#جے_انت
	إذا (mch_stat((حرف *)buf->b_ffname, &st) >= 0)
	{
	    long_منزل_char((لما)st.st_mtime, b0p->b0_mtime);
#جے_تعریف_شدہ CHECK_INODE
	    long_منزل_char((لما)st.st_ino, b0p->b0_ino);
#جے_انت
	    buf_store_ویلا(buf, &st, buf->b_ffname);
	    buf->b_mtime_read = buf->b_mtime;
	    buf->b_mtime_read_ns = buf->b_mtime_ns;
	}
	وإلا
	{
	    long_منزل_char(0L, b0p->b0_mtime);
#جے_تعریف_شدہ CHECK_INODE
	    long_منزل_char(0L, b0p->b0_ino);
#جے_انت
	    buf->b_mtime = 0;
	    buf->b_mtime_ns = 0;
	    buf->b_mtime_read = 0;
	    buf->b_mtime_read_ns = 0;
	    buf->b_orig_ناپ = 0;
	    buf->b_orig_منوال = 0;
	}
    }

    // Also add the 'fileencoding' if there is room.
    جوڑو_b0_fenc(b0p, curbuf);
}

/*
 * Update the B0_SAME_DIR flag of the swap file.  It's set if the file and the
 * swapfile for "buf" are in the same directory.
 * This is fail safe: if we are not sure the directories are equal the flag is
 * not set.
 */
    ساكن فراغ
سیٹ_b0_dir_flag(ZERO_BL *b0p, buf_T *buf)
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
جوڑو_b0_fenc(
    ZERO_BL	*b0p,
    buf_T	*buf)
{
    صحيح		n;
    صحيح		ناپ = B0_FNAME_ناپ_NOCRYPT;

#جے_تعریف_شدہ FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    إذا (*buf->b_p_کلید != NUL)
	ناپ = B0_FNAME_ناپ_CRYPT;
#جے_انت

    n = (صحيح)STRLEN(buf->b_p_fenc);
    إذا ((صحيح)STRLEN(b0p->b0_fname) + n + 1 > ناپ)
	b0p->b0_flags &= ~B0_HAS_FENC;
    وإلا
    {
	mch_memmove((حرف *)b0p->b0_fname + ناپ - n,
					    (حرف *)buf->b_p_fenc, (size_t)n);
	*(b0p->b0_fname + ناپ - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#إذا defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#جے_انت

#إذا defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    ساكن صحيح
swapfile_process_running(ZERO_BL *b0p, char_u *وٹاندرا_fname بن_ورتیا)
{
# إذا defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    بنية sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    إذا (mch_stat((حرف *)وٹاندرا_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < ویلا(عدم) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	أرجع FALSE;
# endif
    أرجع mch_process_running(char_منزل_long(b0p->b0_pid));
}
#جے_انت

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
    char_u	*fname_ورتیا = عدم;
    bhdr_T	*hp = عدم;
    ZERO_BL	*b0p;
    صحيح		b0_ff;
    char_u	*b0_fenc = عدم;
#جے_تعریف_شدہ FEAT_CRYPT
    صحيح		b0_cm = -1;
#جے_انت
    PTR_BL	*pp;
    انکڑے_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    صحيح		ورقہ_گنتی;
    stat_T	org_stat, swp_stat;
    صحيح		len;
    صحيح		directly;
    linenr_T	lnum;
    char_u	*p;
    صحيح		i;
    لما	error;
    صحيح		cannot_open;
    linenr_T	سطر_گنتی;
    صحيح		has_error;
    صحيح		idx;
    صحيح		چوٹی;
    صحيح		txt_شروع;
    off_T	ناپ;
    صحيح		called_from_main;
    صحيح		serious_error = TRUE;
    لما	mtime;
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
#جے_انت
						== 0
		&& vim_strchr((char_u *)"abcdefghijklmnopqrstuvw",
					   TOLOWER_ASC(fname[len - 2])) != عدم
		&& ASCII_ISALPHA(fname[len - 1]))
    {
	directly = TRUE;
	fname_ورتیا = vim_strsave(fname); // make a copy for mf_open()
    }
    وإلا
    {
	directly = FALSE;

	// count the number of matching swap files
	len = recover_ناں(fname, FALSE, عدم, 0, عدم);
	إذا (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_وٹاندرا_file_لبھیا_for_str), fname);
	    اذهب theend;
	}
	إذا (len == 1)		    // one swap file found, use it
	    i = 1;
	وإلا			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (فراغ)recover_ناں(fname, TRUE, عدم, 0, عدم);
	    پیغام_putchar('\n');
	    پیغام_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_نمبر(FALSE, عدم);
	    إذا (i < 1 || i > len)
		اذهب theend;
	}
	// get the swap file name that will be used
	(فراغ)recover_ناں(fname, FALSE, عدم, i, &fname_ورتیا);
    }
    إذا (fname_ورتیا == عدم)
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
    buf->b_ml.ml_ڈھیر_ناپ = 0;	// no stack yet
    buf->b_ml.ml_ڈھیر = عدم;		// no stack yet
    buf->b_ml.ml_ڈھیر_چوٹی = 0;		// nothing in the stack
    buf->b_ml.ml_سطر_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = عدم;		// no locked block
    buf->b_ml.ml_flags = 0;
#جے_تعریف_شدہ FEAT_CRYPT
    buf->b_p_کلید = خالی_تھاواں_option;
    buf->b_p_cm = خالی_تھاواں_option;
#جے_انت

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_ورتیا); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_ورتیا, O_RDONLY);
    fname_ورتیا = p;
    إذا (mfp == عدم || mfp->mf_fd < 0)
    {
	إذا (fname_ورتیا != عدم)
	    semsg(_(e_cannot_open_str), fname_ورتیا);
	اذهب theend;
    }
    buf->b_ml.ml_mfp = mfp;
#جے_تعریف_شدہ FEAT_CRYPT
    mfp->mf_بفر = buf;
#جے_انت

    /*
     * The page size set in mf_open() might be different from the page size
     * used in the swap file, we must get it from block 0.  But to read block
     * 0 we need a page size.  Use the minimal size for block 0 here, it will
     * be set to the real value below.
     */
    mfp->mf_ورقہ_ناپ = MIN_وٹاندرا_ورقہ_ناپ;

    /*
     * try to read block 0
     */
    إذا ((hp = mf_get(mfp, (blocknr_T)0, 1)) == عدم)
    {
	پیغام_شروع();
	پیغام_puts_attr(_("Unable to read block 0 from "), attr | پیغام_HIST);
	پیغام_outtrans_attr(mfp->mf_fname, attr | پیغام_HIST);
	پیغام_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | پیغام_HIST);
	پیغام_سرا();
	اذهب theend;
    }
    b0p = (ZERO_BL *)(hp->bh_انکڑے);
    إذا (STRNCMP(b0p->b0_نسخہ, "VIM 3.0", 7) == 0)
    {
	پیغام_شروع();
	پیغام_outtrans_attr(mfp->mf_fname, پیغام_HIST);
	پیغام_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    پیغام_HIST);
	پیغام_puts_attr(_("Use Vim version 3.0.\n"), پیغام_HIST);
	پیغام_سرا();
	اذهب theend;
    }
    إذا (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_وٹاندرا_file), mfp->mf_fname);
	اذهب theend;
    }
    إذا (b0_magic_wrong(b0p))
    {
	پیغام_شروع();
	پیغام_outtrans_attr(mfp->mf_fname, attr | پیغام_HIST);
#إذا defined(MSWIN)
	إذا (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    پیغام_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | پیغام_HIST);
	وإلا
#جے_انت
	    پیغام_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | پیغام_HIST);
	پیغام_puts_attr(_("The file was created on "), attr | پیغام_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	پیغام_puts_attr((حرف *)b0p->b0_hname, attr | پیغام_HIST);
	پیغام_puts_attr(_(",\nor the file has been damaged."), attr | پیغام_HIST);
	پیغام_سرا();
	اذهب theend;
    }

#جے_تعریف_شدہ FEAT_CRYPT
    لكل (i = 0; i < (صحيح)ARRAY_لمائی(id1_codes); ++i)
	إذا (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    إذا (b0_cm > 0)
	mch_memmove(mfp->mf_بیج, &b0p->b0_بیج, MF_بیج_LEN);
    crypt_سیٹ_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#وإلا
    إذا (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_نسخہ_دا_vim_does_not_support_encryption), mfp->mf_fname);
	اذهب theend;
    }
#جے_انت

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    إذا (mfp->mf_ورقہ_ناپ != (بنانشانی)char_منزل_long(b0p->b0_ورقہ_ناپ))
    {
	بنانشانی پچھلا_ورقہ_ناپ = mfp->mf_ورقہ_ناپ;

	mf_new_ورقہ_ناپ(mfp, (بنانشانی)char_منزل_long(b0p->b0_ورقہ_ناپ));
	إذا (mfp->mf_ورقہ_ناپ < پچھلا_ورقہ_ناپ)
	{
	    پیغام_شروع();
	    پیغام_outtrans_attr(mfp->mf_fname, attr | پیغام_HIST);
	    پیغام_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | پیغام_HIST);
	    پیغام_سرا();
	    اذهب theend;
	}
	إذا ((ناپ = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	وإلا
	    mfp->mf_blocknr_max = (blocknr_T)(ناپ / mfp->mf_ورقہ_ناپ);
	mfp->mf_infile_گنتی = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_ورقہ_ناپ);
	إذا (p == عدم)
	    اذهب theend;
	mch_memmove(p, hp->bh_انکڑے, پچھلا_ورقہ_ناپ);
	vim_free(hp->bh_انکڑے);
	hp->bh_انکڑے = p;
	b0p = (ZERO_BL *)(hp->bh_انکڑے);
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
	emsg(_(e_warning_original_file_may_have_been_بدلیا));
    باہرلا_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_نقاب);
    إذا (b0p->b0_flags & B0_HAS_FENC)
    {
	صحيح fnsize = B0_FNAME_ناپ_NOCRYPT;

#جے_تعریف_شدہ FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	إذا (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_ناپ_CRYPT;
#جے_انت
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
    طالما (!(curbuf->b_ml.ml_flags & ML_خالی_تھاواں))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    إذا (curbuf->b_ffname != عدم)
	orig_file_حالت = readfile(curbuf->b_ffname, عدم, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, عدم, READ_NEW);

#جے_تعریف_شدہ FEAT_CRYPT
    إذا (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	إذا (*curbuf->b_p_کلید != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_ورتیا);
	    پیغام_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    پیغام_puts(_("\nenter the new crypt key."));
	    پیغام_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    پیغام_puts(_("\nto use the same key for text file and swap file"));
	}
	وإلا
	    smsg(_(need_کلید_پیغام), fname_ورتیا);
	buf->b_p_کلید = crypt_get_کلید(FALSE, FALSE);
	إذا (buf->b_p_کلید == عدم)
	    buf->b_p_کلید = curbuf->b_p_کلید;
	وإلا إذا (*buf->b_p_کلید == NUL)
	{
	    vim_free(buf->b_p_کلید);
	    buf->b_p_کلید = curbuf->b_p_کلید;
	}
	إذا (buf->b_p_کلید == عدم)
	    buf->b_p_کلید = خالی_تھاواں_option;
    }
#جے_انت

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    إذا (b0_ff != 0)
	سیٹ_fileformat(b0_ff - 1, OPT_LOCAL);
    إذا (b0_fenc != عدم)
    {
	سیٹ_option_قدر_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    ورقہ_گنتی = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    سطر_گنتی = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_ڈھیر_چوٹی = 0;
    buf->b_ml.ml_ڈھیر = عدم;
    buf->b_ml.ml_ڈھیر_ناپ = 0;	// no stack yet

    إذا (curbuf->b_ffname == عدم)
	cannot_open = TRUE;
    وإلا
	cannot_open = FALSE;

    serious_error = FALSE;
    لكل ( ; !got_int; سطر_breakcheck())
    {
	إذا (hp != عدم)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	إذا ((hp = mf_get(mfp, bnum, ورقہ_گنتی)) == عدم)
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
	    pp = (PTR_BL *)(hp->bh_انکڑے);
	    إذا (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		صحيح ptr_بلاک_error = FALSE;
		إذا (pp->pb_گنتی_max != PB_گنتی_MAX(mfp))
		{
		    ptr_بلاک_error = TRUE;
		    pp->pb_گنتی_max = PB_گنتی_MAX(mfp);
		}
		إذا (pp->pb_گنتی > pp->pb_گنتی_max)
		{
		    ptr_بلاک_error = TRUE;
		    pp->pb_گنتی = pp->pb_گنتی_max;
		}
		إذا (ptr_بلاک_error)
		    emsg(_(e_warning_اشارہ_گر_بلاک_corrupted));

		// check line count when using pointer block first time
		إذا (idx == 0 && سطر_گنتی != 0)
		{
		    لكل (i = 0; i < (صحيح)pp->pb_گنتی; ++i)
			سطر_گنتی -= pp->pb_اشارہ_گر[i].pe_سطر_گنتی;
		    إذا (سطر_گنتی != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		إذا (pp->pb_گنتی == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		وإلا إذا (idx < (صحيح)pp->pb_گنتی)	// go a block deeper
		{
		    إذا (pp->pb_اشارہ_گر[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			إذا (!cannot_open)
			{
			    سطر_گنتی = pp->pb_اشارہ_گر[idx].pe_سطر_گنتی;
			    linenr_T pe_old_lnum = pp->pb_اشارہ_گر[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    إذا (سطر_گنتی <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, عدم, lnum,
					pe_old_lnum - 1,
					سطر_گنتی, عدم, 0) != OK)
				cannot_open = TRUE;
			    وإلا
				lnum += سطر_گنتی;
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
		    إذا ((چوٹی = ml_جوڑو_ڈھیر(buf)) < 0)	// new entry in stack
		    {
			++error;
			اكسر;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_ڈھیر[چوٹی]);
		    ip->ip_bnum = bnum;
		    ip->ip_اشاریہ = idx;

		    bnum = pp->pb_اشارہ_گر[idx].pe_bnum;
		    سطر_گنتی = pp->pb_اشارہ_گر[idx].pe_سطر_گنتی;
		    ورقہ_گنتی = pp->pb_اشارہ_گر[idx].pe_ورقہ_گنتی;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    إذا (ورقہ_گنتی < 1
			    || bnum + ورقہ_گنتی > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_اشاریہ + 1;
			bnum = ip->ip_bnum;
			ورقہ_گنتی = 1;
			--buf->b_ml.ml_ڈھیر_چوٹی;
			استمر;
		    }
		    idx = 0;
		    استمر;
		}
	    }
	    وإلا	    // not a pointer block
	    {
		dp = (انکڑے_BL *)(hp->bh_انکڑے);
		إذا (dp->db_id != انکڑے_ID)	// block id wrong
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
		    إذا (hp->bh_ورقہ_گنتی != ورقہ_گنتی)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			ورقہ_گنتی = hp->bh_ورقہ_گنتی;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    إذا (ورقہ_گنتی * mfp->mf_ورقہ_ناپ != dp->db_txt_سرا)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_سرا = ورقہ_گنتی * mfp->mf_ورقہ_ناپ;
		    }

		    إذا (dp->db_txt_شروع < سرخی_ناپ
			    || dp->db_txt_شروع > dp->db_txt_سرا)
		    {
			ml_append(lnum++, (char_u *)_("??? block header corrupted"),
								    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_شروع = dp->db_txt_سرا;
		    }
		    // Make sure there is a NUL at the end of the block so we
		    // don't go over the end when copying text.
		    *((char_u *)dp + dp->db_txt_سرا - 1) = NUL;

		    // Check the number of lines in the block.
		    // If wrong, use the count in the data block.
		    إذا (سطر_گنتی != dp->db_سطر_گنتی)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    صحيح did_questions = FALSE;
		    لكل (i = 0; i < dp->db_سطر_گنتی; ++i)
		    {
			إذا ((char_u *)&(dp->db_اشاریہ[i])
					    >= (char_u *)dp + dp->db_txt_شروع)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    اكسر;
			}

			txt_شروع = (dp->db_اشاریہ[i] & DB_اشاریہ_نقاب);
			إذا (txt_شروع <= (صحيح)سرخی_ناپ
					  || txt_شروع >= (صحيح)dp->db_txt_سرا)
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
			    p = (char_u *)dp + txt_شروع;
			}
			ml_append(lnum++, p, (colnr_T)0, TRUE);
		    }
		    إذا (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	إذا (buf->b_ml.ml_ڈھیر_چوٹی == 0)	//مکمل
	    اكسر;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_ڈھیر[--(buf->b_ml.ml_ڈھیر_چوٹی)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_اشاریہ + 1;	    // go to next index
	ورقہ_گنتی = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    إذا (orig_file_حالت != OK || curbuf->b_ml.ml_سطر_گنتی != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	إذا (!(curbuf->b_ml.ml_سطر_گنتی == 2 && *ml_get(1) == NUL))
	{
	    بدلیا_اندرونی();
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
		بدلیا_اندرونی();
		++CHANGEDTICK(curbuf);
		اكسر;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    طالما (curbuf->b_ml.ml_سطر_گنتی > lnum
				       && !(curbuf->b_ml.ml_flags & ML_خالی_تھاواں))
	ml_delete(curbuf->b_ml.ml_سطر_گنتی);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    إذا (got_int)
	emsg(_(e_recovery_interrupted));
    وإلا إذا (error)
    {
	++no_wait_return;
	پیغام(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_سطریں_starting_with_questions));
	--no_wait_return;
	پیغام(_("See \":help E312\" for more information."));
	پیغام(">>>>>>>>>>>>>");
    }
    وإلا
    {
	إذا (curbuf->b_بدلیا)
	{
	    پیغام(_("Recovery completed. You should check if everything is OK."));
	    پیغام_puts(_("\n(You might want to write out this file under another name\n"));
	    پیغام_puts(_("and run diff with the original file to check for changes)"));
	}
	وإلا
	    پیغام(_("Recovery completed. Buffer contents equals file contents."));
	پیغام_puts(_("\nYou may want to delete the .swp file now."));
#إذا defined(UNIX) || defined(MSWIN)
	إذا (swapfile_process_running(b0p, fname_ورتیا))
	{
	    // Warn there could be an active Vim on the same file, the user may
	    // want to kill it.
	    پیغام_puts(_("\nNote: process STILL RUNNING: "));
	    پیغام_outnum(char_منزل_long(b0p->b0_pid));
	}
#جے_انت
	پیغام_puts("\n\n");
	cmdline_صف = پیغام_صف;
    }
#جے_تعریف_شدہ FEAT_CRYPT
    إذا (*buf->b_p_کلید != NUL && STRCMP(curbuf->b_p_کلید, buf->b_p_کلید) != 0)
    {
	پیغام_puts(_("Using crypt key from swap file for the text file.\n"));
	سیٹ_option_قدر_give_err((char_u *)"key", 0L, buf->b_p_کلید, OPT_LOCAL);
    }
#جے_انت
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_ورتیا);
    recoverymode = FALSE;
    إذا (mfp != عدم)
    {
	إذا (hp != عدم)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    إذا (buf != عدم)
    {
#جے_تعریف_شدہ FEAT_CRYPT
	إذا (buf->b_p_کلید != curbuf->b_p_کلید)
	    free_string_option(buf->b_p_کلید);
	free_string_option(buf->b_p_cm);
#جے_انت
	vim_free(buf->b_ml.ml_ڈھیر);
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
recover_ناں(
    char_u	*fname,		// base for swap file name
    صحيح		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list بن_ورتیا, // when not NULL add file names to it
    صحيح		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_باہرلا)	// result when "nr" > 0
{
    صحيح		عدد_ناں;
    char_u	*(ناں[6]);
    char_u	*p;
    صحيح		عدد_files;
    صحيح		file_گنتی = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = عدم;
#جے_تعریف_شدہ HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#جے_انت

    إذا (fname != عدم)
    {
#جے_تعریف_شدہ HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	إذا (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	وإلا
#جے_انت
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
	dir_name.لمائی = (size_t)copy_option_part(&dirp, dir_name.string, 31000, ",");

	إذا (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    إذا (fname == عدم)
	    {
#جے_تعریف_شدہ VMS
		ناں[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#وإلا
		ناں[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#جے_انت
#إذا defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		ناں[1] = vim_strnsave((char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"));
		ناں[2] = vim_strnsave((char_u *)".sw?", STRLEN_LITERAL(".sw?"));
		عدد_ناں = 3;
#وإلا
# ifdef VMS
		ناں[1] = vim_strnsave((char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"));
		عدد_ناں = 2;
# وإلا
		عدد_ناں = 1;
# endif
#جے_انت
	    }
	    وإلا
		عدد_ناں = recov_file_ناں(ناں, fname_res, TRUE);
	}
	وإلا			    // check directory dir_name
	{
	    إذا (fname == عدم)
	    {
		string_T    ret;

#جے_تعریف_شدہ VMS
		ناں[0] = concat_fnames(dir_name.string, dir_name.لمائی,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#وإلا
		ناں[0] = concat_fnames(dir_name.string, dir_name.لمائی,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#جے_انت
#إذا defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		ناں[1] = concat_fnames(dir_name.string, dir_name.لمائی,
		    (char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"), TRUE, &ret);
		ناں[2] = concat_fnames(dir_name.string, dir_name.لمائی,
		    (char_u *)".sw?", STRLEN_LITERAL(".sw?"), TRUE, &ret);
		عدد_ناں = 3;
#وإلا
# ifdef VMS
		ناں[1] = concat_fnames(dir_name.string, dir_name.لمائی,
		    (char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"), TRUE, &ret);
		عدد_ناں = 2;
# وإلا
		عدد_ناں = 1;
# endif
#جے_انت
	    }
	    وإلا
	    {
		char_u	*پوچھ;

#إذا defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.لمائی;
		إذا (after_pathsep(dir_name.string, p) && dir_name.لمائی > 1 && p[-1] == p[-2])
		{
		    // Ends with '//', Use Full path for swap name
		    پوچھ = make_percent_swname(dir_name.string, p, fname_res);
		}
		وإلا
#جے_انت
		{
		    string_T	ret;

		    پوچھ = gettail(fname_res);
		    پوچھ = concat_fnames(dir_name.string, dir_name.لمائی,
			پوچھ, STRLEN(پوچھ), TRUE, &ret);
		}
		إذا (پوچھ == عدم)
		    عدد_ناں = 0;
		وإلا
		{
		    عدد_ناں = recov_file_ناں(ناں, پوچھ, FALSE);
		    vim_free(پوچھ);
		}
	    }
	}

	// check for out-of-memory
	لكل (صحيح i = 0; i < عدد_ناں; ++i)
	{
	    إذا (ناں[i] == عدم)
	    {
		لكل (i = 0; i < عدد_ناں; ++i)
		    vim_free(ناں[i]);
		عدد_ناں = 0;
	    }
	}
	إذا (عدد_ناں == 0)
	    عدد_files = 0;
	وإلا إذا (expand_wildcards(عدد_ناں, ناں, &عدد_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    عدد_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	إذا (*dirp == NUL && file_گنتی + عدد_files == 0 && fname != عدم)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#إذا defined(VMS)
			       (char_u *)"_swp", FALSE
#وإلا
			       (char_u *)".swp", TRUE
#جے_انت
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
	    file_گنتی += عدد_files;
	    إذا (nr <= file_گنتی)
	    {
		*fname_باہرلا = vim_strsave(
				      files[nr - 1 + عدد_files - file_گنتی]);
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
		    پیغام_outnum((لما)++file_گنتی);
		    پیغام_puts(".    ");
		    پیغام_puts((حرف *)gettail(files[i]));
		    پیغام_putchar('\n');
		    (فراغ)swapfile_info(files[i]);
		}
	    }
	    وإلا
		پیغام_puts(_("      -- none --\n"));
	    باہرلا_flush();
	}
#جے_تعریف_شدہ FEAT_EVAL
	وإلا إذا (ret_list != عدم)
	{
	    string_T	name;

	    لكل (صحيح i = 0; i < عدد_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.لمائی,
		    files[i], STRLEN(files[i]), TRUE, &name);
		إذا (name.string != عدم)
		{
		    list_append_string(ret_list, name.string, (صحيح)name.لمائی);
		    vim_free(name.string);
		}
	    }
	}
#جے_انت
	وإلا
	    file_گنتی += عدد_files;

	لكل (صحيح i = 0; i < عدد_ناں; ++i)
	    vim_free(ناں[i]);
	إذا (عدد_files > 0)
	    FreeWild(عدد_files, files);
    }
    vim_free(dir_name.string);
    أرجع file_گنتی;
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
make_percent_swname(char_u *dir, char_u *dir_سرا, char_u *name)
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
    fixed_fname.لمائی = (size_t)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_سرا[-1];
    *p = NUL;
    concat_fnames(dir, (size_t)(p - dir), fixed_fname.string, fixed_fname.لمائی, TRUE, &d);
    vim_free(fixed_fname.string);

    أرجع d.string;
}
#جے_انت

#إذا (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
ساكن صحيح process_still_running;
#جے_انت

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
		dict_جوڑو_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    وإلا إذا (b0_magic_wrong(&b0))
		dict_جوڑو_string_len(d, "error",
		    (char_u *)"Magic number mismatch", STRLEN_LITERAL("Magic number mismatch"));
	    وإلا
	    {
		// we have swap information
		dict_جوڑو_string_len(d, "version", b0.b0_نسخہ, 10);
		dict_جوڑو_string_len(d, "user", b0.b0_uname, B0_UNAME_ناپ);
		dict_جوڑو_string_len(d, "host", b0.b0_hname, B0_HNAME_ناپ);
		dict_جوڑو_string_len(d, "fname", b0.b0_fname, B0_FNAME_ناپ_ORG);

		dict_جوڑو_نمبر(d, "pid", char_منزل_long(b0.b0_pid));
		dict_جوڑو_نمبر(d, "mtime", char_منزل_long(b0.b0_mtime));
		dict_جوڑو_نمبر(d, "dirty", b0.b0_dirty ? 1 : 0);
# ifdef CHECK_INODE
		dict_جوڑو_نمبر(d, "inode", char_منزل_long(b0.b0_ino));
# endif
	    }
	}
	وإلا
	    dict_جوڑو_string_len(d, "error", (char_u *)"Cannot read file", STRLEN_LITERAL("Cannot read file"));
	close(fd);
    }
    وإلا
	dict_جوڑو_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#جے_انت

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
#جے_تعریف_شدہ UNIX
    char_u	    uname[B0_UNAME_ناپ];
#جے_انت

    // print the swap file date
    إذا (mch_stat((حرف *)fname, &st) != -1)
    {
#جے_تعریف_شدہ UNIX
	// print name of owner of the file
	إذا (mch_get_uname(st.st_uid, uname, B0_UNAME_ناپ) == OK)
	{
	    پیغام_puts(_("          owned by: "));
	    پیغام_outtrans(uname);
	    پیغام_puts(_("   dated: "));
	}
	وإلا
#جے_انت
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
	    إذا (STRNCMP(b0.b0_نسخہ, "VIM 3.0", 7) == 0)
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
#جے_انت
		}

		إذا (b0_magic_wrong(&b0))
		{
#إذا defined(MSWIN)
		    إذا (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			پیغام_puts(_("\n         [not usable with this version of Vim]"));
		    وإلا
#جے_انت
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
	char_u	    hostname[B0_HNAME_ناپ];

	mch_get_host_name(hostname, B0_HNAME_ناپ);
	hostname[B0_HNAME_ناپ - 1] = NUL;
	b0.b0_hname[B0_HNAME_ناپ - 1] = NUL; // in case of corruption
	إذا (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    إذا (char_منزل_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#جے_انت

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    close(fd);
    أرجع ret;
}

    ساكن صحيح
recov_file_ناں(char_u **ناں, char_u *رستہ, صحيح prepend_dot)
{
    صحيح		عدد_ناں;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    صحيح		i;
#جے_بنا_تعریف MSWIN
    bool    shortname = curbuf->b_shortname;

    curbuf->b_shortname = false;
#جے_انت
    string_T	ret;

    عدد_ناں = 0;

    /*
     * May also add the file name with a dot prepended, for swap file in same
     * dir as original file.
     */
    إذا (prepend_dot)
    {
	ناں[عدد_ناں] = modname(رستہ, (char_u *)".sw?", TRUE);
	إذا (ناں[عدد_ناں] == عدم)
	    اذهب سرا;
	++عدد_ناں;
    }

    /*
     * Form the normal swap file name pattern by appending ".sw?".
     */
#جے_تعریف_شدہ VMS
    ناں[عدد_ناں] = concat_fnames(رستہ, STRLEN(رستہ),
	(char_u *)"_sw%", STRLEN_LITERAL("_sw%"), FALSE, &ret);
#وإلا
    ناں[عدد_ناں] = concat_fnames(رستہ, STRLEN(رستہ),
	(char_u *)".sw?", STRLEN_LITERAL(".sw?"), FALSE, &ret);
#جے_انت
    إذا (ناں[عدد_ناں] == عدم)
	اذهب سرا;
    إذا (عدد_ناں >= 1)	    // check if we have the same name twice
    {
	p = ناں[عدد_ناں - 1];
	i = (صحيح)STRLEN(ناں[عدد_ناں - 1]) - (صحيح)STRLEN(ناں[عدد_ناں]);
	إذا (i > 0)
	    p += i;	    // file name has been expanded to full path

	إذا (STRCMP(p, ناں[عدد_ناں]) != 0)
	    ++عدد_ناں;
	وإلا
	    vim_free(ناں[عدد_ناں]);
    }
    وإلا
	++عدد_ناں;

#جے_بنا_تعریف MSWIN
    /*
     * Also try with 'shortname' set, in case the file is on a DOS filesystem.
     */
    curbuf->b_shortname = true;
# ifdef VMS
    ناں[عدد_ناں] = modname(رستہ, (char_u *)"_sw%", FALSE);
# وإلا
    ناں[عدد_ناں] = modname(رستہ, (char_u *)".sw?", FALSE);
# endif
    إذا (ناں[عدد_ناں] == عدم)
	اذهب سرا;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = ناں[عدد_ناں];
    i = STRLEN(ناں[عدد_ناں]) - STRLEN(ناں[عدد_ناں - 1]);
    إذا (i > 0)
	p += i;		// file name has been expanded to full path
    إذا (STRCMP(ناں[عدد_ناں - 1], p) == 0)
	vim_free(ناں[عدد_ناں]);
    وإلا
	++عدد_ناں;
#جے_انت

سرا:
#جے_بنا_تعریف MSWIN
    curbuf->b_shortname = shortname;
#جے_انت

    أرجع عدد_ناں;
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

#جے_تعریف_شدہ FEAT_CRYPT
	إذا (crypt_may_close_swapfile(buf, buf->b_p_کلید,
						     crypt_get_طریقہ_nr(buf)))
	    استمر;
#جے_انت

	ml_flush_سطر(buf);		    // flush buffered line
					    // flush locked block
	(فراغ)ml_لبھو_سطر(buf, (linenr_T)0, ML_FLUSH);
	إذا (buf_Is_بدلیا(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != عدم)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    إذا (mch_stat((حرف *)buf->b_ffname, &st) == -1
		    || st.st_mtime != buf->b_mtime_read
#جے_تعریف_شدہ ST_MTIM_NSEC
		    || st.ST_MTIM_NSEC != buf->b_mtime_read_ns
#جے_انت
		    || st.st_size != buf->b_orig_ناپ)
	    {
		ml_preserve(buf, FALSE);
		did_check_timestamps = FALSE;
		need_check_timestamps = TRUE;	// give message later
	    }
	}
	إذا (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (فراغ)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (buf_Is_بدلیا(buf) ? MFS_FLUSH : 0));
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
ml_preserve(buf_T *buf, صحيح سنیہا)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    صحيح		حالت;
    صحيح		got_int_save = got_int;

    إذا (mfp == عدم || mfp->mf_fname == عدم)
    {
	إذا (سنیہا)
	    emsg(_(e_cannot_preserve_there_is_no_وٹاندرا_file));
	أرجع;
    }
#جے_تعریف_شدہ FEAT_CRYPT
    إذا (crypt_may_close_swapfile(buf, buf->b_p_کلید, crypt_get_طریقہ_nr(buf)))
	أرجع;
#جے_انت

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_سطر(buf);				    // flush buffered line
    (فراغ)ml_لبھو_سطر(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    حالت = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

    // stack is invalid after mf_sync(.., MFS_ALL)
    buf->b_ml.ml_ڈھیر_چوٹی = 0;

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
	طالما (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_سطر_گنتی)
	{
	    hp = ml_لبھو_سطر(buf, lnum, ML_لبھو);
	    إذا (hp == عدم)
	    {
		حالت = FAIL;
		اذهب theend;
	    }
	    CHECK(buf->b_ml.ml_locked_کم != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_بلند + 1;
	}
	(فراغ)ml_لبھو_سطر(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	إذا (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    حالت = FAIL;
	buf->b_ml.ml_ڈھیر_چوٹی = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    إذا (سنیہا)
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
    أرجع (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->خانہ);
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
							curwin->w_cursor.خانہ);
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
    أرجع ml_get_buf_len(curbuf, pos->lnum) - pos->خانہ;
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
    أرجع ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.خانہ;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*سطر;

    إذا (*(سطر = ml_get_buf(buf, lnum, FALSE)) == NUL)
	أرجع 0;

    إذا (buf->b_ml.ml_سطر_textlen <= 0)
	buf->b_ml.ml_سطر_textlen = (صحيح)STRLEN(سطر) + 1;
    أرجع buf->b_ml.ml_سطر_textlen - 1;
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
    انکڑے_BL	*dp;
    ساكن صحيح	recursive = 0;
    ساكن char_u questions[4];

    إذا (lnum > buf->b_ml.ml_سطر_گنتی)	// invalid line number
    {
	إذا (recursive == 0)
	{
	    // Avoid giving this message for a recursive call, may happen when
	    // the GUI redraws part of the text.
	    ++recursive;
	    siemsg(e_ml_get_invalid_lnum_nr, lnum);
	    --recursive;
	}
	ml_flush_سطر(buf);
errorret:
	STRCPY(questions, "???");
	buf->b_ml.ml_سطر_len = 4;
	buf->b_ml.ml_سطر_textlen = buf->b_ml.ml_سطر_len;
	buf->b_ml.ml_سطر_lnum = lnum;
	أرجع questions;
    }
    إذا (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    إذا (buf->b_ml.ml_mfp == عدم)	// there are no lines
    {
	buf->b_ml.ml_سطر_len = 1;
	buf->b_ml.ml_سطر_textlen = buf->b_ml.ml_سطر_len;
	أرجع (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    إذا (buf->b_ml.ml_سطر_lnum != lnum || mf_dont_release)
    {
	بنانشانی    شروع, سرا;
	صحيح	    idx;

	ml_flush_سطر(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	إذا ((hp = ml_لبھو_سطر(buf, lnum, ML_لبھو)) == عدم)
	{
	    إذا (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_لبھو_سطر_nr_وچ_بفر_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    اذهب errorret;
	}

	dp = (انکڑے_BL *)(hp->bh_انکڑے);

	idx = lnum - buf->b_ml.ml_locked_کم;
	شروع = ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	إذا (idx == 0)
	    سرا = dp->db_txt_سرا;
	وإلا
	    سرا = ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب);

	buf->b_ml.ml_سطر_ptr = (char_u *)dp + شروع;
	buf->b_ml.ml_سطر_len = سرا - شروع;
#إذا defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	إذا (buf->b_has_textprop
			 && (size_t)buf->b_ml.ml_سطر_len > حجم(textprop_T))
	    buf->b_ml.ml_سطر_textlen = 0;  // call STRLEN() later when needed
	وإلا
#جے_انت
	    buf->b_ml.ml_سطر_textlen = buf->b_ml.ml_سطر_len;
	buf->b_ml.ml_سطر_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_سطر_DIRTY | ML_ALLOCATED);
    }
    إذا (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#جے_تعریف_شدہ FEAT_EVAL
	إذا (ml_get_alloc_سطریں && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_سطر_DIRTY;
#جے_انت
    }

#جے_تعریف_شدہ FEAT_EVAL
    إذا (ml_get_alloc_سطریں
		 && (buf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_سطر_len);

	// make sure the text is in allocated memory
	إذا (p != عدم)
	{
	    memmove(p, buf->b_ml.ml_سطر_ptr, buf->b_ml.ml_سطر_len);
	    buf->b_ml.ml_سطر_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    إذا (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_سطر_DIRTY;
	}
    }
#جے_انت
    أرجع buf->b_ml.ml_سطر_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    صحيح
ml_سطر_alloced(فراغ)
{
    أرجع (curbuf->b_ml.ml_flags & ML_سطر_DIRTY);
}

#جے_تعریف_شدہ FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    ساكن فراغ
جوڑو_متن_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**سطر,
	    صحيح		*len,
	    char_u	**tofree)
{
    صحيح		گیڑا;
    صحيح		new_prop_گنتی = 0;
    صحيح		گنتی;
    صحيح		n;
    char_u	*props;
    صحيح		new_len = 0;  // init for gcc
    char_u	*new_سطر = عدم;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    لكل (گیڑا = 1; گیڑا <= 2; ++گیڑا)
    {
	إذا (گیڑا == 2)
	{
	    uint16_t pc;

	    إذا (new_prop_گنتی == 0)
		أرجع;  // nothing to do
	    new_len = *len + (صحيح)PROP_گنتی_ناپ
			     + new_prop_گنتی * (صحيح)حجم(textprop_T);
	    new_سطر = alloc(new_len);
	    إذا (new_سطر == عدم)
		أرجع;
	    mch_memmove(new_سطر, *سطر, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_گنتی;
	    mch_memmove(new_سطر + *len, &pc, PROP_گنتی_ناپ);
	    new_prop_گنتی = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	گنتی = get_متن_props(buf, lnum, &props, FALSE);
	لكل (n = 0; n < گنتی; ++n)
	{
	    mch_memmove(&prop, props + n * حجم(textprop_T),
							   حجم(textprop_T));
	    إذا (prop.tp_flags & TP_FLAG_CONT_اگلا)
	    {
		إذا (گیڑا == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_PREV;
		    prop.tp_خانہ = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_متن_وتھ = 0;
		    mch_memmove(new_سطر + *len + (صحيح)PROP_گنتی_ناپ
			    + new_prop_گنتی * حجم(textprop_T),
			    &prop, حجم(textprop_T));
		}
		++new_prop_گنتی;
	    }
	}
    }
    *سطر = new_سطر;
    *tofree = new_سطر;
    *len = new_len;
}
#جے_انت

    ساكن صحيح
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*سطر_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    صحيح		flags)		// ML_APPEND_ flags
{
    char_u	*سطر = سطر_arg;
    colnr_T	len = len_arg;
    صحيح		i;
    صحيح		سطر_گنتی;	// number of indexes in current block
    صحيح		وتھ;
    صحيح		from, منزل;
    لما	space_needed;	// space needed for new line
    صحيح		ورقہ_ناپ;
    صحيح		ورقہ_گنتی;
    صحيح		db_idx;		// index for lnum in data block
    bhdr_T	*hp;
    memfile_T	*mfp;
    انکڑے_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
#جے_تعریف_شدہ FEAT_PROP_POPUP
    char_u	*tofree = عدم;
# ifdef FEAT_BYTEOFF
    colnr_T	متن_len = 0;	// text len with NUL without text properties
# endif
#جے_انت
    صحيح		ret = FAIL;

    إذا (lnum > buf->b_ml.ml_سطر_گنتی || buf->b_ml.ml_mfp == عدم)
	أرجع FAIL;  // lnum out of range

    إذا (سبھ_توں_نیواں_marked && سبھ_توں_نیواں_marked > lnum)
	سبھ_توں_نیواں_marked = lnum + 1;

    إذا (len == 0)
    {
	len = (colnr_T)STRLEN(سطر) + 1;	// space needed for the text
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	متن_len = len;
#جے_انت
    }
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    وإلا إذا (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	متن_len = (colnr_T)STRLEN(سطر) + 1;
    وإلا
	متن_len = len;
#جے_انت

#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	جوڑو_متن_props_for_append(buf, lnum, &سطر, &len, &tofree);
#جے_انت

    space_needed = len + اشاریہ_ناپ;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    ورقہ_ناپ = mfp->mf_ورقہ_ناپ;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    إذا ((hp = ml_لبھو_سطر(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_داخل_کرو)) == عدم)
	اذهب theend;

    buf->b_ml.ml_flags &= ~ML_خالی_تھاواں;

    إذا (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    وإلا
	db_idx = lnum - buf->b_ml.ml_locked_کم;
		// get line count before the insertion
    سطر_گنتی = buf->b_ml.ml_locked_بلند - buf->b_ml.ml_locked_کم;

    dp = (انکڑے_BL *)(hp->bh_انکڑے);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    إذا ((لما)dp->db_free < space_needed && db_idx == سطر_گنتی - 1
					    && lnum < buf->b_ml.ml_سطر_گنتی)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_بلند);
	إذا ((hp = ml_لبھو_سطر(buf, lnum + 1, ML_داخل_کرو)) == عدم)
	    اذهب theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	سطر_گنتی = buf->b_ml.ml_locked_بلند - buf->b_ml.ml_locked_کم;
	CHECK(buf->b_ml.ml_locked_کم != lnum + 1, "locked_low != lnum + 1");

	dp = (انکڑے_BL *)(hp->bh_انکڑے);
    }

    ++buf->b_ml.ml_سطر_گنتی;

    إذا ((لما)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_شروع -= len;
	dp->db_free -= space_needed;
	++(dp->db_سطر_گنتی);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	إذا (سطر_گنتی > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    إذا (db_idx < 0)
		وتھ = dp->db_txt_سرا;
	    وإلا
		وتھ = ((dp->db_اشاریہ[db_idx]) & DB_اشاریہ_نقاب);
	    mch_memmove((حرف *)dp + dp->db_txt_شروع,
					  (حرف *)dp + dp->db_txt_شروع + len,
				 (size_t)(وتھ - (dp->db_txt_شروع + len)));
	    لكل (i = سطر_گنتی - 1; i > db_idx; --i)
		dp->db_اشاریہ[i + 1] = dp->db_اشاریہ[i] - len;
	    dp->db_اشاریہ[db_idx + 1] = وتھ - len;
	}
	وإلا
	    // add line at the end (which is the start of the text)
	    dp->db_اشاریہ[db_idx + 1] = dp->db_txt_شروع;

	/*
	 * copy the text into the block
	 */
	mch_memmove((حرف *)dp + dp->db_اشاریہ[db_idx + 1], سطر, (size_t)len);
	إذا (flags & ML_APPEND_MARK)
	    dp->db_اشاریہ[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	إذا (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    وإلا	    // not enough space in data block
    {
	لما	    سطر_گنتی_بایاں, سطر_گنتی_دایاں;
	صحيح	    ورقہ_گنتی_بایاں, ورقہ_گنتی_دایاں;
	bhdr_T	    *hp_بایاں;
	bhdr_T	    *hp_دایاں;
	bhdr_T	    *hp_new;
	صحيح	    سطریں_moved;
	صحيح	    انکڑے_moved = 0;	    // init to shut up gcc
	صحيح	    کل_moved = 0;	    // init to shut up gcc
	انکڑے_BL	    *dp_دایاں, *dp_بایاں;
	صحيح	    ڈھیر_idx;
	صحيح	    وچ_بایاں;
	صحيح	    lineadd;
	blocknr_T   bnum_بایاں, bnum_دایاں;
	linenr_T    lnum_بایاں, lnum_دایاں;
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
	    سطریں_moved = 0;
	    وچ_بایاں = TRUE;
	    // space_needed does not change
	}
	وإلا			// left block is existing, right block is new
	{
	    سطریں_moved = سطر_گنتی - db_idx - 1;
	    إذا (سطریں_moved == 0)
		وچ_بایاں = FALSE;	// put new line in right block
					// space_needed does not change
	    وإلا
	    {
		انکڑے_moved = ((dp->db_اشاریہ[db_idx]) & DB_اشاریہ_نقاب) -
							    dp->db_txt_شروع;
		کل_moved = انکڑے_moved + سطریں_moved * اشاریہ_ناپ;
		إذا ((لما)dp->db_free + کل_moved >= space_needed)
		{
		    وچ_بایاں = TRUE;	// put new line in left block
		    space_needed = کل_moved;
		}
		وإلا
		{
		    وچ_بایاں = FALSE;	    // put new line in right block
		    space_needed += کل_moved;
		}
	    }
	}

	ورقہ_گنتی = ((space_needed + سرخی_ناپ) + ورقہ_ناپ - 1) / ورقہ_ناپ;
	إذا ((hp_new = ml_new_انکڑے(mfp, flags & ML_APPEND_NEW, ورقہ_گنتی))
								       == عدم)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_بلند);
	    اذهب theend;
	}
	إذا (db_idx < 0)		// left block is new
	{
	    hp_بایاں = hp_new;
	    hp_دایاں = hp;
	    سطر_گنتی_بایاں = 0;
	    سطر_گنتی_دایاں = سطر_گنتی;
	}
	وإلا			// right block is new
	{
	    hp_بایاں = hp;
	    hp_دایاں = hp_new;
	    سطر_گنتی_بایاں = سطر_گنتی;
	    سطر_گنتی_دایاں = 0;
	}
	dp_دایاں = (انکڑے_BL *)(hp_دایاں->bh_انکڑے);
	dp_بایاں = (انکڑے_BL *)(hp_بایاں->bh_انکڑے);
	bnum_بایاں = hp_بایاں->bh_bnum;
	bnum_دایاں = hp_دایاں->bh_bnum;
	ورقہ_گنتی_بایاں = hp_بایاں->bh_ورقہ_گنتی;
	ورقہ_گنتی_دایاں = hp_دایاں->bh_ورقہ_گنتی;

	/*
	 * May move the new line into the right/new block.
	 */
	إذا (!وچ_بایاں)
	{
	    dp_دایاں->db_txt_شروع -= len;
	    dp_دایاں->db_free -= len + اشاریہ_ناپ;
	    dp_دایاں->db_اشاریہ[0] = dp_دایاں->db_txt_شروع;
	    إذا (flags & ML_APPEND_MARK)
		dp_دایاں->db_اشاریہ[0] |= DB_MARKED;

	    mch_memmove((حرف *)dp_دایاں + dp_دایاں->db_txt_شروع,
							   سطر, (size_t)len);
	    ++سطر_گنتی_دایاں;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	إذا (سطریں_moved)
	{
	    dp_دایاں->db_txt_شروع -= انکڑے_moved;
	    dp_دایاں->db_free -= کل_moved;
	    mch_memmove((حرف *)dp_دایاں + dp_دایاں->db_txt_شروع,
			(حرف *)dp_بایاں + dp_بایاں->db_txt_شروع,
			(size_t)انکڑے_moved);
	    وتھ = dp_دایاں->db_txt_شروع - dp_بایاں->db_txt_شروع;
	    dp_بایاں->db_txt_شروع += انکڑے_moved;
	    dp_بایاں->db_free += کل_moved;

	    /*
	     * update indexes in the new block
	     */
	    لكل (منزل = سطر_گنتی_دایاں, from = db_idx + 1;
					 from < سطر_گنتی_بایاں; ++from, ++منزل)
		dp_دایاں->db_اشاریہ[منزل] = dp->db_اشاریہ[from] + وتھ;
	    سطر_گنتی_دایاں += سطریں_moved;
	    سطر_گنتی_بایاں -= سطریں_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	إذا (وچ_بایاں)
	{
	    dp_بایاں->db_txt_شروع -= len;
	    dp_بایاں->db_free -= len + اشاریہ_ناپ;
	    dp_بایاں->db_اشاریہ[سطر_گنتی_بایاں] = dp_بایاں->db_txt_شروع;
	    إذا (flags & ML_APPEND_MARK)
		dp_بایاں->db_اشاریہ[سطر_گنتی_بایاں] |= DB_MARKED;
	    mch_memmove((حرف *)dp_بایاں + dp_بایاں->db_txt_شروع,
							   سطر, (size_t)len);
	    ++سطر_گنتی_بایاں;
	}

	إذا (db_idx < 0)		// left block is new
	{
	    lnum_بایاں = lnum + 1;
	    lnum_دایاں = 0;
	}
	وإلا			// right block is new
	{
	    lnum_بایاں = 0;
	    إذا (وچ_بایاں)
		lnum_دایاں = lnum + 2;
	    وإلا
		lnum_دایاں = lnum + 1;
	}
	dp_بایاں->db_سطر_گنتی = سطر_گنتی_بایاں;
	dp_دایاں->db_سطر_گنتی = سطر_گنتی_دایاں;

	/*
	 * release the two data blocks
	 * The new one (hp_new) already has a correct blocknumber.
	 * The old one (hp, in ml_locked) gets a positive blocknumber if
	 * we changed it and we are not editing a new file.
	 */
	إذا (سطریں_moved || وچ_بایاں)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	إذا (!(flags & ML_APPEND_NEW) && db_idx >= 0 && وچ_بایاں)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_لبھو_سطر(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	لكل (ڈھیر_idx = buf->b_ml.ml_ڈھیر_چوٹی - 1; ڈھیر_idx >= 0;
								  --ڈھیر_idx)
	{
	    ip = &(buf->b_ml.ml_ڈھیر[ڈھیر_idx]);
	    pb_idx = ip->ip_اشاریہ;
	    إذا ((hp = mf_get(mfp, ip->ip_bnum, 1)) == عدم)
		اذهب theend;
	    pp = (PTR_BL *)(hp->bh_انکڑے);   // must be pointer block
	    إذا (pp->pb_id != PTR_ID)
	    {
		iemsg(e_اشارہ_گر_بلاک_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		اذهب theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    إذا (pp->pb_گنتی < pp->pb_گنتی_max)
	    {
		إذا (pb_idx + 1 < (صحيح)pp->pb_گنتی)
		    mch_memmove(&pp->pb_اشارہ_گر[pb_idx + 2],
				&pp->pb_اشارہ_گر[pb_idx + 1],
			(size_t)(pp->pb_گنتی - pb_idx - 1) * حجم(PTR_EN));
		++pp->pb_گنتی;
		pp->pb_اشارہ_گر[pb_idx].pe_سطر_گنتی = سطر_گنتی_بایاں;
		pp->pb_اشارہ_گر[pb_idx].pe_bnum = bnum_بایاں;
		pp->pb_اشارہ_گر[pb_idx].pe_ورقہ_گنتی = ورقہ_گنتی_بایاں;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_سطر_گنتی = سطر_گنتی_دایاں;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_bnum = bnum_دایاں;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_ورقہ_گنتی = ورقہ_گنتی_دایاں;

		إذا (lnum_بایاں != 0)
		    pp->pb_اشارہ_گر[pb_idx].pe_old_lnum = lnum_بایاں;
		إذا (lnum_دایاں != 0)
		    pp->pb_اشارہ_گر[pb_idx + 1].pe_old_lnum = lnum_دایاں;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_ڈھیر_چوٹی = ڈھیر_idx + 1;	    // truncate stack

		إذا (lineadd)
		{
		    --(buf->b_ml.ml_ڈھیر_چوٹی);
		    // fix line count for rest of blocks in the stack
		    ml_lineadd(buf, lineadd);
							// fix stack itself
		    buf->b_ml.ml_ڈھیر[buf->b_ml.ml_ڈھیر_چوٹی].ip_بلند +=
								      lineadd;
		    ++(buf->b_ml.ml_ڈھیر_چوٹی);
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
		pp_new = (PTR_BL *)(hp_new->bh_انکڑے);

		إذا (hp->bh_bnum != 1)
		    اكسر;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (size_t)ورقہ_ناپ);
		pp->pb_گنتی = 1;
		pp->pb_اشارہ_گر[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_اشارہ_گر[0].pe_سطر_گنتی = buf->b_ml.ml_سطر_گنتی;
		pp->pb_اشارہ_گر[0].pe_old_lnum = 1;
		pp->pb_اشارہ_گر[0].pe_ورقہ_گنتی = 1;
		mf_put(mfp, hp, TRUE, FALSE);   // release block 1
		hp = hp_new;		// new block is to be split
		pp = pp_new;
		CHECK(ڈھیر_idx != 0, _("stack_idx should be 0"));
		ip->ip_اشاریہ = 0;
		++ڈھیر_idx;	// do block 1 again later
	    }
	    /*
	     * move the pointers after the current one to the new block
	     * If there are none, the new entry will be in the new block.
	     */
	    کل_moved = pp->pb_گنتی - pb_idx - 1;
	    إذا (کل_moved)
	    {
		mch_memmove(&pp_new->pb_اشارہ_گر[0],
				&pp->pb_اشارہ_گر[pb_idx + 1],
				(size_t)(کل_moved) * حجم(PTR_EN));
		pp_new->pb_گنتی = کل_moved;
		pp->pb_گنتی -= کل_moved - 1;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_bnum = bnum_دایاں;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_سطر_گنتی = سطر_گنتی_دایاں;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_ورقہ_گنتی = ورقہ_گنتی_دایاں;
		إذا (lnum_دایاں)
		    pp->pb_اشارہ_گر[pb_idx + 1].pe_old_lnum = lnum_دایاں;
	    }
	    وإلا
	    {
		pp_new->pb_گنتی = 1;
		pp_new->pb_اشارہ_گر[0].pe_bnum = bnum_دایاں;
		pp_new->pb_اشارہ_گر[0].pe_سطر_گنتی = سطر_گنتی_دایاں;
		pp_new->pb_اشارہ_گر[0].pe_ورقہ_گنتی = ورقہ_گنتی_دایاں;
		pp_new->pb_اشارہ_گر[0].pe_old_lnum = lnum_دایاں;
	    }
	    pp->pb_اشارہ_گر[pb_idx].pe_bnum = bnum_بایاں;
	    pp->pb_اشارہ_گر[pb_idx].pe_سطر_گنتی = سطر_گنتی_بایاں;
	    pp->pb_اشارہ_گر[pb_idx].pe_ورقہ_گنتی = ورقہ_گنتی_بایاں;
	    إذا (lnum_بایاں)
		pp->pb_اشارہ_گر[pb_idx].pe_old_lnum = lnum_بایاں;
	    lnum_بایاں = 0;
	    lnum_دایاں = 0;

	    /*
	     * recompute line counts
	     */
	    سطر_گنتی_دایاں = 0;
	    لكل (i = 0; i < (صحيح)pp_new->pb_گنتی; ++i)
		سطر_گنتی_دایاں += pp_new->pb_اشارہ_گر[i].pe_سطر_گنتی;
	    سطر_گنتی_بایاں = 0;
	    لكل (i = 0; i < (صحيح)pp->pb_گنتی; ++i)
		سطر_گنتی_بایاں += pp->pb_اشارہ_گر[i].pe_سطر_گنتی;

	    bnum_بایاں = hp->bh_bnum;
	    bnum_دایاں = hp_new->bh_bnum;
	    ورقہ_گنتی_بایاں = 1;
	    ورقہ_گنتی_دایاں = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	إذا (ڈھیر_idx < 0)
	{
	    iemsg(e_updated_too_many_بلاکاں);
	    buf->b_ml.ml_ڈھیر_چوٹی = 0;	// invalidate stack
	}
    }

#جے_تعریف_شدہ FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (لما)متن_len
# وإلا
	    (لما)len
# endif
	    , ML_CHNK_ADDLINE);
#جے_انت

#جے_تعریف_شدہ FEAT_NETBEANS_INTG
    إذا (!(flags & ML_APPEND_NEW) && netbeans_سرگرم())
    {
	صحيح سطر_len = (صحيح)STRLEN(سطر);
	إذا (سطر_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, سطر, سطر_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)سطر_len, (char_u *)"\n", 1);
    }
#جے_انت
#جے_تعریف_شدہ FEAT_کم_CHANNEL
    إذا (!(flags & ML_APPEND_NEW) && buf->b_write_منزل_channel)
	channel_write_new_سطریں(buf);
#جے_انت
    ret = OK;

theend:
#جے_تعریف_شدہ FEAT_PROP_POPUP
    vim_free(tofree);
#جے_انت
    أرجع ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    ساكن صحيح
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*سطر,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    صحيح		flags)		// ML_APPEND_ flags
{
    إذا (lnum > buf->b_ml.ml_سطر_گنتی)
	أرجع FAIL;  // lnum out of range

    إذا (buf->b_ml.ml_سطر_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_سطر(buf);

#جے_تعریف_شدہ FEAT_EVAL
    إذا (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	إذا (buf->b_ml.ml_سطر_lnum != 0)
	    ml_flush_سطر(buf);
    }
#جے_انت

    أرجع ml_append_int(buf, lnum, سطر, len, flags);
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
    char_u	*سطر,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    صحيح		newfile)	// flag, see above
{
    أرجع ml_append_flags(lnum, سطر, len, newfile ? ML_APPEND_NEW : 0);
}

    صحيح
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*سطر,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    صحيح		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    إذا (curbuf->b_ml.ml_mfp == عدم && open_بفر(FALSE, عدم, 0) == FAIL)
	أرجع FAIL;
    أرجع ml_append_flush(curbuf, lnum, سطر, len, flags);
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
    char_u	*سطر,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    صحيح		newfile)	// flag, see above
{
    إذا (buf->b_ml.ml_mfp == عدم)
	أرجع FAIL;
    أرجع ml_append_flush(buf, lnum, سطر, len, newfile ? ML_APPEND_NEW : 0);
}
#جے_انت

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
ml_replace(linenr_T lnum, char_u *سطر, صحيح copy)
{
    colnr_T len = -1;

    إذا (سطر != عدم)
	len = (colnr_T)STRLEN(سطر);
    أرجع ml_replace_len(lnum, سطر, len, FALSE, copy);
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
	char_u	    *سطر_arg,
	colnr_T	    len_arg,
	صحيح	    has_props,
	صحيح	    copy)
{
    char_u *سطر = سطر_arg;
    colnr_T len = len_arg;

    إذا (سطر == عدم)		// just checking...
	أرجع FAIL;

    // When starting up, we might still need to create the memfile
    إذا (curbuf->b_ml.ml_mfp == عدم && open_بفر(FALSE, عدم, 0) == FAIL)
	أرجع FAIL;

    إذا (!has_props)
	++len;  // include the NUL after the text
    إذا (copy)
    {
	// copy the line to allocated memory
#جے_تعریف_شدہ FEAT_PROP_POPUP
	إذا (has_props)
	    سطر = vim_memsave(سطر, len);
	وإلا
#جے_انت
	    سطر = vim_strnsave(سطر, len - 1);
	إذا (سطر == عدم)
	    أرجع FAIL;
    }

#جے_تعریف_شدہ FEAT_NETBEANS_INTG
    إذا (netbeans_سرگرم())
    {
	netbeans_removed(curbuf, lnum, 0, (لما)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, سطر, (صحيح)STRLEN(سطر));
    }
#جے_انت
    إذا (curbuf->b_ml.ml_سطر_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_سطر(curbuf);

#جے_تعریف_شدہ FEAT_PROP_POPUP
	إذا (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#جے_انت
    }

#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (curbuf->b_has_textprop && !has_props)
    {
	size_t	oldtextlen = STRLEN(curbuf->b_ml.ml_سطر_ptr) + 1;

	إذا (oldtextlen < (size_t)curbuf->b_ml.ml_سطر_len)
	{
	    char_u *newline;
	    size_t textproplen = curbuf->b_ml.ml_سطر_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    إذا (newline != عدم)
	    {
		mch_memmove(newline, سطر, len);
		mch_memmove(newline + len, curbuf->b_ml.ml_سطر_ptr
						    + oldtextlen, textproplen);
		vim_free(سطر);
		سطر = newline;
		len += (colnr_T)textproplen;
	    }
	}
    }
#جے_انت

    إذا (curbuf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_سطر_ptr);	// free allocated line

    curbuf->b_ml.ml_سطر_ptr = سطر;
    curbuf->b_ml.ml_سطر_len = len;
    curbuf->b_ml.ml_سطر_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_سطر_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_سطر_DIRTY) & ~ML_خالی_تھاواں;

    أرجع OK;
}

#جے_تعریف_شدہ FEAT_PROP_POPUP
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
    صحيح		did_get_سطر = FALSE;
    صحيح		پورا_del;
    صحيح		پورا_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    انکڑے_BL	*dp;
    صحيح		idx;
    صحيح		سطر_شروع;
    لما	سطر_ناپ;
    صحيح		this_props_len = 0;
    char_u	*متن;
    size_t	textlen;
    صحيح		لبھیا;

    لكل (پورا_del = 0; پورا_del < del_props_len; پورا_del += حجم(textprop_T))
    {
	mch_memmove(&prop_del, del_props + پورا_del, حجم(textprop_T));
	إذا ((above && (prop_del.tp_flags & TP_FLAG_CONT_PREV)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_اگلا))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_اگلا)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_PREV)))
	{
	    إذا (!did_get_سطر)
	    {
		did_get_سطر = TRUE;
		إذا ((hp = ml_لبھو_سطر(buf, lnum, ML_لبھو)) == عدم)
		    أرجع;

		dp = (انکڑے_BL *)(hp->bh_انکڑے);
		idx = lnum - buf->b_ml.ml_locked_کم;
		سطر_شروع = ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب);
		إذا (idx == 0)		// first line in block, text at the end
		    سطر_ناپ = dp->db_txt_سرا - سطر_شروع;
		وإلا
		    سطر_ناپ = ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب)
								  - سطر_شروع;
		متن = (char_u *)dp + سطر_شروع;
		textlen = STRLEN(متن) + 1;
		إذا ((لما)textlen >= سطر_ناپ)
		{
		    // No properties on this line.
		    إذا (above)
			اندرونی_error("no text property above deleted line");
		    وإلا
			اندرونی_error("no text property below deleted line");
		    أرجع;
		}
		إذا ((لما)textlen + (لما)PROP_گنتی_ناپ > سطر_ناپ)
		{
		    اندرونی_error("text property data too short");
		    أرجع;
		}

		uint16_t pc;

		mch_memmove(&pc, متن + textlen, PROP_گنتی_ناپ);
		إذا (!متن_prop_گنتی_valid(pc, (size_t)(سطر_ناپ - (لما)textlen)))
		{
		    اندرونی_error("text property count too large");
		    أرجع;
		}
		this_props_len = pc * (صحيح)حجم(textprop_T);
	    }

	    لبھیا = FALSE;
	    {
		char_u *props_شروع = متن + textlen + PROP_گنتی_ناپ;

		لكل (پورا_this = 0; پورا_this < this_props_len;
					       پورا_this += حجم(textprop_T))
		{
		    صحيح		flag = above ? TP_FLAG_CONT_اگلا
							   : TP_FLAG_CONT_PREV;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_شروع + پورا_this,
							   حجم(textprop_T));
		    إذا ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			لبھیا = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_شروع + پورا_this, &prop_this,
							   حجم(textprop_T));
			اكسر;
		    }
		}
	    }
	    إذا (!لبھیا)
	    {
		إذا (above)
		    اندرونی_error("text property above deleted line not found");
		وإلا
		    اندرونی_error("text property below deleted line not found");
	    }

	    buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
	}
    }
}
#جے_انت

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
    انکڑے_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    صحيح		گنتی;	    // number of entries in block
    صحيح		idx;
    صحيح		ڈھیر_idx;
    صحيح		متن_شروع;
    صحيح		سطر_شروع;
    لما	سطر_ناپ;
    صحيح		i;
    صحيح		ret = FAIL;
#جے_تعریف_شدہ FEAT_PROP_POPUP
    char_u	*textprop_save = عدم;
    لما	textprop_len = 0;
#جے_انت

    إذا (سبھ_توں_نیواں_marked && سبھ_توں_نیواں_marked > lnum)
	سبھ_توں_نیواں_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    إذا (buf->b_ml.ml_سطر_گنتی == 1)	    // file becomes empty
    {
	إذا ((flags & ML_DEL_سنیہا)
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_سطریں
#جے_انت
	   )
	    سیٹ_keep_پیغام((char_u *)_(no_سطریں_پیغام), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_خالی_تھاواں;

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

    إذا ((hp = ml_لبھو_سطر(buf, lnum, ML_DELETE)) == عدم)
	أرجع FAIL;

    dp = (انکڑے_BL *)(hp->bh_انکڑے);
    // compute line count before the delete
    گنتی = (لما)(buf->b_ml.ml_locked_بلند)
					- (لما)(buf->b_ml.ml_locked_کم) + 2;
    idx = lnum - buf->b_ml.ml_locked_کم;

    --buf->b_ml.ml_سطر_گنتی;

    سطر_شروع = ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب);
    إذا (idx == 0)		// first line in block, text at the end
	سطر_ناپ = dp->db_txt_سرا - سطر_شروع;
    وإلا
	سطر_ناپ = ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب) - سطر_شروع;

#جے_تعریف_شدہ FEAT_NETBEANS_INTG
    إذا (netbeans_سرگرم())
	netbeans_removed(buf, lnum, 0, سطر_ناپ);
#جے_انت
#جے_تعریف_شدہ FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    إذا (buf->b_has_textprop)
    {
	size_t	textlen = STRLEN((char_u *)dp + سطر_شروع) + 1;

	textprop_len = سطر_ناپ - (لما)textlen;
	إذا (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + سطر_شروع + textlen,
								 textprop_len);
    }
#جے_انت

/*
 * special case: If there is only one line in the data block it becomes empty.
 * Then we have to remove the entry, pointing to this data block, from the
 * pointer block. If this pointer block also becomes empty, we go up another
 * block, and so on, up to the root if necessary.
 * The line counts in the pointer blocks have already been adjusted by
 * ml_find_line().
 */
    إذا (گنتی == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = عدم;

	لكل (ڈھیر_idx = buf->b_ml.ml_ڈھیر_چوٹی - 1; ڈھیر_idx >= 0;
								  --ڈھیر_idx)
	{
	    buf->b_ml.ml_ڈھیر_چوٹی = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_ڈھیر[ڈھیر_idx]);
	    idx = ip->ip_اشاریہ;
	    إذا ((hp = mf_get(mfp, ip->ip_bnum, 1)) == عدم)
		اذهب theend;
	    pp = (PTR_BL *)(hp->bh_انکڑے);   // must be pointer block
	    إذا (pp->pb_id != PTR_ID)
	    {
		iemsg(e_اشارہ_گر_بلاک_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		اذهب theend;
	    }
	    گنتی = --(pp->pb_گنتی);
	    إذا (گنتی == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    وإلا
	    {
		إذا (گنتی != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_اشارہ_گر[idx], &pp->pb_اشارہ_گر[idx + 1],
				      (size_t)(گنتی - idx) * حجم(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_ڈھیر_چوٹی = ڈھیر_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		إذا (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_ڈھیر[buf->b_ml.ml_ڈھیر_چوٹی].ip_بلند +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_ڈھیر_چوٹی);

		اكسر;
	    }
	}
	CHECK(ڈھیر_idx < 0, _("deleted block 1?"));
    }
    وإلا
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	متن_شروع = dp->db_txt_شروع;
	mch_memmove((حرف *)dp + متن_شروع + سطر_ناپ,
		  (حرف *)dp + متن_شروع, (size_t)(سطر_شروع - متن_شروع));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	لكل (i = idx; i < گنتی - 1; ++i)
	    dp->db_اشاریہ[i] = dp->db_اشاریہ[i + 1] + سطر_ناپ;

	dp->db_free += سطر_ناپ + اشاریہ_ناپ;
	dp->db_txt_شروع += سطر_ناپ;
	--(dp->db_سطر_گنتی);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#جے_تعریف_شدہ FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, سطر_ناپ
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#جے_انت
    ret = OK;

theend:
#جے_تعریف_شدہ FEAT_PROP_POPUP
    إذا (textprop_save != عدم)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_انکڑے;
	صحيح	    props_بائٹاں;

	mch_memmove(&pc, textprop_save, PROP_گنتی_ناپ);
	props_انکڑے = textprop_save + PROP_گنتی_ناپ;
	props_بائٹاں = pc * (صحيح)حجم(textprop_T);
	إذا (!متن_prop_گنتی_valid(pc, (size_t)textprop_len))
	    props_بائٹاں = 0;

	// Adjust text properties in the line above and below.
	إذا (lnum > 1)
	    adjust_متن_props_for_delete(buf, lnum - 1,
					     props_انکڑے, props_بائٹاں, TRUE);
	إذا (lnum <= buf->b_ml.ml_سطر_گنتی)
	    adjust_متن_props_for_delete(buf, lnum,
					    props_انکڑے, props_بائٹاں, FALSE);
    }
    vim_free(textprop_save);
#جے_انت
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
    ml_flush_سطر(curbuf);
    إذا (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_گنتی)
	أرجع FAIL;

#جے_تعریف_شدہ FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#جے_انت

    أرجع ml_delete_int(curbuf, lnum, flags);
}

/*
 * set the DB_MARKED flag for line 'lnum'
 */
    فراغ
ml_setmarked(linenr_T lnum)
{
    bhdr_T    *hp;
    انکڑے_BL *dp;
				    // invalid line number
    إذا (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_گنتی
					       || curbuf->b_ml.ml_mfp == عدم)
	أرجع;			    // give error message?

    إذا (سبھ_توں_نیواں_marked == 0 || سبھ_توں_نیواں_marked > lnum)
	سبھ_توں_نیواں_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    إذا ((hp = ml_لبھو_سطر(curbuf, lnum, ML_لبھو)) == عدم)
	أرجع;		    // give error message?

    dp = (انکڑے_BL *)(hp->bh_انکڑے);
    dp->db_اشاریہ[lnum - curbuf->b_ml.ml_locked_کم] |= DB_MARKED;
    curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
}

/*
 * find the first line with its DB_MARKED flag set
 */
    linenr_T
ml_firstmarked(فراغ)
{
    bhdr_T	*hp;
    انکڑے_BL	*dp;
    linenr_T	lnum;
    صحيح		i;

    إذا (curbuf->b_ml.ml_mfp == عدم)
	أرجع (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    لكل (lnum = سبھ_توں_نیواں_marked; lnum <= curbuf->b_ml.ml_سطر_گنتی; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	إذا ((hp = ml_لبھو_سطر(curbuf, lnum, ML_لبھو)) == عدم)
	    أرجع (linenr_T)0;		    // give error message?

	dp = (انکڑے_BL *)(hp->bh_انکڑے);

	لكل (i = lnum - curbuf->b_ml.ml_locked_کم;
			    lnum <= curbuf->b_ml.ml_locked_بلند; ++i, ++lnum)
	    إذا ((dp->db_اشاریہ[i]) & DB_MARKED)
	    {
		(dp->db_اشاریہ[i]) &= DB_اشاریہ_نقاب;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		سبھ_توں_نیواں_marked = lnum + 1;
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
    انکڑے_BL	*dp;
    linenr_T	lnum;
    صحيح		i;

    إذا (curbuf->b_ml.ml_mfp == عدم)	    // nothing to do
	أرجع;

    /*
     * The search starts with line lowest_marked.
     */
    لكل (lnum = سبھ_توں_نیواں_marked; lnum <= curbuf->b_ml.ml_سطر_گنتی; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	إذا ((hp = ml_لبھو_سطر(curbuf, lnum, ML_لبھو)) == عدم)
	    أرجع;		// give error message?

	dp = (انکڑے_BL *)(hp->bh_انکڑے);

	لكل (i = lnum - curbuf->b_ml.ml_locked_کم;
			    lnum <= curbuf->b_ml.ml_locked_بلند; ++i, ++lnum)
	    إذا ((dp->db_اشاریہ[i]) & DB_MARKED)
	    {
		(dp->db_اشاریہ[i]) &= DB_اشاریہ_نقاب;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	    }
    }

    سبھ_توں_نیواں_marked = 0;
}

/*
 * flush ml_line if necessary
 */
    ساكن فراغ
ml_flush_سطر(buf_T *buf)
{
    bhdr_T	*hp;
    انکڑے_BL	*dp;
    linenr_T	lnum;
    char_u	*new_سطر;
    char_u	*old_سطر;
    colnr_T	new_len;
    صحيح		old_len;
    صحيح		extra;
    صحيح		idx;
    صحيح		شروع;
    صحيح		گنتی;
    صحيح		i;
    ساكن صحيح  entered = FALSE;

    إذا (buf->b_ml.ml_سطر_lnum == 0 || buf->b_ml.ml_mfp == عدم)
	أرجع;		// nothing to do

    إذا (buf->b_ml.ml_flags & ML_سطر_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	إذا (entered)
	    أرجع;
	entered = TRUE;

	lnum = buf->b_ml.ml_سطر_lnum;
	new_سطر = buf->b_ml.ml_سطر_ptr;

	hp = ml_لبھو_سطر(buf, lnum, ML_لبھو);
	إذا (hp == عدم)
	    siemsg(e_cannot_لبھو_سطر_nr, lnum);
	وإلا
	{
	    dp = (انکڑے_BL *)(hp->bh_انکڑے);
	    idx = lnum - buf->b_ml.ml_locked_کم;
	    شروع = ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب);
	    old_سطر = (char_u *)dp + شروع;
	    إذا (idx == 0)	// line is last in block
		old_len = dp->db_txt_سرا - شروع;
	    وإلا		// text of previous line follows
		old_len = (dp->db_اشاریہ[idx - 1] & DB_اشاریہ_نقاب) - شروع;
	    new_len = buf->b_ml.ml_سطر_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    إذا ((صحيح)dp->db_free >= extra)
	    {
#إذا defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		صحيح old_prop_len = 0;
		إذا (buf->b_has_textprop)
		    old_prop_len = old_len - (صحيح)STRLEN(old_سطر) - 1;
#جے_انت
		// if the length changes and there are following lines
		گنتی = buf->b_ml.ml_locked_بلند - buf->b_ml.ml_locked_کم + 1;
		إذا (extra != 0 && idx < گنتی - 1)
		{
		    // move text of following lines
		    mch_memmove((حرف *)dp + dp->db_txt_شروع - extra,
				(حرف *)dp + dp->db_txt_شروع,
				(size_t)(شروع - dp->db_txt_شروع));

		    // adjust pointers of this and following lines
		    لكل (i = idx + 1; i < گنتی; ++i)
			dp->db_اشاریہ[i] -= extra;
		}
		dp->db_اشاریہ[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_شروع -= extra;

		// copy new line into the data block
		mch_memmove(old_سطر - extra, new_سطر, (size_t)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#إذا defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		إذا (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (صحيح)STRLEN(new_سطر) - 1;
		}
		إذا (extra != 0)
		    ml_updatechunk(buf, lnum, (لما)extra, ML_CHNK_UPDLINE);
#جے_انت
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
		(فراغ)ml_append_int(buf, lnum, new_سطر, new_len,
			 ((dp->db_اشاریہ[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#جے_تعریف_شدہ FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#جے_انت
			 );
		(فراغ)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_سطر);

	entered = FALSE;
    }
    وإلا إذا (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_سطر_ptr);

    buf->b_ml.ml_flags &= ~(ML_سطر_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_سطر_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    ساكن bhdr_T *
ml_new_انکڑے(memfile_T *mfp, صحيح negative, صحيح ورقہ_گنتی)
{
    bhdr_T	*hp;
    انکڑے_BL	*dp;

    إذا ((hp = mf_new(mfp, negative, ورقہ_گنتی)) == عدم)
	أرجع عدم;

    dp = (انکڑے_BL *)(hp->bh_انکڑے);
    dp->db_id = انکڑے_ID;
    dp->db_txt_شروع = dp->db_txt_سرا = ورقہ_گنتی * mfp->mf_ورقہ_ناپ;
    dp->db_free = dp->db_txt_شروع - سرخی_ناپ;
    dp->db_سطر_گنتی = 0;

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

    pp = (PTR_BL *)(hp->bh_انکڑے);
    pp->pb_id = PTR_ID;
    pp->pb_گنتی = 0;
    pp->pb_گنتی_max = PB_گنتی_MAX(mfp);

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
ml_لبھو_سطر(buf_T *buf, linenr_T lnum, صحيح action)
{
    انکڑے_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    صحيح		dirty;
    linenr_T	کم_2, بلند;
    صحيح		چوٹی;
    صحيح		ورقہ_گنتی;
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
		&& buf->b_ml.ml_locked_کم <= lnum
		&& buf->b_ml.ml_locked_بلند >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    إذا (action == ML_داخل_کرو)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_بلند);
	    }
	    وإلا إذا (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_بلند);
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
    ورقہ_گنتی = 1;
    کم_2 = 1;
    بلند = buf->b_ml.ml_سطر_گنتی;

    إذا (action == ML_لبھو)	// first try stack entries
    {
	لكل (چوٹی = buf->b_ml.ml_ڈھیر_چوٹی - 1; چوٹی >= 0; --چوٹی)
	{
	    ip = &(buf->b_ml.ml_ڈھیر[چوٹی]);
	    إذا (ip->ip_کم <= lnum && ip->ip_بلند >= lnum)
	    {
		bnum = ip->ip_bnum;
		کم_2 = ip->ip_کم;
		بلند = ip->ip_بلند;
		buf->b_ml.ml_ڈھیر_چوٹی = چوٹی;	// truncate stack at prev entry
		اكسر;
	    }
	}
	إذا (چوٹی < 0)
	    buf->b_ml.ml_ڈھیر_چوٹی = 0;		// not found, start at the root
    }
    وإلا	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_ڈھیر_چوٹی = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    لكل (;;)
    {
	إذا ((hp = mf_get(mfp, bnum, ورقہ_گنتی)) == عدم)
	    اذهب error_noblock;

	/*
	 * update high for insert/delete
	 */
	إذا (action == ML_داخل_کرو)
	    ++بلند;
	وإلا إذا (action == ML_DELETE)
	    --بلند;

	dp = (انکڑے_BL *)(hp->bh_انکڑے);
	إذا (dp->db_id == انکڑے_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_کم = کم_2;
	    buf->b_ml.ml_locked_بلند = بلند;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    أرجع hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	إذا (pp->pb_id != PTR_ID)
	{
	    iemsg(e_اشارہ_گر_بلاک_id_wrong);
	    اذهب error_بلاک;
	}

	إذا ((چوٹی = ml_جوڑو_ڈھیر(buf)) < 0)	// add new entry to stack
	    اذهب error_بلاک;
	ip = &(buf->b_ml.ml_ڈھیر[چوٹی]);
	ip->ip_bnum = bnum;
	ip->ip_کم = کم_2;
	ip->ip_بلند = بلند;
	ip->ip_اشاریہ = -1;		// index not known yet

	dirty = FALSE;
	لكل (idx = 0; idx < (صحيح)pp->pb_گنتی; ++idx)
	{
	    t = pp->pb_اشارہ_گر[idx].pe_سطر_گنتی;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    إذا ((کم_2 += t) > lnum)
	    {
		ip->ip_اشاریہ = idx;
		bnum = pp->pb_اشارہ_گر[idx].pe_bnum;
		ورقہ_گنتی = pp->pb_اشارہ_گر[idx].pe_ورقہ_گنتی;
		بلند = کم_2 - 1;
		کم_2 -= t;

		/*
		 * a negative block number may have been changed
		 */
		إذا (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    إذا (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_اشارہ_گر[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		اكسر;
	    }
	}
	إذا (idx >= (صحيح)pp->pb_گنتی)	    // past the end: something wrong!
	{
	    إذا (lnum > buf->b_ml.ml_سطر_گنتی)
		siemsg(e_سطر_نمبر_باہرلا_دا_range_nr_past_the_سرا,
					      lnum - buf->b_ml.ml_سطر_گنتی);

	    وإلا
		siemsg(e_سطر_گنتی_wrong_وچ_بلاک_nr, bnum);
	    اذهب error_بلاک;
	}
	إذا (action == ML_DELETE)
	{
	    pp->pb_اشارہ_گر[idx].pe_سطر_گنتی--;
	    dirty = TRUE;
	}
	وإلا إذا (action == ML_داخل_کرو)
	{
	    pp->pb_اشارہ_گر[idx].pe_سطر_گنتی++;
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
    وإلا إذا (action == ML_داخل_کرو)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_ڈھیر_چوٹی = 0;
    أرجع عدم;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    ساكن صحيح
ml_جوڑو_ڈھیر(buf_T *buf)
{
    صحيح		چوٹی;
    infoptr_T	*newstack;

    چوٹی = buf->b_ml.ml_ڈھیر_چوٹی;

    // may have to increase the stack size
    إذا (چوٹی == buf->b_ml.ml_ڈھیر_ناپ)
    {
	CHECK(چوٹی > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_ڈھیر_ناپ + ڈھیر_INCR);
	إذا (newstack == عدم)
	    أرجع -1;
	إذا (چوٹی > 0)
	    mch_memmove(newstack, buf->b_ml.ml_ڈھیر,
					     (size_t)چوٹی * حجم(infoptr_T));
	vim_free(buf->b_ml.ml_ڈھیر);
	buf->b_ml.ml_ڈھیر = newstack;
	buf->b_ml.ml_ڈھیر_ناپ += ڈھیر_INCR;
    }

    buf->b_ml.ml_ڈھیر_چوٹی++;
    أرجع چوٹی;
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
ml_lineadd(buf_T *buf, صحيح گنتی)
{
    صحيح		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    لكل (idx = buf->b_ml.ml_ڈھیر_چوٹی - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_ڈھیر[idx]);
	إذا ((hp = mf_get(mfp, ip->ip_bnum, 1)) == عدم)
	    اكسر;
	pp = (PTR_BL *)(hp->bh_انکڑے);	// must be pointer block
	إذا (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_اشارہ_گر_بلاک_id_wrong_two);
	    اكسر;
	}
	pp->pb_اشارہ_گر[ip->ip_اشاریہ].pe_سطر_گنتی += گنتی;
	ip->ip_بلند += گنتی;
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
    صحيح		ڈونگھائی = 0;

    إذا (fname == عدم)
	أرجع FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    لكل (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	إذا (++ڈونگھائی == 100)
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
		إذا (ڈونگھائی == 1)
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
	إذا (mch_is_بھریا_Name(buf))
	    STRCPY(tmp, buf);
	وإلا
	{
	    char_u *پوچھ;

	    پوچھ = gettail(tmp);
	    إذا (STRLEN(پوچھ) + STRLEN(buf) >= MAXPATHL)
		أرجع FAIL;
	    STRCPY(پوچھ, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    أرجع vim_بھریا_Name(tmp, buf, MAXPATHL, TRUE);
}
#جے_انت

/*
 * Make swap file name out of the file name and a directory name.
 * Returns pointer to allocated memory or NULL.
 */
    char_u *
makeswapname(
    char_u	*fname,
    char_u	*ffname بن_ورتیا,
    buf_T	*buf,
    char_u	*dir_name)
{
    char_u	*r, *s;
    char_u	*fname_res = fname;
#جے_تعریف_شدہ HAVE_READLINK
    char_u	fname_buf[MAXPATHL];

    // Expand symlink in the file name, so that we put the swap file with the
    // actual file instead of with the symlink.
    إذا (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#جے_انت

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
#جے_انت

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#إذا defined(VMS)
	    "_swp",
#وإلا
	    ".swp",
#جے_انت
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    إذا (r == عدم)	    // out of memory
	أرجع عدم;

    s = get_file_وچ_dir(r, dir_name);
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
get_file_وچ_dir(
    char_u  *fname,
    char_u  *dname)	// don't use "dirname", it is a global for Alpha
{
    string_T	پوچھ;
    string_T	retval;

    پوچھ.string = gettail(fname);
    پوچھ.لمائی = STRLEN(پوچھ.string);

    إذا (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (size_t)(پوچھ.string - fname) + پوچھ.لمائی);
    وإلا
    {
	size_t	dname_len = STRLEN(dname);

	إذا (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    إذا (پوچھ.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    پوچھ.string, پوچھ.لمائی, TRUE, &retval);
	    وإلا
	    {
		صحيح	    save_char;
		string_T    tmp;

		save_char = *پوچھ.string;
		*پوچھ.string = NUL;
		concat_fnames(fname, (size_t)(پوچھ.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*پوچھ.string = save_char;
		إذا (tmp.string == عدم)	    // out of memory
		    retval.string = عدم;
		وإلا
		{
		    concat_fnames(tmp.string, tmp.لمائی,
			پوچھ.string, پوچھ.لمائی, TRUE, &retval);
		    vim_free(tmp.string);
		}
	    }
	}
	وإلا
	    concat_fnames(dname, dname_len, پوچھ.string, پوچھ.لمائی,
		TRUE, &retval);
    }

#جے_تعریف_شدہ MSWIN
    إذا (retval.string != عدم)
    {
	char_u	*t;

	لكل (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    إذا (*t == ':')
		*t = '%';
    }
#جے_انت

    أرجع retval.string;
}

/*
 * Print the ATTENTION message: info about an existing swap file.
 */
    ساكن فراغ
attention_سنیہا(
    buf_T   *buf,	// buffer being edited
    char_u  *fname)	// swap file name
{
    stat_T	st;
    time_t	وٹاندرا_mtime;

    ++no_wait_return;
    (فراغ)emsg(_(e_attention));
    پیغام_puts(_("\nFound a swap file by the name \""));
    پیغام_home_replace(fname);
    پیغام_puts("\"\n");
    وٹاندرا_mtime = swapfile_info(fname);
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
	إذا (وٹاندرا_mtime != 0 && st.st_mtime > وٹاندرا_mtime)
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
    cmdline_صف = پیغام_صف;
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
    سیٹ_vim_var_string(VV_SWAPNAME, fname, -1);
    سیٹ_vim_var_string(VV_SWAPCHOICE, عدم, -1);

    // Trigger SwapExists autocommands with <afile> set to the file being
    // edited.  Disallow changing directory here.
    ++allbuf_جندرا;
    apply_autocmds(EVENT_SWAPEXISTS, buf->b_fname, عدم, FALSE, عدم);
    --allbuf_جندرا;

    سیٹ_vim_var_string(VV_SWAPNAME, عدم, -1);

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
#جے_انت

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
#جے_تعریف_شدہ AMIGA
    BPTR	fh;
#جے_انت
    صحيح		r;
    char_u	*buf_fname = buf->b_fname;

#إذا !defined(UNIX)
# define CREATE_DUMMY_FILE
    FILE	*dummyfd = عدم;

# ifdef MSWIN
    إذا (buf_fname != عدم && !mch_is_بھریا_Name(buf_fname)
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
#جے_انت

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
	    char_u	    *پوچھ;
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
	    پوچھ = gettail(buf_fname);
	    إذا (       vim_strchr(پوچھ, '.') != عدم
		    || STRLEN(پوچھ) > (size_t)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		إذا (fname2 != عدم)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    إذا (vim_strchr(پوچھ, '.') != عدم)
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
#جے_انت
	/*
	 * check if the swapfile already exists
	 */
	إذا (mch_getperm(fname) < 0)	// it does not exist
	{
#جے_تعریف_شدہ HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    إذا (mch_lstat((حرف *)fname, &sb) < 0)
#وإلا
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (لما)منوال_NEWFILE);
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
	    إذا (IoErr() != ERROR_OBJECT_وچ_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#جے_انت
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
		صحيح		فرق = FALSE;

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
#جے_تعریف_شدہ CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				إذا (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_منزل_long(b0.b0_ino)))
#جے_انت
				    فرق = TRUE;
			    }
			}
			وإلا
			{
			    /*
			     * The name in the swap file may be
			     * "~user/path/file".  Expand it first.
			     */
			    expand_env(b0.b0_fname, NameBuff, MAXPATHL);
#جے_تعریف_شدہ CHECK_INODE
			    إذا (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_منزل_long(b0.b0_ino)))
				فرق = TRUE;
#وإلا
			    إذا (fnamecmp(NameBuff, buf->b_ffname) != 0)
				فرق = TRUE;
#جے_انت
			}
		    }
		    close(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		إذا (فرق == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == عدم)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#جے_تعریف_شدہ CREATE_DUMMY_FILE
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
#جے_انت

#جے_تعریف_شدہ HAVE_PROCESS_STILL_RUNNING
		    process_still_running = FALSE;
#جے_انت
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
			    && وٹاندرا_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#جے_انت

		    إذا (choice == SEA_CHOICE_NONE
					 && وٹاندرا_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    إذا (choice == SEA_CHOICE_NONE)
		    {
#جے_تعریف_شدہ FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			إذا (gui.starting && !gui.وچ_use)
			    gui_شروع(عدم);
#جے_انت
			// Show info about the existing swap file.
			attention_سنیہا(buf, fname);

			// We don't want a 'q' typed at the more-prompt
			// interrupt loading a file.
			got_int = FALSE;

			// If vimrc has "simalt ~x" we don't want it to
			// interfere with the prompt here.
			flush_buffers(FLUSH_TYPEAHEAD);
		    }

#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    إذا (وٹاندرا_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    اگیتر = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			size_t	    سنیہا_ناپ;
			string_T    سنیہا;
			char_u	    *tofree;
			صحيح	    dialog_نتیجہ;

			اگیتر.لمائی = STRLEN(اگیتر.string);
			suffix.لمائی = STRLEN(suffix.string);
			سنیہا_ناپ = اگیتر.لمائی
				+ STRLEN(fname)
				+ suffix.لمائی + 5;
			سنیہا.string = tofree = alloc(سنیہا_ناپ);
			إذا (سنیہا.string != عدم)
			{
			    STRCPY(سنیہا.string, اگیتر.string);
			    سنیہا.لمائی = اگیتر.لمائی;
			    سنیہا.لمائی += home_replace(عدم, fname,
				سنیہا.string + سنیہا.لمائی, (صحيح)(سنیہا_ناپ - سنیہا.لمائی), TRUE);
			    STRCPY(سنیہا.string + سنیہا.لمائی, suffix.string);
			}
			وإلا
			    سنیہا.string = (char_u *)_("Swap file already exists!");
			dialog_نتیجہ = do_dialog(VIM_WARNING,
				    (char_u *)_("VIM - ATTENTION"),
				    سنیہا.string,
# ifdef HAVE_PROCESS_STILL_RUNNING
				    process_still_running
					? (char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Quit\n&Abort") :
# endif
					(char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Delete it\n&Quit\n&Abort"), 1, عدم, FALSE);

# ifdef HAVE_PROCESS_STILL_RUNNING
			إذا (process_still_running && dialog_نتیجہ >= 4)
			    // compensate for missing "Delete it" button
			    dialog_نتیجہ++;
# endif
			choice = dialog_نتیجہ;
			vim_free(tofree);

			// pretend screen didn't scroll, need redraw anyway
			پیغام_scrolled = 0;
			redraw_all_later(UPD_NOT_VALID);
		    }
#جے_انت

		    بدل (choice)
		    {
			حالة SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    اكسر;
			حالة SEA_CHOICE_EDIT:
			    اكسر;
			حالة SEA_CHOICE_RECOVER:
			    وٹاندرا_exists_action = SEA_RECOVER;
			    اكسر;
			حالة SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    اكسر;
			حالة SEA_CHOICE_QUIT:
			    وٹاندرا_exists_action = SEA_QUIT;
			    اكسر;
			حالة SEA_CHOICE_ABORT:
			    وٹاندرا_exists_action = SEA_QUIT;
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

#جے_تعریف_شدہ CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    إذا (did_use_dummy)
			dummyfd = mch_fopen((حرف *)buf_fname, "w");
#جے_انت
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
		emsg(_(e_too_many_وٹاندرا_files_لبھیا));
		VIM_CLEAR(fname);
		اكسر;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#جے_تعریف_شدہ CREATE_DUMMY_FILE
    إذا (dummyfd != عدم)	// file has been created temporarily
    {
	fclose(dummyfd);
	mch_remove(buf_fname);
    }
#جے_انت
#جے_تعریف_شدہ MSWIN
    إذا (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#جے_انت
    أرجع fname;
}

    ساكن صحيح
b0_magic_wrong(ZERO_BL *b0p)
{
    أرجع (b0p->b0_magic_long != (لما)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (صحيح)B0_MAGIC_INT
	    || b0p->b0_magic_short != (چھوٹا)B0_MAGIC_SHORT
	    || b0p->b0_magic_char != B0_MAGIC_CHAR);
}

#جے_تعریف_شدہ CHECK_INODE
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
    لما	ino_block0)
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
    retval_c = vim_بھریا_Name(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_بھریا_Name(fname_s, buf_s, MAXPATHL, TRUE);
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
#جے_انت // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    ساكن فراغ
long_منزل_char(لما n, char_u *s)
{
    s[0] = (char_u)(n & 0xff);
    n = (بنانشانی)n >> 8;
    s[1] = (char_u)(n & 0xff);
    n = (بنانشانی)n >> 8;
    s[2] = (char_u)(n & 0xff);
    n = (بنانشانی)n >> 8;
    s[3] = (char_u)(n & 0xff);
}

    ساكن لما
char_منزل_long(char_u *s)
{
    لما    retval;

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
    لكل (hp = buf->b_ml.ml_mfp->mf_ورتیا_آخری; hp != عدم; hp = hp->bh_prev)
    {
	إذا (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_انکڑے);
	    b0p->b0_dirty = buf->b_بدلیا ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_نقاب)
						  | (get_fileformat(buf) + 1);
	    جوڑو_b0_fenc(b0p, buf);
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
ml_encrypt_انکڑے(
    memfile_T	*mfp,
    char_u	*انکڑے,
    off_T	وتھ,
    بنانشانی	ناپ)
{
    انکڑے_BL	*dp = (انکڑے_BL *)انکڑے;
    char_u	*سر_سرا;
    char_u	*متن_شروع;
    char_u	*new_انکڑے;
    صحيح		متن_len;
    cryptstate_T *state;

    إذا (dp->db_id != انکڑے_ID)
	أرجع انکڑے;

    state = ml_crypt_prepare(mfp, وتھ, FALSE);
    إذا (state == عدم)
	أرجع انکڑے;

    new_انکڑے = alloc(ناپ);
    إذا (new_انکڑے == عدم)
    {
	crypt_free_state(state);
	أرجع عدم;
    }
    سر_سرا = (char_u *)(&dp->db_اشاریہ[dp->db_سطر_گنتی]);
    متن_شروع = (char_u *)dp + dp->db_txt_شروع;
    متن_len = ناپ - dp->db_txt_شروع;

    // Copy the header and the text.
    mch_memmove(new_انکڑے, dp, سر_سرا - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, متن_شروع, متن_len, new_انکڑے + dp->db_txt_شروع,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    إذا (سر_سرا < متن_شروع)
	vim_memset(new_انکڑے + (سر_سرا - انکڑے), 0, متن_شروع - سر_سرا);

    أرجع new_انکڑے;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    فراغ
ml_decrypt_انکڑے(
    memfile_T	*mfp,
    char_u	*انکڑے,
    off_T	وتھ,
    بنانشانی	ناپ)
{
    انکڑے_BL	*dp = (انکڑے_BL *)انکڑے;
    char_u	*سر_سرا;
    char_u	*متن_شروع;
    صحيح		متن_len;
    cryptstate_T *state;

    إذا (dp->db_id != انکڑے_ID)
	أرجع;

    سر_سرا = (char_u *)(&dp->db_اشاریہ[dp->db_سطر_گنتی]);
    متن_شروع = (char_u *)dp + dp->db_txt_شروع;
    متن_len = dp->db_txt_سرا - dp->db_txt_شروع;

    إذا (سر_سرا > متن_شروع || dp->db_txt_شروع > ناپ
	    || dp->db_txt_سرا > ناپ)
	أرجع;  // data was messed up

    state = ml_crypt_prepare(mfp, وتھ, TRUE);
    إذا (state == عدم)
	أرجع;

    // Decrypt the text in place.
    crypt_decode_inplace(state, متن_شروع, متن_len, FALSE);
    crypt_free_state(state);
}

/*
 * Prepare for encryption/decryption, using the key, seed and offset.
 * Return an allocated cryptstate_T *.
 * Note: Encryption not supported for SODIUM
 */
    ساكن cryptstate_T *
ml_crypt_prepare(memfile_T *mfp, off_T وتھ, صحيح reading)
{
    buf_T	*buf = mfp->mf_بفر;
    char_u	salt[50];
    صحيح		طریقہ_nr;
    char_u	*کلید;
    crypt_arg_T arg;

    CLEAR_میدان(arg);
    إذا (reading && mfp->mf_old_کلید != عدم)
    {
	// Reading back blocks with the previous key/method/seed.
	طریقہ_nr = mfp->mf_old_cm;
	کلید = mfp->mf_old_کلید;
	arg.cat_بیج = mfp->mf_old_بیج;
    }
    وإلا
    {
	طریقہ_nr = crypt_get_طریقہ_nr(buf);
	کلید = buf->b_p_کلید;
	arg.cat_بیج = mfp->mf_بیج;
    }

    إذا (*کلید == NUL)
	أرجع عدم;

    إذا (crypt_may_close_swapfile(buf, کلید, طریقہ_nr))
	أرجع عدم;

    إذا (طریقہ_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((حرف *)salt, حجم(salt), "%s%ld", کلید, (لما)وتھ);
	arg.cat_بیج = عدم;
	arg.cat_init_from_file = FALSE;

	أرجع crypt_create(طریقہ_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((حرف *)salt, حجم(salt), "%ld", (لما)وتھ);

    arg.cat_salt = salt;
    arg.cat_salt_len = (صحيح)STRLEN(salt);
    arg.cat_بیج_len = MF_بیج_LEN;
    arg.cat_جوڑو_len = 0;
    arg.cat_جوڑو = عدم;
    arg.cat_init_from_file = FALSE;

    أرجع crypt_create(طریقہ_nr, کلید, &arg);
}

#جے_انت


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
    linenr_T	سطر,
    لما	len,
    صحيح		updtype)
{
    ساكن buf_T	*ml_upd_lastbuf = عدم;
    ساكن linenr_T	ml_upd_lastcurline;
    ساكن صحيح		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    صحيح			curix = ml_upd_lastcurix;
    لما		ناپ;
    chunksize_T		*curchnk;
    صحيح			rest;
    bhdr_T		*hp;
    انکڑے_BL		*dp;

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

    إذا (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_سطر_گنتی == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (لما)buf->b_ml.ml_سطر_len;
	ml_upd_lastbuf = عدم;		// invalidate resume cache
	أرجع;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    إذا (buf != ml_upd_lastbuf || سطر < curline)
    {
	curline = 1;
	curix = 0;
    }
    لكل (; curix < buf->b_ml.ml_usedchunks - 1
	     && سطر >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
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
	    صحيح	    گنتی;	    // number of entries in block
	    صحيح	    idx;
	    صحيح	    سرا_idx;
	    صحيح	    متن_سرا;
	    صحيح	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			حجم(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    ناپ = 0;
	    linecnt = 0;
	    طالما (curline < buf->b_ml.ml_سطر_گنتی
			&& linecnt < MLCS_MINL)
	    {
		إذا ((hp = ml_لبھو_سطر(buf, curline, ML_لبھو)) == عدم)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    أرجع;
		}
		dp = (انکڑے_BL *)(hp->bh_انکڑے);
		گنتی = (لما)(buf->b_ml.ml_locked_بلند) -
			(لما)(buf->b_ml.ml_locked_کم) + 1;
		idx = curline - buf->b_ml.ml_locked_کم;
		curline = buf->b_ml.ml_locked_بلند + 1;

		// compute index of last line to use in this MEMLINE
		rest = گنتی - idx;
		إذا (linecnt + rest > MLCS_MINL)
		{
		    سرا_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		وإلا
		{
		    سرا_idx = گنتی - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		إذا (buf->b_has_textprop)
		{
		    صحيح i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    لكل (i = سرا_idx; i < idx; ++i)
			ناپ += (صحيح)STRLEN((char_u *)dp
				      + (dp->db_اشاریہ[i] & DB_اشاریہ_نقاب)) + 1;
		}
		وإلا
# endif
		{
		    إذا (idx == 0) // first line in block, text at the end
			متن_سرا = dp->db_txt_سرا;
		    وإلا
			متن_سرا = ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب);
		    ناپ += متن_سرا
				   - ((dp->db_اشاریہ[سرا_idx]) & DB_اشاریہ_نقاب);
		}
	    }
	    buf->b_ml.ml_chunksize[curix].mlcs_numlines = linecnt;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_numlines -= linecnt;
	    buf->b_ml.ml_chunksize[curix].mlcs_totalsize = ناپ;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_totalsize -= ناپ;
	    buf->b_ml.ml_usedchunks++;
	    ml_upd_lastbuf = عدم;   // Force recalc of curix & curline
	    أرجع;
	}
	وإلا إذا (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_سطر_گنتی - سطر <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    إذا (سطر == buf->b_ml.ml_سطر_گنتی)
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
		hp = ml_لبھو_سطر(buf, buf->b_ml.ml_سطر_گنتی, ML_لبھو);
		إذا (hp == عدم)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    أرجع;
		}
		dp = (انکڑے_BL *)(hp->bh_انکڑے);
		إذا (dp->db_سطر_گنتی == 1)
		    rest = dp->db_txt_سرا - dp->db_txt_شروع;
		وإلا
		    rest =
			((dp->db_اشاریہ[dp->db_سطر_گنتی - 2]) & DB_اشاریہ_نقاب)
			- dp->db_txt_شروع;
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
    لما
ml_لبھو_سطر_or_وتھ(buf_T *buf, linenr_T lnum, لما *offp)
{
    linenr_T	curline;
    صحيح		curix;
    لما	ناپ;
    bhdr_T	*hp;
    انکڑے_BL	*dp;
    صحيح		گنتی;		// number of entries in block
    صحيح		idx;
    صحيح		شروع_idx;
    صحيح		متن_سرا;
    لما	وتھ;
    صحيح		len;
    صحيح		ffdos = (get_fileformat(buf) == EOL_DOS);
    صحيح		extra = 0;

    // take care of cached line first
    ml_flush_سطر(curbuf);

    إذا (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == عدم
	    || lnum < 0)
	أرجع -1;

    إذا (offp == عدم)
	وتھ = 0;
    وإلا
	وتھ = *offp;
    إذا (lnum == 0 && وتھ <= 0)
	أرجع 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = ناپ = 0;
    طالما (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (وتھ != 0
	       && وتھ > ناپ + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (لما)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	ناپ += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	إذا (وتھ && ffdos)
	    ناپ += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    طالما ((lnum != 0 && curline < lnum) || (وتھ != 0 && ناپ < وتھ))
    {
# ifdef FEAT_PROP_POPUP
	size_t textprop_کل = 0;
# endif

	إذا (curline > buf->b_ml.ml_سطر_گنتی
		|| (hp = ml_لبھو_سطر(buf, curline, ML_لبھو)) == عدم)
	    أرجع -1;
	dp = (انکڑے_BL *)(hp->bh_انکڑے);
	گنتی = (لما)(buf->b_ml.ml_locked_بلند) -
		(لما)(buf->b_ml.ml_locked_کم) + 1;
	شروع_idx = idx = curline - buf->b_ml.ml_locked_کم;
	إذا (idx == 0)  // first line in block, text at the end
	    متن_سرا = dp->db_txt_سرا;
	وإلا
	    متن_سرا = ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب);
	// Compute index of last line to use in this MEMLINE
	إذا (lnum != 0)
	{
	    إذا (curline + (گنتی - idx) >= lnum)
		idx += lnum - curline - 1;
	    وإلا
		idx = گنتی - 1;
	}
	وإلا
	{
	    extra = 0;
	    لكل (;;)
	    {
# ifdef FEAT_PROP_POPUP
		size_t textprop_ناپ = 0;

		إذا (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_سرا
				  : ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب));
		    textprop_ناپ = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		إذا (!(وتھ >= ناپ
			+ متن_سرا - (صحيح)((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب)
# ifdef FEAT_PROP_POPUP
			- (لما)(textprop_کل + textprop_ناپ)
# endif
			+ ffdos))
		    اكسر;

		إذا (ffdos)
		    ناپ++;
# ifdef FEAT_PROP_POPUP
		textprop_کل += textprop_ناپ;
# endif
		إذا (idx == گنتی - 1)
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
	    لكل (i = شروع_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_اشاریہ[i]) & DB_اشاریہ_نقاب);
		len += (صحيح)STRLEN(p) + 1;
	    }
	}
	وإلا
# endif
	    len = متن_سرا - ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب)
# ifdef FEAT_PROP_POPUP
				- (لما)textprop_کل
# endif
				;
	ناپ += len;
	إذا (وتھ != 0 && ناپ >= وتھ)
	{
	    إذا (ناپ + ffdos == وتھ)
		*offp = 0;
	    وإلا إذا (idx == شروع_idx)
		*offp = وتھ - ناپ + len;
	    وإلا
		*offp = وتھ - ناپ + len
		     - (متن_سرا - ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب))
# ifdef FEAT_PROP_POPUP
		     + (لما)textprop_کل
# endif
		     ;
	    curline += idx - شروع_idx + extra;
	    إذا (curline > buf->b_ml.ml_سطر_گنتی)
		أرجع -1;	// exactly one byte beyond the end
	    أرجع curline;
	}
	curline = buf->b_ml.ml_locked_بلند + 1;
    }

    إذا (lnum != 0)
    {
	// Count extra CR characters.
	إذا (ffdos)
	    ناپ += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	إذا ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_سطر_گنتی)
	    ناپ -= ffdos + 1;
    }

    أرجع ناپ;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    فراغ
goto_byte(لما cnt)
{
    لما	boff = cnt;
    linenr_T	lnum;

    ml_flush_سطر(curbuf);	// cached line may be dirty
    setpcmark();
    إذا (boff)
	--boff;
    lnum = ml_لبھو_سطر_or_وتھ(curbuf, (linenr_T)0, &boff);
    إذا (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	curwin->w_curswant = MAXCOL;
	coladvance((colnr_T)MAXCOL);
    }
    وإلا
    {
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.خانہ = (colnr_T)boff;
	curwin->w_cursor.coladd = 0;
	curwin->w_سیٹ_curswant = true;
    }
    check_cursor();

    // Make sure the cursor is on the first byte of a multi-byte char.
    إذا (has_mbyte)
	mb_adjust_cursor();
}
#جے_انت
