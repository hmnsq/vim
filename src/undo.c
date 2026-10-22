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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UH_MAGIC 0x18dade	// value for uh_magic when in use
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UE_MAGIC 0xabc123	// value for ue_magic when in use

// Size of buffer used for encryption.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CRYPT_BUF_ਮਾਪ 8192

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    buf_T	*bi_buf;
    FILE	*bi_fp;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_ਬਫਰ; // CRYPT_BUF_SIZE, NULL when not buffering
    size_t	bi_ਵਰਤਿਆ;    // bytes written to/read from bi_buffer
    size_t	bi_avail;   // bytes available in bi_buffer
#ਜੇ_ਅੰਤ
} bufinfo_T;


ਸਥਿਰ ਖਾਲੀ u_unch_branch(u_ਸਿਰਲੇਖ_T *uhp);
ਸਥਿਰ u_ਇੰਦਰਾਜ_T *u_get_headentry(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ u_getbot(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ u_doit(ਪੂਰਨ ਗਿਣਤੀ_2);
ਸਥਿਰ ਖਾਲੀ u_undoredo(ਪੂਰਨ undo);
ਸਥਿਰ ਖਾਲੀ u_undo_ਸਿਰਾ(ਪੂਰਨ did_undo, ਪੂਰਨ absolute);
ਸਥਿਰ ਖਾਲੀ u_freeheader(buf_T *buf, u_ਸਿਰਲੇਖ_T *uhp, u_ਸਿਰਲੇਖ_T **uhpp);
ਸਥਿਰ ਖਾਲੀ u_freebranch(buf_T *buf, u_ਸਿਰਲੇਖ_T *uhp, u_ਸਿਰਲੇਖ_T **uhpp);
ਸਥਿਰ ਖਾਲੀ u_freeentries(buf_T *buf, u_ਸਿਰਲੇਖ_T *uhp, u_ਸਿਰਲੇਖ_T **uhpp);
ਸਥਿਰ ਖਾਲੀ u_freeentry(u_ਇੰਦਰਾਜ_T *, ਲੰਮਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
ਸਥਿਰ ਪੂਰਨ undo_flush(bufinfo_T *bi);
# endif
ਸਥਿਰ ਪੂਰਨ undo_read(bufinfo_T *bi, char_u *ਬਫਰ, size_t ਮਾਪ);
ਸਥਿਰ ਪੂਰਨ serialize_uep(bufinfo_T *bi, u_ਇੰਦਰਾਜ_T *uep);
ਸਥਿਰ u_ਇੰਦਰਾਜ_T *unserialize_uep(bufinfo_T *bi, ਪੂਰਨ *error, char_u *file_name);
ਸਥਿਰ ਖਾਲੀ serialize_pos(bufinfo_T *bi, pos_T pos);
ਸਥਿਰ ਖਾਲੀ unserialize_pos(bufinfo_T *bi, pos_T *pos);
ਸਥਿਰ ਖਾਲੀ serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
ਸਥਿਰ ਖਾਲੀ unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ u_saveline(linenr_T lnum);
ਸਥਿਰ ਖਾਲੀ u_blockfree(buf_T *buf);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_ALLOC_ਸਤਰ(ਮਾਪ) lalloc(ਮਾਪ, FALSE)

// used in undo_end() to report number of added and deleted lines
ਸਥਿਰ ਲੰਮਾ	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
ਸਥਿਰ ਪੂਰਨ	undo_undoes = FALSE;

ਸਥਿਰ ਪੂਰਨ	lastmark = 0;

#ਜੇ defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
ਸਥਿਰ ਪੂਰਨ ਵੇਖਿਆ_b_u_curhead;
ਸਥਿਰ ਪੂਰਨ ਵੇਖਿਆ_b_u_newhead;
ਸਥਿਰ ਪੂਰਨ ਸਿਰਲੇਖ_ਗਿਣਤੀ;

    ਸਥਿਰ ਖਾਲੀ
u_check_ਰੁੱਖ(u_ਸਿਰਲੇਖ_T *uhp,
	u_ਸਿਰਲੇਖ_T *exp_uh_ਅਗਲਾ,
	u_ਸਿਰਲੇਖ_T *exp_uh_alt_prev)
{
    u_ਇੰਦਰਾਜ_T *uep;

    ਜੇ (uhp == ਨਿਰਾ)
	ਵਾਪਸ;
    ++ਸਿਰਲੇਖ_ਗਿਣਤੀ;
    ਜੇ (uhp == curbuf->b_u_curhead && ++ਵੇਖਿਆ_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	ਵਾਪਸ;
    }
    ਜੇ (uhp == curbuf->b_u_newhead && ++ਵੇਖਿਆ_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	ਵਾਪਸ;
    }

    ਜੇ (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    ਨਹੀਂਤਾਂ
    {
	// Check pointers back are correct.
	ਜੇ (uhp->uh_ਅਗਲਾ.ptr != exp_uh_ਅਗਲਾ)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_ਅਗਲਾ, uhp->uh_ਅਗਲਾ.ptr);
	}
	ਜੇ (uhp->uh_alt_prev.ptr != exp_uh_alt_prev)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_prev, uhp->uh_alt_prev.ptr);
	}

	// Check the undo tree at this header.
	ਲਈ (uep = uhp->uh_ਇੰਦਰਾਜ; uep != ਨਿਰਾ; uep = uep->ue_ਅਗਲਾ)
	{
	    ਜੇ (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		ਰੋਕੋ;
	    }
	}

	// Check the next alt tree.
	u_check_ਰੁੱਖ(uhp->uh_alt_ਅਗਲਾ.ptr, uhp->uh_ਅਗਲਾ.ptr, uhp);

	// Check the next header in this branch.
	u_check_ਰੁੱਖ(uhp->uh_prev.ptr, uhp, ਨਿਰਾ);
    }
}

    ਸਥਿਰ ਖਾਲੀ
u_check(ਪੂਰਨ newhead_may_be_NULL)
{
    ਵੇਖਿਆ_b_u_newhead = 0;
    ਵੇਖਿਆ_b_u_curhead = 0;
    ਸਿਰਲੇਖ_ਗਿਣਤੀ = 0;

    u_check_ਰੁੱਖ(curbuf->b_u_oldhead, ਨਿਰਾ, ਨਿਰਾ);

    ਜੇ (ਵੇਖਿਆ_b_u_newhead == 0 && curbuf->b_u_oldhead != ਨਿਰਾ
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == ਨਿਰਾ))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    ਜੇ (curbuf->b_u_curhead != ਨਿਰਾ && ਵੇਖਿਆ_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    ਜੇ (ਸਿਰਲੇਖ_ਗਿਣਤੀ != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (ਲੰਮਾ)ਸਿਰਲੇਖ_ਗਿਣਤੀ, (ਲੰਮਾ)curbuf->b_u_numhead);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
u_save_cursor(ਖਾਲੀ)
{
    ਵਾਪਸ (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ਪੂਰਨ
u_save(linenr_T ਸਿਖਰ, linenr_T bot)
{
    ਜੇ (undo_off)
	ਵਾਪਸ OK;

    ਜੇ (ਸਿਖਰ >= bot || bot > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
	ਵਾਪਸ FAIL;	// rely on caller to give an error message

    ਜੇ (ਸਿਖਰ + 2 == bot)
	u_saveline((linenr_T)(ਸਿਖਰ + 1));

    ਵਾਪਸ (u_savecommon(ਸਿਖਰ, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ਪੂਰਨ
u_savesub(linenr_T lnum)
{
    ਜੇ (undo_off)
	ਵਾਪਸ OK;

    ਵਾਪਸ (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ਪੂਰਨ
u_inssub(linenr_T lnum)
{
    ਜੇ (undo_off)
	ਵਾਪਸ OK;

    ਵਾਪਸ (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ਪੂਰਨ
u_savedel(linenr_T lnum, ਲੰਮਾ nlines)
{
    ਜੇ (undo_off)
	ਵਾਪਸ OK;

    ਵਾਪਸ (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    ਪੂਰਨ
undo_allowed(ਖਾਲੀ)
{
    // Don't allow changes when 'modifiable' is off.
    ਜੇ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ਵਾਪਸ FALSE;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    ਜੇ (sandbox != 0)
    {
	emsg(_(e_not_allowed_ਵਿੱਚ_sandbox));
	ਵਾਪਸ FALSE;
    }
#ਜੇ_ਅੰਤ

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    ਜੇ (textlock != 0)
    {
	emsg(_(e_not_allowed_ਮੰਜ਼ਿਲ_change_ਪਾਠ_or_change_ਖਿੜਕੀ));
	ਵਾਪਸ FALSE;
    }

    ਵਾਪਸ TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    ਸਥਿਰ ਲੰਮਾ
get_undolevel(ਖਾਲੀ)
{
    ਜੇ (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	ਵਾਪਸ p_ul;
    ਵਾਪਸ curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    ਸਥਿਰ ਪੂਰਨ
u_save_ਸਤਰ(undoline_T *ul, linenr_T lnum)
{
    char_u *ਸਤਰ = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    ਜੇ (curbuf->b_ml.ml_ਸਤਰ_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_ਸਤਰ = vim_strsave((char_u *)"");
    }
    ਨਹੀਂਤਾਂ
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_ਸਤਰ_len;
	ul->ul_ਸਤਰ = vim_memsave(ਸਤਰ, ul->ul_len);
    }
    ਵਾਪਸ ul->ul_ਸਤਰ == ਨਿਰਾ ? FAIL : OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    ਸਥਿਰ ਪੂਰਨ
has_prop_w_flags(linenr_T lnum, ਪੂਰਨ flags)
{
    char_u  *props;
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    proplen = get_ਪਾਠ_props(curbuf, lnum, &props, FALSE);

    ਲਈ (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * ਆਕਾਰ prop, ਆਕਾਰ prop);
	ਜੇ (prop.tp_flags & flags)
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Common code for various ways to save text before a change.
 * "top" is the line above the first changed line.
 * "bot" is the line below the last changed line.
 * "newbot" is the new bottom line.  Use zero when not known.
 * "reload" is TRUE when saving for a buffer reload.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ਪੂਰਨ
u_savecommon(
    linenr_T	ਸਿਖਰ,
    linenr_T	bot,
    linenr_T	newbot,
    ਪੂਰਨ		reload)
{
    linenr_T	lnum;
    ਲੰਮਾ	i;
    u_ਸਿਰਲੇਖ_T	*uhp;
    u_ਸਿਰਲੇਖ_T	*old_curhead;
    u_ਇੰਦਰਾਜ_T	*uep;
    u_ਇੰਦਰਾਜ_T	*prev_uep;
    ਲੰਮਾ	ਮਾਪ;

    ਜੇ (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	ਜੇ (!undo_allowed())
	    ਵਾਪਸ FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	ਜੇ (netbeans_ਸਰਗਰਮ())
	{
	    ਜੇ (netbeans_is_guarded(ਸਿਖਰ, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_ਵਿੱਚ_read_only_files));
		ਵਾਪਸ FAIL;
	    }
	}
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	// A change in a terminal buffer removes the highlighting.
	ਪਦ_change_ਵਿੱਚ_curbuf();
#ਜੇ_ਅੰਤ

	/*
	 * Saving text for undo means we are going to make a change.  Give a
	 * warning for a read-only file before making the change, so that the
	 * FileChangedRO event can replace the buffer with a read-write version
	 * (e.g., obtained from a source control system).
	 */
	change_warning(0);
	ਜੇ (bot > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_ਸਤਰ_ਗਿਣਤੀ_ਬਦਲਿਆ_unexpectedly));
	    ਵਾਪਸ FAIL;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
    u_check(FALSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    ਜੇ (bot - ਸਿਖਰ > 1)
    {
	ਜੇ (ਸਿਖਰ > 0 && has_prop_w_flags(ਸਿਖਰ + 1, TP_FLAG_CONT_PREV))
	    --ਸਿਖਰ;
	ਜੇ (bot <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_ਅਗਲਾ))
	{
	    ++bot;
	    ਜੇ (newbot != 0)
		++newbot;
	}
    }
#ਜੇ_ਅੰਤ

    ਮਾਪ = bot - ਸਿਖਰ - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    ਜੇ (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	ਜੇ (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_ਸਤਰ(ਆਕਾਰ(u_ਸਿਰਲੇਖ_T));
	    ਜੇ (uhp == ਨਿਰਾ)
		ਜਾਓ nomem;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	    uhp = ਨਿਰਾ;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	ਜੇ (old_curhead != ਨਿਰਾ)
	{
	    curbuf->b_u_newhead = old_curhead->uh_ਅਗਲਾ.ptr;
	    curbuf->b_u_curhead = ਨਿਰਾ;
	}

	/*
	 * free headers to keep the size right
	 */
	ਜਦੋਂ (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != ਨਿਰਾ)
	{
	    u_ਸਿਰਲੇਖ_T	    *uhfree = curbuf->b_u_oldhead;

	    ਜੇ (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    ਨਹੀਂਤਾਂ ਜੇ (uhfree->uh_alt_ਅਗਲਾ.ptr == ਨਿਰਾ)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    ਨਹੀਂਤਾਂ
	    {
		// Free the oldest alternate branch as a whole.
		ਜਦੋਂ (uhfree->uh_alt_ਅਗਲਾ.ptr != ਨਿਰਾ)
		    uhfree = uhfree->uh_alt_ਅਗਲਾ.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
	    u_check(TRUE);
#ਜੇ_ਅੰਤ
	}

	ਜੇ (uhp == ਨਿਰਾ)		// no undo at all
	{
	    ਜੇ (old_curhead != ਨਿਰਾ)
		u_freebranch(curbuf, old_curhead, ਨਿਰਾ);
	    curbuf->b_u_synced = false;
	    ਵਾਪਸ OK;
	}

	uhp->uh_prev.ptr = ਨਿਰਾ;
	uhp->uh_ਅਗਲਾ.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_ਅਗਲਾ.ptr = old_curhead;
	ਜੇ (old_curhead != ਨਿਰਾ)
	{
	    uhp->uh_alt_prev.ptr = old_curhead->uh_alt_prev.ptr;
	    ਜੇ (uhp->uh_alt_prev.ptr != ਨਿਰਾ)
		uhp->uh_alt_prev.ptr->uh_alt_ਅਗਲਾ.ptr = uhp;
	    old_curhead->uh_alt_prev.ptr = uhp;
	    ਜੇ (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	ਨਹੀਂਤਾਂ
	    uhp->uh_alt_prev.ptr = ਨਿਰਾ;
	ਜੇ (curbuf->b_u_newhead != ਨਿਰਾ)
	    curbuf->b_u_newhead->uh_prev.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_ਆਖਰੀ;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_ਸਮਾਂ = vim_ਸਮਾਂ();
	uhp->uh_save_nr = 0;
	curbuf->b_u_ਸਮਾਂ_cur = uhp->uh_ਸਮਾਂ + 1;

	uhp->uh_walk = 0;
	uhp->uh_ਇੰਦਰਾਜ = ਨਿਰਾ;
	uhp->uh_getbot_ਇੰਦਰਾਜ = ਨਿਰਾ;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	ਜੇ (virtual_ਸਰਗਰਮ() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	ਨਹੀਂਤਾਂ
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_ਬਦਲਿਆ ? UH_ਬਦਲਿਆ : 0) +
		       ((curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, ਆਕਾਰ(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	ਜੇ (curbuf->b_u_oldhead == ਨਿਰਾ)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (get_undolevel() < 0)	// no undo at all
	    ਵਾਪਸ OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	ਜੇ (ਮਾਪ == 1)
	{
	    uep = u_get_headentry();
	    prev_uep = ਨਿਰਾ;
	    ਲਈ (i = 0; i < 10; ++i)
	    {
		ਜੇ (uep == ਨਿਰਾ)
		    ਰੋਕੋ;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		ਜੇ ((curbuf->b_u_newhead->uh_getbot_ਇੰਦਰਾਜ != uep
			    ? (uep->ue_ਸਿਖਰ + uep->ue_ਮਾਪ + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
			|| (uep->ue_ਮਾਪ > 1
			    && ਸਿਖਰ >= uep->ue_ਸਿਖਰ
			    && ਸਿਖਰ + 2 <= uep->ue_ਸਿਖਰ + uep->ue_ਮਾਪ + 1))
		    ਰੋਕੋ;

		// If it's the same line we can skip saving it again.
		ਜੇ (uep->ue_ਮਾਪ == 1 && uep->ue_ਸਿਖਰ == ਸਿਖਰ)
		{
		    ਜੇ (i > 0)
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
			prev_uep->ue_ਅਗਲਾ = uep->ue_ਅਗਲਾ;
			uep->ue_ਅਗਲਾ = curbuf->b_u_newhead->uh_ਇੰਦਰਾਜ;
			curbuf->b_u_newhead->uh_ਇੰਦਰਾਜ = uep;
		    }

		    // The executed command may change the line count.
		    ਜੇ (newbot != 0)
			uep->ue_bot = newbot;
		    ਨਹੀਂਤਾਂ ਜੇ (bot > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
			uep->ue_bot = 0;
		    ਨਹੀਂਤਾਂ
		    {
			uep->ue_lcount = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
			curbuf->b_u_newhead->uh_getbot_ਇੰਦਰਾਜ = uep;
		    }
		    ਵਾਪਸ OK;
		}
		prev_uep = uep;
		uep = uep->ue_ਅਗਲਾ;
	    }
	}

	// find line number for ue_bot for previous u_save()
	u_getbot();
    }

    /*
     * add lines in front of entry list
     */
    uep = U_ALLOC_ਸਤਰ(ਆਕਾਰ(u_ਇੰਦਰਾਜ_T));
    ਜੇ (uep == ਨਿਰਾ)
	ਜਾਓ nomem;
    CLEAR_ਸੂਚਕ(uep);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
    uep->ue_magic = UE_MAGIC;
#ਜੇ_ਅੰਤ

    uep->ue_ਮਾਪ = ਮਾਪ;
    uep->ue_ਸਿਖਰ = ਸਿਖਰ;
    ਜੇ (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    ਨਹੀਂਤਾਂ ਜੇ (bot > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	uep->ue_bot = 0;
    ਨਹੀਂਤਾਂ
    {
	uep->ue_lcount = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	curbuf->b_u_newhead->uh_getbot_ਇੰਦਰਾਜ = uep;
    }

    ਜੇ (ਮਾਪ > 0)
    {
	ਜੇ ((uep->ue_array = U_ALLOC_ਸਤਰ(ਆਕਾਰ(undoline_T) * ਮਾਪ)) == ਨਿਰਾ)
	{
	    u_freeentry(uep, 0L);
	    ਜਾਓ nomem;
	}
	ਲਈ (i = 0, lnum = ਸਿਖਰ + 1; i < ਮਾਪ; ++i)
	{
	    fast_breakcheck();
	    ਜੇ (got_int)
	    {
		u_freeentry(uep, i);
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (u_save_ਸਤਰ(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		ਜਾਓ nomem;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	uep->ue_array = ਨਿਰਾ;
    uep->ue_ਅਗਲਾ = curbuf->b_u_newhead->uh_ਇੰਦਰਾਜ;
    curbuf->b_u_newhead->uh_ਇੰਦਰਾਜ = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
    u_check(FALSE);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ OK;

nomem:
    ਸੁਨੇਹਾ_silent = 0;	// must display the prompt
    ਜੇ (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	ਵਾਪਸ OK;
    }
    do_outofmem_ਸੁਨੇਹਾ((long_u)0);
    ਵਾਪਸ FAIL;
}

#ਜੇ defined(FEAT_PERSISTENT_UNDO)

# define UF_ਸ਼ੁਰੂ_MAGIC	    "Vim\237UnDo\345"  // magic at start of undofile
# define UF_ਸ਼ੁਰੂ_MAGIC_LEN	9
# define UF_ਸਿਰਲੇਖ_MAGIC	0x5fd0	// magic at start of header
# define UF_ਸਿਰਲੇਖ_ਸਿਰਾ_MAGIC	0xe7aa	// magic after last header
# define UF_ਇੰਦਰਾਜ_MAGIC		0xf518	// magic at start of entry
# define UF_ਇੰਦਰਾਜ_ਸਿਰਾ_MAGIC	0x3581	// magic after last entry
# define UF_ਸੰਸਕਰਣ		2	// 2-byte undofile version number
# define UF_ਸੰਸਕਰਣ_CRYPT	0x8002	// idem, encrypted

// extra fields for header
# define UF_ਆਖਰੀ_SAVE_NR	1

// extra fields for uhp
# define UHP_SAVE_NR		1

/*
 * Compute the hash for the current buffer text into hash[UNDO_HASH_SIZE].
 */
    ਖਾਲੀ
u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;

    sha256_ਸ਼ੁਰੂ(&ctx);
    ਲਈ (lnum = 1; lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++lnum)
	sha256_update(&ctx, ml_get(lnum), (UINT32_T)(ml_get_len(lnum) + 1));
    sha256_ਅੰਤ(&ctx, hash);
}

/*
 * Return an allocated string of the full path of the target undofile.
 * When "reading" is TRUE find the file to read, go over all directories in
 * 'undodir'.
 * When "reading" is FALSE use the first name where the directory exists.
 * Returns NULL when there is no place to write or no file to read.
 */
    ਸਥਿਰ char_u *
u_get_undo_file_name(char_u *buf_ffname, ਪੂਰਨ reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {ਨਿਰਾ, 0};
    char_u	*undo_file_name = ਨਿਰਾ;
    ਪੂਰਨ		dir_len;
    char_u	*p;
    size_t	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    size_t	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    ਜੇ (ffname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    ਜੇ (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    ਜਦੋਂ (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	ਜੇ (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    ਜੇ (undo_file_name == ਨਿਰਾ)
		ਰੋਕੋ;
	    p = gettail(undo_file_name);
	    plen = (size_t)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# ਨਹੀਂਤਾਂ
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	ਨਹੀਂਤਾਂ
	{
	    dir_name[dir_len] = NUL;
	    ਜੇ (mch_isdir(dir_name))
	    {
		string_T    ret;

		ਜੇ (munged_name.string == ਨਿਰਾ)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    ਜੇ (munged_name.string == ਨਿਰਾ)
			ਵਾਪਸ ਨਿਰਾ;
		    munged_name.ਲੰਮਾਈ = ffnamelen;
		    ਲਈ (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			ਜੇ (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (size_t)dir_len,
		    munged_name.string, munged_name.ਲੰਮਾਈ, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	ਜੇ (undo_file_name != ਨਿਰਾ && (!reading
			       || mch_stat((ਅੱਖਰ *)undo_file_name, &st) >= 0))
	    ਰੋਕੋ;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    ਵਾਪਸ undo_file_name;
}

    ਸਥਿਰ ਖਾਲੀ
corruption_error(ਅੱਖਰ *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    ਸਥਿਰ ਖਾਲੀ
u_free_uhp(u_ਸਿਰਲੇਖ_T *uhp)
{
    u_ਇੰਦਰਾਜ_T	*nuep;
    u_ਇੰਦਰਾਜ_T	*uep;

    uep = uhp->uh_ਇੰਦਰਾਜ;
    ਜਦੋਂ (uep != ਨਿਰਾ)
    {
	nuep = uep->ue_ਅਗਲਾ;
	u_freeentry(uep, uep->ue_ਮਾਪ);
	uep = nuep;
    }
    vim_free(uhp);
}

/*
 * Write a sequence of bytes to the undo file.
 * Buffers and encrypts as needed.
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    ਜੇ (bi->bi_ਬਫਰ != ਨਿਰਾ)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	ਜਦੋਂ (bi->bi_ਵਰਤਿਆ + len_todo >= CRYPT_BUF_ਮਾਪ)
	{
	    size_t	n = CRYPT_BUF_ਮਾਪ - bi->bi_ਵਰਤਿਆ;

	    mch_memmove(bi->bi_ਬਫਰ + bi->bi_ਵਰਤਿਆ, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_ਵਰਤਿਆ = CRYPT_BUF_ਮਾਪ;
	    ਜੇ (undo_flush(bi) == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਜੇ (len_todo > 0)
	{
	    mch_memmove(bi->bi_ਬਫਰ + bi->bi_ਵਰਤਿਆ, p, len_todo);
	    bi->bi_ਵਰਤਿਆ += len_todo;
	}
	ਵਾਪਸ OK;
    }
# endif
    ਜੇ (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

# ifdef FEAT_CRYPT
    ਸਥਿਰ ਪੂਰਨ
undo_flush(bufinfo_T *bi)
{
    ਜੇ (bi->bi_ਬਫਰ != ਨਿਰਾ && bi->bi_state != ਨਿਰਾ && bi->bi_ਵਰਤਿਆ > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_ਬਫਰ, bi->bi_ਵਰਤਿਆ, FALSE);
	ਜੇ (fwrite(bi->bi_ਬਫਰ, bi->bi_ਵਰਤਿਆ, (size_t)1, bi->bi_fp) != 1)
	    ਵਾਪਸ FAIL;
	bi->bi_ਵਰਤਿਆ = 0;
    }
    ਵਾਪਸ OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    ਜੇ (bi->bi_state != ਨਿਰਾ && bi->bi_ਬਫਰ == ਨਿਰਾ)
    {
	// crypting every piece of text separately
	ਜੇ (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	ਨਹੀਂਤਾਂ
	{
	    copy = lalloc(len, FALSE);
	    ਜੇ (copy == ਨਿਰਾ)
		ਵਾਪਸ 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	ਜੇ (copy != small_buf)
	    vim_free(copy);
	ਵਾਪਸ i == 1 ? OK : FAIL;
    }
# endif
    ਵਾਪਸ undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
undo_write_ਬਾਈਟਾਂ(bufinfo_T *bi, long_u nr, ਪੂਰਨ len)
{
    char_u  buf[8];
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    bufi = 0;

    ਲਈ (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    ਵਾਪਸ undo_write(bi, buf, (size_t)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    ਸਥਿਰ ਖਾਲੀ
put_ਸਿਰਲੇਖ_ptr(bufinfo_T *bi, u_ਸਿਰਲੇਖ_T *uhp)
{
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)(uhp != ਨਿਰਾ ? uhp->uh_seq : 0), 4);
}

    ਸਥਿਰ ਪੂਰਨ
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ਜੇ (bi->bi_ਬਫਰ != ਨਿਰਾ)
    {
	char_u  buf[4];
	ਪੂਰਨ	n;

	undo_read(bi, buf, (size_t)4);
	n = ((ਬਿਨਾਨਿਸ਼ਾਨੀ)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	ਵਾਪਸ n;
    }
# endif
    ਵਾਪਸ get4c(bi->bi_fp);
}

    ਸਥਿਰ ਪੂਰਨ
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ਜੇ (bi->bi_ਬਫਰ != ਨਿਰਾ)
    {
	char_u  buf[2];
	ਪੂਰਨ	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	ਵਾਪਸ n;
    }
# endif
    ਵਾਪਸ get2c(bi->bi_fp);
}

    ਸਥਿਰ ਪੂਰਨ
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ਜੇ (bi->bi_ਬਫਰ != ਨਿਰਾ)
    {
	char_u  buf[1];

	undo_read(bi, buf, (size_t)1);
	ਵਾਪਸ buf[0];
    }
# endif
    ਵਾਪਸ getc(bi->bi_fp);
}

    ਸਥਿਰ time_t
undo_read_ਸਮਾਂ(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ਜੇ (bi->bi_ਬਫਰ != ਨਿਰਾ)
    {
	char_u  buf[8];
	time_t	n = 0;
	ਪੂਰਨ	i;

	undo_read(bi, buf, (size_t)8);
	ਲਈ (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	ਵਾਪਸ n;
    }
# endif
    ਵਾਪਸ get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
undo_read(bufinfo_T *bi, char_u *ਬਫਰ, size_t ਮਾਪ)
{
    ਪੂਰਨ retval = OK;

# ifdef FEAT_CRYPT
    ਜੇ (bi->bi_ਬਫਰ != ਨਿਰਾ)
    {
	ਪੂਰਨ	ਮਾਪ_todo = (ਪੂਰਨ)ਮਾਪ;
	char_u	*p = ਬਫਰ;

	ਜਦੋਂ (ਮਾਪ_todo > 0)
	{
	    size_t n;

	    ਜੇ (bi->bi_ਵਰਤਿਆ >= bi->bi_avail)
	    {
		n = fread(bi->bi_ਬਫਰ, 1, (size_t)CRYPT_BUF_ਮਾਪ, bi->bi_fp);
		ਜੇ (n == 0)
		{
		    retval = FAIL;
		    ਰੋਕੋ;
		}
		bi->bi_avail = n;
		bi->bi_ਵਰਤਿਆ = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_ਬਫਰ, bi->bi_avail, FALSE);
	    }
	    n = ਮਾਪ_todo;
	    ਜੇ (n > bi->bi_avail - bi->bi_ਵਰਤਿਆ)
		n = bi->bi_avail - bi->bi_ਵਰਤਿਆ;
	    mch_memmove(p, bi->bi_ਬਫਰ + bi->bi_ਵਰਤਿਆ, n);
	    bi->bi_ਵਰਤਿਆ += n;
	    ਮਾਪ_todo -= (ਪੂਰਨ)n;
	    p += n;
	}
    }
    ਨਹੀਂਤਾਂ
# endif
    ਜੇ (fread(ਬਫਰ, ਮਾਪ, 1, bi->bi_fp) != 1)
	retval = FAIL;

    ਜੇ (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(ਬਫਰ, 0, ਮਾਪ);
    ਵਾਪਸ retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    ਸਥਿਰ char_u *
read_string_decrypt(bufinfo_T *bi, ਪੂਰਨ len)
{
    char_u  *ptr = alloc(len + 1);

    ਜੇ (ptr == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	ਵਾਪਸ ਨਿਰਾ;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    ਜੇ (bi->bi_state != ਨਿਰਾ && bi->bi_ਬਫਰ == ਨਿਰਾ)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    ਵਾਪਸ ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    ਸਥਿਰ ਪੂਰਨ
serialize_ਸਿਰਲੇਖ(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	ਸਮਾਂ_buf[8];

    // Start writing, first the magic marker and undo info version.
    ਜੇ (fwrite(UF_ਸ਼ੁਰੂ_MAGIC, (size_t)UF_ਸ਼ੁਰੂ_MAGIC_LEN, (size_t)1, fp) != 1)
	ਵਾਪਸ FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    ਜੇ (*buf->b_p_ਕੁੰਜੀ != NUL)
    {
	char_u *ਸਿਰਲੇਖ;
	ਪੂਰਨ    ਸਿਰਲੇਖ_len;
	ਲੰਮਾ	len;

	undo_write_ਬਾਈਟਾਂ(bi, (long_u)UF_ਸੰਸਕਰਣ_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_ਵਿਧੀ_nr(buf),
					  buf->b_p_ਕੁੰਜੀ, &ਸਿਰਲੇਖ, &ਸਿਰਲੇਖ_len);
	ਜੇ (bi->bi_state == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	len = (ਲੰਮਾ)fwrite(ਸਿਰਲੇਖ, (size_t)ਸਿਰਲੇਖ_len, (size_t)1, fp);
	vim_free(ਸਿਰਲੇਖ);
	ਜੇ (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = ਨਿਰਾ;
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (crypt_ਸਾਰਾ_undofile(crypt_get_ਵਿਧੀ_nr(buf)))
	{
	    bi->bi_ਬਫਰ = alloc(CRYPT_BUF_ਮਾਪ);
	    ਜੇ (bi->bi_ਬਫਰ == ਨਿਰਾ)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = ਨਿਰਾ;
		ਵਾਪਸ FAIL;
	    }
	    bi->bi_ਵਰਤਿਆ = 0;
	}
    }
    ਨਹੀਂਤਾਂ
# endif
	undo_write_ਬਾਈਟਾਂ(bi, (long_u)UF_ਸੰਸਕਰਣ, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    ਜੇ (undo_write(bi, hash, (size_t)UNDO_HASH_ਮਾਪ) == FAIL)
	ਵਾਪਸ FAIL;

    // buffer-specific data
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)buf->b_u_ਸਤਰ_ptr.ul_textlen, 4);
    ਜੇ (buf->b_u_ਸਤਰ_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_ਸਤਰ_ptr.ul_ਸਤਰ,
			(size_t)buf->b_u_ਸਤਰ_ptr.ul_textlen) == FAIL)
	ਵਾਪਸ FAIL;
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)buf->b_u_ਸਤਰ_lnum, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)buf->b_u_ਸਤਰ_colnr, 4);

    // Undo structures header data
    put_ਸਿਰਲੇਖ_ptr(bi, buf->b_u_oldhead);
    put_ਸਿਰਲੇਖ_ptr(bi, buf->b_u_newhead);
    put_ਸਿਰਲੇਖ_ptr(bi, buf->b_u_curhead);

    undo_write_ਬਾਈਟਾਂ(bi, (long_u)buf->b_u_numhead, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)buf->b_u_seq_ਆਖਰੀ, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)buf->b_u_seq_cur, 4);
    ਸਮਾਂ_ਮੰਜ਼ਿਲ_ਬਾਈਟਾਂ(buf->b_u_ਸਮਾਂ_cur, ਸਮਾਂ_buf);
    undo_write(bi, ਸਮਾਂ_buf, 8);

    // Optional fields.
    undo_write_ਬਾਈਟਾਂ(bi, 4, 1);
    undo_write_ਬਾਈਟਾਂ(bi, UF_ਆਖਰੀ_SAVE_NR, 1);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)buf->b_u_save_nr_ਆਖਰੀ, 4);

    undo_write_ਬਾਈਟਾਂ(bi, 0, 1);  // end marker

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
serialize_uhp(bufinfo_T *bi, u_ਸਿਰਲੇਖ_T *uhp)
{
    ਪੂਰਨ		i;
    u_ਇੰਦਰਾਜ_T	*uep;
    char_u	ਸਮਾਂ_buf[8];

    ਜੇ (undo_write_ਬਾਈਟਾਂ(bi, (long_u)UF_ਸਿਰਲੇਖ_MAGIC, 2) == FAIL)
	ਵਾਪਸ FAIL;

    put_ਸਿਰਲੇਖ_ptr(bi, uhp->uh_ਅਗਲਾ.ptr);
    put_ਸਿਰਲੇਖ_ptr(bi, uhp->uh_prev.ptr);
    put_ਸਿਰਲੇਖ_ptr(bi, uhp->uh_alt_ਅਗਲਾ.ptr);
    put_ਸਿਰਲੇਖ_ptr(bi, uhp->uh_alt_prev.ptr);
    undo_write_ਬਾਈਟਾਂ(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    ਲਈ (i = 0; i < NMARKS; ++i)
	serialize_pos(bi, uhp->uh_namedm[i]);
    serialize_visualinfo(bi, &uhp->uh_visual);
    ਸਮਾਂ_ਮੰਜ਼ਿਲ_ਬਾਈਟਾਂ(uhp->uh_ਸਮਾਂ, ਸਮਾਂ_buf);
    undo_write(bi, ਸਮਾਂ_buf, 8);

    // Optional fields.
    undo_write_ਬਾਈਟਾਂ(bi, 4, 1);
    undo_write_ਬਾਈਟਾਂ(bi, UHP_SAVE_NR, 1);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)uhp->uh_save_nr, 4);

    undo_write_ਬਾਈਟਾਂ(bi, 0, 1);  // end marker

    // Write all the entries.
    ਲਈ (uep = uhp->uh_ਇੰਦਰਾਜ; uep != ਨਿਰਾ; uep = uep->ue_ਅਗਲਾ)
    {
	undo_write_ਬਾਈਟਾਂ(bi, (long_u)UF_ਇੰਦਰਾਜ_MAGIC, 2);
	ਜੇ (serialize_uep(bi, uep) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)UF_ਇੰਦਰਾਜ_ਸਿਰਾ_MAGIC, 2);
    ਵਾਪਸ OK;
}

    ਸਥਿਰ u_ਸਿਰਲੇਖ_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_ਸਿਰਲੇਖ_T	*uhp;
    ਪੂਰਨ		i;
    u_ਇੰਦਰਾਜ_T	*uep, *ਆਖਰੀ_uep;
    ਪੂਰਨ		c;
    ਪੂਰਨ		error;

    uhp = U_ALLOC_ਸਤਰ(ਆਕਾਰ(u_ਸਿਰਲੇਖ_T));
    ਜੇ (uhp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    CLEAR_ਸੂਚਕ(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_ਅਗਲਾ.seq = undo_read_4c(bi);
    uhp->uh_prev.seq = undo_read_4c(bi);
    uhp->uh_alt_ਅਗਲਾ.seq = undo_read_4c(bi);
    uhp->uh_alt_prev.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    ਜੇ (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	ਵਾਪਸ ਨਿਰਾ;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    ਲਈ (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_ਸਮਾਂ = undo_read_ਸਮਾਂ(bi);

    // Optional fields.
    ਲਈ (;;)
    {
	ਪੂਰਨ len = undo_read_byte(bi);
	ਪੂਰਨ what;

	ਜੇ (len == EOF)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ਜੇ (len == 0)
	    ਰੋਕੋ;
	what = undo_read_byte(bi);
	ਚੋਣ (what)
	{
	    ਹਾਲਤ UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		ਰੋਕੋ;
	    ਮੂਲ:
		// field not supported, skip
		ਜਦੋਂ (--len >= 0)
		    (ਖਾਲੀ)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    ਆਖਰੀ_uep = ਨਿਰਾ;
    ਜਦੋਂ ((c = undo_read_2c(bi)) == UF_ਇੰਦਰਾਜ_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	ਜੇ (ਆਖਰੀ_uep == ਨਿਰਾ)
	    uhp->uh_ਇੰਦਰਾਜ = uep;
	ਨਹੀਂਤਾਂ
	    ਆਖਰੀ_uep->ue_ਅਗਲਾ = uep;
	ਆਖਰੀ_uep = uep;
	ਜੇ (uep == ਨਿਰਾ || error)
	{
	    u_free_uhp(uhp);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਜੇ (c != UF_ਇੰਦਰਾਜ_ਸਿਰਾ_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ uhp;
}

/*
 * Serialize "uep".
 */
    ਸਥਿਰ ਪੂਰਨ
serialize_uep(
    bufinfo_T	*bi,
    u_ਇੰਦਰਾਜ_T	*uep)
{
    ਪੂਰਨ		i;

    undo_write_ਬਾਈਟਾਂ(bi, (long_u)uep->ue_ਸਿਖਰ, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)uep->ue_bot, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)uep->ue_lcount, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)uep->ue_ਮਾਪ, 4);
    ਲਈ (i = 0; i < uep->ue_ਮਾਪ; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	ਜੇ (undo_write_ਬਾਈਟਾਂ(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    ਵਾਪਸ FAIL;
	ਜੇ (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_ਸਤਰ, uep->ue_array[i].ul_textlen) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

    ਸਥਿਰ u_ਇੰਦਰਾਜ_T *
unserialize_uep(bufinfo_T *bi, ਪੂਰਨ *error, char_u *file_name)
{
    ਪੂਰਨ		i;
    u_ਇੰਦਰਾਜ_T	*uep;
    undoline_T	*array = ਨਿਰਾ;
    char_u	*ਸਤਰ;
    ਪੂਰਨ		ਸਤਰ_len;

    uep = U_ALLOC_ਸਤਰ(ਆਕਾਰ(u_ਇੰਦਰਾਜ_T));
    ਜੇ (uep == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    CLEAR_ਸੂਚਕ(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_ਸਿਖਰ = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_ਮਾਪ = undo_read_4c(bi);
    ਜੇ (uep->ue_ਮਾਪ > 0)
    {
	ਜੇ (uep->ue_ਮਾਪ < LONG_MAX / (ਲੰਮਾ)ਆਕਾਰ(undoline_T))
	    array = U_ALLOC_ਸਤਰ(ਆਕਾਰ(undoline_T) * uep->ue_ਮਾਪ);
	ਜੇ (array == ਨਿਰਾ)
	{
	    *error = TRUE;
	    ਵਾਪਸ uep;
	}
	vim_memset(array, 0, ਆਕਾਰ(undoline_T) * uep->ue_ਮਾਪ);
    }
    uep->ue_array = array;

    ਲਈ (i = 0; i < uep->ue_ਮਾਪ; ++i)
    {
	ਸਤਰ_len = undo_read_4c(bi);
	ਜੇ (ਸਤਰ_len >= 0)
	    ਸਤਰ = read_string_decrypt(bi, ਸਤਰ_len);
	ਨਹੀਂਤਾਂ
	{
	    ਸਤਰ = ਨਿਰਾ;
	    corruption_error("line length", file_name);
	}
	ਜੇ (ਸਤਰ == ਨਿਰਾ)
	{
	    *error = TRUE;
	    ਵਾਪਸ uep;
	}
	array[i].ul_ਸਤਰ = ਸਤਰ;
	array[i].ul_len = ਸਤਰ_len + 1;
	array[i].ul_textlen = ਸਤਰ_len;
    }
    ਵਾਪਸ uep;
}

/*
 * Serialize "pos".
 */
    ਸਥਿਰ ਖਾਲੀ
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)pos.lnum, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)pos.ਖਾਨਾ, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    ਸਥਿਰ ਖਾਲੀ
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    ਜੇ (pos->lnum < 0)
	pos->lnum = 0;
    pos->ਖਾਨਾ = undo_read_4c(bi);
    ਜੇ (pos->ਖਾਨਾ < 0)
	pos->ਖਾਨਾ = 0;
    pos->coladd = undo_read_4c(bi);
    ਜੇ (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    ਸਥਿਰ ਖਾਲੀ
serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    serialize_pos(bi, info->vi_ਸ਼ੁਰੂ);
    serialize_pos(bi, info->vi_ਸਿਰਾ);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)info->vi_ਬਹੁਲਕ, 4);
    undo_write_ਬਾਈਟਾਂ(bi, (long_u)info->vi_curswant, 4);
}

/*
 * Unserialize the visualinfo_T at the current position.
 */
    ਸਥਿਰ ਖਾਲੀ
unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    unserialize_pos(bi, &info->vi_ਸ਼ੁਰੂ);
    unserialize_pos(bi, &info->vi_ਸਿਰਾ);
    info->vi_ਬਹੁਲਕ = undo_read_4c(bi);
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
    ਖਾਲੀ
u_write_undo(
    char_u	*name,
    ਪੂਰਨ		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_ਸਿਰਲੇਖ_T	*uhp;
    char_u	*file_name;
    ਪੂਰਨ		mark;
# ifdef U_DEBUG
    ਪੂਰਨ		ਸਿਰਲੇਖਾਂ_written = 0;
# endif
    ਪੂਰਨ		fd;
    FILE	*fp = ਨਿਰਾ;
    ਪੂਰਨ		perm;
    ਪੂਰਨ		write_ok = FALSE;
# ifdef UNIX
    ਪੂਰਨ		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_ਖੇਤਰ(bi);

    ਜੇ (name == ਨਿਰਾ)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	ਜੇ (file_name == ਨਿਰਾ)
	{
	    ਜੇ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    ਜੇ (buf->b_ffname != ਨਿਰਾ)
    {
# ifdef UNIX
	ਜੇ (mch_stat((ਅੱਖਰ *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_ਬਹੁਲਕ;
	    st_old_valid = TRUE;
	}
# ਨਹੀਂਤਾਂ
	perm = mch_getperm(buf->b_ffname);
	ਜੇ (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    ਜੇ (mch_getperm(file_name) >= 0)
    {
	ਜੇ (name == ਨਿਰਾ || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((ਅੱਖਰ *)file_name, O_RDONLY|O_EXTRA, 0);
	    ਜੇ (fd < 0)
	    {
		ਜੇ (name != ਨਿਰਾ || p_verbose > 0)
		{
		    ਜੇ (name == ਨਿਰਾ)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    ਜੇ (name == ਨਿਰਾ)
			verbose_leave();
		}
		ਜਾਓ theend;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		char_u	mbuf[UF_ਸ਼ੁਰੂ_MAGIC_LEN];
		ਪੂਰਨ	len;

		len = read_eintr(fd, mbuf, UF_ਸ਼ੁਰੂ_MAGIC_LEN);
		close(fd);
		ਜੇ (len < UF_ਸ਼ੁਰੂ_MAGIC_LEN
		      || memcmp(mbuf, UF_ਸ਼ੁਰੂ_MAGIC, UF_ਸ਼ੁਰੂ_MAGIC_LEN) != 0)
		{
		    ਜੇ (name != ਨਿਰਾ || p_verbose > 0)
		    {
			ਜੇ (name == ਨਿਰਾ)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			ਜੇ (name == ਨਿਰਾ)
			    verbose_leave();
		    }
		    ਜਾਓ theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    ਜੇ (buf->b_u_numhead == 0 && buf->b_u_ਸਤਰ_ptr.ul_ਸਤਰ == ਨਿਰਾ)
    {
	ਜੇ (p_verbose > 0)
	    verb_ਸੁਨੇਹਾ(_("Skipping undo file write, nothing to undo"));
	ਜਾਓ theend;
    }

    fd = mch_open((ਅੱਖਰ *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    ਜੇ (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	ਜਾਓ theend;
    }
    (ਖਾਲੀ)mch_setperm(file_name, perm);
    ਜੇ (p_verbose > 0)
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
    ਜੇ (st_old_valid
	    && mch_stat((ਅੱਖਰ *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  ਜੇ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    ਜੇ (buf->b_ffname != ਨਿਰਾ)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    ਜੇ (fp == ਨਿਰਾ)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	close(fd);
	mch_remove(file_name);
	ਜਾਓ theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    ਜੇ (serialize_ਸਿਰਲੇਖ(&bi, hash) == FAIL)
	ਜਾਓ write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    ਜਦੋਂ (uhp != ਨਿਰਾ)
    {
	// Serialize current UHP if we haven't seen it
	ਜੇ (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++ਸਿਰਲੇਖਾਂ_written;
# endif
	    ਜੇ (serialize_uhp(&bi, uhp) == FAIL)
		ਜਾਓ write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	ਜੇ (uhp->uh_prev.ptr != ਨਿਰਾ && uhp->uh_prev.ptr->uh_walk != mark)
	    uhp = uhp->uh_prev.ptr;
	ਨਹੀਂਤਾਂ ਜੇ (uhp->uh_alt_ਅਗਲਾ.ptr != ਨਿਰਾ
				     && uhp->uh_alt_ਅਗਲਾ.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_ਅਗਲਾ.ptr;
	ਨਹੀਂਤਾਂ ਜੇ (uhp->uh_ਅਗਲਾ.ptr != ਨਿਰਾ && uhp->uh_alt_prev.ptr == ਨਿਰਾ
					 && uhp->uh_ਅਗਲਾ.ptr->uh_walk != mark)
	    uhp = uhp->uh_ਅਗਲਾ.ptr;
	ਨਹੀਂਤਾਂ ਜੇ (uhp->uh_alt_prev.ptr != ਨਿਰਾ)
	    uhp = uhp->uh_alt_prev.ptr;
	ਨਹੀਂਤਾਂ
	    uhp = uhp->uh_ਅਗਲਾ.ptr;
    }

    ਜੇ (undo_write_ਬਾਈਟਾਂ(&bi, (long_u)UF_ਸਿਰਲੇਖ_ਸਿਰਾ_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    ਜੇ (ਸਿਰਲੇਖਾਂ_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", ਸਿਰਲੇਖਾਂ_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    ਜੇ (bi.bi_state != ਨਿਰਾ && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# ਜੇ defined(UNIX) && defined(HAVE_FSYNC)
    ਜੇ ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && fflush(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    fclose(fp);
    ਜੇ (!write_ok)
	semsg(_(e_write_error_ਵਿੱਚ_undo_file_str), file_name);

# ਜੇ defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    ਜੇ (buf->b_ffname != ਨਿਰਾ)
	(ਖਾਲੀ)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    ਜੇ (buf->b_ffname != ਨਿਰਾ)
    {
	vim_acl_T	    acl;

	// For systems that support ACL: get the ACL from the original file.
	acl = mch_get_acl(buf->b_ffname);
	mch_ਸੈੱਟ_acl(file_name, acl);
	mch_free_acl(acl);
    }
# endif

theend:
# ifdef FEAT_CRYPT
    ਜੇ (bi.bi_state != ਨਿਰਾ)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_ਬਫਰ);
# endif
    ਜੇ (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    ਸਥਿਰ ਪੂਰਨ
uhp_seq_cmp(ਅਚਲ ਖਾਲੀ *v1, ਅਚਲ ਖਾਲੀ *v2)
{
    ਅਚਲ u_ਸਿਰਲੇਖ_T *u1 = *(u_ਸਿਰਲੇਖ_T **)v1;
    ਅਚਲ u_ਸਿਰਲੇਖ_T *u2 = *(u_ਸਿਰਲੇਖ_T **)v2;

    ਵਾਪਸ u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    ਸਥਿਰ ਲੰਮਾ
uhp_ਸਾਰਣੀ_ਲੱਭੋ(u_ਸਿਰਲੇਖ_T **uhp_ਸਾਰਣੀ, ਲੰਮਾ ਅੰਕ_ਸਿਰ, ਲੰਮਾ seq)
{
    ਲੰਮਾ    lo = 0;
    ਲੰਮਾ    hi = ਅੰਕ_ਸਿਰ - 1;

    ਜਦੋਂ (lo <= hi)
    {
	ਲੰਮਾ mid = lo + (hi - lo) / 2;

	ਜੇ (uhp_ਸਾਰਣੀ[mid]->uh_seq < seq)
	    lo = mid + 1;
	ਨਹੀਂਤਾਂ ਜੇ (uhp_ਸਾਰਣੀ[mid]->uh_seq > seq)
	    hi = mid - 1;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ mid;
    }
    ਵਾਪਸ -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    ਖਾਲੀ
u_read_undo(char_u *name, char_u *hash, char_u *orig_name ਅਣਵਰਤਿਆ)
{
    char_u	*file_name;
    FILE	*fp;
    ਲੰਮਾ	ਸੰਸਕਰਣ_2, str_len;
    undoline_T	ਸਤਰ_ptr;
    linenr_T	ਸਤਰ_lnum;
    colnr_T	ਸਤਰ_colnr;
    linenr_T	ਸਤਰ_ਗਿਣਤੀ;
    ਲੰਮਾ	ਅੰਕ_ਸਿਰ = 0;
    ਲੰਮਾ	old_ਸਿਰਲੇਖ_seq, new_ਸਿਰਲੇਖ_seq, cur_ਸਿਰਲੇਖ_seq;
    ਲੰਮਾ	seq_ਆਖਰੀ, seq_cur;
    ਲੰਮਾ	ਆਖਰੀ_save_nr = 0;
    ਲੰਮਾ	old_idx = -1, new_idx = -1, cur_idx = -1;
    ਲੰਮਾ	ਅੰਕ_read_uhps = 0;
    time_t	seq_ਸਮਾਂ;
    ਲੰਮਾ	i;
    ਪੂਰਨ		c;
    u_ਸਿਰਲੇਖ_T	*uhp;
    u_ਸਿਰਲੇਖ_T	**uhp_ਸਾਰਣੀ = ਨਿਰਾ;
    char_u	read_hash[UNDO_HASH_ਮਾਪ];
    char_u	magic_buf[UF_ਸ਼ੁਰੂ_MAGIC_LEN];
# ifdef U_DEBUG
    ਪੂਰਨ		*uhp_ਸਾਰਣੀ_ਵਰਤਿਆ;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_ਖੇਤਰ(bi);
    ਸਤਰ_ptr.ul_len = 0;
    ਸਤਰ_ptr.ul_textlen = 0;
    ਸਤਰ_ptr.ul_ਸਤਰ = ਨਿਰਾ;

    ਜੇ (name == ਨਿਰਾ)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	ਜੇ (file_name == ਨਿਰਾ)
	    ਵਾਪਸ;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	ਜੇ (mch_stat((ਅੱਖਰ *)orig_name, &st_orig) >= 0
		&& mch_stat((ਅੱਖਰ *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    ਜੇ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    ਵਾਪਸ;
	}
# endif
    }
    ਨਹੀਂਤਾਂ
	file_name = name;

    ਜੇ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((ਅੱਖਰ *)file_name, "r");
    ਜੇ (fp == ਨਿਰਾ)
    {
	ਜੇ (name != ਨਿਰਾ || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	ਜਾਓ error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    ਜੇ (fread(magic_buf, UF_ਸ਼ੁਰੂ_MAGIC_LEN, 1, fp) != 1
		|| memcmp(magic_buf, UF_ਸ਼ੁਰੂ_MAGIC, UF_ਸ਼ੁਰੂ_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	ਜਾਓ error;
    }
    ਸੰਸਕਰਣ_2 = get2c(fp);
    ਜੇ (ਸੰਸਕਰਣ_2 == UF_ਸੰਸਕਰਣ_CRYPT)
    {
# ifdef FEAT_CRYPT
	ਜੇ (*curbuf->b_p_ਕੁੰਜੀ == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    ਜਾਓ error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_ਕੁੰਜੀ);
	ਜੇ (bi.bi_state == ਨਿਰਾ)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    ਜਾਓ error;
	}
	ਜੇ (crypt_ਸਾਰਾ_undofile(bi.bi_state->ਵਿਧੀ_nr))
	{
	    bi.bi_ਬਫਰ = alloc(CRYPT_BUF_ਮਾਪ);
	    ਜੇ (bi.bi_ਬਫਰ == ਨਿਰਾ)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = ਨਿਰਾ;
		ਜਾਓ error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_ਵਰਤਿਆ = 0;
	}
# ਨਹੀਂਤਾਂ
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	ਜਾਓ error;
# endif
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਸੰਸਕਰਣ_2 != UF_ਸੰਸਕਰਣ)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	ਜਾਓ error;
    }

    ਜੇ (undo_read(&bi, read_hash, (size_t)UNDO_HASH_ਮਾਪ) == FAIL)
    {
	corruption_error("hash", file_name);
	ਜਾਓ error;
    }
    ਸਤਰ_ਗਿਣਤੀ = (linenr_T)undo_read_4c(&bi);
    ਜੇ (memcmp(hash, read_hash, UNDO_HASH_ਮਾਪ) != 0
				  || ਸਤਰ_ਗਿਣਤੀ != curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	ਜੇ (p_verbose > 0 || name != ਨਿਰਾ)
	{
	    ਜੇ (name == ਨਿਰਾ)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    ਜੇ (name == ਨਿਰਾ)
		verbose_leave();
	}
	ਜਾਓ error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    ਜੇ (str_len < 0)
	ਜਾਓ error;
    ਜੇ (str_len > 0)
    {
	ਸਤਰ_ptr.ul_ਸਤਰ = read_string_decrypt(&bi, str_len);
	ਸਤਰ_ptr.ul_len = str_len + 1;
	ਸਤਰ_ptr.ul_textlen = str_len;
    }
    ਸਤਰ_lnum = (linenr_T)undo_read_4c(&bi);
    ਸਤਰ_colnr = (colnr_T)undo_read_4c(&bi);
    ਜੇ (ਸਤਰ_lnum < 0 || ਸਤਰ_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	ਜਾਓ error;
    }

    // Begin general undo data
    old_ਸਿਰਲੇਖ_seq = undo_read_4c(&bi);
    new_ਸਿਰਲੇਖ_seq = undo_read_4c(&bi);
    cur_ਸਿਰਲੇਖ_seq = undo_read_4c(&bi);
    ਅੰਕ_ਸਿਰ = undo_read_4c(&bi);
    seq_ਆਖਰੀ = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_ਸਮਾਂ = undo_read_ਸਮਾਂ(&bi);

    // Optional header fields.
    ਲਈ (;;)
    {
	ਪੂਰਨ len = undo_read_byte(&bi);
	ਪੂਰਨ what;

	ਜੇ (len == 0 || len == EOF)
	    ਰੋਕੋ;
	what = undo_read_byte(&bi);
	ਚੋਣ (what)
	{
	    ਹਾਲਤ UF_ਆਖਰੀ_SAVE_NR:
		ਆਖਰੀ_save_nr = undo_read_4c(&bi);
		ਰੋਕੋ;
	    ਮੂਲ:
		// field not supported, skip
		ਜਦੋਂ (--len >= 0)
		    (ਖਾਲੀ)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    ਜੇ (ਅੰਕ_ਸਿਰ > 0)
    {
	ਜੇ (ਅੰਕ_ਸਿਰ < LONG_MAX / (ਲੰਮਾ)ਆਕਾਰ(u_ਸਿਰਲੇਖ_T *))
	    uhp_ਸਾਰਣੀ = U_ALLOC_ਸਤਰ(ਅੰਕ_ਸਿਰ * ਆਕਾਰ(u_ਸਿਰਲੇਖ_T *));
	ਜੇ (uhp_ਸਾਰਣੀ == ਨਿਰਾ)
	    ਜਾਓ error;
    }

    ਜਦੋਂ ((c = undo_read_2c(&bi)) == UF_ਸਿਰਲੇਖ_MAGIC)
    {
	ਜੇ (ਅੰਕ_read_uhps >= ਅੰਕ_ਸਿਰ)
	{
	    corruption_error("num_head too small", file_name);
	    ਜਾਓ error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	ਜੇ (uhp == ਨਿਰਾ)
	    ਜਾਓ error;
	uhp_ਸਾਰਣੀ[ਅੰਕ_read_uhps++] = uhp;
    }

    ਜੇ (ਅੰਕ_read_uhps != ਅੰਕ_ਸਿਰ)
    {
	corruption_error("num_head", file_name);
	ਜਾਓ error;
    }
    ਜੇ (c != UF_ਸਿਰਲੇਖ_ਸਿਰਾ_MAGIC)
    {
	corruption_error("end marker", file_name);
	ਜਾਓ error;
    }

# ifdef U_DEBUG
    uhp_ਸਾਰਣੀ_ਵਰਤਿਆ = alloc_clear(ਆਕਾਰ(ਪੂਰਨ) * ਅੰਕ_ਸਿਰ + 1);
#  define ਸੈੱਟ_FLAG(j) ++uhp_ਸਾਰਣੀ_ਵਰਤਿਆ[j]
# ਨਹੀਂਤਾਂ
#  define ਸੈੱਟ_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    ਜੇ (ਅੰਕ_ਸਿਰ > 0)
	qsort(uhp_ਸਾਰਣੀ, (size_t)ਅੰਕ_ਸਿਰ, ਆਕਾਰ(u_ਸਿਰਲੇਖ_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    ਲਈ (i = 0; i < ਅੰਕ_ਸਿਰ - 1; i++)
    {
	ਜੇ (uhp_ਸਾਰਣੀ[i]->uh_seq == uhp_ਸਾਰਣੀ[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    ਜਾਓ error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    ਕਰੋ { \
	ਲੰਮਾ fidx = uhp_ਸਾਰਣੀ_ਲੱਭੋ(uhp_ਸਾਰਣੀ, ਅੰਕ_ਸਿਰ, (link).seq); \
	\
	ਜੇ (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_ਸਾਰਣੀ[fidx]; \
	    ਸੈੱਟ_FLAG(fidx); \
	} \
	ਨਹੀਂਤਾਂ \
	    (link).ptr = ਨਿਰਾ; \
    } ਜਦੋਂ (0)

    ਲਈ (i = 0; i < ਅੰਕ_ਸਿਰ; i++)
    {
	uhp = uhp_ਸਾਰਣੀ[i];
	SWIZZLE_SEQ(uhp->uh_ਅਗਲਾ, i);
	SWIZZLE_SEQ(uhp->uh_prev, i);
	SWIZZLE_SEQ(uhp->uh_alt_ਅਗਲਾ, i);
	SWIZZLE_SEQ(uhp->uh_alt_prev, i);
	ਜੇ (old_ਸਿਰਲੇਖ_seq > 0 && old_idx < 0 && uhp->uh_seq == old_ਸਿਰਲੇਖ_seq)
	{
	    old_idx = i;
	    ਸੈੱਟ_FLAG(i);
	}
	ਜੇ (new_ਸਿਰਲੇਖ_seq > 0 && new_idx < 0 && uhp->uh_seq == new_ਸਿਰਲੇਖ_seq)
	{
	    new_idx = i;
	    ਸੈੱਟ_FLAG(i);
	}
	ਜੇ (cur_ਸਿਰਲੇਖ_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_ਸਿਰਲੇਖ_seq)
	{
	    cur_idx = i;
	    ਸੈੱਟ_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? ਨਿਰਾ : uhp_ਸਾਰਣੀ[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? ਨਿਰਾ : uhp_ਸਾਰਣੀ[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? ਨਿਰਾ : uhp_ਸਾਰਣੀ[cur_idx];
    curbuf->b_u_ਸਤਰ_ptr = ਸਤਰ_ptr;
    curbuf->b_u_ਸਤਰ_lnum = ਸਤਰ_lnum;
    curbuf->b_u_ਸਤਰ_colnr = ਸਤਰ_colnr;
    curbuf->b_u_numhead = ਅੰਕ_ਸਿਰ;
    curbuf->b_u_seq_ਆਖਰੀ = seq_ਆਖਰੀ;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_ਸਮਾਂ_cur = seq_ਸਮਾਂ;
    curbuf->b_u_save_nr_ਆਖਰੀ = ਆਖਰੀ_save_nr;
    curbuf->b_u_save_nr_cur = ਆਖਰੀ_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_ਸਾਰਣੀ);

# ifdef U_DEBUG
    ਲਈ (i = 0; i < ਅੰਕ_ਸਿਰ; ++i)
	ਜੇ (uhp_ਸਾਰਣੀ_ਵਰਤਿਆ[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_ਸਾਰਣੀ_ਵਰਤਿਆ);
    u_check(TRUE);
# endif

    ਜੇ (name != ਨਿਰਾ)
	smsg(_("Finished reading undo file %s"), file_name);
    ਜਾਓ theend;

error:
    vim_free(ਸਤਰ_ptr.ul_ਸਤਰ);
    ਜੇ (uhp_ਸਾਰਣੀ != ਨਿਰਾ)
    {
	ਲਈ (i = 0; i < ਅੰਕ_read_uhps; i++)
	    ਜੇ (uhp_ਸਾਰਣੀ[i] != ਨਿਰਾ)
		u_free_uhp(uhp_ਸਾਰਣੀ[i]);
	vim_free(uhp_ਸਾਰਣੀ);
    }

theend:
# ifdef FEAT_CRYPT
    ਜੇ (bi.bi_state != ਨਿਰਾ)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_ਬਫਰ);
# endif
    ਜੇ (fp != ਨਿਰਾ)
	fclose(fp);
    ਜੇ (file_name != name)
	vim_free(file_name);
    ਵਾਪਸ;
}

#ਜੇ_ਅੰਤ // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    ਖਾਲੀ
u_undo(ਪੂਰਨ ਗਿਣਤੀ_2)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    ਜੇ (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	ਗਿਣਤੀ_2 = 1;
    }

    ਜੇ (vim_strchr(p_cpo, CPO_UNDO) == ਨਿਰਾ)
	undo_undoes = TRUE;
    ਨਹੀਂਤਾਂ
	undo_undoes = !undo_undoes;
    u_doit(ਗਿਣਤੀ_2);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    ਖਾਲੀ
u_redo(ਪੂਰਨ ਗਿਣਤੀ_2)
{
    ਜੇ (vim_strchr(p_cpo, CPO_UNDO) == ਨਿਰਾ)
	undo_undoes = FALSE;
    u_doit(ਗਿਣਤੀ_2);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    ਸਥਿਰ ਖਾਲੀ
u_doit(ਪੂਰਨ startcount)
{
    ਪੂਰਨ ਗਿਣਤੀ_2 = startcount;

    ਜੇ (!undo_allowed())
	ਵਾਪਸ;

    u_newcount = 0;
    u_oldcount = 0;
    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
	u_oldcount = -1;
    ਜਦੋਂ (ਗਿਣਤੀ_2--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	ਜੇ (undo_undoes)
	{
	    ਜੇ (curbuf->b_u_curhead == ਨਿਰਾ)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    ਨਹੀਂਤਾਂ ਜੇ (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_ਅਗਲਾ.ptr;
	    // nothing to undo
	    ਜੇ (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == ਨਿਰਾ)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		ਜੇ (ਗਿਣਤੀ_2 == startcount - 1)
		{
		    ਜੇ (!shortmess(SHM_UNDO))
			ਸੁਨੇਹਾ(_("Already at oldest change"));
		    ਵਾਪਸ;
		}
		ਰੋਕੋ;
	    }

	    u_undoredo(TRUE);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (curbuf->b_u_curhead == ਨਿਰਾ || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		ਜੇ (ਗਿਣਤੀ_2 == startcount - 1)
		{
		    ਜੇ (!shortmess(SHM_UNDO))
			ਸੁਨੇਹਾ(_("Already at newest change"));
		    ਵਾਪਸ;
		}
		ਰੋਕੋ;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    ਜੇ (curbuf->b_u_curhead->uh_prev.ptr == ਨਿਰਾ)
		curbuf->b_u_newhead = curbuf->b_u_curhead;
	    curbuf->b_u_curhead = curbuf->b_u_curhead->uh_prev.ptr;
	}
    }
    u_undo_ਸਿਰਾ(undo_undoes, FALSE);
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
    ਖਾਲੀ
undo_ਸਮਾਂ(
    ਲੰਮਾ	ਕਦਮ,
    ਪੂਰਨ		sec,
    ਪੂਰਨ		file,
    ਪੂਰਨ		absolute)
{
    ਲੰਮਾ	    ਨਿਸ਼ਾਨਾ;
    ਲੰਮਾ	    closest;
    ਲੰਮਾ	    closest_ਸ਼ੁਰੂ;
    ਲੰਮਾ	    closest_seq = 0;
    ਲੰਮਾ	    val;
    u_ਸਿਰਲੇਖ_T	    *uhp = ਨਿਰਾ;
    u_ਸਿਰਲੇਖ_T	    *ਆਖਰੀ;
    ਪੂਰਨ		    mark;
    ਪੂਰਨ		    nomark = 0;  // shut up compiler
    ਪੂਰਨ		    ਗੇੜਾ;
    ਪੂਰਨ		    dosec = sec;
    ਪੂਰਨ		    dofile = file;
    ਪੂਰਨ		    above = FALSE;
    ਪੂਰਨ		    did_undo = TRUE;

    ਜੇ (ਪਾਠ_locked())
    {
	ਪਾਠ_locked_ਸੁਨੇਹਾ();
	ਵਾਪਸ;
    }

    // First make sure the current undoable change is synced.
    ਜੇ (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    ਜੇ (absolute)
    {
	ਨਿਸ਼ਾਨਾ = ਕਦਮ;
	closest = -1;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (dosec)
	    ਨਿਸ਼ਾਨਾ = (ਲੰਮਾ)(curbuf->b_u_ਸਮਾਂ_cur) + ਕਦਮ;
	ਨਹੀਂਤਾਂ ਜੇ (dofile)
	{
	    ਜੇ (ਕਦਮ < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		ਜੇ (uhp != ਨਿਰਾ)
		    uhp = uhp->uh_ਅਗਲਾ.ptr;
		ਨਹੀਂਤਾਂ
		    uhp = curbuf->b_u_newhead;
		ਜੇ (uhp != ਨਿਰਾ && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    ਨਿਸ਼ਾਨਾ = curbuf->b_u_save_nr_cur + ਕਦਮ;
		ਨਹੀਂਤਾਂ
		    // count the changes since the last write as one step
		    ਨਿਸ਼ਾਨਾ = curbuf->b_u_save_nr_cur + ਕਦਮ + 1;
		ਜੇ (ਨਿਸ਼ਾਨਾ <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Moving forward to a newer write.
		ਨਿਸ਼ਾਨਾ = curbuf->b_u_save_nr_cur + ਕਦਮ;
		ਜੇ (ਨਿਸ਼ਾਨਾ > curbuf->b_u_save_nr_ਆਖਰੀ)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    ਨਿਸ਼ਾਨਾ = curbuf->b_u_seq_ਆਖਰੀ + 1;
		    dofile = FALSE;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਨਿਸ਼ਾਨਾ = curbuf->b_u_seq_cur + ਕਦਮ;
	ਜੇ (ਕਦਮ < 0)
	{
	    ਜੇ (ਨਿਸ਼ਾਨਾ < 0)
		ਨਿਸ਼ਾਨਾ = 0;
	    closest = -1;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (dosec)
		closest = (ਲੰਮਾ)(vim_ਸਮਾਂ() + 1);
	    ਨਹੀਂਤਾਂ ਜੇ (dofile)
		closest = curbuf->b_u_save_nr_ਆਖਰੀ + 2;
	    ਨਹੀਂਤਾਂ
		closest = curbuf->b_u_seq_ਆਖਰੀ + 2;
	    ਜੇ (ਨਿਸ਼ਾਨਾ >= closest)
		ਨਿਸ਼ਾਨਾ = closest - 1;
	}
    }
    closest_ਸ਼ੁਰੂ = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    ਜੇ (ਨਿਸ਼ਾਨਾ == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	ਜਾਓ ਨਿਸ਼ਾਨਾ_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	ਜੇ (curbuf->b_u_curhead == ਨਿਰਾ)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	ਨਹੀਂਤਾਂ
	    uhp = curbuf->b_u_curhead;

	ਜਦੋਂ (uhp != ਨਿਰਾ)
	{
	    uhp->uh_walk = mark;
	    ਜੇ (dosec)
		val = (ਲੰਮਾ)(uhp->uh_ਸਮਾਂ);
	    ਨਹੀਂਤਾਂ ਜੇ (dofile)
		val = uhp->uh_save_nr;
	    ਨਹੀਂਤਾਂ
		val = uhp->uh_seq;

	    ਜੇ (ਗੇੜਾ == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		ਜੇ ((ਕਦਮ < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
			      : uhp->uh_seq > curbuf->b_u_seq_cur)
			&& ((dosec && val == closest)
			    ? (ਕਦਮ < 0
				? uhp->uh_seq < closest_seq
				: uhp->uh_seq > closest_seq)
			    : closest == closest_ਸ਼ੁਰੂ
				|| (val > ਨਿਸ਼ਾਨਾ
				    ? (closest > ਨਿਸ਼ਾਨਾ
					? val - ਨਿਸ਼ਾਨਾ <= closest - ਨਿਸ਼ਾਨਾ
					: val - ਨਿਸ਼ਾਨਾ <= ਨਿਸ਼ਾਨਾ - closest)
				    : (closest > ਨਿਸ਼ਾਨਾ
					? ਨਿਸ਼ਾਨਾ - val <= closest - ਨਿਸ਼ਾਨਾ
					: ਨਿਸ਼ਾਨਾ - val <= ਨਿਸ਼ਾਨਾ - closest))))
		{
		    closest = val;
		    closest_seq = uhp->uh_seq;
		}
	    }

	    // Quit searching when we found a match.  But when searching for a
	    // time we need to continue looking for the best uh_seq.
	    ਜੇ (ਨਿਸ਼ਾਨਾ == val && !dosec)
	    {
		ਨਿਸ਼ਾਨਾ = uhp->uh_seq;
		ਰੋਕੋ;
	    }

	    // go down in the tree if we haven't been there
	    ਜੇ (uhp->uh_prev.ptr != ਨਿਰਾ && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
		uhp = uhp->uh_prev.ptr;

	    // go to alternate branch if we haven't been there
	    ਨਹੀਂਤਾਂ ਜੇ (uhp->uh_alt_ਅਗਲਾ.ptr != ਨਿਰਾ
		    && uhp->uh_alt_ਅਗਲਾ.ptr->uh_walk != nomark
		    && uhp->uh_alt_ਅਗਲਾ.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_ਅਗਲਾ.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    ਨਹੀਂਤਾਂ ਜੇ (uhp->uh_ਅਗਲਾ.ptr != ਨਿਰਾ && uhp->uh_alt_prev.ptr == ਨਿਰਾ
		    && uhp->uh_ਅਗਲਾ.ptr->uh_walk != nomark
		    && uhp->uh_ਅਗਲਾ.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		ਜੇ (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_ਅਗਲਾ.ptr;
	    }

	    ਨਹੀਂਤਾਂ
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		ਜੇ (uhp->uh_alt_prev.ptr != ਨਿਰਾ)
		    uhp = uhp->uh_alt_prev.ptr;
		ਨਹੀਂਤਾਂ
		    uhp = uhp->uh_ਅਗਲਾ.ptr;
	    }
	}

	ਜੇ (uhp != ਨਿਰਾ)    // found it
	    ਰੋਕੋ;

	ਜੇ (absolute)
	{
	    semsg(_(e_undo_ਸੰਖਿਆ_nr_not_ਮਿਲਿਆ), ਕਦਮ);
	    ਵਾਪਸ;
	}

	ਜੇ (closest == closest_ਸ਼ੁਰੂ)
	{
	    ਜੇ (!shortmess(SHM_UNDO))
	    {
		ਜੇ (ਕਦਮ < 0)
		    ਸੁਨੇਹਾ(_("Already at oldest change"));
		ਨਹੀਂਤਾਂ
		    ਸੁਨੇਹਾ(_("Already at newest change"));
	    }
	    ਵਾਪਸ;
	}

	ਨਿਸ਼ਾਨਾ = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	ਜੇ (ਕਦਮ < 0)
	    above = TRUE;	// stop above the header
    }

ਨਿਸ਼ਾਨਾ_zero:
    // If we found it: Follow the path to go to where we want to be.
    ਜੇ (uhp != ਨਿਰਾ || ਨਿਸ਼ਾਨਾ == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	ਜਦੋਂ (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    ਜੇ (uhp == ਨਿਰਾ)
		uhp = curbuf->b_u_newhead;
	    ਨਹੀਂਤਾਂ
		uhp = uhp->uh_ਅਗਲਾ.ptr;
	    ਜੇ (uhp == ਨਿਰਾ || (ਨਿਸ਼ਾਨਾ > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == ਨਿਸ਼ਾਨਾ && !above))
		ਰੋਕੋ;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    ਜੇ (ਨਿਸ਼ਾਨਾ > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	ਜੇ (ਨਿਸ਼ਾਨਾ > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    ਜਦੋਂ (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		ਜੇ (uhp == ਨਿਰਾ)
		    ਰੋਕੋ;

		// Go back to the first branch with a mark.
		ਜਦੋਂ (uhp->uh_alt_prev.ptr != ਨਿਰਾ
				     && uhp->uh_alt_prev.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_prev.ptr;

		// Find the last branch with a mark, that's the one.
		ਆਖਰੀ = uhp;
		ਜਦੋਂ (ਆਖਰੀ->uh_alt_ਅਗਲਾ.ptr != ਨਿਰਾ
				    && ਆਖਰੀ->uh_alt_ਅਗਲਾ.ptr->uh_walk == mark)
		    ਆਖਰੀ = ਆਖਰੀ->uh_alt_ਅਗਲਾ.ptr;
		ਜੇ (ਆਖਰੀ != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    ਜਦੋਂ (uhp->uh_alt_prev.ptr != ਨਿਰਾ)
			uhp = uhp->uh_alt_prev.ptr;
		    ਜੇ (ਆਖਰੀ->uh_alt_ਅਗਲਾ.ptr != ਨਿਰਾ)
			ਆਖਰੀ->uh_alt_ਅਗਲਾ.ptr->uh_alt_prev.ptr =
							ਆਖਰੀ->uh_alt_prev.ptr;
		    ਆਖਰੀ->uh_alt_prev.ptr->uh_alt_ਅਗਲਾ.ptr =
							ਆਖਰੀ->uh_alt_ਅਗਲਾ.ptr;
		    ਆਖਰੀ->uh_alt_prev.ptr = ਨਿਰਾ;
		    ਆਖਰੀ->uh_alt_ਅਗਲਾ.ptr = uhp;
		    uhp->uh_alt_prev.ptr = ਆਖਰੀ;

		    ਜੇ (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = ਆਖਰੀ;
		    uhp = ਆਖਰੀ;
		    ਜੇ (uhp->uh_ਅਗਲਾ.ptr != ਨਿਰਾ)
			uhp->uh_ਅਗਲਾ.ptr->uh_prev.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		ਜੇ (uhp->uh_walk != mark)
		    ਰੋਕੋ;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		ਜੇ (uhp->uh_seq == ਨਿਸ਼ਾਨਾ && above)
		{
		    curbuf->b_u_seq_cur = ਨਿਸ਼ਾਨਾ - 1;
		    ਰੋਕੋ;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		ਜੇ (uhp->uh_prev.ptr == ਨਿਰਾ)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_prev.ptr;
		did_undo = FALSE;

		ਜੇ (uhp->uh_seq == ਨਿਸ਼ਾਨਾ)	// found it!
		    ਰੋਕੋ;

		uhp = uhp->uh_prev.ptr;
		ਜੇ (uhp == ਨਿਰਾ || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    ਅੰਦਰੂਨੀ_error("undo_time()");
		    ਰੋਕੋ;
		}
	    }
	}
    }
    u_undo_ਸਿਰਾ(did_undo, absolute);
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
    ਸਥਿਰ ਖਾਲੀ
u_undoredo(ਪੂਰਨ undo)
{
    undoline_T	*newarray = ਨਿਰਾ;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	ਸਿਖਰ, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    ਲੰਮਾ	i;
    u_ਇੰਦਰਾਜ_T	*uep, *nuep;
    u_ਇੰਦਰਾਜ_T	*newlist = ਨਿਰਾ;
    ਪੂਰਨ		old_flags;
    ਪੂਰਨ		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ;		    // buffer became empty
    u_ਸਿਰਲੇਖ_T	*curhead = curbuf->b_u_curhead;

    // Don't want autocommands using the undo structures here, they are
    // invalid till the end.
    ਖੰਡ_autocmds();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
    u_check(FALSE);
#ਜੇ_ਅੰਤ
    old_flags = curhead->uh_flags;
    new_flags = (curbuf->b_ਬਦਲਿਆ ? UH_ਬਦਲਿਆ : 0) +
	       ((curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ) ? UH_EMPTYBUF : 0);
    setpcmark();

    /*
     * save marks before undo/redo
     */
    mch_memmove(namedm, curbuf->b_namedm, ਆਕਾਰ(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_ਸ਼ੁਰੂ.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    curbuf->b_op_ਸ਼ੁਰੂ.ਖਾਨਾ = 0;
    curbuf->b_op_ਸਿਰਾ.lnum = 0;
    curbuf->b_op_ਸਿਰਾ.ਖਾਨਾ = 0;

    ਲਈ (uep = curhead->uh_ਇੰਦਰਾਜ; uep != ਨਿਰਾ; uep = nuep)
    {
	ਸਿਖਰ = uep->ue_ਸਿਖਰ;
	bot = uep->ue_bot;
	ਜੇ (bot == 0)
	    bot = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1;
	ਜੇ (ਸਿਖਰ > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ || ਸਿਖਰ >= bot
				      || bot > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_ਸਤਰ_numbers_wrong);
	    ਬਦਲਿਆ();		// don't want UNCHANGED now
	    ਵਾਪਸ;
	}

	oldsize = bot - ਸਿਖਰ - 1;    // number of lines before undo
	newsize = uep->ue_ਮਾਪ;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    ਜੇ (lnum >= ਸਿਖਰ && lnum <= ਸਿਖਰ + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਸਿਖਰ < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		ਲਈ (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(ਸਿਖਰ + 1 + i);

		    ਜੇ (curbuf->b_ml.ml_ਸਤਰ_len != uep->ue_array[i].ul_len
			    || memcmp(uep->ue_array[i].ul_ਸਤਰ, p,
						curbuf->b_ml.ml_ਸਤਰ_len) != 0)
			ਰੋਕੋ;
		}
		ਜੇ (i == newsize && newlnum == MAXLNUM && uep->ue_ਅਗਲਾ == ਨਿਰਾ)
		{
		    newlnum = ਸਿਖਰ;
		    new_curpos.lnum = newlnum + 1;
		}
		ਨਹੀਂਤਾਂ ਜੇ (i < newsize)
		{
		    newlnum = ਸਿਖਰ + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	ਜੇ (oldsize > 0)
	{
	    ਜੇ ((newarray = U_ALLOC_ਸਤਰ(ਆਕਾਰ(undoline_T) * oldsize)) == ਨਿਰਾ)
	    {
		do_outofmem_ਸੁਨੇਹਾ((long_u)(ਆਕਾਰ(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		ਜਦੋਂ (uep != ਨਿਰਾ)
		{
		    nuep = uep->ue_ਅਗਲਾ;
		    u_freeentry(uep, uep->ue_ਮਾਪ);
		    uep = nuep;
		}
		ਰੋਕੋ;
	    }
	    // delete backwards, it goes faster in most cases
	    ਲਈ (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		ਜੇ (u_save_ਸਤਰ(&newarray[i], lnum) == FAIL)
		    do_outofmem_ਸੁਨੇਹਾ((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		ਜੇ (curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ == 1)
		    ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	ਨਹੀਂਤਾਂ
	    newarray = ਨਿਰਾ;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	ਜੇ (newsize)
	{
	    ਲਈ (lnum = ਸਿਖਰ, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_ਬਫਰ && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_ਸਤਰ,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		ਨਹੀਂਤਾਂ
		    ml_append_flags(lnum, uep->ue_array[i].ul_ਸਤਰ,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_ਸਤਰ);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, ਸਿਖਰ, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, ਸਿਖਰ + newsize + 1, &uep->ue_props,
								UNDOPROP_ਹੇਠਾਂ);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, ਸਿਖਰ, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_ਹੇਠਾਂ);
	ਜੇ (newsize > 0)
	    adjust_props_for_undo(curbuf, ਸਿਖਰ + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_ਹੇਠਾਂ);
#ਜੇ_ਅੰਤ

	// adjust marks
	ਜੇ (oldsize != newsize)
	{
	    mark_adjust(ਸਿਖਰ + 1, ਸਿਖਰ + oldsize, (ਲੰਮਾ)MAXLNUM,
					       (ਲੰਮਾ)newsize - (ਲੰਮਾ)oldsize);
	    ਜੇ (curbuf->b_op_ਸ਼ੁਰੂ.lnum > ਸਿਖਰ + oldsize)
		curbuf->b_op_ਸ਼ੁਰੂ.lnum += newsize - oldsize;
	    ਜੇ (curbuf->b_op_ਸਿਰਾ.lnum > ਸਿਖਰ + oldsize)
		curbuf->b_op_ਸਿਰਾ.lnum += newsize - oldsize;
	}
	ਜੇ (oldsize > 0 || newsize > 0)
	{
	    ਬਦਲਿਆ_ਸਤਰਾਂ(ਸਿਖਰ + 1, 0, bot, newsize - oldsize);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    ਜੇ (spell_check_ਖਿੜਕੀ(curwin) && bot <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		redrawWinline(curwin, bot);
#ਜੇ_ਅੰਤ
	}

	// Set the '[ mark.
	ਜੇ (ਸਿਖਰ + 1 < curbuf->b_op_ਸ਼ੁਰੂ.lnum)
	    curbuf->b_op_ਸ਼ੁਰੂ.lnum = ਸਿਖਰ + 1;
	// Set the '] mark.
	ਜੇ (newsize == 0 && ਸਿਖਰ + 1 > curbuf->b_op_ਸਿਰਾ.lnum)
	    curbuf->b_op_ਸਿਰਾ.lnum = ਸਿਖਰ + 1;
	ਨਹੀਂਤਾਂ ਜੇ (ਸਿਖਰ + newsize > curbuf->b_op_ਸਿਰਾ.lnum)
	    curbuf->b_op_ਸਿਰਾ.lnum = ਸਿਖਰ + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_ਮਾਪ = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = ਸਿਖਰ + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_ਅਗਲਾ;
	uep->ue_ਅਗਲਾ = newlist;
	newlist = uep;
    }

    // Ensure the '[ and '] marks are within bounds.
    ਜੇ (curbuf->b_op_ਸ਼ੁਰੂ.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	 curbuf->b_op_ਸ਼ੁਰੂ.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਜੇ (curbuf->b_op_ਸਿਰਾ.lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	 curbuf->b_op_ਸਿਰਾ.lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_ਇੰਦਰਾਜ = newlist;
    curhead->uh_flags = new_flags;
    ਜੇ ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_ਖਾਲੀ_ਥਾਵਾਂ;
    ਜੇ (old_flags & UH_ਬਦਲਿਆ)
	ਬਦਲਿਆ();
    ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	ਜੇ (!isNetbeansModified(curbuf))
#ਜੇ_ਅੰਤ
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    ਲਈ (i = 0; i < NMARKS; ++i)
    {
	ਜੇ (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	ਜੇ (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	ਨਹੀਂਤਾਂ
	    curhead->uh_namedm[i].lnum = 0;
    }
    ਜੇ (curhead->uh_visual.vi_ਸ਼ੁਰੂ.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    ਜੇ (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    ਜੇ (curwin->w_cursor.lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	ਜੇ (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.ਖਾਨਾ = curhead->uh_cursor.ਖਾਨਾ;
	    ਜੇ (virtual_ਸਰਗਰਮ() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    ਨਹੀਂਤਾਂ
		curwin->w_cursor.coladd = 0;
	}
	ਨਹੀਂਤਾਂ
	    beginline(BL_SOL | BL_FIX);
    }
    ਨਹੀਂਤਾਂ
    {
	// We get here with the current cursor line being past the end (eg
	// after adding lines at the end of the file, and then undoing it).
	// check_cursor() will move the cursor to the last line.  Move it to
	// the first column here.
	curwin->w_cursor.ਖਾਨਾ = 0;
	curwin->w_cursor.coladd = 0;
    }

    // Make sure the cursor is on an existing line and column.
    check_cursor();

    // Remember where we are for "g-" and ":earlier 10s".
    curbuf->b_u_seq_cur = curhead->uh_seq;
    ਜੇ (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	ਜੇ (curhead->uh_ਅਗਲਾ.ptr != ਨਿਰਾ)
	    curbuf->b_u_seq_cur = curhead->uh_ਅਗਲਾ.ptr->uh_seq;
	ਨਹੀਂਤਾਂ
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    ਜੇ (curhead->uh_save_nr != 0)
    {
	ਜੇ (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	ਨਹੀਂਤਾਂ
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr;
    }

    // The timestamp can be the same for multiple changes, just use the one of
    // the undone/redone change.
    curbuf->b_u_ਸਮਾਂ_cur = curhead->uh_ਸਮਾਂ;

    unblock_autocmds();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
    u_check(FALSE);
#ਜੇ_ਅੰਤ
}

/*
 * If we deleted or added lines, report the number of less/more lines.
 * Otherwise, report the number of changes (this may be incorrect
 * in some cases, but it's better than nothing).
 */
    ਸਥਿਰ ਖਾਲੀ
u_undo_ਸਿਰਾ(
    ਪੂਰਨ		did_undo,	// just did an undo
    ਪੂਰਨ		absolute)	// used ":undo N"
{
    ਅੱਖਰ	*msgstr;
    u_ਸਿਰਲੇਖ_T	*uhp;
    char_u	msgbuf[80];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਜੇ ((fdo_flags & FDO_UNDO) && ਕੁੰਜੀ_Typed)
	foldOpenCursor();
#ਜੇ_ਅੰਤ

    ਜੇ (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	ਵਾਪਸ;

    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
	--u_newcount;

    u_oldcount -= u_newcount;
    ਜੇ (u_oldcount == -1)
	msgstr = N_("more line");
    ਨਹੀਂਤਾਂ ਜੇ (u_oldcount < 0)
	msgstr = N_("more lines");
    ਨਹੀਂਤਾਂ ਜੇ (u_oldcount == 1)
	msgstr = N_("line less");
    ਨਹੀਂਤਾਂ ਜੇ (u_oldcount > 1)
	msgstr = N_("fewer lines");
    ਨਹੀਂਤਾਂ
    {
	u_oldcount = u_newcount;
	ਜੇ (u_newcount == 1)
	    msgstr = N_("change");
	ਨਹੀਂਤਾਂ
	    msgstr = N_("changes");
    }

    ਜੇ (curbuf->b_u_curhead != ਨਿਰਾ)
    {
	// For ":undo N" we prefer a "after #N" message.
	ਜੇ (absolute && curbuf->b_u_curhead->uh_ਅਗਲਾ.ptr != ਨਿਰਾ)
	{
	    uhp = curbuf->b_u_curhead->uh_ਅਗਲਾ.ptr;
	    did_undo = FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (did_undo)
	    uhp = curbuf->b_u_curhead;
	ਨਹੀਂਤਾਂ
	    uhp = curbuf->b_u_curhead->uh_ਅਗਲਾ.ptr;
    }
    ਨਹੀਂਤਾਂ
	uhp = curbuf->b_u_newhead;

    ਜੇ (uhp == ਨਿਰਾ)
	*msgbuf = NUL;
    ਨਹੀਂਤਾਂ
	ਜੋੜੋ_ਸਮਾਂ(msgbuf, ਆਕਾਰ(msgbuf), uhp->uh_ਸਮਾਂ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ਜੇ (wp->w_ਬਫਰ == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#ਜੇ_ਅੰਤ
    ਜੇ (VIsual_ਸਰਗਰਮ)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == ਨਿਰਾ ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    ਖਾਲੀ
u_sync(
    ਪੂਰਨ	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    ਜੇ (curbuf->b_u_synced || (!force && no_u_sync > 0))
	ਵਾਪਸ;
#ਜੇ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ਜੇ (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	ਵਾਪਸ;		    // XIM is busy, don't break an undo sequence
#ਜੇ_ਅੰਤ
    ਜੇ (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    ਨਹੀਂਤਾਂ
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = ਨਿਰਾ;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    ਖਾਲੀ
ex_undolist(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    garray_T	ga;
    u_ਸਿਰਲੇਖ_T	*uhp;
    ਪੂਰਨ		mark;
    ਪੂਰਨ		nomark;
    ਪੂਰਨ		changes = 1;
    ਪੂਰਨ		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ *), 20);

    uhp = curbuf->b_u_oldhead;
    ਜਦੋਂ (uhp != ਨਿਰਾ)
    {
	ਜੇ (uhp->uh_prev.ptr == ਨਿਰਾ && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    ਜੇ (ga_grow(&ga, 1) == FAIL)
		ਰੋਕੋ;
	    len = vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    ਜੋੜੋ_ਸਮਾਂ(IObuff + len, IOSIZE - len, uhp->uh_ਸਮਾਂ);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (ਪੂਰਨ)STRLEN(IObuff + len);
	    ਜੇ (uhp->uh_save_nr > 0)
	    {
		ਪੂਰਨ n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((ਅੱਖਰ *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_ਅੰਕੜੇ))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	ਜੇ (uhp->uh_prev.ptr != ਨਿਰਾ && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_prev.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	ਨਹੀਂਤਾਂ ਜੇ (uhp->uh_alt_ਅਗਲਾ.ptr != ਨਿਰਾ
		&& uhp->uh_alt_ਅਗਲਾ.ptr->uh_walk != nomark
		&& uhp->uh_alt_ਅਗਲਾ.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_ਅਗਲਾ.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	ਨਹੀਂਤਾਂ ਜੇ (uhp->uh_ਅਗਲਾ.ptr != ਨਿਰਾ && uhp->uh_alt_prev.ptr == ਨਿਰਾ
		&& uhp->uh_ਅਗਲਾ.ptr->uh_walk != nomark
		&& uhp->uh_ਅਗਲਾ.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_ਅਗਲਾ.ptr;
	    --changes;
	}

	ਨਹੀਂਤਾਂ
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    ਜੇ (uhp->uh_alt_prev.ptr != ਨਿਰਾ)
		uhp = uhp->uh_alt_prev.ptr;
	    ਨਹੀਂਤਾਂ
	    {
		uhp = uhp->uh_ਅਗਲਾ.ptr;
		--changes;
	    }
	}
    }

    ਜੇ (ga.ga_len == 0)
	ਸੁਨੇਹਾ(_("Nothing to undo"));
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ		i;

	ਛਾਂਟੋ_ਲੜੀਆਂ((char_u **)ga.ga_ਅੰਕੜੇ, ga.ga_len);

	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	ਸੁਨੇਹਾ_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	ਲਈ (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    ਸੁਨੇਹਾ_putchar('\n');
	    ਜੇ (got_int)
		ਰੋਕੋ;
	    ਸੁਨੇਹਾ_puts(((ਅੱਖਰ **)ga.ga_ਅੰਕੜੇ)[i]);
	}
	ਸੁਨੇਹਾ_ਸਿਰਾ();

	ga_clear_ਲੜੀਆਂ(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    ਖਾਲੀ
ex_undojoin(exarg_T *eap ਅਣਵਰਤਿਆ)
{
    ਜੇ (curbuf->b_u_newhead == ਨਿਰਾ)
	ਵਾਪਸ;		    // nothing changed before
    ਜੇ (curbuf->b_u_curhead != ਨਿਰਾ)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	ਵਾਪਸ;
    }
    ਜੇ (!curbuf->b_u_synced)
	ਵਾਪਸ;		    // already unsynced
    ਜੇ (get_undolevel() < 0)
	ਵਾਪਸ;		    // no entries, nothing to do
    ਨਹੀਂਤਾਂ
	// Append next change to the last entry
	curbuf->b_u_synced = false;
}

/*
 * Called after writing or reloading the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
    ਖਾਲੀ
u_unchanged(buf_T *buf)
{
    u_unch_branch(buf->b_u_oldhead);
    buf->b_did_warn = false;
}

/*
 * After reloading a buffer which was saved for 'undoreload': Find the first
 * line that was changed and set the cursor there.
 */
    ਖਾਲੀ
u_ਲੱਭੋ_ਪਹਿਲਾ_ਬਦਲਿਆ(ਖਾਲੀ)
{
    u_ਸਿਰਲੇਖ_T	*uhp = curbuf->b_u_newhead;
    u_ਇੰਦਰਾਜ_T   *uep;
    linenr_T	lnum;

    ਜੇ (curbuf->b_u_curhead != ਨਿਰਾ || uhp == ਨਿਰਾ)
	ਵਾਪਸ;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_ਇੰਦਰਾਜ;
    ਜੇ (uep->ue_ਸਿਖਰ != 0 || uep->ue_bot != 0)
	ਵਾਪਸ;

    ਲਈ (lnum = 1; lnum < curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
					      && lnum <= uep->ue_ਮਾਪ; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	ਜੇ (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_ਸਤਰ_len
		|| memcmp(p, uep->ue_array[lnum - 1].ul_ਸਤਰ, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    ਵਾਪਸ;
	}
    }
    ਜੇ (curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ != uep->ue_ਮਾਪ)
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
    ਖਾਲੀ
u_update_save_nr(buf_T *buf)
{
    u_ਸਿਰਲੇਖ_T	*uhp;

    ++buf->b_u_save_nr_ਆਖਰੀ;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_ਆਖਰੀ;
    uhp = buf->b_u_curhead;
    ਜੇ (uhp != ਨਿਰਾ)
	uhp = uhp->uh_ਅਗਲਾ.ptr;
    ਨਹੀਂਤਾਂ
	uhp = buf->b_u_newhead;
    ਜੇ (uhp != ਨਿਰਾ)
	uhp->uh_save_nr = buf->b_u_save_nr_ਆਖਰੀ;
}

    ਸਥਿਰ ਖਾਲੀ
u_unch_branch(u_ਸਿਰਲੇਖ_T *uhp)
{
    u_ਸਿਰਲੇਖ_T	*uh;

    ਲਈ (uh = uhp; uh != ਨਿਰਾ; uh = uh->uh_prev.ptr)
    {
	uh->uh_flags |= UH_ਬਦਲਿਆ;
	ਜੇ (uh->uh_alt_ਅਗਲਾ.ptr != ਨਿਰਾ)
	    u_unch_branch(uh->uh_alt_ਅਗਲਾ.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    ਸਥਿਰ u_ਇੰਦਰਾਜ_T *
u_get_headentry(ਖਾਲੀ)
{
    ਜੇ (curbuf->b_u_newhead == ਨਿਰਾ || curbuf->b_u_newhead->uh_ਇੰਦਰਾਜ == ਨਿਰਾ)
    {
	iemsg(e_undo_list_corrupt);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ curbuf->b_u_newhead->uh_ਇੰਦਰਾਜ;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    ਸਥਿਰ ਖਾਲੀ
u_getbot(ਖਾਲੀ)
{
    u_ਇੰਦਰਾਜ_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    ਜੇ (uep == ਨਿਰਾ)
	ਵਾਪਸ;

    uep = curbuf->b_u_newhead->uh_getbot_ਇੰਦਰਾਜ;
    ਜੇ (uep != ਨਿਰਾ)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - uep->ue_lcount;
	uep->ue_bot = uep->ue_ਸਿਖਰ + uep->ue_ਮਾਪ + 1 + extra;
	ਜੇ (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    iemsg(e_undo_ਸਤਰ_missing);
	    uep->ue_bot = uep->ue_ਸਿਖਰ + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_ਇੰਦਰਾਜ = ਨਿਰਾ;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    ਸਥਿਰ ਖਾਲੀ
u_freeheader(
    buf_T	    *buf,
    u_ਸਿਰਲੇਖ_T	    *uhp,
    u_ਸਿਰਲੇਖ_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_ਸਿਰਲੇਖ_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    ਜੇ (uhp->uh_alt_ਅਗਲਾ.ptr != ਨਿਰਾ)
	u_freebranch(buf, uhp->uh_alt_ਅਗਲਾ.ptr, uhpp);

    ਜੇ (uhp->uh_alt_prev.ptr != ਨਿਰਾ)
	uhp->uh_alt_prev.ptr->uh_alt_ਅਗਲਾ.ptr = ਨਿਰਾ;

    // Update the links in the list to remove the header.
    ਜੇ (uhp->uh_ਅਗਲਾ.ptr == ਨਿਰਾ)
	buf->b_u_oldhead = uhp->uh_prev.ptr;
    ਨਹੀਂਤਾਂ
	uhp->uh_ਅਗਲਾ.ptr->uh_prev.ptr = uhp->uh_prev.ptr;

    ਜੇ (uhp->uh_prev.ptr == ਨਿਰਾ)
	buf->b_u_newhead = uhp->uh_ਅਗਲਾ.ptr;
    ਨਹੀਂਤਾਂ
	ਲਈ (uhap = uhp->uh_prev.ptr; uhap != ਨਿਰਾ;
						 uhap = uhap->uh_alt_ਅਗਲਾ.ptr)
	    uhap->uh_ਅਗਲਾ.ptr = uhp->uh_ਅਗਲਾ.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    ਸਥਿਰ ਖਾਲੀ
u_freebranch(
    buf_T	    *buf,
    u_ਸਿਰਲੇਖ_T	    *uhp,
    u_ਸਿਰਲੇਖ_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_ਸਿਰਲੇਖ_T	    *tofree, *ਅਗਲਾ;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    ਜੇ (uhp == buf->b_u_oldhead)
    {
	ਜਦੋਂ (buf->b_u_oldhead != ਨਿਰਾ)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	ਵਾਪਸ;
    }

    ਜੇ (uhp->uh_alt_prev.ptr != ਨਿਰਾ)
	uhp->uh_alt_prev.ptr->uh_alt_ਅਗਲਾ.ptr = ਨਿਰਾ;

    ਅਗਲਾ = uhp;
    ਜਦੋਂ (ਅਗਲਾ != ਨਿਰਾ)
    {
	tofree = ਅਗਲਾ;
	ਜੇ (tofree->uh_alt_ਅਗਲਾ.ptr != ਨਿਰਾ)
	    u_freebranch(buf, tofree->uh_alt_ਅਗਲਾ.ptr, uhpp);   // recursive
	ਅਗਲਾ = tofree->uh_prev.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}

/*
 * Free all the undo entries for one header and the header itself.
 * This means that "uhp" is invalid when returning.
 */
    ਸਥਿਰ ਖਾਲੀ
u_freeentries(
    buf_T	    *buf,
    u_ਸਿਰਲੇਖ_T	    *uhp,
    u_ਸਿਰਲੇਖ_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_ਇੰਦਰਾਜ_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    ਜੇ (buf->b_u_curhead == uhp)
	buf->b_u_curhead = ਨਿਰਾ;
    ਜੇ (buf->b_u_newhead == uhp)
	buf->b_u_newhead = ਨਿਰਾ;  // freeing the newest entry
    ਜੇ (uhpp != ਨਿਰਾ && uhp == *uhpp)
	*uhpp = ਨਿਰਾ;

    ਲਈ (uep = uhp->uh_ਇੰਦਰਾਜ; uep != ਨਿਰਾ; uep = nuep)
    {
	nuep = uep->ue_ਅਗਲਾ;
	u_freeentry(uep, uep->ue_ਮਾਪ);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
    uhp->uh_magic = 0;
#ਜੇ_ਅੰਤ
    vim_free((char_u *)uhp);
    --buf->b_u_numhead;
}

/*
 * free entry 'uep' and 'n' lines in uep->ue_array[]
 */
    ਸਥਿਰ ਖਾਲੀ
u_freeentry(u_ਇੰਦਰਾਜ_T *uep, ਲੰਮਾ n)
{
    ਜਦੋਂ (n > 0)
	vim_free(uep->ue_array[--n].ul_ਸਤਰ);
    vim_free((char_u *)uep->ue_array);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ga_clear(&uep->ue_props);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_DEBUG
    uep->ue_magic = 0;
#ਜੇ_ਅੰਤ
    vim_free((char_u *)uep);
}

/*
 * invalidate the undo buffer; called when storage has already been released
 */
    ਸਥਿਰ ਖਾਲੀ
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = ਨਿਰਾ;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_ਸਤਰ_ptr.ul_ਸਤਰ = ਨਿਰਾ;
    buf->b_u_ਸਤਰ_ptr.ul_len = 0;
    buf->b_u_ਸਤਰ_ptr.ul_textlen = 0;
    buf->b_u_ਸਤਰ_lnum = 0;
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 */
    ਸਥਿਰ ਖਾਲੀ
u_blockfree(buf_T *buf)
{
    ਜਦੋਂ (buf->b_u_oldhead != ਨਿਰਾ)
	u_freeheader(buf, buf->b_u_oldhead, ਨਿਰਾ);
    vim_free(buf->b_u_ਸਤਰ_ptr.ul_ਸਤਰ);
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 * and invalidate the undo buffer
 */
    ਖਾਲੀ
u_clearallandblockfree(buf_T *buf)
{
    u_blockfree(buf);
    u_clearall(buf);
}



/*
 * Save the line "lnum" for the "U" command.
 */
    ਸਥਿਰ ਖਾਲੀ
u_saveline(linenr_T lnum)
{
    ਜੇ (lnum == curbuf->b_u_ਸਤਰ_lnum)	    // line is already saved
	ਵਾਪਸ;
    ਜੇ (lnum < 1 || lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ) // should never happen
	ਵਾਪਸ;
    u_clearline();
    curbuf->b_u_ਸਤਰ_lnum = lnum;
    ਜੇ (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_ਸਤਰ_colnr = curwin->w_cursor.ਖਾਨਾ;
    ਨਹੀਂਤਾਂ
	curbuf->b_u_ਸਤਰ_colnr = 0;
    ਜੇ (u_save_ਸਤਰ(&curbuf->b_u_ਸਤਰ_ptr, lnum) == FAIL)
	do_outofmem_ਸੁਨੇਹਾ((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    ਖਾਲੀ
u_clearline(ਖਾਲੀ)
{
    ਜੇ (curbuf->b_u_ਸਤਰ_ptr.ul_ਸਤਰ == ਨਿਰਾ)
	ਵਾਪਸ;

    VIM_CLEAR(curbuf->b_u_ਸਤਰ_ptr.ul_ਸਤਰ);
    curbuf->b_u_ਸਤਰ_ptr.ul_len = 0;
    curbuf->b_u_ਸਤਰ_ptr.ul_textlen = 0;
    curbuf->b_u_ਸਤਰ_lnum = 0;
}

/*
 * Implementation of the "U" command.
 * Differentiation from vi: "U" can be undone with the next "U".
 * We also allow the cursor to be in another line.
 * Careful: may trigger autocommands that reload the buffer.
 */
    ਖਾਲੀ
u_undoline(ਖਾਲੀ)
{
    colnr_T	t;
    undoline_T  oldp;

    ਜੇ (undo_off)
	ਵਾਪਸ;

    ਜੇ (curbuf->b_u_ਸਤਰ_ptr.ul_ਸਤਰ == ਨਿਰਾ
			|| curbuf->b_u_ਸਤਰ_lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	beep_flush();
	ਵਾਪਸ;
    }

    // first save the line for the 'u' command
    ਜੇ (u_savecommon(curbuf->b_u_ਸਤਰ_lnum - 1,
		       curbuf->b_u_ਸਤਰ_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	ਵਾਪਸ;
    ਜੇ (u_save_ਸਤਰ(&oldp, curbuf->b_u_ਸਤਰ_lnum) == FAIL)
    {
	do_outofmem_ਸੁਨੇਹਾ((long_u)0);
	ਵਾਪਸ;
    }
    ml_replace_len(curbuf->b_u_ਸਤਰ_lnum, curbuf->b_u_ਸਤਰ_ptr.ul_ਸਤਰ,
				     curbuf->b_u_ਸਤਰ_ptr.ul_len, TRUE, FALSE);
    ਬਦਲਿਆ_ਬਾਈਟਾਂ(curbuf->b_u_ਸਤਰ_lnum, 0);
    curbuf->b_u_ਸਤਰ_ptr = oldp;

    t = curbuf->b_u_ਸਤਰ_colnr;
    ਜੇ (curwin->w_cursor.lnum == curbuf->b_u_ਸਤਰ_lnum)
	curbuf->b_u_ਸਤਰ_colnr = curwin->w_cursor.ਖਾਨਾ;
    curwin->w_cursor.ਖਾਨਾ = t;
    curwin->w_cursor.lnum = curbuf->b_u_ਸਤਰ_lnum;
    check_cursor_ਖਾਨਾ();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    ਪੂਰਨ
buf_Is_ਬਦਲਿਆ(buf_T *buf)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (ਪਦ_ਕੰਮ_running_not_none(buf->b_ਪਦ))
	ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ buf_Is_ਬਦਲਿਆ_Not_ਪਦ(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    ਪੂਰਨ
any_Buf_Is_ਬਦਲਿਆ(ਖਾਲੀ)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	ਜੇ (buf_Is_ਬਦਲਿਆ(buf))
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    ਪੂਰਨ
buf_Is_ਬਦਲਿਆ_Not_ਪਦ(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    ਵਾਪਸ (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_ਬਦਲਿਆ || file_ff_differs(buf, TRUE));
}

    ਪੂਰਨ
curbuf_Is_ਬਦਲਿਆ(ਖਾਲੀ)
{
    ਵਾਪਸ buf_Is_ਬਦਲਿਆ(curbuf);
}

#ਜੇ defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    ਸਥਿਰ ਖਾਲੀ
u_eval_ਰੁੱਖ(buf_T *buf, u_ਸਿਰਲੇਖ_T *ਪਹਿਲਾ_uhp, list_T *list)
{
    u_ਸਿਰਲੇਖ_T  *uhp = ਪਹਿਲਾ_uhp;
    dict_T	*dict;

    ਜਦੋਂ (uhp != ਨਿਰਾ)
    {
	dict = dict_alloc();
	ਜੇ (dict == ਨਿਰਾ)
	    ਵਾਪਸ;
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "seq", uhp->uh_seq);
	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "time", (ਲੰਮਾ)uhp->uh_ਸਮਾਂ);
	ਜੇ (uhp == buf->b_u_newhead)
	    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "newhead", 1);
	ਜੇ (uhp == buf->b_u_curhead)
	    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "curhead", 1);
	ਜੇ (uhp->uh_save_nr > 0)
	    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "save", uhp->uh_save_nr);

	ਜੇ (uhp->uh_alt_ਅਗਲਾ.ptr != ਨਿਰਾ)
	{
	    list_T	*alt_list = list_alloc();

	    ਜੇ (alt_list != ਨਿਰਾ)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_ਰੁੱਖ(buf, uhp->uh_alt_ਅਗਲਾ.ptr, alt_list);
		dict_ਜੋੜੋ_list(dict, "alt", alt_list);
	    }
	}

	list_append_dict(list, dict);
	uhp = uhp->uh_prev.ptr;
    }
}

/*
 * "undofile(name)" function
 */
    ਖਾਲੀ
f_undofile(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	ਜੇ (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u *ffname = ਭਰਿਆ_Name_save(fname, TRUE);

	    ਜੇ (ffname != ਨਿਰਾ)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# ਨਹੀਂਤਾਂ
    rettv->vval.v_string = ਨਿਰਾ;
# endif
}
# ifdef FEAT_PERSISTENT_UNDO
/*
 * Reset undofile option and delete the undofile
 */
    ਖਾਲੀ
u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    ਜੇ (!buf->b_p_udf)
	ਵਾਪਸ;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    ਜੇ (file_name != ਨਿਰਾ)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"undofile", 0L, ਨਿਰਾ, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    ਖਾਲੀ
f_undotree(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    dict_T *dict = rettv->vval.v_dict;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "synced", (ਲੰਮਾ)buf->b_u_synced);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "seq_last", buf->b_u_seq_ਆਖਰੀ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "save_last", buf->b_u_save_nr_ਆਖਰੀ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "seq_cur", buf->b_u_seq_cur);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "time_cur", (ਲੰਮਾ)buf->b_u_ਸਮਾਂ_cur);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    ਜੇ (list != ਨਿਰਾ)
    {
	u_eval_ਰੁੱਖ(buf, buf->b_u_oldhead, list);
	dict_ਜੋੜੋ_list(dict, "entries", list);
    }
}

#ਜੇ_ਅੰਤ
