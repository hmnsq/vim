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
#define CRYPT_BUF_SIZE 8192

#ଅନ୍ତର୍ଭାଵ "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    buf_T	*bi_buf;
    FILE	*bi_fp;
#ifdef FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_buffer; // CRYPT_BUF_SIZE, NULL when not buffering
    size_t	bi_used;    // bytes written to/read from bi_buffer
    size_t	bi_avail;   // bytes available in bi_buffer
#endif
} bufinfo_T;


ସ୍ଥିର ଶୂନ୍ୟ u_unch_branch(u_header_T *uhp);
ସ୍ଥିର u_entry_T *u_get_headentry(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ u_getbot(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ u_doit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2);
ସ୍ଥିର ଶୂନ୍ୟ u_undoredo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା undo);
ସ୍ଥିର ଶୂନ୍ୟ u_undo_end(ପୂର୍ଣ୍ଣସଂଖ୍ୟା did_undo, ପୂର୍ଣ୍ଣସଂଖ୍ୟା absolute);
ସ୍ଥିର ଶୂନ୍ୟ u_freeheader(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
ସ୍ଥିର ଶୂନ୍ୟ u_freebranch(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
ସ୍ଥିର ଶୂନ୍ୟ u_freeentries(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
ସ୍ଥିର ଶୂନ୍ୟ u_freeentry(u_entry_T *, long);
#ifdef FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା undo_flush(bufinfo_T *bi);
# endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା undo_read(bufinfo_T *bi, char_u *buffer, size_t size);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା serialize_uep(bufinfo_T *bi, u_entry_T *uep);
ସ୍ଥିର u_entry_T *unserialize_uep(bufinfo_T *bi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *error, char_u *file_name);
ସ୍ଥିର ଶୂନ୍ୟ serialize_pos(bufinfo_T *bi, pos_T pos);
ସ୍ଥିର ଶୂନ୍ୟ unserialize_pos(bufinfo_T *bi, pos_T *pos);
ସ୍ଥିର ଶୂନ୍ୟ serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
ସ୍ଥିର ଶୂନ୍ୟ unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#endif
ସ୍ଥିର ଶୂନ୍ୟ u_saveline(linenr_T lnum);
ସ୍ଥିର ଶୂନ୍ୟ u_blockfree(buf_T *buf);

#define U_ALLOC_ଧାଡ଼ି(size) lalloc(size, FALSE)

// used in undo_end() to report number of added and deleted lines
ସ୍ଥିର long	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	undo_undoes = FALSE;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	lastmark = 0;

#ଯଦି defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା seen_b_u_curhead;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା seen_b_u_newhead;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା header_ଗଣନା;

    ସ୍ଥିର ଶୂନ୍ୟ
u_check_ବୃକ୍ଷ(u_header_T *uhp,
	u_header_T *exp_uh_next,
	u_header_T *exp_uh_alt_prev)
{
    u_entry_T *uep;

    ଯଦି (uhp == ଖାଲି)
	ଫେରନ୍ତୁ;
    ++header_ଗଣନା;
    ଯଦି (uhp == curbuf->b_u_curhead && ++seen_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	ଫେରନ୍ତୁ;
    }
    ଯଦି (uhp == curbuf->b_u_newhead && ++seen_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	ଫେରନ୍ତୁ;
    }

    ଯଦି (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    ଅନ୍ୟଥା
    {
	// Check pointers back are correct.
	ଯଦି (uhp->uh_next.ptr != exp_uh_next)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_next, uhp->uh_next.ptr);
	}
	ଯଦି (uhp->uh_alt_prev.ptr != exp_uh_alt_prev)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_prev, uhp->uh_alt_prev.ptr);
	}

	// Check the undo tree at this header.
	ପାଇଁ (uep = uhp->uh_entry; uep != ଖାଲି; uep = uep->ue_next)
	{
	    ଯଦି (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		ଭାଙ୍ଗ;
	    }
	}

	// Check the next alt tree.
	u_check_ବୃକ୍ଷ(uhp->uh_alt_next.ptr, uhp->uh_next.ptr, uhp);

	// Check the next header in this branch.
	u_check_ବୃକ୍ଷ(uhp->uh_prev.ptr, uhp, ଖାଲି);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
u_check(ପୂର୍ଣ୍ଣସଂଖ୍ୟା newhead_may_be_NULL)
{
    seen_b_u_newhead = 0;
    seen_b_u_curhead = 0;
    header_ଗଣନା = 0;

    u_check_ବୃକ୍ଷ(curbuf->b_u_oldhead, ଖାଲି, ଖାଲି);

    ଯଦି (seen_b_u_newhead == 0 && curbuf->b_u_oldhead != ଖାଲି
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == ଖାଲି))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    ଯଦି (curbuf->b_u_curhead != ଖାଲି && seen_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    ଯଦି (header_ଗଣନା != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (long)header_ଗଣନା, (long)curbuf->b_u_numhead);
    }
}
#endif

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
u_save_cursor(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
u_save(linenr_T ଶୀର୍ଷ, linenr_T bot)
{
    ଯଦି (undo_off)
	ଫେରନ୍ତୁ OK;

    ଯଦି (ଶୀର୍ଷ >= bot || bot > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1)
	ଫେରନ୍ତୁ FAIL;	// rely on caller to give an error message

    ଯଦି (ଶୀର୍ଷ + 2 == bot)
	u_saveline((linenr_T)(ଶୀର୍ଷ + 1));

    ଫେରନ୍ତୁ (u_savecommon(ଶୀର୍ଷ, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
u_savesub(linenr_T lnum)
{
    ଯଦି (undo_off)
	ଫେରନ୍ତୁ OK;

    ଫେରନ୍ତୁ (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
u_inssub(linenr_T lnum)
{
    ଯଦି (undo_off)
	ଫେରନ୍ତୁ OK;

    ଫେରନ୍ତୁ (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
u_savedel(linenr_T lnum, long nlines)
{
    ଯଦି (undo_off)
	ଫେରନ୍ତୁ OK;

    ଫେରନ୍ତୁ (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
undo_allowed(ଶୂନ୍ୟ)
{
    // Don't allow changes when 'modifiable' is off.
    ଯଦି (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ଫେରନ୍ତୁ FALSE;
    }

#ifdef HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    ଯଦି (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ଫେରନ୍ତୁ FALSE;
    }
#endif

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    ଯଦି (textlock != 0)
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	ଫେରନ୍ତୁ FALSE;
    }

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    ସ୍ଥିର long
get_undolevel(ଶୂନ୍ୟ)
{
    ଯଦି (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	ଫେରନ୍ତୁ p_ul;
    ଫେରନ୍ତୁ curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
u_save_ଧାଡ଼ି(undoline_T *ul, linenr_T lnum)
{
    char_u *ଧାଡ଼ି = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    ଯଦି (curbuf->b_ml.ml_ଧାଡ଼ି_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_ଧାଡ଼ି = vim_strsave((char_u *)"");
    }
    ଅନ୍ୟଥା
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_ଧାଡ଼ି_len;
	ul->ul_ଧାଡ଼ି = vim_memsave(ଧାଡ଼ି, ul->ul_len);
    }
    ଫେରନ୍ତୁ ul->ul_ଧାଡ଼ି == ଖାଲି ? FAIL : OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_prop_w_flags(linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    char_u  *props;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    proplen = get_text_props(curbuf, lnum, &props, FALSE);

    ପାଇଁ (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * ଆକାର prop, ଆକାର prop);
	ଯଦି (prop.tp_flags & flags)
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
u_savecommon(
    linenr_T	ଶୀର୍ଷ,
    linenr_T	bot,
    linenr_T	newbot,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		reload)
{
    linenr_T	lnum;
    long	i;
    u_header_T	*uhp;
    u_header_T	*old_curhead;
    u_entry_T	*uep;
    u_entry_T	*prev_uep;
    long	size;

    ଯଦି (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	ଯଦି (!undo_allowed())
	    ଫେରନ୍ତୁ FAIL;

#ifdef FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	ଯଦି (netbeans_active())
	{
	    ଯଦି (netbeans_is_guarded(ଶୀର୍ଷ, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_in_read_only_files));
		ଫେରନ୍ତୁ FAIL;
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
	ଯଦି (bot > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_ଧାଡ଼ି_ଗଣନା_changed_unexpectedly));
	    ଫେରନ୍ତୁ FAIL;
	}
    }

#ifdef U_DEBUG
    u_check(FALSE);
#endif

#ifdef FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    ଯଦି (bot - ଶୀର୍ଷ > 1)
    {
	ଯଦି (ଶୀର୍ଷ > 0 && has_prop_w_flags(ଶୀର୍ଷ + 1, TP_FLAG_CONT_PREV))
	    --ଶୀର୍ଷ;
	ଯଦି (bot <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_NEXT))
	{
	    ++bot;
	    ଯଦି (newbot != 0)
		++newbot;
	}
    }
#endif

    size = bot - ଶୀର୍ଷ - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    ଯଦି (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	ଯଦି (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_ଧାଡ଼ି(ଆକାର(u_header_T));
	    ଯଦି (uhp == ଖାଲି)
		ଯାଅ nomem;
#ifdef U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#endif
	}
	ଅନ୍ୟଥା
	    uhp = ଖାଲି;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	ଯଦି (old_curhead != ଖାଲି)
	{
	    curbuf->b_u_newhead = old_curhead->uh_next.ptr;
	    curbuf->b_u_curhead = ଖାଲି;
	}

	/*
	 * free headers to keep the size right
	 */
	ଯେତେବେଳେ (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != ଖାଲି)
	{
	    u_header_T	    *uhfree = curbuf->b_u_oldhead;

	    ଯଦି (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    ଅନ୍ୟଥା ଯଦି (uhfree->uh_alt_next.ptr == ଖାଲି)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    ଅନ୍ୟଥା
	    {
		// Free the oldest alternate branch as a whole.
		ଯେତେବେଳେ (uhfree->uh_alt_next.ptr != ଖାଲି)
		    uhfree = uhfree->uh_alt_next.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#ifdef U_DEBUG
	    u_check(TRUE);
#endif
	}

	ଯଦି (uhp == ଖାଲି)		// no undo at all
	{
	    ଯଦି (old_curhead != ଖାଲି)
		u_freebranch(curbuf, old_curhead, ଖାଲି);
	    curbuf->b_u_synced = false;
	    ଫେରନ୍ତୁ OK;
	}

	uhp->uh_prev.ptr = ଖାଲି;
	uhp->uh_next.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_next.ptr = old_curhead;
	ଯଦି (old_curhead != ଖାଲି)
	{
	    uhp->uh_alt_prev.ptr = old_curhead->uh_alt_prev.ptr;
	    ଯଦି (uhp->uh_alt_prev.ptr != ଖାଲି)
		uhp->uh_alt_prev.ptr->uh_alt_next.ptr = uhp;
	    old_curhead->uh_alt_prev.ptr = uhp;
	    ଯଦି (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	ଅନ୍ୟଥା
	    uhp->uh_alt_prev.ptr = ଖାଲି;
	ଯଦି (curbuf->b_u_newhead != ଖାଲି)
	    curbuf->b_u_newhead->uh_prev.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_last;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_time = vim_time();
	uhp->uh_save_nr = 0;
	curbuf->b_u_time_cur = uhp->uh_time + 1;

	uhp->uh_walk = 0;
	uhp->uh_entry = ଖାଲି;
	uhp->uh_getbot_entry = ଖାଲି;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	ଯଦି (virtual_active() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	ଅନ୍ୟଥା
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_changed ? UH_CHANGED : 0) +
		       ((curbuf->b_ml.ml_flags & ML_EMPTY) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, ଆକାର(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	ଯଦି (curbuf->b_u_oldhead == ଖାଲି)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (get_undolevel() < 0)	// no undo at all
	    ଫେରନ୍ତୁ OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	ଯଦି (size == 1)
	{
	    uep = u_get_headentry();
	    prev_uep = ଖାଲି;
	    ପାଇଁ (i = 0; i < 10; ++i)
	    {
		ଯଦି (uep == ଖାଲି)
		    ଭାଙ୍ଗ;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		ଯଦି ((curbuf->b_u_newhead->uh_getbot_entry != uep
			    ? (uep->ue_ଶୀର୍ଷ + uep->ue_size + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
			|| (uep->ue_size > 1
			    && ଶୀର୍ଷ >= uep->ue_ଶୀର୍ଷ
			    && ଶୀର୍ଷ + 2 <= uep->ue_ଶୀର୍ଷ + uep->ue_size + 1))
		    ଭାଙ୍ଗ;

		// If it's the same line we can skip saving it again.
		ଯଦି (uep->ue_size == 1 && uep->ue_ଶୀର୍ଷ == ଶୀର୍ଷ)
		{
		    ଯଦି (i > 0)
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
			prev_uep->ue_next = uep->ue_next;
			uep->ue_next = curbuf->b_u_newhead->uh_entry;
			curbuf->b_u_newhead->uh_entry = uep;
		    }

		    // The executed command may change the line count.
		    ଯଦି (newbot != 0)
			uep->ue_bot = newbot;
		    ଅନ୍ୟଥା ଯଦି (bot > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
			uep->ue_bot = 0;
		    ଅନ୍ୟଥା
		    {
			uep->ue_lcount = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
			curbuf->b_u_newhead->uh_getbot_entry = uep;
		    }
		    ଫେରନ୍ତୁ OK;
		}
		prev_uep = uep;
		uep = uep->ue_next;
	    }
	}

	// find line number for ue_bot for previous u_save()
	u_getbot();
    }

    /*
     * add lines in front of entry list
     */
    uep = U_ALLOC_ଧାଡ଼ି(ଆକାର(u_entry_T));
    ଯଦି (uep == ଖାଲି)
	ଯାଅ nomem;
    CLEAR_POINTER(uep);
#ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
#endif

    uep->ue_size = size;
    uep->ue_ଶୀର୍ଷ = ଶୀର୍ଷ;
    ଯଦି (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    ଅନ୍ୟଥା ଯଦି (bot > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	uep->ue_bot = 0;
    ଅନ୍ୟଥା
    {
	uep->ue_lcount = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	curbuf->b_u_newhead->uh_getbot_entry = uep;
    }

    ଯଦି (size > 0)
    {
	ଯଦି ((uep->ue_array = U_ALLOC_ଧାଡ଼ି(ଆକାର(undoline_T) * size)) == ଖାଲି)
	{
	    u_freeentry(uep, 0L);
	    ଯାଅ nomem;
	}
	ପାଇଁ (i = 0, lnum = ଶୀର୍ଷ + 1; i < size; ++i)
	{
	    fast_breakcheck();
	    ଯଦି (got_int)
	    {
		u_freeentry(uep, i);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (u_save_ଧାଡ଼ି(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		ଯାଅ nomem;
	    }
	}
    }
    ଅନ୍ୟଥା
	uep->ue_array = ଖାଲି;
    uep->ue_next = curbuf->b_u_newhead->uh_entry;
    curbuf->b_u_newhead->uh_entry = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#ifdef U_DEBUG
    u_check(FALSE);
#endif
    ଫେରନ୍ତୁ OK;

nomem:
    ସନ୍ଦେଶ_silent = 0;	// must display the prompt
    ଯଦି (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	ଫେରନ୍ତୁ OK;
    }
    do_outofmem_ସନ୍ଦେଶ((long_u)0);
    ଫେରନ୍ତୁ FAIL;
}

#ଯଦି defined(FEAT_PERSISTENT_UNDO)

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
    ଶୂନ୍ୟ
u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;

    sha256_start(&ctx);
    ପାଇଁ (lnum = 1; lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା; ++lnum)
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
    ସ୍ଥିର char_u *
u_get_undo_file_name(char_u *buf_ffname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {ଖାଲି, 0};
    char_u	*undo_file_name = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir_len;
    char_u	*p;
    size_t	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    size_t	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    ଯଦି (ffname == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    ଯଦି (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    ଯେତେବେଳେ (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	ଯଦି (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    ଯଦି (undo_file_name == ଖାଲି)
		ଭାଙ୍ଗ;
	    p = gettail(undo_file_name);
	    plen = (size_t)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# ଅନ୍ୟଥା
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	ଅନ୍ୟଥା
	{
	    dir_name[dir_len] = NUL;
	    ଯଦି (mch_isdir(dir_name))
	    {
		string_T    ret;

		ଯଦି (munged_name.string == ଖାଲି)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    ଯଦି (munged_name.string == ଖାଲି)
			ଫେରନ୍ତୁ ଖାଲି;
		    munged_name.length = ffnamelen;
		    ପାଇଁ (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			ଯଦି (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (size_t)dir_len,
		    munged_name.string, munged_name.length, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	ଯଦି (undo_file_name != ଖାଲି && (!reading
			       || mch_stat((ଅକ୍ଷର *)undo_file_name, &st) >= 0))
	    ଭାଙ୍ଗ;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    ଫେରନ୍ତୁ undo_file_name;
}

    ସ୍ଥିର ଶୂନ୍ୟ
corruption_error(ଅକ୍ଷର *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    ସ୍ଥିର ଶୂନ୍ୟ
u_free_uhp(u_header_T *uhp)
{
    u_entry_T	*nuep;
    u_entry_T	*uep;

    uep = uhp->uh_entry;
    ଯେତେବେଳେ (uep != ଖାଲି)
    {
	nuep = uep->ue_next;
	u_freeentry(uep, uep->ue_size);
	uep = nuep;
    }
    vim_free(uhp);
}

/*
 * Write a sequence of bytes to the undo file.
 * Buffers and encrypts as needed.
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    ଯଦି (bi->bi_buffer != ଖାଲି)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	ଯେତେବେଳେ (bi->bi_used + len_todo >= CRYPT_BUF_SIZE)
	{
	    size_t	n = CRYPT_BUF_SIZE - bi->bi_used;

	    mch_memmove(bi->bi_buffer + bi->bi_used, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_used = CRYPT_BUF_SIZE;
	    ଯଦି (undo_flush(bi) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (len_todo > 0)
	{
	    mch_memmove(bi->bi_buffer + bi->bi_used, p, len_todo);
	    bi->bi_used += len_todo;
	}
	ଫେରନ୍ତୁ OK;
    }
# endif
    ଯଦି (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

# ifdef FEAT_CRYPT
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
undo_flush(bufinfo_T *bi)
{
    ଯଦି (bi->bi_buffer != ଖାଲି && bi->bi_state != ଖାଲି && bi->bi_used > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_used, FALSE);
	ଯଦି (fwrite(bi->bi_buffer, bi->bi_used, (size_t)1, bi->bi_fp) != 1)
	    ଫେରନ୍ତୁ FAIL;
	bi->bi_used = 0;
    }
    ଫେରନ୍ତୁ OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    ଯଦି (bi->bi_state != ଖାଲି && bi->bi_buffer == ଖାଲି)
    {
	// crypting every piece of text separately
	ଯଦି (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	ଅନ୍ୟଥା
	{
	    copy = lalloc(len, FALSE);
	    ଯଦି (copy == ଖାଲି)
		ଫେରନ୍ତୁ 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	ଯଦି (copy != small_buf)
	    vim_free(copy);
	ଫେରନ୍ତୁ i == 1 ? OK : FAIL;
    }
# endif
    ଫେରନ୍ତୁ undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
undo_write_bytes(bufinfo_T *bi, long_u nr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u  buf[8];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    bufi = 0;

    ପାଇଁ (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    ଫେରନ୍ତୁ undo_write(bi, buf, (size_t)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    ସ୍ଥିର ଶୂନ୍ୟ
put_header_ptr(bufinfo_T *bi, u_header_T *uhp)
{
    undo_write_bytes(bi, (long_u)(uhp != ଖାଲି ? uhp->uh_seq : 0), 4);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ଯଦି (bi->bi_buffer != ଖାଲି)
    {
	char_u  buf[4];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n;

	undo_read(bi, buf, (size_t)4);
	n = ((unsigned)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	ଫେରନ୍ତୁ n;
    }
# endif
    ଫେରନ୍ତୁ get4c(bi->bi_fp);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ଯଦି (bi->bi_buffer != ଖାଲି)
    {
	char_u  buf[2];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	ଫେରନ୍ତୁ n;
    }
# endif
    ଫେରନ୍ତୁ get2c(bi->bi_fp);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ଯଦି (bi->bi_buffer != ଖାଲି)
    {
	char_u  buf[1];

	undo_read(bi, buf, (size_t)1);
	ଫେରନ୍ତୁ buf[0];
    }
# endif
    ଫେରନ୍ତୁ getc(bi->bi_fp);
}

    ସ୍ଥିର time_t
undo_read_time(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ଯଦି (bi->bi_buffer != ଖାଲି)
    {
	char_u  buf[8];
	time_t	n = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	undo_read(bi, buf, (size_t)8);
	ପାଇଁ (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	ଫେରନ୍ତୁ n;
    }
# endif
    ଫେରନ୍ତୁ get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
undo_read(bufinfo_T *bi, char_u *buffer, size_t size)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = OK;

# ifdef FEAT_CRYPT
    ଯଦି (bi->bi_buffer != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	size_todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)size;
	char_u	*p = buffer;

	ଯେତେବେଳେ (size_todo > 0)
	{
	    size_t n;

	    ଯଦି (bi->bi_used >= bi->bi_avail)
	    {
		n = fread(bi->bi_buffer, 1, (size_t)CRYPT_BUF_SIZE, bi->bi_fp);
		ଯଦି (n == 0)
		{
		    retval = FAIL;
		    ଭାଙ୍ଗ;
		}
		bi->bi_avail = n;
		bi->bi_used = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_avail, FALSE);
	    }
	    n = size_todo;
	    ଯଦି (n > bi->bi_avail - bi->bi_used)
		n = bi->bi_avail - bi->bi_used;
	    mch_memmove(p, bi->bi_buffer + bi->bi_used, n);
	    bi->bi_used += n;
	    size_todo -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n;
	    p += n;
	}
    }
    ଅନ୍ୟଥା
# endif
    ଯଦି (fread(buffer, size, 1, bi->bi_fp) != 1)
	retval = FAIL;

    ଯଦି (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(buffer, 0, size);
    ଫେରନ୍ତୁ retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    ସ୍ଥିର char_u *
read_string_decrypt(bufinfo_T *bi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u  *ptr = alloc(len + 1);

    ଯଦି (ptr == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    ଯଦି (bi->bi_state != ଖାଲି && bi->bi_buffer == ଖାଲି)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    ଫେରନ୍ତୁ ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
serialize_header(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	time_buf[8];

    // Start writing, first the magic marker and undo info version.
    ଯଦି (fwrite(UF_START_MAGIC, (size_t)UF_START_MAGIC_LEN, (size_t)1, fp) != 1)
	ଫେରନ୍ତୁ FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    ଯଦି (*buf->b_p_ଚାବି != NUL)
    {
	char_u *header;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା    header_len;
	long	len;

	undo_write_bytes(bi, (long_u)UF_VERSION_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_method_nr(buf),
					  buf->b_p_ଚାବି, &header, &header_len);
	ଯଦି (bi->bi_state == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	len = (long)fwrite(header, (size_t)header_len, (size_t)1, fp);
	vim_free(header);
	ଯଦି (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = ଖାଲି;
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (crypt_whole_undofile(crypt_get_method_nr(buf)))
	{
	    bi->bi_buffer = alloc(CRYPT_BUF_SIZE);
	    ଯଦି (bi->bi_buffer == ଖାଲି)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = ଖାଲି;
		ଫେରନ୍ତୁ FAIL;
	    }
	    bi->bi_used = 0;
	}
    }
    ଅନ୍ୟଥା
# endif
	undo_write_bytes(bi, (long_u)UF_VERSION, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    ଯଦି (undo_write(bi, hash, (size_t)UNDO_HASH_SIZE) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // buffer-specific data
    undo_write_bytes(bi, (long_u)buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_ଧାଡ଼ି_ptr.ul_textlen, 4);
    ଯଦି (buf->b_u_ଧାଡ଼ି_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି,
			(size_t)buf->b_u_ଧାଡ଼ି_ptr.ul_textlen) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    undo_write_bytes(bi, (long_u)buf->b_u_ଧାଡ଼ି_lnum, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_ଧାଡ଼ି_colnr, 4);

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

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
serialize_uhp(bufinfo_T *bi, u_header_T *uhp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    u_entry_T	*uep;
    char_u	time_buf[8];

    ଯଦି (undo_write_bytes(bi, (long_u)UF_HEADER_MAGIC, 2) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    put_header_ptr(bi, uhp->uh_next.ptr);
    put_header_ptr(bi, uhp->uh_prev.ptr);
    put_header_ptr(bi, uhp->uh_alt_next.ptr);
    put_header_ptr(bi, uhp->uh_alt_prev.ptr);
    undo_write_bytes(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_bytes(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_bytes(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    ପାଇଁ (i = 0; i < NMARKS; ++i)
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
    ପାଇଁ (uep = uhp->uh_entry; uep != ଖାଲି; uep = uep->ue_next)
    {
	undo_write_bytes(bi, (long_u)UF_ENTRY_MAGIC, 2);
	ଯଦି (serialize_uep(bi, uep) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    undo_write_bytes(bi, (long_u)UF_ENTRY_END_MAGIC, 2);
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର u_header_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_header_T	*uhp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    u_entry_T	*uep, *last_uep;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error;

    uhp = U_ALLOC_ଧାଡ଼ି(ଆକାର(u_header_T));
    ଯଦି (uhp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    CLEAR_POINTER(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_next.seq = undo_read_4c(bi);
    uhp->uh_prev.seq = undo_read_4c(bi);
    uhp->uh_alt_next.seq = undo_read_4c(bi);
    uhp->uh_alt_prev.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    ଯଦି (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    ପାଇଁ (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_time = undo_read_time(bi);

    // Optional fields.
    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = undo_read_byte(bi);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା what;

	ଯଦି (len == EOF)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି (len == 0)
	    ଭାଙ୍ଗ;
	what = undo_read_byte(bi);
	ବାଛ (what)
	{
	    ସ୍ଥିତି UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		// field not supported, skip
		ଯେତେବେଳେ (--len >= 0)
		    (ଶୂନ୍ୟ)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    last_uep = ଖାଲି;
    ଯେତେବେଳେ ((c = undo_read_2c(bi)) == UF_ENTRY_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	ଯଦି (last_uep == ଖାଲି)
	    uhp->uh_entry = uep;
	ଅନ୍ୟଥା
	    last_uep->ue_next = uep;
	last_uep = uep;
	ଯଦି (uep == ଖାଲି || error)
	{
	    u_free_uhp(uhp);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଯଦି (c != UF_ENTRY_END_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ uhp;
}

/*
 * Serialize "uep".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
serialize_uep(
    bufinfo_T	*bi,
    u_entry_T	*uep)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    undo_write_bytes(bi, (long_u)uep->ue_ଶୀର୍ଷ, 4);
    undo_write_bytes(bi, (long_u)uep->ue_bot, 4);
    undo_write_bytes(bi, (long_u)uep->ue_lcount, 4);
    undo_write_bytes(bi, (long_u)uep->ue_size, 4);
    ପାଇଁ (i = 0; i < uep->ue_size; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	ଯଦି (undo_write_bytes(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_ଧାଡ଼ି, uep->ue_array[i].ul_textlen) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର u_entry_T *
unserialize_uep(bufinfo_T *bi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *error, char_u *file_name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    u_entry_T	*uep;
    undoline_T	*array = ଖାଲି;
    char_u	*ଧାଡ଼ି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_len;

    uep = U_ALLOC_ଧାଡ଼ି(ଆକାର(u_entry_T));
    ଯଦି (uep == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    CLEAR_POINTER(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_ଶୀର୍ଷ = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_size = undo_read_4c(bi);
    ଯଦି (uep->ue_size > 0)
    {
	ଯଦି (uep->ue_size < LONG_MAX / (long)ଆକାର(undoline_T))
	    array = U_ALLOC_ଧାଡ଼ି(ଆକାର(undoline_T) * uep->ue_size);
	ଯଦି (array == ଖାଲି)
	{
	    *error = TRUE;
	    ଫେରନ୍ତୁ uep;
	}
	vim_memset(array, 0, ଆକାର(undoline_T) * uep->ue_size);
    }
    uep->ue_array = array;

    ପାଇଁ (i = 0; i < uep->ue_size; ++i)
    {
	ଧାଡ଼ି_len = undo_read_4c(bi);
	ଯଦି (ଧାଡ଼ି_len >= 0)
	    ଧାଡ଼ି = read_string_decrypt(bi, ଧାଡ଼ି_len);
	ଅନ୍ୟଥା
	{
	    ଧାଡ଼ି = ଖାଲି;
	    corruption_error("line length", file_name);
	}
	ଯଦି (ଧାଡ଼ି == ଖାଲି)
	{
	    *error = TRUE;
	    ଫେରନ୍ତୁ uep;
	}
	array[i].ul_ଧାଡ଼ି = ଧାଡ଼ି;
	array[i].ul_len = ଧାଡ଼ି_len + 1;
	array[i].ul_textlen = ଧାଡ଼ି_len;
    }
    ଫେରନ୍ତୁ uep;
}

/*
 * Serialize "pos".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_bytes(bi, (long_u)pos.lnum, 4);
    undo_write_bytes(bi, (long_u)pos.col, 4);
    undo_write_bytes(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    ଯଦି (pos->lnum < 0)
	pos->lnum = 0;
    pos->col = undo_read_4c(bi);
    ଯଦି (pos->col < 0)
	pos->col = 0;
    pos->coladd = undo_read_4c(bi);
    ଯଦି (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
u_write_undo(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_header_T	*uhp;
    char_u	*file_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mark;
# ifdef U_DEBUG
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		headers_written = 0;
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd;
    FILE	*fp = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		perm;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		write_ok = FALSE;
# ifdef UNIX
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);

    ଯଦି (name == ଖାଲି)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	ଯଦି (file_name == ଖାଲି)
	{
	    ଯଦି (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    ଯଦି (buf->b_ffname != ଖାଲି)
    {
# ifdef UNIX
	ଯଦି (mch_stat((ଅକ୍ଷର *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_mode;
	    st_old_valid = TRUE;
	}
# ଅନ୍ୟଥା
	perm = mch_getperm(buf->b_ffname);
	ଯଦି (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    ଯଦି (mch_getperm(file_name) >= 0)
    {
	ଯଦି (name == ଖାଲି || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((ଅକ୍ଷର *)file_name, O_RDONLY|O_EXTRA, 0);
	    ଯଦି (fd < 0)
	    {
		ଯଦି (name != ଖାଲି || p_verbose > 0)
		{
		    ଯଦି (name == ଖାଲି)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    ଯଦି (name == ଖାଲି)
			verbose_leave();
		}
		ଯାଅ theend;
	    }
	    ଅନ୍ୟଥା
	    {
		char_u	mbuf[UF_START_MAGIC_LEN];
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;

		len = read_eintr(fd, mbuf, UF_START_MAGIC_LEN);
		close(fd);
		ଯଦି (len < UF_START_MAGIC_LEN
		      || memcmp(mbuf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
		{
		    ଯଦି (name != ଖାଲି || p_verbose > 0)
		    {
			ଯଦି (name == ଖାଲି)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			ଯଦି (name == ଖାଲି)
			    verbose_leave();
		    }
		    ଯାଅ theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    ଯଦି (buf->b_u_numhead == 0 && buf->b_u_ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି == ଖାଲି)
    {
	ଯଦି (p_verbose > 0)
	    verb_ସନ୍ଦେଶ(_("Skipping undo file write, nothing to undo"));
	ଯାଅ theend;
    }

    fd = mch_open((ଅକ୍ଷର *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    ଯଦି (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	ଯାଅ theend;
    }
    (ଶୂନ୍ୟ)mch_setperm(file_name, perm);
    ଯଦି (p_verbose > 0)
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
    ଯଦି (st_old_valid
	    && mch_stat((ଅକ୍ଷର *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  ଯଦି defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    ଯଦି (buf->b_ffname != ଖାଲି)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    ଯଦି (fp == ଖାଲି)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	close(fd);
	mch_remove(file_name);
	ଯାଅ theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    ଯଦି (serialize_header(&bi, hash) == FAIL)
	ଯାଅ write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    ଯେତେବେଳେ (uhp != ଖାଲି)
    {
	// Serialize current UHP if we haven't seen it
	ଯଦି (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++headers_written;
# endif
	    ଯଦି (serialize_uhp(&bi, uhp) == FAIL)
		ଯାଅ write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	ଯଦି (uhp->uh_prev.ptr != ଖାଲି && uhp->uh_prev.ptr->uh_walk != mark)
	    uhp = uhp->uh_prev.ptr;
	ଅନ୍ୟଥା ଯଦି (uhp->uh_alt_next.ptr != ଖାଲି
				     && uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;
	ଅନ୍ୟଥା ଯଦି (uhp->uh_next.ptr != ଖାଲି && uhp->uh_alt_prev.ptr == ଖାଲି
					 && uhp->uh_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_next.ptr;
	ଅନ୍ୟଥା ଯଦି (uhp->uh_alt_prev.ptr != ଖାଲି)
	    uhp = uhp->uh_alt_prev.ptr;
	ଅନ୍ୟଥା
	    uhp = uhp->uh_next.ptr;
    }

    ଯଦି (undo_write_bytes(&bi, (long_u)UF_HEADER_END_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    ଯଦି (headers_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", headers_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    ଯଦି (bi.bi_state != ଖାଲି && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# ଯଦି defined(UNIX) && defined(HAVE_FSYNC)
    ଯଦି ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && fflush(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    fclose(fp);
    ଯଦି (!write_ok)
	semsg(_(e_write_error_in_undo_file_str), file_name);

# ଯଦି defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    ଯଦି (buf->b_ffname != ଖାଲି)
	(ଶୂନ୍ୟ)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    ଯଦି (buf->b_ffname != ଖାଲି)
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
    ଯଦି (bi.bi_state != ଖାଲି)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
# endif
    ଯଦି (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
uhp_seq_cmp(ଅଚଳ ଶୂନ୍ୟ *v1, ଅଚଳ ଶୂନ୍ୟ *v2)
{
    ଅଚଳ u_header_T *u1 = *(u_header_T **)v1;
    ଅଚଳ u_header_T *u2 = *(u_header_T **)v2;

    ଫେରନ୍ତୁ u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    ସ୍ଥିର long
uhp_table_find(u_header_T **uhp_table, long ଅଙ୍କ_head, long seq)
{
    long    lo = 0;
    long    hi = ଅଙ୍କ_head - 1;

    ଯେତେବେଳେ (lo <= hi)
    {
	long mid = lo + (hi - lo) / 2;

	ଯଦି (uhp_table[mid]->uh_seq < seq)
	    lo = mid + 1;
	ଅନ୍ୟଥା ଯଦି (uhp_table[mid]->uh_seq > seq)
	    hi = mid - 1;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ mid;
    }
    ଫେରନ୍ତୁ -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    ଶୂନ୍ୟ
u_read_undo(char_u *name, char_u *hash, char_u *orig_name UNUSED)
{
    char_u	*file_name;
    FILE	*fp;
    long	version, str_len;
    undoline_T	ଧାଡ଼ି_ptr;
    linenr_T	ଧାଡ଼ି_lnum;
    colnr_T	ଧାଡ଼ି_colnr;
    linenr_T	ଧାଡ଼ି_ଗଣନା;
    long	ଅଙ୍କ_head = 0;
    long	old_header_seq, new_header_seq, cur_header_seq;
    long	seq_last, seq_cur;
    long	last_save_nr = 0;
    long	old_idx = -1, new_idx = -1, cur_idx = -1;
    long	ଅଙ୍କ_read_uhps = 0;
    time_t	seq_time;
    long	i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;
    u_header_T	*uhp;
    u_header_T	**uhp_table = ଖାଲି;
    char_u	read_hash[UNDO_HASH_SIZE];
    char_u	magic_buf[UF_START_MAGIC_LEN];
# ifdef U_DEBUG
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*uhp_table_used;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);
    ଧାଡ଼ି_ptr.ul_len = 0;
    ଧାଡ଼ି_ptr.ul_textlen = 0;
    ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି = ଖାଲି;

    ଯଦି (name == ଖାଲି)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	ଯଦି (file_name == ଖାଲି)
	    ଫେରନ୍ତୁ;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	ଯଦି (mch_stat((ଅକ୍ଷର *)orig_name, &st_orig) >= 0
		&& mch_stat((ଅକ୍ଷର *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    ଯଦି (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    ଫେରନ୍ତୁ;
	}
# endif
    }
    ଅନ୍ୟଥା
	file_name = name;

    ଯଦି (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((ଅକ୍ଷର *)file_name, "r");
    ଯଦି (fp == ଖାଲି)
    {
	ଯଦି (name != ଖାଲି || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	ଯାଅ error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    ଯଦି (fread(magic_buf, UF_START_MAGIC_LEN, 1, fp) != 1
		|| memcmp(magic_buf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	ଯାଅ error;
    }
    version = get2c(fp);
    ଯଦି (version == UF_VERSION_CRYPT)
    {
# ifdef FEAT_CRYPT
	ଯଦି (*curbuf->b_p_ଚାବି == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    ଯାଅ error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_ଚାବି);
	ଯଦି (bi.bi_state == ଖାଲି)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    ଯାଅ error;
	}
	ଯଦି (crypt_whole_undofile(bi.bi_state->method_nr))
	{
	    bi.bi_buffer = alloc(CRYPT_BUF_SIZE);
	    ଯଦି (bi.bi_buffer == ଖାଲି)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = ଖାଲି;
		ଯାଅ error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_used = 0;
	}
# ଅନ୍ୟଥା
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	ଯାଅ error;
# endif
    }
    ଅନ୍ୟଥା ଯଦି (version != UF_VERSION)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	ଯାଅ error;
    }

    ଯଦି (undo_read(&bi, read_hash, (size_t)UNDO_HASH_SIZE) == FAIL)
    {
	corruption_error("hash", file_name);
	ଯାଅ error;
    }
    ଧାଡ଼ି_ଗଣନା = (linenr_T)undo_read_4c(&bi);
    ଯଦି (memcmp(hash, read_hash, UNDO_HASH_SIZE) != 0
				  || ଧାଡ଼ି_ଗଣନା != curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	ଯଦି (p_verbose > 0 || name != ଖାଲି)
	{
	    ଯଦି (name == ଖାଲି)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    ଯଦି (name == ଖାଲି)
		verbose_leave();
	}
	ଯାଅ error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    ଯଦି (str_len < 0)
	ଯାଅ error;
    ଯଦି (str_len > 0)
    {
	ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି = read_string_decrypt(&bi, str_len);
	ଧାଡ଼ି_ptr.ul_len = str_len + 1;
	ଧାଡ଼ି_ptr.ul_textlen = str_len;
    }
    ଧାଡ଼ି_lnum = (linenr_T)undo_read_4c(&bi);
    ଧାଡ଼ି_colnr = (colnr_T)undo_read_4c(&bi);
    ଯଦି (ଧାଡ଼ି_lnum < 0 || ଧାଡ଼ି_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	ଯାଅ error;
    }

    // Begin general undo data
    old_header_seq = undo_read_4c(&bi);
    new_header_seq = undo_read_4c(&bi);
    cur_header_seq = undo_read_4c(&bi);
    ଅଙ୍କ_head = undo_read_4c(&bi);
    seq_last = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_time = undo_read_time(&bi);

    // Optional header fields.
    ପାଇଁ (;;)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = undo_read_byte(&bi);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା what;

	ଯଦି (len == 0 || len == EOF)
	    ଭାଙ୍ଗ;
	what = undo_read_byte(&bi);
	ବାଛ (what)
	{
	    ସ୍ଥିତି UF_LAST_SAVE_NR:
		last_save_nr = undo_read_4c(&bi);
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		// field not supported, skip
		ଯେତେବେଳେ (--len >= 0)
		    (ଶୂନ୍ୟ)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    ଯଦି (ଅଙ୍କ_head > 0)
    {
	ଯଦି (ଅଙ୍କ_head < LONG_MAX / (long)ଆକାର(u_header_T *))
	    uhp_table = U_ALLOC_ଧାଡ଼ି(ଅଙ୍କ_head * ଆକାର(u_header_T *));
	ଯଦି (uhp_table == ଖାଲି)
	    ଯାଅ error;
    }

    ଯେତେବେଳେ ((c = undo_read_2c(&bi)) == UF_HEADER_MAGIC)
    {
	ଯଦି (ଅଙ୍କ_read_uhps >= ଅଙ୍କ_head)
	{
	    corruption_error("num_head too small", file_name);
	    ଯାଅ error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	ଯଦି (uhp == ଖାଲି)
	    ଯାଅ error;
	uhp_table[ଅଙ୍କ_read_uhps++] = uhp;
    }

    ଯଦି (ଅଙ୍କ_read_uhps != ଅଙ୍କ_head)
    {
	corruption_error("num_head", file_name);
	ଯାଅ error;
    }
    ଯଦି (c != UF_HEADER_END_MAGIC)
    {
	corruption_error("end marker", file_name);
	ଯାଅ error;
    }

# ifdef U_DEBUG
    uhp_table_used = alloc_clear(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * ଅଙ୍କ_head + 1);
#  define SET_FLAG(j) ++uhp_table_used[j]
# ଅନ୍ୟଥା
#  define SET_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    ଯଦି (ଅଙ୍କ_head > 0)
	qsort(uhp_table, (size_t)ଅଙ୍କ_head, ଆକାର(u_header_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    ପାଇଁ (i = 0; i < ଅଙ୍କ_head - 1; i++)
    {
	ଯଦି (uhp_table[i]->uh_seq == uhp_table[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    ଯାଅ error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    କର { \
	long fidx = uhp_table_find(uhp_table, ଅଙ୍କ_head, (link).seq); \
	\
	ଯଦି (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_table[fidx]; \
	    SET_FLAG(fidx); \
	} \
	ଅନ୍ୟଥା \
	    (link).ptr = ଖାଲି; \
    } ଯେତେବେଳେ (0)

    ପାଇଁ (i = 0; i < ଅଙ୍କ_head; i++)
    {
	uhp = uhp_table[i];
	SWIZZLE_SEQ(uhp->uh_next, i);
	SWIZZLE_SEQ(uhp->uh_prev, i);
	SWIZZLE_SEQ(uhp->uh_alt_next, i);
	SWIZZLE_SEQ(uhp->uh_alt_prev, i);
	ଯଦି (old_header_seq > 0 && old_idx < 0 && uhp->uh_seq == old_header_seq)
	{
	    old_idx = i;
	    SET_FLAG(i);
	}
	ଯଦି (new_header_seq > 0 && new_idx < 0 && uhp->uh_seq == new_header_seq)
	{
	    new_idx = i;
	    SET_FLAG(i);
	}
	ଯଦି (cur_header_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_header_seq)
	{
	    cur_idx = i;
	    SET_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? ଖାଲି : uhp_table[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? ଖାଲି : uhp_table[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? ଖାଲି : uhp_table[cur_idx];
    curbuf->b_u_ଧାଡ଼ି_ptr = ଧାଡ଼ି_ptr;
    curbuf->b_u_ଧାଡ଼ି_lnum = ଧାଡ଼ି_lnum;
    curbuf->b_u_ଧାଡ଼ି_colnr = ଧାଡ଼ି_colnr;
    curbuf->b_u_numhead = ଅଙ୍କ_head;
    curbuf->b_u_seq_last = seq_last;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_time_cur = seq_time;
    curbuf->b_u_save_nr_last = last_save_nr;
    curbuf->b_u_save_nr_cur = last_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_table);

# ifdef U_DEBUG
    ପାଇଁ (i = 0; i < ଅଙ୍କ_head; ++i)
	ଯଦି (uhp_table_used[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_table_used);
    u_check(TRUE);
# endif

    ଯଦି (name != ଖାଲି)
	smsg(_("Finished reading undo file %s"), file_name);
    ଯାଅ theend;

error:
    vim_free(ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି);
    ଯଦି (uhp_table != ଖାଲି)
    {
	ପାଇଁ (i = 0; i < ଅଙ୍କ_read_uhps; i++)
	    ଯଦି (uhp_table[i] != ଖାଲି)
		u_free_uhp(uhp_table[i]);
	vim_free(uhp_table);
    }

theend:
# ifdef FEAT_CRYPT
    ଯଦି (bi.bi_state != ଖାଲି)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
# endif
    ଯଦି (fp != ଖାଲି)
	fclose(fp);
    ଯଦି (file_name != name)
	vim_free(file_name);
    ଫେରନ୍ତୁ;
}

#endif // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    ଶୂନ୍ୟ
u_undo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    ଯଦି (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	ଗଣନା_2 = 1;
    }

    ଯଦି (vim_strchr(p_cpo, CPO_UNDO) == ଖାଲି)
	undo_undoes = TRUE;
    ଅନ୍ୟଥା
	undo_undoes = !undo_undoes;
    u_doit(ଗଣନା_2);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    ଶୂନ୍ୟ
u_redo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    ଯଦି (vim_strchr(p_cpo, CPO_UNDO) == ଖାଲି)
	undo_undoes = FALSE;
    u_doit(ଗଣନା_2);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
u_doit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା startcount)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = startcount;

    ଯଦି (!undo_allowed())
	ଫେରନ୍ତୁ;

    u_newcount = 0;
    u_oldcount = 0;
    ଯଦି (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;
    ଯେତେବେଳେ (ଗଣନା_2--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	ଯଦି (undo_undoes)
	{
	    ଯଦି (curbuf->b_u_curhead == ଖାଲି)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    ଅନ୍ୟଥା ଯଦି (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_next.ptr;
	    // nothing to undo
	    ଯଦି (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == ଖାଲି)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		ଯଦି (ଗଣନା_2 == startcount - 1)
		{
		    ଯଦି (!shortmess(SHM_UNDO))
			ସନ୍ଦେଶ(_("Already at oldest change"));
		    ଫେରନ୍ତୁ;
		}
		ଭାଙ୍ଗ;
	    }

	    u_undoredo(TRUE);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (curbuf->b_u_curhead == ଖାଲି || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		ଯଦି (ଗଣନା_2 == startcount - 1)
		{
		    ଯଦି (!shortmess(SHM_UNDO))
			ସନ୍ଦେଶ(_("Already at newest change"));
		    ଫେରନ୍ତୁ;
		}
		ଭାଙ୍ଗ;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    ଯଦି (curbuf->b_u_curhead->uh_prev.ptr == ଖାଲି)
		curbuf->b_u_newhead = curbuf->b_u_curhead;
	    curbuf->b_u_curhead = curbuf->b_u_curhead->uh_prev.ptr;
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
    ଶୂନ୍ୟ
undo_time(
    long	step,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sec,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		file,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		absolute)
{
    long	    target;
    long	    closest;
    long	    closest_start;
    long	    closest_seq = 0;
    long	    val;
    u_header_T	    *uhp = ଖାଲି;
    u_header_T	    *last;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    mark;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    nomark = 0;  // shut up compiler
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    round;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    dosec = sec;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    dofile = file;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    above = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    did_undo = TRUE;

    ଯଦି (text_locked())
    {
	text_locked_ସନ୍ଦେଶ();
	ଫେରନ୍ତୁ;
    }

    // First make sure the current undoable change is synced.
    ଯଦି (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    ଯଦି (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    ଯଦି (absolute)
    {
	target = step;
	closest = -1;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (dosec)
	    target = (long)(curbuf->b_u_time_cur) + step;
	ଅନ୍ୟଥା ଯଦି (dofile)
	{
	    ଯଦି (step < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		ଯଦି (uhp != ଖାଲି)
		    uhp = uhp->uh_next.ptr;
		ଅନ୍ୟଥା
		    uhp = curbuf->b_u_newhead;
		ଯଦି (uhp != ଖାଲି && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    target = curbuf->b_u_save_nr_cur + step;
		ଅନ୍ୟଥା
		    // count the changes since the last write as one step
		    target = curbuf->b_u_save_nr_cur + step + 1;
		ଯଦି (target <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    ଅନ୍ୟଥା
	    {
		// Moving forward to a newer write.
		target = curbuf->b_u_save_nr_cur + step;
		ଯଦି (target > curbuf->b_u_save_nr_last)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    target = curbuf->b_u_seq_last + 1;
		    dofile = FALSE;
		}
	    }
	}
	ଅନ୍ୟଥା
	    target = curbuf->b_u_seq_cur + step;
	ଯଦି (step < 0)
	{
	    ଯଦି (target < 0)
		target = 0;
	    closest = -1;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (dosec)
		closest = (long)(vim_time() + 1);
	    ଅନ୍ୟଥା ଯଦି (dofile)
		closest = curbuf->b_u_save_nr_last + 2;
	    ଅନ୍ୟଥା
		closest = curbuf->b_u_seq_last + 2;
	    ଯଦି (target >= closest)
		target = closest - 1;
	}
    }
    closest_start = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    ଯଦି (target == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	ଯାଅ target_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    ପାଇଁ (round = 1; round <= 2; ++round)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	ଯଦି (curbuf->b_u_curhead == ଖାଲି)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	ଅନ୍ୟଥା
	    uhp = curbuf->b_u_curhead;

	ଯେତେବେଳେ (uhp != ଖାଲି)
	{
	    uhp->uh_walk = mark;
	    ଯଦି (dosec)
		val = (long)(uhp->uh_time);
	    ଅନ୍ୟଥା ଯଦି (dofile)
		val = uhp->uh_save_nr;
	    ଅନ୍ୟଥା
		val = uhp->uh_seq;

	    ଯଦି (round == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		ଯଦି ((step < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
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
	    ଯଦି (target == val && !dosec)
	    {
		target = uhp->uh_seq;
		ଭାଙ୍ଗ;
	    }

	    // go down in the tree if we haven't been there
	    ଯଦି (uhp->uh_prev.ptr != ଖାଲି && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
		uhp = uhp->uh_prev.ptr;

	    // go to alternate branch if we haven't been there
	    ଅନ୍ୟଥା ଯଦି (uhp->uh_alt_next.ptr != ଖାଲି
		    && uhp->uh_alt_next.ptr->uh_walk != nomark
		    && uhp->uh_alt_next.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_next.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    ଅନ୍ୟଥା ଯଦି (uhp->uh_next.ptr != ଖାଲି && uhp->uh_alt_prev.ptr == ଖାଲି
		    && uhp->uh_next.ptr->uh_walk != nomark
		    && uhp->uh_next.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		ଯଦି (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_next.ptr;
	    }

	    ଅନ୍ୟଥା
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		ଯଦି (uhp->uh_alt_prev.ptr != ଖାଲି)
		    uhp = uhp->uh_alt_prev.ptr;
		ଅନ୍ୟଥା
		    uhp = uhp->uh_next.ptr;
	    }
	}

	ଯଦି (uhp != ଖାଲି)    // found it
	    ଭାଙ୍ଗ;

	ଯଦି (absolute)
	{
	    semsg(_(e_undo_ସଂଖ୍ୟା_nr_not_found), step);
	    ଫେରନ୍ତୁ;
	}

	ଯଦି (closest == closest_start)
	{
	    ଯଦି (!shortmess(SHM_UNDO))
	    {
		ଯଦି (step < 0)
		    ସନ୍ଦେଶ(_("Already at oldest change"));
		ଅନ୍ୟଥା
		    ସନ୍ଦେଶ(_("Already at newest change"));
	    }
	    ଫେରନ୍ତୁ;
	}

	target = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	ଯଦି (step < 0)
	    above = TRUE;	// stop above the header
    }

target_zero:
    // If we found it: Follow the path to go to where we want to be.
    ଯଦି (uhp != ଖାଲି || target == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	ଯେତେବେଳେ (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    ଯଦି (uhp == ଖାଲି)
		uhp = curbuf->b_u_newhead;
	    ଅନ୍ୟଥା
		uhp = uhp->uh_next.ptr;
	    ଯଦି (uhp == ଖାଲି || (target > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == target && !above))
		ଭାଙ୍ଗ;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    ଯଦି (target > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	ଯଦି (target > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    ଯେତେବେଳେ (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		ଯଦି (uhp == ଖାଲି)
		    ଭାଙ୍ଗ;

		// Go back to the first branch with a mark.
		ଯେତେବେଳେ (uhp->uh_alt_prev.ptr != ଖାଲି
				     && uhp->uh_alt_prev.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_prev.ptr;

		// Find the last branch with a mark, that's the one.
		last = uhp;
		ଯେତେବେଳେ (last->uh_alt_next.ptr != ଖାଲି
				    && last->uh_alt_next.ptr->uh_walk == mark)
		    last = last->uh_alt_next.ptr;
		ଯଦି (last != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    ଯେତେବେଳେ (uhp->uh_alt_prev.ptr != ଖାଲି)
			uhp = uhp->uh_alt_prev.ptr;
		    ଯଦି (last->uh_alt_next.ptr != ଖାଲି)
			last->uh_alt_next.ptr->uh_alt_prev.ptr =
							last->uh_alt_prev.ptr;
		    last->uh_alt_prev.ptr->uh_alt_next.ptr =
							last->uh_alt_next.ptr;
		    last->uh_alt_prev.ptr = ଖାଲି;
		    last->uh_alt_next.ptr = uhp;
		    uhp->uh_alt_prev.ptr = last;

		    ଯଦି (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = last;
		    uhp = last;
		    ଯଦି (uhp->uh_next.ptr != ଖାଲି)
			uhp->uh_next.ptr->uh_prev.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		ଯଦି (uhp->uh_walk != mark)
		    ଭାଙ୍ଗ;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		ଯଦି (uhp->uh_seq == target && above)
		{
		    curbuf->b_u_seq_cur = target - 1;
		    ଭାଙ୍ଗ;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		ଯଦି (uhp->uh_prev.ptr == ଖାଲି)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_prev.ptr;
		did_undo = FALSE;

		ଯଦି (uhp->uh_seq == target)	// found it!
		    ଭାଙ୍ଗ;

		uhp = uhp->uh_prev.ptr;
		ଯଦି (uhp == ଖାଲି || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    internal_error("undo_time()");
		    ଭାଙ୍ଗ;
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
    ସ୍ଥିର ଶୂନ୍ୟ
u_undoredo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା undo)
{
    undoline_T	*newarray = ଖାଲି;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	ଶୀର୍ଷ, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    long	i;
    u_entry_T	*uep, *nuep;
    u_entry_T	*newlist = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty_buffer;		    // buffer became empty
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
    mch_memmove(namedm, curbuf->b_namedm, ଆକାର(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_start.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    curbuf->b_op_start.col = 0;
    curbuf->b_op_end.lnum = 0;
    curbuf->b_op_end.col = 0;

    ପାଇଁ (uep = curhead->uh_entry; uep != ଖାଲି; uep = nuep)
    {
	ଶୀର୍ଷ = uep->ue_ଶୀର୍ଷ;
	bot = uep->ue_bot;
	ଯଦି (bot == 0)
	    bot = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1;
	ଯଦି (ଶୀର୍ଷ > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା || ଶୀର୍ଷ >= bot
				      || bot > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_ଧାଡ଼ି_numbers_wrong);
	    changed();		// don't want UNCHANGED now
	    ଫେରନ୍ତୁ;
	}

	oldsize = bot - ଶୀର୍ଷ - 1;    // number of lines before undo
	newsize = uep->ue_size;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    ଯଦି (lnum >= ଶୀର୍ଷ && lnum <= ଶୀର୍ଷ + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    ଅନ୍ୟଥା ଯଦି (ଶୀର୍ଷ < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		ପାଇଁ (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(ଶୀର୍ଷ + 1 + i);

		    ଯଦି (curbuf->b_ml.ml_ଧାଡ଼ି_len != uep->ue_array[i].ul_len
			    || memcmp(uep->ue_array[i].ul_ଧାଡ଼ି, p,
						curbuf->b_ml.ml_ଧାଡ଼ି_len) != 0)
			ଭାଙ୍ଗ;
		}
		ଯଦି (i == newsize && newlnum == MAXLNUM && uep->ue_next == ଖାଲି)
		{
		    newlnum = ଶୀର୍ଷ;
		    new_curpos.lnum = newlnum + 1;
		}
		ଅନ୍ୟଥା ଯଦି (i < newsize)
		{
		    newlnum = ଶୀର୍ଷ + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	empty_buffer = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	ଯଦି (oldsize > 0)
	{
	    ଯଦି ((newarray = U_ALLOC_ଧାଡ଼ି(ଆକାର(undoline_T) * oldsize)) == ଖାଲି)
	    {
		do_outofmem_ସନ୍ଦେଶ((long_u)(ଆକାର(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		ଯେତେବେଳେ (uep != ଖାଲି)
		{
		    nuep = uep->ue_next;
		    u_freeentry(uep, uep->ue_size);
		    uep = nuep;
		}
		ଭାଙ୍ଗ;
	    }
	    // delete backwards, it goes faster in most cases
	    ପାଇଁ (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		ଯଦି (u_save_ଧାଡ଼ି(&newarray[i], lnum) == FAIL)
		    do_outofmem_ସନ୍ଦେଶ((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		ଯଦି (curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା == 1)
		    empty_buffer = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	ଅନ୍ୟଥା
	    newarray = ଖାଲି;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	ଯଦି (newsize)
	{
	    ପାଇଁ (lnum = ଶୀର୍ଷ, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		ଯଦି (empty_buffer && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_ଧାଡ଼ି,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		ଅନ୍ୟଥା
		    ml_append_flags(lnum, uep->ue_array[i].ul_ଧାଡ଼ି,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_ଧାଡ଼ି);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#ifdef FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, ଶୀର୍ଷ, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, ଶୀର୍ଷ + newsize + 1, &uep->ue_props,
								UNDOPROP_BELOW);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, ଶୀର୍ଷ, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_BELOW);
	ଯଦି (newsize > 0)
	    adjust_props_for_undo(curbuf, ଶୀର୍ଷ + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_BELOW);
#endif

	// adjust marks
	ଯଦି (oldsize != newsize)
	{
	    mark_adjust(ଶୀର୍ଷ + 1, ଶୀର୍ଷ + oldsize, (long)MAXLNUM,
					       (long)newsize - (long)oldsize);
	    ଯଦି (curbuf->b_op_start.lnum > ଶୀର୍ଷ + oldsize)
		curbuf->b_op_start.lnum += newsize - oldsize;
	    ଯଦି (curbuf->b_op_end.lnum > ଶୀର୍ଷ + oldsize)
		curbuf->b_op_end.lnum += newsize - oldsize;
	}
	ଯଦି (oldsize > 0 || newsize > 0)
	{
	    changed_ଧାଡ଼ିଗୁଡ଼ିକ(ଶୀର୍ଷ + 1, 0, bot, newsize - oldsize);
#ifdef FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    ଯଦି (spell_check_window(curwin) && bot <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		redrawWinline(curwin, bot);
#endif
	}

	// Set the '[ mark.
	ଯଦି (ଶୀର୍ଷ + 1 < curbuf->b_op_start.lnum)
	    curbuf->b_op_start.lnum = ଶୀର୍ଷ + 1;
	// Set the '] mark.
	ଯଦି (newsize == 0 && ଶୀର୍ଷ + 1 > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = ଶୀର୍ଷ + 1;
	ଅନ୍ୟଥା ଯଦି (ଶୀର୍ଷ + newsize > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = ଶୀର୍ଷ + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_size = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = ଶୀର୍ଷ + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_next;
	uep->ue_next = newlist;
	newlist = uep;
    }

    // Ensure the '[ and '] marks are within bounds.
    ଯଦି (curbuf->b_op_start.lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	 curbuf->b_op_start.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଯଦି (curbuf->b_op_end.lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	 curbuf->b_op_end.lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_entry = newlist;
    curhead->uh_flags = new_flags;
    ଯଦି ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_EMPTY;
    ଯଦି (old_flags & UH_CHANGED)
	changed();
    ଅନ୍ୟଥା
#ifdef FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	ଯଦି (!isNetbeansModified(curbuf))
#endif
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    ପାଇଁ (i = 0; i < NMARKS; ++i)
    {
	ଯଦି (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	ଯଦି (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	ଅନ୍ୟଥା
	    curhead->uh_namedm[i].lnum = 0;
    }
    ଯଦି (curhead->uh_visual.vi_start.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    ଯଦି (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    ଯଦି (curwin->w_cursor.lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	ଯଦି (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = curhead->uh_cursor.col;
	    ଯଦି (virtual_active() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    ଅନ୍ୟଥା
		curwin->w_cursor.coladd = 0;
	}
	ଅନ୍ୟଥା
	    beginline(BL_SOL | BL_FIX);
    }
    ଅନ୍ୟଥା
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
    ଯଦି (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	ଯଦି (curhead->uh_next.ptr != ଖାଲି)
	    curbuf->b_u_seq_cur = curhead->uh_next.ptr->uh_seq;
	ଅନ୍ୟଥା
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    ଯଦି (curhead->uh_save_nr != 0)
    {
	ଯଦି (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
u_undo_end(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_undo,	// just did an undo
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		absolute)	// used ":undo N"
{
    ଅକ୍ଷର	*msgstr;
    u_header_T	*uhp;
    char_u	msgbuf[80];

#ifdef FEAT_FOLDING
    ଯଦି ((fdo_flags & FDO_UNDO) && ଚାବି_Typed)
	foldOpenCursor();
#endif

    ଯଦି (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	ଫେରନ୍ତୁ;

    ଯଦି (curbuf->b_ml.ml_flags & ML_EMPTY)
	--u_newcount;

    u_oldcount -= u_newcount;
    ଯଦି (u_oldcount == -1)
	msgstr = N_("more line");
    ଅନ୍ୟଥା ଯଦି (u_oldcount < 0)
	msgstr = N_("more lines");
    ଅନ୍ୟଥା ଯଦି (u_oldcount == 1)
	msgstr = N_("line less");
    ଅନ୍ୟଥା ଯଦି (u_oldcount > 1)
	msgstr = N_("fewer lines");
    ଅନ୍ୟଥା
    {
	u_oldcount = u_newcount;
	ଯଦି (u_newcount == 1)
	    msgstr = N_("change");
	ଅନ୍ୟଥା
	    msgstr = N_("changes");
    }

    ଯଦି (curbuf->b_u_curhead != ଖାଲି)
    {
	// For ":undo N" we prefer a "after #N" message.
	ଯଦି (absolute && curbuf->b_u_curhead->uh_next.ptr != ଖାଲି)
	{
	    uhp = curbuf->b_u_curhead->uh_next.ptr;
	    did_undo = FALSE;
	}
	ଅନ୍ୟଥା ଯଦି (did_undo)
	    uhp = curbuf->b_u_curhead;
	ଅନ୍ୟଥା
	    uhp = curbuf->b_u_curhead->uh_next.ptr;
    }
    ଅନ୍ୟଥା
	uhp = curbuf->b_u_newhead;

    ଯଦି (uhp == ଖାଲି)
	*msgbuf = NUL;
    ଅନ୍ୟଥା
	add_time(msgbuf, ଆକାର(msgbuf), uhp->uh_time);

#ifdef FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ଯଦି (wp->w_buffer == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#endif
    ଯଦି (VIsual_active)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == ଖାଲି ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    ଶୂନ୍ୟ
u_sync(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    ଯଦି (curbuf->b_u_synced || (!force && no_u_sync > 0))
	ଫେରନ୍ତୁ;
#ଯଦି defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ଯଦି (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	ଫେରନ୍ତୁ;		    // XIM is busy, don't break an undo sequence
#endif
    ଯଦି (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    ଅନ୍ୟଥା
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = ଖାଲି;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    ଶୂନ୍ୟ
ex_undolist(exarg_T *eap UNUSED)
{
    garray_T	ga;
    u_header_T	*uhp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mark;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nomark;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		changes = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର *), 20);

    uhp = curbuf->b_u_oldhead;
    ଯେତେବେଳେ (uhp != ଖାଲି)
    {
	ଯଦି (uhp->uh_prev.ptr == ଖାଲି && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    ଯଦି (ga_grow(&ga, 1) == FAIL)
		ଭାଙ୍ଗ;
	    len = vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    add_time(IObuff + len, IOSIZE - len, uhp->uh_time);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(IObuff + len);
	    ଯଦି (uhp->uh_save_nr > 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((ଅକ୍ଷର *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_data))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	ଯଦି (uhp->uh_prev.ptr != ଖାଲି && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_prev.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	ଅନ୍ୟଥା ଯଦି (uhp->uh_alt_next.ptr != ଖାଲି
		&& uhp->uh_alt_next.ptr->uh_walk != nomark
		&& uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	ଅନ୍ୟଥା ଯଦି (uhp->uh_next.ptr != ଖାଲି && uhp->uh_alt_prev.ptr == ଖାଲି
		&& uhp->uh_next.ptr->uh_walk != nomark
		&& uhp->uh_next.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_next.ptr;
	    --changes;
	}

	ଅନ୍ୟଥା
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    ଯଦି (uhp->uh_alt_prev.ptr != ଖାଲି)
		uhp = uhp->uh_alt_prev.ptr;
	    ଅନ୍ୟଥା
	    {
		uhp = uhp->uh_next.ptr;
		--changes;
	    }
	}
    }

    ଯଦି (ga.ga_len == 0)
	ସନ୍ଦେଶ(_("Nothing to undo"));
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

	sort_strings((char_u **)ga.ga_data, ga.ga_len);

	ସନ୍ଦେଶ_start();
	ସନ୍ଦେଶ_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	ପାଇଁ (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    ସନ୍ଦେଶ_putchar('\n');
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
	    ସନ୍ଦେଶ_puts(((ଅକ୍ଷର **)ga.ga_data)[i]);
	}
	ସନ୍ଦେଶ_end();

	ga_clear_strings(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    ଶୂନ୍ୟ
ex_undojoin(exarg_T *eap UNUSED)
{
    ଯଦି (curbuf->b_u_newhead == ଖାଲି)
	ଫେରନ୍ତୁ;		    // nothing changed before
    ଯଦି (curbuf->b_u_curhead != ଖାଲି)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	ଫେରନ୍ତୁ;
    }
    ଯଦି (!curbuf->b_u_synced)
	ଫେରନ୍ତୁ;		    // already unsynced
    ଯଦି (get_undolevel() < 0)
	ଫେରନ୍ତୁ;		    // no entries, nothing to do
    ଅନ୍ୟଥା
	// Append next change to the last entry
	curbuf->b_u_synced = false;
}

/*
 * Called after writing or reloading the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
    ଶୂନ୍ୟ
u_unchanged(buf_T *buf)
{
    u_unch_branch(buf->b_u_oldhead);
    buf->b_did_warn = false;
}

/*
 * After reloading a buffer which was saved for 'undoreload': Find the first
 * line that was changed and set the cursor there.
 */
    ଶୂନ୍ୟ
u_find_first_changed(ଶୂନ୍ୟ)
{
    u_header_T	*uhp = curbuf->b_u_newhead;
    u_entry_T   *uep;
    linenr_T	lnum;

    ଯଦି (curbuf->b_u_curhead != ଖାଲି || uhp == ଖାଲି)
	ଫେରନ୍ତୁ;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_entry;
    ଯଦି (uep->ue_ଶୀର୍ଷ != 0 || uep->ue_bot != 0)
	ଫେରନ୍ତୁ;

    ପାଇଁ (lnum = 1; lnum < curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
					      && lnum <= uep->ue_size; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	ଯଦି (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_ଧାଡ଼ି_len
		|| memcmp(p, uep->ue_array[lnum - 1].ul_ଧାଡ଼ି, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    ଫେରନ୍ତୁ;
	}
    }
    ଯଦି (curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା != uep->ue_size)
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
    ଶୂନ୍ୟ
u_update_save_nr(buf_T *buf)
{
    u_header_T	*uhp;

    ++buf->b_u_save_nr_last;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_last;
    uhp = buf->b_u_curhead;
    ଯଦି (uhp != ଖାଲି)
	uhp = uhp->uh_next.ptr;
    ଅନ୍ୟଥା
	uhp = buf->b_u_newhead;
    ଯଦି (uhp != ଖାଲି)
	uhp->uh_save_nr = buf->b_u_save_nr_last;
}

    ସ୍ଥିର ଶୂନ୍ୟ
u_unch_branch(u_header_T *uhp)
{
    u_header_T	*uh;

    ପାଇଁ (uh = uhp; uh != ଖାଲି; uh = uh->uh_prev.ptr)
    {
	uh->uh_flags |= UH_CHANGED;
	ଯଦି (uh->uh_alt_next.ptr != ଖାଲି)
	    u_unch_branch(uh->uh_alt_next.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    ସ୍ଥିର u_entry_T *
u_get_headentry(ଶୂନ୍ୟ)
{
    ଯଦି (curbuf->b_u_newhead == ଖାଲି || curbuf->b_u_newhead->uh_entry == ଖାଲି)
    {
	iemsg(e_undo_list_corrupt);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ curbuf->b_u_newhead->uh_entry;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
u_getbot(ଶୂନ୍ୟ)
{
    u_entry_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    ଯଦି (uep == ଖାଲି)
	ଫେରନ୍ତୁ;

    uep = curbuf->b_u_newhead->uh_getbot_entry;
    ଯଦି (uep != ଖାଲି)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - uep->ue_lcount;
	uep->ue_bot = uep->ue_ଶୀର୍ଷ + uep->ue_size + 1 + extra;
	ଯଦି (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    iemsg(e_undo_ଧାଡ଼ି_missing);
	    uep->ue_bot = uep->ue_ଶୀର୍ଷ + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_entry = ଖାଲି;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
u_freeheader(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    ଯଦି (uhp->uh_alt_next.ptr != ଖାଲି)
	u_freebranch(buf, uhp->uh_alt_next.ptr, uhpp);

    ଯଦି (uhp->uh_alt_prev.ptr != ଖାଲି)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = ଖାଲି;

    // Update the links in the list to remove the header.
    ଯଦି (uhp->uh_next.ptr == ଖାଲି)
	buf->b_u_oldhead = uhp->uh_prev.ptr;
    ଅନ୍ୟଥା
	uhp->uh_next.ptr->uh_prev.ptr = uhp->uh_prev.ptr;

    ଯଦି (uhp->uh_prev.ptr == ଖାଲି)
	buf->b_u_newhead = uhp->uh_next.ptr;
    ଅନ୍ୟଥା
	ପାଇଁ (uhap = uhp->uh_prev.ptr; uhap != ଖାଲି;
						 uhap = uhap->uh_alt_next.ptr)
	    uhap->uh_next.ptr = uhp->uh_next.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
u_freebranch(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *tofree, *next;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    ଯଦି (uhp == buf->b_u_oldhead)
    {
	ଯେତେବେଳେ (buf->b_u_oldhead != ଖାଲି)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (uhp->uh_alt_prev.ptr != ଖାଲି)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = ଖାଲି;

    next = uhp;
    ଯେତେବେଳେ (next != ଖାଲି)
    {
	tofree = next;
	ଯଦି (tofree->uh_alt_next.ptr != ଖାଲି)
	    u_freebranch(buf, tofree->uh_alt_next.ptr, uhpp);   // recursive
	next = tofree->uh_prev.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}

/*
 * Free all the undo entries for one header and the header itself.
 * This means that "uhp" is invalid when returning.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
u_freeentries(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_entry_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    ଯଦି (buf->b_u_curhead == uhp)
	buf->b_u_curhead = ଖାଲି;
    ଯଦି (buf->b_u_newhead == uhp)
	buf->b_u_newhead = ଖାଲି;  // freeing the newest entry
    ଯଦି (uhpp != ଖାଲି && uhp == *uhpp)
	*uhpp = ଖାଲି;

    ପାଇଁ (uep = uhp->uh_entry; uep != ଖାଲି; uep = nuep)
    {
	nuep = uep->ue_next;
	u_freeentry(uep, uep->ue_size);
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
    ସ୍ଥିର ଶୂନ୍ୟ
u_freeentry(u_entry_T *uep, long n)
{
    ଯେତେବେଳେ (n > 0)
	vim_free(uep->ue_array[--n].ul_ଧାଡ଼ି);
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
    ସ୍ଥିର ଶୂନ୍ୟ
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = ଖାଲି;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି = ଖାଲି;
    buf->b_u_ଧାଡ଼ି_ptr.ul_len = 0;
    buf->b_u_ଧାଡ଼ି_ptr.ul_textlen = 0;
    buf->b_u_ଧାଡ଼ି_lnum = 0;
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
u_blockfree(buf_T *buf)
{
    ଯେତେବେଳେ (buf->b_u_oldhead != ଖାଲି)
	u_freeheader(buf, buf->b_u_oldhead, ଖାଲି);
    vim_free(buf->b_u_ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି);
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 * and invalidate the undo buffer
 */
    ଶୂନ୍ୟ
u_clearallandblockfree(buf_T *buf)
{
    u_blockfree(buf);
    u_clearall(buf);
}



/*
 * Save the line "lnum" for the "U" command.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
u_saveline(linenr_T lnum)
{
    ଯଦି (lnum == curbuf->b_u_ଧାଡ଼ି_lnum)	    // line is already saved
	ଫେରନ୍ତୁ;
    ଯଦି (lnum < 1 || lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା) // should never happen
	ଫେରନ୍ତୁ;
    u_clearline();
    curbuf->b_u_ଧାଡ଼ି_lnum = lnum;
    ଯଦି (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_ଧାଡ଼ି_colnr = curwin->w_cursor.col;
    ଅନ୍ୟଥା
	curbuf->b_u_ଧାଡ଼ି_colnr = 0;
    ଯଦି (u_save_ଧାଡ଼ି(&curbuf->b_u_ଧାଡ଼ି_ptr, lnum) == FAIL)
	do_outofmem_ସନ୍ଦେଶ((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    ଶୂନ୍ୟ
u_clearline(ଶୂନ୍ୟ)
{
    ଯଦି (curbuf->b_u_ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି == ଖାଲି)
	ଫେରନ୍ତୁ;

    VIM_CLEAR(curbuf->b_u_ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି);
    curbuf->b_u_ଧାଡ଼ି_ptr.ul_len = 0;
    curbuf->b_u_ଧାଡ଼ି_ptr.ul_textlen = 0;
    curbuf->b_u_ଧାଡ଼ି_lnum = 0;
}

/*
 * Implementation of the "U" command.
 * Differentiation from vi: "U" can be undone with the next "U".
 * We also allow the cursor to be in another line.
 * Careful: may trigger autocommands that reload the buffer.
 */
    ଶୂନ୍ୟ
u_undoline(ଶୂନ୍ୟ)
{
    colnr_T	t;
    undoline_T  oldp;

    ଯଦି (undo_off)
	ଫେରନ୍ତୁ;

    ଯଦି (curbuf->b_u_ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି == ଖାଲି
			|| curbuf->b_u_ଧାଡ଼ି_lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	beep_flush();
	ଫେରନ୍ତୁ;
    }

    // first save the line for the 'u' command
    ଯଦି (u_savecommon(curbuf->b_u_ଧାଡ଼ି_lnum - 1,
		       curbuf->b_u_ଧାଡ଼ି_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (u_save_ଧାଡ଼ି(&oldp, curbuf->b_u_ଧାଡ଼ି_lnum) == FAIL)
    {
	do_outofmem_ସନ୍ଦେଶ((long_u)0);
	ଫେରନ୍ତୁ;
    }
    ml_replace_len(curbuf->b_u_ଧାଡ଼ି_lnum, curbuf->b_u_ଧାଡ଼ି_ptr.ul_ଧାଡ଼ି,
				     curbuf->b_u_ଧାଡ଼ି_ptr.ul_len, TRUE, FALSE);
    changed_bytes(curbuf->b_u_ଧାଡ଼ି_lnum, 0);
    curbuf->b_u_ଧାଡ଼ି_ptr = oldp;

    t = curbuf->b_u_ଧାଡ଼ି_colnr;
    ଯଦି (curwin->w_cursor.lnum == curbuf->b_u_ଧାଡ଼ି_lnum)
	curbuf->b_u_ଧାଡ଼ି_colnr = curwin->w_cursor.col;
    curwin->w_cursor.col = t;
    curwin->w_cursor.lnum = curbuf->b_u_ଧାଡ଼ି_lnum;
    check_cursor_col();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bufIsChanged(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    ଯଦି (term_job_running_not_none(buf->b_term))
	ଫେରନ୍ତୁ TRUE;
#endif
    ଫେରନ୍ତୁ bufIsChangedNotTerm(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
anyBufIsChanged(ଶୂନ୍ୟ)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	ଯଦି (bufIsChanged(buf))
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bufIsChangedNotTerm(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    ଫେରନ୍ତୁ (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_changed || file_ff_differs(buf, TRUE));
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
curbufIsChanged(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ bufIsChanged(curbuf);
}

#ଯଦି defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
u_eval_ବୃକ୍ଷ(buf_T *buf, u_header_T *first_uhp, list_T *list)
{
    u_header_T  *uhp = first_uhp;
    dict_T	*dict;

    ଯେତେବେଳେ (uhp != ଖାଲି)
    {
	dict = dict_alloc();
	ଯଦି (dict == ଖାଲି)
	    ଫେରନ୍ତୁ;
	dict_add_ସଂଖ୍ୟା(dict, "seq", uhp->uh_seq);
	dict_add_ସଂଖ୍ୟା(dict, "time", (long)uhp->uh_time);
	ଯଦି (uhp == buf->b_u_newhead)
	    dict_add_ସଂଖ୍ୟା(dict, "newhead", 1);
	ଯଦି (uhp == buf->b_u_curhead)
	    dict_add_ସଂଖ୍ୟା(dict, "curhead", 1);
	ଯଦି (uhp->uh_save_nr > 0)
	    dict_add_ସଂଖ୍ୟା(dict, "save", uhp->uh_save_nr);

	ଯଦି (uhp->uh_alt_next.ptr != ଖାଲି)
	{
	    list_T	*alt_list = list_alloc();

	    ଯଦି (alt_list != ଖାଲି)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_ବୃକ୍ଷ(buf, uhp->uh_alt_next.ptr, alt_list);
		dict_add_list(dict, "alt", alt_list);
	    }
	}

	list_append_dict(list, dict);
	uhp = uhp->uh_prev.ptr;
    }
}

/*
 * "undofile(name)" function
 */
    ଶୂନ୍ୟ
f_undofile(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	ଯଦି (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    char_u *ffname = FullName_save(fname, TRUE);

	    ଯଦି (ffname != ଖାଲି)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# ଅନ୍ୟଥା
    rettv->vval.v_string = ଖାଲି;
# endif
}
# ifdef FEAT_PERSISTENT_UNDO
/*
 * Reset undofile option and delete the undofile
 */
    ଶୂନ୍ୟ
u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    ଯଦି (!buf->b_p_udf)
	ଫେରନ୍ତୁ;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    ଯଦି (file_name != ଖାଲି)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    set_option_ମୂଲ୍ୟ_give_err((char_u *)"undofile", 0L, ଖାଲି, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    ଶୂନ୍ୟ
f_undotree(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_ସଂଖ୍ୟା(dict, "synced", (long)buf->b_u_synced);
    dict_add_ସଂଖ୍ୟା(dict, "seq_last", buf->b_u_seq_last);
    dict_add_ସଂଖ୍ୟା(dict, "save_last", buf->b_u_save_nr_last);
    dict_add_ସଂଖ୍ୟା(dict, "seq_cur", buf->b_u_seq_cur);
    dict_add_ସଂଖ୍ୟା(dict, "time_cur", (long)buf->b_u_time_cur);
    dict_add_ସଂଖ୍ୟା(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    ଯଦି (list != ଖାଲି)
    {
	u_eval_ବୃକ୍ଷ(buf, buf->b_u_oldhead, list);
	dict_add_list(dict, "entries", list);
    }
}

#endif
