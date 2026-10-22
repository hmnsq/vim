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
#تعریف_کرو CHECK(c, s)	کرو { /**/ } جب (0)

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

#شامل "vim.h"

#شامل <time.h>

#اگر defined(SASC) || defined(__amigaos4__)
# include <proto/dos.h>	    // for Open() and Close()
#اگر_ختم

قسمتعریف ساخت block0		ZERO_BL;    // contents of the first block
قسمتعریف ساخت اشارہ_گر_بلاک	PTR_BL;	    // contents of a pointer block
قسمتعریف ساخت ڈیٹا_بلاک	ڈیٹا_BL;    // contents of a data block
قسمتعریف ساخت اشارہ_گر_اندراج	PTR_EN;	    // block/line-count pair

#تعریف_کرو ڈیٹا_ID	       (('d' << 8) + 'a')   // data block id
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

#اگر defined(FEAT_CRYPT)
ساکن ثابت صحیح id1_codes[] = {
    BLOCK0_ID1_C0,  // CRYPT_M_ZIP
    BLOCK0_ID1_C1,  // CRYPT_M_BF
    BLOCK0_ID1_C2,  // CRYPT_M_BF2
    BLOCK0_ID1_C3,  // CRYPT_M_SOD  - Unused!
    BLOCK0_ID1_C4,  // CRYPT_M_SOD2  - Unused!
};
#اگر_ختم

/*
 * pointer to a block, used in a pointer block
 */
ساخت اشارہ_گر_اندراج
{
    blocknr_T	pe_bnum;	// block number
    linenr_T	pe_سطر_گنتی;	// number of lines in this branch
    linenr_T	pe_old_lnum;	// lnum for this block (for recovery)
    صحیح		pe_صفحہ_گنتی;	// number of pages in block pe_bnum
};

/*
 * A pointer block contains a list of branches in the tree.
 */
ساخت اشارہ_گر_بلاک
{
    short_u	pb_id;		// ID for pointer block: PTR_ID
    short_u	pb_گنتی;	// number of pointers in this block
    short_u	pb_گنتی_max;	// maximum value for pb_count
    PTR_EN	pb_اشارہ_گر[1];	// list of pointers to blocks (actually longer)
				// followed by empty space until end of page
};

// Value for pb_count_max.
#تعریف_کرو PB_گنتی_MAX(mfp) (short_u)(((mfp)->mf_صفحہ_سائز - offsetof(PTR_BL, pb_اشارہ_گر)) / حجم(PTR_EN))

/*
 * A data block is a leaf in the tree.
 *
 * The text of the lines is at the end of the block. The text of the first line
 * in the block is put at the end, the text of the second line in front of it,
 * etc. Thus the order of the lines is the opposite of the line number.
 */
ساخت ڈیٹا_بلاک
{
    short_u	db_id;		// ID for data block: DATA_ID
    غیرعلامتی	db_free;	// free space available
    غیرعلامتی	db_txt_آغاز;	// byte where text starts
    غیرعلامتی	db_txt_انتہا;	// byte just after data block
    linenr_T	db_سطر_گنتی;	// number of lines in this block
    غیرعلامتی	db_اشاریہ[1];	// index for start of line (actually bigger)
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
#تعریف_کرو DB_MARKED	((غیرعلامتی)1 << ((حجم(غیرعلامتی) * 8) - 1))
#تعریف_کرو DB_اشاریہ_نقاب	(~DB_MARKED)

#تعریف_کرو اشاریہ_سائز  (حجم(غیرعلامتی))	    // size of one db_index entry
#تعریف_کرو سرخی_سائز (offsetof(ڈیٹا_BL, db_اشاریہ))  // size of data block header

#تعریف_کرو B0_FNAME_سائز_ORG	900	// what it was in older versions
#تعریف_کرو B0_FNAME_سائز_NOCRYPT	898	// 2 bytes used for other things
#تعریف_کرو B0_FNAME_سائز_CRYPT	890	// 10 bytes used for other things
#تعریف_کرو B0_UNAME_سائز		40
#تعریف_کرو B0_HNAME_سائز		40
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
ساخت block0
{
    char_u	b0_id[2];	// id for block 0: BLOCK0_ID0 and BLOCK0_ID1,
				// BLOCK0_ID1_C0, BLOCK0_ID1_C1, etc.
    char_u	b0_نسخہ[10];	// Vim version string
    char_u	b0_صفحہ_سائز[4];// number of bytes per page
    char_u	b0_mtime[4];	// last modification time of file
    char_u	b0_ino[4];	// inode of b0_fname
    char_u	b0_pid[4];	// process id of creator (or 0)
    char_u	b0_uname[B0_UNAME_سائز]; // name of user (uid if no name)
    char_u	b0_hname[B0_HNAME_سائز]; // host name (if it has a name)
    char_u	b0_fname[B0_FNAME_سائز_ORG]; // name of file being edited
    طویل	b0_magic_long;	// check for byte order of long
    صحیح		b0_magic_int;	// check for byte order of int
    مختصر	b0_magic_short;	// check for byte order of short
    char_u	b0_magic_char;	// check for last char
};

/*
 * Note: b0_dirty and b0_flags are put at the end of the file name.  For very
 * long file names in older versions of Vim they are invalid.
 * The 'fileencoding' comes before b0_flags, with a NUL in front.  But only
 * when there is room, for very long file names it's omitted.
 */
#تعریف_کرو B0_DIRTY	0x55
#تعریف_کرو b0_dirty	b0_fname[B0_FNAME_سائز_ORG - 1]

/*
 * The b0_flags field is new in Vim 7.0.
 */
#تعریف_کرو b0_flags	b0_fname[B0_FNAME_سائز_ORG - 2]

/*
 * Crypt seed goes here, 8 bytes.  New in Vim 7.3.
 * Without encryption these bytes may be used for 'fenc'.
 */
#تعریف_کرو b0_بیج		b0_fname[B0_FNAME_سائز_ORG - 2 - MF_بیج_LEN]

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
ساکن linenr_T	سب_سے_نیچا_marked = 0;

/*
 * arguments for ml_find_line()
 */
#تعریف_کرو ML_DELETE	0x11	    // delete line
#تعریف_کرو ML_داخل_کرو	0x12	    // insert line
#تعریف_کرو ML_ڈھونڈو		0x13	    // just find the line
#تعریف_کرو ML_FLUSH	0x02	    // flush locked block
#تعریف_کرو ML_SIMPLE(x)	((x) & 0x10)  // DEL, INS or FIND

// argument for ml_upd_block0()
قسمتعریف شمار {
      UB_FNAME = 0	// update timestamp and filename
    , UB_SAME_DIR       // update the B0_SAME_DIR flag
    , UB_CRYPT		// update crypt key
} upd_block0_T;

#اگر_تعریف_شدہ FEAT_CRYPT
ساکن خالی ml_سیٹ_b0_crypt(buf_T *buf, ZERO_BL *b0p);
#اگر_ختم
ساکن خالی ml_upd_block0(buf_T *buf, upd_block0_T what);
ساکن خالی سیٹ_b0_fname(ZERO_BL *, buf_T *buf);
ساکن خالی سیٹ_b0_dir_flag(ZERO_BL *b0p, buf_T *buf);
ساکن خالی جوڑو_b0_fenc(ZERO_BL *b0p, buf_T *buf);
ساکن time_t swapfile_info(char_u *);
ساکن صحیح recov_file_نام(char_u **, char_u *, صحیح prepend_dot);
ساکن char_u *findswapname(buf_T *, char_u **, char_u *);
ساکن خالی ml_flush_سطر(buf_T *);
ساکن bhdr_T *ml_new_ڈیٹا(memfile_T *, صحیح, صحیح);
ساکن bhdr_T *ml_new_ptr(memfile_T *);
ساکن bhdr_T *ml_ڈھونڈو_سطر(buf_T *, linenr_T, صحیح);
ساکن صحیح ml_جوڑو_ڈھیر(buf_T *);
ساکن خالی ml_lineadd(buf_T *, صحیح);
ساکن صحیح b0_magic_wrong(ZERO_BL *);
#اگر_تعریف_شدہ CHECK_INODE
ساکن صحیح fnamecmp_ino(char_u *, char_u *, طویل);
#اگر_ختم
ساکن خالی long_مقصد_char(طویل, char_u *);
ساکن طویل char_مقصد_long(char_u *);
#اگر_تعریف_شدہ FEAT_CRYPT
ساکن cryptstate_T *ml_crypt_prepare(memfile_T *mfp, off_T انحراف, صحیح reading);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_BYTEOFF
ساکن خالی ml_updatechunk(buf_T *buf, طویل سطر, طویل len, صحیح updtype);
#اگر_ختم

/*
 * Open a new memline for "buf".
 *
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
ml_open(buf_T *buf)
{
    memfile_T	*mfp;
    bhdr_T	*hp = تہی;
    ZERO_BL	*b0p;
    PTR_BL	*pp;
    ڈیٹا_BL	*dp;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ڈھیر_سائز = 0; // no stack yet
    buf->b_ml.ml_ڈھیر = تہی;	// no stack yet
    buf->b_ml.ml_ڈھیر_چوٹی = 0;	// nothing in the stack
    buf->b_ml.ml_locked = تہی;	// no cached block
    buf->b_ml.ml_سطر_lnum = 0;	// no cached line
#اگر_تعریف_شدہ FEAT_BYTEOFF
    buf->b_ml.ml_chunksize = تہی;
    buf->b_ml.ml_usedchunks = 0;
#اگر_ختم

    اگر (cmdmod.cmod_flags & CMOD_NOSWAPFILE)
	buf->b_p_swf = FALSE;

    /*
     * When 'updatecount' is non-zero swap file may be opened later.
     */
    اگر (p_uc && buf->b_p_swf)
	buf->b_may_تبادلہ = true;
    ورنہ
	buf->b_may_تبادلہ = false;

    /*
     * Open the memfile.  No swap file is created yet.
     */
    mfp = mf_open(تہی, 0);
    اگر (mfp == تہی)
	جاؤ error;

    buf->b_ml.ml_mfp = mfp;
#اگر_تعریف_شدہ FEAT_CRYPT
    mfp->mf_بفر = buf;
#اگر_ختم
    buf->b_ml.ml_flags = ML_خالی_خانے;
    buf->b_ml.ml_سطر_گنتی = 1;

/*
 * fill block0 struct and write page 0
 */
    اگر ((hp = mf_new(mfp, FALSE, 1)) == تہی)
	جاؤ error;
    اگر (hp->bh_bnum != 0)
    {
	iemsg(e_didnt_get_بلاک_nr_zero);
	جاؤ error;
    }
    b0p = (ZERO_BL *)(hp->bh_ڈیٹا);

    b0p->b0_id[0] = BLOCK0_ID0;
    b0p->b0_id[1] = BLOCK0_ID1;
    b0p->b0_magic_long = (طویل)B0_MAGIC_LONG;
    b0p->b0_magic_int = (صحیح)B0_MAGIC_INT;
    b0p->b0_magic_short = (مختصر)B0_MAGIC_SHORT;
    b0p->b0_magic_char = B0_MAGIC_CHAR;
    mch_memmove(b0p->b0_نسخہ, "VIM ", 4);
    STRNCPY(b0p->b0_نسخہ + 4, نسخہ, 6);
    long_مقصد_char((طویل)mfp->mf_صفحہ_سائز, b0p->b0_صفحہ_سائز);

#اگر_تعریف_شدہ FEAT_SPELL
    اگر (!buf->b_spell)
#اگر_ختم
    {
	b0p->b0_dirty = buf->b_بدلا ? B0_DIRTY : 0;
	b0p->b0_flags = get_fileformat(buf) + 1;
	سیٹ_b0_fname(b0p, buf);
	(خالی)get_user_name(b0p->b0_uname, B0_UNAME_سائز);
	b0p->b0_uname[B0_UNAME_سائز - 1] = NUL;
	mch_get_host_name(b0p->b0_hname, B0_HNAME_سائز);
	b0p->b0_hname[B0_HNAME_سائز - 1] = NUL;
	long_مقصد_char(mch_get_pid(), b0p->b0_pid);
#اگر_تعریف_شدہ FEAT_CRYPT
	ml_سیٹ_b0_crypt(buf, b0p);
#اگر_ختم
    }

    /*
     * Always sync block number 0 to disk, so we can check the file name in
     * the swap file in findswapname(). Don't do this for a help files or
     * a spell buffer though.
     * Only works when there's a swapfile, otherwise it's done when the file
     * is created.
     */
    mf_put(mfp, hp, TRUE, FALSE);
    اگر (!buf->b_help && !B_SPELL(buf))
	(خالی)mf_sync(mfp, 0);

    /*
     * Fill in root pointer block and write page 1.
     */
    اگر ((hp = ml_new_ptr(mfp)) == تہی)
	جاؤ error;
    اگر (hp->bh_bnum != 1)
    {
	iemsg(e_didnt_get_بلاک_nr_one);
	جاؤ error;
    }
    pp = (PTR_BL *)(hp->bh_ڈیٹا);
    pp->pb_گنتی = 1;
    pp->pb_اشارہ_گر[0].pe_bnum = 2;
    pp->pb_اشارہ_گر[0].pe_صفحہ_گنتی = 1;
    pp->pb_اشارہ_گر[0].pe_old_lnum = 1;
    pp->pb_اشارہ_گر[0].pe_سطر_گنتی = 1;    // line count after insertion
    mf_put(mfp, hp, TRUE, FALSE);

    /*
     * Allocate first data block and create an empty line 1.
     */
    اگر ((hp = ml_new_ڈیٹا(mfp, FALSE, 1)) == تہی)
	جاؤ error;
    اگر (hp->bh_bnum != 2)
    {
	iemsg(e_didnt_get_بلاک_nr_two);
	جاؤ error;
    }

    dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
    dp->db_اشاریہ[0] = --dp->db_txt_آغاز;	// at end of block
    dp->db_free -= 1 + اشاریہ_سائز;
    dp->db_سطر_گنتی = 1;
    *((char_u *)dp + dp->db_txt_آغاز) = NUL;	// empty line

    واپس OK;

error:
    اگر (mfp != تہی)
    {
	اگر (hp)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, TRUE);	    // will also free(mfp->mf_fname)
    }
    buf->b_ml.ml_mfp = تہی;
    واپس FAIL;
}

#اگر defined(FEAT_CRYPT)
/*
 * Swapfile encryption is not supported by XChaCha20.  If this crypt method is
 * used then disable the swapfile, to avoid plain text being written to disk,
 * and return TRUE.
 * Otherwise return FALSE.
 */
    ساکن صحیح
crypt_may_close_swapfile(buf_T *buf, char_u *کلید, صحیح طریقہ)
{
    اگر (crypt_طریقہ_is_sodium(طریقہ) && *کلید != NUL)
    {
	mf_close_file(buf, TRUE);
	buf->b_p_swf = FALSE;
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Prepare encryption for "buf" for the current key and method.
 */
    ساکن خالی
ml_سیٹ_mfp_crypt(buf_T *buf)
{
    اگر (*buf->b_p_کلید == NUL)
	واپس;

    صحیح طریقہ_nr = crypt_get_طریقہ_nr(buf);

    اگر (طریقہ_nr > CRYPT_M_ZIP && طریقہ_nr < CRYPT_M_SOD)
    {
	// Generate a seed and store it in the memfile.
	sha2_بیج(buf->b_ml.ml_mfp->mf_بیج, MF_بیج_LEN, تہی, 0);
    }
# ifdef FEAT_SODIUM
    ورنہ اگر (crypt_طریقہ_is_sodium(طریقہ_nr))
	crypt_sodium_randombytes_buf(buf->b_ml.ml_mfp->mf_بیج, MF_بیج_LEN);
# endif
}

/*
 * Prepare encryption for "buf" with block 0 "b0p".
 * Note: should not be called with libsodium encryption, since xchacha20 does
 * not support swapfile encryption.
 */
    ساکن خالی
ml_سیٹ_b0_crypt(buf_T *buf, ZERO_BL *b0p)
{
    اگر (*buf->b_p_کلید == NUL)
	b0p->b0_id[1] = BLOCK0_ID1;
    ورنہ
    {
	صحیح طریقہ_nr = crypt_get_طریقہ_nr(buf);

	b0p->b0_id[1] = id1_codes[طریقہ_nr];
	اگر (طریقہ_nr > CRYPT_M_ZIP && طریقہ_nr < CRYPT_M_SOD)
	{
	    // Generate a seed and store it in block 0 and in the memfile.
	    sha2_بیج(&b0p->b0_بیج, MF_بیج_LEN, تہی, 0);
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
    خالی
ml_سیٹ_crypt_کلید(
    buf_T	*buf,
    char_u	*old_کلید,
    char_u	*old_cm)
{
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;
    صحیح		صفحہ_گنتی;
    صحیح		idx;
    طویل	error;
    infoptr_T	*ip;
    PTR_BL	*pp;
    ڈیٹا_BL	*dp;
    blocknr_T	bnum;
    صحیح		چوٹی;
    صحیح		old_طریقہ;

    اگر (mfp == تہی || mfp->mf_fd < 0)
	واپس;  // no memfile yet, nothing to do
    old_طریقہ = crypt_طریقہ_nr_from_name(old_cm);

# ifdef FEAT_CRYPT
    اگر (crypt_may_close_swapfile(buf, buf->b_p_کلید, crypt_get_طریقہ_nr(buf)))
	واپس;
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
    اگر (old_طریقہ > 0 && *old_کلید != NUL)
	mch_memmove(mfp->mf_old_بیج, mfp->mf_بیج, MF_بیج_LEN);

    // Update block 0 with the crypt flag and may set a new seed.
    ml_upd_block0(buf, UB_CRYPT);

    اگر (mfp->mf_infile_گنتی > 2)
    {
	/*
	 * Need to read back all data blocks from disk, decrypt them with the
	 * old key/method and mark them to be written. The algorithm is
	 * similar to what happens in ml_recover(), but we skip negative block
	 * numbers.
	 */
	ml_flush_سطر(buf);		    // flush buffered line
	(خالی)ml_ڈھونڈو_سطر(buf, (linenr_T)0, ML_FLUSH); // flush locked block

	hp = تہی;
	bnum = 1;		// start with block 1
	صفحہ_گنتی = 1;		// which is 1 page
	idx = 0;		// start with first index in block 1
	error = 0;
	buf->b_ml.ml_ڈھیر_چوٹی = 0;
	VIM_CLEAR(buf->b_ml.ml_ڈھیر);
	buf->b_ml.ml_ڈھیر_سائز = 0;	// no stack yet

	برائے ( ; !got_int; سطر_breakcheck())
	{
	    اگر (hp != تہی)
		mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	    // get the block (pointer or data)
	    اگر ((hp = mf_get(mfp, bnum, صفحہ_گنتی)) == تہی)
	    {
		اگر (bnum == 1)
		    توڑو;
		++error;
	    }
	    ورنہ
	    {
		pp = (PTR_BL *)(hp->bh_ڈیٹا);
		اگر (pp->pb_id == PTR_ID)	// it is a pointer block
		{
		    اگر (pp->pb_گنتی == 0)
		    {
			// empty block?
			++error;
		    }
		    ورنہ اگر (idx < (صحیح)pp->pb_گنتی)	// go a block deeper
		    {
			اگر (pp->pb_اشارہ_گر[idx].pe_bnum < 0)
			{
			    // Skip data block with negative block number.
			    // Should not happen, because of the ml_preserve()
			    // above. Get same block again for next index.
			    ++idx;
			    جاری;
			}

			// going one block deeper in the tree, new entry in
			//ڈھیر
			اگر ((چوٹی = ml_جوڑو_ڈھیر(buf)) < 0)
			{
			    ++error;
			    توڑو;		    // out of memory
			}
			ip = &(buf->b_ml.ml_ڈھیر[چوٹی]);
			ip->ip_bnum = bnum;
			ip->ip_اشاریہ = idx;

			bnum = pp->pb_اشارہ_گر[idx].pe_bnum;
			صفحہ_گنتی = pp->pb_اشارہ_گر[idx].pe_صفحہ_گنتی;
			idx = 0;
			جاری;
		    }
		}
		ورنہ	    // not a pointer block
		{
		    dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
		    اگر (dp->db_id != ڈیٹا_ID)	// block id wrong
			++error;
		    ورنہ
		    {
			// It is a data block, need to write it back to disk.
			mf_put(mfp, hp, TRUE, FALSE);
			hp = تہی;
		    }
		}
	    }

	    اگر (buf->b_ml.ml_ڈھیر_چوٹی == 0)	//ختم_شدہ
		توڑو;

	    // go one block up in the tree
	    ip = &(buf->b_ml.ml_ڈھیر[--(buf->b_ml.ml_ڈھیر_چوٹی)]);
	    bnum = ip->ip_bnum;
	    idx = ip->ip_اشاریہ + 1;	    // go to next index
	    صفحہ_گنتی = 1;
	}
	اگر (hp != تہی)
	    mf_put(mfp, hp, FALSE, FALSE);  // release previous block

	اگر (error > 0)
	    emsg(_(e_error_while_updating_تبادلہ_file_crypt));
    }

    mfp->mf_old_کلید = تہی;
}
#اگر_ختم

/*
 * ml_setname() is called when the file name of "buf" has been changed.
 * It may rename the swap file.
 */
    خالی
ml_setname(buf_T *buf)
{
    صحیح		success = FALSE;
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;
#اگر defined(MSWIN)
    char_u	*p;
#اگر_ختم

    mfp = buf->b_ml.ml_mfp;
    اگر (mfp->mf_fd < 0)		    // there is no swap file yet
    {
	/*
	 * When 'updatecount' is 0 and 'noswapfile' there is no swap file.
	 * For help files we will make a swap file now.
	 */
	اگر (p_uc != 0 && (cmdmod.cmod_flags & CMOD_NOSWAPFILE) == 0)
	    ml_open_file(buf);	    // create a swap file
	واپس;
    }

    /*
     * Try all directories in the 'directory' option.
     */
    dirp = p_dir;
    برائے (;;)
    {
	اگر (*dirp == NUL)	    // tried all directories, fail
	    توڑو;
	fname = findswapname(buf, &dirp, mfp->mf_fname);
	// autocmd may have freed mfp if findswapname creates
	// different swapfile name
	اگر (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    واپس;
	}
						    // alloc's fname
	اگر (dirp == تہی)	    // out of memory
	    توڑو;
	اگر (fname == تہی)	    // no file name found for this dir
	    جاری;

#اگر defined(MSWIN)
	/*
	 * Set full pathname for swap file now, because a ":!cd dir" may
	 * change directory without us knowing it.
	 */
	p = بھرا_Name_save(fname, FALSE);
	vim_free(fname);
	fname = p;
	اگر (fname == تہی)
	    جاری;
#اگر_ختم
	// if the file name is the same we don't have to do anything
	اگر (fnamecmp(fname, mfp->mf_fname) == 0)
	{
	    vim_free(fname);
	    success = TRUE;
	    توڑو;
	}
	// need to close the swap file before renaming
	اگر (mfp->mf_fd >= 0)
	{
	    close(mfp->mf_fd);
	    mfp->mf_fd = -1;
	}

	// try to rename the swap file
	اگر (vim_rename(mfp->mf_fname, fname) == 0)
	{
	    success = TRUE;
	    vim_free(mfp->mf_fname);
	    mfp->mf_fname = fname;
	    vim_free(mfp->mf_ffname);
#اگر defined(MSWIN)
	    mfp->mf_ffname = تہی;  // mf_fname is full pathname already
#ورنہ
	    mf_سیٹ_ffname(mfp);
#اگر_ختم
	    ml_upd_block0(buf, UB_SAME_DIR);
	    توڑو;
	}
	vim_free(fname);	    // this fname didn't work, try another
    }

    اگر (mfp->mf_fd == -1)	    // need to (re)open the swap file
    {
	mfp->mf_fd = mch_open((حرف *)mfp->mf_fname, O_RDWR | O_EXTRA, 0);
	اگر (mfp->mf_fd < 0)
	{
	    // could not (re)open the swap file, what can we do????
	    emsg(_(e_oops_گمشدہ_the_تبادلہ_file));
	    واپس;
	}
#اگر_تعریف_شدہ HAVE_FD_CLOEXEC
	{
	    صحیح fdflags = fcntl(mfp->mf_fd, F_GETFD);
	    اگر (fdflags >= 0 && (fdflags & FD_CLOEXEC) == 0)
		(خالی)fcntl(mfp->mf_fd, F_SETFD, fdflags | FD_CLOEXEC);
	}
#اگر_ختم
    }
    اگر (!success)
	emsg(_(e_could_not_rename_تبادلہ_file));
}

/*
 * Open a file for the memfile for all buffers that are not readonly or have
 * been modified.
 * Used when 'updatecount' changes from zero to non-zero.
 */
    خالی
ml_open_files(خالی)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	اگر (!buf->b_p_ro || buf->b_بدلا)
	    ml_open_file(buf);
}

/*
 * Open a swap file for an existing memfile, if there is no swap file yet.
 * If we are unable to find a file name, mf_fname will be NULL
 * and the memfile will be in memory only (no recovery possible).
 */
    خالی
ml_open_file(buf_T *buf)
{
    memfile_T	*mfp;
    char_u	*fname;
    char_u	*dirp;

    mfp = buf->b_ml.ml_mfp;
    اگر (mfp == تہی || mfp->mf_fd >= 0 || !buf->b_p_swf
				      || (cmdmod.cmod_flags & CMOD_NOSWAPFILE))
	واپس;		// nothing to do

#اگر_تعریف_شدہ FEAT_SPELL
    // For a spell buffer use a temp file name.
    اگر (buf->b_spell)
    {
	fname = vim_tempname('s', FALSE);
	اگر (fname != تہی)
	    (خالی)mf_open_file(mfp, fname);	// consumes fname!
	buf->b_may_تبادلہ = false;
	واپس;
    }
#اگر_ختم

    /*
     * Try all directories in 'directory' option.
     */
    dirp = p_dir;
    برائے (;;)
    {
	اگر (*dirp == NUL)
	    توڑو;
	// There is a small chance that between choosing the swap file name
	// and creating it, another Vim creates the file.  In that case the
	// creation will fail and we will use another directory.
	fname = findswapname(buf, &dirp, تہی); // allocates fname
	// autocmd may have freed mfp, grr!
	اگر (buf->b_ml.ml_mfp != mfp)
	{
	    vim_free(fname);
	    واپس;
	}
	اگر (dirp == تہی)
	    توڑو;  // out of memory
	اگر (fname == تہی)
	    جاری;
	اگر (mf_open_file(mfp, fname) == OK)	// consumes fname!
	{
	    // don't sync yet in ml_sync_all()
	    mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;
#اگر defined(MSWIN)
	    /*
	     * set full pathname for swap file now, because a ":!cd dir" may
	     * change directory without us knowing it.
	     */
	    mf_fullname(mfp);
#اگر_ختم
	    ml_upd_block0(buf, UB_SAME_DIR);

	    // Flush block zero, so others can read it
	    اگر (mf_sync(mfp, MFS_ZERO) == OK)
	    {
		// Mark all blocks that should be in the swapfile as dirty.
		// Needed for when the 'swapfile' option was reset, so that
		// the swap file was deleted, and then on again.
		mf_سیٹ_dirty(mfp);
		توڑو;
	    }
	    // Writing block 0 failed: close the file and try another dir
	    mf_close_file(buf, FALSE);
	}
    }

    اگر (*p_dir != NUL && mfp->mf_fname == تہی)
    {
	need_wait_return = TRUE;	// call wait_return() later
	++no_wait_return;
	(خالی)semsg(_(e_unable_مقصد_open_تبادلہ_file_for_str_recovery_impossible),
		    buf_spname(buf) != تہی ? buf_spname(buf) : buf->b_fname);
	--no_wait_return;
    }

    // don't try to open a swap file again
    buf->b_may_تبادلہ = false;
}

/*
 * If still need to create a swap file, and starting to edit a not-readonly
 * file, or reading into an existing buffer, create a swap file now.
 */
    خالی
check_need_تبادلہ(
    صحیح	    newfile)		// reading file into new buffer
{
    صحیح old_پیغام_silent = پیغام_silent; // might be reset by an E325 message

    اگر (curbuf->b_may_تبادلہ && (!curbuf->b_p_ro || !newfile))
	ml_open_file(curbuf);
    پیغام_silent = old_پیغام_silent;
}

/*
 * Close memline for buffer 'buf'.
 * If 'del_file' is TRUE, delete the swap file
 */
    خالی
ml_close(buf_T *buf, صحیح del_file)
{
    اگر (buf->b_ml.ml_mfp == تہی)		// not open
	واپس;
    mf_close(buf->b_ml.ml_mfp, del_file);	// close the .swp file
    اگر (buf->b_ml.ml_سطر_lnum != 0
		      && (buf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED)))
	vim_free(buf->b_ml.ml_سطر_ptr);
    vim_free(buf->b_ml.ml_ڈھیر);
#اگر_تعریف_شدہ FEAT_BYTEOFF
    VIM_CLEAR(buf->b_ml.ml_chunksize);
#اگر_ختم
    buf->b_ml.ml_mfp = تہی;

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
    خالی
ml_close_all(صحیح del_file)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	ml_close(buf, del_file && ((buf->b_flags & BF_PRESERVED) == 0
				 || vim_strchr(p_cpo, CPO_PRESERVE) == تہی));
#اگر_تعریف_شدہ FEAT_SPELL
    spell_delete_wordlist();	// delete the internal wordlist
#اگر_ختم
#اگر_تعریف_شدہ TEMPDIRNAMES
    vim_deltempdir();		// delete created temp directory
#اگر_ختم
}

/*
 * Close all memfiles for not modified buffers.
 * Only use just before exiting!
 */
    خالی
ml_close_notmod(خالی)
{
    buf_T	*buf;

    FOR_ALL_BUFFERS(buf)
	اگر (!buf_Is_بدلا(buf))
	    ml_close(buf, TRUE);    // close all not-modified buffers
}

/*
 * Update the timestamp in the .swp file.
 * Used when the file has been written.
 */
    خالی
ml_timestamp(buf_T *buf)
{
    ml_upd_block0(buf, UB_FNAME);
}

/*
 * Return FAIL when the ID of "b0p" is wrong.
 */
    ساکن صحیح
ml_check_b0_id(ZERO_BL *b0p)
{
    اگر (b0p->b0_id[0] != BLOCK0_ID0
	    || (b0p->b0_id[1] != BLOCK0_ID1
		&& b0p->b0_id[1] != BLOCK0_ID1_C0
		&& b0p->b0_id[1] != BLOCK0_ID1_C1
		&& b0p->b0_id[1] != BLOCK0_ID1_C2
		&& b0p->b0_id[1] != BLOCK0_ID1_C3
		&& b0p->b0_id[1] != BLOCK0_ID1_C4)
	    )
	واپس FAIL;
    واپس OK;
}

/*
 * Update the timestamp or the B0_SAME_DIR flag of the .swp file.
 */
    ساکن خالی
ml_upd_block0(buf_T *buf, upd_block0_T what)
{
    memfile_T	*mfp;
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    mfp = buf->b_ml.ml_mfp;
    اگر (mfp == تہی)
	واپس;
    hp = mf_get(mfp, (blocknr_T)0, 1);
    اگر (hp == تہی)
    {
#اگر_تعریف_شدہ FEAT_CRYPT
	// Possibly update the seed in the memfile before there is a block0.
	اگر (what == UB_CRYPT)
	    ml_سیٹ_mfp_crypt(buf);
#اگر_ختم
	واپس;
    }

    b0p = (ZERO_BL *)(hp->bh_ڈیٹا);
    اگر (ml_check_b0_id(b0p) == FAIL)
	iemsg(e_ml_upd_block0_didnt_get_بلاک_zero);
    ورنہ
    {
	اگر (what == UB_FNAME)
	    سیٹ_b0_fname(b0p, buf);
#اگر_تعریف_شدہ FEAT_CRYPT
	ورنہ اگر (what == UB_CRYPT)
	    ml_سیٹ_b0_crypt(buf, b0p);
#اگر_ختم
	ورنہ // what == UB_SAME_DIR
	    سیٹ_b0_dir_flag(b0p, buf);
    }
    mf_put(mfp, hp, TRUE, FALSE);
}

/*
 * Write file name and timestamp into block 0 of a swap file.
 * Also set buf->b_mtime.
 * Don't use NameBuff[]!!!
 */
    ساکن خالی
سیٹ_b0_fname(ZERO_BL *b0p, buf_T *buf)
{
    stat_T	st;

    اگر (buf->b_ffname == تہی)
	b0p->b0_fname[0] = NUL;
    ورنہ
    {
#اگر defined(MSWIN) || defined(AMIGA)
	// Systems that cannot translate "~user" back into a path: copy the
	// file name unmodified.  Do use slashes instead of backslashes for
	// portability.
	vim_strncpy(b0p->b0_fname, buf->b_ffname, B0_FNAME_سائز_CRYPT - 1);
# ifdef BACKSLASH_اندر_FILENAME
	forward_slash(b0p->b0_fname);
# endif
#ورنہ
	size_t	flen;
	char_u	uname[B0_UNAME_سائز];

	/*
	 * For a file under the home directory of the current user, we try to
	 * replace the home directory path with "~user". This helps when
	 * editing the same file on different machines over a network.
	 * First replace home dir path with "~/" with home_replace().
	 * Then insert the user name to get "~user/".
	 */
	flen = home_replace(تہی, buf->b_ffname, b0p->b0_fname,
						   B0_FNAME_سائز_CRYPT, TRUE);
	اگر (b0p->b0_fname[0] == '~')
	{
	    size_t  ulen;

	    // If there is no user name or it is too long, don't use "~/"
	    اگر (get_user_name(uname, B0_UNAME_سائز) == FAIL
		   || (ulen = STRLEN(uname)) + flen > B0_FNAME_سائز_CRYPT - 1)
		vim_strncpy(b0p->b0_fname, buf->b_ffname,
						     B0_FNAME_سائز_CRYPT - 1);
	    ورنہ
	    {
		mch_memmove(b0p->b0_fname + ulen + 1, b0p->b0_fname + 1, flen);
		mch_memmove(b0p->b0_fname + 1, uname, ulen);
	    }
	}
#اگر_ختم
	اگر (mch_stat((حرف *)buf->b_ffname, &st) >= 0)
	{
	    long_مقصد_char((طویل)st.st_mtime, b0p->b0_mtime);
#اگر_تعریف_شدہ CHECK_INODE
	    long_مقصد_char((طویل)st.st_ino, b0p->b0_ino);
#اگر_ختم
	    buf_store_وقت(buf, &st, buf->b_ffname);
	    buf->b_mtime_read = buf->b_mtime;
	    buf->b_mtime_read_ns = buf->b_mtime_ns;
	}
	ورنہ
	{
	    long_مقصد_char(0L, b0p->b0_mtime);
#اگر_تعریف_شدہ CHECK_INODE
	    long_مقصد_char(0L, b0p->b0_ino);
#اگر_ختم
	    buf->b_mtime = 0;
	    buf->b_mtime_ns = 0;
	    buf->b_mtime_read = 0;
	    buf->b_mtime_read_ns = 0;
	    buf->b_orig_سائز = 0;
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
    ساکن خالی
سیٹ_b0_dir_flag(ZERO_BL *b0p, buf_T *buf)
{
    اگر (same_directory(buf->b_ml.ml_mfp->mf_fname, buf->b_ffname))
	b0p->b0_flags |= B0_SAME_DIR;
    ورنہ
	b0p->b0_flags &= ~B0_SAME_DIR;
}

/*
 * When there is room, add the 'fileencoding' to block zero.
 */
    ساکن خالی
جوڑو_b0_fenc(
    ZERO_BL	*b0p,
    buf_T	*buf)
{
    صحیح		n;
    صحیح		سائز = B0_FNAME_سائز_NOCRYPT;

#اگر_تعریف_شدہ FEAT_CRYPT
    // Without encryption use the same offset as in Vim 7.2 to be compatible.
    // With encryption it's OK to move elsewhere, the swap file is not
    // compatible anyway.
    اگر (*buf->b_p_کلید != NUL)
	سائز = B0_FNAME_سائز_CRYPT;
#اگر_ختم

    n = (صحیح)STRLEN(buf->b_p_fenc);
    اگر ((صحیح)STRLEN(b0p->b0_fname) + n + 1 > سائز)
	b0p->b0_flags &= ~B0_HAS_FENC;
    ورنہ
    {
	mch_memmove((حرف *)b0p->b0_fname + سائز - n,
					    (حرف *)buf->b_p_fenc, (size_t)n);
	*(b0p->b0_fname + سائز - n - 1) = NUL;
	b0p->b0_flags |= B0_HAS_FENC;
    }
}

#اگر defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO_UPTIME)
# include <sys/sysinfo.h>
#اگر_ختم

#اگر defined(UNIX) || defined(MSWIN)
/*
 * Return TRUE if the process with number "b0p->b0_pid" is still running.
 * "swap_fname" is the name of the swap file, if it's from before a reboot then
 * the result is FALSE;
 */
    ساکن صحیح
swapfile_process_running(ZERO_BL *b0p, char_u *تبادلہ_fname غیر_مستعمل)
{
# اگر defined(HAVE_SYSINFO) && defined(HAVE_SYSINFO_UPTIME)
    stat_T	    st;
    ساخت sysinfo  sinfo;

    // If the system rebooted after when the swap file was written then the
    // process can't be running now.
    اگر (mch_stat((حرف *)تبادلہ_fname, &st) != -1
	    && sysinfo(&sinfo) == 0
	    && st.st_mtime < وقت(تہی) - (
#  ifdef FEAT_EVAL
		override_sysinfo_uptime >= 0 ? override_sysinfo_uptime :
#  endif
		sinfo.uptime))
	واپس FALSE;
# endif
    واپس mch_process_running(char_مقصد_long(b0p->b0_pid));
}
#اگر_ختم

/*
 * Try to recover curbuf from the .swp file.
 * If "checkext" is TRUE, check the extension and detect whether it is
 * a swap file.
 */
    خالی
ml_recover(صحیح checkext)
{
    buf_T	*buf = تہی;
    memfile_T	*mfp = تہی;
    char_u	*fname;
    char_u	*fname_مستعمل = تہی;
    bhdr_T	*hp = تہی;
    ZERO_BL	*b0p;
    صحیح		b0_ff;
    char_u	*b0_fenc = تہی;
#اگر_تعریف_شدہ FEAT_CRYPT
    صحیح		b0_cm = -1;
#اگر_ختم
    PTR_BL	*pp;
    ڈیٹا_BL	*dp;
    infoptr_T	*ip;
    blocknr_T	bnum;
    صحیح		صفحہ_گنتی;
    stat_T	org_stat, swp_stat;
    صحیح		len;
    صحیح		directly;
    linenr_T	lnum;
    char_u	*p;
    صحیح		i;
    طویل	error;
    صحیح		cannot_open;
    linenr_T	سطر_گنتی;
    صحیح		has_error;
    صحیح		idx;
    صحیح		چوٹی;
    صحیح		txt_آغاز;
    off_T	سائز;
    صحیح		called_from_main;
    صحیح		serious_error = TRUE;
    طویل	mtime;
    صحیح		attr;
    صحیح		orig_file_حالت = NOTDONE;

    recoverymode = TRUE;
    called_from_main = (curbuf->b_ml.ml_mfp == تہی);
    attr = HL_ATTR(HLF_E);

    /*
     * If the file name ends in ".s[a-w][a-z]" we assume this is the swap file.
     * Otherwise a search is done to find the swap file(s).
     */
    fname = curbuf->b_fname;
    اگر (fname == تہی)		    // When there is no file name
	fname = (char_u *)"";
    len = (صحیح)STRLEN(fname);
    اگر (checkext && len >= 4 &&
#اگر defined(VMS)
	    STRNICMP(fname + len - 4, "_s", 2)
#ورنہ
	    STRNICMP(fname + len - 4, ".s", 2)
#اگر_ختم
						== 0
		&& vim_strchr((char_u *)"abcdefghijklmnopqrstuvw",
					   TOLOWER_ASC(fname[len - 2])) != تہی
		&& ASCII_ISALPHA(fname[len - 1]))
    {
	directly = TRUE;
	fname_مستعمل = vim_strsave(fname); // make a copy for mf_open()
    }
    ورنہ
    {
	directly = FALSE;

	// count the number of matching swap files
	len = recover_نام(fname, FALSE, تہی, 0, تہی);
	اگر (len == 0)		    // no swap files found
	{
	    semsg(_(e_no_تبادلہ_file_ملا_for_str), fname);
	    جاؤ theend;
	}
	اگر (len == 1)		    // one swap file found, use it
	    i = 1;
	ورنہ			    // several swap files found, choose
	{
	    // list the names of the swap files
	    (خالی)recover_نام(fname, TRUE, تہی, 0, تہی);
	    پیغام_putchar('\n');
	    پیغام_puts(_("Enter number of swap file to use (0 to quit): "));
	    i = get_نمبر(FALSE, تہی);
	    اگر (i < 1 || i > len)
		جاؤ theend;
	}
	// get the swap file name that will be used
	(خالی)recover_نام(fname, FALSE, تہی, i, &fname_مستعمل);
    }
    اگر (fname_مستعمل == تہی)
	جاؤ theend;			// out of memory

    // When called from main() still need to initialize storage structure
    اگر (called_from_main && ml_open(curbuf) == FAIL)
	getout(1);

    /*
     * Allocate a buffer structure for the swap file that is used for recovery.
     * Only the memline and crypt information in it are really used.
     */
    buf = ALLOC_ONE(buf_T);
    اگر (buf == تہی)
	جاؤ theend;

    /*
     * init fields in memline struct
     */
    buf->b_ml.ml_ڈھیر_سائز = 0;	// no stack yet
    buf->b_ml.ml_ڈھیر = تہی;		// no stack yet
    buf->b_ml.ml_ڈھیر_چوٹی = 0;		// nothing in the stack
    buf->b_ml.ml_سطر_lnum = 0;		// no cached line
    buf->b_ml.ml_locked = تہی;		// no locked block
    buf->b_ml.ml_flags = 0;
#اگر_تعریف_شدہ FEAT_CRYPT
    buf->b_p_کلید = خالی_خانے_option;
    buf->b_p_cm = خالی_خانے_option;
#اگر_ختم

    /*
     * open the memfile from the old swap file
     */
    p = vim_strsave(fname_مستعمل); // save "fname_used" for the message:
				 // mf_open() will consume "fname_used"!
    mfp = mf_open(fname_مستعمل, O_RDONLY);
    fname_مستعمل = p;
    اگر (mfp == تہی || mfp->mf_fd < 0)
    {
	اگر (fname_مستعمل != تہی)
	    semsg(_(e_cannot_open_str), fname_مستعمل);
	جاؤ theend;
    }
    buf->b_ml.ml_mfp = mfp;
#اگر_تعریف_شدہ FEAT_CRYPT
    mfp->mf_بفر = buf;
#اگر_ختم

    /*
     * The page size set in mf_open() might be different from the page size
     * used in the swap file, we must get it from block 0.  But to read block
     * 0 we need a page size.  Use the minimal size for block 0 here, it will
     * be set to the real value below.
     */
    mfp->mf_صفحہ_سائز = MIN_تبادلہ_صفحہ_سائز;

    /*
     * try to read block 0
     */
    اگر ((hp = mf_get(mfp, (blocknr_T)0, 1)) == تہی)
    {
	پیغام_آغاز();
	پیغام_puts_attr(_("Unable to read block 0 from "), attr | پیغام_HIST);
	پیغام_outtrans_attr(mfp->mf_fname, attr | پیغام_HIST);
	پیغام_puts_attr(_("\nMaybe no changes were made or Vim did not update the swap file."),
		attr | پیغام_HIST);
	پیغام_انتہا();
	جاؤ theend;
    }
    b0p = (ZERO_BL *)(hp->bh_ڈیٹا);
    اگر (STRNCMP(b0p->b0_نسخہ, "VIM 3.0", 7) == 0)
    {
	پیغام_آغاز();
	پیغام_outtrans_attr(mfp->mf_fname, پیغام_HIST);
	پیغام_puts_attr(_(" cannot be used with this version of Vim.\n"),
								    پیغام_HIST);
	پیغام_puts_attr(_("Use Vim version 3.0.\n"), پیغام_HIST);
	پیغام_انتہا();
	جاؤ theend;
    }
    اگر (ml_check_b0_id(b0p) == FAIL)
    {
	semsg(_(e_str_does_not_look_like_vim_تبادلہ_file), mfp->mf_fname);
	جاؤ theend;
    }
    اگر (b0_magic_wrong(b0p))
    {
	پیغام_آغاز();
	پیغام_outtrans_attr(mfp->mf_fname, attr | پیغام_HIST);
#اگر defined(MSWIN)
	اگر (STRNCMP(b0p->b0_hname, "PC ", 3) == 0)
	    پیغام_puts_attr(_(" cannot be used with this version of Vim.\n"),
							     attr | پیغام_HIST);
	ورنہ
#اگر_ختم
	    پیغام_puts_attr(_(" cannot be used on this computer.\n"),
							     attr | پیغام_HIST);
	پیغام_puts_attr(_("The file was created on "), attr | پیغام_HIST);
	// avoid going past the end of a corrupted hostname
	b0p->b0_fname[0] = NUL;
	پیغام_puts_attr((حرف *)b0p->b0_hname, attr | پیغام_HIST);
	پیغام_puts_attr(_(",\nor the file has been damaged."), attr | پیغام_HIST);
	پیغام_انتہا();
	جاؤ theend;
    }

#اگر_تعریف_شدہ FEAT_CRYPT
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(id1_codes); ++i)
	اگر (id1_codes[i] == b0p->b0_id[1])
	    b0_cm = i;
    اگر (b0_cm > 0)
	mch_memmove(mfp->mf_بیج, &b0p->b0_بیج, MF_بیج_LEN);
    crypt_سیٹ_cm_option(buf, b0_cm < 0 ? 0 : b0_cm);
#ورنہ
    اگر (b0p->b0_id[1] != BLOCK0_ID1)
    {
	semsg(_(e_str_is_encrypted_and_this_نسخہ_کا_vim_does_not_support_encryption), mfp->mf_fname);
	جاؤ theend;
    }
#اگر_ختم

    /*
     * If we guessed the wrong page size, we have to recalculate the
     * highest block number in the file.
     */
    اگر (mfp->mf_صفحہ_سائز != (غیرعلامتی)char_مقصد_long(b0p->b0_صفحہ_سائز))
    {
	غیرعلامتی پچھلا_صفحہ_سائز = mfp->mf_صفحہ_سائز;

	mf_new_صفحہ_سائز(mfp, (غیرعلامتی)char_مقصد_long(b0p->b0_صفحہ_سائز));
	اگر (mfp->mf_صفحہ_سائز < پچھلا_صفحہ_سائز)
	{
	    پیغام_آغاز();
	    پیغام_outtrans_attr(mfp->mf_fname, attr | پیغام_HIST);
	    پیغام_puts_attr(_(" has been damaged (page size is smaller than minimum value).\n"),
			attr | پیغام_HIST);
	    پیغام_انتہا();
	    جاؤ theend;
	}
	اگر ((سائز = vim_lseek(mfp->mf_fd, (off_T)0L, SEEK_END)) <= 0)
	    mfp->mf_blocknr_max = 0;	    // no file or empty file
	ورنہ
	    mfp->mf_blocknr_max = (blocknr_T)(سائز / mfp->mf_صفحہ_سائز);
	mfp->mf_infile_گنتی = mfp->mf_blocknr_max;

	// need to reallocate the memory used to store the data
	p = alloc(mfp->mf_صفحہ_سائز);
	اگر (p == تہی)
	    جاؤ theend;
	mch_memmove(p, hp->bh_ڈیٹا, پچھلا_صفحہ_سائز);
	vim_free(hp->bh_ڈیٹا);
	hp->bh_ڈیٹا = p;
	b0p = (ZERO_BL *)(hp->bh_ڈیٹا);
    }

    /*
     * If .swp file name given directly, use name from swap file for buffer.
     */
    اگر (directly)
    {
	expand_env(b0p->b0_fname, NameBuff, MAXPATHL);
	اگر (setfname(curbuf, NameBuff, تہی, TRUE) == FAIL)
	    جاؤ theend;
    }

    home_replace(تہی, mfp->mf_fname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Using swap file \"%s\""), NameBuff);

    اگر (buf_spname(curbuf) != تہی)
	vim_strncpy(NameBuff, buf_spname(curbuf), MAXPATHL - 1);
    ورنہ
	home_replace(تہی, curbuf->b_ffname, NameBuff, MAXPATHL, TRUE);
    smsg(_("Original file \"%s\""), NameBuff);
    پیغام_putchar('\n');

    /*
     * check date of swap file and original file
     */
    mtime = char_مقصد_long(b0p->b0_mtime);
    اگر (curbuf->b_ffname != تہی
	    && mch_stat((حرف *)curbuf->b_ffname, &org_stat) != -1
	    && ((mch_stat((حرف *)mfp->mf_fname, &swp_stat) != -1
		    && org_stat.st_mtime > swp_stat.st_mtime)
		|| org_stat.st_mtime != mtime))
	emsg(_(e_warning_original_file_may_have_been_بدلا));
    برآمد_flush();

    // Get the 'fileformat' and 'fileencoding' from block zero.
    b0_ff = (b0p->b0_flags & B0_FF_نقاب);
    اگر (b0p->b0_flags & B0_HAS_FENC)
    {
	صحیح fnsize = B0_FNAME_سائز_NOCRYPT;

#اگر_تعریف_شدہ FEAT_CRYPT
	// Use the same size as in add_b0_fenc().
	اگر (b0p->b0_id[1] != BLOCK0_ID1)
	    fnsize = B0_FNAME_سائز_CRYPT;
#اگر_ختم
	برائے (p = b0p->b0_fname + fnsize; p > b0p->b0_fname && p[-1] != NUL; --p)
	    ;
	b0_fenc = vim_strnsave(p, b0p->b0_fname + fnsize - p);
    }

    mf_put(mfp, hp, FALSE, FALSE);	// release block 0
    hp = تہی;

    /*
     * Now that we are sure that the file is going to be recovered, clear the
     * contents of the current buffer.
     */
    جب (!(curbuf->b_ml.ml_flags & ML_خالی_خانے))
	ml_delete((linenr_T)1);

    /*
     * Try reading the original file to obtain the values of 'fileformat',
     * 'fileencoding', etc.  Ignore errors.  The text itself is not used.
     * When the file is encrypted the user is asked to enter the key.
     */
    اگر (curbuf->b_ffname != تہی)
	orig_file_حالت = readfile(curbuf->b_ffname, تہی, (linenr_T)0,
			      (linenr_T)0, (linenr_T)MAXLNUM, تہی, READ_NEW);

#اگر_تعریف_شدہ FEAT_CRYPT
    اگر (b0_cm >= 0)
    {
	// Need to ask the user for the crypt key.  If this fails we continue
	// without a key, will probably get garbage text.
	اگر (*curbuf->b_p_کلید != NUL)
	{
	    smsg(_("Swap file is encrypted: \"%s\""), fname_مستعمل);
	    پیغام_puts(_("\nIf you entered a new crypt key but did not write the text file,"));
	    پیغام_puts(_("\nenter the new crypt key."));
	    پیغام_puts(_("\nIf you wrote the text file after changing the crypt key press enter"));
	    پیغام_puts(_("\nto use the same key for text file and swap file"));
	}
	ورنہ
	    smsg(_(need_کلید_پیغام), fname_مستعمل);
	buf->b_p_کلید = crypt_get_کلید(FALSE, FALSE);
	اگر (buf->b_p_کلید == تہی)
	    buf->b_p_کلید = curbuf->b_p_کلید;
	ورنہ اگر (*buf->b_p_کلید == NUL)
	{
	    vim_free(buf->b_p_کلید);
	    buf->b_p_کلید = curbuf->b_p_کلید;
	}
	اگر (buf->b_p_کلید == تہی)
	    buf->b_p_کلید = خالی_خانے_option;
    }
#اگر_ختم

    // Use the 'fileformat' and 'fileencoding' as stored in the swap file.
    اگر (b0_ff != 0)
	سیٹ_fileformat(b0_ff - 1, OPT_LOCAL);
    اگر (b0_fenc != تہی)
    {
	سیٹ_option_قدر_give_err((char_u *)"fenc", 0L, b0_fenc, OPT_LOCAL);
	vim_free(b0_fenc);
    }
    unchanged(curbuf, TRUE, TRUE);

    bnum = 1;		// start with block 1
    صفحہ_گنتی = 1;	// which is 1 page
    lnum = 0;		// append after line 0 in curbuf
    سطر_گنتی = 0;
    idx = 0;		// start with first index in block 1
    error = 0;
    buf->b_ml.ml_ڈھیر_چوٹی = 0;
    buf->b_ml.ml_ڈھیر = تہی;
    buf->b_ml.ml_ڈھیر_سائز = 0;	// no stack yet

    اگر (curbuf->b_ffname == تہی)
	cannot_open = TRUE;
    ورنہ
	cannot_open = FALSE;

    serious_error = FALSE;
    برائے ( ; !got_int; سطر_breakcheck())
    {
	اگر (hp != تہی)
	    mf_put(mfp, hp, FALSE, FALSE);	// release previous block

	/*
	 * get block
	 */
	اگر ((hp = mf_get(mfp, bnum, صفحہ_گنتی)) == تہی)
	{
	    اگر (bnum == 1)
	    {
		semsg(_(e_unable_مقصد_read_بلاک_one_from_str), mfp->mf_fname);
		جاؤ theend;
	    }
	    ++error;
	    ml_append(lnum++, (char_u *)_("???MANY LINES MISSING"),
							    (colnr_T)0, TRUE);
	}
	ورنہ		// there is a block
	{
	    pp = (PTR_BL *)(hp->bh_ڈیٹا);
	    اگر (pp->pb_id == PTR_ID)		// it is a pointer block
	    {
		صحیح ptr_بلاک_error = FALSE;
		اگر (pp->pb_گنتی_max != PB_گنتی_MAX(mfp))
		{
		    ptr_بلاک_error = TRUE;
		    pp->pb_گنتی_max = PB_گنتی_MAX(mfp);
		}
		اگر (pp->pb_گنتی > pp->pb_گنتی_max)
		{
		    ptr_بلاک_error = TRUE;
		    pp->pb_گنتی = pp->pb_گنتی_max;
		}
		اگر (ptr_بلاک_error)
		    emsg(_(e_warning_اشارہ_گر_بلاک_corrupted));

		// check line count when using pointer block first time
		اگر (idx == 0 && سطر_گنتی != 0)
		{
		    برائے (i = 0; i < (صحیح)pp->pb_گنتی; ++i)
			سطر_گنتی -= pp->pb_اشارہ_گر[i].pe_سطر_گنتی;
		    اگر (سطر_گنتی != 0)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("???LINE COUNT WRONG"),
							    (colnr_T)0, TRUE);
		    }
		}

		اگر (pp->pb_گنتی == 0)
		{
		    ml_append(lnum++, (char_u *)_("???EMPTY BLOCK"),
							    (colnr_T)0, TRUE);
		    ++error;
		}
		ورنہ اگر (idx < (صحیح)pp->pb_گنتی)	// go a block deeper
		{
		    اگر (pp->pb_اشارہ_گر[idx].pe_bnum < 0)
		    {
			/*
			 * Data block with negative block number.
			 * Try to read lines from the original file.
			 * This is slow, but it works.
			 */
			اگر (!cannot_open)
			{
			    سطر_گنتی = pp->pb_اشارہ_گر[idx].pe_سطر_گنتی;
			    linenr_T pe_old_lnum = pp->pb_اشارہ_گر[idx].pe_old_lnum;
			    // Validate pe_line_count and pe_old_lnum from the
			    // untrusted swap file before passing to readfile().
			    اگر (سطر_گنتی <= 0 || pe_old_lnum < 1 ||
				    readfile(curbuf->b_ffname, تہی, lnum,
					pe_old_lnum - 1,
					سطر_گنتی, تہی, 0) != OK)
				cannot_open = TRUE;
			    ورنہ
				lnum += سطر_گنتی;
			}
			اگر (cannot_open)
			{
			    ++error;
			    ml_append(lnum++, (char_u *)_("???LINES MISSING"),
							    (colnr_T)0, TRUE);
			}
			++idx;	    // get same block again for next index
			جاری;
		    }

		    /*
		     * going one block deeper in the tree
		     */
		    اگر ((چوٹی = ml_جوڑو_ڈھیر(buf)) < 0)	// new entry in stack
		    {
			++error;
			توڑو;		    // out of memory
		    }
		    ip = &(buf->b_ml.ml_ڈھیر[چوٹی]);
		    ip->ip_bnum = bnum;
		    ip->ip_اشاریہ = idx;

		    bnum = pp->pb_اشارہ_گر[idx].pe_bnum;
		    سطر_گنتی = pp->pb_اشارہ_گر[idx].pe_سطر_گنتی;
		    صفحہ_گنتی = pp->pb_اشارہ_گر[idx].pe_صفحہ_گنتی;
		    // Validate pe_bnum and pe_page_count from the untrusted
		    // swap file before passing to mf_get(), which uses
		    // page_count to calculate allocation size.  A bogus value
		    // (e.g. 0x40000000) would cause a multi-GB allocation.
		    // pe_page_count must be >= 1 and bnum + page_count must
		    // not exceed the number of pages in the swap file.
		    اگر (صفحہ_گنتی < 1
			    || bnum + صفحہ_گنتی > mfp->mf_blocknr_max + 1)
		    {
			++error;
			ml_append(lnum++,
				(char_u *)_("???ILLEGAL BLOCK NUMBER"),
				(colnr_T)0, TRUE);
			// Skip this entry and pop back up the stack to keep
			// recovering whatever else we can.
			idx = ip->ip_اشاریہ + 1;
			bnum = ip->ip_bnum;
			صفحہ_گنتی = 1;
			--buf->b_ml.ml_ڈھیر_چوٹی;
			جاری;
		    }
		    idx = 0;
		    جاری;
		}
	    }
	    ورنہ	    // not a pointer block
	    {
		dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
		اگر (dp->db_id != ڈیٹا_ID)	// block id wrong
		{
		    اگر (bnum == 1)
		    {
			semsg(_(e_بلاک_one_id_wrong_str_not_swp_file),
							       mfp->mf_fname);
			جاؤ theend;
		    }
		    ++error;
		    ml_append(lnum++, (char_u *)_("???BLOCK MISSING"),
							    (colnr_T)0, TRUE);
		}
		ورنہ
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
		    اگر (hp->bh_صفحہ_گنتی != صفحہ_گنتی)
		    {
			++error;
			ml_append(lnum++, (char_u *)_("??? BLOCK PAGE COUNT MISMATCH"),
							    (colnr_T)0, TRUE);
			صفحہ_گنتی = hp->bh_صفحہ_گنتی;
		    }

		    // Check the length of the block.
		    // If wrong, use the length given in the pointer block.
		    اگر (صفحہ_گنتی * mfp->mf_صفحہ_سائز != dp->db_txt_انتہا)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may be messed up"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_انتہا = صفحہ_گنتی * mfp->mf_صفحہ_سائز;
		    }

		    اگر (dp->db_txt_آغاز < سرخی_سائز
			    || dp->db_txt_آغاز > dp->db_txt_انتہا)
		    {
			ml_append(lnum++, (char_u *)_("??? block header corrupted"),
								    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
			dp->db_txt_آغاز = dp->db_txt_انتہا;
		    }
		    // Make sure there is a NUL at the end of the block so we
		    // don't go over the end when copying text.
		    *((char_u *)dp + dp->db_txt_انتہا - 1) = NUL;

		    // Check the number of lines in the block.
		    // If wrong, use the count in the data block.
		    اگر (سطر_گنتی != dp->db_سطر_گنتی)
		    {
			ml_append(lnum++, (char_u *)_("??? from here until ???END lines may have been inserted/deleted"),
							    (colnr_T)0, TRUE);
			++error;
			has_error = TRUE;
		    }

		    صحیح did_questions = FALSE;
		    برائے (i = 0; i < dp->db_سطر_گنتی; ++i)
		    {
			اگر ((char_u *)&(dp->db_اشاریہ[i])
					    >= (char_u *)dp + dp->db_txt_آغاز)
			{
			    // line count must be wrong
			    ++error;
			    ml_append(lnum++,
				    (char_u *)_("??? lines may be missing"),
							     (colnr_T)0, TRUE);
			    توڑو;
			}

			txt_آغاز = (dp->db_اشاریہ[i] & DB_اشاریہ_نقاب);
			اگر (txt_آغاز <= (صحیح)سرخی_سائز
					  || txt_آغاز >= (صحیح)dp->db_txt_انتہا)
			{
			    ++error;
			    // avoid lots of lines with "???"
			    اگر (did_questions)
				جاری;
			    did_questions = TRUE;
			    p = (char_u *)"???";
			}
			ورنہ
			{
			    did_questions = FALSE;
			    p = (char_u *)dp + txt_آغاز;
			}
			ml_append(lnum++, p, (colnr_T)0, TRUE);
		    }
		    اگر (has_error)
			ml_append(lnum++, (char_u *)_("???END"),
							    (colnr_T)0, TRUE);
		}
	    }
	}

	اگر (buf->b_ml.ml_ڈھیر_چوٹی == 0)	//ختم_شدہ
	    توڑو;

	/*
	 * go one block up in the tree
	 */
	ip = &(buf->b_ml.ml_ڈھیر[--(buf->b_ml.ml_ڈھیر_چوٹی)]);
	bnum = ip->ip_bnum;
	idx = ip->ip_اشاریہ + 1;	    // go to next index
	صفحہ_گنتی = 1;
    }

    /*
     * Compare the buffer contents with the original file.  When they differ
     * set the 'modified' flag.
     * Lines 1 - lnum are the new contents.
     * Lines lnum + 1 to ml_line_count are the original contents.
     * Line ml_line_count + 1 in the dummy empty line.
     */
    اگر (orig_file_حالت != OK || curbuf->b_ml.ml_سطر_گنتی != lnum * 2 + 1)
    {
	// Recovering an empty file results in two lines and the first line is
	// empty.  Don't set the modified flag then.
	اگر (!(curbuf->b_ml.ml_سطر_گنتی == 2 && *ml_get(1) == NUL))
	{
	    بدلا_اندرونی();
	    ++CHANGEDTICK(curbuf);
	}
    }
    ورنہ
    {
	برائے (idx = 1; idx <= lnum; ++idx)
	{
	    // Need to copy one line, fetching the other one may flush it.
	    p = vim_strnsave(ml_get(idx), ml_get_len(idx));
	    i = STRCMP(p, ml_get(idx + lnum));
	    vim_free(p);
	    اگر (i != 0)
	    {
		بدلا_اندرونی();
		++CHANGEDTICK(curbuf);
		توڑو;
	    }
	}
    }

    /*
     * Delete the lines from the original file and the dummy line from the
     * empty buffer.  These will now be after the last line in the buffer.
     */
    جب (curbuf->b_ml.ml_سطر_گنتی > lnum
				       && !(curbuf->b_ml.ml_flags & ML_خالی_خانے))
	ml_delete(curbuf->b_ml.ml_سطر_گنتی);
    curbuf->b_flags |= BF_RECOVERED;
    check_cursor();

    recoverymode = FALSE;
    اگر (got_int)
	emsg(_(e_recovery_interrupted));
    ورنہ اگر (error)
    {
	++no_wait_return;
	پیغام(">>>>>>>>>>>>>");
	emsg(_(e_errors_detected_while_recovering_look_for_سطریں_starting_with_questions));
	--no_wait_return;
	پیغام(_("See \":help E312\" for more information."));
	پیغام(">>>>>>>>>>>>>");
    }
    ورنہ
    {
	اگر (curbuf->b_بدلا)
	{
	    پیغام(_("Recovery completed. You should check if everything is OK."));
	    پیغام_puts(_("\n(You might want to write out this file under another name\n"));
	    پیغام_puts(_("and run diff with the original file to check for changes)"));
	}
	ورنہ
	    پیغام(_("Recovery completed. Buffer contents equals file contents."));
	پیغام_puts(_("\nYou may want to delete the .swp file now."));
#اگر defined(UNIX) || defined(MSWIN)
	اگر (swapfile_process_running(b0p, fname_مستعمل))
	{
	    // Warn there could be an active Vim on the same file, the user may
	    // want to kill it.
	    پیغام_puts(_("\nNote: process STILL RUNNING: "));
	    پیغام_outnum(char_مقصد_long(b0p->b0_pid));
	}
#اگر_ختم
	پیغام_puts("\n\n");
	cmdline_صف = پیغام_صف;
    }
#اگر_تعریف_شدہ FEAT_CRYPT
    اگر (*buf->b_p_کلید != NUL && STRCMP(curbuf->b_p_کلید, buf->b_p_کلید) != 0)
    {
	پیغام_puts(_("Using crypt key from swap file for the text file.\n"));
	سیٹ_option_قدر_give_err((char_u *)"key", 0L, buf->b_p_کلید, OPT_LOCAL);
    }
#اگر_ختم
    redraw_curbuf_later(UPD_NOT_VALID);

theend:
    vim_free(fname_مستعمل);
    recoverymode = FALSE;
    اگر (mfp != تہی)
    {
	اگر (hp != تہی)
	    mf_put(mfp, hp, FALSE, FALSE);
	mf_close(mfp, FALSE);	    // will also vim_free(mfp->mf_fname)
    }
    اگر (buf != تہی)
    {
#اگر_تعریف_شدہ FEAT_CRYPT
	اگر (buf->b_p_کلید != curbuf->b_p_کلید)
	    free_string_option(buf->b_p_کلید);
	free_string_option(buf->b_p_cm);
#اگر_ختم
	vim_free(buf->b_ml.ml_ڈھیر);
	vim_free(buf);
    }
    اگر (serious_error && called_from_main)
	ml_close(curbuf, TRUE);
    ورنہ
    {
	apply_autocmds(EVENT_BUFREADPOST, تہی, curbuf->b_fname, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, تہی, curbuf->b_fname, FALSE, curbuf);
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
    صحیح
recover_نام(
    char_u	*fname,		// base for swap file name
    صحیح		do_list,	// when TRUE, list the swap file names
    list_T	*ret_list غیر_مستعمل, // when not NULL add file names to it
    صحیح		nr,		// when non-zero, return nr'th swap file name
    char_u	**fname_برآمد)	// result when "nr" > 0
{
    صحیح		عدد_نام;
    char_u	*(نام[6]);
    char_u	*p;
    صحیح		عدد_files;
    صحیح		file_گنتی = 0;
    char_u	**files;
    char_u	*dirp;
    string_T	dir_name;
    char_u	*fname_res = تہی;
#اگر_تعریف_شدہ HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
#اگر_ختم

    اگر (fname != تہی)
    {
#اگر_تعریف_شدہ HAVE_READLINK
	// Expand symlink in the file name, because the swap file is created
	// with the actual file instead of with the symlink.
	اگر (resolve_symlink(fname, fname_buf) == OK)
	    fname_res = fname_buf;
	ورنہ
#اگر_ختم
	    fname_res = fname;
    }

    اگر (do_list)
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
    جب (dir_name.string != تہی && *dirp)
    {
	/*
	 * Isolate a directory name from *dirp and put it in dir_name (we know
	 * it is large enough, so use 31000 for length).
	 * Advance dirp to next directory name.
	 */
	dir_name.لمبائی = (size_t)copy_option_part(&dirp, dir_name.string, 31000, ",");

	اگر (dir_name.string[0] == '.' && dir_name.string[1] == NUL)	// check current dir
	{
	    اگر (fname == تہی)
	    {
#اگر_تعریف_شدہ VMS
		نام[0] = vim_strnsave((char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"));
#ورنہ
		نام[0] = vim_strnsave((char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"));
#اگر_ختم
#اگر defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		نام[1] = vim_strnsave((char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"));
		نام[2] = vim_strnsave((char_u *)".sw?", STRLEN_LITERAL(".sw?"));
		عدد_نام = 3;
#ورنہ
# ifdef VMS
		نام[1] = vim_strnsave((char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"));
		عدد_نام = 2;
# ورنہ
		عدد_نام = 1;
# endif
#اگر_ختم
	    }
	    ورنہ
		عدد_نام = recov_file_نام(نام, fname_res, TRUE);
	}
	ورنہ			    // check directory dir_name
	{
	    اگر (fname == تہی)
	    {
		string_T    ret;

#اگر_تعریف_شدہ VMS
		نام[0] = concat_fnames(dir_name.string, dir_name.لمبائی,
		    (char_u *)"*_sw%", STRLEN_LITERAL("*_sw%"), TRUE, &ret);
#ورنہ
		نام[0] = concat_fnames(dir_name.string, dir_name.لمبائی,
		    (char_u *)"*.sw?", STRLEN_LITERAL("*.sw?"), TRUE, &ret);
#اگر_ختم
#اگر defined(UNIX) || defined(MSWIN)
		// For Unix names starting with a dot are special.  MS-Windows
		// supports this too, on some file systems.
		نام[1] = concat_fnames(dir_name.string, dir_name.لمبائی,
		    (char_u *)".*.sw?", STRLEN_LITERAL(".*.sw?"), TRUE, &ret);
		نام[2] = concat_fnames(dir_name.string, dir_name.لمبائی,
		    (char_u *)".sw?", STRLEN_LITERAL(".sw?"), TRUE, &ret);
		عدد_نام = 3;
#ورنہ
# ifdef VMS
		نام[1] = concat_fnames(dir_name.string, dir_name.لمبائی,
		    (char_u *)".*_sw%", STRLEN_LITERAL(".*_sw%"), TRUE, &ret);
		عدد_نام = 2;
# ورنہ
		عدد_نام = 1;
# endif
#اگر_ختم
	    }
	    ورنہ
	    {
		char_u	*دم;

#اگر defined(UNIX) || defined(MSWIN)
		p = dir_name.string + dir_name.لمبائی;
		اگر (after_pathsep(dir_name.string, p) && dir_name.لمبائی > 1 && p[-1] == p[-2])
		{
		    // Ends with '//', Use Full path for swap name
		    دم = make_percent_swname(dir_name.string, p, fname_res);
		}
		ورنہ
#اگر_ختم
		{
		    string_T	ret;

		    دم = gettail(fname_res);
		    دم = concat_fnames(dir_name.string, dir_name.لمبائی,
			دم, STRLEN(دم), TRUE, &ret);
		}
		اگر (دم == تہی)
		    عدد_نام = 0;
		ورنہ
		{
		    عدد_نام = recov_file_نام(نام, دم, FALSE);
		    vim_free(دم);
		}
	    }
	}

	// check for out-of-memory
	برائے (صحیح i = 0; i < عدد_نام; ++i)
	{
	    اگر (نام[i] == تہی)
	    {
		برائے (i = 0; i < عدد_نام; ++i)
		    vim_free(نام[i]);
		عدد_نام = 0;
	    }
	}
	اگر (عدد_نام == 0)
	    عدد_files = 0;
	ورنہ اگر (expand_wildcards(عدد_نام, نام, &عدد_files, &files,
			    EW_NOTENV|EW_KEEPALL|EW_FILE|EW_SILENT) == FAIL)
	    عدد_files = 0;

	/*
	 * When no swap file found, wildcard expansion might have failed (e.g.
	 * not able to execute the shell).
	 * Try finding a swap file by simply adding ".swp" to the file name.
	 */
	اگر (*dirp == NUL && file_گنتی + عدد_files == 0 && fname != تہی)
	{
	    stat_T	    st;
	    char_u	    *swapname;

	    swapname = modname(fname_res,
#اگر defined(VMS)
			       (char_u *)"_swp", FALSE
#ورنہ
			       (char_u *)".swp", TRUE
#اگر_ختم
			      );
	    اگر (swapname != تہی)
	    {
		اگر (mch_stat((حرف *)swapname, &st) != -1)    // It exists!
		{
		    files = ALLOC_ONE(char_u *);
		    اگر (files != تہی)
		    {
			files[0] = swapname;
			swapname = تہی;
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
	اگر (curbuf->b_ml.ml_mfp != تہی
			       && (p = curbuf->b_ml.ml_mfp->mf_fname) != تہی
			       && ret_list == تہی)
	{
	    برائے (صحیح i = 0; i < عدد_files; ++i)
		// Do not expand wildcards, on windows would try to expand
		// "%tmp%" in "%tmp%file".
		اگر (fullpathcmp(p, files[i], TRUE, FALSE) & FPC_SAME)
		{
		    // Remove the name from files[i].  Move further entries
		    // down.  When the array becomes empty free it here, since
		    // FreeWild() won't be called below.
		    vim_free(files[i]);
		    اگر (--عدد_files == 0)
			vim_free(files);
		    ورنہ
			برائے ( ; i < عدد_files; ++i)
			    files[i] = files[i + 1];
		}
	}
	اگر (nr > 0)
	{
	    file_گنتی += عدد_files;
	    اگر (nr <= file_گنتی)
	    {
		*fname_برآمد = vim_strsave(
				      files[nr - 1 + عدد_files - file_گنتی]);
		dirp = (char_u *)"";		    // stop searching
	    }
	}
	ورنہ اگر (do_list)
	{
	    اگر (dir_name.string[0] == '.' && dir_name.string[1] == NUL)
	    {
		اگر (fname == تہی)
		    پیغام_puts(_("   In current directory:\n"));
		ورنہ
		    پیغام_puts(_("   Using specified name:\n"));
	    }
	    ورنہ
	    {
		پیغام_puts(_("   In directory "));
		پیغام_home_replace(dir_name.string);
		پیغام_puts(":\n");
	    }

	    اگر (عدد_files)
	    {
		برائے (صحیح i = 0; i < عدد_files; ++i)
		{
		    // print the swap file name
		    پیغام_outnum((طویل)++file_گنتی);
		    پیغام_puts(".    ");
		    پیغام_puts((حرف *)gettail(files[i]));
		    پیغام_putchar('\n');
		    (خالی)swapfile_info(files[i]);
		}
	    }
	    ورنہ
		پیغام_puts(_("      -- none --\n"));
	    برآمد_flush();
	}
#اگر_تعریف_شدہ FEAT_EVAL
	ورنہ اگر (ret_list != تہی)
	{
	    string_T	name;

	    برائے (صحیح i = 0; i < عدد_files; ++i)
	    {
		concat_fnames(dir_name.string, dir_name.لمبائی,
		    files[i], STRLEN(files[i]), TRUE, &name);
		اگر (name.string != تہی)
		{
		    list_append_string(ret_list, name.string, (صحیح)name.لمبائی);
		    vim_free(name.string);
		}
	    }
	}
#اگر_ختم
	ورنہ
	    file_گنتی += عدد_files;

	برائے (صحیح i = 0; i < عدد_نام; ++i)
	    vim_free(نام[i]);
	اگر (عدد_files > 0)
	    FreeWild(عدد_files, files);
    }
    vim_free(dir_name.string);
    واپس file_گنتی;
}

#اگر defined(UNIX) || defined(MSWIN)
/*
 * Need _very_ long file names.
 * Append the full path to name with path separators made into percent
 * signs, to "dir". An unnamed buffer is handled as "" (<currentdir>/"")
 * The last character in "dir" must be an extra slash or backslash, it is
 * removed.
 */
    char_u *
make_percent_swname(char_u *dir, char_u *dir_انتہا, char_u *name)
{
    string_T	d = {تہی, 0};
    string_T	fixed_fname;
    char_u	*p;

    fixed_fname.string = fix_fname(name != تہی ? name : (char_u *)"");
    اگر (fixed_fname.string == تہی)
	واپس تہی;

    برائے (p = fixed_fname.string; *p != NUL; MB_PTR_ADV(p))
	اگر (vim_ispathsep(*p))
	    *p = '%';
    fixed_fname.لمبائی = (size_t)(p - fixed_fname.string);

    // remove one trailing slash
    p = &dir_انتہا[-1];
    *p = NUL;
    concat_fnames(dir, (size_t)(p - dir), fixed_fname.string, fixed_fname.لمبائی, TRUE, &d);
    vim_free(fixed_fname.string);

    واپس d.string;
}
#اگر_ختم

#اگر (defined(UNIX) || defined(VMS) || defined(MSWIN)) \
	&& (defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG))
# define HAVE_PROCESS_STILL_RUNNING
ساکن صحیح process_still_running;
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * Return information found in swapfile "fname" in dictionary "d".
 * This is used by the swapinfo() function.
 */
    خالی
get_b0_dict(char_u *fname, dict_T *d)
{
    صحیح fd;
    ساخت block0 b0;

    اگر ((fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0)) >= 0)
    {
	اگر (read_eintr(fd, &b0, حجم(b0)) == حجم(b0))
	{
	    اگر (ml_check_b0_id(&b0) == FAIL)
		dict_جوڑو_string_len(d, "error",
		    (char_u *)"Not a swap file", STRLEN_LITERAL("Not a swap file"));
	    ورنہ اگر (b0_magic_wrong(&b0))
		dict_جوڑو_string_len(d, "error",
		    (char_u *)"Magic number mismatch", STRLEN_LITERAL("Magic number mismatch"));
	    ورنہ
	    {
		// we have swap information
		dict_جوڑو_string_len(d, "version", b0.b0_نسخہ, 10);
		dict_جوڑو_string_len(d, "user", b0.b0_uname, B0_UNAME_سائز);
		dict_جوڑو_string_len(d, "host", b0.b0_hname, B0_HNAME_سائز);
		dict_جوڑو_string_len(d, "fname", b0.b0_fname, B0_FNAME_سائز_ORG);

		dict_جوڑو_نمبر(d, "pid", char_مقصد_long(b0.b0_pid));
		dict_جوڑو_نمبر(d, "mtime", char_مقصد_long(b0.b0_mtime));
		dict_جوڑو_نمبر(d, "dirty", b0.b0_dirty ? 1 : 0);
# ifdef CHECK_INODE
		dict_جوڑو_نمبر(d, "inode", char_مقصد_long(b0.b0_ino));
# endif
	    }
	}
	ورنہ
	    dict_جوڑو_string_len(d, "error", (char_u *)"Cannot read file", STRLEN_LITERAL("Cannot read file"));
	close(fd);
    }
    ورنہ
	dict_جوڑو_string_len(d, "error", (char_u *)"Cannot open file", STRLEN_LITERAL("Cannot open file"));
}
#اگر_ختم

/*
 * Give information about an existing swap file.
 * Returns timestamp (0 when unknown).
 */
    ساکن time_t
swapfile_info(char_u *fname)
{
    stat_T	    st;
    صحیح		    fd;
    ساخت block0   b0;
#اگر_تعریف_شدہ UNIX
    char_u	    uname[B0_UNAME_سائز];
#اگر_ختم

    // print the swap file date
    اگر (mch_stat((حرف *)fname, &st) != -1)
    {
#اگر_تعریف_شدہ UNIX
	// print name of owner of the file
	اگر (mch_get_uname(st.st_uid, uname, B0_UNAME_سائز) == OK)
	{
	    پیغام_puts(_("          owned by: "));
	    پیغام_outtrans(uname);
	    پیغام_puts(_("   dated: "));
	}
	ورنہ
#اگر_ختم
	    پیغام_puts(_("             dated: "));
	پیغام_puts(get_ctime(st.st_mtime, TRUE));
    }
    ورنہ
	st.st_mtime = 0;

    /*
     * print the original file name
     */
    fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
    اگر (fd >= 0)
    {
	اگر (read_eintr(fd, &b0, حجم(b0)) == حجم(b0))
	{
	    اگر (STRNCMP(b0.b0_نسخہ, "VIM 3.0", 7) == 0)
	    {
		پیغام_puts(_("         [from Vim version 3.0]"));
	    }
	    ورنہ اگر (ml_check_b0_id(&b0) == FAIL)
	    {
		پیغام_puts(_("         [does not look like a Vim swap file]"));
	    }
	    ورنہ
	    {
		پیغام_puts(_("         file name: "));
		اگر (b0.b0_fname[0] == NUL)
		    پیغام_puts(_("[No Name]"));
		ورنہ
		    پیغام_outtrans(b0.b0_fname);

		پیغام_puts(_("\n          modified: "));
		پیغام_puts(b0.b0_dirty ? _("YES") : _("no"));

		اگر (*(b0.b0_uname) != NUL)
		{
		    پیغام_puts(_("\n         user name: "));
		    پیغام_outtrans(b0.b0_uname);
		}

		اگر (*(b0.b0_hname) != NUL)
		{
		    اگر (*(b0.b0_uname) != NUL)
			پیغام_puts(_("   host name: "));
		    ورنہ
			پیغام_puts(_("\n         host name: "));
		    پیغام_outtrans(b0.b0_hname);
		}

		اگر (char_مقصد_long(b0.b0_pid) != 0L)
		{
		    پیغام_puts(_("\n        process ID: "));
		    پیغام_outnum(char_مقصد_long(b0.b0_pid));
#اگر defined(UNIX) || defined(MSWIN)
		    اگر (swapfile_process_running(&b0, fname))
		    {
			پیغام_puts(_(" (STILL RUNNING)"));
# ifdef HAVE_PROCESS_STILL_RUNNING
			process_still_running = TRUE;
# endif
		    }
#اگر_ختم
		}

		اگر (b0_magic_wrong(&b0))
		{
#اگر defined(MSWIN)
		    اگر (STRNCMP(b0.b0_hname, "PC ", 3) == 0)
			پیغام_puts(_("\n         [not usable with this version of Vim]"));
		    ورنہ
#اگر_ختم
			پیغام_puts(_("\n         [not usable on this computer]"));
		}
	    }
	}
	ورنہ
	    پیغام_puts(_("         [cannot be read]"));
	close(fd);
    }
    ورنہ
	پیغام_puts(_("         [cannot be opened]"));
    پیغام_putchar('\n');

    واپس st.st_mtime;
}

/*
 * Return TRUE if the swap file looks OK and there are no changes, thus it can
 * be safely deleted.
 */
    ساکن صحیح
swapfile_unchanged(char_u *fname)
{
    stat_T	    st;
    صحیح		    fd;
    ساخت block0   b0;
    صحیح		    ret = TRUE;

    // must be able to stat the swap file
    اگر (mch_stat((حرف *)fname, &st) == -1)
	واپس FALSE;

    // must be able to read the first block
    fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
    اگر (fd < 0)
	واپس FALSE;
    اگر (read_eintr(fd, &b0, حجم(b0)) != حجم(b0))
    {
	close(fd);
	واپس FALSE;
    }

    // the ID and magic number must be correct
    اگر (ml_check_b0_id(&b0) == FAIL|| b0_magic_wrong(&b0))
	ret = FALSE;

    // must be unchanged
    اگر (b0.b0_dirty)
	ret = FALSE;

#اگر defined(UNIX) || defined(MSWIN)
    // Host name must be known and must equal the current host name, otherwise
    // comparing pid is meaningless.
    اگر (*(b0.b0_hname) == NUL)
    {
	ret = FALSE;
    }
    ورنہ
    {
	char_u	    hostname[B0_HNAME_سائز];

	mch_get_host_name(hostname, B0_HNAME_سائز);
	hostname[B0_HNAME_سائز - 1] = NUL;
	b0.b0_hname[B0_HNAME_سائز - 1] = NUL; // in case of corruption
	اگر (STRICMP(b0.b0_hname, hostname) != 0)
	    ret = FALSE;
    }

    // process must be known and not be running
    اگر (char_مقصد_long(b0.b0_pid) == 0L || swapfile_process_running(&b0, fname))
	ret = FALSE;
#اگر_ختم

    // We do not check the user, it should be irrelevant for whether the swap
    // file is still useful.

    close(fd);
    واپس ret;
}

    ساکن صحیح
recov_file_نام(char_u **نام, char_u *راستہ, صحیح prepend_dot)
{
    صحیح		عدد_نام;

    /*
     * (Win32 and Win64) never short names, but do prepend a dot.
     * (Not MS-DOS or Win32 or Win64) maybe short name, maybe not: Try both.
     * Only use the short name if it is different.
     */
    char_u	*p;
    صحیح		i;
#اگر_غیر_تعریف_شدہ MSWIN
    bool    shortname = curbuf->b_shortname;

    curbuf->b_shortname = false;
#اگر_ختم
    string_T	ret;

    عدد_نام = 0;

    /*
     * May also add the file name with a dot prepended, for swap file in same
     * dir as original file.
     */
    اگر (prepend_dot)
    {
	نام[عدد_نام] = modname(راستہ, (char_u *)".sw?", TRUE);
	اگر (نام[عدد_نام] == تہی)
	    جاؤ انتہا;
	++عدد_نام;
    }

    /*
     * Form the normal swap file name pattern by appending ".sw?".
     */
#اگر_تعریف_شدہ VMS
    نام[عدد_نام] = concat_fnames(راستہ, STRLEN(راستہ),
	(char_u *)"_sw%", STRLEN_LITERAL("_sw%"), FALSE, &ret);
#ورنہ
    نام[عدد_نام] = concat_fnames(راستہ, STRLEN(راستہ),
	(char_u *)".sw?", STRLEN_LITERAL(".sw?"), FALSE, &ret);
#اگر_ختم
    اگر (نام[عدد_نام] == تہی)
	جاؤ انتہا;
    اگر (عدد_نام >= 1)	    // check if we have the same name twice
    {
	p = نام[عدد_نام - 1];
	i = (صحیح)STRLEN(نام[عدد_نام - 1]) - (صحیح)STRLEN(نام[عدد_نام]);
	اگر (i > 0)
	    p += i;	    // file name has been expanded to full path

	اگر (STRCMP(p, نام[عدد_نام]) != 0)
	    ++عدد_نام;
	ورنہ
	    vim_free(نام[عدد_نام]);
    }
    ورنہ
	++عدد_نام;

#اگر_غیر_تعریف_شدہ MSWIN
    /*
     * Also try with 'shortname' set, in case the file is on a DOS filesystem.
     */
    curbuf->b_shortname = true;
# ifdef VMS
    نام[عدد_نام] = modname(راستہ, (char_u *)"_sw%", FALSE);
# ورنہ
    نام[عدد_نام] = modname(راستہ, (char_u *)".sw?", FALSE);
# endif
    اگر (نام[عدد_نام] == تہی)
	جاؤ انتہا;

    /*
     * Remove the one from 'shortname', if it's the same as with 'noshortname'.
     */
    p = نام[عدد_نام];
    i = STRLEN(نام[عدد_نام]) - STRLEN(نام[عدد_نام - 1]);
    اگر (i > 0)
	p += i;		// file name has been expanded to full path
    اگر (STRCMP(نام[عدد_نام - 1], p) == 0)
	vim_free(نام[عدد_نام]);
    ورنہ
	++عدد_نام;
#اگر_ختم

انتہا:
#اگر_غیر_تعریف_شدہ MSWIN
    curbuf->b_shortname = shortname;
#اگر_ختم

    واپس عدد_نام;
}

/*
 * sync all memlines
 *
 * If 'check_file' is TRUE, check if original file exists and was not changed.
 * If 'check_char' is TRUE, stop syncing when character becomes available, but
 * always sync at least one block.
 */
    خالی
ml_sync_all(صحیح check_file, صحیح check_char)
{
    buf_T		*buf;
    stat_T		st;

    FOR_ALL_BUFFERS(buf)
    {
	اگر (buf->b_ml.ml_mfp == تہی
		|| buf->b_ml.ml_mfp->mf_fname == تہی
		|| buf->b_ml.ml_mfp->mf_fd < 0)
	    جاری;			    // no file

#اگر_تعریف_شدہ FEAT_CRYPT
	اگر (crypt_may_close_swapfile(buf, buf->b_p_کلید,
						     crypt_get_طریقہ_nr(buf)))
	    جاری;
#اگر_ختم

	ml_flush_سطر(buf);		    // flush buffered line
					    // flush locked block
	(خالی)ml_ڈھونڈو_سطر(buf, (linenr_T)0, ML_FLUSH);
	اگر (buf_Is_بدلا(buf) && check_file && mf_need_trans(buf->b_ml.ml_mfp)
						     && buf->b_ffname != تہی)
	{
	    /*
	     * If the original file does not exist anymore or has been changed
	     * call ml_preserve() to get rid of all negative numbered blocks.
	     */
	    اگر (mch_stat((حرف *)buf->b_ffname, &st) == -1
		    || st.st_mtime != buf->b_mtime_read
#اگر_تعریف_شدہ ST_MTIM_NSEC
		    || st.ST_MTIM_NSEC != buf->b_mtime_read_ns
#اگر_ختم
		    || st.st_size != buf->b_orig_سائز)
	    {
		ml_preserve(buf, FALSE);
		did_check_timestamps = FALSE;
		need_check_timestamps = TRUE;	// give message later
	    }
	}
	اگر (buf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES)
	{
	    (خالی)mf_sync(buf->b_ml.ml_mfp, (check_char ? MFS_STOP : 0)
					| (buf_Is_بدلا(buf) ? MFS_FLUSH : 0));
	    اگر (check_char && ui_char_avail())	// character available now
		توڑو;
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
    خالی
ml_preserve(buf_T *buf, صحیح پیغام_2)
{
    bhdr_T	*hp;
    linenr_T	lnum;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    صحیح		حالت_2;
    صحیح		got_int_save = got_int;

    اگر (mfp == تہی || mfp->mf_fname == تہی)
    {
	اگر (پیغام_2)
	    emsg(_(e_cannot_preserve_there_is_no_تبادلہ_file));
	واپس;
    }
#اگر_تعریف_شدہ FEAT_CRYPT
    اگر (crypt_may_close_swapfile(buf, buf->b_p_کلید, crypt_get_طریقہ_nr(buf)))
	واپس;
#اگر_ختم

    // We only want to stop when interrupted here, not when interrupted
    // before.
    got_int = FALSE;

    ml_flush_سطر(buf);				    // flush buffered line
    (خالی)ml_ڈھونڈو_سطر(buf, (linenr_T)0, ML_FLUSH); // flush locked block
    حالت_2 = mf_sync(mfp, MFS_ALL | MFS_FLUSH);

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
    اگر (mf_need_trans(mfp) && !got_int)
    {
	lnum = 1;
	جب (mf_need_trans(mfp) && lnum <= buf->b_ml.ml_سطر_گنتی)
	{
	    hp = ml_ڈھونڈو_سطر(buf, lnum, ML_ڈھونڈو);
	    اگر (hp == تہی)
	    {
		حالت_2 = FAIL;
		جاؤ theend;
	    }
	    CHECK(buf->b_ml.ml_locked_کم != lnum, "low != lnum");
	    lnum = buf->b_ml.ml_locked_بلند + 1;
	}
	(خالی)ml_ڈھونڈو_سطر(buf, (linenr_T)0, ML_FLUSH);	// flush locked block
	// sync the updated pointer blocks
	اگر (mf_sync(mfp, MFS_ALL | MFS_FLUSH) == FAIL)
	    حالت_2 = FAIL;
	buf->b_ml.ml_ڈھیر_چوٹی = 0;	    // stack is invalid now
    }
theend:
    got_int |= got_int_save;

    اگر (پیغام_2)
    {
	اگر (حالت_2 == OK)
	    پیغام(_("File preserved"));
	ورنہ
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
    واپس ml_get_buf(curbuf, lnum, FALSE);
}

/*
 * Return pointer to position "pos".
 */
    char_u *
ml_get_pos(pos_T *pos)
{
    واپس (ml_get_buf(curbuf, pos->lnum, FALSE) + pos->ستون);
}

/*
 * Return pointer to cursor line.
 */
    char_u *
ml_get_curline(خالی)
{
    واپس ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE);
}

/*
 * Return pointer to cursor position.
 */
    char_u *
ml_get_cursor(خالی)
{
    واپس (ml_get_buf(curbuf, curwin->w_cursor.lnum, FALSE) +
							curwin->w_cursor.ستون);
}

// return length (excluding the NUL) of the given line
    colnr_T
ml_get_len(linenr_T lnum)
{
    واپس ml_get_buf_len(curbuf, lnum);
}

// return length (excluding the NUL) of the text after position "pos"
    colnr_T
ml_get_pos_len(pos_T *pos)
{
    واپس ml_get_buf_len(curbuf, pos->lnum) - pos->ستون;
}

// return length (excluding the NUL) of the cursor line
    colnr_T
ml_get_curline_len(خالی)
{
    واپس ml_get_buf_len(curbuf, curwin->w_cursor.lnum);
}

// return length (excluding the NUL) of the cursor position
    colnr_T
ml_get_cursor_len(خالی)
{
    واپس ml_get_buf_len(curbuf, curwin->w_cursor.lnum) - curwin->w_cursor.ستون;
}

// return length (excluding the NUL) of the given line in the given buffer
    colnr_T
ml_get_buf_len(buf_T *buf, linenr_T lnum)
{
    char_u	*سطر;

    اگر (*(سطر = ml_get_buf(buf, lnum, FALSE)) == NUL)
	واپس 0;

    اگر (buf->b_ml.ml_سطر_textlen <= 0)
	buf->b_ml.ml_سطر_textlen = (صحیح)STRLEN(سطر) + 1;
    واپس buf->b_ml.ml_سطر_textlen - 1;
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
    صحیح		will_change)		// line will be changed
{
    bhdr_T	*hp;
    ڈیٹا_BL	*dp;
    ساکن صحیح	recursive = 0;
    ساکن char_u questions[4];

    اگر (lnum > buf->b_ml.ml_سطر_گنتی)	// invalid line number
    {
	اگر (recursive == 0)
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
	واپس questions;
    }
    اگر (lnum <= 0)			// pretend line 0 is line 1
	lnum = 1;

    اگر (buf->b_ml.ml_mfp == تہی)	// there are no lines
    {
	buf->b_ml.ml_سطر_len = 1;
	buf->b_ml.ml_سطر_textlen = buf->b_ml.ml_سطر_len;
	واپس (char_u *)"";
    }

    /*
     * See if it is the same line as requested last time.
     * Otherwise may need to flush last used line.
     * Don't use the last used line when 'swapfile' is reset, need to load all
     * blocks.
     */
    اگر (buf->b_ml.ml_سطر_lnum != lnum || mf_dont_release)
    {
	غیرعلامتی    آغاز, انتہا;
	صحیح	    idx;

	ml_flush_سطر(buf);

	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	اگر ((hp = ml_ڈھونڈو_سطر(buf, lnum, ML_ڈھونڈو)) == تہی)
	{
	    اگر (recursive == 0)
	    {
		// Avoid giving this message for a recursive call, may happen
		// when the GUI redraws part of the text.
		++recursive;
		get_trans_bufname(buf);
		shorten_dir(NameBuff);
		siemsg(e_ml_get_cannot_ڈھونڈو_سطر_nr_اندر_بفر_nr_str,
						  lnum, buf->b_fnum, NameBuff);
		--recursive;
	    }
	    جاؤ errorret;
	}

	dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);

	idx = lnum - buf->b_ml.ml_locked_کم;
	آغاز = ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب);
	// The text ends where the previous line starts.  The first line ends
	// at the end of the block.
	اگر (idx == 0)
	    انتہا = dp->db_txt_انتہا;
	ورنہ
	    انتہا = ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب);

	buf->b_ml.ml_سطر_ptr = (char_u *)dp + آغاز;
	buf->b_ml.ml_سطر_len = انتہا - آغاز;
#اگر defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
	// Text properties come after a NUL byte, so ml_line_len should be
	// larger than the size of textprop_T if there is any.
	اگر (buf->b_has_textprop
			 && (size_t)buf->b_ml.ml_سطر_len > حجم(textprop_T))
	    buf->b_ml.ml_سطر_textlen = 0;  // call STRLEN() later when needed
	ورنہ
#اگر_ختم
	    buf->b_ml.ml_سطر_textlen = buf->b_ml.ml_سطر_len;
	buf->b_ml.ml_سطر_lnum = lnum;
	buf->b_ml.ml_flags &= ~(ML_سطر_DIRTY | ML_ALLOCATED);
    }
    اگر (will_change)
    {
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (ml_get_alloc_سطریں && (buf->b_ml.ml_flags & ML_ALLOCATED))
	    // can't make the change in the data block
	    buf->b_ml.ml_flags |= ML_سطر_DIRTY;
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (ml_get_alloc_سطریں
		 && (buf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED)) == 0)
    {
	char_u *p = alloc(buf->b_ml.ml_سطر_len);

	// make sure the text is in allocated memory
	اگر (p != تہی)
	{
	    memmove(p, buf->b_ml.ml_سطر_ptr, buf->b_ml.ml_سطر_len);
	    buf->b_ml.ml_سطر_ptr = p;
	    buf->b_ml.ml_flags |= ML_ALLOCATED;
	    اگر (will_change)
		// can't make the change in the data block
		buf->b_ml.ml_flags |= ML_سطر_DIRTY;
	}
    }
#اگر_ختم
    واپس buf->b_ml.ml_سطر_ptr;
}

/*
 * Check if a line that was just obtained by a call to ml_get
 * is in allocated memory.
 * This ignores ML_ALLOCATED to get the same behavior as without the test
 * override.
 */
    صحیح
ml_سطر_alloced(خالی)
{
    واپس (curbuf->b_ml.ml_flags & ML_سطر_DIRTY);
}

#اگر_تعریف_شدہ FEAT_PROP_POPUP
/*
 * Add text properties that continue from the previous line.
 */
    ساکن خالی
جوڑو_متن_props_for_append(
	    buf_T	*buf,
	    linenr_T	lnum,
	    char_u	**سطر,
	    صحیح		*len,
	    char_u	**tofree)
{
    صحیح		باری;
    صحیح		new_prop_گنتی = 0;
    صحیح		گنتی;
    صحیح		n;
    char_u	*props;
    صحیح		new_len = 0;  // init for gcc
    char_u	*new_سطر = تہی;
    textprop_T	prop;

    // Make two rounds:
    // 1. calculate the extra space needed
    // 2. allocate the space and fill it
    برائے (باری = 1; باری <= 2; ++باری)
    {
	اگر (باری == 2)
	{
	    uint16_t pc;

	    اگر (new_prop_گنتی == 0)
		واپس;  // nothing to do
	    new_len = *len + (صحیح)PROP_گنتی_سائز
			     + new_prop_گنتی * (صحیح)حجم(textprop_T);
	    new_سطر = alloc(new_len);
	    اگر (new_سطر == تہی)
		واپس;
	    mch_memmove(new_سطر, *سطر, *len);
	    // Write prop_count header.
	    pc = (uint16_t)new_prop_گنتی;
	    mch_memmove(new_سطر + *len, &pc, PROP_گنتی_سائز);
	    new_prop_گنتی = 0;
	}

	// Get the line above to find any props that continue in the next
	// line.
	گنتی = get_متن_props(buf, lnum, &props, FALSE);
	برائے (n = 0; n < گنتی; ++n)
	{
	    mch_memmove(&prop, props + n * حجم(textprop_T),
							   حجم(textprop_T));
	    اگر (prop.tp_flags & TP_FLAG_CONT_اگلا)
	    {
		اگر (باری == 2)
		{
		    prop.tp_flags |= TP_FLAG_CONT_پچھلا;
		    prop.tp_ستون = 1;
		    prop.tp_len = *len;  // not exactly the right length
		    prop.u.tp_متن_انحراف = 0;
		    mch_memmove(new_سطر + *len + (صحیح)PROP_گنتی_سائز
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
#اگر_ختم

    ساکن صحیح
ml_append_int(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*سطر_arg,	// text of the new line
    colnr_T	len_arg,	// length of line, including NUL, or 0
    صحیح		flags)		// ML_APPEND_ flags
{
    char_u	*سطر = سطر_arg;
    colnr_T	len = len_arg;
    صحیح		i;
    صحیح		سطر_گنتی;	// number of indexes in current block
    صحیح		انحراف;
    صحیح		from, مقصد;
    طویل	space_needed;	// space needed for new line
    صحیح		صفحہ_سائز;
    صحیح		صفحہ_گنتی;
    صحیح		db_idx;		// index for lnum in data block
    bhdr_T	*hp;
    memfile_T	*mfp;
    ڈیٹا_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    char_u	*tofree = تہی;
# ifdef FEAT_BYTEOFF
    colnr_T	متن_len = 0;	// text len with NUL without text properties
# endif
#اگر_ختم
    صحیح		ret = FAIL;

    اگر (lnum > buf->b_ml.ml_سطر_گنتی || buf->b_ml.ml_mfp == تہی)
	واپس FAIL;  // lnum out of range

    اگر (سب_سے_نیچا_marked && سب_سے_نیچا_marked > lnum)
	سب_سے_نیچا_marked = lnum + 1;

    اگر (len == 0)
    {
	len = (colnr_T)STRLEN(سطر) + 1;	// space needed for the text
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
	متن_len = len;
#اگر_ختم
    }
#اگر defined(FEAT_PROP_POPUP) && defined(FEAT_BYTEOFF)
    ورنہ اگر (curbuf->b_has_textprop)
	// "len" may include text properties, get the length of the text.
	متن_len = (colnr_T)STRLEN(سطر) + 1;
    ورنہ
	متن_len = len;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (curbuf->b_has_textprop && lnum > 0
			     && !(flags & (ML_APPEND_UNDO | ML_APPEND_NOPROP)))
	// Add text properties that continue from the previous line.
	جوڑو_متن_props_for_append(buf, lnum, &سطر, &len, &tofree);
#اگر_ختم

    space_needed = len + اشاریہ_سائز;	// space needed for text + index

    mfp = buf->b_ml.ml_mfp;
    صفحہ_سائز = mfp->mf_صفحہ_سائز;

/*
 * find the data block containing the previous line
 * This also fills the stack with the blocks from the root to the data block
 * This also releases any locked block.
 */
    اگر ((hp = ml_ڈھونڈو_سطر(buf, lnum == 0 ? (linenr_T)1 : lnum,
							  ML_داخل_کرو)) == تہی)
	جاؤ theend;

    buf->b_ml.ml_flags &= ~ML_خالی_خانے;

    اگر (lnum == 0)		// got line one instead, correct db_idx
	db_idx = -1;		// careful, it is negative!
    ورنہ
	db_idx = lnum - buf->b_ml.ml_locked_کم;
		// get line count before the insertion
    سطر_گنتی = buf->b_ml.ml_locked_بلند - buf->b_ml.ml_locked_کم;

    dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);

/*
 * If
 * - there is not enough room in the current block
 * - appending to the last line in the block
 * - not appending to the last line in the file
 * insert in front of the next block.
 */
    اگر ((طویل)dp->db_free < space_needed && db_idx == سطر_گنتی - 1
					    && lnum < buf->b_ml.ml_سطر_گنتی)
    {
	/*
	 * Now that the line is not going to be inserted in the block that we
	 * expected, the line count has to be adjusted in the pointer blocks
	 * by using ml_locked_lineadd.
	 */
	--(buf->b_ml.ml_locked_lineadd);
	--(buf->b_ml.ml_locked_بلند);
	اگر ((hp = ml_ڈھونڈو_سطر(buf, lnum + 1, ML_داخل_کرو)) == تہی)
	    جاؤ theend;

	db_idx = -1;		    // careful, it is negative!
		    // get line count before the insertion
	سطر_گنتی = buf->b_ml.ml_locked_بلند - buf->b_ml.ml_locked_کم;
	CHECK(buf->b_ml.ml_locked_کم != lnum + 1, "locked_low != lnum + 1");

	dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
    }

    ++buf->b_ml.ml_سطر_گنتی;

    اگر ((طویل)dp->db_free >= space_needed)	// enough room in data block
    {
	/*
	 * Insert the new line in an existing data block, or in the data block
	 * allocated above.
	 */
	dp->db_txt_آغاز -= len;
	dp->db_free -= space_needed;
	++(dp->db_سطر_گنتی);

	/*
	 * move the text of the lines that follow to the front
	 * adjust the indexes of the lines that follow
	 */
	اگر (سطر_گنتی > db_idx + 1)	    // if there are following lines
	{
	    /*
	     * Offset is the start of the previous line.
	     * This will become the character just after the new line.
	     */
	    اگر (db_idx < 0)
		انحراف = dp->db_txt_انتہا;
	    ورنہ
		انحراف = ((dp->db_اشاریہ[db_idx]) & DB_اشاریہ_نقاب);
	    mch_memmove((حرف *)dp + dp->db_txt_آغاز,
					  (حرف *)dp + dp->db_txt_آغاز + len,
				 (size_t)(انحراف - (dp->db_txt_آغاز + len)));
	    برائے (i = سطر_گنتی - 1; i > db_idx; --i)
		dp->db_اشاریہ[i + 1] = dp->db_اشاریہ[i] - len;
	    dp->db_اشاریہ[db_idx + 1] = انحراف - len;
	}
	ورنہ
	    // add line at the end (which is the start of the text)
	    dp->db_اشاریہ[db_idx + 1] = dp->db_txt_آغاز;

	/*
	 * copy the text into the block
	 */
	mch_memmove((حرف *)dp + dp->db_اشاریہ[db_idx + 1], سطر, (size_t)len);
	اگر (flags & ML_APPEND_MARK)
	    dp->db_اشاریہ[db_idx + 1] |= DB_MARKED;

	/*
	 * Mark the block dirty.
	 */
	buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	اگر (!(flags & ML_APPEND_NEW))
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
    }
    ورنہ	    // not enough space in data block
    {
	طویل	    سطر_گنتی_بایاں, سطر_گنتی_دایاں;
	صحیح	    صفحہ_گنتی_بایاں, صفحہ_گنتی_دایاں;
	bhdr_T	    *hp_بایاں;
	bhdr_T	    *hp_دایاں;
	bhdr_T	    *hp_new;
	صحیح	    سطریں_moved;
	صحیح	    ڈیٹا_moved = 0;	    // init to shut up gcc
	صحیح	    کل_moved = 0;	    // init to shut up gcc
	ڈیٹا_BL	    *dp_دایاں, *dp_بایاں;
	صحیح	    ڈھیر_idx;
	صحیح	    اندر_بایاں;
	صحیح	    lineadd;
	blocknr_T   bnum_بایاں, bnum_دایاں;
	linenr_T    lnum_بایاں, lnum_دایاں;
	صحیح	    pb_idx;
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
	اگر (db_idx < 0)		// left block is new, right block is existing
	{
	    سطریں_moved = 0;
	    اندر_بایاں = TRUE;
	    // space_needed does not change
	}
	ورنہ			// left block is existing, right block is new
	{
	    سطریں_moved = سطر_گنتی - db_idx - 1;
	    اگر (سطریں_moved == 0)
		اندر_بایاں = FALSE;	// put new line in right block
					// space_needed does not change
	    ورنہ
	    {
		ڈیٹا_moved = ((dp->db_اشاریہ[db_idx]) & DB_اشاریہ_نقاب) -
							    dp->db_txt_آغاز;
		کل_moved = ڈیٹا_moved + سطریں_moved * اشاریہ_سائز;
		اگر ((طویل)dp->db_free + کل_moved >= space_needed)
		{
		    اندر_بایاں = TRUE;	// put new line in left block
		    space_needed = کل_moved;
		}
		ورنہ
		{
		    اندر_بایاں = FALSE;	    // put new line in right block
		    space_needed += کل_moved;
		}
	    }
	}

	صفحہ_گنتی = ((space_needed + سرخی_سائز) + صفحہ_سائز - 1) / صفحہ_سائز;
	اگر ((hp_new = ml_new_ڈیٹا(mfp, flags & ML_APPEND_NEW, صفحہ_گنتی))
								       == تہی)
	{
			// correct line counts in pointer blocks
	    --(buf->b_ml.ml_locked_lineadd);
	    --(buf->b_ml.ml_locked_بلند);
	    جاؤ theend;
	}
	اگر (db_idx < 0)		// left block is new
	{
	    hp_بایاں = hp_new;
	    hp_دایاں = hp;
	    سطر_گنتی_بایاں = 0;
	    سطر_گنتی_دایاں = سطر_گنتی;
	}
	ورنہ			// right block is new
	{
	    hp_بایاں = hp;
	    hp_دایاں = hp_new;
	    سطر_گنتی_بایاں = سطر_گنتی;
	    سطر_گنتی_دایاں = 0;
	}
	dp_دایاں = (ڈیٹا_BL *)(hp_دایاں->bh_ڈیٹا);
	dp_بایاں = (ڈیٹا_BL *)(hp_بایاں->bh_ڈیٹا);
	bnum_بایاں = hp_بایاں->bh_bnum;
	bnum_دایاں = hp_دایاں->bh_bnum;
	صفحہ_گنتی_بایاں = hp_بایاں->bh_صفحہ_گنتی;
	صفحہ_گنتی_دایاں = hp_دایاں->bh_صفحہ_گنتی;

	/*
	 * May move the new line into the right/new block.
	 */
	اگر (!اندر_بایاں)
	{
	    dp_دایاں->db_txt_آغاز -= len;
	    dp_دایاں->db_free -= len + اشاریہ_سائز;
	    dp_دایاں->db_اشاریہ[0] = dp_دایاں->db_txt_آغاز;
	    اگر (flags & ML_APPEND_MARK)
		dp_دایاں->db_اشاریہ[0] |= DB_MARKED;

	    mch_memmove((حرف *)dp_دایاں + dp_دایاں->db_txt_آغاز,
							   سطر, (size_t)len);
	    ++سطر_گنتی_دایاں;
	}
	/*
	 * may move lines from the left/old block to the right/new one.
	 */
	اگر (سطریں_moved)
	{
	    dp_دایاں->db_txt_آغاز -= ڈیٹا_moved;
	    dp_دایاں->db_free -= کل_moved;
	    mch_memmove((حرف *)dp_دایاں + dp_دایاں->db_txt_آغاز,
			(حرف *)dp_بایاں + dp_بایاں->db_txt_آغاز,
			(size_t)ڈیٹا_moved);
	    انحراف = dp_دایاں->db_txt_آغاز - dp_بایاں->db_txt_آغاز;
	    dp_بایاں->db_txt_آغاز += ڈیٹا_moved;
	    dp_بایاں->db_free += کل_moved;

	    /*
	     * update indexes in the new block
	     */
	    برائے (مقصد = سطر_گنتی_دایاں, from = db_idx + 1;
					 from < سطر_گنتی_بایاں; ++from, ++مقصد)
		dp_دایاں->db_اشاریہ[مقصد] = dp->db_اشاریہ[from] + انحراف;
	    سطر_گنتی_دایاں += سطریں_moved;
	    سطر_گنتی_بایاں -= سطریں_moved;
	}

	/*
	 * May move the new line into the left (old or new) block.
	 */
	اگر (اندر_بایاں)
	{
	    dp_بایاں->db_txt_آغاز -= len;
	    dp_بایاں->db_free -= len + اشاریہ_سائز;
	    dp_بایاں->db_اشاریہ[سطر_گنتی_بایاں] = dp_بایاں->db_txt_آغاز;
	    اگر (flags & ML_APPEND_MARK)
		dp_بایاں->db_اشاریہ[سطر_گنتی_بایاں] |= DB_MARKED;
	    mch_memmove((حرف *)dp_بایاں + dp_بایاں->db_txt_آغاز,
							   سطر, (size_t)len);
	    ++سطر_گنتی_بایاں;
	}

	اگر (db_idx < 0)		// left block is new
	{
	    lnum_بایاں = lnum + 1;
	    lnum_دایاں = 0;
	}
	ورنہ			// right block is new
	{
	    lnum_بایاں = 0;
	    اگر (اندر_بایاں)
		lnum_دایاں = lnum + 2;
	    ورنہ
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
	اگر (سطریں_moved || اندر_بایاں)
	    buf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	اگر (!(flags & ML_APPEND_NEW) && db_idx >= 0 && اندر_بایاں)
	    buf->b_ml.ml_flags |= ML_LOCKED_POS;
	mf_put(mfp, hp_new, TRUE, FALSE);

	/*
	 * flush the old data block
	 * set ml_locked_lineadd to 0, because the updating of the
	 * pointer blocks is done below
	 */
	lineadd = buf->b_ml.ml_locked_lineadd;
	buf->b_ml.ml_locked_lineadd = 0;
	ml_ڈھونڈو_سطر(buf, (linenr_T)0, ML_FLUSH);   // flush data block

	/*
	 * update pointer blocks for the new data block
	 */
	برائے (ڈھیر_idx = buf->b_ml.ml_ڈھیر_چوٹی - 1; ڈھیر_idx >= 0;
								  --ڈھیر_idx)
	{
	    ip = &(buf->b_ml.ml_ڈھیر[ڈھیر_idx]);
	    pb_idx = ip->ip_اشاریہ;
	    اگر ((hp = mf_get(mfp, ip->ip_bnum, 1)) == تہی)
		جاؤ theend;
	    pp = (PTR_BL *)(hp->bh_ڈیٹا);   // must be pointer block
	    اگر (pp->pb_id != PTR_ID)
	    {
		iemsg(e_اشارہ_گر_بلاک_id_wrong_three);
		mf_put(mfp, hp, FALSE, FALSE);
		جاؤ theend;
	    }
	    /*
	     * TODO: If the pointer block is full and we are adding at the end
	     * try to insert in front of the next block
	     */
	    // block not full, add one entry
	    اگر (pp->pb_گنتی < pp->pb_گنتی_max)
	    {
		اگر (pb_idx + 1 < (صحیح)pp->pb_گنتی)
		    mch_memmove(&pp->pb_اشارہ_گر[pb_idx + 2],
				&pp->pb_اشارہ_گر[pb_idx + 1],
			(size_t)(pp->pb_گنتی - pb_idx - 1) * حجم(PTR_EN));
		++pp->pb_گنتی;
		pp->pb_اشارہ_گر[pb_idx].pe_سطر_گنتی = سطر_گنتی_بایاں;
		pp->pb_اشارہ_گر[pb_idx].pe_bnum = bnum_بایاں;
		pp->pb_اشارہ_گر[pb_idx].pe_صفحہ_گنتی = صفحہ_گنتی_بایاں;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_سطر_گنتی = سطر_گنتی_دایاں;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_bnum = bnum_دایاں;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_صفحہ_گنتی = صفحہ_گنتی_دایاں;

		اگر (lnum_بایاں != 0)
		    pp->pb_اشارہ_گر[pb_idx].pe_old_lnum = lnum_بایاں;
		اگر (lnum_دایاں != 0)
		    pp->pb_اشارہ_گر[pb_idx + 1].pe_old_lnum = lnum_دایاں;

		mf_put(mfp, hp, TRUE, FALSE);
		buf->b_ml.ml_ڈھیر_چوٹی = ڈھیر_idx + 1;	    // truncate stack

		اگر (lineadd)
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
		توڑو;
	    }
	    // pointer block full
	    /*
	     * split the pointer block
	     * allocate a new pointer block
	     * move some of the pointer into the new block
	     * prepare for updating the parent block
	     */
	    برائے (;;)	// do this twice when splitting block 1
	    {
		hp_new = ml_new_ptr(mfp);
		اگر (hp_new == تہی)	    // TODO: try to fix tree
		    جاؤ theend;
		pp_new = (PTR_BL *)(hp_new->bh_ڈیٹا);

		اگر (hp->bh_bnum != 1)
		    توڑو;

		/*
		 * if block 1 becomes full the tree is given an extra level
		 * The pointers from block 1 are moved into the new block.
		 * block 1 is updated to point to the new block
		 * then continue to split the new block
		 */
		mch_memmove(pp_new, pp, (size_t)صفحہ_سائز);
		pp->pb_گنتی = 1;
		pp->pb_اشارہ_گر[0].pe_bnum = hp_new->bh_bnum;
		pp->pb_اشارہ_گر[0].pe_سطر_گنتی = buf->b_ml.ml_سطر_گنتی;
		pp->pb_اشارہ_گر[0].pe_old_lnum = 1;
		pp->pb_اشارہ_گر[0].pe_صفحہ_گنتی = 1;
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
	    اگر (کل_moved)
	    {
		mch_memmove(&pp_new->pb_اشارہ_گر[0],
				&pp->pb_اشارہ_گر[pb_idx + 1],
				(size_t)(کل_moved) * حجم(PTR_EN));
		pp_new->pb_گنتی = کل_moved;
		pp->pb_گنتی -= کل_moved - 1;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_bnum = bnum_دایاں;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_سطر_گنتی = سطر_گنتی_دایاں;
		pp->pb_اشارہ_گر[pb_idx + 1].pe_صفحہ_گنتی = صفحہ_گنتی_دایاں;
		اگر (lnum_دایاں)
		    pp->pb_اشارہ_گر[pb_idx + 1].pe_old_lnum = lnum_دایاں;
	    }
	    ورنہ
	    {
		pp_new->pb_گنتی = 1;
		pp_new->pb_اشارہ_گر[0].pe_bnum = bnum_دایاں;
		pp_new->pb_اشارہ_گر[0].pe_سطر_گنتی = سطر_گنتی_دایاں;
		pp_new->pb_اشارہ_گر[0].pe_صفحہ_گنتی = صفحہ_گنتی_دایاں;
		pp_new->pb_اشارہ_گر[0].pe_old_lnum = lnum_دایاں;
	    }
	    pp->pb_اشارہ_گر[pb_idx].pe_bnum = bnum_بایاں;
	    pp->pb_اشارہ_گر[pb_idx].pe_سطر_گنتی = سطر_گنتی_بایاں;
	    pp->pb_اشارہ_گر[pb_idx].pe_صفحہ_گنتی = صفحہ_گنتی_بایاں;
	    اگر (lnum_بایاں)
		pp->pb_اشارہ_گر[pb_idx].pe_old_lnum = lnum_بایاں;
	    lnum_بایاں = 0;
	    lnum_دایاں = 0;

	    /*
	     * recompute line counts
	     */
	    سطر_گنتی_دایاں = 0;
	    برائے (i = 0; i < (صحیح)pp_new->pb_گنتی; ++i)
		سطر_گنتی_دایاں += pp_new->pb_اشارہ_گر[i].pe_سطر_گنتی;
	    سطر_گنتی_بایاں = 0;
	    برائے (i = 0; i < (صحیح)pp->pb_گنتی; ++i)
		سطر_گنتی_بایاں += pp->pb_اشارہ_گر[i].pe_سطر_گنتی;

	    bnum_بایاں = hp->bh_bnum;
	    bnum_دایاں = hp_new->bh_bnum;
	    صفحہ_گنتی_بایاں = 1;
	    صفحہ_گنتی_دایاں = 1;
	    mf_put(mfp, hp, TRUE, FALSE);
	    mf_put(mfp, hp_new, TRUE, FALSE);

	}

	/*
	 * Safety check: fallen out of for loop?
	 */
	اگر (ڈھیر_idx < 0)
	{
	    iemsg(e_updated_too_many_بلاکیں);
	    buf->b_ml.ml_ڈھیر_چوٹی = 0;	// invalidate stack
	}
    }

#اگر_تعریف_شدہ FEAT_BYTEOFF
    // The line was inserted below 'lnum'
    ml_updatechunk(buf, lnum + 1,
# ifdef FEAT_PROP_POPUP
	    (طویل)متن_len
# ورنہ
	    (طویل)len
# endif
	    , ML_CHNK_ADDLINE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    اگر (!(flags & ML_APPEND_NEW) && netbeans_فعال())
    {
	صحیح سطر_len = (صحیح)STRLEN(سطر);
	اگر (سطر_len > 0)
	    netbeans_inserted(buf, lnum+1, (colnr_T)0, سطر, سطر_len);
	netbeans_inserted(buf, lnum+1, (colnr_T)سطر_len, (char_u *)"\n", 1);
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    اگر (!(flags & ML_APPEND_NEW) && buf->b_write_مقصد_channel)
	channel_write_new_سطریں(buf);
#اگر_ختم
    ret = OK;

theend:
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    vim_free(tofree);
#اگر_ختم
    واپس ret;
}

/*
 * Flush any pending change and call ml_append_int()
 */
    ساکن صحیح
ml_append_flush(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*سطر,		// text of the new line
    colnr_T	len,		// length of line, including NUL, or 0
    صحیح		flags)		// ML_APPEND_ flags
{
    اگر (lnum > buf->b_ml.ml_سطر_گنتی)
	واپس FAIL;  // lnum out of range

    اگر (buf->b_ml.ml_سطر_lnum != 0)
	// This may also invoke ml_append_int().
	ml_flush_سطر(buf);

#اگر_تعریف_شدہ FEAT_EVAL
    اگر (!(flags & ML_APPEND_NEW))
    {
	// When inserting above recorded changes: flush the changes before
	// changing the text.  Then flush the cached line, it may become
	// invalid.  Skip during initial file read for performance.
	may_invoke_listeners(buf, lnum + 1, lnum + 1, 1);
	اگر (buf->b_ml.ml_سطر_lnum != 0)
	    ml_flush_سطر(buf);
    }
#اگر_ختم

    واپس ml_append_int(buf, lnum, سطر, len, flags);
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
    صحیح
ml_append(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*سطر,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    صحیح		newfile)	// flag, see above
{
    واپس ml_append_flags(lnum, سطر, len, newfile ? ML_APPEND_NEW : 0);
}

    صحیح
ml_append_flags(
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*سطر,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    صحیح		flags)		// ML_APPEND_ values
{
    // When starting up, we might still need to create the memfile
    اگر (curbuf->b_ml.ml_mfp == تہی && open_بفر(FALSE, تہی, 0) == FAIL)
	واپس FAIL;
    واپس ml_append_flush(curbuf, lnum, سطر, len, flags);
}


#اگر defined(FEAT_SPELL) || defined(FEAT_QUICKFIX) || defined(FEAT_PROP_POPUP)

/*
 * Like ml_append() but for an arbitrary buffer.  The buffer must already have
 * a memline.
 */
    صحیح
ml_append_buf(
    buf_T	*buf,
    linenr_T	lnum,		// append after this line (can be 0)
    char_u	*سطر,		// text of the new line
    colnr_T	len,		// length of new line, including NUL, or 0
    صحیح		newfile)	// flag, see above
{
    اگر (buf->b_ml.ml_mfp == تہی)
	واپس FAIL;
    واپس ml_append_flush(buf, lnum, سطر, len, newfile ? ML_APPEND_NEW : 0);
}
#اگر_ختم

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
    صحیح
ml_replace(linenr_T lnum, char_u *سطر, صحیح copy)
{
    colnr_T len = -1;

    اگر (سطر != تہی)
	len = (colnr_T)STRLEN(سطر);
    واپس ml_replace_len(lnum, سطر, len, FALSE, copy);
}

/*
 * Replace a line for the current buffer.  Like ml_replace() with:
 * "len_arg" is the length of the text, excluding NUL.
 * If "has_props" is TRUE then "line_arg" includes the text properties and
 * "len_arg" includes the NUL of the text and text properties.
 * When "copy" is TRUE copy the text into allocated memory, otherwise
 * "line_arg" must be allocated and will be consumed here.
 */
    صحیح
ml_replace_len(
	linenr_T    lnum,
	char_u	    *سطر_arg,
	colnr_T	    len_arg,
	صحیح	    has_props,
	صحیح	    copy)
{
    char_u *سطر = سطر_arg;
    colnr_T len = len_arg;

    اگر (سطر == تہی)		// just checking...
	واپس FAIL;

    // When starting up, we might still need to create the memfile
    اگر (curbuf->b_ml.ml_mfp == تہی && open_بفر(FALSE, تہی, 0) == FAIL)
	واپس FAIL;

    اگر (!has_props)
	++len;  // include the NUL after the text
    اگر (copy)
    {
	// copy the line to allocated memory
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (has_props)
	    سطر = vim_memsave(سطر, len);
	ورنہ
#اگر_ختم
	    سطر = vim_strnsave(سطر, len - 1);
	اگر (سطر == تہی)
	    واپس FAIL;
    }

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    اگر (netbeans_فعال())
    {
	netbeans_removed(curbuf, lnum, 0, (طویل)ml_get_len(lnum));
	netbeans_inserted(curbuf, lnum, 0, سطر, (صحیح)STRLEN(سطر));
    }
#اگر_ختم
    اگر (curbuf->b_ml.ml_سطر_lnum != lnum)
    {
	// another line is buffered, flush it
	ml_flush_سطر(curbuf);

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (curbuf->b_has_textprop && !has_props)
	    // Need to fetch the old line to copy over any text properties.
	    ml_get_buf(curbuf, lnum, TRUE);
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (curbuf->b_has_textprop && !has_props)
    {
	size_t	oldtextlen = STRLEN(curbuf->b_ml.ml_سطر_ptr) + 1;

	اگر (oldtextlen < (size_t)curbuf->b_ml.ml_سطر_len)
	{
	    char_u *newline;
	    size_t textproplen = curbuf->b_ml.ml_سطر_len - oldtextlen;

	    // Need to copy over text properties, stored after the text.
	    newline = alloc(len + textproplen);
	    اگر (newline != تہی)
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
#اگر_ختم

    اگر (curbuf->b_ml.ml_flags & (ML_سطر_DIRTY | ML_ALLOCATED))
	vim_free(curbuf->b_ml.ml_سطر_ptr);	// free allocated line

    curbuf->b_ml.ml_سطر_ptr = سطر;
    curbuf->b_ml.ml_سطر_len = len;
    curbuf->b_ml.ml_سطر_textlen = !has_props ? len_arg + 1 : 0;
    curbuf->b_ml.ml_سطر_lnum = lnum;
    curbuf->b_ml.ml_flags = (curbuf->b_ml.ml_flags | ML_سطر_DIRTY) & ~ML_خالی_خانے;

    واپس OK;
}

#اگر_تعریف_شدہ FEAT_PROP_POPUP
/*
 * Adjust text properties in line "lnum" for a deleted line.
 * When "above" is true this is the line above the deleted line, otherwise this
 * is the line below the deleted line.
 * "del_props[del_props_len]" are the properties of the deleted line.
 */
    ساکن خالی
adjust_متن_props_for_delete(
	buf_T	    *buf,
	linenr_T    lnum,
	char_u	    *del_props,
	صحیح	    del_props_len,
	صحیح	    above)
{
    صحیح		did_get_سطر = FALSE;
    صحیح		مکمل_del;
    صحیح		مکمل_this;
    textprop_T	prop_del;
    bhdr_T	*hp;
    ڈیٹا_BL	*dp;
    صحیح		idx;
    صحیح		سطر_آغاز;
    طویل	سطر_سائز;
    صحیح		this_props_len = 0;
    char_u	*متن;
    size_t	textlen;
    صحیح		ملا;

    برائے (مکمل_del = 0; مکمل_del < del_props_len; مکمل_del += حجم(textprop_T))
    {
	mch_memmove(&prop_del, del_props + مکمل_del, حجم(textprop_T));
	اگر ((above && (prop_del.tp_flags & TP_FLAG_CONT_پچھلا)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_اگلا))
		|| (!above && (prop_del.tp_flags & TP_FLAG_CONT_اگلا)
		    && !(prop_del.tp_flags & TP_FLAG_CONT_پچھلا)))
	{
	    اگر (!did_get_سطر)
	    {
		did_get_سطر = TRUE;
		اگر ((hp = ml_ڈھونڈو_سطر(buf, lnum, ML_ڈھونڈو)) == تہی)
		    واپس;

		dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
		idx = lnum - buf->b_ml.ml_locked_کم;
		سطر_آغاز = ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب);
		اگر (idx == 0)		// first line in block, text at the end
		    سطر_سائز = dp->db_txt_انتہا - سطر_آغاز;
		ورنہ
		    سطر_سائز = ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب)
								  - سطر_آغاز;
		متن = (char_u *)dp + سطر_آغاز;
		textlen = STRLEN(متن) + 1;
		اگر ((طویل)textlen >= سطر_سائز)
		{
		    // No properties on this line.
		    اگر (above)
			اندرونی_error("no text property above deleted line");
		    ورنہ
			اندرونی_error("no text property below deleted line");
		    واپس;
		}
		اگر ((طویل)textlen + (طویل)PROP_گنتی_سائز > سطر_سائز)
		{
		    اندرونی_error("text property data too short");
		    واپس;
		}

		uint16_t pc;

		mch_memmove(&pc, متن + textlen, PROP_گنتی_سائز);
		اگر (!متن_prop_گنتی_valid(pc, (size_t)(سطر_سائز - (طویل)textlen)))
		{
		    اندرونی_error("text property count too large");
		    واپس;
		}
		this_props_len = pc * (صحیح)حجم(textprop_T);
	    }

	    ملا = FALSE;
	    {
		char_u *props_آغاز = متن + textlen + PROP_گنتی_سائز;

		برائے (مکمل_this = 0; مکمل_this < this_props_len;
					       مکمل_this += حجم(textprop_T))
		{
		    صحیح		flag = above ? TP_FLAG_CONT_اگلا
							   : TP_FLAG_CONT_پچھلا;
		    textprop_T	prop_this;

		    mch_memmove(&prop_this, props_آغاز + مکمل_this,
							   حجم(textprop_T));
		    اگر ((prop_this.tp_flags & flag)
			    && prop_del.tp_id == prop_this.tp_id
			    && prop_del.tp_type == prop_this.tp_type)
		    {
			ملا = TRUE;
			prop_this.tp_flags &= ~flag;
			mch_memmove(props_آغاز + مکمل_this, &prop_this,
							   حجم(textprop_T));
			توڑو;
		    }
		}
	    }
	    اگر (!ملا)
	    {
		اگر (above)
		    اندرونی_error("text property above deleted line not found");
		ورنہ
		    اندرونی_error("text property below deleted line not found");
	    }

	    buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
	}
    }
}
#اگر_ختم

/*
 * Delete line "lnum" in the current buffer.
 * When "flags" has ML_DEL_MESSAGE may give a "No lines in buffer" message.
 * When "flags" has ML_DEL_UNDO this is called from undo.
 *
 * return FAIL for failure, OK otherwise
 */
    ساکن صحیح
ml_delete_int(buf_T *buf, linenr_T lnum, صحیح flags)
{
    bhdr_T	*hp;
    memfile_T	*mfp;
    ڈیٹا_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    صحیح		گنتی;	    // number of entries in block
    صحیح		idx;
    صحیح		ڈھیر_idx;
    صحیح		متن_آغاز;
    صحیح		سطر_آغاز;
    طویل	سطر_سائز;
    صحیح		i;
    صحیح		ret = FAIL;
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    char_u	*textprop_save = تہی;
    طویل	textprop_len = 0;
#اگر_ختم

    اگر (سب_سے_نیچا_marked && سب_سے_نیچا_marked > lnum)
	سب_سے_نیچا_marked--;

/*
 * If the file becomes empty the last line is replaced by an empty line.
 */
    اگر (buf->b_ml.ml_سطر_گنتی == 1)	    // file becomes empty
    {
	اگر ((flags & ML_DEL_پیغام)
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
		&& !netbeans_Suppress_No_سطریں
#اگر_ختم
	   )
	    سیٹ_keep_پیغام((char_u *)_(no_سطریں_پیغام), 0);

	// FEAT_BYTEOFF already handled in there, don't worry 'bout it below
	i = ml_replace((linenr_T)1, (char_u *)"", TRUE);
	buf->b_ml.ml_flags |= ML_خالی_خانے;

	واپس i;
    }

/*
 * Find the data block containing the line.
 * This also fills the stack with the blocks from the root to the data block.
 * This also releases any locked block..
 */
    mfp = buf->b_ml.ml_mfp;
    اگر (mfp == تہی)
	واپس FAIL;

    اگر ((hp = ml_ڈھونڈو_سطر(buf, lnum, ML_DELETE)) == تہی)
	واپس FAIL;

    dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
    // compute line count before the delete
    گنتی = (طویل)(buf->b_ml.ml_locked_بلند)
					- (طویل)(buf->b_ml.ml_locked_کم) + 2;
    idx = lnum - buf->b_ml.ml_locked_کم;

    --buf->b_ml.ml_سطر_گنتی;

    سطر_آغاز = ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب);
    اگر (idx == 0)		// first line in block, text at the end
	سطر_سائز = dp->db_txt_انتہا - سطر_آغاز;
    ورنہ
	سطر_سائز = ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب) - سطر_آغاز;

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    اگر (netbeans_فعال())
	netbeans_removed(buf, lnum, 0, سطر_سائز);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // If there are text properties compute their byte length.
    // if needed make a copy, so that we can update properties in preceding and
    // following lines.
    اگر (buf->b_has_textprop)
    {
	size_t	textlen = STRLEN((char_u *)dp + سطر_آغاز) + 1;

	textprop_len = سطر_سائز - (طویل)textlen;
	اگر (!(flags & (ML_DEL_UNDO | ML_DEL_NOPROP)) && textprop_len > 0)
	    textprop_save = vim_memsave((char_u *)dp + سطر_آغاز + textlen,
								 textprop_len);
    }
#اگر_ختم

/*
 * special case: If there is only one line in the data block it becomes empty.
 * Then we have to remove the entry, pointing to this data block, from the
 * pointer block. If this pointer block also becomes empty, we go up another
 * block, and so on, up to the root if necessary.
 * The line counts in the pointer blocks have already been adjusted by
 * ml_find_line().
 */
    اگر (گنتی == 1)
    {
	mf_free(mfp, hp);	// free the data block
	buf->b_ml.ml_locked = تہی;

	برائے (ڈھیر_idx = buf->b_ml.ml_ڈھیر_چوٹی - 1; ڈھیر_idx >= 0;
								  --ڈھیر_idx)
	{
	    buf->b_ml.ml_ڈھیر_چوٹی = 0;	    // stack is invalid when failing
	    ip = &(buf->b_ml.ml_ڈھیر[ڈھیر_idx]);
	    idx = ip->ip_اشاریہ;
	    اگر ((hp = mf_get(mfp, ip->ip_bnum, 1)) == تہی)
		جاؤ theend;
	    pp = (PTR_BL *)(hp->bh_ڈیٹا);   // must be pointer block
	    اگر (pp->pb_id != PTR_ID)
	    {
		iemsg(e_اشارہ_گر_بلاک_id_wrong_four);
		mf_put(mfp, hp, FALSE, FALSE);
		جاؤ theend;
	    }
	    گنتی = --(pp->pb_گنتی);
	    اگر (گنتی == 0)	    // the pointer block becomes empty!
		mf_free(mfp, hp);
	    ورنہ
	    {
		اگر (گنتی != idx)	// move entries after the deleted one
		    mch_memmove(&pp->pb_اشارہ_گر[idx], &pp->pb_اشارہ_گر[idx + 1],
				      (size_t)(گنتی - idx) * حجم(PTR_EN));
		mf_put(mfp, hp, TRUE, FALSE);

		buf->b_ml.ml_ڈھیر_چوٹی = ڈھیر_idx;	// truncate stack
		// fix line count for rest of blocks in the stack
		اگر (buf->b_ml.ml_locked_lineadd != 0)
		{
		    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
		    buf->b_ml.ml_ڈھیر[buf->b_ml.ml_ڈھیر_چوٹی].ip_بلند +=
						  buf->b_ml.ml_locked_lineadd;
		}
		++(buf->b_ml.ml_ڈھیر_چوٹی);

		توڑو;
	    }
	}
	CHECK(ڈھیر_idx < 0, _("deleted block 1?"));
    }
    ورنہ
    {
	/*
	 * delete the text by moving the next lines forwards
	 */
	متن_آغاز = dp->db_txt_آغاز;
	mch_memmove((حرف *)dp + متن_آغاز + سطر_سائز,
		  (حرف *)dp + متن_آغاز, (size_t)(سطر_آغاز - متن_آغاز));

	/*
	 * delete the index by moving the next indexes backwards
	 * Adjust the indexes for the text movement.
	 */
	برائے (i = idx; i < گنتی - 1; ++i)
	    dp->db_اشاریہ[i] = dp->db_اشاریہ[i + 1] + سطر_سائز;

	dp->db_free += سطر_سائز + اشاریہ_سائز;
	dp->db_txt_آغاز += سطر_سائز;
	--(dp->db_سطر_گنتی);

	/*
	 * mark the block dirty and make sure it is in the file (for recovery)
	 */
	buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
    }

#اگر_تعریف_شدہ FEAT_BYTEOFF
    ml_updatechunk(buf, lnum, سطر_سائز
# ifdef FEAT_PROP_POPUP
					- textprop_len
# endif
						    , ML_CHNK_DELLINE);
#اگر_ختم
    ret = OK;

theend:
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (textprop_save != تہی)
    {
	// textprop_save is [prop_count][textprop_T...][vtext...].
	// Skip prop_count header and pass only the textprop_T part.
	uint16_t    pc;
	char_u	    *props_ڈیٹا;
	صحیح	    props_بائٹیں;

	mch_memmove(&pc, textprop_save, PROP_گنتی_سائز);
	props_ڈیٹا = textprop_save + PROP_گنتی_سائز;
	props_بائٹیں = pc * (صحیح)حجم(textprop_T);
	اگر (!متن_prop_گنتی_valid(pc, (size_t)textprop_len))
	    props_بائٹیں = 0;

	// Adjust text properties in the line above and below.
	اگر (lnum > 1)
	    adjust_متن_props_for_delete(buf, lnum - 1,
					     props_ڈیٹا, props_بائٹیں, TRUE);
	اگر (lnum <= buf->b_ml.ml_سطر_گنتی)
	    adjust_متن_props_for_delete(buf, lnum,
					    props_ڈیٹا, props_بائٹیں, FALSE);
    }
    vim_free(textprop_save);
#اگر_ختم
    واپس ret;
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
    صحیح
ml_delete(linenr_T lnum)
{
    واپس ml_delete_flags(lnum, 0);
}

/*
 * Like ml_delete() but using flags (see ml_delete_int()).
 */
    صحیح
ml_delete_flags(linenr_T lnum, صحیح flags)
{
    ml_flush_سطر(curbuf);
    اگر (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_گنتی)
	واپس FAIL;

#اگر_تعریف_شدہ FEAT_EVAL
    // When inserting above recorded changes: flush the changes before changing
    // the text.
    may_invoke_listeners(curbuf, lnum, lnum + 1, -1);
#اگر_ختم

    واپس ml_delete_int(curbuf, lnum, flags);
}

/*
 * set the DB_MARKED flag for line 'lnum'
 */
    خالی
ml_setmarked(linenr_T lnum)
{
    bhdr_T    *hp;
    ڈیٹا_BL *dp;
				    // invalid line number
    اگر (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_گنتی
					       || curbuf->b_ml.ml_mfp == تہی)
	واپس;			    // give error message?

    اگر (سب_سے_نیچا_marked == 0 || سب_سے_نیچا_marked > lnum)
	سب_سے_نیچا_marked = lnum;

    /*
     * find the data block containing the line
     * This also fills the stack with the blocks from the root to the data block
     * This also releases any locked block.
     */
    اگر ((hp = ml_ڈھونڈو_سطر(curbuf, lnum, ML_ڈھونڈو)) == تہی)
	واپس;		    // give error message?

    dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
    dp->db_اشاریہ[lnum - curbuf->b_ml.ml_locked_کم] |= DB_MARKED;
    curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
}

/*
 * find the first line with its DB_MARKED flag set
 */
    linenr_T
ml_firstmarked(خالی)
{
    bhdr_T	*hp;
    ڈیٹا_BL	*dp;
    linenr_T	lnum;
    صحیح		i;

    اگر (curbuf->b_ml.ml_mfp == تہی)
	واپس (linenr_T) 0;

    /*
     * The search starts with lowest_marked line. This is the last line where
     * a mark was found, adjusted by inserting/deleting lines.
     */
    برائے (lnum = سب_سے_نیچا_marked; lnum <= curbuf->b_ml.ml_سطر_گنتی; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block This also releases any locked block.
	 */
	اگر ((hp = ml_ڈھونڈو_سطر(curbuf, lnum, ML_ڈھونڈو)) == تہی)
	    واپس (linenr_T)0;		    // give error message?

	dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);

	برائے (i = lnum - curbuf->b_ml.ml_locked_کم;
			    lnum <= curbuf->b_ml.ml_locked_بلند; ++i, ++lnum)
	    اگر ((dp->db_اشاریہ[i]) & DB_MARKED)
	    {
		(dp->db_اشاریہ[i]) &= DB_اشاریہ_نقاب;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
		سب_سے_نیچا_marked = lnum + 1;
		واپس lnum;
	    }
    }

    واپس (linenr_T) 0;
}

/*
 * clear all DB_MARKED flags
 */
    خالی
ml_clearmarked(خالی)
{
    bhdr_T	*hp;
    ڈیٹا_BL	*dp;
    linenr_T	lnum;
    صحیح		i;

    اگر (curbuf->b_ml.ml_mfp == تہی)	    // nothing to do
	واپس;

    /*
     * The search starts with line lowest_marked.
     */
    برائے (lnum = سب_سے_نیچا_marked; lnum <= curbuf->b_ml.ml_سطر_گنتی; )
    {
	/*
	 * Find the data block containing the line.
	 * This also fills the stack with the blocks from the root to the data
	 * block and releases any locked block.
	 */
	اگر ((hp = ml_ڈھونڈو_سطر(curbuf, lnum, ML_ڈھونڈو)) == تہی)
	    واپس;		// give error message?

	dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);

	برائے (i = lnum - curbuf->b_ml.ml_locked_کم;
			    lnum <= curbuf->b_ml.ml_locked_بلند; ++i, ++lnum)
	    اگر ((dp->db_اشاریہ[i]) & DB_MARKED)
	    {
		(dp->db_اشاریہ[i]) &= DB_اشاریہ_نقاب;
		curbuf->b_ml.ml_flags |= ML_LOCKED_DIRTY;
	    }
    }

    سب_سے_نیچا_marked = 0;
}

/*
 * flush ml_line if necessary
 */
    ساکن خالی
ml_flush_سطر(buf_T *buf)
{
    bhdr_T	*hp;
    ڈیٹا_BL	*dp;
    linenr_T	lnum;
    char_u	*new_سطر;
    char_u	*old_سطر;
    colnr_T	new_len;
    صحیح		old_len;
    صحیح		extra;
    صحیح		idx;
    صحیح		آغاز;
    صحیح		گنتی;
    صحیح		i;
    ساکن صحیح  entered = FALSE;

    اگر (buf->b_ml.ml_سطر_lnum == 0 || buf->b_ml.ml_mfp == تہی)
	واپس;		// nothing to do

    اگر (buf->b_ml.ml_flags & ML_سطر_DIRTY)
    {
	// This code doesn't work recursively, but Netbeans may call back here
	// when obtaining the cursor position.
	اگر (entered)
	    واپس;
	entered = TRUE;

	lnum = buf->b_ml.ml_سطر_lnum;
	new_سطر = buf->b_ml.ml_سطر_ptr;

	hp = ml_ڈھونڈو_سطر(buf, lnum, ML_ڈھونڈو);
	اگر (hp == تہی)
	    siemsg(e_cannot_ڈھونڈو_سطر_nr, lnum);
	ورنہ
	{
	    dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
	    idx = lnum - buf->b_ml.ml_locked_کم;
	    آغاز = ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب);
	    old_سطر = (char_u *)dp + آغاز;
	    اگر (idx == 0)	// line is last in block
		old_len = dp->db_txt_انتہا - آغاز;
	    ورنہ		// text of previous line follows
		old_len = (dp->db_اشاریہ[idx - 1] & DB_اشاریہ_نقاب) - آغاز;
	    new_len = buf->b_ml.ml_سطر_len;
	    extra = new_len - old_len;	    // negative if lines gets smaller

	    /*
	     * if new line fits in data block, replace directly
	     */
	    اگر ((صحیح)dp->db_free >= extra)
	    {
#اگر defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		صحیح old_prop_len = 0;
		اگر (buf->b_has_textprop)
		    old_prop_len = old_len - (صحیح)STRLEN(old_سطر) - 1;
#اگر_ختم
		// if the length changes and there are following lines
		گنتی = buf->b_ml.ml_locked_بلند - buf->b_ml.ml_locked_کم + 1;
		اگر (extra != 0 && idx < گنتی - 1)
		{
		    // move text of following lines
		    mch_memmove((حرف *)dp + dp->db_txt_آغاز - extra,
				(حرف *)dp + dp->db_txt_آغاز,
				(size_t)(آغاز - dp->db_txt_آغاز));

		    // adjust pointers of this and following lines
		    برائے (i = idx + 1; i < گنتی; ++i)
			dp->db_اشاریہ[i] -= extra;
		}
		dp->db_اشاریہ[idx] -= extra;

		// adjust free space
		dp->db_free -= extra;
		dp->db_txt_آغاز -= extra;

		// copy new line into the data block
		mch_memmove(old_سطر - extra, new_سطر, (size_t)new_len);
		buf->b_ml.ml_flags |= (ML_LOCKED_DIRTY | ML_LOCKED_POS);
#اگر defined(FEAT_BYTEOFF) && defined(FEAT_PROP_POPUP)
		// The else case is already covered by the insert and delete
		اگر (buf->b_has_textprop)
		{
		    // Do not count the size of any text properties.
		    extra += old_prop_len;
		    extra -= new_len - (صحیح)STRLEN(new_سطر) - 1;
		}
		اگر (extra != 0)
		    ml_updatechunk(buf, lnum, (طویل)extra, ML_CHNK_UPDLINE);
#اگر_ختم
	    }
	    ورنہ
	    {
		/*
		 * Cannot do it in one data block: Delete and append.
		 * Append first, because ml_delete_int() cannot delete the
		 * last line in a buffer, which causes trouble for a buffer
		 * that has only one line.
		 * Don't forget to copy the mark!
		 */
		// How about handling errors???
		(خالی)ml_append_int(buf, lnum, new_سطر, new_len,
			 ((dp->db_اشاریہ[idx] & DB_MARKED) ? ML_APPEND_MARK : 0)
#اگر_تعریف_شدہ FEAT_PROP_POPUP
			     | ML_APPEND_NOPROP
#اگر_ختم
			 );
		(خالی)ml_delete_int(buf, lnum, ML_DEL_NOPROP);
	    }
	}
	vim_free(new_سطر);

	entered = FALSE;
    }
    ورنہ اگر (buf->b_ml.ml_flags & ML_ALLOCATED)
	vim_free(buf->b_ml.ml_سطر_ptr);

    buf->b_ml.ml_flags &= ~(ML_سطر_DIRTY | ML_ALLOCATED);
    buf->b_ml.ml_سطر_lnum = 0;
}

/*
 * create a new, empty, data block
 */
    ساکن bhdr_T *
ml_new_ڈیٹا(memfile_T *mfp, صحیح negative, صحیح صفحہ_گنتی)
{
    bhdr_T	*hp;
    ڈیٹا_BL	*dp;

    اگر ((hp = mf_new(mfp, negative, صفحہ_گنتی)) == تہی)
	واپس تہی;

    dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
    dp->db_id = ڈیٹا_ID;
    dp->db_txt_آغاز = dp->db_txt_انتہا = صفحہ_گنتی * mfp->mf_صفحہ_سائز;
    dp->db_free = dp->db_txt_آغاز - سرخی_سائز;
    dp->db_سطر_گنتی = 0;

    واپس hp;
}

/*
 * create a new, empty, pointer block
 */
    ساکن bhdr_T *
ml_new_ptr(memfile_T *mfp)
{
    bhdr_T	*hp;
    PTR_BL	*pp;

    اگر ((hp = mf_new(mfp, FALSE, 1)) == تہی)
	واپس تہی;

    pp = (PTR_BL *)(hp->bh_ڈیٹا);
    pp->pb_id = PTR_ID;
    pp->pb_گنتی = 0;
    pp->pb_گنتی_max = PB_گنتی_MAX(mfp);

    واپس hp;
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
    ساکن bhdr_T *
ml_ڈھونڈو_سطر(buf_T *buf, linenr_T lnum, صحیح action)
{
    ڈیٹا_BL	*dp;
    PTR_BL	*pp;
    infoptr_T	*ip;
    bhdr_T	*hp;
    memfile_T	*mfp;
    linenr_T	t;
    blocknr_T	bnum, bnum2;
    صحیح		dirty;
    linenr_T	کم, بلند;
    صحیح		چوٹی;
    صحیح		صفحہ_گنتی;
    صحیح		idx;

    mfp = buf->b_ml.ml_mfp;

    /*
     * If there is a locked block check if the wanted line is in it.
     * If not, flush and release the locked block.
     * Don't do this for ML_INSERT_SAME, because the stack need to be updated.
     * Don't do this for ML_FLUSH, because we want to flush the locked block.
     * Don't do this when 'swapfile' is reset, we want to load all the blocks.
     */
    اگر (buf->b_ml.ml_locked)
    {
	اگر (ML_SIMPLE(action)
		&& buf->b_ml.ml_locked_کم <= lnum
		&& buf->b_ml.ml_locked_بلند >= lnum
		&& !mf_dont_release)
	{
	    // remember to update pointer blocks and stack later
	    اگر (action == ML_داخل_کرو)
	    {
		++(buf->b_ml.ml_locked_lineadd);
		++(buf->b_ml.ml_locked_بلند);
	    }
	    ورنہ اگر (action == ML_DELETE)
	    {
		--(buf->b_ml.ml_locked_lineadd);
		--(buf->b_ml.ml_locked_بلند);
	    }
	    واپس (buf->b_ml.ml_locked);
	}

	mf_put(mfp, buf->b_ml.ml_locked, buf->b_ml.ml_flags & ML_LOCKED_DIRTY,
					    buf->b_ml.ml_flags & ML_LOCKED_POS);
	buf->b_ml.ml_locked = تہی;

	/*
	 * If lines have been added or deleted in the locked block, need to
	 * update the line count in pointer blocks.
	 */
	اگر (buf->b_ml.ml_locked_lineadd != 0)
	    ml_lineadd(buf, buf->b_ml.ml_locked_lineadd);
    }

    اگر (action == ML_FLUSH)	    // nothing else to do
	واپس تہی;

    bnum = 1;			    // start at the root of the tree
    صفحہ_گنتی = 1;
    کم = 1;
    بلند = buf->b_ml.ml_سطر_گنتی;

    اگر (action == ML_ڈھونڈو)	// first try stack entries
    {
	برائے (چوٹی = buf->b_ml.ml_ڈھیر_چوٹی - 1; چوٹی >= 0; --چوٹی)
	{
	    ip = &(buf->b_ml.ml_ڈھیر[چوٹی]);
	    اگر (ip->ip_کم <= lnum && ip->ip_بلند >= lnum)
	    {
		bnum = ip->ip_bnum;
		کم = ip->ip_کم;
		بلند = ip->ip_بلند;
		buf->b_ml.ml_ڈھیر_چوٹی = چوٹی;	// truncate stack at prev entry
		توڑو;
	    }
	}
	اگر (چوٹی < 0)
	    buf->b_ml.ml_ڈھیر_چوٹی = 0;		// not found, start at the root
    }
    ورنہ	// ML_DELETE or ML_INSERT
	buf->b_ml.ml_ڈھیر_چوٹی = 0;	// start at the root

/*
 * search downwards in the tree until a data block is found
 */
    برائے (;;)
    {
	اگر ((hp = mf_get(mfp, bnum, صفحہ_گنتی)) == تہی)
	    جاؤ error_noblock;

	/*
	 * update high for insert/delete
	 */
	اگر (action == ML_داخل_کرو)
	    ++بلند;
	ورنہ اگر (action == ML_DELETE)
	    --بلند;

	dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
	اگر (dp->db_id == ڈیٹا_ID)	// data block
	{
	    buf->b_ml.ml_locked = hp;
	    buf->b_ml.ml_locked_کم = کم;
	    buf->b_ml.ml_locked_بلند = بلند;
	    buf->b_ml.ml_locked_lineadd = 0;
	    buf->b_ml.ml_flags &= ~(ML_LOCKED_DIRTY | ML_LOCKED_POS);
	    واپس hp;
	}

	pp = (PTR_BL *)(dp);		// must be pointer block
	اگر (pp->pb_id != PTR_ID)
	{
	    iemsg(e_اشارہ_گر_بلاک_id_wrong);
	    جاؤ error_بلاک;
	}

	اگر ((چوٹی = ml_جوڑو_ڈھیر(buf)) < 0)	// add new entry to stack
	    جاؤ error_بلاک;
	ip = &(buf->b_ml.ml_ڈھیر[چوٹی]);
	ip->ip_bnum = bnum;
	ip->ip_کم = کم;
	ip->ip_بلند = بلند;
	ip->ip_اشاریہ = -1;		// index not known yet

	dirty = FALSE;
	برائے (idx = 0; idx < (صحیح)pp->pb_گنتی; ++idx)
	{
	    t = pp->pb_اشارہ_گر[idx].pe_سطر_گنتی;
	    CHECK(t == 0, _("pe_line_count is zero"));
	    اگر ((کم += t) > lnum)
	    {
		ip->ip_اشاریہ = idx;
		bnum = pp->pb_اشارہ_گر[idx].pe_bnum;
		صفحہ_گنتی = pp->pb_اشارہ_گر[idx].pe_صفحہ_گنتی;
		بلند = کم - 1;
		کم -= t;

		/*
		 * a negative block number may have been changed
		 */
		اگر (bnum < 0)
		{
		    bnum2 = mf_trans_del(mfp, bnum);
		    اگر (bnum != bnum2)
		    {
			bnum = bnum2;
			pp->pb_اشارہ_گر[idx].pe_bnum = bnum;
			dirty = TRUE;
		    }
		}

		توڑو;
	    }
	}
	اگر (idx >= (صحیح)pp->pb_گنتی)	    // past the end: something wrong!
	{
	    اگر (lnum > buf->b_ml.ml_سطر_گنتی)
		siemsg(e_سطر_نمبر_برآمد_کا_range_nr_past_the_انتہا,
					      lnum - buf->b_ml.ml_سطر_گنتی);

	    ورنہ
		siemsg(e_سطر_گنتی_wrong_اندر_بلاک_nr, bnum);
	    جاؤ error_بلاک;
	}
	اگر (action == ML_DELETE)
	{
	    pp->pb_اشارہ_گر[idx].pe_سطر_گنتی--;
	    dirty = TRUE;
	}
	ورنہ اگر (action == ML_داخل_کرو)
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
    اگر (action == ML_DELETE)
	ml_lineadd(buf, 1);
    ورنہ اگر (action == ML_داخل_کرو)
	ml_lineadd(buf, -1);
    buf->b_ml.ml_ڈھیر_چوٹی = 0;
    واپس تہی;
}

/*
 * add an entry to the info pointer stack
 *
 * return -1 for failure, number of the new entry otherwise
 */
    ساکن صحیح
ml_جوڑو_ڈھیر(buf_T *buf)
{
    صحیح		چوٹی;
    infoptr_T	*newstack;

    چوٹی = buf->b_ml.ml_ڈھیر_چوٹی;

    // may have to increase the stack size
    اگر (چوٹی == buf->b_ml.ml_ڈھیر_سائز)
    {
	CHECK(چوٹی > 0, _("Stack size increases")); // more than 5 levels???

	newstack = ALLOC_MULT(infoptr_T, buf->b_ml.ml_ڈھیر_سائز + ڈھیر_INCR);
	اگر (newstack == تہی)
	    واپس -1;
	اگر (چوٹی > 0)
	    mch_memmove(newstack, buf->b_ml.ml_ڈھیر,
					     (size_t)چوٹی * حجم(infoptr_T));
	vim_free(buf->b_ml.ml_ڈھیر);
	buf->b_ml.ml_ڈھیر = newstack;
	buf->b_ml.ml_ڈھیر_سائز += ڈھیر_INCR;
    }

    buf->b_ml.ml_ڈھیر_چوٹی++;
    واپس چوٹی;
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
    ساکن خالی
ml_lineadd(buf_T *buf, صحیح گنتی)
{
    صحیح		idx;
    infoptr_T	*ip;
    PTR_BL	*pp;
    memfile_T	*mfp = buf->b_ml.ml_mfp;
    bhdr_T	*hp;

    برائے (idx = buf->b_ml.ml_ڈھیر_چوٹی - 1; idx >= 0; --idx)
    {
	ip = &(buf->b_ml.ml_ڈھیر[idx]);
	اگر ((hp = mf_get(mfp, ip->ip_bnum, 1)) == تہی)
	    توڑو;
	pp = (PTR_BL *)(hp->bh_ڈیٹا);	// must be pointer block
	اگر (pp->pb_id != PTR_ID)
	{
	    mf_put(mfp, hp, FALSE, FALSE);
	    iemsg(e_اشارہ_گر_بلاک_id_wrong_two);
	    توڑو;
	}
	pp->pb_اشارہ_گر[ip->ip_اشاریہ].pe_سطر_گنتی += گنتی;
	ip->ip_بلند += گنتی;
	mf_put(mfp, hp, TRUE, FALSE);
    }
}

#اگر defined(HAVE_READLINK)
/*
 * Resolve a symlink in the last component of a file name.
 * Note that f_resolve() does it for every part of the path, we don't do that
 * here.
 * If it worked returns OK and the resolved link in "buf[MAXPATHL]".
 * Otherwise returns FAIL.
 */
    صحیح
resolve_symlink(char_u *fname, char_u *buf)
{
    char_u	tmp[MAXPATHL];
    صحیح		ret;
    صحیح		گہرائی = 0;

    اگر (fname == تہی)
	واپس FAIL;

    // Put the result so far in tmp[], starting with the original name.
    vim_strncpy(tmp, fname, MAXPATHL - 1);

    برائے (;;)
    {
	// Limit symlink depth to 100, catch recursive loops.
	اگر (++گہرائی == 100)
	{
	    semsg(_(e_symlink_loop_for_str), fname);
	    واپس FAIL;
	}

	ret = readlink((حرف *)tmp, (حرف *)buf, MAXPATHL - 1);
	اگر (ret <= 0)
	{
	    اگر (errno == EINVAL || errno == ENOENT)
	    {
		// Found non-symlink or not existing file, stop here.
		// When at the first level use the unmodified name, skip the
		// call to vim_FullName().
		اگر (گہرائی == 1)
		    واپس FAIL;

		// Use the resolved name in tmp[].
		توڑو;
	    }

	    // There must be some error reading links, use original name.
	    واپس FAIL;
	}
	buf[ret] = NUL;

	/*
	 * Check whether the symlink is relative or absolute.
	 * If it's relative, build a new path based on the directory
	 * portion of the filename (if any) and the path the symlink
	 * points to.
	 */
	اگر (mch_is_بھرا_Name(buf))
	    STRCPY(tmp, buf);
	ورنہ
	{
	    char_u *دم;

	    دم = gettail(tmp);
	    اگر (STRLEN(دم) + STRLEN(buf) >= MAXPATHL)
		واپس FAIL;
	    STRCPY(دم, buf);
	}
    }

    /*
     * Try to resolve the full name of the file so that the swapfile name will
     * be consistent even when opening a relative symlink from different
     * working directories.
     */
    واپس vim_بھرا_Name(tmp, buf, MAXPATHL, TRUE);
}
#اگر_ختم

/*
 * Make swap file name out of the file name and a directory name.
 * Returns pointer to allocated memory or NULL.
 */
    char_u *
makeswapname(
    char_u	*fname,
    char_u	*ffname غیر_مستعمل,
    buf_T	*buf,
    char_u	*dir_name)
{
    char_u	*r, *s;
    char_u	*fname_res = fname;
#اگر_تعریف_شدہ HAVE_READLINK
    char_u	fname_buf[MAXPATHL];

    // Expand symlink in the file name, so that we put the swap file with the
    // actual file instead of with the symlink.
    اگر (resolve_symlink(fname, fname_buf) == OK)
	fname_res = fname_buf;
#اگر_ختم

#اگر defined(UNIX) || defined(MSWIN)  // Need _very_ long file names
    صحیح		len = (صحیح)STRLEN(dir_name);

    s = dir_name + len;
    اگر (after_pathsep(dir_name, s) && len > 1 && s[-1] == s[-2])
    {			       // Ends with '//', Use Full path
	r = تہی;
	اگر ((s = make_percent_swname(dir_name, s, fname_res)) != تہی)
	{
	    r = modname(s, (char_u *)".swp", FALSE);
	    vim_free(s);
	}
	واپس r;
    }
#اگر_ختم

    r = buf_modname(
	    (buf->b_p_sn || buf->b_shortname),
	    fname_res,
	    (char_u *)
#اگر defined(VMS)
	    "_swp",
#ورنہ
	    ".swp",
#اگر_ختم
	    // Prepend a '.' to the swap file name for the current directory.
	    dir_name[0] == '.' && dir_name[1] == NUL);
    اگر (r == تہی)	    // out of memory
	واپس تہی;

    s = get_file_اندر_dir(r, dir_name);
    vim_free(r);
    واپس s;
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
get_file_اندر_dir(
    char_u  *fname,
    char_u  *dname)	// don't use "dirname", it is a global for Alpha
{
    string_T	دم;
    string_T	retval;

    دم.string = gettail(fname);
    دم.لمبائی = STRLEN(دم.string);

    اگر (dname[0] == '.' && dname[1] == NUL)
	retval.string =
	    vim_strnsave(fname, (size_t)(دم.string - fname) + دم.لمبائی);
    ورنہ
    {
	size_t	dname_len = STRLEN(dname);

	اگر (dname[0] == '.' && vim_ispathsep(dname[1]))
	{
	    اگر (دم.string == fname)	    // no path before file name
		concat_fnames(dname + 2, dname_len - 2,
		    دم.string, دم.لمبائی, TRUE, &retval);
	    ورنہ
	    {
		صحیح	    save_char;
		string_T    tmp;

		save_char = *دم.string;
		*دم.string = NUL;
		concat_fnames(fname, (size_t)(دم.string - fname),
		    dname + 2, dname_len - 2, TRUE, &tmp);
		*دم.string = save_char;
		اگر (tmp.string == تہی)	    // out of memory
		    retval.string = تہی;
		ورنہ
		{
		    concat_fnames(tmp.string, tmp.لمبائی,
			دم.string, دم.لمبائی, TRUE, &retval);
		    vim_free(tmp.string);
		}
	    }
	}
	ورنہ
	    concat_fnames(dname, dname_len, دم.string, دم.لمبائی,
		TRUE, &retval);
    }

#اگر_تعریف_شدہ MSWIN
    اگر (retval.string != تہی)
    {
	char_u	*t;

	برائے (t = gettail(retval.string); *t != NUL; MB_PTR_ADV(t))
	    اگر (*t == ':')
		*t = '%';
    }
#اگر_ختم

    واپس retval.string;
}

/*
 * Print the ATTENTION message: info about an existing swap file.
 */
    ساکن خالی
attention_پیغام(
    buf_T   *buf,	// buffer being edited
    char_u  *fname)	// swap file name
{
    stat_T	st;
    time_t	تبادلہ_mtime;

    ++no_wait_return;
    (خالی)emsg(_(e_attention));
    پیغام_puts(_("\nFound a swap file by the name \""));
    پیغام_home_replace(fname);
    پیغام_puts("\"\n");
    تبادلہ_mtime = swapfile_info(fname);
    پیغام_puts(_("While opening file \""));
    پیغام_outtrans(buf->b_fname);
    پیغام_puts("\"\n");
    اگر (mch_stat((حرف *)buf->b_fname, &st) == -1)
    {
	پیغام_puts(_("      CANNOT BE FOUND"));
    }
    ورنہ
    {
	پیغام_puts(_("             dated: "));
	پیغام_puts(get_ctime(st.st_mtime, TRUE));
	اگر (تبادلہ_mtime != 0 && st.st_mtime > تبادلہ_mtime)
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

قسمتعریف شمار {
    SEA_CHOICE_NONE = 0,
    SEA_CHOICE_READONLY = 1,
    SEA_CHOICE_EDIT = 2,
    SEA_CHOICE_RECOVER = 3,
    SEA_CHOICE_DELETE = 4,
    SEA_CHOICE_QUIT = 5,
    SEA_CHOICE_ABORT = 6
} sea_choice_T;

#اگر defined(FEAT_EVAL)
/*
 * Trigger the SwapExists autocommands.
 * Returns a value for equivalent to do_dialog().
 */
    ساکن sea_choice_T
do_swapexists(buf_T *buf, char_u *fname)
{
    سیٹ_vim_var_string(VV_SWAPNAME, fname, -1);
    سیٹ_vim_var_string(VV_SWAPCHOICE, تہی, -1);

    // Trigger SwapExists autocommands with <afile> set to the file being
    // edited.  Disallow changing directory here.
    ++allbuf_تالا;
    apply_autocmds(EVENT_SWAPEXISTS, buf->b_fname, تہی, FALSE, تہی);
    --allbuf_تالا;

    سیٹ_vim_var_string(VV_SWAPNAME, تہی, -1);

    انتخاب (*get_vim_var_str(VV_SWAPCHOICE))
    {
	حالت 'o': واپس SEA_CHOICE_READONLY;
	حالت 'e': واپس SEA_CHOICE_EDIT;
	حالت 'r': واپس SEA_CHOICE_RECOVER;
	حالت 'd': واپس SEA_CHOICE_DELETE;
	حالت 'q': واپس SEA_CHOICE_QUIT;
	حالت 'a': واپس SEA_CHOICE_ABORT;
    }

    واپس SEA_CHOICE_NONE;
}
#اگر_ختم

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
    ساکن char_u *
findswapname(
    buf_T	*buf,
    char_u	**dirp,		// pointer to list of directories
    char_u	*old_fname)	// don't give warning for this file name
{
    char_u	*fname;
    صحیح		n;
    char_u	*dir_name;
#اگر_تعریف_شدہ AMIGA
    BPTR	fh;
#اگر_ختم
    صحیح		r;
    char_u	*buf_fname = buf->b_fname;

#اگر !defined(UNIX)
# define CREATE_DUMMY_FILE
    FILE	*dummyfd = تہی;

# ifdef MSWIN
    اگر (buf_fname != تہی && !mch_is_بھرا_Name(buf_fname)
				       && vim_strchr(gettail(buf_fname), ':'))
    {
	char_u *t;

	buf_fname = vim_strsave(buf_fname);
	اگر (buf_fname == تہی)
	    buf_fname = buf->b_fname;
	ورنہ
	    برائے (t = gettail(buf_fname); *t != NUL; MB_PTR_ADV(t))
		اگر (*t == ':')
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
    اگر (!(buf->b_p_sn || buf->b_shortname) && buf_fname != تہی
					     && mch_getperm(buf_fname) < 0)
	dummyfd = mch_fopen((حرف *)buf_fname, "w");
#اگر_ختم

    /*
     * Isolate a directory name from *dirp and put it in dir_name.
     * First allocate some memory to put the directory name in.
     */
    dir_name = alloc(STRLEN(*dirp) + 1);
    اگر (dir_name == تہی)
	*dirp = تہی;
    ورنہ
	(خالی)copy_option_part(dirp, dir_name, 31000, ",");

    /*
     * we try different names until we find one that does not exist yet
     */
    اگر (dir_name == تہی)	    // out of memory
	fname = تہی;
    ورنہ
	fname = makeswapname(buf_fname, buf->b_ffname, buf, dir_name);

    برائے (;;)
    {
	اگر (fname == تہی)	// must be out of memory
	    توڑو;
	اگر ((n = (صحیح)STRLEN(fname)) == 0)	// safety check
	{
	    VIM_CLEAR(fname);
	    توڑو;
	}
#اگر defined(UNIX)
/*
 * Some systems have a MS-DOS compatible filesystem that use 8.3 character
 * file names. If this is the first try and the swap file name does not fit in
 * 8.3, detect if this is the case, set shortname and try again.
 */
	اگر (fname[n - 2] == 'w' && fname[n - 1] == 'p'
					&& !(buf->b_p_sn || buf->b_shortname))
	{
	    char_u	    *دم;
	    char_u	    *fname2;
	    stat_T	    s1, s2;
	    صحیح		    f1, f2;
	    صحیح		    created1 = FALSE, created2 = FALSE;
	    صحیح		    same = FALSE;

	    /*
	     * Check if swapfile name does not fit in 8.3:
	     * It either contains two dots, is longer than 8 chars, or starts
	     * with a dot.
	     */
	    دم = gettail(buf_fname);
	    اگر (       vim_strchr(دم, '.') != تہی
		    || STRLEN(دم) > (size_t)8
		    || *gettail(fname) == '.')
	    {
		fname2 = alloc(n + 2);
		اگر (fname2 != تہی)
		{
		    STRCPY(fname2, fname);
		    // if fname == "xx.xx.swp",	    fname2 = "xx.xx.swx"
		    // if fname == ".xx.swp",	    fname2 = ".xx.swpx"
		    // if fname == "123456789.swp", fname2 = "12345678x.swp"
		    اگر (vim_strchr(دم, '.') != تہی)
			fname2[n - 1] = 'x';
		    ورنہ اگر (*gettail(fname) == '.')
		    {
			fname2[n] = 'x';
			fname2[n + 1] = NUL;
		    }
		    ورنہ
			fname2[n - 5] += 1;
		    /*
		     * may need to create the files to be able to use mch_stat()
		     */
		    f1 = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
		    اگر (f1 < 0)
		    {
			f1 = mch_open_rw((حرف *)fname,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			created1 = TRUE;
		    }
		    اگر (f1 >= 0)
		    {
			f2 = mch_open((حرف *)fname2, O_RDONLY | O_EXTRA, 0);
			اگر (f2 < 0)
			{
			    f2 = mch_open_rw((حرف *)fname2,
					       O_RDWR|O_CREAT|O_EXCL|O_EXTRA);
			    created2 = TRUE;
			}
			اگر (f2 >= 0)
			{
			    /*
			     * Both files exist now. If mch_stat() returns the
			     * same device and inode they are the same file.
			     */
			    اگر (mch_fstat(f1, &s1) != -1
				    && mch_fstat(f2, &s2) != -1
				    && s1.st_dev == s2.st_dev
				    && s1.st_ino == s2.st_ino)
				same = TRUE;
			    close(f2);
			    اگر (created2)
				mch_remove(fname2);
			}
			close(f1);
			اگر (created1)
			    mch_remove(fname);
		    }
		    vim_free(fname2);
		    اگر (same)
		    {
			buf->b_shortname = true;
			vim_free(fname);
			fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
			جاری;	// try again with b_shortname set
		    }
		}
	    }
	}
#اگر_ختم
	/*
	 * check if the swapfile already exists
	 */
	اگر (mch_getperm(fname) < 0)	// it does not exist
	{
#اگر_تعریف_شدہ HAVE_LSTAT
	    stat_T	sb;

	    /*
	     * Extra security check: When a swap file is a symbolic link, this
	     * is most likely a symlink attack.
	     */
	    اگر (mch_lstat((حرف *)fname, &sb) < 0)
#ورنہ
# ifdef AMIGA
	    fh = Open((UBYTE *)fname, (طویل)منوال_NEWFILE);
	    /*
	     * on the Amiga mch_getperm() will return -1 when the file exists
	     * but is being used by another program. This happens if you edit
	     * a file twice.
	     */
	    اگر (fh != (BPTR)تہی)	// can open file, OK
	    {
		Close(fh);
		mch_remove(fname);
		توڑو;
	    }
	    اگر (IoErr() != ERROR_OBJECT_اندر_USE
					    && IoErr() != ERROR_OBJECT_EXISTS)
# endif
#اگر_ختم
		توڑو;
	}

	/*
	 * A file name equal to old_fname is OK to use.
	 */
	اگر (old_fname != تہی && fnamecmp(fname, old_fname) == 0)
	    توڑو;

	/*
	 * get here when file already exists
	 */
	اگر (fname[n - 2] == 'w' && fname[n - 1] == 'p')	// first try
	{
	    /*
	     * on MS-DOS compatible filesystems (e.g. messydos) file.doc.swp
	     * and file.doc are the same file. To guess if this problem is
	     * present try if file.doc.swx exists. If it does, we set
	     * buf->b_shortname and try file_doc.swp (dots replaced by
	     * underscores for this file), and try again. If it doesn't we
	     * assume that "file.doc.swp" already exists.
	     */
	    اگر (!(buf->b_p_sn || buf->b_shortname))	// not tried yet
	    {
		fname[n - 1] = 'x';
		r = mch_getperm(fname);		// try "file.swx"
		fname[n - 1] = 'p';
		اگر (r >= 0)		    // "file.swx" seems to exist
		{
		    buf->b_shortname = true;
		    vim_free(fname);
		    fname = makeswapname(buf_fname, buf->b_ffname,
							       buf, dir_name);
		    جاری;	    // try again with '.' replaced with '_'
		}
	    }
	    /*
	     * If we get here the ".swp" file really exists.
	     * Give an error message, unless recovering, no file name, we are
	     * viewing a help file or when the path of the file is different
	     * (happens when all .swp files are in one directory).
	     */
	    اگر (!recoverymode && buf_fname != تہی
				&& !buf->b_help
				&& !(buf->b_flags & (BF_DUMMY | BF_NO_SEA)))
	    {
		صحیح		fd;
		ساخت block0	b0;
		صحیح		فرق = FALSE;

		/*
		 * Try to read block 0 from the swap file to get the original
		 * file name (and inode number).
		 */
		fd = mch_open((حرف *)fname, O_RDONLY | O_EXTRA, 0);
		اگر (fd >= 0)
		{
		    اگر (read_eintr(fd, &b0, حجم(b0)) == حجم(b0))
		    {
			/*
			 * If the swapfile has the same directory as the
			 * buffer don't compare the directory names, they can
			 * have a different mountpoint.
			 */
			اگر (b0.b0_flags & B0_SAME_DIR)
			{
			    اگر (fnamecmp(gettail(buf->b_ffname),
						   gettail(b0.b0_fname)) != 0
				    || !same_directory(fname, buf->b_ffname))
			    {
#اگر_تعریف_شدہ CHECK_INODE
				// Symlinks may point to the same file even
				// when the name differs, need to check the
				// inode too.
				expand_env(b0.b0_fname, NameBuff, MAXPATHL);
				اگر (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_مقصد_long(b0.b0_ino)))
#اگر_ختم
				    فرق = TRUE;
			    }
			}
			ورنہ
			{
			    /*
			     * The name in the swap file may be
			     * "~user/path/file".  Expand it first.
			     */
			    expand_env(b0.b0_fname, NameBuff, MAXPATHL);
#اگر_تعریف_شدہ CHECK_INODE
			    اگر (fnamecmp_ino(buf->b_ffname, NameBuff,
						     char_مقصد_long(b0.b0_ino)))
				فرق = TRUE;
#ورنہ
			    اگر (fnamecmp(NameBuff, buf->b_ffname) != 0)
				فرق = TRUE;
#اگر_ختم
			}
		    }
		    close(fd);
		}

		// give the ATTENTION message when there is an old swap file
		// for the current file, and the buffer was not recovered.
		اگر (فرق == FALSE && !(curbuf->b_flags & BF_RECOVERED)
			&& vim_strchr(p_shm, SHM_ATTENTION) == تہی)
		{
		    sea_choice_T choice = SEA_CHOICE_NONE;
		    stat_T	 st;
#اگر_تعریف_شدہ CREATE_DUMMY_FILE
		    صحیح		 did_use_dummy = FALSE;

		    // Avoid getting a warning for the file being created
		    // outside of Vim, it was created at the start of this
		    // function.  Delete the file now, because Vim might exit
		    // here if the window is closed.
		    اگر (dummyfd != تہی)
		    {
			fclose(dummyfd);
			dummyfd = تہی;
			mch_remove(buf_fname);
			did_use_dummy = TRUE;
		    }
#اگر_ختم

#اگر_تعریف_شدہ HAVE_PROCESS_STILL_RUNNING
		    process_still_running = FALSE;
#اگر_ختم
		    // It's safe to delete the swap file if all these are true:
		    // - the edited file exists
		    // - the swap file has no changes and looks OK
		    اگر (mch_stat((حرف *)buf->b_fname, &st) == 0
						  && swapfile_unchanged(fname))
		    {
			choice = SEA_CHOICE_DELETE;
			اگر (p_verbose > 0)
			    verb_پیغام(_("Found a swap file that is not useful, deleting it"));
		    }

#اگر defined(FEAT_EVAL)
		    /*
		     * If there is an SwapExists autocommand and we can handle
		     * the response, trigger it.  It may return 0 to ask the
		     * user anyway.
		     */
		    اگر (choice == SEA_CHOICE_NONE
			    && تبادلہ_exists_action != SEA_NONE
			    && has_autocmd(EVENT_SWAPEXISTS, buf_fname, buf))
			choice = do_swapexists(buf, fname);
#اگر_ختم

		    اگر (choice == SEA_CHOICE_NONE
					 && تبادلہ_exists_action == SEA_READONLY)
		    {
			// always open readonly.
			choice = SEA_CHOICE_READONLY;
		    }

		    اگر (choice == SEA_CHOICE_NONE)
		    {
#اگر_تعریف_شدہ FEAT_GUI
			// If we are supposed to start the GUI but it wasn't
			// completely started yet, start it now.  This makes
			// the messages displayed in the Vim window when
			// loading a session from the .gvimrc file.
			اگر (gui.starting && !gui.اندر_use)
			    gui_آغاز(تہی);
#اگر_ختم
			// Show info about the existing swap file.
			attention_پیغام(buf, fname);

			// We don't want a 'q' typed at the more-prompt
			// interrupt loading a file.
			got_int = FALSE;

			// If vimrc has "simalt ~x" we don't want it to
			// interfere with the prompt here.
			flush_buffers(FLUSH_TYPEAHEAD);
		    }

#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
		    اگر (تبادلہ_exists_action != SEA_NONE
						  && choice == SEA_CHOICE_NONE)
		    {
			string_T    سابقہ = {(char_u *)_("Swap file \""), 0};
			string_T    suffix = {(char_u *)_("\" already exists!"), 0};
			size_t	    پیغام_سائز;
			string_T    پیغام_2;
			char_u	    *tofree;
			صحیح	    dialog_نتیجہ;

			سابقہ.لمبائی = STRLEN(سابقہ.string);
			suffix.لمبائی = STRLEN(suffix.string);
			پیغام_سائز = سابقہ.لمبائی
				+ STRLEN(fname)
				+ suffix.لمبائی + 5;
			پیغام_2.string = tofree = alloc(پیغام_سائز);
			اگر (پیغام_2.string != تہی)
			{
			    STRCPY(پیغام_2.string, سابقہ.string);
			    پیغام_2.لمبائی = سابقہ.لمبائی;
			    پیغام_2.لمبائی += home_replace(تہی, fname,
				پیغام_2.string + پیغام_2.لمبائی, (صحیح)(پیغام_سائز - پیغام_2.لمبائی), TRUE);
			    STRCPY(پیغام_2.string + پیغام_2.لمبائی, suffix.string);
			}
			ورنہ
			    پیغام_2.string = (char_u *)_("Swap file already exists!");
			dialog_نتیجہ = do_dialog(VIM_WARNING,
				    (char_u *)_("VIM - ATTENTION"),
				    پیغام_2.string,
# ifdef HAVE_PROCESS_STILL_RUNNING
				    process_still_running
					? (char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Quit\n&Abort") :
# endif
					(char_u *)_("&Open Read-Only\n&Edit anyway\n&Recover\n&Delete it\n&Quit\n&Abort"), 1, تہی, FALSE);

# ifdef HAVE_PROCESS_STILL_RUNNING
			اگر (process_still_running && dialog_نتیجہ >= 4)
			    // compensate for missing "Delete it" button
			    dialog_نتیجہ++;
# endif
			choice = dialog_نتیجہ;
			vim_free(tofree);

			// pretend screen didn't scroll, need redraw anyway
			پیغام_scrolled = 0;
			redraw_all_later(UPD_NOT_VALID);
		    }
#اگر_ختم

		    انتخاب (choice)
		    {
			حالت SEA_CHOICE_READONLY:
			    buf->b_p_ro = TRUE;
			    توڑو;
			حالت SEA_CHOICE_EDIT:
			    توڑو;
			حالت SEA_CHOICE_RECOVER:
			    تبادلہ_exists_action = SEA_RECOVER;
			    توڑو;
			حالت SEA_CHOICE_DELETE:
			    mch_remove(fname);
			    توڑو;
			حالت SEA_CHOICE_QUIT:
			    تبادلہ_exists_action = SEA_QUIT;
			    توڑو;
			حالت SEA_CHOICE_ABORT:
			    تبادلہ_exists_action = SEA_QUIT;
			    got_int = TRUE;
			    توڑو;
			حالت SEA_CHOICE_NONE:
			    پیغام_puts("\n");
			    اگر (پیغام_silent == 0)
				// call wait_return() later
				need_wait_return = TRUE;
			    توڑو;
		    }

		    // If the file was deleted this fname can be used.
		    اگر (choice != SEA_CHOICE_NONE && mch_getperm(fname) < 0)
			توڑو;

#اگر_تعریف_شدہ CREATE_DUMMY_FILE
		    // Going to try another name, need the dummy file again.
		    اگر (did_use_dummy)
			dummyfd = mch_fopen((حرف *)buf_fname, "w");
#اگر_ختم
		}
	    }
	}

	/*
	 * Change the ".swp" extension to find another file that can be used.
	 * First decrement the last char: ".swo", ".swn", etc.
	 * If that still isn't enough decrement the last but one char: ".svz"
	 * Can happen when editing many "No Name" buffers.
	 */
	اگر (fname[n - 1] == 'a')	// ".s?a"
	{
	    اگر (fname[n - 2] == 'a')    // ".saa": tried enough, give up
	    {
		emsg(_(e_too_many_تبادلہ_files_ملا));
		VIM_CLEAR(fname);
		توڑو;
	    }
	    --fname[n - 2];		// ".svz", ".suz", etc.
	    fname[n - 1] = 'z' + 1;
	}
	--fname[n - 1];			// ".swo", ".swn", etc.
    }

    vim_free(dir_name);
#اگر_تعریف_شدہ CREATE_DUMMY_FILE
    اگر (dummyfd != تہی)	// file has been created temporarily
    {
	fclose(dummyfd);
	mch_remove(buf_fname);
    }
#اگر_ختم
#اگر_تعریف_شدہ MSWIN
    اگر (buf_fname != buf->b_fname)
	vim_free(buf_fname);
#اگر_ختم
    واپس fname;
}

    ساکن صحیح
b0_magic_wrong(ZERO_BL *b0p)
{
    واپس (b0p->b0_magic_long != (طویل)B0_MAGIC_LONG
	    || b0p->b0_magic_int != (صحیح)B0_MAGIC_INT
	    || b0p->b0_magic_short != (مختصر)B0_MAGIC_SHORT
	    || b0p->b0_magic_char != B0_MAGIC_CHAR);
}

#اگر_تعریف_شدہ CHECK_INODE
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

    ساکن صحیح
fnamecmp_ino(
    char_u	*fname_c,	    // current file name
    char_u	*fname_s,	    // file name from swap file
    طویل	ino_block0)
{
    stat_T	st;
    ino_t	ino_c = 0;	    // ino of current file
    ino_t	ino_s;		    // ino of file from swap file
    char_u	buf_c[MAXPATHL];    // full path of fname_c
    char_u	buf_s[MAXPATHL];    // full path of fname_s
    صحیح		retval_c;	    // flag: buf_c valid
    صحیح		retval_s;	    // flag: buf_s valid

    اگر (mch_stat((حرف *)fname_c, &st) == 0)
	ino_c = (ino_t)st.st_ino;

    /*
     * First we try to get the inode from the file name, because the inode in
     * the swap file may be outdated.  If that fails (e.g. this path is not
     * valid on this machine), use the inode from block 0.
     */
    اگر (mch_stat((حرف *)fname_s, &st) == 0)
	ino_s = (ino_t)st.st_ino;
    ورنہ
	ino_s = (ino_t)ino_block0;

    اگر (ino_c && ino_s)
	واپس (ino_c != ino_s);

    /*
     * One of the inode numbers is unknown, try a forced vim_FullName() and
     * compare the file names.
     */
    retval_c = vim_بھرا_Name(fname_c, buf_c, MAXPATHL, TRUE);
    retval_s = vim_بھرا_Name(fname_s, buf_s, MAXPATHL, TRUE);
    اگر (retval_c == OK && retval_s == OK)
	واپس STRCMP(buf_c, buf_s) != 0;

    /*
     * Can't compare inodes or file names, guess that the files are different,
     * unless both appear not to exist at all, then compare with the file name
     * in the swap file.
     */
    اگر (ino_s == 0 && ino_c == 0 && retval_c == FAIL && retval_s == FAIL)
	واپس STRCMP(fname_c, fname_s) != 0;
    واپس TRUE;
}
#اگر_ختم // CHECK_INODE

/*
 * Move a long integer into a four byte character array.
 * Used for machine independency in block zero.
 */
    ساکن خالی
long_مقصد_char(طویل n, char_u *s)
{
    s[0] = (char_u)(n & 0xff);
    n = (غیرعلامتی)n >> 8;
    s[1] = (char_u)(n & 0xff);
    n = (غیرعلامتی)n >> 8;
    s[2] = (char_u)(n & 0xff);
    n = (غیرعلامتی)n >> 8;
    s[3] = (char_u)(n & 0xff);
}

    ساکن طویل
char_مقصد_long(char_u *s)
{
    طویل    retval;

    retval = s[3];
    retval <<= 8;
    retval |= s[2];
    retval <<= 8;
    retval |= s[1];
    retval <<= 8;
    retval |= s[0];

    واپس retval;
}

/*
 * Set the flags in the first block of the swap file:
 * - file is modified or not: buf->b_changed
 * - 'fileformat'
 * - 'fileencoding'
 */
    خالی
ml_setflags(buf_T *buf)
{
    bhdr_T	*hp;
    ZERO_BL	*b0p;

    اگر (!buf->b_ml.ml_mfp)
	واپس;
    برائے (hp = buf->b_ml.ml_mfp->mf_مستعمل_آخری; hp != تہی; hp = hp->bh_پچھلا)
    {
	اگر (hp->bh_bnum == 0)
	{
	    b0p = (ZERO_BL *)(hp->bh_ڈیٹا);
	    b0p->b0_dirty = buf->b_بدلا ? B0_DIRTY : 0;
	    b0p->b0_flags = (b0p->b0_flags & ~B0_FF_نقاب)
						  | (get_fileformat(buf) + 1);
	    جوڑو_b0_fenc(b0p, buf);
	    hp->bh_flags |= BH_DIRTY;
	    mf_sync(buf->b_ml.ml_mfp, MFS_ZERO);
	    توڑو;
	}
    }
}

#اگر defined(FEAT_CRYPT)
/*
 * If "data" points to a data block encrypt the text in it and return a copy
 * in allocated memory.  Return NULL when out of memory.
 * Otherwise return "data".
 */
    char_u *
ml_encrypt_ڈیٹا(
    memfile_T	*mfp,
    char_u	*ڈیٹا,
    off_T	انحراف,
    غیرعلامتی	سائز)
{
    ڈیٹا_BL	*dp = (ڈیٹا_BL *)ڈیٹا;
    char_u	*سر_انتہا;
    char_u	*متن_آغاز;
    char_u	*new_ڈیٹا;
    صحیح		متن_len;
    cryptstate_T *state;

    اگر (dp->db_id != ڈیٹا_ID)
	واپس ڈیٹا;

    state = ml_crypt_prepare(mfp, انحراف, FALSE);
    اگر (state == تہی)
	واپس ڈیٹا;

    new_ڈیٹا = alloc(سائز);
    اگر (new_ڈیٹا == تہی)
    {
	crypt_free_state(state);
	واپس تہی;
    }
    سر_انتہا = (char_u *)(&dp->db_اشاریہ[dp->db_سطر_گنتی]);
    متن_آغاز = (char_u *)dp + dp->db_txt_آغاز;
    متن_len = سائز - dp->db_txt_آغاز;

    // Copy the header and the text.
    mch_memmove(new_ڈیٹا, dp, سر_انتہا - (char_u *)dp);

    // Encrypt the text.
    crypt_encode(state, متن_آغاز, متن_len, new_ڈیٹا + dp->db_txt_آغاز,
									FALSE);
    crypt_free_state(state);

    // Clear the gap.
    اگر (سر_انتہا < متن_آغاز)
	vim_memset(new_ڈیٹا + (سر_انتہا - ڈیٹا), 0, متن_آغاز - سر_انتہا);

    واپس new_ڈیٹا;
}

/*
 * Decrypt the text in "data" if it points to an encrypted data block.
 */
    خالی
ml_decrypt_ڈیٹا(
    memfile_T	*mfp,
    char_u	*ڈیٹا,
    off_T	انحراف,
    غیرعلامتی	سائز)
{
    ڈیٹا_BL	*dp = (ڈیٹا_BL *)ڈیٹا;
    char_u	*سر_انتہا;
    char_u	*متن_آغاز;
    صحیح		متن_len;
    cryptstate_T *state;

    اگر (dp->db_id != ڈیٹا_ID)
	واپس;

    سر_انتہا = (char_u *)(&dp->db_اشاریہ[dp->db_سطر_گنتی]);
    متن_آغاز = (char_u *)dp + dp->db_txt_آغاز;
    متن_len = dp->db_txt_انتہا - dp->db_txt_آغاز;

    اگر (سر_انتہا > متن_آغاز || dp->db_txt_آغاز > سائز
	    || dp->db_txt_انتہا > سائز)
	واپس;  // data was messed up

    state = ml_crypt_prepare(mfp, انحراف, TRUE);
    اگر (state == تہی)
	واپس;

    // Decrypt the text in place.
    crypt_decode_inplace(state, متن_آغاز, متن_len, FALSE);
    crypt_free_state(state);
}

/*
 * Prepare for encryption/decryption, using the key, seed and offset.
 * Return an allocated cryptstate_T *.
 * Note: Encryption not supported for SODIUM
 */
    ساکن cryptstate_T *
ml_crypt_prepare(memfile_T *mfp, off_T انحراف, صحیح reading)
{
    buf_T	*buf = mfp->mf_بفر;
    char_u	salt[50];
    صحیح		طریقہ_nr;
    char_u	*کلید;
    crypt_arg_T arg;

    CLEAR_میدان(arg);
    اگر (reading && mfp->mf_old_کلید != تہی)
    {
	// Reading back blocks with the previous key/method/seed.
	طریقہ_nr = mfp->mf_old_cm;
	کلید = mfp->mf_old_کلید;
	arg.cat_بیج = mfp->mf_old_بیج;
    }
    ورنہ
    {
	طریقہ_nr = crypt_get_طریقہ_nr(buf);
	کلید = buf->b_p_کلید;
	arg.cat_بیج = mfp->mf_بیج;
    }

    اگر (*کلید == NUL)
	واپس تہی;

    اگر (crypt_may_close_swapfile(buf, کلید, طریقہ_nr))
	واپس تہی;

    اگر (طریقہ_nr == CRYPT_M_ZIP)
    {
	// For PKzip: Append the offset to the key, so that we use a different
	// key for every block.
	vim_snprintf((حرف *)salt, حجم(salt), "%s%ld", کلید, (طویل)انحراف);
	arg.cat_بیج = تہی;
	arg.cat_init_from_file = FALSE;

	واپس crypt_create(طریقہ_nr, salt, &arg);
    }

    // Using blowfish or better: add salt and seed. We use the byte offset
    // of the block for the salt.
    vim_snprintf((حرف *)salt, حجم(salt), "%ld", (طویل)انحراف);

    arg.cat_salt = salt;
    arg.cat_salt_len = (صحیح)STRLEN(salt);
    arg.cat_بیج_len = MF_بیج_LEN;
    arg.cat_جوڑو_len = 0;
    arg.cat_جوڑو = تہی;
    arg.cat_init_from_file = FALSE;

    واپس crypt_create(طریقہ_nr, کلید, &arg);
}

#اگر_ختم


#اگر defined(FEAT_BYTEOFF)

# define MLCS_MAXL 800	// max no of lines in chunk
# define MLCS_MINL 400   // should be half of MLCS_MAXL

/*
 * Keep information for finding byte offset of a line, updtype may be one of:
 * ML_CHNK_ADDLINE: Add len to parent chunk, possibly splitting it
 *	   Careful: ML_CHNK_ADDLINE may cause ml_find_line() to be called.
 * ML_CHNK_DELLINE: Subtract len from parent chunk, possibly deleting it
 * ML_CHNK_UPDLINE: Add len to parent chunk, as a signed entity.
 */
    ساکن خالی
ml_updatechunk(
    buf_T	*buf,
    linenr_T	سطر,
    طویل	len,
    صحیح		updtype)
{
    ساکن buf_T	*ml_upd_lastbuf = تہی;
    ساکن linenr_T	ml_upd_lastcurline;
    ساکن صحیح		ml_upd_lastcurix;

    linenr_T		curline = ml_upd_lastcurline;
    صحیح			curix = ml_upd_lastcurix;
    طویل		سائز;
    chunksize_T		*curchnk;
    صحیح			rest;
    bhdr_T		*hp;
    ڈیٹا_BL		*dp;

    اگر (buf->b_ml.ml_usedchunks == -1 || len == 0)
	واپس;
    اگر (buf->b_ml.ml_chunksize == تہی)
    {
	buf->b_ml.ml_chunksize = ALLOC_MULT(chunksize_T, 100);
	اگر (buf->b_ml.ml_chunksize == تہی)
	{
	    buf->b_ml.ml_usedchunks = -1;
	    واپس;
	}
	buf->b_ml.ml_numchunks = 100;
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = 1;
	ml_upd_lastbuf = تہی;		// invalidate resume cache
    }

    اگر (updtype == ML_CHNK_UPDLINE && buf->b_ml.ml_سطر_گنتی == 1)
    {
	/*
	 * First line in empty buffer from ml_flush_line() -- reset
	 */
	buf->b_ml.ml_usedchunks = 1;
	buf->b_ml.ml_chunksize[0].mlcs_numlines = 1;
	buf->b_ml.ml_chunksize[0].mlcs_totalsize = (طویل)buf->b_ml.ml_سطر_len;
	ml_upd_lastbuf = تہی;		// invalidate resume cache
	واپس;
    }

    /*
     * Find chunk that our line belongs to, curline will be at start of the
     * chunk.
     * The scan resumes at the cached chunk while ml_upd_lastbuf is set: the
     * chunks have not moved since the last call.
     */
    اگر (buf != ml_upd_lastbuf || سطر < curline)
    {
	curline = 1;
	curix = 0;
    }
    برائے (; curix < buf->b_ml.ml_usedchunks - 1
	     && سطر >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	     curix++)
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
    curchnk = buf->b_ml.ml_chunksize + curix;

    اگر (updtype == ML_CHNK_DELLINE)
	len = -len;
    curchnk->mlcs_totalsize += len;
    اگر (updtype == ML_CHNK_ADDLINE)
    {
	curchnk->mlcs_numlines++;

	// May resize here so we don't have to do it in both cases below
	اگر (buf->b_ml.ml_usedchunks + 1 >= buf->b_ml.ml_numchunks)
	{
	    chunksize_T *t_chunksize = buf->b_ml.ml_chunksize;

	    buf->b_ml.ml_numchunks = buf->b_ml.ml_numchunks * 3 / 2;
	    buf->b_ml.ml_chunksize = vim_realloc(buf->b_ml.ml_chunksize,
			    حجم(chunksize_T) * buf->b_ml.ml_numchunks);
	    اگر (buf->b_ml.ml_chunksize == تہی)
	    {
		// Hmmmm, Give up on offset for this buffer
		vim_free(t_chunksize);
		buf->b_ml.ml_usedchunks = -1;
		واپس;
	    }
	}

	اگر (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MAXL)
	{
	    صحیح	    گنتی;	    // number of entries in block
	    صحیح	    idx;
	    صحیح	    انتہا_idx;
	    صحیح	    متن_انتہا;
	    صحیح	    linecnt;

	    mch_memmove(buf->b_ml.ml_chunksize + curix + 1,
			buf->b_ml.ml_chunksize + curix,
			(buf->b_ml.ml_usedchunks - curix) *
			حجم(chunksize_T));
	    // Compute length of first half of lines in the split chunk
	    سائز = 0;
	    linecnt = 0;
	    جب (curline < buf->b_ml.ml_سطر_گنتی
			&& linecnt < MLCS_MINL)
	    {
		اگر ((hp = ml_ڈھونڈو_سطر(buf, curline, ML_ڈھونڈو)) == تہی)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    واپس;
		}
		dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
		گنتی = (طویل)(buf->b_ml.ml_locked_بلند) -
			(طویل)(buf->b_ml.ml_locked_کم) + 1;
		idx = curline - buf->b_ml.ml_locked_کم;
		curline = buf->b_ml.ml_locked_بلند + 1;

		// compute index of last line to use in this MEMLINE
		rest = گنتی - idx;
		اگر (linecnt + rest > MLCS_MINL)
		{
		    انتہا_idx = idx + MLCS_MINL - linecnt - 1;
		    linecnt = MLCS_MINL;
		}
		ورنہ
		{
		    انتہا_idx = گنتی - 1;
		    linecnt += rest;
		}
# ifdef FEAT_PROP_POPUP
		اگر (buf->b_has_textprop)
		{
		    صحیح i;

		    // We cannot use the text pointers to get the text length,
		    // the text prop info would also be counted.  Go over the
		    // lines.
		    برائے (i = انتہا_idx; i < idx; ++i)
			سائز += (صحیح)STRLEN((char_u *)dp
				      + (dp->db_اشاریہ[i] & DB_اشاریہ_نقاب)) + 1;
		}
		ورنہ
# endif
		{
		    اگر (idx == 0) // first line in block, text at the end
			متن_انتہا = dp->db_txt_انتہا;
		    ورنہ
			متن_انتہا = ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب);
		    سائز += متن_انتہا
				   - ((dp->db_اشاریہ[انتہا_idx]) & DB_اشاریہ_نقاب);
		}
	    }
	    buf->b_ml.ml_chunksize[curix].mlcs_numlines = linecnt;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_numlines -= linecnt;
	    buf->b_ml.ml_chunksize[curix].mlcs_totalsize = سائز;
	    buf->b_ml.ml_chunksize[curix + 1].mlcs_totalsize -= سائز;
	    buf->b_ml.ml_usedchunks++;
	    ml_upd_lastbuf = تہی;   // Force recalc of curix & curline
	    واپس;
	}
	ورنہ اگر (buf->b_ml.ml_chunksize[curix].mlcs_numlines >= MLCS_MINL
		     && curix == buf->b_ml.ml_usedchunks - 1
		     && buf->b_ml.ml_سطر_گنتی - سطر <= 1)
	{
	    /*
	     * We are in the last chunk and it is cheap to create a new one
	     * after this. Do it now to avoid the loop above later on
	     */
	    curchnk = buf->b_ml.ml_chunksize + curix + 1;
	    buf->b_ml.ml_usedchunks++;
	    اگر (سطر == buf->b_ml.ml_سطر_گنتی)
	    {
		curchnk->mlcs_numlines = 0;
		curchnk->mlcs_totalsize = 0;
	    }
	    ورنہ
	    {
		/*
		 * Line is just prior to last, move count for last
		 * This is the common case  when loading a new file
		 */
		hp = ml_ڈھونڈو_سطر(buf, buf->b_ml.ml_سطر_گنتی, ML_ڈھونڈو);
		اگر (hp == تہی)
		{
		    buf->b_ml.ml_usedchunks = -1;
		    واپس;
		}
		dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
		اگر (dp->db_سطر_گنتی == 1)
		    rest = dp->db_txt_انتہا - dp->db_txt_آغاز;
		ورنہ
		    rest =
			((dp->db_اشاریہ[dp->db_سطر_گنتی - 2]) & DB_اشاریہ_نقاب)
			- dp->db_txt_آغاز;
		curchnk->mlcs_totalsize = rest;
		curchnk->mlcs_numlines = 1;
		curchnk[-1].mlcs_totalsize -= rest;
		curchnk[-1].mlcs_numlines -= 1;
	    }
	}
    }
    ورنہ اگر (updtype == ML_CHNK_DELLINE)
    {
	curchnk->mlcs_numlines--;
	اگر (curix < buf->b_ml.ml_usedchunks - 1
		&& curchnk->mlcs_numlines + curchnk[1].mlcs_numlines
								  <= MLCS_MINL)
	{
	    curix++;
	    curchnk = buf->b_ml.ml_chunksize + curix;
	}
	ورنہ اگر (curix == 0 && curchnk->mlcs_numlines <= 0)
	{
	    ml_upd_lastbuf = تہی;   // Force recalc of curix & curline
	    buf->b_ml.ml_usedchunks--;
	    mch_memmove(buf->b_ml.ml_chunksize, buf->b_ml.ml_chunksize + 1,
			buf->b_ml.ml_usedchunks * حجم(chunksize_T));
	    واپس;
	}
	ورنہ اگر (curix == 0 || (curchnk->mlcs_numlines > 10
		    && curchnk->mlcs_numlines + curchnk[-1].mlcs_numlines
								  > MLCS_MINL))
	{
	    // The chunks are left as they are, the cached position stays
	    // valid.
	    ml_upd_lastbuf = buf;
	    ml_upd_lastcurline = curline;
	    ml_upd_lastcurix = curix;
	    واپس;
	}

	// Collapse chunks
	ml_upd_lastbuf = تہی;   // Force recalc of curix & curline
	curchnk[-1].mlcs_numlines += curchnk->mlcs_numlines;
	curchnk[-1].mlcs_totalsize += curchnk->mlcs_totalsize;
	buf->b_ml.ml_usedchunks--;
	اگر (curix < buf->b_ml.ml_usedchunks)
	    mch_memmove(buf->b_ml.ml_chunksize + curix,
			buf->b_ml.ml_chunksize + curix + 1,
			(buf->b_ml.ml_usedchunks - curix) *
			حجم(chunksize_T));
	واپس;
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
    طویل
ml_ڈھونڈو_سطر_or_انحراف(buf_T *buf, linenr_T lnum, طویل *offp)
{
    linenr_T	curline;
    صحیح		curix;
    طویل	سائز;
    bhdr_T	*hp;
    ڈیٹا_BL	*dp;
    صحیح		گنتی;		// number of entries in block
    صحیح		idx;
    صحیح		آغاز_idx;
    صحیح		متن_انتہا;
    طویل	انحراف;
    صحیح		len;
    صحیح		ffdos = (get_fileformat(buf) == EOL_DOS);
    صحیح		extra = 0;

    // take care of cached line first
    ml_flush_سطر(curbuf);

    اگر (buf->b_ml.ml_usedchunks == -1
	    || buf->b_ml.ml_chunksize == تہی
	    || lnum < 0)
	واپس -1;

    اگر (offp == تہی)
	انحراف = 0;
    ورنہ
	انحراف = *offp;
    اگر (lnum == 0 && انحراف <= 0)
	واپس 1;   // Not a "find offset" and offset 0 _must_ be in line 1
    /*
     * Find the last chunk before the one containing our line. Last chunk is
     * special because it will never qualify.
     */
    curline = 1;
    curix = سائز = 0;
    جب (curix < buf->b_ml.ml_usedchunks - 1
	    && ((lnum != 0
	     && lnum >= curline + buf->b_ml.ml_chunksize[curix].mlcs_numlines)
		|| (انحراف != 0
	       && انحراف > سائز + buf->b_ml.ml_chunksize[curix].mlcs_totalsize
		 + (طویل)ffdos * buf->b_ml.ml_chunksize[curix].mlcs_numlines)))
    {
	curline += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	سائز += buf->b_ml.ml_chunksize[curix].mlcs_totalsize;
	اگر (انحراف && ffdos)
	    سائز += buf->b_ml.ml_chunksize[curix].mlcs_numlines;
	curix++;
    }

    جب ((lnum != 0 && curline < lnum) || (انحراف != 0 && سائز < انحراف))
    {
# ifdef FEAT_PROP_POPUP
	size_t textprop_کل = 0;
# endif

	اگر (curline > buf->b_ml.ml_سطر_گنتی
		|| (hp = ml_ڈھونڈو_سطر(buf, curline, ML_ڈھونڈو)) == تہی)
	    واپس -1;
	dp = (ڈیٹا_BL *)(hp->bh_ڈیٹا);
	گنتی = (طویل)(buf->b_ml.ml_locked_بلند) -
		(طویل)(buf->b_ml.ml_locked_کم) + 1;
	آغاز_idx = idx = curline - buf->b_ml.ml_locked_کم;
	اگر (idx == 0)  // first line in block, text at the end
	    متن_انتہا = dp->db_txt_انتہا;
	ورنہ
	    متن_انتہا = ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب);
	// Compute index of last line to use in this MEMLINE
	اگر (lnum != 0)
	{
	    اگر (curline + (گنتی - idx) >= lnum)
		idx += lnum - curline - 1;
	    ورنہ
		idx = گنتی - 1;
	}
	ورنہ
	{
	    extra = 0;
	    برائے (;;)
	    {
# ifdef FEAT_PROP_POPUP
		size_t textprop_سائز = 0;

		اگر (buf->b_has_textprop)
		{
		    char_u *l1, *l2;

		    // compensate for the extra bytes taken by textprops
		    l1 = (char_u *)dp + ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب);
		    l2 = (char_u *)dp + (idx == 0 ? dp->db_txt_انتہا
				  : ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب));
		    textprop_سائز = (l2 - l1) - (STRLEN(l1) + 1);
		}
# endif
		اگر (!(انحراف >= سائز
			+ متن_انتہا - (صحیح)((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب)
# ifdef FEAT_PROP_POPUP
			- (طویل)(textprop_کل + textprop_سائز)
# endif
			+ ffdos))
		    توڑو;

		اگر (ffdos)
		    سائز++;
# ifdef FEAT_PROP_POPUP
		textprop_کل += textprop_سائز;
# endif
		اگر (idx == گنتی - 1)
		{
		    extra = 1;
		    توڑو;
		}
		idx++;
	    }
	}
# ifdef FEAT_PROP_POPUP
	اگر (buf->b_has_textprop && lnum != 0)
	{
	    صحیح i;

	    // cannot use the db_index pointer, need to get the actual text
	    // lengths.
	    len = 0;
	    برائے (i = آغاز_idx; i <= idx; ++i)
	    {
		char_u *p = (char_u *)dp + ((dp->db_اشاریہ[i]) & DB_اشاریہ_نقاب);
		len += (صحیح)STRLEN(p) + 1;
	    }
	}
	ورنہ
# endif
	    len = متن_انتہا - ((dp->db_اشاریہ[idx]) & DB_اشاریہ_نقاب)
# ifdef FEAT_PROP_POPUP
				- (طویل)textprop_کل
# endif
				;
	سائز += len;
	اگر (انحراف != 0 && سائز >= انحراف)
	{
	    اگر (سائز + ffdos == انحراف)
		*offp = 0;
	    ورنہ اگر (idx == آغاز_idx)
		*offp = انحراف - سائز + len;
	    ورنہ
		*offp = انحراف - سائز + len
		     - (متن_انتہا - ((dp->db_اشاریہ[idx - 1]) & DB_اشاریہ_نقاب))
# ifdef FEAT_PROP_POPUP
		     + (طویل)textprop_کل
# endif
		     ;
	    curline += idx - آغاز_idx + extra;
	    اگر (curline > buf->b_ml.ml_سطر_گنتی)
		واپس -1;	// exactly one byte beyond the end
	    واپس curline;
	}
	curline = buf->b_ml.ml_locked_بلند + 1;
    }

    اگر (lnum != 0)
    {
	// Count extra CR characters.
	اگر (ffdos)
	    سائز += lnum - 1;

	// Don't count the last line break if 'noeol' and ('bin' or
	// 'nofixeol').
	اگر ((!buf->b_p_fixeol || buf->b_p_bin) && !buf->b_p_eol
					   && lnum > buf->b_ml.ml_سطر_گنتی)
	    سائز -= ffdos + 1;
    }

    واپس سائز;
}

/*
 * Goto byte in buffer with offset 'cnt'.
 */
    خالی
goto_byte(طویل cnt)
{
    طویل	boff = cnt;
    linenr_T	lnum;

    ml_flush_سطر(curbuf);	// cached line may be dirty
    setpcmark();
    اگر (boff)
	--boff;
    lnum = ml_ڈھونڈو_سطر_or_انحراف(curbuf, (linenr_T)0, &boff);
    اگر (lnum < 1)	// past the end
    {
	curwin->w_cursor.lnum = curbuf->b_ml.ml_سطر_گنتی;
	curwin->w_curswant = MAXCOL;
	coladvance((colnr_T)MAXCOL);
    }
    ورنہ
    {
	curwin->w_cursor.lnum = lnum;
	curwin->w_cursor.ستون = (colnr_T)boff;
	curwin->w_cursor.coladd = 0;
	curwin->w_سیٹ_curswant = true;
    }
    check_cursor();

    // Make sure the cursor is on the first byte of a multi-byte char.
    اگر (has_mbyte)
	mb_adjust_cursor();
}
#اگر_ختم
