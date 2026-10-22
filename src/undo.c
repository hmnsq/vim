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
#ವ್ಯಾಖ್ಯಾನಿಸು UH_MAGIC 0x18dade	// value for uh_magic when in use
#ವ್ಯಾಖ್ಯಾನಿಸು UE_MAGIC 0xabc123	// value for ue_magic when in use

// Size of buffer used for encryption.
#ವ್ಯಾಖ್ಯಾನಿಸು CRYPT_BUF_ಗಾತ್ರ 8192

#ಅನ್ತರ್ಭಾವ "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    buf_T	*bi_buf;
    FILE	*bi_fp;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_ಬಫರ್; // CRYPT_BUF_SIZE, NULL when not buffering
    size_t	bi_ಬಳಸಿದ;    // bytes written to/read from bi_buffer
    size_t	bi_avail;   // bytes available in bi_buffer
#ಆದರೆ_ಅಂತ್ಯ
} bufinfo_T;


ಸ್ಥಿರ ಶೂನ್ಯ u_unch_branch(u_ಶೀರ್ಷಿಕೆ_T *uhp);
ಸ್ಥಿರ u_ನಮೂದು_T *u_get_headentry(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ u_getbot(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ u_doit(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2);
ಸ್ಥಿರ ಶೂನ್ಯ u_undoredo(ಪೂರ್ಣಾಂಕ undo);
ಸ್ಥಿರ ಶೂನ್ಯ u_undo_ಕೊನೆ(ಪೂರ್ಣಾಂಕ did_undo, ಪೂರ್ಣಾಂಕ absolute);
ಸ್ಥಿರ ಶೂನ್ಯ u_freeheader(buf_T *buf, u_ಶೀರ್ಷಿಕೆ_T *uhp, u_ಶೀರ್ಷಿಕೆ_T **uhpp);
ಸ್ಥಿರ ಶೂನ್ಯ u_freebranch(buf_T *buf, u_ಶೀರ್ಷಿಕೆ_T *uhp, u_ಶೀರ್ಷಿಕೆ_T **uhpp);
ಸ್ಥಿರ ಶೂನ್ಯ u_freeentries(buf_T *buf, u_ಶೀರ್ಷಿಕೆ_T *uhp, u_ಶೀರ್ಷಿಕೆ_T **uhpp);
ಸ್ಥಿರ ಶೂನ್ಯ u_freeentry(u_ನಮೂದು_T *, ದೀರ್ಘ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ undo_flush(bufinfo_T *bi);
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ undo_read(bufinfo_T *bi, char_u *ಬಫರ್, size_t ಗಾತ್ರ_2);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ serialize_uep(bufinfo_T *bi, u_ನಮೂದು_T *uep);
ಸ್ಥಿರ u_ನಮೂದು_T *unserialize_uep(bufinfo_T *bi, ಪೂರ್ಣಾಂಕ *error, char_u *file_name);
ಸ್ಥಿರ ಶೂನ್ಯ serialize_pos(bufinfo_T *bi, pos_T pos);
ಸ್ಥಿರ ಶೂನ್ಯ unserialize_pos(bufinfo_T *bi, pos_T *pos);
ಸ್ಥಿರ ಶೂನ್ಯ serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
ಸ್ಥಿರ ಶೂನ್ಯ unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ u_saveline(linenr_T lnum);
ಸ್ಥಿರ ಶೂನ್ಯ u_blockfree(buf_T *buf);

#ವ್ಯಾಖ್ಯಾನಿಸು U_ALLOC_ಸಾಲು(ಗಾತ್ರ_2) lalloc(ಗಾತ್ರ_2, FALSE)

// used in undo_end() to report number of added and deleted lines
ಸ್ಥಿರ ದೀರ್ಘ	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	undo_undoes = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	lastmark = 0;

#ಆದರೆ defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ನೋಡಿದ_b_u_curhead;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ನೋಡಿದ_b_u_newhead;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ;

    ಸ್ಥಿರ ಶೂನ್ಯ
u_check_ವೃಕ್ಷ(u_ಶೀರ್ಷಿಕೆ_T *uhp,
	u_ಶೀರ್ಷಿಕೆ_T *exp_uh_ಮುಂದಿನ,
	u_ಶೀರ್ಷಿಕೆ_T *exp_uh_alt_prev)
{
    u_ನಮೂದು_T *uep;

    ಆದರೆ (uhp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ++ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ;
    ಆದರೆ (uhp == curbuf->b_u_curhead && ++ನೋಡಿದ_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (uhp == curbuf->b_u_newhead && ++ನೋಡಿದ_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Check pointers back are correct.
	ಆದರೆ (uhp->uh_ಮುಂದಿನ.ptr != exp_uh_ಮುಂದಿನ)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_ಮುಂದಿನ, uhp->uh_ಮುಂದಿನ.ptr);
	}
	ಆದರೆ (uhp->uh_alt_prev.ptr != exp_uh_alt_prev)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_prev, uhp->uh_alt_prev.ptr);
	}

	// Check the undo tree at this header.
	ಗಾಗಿ (uep = uhp->uh_ನಮೂದು; uep != ಖಾಲಿ; uep = uep->ue_ಮುಂದಿನ)
	{
	    ಆದರೆ (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		ನಿಲ್ಲಿಸು;
	    }
	}

	// Check the next alt tree.
	u_check_ವೃಕ್ಷ(uhp->uh_alt_ಮುಂದಿನ.ptr, uhp->uh_ಮುಂದಿನ.ptr, uhp);

	// Check the next header in this branch.
	u_check_ವೃಕ್ಷ(uhp->uh_prev.ptr, uhp, ಖಾಲಿ);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
u_check(ಪೂರ್ಣಾಂಕ newhead_may_be_NULL)
{
    ನೋಡಿದ_b_u_newhead = 0;
    ನೋಡಿದ_b_u_curhead = 0;
    ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ = 0;

    u_check_ವೃಕ್ಷ(curbuf->b_u_oldhead, ಖಾಲಿ, ಖಾಲಿ);

    ಆದರೆ (ನೋಡಿದ_b_u_newhead == 0 && curbuf->b_u_oldhead != ಖಾಲಿ
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == ಖಾಲಿ))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    ಆದರೆ (curbuf->b_u_curhead != ಖಾಲಿ && ನೋಡಿದ_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    ಆದರೆ (ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (ದೀರ್ಘ)ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ, (ದೀರ್ಘ)curbuf->b_u_numhead);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
u_save_cursor(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
u_save(linenr_T ತುದಿ, linenr_T bot)
{
    ಆದರೆ (undo_off)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (ತುದಿ >= bot || bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	ಹಿಂತಿರುಗಿ FAIL;	// rely on caller to give an error message

    ಆದರೆ (ತುದಿ + 2 == bot)
	u_saveline((linenr_T)(ತುದಿ + 1));

    ಹಿಂತಿರುಗಿ (u_savecommon(ತುದಿ, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
u_savesub(linenr_T lnum)
{
    ಆದರೆ (undo_off)
	ಹಿಂತಿರುಗಿ OK;

    ಹಿಂತಿರುಗಿ (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
u_inssub(linenr_T lnum)
{
    ಆದರೆ (undo_off)
	ಹಿಂತಿರುಗಿ OK;

    ಹಿಂತಿರುಗಿ (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
u_savedel(linenr_T lnum, ದೀರ್ಘ nlines)
{
    ಆದರೆ (undo_off)
	ಹಿಂತಿರುಗಿ OK;

    ಹಿಂತಿರುಗಿ (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    ಪೂರ್ಣಾಂಕ
undo_allowed(ಶೂನ್ಯ)
{
    // Don't allow changes when 'modifiable' is off.
    ಆದರೆ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ಹಿಂತಿರುಗಿ FALSE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    ಆದರೆ (sandbox != 0)
    {
	emsg(_(e_not_allowed_ಒಳ_sandbox));
	ಹಿಂತಿರುಗಿ FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    ಆದರೆ (textlock != 0)
    {
	emsg(_(e_not_allowed_ಗಮ್ಯ_change_ಪಠ್ಯ_or_change_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
get_undolevel(ಶೂನ್ಯ)
{
    ಆದರೆ (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	ಹಿಂತಿರುಗಿ p_ul;
    ಹಿಂತಿರುಗಿ curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
u_save_ಸಾಲು(undoline_T *ul, linenr_T lnum)
{
    char_u *ಸಾಲು = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    ಆದರೆ (curbuf->b_ml.ml_ಸಾಲು_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_ಸಾಲು = vim_strsave((char_u *)"");
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_ಸಾಲು_len;
	ul->ul_ಸಾಲು = vim_memsave(ಸಾಲು, ul->ul_len);
    }
    ಹಿಂತಿರುಗಿ ul->ul_ಸಾಲು == ಖಾಲಿ ? FAIL : OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
has_prop_w_flags(linenr_T lnum, ಪೂರ್ಣಾಂಕ flags)
{
    char_u  *props;
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    proplen = get_ಪಠ್ಯ_props(curbuf, lnum, &props, FALSE);

    ಗಾಗಿ (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * ಗಾತ್ರ prop, ಗಾತ್ರ prop);
	ಆದರೆ (prop.tp_flags & flags)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Common code for various ways to save text before a change.
 * "top" is the line above the first changed line.
 * "bot" is the line below the last changed line.
 * "newbot" is the new bottom line.  Use zero when not known.
 * "reload" is TRUE when saving for a buffer reload.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
u_savecommon(
    linenr_T	ತುದಿ,
    linenr_T	bot,
    linenr_T	newbot,
    ಪೂರ್ಣಾಂಕ		reload)
{
    linenr_T	lnum;
    ದೀರ್ಘ	i;
    u_ಶೀರ್ಷಿಕೆ_T	*uhp;
    u_ಶೀರ್ಷಿಕೆ_T	*old_curhead;
    u_ನಮೂದು_T	*uep;
    u_ನಮೂದು_T	*prev_uep;
    ದೀರ್ಘ	ಗಾತ್ರ_2;

    ಆದರೆ (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	ಆದರೆ (!undo_allowed())
	    ಹಿಂತಿರುಗಿ FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	ಆದರೆ (netbeans_ಸಕ್ರಿಯ())
	{
	    ಆದರೆ (netbeans_is_guarded(ತುದಿ, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_ಒಳ_read_only_files));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	// A change in a terminal buffer removes the highlighting.
	ಪದ_change_ಒಳ_curbuf();
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Saving text for undo means we are going to make a change.  Give a
	 * warning for a read-only file before making the change, so that the
	 * FileChangedRO event can replace the buffer with a read-write version
	 * (e.g., obtained from a source control system).
	 */
	change_warning(0);
	ಆದರೆ (bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_ಸಾಲು_ಎಣಿಕೆ_ಬದಲಾಯಿತು_unexpectedly));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
    u_check(FALSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    ಆದರೆ (bot - ತುದಿ > 1)
    {
	ಆದರೆ (ತುದಿ > 0 && has_prop_w_flags(ತುದಿ + 1, TP_FLAG_CONT_PREV))
	    --ತುದಿ;
	ಆದರೆ (bot <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_ಮುಂದಿನ))
	{
	    ++bot;
	    ಆದರೆ (newbot != 0)
		++newbot;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಗಾತ್ರ_2 = bot - ತುದಿ - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    ಆದರೆ (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	ಆದರೆ (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_ಸಾಲು(ಗಾತ್ರ(u_ಶೀರ್ಷಿಕೆ_T));
	    ಆದರೆ (uhp == ಖಾಲಿ)
		ಹೋಗು nomem;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    uhp = ಖಾಲಿ;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	ಆದರೆ (old_curhead != ಖಾಲಿ)
	{
	    curbuf->b_u_newhead = old_curhead->uh_ಮುಂದಿನ.ptr;
	    curbuf->b_u_curhead = ಖಾಲಿ;
	}

	/*
	 * free headers to keep the size right
	 */
	ಆಗ (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != ಖಾಲಿ)
	{
	    u_ಶೀರ್ಷಿಕೆ_T	    *uhfree = curbuf->b_u_oldhead;

	    ಆದರೆ (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uhfree->uh_alt_ಮುಂದಿನ.ptr == ಖಾಲಿ)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Free the oldest alternate branch as a whole.
		ಆಗ (uhfree->uh_alt_ಮುಂದಿನ.ptr != ಖಾಲಿ)
		    uhfree = uhfree->uh_alt_ಮುಂದಿನ.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
	    u_check(TRUE);
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ (uhp == ಖಾಲಿ)		// no undo at all
	{
	    ಆದರೆ (old_curhead != ಖಾಲಿ)
		u_freebranch(curbuf, old_curhead, ಖಾಲಿ);
	    curbuf->b_u_synced = false;
	    ಹಿಂತಿರುಗಿ OK;
	}

	uhp->uh_prev.ptr = ಖಾಲಿ;
	uhp->uh_ಮುಂದಿನ.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_ಮುಂದಿನ.ptr = old_curhead;
	ಆದರೆ (old_curhead != ಖಾಲಿ)
	{
	    uhp->uh_alt_prev.ptr = old_curhead->uh_alt_prev.ptr;
	    ಆದರೆ (uhp->uh_alt_prev.ptr != ಖಾಲಿ)
		uhp->uh_alt_prev.ptr->uh_alt_ಮುಂದಿನ.ptr = uhp;
	    old_curhead->uh_alt_prev.ptr = uhp;
	    ಆದರೆ (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    uhp->uh_alt_prev.ptr = ಖಾಲಿ;
	ಆದರೆ (curbuf->b_u_newhead != ಖಾಲಿ)
	    curbuf->b_u_newhead->uh_prev.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_ಕೊನೆಯ;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_ಸಮಯ = vim_ಸಮಯ();
	uhp->uh_save_nr = 0;
	curbuf->b_u_ಸಮಯ_cur = uhp->uh_ಸಮಯ + 1;

	uhp->uh_walk = 0;
	uhp->uh_ನಮೂದು = ಖಾಲಿ;
	uhp->uh_getbot_ನಮೂದು = ಖಾಲಿ;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	ಆದರೆ (virtual_ಸಕ್ರಿಯ() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	ಇಲ್ಲದಿದ್ದರೆ
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_ಬದಲಾಯಿತು ? UH_ಬದಲಾಯಿತು : 0) +
		       ((curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, ಗಾತ್ರ(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	ಆದರೆ (curbuf->b_u_oldhead == ಖಾಲಿ)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (get_undolevel() < 0)	// no undo at all
	    ಹಿಂತಿರುಗಿ OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	ಆದರೆ (ಗಾತ್ರ_2 == 1)
	{
	    uep = u_get_headentry();
	    prev_uep = ಖಾಲಿ;
	    ಗಾಗಿ (i = 0; i < 10; ++i)
	    {
		ಆದರೆ (uep == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		ಆದರೆ ((curbuf->b_u_newhead->uh_getbot_ನಮೂದು != uep
			    ? (uep->ue_ತುದಿ + uep->ue_ಗಾತ್ರ + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			|| (uep->ue_ಗಾತ್ರ > 1
			    && ತುದಿ >= uep->ue_ತುದಿ
			    && ತುದಿ + 2 <= uep->ue_ತುದಿ + uep->ue_ಗಾತ್ರ + 1))
		    ನಿಲ್ಲಿಸು;

		// If it's the same line we can skip saving it again.
		ಆದರೆ (uep->ue_ಗಾತ್ರ == 1 && uep->ue_ತುದಿ == ತುದಿ)
		{
		    ಆದರೆ (i > 0)
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
			prev_uep->ue_ಮುಂದಿನ = uep->ue_ಮುಂದಿನ;
			uep->ue_ಮುಂದಿನ = curbuf->b_u_newhead->uh_ನಮೂದು;
			curbuf->b_u_newhead->uh_ನಮೂದು = uep;
		    }

		    // The executed command may change the line count.
		    ಆದರೆ (newbot != 0)
			uep->ue_bot = newbot;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
			uep->ue_bot = 0;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			uep->ue_lcount = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
			curbuf->b_u_newhead->uh_getbot_ನಮೂದು = uep;
		    }
		    ಹಿಂತಿರುಗಿ OK;
		}
		prev_uep = uep;
		uep = uep->ue_ಮುಂದಿನ;
	    }
	}

	// find line number for ue_bot for previous u_save()
	u_getbot();
    }

    /*
     * add lines in front of entry list
     */
    uep = U_ALLOC_ಸಾಲು(ಗಾತ್ರ(u_ನಮೂದು_T));
    ಆದರೆ (uep == ಖಾಲಿ)
	ಹೋಗು nomem;
    CLEAR_ಸೂಚಕ(uep);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
    uep->ue_magic = UE_MAGIC;
#ಆದರೆ_ಅಂತ್ಯ

    uep->ue_ಗಾತ್ರ = ಗಾತ್ರ_2;
    uep->ue_ತುದಿ = ತುದಿ;
    ಆದರೆ (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	uep->ue_bot = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	uep->ue_lcount = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	curbuf->b_u_newhead->uh_getbot_ನಮೂದು = uep;
    }

    ಆದರೆ (ಗಾತ್ರ_2 > 0)
    {
	ಆದರೆ ((uep->ue_array = U_ALLOC_ಸಾಲು(ಗಾತ್ರ(undoline_T) * ಗಾತ್ರ_2)) == ಖಾಲಿ)
	{
	    u_freeentry(uep, 0L);
	    ಹೋಗು nomem;
	}
	ಗಾಗಿ (i = 0, lnum = ತುದಿ + 1; i < ಗಾತ್ರ_2; ++i)
	{
	    fast_breakcheck();
	    ಆದರೆ (got_int)
	    {
		u_freeentry(uep, i);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (u_save_ಸಾಲು(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		ಹೋಗು nomem;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	uep->ue_array = ಖಾಲಿ;
    uep->ue_ಮುಂದಿನ = curbuf->b_u_newhead->uh_ನಮೂದು;
    curbuf->b_u_newhead->uh_ನಮೂದು = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
    u_check(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ OK;

nomem:
    ಸಂದೇಶ_silent = 0;	// must display the prompt
    ಆದರೆ (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	ಹಿಂತಿರುಗಿ OK;
    }
    do_outofmem_ಸಂದೇಶ((long_u)0);
    ಹಿಂತಿರುಗಿ FAIL;
}

#ಆದರೆ defined(FEAT_PERSISTENT_UNDO)

# define UF_ಆರಂಭ_MAGIC	    "Vim\237UnDo\345"  // magic at start of undofile
# define UF_ಆರಂಭ_MAGIC_LEN	9
# define UF_ಶೀರ್ಷಿಕೆ_MAGIC	0x5fd0	// magic at start of header
# define UF_ಶೀರ್ಷಿಕೆ_ಕೊನೆ_MAGIC	0xe7aa	// magic after last header
# define UF_ನಮೂದು_MAGIC		0xf518	// magic at start of entry
# define UF_ನಮೂದು_ಕೊನೆ_MAGIC	0x3581	// magic after last entry
# define UF_ಆವೃತ್ತಿ		2	// 2-byte undofile version number
# define UF_ಆವೃತ್ತಿ_CRYPT	0x8002	// idem, encrypted

// extra fields for header
# define UF_ಕೊನೆಯ_SAVE_NR	1

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

    sha256_ಆರಂಭ(&ctx);
    ಗಾಗಿ (lnum = 1; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
	sha256_update(&ctx, ml_get(lnum), (UINT32_T)(ml_get_len(lnum) + 1));
    sha256_ಮುಕ್ತಾಯ(&ctx, hash);
}

/*
 * Return an allocated string of the full path of the target undofile.
 * When "reading" is TRUE find the file to read, go over all directories in
 * 'undodir'.
 * When "reading" is FALSE use the first name where the directory exists.
 * Returns NULL when there is no place to write or no file to read.
 */
    ಸ್ಥಿರ char_u *
u_get_undo_file_name(char_u *buf_ffname, ಪೂರ್ಣಾಂಕ reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {ಖಾಲಿ, 0};
    char_u	*undo_file_name = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		dir_len;
    char_u	*p;
    size_t	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    size_t	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    ಆದರೆ (ffname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    ಆದರೆ (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    ಆಗ (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	ಆದರೆ (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    ಆದರೆ (undo_file_name == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    p = gettail(undo_file_name);
	    plen = (size_t)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# ಇಲ್ಲದಿದ್ದರೆ
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dir_name[dir_len] = NUL;
	    ಆದರೆ (mch_isdir(dir_name))
	    {
		string_T    ret;

		ಆದರೆ (munged_name.string == ಖಾಲಿ)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    ಆದರೆ (munged_name.string == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		    munged_name.ಉದ್ದಳತೆ = ffnamelen;
		    ಗಾಗಿ (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			ಆದರೆ (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (size_t)dir_len,
		    munged_name.string, munged_name.ಉದ್ದಳತೆ, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	ಆದರೆ (undo_file_name != ಖಾಲಿ && (!reading
			       || mch_stat((ಅಕ್ಷರ *)undo_file_name, &st) >= 0))
	    ನಿಲ್ಲಿಸು;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    ಹಿಂತಿರುಗಿ undo_file_name;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
corruption_error(ಅಕ್ಷರ *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
u_free_uhp(u_ಶೀರ್ಷಿಕೆ_T *uhp)
{
    u_ನಮೂದು_T	*nuep;
    u_ನಮೂದು_T	*uep;

    uep = uhp->uh_ನಮೂದು;
    ಆಗ (uep != ಖಾಲಿ)
    {
	nuep = uep->ue_ಮುಂದಿನ;
	u_freeentry(uep, uep->ue_ಗಾತ್ರ);
	uep = nuep;
    }
    vim_free(uhp);
}

/*
 * Write a sequence of bytes to the undo file.
 * Buffers and encrypts as needed.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    ಆದರೆ (bi->bi_ಬಫರ್ != ಖಾಲಿ)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	ಆಗ (bi->bi_ಬಳಸಿದ + len_todo >= CRYPT_BUF_ಗಾತ್ರ)
	{
	    size_t	n = CRYPT_BUF_ಗಾತ್ರ - bi->bi_ಬಳಸಿದ;

	    mch_memmove(bi->bi_ಬಫರ್ + bi->bi_ಬಳಸಿದ, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_ಬಳಸಿದ = CRYPT_BUF_ಗಾತ್ರ;
	    ಆದರೆ (undo_flush(bi) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (len_todo > 0)
	{
	    mch_memmove(bi->bi_ಬಫರ್ + bi->bi_ಬಳಸಿದ, p, len_todo);
	    bi->bi_ಬಳಸಿದ += len_todo;
	}
	ಹಿಂತಿರುಗಿ OK;
    }
# endif
    ಆದರೆ (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

# ifdef FEAT_CRYPT
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
undo_flush(bufinfo_T *bi)
{
    ಆದರೆ (bi->bi_ಬಫರ್ != ಖಾಲಿ && bi->bi_state != ಖಾಲಿ && bi->bi_ಬಳಸಿದ > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_ಬಫರ್, bi->bi_ಬಳಸಿದ, FALSE);
	ಆದರೆ (fwrite(bi->bi_ಬಫರ್, bi->bi_ಬಳಸಿದ, (size_t)1, bi->bi_fp) != 1)
	    ಹಿಂತಿರುಗಿ FAIL;
	bi->bi_ಬಳಸಿದ = 0;
    }
    ಹಿಂತಿರುಗಿ OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    ಆದರೆ (bi->bi_state != ಖಾಲಿ && bi->bi_ಬಫರ್ == ಖಾಲಿ)
    {
	// crypting every piece of text separately
	ಆದರೆ (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    copy = lalloc(len, FALSE);
	    ಆದರೆ (copy == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	ಆದರೆ (copy != small_buf)
	    vim_free(copy);
	ಹಿಂತಿರುಗಿ i == 1 ? OK : FAIL;
    }
# endif
    ಹಿಂತಿರುಗಿ undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
undo_write_ಬೈಟುಗಳು(bufinfo_T *bi, long_u nr, ಪೂರ್ಣಾಂಕ len)
{
    char_u  buf[8];
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    bufi = 0;

    ಗಾಗಿ (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    ಹಿಂತಿರುಗಿ undo_write(bi, buf, (size_t)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    ಸ್ಥಿರ ಶೂನ್ಯ
put_ಶೀರ್ಷಿಕೆ_ptr(bufinfo_T *bi, u_ಶೀರ್ಷಿಕೆ_T *uhp)
{
    undo_write_ಬೈಟುಗಳು(bi, (long_u)(uhp != ಖಾಲಿ ? uhp->uh_seq : 0), 4);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ಆದರೆ (bi->bi_ಬಫರ್ != ಖಾಲಿ)
    {
	char_u  buf[4];
	ಪೂರ್ಣಾಂಕ	n;

	undo_read(bi, buf, (size_t)4);
	n = ((ಚಿಹ್ನೆರಹಿತ)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	ಹಿಂತಿರುಗಿ n;
    }
# endif
    ಹಿಂತಿರುಗಿ get4c(bi->bi_fp);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ಆದರೆ (bi->bi_ಬಫರ್ != ಖಾಲಿ)
    {
	char_u  buf[2];
	ಪೂರ್ಣಾಂಕ	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	ಹಿಂತಿರುಗಿ n;
    }
# endif
    ಹಿಂತಿರುಗಿ get2c(bi->bi_fp);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ಆದರೆ (bi->bi_ಬಫರ್ != ಖಾಲಿ)
    {
	char_u  buf[1];

	undo_read(bi, buf, (size_t)1);
	ಹಿಂತಿರುಗಿ buf[0];
    }
# endif
    ಹಿಂತಿರುಗಿ getc(bi->bi_fp);
}

    ಸ್ಥಿರ time_t
undo_read_ಸಮಯ(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ಆದರೆ (bi->bi_ಬಫರ್ != ಖಾಲಿ)
    {
	char_u  buf[8];
	time_t	n = 0;
	ಪೂರ್ಣಾಂಕ	i;

	undo_read(bi, buf, (size_t)8);
	ಗಾಗಿ (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	ಹಿಂತಿರುಗಿ n;
    }
# endif
    ಹಿಂತಿರುಗಿ get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
undo_read(bufinfo_T *bi, char_u *ಬಫರ್, size_t ಗಾತ್ರ_2)
{
    ಪೂರ್ಣಾಂಕ retval = OK;

# ifdef FEAT_CRYPT
    ಆದರೆ (bi->bi_ಬಫರ್ != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	ಗಾತ್ರ_todo = (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ_2;
	char_u	*p = ಬಫರ್;

	ಆಗ (ಗಾತ್ರ_todo > 0)
	{
	    size_t n;

	    ಆದರೆ (bi->bi_ಬಳಸಿದ >= bi->bi_avail)
	    {
		n = fread(bi->bi_ಬಫರ್, 1, (size_t)CRYPT_BUF_ಗಾತ್ರ, bi->bi_fp);
		ಆದರೆ (n == 0)
		{
		    retval = FAIL;
		    ನಿಲ್ಲಿಸು;
		}
		bi->bi_avail = n;
		bi->bi_ಬಳಸಿದ = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_ಬಫರ್, bi->bi_avail, FALSE);
	    }
	    n = ಗಾತ್ರ_todo;
	    ಆದರೆ (n > bi->bi_avail - bi->bi_ಬಳಸಿದ)
		n = bi->bi_avail - bi->bi_ಬಳಸಿದ;
	    mch_memmove(p, bi->bi_ಬಫರ್ + bi->bi_ಬಳಸಿದ, n);
	    bi->bi_ಬಳಸಿದ += n;
	    ಗಾತ್ರ_todo -= (ಪೂರ್ಣಾಂಕ)n;
	    p += n;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    ಆದರೆ (fread(ಬಫರ್, ಗಾತ್ರ_2, 1, bi->bi_fp) != 1)
	retval = FAIL;

    ಆದರೆ (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(ಬಫರ್, 0, ಗಾತ್ರ_2);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    ಸ್ಥಿರ char_u *
read_string_decrypt(bufinfo_T *bi, ಪೂರ್ಣಾಂಕ len)
{
    char_u  *ptr = alloc(len + 1);

    ಆದರೆ (ptr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    ಆದರೆ (bi->bi_state != ಖಾಲಿ && bi->bi_ಬಫರ್ == ಖಾಲಿ)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    ಹಿಂತಿರುಗಿ ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
serialize_ಶೀರ್ಷಿಕೆ(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	ಸಮಯ_buf[8];

    // Start writing, first the magic marker and undo info version.
    ಆದರೆ (fwrite(UF_ಆರಂಭ_MAGIC, (size_t)UF_ಆರಂಭ_MAGIC_LEN, (size_t)1, fp) != 1)
	ಹಿಂತಿರುಗಿ FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    ಆದರೆ (*buf->b_p_ಕೀಲಿ != NUL)
    {
	char_u *ಶೀರ್ಷಿಕೆ;
	ಪೂರ್ಣಾಂಕ    ಶೀರ್ಷಿಕೆ_len;
	ದೀರ್ಘ	len;

	undo_write_ಬೈಟುಗಳು(bi, (long_u)UF_ಆವೃತ್ತಿ_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_ವಿಧಾನ_nr(buf),
					  buf->b_p_ಕೀಲಿ, &ಶೀರ್ಷಿಕೆ, &ಶೀರ್ಷಿಕೆ_len);
	ಆದರೆ (bi->bi_state == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	len = (ದೀರ್ಘ)fwrite(ಶೀರ್ಷಿಕೆ, (size_t)ಶೀರ್ಷಿಕೆ_len, (size_t)1, fp);
	vim_free(ಶೀರ್ಷಿಕೆ);
	ಆದರೆ (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = ಖಾಲಿ;
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (crypt_ಸಂಪೂರ್ಣ_undofile(crypt_get_ವಿಧಾನ_nr(buf)))
	{
	    bi->bi_ಬಫರ್ = alloc(CRYPT_BUF_ಗಾತ್ರ);
	    ಆದರೆ (bi->bi_ಬಫರ್ == ಖಾಲಿ)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = ಖಾಲಿ;
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    bi->bi_ಬಳಸಿದ = 0;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	undo_write_ಬೈಟುಗಳು(bi, (long_u)UF_ಆವೃತ್ತಿ, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    ಆದರೆ (undo_write(bi, hash, (size_t)UNDO_HASH_ಗಾತ್ರ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // buffer-specific data
    undo_write_ಬೈಟುಗಳು(bi, (long_u)buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)buf->b_u_ಸಾಲು_ptr.ul_textlen, 4);
    ಆದರೆ (buf->b_u_ಸಾಲು_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು,
			(size_t)buf->b_u_ಸಾಲು_ptr.ul_textlen) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    undo_write_ಬೈಟುಗಳು(bi, (long_u)buf->b_u_ಸಾಲು_lnum, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)buf->b_u_ಸಾಲು_colnr, 4);

    // Undo structures header data
    put_ಶೀರ್ಷಿಕೆ_ptr(bi, buf->b_u_oldhead);
    put_ಶೀರ್ಷಿಕೆ_ptr(bi, buf->b_u_newhead);
    put_ಶೀರ್ಷಿಕೆ_ptr(bi, buf->b_u_curhead);

    undo_write_ಬೈಟುಗಳು(bi, (long_u)buf->b_u_numhead, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)buf->b_u_seq_ಕೊನೆಯ, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)buf->b_u_seq_cur, 4);
    ಸಮಯ_ಗಮ್ಯ_ಬೈಟುಗಳು(buf->b_u_ಸಮಯ_cur, ಸಮಯ_buf);
    undo_write(bi, ಸಮಯ_buf, 8);

    // Optional fields.
    undo_write_ಬೈಟುಗಳು(bi, 4, 1);
    undo_write_ಬೈಟುಗಳು(bi, UF_ಕೊನೆಯ_SAVE_NR, 1);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)buf->b_u_save_nr_ಕೊನೆಯ, 4);

    undo_write_ಬೈಟುಗಳು(bi, 0, 1);  // end marker

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
serialize_uhp(bufinfo_T *bi, u_ಶೀರ್ಷಿಕೆ_T *uhp)
{
    ಪೂರ್ಣಾಂಕ		i;
    u_ನಮೂದು_T	*uep;
    char_u	ಸಮಯ_buf[8];

    ಆದರೆ (undo_write_ಬೈಟುಗಳು(bi, (long_u)UF_ಶೀರ್ಷಿಕೆ_MAGIC, 2) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    put_ಶೀರ್ಷಿಕೆ_ptr(bi, uhp->uh_ಮುಂದಿನ.ptr);
    put_ಶೀರ್ಷಿಕೆ_ptr(bi, uhp->uh_prev.ptr);
    put_ಶೀರ್ಷಿಕೆ_ptr(bi, uhp->uh_alt_ಮುಂದಿನ.ptr);
    put_ಶೀರ್ಷಿಕೆ_ptr(bi, uhp->uh_alt_prev.ptr);
    undo_write_ಬೈಟುಗಳು(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    ಗಾಗಿ (i = 0; i < NMARKS; ++i)
	serialize_pos(bi, uhp->uh_namedm[i]);
    serialize_visualinfo(bi, &uhp->uh_visual);
    ಸಮಯ_ಗಮ್ಯ_ಬೈಟುಗಳು(uhp->uh_ಸಮಯ, ಸಮಯ_buf);
    undo_write(bi, ಸಮಯ_buf, 8);

    // Optional fields.
    undo_write_ಬೈಟುಗಳು(bi, 4, 1);
    undo_write_ಬೈಟುಗಳು(bi, UHP_SAVE_NR, 1);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)uhp->uh_save_nr, 4);

    undo_write_ಬೈಟುಗಳು(bi, 0, 1);  // end marker

    // Write all the entries.
    ಗಾಗಿ (uep = uhp->uh_ನಮೂದು; uep != ಖಾಲಿ; uep = uep->ue_ಮುಂದಿನ)
    {
	undo_write_ಬೈಟುಗಳು(bi, (long_u)UF_ನಮೂದು_MAGIC, 2);
	ಆದರೆ (serialize_uep(bi, uep) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    undo_write_ಬೈಟುಗಳು(bi, (long_u)UF_ನಮೂದು_ಕೊನೆ_MAGIC, 2);
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ u_ಶೀರ್ಷಿಕೆ_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_ಶೀರ್ಷಿಕೆ_T	*uhp;
    ಪೂರ್ಣಾಂಕ		i;
    u_ನಮೂದು_T	*uep, *ಕೊನೆಯ_uep;
    ಪೂರ್ಣಾಂಕ		c;
    ಪೂರ್ಣಾಂಕ		error;

    uhp = U_ALLOC_ಸಾಲು(ಗಾತ್ರ(u_ಶೀರ್ಷಿಕೆ_T));
    ಆದರೆ (uhp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    CLEAR_ಸೂಚಕ(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_ಮುಂದಿನ.seq = undo_read_4c(bi);
    uhp->uh_prev.seq = undo_read_4c(bi);
    uhp->uh_alt_ಮುಂದಿನ.seq = undo_read_4c(bi);
    uhp->uh_alt_prev.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    ಆದರೆ (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    ಗಾಗಿ (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_ಸಮಯ = undo_read_ಸಮಯ(bi);

    // Optional fields.
    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ len = undo_read_byte(bi);
	ಪೂರ್ಣಾಂಕ what;

	ಆದರೆ (len == EOF)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ (len == 0)
	    ನಿಲ್ಲಿಸು;
	what = undo_read_byte(bi);
	ಆಯ್ಕೆ (what)
	{
	    ಸ್ಥಿತಿ UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		// field not supported, skip
		ಆಗ (--len >= 0)
		    (ಶೂನ್ಯ)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    ಕೊನೆಯ_uep = ಖಾಲಿ;
    ಆಗ ((c = undo_read_2c(bi)) == UF_ನಮೂದು_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	ಆದರೆ (ಕೊನೆಯ_uep == ಖಾಲಿ)
	    uhp->uh_ನಮೂದು = uep;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೊನೆಯ_uep->ue_ಮುಂದಿನ = uep;
	ಕೊನೆಯ_uep = uep;
	ಆದರೆ (uep == ಖಾಲಿ || error)
	{
	    u_free_uhp(uhp);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಆದರೆ (c != UF_ನಮೂದು_ಕೊನೆ_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ uhp;
}

/*
 * Serialize "uep".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
serialize_uep(
    bufinfo_T	*bi,
    u_ನಮೂದು_T	*uep)
{
    ಪೂರ್ಣಾಂಕ		i;

    undo_write_ಬೈಟುಗಳು(bi, (long_u)uep->ue_ತುದಿ, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)uep->ue_bot, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)uep->ue_lcount, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)uep->ue_ಗಾತ್ರ, 4);
    ಗಾಗಿ (i = 0; i < uep->ue_ಗಾತ್ರ; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	ಆದರೆ (undo_write_ಬೈಟುಗಳು(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_ಸಾಲು, uep->ue_array[i].ul_textlen) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ u_ನಮೂದು_T *
unserialize_uep(bufinfo_T *bi, ಪೂರ್ಣಾಂಕ *error, char_u *file_name)
{
    ಪೂರ್ಣಾಂಕ		i;
    u_ನಮೂದು_T	*uep;
    undoline_T	*array = ಖಾಲಿ;
    char_u	*ಸಾಲು;
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_len;

    uep = U_ALLOC_ಸಾಲು(ಗಾತ್ರ(u_ನಮೂದು_T));
    ಆದರೆ (uep == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    CLEAR_ಸೂಚಕ(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_ತುದಿ = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_ಗಾತ್ರ = undo_read_4c(bi);
    ಆದರೆ (uep->ue_ಗಾತ್ರ > 0)
    {
	ಆದರೆ (uep->ue_ಗಾತ್ರ < LONG_MAX / (ದೀರ್ಘ)ಗಾತ್ರ(undoline_T))
	    array = U_ALLOC_ಸಾಲು(ಗಾತ್ರ(undoline_T) * uep->ue_ಗಾತ್ರ);
	ಆದರೆ (array == ಖಾಲಿ)
	{
	    *error = TRUE;
	    ಹಿಂತಿರುಗಿ uep;
	}
	vim_memset(array, 0, ಗಾತ್ರ(undoline_T) * uep->ue_ಗಾತ್ರ);
    }
    uep->ue_array = array;

    ಗಾಗಿ (i = 0; i < uep->ue_ಗಾತ್ರ; ++i)
    {
	ಸಾಲು_len = undo_read_4c(bi);
	ಆದರೆ (ಸಾಲು_len >= 0)
	    ಸಾಲು = read_string_decrypt(bi, ಸಾಲು_len);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಸಾಲು = ಖಾಲಿ;
	    corruption_error("line length", file_name);
	}
	ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
	{
	    *error = TRUE;
	    ಹಿಂತಿರುಗಿ uep;
	}
	array[i].ul_ಸಾಲು = ಸಾಲು;
	array[i].ul_len = ಸಾಲು_len + 1;
	array[i].ul_textlen = ಸಾಲು_len;
    }
    ಹಿಂತಿರುಗಿ uep;
}

/*
 * Serialize "pos".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_ಬೈಟುಗಳು(bi, (long_u)pos.lnum, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)pos.ಲಂಬಸಾಲು, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    ಆದರೆ (pos->lnum < 0)
	pos->lnum = 0;
    pos->ಲಂಬಸಾಲು = undo_read_4c(bi);
    ಆದರೆ (pos->ಲಂಬಸಾಲು < 0)
	pos->ಲಂಬಸಾಲು = 0;
    pos->coladd = undo_read_4c(bi);
    ಆದರೆ (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    serialize_pos(bi, info->vi_ಆರಂಭ);
    serialize_pos(bi, info->vi_ಕೊನೆ);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)info->vi_ಬಹುಲಕ, 4);
    undo_write_ಬೈಟುಗಳು(bi, (long_u)info->vi_curswant, 4);
}

/*
 * Unserialize the visualinfo_T at the current position.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info)
{
    unserialize_pos(bi, &info->vi_ಆರಂಭ);
    unserialize_pos(bi, &info->vi_ಕೊನೆ);
    info->vi_ಬಹುಲಕ = undo_read_4c(bi);
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
    ಪೂರ್ಣಾಂಕ		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_ಶೀರ್ಷಿಕೆ_T	*uhp;
    char_u	*file_name;
    ಪೂರ್ಣಾಂಕ		mark;
# ifdef U_DEBUG
    ಪೂರ್ಣಾಂಕ		ಶೀರ್ಷಿಕೆಗಳು_written = 0;
# endif
    ಪೂರ್ಣಾಂಕ		fd;
    FILE	*fp = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		perm;
    ಪೂರ್ಣಾಂಕ		write_ok = FALSE;
# ifdef UNIX
    ಪೂರ್ಣಾಂಕ		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_ಕ್ಷೇತ್ರ(bi);

    ಆದರೆ (name == ಖಾಲಿ)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	ಆದರೆ (file_name == ಖಾಲಿ)
	{
	    ಆದರೆ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    ಆದರೆ (buf->b_ffname != ಖಾಲಿ)
    {
# ifdef UNIX
	ಆದರೆ (mch_stat((ಅಕ್ಷರ *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_ಬಹುಲಕ;
	    st_old_valid = TRUE;
	}
# ಇಲ್ಲದಿದ್ದರೆ
	perm = mch_getperm(buf->b_ffname);
	ಆದರೆ (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    ಆದರೆ (mch_getperm(file_name) >= 0)
    {
	ಆದರೆ (name == ಖಾಲಿ || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((ಅಕ್ಷರ *)file_name, O_RDONLY|O_EXTRA, 0);
	    ಆದರೆ (fd < 0)
	    {
		ಆದರೆ (name != ಖಾಲಿ || p_verbose > 0)
		{
		    ಆದರೆ (name == ಖಾಲಿ)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    ಆದರೆ (name == ಖಾಲಿ)
			verbose_leave();
		}
		ಹೋಗು theend;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		char_u	mbuf[UF_ಆರಂಭ_MAGIC_LEN];
		ಪೂರ್ಣಾಂಕ	len;

		len = read_eintr(fd, mbuf, UF_ಆರಂಭ_MAGIC_LEN);
		close(fd);
		ಆದರೆ (len < UF_ಆರಂಭ_MAGIC_LEN
		      || memcmp(mbuf, UF_ಆರಂಭ_MAGIC, UF_ಆರಂಭ_MAGIC_LEN) != 0)
		{
		    ಆದರೆ (name != ಖಾಲಿ || p_verbose > 0)
		    {
			ಆದರೆ (name == ಖಾಲಿ)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			ಆದರೆ (name == ಖಾಲಿ)
			    verbose_leave();
		    }
		    ಹೋಗು theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    ಆದರೆ (buf->b_u_numhead == 0 && buf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು == ಖಾಲಿ)
    {
	ಆದರೆ (p_verbose > 0)
	    verb_ಸಂದೇಶ(_("Skipping undo file write, nothing to undo"));
	ಹೋಗು theend;
    }

    fd = mch_open((ಅಕ್ಷರ *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    ಆದರೆ (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	ಹೋಗು theend;
    }
    (ಶೂನ್ಯ)mch_setperm(file_name, perm);
    ಆದರೆ (p_verbose > 0)
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
    ಆದರೆ (st_old_valid
	    && mch_stat((ಅಕ್ಷರ *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  ಆದರೆ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    ಆದರೆ (buf->b_ffname != ಖಾಲಿ)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    ಆದರೆ (fp == ಖಾಲಿ)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	close(fd);
	mch_remove(file_name);
	ಹೋಗು theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    ಆದರೆ (serialize_ಶೀರ್ಷಿಕೆ(&bi, hash) == FAIL)
	ಹೋಗು write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    ಆಗ (uhp != ಖಾಲಿ)
    {
	// Serialize current UHP if we haven't seen it
	ಆದರೆ (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++ಶೀರ್ಷಿಕೆಗಳು_written;
# endif
	    ಆದರೆ (serialize_uhp(&bi, uhp) == FAIL)
		ಹೋಗು write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	ಆದರೆ (uhp->uh_prev.ptr != ಖಾಲಿ && uhp->uh_prev.ptr->uh_walk != mark)
	    uhp = uhp->uh_prev.ptr;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uhp->uh_alt_ಮುಂದಿನ.ptr != ಖಾಲಿ
				     && uhp->uh_alt_ಮುಂದಿನ.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_ಮುಂದಿನ.ptr;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uhp->uh_ಮುಂದಿನ.ptr != ಖಾಲಿ && uhp->uh_alt_prev.ptr == ಖಾಲಿ
					 && uhp->uh_ಮುಂದಿನ.ptr->uh_walk != mark)
	    uhp = uhp->uh_ಮುಂದಿನ.ptr;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uhp->uh_alt_prev.ptr != ಖಾಲಿ)
	    uhp = uhp->uh_alt_prev.ptr;
	ಇಲ್ಲದಿದ್ದರೆ
	    uhp = uhp->uh_ಮುಂದಿನ.ptr;
    }

    ಆದರೆ (undo_write_ಬೈಟುಗಳು(&bi, (long_u)UF_ಶೀರ್ಷಿಕೆ_ಕೊನೆ_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    ಆದರೆ (ಶೀರ್ಷಿಕೆಗಳು_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", ಶೀರ್ಷಿಕೆಗಳು_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    ಆದರೆ (bi.bi_state != ಖಾಲಿ && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# ಆದರೆ defined(UNIX) && defined(HAVE_FSYNC)
    ಆದರೆ ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && fflush(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    fclose(fp);
    ಆದರೆ (!write_ok)
	semsg(_(e_write_error_ಒಳ_undo_file_str), file_name);

# ಆದರೆ defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    ಆದರೆ (buf->b_ffname != ಖಾಲಿ)
	(ಶೂನ್ಯ)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    ಆದರೆ (buf->b_ffname != ಖಾಲಿ)
    {
	vim_acl_T	    acl;

	// For systems that support ACL: get the ACL from the original file.
	acl = mch_get_acl(buf->b_ffname);
	mch_ಗಣ_acl(file_name, acl);
	mch_free_acl(acl);
    }
# endif

theend:
# ifdef FEAT_CRYPT
    ಆದರೆ (bi.bi_state != ಖಾಲಿ)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_ಬಫರ್);
# endif
    ಆದರೆ (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
uhp_seq_cmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *v1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *v2)
{
    ಸ್ಥಿರಾಂಕ u_ಶೀರ್ಷಿಕೆ_T *u1 = *(u_ಶೀರ್ಷಿಕೆ_T **)v1;
    ಸ್ಥಿರಾಂಕ u_ಶೀರ್ಷಿಕೆ_T *u2 = *(u_ಶೀರ್ಷಿಕೆ_T **)v2;

    ಹಿಂತಿರುಗಿ u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
uhp_ಕೋಷ್ಟಕ_ಕಂಡುಹಿಡಿ(u_ಶೀರ್ಷಿಕೆ_T **uhp_ಕೋಷ್ಟಕ, ದೀರ್ಘ ಅಂಕೆ_ತಲೆ, ದೀರ್ಘ seq)
{
    ದೀರ್ಘ    lo = 0;
    ದೀರ್ಘ    hi = ಅಂಕೆ_ತಲೆ - 1;

    ಆಗ (lo <= hi)
    {
	ದೀರ್ಘ mid = lo + (hi - lo) / 2;

	ಆದರೆ (uhp_ಕೋಷ್ಟಕ[mid]->uh_seq < seq)
	    lo = mid + 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uhp_ಕೋಷ್ಟಕ[mid]->uh_seq > seq)
	    hi = mid - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ mid;
    }
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    ಶೂನ್ಯ
u_read_undo(char_u *name, char_u *hash, char_u *orig_name ಬಳಸದ)
{
    char_u	*file_name;
    FILE	*fp;
    ದೀರ್ಘ	ಆವೃತ್ತಿ_2, str_len;
    undoline_T	ಸಾಲು_ptr;
    linenr_T	ಸಾಲು_lnum;
    colnr_T	ಸಾಲು_colnr;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ;
    ದೀರ್ಘ	ಅಂಕೆ_ತಲೆ = 0;
    ದೀರ್ಘ	old_ಶೀರ್ಷಿಕೆ_seq, new_ಶೀರ್ಷಿಕೆ_seq, cur_ಶೀರ್ಷಿಕೆ_seq;
    ದೀರ್ಘ	seq_ಕೊನೆಯ, seq_cur;
    ದೀರ್ಘ	ಕೊನೆಯ_save_nr = 0;
    ದೀರ್ಘ	old_idx = -1, new_idx = -1, cur_idx = -1;
    ದೀರ್ಘ	ಅಂಕೆ_read_uhps = 0;
    time_t	seq_ಸಮಯ;
    ದೀರ್ಘ	i;
    ಪೂರ್ಣಾಂಕ		c;
    u_ಶೀರ್ಷಿಕೆ_T	*uhp;
    u_ಶೀರ್ಷಿಕೆ_T	**uhp_ಕೋಷ್ಟಕ = ಖಾಲಿ;
    char_u	read_hash[UNDO_HASH_ಗಾತ್ರ];
    char_u	magic_buf[UF_ಆರಂಭ_MAGIC_LEN];
# ifdef U_DEBUG
    ಪೂರ್ಣಾಂಕ		*uhp_ಕೋಷ್ಟಕ_ಬಳಸಿದ;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_ಕ್ಷೇತ್ರ(bi);
    ಸಾಲು_ptr.ul_len = 0;
    ಸಾಲು_ptr.ul_textlen = 0;
    ಸಾಲು_ptr.ul_ಸಾಲು = ಖಾಲಿ;

    ಆದರೆ (name == ಖಾಲಿ)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	ಆದರೆ (file_name == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	ಆದರೆ (mch_stat((ಅಕ್ಷರ *)orig_name, &st_orig) >= 0
		&& mch_stat((ಅಕ್ಷರ *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    ಆದರೆ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    ಹಿಂತಿರುಗಿ;
	}
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ
	file_name = name;

    ಆದರೆ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((ಅಕ್ಷರ *)file_name, "r");
    ಆದರೆ (fp == ಖಾಲಿ)
    {
	ಆದರೆ (name != ಖಾಲಿ || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	ಹೋಗು error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    ಆದರೆ (fread(magic_buf, UF_ಆರಂಭ_MAGIC_LEN, 1, fp) != 1
		|| memcmp(magic_buf, UF_ಆರಂಭ_MAGIC, UF_ಆರಂಭ_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	ಹೋಗು error;
    }
    ಆವೃತ್ತಿ_2 = get2c(fp);
    ಆದರೆ (ಆವೃತ್ತಿ_2 == UF_ಆವೃತ್ತಿ_CRYPT)
    {
# ifdef FEAT_CRYPT
	ಆದರೆ (*curbuf->b_p_ಕೀಲಿ == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    ಹೋಗು error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_ಕೀಲಿ);
	ಆದರೆ (bi.bi_state == ಖಾಲಿ)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    ಹೋಗು error;
	}
	ಆದರೆ (crypt_ಸಂಪೂರ್ಣ_undofile(bi.bi_state->ವಿಧಾನ_nr))
	{
	    bi.bi_ಬಫರ್ = alloc(CRYPT_BUF_ಗಾತ್ರ);
	    ಆದರೆ (bi.bi_ಬಫರ್ == ಖಾಲಿ)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = ಖಾಲಿ;
		ಹೋಗು error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_ಬಳಸಿದ = 0;
	}
# ಇಲ್ಲದಿದ್ದರೆ
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	ಹೋಗು error;
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆವೃತ್ತಿ_2 != UF_ಆವೃತ್ತಿ)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	ಹೋಗು error;
    }

    ಆದರೆ (undo_read(&bi, read_hash, (size_t)UNDO_HASH_ಗಾತ್ರ) == FAIL)
    {
	corruption_error("hash", file_name);
	ಹೋಗು error;
    }
    ಸಾಲು_ಎಣಿಕೆ = (linenr_T)undo_read_4c(&bi);
    ಆದರೆ (memcmp(hash, read_hash, UNDO_HASH_ಗಾತ್ರ) != 0
				  || ಸಾಲು_ಎಣಿಕೆ != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	ಆದರೆ (p_verbose > 0 || name != ಖಾಲಿ)
	{
	    ಆದರೆ (name == ಖಾಲಿ)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    ಆದರೆ (name == ಖಾಲಿ)
		verbose_leave();
	}
	ಹೋಗು error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    ಆದರೆ (str_len < 0)
	ಹೋಗು error;
    ಆದರೆ (str_len > 0)
    {
	ಸಾಲು_ptr.ul_ಸಾಲು = read_string_decrypt(&bi, str_len);
	ಸಾಲು_ptr.ul_len = str_len + 1;
	ಸಾಲು_ptr.ul_textlen = str_len;
    }
    ಸಾಲು_lnum = (linenr_T)undo_read_4c(&bi);
    ಸಾಲು_colnr = (colnr_T)undo_read_4c(&bi);
    ಆದರೆ (ಸಾಲು_lnum < 0 || ಸಾಲು_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	ಹೋಗು error;
    }

    // Begin general undo data
    old_ಶೀರ್ಷಿಕೆ_seq = undo_read_4c(&bi);
    new_ಶೀರ್ಷಿಕೆ_seq = undo_read_4c(&bi);
    cur_ಶೀರ್ಷಿಕೆ_seq = undo_read_4c(&bi);
    ಅಂಕೆ_ತಲೆ = undo_read_4c(&bi);
    seq_ಕೊನೆಯ = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_ಸಮಯ = undo_read_ಸಮಯ(&bi);

    // Optional header fields.
    ಗಾಗಿ (;;)
    {
	ಪೂರ್ಣಾಂಕ len = undo_read_byte(&bi);
	ಪೂರ್ಣಾಂಕ what;

	ಆದರೆ (len == 0 || len == EOF)
	    ನಿಲ್ಲಿಸು;
	what = undo_read_byte(&bi);
	ಆಯ್ಕೆ (what)
	{
	    ಸ್ಥಿತಿ UF_ಕೊನೆಯ_SAVE_NR:
		ಕೊನೆಯ_save_nr = undo_read_4c(&bi);
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		// field not supported, skip
		ಆಗ (--len >= 0)
		    (ಶೂನ್ಯ)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    ಆದರೆ (ಅಂಕೆ_ತಲೆ > 0)
    {
	ಆದರೆ (ಅಂಕೆ_ತಲೆ < LONG_MAX / (ದೀರ್ಘ)ಗಾತ್ರ(u_ಶೀರ್ಷಿಕೆ_T *))
	    uhp_ಕೋಷ್ಟಕ = U_ALLOC_ಸಾಲು(ಅಂಕೆ_ತಲೆ * ಗಾತ್ರ(u_ಶೀರ್ಷಿಕೆ_T *));
	ಆದರೆ (uhp_ಕೋಷ್ಟಕ == ಖಾಲಿ)
	    ಹೋಗು error;
    }

    ಆಗ ((c = undo_read_2c(&bi)) == UF_ಶೀರ್ಷಿಕೆ_MAGIC)
    {
	ಆದರೆ (ಅಂಕೆ_read_uhps >= ಅಂಕೆ_ತಲೆ)
	{
	    corruption_error("num_head too small", file_name);
	    ಹೋಗು error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	ಆದರೆ (uhp == ಖಾಲಿ)
	    ಹೋಗು error;
	uhp_ಕೋಷ್ಟಕ[ಅಂಕೆ_read_uhps++] = uhp;
    }

    ಆದರೆ (ಅಂಕೆ_read_uhps != ಅಂಕೆ_ತಲೆ)
    {
	corruption_error("num_head", file_name);
	ಹೋಗು error;
    }
    ಆದರೆ (c != UF_ಶೀರ್ಷಿಕೆ_ಕೊನೆ_MAGIC)
    {
	corruption_error("end marker", file_name);
	ಹೋಗು error;
    }

# ifdef U_DEBUG
    uhp_ಕೋಷ್ಟಕ_ಬಳಸಿದ = alloc_clear(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * ಅಂಕೆ_ತಲೆ + 1);
#  define ಗಣ_FLAG(j) ++uhp_ಕೋಷ್ಟಕ_ಬಳಸಿದ[j]
# ಇಲ್ಲದಿದ್ದರೆ
#  define ಗಣ_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    ಆದರೆ (ಅಂಕೆ_ತಲೆ > 0)
	qsort(uhp_ಕೋಷ್ಟಕ, (size_t)ಅಂಕೆ_ತಲೆ, ಗಾತ್ರ(u_ಶೀರ್ಷಿಕೆ_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_ತಲೆ - 1; i++)
    {
	ಆದರೆ (uhp_ಕೋಷ್ಟಕ[i]->uh_seq == uhp_ಕೋಷ್ಟಕ[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    ಹೋಗು error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    ಮಾಡು { \
	ದೀರ್ಘ fidx = uhp_ಕೋಷ್ಟಕ_ಕಂಡುಹಿಡಿ(uhp_ಕೋಷ್ಟಕ, ಅಂಕೆ_ತಲೆ, (link).seq); \
	\
	ಆದರೆ (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_ಕೋಷ್ಟಕ[fidx]; \
	    ಗಣ_FLAG(fidx); \
	} \
	ಇಲ್ಲದಿದ್ದರೆ \
	    (link).ptr = ಖಾಲಿ; \
    } ಆಗ (0)

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_ತಲೆ; i++)
    {
	uhp = uhp_ಕೋಷ್ಟಕ[i];
	SWIZZLE_SEQ(uhp->uh_ಮುಂದಿನ, i);
	SWIZZLE_SEQ(uhp->uh_prev, i);
	SWIZZLE_SEQ(uhp->uh_alt_ಮುಂದಿನ, i);
	SWIZZLE_SEQ(uhp->uh_alt_prev, i);
	ಆದರೆ (old_ಶೀರ್ಷಿಕೆ_seq > 0 && old_idx < 0 && uhp->uh_seq == old_ಶೀರ್ಷಿಕೆ_seq)
	{
	    old_idx = i;
	    ಗಣ_FLAG(i);
	}
	ಆದರೆ (new_ಶೀರ್ಷಿಕೆ_seq > 0 && new_idx < 0 && uhp->uh_seq == new_ಶೀರ್ಷಿಕೆ_seq)
	{
	    new_idx = i;
	    ಗಣ_FLAG(i);
	}
	ಆದರೆ (cur_ಶೀರ್ಷಿಕೆ_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_ಶೀರ್ಷಿಕೆ_seq)
	{
	    cur_idx = i;
	    ಗಣ_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? ಖಾಲಿ : uhp_ಕೋಷ್ಟಕ[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? ಖಾಲಿ : uhp_ಕೋಷ್ಟಕ[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? ಖಾಲಿ : uhp_ಕೋಷ್ಟಕ[cur_idx];
    curbuf->b_u_ಸಾಲು_ptr = ಸಾಲು_ptr;
    curbuf->b_u_ಸಾಲು_lnum = ಸಾಲು_lnum;
    curbuf->b_u_ಸಾಲು_colnr = ಸಾಲು_colnr;
    curbuf->b_u_numhead = ಅಂಕೆ_ತಲೆ;
    curbuf->b_u_seq_ಕೊನೆಯ = seq_ಕೊನೆಯ;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_ಸಮಯ_cur = seq_ಸಮಯ;
    curbuf->b_u_save_nr_ಕೊನೆಯ = ಕೊನೆಯ_save_nr;
    curbuf->b_u_save_nr_cur = ಕೊನೆಯ_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_ಕೋಷ್ಟಕ);

# ifdef U_DEBUG
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_ತಲೆ; ++i)
	ಆದರೆ (uhp_ಕೋಷ್ಟಕ_ಬಳಸಿದ[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_ಕೋಷ್ಟಕ_ಬಳಸಿದ);
    u_check(TRUE);
# endif

    ಆದರೆ (name != ಖಾಲಿ)
	smsg(_("Finished reading undo file %s"), file_name);
    ಹೋಗು theend;

error:
    vim_free(ಸಾಲು_ptr.ul_ಸಾಲು);
    ಆದರೆ (uhp_ಕೋಷ್ಟಕ != ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_read_uhps; i++)
	    ಆದರೆ (uhp_ಕೋಷ್ಟಕ[i] != ಖಾಲಿ)
		u_free_uhp(uhp_ಕೋಷ್ಟಕ[i]);
	vim_free(uhp_ಕೋಷ್ಟಕ);
    }

theend:
# ifdef FEAT_CRYPT
    ಆದರೆ (bi.bi_state != ಖಾಲಿ)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_ಬಫರ್);
# endif
    ಆದರೆ (fp != ಖಾಲಿ)
	fclose(fp);
    ಆದರೆ (file_name != name)
	vim_free(file_name);
    ಹಿಂತಿರುಗಿ;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    ಶೂನ್ಯ
u_undo(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    ಆದರೆ (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	ಎಣಿಕೆ_2 = 1;
    }

    ಆದರೆ (vim_strchr(p_cpo, CPO_UNDO) == ಖಾಲಿ)
	undo_undoes = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
	undo_undoes = !undo_undoes;
    u_doit(ಎಣಿಕೆ_2);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    ಶೂನ್ಯ
u_redo(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    ಆದರೆ (vim_strchr(p_cpo, CPO_UNDO) == ಖಾಲಿ)
	undo_undoes = FALSE;
    u_doit(ಎಣಿಕೆ_2);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_doit(ಪೂರ್ಣಾಂಕ startcount)
{
    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = startcount;

    ಆದರೆ (!undo_allowed())
	ಹಿಂತಿರುಗಿ;

    u_newcount = 0;
    u_oldcount = 0;
    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
	u_oldcount = -1;
    ಆಗ (ಎಣಿಕೆ_2--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	ಆದರೆ (undo_undoes)
	{
	    ಆದರೆ (curbuf->b_u_curhead == ಖಾಲಿ)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_ಮುಂದಿನ.ptr;
	    // nothing to undo
	    ಆದರೆ (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == ಖಾಲಿ)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		ಆದರೆ (ಎಣಿಕೆ_2 == startcount - 1)
		{
		    ಆದರೆ (!shortmess(SHM_UNDO))
			ಸಂದೇಶ(_("Already at oldest change"));
		    ಹಿಂತಿರುಗಿ;
		}
		ನಿಲ್ಲಿಸು;
	    }

	    u_undoredo(TRUE);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (curbuf->b_u_curhead == ಖಾಲಿ || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		ಆದರೆ (ಎಣಿಕೆ_2 == startcount - 1)
		{
		    ಆದರೆ (!shortmess(SHM_UNDO))
			ಸಂದೇಶ(_("Already at newest change"));
		    ಹಿಂತಿರುಗಿ;
		}
		ನಿಲ್ಲಿಸು;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    ಆದರೆ (curbuf->b_u_curhead->uh_prev.ptr == ಖಾಲಿ)
		curbuf->b_u_newhead = curbuf->b_u_curhead;
	    curbuf->b_u_curhead = curbuf->b_u_curhead->uh_prev.ptr;
	}
    }
    u_undo_ಕೊನೆ(undo_undoes, FALSE);
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
undo_ಸಮಯ(
    ದೀರ್ಘ	ಹೆಜ್ಜೆ,
    ಪೂರ್ಣಾಂಕ		sec,
    ಪೂರ್ಣಾಂಕ		file,
    ಪೂರ್ಣಾಂಕ		absolute)
{
    ದೀರ್ಘ	    ಗುರಿ;
    ದೀರ್ಘ	    closest;
    ದೀರ್ಘ	    closest_ಆರಂಭ;
    ದೀರ್ಘ	    closest_seq = 0;
    ದೀರ್ಘ	    val;
    u_ಶೀರ್ಷಿಕೆ_T	    *uhp = ಖಾಲಿ;
    u_ಶೀರ್ಷಿಕೆ_T	    *ಕೊನೆಯ;
    ಪೂರ್ಣಾಂಕ		    mark;
    ಪೂರ್ಣಾಂಕ		    nomark = 0;  // shut up compiler
    ಪೂರ್ಣಾಂಕ		    ಸುತ್ತು;
    ಪೂರ್ಣಾಂಕ		    dosec = sec;
    ಪೂರ್ಣಾಂಕ		    dofile = file;
    ಪೂರ್ಣಾಂಕ		    above = FALSE;
    ಪೂರ್ಣಾಂಕ		    did_undo = TRUE;

    ಆದರೆ (ಪಠ್ಯ_locked())
    {
	ಪಠ್ಯ_locked_ಸಂದೇಶ();
	ಹಿಂತಿರುಗಿ;
    }

    // First make sure the current undoable change is synced.
    ಆದರೆ (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    ಆದರೆ (absolute)
    {
	ಗುರಿ = ಹೆಜ್ಜೆ;
	closest = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (dosec)
	    ಗುರಿ = (ದೀರ್ಘ)(curbuf->b_u_ಸಮಯ_cur) + ಹೆಜ್ಜೆ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dofile)
	{
	    ಆದರೆ (ಹೆಜ್ಜೆ < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		ಆದರೆ (uhp != ಖಾಲಿ)
		    uhp = uhp->uh_ಮುಂದಿನ.ptr;
		ಇಲ್ಲದಿದ್ದರೆ
		    uhp = curbuf->b_u_newhead;
		ಆದರೆ (uhp != ಖಾಲಿ && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    ಗುರಿ = curbuf->b_u_save_nr_cur + ಹೆಜ್ಜೆ;
		ಇಲ್ಲದಿದ್ದರೆ
		    // count the changes since the last write as one step
		    ಗುರಿ = curbuf->b_u_save_nr_cur + ಹೆಜ್ಜೆ + 1;
		ಆದರೆ (ಗುರಿ <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Moving forward to a newer write.
		ಗುರಿ = curbuf->b_u_save_nr_cur + ಹೆಜ್ಜೆ;
		ಆದರೆ (ಗುರಿ > curbuf->b_u_save_nr_ಕೊನೆಯ)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    ಗುರಿ = curbuf->b_u_seq_ಕೊನೆಯ + 1;
		    dofile = FALSE;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗುರಿ = curbuf->b_u_seq_cur + ಹೆಜ್ಜೆ;
	ಆದರೆ (ಹೆಜ್ಜೆ < 0)
	{
	    ಆದರೆ (ಗುರಿ < 0)
		ಗುರಿ = 0;
	    closest = -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (dosec)
		closest = (ದೀರ್ಘ)(vim_ಸಮಯ() + 1);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dofile)
		closest = curbuf->b_u_save_nr_ಕೊನೆಯ + 2;
	    ಇಲ್ಲದಿದ್ದರೆ
		closest = curbuf->b_u_seq_ಕೊನೆಯ + 2;
	    ಆದರೆ (ಗುರಿ >= closest)
		ಗುರಿ = closest - 1;
	}
    }
    closest_ಆರಂಭ = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    ಆದರೆ (ಗುರಿ == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	ಹೋಗು ಗುರಿ_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	ಆದರೆ (curbuf->b_u_curhead == ಖಾಲಿ)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	ಇಲ್ಲದಿದ್ದರೆ
	    uhp = curbuf->b_u_curhead;

	ಆಗ (uhp != ಖಾಲಿ)
	{
	    uhp->uh_walk = mark;
	    ಆದರೆ (dosec)
		val = (ದೀರ್ಘ)(uhp->uh_ಸಮಯ);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dofile)
		val = uhp->uh_save_nr;
	    ಇಲ್ಲದಿದ್ದರೆ
		val = uhp->uh_seq;

	    ಆದರೆ (ಸುತ್ತು == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		ಆದರೆ ((ಹೆಜ್ಜೆ < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
			      : uhp->uh_seq > curbuf->b_u_seq_cur)
			&& ((dosec && val == closest)
			    ? (ಹೆಜ್ಜೆ < 0
				? uhp->uh_seq < closest_seq
				: uhp->uh_seq > closest_seq)
			    : closest == closest_ಆರಂಭ
				|| (val > ಗುರಿ
				    ? (closest > ಗುರಿ
					? val - ಗುರಿ <= closest - ಗುರಿ
					: val - ಗುರಿ <= ಗುರಿ - closest)
				    : (closest > ಗುರಿ
					? ಗುರಿ - val <= closest - ಗುರಿ
					: ಗುರಿ - val <= ಗುರಿ - closest))))
		{
		    closest = val;
		    closest_seq = uhp->uh_seq;
		}
	    }

	    // Quit searching when we found a match.  But when searching for a
	    // time we need to continue looking for the best uh_seq.
	    ಆದರೆ (ಗುರಿ == val && !dosec)
	    {
		ಗುರಿ = uhp->uh_seq;
		ನಿಲ್ಲಿಸು;
	    }

	    // go down in the tree if we haven't been there
	    ಆದರೆ (uhp->uh_prev.ptr != ಖಾಲಿ && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
		uhp = uhp->uh_prev.ptr;

	    // go to alternate branch if we haven't been there
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uhp->uh_alt_ಮುಂದಿನ.ptr != ಖಾಲಿ
		    && uhp->uh_alt_ಮುಂದಿನ.ptr->uh_walk != nomark
		    && uhp->uh_alt_ಮುಂದಿನ.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_ಮುಂದಿನ.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uhp->uh_ಮುಂದಿನ.ptr != ಖಾಲಿ && uhp->uh_alt_prev.ptr == ಖಾಲಿ
		    && uhp->uh_ಮುಂದಿನ.ptr->uh_walk != nomark
		    && uhp->uh_ಮುಂದಿನ.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		ಆದರೆ (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_ಮುಂದಿನ.ptr;
	    }

	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		ಆದರೆ (uhp->uh_alt_prev.ptr != ಖಾಲಿ)
		    uhp = uhp->uh_alt_prev.ptr;
		ಇಲ್ಲದಿದ್ದರೆ
		    uhp = uhp->uh_ಮುಂದಿನ.ptr;
	    }
	}

	ಆದರೆ (uhp != ಖಾಲಿ)    // found it
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (absolute)
	{
	    semsg(_(e_undo_ಸಂಖ್ಯೆ_nr_not_ಸಿಕ್ಕಿತು), ಹೆಜ್ಜೆ);
	    ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (closest == closest_ಆರಂಭ)
	{
	    ಆದರೆ (!shortmess(SHM_UNDO))
	    {
		ಆದರೆ (ಹೆಜ್ಜೆ < 0)
		    ಸಂದೇಶ(_("Already at oldest change"));
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸಂದೇಶ(_("Already at newest change"));
	    }
	    ಹಿಂತಿರುಗಿ;
	}

	ಗುರಿ = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	ಆದರೆ (ಹೆಜ್ಜೆ < 0)
	    above = TRUE;	// stop above the header
    }

ಗುರಿ_zero:
    // If we found it: Follow the path to go to where we want to be.
    ಆದರೆ (uhp != ಖಾಲಿ || ಗುರಿ == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	ಆಗ (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    ಆದರೆ (uhp == ಖಾಲಿ)
		uhp = curbuf->b_u_newhead;
	    ಇಲ್ಲದಿದ್ದರೆ
		uhp = uhp->uh_ಮುಂದಿನ.ptr;
	    ಆದರೆ (uhp == ಖಾಲಿ || (ಗುರಿ > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == ಗುರಿ && !above))
		ನಿಲ್ಲಿಸು;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    ಆದರೆ (ಗುರಿ > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	ಆದರೆ (ಗುರಿ > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    ಆಗ (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		ಆದರೆ (uhp == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;

		// Go back to the first branch with a mark.
		ಆಗ (uhp->uh_alt_prev.ptr != ಖಾಲಿ
				     && uhp->uh_alt_prev.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_prev.ptr;

		// Find the last branch with a mark, that's the one.
		ಕೊನೆಯ = uhp;
		ಆಗ (ಕೊನೆಯ->uh_alt_ಮುಂದಿನ.ptr != ಖಾಲಿ
				    && ಕೊನೆಯ->uh_alt_ಮುಂದಿನ.ptr->uh_walk == mark)
		    ಕೊನೆಯ = ಕೊನೆಯ->uh_alt_ಮುಂದಿನ.ptr;
		ಆದರೆ (ಕೊನೆಯ != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    ಆಗ (uhp->uh_alt_prev.ptr != ಖಾಲಿ)
			uhp = uhp->uh_alt_prev.ptr;
		    ಆದರೆ (ಕೊನೆಯ->uh_alt_ಮುಂದಿನ.ptr != ಖಾಲಿ)
			ಕೊನೆಯ->uh_alt_ಮುಂದಿನ.ptr->uh_alt_prev.ptr =
							ಕೊನೆಯ->uh_alt_prev.ptr;
		    ಕೊನೆಯ->uh_alt_prev.ptr->uh_alt_ಮುಂದಿನ.ptr =
							ಕೊನೆಯ->uh_alt_ಮುಂದಿನ.ptr;
		    ಕೊನೆಯ->uh_alt_prev.ptr = ಖಾಲಿ;
		    ಕೊನೆಯ->uh_alt_ಮುಂದಿನ.ptr = uhp;
		    uhp->uh_alt_prev.ptr = ಕೊನೆಯ;

		    ಆದರೆ (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = ಕೊನೆಯ;
		    uhp = ಕೊನೆಯ;
		    ಆದರೆ (uhp->uh_ಮುಂದಿನ.ptr != ಖಾಲಿ)
			uhp->uh_ಮುಂದಿನ.ptr->uh_prev.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		ಆದರೆ (uhp->uh_walk != mark)
		    ನಿಲ್ಲಿಸು;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		ಆದರೆ (uhp->uh_seq == ಗುರಿ && above)
		{
		    curbuf->b_u_seq_cur = ಗುರಿ - 1;
		    ನಿಲ್ಲಿಸು;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		ಆದರೆ (uhp->uh_prev.ptr == ಖಾಲಿ)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_prev.ptr;
		did_undo = FALSE;

		ಆದರೆ (uhp->uh_seq == ಗುರಿ)	// found it!
		    ನಿಲ್ಲಿಸು;

		uhp = uhp->uh_prev.ptr;
		ಆದರೆ (uhp == ಖಾಲಿ || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    ಆಂತರಿಕ_error("undo_time()");
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
    }
    u_undo_ಕೊನೆ(did_undo, absolute);
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
u_undoredo(ಪೂರ್ಣಾಂಕ undo)
{
    undoline_T	*newarray = ಖಾಲಿ;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	ತುದಿ, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    ದೀರ್ಘ	i;
    u_ನಮೂದು_T	*uep, *nuep;
    u_ನಮೂದು_T	*newlist = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		old_flags;
    ಪೂರ್ಣಾಂಕ		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_ಬಫರ್;		    // buffer became empty
    u_ಶೀರ್ಷಿಕೆ_T	*curhead = curbuf->b_u_curhead;

    // Don't want autocommands using the undo structures here, they are
    // invalid till the end.
    ಖಂಡ_autocmds();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
    u_check(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
    old_flags = curhead->uh_flags;
    new_flags = (curbuf->b_ಬದಲಾಯಿತು ? UH_ಬದಲಾಯಿತು : 0) +
	       ((curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು) ? UH_EMPTYBUF : 0);
    setpcmark();

    /*
     * save marks before undo/redo
     */
    mch_memmove(namedm, curbuf->b_namedm, ಗಾತ್ರ(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_ಆರಂಭ.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    curbuf->b_op_ಆರಂಭ.ಲಂಬಸಾಲು = 0;
    curbuf->b_op_ಕೊನೆ.lnum = 0;
    curbuf->b_op_ಕೊನೆ.ಲಂಬಸಾಲು = 0;

    ಗಾಗಿ (uep = curhead->uh_ನಮೂದು; uep != ಖಾಲಿ; uep = nuep)
    {
	ತುದಿ = uep->ue_ತುದಿ;
	bot = uep->ue_bot;
	ಆದರೆ (bot == 0)
	    bot = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
	ಆದರೆ (ತುದಿ > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || ತುದಿ >= bot
				      || bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_ಸಾಲು_numbers_wrong);
	    ಬದಲಾಯಿತು();		// don't want UNCHANGED now
	    ಹಿಂತಿರುಗಿ;
	}

	oldsize = bot - ತುದಿ - 1;    // number of lines before undo
	newsize = uep->ue_ಗಾತ್ರ;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    ಆದರೆ (lnum >= ತುದಿ && lnum <= ತುದಿ + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ತುದಿ < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		ಗಾಗಿ (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(ತುದಿ + 1 + i);

		    ಆದರೆ (curbuf->b_ml.ml_ಸಾಲು_len != uep->ue_array[i].ul_len
			    || memcmp(uep->ue_array[i].ul_ಸಾಲು, p,
						curbuf->b_ml.ml_ಸಾಲು_len) != 0)
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (i == newsize && newlnum == MAXLNUM && uep->ue_ಮುಂದಿನ == ಖಾಲಿ)
		{
		    newlnum = ತುದಿ;
		    new_curpos.lnum = newlnum + 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i < newsize)
		{
		    newlnum = ತುದಿ + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	ಖಾಲಿಗಳು_ಬಫರ್ = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	ಆದರೆ (oldsize > 0)
	{
	    ಆದರೆ ((newarray = U_ALLOC_ಸಾಲು(ಗಾತ್ರ(undoline_T) * oldsize)) == ಖಾಲಿ)
	    {
		do_outofmem_ಸಂದೇಶ((long_u)(ಗಾತ್ರ(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		ಆಗ (uep != ಖಾಲಿ)
		{
		    nuep = uep->ue_ಮುಂದಿನ;
		    u_freeentry(uep, uep->ue_ಗಾತ್ರ);
		    uep = nuep;
		}
		ನಿಲ್ಲಿಸು;
	    }
	    // delete backwards, it goes faster in most cases
	    ಗಾಗಿ (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		ಆದರೆ (u_save_ಸಾಲು(&newarray[i], lnum) == FAIL)
		    do_outofmem_ಸಂದೇಶ((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		ಆದರೆ (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == 1)
		    ಖಾಲಿಗಳು_ಬಫರ್ = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    newarray = ಖಾಲಿ;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	ಆದರೆ (newsize)
	{
	    ಗಾಗಿ (lnum = ತುದಿ, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		ಆದರೆ (ಖಾಲಿಗಳು_ಬಫರ್ && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_ಸಾಲು,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		ಇಲ್ಲದಿದ್ದರೆ
		    ml_append_flags(lnum, uep->ue_array[i].ul_ಸಾಲು,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_ಸಾಲು);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, ತುದಿ, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, ತುದಿ + newsize + 1, &uep->ue_props,
								UNDOPROP_ಕೆಳಗೆ);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, ತುದಿ, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_ಕೆಳಗೆ);
	ಆದರೆ (newsize > 0)
	    adjust_props_for_undo(curbuf, ತುದಿ + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_ಕೆಳಗೆ);
#ಆದರೆ_ಅಂತ್ಯ

	// adjust marks
	ಆದರೆ (oldsize != newsize)
	{
	    mark_adjust(ತುದಿ + 1, ತುದಿ + oldsize, (ದೀರ್ಘ)MAXLNUM,
					       (ದೀರ್ಘ)newsize - (ದೀರ್ಘ)oldsize);
	    ಆದರೆ (curbuf->b_op_ಆರಂಭ.lnum > ತುದಿ + oldsize)
		curbuf->b_op_ಆರಂಭ.lnum += newsize - oldsize;
	    ಆದರೆ (curbuf->b_op_ಕೊನೆ.lnum > ತುದಿ + oldsize)
		curbuf->b_op_ಕೊನೆ.lnum += newsize - oldsize;
	}
	ಆದರೆ (oldsize > 0 || newsize > 0)
	{
	    ಬದಲಾಯಿತು_ಸಾಲುಗಳು(ತುದಿ + 1, 0, bot, newsize - oldsize);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    ಆದರೆ (spell_check_ಕಿಟಕಿ(curwin) && bot <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		redrawWinline(curwin, bot);
#ಆದರೆ_ಅಂತ್ಯ
	}

	// Set the '[ mark.
	ಆದರೆ (ತುದಿ + 1 < curbuf->b_op_ಆರಂಭ.lnum)
	    curbuf->b_op_ಆರಂಭ.lnum = ತುದಿ + 1;
	// Set the '] mark.
	ಆದರೆ (newsize == 0 && ತುದಿ + 1 > curbuf->b_op_ಕೊನೆ.lnum)
	    curbuf->b_op_ಕೊನೆ.lnum = ತುದಿ + 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ತುದಿ + newsize > curbuf->b_op_ಕೊನೆ.lnum)
	    curbuf->b_op_ಕೊನೆ.lnum = ತುದಿ + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_ಗಾತ್ರ = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = ತುದಿ + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_ಮುಂದಿನ;
	uep->ue_ಮುಂದಿನ = newlist;
	newlist = uep;
    }

    // Ensure the '[ and '] marks are within bounds.
    ಆದರೆ (curbuf->b_op_ಆರಂಭ.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	 curbuf->b_op_ಆರಂಭ.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಆದರೆ (curbuf->b_op_ಕೊನೆ.lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	 curbuf->b_op_ಕೊನೆ.lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_ನಮೂದು = newlist;
    curhead->uh_flags = new_flags;
    ಆದರೆ ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_ಖಾಲಿಗಳು;
    ಆದರೆ (old_flags & UH_ಬದಲಾಯಿತು)
	ಬದಲಾಯಿತು();
    ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	ಆದರೆ (!isNetbeansModified(curbuf))
#ಆದರೆ_ಅಂತ್ಯ
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    ಗಾಗಿ (i = 0; i < NMARKS; ++i)
    {
	ಆದರೆ (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	ಆದರೆ (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	ಇಲ್ಲದಿದ್ದರೆ
	    curhead->uh_namedm[i].lnum = 0;
    }
    ಆದರೆ (curhead->uh_visual.vi_ಆರಂಭ.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    ಆದರೆ (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    ಆದರೆ (curwin->w_cursor.lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	ಆದರೆ (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.ಲಂಬಸಾಲು = curhead->uh_cursor.ಲಂಬಸಾಲು;
	    ಆದರೆ (virtual_ಸಕ್ರಿಯ() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    ಇಲ್ಲದಿದ್ದರೆ
		curwin->w_cursor.coladd = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    beginline(BL_SOL | BL_FIX);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// We get here with the current cursor line being past the end (eg
	// after adding lines at the end of the file, and then undoing it).
	// check_cursor() will move the cursor to the last line.  Move it to
	// the first column here.
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
	curwin->w_cursor.coladd = 0;
    }

    // Make sure the cursor is on an existing line and column.
    check_cursor();

    // Remember where we are for "g-" and ":earlier 10s".
    curbuf->b_u_seq_cur = curhead->uh_seq;
    ಆದರೆ (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	ಆದರೆ (curhead->uh_ಮುಂದಿನ.ptr != ಖಾಲಿ)
	    curbuf->b_u_seq_cur = curhead->uh_ಮುಂದಿನ.ptr->uh_seq;
	ಇಲ್ಲದಿದ್ದರೆ
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    ಆದರೆ (curhead->uh_save_nr != 0)
    {
	ಆದರೆ (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr;
    }

    // The timestamp can be the same for multiple changes, just use the one of
    // the undone/redone change.
    curbuf->b_u_ಸಮಯ_cur = curhead->uh_ಸಮಯ;

    unblock_autocmds();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
    u_check(FALSE);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * If we deleted or added lines, report the number of less/more lines.
 * Otherwise, report the number of changes (this may be incorrect
 * in some cases, but it's better than nothing).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_undo_ಕೊನೆ(
    ಪೂರ್ಣಾಂಕ		did_undo,	// just did an undo
    ಪೂರ್ಣಾಂಕ		absolute)	// used ":undo N"
{
    ಅಕ್ಷರ	*msgstr;
    u_ಶೀರ್ಷಿಕೆ_T	*uhp;
    char_u	msgbuf[80];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಆದರೆ ((fdo_flags & FDO_UNDO) && ಕೀಲಿ_Typed)
	foldOpenCursor();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
	--u_newcount;

    u_oldcount -= u_newcount;
    ಆದರೆ (u_oldcount == -1)
	msgstr = N_("more line");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (u_oldcount < 0)
	msgstr = N_("more lines");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (u_oldcount == 1)
	msgstr = N_("line less");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (u_oldcount > 1)
	msgstr = N_("fewer lines");
    ಇಲ್ಲದಿದ್ದರೆ
    {
	u_oldcount = u_newcount;
	ಆದರೆ (u_newcount == 1)
	    msgstr = N_("change");
	ಇಲ್ಲದಿದ್ದರೆ
	    msgstr = N_("changes");
    }

    ಆದರೆ (curbuf->b_u_curhead != ಖಾಲಿ)
    {
	// For ":undo N" we prefer a "after #N" message.
	ಆದರೆ (absolute && curbuf->b_u_curhead->uh_ಮುಂದಿನ.ptr != ಖಾಲಿ)
	{
	    uhp = curbuf->b_u_curhead->uh_ಮುಂದಿನ.ptr;
	    did_undo = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (did_undo)
	    uhp = curbuf->b_u_curhead;
	ಇಲ್ಲದಿದ್ದರೆ
	    uhp = curbuf->b_u_curhead->uh_ಮುಂದಿನ.ptr;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	uhp = curbuf->b_u_newhead;

    ಆದರೆ (uhp == ಖಾಲಿ)
	*msgbuf = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೂಡಿಸು_ಸಮಯ(msgbuf, ಗಾತ್ರ(msgbuf), uhp->uh_ಸಮಯ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ಆದರೆ (wp->w_ಬಫರ್ == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == ಖಾಲಿ ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    ಶೂನ್ಯ
u_sync(
    ಪೂರ್ಣಾಂಕ	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    ಆದರೆ (curbuf->b_u_synced || (!force && no_u_sync > 0))
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ಆದರೆ (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	ಹಿಂತಿರುಗಿ;		    // XIM is busy, don't break an undo sequence
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    ಇಲ್ಲದಿದ್ದರೆ
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = ಖಾಲಿ;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    ಶೂನ್ಯ
ex_undolist(exarg_T *eap ಬಳಸದ)
{
    garray_T	ga;
    u_ಶೀರ್ಷಿಕೆ_T	*uhp;
    ಪೂರ್ಣಾಂಕ		mark;
    ಪೂರ್ಣಾಂಕ		nomark;
    ಪೂರ್ಣಾಂಕ		changes = 1;
    ಪೂರ್ಣಾಂಕ		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ *), 20);

    uhp = curbuf->b_u_oldhead;
    ಆಗ (uhp != ಖಾಲಿ)
    {
	ಆದರೆ (uhp->uh_prev.ptr == ಖಾಲಿ && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    ಆದರೆ (ga_grow(&ga, 1) == FAIL)
		ನಿಲ್ಲಿಸು;
	    len = vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    ಕೂಡಿಸು_ಸಮಯ(IObuff + len, IOSIZE - len, uhp->uh_ಸಮಯ);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (ಪೂರ್ಣಾಂಕ)STRLEN(IObuff + len);
	    ಆದರೆ (uhp->uh_save_nr > 0)
	    {
		ಪೂರ್ಣಾಂಕ n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((ಅಕ್ಷರ *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_ದತ್ತಾಂಶ))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	ಆದರೆ (uhp->uh_prev.ptr != ಖಾಲಿ && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_prev.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uhp->uh_alt_ಮುಂದಿನ.ptr != ಖಾಲಿ
		&& uhp->uh_alt_ಮುಂದಿನ.ptr->uh_walk != nomark
		&& uhp->uh_alt_ಮುಂದಿನ.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_ಮುಂದಿನ.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uhp->uh_ಮುಂದಿನ.ptr != ಖಾಲಿ && uhp->uh_alt_prev.ptr == ಖಾಲಿ
		&& uhp->uh_ಮುಂದಿನ.ptr->uh_walk != nomark
		&& uhp->uh_ಮುಂದಿನ.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_ಮುಂದಿನ.ptr;
	    --changes;
	}

	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    ಆದರೆ (uhp->uh_alt_prev.ptr != ಖಾಲಿ)
		uhp = uhp->uh_alt_prev.ptr;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		uhp = uhp->uh_ಮುಂದಿನ.ptr;
		--changes;
	    }
	}
    }

    ಆದರೆ (ga.ga_len == 0)
	ಸಂದೇಶ(_("Nothing to undo"));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ		i;

	ವಿಂಗಡಿಸು_ಅಕ್ಷರಸರಣಿಗಳು((char_u **)ga.ga_ದತ್ತಾಂಶ, ga.ga_len);

	ಸಂದೇಶ_ಆರಂಭ();
	ಸಂದೇಶ_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	ಗಾಗಿ (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    ಸಂದೇಶ_putchar('\n');
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	    ಸಂದೇಶ_puts(((ಅಕ್ಷರ **)ga.ga_ದತ್ತಾಂಶ)[i]);
	}
	ಸಂದೇಶ_ಕೊನೆ();

	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    ಶೂನ್ಯ
ex_undojoin(exarg_T *eap ಬಳಸದ)
{
    ಆದರೆ (curbuf->b_u_newhead == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;		    // nothing changed before
    ಆದರೆ (curbuf->b_u_curhead != ಖಾಲಿ)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (!curbuf->b_u_synced)
	ಹಿಂತಿರುಗಿ;		    // already unsynced
    ಆದರೆ (get_undolevel() < 0)
	ಹಿಂತಿರುಗಿ;		    // no entries, nothing to do
    ಇಲ್ಲದಿದ್ದರೆ
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
u_ಕಂಡುಹಿಡಿ_ಮೊದಲ_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    u_ಶೀರ್ಷಿಕೆ_T	*uhp = curbuf->b_u_newhead;
    u_ನಮೂದು_T   *uep;
    linenr_T	lnum;

    ಆದರೆ (curbuf->b_u_curhead != ಖಾಲಿ || uhp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_ನಮೂದು;
    ಆದರೆ (uep->ue_ತುದಿ != 0 || uep->ue_bot != 0)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (lnum = 1; lnum < curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
					      && lnum <= uep->ue_ಗಾತ್ರ; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	ಆದರೆ (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_ಸಾಲು_len
		|| memcmp(p, uep->ue_array[lnum - 1].ul_ಸಾಲು, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಆದರೆ (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ != uep->ue_ಗಾತ್ರ)
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
    u_ಶೀರ್ಷಿಕೆ_T	*uhp;

    ++buf->b_u_save_nr_ಕೊನೆಯ;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_ಕೊನೆಯ;
    uhp = buf->b_u_curhead;
    ಆದರೆ (uhp != ಖಾಲಿ)
	uhp = uhp->uh_ಮುಂದಿನ.ptr;
    ಇಲ್ಲದಿದ್ದರೆ
	uhp = buf->b_u_newhead;
    ಆದರೆ (uhp != ಖಾಲಿ)
	uhp->uh_save_nr = buf->b_u_save_nr_ಕೊನೆಯ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
u_unch_branch(u_ಶೀರ್ಷಿಕೆ_T *uhp)
{
    u_ಶೀರ್ಷಿಕೆ_T	*uh;

    ಗಾಗಿ (uh = uhp; uh != ಖಾಲಿ; uh = uh->uh_prev.ptr)
    {
	uh->uh_flags |= UH_ಬದಲಾಯಿತು;
	ಆದರೆ (uh->uh_alt_ಮುಂದಿನ.ptr != ಖಾಲಿ)
	    u_unch_branch(uh->uh_alt_ಮುಂದಿನ.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    ಸ್ಥಿರ u_ನಮೂದು_T *
u_get_headentry(ಶೂನ್ಯ)
{
    ಆದರೆ (curbuf->b_u_newhead == ಖಾಲಿ || curbuf->b_u_newhead->uh_ನಮೂದು == ಖಾಲಿ)
    {
	iemsg(e_undo_list_corrupt);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ curbuf->b_u_newhead->uh_ನಮೂದು;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_getbot(ಶೂನ್ಯ)
{
    u_ನಮೂದು_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    ಆದರೆ (uep == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    uep = curbuf->b_u_newhead->uh_getbot_ನಮೂದು;
    ಆದರೆ (uep != ಖಾಲಿ)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - uep->ue_lcount;
	uep->ue_bot = uep->ue_ತುದಿ + uep->ue_ಗಾತ್ರ + 1 + extra;
	ಆದರೆ (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    iemsg(e_undo_ಸಾಲು_missing);
	    uep->ue_bot = uep->ue_ತುದಿ + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_ನಮೂದು = ಖಾಲಿ;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_freeheader(
    buf_T	    *buf,
    u_ಶೀರ್ಷಿಕೆ_T	    *uhp,
    u_ಶೀರ್ಷಿಕೆ_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_ಶೀರ್ಷಿಕೆ_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    ಆದರೆ (uhp->uh_alt_ಮುಂದಿನ.ptr != ಖಾಲಿ)
	u_freebranch(buf, uhp->uh_alt_ಮುಂದಿನ.ptr, uhpp);

    ಆದರೆ (uhp->uh_alt_prev.ptr != ಖಾಲಿ)
	uhp->uh_alt_prev.ptr->uh_alt_ಮುಂದಿನ.ptr = ಖಾಲಿ;

    // Update the links in the list to remove the header.
    ಆದರೆ (uhp->uh_ಮುಂದಿನ.ptr == ಖಾಲಿ)
	buf->b_u_oldhead = uhp->uh_prev.ptr;
    ಇಲ್ಲದಿದ್ದರೆ
	uhp->uh_ಮುಂದಿನ.ptr->uh_prev.ptr = uhp->uh_prev.ptr;

    ಆದರೆ (uhp->uh_prev.ptr == ಖಾಲಿ)
	buf->b_u_newhead = uhp->uh_ಮುಂದಿನ.ptr;
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (uhap = uhp->uh_prev.ptr; uhap != ಖಾಲಿ;
						 uhap = uhap->uh_alt_ಮುಂದಿನ.ptr)
	    uhap->uh_ಮುಂದಿನ.ptr = uhp->uh_ಮುಂದಿನ.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_freebranch(
    buf_T	    *buf,
    u_ಶೀರ್ಷಿಕೆ_T	    *uhp,
    u_ಶೀರ್ಷಿಕೆ_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_ಶೀರ್ಷಿಕೆ_T	    *tofree, *ಮುಂದಿನ;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    ಆದರೆ (uhp == buf->b_u_oldhead)
    {
	ಆಗ (buf->b_u_oldhead != ಖಾಲಿ)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (uhp->uh_alt_prev.ptr != ಖಾಲಿ)
	uhp->uh_alt_prev.ptr->uh_alt_ಮುಂದಿನ.ptr = ಖಾಲಿ;

    ಮುಂದಿನ = uhp;
    ಆಗ (ಮುಂದಿನ != ಖಾಲಿ)
    {
	tofree = ಮುಂದಿನ;
	ಆದರೆ (tofree->uh_alt_ಮುಂದಿನ.ptr != ಖಾಲಿ)
	    u_freebranch(buf, tofree->uh_alt_ಮುಂದಿನ.ptr, uhpp);   // recursive
	ಮುಂದಿನ = tofree->uh_prev.ptr;
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
    u_ಶೀರ್ಷಿಕೆ_T	    *uhp,
    u_ಶೀರ್ಷಿಕೆ_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_ನಮೂದು_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    ಆದರೆ (buf->b_u_curhead == uhp)
	buf->b_u_curhead = ಖಾಲಿ;
    ಆದರೆ (buf->b_u_newhead == uhp)
	buf->b_u_newhead = ಖಾಲಿ;  // freeing the newest entry
    ಆದರೆ (uhpp != ಖಾಲಿ && uhp == *uhpp)
	*uhpp = ಖಾಲಿ;

    ಗಾಗಿ (uep = uhp->uh_ನಮೂದು; uep != ಖಾಲಿ; uep = nuep)
    {
	nuep = uep->ue_ಮುಂದಿನ;
	u_freeentry(uep, uep->ue_ಗಾತ್ರ);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
    uhp->uh_magic = 0;
#ಆದರೆ_ಅಂತ್ಯ
    vim_free((char_u *)uhp);
    --buf->b_u_numhead;
}

/*
 * free entry 'uep' and 'n' lines in uep->ue_array[]
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_freeentry(u_ನಮೂದು_T *uep, ದೀರ್ಘ n)
{
    ಆಗ (n > 0)
	vim_free(uep->ue_array[--n].ul_ಸಾಲು);
    vim_free((char_u *)uep->ue_array);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ga_clear(&uep->ue_props);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_DEBUG
    uep->ue_magic = 0;
#ಆದರೆ_ಅಂತ್ಯ
    vim_free((char_u *)uep);
}

/*
 * invalidate the undo buffer; called when storage has already been released
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = ಖಾಲಿ;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು = ಖಾಲಿ;
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
    ಆಗ (buf->b_u_oldhead != ಖಾಲಿ)
	u_freeheader(buf, buf->b_u_oldhead, ಖಾಲಿ);
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
    ಆದರೆ (lnum == curbuf->b_u_ಸಾಲು_lnum)	    // line is already saved
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (lnum < 1 || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ) // should never happen
	ಹಿಂತಿರುಗಿ;
    u_clearline();
    curbuf->b_u_ಸಾಲು_lnum = lnum;
    ಆದರೆ (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_ಸಾಲು_colnr = curwin->w_cursor.ಲಂಬಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ
	curbuf->b_u_ಸಾಲು_colnr = 0;
    ಆದರೆ (u_save_ಸಾಲು(&curbuf->b_u_ಸಾಲು_ptr, lnum) == FAIL)
	do_outofmem_ಸಂದೇಶ((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    ಶೂನ್ಯ
u_clearline(ಶೂನ್ಯ)
{
    ಆದರೆ (curbuf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

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

    ಆದರೆ (undo_off)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (curbuf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು == ಖಾಲಿ
			|| curbuf->b_u_ಸಾಲು_lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	beep_flush();
	ಹಿಂತಿರುಗಿ;
    }

    // first save the line for the 'u' command
    ಆದರೆ (u_savecommon(curbuf->b_u_ಸಾಲು_lnum - 1,
		       curbuf->b_u_ಸಾಲು_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (u_save_ಸಾಲು(&oldp, curbuf->b_u_ಸಾಲು_lnum) == FAIL)
    {
	do_outofmem_ಸಂದೇಶ((long_u)0);
	ಹಿಂತಿರುಗಿ;
    }
    ml_replace_len(curbuf->b_u_ಸಾಲು_lnum, curbuf->b_u_ಸಾಲು_ptr.ul_ಸಾಲು,
				     curbuf->b_u_ಸಾಲು_ptr.ul_len, TRUE, FALSE);
    ಬದಲಾಯಿತು_ಬೈಟುಗಳು(curbuf->b_u_ಸಾಲು_lnum, 0);
    curbuf->b_u_ಸಾಲು_ptr = oldp;

    t = curbuf->b_u_ಸಾಲು_colnr;
    ಆದರೆ (curwin->w_cursor.lnum == curbuf->b_u_ಸಾಲು_lnum)
	curbuf->b_u_ಸಾಲು_colnr = curwin->w_cursor.ಲಂಬಸಾಲು;
    curwin->w_cursor.ಲಂಬಸಾಲು = t;
    curwin->w_cursor.lnum = curbuf->b_u_ಸಾಲು_lnum;
    check_cursor_ಲಂಬಸಾಲು();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    ಪೂರ್ಣಾಂಕ
buf_Is_ಬದಲಾಯಿತು(buf_T *buf)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (ಪದ_ಕೆಲಸ_running_not_none(buf->b_ಪದ))
	ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ buf_Is_ಬದಲಾಯಿತು_Not_ಪದ(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    ಪೂರ್ಣಾಂಕ
any_Buf_Is_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(buf))
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    ಪೂರ್ಣಾಂಕ
buf_Is_ಬದಲಾಯಿತು_Not_ಪದ(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    ಹಿಂತಿರುಗಿ (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_ಬದಲಾಯಿತು || file_ff_differs(buf, TRUE));
}

    ಪೂರ್ಣಾಂಕ
curbuf_Is_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ buf_Is_ಬದಲಾಯಿತು(curbuf);
}

#ಆದರೆ defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
u_eval_ವೃಕ್ಷ(buf_T *buf, u_ಶೀರ್ಷಿಕೆ_T *ಮೊದಲ_uhp, list_T *list)
{
    u_ಶೀರ್ಷಿಕೆ_T  *uhp = ಮೊದಲ_uhp;
    dict_T	*dict;

    ಆಗ (uhp != ಖಾಲಿ)
    {
	dict = dict_alloc();
	ಆದರೆ (dict == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "seq", uhp->uh_seq);
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "time", (ದೀರ್ಘ)uhp->uh_ಸಮಯ);
	ಆದರೆ (uhp == buf->b_u_newhead)
	    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "newhead", 1);
	ಆದರೆ (uhp == buf->b_u_curhead)
	    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "curhead", 1);
	ಆದರೆ (uhp->uh_save_nr > 0)
	    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "save", uhp->uh_save_nr);

	ಆದರೆ (uhp->uh_alt_ಮುಂದಿನ.ptr != ಖಾಲಿ)
	{
	    list_T	*alt_list = list_alloc();

	    ಆದರೆ (alt_list != ಖಾಲಿ)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_ವೃಕ್ಷ(buf, uhp->uh_alt_ಮುಂದಿನ.ptr, alt_list);
		dict_ಕೂಡಿಸು_list(dict, "alt", alt_list);
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
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	ಆದರೆ (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u *ffname = ತುಂಬಿದ_Name_save(fname, TRUE);

	    ಆದರೆ (ffname != ಖಾಲಿ)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# ಇಲ್ಲದಿದ್ದರೆ
    rettv->vval.v_string = ಖಾಲಿ;
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

    ಆದರೆ (!buf->b_p_udf)
	ಹಿಂತಿರುಗಿ;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    ಆದರೆ (file_name != ಖಾಲಿ)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"undofile", 0L, ಖಾಲಿ, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    ಶೂನ್ಯ
f_undotree(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    dict_T *dict = rettv->vval.v_dict;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "synced", (ದೀರ್ಘ)buf->b_u_synced);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "seq_last", buf->b_u_seq_ಕೊನೆಯ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "save_last", buf->b_u_save_nr_ಕೊನೆಯ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "seq_cur", buf->b_u_seq_cur);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "time_cur", (ದೀರ್ಘ)buf->b_u_ಸಮಯ_cur);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    ಆದರೆ (list != ಖಾಲಿ)
    {
	u_eval_ವೃಕ್ಷ(buf, buf->b_u_oldhead, list);
	dict_ಕೂಡಿಸು_list(dict, "entries", list);
    }
}

#ಆದರೆ_ಅಂತ್ಯ
