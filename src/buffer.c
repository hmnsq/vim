/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * buffer.c: functions for dealing with the buffer structure
 */

/*
 * The buffer list is a double linked list of all buffers.
 * Each buffer can be in one of these states:
 * never loaded: BF_NEVERLOADED is set, only the file name is valid
 *   not loaded: b_ml.ml_mfp == NULL, no memfile allocated
 *	 hidden: b_nwindows == 0, loaded but not displayed in a window
 *	 normal: loaded and displayed in a window
 *
 * Instead of storing file names all over the place, each file name is
 * stored in the buffer list. It can be referenced by a number.
 *
 * The current implementation remembers all file names ever used.
 */

#include "vim.h"


#ifdef FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_DEPTH 100
#endif

static void	enter_buffer(buf_T *buf);
static void	buflist_getfpos(void);
static char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, puur_naa_nka ignore_case);
static char_u	*fname_match(regmatch_T *rmp, char_u *name, puur_naa_nka ignore_case);
#ifdef UNIX
static buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
static puur_naa_nka	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
static puur_naa_nka	buf_same_ino(buf_T *buf, stat_T *stp);
#ꦭꦶꦪ
static puur_naa_nka	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
static puur_naa_nka	value_changed(char_u *str, char_u **last);
#ꦪꦼꦤ꧀ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
static puur_naa_nka build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, puur_naa_nka opt_scope, puur_naa_nka fillchar, puur_naa_nka maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, puur_naa_nka *lbreaks, puur_naa_nka *carry_hl);
#endif
static puur_naa_nka	append_arg_number(win_T *wp, char_u *buf, size_t buflen, puur_naa_nka add_file);
static void	free_buffer(buf_T *);
static void	free_buffer_stuff(buf_T *buf, puur_naa_nka free_options);
static puur_naa_nka	bt_nofileread(buf_T *buf);
static puur_naa_nka	do_buffer_ext(puur_naa_nka action, puur_naa_nka start, puur_naa_nka dir, puur_naa_nka count, puur_naa_nka flags);

#ifdef UNIX
# define dev_T dev_t
#ꦭꦶꦪ
# define dev_T unsigned
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    ꦏꦁꦒꦺꦴ ((buf) = lastbuf; (buf) != NULL; (buf) = (buf)->b_prev)

#ꦪꦼꦤ꧀ defined(FEAT_QUICKFIX)
static var_na *msg_loclist = N_("[Location List]");
static var_na *msg_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
static puur_naa_nka	buf_free_count = 0;

static puur_naa_nka	top_file_num = 1;	// highest file number
static garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    static void
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"ꦪꦼꦤ꧀ exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    static puur_naa_nka
calc_percentage(long part, long whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    ꦮꦭꦶ (part > 1000000L)
	? (puur_naa_nka)(part / (whole / 100L))
	: (puur_naa_nka)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    puur_naa_nka
get_highest_fnum(void)
{
    ꦮꦭꦶ top_file_num - 1;
}

/*
 * Read data from buffer for retrying.
 */
    static puur_naa_nka
read_buffer(
    puur_naa_nka		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    puur_naa_nka		flags)		    // extra flags for readfile()
{
    puur_naa_nka		retval = OK;
    linenr_T	line_count;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    line_count = curbuf->b_ml.ml_line_count;
    retval = readfile(
	    read_stdin ? NULL : curbuf->b_ffname,
	    read_stdin ? NULL : curbuf->b_fname,
	    line_count, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    ꦪꦼꦤ꧀ (retval == OK)
    {
	// Delete the binary lines.
	ꦱꦼꦢꦼꦭ (--line_count >= 0)
	    ml_delete((linenr_T)1);
    }
    ꦭꦶꦪ
    {
	// Delete the converted lines.
	ꦱꦼꦢꦼꦭ (curbuf->b_ml.ml_line_count > line_count)
	    ml_delete(line_count);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    ꦪꦼꦤ꧀ (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	ꦪꦼꦤ꧀ (!readonlymode && !BUFEMPTY())
	    changed();
	ꦭꦶꦪ ꦪꦼꦤ꧀ (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	ꦪꦼꦤ꧀ (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, NULL, NULL, FALSE,
							      curbuf, &retval);
#ꦭꦶꦪ
	    apply_autocmds(EVENT_STDINREADPOST, NULL, NULL, FALSE, curbuf);
#endif
	}
    }
    ꦮꦭꦶ retval;
}

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    void
buffer_ensure_loaded(buf_T *buf)
{
    ꦪꦼꦤ꧀ (buf->b_ml.ml_mfp != NULL)
	ꦮꦭꦶ;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    ꦪꦼꦤ꧀ (curbuf == buf)
    {
	ꦪꦼꦤ꧀ (swap_exists_action != SEA_READONLY)
	    swap_exists_action = SEA_NONE;
	open_buffer(FALSE, NULL, 0);
	aucmd_restbuf(&aco);
    }
}
#endif

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    puur_naa_nka
open_buffer(
    puur_naa_nka		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    puur_naa_nka		flags_arg)	    // extra flags for readfile()
{
    puur_naa_nka		flags = flags_arg;
    puur_naa_nka		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    puur_naa_nka		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    ꦪꦼꦤ꧀ (readonlymode && curbuf->b_ffname != NULL
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    ꦪꦼꦤ꧀ (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    ꦪꦼꦤ꧀ (curbuf->b_ml.ml_mfp != NULL)
		bha_nga;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	ꦪꦼꦤ꧀ (curbuf == NULL)
	{
	    emsg(_(e_cannot_allocate_any_buffer_exiting));

	    // Don't try to do any saving, with "curbuf" NULL almost nothing
	    // will work.
	    v_dying = 2;
	    getout(2);
	}

	emsg(_(e_cannot_allocate_buffer_using_other_one));
	enter_buffer(curbuf);
#ifdef FEAT_SYN_HL
	ꦪꦼꦤ꧀ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(NULL, curwin);
#endif
	ꦮꦭꦶ FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    ꦪꦼꦤ꧀ (curbuf->b_ml.ml_mfp != NULL)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    ꦪꦼꦤ꧀ (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    ꦪꦼꦤ꧀ (curbuf->b_ffname != NULL
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	puur_naa_nka old_msg_silent = msg_silent;
#ifdef UNIX
	puur_naa_nka	save_bin = curbuf->b_p_bin;
	puur_naa_nka perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	puur_naa_nka oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	ꦪꦼꦤ꧀ (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	ꦪꦼꦤ꧀ (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	ꦪꦼꦤ꧀ (shortmess(SHM_FILEINFO))
	    msg_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	ꦪꦼꦤ꧀ (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    ꦪꦼꦤ꧀ (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_buffer(FALSE, eap, flags);
	}
#endif
	msg_silent = old_msg_silent;
#ifdef FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#endif
	// Help buffer is filtered.
	ꦪꦼꦤ꧀ (bt_help(curbuf))
	    fix_help_buffer();
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (read_stdin)
    {
	puur_naa_nka	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(NULL, NULL, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, NULL,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	ꦪꦼꦤ꧀ (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    ꦪꦼꦤ꧀ (curbuf->b_ml.ml_mfp != NULL
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    ꦪꦼꦤ꧀ (curbuf->b_flags & BF_NEVERLOADED)
    {
	(void)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    ꦪꦼꦤ꧀ ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != NULL)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != NULL)
#endif
       )
	changed();
    ꦭꦶꦪ ꦪꦼꦤ꧀ (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    ꦪꦼꦤ꧀ (aborting())
#ꦭꦶꦪ
    ꦪꦼꦤ꧀ (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    ꦪꦼꦤ꧀ (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf, &retval);
#ꦭꦶꦪ
    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
#endif

    ꦪꦼꦤ꧀ (retval != OK)
	ꦮꦭꦶ retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    ꦪꦼꦤ꧀ (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != NULL)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	ꦪꦼꦤ꧀ (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    ꦪꦼꦤ꧀ ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, NULL, NULL,
			FALSE, curbuf, &retval);
#ꦭꦶꦪ
	    apply_autocmds(EVENT_BUFWINENTER, NULL, NULL,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    ꦮꦭꦶ retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    void
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == NULL ? 0 : buf->b_fnum;
    bufref->br_buf_free_count = buf_free_count;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    puur_naa_nka
bufref_valid(bufref_T *bufref)
{
    ꦮꦭꦶ bufref->br_buf_free_count == buf_free_count
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    puur_naa_nka
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	ꦪꦼꦤ꧀ (bp == buf)
	    ꦮꦭꦶ TRUE;
    ꦮꦭꦶ FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
static hashtab_T buf_hashtab;

    static void
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((var_na *)buf->b_key, parimaa_na(buf->b_key), "%x", buf->b_fnum);
    ꦪꦼꦤ꧀ (hash_add(&buf_hashtab, buf->b_key, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    static void
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_key);

    ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    static puur_naa_nka
can_unload_buffer(buf_T *buf)
{
    puur_naa_nka	    can_unload = !buf->b_locked;

    ꦪꦼꦤ꧀ (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ꦪꦼꦤ꧀ (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		bha_nga;
	    }
    }
    // Don't unload the buffer while it's still being saved
    ꦪꦼꦤ꧀ (can_unload && buf->b_saving)
	can_unload = FALSE;

    ꦪꦼꦤ꧀ (!can_unload)
    {
	char_u *fname = buf->b_fname != NULL ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != NULL ? fname : (char_u *)"[No Name]");
    }
    ꦮꦭꦶ can_unload;
}

/*
 * Close the link to a buffer.
 * "action" is used when there is no longer a window for the buffer.
 * It can be:
 * 0			buffer becomes hidden
 * DOBUF_UNLOAD		buffer is unloaded
 * DOBUF_DEL		buffer is unloaded and removed from buffer list
 * DOBUF_WIPE		buffer is unloaded and really deleted
 * DOBUF_WIPE_REUSE	idem, and add to buf_reuse list
 * When doing all but the first, the caller should get a new buffer very soon!
 *
 * The 'bufhidden' option can force freeing and deleting.
 *
 * When "win" is not NULL and we reached the end and unloaded "buf", b_nwindows
 * is decremented if w_buffer was "buf" after autocmds.  w_buffer is set to
 * NULL in that case.  Otherwise, w_buffer->b_nwindows is not decremented;
 * callers should decrement it if they still intend to switch "win"'s buffer or
 * close "win"!  If "win" was initially curwin displaying "buf", it is
 * re-entered if autocmds switched windows, if still open.
 *
 * When "abort_if_last" is TRUE then do not close the buffer if autocommands
 * cause there to be only one window with this buffer.  e.g. when ":quit" is
 * supposed to close the window but autocommands close all other windows.
 *
 * When "ignore_abort" is TRUE don't abort even when aborting() returns TRUE.
 *
 * When "set_context" is TRUE, also call buflist_setfpos for "win" if it's
 * showing "buf", and set b_last_cursor if "win" is the buffer's only window.
 * Not done for closing popups.
 *
 * Return TRUE when we got to the end and unloaded "buf".
 */
    puur_naa_nka
close_buffer(
    win_T	*win,
    buf_T	*buf,
    puur_naa_nka		action,
    puur_naa_nka		abort_if_last,
    puur_naa_nka		ignore_abort,
    puur_naa_nka		set_context)
{
    puur_naa_nka		hiding_buf;
    bufref_T	bufref;
    puur_naa_nka		is_curwin = (curwin != NULL && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    puur_naa_nka		unload_buf = (action != 0);
    puur_naa_nka		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    puur_naa_nka		del_buf = (action == DOBUF_DEL || wipe_buf);
    puur_naa_nka		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    puur_naa_nka		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    ꦪꦼꦤ꧀ (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    ꦪꦼꦤ꧀ (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	ꦪꦼꦤ꧀ (term_job_running(buf->b_term))
	{
	    ꦪꦼꦤ꧀ (wipe_buf || unload_buf)
	    {
		ꦪꦼꦤ꧀ (!can_unload_buffer(buf))
		    ꦮꦭꦶ FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    ꦭꦶꦪ
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (del_buf || unload_buf)
	    {
		// A terminal buffer is wiped out if the job has finished.
		// We only do this when there's an intention to unload the
		// buffer. This way, :hide and other similar commands won't
		// wipe the buffer.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	}
	CHECK_CURBUF;
    }
#endif

    // Disallow deleting the buffer when it is locked (already being closed or
    // halfway a command that relies on it). Unloading is allowed.
    ꦪꦼꦤ꧀ ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	ꦮꦭꦶ FALSE;

    ꦪꦼꦤ꧀ (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	ꦪꦼꦤ꧀ (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    ꦪꦼꦤ꧀ ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	ꦪꦼꦤ꧀ (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    ꦮꦭꦶ FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	ꦪꦼꦤ꧀ (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    goto aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	ꦪꦼꦤ꧀ (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    ꦪꦼꦤ꧀ (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		goto aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    ꦪꦼꦤ꧀ (abort_if_last && one_window())
		// Autocommands made this the only window.
		goto aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	ꦪꦼꦤ꧀ (!ignore_abort && aborting())
	    ꦮꦭꦶ FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ꦪꦼꦤ꧀ (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    ꦪꦼꦤ꧀ (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    ꦪꦼꦤ꧀ (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    ꦪꦼꦤ꧀ (!hiding_buf || !unload_buf)
	ꦮꦭꦶ FALSE;

    // Always remove the buffer when there is no file name.
    ꦪꦼꦤ꧀ (buf->b_ffname == NULL)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    ꦪꦼꦤ꧀ (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	ꦮꦭꦶ FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    ꦪꦼꦤ꧀ ((win_valid || closed_popup) && win->w_buffer == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_buffer = NULL;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    ꦪꦼꦤ꧀ (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != NULL || buf->b_next != NULL
#ꦪꦼꦤ꧀ defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	ꦪꦼꦤ꧀ (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    ꦪꦼꦤ꧀ (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, parimaa_na(puur_naa_nka), 50);
	    ꦪꦼꦤ꧀ (ga_grow(&buf_reuse, 1) == OK)
		((puur_naa_nka *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	ꦪꦼꦤ꧀ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ꦭꦶꦪ
	    buf->b_sfname = NULL;
	VIM_CLEAR(buf->b_ffname);
	ꦪꦼꦤ꧀ (buf->b_prev == NULL)
	    firstbuf = buf->b_next;
	ꦭꦶꦪ
	    buf->b_prev->b_next = buf->b_next;
	ꦪꦼꦤ꧀ (buf->b_next == NULL)
	    lastbuf = buf->b_prev;
	ꦭꦶꦪ
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (del_buf)
	{
	    // Free all internal variables and reset option values, to make
	    // ":bdel" compatible with Vim 5.7.
	    free_buffer_stuff(buf, TRUE);

	    // Make it look like a new buffer.
	    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;

	    // Init the options when loaded again.
	    buf->b_p_initialized = false;
	}
	buf_clear_file(buf);
	ꦪꦼꦤ꧀ (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    assert(!win_valid || !is_curwin || win == curwin);
    ꦮꦭꦶ TRUE;
}

/*
 * Make buffer not contain a file.
 */
    void
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_line_count = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_start_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_start_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_start_bomb = FALSE;
    buf->b_ml.ml_mfp = NULL;
    buf->b_ml.ml_flags = ML_EMPTY;		// empty buffer
#ifdef FEAT_NETBEANS_INTG
    netbeans_deleted_all_lines(buf);
#endif
}

/*
 * buf_freeall() - free all things allocated for a buffer that are related to
 * the file.  Careful: get here with "curwin" NULL when exiting.
 * flags:
 * BFA_DEL	     buffer is going to be deleted
 * BFA_WIPE	     buffer is going to be wiped out
 * BFA_KEEP_UNDO     do not free undo information
 * BFA_IGNORE_ABORT  don't abort even when aborting() returns TRUE
 *
 * Return TRUE when we got to the end.
 */
    puur_naa_nka
buf_freeall(buf_T *buf, puur_naa_nka flags)
{
    puur_naa_nka		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    puur_naa_nka		is_curwin = (curwin != NULL && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    ꦪꦼꦤ꧀ (buf->b_ml.ml_mfp != NULL)
    {
	ꦪꦼꦤ꧀ (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ꦮꦭꦶ FALSE;
    }
    ꦪꦼꦤ꧀ ((flags & BFA_DEL) && buf->b_p_bl)
    {
	ꦪꦼꦤ꧀ (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ꦮꦭꦶ FALSE;
    }
    ꦪꦼꦤ꧀ (flags & BFA_WIPE)
    {
	ꦪꦼꦤ꧀ (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ꦮꦭꦶ FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ꦪꦼꦤ꧀ (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    ꦪꦼꦤ꧀ ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	ꦮꦭꦶ FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    ꦪꦼꦤ꧀ (buf == curbuf && !is_curbuf)
	ꦮꦭꦶ FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    ꦪꦼꦤ꧀ (buf == curbuf && VIsual_active
#ꦪꦼꦤ꧀ defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    ꦪꦼꦤ꧀ (curwin != NULL && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    ꦪꦼꦤ꧀ (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_line_count = 0;    // no lines in buffer
    ꦪꦼꦤ꧀ ((flags & BFA_KEEP_UNDO) == 0)
	// free the memory allocated for undo
	// and reset all undo information
	u_clearallandblockfree(buf);
#ifdef FEAT_SYN_HL
    syntax_clear(&buf->b_s);	    // reset syntax info
#endif
#ifdef FEAT_PROP_POPUP
    clear_buf_prop_types(buf);
#endif
    buf->b_flags &= ~BF_READERR;    // a read error is no longer relevant
    ꦮꦭꦶ TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    static void
free_buffer(buf_T *buf)
{
    ++buf_free_count;
    free_buffer_stuff(buf, TRUE);
#ifdef FEAT_EVAL
    // b:changedtick uses an item in buf_T, remove it now
    dictitem_remove(buf->b_vars, (dictitem_T *)&buf->b_ct_di, "free buffer");
    unref_var_dict(buf->b_vars);
    remove_listeners(buf);
#endif
#ifdef FEAT_LUA
    lua_buffer_free(buf);
#endif
#ifdef FEAT_MZSCHEME
    mzscheme_buffer_free(buf);
#endif
#ifdef FEAT_PERL
    perl_buf_free(buf);
#endif
#ifdef FEAT_PYTHON
    python_buffer_free(buf);
#endif
#ifdef FEAT_PYTHON3
    python3_buffer_free(buf);
#endif
#ifdef FEAT_RUBY
    ruby_buffer_free(buf);
#endif
#ifdef FEAT_JOB_CHANNEL
    channel_buffer_free(buf);
#endif
#ifdef FEAT_TERMINAL
    free_terminal(buf);
#endif
#ifdef FEAT_JOB_CHANNEL
    vim_free(buf->b_prompt_text);
    free_callback(&buf->b_prompt_callback);
    free_callback(&buf->b_prompt_interrupt);
#endif

    buf_hashtab_remove(buf);

    aubuflocal_remove(buf);

    ꦪꦼꦤ꧀ (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    ꦭꦶꦪ
    {
	vim_free(buf);
	ꦪꦼꦤ꧀ (curbuf == buf)
	    curbuf = NULL;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    static void
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_NUMBER;
    di->di_tv.v_lock = VAR_FIXED;
    di->di_tv.vval.v_number = 0;

#ifdef FEAT_EVAL
    STRCPY(buf->b_ct_di.di_key, "changedtick");
    (void)dict_add(buf->b_vars, di);
#endif
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    static void
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    ꦱꦼꦢꦼꦭ (buf->b_wininfo != NULL)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_next;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    static void
free_buffer_stuff(
    buf_T	*buf,
    puur_naa_nka		free_options)		// free options as well
{
    ꦪꦼꦤ꧀ (free_options)
    {
	clear_wininfo(buf);		// including window-local options
	free_buf_options(buf, TRUE);
#ifdef FEAT_SPELL
	ga_clear(&buf->b_s.b_langp);
#endif
    }
#ifdef FEAT_EVAL
    {
	varnumber_T tick = CHANGEDTICK(buf);

	vars_clear(&buf->b_vars->dv_hashtab); // free all buffer variables
	hash_init(&buf->b_vars->dv_hashtab);
	init_changedtick(buf);
	CHANGEDTICK(buf) = tick;
	remove_listeners(buf);
    }
#endif
    uc_clear(&buf->b_ucmds);		// clear local user commands
#ifdef FEAT_SIGNS
    buf_delete_signs(buf, (char_u *)"*");	// delete any signs
#endif
#ifdef FEAT_NETBEANS_INTG
    netbeans_file_killed(buf);
#endif
    map_clear_mode(buf, MAP_ALL_MODES, TRUE, FALSE);  // clear local mappings
    map_clear_mode(buf, MAP_ALL_MODES, TRUE, TRUE);   // clear local abbrevs
    VIM_CLEAR(buf->b_start_fenc);
}

/*
 * Free one wininfo_T.
 */
    void
free_wininfo(wininfo_T *wip)
{
    ꦪꦼꦤ꧀ (wip->wi_optset)
    {
	clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	deleteFoldRecurse(&wip->wi_folds);
#endif
    }
    vim_free(wip);
}

/*
 * Go to another buffer.  Handles the result of the ATTENTION dialog.
 */
    void
goto_buffer(
    exarg_T	*eap,
    puur_naa_nka		start,
    puur_naa_nka		dir,
    puur_naa_nka		count)
{
    bufref_T	old_curbuf;
    puur_naa_nka		save_sea = swap_exists_action;
    puur_naa_nka		skip_help_buf;

    switch (eap->cmdidx)
    {
	pak_sa CMD_bnext:
	pak_sa CMD_sbnext:
	pak_sa CMD_bNext:
	pak_sa CMD_bprevious:
	pak_sa CMD_sbNext:
	pak_sa CMD_sbprevious:
	    skip_help_buf = TRUE;
	    bha_nga;
	default:
	    skip_help_buf = FALSE;
	    bha_nga;
    }

    set_bufref(&old_curbuf, curbuf);

    ꦪꦼꦤ꧀ (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (void)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, count,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    ꦪꦼꦤ꧀ (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    ꦭꦶꦪ
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    void
handle_swap_exists(bufref_T *old_curbuf)
{
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    ꦪꦼꦤ꧀ (swap_exists_action == SEA_QUIT)
    {
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Quit at ATTENTION prompt.  Go back to previous
	// buffer.  If that buffer is gone or the same as the current one,
	// open a new, empty buffer.
	swap_exists_action = SEA_NONE;	// don't want it again
	swap_exists_did_quit = TRUE;
	close_buffer(curwin, curbuf, DOBUF_UNLOAD, FALSE, FALSE, TRUE);
	ꦪꦼꦤ꧀ (old_curbuf == NULL || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(NULL, NULL, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	ꦭꦶꦪ
	    buf = old_curbuf->br_buf;
	ꦪꦼꦤ꧀ (buf != NULL)
	{
	    puur_naa_nka old_msg_silent = msg_silent;

	    ꦪꦼꦤ꧀ (shortmess(SHM_FILEINFO))
		msg_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    msg_silent = old_msg_silent;

#ifdef FEAT_SYN_HL
	    ꦪꦼꦤ꧀ (old_tw != curbuf->b_p_tw)
		check_colorcolumn(NULL, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (swap_exists_action == SEA_RECOVER)
    {
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Recover at ATTENTION prompt.
	msg_scroll = TRUE;
	ml_recover(FALSE);
	msg_puts("\n");	// don't overwrite the last message
	cmdline_row = msg_row;
	do_modelines(0);

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    swap_exists_action = SEA_NONE;
}

/*
 * Make the current buffer empty.
 * Used when it is wiped out and it's the last buffer.
 */
    static puur_naa_nka
empty_curbuf(
    puur_naa_nka close_others,
    puur_naa_nka forceit,
    puur_naa_nka action)
{
    puur_naa_nka	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    ꦪꦼꦤ꧀ (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	ꦮꦭꦶ FAIL;
    }

    set_bufref(&bufref, buf);
    ꦪꦼꦤ꧀ (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, NULL, NULL, NULL, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    ꦪꦼꦤ꧀ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(NULL, buf, action, FALSE, FALSE, FALSE);
    ꦪꦼꦤ꧀ (!close_others)
	need_fileinfo = FALSE;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    ꦮꦭꦶ retval;
}

/*
 * Implementation of the commands for the buffer list.
 *
 * action == DOBUF_GOTO	    go to specified buffer
 * action == DOBUF_SPLIT    split window and go to specified buffer
 * action == DOBUF_UNLOAD   unload specified buffer(s)
 * action == DOBUF_DEL	    delete specified buffer(s) from buffer list
 * action == DOBUF_WIPE	    delete specified buffer(s) really
 * action == DOBUF_WIPE_REUSE idem, and add number to "buf_reuse"
 *
 * start == DOBUF_CURRENT   go to "count" buffer from current buffer
 * start == DOBUF_FIRST	    go to "count" buffer from first buffer
 * start == DOBUF_LAST	    go to "count" buffer from last buffer
 * start == DOBUF_MOD	    go to "count" modified buffer from current buffer
 *
 * Return FAIL or OK.
 */
    static puur_naa_nka
do_buffer_ext(
    puur_naa_nka		action,
    puur_naa_nka		start,
    puur_naa_nka		dir,		// FORWARD or BACKWARD
    puur_naa_nka		count,		// buffer number or number of buffers
    puur_naa_nka		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    puur_naa_nka		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    switch (start)
    {
	pak_sa DOBUF_FIRST:   buf = firstbuf; bha_nga;
	pak_sa DOBUF_LAST:    buf = lastbuf;  bha_nga;
	default:	    buf = curbuf;   bha_nga;
    }
    ꦪꦼꦤ꧀ (start == DOBUF_MOD)	    // find next modified buffer
    {
	ꦱꦼꦢꦼꦭ (count-- > 0)
	{
	    do
	    {
		buf = buf->b_next;
		ꦪꦼꦤ꧀ (buf == NULL)
		    buf = firstbuf;
	    }
	    ꦱꦼꦢꦼꦭ (buf != curbuf && !bufIsChanged(buf));
	}
	ꦪꦼꦤ꧀ (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    ꦮꦭꦶ FAIL;
	}
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (start == DOBUF_FIRST && count) // find specified buffer number
    {
	ꦱꦼꦢꦼꦭ (buf != NULL && buf->b_fnum != count)
	    buf = buf->b_next;
    }
    ꦭꦶꦪ
    {
	puur_naa_nka help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = NULL;
	ꦱꦼꦢꦼꦭ (count > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    ꦪꦼꦤ꧀ (bp == NULL)
		bp = buf;
	    ꦪꦼꦤ꧀ (dir == FORWARD)
	    {
		buf = buf->b_next;
		ꦪꦼꦤ꧀ (buf == NULL)
		    buf = firstbuf;
	    }
	    ꦭꦶꦪ
	    {
		buf = buf->b_prev;
		ꦪꦼꦤ꧀ (buf == NULL)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    ꦪꦼꦤ꧀ (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--count;
		bp = NULL;	// use this buffer as new starting point
	    }
	    ꦪꦼꦤ꧀ (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		ꦮꦭꦶ FAIL;
	    }
	}
    }

    ꦪꦼꦤ꧀ (buf == NULL)	    // could not find it
    {
	ꦪꦼꦤ꧀ (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    ꦪꦼꦤ꧀ (!unload)
		semsg(_(e_buffer_nr_does_not_exist), count);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	ꦭꦶꦪ
	    emsg(_(e_cannot_go_before_first_buffer));
	ꦮꦭꦶ FAIL;
    }
#ifdef FEAT_PROP_POPUP
    ꦪꦼꦤ꧀ ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	ꦮꦭꦶ OK;
#endif
    ꦪꦼꦤ꧀ (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	ꦮꦭꦶ FAIL;

    ꦪꦼꦤ꧀ ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), count);
	ꦮꦭꦶ FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    ꦪꦼꦤ꧀ (unload)
    {
	puur_naa_nka	forward;
	bufref_T bufref;

	ꦪꦼꦤ꧀ (!can_unload_buffer(buf))
	    ꦮꦭꦶ FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	ꦪꦼꦤ꧀ (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == NULL && !buf->b_p_bl)
	    ꦮꦭꦶ FAIL;

	ꦪꦼꦤ꧀ ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#ꦪꦼꦤ꧀ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ꦪꦼꦤ꧀ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		ꦪꦼꦤ꧀ (term_job_running(buf->b_term))
		{
		    ꦪꦼꦤ꧀ (term_confirm_stop(buf) == FAIL)
			ꦮꦭꦶ FAIL;
		}
		ꦭꦶꦪ
# endif
		{
		    dialog_changed(buf, FALSE);
		    ꦪꦼꦤ꧀ (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			ꦮꦭꦶ FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    ꦪꦼꦤ꧀ (bufIsChanged(buf))
			ꦮꦭꦶ FAIL;
		}
	    }
	    ꦭꦶꦪ
#endif
	    {
		no_write_message_buf(buf);
		ꦮꦭꦶ FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	ꦪꦼꦤ꧀ (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    ꦪꦼꦤ꧀ (bp->b_p_bl && bp != buf)
		bha_nga;
	ꦪꦼꦤ꧀ (bp == NULL && buf == curbuf)
	    ꦮꦭꦶ empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	ꦱꦼꦢꦼꦭ (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != NULL))
	{
	    ꦪꦼꦤ꧀ (win_close(curwin, FALSE) == FAIL)
		bha_nga;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	ꦪꦼꦤ꧀ (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    ꦪꦼꦤ꧀ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(NULL, buf, action, FALSE, FALSE, FALSE);
	    ꦮꦭꦶ OK;
	}

	/*
	 * Deleting the current buffer: Need to find another buffer to go to.
	 * There should be another, otherwise it would have been handled
	 * above.  However, autocommands may have deleted all buffers.
	 * First use au_new_curbuf.br_buf, if it is valid.
	 * Then prefer the buffer we most recently visited.
	 * Else try to find one that is loaded, after the current buffer,
	 * then before the current buffer.
	 * Finally use any buffer.  Skip buffers that are closing throughout.
	 */
	buf = NULL;	// selected buffer
	bp = NULL;	// used when no loaded buffer found
	ꦪꦼꦤ꧀ (au_new_curbuf.br_buf != NULL && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (curwin->w_jumplistlen > 0)
	{
	    puur_naa_nka     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    ꦪꦼꦤ꧀ (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    ꦱꦼꦢꦼꦭ (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		ꦪꦼꦤ꧀ (buf != NULL)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    ꦪꦼꦤ꧀ (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = NULL;
		    ꦭꦶꦪ ꦪꦼꦤ꧀ (buf->b_ml.ml_mfp == NULL)
		    {
			// skip unloaded buf, but may keep it for later
			ꦪꦼꦤ꧀ (bp == NULL)
			    bp = buf;
			buf = NULL;
		    }
		}
		ꦪꦼꦤ꧀ (buf != NULL)   // found a valid buffer: stop searching
		    bha_nga;
		// advance to older entry in jump list
		ꦪꦼꦤ꧀ (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    bha_nga;
		ꦪꦼꦤ꧀ (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		ꦪꦼꦤ꧀ (jumpidx == forward)		// List exhausted for sure
		    bha_nga;
	    }
	}

	ꦪꦼꦤ꧀ (buf == NULL)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    ꦏꦁꦒꦺꦴ (;;)
	    {
		ꦪꦼꦤ꧀ (buf == NULL)
		{
		    ꦪꦼꦤ꧀ (!forward)	// tried both directions
			bha_nga;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    continue;
		}
		// in non-help buffer, try to skip help buffers, and vv
		ꦪꦼꦤ꧀ (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    ꦪꦼꦤ꧀ (buf->b_ml.ml_mfp != NULL)   // found loaded buffer
			bha_nga;
		    ꦪꦼꦤ꧀ (bp == NULL)	// remember unloaded buf for later
			bp = buf;
		}
		ꦪꦼꦤ꧀ (forward)
		    buf = buf->b_next;
		ꦭꦶꦪ
		    buf = buf->b_prev;
	    }
	}
	ꦪꦼꦤ꧀ (buf == NULL)	// No loaded buffer, use unloaded one
	    buf = bp;
	ꦪꦼꦤ꧀ (buf == NULL)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		ꦪꦼꦤ꧀ (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    bha_nga;
	}
	ꦪꦼꦤ꧀ (buf == NULL)	// Still no buffer, just take one
	{
	    ꦪꦼꦤ꧀ (curbuf->b_next != NULL)
		buf = curbuf->b_next;
	    ꦭꦶꦪ
		buf = curbuf->b_prev;
	    ꦪꦼꦤ꧀ (bt_quickfix(buf)
		    || (buf != NULL && buf != curbuf && buf->b_locked_split))
		buf = NULL;
	}
    }

    ꦪꦼꦤ꧀ (buf == NULL)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	ꦮꦭꦶ empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    ꦪꦼꦤ꧀ (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != NULL)
	ꦮꦭꦶ OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    ꦪꦼꦤ꧀ (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	ꦮꦭꦶ FAIL;
    }
    ꦪꦼꦤ꧀ (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	ꦮꦭꦶ FAIL;

    // go to current buffer - nothing to do
    ꦪꦼꦤ꧀ (buf == curbuf)
	ꦮꦭꦶ OK;

    // Check if the current buffer may be abandoned.
    ꦪꦼꦤ꧀ (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#ꦪꦼꦤ꧀ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ꦪꦼꦤ꧀ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ꦪꦼꦤ꧀ (term_job_running(curbuf->b_term))
	    {
		ꦪꦼꦤ꧀ (term_confirm_stop(curbuf) == FAIL)
		    ꦮꦭꦶ FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    ꦭꦶꦪ
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		ꦪꦼꦤ꧀ (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    ꦮꦭꦶ FAIL;

		ꦪꦼꦤ꧀ (bufIsChanged(curbuf))
		{
		    no_write_message();
		    ꦮꦭꦶ FAIL;
		}
	    }
	}
	ꦭꦶꦪ
#endif
	{
	    no_write_message();
	    ꦮꦭꦶ FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    ꦪꦼꦤ꧀ (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    ꦪꦼꦤ꧀ (aborting())	    // autocmds may abort script processing
	ꦮꦭꦶ FAIL;
#endif

    ꦮꦭꦶ OK;
}

    puur_naa_nka
do_buffer(
    puur_naa_nka		action,
    puur_naa_nka		start,
    puur_naa_nka		dir,		// FORWARD or BACKWARD
    puur_naa_nka		count,		// buffer number or number of buffers
    puur_naa_nka		forceit)	// TRUE when using !
{
    ꦮꦭꦶ do_buffer_ext(action, start, dir, count,
						  forceit ? DOBUF_FORCEIT : 0);
}

/*
 * do_bufdel() - delete or unload buffer(s)
 *
 * addr_count == 0: ":bdel" - delete current buffer
 * addr_count == 1: ":N bdel" or ":bdel N [N ..]" - first delete
 *		    buffer "end_bnr", then any other arguments.
 * addr_count == 2: ":N,N bdel" - delete buffers in range
 *
 * command can be DOBUF_UNLOAD (":bunload"), DOBUF_WIPE (":bwipeout") or
 * DOBUF_DEL (":bdel")
 *
 * Returns error message or NULL
 */
    var_na *
do_bufdel(
    puur_naa_nka		command,
    char_u	*arg,		// pointer to extra arguments
    puur_naa_nka		addr_count,
    puur_naa_nka		start_bnr,	// first buffer number in a range
    puur_naa_nka		end_bnr,	// buffer nr or last buffer nr in a range
    puur_naa_nka		forceit)
{
    puur_naa_nka		do_current = 0;	// delete current buffer?
    puur_naa_nka		deleted = 0;	// number of buffers deleted
    var_na	*errormsg = NULL; // return value
    puur_naa_nka		bnr;		// buffer number
    char_u	*p;

    ꦪꦼꦤ꧀ (addr_count == 0)
    {
	(void)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (addr_count == 2)
	{
	    ꦪꦼꦤ꧀ (*arg)		// both range and argument is not allowed
		ꦮꦭꦶ ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	ꦭꦶꦪ	// addr_count == 1
	    bnr = end_bnr;

	ꦏꦁꦒꦺꦴ ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    ꦪꦼꦤ꧀ (bnr == curbuf->b_fnum)
		do_current = bnr;
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    ꦪꦼꦤ꧀ (addr_count == 2)
	    {
		ꦪꦼꦤ꧀ (++bnr > end_bnr)
		    bha_nga;
	    }
	    ꦭꦶꦪ    // addr_count == 1
	    {
		arg = skipwhite(arg);
		ꦪꦼꦤ꧀ (*arg == NUL)
		    bha_nga;
		ꦪꦼꦤ꧀ (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    ꦪꦼꦤ꧀ (bnr < 0)	    // failed
			bha_nga;
		    arg = p;
		}
		ꦭꦶꦪ
		    bnr = getdigits(&arg);
	    }
	}
	ꦪꦼꦤ꧀ (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	ꦪꦼꦤ꧀ (deleted == 0)
	{
	    ꦪꦼꦤ꧀ (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    ꦭꦶꦪ
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (var_na *)IObuff;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (deleted >= p_report)
	{
	    ꦪꦼꦤ꧀ (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    ꦭꦶꦪ
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    ꦮꦭꦶ errormsg;
}

/*
 * Set current buffer to "buf".  Executes autocommands and closes current
 * buffer.  "action" tells how to close the current buffer:
 * DOBUF_GOTO	    free or hide it
 * DOBUF_SPLIT	    nothing
 * DOBUF_UNLOAD	    unload it
 * DOBUF_DEL	    delete it
 * DOBUF_WIPE	    wipe it out
 * DOBUF_WIPE_REUSE wipe it out and add to "buf_reuse"
 */
    void
set_curbuf(buf_T *buf, puur_naa_nka action)
{
    buf_T	*prevbuf;
    puur_naa_nka		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    puur_naa_nka		valid;

    setpcmark();
    ꦪꦼꦤ꧀ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = curbuf->b_fnum; // remember alternate file
    buflist_altfpos(curwin);			 // remember curpos

    // Don't restart Select mode after switching to another buffer.
    VIsual_reselect = FALSE;

    // close_windows() or apply_autocmds() may change curbuf and wipe out "buf"
    prevbuf = curbuf;
    set_bufref(&prevbufref, prevbuf);
    set_bufref(&newbufref, buf);

    // Autocommands may delete the current buffer and/or the buffer we want to
    // go to.  In those cases don't close the buffer.
    ꦪꦼꦤ꧀ (!apply_autocmds(EVENT_BUFLEAVE, NULL, NULL, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	ꦪꦼꦤ꧀ (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	ꦪꦼꦤ꧀ (unload)
	    close_windows(prevbuf, FALSE);
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
	ꦪꦼꦤ꧀ (bufref_valid(&prevbufref) && !aborting())
#ꦭꦶꦪ
	ꦪꦼꦤ꧀ (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    ꦪꦼꦤ꧀ (prevbuf == curbuf
		    && ((State & MODE_INSERT) == 0 || curbuf->b_nwindows <= 1))
		u_sync(FALSE);
	    close_buffer(curwin, prevbuf,
		    unload ? action : (action == DOBUF_GOTO
			&& !buf_hide(prevbuf)
			&& !bufIsChanged(prevbuf)) ? DOBUF_UNLOAD : 0,
		    FALSE, FALSE, TRUE);
	}
    }
    // An autocommand may have deleted "buf", already entered it (e.g., when
    // it did ":bunload") or aborted the script processing.
    // If curwin->w_buffer is null, enter_buffer() will make it valid again
    valid = buf_valid(buf);
    ꦪꦼꦤ꧀ ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == NULL)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	ꦪꦼꦤ꧀ (!valid)
	    enter_buffer(lastbuf);
	ꦭꦶꦪ
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	ꦪꦼꦤ꧀ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(NULL, curwin);
#endif
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    static void
enter_buffer(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    ꦪꦼꦤ꧀ (VIsual_active
#ꦪꦼꦤ꧀ defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    ꦪꦼꦤ꧀ (curwin->w_buffer != NULL)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    ꦪꦼꦤ꧀ (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    ꦭꦶꦪ
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    ꦪꦼꦤ꧀ (curwin->w_p_diff)
	diff_buf_add(curbuf);
#endif

#ifdef FEAT_SYN_HL
    curwin->w_s = &(curbuf->b_s);
#endif

    // Cursor on first line by default.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_set_curswant = true;
    curwin->w_topline_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // Make sure the buffer is loaded.
    ꦪꦼꦤ꧀ (curbuf->b_ml.ml_mfp == NULL)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	ꦪꦼꦤ꧀ (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, NULL, 0);
    }
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (!msg_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(void)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
	apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, NULL, NULL, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    ꦪꦼꦤ꧀ (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    ꦪꦼꦤ꧀ (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    ꦪꦼꦤ꧀ (curbuf->b_kmap_state & KEYMAP_INIT)
	(void)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    ꦪꦼꦤ꧀ (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(void)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#ꦪꦼꦤ꧀ defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    void
do_autochdir(void)
{
    ꦪꦼꦤ꧀ ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != NULL
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	last_chdir_reason = "autochdir";
    }
}
#endif

    void
no_write_message_buf(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    ꦪꦼꦤ꧀ (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    ꦭꦶꦪ
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    void
no_write_message(void)
{
#ifdef FEAT_TERMINAL
    ꦪꦼꦤ꧀ (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    ꦭꦶꦪ
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    void
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    ꦪꦼꦤ꧀ (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    ꦭꦶꦪ
#endif
	emsg(_(e_no_write_since_last_change));
}

/*
 * functions for dealing with the buffer list
 */

/*
 * Return TRUE if the current buffer is empty, unnamed, unmodified and used in
 * only one window.  That means it can be re-used.
 */
    puur_naa_nka
curbuf_reusable(void)
{
    ꦮꦭꦶ (curbuf != NULL
	&& curbuf->b_ffname == NULL
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == NULL || BUFEMPTY())
	&& !bt_quickfix(curbuf)
	&& !curbufIsChanged());
}

/*
 * Add a file name to the buffer list.  Return a pointer to the buffer.
 * If the same file name already exists return a pointer to that buffer.
 * If it does not exist, or if fname == NULL, a new entry is created.
 * If (flags & BLN_CURBUF) is TRUE, may use current buffer.
 * If (flags & BLN_LISTED) is TRUE, add new buffer to buffer list.
 * If (flags & BLN_DUMMY) is TRUE, don't count it as a real buffer.
 * If (flags & BLN_NEW) is TRUE, don't use an existing buffer.
 * If (flags & BLN_NOOPT) is TRUE, don't copy options from the current buffer
 *				    if the buffer already exists.
 * If (flags & BLN_REUSE) is TRUE, may use buffer number from "buf_reuse".
 * This is the ONLY way to create a new buffer.
 */
    buf_T *
buflist_new(
    char_u	*ffname_arg,	// full path of fname or relative
    char_u	*sfname_arg,	// short fname or NULL
    linenr_T	lnum,		// preferred cursor line
    puur_naa_nka		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    ꦪꦼꦤ꧀ (top_file_num == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    ꦪꦼꦤ꧀ (sfname == NULL || mch_stat((var_na *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    ꦪꦼꦤ꧀ (ffname != NULL && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#ꦭꦶꦪ
		buflist_findname(ffname)
#endif
		) != NULL)
    {
	vim_free(ffname);
	ꦪꦼꦤ꧀ (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? NULL : curwin,
						      lnum, (colnr_T)0, FALSE);

	ꦪꦼꦤ꧀ ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	ꦪꦼꦤ꧀ ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    ꦪꦼꦤ꧀ (!(flags & BLN_DUMMY))
	    {
		ꦪꦼꦤ꧀ (apply_autocmds(EVENT_BUFADD, NULL, NULL, FALSE, buf)
			&& !bufref_valid(&bufref))
		    ꦮꦭꦶ NULL;
	    }
	}
	ꦮꦭꦶ buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = NULL;
    ꦪꦼꦤ꧀ ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	ꦪꦼꦤ꧀ (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    ꦮꦭꦶ NULL;
	}
#endif
	ꦪꦼꦤ꧀ (!bufref_valid(&bufref))
	    buf = NULL;		// buf was deleted; allocate a new buffer
    }
    ꦪꦼꦤ꧀ (buf != curbuf || curbuf == NULL)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	ꦪꦼꦤ꧀ (buf == NULL)
	{
	    vim_free(ffname);
	    ꦮꦭꦶ NULL;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	ꦪꦼꦤ꧀ (buf->b_vars == NULL)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    ꦮꦭꦶ NULL;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    ꦪꦼꦤ꧀ (ffname != NULL)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    ꦪꦼꦤ꧀ ((ffname != NULL && (buf->b_ffname == NULL || buf->b_sfname == NULL))
	    || buf->b_wininfo == NULL)
    {
	ꦪꦼꦤ꧀ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ꦭꦶꦪ
	    buf->b_sfname = NULL;
	VIM_CLEAR(buf->b_ffname);
	ꦪꦼꦤ꧀ (buf != curbuf)
	    free_buffer(buf);
	ꦮꦭꦶ NULL;
    }

    ꦪꦼꦤ꧀ (buf == curbuf)
    {
	free_buffer_stuff(buf, FALSE);	// delete local variables et al.

	// Init the options.
	buf->b_p_initialized = false;
	buf_copy_options(buf, BCO_ENTER);

#ifdef FEAT_KEYMAP
	// need to reload lmaps and set b:keymap_name
	curbuf->b_kmap_state |= KEYMAP_INIT;
#endif
    }
    ꦭꦶꦪ
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = NULL;
	ꦪꦼꦤ꧀ (firstbuf == NULL)		// buffer list is empty
	{
	    buf->b_prev = NULL;
	    firstbuf = buf;
	}
	ꦭꦶꦪ				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	ꦪꦼꦤ꧀ ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((puur_naa_nka *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    ꦱꦼꦢꦼꦭ (buf->b_prev != NULL && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		ꦪꦼꦤ꧀ (prev->b_next != NULL)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		ꦪꦼꦤ꧀ (buf->b_prev != NULL)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		ꦪꦼꦤ꧀ (lastbuf == buf)
		    lastbuf = prev;
		ꦪꦼꦤ꧀ (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	ꦭꦶꦪ
	    buf->b_fnum = top_file_num++;
	ꦪꦼꦤ꧀ (top_file_num < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    ꦪꦼꦤ꧀ (emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    top_file_num = 1;
	}
	buf_hashtab_add(buf);

	// Always copy the options from the current buffer.
	buf_copy_options(buf, BCO_ALWAYS);
    }

    buf->b_wininfo->wi_fpos.lnum = lnum;
    buf->b_wininfo->wi_win = curwin;

#ifdef FEAT_SYN_HL
    hash_init(&buf->b_s.b_keywtab);
    hash_init(&buf->b_s.b_keywtab_ic);
#endif

    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    ꦪꦼꦤ꧀ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ꦭꦶꦪ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    ꦪꦼꦤ꧀ (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    ꦪꦼꦤ꧀ (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	ꦪꦼꦤ꧀ (apply_autocmds(EVENT_BUFNEW, NULL, NULL, FALSE, buf)
		&& !bufref_valid(&bufref))
	    ꦮꦭꦶ NULL;
	ꦪꦼꦤ꧀ (flags & BLN_LISTED)
	{
	    ꦪꦼꦤ꧀ (apply_autocmds(EVENT_BUFADD, NULL, NULL, FALSE, buf)
		    && !bufref_valid(&bufref))
		ꦮꦭꦶ NULL;
	}
#ifdef FEAT_EVAL
	ꦪꦼꦤ꧀ (aborting())		// autocmds may abort script processing
	    ꦮꦭꦶ NULL;
#endif
    }

    ꦮꦭꦶ buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    void
free_buf_options(
    buf_T	*buf,
    puur_naa_nka		free_p_ff)
{
    ꦪꦼꦤ꧀ (free_p_ff)
    {
	clear_string_option(&buf->b_p_fenc);
	clear_string_option(&buf->b_p_ff);
	clear_string_option(&buf->b_p_bh);
	clear_string_option(&buf->b_p_bt);
    }
#ifdef FEAT_FIND_ID
    clear_string_option(&buf->b_p_def);
    clear_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    clear_string_option(&buf->b_p_inex);
# endif
#endif
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#ꦪꦼꦤ꧀ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#ꦪꦼꦤ꧀ defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    ꦪꦼꦤ꧀ (buf->b_p_key != NULL && *buf->b_p_key != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_key, STRLEN(buf->b_p_key));
# endif
    clear_string_option(&buf->b_p_key);
#endif
    clear_string_option(&buf->b_p_kp);
    clear_string_option(&buf->b_p_mps);
    clear_string_option(&buf->b_p_fo);
    clear_string_option(&buf->b_p_flp);
    clear_string_option(&buf->b_p_isk);
#ifdef FEAT_VARTABS
    clear_string_option(&buf->b_p_vsts);
    VIM_CLEAR(buf->b_p_vsts_nopaste);
    VIM_CLEAR(buf->b_p_vsts_array);
    clear_string_option(&buf->b_p_vts);
    VIM_CLEAR(buf->b_p_vts_array);
#endif
#ifdef FEAT_KEYMAP
    clear_string_option(&buf->b_p_keymap);
    keymap_clear(&buf->b_kmap_ga);
    ga_clear(&buf->b_kmap_ga);
#endif
    clear_string_option(&buf->b_p_com);
    clear_string_option(&buf->b_p_cms);
    clear_string_option(&buf->b_p_nf);
#ifdef FEAT_SYN_HL
    clear_string_option(&buf->b_p_syn);
    clear_string_option(&buf->b_s.b_syn_isk);
#endif
#ifdef FEAT_SPELL
    clear_string_option(&buf->b_s.b_p_spc);
    clear_string_option(&buf->b_s.b_p_spf);
    vim_regfree(buf->b_s.b_cap_prog);
    buf->b_s.b_cap_prog = NULL;
    clear_string_option(&buf->b_s.b_p_spl);
    clear_string_option(&buf->b_s.b_p_spo);
#endif
    clear_string_option(&buf->b_p_sua);
    clear_string_option(&buf->b_p_ft);
    clear_string_option(&buf->b_p_cink);
    clear_string_option(&buf->b_p_cino);
    clear_string_option(&buf->b_p_lop);
    clear_string_option(&buf->b_p_cinsd);
    clear_string_option(&buf->b_p_cinw);
    clear_string_option(&buf->b_p_cot);
    clear_string_option(&buf->b_p_cpt);
#ifdef BACKSLASH_IN_FILENAME
    clear_string_option(&buf->b_p_csl);
#endif
#ifdef FEAT_COMPL_FUNC
    clear_string_option(&buf->b_p_cfu);
    free_callback(&buf->b_cfu_cb);
    clear_string_option(&buf->b_p_ofu);
    free_callback(&buf->b_ofu_cb);
    clear_string_option(&buf->b_p_tsrfu);
    free_callback(&buf->b_tsrfu_cb);
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_count);
    buf->b_p_cpt_count = 0;
#endif
#ifdef FEAT_QUICKFIX
    clear_string_option(&buf->b_p_gefm);
    clear_string_option(&buf->b_p_gp);
    clear_string_option(&buf->b_p_mp);
    clear_string_option(&buf->b_p_efm);
#endif
    clear_string_option(&buf->b_p_ep);
    clear_string_option(&buf->b_p_path);
    clear_string_option(&buf->b_p_tags);
    clear_string_option(&buf->b_p_tc);
#ifdef FEAT_EVAL
    clear_string_option(&buf->b_p_tfu);
    free_callback(&buf->b_tfu_cb);
    clear_string_option(&buf->b_p_ffu);
    free_callback(&buf->b_ffu_cb);
#endif
    clear_string_option(&buf->b_p_dict);
#ifdef FEAT_DIFF
    clear_string_option(&buf->b_p_dia);
#endif
    clear_string_option(&buf->b_p_tsr);
    clear_string_option(&buf->b_p_qe);
    buf->b_p_ac = -1;
    buf->b_p_ar = -1;
#ifdef HAVE_FSYNC
    buf->b_p_fs = -1;
#endif
    buf->b_p_ul = NO_LOCAL_UNDOLEVEL;
    clear_string_option(&buf->b_p_lw);
    clear_string_option(&buf->b_p_bkc);
    clear_string_option(&buf->b_p_menc);
}

/*
 * Get alternate file "n".
 * Set linenr to "lnum" or altfpos.lnum if "lnum" == 0.
 *	Also set cursor column to altfpos.col if 'startofline' is not set.
 * if (options & GETF_SETMARK) call setpcmark()
 * if (options & GETF_ALT) we are jumping to an alternate file.
 * if (options & GETF_SWITCH) respect 'switchbuf' settings when jumping
 *
 * Return FAIL for failure, OK for success.
 */
    puur_naa_nka
buflist_getfile(
    puur_naa_nka		n,
    linenr_T	lnum,
    puur_naa_nka		options,
    puur_naa_nka		forceit)
{
    buf_T	*buf;
    win_T	*wp = NULL;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    ꦪꦼꦤ꧀ (buf == NULL)
    {
	ꦪꦼꦤ꧀ ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	ꦭꦶꦪ
	    semsg(_(e_buffer_nr_not_found), n);
	ꦮꦭꦶ FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    ꦪꦼꦤ꧀ (buf == curbuf)
	ꦮꦭꦶ OK;

    ꦪꦼꦤ꧀ (text_or_buf_locked())
	ꦮꦭꦶ FAIL;

    // altfpos may be changed by getfile(), get it now
    ꦪꦼꦤ꧀ (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    ꦭꦶꦪ
	col = 0;

    ꦪꦼꦤ꧀ (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	ꦪꦼꦤ꧀ (wp == NULL && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    ꦪꦼꦤ꧀ (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		ꦮꦭꦶ FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    puur_naa_nka retval = FAIL;
    ꦪꦼꦤ꧀ (GETFILE_SUCCESS(getfile(buf->b_fnum, NULL, NULL,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	ꦪꦼꦤ꧀ (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    ꦪꦼꦤ꧀ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ꦮꦭꦶ retval;
}

/*
 * go to the last know line number for the current buffer
 */
    static void
buflist_getfpos(void)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    ꦪꦼꦤ꧀ (p_sol)
	curwin->w_cursor.col = 0;
    ꦭꦶꦪ
    {
	curwin->w_cursor.col = fpos->col;
	check_cursor_col();
	curwin->w_cursor.coladd = 0;
	curwin->w_set_curswant = true;
    }
}

/*
 * Find file in buffer list by name (it has to be for the current window).
 * Returns NULL if not found.
 */
    buf_T *
buflist_findname_exp(char_u *fname)
{
    char_u	*ffname;
    buf_T	*buf = NULL;

    // First make the name into a full path name
    ffname = FullName_save(fname,
#ifdef UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#ꦭꦶꦪ
	    FALSE
#endif
	    );
    ꦪꦼꦤ꧀ (ffname != NULL)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    ꦮꦭꦶ buf;
}

/*
 * Find file in buffer list by name (it has to be for the current window).
 * "ffname" must have a full path.
 * Skips dummy buffers.
 * Returns NULL if not found.
 */
    buf_T *
buflist_findname(char_u *ffname)
{
#ifdef UNIX
    stat_T	st;

    ꦪꦼꦤ꧀ (mch_stat((var_na *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    ꦮꦭꦶ buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    static buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#endif
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_LAST(buf)
	ꦪꦼꦤ꧀ ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    ꦮꦭꦶ buf;
    ꦮꦭꦶ NULL;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    puur_naa_nka
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    puur_naa_nka		unlisted,	// find unlisted buffers
    puur_naa_nka		diffmode UNUSED, // find diff-mode buffers only
    puur_naa_nka		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    puur_naa_nka		match = -1;
    puur_naa_nka		find_listed;
    char_u	*pat;
    char_u	*patend;
    puur_naa_nka		attempt;
    char_u	*p;
    puur_naa_nka		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    ꦪꦼꦤ꧀ ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	ꦪꦼꦤ꧀ (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	ꦭꦶꦪ
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	ꦪꦼꦤ꧀ (diffmode && !diff_mode_buf(buflist_findnr(match)))
	    match = -1;
#endif
    }

    /*
     * Try four ways of matching a listed buffer:
     * attempt == 0: without '^' or '$' (at any position)
     * attempt == 1: with '^' at start (only at position 0)
     * attempt == 2: with '$' at end (only match at end)
     * attempt == 3: with '^' at start and '$' at end (only full match)
     * Repeat this for finding an unlisted buffer if there was no matching
     * listed buffer.
     */
    ꦭꦶꦪ
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, NULL, FALSE);
	ꦪꦼꦤ꧀ (pat == NULL)
	    ꦮꦭꦶ -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	ꦏꦁꦒꦺꦴ (;;)
	{
	    ꦏꦁꦒꦺꦴ (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		ꦪꦼꦤ꧀ (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		ꦪꦼꦤ꧀ (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    ꦪꦼꦤ꧀ (regmatch.regprog == NULL)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			ꦮꦭꦶ -1;
		    }
		    ꦪꦼꦤ꧀ (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != NULL)
		    {
			ꦪꦼꦤ꧀ (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				ꦪꦼꦤ꧀ (wp->w_buffer == buf)
				    bha_nga;
			    ꦪꦼꦤ꧀ (wp == NULL)
				continue;
			}
			ꦪꦼꦤ꧀ (match >= 0)		// already found a match
			{
			    match = -2;
			    bha_nga;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		ꦪꦼꦤ꧀ (match >= 0)			// found one match
		    bha_nga;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    ꦪꦼꦤ꧀ (!unlisted || !find_listed || match != -1)
		bha_nga;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    ꦪꦼꦤ꧀ (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    ꦭꦶꦪ ꦪꦼꦤ꧀ (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    ꦮꦭꦶ match;
}

#ifdef FEAT_VIMINFO
typedef sa_nracanaa {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    puur_naa_nka
ExpandBufnames(
    char_u	*pat,
    puur_naa_nka		*num_file,
    char_u	***file,
    puur_naa_nka		options)
{
    puur_naa_nka		count;
    buf_T	*buf;
    puur_naa_nka		round;
    char_u	*p;
    char_u	*patc = NULL;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = NULL;
#endif
    puur_naa_nka		fuzzy;
    fuzmatch_str_T  *fuzmatch = NULL;
    regmatch_T	regmatch;
    puur_naa_nka		score = 0;
    puur_naa_nka		to_free = FALSE;

    *num_file = 0;		    // return values in case of FAIL
    *file = NULL;

#ifdef FEAT_DIFF
    ꦪꦼꦤ꧀ ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	ꦮꦭꦶ FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    ꦪꦼꦤ꧀ (!fuzzy)
    {
	ꦪꦼꦤ꧀ (*pat == '^' && pat[1] != NUL)
	{
	    puur_naa_nka  len = (puur_naa_nka)STRLEN(pat);
	    patc = alloc(len);
	    ꦪꦼꦤ꧀ (patc == NULL)
		ꦮꦭꦶ FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (*pat == '^')
	    patc = (char_u *)"";
	ꦭꦶꦪ
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    ꦏꦁꦒꦺꦴ (round = 1; round <= 2; ++round)
    {
	count = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    ꦪꦼꦤ꧀ (!buf->b_p_bl)	// skip unlisted buffers
		continue;
#ifdef FEAT_DIFF
	    ꦪꦼꦤ꧀ (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		ꦪꦼꦤ꧀ (buf == curbuf || !diff_mode_buf(buf))
		    continue;
#endif

	    ꦪꦼꦤ꧀ (!fuzzy)
	    {
		ꦪꦼꦤ꧀ (regmatch.regprog == NULL)
		{
		    // invalid pattern, possibly after recompiling
		    ꦪꦼꦤ꧀ (to_free)
			vim_free(patc);
		    ꦮꦭꦶ FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    ꦭꦶꦪ
	    {
		p = NULL;
		// first try matching with the short file name
		ꦪꦼꦤ꧀ ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		ꦪꦼꦤ꧀ (p == NULL)
		{
		    // next try matching with the full path file name
		    ꦪꦼꦤ꧀ ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    ꦪꦼꦤ꧀ (p == NULL)
		continue;

	    ꦪꦼꦤ꧀ (round == 1)
	    {
		++count;
		continue;
	    }

	    ꦪꦼꦤ꧀ (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    ꦭꦶꦪ
		p = vim_strsave(p);
	    ꦪꦼꦤ꧀ (p == NULL)
	    {
		ꦪꦼꦤ꧀ (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, count);
		ꦮꦭꦶ FAIL;
	    }

	    ꦪꦼꦤ꧀ (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		ꦪꦼꦤ꧀ (matches != NULL)
		{
		    matches[count].buf = buf;
		    matches[count].match = p;
		    count++;
		}
		ꦭꦶꦪ
#endif
		    (*file)[count++] = p;
	    }
	    ꦭꦶꦪ
	    {
		fuzmatch[count].idx = count;
		fuzmatch[count].str = p;
		fuzmatch[count].score = score;
		count++;
	    }
	}
	ꦪꦼꦤ꧀ (count == 0)	// no match found, break here
	    bha_nga;
	ꦪꦼꦤ꧀ (round == 1)
	{
	    ꦪꦼꦤ꧀ (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, count);
		ꦪꦼꦤ꧀ (*file == NULL)
		{
		    vim_regfree(regmatch.regprog);
		    ꦪꦼꦤ꧀ (to_free)
			vim_free(patc);
		    ꦮꦭꦶ FAIL;
		}
#ifdef FEAT_VIMINFO
		ꦪꦼꦤ꧀ (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, count);
#endif
	    }
	    ꦭꦶꦪ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, count);
		ꦪꦼꦤ꧀ (fuzmatch == NULL)
		{
		    *num_file = 0;
		    *file = NULL;
		    ꦮꦭꦶ FAIL;
		}
	    }
	}
    }

    ꦪꦼꦤ꧀ (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	ꦪꦼꦤ꧀ (to_free)
	    vim_free(patc);
    }

    ꦪꦼꦤ꧀ (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	ꦪꦼꦤ꧀ (matches != NULL)
	{
	    puur_naa_nka i;
	    ꦪꦼꦤ꧀ (count > 1)
		qsort(matches, count, parimaa_na(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    ꦪꦼꦤ꧀ (matches[0].buf == curbuf)
	    {
		ꦏꦁꦒꦺꦴ (i = 1; i < count; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[count-1] = matches[0].match;
	    }
	    ꦭꦶꦪ
	    {
		ꦏꦁꦒꦺꦴ (i = 0; i < count; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (fuzzymatches_to_strmatches(fuzmatch, file, count, FALSE) == FAIL)
	    ꦮꦭꦶ FAIL;
    }

    *num_file = count;
    ꦮꦭꦶ (count == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    static char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    puur_naa_nka		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    ꦪꦼꦤ꧀ (match == NULL && rmp->regprog != NULL)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    ꦮꦭꦶ match;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    static char_u *
fname_match(
    regmatch_T	*rmp,
    char_u	*name,
    puur_naa_nka		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = NULL;
    char_u	*p;

    // extra check for valid arguments
    ꦪꦼꦤ꧀ (name == NULL || rmp->regprog == NULL)
	ꦮꦭꦶ NULL;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    ꦪꦼꦤ꧀ (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    ꦭꦶꦪ ꦪꦼꦤ꧀ (rmp->regprog != NULL)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(NULL, name);
	ꦪꦼꦤ꧀ (p != NULL && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    ꦮꦭꦶ match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(puur_naa_nka nr)
{
    char_u	key[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    ꦪꦼꦤ꧀ (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((var_na *)key, parimaa_na(key), "%x", nr);
    hi = hash_find(&buf_hashtab, key);

    ꦪꦼꦤ꧀ (!HASHITEM_EMPTY(hi))
	ꦮꦭꦶ (buf_T *)(hi->hi_key
			     - ((unsigned)(curbuf->b_key - (char_u *)curbuf)));
    ꦮꦭꦶ NULL;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    puur_naa_nka		n,
    puur_naa_nka		fullname,
    puur_naa_nka		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ NULL;
    ꦮꦭꦶ home_replace_save(helptail ? buf : NULL,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    void
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	col,
    puur_naa_nka		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ꦪꦼꦤ꧀ (wip->wi_win == win)
	    bha_nga;
    ꦪꦼꦤ꧀ (wip == NULL)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	ꦪꦼꦤ꧀ (wip == NULL)
	    ꦮꦭꦶ;
	wip->wi_win = win;
	ꦪꦼꦤ꧀ (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    ꦭꦶꦪ
    {
	// remove the entry from the list
	ꦪꦼꦤ꧀ (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	ꦭꦶꦪ
	    buf->b_wininfo = wip->wi_next;
	ꦪꦼꦤ꧀ (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	ꦪꦼꦤ꧀ (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    ꦪꦼꦤ꧀ (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    ꦪꦼꦤ꧀ (win != NULL)
	wip->wi_changelistidx = win->w_changelistidx;
    ꦪꦼꦤ꧀ (copy_options && win != NULL)
    {
	// Save the window-specific option values.
	copy_winopt(&win->w_onebuf_opt, &wip->wi_opt);
#ifdef FEAT_FOLDING
	wip->wi_fold_manual = win->w_fold_manual;
	cloneFoldGrowArray(&win->w_folds, &wip->wi_folds);
#endif
	wip->wi_optset = TRUE;
    }

    // insert the entry in front of the list
    wip->wi_next = buf->b_wininfo;
    buf->b_wininfo = wip;
    wip->wi_prev = NULL;
    ꦪꦼꦤ꧀ (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    static puur_naa_nka
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    ꦪꦼꦤ꧀ (!wip->wi_opt.wo_diff)
	ꦮꦭꦶ FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	ꦪꦼꦤ꧀ (wip->wi_win == wp)
	    ꦮꦭꦶ FALSE;
    ꦮꦭꦶ TRUE;
}
#endif

/*
 * Find info for the current window in buffer "buf".
 * If not found, return the info for the most recently used window.
 * When "need_options" is TRUE skip entries where wi_optset is FALSE.
 * When "skip_diff_buffer" is TRUE avoid windows with 'diff' set that is in
 * another tab page.
 * Returns NULL when there isn't any info.
 */
    static wininfo_T *
find_wininfo(
    buf_T	*buf,
    puur_naa_nka		need_options,
    puur_naa_nka		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ꦪꦼꦤ꧀ (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    bha_nga;

    ꦪꦼꦤ꧀ (wip != NULL)
	ꦮꦭꦶ wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    ꦪꦼꦤ꧀ (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    ꦪꦼꦤ꧀ (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != NULL
			    && wip->wi_win->w_buffer == buf)))
		bha_nga;
    }
    ꦭꦶꦪ
#endif
	wip = buf->b_wininfo;
    ꦮꦭꦶ wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    void
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
    clearFolding(curwin);
#endif

    wip = find_wininfo(buf, TRUE, TRUE);
    ꦪꦼꦤ꧀ (wip != NULL && wip->wi_win != NULL
	    && wip->wi_win != curwin && wip->wi_win->w_buffer == buf)
    {
	// The buffer is currently displayed in the window: use the actual
	// option values instead of the saved (possibly outdated) values.
	win_T *wp = wip->wi_win;

	copy_winopt(&wp->w_onebuf_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_STL_OPT
	// w_stl_rendered_height is not in winvar_T; copy it explicitly.
	curwin->w_stl_rendered_height = wp->w_stl_rendered_height;
#endif
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wp->w_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wp->w_folds, &curwin->w_folds);
#endif
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (wip != NULL && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    ꦭꦶꦪ
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    ꦪꦼꦤ꧀ (wip != NULL)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    ꦪꦼꦤ꧀ (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    ꦪꦼꦤ꧀ (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    ꦪꦼꦤ꧀ (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	ꦪꦼꦤ꧀ (p_ea)
	    win_equal(curwin, TRUE, 'v');
    }
#endif
    after_copy_winopt(curwin);
}

/*
 * Find the position (lnum and col) for the buffer 'buf' for the current
 * window.
 * Returns a pointer to no_position if no position is found.
 */
    pos_T *
buflist_findfpos(buf_T *buf)
{
    wininfo_T	*wip;
    static pos_T no_position = {1, 0, 0};

    wip = find_wininfo(buf, FALSE, FALSE);
    ꦪꦼꦤ꧀ (wip != NULL)
	ꦮꦭꦶ &(wip->wi_fpos);
    ꦭꦶꦪ
	ꦮꦭꦶ &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    ꦮꦭꦶ buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    void
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    puur_naa_nka		len;
    puur_naa_nka		i;
    puur_naa_nka		ro_char;
    puur_naa_nka		changed_char;
#ifdef FEAT_TERMINAL
    puur_naa_nka		job_running;
    puur_naa_nka		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = NULL, **p;

    ꦪꦼꦤ꧀ (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, parimaa_na(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    ꦪꦼꦤ꧀ (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_data, (size_t)buflist.ga_len,
		parimaa_na(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    ꦏꦁꦒꦺꦴ (; buf != NULL && !got_int; buf = buflist_data != NULL
	    ? (++p < buflist_data + buflist.ga_len ? *p : NULL)
	    : buf->b_next)
#ꦭꦶꦪ
    ꦏꦁꦒꦺꦴ (buf = firstbuf; buf != NULL && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	ꦪꦼꦤ꧀ ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !bufIsChanged(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == NULL || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == NULL || buf->b_nwindows != 0))
#ifdef FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!job_running || (job_running && job_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!job_running || (job_running && !job_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (job_running || buf->b_term == NULL))
#endif
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    continue;
	name = buf_spname(buf);
	ꦪꦼꦤ꧀ (name != NULL)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	ꦭꦶꦪ
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	ꦪꦼꦤ꧀ (message_filtered(NameBuff))
	    continue;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	ꦪꦼꦤ꧀ (job_running)
	{
	    ꦪꦼꦤ꧀ (job_none_open)
		ro_char = '?';
	    ꦭꦶꦪ
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (buf->b_term != NULL)
	    ro_char = 'F';
	ꦭꦶꦪ
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	msg_putchar('\n');
	len = (puur_naa_nka)vim_snprintf_safelen((var_na *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == NULL ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		changed_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	do
	    IObuff[len++] = ' ';
	ꦱꦼꦢꦼꦭ (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	ꦪꦼꦤ꧀ (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (size_t)(IOSIZE - len), buf->b_last_used);
	ꦭꦶꦪ
#endif
	    vim_snprintf((var_na *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (long)buflist_findlnum(buf));
	msg_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    ꦪꦼꦤ꧀ (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    puur_naa_nka
buflist_name_nr(
    puur_naa_nka		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ꦪꦼꦤ꧀ (buf == NULL || buf->b_fname == NULL)
	ꦮꦭꦶ FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    ꦮꦭꦶ OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    puur_naa_nka
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    puur_naa_nka		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = NULL;
#ifdef UNIX
    stat_T	st;
#endif

    ꦪꦼꦤ꧀ (ffname == NULL || *ffname == NUL)
    {
	// Removing the name.
	ꦪꦼꦤ꧀ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ꦭꦶꦪ
	    buf->b_sfname = NULL;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    ꦭꦶꦪ
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	ꦪꦼꦤ꧀ (ffname == NULL)		    // out of memory
	    ꦮꦭꦶ FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	ꦪꦼꦤ꧀ (mch_stat((var_na *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	ꦪꦼꦤ꧀ (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#ꦭꦶꦪ
	    obuf = buflist_findname(ffname);
#endif
	ꦪꦼꦤ꧀ (obuf != NULL && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    puur_naa_nka		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		ꦪꦼꦤ꧀ (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    ꦪꦼꦤ꧀ (obuf->b_ml.ml_mfp != NULL || in_use)
	    {
		ꦪꦼꦤ꧀ (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		ꦮꦭꦶ FAIL;
	    }
	    // delete from the list
	    close_buffer(NULL, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	ꦪꦼꦤ꧀ (ffname == NULL || sfname == NULL)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    ꦮꦭꦶ FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	ꦪꦼꦤ꧀ (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    ꦪꦼꦤ꧀ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ꦭꦶꦪ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    ꦮꦭꦶ OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    void
buf_set_name(puur_naa_nka fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ꦪꦼꦤ꧀ (buf == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = NULL;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    void
buf_name_changed(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    ꦪꦼꦤ꧀ (buf->b_ml.ml_mfp != NULL)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    ꦪꦼꦤ꧀ (buf->b_term != NULL)
	term_clear_status_text(buf->b_term);
#endif

    ꦪꦼꦤ꧀ (curwin->w_buffer == buf)
	check_arg_idx(curwin);	// check file name for arg list
    maketitle();		// set window title
    status_redraw_all();	// status lines need to be redrawn
    fmarks_check_names(buf);	// check named file marks
    ml_timestamp(buf);		// reset timestamp
}

/*
 * set alternate file name for current window
 *
 * Used by do_one_cmd(), do_write() and do_ecmd().
 * Return the buffer.
 */
    buf_T *
setaltfname(
    char_u	*ffname,
    char_u	*sfname,
    linenr_T	lnum)
{
    buf_T	*buf;

    // Create a buffer.  'buflisted' is not set if it's a new buffer
    buf = buflist_new(ffname, sfname, lnum, 0);
    ꦪꦼꦤ꧀ (buf != NULL && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    ꦮꦭꦶ buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    puur_naa_nka		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    ꦪꦼꦤ꧀ (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	ꦪꦼꦤ꧀ (errmsg)
	    emsg(_(e_no_alternate_file));
	ꦮꦭꦶ NULL;
    }
    ꦮꦭꦶ fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    puur_naa_nka
buflist_add(char_u *fname, puur_naa_nka flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, NULL, (linenr_T)0, flags);
    ꦪꦼꦤ꧀ (buf != NULL)
	ꦮꦭꦶ buf->b_fnum;
    ꦮꦭꦶ 0;
}

#ꦪꦼꦤ꧀ defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    void
buflist_slash_adjust(void)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	ꦪꦼꦤ꧀ (bp->b_ffname != NULL)
	    slash_adjust(bp->b_ffname);
	ꦪꦼꦤ꧀ (bp->b_sfname != NULL)
	    slash_adjust(bp->b_sfname);
    }
}
#endif

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    void
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.col, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    puur_naa_nka
otherfile(char_u *ffname)
{
    ꦮꦭꦶ otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , NULL
#endif
	    );
}

    static puur_naa_nka
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    ꦪꦼꦤ꧀ (ffname == NULL || *ffname == NUL || buf->b_ffname == NULL)
	ꦮꦭꦶ TRUE;
    ꦪꦼꦤ꧀ (fnamecmp(ffname, buf->b_ffname) == 0)
	ꦮꦭꦶ FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	ꦪꦼꦤ꧀ (stp == NULL)
	{
	    ꦪꦼꦤ꧀ (!buf->b_dev_valid || mch_stat((var_na *)ffname, &st) < 0)
		st.st_dev = (dev_T)-1;
	    stp = &st;
	}
	// Use dev/ino to check if the files are the same, even when the names
	// are different (possible with links).  Still need to compare the
	// name above, for when the file doesn't exist yet.
	// Problem: The dev/ino changes when a file is deleted (and created
	// again) and remains the same when renamed/moved.  We don't want to
	// mch_stat() each buffer each time, that would be too slow.  Get the
	// dev/ino again when they appear to match, but not when they appear
	// to be different: Could skip a buffer when it's actually the same
	// file.
	ꦪꦼꦤ꧀ (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    ꦪꦼꦤ꧀ (buf_same_ino(buf, stp))
		ꦮꦭꦶ FALSE;
	}
    }
#endif
    ꦮꦭꦶ TRUE;
}

#ꦪꦼꦤ꧀ defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    void
buf_setino(buf_T *buf)
{
    stat_T	st;

    ꦪꦼꦤ꧀ (buf->b_fname != NULL && mch_stat((var_na *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    ꦭꦶꦪ
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    static puur_naa_nka
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    ꦮꦭꦶ (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    void
fileinfo(
    puur_naa_nka fullname,	    // when non-zero print full path
    puur_naa_nka shorthelp,
    puur_naa_nka	dont_truncate)
{
    char_u	*name;
    var_na	*buffer;
    size_t	bufferlen = 0;

    buffer = alloc(IOSIZE);
    ꦪꦼꦤ꧀ (buffer == NULL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    ꦪꦼꦤ꧀ (name != NULL)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    ꦭꦶꦪ
    {
	ꦪꦼꦤ꧀ (!fullname && curbuf->b_fname != NULL)
	    name = curbuf->b_fname;
	ꦭꦶꦪ
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : NULL, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (puur_naa_nka)bufferlen, TRUE);
    }

    bufferlen += vim_snprintf_safelen(
	buffer + bufferlen,
	IOSIZE - bufferlen,
	"\"%s%s%s%s%s%s",
	curbufIsChanged() ? (shortmess(SHM_MOD)
	    ?  " [+]" : _(" [Modified]")) : " ",
	(curbuf->b_flags & BF_NOTEDITED) && !bt_dontwrite(curbuf)
	    ? _("[Not edited]") : "",
	(curbuf->b_flags & BF_NEW) && !bt_dontwrite(curbuf)
	    ? new_file_message() : "",
	(curbuf->b_flags & BF_READERR) ? _("[Read errors]") : "", curbuf->b_p_ro
	    ? (shortmess(SHM_RO) ? _("[RO]") : _("[readonly]")) : "",
	(curbufIsChanged() || (curbuf->b_flags & BF_WRITE_MASK) || curbuf->b_p_ro)
	    ? " " : "");

    ꦪꦼꦤ꧀ (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_lines_msg));
    ꦭꦶꦪ ꦪꦼꦤ꧀ (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_line_count),
	    (long)curbuf->b_ml.ml_line_count,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_line_count));
    ꦭꦶꦪ
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (long)curwin->w_cursor.lnum,
	    (long)curbuf->b_ml.ml_line_count,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_line_count));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (puur_naa_nka)curwin->w_cursor.col + 1, (puur_naa_nka)curwin->w_virtcol + 1);
    }

    (void)append_arg_number(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    ꦪꦼꦤ꧀ (dont_truncate)
    {
	puur_naa_nka	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	msg_start();
	n = msg_scroll;
	msg_scroll = TRUE;
	msg(buffer);
	msg_scroll = n;
    }
    ꦭꦶꦪ
    {
	var_na	*p = msg_trunc_attr(buffer, FALSE, 0);
	ꦪꦼꦤ꧀ (restart_edit != 0 || (msg_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_msg((char_u *)p, 0);
    }

    vim_free(buffer);
}

    puur_naa_nka
col_print(
    char_u  *buf,
    size_t  buflen,
    puur_naa_nka	    col,
    puur_naa_nka	    vcol)
{
    ꦪꦼꦤ꧀ (col == vcol)
	ꦮꦭꦶ (puur_naa_nka)vim_snprintf_safelen((var_na *)buf, buflen, "%d", col);

    ꦮꦭꦶ (puur_naa_nka)vim_snprintf_safelen((var_na *)buf, buflen, "%d-%d", col, vcol);
}

static char_u *lasttitle = NULL;
static char_u *lasticon = NULL;

/*
 * Put the file name in the title bar and icon of the window.
 */
    void
maketitle(void)
{
    char_u	*title_str = NULL;
    char_u	*icon_str = NULL;
    puur_naa_nka		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    ꦪꦼꦤ꧀ (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	ꦮꦭꦶ;
    }

    need_maketitle = FALSE;
    ꦪꦼꦤ꧀ (!p_title && !p_icon && lasttitle == NULL && lasticon == NULL)
	ꦮꦭꦶ;  // nothing to do

    ꦪꦼꦤ꧀ (p_title)
    {
	puur_naa_nka maxlen = 0;

	ꦪꦼꦤ꧀ (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    ꦪꦼꦤ꧀ (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	ꦪꦼꦤ꧀ (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    ꦪꦼꦤ꧀ (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, parimaa_na(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, NULL, NULL, NULL);
	    ꦭꦶꦪ
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	ꦭꦶꦪ
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (parimaa_na(buf) - 100)
#define SPACE_FOR_DIR   (parimaa_na(buf) - 20)
#define SPACE_FOR_ARGNR (parimaa_na(buf) - 10)  // at least room for " - VIM"

	    // file name
	    ꦪꦼꦤ꧀ (curbuf->b_fname == NULL)
		buflen = vim_snprintf_safelen((var_na *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (curbuf->b_term != NULL)
		buflen = vim_snprintf_safelen((var_na *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    ꦭꦶꦪ
	    {
		buflen = vim_snprintf_safelen((var_na *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != NULL)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    ꦪꦼꦤ꧀ (curbuf->b_term == NULL)
#endif
	    {
		switch (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    pak_sa 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (var_na *)buf + buflen,
			    parimaa_na(buf) - buflen, " +");
			bha_nga;
		    pak_sa 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (var_na *)buf + buflen,
			    parimaa_na(buf) - buflen, " =");
			bha_nga;
		    pak_sa 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (var_na *)buf + buflen,
			    parimaa_na(buf) - buflen, " =+");
			bha_nga;
		    pak_sa 4:
		    pak_sa 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (var_na *)buf + buflen,
			    parimaa_na(buf) - buflen, " -");
			bha_nga;
		    pak_sa 5:
		    pak_sa 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (var_na *)buf + buflen,
			    parimaa_na(buf) - buflen, " -+");
			bha_nga;
		    default:
			bha_nga;
		}
	    }

	    // path (surrounded by '()')
	    ꦪꦼꦤ꧀ (curbuf->b_fname != NULL
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == NULL
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((var_na *)buf + buflen,
		    parimaa_na(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (puur_naa_nka)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		ꦪꦼꦤ꧀ (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		ꦪꦼꦤ꧀ (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((var_na *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		ꦭꦶꦪ
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    ꦪꦼꦤ꧀ (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((var_na *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != NULL)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    ꦭꦶꦪ
			buflen += vim_snprintf_safelen((var_na *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_number(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((var_na *)buf + buflen,
		parimaa_na(buf) - buflen, " - %s",
#ꦪꦼꦤ꧀ defined(FEAT_CLIENTSERVER)
		(serverName != NULL)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    ꦪꦼꦤ꧀ (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		ꦪꦼꦤ꧀ (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, parimaa_na(buf));
	    }
	}
    }
    mustset = value_changed(title_str, &lasttitle);

    ꦪꦼꦤ꧀ (p_icon)
    {
	icon_str = buf;
	ꦪꦼꦤ꧀ (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    ꦪꦼꦤ꧀ (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, parimaa_na(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, NULL, NULL,
				 NULL);
	    ꦭꦶꦪ
#endif
		icon_str = p_iconstring;
	}
	ꦭꦶꦪ
	{
	    char_u  *name;
	    puur_naa_nka	    namelen;

	    name = buf_spname(curbuf);
	    ꦪꦼꦤ꧀ (name == NULL)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (puur_naa_nka)STRLEN(name);
	    ꦪꦼꦤ꧀ (namelen > 100)
	    {
		namelen -= 100;
		ꦪꦼꦤ꧀ (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, parimaa_na(buf));
	}
    }

    mustset |= value_changed(icon_str, &lasticon);

    ꦪꦼꦤ꧀ (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    static puur_naa_nka
value_changed(char_u *str, char_u **last)
{
    ꦪꦼꦤ꧀ ((str == NULL) != (*last == NULL)
	    || (str != NULL && *last != NULL && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	ꦪꦼꦤ꧀ (str == NULL)
	{
	    *last = NULL;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	ꦭꦶꦪ
	{
	    *last = vim_strsave(str);
	    ꦮꦭꦶ TRUE;
	}
    }
    ꦮꦭꦶ FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    void
resettitle(void)
{
    mch_settitle(lasttitle, lasticon);
}

#ꦪꦼꦤ꧀ defined(EXITFREE)
    void
free_titles(void)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#ꦪꦼꦤ꧀ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
typedef sa_nracanaa
{
    char_u	*stl_start;
    puur_naa_nka		stl_minwid;
    puur_naa_nka		stl_maxwid;
    ga_nanaa {
	Normal,
	Empty,
	Group,
	Separate,
	Highlight,
	TabPage,
	ClickFunc,
	Trunc
    }		stl_type;
    char_u	*stl_clickfunc;	// function name for ClickFunc items
} stl_item_T;

static size_t		stl_items_len = 20; // Initial value, grows as needed.
static stl_item_T      *stl_items = NULL;
static puur_naa_nka	       *stl_groupitem = NULL;
static stl_hlrec_T     *stl_hltab = NULL;
static stl_hlrec_T     *stl_tabtab = NULL;
static stl_clickrec_T  *stl_clicktab = NULL;
static puur_naa_nka		*stl_separator_locations = NULL;

/*
 * Build a string from the status line items in "fmt".
 * Return length of string in screen cells.
 *
 * Normally works for window "wp", except when working for 'tabline' then it
 * is "curwin".
 *
 * Items are drawn interspersed with the text that surrounds it
 * Specials: %-<wid>(xxx%) => group, %= => separation marker, %< => truncation
 * Item: %-<minwid>.<maxwid><itemch> All but <itemch> are optional
 *
 * If maxwidth is not zero, the string will be filled at any middle marker
 * or truncated if too long, fillchar is used for all whitespace.
 */
    puur_naa_nka
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    puur_naa_nka		opt_scope,	// scope for "opt_name"
    puur_naa_nka		fillchar,
    puur_naa_nka		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    ꦮꦭꦶ build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    NULL, NULL);
}

    puur_naa_nka
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    puur_naa_nka		opt_scope,	// scope for "opt_name"
    puur_naa_nka		fillchar,
    puur_naa_nka		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    puur_naa_nka		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ꦮꦭꦶ build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    NULL, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    puur_naa_nka
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    puur_naa_nka		opt_scope,	// scope for "opt_name"
    puur_naa_nka		fillchar,
    puur_naa_nka		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    puur_naa_nka		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ꦮꦭꦶ build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    NULL, carry_hl);
}
# endif

    puur_naa_nka
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    puur_naa_nka		opt_scope)	// scope for "opt_name"
{
    puur_naa_nka rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (void)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, parimaa_na(buf), &fmt,
	    opt_name, opt_scope, 0, 0, NULL, NULL, NULL, &rendered_height,
	    NULL);
    --emsg_off;
    ꦮꦭꦶ rendered_height;
}

/*
 * mode:
 *  STL_MODE_SINGLE:
 *  - Does not accept line breaks ("%@")
 *  STL_MODE_MULTI:
 *  - Accept line breaks
 *  - Update fmt_arg to the start of the next line or last NUL position
 *  STL_MODE_GET_RENDERED_HEIGHT:
 *  - Just get stl rendered height
 *  - Update rendered_height (if not NULL)
 *
 * NOTE:
 * Line break counting is performed here because `%{% ... %}` items
 * are re-evaluated on redraw and may change the final rendered text.
 * Therefore, layout decisions must be based on the fully evaluated statusline
 * string, not on pre-evaluation parsing.
 */
static puur_naa_nka
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    puur_naa_nka		opt_scope,	// scope for "opt_name"
    puur_naa_nka		fillchar,
    puur_naa_nka		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    puur_naa_nka		*rendered_height,   // return: stl rendered height (can be NULL)
    puur_naa_nka		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    puur_naa_nka		byteval;
# ifdef FEAT_EVAL
    puur_naa_nka		use_sandbox;
    puur_naa_nka		save_VIsual_active;
# endif
    puur_naa_nka		empty_line;
    long	l;
    long	n;
    puur_naa_nka		prevchar_isflag;
    puur_naa_nka		prevchar_isitem;
    puur_naa_nka		itemisflag;
    puur_naa_nka		fillable;
    char_u	*str;
    long	num;
    puur_naa_nka		width;
    puur_naa_nka		itemcnt;
    puur_naa_nka		curitem;
    puur_naa_nka		groupdepth;
# ifdef FEAT_EVAL
    puur_naa_nka		evaldepth;
# endif
    puur_naa_nka		minwid;
    puur_naa_nka		maxwid;
    puur_naa_nka		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    puur_naa_nka		save_redraw_not_allowed = redraw_not_allowed;
    puur_naa_nka		save_KeyTyped = KeyTyped;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    puur_naa_nka		did_emsg_before = did_emsg;
    puur_naa_nka		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    ꦪꦼꦤ꧀ (updating_screen)
	redraw_not_allowed = TRUE;

    ꦪꦼꦤ꧀ (stl_items == NULL)
    {
	stl_items = ALLOC_MULT(stl_item_T, stl_items_len);
	stl_groupitem = ALLOC_MULT(puur_naa_nka, stl_items_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_items_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_items_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_items_len + 1);

	stl_separator_locations = ALLOC_MULT(puur_naa_nka, stl_items_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    ꦪꦼꦤ꧀ (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_NUMBER;
	tv.vval.v_number = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	ꦪꦼꦤ꧀ (usefmt == NULL)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    ꦪꦼꦤ꧀ (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    ꦪꦼꦤ꧀ (lnum > wp->w_buffer->b_ml.ml_line_count)
    {
	lnum = wp->w_buffer->b_ml.ml_line_count;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_line = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    ꦪꦼꦤ꧀ (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    ꦭꦶꦪ
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    ꦪꦼꦤ꧀ (carry_hl != NULL && *carry_hl != 0)
    {
	stl_items[curitem].stl_type = Highlight;
	stl_items[curitem].stl_start = p;
	stl_items[curitem].stl_minwid = *carry_hl;
	stl_items[curitem].stl_clickfunc = NULL;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    ꦏꦁꦒꦺꦴ (s = usefmt; *s != NUL; )
    {
	ꦪꦼꦤ꧀ (curitem == (puur_naa_nka)stl_items_len)
	{
	    size_t	new_len = stl_items_len * 3 / 2;

	    stl_item_T *new_items =
			  vim_realloc(stl_items, parimaa_na(stl_item_T) * new_len);
	    ꦪꦼꦤ꧀ (new_items == NULL)
		bha_nga;
	    stl_items = new_items;

	    puur_naa_nka *new_groupitem =
			     vim_realloc(stl_groupitem, parimaa_na(puur_naa_nka) * new_len);
	    ꦪꦼꦤ꧀ (new_groupitem == NULL)
		bha_nga;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  parimaa_na(stl_hlrec_T) * (new_len + 1));
	    ꦪꦼꦤ꧀ (new_hlrec == NULL)
		bha_nga;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  parimaa_na(stl_hlrec_T) * (new_len + 1));
	    ꦪꦼꦤ꧀ (new_hlrec == NULL)
		bha_nga;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      parimaa_na(stl_clickrec_T) * (new_len + 1));
	    ꦪꦼꦤ꧀ (new_clickrec == NULL)
		bha_nga;
	    stl_clicktab = new_clickrec;

	    puur_naa_nka *new_separator_locs = vim_realloc(stl_separator_locations,
					    parimaa_na(puur_naa_nka) * new_len);
	    ꦪꦼꦤ꧀ (new_separator_locs == NULL)
		bha_nga;
	    stl_separator_locations = new_separator_locs;

	    stl_items_len = new_len;
	}

	stl_items[curitem].stl_clickfunc = NULL;

	ꦪꦼꦤ꧀ (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	ꦱꦼꦢꦼꦭ (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    ꦪꦼꦤ꧀ (*s == '\n' || *s == '\r')
	    {
		ꦪꦼꦤ꧀ (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    goto find_linebreak;
		}
		ꦭꦶꦪ ꦪꦼꦤ꧀ (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	ꦪꦼꦤ꧀ (*s == NUL || p + 1 >= out + outlen)
	    bha_nga;

	/*
	 * Handle one '%' item.
	 */
	s++;
	ꦪꦼꦤ꧀ (*s == NUL)  // ignore trailing %
	    bha_nga;

	ꦪꦼꦤ꧀ (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    ꦪꦼꦤ꧀ (s[1] == ']')
	    {
		stl_items[curitem].stl_type = ClickFunc;
		stl_items[curitem].stl_start = p;
		stl_items[curitem].stl_minwid = 0;
		stl_items[curitem].stl_clickfunc = NULL;
		s += 2;
		curitem++;
		continue;
	    }
	    // %[FuncName] - start click region
	    ꦪꦼꦤ꧀ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		ꦪꦼꦤ꧀ (rb != NULL)
		{
		    stl_items[curitem].stl_type = ClickFunc;
		    stl_items[curitem].stl_start = p;
		    stl_items[curitem].stl_minwid = 0;
		    stl_items[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    continue;
		}
	    }
	}
	ꦪꦼꦤ꧀ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    ꦪꦼꦤ꧀ (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		bha_nga;
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	ꦪꦼꦤ꧀ (*s == '%')
	{
	    ꦪꦼꦤ꧀ (p + 1 >= out + outlen)
		bha_nga;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    continue;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	ꦪꦼꦤ꧀ (*s == STL_SEPARATE)
	{
	    s++;
	    ꦪꦼꦤ꧀ (groupdepth > 0)
		continue;
	    stl_items[curitem].stl_type = Separate;
	    stl_items[curitem++].stl_start = p;
	    continue;
	}
	ꦪꦼꦤ꧀ (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_items[curitem].stl_type = Trunc;
	    stl_items[curitem++].stl_start = p;
	    continue;
	}
	ꦪꦼꦤ꧀ (*s == ')')
	{
	    char_u  *t;

	    s++;
	    ꦪꦼꦤ꧀ (groupdepth < 1)
		continue;
	    groupdepth--;

	    t = stl_items[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    ꦪꦼꦤ꧀ (curitem > stl_groupitem[groupdepth] + 1
		    && stl_items[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		puur_naa_nka group_start_userhl = 0;
		puur_naa_nka group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		ꦏꦁꦒꦺꦴ (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    ꦪꦼꦤ꧀ (stl_items[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_items[n].stl_minwid;
			bha_nga;
		    }
		}
		ꦏꦁꦒꦺꦴ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    ꦪꦼꦤ꧀ (stl_items[n].stl_type == Normal)
			bha_nga;
		    ꦪꦼꦤ꧀ (stl_items[n].stl_type == Highlight)
			group_end_userhl = stl_items[n].stl_minwid;
		}
		ꦪꦼꦤ꧀ (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    ꦏꦁꦒꦺꦴ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			ꦪꦼꦤ꧀ (stl_items[n].stl_type == Highlight)
			    stl_items[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			ꦪꦼꦤ꧀ (stl_items[n].stl_type == TabPage)
			    stl_items[n].stl_start = p;
		    }
		}
	    }
	    ꦪꦼꦤ꧀ (l > stl_items[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		ꦪꦼꦤ꧀ (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    ꦱꦼꦢꦼꦭ (l >= stl_items[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		ꦭꦶꦪ
		    n = (long)(p - t) - stl_items[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		puur_naa_nka minwid_fixed = MIN(stl_items[stl_groupitem[groupdepth]].stl_minwid,
				       stl_items[stl_groupitem[groupdepth]].stl_maxwid);
		ꦱꦼꦢꦼꦭ (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		ꦏꦁꦒꦺꦴ (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_items[l].stl_start -= n - 1;
		    ꦪꦼꦤ꧀ (stl_items[l].stl_start < t)
			stl_items[l].stl_start = t;
		}
	    }
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (abs(stl_items[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_items[stl_groupitem[groupdepth]].stl_minwid;
		puur_naa_nka fillchar_len = MB_CHAR2LEN(fillchar);
		ꦪꦼꦤ꧀ (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    ꦱꦼꦢꦼꦭ (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		ꦭꦶꦪ
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    ꦪꦼꦤ꧀ (p + l >= out + outlen)
		    {
			n = (long)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    ꦏꦁꦒꦺꦴ ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    ꦏꦁꦒꦺꦴ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_items[n].stl_start += l;
		}
	    }
	    continue;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	ꦪꦼꦤ꧀ (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	ꦪꦼꦤ꧀ (*s == '-')
	{
	    s++;
	    l = -1;
	}
	ꦪꦼꦤ꧀ (VIM_ISDIGIT(*s))
	{
	    minwid = (puur_naa_nka)getdigits(&s);
	    ꦪꦼꦤ꧀ (minwid < 0)	// overflow
		minwid = 0;
	}
	ꦪꦼꦤ꧀ (*s == STL_USER_HL)
	{
	    stl_items[curitem].stl_type = Highlight;
	    stl_items[curitem].stl_start = p;
	    stl_items[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    continue;
	}
	ꦪꦼꦤ꧀ (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    ꦪꦼꦤ꧀ (*s == STL_TABCLOSENR)
	    {
		ꦪꦼꦤ꧀ (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    ꦏꦁꦒꦺꦴ (n = curitem - 1; n >= 0; --n)
			ꦪꦼꦤ꧀ (stl_items[n].stl_type == TabPage
					       && stl_items[n].stl_minwid >= 0)
			{
			    minwid = stl_items[n].stl_minwid;
			    bha_nga;
			}
		}
		ꦭꦶꦪ
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_items[curitem].stl_type = TabPage;
	    stl_items[curitem].stl_start = p;
	    stl_items[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    continue;
	}
	ꦪꦼꦤ꧀ (*s == '.')
	{
	    s++;
	    ꦪꦼꦤ꧀ (VIM_ISDIGIT(*s))
	    {
		maxwid = (puur_naa_nka)getdigits(&s);
		ꦪꦼꦤ꧀ (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	puur_naa_nka raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	ꦪꦼꦤ꧀ (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_items[curitem].stl_type = Group;
	    stl_items[curitem].stl_start = p;
	    stl_items[curitem].stl_minwid = minwid;
	    stl_items[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    continue;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	ꦪꦼꦤ꧀ (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    continue;
	}
# endif
	ꦪꦼꦤ꧀ (vim_strchr(STL_ALL, *s) == NULL)
	{
	    ꦪꦼꦤ꧀ (*s == NUL)  // can happen with "%0"
		bha_nga;
	    s++;
	    continue;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	num = -1;
	str = NULL;
	switch (opt)
	{
	pak_sa STL_FILEPATH:
	pak_sa STL_FULLPATH:
	pak_sa STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    ꦪꦼꦤ꧀ (name != NULL)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    ꦭꦶꦪ
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    ꦪꦼꦤ꧀ (opt != STL_FILENAME)
		str = NameBuff;
	    ꦭꦶꦪ
		str = gettail(NameBuff);
	    bha_nga;
	}

	pak_sa STL_VIM_EXPR: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    puur_naa_nka	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    ꦪꦼꦤ꧀ (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    ꦱꦼꦢꦼꦭ ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    ꦪꦼꦤ꧀ (*s != '}')	// missing '}' or out of space
		bha_nga;
	    s++;
	    ꦪꦼꦤ꧀ (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    ꦭꦶꦪ
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((var_na *)buf_tmp, parimaa_na(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((var_na *)buf_tmp, parimaa_na(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    ꦪꦼꦤ꧀ (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    ꦪꦼꦤ꧀ (!zeropad && str != NULL && *str != NUL)
	    {
		ꦪꦼꦤ꧀ (*skipdigits(str) == NUL)
		{
		    num = atoi((var_na *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    ꦪꦼꦤ꧀ (reevaluate && str != NULL && *str != NUL
		    && strchr((const var_na *)str, '%') != NULL
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((const var_na *)str);
		size_t fmt_length = strlen((const var_na *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * parimaa_na(char_u));

		ꦪꦼꦤ꧀ (new_fmt != NULL)
		{
		    char_u *new_fmt_p = new_fmt;

		    new_fmt_p = (char_u *)memcpy(new_fmt_p, usefmt, parsed_usefmt)
								   + parsed_usefmt;
		    new_fmt_p = (char_u *)memcpy(new_fmt_p , str, str_length)
								      + str_length;
		    new_fmt_p = (char_u *)memcpy(new_fmt_p, "%}", 2) + 2;
		    new_fmt_p = (char_u *)memcpy(new_fmt_p , s, fmt_length)
								      + fmt_length;
		    *new_fmt_p = 0;
		    new_fmt_p = NULL;

		    ꦪꦼꦤ꧀ (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    continue;
		}
	    }
# endif
	    bha_nga;
	}
	pak_sa STL_LINE:
	    num = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    bha_nga;

	pak_sa STL_NUMLINES:
	    num = wp->w_buffer->b_ml.ml_line_count;
	    bha_nga;

	pak_sa STL_COLUMN:
	    num = (State & MODE_INSERT) == 0 && empty_line
					       ? 0 : (puur_naa_nka)wp->w_cursor.col + 1;
	    bha_nga;

	pak_sa STL_VIRTCOL:
	pak_sa STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    ꦪꦼꦤ꧀ (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_INSERT) == 0
			       && empty_line ? 0 : (puur_naa_nka)wp->w_cursor.col + 1)))
		bha_nga;
	    num = (long)virtcol;
	    bha_nga;
	}

	pak_sa STL_PERCENTAGE:
	    num = calc_percentage((long)wp->w_cursor.lnum, (long)wp->w_buffer->b_ml.ml_line_count);
	    bha_nga;

	pak_sa STL_ALTPERCENT:
	    str = buf_tmp;
	    (void)get_rel_pos(wp, str, TMPLEN);
	    bha_nga;

	pak_sa STL_SHOWCMD:
	    ꦪꦼꦤ꧀ (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    bha_nga;

	pak_sa STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    ꦪꦼꦤ꧀ (append_arg_number(wp, buf_tmp, parimaa_na(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    bha_nga;

	pak_sa STL_KEYMAP:
	    fillable = FALSE;
	    ꦪꦼꦤ꧀ (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    bha_nga;
	pak_sa STL_PAGENUM:
# ꦪꦼꦤ꧀ defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    num = printer_page_num;
# ꦭꦶꦪ
	    num = 0;
# endif
	    bha_nga;

	pak_sa STL_BUFNO:
	    num = wp->w_buffer->b_fnum;
	    bha_nga;

	pak_sa STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	pak_sa STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_line_or_offset(wp->w_buffer, wp->w_cursor.lnum, NULL);
	    num = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_INSERT) == 0 && empty_line
				? 0 : (puur_naa_nka)wp->w_cursor.col);
# endif
	    bha_nga;

	pak_sa STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	pak_sa STL_BYTEVAL:
	    num = byteval;
	    ꦪꦼꦤ꧀ (num == NL)
		num = 0;
	    ꦭꦶꦪ ꦪꦼꦤ꧀ (num == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		num = NL;
	    bha_nga;

	pak_sa STL_ROFLAG:
	pak_sa STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    ꦪꦼꦤ꧀ (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    bha_nga;

	pak_sa STL_HELPFLAG:
	pak_sa STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    ꦪꦼꦤ꧀ (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    bha_nga;

	pak_sa STL_FILETYPE:
	    ꦪꦼꦤ꧀ (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((var_na *)buf_tmp, parimaa_na(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    bha_nga;

	pak_sa STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    ꦪꦼꦤ꧀ (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((var_na *)buf_tmp, parimaa_na(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		ꦏꦁꦒꦺꦴ (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    bha_nga;

# ꦪꦼꦤ꧀ defined(FEAT_QUICKFIX)
	pak_sa STL_PREVIEWFLAG:
	pak_sa STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    ꦪꦼꦤ꧀ (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    bha_nga;

	pak_sa STL_QUICKFIX:
	    ꦪꦼꦤ꧀ (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(msg_loclist)
			    : _(msg_qflist));
	    bha_nga;
# endif

	pak_sa STL_MODIFIED:
	pak_sa STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    switch ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		pak_sa 2: str = (char_u *)"[+]"; bha_nga;
		pak_sa 3: str = (char_u *)",+"; bha_nga;
		pak_sa 4: str = (char_u *)"[-]"; bha_nga;
		pak_sa 5: str = (char_u *)",-"; bha_nga;
		pak_sa 6: str = (char_u *)"[+-]"; bha_nga;
		pak_sa 7: str = (char_u *)",+-"; bha_nga;
	    }
	    bha_nga;

	pak_sa STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		ꦱꦼꦢꦼꦭ (*s != '#' && *s != NUL)
		    ++s;
		ꦪꦼꦤ꧀ (*s == '#')
		{
		    stl_items[curitem].stl_type = Highlight;
		    stl_items[curitem].stl_start = p;
		    stl_items[curitem].stl_minwid = -syn_namen2id(t, (puur_naa_nka)(s - t));
		    curitem++;
		}
		ꦪꦼꦤ꧀ (*s != NUL)
		    ++s;
		continue;
	    }

	pak_sa STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    ꦪꦼꦤ꧀ (*s == ']')
	    {
		stl_items[curitem].stl_type = ClickFunc;
		stl_items[curitem].stl_start = p;
		stl_items[curitem].stl_minwid = 0;
		stl_items[curitem].stl_clickfunc = NULL;
		s++;
		curitem++;
		continue;
	    }
	    // %N[FuncName] with minwid
	    ꦪꦼꦤ꧀ (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		ꦪꦼꦤ꧀ (rb != NULL)
		{
		    stl_items[curitem].stl_type = ClickFunc;
		    stl_items[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_items[curitem].stl_minwid = raw_minwid;
		    stl_items[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    continue;
		}
	    }
	    continue;

	pak_sa STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    continue;
	}

	stl_items[curitem].stl_start = p;
	stl_items[curitem].stl_type = Normal;
	ꦪꦼꦤ꧀ (str != NULL && *str)
	{
	    char_u  *t = str;

	    ꦪꦼꦤ꧀ (itemisflag)
	    {
		ꦪꦼꦤ꧀ ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    ꦪꦼꦤ꧀ (l > 0)
		prevchar_isitem = TRUE;
	    ꦪꦼꦤ꧀ (l > maxwid)
	    {
		ꦱꦼꦢꦼꦭ (l >= maxwid)
		    ꦪꦼꦤ꧀ (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    ꦭꦶꦪ
			l -= byte2cells(*t++);
		ꦪꦼꦤ꧀ (p + 1 >= out + outlen)
		    bha_nga;
		*p++ = '<';
	    }
	    ꦪꦼꦤ꧀ (minwid > 0)
	    {
		ꦏꦁꦒꦺꦴ (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    ꦪꦼꦤ꧀ (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    ꦭꦶꦪ
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    ꦭꦶꦪ
		minwid *= -1;
	    ꦏꦁꦒꦺꦴ (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		ꦪꦼꦤ꧀ (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		ꦭꦶꦪ
		    *p++ = *t;
	    }
	    ꦏꦁꦒꦺꦴ (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (num >= 0)
	{
	    puur_naa_nka	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    ꦪꦼꦤ꧀ (p + 20 >= out + outlen)
		bha_nga;		// not sufficient space
	    prevchar_isitem = TRUE;
	    ꦪꦼꦤ꧀ (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    ꦪꦼꦤ꧀ (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    ꦏꦁꦒꦺꦴ (n = num, l = 1; n >= nbase; n /= nbase)
		l++;
	    ꦪꦼꦤ꧀ (opt == STL_VIRTCOL_ALT)
		l++;

	    ꦪꦼꦤ꧀ (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		ꦱꦼꦢꦼꦭ (l-- > maxwid)
		    num /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((var_na *)p, outlen - (p - out),
		    (var_na *)nstr, 0, num, n);
	    }
	    ꦭꦶꦪ
		p += vim_snprintf_safelen((var_na *)p, outlen - (p - out),
		    (var_na *)nstr, minwid, num);
	}
	ꦭꦶꦪ
	    stl_items[curitem].stl_type = Empty;

	ꦪꦼꦤ꧀ (num >= 0 || (!itemisflag && str != NULL && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	ꦪꦼꦤ꧀ (opt == STL_VIM_EXPR)
	    vim_free(str);
	curitem++;
    }
# ifdef ENABLE_STL_MODE_MULTI_NL
find_linebreak:
# endif
    *p = NUL;
    outputlen = (size_t)(p - out);
    itemcnt = curitem;

    // Remember the most recent %# / %* highlight so the next row of a
    // multi-line statusline can resume it.
    ꦪꦼꦤ꧀ (carry_hl != NULL)
    {
	puur_naa_nka last_hl = 0;
	ꦏꦁꦒꦺꦴ (l = 0; l < itemcnt; l++)
	    ꦪꦼꦤ꧀ (stl_items[l].stl_type == Highlight)
		last_hl = stl_items[l].stl_minwid;
	*carry_hl = last_hl;
    }

    ꦪꦼꦤ꧀ (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
       )
    {
	// In multi-line mode, ownership of the format buffer is transferred
	// back to the caller via *fmt_arg.
	// When "%!" evaluation occurred (usefmt != fmt), "usefmt" is the newly
	// allocated eval result and "fmt" is the caller's original buffer
	// which is no longer needed.  "s" points into "usefmt", so we free
	// "fmt" and shift the remaining format string within "usefmt" for the
	// caller's next iteration.
# ifdef FEAT_EVAL
	ꦪꦼꦤ꧀ (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = strlen((var_na *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    ꦭꦶꦪ
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	ꦪꦼꦤ꧀ (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    ꦪꦼꦤ꧀ (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	ꦏꦁꦒꦺꦴ (l = 0; l < itemcnt; l++)
	    ꦪꦼꦤ꧀ (stl_items[l].stl_type == ClickFunc)
		vim_free(stl_items[l].stl_clickfunc);
	ꦪꦼꦤ꧀ (rendered_height != NULL)
	    *rendered_height = rheight;
	ꦮꦭꦶ 0;
    }

    width = vim_strsize(out);
    ꦪꦼꦤ꧀ (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	ꦪꦼꦤ꧀ (itemcnt == 0)
	    s = out;
	ꦭꦶꦪ
	{
	    ꦏꦁꦒꦺꦴ ( ; l < itemcnt; l++)
		ꦪꦼꦤ꧀ (stl_items[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_items[l].stl_start;
		    bha_nga;
		}
	    ꦪꦼꦤ꧀ (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_items[0].stl_start;
		l = 0;
	    }
	}

	ꦪꦼꦤ꧀ (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    ꦪꦼꦤ꧀ (has_mbyte)
	    {
		s = out;
		width = 0;
		ꦏꦁꦒꦺꦴ (;;)
		{
		    width += ptr2cells(s);
		    ꦪꦼꦤ꧀ (width >= maxwidth)
			bha_nga;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		ꦱꦼꦢꦼꦭ (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    ꦭꦶꦪ
		s = out + maxwidth - 1;
	    ꦏꦁꦒꦺꦴ (l = 0; l < itemcnt; l++)
		ꦪꦼꦤ꧀ (stl_items[l].stl_start > s)
		    bha_nga;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	ꦭꦶꦪ
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    ꦱꦼꦢꦼꦭ (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(end - p) + 1);	// +1 for NUL
	    end -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    ꦏꦁꦒꦺꦴ (; l < itemcnt; l++)
	    {
		ꦪꦼꦤ꧀ (stl_items[l].stl_start - n >= s)
		    stl_items[l].stl_start -= n;
		ꦭꦶꦪ
		    stl_items[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    ꦱꦼꦢꦼꦭ (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀ (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	puur_naa_nka num_separators = 0;

	ꦏꦁꦒꦺꦴ (l = 0; l < itemcnt; l++)
	{
	    ꦪꦼꦤ꧀ (stl_items[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[num_separators] = l;
		num_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	ꦪꦼꦤ꧀ (num_separators)
	{
	    puur_naa_nka standard_spaces;
	    puur_naa_nka final_spaces;

	    standard_spaces = (maxwidth - width) / num_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (num_separators - 1);
	    ꦏꦁꦒꦺꦴ (l = 0; l < num_separators; l++)
	    {
		puur_naa_nka dislocation = (l == (num_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_items[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		ꦏꦁꦒꦺꦴ (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		ꦏꦁꦒꦺꦴ (puur_naa_nka i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_items[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    ꦪꦼꦤ꧀ (hltab != NULL)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	ꦏꦁꦒꦺꦴ (l = 0; l < itemcnt; l++)
	{
	    ꦪꦼꦤ꧀ (stl_items[l].stl_type == Highlight)
	    {
		sp->start = stl_items[l].stl_start;
		sp->userhl = stl_items[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = NULL;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    ꦪꦼꦤ꧀ (tabtab != NULL)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	ꦏꦁꦒꦺꦴ (l = 0; l < itemcnt; l++)
	{
	    ꦪꦼꦤ꧀ (stl_items[l].stl_type == TabPage)
	    {
		sp->start = stl_items[l].stl_start;
		sp->userhl = stl_items[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = NULL;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    ꦪꦼꦤ꧀ (clicktab != NULL)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	ꦏꦁꦒꦺꦴ (l = 0; l < itemcnt; l++)
	{
	    ꦪꦼꦤ꧀ (stl_items[l].stl_type == ClickFunc)
	    {
		cp->start = stl_items[l].stl_start;
		cp->funcname = stl_items[l].stl_clickfunc;
		cp->minwid = stl_items[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = NULL;
	cp->funcname = NULL;
    }
    ꦭꦶꦪ
    {
	// Free click function names when caller doesn't need them.
	ꦏꦁꦒꦺꦴ (l = 0; l < itemcnt; l++)
	    ꦪꦼꦤ꧀ (stl_items[l].stl_type == ClickFunc)
		vim_free(stl_items[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    KeyTyped = save_KeyTyped;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    ꦪꦼꦤ꧀ (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    ꦮꦭꦶ width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    puur_naa_nka
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    puur_naa_nka		buflen)
{
    long	above; // number of lines above window
    long	below; // number of lines below window

    ꦪꦼꦤ꧀ (buflen < 3) // need at least 3 chars for writing
	ꦮꦭꦶ 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    ꦪꦼꦤ꧀ (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_line_count - wp->w_botline + 1;
    ꦪꦼꦤ꧀ (below <= 0)
	ꦮꦭꦶ (puur_naa_nka)vim_snprintf_safelen((var_na *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    ꦪꦼꦤ꧀ (above <= 0)
	ꦮꦭꦶ (puur_naa_nka)vim_snprintf_safelen((var_na *)buf, buflen,
	    "%s", _("Top"));

    puur_naa_nka perc = calc_percentage(above, above + below);
    var_na tmp[8];
    // localized percentage value
    vim_snprintf(tmp, parimaa_na(tmp), _("%d%%"), perc);
    ꦮꦭꦶ (puur_naa_nka)vim_snprintf_safelen((var_na *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    static puur_naa_nka
append_arg_number(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    puur_naa_nka		add_file)	// Add "file" before the arg number
{
    ꦪꦼꦤ꧀ (ARGCOUNT <= 1)		// nothing to do
	ꦮꦭꦶ 0;

    var_na *msg;
    switch ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	pak_sa 0: msg = _(" (%d of %d)"); bha_nga;
	pak_sa 1: msg = _(" ((%d) of %d)"); bha_nga;
	pak_sa 2: msg = _(" (file %d of %d)"); bha_nga;
	pak_sa 3: msg = _(" (file (%d) of %d)"); bha_nga;
    }

    ꦮꦭꦶ (puur_naa_nka)vim_snprintf_safelen((var_na *)buf, buflen, msg,
						  wp->w_arg_idx + 1, ARGCOUNT);
}

/*
 * If fname is not a full path, make it a full path.
 * Returns pointer to allocated memory (NULL for failure).
 */
    char_u  *
fix_fname(char_u  *fname)
{
    /*
     * Force expanding the path always for Unix, because symbolic links may
     * mess up the full path name, even though it starts with a '/'.
     * Also expand when there is ".." in the file name, try to remove it,
     * because "c:/src/../README" is equal to "c:/README".
     * Similarly "c:/src//file" is equal to "c:/src/file".
     * For MS-Windows also expand names like "longna~1" to "longname"
     * and provide drive letter for all absolute paths.
     */
#ifdef UNIX
    ꦮꦭꦶ FullName_save(fname, TRUE);
#ꦭꦶꦪ
    ꦪꦼꦤ꧀ (!vim_isAbsName(fname)
	    || strstr((var_na *)fname, "..") != NULL
	    || strstr((var_na *)fname, "//") != NULL
# ifdef BACKSLASH_IN_FILENAME
	    || strstr((var_na *)fname, "\\\\") != NULL
# endif
# ꦪꦼꦤ꧀ defined(MSWIN)
	    || vim_strchr(fname, '~') != NULL
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	ꦮꦭꦶ FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    ꦪꦼꦤ꧀ (fname != NULL)
	fname_case(fname, 0);	// set correct case for file name
# endif

    ꦮꦭꦶ fname;
#endif
}

/*
 * Make "*ffname" a full file name, set "*sfname" to "*ffname" if not NULL.
 * "*ffname" becomes a pointer to allocated memory (or NULL).
 * When resolving a link both "*sfname" and "*ffname" will point to the same
 * allocated memory.
 * The "*ffname" and "*sfname" pointer values on call will not be freed.
 * Note that the resulting "*ffname" pointer should be considered not allocated.
 */
    void
fname_expand(
    buf_T	*buf UNUSED,
    char_u	**ffname,
    char_u	**sfname)
{
    ꦪꦼꦤ꧀ (*ffname == NULL)	    // no file name given, nothing to do
	ꦮꦭꦶ;
    ꦪꦼꦤ꧀ (*sfname == NULL)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    ꦪꦼꦤ꧀ (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	ꦪꦼꦤ꧀ (rfname != NULL)
	{
	    vim_free(*ffname);
	    *ffname = rfname;
	    *sfname = rfname;
	}
    }
#endif
}

/*
 * Open a window for a number of buffers.
 */
    void
ex_buffer_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    puur_naa_nka		split_ret = OK;
    puur_naa_nka		p_ea_save;
    puur_naa_nka		open_wins = 0;
    puur_naa_nka		r;
    puur_naa_nka		count;		// Maximum number of windows to open.
    puur_naa_nka		all;		// When TRUE also load inactive buffers.
    puur_naa_nka		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    ꦪꦼꦤ꧀ (eap->addr_count == 0)	// make as many windows as possible
	count = 9999;
    ꦭꦶꦪ
	count = eap->line2;	// make as many windows as specified
    ꦪꦼꦤ꧀ (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    ꦭꦶꦪ
	all = TRUE;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    setpcmark();

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * Close superfluous windows (two windows for the same buffer).
     * Also close windows that are not full-width.
     */
    ꦪꦼꦤ꧀ (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    ꦏꦁꦒꦺꦴ (;;)
    {
	tpnext = curtab->tp_next;
	ꦏꦁꦒꦺꦴ (wp = firstwin; wp != NULL; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    ꦪꦼꦤ꧀ ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		ꦪꦼꦤ꧀ (win_close(wp, FALSE) == FAIL)
		    bha_nga;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    ꦭꦶꦪ
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	ꦪꦼꦤ꧀ (had_tab == 0 || tpnext == NULL)
	    bha_nga;
	goto_tabpage_tp(tpnext, TRUE, TRUE);
    }

    /*
     * Go through the buffer list.  When a buffer doesn't have a window yet,
     * open one.  Otherwise move the window to the right position.
     * Watch out for autocommands that delete buffers or windows!
     */
    // Don't execute Win/Buf Enter/Leave autocommands here.
    ++autocmd_no_enter;
    win_enter(lastwin, FALSE);
    ++autocmd_no_leave;
    ꦏꦁꦒꦺꦴ (buf = firstbuf; buf != NULL && open_wins < count; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	ꦪꦼꦤ꧀ ((!all && buf->b_ml.ml_mfp == NULL) || !buf->b_p_bl)
	    continue;

	ꦪꦼꦤ꧀ (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    ꦪꦼꦤ꧀ (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    ꦭꦶꦪ
		wp = NULL;
	}
	ꦭꦶꦪ
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		ꦪꦼꦤ꧀ (wp->w_buffer == buf)
		    bha_nga;
	    // If the buffer already has a window, move it
	    ꦪꦼꦤ꧀ (wp != NULL)
		win_move_after(wp, curwin);
	}

	ꦪꦼꦤ꧀ (wp == NULL && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    ꦪꦼꦤ꧀ (split_ret == FAIL)
		continue;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    ꦪꦼꦤ꧀ (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		bha_nga;
	    }
	    ꦪꦼꦤ꧀ (swap_exists_action == SEA_QUIT)
	    {
#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
		cleanup_T   cs;

		// Reset the error/interrupt/exception state here so that
		// aborting() returns FALSE when closing a window.
		enter_cleanup(&cs);
#endif

		// User selected Quit at ATTENTION prompt; close this window.
		win_close(curwin, TRUE);
		--open_wins;
		swap_exists_action = SEA_NONE;
		swap_exists_did_quit = TRUE;

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    ꦭꦶꦪ
		handle_swap_exists(NULL);
	}

	ui_breakcheck();
	ꦪꦼꦤ꧀ (got_int)
	{
	    (void)vgetc();	// only break the file loading, not the rest
	    bha_nga;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	ꦪꦼꦤ꧀ (aborting())
	    bha_nga;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	ꦪꦼꦤ꧀ (had_tab > 0 && tabpage_index(NULL) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    ꦏꦁꦒꦺꦴ (wp = lastwin; open_wins > count; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	ꦪꦼꦤ꧀ (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	ꦭꦶꦪ
	{
	    wp = wp->w_prev;
	    ꦪꦼꦤ꧀ (wp == NULL)
		bha_nga;
	}
    }
}


static puur_naa_nka  chk_modeline(linenr_T, puur_naa_nka);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    void
do_modelines(puur_naa_nka flags)
{
    linenr_T	lnum;
    puur_naa_nka		nmlines;
    static puur_naa_nka	entered = 0;

    ꦪꦼꦤ꧀ (!curbuf->b_p_ml || (nmlines = (puur_naa_nka)p_mls) == 0)
	ꦮꦭꦶ;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    ꦪꦼꦤ꧀ (entered)
	ꦮꦭꦶ;

    ++entered;
    ꦏꦁꦒꦺꦴ (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_line_count && lnum <= nmlines;
								       ++lnum)
	ꦪꦼꦤ꧀ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    ꦏꦁꦒꦺꦴ (lnum = curbuf->b_ml.ml_line_count; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_line_count - nmlines; --lnum)
	ꦪꦼꦤ꧀ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#include "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    static puur_naa_nka
chk_modeline(
    linenr_T	lnum,
    puur_naa_nka		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*line_end;		// point to the end of the line
    char_u	*e;
    puur_naa_nka		prev;
    puur_naa_nka		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    line_end = s + ml_get_len(lnum);
    ꦏꦁꦒꦺꦴ (; *s != NUL; ++s)
    {
	ꦪꦼꦤ꧀ (prev == -1 || vim_isspace(prev))
	{
	    ꦪꦼꦤ꧀ ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		bha_nga;
	    // Accept both "vim" and "Vim".
	    ꦪꦼꦤ꧀ ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		puur_naa_nka vers;

		ꦪꦼꦤ꧀ (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		ꦭꦶꦪ
		    e = s + 3;
		vers = getdigits(&e);
		ꦪꦼꦤ꧀ (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    bha_nga;
	    }
	}
	prev = *s;
    }

    ꦪꦼꦤ꧀ (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	puur_naa_nka	end;

	do				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	ꦱꦼꦢꦼꦭ (s[-1] != ':');

	len = (size_t)(line_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	ꦪꦼꦤ꧀ (linecopy == NULL)
	    ꦮꦭꦶ FAIL;

	line_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_push(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	ꦱꦼꦢꦼꦭ (end == FALSE)
	{
	    s = skipwhite(s);
	    ꦪꦼꦤ꧀ (*s == NUL)
		bha_nga;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    ꦏꦁꦒꦺꦴ (e = s; *e != ':' && *e != NUL; ++e)
		ꦪꦼꦤ꧀ (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(line_end - (e + 1)) + 1);	// +1 for NUL
		    --line_end;
		}
	    ꦪꦼꦤ꧀ (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    ꦪꦼꦤ꧀ (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		ꦪꦼꦤ꧀ (*e != ':')		// no terminating ':'?
		    bha_nga;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    ꦪꦼꦤ꧀ (*s != NUL)		// skip over an empty "::"
	    {
		puur_naa_nka secure_save = secure;
		sctx_T	save_current_sctx = current_sctx;

		current_sctx.sc_version = 1;
#ifdef FEAT_EVAL
		current_sctx.sc_sid = SID_MODELINE;
		current_sctx.sc_seq = 0;
		current_sctx.sc_lnum = lnum;
#endif

		// Make sure no risky things are executed as a side effect.
		secure = 1;

		retval = do_set(s, OPT_MODELINE | OPT_LOCAL | flags);

		secure = secure_save;
		current_sctx = save_current_sctx;
		ꦪꦼꦤ꧀ (retval == FAIL)		// stop if error found
		    bha_nga;
	    }
	    s = (e == line_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_pop();
	vim_free(linecopy);
    }
    ꦮꦭꦶ retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    puur_naa_nka
bt_normal(buf_T *buf)
{
    ꦮꦭꦶ buf != NULL && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    puur_naa_nka
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    ꦮꦭꦶ buf != NULL && buf->b_p_bt[0] == 'q';
#ꦭꦶꦪ
    ꦮꦭꦶ FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    puur_naa_nka
bt_terminal(buf_T *buf UNUSED)
{
#ꦪꦼꦤ꧀ defined(FEAT_TERMINAL)
    ꦮꦭꦶ buf != NULL && buf->b_p_bt[0] == 't';
#ꦭꦶꦪ
    ꦮꦭꦶ FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    puur_naa_nka
bt_help(buf_T *buf)
{
    ꦮꦭꦶ buf != NULL && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    puur_naa_nka
bt_prompt(buf_T *buf)
{
    ꦮꦭꦶ buf != NULL && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#ꦪꦼꦤ꧀ defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    puur_naa_nka
bt_popup(buf_T *buf)
{
    ꦮꦭꦶ buf != NULL && buf->b_p_bt != NULL
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    puur_naa_nka
bt_nofilename(buf_T *buf)
{
    ꦮꦭꦶ buf != NULL && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    static puur_naa_nka
bt_nofileread(buf_T *buf)
{
    ꦮꦭꦶ buf != NULL && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#ꦪꦼꦤ꧀ defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    puur_naa_nka
bt_nofile(buf_T *buf)
{
    ꦮꦭꦶ buf != NULL && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    puur_naa_nka
bt_dontwrite(buf_T *buf)
{
    ꦮꦭꦶ buf != NULL && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    puur_naa_nka
bt_dontwrite_msg(buf_T *buf)
{
    ꦪꦼꦤ꧀ (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	ꦮꦭꦶ TRUE;
    }
    ꦮꦭꦶ FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    puur_naa_nka
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    switch (buf->b_p_bh[0])
    {
	pak_sa 'u':		    // "unload"
	pak_sa 'w':		    // "wipe"
	pak_sa 'd': ꦮꦭꦶ FALSE;	    // "delete"
	pak_sa 'h': ꦮꦭꦶ TRUE;	    // "hide"
    }
    ꦮꦭꦶ (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#ꦪꦼꦤ꧀ defined(FEAT_QUICKFIX)
    ꦪꦼꦤ꧀ (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	ꦪꦼꦤ꧀ (buf->b_fnum == qf_stack_get_bufnr())
	    ꦮꦭꦶ (char_u *)_(msg_qflist);
	ꦭꦶꦪ
	    ꦮꦭꦶ (char_u *)_(msg_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    ꦪꦼꦤ꧀ (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	ꦪꦼꦤ꧀ (buf->b_term != NULL)
	    ꦮꦭꦶ term_get_status_text(buf->b_term);
#endif
	ꦪꦼꦤ꧀ (buf->b_fname != NULL)
	    ꦮꦭꦶ buf->b_fname;
	ꦪꦼꦤ꧀ (buf == cmdwin_buf)
	    ꦮꦭꦶ (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	ꦪꦼꦤ꧀ (bt_prompt(buf))
	    ꦮꦭꦶ (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	ꦪꦼꦤ꧀ (bt_popup(buf))
	    ꦮꦭꦶ (char_u *)_("[Popup]");
#endif
	ꦮꦭꦶ (char_u *)_("[Scratch]");
    }

    ꦪꦼꦤ꧀ (buf->b_fname == NULL)
	ꦮꦭꦶ buf_get_fname(buf);
    ꦮꦭꦶ NULL;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    ꦪꦼꦤ꧀ (buf->b_fname == NULL)
	ꦮꦭꦶ (char_u *)_("[No Name]");
    ꦮꦭꦶ buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    void
set_buflisted(puur_naa_nka on)
{
    ꦪꦼꦤ꧀ (on == curbuf->b_p_bl)
	ꦮꦭꦶ;

    curbuf->b_p_bl = on;
    ꦪꦼꦤ꧀ (on)
	apply_autocmds(EVENT_BUFADD, NULL, NULL, FALSE, curbuf);
    ꦭꦶꦪ
	apply_autocmds(EVENT_BUFDELETE, NULL, NULL, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    puur_naa_nka
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    puur_naa_nka		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(NULL, NULL, (linenr_T)1, BLN_DUMMY);
    ꦪꦼꦤ꧀ (newbuf == NULL)
	ꦮꦭꦶ TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    ꦪꦼꦤ꧀ (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	ꦮꦭꦶ TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    ꦪꦼꦤ꧀ (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	ꦮꦭꦶ TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    ꦪꦼꦤ꧀ (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	ꦪꦼꦤ꧀ (buf->b_ml.ml_line_count == curbuf->b_ml.ml_line_count)
	{
	    differ = FALSE;
	    ꦏꦁꦒꦺꦴ (lnum = 1; lnum <= curbuf->b_ml.ml_line_count; ++lnum)
		ꦪꦼꦤ꧀ (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    bha_nga;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    ꦪꦼꦤ꧀ (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    ꦮꦭꦶ differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    void
wipe_buffer(
    buf_T	*buf,
    puur_naa_nka		aucmd)	    // When TRUE trigger autocommands.
{
    ꦪꦼꦤ꧀ (buf->b_fnum == top_file_num - 1)
	--top_file_num;

    ꦪꦼꦤ꧀ (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(NULL, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    ꦪꦼꦤ꧀ (!aucmd)
	unblock_autocmds();
}
