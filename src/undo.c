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

#अन्तर्भाव "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
प्रकारव्याख्या संरचना {
    buf_T	*bi_buf;
    खाता	*bi_fp;
#ifdef FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_buffer; // CRYPT_BUF_SIZE, NULL when not buffering
    माप_प्रकार	bi_used;    // bytes written to/read from bi_buffer
    माप_प्रकार	bi_avail;   // bytes available in bi_buffer
#endif
} bufinfo_T;


स्थिर शून्य u_unch_branch(u_header_T *uhp);
स्थिर u_entry_T *u_get_headentry(शून्य);
स्थिर शून्य u_getbot(शून्य);
स्थिर शून्य u_doit(पूर्णांक मोजणी);
स्थिर शून्य u_undoredo(पूर्णांक undo);
स्थिर शून्य u_undo_end(पूर्णांक did_undo, पूर्णांक absolute);
स्थिर शून्य u_freeheader(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
स्थिर शून्य u_freebranch(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
स्थिर शून्य u_freeentries(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
स्थिर शून्य u_freeentry(u_entry_T *, दीर्घ);
#ifdef FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
स्थिर पूर्णांक undo_flush(bufinfo_T *bi);
# endif
स्थिर पूर्णांक undo_read(bufinfo_T *bi, char_u *buffer, माप_प्रकार size);
स्थिर पूर्णांक serialize_uep(bufinfo_T *bi, u_entry_T *uep);
स्थिर u_entry_T *unserialize_uep(bufinfo_T *bi, पूर्णांक *error, char_u *file_name);
स्थिर शून्य serialize_pos(bufinfo_T *bi, pos_T pos);
स्थिर शून्य unserialize_pos(bufinfo_T *bi, pos_T *pos);
स्थिर शून्य serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
स्थिर शून्य unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#endif
स्थिर शून्य u_saveline(linenr_T lnum);
स्थिर शून्य u_blockfree(buf_T *buf);

#define U_ALLOC_ओळ(size) lalloc(size, FALSE)

// used in undo_end() to report number of added and deleted lines
स्थिर दीर्घ	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
स्थिर पूर्णांक	undo_undoes = FALSE;

स्थिर पूर्णांक	lastmark = 0;

#जर defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
स्थिर पूर्णांक seen_b_u_curhead;
स्थिर पूर्णांक seen_b_u_newhead;
स्थिर पूर्णांक header_मोजणी;

    स्थिर शून्य
u_check_वृक्ष(u_header_T *uhp,
	u_header_T *exp_uh_next,
	u_header_T *exp_uh_alt_prev)
{
    u_entry_T *uep;

    जर (uhp == रिक्त)
	परत;
    ++header_मोजणी;
    जर (uhp == curbuf->b_u_curhead && ++seen_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	परत;
    }
    जर (uhp == curbuf->b_u_newhead && ++seen_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	परत;
    }

    जर (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    अन्यथा
    {
	// Check pointers back are correct.
	जर (uhp->uh_next.ptr != exp_uh_next)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_next, uhp->uh_next.ptr);
	}
	जर (uhp->uh_alt_prev.ptr != exp_uh_alt_prev)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_prev, uhp->uh_alt_prev.ptr);
	}

	// Check the undo tree at this header.
	साठी (uep = uhp->uh_entry; uep != रिक्त; uep = uep->ue_next)
	{
	    जर (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		थांबा;
	    }
	}

	// Check the next alt tree.
	u_check_वृक्ष(uhp->uh_alt_next.ptr, uhp->uh_next.ptr, uhp);

	// Check the next header in this branch.
	u_check_वृक्ष(uhp->uh_prev.ptr, uhp, रिक्त);
    }
}

    स्थिर शून्य
u_check(पूर्णांक newhead_may_be_NULL)
{
    seen_b_u_newhead = 0;
    seen_b_u_curhead = 0;
    header_मोजणी = 0;

    u_check_वृक्ष(curbuf->b_u_oldhead, रिक्त, रिक्त);

    जर (seen_b_u_newhead == 0 && curbuf->b_u_oldhead != रिक्त
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == रिक्त))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    जर (curbuf->b_u_curhead != रिक्त && seen_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    जर (header_मोजणी != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (दीर्घ)header_मोजणी, (दीर्घ)curbuf->b_u_numhead);
    }
}
#endif

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    पूर्णांक
u_save_cursor(शून्य)
{
    परत (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    पूर्णांक
u_save(linenr_T शीर्ष, linenr_T bot)
{
    जर (undo_off)
	परत OK;

    जर (शीर्ष >= bot || bot > curbuf->b_ml.ml_ओळ_मोजणी + 1)
	परत FAIL;	// rely on caller to give an error message

    जर (शीर्ष + 2 == bot)
	u_saveline((linenr_T)(शीर्ष + 1));

    परत (u_savecommon(शीर्ष, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    पूर्णांक
u_savesub(linenr_T lnum)
{
    जर (undo_off)
	परत OK;

    परत (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    पूर्णांक
u_inssub(linenr_T lnum)
{
    जर (undo_off)
	परत OK;

    परत (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    पूर्णांक
u_savedel(linenr_T lnum, दीर्घ nlines)
{
    जर (undo_off)
	परत OK;

    परत (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_ओळ_मोजणी ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    पूर्णांक
undo_allowed(शून्य)
{
    // Don't allow changes when 'modifiable' is off.
    जर (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	परत FALSE;
    }

#ifdef HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    जर (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	परत FALSE;
    }
#endif

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    जर (textlock != 0)
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	परत FALSE;
    }

    परत TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    स्थिर दीर्घ
get_undolevel(शून्य)
{
    जर (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	परत p_ul;
    परत curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    स्थिर पूर्णांक
u_save_ओळ(undoline_T *ul, linenr_T lnum)
{
    char_u *ओळ = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    जर (curbuf->b_ml.ml_ओळ_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_ओळ = vim_strsave((char_u *)"");
    }
    अन्यथा
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_ओळ_len;
	ul->ul_ओळ = vim_memsave(ओळ, ul->ul_len);
    }
    परत ul->ul_ओळ == रिक्त ? FAIL : OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    स्थिर पूर्णांक
has_prop_w_flags(linenr_T lnum, पूर्णांक flags)
{
    char_u  *props;
    पूर्णांक	    i;
    पूर्णांक	    proplen = get_text_props(curbuf, lnum, &props, FALSE);

    साठी (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * आकार prop, आकार prop);
	जर (prop.tp_flags & flags)
	    परत TRUE;
    }
    परत FALSE;
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
    पूर्णांक
u_savecommon(
    linenr_T	शीर्ष,
    linenr_T	bot,
    linenr_T	newbot,
    पूर्णांक		reload)
{
    linenr_T	lnum;
    दीर्घ	i;
    u_header_T	*uhp;
    u_header_T	*old_curhead;
    u_entry_T	*uep;
    u_entry_T	*prev_uep;
    दीर्घ	size;

    जर (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	जर (!undo_allowed())
	    परत FAIL;

#ifdef FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	जर (netbeans_active())
	{
	    जर (netbeans_is_guarded(शीर्ष, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		परत FAIL;
	    }
	    जर (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_in_read_only_files));
		परत FAIL;
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
	जर (bot > curbuf->b_ml.ml_ओळ_मोजणी + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_ओळ_मोजणी_changed_unexpectedly));
	    परत FAIL;
	}
    }

#ifdef U_DEBUG
    u_check(FALSE);
#endif

#ifdef FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    जर (bot - शीर्ष > 1)
    {
	जर (शीर्ष > 0 && has_prop_w_flags(शीर्ष + 1, TP_FLAG_CONT_PREV))
	    --शीर्ष;
	जर (bot <= curbuf->b_ml.ml_ओळ_मोजणी
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_NEXT))
	{
	    ++bot;
	    जर (newbot != 0)
		++newbot;
	}
    }
#endif

    size = bot - शीर्ष - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    जर (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	जर (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_ओळ(आकार(u_header_T));
	    जर (uhp == रिक्त)
		जा nomem;
#ifdef U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#endif
	}
	अन्यथा
	    uhp = रिक्त;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	जर (old_curhead != रिक्त)
	{
	    curbuf->b_u_newhead = old_curhead->uh_next.ptr;
	    curbuf->b_u_curhead = रिक्त;
	}

	/*
	 * free headers to keep the size right
	 */
	पर्यंत (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != रिक्त)
	{
	    u_header_T	    *uhfree = curbuf->b_u_oldhead;

	    जर (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    अन्यथा जर (uhfree->uh_alt_next.ptr == रिक्त)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    अन्यथा
	    {
		// Free the oldest alternate branch as a whole.
		पर्यंत (uhfree->uh_alt_next.ptr != रिक्त)
		    uhfree = uhfree->uh_alt_next.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#ifdef U_DEBUG
	    u_check(TRUE);
#endif
	}

	जर (uhp == रिक्त)		// no undo at all
	{
	    जर (old_curhead != रिक्त)
		u_freebranch(curbuf, old_curhead, रिक्त);
	    curbuf->b_u_synced = false;
	    परत OK;
	}

	uhp->uh_prev.ptr = रिक्त;
	uhp->uh_next.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_next.ptr = old_curhead;
	जर (old_curhead != रिक्त)
	{
	    uhp->uh_alt_prev.ptr = old_curhead->uh_alt_prev.ptr;
	    जर (uhp->uh_alt_prev.ptr != रिक्त)
		uhp->uh_alt_prev.ptr->uh_alt_next.ptr = uhp;
	    old_curhead->uh_alt_prev.ptr = uhp;
	    जर (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	अन्यथा
	    uhp->uh_alt_prev.ptr = रिक्त;
	जर (curbuf->b_u_newhead != रिक्त)
	    curbuf->b_u_newhead->uh_prev.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_last;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_time = vim_time();
	uhp->uh_save_nr = 0;
	curbuf->b_u_time_cur = uhp->uh_time + 1;

	uhp->uh_walk = 0;
	uhp->uh_entry = रिक्त;
	uhp->uh_getbot_entry = रिक्त;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	जर (virtual_active() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	अन्यथा
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_changed ? UH_CHANGED : 0) +
		       ((curbuf->b_ml.ml_flags & ML_EMPTY) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, आकार(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	जर (curbuf->b_u_oldhead == रिक्त)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    अन्यथा
    {
	जर (get_undolevel() < 0)	// no undo at all
	    परत OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	जर (size == 1)
	{
	    uep = u_get_headentry();
	    prev_uep = रिक्त;
	    साठी (i = 0; i < 10; ++i)
	    {
		जर (uep == रिक्त)
		    थांबा;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		जर ((curbuf->b_u_newhead->uh_getbot_entry != uep
			    ? (uep->ue_शीर्ष + uep->ue_size + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_ओळ_मोजणी + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_ओळ_मोजणी)
			|| (uep->ue_size > 1
			    && शीर्ष >= uep->ue_शीर्ष
			    && शीर्ष + 2 <= uep->ue_शीर्ष + uep->ue_size + 1))
		    थांबा;

		// If it's the same line we can skip saving it again.
		जर (uep->ue_size == 1 && uep->ue_शीर्ष == शीर्ष)
		{
		    जर (i > 0)
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
		    जर (newbot != 0)
			uep->ue_bot = newbot;
		    अन्यथा जर (bot > curbuf->b_ml.ml_ओळ_मोजणी)
			uep->ue_bot = 0;
		    अन्यथा
		    {
			uep->ue_lcount = curbuf->b_ml.ml_ओळ_मोजणी;
			curbuf->b_u_newhead->uh_getbot_entry = uep;
		    }
		    परत OK;
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
    uep = U_ALLOC_ओळ(आकार(u_entry_T));
    जर (uep == रिक्त)
	जा nomem;
    CLEAR_POINTER(uep);
#ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
#endif

    uep->ue_size = size;
    uep->ue_शीर्ष = शीर्ष;
    जर (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    अन्यथा जर (bot > curbuf->b_ml.ml_ओळ_मोजणी)
	uep->ue_bot = 0;
    अन्यथा
    {
	uep->ue_lcount = curbuf->b_ml.ml_ओळ_मोजणी;
	curbuf->b_u_newhead->uh_getbot_entry = uep;
    }

    जर (size > 0)
    {
	जर ((uep->ue_array = U_ALLOC_ओळ(आकार(undoline_T) * size)) == रिक्त)
	{
	    u_freeentry(uep, 0L);
	    जा nomem;
	}
	साठी (i = 0, lnum = शीर्ष + 1; i < size; ++i)
	{
	    fast_breakcheck();
	    जर (got_int)
	    {
		u_freeentry(uep, i);
		परत FAIL;
	    }
	    जर (u_save_ओळ(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		जा nomem;
	    }
	}
    }
    अन्यथा
	uep->ue_array = रिक्त;
    uep->ue_next = curbuf->b_u_newhead->uh_entry;
    curbuf->b_u_newhead->uh_entry = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#ifdef U_DEBUG
    u_check(FALSE);
#endif
    परत OK;

nomem:
    संदेश_silent = 0;	// must display the prompt
    जर (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	परत OK;
    }
    do_outofmem_संदेश((long_u)0);
    परत FAIL;
}

#जर defined(FEAT_PERSISTENT_UNDO)

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
    शून्य
u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;

    sha256_start(&ctx);
    साठी (lnum = 1; lnum <= curbuf->b_ml.ml_ओळ_मोजणी; ++lnum)
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
    स्थिर char_u *
u_get_undo_file_name(char_u *buf_ffname, पूर्णांक reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {रिक्त, 0};
    char_u	*undo_file_name = रिक्त;
    पूर्णांक		dir_len;
    char_u	*p;
    माप_प्रकार	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    माप_प्रकार	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    जर (ffname == रिक्त)
	परत रिक्त;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    जर (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    पर्यंत (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	जर (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    जर (undo_file_name == रिक्त)
		थांबा;
	    p = gettail(undo_file_name);
	    plen = (माप_प्रकार)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# अन्यथा
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	अन्यथा
	{
	    dir_name[dir_len] = NUL;
	    जर (mch_isdir(dir_name))
	    {
		string_T    ret;

		जर (munged_name.string == रिक्त)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    जर (munged_name.string == रिक्त)
			परत रिक्त;
		    munged_name.length = ffnamelen;
		    साठी (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			जर (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (माप_प्रकार)dir_len,
		    munged_name.string, munged_name.length, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	जर (undo_file_name != रिक्त && (!reading
			       || mch_stat((अक्षर *)undo_file_name, &st) >= 0))
	    थांबा;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    परत undo_file_name;
}

    स्थिर शून्य
corruption_error(अक्षर *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    स्थिर शून्य
u_free_uhp(u_header_T *uhp)
{
    u_entry_T	*nuep;
    u_entry_T	*uep;

    uep = uhp->uh_entry;
    पर्यंत (uep != रिक्त)
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
    स्थिर पूर्णांक
undo_write(bufinfo_T *bi, char_u *ptr, माप_प्रकार len)
{
# ifdef FEAT_CRYPT
    जर (bi->bi_buffer != रिक्त)
    {
	माप_प्रकार	len_todo = len;
	char_u  *p = ptr;

	पर्यंत (bi->bi_used + len_todo >= CRYPT_BUF_SIZE)
	{
	    माप_प्रकार	n = CRYPT_BUF_SIZE - bi->bi_used;

	    mch_memmove(bi->bi_buffer + bi->bi_used, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_used = CRYPT_BUF_SIZE;
	    जर (undo_flush(bi) == FAIL)
		परत FAIL;
	}
	जर (len_todo > 0)
	{
	    mch_memmove(bi->bi_buffer + bi->bi_used, p, len_todo);
	    bi->bi_used += len_todo;
	}
	परत OK;
    }
# endif
    जर (ख_डालो(ptr, len, (माप_प्रकार)1, bi->bi_fp) != 1)
	परत FAIL;
    परत OK;
}

# ifdef FEAT_CRYPT
    स्थिर पूर्णांक
undo_flush(bufinfo_T *bi)
{
    जर (bi->bi_buffer != रिक्त && bi->bi_state != रिक्त && bi->bi_used > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_used, FALSE);
	जर (ख_डालो(bi->bi_buffer, bi->bi_used, (माप_प्रकार)1, bi->bi_fp) != 1)
	    परत FAIL;
	bi->bi_used = 0;
    }
    परत OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णांक
fwrite_crypt(bufinfo_T *bi, char_u *ptr, माप_प्रकार len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    माप_प्रकार  i;

    जर (bi->bi_state != रिक्त && bi->bi_buffer == रिक्त)
    {
	// crypting every piece of text separately
	जर (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	अन्यथा
	{
	    copy = lalloc(len, FALSE);
	    जर (copy == रिक्त)
		परत 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = ख_डालो(copy, len, (माप_प्रकार)1, bi->bi_fp);
	जर (copy != small_buf)
	    vim_free(copy);
	परत i == 1 ? OK : FAIL;
    }
# endif
    परत undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णांक
undo_write_bytes(bufinfo_T *bi, long_u nr, पूर्णांक len)
{
    char_u  buf[8];
    पूर्णांक	    i;
    पूर्णांक	    bufi = 0;

    साठी (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    परत undo_write(bi, buf, (माप_प्रकार)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    स्थिर शून्य
put_header_ptr(bufinfo_T *bi, u_header_T *uhp)
{
    undo_write_bytes(bi, (long_u)(uhp != रिक्त ? uhp->uh_seq : 0), 4);
}

    स्थिर पूर्णांक
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    जर (bi->bi_buffer != रिक्त)
    {
	char_u  buf[4];
	पूर्णांक	n;

	undo_read(bi, buf, (माप_प्रकार)4);
	n = ((अचिन्हित)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	परत n;
    }
# endif
    परत get4c(bi->bi_fp);
}

    स्थिर पूर्णांक
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    जर (bi->bi_buffer != रिक्त)
    {
	char_u  buf[2];
	पूर्णांक	n;

	undo_read(bi, buf, (माप_प्रकार)2);
	n = (buf[0] << 8) + buf[1];
	परत n;
    }
# endif
    परत get2c(bi->bi_fp);
}

    स्थिर पूर्णांक
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    जर (bi->bi_buffer != रिक्त)
    {
	char_u  buf[1];

	undo_read(bi, buf, (माप_प्रकार)1);
	परत buf[0];
    }
# endif
    परत अ_लो(bi->bi_fp);
}

    स्थिर समय_प्रकार
undo_read_time(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    जर (bi->bi_buffer != रिक्त)
    {
	char_u  buf[8];
	समय_प्रकार	n = 0;
	पूर्णांक	i;

	undo_read(bi, buf, (माप_प्रकार)8);
	साठी (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	परत n;
    }
# endif
    परत get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    स्थिर पूर्णांक
undo_read(bufinfo_T *bi, char_u *buffer, माप_प्रकार size)
{
    पूर्णांक retval = OK;

# ifdef FEAT_CRYPT
    जर (bi->bi_buffer != रिक्त)
    {
	पूर्णांक	size_todo = (पूर्णांक)size;
	char_u	*p = buffer;

	पर्यंत (size_todo > 0)
	{
	    माप_प्रकार n;

	    जर (bi->bi_used >= bi->bi_avail)
	    {
		n = ख_पढ़ो(bi->bi_buffer, 1, (माप_प्रकार)CRYPT_BUF_SIZE, bi->bi_fp);
		जर (n == 0)
		{
		    retval = FAIL;
		    थांबा;
		}
		bi->bi_avail = n;
		bi->bi_used = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_avail, FALSE);
	    }
	    n = size_todo;
	    जर (n > bi->bi_avail - bi->bi_used)
		n = bi->bi_avail - bi->bi_used;
	    mch_memmove(p, bi->bi_buffer + bi->bi_used, n);
	    bi->bi_used += n;
	    size_todo -= (पूर्णांक)n;
	    p += n;
	}
    }
    अन्यथा
# endif
    जर (ख_पढ़ो(buffer, size, 1, bi->bi_fp) != 1)
	retval = FAIL;

    जर (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(buffer, 0, size);
    परत retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    स्थिर char_u *
read_string_decrypt(bufinfo_T *bi, पूर्णांक len)
{
    char_u  *ptr = alloc(len + 1);

    जर (ptr == रिक्त)
	परत रिक्त;

    जर (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	परत रिक्त;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    जर (bi->bi_state != रिक्त && bi->bi_buffer == रिक्त)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    परत ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    स्थिर पूर्णांक
serialize_header(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    खाता	*fp = bi->bi_fp;
    char_u	time_buf[8];

    // Start writing, first the magic marker and undo info version.
    जर (ख_डालो(UF_START_MAGIC, (माप_प्रकार)UF_START_MAGIC_LEN, (माप_प्रकार)1, fp) != 1)
	परत FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    जर (*buf->b_p_किल्ली != NUL)
    {
	char_u *header;
	पूर्णांक    header_len;
	दीर्घ	len;

	undo_write_bytes(bi, (long_u)UF_VERSION_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_method_nr(buf),
					  buf->b_p_किल्ली, &header, &header_len);
	जर (bi->bi_state == रिक्त)
	    परत FAIL;
	len = (दीर्घ)ख_डालो(header, (माप_प्रकार)header_len, (माप_प्रकार)1, fp);
	vim_free(header);
	जर (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = रिक्त;
	    परत FAIL;
	}

	जर (crypt_whole_undofile(crypt_get_method_nr(buf)))
	{
	    bi->bi_buffer = alloc(CRYPT_BUF_SIZE);
	    जर (bi->bi_buffer == रिक्त)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = रिक्त;
		परत FAIL;
	    }
	    bi->bi_used = 0;
	}
    }
    अन्यथा
# endif
	undo_write_bytes(bi, (long_u)UF_VERSION, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    जर (undo_write(bi, hash, (माप_प्रकार)UNDO_HASH_SIZE) == FAIL)
	परत FAIL;

    // buffer-specific data
    undo_write_bytes(bi, (long_u)buf->b_ml.ml_ओळ_मोजणी, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_ओळ_ptr.ul_textlen, 4);
    जर (buf->b_u_ओळ_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_ओळ_ptr.ul_ओळ,
			(माप_प्रकार)buf->b_u_ओळ_ptr.ul_textlen) == FAIL)
	परत FAIL;
    undo_write_bytes(bi, (long_u)buf->b_u_ओळ_lnum, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_ओळ_colnr, 4);

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

    परत OK;
}

    स्थिर पूर्णांक
serialize_uhp(bufinfo_T *bi, u_header_T *uhp)
{
    पूर्णांक		i;
    u_entry_T	*uep;
    char_u	time_buf[8];

    जर (undo_write_bytes(bi, (long_u)UF_HEADER_MAGIC, 2) == FAIL)
	परत FAIL;

    put_header_ptr(bi, uhp->uh_next.ptr);
    put_header_ptr(bi, uhp->uh_prev.ptr);
    put_header_ptr(bi, uhp->uh_alt_next.ptr);
    put_header_ptr(bi, uhp->uh_alt_prev.ptr);
    undo_write_bytes(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_bytes(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_bytes(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    साठी (i = 0; i < NMARKS; ++i)
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
    साठी (uep = uhp->uh_entry; uep != रिक्त; uep = uep->ue_next)
    {
	undo_write_bytes(bi, (long_u)UF_ENTRY_MAGIC, 2);
	जर (serialize_uep(bi, uep) == FAIL)
	    परत FAIL;
    }
    undo_write_bytes(bi, (long_u)UF_ENTRY_END_MAGIC, 2);
    परत OK;
}

    स्थिर u_header_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_header_T	*uhp;
    पूर्णांक		i;
    u_entry_T	*uep, *last_uep;
    पूर्णांक		c;
    पूर्णांक		error;

    uhp = U_ALLOC_ओळ(आकार(u_header_T));
    जर (uhp == रिक्त)
	परत रिक्त;
    CLEAR_POINTER(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_next.seq = undo_read_4c(bi);
    uhp->uh_prev.seq = undo_read_4c(bi);
    uhp->uh_alt_next.seq = undo_read_4c(bi);
    uhp->uh_alt_prev.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    जर (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	परत रिक्त;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    साठी (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_time = undo_read_time(bi);

    // Optional fields.
    साठी (;;)
    {
	पूर्णांक len = undo_read_byte(bi);
	पूर्णांक what;

	जर (len == खातापूर्ण)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    परत रिक्त;
	}
	जर (len == 0)
	    थांबा;
	what = undo_read_byte(bi);
	निवड (what)
	{
	    स्थिती UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		थांबा;
	    पूर्वनिर्धारित:
		// field not supported, skip
		पर्यंत (--len >= 0)
		    (शून्य)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    last_uep = रिक्त;
    पर्यंत ((c = undo_read_2c(bi)) == UF_ENTRY_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	जर (last_uep == रिक्त)
	    uhp->uh_entry = uep;
	अन्यथा
	    last_uep->ue_next = uep;
	last_uep = uep;
	जर (uep == रिक्त || error)
	{
	    u_free_uhp(uhp);
	    परत रिक्त;
	}
    }
    जर (c != UF_ENTRY_END_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	परत रिक्त;
    }

    परत uhp;
}

/*
 * Serialize "uep".
 */
    स्थिर पूर्णांक
serialize_uep(
    bufinfo_T	*bi,
    u_entry_T	*uep)
{
    पूर्णांक		i;

    undo_write_bytes(bi, (long_u)uep->ue_शीर्ष, 4);
    undo_write_bytes(bi, (long_u)uep->ue_bot, 4);
    undo_write_bytes(bi, (long_u)uep->ue_lcount, 4);
    undo_write_bytes(bi, (long_u)uep->ue_size, 4);
    साठी (i = 0; i < uep->ue_size; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	जर (undo_write_bytes(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    परत FAIL;
	जर (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_ओळ, uep->ue_array[i].ul_textlen) == FAIL)
	    परत FAIL;
    }
    परत OK;
}

    स्थिर u_entry_T *
unserialize_uep(bufinfo_T *bi, पूर्णांक *error, char_u *file_name)
{
    पूर्णांक		i;
    u_entry_T	*uep;
    undoline_T	*array = रिक्त;
    char_u	*ओळ;
    पूर्णांक		ओळ_len;

    uep = U_ALLOC_ओळ(आकार(u_entry_T));
    जर (uep == रिक्त)
	परत रिक्त;
    CLEAR_POINTER(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_शीर्ष = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_size = undo_read_4c(bi);
    जर (uep->ue_size > 0)
    {
	जर (uep->ue_size < दीर्घ_उच्च / (दीर्घ)आकार(undoline_T))
	    array = U_ALLOC_ओळ(आकार(undoline_T) * uep->ue_size);
	जर (array == रिक्त)
	{
	    *error = TRUE;
	    परत uep;
	}
	vim_memset(array, 0, आकार(undoline_T) * uep->ue_size);
    }
    uep->ue_array = array;

    साठी (i = 0; i < uep->ue_size; ++i)
    {
	ओळ_len = undo_read_4c(bi);
	जर (ओळ_len >= 0)
	    ओळ = read_string_decrypt(bi, ओळ_len);
	अन्यथा
	{
	    ओळ = रिक्त;
	    corruption_error("line length", file_name);
	}
	जर (ओळ == रिक्त)
	{
	    *error = TRUE;
	    परत uep;
	}
	array[i].ul_ओळ = ओळ;
	array[i].ul_len = ओळ_len + 1;
	array[i].ul_textlen = ओळ_len;
    }
    परत uep;
}

/*
 * Serialize "pos".
 */
    स्थिर शून्य
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_bytes(bi, (long_u)pos.lnum, 4);
    undo_write_bytes(bi, (long_u)pos.col, 4);
    undo_write_bytes(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    स्थिर शून्य
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    जर (pos->lnum < 0)
	pos->lnum = 0;
    pos->col = undo_read_4c(bi);
    जर (pos->col < 0)
	pos->col = 0;
    pos->coladd = undo_read_4c(bi);
    जर (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    स्थिर शून्य
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
    स्थिर शून्य
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
    शून्य
u_write_undo(
    char_u	*name,
    पूर्णांक		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_header_T	*uhp;
    char_u	*file_name;
    पूर्णांक		mark;
# ifdef U_DEBUG
    पूर्णांक		headers_written = 0;
# endif
    पूर्णांक		fd;
    खाता	*fp = रिक्त;
    पूर्णांक		perm;
    पूर्णांक		write_ok = FALSE;
# ifdef UNIX
    पूर्णांक		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);

    जर (name == रिक्त)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	जर (file_name == रिक्त)
	{
	    जर (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    परत;
	}
    }
    अन्यथा
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    जर (buf->b_ffname != रिक्त)
    {
# ifdef UNIX
	जर (mch_stat((अक्षर *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_mode;
	    st_old_valid = TRUE;
	}
# अन्यथा
	perm = mch_getperm(buf->b_ffname);
	जर (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    जर (mch_getperm(file_name) >= 0)
    {
	जर (name == रिक्त || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((अक्षर *)file_name, O_RDONLY|O_EXTRA, 0);
	    जर (fd < 0)
	    {
		जर (name != रिक्त || p_verbose > 0)
		{
		    जर (name == रिक्त)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    जर (name == रिक्त)
			verbose_leave();
		}
		जा theend;
	    }
	    अन्यथा
	    {
		char_u	mbuf[UF_START_MAGIC_LEN];
		पूर्णांक	len;

		len = read_eintr(fd, mbuf, UF_START_MAGIC_LEN);
		बंद(fd);
		जर (len < UF_START_MAGIC_LEN
		      || स_भेद(mbuf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
		{
		    जर (name != रिक्त || p_verbose > 0)
		    {
			जर (name == रिक्त)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			जर (name == रिक्त)
			    verbose_leave();
		    }
		    जा theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    जर (buf->b_u_numhead == 0 && buf->b_u_ओळ_ptr.ul_ओळ == रिक्त)
    {
	जर (p_verbose > 0)
	    verb_संदेश(_("Skipping undo file write, nothing to undo"));
	जा theend;
    }

    fd = mch_open((अक्षर *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    जर (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	जा theend;
    }
    (शून्य)mch_setperm(file_name, perm);
    जर (p_verbose > 0)
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
    जर (st_old_valid
	    && mch_stat((अक्षर *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  जर defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    जर (buf->b_ffname != रिक्त)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    जर (fp == रिक्त)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	बंद(fd);
	mch_remove(file_name);
	जा theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    जर (serialize_header(&bi, hash) == FAIL)
	जा write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    पर्यंत (uhp != रिक्त)
    {
	// Serialize current UHP if we haven't seen it
	जर (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++headers_written;
# endif
	    जर (serialize_uhp(&bi, uhp) == FAIL)
		जा write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	जर (uhp->uh_prev.ptr != रिक्त && uhp->uh_prev.ptr->uh_walk != mark)
	    uhp = uhp->uh_prev.ptr;
	अन्यथा जर (uhp->uh_alt_next.ptr != रिक्त
				     && uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;
	अन्यथा जर (uhp->uh_next.ptr != रिक्त && uhp->uh_alt_prev.ptr == रिक्त
					 && uhp->uh_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_next.ptr;
	अन्यथा जर (uhp->uh_alt_prev.ptr != रिक्त)
	    uhp = uhp->uh_alt_prev.ptr;
	अन्यथा
	    uhp = uhp->uh_next.ptr;
    }

    जर (undo_write_bytes(&bi, (long_u)UF_HEADER_END_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    जर (headers_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", headers_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    जर (bi.bi_state != रिक्त && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# जर defined(UNIX) && defined(HAVE_FSYNC)
    जर ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && ख_साफ(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    ख_बंद(fp);
    जर (!write_ok)
	semsg(_(e_write_error_in_undo_file_str), file_name);

# जर defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    जर (buf->b_ffname != रिक्त)
	(शून्य)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    जर (buf->b_ffname != रिक्त)
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
    जर (bi.bi_state != रिक्त)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
# endif
    जर (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    स्थिर पूर्णांक
uhp_seq_cmp(स्थिरांक शून्य *v1, स्थिरांक शून्य *v2)
{
    स्थिरांक u_header_T *u1 = *(u_header_T **)v1;
    स्थिरांक u_header_T *u2 = *(u_header_T **)v2;

    परत u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    स्थिर दीर्घ
uhp_table_find(u_header_T **uhp_table, दीर्घ अंक_head, दीर्घ seq)
{
    दीर्घ    lo = 0;
    दीर्घ    hi = अंक_head - 1;

    पर्यंत (lo <= hi)
    {
	दीर्घ mid = lo + (hi - lo) / 2;

	जर (uhp_table[mid]->uh_seq < seq)
	    lo = mid + 1;
	अन्यथा जर (uhp_table[mid]->uh_seq > seq)
	    hi = mid - 1;
	अन्यथा
	    परत mid;
    }
    परत -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    शून्य
u_read_undo(char_u *name, char_u *hash, char_u *orig_name UNUSED)
{
    char_u	*file_name;
    खाता	*fp;
    दीर्घ	version, str_len;
    undoline_T	ओळ_ptr;
    linenr_T	ओळ_lnum;
    colnr_T	ओळ_colnr;
    linenr_T	ओळ_मोजणी;
    दीर्घ	अंक_head = 0;
    दीर्घ	old_header_seq, new_header_seq, cur_header_seq;
    दीर्घ	seq_last, seq_cur;
    दीर्घ	last_save_nr = 0;
    दीर्घ	old_idx = -1, new_idx = -1, cur_idx = -1;
    दीर्घ	अंक_read_uhps = 0;
    समय_प्रकार	seq_time;
    दीर्घ	i;
    पूर्णांक		c;
    u_header_T	*uhp;
    u_header_T	**uhp_table = रिक्त;
    char_u	read_hash[UNDO_HASH_SIZE];
    char_u	magic_buf[UF_START_MAGIC_LEN];
# ifdef U_DEBUG
    पूर्णांक		*uhp_table_used;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);
    ओळ_ptr.ul_len = 0;
    ओळ_ptr.ul_textlen = 0;
    ओळ_ptr.ul_ओळ = रिक्त;

    जर (name == रिक्त)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	जर (file_name == रिक्त)
	    परत;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	जर (mch_stat((अक्षर *)orig_name, &st_orig) >= 0
		&& mch_stat((अक्षर *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    जर (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    परत;
	}
# endif
    }
    अन्यथा
	file_name = name;

    जर (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((अक्षर *)file_name, "r");
    जर (fp == रिक्त)
    {
	जर (name != रिक्त || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	जा error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    जर (ख_पढ़ो(magic_buf, UF_START_MAGIC_LEN, 1, fp) != 1
		|| स_भेद(magic_buf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	जा error;
    }
    version = get2c(fp);
    जर (version == UF_VERSION_CRYPT)
    {
# ifdef FEAT_CRYPT
	जर (*curbuf->b_p_किल्ली == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    जा error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_किल्ली);
	जर (bi.bi_state == रिक्त)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    जा error;
	}
	जर (crypt_whole_undofile(bi.bi_state->method_nr))
	{
	    bi.bi_buffer = alloc(CRYPT_BUF_SIZE);
	    जर (bi.bi_buffer == रिक्त)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = रिक्त;
		जा error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_used = 0;
	}
# अन्यथा
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	जा error;
# endif
    }
    अन्यथा जर (version != UF_VERSION)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	जा error;
    }

    जर (undo_read(&bi, read_hash, (माप_प्रकार)UNDO_HASH_SIZE) == FAIL)
    {
	corruption_error("hash", file_name);
	जा error;
    }
    ओळ_मोजणी = (linenr_T)undo_read_4c(&bi);
    जर (स_भेद(hash, read_hash, UNDO_HASH_SIZE) != 0
				  || ओळ_मोजणी != curbuf->b_ml.ml_ओळ_मोजणी)
    {
	जर (p_verbose > 0 || name != रिक्त)
	{
	    जर (name == रिक्त)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    जर (name == रिक्त)
		verbose_leave();
	}
	जा error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    जर (str_len < 0)
	जा error;
    जर (str_len > 0)
    {
	ओळ_ptr.ul_ओळ = read_string_decrypt(&bi, str_len);
	ओळ_ptr.ul_len = str_len + 1;
	ओळ_ptr.ul_textlen = str_len;
    }
    ओळ_lnum = (linenr_T)undo_read_4c(&bi);
    ओळ_colnr = (colnr_T)undo_read_4c(&bi);
    जर (ओळ_lnum < 0 || ओळ_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	जा error;
    }

    // Begin general undo data
    old_header_seq = undo_read_4c(&bi);
    new_header_seq = undo_read_4c(&bi);
    cur_header_seq = undo_read_4c(&bi);
    अंक_head = undo_read_4c(&bi);
    seq_last = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_time = undo_read_time(&bi);

    // Optional header fields.
    साठी (;;)
    {
	पूर्णांक len = undo_read_byte(&bi);
	पूर्णांक what;

	जर (len == 0 || len == खातापूर्ण)
	    थांबा;
	what = undo_read_byte(&bi);
	निवड (what)
	{
	    स्थिती UF_LAST_SAVE_NR:
		last_save_nr = undo_read_4c(&bi);
		थांबा;
	    पूर्वनिर्धारित:
		// field not supported, skip
		पर्यंत (--len >= 0)
		    (शून्य)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    जर (अंक_head > 0)
    {
	जर (अंक_head < दीर्घ_उच्च / (दीर्घ)आकार(u_header_T *))
	    uhp_table = U_ALLOC_ओळ(अंक_head * आकार(u_header_T *));
	जर (uhp_table == रिक्त)
	    जा error;
    }

    पर्यंत ((c = undo_read_2c(&bi)) == UF_HEADER_MAGIC)
    {
	जर (अंक_read_uhps >= अंक_head)
	{
	    corruption_error("num_head too small", file_name);
	    जा error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	जर (uhp == रिक्त)
	    जा error;
	uhp_table[अंक_read_uhps++] = uhp;
    }

    जर (अंक_read_uhps != अंक_head)
    {
	corruption_error("num_head", file_name);
	जा error;
    }
    जर (c != UF_HEADER_END_MAGIC)
    {
	corruption_error("end marker", file_name);
	जा error;
    }

# ifdef U_DEBUG
    uhp_table_used = alloc_clear(आकार(पूर्णांक) * अंक_head + 1);
#  define SET_FLAG(j) ++uhp_table_used[j]
# अन्यथा
#  define SET_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    जर (अंक_head > 0)
	क्विक(uhp_table, (माप_प्रकार)अंक_head, आकार(u_header_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    साठी (i = 0; i < अंक_head - 1; i++)
    {
	जर (uhp_table[i]->uh_seq == uhp_table[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    जा error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    करा { \
	दीर्घ fidx = uhp_table_find(uhp_table, अंक_head, (link).seq); \
	\
	जर (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_table[fidx]; \
	    SET_FLAG(fidx); \
	} \
	अन्यथा \
	    (link).ptr = रिक्त; \
    } पर्यंत (0)

    साठी (i = 0; i < अंक_head; i++)
    {
	uhp = uhp_table[i];
	SWIZZLE_SEQ(uhp->uh_next, i);
	SWIZZLE_SEQ(uhp->uh_prev, i);
	SWIZZLE_SEQ(uhp->uh_alt_next, i);
	SWIZZLE_SEQ(uhp->uh_alt_prev, i);
	जर (old_header_seq > 0 && old_idx < 0 && uhp->uh_seq == old_header_seq)
	{
	    old_idx = i;
	    SET_FLAG(i);
	}
	जर (new_header_seq > 0 && new_idx < 0 && uhp->uh_seq == new_header_seq)
	{
	    new_idx = i;
	    SET_FLAG(i);
	}
	जर (cur_header_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_header_seq)
	{
	    cur_idx = i;
	    SET_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? रिक्त : uhp_table[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? रिक्त : uhp_table[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? रिक्त : uhp_table[cur_idx];
    curbuf->b_u_ओळ_ptr = ओळ_ptr;
    curbuf->b_u_ओळ_lnum = ओळ_lnum;
    curbuf->b_u_ओळ_colnr = ओळ_colnr;
    curbuf->b_u_numhead = अंक_head;
    curbuf->b_u_seq_last = seq_last;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_time_cur = seq_time;
    curbuf->b_u_save_nr_last = last_save_nr;
    curbuf->b_u_save_nr_cur = last_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_table);

# ifdef U_DEBUG
    साठी (i = 0; i < अंक_head; ++i)
	जर (uhp_table_used[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_table_used);
    u_check(TRUE);
# endif

    जर (name != रिक्त)
	smsg(_("Finished reading undo file %s"), file_name);
    जा theend;

error:
    vim_free(ओळ_ptr.ul_ओळ);
    जर (uhp_table != रिक्त)
    {
	साठी (i = 0; i < अंक_read_uhps; i++)
	    जर (uhp_table[i] != रिक्त)
		u_free_uhp(uhp_table[i]);
	vim_free(uhp_table);
    }

theend:
# ifdef FEAT_CRYPT
    जर (bi.bi_state != रिक्त)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
# endif
    जर (fp != रिक्त)
	ख_बंद(fp);
    जर (file_name != name)
	vim_free(file_name);
    परत;
}

#endif // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    शून्य
u_undo(पूर्णांक मोजणी)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    जर (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	मोजणी = 1;
    }

    जर (vim_strchr(p_cpo, CPO_UNDO) == रिक्त)
	undo_undoes = TRUE;
    अन्यथा
	undo_undoes = !undo_undoes;
    u_doit(मोजणी);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    शून्य
u_redo(पूर्णांक मोजणी)
{
    जर (vim_strchr(p_cpo, CPO_UNDO) == रिक्त)
	undo_undoes = FALSE;
    u_doit(मोजणी);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    स्थिर शून्य
u_doit(पूर्णांक startcount)
{
    पूर्णांक मोजणी = startcount;

    जर (!undo_allowed())
	परत;

    u_newcount = 0;
    u_oldcount = 0;
    जर (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;
    पर्यंत (मोजणी--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	जर (undo_undoes)
	{
	    जर (curbuf->b_u_curhead == रिक्त)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    अन्यथा जर (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_next.ptr;
	    // nothing to undo
	    जर (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == रिक्त)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		जर (मोजणी == startcount - 1)
		{
		    जर (!shortmess(SHM_UNDO))
			संदेश(_("Already at oldest change"));
		    परत;
		}
		थांबा;
	    }

	    u_undoredo(TRUE);
	}
	अन्यथा
	{
	    जर (curbuf->b_u_curhead == रिक्त || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		जर (मोजणी == startcount - 1)
		{
		    जर (!shortmess(SHM_UNDO))
			संदेश(_("Already at newest change"));
		    परत;
		}
		थांबा;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    जर (curbuf->b_u_curhead->uh_prev.ptr == रिक्त)
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
    शून्य
undo_time(
    दीर्घ	step,
    पूर्णांक		sec,
    पूर्णांक		file,
    पूर्णांक		absolute)
{
    दीर्घ	    target;
    दीर्घ	    closest;
    दीर्घ	    closest_start;
    दीर्घ	    closest_seq = 0;
    दीर्घ	    val;
    u_header_T	    *uhp = रिक्त;
    u_header_T	    *last;
    पूर्णांक		    mark;
    पूर्णांक		    nomark = 0;  // shut up compiler
    पूर्णांक		    round;
    पूर्णांक		    dosec = sec;
    पूर्णांक		    dofile = file;
    पूर्णांक		    above = FALSE;
    पूर्णांक		    did_undo = TRUE;

    जर (text_locked())
    {
	text_locked_संदेश();
	परत;
    }

    // First make sure the current undoable change is synced.
    जर (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    जर (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    जर (absolute)
    {
	target = step;
	closest = -1;
    }
    अन्यथा
    {
	जर (dosec)
	    target = (दीर्घ)(curbuf->b_u_time_cur) + step;
	अन्यथा जर (dofile)
	{
	    जर (step < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		जर (uhp != रिक्त)
		    uhp = uhp->uh_next.ptr;
		अन्यथा
		    uhp = curbuf->b_u_newhead;
		जर (uhp != रिक्त && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    target = curbuf->b_u_save_nr_cur + step;
		अन्यथा
		    // count the changes since the last write as one step
		    target = curbuf->b_u_save_nr_cur + step + 1;
		जर (target <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    अन्यथा
	    {
		// Moving forward to a newer write.
		target = curbuf->b_u_save_nr_cur + step;
		जर (target > curbuf->b_u_save_nr_last)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    target = curbuf->b_u_seq_last + 1;
		    dofile = FALSE;
		}
	    }
	}
	अन्यथा
	    target = curbuf->b_u_seq_cur + step;
	जर (step < 0)
	{
	    जर (target < 0)
		target = 0;
	    closest = -1;
	}
	अन्यथा
	{
	    जर (dosec)
		closest = (दीर्घ)(vim_time() + 1);
	    अन्यथा जर (dofile)
		closest = curbuf->b_u_save_nr_last + 2;
	    अन्यथा
		closest = curbuf->b_u_seq_last + 2;
	    जर (target >= closest)
		target = closest - 1;
	}
    }
    closest_start = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    जर (target == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	जा target_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    साठी (round = 1; round <= 2; ++round)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	जर (curbuf->b_u_curhead == रिक्त)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	अन्यथा
	    uhp = curbuf->b_u_curhead;

	पर्यंत (uhp != रिक्त)
	{
	    uhp->uh_walk = mark;
	    जर (dosec)
		val = (दीर्घ)(uhp->uh_time);
	    अन्यथा जर (dofile)
		val = uhp->uh_save_nr;
	    अन्यथा
		val = uhp->uh_seq;

	    जर (round == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		जर ((step < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
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
	    जर (target == val && !dosec)
	    {
		target = uhp->uh_seq;
		थांबा;
	    }

	    // go down in the tree if we haven't been there
	    जर (uhp->uh_prev.ptr != रिक्त && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
		uhp = uhp->uh_prev.ptr;

	    // go to alternate branch if we haven't been there
	    अन्यथा जर (uhp->uh_alt_next.ptr != रिक्त
		    && uhp->uh_alt_next.ptr->uh_walk != nomark
		    && uhp->uh_alt_next.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_next.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    अन्यथा जर (uhp->uh_next.ptr != रिक्त && uhp->uh_alt_prev.ptr == रिक्त
		    && uhp->uh_next.ptr->uh_walk != nomark
		    && uhp->uh_next.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		जर (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_next.ptr;
	    }

	    अन्यथा
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		जर (uhp->uh_alt_prev.ptr != रिक्त)
		    uhp = uhp->uh_alt_prev.ptr;
		अन्यथा
		    uhp = uhp->uh_next.ptr;
	    }
	}

	जर (uhp != रिक्त)    // found it
	    थांबा;

	जर (absolute)
	{
	    semsg(_(e_undo_संख्या_nr_not_found), step);
	    परत;
	}

	जर (closest == closest_start)
	{
	    जर (!shortmess(SHM_UNDO))
	    {
		जर (step < 0)
		    संदेश(_("Already at oldest change"));
		अन्यथा
		    संदेश(_("Already at newest change"));
	    }
	    परत;
	}

	target = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	जर (step < 0)
	    above = TRUE;	// stop above the header
    }

target_zero:
    // If we found it: Follow the path to go to where we want to be.
    जर (uhp != रिक्त || target == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	पर्यंत (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    जर (uhp == रिक्त)
		uhp = curbuf->b_u_newhead;
	    अन्यथा
		uhp = uhp->uh_next.ptr;
	    जर (uhp == रिक्त || (target > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == target && !above))
		थांबा;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    जर (target > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	जर (target > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    पर्यंत (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		जर (uhp == रिक्त)
		    थांबा;

		// Go back to the first branch with a mark.
		पर्यंत (uhp->uh_alt_prev.ptr != रिक्त
				     && uhp->uh_alt_prev.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_prev.ptr;

		// Find the last branch with a mark, that's the one.
		last = uhp;
		पर्यंत (last->uh_alt_next.ptr != रिक्त
				    && last->uh_alt_next.ptr->uh_walk == mark)
		    last = last->uh_alt_next.ptr;
		जर (last != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    पर्यंत (uhp->uh_alt_prev.ptr != रिक्त)
			uhp = uhp->uh_alt_prev.ptr;
		    जर (last->uh_alt_next.ptr != रिक्त)
			last->uh_alt_next.ptr->uh_alt_prev.ptr =
							last->uh_alt_prev.ptr;
		    last->uh_alt_prev.ptr->uh_alt_next.ptr =
							last->uh_alt_next.ptr;
		    last->uh_alt_prev.ptr = रिक्त;
		    last->uh_alt_next.ptr = uhp;
		    uhp->uh_alt_prev.ptr = last;

		    जर (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = last;
		    uhp = last;
		    जर (uhp->uh_next.ptr != रिक्त)
			uhp->uh_next.ptr->uh_prev.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		जर (uhp->uh_walk != mark)
		    थांबा;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		जर (uhp->uh_seq == target && above)
		{
		    curbuf->b_u_seq_cur = target - 1;
		    थांबा;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		जर (uhp->uh_prev.ptr == रिक्त)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_prev.ptr;
		did_undo = FALSE;

		जर (uhp->uh_seq == target)	// found it!
		    थांबा;

		uhp = uhp->uh_prev.ptr;
		जर (uhp == रिक्त || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    internal_error("undo_time()");
		    थांबा;
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
    स्थिर शून्य
u_undoredo(पूर्णांक undo)
{
    undoline_T	*newarray = रिक्त;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	शीर्ष, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    दीर्घ	i;
    u_entry_T	*uep, *nuep;
    u_entry_T	*newlist = रिक्त;
    पूर्णांक		old_flags;
    पूर्णांक		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    पूर्णांक		empty_buffer;		    // buffer became empty
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
    mch_memmove(namedm, curbuf->b_namedm, आकार(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_start.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
    curbuf->b_op_start.col = 0;
    curbuf->b_op_end.lnum = 0;
    curbuf->b_op_end.col = 0;

    साठी (uep = curhead->uh_entry; uep != रिक्त; uep = nuep)
    {
	शीर्ष = uep->ue_शीर्ष;
	bot = uep->ue_bot;
	जर (bot == 0)
	    bot = curbuf->b_ml.ml_ओळ_मोजणी + 1;
	जर (शीर्ष > curbuf->b_ml.ml_ओळ_मोजणी || शीर्ष >= bot
				      || bot > curbuf->b_ml.ml_ओळ_मोजणी + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_ओळ_numbers_wrong);
	    changed();		// don't want UNCHANGED now
	    परत;
	}

	oldsize = bot - शीर्ष - 1;    // number of lines before undo
	newsize = uep->ue_size;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    जर (lnum >= शीर्ष && lnum <= शीर्ष + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    अन्यथा जर (शीर्ष < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		साठी (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(शीर्ष + 1 + i);

		    जर (curbuf->b_ml.ml_ओळ_len != uep->ue_array[i].ul_len
			    || स_भेद(uep->ue_array[i].ul_ओळ, p,
						curbuf->b_ml.ml_ओळ_len) != 0)
			थांबा;
		}
		जर (i == newsize && newlnum == MAXLNUM && uep->ue_next == रिक्त)
		{
		    newlnum = शीर्ष;
		    new_curpos.lnum = newlnum + 1;
		}
		अन्यथा जर (i < newsize)
		{
		    newlnum = शीर्ष + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	empty_buffer = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	जर (oldsize > 0)
	{
	    जर ((newarray = U_ALLOC_ओळ(आकार(undoline_T) * oldsize)) == रिक्त)
	    {
		do_outofmem_संदेश((long_u)(आकार(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		पर्यंत (uep != रिक्त)
		{
		    nuep = uep->ue_next;
		    u_freeentry(uep, uep->ue_size);
		    uep = nuep;
		}
		थांबा;
	    }
	    // delete backwards, it goes faster in most cases
	    साठी (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		जर (u_save_ओळ(&newarray[i], lnum) == FAIL)
		    do_outofmem_संदेश((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		जर (curbuf->b_ml.ml_ओळ_मोजणी == 1)
		    empty_buffer = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	अन्यथा
	    newarray = रिक्त;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	जर (newsize)
	{
	    साठी (lnum = शीर्ष, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		जर (empty_buffer && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_ओळ,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		अन्यथा
		    ml_append_flags(lnum, uep->ue_array[i].ul_ओळ,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_ओळ);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#ifdef FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, शीर्ष, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, शीर्ष + newsize + 1, &uep->ue_props,
								UNDOPROP_BELOW);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, शीर्ष, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_BELOW);
	जर (newsize > 0)
	    adjust_props_for_undo(curbuf, शीर्ष + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_BELOW);
#endif

	// adjust marks
	जर (oldsize != newsize)
	{
	    mark_adjust(शीर्ष + 1, शीर्ष + oldsize, (दीर्घ)MAXLNUM,
					       (दीर्घ)newsize - (दीर्घ)oldsize);
	    जर (curbuf->b_op_start.lnum > शीर्ष + oldsize)
		curbuf->b_op_start.lnum += newsize - oldsize;
	    जर (curbuf->b_op_end.lnum > शीर्ष + oldsize)
		curbuf->b_op_end.lnum += newsize - oldsize;
	}
	जर (oldsize > 0 || newsize > 0)
	{
	    changed_ओळी(शीर्ष + 1, 0, bot, newsize - oldsize);
#ifdef FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    जर (spell_check_window(curwin) && bot <= curbuf->b_ml.ml_ओळ_मोजणी)
		redrawWinline(curwin, bot);
#endif
	}

	// Set the '[ mark.
	जर (शीर्ष + 1 < curbuf->b_op_start.lnum)
	    curbuf->b_op_start.lnum = शीर्ष + 1;
	// Set the '] mark.
	जर (newsize == 0 && शीर्ष + 1 > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = शीर्ष + 1;
	अन्यथा जर (शीर्ष + newsize > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = शीर्ष + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_size = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = शीर्ष + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_next;
	uep->ue_next = newlist;
	newlist = uep;
    }

    // Ensure the '[ and '] marks are within bounds.
    जर (curbuf->b_op_start.lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	 curbuf->b_op_start.lnum = curbuf->b_ml.ml_ओळ_मोजणी;
    जर (curbuf->b_op_end.lnum > curbuf->b_ml.ml_ओळ_मोजणी)
	 curbuf->b_op_end.lnum = curbuf->b_ml.ml_ओळ_मोजणी;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_entry = newlist;
    curhead->uh_flags = new_flags;
    जर ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_EMPTY;
    जर (old_flags & UH_CHANGED)
	changed();
    अन्यथा
#ifdef FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	जर (!isNetbeansModified(curbuf))
#endif
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    साठी (i = 0; i < NMARKS; ++i)
    {
	जर (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	जर (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	अन्यथा
	    curhead->uh_namedm[i].lnum = 0;
    }
    जर (curhead->uh_visual.vi_start.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    जर (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    जर (curwin->w_cursor.lnum <= curbuf->b_ml.ml_ओळ_मोजणी)
    {
	जर (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = curhead->uh_cursor.col;
	    जर (virtual_active() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    अन्यथा
		curwin->w_cursor.coladd = 0;
	}
	अन्यथा
	    beginline(BL_SOL | BL_FIX);
    }
    अन्यथा
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
    जर (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	जर (curhead->uh_next.ptr != रिक्त)
	    curbuf->b_u_seq_cur = curhead->uh_next.ptr->uh_seq;
	अन्यथा
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    जर (curhead->uh_save_nr != 0)
    {
	जर (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	अन्यथा
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
    स्थिर शून्य
u_undo_end(
    पूर्णांक		did_undo,	// just did an undo
    पूर्णांक		absolute)	// used ":undo N"
{
    अक्षर	*msgstr;
    u_header_T	*uhp;
    char_u	msgbuf[80];

#ifdef FEAT_FOLDING
    जर ((fdo_flags & FDO_UNDO) && किल्ली_Typed)
	foldOpenCursor();
#endif

    जर (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	परत;

    जर (curbuf->b_ml.ml_flags & ML_EMPTY)
	--u_newcount;

    u_oldcount -= u_newcount;
    जर (u_oldcount == -1)
	msgstr = N_("more line");
    अन्यथा जर (u_oldcount < 0)
	msgstr = N_("more lines");
    अन्यथा जर (u_oldcount == 1)
	msgstr = N_("line less");
    अन्यथा जर (u_oldcount > 1)
	msgstr = N_("fewer lines");
    अन्यथा
    {
	u_oldcount = u_newcount;
	जर (u_newcount == 1)
	    msgstr = N_("change");
	अन्यथा
	    msgstr = N_("changes");
    }

    जर (curbuf->b_u_curhead != रिक्त)
    {
	// For ":undo N" we prefer a "after #N" message.
	जर (absolute && curbuf->b_u_curhead->uh_next.ptr != रिक्त)
	{
	    uhp = curbuf->b_u_curhead->uh_next.ptr;
	    did_undo = FALSE;
	}
	अन्यथा जर (did_undo)
	    uhp = curbuf->b_u_curhead;
	अन्यथा
	    uhp = curbuf->b_u_curhead->uh_next.ptr;
    }
    अन्यथा
	uhp = curbuf->b_u_newhead;

    जर (uhp == रिक्त)
	*msgbuf = NUL;
    अन्यथा
	add_time(msgbuf, आकार(msgbuf), uhp->uh_time);

#ifdef FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    जर (wp->w_buffer == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#endif
    जर (VIsual_active)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == रिक्त ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    शून्य
u_sync(
    पूर्णांक	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    जर (curbuf->b_u_synced || (!force && no_u_sync > 0))
	परत;
#जर defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    जर (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	परत;		    // XIM is busy, don't break an undo sequence
#endif
    जर (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    अन्यथा
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = रिक्त;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    शून्य
ex_undolist(exarg_T *eap UNUSED)
{
    garray_T	ga;
    u_header_T	*uhp;
    पूर्णांक		mark;
    पूर्णांक		nomark;
    पूर्णांक		changes = 1;
    पूर्णांक		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, आकार(अक्षर *), 20);

    uhp = curbuf->b_u_oldhead;
    पर्यंत (uhp != रिक्त)
    {
	जर (uhp->uh_prev.ptr == रिक्त && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    जर (ga_grow(&ga, 1) == FAIL)
		थांबा;
	    len = vim_snprintf((अक्षर *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    add_time(IObuff + len, IOSIZE - len, uhp->uh_time);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (पूर्णांक)STRLEN(IObuff + len);
	    जर (uhp->uh_save_nr > 0)
	    {
		पूर्णांक n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((अक्षर *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_data))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	जर (uhp->uh_prev.ptr != रिक्त && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_prev.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	अन्यथा जर (uhp->uh_alt_next.ptr != रिक्त
		&& uhp->uh_alt_next.ptr->uh_walk != nomark
		&& uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	अन्यथा जर (uhp->uh_next.ptr != रिक्त && uhp->uh_alt_prev.ptr == रिक्त
		&& uhp->uh_next.ptr->uh_walk != nomark
		&& uhp->uh_next.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_next.ptr;
	    --changes;
	}

	अन्यथा
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    जर (uhp->uh_alt_prev.ptr != रिक्त)
		uhp = uhp->uh_alt_prev.ptr;
	    अन्यथा
	    {
		uhp = uhp->uh_next.ptr;
		--changes;
	    }
	}
    }

    जर (ga.ga_len == 0)
	संदेश(_("Nothing to undo"));
    अन्यथा
    {
	पूर्णांक		i;

	sort_strings((char_u **)ga.ga_data, ga.ga_len);

	संदेश_start();
	संदेश_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	साठी (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    संदेश_putchar('\n');
	    जर (got_int)
		थांबा;
	    संदेश_puts(((अक्षर **)ga.ga_data)[i]);
	}
	संदेश_end();

	ga_clear_strings(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    शून्य
ex_undojoin(exarg_T *eap UNUSED)
{
    जर (curbuf->b_u_newhead == रिक्त)
	परत;		    // nothing changed before
    जर (curbuf->b_u_curhead != रिक्त)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	परत;
    }
    जर (!curbuf->b_u_synced)
	परत;		    // already unsynced
    जर (get_undolevel() < 0)
	परत;		    // no entries, nothing to do
    अन्यथा
	// Append next change to the last entry
	curbuf->b_u_synced = false;
}

/*
 * Called after writing or reloading the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
    शून्य
u_unchanged(buf_T *buf)
{
    u_unch_branch(buf->b_u_oldhead);
    buf->b_did_warn = false;
}

/*
 * After reloading a buffer which was saved for 'undoreload': Find the first
 * line that was changed and set the cursor there.
 */
    शून्य
u_find_first_changed(शून्य)
{
    u_header_T	*uhp = curbuf->b_u_newhead;
    u_entry_T   *uep;
    linenr_T	lnum;

    जर (curbuf->b_u_curhead != रिक्त || uhp == रिक्त)
	परत;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_entry;
    जर (uep->ue_शीर्ष != 0 || uep->ue_bot != 0)
	परत;

    साठी (lnum = 1; lnum < curbuf->b_ml.ml_ओळ_मोजणी
					      && lnum <= uep->ue_size; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	जर (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_ओळ_len
		|| स_भेद(p, uep->ue_array[lnum - 1].ul_ओळ, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    परत;
	}
    }
    जर (curbuf->b_ml.ml_ओळ_मोजणी != uep->ue_size)
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
    शून्य
u_update_save_nr(buf_T *buf)
{
    u_header_T	*uhp;

    ++buf->b_u_save_nr_last;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_last;
    uhp = buf->b_u_curhead;
    जर (uhp != रिक्त)
	uhp = uhp->uh_next.ptr;
    अन्यथा
	uhp = buf->b_u_newhead;
    जर (uhp != रिक्त)
	uhp->uh_save_nr = buf->b_u_save_nr_last;
}

    स्थिर शून्य
u_unch_branch(u_header_T *uhp)
{
    u_header_T	*uh;

    साठी (uh = uhp; uh != रिक्त; uh = uh->uh_prev.ptr)
    {
	uh->uh_flags |= UH_CHANGED;
	जर (uh->uh_alt_next.ptr != रिक्त)
	    u_unch_branch(uh->uh_alt_next.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    स्थिर u_entry_T *
u_get_headentry(शून्य)
{
    जर (curbuf->b_u_newhead == रिक्त || curbuf->b_u_newhead->uh_entry == रिक्त)
    {
	iemsg(e_undo_list_corrupt);
	परत रिक्त;
    }
    परत curbuf->b_u_newhead->uh_entry;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    स्थिर शून्य
u_getbot(शून्य)
{
    u_entry_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    जर (uep == रिक्त)
	परत;

    uep = curbuf->b_u_newhead->uh_getbot_entry;
    जर (uep != रिक्त)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_ओळ_मोजणी - uep->ue_lcount;
	uep->ue_bot = uep->ue_शीर्ष + uep->ue_size + 1 + extra;
	जर (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    iemsg(e_undo_ओळ_missing);
	    uep->ue_bot = uep->ue_शीर्ष + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_entry = रिक्त;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    स्थिर शून्य
u_freeheader(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    जर (uhp->uh_alt_next.ptr != रिक्त)
	u_freebranch(buf, uhp->uh_alt_next.ptr, uhpp);

    जर (uhp->uh_alt_prev.ptr != रिक्त)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = रिक्त;

    // Update the links in the list to remove the header.
    जर (uhp->uh_next.ptr == रिक्त)
	buf->b_u_oldhead = uhp->uh_prev.ptr;
    अन्यथा
	uhp->uh_next.ptr->uh_prev.ptr = uhp->uh_prev.ptr;

    जर (uhp->uh_prev.ptr == रिक्त)
	buf->b_u_newhead = uhp->uh_next.ptr;
    अन्यथा
	साठी (uhap = uhp->uh_prev.ptr; uhap != रिक्त;
						 uhap = uhap->uh_alt_next.ptr)
	    uhap->uh_next.ptr = uhp->uh_next.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    स्थिर शून्य
u_freebranch(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *tofree, *next;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    जर (uhp == buf->b_u_oldhead)
    {
	पर्यंत (buf->b_u_oldhead != रिक्त)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	परत;
    }

    जर (uhp->uh_alt_prev.ptr != रिक्त)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = रिक्त;

    next = uhp;
    पर्यंत (next != रिक्त)
    {
	tofree = next;
	जर (tofree->uh_alt_next.ptr != रिक्त)
	    u_freebranch(buf, tofree->uh_alt_next.ptr, uhpp);   // recursive
	next = tofree->uh_prev.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}

/*
 * Free all the undo entries for one header and the header itself.
 * This means that "uhp" is invalid when returning.
 */
    स्थिर शून्य
u_freeentries(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_entry_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    जर (buf->b_u_curhead == uhp)
	buf->b_u_curhead = रिक्त;
    जर (buf->b_u_newhead == uhp)
	buf->b_u_newhead = रिक्त;  // freeing the newest entry
    जर (uhpp != रिक्त && uhp == *uhpp)
	*uhpp = रिक्त;

    साठी (uep = uhp->uh_entry; uep != रिक्त; uep = nuep)
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
    स्थिर शून्य
u_freeentry(u_entry_T *uep, दीर्घ n)
{
    पर्यंत (n > 0)
	vim_free(uep->ue_array[--n].ul_ओळ);
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
    स्थिर शून्य
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = रिक्त;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_ओळ_ptr.ul_ओळ = रिक्त;
    buf->b_u_ओळ_ptr.ul_len = 0;
    buf->b_u_ओळ_ptr.ul_textlen = 0;
    buf->b_u_ओळ_lnum = 0;
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 */
    स्थिर शून्य
u_blockfree(buf_T *buf)
{
    पर्यंत (buf->b_u_oldhead != रिक्त)
	u_freeheader(buf, buf->b_u_oldhead, रिक्त);
    vim_free(buf->b_u_ओळ_ptr.ul_ओळ);
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 * and invalidate the undo buffer
 */
    शून्य
u_clearallandblockfree(buf_T *buf)
{
    u_blockfree(buf);
    u_clearall(buf);
}



/*
 * Save the line "lnum" for the "U" command.
 */
    स्थिर शून्य
u_saveline(linenr_T lnum)
{
    जर (lnum == curbuf->b_u_ओळ_lnum)	    // line is already saved
	परत;
    जर (lnum < 1 || lnum > curbuf->b_ml.ml_ओळ_मोजणी) // should never happen
	परत;
    u_clearline();
    curbuf->b_u_ओळ_lnum = lnum;
    जर (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_ओळ_colnr = curwin->w_cursor.col;
    अन्यथा
	curbuf->b_u_ओळ_colnr = 0;
    जर (u_save_ओळ(&curbuf->b_u_ओळ_ptr, lnum) == FAIL)
	do_outofmem_संदेश((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    शून्य
u_clearline(शून्य)
{
    जर (curbuf->b_u_ओळ_ptr.ul_ओळ == रिक्त)
	परत;

    VIM_CLEAR(curbuf->b_u_ओळ_ptr.ul_ओळ);
    curbuf->b_u_ओळ_ptr.ul_len = 0;
    curbuf->b_u_ओळ_ptr.ul_textlen = 0;
    curbuf->b_u_ओळ_lnum = 0;
}

/*
 * Implementation of the "U" command.
 * Differentiation from vi: "U" can be undone with the next "U".
 * We also allow the cursor to be in another line.
 * Careful: may trigger autocommands that reload the buffer.
 */
    शून्य
u_undoline(शून्य)
{
    colnr_T	t;
    undoline_T  oldp;

    जर (undo_off)
	परत;

    जर (curbuf->b_u_ओळ_ptr.ul_ओळ == रिक्त
			|| curbuf->b_u_ओळ_lnum > curbuf->b_ml.ml_ओळ_मोजणी)
    {
	beep_flush();
	परत;
    }

    // first save the line for the 'u' command
    जर (u_savecommon(curbuf->b_u_ओळ_lnum - 1,
		       curbuf->b_u_ओळ_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	परत;
    जर (u_save_ओळ(&oldp, curbuf->b_u_ओळ_lnum) == FAIL)
    {
	do_outofmem_संदेश((long_u)0);
	परत;
    }
    ml_replace_len(curbuf->b_u_ओळ_lnum, curbuf->b_u_ओळ_ptr.ul_ओळ,
				     curbuf->b_u_ओळ_ptr.ul_len, TRUE, FALSE);
    changed_bytes(curbuf->b_u_ओळ_lnum, 0);
    curbuf->b_u_ओळ_ptr = oldp;

    t = curbuf->b_u_ओळ_colnr;
    जर (curwin->w_cursor.lnum == curbuf->b_u_ओळ_lnum)
	curbuf->b_u_ओळ_colnr = curwin->w_cursor.col;
    curwin->w_cursor.col = t;
    curwin->w_cursor.lnum = curbuf->b_u_ओळ_lnum;
    check_cursor_col();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    पूर्णांक
bufIsChanged(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    जर (term_job_running_not_none(buf->b_term))
	परत TRUE;
#endif
    परत bufIsChangedNotTerm(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    पूर्णांक
anyBufIsChanged(शून्य)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	जर (bufIsChanged(buf))
	    परत TRUE;
    परत FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    पूर्णांक
bufIsChangedNotTerm(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    परत (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_changed || file_ff_differs(buf, TRUE));
}

    पूर्णांक
curbufIsChanged(शून्य)
{
    परत bufIsChanged(curbuf);
}

#जर defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    स्थिर शून्य
u_eval_वृक्ष(buf_T *buf, u_header_T *first_uhp, list_T *list)
{
    u_header_T  *uhp = first_uhp;
    dict_T	*dict;

    पर्यंत (uhp != रिक्त)
    {
	dict = dict_alloc();
	जर (dict == रिक्त)
	    परत;
	dict_add_संख्या(dict, "seq", uhp->uh_seq);
	dict_add_संख्या(dict, "time", (दीर्घ)uhp->uh_time);
	जर (uhp == buf->b_u_newhead)
	    dict_add_संख्या(dict, "newhead", 1);
	जर (uhp == buf->b_u_curhead)
	    dict_add_संख्या(dict, "curhead", 1);
	जर (uhp->uh_save_nr > 0)
	    dict_add_संख्या(dict, "save", uhp->uh_save_nr);

	जर (uhp->uh_alt_next.ptr != रिक्त)
	{
	    list_T	*alt_list = list_alloc();

	    जर (alt_list != रिक्त)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_वृक्ष(buf, uhp->uh_alt_next.ptr, alt_list);
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
    शून्य
f_undofile(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	जर (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = रिक्त;
	}
	अन्यथा
	{
	    char_u *ffname = FullName_save(fname, TRUE);

	    जर (ffname != रिक्त)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# अन्यथा
    rettv->vval.v_string = रिक्त;
# endif
}
# ifdef FEAT_PERSISTENT_UNDO
/*
 * Reset undofile option and delete the undofile
 */
    शून्य
u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    जर (!buf->b_p_udf)
	परत;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    जर (file_name != रिक्त)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    set_option_मूल्य_give_err((char_u *)"undofile", 0L, रिक्त, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    शून्य
f_undotree(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	परत;

    जर (rettv_dict_alloc(rettv) == FAIL)
	परत;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    जर (buf == रिक्त)
	परत;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_संख्या(dict, "synced", (दीर्घ)buf->b_u_synced);
    dict_add_संख्या(dict, "seq_last", buf->b_u_seq_last);
    dict_add_संख्या(dict, "save_last", buf->b_u_save_nr_last);
    dict_add_संख्या(dict, "seq_cur", buf->b_u_seq_cur);
    dict_add_संख्या(dict, "time_cur", (दीर्घ)buf->b_u_time_cur);
    dict_add_संख्या(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    जर (list != रिक्त)
    {
	u_eval_वृक्ष(buf, buf->b_u_oldhead, list);
	dict_add_list(dict, "entries", list);
    }
}

#endif
