/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * undo.c: multi level undo facility
 *
 * The saved lines are stored in a list of lists (one for each buffer):
 *
 * b_u_oldhead------------------------------------------------+
 *							      |
 *							      V
 *		  +--------------+    +--------------+	  +--------------+
 * b_u_newhead--->| u_header	 |    | u_header     |	  | u_header	 |
 *		  |	uh_next------>|     uh_next------>|	uh_next---->NULL
 *	   NULL<--------uh_prev  |<---------uh_prev  |<---------uh_prev  |
 *		  |	uh_entry |    |     uh_entry |	  |	uh_entry |
 *		  +--------|-----+    +--------|-----+	  +--------|-----+
 *			   |		       |		   |
 *			   V		       V		   V
 *		  +--------------+    +--------------+	  +--------------+
 *		  | u_entry	 |    | u_entry      |	  | u_entry	 |
 *		  |	ue_next  |    |     ue_next  |	  |	ue_next  |
 *		  +--------|-----+    +--------|-----+	  +--------|-----+
 *			   |		       |		   |
 *			   V		       V		   V
 *		  +--------------+	      NULL		  NULL
 *		  | u_entry	 |
 *		  |	ue_next  |
 *		  +--------|-----+
 *			   |
 *			   V
 *			  etc.
 *
 * Each u_entry list contains the information for one undo or redo.
 * curbuf->b_u_curhead points to the header of the last undo (the next redo),
 * or is NULL if nothing has been undone (end of the branch).
 *
 * For keeping alternate undo/redo branches the uh_alt field is used.  Thus at
 * each point in the list a branch may appear for an alternate to redo.  The
 * uh_seq field is numbered sequentially to be able to find a newer or older
 * branch.
 *
 *		   +---------------+	+---------------+
 * b_u_oldhead --->| u_header	   |	| u_header	|
 *		   |   uh_alt_next ---->|   uh_alt_next ----> NULL
 *	   NULL <----- uh_alt_prev |<------ uh_alt_prev |
 *		   |   uh_prev	   |	|   uh_prev	|
 *		   +-----|---------+	+-----|---------+
 *			 |		      |
 *			 V		      V
 *		   +---------------+	+---------------+
 *		   | u_header	   |	| u_header	|
 *		   |   uh_alt_next |	|   uh_alt_next |
 * b_u_newhead --->|   uh_alt_prev |	|   uh_alt_prev |
 *		   |   uh_prev	   |	|   uh_prev	|
 *		   +-----|---------+	+-----|---------+
 *			 |		      |
 *			 V		      V
 *		       NULL		+---------------+    +---------------+
 *					| u_header	|    | u_header      |
 *					|   uh_alt_next ---->|	 uh_alt_next |
 *					|   uh_alt_prev |<------ uh_alt_prev |
 *					|   uh_prev	|    |	 uh_prev     |
 *					+-----|---------+    +-----|---------+
 *					      |			   |
 *					     etc.		  etc.
 *
 *
 * All data is allocated and will all be freed when the buffer is unloaded.
 */

// Uncomment the next line for including the u_check() function.  This warns
// for errors in the debug information.
// #define U_DEBUG 1
#تعریف_کرو UH_MAGIC 0x18dade	// value for uh_magic when in use
#تعریف_کرو UE_MAGIC 0xabc123	// value for ue_magic when in use

// Size of buffer used for encryption.
#تعریف_کرو CRYPT_BUF_ناپ 8192

#ضمن "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
تعريفنوع بنية {
    buf_T	*bi_buf;
    FILE	*bi_fp;
#جے_تعریف_شدہ FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_بفر; // CRYPT_BUF_SIZE, NULL when not buffering
    size_t	bi_ورتیا;    // bytes written to/read from bi_buffer
    size_t	bi_avail;   // bytes available in bi_buffer
#جے_انت
} bufinfo_T;


ساكن فراغ u_unch_branch(u_سرخی_T *uhp);
ساكن u_اندراج_T *u_get_headentry(فراغ);
ساكن فراغ u_getbot(فراغ);
ساكن فراغ u_doit(صحيح گنتی);
ساكن فراغ u_undoredo(صحيح undo);
ساكن فراغ u_undo_سرا(صحيح did_undo, صحيح absolute);
ساكن فراغ u_freeheader(buf_T *buf, u_سرخی_T *uhp, u_سرخی_T **uhpp);
ساكن فراغ u_freebranch(buf_T *buf, u_سرخی_T *uhp, u_سرخی_T **uhpp);
ساكن فراغ u_freeentries(buf_T *buf, u_سرخی_T *uhp, u_سرخی_T **uhpp);
ساكن فراغ u_freeentry(u_اندراج_T *, لما);
#جے_تعریف_شدہ FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
ساكن صحيح undo_flush(bufinfo_T *bi);
# endif
ساكن صحيح undo_read(bufinfo_T *bi, char_u *بفر, size_t ناپ);
ساكن صحيح serialize_uep(bufinfo_T *bi, u_اندراج_T *uep);
ساكن u_اندراج_T *unserialize_uep(bufinfo_T *bi, صحيح *error, char_u *file_name);
ساكن فراغ serialize_pos(bufinfo_T *bi, pos_T pos);
ساكن فراغ unserialize_pos(bufinfo_T *bi, pos_T *pos);
ساكن فراغ serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
ساكن فراغ unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#جے_انت
ساكن فراغ u_saveline(linenr_T lnum);
ساكن فراغ u_blockfree(buf_T *buf);

#تعریف_کرو U_ALLOC_سطر(ناپ) lalloc(ناپ, FALSE)

// used in undo_end() to report number of added and deleted lines
ساكن لما	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
ساكن صحيح	undo_undoes = FALSE;

ساكن صحيح	lastmark = 0;

#إذا defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
ساكن صحيح تکیا_b_u_curhead;
ساكن صحيح تکیا_b_u_newhead;
ساكن صحيح سرخی_گنتی;

    ساكن فراغ
u_check_درخت(u_سرخی_T *uhp,
	u_سرخی_T *exp_uh_اگلا,
	u_سرخی_T *exp_uh_alt_prev)
{
    u_اندراج_T *uep;

    إذا (uhp == عدم)
	أرجع;
    ++سرخی_گنتی;
    إذا (uhp == curbuf->b_u_curhead && ++تکیا_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	أرجع;
    }
    إذا (uhp == curbuf->b_u_newhead && ++تکیا_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	أرجع;
    }

    إذا (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    وإلا
    {
	// Check pointers back are correct.
	إذا (uhp->uh_اگلا.ptr != exp_uh_اگلا)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_اگلا, uhp->uh_اگلا.ptr);
	}
	إذا (uhp->uh_alt_prev.ptr != exp_uh_alt_prev)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_prev, uhp->uh_alt_prev.ptr);
	}

	// Check the undo tree at this header.
	لكل (uep = uhp->uh_اندراج; uep != عدم; uep = uep->ue_اگلا)
	{
	    إذا (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		اكسر;
	    }
	}

	// Check the next alt tree.
	u_check_درخت(uhp->uh_alt_اگلا.ptr, uhp->uh_اگلا.ptr, uhp);

	// Check the next header in this branch.
	u_check_درخت(uhp->uh_prev.ptr, uhp, عدم);
    }
}

    ساكن فراغ
u_check(صحيح newhead_may_be_NULL)
{
    تکیا_b_u_newhead = 0;
    تکیا_b_u_curhead = 0;
    سرخی_گنتی = 0;

    u_check_درخت(curbuf->b_u_oldhead, عدم, عدم);

    إذا (تکیا_b_u_newhead == 0 && curbuf->b_u_oldhead != عدم
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == عدم))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    إذا (curbuf->b_u_curhead != عدم && تکیا_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    إذا (سرخی_گنتی != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (لما)سرخی_گنتی, (لما)curbuf->b_u_numhead);
    }
}
#جے_انت

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    صحيح
u_save_cursor(فراغ)
{
    أرجع (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    صحيح
u_save(linenr_T چوٹی, linenr_T bot)
{
    إذا (undo_off)
	أرجع OK;

    إذا (چوٹی >= bot || bot > curbuf->b_ml.ml_سطر_گنتی + 1)
	أرجع FAIL;	// rely on caller to give an error message

    إذا (چوٹی + 2 == bot)
	u_saveline((linenr_T)(چوٹی + 1));

    أرجع (u_savecommon(چوٹی, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    صحيح
u_savesub(linenr_T lnum)
{
    إذا (undo_off)
	أرجع OK;

    أرجع (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    صحيح
u_inssub(linenr_T lnum)
{
    إذا (undo_off)
	أرجع OK;

    أرجع (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    صحيح
u_savedel(linenr_T lnum, لما nlines)
{
    إذا (undo_off)
	أرجع OK;

    أرجع (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_سطر_گنتی ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    صحيح
undo_allowed(فراغ)
{
    // Don't allow changes when 'modifiable' is off.
    إذا (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	أرجع FALSE;
    }

#جے_تعریف_شدہ HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    إذا (sandbox != 0)
    {
	emsg(_(e_not_allowed_وچ_sandbox));
	أرجع FALSE;
    }
#جے_انت

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    إذا (textlock != 0)
    {
	emsg(_(e_not_allowed_منزل_change_متن_or_change_کھڑکی));
	أرجع FALSE;
    }

    أرجع TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    ساكن لما
get_undolevel(فراغ)
{
    إذا (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	أرجع p_ul;
    أرجع curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    ساكن صحيح
u_save_سطر(undoline_T *ul, linenr_T lnum)
{
    char_u *سطر = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    إذا (curbuf->b_ml.ml_سطر_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_سطر = vim_strsave((char_u *)"");
    }
    وإلا
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_سطر_len;
	ul->ul_سطر = vim_memsave(سطر, ul->ul_len);
    }
    أرجع ul->ul_سطر == عدم ? FAIL : OK;
}

#جے_تعریف_شدہ FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    ساكن صحيح
has_prop_w_flags(linenr_T lnum, صحيح flags)
{
    char_u  *props;
    صحيح	    i;
    صحيح	    proplen = get_متن_props(curbuf, lnum, &props, FALSE);

    لكل (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * حجم prop, حجم prop);
	إذا (prop.tp_flags & flags)
	    أرجع TRUE;
    }
    أرجع FALSE;
}
#جے_انت

/*
 * Common code for various ways to save text before a change.
 * "top" is the line above the first changed line.
 * "bot" is the line below the last changed line.
 * "newbot" is the new bottom line.  Use zero when not known.
 * "reload" is TRUE when saving for a buffer reload.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    صحيح
u_savecommon(
    linenr_T	چوٹی,
    linenr_T	bot,
    linenr_T	newbot,
    صحيح		reload)
{
    linenr_T	lnum;
    لما	i;
    u_سرخی_T	*uhp;
    u_سرخی_T	*old_curhead;
    u_اندراج_T	*uep;
    u_اندراج_T	*prev_uep;
    لما	ناپ;

    إذا (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	إذا (!undo_allowed())
	    أرجع FAIL;

#جے_تعریف_شدہ FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	إذا (netbeans_سرگرم())
	{
	    إذا (netbeans_is_guarded(چوٹی, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		أرجع FAIL;
	    }
	    إذا (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_وچ_read_only_files));
		أرجع FAIL;
	    }
	}
#جے_انت
#جے_تعریف_شدہ FEAT_TERMINAL
	// A change in a terminal buffer removes the highlighting.
	رکن_change_وچ_curbuf();
#جے_انت

	/*
	 * Saving text for undo means we are going to make a change.  Give a
	 * warning for a read-only file before making the change, so that the
	 * FileChangedRO event can replace the buffer with a read-write version
	 * (e.g., obtained from a source control system).
	 */
	change_warning(0);
	إذا (bot > curbuf->b_ml.ml_سطر_گنتی + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_سطر_گنتی_بدلیا_unexpectedly));
	    أرجع FAIL;
	}
    }

#جے_تعریف_شدہ U_DEBUG
    u_check(FALSE);
#جے_انت

#جے_تعریف_شدہ FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    إذا (bot - چوٹی > 1)
    {
	إذا (چوٹی > 0 && has_prop_w_flags(چوٹی + 1, TP_FLAG_CONT_PREV))
	    --چوٹی;
	إذا (bot <= curbuf->b_ml.ml_سطر_گنتی
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_اگلا))
	{
	    ++bot;
	    إذا (newbot != 0)
		++newbot;
	}
    }
#جے_انت

    ناپ = bot - چوٹی - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    إذا (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	إذا (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_سطر(حجم(u_سرخی_T));
	    إذا (uhp == عدم)
		اذهب nomem;
#جے_تعریف_شدہ U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#جے_انت
	}
	وإلا
	    uhp = عدم;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	إذا (old_curhead != عدم)
	{
	    curbuf->b_u_newhead = old_curhead->uh_اگلا.ptr;
	    curbuf->b_u_curhead = عدم;
	}

	/*
	 * free headers to keep the size right
	 */
	طالما (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != عدم)
	{
	    u_سرخی_T	    *uhfree = curbuf->b_u_oldhead;

	    إذا (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    وإلا إذا (uhfree->uh_alt_اگلا.ptr == عدم)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    وإلا
	    {
		// Free the oldest alternate branch as a whole.
		طالما (uhfree->uh_alt_اگلا.ptr != عدم)
		    uhfree = uhfree->uh_alt_اگلا.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#جے_تعریف_شدہ U_DEBUG
	    u_check(TRUE);
#جے_انت
	}

	إذا (uhp == عدم)		// no undo at all
	{
	    إذا (old_curhead != عدم)
		u_freebranch(curbuf, old_curhead, عدم);
	    curbuf->b_u_synced = false;
	    أرجع OK;
	}

	uhp->uh_prev.ptr = عدم;
	uhp->uh_اگلا.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_اگلا.ptr = old_curhead;
	إذا (old_curhead != عدم)
	{
	    uhp->uh_alt_prev.ptr = old_curhead->uh_alt_prev.ptr;
	    إذا (uhp->uh_alt_prev.ptr != عدم)
		uhp->uh_alt_prev.ptr->uh_alt_اگلا.ptr = uhp;
	    old_curhead->uh_alt_prev.ptr = uhp;
	    إذا (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	وإلا
	    uhp->uh_alt_prev.ptr = عدم;
	إذا (curbuf->b_u_newhead != عدم)
	    curbuf->b_u_newhead->uh_prev.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_آخری;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_ویلا = vim_ویلا();
	uhp->uh_save_nr = 0;
	curbuf->b_u_ویلا_cur = uhp->uh_ویلا + 1;

	uhp->uh_walk = 0;
	uhp->uh_اندراج = عدم;
	uhp->uh_getbot_اندراج = عدم;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	إذا (virtual_سرگرم() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	وإلا
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_بدلیا ? UH_بدلیا : 0) +
		       ((curbuf->b_ml.ml_flags & ML_خالی_تھاواں) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, حجم(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	إذا (curbuf->b_u_oldhead == عدم)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    وإلا
    {
	إذا (get_undolevel() < 0)	// no undo at all
	    أرجع OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	إذا (ناپ == 1)
	{
	    uep = u_get_headentry();
	    prev_uep = عدم;
	    لكل (i = 0; i < 10; ++i)
	    {
		إذا (uep == عدم)
		    اكسر;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		إذا ((curbuf->b_u_newhead->uh_getbot_اندراج != uep
			    ? (uep->ue_چوٹی + uep->ue_ناپ + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_سطر_گنتی + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_سطر_گنتی)
			|| (uep->ue_ناپ > 1
			    && چوٹی >= uep->ue_چوٹی
			    && چوٹی + 2 <= uep->ue_چوٹی + uep->ue_ناپ + 1))
		    اكسر;

		// If it's the same line we can skip saving it again.
		إذا (uep->ue_ناپ == 1 && uep->ue_چوٹی == چوٹی)
		{
		    إذا (i > 0)
		    {
			// It's not the last entry: get ue_bot for the last
			// entry now.  Following deleted/inserted lines go to
			// the re-used entry.
			u_getbot();
			curbuf->b_u_synced = false;

			// Move the found entry to become the last entry.  The
			// order of undo/redo doesn't matter for the entries
			// we move it over, since they don't change the line
			// count and don't include this line.  It does matter
			// for the found entry if the line count is changed by
			// the executed command.
			prev_uep->ue_اگلا = uep->ue_اگلا;
			uep->ue_اگلا = curbuf->b_u_newhead->uh_اندراج;
			curbuf->b_u_newhead->uh_اندراج = uep;
		    }

		    // The executed command may change the line count.
		    إذا (newbot != 0)
			uep->ue_bot = newbot;
		    وإلا إذا (bot > curbuf->b_ml.ml_سطر_گنتی)
			uep->ue_bot = 0;
		    وإلا
		    {
			uep->ue_lcount = curbuf->b_ml.ml_سطر_گنتی;
			curbuf->b_u_newhead->uh_getbot_اندراج = uep;
		    }
		    أرجع OK;
		}
		prev_uep = uep;
		uep = uep->ue_اگلا;
	    }
	}

	// find line number for ue_bot for previous u_save()
	u_getbot();
    }

    /*
     * add lines in front of entry list
     */
    uep = U_ALLOC_سطر(حجم(u_اندراج_T));
    إذا (uep == عدم)
	اذهب nomem;
    CLEAR_اشارہ_گر(uep);
#جے_تعریف_شدہ U_DEBUG
    uep->ue_magic = UE_MAGIC;
#جے_انت

    uep->ue_ناپ = ناپ;
    uep->ue_چوٹی = چوٹی;
    إذا (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    وإلا إذا (bot > curbuf->b_ml.ml_سطر_گنتی)
	uep->ue_bot = 0;
    وإلا
    {
	uep->ue_lcount = curbuf->b_ml.ml_سطر_گنتی;
	curbuf->b_u_newhead->uh_getbot_اندراج = uep;
    }

    إذا (ناپ > 0)
    {
	إذا ((uep->ue_array = U_ALLOC_سطر(حجم(undoline_T) * ناپ)) == عدم)
	{
	    u_freeentry(uep, 0L);
	    اذهب nomem;
	}
	لكل (i = 0, lnum = چوٹی + 1; i < ناپ; ++i)
	{
	    fast_breakcheck();
	    إذا (got_int)
	    {
		u_freeentry(uep, i);
		أرجع FAIL;
	    }
	    إذا (u_save_سطر(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		اذهب nomem;
	    }
	}
    }
    وإلا
	uep->ue_array = عدم;
    uep->ue_اگلا = curbuf->b_u_newhead->uh_اندراج;
    curbuf->b_u_newhead->uh_اندراج = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#جے_تعریف_شدہ U_DEBUG
    u_check(FALSE);
#جے_انت
    أرجع OK;

nomem:
    پیغام_silent = 0;	// must display the prompt
    إذا (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	أرجع OK;
    }
    do_outofmem_پیغام((long_u)0);
    أرجع FAIL;
}

#إذا defined(FEAT_PERSISTENT_UNDO)

# define UF_شروع_MAGIC	    "Vim\237UnDo\345"  // magic at start of undofile
# define UF_شروع_MAGIC_LEN	9
# define UF_سرخی_MAGIC	0x5fd0	// magic at start of header
# define UF_سرخی_سرا_MAGIC	0xe7aa	// magic after last header
# define UF_اندراج_MAGIC		0xf518	// magic at start of entry
# define UF_اندراج_سرا_MAGIC	0x3581	// magic after last entry
# define UF_نسخہ		2	// 2-byte undofile version number
# define UF_نسخہ_CRYPT	0x8002	// idem, encrypted

// extra fields for header
# define UF_آخری_SAVE_NR	1

// extra fields for uhp
# define UHP_SAVE_NR		1

/*
 * Compute the hash for the current buffer text into hash[UNDO_HASH_SIZE].
 */
    فراغ
u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;

    sha256_شروع(&ctx);
    لكل (lnum = 1; lnum <= curbuf->b_ml.ml_سطر_گنتی; ++lnum)
	sha256_update(&ctx, ml_get(lnum), (UINT32_T)(ml_get_len(lnum) + 1));
    sha256_انت(&ctx, hash);
}

/*
 * Return an allocated string of the full path of the target undofile.
 * When "reading" is TRUE find the file to read, go over all directories in
 * 'undodir'.
 * When "reading" is FALSE use the first name where the directory exists.
 * Returns NULL when there is no place to write or no file to read.
 */
    ساكن char_u *
u_get_undo_file_name(char_u *buf_ffname, صحيح reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {عدم, 0};
    char_u	*undo_file_name = عدم;
    صحيح		dir_len;
    char_u	*p;
    size_t	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    size_t	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    إذا (ffname == عدم)
	أرجع عدم;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    إذا (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    طالما (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	إذا (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    إذا (undo_file_name == عدم)
		اكسر;
	    p = gettail(undo_file_name);
	    plen = (size_t)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# وإلا
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	وإلا
	{
	    dir_name[dir_len] = NUL;
	    إذا (mch_isdir(dir_name))
	    {
		string_T    ret;

		إذا (munged_name.string == عدم)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    إذا (munged_name.string == عدم)
			أرجع عدم;
		    munged_name.لمائی = ffnamelen;
		    لكل (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			إذا (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (size_t)dir_len,
		    munged_name.string, munged_name.لمائی, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	إذا (undo_file_name != عدم && (!reading
			       || mch_stat((حرف *)undo_file_name, &st) >= 0))
	    اكسر;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    أرجع undo_file_name;
}

    ساكن فراغ
corruption_error(حرف *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    ساكن فراغ
u_free_uhp(u_سرخی_T *uhp)
{
    u_اندراج_T	*nuep;
    u_اندراج_T	*uep;

    uep = uhp->uh_اندراج;
    طالما (uep != عدم)
    {
	nuep = uep->ue_اگلا;
	u_freeentry(uep, uep->ue_ناپ);
	uep = nuep;
    }
    vim_free(uhp);
}

/*
 * Write a sequence of bytes to the undo file.
 * Buffers and encrypts as needed.
 * Returns OK or FAIL.
 */
    ساكن صحيح
undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    إذا (bi->bi_بفر != عدم)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	طالما (bi->bi_ورتیا + len_todo >= CRYPT_BUF_ناپ)
	{
	    size_t	n = CRYPT_BUF_ناپ - bi->bi_ورتیا;

	    mch_memmove(bi->bi_بفر + bi->bi_ورتیا, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_ورتیا = CRYPT_BUF_ناپ;
	    إذا (undo_flush(bi) == FAIL)
		أرجع FAIL;
	}
	إذا (len_todo > 0)
	{
	    mch_memmove(bi->bi_بفر + bi->bi_ورتیا, p, len_todo);
	    bi->bi_ورتیا += len_todo;
	}
	أرجع OK;
    }
# endif
    إذا (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	أرجع FAIL;
    أرجع OK;
}

# ifdef FEAT_CRYPT
    ساكن صحيح
undo_flush(bufinfo_T *bi)
{
    إذا (bi->bi_بفر != عدم && bi->bi_state != عدم && bi->bi_ورتیا > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_بفر, bi->bi_ورتیا, FALSE);
	إذا (fwrite(bi->bi_بفر, bi->bi_ورتیا, (size_t)1, bi->bi_fp) != 1)
	    أرجع FAIL;
	bi->bi_ورتیا = 0;
    }
    أرجع OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    ساكن صحيح
fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    إذا (bi->bi_state != عدم && bi->bi_بفر == عدم)
    {
	// crypting every piece of text separately
	إذا (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	وإلا
	{
	    copy = lalloc(len, FALSE);
	    إذا (copy == عدم)
		أرجع 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	إذا (copy != small_buf)
	    vim_free(copy);
	أرجع i == 1 ? OK : FAIL;
    }
# endif
    أرجع undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    ساكن صحيح
undo_write_بائٹاں(bufinfo_T *bi, long_u nr, صحيح len)
{
    char_u  buf[8];
    صحيح	    i;
    صحيح	    bufi = 0;

    لكل (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    أرجع undo_write(bi, buf, (size_t)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    ساكن فراغ
put_سرخی_ptr(bufinfo_T *bi, u_سرخی_T *uhp)
{
    undo_write_بائٹاں(bi, (long_u)(uhp != عدم ? uhp->uh_seq : 0), 4);
}

    ساكن صحيح
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    إذا (bi->bi_بفر != عدم)
    {
	char_u  buf[4];
	صحيح	n;

	undo_read(bi, buf, (size_t)4);
	n = ((بنانشانی)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	أرجع n;
    }
# endif
    أرجع get4c(bi->bi_fp);
}

    ساكن صحيح
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    إذا (bi->bi_بفر != عدم)
    {
	char_u  buf[2];
	صحيح	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	أرجع n;
    }
# endif
    أرجع get2c(bi->bi_fp);
}

    ساكن صحيح
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    إذا (bi->bi_بفر != عدم)
    {
	char_u  buf[1];

	undo_read(bi, buf, (size_t)1);
	أرجع buf[0];
    }
# endif
    أرجع getc(bi->bi_fp);
}

    ساكن time_t
undo_read_ویلا(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    إذا (bi->bi_بفر != عدم)
    {
	char_u  buf[8];
	time_t	n = 0;
	صحيح	i;

	undo_read(bi, buf, (size_t)8);
	لكل (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	أرجع n;
    }
# endif
    أرجع get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    ساكن صحيح
undo_read(bufinfo_T *bi, char_u *بفر, size_t ناپ)
{
    صحيح retval = OK;

# ifdef FEAT_CRYPT
    إذا (bi->bi_بفر != عدم)
    {
	صحيح	ناپ_todo = (صحيح)ناپ;
	char_u	*p = بفر;

	طالما (ناپ_todo > 0)
	{
	    size_t n;

	    إذا (bi->bi_ورتیا >= bi->bi_avail)
	    {
		n = fread(bi->bi_بفر, 1, (size_t)CRYPT_BUF_ناپ, bi->bi_fp);
		إذا (n == 0)
		{
		    retval = FAIL;
		    اكسر;
		}
		bi->bi_avail = n;
		bi->bi_ورتیا = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_بفر, bi->bi_avail, FALSE);
	    }
	    n = ناپ_todo;
	    إذا (n > bi->bi_avail - bi->bi_ورتیا)
		n = bi->bi_avail - bi->bi_ورتیا;
	    mch_memmove(p, bi->bi_بفر + bi->bi_ورتیا, n);
	    bi->bi_ورتیا += n;
	    ناپ_todo -= (صحيح)n;
	    p += n;
	}
    }
    وإلا
# endif
    إذا (fread(بفر, ناپ, 1, bi->bi_fp) != 1)
	retval = FAIL;

    إذا (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(بفر, 0, ناپ);
    أرجع retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    ساكن char_u *
read_string_decrypt(bufinfo_T *bi, صحيح len)
{
    char_u  *ptr = alloc(len + 1);

    إذا (ptr == عدم)
	أرجع عدم;

    إذا (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	أرجع عدم;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    إذا (bi->bi_state != عدم && bi->bi_بفر == عدم)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    أرجع ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    ساكن صحيح
serialize_سرخی(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	ویلا_buf[8];

    // Start writing, first the magic marker and undo info version.
    إذا (fwrite(UF_شروع_MAGIC, (size_t)UF_شروع_MAGIC_LEN, (size_t)1, fp) != 1)
	أرجع FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    إذا (*buf->b_p_کلید != NUL)
    {
	char_u *سرخی;
	صحيح    سرخی_len;
	لما	len;

	undo_write_بائٹاں(bi, (long_u)UF_نسخہ_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_طریقہ_nr(buf),
					  buf->b_p_کلید, &سرخی, &سرخی_len);
	إذا (bi->bi_state == عدم)
	    أرجع FAIL;
	len = (لما)fwrite(سرخی, (size_t)سرخی_len, (size_t)1, fp);
	vim_free(سرخی);
	إذا (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = عدم;
	    أرجع FAIL;
	}

	إذا (crypt_سارا_undofile(crypt_get_طریقہ_nr(buf)))
	{
	    bi->bi_بفر = alloc(CRYPT_BUF_ناپ);
	    إذا (bi->bi_بفر == عدم)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = عدم;
		أرجع FAIL;
	    }
	    bi->bi_ورتیا = 0;
	}
    }
    وإلا
# endif
	undo_write_بائٹاں(bi, (long_u)UF_نسخہ, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    إذا (undo_write(bi, hash, (size_t)UNDO_HASH_ناپ) == FAIL)
	أرجع FAIL;

    // buffer-specific data
    undo_write_بائٹاں(bi, (long_u)buf->b_ml.ml_سطر_گنتی, 4);
    undo_write_بائٹاں(bi, (long_u)buf->b_u_سطر_ptr.ul_textlen, 4);
    إذا (buf->b_u_سطر_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_سطر_ptr.ul_سطر,
			(size_t)buf->b_u_سطر_ptr.ul_textlen) == FAIL)
	أرجع FAIL;
    undo_write_بائٹاں(bi, (long_u)buf->b_u_سطر_lnum, 4);
    undo_write_بائٹاں(bi, (long_u)buf->b_u_سطر_colnr, 4);

    // Undo structures header data
    put_سرخی_ptr(bi, buf->b_u_oldhead);
    put_سرخی_ptr(bi, buf->b_u_newhead);
    put_سرخی_ptr(bi, buf->b_u_curhead);

    undo_write_بائٹاں(bi, (long_u)buf->b_u_numhead, 4);
    undo_write_بائٹاں(bi, (long_u)buf->b_u_seq_آخری, 4);
    undo_write_بائٹاں(bi, (long_u)buf->b_u_seq_cur, 4);
    ویلا_منزل_بائٹاں(buf->b_u_ویلا_cur, ویلا_buf);
    undo_write(bi, ویلا_buf, 8);

    // Optional fields.
    undo_write_بائٹاں(bi, 4, 1);
    undo_write_بائٹاں(bi, UF_آخری_SAVE_NR, 1);
    undo_write_بائٹاں(bi, (long_u)buf->b_u_save_nr_آخری, 4);

    undo_write_بائٹاں(bi, 0, 1);  // end marker

    أرجع OK;
}

    ساكن صحيح
serialize_uhp(bufinfo_T *bi, u_سرخی_T *uhp)
{
    صحيح		i;
    u_اندراج_T	*uep;
    char_u	ویلا_buf[8];

    إذا (undo_write_بائٹاں(bi, (long_u)UF_سرخی_MAGIC, 2) == FAIL)
	أرجع FAIL;

    put_سرخی_ptr(bi, uhp->uh_اگلا.ptr);
    put_سرخی_ptr(bi, uhp->uh_prev.ptr);
    put_سرخی_ptr(bi, uhp->uh_alt_اگلا.ptr);
    put_سرخی_ptr(bi, uhp->uh_alt_prev.ptr);
    undo_write_بائٹاں(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_بائٹاں(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_بائٹاں(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    لكل (i = 0; i < NMARKS; ++i)
	serialize_pos(bi, uhp->uh_namedm[i]);
    serialize_visualinfo(bi, &uhp->uh_visual);
    ویلا_منزل_بائٹاں(uhp->uh_ویلا, ویلا_buf);
    undo_write(bi, ویلا_buf, 8);

    // Optional fields.
    undo_write_بائٹاں(bi, 4, 1);
    undo_write_بائٹاں(bi, UHP_SAVE_NR, 1);
    undo_write_بائٹاں(bi, (long_u)uhp->uh_save_nr, 4);

    undo_write_بائٹاں(bi, 0, 1);  // end marker

    // Write all the entries.
    لكل (uep = uhp->uh_اندراج; uep != عدم; uep = uep->ue_اگلا)
    {
	undo_write_بائٹاں(bi, (long_u)UF_اندراج_MAGIC, 2);
	إذا (serialize_uep(bi, uep) == FAIL)
	    أرجع FAIL;
    }
    undo_write_بائٹاں(bi, (long_u)UF_اندراج_سرا_MAGIC, 2);
    أرجع OK;
}

    ساكن u_سرخی_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_سرخی_T	*uhp;
    صحيح		i;
    u_اندراج_T	*uep, *آخری_uep;
    صحيح		c;
    صحيح		error;

    uhp = U_ALLOC_سطر(حجم(u_سرخی_T));
    إذا (uhp == عدم)
	أرجع عدم;
    CLEAR_اشارہ_گر(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_اگلا.seq = undo_read_4c(bi);
    uhp->uh_prev.seq = undo_read_4c(bi);
    uhp->uh_alt_اگلا.seq = undo_read_4c(bi);
    uhp->uh_alt_prev.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    إذا (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	أرجع عدم;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    لكل (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_ویلا = undo_read_ویلا(bi);

    // Optional fields.
    لكل (;;)
    {
	صحيح len = undo_read_byte(bi);
	صحيح what;

	إذا (len == EOF)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    أرجع عدم;
	}
	إذا (len == 0)
	    اكسر;
	what = undo_read_byte(bi);
	بدل (what)
	{
	    حالة UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		اكسر;
	    افتراضي:
		// field not supported, skip
		طالما (--len >= 0)
		    (فراغ)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    آخری_uep = عدم;
    طالما ((c = undo_read_2c(bi)) == UF_اندراج_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	إذا (آخری_uep == عدم)
	    uhp->uh_اندراج = uep;
	وإلا
	    آخری_uep->ue_اگلا = uep;
	آخری_uep = uep;
	إذا (uep == عدم || error)
	{
	    u_free_uhp(uhp);
	    أرجع عدم;
	}
    }
    إذا (c != UF_اندراج_سرا_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	أرجع عدم;
    }

    أرجع uhp;
}

/*
 * Serialize "uep".
 */
    ساكن صحيح
serialize_uep(
    bufinfo_T	*bi,
    u_اندراج_T	*uep)
{
    صحيح		i;

    undo_write_بائٹاں(bi, (long_u)uep->ue_چوٹی, 4);
    undo_write_بائٹاں(bi, (long_u)uep->ue_bot, 4);
    undo_write_بائٹاں(bi, (long_u)uep->ue_lcount, 4);
    undo_write_بائٹاں(bi, (long_u)uep->ue_ناپ, 4);
    لكل (i = 0; i < uep->ue_ناپ; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	إذا (undo_write_بائٹاں(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    أرجع FAIL;
	إذا (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_سطر, uep->ue_array[i].ul_textlen) == FAIL)
	    أرجع FAIL;
    }
    أرجع OK;
}

    ساكن u_اندراج_T *
unserialize_uep(bufinfo_T *bi, صحيح *error, char_u *file_name)
{
    صحيح		i;
    u_اندراج_T	*uep;
    undoline_T	*array = عدم;
    char_u	*سطر;
    صحيح		سطر_len;

    uep = U_ALLOC_سطر(حجم(u_اندراج_T));
    إذا (uep == عدم)
	أرجع عدم;
    CLEAR_اشارہ_گر(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_چوٹی = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_ناپ = undo_read_4c(bi);
    إذا (uep->ue_ناپ > 0)
    {
	إذا (uep->ue_ناپ < LONG_MAX / (لما)حجم(undoline_T))
	    array = U_ALLOC_سطر(حجم(undoline_T) * uep->ue_ناپ);
	إذا (array == عدم)
	{
	    *error = TRUE;
	    أرجع uep;
	}
	vim_memset(array, 0, حجم(undoline_T) * uep->ue_ناپ);
    }
    uep->ue_array = array;

    لكل (i = 0; i < uep->ue_ناپ; ++i)
    {
	سطر_len = undo_read_4c(bi);
	إذا (سطر_len >= 0)
	    سطر = read_string_decrypt(bi, سطر_len);
	وإلا
	{
	    سطر = عدم;
	    corruption_error("line length", file_name);
	}
	إذا (سطر == عدم)
	{
	    *error = TRUE;
	    أرجع uep;
	}
	array[i].ul_سطر = سطر;
	array[i].ul_len = سطر_len + 1;
	array[i].ul_textlen = سطر_len;
    }
    أرجع uep;
}

/*
 * Serialize "pos".
 */
    ساكن فراغ
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_بائٹاں(bi, (long_u)pos.lnum, 4);
    undo_write_بائٹاں(bi, (long_u)pos.خانہ, 4);
    undo_write_بائٹاں(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    ساكن فراغ
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    إذا (pos->lnum < 0)
	pos->lnum = 0;
    pos->خانہ = undo_read_4c(bi);
    إذا (pos->خانہ < 0)
	pos->خانہ = 0;
    pos->coladd = undo_read_4c(bi);
    إذا (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    ساكن فراغ
serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    serialize_pos(bi, info->vi_شروع);
    serialize_pos(bi, info->vi_سرا);
    undo_write_بائٹاں(bi, (long_u)info->vi_منوال, 4);
    undo_write_بائٹاں(bi, (long_u)info->vi_curswant, 4);
}

/*
 * Unserialize the visualinfo_T at the current position.
 */
    ساكن فراغ
unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    unserialize_pos(bi, &info->vi_شروع);
    unserialize_pos(bi, &info->vi_سرا);
    info->vi_منوال = undo_read_4c(bi);
    info->vi_curswant = undo_read_4c(bi);
}

/*
 * Write the undo tree in an undo file.
 * When "name" is not NULL, use it as the name of the undo file.
 * Otherwise use buf->b_ffname to generate the undo file name.
 * "buf" must never be null, buf->b_ffname is used to obtain the original file
 * permissions.
 * "forceit" is TRUE for ":wundo!", FALSE otherwise.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    فراغ
u_write_undo(
    char_u	*name,
    صحيح		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_سرخی_T	*uhp;
    char_u	*file_name;
    صحيح		mark;
# ifdef U_DEBUG
    صحيح		سرخیاں_written = 0;
# endif
    صحيح		fd;
    FILE	*fp = عدم;
    صحيح		perm;
    صحيح		write_ok = FALSE;
# ifdef UNIX
    صحيح		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_میدان(bi);

    إذا (name == عدم)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	إذا (file_name == عدم)
	{
	    إذا (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    أرجع;
	}
    }
    وإلا
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    إذا (buf->b_ffname != عدم)
    {
# ifdef UNIX
	إذا (mch_stat((حرف *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_منوال;
	    st_old_valid = TRUE;
	}
# وإلا
	perm = mch_getperm(buf->b_ffname);
	إذا (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    إذا (mch_getperm(file_name) >= 0)
    {
	إذا (name == عدم || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((حرف *)file_name, O_RDONLY|O_EXTRA, 0);
	    إذا (fd < 0)
	    {
		إذا (name != عدم || p_verbose > 0)
		{
		    إذا (name == عدم)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    إذا (name == عدم)
			verbose_leave();
		}
		اذهب theend;
	    }
	    وإلا
	    {
		char_u	mbuf[UF_شروع_MAGIC_LEN];
		صحيح	len;

		len = read_eintr(fd, mbuf, UF_شروع_MAGIC_LEN);
		close(fd);
		إذا (len < UF_شروع_MAGIC_LEN
		      || memcmp(mbuf, UF_شروع_MAGIC, UF_شروع_MAGIC_LEN) != 0)
		{
		    إذا (name != عدم || p_verbose > 0)
		    {
			إذا (name == عدم)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			إذا (name == عدم)
			    verbose_leave();
		    }
		    اذهب theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    إذا (buf->b_u_numhead == 0 && buf->b_u_سطر_ptr.ul_سطر == عدم)
    {
	إذا (p_verbose > 0)
	    verb_پیغام(_("Skipping undo file write, nothing to undo"));
	اذهب theend;
    }

    fd = mch_open((حرف *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    إذا (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	اذهب theend;
    }
    (فراغ)mch_setperm(file_name, perm);
    إذا (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Writing undo file: %s"), file_name);
	verbose_leave();
    }

# ifdef U_DEBUG
    // Check there is no problem in undo info before writing.
    u_check(FALSE);
# endif

# ifdef UNIX
    /*
     * Try to set the group of the undo file same as the original file. If
     * this fails, set the protection bits for the group same as the
     * protection bits for others.
     */
    إذا (st_old_valid
	    && mch_stat((حرف *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  إذا defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    إذا (buf->b_ffname != عدم)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    إذا (fp == عدم)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	close(fd);
	mch_remove(file_name);
	اذهب theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    إذا (serialize_سرخی(&bi, hash) == FAIL)
	اذهب write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    طالما (uhp != عدم)
    {
	// Serialize current UHP if we haven't seen it
	إذا (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++سرخیاں_written;
# endif
	    إذا (serialize_uhp(&bi, uhp) == FAIL)
		اذهب write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	إذا (uhp->uh_prev.ptr != عدم && uhp->uh_prev.ptr->uh_walk != mark)
	    uhp = uhp->uh_prev.ptr;
	وإلا إذا (uhp->uh_alt_اگلا.ptr != عدم
				     && uhp->uh_alt_اگلا.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_اگلا.ptr;
	وإلا إذا (uhp->uh_اگلا.ptr != عدم && uhp->uh_alt_prev.ptr == عدم
					 && uhp->uh_اگلا.ptr->uh_walk != mark)
	    uhp = uhp->uh_اگلا.ptr;
	وإلا إذا (uhp->uh_alt_prev.ptr != عدم)
	    uhp = uhp->uh_alt_prev.ptr;
	وإلا
	    uhp = uhp->uh_اگلا.ptr;
    }

    إذا (undo_write_بائٹاں(&bi, (long_u)UF_سرخی_سرا_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    إذا (سرخیاں_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", سرخیاں_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    إذا (bi.bi_state != عدم && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# إذا defined(UNIX) && defined(HAVE_FSYNC)
    إذا ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && fflush(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    fclose(fp);
    إذا (!write_ok)
	semsg(_(e_write_error_وچ_undo_file_str), file_name);

# إذا defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    إذا (buf->b_ffname != عدم)
	(فراغ)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    إذا (buf->b_ffname != عدم)
    {
	vim_acl_T	    acl;

	// For systems that support ACL: get the ACL from the original file.
	acl = mch_get_acl(buf->b_ffname);
	mch_سیٹ_acl(file_name, acl);
	mch_free_acl(acl);
    }
# endif

theend:
# ifdef FEAT_CRYPT
    إذا (bi.bi_state != عدم)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_بفر);
# endif
    إذا (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    ساكن صحيح
uhp_seq_cmp(ثابت فراغ *v1, ثابت فراغ *v2)
{
    ثابت u_سرخی_T *u1 = *(u_سرخی_T **)v1;
    ثابت u_سرخی_T *u2 = *(u_سرخی_T **)v2;

    أرجع u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    ساكن لما
uhp_جدول_لبھو(u_سرخی_T **uhp_جدول, لما عدد_سر, لما seq)
{
    لما    lo = 0;
    لما    hi = عدد_سر - 1;

    طالما (lo <= hi)
    {
	لما mid = lo + (hi - lo) / 2;

	إذا (uhp_جدول[mid]->uh_seq < seq)
	    lo = mid + 1;
	وإلا إذا (uhp_جدول[mid]->uh_seq > seq)
	    hi = mid - 1;
	وإلا
	    أرجع mid;
    }
    أرجع -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    فراغ
u_read_undo(char_u *name, char_u *hash, char_u *orig_name بن_ورتیا)
{
    char_u	*file_name;
    FILE	*fp;
    لما	نسخہ_2, str_len;
    undoline_T	سطر_ptr;
    linenr_T	سطر_lnum;
    colnr_T	سطر_colnr;
    linenr_T	سطر_گنتی;
    لما	عدد_سر = 0;
    لما	old_سرخی_seq, new_سرخی_seq, cur_سرخی_seq;
    لما	seq_آخری, seq_cur;
    لما	آخری_save_nr = 0;
    لما	old_idx = -1, new_idx = -1, cur_idx = -1;
    لما	عدد_read_uhps = 0;
    time_t	seq_ویلا;
    لما	i;
    صحيح		c;
    u_سرخی_T	*uhp;
    u_سرخی_T	**uhp_جدول = عدم;
    char_u	read_hash[UNDO_HASH_ناپ];
    char_u	magic_buf[UF_شروع_MAGIC_LEN];
# ifdef U_DEBUG
    صحيح		*uhp_جدول_ورتیا;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_میدان(bi);
    سطر_ptr.ul_len = 0;
    سطر_ptr.ul_textlen = 0;
    سطر_ptr.ul_سطر = عدم;

    إذا (name == عدم)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	إذا (file_name == عدم)
	    أرجع;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	إذا (mch_stat((حرف *)orig_name, &st_orig) >= 0
		&& mch_stat((حرف *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    إذا (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    أرجع;
	}
# endif
    }
    وإلا
	file_name = name;

    إذا (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((حرف *)file_name, "r");
    إذا (fp == عدم)
    {
	إذا (name != عدم || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	اذهب error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    إذا (fread(magic_buf, UF_شروع_MAGIC_LEN, 1, fp) != 1
		|| memcmp(magic_buf, UF_شروع_MAGIC, UF_شروع_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	اذهب error;
    }
    نسخہ_2 = get2c(fp);
    إذا (نسخہ_2 == UF_نسخہ_CRYPT)
    {
# ifdef FEAT_CRYPT
	إذا (*curbuf->b_p_کلید == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    اذهب error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_کلید);
	إذا (bi.bi_state == عدم)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    اذهب error;
	}
	إذا (crypt_سارا_undofile(bi.bi_state->طریقہ_nr))
	{
	    bi.bi_بفر = alloc(CRYPT_BUF_ناپ);
	    إذا (bi.bi_بفر == عدم)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = عدم;
		اذهب error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_ورتیا = 0;
	}
# وإلا
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	اذهب error;
# endif
    }
    وإلا إذا (نسخہ_2 != UF_نسخہ)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	اذهب error;
    }

    إذا (undo_read(&bi, read_hash, (size_t)UNDO_HASH_ناپ) == FAIL)
    {
	corruption_error("hash", file_name);
	اذهب error;
    }
    سطر_گنتی = (linenr_T)undo_read_4c(&bi);
    إذا (memcmp(hash, read_hash, UNDO_HASH_ناپ) != 0
				  || سطر_گنتی != curbuf->b_ml.ml_سطر_گنتی)
    {
	إذا (p_verbose > 0 || name != عدم)
	{
	    إذا (name == عدم)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    إذا (name == عدم)
		verbose_leave();
	}
	اذهب error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    إذا (str_len < 0)
	اذهب error;
    إذا (str_len > 0)
    {
	سطر_ptr.ul_سطر = read_string_decrypt(&bi, str_len);
	سطر_ptr.ul_len = str_len + 1;
	سطر_ptr.ul_textlen = str_len;
    }
    سطر_lnum = (linenr_T)undo_read_4c(&bi);
    سطر_colnr = (colnr_T)undo_read_4c(&bi);
    إذا (سطر_lnum < 0 || سطر_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	اذهب error;
    }

    // Begin general undo data
    old_سرخی_seq = undo_read_4c(&bi);
    new_سرخی_seq = undo_read_4c(&bi);
    cur_سرخی_seq = undo_read_4c(&bi);
    عدد_سر = undo_read_4c(&bi);
    seq_آخری = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_ویلا = undo_read_ویلا(&bi);

    // Optional header fields.
    لكل (;;)
    {
	صحيح len = undo_read_byte(&bi);
	صحيح what;

	إذا (len == 0 || len == EOF)
	    اكسر;
	what = undo_read_byte(&bi);
	بدل (what)
	{
	    حالة UF_آخری_SAVE_NR:
		آخری_save_nr = undo_read_4c(&bi);
		اكسر;
	    افتراضي:
		// field not supported, skip
		طالما (--len >= 0)
		    (فراغ)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    إذا (عدد_سر > 0)
    {
	إذا (عدد_سر < LONG_MAX / (لما)حجم(u_سرخی_T *))
	    uhp_جدول = U_ALLOC_سطر(عدد_سر * حجم(u_سرخی_T *));
	إذا (uhp_جدول == عدم)
	    اذهب error;
    }

    طالما ((c = undo_read_2c(&bi)) == UF_سرخی_MAGIC)
    {
	إذا (عدد_read_uhps >= عدد_سر)
	{
	    corruption_error("num_head too small", file_name);
	    اذهب error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	إذا (uhp == عدم)
	    اذهب error;
	uhp_جدول[عدد_read_uhps++] = uhp;
    }

    إذا (عدد_read_uhps != عدد_سر)
    {
	corruption_error("num_head", file_name);
	اذهب error;
    }
    إذا (c != UF_سرخی_سرا_MAGIC)
    {
	corruption_error("end marker", file_name);
	اذهب error;
    }

# ifdef U_DEBUG
    uhp_جدول_ورتیا = alloc_clear(حجم(صحيح) * عدد_سر + 1);
#  define سیٹ_FLAG(j) ++uhp_جدول_ورتیا[j]
# وإلا
#  define سیٹ_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    إذا (عدد_سر > 0)
	qsort(uhp_جدول, (size_t)عدد_سر, حجم(u_سرخی_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    لكل (i = 0; i < عدد_سر - 1; i++)
    {
	إذا (uhp_جدول[i]->uh_seq == uhp_جدول[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    اذهب error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    افعل { \
	لما fidx = uhp_جدول_لبھو(uhp_جدول, عدد_سر, (link).seq); \
	\
	إذا (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_جدول[fidx]; \
	    سیٹ_FLAG(fidx); \
	} \
	وإلا \
	    (link).ptr = عدم; \
    } طالما (0)

    لكل (i = 0; i < عدد_سر; i++)
    {
	uhp = uhp_جدول[i];
	SWIZZLE_SEQ(uhp->uh_اگلا, i);
	SWIZZLE_SEQ(uhp->uh_prev, i);
	SWIZZLE_SEQ(uhp->uh_alt_اگلا, i);
	SWIZZLE_SEQ(uhp->uh_alt_prev, i);
	إذا (old_سرخی_seq > 0 && old_idx < 0 && uhp->uh_seq == old_سرخی_seq)
	{
	    old_idx = i;
	    سیٹ_FLAG(i);
	}
	إذا (new_سرخی_seq > 0 && new_idx < 0 && uhp->uh_seq == new_سرخی_seq)
	{
	    new_idx = i;
	    سیٹ_FLAG(i);
	}
	إذا (cur_سرخی_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_سرخی_seq)
	{
	    cur_idx = i;
	    سیٹ_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? عدم : uhp_جدول[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? عدم : uhp_جدول[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? عدم : uhp_جدول[cur_idx];
    curbuf->b_u_سطر_ptr = سطر_ptr;
    curbuf->b_u_سطر_lnum = سطر_lnum;
    curbuf->b_u_سطر_colnr = سطر_colnr;
    curbuf->b_u_numhead = عدد_سر;
    curbuf->b_u_seq_آخری = seq_آخری;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_ویلا_cur = seq_ویلا;
    curbuf->b_u_save_nr_آخری = آخری_save_nr;
    curbuf->b_u_save_nr_cur = آخری_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_جدول);

# ifdef U_DEBUG
    لكل (i = 0; i < عدد_سر; ++i)
	إذا (uhp_جدول_ورتیا[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_جدول_ورتیا);
    u_check(TRUE);
# endif

    إذا (name != عدم)
	smsg(_("Finished reading undo file %s"), file_name);
    اذهب theend;

error:
    vim_free(سطر_ptr.ul_سطر);
    إذا (uhp_جدول != عدم)
    {
	لكل (i = 0; i < عدد_read_uhps; i++)
	    إذا (uhp_جدول[i] != عدم)
		u_free_uhp(uhp_جدول[i]);
	vim_free(uhp_جدول);
    }

theend:
# ifdef FEAT_CRYPT
    إذا (bi.bi_state != عدم)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_بفر);
# endif
    إذا (fp != عدم)
	fclose(fp);
    إذا (file_name != name)
	vim_free(file_name);
    أرجع;
}

#جے_انت // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    فراغ
u_undo(صحيح گنتی)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    إذا (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	گنتی = 1;
    }

    إذا (vim_strchr(p_cpo, CPO_UNDO) == عدم)
	undo_undoes = TRUE;
    وإلا
	undo_undoes = !undo_undoes;
    u_doit(گنتی);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    فراغ
u_redo(صحيح گنتی)
{
    إذا (vim_strchr(p_cpo, CPO_UNDO) == عدم)
	undo_undoes = FALSE;
    u_doit(گنتی);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    ساكن فراغ
u_doit(صحيح startcount)
{
    صحيح گنتی = startcount;

    إذا (!undo_allowed())
	أرجع;

    u_newcount = 0;
    u_oldcount = 0;
    إذا (curbuf->b_ml.ml_flags & ML_خالی_تھاواں)
	u_oldcount = -1;
    طالما (گنتی--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	إذا (undo_undoes)
	{
	    إذا (curbuf->b_u_curhead == عدم)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    وإلا إذا (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_اگلا.ptr;
	    // nothing to undo
	    إذا (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == عدم)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		إذا (گنتی == startcount - 1)
		{
		    إذا (!shortmess(SHM_UNDO))
			پیغام(_("Already at oldest change"));
		    أرجع;
		}
		اكسر;
	    }

	    u_undoredo(TRUE);
	}
	وإلا
	{
	    إذا (curbuf->b_u_curhead == عدم || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		إذا (گنتی == startcount - 1)
		{
		    إذا (!shortmess(SHM_UNDO))
			پیغام(_("Already at newest change"));
		    أرجع;
		}
		اكسر;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    إذا (curbuf->b_u_curhead->uh_prev.ptr == عدم)
		curbuf->b_u_newhead = curbuf->b_u_curhead;
	    curbuf->b_u_curhead = curbuf->b_u_curhead->uh_prev.ptr;
	}
    }
    u_undo_سرا(undo_undoes, FALSE);
}

/*
 * Undo or redo over the timeline.
 * When "step" is negative go back in time, otherwise goes forward in time.
 * When "sec" is FALSE make "step" steps, when "sec" is TRUE use "step" as
 * seconds.
 * When "file" is TRUE use "step" as a number of file writes.
 * When "absolute" is TRUE use "step" as the sequence number to jump to.
 * "sec" must be FALSE then.
 */
    فراغ
undo_ویلا(
    لما	قدم,
    صحيح		sec,
    صحيح		file,
    صحيح		absolute)
{
    لما	    نشانہ;
    لما	    closest;
    لما	    closest_شروع;
    لما	    closest_seq = 0;
    لما	    val;
    u_سرخی_T	    *uhp = عدم;
    u_سرخی_T	    *آخری;
    صحيح		    mark;
    صحيح		    nomark = 0;  // shut up compiler
    صحيح		    گیڑا;
    صحيح		    dosec = sec;
    صحيح		    dofile = file;
    صحيح		    above = FALSE;
    صحيح		    did_undo = TRUE;

    إذا (متن_locked())
    {
	متن_locked_پیغام();
	أرجع;
    }

    // First make sure the current undoable change is synced.
    إذا (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    إذا (curbuf->b_ml.ml_flags & ML_خالی_تھاواں)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    إذا (absolute)
    {
	نشانہ = قدم;
	closest = -1;
    }
    وإلا
    {
	إذا (dosec)
	    نشانہ = (لما)(curbuf->b_u_ویلا_cur) + قدم;
	وإلا إذا (dofile)
	{
	    إذا (قدم < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		إذا (uhp != عدم)
		    uhp = uhp->uh_اگلا.ptr;
		وإلا
		    uhp = curbuf->b_u_newhead;
		إذا (uhp != عدم && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    نشانہ = curbuf->b_u_save_nr_cur + قدم;
		وإلا
		    // count the changes since the last write as one step
		    نشانہ = curbuf->b_u_save_nr_cur + قدم + 1;
		إذا (نشانہ <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    وإلا
	    {
		// Moving forward to a newer write.
		نشانہ = curbuf->b_u_save_nr_cur + قدم;
		إذا (نشانہ > curbuf->b_u_save_nr_آخری)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    نشانہ = curbuf->b_u_seq_آخری + 1;
		    dofile = FALSE;
		}
	    }
	}
	وإلا
	    نشانہ = curbuf->b_u_seq_cur + قدم;
	إذا (قدم < 0)
	{
	    إذا (نشانہ < 0)
		نشانہ = 0;
	    closest = -1;
	}
	وإلا
	{
	    إذا (dosec)
		closest = (لما)(vim_ویلا() + 1);
	    وإلا إذا (dofile)
		closest = curbuf->b_u_save_nr_آخری + 2;
	    وإلا
		closest = curbuf->b_u_seq_آخری + 2;
	    إذا (نشانہ >= closest)
		نشانہ = closest - 1;
	}
    }
    closest_شروع = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    إذا (نشانہ == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	اذهب نشانہ_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    لكل (گیڑا = 1; گیڑا <= 2; ++گیڑا)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	إذا (curbuf->b_u_curhead == عدم)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	وإلا
	    uhp = curbuf->b_u_curhead;

	طالما (uhp != عدم)
	{
	    uhp->uh_walk = mark;
	    إذا (dosec)
		val = (لما)(uhp->uh_ویلا);
	    وإلا إذا (dofile)
		val = uhp->uh_save_nr;
	    وإلا
		val = uhp->uh_seq;

	    إذا (گیڑا == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		إذا ((قدم < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
			      : uhp->uh_seq > curbuf->b_u_seq_cur)
			&& ((dosec && val == closest)
			    ? (قدم < 0
				? uhp->uh_seq < closest_seq
				: uhp->uh_seq > closest_seq)
			    : closest == closest_شروع
				|| (val > نشانہ
				    ? (closest > نشانہ
					? val - نشانہ <= closest - نشانہ
					: val - نشانہ <= نشانہ - closest)
				    : (closest > نشانہ
					? نشانہ - val <= closest - نشانہ
					: نشانہ - val <= نشانہ - closest))))
		{
		    closest = val;
		    closest_seq = uhp->uh_seq;
		}
	    }

	    // Quit searching when we found a match.  But when searching for a
	    // time we need to continue looking for the best uh_seq.
	    إذا (نشانہ == val && !dosec)
	    {
		نشانہ = uhp->uh_seq;
		اكسر;
	    }

	    // go down in the tree if we haven't been there
	    إذا (uhp->uh_prev.ptr != عدم && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
		uhp = uhp->uh_prev.ptr;

	    // go to alternate branch if we haven't been there
	    وإلا إذا (uhp->uh_alt_اگلا.ptr != عدم
		    && uhp->uh_alt_اگلا.ptr->uh_walk != nomark
		    && uhp->uh_alt_اگلا.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_اگلا.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    وإلا إذا (uhp->uh_اگلا.ptr != عدم && uhp->uh_alt_prev.ptr == عدم
		    && uhp->uh_اگلا.ptr->uh_walk != nomark
		    && uhp->uh_اگلا.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		إذا (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_اگلا.ptr;
	    }

	    وإلا
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		إذا (uhp->uh_alt_prev.ptr != عدم)
		    uhp = uhp->uh_alt_prev.ptr;
		وإلا
		    uhp = uhp->uh_اگلا.ptr;
	    }
	}

	إذا (uhp != عدم)    // found it
	    اكسر;

	إذا (absolute)
	{
	    semsg(_(e_undo_نمبر_nr_not_لبھیا), قدم);
	    أرجع;
	}

	إذا (closest == closest_شروع)
	{
	    إذا (!shortmess(SHM_UNDO))
	    {
		إذا (قدم < 0)
		    پیغام(_("Already at oldest change"));
		وإلا
		    پیغام(_("Already at newest change"));
	    }
	    أرجع;
	}

	نشانہ = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	إذا (قدم < 0)
	    above = TRUE;	// stop above the header
    }

نشانہ_zero:
    // If we found it: Follow the path to go to where we want to be.
    إذا (uhp != عدم || نشانہ == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	طالما (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    إذا (uhp == عدم)
		uhp = curbuf->b_u_newhead;
	    وإلا
		uhp = uhp->uh_اگلا.ptr;
	    إذا (uhp == عدم || (نشانہ > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == نشانہ && !above))
		اكسر;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    إذا (نشانہ > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	إذا (نشانہ > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    طالما (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		إذا (uhp == عدم)
		    اكسر;

		// Go back to the first branch with a mark.
		طالما (uhp->uh_alt_prev.ptr != عدم
				     && uhp->uh_alt_prev.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_prev.ptr;

		// Find the last branch with a mark, that's the one.
		آخری = uhp;
		طالما (آخری->uh_alt_اگلا.ptr != عدم
				    && آخری->uh_alt_اگلا.ptr->uh_walk == mark)
		    آخری = آخری->uh_alt_اگلا.ptr;
		إذا (آخری != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    طالما (uhp->uh_alt_prev.ptr != عدم)
			uhp = uhp->uh_alt_prev.ptr;
		    إذا (آخری->uh_alt_اگلا.ptr != عدم)
			آخری->uh_alt_اگلا.ptr->uh_alt_prev.ptr =
							آخری->uh_alt_prev.ptr;
		    آخری->uh_alt_prev.ptr->uh_alt_اگلا.ptr =
							آخری->uh_alt_اگلا.ptr;
		    آخری->uh_alt_prev.ptr = عدم;
		    آخری->uh_alt_اگلا.ptr = uhp;
		    uhp->uh_alt_prev.ptr = آخری;

		    إذا (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = آخری;
		    uhp = آخری;
		    إذا (uhp->uh_اگلا.ptr != عدم)
			uhp->uh_اگلا.ptr->uh_prev.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		إذا (uhp->uh_walk != mark)
		    اكسر;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		إذا (uhp->uh_seq == نشانہ && above)
		{
		    curbuf->b_u_seq_cur = نشانہ - 1;
		    اكسر;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		إذا (uhp->uh_prev.ptr == عدم)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_prev.ptr;
		did_undo = FALSE;

		إذا (uhp->uh_seq == نشانہ)	// found it!
		    اكسر;

		uhp = uhp->uh_prev.ptr;
		إذا (uhp == عدم || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    اندرونی_error("undo_time()");
		    اكسر;
		}
	    }
	}
    }
    u_undo_سرا(did_undo, absolute);
}

/*
 * u_undoredo: common code for undo and redo
 *
 * The lines in the file are replaced by the lines in the entry list at
 * curbuf->b_u_curhead. The replaced lines in the file are saved in the entry
 * list for the next undo/redo.
 *
 * When "undo" is TRUE we go up in the tree, when FALSE we go down.
 */
    ساكن فراغ
u_undoredo(صحيح undo)
{
    undoline_T	*newarray = عدم;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	چوٹی, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    لما	i;
    u_اندراج_T	*uep, *nuep;
    u_اندراج_T	*newlist = عدم;
    صحيح		old_flags;
    صحيح		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    صحيح		خالی_تھاواں_بفر;		    // buffer became empty
    u_سرخی_T	*curhead = curbuf->b_u_curhead;

    // Don't want autocommands using the undo structures here, they are
    // invalid till the end.
    بلاک_autocmds();

#جے_تعریف_شدہ U_DEBUG
    u_check(FALSE);
#جے_انت
    old_flags = curhead->uh_flags;
    new_flags = (curbuf->b_بدلیا ? UH_بدلیا : 0) +
	       ((curbuf->b_ml.ml_flags & ML_خالی_تھاواں) ? UH_EMPTYBUF : 0);
    setpcmark();

    /*
     * save marks before undo/redo
     */
    mch_memmove(namedm, curbuf->b_namedm, حجم(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_شروع.lnum = curbuf->b_ml.ml_سطر_گنتی;
    curbuf->b_op_شروع.خانہ = 0;
    curbuf->b_op_سرا.lnum = 0;
    curbuf->b_op_سرا.خانہ = 0;

    لكل (uep = curhead->uh_اندراج; uep != عدم; uep = nuep)
    {
	چوٹی = uep->ue_چوٹی;
	bot = uep->ue_bot;
	إذا (bot == 0)
	    bot = curbuf->b_ml.ml_سطر_گنتی + 1;
	إذا (چوٹی > curbuf->b_ml.ml_سطر_گنتی || چوٹی >= bot
				      || bot > curbuf->b_ml.ml_سطر_گنتی + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_سطر_numbers_wrong);
	    بدلیا();		// don't want UNCHANGED now
	    أرجع;
	}

	oldsize = bot - چوٹی - 1;    // number of lines before undo
	newsize = uep->ue_ناپ;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    إذا (lnum >= چوٹی && lnum <= چوٹی + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    وإلا إذا (چوٹی < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		لكل (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(چوٹی + 1 + i);

		    إذا (curbuf->b_ml.ml_سطر_len != uep->ue_array[i].ul_len
			    || memcmp(uep->ue_array[i].ul_سطر, p,
						curbuf->b_ml.ml_سطر_len) != 0)
			اكسر;
		}
		إذا (i == newsize && newlnum == MAXLNUM && uep->ue_اگلا == عدم)
		{
		    newlnum = چوٹی;
		    new_curpos.lnum = newlnum + 1;
		}
		وإلا إذا (i < newsize)
		{
		    newlnum = چوٹی + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	خالی_تھاواں_بفر = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	إذا (oldsize > 0)
	{
	    إذا ((newarray = U_ALLOC_سطر(حجم(undoline_T) * oldsize)) == عدم)
	    {
		do_outofmem_پیغام((long_u)(حجم(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		طالما (uep != عدم)
		{
		    nuep = uep->ue_اگلا;
		    u_freeentry(uep, uep->ue_ناپ);
		    uep = nuep;
		}
		اكسر;
	    }
	    // delete backwards, it goes faster in most cases
	    لكل (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		إذا (u_save_سطر(&newarray[i], lnum) == FAIL)
		    do_outofmem_پیغام((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		إذا (curbuf->b_ml.ml_سطر_گنتی == 1)
		    خالی_تھاواں_بفر = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	وإلا
	    newarray = عدم;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	إذا (newsize)
	{
	    لكل (lnum = چوٹی, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		إذا (خالی_تھاواں_بفر && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_سطر,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		وإلا
		    ml_append_flags(lnum, uep->ue_array[i].ul_سطر,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_سطر);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#جے_تعریف_شدہ FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, چوٹی, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, چوٹی + newsize + 1, &uep->ue_props,
								UNDOPROP_ہیٹھاں);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, چوٹی, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_ہیٹھاں);
	إذا (newsize > 0)
	    adjust_props_for_undo(curbuf, چوٹی + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_ہیٹھاں);
#جے_انت

	// adjust marks
	إذا (oldsize != newsize)
	{
	    mark_adjust(چوٹی + 1, چوٹی + oldsize, (لما)MAXLNUM,
					       (لما)newsize - (لما)oldsize);
	    إذا (curbuf->b_op_شروع.lnum > چوٹی + oldsize)
		curbuf->b_op_شروع.lnum += newsize - oldsize;
	    إذا (curbuf->b_op_سرا.lnum > چوٹی + oldsize)
		curbuf->b_op_سرا.lnum += newsize - oldsize;
	}
	إذا (oldsize > 0 || newsize > 0)
	{
	    بدلیا_سطریں(چوٹی + 1, 0, bot, newsize - oldsize);
#جے_تعریف_شدہ FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    إذا (spell_check_کھڑکی(curwin) && bot <= curbuf->b_ml.ml_سطر_گنتی)
		redrawWinline(curwin, bot);
#جے_انت
	}

	// Set the '[ mark.
	إذا (چوٹی + 1 < curbuf->b_op_شروع.lnum)
	    curbuf->b_op_شروع.lnum = چوٹی + 1;
	// Set the '] mark.
	إذا (newsize == 0 && چوٹی + 1 > curbuf->b_op_سرا.lnum)
	    curbuf->b_op_سرا.lnum = چوٹی + 1;
	وإلا إذا (چوٹی + newsize > curbuf->b_op_سرا.lnum)
	    curbuf->b_op_سرا.lnum = چوٹی + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_ناپ = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = چوٹی + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_اگلا;
	uep->ue_اگلا = newlist;
	newlist = uep;
    }

    // Ensure the '[ and '] marks are within bounds.
    إذا (curbuf->b_op_شروع.lnum > curbuf->b_ml.ml_سطر_گنتی)
	 curbuf->b_op_شروع.lnum = curbuf->b_ml.ml_سطر_گنتی;
    إذا (curbuf->b_op_سرا.lnum > curbuf->b_ml.ml_سطر_گنتی)
	 curbuf->b_op_سرا.lnum = curbuf->b_ml.ml_سطر_گنتی;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_اندراج = newlist;
    curhead->uh_flags = new_flags;
    إذا ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_خالی_تھاواں;
    إذا (old_flags & UH_بدلیا)
	بدلیا();
    وإلا
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	إذا (!isNetbeansModified(curbuf))
#جے_انت
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    لكل (i = 0; i < NMARKS; ++i)
    {
	إذا (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	إذا (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	وإلا
	    curhead->uh_namedm[i].lnum = 0;
    }
    إذا (curhead->uh_visual.vi_شروع.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    إذا (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    إذا (curwin->w_cursor.lnum <= curbuf->b_ml.ml_سطر_گنتی)
    {
	إذا (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.خانہ = curhead->uh_cursor.خانہ;
	    إذا (virtual_سرگرم() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    وإلا
		curwin->w_cursor.coladd = 0;
	}
	وإلا
	    beginline(BL_SOL | BL_FIX);
    }
    وإلا
    {
	// We get here with the current cursor line being past the end (eg
	// after adding lines at the end of the file, and then undoing it).
	// check_cursor() will move the cursor to the last line.  Move it to
	// the first column here.
	curwin->w_cursor.خانہ = 0;
	curwin->w_cursor.coladd = 0;
    }

    // Make sure the cursor is on an existing line and column.
    check_cursor();

    // Remember where we are for "g-" and ":earlier 10s".
    curbuf->b_u_seq_cur = curhead->uh_seq;
    إذا (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	إذا (curhead->uh_اگلا.ptr != عدم)
	    curbuf->b_u_seq_cur = curhead->uh_اگلا.ptr->uh_seq;
	وإلا
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    إذا (curhead->uh_save_nr != 0)
    {
	إذا (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	وإلا
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr;
    }

    // The timestamp can be the same for multiple changes, just use the one of
    // the undone/redone change.
    curbuf->b_u_ویلا_cur = curhead->uh_ویلا;

    unblock_autocmds();
#جے_تعریف_شدہ U_DEBUG
    u_check(FALSE);
#جے_انت
}

/*
 * If we deleted or added lines, report the number of less/more lines.
 * Otherwise, report the number of changes (this may be incorrect
 * in some cases, but it's better than nothing).
 */
    ساكن فراغ
u_undo_سرا(
    صحيح		did_undo,	// just did an undo
    صحيح		absolute)	// used ":undo N"
{
    حرف	*msgstr;
    u_سرخی_T	*uhp;
    char_u	msgbuf[80];

#جے_تعریف_شدہ FEAT_FOLDING
    إذا ((fdo_flags & FDO_UNDO) && کلید_Typed)
	foldOpenCursor();
#جے_انت

    إذا (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	أرجع;

    إذا (curbuf->b_ml.ml_flags & ML_خالی_تھاواں)
	--u_newcount;

    u_oldcount -= u_newcount;
    إذا (u_oldcount == -1)
	msgstr = N_("more line");
    وإلا إذا (u_oldcount < 0)
	msgstr = N_("more lines");
    وإلا إذا (u_oldcount == 1)
	msgstr = N_("line less");
    وإلا إذا (u_oldcount > 1)
	msgstr = N_("fewer lines");
    وإلا
    {
	u_oldcount = u_newcount;
	إذا (u_newcount == 1)
	    msgstr = N_("change");
	وإلا
	    msgstr = N_("changes");
    }

    إذا (curbuf->b_u_curhead != عدم)
    {
	// For ":undo N" we prefer a "after #N" message.
	إذا (absolute && curbuf->b_u_curhead->uh_اگلا.ptr != عدم)
	{
	    uhp = curbuf->b_u_curhead->uh_اگلا.ptr;
	    did_undo = FALSE;
	}
	وإلا إذا (did_undo)
	    uhp = curbuf->b_u_curhead;
	وإلا
	    uhp = curbuf->b_u_curhead->uh_اگلا.ptr;
    }
    وإلا
	uhp = curbuf->b_u_newhead;

    إذا (uhp == عدم)
	*msgbuf = NUL;
    وإلا
	جوڑو_ویلا(msgbuf, حجم(msgbuf), uhp->uh_ویلا);

#جے_تعریف_شدہ FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_بفر == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#جے_انت
    إذا (VIsual_سرگرم)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == عدم ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    فراغ
u_sync(
    صحيح	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    إذا (curbuf->b_u_synced || (!force && no_u_sync > 0))
	أرجع;
#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    إذا (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	أرجع;		    // XIM is busy, don't break an undo sequence
#جے_انت
    إذا (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    وإلا
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = عدم;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    فراغ
ex_undolist(exarg_T *eap بن_ورتیا)
{
    garray_T	ga;
    u_سرخی_T	*uhp;
    صحيح		mark;
    صحيح		nomark;
    صحيح		changes = 1;
    صحيح		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, حجم(حرف *), 20);

    uhp = curbuf->b_u_oldhead;
    طالما (uhp != عدم)
    {
	إذا (uhp->uh_prev.ptr == عدم && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    إذا (ga_grow(&ga, 1) == FAIL)
		اكسر;
	    len = vim_snprintf((حرف *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    جوڑو_ویلا(IObuff + len, IOSIZE - len, uhp->uh_ویلا);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (صحيح)STRLEN(IObuff + len);
	    إذا (uhp->uh_save_nr > 0)
	    {
		صحيح n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((حرف *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_انکڑے))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	إذا (uhp->uh_prev.ptr != عدم && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_prev.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	وإلا إذا (uhp->uh_alt_اگلا.ptr != عدم
		&& uhp->uh_alt_اگلا.ptr->uh_walk != nomark
		&& uhp->uh_alt_اگلا.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_اگلا.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	وإلا إذا (uhp->uh_اگلا.ptr != عدم && uhp->uh_alt_prev.ptr == عدم
		&& uhp->uh_اگلا.ptr->uh_walk != nomark
		&& uhp->uh_اگلا.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_اگلا.ptr;
	    --changes;
	}

	وإلا
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    إذا (uhp->uh_alt_prev.ptr != عدم)
		uhp = uhp->uh_alt_prev.ptr;
	    وإلا
	    {
		uhp = uhp->uh_اگلا.ptr;
		--changes;
	    }
	}
    }

    إذا (ga.ga_len == 0)
	پیغام(_("Nothing to undo"));
    وإلا
    {
	صحيح		i;

	ترتیب_دیو_لڑیاں((char_u **)ga.ga_انکڑے, ga.ga_len);

	پیغام_شروع();
	پیغام_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	لكل (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    پیغام_putchar('\n');
	    إذا (got_int)
		اكسر;
	    پیغام_puts(((حرف **)ga.ga_انکڑے)[i]);
	}
	پیغام_سرا();

	ga_clear_لڑیاں(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    فراغ
ex_undojoin(exarg_T *eap بن_ورتیا)
{
    إذا (curbuf->b_u_newhead == عدم)
	أرجع;		    // nothing changed before
    إذا (curbuf->b_u_curhead != عدم)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	أرجع;
    }
    إذا (!curbuf->b_u_synced)
	أرجع;		    // already unsynced
    إذا (get_undolevel() < 0)
	أرجع;		    // no entries, nothing to do
    وإلا
	// Append next change to the last entry
	curbuf->b_u_synced = false;
}

/*
 * Called after writing or reloading the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
    فراغ
u_unchanged(buf_T *buf)
{
    u_unch_branch(buf->b_u_oldhead);
    buf->b_did_warn = false;
}

/*
 * After reloading a buffer which was saved for 'undoreload': Find the first
 * line that was changed and set the cursor there.
 */
    فراغ
u_لبھو_پہلا_بدلیا(فراغ)
{
    u_سرخی_T	*uhp = curbuf->b_u_newhead;
    u_اندراج_T   *uep;
    linenr_T	lnum;

    إذا (curbuf->b_u_curhead != عدم || uhp == عدم)
	أرجع;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_اندراج;
    إذا (uep->ue_چوٹی != 0 || uep->ue_bot != 0)
	أرجع;

    لكل (lnum = 1; lnum < curbuf->b_ml.ml_سطر_گنتی
					      && lnum <= uep->ue_ناپ; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	إذا (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_سطر_len
		|| memcmp(p, uep->ue_array[lnum - 1].ul_سطر, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    أرجع;
	}
    }
    إذا (curbuf->b_ml.ml_سطر_گنتی != uep->ue_ناپ)
    {
	// lines added or deleted at the end, put the cursor there
	CLEAR_POS(&(uhp->uh_cursor));
	uhp->uh_cursor.lnum = lnum;
    }
}

/*
 * Increase the write count, store it in the last undo header, what would be
 * used for "u".
 */
    فراغ
u_update_save_nr(buf_T *buf)
{
    u_سرخی_T	*uhp;

    ++buf->b_u_save_nr_آخری;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_آخری;
    uhp = buf->b_u_curhead;
    إذا (uhp != عدم)
	uhp = uhp->uh_اگلا.ptr;
    وإلا
	uhp = buf->b_u_newhead;
    إذا (uhp != عدم)
	uhp->uh_save_nr = buf->b_u_save_nr_آخری;
}

    ساكن فراغ
u_unch_branch(u_سرخی_T *uhp)
{
    u_سرخی_T	*uh;

    لكل (uh = uhp; uh != عدم; uh = uh->uh_prev.ptr)
    {
	uh->uh_flags |= UH_بدلیا;
	إذا (uh->uh_alt_اگلا.ptr != عدم)
	    u_unch_branch(uh->uh_alt_اگلا.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    ساكن u_اندراج_T *
u_get_headentry(فراغ)
{
    إذا (curbuf->b_u_newhead == عدم || curbuf->b_u_newhead->uh_اندراج == عدم)
    {
	iemsg(e_undo_list_corrupt);
	أرجع عدم;
    }
    أرجع curbuf->b_u_newhead->uh_اندراج;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    ساكن فراغ
u_getbot(فراغ)
{
    u_اندراج_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    إذا (uep == عدم)
	أرجع;

    uep = curbuf->b_u_newhead->uh_getbot_اندراج;
    إذا (uep != عدم)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_سطر_گنتی - uep->ue_lcount;
	uep->ue_bot = uep->ue_چوٹی + uep->ue_ناپ + 1 + extra;
	إذا (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_سطر_گنتی)
	{
	    iemsg(e_undo_سطر_missing);
	    uep->ue_bot = uep->ue_چوٹی + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_اندراج = عدم;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    ساكن فراغ
u_freeheader(
    buf_T	    *buf,
    u_سرخی_T	    *uhp,
    u_سرخی_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_سرخی_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    إذا (uhp->uh_alt_اگلا.ptr != عدم)
	u_freebranch(buf, uhp->uh_alt_اگلا.ptr, uhpp);

    إذا (uhp->uh_alt_prev.ptr != عدم)
	uhp->uh_alt_prev.ptr->uh_alt_اگلا.ptr = عدم;

    // Update the links in the list to remove the header.
    إذا (uhp->uh_اگلا.ptr == عدم)
	buf->b_u_oldhead = uhp->uh_prev.ptr;
    وإلا
	uhp->uh_اگلا.ptr->uh_prev.ptr = uhp->uh_prev.ptr;

    إذا (uhp->uh_prev.ptr == عدم)
	buf->b_u_newhead = uhp->uh_اگلا.ptr;
    وإلا
	لكل (uhap = uhp->uh_prev.ptr; uhap != عدم;
						 uhap = uhap->uh_alt_اگلا.ptr)
	    uhap->uh_اگلا.ptr = uhp->uh_اگلا.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    ساكن فراغ
u_freebranch(
    buf_T	    *buf,
    u_سرخی_T	    *uhp,
    u_سرخی_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_سرخی_T	    *tofree, *اگلا;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    إذا (uhp == buf->b_u_oldhead)
    {
	طالما (buf->b_u_oldhead != عدم)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	أرجع;
    }

    إذا (uhp->uh_alt_prev.ptr != عدم)
	uhp->uh_alt_prev.ptr->uh_alt_اگلا.ptr = عدم;

    اگلا = uhp;
    طالما (اگلا != عدم)
    {
	tofree = اگلا;
	إذا (tofree->uh_alt_اگلا.ptr != عدم)
	    u_freebranch(buf, tofree->uh_alt_اگلا.ptr, uhpp);   // recursive
	اگلا = tofree->uh_prev.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}

/*
 * Free all the undo entries for one header and the header itself.
 * This means that "uhp" is invalid when returning.
 */
    ساكن فراغ
u_freeentries(
    buf_T	    *buf,
    u_سرخی_T	    *uhp,
    u_سرخی_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_اندراج_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    إذا (buf->b_u_curhead == uhp)
	buf->b_u_curhead = عدم;
    إذا (buf->b_u_newhead == uhp)
	buf->b_u_newhead = عدم;  // freeing the newest entry
    إذا (uhpp != عدم && uhp == *uhpp)
	*uhpp = عدم;

    لكل (uep = uhp->uh_اندراج; uep != عدم; uep = nuep)
    {
	nuep = uep->ue_اگلا;
	u_freeentry(uep, uep->ue_ناپ);
    }

#جے_تعریف_شدہ U_DEBUG
    uhp->uh_magic = 0;
#جے_انت
    vim_free((char_u *)uhp);
    --buf->b_u_numhead;
}

/*
 * free entry 'uep' and 'n' lines in uep->ue_array[]
 */
    ساكن فراغ
u_freeentry(u_اندراج_T *uep, لما n)
{
    طالما (n > 0)
	vim_free(uep->ue_array[--n].ul_سطر);
    vim_free((char_u *)uep->ue_array);
#جے_تعریف_شدہ FEAT_PROP_POPUP
    ga_clear(&uep->ue_props);
#جے_انت
#جے_تعریف_شدہ U_DEBUG
    uep->ue_magic = 0;
#جے_انت
    vim_free((char_u *)uep);
}

/*
 * invalidate the undo buffer; called when storage has already been released
 */
    ساكن فراغ
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = عدم;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_سطر_ptr.ul_سطر = عدم;
    buf->b_u_سطر_ptr.ul_len = 0;
    buf->b_u_سطر_ptr.ul_textlen = 0;
    buf->b_u_سطر_lnum = 0;
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 */
    ساكن فراغ
u_blockfree(buf_T *buf)
{
    طالما (buf->b_u_oldhead != عدم)
	u_freeheader(buf, buf->b_u_oldhead, عدم);
    vim_free(buf->b_u_سطر_ptr.ul_سطر);
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 * and invalidate the undo buffer
 */
    فراغ
u_clearallandblockfree(buf_T *buf)
{
    u_blockfree(buf);
    u_clearall(buf);
}



/*
 * Save the line "lnum" for the "U" command.
 */
    ساكن فراغ
u_saveline(linenr_T lnum)
{
    إذا (lnum == curbuf->b_u_سطر_lnum)	    // line is already saved
	أرجع;
    إذا (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_گنتی) // should never happen
	أرجع;
    u_clearline();
    curbuf->b_u_سطر_lnum = lnum;
    إذا (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_سطر_colnr = curwin->w_cursor.خانہ;
    وإلا
	curbuf->b_u_سطر_colnr = 0;
    إذا (u_save_سطر(&curbuf->b_u_سطر_ptr, lnum) == FAIL)
	do_outofmem_پیغام((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    فراغ
u_clearline(فراغ)
{
    إذا (curbuf->b_u_سطر_ptr.ul_سطر == عدم)
	أرجع;

    VIM_CLEAR(curbuf->b_u_سطر_ptr.ul_سطر);
    curbuf->b_u_سطر_ptr.ul_len = 0;
    curbuf->b_u_سطر_ptr.ul_textlen = 0;
    curbuf->b_u_سطر_lnum = 0;
}

/*
 * Implementation of the "U" command.
 * Differentiation from vi: "U" can be undone with the next "U".
 * We also allow the cursor to be in another line.
 * Careful: may trigger autocommands that reload the buffer.
 */
    فراغ
u_undoline(فراغ)
{
    colnr_T	t;
    undoline_T  oldp;

    إذا (undo_off)
	أرجع;

    إذا (curbuf->b_u_سطر_ptr.ul_سطر == عدم
			|| curbuf->b_u_سطر_lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	beep_flush();
	أرجع;
    }

    // first save the line for the 'u' command
    إذا (u_savecommon(curbuf->b_u_سطر_lnum - 1,
		       curbuf->b_u_سطر_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	أرجع;
    إذا (u_save_سطر(&oldp, curbuf->b_u_سطر_lnum) == FAIL)
    {
	do_outofmem_پیغام((long_u)0);
	أرجع;
    }
    ml_replace_len(curbuf->b_u_سطر_lnum, curbuf->b_u_سطر_ptr.ul_سطر,
				     curbuf->b_u_سطر_ptr.ul_len, TRUE, FALSE);
    بدلیا_بائٹاں(curbuf->b_u_سطر_lnum, 0);
    curbuf->b_u_سطر_ptr = oldp;

    t = curbuf->b_u_سطر_colnr;
    إذا (curwin->w_cursor.lnum == curbuf->b_u_سطر_lnum)
	curbuf->b_u_سطر_colnr = curwin->w_cursor.خانہ;
    curwin->w_cursor.خانہ = t;
    curwin->w_cursor.lnum = curbuf->b_u_سطر_lnum;
    check_cursor_خانہ();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    صحيح
buf_Is_بدلیا(buf_T *buf)
{
#جے_تعریف_شدہ FEAT_TERMINAL
    إذا (رکن_کم_running_not_none(buf->b_رکن))
	أرجع TRUE;
#جے_انت
    أرجع buf_Is_بدلیا_Not_رکن(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    صحيح
any_Buf_Is_بدلیا(فراغ)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	إذا (buf_Is_بدلیا(buf))
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    صحيح
buf_Is_بدلیا_Not_رکن(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    أرجع (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_بدلیا || file_ff_differs(buf, TRUE));
}

    صحيح
curbuf_Is_بدلیا(فراغ)
{
    أرجع buf_Is_بدلیا(curbuf);
}

#إذا defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    ساكن فراغ
u_eval_درخت(buf_T *buf, u_سرخی_T *پہلا_uhp, list_T *list)
{
    u_سرخی_T  *uhp = پہلا_uhp;
    dict_T	*dict;

    طالما (uhp != عدم)
    {
	dict = dict_alloc();
	إذا (dict == عدم)
	    أرجع;
	dict_جوڑو_نمبر(dict, "seq", uhp->uh_seq);
	dict_جوڑو_نمبر(dict, "time", (لما)uhp->uh_ویلا);
	إذا (uhp == buf->b_u_newhead)
	    dict_جوڑو_نمبر(dict, "newhead", 1);
	إذا (uhp == buf->b_u_curhead)
	    dict_جوڑو_نمبر(dict, "curhead", 1);
	إذا (uhp->uh_save_nr > 0)
	    dict_جوڑو_نمبر(dict, "save", uhp->uh_save_nr);

	إذا (uhp->uh_alt_اگلا.ptr != عدم)
	{
	    list_T	*alt_list = list_alloc();

	    إذا (alt_list != عدم)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_درخت(buf, uhp->uh_alt_اگلا.ptr, alt_list);
		dict_جوڑو_list(dict, "alt", alt_list);
	    }
	}

	list_append_dict(list, dict);
	uhp = uhp->uh_prev.ptr;
    }
}

/*
 * "undofile(name)" function
 */
    فراغ
f_undofile(typval_T *argvars, typval_T *rettv)
{
    إذا (وچ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	إذا (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = عدم;
	}
	وإلا
	{
	    char_u *ffname = بھریا_Name_save(fname, TRUE);

	    إذا (ffname != عدم)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# وإلا
    rettv->vval.v_string = عدم;
# endif
}
# ifdef FEAT_PERSISTENT_UNDO
/*
 * Reset undofile option and delete the undofile
 */
    فراغ
u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    إذا (!buf->b_p_udf)
	أرجع;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    إذا (file_name != عدم)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    سیٹ_option_قدر_give_err((char_u *)"undofile", 0L, عدم, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    فراغ
f_undotree(typval_T *argvars, typval_T *rettv)
{
    إذا (وچ_vim9script() && check_for_opt_بفر_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    إذا (buf == عدم)
	أرجع;

    dict_T *dict = rettv->vval.v_dict;

    dict_جوڑو_نمبر(dict, "synced", (لما)buf->b_u_synced);
    dict_جوڑو_نمبر(dict, "seq_last", buf->b_u_seq_آخری);
    dict_جوڑو_نمبر(dict, "save_last", buf->b_u_save_nr_آخری);
    dict_جوڑو_نمبر(dict, "seq_cur", buf->b_u_seq_cur);
    dict_جوڑو_نمبر(dict, "time_cur", (لما)buf->b_u_ویلا_cur);
    dict_جوڑو_نمبر(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    إذا (list != عدم)
    {
	u_eval_درخت(buf, buf->b_u_oldhead, list);
	dict_جوڑو_list(dict, "entries", list);
    }
}

#جے_انت
