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

#ಅನ್ತರ್ಭಾವ "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    buf_T	*bi_buf;
    FILE	*bi_fp;
#ifdef FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_buffer; // CRYPT_BUF_SIZE, NULL when not buffering
    size_t	bi_used;    // bytes written to/read from bi_buffer
    size_t	bi_avail;   // bytes available in bi_buffer
#endif
} bufinfo_T;


ಸ್ಥಿರ ಶೂನ್ಯ u_unch_branch(u_header_T *uhp);
ಸ್ಥಿರ u_entry_T *u_get_headentry(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ u_getbot(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ u_doit(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ);
ಸ್ಥಿರ ಶೂನ್ಯ u_undoredo(ಪೂರ್ಣಾಙ್ಕ undo);
ಸ್ಥಿರ ಶೂನ್ಯ u_undo_end(ಪೂರ್ಣಾಙ್ಕ did_undo, ಪೂರ್ಣಾಙ್ಕ absolute);
ಸ್ಥಿರ ಶೂನ್ಯ u_freeheader(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
ಸ್ಥಿರ ಶೂನ್ಯ u_freebranch(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
ಸ್ಥಿರ ಶೂನ್ಯ u_freeentries(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
ಸ್ಥಿರ ಶೂನ್ಯ u_freeentry(u_entry_T *, long);
#ifdef FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ undo_flush(bufinfo_T *bi);
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ undo_read(bufinfo_T *bi, char_u *buffer, size_t size);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ serialize_uep(bufinfo_T *bi, u_entry_T *uep);
ಸ್ಥಿರ u_entry_T *unserialize_uep(bufinfo_T *bi, ಪೂರ್ಣಾಙ್ಕ *error, char_u *file_name);
ಸ್ಥಿರ ಶೂನ್ಯ serialize_pos(bufinfo_T *bi, pos_T pos);
ಸ್ಥಿರ ಶೂನ್ಯ unserialize_pos(bufinfo_T *bi, pos_T *pos);
ಸ್ಥಿರ ಶೂನ್ಯ serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
ಸ್ಥಿರ ಶೂನ್ಯ unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ u_saveline(linenr_T lnum);
ಸ್ಥಿರ ಶೂನ್ಯ u_blockfree(buf_T *buf);

#define U_ALLOC_ಸಾಲು(size) lalloc(size, FALSE)

// used in undo_end() to report number of added and deleted lines
ಸ್ಥಿರ long	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	undo_undoes = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	lastmark = 0;

#ಯದಿ defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ seen_b_u_curhead;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ seen_b_u_newhead;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ header_ಎಣಿಕೆ;

    ಸ್ಥಿರ ಶೂನ್ಯ
u_check_ವೃಕ್ಷ(u_header_T *uhp,
	u_header_T *exp_uh_next,
	u_header_T *exp_uh_alt_prev)
{
    u_entry_T *uep;

    ಯದಿ (uhp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ++header_ಎಣಿಕೆ;
    ಯದಿ (uhp == curbuf->b_u_curhead && ++seen_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (uhp == curbuf->b_u_newhead && ++seen_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    ಅನ್ಯಥಾ
    {
	// Check pointers back are correct.
	ಯದಿ (uhp->uh_next.ptr != exp_uh_next)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_next, uhp->uh_next.ptr);
	}
	ಯದಿ (uhp->uh_alt_prev.ptr != exp_uh_alt_prev)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_prev, uhp->uh_alt_prev.ptr);
	}

	// Check the undo tree at this header.
	ಪ್ರತಿ (uep = uhp->uh_entry; uep != ರಿಕ್ತ; uep = uep->ue_next)
	{
	    ಯದಿ (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		ಭಙ್ಗ;
	    }
	}

	// Check the next alt tree.
	u_check_ವೃಕ್ಷ(uhp->uh_alt_next.ptr, uhp->uh_next.ptr, uhp);

	// Check the next header in this branch.
	u_check_ವೃಕ್ಷ(uhp->uh_prev.ptr, uhp, ರಿಕ್ತ);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
u_check(ಪೂರ್ಣಾಙ್ಕ newhead_may_be_NULL)
{
    seen_b_u_newhead = 0;
    seen_b_u_curhead = 0;
    header_ಎಣಿಕೆ = 0;

    u_check_ವೃಕ್ಷ(curbuf->b_u_oldhead, ರಿಕ್ತ, ರಿಕ್ತ);

    ಯದಿ (seen_b_u_newhead == 0 && curbuf->b_u_oldhead != ರಿಕ್ತ
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == ರಿಕ್ತ))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    ಯದಿ (curbuf->b_u_curhead != ರಿಕ್ತ && seen_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    ಯದಿ (header_ಎಣಿಕೆ != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (long)header_ಎಣಿಕೆ, (long)curbuf->b_u_numhead);
    }
}
#endif

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
u_save_cursor(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
u_save(linenr_T ತುದಿ, linenr_T bot)
{
    ಯದಿ (undo_off)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (ತುದಿ >= bot || bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	ಪ್ರತ್ಯಾಗಮ FAIL;	// rely on caller to give an error message

    ಯದಿ (ತುದಿ + 2 == bot)
	u_saveline((linenr_T)(ತುದಿ + 1));

    ಪ್ರತ್ಯಾಗಮ (u_savecommon(ತುದಿ, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
u_savesub(linenr_T lnum)
{
    ಯದಿ (undo_off)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಪ್ರತ್ಯಾಗಮ (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
u_inssub(linenr_T lnum)
{
    ಯದಿ (undo_off)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಪ್ರತ್ಯಾಗಮ (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
u_savedel(linenr_T lnum, long nlines)
{
    ಯದಿ (undo_off)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಪ್ರತ್ಯಾಗಮ (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
undo_allowed(ಶೂನ್ಯ)
{
    // Don't allow changes when 'modifiable' is off.
    ಯದಿ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

#ifdef HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    ಯದಿ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
#endif

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    ಯದಿ (textlock != 0)
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    ಸ್ಥಿರ long
get_undolevel(ಶೂನ್ಯ)
{
    ಯದಿ (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	ಪ್ರತ್ಯಾಗಮ p_ul;
    ಪ್ರತ್ಯಾಗಮ curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
u_save_ಸಾಲು(undoline_T *ul, linenr_T lnum)
{
    char_u *ಸಾಲು = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    ಯದಿ (curbuf->b_ml.ml_ಸಾಲು_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_ಸಾಲು = vim_strsave((char_u *)"");
    }
    ಅನ್ಯಥಾ
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_ಸಾಲು_len;
	ul->ul_ಸಾಲು = vim_memsave(ಸಾಲು, ul->ul_len);
    }
    ಪ್ರತ್ಯಾಗಮ ul->ul_ಸಾಲು == ರಿಕ್ತ ? FAIL : OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
has_prop_w_flags(linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ flags)
{
    char_u  *props;
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    proplen = get_text_props(curbuf, lnum, &props, FALSE);

    ಪ್ರತಿ (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * ಪರಿಮಾಣ prop, ಪರಿಮಾಣ prop);
	ಯದಿ (prop.tp_flags & flags)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಪೂರ್ಣಾಙ್ಕ
u_savecommon(
    linenr_T	ತುದಿ,
    linenr_T	bot,
    linenr_T	newbot,
    ಪೂರ್ಣಾಙ್ಕ		reload)
{
    linenr_T	lnum;
    long	i;
    u_header_T	*uhp;
    u_header_T	*old_curhead;
    u_entry_T	*uep;
    u_entry_T	*prev_uep;
    long	size;

    ಯದಿ (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	ಯದಿ (!undo_allowed())
	    ಪ್ರತ್ಯಾಗಮ FAIL;

#ifdef FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	ಯದಿ (netbeans_active())
	{
	    ಯದಿ (netbeans_is_guarded(ತುದಿ, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_in_read_only_files));
		ಪ್ರತ್ಯಾಗಮ FAIL;
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
	ಯದಿ (bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_ಸಾಲು_ಎಣಿಕೆ_changed_unexpectedly));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

#ifdef U_DEBUG
    u_check(FALSE);
#endif

#ifdef FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    ಯದಿ (bot - ತುದಿ > 1)
    {
	ಯದಿ (ತುದಿ > 0 && has_prop_w_flags(ತುದಿ + 1, TP_FLAG_CONT_PREV))
	    --ತುದಿ;
	ಯದಿ (bot <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_NEXT))
	{
	    ++bot;
	    ಯದಿ (newbot != 0)
		++newbot;
	}
    }
#endif

    size = bot - ತುದಿ - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    ಯದಿ (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	ಯದಿ (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_ಸಾಲು(ಪರಿಮಾಣ(u_header_T));
	    ಯದಿ (uhp == ರಿಕ್ತ)
		ಗಚ್ಛ nomem;
#ifdef U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#endif
	}
	ಅನ್ಯಥಾ
	    uhp = ರಿಕ್ತ;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	ಯದಿ (old_curhead != ರಿಕ್ತ)
	{
	    curbuf->b_u_newhead = old_curhead->uh_next.ptr;
	    curbuf->b_u_curhead = ರಿಕ್ತ;
	}

	/*
	 * free headers to keep the size right
	 */
	ಯಾವತ್ (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != ರಿಕ್ತ)
	{
	    u_header_T	    *uhfree = curbuf->b_u_oldhead;

	    ಯದಿ (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    ಅನ್ಯಥಾ ಯದಿ (uhfree->uh_alt_next.ptr == ರಿಕ್ತ)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    ಅನ್ಯಥಾ
	    {
		// Free the oldest alternate branch as a whole.
		ಯಾವತ್ (uhfree->uh_alt_next.ptr != ರಿಕ್ತ)
		    uhfree = uhfree->uh_alt_next.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#ifdef U_DEBUG
	    u_check(TRUE);
#endif
	}

	ಯದಿ (uhp == ರಿಕ್ತ)		// no undo at all
	{
	    ಯದಿ (old_curhead != ರಿಕ್ತ)
		u_freebranch(curbuf, old_curhead, ರಿಕ್ತ);
	    curbuf->b_u_synced = false;
	    ಪ್ರತ್ಯಾಗಮ OK;
	}

	uhp->uh_prev.ptr = ರಿಕ್ತ;
	uhp->uh_next.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_next.ptr = old_curhead;
	ಯದಿ (old_curhead != ರಿಕ್ತ)
	{
	    uhp->uh_alt_prev.ptr = old_curhead->uh_alt_prev.ptr;
	    ಯದಿ (uhp->uh_alt_prev.ptr != ರಿಕ್ತ)
		uhp->uh_alt_prev.ptr->uh_alt_next.ptr = uhp;
	    old_curhead->uh_alt_prev.ptr = uhp;
	    ಯದಿ (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	ಅನ್ಯಥಾ
	    uhp->uh_alt_prev.ptr = ರಿಕ್ತ;
	ಯದಿ (curbuf->b_u_newhead != ರಿಕ್ತ)
	    curbuf->b_u_newhead->uh_prev.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_last;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_time = vim_time();
	uhp->uh_save_nr = 0;
	curbuf->b_u_time_cur = uhp->uh_time + 1;

	uhp->uh_walk = 0;
	uhp->uh_entry = ರಿಕ್ತ;
	uhp->uh_getbot_entry = ರಿಕ್ತ;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	ಯದಿ (virtual_active() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	ಅನ್ಯಥಾ
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_changed ? UH_CHANGED : 0) +
		       ((curbuf->b_ml.ml_flags & ML_EMPTY) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, ಪರಿಮಾಣ(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	ಯದಿ (curbuf->b_u_oldhead == ರಿಕ್ತ)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (get_undolevel() < 0)	// no undo at all
	    ಪ್ರತ್ಯಾಗಮ OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	ಯದಿ (size == 1)
	{
	    uep = u_get_headentry();
	    prev_uep = ರಿಕ್ತ;
	    ಪ್ರತಿ (i = 0; i < 10; ++i)
	    {
		ಯದಿ (uep == ರಿಕ್ತ)
		    ಭಙ್ಗ;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		ಯದಿ ((curbuf->b_u_newhead->uh_getbot_entry != uep
			    ? (uep->ue_ತುದಿ + uep->ue_size + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			|| (uep->ue_size > 1
			    && ತುದಿ >= uep->ue_ತುದಿ
			    && ತುದಿ + 2 <= uep->ue_ತುದಿ + uep->ue_size + 1))
		    ಭಙ್ಗ;

		// If it's the same line we can skip saving it again.
		ಯದಿ (uep->ue_size == 1 && uep->ue_ತುದಿ == ತುದಿ)
		{
		    ಯದಿ (i > 0)
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
		    ಯದಿ (newbot != 0)
			uep->ue_bot = newbot;
		    ಅನ್ಯಥಾ ಯದಿ (bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			uep->ue_bot = 0;
		    ಅನ್ಯಥಾ
		    {
			uep->ue_lcount = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
			curbuf->b_u_newhead->uh_getbot_entry = uep;
		    }
		    ಪ್ರತ್ಯಾಗಮ OK;
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
    uep = U_ALLOC_ಸಾಲು(ಪರಿಮಾಣ(u_entry_T));
    ಯದಿ (uep == ರಿಕ್ತ)
	ಗಚ್ಛ nomem;
    CLEAR_POINTER(uep);
#ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
#endif

    uep->ue_size = size;
    uep->ue_ತುದಿ = ತುದಿ;
    ಯದಿ (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    ಅನ್ಯಥಾ ಯದಿ (bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	uep->ue_bot = 0;
    ಅನ್ಯಥಾ
    {
	uep->ue_lcount = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	curbuf->b_u_newhead->uh_getbot_entry = uep;
    }

    ಯದಿ (size > 0)
    {
	ಯದಿ ((uep->ue_array = U_ALLOC_ಸಾಲು(ಪರಿಮಾಣ(undoline_T) * size)) == ರಿಕ್ತ)
	{
	    u_freeentry(uep, 0L);
	    ಗಚ್ಛ nomem;
	}
	ಪ್ರತಿ (i = 0, lnum = ತುದಿ + 1; i < size; ++i)
	{
	    fast_breakcheck();
	    ಯದಿ (got_int)
	    {
		u_freeentry(uep, i);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (u_save_ಸಾಲು(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		ಗಚ್ಛ nomem;
	    }
	}
    }
    ಅನ್ಯಥಾ
	uep->ue_array = ರಿಕ್ತ;
    uep->ue_next = curbuf->b_u_newhead->uh_entry;
    curbuf->b_u_newhead->uh_entry = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#ifdef U_DEBUG
    u_check(FALSE);
#endif
    ಪ್ರತ್ಯಾಗಮ OK;

nomem:
    ಸಂದೇಶ_silent = 0;	// must display the prompt
    ಯದಿ (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    do_outofmem_ಸಂದೇಶ((long_u)0);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

#ಯದಿ defined(FEAT_PERSISTENT_UNDO)

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
    ಶೂನ್ಯ
u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;

    sha256_start(&ctx);
    ಪ್ರತಿ (lnum = 1; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
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
    ಸ್ಥಿರ char_u *
u_get_undo_file_name(char_u *buf_ffname, ಪೂರ್ಣಾಙ್ಕ reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {ರಿಕ್ತ, 0};
    char_u	*undo_file_name = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		dir_len;
    char_u	*p;
    size_t	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    size_t	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    ಯದಿ (ffname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    ಯದಿ (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    ಯಾವತ್ (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	ಯದಿ (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    ಯದಿ (undo_file_name == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    p = gettail(undo_file_name);
	    plen = (size_t)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# ಅನ್ಯಥಾ
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	ಅನ್ಯಥಾ
	{
	    dir_name[dir_len] = NUL;
	    ಯದಿ (mch_isdir(dir_name))
	    {
		string_T    ret;

		ಯದಿ (munged_name.string == ರಿಕ್ತ)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    ಯದಿ (munged_name.string == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		    munged_name.length = ffnamelen;
		    ಪ್ರತಿ (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			ಯದಿ (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (size_t)dir_len,
		    munged_name.string, munged_name.length, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	ಯದಿ (undo_file_name != ರಿಕ್ತ && (!reading
			       || mch_stat((ವರ್ಣ *)undo_file_name, &st) >= 0))
	    ಭಙ್ಗ;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    ಪ್ರತ್ಯಾಗಮ undo_file_name;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
corruption_error(ವರ್ಣ *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
u_free_uhp(u_header_T *uhp)
{
    u_entry_T	*nuep;
    u_entry_T	*uep;

    uep = uhp->uh_entry;
    ಯಾವತ್ (uep != ರಿಕ್ತ)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    ಯದಿ (bi->bi_buffer != ರಿಕ್ತ)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	ಯಾವತ್ (bi->bi_used + len_todo >= CRYPT_BUF_SIZE)
	{
	    size_t	n = CRYPT_BUF_SIZE - bi->bi_used;

	    mch_memmove(bi->bi_buffer + bi->bi_used, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_used = CRYPT_BUF_SIZE;
	    ಯದಿ (undo_flush(bi) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (len_todo > 0)
	{
	    mch_memmove(bi->bi_buffer + bi->bi_used, p, len_todo);
	    bi->bi_used += len_todo;
	}
	ಪ್ರತ್ಯಾಗಮ OK;
    }
# endif
    ಯದಿ (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

# ifdef FEAT_CRYPT
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
undo_flush(bufinfo_T *bi)
{
    ಯದಿ (bi->bi_buffer != ರಿಕ್ತ && bi->bi_state != ರಿಕ್ತ && bi->bi_used > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_used, FALSE);
	ಯದಿ (fwrite(bi->bi_buffer, bi->bi_used, (size_t)1, bi->bi_fp) != 1)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	bi->bi_used = 0;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    ಯದಿ (bi->bi_state != ರಿಕ್ತ && bi->bi_buffer == ರಿಕ್ತ)
    {
	// crypting every piece of text separately
	ಯದಿ (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	ಅನ್ಯಥಾ
	{
	    copy = lalloc(len, FALSE);
	    ಯದಿ (copy == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	ಯದಿ (copy != small_buf)
	    vim_free(copy);
	ಪ್ರತ್ಯಾಗಮ i == 1 ? OK : FAIL;
    }
# endif
    ಪ್ರತ್ಯಾಗಮ undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
undo_write_bytes(bufinfo_T *bi, long_u nr, ಪೂರ್ಣಾಙ್ಕ len)
{
    char_u  buf[8];
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    bufi = 0;

    ಪ್ರತಿ (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    ಪ್ರತ್ಯಾಗಮ undo_write(bi, buf, (size_t)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    ಸ್ಥಿರ ಶೂನ್ಯ
put_header_ptr(bufinfo_T *bi, u_header_T *uhp)
{
    undo_write_bytes(bi, (long_u)(uhp != ರಿಕ್ತ ? uhp->uh_seq : 0), 4);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ಯದಿ (bi->bi_buffer != ರಿಕ್ತ)
    {
	char_u  buf[4];
	ಪೂರ್ಣಾಙ್ಕ	n;

	undo_read(bi, buf, (size_t)4);
	n = ((unsigned)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	ಪ್ರತ್ಯಾಗಮ n;
    }
# endif
    ಪ್ರತ್ಯಾಗಮ get4c(bi->bi_fp);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ಯದಿ (bi->bi_buffer != ರಿಕ್ತ)
    {
	char_u  buf[2];
	ಪೂರ್ಣಾಙ್ಕ	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	ಪ್ರತ್ಯಾಗಮ n;
    }
# endif
    ಪ್ರತ್ಯಾಗಮ get2c(bi->bi_fp);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ಯದಿ (bi->bi_buffer != ರಿಕ್ತ)
    {
	char_u  buf[1];

	undo_read(bi, buf, (size_t)1);
	ಪ್ರತ್ಯಾಗಮ buf[0];
    }
# endif
    ಪ್ರತ್ಯಾಗಮ getc(bi->bi_fp);
}

    ಸ್ಥಿರ time_t
undo_read_time(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ಯದಿ (bi->bi_buffer != ರಿಕ್ತ)
    {
	char_u  buf[8];
	time_t	n = 0;
	ಪೂರ್ಣಾಙ್ಕ	i;

	undo_read(bi, buf, (size_t)8);
	ಪ್ರತಿ (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	ಪ್ರತ್ಯಾಗಮ n;
    }
# endif
    ಪ್ರತ್ಯಾಗಮ get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
undo_read(bufinfo_T *bi, char_u *buffer, size_t size)
{
    ಪೂರ್ಣಾಙ್ಕ retval = OK;

# ifdef FEAT_CRYPT
    ಯದಿ (bi->bi_buffer != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	size_todo = (ಪೂರ್ಣಾಙ್ಕ)size;
	char_u	*p = buffer;

	ಯಾವತ್ (size_todo > 0)
	{
	    size_t n;

	    ಯದಿ (bi->bi_used >= bi->bi_avail)
	    {
		n = fread(bi->bi_buffer, 1, (size_t)CRYPT_BUF_SIZE, bi->bi_fp);
		ಯದಿ (n == 0)
		{
		    retval = FAIL;
		    ಭಙ್ಗ;
		}
		bi->bi_avail = n;
		bi->bi_used = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_avail, FALSE);
	    }
	    n = size_todo;
	    ಯದಿ (n > bi->bi_avail - bi->bi_used)
		n = bi->bi_avail - bi->bi_used;
	    mch_memmove(p, bi->bi_buffer + bi->bi_used, n);
	    bi->bi_used += n;
	    size_todo -= (ಪೂರ್ಣಾಙ್ಕ)n;
	    p += n;
	}
    }
    ಅನ್ಯಥಾ
# endif
    ಯದಿ (fread(buffer, size, 1, bi->bi_fp) != 1)
	retval = FAIL;

    ಯದಿ (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(buffer, 0, size);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    ಸ್ಥಿರ char_u *
read_string_decrypt(bufinfo_T *bi, ಪೂರ್ಣಾಙ್ಕ len)
{
    char_u  *ptr = alloc(len + 1);

    ಯದಿ (ptr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    ಯದಿ (bi->bi_state != ರಿಕ್ತ && bi->bi_buffer == ರಿಕ್ತ)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    ಪ್ರತ್ಯಾಗಮ ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
serialize_header(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	time_buf[8];

    // Start writing, first the magic marker and undo info version.
    ಯದಿ (fwrite(UF_START_MAGIC, (size_t)UF_START_MAGIC_LEN, (size_t)1, fp) != 1)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    ಯದಿ (*buf->b_p_ಕೀಲಿ != NUL)
    {
	char_u *header;
	ಪೂರ್ಣಾಙ್ಕ    header_len;
	long	len;

	undo_write_bytes(bi, (long_u)UF_VERSION_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_method_nr(buf),
					  buf->b_p_ಕೀಲಿ, &header, &header_len);
	ಯದಿ (bi->bi_state == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	len = (long)fwrite(header, (size_t)header_len, (size_t)1, fp);
	vim_free(header);
	ಯದಿ (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = ರಿಕ್ತ;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (crypt_whole_undofile(crypt_get_method_nr(buf)))
	{
	    bi->bi_buffer = alloc(CRYPT_BUF_SIZE);
	    ಯದಿ (bi->bi_buffer == ರಿಕ್ತ)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = ರಿಕ್ತ;
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    bi->bi_used = 0;
	}
    }
    ಅನ್ಯಥಾ
# endif
	undo_write_bytes(bi, (long_u)UF_VERSION, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    ಯದಿ (undo_write(bi, hash, (size_t)UNDO_HASH_SIZE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // buffer-specific data
    undo_write_bytes(bi, (long_u)buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_ಸಾಲು_ptr.ul_textlen, 4);
    ಯದಿ (buf->b_u_ಸಾಲು_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು,
			(size_t)buf->b_u_ಸಾಲು_ptr.ul_textlen) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    undo_write_bytes(bi, (long_u)buf->b_u_ಸಾಲು_lnum, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_ಸಾಲು_colnr, 4);

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

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
serialize_uhp(bufinfo_T *bi, u_header_T *uhp)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    u_entry_T	*uep;
    char_u	time_buf[8];

    ಯದಿ (undo_write_bytes(bi, (long_u)UF_HEADER_MAGIC, 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    put_header_ptr(bi, uhp->uh_next.ptr);
    put_header_ptr(bi, uhp->uh_prev.ptr);
    put_header_ptr(bi, uhp->uh_alt_next.ptr);
    put_header_ptr(bi, uhp->uh_alt_prev.ptr);
    undo_write_bytes(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_bytes(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_bytes(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    ಪ್ರತಿ (i = 0; i < NMARKS; ++i)
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
    ಪ್ರತಿ (uep = uhp->uh_entry; uep != ರಿಕ್ತ; uep = uep->ue_next)
    {
	undo_write_bytes(bi, (long_u)UF_ENTRY_MAGIC, 2);
	ಯದಿ (serialize_uep(bi, uep) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    undo_write_bytes(bi, (long_u)UF_ENTRY_END_MAGIC, 2);
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ u_header_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_header_T	*uhp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    u_entry_T	*uep, *last_uep;
    ಪೂರ್ಣಾಙ್ಕ		c;
    ಪೂರ್ಣಾಙ್ಕ		error;

    uhp = U_ALLOC_ಸಾಲು(ಪರಿಮಾಣ(u_header_T));
    ಯದಿ (uhp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    CLEAR_POINTER(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_next.seq = undo_read_4c(bi);
    uhp->uh_prev.seq = undo_read_4c(bi);
    uhp->uh_alt_next.seq = undo_read_4c(bi);
    uhp->uh_alt_prev.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    ಯದಿ (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    ಪ್ರತಿ (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_time = undo_read_time(bi);

    // Optional fields.
    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ len = undo_read_byte(bi);
	ಪೂರ್ಣಾಙ್ಕ what;

	ಯದಿ (len == EOF)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ (len == 0)
	    ಭಙ್ಗ;
	what = undo_read_byte(bi);
	ವಿಕಲ್ಪ (what)
	{
	    ಪಕ್ಷ UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		// field not supported, skip
		ಯಾವತ್ (--len >= 0)
		    (ಶೂನ್ಯ)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    last_uep = ರಿಕ್ತ;
    ಯಾವತ್ ((c = undo_read_2c(bi)) == UF_ENTRY_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	ಯದಿ (last_uep == ರಿಕ್ತ)
	    uhp->uh_entry = uep;
	ಅನ್ಯಥಾ
	    last_uep->ue_next = uep;
	last_uep = uep;
	ಯದಿ (uep == ರಿಕ್ತ || error)
	{
	    u_free_uhp(uhp);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಯದಿ (c != UF_ENTRY_END_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ uhp;
}

/*
 * Serialize "uep".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
serialize_uep(
    bufinfo_T	*bi,
    u_entry_T	*uep)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    undo_write_bytes(bi, (long_u)uep->ue_ತುದಿ, 4);
    undo_write_bytes(bi, (long_u)uep->ue_bot, 4);
    undo_write_bytes(bi, (long_u)uep->ue_lcount, 4);
    undo_write_bytes(bi, (long_u)uep->ue_size, 4);
    ಪ್ರತಿ (i = 0; i < uep->ue_size; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	ಯದಿ (undo_write_bytes(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_ಸಾಲು, uep->ue_array[i].ul_textlen) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ u_entry_T *
unserialize_uep(bufinfo_T *bi, ಪೂರ್ಣಾಙ್ಕ *error, char_u *file_name)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    u_entry_T	*uep;
    undoline_T	*array = ರಿಕ್ತ;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_len;

    uep = U_ALLOC_ಸಾಲು(ಪರಿಮಾಣ(u_entry_T));
    ಯದಿ (uep == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    CLEAR_POINTER(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_ತುದಿ = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_size = undo_read_4c(bi);
    ಯದಿ (uep->ue_size > 0)
    {
	ಯದಿ (uep->ue_size < LONG_MAX / (long)ಪರಿಮಾಣ(undoline_T))
	    array = U_ALLOC_ಸಾಲು(ಪರಿಮಾಣ(undoline_T) * uep->ue_size);
	ಯದಿ (array == ರಿಕ್ತ)
	{
	    *error = TRUE;
	    ಪ್ರತ್ಯಾಗಮ uep;
	}
	vim_memset(array, 0, ಪರಿಮಾಣ(undoline_T) * uep->ue_size);
    }
    uep->ue_array = array;

    ಪ್ರತಿ (i = 0; i < uep->ue_size; ++i)
    {
	ಸಾಲು_len = undo_read_4c(bi);
	ಯದಿ (ಸಾಲು_len >= 0)
	    ಸಾಲು = read_string_decrypt(bi, ಸಾಲು_len);
	ಅನ್ಯಥಾ
	{
	    ಸಾಲು = ರಿಕ್ತ;
	    corruption_error("line length", file_name);
	}
	ಯದಿ (ಸಾಲು == ರಿಕ್ತ)
	{
	    *error = TRUE;
	    ಪ್ರತ್ಯಾಗಮ uep;
	}
	array[i].ul_ಸಾಲು = ಸಾಲು;
	array[i].ul_len = ಸಾಲು_len + 1;
	array[i].ul_textlen = ಸಾಲು_len;
    }
    ಪ್ರತ್ಯಾಗಮ uep;
}

/*
 * Serialize "pos".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_bytes(bi, (long_u)pos.lnum, 4);
    undo_write_bytes(bi, (long_u)pos.col, 4);
    undo_write_bytes(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    ಯದಿ (pos->lnum < 0)
	pos->lnum = 0;
    pos->col = undo_read_4c(bi);
    ಯದಿ (pos->col < 0)
	pos->col = 0;
    pos->coladd = undo_read_4c(bi);
    ಯದಿ (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಶೂನ್ಯ
u_write_undo(
    char_u	*name,
    ಪೂರ್ಣಾಙ್ಕ		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_header_T	*uhp;
    char_u	*file_name;
    ಪೂರ್ಣಾಙ್ಕ		mark;
# ifdef U_DEBUG
    ಪೂರ್ಣಾಙ್ಕ		headers_written = 0;
# endif
    ಪೂರ್ಣಾಙ್ಕ		fd;
    FILE	*fp = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		perm;
    ಪೂರ್ಣಾಙ್ಕ		write_ok = FALSE;
# ifdef UNIX
    ಪೂರ್ಣಾಙ್ಕ		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);

    ಯದಿ (name == ರಿಕ್ತ)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	ಯದಿ (file_name == ರಿಕ್ತ)
	{
	    ಯದಿ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    ಯದಿ (buf->b_ffname != ರಿಕ್ತ)
    {
# ifdef UNIX
	ಯದಿ (mch_stat((ವರ್ಣ *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_mode;
	    st_old_valid = TRUE;
	}
# ಅನ್ಯಥಾ
	perm = mch_getperm(buf->b_ffname);
	ಯದಿ (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    ಯದಿ (mch_getperm(file_name) >= 0)
    {
	ಯದಿ (name == ರಿಕ್ತ || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((ವರ್ಣ *)file_name, O_RDONLY|O_EXTRA, 0);
	    ಯದಿ (fd < 0)
	    {
		ಯದಿ (name != ರಿಕ್ತ || p_verbose > 0)
		{
		    ಯದಿ (name == ರಿಕ್ತ)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    ಯದಿ (name == ರಿಕ್ತ)
			verbose_leave();
		}
		ಗಚ್ಛ theend;
	    }
	    ಅನ್ಯಥಾ
	    {
		char_u	mbuf[UF_START_MAGIC_LEN];
		ಪೂರ್ಣಾಙ್ಕ	len;

		len = read_eintr(fd, mbuf, UF_START_MAGIC_LEN);
		close(fd);
		ಯದಿ (len < UF_START_MAGIC_LEN
		      || memcmp(mbuf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
		{
		    ಯದಿ (name != ರಿಕ್ತ || p_verbose > 0)
		    {
			ಯದಿ (name == ರಿಕ್ತ)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			ಯದಿ (name == ರಿಕ್ತ)
			    verbose_leave();
		    }
		    ಗಚ್ಛ theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    ಯದಿ (buf->b_u_numhead == 0 && buf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು == ರಿಕ್ತ)
    {
	ಯದಿ (p_verbose > 0)
	    verb_ಸಂದೇಶ(_("Skipping undo file write, nothing to undo"));
	ಗಚ್ಛ theend;
    }

    fd = mch_open((ವರ್ಣ *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    ಯದಿ (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	ಗಚ್ಛ theend;
    }
    (ಶೂನ್ಯ)mch_setperm(file_name, perm);
    ಯದಿ (p_verbose > 0)
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
    ಯದಿ (st_old_valid
	    && mch_stat((ವರ್ಣ *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  ಯದಿ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    ಯದಿ (buf->b_ffname != ರಿಕ್ತ)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    ಯದಿ (fp == ರಿಕ್ತ)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	close(fd);
	mch_remove(file_name);
	ಗಚ್ಛ theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    ಯದಿ (serialize_header(&bi, hash) == FAIL)
	ಗಚ್ಛ write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    ಯಾವತ್ (uhp != ರಿಕ್ತ)
    {
	// Serialize current UHP if we haven't seen it
	ಯದಿ (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++headers_written;
# endif
	    ಯದಿ (serialize_uhp(&bi, uhp) == FAIL)
		ಗಚ್ಛ write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	ಯದಿ (uhp->uh_prev.ptr != ರಿಕ್ತ && uhp->uh_prev.ptr->uh_walk != mark)
	    uhp = uhp->uh_prev.ptr;
	ಅನ್ಯಥಾ ಯದಿ (uhp->uh_alt_next.ptr != ರಿಕ್ತ
				     && uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;
	ಅನ್ಯಥಾ ಯದಿ (uhp->uh_next.ptr != ರಿಕ್ತ && uhp->uh_alt_prev.ptr == ರಿಕ್ತ
					 && uhp->uh_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_next.ptr;
	ಅನ್ಯಥಾ ಯದಿ (uhp->uh_alt_prev.ptr != ರಿಕ್ತ)
	    uhp = uhp->uh_alt_prev.ptr;
	ಅನ್ಯಥಾ
	    uhp = uhp->uh_next.ptr;
    }

    ಯದಿ (undo_write_bytes(&bi, (long_u)UF_HEADER_END_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    ಯದಿ (headers_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", headers_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    ಯದಿ (bi.bi_state != ರಿಕ್ತ && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# ಯದಿ defined(UNIX) && defined(HAVE_FSYNC)
    ಯದಿ ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && fflush(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    fclose(fp);
    ಯದಿ (!write_ok)
	semsg(_(e_write_error_in_undo_file_str), file_name);

# ಯದಿ defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    ಯದಿ (buf->b_ffname != ರಿಕ್ತ)
	(ಶೂನ್ಯ)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    ಯದಿ (buf->b_ffname != ರಿಕ್ತ)
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
    ಯದಿ (bi.bi_state != ರಿಕ್ತ)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
# endif
    ಯದಿ (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
uhp_seq_cmp(ಧ್ರುವ ಶೂನ್ಯ *v1, ಧ್ರುವ ಶೂನ್ಯ *v2)
{
    ಧ್ರುವ u_header_T *u1 = *(u_header_T **)v1;
    ಧ್ರುವ u_header_T *u2 = *(u_header_T **)v2;

    ಪ್ರತ್ಯಾಗಮ u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    ಸ್ಥಿರ long
uhp_table_find(u_header_T **uhp_table, long ಅಂಕೆ_head, long seq)
{
    long    lo = 0;
    long    hi = ಅಂಕೆ_head - 1;

    ಯಾವತ್ (lo <= hi)
    {
	long mid = lo + (hi - lo) / 2;

	ಯದಿ (uhp_table[mid]->uh_seq < seq)
	    lo = mid + 1;
	ಅನ್ಯಥಾ ಯದಿ (uhp_table[mid]->uh_seq > seq)
	    hi = mid - 1;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ mid;
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    ಶೂನ್ಯ
u_read_undo(char_u *name, char_u *hash, char_u *orig_name UNUSED)
{
    char_u	*file_name;
    FILE	*fp;
    long	version, str_len;
    undoline_T	ಸಾಲು_ptr;
    linenr_T	ಸಾಲು_lnum;
    colnr_T	ಸಾಲು_colnr;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ;
    long	ಅಂಕೆ_head = 0;
    long	old_header_seq, new_header_seq, cur_header_seq;
    long	seq_last, seq_cur;
    long	last_save_nr = 0;
    long	old_idx = -1, new_idx = -1, cur_idx = -1;
    long	ಅಂಕೆ_read_uhps = 0;
    time_t	seq_time;
    long	i;
    ಪೂರ್ಣಾಙ್ಕ		c;
    u_header_T	*uhp;
    u_header_T	**uhp_table = ರಿಕ್ತ;
    char_u	read_hash[UNDO_HASH_SIZE];
    char_u	magic_buf[UF_START_MAGIC_LEN];
# ifdef U_DEBUG
    ಪೂರ್ಣಾಙ್ಕ		*uhp_table_used;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);
    ಸಾಲು_ptr.ul_len = 0;
    ಸಾಲು_ptr.ul_textlen = 0;
    ಸಾಲು_ptr.ul_ಸಾಲು = ರಿಕ್ತ;

    ಯದಿ (name == ರಿಕ್ತ)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	ಯದಿ (file_name == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	ಯದಿ (mch_stat((ವರ್ಣ *)orig_name, &st_orig) >= 0
		&& mch_stat((ವರ್ಣ *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    ಯದಿ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    ಪ್ರತ್ಯಾಗಮ;
	}
# endif
    }
    ಅನ್ಯಥಾ
	file_name = name;

    ಯದಿ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((ವರ್ಣ *)file_name, "r");
    ಯದಿ (fp == ರಿಕ್ತ)
    {
	ಯದಿ (name != ರಿಕ್ತ || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	ಗಚ್ಛ error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    ಯದಿ (fread(magic_buf, UF_START_MAGIC_LEN, 1, fp) != 1
		|| memcmp(magic_buf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	ಗಚ್ಛ error;
    }
    version = get2c(fp);
    ಯದಿ (version == UF_VERSION_CRYPT)
    {
# ifdef FEAT_CRYPT
	ಯದಿ (*curbuf->b_p_ಕೀಲಿ == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    ಗಚ್ಛ error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_ಕೀಲಿ);
	ಯದಿ (bi.bi_state == ರಿಕ್ತ)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    ಗಚ್ಛ error;
	}
	ಯದಿ (crypt_whole_undofile(bi.bi_state->method_nr))
	{
	    bi.bi_buffer = alloc(CRYPT_BUF_SIZE);
	    ಯದಿ (bi.bi_buffer == ರಿಕ್ತ)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = ರಿಕ್ತ;
		ಗಚ್ಛ error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_used = 0;
	}
# ಅನ್ಯಥಾ
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	ಗಚ್ಛ error;
# endif
    }
    ಅನ್ಯಥಾ ಯದಿ (version != UF_VERSION)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	ಗಚ್ಛ error;
    }

    ಯದಿ (undo_read(&bi, read_hash, (size_t)UNDO_HASH_SIZE) == FAIL)
    {
	corruption_error("hash", file_name);
	ಗಚ್ಛ error;
    }
    ಸಾಲು_ಎಣಿಕೆ = (linenr_T)undo_read_4c(&bi);
    ಯದಿ (memcmp(hash, read_hash, UNDO_HASH_SIZE) != 0
				  || ಸಾಲು_ಎಣಿಕೆ != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	ಯದಿ (p_verbose > 0 || name != ರಿಕ್ತ)
	{
	    ಯದಿ (name == ರಿಕ್ತ)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    ಯದಿ (name == ರಿಕ್ತ)
		verbose_leave();
	}
	ಗಚ್ಛ error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    ಯದಿ (str_len < 0)
	ಗಚ್ಛ error;
    ಯದಿ (str_len > 0)
    {
	ಸಾಲು_ptr.ul_ಸಾಲು = read_string_decrypt(&bi, str_len);
	ಸಾಲು_ptr.ul_len = str_len + 1;
	ಸಾಲು_ptr.ul_textlen = str_len;
    }
    ಸಾಲು_lnum = (linenr_T)undo_read_4c(&bi);
    ಸಾಲು_colnr = (colnr_T)undo_read_4c(&bi);
    ಯದಿ (ಸಾಲು_lnum < 0 || ಸಾಲು_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	ಗಚ್ಛ error;
    }

    // Begin general undo data
    old_header_seq = undo_read_4c(&bi);
    new_header_seq = undo_read_4c(&bi);
    cur_header_seq = undo_read_4c(&bi);
    ಅಂಕೆ_head = undo_read_4c(&bi);
    seq_last = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_time = undo_read_time(&bi);

    // Optional header fields.
    ಪ್ರತಿ (;;)
    {
	ಪೂರ್ಣಾಙ್ಕ len = undo_read_byte(&bi);
	ಪೂರ್ಣಾಙ್ಕ what;

	ಯದಿ (len == 0 || len == EOF)
	    ಭಙ್ಗ;
	what = undo_read_byte(&bi);
	ವಿಕಲ್ಪ (what)
	{
	    ಪಕ್ಷ UF_LAST_SAVE_NR:
		last_save_nr = undo_read_4c(&bi);
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		// field not supported, skip
		ಯಾವತ್ (--len >= 0)
		    (ಶೂನ್ಯ)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    ಯದಿ (ಅಂಕೆ_head > 0)
    {
	ಯದಿ (ಅಂಕೆ_head < LONG_MAX / (long)ಪರಿಮಾಣ(u_header_T *))
	    uhp_table = U_ALLOC_ಸಾಲು(ಅಂಕೆ_head * ಪರಿಮಾಣ(u_header_T *));
	ಯದಿ (uhp_table == ರಿಕ್ತ)
	    ಗಚ್ಛ error;
    }

    ಯಾವತ್ ((c = undo_read_2c(&bi)) == UF_HEADER_MAGIC)
    {
	ಯದಿ (ಅಂಕೆ_read_uhps >= ಅಂಕೆ_head)
	{
	    corruption_error("num_head too small", file_name);
	    ಗಚ್ಛ error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	ಯದಿ (uhp == ರಿಕ್ತ)
	    ಗಚ್ಛ error;
	uhp_table[ಅಂಕೆ_read_uhps++] = uhp;
    }

    ಯದಿ (ಅಂಕೆ_read_uhps != ಅಂಕೆ_head)
    {
	corruption_error("num_head", file_name);
	ಗಚ್ಛ error;
    }
    ಯದಿ (c != UF_HEADER_END_MAGIC)
    {
	corruption_error("end marker", file_name);
	ಗಚ್ಛ error;
    }

# ifdef U_DEBUG
    uhp_table_used = alloc_clear(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * ಅಂಕೆ_head + 1);
#  define SET_FLAG(j) ++uhp_table_used[j]
# ಅನ್ಯಥಾ
#  define SET_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    ಯದಿ (ಅಂಕೆ_head > 0)
	qsort(uhp_table, (size_t)ಅಂಕೆ_head, ಪರಿಮಾಣ(u_header_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_head - 1; i++)
    {
	ಯದಿ (uhp_table[i]->uh_seq == uhp_table[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    ಗಚ್ಛ error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    ಕುರು { \
	long fidx = uhp_table_find(uhp_table, ಅಂಕೆ_head, (link).seq); \
	\
	ಯದಿ (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_table[fidx]; \
	    SET_FLAG(fidx); \
	} \
	ಅನ್ಯಥಾ \
	    (link).ptr = ರಿಕ್ತ; \
    } ಯಾವತ್ (0)

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_head; i++)
    {
	uhp = uhp_table[i];
	SWIZZLE_SEQ(uhp->uh_next, i);
	SWIZZLE_SEQ(uhp->uh_prev, i);
	SWIZZLE_SEQ(uhp->uh_alt_next, i);
	SWIZZLE_SEQ(uhp->uh_alt_prev, i);
	ಯದಿ (old_header_seq > 0 && old_idx < 0 && uhp->uh_seq == old_header_seq)
	{
	    old_idx = i;
	    SET_FLAG(i);
	}
	ಯದಿ (new_header_seq > 0 && new_idx < 0 && uhp->uh_seq == new_header_seq)
	{
	    new_idx = i;
	    SET_FLAG(i);
	}
	ಯದಿ (cur_header_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_header_seq)
	{
	    cur_idx = i;
	    SET_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? ರಿಕ್ತ : uhp_table[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? ರಿಕ್ತ : uhp_table[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? ರಿಕ್ತ : uhp_table[cur_idx];
    curbuf->b_u_ಸಾಲು_ptr = ಸಾಲು_ptr;
    curbuf->b_u_ಸಾಲು_lnum = ಸಾಲು_lnum;
    curbuf->b_u_ಸಾಲು_colnr = ಸಾಲು_colnr;
    curbuf->b_u_numhead = ಅಂಕೆ_head;
    curbuf->b_u_seq_last = seq_last;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_time_cur = seq_time;
    curbuf->b_u_save_nr_last = last_save_nr;
    curbuf->b_u_save_nr_cur = last_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_table);

# ifdef U_DEBUG
    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_head; ++i)
	ಯದಿ (uhp_table_used[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_table_used);
    u_check(TRUE);
# endif

    ಯದಿ (name != ರಿಕ್ತ)
	smsg(_("Finished reading undo file %s"), file_name);
    ಗಚ್ಛ theend;

error:
    vim_free(ಸಾಲು_ptr.ul_ಸಾಲು);
    ಯದಿ (uhp_table != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_read_uhps; i++)
	    ಯದಿ (uhp_table[i] != ರಿಕ್ತ)
		u_free_uhp(uhp_table[i]);
	vim_free(uhp_table);
    }

theend:
# ifdef FEAT_CRYPT
    ಯದಿ (bi.bi_state != ರಿಕ್ತ)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
# endif
    ಯದಿ (fp != ರಿಕ್ತ)
	fclose(fp);
    ಯದಿ (file_name != name)
	vim_free(file_name);
    ಪ್ರತ್ಯಾಗಮ;
}

#endif // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    ಶೂನ್ಯ
u_undo(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    ಯದಿ (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	ಎಣಿಕೆ = 1;
    }

    ಯದಿ (vim_strchr(p_cpo, CPO_UNDO) == ರಿಕ್ತ)
	undo_undoes = TRUE;
    ಅನ್ಯಥಾ
	undo_undoes = !undo_undoes;
    u_doit(ಎಣಿಕೆ);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    ಶೂನ್ಯ
u_redo(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    ಯದಿ (vim_strchr(p_cpo, CPO_UNDO) == ರಿಕ್ತ)
	undo_undoes = FALSE;
    u_doit(ಎಣಿಕೆ);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_doit(ಪೂರ್ಣಾಙ್ಕ startcount)
{
    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = startcount;

    ಯದಿ (!undo_allowed())
	ಪ್ರತ್ಯಾಗಮ;

    u_newcount = 0;
    u_oldcount = 0;
    ಯದಿ (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;
    ಯಾವತ್ (ಎಣಿಕೆ--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	ಯದಿ (undo_undoes)
	{
	    ಯದಿ (curbuf->b_u_curhead == ರಿಕ್ತ)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    ಅನ್ಯಥಾ ಯದಿ (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_next.ptr;
	    // nothing to undo
	    ಯದಿ (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == ರಿಕ್ತ)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		ಯದಿ (ಎಣಿಕೆ == startcount - 1)
		{
		    ಯದಿ (!shortmess(SHM_UNDO))
			ಸಂದೇಶ(_("Already at oldest change"));
		    ಪ್ರತ್ಯಾಗಮ;
		}
		ಭಙ್ಗ;
	    }

	    u_undoredo(TRUE);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (curbuf->b_u_curhead == ರಿಕ್ತ || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		ಯದಿ (ಎಣಿಕೆ == startcount - 1)
		{
		    ಯದಿ (!shortmess(SHM_UNDO))
			ಸಂದೇಶ(_("Already at newest change"));
		    ಪ್ರತ್ಯಾಗಮ;
		}
		ಭಙ್ಗ;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    ಯದಿ (curbuf->b_u_curhead->uh_prev.ptr == ರಿಕ್ತ)
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
    ಶೂನ್ಯ
undo_time(
    long	step,
    ಪೂರ್ಣಾಙ್ಕ		sec,
    ಪೂರ್ಣಾಙ್ಕ		file,
    ಪೂರ್ಣಾಙ್ಕ		absolute)
{
    long	    target;
    long	    closest;
    long	    closest_start;
    long	    closest_seq = 0;
    long	    val;
    u_header_T	    *uhp = ರಿಕ್ತ;
    u_header_T	    *last;
    ಪೂರ್ಣಾಙ್ಕ		    mark;
    ಪೂರ್ಣಾಙ್ಕ		    nomark = 0;  // shut up compiler
    ಪೂರ್ಣಾಙ್ಕ		    round;
    ಪೂರ್ಣಾಙ್ಕ		    dosec = sec;
    ಪೂರ್ಣಾಙ್ಕ		    dofile = file;
    ಪೂರ್ಣಾಙ್ಕ		    above = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		    did_undo = TRUE;

    ಯದಿ (text_locked())
    {
	text_locked_ಸಂದೇಶ();
	ಪ್ರತ್ಯಾಗಮ;
    }

    // First make sure the current undoable change is synced.
    ಯದಿ (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    ಯದಿ (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    ಯದಿ (absolute)
    {
	target = step;
	closest = -1;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (dosec)
	    target = (long)(curbuf->b_u_time_cur) + step;
	ಅನ್ಯಥಾ ಯದಿ (dofile)
	{
	    ಯದಿ (step < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		ಯದಿ (uhp != ರಿಕ್ತ)
		    uhp = uhp->uh_next.ptr;
		ಅನ್ಯಥಾ
		    uhp = curbuf->b_u_newhead;
		ಯದಿ (uhp != ರಿಕ್ತ && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    target = curbuf->b_u_save_nr_cur + step;
		ಅನ್ಯಥಾ
		    // count the changes since the last write as one step
		    target = curbuf->b_u_save_nr_cur + step + 1;
		ಯದಿ (target <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Moving forward to a newer write.
		target = curbuf->b_u_save_nr_cur + step;
		ಯದಿ (target > curbuf->b_u_save_nr_last)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    target = curbuf->b_u_seq_last + 1;
		    dofile = FALSE;
		}
	    }
	}
	ಅನ್ಯಥಾ
	    target = curbuf->b_u_seq_cur + step;
	ಯದಿ (step < 0)
	{
	    ಯದಿ (target < 0)
		target = 0;
	    closest = -1;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (dosec)
		closest = (long)(vim_time() + 1);
	    ಅನ್ಯಥಾ ಯದಿ (dofile)
		closest = curbuf->b_u_save_nr_last + 2;
	    ಅನ್ಯಥಾ
		closest = curbuf->b_u_seq_last + 2;
	    ಯದಿ (target >= closest)
		target = closest - 1;
	}
    }
    closest_start = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    ಯದಿ (target == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	ಗಚ್ಛ target_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    ಪ್ರತಿ (round = 1; round <= 2; ++round)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	ಯದಿ (curbuf->b_u_curhead == ರಿಕ್ತ)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	ಅನ್ಯಥಾ
	    uhp = curbuf->b_u_curhead;

	ಯಾವತ್ (uhp != ರಿಕ್ತ)
	{
	    uhp->uh_walk = mark;
	    ಯದಿ (dosec)
		val = (long)(uhp->uh_time);
	    ಅನ್ಯಥಾ ಯದಿ (dofile)
		val = uhp->uh_save_nr;
	    ಅನ್ಯಥಾ
		val = uhp->uh_seq;

	    ಯದಿ (round == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		ಯದಿ ((step < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
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
	    ಯದಿ (target == val && !dosec)
	    {
		target = uhp->uh_seq;
		ಭಙ್ಗ;
	    }

	    // go down in the tree if we haven't been there
	    ಯದಿ (uhp->uh_prev.ptr != ರಿಕ್ತ && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
		uhp = uhp->uh_prev.ptr;

	    // go to alternate branch if we haven't been there
	    ಅನ್ಯಥಾ ಯದಿ (uhp->uh_alt_next.ptr != ರಿಕ್ತ
		    && uhp->uh_alt_next.ptr->uh_walk != nomark
		    && uhp->uh_alt_next.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_next.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    ಅನ್ಯಥಾ ಯದಿ (uhp->uh_next.ptr != ರಿಕ್ತ && uhp->uh_alt_prev.ptr == ರಿಕ್ತ
		    && uhp->uh_next.ptr->uh_walk != nomark
		    && uhp->uh_next.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		ಯದಿ (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_next.ptr;
	    }

	    ಅನ್ಯಥಾ
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		ಯದಿ (uhp->uh_alt_prev.ptr != ರಿಕ್ತ)
		    uhp = uhp->uh_alt_prev.ptr;
		ಅನ್ಯಥಾ
		    uhp = uhp->uh_next.ptr;
	    }
	}

	ಯದಿ (uhp != ರಿಕ್ತ)    // found it
	    ಭಙ್ಗ;

	ಯದಿ (absolute)
	{
	    semsg(_(e_undo_ಸಂಖ್ಯೆ_nr_not_found), step);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (closest == closest_start)
	{
	    ಯದಿ (!shortmess(SHM_UNDO))
	    {
		ಯದಿ (step < 0)
		    ಸಂದೇಶ(_("Already at oldest change"));
		ಅನ್ಯಥಾ
		    ಸಂದೇಶ(_("Already at newest change"));
	    }
	    ಪ್ರತ್ಯಾಗಮ;
	}

	target = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	ಯದಿ (step < 0)
	    above = TRUE;	// stop above the header
    }

target_zero:
    // If we found it: Follow the path to go to where we want to be.
    ಯದಿ (uhp != ರಿಕ್ತ || target == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	ಯಾವತ್ (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    ಯದಿ (uhp == ರಿಕ್ತ)
		uhp = curbuf->b_u_newhead;
	    ಅನ್ಯಥಾ
		uhp = uhp->uh_next.ptr;
	    ಯದಿ (uhp == ರಿಕ್ತ || (target > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == target && !above))
		ಭಙ್ಗ;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    ಯದಿ (target > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	ಯದಿ (target > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    ಯಾವತ್ (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		ಯದಿ (uhp == ರಿಕ್ತ)
		    ಭಙ್ಗ;

		// Go back to the first branch with a mark.
		ಯಾವತ್ (uhp->uh_alt_prev.ptr != ರಿಕ್ತ
				     && uhp->uh_alt_prev.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_prev.ptr;

		// Find the last branch with a mark, that's the one.
		last = uhp;
		ಯಾವತ್ (last->uh_alt_next.ptr != ರಿಕ್ತ
				    && last->uh_alt_next.ptr->uh_walk == mark)
		    last = last->uh_alt_next.ptr;
		ಯದಿ (last != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    ಯಾವತ್ (uhp->uh_alt_prev.ptr != ರಿಕ್ತ)
			uhp = uhp->uh_alt_prev.ptr;
		    ಯದಿ (last->uh_alt_next.ptr != ರಿಕ್ತ)
			last->uh_alt_next.ptr->uh_alt_prev.ptr =
							last->uh_alt_prev.ptr;
		    last->uh_alt_prev.ptr->uh_alt_next.ptr =
							last->uh_alt_next.ptr;
		    last->uh_alt_prev.ptr = ರಿಕ್ತ;
		    last->uh_alt_next.ptr = uhp;
		    uhp->uh_alt_prev.ptr = last;

		    ಯದಿ (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = last;
		    uhp = last;
		    ಯದಿ (uhp->uh_next.ptr != ರಿಕ್ತ)
			uhp->uh_next.ptr->uh_prev.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		ಯದಿ (uhp->uh_walk != mark)
		    ಭಙ್ಗ;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		ಯದಿ (uhp->uh_seq == target && above)
		{
		    curbuf->b_u_seq_cur = target - 1;
		    ಭಙ್ಗ;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		ಯದಿ (uhp->uh_prev.ptr == ರಿಕ್ತ)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_prev.ptr;
		did_undo = FALSE;

		ಯದಿ (uhp->uh_seq == target)	// found it!
		    ಭಙ್ಗ;

		uhp = uhp->uh_prev.ptr;
		ಯದಿ (uhp == ರಿಕ್ತ || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    internal_error("undo_time()");
		    ಭಙ್ಗ;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
u_undoredo(ಪೂರ್ಣಾಙ್ಕ undo)
{
    undoline_T	*newarray = ರಿಕ್ತ;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	ತುದಿ, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    long	i;
    u_entry_T	*uep, *nuep;
    u_entry_T	*newlist = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		old_flags;
    ಪೂರ್ಣಾಙ್ಕ		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    ಪೂರ್ಣಾಙ್ಕ		empty_buffer;		    // buffer became empty
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
    mch_memmove(namedm, curbuf->b_namedm, ಪರಿಮಾಣ(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_start.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    curbuf->b_op_start.col = 0;
    curbuf->b_op_end.lnum = 0;
    curbuf->b_op_end.col = 0;

    ಪ್ರತಿ (uep = curhead->uh_entry; uep != ರಿಕ್ತ; uep = nuep)
    {
	ತುದಿ = uep->ue_ತುದಿ;
	bot = uep->ue_bot;
	ಯದಿ (bot == 0)
	    bot = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
	ಯದಿ (ತುದಿ > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || ತುದಿ >= bot
				      || bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_ಸಾಲು_numbers_wrong);
	    changed();		// don't want UNCHANGED now
	    ಪ್ರತ್ಯಾಗಮ;
	}

	oldsize = bot - ತುದಿ - 1;    // number of lines before undo
	newsize = uep->ue_size;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    ಯದಿ (lnum >= ತುದಿ && lnum <= ತುದಿ + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (ತುದಿ < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		ಪ್ರತಿ (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(ತುದಿ + 1 + i);

		    ಯದಿ (curbuf->b_ml.ml_ಸಾಲು_len != uep->ue_array[i].ul_len
			    || memcmp(uep->ue_array[i].ul_ಸಾಲು, p,
						curbuf->b_ml.ml_ಸಾಲು_len) != 0)
			ಭಙ್ಗ;
		}
		ಯದಿ (i == newsize && newlnum == MAXLNUM && uep->ue_next == ರಿಕ್ತ)
		{
		    newlnum = ತುದಿ;
		    new_curpos.lnum = newlnum + 1;
		}
		ಅನ್ಯಥಾ ಯದಿ (i < newsize)
		{
		    newlnum = ತುದಿ + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	empty_buffer = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	ಯದಿ (oldsize > 0)
	{
	    ಯದಿ ((newarray = U_ALLOC_ಸಾಲು(ಪರಿಮಾಣ(undoline_T) * oldsize)) == ರಿಕ್ತ)
	    {
		do_outofmem_ಸಂದೇಶ((long_u)(ಪರಿಮಾಣ(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		ಯಾವತ್ (uep != ರಿಕ್ತ)
		{
		    nuep = uep->ue_next;
		    u_freeentry(uep, uep->ue_size);
		    uep = nuep;
		}
		ಭಙ್ಗ;
	    }
	    // delete backwards, it goes faster in most cases
	    ಪ್ರತಿ (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		ಯದಿ (u_save_ಸಾಲು(&newarray[i], lnum) == FAIL)
		    do_outofmem_ಸಂದೇಶ((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		ಯದಿ (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 1)
		    empty_buffer = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	ಅನ್ಯಥಾ
	    newarray = ರಿಕ್ತ;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	ಯದಿ (newsize)
	{
	    ಪ್ರತಿ (lnum = ತುದಿ, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		ಯದಿ (empty_buffer && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_ಸಾಲು,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		ಅನ್ಯಥಾ
		    ml_append_flags(lnum, uep->ue_array[i].ul_ಸಾಲು,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_ಸಾಲು);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#ifdef FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, ತುದಿ, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, ತುದಿ + newsize + 1, &uep->ue_props,
								UNDOPROP_BELOW);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, ತುದಿ, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_BELOW);
	ಯದಿ (newsize > 0)
	    adjust_props_for_undo(curbuf, ತುದಿ + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_BELOW);
#endif

	// adjust marks
	ಯದಿ (oldsize != newsize)
	{
	    mark_adjust(ತುದಿ + 1, ತುದಿ + oldsize, (long)MAXLNUM,
					       (long)newsize - (long)oldsize);
	    ಯದಿ (curbuf->b_op_start.lnum > ತುದಿ + oldsize)
		curbuf->b_op_start.lnum += newsize - oldsize;
	    ಯದಿ (curbuf->b_op_end.lnum > ತುದಿ + oldsize)
		curbuf->b_op_end.lnum += newsize - oldsize;
	}
	ಯದಿ (oldsize > 0 || newsize > 0)
	{
	    changed_ಸಾಲುಗಳು(ತುದಿ + 1, 0, bot, newsize - oldsize);
#ifdef FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    ಯದಿ (spell_check_window(curwin) && bot <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		redrawWinline(curwin, bot);
#endif
	}

	// Set the '[ mark.
	ಯದಿ (ತುದಿ + 1 < curbuf->b_op_start.lnum)
	    curbuf->b_op_start.lnum = ತುದಿ + 1;
	// Set the '] mark.
	ಯದಿ (newsize == 0 && ತುದಿ + 1 > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = ತುದಿ + 1;
	ಅನ್ಯಥಾ ಯದಿ (ತುದಿ + newsize > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = ತುದಿ + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_size = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = ತುದಿ + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_next;
	uep->ue_next = newlist;
	newlist = uep;
    }

    // Ensure the '[ and '] marks are within bounds.
    ಯದಿ (curbuf->b_op_start.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	 curbuf->b_op_start.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಯದಿ (curbuf->b_op_end.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	 curbuf->b_op_end.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_entry = newlist;
    curhead->uh_flags = new_flags;
    ಯದಿ ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_EMPTY;
    ಯದಿ (old_flags & UH_CHANGED)
	changed();
    ಅನ್ಯಥಾ
#ifdef FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	ಯದಿ (!isNetbeansModified(curbuf))
#endif
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    ಪ್ರತಿ (i = 0; i < NMARKS; ++i)
    {
	ಯದಿ (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	ಯದಿ (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	ಅನ್ಯಥಾ
	    curhead->uh_namedm[i].lnum = 0;
    }
    ಯದಿ (curhead->uh_visual.vi_start.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    ಯದಿ (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    ಯದಿ (curwin->w_cursor.lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	ಯದಿ (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = curhead->uh_cursor.col;
	    ಯದಿ (virtual_active() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    ಅನ್ಯಥಾ
		curwin->w_cursor.coladd = 0;
	}
	ಅನ್ಯಥಾ
	    beginline(BL_SOL | BL_FIX);
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	ಯದಿ (curhead->uh_next.ptr != ರಿಕ್ತ)
	    curbuf->b_u_seq_cur = curhead->uh_next.ptr->uh_seq;
	ಅನ್ಯಥಾ
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    ಯದಿ (curhead->uh_save_nr != 0)
    {
	ಯದಿ (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
u_undo_end(
    ಪೂರ್ಣಾಙ್ಕ		did_undo,	// just did an undo
    ಪೂರ್ಣಾಙ್ಕ		absolute)	// used ":undo N"
{
    ವರ್ಣ	*msgstr;
    u_header_T	*uhp;
    char_u	msgbuf[80];

#ifdef FEAT_FOLDING
    ಯದಿ ((fdo_flags & FDO_UNDO) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#endif

    ಯದಿ (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (curbuf->b_ml.ml_flags & ML_EMPTY)
	--u_newcount;

    u_oldcount -= u_newcount;
    ಯದಿ (u_oldcount == -1)
	msgstr = N_("more line");
    ಅನ್ಯಥಾ ಯದಿ (u_oldcount < 0)
	msgstr = N_("more lines");
    ಅನ್ಯಥಾ ಯದಿ (u_oldcount == 1)
	msgstr = N_("line less");
    ಅನ್ಯಥಾ ಯದಿ (u_oldcount > 1)
	msgstr = N_("fewer lines");
    ಅನ್ಯಥಾ
    {
	u_oldcount = u_newcount;
	ಯದಿ (u_newcount == 1)
	    msgstr = N_("change");
	ಅನ್ಯಥಾ
	    msgstr = N_("changes");
    }

    ಯದಿ (curbuf->b_u_curhead != ರಿಕ್ತ)
    {
	// For ":undo N" we prefer a "after #N" message.
	ಯದಿ (absolute && curbuf->b_u_curhead->uh_next.ptr != ರಿಕ್ತ)
	{
	    uhp = curbuf->b_u_curhead->uh_next.ptr;
	    did_undo = FALSE;
	}
	ಅನ್ಯಥಾ ಯದಿ (did_undo)
	    uhp = curbuf->b_u_curhead;
	ಅನ್ಯಥಾ
	    uhp = curbuf->b_u_curhead->uh_next.ptr;
    }
    ಅನ್ಯಥಾ
	uhp = curbuf->b_u_newhead;

    ಯದಿ (uhp == ರಿಕ್ತ)
	*msgbuf = NUL;
    ಅನ್ಯಥಾ
	add_time(msgbuf, ಪರಿಮಾಣ(msgbuf), uhp->uh_time);

#ifdef FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ಯದಿ (wp->w_buffer == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#endif
    ಯದಿ (VIsual_active)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == ರಿಕ್ತ ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    ಶೂನ್ಯ
u_sync(
    ಪೂರ್ಣಾಙ್ಕ	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    ಯದಿ (curbuf->b_u_synced || (!force && no_u_sync > 0))
	ಪ್ರತ್ಯಾಗಮ;
#ಯದಿ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ಯದಿ (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	ಪ್ರತ್ಯಾಗಮ;		    // XIM is busy, don't break an undo sequence
#endif
    ಯದಿ (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    ಅನ್ಯಥಾ
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = ರಿಕ್ತ;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    ಶೂನ್ಯ
ex_undolist(exarg_T *eap UNUSED)
{
    garray_T	ga;
    u_header_T	*uhp;
    ಪೂರ್ಣಾಙ್ಕ		mark;
    ಪೂರ್ಣಾಙ್ಕ		nomark;
    ಪೂರ್ಣಾಙ್ಕ		changes = 1;
    ಪೂರ್ಣಾಙ್ಕ		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ *), 20);

    uhp = curbuf->b_u_oldhead;
    ಯಾವತ್ (uhp != ರಿಕ್ತ)
    {
	ಯದಿ (uhp->uh_prev.ptr == ರಿಕ್ತ && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    ಯದಿ (ga_grow(&ga, 1) == FAIL)
		ಭಙ್ಗ;
	    len = vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    add_time(IObuff + len, IOSIZE - len, uhp->uh_time);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (ಪೂರ್ಣಾಙ್ಕ)STRLEN(IObuff + len);
	    ಯದಿ (uhp->uh_save_nr > 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((ವರ್ಣ *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_data))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	ಯದಿ (uhp->uh_prev.ptr != ರಿಕ್ತ && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_prev.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	ಅನ್ಯಥಾ ಯದಿ (uhp->uh_alt_next.ptr != ರಿಕ್ತ
		&& uhp->uh_alt_next.ptr->uh_walk != nomark
		&& uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	ಅನ್ಯಥಾ ಯದಿ (uhp->uh_next.ptr != ರಿಕ್ತ && uhp->uh_alt_prev.ptr == ರಿಕ್ತ
		&& uhp->uh_next.ptr->uh_walk != nomark
		&& uhp->uh_next.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_next.ptr;
	    --changes;
	}

	ಅನ್ಯಥಾ
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    ಯದಿ (uhp->uh_alt_prev.ptr != ರಿಕ್ತ)
		uhp = uhp->uh_alt_prev.ptr;
	    ಅನ್ಯಥಾ
	    {
		uhp = uhp->uh_next.ptr;
		--changes;
	    }
	}
    }

    ಯದಿ (ga.ga_len == 0)
	ಸಂದೇಶ(_("Nothing to undo"));
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ		i;

	sort_strings((char_u **)ga.ga_data, ga.ga_len);

	ಸಂದೇಶ_start();
	ಸಂದೇಶ_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	ಪ್ರತಿ (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    ಸಂದೇಶ_putchar('\n');
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
	    ಸಂದೇಶ_puts(((ವರ್ಣ **)ga.ga_data)[i]);
	}
	ಸಂದೇಶ_end();

	ga_clear_strings(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    ಶೂನ್ಯ
ex_undojoin(exarg_T *eap UNUSED)
{
    ಯದಿ (curbuf->b_u_newhead == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;		    // nothing changed before
    ಯದಿ (curbuf->b_u_curhead != ರಿಕ್ತ)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (!curbuf->b_u_synced)
	ಪ್ರತ್ಯಾಗಮ;		    // already unsynced
    ಯದಿ (get_undolevel() < 0)
	ಪ್ರತ್ಯಾಗಮ;		    // no entries, nothing to do
    ಅನ್ಯಥಾ
	// Append next change to the last entry
	curbuf->b_u_synced = false;
}

/*
 * Called after writing or reloading the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
    ಶೂನ್ಯ
u_unchanged(buf_T *buf)
{
    u_unch_branch(buf->b_u_oldhead);
    buf->b_did_warn = false;
}

/*
 * After reloading a buffer which was saved for 'undoreload': Find the first
 * line that was changed and set the cursor there.
 */
    ಶೂನ್ಯ
u_find_first_changed(ಶೂನ್ಯ)
{
    u_header_T	*uhp = curbuf->b_u_newhead;
    u_entry_T   *uep;
    linenr_T	lnum;

    ಯದಿ (curbuf->b_u_curhead != ರಿಕ್ತ || uhp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_entry;
    ಯದಿ (uep->ue_ತುದಿ != 0 || uep->ue_bot != 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (lnum = 1; lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
					      && lnum <= uep->ue_size; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	ಯದಿ (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_ಸಾಲು_len
		|| memcmp(p, uep->ue_array[lnum - 1].ul_ಸಾಲು, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಯದಿ (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ != uep->ue_size)
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
    ಶೂನ್ಯ
u_update_save_nr(buf_T *buf)
{
    u_header_T	*uhp;

    ++buf->b_u_save_nr_last;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_last;
    uhp = buf->b_u_curhead;
    ಯದಿ (uhp != ರಿಕ್ತ)
	uhp = uhp->uh_next.ptr;
    ಅನ್ಯಥಾ
	uhp = buf->b_u_newhead;
    ಯದಿ (uhp != ರಿಕ್ತ)
	uhp->uh_save_nr = buf->b_u_save_nr_last;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
u_unch_branch(u_header_T *uhp)
{
    u_header_T	*uh;

    ಪ್ರತಿ (uh = uhp; uh != ರಿಕ್ತ; uh = uh->uh_prev.ptr)
    {
	uh->uh_flags |= UH_CHANGED;
	ಯದಿ (uh->uh_alt_next.ptr != ರಿಕ್ತ)
	    u_unch_branch(uh->uh_alt_next.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    ಸ್ಥಿರ u_entry_T *
u_get_headentry(ಶೂನ್ಯ)
{
    ಯದಿ (curbuf->b_u_newhead == ರಿಕ್ತ || curbuf->b_u_newhead->uh_entry == ರಿಕ್ತ)
    {
	iemsg(e_undo_list_corrupt);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ curbuf->b_u_newhead->uh_entry;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_getbot(ಶೂನ್ಯ)
{
    u_entry_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    ಯದಿ (uep == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    uep = curbuf->b_u_newhead->uh_getbot_entry;
    ಯದಿ (uep != ರಿಕ್ತ)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - uep->ue_lcount;
	uep->ue_bot = uep->ue_ತುದಿ + uep->ue_size + 1 + extra;
	ಯದಿ (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    iemsg(e_undo_ಸಾಲು_missing);
	    uep->ue_bot = uep->ue_ತುದಿ + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_entry = ರಿಕ್ತ;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_freeheader(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    ಯದಿ (uhp->uh_alt_next.ptr != ರಿಕ್ತ)
	u_freebranch(buf, uhp->uh_alt_next.ptr, uhpp);

    ಯದಿ (uhp->uh_alt_prev.ptr != ರಿಕ್ತ)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = ರಿಕ್ತ;

    // Update the links in the list to remove the header.
    ಯದಿ (uhp->uh_next.ptr == ರಿಕ್ತ)
	buf->b_u_oldhead = uhp->uh_prev.ptr;
    ಅನ್ಯಥಾ
	uhp->uh_next.ptr->uh_prev.ptr = uhp->uh_prev.ptr;

    ಯದಿ (uhp->uh_prev.ptr == ರಿಕ್ತ)
	buf->b_u_newhead = uhp->uh_next.ptr;
    ಅನ್ಯಥಾ
	ಪ್ರತಿ (uhap = uhp->uh_prev.ptr; uhap != ರಿಕ್ತ;
						 uhap = uhap->uh_alt_next.ptr)
	    uhap->uh_next.ptr = uhp->uh_next.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_freebranch(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *tofree, *next;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    ಯದಿ (uhp == buf->b_u_oldhead)
    {
	ಯಾವತ್ (buf->b_u_oldhead != ರಿಕ್ತ)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (uhp->uh_alt_prev.ptr != ರಿಕ್ತ)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = ರಿಕ್ತ;

    next = uhp;
    ಯಾವತ್ (next != ರಿಕ್ತ)
    {
	tofree = next;
	ಯದಿ (tofree->uh_alt_next.ptr != ರಿಕ್ತ)
	    u_freebranch(buf, tofree->uh_alt_next.ptr, uhpp);   // recursive
	next = tofree->uh_prev.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}

/*
 * Free all the undo entries for one header and the header itself.
 * This means that "uhp" is invalid when returning.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_freeentries(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_entry_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    ಯದಿ (buf->b_u_curhead == uhp)
	buf->b_u_curhead = ರಿಕ್ತ;
    ಯದಿ (buf->b_u_newhead == uhp)
	buf->b_u_newhead = ರಿಕ್ತ;  // freeing the newest entry
    ಯದಿ (uhpp != ರಿಕ್ತ && uhp == *uhpp)
	*uhpp = ರಿಕ್ತ;

    ಪ್ರತಿ (uep = uhp->uh_entry; uep != ರಿಕ್ತ; uep = nuep)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
u_freeentry(u_entry_T *uep, long n)
{
    ಯಾವತ್ (n > 0)
	vim_free(uep->ue_array[--n].ul_ಸಾಲು);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = ರಿಕ್ತ;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು = ರಿಕ್ತ;
    buf->b_u_ಸಾಲು_ptr.ul_len = 0;
    buf->b_u_ಸಾಲು_ptr.ul_textlen = 0;
    buf->b_u_ಸಾಲು_lnum = 0;
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_blockfree(buf_T *buf)
{
    ಯಾವತ್ (buf->b_u_oldhead != ರಿಕ್ತ)
	u_freeheader(buf, buf->b_u_oldhead, ರಿಕ್ತ);
    vim_free(buf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು);
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 * and invalidate the undo buffer
 */
    ಶೂನ್ಯ
u_clearallandblockfree(buf_T *buf)
{
    u_blockfree(buf);
    u_clearall(buf);
}



/*
 * Save the line "lnum" for the "U" command.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_saveline(linenr_T lnum)
{
    ಯದಿ (lnum == curbuf->b_u_ಸಾಲು_lnum)	    // line is already saved
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ) // should never happen
	ಪ್ರತ್ಯಾಗಮ;
    u_clearline();
    curbuf->b_u_ಸಾಲು_lnum = lnum;
    ಯದಿ (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_ಸಾಲು_colnr = curwin->w_cursor.col;
    ಅನ್ಯಥಾ
	curbuf->b_u_ಸಾಲು_colnr = 0;
    ಯದಿ (u_save_ಸಾಲು(&curbuf->b_u_ಸಾಲು_ptr, lnum) == FAIL)
	do_outofmem_ಸಂದೇಶ((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    ಶೂನ್ಯ
u_clearline(ಶೂನ್ಯ)
{
    ಯದಿ (curbuf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    VIM_CLEAR(curbuf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು);
    curbuf->b_u_ಸಾಲು_ptr.ul_len = 0;
    curbuf->b_u_ಸಾಲು_ptr.ul_textlen = 0;
    curbuf->b_u_ಸಾಲು_lnum = 0;
}

/*
 * Implementation of the "U" command.
 * Differentiation from vi: "U" can be undone with the next "U".
 * We also allow the cursor to be in another line.
 * Careful: may trigger autocommands that reload the buffer.
 */
    ಶೂನ್ಯ
u_undoline(ಶೂನ್ಯ)
{
    colnr_T	t;
    undoline_T  oldp;

    ಯದಿ (undo_off)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (curbuf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು == ರಿಕ್ತ
			|| curbuf->b_u_ಸಾಲು_lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	beep_flush();
	ಪ್ರತ್ಯಾಗಮ;
    }

    // first save the line for the 'u' command
    ಯದಿ (u_savecommon(curbuf->b_u_ಸಾಲು_lnum - 1,
		       curbuf->b_u_ಸಾಲು_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (u_save_ಸಾಲು(&oldp, curbuf->b_u_ಸಾಲು_lnum) == FAIL)
    {
	do_outofmem_ಸಂದೇಶ((long_u)0);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ml_replace_len(curbuf->b_u_ಸಾಲು_lnum, curbuf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು,
				     curbuf->b_u_ಸಾಲು_ptr.ul_len, TRUE, FALSE);
    changed_bytes(curbuf->b_u_ಸಾಲು_lnum, 0);
    curbuf->b_u_ಸಾಲು_ptr = oldp;

    t = curbuf->b_u_ಸಾಲು_colnr;
    ಯದಿ (curwin->w_cursor.lnum == curbuf->b_u_ಸಾಲು_lnum)
	curbuf->b_u_ಸಾಲು_colnr = curwin->w_cursor.col;
    curwin->w_cursor.col = t;
    curwin->w_cursor.lnum = curbuf->b_u_ಸಾಲು_lnum;
    check_cursor_col();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    ಪೂರ್ಣಾಙ್ಕ
bufIsChanged(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    ಯದಿ (term_job_running_not_none(buf->b_term))
	ಪ್ರತ್ಯಾಗಮ TRUE;
#endif
    ಪ್ರತ್ಯಾಗಮ bufIsChangedNotTerm(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    ಪೂರ್ಣಾಙ್ಕ
anyBufIsChanged(ಶೂನ್ಯ)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	ಯದಿ (bufIsChanged(buf))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    ಪೂರ್ಣಾಙ್ಕ
bufIsChangedNotTerm(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    ಪ್ರತ್ಯಾಗಮ (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_changed || file_ff_differs(buf, TRUE));
}

    ಪೂರ್ಣಾಙ್ಕ
curbufIsChanged(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ bufIsChanged(curbuf);
}

#ಯದಿ defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_eval_ವೃಕ್ಷ(buf_T *buf, u_header_T *first_uhp, list_T *list)
{
    u_header_T  *uhp = first_uhp;
    dict_T	*dict;

    ಯಾವತ್ (uhp != ರಿಕ್ತ)
    {
	dict = dict_alloc();
	ಯದಿ (dict == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	dict_add_ಸಂಖ್ಯೆ(dict, "seq", uhp->uh_seq);
	dict_add_ಸಂಖ್ಯೆ(dict, "time", (long)uhp->uh_time);
	ಯದಿ (uhp == buf->b_u_newhead)
	    dict_add_ಸಂಖ್ಯೆ(dict, "newhead", 1);
	ಯದಿ (uhp == buf->b_u_curhead)
	    dict_add_ಸಂಖ್ಯೆ(dict, "curhead", 1);
	ಯದಿ (uhp->uh_save_nr > 0)
	    dict_add_ಸಂಖ್ಯೆ(dict, "save", uhp->uh_save_nr);

	ಯದಿ (uhp->uh_alt_next.ptr != ರಿಕ್ತ)
	{
	    list_T	*alt_list = list_alloc();

	    ಯದಿ (alt_list != ರಿಕ್ತ)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_ವೃಕ್ಷ(buf, uhp->uh_alt_next.ptr, alt_list);
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
    ಶೂನ್ಯ
f_undofile(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	ಯದಿ (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    char_u *ffname = FullName_save(fname, TRUE);

	    ಯದಿ (ffname != ರಿಕ್ತ)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# ಅನ್ಯಥಾ
    rettv->vval.v_string = ರಿಕ್ತ;
# endif
}
# ifdef FEAT_PERSISTENT_UNDO
/*
 * Reset undofile option and delete the undofile
 */
    ಶೂನ್ಯ
u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    ಯದಿ (!buf->b_p_udf)
	ಪ್ರತ್ಯಾಗಮ;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    ಯದಿ (file_name != ರಿಕ್ತ)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    set_option_ಮೌಲ್ಯ_give_err((char_u *)"undofile", 0L, ರಿಕ್ತ, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    ಶೂನ್ಯ
f_undotree(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_ಸಂಖ್ಯೆ(dict, "synced", (long)buf->b_u_synced);
    dict_add_ಸಂಖ್ಯೆ(dict, "seq_last", buf->b_u_seq_last);
    dict_add_ಸಂಖ್ಯೆ(dict, "save_last", buf->b_u_save_nr_last);
    dict_add_ಸಂಖ್ಯೆ(dict, "seq_cur", buf->b_u_seq_cur);
    dict_add_ಸಂಖ್ಯೆ(dict, "time_cur", (long)buf->b_u_time_cur);
    dict_add_ಸಂಖ್ಯೆ(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    ಯದಿ (list != ರಿಕ್ತ)
    {
	u_eval_ವೃಕ್ಷ(buf, buf->b_u_oldhead, list);
	dict_add_list(dict, "entries", list);
    }
}

#endif
