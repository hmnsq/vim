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

#include "vim.h"

// Structure passed around between functions.
// Avoids passing cryptstate_T when encryption not available.
typedef sa_nracanaa {
    buf_T	*bi_buf;
    FILE	*bi_fp;
#ifdef FEAT_CRYPT
    cryptstate_T *bi_state;
    char_u	*bi_buffer; // CRYPT_BUF_SIZE, NULL when not buffering
    size_t	bi_used;    // bytes written to/read from bi_buffer
    size_t	bi_avail;   // bytes available in bi_buffer
#endif
} bufinfo_T;


static void u_unch_branch(u_header_T *uhp);
static u_entry_T *u_get_headentry(void);
static void u_getbot(void);
static void u_doit(puur_naa_nka count);
static void u_undoredo(puur_naa_nka undo);
static void u_undo_end(puur_naa_nka did_undo, puur_naa_nka absolute);
static void u_freeheader(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
static void u_freebranch(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
static void u_freeentries(buf_T *buf, u_header_T *uhp, u_header_T **uhpp);
static void u_freeentry(u_entry_T *, long);
#ifdef FEAT_PERSISTENT_UNDO
# ifdef FEAT_CRYPT
static puur_naa_nka undo_flush(bufinfo_T *bi);
# endif
static puur_naa_nka undo_read(bufinfo_T *bi, char_u *buffer, size_t size);
static puur_naa_nka serialize_uep(bufinfo_T *bi, u_entry_T *uep);
static u_entry_T *unserialize_uep(bufinfo_T *bi, puur_naa_nka *error, char_u *file_name);
static void serialize_pos(bufinfo_T *bi, pos_T pos);
static void unserialize_pos(bufinfo_T *bi, pos_T *pos);
static void serialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
static void unserialize_visualinfo(bufinfo_T *bi, visualinfo_T *info);
#endif
static void u_saveline(linenr_T lnum);
static void u_blockfree(buf_T *buf);

#define U_ALLOC_LINE(size) lalloc(size, FALSE)

// used in undo_end() to report number of added and deleted lines
static long	u_newcount, u_oldcount;

/*
 * When 'u' flag included in 'cpoptions', we behave like vi.  Need to remember
 * the action that "u" should do.
 */
static puur_naa_nka	undo_undoes = FALSE;

static puur_naa_nka	lastmark = 0;

#ᰀᰧ defined(U_DEBUG)
/*
 * Check the undo structures for being valid.  Print a warning when something
 * looks wrong.
 */
static puur_naa_nka seen_b_u_curhead;
static puur_naa_nka seen_b_u_newhead;
static puur_naa_nka header_count;

    static void
u_check_tree(u_header_T *uhp,
	u_header_T *exp_uh_next,
	u_header_T *exp_uh_alt_prev)
{
    u_entry_T *uep;

    ᰀᰧ (uhp == NULL)
	ᰀᰧᰌ;
    ++header_count;
    ᰀᰧ (uhp == curbuf->b_u_curhead && ++seen_b_u_curhead > 1)
    {
	emsg("b_u_curhead found twice (looping?)");
	ᰀᰧᰌ;
    }
    ᰀᰧ (uhp == curbuf->b_u_newhead && ++seen_b_u_newhead > 1)
    {
	emsg("b_u_newhead found twice (looping?)");
	ᰀᰧᰌ;
    }

    ᰀᰧ (uhp->uh_magic != UH_MAGIC)
	emsg("uh_magic wrong (may be using freed memory)");
    ᰀᰧᰀ
    {
	// Check pointers back are correct.
	ᰀᰧ (uhp->uh_next.ptr != exp_uh_next)
	{
	    emsg("uh_next wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
					       exp_uh_next, uhp->uh_next.ptr);
	}
	ᰀᰧ (uhp->uh_alt_prev.ptr != exp_uh_alt_prev)
	{
	    emsg("uh_alt_prev wrong");
	    smsg("expected: 0x%x, actual: 0x%x",
				       exp_uh_alt_prev, uhp->uh_alt_prev.ptr);
	}

	// Check the undo tree at this header.
	for (uep = uhp->uh_entry; uep != NULL; uep = uep->ue_next)
	{
	    ᰀᰧ (uep->ue_magic != UE_MAGIC)
	    {
		emsg("ue_magic wrong (may be using freed memory)");
		bha_nga;
	    }
	}

	// Check the next alt tree.
	u_check_tree(uhp->uh_alt_next.ptr, uhp->uh_next.ptr, uhp);

	// Check the next header in this branch.
	u_check_tree(uhp->uh_prev.ptr, uhp, NULL);
    }
}

    static void
u_check(puur_naa_nka newhead_may_be_NULL)
{
    seen_b_u_newhead = 0;
    seen_b_u_curhead = 0;
    header_count = 0;

    u_check_tree(curbuf->b_u_oldhead, NULL, NULL);

    ᰀᰧ (seen_b_u_newhead == 0 && curbuf->b_u_oldhead != NULL
	    && !(newhead_may_be_NULL && curbuf->b_u_newhead == NULL))
	semsg("b_u_newhead invalid: 0x%x", curbuf->b_u_newhead);
    ᰀᰧ (curbuf->b_u_curhead != NULL && seen_b_u_curhead == 0)
	semsg("b_u_curhead invalid: 0x%x", curbuf->b_u_curhead);
    ᰀᰧ (header_count != curbuf->b_u_numhead)
    {
	emsg("b_u_numhead invalid");
	smsg("expected: %ld, actual: %ld",
			       (long)header_count, (long)curbuf->b_u_numhead);
    }
}
#endif

/*
 * Save the current line for both the "u" and "U" command.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns OK or FAIL.
 */
    puur_naa_nka
u_save_cursor(void)
{
    ᰀᰧᰌ (u_save((linenr_T)(curwin->w_cursor.lnum - 1),
				      (linenr_T)(curwin->w_cursor.lnum + 1)));
}

/*
 * Save the lines between "top" and "bot" for both the "u" and "U" command.
 * "top" may be 0 and "bot" may be curbuf->b_ml.ml_line_count + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    puur_naa_nka
u_save(linenr_T top, linenr_T bot)
{
    ᰀᰧ (undo_off)
	ᰀᰧᰌ OK;

    ᰀᰧ (top >= bot || bot > curbuf->b_ml.ml_line_count + 1)
	ᰀᰧᰌ FAIL;	// rely on caller to give an error message

    ᰀᰧ (top + 2 == bot)
	u_saveline((linenr_T)(top + 1));

    ᰀᰧᰌ (u_savecommon(top, bot, (linenr_T)0, FALSE));
}

/*
 * Save the line "lnum" (used by ":s" and "~" command).
 * The line is replaced, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    puur_naa_nka
u_savesub(linenr_T lnum)
{
    ᰀᰧ (undo_off)
	ᰀᰧᰌ OK;

    ᰀᰧᰌ (u_savecommon(lnum - 1, lnum + 1, lnum + 1, FALSE));
}

/*
 * A new line is inserted before line "lnum" (used by :s command).
 * The line is inserted, so the new bottom line is lnum + 1.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    puur_naa_nka
u_inssub(linenr_T lnum)
{
    ᰀᰧ (undo_off)
	ᰀᰧᰌ OK;

    ᰀᰧᰌ (u_savecommon(lnum - 1, lnum, lnum + 1, FALSE));
}

/*
 * Save the lines "lnum" - "lnum" + nlines (used by delete command).
 * The lines are deleted, so the new bottom line is lnum, unless the buffer
 * becomes empty.
 * Careful: may trigger autocommands that reload the buffer.
 * Returns FAIL when lines could not be saved, OK otherwise.
 */
    puur_naa_nka
u_savedel(linenr_T lnum, long nlines)
{
    ᰀᰧ (undo_off)
	ᰀᰧᰌ OK;

    ᰀᰧᰌ (u_savecommon(lnum - 1, lnum + nlines,
		     nlines == curbuf->b_ml.ml_line_count ? 2 : lnum, FALSE));
}

/*
 * Return TRUE when undo is allowed.  Otherwise give an error message and
 * return FALSE.
 */
    puur_naa_nka
undo_allowed(void)
{
    // Don't allow changes when 'modifiable' is off.
    ᰀᰧ (!curbuf->b_p_ma)
    {
	emsg(_(e_cannot_make_changes_modifiable_is_off));
	ᰀᰧᰌ FALSE;
    }

#ifdef HAVE_SANDBOX
    // In the sandbox it's not allowed to change the text.
    ᰀᰧ (sandbox != 0)
    {
	emsg(_(e_not_allowed_in_sandbox));
	ᰀᰧᰌ FALSE;
    }
#endif

    // Don't allow changes in the buffer while editing the cmdline.  The
    // caller of getcmdline() may get confused.
    ᰀᰧ (textlock != 0)
    {
	emsg(_(e_not_allowed_to_change_text_or_change_window));
	ᰀᰧᰌ FALSE;
    }

    ᰀᰧᰌ TRUE;
}

/*
 * Get the undolevel value for the current buffer.
 */
    static long
get_undolevel(void)
{
    ᰀᰧ (curbuf->b_p_ul == NO_LOCAL_UNDOLEVEL)
	ᰀᰧᰌ p_ul;
    ᰀᰧᰌ curbuf->b_p_ul;
}

/*
 * u_save_line(): save an allocated copy of line "lnum" into "ul".
 * Returns FAIL when out of memory.
 */
    static puur_naa_nka
u_save_line(undoline_T *ul, linenr_T lnum)
{
    char_u *line = ml_get(lnum);

    ul->ul_textlen = ml_get_len(lnum);
    ᰀᰧ (curbuf->b_ml.ml_line_len == 0)
    {
	ul->ul_len = 1;
	ul->ul_line = vim_strsave((char_u *)"");
    }
    ᰀᰧᰀ
    {
	// This uses the length in the memline, thus text properties are
	// included.
	ul->ul_len = curbuf->b_ml.ml_line_len;
	ul->ul_line = vim_memsave(line, ul->ul_len);
    }
    ᰀᰧᰌ ul->ul_line == NULL ? FAIL : OK;
}

#ifdef FEAT_PROP_POPUP
/*
 * return TRUE if line "lnum" has text property "flags".
 */
    static puur_naa_nka
has_prop_w_flags(linenr_T lnum, puur_naa_nka flags)
{
    char_u  *props;
    puur_naa_nka	    i;
    puur_naa_nka	    proplen = get_text_props(curbuf, lnum, &props, FALSE);

    for (i = 0; i < proplen; ++i)
    {
	textprop_T prop;

	mch_memmove(&prop, props + i * parimaa_na prop, parimaa_na prop);
	ᰀᰧ (prop.tp_flags & flags)
	    ᰀᰧᰌ TRUE;
    }
    ᰀᰧᰌ FALSE;
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
    puur_naa_nka
u_savecommon(
    linenr_T	top,
    linenr_T	bot,
    linenr_T	newbot,
    puur_naa_nka		reload)
{
    linenr_T	lnum;
    long	i;
    u_header_T	*uhp;
    u_header_T	*old_curhead;
    u_entry_T	*uep;
    u_entry_T	*prev_uep;
    long	size;

    ᰀᰧ (!reload)
    {
	// When making changes is not allowed return FAIL.  It's a crude way
	// to make all change commands fail.
	ᰀᰧ (!undo_allowed())
	    ᰀᰧᰌ FAIL;

#ifdef FEAT_NETBEANS_INTG
	/*
	 * Netbeans defines areas that cannot be modified.  Bail out here when
	 * trying to change text in a guarded area.
	 */
	ᰀᰧ (netbeans_active())
	{
	    ᰀᰧ (netbeans_is_guarded(top, bot))
	    {
		emsg(_(e_region_is_guarded_cannot_modify));
		ᰀᰧᰌ FAIL;
	    }
	    ᰀᰧ (curbuf->b_p_ro)
	    {
		emsg(_(e_netbeans_does_not_allow_changes_in_read_only_files));
		ᰀᰧᰌ FAIL;
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
	ᰀᰧ (bot > curbuf->b_ml.ml_line_count + 1)
	{
	    // This happens when the FileChangedRO autocommand changes the
	    // file in a way it becomes shorter.
	    emsg(_(e_line_count_changed_unexpectedly));
	    ᰀᰧᰌ FAIL;
	}
    }

#ifdef U_DEBUG
    u_check(FALSE);
#endif

#ifdef FEAT_PROP_POPUP
    // Include the line above if a text property continues from it.
    // Include the line below if a text property continues to it.
    ᰀᰧ (bot - top > 1)
    {
	ᰀᰧ (top > 0 && has_prop_w_flags(top + 1, TP_FLAG_CONT_PREV))
	    --top;
	ᰀᰧ (bot <= curbuf->b_ml.ml_line_count
			       && has_prop_w_flags(bot - 1, TP_FLAG_CONT_NEXT))
	{
	    ++bot;
	    ᰀᰧ (newbot != 0)
		++newbot;
	}
    }
#endif

    size = bot - top - 1;

    /*
     * If curbuf->b_u_synced is true make a new header.
     */
    ᰀᰧ (curbuf->b_u_synced)
    {
	// Need to create new entry in b_changelist.
	curbuf->b_new_change = true;

	ᰀᰧ (get_undolevel() >= 0)
	{
	    /*
	     * Make a new header entry.  Do this first so that we don't mess
	     * up the undo info when out of memory.
	     */
	    uhp = U_ALLOC_LINE(parimaa_na(u_header_T));
	    ᰀᰧ (uhp == NULL)
		goto nomem;
#ifdef U_DEBUG
	    uhp->uh_magic = UH_MAGIC;
#endif
	}
	ᰀᰧᰀ
	    uhp = NULL;

	/*
	 * If we undid more than we redid, move the entry lists before and
	 * including curbuf->b_u_curhead to an alternate branch.
	 */
	old_curhead = curbuf->b_u_curhead;
	ᰀᰧ (old_curhead != NULL)
	{
	    curbuf->b_u_newhead = old_curhead->uh_next.ptr;
	    curbuf->b_u_curhead = NULL;
	}

	/*
	 * free headers to keep the size right
	 */
	ᰀᰬ (curbuf->b_u_numhead > get_undolevel()
					       && curbuf->b_u_oldhead != NULL)
	{
	    u_header_T	    *uhfree = curbuf->b_u_oldhead;

	    ᰀᰧ (uhfree == old_curhead)
		// Can't reconnect the branch, delete all of it.
		u_freebranch(curbuf, uhfree, &old_curhead);
	    ᰀᰧᰀ ᰀᰧ (uhfree->uh_alt_next.ptr == NULL)
		// There is no branch, only free one header.
		u_freeheader(curbuf, uhfree, &old_curhead);
	    ᰀᰧᰀ
	    {
		// Free the oldest alternate branch as a whole.
		ᰀᰬ (uhfree->uh_alt_next.ptr != NULL)
		    uhfree = uhfree->uh_alt_next.ptr;
		u_freebranch(curbuf, uhfree, &old_curhead);
	    }
#ifdef U_DEBUG
	    u_check(TRUE);
#endif
	}

	ᰀᰧ (uhp == NULL)		// no undo at all
	{
	    ᰀᰧ (old_curhead != NULL)
		u_freebranch(curbuf, old_curhead, NULL);
	    curbuf->b_u_synced = false;
	    ᰀᰧᰌ OK;
	}

	uhp->uh_prev.ptr = NULL;
	uhp->uh_next.ptr = curbuf->b_u_newhead;
	uhp->uh_alt_next.ptr = old_curhead;
	ᰀᰧ (old_curhead != NULL)
	{
	    uhp->uh_alt_prev.ptr = old_curhead->uh_alt_prev.ptr;
	    ᰀᰧ (uhp->uh_alt_prev.ptr != NULL)
		uhp->uh_alt_prev.ptr->uh_alt_next.ptr = uhp;
	    old_curhead->uh_alt_prev.ptr = uhp;
	    ᰀᰧ (curbuf->b_u_oldhead == old_curhead)
		curbuf->b_u_oldhead = uhp;
	}
	ᰀᰧᰀ
	    uhp->uh_alt_prev.ptr = NULL;
	ᰀᰧ (curbuf->b_u_newhead != NULL)
	    curbuf->b_u_newhead->uh_prev.ptr = uhp;

	uhp->uh_seq = ++curbuf->b_u_seq_last;
	curbuf->b_u_seq_cur = uhp->uh_seq;
	uhp->uh_time = vim_time();
	uhp->uh_save_nr = 0;
	curbuf->b_u_time_cur = uhp->uh_time + 1;

	uhp->uh_walk = 0;
	uhp->uh_entry = NULL;
	uhp->uh_getbot_entry = NULL;
	uhp->uh_cursor = curwin->w_cursor;	// save cursor pos. for undo
	ᰀᰧ (virtual_active() && curwin->w_cursor.coladd > 0)
	    uhp->uh_cursor_vcol = getviscol();
	ᰀᰧᰀ
	    uhp->uh_cursor_vcol = -1;

	// save changed and buffer empty flag for undo
	uhp->uh_flags = (curbuf->b_changed ? UH_CHANGED : 0) +
		       ((curbuf->b_ml.ml_flags & ML_EMPTY) ? UH_EMPTYBUF : 0);

	// save named marks and Visual marks for undo
	mch_memmove(uhp->uh_namedm, curbuf->b_namedm, parimaa_na(pos_T) * NMARKS);
	uhp->uh_visual = curbuf->b_visual;

	curbuf->b_u_newhead = uhp;
	ᰀᰧ (curbuf->b_u_oldhead == NULL)
	    curbuf->b_u_oldhead = uhp;
	++curbuf->b_u_numhead;
    }
    ᰀᰧᰀ
    {
	ᰀᰧ (get_undolevel() < 0)	// no undo at all
	    ᰀᰧᰌ OK;

	/*
	 * When saving a single line, and it has been saved just before, it
	 * doesn't make sense saving it again.  Saves a lot of memory when
	 * making lots of changes inside the same line.
	 * This is only possible if the previous change didn't increase or
	 * decrease the number of lines.
	 * Check the ten last changes.  More doesn't make sense and takes too
	 * long.
	 */
	ᰀᰧ (size == 1)
	{
	    uep = u_get_headentry();
	    prev_uep = NULL;
	    for (i = 0; i < 10; ++i)
	    {
		ᰀᰧ (uep == NULL)
		    bha_nga;

		// If lines have been inserted/deleted we give up.
		// Also when the line was included in a multi-line save.
		ᰀᰧ ((curbuf->b_u_newhead->uh_getbot_entry != uep
			    ? (uep->ue_top + uep->ue_size + 1
				!= (uep->ue_bot == 0
				    ? curbuf->b_ml.ml_line_count + 1
				    : uep->ue_bot))
			    : uep->ue_lcount != curbuf->b_ml.ml_line_count)
			|| (uep->ue_size > 1
			    && top >= uep->ue_top
			    && top + 2 <= uep->ue_top + uep->ue_size + 1))
		    bha_nga;

		// If it's the same line we can skip saving it again.
		ᰀᰧ (uep->ue_size == 1 && uep->ue_top == top)
		{
		    ᰀᰧ (i > 0)
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
		    ᰀᰧ (newbot != 0)
			uep->ue_bot = newbot;
		    ᰀᰧᰀ ᰀᰧ (bot > curbuf->b_ml.ml_line_count)
			uep->ue_bot = 0;
		    ᰀᰧᰀ
		    {
			uep->ue_lcount = curbuf->b_ml.ml_line_count;
			curbuf->b_u_newhead->uh_getbot_entry = uep;
		    }
		    ᰀᰧᰌ OK;
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
    uep = U_ALLOC_LINE(parimaa_na(u_entry_T));
    ᰀᰧ (uep == NULL)
	goto nomem;
    CLEAR_POINTER(uep);
#ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
#endif

    uep->ue_size = size;
    uep->ue_top = top;
    ᰀᰧ (newbot != 0)
	uep->ue_bot = newbot;
    /*
     * Use 0 for ue_bot if bot is below last line.
     * Otherwise we have to compute ue_bot later.
     */
    ᰀᰧᰀ ᰀᰧ (bot > curbuf->b_ml.ml_line_count)
	uep->ue_bot = 0;
    ᰀᰧᰀ
    {
	uep->ue_lcount = curbuf->b_ml.ml_line_count;
	curbuf->b_u_newhead->uh_getbot_entry = uep;
    }

    ᰀᰧ (size > 0)
    {
	ᰀᰧ ((uep->ue_array = U_ALLOC_LINE(parimaa_na(undoline_T) * size)) == NULL)
	{
	    u_freeentry(uep, 0L);
	    goto nomem;
	}
	for (i = 0, lnum = top + 1; i < size; ++i)
	{
	    fast_breakcheck();
	    ᰀᰧ (got_int)
	    {
		u_freeentry(uep, i);
		ᰀᰧᰌ FAIL;
	    }
	    ᰀᰧ (u_save_line(&uep->ue_array[i], lnum++) == FAIL)
	    {
		u_freeentry(uep, i);
		goto nomem;
	    }
	}
    }
    ᰀᰧᰀ
	uep->ue_array = NULL;
    uep->ue_next = curbuf->b_u_newhead->uh_entry;
    curbuf->b_u_newhead->uh_entry = uep;
    curbuf->b_u_synced = false;
    undo_undoes = FALSE;

#ifdef U_DEBUG
    u_check(FALSE);
#endif
    ᰀᰧᰌ OK;

nomem:
    msg_silent = 0;	// must display the prompt
    ᰀᰧ (ask_yesno((char_u *)_("No undo possible; continue anyway"), TRUE)
								       == 'y')
    {
	undo_off = TRUE;	    // will be reset when character typed
	ᰀᰧᰌ OK;
    }
    do_outofmem_msg((long_u)0);
    ᰀᰧᰌ FAIL;
}

#ᰀᰧ defined(FEAT_PERSISTENT_UNDO)

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
    void
u_compute_hash(char_u *hash)
{
    context_sha256_T	ctx;
    linenr_T		lnum;

    sha256_start(&ctx);
    for (lnum = 1; lnum <= curbuf->b_ml.ml_line_count; ++lnum)
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
    static char_u *
u_get_undo_file_name(char_u *buf_ffname, puur_naa_nka reading)
{
    char_u	*dirp;
    char_u	dir_name[IOSIZE + 1];
    string_T	munged_name = {NULL, 0};
    char_u	*undo_file_name = NULL;
    puur_naa_nka		dir_len;
    char_u	*p;
    size_t	plen;
    stat_T	st;
    char_u	*ffname = buf_ffname;
    size_t	ffnamelen;
# ifdef HAVE_READLINK
    char_u	fname_buf[MAXPATHL];
# endif

    ᰀᰧ (ffname == NULL)
	ᰀᰧᰌ NULL;

# ifdef HAVE_READLINK
    // Expand symlink in the file name, so that we put the undo file with the
    // actual file instead of with the symlink.
    ᰀᰧ (resolve_symlink(ffname, fname_buf) == OK)
	ffname = fname_buf;
# endif

    ffnamelen = STRLEN(ffname);
    // Loop over 'undodir'.  When reading find the first file that exists.
    // When not reading use the first directory that exists or ".".
    dirp = p_udir;
    ᰀᰬ (*dirp != NUL)
    {
	dir_len = copy_option_part(&dirp, dir_name, IOSIZE, ",");
	ᰀᰧ (dir_len == 1 && dir_name[0] == '.')
	{
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    undo_file_name = vim_strnsave(ffname, ffnamelen + 5);
	    ᰀᰧ (undo_file_name == NULL)
		bha_nga;
	    p = gettail(undo_file_name);
	    plen = (size_t)(ffnamelen - (p - undo_file_name));
# ifdef VMS
	    // VMS can not handle more than one dot in the filenames
	    // use "dir/name" -> "dir/_un_name" - add _un_
	    // at the beginning to keep the extension
	    mch_memmove(p + 4,  p, plen + 1);
	    mch_memmove(p, "_un_", 4);

# ᰀᰧᰀ
	    // Use same directory as the ffname,
	    // "dir/name" -> "dir/.name.un~"
	    mch_memmove(p + 1, p, plen + 1);
	    *p = '.';
	    STRCPY(p + plen + 1, ".un~");
# endif
	}
	ᰀᰧᰀ
	{
	    dir_name[dir_len] = NUL;
	    ᰀᰧ (mch_isdir(dir_name))
	    {
		string_T    ret;

		ᰀᰧ (munged_name.string == NULL)
		{
		    munged_name.string = vim_strnsave(ffname, ffnamelen);
		    ᰀᰧ (munged_name.string == NULL)
			ᰀᰧᰌ NULL;
		    munged_name.length = ffnamelen;
		    for (p = munged_name.string; *p != NUL; MB_PTR_ADV(p))
			ᰀᰧ (vim_ispathsep(*p))
			    *p = '%';
		}
		undo_file_name = concat_fnames(dir_name, (size_t)dir_len,
		    munged_name.string, munged_name.length, TRUE, &ret);
	    }
	}

	// When reading check if the file exists.
	ᰀᰧ (undo_file_name != NULL && (!reading
			       || mch_stat((var_na *)undo_file_name, &st) >= 0))
	    bha_nga;
	VIM_CLEAR(undo_file_name);
    }

    vim_free(munged_name.string);
    ᰀᰧᰌ undo_file_name;
}

    static void
corruption_error(var_na *mesg, char_u *file_name)
{
    semsg(_(e_corrupted_undo_file_str_str), mesg, file_name);
}

    static void
u_free_uhp(u_header_T *uhp)
{
    u_entry_T	*nuep;
    u_entry_T	*uep;

    uep = uhp->uh_entry;
    ᰀᰬ (uep != NULL)
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
    static puur_naa_nka
undo_write(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    ᰀᰧ (bi->bi_buffer != NULL)
    {
	size_t	len_todo = len;
	char_u  *p = ptr;

	ᰀᰬ (bi->bi_used + len_todo >= CRYPT_BUF_SIZE)
	{
	    size_t	n = CRYPT_BUF_SIZE - bi->bi_used;

	    mch_memmove(bi->bi_buffer + bi->bi_used, p, n);
	    len_todo -= n;
	    p += n;
	    bi->bi_used = CRYPT_BUF_SIZE;
	    ᰀᰧ (undo_flush(bi) == FAIL)
		ᰀᰧᰌ FAIL;
	}
	ᰀᰧ (len_todo > 0)
	{
	    mch_memmove(bi->bi_buffer + bi->bi_used, p, len_todo);
	    bi->bi_used += len_todo;
	}
	ᰀᰧᰌ OK;
    }
# endif
    ᰀᰧ (fwrite(ptr, len, (size_t)1, bi->bi_fp) != 1)
	ᰀᰧᰌ FAIL;
    ᰀᰧᰌ OK;
}

# ifdef FEAT_CRYPT
    static puur_naa_nka
undo_flush(bufinfo_T *bi)
{
    ᰀᰧ (bi->bi_buffer != NULL && bi->bi_state != NULL && bi->bi_used > 0)
    {
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_used, FALSE);
	ᰀᰧ (fwrite(bi->bi_buffer, bi->bi_used, (size_t)1, bi->bi_fp) != 1)
	    ᰀᰧᰌ FAIL;
	bi->bi_used = 0;
    }
    ᰀᰧᰌ OK;
}
# endif

/*
 * Write "ptr[len]" and crypt the bytes when needed.
 * Returns OK or FAIL.
 */
    static puur_naa_nka
fwrite_crypt(bufinfo_T *bi, char_u *ptr, size_t len)
{
# ifdef FEAT_CRYPT
    char_u  *copy;
    char_u  small_buf[100];
    size_t  i;

    ᰀᰧ (bi->bi_state != NULL && bi->bi_buffer == NULL)
    {
	// crypting every piece of text separately
	ᰀᰧ (len < 100)
	    copy = small_buf;  // no malloc()/free() for short strings
	ᰀᰧᰀ
	{
	    copy = lalloc(len, FALSE);
	    ᰀᰧ (copy == NULL)
		ᰀᰧᰌ 0;
	}
	// Last parameter is only used for sodium encryption and that
	// explicitly disables encryption of undofiles.
	crypt_encode(bi->bi_state, ptr, len, copy, TRUE);
	i = fwrite(copy, len, (size_t)1, bi->bi_fp);
	ᰀᰧ (copy != small_buf)
	    vim_free(copy);
	ᰀᰧᰌ i == 1 ? OK : FAIL;
    }
# endif
    ᰀᰧᰌ undo_write(bi, ptr, len);
}

/*
 * Write a number, MSB first, in "len" bytes.
 * Must match with undo_read_?c() functions.
 * Returns OK or FAIL.
 */
    static puur_naa_nka
undo_write_bytes(bufinfo_T *bi, long_u nr, puur_naa_nka len)
{
    char_u  buf[8];
    puur_naa_nka	    i;
    puur_naa_nka	    bufi = 0;

    for (i = len - 1; i >= 0; --i)
	buf[bufi++] = (char_u)(nr >> (i * 8));
    ᰀᰧᰌ undo_write(bi, buf, (size_t)len);
}

/*
 * Write the pointer to an undo header.  Instead of writing the pointer itself
 * we use the sequence number of the header.  This is converted back to
 * pointers when reading. */
    static void
put_header_ptr(bufinfo_T *bi, u_header_T *uhp)
{
    undo_write_bytes(bi, (long_u)(uhp != NULL ? uhp->uh_seq : 0), 4);
}

    static puur_naa_nka
undo_read_4c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ᰀᰧ (bi->bi_buffer != NULL)
    {
	char_u  buf[4];
	puur_naa_nka	n;

	undo_read(bi, buf, (size_t)4);
	n = ((unsigned)buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	ᰀᰧᰌ n;
    }
# endif
    ᰀᰧᰌ get4c(bi->bi_fp);
}

    static puur_naa_nka
undo_read_2c(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ᰀᰧ (bi->bi_buffer != NULL)
    {
	char_u  buf[2];
	puur_naa_nka	n;

	undo_read(bi, buf, (size_t)2);
	n = (buf[0] << 8) + buf[1];
	ᰀᰧᰌ n;
    }
# endif
    ᰀᰧᰌ get2c(bi->bi_fp);
}

    static puur_naa_nka
undo_read_byte(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ᰀᰧ (bi->bi_buffer != NULL)
    {
	char_u  buf[1];

	undo_read(bi, buf, (size_t)1);
	ᰀᰧᰌ buf[0];
    }
# endif
    ᰀᰧᰌ getc(bi->bi_fp);
}

    static time_t
undo_read_time(bufinfo_T *bi)
{
# ifdef FEAT_CRYPT
    ᰀᰧ (bi->bi_buffer != NULL)
    {
	char_u  buf[8];
	time_t	n = 0;
	puur_naa_nka	i;

	undo_read(bi, buf, (size_t)8);
	for (i = 0; i < 8; ++i)
	    n = (n << 8) + buf[i];
	ᰀᰧᰌ n;
    }
# endif
    ᰀᰧᰌ get8ctime(bi->bi_fp);
}

/*
 * Read "buffer[size]" from the undo file.
 * Return OK or FAIL.
 */
    static puur_naa_nka
undo_read(bufinfo_T *bi, char_u *buffer, size_t size)
{
    puur_naa_nka retval = OK;

# ifdef FEAT_CRYPT
    ᰀᰧ (bi->bi_buffer != NULL)
    {
	puur_naa_nka	size_todo = (puur_naa_nka)size;
	char_u	*p = buffer;

	ᰀᰬ (size_todo > 0)
	{
	    size_t n;

	    ᰀᰧ (bi->bi_used >= bi->bi_avail)
	    {
		n = fread(bi->bi_buffer, 1, (size_t)CRYPT_BUF_SIZE, bi->bi_fp);
		ᰀᰧ (n == 0)
		{
		    retval = FAIL;
		    bha_nga;
		}
		bi->bi_avail = n;
		bi->bi_used = 0;
		crypt_decode_inplace(bi->bi_state, bi->bi_buffer, bi->bi_avail, FALSE);
	    }
	    n = size_todo;
	    ᰀᰧ (n > bi->bi_avail - bi->bi_used)
		n = bi->bi_avail - bi->bi_used;
	    mch_memmove(p, bi->bi_buffer + bi->bi_used, n);
	    bi->bi_used += n;
	    size_todo -= (puur_naa_nka)n;
	    p += n;
	}
    }
    ᰀᰧᰀ
# endif
    ᰀᰧ (fread(buffer, size, 1, bi->bi_fp) != 1)
	retval = FAIL;

    ᰀᰧ (retval == FAIL)
	// Error may be checked for only later.  Fill with zeros,
	// so that the reader won't use garbage.
	vim_memset(buffer, 0, size);
    ᰀᰧᰌ retval;
}

/*
 * Read a string of length "len" from "bi->bi_fd".
 * "len" can be zero to allocate an empty line.
 * Decrypt the bytes if needed.
 * Append a NUL.
 * Returns a pointer to allocated memory or NULL for failure.
 */
    static char_u *
read_string_decrypt(bufinfo_T *bi, puur_naa_nka len)
{
    char_u  *ptr = alloc(len + 1);

    ᰀᰧ (ptr == NULL)
	ᰀᰧᰌ NULL;

    ᰀᰧ (len > 0 && undo_read(bi, ptr, len) == FAIL)
    {
	vim_free(ptr);
	ᰀᰧᰌ NULL;
    }
    // In case there are text properties there already is a NUL, but
    // checking for that is more expensive than just adding a dummy byte.
    ptr[len] = NUL;
# ifdef FEAT_CRYPT
    ᰀᰧ (bi->bi_state != NULL && bi->bi_buffer == NULL)
	crypt_decode_inplace(bi->bi_state, ptr, len, FALSE);
# endif
    ᰀᰧᰌ ptr;
}

/*
 * Writes the (not encrypted) header and initializes encryption if needed.
 */
    static puur_naa_nka
serialize_header(bufinfo_T *bi, char_u *hash)
{
    buf_T	*buf = bi->bi_buf;
    FILE	*fp = bi->bi_fp;
    char_u	time_buf[8];

    // Start writing, first the magic marker and undo info version.
    ᰀᰧ (fwrite(UF_START_MAGIC, (size_t)UF_START_MAGIC_LEN, (size_t)1, fp) != 1)
	ᰀᰧᰌ FAIL;

    // If the buffer is encrypted then all text bytes following will be
    // encrypted.  Numbers and other info is not crypted.
# ifdef FEAT_CRYPT
    ᰀᰧ (*buf->b_p_key != NUL)
    {
	char_u *header;
	puur_naa_nka    header_len;
	long	len;

	undo_write_bytes(bi, (long_u)UF_VERSION_CRYPT, 2);
	bi->bi_state = crypt_create_for_writing(crypt_get_method_nr(buf),
					  buf->b_p_key, &header, &header_len);
	ᰀᰧ (bi->bi_state == NULL)
	    ᰀᰧᰌ FAIL;
	len = (long)fwrite(header, (size_t)header_len, (size_t)1, fp);
	vim_free(header);
	ᰀᰧ (len != 1)
	{
	    crypt_free_state(bi->bi_state);
	    bi->bi_state = NULL;
	    ᰀᰧᰌ FAIL;
	}

	ᰀᰧ (crypt_whole_undofile(crypt_get_method_nr(buf)))
	{
	    bi->bi_buffer = alloc(CRYPT_BUF_SIZE);
	    ᰀᰧ (bi->bi_buffer == NULL)
	    {
		crypt_free_state(bi->bi_state);
		bi->bi_state = NULL;
		ᰀᰧᰌ FAIL;
	    }
	    bi->bi_used = 0;
	}
    }
    ᰀᰧᰀ
# endif
	undo_write_bytes(bi, (long_u)UF_VERSION, 2);


    // Write a hash of the buffer text, so that we can verify it is still the
    // same when reading the buffer text.
    ᰀᰧ (undo_write(bi, hash, (size_t)UNDO_HASH_SIZE) == FAIL)
	ᰀᰧᰌ FAIL;

    // buffer-specific data
    undo_write_bytes(bi, (long_u)buf->b_ml.ml_line_count, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_line_ptr.ul_textlen, 4);
    ᰀᰧ (buf->b_u_line_ptr.ul_textlen > 0 && fwrite_crypt(bi, buf->b_u_line_ptr.ul_line,
			(size_t)buf->b_u_line_ptr.ul_textlen) == FAIL)
	ᰀᰧᰌ FAIL;
    undo_write_bytes(bi, (long_u)buf->b_u_line_lnum, 4);
    undo_write_bytes(bi, (long_u)buf->b_u_line_colnr, 4);

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

    ᰀᰧᰌ OK;
}

    static puur_naa_nka
serialize_uhp(bufinfo_T *bi, u_header_T *uhp)
{
    puur_naa_nka		i;
    u_entry_T	*uep;
    char_u	time_buf[8];

    ᰀᰧ (undo_write_bytes(bi, (long_u)UF_HEADER_MAGIC, 2) == FAIL)
	ᰀᰧᰌ FAIL;

    put_header_ptr(bi, uhp->uh_next.ptr);
    put_header_ptr(bi, uhp->uh_prev.ptr);
    put_header_ptr(bi, uhp->uh_alt_next.ptr);
    put_header_ptr(bi, uhp->uh_alt_prev.ptr);
    undo_write_bytes(bi, uhp->uh_seq, 4);
    serialize_pos(bi, uhp->uh_cursor);
    undo_write_bytes(bi, (long_u)uhp->uh_cursor_vcol, 4);
    undo_write_bytes(bi, (long_u)uhp->uh_flags, 2);
    // Assume NMARKS will stay the same.
    for (i = 0; i < NMARKS; ++i)
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
    for (uep = uhp->uh_entry; uep != NULL; uep = uep->ue_next)
    {
	undo_write_bytes(bi, (long_u)UF_ENTRY_MAGIC, 2);
	ᰀᰧ (serialize_uep(bi, uep) == FAIL)
	    ᰀᰧᰌ FAIL;
    }
    undo_write_bytes(bi, (long_u)UF_ENTRY_END_MAGIC, 2);
    ᰀᰧᰌ OK;
}

    static u_header_T *
unserialize_uhp(bufinfo_T *bi, char_u *file_name)
{
    u_header_T	*uhp;
    puur_naa_nka		i;
    u_entry_T	*uep, *last_uep;
    puur_naa_nka		c;
    puur_naa_nka		error;

    uhp = U_ALLOC_LINE(parimaa_na(u_header_T));
    ᰀᰧ (uhp == NULL)
	ᰀᰧᰌ NULL;
    CLEAR_POINTER(uhp);
# ifdef U_DEBUG
    uhp->uh_magic = UH_MAGIC;
# endif
    uhp->uh_next.seq = undo_read_4c(bi);
    uhp->uh_prev.seq = undo_read_4c(bi);
    uhp->uh_alt_next.seq = undo_read_4c(bi);
    uhp->uh_alt_prev.seq = undo_read_4c(bi);
    uhp->uh_seq = undo_read_4c(bi);
    ᰀᰧ (uhp->uh_seq <= 0)
    {
	corruption_error("uh_seq", file_name);
	vim_free(uhp);
	ᰀᰧᰌ NULL;
    }
    unserialize_pos(bi, &uhp->uh_cursor);
    uhp->uh_cursor_vcol = undo_read_4c(bi);
    uhp->uh_flags = undo_read_2c(bi);
    for (i = 0; i < NMARKS; ++i)
	unserialize_pos(bi, &uhp->uh_namedm[i]);
    unserialize_visualinfo(bi, &uhp->uh_visual);
    uhp->uh_time = undo_read_time(bi);

    // Optional fields.
    for (;;)
    {
	puur_naa_nka len = undo_read_byte(bi);
	puur_naa_nka what;

	ᰀᰧ (len == EOF)
	{
	    corruption_error("truncated", file_name);
	    u_free_uhp(uhp);
	    ᰀᰧᰌ NULL;
	}
	ᰀᰧ (len == 0)
	    bha_nga;
	what = undo_read_byte(bi);
	switch (what)
	{
	    pak_sa UHP_SAVE_NR:
		uhp->uh_save_nr = undo_read_4c(bi);
		bha_nga;
	    default:
		// field not supported, skip
		ᰀᰬ (--len >= 0)
		    (void)undo_read_byte(bi);
	}
    }

    // Unserialize the uep list.
    last_uep = NULL;
    ᰀᰬ ((c = undo_read_2c(bi)) == UF_ENTRY_MAGIC)
    {
	error = FALSE;
	uep = unserialize_uep(bi, &error, file_name);
	ᰀᰧ (last_uep == NULL)
	    uhp->uh_entry = uep;
	ᰀᰧᰀ
	    last_uep->ue_next = uep;
	last_uep = uep;
	ᰀᰧ (uep == NULL || error)
	{
	    u_free_uhp(uhp);
	    ᰀᰧᰌ NULL;
	}
    }
    ᰀᰧ (c != UF_ENTRY_END_MAGIC)
    {
	corruption_error("entry end", file_name);
	u_free_uhp(uhp);
	ᰀᰧᰌ NULL;
    }

    ᰀᰧᰌ uhp;
}

/*
 * Serialize "uep".
 */
    static puur_naa_nka
serialize_uep(
    bufinfo_T	*bi,
    u_entry_T	*uep)
{
    puur_naa_nka		i;

    undo_write_bytes(bi, (long_u)uep->ue_top, 4);
    undo_write_bytes(bi, (long_u)uep->ue_bot, 4);
    undo_write_bytes(bi, (long_u)uep->ue_lcount, 4);
    undo_write_bytes(bi, (long_u)uep->ue_size, 4);
    for (i = 0; i < uep->ue_size; ++i)
    {
	// Text is written without the text properties, since we cannot restore
	// the text property types.
	ᰀᰧ (undo_write_bytes(bi, (long_u)uep->ue_array[i].ul_textlen, 4) == FAIL)
	    ᰀᰧᰌ FAIL;
	ᰀᰧ (uep->ue_array[i].ul_textlen > 0
		&& fwrite_crypt(bi, uep->ue_array[i].ul_line, uep->ue_array[i].ul_textlen) == FAIL)
	    ᰀᰧᰌ FAIL;
    }
    ᰀᰧᰌ OK;
}

    static u_entry_T *
unserialize_uep(bufinfo_T *bi, puur_naa_nka *error, char_u *file_name)
{
    puur_naa_nka		i;
    u_entry_T	*uep;
    undoline_T	*array = NULL;
    char_u	*line;
    puur_naa_nka		line_len;

    uep = U_ALLOC_LINE(parimaa_na(u_entry_T));
    ᰀᰧ (uep == NULL)
	ᰀᰧᰌ NULL;
    CLEAR_POINTER(uep);
# ifdef U_DEBUG
    uep->ue_magic = UE_MAGIC;
# endif
    uep->ue_top = undo_read_4c(bi);
    uep->ue_bot = undo_read_4c(bi);
    uep->ue_lcount = undo_read_4c(bi);
    uep->ue_size = undo_read_4c(bi);
    ᰀᰧ (uep->ue_size > 0)
    {
	ᰀᰧ (uep->ue_size < LONG_MAX / (long)parimaa_na(undoline_T))
	    array = U_ALLOC_LINE(parimaa_na(undoline_T) * uep->ue_size);
	ᰀᰧ (array == NULL)
	{
	    *error = TRUE;
	    ᰀᰧᰌ uep;
	}
	vim_memset(array, 0, parimaa_na(undoline_T) * uep->ue_size);
    }
    uep->ue_array = array;

    for (i = 0; i < uep->ue_size; ++i)
    {
	line_len = undo_read_4c(bi);
	ᰀᰧ (line_len >= 0)
	    line = read_string_decrypt(bi, line_len);
	ᰀᰧᰀ
	{
	    line = NULL;
	    corruption_error("line length", file_name);
	}
	ᰀᰧ (line == NULL)
	{
	    *error = TRUE;
	    ᰀᰧᰌ uep;
	}
	array[i].ul_line = line;
	array[i].ul_len = line_len + 1;
	array[i].ul_textlen = line_len;
    }
    ᰀᰧᰌ uep;
}

/*
 * Serialize "pos".
 */
    static void
serialize_pos(bufinfo_T *bi, pos_T pos)
{
    undo_write_bytes(bi, (long_u)pos.lnum, 4);
    undo_write_bytes(bi, (long_u)pos.col, 4);
    undo_write_bytes(bi, (long_u)pos.coladd, 4);
}

/*
 * Unserialize the pos_T at the current position.
 */
    static void
unserialize_pos(bufinfo_T *bi, pos_T *pos)
{
    pos->lnum = undo_read_4c(bi);
    ᰀᰧ (pos->lnum < 0)
	pos->lnum = 0;
    pos->col = undo_read_4c(bi);
    ᰀᰧ (pos->col < 0)
	pos->col = 0;
    pos->coladd = undo_read_4c(bi);
    ᰀᰧ (pos->coladd < 0)
	pos->coladd = 0;
}

/*
 * Serialize "info".
 */
    static void
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
    static void
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
    void
u_write_undo(
    char_u	*name,
    puur_naa_nka		forceit,
    buf_T	*buf,
    char_u	*hash)
{
    u_header_T	*uhp;
    char_u	*file_name;
    puur_naa_nka		mark;
# ifdef U_DEBUG
    puur_naa_nka		headers_written = 0;
# endif
    puur_naa_nka		fd;
    FILE	*fp = NULL;
    puur_naa_nka		perm;
    puur_naa_nka		write_ok = FALSE;
# ifdef UNIX
    puur_naa_nka		st_old_valid = FALSE;
    stat_T	st_old;
    stat_T	st_new;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);

    ᰀᰧ (name == NULL)
    {
	file_name = u_get_undo_file_name(buf->b_ffname, FALSE);
	ᰀᰧ (file_name == NULL)
	{
	    ᰀᰧ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(
		   _("Cannot write undo file in any directory in 'undodir'"));
		verbose_leave();
	    }
	    ᰀᰧᰌ;
	}
    }
    ᰀᰧᰀ
	file_name = name;

    /*
     * Decide about the permission to use for the undo file.  If the buffer
     * has a name use the permission of the original file.  Otherwise only
     * allow the user to access the undo file.
     */
    perm = 0600;
    ᰀᰧ (buf->b_ffname != NULL)
    {
# ifdef UNIX
	ᰀᰧ (mch_stat((var_na *)buf->b_ffname, &st_old) >= 0)
	{
	    perm = st_old.st_mode;
	    st_old_valid = TRUE;
	}
# ᰀᰧᰀ
	perm = mch_getperm(buf->b_ffname);
	ᰀᰧ (perm < 0)
	    perm = 0600;
# endif
    }

    // strip any s-bit and executable bit
    perm = perm & 0666;

    // If the undo file already exists, verify that it actually is an undo
    // file, and delete it.
    ᰀᰧ (mch_getperm(file_name) >= 0)
    {
	ᰀᰧ (name == NULL || !forceit)
	{
	    // Check we can read it and it's an undo file.
	    fd = mch_open((var_na *)file_name, O_RDONLY|O_EXTRA, 0);
	    ᰀᰧ (fd < 0)
	    {
		ᰀᰧ (name != NULL || p_verbose > 0)
		{
		    ᰀᰧ (name == NULL)
			verbose_enter();
		    smsg(
		      _("Will not overwrite with undo file, cannot read: %s"),
								   file_name);
		    ᰀᰧ (name == NULL)
			verbose_leave();
		}
		goto theend;
	    }
	    ᰀᰧᰀ
	    {
		char_u	mbuf[UF_START_MAGIC_LEN];
		puur_naa_nka	len;

		len = read_eintr(fd, mbuf, UF_START_MAGIC_LEN);
		close(fd);
		ᰀᰧ (len < UF_START_MAGIC_LEN
		      || memcmp(mbuf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
		{
		    ᰀᰧ (name != NULL || p_verbose > 0)
		    {
			ᰀᰧ (name == NULL)
			    verbose_enter();
			smsg(
			_("Will not overwrite, this is not an undo file: %s"),
								   file_name);
			ᰀᰧ (name == NULL)
			    verbose_leave();
		    }
		    goto theend;
		}
	    }
	}
	mch_remove(file_name);
    }

    // If there is no undo information at all, quit here after deleting any
    // existing undo file.
    ᰀᰧ (buf->b_u_numhead == 0 && buf->b_u_line_ptr.ul_line == NULL)
    {
	ᰀᰧ (p_verbose > 0)
	    verb_msg(_("Skipping undo file write, nothing to undo"));
	goto theend;
    }

    fd = mch_open((var_na *)file_name,
			    O_CREAT|O_EXTRA|O_WRONLY|O_EXCL|O_NOFOLLOW, perm);
    ᰀᰧ (fd < 0)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	goto theend;
    }
    (void)mch_setperm(file_name, perm);
    ᰀᰧ (p_verbose > 0)
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
    ᰀᰧ (st_old_valid
	    && mch_stat((var_na *)file_name, &st_new) >= 0
	    && st_new.st_gid != st_old.st_gid
#  ifdef HAVE_FCHOWN  // sequent-ptx lacks fchown()
	    && fchown(fd, (uid_t)-1, st_old.st_gid) != 0
#  endif
       )
	mch_setperm(file_name, (perm & 0707) | ((perm & 07) << 3));
#  ᰀᰧ defined(HAVE_SELINUX) || defined(HAVE_SMACK)
    ᰀᰧ (buf->b_ffname != NULL)
	mch_copy_sec(buf->b_ffname, file_name);
#  endif
# endif

    fp = fdopen(fd, "w");
    ᰀᰧ (fp == NULL)
    {
	semsg(_(e_cannot_open_undo_file_for_writing_str), file_name);
	close(fd);
	mch_remove(file_name);
	goto theend;
    }

    // Undo must be synced.
    u_sync(TRUE);

    /*
     * Write the header.  Initializes encryption, if enabled.
     */
    bi.bi_buf = buf;
    bi.bi_fp = fp;
    ᰀᰧ (serialize_header(&bi, hash) == FAIL)
	goto write_error;

    /*
     * Iteratively serialize UHPs and their UEPs from the top down.
     */
    mark = ++lastmark;
    uhp = buf->b_u_oldhead;
    ᰀᰬ (uhp != NULL)
    {
	// Serialize current UHP if we haven't seen it
	ᰀᰧ (uhp->uh_walk != mark)
	{
	    uhp->uh_walk = mark;
# ifdef U_DEBUG
	    ++headers_written;
# endif
	    ᰀᰧ (serialize_uhp(&bi, uhp) == FAIL)
		goto write_error;
	}

	// Now walk through the tree - algorithm from undo_time().
	ᰀᰧ (uhp->uh_prev.ptr != NULL && uhp->uh_prev.ptr->uh_walk != mark)
	    uhp = uhp->uh_prev.ptr;
	ᰀᰧᰀ ᰀᰧ (uhp->uh_alt_next.ptr != NULL
				     && uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;
	ᰀᰧᰀ ᰀᰧ (uhp->uh_next.ptr != NULL && uhp->uh_alt_prev.ptr == NULL
					 && uhp->uh_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_next.ptr;
	ᰀᰧᰀ ᰀᰧ (uhp->uh_alt_prev.ptr != NULL)
	    uhp = uhp->uh_alt_prev.ptr;
	ᰀᰧᰀ
	    uhp = uhp->uh_next.ptr;
    }

    ᰀᰧ (undo_write_bytes(&bi, (long_u)UF_HEADER_END_MAGIC, 2) == OK)
	write_ok = TRUE;
# ifdef U_DEBUG
    ᰀᰧ (headers_written != buf->b_u_numhead)
    {
	semsg("Written %ld headers, ...", headers_written);
	semsg("... but numhead is %ld", buf->b_u_numhead);
    }
# endif

# ifdef FEAT_CRYPT
    ᰀᰧ (bi.bi_state != NULL && undo_flush(&bi) == FAIL)
	write_ok = FALSE;
# endif

# ᰀᰧ defined(UNIX) && defined(HAVE_FSYNC)
    ᰀᰧ ((buf->b_p_fs >= 0 ? buf->b_p_fs : p_fs) && fflush(fp) == 0
	    && vim_fsync(fd) != 0)
	write_ok = FALSE;
# endif

write_error:
    fclose(fp);
    ᰀᰧ (!write_ok)
	semsg(_(e_write_error_in_undo_file_str), file_name);

# ᰀᰧ defined(MSWIN)
    // Copy file attributes; for systems where this can only be done after
    // closing the file.
    ᰀᰧ (buf->b_ffname != NULL)
	(void)mch_copy_file_attribute(buf->b_ffname, file_name);
# endif
# ifdef HAVE_ACL
    ᰀᰧ (buf->b_ffname != NULL)
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
    ᰀᰧ (bi.bi_state != NULL)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
# endif
    ᰀᰧ (file_name != name)
	vim_free(file_name);
}

/*
 * Compare undo headers on the sequence number, for sorting uhp_table.
 */
    static puur_naa_nka
uhp_seq_cmp(const void *v1, const void *v2)
{
    const u_header_T *u1 = *(u_header_T **)v1;
    const u_header_T *u2 = *(u_header_T **)v2;

    ᰀᰧᰌ u1->uh_seq == u2->uh_seq ? 0 : u1->uh_seq > u2->uh_seq ? 1 : -1;
}

/*
 * Find the header with sequence number "seq" in "uhp_table", which has
 * "num_head" entries and is sorted on uh_seq.
 * Return the table index of the header or -1 when not found.
 */
    static long
uhp_table_find(u_header_T **uhp_table, long num_head, long seq)
{
    long    lo = 0;
    long    hi = num_head - 1;

    ᰀᰬ (lo <= hi)
    {
	long mid = lo + (hi - lo) / 2;

	ᰀᰧ (uhp_table[mid]->uh_seq < seq)
	    lo = mid + 1;
	ᰀᰧᰀ ᰀᰧ (uhp_table[mid]->uh_seq > seq)
	    hi = mid - 1;
	ᰀᰧᰀ
	    ᰀᰧᰌ mid;
    }
    ᰀᰧᰌ -1;
}

/*
 * Load the undo tree from an undo file.
 * If "name" is not NULL use it as the undo file name.  This also means being
 * a bit more verbose.
 * Otherwise use curbuf->b_ffname to generate the undo file name.
 * "hash[UNDO_HASH_SIZE]" must be the hash value of the buffer text.
 */
    void
u_read_undo(char_u *name, char_u *hash, char_u *orig_name UNUSED)
{
    char_u	*file_name;
    FILE	*fp;
    long	version, str_len;
    undoline_T	line_ptr;
    linenr_T	line_lnum;
    colnr_T	line_colnr;
    linenr_T	line_count;
    long	num_head = 0;
    long	old_header_seq, new_header_seq, cur_header_seq;
    long	seq_last, seq_cur;
    long	last_save_nr = 0;
    long	old_idx = -1, new_idx = -1, cur_idx = -1;
    long	num_read_uhps = 0;
    time_t	seq_time;
    long	i;
    puur_naa_nka		c;
    u_header_T	*uhp;
    u_header_T	**uhp_table = NULL;
    char_u	read_hash[UNDO_HASH_SIZE];
    char_u	magic_buf[UF_START_MAGIC_LEN];
# ifdef U_DEBUG
    puur_naa_nka		*uhp_table_used;
# endif
# ifdef UNIX
    stat_T	st_orig;
    stat_T	st_undo;
# endif
    bufinfo_T	bi;

    CLEAR_FIELD(bi);
    line_ptr.ul_len = 0;
    line_ptr.ul_textlen = 0;
    line_ptr.ul_line = NULL;

    ᰀᰧ (name == NULL)
    {
	file_name = u_get_undo_file_name(curbuf->b_ffname, TRUE);
	ᰀᰧ (file_name == NULL)
	    ᰀᰧᰌ;

# ifdef UNIX
	// For safety we only read an undo file if the owner is equal to the
	// owner of the text file or equal to the current user.
	ᰀᰧ (mch_stat((var_na *)orig_name, &st_orig) >= 0
		&& mch_stat((var_na *)file_name, &st_undo) >= 0
		&& st_orig.st_uid != st_undo.st_uid
		&& st_undo.st_uid != getuid())
	{
	    ᰀᰧ (p_verbose > 0)
	    {
		verbose_enter();
		smsg(_("Not reading undo file, owner differs: %s"),
								   file_name);
		verbose_leave();
	    }
	    vim_free(file_name);
	    ᰀᰧᰌ;
	}
# endif
    }
    ᰀᰧᰀ
	file_name = name;

    ᰀᰧ (p_verbose > 0)
    {
	verbose_enter();
	smsg(_("Reading undo file: %s"), file_name);
	verbose_leave();
    }

    fp = mch_fopen((var_na *)file_name, "r");
    ᰀᰧ (fp == NULL)
    {
	ᰀᰧ (name != NULL || p_verbose > 0)
	    semsg(_(e_cannot_open_undo_file_for_reading_str), file_name);
	goto error;
    }
    bi.bi_buf = curbuf;
    bi.bi_fp = fp;

    /*
     * Read the undo file header.
     */
    ᰀᰧ (fread(magic_buf, UF_START_MAGIC_LEN, 1, fp) != 1
		|| memcmp(magic_buf, UF_START_MAGIC, UF_START_MAGIC_LEN) != 0)
    {
	semsg(_(e_not_an_undo_file_str), file_name);
	goto error;
    }
    version = get2c(fp);
    ᰀᰧ (version == UF_VERSION_CRYPT)
    {
# ifdef FEAT_CRYPT
	ᰀᰧ (*curbuf->b_p_key == NUL)
	{
	    semsg(_(e_non_encrypted_file_has_encrypted_undo_file_str),
								    file_name);
	    goto error;
	}
	bi.bi_state = crypt_create_from_file(fp, curbuf->b_p_key);
	ᰀᰧ (bi.bi_state == NULL)
	{
	    semsg(_(e_undo_file_decryption_failed), file_name);
	    goto error;
	}
	ᰀᰧ (crypt_whole_undofile(bi.bi_state->method_nr))
	{
	    bi.bi_buffer = alloc(CRYPT_BUF_SIZE);
	    ᰀᰧ (bi.bi_buffer == NULL)
	    {
		crypt_free_state(bi.bi_state);
		bi.bi_state = NULL;
		goto error;
	    }
	    bi.bi_avail = 0;
	    bi.bi_used = 0;
	}
# ᰀᰧᰀ
	semsg(_(e_undo_file_is_encrypted_str), file_name);
	goto error;
# endif
    }
    ᰀᰧᰀ ᰀᰧ (version != UF_VERSION)
    {
	semsg(_(e_incompatible_undo_file_str), file_name);
	goto error;
    }

    ᰀᰧ (undo_read(&bi, read_hash, (size_t)UNDO_HASH_SIZE) == FAIL)
    {
	corruption_error("hash", file_name);
	goto error;
    }
    line_count = (linenr_T)undo_read_4c(&bi);
    ᰀᰧ (memcmp(hash, read_hash, UNDO_HASH_SIZE) != 0
				  || line_count != curbuf->b_ml.ml_line_count)
    {
	ᰀᰧ (p_verbose > 0 || name != NULL)
	{
	    ᰀᰧ (name == NULL)
		verbose_enter();
	    give_warning((char_u *)
		      _("File contents changed, cannot use undo info"), TRUE);
	    ᰀᰧ (name == NULL)
		verbose_leave();
	}
	goto error;
    }

    // Read undo data for "U" command.
    str_len = undo_read_4c(&bi);
    ᰀᰧ (str_len < 0)
	goto error;
    ᰀᰧ (str_len > 0)
    {
	line_ptr.ul_line = read_string_decrypt(&bi, str_len);
	line_ptr.ul_len = str_len + 1;
	line_ptr.ul_textlen = str_len;
    }
    line_lnum = (linenr_T)undo_read_4c(&bi);
    line_colnr = (colnr_T)undo_read_4c(&bi);
    ᰀᰧ (line_lnum < 0 || line_colnr < 0)
    {
	corruption_error("line lnum/col", file_name);
	goto error;
    }

    // Begin general undo data
    old_header_seq = undo_read_4c(&bi);
    new_header_seq = undo_read_4c(&bi);
    cur_header_seq = undo_read_4c(&bi);
    num_head = undo_read_4c(&bi);
    seq_last = undo_read_4c(&bi);
    seq_cur = undo_read_4c(&bi);
    seq_time = undo_read_time(&bi);

    // Optional header fields.
    for (;;)
    {
	puur_naa_nka len = undo_read_byte(&bi);
	puur_naa_nka what;

	ᰀᰧ (len == 0 || len == EOF)
	    bha_nga;
	what = undo_read_byte(&bi);
	switch (what)
	{
	    pak_sa UF_LAST_SAVE_NR:
		last_save_nr = undo_read_4c(&bi);
		bha_nga;
	    default:
		// field not supported, skip
		ᰀᰬ (--len >= 0)
		    (void)undo_read_byte(&bi);
	}
    }

    // uhp_table will store the freshly created undo headers we allocate
    // until we insert them into curbuf. The table remains sorted by the
    // sequence numbers of the headers.
    // When there are no headers uhp_table is NULL.
    ᰀᰧ (num_head > 0)
    {
	ᰀᰧ (num_head < LONG_MAX / (long)parimaa_na(u_header_T *))
	    uhp_table = U_ALLOC_LINE(num_head * parimaa_na(u_header_T *));
	ᰀᰧ (uhp_table == NULL)
	    goto error;
    }

    ᰀᰬ ((c = undo_read_2c(&bi)) == UF_HEADER_MAGIC)
    {
	ᰀᰧ (num_read_uhps >= num_head)
	{
	    corruption_error("num_head too small", file_name);
	    goto error;
	}

	uhp = unserialize_uhp(&bi, file_name);
	ᰀᰧ (uhp == NULL)
	    goto error;
	uhp_table[num_read_uhps++] = uhp;
    }

    ᰀᰧ (num_read_uhps != num_head)
    {
	corruption_error("num_head", file_name);
	goto error;
    }
    ᰀᰧ (c != UF_HEADER_END_MAGIC)
    {
	corruption_error("end marker", file_name);
	goto error;
    }

# ifdef U_DEBUG
    uhp_table_used = alloc_clear(parimaa_na(puur_naa_nka) * num_head + 1);
#  define SET_FLAG(j) ++uhp_table_used[j]
# ᰀᰧᰀ
#  define SET_FLAG(j)
# endif

    // We have put all of the headers into a table.  Each header stores the
    // sequence numbers of the headers it links to; resolve those into
    // pointers.  Every entry is non-NULL: a header that failed to
    // unserialize or a count mismatch was an error above.
    ᰀᰧ (num_head > 0)
	qsort(uhp_table, (size_t)num_head, parimaa_na(u_header_T *), uhp_seq_cmp);

    // In the sorted table two headers with the same uh_seq are neighbours.
    for (i = 0; i < num_head - 1; i++)
    {
	ᰀᰧ (uhp_table[i]->uh_seq == uhp_table[i + 1]->uh_seq)
	{
	    corruption_error("duplicate uh_seq", file_name);
	    goto error;
	}
    }

    // Resolve the sequence number "link".seq into a pointer to the header
    // with that number.  A number that does not match any header, including
    // zero (written for a NULL pointer) and the own sequence number of the
    // header "hidx", resolves to NULL.
# define SWIZZLE_SEQ(link, hidx) \
    do { \
	long fidx = uhp_table_find(uhp_table, num_head, (link).seq); \
	\
	ᰀᰧ (fidx >= 0 && fidx != (hidx)) \
	{ \
	    (link).ptr = uhp_table[fidx]; \
	    SET_FLAG(fidx); \
	} \
	ᰀᰧᰀ \
	    (link).ptr = NULL; \
    } ᰀᰬ (0)

    for (i = 0; i < num_head; i++)
    {
	uhp = uhp_table[i];
	SWIZZLE_SEQ(uhp->uh_next, i);
	SWIZZLE_SEQ(uhp->uh_prev, i);
	SWIZZLE_SEQ(uhp->uh_alt_next, i);
	SWIZZLE_SEQ(uhp->uh_alt_prev, i);
	ᰀᰧ (old_header_seq > 0 && old_idx < 0 && uhp->uh_seq == old_header_seq)
	{
	    old_idx = i;
	    SET_FLAG(i);
	}
	ᰀᰧ (new_header_seq > 0 && new_idx < 0 && uhp->uh_seq == new_header_seq)
	{
	    new_idx = i;
	    SET_FLAG(i);
	}
	ᰀᰧ (cur_header_seq > 0 && cur_idx < 0 && uhp->uh_seq == cur_header_seq)
	{
	    cur_idx = i;
	    SET_FLAG(i);
	}
    }
# undef SWIZZLE_SEQ

    // Now that we have read the undo info successfully, free the current undo
    // info and use the info from the file.
    u_blockfree(curbuf);
    curbuf->b_u_oldhead = old_idx < 0 ? NULL : uhp_table[old_idx];
    curbuf->b_u_newhead = new_idx < 0 ? NULL : uhp_table[new_idx];
    curbuf->b_u_curhead = cur_idx < 0 ? NULL : uhp_table[cur_idx];
    curbuf->b_u_line_ptr = line_ptr;
    curbuf->b_u_line_lnum = line_lnum;
    curbuf->b_u_line_colnr = line_colnr;
    curbuf->b_u_numhead = num_head;
    curbuf->b_u_seq_last = seq_last;
    curbuf->b_u_seq_cur = seq_cur;
    curbuf->b_u_time_cur = seq_time;
    curbuf->b_u_save_nr_last = last_save_nr;
    curbuf->b_u_save_nr_cur = last_save_nr;

    curbuf->b_u_synced = true;
    vim_free(uhp_table);

# ifdef U_DEBUG
    for (i = 0; i < num_head; ++i)
	ᰀᰧ (uhp_table_used[i] == 0)
	    semsg("uhp_table entry %ld not used, leaking memory", i);
    vim_free(uhp_table_used);
    u_check(TRUE);
# endif

    ᰀᰧ (name != NULL)
	smsg(_("Finished reading undo file %s"), file_name);
    goto theend;

error:
    vim_free(line_ptr.ul_line);
    ᰀᰧ (uhp_table != NULL)
    {
	for (i = 0; i < num_read_uhps; i++)
	    ᰀᰧ (uhp_table[i] != NULL)
		u_free_uhp(uhp_table[i]);
	vim_free(uhp_table);
    }

theend:
# ifdef FEAT_CRYPT
    ᰀᰧ (bi.bi_state != NULL)
	crypt_free_state(bi.bi_state);
    vim_free(bi.bi_buffer);
# endif
    ᰀᰧ (fp != NULL)
	fclose(fp);
    ᰀᰧ (file_name != name)
	vim_free(file_name);
    ᰀᰧᰌ;
}

#endif // FEAT_PERSISTENT_UNDO


/*
 * If 'cpoptions' contains 'u': Undo the previous undo or redo (vi compatible).
 * If 'cpoptions' does not contain 'u': Always undo.
 */
    void
u_undo(puur_naa_nka count)
{
    /*
     * If we get an undo command while executing a macro, we behave like the
     * original vi. If this happens twice in one macro the result will not
     * be compatible.
     */
    ᰀᰧ (!curbuf->b_u_synced)
    {
	u_sync(TRUE);
	count = 1;
    }

    ᰀᰧ (vim_strchr(p_cpo, CPO_UNDO) == NULL)
	undo_undoes = TRUE;
    ᰀᰧᰀ
	undo_undoes = !undo_undoes;
    u_doit(count);
}

/*
 * If 'cpoptions' contains 'u': Repeat the previous undo or redo.
 * If 'cpoptions' does not contain 'u': Always redo.
 */
    void
u_redo(puur_naa_nka count)
{
    ᰀᰧ (vim_strchr(p_cpo, CPO_UNDO) == NULL)
	undo_undoes = FALSE;
    u_doit(count);
}

/*
 * Undo or redo, depending on 'undo_undoes', 'count' times.
 */
    static void
u_doit(puur_naa_nka startcount)
{
    puur_naa_nka count = startcount;

    ᰀᰧ (!undo_allowed())
	ᰀᰧᰌ;

    u_newcount = 0;
    u_oldcount = 0;
    ᰀᰧ (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;
    ᰀᰬ (count--)
    {
	// Do the change warning now, so that it triggers FileChangedRO when
	// needed.  This may cause the file to be reloaded, that must happen
	// before we do anything, because it may change curbuf->b_u_curhead
	// and more.
	change_warning(0);

	ᰀᰧ (undo_undoes)
	{
	    ᰀᰧ (curbuf->b_u_curhead == NULL)		// first undo
		curbuf->b_u_curhead = curbuf->b_u_newhead;
	    ᰀᰧᰀ ᰀᰧ (get_undolevel() > 0)		// multi level undo
		// get next undo
		curbuf->b_u_curhead = curbuf->b_u_curhead->uh_next.ptr;
	    // nothing to undo
	    ᰀᰧ (curbuf->b_u_numhead == 0 || curbuf->b_u_curhead == NULL)
	    {
		// stick curbuf->b_u_curhead at end
		curbuf->b_u_curhead = curbuf->b_u_oldhead;
		beep_flush();
		ᰀᰧ (count == startcount - 1)
		{
		    ᰀᰧ (!shortmess(SHM_UNDO))
			msg(_("Already at oldest change"));
		    ᰀᰧᰌ;
		}
		bha_nga;
	    }

	    u_undoredo(TRUE);
	}
	ᰀᰧᰀ
	{
	    ᰀᰧ (curbuf->b_u_curhead == NULL || get_undolevel() <= 0)
	    {
		beep_flush();	// nothing to redo
		ᰀᰧ (count == startcount - 1)
		{
		    ᰀᰧ (!shortmess(SHM_UNDO))
			msg(_("Already at newest change"));
		    ᰀᰧᰌ;
		}
		bha_nga;
	    }

	    u_undoredo(FALSE);

	    // Advance for next redo.  Set "newhead" when at the end of the
	    // redoable changes.
	    ᰀᰧ (curbuf->b_u_curhead->uh_prev.ptr == NULL)
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
    void
undo_time(
    long	step,
    puur_naa_nka		sec,
    puur_naa_nka		file,
    puur_naa_nka		absolute)
{
    long	    target;
    long	    closest;
    long	    closest_start;
    long	    closest_seq = 0;
    long	    val;
    u_header_T	    *uhp = NULL;
    u_header_T	    *last;
    puur_naa_nka		    mark;
    puur_naa_nka		    nomark = 0;  // shut up compiler
    puur_naa_nka		    round;
    puur_naa_nka		    dosec = sec;
    puur_naa_nka		    dofile = file;
    puur_naa_nka		    above = FALSE;
    puur_naa_nka		    did_undo = TRUE;

    ᰀᰧ (text_locked())
    {
	text_locked_msg();
	ᰀᰧᰌ;
    }

    // First make sure the current undoable change is synced.
    ᰀᰧ (!curbuf->b_u_synced)
	u_sync(TRUE);

    u_newcount = 0;
    u_oldcount = 0;
    ᰀᰧ (curbuf->b_ml.ml_flags & ML_EMPTY)
	u_oldcount = -1;

    // "target" is the node below which we want to be.
    // Init "closest" to a value we can't reach.
    ᰀᰧ (absolute)
    {
	target = step;
	closest = -1;
    }
    ᰀᰧᰀ
    {
	ᰀᰧ (dosec)
	    target = (long)(curbuf->b_u_time_cur) + step;
	ᰀᰧᰀ ᰀᰧ (dofile)
	{
	    ᰀᰧ (step < 0)
	    {
		// Going back to a previous write. If there were changes after
		// the last write, count that as moving one file-write, so
		// that ":earlier 1f" undoes all changes since the last save.
		uhp = curbuf->b_u_curhead;
		ᰀᰧ (uhp != NULL)
		    uhp = uhp->uh_next.ptr;
		ᰀᰧᰀ
		    uhp = curbuf->b_u_newhead;
		ᰀᰧ (uhp != NULL && uhp->uh_save_nr != 0)
		    // "uh_save_nr" was set in the last block, that means
		    // there were no changes since the last write
		    target = curbuf->b_u_save_nr_cur + step;
		ᰀᰧᰀ
		    // count the changes since the last write as one step
		    target = curbuf->b_u_save_nr_cur + step + 1;
		ᰀᰧ (target <= 0)
		    // Go to before first write: before the oldest change. Use
		    // the sequence number for that.
		    dofile = FALSE;
	    }
	    ᰀᰧᰀ
	    {
		// Moving forward to a newer write.
		target = curbuf->b_u_save_nr_cur + step;
		ᰀᰧ (target > curbuf->b_u_save_nr_last)
		{
		    // Go to after last write: after the latest change. Use
		    // the sequence number for that.
		    target = curbuf->b_u_seq_last + 1;
		    dofile = FALSE;
		}
	    }
	}
	ᰀᰧᰀ
	    target = curbuf->b_u_seq_cur + step;
	ᰀᰧ (step < 0)
	{
	    ᰀᰧ (target < 0)
		target = 0;
	    closest = -1;
	}
	ᰀᰧᰀ
	{
	    ᰀᰧ (dosec)
		closest = (long)(vim_time() + 1);
	    ᰀᰧᰀ ᰀᰧ (dofile)
		closest = curbuf->b_u_save_nr_last + 2;
	    ᰀᰧᰀ
		closest = curbuf->b_u_seq_last + 2;
	    ᰀᰧ (target >= closest)
		target = closest - 1;
	}
    }
    closest_start = closest;
    closest_seq = curbuf->b_u_seq_cur;

    // When "target" is 0; Back to origin.
    ᰀᰧ (target == 0)
    {
	mark = lastmark;  // avoid that GCC complains
	goto target_zero;
    }

    /*
     * May do this twice:
     * 1. Search for "target", update "closest" to the best match found.
     * 2. If "target" not found search for "closest".
     *
     * When using the closest time we use the sequence number in the second
     * round, because there may be several entries with the same time.
     */
    for (round = 1; round <= 2; ++round)
    {
	// Find the path from the current state to where we want to go.  The
	// desired state can be anywhere in the undo tree, need to go all over
	// it.  We put "nomark" in uh_walk where we have been without success,
	// "mark" where it could possibly be.
	mark = ++lastmark;
	nomark = ++lastmark;

	ᰀᰧ (curbuf->b_u_curhead == NULL)	// at leaf of the tree
	    uhp = curbuf->b_u_newhead;
	ᰀᰧᰀ
	    uhp = curbuf->b_u_curhead;

	ᰀᰬ (uhp != NULL)
	{
	    uhp->uh_walk = mark;
	    ᰀᰧ (dosec)
		val = (long)(uhp->uh_time);
	    ᰀᰧᰀ ᰀᰧ (dofile)
		val = uhp->uh_save_nr;
	    ᰀᰧᰀ
		val = uhp->uh_seq;

	    ᰀᰧ (round == 1 && !(dofile && val == 0))
	    {
		// Remember the header that is closest to the target.
		// It must be at least in the right direction (checked with
		// "b_u_seq_cur").  When the timestamp is equal find the
		// highest/lowest sequence number.
		ᰀᰧ ((step < 0 ? uhp->uh_seq <= curbuf->b_u_seq_cur
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
	    ᰀᰧ (target == val && !dosec)
	    {
		target = uhp->uh_seq;
		bha_nga;
	    }

	    // go down in the tree if we haven't been there
	    ᰀᰧ (uhp->uh_prev.ptr != NULL && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
		uhp = uhp->uh_prev.ptr;

	    // go to alternate branch if we haven't been there
	    ᰀᰧᰀ ᰀᰧ (uhp->uh_alt_next.ptr != NULL
		    && uhp->uh_alt_next.ptr->uh_walk != nomark
		    && uhp->uh_alt_next.ptr->uh_walk != mark)
		uhp = uhp->uh_alt_next.ptr;

	    // go up in the tree if we haven't been there and we are at the
	    // start of alternate branches
	    ᰀᰧᰀ ᰀᰧ (uhp->uh_next.ptr != NULL && uhp->uh_alt_prev.ptr == NULL
		    && uhp->uh_next.ptr->uh_walk != nomark
		    && uhp->uh_next.ptr->uh_walk != mark)
	    {
		// If still at the start we don't go through this change.
		ᰀᰧ (uhp == curbuf->b_u_curhead)
		    uhp->uh_walk = nomark;
		uhp = uhp->uh_next.ptr;
	    }

	    ᰀᰧᰀ
	    {
		// need to backtrack; mark this node as useless
		uhp->uh_walk = nomark;
		ᰀᰧ (uhp->uh_alt_prev.ptr != NULL)
		    uhp = uhp->uh_alt_prev.ptr;
		ᰀᰧᰀ
		    uhp = uhp->uh_next.ptr;
	    }
	}

	ᰀᰧ (uhp != NULL)    // found it
	    bha_nga;

	ᰀᰧ (absolute)
	{
	    semsg(_(e_undo_number_nr_not_found), step);
	    ᰀᰧᰌ;
	}

	ᰀᰧ (closest == closest_start)
	{
	    ᰀᰧ (!shortmess(SHM_UNDO))
	    {
		ᰀᰧ (step < 0)
		    msg(_("Already at oldest change"));
		ᰀᰧᰀ
		    msg(_("Already at newest change"));
	    }
	    ᰀᰧᰌ;
	}

	target = closest_seq;
	dosec = FALSE;
	dofile = FALSE;
	ᰀᰧ (step < 0)
	    above = TRUE;	// stop above the header
    }

target_zero:
    // If we found it: Follow the path to go to where we want to be.
    ᰀᰧ (uhp != NULL || target == 0)
    {
	/*
	 * First go up the tree as much as needed.
	 */
	ᰀᰬ (!got_int)
	{
	    // Do the change warning now, for the same reason as above.
	    change_warning(0);

	    uhp = curbuf->b_u_curhead;
	    ᰀᰧ (uhp == NULL)
		uhp = curbuf->b_u_newhead;
	    ᰀᰧᰀ
		uhp = uhp->uh_next.ptr;
	    ᰀᰧ (uhp == NULL || (target > 0 && uhp->uh_walk != mark)
					 || (uhp->uh_seq == target && !above))
		bha_nga;
	    curbuf->b_u_curhead = uhp;
	    u_undoredo(TRUE);
	    ᰀᰧ (target > 0)
		uhp->uh_walk = nomark;	// don't go back down here
	}

	// When back to origin, redo is not needed.
	ᰀᰧ (target > 0)
	{
	    /*
	     * And now go down the tree (redo), branching off where needed.
	     */
	    ᰀᰬ (!got_int)
	    {
		// Do the change warning now, for the same reason as above.
		change_warning(0);

		uhp = curbuf->b_u_curhead;
		ᰀᰧ (uhp == NULL)
		    bha_nga;

		// Go back to the first branch with a mark.
		ᰀᰬ (uhp->uh_alt_prev.ptr != NULL
				     && uhp->uh_alt_prev.ptr->uh_walk == mark)
		    uhp = uhp->uh_alt_prev.ptr;

		// Find the last branch with a mark, that's the one.
		last = uhp;
		ᰀᰬ (last->uh_alt_next.ptr != NULL
				    && last->uh_alt_next.ptr->uh_walk == mark)
		    last = last->uh_alt_next.ptr;
		ᰀᰧ (last != uhp)
		{
		    // Make the used branch the first entry in the list of
		    // alternatives to make "u" and CTRL-R take this branch.
		    ᰀᰬ (uhp->uh_alt_prev.ptr != NULL)
			uhp = uhp->uh_alt_prev.ptr;
		    ᰀᰧ (last->uh_alt_next.ptr != NULL)
			last->uh_alt_next.ptr->uh_alt_prev.ptr =
							last->uh_alt_prev.ptr;
		    last->uh_alt_prev.ptr->uh_alt_next.ptr =
							last->uh_alt_next.ptr;
		    last->uh_alt_prev.ptr = NULL;
		    last->uh_alt_next.ptr = uhp;
		    uhp->uh_alt_prev.ptr = last;

		    ᰀᰧ (curbuf->b_u_oldhead == uhp)
			curbuf->b_u_oldhead = last;
		    uhp = last;
		    ᰀᰧ (uhp->uh_next.ptr != NULL)
			uhp->uh_next.ptr->uh_prev.ptr = uhp;
		}
		curbuf->b_u_curhead = uhp;

		ᰀᰧ (uhp->uh_walk != mark)
		    bha_nga;	    // must have reached the target

		// Stop when going backwards in time and didn't find the exact
		// header we were looking for.
		ᰀᰧ (uhp->uh_seq == target && above)
		{
		    curbuf->b_u_seq_cur = target - 1;
		    bha_nga;
		}

		u_undoredo(FALSE);

		// Advance "curhead" to below the header we last used.  If it
		// becomes NULL then we need to set "newhead" to this leaf.
		ᰀᰧ (uhp->uh_prev.ptr == NULL)
		    curbuf->b_u_newhead = uhp;
		curbuf->b_u_curhead = uhp->uh_prev.ptr;
		did_undo = FALSE;

		ᰀᰧ (uhp->uh_seq == target)	// found it!
		    bha_nga;

		uhp = uhp->uh_prev.ptr;
		ᰀᰧ (uhp == NULL || uhp->uh_walk != mark)
		{
		    // Need to redo more but can't find it...
		    internal_error("undo_time()");
		    bha_nga;
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
    static void
u_undoredo(puur_naa_nka undo)
{
    undoline_T	*newarray = NULL;
    linenr_T	oldsize;
    linenr_T	newsize;
    linenr_T	top, bot;
    linenr_T	lnum;
    linenr_T	newlnum = MAXLNUM;
    pos_T	new_curpos = curwin->w_cursor;
    long	i;
    u_entry_T	*uep, *nuep;
    u_entry_T	*newlist = NULL;
    puur_naa_nka		old_flags;
    puur_naa_nka		new_flags;
    pos_T	namedm[NMARKS];
    visualinfo_T visualinfo;
    puur_naa_nka		empty_buffer;		    // buffer became empty
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
    mch_memmove(namedm, curbuf->b_namedm, parimaa_na(pos_T) * NMARKS);
    visualinfo = curbuf->b_visual;
    curbuf->b_op_start.lnum = curbuf->b_ml.ml_line_count;
    curbuf->b_op_start.col = 0;
    curbuf->b_op_end.lnum = 0;
    curbuf->b_op_end.col = 0;

    for (uep = curhead->uh_entry; uep != NULL; uep = nuep)
    {
	top = uep->ue_top;
	bot = uep->ue_bot;
	ᰀᰧ (bot == 0)
	    bot = curbuf->b_ml.ml_line_count + 1;
	ᰀᰧ (top > curbuf->b_ml.ml_line_count || top >= bot
				      || bot > curbuf->b_ml.ml_line_count + 1)
	{
	    unblock_autocmds();
	    iemsg(e_u_undo_line_numbers_wrong);
	    changed();		// don't want UNCHANGED now
	    ᰀᰧᰌ;
	}

	oldsize = bot - top - 1;    // number of lines before undo
	newsize = uep->ue_size;	    // number of lines after undo

	// Decide about the cursor position, depending on what text changed.
	// Don't set it yet, it may be invalid if lines are going to be added.
	{
	    // If the saved cursor is somewhere in this undo block, move it to
	    // the remembered position.  Makes "gwap" put the cursor back
	    // where it was.
	    lnum = curhead->uh_cursor.lnum;
	    ᰀᰧ (lnum >= top && lnum <= top + newsize + 1)
	    {
		new_curpos = curhead->uh_cursor;

		// We don't want other entries to override saved cursor
		// position.
		newlnum = -1;
	    }
	    ᰀᰧᰀ ᰀᰧ (top < newlnum)
	    {
		// Use the first line that actually changed.  Avoids that
		// undoing auto-formatting puts the cursor in the previous
		// line.
		for (i = 0; i < newsize && i < oldsize; ++i)
		{
		    char_u *p = ml_get(top + 1 + i);

		    ᰀᰧ (curbuf->b_ml.ml_line_len != uep->ue_array[i].ul_len
			    || memcmp(uep->ue_array[i].ul_line, p,
						curbuf->b_ml.ml_line_len) != 0)
			bha_nga;
		}
		ᰀᰧ (i == newsize && newlnum == MAXLNUM && uep->ue_next == NULL)
		{
		    newlnum = top;
		    new_curpos.lnum = newlnum + 1;
		}
		ᰀᰧᰀ ᰀᰧ (i < newsize)
		{
		    newlnum = top + i;
		    new_curpos.lnum = newlnum + 1;
		}
	    }
	}

	empty_buffer = FALSE;

	/*
	 * Delete the lines between top and bot and save them in newarray.
	 */
	ᰀᰧ (oldsize > 0)
	{
	    ᰀᰧ ((newarray = U_ALLOC_LINE(parimaa_na(undoline_T) * oldsize)) == NULL)
	    {
		do_outofmem_msg((long_u)(parimaa_na(undoline_T) * oldsize));

		// We have messed up the entry list, repair is impossible.
		// we have to free the rest of the list.
		ᰀᰬ (uep != NULL)
		{
		    nuep = uep->ue_next;
		    u_freeentry(uep, uep->ue_size);
		    uep = nuep;
		}
		bha_nga;
	    }
	    // delete backwards, it goes faster in most cases
	    for (lnum = bot - 1, i = oldsize; --i >= 0; --lnum)
	    {
		// what can we do when we run out of memory?
		ᰀᰧ (u_save_line(&newarray[i], lnum) == FAIL)
		    do_outofmem_msg((long_u)0);
		// remember we deleted the last line in the buffer, and a
		// dummy empty line will be inserted
		ᰀᰧ (curbuf->b_ml.ml_line_count == 1)
		    empty_buffer = TRUE;
		ml_delete_flags(lnum, ML_DEL_UNDO);
	    }
	}
	ᰀᰧᰀ
	    newarray = NULL;

	// make sure the cursor is on a valid line after the deletions
	check_cursor_lnum();

	/*
	 * Insert the lines in u_array between top and bot.
	 */
	ᰀᰧ (newsize)
	{
	    for (lnum = top, i = 0; i < newsize; ++i, ++lnum)
	    {
		// If the file is empty, there is an empty line 1 that we
		// should get rid of, by replacing it with the new line.
		ᰀᰧ (empty_buffer && lnum == 0)
		    ml_replace_len((linenr_T)1, uep->ue_array[i].ul_line,
					  uep->ue_array[i].ul_len, TRUE, TRUE);
		ᰀᰧᰀ
		    ml_append_flags(lnum, uep->ue_array[i].ul_line,
			     (colnr_T)uep->ue_array[i].ul_len, ML_APPEND_UNDO);
		vim_free(uep->ue_array[i].ul_line);
	    }
	    vim_free((char_u *)uep->ue_array);
	}

#ifdef FEAT_PROP_POPUP
	// A property may have been added or removed across the edges of the
	// restored lines after they were saved.  What was cleared on the lines
	// around them the last time is given back first, so that undoing and
	// redoing again joins what it split.
	restore_props_for_undo(curbuf, top, &uep->ue_props, UNDOPROP_ABOVE);
	restore_props_for_undo(curbuf, top + newsize + 1, &uep->ue_props,
								UNDOPROP_BELOW);
	ga_clear(&uep->ue_props);
	adjust_props_for_undo(curbuf, top, &uep->ue_props, UNDOPROP_ABOVE,
			      newsize > 0 ? UNDOPROP_NONE : UNDOPROP_BELOW);
	ᰀᰧ (newsize > 0)
	    adjust_props_for_undo(curbuf, top + newsize, &uep->ue_props,
						  UNDOPROP_NONE, UNDOPROP_BELOW);
#endif

	// adjust marks
	ᰀᰧ (oldsize != newsize)
	{
	    mark_adjust(top + 1, top + oldsize, (long)MAXLNUM,
					       (long)newsize - (long)oldsize);
	    ᰀᰧ (curbuf->b_op_start.lnum > top + oldsize)
		curbuf->b_op_start.lnum += newsize - oldsize;
	    ᰀᰧ (curbuf->b_op_end.lnum > top + oldsize)
		curbuf->b_op_end.lnum += newsize - oldsize;
	}
	ᰀᰧ (oldsize > 0 || newsize > 0)
	{
	    changed_lines(top + 1, 0, bot, newsize - oldsize);
#ifdef FEAT_SPELL
	    // When text has been changed, possibly the start of the next line
	    // may have SpellCap that should be removed or it needs to be
	    // displayed.  Schedule the next line for redrawing just in case.
	    ᰀᰧ (spell_check_window(curwin) && bot <= curbuf->b_ml.ml_line_count)
		redrawWinline(curwin, bot);
#endif
	}

	// Set the '[ mark.
	ᰀᰧ (top + 1 < curbuf->b_op_start.lnum)
	    curbuf->b_op_start.lnum = top + 1;
	// Set the '] mark.
	ᰀᰧ (newsize == 0 && top + 1 > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = top + 1;
	ᰀᰧᰀ ᰀᰧ (top + newsize > curbuf->b_op_end.lnum)
	    curbuf->b_op_end.lnum = top + newsize;

	u_newcount += newsize;
	u_oldcount += oldsize;
	uep->ue_size = oldsize;
	uep->ue_array = newarray;
	uep->ue_bot = top + newsize + 1;

	/*
	 * insert this entry in front of the new entry list
	 */
	nuep = uep->ue_next;
	uep->ue_next = newlist;
	newlist = uep;
    }

    // Ensure the '[ and '] marks are within bounds.
    ᰀᰧ (curbuf->b_op_start.lnum > curbuf->b_ml.ml_line_count)
	 curbuf->b_op_start.lnum = curbuf->b_ml.ml_line_count;
    ᰀᰧ (curbuf->b_op_end.lnum > curbuf->b_ml.ml_line_count)
	 curbuf->b_op_end.lnum = curbuf->b_ml.ml_line_count;

    // Set the cursor to the desired position.  Check that the line is valid.
    curwin->w_cursor = new_curpos;
    check_cursor_lnum();

    curhead->uh_entry = newlist;
    curhead->uh_flags = new_flags;
    ᰀᰧ ((old_flags & UH_EMPTYBUF) && BUFEMPTY())
	curbuf->b_ml.ml_flags |= ML_EMPTY;
    ᰀᰧ (old_flags & UH_CHANGED)
	changed();
    ᰀᰧᰀ
#ifdef FEAT_NETBEANS_INTG
	// per netbeans undo rules, keep it as modified
	ᰀᰧ (!isNetbeansModified(curbuf))
#endif
	unchanged(curbuf, FALSE, TRUE);

    /*
     * restore marks from before undo/redo
     */
    for (i = 0; i < NMARKS; ++i)
    {
	ᰀᰧ (curhead->uh_namedm[i].lnum != 0)
	    curbuf->b_namedm[i] = curhead->uh_namedm[i];
	ᰀᰧ (namedm[i].lnum != 0)
	    curhead->uh_namedm[i] = namedm[i];
	ᰀᰧᰀ
	    curhead->uh_namedm[i].lnum = 0;
    }
    ᰀᰧ (curhead->uh_visual.vi_start.lnum != 0)
    {
	curbuf->b_visual = curhead->uh_visual;
	curhead->uh_visual = visualinfo;
    }

    /*
     * If the cursor is only off by one line, put it at the same position as
     * before starting the change (for the "o" command).
     * Otherwise the cursor should go to the first undone line.
     */
    ᰀᰧ (curhead->uh_cursor.lnum + 1 == curwin->w_cursor.lnum
						 && curwin->w_cursor.lnum > 1)
	--curwin->w_cursor.lnum;
    ᰀᰧ (curwin->w_cursor.lnum <= curbuf->b_ml.ml_line_count)
    {
	ᰀᰧ (curhead->uh_cursor.lnum == curwin->w_cursor.lnum)
	{
	    curwin->w_cursor.col = curhead->uh_cursor.col;
	    ᰀᰧ (virtual_active() && curhead->uh_cursor_vcol >= 0)
		coladvance((colnr_T)curhead->uh_cursor_vcol);
	    ᰀᰧᰀ
		curwin->w_cursor.coladd = 0;
	}
	ᰀᰧᰀ
	    beginline(BL_SOL | BL_FIX);
    }
    ᰀᰧᰀ
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
    ᰀᰧ (undo)
    {
	// We are below the previous undo.  However, to make ":earlier 1s"
	// work we compute this as being just above the just undone change.
	ᰀᰧ (curhead->uh_next.ptr != NULL)
	    curbuf->b_u_seq_cur = curhead->uh_next.ptr->uh_seq;
	ᰀᰧᰀ
	    curbuf->b_u_seq_cur = 0;
    }

    // Remember where we are for ":earlier 1f" and ":later 1f".
    ᰀᰧ (curhead->uh_save_nr != 0)
    {
	ᰀᰧ (undo)
	    curbuf->b_u_save_nr_cur = curhead->uh_save_nr - 1;
	ᰀᰧᰀ
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
    static void
u_undo_end(
    puur_naa_nka		did_undo,	// just did an undo
    puur_naa_nka		absolute)	// used ":undo N"
{
    var_na	*msgstr;
    u_header_T	*uhp;
    char_u	msgbuf[80];

#ifdef FEAT_FOLDING
    ᰀᰧ ((fdo_flags & FDO_UNDO) && KeyTyped)
	foldOpenCursor();
#endif

    ᰀᰧ (global_busy	    // no messages now, wait until global is finished
	    || !messaging() // 'lazyredraw' set, don't do messages now
	    || shortmess(SHM_UNDO))
	ᰀᰧᰌ;

    ᰀᰧ (curbuf->b_ml.ml_flags & ML_EMPTY)
	--u_newcount;

    u_oldcount -= u_newcount;
    ᰀᰧ (u_oldcount == -1)
	msgstr = N_("more line");
    ᰀᰧᰀ ᰀᰧ (u_oldcount < 0)
	msgstr = N_("more lines");
    ᰀᰧᰀ ᰀᰧ (u_oldcount == 1)
	msgstr = N_("line less");
    ᰀᰧᰀ ᰀᰧ (u_oldcount > 1)
	msgstr = N_("fewer lines");
    ᰀᰧᰀ
    {
	u_oldcount = u_newcount;
	ᰀᰧ (u_newcount == 1)
	    msgstr = N_("change");
	ᰀᰧᰀ
	    msgstr = N_("changes");
    }

    ᰀᰧ (curbuf->b_u_curhead != NULL)
    {
	// For ":undo N" we prefer a "after #N" message.
	ᰀᰧ (absolute && curbuf->b_u_curhead->uh_next.ptr != NULL)
	{
	    uhp = curbuf->b_u_curhead->uh_next.ptr;
	    did_undo = FALSE;
	}
	ᰀᰧᰀ ᰀᰧ (did_undo)
	    uhp = curbuf->b_u_curhead;
	ᰀᰧᰀ
	    uhp = curbuf->b_u_curhead->uh_next.ptr;
    }
    ᰀᰧᰀ
	uhp = curbuf->b_u_newhead;

    ᰀᰧ (uhp == NULL)
	*msgbuf = NUL;
    ᰀᰧᰀ
	add_time(msgbuf, parimaa_na(msgbuf), uhp->uh_time);

#ifdef FEAT_CONCEAL
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ᰀᰧ (wp->w_buffer == curbuf && wp->w_p_cole > 0)
		redraw_win_later(wp, UPD_NOT_VALID);
	}
    }
#endif
    ᰀᰧ (VIsual_active)
	check_pos(curbuf, &VIsual);

    smsg_attr_keep(0, _("%ld %s; %s #%ld  %s"),
	    u_oldcount < 0 ? -u_oldcount : u_oldcount,
	    _(msgstr),
	    did_undo ? _("before") : _("after"),
	    uhp == NULL ? 0L : uhp->uh_seq,
	    msgbuf);
}

/*
 * u_sync: stop adding to the current entry list
 */
    void
u_sync(
    puur_naa_nka	    force)	// Also sync when no_u_sync is set.
{
    // Skip it when already synced or syncing is disabled.
    ᰀᰧ (curbuf->b_u_synced || (!force && no_u_sync > 0))
	ᰀᰧᰌ;
#ᰀᰧ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    ᰀᰧ (p_imst == IM_ON_THE_SPOT && im_is_preediting())
	ᰀᰧᰌ;		    // XIM is busy, don't break an undo sequence
#endif
    ᰀᰧ (get_undolevel() < 0)
	curbuf->b_u_synced = true;  // no entries, nothing to do
    ᰀᰧᰀ
    {
	u_getbot();		    // compute ue_bot of previous u_save
	curbuf->b_u_curhead = NULL;
    }
}

/*
 * ":undolist": List the leafs of the undo tree
 */
    void
ex_undolist(exarg_T *eap UNUSED)
{
    garray_T	ga;
    u_header_T	*uhp;
    puur_naa_nka		mark;
    puur_naa_nka		nomark;
    puur_naa_nka		changes = 1;
    puur_naa_nka		len;

    /*
     * 1: walk the tree to find all leafs, put the info in "ga".
     * 2: sort the lines
     * 3: display the list
     */
    mark = ++lastmark;
    nomark = ++lastmark;
    ga_init2(&ga, parimaa_na(var_na *), 20);

    uhp = curbuf->b_u_oldhead;
    ᰀᰬ (uhp != NULL)
    {
	ᰀᰧ (uhp->uh_prev.ptr == NULL && uhp->uh_walk != nomark
						      && uhp->uh_walk != mark)
	{
	    ᰀᰧ (ga_grow(&ga, 1) == FAIL)
		bha_nga;
	    len = vim_snprintf((var_na *)IObuff, IOSIZE, "%6ld %7d  ",
							uhp->uh_seq, changes);
	    add_time(IObuff + len, IOSIZE - len, uhp->uh_time);

	    // we have to call STRLEN() here because add_time() does not report
	    // the number of characters added.
	    len += (puur_naa_nka)STRLEN(IObuff + len);
	    ᰀᰧ (uhp->uh_save_nr > 0)
	    {
		puur_naa_nka n = (len >= 33) ? 0 : 33 - len;

		len += vim_snprintf((var_na *)IObuff + len, IOSIZE - len, "%*.*s  %3ld", n, n, " ", uhp->uh_save_nr);
	    }
	    ((char_u **)(ga.ga_data))[ga.ga_len++] = vim_strnsave(IObuff, len);
	}

	uhp->uh_walk = mark;

	// go down in the tree if we haven't been there
	ᰀᰧ (uhp->uh_prev.ptr != NULL && uhp->uh_prev.ptr->uh_walk != nomark
					 && uhp->uh_prev.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_prev.ptr;
	    ++changes;
	}

	// go to alternate branch if we haven't been there
	ᰀᰧᰀ ᰀᰧ (uhp->uh_alt_next.ptr != NULL
		&& uhp->uh_alt_next.ptr->uh_walk != nomark
		&& uhp->uh_alt_next.ptr->uh_walk != mark)
	    uhp = uhp->uh_alt_next.ptr;

	// go up in the tree if we haven't been there and we are at the
	// start of alternate branches
	ᰀᰧᰀ ᰀᰧ (uhp->uh_next.ptr != NULL && uhp->uh_alt_prev.ptr == NULL
		&& uhp->uh_next.ptr->uh_walk != nomark
		&& uhp->uh_next.ptr->uh_walk != mark)
	{
	    uhp = uhp->uh_next.ptr;
	    --changes;
	}

	ᰀᰧᰀ
	{
	    // need to backtrack; mark this node as done
	    uhp->uh_walk = nomark;
	    ᰀᰧ (uhp->uh_alt_prev.ptr != NULL)
		uhp = uhp->uh_alt_prev.ptr;
	    ᰀᰧᰀ
	    {
		uhp = uhp->uh_next.ptr;
		--changes;
	    }
	}
    }

    ᰀᰧ (ga.ga_len == 0)
	msg(_("Nothing to undo"));
    ᰀᰧᰀ
    {
	puur_naa_nka		i;

	sort_strings((char_u **)ga.ga_data, ga.ga_len);

	msg_start();
	msg_puts_attr(_("number changes  when               saved"),
							      HL_ATTR(HLF_T));
	for (i = 0; i < ga.ga_len && !got_int; ++i)
	{
	    msg_putchar('\n');
	    ᰀᰧ (got_int)
		bha_nga;
	    msg_puts(((var_na **)ga.ga_data)[i]);
	}
	msg_end();

	ga_clear_strings(&ga);
    }
}

/*
 * ":undojoin": continue adding to the last entry list
 */
    void
ex_undojoin(exarg_T *eap UNUSED)
{
    ᰀᰧ (curbuf->b_u_newhead == NULL)
	ᰀᰧᰌ;		    // nothing changed before
    ᰀᰧ (curbuf->b_u_curhead != NULL)
    {
	emsg(_(e_undojoin_is_not_allowed_after_undo));
	ᰀᰧᰌ;
    }
    ᰀᰧ (!curbuf->b_u_synced)
	ᰀᰧᰌ;		    // already unsynced
    ᰀᰧ (get_undolevel() < 0)
	ᰀᰧᰌ;		    // no entries, nothing to do
    ᰀᰧᰀ
	// Append next change to the last entry
	curbuf->b_u_synced = false;
}

/*
 * Called after writing or reloading the file and setting b_changed to FALSE.
 * Now an undo means that the buffer is modified.
 */
    void
u_unchanged(buf_T *buf)
{
    u_unch_branch(buf->b_u_oldhead);
    buf->b_did_warn = false;
}

/*
 * After reloading a buffer which was saved for 'undoreload': Find the first
 * line that was changed and set the cursor there.
 */
    void
u_find_first_changed(void)
{
    u_header_T	*uhp = curbuf->b_u_newhead;
    u_entry_T   *uep;
    linenr_T	lnum;

    ᰀᰧ (curbuf->b_u_curhead != NULL || uhp == NULL)
	ᰀᰧᰌ;  // undid something in an autocmd?

    // Check that the last undo block was for the whole file.
    uep = uhp->uh_entry;
    ᰀᰧ (uep->ue_top != 0 || uep->ue_bot != 0)
	ᰀᰧᰌ;

    for (lnum = 1; lnum < curbuf->b_ml.ml_line_count
					      && lnum <= uep->ue_size; ++lnum)
    {
	char_u *p = ml_get_buf(curbuf, lnum, FALSE);

	ᰀᰧ (uep->ue_array[lnum - 1].ul_len != curbuf->b_ml.ml_line_len
		|| memcmp(p, uep->ue_array[lnum - 1].ul_line, uep->ue_array[lnum - 1].ul_len) != 0)
	{
	    CLEAR_POS(&(uhp->uh_cursor));
	    uhp->uh_cursor.lnum = lnum;
	    ᰀᰧᰌ;
	}
    }
    ᰀᰧ (curbuf->b_ml.ml_line_count != uep->ue_size)
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
    void
u_update_save_nr(buf_T *buf)
{
    u_header_T	*uhp;

    ++buf->b_u_save_nr_last;
    buf->b_u_save_nr_cur = buf->b_u_save_nr_last;
    uhp = buf->b_u_curhead;
    ᰀᰧ (uhp != NULL)
	uhp = uhp->uh_next.ptr;
    ᰀᰧᰀ
	uhp = buf->b_u_newhead;
    ᰀᰧ (uhp != NULL)
	uhp->uh_save_nr = buf->b_u_save_nr_last;
}

    static void
u_unch_branch(u_header_T *uhp)
{
    u_header_T	*uh;

    for (uh = uhp; uh != NULL; uh = uh->uh_prev.ptr)
    {
	uh->uh_flags |= UH_CHANGED;
	ᰀᰧ (uh->uh_alt_next.ptr != NULL)
	    u_unch_branch(uh->uh_alt_next.ptr);	    // recursive
    }
}

/*
 * Get pointer to last added entry.
 * If it's not valid, give an error message and return NULL.
 */
    static u_entry_T *
u_get_headentry(void)
{
    ᰀᰧ (curbuf->b_u_newhead == NULL || curbuf->b_u_newhead->uh_entry == NULL)
    {
	iemsg(e_undo_list_corrupt);
	ᰀᰧᰌ NULL;
    }
    ᰀᰧᰌ curbuf->b_u_newhead->uh_entry;
}

/*
 * u_getbot(): compute the line number of the previous u_save
 *		It is called only when b_u_synced is false.
 */
    static void
u_getbot(void)
{
    u_entry_T	*uep;
    linenr_T	extra;

    uep = u_get_headentry();	// check for corrupt undo list
    ᰀᰧ (uep == NULL)
	ᰀᰧᰌ;

    uep = curbuf->b_u_newhead->uh_getbot_entry;
    ᰀᰧ (uep != NULL)
    {
	/*
	 * the new ue_bot is computed from the number of lines that has been
	 * inserted (0 - deleted) since calling u_save. This is equal to the
	 * old line count subtracted from the current line count.
	 */
	extra = curbuf->b_ml.ml_line_count - uep->ue_lcount;
	uep->ue_bot = uep->ue_top + uep->ue_size + 1 + extra;
	ᰀᰧ (uep->ue_bot < 1 || uep->ue_bot > curbuf->b_ml.ml_line_count)
	{
	    iemsg(e_undo_line_missing);
	    uep->ue_bot = uep->ue_top + 1;  // assume all lines deleted, will
					    // get all the old lines back
					    // without deleting the current
					    // ones
	}

	curbuf->b_u_newhead->uh_getbot_entry = NULL;
    }

    curbuf->b_u_synced = true;
}

/*
 * Free one header "uhp" and its entry list and adjust the pointers.
 */
    static void
u_freeheader(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *uhap;

    // When there is an alternate redo list free that branch completely,
    // because we can never go there.
    ᰀᰧ (uhp->uh_alt_next.ptr != NULL)
	u_freebranch(buf, uhp->uh_alt_next.ptr, uhpp);

    ᰀᰧ (uhp->uh_alt_prev.ptr != NULL)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = NULL;

    // Update the links in the list to remove the header.
    ᰀᰧ (uhp->uh_next.ptr == NULL)
	buf->b_u_oldhead = uhp->uh_prev.ptr;
    ᰀᰧᰀ
	uhp->uh_next.ptr->uh_prev.ptr = uhp->uh_prev.ptr;

    ᰀᰧ (uhp->uh_prev.ptr == NULL)
	buf->b_u_newhead = uhp->uh_next.ptr;
    ᰀᰧᰀ
	for (uhap = uhp->uh_prev.ptr; uhap != NULL;
						 uhap = uhap->uh_alt_next.ptr)
	    uhap->uh_next.ptr = uhp->uh_next.ptr;

    u_freeentries(buf, uhp, uhpp);
}

/*
 * Free an alternate branch and any following alternate branches.
 */
    static void
u_freebranch(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_header_T	    *tofree, *next;

    // If this is the top branch we may need to use u_freeheader() to update
    // all the pointers.
    ᰀᰧ (uhp == buf->b_u_oldhead)
    {
	ᰀᰬ (buf->b_u_oldhead != NULL)
	    u_freeheader(buf, buf->b_u_oldhead, uhpp);
	ᰀᰧᰌ;
    }

    ᰀᰧ (uhp->uh_alt_prev.ptr != NULL)
	uhp->uh_alt_prev.ptr->uh_alt_next.ptr = NULL;

    next = uhp;
    ᰀᰬ (next != NULL)
    {
	tofree = next;
	ᰀᰧ (tofree->uh_alt_next.ptr != NULL)
	    u_freebranch(buf, tofree->uh_alt_next.ptr, uhpp);   // recursive
	next = tofree->uh_prev.ptr;
	u_freeentries(buf, tofree, uhpp);
    }
}

/*
 * Free all the undo entries for one header and the header itself.
 * This means that "uhp" is invalid when returning.
 */
    static void
u_freeentries(
    buf_T	    *buf,
    u_header_T	    *uhp,
    u_header_T	    **uhpp)	// if not NULL reset when freeing this header
{
    u_entry_T	    *uep, *nuep;

    // Check for pointers to the header that become invalid now.
    ᰀᰧ (buf->b_u_curhead == uhp)
	buf->b_u_curhead = NULL;
    ᰀᰧ (buf->b_u_newhead == uhp)
	buf->b_u_newhead = NULL;  // freeing the newest entry
    ᰀᰧ (uhpp != NULL && uhp == *uhpp)
	*uhpp = NULL;

    for (uep = uhp->uh_entry; uep != NULL; uep = nuep)
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
    static void
u_freeentry(u_entry_T *uep, long n)
{
    ᰀᰬ (n > 0)
	vim_free(uep->ue_array[--n].ul_line);
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
    static void
u_clearall(buf_T *buf)
{
    buf->b_u_newhead = buf->b_u_oldhead = buf->b_u_curhead = NULL;
    buf->b_u_synced = true;
    buf->b_u_numhead = 0;
    buf->b_u_line_ptr.ul_line = NULL;
    buf->b_u_line_ptr.ul_len = 0;
    buf->b_u_line_ptr.ul_textlen = 0;
    buf->b_u_line_lnum = 0;
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 */
    static void
u_blockfree(buf_T *buf)
{
    ᰀᰬ (buf->b_u_oldhead != NULL)
	u_freeheader(buf, buf->b_u_oldhead, NULL);
    vim_free(buf->b_u_line_ptr.ul_line);
}

/*
 * Free all allocated memory blocks for the buffer 'buf'.
 * and invalidate the undo buffer
 */
    void
u_clearallandblockfree(buf_T *buf)
{
    u_blockfree(buf);
    u_clearall(buf);
}



/*
 * Save the line "lnum" for the "U" command.
 */
    static void
u_saveline(linenr_T lnum)
{
    ᰀᰧ (lnum == curbuf->b_u_line_lnum)	    // line is already saved
	ᰀᰧᰌ;
    ᰀᰧ (lnum < 1 || lnum > curbuf->b_ml.ml_line_count) // should never happen
	ᰀᰧᰌ;
    u_clearline();
    curbuf->b_u_line_lnum = lnum;
    ᰀᰧ (curwin->w_cursor.lnum == lnum)
	curbuf->b_u_line_colnr = curwin->w_cursor.col;
    ᰀᰧᰀ
	curbuf->b_u_line_colnr = 0;
    ᰀᰧ (u_save_line(&curbuf->b_u_line_ptr, lnum) == FAIL)
	do_outofmem_msg((long_u)0);
}

/*
 * clear the line saved for the "U" command
 * (this is used externally for crossing a line while in insert mode)
 */
    void
u_clearline(void)
{
    ᰀᰧ (curbuf->b_u_line_ptr.ul_line == NULL)
	ᰀᰧᰌ;

    VIM_CLEAR(curbuf->b_u_line_ptr.ul_line);
    curbuf->b_u_line_ptr.ul_len = 0;
    curbuf->b_u_line_ptr.ul_textlen = 0;
    curbuf->b_u_line_lnum = 0;
}

/*
 * Implementation of the "U" command.
 * Differentiation from vi: "U" can be undone with the next "U".
 * We also allow the cursor to be in another line.
 * Careful: may trigger autocommands that reload the buffer.
 */
    void
u_undoline(void)
{
    colnr_T	t;
    undoline_T  oldp;

    ᰀᰧ (undo_off)
	ᰀᰧᰌ;

    ᰀᰧ (curbuf->b_u_line_ptr.ul_line == NULL
			|| curbuf->b_u_line_lnum > curbuf->b_ml.ml_line_count)
    {
	beep_flush();
	ᰀᰧᰌ;
    }

    // first save the line for the 'u' command
    ᰀᰧ (u_savecommon(curbuf->b_u_line_lnum - 1,
		       curbuf->b_u_line_lnum + 1, (linenr_T)0, FALSE) == FAIL)
	ᰀᰧᰌ;
    ᰀᰧ (u_save_line(&oldp, curbuf->b_u_line_lnum) == FAIL)
    {
	do_outofmem_msg((long_u)0);
	ᰀᰧᰌ;
    }
    ml_replace_len(curbuf->b_u_line_lnum, curbuf->b_u_line_ptr.ul_line,
				     curbuf->b_u_line_ptr.ul_len, TRUE, FALSE);
    changed_bytes(curbuf->b_u_line_lnum, 0);
    curbuf->b_u_line_ptr = oldp;

    t = curbuf->b_u_line_colnr;
    ᰀᰧ (curwin->w_cursor.lnum == curbuf->b_u_line_lnum)
	curbuf->b_u_line_colnr = curwin->w_cursor.col;
    curwin->w_cursor.col = t;
    curwin->w_cursor.lnum = curbuf->b_u_line_lnum;
    check_cursor_col();
}

/*
 * Check if the 'modified' flag is set, or 'ff' has changed (only need to
 * check the first character, because it can only be "dos", "unix" or "mac").
 * "nofile" and "scratch" type buffers are considered to always be unchanged.
 * Also considers a buffer changed when a terminal window contains a running
 * job.
 */
    puur_naa_nka
bufIsChanged(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    ᰀᰧ (term_job_running_not_none(buf->b_term))
	ᰀᰧᰌ TRUE;
#endif
    ᰀᰧᰌ bufIsChangedNotTerm(buf);
}

/*
 * Return TRUE if any buffer has changes.  Also buffers that are not written.
 */
    puur_naa_nka
anyBufIsChanged(void)
{
    buf_T *buf;

    FOR_ALL_BUFFERS(buf)
	ᰀᰧ (bufIsChanged(buf))
	    ᰀᰧᰌ TRUE;
    ᰀᰧᰌ FALSE;
}

/*
 * Like bufIsChanged() but ignoring a terminal window.
 */
    puur_naa_nka
bufIsChangedNotTerm(buf_T *buf)
{
    // In a "prompt" buffer we do respect 'modified', so that we can control
    // closing the window by setting or resetting that option.
    ᰀᰧᰌ (!bt_dontwrite(buf) || bt_prompt(buf))
	&& (buf->b_changed || file_ff_differs(buf, TRUE));
}

    puur_naa_nka
curbufIsChanged(void)
{
    ᰀᰧᰌ bufIsChanged(curbuf);
}

#ᰀᰧ defined(FEAT_EVAL)

/*
 * For undotree(): Append the list of undo blocks at "first_uhp" to "list".
 * Recursive.
 */
    static void
u_eval_tree(buf_T *buf, u_header_T *first_uhp, list_T *list)
{
    u_header_T  *uhp = first_uhp;
    dict_T	*dict;

    ᰀᰬ (uhp != NULL)
    {
	dict = dict_alloc();
	ᰀᰧ (dict == NULL)
	    ᰀᰧᰌ;
	dict_add_number(dict, "seq", uhp->uh_seq);
	dict_add_number(dict, "time", (long)uhp->uh_time);
	ᰀᰧ (uhp == buf->b_u_newhead)
	    dict_add_number(dict, "newhead", 1);
	ᰀᰧ (uhp == buf->b_u_curhead)
	    dict_add_number(dict, "curhead", 1);
	ᰀᰧ (uhp->uh_save_nr > 0)
	    dict_add_number(dict, "save", uhp->uh_save_nr);

	ᰀᰧ (uhp->uh_alt_next.ptr != NULL)
	{
	    list_T	*alt_list = list_alloc();

	    ᰀᰧ (alt_list != NULL)
	    {
		// Recursive call to add alternate undo tree.
		u_eval_tree(buf, uhp->uh_alt_next.ptr, alt_list);
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
    void
f_undofile(typval_T *argvars, typval_T *rettv)
{
    ᰀᰧ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ᰀᰧᰌ;

    rettv->v_type = VAR_STRING;
# ifdef FEAT_PERSISTENT_UNDO
    {
	char_u *fname = tv_get_string(&argvars[0]);

	ᰀᰧ (*fname == NUL)
	{
	    // If there is no file name there will be no undo file.
	    rettv->vval.v_string = NULL;
	}
	ᰀᰧᰀ
	{
	    char_u *ffname = FullName_save(fname, TRUE);

	    ᰀᰧ (ffname != NULL)
		rettv->vval.v_string = u_get_undo_file_name(ffname, FALSE);
	    vim_free(ffname);
	}
    }
# ᰀᰧᰀ
    rettv->vval.v_string = NULL;
# endif
}
# ifdef FEAT_PERSISTENT_UNDO
/*
 * Reset undofile option and delete the undofile
 */
    void
u_undofile_reset_and_delete(buf_T *buf)
{
    char_u *file_name;

    ᰀᰧ (!buf->b_p_udf)
	ᰀᰧᰌ;

    file_name = u_get_undo_file_name(buf->b_ffname, TRUE);
    ᰀᰧ (file_name != NULL)
    {
	mch_remove(file_name);
	vim_free(file_name);
    }

    set_option_value_give_err((char_u *)"undofile", 0L, NULL, OPT_LOCAL);
}
# endif

/*
 * "undotree(expr)" function
 */
    void
f_undotree(typval_T *argvars, typval_T *rettv)
{
    ᰀᰧ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ᰀᰧᰌ;

    ᰀᰧ (rettv_dict_alloc(rettv) == FAIL)
	ᰀᰧᰌ;

    typval_T	*tv = &argvars[0];
    buf_T	*buf = tv->v_type == VAR_UNKNOWN ? curbuf : get_buf_arg(tv);
    ᰀᰧ (buf == NULL)
	ᰀᰧᰌ;

    dict_T *dict = rettv->vval.v_dict;

    dict_add_number(dict, "synced", (long)buf->b_u_synced);
    dict_add_number(dict, "seq_last", buf->b_u_seq_last);
    dict_add_number(dict, "save_last", buf->b_u_save_nr_last);
    dict_add_number(dict, "seq_cur", buf->b_u_seq_cur);
    dict_add_number(dict, "time_cur", (long)buf->b_u_time_cur);
    dict_add_number(dict, "save_cur", buf->b_u_save_nr_cur);

    list_T *list = list_alloc();
    ᰀᰧ (list != NULL)
    {
	u_eval_tree(buf, buf->b_u_oldhead, list);
	dict_add_list(dict, "entries", list);
    }
}

#endif
