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
#تعریف_کرو CRYPT_BUF_سائز 8192

#شامل "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
قسمتعریف ساخت {
    buf_T	*bi_buf;
    FILE	*bi_fp;
#اگر_تعریف_شدہ FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_بفر; // CRYPT_BUF_SIZE, NULL when not buffering
    size_t	bi_مستعمل;    // bytes written to/read from bi_buffer
    size_t	bi_avail;   // bytes available in bi_buffer
#اگر_ختم
} bufinfo_T;


ساکن خالی u_unch_branch(u_سرخی_T *uhp);
ساکن u_اندراج_T *u_get_headentry(خالی);
ساکن خالی u_getbot(خالی);
ساکن خالی u_doit(صحیح گنتی);
ساکن خالی u_undoredo(صحیح undo);
ساکن خالی u_undo_انتہا(صحیح did_undo, صحیح absolute);
ساکن خالی u_freeheader(buf_T *buf, u_سرخی_T *uhp, u_سرخی_T **uhpp);
ساکن خالی u_freebranch(buf_T *buf, u_سرخی_T *uhp, u_سرخی_T **uhpp);
ساکن خالی u_freeentries(buf_T *buf, u_سرخی_T *uhp, u_سرخی_T **uhpp);
ساکن خالی u_freeentry(u_اندراج_T *, طویل);
#اگر_تعریف_شدہ FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
ساکن صحیح undo_flush(bufinfo_T *bi);
# endif
ساکن صحیح undo_read(bufinfo_T *bi, char_u *بفر, size_t سائز);
ساکن صحیح serialize_uep(bufinfo_T *bi, u_اندراج_T *uep);
ساکن u_اندراج_T *unserialize_uep(bufinfo_T *bi, صحیح *error, char_u *file_name);
ساکن خالی serialize_pos(bufinfo_T *bi, pos_T pos);
ساکن خالی unserialize_pos(bufinfo_T *bi, pos_T *pos);
ساکن خالی serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
ساکن خالی unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#اگر_ختم
ساکن خالی u_saveline(linenr_T lnum);
ساکن خالی u_blockfree(buf_T *buf);

#تعریف_کرو U_ALLOC_سطر(سائز) lalloc(سائز, FALSE)

// used in undo_end() to report number of added and deleted lines
ساکن طویل	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
ساکن صحیح	undo_undoes = FALSE;

ساکن صحیح	lastmark = 0;

#اگر defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
ساکن صحیح نظر_آیا_b_u_curhead;
ساکن صحیح نظر_آیا_b_u_newhead;
ساکن صحیح سرخی_گنتی;

    ساکن خالی
u_check_درخت(u_سرخی_T *uhp,
	u_سرخی_T *exp_uh_اگلا,
	u_سرخی_T *exp_uh_alt_پچھلا)
{
    u_اندراج_T *uep;

    اگر (uhp == تہی)
	واپس;
    ++سرخی_گنتی;
    اگر (uhp == curbuf->b_u_curhead && ++نظر_آیا_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	واپس;
    }
    اگر (uhp == curbuf->b_u_newhead && ++نظر_آیا_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	واپس;
    }

    اگر (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    ورنہ
    {
	// Check pointers back are correct.
	اگر (uhp->uh_اگلا.ptr != exp_uh_اگلا)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_اگلا, uhp->uh_اگلا.ptr);
	}
	اگر (uhp->uh_alt_پچھلا.ptr != exp_uh_alt_پچھلا)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_پچھلا, uhp->uh_alt_پچھلا.ptr);
	}

	// Check the undo tree at this header.
	برائے (uep = uhp->uh_اندراج; uep != تہی; uep = uep->ue_اگلا)
	{
	    اگر (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		توڑو;
	    }
	}

	// Check the next alt tree.
	u_check_درخت(uhp->uh_alt_اگلا.ptr, uhp->uh_اگلا.ptr, uhp);

	// Check the next header in this branch.
	u_check_درخت(uhp->uh_پچھلا.ptr, uhp, تہی);
    }
}

    ساکن خالی
u_check(صحیح newhead_may_be_NULL)
{
    نظر_آیا_b_u_newhead = 0;
    نظر_آیا_b_u_curhead = 0;
    سرخی_گنتی = 0;

    u_check_درخت(curbuf->b_u_oldhead, تہی, تہی);

    اگر (نظر_آیا_b_u_newhead == 0 && curbuf->b_u_oldhead != تہی
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == تہی))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    اگر (curbuf->b_u_curhead != تہی && نظر_آیا_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    اگر (سرخی_گنتی != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (طویل)سرخی_گنتی, (طویل)curbuf->b_u_numhead);
    }
}
#اگر_ختم

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    صحیح
u_save_cursor(خالی)
{
    واپس (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    صحیح
u_save(linenr_T چوٹی, linenr_T bot)
{
    اگر (undo_off)
	واپس OK;

    اگر (چوٹی >= bot || bot > curbuf->b_ml.ml_سطر_گنتی + 1)
	واپس FAIL;	// rely on caller to give an error message

    اگر (چوٹی + 2 == bot)
	u_saveline((linenr_T)(چوٹی + 1));

    واپس (u_savecommon(چوٹی, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    صحیح
u_savesub(linenr_T lnum)
{
    اگر (undo_off)
	واپس OK;

    واپس (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    صحیح
u_inssub(linenr_T lnum)
{
    اگر (undo_off)
	واپس OK;

    واپس (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    صحیح
u_savedel(linenr_T lnum, طویل nlines)
{
    اگر (undo_off)
	واپس OK;

    واپس (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_سطر_گنتی ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    صحیح
undo_allowed(خالی)
{
    // Don't allow changes when 'modifiable' is off.
    اگر (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	واپس FALSE;
    }

#اگر_تعریف_شدہ HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    اگر (sandbox != 0)
    {
	emsg(_(e_not_allowed_اندر_sandbox));
	واپس FALSE;
    }
#اگر_ختم

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    اگر (textlock != 0)
    {
	emsg(_(e_not_allowed_مقصد_change_متن_or_change_کھڑکی));
	واپس FALSE;
    }

    واپس TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    ساکن طویل
get_undolevel(خالی)
{
    اگر (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	واپس p_ul;
    واپس curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    ساکن صحیح
u_save_سطر(undoline_T *ul, linenr_T lnum)
{
    char_u *سطر = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    اگر (curbuf->b_ml.ml_سطر_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_سطر = vim_strsave((char_u *)"");
    }
    ورنہ
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_سطر_len;
	ul->ul_سطر = vim_memsave(سطر, ul->ul_len);
    }
    واپس ul->ul_سطر == تہی ? FAIL : OK;
}

#اگر_تعریف_شدہ FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    ساکن صحیح
has_prop_w_flags(linenr_T lnum, صحیح flags)
{
    char_u  *props;
    صحیح	    i;
    صحیح	    proplen = get_متن_props(curbuf, lnum, &props, FALSE);

    برائے (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * حجم prop, حجم prop);
	اگر (prop.tp_flags & flags)
	    واپس TRUE;
    }
    واپس FALSE;
}
#اگر_ختم

/*
 * Common code for various ways to save text before a change.
 * "top" is the line above the first changed line.
 * "bot" is the line below the last changed line.
 * "newbot" is the new bottom line.  Use zero when not known.
 * "reload" is TRUE when saving for a buffer reload.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    صحیح
u_savecommon(
    linenr_T	چوٹی,
    linenr_T	bot,
    linenr_T	newbot,
    صحیح		reload)
{
    linenr_T	lnum;
    طویل	i;
    u_سرخی_T	*uhp;
    u_سرخی_T	*old_curhead;
    u_اندراج_T	*uep;
    u_اندراج_T	*پچھلا_uep;
    طویل	سائز;

    اگر (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	اگر (!undo_allowed())
	    واپس FAIL;

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	اگر (netbeans_فعال())
	{
	    اگر (netbeans_is_guarded(چوٹی, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		واپس FAIL;
	    }
	    اگر (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_اندر_read_only_files));
		واپس FAIL;
	    }
	}
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
	// A change in a terminal buffer removes the highlighting.
	رکن_change_اندر_curbuf();
#اگر_ختم

	/*
	 * Saving text for undo means we are going to make a change.  Give a
	 * warning for a read-only file before making the change, so that the
	 * FileChangedRO event can replace the buffer with a read-write version
	 * (e.g., obtained from a source control system).
	 */
	change_warning(0);
	اگر (bot > curbuf->b_ml.ml_سطر_گنتی + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_سطر_گنتی_بدلا_unexpectedly));
	    واپس FAIL;
	}
    }

#اگر_تعریف_شدہ U_DEBUG
    u_check(FALSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    اگر (bot - چوٹی > 1)
    {
	اگر (چوٹی > 0 && has_prop_w_flags(چوٹی + 1, TP_FLAG_CONT_پچھلا))
	    --چوٹی;
	اگر (bot <= curbuf->b_ml.ml_سطر_گنتی
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_اگلا))
	{
	    ++bot;
	    اگر (newbot != 0)
		++newbot;
	}
    }
#اگر_ختم

    سائز = bot - چوٹی - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    اگر (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	اگر (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_سطر(حجم(u_سرخی_T));
	    اگر (uhp == تہی)
		جاؤ nomem;
#اگر_تعریف_شدہ U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#اگر_ختم
	}
	ورنہ
	    uhp = تہی;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	اگر (old_curhead != تہی)
	{
	    curbuf->b_u_newhead = old_curhead->uh_اگلا.ptr;
	    curbuf->b_u_curhead = تہی;
	}

	/*
	 * free headers to keep the size right
	 */
	جب (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != تہی)
	{
	    u_سرخی_T	    *uhfree = curbuf->b_u_oldhead;

	    اگر (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    ورنہ اگر (uhfree->uh_alt_اگلا.ptr == تہی)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    ورنہ
	    {
		// Free the oldest alternate branch as a whole.
		جب (uhfree->uh_alt_اگلا.ptr != تہی)
		    uhfree = uhfree->uh_alt_اگلا.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#اگر_تعریف_شدہ U_DEBUG
	    u_check(TRUE);
#اگر_ختم
	}

	اگر (uhp == تہی)		// no undo at all
	{
	    اگر (old_curhead != تہی)
		u_freebranch(curbuf, old_curhead, تہی);
	    curbuf->b_u_synced = false;
	    واپس OK;
	}

	uhp->uh_پچھلا.ptr = تہی;
	uhp->uh_اگلا.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_اگلا.ptr = old_curhead;
	اگر (old_curhead != تہی)
	{
	    uhp->uh_alt_پچھلا.ptr = old_curhead->uh_alt_پچھلا.ptr;
	    اگر (uhp->uh_alt_پچھلا.ptr != تہی)
		uhp->uh_alt_پچھلا.ptr->uh_alt_اگلا.ptr = uhp;
	    old_curhead->uh_alt_پچھلا.ptr = uhp;
	    اگر (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	ورنہ
	    uhp->uh_alt_پچھلا.ptr = تہی;
	اگر (curbuf->b_u_newhead != تہی)
	    curbuf->b_u_newhead->uh_پچھلا.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_آخری;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_وقت = vim_وقت();
	uhp->uh_save_nr = 0;
	curbuf->b_u_وقت_cur = uhp->uh_وقت + 1;

	uhp->uh_walk = 0;
	uhp->uh_اندراج = تہی;
	uhp->uh_getbot_اندراج = تہی;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	اگر (virtual_فعال() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	ورنہ
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_بدلا ? UH_بدلا : 0) +
		       ((curbuf->b_ml.ml_flags & ML_خالی_خانے) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, حجم(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	اگر (curbuf->b_u_oldhead == تہی)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    ورنہ
    {
	اگر (get_undolevel() < 0)	// no undo at all
	    واپس OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	اگر (سائز == 1)
	{
	    uep = u_get_headentry();
	    پچھلا_uep = تہی;
	    برائے (i = 0; i < 10; ++i)
	    {
		اگر (uep == تہی)
		    توڑو;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		اگر ((curbuf->b_u_newhead->uh_getbot_اندراج != uep
			    ? (uep->ue_چوٹی + uep->ue_سائز + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_سطر_گنتی + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_سطر_گنتی)
			|| (uep->ue_سائز > 1
			    && چوٹی >= uep->ue_چوٹی
			    && چوٹی + 2 <= uep->ue_چوٹی + uep->ue_سائز + 1))
		    توڑو;

		// If it's the same line we can skip saving it again.
		اگر (uep->ue_سائز == 1 && uep->ue_چوٹی == چوٹی)
		{
		    اگر (i > 0)
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
			پچھلا_uep->ue_اگلا = uep->ue_اگلا;
			uep->ue_اگلا = curbuf->b_u_newhead->uh_اندراج;
			curbuf->b_u_newhead->uh_اندراج = uep;
		    }

		    // The executed command may change the line count.
		    اگر (newbot != 0)
			uep->ue_bot = newbot;
		    ورنہ اگر (bot > curbuf->b_ml.ml_سطر_گنتی)
			uep->ue_bot = 0;
		    ورنہ
		    {
			uep->ue_lcount = curbuf->b_ml.ml_سطر_گنتی;
			curbuf->b_u_newhead->uh_getbot_اندراج = uep;
		    }
		    واپس OK;
		}
		پچھلا_uep = uep;
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
    اگر (uep == تہی)
	جاؤ nomem;
    CLEAR_اشارہ_گر(uep);
#اگر_تعریف_شدہ U_DEBUG
    uep->ue_magic = UE_MAGIC;
#اگر_ختم

    uep->ue_سائز = سائز;
    uep->ue_چوٹی = چوٹی;
    اگر (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    ورنہ اگر (bot > curbuf->b_ml.ml_سطر_گنتی)
	uep->ue_bot = 0;
    ورنہ
    {
	uep->ue_lcount = curbuf->b_ml.ml_سطر_گنتی;
	curbuf->b_u_newhead->uh_getbot_اندراج = uep;
    }

    اگر (سائز > 0)
    {
	اگر ((uep->ue_array = U_ALLOC_سطر(حجم(undoline_T) * سائز)) == تہی)
	{
	    u_freeentry(uep, 0L);
	    جاؤ nomem;
	}
	برائے (i = 0, lnum = چوٹی + 1; i < سائز; ++i)
	{
	    fast_breakcheck();
	    اگر (got_int)
	    {
		u_freeentry(uep, i);
		واپس FAIL;
	    }
	    اگر (u_save_سطر(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		جاؤ nomem;
	    }
	}
    }
    ورنہ
	uep->ue_array = تہی;
    uep->ue_اگلا = curbuf->b_u_newhead->uh_اندراج;
    curbuf->b_u_newhead->uh_اندراج = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#اگر_تعریف_شدہ U_DEBUG
    u_check(FALSE);
#اگر_ختم
    واپس OK;

nomem:
    پیغام_silent = 0;	// must display the prompt
    اگر (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	واپس OK;
    }
    do_outofmem_پیغام((long_u)0);
    واپس FAIL;
}

#اگر defined(FEAT_PERSISTENT_UNDO)

# define UF_آغاز_MAGIC	    "Vim\237UnDo\345"  // magic at start of undofile
# define UF_آغاز_MAGIC_LEN	9
# define UF_سرخی_MAGIC	0x5fd0	// magic at start of header
# define UF_سرخی_انتہا_MAGIC	0xe7aa	// magic after last header
# define UF_اندراج_MAGIC		0xf518	// magic at start of entry
# define UF_اندراج_انتہا_MAGIC	0x3581	// magic after last entry
# define UF_نسخہ		2	// 2-byte undofile version number
# define UF_نسخہ_CRYPT	0x8002	// idem, encrypted

// extra fields for header
# define UF_آخری_SAVE_NR	1

// extra fields for uhp
# define UHP_SAVE_NR		1

/*
 * Compute the hash for the current buffer text into hash[UNDO_HASH_SIZE].
 */
    خالی
u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;

    sha256_آغاز(&ctx);
    برائے (lnum = 1; lnum <= curbuf->b_ml.ml_سطر_گنتی; ++lnum)
	sha256_update(&ctx, ml_get(lnum), (UINT32_T)(ml_get_len(lnum) + 1));
    sha256_اختتام(&ctx, hash);
}

/*
 * Return an allocated string of the full path of the target undofile.
 * When "reading" is TRUE find the file to read, go over all directories in
 * 'undodir'.
 * When "reading" is FALSE use the first name where the directory exists.
 * Returns NULL when there is no place to write or no file to read.
 */
    ساکن char_u *
u_get_undo_file_name(char_u *buf_ffname, صحیح reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {تہی, 0};
    char_u	*undo_file_name = تہی;
    صحیح		dir_len;
    char_u	*p;
    size_t	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    size_t	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    اگر (ffname == تہی)
	واپس تہی;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    اگر (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    جب (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	اگر (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    اگر (undo_file_name == تہی)
		توڑو;
	    p = gettail(undo_file_name);
	    plen = (size_t)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# ورنہ
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	ورنہ
	{
	    dir_name[dir_len] = NUL;
	    اگر (mch_isdir(dir_name))
	    {
		string_T    ret;

		اگر (munged_name.string == تہی)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    اگر (munged_name.string == تہی)
			واپس تہی;
		    munged_name.لمبائی = ffnamelen;
		    برائے (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			اگر (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (size_t)dir_len,
		    munged_name.string, munged_name.لمبائی, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	اگر (undo_file_name != تہی && (!reading
			       || mch_stat((حرف *)undo_file_name, &st) >= 0))
	    توڑو;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    واپس undo_file_name;
}

    ساکن خالی
corruption_error(حرف *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    ساکن خالی
u_free_uhp(u_سرخی_T *uhp)
{
    u_اندراج_T	*nuep;
    u_اندراج_T	*uep;

    uep = uhp->uh_اندراج;
    جب (uep != تہی)
    {
	nuep = uep->ue_اگلا;
	u_freeentry(uep, uep->ue_سائز);
	uep = nuep;
    }
    vim_free(uhp);
}

/*
 * Write a sequence of bytes to the undo file.
 * Buffers and encrypts as needed.
 * Returns OK or FAIL.
 */
    ساکن صحیح
undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    اگر (bi->bi_بفر != تہی)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	جب (bi->bi_مستعمل + len_todo >= CRYPT_BUF_سائز)
	{
	    size_t	n = CRYPT_BUF_سائز - bi->bi_مستعمل;

	    mch_memmove(bi->bi_بفر + bi->bi_مستعمل, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_مستعمل = CRYPT_BUF_سائز;
	    اگر (undo_flush(bi) == FAIL)
		واپس FAIL;
	}
	اگر (len_todo > 0)
	{
	    mch_memmove(bi->bi_بفر + bi->bi_مستعمل, p, len_todo);
	    bi->bi_مستعمل += len_todo;
	}
	واپس OK;
    }
# endif
    اگر (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	واپس FAIL;
    واپس OK;
}

# ifdef FEAT_CRYPT
    ساکن صحیح
undo_flush(bufinfo_T *bi)
{
    اگر (bi->bi_بفر != تہی && bi->bi_state != تہی && bi->bi_مستعمل > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_بفر, bi->bi_مستعمل, FALSE);
	اگر (fwrite(bi->bi_بفر, bi->bi_مستعمل, (size_t)1, bi->bi_fp) != 1)
	    واپس FAIL;
	bi->bi_مستعمل = 0;
    }
    واپس OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    ساکن صحیح
fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    اگر (bi->bi_state != تہی && bi->bi_بفر == تہی)
    {
	// crypting every piece of text separately
	اگر (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	ورنہ
	{
	    copy = lalloc(len, FALSE);
	    اگر (copy == تہی)
		واپس 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	اگر (copy != small_buf)
	    vim_free(copy);
	واپس i == 1 ? OK : FAIL;
    }
# endif
    واپس undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    ساکن صحیح
undo_write_بائٹیں(bufinfo_T *bi, long_u nr, صحیح len)
{
    char_u  buf[8];
    صحیح	    i;
    صحیح	    bufi = 0;

    برائے (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    واپس undo_write(bi, buf, (size_t)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    ساکن خالی
put_سرخی_ptr(bufinfo_T *bi, u_سرخی_T *uhp)
{
    undo_write_بائٹیں(bi, (long_u)(uhp != تہی ? uhp->uh_seq : 0), 4);
}

    ساکن صحیح
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    اگر (bi->bi_بفر != تہی)
    {
	char_u  buf[4];
	صحیح	n;

	undo_read(bi, buf, (size_t)4);
	n = ((غیرعلامتی)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	واپس n;
    }
# endif
    واپس get4c(bi->bi_fp);
}

    ساکن صحیح
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    اگر (bi->bi_بفر != تہی)
    {
	char_u  buf[2];
	صحیح	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	واپس n;
    }
# endif
    واپس get2c(bi->bi_fp);
}

    ساکن صحیح
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    اگر (bi->bi_بفر != تہی)
    {
	char_u  buf[1];

	undo_read(bi, buf, (size_t)1);
	واپس buf[0];
    }
# endif
    واپس getc(bi->bi_fp);
}

    ساکن time_t
undo_read_وقت(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    اگر (bi->bi_بفر != تہی)
    {
	char_u  buf[8];
	time_t	n = 0;
	صحیح	i;

	undo_read(bi, buf, (size_t)8);
	برائے (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	واپس n;
    }
# endif
    واپس get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    ساکن صحیح
undo_read(bufinfo_T *bi, char_u *بفر, size_t سائز)
{
    صحیح retval = OK;

# ifdef FEAT_CRYPT
    اگر (bi->bi_بفر != تہی)
    {
	صحیح	سائز_todo = (صحیح)سائز;
	char_u	*p = بفر;

	جب (سائز_todo > 0)
	{
	    size_t n;

	    اگر (bi->bi_مستعمل >= bi->bi_avail)
	    {
		n = fread(bi->bi_بفر, 1, (size_t)CRYPT_BUF_سائز, bi->bi_fp);
		اگر (n == 0)
		{
		    retval = FAIL;
		    توڑو;
		}
		bi->bi_avail = n;
		bi->bi_مستعمل = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_بفر, bi->bi_avail, FALSE);
	    }
	    n = سائز_todo;
	    اگر (n > bi->bi_avail - bi->bi_مستعمل)
		n = bi->bi_avail - bi->bi_مستعمل;
	    mch_memmove(p, bi->bi_بفر + bi->bi_مستعمل, n);
	    bi->bi_مستعمل += n;
	    سائز_todo -= (صحیح)n;
	    p += n;
	}
    }
    ورنہ
# endif
    اگر (fread(بفر, سائز, 1, bi->bi_fp) != 1)
	retval = FAIL;

    اگر (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(بفر, 0, سائز);
    واپس retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    ساکن char_u *
read_string_decrypt(bufinfo_T *bi, صحیح len)
{
    char_u  *ptr = alloc(len + 1);

    اگر (ptr == تہی)
	واپس تہی;

    اگر (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	واپس تہی;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    اگر (bi->bi_state != تہی && bi->bi_بفر == تہی)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    واپس ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    ساکن صحیح
serialize_سرخی(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	وقت_buf[8];

    // Start writing, first the magic marker and undo info version.
    اگر (fwrite(UF_آغاز_MAGIC, (size_t)UF_آغاز_MAGIC_LEN, (size_t)1, fp) != 1)
	واپس FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    اگر (*buf->b_p_کلید != NUL)
    {
	char_u *سرخی;
	صحیح    سرخی_len;
	طویل	len;

	undo_write_بائٹیں(bi, (long_u)UF_نسخہ_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_طریقہ_nr(buf),
					  buf->b_p_کلید, &سرخی, &سرخی_len);
	اگر (bi->bi_state == تہی)
	    واپس FAIL;
	len = (طویل)fwrite(سرخی, (size_t)سرخی_len, (size_t)1, fp);
	vim_free(سرخی);
	اگر (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = تہی;
	    واپس FAIL;
	}

	اگر (crypt_پورا_undofile(crypt_get_طریقہ_nr(buf)))
	{
	    bi->bi_بفر = alloc(CRYPT_BUF_سائز);
	    اگر (bi->bi_بفر == تہی)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = تہی;
		واپس FAIL;
	    }
	    bi->bi_مستعمل = 0;
	}
    }
    ورنہ
# endif
	undo_write_بائٹیں(bi, (long_u)UF_نسخہ, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    اگر (undo_write(bi, hash, (size_t)UNDO_HASH_سائز) == FAIL)
	واپس FAIL;

    // buffer-specific data
    undo_write_بائٹیں(bi, (long_u)buf->b_ml.ml_سطر_گنتی, 4);
    undo_write_بائٹیں(bi, (long_u)buf->b_u_سطر_ptr.ul_textlen, 4);
    اگر (buf->b_u_سطر_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_سطر_ptr.ul_سطر,
			(size_t)buf->b_u_سطر_ptr.ul_textlen) == FAIL)
	واپس FAIL;
    undo_write_بائٹیں(bi, (long_u)buf->b_u_سطر_lnum, 4);
    undo_write_بائٹیں(bi, (long_u)buf->b_u_سطر_colnr, 4);

    // Undo structures header data
    put_سرخی_ptr(bi, buf->b_u_oldhead);
    put_سرخی_ptr(bi, buf->b_u_newhead);
    put_سرخی_ptr(bi, buf->b_u_curhead);

    undo_write_بائٹیں(bi, (long_u)buf->b_u_numhead, 4);
    undo_write_بائٹیں(bi, (long_u)buf->b_u_seq_آخری, 4);
    undo_write_بائٹیں(bi, (long_u)buf->b_u_seq_cur, 4);
    وقت_مقصد_بائٹیں(buf->b_u_وقت_cur, وقت_buf);
    undo_write(bi, وقت_buf, 8);

    // Optional fields.
    undo_write_بائٹیں(bi, 4, 1);
    undo_write_بائٹیں(bi, UF_آخری_SAVE_NR, 1);
    undo_write_بائٹیں(bi, (long_u)buf->b_u_save_nr_آخری, 4);

    undo_write_بائٹیں(bi, 0, 1);  // end marker

    واپس OK;
}

    ساکن صحیح
serialize_uhp(bufinfo_T *bi, u_سرخی_T *uhp)
{
    صحیح		i;
    u_اندراج_T	*uep;
    char_u	وقت_buf[8];

    اگر (undo_write_بائٹیں(bi, (long_u)UF_سرخی_MAGIC, 2) == FAIL)
	واپس FAIL;

    put_سرخی_ptr(bi, uhp->uh_اگلا.ptr);
    put_سرخی_ptr(bi, uhp->uh_پچھلا.ptr);
    put_سرخی_ptr(bi, uhp->uh_alt_اگلا.ptr);
    put_سرخی_ptr(bi, uhp->uh_alt_پچھلا.ptr);
    undo_write_بائٹیں(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_بائٹیں(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_بائٹیں(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    برائے (i = 0; i < NMARKS; ++i)
	serialize_pos(bi, uhp->uh_namedm[i]);
    serialize_visualinfo(bi, &uhp->uh_visual);
    وقت_مقصد_بائٹیں(uhp->uh_وقت, وقت_buf);
    undo_write(bi, وقت_buf, 8);

    // Optional fields.
    undo_write_بائٹیں(bi, 4, 1);
    undo_write_بائٹیں(bi, UHP_SAVE_NR, 1);
    undo_write_بائٹیں(bi, (long_u)uhp->uh_save_nr, 4);

    undo_write_بائٹیں(bi, 0, 1);  // end marker

    // Write all the entries.
    برائے (uep = uhp->uh_اندراج; uep != تہی; uep = uep->ue_اگلا)
    {
	undo_write_بائٹیں(bi, (long_u)UF_اندراج_MAGIC, 2);
	اگر (serialize_uep(bi, uep) == FAIL)
	    واپس FAIL;
    }
    undo_write_بائٹیں(bi, (long_u)UF_اندراج_انتہا_MAGIC, 2);
    واپس OK;
}

    ساکن u_سرخی_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_سرخی_T	*uhp;
    صحیح		i;
    u_اندراج_T	*uep, *آخری_uep;
    صحیح		c;
    صحیح		error;

    uhp = U_ALLOC_سطر(حجم(u_سرخی_T));
    اگر (uhp == تہی)
	واپس تہی;
    CLEAR_اشارہ_گر(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_اگلا.seq = undo_read_4c(bi);
    uhp->uh_پچھلا.seq = undo_read_4c(bi);
    uhp->uh_alt_اگلا.seq = undo_read_4c(bi);
    uhp->uh_alt_پچھلا.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    اگر (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	واپس تہی;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    برائے (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_وقت = undo_read_وقت(bi);

    // Optional fields.
    برائے (;;)
    {
	صحیح len = undo_read_byte(bi);
	صحیح what;

	اگر (len == EOF)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    واپس تہی;
	}
	اگر (len == 0)
	    توڑو;
	what = undo_read_byte(bi);
	انتخاب (what)
	{
	    حالت UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		توڑو;
	    طےشدہ:
		// field not supported, skip
		جب (--len >= 0)
		    (خالی)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    آخری_uep = تہی;
    جب ((c = undo_read_2c(bi)) == UF_اندراج_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	اگر (آخری_uep == تہی)
	    uhp->uh_اندراج = uep;
	ورنہ
	    آخری_uep->ue_اگلا = uep;
	آخری_uep = uep;
	اگر (uep == تہی || error)
	{
	    u_free_uhp(uhp);
	    واپس تہی;
	}
    }
    اگر (c != UF_اندراج_انتہا_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	واپس تہی;
    }

    واپس uhp;
}

/*
 * Serialize "uep".
 */
    ساکن صحیح
serialize_uep(
    bufinfo_T	*bi,
    u_اندراج_T	*uep)
{
    صحیح		i;

    undo_write_بائٹیں(bi, (long_u)uep->ue_چوٹی, 4);
    undo_write_بائٹیں(bi, (long_u)uep->ue_bot, 4);
    undo_write_بائٹیں(bi, (long_u)uep->ue_lcount, 4);
    undo_write_بائٹیں(bi, (long_u)uep->ue_سائز, 4);
    برائے (i = 0; i < uep->ue_سائز; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	اگر (undo_write_بائٹیں(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    واپس FAIL;
	اگر (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_سطر, uep->ue_array[i].ul_textlen) == FAIL)
	    واپس FAIL;
    }
    واپس OK;
}

    ساکن u_اندراج_T *
unserialize_uep(bufinfo_T *bi, صحیح *error, char_u *file_name)
{
    صحیح		i;
    u_اندراج_T	*uep;
    undoline_T	*array = تہی;
    char_u	*سطر;
    صحیح		سطر_len;

    uep = U_ALLOC_سطر(حجم(u_اندراج_T));
    اگر (uep == تہی)
	واپس تہی;
    CLEAR_اشارہ_گر(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_چوٹی = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_سائز = undo_read_4c(bi);
    اگر (uep->ue_سائز > 0)
    {
	اگر (uep->ue_سائز < LONG_MAX / (طویل)حجم(undoline_T))
	    array = U_ALLOC_سطر(حجم(undoline_T) * uep->ue_سائز);
	اگر (array == تہی)
	{
	    *error = TRUE;
	    واپس uep;
	}
	vim_memset(array, 0, حجم(undoline_T) * uep->ue_سائز);
    }
    uep->ue_array = array;

    برائے (i = 0; i < uep->ue_سائز; ++i)
    {
	سطر_len = undo_read_4c(bi);
	اگر (سطر_len >= 0)
	    سطر = read_string_decrypt(bi, سطر_len);
	ورنہ
	{
	    سطر = تہی;
	    corruption_error("line length", file_name);
	}
	اگر (سطر == تہی)
	{
	    *error = TRUE;
	    واپس uep;
	}
	array[i].ul_سطر = سطر;
	array[i].ul_len = سطر_len + 1;
	array[i].ul_textlen = سطر_len;
    }
    واپس uep;
}

/*
 * Serialize "pos".
 */
    ساکن خالی
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_بائٹیں(bi, (long_u)pos.lnum, 4);
    undo_write_بائٹیں(bi, (long_u)pos.ستون, 4);
    undo_write_بائٹیں(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    ساکن خالی
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    اگر (pos->lnum < 0)
	pos->lnum = 0;
    pos->ستون = undo_read_4c(bi);
    اگر (pos->ستون < 0)
	pos->ستون = 0;
    pos->coladd = undo_read_4c(bi);
    اگر (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    ساکن خالی
serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    serialize_pos(bi, info->vi_آغاز);
    serialize_pos(bi, info->vi_انتہا);
    undo_write_بائٹیں(bi, (long_u)info->vi_منوال, 4);
    undo_write_بائٹیں(bi, (long_u)info->vi_curswant, 4);
}

/*
 * Unserialize the visualinfo_T at the current position.
 */
    ساکن خالی
unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    unserialize_pos(bi, &info->vi_آغاز);
    unserialize_pos(bi, &info->vi_انتہا);
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
    خالی
u_write_undo(
    char_u	*name,
    صحیح		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_سرخی_T	*uhp;
    char_u	*file_name;
    صحیح		mark;
# ifdef U_DEBUG
    صحیح		سرخیاں_written = 0;
# endif
    صحیح		fd;
    FILE	*fp = تہی;
    صحیح		perm;
    صحیح		write_ok = FALSE;
# ifdef UNIX
    صحیح		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_میدان(bi);

    اگر (name == تہی)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	اگر (file_name == تہی)
	{
	    اگر (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    واپس;
	}
    }
    ورنہ
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    اگر (buf->b_ffname != تہی)
    {
# ifdef UNIX
	اگر (mch_stat((حرف *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_منوال;
	    st_old_valid = TRUE;
	}
# ورنہ
	perm = mch_getperm(buf->b_ffname);
	اگر (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    اگر (mch_getperm(file_name) >= 0)
    {
	اگر (name == تہی || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((حرف *)file_name, O_RDONLY|O_EXTRA, 0);
	    اگر (fd < 0)
	    {
		اگر (name != تہی || p_verbose > 0)
		{
		    اگر (name == تہی)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    اگر (name == تہی)
			verbose_leave();
		}
		جاؤ theend;
	    }
	    ورنہ
	    {
		char_u	mbuf[UF_آغاز_MAGIC_LEN];
		صحیح	len;

		len = read_eintr(fd, mbuf, UF_آغاز_MAGIC_LEN);
		close(fd);
		اگر (len < UF_آغاز_MAGIC_LEN
		      || memcmp(mbuf, UF_آغاز_MAGIC, UF_آغاز_MAGIC_LEN) != 0)
		{
		    اگر (name != تہی || p_verbose > 0)
		    {
			اگر (name == تہی)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			اگر (name == تہی)
			    verbose_leave();
		    }
		    جاؤ theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    اگر (buf->b_u_numhead == 0 && buf->b_u_سطر_ptr.ul_سطر == تہی)
    {
	اگر (p_verbose > 0)
	    verb_پیغام(_("Skipping undo file write, nothing to undo"));
	جاؤ theend;
    }

    fd = mch_open((حرف *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    اگر (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	جاؤ theend;
    }
    (خالی)mch_setperm(file_name, perm);
    اگر (p_verbose > 0)
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
    اگر (st_old_valid
	    && mch_stat((حرف *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  اگر defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    اگر (buf->b_ffname != تہی)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    اگر (fp == تہی)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	close(fd);
	mch_remove(file_name);
	جاؤ theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    اگر (serialize_سرخی(&bi, hash) == FAIL)
	جاؤ write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    جب (uhp != تہی)
    {
	// Serialize current UHP if we haven't seen it
	اگر (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++سرخیاں_written;
# endif
	    اگر (serialize_uhp(&bi, uhp) == FAIL)
		جاؤ write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	اگر (uhp->uh_پچھلا.ptr != تہی && uhp->uh_پچھلا.ptr->uh_walk != mark)
	    uhp = uhp->uh_پچھلا.ptr;
	ورنہ اگر (uhp->uh_alt_اگلا.ptr != تہی
				     && uhp->uh_alt_اگلا.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_اگلا.ptr;
	ورنہ اگر (uhp->uh_اگلا.ptr != تہی && uhp->uh_alt_پچھلا.ptr == تہی
					 && uhp->uh_اگلا.ptr->uh_walk != mark)
	    uhp = uhp->uh_اگلا.ptr;
	ورنہ اگر (uhp->uh_alt_پچھلا.ptr != تہی)
	    uhp = uhp->uh_alt_پچھلا.ptr;
	ورنہ
	    uhp = uhp->uh_اگلا.ptr;
    }

    اگر (undo_write_بائٹیں(&bi, (long_u)UF_سرخی_انتہا_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    اگر (سرخیاں_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", سرخیاں_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    اگر (bi.bi_state != تہی && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# اگر defined(UNIX) && defined(HAVE_FSYNC)
    اگر ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && fflush(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    fclose(fp);
    اگر (!write_ok)
	semsg(_(e_write_error_اندر_undo_file_str), file_name);

# اگر defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    اگر (buf->b_ffname != تہی)
	(خالی)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    اگر (buf->b_ffname != تہی)
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
    اگر (bi.bi_state != تہی)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_بفر);
# endif
    اگر (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    ساکن صحیح
uhp_seq_cmp(ثابت خالی *v1, ثابت خالی *v2)
{
    ثابت u_سرخی_T *u1 = *(u_سرخی_T **)v1;
    ثابت u_سرخی_T *u2 = *(u_سرخی_T **)v2;

    واپس u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    ساکن طویل
uhp_جدول_ڈھونڈو(u_سرخی_T **uhp_جدول, طویل عدد_سر, طویل seq)
{
    طویل    lo = 0;
    طویل    hi = عدد_سر - 1;

    جب (lo <= hi)
    {
	طویل mid = lo + (hi - lo) / 2;

	اگر (uhp_جدول[mid]->uh_seq < seq)
	    lo = mid + 1;
	ورنہ اگر (uhp_جدول[mid]->uh_seq > seq)
	    hi = mid - 1;
	ورنہ
	    واپس mid;
    }
    واپس -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    خالی
u_read_undo(char_u *name, char_u *hash, char_u *orig_name غیر_مستعمل)
{
    char_u	*file_name;
    FILE	*fp;
    طویل	نسخہ_2, str_len;
    undoline_T	سطر_ptr;
    linenr_T	سطر_lnum;
    colnr_T	سطر_colnr;
    linenr_T	سطر_گنتی;
    طویل	عدد_سر = 0;
    طویل	old_سرخی_seq, new_سرخی_seq, cur_سرخی_seq;
    طویل	seq_آخری, seq_cur;
    طویل	آخری_save_nr = 0;
    طویل	old_idx = -1, new_idx = -1, cur_idx = -1;
    طویل	عدد_read_uhps = 0;
    time_t	seq_وقت;
    طویل	i;
    صحیح		c;
    u_سرخی_T	*uhp;
    u_سرخی_T	**uhp_جدول = تہی;
    char_u	read_hash[UNDO_HASH_سائز];
    char_u	magic_buf[UF_آغاز_MAGIC_LEN];
# ifdef U_DEBUG
    صحیح		*uhp_جدول_مستعمل;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_میدان(bi);
    سطر_ptr.ul_len = 0;
    سطر_ptr.ul_textlen = 0;
    سطر_ptr.ul_سطر = تہی;

    اگر (name == تہی)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	اگر (file_name == تہی)
	    واپس;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	اگر (mch_stat((حرف *)orig_name, &st_orig) >= 0
		&& mch_stat((حرف *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    اگر (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    واپس;
	}
# endif
    }
    ورنہ
	file_name = name;

    اگر (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((حرف *)file_name, "r");
    اگر (fp == تہی)
    {
	اگر (name != تہی || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	جاؤ error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    اگر (fread(magic_buf, UF_آغاز_MAGIC_LEN, 1, fp) != 1
		|| memcmp(magic_buf, UF_آغاز_MAGIC, UF_آغاز_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	جاؤ error;
    }
    نسخہ_2 = get2c(fp);
    اگر (نسخہ_2 == UF_نسخہ_CRYPT)
    {
# ifdef FEAT_CRYPT
	اگر (*curbuf->b_p_کلید == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    جاؤ error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_کلید);
	اگر (bi.bi_state == تہی)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    جاؤ error;
	}
	اگر (crypt_پورا_undofile(bi.bi_state->طریقہ_nr))
	{
	    bi.bi_بفر = alloc(CRYPT_BUF_سائز);
	    اگر (bi.bi_بفر == تہی)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = تہی;
		جاؤ error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_مستعمل = 0;
	}
# ورنہ
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	جاؤ error;
# endif
    }
    ورنہ اگر (نسخہ_2 != UF_نسخہ)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	جاؤ error;
    }

    اگر (undo_read(&bi, read_hash, (size_t)UNDO_HASH_سائز) == FAIL)
    {
	corruption_error("hash", file_name);
	جاؤ error;
    }
    سطر_گنتی = (linenr_T)undo_read_4c(&bi);
    اگر (memcmp(hash, read_hash, UNDO_HASH_سائز) != 0
				  || سطر_گنتی != curbuf->b_ml.ml_سطر_گنتی)
    {
	اگر (p_verbose > 0 || name != تہی)
	{
	    اگر (name == تہی)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    اگر (name == تہی)
		verbose_leave();
	}
	جاؤ error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    اگر (str_len < 0)
	جاؤ error;
    اگر (str_len > 0)
    {
	سطر_ptr.ul_سطر = read_string_decrypt(&bi, str_len);
	سطر_ptr.ul_len = str_len + 1;
	سطر_ptr.ul_textlen = str_len;
    }
    سطر_lnum = (linenr_T)undo_read_4c(&bi);
    سطر_colnr = (colnr_T)undo_read_4c(&bi);
    اگر (سطر_lnum < 0 || سطر_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	جاؤ error;
    }

    // Begin general undo data
    old_سرخی_seq = undo_read_4c(&bi);
    new_سرخی_seq = undo_read_4c(&bi);
    cur_سرخی_seq = undo_read_4c(&bi);
    عدد_سر = undo_read_4c(&bi);
    seq_آخری = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_وقت = undo_read_وقت(&bi);

    // Optional header fields.
    برائے (;;)
    {
	صحیح len = undo_read_byte(&bi);
	صحیح what;

	اگر (len == 0 || len == EOF)
	    توڑو;
	what = undo_read_byte(&bi);
	انتخاب (what)
	{
	    حالت UF_آخری_SAVE_NR:
		آخری_save_nr = undo_read_4c(&bi);
		توڑو;
	    طےشدہ:
		// field not supported, skip
		جب (--len >= 0)
		    (خالی)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    اگر (عدد_سر > 0)
    {
	اگر (عدد_سر < LONG_MAX / (طویل)حجم(u_سرخی_T *))
	    uhp_جدول = U_ALLOC_سطر(عدد_سر * حجم(u_سرخی_T *));
	اگر (uhp_جدول == تہی)
	    جاؤ error;
    }

    جب ((c = undo_read_2c(&bi)) == UF_سرخی_MAGIC)
    {
	اگر (عدد_read_uhps >= عدد_سر)
	{
	    corruption_error("num_head too small", file_name);
	    جاؤ error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	اگر (uhp == تہی)
	    جاؤ error;
	uhp_جدول[عدد_read_uhps++] = uhp;
    }

    اگر (عدد_read_uhps != عدد_سر)
    {
	corruption_error("num_head", file_name);
	جاؤ error;
    }
    اگر (c != UF_سرخی_انتہا_MAGIC)
    {
	corruption_error("end marker", file_name);
	جاؤ error;
    }

# ifdef U_DEBUG
    uhp_جدول_مستعمل = alloc_clear(حجم(صحیح) * عدد_سر + 1);
#  define سیٹ_FLAG(j) ++uhp_جدول_مستعمل[j]
# ورنہ
#  define سیٹ_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    اگر (عدد_سر > 0)
	qsort(uhp_جدول, (size_t)عدد_سر, حجم(u_سرخی_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    برائے (i = 0; i < عدد_سر - 1; i++)
    {
	اگر (uhp_جدول[i]->uh_seq == uhp_جدول[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    جاؤ error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    کرو { \
	طویل fidx = uhp_جدول_ڈھونڈو(uhp_جدول, عدد_سر, (link).seq); \
	\
	اگر (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_جدول[fidx]; \
	    سیٹ_FLAG(fidx); \
	} \
	ورنہ \
	    (link).ptr = تہی; \
    } جب (0)

    برائے (i = 0; i < عدد_سر; i++)
    {
	uhp = uhp_جدول[i];
	SWIZZLE_SEQ(uhp->uh_اگلا, i);
	SWIZZLE_SEQ(uhp->uh_پچھلا, i);
	SWIZZLE_SEQ(uhp->uh_alt_اگلا, i);
	SWIZZLE_SEQ(uhp->uh_alt_پچھلا, i);
	اگر (old_سرخی_seq > 0 && old_idx < 0 && uhp->uh_seq == old_سرخی_seq)
	{
	    old_idx = i;
	    سیٹ_FLAG(i);
	}
	اگر (new_سرخی_seq > 0 && new_idx < 0 && uhp->uh_seq == new_سرخی_seq)
	{
	    new_idx = i;
	    سیٹ_FLAG(i);
	}
	اگر (cur_سرخی_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_سرخی_seq)
	{
	    cur_idx = i;
	    سیٹ_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? تہی : uhp_جدول[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? تہی : uhp_جدول[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? تہی : uhp_جدول[cur_idx];
    curbuf->b_u_سطر_ptr = سطر_ptr;
    curbuf->b_u_سطر_lnum = سطر_lnum;
    curbuf->b_u_سطر_colnr = سطر_colnr;
    curbuf->b_u_numhead = عدد_سر;
    curbuf->b_u_seq_آخری = seq_آخری;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_وقت_cur = seq_وقت;
    curbuf->b_u_save_nr_آخری = آخری_save_nr;
    curbuf->b_u_save_nr_cur = آخری_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_جدول);

# ifdef U_DEBUG
    برائے (i = 0; i < عدد_سر; ++i)
	اگر (uhp_جدول_مستعمل[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_جدول_مستعمل);
    u_check(TRUE);
# endif

    اگر (name != تہی)
	smsg(_("Finished reading undo file %s"), file_name);
    جاؤ theend;

error:
    vim_free(سطر_ptr.ul_سطر);
    اگر (uhp_جدول != تہی)
    {
	برائے (i = 0; i < عدد_read_uhps; i++)
	    اگر (uhp_جدول[i] != تہی)
		u_free_uhp(uhp_جدول[i]);
	vim_free(uhp_جدول);
    }

theend:
# ifdef FEAT_CRYPT
    اگر (bi.bi_state != تہی)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_بفر);
# endif
    اگر (fp != تہی)
	fclose(fp);
    اگر (file_name != name)
	vim_free(file_name);
    واپس;
}

#اگر_ختم // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    خالی
u_undo(صحیح گنتی)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    اگر (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	گنتی = 1;
    }

    اگر (vim_strchr(p_cpo, CPO_UNDO) == تہی)
	undo_undoes = TRUE;
    ورنہ
	undo_undoes = !undo_undoes;
    u_doit(گنتی);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    خالی
u_redo(صحیح گنتی)
{
    اگر (vim_strchr(p_cpo, CPO_UNDO) == تہی)
	undo_undoes = FALSE;
    u_doit(گنتی);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    ساکن خالی
u_doit(صحیح startcount)
{
    صحیح گنتی = startcount;

    اگر (!undo_allowed())
	واپس;

    u_newcount = 0;
    u_oldcount = 0;
    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)
	u_oldcount = -1;
    جب (گنتی--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	اگر (undo_undoes)
	{
	    اگر (curbuf->b_u_curhead == تہی)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    ورنہ اگر (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_اگلا.ptr;
	    // nothing to undo
	    اگر (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == تہی)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		اگر (گنتی == startcount - 1)
		{
		    اگر (!shortmess(SHM_UNDO))
			پیغام(_("Already at oldest change"));
		    واپس;
		}
		توڑو;
	    }

	    u_undoredo(TRUE);
	}
	ورنہ
	{
	    اگر (curbuf->b_u_curhead == تہی || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		اگر (گنتی == startcount - 1)
		{
		    اگر (!shortmess(SHM_UNDO))
			پیغام(_("Already at newest change"));
		    واپس;
		}
		توڑو;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    اگر (curbuf->b_u_curhead->uh_پچھلا.ptr == تہی)
		curbuf->b_u_newhead = curbuf->b_u_curhead;
	    curbuf->b_u_curhead = curbuf->b_u_curhead->uh_پچھلا.ptr;
	}
    }
    u_undo_انتہا(undo_undoes, FALSE);
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
    خالی
undo_وقت(
    طویل	قدم,
    صحیح		sec,
    صحیح		file,
    صحیح		absolute)
{
    طویل	    ہدف;
    طویل	    closest;
    طویل	    closest_آغاز;
    طویل	    closest_seq = 0;
    طویل	    val;
    u_سرخی_T	    *uhp = تہی;
    u_سرخی_T	    *آخری;
    صحیح		    mark;
    صحیح		    nomark = 0;  // shut up compiler
    صحیح		    باری;
    صحیح		    dosec = sec;
    صحیح		    dofile = file;
    صحیح		    above = FALSE;
    صحیح		    did_undo = TRUE;

    اگر (متن_locked())
    {
	متن_locked_پیغام();
	واپس;
    }

    // First make sure the current undoable change is synced.
    اگر (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    اگر (absolute)
    {
	ہدف = قدم;
	closest = -1;
    }
    ورنہ
    {
	اگر (dosec)
	    ہدف = (طویل)(curbuf->b_u_وقت_cur) + قدم;
	ورنہ اگر (dofile)
	{
	    اگر (قدم < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		اگر (uhp != تہی)
		    uhp = uhp->uh_اگلا.ptr;
		ورنہ
		    uhp = curbuf->b_u_newhead;
		اگر (uhp != تہی && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    ہدف = curbuf->b_u_save_nr_cur + قدم;
		ورنہ
		    // count the changes since the last write as one step
		    ہدف = curbuf->b_u_save_nr_cur + قدم + 1;
		اگر (ہدف <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    ورنہ
	    {
		// Moving forward to a newer write.
		ہدف = curbuf->b_u_save_nr_cur + قدم;
		اگر (ہدف > curbuf->b_u_save_nr_آخری)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    ہدف = curbuf->b_u_seq_آخری + 1;
		    dofile = FALSE;
		}
	    }
	}
	ورنہ
	    ہدف = curbuf->b_u_seq_cur + قدم;
	اگر (قدم < 0)
	{
	    اگر (ہدف < 0)
		ہدف = 0;
	    closest = -1;
	}
	ورنہ
	{
	    اگر (dosec)
		closest = (طویل)(vim_وقت() + 1);
	    ورنہ اگر (dofile)
		closest = curbuf->b_u_save_nr_آخری + 2;
	    ورنہ
		closest = curbuf->b_u_seq_آخری + 2;
	    اگر (ہدف >= closest)
		ہدف = closest - 1;
	}
    }
    closest_آغاز = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    اگر (ہدف == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	جاؤ ہدف_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    برائے (باری = 1; باری <= 2; ++باری)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	اگر (curbuf->b_u_curhead == تہی)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	ورنہ
	    uhp = curbuf->b_u_curhead;

	جب (uhp != تہی)
	{
	    uhp->uh_walk = mark;
	    اگر (dosec)
		val = (طویل)(uhp->uh_وقت);
	    ورنہ اگر (dofile)
		val = uhp->uh_save_nr;
	    ورنہ
		val = uhp->uh_seq;

	    اگر (باری == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		اگر ((قدم < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
			      : uhp->uh_seq > curbuf->b_u_seq_cur)
			&& ((dosec && val == closest)
			    ? (قدم < 0
				? uhp->uh_seq < closest_seq
				: uhp->uh_seq > closest_seq)
			    : closest == closest_آغاز
				|| (val > ہدف
				    ? (closest > ہدف
					? val - ہدف <= closest - ہدف
					: val - ہدف <= ہدف - closest)
				    : (closest > ہدف
					? ہدف - val <= closest - ہدف
					: ہدف - val <= ہدف - closest))))
		{
		    closest = val;
		    closest_seq = uhp->uh_seq;
		}
	    }

	    // Quit searching when we found a match.  But when searching for a
	    // time we need to continue looking for the best uh_seq.
	    اگر (ہدف == val && !dosec)
	    {
		ہدف = uhp->uh_seq;
		توڑو;
	    }

	    // go down in the tree if we haven't been there
	    اگر (uhp->uh_پچھلا.ptr != تہی && uhp->uh_پچھلا.ptr->uh_walk != nomark
					 && uhp->uh_پچھلا.ptr->uh_walk != mark)
		uhp = uhp->uh_پچھلا.ptr;

	    // go to alternate branch if we haven't been there
	    ورنہ اگر (uhp->uh_alt_اگلا.ptr != تہی
		    && uhp->uh_alt_اگلا.ptr->uh_walk != nomark
		    && uhp->uh_alt_اگلا.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_اگلا.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    ورنہ اگر (uhp->uh_اگلا.ptr != تہی && uhp->uh_alt_پچھلا.ptr == تہی
		    && uhp->uh_اگلا.ptr->uh_walk != nomark
		    && uhp->uh_اگلا.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		اگر (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_اگلا.ptr;
	    }

	    ورنہ
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		اگر (uhp->uh_alt_پچھلا.ptr != تہی)
		    uhp = uhp->uh_alt_پچھلا.ptr;
		ورنہ
		    uhp = uhp->uh_اگلا.ptr;
	    }
	}

	اگر (uhp != تہی)    // found it
	    توڑو;

	اگر (absolute)
	{
	    semsg(_(e_undo_نمبر_nr_not_ملا), قدم);
	    واپس;
	}

	اگر (closest == closest_آغاز)
	{
	    اگر (!shortmess(SHM_UNDO))
	    {
		اگر (قدم < 0)
		    پیغام(_("Already at oldest change"));
		ورنہ
		    پیغام(_("Already at newest change"));
	    }
	    واپس;
	}

	ہدف = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	اگر (قدم < 0)
	    above = TRUE;	// stop above the header
    }

ہدف_zero:
    // If we found it: Follow the path to go to where we want to be.
    اگر (uhp != تہی || ہدف == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	جب (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    اگر (uhp == تہی)
		uhp = curbuf->b_u_newhead;
	    ورنہ
		uhp = uhp->uh_اگلا.ptr;
	    اگر (uhp == تہی || (ہدف > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == ہدف && !above))
		توڑو;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    اگر (ہدف > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	اگر (ہدف > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    جب (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		اگر (uhp == تہی)
		    توڑو;

		// Go back to the first branch with a mark.
		جب (uhp->uh_alt_پچھلا.ptr != تہی
				     && uhp->uh_alt_پچھلا.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_پچھلا.ptr;

		// Find the last branch with a mark, that's the one.
		آخری = uhp;
		جب (آخری->uh_alt_اگلا.ptr != تہی
				    && آخری->uh_alt_اگلا.ptr->uh_walk == mark)
		    آخری = آخری->uh_alt_اگلا.ptr;
		اگر (آخری != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    جب (uhp->uh_alt_پچھلا.ptr != تہی)
			uhp = uhp->uh_alt_پچھلا.ptr;
		    اگر (آخری->uh_alt_اگلا.ptr != تہی)
			آخری->uh_alt_اگلا.ptr->uh_alt_پچھلا.ptr =
							آخری->uh_alt_پچھلا.ptr;
		    آخری->uh_alt_پچھلا.ptr->uh_alt_اگلا.ptr =
							آخری->uh_alt_اگلا.ptr;
		    آخری->uh_alt_پچھلا.ptr = تہی;
		    آخری->uh_alt_اگلا.ptr = uhp;
		    uhp->uh_alt_پچھلا.ptr = آخری;

		    اگر (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = آخری;
		    uhp = آخری;
		    اگر (uhp->uh_اگلا.ptr != تہی)
			uhp->uh_اگلا.ptr->uh_پچھلا.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		اگر (uhp->uh_walk != mark)
		    توڑو;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		اگر (uhp->uh_seq == ہدف && above)
		{
		    curbuf->b_u_seq_cur = ہدف - 1;
		    توڑو;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		اگر (uhp->uh_پچھلا.ptr == تہی)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_پچھلا.ptr;
		did_undo = FALSE;

		اگر (uhp->uh_seq == ہدف)	// found it!
		    توڑو;

		uhp = uhp->uh_پچھلا.ptr;
		اگر (uhp == تہی || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    اندرونی_error("undo_time()");
		    توڑو;
		}
	    }
	}
    }
    u_undo_انتہا(did_undo, absolute);
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
    ساکن خالی
u_undoredo(صحیح undo)
{
    undoline_T	*newarray = تہی;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	چوٹی, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    طویل	i;
    u_اندراج_T	*uep, *nuep;
    u_اندراج_T	*newlist = تہی;
    صحیح		old_flags;
    صحیح		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    صحیح		خالی_خانے_بفر;		    // buffer became empty
    u_سرخی_T	*curhead = curbuf->b_u_curhead;

    // Don't want autocommands using the undo structures here, they are
    // invalid till the end.
    بلاک_autocmds();

#اگر_تعریف_شدہ U_DEBUG
    u_check(FALSE);
#اگر_ختم
    old_flags = curhead->uh_flags;
    new_flags = (curbuf->b_بدلا ? UH_بدلا : 0) +
	       ((curbuf->b_ml.ml_flags & ML_خالی_خانے) ? UH_EMPTYBUF : 0);
    setpcmark();

    /*
     * save marks before undo/redo
     */
    mch_memmove(namedm, curbuf->b_namedm, حجم(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_آغاز.lnum = curbuf->b_ml.ml_سطر_گنتی;
    curbuf->b_op_آغاز.ستون = 0;
    curbuf->b_op_انتہا.lnum = 0;
    curbuf->b_op_انتہا.ستون = 0;

    برائے (uep = curhead->uh_اندراج; uep != تہی; uep = nuep)
    {
	چوٹی = uep->ue_چوٹی;
	bot = uep->ue_bot;
	اگر (bot == 0)
	    bot = curbuf->b_ml.ml_سطر_گنتی + 1;
	اگر (چوٹی > curbuf->b_ml.ml_سطر_گنتی || چوٹی >= bot
				      || bot > curbuf->b_ml.ml_سطر_گنتی + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_سطر_numbers_wrong);
	    بدلا();		// don't want UNCHANGED now
	    واپس;
	}

	oldsize = bot - چوٹی - 1;    // number of lines before undo
	newsize = uep->ue_سائز;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    اگر (lnum >= چوٹی && lnum <= چوٹی + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    ورنہ اگر (چوٹی < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		برائے (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(چوٹی + 1 + i);

		    اگر (curbuf->b_ml.ml_سطر_len != uep->ue_array[i].ul_len
			    || memcmp(uep->ue_array[i].ul_سطر, p,
						curbuf->b_ml.ml_سطر_len) != 0)
			توڑو;
		}
		اگر (i == newsize && newlnum == MAXLNUM && uep->ue_اگلا == تہی)
		{
		    newlnum = چوٹی;
		    new_curpos.lnum = newlnum + 1;
		}
		ورنہ اگر (i < newsize)
		{
		    newlnum = چوٹی + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	خالی_خانے_بفر = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	اگر (oldsize > 0)
	{
	    اگر ((newarray = U_ALLOC_سطر(حجم(undoline_T) * oldsize)) == تہی)
	    {
		do_outofmem_پیغام((long_u)(حجم(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		جب (uep != تہی)
		{
		    nuep = uep->ue_اگلا;
		    u_freeentry(uep, uep->ue_سائز);
		    uep = nuep;
		}
		توڑو;
	    }
	    // delete backwards, it goes faster in most cases
	    برائے (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		اگر (u_save_سطر(&newarray[i], lnum) == FAIL)
		    do_outofmem_پیغام((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		اگر (curbuf->b_ml.ml_سطر_گنتی == 1)
		    خالی_خانے_بفر = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	ورنہ
	    newarray = تہی;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	اگر (newsize)
	{
	    برائے (lnum = چوٹی, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		اگر (خالی_خانے_بفر && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_سطر,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		ورنہ
		    ml_append_flags(lnum, uep->ue_array[i].ul_سطر,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_سطر);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#اگر_تعریف_شدہ FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, چوٹی, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, چوٹی + newsize + 1, &uep->ue_props,
								UNDOPROP_نیچے);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, چوٹی, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_نیچے);
	اگر (newsize > 0)
	    adjust_props_for_undo(curbuf, چوٹی + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_نیچے);
#اگر_ختم

	// adjust marks
	اگر (oldsize != newsize)
	{
	    mark_adjust(چوٹی + 1, چوٹی + oldsize, (طویل)MAXLNUM,
					       (طویل)newsize - (طویل)oldsize);
	    اگر (curbuf->b_op_آغاز.lnum > چوٹی + oldsize)
		curbuf->b_op_آغاز.lnum += newsize - oldsize;
	    اگر (curbuf->b_op_انتہا.lnum > چوٹی + oldsize)
		curbuf->b_op_انتہا.lnum += newsize - oldsize;
	}
	اگر (oldsize > 0 || newsize > 0)
	{
	    بدلا_سطریں(چوٹی + 1, 0, bot, newsize - oldsize);
#اگر_تعریف_شدہ FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    اگر (spell_check_کھڑکی(curwin) && bot <= curbuf->b_ml.ml_سطر_گنتی)
		redrawWinline(curwin, bot);
#اگر_ختم
	}

	// Set the '[ mark.
	اگر (چوٹی + 1 < curbuf->b_op_آغاز.lnum)
	    curbuf->b_op_آغاز.lnum = چوٹی + 1;
	// Set the '] mark.
	اگر (newsize == 0 && چوٹی + 1 > curbuf->b_op_انتہا.lnum)
	    curbuf->b_op_انتہا.lnum = چوٹی + 1;
	ورنہ اگر (چوٹی + newsize > curbuf->b_op_انتہا.lnum)
	    curbuf->b_op_انتہا.lnum = چوٹی + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_سائز = oldsize;
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
    اگر (curbuf->b_op_آغاز.lnum > curbuf->b_ml.ml_سطر_گنتی)
	 curbuf->b_op_آغاز.lnum = curbuf->b_ml.ml_سطر_گنتی;
    اگر (curbuf->b_op_انتہا.lnum > curbuf->b_ml.ml_سطر_گنتی)
	 curbuf->b_op_انتہا.lnum = curbuf->b_ml.ml_سطر_گنتی;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_اندراج = newlist;
    curhead->uh_flags = new_flags;
    اگر ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_خالی_خانے;
    اگر (old_flags & UH_بدلا)
	بدلا();
    ورنہ
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	اگر (!isNetbeansModified(curbuf))
#اگر_ختم
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    برائے (i = 0; i < NMARKS; ++i)
    {
	اگر (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	اگر (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	ورنہ
	    curhead->uh_namedm[i].lnum = 0;
    }
    اگر (curhead->uh_visual.vi_آغاز.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    اگر (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    اگر (curwin->w_cursor.lnum <= curbuf->b_ml.ml_سطر_گنتی)
    {
	اگر (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.ستون = curhead->uh_cursor.ستون;
	    اگر (virtual_فعال() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    ورنہ
		curwin->w_cursor.coladd = 0;
	}
	ورنہ
	    beginline(BL_SOL | BL_FIX);
    }
    ورنہ
    {
	// We get here with the current cursor line being past the end (eg
	// after adding lines at the end of the file, and then undoing it).
	// check_cursor() will move the cursor to the last line.  Move it to
	// the first column here.
	curwin->w_cursor.ستون = 0;
	curwin->w_cursor.coladd = 0;
    }

    // Make sure the cursor is on an existing line and column.
    check_cursor();

    // Remember where we are for "g-" and ":earlier 10s".
    curbuf->b_u_seq_cur = curhead->uh_seq;
    اگر (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	اگر (curhead->uh_اگلا.ptr != تہی)
	    curbuf->b_u_seq_cur = curhead->uh_اگلا.ptr->uh_seq;
	ورنہ
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    اگر (curhead->uh_save_nr != 0)
    {
	اگر (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	ورنہ
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr;
    }

    // The timestamp can be the same for multiple changes, just use the one of
    // the undone/redone change.
    curbuf->b_u_وقت_cur = curhead->uh_وقت;

    unblock_autocmds();
#اگر_تعریف_شدہ U_DEBUG
    u_check(FALSE);
#اگر_ختم
}

/*
 * If we deleted or added lines, report the number of less/more lines.
 * Otherwise, report the number of changes (this may be incorrect
 * in some cases, but it's better than nothing).
 */
    ساکن خالی
u_undo_انتہا(
    صحیح		did_undo,	// just did an undo
    صحیح		absolute)	// used ":undo N"
{
    حرف	*msgstr;
    u_سرخی_T	*uhp;
    char_u	msgbuf[80];

#اگر_تعریف_شدہ FEAT_FOLDING
    اگر ((fdo_flags & FDO_UNDO) && کلید_Typed)
	foldOpenCursor();
#اگر_ختم

    اگر (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	واپس;

    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)
	--u_newcount;

    u_oldcount -= u_newcount;
    اگر (u_oldcount == -1)
	msgstr = N_("more line");
    ورنہ اگر (u_oldcount < 0)
	msgstr = N_("more lines");
    ورنہ اگر (u_oldcount == 1)
	msgstr = N_("line less");
    ورنہ اگر (u_oldcount > 1)
	msgstr = N_("fewer lines");
    ورنہ
    {
	u_oldcount = u_newcount;
	اگر (u_newcount == 1)
	    msgstr = N_("change");
	ورنہ
	    msgstr = N_("changes");
    }

    اگر (curbuf->b_u_curhead != تہی)
    {
	// For ":undo N" we prefer a "after #N" message.
	اگر (absolute && curbuf->b_u_curhead->uh_اگلا.ptr != تہی)
	{
	    uhp = curbuf->b_u_curhead->uh_اگلا.ptr;
	    did_undo = FALSE;
	}
	ورنہ اگر (did_undo)
	    uhp = curbuf->b_u_curhead;
	ورنہ
	    uhp = curbuf->b_u_curhead->uh_اگلا.ptr;
    }
    ورنہ
	uhp = curbuf->b_u_newhead;

    اگر (uhp == تہی)
	*msgbuf = NUL;
    ورنہ
	جوڑو_وقت(msgbuf, حجم(msgbuf), uhp->uh_وقت);

#اگر_تعریف_شدہ FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp->w_بفر == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#اگر_ختم
    اگر (VIsual_فعال)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == تہی ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    خالی
u_sync(
    صحیح	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    اگر (curbuf->b_u_synced || (!force && no_u_sync > 0))
	واپس;
#اگر defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    اگر (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	واپس;		    // XIM is busy, don't break an undo sequence
#اگر_ختم
    اگر (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    ورنہ
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = تہی;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    خالی
ex_undolist(exarg_T *eap غیر_مستعمل)
{
    garray_T	ga;
    u_سرخی_T	*uhp;
    صحیح		mark;
    صحیح		nomark;
    صحیح		changes = 1;
    صحیح		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, حجم(حرف *), 20);

    uhp = curbuf->b_u_oldhead;
    جب (uhp != تہی)
    {
	اگر (uhp->uh_پچھلا.ptr == تہی && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    اگر (ga_grow(&ga, 1) == FAIL)
		توڑو;
	    len = vim_snprintf((حرف *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    جوڑو_وقت(IObuff + len, IOSIZE - len, uhp->uh_وقت);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (صحیح)STRLEN(IObuff + len);
	    اگر (uhp->uh_save_nr > 0)
	    {
		صحیح n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((حرف *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_ڈیٹا))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	اگر (uhp->uh_پچھلا.ptr != تہی && uhp->uh_پچھلا.ptr->uh_walk != nomark
					 && uhp->uh_پچھلا.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_پچھلا.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	ورنہ اگر (uhp->uh_alt_اگلا.ptr != تہی
		&& uhp->uh_alt_اگلا.ptr->uh_walk != nomark
		&& uhp->uh_alt_اگلا.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_اگلا.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	ورنہ اگر (uhp->uh_اگلا.ptr != تہی && uhp->uh_alt_پچھلا.ptr == تہی
		&& uhp->uh_اگلا.ptr->uh_walk != nomark
		&& uhp->uh_اگلا.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_اگلا.ptr;
	    --changes;
	}

	ورنہ
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    اگر (uhp->uh_alt_پچھلا.ptr != تہی)
		uhp = uhp->uh_alt_پچھلا.ptr;
	    ورنہ
	    {
		uhp = uhp->uh_اگلا.ptr;
		--changes;
	    }
	}
    }

    اگر (ga.ga_len == 0)
	پیغام(_("Nothing to undo"));
    ورنہ
    {
	صحیح		i;

	ترتیب_دو_لڑیاں((char_u **)ga.ga_ڈیٹا, ga.ga_len);

	پیغام_آغاز();
	پیغام_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	برائے (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    پیغام_putchar('\n');
	    اگر (got_int)
		توڑو;
	    پیغام_puts(((حرف **)ga.ga_ڈیٹا)[i]);
	}
	پیغام_انتہا();

	ga_clear_لڑیاں(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    خالی
ex_undojoin(exarg_T *eap غیر_مستعمل)
{
    اگر (curbuf->b_u_newhead == تہی)
	واپس;		    // nothing changed before
    اگر (curbuf->b_u_curhead != تہی)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	واپس;
    }
    اگر (!curbuf->b_u_synced)
	واپس;		    // already unsynced
    اگر (get_undolevel() < 0)
	واپس;		    // no entries, nothing to do
    ورنہ
	// Append next change to the last entry
	curbuf->b_u_synced = false;
}

/*
 * Called after writing or reloading the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
    خالی
u_unchanged(buf_T *buf)
{
    u_unch_branch(buf->b_u_oldhead);
    buf->b_did_warn = false;
}

/*
 * After reloading a buffer which was saved for 'undoreload': Find the first
 * line that was changed and set the cursor there.
 */
    خالی
u_ڈھونڈو_پہلا_بدلا(خالی)
{
    u_سرخی_T	*uhp = curbuf->b_u_newhead;
    u_اندراج_T   *uep;
    linenr_T	lnum;

    اگر (curbuf->b_u_curhead != تہی || uhp == تہی)
	واپس;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_اندراج;
    اگر (uep->ue_چوٹی != 0 || uep->ue_bot != 0)
	واپس;

    برائے (lnum = 1; lnum < curbuf->b_ml.ml_سطر_گنتی
					      && lnum <= uep->ue_سائز; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	اگر (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_سطر_len
		|| memcmp(p, uep->ue_array[lnum - 1].ul_سطر, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    واپس;
	}
    }
    اگر (curbuf->b_ml.ml_سطر_گنتی != uep->ue_سائز)
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
    خالی
u_update_save_nr(buf_T *buf)
{
    u_سرخی_T	*uhp;

    ++buf->b_u_save_nr_آخری;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_آخری;
    uhp = buf->b_u_curhead;
    اگر (uhp != تہی)
	uhp = uhp->uh_اگلا.ptr;
    ورنہ
	uhp = buf->b_u_newhead;
    اگر (uhp != تہی)
	uhp->uh_save_nr = buf->b_u_save_nr_آخری;
}

    ساکن خالی
u_unch_branch(u_سرخی_T *uhp)
{
    u_سرخی_T	*uh;

    برائے (uh = uhp; uh != تہی; uh = uh->uh_پچھلا.ptr)
    {
	uh->uh_flags |= UH_بدلا;
	اگر (uh->uh_alt_اگلا.ptr != تہی)
	    u_unch_branch(uh->uh_alt_اگلا.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    ساکن u_اندراج_T *
u_get_headentry(خالی)
{
    اگر (curbuf->b_u_newhead == تہی || curbuf->b_u_newhead->uh_اندراج == تہی)
    {
	iemsg(e_undo_list_corrupt);
	واپس تہی;
    }
    واپس curbuf->b_u_newhead->uh_اندراج;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    ساکن خالی
u_getbot(خالی)
{
    u_اندراج_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    اگر (uep == تہی)
	واپس;

    uep = curbuf->b_u_newhead->uh_getbot_اندراج;
    اگر (uep != تہی)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_سطر_گنتی - uep->ue_lcount;
	uep->ue_bot = uep->ue_چوٹی + uep->ue_سائز + 1 + extra;
	اگر (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_سطر_گنتی)
	{
	    iemsg(e_undo_سطر_missing);
	    uep->ue_bot = uep->ue_چوٹی + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_اندراج = تہی;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    ساکن خالی
u_freeheader(
    buf_T	    *buf,
    u_سرخی_T	    *uhp,
    u_سرخی_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_سرخی_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    اگر (uhp->uh_alt_اگلا.ptr != تہی)
	u_freebranch(buf, uhp->uh_alt_اگلا.ptr, uhpp);

    اگر (uhp->uh_alt_پچھلا.ptr != تہی)
	uhp->uh_alt_پچھلا.ptr->uh_alt_اگلا.ptr = تہی;

    // Update the links in the list to remove the header.
    اگر (uhp->uh_اگلا.ptr == تہی)
	buf->b_u_oldhead = uhp->uh_پچھلا.ptr;
    ورنہ
	uhp->uh_اگلا.ptr->uh_پچھلا.ptr = uhp->uh_پچھلا.ptr;

    اگر (uhp->uh_پچھلا.ptr == تہی)
	buf->b_u_newhead = uhp->uh_اگلا.ptr;
    ورنہ
	برائے (uhap = uhp->uh_پچھلا.ptr; uhap != تہی;
						 uhap = uhap->uh_alt_اگلا.ptr)
	    uhap->uh_اگلا.ptr = uhp->uh_اگلا.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    ساکن خالی
u_freebranch(
    buf_T	    *buf,
    u_سرخی_T	    *uhp,
    u_سرخی_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_سرخی_T	    *tofree, *اگلا;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    اگر (uhp == buf->b_u_oldhead)
    {
	جب (buf->b_u_oldhead != تہی)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	واپس;
    }

    اگر (uhp->uh_alt_پچھلا.ptr != تہی)
	uhp->uh_alt_پچھلا.ptr->uh_alt_اگلا.ptr = تہی;

    اگلا = uhp;
    جب (اگلا != تہی)
    {
	tofree = اگلا;
	اگر (tofree->uh_alt_اگلا.ptr != تہی)
	    u_freebranch(buf, tofree->uh_alt_اگلا.ptr, uhpp);   // recursive
	اگلا = tofree->uh_پچھلا.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}

/*
 * Free all the undo entries for one header and the header itself.
 * This means that "uhp" is invalid when returning.
 */
    ساکن خالی
u_freeentries(
    buf_T	    *buf,
    u_سرخی_T	    *uhp,
    u_سرخی_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_اندراج_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    اگر (buf->b_u_curhead == uhp)
	buf->b_u_curhead = تہی;
    اگر (buf->b_u_newhead == uhp)
	buf->b_u_newhead = تہی;  // freeing the newest entry
    اگر (uhpp != تہی && uhp == *uhpp)
	*uhpp = تہی;

    برائے (uep = uhp->uh_اندراج; uep != تہی; uep = nuep)
    {
	nuep = uep->ue_اگلا;
	u_freeentry(uep, uep->ue_سائز);
    }

#اگر_تعریف_شدہ U_DEBUG
    uhp->uh_magic = 0;
#اگر_ختم
    vim_free((char_u *)uhp);
    --buf->b_u_numhead;
}

/*
 * free entry 'uep' and 'n' lines in uep->ue_array[]
 */
    ساکن خالی
u_freeentry(u_اندراج_T *uep, طویل n)
{
    جب (n > 0)
	vim_free(uep->ue_array[--n].ul_سطر);
    vim_free((char_u *)uep->ue_array);
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    ga_clear(&uep->ue_props);
#اگر_ختم
#اگر_تعریف_شدہ U_DEBUG
    uep->ue_magic = 0;
#اگر_ختم
    vim_free((char_u *)uep);
}

/*
 * invalidate the undo buffer; called when storage has already been released
 */
    ساکن خالی
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = تہی;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_سطر_ptr.ul_سطر = تہی;
    buf->b_u_سطر_ptr.ul_len = 0;
    buf->b_u_سطر_ptr.ul_textlen = 0;
    buf->b_u_سطر_lnum = 0;
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 */
    ساکن خالی
u_blockfree(buf_T *buf)
{
    جب (buf->b_u_oldhead != تہی)
	u_freeheader(buf, buf->b_u_oldhead, تہی);
    vim_free(buf->b_u_سطر_ptr.ul_سطر);
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 * and invalidate the undo buffer
 */
    خالی
u_clearallandblockfree(buf_T *buf)
{
    u_blockfree(buf);
    u_clearall(buf);
}



/*
 * Save the line "lnum" for the "U" command.
 */
    ساکن خالی
u_saveline(linenr_T lnum)
{
    اگر (lnum == curbuf->b_u_سطر_lnum)	    // line is already saved
	واپس;
    اگر (lnum < 1 || lnum > curbuf->b_ml.ml_سطر_گنتی) // should never happen
	واپس;
    u_clearline();
    curbuf->b_u_سطر_lnum = lnum;
    اگر (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_سطر_colnr = curwin->w_cursor.ستون;
    ورنہ
	curbuf->b_u_سطر_colnr = 0;
    اگر (u_save_سطر(&curbuf->b_u_سطر_ptr, lnum) == FAIL)
	do_outofmem_پیغام((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    خالی
u_clearline(خالی)
{
    اگر (curbuf->b_u_سطر_ptr.ul_سطر == تہی)
	واپس;

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
    خالی
u_undoline(خالی)
{
    colnr_T	t;
    undoline_T  oldp;

    اگر (undo_off)
	واپس;

    اگر (curbuf->b_u_سطر_ptr.ul_سطر == تہی
			|| curbuf->b_u_سطر_lnum > curbuf->b_ml.ml_سطر_گنتی)
    {
	beep_flush();
	واپس;
    }

    // first save the line for the 'u' command
    اگر (u_savecommon(curbuf->b_u_سطر_lnum - 1,
		       curbuf->b_u_سطر_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	واپس;
    اگر (u_save_سطر(&oldp, curbuf->b_u_سطر_lnum) == FAIL)
    {
	do_outofmem_پیغام((long_u)0);
	واپس;
    }
    ml_replace_len(curbuf->b_u_سطر_lnum, curbuf->b_u_سطر_ptr.ul_سطر,
				     curbuf->b_u_سطر_ptr.ul_len, TRUE, FALSE);
    بدلا_بائٹیں(curbuf->b_u_سطر_lnum, 0);
    curbuf->b_u_سطر_ptr = oldp;

    t = curbuf->b_u_سطر_colnr;
    اگر (curwin->w_cursor.lnum == curbuf->b_u_سطر_lnum)
	curbuf->b_u_سطر_colnr = curwin->w_cursor.ستون;
    curwin->w_cursor.ستون = t;
    curwin->w_cursor.lnum = curbuf->b_u_سطر_lnum;
    check_cursor_ستون();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    صحیح
buf_Is_بدلا(buf_T *buf)
{
#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (رکن_کام_running_not_none(buf->b_رکن))
	واپس TRUE;
#اگر_ختم
    واپس buf_Is_بدلا_Not_رکن(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    صحیح
any_Buf_Is_بدلا(خالی)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	اگر (buf_Is_بدلا(buf))
	    واپس TRUE;
    واپس FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    صحیح
buf_Is_بدلا_Not_رکن(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    واپس (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_بدلا || file_ff_differs(buf, TRUE));
}

    صحیح
curbuf_Is_بدلا(خالی)
{
    واپس buf_Is_بدلا(curbuf);
}

#اگر defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    ساکن خالی
u_eval_درخت(buf_T *buf, u_سرخی_T *پہلا_uhp, list_T *list)
{
    u_سرخی_T  *uhp = پہلا_uhp;
    dict_T	*dict;

    جب (uhp != تہی)
    {
	dict = dict_alloc();
	اگر (dict == تہی)
	    واپس;
	dict_جوڑو_نمبر(dict, "seq", uhp->uh_seq);
	dict_جوڑو_نمبر(dict, "time", (طویل)uhp->uh_وقت);
	اگر (uhp == buf->b_u_newhead)
	    dict_جوڑو_نمبر(dict, "newhead", 1);
	اگر (uhp == buf->b_u_curhead)
	    dict_جوڑو_نمبر(dict, "curhead", 1);
	اگر (uhp->uh_save_nr > 0)
	    dict_جوڑو_نمبر(dict, "save", uhp->uh_save_nr);

	اگر (uhp->uh_alt_اگلا.ptr != تہی)
	{
	    list_T	*alt_list = list_alloc();

	    اگر (alt_list != تہی)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_درخت(buf, uhp->uh_alt_اگلا.ptr, alt_list);
		dict_جوڑو_list(dict, "alt", alt_list);
	    }
	}

	list_append_dict(list, dict);
	uhp = uhp->uh_پچھلا.ptr;
    }
}

/*
 * "undofile(name)" function
 */
    خالی
f_undofile(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	اگر (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = تہی;
	}
	ورنہ
	{
	    char_u *ffname = بھرا_Name_save(fname, TRUE);

	    اگر (ffname != تہی)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# ورنہ
    rettv->vval.v_string = تہی;
# endif
}
# ifdef FEAT_PERSISTENT_UNDO
/*
 * Reset undofile option and delete the undofile
 */
    خالی
u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    اگر (!buf->b_p_udf)
	واپس;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    اگر (file_name != تہی)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    سیٹ_option_قدر_give_err((char_u *)"undofile", 0L, تہی, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    خالی
f_undotree(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_opt_بفر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    اگر (buf == تہی)
	واپس;

    dict_T *dict = rettv->vval.v_dict;

    dict_جوڑو_نمبر(dict, "synced", (طویل)buf->b_u_synced);
    dict_جوڑو_نمبر(dict, "seq_last", buf->b_u_seq_آخری);
    dict_جوڑو_نمبر(dict, "save_last", buf->b_u_save_nr_آخری);
    dict_جوڑو_نمبر(dict, "seq_cur", buf->b_u_seq_cur);
    dict_جوڑو_نمبر(dict, "time_cur", (طویل)buf->b_u_وقت_cur);
    dict_جوڑو_نمبر(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    اگر (list != تہی)
    {
	u_eval_درخت(buf, buf->b_u_oldhead, list);
	dict_جوڑو_list(dict, "entries", list);
    }
}

#اگر_ختم
