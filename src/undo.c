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
#define UH_MAGIC 0x18dade	// value for uh_magic when in use
#define UE_MAGIC 0xabc123	// value for ue_magic when in use

// Size of buffer used for encryption.
#define CRYPT_BUF_আকার 8192

#অন্তর্ভাব "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
প্রকারলেখ কাঠামো {
    buf_T	*bi_buf;
    FILE	*bi_fp;
#ifdef FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_বাফার; // CRYPT_BUF_SIZE, NULL when not buffering
    size_t	bi_used;    // bytes written to/read from bi_buffer
    size_t	bi_avail;   // bytes available in bi_buffer
#endif
} bufinfo_T;


স্থির শূন্য u_unch_branch(u_header_T *uhp);
স্থির u_entry_T *u_get_headentry(শূন্য);
স্থির শূন্য u_getbot(শূন্য);
স্থির শূন্য u_doit(পূর্ণসংখ্যা গুনতি);
স্থির শূন্য u_undoredo(পূর্ণসংখ্যা undo);
স্থির শূন্য u_undo_end(পূর্ণসংখ্যা did_undo, পূর্ণসংখ্যা absolute);
স্থির শূন্য u_freeheader(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
স্থির শূন্য u_freebranch(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
স্থির শূন্য u_freeentries(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
স্থির শূন্য u_freeentry(u_entry_T *, long);
#ifdef FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
স্থির পূর্ণসংখ্যা undo_flush(bufinfo_T *bi);
# endif
স্থির পূর্ণসংখ্যা undo_read(bufinfo_T *bi, char_u *বাফার, size_t আকার_2);
স্থির পূর্ণসংখ্যা serialize_uep(bufinfo_T *bi, u_entry_T *uep);
স্থির u_entry_T *unserialize_uep(bufinfo_T *bi, পূর্ণসংখ্যা *error, char_u *file_name);
স্থির শূন্য serialize_pos(bufinfo_T *bi, pos_T pos);
স্থির শূন্য unserialize_pos(bufinfo_T *bi, pos_T *pos);
স্থির শূন্য serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
স্থির শূন্য unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#endif
স্থির শূন্য u_saveline(linenr_T lnum);
স্থির শূন্য u_blockfree(buf_T *buf);

#define U_ALLOC_সারি(আকার_2) lalloc(আকার_2, FALSE)

// used in undo_end() to report number of added and deleted lines
স্থির long	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
স্থির পূর্ণসংখ্যা	undo_undoes = FALSE;

স্থির পূর্ণসংখ্যা	lastmark = 0;

#যদি defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
স্থির পূর্ণসংখ্যা seen_b_u_curhead;
স্থির পূর্ণসংখ্যা seen_b_u_newhead;
স্থির পূর্ণসংখ্যা header_গুনতি;

    স্থির শূন্য
u_check_বৃক্ষ(u_header_T *uhp,
	u_header_T *exp_uh_পরবর্তী,
	u_header_T *exp_uh_alt_পূর্ব)
{
    u_entry_T *uep;

    যদি (uhp == ফাঁকা)
	ফেরত;
    ++header_গুনতি;
    যদি (uhp == curbuf->b_u_curhead && ++seen_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	ফেরত;
    }
    যদি (uhp == curbuf->b_u_newhead && ++seen_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	ফেরত;
    }

    যদি (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    অন্যথা
    {
	// Check pointers back are correct.
	যদি (uhp->uh_পরবর্তী.ptr != exp_uh_পরবর্তী)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_পরবর্তী, uhp->uh_পরবর্তী.ptr);
	}
	যদি (uhp->uh_alt_পূর্ব.ptr != exp_uh_alt_পূর্ব)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_পূর্ব, uhp->uh_alt_পূর্ব.ptr);
	}

	// Check the undo tree at this header.
	জন্য (uep = uhp->uh_entry; uep != ফাঁকা; uep = uep->ue_পরবর্তী)
	{
	    যদি (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		বিরতি;
	    }
	}

	// Check the next alt tree.
	u_check_বৃক্ষ(uhp->uh_alt_পরবর্তী.ptr, uhp->uh_পরবর্তী.ptr, uhp);

	// Check the next header in this branch.
	u_check_বৃক্ষ(uhp->uh_পূর্ব.ptr, uhp, ফাঁকা);
    }
}

    স্থির শূন্য
u_check(পূর্ণসংখ্যা newhead_may_be_NULL)
{
    seen_b_u_newhead = 0;
    seen_b_u_curhead = 0;
    header_গুনতি = 0;

    u_check_বৃক্ষ(curbuf->b_u_oldhead, ফাঁকা, ফাঁকা);

    যদি (seen_b_u_newhead == 0 && curbuf->b_u_oldhead != ফাঁকা
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == ফাঁকা))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    যদি (curbuf->b_u_curhead != ফাঁকা && seen_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    যদি (header_গুনতি != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (long)header_গুনতি, (long)curbuf->b_u_numhead);
    }
}
#endif

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    পূর্ণসংখ্যা
u_save_cursor(শূন্য)
{
    ফেরত (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    পূর্ণসংখ্যা
u_save(linenr_T শীর্ষ_2, linenr_T bot)
{
    যদি (undo_off)
	ফেরত OK;

    যদি (শীর্ষ_2 >= bot || bot > curbuf->b_ml.ml_সারি_গুনতি + 1)
	ফেরত FAIL;	// rely on caller to give an error message

    যদি (শীর্ষ_2 + 2 == bot)
	u_saveline((linenr_T)(শীর্ষ_2 + 1));

    ফেরত (u_savecommon(শীর্ষ_2, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    পূর্ণসংখ্যা
u_savesub(linenr_T lnum)
{
    যদি (undo_off)
	ফেরত OK;

    ফেরত (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    পূর্ণসংখ্যা
u_inssub(linenr_T lnum)
{
    যদি (undo_off)
	ফেরত OK;

    ফেরত (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    পূর্ণসংখ্যা
u_savedel(linenr_T lnum, long nlines)
{
    যদি (undo_off)
	ফেরত OK;

    ফেরত (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_সারি_গুনতি ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    পূর্ণসংখ্যা
undo_allowed(শূন্য)
{
    // Don't allow changes when 'modifiable' is off.
    যদি (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ফেরত FALSE;
    }

#ifdef HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    যদি (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ফেরত FALSE;
    }
#endif

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    যদি (textlock != 0)
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	ফেরত FALSE;
    }

    ফেরত TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    স্থির long
get_undolevel(শূন্য)
{
    যদি (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	ফেরত p_ul;
    ফেরত curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    স্থির পূর্ণসংখ্যা
u_save_সারি(undoline_T *ul, linenr_T lnum)
{
    char_u *সারি = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    যদি (curbuf->b_ml.ml_সারি_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_সারি = vim_strsave((char_u *)"");
    }
    অন্যথা
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_সারি_len;
	ul->ul_সারি = vim_memsave(সারি, ul->ul_len);
    }
    ফেরত ul->ul_সারি == ফাঁকা ? FAIL : OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    স্থির পূর্ণসংখ্যা
has_prop_w_flags(linenr_T lnum, পূর্ণসংখ্যা flags)
{
    char_u  *props;
    পূর্ণসংখ্যা	    i;
    পূর্ণসংখ্যা	    proplen = get_text_props(curbuf, lnum, &props, FALSE);

    জন্য (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * আকার prop, আকার prop);
	যদি (prop.tp_flags & flags)
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}
#endif

/*
 * Common code for various ways to save text before a change.
 * "top" is the line above the first changed line.
 * "bot" is the line below the last changed line.
 * "newbot" is the new bottom line.  Use zero when not known.
 * "reload" is TRUE when saving for a buffer reload.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    পূর্ণসংখ্যা
u_savecommon(
    linenr_T	শীর্ষ_2,
    linenr_T	bot,
    linenr_T	newbot,
    পূর্ণসংখ্যা		reload)
{
    linenr_T	lnum;
    long	i;
    u_header_T	*uhp;
    u_header_T	*old_curhead;
    u_entry_T	*uep;
    u_entry_T	*পূর্ব_uep;
    long	আকার_2;

    যদি (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	যদি (!undo_allowed())
	    ফেরত FAIL;

#ifdef FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	যদি (netbeans_active())
	{
	    যদি (netbeans_is_guarded(শীর্ষ_2, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		ফেরত FAIL;
	    }
	    যদি (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_in_read_only_files));
		ফেরত FAIL;
	    }
	}
#endif
#ifdef FEAT_TERMINAL
	// A change in a terminal buffer removes the highlighting.
	term_change_in_curbuf();
#endif

	/*
	 * Saving text for undo means we are going to make a change.  Give a
	 * warning for a read-only file before making the change, so that the
	 * FileChangedRO event can replace the buffer with a read-write version
	 * (e.g., obtained from a source control system).
	 */
	change_warning(0);
	যদি (bot > curbuf->b_ml.ml_সারি_গুনতি + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_সারি_গুনতি_changed_unexpectedly));
	    ফেরত FAIL;
	}
    }

#ifdef U_DEBUG
    u_check(FALSE);
#endif

#ifdef FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    যদি (bot - শীর্ষ_2 > 1)
    {
	যদি (শীর্ষ_2 > 0 && has_prop_w_flags(শীর্ষ_2 + 1, TP_FLAG_CONT_পূর্ব))
	    --শীর্ষ_2;
	যদি (bot <= curbuf->b_ml.ml_সারি_গুনতি
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_পরবর্তী))
	{
	    ++bot;
	    যদি (newbot != 0)
		++newbot;
	}
    }
#endif

    আকার_2 = bot - শীর্ষ_2 - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    যদি (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	যদি (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_সারি(আকার(u_header_T));
	    যদি (uhp == ফাঁকা)
		যাও nomem;
#ifdef U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#endif
	}
	অন্যথা
	    uhp = ফাঁকা;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	যদি (old_curhead != ফাঁকা)
	{
	    curbuf->b_u_newhead = old_curhead->uh_পরবর্তী.ptr;
	    curbuf->b_u_curhead = ফাঁকা;
	}

	/*
	 * free headers to keep the size right
	 */
	যখন (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != ফাঁকা)
	{
	    u_header_T	    *uhfree = curbuf->b_u_oldhead;

	    যদি (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    অন্যথা যদি (uhfree->uh_alt_পরবর্তী.ptr == ফাঁকা)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    অন্যথা
	    {
		// Free the oldest alternate branch as a whole.
		যখন (uhfree->uh_alt_পরবর্তী.ptr != ফাঁকা)
		    uhfree = uhfree->uh_alt_পরবর্তী.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#ifdef U_DEBUG
	    u_check(TRUE);
#endif
	}

	যদি (uhp == ফাঁকা)		// no undo at all
	{
	    যদি (old_curhead != ফাঁকা)
		u_freebranch(curbuf, old_curhead, ফাঁকা);
	    curbuf->b_u_synced = false;
	    ফেরত OK;
	}

	uhp->uh_পূর্ব.ptr = ফাঁকা;
	uhp->uh_পরবর্তী.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_পরবর্তী.ptr = old_curhead;
	যদি (old_curhead != ফাঁকা)
	{
	    uhp->uh_alt_পূর্ব.ptr = old_curhead->uh_alt_পূর্ব.ptr;
	    যদি (uhp->uh_alt_পূর্ব.ptr != ফাঁকা)
		uhp->uh_alt_পূর্ব.ptr->uh_alt_পরবর্তী.ptr = uhp;
	    old_curhead->uh_alt_পূর্ব.ptr = uhp;
	    যদি (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	অন্যথা
	    uhp->uh_alt_পূর্ব.ptr = ফাঁকা;
	যদি (curbuf->b_u_newhead != ফাঁকা)
	    curbuf->b_u_newhead->uh_পূর্ব.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_last;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_time = vim_time();
	uhp->uh_save_nr = 0;
	curbuf->b_u_time_cur = uhp->uh_time + 1;

	uhp->uh_walk = 0;
	uhp->uh_entry = ফাঁকা;
	uhp->uh_getbot_entry = ফাঁকা;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	যদি (virtual_active() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	অন্যথা
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_changed ? UH_CHANGED : 0) +
		       ((curbuf->b_ml.ml_flags & ML_EMPTY) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, আকার(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	যদি (curbuf->b_u_oldhead == ফাঁকা)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    অন্যথা
    {
	যদি (get_undolevel() < 0)	// no undo at all
	    ফেরত OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	যদি (আকার_2 == 1)
	{
	    uep = u_get_headentry();
	    পূর্ব_uep = ফাঁকা;
	    জন্য (i = 0; i < 10; ++i)
	    {
		যদি (uep == ফাঁকা)
		    বিরতি;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		যদি ((curbuf->b_u_newhead->uh_getbot_entry != uep
			    ? (uep->ue_শীর্ষ + uep->ue_আকার + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_সারি_গুনতি + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_সারি_গুনতি)
			|| (uep->ue_আকার > 1
			    && শীর্ষ_2 >= uep->ue_শীর্ষ
			    && শীর্ষ_2 + 2 <= uep->ue_শীর্ষ + uep->ue_আকার + 1))
		    বিরতি;

		// If it's the same line we can skip saving it again.
		যদি (uep->ue_আকার == 1 && uep->ue_শীর্ষ == শীর্ষ_2)
		{
		    যদি (i > 0)
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
			পূর্ব_uep->ue_পরবর্তী = uep->ue_পরবর্তী;
			uep->ue_পরবর্তী = curbuf->b_u_newhead->uh_entry;
			curbuf->b_u_newhead->uh_entry = uep;
		    }

		    // The executed command may change the line count.
		    যদি (newbot != 0)
			uep->ue_bot = newbot;
		    অন্যথা যদি (bot > curbuf->b_ml.ml_সারি_গুনতি)
			uep->ue_bot = 0;
		    অন্যথা
		    {
			uep->ue_lcount = curbuf->b_ml.ml_সারি_গুনতি;
			curbuf->b_u_newhead->uh_getbot_entry = uep;
		    }
		    ফেরত OK;
		}
		পূর্ব_uep = uep;
		uep = uep->ue_পরবর্তী;
	    }
	}

	// find line number for ue_bot for previous u_save()
	u_getbot();
    }

    /*
     * add lines in front of entry list
     */
    uep = U_ALLOC_সারি(আকার(u_entry_T));
    যদি (uep == ফাঁকা)
	যাও nomem;
    CLEAR_POINTER(uep);
#ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
#endif

    uep->ue_আকার = আকার_2;
    uep->ue_শীর্ষ = শীর্ষ_2;
    যদি (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    অন্যথা যদি (bot > curbuf->b_ml.ml_সারি_গুনতি)
	uep->ue_bot = 0;
    অন্যথা
    {
	uep->ue_lcount = curbuf->b_ml.ml_সারি_গুনতি;
	curbuf->b_u_newhead->uh_getbot_entry = uep;
    }

    যদি (আকার_2 > 0)
    {
	যদি ((uep->ue_array = U_ALLOC_সারি(আকার(undoline_T) * আকার_2)) == ফাঁকা)
	{
	    u_freeentry(uep, 0L);
	    যাও nomem;
	}
	জন্য (i = 0, lnum = শীর্ষ_2 + 1; i < আকার_2; ++i)
	{
	    fast_breakcheck();
	    যদি (got_int)
	    {
		u_freeentry(uep, i);
		ফেরত FAIL;
	    }
	    যদি (u_save_সারি(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		যাও nomem;
	    }
	}
    }
    অন্যথা
	uep->ue_array = ফাঁকা;
    uep->ue_পরবর্তী = curbuf->b_u_newhead->uh_entry;
    curbuf->b_u_newhead->uh_entry = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#ifdef U_DEBUG
    u_check(FALSE);
#endif
    ফেরত OK;

nomem:
    বার্তা_silent = 0;	// must display the prompt
    যদি (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	ফেরত OK;
    }
    do_outofmem_বার্তা((long_u)0);
    ফেরত FAIL;
}

#যদি defined(FEAT_PERSISTENT_UNDO)

# define UF_START_MAGIC	    "Vim\237UnDo\345"  // magic at start of undofile
# define UF_START_MAGIC_LEN	9
# define UF_HEADER_MAGIC	0x5fd0	// magic at start of header
# define UF_HEADER_END_MAGIC	0xe7aa	// magic after last header
# define UF_ENTRY_MAGIC		0xf518	// magic at start of entry
# define UF_ENTRY_END_MAGIC	0x3581	// magic after last entry
# define UF_VERSION		2	// 2-byte undofile version number
# define UF_VERSION_CRYPT	0x8002	// idem, encrypted

// extra fields for header
# define UF_LAST_SAVE_NR	1

// extra fields for uhp
# define UHP_SAVE_NR		1

/*
 * Compute the hash for the current buffer text into hash[UNDO_HASH_SIZE].
 */
    শূন্য
u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;

    sha256_start(&ctx);
    জন্য (lnum = 1; lnum <= curbuf->b_ml.ml_সারি_গুনতি; ++lnum)
	sha256_update(&ctx, ml_get(lnum), (UINT32_T)(ml_get_len(lnum) + 1));
    sha256_finish(&ctx, hash);
}

/*
 * Return an allocated string of the full path of the target undofile.
 * When "reading" is TRUE find the file to read, go over all directories in
 * 'undodir'.
 * When "reading" is FALSE use the first name where the directory exists.
 * Returns NULL when there is no place to write or no file to read.
 */
    স্থির char_u *
u_get_undo_file_name(char_u *buf_ffname, পূর্ণসংখ্যা reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {ফাঁকা, 0};
    char_u	*undo_file_name = ফাঁকা;
    পূর্ণসংখ্যা		dir_len;
    char_u	*p;
    size_t	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    size_t	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    যদি (ffname == ফাঁকা)
	ফেরত ফাঁকা;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    যদি (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    যখন (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	যদি (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    যদি (undo_file_name == ফাঁকা)
		বিরতি;
	    p = gettail(undo_file_name);
	    plen = (size_t)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# অন্যথা
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	অন্যথা
	{
	    dir_name[dir_len] = NUL;
	    যদি (mch_isdir(dir_name))
	    {
		string_T    ret;

		যদি (munged_name.string == ফাঁকা)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    যদি (munged_name.string == ফাঁকা)
			ফেরত ফাঁকা;
		    munged_name.দৈর্ঘ্য = ffnamelen;
		    জন্য (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			যদি (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (size_t)dir_len,
		    munged_name.string, munged_name.দৈর্ঘ্য, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	যদি (undo_file_name != ফাঁকা && (!reading
			       || mch_stat((অক্ষর *)undo_file_name, &st) >= 0))
	    বিরতি;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    ফেরত undo_file_name;
}

    স্থির শূন্য
corruption_error(অক্ষর *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    স্থির শূন্য
u_free_uhp(u_header_T *uhp)
{
    u_entry_T	*nuep;
    u_entry_T	*uep;

    uep = uhp->uh_entry;
    যখন (uep != ফাঁকা)
    {
	nuep = uep->ue_পরবর্তী;
	u_freeentry(uep, uep->ue_আকার);
	uep = nuep;
    }
    vim_free(uhp);
}

/*
 * Write a sequence of bytes to the undo file.
 * Buffers and encrypts as needed.
 * Returns OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    যদি (bi->bi_বাফার != ফাঁকা)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	যখন (bi->bi_used + len_todo >= CRYPT_BUF_আকার)
	{
	    size_t	n = CRYPT_BUF_আকার - bi->bi_used;

	    mch_memmove(bi->bi_বাফার + bi->bi_used, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_used = CRYPT_BUF_আকার;
	    যদি (undo_flush(bi) == FAIL)
		ফেরত FAIL;
	}
	যদি (len_todo > 0)
	{
	    mch_memmove(bi->bi_বাফার + bi->bi_used, p, len_todo);
	    bi->bi_used += len_todo;
	}
	ফেরত OK;
    }
# endif
    যদি (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	ফেরত FAIL;
    ফেরত OK;
}

# ifdef FEAT_CRYPT
    স্থির পূর্ণসংখ্যা
undo_flush(bufinfo_T *bi)
{
    যদি (bi->bi_বাফার != ফাঁকা && bi->bi_state != ফাঁকা && bi->bi_used > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_বাফার, bi->bi_used, FALSE);
	যদি (fwrite(bi->bi_বাফার, bi->bi_used, (size_t)1, bi->bi_fp) != 1)
	    ফেরত FAIL;
	bi->bi_used = 0;
    }
    ফেরত OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    যদি (bi->bi_state != ফাঁকা && bi->bi_বাফার == ফাঁকা)
    {
	// crypting every piece of text separately
	যদি (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	অন্যথা
	{
	    copy = lalloc(len, FALSE);
	    যদি (copy == ফাঁকা)
		ফেরত 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	যদি (copy != small_buf)
	    vim_free(copy);
	ফেরত i == 1 ? OK : FAIL;
    }
# endif
    ফেরত undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
undo_write_bytes(bufinfo_T *bi, long_u nr, পূর্ণসংখ্যা len)
{
    char_u  buf[8];
    পূর্ণসংখ্যা	    i;
    পূর্ণসংখ্যা	    bufi = 0;

    জন্য (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    ফেরত undo_write(bi, buf, (size_t)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    স্থির শূন্য
put_header_ptr(bufinfo_T *bi, u_header_T *uhp)
{
    undo_write_bytes(bi, (long_u)(uhp != ফাঁকা ? uhp->uh_seq : 0), 4);
}

    স্থির পূর্ণসংখ্যা
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    যদি (bi->bi_বাফার != ফাঁকা)
    {
	char_u  buf[4];
	পূর্ণসংখ্যা	n;

	undo_read(bi, buf, (size_t)4);
	n = ((unsigned)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	ফেরত n;
    }
# endif
    ফেরত get4c(bi->bi_fp);
}

    স্থির পূর্ণসংখ্যা
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    যদি (bi->bi_বাফার != ফাঁকা)
    {
	char_u  buf[2];
	পূর্ণসংখ্যা	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	ফেরত n;
    }
# endif
    ফেরত get2c(bi->bi_fp);
}

    স্থির পূর্ণসংখ্যা
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    যদি (bi->bi_বাফার != ফাঁকা)
    {
	char_u  buf[1];

	undo_read(bi, buf, (size_t)1);
	ফেরত buf[0];
    }
# endif
    ফেরত getc(bi->bi_fp);
}

    স্থির time_t
undo_read_time(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    যদি (bi->bi_বাফার != ফাঁকা)
    {
	char_u  buf[8];
	time_t	n = 0;
	পূর্ণসংখ্যা	i;

	undo_read(bi, buf, (size_t)8);
	জন্য (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	ফেরত n;
    }
# endif
    ফেরত get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
undo_read(bufinfo_T *bi, char_u *বাফার, size_t আকার_2)
{
    পূর্ণসংখ্যা retval = OK;

# ifdef FEAT_CRYPT
    যদি (bi->bi_বাফার != ফাঁকা)
    {
	পূর্ণসংখ্যা	আকার_todo = (পূর্ণসংখ্যা)আকার_2;
	char_u	*p = বাফার;

	যখন (আকার_todo > 0)
	{
	    size_t n;

	    যদি (bi->bi_used >= bi->bi_avail)
	    {
		n = fread(bi->bi_বাফার, 1, (size_t)CRYPT_BUF_আকার, bi->bi_fp);
		যদি (n == 0)
		{
		    retval = FAIL;
		    বিরতি;
		}
		bi->bi_avail = n;
		bi->bi_used = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_বাফার, bi->bi_avail, FALSE);
	    }
	    n = আকার_todo;
	    যদি (n > bi->bi_avail - bi->bi_used)
		n = bi->bi_avail - bi->bi_used;
	    mch_memmove(p, bi->bi_বাফার + bi->bi_used, n);
	    bi->bi_used += n;
	    আকার_todo -= (পূর্ণসংখ্যা)n;
	    p += n;
	}
    }
    অন্যথা
# endif
    যদি (fread(বাফার, আকার_2, 1, bi->bi_fp) != 1)
	retval = FAIL;

    যদি (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(বাফার, 0, আকার_2);
    ফেরত retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    স্থির char_u *
read_string_decrypt(bufinfo_T *bi, পূর্ণসংখ্যা len)
{
    char_u  *ptr = alloc(len + 1);

    যদি (ptr == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	ফেরত ফাঁকা;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    যদি (bi->bi_state != ফাঁকা && bi->bi_বাফার == ফাঁকা)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    ফেরত ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    স্থির পূর্ণসংখ্যা
serialize_header(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	time_buf[8];

    // Start writing, first the magic marker and undo info version.
    যদি (fwrite(UF_START_MAGIC, (size_t)UF_START_MAGIC_LEN, (size_t)1, fp) != 1)
	ফেরত FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    যদি (*buf->b_p_চাবি != NUL)
    {
	char_u *header;
	পূর্ণসংখ্যা    header_len;
	long	len;

	undo_write_bytes(bi, (long_u)UF_VERSION_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_method_nr(buf),
					  buf->b_p_চাবি, &header, &header_len);
	যদি (bi->bi_state == ফাঁকা)
	    ফেরত FAIL;
	len = (long)fwrite(header, (size_t)header_len, (size_t)1, fp);
	vim_free(header);
	যদি (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = ফাঁকা;
	    ফেরত FAIL;
	}

	যদি (crypt_whole_undofile(crypt_get_method_nr(buf)))
	{
	    bi->bi_বাফার = alloc(CRYPT_BUF_আকার);
	    যদি (bi->bi_বাফার == ফাঁকা)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = ফাঁকা;
		ফেরত FAIL;
	    }
	    bi->bi_used = 0;
	}
    }
    অন্যথা
# endif
	undo_write_bytes(bi, (long_u)UF_VERSION, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    যদি (undo_write(bi, hash, (size_t)UNDO_HASH_আকার) == FAIL)
	ফেরত FAIL;

    // buffer-specific data
    undo_write_bytes(bi, (long_u)buf->b_ml.ml_সারি_গুনতি, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_সারি_ptr.ul_textlen, 4);
    যদি (buf->b_u_সারি_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_সারি_ptr.ul_সারি,
			(size_t)buf->b_u_সারি_ptr.ul_textlen) == FAIL)
	ফেরত FAIL;
    undo_write_bytes(bi, (long_u)buf->b_u_সারি_lnum, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_সারি_colnr, 4);

    // Undo structures header data
    put_header_ptr(bi, buf->b_u_oldhead);
    put_header_ptr(bi, buf->b_u_newhead);
    put_header_ptr(bi, buf->b_u_curhead);

    undo_write_bytes(bi, (long_u)buf->b_u_numhead, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_seq_last, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_seq_cur, 4);
    time_to_bytes(buf->b_u_time_cur, time_buf);
    undo_write(bi, time_buf, 8);

    // Optional fields.
    undo_write_bytes(bi, 4, 1);
    undo_write_bytes(bi, UF_LAST_SAVE_NR, 1);
    undo_write_bytes(bi, (long_u)buf->b_u_save_nr_last, 4);

    undo_write_bytes(bi, 0, 1);  // end marker

    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
serialize_uhp(bufinfo_T *bi, u_header_T *uhp)
{
    পূর্ণসংখ্যা		i;
    u_entry_T	*uep;
    char_u	time_buf[8];

    যদি (undo_write_bytes(bi, (long_u)UF_HEADER_MAGIC, 2) == FAIL)
	ফেরত FAIL;

    put_header_ptr(bi, uhp->uh_পরবর্তী.ptr);
    put_header_ptr(bi, uhp->uh_পূর্ব.ptr);
    put_header_ptr(bi, uhp->uh_alt_পরবর্তী.ptr);
    put_header_ptr(bi, uhp->uh_alt_পূর্ব.ptr);
    undo_write_bytes(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_bytes(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_bytes(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    জন্য (i = 0; i < NMARKS; ++i)
	serialize_pos(bi, uhp->uh_namedm[i]);
    serialize_visualinfo(bi, &uhp->uh_visual);
    time_to_bytes(uhp->uh_time, time_buf);
    undo_write(bi, time_buf, 8);

    // Optional fields.
    undo_write_bytes(bi, 4, 1);
    undo_write_bytes(bi, UHP_SAVE_NR, 1);
    undo_write_bytes(bi, (long_u)uhp->uh_save_nr, 4);

    undo_write_bytes(bi, 0, 1);  // end marker

    // Write all the entries.
    জন্য (uep = uhp->uh_entry; uep != ফাঁকা; uep = uep->ue_পরবর্তী)
    {
	undo_write_bytes(bi, (long_u)UF_ENTRY_MAGIC, 2);
	যদি (serialize_uep(bi, uep) == FAIL)
	    ফেরত FAIL;
    }
    undo_write_bytes(bi, (long_u)UF_ENTRY_END_MAGIC, 2);
    ফেরত OK;
}

    স্থির u_header_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_header_T	*uhp;
    পূর্ণসংখ্যা		i;
    u_entry_T	*uep, *last_uep;
    পূর্ণসংখ্যা		c;
    পূর্ণসংখ্যা		error;

    uhp = U_ALLOC_সারি(আকার(u_header_T));
    যদি (uhp == ফাঁকা)
	ফেরত ফাঁকা;
    CLEAR_POINTER(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_পরবর্তী.seq = undo_read_4c(bi);
    uhp->uh_পূর্ব.seq = undo_read_4c(bi);
    uhp->uh_alt_পরবর্তী.seq = undo_read_4c(bi);
    uhp->uh_alt_পূর্ব.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    যদি (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	ফেরত ফাঁকা;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    জন্য (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_time = undo_read_time(bi);

    // Optional fields.
    জন্য (;;)
    {
	পূর্ণসংখ্যা len = undo_read_byte(bi);
	পূর্ণসংখ্যা what;

	যদি (len == EOF)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    ফেরত ফাঁকা;
	}
	যদি (len == 0)
	    বিরতি;
	what = undo_read_byte(bi);
	নির্বাচন (what)
	{
	    ক্ষেত্র UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		বিরতি;
	    পূর্বনির্ধারিত:
		// field not supported, skip
		যখন (--len >= 0)
		    (শূন্য)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    last_uep = ফাঁকা;
    যখন ((c = undo_read_2c(bi)) == UF_ENTRY_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	যদি (last_uep == ফাঁকা)
	    uhp->uh_entry = uep;
	অন্যথা
	    last_uep->ue_পরবর্তী = uep;
	last_uep = uep;
	যদি (uep == ফাঁকা || error)
	{
	    u_free_uhp(uhp);
	    ফেরত ফাঁকা;
	}
    }
    যদি (c != UF_ENTRY_END_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	ফেরত ফাঁকা;
    }

    ফেরত uhp;
}

/*
 * Serialize "uep".
 */
    স্থির পূর্ণসংখ্যা
serialize_uep(
    bufinfo_T	*bi,
    u_entry_T	*uep)
{
    পূর্ণসংখ্যা		i;

    undo_write_bytes(bi, (long_u)uep->ue_শীর্ষ, 4);
    undo_write_bytes(bi, (long_u)uep->ue_bot, 4);
    undo_write_bytes(bi, (long_u)uep->ue_lcount, 4);
    undo_write_bytes(bi, (long_u)uep->ue_আকার, 4);
    জন্য (i = 0; i < uep->ue_আকার; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	যদি (undo_write_bytes(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    ফেরত FAIL;
	যদি (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_সারি, uep->ue_array[i].ul_textlen) == FAIL)
	    ফেরত FAIL;
    }
    ফেরত OK;
}

    স্থির u_entry_T *
unserialize_uep(bufinfo_T *bi, পূর্ণসংখ্যা *error, char_u *file_name)
{
    পূর্ণসংখ্যা		i;
    u_entry_T	*uep;
    undoline_T	*array = ফাঁকা;
    char_u	*সারি;
    পূর্ণসংখ্যা		সারি_len;

    uep = U_ALLOC_সারি(আকার(u_entry_T));
    যদি (uep == ফাঁকা)
	ফেরত ফাঁকা;
    CLEAR_POINTER(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_শীর্ষ = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_আকার = undo_read_4c(bi);
    যদি (uep->ue_আকার > 0)
    {
	যদি (uep->ue_আকার < LONG_MAX / (long)আকার(undoline_T))
	    array = U_ALLOC_সারি(আকার(undoline_T) * uep->ue_আকার);
	যদি (array == ফাঁকা)
	{
	    *error = TRUE;
	    ফেরত uep;
	}
	vim_memset(array, 0, আকার(undoline_T) * uep->ue_আকার);
    }
    uep->ue_array = array;

    জন্য (i = 0; i < uep->ue_আকার; ++i)
    {
	সারি_len = undo_read_4c(bi);
	যদি (সারি_len >= 0)
	    সারি = read_string_decrypt(bi, সারি_len);
	অন্যথা
	{
	    সারি = ফাঁকা;
	    corruption_error("line length", file_name);
	}
	যদি (সারি == ফাঁকা)
	{
	    *error = TRUE;
	    ফেরত uep;
	}
	array[i].ul_সারি = সারি;
	array[i].ul_len = সারি_len + 1;
	array[i].ul_textlen = সারি_len;
    }
    ফেরত uep;
}

/*
 * Serialize "pos".
 */
    স্থির শূন্য
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_bytes(bi, (long_u)pos.lnum, 4);
    undo_write_bytes(bi, (long_u)pos.col, 4);
    undo_write_bytes(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    স্থির শূন্য
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    যদি (pos->lnum < 0)
	pos->lnum = 0;
    pos->col = undo_read_4c(bi);
    যদি (pos->col < 0)
	pos->col = 0;
    pos->coladd = undo_read_4c(bi);
    যদি (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    স্থির শূন্য
serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    serialize_pos(bi, info->vi_start);
    serialize_pos(bi, info->vi_end);
    undo_write_bytes(bi, (long_u)info->vi_mode, 4);
    undo_write_bytes(bi, (long_u)info->vi_curswant, 4);
}

/*
 * Unserialize the visualinfo_T at the current position.
 */
    স্থির শূন্য
unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    unserialize_pos(bi, &info->vi_start);
    unserialize_pos(bi, &info->vi_end);
    info->vi_mode = undo_read_4c(bi);
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
    শূন্য
u_write_undo(
    char_u	*name,
    পূর্ণসংখ্যা		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_header_T	*uhp;
    char_u	*file_name;
    পূর্ণসংখ্যা		mark;
# ifdef U_DEBUG
    পূর্ণসংখ্যা		headers_written = 0;
# endif
    পূর্ণসংখ্যা		fd;
    FILE	*fp = ফাঁকা;
    পূর্ণসংখ্যা		perm;
    পূর্ণসংখ্যা		write_ok = FALSE;
# ifdef UNIX
    পূর্ণসংখ্যা		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);

    যদি (name == ফাঁকা)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	যদি (file_name == ফাঁকা)
	{
	    যদি (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    ফেরত;
	}
    }
    অন্যথা
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    যদি (buf->b_ffname != ফাঁকা)
    {
# ifdef UNIX
	যদি (mch_stat((অক্ষর *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_mode;
	    st_old_valid = TRUE;
	}
# অন্যথা
	perm = mch_getperm(buf->b_ffname);
	যদি (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    যদি (mch_getperm(file_name) >= 0)
    {
	যদি (name == ফাঁকা || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((অক্ষর *)file_name, O_RDONLY|O_EXTRA, 0);
	    যদি (fd < 0)
	    {
		যদি (name != ফাঁকা || p_verbose > 0)
		{
		    যদি (name == ফাঁকা)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    যদি (name == ফাঁকা)
			verbose_leave();
		}
		যাও theend;
	    }
	    অন্যথা
	    {
		char_u	mbuf[UF_START_MAGIC_LEN];
		পূর্ণসংখ্যা	len;

		len = read_eintr(fd, mbuf, UF_START_MAGIC_LEN);
		close(fd);
		যদি (len < UF_START_MAGIC_LEN
		      || memcmp(mbuf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
		{
		    যদি (name != ফাঁকা || p_verbose > 0)
		    {
			যদি (name == ফাঁকা)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			যদি (name == ফাঁকা)
			    verbose_leave();
		    }
		    যাও theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    যদি (buf->b_u_numhead == 0 && buf->b_u_সারি_ptr.ul_সারি == ফাঁকা)
    {
	যদি (p_verbose > 0)
	    verb_বার্তা(_("Skipping undo file write, nothing to undo"));
	যাও theend;
    }

    fd = mch_open((অক্ষর *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    যদি (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	যাও theend;
    }
    (শূন্য)mch_setperm(file_name, perm);
    যদি (p_verbose > 0)
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
    যদি (st_old_valid
	    && mch_stat((অক্ষর *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  যদি defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    যদি (buf->b_ffname != ফাঁকা)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    যদি (fp == ফাঁকা)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	close(fd);
	mch_remove(file_name);
	যাও theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    যদি (serialize_header(&bi, hash) == FAIL)
	যাও write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    যখন (uhp != ফাঁকা)
    {
	// Serialize current UHP if we haven't seen it
	যদি (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++headers_written;
# endif
	    যদি (serialize_uhp(&bi, uhp) == FAIL)
		যাও write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	যদি (uhp->uh_পূর্ব.ptr != ফাঁকা && uhp->uh_পূর্ব.ptr->uh_walk != mark)
	    uhp = uhp->uh_পূর্ব.ptr;
	অন্যথা যদি (uhp->uh_alt_পরবর্তী.ptr != ফাঁকা
				     && uhp->uh_alt_পরবর্তী.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_পরবর্তী.ptr;
	অন্যথা যদি (uhp->uh_পরবর্তী.ptr != ফাঁকা && uhp->uh_alt_পূর্ব.ptr == ফাঁকা
					 && uhp->uh_পরবর্তী.ptr->uh_walk != mark)
	    uhp = uhp->uh_পরবর্তী.ptr;
	অন্যথা যদি (uhp->uh_alt_পূর্ব.ptr != ফাঁকা)
	    uhp = uhp->uh_alt_পূর্ব.ptr;
	অন্যথা
	    uhp = uhp->uh_পরবর্তী.ptr;
    }

    যদি (undo_write_bytes(&bi, (long_u)UF_HEADER_END_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    যদি (headers_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", headers_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    যদি (bi.bi_state != ফাঁকা && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# যদি defined(UNIX) && defined(HAVE_FSYNC)
    যদি ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && fflush(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    fclose(fp);
    যদি (!write_ok)
	semsg(_(e_write_error_in_undo_file_str), file_name);

# যদি defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    যদি (buf->b_ffname != ফাঁকা)
	(শূন্য)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    যদি (buf->b_ffname != ফাঁকা)
    {
	vim_acl_T	    acl;

	// For systems that support ACL: get the ACL from the original file.
	acl = mch_get_acl(buf->b_ffname);
	mch_set_acl(file_name, acl);
	mch_free_acl(acl);
    }
# endif

theend:
# ifdef FEAT_CRYPT
    যদি (bi.bi_state != ফাঁকা)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_বাফার);
# endif
    যদি (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    স্থির পূর্ণসংখ্যা
uhp_seq_cmp(ধ্রুবক শূন্য *v1, ধ্রুবক শূন্য *v2)
{
    ধ্রুবক u_header_T *u1 = *(u_header_T **)v1;
    ধ্রুবক u_header_T *u2 = *(u_header_T **)v2;

    ফেরত u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    স্থির long
uhp_table_find(u_header_T **uhp_table, long অঙ্ক_শীর্ষ, long seq)
{
    long    lo = 0;
    long    hi = অঙ্ক_শীর্ষ - 1;

    যখন (lo <= hi)
    {
	long mid = lo + (hi - lo) / 2;

	যদি (uhp_table[mid]->uh_seq < seq)
	    lo = mid + 1;
	অন্যথা যদি (uhp_table[mid]->uh_seq > seq)
	    hi = mid - 1;
	অন্যথা
	    ফেরত mid;
    }
    ফেরত -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    শূন্য
u_read_undo(char_u *name, char_u *hash, char_u *orig_name UNUSED)
{
    char_u	*file_name;
    FILE	*fp;
    long	version, str_len;
    undoline_T	সারি_ptr;
    linenr_T	সারি_lnum;
    colnr_T	সারি_colnr;
    linenr_T	সারি_গুনতি;
    long	অঙ্ক_শীর্ষ = 0;
    long	old_header_seq, new_header_seq, cur_header_seq;
    long	seq_last, seq_cur;
    long	last_save_nr = 0;
    long	old_idx = -1, new_idx = -1, cur_idx = -1;
    long	অঙ্ক_read_uhps = 0;
    time_t	seq_time;
    long	i;
    পূর্ণসংখ্যা		c;
    u_header_T	*uhp;
    u_header_T	**uhp_table = ফাঁকা;
    char_u	read_hash[UNDO_HASH_আকার];
    char_u	magic_buf[UF_START_MAGIC_LEN];
# ifdef U_DEBUG
    পূর্ণসংখ্যা		*uhp_table_used;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);
    সারি_ptr.ul_len = 0;
    সারি_ptr.ul_textlen = 0;
    সারি_ptr.ul_সারি = ফাঁকা;

    যদি (name == ফাঁকা)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	যদি (file_name == ফাঁকা)
	    ফেরত;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	যদি (mch_stat((অক্ষর *)orig_name, &st_orig) >= 0
		&& mch_stat((অক্ষর *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    যদি (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    ফেরত;
	}
# endif
    }
    অন্যথা
	file_name = name;

    যদি (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((অক্ষর *)file_name, "r");
    যদি (fp == ফাঁকা)
    {
	যদি (name != ফাঁকা || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	যাও error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    যদি (fread(magic_buf, UF_START_MAGIC_LEN, 1, fp) != 1
		|| memcmp(magic_buf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	যাও error;
    }
    version = get2c(fp);
    যদি (version == UF_VERSION_CRYPT)
    {
# ifdef FEAT_CRYPT
	যদি (*curbuf->b_p_চাবি == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    যাও error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_চাবি);
	যদি (bi.bi_state == ফাঁকা)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    যাও error;
	}
	যদি (crypt_whole_undofile(bi.bi_state->method_nr))
	{
	    bi.bi_বাফার = alloc(CRYPT_BUF_আকার);
	    যদি (bi.bi_বাফার == ফাঁকা)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = ফাঁকা;
		যাও error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_used = 0;
	}
# অন্যথা
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	যাও error;
# endif
    }
    অন্যথা যদি (version != UF_VERSION)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	যাও error;
    }

    যদি (undo_read(&bi, read_hash, (size_t)UNDO_HASH_আকার) == FAIL)
    {
	corruption_error("hash", file_name);
	যাও error;
    }
    সারি_গুনতি = (linenr_T)undo_read_4c(&bi);
    যদি (memcmp(hash, read_hash, UNDO_HASH_আকার) != 0
				  || সারি_গুনতি != curbuf->b_ml.ml_সারি_গুনতি)
    {
	যদি (p_verbose > 0 || name != ফাঁকা)
	{
	    যদি (name == ফাঁকা)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    যদি (name == ফাঁকা)
		verbose_leave();
	}
	যাও error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    যদি (str_len < 0)
	যাও error;
    যদি (str_len > 0)
    {
	সারি_ptr.ul_সারি = read_string_decrypt(&bi, str_len);
	সারি_ptr.ul_len = str_len + 1;
	সারি_ptr.ul_textlen = str_len;
    }
    সারি_lnum = (linenr_T)undo_read_4c(&bi);
    সারি_colnr = (colnr_T)undo_read_4c(&bi);
    যদি (সারি_lnum < 0 || সারি_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	যাও error;
    }

    // Begin general undo data
    old_header_seq = undo_read_4c(&bi);
    new_header_seq = undo_read_4c(&bi);
    cur_header_seq = undo_read_4c(&bi);
    অঙ্ক_শীর্ষ = undo_read_4c(&bi);
    seq_last = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_time = undo_read_time(&bi);

    // Optional header fields.
    জন্য (;;)
    {
	পূর্ণসংখ্যা len = undo_read_byte(&bi);
	পূর্ণসংখ্যা what;

	যদি (len == 0 || len == EOF)
	    বিরতি;
	what = undo_read_byte(&bi);
	নির্বাচন (what)
	{
	    ক্ষেত্র UF_LAST_SAVE_NR:
		last_save_nr = undo_read_4c(&bi);
		বিরতি;
	    পূর্বনির্ধারিত:
		// field not supported, skip
		যখন (--len >= 0)
		    (শূন্য)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    যদি (অঙ্ক_শীর্ষ > 0)
    {
	যদি (অঙ্ক_শীর্ষ < LONG_MAX / (long)আকার(u_header_T *))
	    uhp_table = U_ALLOC_সারি(অঙ্ক_শীর্ষ * আকার(u_header_T *));
	যদি (uhp_table == ফাঁকা)
	    যাও error;
    }

    যখন ((c = undo_read_2c(&bi)) == UF_HEADER_MAGIC)
    {
	যদি (অঙ্ক_read_uhps >= অঙ্ক_শীর্ষ)
	{
	    corruption_error("num_head too small", file_name);
	    যাও error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	যদি (uhp == ফাঁকা)
	    যাও error;
	uhp_table[অঙ্ক_read_uhps++] = uhp;
    }

    যদি (অঙ্ক_read_uhps != অঙ্ক_শীর্ষ)
    {
	corruption_error("num_head", file_name);
	যাও error;
    }
    যদি (c != UF_HEADER_END_MAGIC)
    {
	corruption_error("end marker", file_name);
	যাও error;
    }

# ifdef U_DEBUG
    uhp_table_used = alloc_clear(আকার(পূর্ণসংখ্যা) * অঙ্ক_শীর্ষ + 1);
#  define SET_FLAG(j) ++uhp_table_used[j]
# অন্যথা
#  define SET_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    যদি (অঙ্ক_শীর্ষ > 0)
	qsort(uhp_table, (size_t)অঙ্ক_শীর্ষ, আকার(u_header_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    জন্য (i = 0; i < অঙ্ক_শীর্ষ - 1; i++)
    {
	যদি (uhp_table[i]->uh_seq == uhp_table[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    যাও error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    করো { \
	long fidx = uhp_table_find(uhp_table, অঙ্ক_শীর্ষ, (link).seq); \
	\
	যদি (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_table[fidx]; \
	    SET_FLAG(fidx); \
	} \
	অন্যথা \
	    (link).ptr = ফাঁকা; \
    } যখন (0)

    জন্য (i = 0; i < অঙ্ক_শীর্ষ; i++)
    {
	uhp = uhp_table[i];
	SWIZZLE_SEQ(uhp->uh_পরবর্তী, i);
	SWIZZLE_SEQ(uhp->uh_পূর্ব, i);
	SWIZZLE_SEQ(uhp->uh_alt_পরবর্তী, i);
	SWIZZLE_SEQ(uhp->uh_alt_পূর্ব, i);
	যদি (old_header_seq > 0 && old_idx < 0 && uhp->uh_seq == old_header_seq)
	{
	    old_idx = i;
	    SET_FLAG(i);
	}
	যদি (new_header_seq > 0 && new_idx < 0 && uhp->uh_seq == new_header_seq)
	{
	    new_idx = i;
	    SET_FLAG(i);
	}
	যদি (cur_header_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_header_seq)
	{
	    cur_idx = i;
	    SET_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? ফাঁকা : uhp_table[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? ফাঁকা : uhp_table[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? ফাঁকা : uhp_table[cur_idx];
    curbuf->b_u_সারি_ptr = সারি_ptr;
    curbuf->b_u_সারি_lnum = সারি_lnum;
    curbuf->b_u_সারি_colnr = সারি_colnr;
    curbuf->b_u_numhead = অঙ্ক_শীর্ষ;
    curbuf->b_u_seq_last = seq_last;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_time_cur = seq_time;
    curbuf->b_u_save_nr_last = last_save_nr;
    curbuf->b_u_save_nr_cur = last_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_table);

# ifdef U_DEBUG
    জন্য (i = 0; i < অঙ্ক_শীর্ষ; ++i)
	যদি (uhp_table_used[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_table_used);
    u_check(TRUE);
# endif

    যদি (name != ফাঁকা)
	smsg(_("Finished reading undo file %s"), file_name);
    যাও theend;

error:
    vim_free(সারি_ptr.ul_সারি);
    যদি (uhp_table != ফাঁকা)
    {
	জন্য (i = 0; i < অঙ্ক_read_uhps; i++)
	    যদি (uhp_table[i] != ফাঁকা)
		u_free_uhp(uhp_table[i]);
	vim_free(uhp_table);
    }

theend:
# ifdef FEAT_CRYPT
    যদি (bi.bi_state != ফাঁকা)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_বাফার);
# endif
    যদি (fp != ফাঁকা)
	fclose(fp);
    যদি (file_name != name)
	vim_free(file_name);
    ফেরত;
}

#endif // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    শূন্য
u_undo(পূর্ণসংখ্যা গুনতি)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    যদি (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	গুনতি = 1;
    }

    যদি (vim_strchr(p_cpo, CPO_UNDO) == ফাঁকা)
	undo_undoes = TRUE;
    অন্যথা
	undo_undoes = !undo_undoes;
    u_doit(গুনতি);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    শূন্য
u_redo(পূর্ণসংখ্যা গুনতি)
{
    যদি (vim_strchr(p_cpo, CPO_UNDO) == ফাঁকা)
	undo_undoes = FALSE;
    u_doit(গুনতি);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    স্থির শূন্য
u_doit(পূর্ণসংখ্যা startcount)
{
    পূর্ণসংখ্যা গুনতি = startcount;

    যদি (!undo_allowed())
	ফেরত;

    u_newcount = 0;
    u_oldcount = 0;
    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;
    যখন (গুনতি--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	যদি (undo_undoes)
	{
	    যদি (curbuf->b_u_curhead == ফাঁকা)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    অন্যথা যদি (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_পরবর্তী.ptr;
	    // nothing to undo
	    যদি (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == ফাঁকা)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		যদি (গুনতি == startcount - 1)
		{
		    যদি (!shortmess(SHM_UNDO))
			বার্তা(_("Already at oldest change"));
		    ফেরত;
		}
		বিরতি;
	    }

	    u_undoredo(TRUE);
	}
	অন্যথা
	{
	    যদি (curbuf->b_u_curhead == ফাঁকা || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		যদি (গুনতি == startcount - 1)
		{
		    যদি (!shortmess(SHM_UNDO))
			বার্তা(_("Already at newest change"));
		    ফেরত;
		}
		বিরতি;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    যদি (curbuf->b_u_curhead->uh_পূর্ব.ptr == ফাঁকা)
		curbuf->b_u_newhead = curbuf->b_u_curhead;
	    curbuf->b_u_curhead = curbuf->b_u_curhead->uh_পূর্ব.ptr;
	}
    }
    u_undo_end(undo_undoes, FALSE);
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
    শূন্য
undo_time(
    long	step,
    পূর্ণসংখ্যা		sec,
    পূর্ণসংখ্যা		file,
    পূর্ণসংখ্যা		absolute)
{
    long	    target;
    long	    closest;
    long	    closest_start;
    long	    closest_seq = 0;
    long	    val;
    u_header_T	    *uhp = ফাঁকা;
    u_header_T	    *last;
    পূর্ণসংখ্যা		    mark;
    পূর্ণসংখ্যা		    nomark = 0;  // shut up compiler
    পূর্ণসংখ্যা		    round;
    পূর্ণসংখ্যা		    dosec = sec;
    পূর্ণসংখ্যা		    dofile = file;
    পূর্ণসংখ্যা		    above = FALSE;
    পূর্ণসংখ্যা		    did_undo = TRUE;

    যদি (text_locked())
    {
	text_locked_বার্তা();
	ফেরত;
    }

    // First make sure the current undoable change is synced.
    যদি (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    যদি (absolute)
    {
	target = step;
	closest = -1;
    }
    অন্যথা
    {
	যদি (dosec)
	    target = (long)(curbuf->b_u_time_cur) + step;
	অন্যথা যদি (dofile)
	{
	    যদি (step < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		যদি (uhp != ফাঁকা)
		    uhp = uhp->uh_পরবর্তী.ptr;
		অন্যথা
		    uhp = curbuf->b_u_newhead;
		যদি (uhp != ফাঁকা && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    target = curbuf->b_u_save_nr_cur + step;
		অন্যথা
		    // count the changes since the last write as one step
		    target = curbuf->b_u_save_nr_cur + step + 1;
		যদি (target <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    অন্যথা
	    {
		// Moving forward to a newer write.
		target = curbuf->b_u_save_nr_cur + step;
		যদি (target > curbuf->b_u_save_nr_last)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    target = curbuf->b_u_seq_last + 1;
		    dofile = FALSE;
		}
	    }
	}
	অন্যথা
	    target = curbuf->b_u_seq_cur + step;
	যদি (step < 0)
	{
	    যদি (target < 0)
		target = 0;
	    closest = -1;
	}
	অন্যথা
	{
	    যদি (dosec)
		closest = (long)(vim_time() + 1);
	    অন্যথা যদি (dofile)
		closest = curbuf->b_u_save_nr_last + 2;
	    অন্যথা
		closest = curbuf->b_u_seq_last + 2;
	    যদি (target >= closest)
		target = closest - 1;
	}
    }
    closest_start = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    যদি (target == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	যাও target_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    জন্য (round = 1; round <= 2; ++round)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	যদি (curbuf->b_u_curhead == ফাঁকা)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	অন্যথা
	    uhp = curbuf->b_u_curhead;

	যখন (uhp != ফাঁকা)
	{
	    uhp->uh_walk = mark;
	    যদি (dosec)
		val = (long)(uhp->uh_time);
	    অন্যথা যদি (dofile)
		val = uhp->uh_save_nr;
	    অন্যথা
		val = uhp->uh_seq;

	    যদি (round == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		যদি ((step < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
			      : uhp->uh_seq > curbuf->b_u_seq_cur)
			&& ((dosec && val == closest)
			    ? (step < 0
				? uhp->uh_seq < closest_seq
				: uhp->uh_seq > closest_seq)
			    : closest == closest_start
				|| (val > target
				    ? (closest > target
					? val - target <= closest - target
					: val - target <= target - closest)
				    : (closest > target
					? target - val <= closest - target
					: target - val <= target - closest))))
		{
		    closest = val;
		    closest_seq = uhp->uh_seq;
		}
	    }

	    // Quit searching when we found a match.  But when searching for a
	    // time we need to continue looking for the best uh_seq.
	    যদি (target == val && !dosec)
	    {
		target = uhp->uh_seq;
		বিরতি;
	    }

	    // go down in the tree if we haven't been there
	    যদি (uhp->uh_পূর্ব.ptr != ফাঁকা && uhp->uh_পূর্ব.ptr->uh_walk != nomark
					 && uhp->uh_পূর্ব.ptr->uh_walk != mark)
		uhp = uhp->uh_পূর্ব.ptr;

	    // go to alternate branch if we haven't been there
	    অন্যথা যদি (uhp->uh_alt_পরবর্তী.ptr != ফাঁকা
		    && uhp->uh_alt_পরবর্তী.ptr->uh_walk != nomark
		    && uhp->uh_alt_পরবর্তী.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_পরবর্তী.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    অন্যথা যদি (uhp->uh_পরবর্তী.ptr != ফাঁকা && uhp->uh_alt_পূর্ব.ptr == ফাঁকা
		    && uhp->uh_পরবর্তী.ptr->uh_walk != nomark
		    && uhp->uh_পরবর্তী.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		যদি (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_পরবর্তী.ptr;
	    }

	    অন্যথা
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		যদি (uhp->uh_alt_পূর্ব.ptr != ফাঁকা)
		    uhp = uhp->uh_alt_পূর্ব.ptr;
		অন্যথা
		    uhp = uhp->uh_পরবর্তী.ptr;
	    }
	}

	যদি (uhp != ফাঁকা)    // found it
	    বিরতি;

	যদি (absolute)
	{
	    semsg(_(e_undo_সংখ্যা_nr_not_found), step);
	    ফেরত;
	}

	যদি (closest == closest_start)
	{
	    যদি (!shortmess(SHM_UNDO))
	    {
		যদি (step < 0)
		    বার্তা(_("Already at oldest change"));
		অন্যথা
		    বার্তা(_("Already at newest change"));
	    }
	    ফেরত;
	}

	target = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	যদি (step < 0)
	    above = TRUE;	// stop above the header
    }

target_zero:
    // If we found it: Follow the path to go to where we want to be.
    যদি (uhp != ফাঁকা || target == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	যখন (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    যদি (uhp == ফাঁকা)
		uhp = curbuf->b_u_newhead;
	    অন্যথা
		uhp = uhp->uh_পরবর্তী.ptr;
	    যদি (uhp == ফাঁকা || (target > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == target && !above))
		বিরতি;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    যদি (target > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	যদি (target > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    যখন (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		যদি (uhp == ফাঁকা)
		    বিরতি;

		// Go back to the first branch with a mark.
		যখন (uhp->uh_alt_পূর্ব.ptr != ফাঁকা
				     && uhp->uh_alt_পূর্ব.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_পূর্ব.ptr;

		// Find the last branch with a mark, that's the one.
		last = uhp;
		যখন (last->uh_alt_পরবর্তী.ptr != ফাঁকা
				    && last->uh_alt_পরবর্তী.ptr->uh_walk == mark)
		    last = last->uh_alt_পরবর্তী.ptr;
		যদি (last != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    যখন (uhp->uh_alt_পূর্ব.ptr != ফাঁকা)
			uhp = uhp->uh_alt_পূর্ব.ptr;
		    যদি (last->uh_alt_পরবর্তী.ptr != ফাঁকা)
			last->uh_alt_পরবর্তী.ptr->uh_alt_পূর্ব.ptr =
							last->uh_alt_পূর্ব.ptr;
		    last->uh_alt_পূর্ব.ptr->uh_alt_পরবর্তী.ptr =
							last->uh_alt_পরবর্তী.ptr;
		    last->uh_alt_পূর্ব.ptr = ফাঁকা;
		    last->uh_alt_পরবর্তী.ptr = uhp;
		    uhp->uh_alt_পূর্ব.ptr = last;

		    যদি (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = last;
		    uhp = last;
		    যদি (uhp->uh_পরবর্তী.ptr != ফাঁকা)
			uhp->uh_পরবর্তী.ptr->uh_পূর্ব.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		যদি (uhp->uh_walk != mark)
		    বিরতি;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		যদি (uhp->uh_seq == target && above)
		{
		    curbuf->b_u_seq_cur = target - 1;
		    বিরতি;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		যদি (uhp->uh_পূর্ব.ptr == ফাঁকা)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_পূর্ব.ptr;
		did_undo = FALSE;

		যদি (uhp->uh_seq == target)	// found it!
		    বিরতি;

		uhp = uhp->uh_পূর্ব.ptr;
		যদি (uhp == ফাঁকা || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    internal_error("undo_time()");
		    বিরতি;
		}
	    }
	}
    }
    u_undo_end(did_undo, absolute);
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
    স্থির শূন্য
u_undoredo(পূর্ণসংখ্যা undo)
{
    undoline_T	*newarray = ফাঁকা;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	শীর্ষ_2, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    long	i;
    u_entry_T	*uep, *nuep;
    u_entry_T	*newlist = ফাঁকা;
    পূর্ণসংখ্যা		old_flags;
    পূর্ণসংখ্যা		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    পূর্ণসংখ্যা		empty_বাফার;		    // buffer became empty
    u_header_T	*curhead = curbuf->b_u_curhead;

    // Don't want autocommands using the undo structures here, they are
    // invalid till the end.
    block_autocmds();

#ifdef U_DEBUG
    u_check(FALSE);
#endif
    old_flags = curhead->uh_flags;
    new_flags = (curbuf->b_changed ? UH_CHANGED : 0) +
	       ((curbuf->b_ml.ml_flags & ML_EMPTY) ? UH_EMPTYBUF : 0);
    setpcmark();

    /*
     * save marks before undo/redo
     */
    mch_memmove(namedm, curbuf->b_namedm, আকার(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_start.lnum = curbuf->b_ml.ml_সারি_গুনতি;
    curbuf->b_op_start.col = 0;
    curbuf->b_op_end.lnum = 0;
    curbuf->b_op_end.col = 0;

    জন্য (uep = curhead->uh_entry; uep != ফাঁকা; uep = nuep)
    {
	শীর্ষ_2 = uep->ue_শীর্ষ;
	bot = uep->ue_bot;
	যদি (bot == 0)
	    bot = curbuf->b_ml.ml_সারি_গুনতি + 1;
	যদি (শীর্ষ_2 > curbuf->b_ml.ml_সারি_গুনতি || শীর্ষ_2 >= bot
				      || bot > curbuf->b_ml.ml_সারি_গুনতি + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_সারি_numbers_wrong);
	    changed();		// don't want UNCHANGED now
	    ফেরত;
	}

	oldsize = bot - শীর্ষ_2 - 1;    // number of lines before undo
	newsize = uep->ue_আকার;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    যদি (lnum >= শীর্ষ_2 && lnum <= শীর্ষ_2 + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    অন্যথা যদি (শীর্ষ_2 < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		জন্য (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(শীর্ষ_2 + 1 + i);

		    যদি (curbuf->b_ml.ml_সারি_len != uep->ue_array[i].ul_len
			    || memcmp(uep->ue_array[i].ul_সারি, p,
						curbuf->b_ml.ml_সারি_len) != 0)
			বিরতি;
		}
		যদি (i == newsize && newlnum == MAXLNUM && uep->ue_পরবর্তী == ফাঁকা)
		{
		    newlnum = শীর্ষ_2;
		    new_curpos.lnum = newlnum + 1;
		}
		অন্যথা যদি (i < newsize)
		{
		    newlnum = শীর্ষ_2 + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	empty_বাফার = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	যদি (oldsize > 0)
	{
	    যদি ((newarray = U_ALLOC_সারি(আকার(undoline_T) * oldsize)) == ফাঁকা)
	    {
		do_outofmem_বার্তা((long_u)(আকার(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		যখন (uep != ফাঁকা)
		{
		    nuep = uep->ue_পরবর্তী;
		    u_freeentry(uep, uep->ue_আকার);
		    uep = nuep;
		}
		বিরতি;
	    }
	    // delete backwards, it goes faster in most cases
	    জন্য (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		যদি (u_save_সারি(&newarray[i], lnum) == FAIL)
		    do_outofmem_বার্তা((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		যদি (curbuf->b_ml.ml_সারি_গুনতি == 1)
		    empty_বাফার = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	অন্যথা
	    newarray = ফাঁকা;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	যদি (newsize)
	{
	    জন্য (lnum = শীর্ষ_2, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		যদি (empty_বাফার && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_সারি,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		অন্যথা
		    ml_append_flags(lnum, uep->ue_array[i].ul_সারি,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_সারি);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#ifdef FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, শীর্ষ_2, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, শীর্ষ_2 + newsize + 1, &uep->ue_props,
								UNDOPROP_BELOW);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, শীর্ষ_2, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_BELOW);
	যদি (newsize > 0)
	    adjust_props_for_undo(curbuf, শীর্ষ_2 + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_BELOW);
#endif

	// adjust marks
	যদি (oldsize != newsize)
	{
	    mark_adjust(শীর্ষ_2 + 1, শীর্ষ_2 + oldsize, (long)MAXLNUM,
					       (long)newsize - (long)oldsize);
	    যদি (curbuf->b_op_start.lnum > শীর্ষ_2 + oldsize)
		curbuf->b_op_start.lnum += newsize - oldsize;
	    যদি (curbuf->b_op_end.lnum > শীর্ষ_2 + oldsize)
		curbuf->b_op_end.lnum += newsize - oldsize;
	}
	যদি (oldsize > 0 || newsize > 0)
	{
	    changed_সারিগুলি(শীর্ষ_2 + 1, 0, bot, newsize - oldsize);
#ifdef FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    যদি (spell_check_window(curwin) && bot <= curbuf->b_ml.ml_সারি_গুনতি)
		redrawWinline(curwin, bot);
#endif
	}

	// Set the '[ mark.
	যদি (শীর্ষ_2 + 1 < curbuf->b_op_start.lnum)
	    curbuf->b_op_start.lnum = শীর্ষ_2 + 1;
	// Set the '] mark.
	যদি (newsize == 0 && শীর্ষ_2 + 1 > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = শীর্ষ_2 + 1;
	অন্যথা যদি (শীর্ষ_2 + newsize > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = শীর্ষ_2 + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_আকার = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = শীর্ষ_2 + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_পরবর্তী;
	uep->ue_পরবর্তী = newlist;
	newlist = uep;
    }

    // Ensure the '[ and '] marks are within bounds.
    যদি (curbuf->b_op_start.lnum > curbuf->b_ml.ml_সারি_গুনতি)
	 curbuf->b_op_start.lnum = curbuf->b_ml.ml_সারি_গুনতি;
    যদি (curbuf->b_op_end.lnum > curbuf->b_ml.ml_সারি_গুনতি)
	 curbuf->b_op_end.lnum = curbuf->b_ml.ml_সারি_গুনতি;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_entry = newlist;
    curhead->uh_flags = new_flags;
    যদি ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_EMPTY;
    যদি (old_flags & UH_CHANGED)
	changed();
    অন্যথা
#ifdef FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	যদি (!isNetbeansModified(curbuf))
#endif
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    জন্য (i = 0; i < NMARKS; ++i)
    {
	যদি (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	যদি (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	অন্যথা
	    curhead->uh_namedm[i].lnum = 0;
    }
    যদি (curhead->uh_visual.vi_start.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    যদি (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    যদি (curwin->w_cursor.lnum <= curbuf->b_ml.ml_সারি_গুনতি)
    {
	যদি (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = curhead->uh_cursor.col;
	    যদি (virtual_active() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    অন্যথা
		curwin->w_cursor.coladd = 0;
	}
	অন্যথা
	    beginline(BL_SOL | BL_FIX);
    }
    অন্যথা
    {
	// We get here with the current cursor line being past the end (eg
	// after adding lines at the end of the file, and then undoing it).
	// check_cursor() will move the cursor to the last line.  Move it to
	// the first column here.
	curwin->w_cursor.col = 0;
	curwin->w_cursor.coladd = 0;
    }

    // Make sure the cursor is on an existing line and column.
    check_cursor();

    // Remember where we are for "g-" and ":earlier 10s".
    curbuf->b_u_seq_cur = curhead->uh_seq;
    যদি (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	যদি (curhead->uh_পরবর্তী.ptr != ফাঁকা)
	    curbuf->b_u_seq_cur = curhead->uh_পরবর্তী.ptr->uh_seq;
	অন্যথা
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    যদি (curhead->uh_save_nr != 0)
    {
	যদি (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	অন্যথা
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr;
    }

    // The timestamp can be the same for multiple changes, just use the one of
    // the undone/redone change.
    curbuf->b_u_time_cur = curhead->uh_time;

    unblock_autocmds();
#ifdef U_DEBUG
    u_check(FALSE);
#endif
}

/*
 * If we deleted or added lines, report the number of less/more lines.
 * Otherwise, report the number of changes (this may be incorrect
 * in some cases, but it's better than nothing).
 */
    স্থির শূন্য
u_undo_end(
    পূর্ণসংখ্যা		did_undo,	// just did an undo
    পূর্ণসংখ্যা		absolute)	// used ":undo N"
{
    অক্ষর	*msgstr;
    u_header_T	*uhp;
    char_u	msgbuf[80];

#ifdef FEAT_FOLDING
    যদি ((fdo_flags & FDO_UNDO) && চাবি_Typed)
	foldOpenCursor();
#endif

    যদি (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	ফেরত;

    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)
	--u_newcount;

    u_oldcount -= u_newcount;
    যদি (u_oldcount == -1)
	msgstr = N_("more line");
    অন্যথা যদি (u_oldcount < 0)
	msgstr = N_("more lines");
    অন্যথা যদি (u_oldcount == 1)
	msgstr = N_("line less");
    অন্যথা যদি (u_oldcount > 1)
	msgstr = N_("fewer lines");
    অন্যথা
    {
	u_oldcount = u_newcount;
	যদি (u_newcount == 1)
	    msgstr = N_("change");
	অন্যথা
	    msgstr = N_("changes");
    }

    যদি (curbuf->b_u_curhead != ফাঁকা)
    {
	// For ":undo N" we prefer a "after #N" message.
	যদি (absolute && curbuf->b_u_curhead->uh_পরবর্তী.ptr != ফাঁকা)
	{
	    uhp = curbuf->b_u_curhead->uh_পরবর্তী.ptr;
	    did_undo = FALSE;
	}
	অন্যথা যদি (did_undo)
	    uhp = curbuf->b_u_curhead;
	অন্যথা
	    uhp = curbuf->b_u_curhead->uh_পরবর্তী.ptr;
    }
    অন্যথা
	uhp = curbuf->b_u_newhead;

    যদি (uhp == ফাঁকা)
	*msgbuf = NUL;
    অন্যথা
	add_time(msgbuf, আকার(msgbuf), uhp->uh_time);

#ifdef FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp->w_বাফার == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#endif
    যদি (VIsual_active)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == ফাঁকা ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    শূন্য
u_sync(
    পূর্ণসংখ্যা	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    যদি (curbuf->b_u_synced || (!force && no_u_sync > 0))
	ফেরত;
#যদি defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    যদি (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	ফেরত;		    // XIM is busy, don't break an undo sequence
#endif
    যদি (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    অন্যথা
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = ফাঁকা;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    শূন্য
ex_undolist(exarg_T *eap UNUSED)
{
    garray_T	ga;
    u_header_T	*uhp;
    পূর্ণসংখ্যা		mark;
    পূর্ণসংখ্যা		nomark;
    পূর্ণসংখ্যা		changes = 1;
    পূর্ণসংখ্যা		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, আকার(অক্ষর *), 20);

    uhp = curbuf->b_u_oldhead;
    যখন (uhp != ফাঁকা)
    {
	যদি (uhp->uh_পূর্ব.ptr == ফাঁকা && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    যদি (ga_grow(&ga, 1) == FAIL)
		বিরতি;
	    len = vim_snprintf((অক্ষর *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    add_time(IObuff + len, IOSIZE - len, uhp->uh_time);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (পূর্ণসংখ্যা)STRLEN(IObuff + len);
	    যদি (uhp->uh_save_nr > 0)
	    {
		পূর্ণসংখ্যা n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((অক্ষর *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_তথ্য))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	যদি (uhp->uh_পূর্ব.ptr != ফাঁকা && uhp->uh_পূর্ব.ptr->uh_walk != nomark
					 && uhp->uh_পূর্ব.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_পূর্ব.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	অন্যথা যদি (uhp->uh_alt_পরবর্তী.ptr != ফাঁকা
		&& uhp->uh_alt_পরবর্তী.ptr->uh_walk != nomark
		&& uhp->uh_alt_পরবর্তী.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_পরবর্তী.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	অন্যথা যদি (uhp->uh_পরবর্তী.ptr != ফাঁকা && uhp->uh_alt_পূর্ব.ptr == ফাঁকা
		&& uhp->uh_পরবর্তী.ptr->uh_walk != nomark
		&& uhp->uh_পরবর্তী.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_পরবর্তী.ptr;
	    --changes;
	}

	অন্যথা
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    যদি (uhp->uh_alt_পূর্ব.ptr != ফাঁকা)
		uhp = uhp->uh_alt_পূর্ব.ptr;
	    অন্যথা
	    {
		uhp = uhp->uh_পরবর্তী.ptr;
		--changes;
	    }
	}
    }

    যদি (ga.ga_len == 0)
	বার্তা(_("Nothing to undo"));
    অন্যথা
    {
	পূর্ণসংখ্যা		i;

	sort_strings((char_u **)ga.ga_তথ্য, ga.ga_len);

	বার্তা_start();
	বার্তা_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	জন্য (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    বার্তা_putchar('\n');
	    যদি (got_int)
		বিরতি;
	    বার্তা_puts(((অক্ষর **)ga.ga_তথ্য)[i]);
	}
	বার্তা_end();

	ga_clear_strings(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    শূন্য
ex_undojoin(exarg_T *eap UNUSED)
{
    যদি (curbuf->b_u_newhead == ফাঁকা)
	ফেরত;		    // nothing changed before
    যদি (curbuf->b_u_curhead != ফাঁকা)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	ফেরত;
    }
    যদি (!curbuf->b_u_synced)
	ফেরত;		    // already unsynced
    যদি (get_undolevel() < 0)
	ফেরত;		    // no entries, nothing to do
    অন্যথা
	// Append next change to the last entry
	curbuf->b_u_synced = false;
}

/*
 * Called after writing or reloading the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
    শূন্য
u_unchanged(buf_T *buf)
{
    u_unch_branch(buf->b_u_oldhead);
    buf->b_did_warn = false;
}

/*
 * After reloading a buffer which was saved for 'undoreload': Find the first
 * line that was changed and set the cursor there.
 */
    শূন্য
u_find_first_changed(শূন্য)
{
    u_header_T	*uhp = curbuf->b_u_newhead;
    u_entry_T   *uep;
    linenr_T	lnum;

    যদি (curbuf->b_u_curhead != ফাঁকা || uhp == ফাঁকা)
	ফেরত;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_entry;
    যদি (uep->ue_শীর্ষ != 0 || uep->ue_bot != 0)
	ফেরত;

    জন্য (lnum = 1; lnum < curbuf->b_ml.ml_সারি_গুনতি
					      && lnum <= uep->ue_আকার; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	যদি (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_সারি_len
		|| memcmp(p, uep->ue_array[lnum - 1].ul_সারি, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    ফেরত;
	}
    }
    যদি (curbuf->b_ml.ml_সারি_গুনতি != uep->ue_আকার)
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
    শূন্য
u_update_save_nr(buf_T *buf)
{
    u_header_T	*uhp;

    ++buf->b_u_save_nr_last;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_last;
    uhp = buf->b_u_curhead;
    যদি (uhp != ফাঁকা)
	uhp = uhp->uh_পরবর্তী.ptr;
    অন্যথা
	uhp = buf->b_u_newhead;
    যদি (uhp != ফাঁকা)
	uhp->uh_save_nr = buf->b_u_save_nr_last;
}

    স্থির শূন্য
u_unch_branch(u_header_T *uhp)
{
    u_header_T	*uh;

    জন্য (uh = uhp; uh != ফাঁকা; uh = uh->uh_পূর্ব.ptr)
    {
	uh->uh_flags |= UH_CHANGED;
	যদি (uh->uh_alt_পরবর্তী.ptr != ফাঁকা)
	    u_unch_branch(uh->uh_alt_পরবর্তী.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    স্থির u_entry_T *
u_get_headentry(শূন্য)
{
    যদি (curbuf->b_u_newhead == ফাঁকা || curbuf->b_u_newhead->uh_entry == ফাঁকা)
    {
	iemsg(e_undo_list_corrupt);
	ফেরত ফাঁকা;
    }
    ফেরত curbuf->b_u_newhead->uh_entry;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    স্থির শূন্য
u_getbot(শূন্য)
{
    u_entry_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    যদি (uep == ফাঁকা)
	ফেরত;

    uep = curbuf->b_u_newhead->uh_getbot_entry;
    যদি (uep != ফাঁকা)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_সারি_গুনতি - uep->ue_lcount;
	uep->ue_bot = uep->ue_শীর্ষ + uep->ue_আকার + 1 + extra;
	যদি (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_সারি_গুনতি)
	{
	    iemsg(e_undo_সারি_missing);
	    uep->ue_bot = uep->ue_শীর্ষ + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_entry = ফাঁকা;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    স্থির শূন্য
u_freeheader(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    যদি (uhp->uh_alt_পরবর্তী.ptr != ফাঁকা)
	u_freebranch(buf, uhp->uh_alt_পরবর্তী.ptr, uhpp);

    যদি (uhp->uh_alt_পূর্ব.ptr != ফাঁকা)
	uhp->uh_alt_পূর্ব.ptr->uh_alt_পরবর্তী.ptr = ফাঁকা;

    // Update the links in the list to remove the header.
    যদি (uhp->uh_পরবর্তী.ptr == ফাঁকা)
	buf->b_u_oldhead = uhp->uh_পূর্ব.ptr;
    অন্যথা
	uhp->uh_পরবর্তী.ptr->uh_পূর্ব.ptr = uhp->uh_পূর্ব.ptr;

    যদি (uhp->uh_পূর্ব.ptr == ফাঁকা)
	buf->b_u_newhead = uhp->uh_পরবর্তী.ptr;
    অন্যথা
	জন্য (uhap = uhp->uh_পূর্ব.ptr; uhap != ফাঁকা;
						 uhap = uhap->uh_alt_পরবর্তী.ptr)
	    uhap->uh_পরবর্তী.ptr = uhp->uh_পরবর্তী.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    স্থির শূন্য
u_freebranch(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *tofree, *পরবর্তী;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    যদি (uhp == buf->b_u_oldhead)
    {
	যখন (buf->b_u_oldhead != ফাঁকা)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	ফেরত;
    }

    যদি (uhp->uh_alt_পূর্ব.ptr != ফাঁকা)
	uhp->uh_alt_পূর্ব.ptr->uh_alt_পরবর্তী.ptr = ফাঁকা;

    পরবর্তী = uhp;
    যখন (পরবর্তী != ফাঁকা)
    {
	tofree = পরবর্তী;
	যদি (tofree->uh_alt_পরবর্তী.ptr != ফাঁকা)
	    u_freebranch(buf, tofree->uh_alt_পরবর্তী.ptr, uhpp);   // recursive
	পরবর্তী = tofree->uh_পূর্ব.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}

/*
 * Free all the undo entries for one header and the header itself.
 * This means that "uhp" is invalid when returning.
 */
    স্থির শূন্য
u_freeentries(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_entry_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    যদি (buf->b_u_curhead == uhp)
	buf->b_u_curhead = ফাঁকা;
    যদি (buf->b_u_newhead == uhp)
	buf->b_u_newhead = ফাঁকা;  // freeing the newest entry
    যদি (uhpp != ফাঁকা && uhp == *uhpp)
	*uhpp = ফাঁকা;

    জন্য (uep = uhp->uh_entry; uep != ফাঁকা; uep = nuep)
    {
	nuep = uep->ue_পরবর্তী;
	u_freeentry(uep, uep->ue_আকার);
    }

#ifdef U_DEBUG
    uhp->uh_magic = 0;
#endif
    vim_free((char_u *)uhp);
    --buf->b_u_numhead;
}

/*
 * free entry 'uep' and 'n' lines in uep->ue_array[]
 */
    স্থির শূন্য
u_freeentry(u_entry_T *uep, long n)
{
    যখন (n > 0)
	vim_free(uep->ue_array[--n].ul_সারি);
    vim_free((char_u *)uep->ue_array);
#ifdef FEAT_PROP_POPUP
    ga_clear(&uep->ue_props);
#endif
#ifdef U_DEBUG
    uep->ue_magic = 0;
#endif
    vim_free((char_u *)uep);
}

/*
 * invalidate the undo buffer; called when storage has already been released
 */
    স্থির শূন্য
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = ফাঁকা;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_সারি_ptr.ul_সারি = ফাঁকা;
    buf->b_u_সারি_ptr.ul_len = 0;
    buf->b_u_সারি_ptr.ul_textlen = 0;
    buf->b_u_সারি_lnum = 0;
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 */
    স্থির শূন্য
u_blockfree(buf_T *buf)
{
    যখন (buf->b_u_oldhead != ফাঁকা)
	u_freeheader(buf, buf->b_u_oldhead, ফাঁকা);
    vim_free(buf->b_u_সারি_ptr.ul_সারি);
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 * and invalidate the undo buffer
 */
    শূন্য
u_clearallandblockfree(buf_T *buf)
{
    u_blockfree(buf);
    u_clearall(buf);
}



/*
 * Save the line "lnum" for the "U" command.
 */
    স্থির শূন্য
u_saveline(linenr_T lnum)
{
    যদি (lnum == curbuf->b_u_সারি_lnum)	    // line is already saved
	ফেরত;
    যদি (lnum < 1 || lnum > curbuf->b_ml.ml_সারি_গুনতি) // should never happen
	ফেরত;
    u_clearline();
    curbuf->b_u_সারি_lnum = lnum;
    যদি (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_সারি_colnr = curwin->w_cursor.col;
    অন্যথা
	curbuf->b_u_সারি_colnr = 0;
    যদি (u_save_সারি(&curbuf->b_u_সারি_ptr, lnum) == FAIL)
	do_outofmem_বার্তা((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    শূন্য
u_clearline(শূন্য)
{
    যদি (curbuf->b_u_সারি_ptr.ul_সারি == ফাঁকা)
	ফেরত;

    VIM_CLEAR(curbuf->b_u_সারি_ptr.ul_সারি);
    curbuf->b_u_সারি_ptr.ul_len = 0;
    curbuf->b_u_সারি_ptr.ul_textlen = 0;
    curbuf->b_u_সারি_lnum = 0;
}

/*
 * Implementation of the "U" command.
 * Differentiation from vi: "U" can be undone with the next "U".
 * We also allow the cursor to be in another line.
 * Careful: may trigger autocommands that reload the buffer.
 */
    শূন্য
u_undoline(শূন্য)
{
    colnr_T	t;
    undoline_T  oldp;

    যদি (undo_off)
	ফেরত;

    যদি (curbuf->b_u_সারি_ptr.ul_সারি == ফাঁকা
			|| curbuf->b_u_সারি_lnum > curbuf->b_ml.ml_সারি_গুনতি)
    {
	beep_flush();
	ফেরত;
    }

    // first save the line for the 'u' command
    যদি (u_savecommon(curbuf->b_u_সারি_lnum - 1,
		       curbuf->b_u_সারি_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	ফেরত;
    যদি (u_save_সারি(&oldp, curbuf->b_u_সারি_lnum) == FAIL)
    {
	do_outofmem_বার্তা((long_u)0);
	ফেরত;
    }
    ml_replace_len(curbuf->b_u_সারি_lnum, curbuf->b_u_সারি_ptr.ul_সারি,
				     curbuf->b_u_সারি_ptr.ul_len, TRUE, FALSE);
    changed_bytes(curbuf->b_u_সারি_lnum, 0);
    curbuf->b_u_সারি_ptr = oldp;

    t = curbuf->b_u_সারি_colnr;
    যদি (curwin->w_cursor.lnum == curbuf->b_u_সারি_lnum)
	curbuf->b_u_সারি_colnr = curwin->w_cursor.col;
    curwin->w_cursor.col = t;
    curwin->w_cursor.lnum = curbuf->b_u_সারি_lnum;
    check_cursor_col();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    পূর্ণসংখ্যা
bufIsChanged(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    যদি (term_job_running_not_none(buf->b_term))
	ফেরত TRUE;
#endif
    ফেরত bufIsChangedNotTerm(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    পূর্ণসংখ্যা
anyBufIsChanged(শূন্য)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	যদি (bufIsChanged(buf))
	    ফেরত TRUE;
    ফেরত FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    পূর্ণসংখ্যা
bufIsChangedNotTerm(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    ফেরত (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_changed || file_ff_differs(buf, TRUE));
}

    পূর্ণসংখ্যা
curbufIsChanged(শূন্য)
{
    ফেরত bufIsChanged(curbuf);
}

#যদি defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    স্থির শূন্য
u_eval_বৃক্ষ(buf_T *buf, u_header_T *first_uhp, list_T *list)
{
    u_header_T  *uhp = first_uhp;
    dict_T	*dict;

    যখন (uhp != ফাঁকা)
    {
	dict = dict_alloc();
	যদি (dict == ফাঁকা)
	    ফেরত;
	dict_add_সংখ্যা(dict, "seq", uhp->uh_seq);
	dict_add_সংখ্যা(dict, "time", (long)uhp->uh_time);
	যদি (uhp == buf->b_u_newhead)
	    dict_add_সংখ্যা(dict, "newhead", 1);
	যদি (uhp == buf->b_u_curhead)
	    dict_add_সংখ্যা(dict, "curhead", 1);
	যদি (uhp->uh_save_nr > 0)
	    dict_add_সংখ্যা(dict, "save", uhp->uh_save_nr);

	যদি (uhp->uh_alt_পরবর্তী.ptr != ফাঁকা)
	{
	    list_T	*alt_list = list_alloc();

	    যদি (alt_list != ফাঁকা)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_বৃক্ষ(buf, uhp->uh_alt_পরবর্তী.ptr, alt_list);
		dict_add_list(dict, "alt", alt_list);
	    }
	}

	list_append_dict(list, dict);
	uhp = uhp->uh_পূর্ব.ptr;
    }
}

/*
 * "undofile(name)" function
 */
    শূন্য
f_undofile(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	যদি (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = ফাঁকা;
	}
	অন্যথা
	{
	    char_u *ffname = FullName_save(fname, TRUE);

	    যদি (ffname != ফাঁকা)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# অন্যথা
    rettv->vval.v_string = ফাঁকা;
# endif
}
# ifdef FEAT_PERSISTENT_UNDO
/*
 * Reset undofile option and delete the undofile
 */
    শূন্য
u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    যদি (!buf->b_p_udf)
	ফেরত;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    যদি (file_name != ফাঁকা)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    set_option_মান_give_err((char_u *)"undofile", 0L, ফাঁকা, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    শূন্য
f_undotree(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_opt_বাফার_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    যদি (buf == ফাঁকা)
	ফেরত;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_সংখ্যা(dict, "synced", (long)buf->b_u_synced);
    dict_add_সংখ্যা(dict, "seq_last", buf->b_u_seq_last);
    dict_add_সংখ্যা(dict, "save_last", buf->b_u_save_nr_last);
    dict_add_সংখ্যা(dict, "seq_cur", buf->b_u_seq_cur);
    dict_add_সংখ্যা(dict, "time_cur", (long)buf->b_u_time_cur);
    dict_add_সংখ্যা(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    যদি (list != ফাঁকা)
    {
	u_eval_বৃক্ষ(buf, buf->b_u_oldhead, list);
	dict_add_list(dict, "entries", list);
    }
}

#endif
