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

#അന്തര്ഭാവ "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
തരനിർവചനം ഘടന {
    buf_T	*bi_buf;
    FILE	*bi_fp;
#ifdef FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_buffer; // CRYPT_BUF_SIZE, NULL when not buffering
    size_t	bi_used;    // bytes written to/read from bi_buffer
    size_t	bi_avail;   // bytes available in bi_buffer
#endif
} bufinfo_T;


സ്ഥിതം ശൂന്യം u_unch_branch(u_header_T *uhp);
സ്ഥിതം u_entry_T *u_get_headentry(ശൂന്യം);
സ്ഥിതം ശൂന്യം u_getbot(ശൂന്യം);
സ്ഥിതം ശൂന്യം u_doit(പൂർണ്ണസംഖ്യ എണ്ണം);
സ്ഥിതം ശൂന്യം u_undoredo(പൂർണ്ണസംഖ്യ undo);
സ്ഥിതം ശൂന്യം u_undo_end(പൂർണ്ണസംഖ്യ did_undo, പൂർണ്ണസംഖ്യ absolute);
സ്ഥിതം ശൂന്യം u_freeheader(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
സ്ഥിതം ശൂന്യം u_freebranch(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
സ്ഥിതം ശൂന്യം u_freeentries(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
സ്ഥിതം ശൂന്യം u_freeentry(u_entry_T *, long);
#ifdef FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
സ്ഥിതം പൂർണ്ണസംഖ്യ undo_flush(bufinfo_T *bi);
# endif
സ്ഥിതം പൂർണ്ണസംഖ്യ undo_read(bufinfo_T *bi, char_u *buffer, size_t size);
സ്ഥിതം പൂർണ്ണസംഖ്യ serialize_uep(bufinfo_T *bi, u_entry_T *uep);
സ്ഥിതം u_entry_T *unserialize_uep(bufinfo_T *bi, പൂർണ്ണസംഖ്യ *error, char_u *file_name);
സ്ഥിതം ശൂന്യം serialize_pos(bufinfo_T *bi, pos_T pos);
സ്ഥിതം ശൂന്യം unserialize_pos(bufinfo_T *bi, pos_T *pos);
സ്ഥിതം ശൂന്യം serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
സ്ഥിതം ശൂന്യം unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#endif
സ്ഥിതം ശൂന്യം u_saveline(linenr_T lnum);
സ്ഥിതം ശൂന്യം u_blockfree(buf_T *buf);

#define U_ALLOC_വരി(size) lalloc(size, FALSE)

// used in undo_end() to report number of added and deleted lines
സ്ഥിതം long	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ	undo_undoes = FALSE;

സ്ഥിതം പൂർണ്ണസംഖ്യ	lastmark = 0;

#എങ്കിൽ defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ seen_b_u_curhead;
സ്ഥിതം പൂർണ്ണസംഖ്യ seen_b_u_newhead;
സ്ഥിതം പൂർണ്ണസംഖ്യ header_എണ്ണം;

    സ്ഥിതം ശൂന്യം
u_check_വൃക്ഷം(u_header_T *uhp,
	u_header_T *exp_uh_next,
	u_header_T *exp_uh_alt_prev)
{
    u_entry_T *uep;

    എങ്കിൽ (uhp == ശൂന്യ)
	തിരികെ;
    ++header_എണ്ണം;
    എങ്കിൽ (uhp == curbuf->b_u_curhead && ++seen_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	തിരികെ;
    }
    എങ്കിൽ (uhp == curbuf->b_u_newhead && ++seen_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	തിരികെ;
    }

    എങ്കിൽ (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    അല്ലെങ്കിൽ
    {
	// Check pointers back are correct.
	എങ്കിൽ (uhp->uh_next.ptr != exp_uh_next)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_next, uhp->uh_next.ptr);
	}
	എങ്കിൽ (uhp->uh_alt_prev.ptr != exp_uh_alt_prev)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_prev, uhp->uh_alt_prev.ptr);
	}

	// Check the undo tree at this header.
	വേണ്ടി (uep = uhp->uh_entry; uep != ശൂന്യ; uep = uep->ue_next)
	{
	    എങ്കിൽ (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		നിർത്തുക;
	    }
	}

	// Check the next alt tree.
	u_check_വൃക്ഷം(uhp->uh_alt_next.ptr, uhp->uh_next.ptr, uhp);

	// Check the next header in this branch.
	u_check_വൃക്ഷം(uhp->uh_prev.ptr, uhp, ശൂന്യ);
    }
}

    സ്ഥിതം ശൂന്യം
u_check(പൂർണ്ണസംഖ്യ newhead_may_be_NULL)
{
    seen_b_u_newhead = 0;
    seen_b_u_curhead = 0;
    header_എണ്ണം = 0;

    u_check_വൃക്ഷം(curbuf->b_u_oldhead, ശൂന്യ, ശൂന്യ);

    എങ്കിൽ (seen_b_u_newhead == 0 && curbuf->b_u_oldhead != ശൂന്യ
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == ശൂന്യ))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    എങ്കിൽ (curbuf->b_u_curhead != ശൂന്യ && seen_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    എങ്കിൽ (header_എണ്ണം != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (long)header_എണ്ണം, (long)curbuf->b_u_numhead);
    }
}
#endif

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
u_save_cursor(ശൂന്യം)
{
    തിരികെ (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
u_save(linenr_T മുകൾ, linenr_T bot)
{
    എങ്കിൽ (undo_off)
	തിരികെ OK;

    എങ്കിൽ (മുകൾ >= bot || bot > curbuf->b_ml.ml_വരി_എണ്ണം + 1)
	തിരികെ FAIL;	// rely on caller to give an error message

    എങ്കിൽ (മുകൾ + 2 == bot)
	u_saveline((linenr_T)(മുകൾ + 1));

    തിരികെ (u_savecommon(മുകൾ, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
u_savesub(linenr_T lnum)
{
    എങ്കിൽ (undo_off)
	തിരികെ OK;

    തിരികെ (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
u_inssub(linenr_T lnum)
{
    എങ്കിൽ (undo_off)
	തിരികെ OK;

    തിരികെ (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
u_savedel(linenr_T lnum, long nlines)
{
    എങ്കിൽ (undo_off)
	തിരികെ OK;

    തിരികെ (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_വരി_എണ്ണം ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    പൂർണ്ണസംഖ്യ
undo_allowed(ശൂന്യം)
{
    // Don't allow changes when 'modifiable' is off.
    എങ്കിൽ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	തിരികെ FALSE;
    }

#ifdef HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    എങ്കിൽ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	തിരികെ FALSE;
    }
#endif

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    എങ്കിൽ (textlock != 0)
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	തിരികെ FALSE;
    }

    തിരികെ TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    സ്ഥിതം long
get_undolevel(ശൂന്യം)
{
    എങ്കിൽ (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	തിരികെ p_ul;
    തിരികെ curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
u_save_വരി(undoline_T *ul, linenr_T lnum)
{
    char_u *വരി = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    എങ്കിൽ (curbuf->b_ml.ml_വരി_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_വരി = vim_strsave((char_u *)"");
    }
    അല്ലെങ്കിൽ
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_വരി_len;
	ul->ul_വരി = vim_memsave(വരി, ul->ul_len);
    }
    തിരികെ ul->ul_വരി == ശൂന്യ ? FAIL : OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
has_prop_w_flags(linenr_T lnum, പൂർണ്ണസംഖ്യ flags)
{
    char_u  *props;
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    proplen = get_text_props(curbuf, lnum, &props, FALSE);

    വേണ്ടി (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * വലുപ്പം prop, വലുപ്പം prop);
	എങ്കിൽ (prop.tp_flags & flags)
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
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
    പൂർണ്ണസംഖ്യ
u_savecommon(
    linenr_T	മുകൾ,
    linenr_T	bot,
    linenr_T	newbot,
    പൂർണ്ണസംഖ്യ		reload)
{
    linenr_T	lnum;
    long	i;
    u_header_T	*uhp;
    u_header_T	*old_curhead;
    u_entry_T	*uep;
    u_entry_T	*prev_uep;
    long	size;

    എങ്കിൽ (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	എങ്കിൽ (!undo_allowed())
	    തിരികെ FAIL;

#ifdef FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	എങ്കിൽ (netbeans_active())
	{
	    എങ്കിൽ (netbeans_is_guarded(മുകൾ, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_in_read_only_files));
		തിരികെ FAIL;
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
	എങ്കിൽ (bot > curbuf->b_ml.ml_വരി_എണ്ണം + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_വരി_എണ്ണം_changed_unexpectedly));
	    തിരികെ FAIL;
	}
    }

#ifdef U_DEBUG
    u_check(FALSE);
#endif

#ifdef FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    എങ്കിൽ (bot - മുകൾ > 1)
    {
	എങ്കിൽ (മുകൾ > 0 && has_prop_w_flags(മുകൾ + 1, TP_FLAG_CONT_PREV))
	    --മുകൾ;
	എങ്കിൽ (bot <= curbuf->b_ml.ml_വരി_എണ്ണം
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_NEXT))
	{
	    ++bot;
	    എങ്കിൽ (newbot != 0)
		++newbot;
	}
    }
#endif

    size = bot - മുകൾ - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    എങ്കിൽ (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	എങ്കിൽ (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_വരി(വലുപ്പം(u_header_T));
	    എങ്കിൽ (uhp == ശൂന്യ)
		പോകുക nomem;
#ifdef U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#endif
	}
	അല്ലെങ്കിൽ
	    uhp = ശൂന്യ;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	എങ്കിൽ (old_curhead != ശൂന്യ)
	{
	    curbuf->b_u_newhead = old_curhead->uh_next.ptr;
	    curbuf->b_u_curhead = ശൂന്യ;
	}

	/*
	 * free headers to keep the size right
	 */
	ആകുമ്പോൾ (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != ശൂന്യ)
	{
	    u_header_T	    *uhfree = curbuf->b_u_oldhead;

	    എങ്കിൽ (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    അല്ലെങ്കിൽ എങ്കിൽ (uhfree->uh_alt_next.ptr == ശൂന്യ)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    അല്ലെങ്കിൽ
	    {
		// Free the oldest alternate branch as a whole.
		ആകുമ്പോൾ (uhfree->uh_alt_next.ptr != ശൂന്യ)
		    uhfree = uhfree->uh_alt_next.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#ifdef U_DEBUG
	    u_check(TRUE);
#endif
	}

	എങ്കിൽ (uhp == ശൂന്യ)		// no undo at all
	{
	    എങ്കിൽ (old_curhead != ശൂന്യ)
		u_freebranch(curbuf, old_curhead, ശൂന്യ);
	    curbuf->b_u_synced = false;
	    തിരികെ OK;
	}

	uhp->uh_prev.ptr = ശൂന്യ;
	uhp->uh_next.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_next.ptr = old_curhead;
	എങ്കിൽ (old_curhead != ശൂന്യ)
	{
	    uhp->uh_alt_prev.ptr = old_curhead->uh_alt_prev.ptr;
	    എങ്കിൽ (uhp->uh_alt_prev.ptr != ശൂന്യ)
		uhp->uh_alt_prev.ptr->uh_alt_next.ptr = uhp;
	    old_curhead->uh_alt_prev.ptr = uhp;
	    എങ്കിൽ (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	അല്ലെങ്കിൽ
	    uhp->uh_alt_prev.ptr = ശൂന്യ;
	എങ്കിൽ (curbuf->b_u_newhead != ശൂന്യ)
	    curbuf->b_u_newhead->uh_prev.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_last;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_time = vim_time();
	uhp->uh_save_nr = 0;
	curbuf->b_u_time_cur = uhp->uh_time + 1;

	uhp->uh_walk = 0;
	uhp->uh_entry = ശൂന്യ;
	uhp->uh_getbot_entry = ശൂന്യ;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	എങ്കിൽ (virtual_active() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	അല്ലെങ്കിൽ
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_changed ? UH_CHANGED : 0) +
		       ((curbuf->b_ml.ml_flags & ML_EMPTY) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, വലുപ്പം(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	എങ്കിൽ (curbuf->b_u_oldhead == ശൂന്യ)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (get_undolevel() < 0)	// no undo at all
	    തിരികെ OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	എങ്കിൽ (size == 1)
	{
	    uep = u_get_headentry();
	    prev_uep = ശൂന്യ;
	    വേണ്ടി (i = 0; i < 10; ++i)
	    {
		എങ്കിൽ (uep == ശൂന്യ)
		    നിർത്തുക;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		എങ്കിൽ ((curbuf->b_u_newhead->uh_getbot_entry != uep
			    ? (uep->ue_മുകൾ + uep->ue_size + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_വരി_എണ്ണം + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_വരി_എണ്ണം)
			|| (uep->ue_size > 1
			    && മുകൾ >= uep->ue_മുകൾ
			    && മുകൾ + 2 <= uep->ue_മുകൾ + uep->ue_size + 1))
		    നിർത്തുക;

		// If it's the same line we can skip saving it again.
		എങ്കിൽ (uep->ue_size == 1 && uep->ue_മുകൾ == മുകൾ)
		{
		    എങ്കിൽ (i > 0)
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
		    എങ്കിൽ (newbot != 0)
			uep->ue_bot = newbot;
		    അല്ലെങ്കിൽ എങ്കിൽ (bot > curbuf->b_ml.ml_വരി_എണ്ണം)
			uep->ue_bot = 0;
		    അല്ലെങ്കിൽ
		    {
			uep->ue_lcount = curbuf->b_ml.ml_വരി_എണ്ണം;
			curbuf->b_u_newhead->uh_getbot_entry = uep;
		    }
		    തിരികെ OK;
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
    uep = U_ALLOC_വരി(വലുപ്പം(u_entry_T));
    എങ്കിൽ (uep == ശൂന്യ)
	പോകുക nomem;
    CLEAR_POINTER(uep);
#ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
#endif

    uep->ue_size = size;
    uep->ue_മുകൾ = മുകൾ;
    എങ്കിൽ (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    അല്ലെങ്കിൽ എങ്കിൽ (bot > curbuf->b_ml.ml_വരി_എണ്ണം)
	uep->ue_bot = 0;
    അല്ലെങ്കിൽ
    {
	uep->ue_lcount = curbuf->b_ml.ml_വരി_എണ്ണം;
	curbuf->b_u_newhead->uh_getbot_entry = uep;
    }

    എങ്കിൽ (size > 0)
    {
	എങ്കിൽ ((uep->ue_array = U_ALLOC_വരി(വലുപ്പം(undoline_T) * size)) == ശൂന്യ)
	{
	    u_freeentry(uep, 0L);
	    പോകുക nomem;
	}
	വേണ്ടി (i = 0, lnum = മുകൾ + 1; i < size; ++i)
	{
	    fast_breakcheck();
	    എങ്കിൽ (got_int)
	    {
		u_freeentry(uep, i);
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (u_save_വരി(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		പോകുക nomem;
	    }
	}
    }
    അല്ലെങ്കിൽ
	uep->ue_array = ശൂന്യ;
    uep->ue_next = curbuf->b_u_newhead->uh_entry;
    curbuf->b_u_newhead->uh_entry = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#ifdef U_DEBUG
    u_check(FALSE);
#endif
    തിരികെ OK;

nomem:
    സന്ദേശം_silent = 0;	// must display the prompt
    എങ്കിൽ (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	തിരികെ OK;
    }
    do_outofmem_സന്ദേശം((long_u)0);
    തിരികെ FAIL;
}

#എങ്കിൽ defined(FEAT_PERSISTENT_UNDO)

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
    ശൂന്യം
u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;

    sha256_start(&ctx);
    വേണ്ടി (lnum = 1; lnum <= curbuf->b_ml.ml_വരി_എണ്ണം; ++lnum)
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
    സ്ഥിതം char_u *
u_get_undo_file_name(char_u *buf_ffname, പൂർണ്ണസംഖ്യ reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {ശൂന്യ, 0};
    char_u	*undo_file_name = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		dir_len;
    char_u	*p;
    size_t	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    size_t	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    എങ്കിൽ (ffname == ശൂന്യ)
	തിരികെ ശൂന്യ;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    എങ്കിൽ (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    ആകുമ്പോൾ (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	എങ്കിൽ (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    എങ്കിൽ (undo_file_name == ശൂന്യ)
		നിർത്തുക;
	    p = gettail(undo_file_name);
	    plen = (size_t)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# അല്ലെങ്കിൽ
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	അല്ലെങ്കിൽ
	{
	    dir_name[dir_len] = NUL;
	    എങ്കിൽ (mch_isdir(dir_name))
	    {
		string_T    ret;

		എങ്കിൽ (munged_name.string == ശൂന്യ)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    എങ്കിൽ (munged_name.string == ശൂന്യ)
			തിരികെ ശൂന്യ;
		    munged_name.length = ffnamelen;
		    വേണ്ടി (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			എങ്കിൽ (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (size_t)dir_len,
		    munged_name.string, munged_name.length, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	എങ്കിൽ (undo_file_name != ശൂന്യ && (!reading
			       || mch_stat((അക്ഷരം *)undo_file_name, &st) >= 0))
	    നിർത്തുക;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    തിരികെ undo_file_name;
}

    സ്ഥിതം ശൂന്യം
corruption_error(അക്ഷരം *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    സ്ഥിതം ശൂന്യം
u_free_uhp(u_header_T *uhp)
{
    u_entry_T	*nuep;
    u_entry_T	*uep;

    uep = uhp->uh_entry;
    ആകുമ്പോൾ (uep != ശൂന്യ)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    എങ്കിൽ (bi->bi_buffer != ശൂന്യ)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	ആകുമ്പോൾ (bi->bi_used + len_todo >= CRYPT_BUF_SIZE)
	{
	    size_t	n = CRYPT_BUF_SIZE - bi->bi_used;

	    mch_memmove(bi->bi_buffer + bi->bi_used, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_used = CRYPT_BUF_SIZE;
	    എങ്കിൽ (undo_flush(bi) == FAIL)
		തിരികെ FAIL;
	}
	എങ്കിൽ (len_todo > 0)
	{
	    mch_memmove(bi->bi_buffer + bi->bi_used, p, len_todo);
	    bi->bi_used += len_todo;
	}
	തിരികെ OK;
    }
# endif
    എങ്കിൽ (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	തിരികെ FAIL;
    തിരികെ OK;
}

# ifdef FEAT_CRYPT
    സ്ഥിതം പൂർണ്ണസംഖ്യ
undo_flush(bufinfo_T *bi)
{
    എങ്കിൽ (bi->bi_buffer != ശൂന്യ && bi->bi_state != ശൂന്യ && bi->bi_used > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_used, FALSE);
	എങ്കിൽ (fwrite(bi->bi_buffer, bi->bi_used, (size_t)1, bi->bi_fp) != 1)
	    തിരികെ FAIL;
	bi->bi_used = 0;
    }
    തിരികെ OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    എങ്കിൽ (bi->bi_state != ശൂന്യ && bi->bi_buffer == ശൂന്യ)
    {
	// crypting every piece of text separately
	എങ്കിൽ (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	അല്ലെങ്കിൽ
	{
	    copy = lalloc(len, FALSE);
	    എങ്കിൽ (copy == ശൂന്യ)
		തിരികെ 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	എങ്കിൽ (copy != small_buf)
	    vim_free(copy);
	തിരികെ i == 1 ? OK : FAIL;
    }
# endif
    തിരികെ undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
undo_write_bytes(bufinfo_T *bi, long_u nr, പൂർണ്ണസംഖ്യ len)
{
    char_u  buf[8];
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    bufi = 0;

    വേണ്ടി (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    തിരികെ undo_write(bi, buf, (size_t)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    സ്ഥിതം ശൂന്യം
put_header_ptr(bufinfo_T *bi, u_header_T *uhp)
{
    undo_write_bytes(bi, (long_u)(uhp != ശൂന്യ ? uhp->uh_seq : 0), 4);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    എങ്കിൽ (bi->bi_buffer != ശൂന്യ)
    {
	char_u  buf[4];
	പൂർണ്ണസംഖ്യ	n;

	undo_read(bi, buf, (size_t)4);
	n = ((unsigned)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	തിരികെ n;
    }
# endif
    തിരികെ get4c(bi->bi_fp);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    എങ്കിൽ (bi->bi_buffer != ശൂന്യ)
    {
	char_u  buf[2];
	പൂർണ്ണസംഖ്യ	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	തിരികെ n;
    }
# endif
    തിരികെ get2c(bi->bi_fp);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    എങ്കിൽ (bi->bi_buffer != ശൂന്യ)
    {
	char_u  buf[1];

	undo_read(bi, buf, (size_t)1);
	തിരികെ buf[0];
    }
# endif
    തിരികെ getc(bi->bi_fp);
}

    സ്ഥിതം time_t
undo_read_time(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    എങ്കിൽ (bi->bi_buffer != ശൂന്യ)
    {
	char_u  buf[8];
	time_t	n = 0;
	പൂർണ്ണസംഖ്യ	i;

	undo_read(bi, buf, (size_t)8);
	വേണ്ടി (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	തിരികെ n;
    }
# endif
    തിരികെ get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
undo_read(bufinfo_T *bi, char_u *buffer, size_t size)
{
    പൂർണ്ണസംഖ്യ retval = OK;

# ifdef FEAT_CRYPT
    എങ്കിൽ (bi->bi_buffer != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	size_todo = (പൂർണ്ണസംഖ്യ)size;
	char_u	*p = buffer;

	ആകുമ്പോൾ (size_todo > 0)
	{
	    size_t n;

	    എങ്കിൽ (bi->bi_used >= bi->bi_avail)
	    {
		n = fread(bi->bi_buffer, 1, (size_t)CRYPT_BUF_SIZE, bi->bi_fp);
		എങ്കിൽ (n == 0)
		{
		    retval = FAIL;
		    നിർത്തുക;
		}
		bi->bi_avail = n;
		bi->bi_used = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_avail, FALSE);
	    }
	    n = size_todo;
	    എങ്കിൽ (n > bi->bi_avail - bi->bi_used)
		n = bi->bi_avail - bi->bi_used;
	    mch_memmove(p, bi->bi_buffer + bi->bi_used, n);
	    bi->bi_used += n;
	    size_todo -= (പൂർണ്ണസംഖ്യ)n;
	    p += n;
	}
    }
    അല്ലെങ്കിൽ
# endif
    എങ്കിൽ (fread(buffer, size, 1, bi->bi_fp) != 1)
	retval = FAIL;

    എങ്കിൽ (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(buffer, 0, size);
    തിരികെ retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    സ്ഥിതം char_u *
read_string_decrypt(bufinfo_T *bi, പൂർണ്ണസംഖ്യ len)
{
    char_u  *ptr = alloc(len + 1);

    എങ്കിൽ (ptr == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	തിരികെ ശൂന്യ;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    എങ്കിൽ (bi->bi_state != ശൂന്യ && bi->bi_buffer == ശൂന്യ)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    തിരികെ ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
serialize_header(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	time_buf[8];

    // Start writing, first the magic marker and undo info version.
    എങ്കിൽ (fwrite(UF_START_MAGIC, (size_t)UF_START_MAGIC_LEN, (size_t)1, fp) != 1)
	തിരികെ FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    എങ്കിൽ (*buf->b_p_താക്കോൽ != NUL)
    {
	char_u *header;
	പൂർണ്ണസംഖ്യ    header_len;
	long	len;

	undo_write_bytes(bi, (long_u)UF_VERSION_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_method_nr(buf),
					  buf->b_p_താക്കോൽ, &header, &header_len);
	എങ്കിൽ (bi->bi_state == ശൂന്യ)
	    തിരികെ FAIL;
	len = (long)fwrite(header, (size_t)header_len, (size_t)1, fp);
	vim_free(header);
	എങ്കിൽ (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = ശൂന്യ;
	    തിരികെ FAIL;
	}

	എങ്കിൽ (crypt_whole_undofile(crypt_get_method_nr(buf)))
	{
	    bi->bi_buffer = alloc(CRYPT_BUF_SIZE);
	    എങ്കിൽ (bi->bi_buffer == ശൂന്യ)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = ശൂന്യ;
		തിരികെ FAIL;
	    }
	    bi->bi_used = 0;
	}
    }
    അല്ലെങ്കിൽ
# endif
	undo_write_bytes(bi, (long_u)UF_VERSION, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    എങ്കിൽ (undo_write(bi, hash, (size_t)UNDO_HASH_SIZE) == FAIL)
	തിരികെ FAIL;

    // buffer-specific data
    undo_write_bytes(bi, (long_u)buf->b_ml.ml_വരി_എണ്ണം, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_വരി_ptr.ul_textlen, 4);
    എങ്കിൽ (buf->b_u_വരി_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_വരി_ptr.ul_വരി,
			(size_t)buf->b_u_വരി_ptr.ul_textlen) == FAIL)
	തിരികെ FAIL;
    undo_write_bytes(bi, (long_u)buf->b_u_വരി_lnum, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_വരി_colnr, 4);

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

    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
serialize_uhp(bufinfo_T *bi, u_header_T *uhp)
{
    പൂർണ്ണസംഖ്യ		i;
    u_entry_T	*uep;
    char_u	time_buf[8];

    എങ്കിൽ (undo_write_bytes(bi, (long_u)UF_HEADER_MAGIC, 2) == FAIL)
	തിരികെ FAIL;

    put_header_ptr(bi, uhp->uh_next.ptr);
    put_header_ptr(bi, uhp->uh_prev.ptr);
    put_header_ptr(bi, uhp->uh_alt_next.ptr);
    put_header_ptr(bi, uhp->uh_alt_prev.ptr);
    undo_write_bytes(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_bytes(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_bytes(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    വേണ്ടി (i = 0; i < NMARKS; ++i)
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
    വേണ്ടി (uep = uhp->uh_entry; uep != ശൂന്യ; uep = uep->ue_next)
    {
	undo_write_bytes(bi, (long_u)UF_ENTRY_MAGIC, 2);
	എങ്കിൽ (serialize_uep(bi, uep) == FAIL)
	    തിരികെ FAIL;
    }
    undo_write_bytes(bi, (long_u)UF_ENTRY_END_MAGIC, 2);
    തിരികെ OK;
}

    സ്ഥിതം u_header_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_header_T	*uhp;
    പൂർണ്ണസംഖ്യ		i;
    u_entry_T	*uep, *last_uep;
    പൂർണ്ണസംഖ്യ		c;
    പൂർണ്ണസംഖ്യ		error;

    uhp = U_ALLOC_വരി(വലുപ്പം(u_header_T));
    എങ്കിൽ (uhp == ശൂന്യ)
	തിരികെ ശൂന്യ;
    CLEAR_POINTER(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_next.seq = undo_read_4c(bi);
    uhp->uh_prev.seq = undo_read_4c(bi);
    uhp->uh_alt_next.seq = undo_read_4c(bi);
    uhp->uh_alt_prev.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    എങ്കിൽ (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	തിരികെ ശൂന്യ;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    വേണ്ടി (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_time = undo_read_time(bi);

    // Optional fields.
    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ len = undo_read_byte(bi);
	പൂർണ്ണസംഖ്യ what;

	എങ്കിൽ (len == EOF)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    തിരികെ ശൂന്യ;
	}
	എങ്കിൽ (len == 0)
	    നിർത്തുക;
	what = undo_read_byte(bi);
	തിരഞ്ഞെടുപ്പ് (what)
	{
	    അവസ്ഥ UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		// field not supported, skip
		ആകുമ്പോൾ (--len >= 0)
		    (ശൂന്യം)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    last_uep = ശൂന്യ;
    ആകുമ്പോൾ ((c = undo_read_2c(bi)) == UF_ENTRY_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	എങ്കിൽ (last_uep == ശൂന്യ)
	    uhp->uh_entry = uep;
	അല്ലെങ്കിൽ
	    last_uep->ue_next = uep;
	last_uep = uep;
	എങ്കിൽ (uep == ശൂന്യ || error)
	{
	    u_free_uhp(uhp);
	    തിരികെ ശൂന്യ;
	}
    }
    എങ്കിൽ (c != UF_ENTRY_END_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	തിരികെ ശൂന്യ;
    }

    തിരികെ uhp;
}

/*
 * Serialize "uep".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
serialize_uep(
    bufinfo_T	*bi,
    u_entry_T	*uep)
{
    പൂർണ്ണസംഖ്യ		i;

    undo_write_bytes(bi, (long_u)uep->ue_മുകൾ, 4);
    undo_write_bytes(bi, (long_u)uep->ue_bot, 4);
    undo_write_bytes(bi, (long_u)uep->ue_lcount, 4);
    undo_write_bytes(bi, (long_u)uep->ue_size, 4);
    വേണ്ടി (i = 0; i < uep->ue_size; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	എങ്കിൽ (undo_write_bytes(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    തിരികെ FAIL;
	എങ്കിൽ (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_വരി, uep->ue_array[i].ul_textlen) == FAIL)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

    സ്ഥിതം u_entry_T *
unserialize_uep(bufinfo_T *bi, പൂർണ്ണസംഖ്യ *error, char_u *file_name)
{
    പൂർണ്ണസംഖ്യ		i;
    u_entry_T	*uep;
    undoline_T	*array = ശൂന്യ;
    char_u	*വരി;
    പൂർണ്ണസംഖ്യ		വരി_len;

    uep = U_ALLOC_വരി(വലുപ്പം(u_entry_T));
    എങ്കിൽ (uep == ശൂന്യ)
	തിരികെ ശൂന്യ;
    CLEAR_POINTER(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_മുകൾ = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_size = undo_read_4c(bi);
    എങ്കിൽ (uep->ue_size > 0)
    {
	എങ്കിൽ (uep->ue_size < LONG_MAX / (long)വലുപ്പം(undoline_T))
	    array = U_ALLOC_വരി(വലുപ്പം(undoline_T) * uep->ue_size);
	എങ്കിൽ (array == ശൂന്യ)
	{
	    *error = TRUE;
	    തിരികെ uep;
	}
	vim_memset(array, 0, വലുപ്പം(undoline_T) * uep->ue_size);
    }
    uep->ue_array = array;

    വേണ്ടി (i = 0; i < uep->ue_size; ++i)
    {
	വരി_len = undo_read_4c(bi);
	എങ്കിൽ (വരി_len >= 0)
	    വരി = read_string_decrypt(bi, വരി_len);
	അല്ലെങ്കിൽ
	{
	    വരി = ശൂന്യ;
	    corruption_error("line length", file_name);
	}
	എങ്കിൽ (വരി == ശൂന്യ)
	{
	    *error = TRUE;
	    തിരികെ uep;
	}
	array[i].ul_വരി = വരി;
	array[i].ul_len = വരി_len + 1;
	array[i].ul_textlen = വരി_len;
    }
    തിരികെ uep;
}

/*
 * Serialize "pos".
 */
    സ്ഥിതം ശൂന്യം
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_bytes(bi, (long_u)pos.lnum, 4);
    undo_write_bytes(bi, (long_u)pos.col, 4);
    undo_write_bytes(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    സ്ഥിതം ശൂന്യം
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    എങ്കിൽ (pos->lnum < 0)
	pos->lnum = 0;
    pos->col = undo_read_4c(bi);
    എങ്കിൽ (pos->col < 0)
	pos->col = 0;
    pos->coladd = undo_read_4c(bi);
    എങ്കിൽ (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
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
    ശൂന്യം
u_write_undo(
    char_u	*name,
    പൂർണ്ണസംഖ്യ		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_header_T	*uhp;
    char_u	*file_name;
    പൂർണ്ണസംഖ്യ		mark;
# ifdef U_DEBUG
    പൂർണ്ണസംഖ്യ		headers_written = 0;
# endif
    പൂർണ്ണസംഖ്യ		fd;
    FILE	*fp = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		perm;
    പൂർണ്ണസംഖ്യ		write_ok = FALSE;
# ifdef UNIX
    പൂർണ്ണസംഖ്യ		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);

    എങ്കിൽ (name == ശൂന്യ)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	എങ്കിൽ (file_name == ശൂന്യ)
	{
	    എങ്കിൽ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    എങ്കിൽ (buf->b_ffname != ശൂന്യ)
    {
# ifdef UNIX
	എങ്കിൽ (mch_stat((അക്ഷരം *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_mode;
	    st_old_valid = TRUE;
	}
# അല്ലെങ്കിൽ
	perm = mch_getperm(buf->b_ffname);
	എങ്കിൽ (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    എങ്കിൽ (mch_getperm(file_name) >= 0)
    {
	എങ്കിൽ (name == ശൂന്യ || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((അക്ഷരം *)file_name, O_RDONLY|O_EXTRA, 0);
	    എങ്കിൽ (fd < 0)
	    {
		എങ്കിൽ (name != ശൂന്യ || p_verbose > 0)
		{
		    എങ്കിൽ (name == ശൂന്യ)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    എങ്കിൽ (name == ശൂന്യ)
			verbose_leave();
		}
		പോകുക theend;
	    }
	    അല്ലെങ്കിൽ
	    {
		char_u	mbuf[UF_START_MAGIC_LEN];
		പൂർണ്ണസംഖ്യ	len;

		len = read_eintr(fd, mbuf, UF_START_MAGIC_LEN);
		close(fd);
		എങ്കിൽ (len < UF_START_MAGIC_LEN
		      || memcmp(mbuf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
		{
		    എങ്കിൽ (name != ശൂന്യ || p_verbose > 0)
		    {
			എങ്കിൽ (name == ശൂന്യ)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			എങ്കിൽ (name == ശൂന്യ)
			    verbose_leave();
		    }
		    പോകുക theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    എങ്കിൽ (buf->b_u_numhead == 0 && buf->b_u_വരി_ptr.ul_വരി == ശൂന്യ)
    {
	എങ്കിൽ (p_verbose > 0)
	    verb_സന്ദേശം(_("Skipping undo file write, nothing to undo"));
	പോകുക theend;
    }

    fd = mch_open((അക്ഷരം *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    എങ്കിൽ (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	പോകുക theend;
    }
    (ശൂന്യം)mch_setperm(file_name, perm);
    എങ്കിൽ (p_verbose > 0)
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
    എങ്കിൽ (st_old_valid
	    && mch_stat((അക്ഷരം *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  എങ്കിൽ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    എങ്കിൽ (buf->b_ffname != ശൂന്യ)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    എങ്കിൽ (fp == ശൂന്യ)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	close(fd);
	mch_remove(file_name);
	പോകുക theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    എങ്കിൽ (serialize_header(&bi, hash) == FAIL)
	പോകുക write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    ആകുമ്പോൾ (uhp != ശൂന്യ)
    {
	// Serialize current UHP if we haven't seen it
	എങ്കിൽ (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++headers_written;
# endif
	    എങ്കിൽ (serialize_uhp(&bi, uhp) == FAIL)
		പോകുക write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	എങ്കിൽ (uhp->uh_prev.ptr != ശൂന്യ && uhp->uh_prev.ptr->uh_walk != mark)
	    uhp = uhp->uh_prev.ptr;
	അല്ലെങ്കിൽ എങ്കിൽ (uhp->uh_alt_next.ptr != ശൂന്യ
				     && uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;
	അല്ലെങ്കിൽ എങ്കിൽ (uhp->uh_next.ptr != ശൂന്യ && uhp->uh_alt_prev.ptr == ശൂന്യ
					 && uhp->uh_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_next.ptr;
	അല്ലെങ്കിൽ എങ്കിൽ (uhp->uh_alt_prev.ptr != ശൂന്യ)
	    uhp = uhp->uh_alt_prev.ptr;
	അല്ലെങ്കിൽ
	    uhp = uhp->uh_next.ptr;
    }

    എങ്കിൽ (undo_write_bytes(&bi, (long_u)UF_HEADER_END_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    എങ്കിൽ (headers_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", headers_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    എങ്കിൽ (bi.bi_state != ശൂന്യ && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# എങ്കിൽ defined(UNIX) && defined(HAVE_FSYNC)
    എങ്കിൽ ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && fflush(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    fclose(fp);
    എങ്കിൽ (!write_ok)
	semsg(_(e_write_error_in_undo_file_str), file_name);

# എങ്കിൽ defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    എങ്കിൽ (buf->b_ffname != ശൂന്യ)
	(ശൂന്യം)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    എങ്കിൽ (buf->b_ffname != ശൂന്യ)
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
    എങ്കിൽ (bi.bi_state != ശൂന്യ)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
# endif
    എങ്കിൽ (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
uhp_seq_cmp(സ്ഥിരം ശൂന്യം *v1, സ്ഥിരം ശൂന്യം *v2)
{
    സ്ഥിരം u_header_T *u1 = *(u_header_T **)v1;
    സ്ഥിരം u_header_T *u2 = *(u_header_T **)v2;

    തിരികെ u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    സ്ഥിതം long
uhp_table_find(u_header_T **uhp_table, long അക്കം_head, long seq)
{
    long    lo = 0;
    long    hi = അക്കം_head - 1;

    ആകുമ്പോൾ (lo <= hi)
    {
	long mid = lo + (hi - lo) / 2;

	എങ്കിൽ (uhp_table[mid]->uh_seq < seq)
	    lo = mid + 1;
	അല്ലെങ്കിൽ എങ്കിൽ (uhp_table[mid]->uh_seq > seq)
	    hi = mid - 1;
	അല്ലെങ്കിൽ
	    തിരികെ mid;
    }
    തിരികെ -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    ശൂന്യം
u_read_undo(char_u *name, char_u *hash, char_u *orig_name UNUSED)
{
    char_u	*file_name;
    FILE	*fp;
    long	version, str_len;
    undoline_T	വരി_ptr;
    linenr_T	വരി_lnum;
    colnr_T	വരി_colnr;
    linenr_T	വരി_എണ്ണം;
    long	അക്കം_head = 0;
    long	old_header_seq, new_header_seq, cur_header_seq;
    long	seq_last, seq_cur;
    long	last_save_nr = 0;
    long	old_idx = -1, new_idx = -1, cur_idx = -1;
    long	അക്കം_read_uhps = 0;
    time_t	seq_time;
    long	i;
    പൂർണ്ണസംഖ്യ		c;
    u_header_T	*uhp;
    u_header_T	**uhp_table = ശൂന്യ;
    char_u	read_hash[UNDO_HASH_SIZE];
    char_u	magic_buf[UF_START_MAGIC_LEN];
# ifdef U_DEBUG
    പൂർണ്ണസംഖ്യ		*uhp_table_used;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);
    വരി_ptr.ul_len = 0;
    വരി_ptr.ul_textlen = 0;
    വരി_ptr.ul_വരി = ശൂന്യ;

    എങ്കിൽ (name == ശൂന്യ)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	എങ്കിൽ (file_name == ശൂന്യ)
	    തിരികെ;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	എങ്കിൽ (mch_stat((അക്ഷരം *)orig_name, &st_orig) >= 0
		&& mch_stat((അക്ഷരം *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    എങ്കിൽ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    തിരികെ;
	}
# endif
    }
    അല്ലെങ്കിൽ
	file_name = name;

    എങ്കിൽ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((അക്ഷരം *)file_name, "r");
    എങ്കിൽ (fp == ശൂന്യ)
    {
	എങ്കിൽ (name != ശൂന്യ || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	പോകുക error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    എങ്കിൽ (fread(magic_buf, UF_START_MAGIC_LEN, 1, fp) != 1
		|| memcmp(magic_buf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	പോകുക error;
    }
    version = get2c(fp);
    എങ്കിൽ (version == UF_VERSION_CRYPT)
    {
# ifdef FEAT_CRYPT
	എങ്കിൽ (*curbuf->b_p_താക്കോൽ == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    പോകുക error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_താക്കോൽ);
	എങ്കിൽ (bi.bi_state == ശൂന്യ)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    പോകുക error;
	}
	എങ്കിൽ (crypt_whole_undofile(bi.bi_state->method_nr))
	{
	    bi.bi_buffer = alloc(CRYPT_BUF_SIZE);
	    എങ്കിൽ (bi.bi_buffer == ശൂന്യ)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = ശൂന്യ;
		പോകുക error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_used = 0;
	}
# അല്ലെങ്കിൽ
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	പോകുക error;
# endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (version != UF_VERSION)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	പോകുക error;
    }

    എങ്കിൽ (undo_read(&bi, read_hash, (size_t)UNDO_HASH_SIZE) == FAIL)
    {
	corruption_error("hash", file_name);
	പോകുക error;
    }
    വരി_എണ്ണം = (linenr_T)undo_read_4c(&bi);
    എങ്കിൽ (memcmp(hash, read_hash, UNDO_HASH_SIZE) != 0
				  || വരി_എണ്ണം != curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	എങ്കിൽ (p_verbose > 0 || name != ശൂന്യ)
	{
	    എങ്കിൽ (name == ശൂന്യ)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    എങ്കിൽ (name == ശൂന്യ)
		verbose_leave();
	}
	പോകുക error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    എങ്കിൽ (str_len < 0)
	പോകുക error;
    എങ്കിൽ (str_len > 0)
    {
	വരി_ptr.ul_വരി = read_string_decrypt(&bi, str_len);
	വരി_ptr.ul_len = str_len + 1;
	വരി_ptr.ul_textlen = str_len;
    }
    വരി_lnum = (linenr_T)undo_read_4c(&bi);
    വരി_colnr = (colnr_T)undo_read_4c(&bi);
    എങ്കിൽ (വരി_lnum < 0 || വരി_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	പോകുക error;
    }

    // Begin general undo data
    old_header_seq = undo_read_4c(&bi);
    new_header_seq = undo_read_4c(&bi);
    cur_header_seq = undo_read_4c(&bi);
    അക്കം_head = undo_read_4c(&bi);
    seq_last = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_time = undo_read_time(&bi);

    // Optional header fields.
    വേണ്ടി (;;)
    {
	പൂർണ്ണസംഖ്യ len = undo_read_byte(&bi);
	പൂർണ്ണസംഖ്യ what;

	എങ്കിൽ (len == 0 || len == EOF)
	    നിർത്തുക;
	what = undo_read_byte(&bi);
	തിരഞ്ഞെടുപ്പ് (what)
	{
	    അവസ്ഥ UF_LAST_SAVE_NR:
		last_save_nr = undo_read_4c(&bi);
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		// field not supported, skip
		ആകുമ്പോൾ (--len >= 0)
		    (ശൂന്യം)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    എങ്കിൽ (അക്കം_head > 0)
    {
	എങ്കിൽ (അക്കം_head < LONG_MAX / (long)വലുപ്പം(u_header_T *))
	    uhp_table = U_ALLOC_വരി(അക്കം_head * വലുപ്പം(u_header_T *));
	എങ്കിൽ (uhp_table == ശൂന്യ)
	    പോകുക error;
    }

    ആകുമ്പോൾ ((c = undo_read_2c(&bi)) == UF_HEADER_MAGIC)
    {
	എങ്കിൽ (അക്കം_read_uhps >= അക്കം_head)
	{
	    corruption_error("num_head too small", file_name);
	    പോകുക error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	എങ്കിൽ (uhp == ശൂന്യ)
	    പോകുക error;
	uhp_table[അക്കം_read_uhps++] = uhp;
    }

    എങ്കിൽ (അക്കം_read_uhps != അക്കം_head)
    {
	corruption_error("num_head", file_name);
	പോകുക error;
    }
    എങ്കിൽ (c != UF_HEADER_END_MAGIC)
    {
	corruption_error("end marker", file_name);
	പോകുക error;
    }

# ifdef U_DEBUG
    uhp_table_used = alloc_clear(വലുപ്പം(പൂർണ്ണസംഖ്യ) * അക്കം_head + 1);
#  define SET_FLAG(j) ++uhp_table_used[j]
# അല്ലെങ്കിൽ
#  define SET_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    എങ്കിൽ (അക്കം_head > 0)
	qsort(uhp_table, (size_t)അക്കം_head, വലുപ്പം(u_header_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    വേണ്ടി (i = 0; i < അക്കം_head - 1; i++)
    {
	എങ്കിൽ (uhp_table[i]->uh_seq == uhp_table[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    പോകുക error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    ചെയ്യുക { \
	long fidx = uhp_table_find(uhp_table, അക്കം_head, (link).seq); \
	\
	എങ്കിൽ (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_table[fidx]; \
	    SET_FLAG(fidx); \
	} \
	അല്ലെങ്കിൽ \
	    (link).ptr = ശൂന്യ; \
    } ആകുമ്പോൾ (0)

    വേണ്ടി (i = 0; i < അക്കം_head; i++)
    {
	uhp = uhp_table[i];
	SWIZZLE_SEQ(uhp->uh_next, i);
	SWIZZLE_SEQ(uhp->uh_prev, i);
	SWIZZLE_SEQ(uhp->uh_alt_next, i);
	SWIZZLE_SEQ(uhp->uh_alt_prev, i);
	എങ്കിൽ (old_header_seq > 0 && old_idx < 0 && uhp->uh_seq == old_header_seq)
	{
	    old_idx = i;
	    SET_FLAG(i);
	}
	എങ്കിൽ (new_header_seq > 0 && new_idx < 0 && uhp->uh_seq == new_header_seq)
	{
	    new_idx = i;
	    SET_FLAG(i);
	}
	എങ്കിൽ (cur_header_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_header_seq)
	{
	    cur_idx = i;
	    SET_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? ശൂന്യ : uhp_table[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? ശൂന്യ : uhp_table[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? ശൂന്യ : uhp_table[cur_idx];
    curbuf->b_u_വരി_ptr = വരി_ptr;
    curbuf->b_u_വരി_lnum = വരി_lnum;
    curbuf->b_u_വരി_colnr = വരി_colnr;
    curbuf->b_u_numhead = അക്കം_head;
    curbuf->b_u_seq_last = seq_last;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_time_cur = seq_time;
    curbuf->b_u_save_nr_last = last_save_nr;
    curbuf->b_u_save_nr_cur = last_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_table);

# ifdef U_DEBUG
    വേണ്ടി (i = 0; i < അക്കം_head; ++i)
	എങ്കിൽ (uhp_table_used[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_table_used);
    u_check(TRUE);
# endif

    എങ്കിൽ (name != ശൂന്യ)
	smsg(_("Finished reading undo file %s"), file_name);
    പോകുക theend;

error:
    vim_free(വരി_ptr.ul_വരി);
    എങ്കിൽ (uhp_table != ശൂന്യ)
    {
	വേണ്ടി (i = 0; i < അക്കം_read_uhps; i++)
	    എങ്കിൽ (uhp_table[i] != ശൂന്യ)
		u_free_uhp(uhp_table[i]);
	vim_free(uhp_table);
    }

theend:
# ifdef FEAT_CRYPT
    എങ്കിൽ (bi.bi_state != ശൂന്യ)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
# endif
    എങ്കിൽ (fp != ശൂന്യ)
	fclose(fp);
    എങ്കിൽ (file_name != name)
	vim_free(file_name);
    തിരികെ;
}

#endif // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    ശൂന്യം
u_undo(പൂർണ്ണസംഖ്യ എണ്ണം)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    എങ്കിൽ (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	എണ്ണം = 1;
    }

    എങ്കിൽ (vim_strchr(p_cpo, CPO_UNDO) == ശൂന്യ)
	undo_undoes = TRUE;
    അല്ലെങ്കിൽ
	undo_undoes = !undo_undoes;
    u_doit(എണ്ണം);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    ശൂന്യം
u_redo(പൂർണ്ണസംഖ്യ എണ്ണം)
{
    എങ്കിൽ (vim_strchr(p_cpo, CPO_UNDO) == ശൂന്യ)
	undo_undoes = FALSE;
    u_doit(എണ്ണം);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    സ്ഥിതം ശൂന്യം
u_doit(പൂർണ്ണസംഖ്യ startcount)
{
    പൂർണ്ണസംഖ്യ എണ്ണം = startcount;

    എങ്കിൽ (!undo_allowed())
	തിരികെ;

    u_newcount = 0;
    u_oldcount = 0;
    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;
    ആകുമ്പോൾ (എണ്ണം--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	എങ്കിൽ (undo_undoes)
	{
	    എങ്കിൽ (curbuf->b_u_curhead == ശൂന്യ)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    അല്ലെങ്കിൽ എങ്കിൽ (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_next.ptr;
	    // nothing to undo
	    എങ്കിൽ (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == ശൂന്യ)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		എങ്കിൽ (എണ്ണം == startcount - 1)
		{
		    എങ്കിൽ (!shortmess(SHM_UNDO))
			സന്ദേശം(_("Already at oldest change"));
		    തിരികെ;
		}
		നിർത്തുക;
	    }

	    u_undoredo(TRUE);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (curbuf->b_u_curhead == ശൂന്യ || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		എങ്കിൽ (എണ്ണം == startcount - 1)
		{
		    എങ്കിൽ (!shortmess(SHM_UNDO))
			സന്ദേശം(_("Already at newest change"));
		    തിരികെ;
		}
		നിർത്തുക;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    എങ്കിൽ (curbuf->b_u_curhead->uh_prev.ptr == ശൂന്യ)
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
    ശൂന്യം
undo_time(
    long	step,
    പൂർണ്ണസംഖ്യ		sec,
    പൂർണ്ണസംഖ്യ		file,
    പൂർണ്ണസംഖ്യ		absolute)
{
    long	    target;
    long	    closest;
    long	    closest_start;
    long	    closest_seq = 0;
    long	    val;
    u_header_T	    *uhp = ശൂന്യ;
    u_header_T	    *last;
    പൂർണ്ണസംഖ്യ		    mark;
    പൂർണ്ണസംഖ്യ		    nomark = 0;  // shut up compiler
    പൂർണ്ണസംഖ്യ		    round;
    പൂർണ്ണസംഖ്യ		    dosec = sec;
    പൂർണ്ണസംഖ്യ		    dofile = file;
    പൂർണ്ണസംഖ്യ		    above = FALSE;
    പൂർണ്ണസംഖ്യ		    did_undo = TRUE;

    എങ്കിൽ (text_locked())
    {
	text_locked_സന്ദേശം();
	തിരികെ;
    }

    // First make sure the current undoable change is synced.
    എങ്കിൽ (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    എങ്കിൽ (absolute)
    {
	target = step;
	closest = -1;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (dosec)
	    target = (long)(curbuf->b_u_time_cur) + step;
	അല്ലെങ്കിൽ എങ്കിൽ (dofile)
	{
	    എങ്കിൽ (step < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		എങ്കിൽ (uhp != ശൂന്യ)
		    uhp = uhp->uh_next.ptr;
		അല്ലെങ്കിൽ
		    uhp = curbuf->b_u_newhead;
		എങ്കിൽ (uhp != ശൂന്യ && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    target = curbuf->b_u_save_nr_cur + step;
		അല്ലെങ്കിൽ
		    // count the changes since the last write as one step
		    target = curbuf->b_u_save_nr_cur + step + 1;
		എങ്കിൽ (target <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Moving forward to a newer write.
		target = curbuf->b_u_save_nr_cur + step;
		എങ്കിൽ (target > curbuf->b_u_save_nr_last)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    target = curbuf->b_u_seq_last + 1;
		    dofile = FALSE;
		}
	    }
	}
	അല്ലെങ്കിൽ
	    target = curbuf->b_u_seq_cur + step;
	എങ്കിൽ (step < 0)
	{
	    എങ്കിൽ (target < 0)
		target = 0;
	    closest = -1;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (dosec)
		closest = (long)(vim_time() + 1);
	    അല്ലെങ്കിൽ എങ്കിൽ (dofile)
		closest = curbuf->b_u_save_nr_last + 2;
	    അല്ലെങ്കിൽ
		closest = curbuf->b_u_seq_last + 2;
	    എങ്കിൽ (target >= closest)
		target = closest - 1;
	}
    }
    closest_start = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    എങ്കിൽ (target == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	പോകുക target_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    വേണ്ടി (round = 1; round <= 2; ++round)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	എങ്കിൽ (curbuf->b_u_curhead == ശൂന്യ)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	അല്ലെങ്കിൽ
	    uhp = curbuf->b_u_curhead;

	ആകുമ്പോൾ (uhp != ശൂന്യ)
	{
	    uhp->uh_walk = mark;
	    എങ്കിൽ (dosec)
		val = (long)(uhp->uh_time);
	    അല്ലെങ്കിൽ എങ്കിൽ (dofile)
		val = uhp->uh_save_nr;
	    അല്ലെങ്കിൽ
		val = uhp->uh_seq;

	    എങ്കിൽ (round == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		എങ്കിൽ ((step < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
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
	    എങ്കിൽ (target == val && !dosec)
	    {
		target = uhp->uh_seq;
		നിർത്തുക;
	    }

	    // go down in the tree if we haven't been there
	    എങ്കിൽ (uhp->uh_prev.ptr != ശൂന്യ && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
		uhp = uhp->uh_prev.ptr;

	    // go to alternate branch if we haven't been there
	    അല്ലെങ്കിൽ എങ്കിൽ (uhp->uh_alt_next.ptr != ശൂന്യ
		    && uhp->uh_alt_next.ptr->uh_walk != nomark
		    && uhp->uh_alt_next.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_next.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    അല്ലെങ്കിൽ എങ്കിൽ (uhp->uh_next.ptr != ശൂന്യ && uhp->uh_alt_prev.ptr == ശൂന്യ
		    && uhp->uh_next.ptr->uh_walk != nomark
		    && uhp->uh_next.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		എങ്കിൽ (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_next.ptr;
	    }

	    അല്ലെങ്കിൽ
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		എങ്കിൽ (uhp->uh_alt_prev.ptr != ശൂന്യ)
		    uhp = uhp->uh_alt_prev.ptr;
		അല്ലെങ്കിൽ
		    uhp = uhp->uh_next.ptr;
	    }
	}

	എങ്കിൽ (uhp != ശൂന്യ)    // found it
	    നിർത്തുക;

	എങ്കിൽ (absolute)
	{
	    semsg(_(e_undo_സംഖ്യ_nr_not_found), step);
	    തിരികെ;
	}

	എങ്കിൽ (closest == closest_start)
	{
	    എങ്കിൽ (!shortmess(SHM_UNDO))
	    {
		എങ്കിൽ (step < 0)
		    സന്ദേശം(_("Already at oldest change"));
		അല്ലെങ്കിൽ
		    സന്ദേശം(_("Already at newest change"));
	    }
	    തിരികെ;
	}

	target = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	എങ്കിൽ (step < 0)
	    above = TRUE;	// stop above the header
    }

target_zero:
    // If we found it: Follow the path to go to where we want to be.
    എങ്കിൽ (uhp != ശൂന്യ || target == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	ആകുമ്പോൾ (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    എങ്കിൽ (uhp == ശൂന്യ)
		uhp = curbuf->b_u_newhead;
	    അല്ലെങ്കിൽ
		uhp = uhp->uh_next.ptr;
	    എങ്കിൽ (uhp == ശൂന്യ || (target > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == target && !above))
		നിർത്തുക;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    എങ്കിൽ (target > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	എങ്കിൽ (target > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    ആകുമ്പോൾ (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		എങ്കിൽ (uhp == ശൂന്യ)
		    നിർത്തുക;

		// Go back to the first branch with a mark.
		ആകുമ്പോൾ (uhp->uh_alt_prev.ptr != ശൂന്യ
				     && uhp->uh_alt_prev.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_prev.ptr;

		// Find the last branch with a mark, that's the one.
		last = uhp;
		ആകുമ്പോൾ (last->uh_alt_next.ptr != ശൂന്യ
				    && last->uh_alt_next.ptr->uh_walk == mark)
		    last = last->uh_alt_next.ptr;
		എങ്കിൽ (last != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    ആകുമ്പോൾ (uhp->uh_alt_prev.ptr != ശൂന്യ)
			uhp = uhp->uh_alt_prev.ptr;
		    എങ്കിൽ (last->uh_alt_next.ptr != ശൂന്യ)
			last->uh_alt_next.ptr->uh_alt_prev.ptr =
							last->uh_alt_prev.ptr;
		    last->uh_alt_prev.ptr->uh_alt_next.ptr =
							last->uh_alt_next.ptr;
		    last->uh_alt_prev.ptr = ശൂന്യ;
		    last->uh_alt_next.ptr = uhp;
		    uhp->uh_alt_prev.ptr = last;

		    എങ്കിൽ (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = last;
		    uhp = last;
		    എങ്കിൽ (uhp->uh_next.ptr != ശൂന്യ)
			uhp->uh_next.ptr->uh_prev.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		എങ്കിൽ (uhp->uh_walk != mark)
		    നിർത്തുക;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		എങ്കിൽ (uhp->uh_seq == target && above)
		{
		    curbuf->b_u_seq_cur = target - 1;
		    നിർത്തുക;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		എങ്കിൽ (uhp->uh_prev.ptr == ശൂന്യ)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_prev.ptr;
		did_undo = FALSE;

		എങ്കിൽ (uhp->uh_seq == target)	// found it!
		    നിർത്തുക;

		uhp = uhp->uh_prev.ptr;
		എങ്കിൽ (uhp == ശൂന്യ || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    internal_error("undo_time()");
		    നിർത്തുക;
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
    സ്ഥിതം ശൂന്യം
u_undoredo(പൂർണ്ണസംഖ്യ undo)
{
    undoline_T	*newarray = ശൂന്യ;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	മുകൾ, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    long	i;
    u_entry_T	*uep, *nuep;
    u_entry_T	*newlist = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		old_flags;
    പൂർണ്ണസംഖ്യ		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    പൂർണ്ണസംഖ്യ		empty_buffer;		    // buffer became empty
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
    mch_memmove(namedm, curbuf->b_namedm, വലുപ്പം(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_start.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
    curbuf->b_op_start.col = 0;
    curbuf->b_op_end.lnum = 0;
    curbuf->b_op_end.col = 0;

    വേണ്ടി (uep = curhead->uh_entry; uep != ശൂന്യ; uep = nuep)
    {
	മുകൾ = uep->ue_മുകൾ;
	bot = uep->ue_bot;
	എങ്കിൽ (bot == 0)
	    bot = curbuf->b_ml.ml_വരി_എണ്ണം + 1;
	എങ്കിൽ (മുകൾ > curbuf->b_ml.ml_വരി_എണ്ണം || മുകൾ >= bot
				      || bot > curbuf->b_ml.ml_വരി_എണ്ണം + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_വരി_numbers_wrong);
	    changed();		// don't want UNCHANGED now
	    തിരികെ;
	}

	oldsize = bot - മുകൾ - 1;    // number of lines before undo
	newsize = uep->ue_size;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    എങ്കിൽ (lnum >= മുകൾ && lnum <= മുകൾ + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (മുകൾ < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		വേണ്ടി (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(മുകൾ + 1 + i);

		    എങ്കിൽ (curbuf->b_ml.ml_വരി_len != uep->ue_array[i].ul_len
			    || memcmp(uep->ue_array[i].ul_വരി, p,
						curbuf->b_ml.ml_വരി_len) != 0)
			നിർത്തുക;
		}
		എങ്കിൽ (i == newsize && newlnum == MAXLNUM && uep->ue_next == ശൂന്യ)
		{
		    newlnum = മുകൾ;
		    new_curpos.lnum = newlnum + 1;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (i < newsize)
		{
		    newlnum = മുകൾ + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	empty_buffer = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	എങ്കിൽ (oldsize > 0)
	{
	    എങ്കിൽ ((newarray = U_ALLOC_വരി(വലുപ്പം(undoline_T) * oldsize)) == ശൂന്യ)
	    {
		do_outofmem_സന്ദേശം((long_u)(വലുപ്പം(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		ആകുമ്പോൾ (uep != ശൂന്യ)
		{
		    nuep = uep->ue_next;
		    u_freeentry(uep, uep->ue_size);
		    uep = nuep;
		}
		നിർത്തുക;
	    }
	    // delete backwards, it goes faster in most cases
	    വേണ്ടി (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		എങ്കിൽ (u_save_വരി(&newarray[i], lnum) == FAIL)
		    do_outofmem_സന്ദേശം((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		എങ്കിൽ (curbuf->b_ml.ml_വരി_എണ്ണം == 1)
		    empty_buffer = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	അല്ലെങ്കിൽ
	    newarray = ശൂന്യ;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	എങ്കിൽ (newsize)
	{
	    വേണ്ടി (lnum = മുകൾ, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		എങ്കിൽ (empty_buffer && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_വരി,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		അല്ലെങ്കിൽ
		    ml_append_flags(lnum, uep->ue_array[i].ul_വരി,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_വരി);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#ifdef FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, മുകൾ, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, മുകൾ + newsize + 1, &uep->ue_props,
								UNDOPROP_BELOW);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, മുകൾ, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_BELOW);
	എങ്കിൽ (newsize > 0)
	    adjust_props_for_undo(curbuf, മുകൾ + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_BELOW);
#endif

	// adjust marks
	എങ്കിൽ (oldsize != newsize)
	{
	    mark_adjust(മുകൾ + 1, മുകൾ + oldsize, (long)MAXLNUM,
					       (long)newsize - (long)oldsize);
	    എങ്കിൽ (curbuf->b_op_start.lnum > മുകൾ + oldsize)
		curbuf->b_op_start.lnum += newsize - oldsize;
	    എങ്കിൽ (curbuf->b_op_end.lnum > മുകൾ + oldsize)
		curbuf->b_op_end.lnum += newsize - oldsize;
	}
	എങ്കിൽ (oldsize > 0 || newsize > 0)
	{
	    changed_വരികൾ(മുകൾ + 1, 0, bot, newsize - oldsize);
#ifdef FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    എങ്കിൽ (spell_check_window(curwin) && bot <= curbuf->b_ml.ml_വരി_എണ്ണം)
		redrawWinline(curwin, bot);
#endif
	}

	// Set the '[ mark.
	എങ്കിൽ (മുകൾ + 1 < curbuf->b_op_start.lnum)
	    curbuf->b_op_start.lnum = മുകൾ + 1;
	// Set the '] mark.
	എങ്കിൽ (newsize == 0 && മുകൾ + 1 > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = മുകൾ + 1;
	അല്ലെങ്കിൽ എങ്കിൽ (മുകൾ + newsize > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = മുകൾ + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_size = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = മുകൾ + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_next;
	uep->ue_next = newlist;
	newlist = uep;
    }

    // Ensure the '[ and '] marks are within bounds.
    എങ്കിൽ (curbuf->b_op_start.lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	 curbuf->b_op_start.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;
    എങ്കിൽ (curbuf->b_op_end.lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
	 curbuf->b_op_end.lnum = curbuf->b_ml.ml_വരി_എണ്ണം;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_entry = newlist;
    curhead->uh_flags = new_flags;
    എങ്കിൽ ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_EMPTY;
    എങ്കിൽ (old_flags & UH_CHANGED)
	changed();
    അല്ലെങ്കിൽ
#ifdef FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	എങ്കിൽ (!isNetbeansModified(curbuf))
#endif
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    വേണ്ടി (i = 0; i < NMARKS; ++i)
    {
	എങ്കിൽ (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	എങ്കിൽ (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	അല്ലെങ്കിൽ
	    curhead->uh_namedm[i].lnum = 0;
    }
    എങ്കിൽ (curhead->uh_visual.vi_start.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    എങ്കിൽ (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    എങ്കിൽ (curwin->w_cursor.lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	എങ്കിൽ (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = curhead->uh_cursor.col;
	    എങ്കിൽ (virtual_active() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    അല്ലെങ്കിൽ
		curwin->w_cursor.coladd = 0;
	}
	അല്ലെങ്കിൽ
	    beginline(BL_SOL | BL_FIX);
    }
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	എങ്കിൽ (curhead->uh_next.ptr != ശൂന്യ)
	    curbuf->b_u_seq_cur = curhead->uh_next.ptr->uh_seq;
	അല്ലെങ്കിൽ
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    എങ്കിൽ (curhead->uh_save_nr != 0)
    {
	എങ്കിൽ (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
u_undo_end(
    പൂർണ്ണസംഖ്യ		did_undo,	// just did an undo
    പൂർണ്ണസംഖ്യ		absolute)	// used ":undo N"
{
    അക്ഷരം	*msgstr;
    u_header_T	*uhp;
    char_u	msgbuf[80];

#ifdef FEAT_FOLDING
    എങ്കിൽ ((fdo_flags & FDO_UNDO) && താക്കോൽ_Typed)
	foldOpenCursor();
#endif

    എങ്കിൽ (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	തിരികെ;

    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)
	--u_newcount;

    u_oldcount -= u_newcount;
    എങ്കിൽ (u_oldcount == -1)
	msgstr = N_("more line");
    അല്ലെങ്കിൽ എങ്കിൽ (u_oldcount < 0)
	msgstr = N_("more lines");
    അല്ലെങ്കിൽ എങ്കിൽ (u_oldcount == 1)
	msgstr = N_("line less");
    അല്ലെങ്കിൽ എങ്കിൽ (u_oldcount > 1)
	msgstr = N_("fewer lines");
    അല്ലെങ്കിൽ
    {
	u_oldcount = u_newcount;
	എങ്കിൽ (u_newcount == 1)
	    msgstr = N_("change");
	അല്ലെങ്കിൽ
	    msgstr = N_("changes");
    }

    എങ്കിൽ (curbuf->b_u_curhead != ശൂന്യ)
    {
	// For ":undo N" we prefer a "after #N" message.
	എങ്കിൽ (absolute && curbuf->b_u_curhead->uh_next.ptr != ശൂന്യ)
	{
	    uhp = curbuf->b_u_curhead->uh_next.ptr;
	    did_undo = FALSE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (did_undo)
	    uhp = curbuf->b_u_curhead;
	അല്ലെങ്കിൽ
	    uhp = curbuf->b_u_curhead->uh_next.ptr;
    }
    അല്ലെങ്കിൽ
	uhp = curbuf->b_u_newhead;

    എങ്കിൽ (uhp == ശൂന്യ)
	*msgbuf = NUL;
    അല്ലെങ്കിൽ
	add_time(msgbuf, വലുപ്പം(msgbuf), uhp->uh_time);

#ifdef FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    എങ്കിൽ (wp->w_buffer == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#endif
    എങ്കിൽ (VIsual_active)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == ശൂന്യ ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    ശൂന്യം
u_sync(
    പൂർണ്ണസംഖ്യ	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    എങ്കിൽ (curbuf->b_u_synced || (!force && no_u_sync > 0))
	തിരികെ;
#എങ്കിൽ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    എങ്കിൽ (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	തിരികെ;		    // XIM is busy, don't break an undo sequence
#endif
    എങ്കിൽ (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    അല്ലെങ്കിൽ
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = ശൂന്യ;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    ശൂന്യം
ex_undolist(exarg_T *eap UNUSED)
{
    garray_T	ga;
    u_header_T	*uhp;
    പൂർണ്ണസംഖ്യ		mark;
    പൂർണ്ണസംഖ്യ		nomark;
    പൂർണ്ണസംഖ്യ		changes = 1;
    പൂർണ്ണസംഖ്യ		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, വലുപ്പം(അക്ഷരം *), 20);

    uhp = curbuf->b_u_oldhead;
    ആകുമ്പോൾ (uhp != ശൂന്യ)
    {
	എങ്കിൽ (uhp->uh_prev.ptr == ശൂന്യ && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    എങ്കിൽ (ga_grow(&ga, 1) == FAIL)
		നിർത്തുക;
	    len = vim_snprintf((അക്ഷരം *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    add_time(IObuff + len, IOSIZE - len, uhp->uh_time);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (പൂർണ്ണസംഖ്യ)STRLEN(IObuff + len);
	    എങ്കിൽ (uhp->uh_save_nr > 0)
	    {
		പൂർണ്ണസംഖ്യ n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((അക്ഷരം *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_data))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	എങ്കിൽ (uhp->uh_prev.ptr != ശൂന്യ && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_prev.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	അല്ലെങ്കിൽ എങ്കിൽ (uhp->uh_alt_next.ptr != ശൂന്യ
		&& uhp->uh_alt_next.ptr->uh_walk != nomark
		&& uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	അല്ലെങ്കിൽ എങ്കിൽ (uhp->uh_next.ptr != ശൂന്യ && uhp->uh_alt_prev.ptr == ശൂന്യ
		&& uhp->uh_next.ptr->uh_walk != nomark
		&& uhp->uh_next.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_next.ptr;
	    --changes;
	}

	അല്ലെങ്കിൽ
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    എങ്കിൽ (uhp->uh_alt_prev.ptr != ശൂന്യ)
		uhp = uhp->uh_alt_prev.ptr;
	    അല്ലെങ്കിൽ
	    {
		uhp = uhp->uh_next.ptr;
		--changes;
	    }
	}
    }

    എങ്കിൽ (ga.ga_len == 0)
	സന്ദേശം(_("Nothing to undo"));
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ		i;

	sort_strings((char_u **)ga.ga_data, ga.ga_len);

	സന്ദേശം_start();
	സന്ദേശം_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	വേണ്ടി (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    സന്ദേശം_putchar('\n');
	    എങ്കിൽ (got_int)
		നിർത്തുക;
	    സന്ദേശം_puts(((അക്ഷരം **)ga.ga_data)[i]);
	}
	സന്ദേശം_end();

	ga_clear_strings(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    ശൂന്യം
ex_undojoin(exarg_T *eap UNUSED)
{
    എങ്കിൽ (curbuf->b_u_newhead == ശൂന്യ)
	തിരികെ;		    // nothing changed before
    എങ്കിൽ (curbuf->b_u_curhead != ശൂന്യ)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	തിരികെ;
    }
    എങ്കിൽ (!curbuf->b_u_synced)
	തിരികെ;		    // already unsynced
    എങ്കിൽ (get_undolevel() < 0)
	തിരികെ;		    // no entries, nothing to do
    അല്ലെങ്കിൽ
	// Append next change to the last entry
	curbuf->b_u_synced = false;
}

/*
 * Called after writing or reloading the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
    ശൂന്യം
u_unchanged(buf_T *buf)
{
    u_unch_branch(buf->b_u_oldhead);
    buf->b_did_warn = false;
}

/*
 * After reloading a buffer which was saved for 'undoreload': Find the first
 * line that was changed and set the cursor there.
 */
    ശൂന്യം
u_find_first_changed(ശൂന്യം)
{
    u_header_T	*uhp = curbuf->b_u_newhead;
    u_entry_T   *uep;
    linenr_T	lnum;

    എങ്കിൽ (curbuf->b_u_curhead != ശൂന്യ || uhp == ശൂന്യ)
	തിരികെ;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_entry;
    എങ്കിൽ (uep->ue_മുകൾ != 0 || uep->ue_bot != 0)
	തിരികെ;

    വേണ്ടി (lnum = 1; lnum < curbuf->b_ml.ml_വരി_എണ്ണം
					      && lnum <= uep->ue_size; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	എങ്കിൽ (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_വരി_len
		|| memcmp(p, uep->ue_array[lnum - 1].ul_വരി, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    തിരികെ;
	}
    }
    എങ്കിൽ (curbuf->b_ml.ml_വരി_എണ്ണം != uep->ue_size)
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
    ശൂന്യം
u_update_save_nr(buf_T *buf)
{
    u_header_T	*uhp;

    ++buf->b_u_save_nr_last;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_last;
    uhp = buf->b_u_curhead;
    എങ്കിൽ (uhp != ശൂന്യ)
	uhp = uhp->uh_next.ptr;
    അല്ലെങ്കിൽ
	uhp = buf->b_u_newhead;
    എങ്കിൽ (uhp != ശൂന്യ)
	uhp->uh_save_nr = buf->b_u_save_nr_last;
}

    സ്ഥിതം ശൂന്യം
u_unch_branch(u_header_T *uhp)
{
    u_header_T	*uh;

    വേണ്ടി (uh = uhp; uh != ശൂന്യ; uh = uh->uh_prev.ptr)
    {
	uh->uh_flags |= UH_CHANGED;
	എങ്കിൽ (uh->uh_alt_next.ptr != ശൂന്യ)
	    u_unch_branch(uh->uh_alt_next.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    സ്ഥിതം u_entry_T *
u_get_headentry(ശൂന്യം)
{
    എങ്കിൽ (curbuf->b_u_newhead == ശൂന്യ || curbuf->b_u_newhead->uh_entry == ശൂന്യ)
    {
	iemsg(e_undo_list_corrupt);
	തിരികെ ശൂന്യ;
    }
    തിരികെ curbuf->b_u_newhead->uh_entry;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    സ്ഥിതം ശൂന്യം
u_getbot(ശൂന്യം)
{
    u_entry_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    എങ്കിൽ (uep == ശൂന്യ)
	തിരികെ;

    uep = curbuf->b_u_newhead->uh_getbot_entry;
    എങ്കിൽ (uep != ശൂന്യ)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_വരി_എണ്ണം - uep->ue_lcount;
	uep->ue_bot = uep->ue_മുകൾ + uep->ue_size + 1 + extra;
	എങ്കിൽ (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    iemsg(e_undo_വരി_missing);
	    uep->ue_bot = uep->ue_മുകൾ + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_entry = ശൂന്യ;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    സ്ഥിതം ശൂന്യം
u_freeheader(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    എങ്കിൽ (uhp->uh_alt_next.ptr != ശൂന്യ)
	u_freebranch(buf, uhp->uh_alt_next.ptr, uhpp);

    എങ്കിൽ (uhp->uh_alt_prev.ptr != ശൂന്യ)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = ശൂന്യ;

    // Update the links in the list to remove the header.
    എങ്കിൽ (uhp->uh_next.ptr == ശൂന്യ)
	buf->b_u_oldhead = uhp->uh_prev.ptr;
    അല്ലെങ്കിൽ
	uhp->uh_next.ptr->uh_prev.ptr = uhp->uh_prev.ptr;

    എങ്കിൽ (uhp->uh_prev.ptr == ശൂന്യ)
	buf->b_u_newhead = uhp->uh_next.ptr;
    അല്ലെങ്കിൽ
	വേണ്ടി (uhap = uhp->uh_prev.ptr; uhap != ശൂന്യ;
						 uhap = uhap->uh_alt_next.ptr)
	    uhap->uh_next.ptr = uhp->uh_next.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    സ്ഥിതം ശൂന്യം
u_freebranch(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *tofree, *next;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    എങ്കിൽ (uhp == buf->b_u_oldhead)
    {
	ആകുമ്പോൾ (buf->b_u_oldhead != ശൂന്യ)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	തിരികെ;
    }

    എങ്കിൽ (uhp->uh_alt_prev.ptr != ശൂന്യ)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = ശൂന്യ;

    next = uhp;
    ആകുമ്പോൾ (next != ശൂന്യ)
    {
	tofree = next;
	എങ്കിൽ (tofree->uh_alt_next.ptr != ശൂന്യ)
	    u_freebranch(buf, tofree->uh_alt_next.ptr, uhpp);   // recursive
	next = tofree->uh_prev.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}

/*
 * Free all the undo entries for one header and the header itself.
 * This means that "uhp" is invalid when returning.
 */
    സ്ഥിതം ശൂന്യം
u_freeentries(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_entry_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    എങ്കിൽ (buf->b_u_curhead == uhp)
	buf->b_u_curhead = ശൂന്യ;
    എങ്കിൽ (buf->b_u_newhead == uhp)
	buf->b_u_newhead = ശൂന്യ;  // freeing the newest entry
    എങ്കിൽ (uhpp != ശൂന്യ && uhp == *uhpp)
	*uhpp = ശൂന്യ;

    വേണ്ടി (uep = uhp->uh_entry; uep != ശൂന്യ; uep = nuep)
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
    സ്ഥിതം ശൂന്യം
u_freeentry(u_entry_T *uep, long n)
{
    ആകുമ്പോൾ (n > 0)
	vim_free(uep->ue_array[--n].ul_വരി);
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
    സ്ഥിതം ശൂന്യം
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = ശൂന്യ;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_വരി_ptr.ul_വരി = ശൂന്യ;
    buf->b_u_വരി_ptr.ul_len = 0;
    buf->b_u_വരി_ptr.ul_textlen = 0;
    buf->b_u_വരി_lnum = 0;
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 */
    സ്ഥിതം ശൂന്യം
u_blockfree(buf_T *buf)
{
    ആകുമ്പോൾ (buf->b_u_oldhead != ശൂന്യ)
	u_freeheader(buf, buf->b_u_oldhead, ശൂന്യ);
    vim_free(buf->b_u_വരി_ptr.ul_വരി);
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 * and invalidate the undo buffer
 */
    ശൂന്യം
u_clearallandblockfree(buf_T *buf)
{
    u_blockfree(buf);
    u_clearall(buf);
}



/*
 * Save the line "lnum" for the "U" command.
 */
    സ്ഥിതം ശൂന്യം
u_saveline(linenr_T lnum)
{
    എങ്കിൽ (lnum == curbuf->b_u_വരി_lnum)	    // line is already saved
	തിരികെ;
    എങ്കിൽ (lnum < 1 || lnum > curbuf->b_ml.ml_വരി_എണ്ണം) // should never happen
	തിരികെ;
    u_clearline();
    curbuf->b_u_വരി_lnum = lnum;
    എങ്കിൽ (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_വരി_colnr = curwin->w_cursor.col;
    അല്ലെങ്കിൽ
	curbuf->b_u_വരി_colnr = 0;
    എങ്കിൽ (u_save_വരി(&curbuf->b_u_വരി_ptr, lnum) == FAIL)
	do_outofmem_സന്ദേശം((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    ശൂന്യം
u_clearline(ശൂന്യം)
{
    എങ്കിൽ (curbuf->b_u_വരി_ptr.ul_വരി == ശൂന്യ)
	തിരികെ;

    VIM_CLEAR(curbuf->b_u_വരി_ptr.ul_വരി);
    curbuf->b_u_വരി_ptr.ul_len = 0;
    curbuf->b_u_വരി_ptr.ul_textlen = 0;
    curbuf->b_u_വരി_lnum = 0;
}

/*
 * Implementation of the "U" command.
 * Differentiation from vi: "U" can be undone with the next "U".
 * We also allow the cursor to be in another line.
 * Careful: may trigger autocommands that reload the buffer.
 */
    ശൂന്യം
u_undoline(ശൂന്യം)
{
    colnr_T	t;
    undoline_T  oldp;

    എങ്കിൽ (undo_off)
	തിരികെ;

    എങ്കിൽ (curbuf->b_u_വരി_ptr.ul_വരി == ശൂന്യ
			|| curbuf->b_u_വരി_lnum > curbuf->b_ml.ml_വരി_എണ്ണം)
    {
	beep_flush();
	തിരികെ;
    }

    // first save the line for the 'u' command
    എങ്കിൽ (u_savecommon(curbuf->b_u_വരി_lnum - 1,
		       curbuf->b_u_വരി_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	തിരികെ;
    എങ്കിൽ (u_save_വരി(&oldp, curbuf->b_u_വരി_lnum) == FAIL)
    {
	do_outofmem_സന്ദേശം((long_u)0);
	തിരികെ;
    }
    ml_replace_len(curbuf->b_u_വരി_lnum, curbuf->b_u_വരി_ptr.ul_വരി,
				     curbuf->b_u_വരി_ptr.ul_len, TRUE, FALSE);
    changed_bytes(curbuf->b_u_വരി_lnum, 0);
    curbuf->b_u_വരി_ptr = oldp;

    t = curbuf->b_u_വരി_colnr;
    എങ്കിൽ (curwin->w_cursor.lnum == curbuf->b_u_വരി_lnum)
	curbuf->b_u_വരി_colnr = curwin->w_cursor.col;
    curwin->w_cursor.col = t;
    curwin->w_cursor.lnum = curbuf->b_u_വരി_lnum;
    check_cursor_col();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    പൂർണ്ണസംഖ്യ
bufIsChanged(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    എങ്കിൽ (term_job_running_not_none(buf->b_term))
	തിരികെ TRUE;
#endif
    തിരികെ bufIsChangedNotTerm(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    പൂർണ്ണസംഖ്യ
anyBufIsChanged(ശൂന്യം)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	എങ്കിൽ (bufIsChanged(buf))
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    പൂർണ്ണസംഖ്യ
bufIsChangedNotTerm(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    തിരികെ (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_changed || file_ff_differs(buf, TRUE));
}

    പൂർണ്ണസംഖ്യ
curbufIsChanged(ശൂന്യം)
{
    തിരികെ bufIsChanged(curbuf);
}

#എങ്കിൽ defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    സ്ഥിതം ശൂന്യം
u_eval_വൃക്ഷം(buf_T *buf, u_header_T *first_uhp, list_T *list)
{
    u_header_T  *uhp = first_uhp;
    dict_T	*dict;

    ആകുമ്പോൾ (uhp != ശൂന്യ)
    {
	dict = dict_alloc();
	എങ്കിൽ (dict == ശൂന്യ)
	    തിരികെ;
	dict_add_സംഖ്യ(dict, "seq", uhp->uh_seq);
	dict_add_സംഖ്യ(dict, "time", (long)uhp->uh_time);
	എങ്കിൽ (uhp == buf->b_u_newhead)
	    dict_add_സംഖ്യ(dict, "newhead", 1);
	എങ്കിൽ (uhp == buf->b_u_curhead)
	    dict_add_സംഖ്യ(dict, "curhead", 1);
	എങ്കിൽ (uhp->uh_save_nr > 0)
	    dict_add_സംഖ്യ(dict, "save", uhp->uh_save_nr);

	എങ്കിൽ (uhp->uh_alt_next.ptr != ശൂന്യ)
	{
	    list_T	*alt_list = list_alloc();

	    എങ്കിൽ (alt_list != ശൂന്യ)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_വൃക്ഷം(buf, uhp->uh_alt_next.ptr, alt_list);
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
    ശൂന്യം
f_undofile(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	എങ്കിൽ (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    char_u *ffname = FullName_save(fname, TRUE);

	    എങ്കിൽ (ffname != ശൂന്യ)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# അല്ലെങ്കിൽ
    rettv->vval.v_string = ശൂന്യ;
# endif
}
# ifdef FEAT_PERSISTENT_UNDO
/*
 * Reset undofile option and delete the undofile
 */
    ശൂന്യം
u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    എങ്കിൽ (!buf->b_p_udf)
	തിരികെ;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    എങ്കിൽ (file_name != ശൂന്യ)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    set_option_മൂല്യം_give_err((char_u *)"undofile", 0L, ശൂന്യ, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    ശൂന്യം
f_undotree(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_സംഖ്യ(dict, "synced", (long)buf->b_u_synced);
    dict_add_സംഖ്യ(dict, "seq_last", buf->b_u_seq_last);
    dict_add_സംഖ്യ(dict, "save_last", buf->b_u_save_nr_last);
    dict_add_സംഖ്യ(dict, "seq_cur", buf->b_u_seq_cur);
    dict_add_സംഖ്യ(dict, "time_cur", (long)buf->b_u_time_cur);
    dict_add_സംഖ്യ(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    എങ്കിൽ (list != ശൂന്യ)
    {
	u_eval_വൃക്ഷം(buf, buf->b_u_oldhead, list);
	dict_add_list(dict, "entries", list);
    }
}

#endif
