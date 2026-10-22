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
#నిర్వచించు UH_MAGIC 0x18dade	// value for uh_magic when in use
#నిర్వచించు UE_MAGIC 0xabc123	// value for ue_magic when in use

// Size of buffer used for encryption.
#నిర్వచించు CRYPT_BUF_కొలత 8192

#అన్తర్భావ "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
రకనిర్వచనం నిర్మాణం {
    buf_T	*bi_buf;
    FILE	*bi_fp;
#నిర్వచితమైతే FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_బఫరు; // CRYPT_BUF_SIZE, NULL when not buffering
    size_t	bi_వాడిన;    // bytes written to/read from bi_buffer
    size_t	bi_avail;   // bytes available in bi_buffer
#అయితే_ముగింపు
} bufinfo_T;


స్థిర శూన్య u_unch_branch(u_శీర్షిక_T *uhp);
స్థిర u_నమోదు_T *u_get_headentry(శూన్య);
స్థిర శూన్య u_getbot(శూన్య);
స్థిర శూన్య u_doit(పూర్ణాంకం లెక్క);
స్థిర శూన్య u_undoredo(పూర్ణాంకం undo);
స్థిర శూన్య u_undo_చివర(పూర్ణాంకం did_undo, పూర్ణాంకం absolute);
స్థిర శూన్య u_freeheader(buf_T *buf, u_శీర్షిక_T *uhp, u_శీర్షిక_T **uhpp);
స్థిర శూన్య u_freebranch(buf_T *buf, u_శీర్షిక_T *uhp, u_శీర్షిక_T **uhpp);
స్థిర శూన్య u_freeentries(buf_T *buf, u_శీర్షిక_T *uhp, u_శీర్షిక_T **uhpp);
స్థిర శూన్య u_freeentry(u_నమోదు_T *, దీర్ఘ);
#నిర్వచితమైతే FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
స్థిర పూర్ణాంకం undo_flush(bufinfo_T *bi);
# endif
స్థిర పూర్ణాంకం undo_read(bufinfo_T *bi, char_u *బఫరు, size_t కొలత);
స్థిర పూర్ణాంకం serialize_uep(bufinfo_T *bi, u_నమోదు_T *uep);
స్థిర u_నమోదు_T *unserialize_uep(bufinfo_T *bi, పూర్ణాంకం *error, char_u *file_name);
స్థిర శూన్య serialize_pos(bufinfo_T *bi, pos_T pos);
స్థిర శూన్య unserialize_pos(bufinfo_T *bi, pos_T *pos);
స్థిర శూన్య serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
స్థిర శూన్య unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#అయితే_ముగింపు
స్థిర శూన్య u_saveline(linenr_T lnum);
స్థిర శూన్య u_blockfree(buf_T *buf);

#నిర్వచించు U_ALLOC_పంక్తి(కొలత) lalloc(కొలత, FALSE)

// used in undo_end() to report number of added and deleted lines
స్థిర దీర్ఘ	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
స్థిర పూర్ణాంకం	undo_undoes = FALSE;

స్థిర పూర్ణాంకం	lastmark = 0;

#ఒకవేళ defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
స్థిర పూర్ణాంకం చూసిన_b_u_curhead;
స్థిర పూర్ణాంకం చూసిన_b_u_newhead;
స్థిర పూర్ణాంకం శీర్షిక_లెక్క;

    స్థిర శూన్య
u_check_వృక్షం(u_శీర్షిక_T *uhp,
	u_శీర్షిక_T *exp_uh_తదుపరి,
	u_శీర్షిక_T *exp_uh_alt_prev)
{
    u_నమోదు_T *uep;

    ఒకవేళ (uhp == ఖాళీ)
	తిరిగి;
    ++శీర్షిక_లెక్క;
    ఒకవేళ (uhp == curbuf->b_u_curhead && ++చూసిన_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	తిరిగి;
    }
    ఒకవేళ (uhp == curbuf->b_u_newhead && ++చూసిన_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	తిరిగి;
    }

    ఒకవేళ (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    లేకపోతే
    {
	// Check pointers back are correct.
	ఒకవేళ (uhp->uh_తదుపరి.ptr != exp_uh_తదుపరి)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_తదుపరి, uhp->uh_తదుపరి.ptr);
	}
	ఒకవేళ (uhp->uh_alt_prev.ptr != exp_uh_alt_prev)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_prev, uhp->uh_alt_prev.ptr);
	}

	// Check the undo tree at this header.
	కొరకు (uep = uhp->uh_నమోదు; uep != ఖాళీ; uep = uep->ue_తదుపరి)
	{
	    ఒకవేళ (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		ఆపు;
	    }
	}

	// Check the next alt tree.
	u_check_వృక్షం(uhp->uh_alt_తదుపరి.ptr, uhp->uh_తదుపరి.ptr, uhp);

	// Check the next header in this branch.
	u_check_వృక్షం(uhp->uh_prev.ptr, uhp, ఖాళీ);
    }
}

    స్థిర శూన్య
u_check(పూర్ణాంకం newhead_may_be_NULL)
{
    చూసిన_b_u_newhead = 0;
    చూసిన_b_u_curhead = 0;
    శీర్షిక_లెక్క = 0;

    u_check_వృక్షం(curbuf->b_u_oldhead, ఖాళీ, ఖాళీ);

    ఒకవేళ (చూసిన_b_u_newhead == 0 && curbuf->b_u_oldhead != ఖాళీ
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == ఖాళీ))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    ఒకవేళ (curbuf->b_u_curhead != ఖాళీ && చూసిన_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    ఒకవేళ (శీర్షిక_లెక్క != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (దీర్ఘ)శీర్షిక_లెక్క, (దీర్ఘ)curbuf->b_u_numhead);
    }
}
#అయితే_ముగింపు

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
u_save_cursor(శూన్య)
{
    తిరిగి (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    పూర్ణాంకం
u_save(linenr_T శిఖరం, linenr_T bot)
{
    ఒకవేళ (undo_off)
	తిరిగి OK;

    ఒకవేళ (శిఖరం >= bot || bot > curbuf->b_ml.ml_పంక్తి_లెక్క + 1)
	తిరిగి FAIL;	// rely on caller to give an error message

    ఒకవేళ (శిఖరం + 2 == bot)
	u_saveline((linenr_T)(శిఖరం + 1));

    తిరిగి (u_savecommon(శిఖరం, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    పూర్ణాంకం
u_savesub(linenr_T lnum)
{
    ఒకవేళ (undo_off)
	తిరిగి OK;

    తిరిగి (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    పూర్ణాంకం
u_inssub(linenr_T lnum)
{
    ఒకవేళ (undo_off)
	తిరిగి OK;

    తిరిగి (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    పూర్ణాంకం
u_savedel(linenr_T lnum, దీర్ఘ nlines)
{
    ఒకవేళ (undo_off)
	తిరిగి OK;

    తిరిగి (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_పంక్తి_లెక్క ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    పూర్ణాంకం
undo_allowed(శూన్య)
{
    // Don't allow changes when 'modifiable' is off.
    ఒకవేళ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	తిరిగి FALSE;
    }

#నిర్వచితమైతే HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    ఒకవేళ (sandbox != 0)
    {
	emsg(_(e_not_allowed_లోపల_sandbox));
	తిరిగి FALSE;
    }
#అయితే_ముగింపు

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    ఒకవేళ (textlock != 0)
    {
	emsg(_(e_not_allowed_గమ్యం_change_పాఠ్యం_or_change_కిటికీ));
	తిరిగి FALSE;
    }

    తిరిగి TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    స్థిర దీర్ఘ
get_undolevel(శూన్య)
{
    ఒకవేళ (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	తిరిగి p_ul;
    తిరిగి curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    స్థిర పూర్ణాంకం
u_save_పంక్తి(undoline_T *ul, linenr_T lnum)
{
    char_u *పంక్తి = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    ఒకవేళ (curbuf->b_ml.ml_పంక్తి_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_పంక్తి = vim_strsave((char_u *)"");
    }
    లేకపోతే
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_పంక్తి_len;
	ul->ul_పంక్తి = vim_memsave(పంక్తి, ul->ul_len);
    }
    తిరిగి ul->ul_పంక్తి == ఖాళీ ? FAIL : OK;
}

#నిర్వచితమైతే FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    స్థిర పూర్ణాంకం
has_prop_w_flags(linenr_T lnum, పూర్ణాంకం flags)
{
    char_u  *props;
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    proplen = get_పాఠ్యం_props(curbuf, lnum, &props, FALSE);

    కొరకు (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * పరిమాణం prop, పరిమాణం prop);
	ఒకవేళ (prop.tp_flags & flags)
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Common code for various ways to save text before a change.
 * "top" is the line above the first changed line.
 * "bot" is the line below the last changed line.
 * "newbot" is the new bottom line.  Use zero when not known.
 * "reload" is TRUE when saving for a buffer reload.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    పూర్ణాంకం
u_savecommon(
    linenr_T	శిఖరం,
    linenr_T	bot,
    linenr_T	newbot,
    పూర్ణాంకం		reload)
{
    linenr_T	lnum;
    దీర్ఘ	i;
    u_శీర్షిక_T	*uhp;
    u_శీర్షిక_T	*old_curhead;
    u_నమోదు_T	*uep;
    u_నమోదు_T	*prev_uep;
    దీర్ఘ	కొలత;

    ఒకవేళ (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	ఒకవేళ (!undo_allowed())
	    తిరిగి FAIL;

#నిర్వచితమైతే FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	ఒకవేళ (netbeans_చురుకైన())
	{
	    ఒకవేళ (netbeans_is_guarded(శిఖరం, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		తిరిగి FAIL;
	    }
	    ఒకవేళ (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_లోపల_read_only_files));
		తిరిగి FAIL;
	    }
	}
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
	// A change in a terminal buffer removes the highlighting.
	పదం_change_లోపల_curbuf();
#అయితే_ముగింపు

	/*
	 * Saving text for undo means we are going to make a change.  Give a
	 * warning for a read-only file before making the change, so that the
	 * FileChangedRO event can replace the buffer with a read-write version
	 * (e.g., obtained from a source control system).
	 */
	change_warning(0);
	ఒకవేళ (bot > curbuf->b_ml.ml_పంక్తి_లెక్క + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_పంక్తి_లెక్క_మారింది_unexpectedly));
	    తిరిగి FAIL;
	}
    }

#నిర్వచితమైతే U_DEBUG
    u_check(FALSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    ఒకవేళ (bot - శిఖరం > 1)
    {
	ఒకవేళ (శిఖరం > 0 && has_prop_w_flags(శిఖరం + 1, TP_FLAG_CONT_PREV))
	    --శిఖరం;
	ఒకవేళ (bot <= curbuf->b_ml.ml_పంక్తి_లెక్క
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_తదుపరి))
	{
	    ++bot;
	    ఒకవేళ (newbot != 0)
		++newbot;
	}
    }
#అయితే_ముగింపు

    కొలత = bot - శిఖరం - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    ఒకవేళ (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	ఒకవేళ (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_పంక్తి(పరిమాణం(u_శీర్షిక_T));
	    ఒకవేళ (uhp == ఖాళీ)
		వెళ్ళు nomem;
#నిర్వచితమైతే U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#అయితే_ముగింపు
	}
	లేకపోతే
	    uhp = ఖాళీ;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	ఒకవేళ (old_curhead != ఖాళీ)
	{
	    curbuf->b_u_newhead = old_curhead->uh_తదుపరి.ptr;
	    curbuf->b_u_curhead = ఖాళీ;
	}

	/*
	 * free headers to keep the size right
	 */
	అయితే (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != ఖాళీ)
	{
	    u_శీర్షిక_T	    *uhfree = curbuf->b_u_oldhead;

	    ఒకవేళ (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    లేకపోతే ఒకవేళ (uhfree->uh_alt_తదుపరి.ptr == ఖాళీ)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    లేకపోతే
	    {
		// Free the oldest alternate branch as a whole.
		అయితే (uhfree->uh_alt_తదుపరి.ptr != ఖాళీ)
		    uhfree = uhfree->uh_alt_తదుపరి.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#నిర్వచితమైతే U_DEBUG
	    u_check(TRUE);
#అయితే_ముగింపు
	}

	ఒకవేళ (uhp == ఖాళీ)		// no undo at all
	{
	    ఒకవేళ (old_curhead != ఖాళీ)
		u_freebranch(curbuf, old_curhead, ఖాళీ);
	    curbuf->b_u_synced = false;
	    తిరిగి OK;
	}

	uhp->uh_prev.ptr = ఖాళీ;
	uhp->uh_తదుపరి.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_తదుపరి.ptr = old_curhead;
	ఒకవేళ (old_curhead != ఖాళీ)
	{
	    uhp->uh_alt_prev.ptr = old_curhead->uh_alt_prev.ptr;
	    ఒకవేళ (uhp->uh_alt_prev.ptr != ఖాళీ)
		uhp->uh_alt_prev.ptr->uh_alt_తదుపరి.ptr = uhp;
	    old_curhead->uh_alt_prev.ptr = uhp;
	    ఒకవేళ (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	లేకపోతే
	    uhp->uh_alt_prev.ptr = ఖాళీ;
	ఒకవేళ (curbuf->b_u_newhead != ఖాళీ)
	    curbuf->b_u_newhead->uh_prev.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_చివరి;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_సమయం = vim_సమయం();
	uhp->uh_save_nr = 0;
	curbuf->b_u_సమయం_cur = uhp->uh_సమయం + 1;

	uhp->uh_walk = 0;
	uhp->uh_నమోదు = ఖాళీ;
	uhp->uh_getbot_నమోదు = ఖాళీ;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	ఒకవేళ (virtual_చురుకైన() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	లేకపోతే
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_మారింది ? UH_మారింది : 0) +
		       ((curbuf->b_ml.ml_flags & ML_ఖాళీలు) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, పరిమాణం(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	ఒకవేళ (curbuf->b_u_oldhead == ఖాళీ)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    లేకపోతే
    {
	ఒకవేళ (get_undolevel() < 0)	// no undo at all
	    తిరిగి OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	ఒకవేళ (కొలత == 1)
	{
	    uep = u_get_headentry();
	    prev_uep = ఖాళీ;
	    కొరకు (i = 0; i < 10; ++i)
	    {
		ఒకవేళ (uep == ఖాళీ)
		    ఆపు;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		ఒకవేళ ((curbuf->b_u_newhead->uh_getbot_నమోదు != uep
			    ? (uep->ue_శిఖరం + uep->ue_కొలత + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_పంక్తి_లెక్క + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_పంక్తి_లెక్క)
			|| (uep->ue_కొలత > 1
			    && శిఖరం >= uep->ue_శిఖరం
			    && శిఖరం + 2 <= uep->ue_శిఖరం + uep->ue_కొలత + 1))
		    ఆపు;

		// If it's the same line we can skip saving it again.
		ఒకవేళ (uep->ue_కొలత == 1 && uep->ue_శిఖరం == శిఖరం)
		{
		    ఒకవేళ (i > 0)
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
			prev_uep->ue_తదుపరి = uep->ue_తదుపరి;
			uep->ue_తదుపరి = curbuf->b_u_newhead->uh_నమోదు;
			curbuf->b_u_newhead->uh_నమోదు = uep;
		    }

		    // The executed command may change the line count.
		    ఒకవేళ (newbot != 0)
			uep->ue_bot = newbot;
		    లేకపోతే ఒకవేళ (bot > curbuf->b_ml.ml_పంక్తి_లెక్క)
			uep->ue_bot = 0;
		    లేకపోతే
		    {
			uep->ue_lcount = curbuf->b_ml.ml_పంక్తి_లెక్క;
			curbuf->b_u_newhead->uh_getbot_నమోదు = uep;
		    }
		    తిరిగి OK;
		}
		prev_uep = uep;
		uep = uep->ue_తదుపరి;
	    }
	}

	// find line number for ue_bot for previous u_save()
	u_getbot();
    }

    /*
     * add lines in front of entry list
     */
    uep = U_ALLOC_పంక్తి(పరిమాణం(u_నమోదు_T));
    ఒకవేళ (uep == ఖాళీ)
	వెళ్ళు nomem;
    CLEAR_సూచిక(uep);
#నిర్వచితమైతే U_DEBUG
    uep->ue_magic = UE_MAGIC;
#అయితే_ముగింపు

    uep->ue_కొలత = కొలత;
    uep->ue_శిఖరం = శిఖరం;
    ఒకవేళ (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    లేకపోతే ఒకవేళ (bot > curbuf->b_ml.ml_పంక్తి_లెక్క)
	uep->ue_bot = 0;
    లేకపోతే
    {
	uep->ue_lcount = curbuf->b_ml.ml_పంక్తి_లెక్క;
	curbuf->b_u_newhead->uh_getbot_నమోదు = uep;
    }

    ఒకవేళ (కొలత > 0)
    {
	ఒకవేళ ((uep->ue_array = U_ALLOC_పంక్తి(పరిమాణం(undoline_T) * కొలత)) == ఖాళీ)
	{
	    u_freeentry(uep, 0L);
	    వెళ్ళు nomem;
	}
	కొరకు (i = 0, lnum = శిఖరం + 1; i < కొలత; ++i)
	{
	    fast_breakcheck();
	    ఒకవేళ (got_int)
	    {
		u_freeentry(uep, i);
		తిరిగి FAIL;
	    }
	    ఒకవేళ (u_save_పంక్తి(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		వెళ్ళు nomem;
	    }
	}
    }
    లేకపోతే
	uep->ue_array = ఖాళీ;
    uep->ue_తదుపరి = curbuf->b_u_newhead->uh_నమోదు;
    curbuf->b_u_newhead->uh_నమోదు = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#నిర్వచితమైతే U_DEBUG
    u_check(FALSE);
#అయితే_ముగింపు
    తిరిగి OK;

nomem:
    సందేశం_silent = 0;	// must display the prompt
    ఒకవేళ (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	తిరిగి OK;
    }
    do_outofmem_సందేశం((long_u)0);
    తిరిగి FAIL;
}

#ఒకవేళ defined(FEAT_PERSISTENT_UNDO)

# define UF_ప్రారంభం_MAGIC	    "Vim\237UnDo\345"  // magic at start of undofile
# define UF_ప్రారంభం_MAGIC_LEN	9
# define UF_శీర్షిక_MAGIC	0x5fd0	// magic at start of header
# define UF_శీర్షిక_చివర_MAGIC	0xe7aa	// magic after last header
# define UF_నమోదు_MAGIC		0xf518	// magic at start of entry
# define UF_నమోదు_చివర_MAGIC	0x3581	// magic after last entry
# define UF_సంస్కరణ		2	// 2-byte undofile version number
# define UF_సంస్కరణ_CRYPT	0x8002	// idem, encrypted

// extra fields for header
# define UF_చివరి_SAVE_NR	1

// extra fields for uhp
# define UHP_SAVE_NR		1

/*
 * Compute the hash for the current buffer text into hash[UNDO_HASH_SIZE].
 */
    శూన్య
u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;

    sha256_ప్రారంభం(&ctx);
    కొరకు (lnum = 1; lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క; ++lnum)
	sha256_update(&ctx, ml_get(lnum), (UINT32_T)(ml_get_len(lnum) + 1));
    sha256_ముగింపు(&ctx, hash);
}

/*
 * Return an allocated string of the full path of the target undofile.
 * When "reading" is TRUE find the file to read, go over all directories in
 * 'undodir'.
 * When "reading" is FALSE use the first name where the directory exists.
 * Returns NULL when there is no place to write or no file to read.
 */
    స్థిర char_u *
u_get_undo_file_name(char_u *buf_ffname, పూర్ణాంకం reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {ఖాళీ, 0};
    char_u	*undo_file_name = ఖాళీ;
    పూర్ణాంకం		dir_len;
    char_u	*p;
    size_t	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    size_t	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    ఒకవేళ (ffname == ఖాళీ)
	తిరిగి ఖాళీ;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    ఒకవేళ (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    అయితే (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	ఒకవేళ (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    ఒకవేళ (undo_file_name == ఖాళీ)
		ఆపు;
	    p = gettail(undo_file_name);
	    plen = (size_t)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# లేకపోతే
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	లేకపోతే
	{
	    dir_name[dir_len] = NUL;
	    ఒకవేళ (mch_isdir(dir_name))
	    {
		string_T    ret;

		ఒకవేళ (munged_name.string == ఖాళీ)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    ఒకవేళ (munged_name.string == ఖాళీ)
			తిరిగి ఖాళీ;
		    munged_name.నిడివి = ffnamelen;
		    కొరకు (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			ఒకవేళ (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (size_t)dir_len,
		    munged_name.string, munged_name.నిడివి, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	ఒకవేళ (undo_file_name != ఖాళీ && (!reading
			       || mch_stat((అక్షరం *)undo_file_name, &st) >= 0))
	    ఆపు;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    తిరిగి undo_file_name;
}

    స్థిర శూన్య
corruption_error(అక్షరం *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    స్థిర శూన్య
u_free_uhp(u_శీర్షిక_T *uhp)
{
    u_నమోదు_T	*nuep;
    u_నమోదు_T	*uep;

    uep = uhp->uh_నమోదు;
    అయితే (uep != ఖాళీ)
    {
	nuep = uep->ue_తదుపరి;
	u_freeentry(uep, uep->ue_కొలత);
	uep = nuep;
    }
    vim_free(uhp);
}

/*
 * Write a sequence of bytes to the undo file.
 * Buffers and encrypts as needed.
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    ఒకవేళ (bi->bi_బఫరు != ఖాళీ)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	అయితే (bi->bi_వాడిన + len_todo >= CRYPT_BUF_కొలత)
	{
	    size_t	n = CRYPT_BUF_కొలత - bi->bi_వాడిన;

	    mch_memmove(bi->bi_బఫరు + bi->bi_వాడిన, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_వాడిన = CRYPT_BUF_కొలత;
	    ఒకవేళ (undo_flush(bi) == FAIL)
		తిరిగి FAIL;
	}
	ఒకవేళ (len_todo > 0)
	{
	    mch_memmove(bi->bi_బఫరు + bi->bi_వాడిన, p, len_todo);
	    bi->bi_వాడిన += len_todo;
	}
	తిరిగి OK;
    }
# endif
    ఒకవేళ (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	తిరిగి FAIL;
    తిరిగి OK;
}

# ifdef FEAT_CRYPT
    స్థిర పూర్ణాంకం
undo_flush(bufinfo_T *bi)
{
    ఒకవేళ (bi->bi_బఫరు != ఖాళీ && bi->bi_state != ఖాళీ && bi->bi_వాడిన > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_బఫరు, bi->bi_వాడిన, FALSE);
	ఒకవేళ (fwrite(bi->bi_బఫరు, bi->bi_వాడిన, (size_t)1, bi->bi_fp) != 1)
	    తిరిగి FAIL;
	bi->bi_వాడిన = 0;
    }
    తిరిగి OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    ఒకవేళ (bi->bi_state != ఖాళీ && bi->bi_బఫరు == ఖాళీ)
    {
	// crypting every piece of text separately
	ఒకవేళ (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	లేకపోతే
	{
	    copy = lalloc(len, FALSE);
	    ఒకవేళ (copy == ఖాళీ)
		తిరిగి 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	ఒకవేళ (copy != small_buf)
	    vim_free(copy);
	తిరిగి i == 1 ? OK : FAIL;
    }
# endif
    తిరిగి undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
undo_write_బైట్లు(bufinfo_T *bi, long_u nr, పూర్ణాంకం len)
{
    char_u  buf[8];
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    bufi = 0;

    కొరకు (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    తిరిగి undo_write(bi, buf, (size_t)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    స్థిర శూన్య
put_శీర్షిక_ptr(bufinfo_T *bi, u_శీర్షిక_T *uhp)
{
    undo_write_బైట్లు(bi, (long_u)(uhp != ఖాళీ ? uhp->uh_seq : 0), 4);
}

    స్థిర పూర్ణాంకం
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ఒకవేళ (bi->bi_బఫరు != ఖాళీ)
    {
	char_u  buf[4];
	పూర్ణాంకం	n;

	undo_read(bi, buf, (size_t)4);
	n = ((గుర్తులేని)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	తిరిగి n;
    }
# endif
    తిరిగి get4c(bi->bi_fp);
}

    స్థిర పూర్ణాంకం
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ఒకవేళ (bi->bi_బఫరు != ఖాళీ)
    {
	char_u  buf[2];
	పూర్ణాంకం	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	తిరిగి n;
    }
# endif
    తిరిగి get2c(bi->bi_fp);
}

    స్థిర పూర్ణాంకం
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ఒకవేళ (bi->bi_బఫరు != ఖాళీ)
    {
	char_u  buf[1];

	undo_read(bi, buf, (size_t)1);
	తిరిగి buf[0];
    }
# endif
    తిరిగి getc(bi->bi_fp);
}

    స్థిర time_t
undo_read_సమయం(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ఒకవేళ (bi->bi_బఫరు != ఖాళీ)
    {
	char_u  buf[8];
	time_t	n = 0;
	పూర్ణాంకం	i;

	undo_read(bi, buf, (size_t)8);
	కొరకు (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	తిరిగి n;
    }
# endif
    తిరిగి get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
undo_read(bufinfo_T *bi, char_u *బఫరు, size_t కొలత)
{
    పూర్ణాంకం retval = OK;

# ifdef FEAT_CRYPT
    ఒకవేళ (bi->bi_బఫరు != ఖాళీ)
    {
	పూర్ణాంకం	కొలత_todo = (పూర్ణాంకం)కొలత;
	char_u	*p = బఫరు;

	అయితే (కొలత_todo > 0)
	{
	    size_t n;

	    ఒకవేళ (bi->bi_వాడిన >= bi->bi_avail)
	    {
		n = fread(bi->bi_బఫరు, 1, (size_t)CRYPT_BUF_కొలత, bi->bi_fp);
		ఒకవేళ (n == 0)
		{
		    retval = FAIL;
		    ఆపు;
		}
		bi->bi_avail = n;
		bi->bi_వాడిన = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_బఫరు, bi->bi_avail, FALSE);
	    }
	    n = కొలత_todo;
	    ఒకవేళ (n > bi->bi_avail - bi->bi_వాడిన)
		n = bi->bi_avail - bi->bi_వాడిన;
	    mch_memmove(p, bi->bi_బఫరు + bi->bi_వాడిన, n);
	    bi->bi_వాడిన += n;
	    కొలత_todo -= (పూర్ణాంకం)n;
	    p += n;
	}
    }
    లేకపోతే
# endif
    ఒకవేళ (fread(బఫరు, కొలత, 1, bi->bi_fp) != 1)
	retval = FAIL;

    ఒకవేళ (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(బఫరు, 0, కొలత);
    తిరిగి retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    స్థిర char_u *
read_string_decrypt(bufinfo_T *bi, పూర్ణాంకం len)
{
    char_u  *ptr = alloc(len + 1);

    ఒకవేళ (ptr == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	తిరిగి ఖాళీ;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    ఒకవేళ (bi->bi_state != ఖాళీ && bi->bi_బఫరు == ఖాళీ)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    తిరిగి ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    స్థిర పూర్ణాంకం
serialize_శీర్షిక(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	సమయం_buf[8];

    // Start writing, first the magic marker and undo info version.
    ఒకవేళ (fwrite(UF_ప్రారంభం_MAGIC, (size_t)UF_ప్రారంభం_MAGIC_LEN, (size_t)1, fp) != 1)
	తిరిగి FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    ఒకవేళ (*buf->b_p_కీలకం != NUL)
    {
	char_u *శీర్షిక;
	పూర్ణాంకం    శీర్షిక_len;
	దీర్ఘ	len;

	undo_write_బైట్లు(bi, (long_u)UF_సంస్కరణ_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_పద్ధతి_nr(buf),
					  buf->b_p_కీలకం, &శీర్షిక, &శీర్షిక_len);
	ఒకవేళ (bi->bi_state == ఖాళీ)
	    తిరిగి FAIL;
	len = (దీర్ఘ)fwrite(శీర్షిక, (size_t)శీర్షిక_len, (size_t)1, fp);
	vim_free(శీర్షిక);
	ఒకవేళ (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = ఖాళీ;
	    తిరిగి FAIL;
	}

	ఒకవేళ (crypt_సంపూర్ణ_undofile(crypt_get_పద్ధతి_nr(buf)))
	{
	    bi->bi_బఫరు = alloc(CRYPT_BUF_కొలత);
	    ఒకవేళ (bi->bi_బఫరు == ఖాళీ)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = ఖాళీ;
		తిరిగి FAIL;
	    }
	    bi->bi_వాడిన = 0;
	}
    }
    లేకపోతే
# endif
	undo_write_బైట్లు(bi, (long_u)UF_సంస్కరణ, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    ఒకవేళ (undo_write(bi, hash, (size_t)UNDO_HASH_కొలత) == FAIL)
	తిరిగి FAIL;

    // buffer-specific data
    undo_write_బైట్లు(bi, (long_u)buf->b_ml.ml_పంక్తి_లెక్క, 4);
    undo_write_బైట్లు(bi, (long_u)buf->b_u_పంక్తి_ptr.ul_textlen, 4);
    ఒకవేళ (buf->b_u_పంక్తి_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_పంక్తి_ptr.ul_పంక్తి,
			(size_t)buf->b_u_పంక్తి_ptr.ul_textlen) == FAIL)
	తిరిగి FAIL;
    undo_write_బైట్లు(bi, (long_u)buf->b_u_పంక్తి_lnum, 4);
    undo_write_బైట్లు(bi, (long_u)buf->b_u_పంక్తి_colnr, 4);

    // Undo structures header data
    put_శీర్షిక_ptr(bi, buf->b_u_oldhead);
    put_శీర్షిక_ptr(bi, buf->b_u_newhead);
    put_శీర్షిక_ptr(bi, buf->b_u_curhead);

    undo_write_బైట్లు(bi, (long_u)buf->b_u_numhead, 4);
    undo_write_బైట్లు(bi, (long_u)buf->b_u_seq_చివరి, 4);
    undo_write_బైట్లు(bi, (long_u)buf->b_u_seq_cur, 4);
    సమయం_గమ్యం_బైట్లు(buf->b_u_సమయం_cur, సమయం_buf);
    undo_write(bi, సమయం_buf, 8);

    // Optional fields.
    undo_write_బైట్లు(bi, 4, 1);
    undo_write_బైట్లు(bi, UF_చివరి_SAVE_NR, 1);
    undo_write_బైట్లు(bi, (long_u)buf->b_u_save_nr_చివరి, 4);

    undo_write_బైట్లు(bi, 0, 1);  // end marker

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
serialize_uhp(bufinfo_T *bi, u_శీర్షిక_T *uhp)
{
    పూర్ణాంకం		i;
    u_నమోదు_T	*uep;
    char_u	సమయం_buf[8];

    ఒకవేళ (undo_write_బైట్లు(bi, (long_u)UF_శీర్షిక_MAGIC, 2) == FAIL)
	తిరిగి FAIL;

    put_శీర్షిక_ptr(bi, uhp->uh_తదుపరి.ptr);
    put_శీర్షిక_ptr(bi, uhp->uh_prev.ptr);
    put_శీర్షిక_ptr(bi, uhp->uh_alt_తదుపరి.ptr);
    put_శీర్షిక_ptr(bi, uhp->uh_alt_prev.ptr);
    undo_write_బైట్లు(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_బైట్లు(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_బైట్లు(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    కొరకు (i = 0; i < NMARKS; ++i)
	serialize_pos(bi, uhp->uh_namedm[i]);
    serialize_visualinfo(bi, &uhp->uh_visual);
    సమయం_గమ్యం_బైట్లు(uhp->uh_సమయం, సమయం_buf);
    undo_write(bi, సమయం_buf, 8);

    // Optional fields.
    undo_write_బైట్లు(bi, 4, 1);
    undo_write_బైట్లు(bi, UHP_SAVE_NR, 1);
    undo_write_బైట్లు(bi, (long_u)uhp->uh_save_nr, 4);

    undo_write_బైట్లు(bi, 0, 1);  // end marker

    // Write all the entries.
    కొరకు (uep = uhp->uh_నమోదు; uep != ఖాళీ; uep = uep->ue_తదుపరి)
    {
	undo_write_బైట్లు(bi, (long_u)UF_నమోదు_MAGIC, 2);
	ఒకవేళ (serialize_uep(bi, uep) == FAIL)
	    తిరిగి FAIL;
    }
    undo_write_బైట్లు(bi, (long_u)UF_నమోదు_చివర_MAGIC, 2);
    తిరిగి OK;
}

    స్థిర u_శీర్షిక_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_శీర్షిక_T	*uhp;
    పూర్ణాంకం		i;
    u_నమోదు_T	*uep, *చివరి_uep;
    పూర్ణాంకం		c;
    పూర్ణాంకం		error;

    uhp = U_ALLOC_పంక్తి(పరిమాణం(u_శీర్షిక_T));
    ఒకవేళ (uhp == ఖాళీ)
	తిరిగి ఖాళీ;
    CLEAR_సూచిక(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_తదుపరి.seq = undo_read_4c(bi);
    uhp->uh_prev.seq = undo_read_4c(bi);
    uhp->uh_alt_తదుపరి.seq = undo_read_4c(bi);
    uhp->uh_alt_prev.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    ఒకవేళ (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	తిరిగి ఖాళీ;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    కొరకు (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_సమయం = undo_read_సమయం(bi);

    // Optional fields.
    కొరకు (;;)
    {
	పూర్ణాంకం len = undo_read_byte(bi);
	పూర్ణాంకం what;

	ఒకవేళ (len == EOF)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    తిరిగి ఖాళీ;
	}
	ఒకవేళ (len == 0)
	    ఆపు;
	what = undo_read_byte(bi);
	ఎంపిక (what)
	{
	    స్థితి UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		ఆపు;
	    అప్రమేయ:
		// field not supported, skip
		అయితే (--len >= 0)
		    (శూన్య)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    చివరి_uep = ఖాళీ;
    అయితే ((c = undo_read_2c(bi)) == UF_నమోదు_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	ఒకవేళ (చివరి_uep == ఖాళీ)
	    uhp->uh_నమోదు = uep;
	లేకపోతే
	    చివరి_uep->ue_తదుపరి = uep;
	చివరి_uep = uep;
	ఒకవేళ (uep == ఖాళీ || error)
	{
	    u_free_uhp(uhp);
	    తిరిగి ఖాళీ;
	}
    }
    ఒకవేళ (c != UF_నమోదు_చివర_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	తిరిగి ఖాళీ;
    }

    తిరిగి uhp;
}

/*
 * Serialize "uep".
 */
    స్థిర పూర్ణాంకం
serialize_uep(
    bufinfo_T	*bi,
    u_నమోదు_T	*uep)
{
    పూర్ణాంకం		i;

    undo_write_బైట్లు(bi, (long_u)uep->ue_శిఖరం, 4);
    undo_write_బైట్లు(bi, (long_u)uep->ue_bot, 4);
    undo_write_బైట్లు(bi, (long_u)uep->ue_lcount, 4);
    undo_write_బైట్లు(bi, (long_u)uep->ue_కొలత, 4);
    కొరకు (i = 0; i < uep->ue_కొలత; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	ఒకవేళ (undo_write_బైట్లు(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    తిరిగి FAIL;
	ఒకవేళ (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_పంక్తి, uep->ue_array[i].ul_textlen) == FAIL)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

    స్థిర u_నమోదు_T *
unserialize_uep(bufinfo_T *bi, పూర్ణాంకం *error, char_u *file_name)
{
    పూర్ణాంకం		i;
    u_నమోదు_T	*uep;
    undoline_T	*array = ఖాళీ;
    char_u	*పంక్తి;
    పూర్ణాంకం		పంక్తి_len;

    uep = U_ALLOC_పంక్తి(పరిమాణం(u_నమోదు_T));
    ఒకవేళ (uep == ఖాళీ)
	తిరిగి ఖాళీ;
    CLEAR_సూచిక(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_శిఖరం = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_కొలత = undo_read_4c(bi);
    ఒకవేళ (uep->ue_కొలత > 0)
    {
	ఒకవేళ (uep->ue_కొలత < LONG_MAX / (దీర్ఘ)పరిమాణం(undoline_T))
	    array = U_ALLOC_పంక్తి(పరిమాణం(undoline_T) * uep->ue_కొలత);
	ఒకవేళ (array == ఖాళీ)
	{
	    *error = TRUE;
	    తిరిగి uep;
	}
	vim_memset(array, 0, పరిమాణం(undoline_T) * uep->ue_కొలత);
    }
    uep->ue_array = array;

    కొరకు (i = 0; i < uep->ue_కొలత; ++i)
    {
	పంక్తి_len = undo_read_4c(bi);
	ఒకవేళ (పంక్తి_len >= 0)
	    పంక్తి = read_string_decrypt(bi, పంక్తి_len);
	లేకపోతే
	{
	    పంక్తి = ఖాళీ;
	    corruption_error("line length", file_name);
	}
	ఒకవేళ (పంక్తి == ఖాళీ)
	{
	    *error = TRUE;
	    తిరిగి uep;
	}
	array[i].ul_పంక్తి = పంక్తి;
	array[i].ul_len = పంక్తి_len + 1;
	array[i].ul_textlen = పంక్తి_len;
    }
    తిరిగి uep;
}

/*
 * Serialize "pos".
 */
    స్థిర శూన్య
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_బైట్లు(bi, (long_u)pos.lnum, 4);
    undo_write_బైట్లు(bi, (long_u)pos.నిలువు, 4);
    undo_write_బైట్లు(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    స్థిర శూన్య
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    ఒకవేళ (pos->lnum < 0)
	pos->lnum = 0;
    pos->నిలువు = undo_read_4c(bi);
    ఒకవేళ (pos->నిలువు < 0)
	pos->నిలువు = 0;
    pos->coladd = undo_read_4c(bi);
    ఒకవేళ (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    స్థిర శూన్య
serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    serialize_pos(bi, info->vi_ప్రారంభం);
    serialize_pos(bi, info->vi_చివర);
    undo_write_బైట్లు(bi, (long_u)info->vi_బహుళకం, 4);
    undo_write_బైట్లు(bi, (long_u)info->vi_curswant, 4);
}

/*
 * Unserialize the visualinfo_T at the current position.
 */
    స్థిర శూన్య
unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    unserialize_pos(bi, &info->vi_ప్రారంభం);
    unserialize_pos(bi, &info->vi_చివర);
    info->vi_బహుళకం = undo_read_4c(bi);
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
    శూన్య
u_write_undo(
    char_u	*name,
    పూర్ణాంకం		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_శీర్షిక_T	*uhp;
    char_u	*file_name;
    పూర్ణాంకం		mark;
# ifdef U_DEBUG
    పూర్ణాంకం		శీర్షికలు_written = 0;
# endif
    పూర్ణాంకం		fd;
    FILE	*fp = ఖాళీ;
    పూర్ణాంకం		perm;
    పూర్ణాంకం		write_ok = FALSE;
# ifdef UNIX
    పూర్ణాంకం		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_క్షేత్రం(bi);

    ఒకవేళ (name == ఖాళీ)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	ఒకవేళ (file_name == ఖాళీ)
	{
	    ఒకవేళ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    తిరిగి;
	}
    }
    లేకపోతే
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    ఒకవేళ (buf->b_ffname != ఖాళీ)
    {
# ifdef UNIX
	ఒకవేళ (mch_stat((అక్షరం *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_బహుళకం;
	    st_old_valid = TRUE;
	}
# లేకపోతే
	perm = mch_getperm(buf->b_ffname);
	ఒకవేళ (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    ఒకవేళ (mch_getperm(file_name) >= 0)
    {
	ఒకవేళ (name == ఖాళీ || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((అక్షరం *)file_name, O_RDONLY|O_EXTRA, 0);
	    ఒకవేళ (fd < 0)
	    {
		ఒకవేళ (name != ఖాళీ || p_verbose > 0)
		{
		    ఒకవేళ (name == ఖాళీ)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    ఒకవేళ (name == ఖాళీ)
			verbose_leave();
		}
		వెళ్ళు theend;
	    }
	    లేకపోతే
	    {
		char_u	mbuf[UF_ప్రారంభం_MAGIC_LEN];
		పూర్ణాంకం	len;

		len = read_eintr(fd, mbuf, UF_ప్రారంభం_MAGIC_LEN);
		close(fd);
		ఒకవేళ (len < UF_ప్రారంభం_MAGIC_LEN
		      || memcmp(mbuf, UF_ప్రారంభం_MAGIC, UF_ప్రారంభం_MAGIC_LEN) != 0)
		{
		    ఒకవేళ (name != ఖాళీ || p_verbose > 0)
		    {
			ఒకవేళ (name == ఖాళీ)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			ఒకవేళ (name == ఖాళీ)
			    verbose_leave();
		    }
		    వెళ్ళు theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    ఒకవేళ (buf->b_u_numhead == 0 && buf->b_u_పంక్తి_ptr.ul_పంక్తి == ఖాళీ)
    {
	ఒకవేళ (p_verbose > 0)
	    verb_సందేశం(_("Skipping undo file write, nothing to undo"));
	వెళ్ళు theend;
    }

    fd = mch_open((అక్షరం *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    ఒకవేళ (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	వెళ్ళు theend;
    }
    (శూన్య)mch_setperm(file_name, perm);
    ఒకవేళ (p_verbose > 0)
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
    ఒకవేళ (st_old_valid
	    && mch_stat((అక్షరం *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  ఒకవేళ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    ఒకవేళ (buf->b_ffname != ఖాళీ)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    ఒకవేళ (fp == ఖాళీ)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	close(fd);
	mch_remove(file_name);
	వెళ్ళు theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    ఒకవేళ (serialize_శీర్షిక(&bi, hash) == FAIL)
	వెళ్ళు write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    అయితే (uhp != ఖాళీ)
    {
	// Serialize current UHP if we haven't seen it
	ఒకవేళ (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++శీర్షికలు_written;
# endif
	    ఒకవేళ (serialize_uhp(&bi, uhp) == FAIL)
		వెళ్ళు write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	ఒకవేళ (uhp->uh_prev.ptr != ఖాళీ && uhp->uh_prev.ptr->uh_walk != mark)
	    uhp = uhp->uh_prev.ptr;
	లేకపోతే ఒకవేళ (uhp->uh_alt_తదుపరి.ptr != ఖాళీ
				     && uhp->uh_alt_తదుపరి.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_తదుపరి.ptr;
	లేకపోతే ఒకవేళ (uhp->uh_తదుపరి.ptr != ఖాళీ && uhp->uh_alt_prev.ptr == ఖాళీ
					 && uhp->uh_తదుపరి.ptr->uh_walk != mark)
	    uhp = uhp->uh_తదుపరి.ptr;
	లేకపోతే ఒకవేళ (uhp->uh_alt_prev.ptr != ఖాళీ)
	    uhp = uhp->uh_alt_prev.ptr;
	లేకపోతే
	    uhp = uhp->uh_తదుపరి.ptr;
    }

    ఒకవేళ (undo_write_బైట్లు(&bi, (long_u)UF_శీర్షిక_చివర_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    ఒకవేళ (శీర్షికలు_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", శీర్షికలు_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    ఒకవేళ (bi.bi_state != ఖాళీ && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# ఒకవేళ defined(UNIX) && defined(HAVE_FSYNC)
    ఒకవేళ ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && fflush(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    fclose(fp);
    ఒకవేళ (!write_ok)
	semsg(_(e_write_error_లోపల_undo_file_str), file_name);

# ఒకవేళ defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    ఒకవేళ (buf->b_ffname != ఖాళీ)
	(శూన్య)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    ఒకవేళ (buf->b_ffname != ఖాళీ)
    {
	vim_acl_T	    acl;

	// For systems that support ACL: get the ACL from the original file.
	acl = mch_get_acl(buf->b_ffname);
	mch_సమితి_acl(file_name, acl);
	mch_free_acl(acl);
    }
# endif

theend:
# ifdef FEAT_CRYPT
    ఒకవేళ (bi.bi_state != ఖాళీ)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_బఫరు);
# endif
    ఒకవేళ (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    స్థిర పూర్ణాంకం
uhp_seq_cmp(స్థిరరాశి శూన్య *v1, స్థిరరాశి శూన్య *v2)
{
    స్థిరరాశి u_శీర్షిక_T *u1 = *(u_శీర్షిక_T **)v1;
    స్థిరరాశి u_శీర్షిక_T *u2 = *(u_శీర్షిక_T **)v2;

    తిరిగి u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    స్థిర దీర్ఘ
uhp_పట్టిక_కనుగొను(u_శీర్షిక_T **uhp_పట్టిక, దీర్ఘ అంకె_తల, దీర్ఘ seq)
{
    దీర్ఘ    lo = 0;
    దీర్ఘ    hi = అంకె_తల - 1;

    అయితే (lo <= hi)
    {
	దీర్ఘ mid = lo + (hi - lo) / 2;

	ఒకవేళ (uhp_పట్టిక[mid]->uh_seq < seq)
	    lo = mid + 1;
	లేకపోతే ఒకవేళ (uhp_పట్టిక[mid]->uh_seq > seq)
	    hi = mid - 1;
	లేకపోతే
	    తిరిగి mid;
    }
    తిరిగి -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    శూన్య
u_read_undo(char_u *name, char_u *hash, char_u *orig_name వాడని)
{
    char_u	*file_name;
    FILE	*fp;
    దీర్ఘ	సంస్కరణ_2, str_len;
    undoline_T	పంక్తి_ptr;
    linenr_T	పంక్తి_lnum;
    colnr_T	పంక్తి_colnr;
    linenr_T	పంక్తి_లెక్క;
    దీర్ఘ	అంకె_తల = 0;
    దీర్ఘ	old_శీర్షిక_seq, new_శీర్షిక_seq, cur_శీర్షిక_seq;
    దీర్ఘ	seq_చివరి, seq_cur;
    దీర్ఘ	చివరి_save_nr = 0;
    దీర్ఘ	old_idx = -1, new_idx = -1, cur_idx = -1;
    దీర్ఘ	అంకె_read_uhps = 0;
    time_t	seq_సమయం;
    దీర్ఘ	i;
    పూర్ణాంకం		c;
    u_శీర్షిక_T	*uhp;
    u_శీర్షిక_T	**uhp_పట్టిక = ఖాళీ;
    char_u	read_hash[UNDO_HASH_కొలత];
    char_u	magic_buf[UF_ప్రారంభం_MAGIC_LEN];
# ifdef U_DEBUG
    పూర్ణాంకం		*uhp_పట్టిక_వాడిన;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_క్షేత్రం(bi);
    పంక్తి_ptr.ul_len = 0;
    పంక్తి_ptr.ul_textlen = 0;
    పంక్తి_ptr.ul_పంక్తి = ఖాళీ;

    ఒకవేళ (name == ఖాళీ)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	ఒకవేళ (file_name == ఖాళీ)
	    తిరిగి;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	ఒకవేళ (mch_stat((అక్షరం *)orig_name, &st_orig) >= 0
		&& mch_stat((అక్షరం *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    ఒకవేళ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    తిరిగి;
	}
# endif
    }
    లేకపోతే
	file_name = name;

    ఒకవేళ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((అక్షరం *)file_name, "r");
    ఒకవేళ (fp == ఖాళీ)
    {
	ఒకవేళ (name != ఖాళీ || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	వెళ్ళు error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    ఒకవేళ (fread(magic_buf, UF_ప్రారంభం_MAGIC_LEN, 1, fp) != 1
		|| memcmp(magic_buf, UF_ప్రారంభం_MAGIC, UF_ప్రారంభం_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	వెళ్ళు error;
    }
    సంస్కరణ_2 = get2c(fp);
    ఒకవేళ (సంస్కరణ_2 == UF_సంస్కరణ_CRYPT)
    {
# ifdef FEAT_CRYPT
	ఒకవేళ (*curbuf->b_p_కీలకం == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    వెళ్ళు error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_కీలకం);
	ఒకవేళ (bi.bi_state == ఖాళీ)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    వెళ్ళు error;
	}
	ఒకవేళ (crypt_సంపూర్ణ_undofile(bi.bi_state->పద్ధతి_nr))
	{
	    bi.bi_బఫరు = alloc(CRYPT_BUF_కొలత);
	    ఒకవేళ (bi.bi_బఫరు == ఖాళీ)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = ఖాళీ;
		వెళ్ళు error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_వాడిన = 0;
	}
# లేకపోతే
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	వెళ్ళు error;
# endif
    }
    లేకపోతే ఒకవేళ (సంస్కరణ_2 != UF_సంస్కరణ)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	వెళ్ళు error;
    }

    ఒకవేళ (undo_read(&bi, read_hash, (size_t)UNDO_HASH_కొలత) == FAIL)
    {
	corruption_error("hash", file_name);
	వెళ్ళు error;
    }
    పంక్తి_లెక్క = (linenr_T)undo_read_4c(&bi);
    ఒకవేళ (memcmp(hash, read_hash, UNDO_HASH_కొలత) != 0
				  || పంక్తి_లెక్క != curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	ఒకవేళ (p_verbose > 0 || name != ఖాళీ)
	{
	    ఒకవేళ (name == ఖాళీ)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    ఒకవేళ (name == ఖాళీ)
		verbose_leave();
	}
	వెళ్ళు error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    ఒకవేళ (str_len < 0)
	వెళ్ళు error;
    ఒకవేళ (str_len > 0)
    {
	పంక్తి_ptr.ul_పంక్తి = read_string_decrypt(&bi, str_len);
	పంక్తి_ptr.ul_len = str_len + 1;
	పంక్తి_ptr.ul_textlen = str_len;
    }
    పంక్తి_lnum = (linenr_T)undo_read_4c(&bi);
    పంక్తి_colnr = (colnr_T)undo_read_4c(&bi);
    ఒకవేళ (పంక్తి_lnum < 0 || పంక్తి_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	వెళ్ళు error;
    }

    // Begin general undo data
    old_శీర్షిక_seq = undo_read_4c(&bi);
    new_శీర్షిక_seq = undo_read_4c(&bi);
    cur_శీర్షిక_seq = undo_read_4c(&bi);
    అంకె_తల = undo_read_4c(&bi);
    seq_చివరి = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_సమయం = undo_read_సమయం(&bi);

    // Optional header fields.
    కొరకు (;;)
    {
	పూర్ణాంకం len = undo_read_byte(&bi);
	పూర్ణాంకం what;

	ఒకవేళ (len == 0 || len == EOF)
	    ఆపు;
	what = undo_read_byte(&bi);
	ఎంపిక (what)
	{
	    స్థితి UF_చివరి_SAVE_NR:
		చివరి_save_nr = undo_read_4c(&bi);
		ఆపు;
	    అప్రమేయ:
		// field not supported, skip
		అయితే (--len >= 0)
		    (శూన్య)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    ఒకవేళ (అంకె_తల > 0)
    {
	ఒకవేళ (అంకె_తల < LONG_MAX / (దీర్ఘ)పరిమాణం(u_శీర్షిక_T *))
	    uhp_పట్టిక = U_ALLOC_పంక్తి(అంకె_తల * పరిమాణం(u_శీర్షిక_T *));
	ఒకవేళ (uhp_పట్టిక == ఖాళీ)
	    వెళ్ళు error;
    }

    అయితే ((c = undo_read_2c(&bi)) == UF_శీర్షిక_MAGIC)
    {
	ఒకవేళ (అంకె_read_uhps >= అంకె_తల)
	{
	    corruption_error("num_head too small", file_name);
	    వెళ్ళు error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	ఒకవేళ (uhp == ఖాళీ)
	    వెళ్ళు error;
	uhp_పట్టిక[అంకె_read_uhps++] = uhp;
    }

    ఒకవేళ (అంకె_read_uhps != అంకె_తల)
    {
	corruption_error("num_head", file_name);
	వెళ్ళు error;
    }
    ఒకవేళ (c != UF_శీర్షిక_చివర_MAGIC)
    {
	corruption_error("end marker", file_name);
	వెళ్ళు error;
    }

# ifdef U_DEBUG
    uhp_పట్టిక_వాడిన = alloc_clear(పరిమాణం(పూర్ణాంకం) * అంకె_తల + 1);
#  define సమితి_FLAG(j) ++uhp_పట్టిక_వాడిన[j]
# లేకపోతే
#  define సమితి_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    ఒకవేళ (అంకె_తల > 0)
	qsort(uhp_పట్టిక, (size_t)అంకె_తల, పరిమాణం(u_శీర్షిక_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    కొరకు (i = 0; i < అంకె_తల - 1; i++)
    {
	ఒకవేళ (uhp_పట్టిక[i]->uh_seq == uhp_పట్టిక[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    వెళ్ళు error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    చేయి { \
	దీర్ఘ fidx = uhp_పట్టిక_కనుగొను(uhp_పట్టిక, అంకె_తల, (link).seq); \
	\
	ఒకవేళ (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_పట్టిక[fidx]; \
	    సమితి_FLAG(fidx); \
	} \
	లేకపోతే \
	    (link).ptr = ఖాళీ; \
    } అయితే (0)

    కొరకు (i = 0; i < అంకె_తల; i++)
    {
	uhp = uhp_పట్టిక[i];
	SWIZZLE_SEQ(uhp->uh_తదుపరి, i);
	SWIZZLE_SEQ(uhp->uh_prev, i);
	SWIZZLE_SEQ(uhp->uh_alt_తదుపరి, i);
	SWIZZLE_SEQ(uhp->uh_alt_prev, i);
	ఒకవేళ (old_శీర్షిక_seq > 0 && old_idx < 0 && uhp->uh_seq == old_శీర్షిక_seq)
	{
	    old_idx = i;
	    సమితి_FLAG(i);
	}
	ఒకవేళ (new_శీర్షిక_seq > 0 && new_idx < 0 && uhp->uh_seq == new_శీర్షిక_seq)
	{
	    new_idx = i;
	    సమితి_FLAG(i);
	}
	ఒకవేళ (cur_శీర్షిక_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_శీర్షిక_seq)
	{
	    cur_idx = i;
	    సమితి_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? ఖాళీ : uhp_పట్టిక[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? ఖాళీ : uhp_పట్టిక[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? ఖాళీ : uhp_పట్టిక[cur_idx];
    curbuf->b_u_పంక్తి_ptr = పంక్తి_ptr;
    curbuf->b_u_పంక్తి_lnum = పంక్తి_lnum;
    curbuf->b_u_పంక్తి_colnr = పంక్తి_colnr;
    curbuf->b_u_numhead = అంకె_తల;
    curbuf->b_u_seq_చివరి = seq_చివరి;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_సమయం_cur = seq_సమయం;
    curbuf->b_u_save_nr_చివరి = చివరి_save_nr;
    curbuf->b_u_save_nr_cur = చివరి_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_పట్టిక);

# ifdef U_DEBUG
    కొరకు (i = 0; i < అంకె_తల; ++i)
	ఒకవేళ (uhp_పట్టిక_వాడిన[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_పట్టిక_వాడిన);
    u_check(TRUE);
# endif

    ఒకవేళ (name != ఖాళీ)
	smsg(_("Finished reading undo file %s"), file_name);
    వెళ్ళు theend;

error:
    vim_free(పంక్తి_ptr.ul_పంక్తి);
    ఒకవేళ (uhp_పట్టిక != ఖాళీ)
    {
	కొరకు (i = 0; i < అంకె_read_uhps; i++)
	    ఒకవేళ (uhp_పట్టిక[i] != ఖాళీ)
		u_free_uhp(uhp_పట్టిక[i]);
	vim_free(uhp_పట్టిక);
    }

theend:
# ifdef FEAT_CRYPT
    ఒకవేళ (bi.bi_state != ఖాళీ)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_బఫరు);
# endif
    ఒకవేళ (fp != ఖాళీ)
	fclose(fp);
    ఒకవేళ (file_name != name)
	vim_free(file_name);
    తిరిగి;
}

#అయితే_ముగింపు // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    శూన్య
u_undo(పూర్ణాంకం లెక్క)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    ఒకవేళ (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	లెక్క = 1;
    }

    ఒకవేళ (vim_strchr(p_cpo, CPO_UNDO) == ఖాళీ)
	undo_undoes = TRUE;
    లేకపోతే
	undo_undoes = !undo_undoes;
    u_doit(లెక్క);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    శూన్య
u_redo(పూర్ణాంకం లెక్క)
{
    ఒకవేళ (vim_strchr(p_cpo, CPO_UNDO) == ఖాళీ)
	undo_undoes = FALSE;
    u_doit(లెక్క);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    స్థిర శూన్య
u_doit(పూర్ణాంకం startcount)
{
    పూర్ణాంకం లెక్క = startcount;

    ఒకవేళ (!undo_allowed())
	తిరిగి;

    u_newcount = 0;
    u_oldcount = 0;
    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
	u_oldcount = -1;
    అయితే (లెక్క--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	ఒకవేళ (undo_undoes)
	{
	    ఒకవేళ (curbuf->b_u_curhead == ఖాళీ)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    లేకపోతే ఒకవేళ (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_తదుపరి.ptr;
	    // nothing to undo
	    ఒకవేళ (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == ఖాళీ)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		ఒకవేళ (లెక్క == startcount - 1)
		{
		    ఒకవేళ (!shortmess(SHM_UNDO))
			సందేశం(_("Already at oldest change"));
		    తిరిగి;
		}
		ఆపు;
	    }

	    u_undoredo(TRUE);
	}
	లేకపోతే
	{
	    ఒకవేళ (curbuf->b_u_curhead == ఖాళీ || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		ఒకవేళ (లెక్క == startcount - 1)
		{
		    ఒకవేళ (!shortmess(SHM_UNDO))
			సందేశం(_("Already at newest change"));
		    తిరిగి;
		}
		ఆపు;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    ఒకవేళ (curbuf->b_u_curhead->uh_prev.ptr == ఖాళీ)
		curbuf->b_u_newhead = curbuf->b_u_curhead;
	    curbuf->b_u_curhead = curbuf->b_u_curhead->uh_prev.ptr;
	}
    }
    u_undo_చివర(undo_undoes, FALSE);
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
    శూన్య
undo_సమయం(
    దీర్ఘ	అడుగు,
    పూర్ణాంకం		sec,
    పూర్ణాంకం		file,
    పూర్ణాంకం		absolute)
{
    దీర్ఘ	    లక్ష్యం;
    దీర్ఘ	    closest;
    దీర్ఘ	    closest_ప్రారంభం;
    దీర్ఘ	    closest_seq = 0;
    దీర్ఘ	    val;
    u_శీర్షిక_T	    *uhp = ఖాళీ;
    u_శీర్షిక_T	    *చివరి;
    పూర్ణాంకం		    mark;
    పూర్ణాంకం		    nomark = 0;  // shut up compiler
    పూర్ణాంకం		    విడత;
    పూర్ణాంకం		    dosec = sec;
    పూర్ణాంకం		    dofile = file;
    పూర్ణాంకం		    above = FALSE;
    పూర్ణాంకం		    did_undo = TRUE;

    ఒకవేళ (పాఠ్యం_locked())
    {
	పాఠ్యం_locked_సందేశం();
	తిరిగి;
    }

    // First make sure the current undoable change is synced.
    ఒకవేళ (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    ఒకవేళ (absolute)
    {
	లక్ష్యం = అడుగు;
	closest = -1;
    }
    లేకపోతే
    {
	ఒకవేళ (dosec)
	    లక్ష్యం = (దీర్ఘ)(curbuf->b_u_సమయం_cur) + అడుగు;
	లేకపోతే ఒకవేళ (dofile)
	{
	    ఒకవేళ (అడుగు < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		ఒకవేళ (uhp != ఖాళీ)
		    uhp = uhp->uh_తదుపరి.ptr;
		లేకపోతే
		    uhp = curbuf->b_u_newhead;
		ఒకవేళ (uhp != ఖాళీ && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    లక్ష్యం = curbuf->b_u_save_nr_cur + అడుగు;
		లేకపోతే
		    // count the changes since the last write as one step
		    లక్ష్యం = curbuf->b_u_save_nr_cur + అడుగు + 1;
		ఒకవేళ (లక్ష్యం <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    లేకపోతే
	    {
		// Moving forward to a newer write.
		లక్ష్యం = curbuf->b_u_save_nr_cur + అడుగు;
		ఒకవేళ (లక్ష్యం > curbuf->b_u_save_nr_చివరి)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    లక్ష్యం = curbuf->b_u_seq_చివరి + 1;
		    dofile = FALSE;
		}
	    }
	}
	లేకపోతే
	    లక్ష్యం = curbuf->b_u_seq_cur + అడుగు;
	ఒకవేళ (అడుగు < 0)
	{
	    ఒకవేళ (లక్ష్యం < 0)
		లక్ష్యం = 0;
	    closest = -1;
	}
	లేకపోతే
	{
	    ఒకవేళ (dosec)
		closest = (దీర్ఘ)(vim_సమయం() + 1);
	    లేకపోతే ఒకవేళ (dofile)
		closest = curbuf->b_u_save_nr_చివరి + 2;
	    లేకపోతే
		closest = curbuf->b_u_seq_చివరి + 2;
	    ఒకవేళ (లక్ష్యం >= closest)
		లక్ష్యం = closest - 1;
	}
    }
    closest_ప్రారంభం = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    ఒకవేళ (లక్ష్యం == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	వెళ్ళు లక్ష్యం_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    కొరకు (విడత = 1; విడత <= 2; ++విడత)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	ఒకవేళ (curbuf->b_u_curhead == ఖాళీ)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	లేకపోతే
	    uhp = curbuf->b_u_curhead;

	అయితే (uhp != ఖాళీ)
	{
	    uhp->uh_walk = mark;
	    ఒకవేళ (dosec)
		val = (దీర్ఘ)(uhp->uh_సమయం);
	    లేకపోతే ఒకవేళ (dofile)
		val = uhp->uh_save_nr;
	    లేకపోతే
		val = uhp->uh_seq;

	    ఒకవేళ (విడత == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		ఒకవేళ ((అడుగు < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
			      : uhp->uh_seq > curbuf->b_u_seq_cur)
			&& ((dosec && val == closest)
			    ? (అడుగు < 0
				? uhp->uh_seq < closest_seq
				: uhp->uh_seq > closest_seq)
			    : closest == closest_ప్రారంభం
				|| (val > లక్ష్యం
				    ? (closest > లక్ష్యం
					? val - లక్ష్యం <= closest - లక్ష్యం
					: val - లక్ష్యం <= లక్ష్యం - closest)
				    : (closest > లక్ష్యం
					? లక్ష్యం - val <= closest - లక్ష్యం
					: లక్ష్యం - val <= లక్ష్యం - closest))))
		{
		    closest = val;
		    closest_seq = uhp->uh_seq;
		}
	    }

	    // Quit searching when we found a match.  But when searching for a
	    // time we need to continue looking for the best uh_seq.
	    ఒకవేళ (లక్ష్యం == val && !dosec)
	    {
		లక్ష్యం = uhp->uh_seq;
		ఆపు;
	    }

	    // go down in the tree if we haven't been there
	    ఒకవేళ (uhp->uh_prev.ptr != ఖాళీ && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
		uhp = uhp->uh_prev.ptr;

	    // go to alternate branch if we haven't been there
	    లేకపోతే ఒకవేళ (uhp->uh_alt_తదుపరి.ptr != ఖాళీ
		    && uhp->uh_alt_తదుపరి.ptr->uh_walk != nomark
		    && uhp->uh_alt_తదుపరి.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_తదుపరి.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    లేకపోతే ఒకవేళ (uhp->uh_తదుపరి.ptr != ఖాళీ && uhp->uh_alt_prev.ptr == ఖాళీ
		    && uhp->uh_తదుపరి.ptr->uh_walk != nomark
		    && uhp->uh_తదుపరి.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		ఒకవేళ (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_తదుపరి.ptr;
	    }

	    లేకపోతే
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		ఒకవేళ (uhp->uh_alt_prev.ptr != ఖాళీ)
		    uhp = uhp->uh_alt_prev.ptr;
		లేకపోతే
		    uhp = uhp->uh_తదుపరి.ptr;
	    }
	}

	ఒకవేళ (uhp != ఖాళీ)    // found it
	    ఆపు;

	ఒకవేళ (absolute)
	{
	    semsg(_(e_undo_సంఖ్య_nr_not_దొరికింది), అడుగు);
	    తిరిగి;
	}

	ఒకవేళ (closest == closest_ప్రారంభం)
	{
	    ఒకవేళ (!shortmess(SHM_UNDO))
	    {
		ఒకవేళ (అడుగు < 0)
		    సందేశం(_("Already at oldest change"));
		లేకపోతే
		    సందేశం(_("Already at newest change"));
	    }
	    తిరిగి;
	}

	లక్ష్యం = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	ఒకవేళ (అడుగు < 0)
	    above = TRUE;	// stop above the header
    }

లక్ష్యం_zero:
    // If we found it: Follow the path to go to where we want to be.
    ఒకవేళ (uhp != ఖాళీ || లక్ష్యం == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	అయితే (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    ఒకవేళ (uhp == ఖాళీ)
		uhp = curbuf->b_u_newhead;
	    లేకపోతే
		uhp = uhp->uh_తదుపరి.ptr;
	    ఒకవేళ (uhp == ఖాళీ || (లక్ష్యం > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == లక్ష్యం && !above))
		ఆపు;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    ఒకవేళ (లక్ష్యం > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	ఒకవేళ (లక్ష్యం > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    అయితే (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		ఒకవేళ (uhp == ఖాళీ)
		    ఆపు;

		// Go back to the first branch with a mark.
		అయితే (uhp->uh_alt_prev.ptr != ఖాళీ
				     && uhp->uh_alt_prev.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_prev.ptr;

		// Find the last branch with a mark, that's the one.
		చివరి = uhp;
		అయితే (చివరి->uh_alt_తదుపరి.ptr != ఖాళీ
				    && చివరి->uh_alt_తదుపరి.ptr->uh_walk == mark)
		    చివరి = చివరి->uh_alt_తదుపరి.ptr;
		ఒకవేళ (చివరి != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    అయితే (uhp->uh_alt_prev.ptr != ఖాళీ)
			uhp = uhp->uh_alt_prev.ptr;
		    ఒకవేళ (చివరి->uh_alt_తదుపరి.ptr != ఖాళీ)
			చివరి->uh_alt_తదుపరి.ptr->uh_alt_prev.ptr =
							చివరి->uh_alt_prev.ptr;
		    చివరి->uh_alt_prev.ptr->uh_alt_తదుపరి.ptr =
							చివరి->uh_alt_తదుపరి.ptr;
		    చివరి->uh_alt_prev.ptr = ఖాళీ;
		    చివరి->uh_alt_తదుపరి.ptr = uhp;
		    uhp->uh_alt_prev.ptr = చివరి;

		    ఒకవేళ (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = చివరి;
		    uhp = చివరి;
		    ఒకవేళ (uhp->uh_తదుపరి.ptr != ఖాళీ)
			uhp->uh_తదుపరి.ptr->uh_prev.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		ఒకవేళ (uhp->uh_walk != mark)
		    ఆపు;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		ఒకవేళ (uhp->uh_seq == లక్ష్యం && above)
		{
		    curbuf->b_u_seq_cur = లక్ష్యం - 1;
		    ఆపు;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		ఒకవేళ (uhp->uh_prev.ptr == ఖాళీ)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_prev.ptr;
		did_undo = FALSE;

		ఒకవేళ (uhp->uh_seq == లక్ష్యం)	// found it!
		    ఆపు;

		uhp = uhp->uh_prev.ptr;
		ఒకవేళ (uhp == ఖాళీ || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    అంతర్గత_error("undo_time()");
		    ఆపు;
		}
	    }
	}
    }
    u_undo_చివర(did_undo, absolute);
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
    స్థిర శూన్య
u_undoredo(పూర్ణాంకం undo)
{
    undoline_T	*newarray = ఖాళీ;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	శిఖరం, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    దీర్ఘ	i;
    u_నమోదు_T	*uep, *nuep;
    u_నమోదు_T	*newlist = ఖాళీ;
    పూర్ణాంకం		old_flags;
    పూర్ణాంకం		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    పూర్ణాంకం		ఖాళీలు_బఫరు;		    // buffer became empty
    u_శీర్షిక_T	*curhead = curbuf->b_u_curhead;

    // Don't want autocommands using the undo structures here, they are
    // invalid till the end.
    ఖండం_autocmds();

#నిర్వచితమైతే U_DEBUG
    u_check(FALSE);
#అయితే_ముగింపు
    old_flags = curhead->uh_flags;
    new_flags = (curbuf->b_మారింది ? UH_మారింది : 0) +
	       ((curbuf->b_ml.ml_flags & ML_ఖాళీలు) ? UH_EMPTYBUF : 0);
    setpcmark();

    /*
     * save marks before undo/redo
     */
    mch_memmove(namedm, curbuf->b_namedm, పరిమాణం(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_ప్రారంభం.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
    curbuf->b_op_ప్రారంభం.నిలువు = 0;
    curbuf->b_op_చివర.lnum = 0;
    curbuf->b_op_చివర.నిలువు = 0;

    కొరకు (uep = curhead->uh_నమోదు; uep != ఖాళీ; uep = nuep)
    {
	శిఖరం = uep->ue_శిఖరం;
	bot = uep->ue_bot;
	ఒకవేళ (bot == 0)
	    bot = curbuf->b_ml.ml_పంక్తి_లెక్క + 1;
	ఒకవేళ (శిఖరం > curbuf->b_ml.ml_పంక్తి_లెక్క || శిఖరం >= bot
				      || bot > curbuf->b_ml.ml_పంక్తి_లెక్క + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_పంక్తి_numbers_wrong);
	    మారింది();		// don't want UNCHANGED now
	    తిరిగి;
	}

	oldsize = bot - శిఖరం - 1;    // number of lines before undo
	newsize = uep->ue_కొలత;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    ఒకవేళ (lnum >= శిఖరం && lnum <= శిఖరం + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    లేకపోతే ఒకవేళ (శిఖరం < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		కొరకు (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(శిఖరం + 1 + i);

		    ఒకవేళ (curbuf->b_ml.ml_పంక్తి_len != uep->ue_array[i].ul_len
			    || memcmp(uep->ue_array[i].ul_పంక్తి, p,
						curbuf->b_ml.ml_పంక్తి_len) != 0)
			ఆపు;
		}
		ఒకవేళ (i == newsize && newlnum == MAXLNUM && uep->ue_తదుపరి == ఖాళీ)
		{
		    newlnum = శిఖరం;
		    new_curpos.lnum = newlnum + 1;
		}
		లేకపోతే ఒకవేళ (i < newsize)
		{
		    newlnum = శిఖరం + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	ఖాళీలు_బఫరు = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	ఒకవేళ (oldsize > 0)
	{
	    ఒకవేళ ((newarray = U_ALLOC_పంక్తి(పరిమాణం(undoline_T) * oldsize)) == ఖాళీ)
	    {
		do_outofmem_సందేశం((long_u)(పరిమాణం(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		అయితే (uep != ఖాళీ)
		{
		    nuep = uep->ue_తదుపరి;
		    u_freeentry(uep, uep->ue_కొలత);
		    uep = nuep;
		}
		ఆపు;
	    }
	    // delete backwards, it goes faster in most cases
	    కొరకు (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		ఒకవేళ (u_save_పంక్తి(&newarray[i], lnum) == FAIL)
		    do_outofmem_సందేశం((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		ఒకవేళ (curbuf->b_ml.ml_పంక్తి_లెక్క == 1)
		    ఖాళీలు_బఫరు = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	లేకపోతే
	    newarray = ఖాళీ;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	ఒకవేళ (newsize)
	{
	    కొరకు (lnum = శిఖరం, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		ఒకవేళ (ఖాళీలు_బఫరు && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_పంక్తి,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		లేకపోతే
		    ml_append_flags(lnum, uep->ue_array[i].ul_పంక్తి,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_పంక్తి);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#నిర్వచితమైతే FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, శిఖరం, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, శిఖరం + newsize + 1, &uep->ue_props,
								UNDOPROP_క్రింద);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, శిఖరం, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_క్రింద);
	ఒకవేళ (newsize > 0)
	    adjust_props_for_undo(curbuf, శిఖరం + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_క్రింద);
#అయితే_ముగింపు

	// adjust marks
	ఒకవేళ (oldsize != newsize)
	{
	    mark_adjust(శిఖరం + 1, శిఖరం + oldsize, (దీర్ఘ)MAXLNUM,
					       (దీర్ఘ)newsize - (దీర్ఘ)oldsize);
	    ఒకవేళ (curbuf->b_op_ప్రారంభం.lnum > శిఖరం + oldsize)
		curbuf->b_op_ప్రారంభం.lnum += newsize - oldsize;
	    ఒకవేళ (curbuf->b_op_చివర.lnum > శిఖరం + oldsize)
		curbuf->b_op_చివర.lnum += newsize - oldsize;
	}
	ఒకవేళ (oldsize > 0 || newsize > 0)
	{
	    మారింది_పంక్తులు(శిఖరం + 1, 0, bot, newsize - oldsize);
#నిర్వచితమైతే FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    ఒకవేళ (spell_check_కిటికీ(curwin) && bot <= curbuf->b_ml.ml_పంక్తి_లెక్క)
		redrawWinline(curwin, bot);
#అయితే_ముగింపు
	}

	// Set the '[ mark.
	ఒకవేళ (శిఖరం + 1 < curbuf->b_op_ప్రారంభం.lnum)
	    curbuf->b_op_ప్రారంభం.lnum = శిఖరం + 1;
	// Set the '] mark.
	ఒకవేళ (newsize == 0 && శిఖరం + 1 > curbuf->b_op_చివర.lnum)
	    curbuf->b_op_చివర.lnum = శిఖరం + 1;
	లేకపోతే ఒకవేళ (శిఖరం + newsize > curbuf->b_op_చివర.lnum)
	    curbuf->b_op_చివర.lnum = శిఖరం + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_కొలత = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = శిఖరం + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_తదుపరి;
	uep->ue_తదుపరి = newlist;
	newlist = uep;
    }

    // Ensure the '[ and '] marks are within bounds.
    ఒకవేళ (curbuf->b_op_ప్రారంభం.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	 curbuf->b_op_ప్రారంభం.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;
    ఒకవేళ (curbuf->b_op_చివర.lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
	 curbuf->b_op_చివర.lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_నమోదు = newlist;
    curhead->uh_flags = new_flags;
    ఒకవేళ ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_ఖాళీలు;
    ఒకవేళ (old_flags & UH_మారింది)
	మారింది();
    లేకపోతే
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	ఒకవేళ (!isNetbeansModified(curbuf))
#అయితే_ముగింపు
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    కొరకు (i = 0; i < NMARKS; ++i)
    {
	ఒకవేళ (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	ఒకవేళ (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	లేకపోతే
	    curhead->uh_namedm[i].lnum = 0;
    }
    ఒకవేళ (curhead->uh_visual.vi_ప్రారంభం.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    ఒకవేళ (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    ఒకవేళ (curwin->w_cursor.lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	ఒకవేళ (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.నిలువు = curhead->uh_cursor.నిలువు;
	    ఒకవేళ (virtual_చురుకైన() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    లేకపోతే
		curwin->w_cursor.coladd = 0;
	}
	లేకపోతే
	    beginline(BL_SOL | BL_FIX);
    }
    లేకపోతే
    {
	// We get here with the current cursor line being past the end (eg
	// after adding lines at the end of the file, and then undoing it).
	// check_cursor() will move the cursor to the last line.  Move it to
	// the first column here.
	curwin->w_cursor.నిలువు = 0;
	curwin->w_cursor.coladd = 0;
    }

    // Make sure the cursor is on an existing line and column.
    check_cursor();

    // Remember where we are for "g-" and ":earlier 10s".
    curbuf->b_u_seq_cur = curhead->uh_seq;
    ఒకవేళ (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	ఒకవేళ (curhead->uh_తదుపరి.ptr != ఖాళీ)
	    curbuf->b_u_seq_cur = curhead->uh_తదుపరి.ptr->uh_seq;
	లేకపోతే
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    ఒకవేళ (curhead->uh_save_nr != 0)
    {
	ఒకవేళ (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	లేకపోతే
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr;
    }

    // The timestamp can be the same for multiple changes, just use the one of
    // the undone/redone change.
    curbuf->b_u_సమయం_cur = curhead->uh_సమయం;

    unblock_autocmds();
#నిర్వచితమైతే U_DEBUG
    u_check(FALSE);
#అయితే_ముగింపు
}

/*
 * If we deleted or added lines, report the number of less/more lines.
 * Otherwise, report the number of changes (this may be incorrect
 * in some cases, but it's better than nothing).
 */
    స్థిర శూన్య
u_undo_చివర(
    పూర్ణాంకం		did_undo,	// just did an undo
    పూర్ణాంకం		absolute)	// used ":undo N"
{
    అక్షరం	*msgstr;
    u_శీర్షిక_T	*uhp;
    char_u	msgbuf[80];

#నిర్వచితమైతే FEAT_FOLDING
    ఒకవేళ ((fdo_flags & FDO_UNDO) && కీలకం_Typed)
	foldOpenCursor();
#అయితే_ముగింపు

    ఒకవేళ (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	తిరిగి;

    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
	--u_newcount;

    u_oldcount -= u_newcount;
    ఒకవేళ (u_oldcount == -1)
	msgstr = N_("more line");
    లేకపోతే ఒకవేళ (u_oldcount < 0)
	msgstr = N_("more lines");
    లేకపోతే ఒకవేళ (u_oldcount == 1)
	msgstr = N_("line less");
    లేకపోతే ఒకవేళ (u_oldcount > 1)
	msgstr = N_("fewer lines");
    లేకపోతే
    {
	u_oldcount = u_newcount;
	ఒకవేళ (u_newcount == 1)
	    msgstr = N_("change");
	లేకపోతే
	    msgstr = N_("changes");
    }

    ఒకవేళ (curbuf->b_u_curhead != ఖాళీ)
    {
	// For ":undo N" we prefer a "after #N" message.
	ఒకవేళ (absolute && curbuf->b_u_curhead->uh_తదుపరి.ptr != ఖాళీ)
	{
	    uhp = curbuf->b_u_curhead->uh_తదుపరి.ptr;
	    did_undo = FALSE;
	}
	లేకపోతే ఒకవేళ (did_undo)
	    uhp = curbuf->b_u_curhead;
	లేకపోతే
	    uhp = curbuf->b_u_curhead->uh_తదుపరి.ptr;
    }
    లేకపోతే
	uhp = curbuf->b_u_newhead;

    ఒకవేళ (uhp == ఖాళీ)
	*msgbuf = NUL;
    లేకపోతే
	కూడు_సమయం(msgbuf, పరిమాణం(msgbuf), uhp->uh_సమయం);

#నిర్వచితమైతే FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ఒకవేళ (wp->w_బఫరు == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#అయితే_ముగింపు
    ఒకవేళ (VIsual_చురుకైన)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == ఖాళీ ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    శూన్య
u_sync(
    పూర్ణాంకం	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    ఒకవేళ (curbuf->b_u_synced || (!force && no_u_sync > 0))
	తిరిగి;
#ఒకవేళ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ఒకవేళ (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	తిరిగి;		    // XIM is busy, don't break an undo sequence
#అయితే_ముగింపు
    ఒకవేళ (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    లేకపోతే
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = ఖాళీ;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    శూన్య
ex_undolist(exarg_T *eap వాడని)
{
    garray_T	ga;
    u_శీర్షిక_T	*uhp;
    పూర్ణాంకం		mark;
    పూర్ణాంకం		nomark;
    పూర్ణాంకం		changes = 1;
    పూర్ణాంకం		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, పరిమాణం(అక్షరం *), 20);

    uhp = curbuf->b_u_oldhead;
    అయితే (uhp != ఖాళీ)
    {
	ఒకవేళ (uhp->uh_prev.ptr == ఖాళీ && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    ఒకవేళ (ga_grow(&ga, 1) == FAIL)
		ఆపు;
	    len = vim_snprintf((అక్షరం *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    కూడు_సమయం(IObuff + len, IOSIZE - len, uhp->uh_సమయం);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (పూర్ణాంకం)STRLEN(IObuff + len);
	    ఒకవేళ (uhp->uh_save_nr > 0)
	    {
		పూర్ణాంకం n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((అక్షరం *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_దత్తాంశం))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	ఒకవేళ (uhp->uh_prev.ptr != ఖాళీ && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_prev.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	లేకపోతే ఒకవేళ (uhp->uh_alt_తదుపరి.ptr != ఖాళీ
		&& uhp->uh_alt_తదుపరి.ptr->uh_walk != nomark
		&& uhp->uh_alt_తదుపరి.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_తదుపరి.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	లేకపోతే ఒకవేళ (uhp->uh_తదుపరి.ptr != ఖాళీ && uhp->uh_alt_prev.ptr == ఖాళీ
		&& uhp->uh_తదుపరి.ptr->uh_walk != nomark
		&& uhp->uh_తదుపరి.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_తదుపరి.ptr;
	    --changes;
	}

	లేకపోతే
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    ఒకవేళ (uhp->uh_alt_prev.ptr != ఖాళీ)
		uhp = uhp->uh_alt_prev.ptr;
	    లేకపోతే
	    {
		uhp = uhp->uh_తదుపరి.ptr;
		--changes;
	    }
	}
    }

    ఒకవేళ (ga.ga_len == 0)
	సందేశం(_("Nothing to undo"));
    లేకపోతే
    {
	పూర్ణాంకం		i;

	క్రమీకరించు_అక్షరశ్రేణులు((char_u **)ga.ga_దత్తాంశం, ga.ga_len);

	సందేశం_ప్రారంభం();
	సందేశం_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	కొరకు (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    సందేశం_putchar('\n');
	    ఒకవేళ (got_int)
		ఆపు;
	    సందేశం_puts(((అక్షరం **)ga.ga_దత్తాంశం)[i]);
	}
	సందేశం_చివర();

	ga_clear_అక్షరశ్రేణులు(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    శూన్య
ex_undojoin(exarg_T *eap వాడని)
{
    ఒకవేళ (curbuf->b_u_newhead == ఖాళీ)
	తిరిగి;		    // nothing changed before
    ఒకవేళ (curbuf->b_u_curhead != ఖాళీ)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	తిరిగి;
    }
    ఒకవేళ (!curbuf->b_u_synced)
	తిరిగి;		    // already unsynced
    ఒకవేళ (get_undolevel() < 0)
	తిరిగి;		    // no entries, nothing to do
    లేకపోతే
	// Append next change to the last entry
	curbuf->b_u_synced = false;
}

/*
 * Called after writing or reloading the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
    శూన్య
u_unchanged(buf_T *buf)
{
    u_unch_branch(buf->b_u_oldhead);
    buf->b_did_warn = false;
}

/*
 * After reloading a buffer which was saved for 'undoreload': Find the first
 * line that was changed and set the cursor there.
 */
    శూన్య
u_కనుగొను_మొదటి_మారింది(శూన్య)
{
    u_శీర్షిక_T	*uhp = curbuf->b_u_newhead;
    u_నమోదు_T   *uep;
    linenr_T	lnum;

    ఒకవేళ (curbuf->b_u_curhead != ఖాళీ || uhp == ఖాళీ)
	తిరిగి;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_నమోదు;
    ఒకవేళ (uep->ue_శిఖరం != 0 || uep->ue_bot != 0)
	తిరిగి;

    కొరకు (lnum = 1; lnum < curbuf->b_ml.ml_పంక్తి_లెక్క
					      && lnum <= uep->ue_కొలత; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	ఒకవేళ (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_పంక్తి_len
		|| memcmp(p, uep->ue_array[lnum - 1].ul_పంక్తి, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    తిరిగి;
	}
    }
    ఒకవేళ (curbuf->b_ml.ml_పంక్తి_లెక్క != uep->ue_కొలత)
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
    శూన్య
u_update_save_nr(buf_T *buf)
{
    u_శీర్షిక_T	*uhp;

    ++buf->b_u_save_nr_చివరి;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_చివరి;
    uhp = buf->b_u_curhead;
    ఒకవేళ (uhp != ఖాళీ)
	uhp = uhp->uh_తదుపరి.ptr;
    లేకపోతే
	uhp = buf->b_u_newhead;
    ఒకవేళ (uhp != ఖాళీ)
	uhp->uh_save_nr = buf->b_u_save_nr_చివరి;
}

    స్థిర శూన్య
u_unch_branch(u_శీర్షిక_T *uhp)
{
    u_శీర్షిక_T	*uh;

    కొరకు (uh = uhp; uh != ఖాళీ; uh = uh->uh_prev.ptr)
    {
	uh->uh_flags |= UH_మారింది;
	ఒకవేళ (uh->uh_alt_తదుపరి.ptr != ఖాళీ)
	    u_unch_branch(uh->uh_alt_తదుపరి.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    స్థిర u_నమోదు_T *
u_get_headentry(శూన్య)
{
    ఒకవేళ (curbuf->b_u_newhead == ఖాళీ || curbuf->b_u_newhead->uh_నమోదు == ఖాళీ)
    {
	iemsg(e_undo_list_corrupt);
	తిరిగి ఖాళీ;
    }
    తిరిగి curbuf->b_u_newhead->uh_నమోదు;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    స్థిర శూన్య
u_getbot(శూన్య)
{
    u_నమోదు_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    ఒకవేళ (uep == ఖాళీ)
	తిరిగి;

    uep = curbuf->b_u_newhead->uh_getbot_నమోదు;
    ఒకవేళ (uep != ఖాళీ)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_పంక్తి_లెక్క - uep->ue_lcount;
	uep->ue_bot = uep->ue_శిఖరం + uep->ue_కొలత + 1 + extra;
	ఒకవేళ (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    iemsg(e_undo_పంక్తి_missing);
	    uep->ue_bot = uep->ue_శిఖరం + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_నమోదు = ఖాళీ;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    స్థిర శూన్య
u_freeheader(
    buf_T	    *buf,
    u_శీర్షిక_T	    *uhp,
    u_శీర్షిక_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_శీర్షిక_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    ఒకవేళ (uhp->uh_alt_తదుపరి.ptr != ఖాళీ)
	u_freebranch(buf, uhp->uh_alt_తదుపరి.ptr, uhpp);

    ఒకవేళ (uhp->uh_alt_prev.ptr != ఖాళీ)
	uhp->uh_alt_prev.ptr->uh_alt_తదుపరి.ptr = ఖాళీ;

    // Update the links in the list to remove the header.
    ఒకవేళ (uhp->uh_తదుపరి.ptr == ఖాళీ)
	buf->b_u_oldhead = uhp->uh_prev.ptr;
    లేకపోతే
	uhp->uh_తదుపరి.ptr->uh_prev.ptr = uhp->uh_prev.ptr;

    ఒకవేళ (uhp->uh_prev.ptr == ఖాళీ)
	buf->b_u_newhead = uhp->uh_తదుపరి.ptr;
    లేకపోతే
	కొరకు (uhap = uhp->uh_prev.ptr; uhap != ఖాళీ;
						 uhap = uhap->uh_alt_తదుపరి.ptr)
	    uhap->uh_తదుపరి.ptr = uhp->uh_తదుపరి.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    స్థిర శూన్య
u_freebranch(
    buf_T	    *buf,
    u_శీర్షిక_T	    *uhp,
    u_శీర్షిక_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_శీర్షిక_T	    *tofree, *తదుపరి;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    ఒకవేళ (uhp == buf->b_u_oldhead)
    {
	అయితే (buf->b_u_oldhead != ఖాళీ)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	తిరిగి;
    }

    ఒకవేళ (uhp->uh_alt_prev.ptr != ఖాళీ)
	uhp->uh_alt_prev.ptr->uh_alt_తదుపరి.ptr = ఖాళీ;

    తదుపరి = uhp;
    అయితే (తదుపరి != ఖాళీ)
    {
	tofree = తదుపరి;
	ఒకవేళ (tofree->uh_alt_తదుపరి.ptr != ఖాళీ)
	    u_freebranch(buf, tofree->uh_alt_తదుపరి.ptr, uhpp);   // recursive
	తదుపరి = tofree->uh_prev.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}

/*
 * Free all the undo entries for one header and the header itself.
 * This means that "uhp" is invalid when returning.
 */
    స్థిర శూన్య
u_freeentries(
    buf_T	    *buf,
    u_శీర్షిక_T	    *uhp,
    u_శీర్షిక_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_నమోదు_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    ఒకవేళ (buf->b_u_curhead == uhp)
	buf->b_u_curhead = ఖాళీ;
    ఒకవేళ (buf->b_u_newhead == uhp)
	buf->b_u_newhead = ఖాళీ;  // freeing the newest entry
    ఒకవేళ (uhpp != ఖాళీ && uhp == *uhpp)
	*uhpp = ఖాళీ;

    కొరకు (uep = uhp->uh_నమోదు; uep != ఖాళీ; uep = nuep)
    {
	nuep = uep->ue_తదుపరి;
	u_freeentry(uep, uep->ue_కొలత);
    }

#నిర్వచితమైతే U_DEBUG
    uhp->uh_magic = 0;
#అయితే_ముగింపు
    vim_free((char_u *)uhp);
    --buf->b_u_numhead;
}

/*
 * free entry 'uep' and 'n' lines in uep->ue_array[]
 */
    స్థిర శూన్య
u_freeentry(u_నమోదు_T *uep, దీర్ఘ n)
{
    అయితే (n > 0)
	vim_free(uep->ue_array[--n].ul_పంక్తి);
    vim_free((char_u *)uep->ue_array);
#నిర్వచితమైతే FEAT_PROP_POPUP
    ga_clear(&uep->ue_props);
#అయితే_ముగింపు
#నిర్వచితమైతే U_DEBUG
    uep->ue_magic = 0;
#అయితే_ముగింపు
    vim_free((char_u *)uep);
}

/*
 * invalidate the undo buffer; called when storage has already been released
 */
    స్థిర శూన్య
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = ఖాళీ;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_పంక్తి_ptr.ul_పంక్తి = ఖాళీ;
    buf->b_u_పంక్తి_ptr.ul_len = 0;
    buf->b_u_పంక్తి_ptr.ul_textlen = 0;
    buf->b_u_పంక్తి_lnum = 0;
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 */
    స్థిర శూన్య
u_blockfree(buf_T *buf)
{
    అయితే (buf->b_u_oldhead != ఖాళీ)
	u_freeheader(buf, buf->b_u_oldhead, ఖాళీ);
    vim_free(buf->b_u_పంక్తి_ptr.ul_పంక్తి);
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 * and invalidate the undo buffer
 */
    శూన్య
u_clearallandblockfree(buf_T *buf)
{
    u_blockfree(buf);
    u_clearall(buf);
}



/*
 * Save the line "lnum" for the "U" command.
 */
    స్థిర శూన్య
u_saveline(linenr_T lnum)
{
    ఒకవేళ (lnum == curbuf->b_u_పంక్తి_lnum)	    // line is already saved
	తిరిగి;
    ఒకవేళ (lnum < 1 || lnum > curbuf->b_ml.ml_పంక్తి_లెక్క) // should never happen
	తిరిగి;
    u_clearline();
    curbuf->b_u_పంక్తి_lnum = lnum;
    ఒకవేళ (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_పంక్తి_colnr = curwin->w_cursor.నిలువు;
    లేకపోతే
	curbuf->b_u_పంక్తి_colnr = 0;
    ఒకవేళ (u_save_పంక్తి(&curbuf->b_u_పంక్తి_ptr, lnum) == FAIL)
	do_outofmem_సందేశం((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    శూన్య
u_clearline(శూన్య)
{
    ఒకవేళ (curbuf->b_u_పంక్తి_ptr.ul_పంక్తి == ఖాళీ)
	తిరిగి;

    VIM_CLEAR(curbuf->b_u_పంక్తి_ptr.ul_పంక్తి);
    curbuf->b_u_పంక్తి_ptr.ul_len = 0;
    curbuf->b_u_పంక్తి_ptr.ul_textlen = 0;
    curbuf->b_u_పంక్తి_lnum = 0;
}

/*
 * Implementation of the "U" command.
 * Differentiation from vi: "U" can be undone with the next "U".
 * We also allow the cursor to be in another line.
 * Careful: may trigger autocommands that reload the buffer.
 */
    శూన్య
u_undoline(శూన్య)
{
    colnr_T	t;
    undoline_T  oldp;

    ఒకవేళ (undo_off)
	తిరిగి;

    ఒకవేళ (curbuf->b_u_పంక్తి_ptr.ul_పంక్తి == ఖాళీ
			|| curbuf->b_u_పంక్తి_lnum > curbuf->b_ml.ml_పంక్తి_లెక్క)
    {
	beep_flush();
	తిరిగి;
    }

    // first save the line for the 'u' command
    ఒకవేళ (u_savecommon(curbuf->b_u_పంక్తి_lnum - 1,
		       curbuf->b_u_పంక్తి_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	తిరిగి;
    ఒకవేళ (u_save_పంక్తి(&oldp, curbuf->b_u_పంక్తి_lnum) == FAIL)
    {
	do_outofmem_సందేశం((long_u)0);
	తిరిగి;
    }
    ml_replace_len(curbuf->b_u_పంక్తి_lnum, curbuf->b_u_పంక్తి_ptr.ul_పంక్తి,
				     curbuf->b_u_పంక్తి_ptr.ul_len, TRUE, FALSE);
    మారింది_బైట్లు(curbuf->b_u_పంక్తి_lnum, 0);
    curbuf->b_u_పంక్తి_ptr = oldp;

    t = curbuf->b_u_పంక్తి_colnr;
    ఒకవేళ (curwin->w_cursor.lnum == curbuf->b_u_పంక్తి_lnum)
	curbuf->b_u_పంక్తి_colnr = curwin->w_cursor.నిలువు;
    curwin->w_cursor.నిలువు = t;
    curwin->w_cursor.lnum = curbuf->b_u_పంక్తి_lnum;
    check_cursor_నిలువు();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    పూర్ణాంకం
buf_Is_మారింది(buf_T *buf)
{
#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (పదం_పని_running_not_none(buf->b_పదం))
	తిరిగి TRUE;
#అయితే_ముగింపు
    తిరిగి buf_Is_మారింది_Not_పదం(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    పూర్ణాంకం
any_Buf_Is_మారింది(శూన్య)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	ఒకవేళ (buf_Is_మారింది(buf))
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    పూర్ణాంకం
buf_Is_మారింది_Not_పదం(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    తిరిగి (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_మారింది || file_ff_differs(buf, TRUE));
}

    పూర్ణాంకం
curbuf_Is_మారింది(శూన్య)
{
    తిరిగి buf_Is_మారింది(curbuf);
}

#ఒకవేళ defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    స్థిర శూన్య
u_eval_వృక్షం(buf_T *buf, u_శీర్షిక_T *మొదటి_uhp, list_T *list)
{
    u_శీర్షిక_T  *uhp = మొదటి_uhp;
    dict_T	*dict;

    అయితే (uhp != ఖాళీ)
    {
	dict = dict_alloc();
	ఒకవేళ (dict == ఖాళీ)
	    తిరిగి;
	dict_కూడు_సంఖ్య(dict, "seq", uhp->uh_seq);
	dict_కూడు_సంఖ్య(dict, "time", (దీర్ఘ)uhp->uh_సమయం);
	ఒకవేళ (uhp == buf->b_u_newhead)
	    dict_కూడు_సంఖ్య(dict, "newhead", 1);
	ఒకవేళ (uhp == buf->b_u_curhead)
	    dict_కూడు_సంఖ్య(dict, "curhead", 1);
	ఒకవేళ (uhp->uh_save_nr > 0)
	    dict_కూడు_సంఖ్య(dict, "save", uhp->uh_save_nr);

	ఒకవేళ (uhp->uh_alt_తదుపరి.ptr != ఖాళీ)
	{
	    list_T	*alt_list = list_alloc();

	    ఒకవేళ (alt_list != ఖాళీ)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_వృక్షం(buf, uhp->uh_alt_తదుపరి.ptr, alt_list);
		dict_కూడు_list(dict, "alt", alt_list);
	    }
	}

	list_append_dict(list, dict);
	uhp = uhp->uh_prev.ptr;
    }
}

/*
 * "undofile(name)" function
 */
    శూన్య
f_undofile(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	ఒకవేళ (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = ఖాళీ;
	}
	లేకపోతే
	{
	    char_u *ffname = నిండిన_Name_save(fname, TRUE);

	    ఒకవేళ (ffname != ఖాళీ)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# లేకపోతే
    rettv->vval.v_string = ఖాళీ;
# endif
}
# ifdef FEAT_PERSISTENT_UNDO
/*
 * Reset undofile option and delete the undofile
 */
    శూన్య
u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    ఒకవేళ (!buf->b_p_udf)
	తిరిగి;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    ఒకవేళ (file_name != ఖాళీ)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    సమితి_option_విలువ_give_err((char_u *)"undofile", 0L, ఖాళీ, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    శూన్య
f_undotree(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_opt_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    dict_T *dict = rettv->vval.v_dict;

    dict_కూడు_సంఖ్య(dict, "synced", (దీర్ఘ)buf->b_u_synced);
    dict_కూడు_సంఖ్య(dict, "seq_last", buf->b_u_seq_చివరి);
    dict_కూడు_సంఖ్య(dict, "save_last", buf->b_u_save_nr_చివరి);
    dict_కూడు_సంఖ్య(dict, "seq_cur", buf->b_u_seq_cur);
    dict_కూడు_సంఖ్య(dict, "time_cur", (దీర్ఘ)buf->b_u_సమయం_cur);
    dict_కూడు_సంఖ్య(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    ఒకవేళ (list != ఖాళీ)
    {
	u_eval_వృక్షం(buf, buf->b_u_oldhead, list);
	dict_కూడు_list(dict, "entries", list);
    }
}

#అయితే_ముగింపు
