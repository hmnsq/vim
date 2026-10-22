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
static char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, 整数 ignore_case);
static char_u	*fname_match(regmatch_T *rmp, char_u *name, 整数 ignore_case);
#ifdef UNIX
static buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
static 整数	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
static 整数	buf_same_ino(buf_T *buf, stat_T *stp);
#否则
static 整数	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
static 整数	value_changed(char_u *str, char_u **last);
#如果 defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
static 整数 build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, 整数 opt_scope, 整数 fillchar, 整数 maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, 整数 *lbreaks, 整数 *carry_hl);
#endif
static 整数	append_arg_number(win_T *wp, char_u *buf, size_t buflen, 整数 add_file);
static void	free_buffer(buf_T *);
static void	free_buffer_stuff(buf_T *buf, 整数 free_options);
static 整数	bt_nofileread(buf_T *buf);
static 整数	do_buffer_ext(整数 action, 整数 start, 整数 dir, 整数 count, 整数 flags);

#ifdef UNIX
# define dev_T dev_t
#否则
# define dev_T unsigned
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    为 ((buf) = lastbuf; (buf) != NULL; (buf) = (buf)->b_prev)

#如果 defined(FEAT_QUICKFIX)
static char *msg_loclist = N_("[Location List]");
static char *msg_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
static 整数	buf_free_count = 0;

static 整数	top_file_num = 1;	// highest file number
static garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    static void
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"如果 exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    static 整数
calc_percentage(long part, long whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    返回 (part > 1000000L)
	? (整数)(part / (whole / 100L))
	: (整数)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    整数
get_highest_fnum(void)
{
    返回 top_file_num - 1;
}

/*
 * Read data from buffer for retrying.
 */
    static 整数
read_buffer(
    整数		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    整数		flags)		    // extra flags for readfile()
{
    整数		retval = OK;
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
    如果 (retval == OK)
    {
	// Delete the binary lines.
	当 (--line_count >= 0)
	    ml_delete((linenr_T)1);
    }
    否则
    {
	// Delete the converted lines.
	当 (curbuf->b_ml.ml_line_count > line_count)
	    ml_delete(line_count);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    如果 (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	如果 (!readonlymode && !BUFEMPTY())
	    changed();
	否则 如果 (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	如果 (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, NULL, NULL, FALSE,
							      curbuf, &retval);
#否则
	    apply_autocmds(EVENT_STDINREADPOST, NULL, NULL, FALSE, curbuf);
#endif
	}
    }
    返回 retval;
}

#如果 defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    void
buffer_ensure_loaded(buf_T *buf)
{
    如果 (buf->b_ml.ml_mfp != NULL)
	返回;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    如果 (curbuf == buf)
    {
	如果 (swap_exists_action != SEA_READONLY)
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
    整数
open_buffer(
    整数		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    整数		flags_arg)	    // extra flags for readfile()
{
    整数		flags = flags_arg;
    整数		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    整数		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    如果 (readonlymode && curbuf->b_ffname != NULL
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    如果 (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    如果 (curbuf->b_ml.ml_mfp != NULL)
		break;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	如果 (curbuf == NULL)
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
	如果 (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(NULL, curwin);
#endif
	返回 FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    如果 (curbuf->b_ml.ml_mfp != NULL)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    如果 (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    如果 (curbuf->b_ffname != NULL
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	整数 old_msg_silent = msg_silent;
#ifdef UNIX
	整数	save_bin = curbuf->b_p_bin;
	整数 perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	整数 oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	如果 (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	如果 (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	如果 (shortmess(SHM_FILEINFO))
	    msg_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	如果 (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    如果 (retval == OK)
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
	如果 (bt_help(curbuf))
	    fix_help_buffer();
    }
    否则 如果 (read_stdin)
    {
	整数	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(NULL, NULL, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, NULL,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	如果 (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    如果 (curbuf->b_ml.ml_mfp != NULL
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    如果 (curbuf->b_flags & BF_NEVERLOADED)
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
    如果 ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != NULL)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != NULL)
#endif
       )
	changed();
    否则 如果 (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    如果 (aborting())
#否则
    如果 (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    如果 (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf, &retval);
#否则
    apply_autocmds(EVENT_BUFENTER, NULL, NULL, FALSE, curbuf);
#endif

    如果 (retval != OK)
	返回 retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    如果 (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != NULL)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	如果 (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    如果 ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, NULL, NULL,
			FALSE, curbuf, &retval);
#否则
	    apply_autocmds(EVENT_BUFWINENTER, NULL, NULL,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    返回 retval;
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
    整数
bufref_valid(bufref_T *bufref)
{
    返回 bufref->br_buf_free_count == buf_free_count
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    整数
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	如果 (bp == buf)
	    返回 TRUE;
    返回 FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
static hashtab_T buf_hashtab;

    static void
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((char *)buf->b_key, sizeof(buf->b_key), "%x", buf->b_fnum);
    如果 (hash_add(&buf_hashtab, buf->b_key, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    static void
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_key);

    如果 (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    static 整数
can_unload_buffer(buf_T *buf)
{
    整数	    can_unload = !buf->b_locked;

    如果 (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    如果 (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		break;
	    }
    }
    // Don't unload the buffer while it's still being saved
    如果 (can_unload && buf->b_saving)
	can_unload = FALSE;

    如果 (!can_unload)
    {
	char_u *fname = buf->b_fname != NULL ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != NULL ? fname : (char_u *)"[No Name]");
    }
    返回 can_unload;
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
    整数
close_buffer(
    win_T	*win,
    buf_T	*buf,
    整数		action,
    整数		abort_if_last,
    整数		ignore_abort,
    整数		set_context)
{
    整数		hiding_buf;
    bufref_T	bufref;
    整数		is_curwin = (curwin != NULL && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    整数		unload_buf = (action != 0);
    整数		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    整数		del_buf = (action == DOBUF_DEL || wipe_buf);
    整数		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    整数		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    如果 (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    否则 如果 (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    否则 如果 (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    如果 (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	如果 (term_job_running(buf->b_term))
	{
	    如果 (wipe_buf || unload_buf)
	    {
		如果 (!can_unload_buffer(buf))
		    返回 FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    否则
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	否则 如果 (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	否则
	{
	    如果 (del_buf || unload_buf)
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
    如果 ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	返回 FALSE;

    如果 (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	如果 (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    如果 ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	如果 (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    返回 FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	如果 (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    goto aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	如果 (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    如果 (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		goto aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    如果 (abort_if_last && one_window())
		// Autocommands made this the only window.
		goto aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	如果 (!ignore_abort && aborting())
	    返回 FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    如果 (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    如果 (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    如果 (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    如果 (!hiding_buf || !unload_buf)
	返回 FALSE;

    // Always remove the buffer when there is no file name.
    如果 (buf->b_ffname == NULL)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    如果 (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	返回 FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    如果 ((win_valid || closed_popup) && win->w_buffer == buf)
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
    如果 (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != NULL || buf->b_next != NULL
#如果 defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	如果 (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    如果 (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, sizeof(整数), 50);
	    如果 (ga_grow(&buf_reuse, 1) == OK)
		((整数 *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	如果 (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	否则
	    buf->b_sfname = NULL;
	VIM_CLEAR(buf->b_ffname);
	如果 (buf->b_prev == NULL)
	    firstbuf = buf->b_next;
	否则
	    buf->b_prev->b_next = buf->b_next;
	如果 (buf->b_next == NULL)
	    lastbuf = buf->b_prev;
	否则
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    否则
    {
	如果 (del_buf)
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
	如果 (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    assert(!win_valid || !is_curwin || win == curwin);
    返回 TRUE;
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
    整数
buf_freeall(buf_T *buf, 整数 flags)
{
    整数		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    整数		is_curwin = (curwin != NULL && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    如果 (buf->b_ml.ml_mfp != NULL)
    {
	如果 (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    返回 FALSE;
    }
    如果 ((flags & BFA_DEL) && buf->b_p_bl)
    {
	如果 (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    返回 FALSE;
    }
    如果 (flags & BFA_WIPE)
    {
	如果 (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    返回 FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    如果 (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    如果 ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	返回 FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    如果 (buf == curbuf && !is_curbuf)
	返回 FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    如果 (buf == curbuf && VIsual_active
#如果 defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    如果 (curwin != NULL && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    如果 (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_line_count = 0;    // no lines in buffer
    如果 ((flags & BFA_KEEP_UNDO) == 0)
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
    返回 TRUE;
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

    如果 (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    否则
    {
	vim_free(buf);
	如果 (curbuf == buf)
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

    当 (buf->b_wininfo != NULL)
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
    整数		free_options)		// free options as well
{
    如果 (free_options)
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
    如果 (wip->wi_optset)
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
    整数		start,
    整数		dir,
    整数		count)
{
    bufref_T	old_curbuf;
    整数		save_sea = swap_exists_action;
    整数		skip_help_buf;

    switch (eap->cmdidx)
    {
	case CMD_bnext:
	case CMD_sbnext:
	case CMD_bNext:
	case CMD_bprevious:
	case CMD_sbNext:
	case CMD_sbprevious:
	    skip_help_buf = TRUE;
	    break;
	default:
	    skip_help_buf = FALSE;
	    break;
    }

    set_bufref(&old_curbuf, curbuf);

    如果 (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (void)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, count,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    如果 (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#如果 defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#如果 defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    否则
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    void
handle_swap_exists(bufref_T *old_curbuf)
{
#如果 defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    如果 (swap_exists_action == SEA_QUIT)
    {
#如果 defined(FEAT_EVAL)
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
	如果 (old_curbuf == NULL || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(NULL, NULL, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	否则
	    buf = old_curbuf->br_buf;
	如果 (buf != NULL)
	{
	    整数 old_msg_silent = msg_silent;

	    如果 (shortmess(SHM_FILEINFO))
		msg_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    msg_silent = old_msg_silent;

#ifdef FEAT_SYN_HL
	    如果 (old_tw != curbuf->b_p_tw)
		check_colorcolumn(NULL, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#如果 defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    否则 如果 (swap_exists_action == SEA_RECOVER)
    {
#如果 defined(FEAT_EVAL)
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

#如果 defined(FEAT_EVAL)
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
    static 整数
empty_curbuf(
    整数 close_others,
    整数 forceit,
    整数 action)
{
    整数	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    如果 (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	返回 FAIL;
    }

    set_bufref(&bufref, buf);
    如果 (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, NULL, NULL, NULL, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    如果 (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(NULL, buf, action, FALSE, FALSE, FALSE);
    如果 (!close_others)
	need_fileinfo = FALSE;
    否则 如果 (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    返回 retval;
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
    static 整数
do_buffer_ext(
    整数		action,
    整数		start,
    整数		dir,		// FORWARD or BACKWARD
    整数		count,		// buffer number or number of buffers
    整数		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    整数		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    switch (start)
    {
	case DOBUF_FIRST:   buf = firstbuf; break;
	case DOBUF_LAST:    buf = lastbuf;  break;
	default:	    buf = curbuf;   break;
    }
    如果 (start == DOBUF_MOD)	    // find next modified buffer
    {
	当 (count-- > 0)
	{
	    do
	    {
		buf = buf->b_next;
		如果 (buf == NULL)
		    buf = firstbuf;
	    }
	    当 (buf != curbuf && !bufIsChanged(buf));
	}
	如果 (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    返回 FAIL;
	}
    }
    否则 如果 (start == DOBUF_FIRST && count) // find specified buffer number
    {
	当 (buf != NULL && buf->b_fnum != count)
	    buf = buf->b_next;
    }
    否则
    {
	整数 help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = NULL;
	当 (count > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    如果 (bp == NULL)
		bp = buf;
	    如果 (dir == FORWARD)
	    {
		buf = buf->b_next;
		如果 (buf == NULL)
		    buf = firstbuf;
	    }
	    否则
	    {
		buf = buf->b_prev;
		如果 (buf == NULL)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    如果 (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--count;
		bp = NULL;	// use this buffer as new starting point
	    }
	    如果 (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		返回 FAIL;
	    }
	}
    }

    如果 (buf == NULL)	    // could not find it
    {
	如果 (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    如果 (!unload)
		semsg(_(e_buffer_nr_does_not_exist), count);
	}
	否则 如果 (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	否则
	    emsg(_(e_cannot_go_before_first_buffer));
	返回 FAIL;
    }
#ifdef FEAT_PROP_POPUP
    如果 ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	返回 OK;
#endif
    如果 (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	返回 FAIL;

    如果 ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), count);
	返回 FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    如果 (unload)
    {
	整数	forward;
	bufref_T bufref;

	如果 (!can_unload_buffer(buf))
	    返回 FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	如果 (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == NULL && !buf->b_p_bl)
	    返回 FAIL;

	如果 ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#如果 defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    如果 ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		如果 (term_job_running(buf->b_term))
		{
		    如果 (term_confirm_stop(buf) == FAIL)
			返回 FAIL;
		}
		否则
# endif
		{
		    dialog_changed(buf, FALSE);
		    如果 (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			返回 FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    如果 (bufIsChanged(buf))
			返回 FAIL;
		}
	    }
	    否则
#endif
	    {
		no_write_message_buf(buf);
		返回 FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	如果 (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    如果 (bp->b_p_bl && bp != buf)
		break;
	如果 (bp == NULL && buf == curbuf)
	    返回 empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	当 (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != NULL))
	{
	    如果 (win_close(curwin, FALSE) == FAIL)
		break;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	如果 (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    如果 (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(NULL, buf, action, FALSE, FALSE, FALSE);
	    返回 OK;
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
	如果 (au_new_curbuf.br_buf != NULL && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	否则 如果 (curwin->w_jumplistlen > 0)
	{
	    整数     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    如果 (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    当 (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		如果 (buf != NULL)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    如果 (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = NULL;
		    否则 如果 (buf->b_ml.ml_mfp == NULL)
		    {
			// skip unloaded buf, but may keep it for later
			如果 (bp == NULL)
			    bp = buf;
			buf = NULL;
		    }
		}
		如果 (buf != NULL)   // found a valid buffer: stop searching
		    break;
		// advance to older entry in jump list
		如果 (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    break;
		如果 (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		如果 (jumpidx == forward)		// List exhausted for sure
		    break;
	    }
	}

	如果 (buf == NULL)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    为 (;;)
	    {
		如果 (buf == NULL)
		{
		    如果 (!forward)	// tried both directions
			break;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    continue;
		}
		// in non-help buffer, try to skip help buffers, and vv
		如果 (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    如果 (buf->b_ml.ml_mfp != NULL)   // found loaded buffer
			break;
		    如果 (bp == NULL)	// remember unloaded buf for later
			bp = buf;
		}
		如果 (forward)
		    buf = buf->b_next;
		否则
		    buf = buf->b_prev;
	    }
	}
	如果 (buf == NULL)	// No loaded buffer, use unloaded one
	    buf = bp;
	如果 (buf == NULL)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		如果 (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    break;
	}
	如果 (buf == NULL)	// Still no buffer, just take one
	{
	    如果 (curbuf->b_next != NULL)
		buf = curbuf->b_next;
	    否则
		buf = curbuf->b_prev;
	    如果 (bt_quickfix(buf)
		    || (buf != NULL && buf != curbuf && buf->b_locked_split))
		buf = NULL;
	}
    }

    如果 (buf == NULL)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	返回 empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    如果 (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != NULL)
	返回 OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    如果 (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	返回 FAIL;
    }
    如果 (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	返回 FAIL;

    // go to current buffer - nothing to do
    如果 (buf == curbuf)
	返回 OK;

    // Check if the current buffer may be abandoned.
    如果 (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#如果 defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	如果 ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    如果 (term_job_running(curbuf->b_term))
	    {
		如果 (term_confirm_stop(curbuf) == FAIL)
		    返回 FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    否则
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		如果 (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    返回 FAIL;

		如果 (bufIsChanged(curbuf))
		{
		    no_write_message();
		    返回 FAIL;
		}
	    }
	}
	否则
#endif
	{
	    no_write_message();
	    返回 FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    如果 (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#如果 defined(FEAT_EVAL)
    如果 (aborting())	    // autocmds may abort script processing
	返回 FAIL;
#endif

    返回 OK;
}

    整数
do_buffer(
    整数		action,
    整数		start,
    整数		dir,		// FORWARD or BACKWARD
    整数		count,		// buffer number or number of buffers
    整数		forceit)	// TRUE when using !
{
    返回 do_buffer_ext(action, start, dir, count,
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
    char *
do_bufdel(
    整数		command,
    char_u	*arg,		// pointer to extra arguments
    整数		addr_count,
    整数		start_bnr,	// first buffer number in a range
    整数		end_bnr,	// buffer nr or last buffer nr in a range
    整数		forceit)
{
    整数		do_current = 0;	// delete current buffer?
    整数		deleted = 0;	// number of buffers deleted
    char	*errormsg = NULL; // return value
    整数		bnr;		// buffer number
    char_u	*p;

    如果 (addr_count == 0)
    {
	(void)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    否则
    {
	如果 (addr_count == 2)
	{
	    如果 (*arg)		// both range and argument is not allowed
		返回 ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	否则	// addr_count == 1
	    bnr = end_bnr;

	为 ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    如果 (bnr == curbuf->b_fnum)
		do_current = bnr;
	    否则 如果 (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    如果 (addr_count == 2)
	    {
		如果 (++bnr > end_bnr)
		    break;
	    }
	    否则    // addr_count == 1
	    {
		arg = skipwhite(arg);
		如果 (*arg == NUL)
		    break;
		如果 (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    如果 (bnr < 0)	    // failed
			break;
		    arg = p;
		}
		否则
		    bnr = getdigits(&arg);
	    }
	}
	如果 (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	如果 (deleted == 0)
	{
	    如果 (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    否则 如果 (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    否则
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (char *)IObuff;
	}
	否则 如果 (deleted >= p_report)
	{
	    如果 (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    否则 如果 (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    否则
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    返回 errormsg;
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
set_curbuf(buf_T *buf, 整数 action)
{
    buf_T	*prevbuf;
    整数		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    整数		valid;

    setpcmark();
    如果 ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    如果 (!apply_autocmds(EVENT_BUFLEAVE, NULL, NULL, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	如果 (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	如果 (unload)
	    close_windows(prevbuf, FALSE);
#如果 defined(FEAT_EVAL)
	如果 (bufref_valid(&prevbufref) && !aborting())
#否则
	如果 (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    如果 (prevbuf == curbuf
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
    如果 ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == NULL)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	如果 (!valid)
	    enter_buffer(lastbuf);
	否则
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	如果 (old_tw != curbuf->b_p_tw)
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
    如果 (VIsual_active
#如果 defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    如果 (curwin->w_buffer != NULL)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    如果 (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    否则
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    如果 (curwin->w_p_diff)
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
    如果 (curbuf->b_ml.ml_mfp == NULL)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	如果 (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, NULL, 0);
    }
    否则
    {
	如果 (!msg_silent && !shortmess(SHM_FILEINFO))
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
    如果 (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    如果 (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    如果 (curbuf->b_kmap_state & KEYMAP_INIT)
	(void)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    如果 (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(void)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#如果 defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    void
do_autochdir(void)
{
    如果 ((starting == 0 || test_autochdir)
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
    如果 (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    否则
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    void
no_write_message(void)
{
#ifdef FEAT_TERMINAL
    如果 (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    否则
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    void
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    如果 (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    否则
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
    整数
curbuf_reusable(void)
{
    返回 (curbuf != NULL
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
    整数		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    如果 (top_file_num == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    如果 (sfname == NULL || mch_stat((char *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    如果 (ffname != NULL && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#否则
		buflist_findname(ffname)
#endif
		) != NULL)
    {
	vim_free(ffname);
	如果 (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? NULL : curwin,
						      lnum, (colnr_T)0, FALSE);

	如果 ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	如果 ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    如果 (!(flags & BLN_DUMMY))
	    {
		如果 (apply_autocmds(EVENT_BUFADD, NULL, NULL, FALSE, buf)
			&& !bufref_valid(&bufref))
		    返回 NULL;
	    }
	}
	返回 buf;
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
    如果 ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	如果 (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    返回 NULL;
	}
#endif
	如果 (!bufref_valid(&bufref))
	    buf = NULL;		// buf was deleted; allocate a new buffer
    }
    如果 (buf != curbuf || curbuf == NULL)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	如果 (buf == NULL)
	{
	    vim_free(ffname);
	    返回 NULL;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	如果 (buf->b_vars == NULL)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    返回 NULL;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    如果 (ffname != NULL)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    如果 ((ffname != NULL && (buf->b_ffname == NULL || buf->b_sfname == NULL))
	    || buf->b_wininfo == NULL)
    {
	如果 (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	否则
	    buf->b_sfname = NULL;
	VIM_CLEAR(buf->b_ffname);
	如果 (buf != curbuf)
	    free_buffer(buf);
	返回 NULL;
    }

    如果 (buf == curbuf)
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
    否则
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = NULL;
	如果 (firstbuf == NULL)		// buffer list is empty
	{
	    buf->b_prev = NULL;
	    firstbuf = buf;
	}
	否则				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	如果 ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((整数 *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    当 (buf->b_prev != NULL && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		如果 (prev->b_next != NULL)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		如果 (buf->b_prev != NULL)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		如果 (lastbuf == buf)
		    lastbuf = prev;
		如果 (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	否则
	    buf->b_fnum = top_file_num++;
	如果 (top_file_num < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    如果 (emsg_silent == 0 && !in_assert_fails)
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
    如果 (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    否则
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    如果 (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    如果 (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	如果 (apply_autocmds(EVENT_BUFNEW, NULL, NULL, FALSE, buf)
		&& !bufref_valid(&bufref))
	    返回 NULL;
	如果 (flags & BLN_LISTED)
	{
	    如果 (apply_autocmds(EVENT_BUFADD, NULL, NULL, FALSE, buf)
		    && !bufref_valid(&bufref))
		返回 NULL;
	}
#ifdef FEAT_EVAL
	如果 (aborting())		// autocmds may abort script processing
	    返回 NULL;
#endif
    }

    返回 buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    void
free_buf_options(
    buf_T	*buf,
    整数		free_p_ff)
{
    如果 (free_p_ff)
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
#如果 defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#如果 defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#如果 defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#如果 defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    如果 (buf->b_p_key != NULL && *buf->b_p_key != NUL
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
    整数
buflist_getfile(
    整数		n,
    linenr_T	lnum,
    整数		options,
    整数		forceit)
{
    buf_T	*buf;
    win_T	*wp = NULL;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    如果 (buf == NULL)
    {
	如果 ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	否则
	    semsg(_(e_buffer_nr_not_found), n);
	返回 FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    如果 (buf == curbuf)
	返回 OK;

    如果 (text_or_buf_locked())
	返回 FAIL;

    // altfpos may be changed by getfile(), get it now
    如果 (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    否则
	col = 0;

    如果 (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	如果 (wp == NULL && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    如果 (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    否则 如果 (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		返回 FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    整数 retval = FAIL;
    如果 (GETFILE_SUCCESS(getfile(buf->b_fnum, NULL, NULL,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	如果 (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    如果 (RedrawingDisabled > 0)
	--RedrawingDisabled;
    返回 retval;
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

    如果 (p_sol)
	curwin->w_cursor.col = 0;
    否则
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
#否则
	    FALSE
#endif
	    );
    如果 (ffname != NULL)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    返回 buf;
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

    如果 (mch_stat((char *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    返回 buflist_findname_stat(ffname, &st);
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
	如果 ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    返回 buf;
    返回 NULL;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    整数
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    整数		unlisted,	// find unlisted buffers
    整数		diffmode UNUSED, // find diff-mode buffers only
    整数		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    整数		match = -1;
    整数		find_listed;
    char_u	*pat;
    char_u	*patend;
    整数		attempt;
    char_u	*p;
    整数		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    如果 ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	如果 (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	否则
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	如果 (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
    否则
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, NULL, FALSE);
	如果 (pat == NULL)
	    返回 -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	为 (;;)
	{
	    为 (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		如果 (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		如果 (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    如果 (regmatch.regprog == NULL)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			返回 -1;
		    }
		    如果 (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != NULL)
		    {
			如果 (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				如果 (wp->w_buffer == buf)
				    break;
			    如果 (wp == NULL)
				continue;
			}
			如果 (match >= 0)		// already found a match
			{
			    match = -2;
			    break;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		如果 (match >= 0)			// found one match
		    break;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    如果 (!unlisted || !find_listed || match != -1)
		break;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    如果 (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    否则 如果 (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    返回 match;
}

#ifdef FEAT_VIMINFO
typedef struct {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    整数
ExpandBufnames(
    char_u	*pat,
    整数		*num_file,
    char_u	***file,
    整数		options)
{
    整数		count;
    buf_T	*buf;
    整数		round;
    char_u	*p;
    char_u	*patc = NULL;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = NULL;
#endif
    整数		fuzzy;
    fuzmatch_str_T  *fuzmatch = NULL;
    regmatch_T	regmatch;
    整数		score = 0;
    整数		to_free = FALSE;

    *num_file = 0;		    // return values in case of FAIL
    *file = NULL;

#ifdef FEAT_DIFF
    如果 ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	返回 FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    如果 (!fuzzy)
    {
	如果 (*pat == '^' && pat[1] != NUL)
	{
	    整数  len = (整数)STRLEN(pat);
	    patc = alloc(len);
	    如果 (patc == NULL)
		返回 FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	否则 如果 (*pat == '^')
	    patc = (char_u *)"";
	否则
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    为 (round = 1; round <= 2; ++round)
    {
	count = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    如果 (!buf->b_p_bl)	// skip unlisted buffers
		continue;
#ifdef FEAT_DIFF
	    如果 (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		如果 (buf == curbuf || !diff_mode_buf(buf))
		    continue;
#endif

	    如果 (!fuzzy)
	    {
		如果 (regmatch.regprog == NULL)
		{
		    // invalid pattern, possibly after recompiling
		    如果 (to_free)
			vim_free(patc);
		    返回 FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    否则
	    {
		p = NULL;
		// first try matching with the short file name
		如果 ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		如果 (p == NULL)
		{
		    // next try matching with the full path file name
		    如果 ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    如果 (p == NULL)
		continue;

	    如果 (round == 1)
	    {
		++count;
		continue;
	    }

	    如果 (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    否则
		p = vim_strsave(p);
	    如果 (p == NULL)
	    {
		如果 (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, count);
		返回 FAIL;
	    }

	    如果 (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		如果 (matches != NULL)
		{
		    matches[count].buf = buf;
		    matches[count].match = p;
		    count++;
		}
		否则
#endif
		    (*file)[count++] = p;
	    }
	    否则
	    {
		fuzmatch[count].idx = count;
		fuzmatch[count].str = p;
		fuzmatch[count].score = score;
		count++;
	    }
	}
	如果 (count == 0)	// no match found, break here
	    break;
	如果 (round == 1)
	{
	    如果 (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, count);
		如果 (*file == NULL)
		{
		    vim_regfree(regmatch.regprog);
		    如果 (to_free)
			vim_free(patc);
		    返回 FAIL;
		}
#ifdef FEAT_VIMINFO
		如果 (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, count);
#endif
	    }
	    否则
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, count);
		如果 (fuzmatch == NULL)
		{
		    *num_file = 0;
		    *file = NULL;
		    返回 FAIL;
		}
	    }
	}
    }

    如果 (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	如果 (to_free)
	    vim_free(patc);
    }

    如果 (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	如果 (matches != NULL)
	{
	    整数 i;
	    如果 (count > 1)
		qsort(matches, count, sizeof(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    如果 (matches[0].buf == curbuf)
	    {
		为 (i = 1; i < count; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[count-1] = matches[0].match;
	    }
	    否则
	    {
		为 (i = 0; i < count; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    否则
    {
	如果 (fuzzymatches_to_strmatches(fuzmatch, file, count, FALSE) == FAIL)
	    返回 FAIL;
    }

    *num_file = count;
    返回 (count == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    static char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    整数		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    如果 (match == NULL && rmp->regprog != NULL)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    返回 match;
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
    整数		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = NULL;
    char_u	*p;

    // extra check for valid arguments
    如果 (name == NULL || rmp->regprog == NULL)
	返回 NULL;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    如果 (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    否则 如果 (rmp->regprog != NULL)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(NULL, name);
	如果 (p != NULL && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    返回 match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(整数 nr)
{
    char_u	key[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    如果 (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((char *)key, sizeof(key), "%x", nr);
    hi = hash_find(&buf_hashtab, key);

    如果 (!HASHITEM_EMPTY(hi))
	返回 (buf_T *)(hi->hi_key
			     - ((unsigned)(curbuf->b_key - (char_u *)curbuf)));
    返回 NULL;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    整数		n,
    整数		fullname,
    整数		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    如果 (buf == NULL)
	返回 NULL;
    返回 home_replace_save(helptail ? buf : NULL,
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
    整数		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	如果 (wip->wi_win == win)
	    break;
    如果 (wip == NULL)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	如果 (wip == NULL)
	    返回;
	wip->wi_win = win;
	如果 (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    否则
    {
	// remove the entry from the list
	如果 (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	否则
	    buf->b_wininfo = wip->wi_next;
	如果 (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	如果 (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    如果 (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    如果 (win != NULL)
	wip->wi_changelistidx = win->w_changelistidx;
    如果 (copy_options && win != NULL)
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
    如果 (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    static 整数
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    如果 (!wip->wi_opt.wo_diff)
	返回 FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	如果 (wip->wi_win == wp)
	    返回 FALSE;
    返回 TRUE;
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
    整数		need_options,
    整数		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	如果 (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    break;

    如果 (wip != NULL)
	返回 wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    如果 (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    如果 (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != NULL
			    && wip->wi_win->w_buffer == buf)))
		break;
    }
    否则
#endif
	wip = buf->b_wininfo;
    返回 wip;
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
    如果 (wip != NULL && wip->wi_win != NULL
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
    否则 如果 (wip != NULL && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    否则
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    如果 (wip != NULL)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    如果 (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    如果 (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    如果 (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	如果 (p_ea)
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
    如果 (wip != NULL)
	返回 &(wip->wi_fpos);
    否则
	返回 &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    返回 buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    void
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    整数		len;
    整数		i;
    整数		ro_char;
    整数		changed_char;
#ifdef FEAT_TERMINAL
    整数		job_running;
    整数		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = NULL, **p;

    如果 (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, sizeof(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    如果 (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_data, (size_t)buflist.ga_len,
		sizeof(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    为 (; buf != NULL && !got_int; buf = buflist_data != NULL
	    ? (++p < buflist_data + buflist.ga_len ? *p : NULL)
	    : buf->b_next)
#否则
    为 (buf = firstbuf; buf != NULL && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	如果 ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
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
	如果 (name != NULL)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	否则
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	如果 (message_filtered(NameBuff))
	    continue;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	如果 (job_running)
	{
	    如果 (job_none_open)
		ro_char = '?';
	    否则
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	否则 如果 (buf->b_term != NULL)
	    ro_char = 'F';
	否则
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	msg_putchar('\n');
	len = (整数)vim_snprintf_safelen((char *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
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
	当 (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	如果 (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (size_t)(IOSIZE - len), buf->b_last_used);
	否则
#endif
	    vim_snprintf((char *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (long)buflist_findlnum(buf));
	msg_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    如果 (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    整数
buflist_name_nr(
    整数		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    如果 (buf == NULL || buf->b_fname == NULL)
	返回 FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    返回 OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    整数
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    整数		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = NULL;
#ifdef UNIX
    stat_T	st;
#endif

    如果 (ffname == NULL || *ffname == NUL)
    {
	// Removing the name.
	如果 (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	否则
	    buf->b_sfname = NULL;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    否则
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	如果 (ffname == NULL)		    // out of memory
	    返回 FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	如果 (mch_stat((char *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	如果 (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#否则
	    obuf = buflist_findname(ffname);
#endif
	如果 (obuf != NULL && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    整数		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		如果 (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    如果 (obuf->b_ml.ml_mfp != NULL || in_use)
	    {
		如果 (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		返回 FAIL;
	    }
	    // delete from the list
	    close_buffer(NULL, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	如果 (ffname == NULL || sfname == NULL)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    返回 FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	如果 (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    如果 (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    否则
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    返回 OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    void
buf_set_name(整数 fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    如果 (buf == NULL)
	返回;

    如果 (buf->b_sfname != buf->b_ffname)
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
    如果 (buf->b_ml.ml_mfp != NULL)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    如果 (buf->b_term != NULL)
	term_clear_status_text(buf->b_term);
#endif

    如果 (curwin->w_buffer == buf)
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
    如果 (buf != NULL && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    返回 buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    整数		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    如果 (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	如果 (errmsg)
	    emsg(_(e_no_alternate_file));
	返回 NULL;
    }
    返回 fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    整数
buflist_add(char_u *fname, 整数 flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, NULL, (linenr_T)0, flags);
    如果 (buf != NULL)
	返回 buf->b_fnum;
    返回 0;
}

#如果 defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    void
buflist_slash_adjust(void)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	如果 (bp->b_ffname != NULL)
	    slash_adjust(bp->b_ffname);
	如果 (bp->b_sfname != NULL)
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
    整数
otherfile(char_u *ffname)
{
    返回 otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , NULL
#endif
	    );
}

    static 整数
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    如果 (ffname == NULL || *ffname == NUL || buf->b_ffname == NULL)
	返回 TRUE;
    如果 (fnamecmp(ffname, buf->b_ffname) == 0)
	返回 FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	如果 (stp == NULL)
	{
	    如果 (!buf->b_dev_valid || mch_stat((char *)ffname, &st) < 0)
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
	如果 (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    如果 (buf_same_ino(buf, stp))
		返回 FALSE;
	}
    }
#endif
    返回 TRUE;
}

#如果 defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    void
buf_setino(buf_T *buf)
{
    stat_T	st;

    如果 (buf->b_fname != NULL && mch_stat((char *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    否则
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    static 整数
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    返回 (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    void
fileinfo(
    整数 fullname,	    // when non-zero print full path
    整数 shorthelp,
    整数	dont_truncate)
{
    char_u	*name;
    char	*buffer;
    size_t	bufferlen = 0;

    buffer = alloc(IOSIZE);
    如果 (buffer == NULL)
	返回;

    如果 (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    如果 (name != NULL)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    否则
    {
	如果 (!fullname && curbuf->b_fname != NULL)
	    name = curbuf->b_fname;
	否则
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : NULL, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (整数)bufferlen, TRUE);
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

    如果 (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_lines_msg));
    否则 如果 (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_line_count),
	    (long)curbuf->b_ml.ml_line_count,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_line_count));
    否则
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
		   (整数)curwin->w_cursor.col + 1, (整数)curwin->w_virtcol + 1);
    }

    (void)append_arg_number(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    如果 (dont_truncate)
    {
	整数	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	msg_start();
	n = msg_scroll;
	msg_scroll = TRUE;
	msg(buffer);
	msg_scroll = n;
    }
    否则
    {
	char	*p = msg_trunc_attr(buffer, FALSE, 0);
	如果 (restart_edit != 0 || (msg_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_msg((char_u *)p, 0);
    }

    vim_free(buffer);
}

    整数
col_print(
    char_u  *buf,
    size_t  buflen,
    整数	    col,
    整数	    vcol)
{
    如果 (col == vcol)
	返回 (整数)vim_snprintf_safelen((char *)buf, buflen, "%d", col);

    返回 (整数)vim_snprintf_safelen((char *)buf, buflen, "%d-%d", col, vcol);
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
    整数		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    如果 (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	返回;
    }

    need_maketitle = FALSE;
    如果 (!p_title && !p_icon && lasttitle == NULL && lasticon == NULL)
	返回;  // nothing to do

    如果 (p_title)
    {
	整数 maxlen = 0;

	如果 (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    如果 (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	如果 (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    如果 (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, sizeof(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, NULL, NULL, NULL);
	    否则
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	否则
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (sizeof(buf) - 100)
#define SPACE_FOR_DIR   (sizeof(buf) - 20)
#define SPACE_FOR_ARGNR (sizeof(buf) - 10)  // at least room for " - VIM"

	    // file name
	    如果 (curbuf->b_fname == NULL)
		buflen = vim_snprintf_safelen((char *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    否则 如果 (curbuf->b_term != NULL)
		buflen = vim_snprintf_safelen((char *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    否则
	    {
		buflen = vim_snprintf_safelen((char *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != NULL)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    如果 (curbuf->b_term == NULL)
#endif
	    {
		switch (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    case 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (char *)buf + buflen,
			    sizeof(buf) - buflen, " +");
			break;
		    case 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (char *)buf + buflen,
			    sizeof(buf) - buflen, " =");
			break;
		    case 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (char *)buf + buflen,
			    sizeof(buf) - buflen, " =+");
			break;
		    case 4:
		    case 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (char *)buf + buflen,
			    sizeof(buf) - buflen, " -");
			break;
		    case 5:
		    case 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (char *)buf + buflen,
			    sizeof(buf) - buflen, " -+");
			break;
		    default:
			break;
		}
	    }

	    // path (surrounded by '()')
	    如果 (curbuf->b_fname != NULL
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == NULL
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((char *)buf + buflen,
		    sizeof(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (整数)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		如果 (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		如果 (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((char *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		否则
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    如果 (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((char *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != NULL)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    否则
			buflen += vim_snprintf_safelen((char *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_number(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((char *)buf + buflen,
		sizeof(buf) - buflen, " - %s",
#如果 defined(FEAT_CLIENTSERVER)
		(serverName != NULL)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    如果 (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		如果 (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, sizeof(buf));
	    }
	}
    }
    mustset = value_changed(title_str, &lasttitle);

    如果 (p_icon)
    {
	icon_str = buf;
	如果 (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    如果 (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, sizeof(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, NULL, NULL,
				 NULL);
	    否则
#endif
		icon_str = p_iconstring;
	}
	否则
	{
	    char_u  *name;
	    整数	    namelen;

	    name = buf_spname(curbuf);
	    如果 (name == NULL)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (整数)STRLEN(name);
	    如果 (namelen > 100)
	    {
		namelen -= 100;
		如果 (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, sizeof(buf));
	}
    }

    mustset |= value_changed(icon_str, &lasticon);

    如果 (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    static 整数
value_changed(char_u *str, char_u **last)
{
    如果 ((str == NULL) != (*last == NULL)
	    || (str != NULL && *last != NULL && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	如果 (str == NULL)
	{
	    *last = NULL;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	否则
	{
	    *last = vim_strsave(str);
	    返回 TRUE;
	}
    }
    返回 FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    void
resettitle(void)
{
    mch_settitle(lasttitle, lasticon);
}

#如果 defined(EXITFREE)
    void
free_titles(void)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#如果 defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
typedef struct
{
    char_u	*stl_start;
    整数		stl_minwid;
    整数		stl_maxwid;
    enum {
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
static 整数	       *stl_groupitem = NULL;
static stl_hlrec_T     *stl_hltab = NULL;
static stl_hlrec_T     *stl_tabtab = NULL;
static stl_clickrec_T  *stl_clicktab = NULL;
static 整数		*stl_separator_locations = NULL;

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
    整数
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    整数		opt_scope,	// scope for "opt_name"
    整数		fillchar,
    整数		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    返回 build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    NULL, NULL);
}

    整数
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    整数		opt_scope,	// scope for "opt_name"
    整数		fillchar,
    整数		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    整数		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    返回 build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    NULL, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    整数
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    整数		opt_scope,	// scope for "opt_name"
    整数		fillchar,
    整数		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    整数		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    返回 build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    NULL, carry_hl);
}
# endif

    整数
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    整数		opt_scope)	// scope for "opt_name"
{
    整数 rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (void)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, sizeof(buf), &fmt,
	    opt_name, opt_scope, 0, 0, NULL, NULL, NULL, &rendered_height,
	    NULL);
    --emsg_off;
    返回 rendered_height;
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
static 整数
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    整数		opt_scope,	// scope for "opt_name"
    整数		fillchar,
    整数		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    整数		*rendered_height,   // return: stl rendered height (can be NULL)
    整数		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    整数		byteval;
# ifdef FEAT_EVAL
    整数		use_sandbox;
    整数		save_VIsual_active;
# endif
    整数		empty_line;
    long	l;
    long	n;
    整数		prevchar_isflag;
    整数		prevchar_isitem;
    整数		itemisflag;
    整数		fillable;
    char_u	*str;
    long	num;
    整数		width;
    整数		itemcnt;
    整数		curitem;
    整数		groupdepth;
# ifdef FEAT_EVAL
    整数		evaldepth;
# endif
    整数		minwid;
    整数		maxwid;
    整数		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    整数		save_redraw_not_allowed = redraw_not_allowed;
    整数		save_KeyTyped = KeyTyped;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    整数		did_emsg_before = did_emsg;
    整数		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    如果 (updating_screen)
	redraw_not_allowed = TRUE;

    如果 (stl_items == NULL)
    {
	stl_items = ALLOC_MULT(stl_item_T, stl_items_len);
	stl_groupitem = ALLOC_MULT(整数, stl_items_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_items_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_items_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_items_len + 1);

	stl_separator_locations = ALLOC_MULT(整数, stl_items_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    如果 (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_NUMBER;
	tv.vval.v_number = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	如果 (usefmt == NULL)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    如果 (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    如果 (lnum > wp->w_buffer->b_ml.ml_line_count)
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
    如果 (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    否则
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    如果 (carry_hl != NULL && *carry_hl != 0)
    {
	stl_items[curitem].stl_type = Highlight;
	stl_items[curitem].stl_start = p;
	stl_items[curitem].stl_minwid = *carry_hl;
	stl_items[curitem].stl_clickfunc = NULL;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    为 (s = usefmt; *s != NUL; )
    {
	如果 (curitem == (整数)stl_items_len)
	{
	    size_t	new_len = stl_items_len * 3 / 2;

	    stl_item_T *new_items =
			  vim_realloc(stl_items, sizeof(stl_item_T) * new_len);
	    如果 (new_items == NULL)
		break;
	    stl_items = new_items;

	    整数 *new_groupitem =
			     vim_realloc(stl_groupitem, sizeof(整数) * new_len);
	    如果 (new_groupitem == NULL)
		break;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  sizeof(stl_hlrec_T) * (new_len + 1));
	    如果 (new_hlrec == NULL)
		break;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  sizeof(stl_hlrec_T) * (new_len + 1));
	    如果 (new_hlrec == NULL)
		break;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      sizeof(stl_clickrec_T) * (new_len + 1));
	    如果 (new_clickrec == NULL)
		break;
	    stl_clicktab = new_clickrec;

	    整数 *new_separator_locs = vim_realloc(stl_separator_locations,
					    sizeof(整数) * new_len);
	    如果 (new_separator_locs == NULL)
		break;
	    stl_separator_locations = new_separator_locs;

	    stl_items_len = new_len;
	}

	stl_items[curitem].stl_clickfunc = NULL;

	如果 (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	当 (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    如果 (*s == '\n' || *s == '\r')
	    {
		如果 (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    goto find_linebreak;
		}
		否则 如果 (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	如果 (*s == NUL || p + 1 >= out + outlen)
	    break;

	/*
	 * Handle one '%' item.
	 */
	s++;
	如果 (*s == NUL)  // ignore trailing %
	    break;

	如果 (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    如果 (s[1] == ']')
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
	    如果 (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		如果 (rb != NULL)
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
	如果 (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    如果 (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		break;
	    }
	    否则 如果 (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	如果 (*s == '%')
	{
	    如果 (p + 1 >= out + outlen)
		break;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    continue;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	如果 (*s == STL_SEPARATE)
	{
	    s++;
	    如果 (groupdepth > 0)
		continue;
	    stl_items[curitem].stl_type = Separate;
	    stl_items[curitem++].stl_start = p;
	    continue;
	}
	如果 (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_items[curitem].stl_type = Trunc;
	    stl_items[curitem++].stl_start = p;
	    continue;
	}
	如果 (*s == ')')
	{
	    char_u  *t;

	    s++;
	    如果 (groupdepth < 1)
		continue;
	    groupdepth--;

	    t = stl_items[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    如果 (curitem > stl_groupitem[groupdepth] + 1
		    && stl_items[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		整数 group_start_userhl = 0;
		整数 group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		为 (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    如果 (stl_items[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_items[n].stl_minwid;
			break;
		    }
		}
		为 (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    如果 (stl_items[n].stl_type == Normal)
			break;
		    如果 (stl_items[n].stl_type == Highlight)
			group_end_userhl = stl_items[n].stl_minwid;
		}
		如果 (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    为 (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			如果 (stl_items[n].stl_type == Highlight)
			    stl_items[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			如果 (stl_items[n].stl_type == TabPage)
			    stl_items[n].stl_start = p;
		    }
		}
	    }
	    如果 (l > stl_items[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		如果 (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    当 (l >= stl_items[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		否则
		    n = (long)(p - t) - stl_items[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		整数 minwid_fixed = MIN(stl_items[stl_groupitem[groupdepth]].stl_minwid,
				       stl_items[stl_groupitem[groupdepth]].stl_maxwid);
		当 (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		为 (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_items[l].stl_start -= n - 1;
		    如果 (stl_items[l].stl_start < t)
			stl_items[l].stl_start = t;
		}
	    }
	    否则 如果 (abs(stl_items[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_items[stl_groupitem[groupdepth]].stl_minwid;
		整数 fillchar_len = MB_CHAR2LEN(fillchar);
		如果 (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    当 (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		否则
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    如果 (p + l >= out + outlen)
		    {
			n = (long)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    为 ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    为 (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_items[n].stl_start += l;
		}
	    }
	    continue;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	如果 (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	如果 (*s == '-')
	{
	    s++;
	    l = -1;
	}
	如果 (VIM_ISDIGIT(*s))
	{
	    minwid = (整数)getdigits(&s);
	    如果 (minwid < 0)	// overflow
		minwid = 0;
	}
	如果 (*s == STL_USER_HL)
	{
	    stl_items[curitem].stl_type = Highlight;
	    stl_items[curitem].stl_start = p;
	    stl_items[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    continue;
	}
	如果 (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    如果 (*s == STL_TABCLOSENR)
	    {
		如果 (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    为 (n = curitem - 1; n >= 0; --n)
			如果 (stl_items[n].stl_type == TabPage
					       && stl_items[n].stl_minwid >= 0)
			{
			    minwid = stl_items[n].stl_minwid;
			    break;
			}
		}
		否则
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
	如果 (*s == '.')
	{
	    s++;
	    如果 (VIM_ISDIGIT(*s))
	    {
		maxwid = (整数)getdigits(&s);
		如果 (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	整数 raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	如果 (*s == '(')
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
	如果 (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    continue;
	}
# endif
	如果 (vim_strchr(STL_ALL, *s) == NULL)
	{
	    如果 (*s == NUL)  // can happen with "%0"
		break;
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
	case STL_FILEPATH:
	case STL_FULLPATH:
	case STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    如果 (name != NULL)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    否则
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    如果 (opt != STL_FILENAME)
		str = NameBuff;
	    否则
		str = gettail(NameBuff);
	    break;
	}

	case STL_VIM_EXPR: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    整数	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    如果 (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    当 ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    如果 (*s != '}')	// missing '}' or out of space
		break;
	    s++;
	    如果 (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    否则
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((char *)buf_tmp, sizeof(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((char *)buf_tmp, sizeof(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    如果 (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    如果 (!zeropad && str != NULL && *str != NUL)
	    {
		如果 (*skipdigits(str) == NUL)
		{
		    num = atoi((char *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    如果 (reevaluate && str != NULL && *str != NUL
		    && strchr((const char *)str, '%') != NULL
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((const char *)str);
		size_t fmt_length = strlen((const char *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * sizeof(char_u));

		如果 (new_fmt != NULL)
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

		    如果 (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    continue;
		}
	    }
# endif
	    break;
	}
	case STL_LINE:
	    num = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    break;

	case STL_NUMLINES:
	    num = wp->w_buffer->b_ml.ml_line_count;
	    break;

	case STL_COLUMN:
	    num = (State & MODE_INSERT) == 0 && empty_line
					       ? 0 : (整数)wp->w_cursor.col + 1;
	    break;

	case STL_VIRTCOL:
	case STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    如果 (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_INSERT) == 0
			       && empty_line ? 0 : (整数)wp->w_cursor.col + 1)))
		break;
	    num = (long)virtcol;
	    break;
	}

	case STL_PERCENTAGE:
	    num = calc_percentage((long)wp->w_cursor.lnum, (long)wp->w_buffer->b_ml.ml_line_count);
	    break;

	case STL_ALTPERCENT:
	    str = buf_tmp;
	    (void)get_rel_pos(wp, str, TMPLEN);
	    break;

	case STL_SHOWCMD:
	    如果 (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    break;

	case STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    如果 (append_arg_number(wp, buf_tmp, sizeof(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    break;

	case STL_KEYMAP:
	    fillable = FALSE;
	    如果 (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    break;
	case STL_PAGENUM:
# 如果 defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    num = printer_page_num;
# 否则
	    num = 0;
# endif
	    break;

	case STL_BUFNO:
	    num = wp->w_buffer->b_fnum;
	    break;

	case STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	case STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_line_or_offset(wp->w_buffer, wp->w_cursor.lnum, NULL);
	    num = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_INSERT) == 0 && empty_line
				? 0 : (整数)wp->w_cursor.col);
# endif
	    break;

	case STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	case STL_BYTEVAL:
	    num = byteval;
	    如果 (num == NL)
		num = 0;
	    否则 如果 (num == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		num = NL;
	    break;

	case STL_ROFLAG:
	case STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    如果 (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    break;

	case STL_HELPFLAG:
	case STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    如果 (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    break;

	case STL_FILETYPE:
	    如果 (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((char *)buf_tmp, sizeof(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    break;

	case STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    如果 (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((char *)buf_tmp, sizeof(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		为 (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    break;

# 如果 defined(FEAT_QUICKFIX)
	case STL_PREVIEWFLAG:
	case STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    如果 (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    break;

	case STL_QUICKFIX:
	    如果 (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(msg_loclist)
			    : _(msg_qflist));
	    break;
# endif

	case STL_MODIFIED:
	case STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    switch ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		case 2: str = (char_u *)"[+]"; break;
		case 3: str = (char_u *)",+"; break;
		case 4: str = (char_u *)"[-]"; break;
		case 5: str = (char_u *)",-"; break;
		case 6: str = (char_u *)"[+-]"; break;
		case 7: str = (char_u *)",+-"; break;
	    }
	    break;

	case STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		当 (*s != '#' && *s != NUL)
		    ++s;
		如果 (*s == '#')
		{
		    stl_items[curitem].stl_type = Highlight;
		    stl_items[curitem].stl_start = p;
		    stl_items[curitem].stl_minwid = -syn_namen2id(t, (整数)(s - t));
		    curitem++;
		}
		如果 (*s != NUL)
		    ++s;
		continue;
	    }

	case STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    如果 (*s == ']')
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
	    如果 (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		如果 (rb != NULL)
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

	case STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    continue;
	}

	stl_items[curitem].stl_start = p;
	stl_items[curitem].stl_type = Normal;
	如果 (str != NULL && *str)
	{
	    char_u  *t = str;

	    如果 (itemisflag)
	    {
		如果 ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    如果 (l > 0)
		prevchar_isitem = TRUE;
	    如果 (l > maxwid)
	    {
		当 (l >= maxwid)
		    如果 (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    否则
			l -= byte2cells(*t++);
		如果 (p + 1 >= out + outlen)
		    break;
		*p++ = '<';
	    }
	    如果 (minwid > 0)
	    {
		为 (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    如果 (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    否则
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    否则
		minwid *= -1;
	    为 (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		如果 (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		否则
		    *p++ = *t;
	    }
	    为 (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	否则 如果 (num >= 0)
	{
	    整数	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    如果 (p + 20 >= out + outlen)
		break;		// not sufficient space
	    prevchar_isitem = TRUE;
	    如果 (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    如果 (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    为 (n = num, l = 1; n >= nbase; n /= nbase)
		l++;
	    如果 (opt == STL_VIRTCOL_ALT)
		l++;

	    如果 (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		当 (l-- > maxwid)
		    num /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((char *)p, outlen - (p - out),
		    (char *)nstr, 0, num, n);
	    }
	    否则
		p += vim_snprintf_safelen((char *)p, outlen - (p - out),
		    (char *)nstr, minwid, num);
	}
	否则
	    stl_items[curitem].stl_type = Empty;

	如果 (num >= 0 || (!itemisflag && str != NULL && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	如果 (opt == STL_VIM_EXPR)
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
    如果 (carry_hl != NULL)
    {
	整数 last_hl = 0;
	为 (l = 0; l < itemcnt; l++)
	    如果 (stl_items[l].stl_type == Highlight)
		last_hl = stl_items[l].stl_minwid;
	*carry_hl = last_hl;
    }

    如果 (mode == STL_MODE_MULTI
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
	如果 (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = strlen((char *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    否则
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	如果 (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    如果 (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	为 (l = 0; l < itemcnt; l++)
	    如果 (stl_items[l].stl_type == ClickFunc)
		vim_free(stl_items[l].stl_clickfunc);
	如果 (rendered_height != NULL)
	    *rendered_height = rheight;
	返回 0;
    }

    width = vim_strsize(out);
    如果 (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	如果 (itemcnt == 0)
	    s = out;
	否则
	{
	    为 ( ; l < itemcnt; l++)
		如果 (stl_items[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_items[l].stl_start;
		    break;
		}
	    如果 (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_items[0].stl_start;
		l = 0;
	    }
	}

	如果 (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    如果 (has_mbyte)
	    {
		s = out;
		width = 0;
		为 (;;)
		{
		    width += ptr2cells(s);
		    如果 (width >= maxwidth)
			break;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		当 (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    否则
		s = out + maxwidth - 1;
	    为 (l = 0; l < itemcnt; l++)
		如果 (stl_items[l].stl_start > s)
		    break;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	否则
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    当 (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(end - p) + 1);	// +1 for NUL
	    end -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    为 (; l < itemcnt; l++)
	    {
		如果 (stl_items[l].stl_start - n >= s)
		    stl_items[l].stl_start -= n;
		否则
		    stl_items[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    当 (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    否则 如果 (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	整数 num_separators = 0;

	为 (l = 0; l < itemcnt; l++)
	{
	    如果 (stl_items[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[num_separators] = l;
		num_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	如果 (num_separators)
	{
	    整数 standard_spaces;
	    整数 final_spaces;

	    standard_spaces = (maxwidth - width) / num_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (num_separators - 1);
	    为 (l = 0; l < num_separators; l++)
	    {
		整数 dislocation = (l == (num_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_items[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		为 (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		为 (整数 i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_items[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    如果 (hltab != NULL)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	为 (l = 0; l < itemcnt; l++)
	{
	    如果 (stl_items[l].stl_type == Highlight)
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
    如果 (tabtab != NULL)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	为 (l = 0; l < itemcnt; l++)
	{
	    如果 (stl_items[l].stl_type == TabPage)
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
    如果 (clicktab != NULL)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	为 (l = 0; l < itemcnt; l++)
	{
	    如果 (stl_items[l].stl_type == ClickFunc)
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
    否则
    {
	// Free click function names when caller doesn't need them.
	为 (l = 0; l < itemcnt; l++)
	    如果 (stl_items[l].stl_type == ClickFunc)
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
    如果 (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    返回 width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    整数
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    整数		buflen)
{
    long	above; // number of lines above window
    long	below; // number of lines below window

    如果 (buflen < 3) // need at least 3 chars for writing
	返回 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    如果 (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_line_count - wp->w_botline + 1;
    如果 (below <= 0)
	返回 (整数)vim_snprintf_safelen((char *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    如果 (above <= 0)
	返回 (整数)vim_snprintf_safelen((char *)buf, buflen,
	    "%s", _("Top"));

    整数 perc = calc_percentage(above, above + below);
    char tmp[8];
    // localized percentage value
    vim_snprintf(tmp, sizeof(tmp), _("%d%%"), perc);
    返回 (整数)vim_snprintf_safelen((char *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    static 整数
append_arg_number(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    整数		add_file)	// Add "file" before the arg number
{
    如果 (ARGCOUNT <= 1)		// nothing to do
	返回 0;

    char *msg;
    switch ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	case 0: msg = _(" (%d of %d)"); break;
	case 1: msg = _(" ((%d) of %d)"); break;
	case 2: msg = _(" (file %d of %d)"); break;
	case 3: msg = _(" (file (%d) of %d)"); break;
    }

    返回 (整数)vim_snprintf_safelen((char *)buf, buflen, msg,
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
    返回 FullName_save(fname, TRUE);
#否则
    如果 (!vim_isAbsName(fname)
	    || strstr((char *)fname, "..") != NULL
	    || strstr((char *)fname, "//") != NULL
# ifdef BACKSLASH_IN_FILENAME
	    || strstr((char *)fname, "\\\\") != NULL
# endif
# 如果 defined(MSWIN)
	    || vim_strchr(fname, '~') != NULL
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	返回 FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    如果 (fname != NULL)
	fname_case(fname, 0);	// set correct case for file name
# endif

    返回 fname;
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
    如果 (*ffname == NULL)	    // no file name given, nothing to do
	返回;
    如果 (*sfname == NULL)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    如果 (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	如果 (rfname != NULL)
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
    整数		split_ret = OK;
    整数		p_ea_save;
    整数		open_wins = 0;
    整数		r;
    整数		count;		// Maximum number of windows to open.
    整数		all;		// When TRUE also load inactive buffers.
    整数		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    如果 (eap->addr_count == 0)	// make as many windows as possible
	count = 9999;
    否则
	count = eap->line2;	// make as many windows as specified
    如果 (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    否则
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
    如果 (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    为 (;;)
    {
	tpnext = curtab->tp_next;
	为 (wp = firstwin; wp != NULL; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    如果 ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		如果 (win_close(wp, FALSE) == FAIL)
		    break;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    否则
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	如果 (had_tab == 0 || tpnext == NULL)
	    break;
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
    为 (buf = firstbuf; buf != NULL && open_wins < count; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	如果 ((!all && buf->b_ml.ml_mfp == NULL) || !buf->b_p_bl)
	    continue;

	如果 (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    如果 (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    否则
		wp = NULL;
	}
	否则
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		如果 (wp->w_buffer == buf)
		    break;
	    // If the buffer already has a window, move it
	    如果 (wp != NULL)
		win_move_after(wp, curwin);
	}

	如果 (wp == NULL && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    如果 (split_ret == FAIL)
		continue;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    如果 (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		break;
	    }
	    如果 (swap_exists_action == SEA_QUIT)
	    {
#如果 defined(FEAT_EVAL)
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

#如果 defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    否则
		handle_swap_exists(NULL);
	}

	ui_breakcheck();
	如果 (got_int)
	{
	    (void)vgetc();	// only break the file loading, not the rest
	    break;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	如果 (aborting())
	    break;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	如果 (had_tab > 0 && tabpage_index(NULL) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    为 (wp = lastwin; open_wins > count; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	如果 (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	否则 如果 (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	否则
	{
	    wp = wp->w_prev;
	    如果 (wp == NULL)
		break;
	}
    }
}


static 整数  chk_modeline(linenr_T, 整数);

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
do_modelines(整数 flags)
{
    linenr_T	lnum;
    整数		nmlines;
    static 整数	entered = 0;

    如果 (!curbuf->b_p_ml || (nmlines = (整数)p_mls) == 0)
	返回;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    如果 (entered)
	返回;

    ++entered;
    为 (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_line_count && lnum <= nmlines;
								       ++lnum)
	如果 (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    为 (lnum = curbuf->b_ml.ml_line_count; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_line_count - nmlines; --lnum)
	如果 (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#include "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    static 整数
chk_modeline(
    linenr_T	lnum,
    整数		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*line_end;		// point to the end of the line
    char_u	*e;
    整数		prev;
    整数		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    line_end = s + ml_get_len(lnum);
    为 (; *s != NUL; ++s)
    {
	如果 (prev == -1 || vim_isspace(prev))
	{
	    如果 ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		break;
	    // Accept both "vim" and "Vim".
	    如果 ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		整数 vers;

		如果 (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		否则
		    e = s + 3;
		vers = getdigits(&e);
		如果 (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    break;
	    }
	}
	prev = *s;
    }

    如果 (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	整数	end;

	do				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	当 (s[-1] != ':');

	len = (size_t)(line_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	如果 (linecopy == NULL)
	    返回 FAIL;

	line_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_push(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	当 (end == FALSE)
	{
	    s = skipwhite(s);
	    如果 (*s == NUL)
		break;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    为 (e = s; *e != ':' && *e != NUL; ++e)
		如果 (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(line_end - (e + 1)) + 1);	// +1 for NUL
		    --line_end;
		}
	    如果 (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    如果 (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		如果 (*e != ':')		// no terminating ':'?
		    break;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    如果 (*s != NUL)		// skip over an empty "::"
	    {
		整数 secure_save = secure;
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
		如果 (retval == FAIL)		// stop if error found
		    break;
	    }
	    s = (e == line_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_pop();
	vim_free(linecopy);
    }
    返回 retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    整数
bt_normal(buf_T *buf)
{
    返回 buf != NULL && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    整数
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    返回 buf != NULL && buf->b_p_bt[0] == 'q';
#否则
    返回 FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    整数
bt_terminal(buf_T *buf UNUSED)
{
#如果 defined(FEAT_TERMINAL)
    返回 buf != NULL && buf->b_p_bt[0] == 't';
#否则
    返回 FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    整数
bt_help(buf_T *buf)
{
    返回 buf != NULL && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    整数
bt_prompt(buf_T *buf)
{
    返回 buf != NULL && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#如果 defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    整数
bt_popup(buf_T *buf)
{
    返回 buf != NULL && buf->b_p_bt != NULL
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    整数
bt_nofilename(buf_T *buf)
{
    返回 buf != NULL && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    static 整数
bt_nofileread(buf_T *buf)
{
    返回 buf != NULL && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#如果 defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    整数
bt_nofile(buf_T *buf)
{
    返回 buf != NULL && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    整数
bt_dontwrite(buf_T *buf)
{
    返回 buf != NULL && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    整数
bt_dontwrite_msg(buf_T *buf)
{
    如果 (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	返回 TRUE;
    }
    返回 FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    整数
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    switch (buf->b_p_bh[0])
    {
	case 'u':		    // "unload"
	case 'w':		    // "wipe"
	case 'd': 返回 FALSE;	    // "delete"
	case 'h': 返回 TRUE;	    // "hide"
    }
    返回 (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#如果 defined(FEAT_QUICKFIX)
    如果 (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	如果 (buf->b_fnum == qf_stack_get_bufnr())
	    返回 (char_u *)_(msg_qflist);
	否则
	    返回 (char_u *)_(msg_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    如果 (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	如果 (buf->b_term != NULL)
	    返回 term_get_status_text(buf->b_term);
#endif
	如果 (buf->b_fname != NULL)
	    返回 buf->b_fname;
	如果 (buf == cmdwin_buf)
	    返回 (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	如果 (bt_prompt(buf))
	    返回 (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	如果 (bt_popup(buf))
	    返回 (char_u *)_("[Popup]");
#endif
	返回 (char_u *)_("[Scratch]");
    }

    如果 (buf->b_fname == NULL)
	返回 buf_get_fname(buf);
    返回 NULL;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    如果 (buf->b_fname == NULL)
	返回 (char_u *)_("[No Name]");
    返回 buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    void
set_buflisted(整数 on)
{
    如果 (on == curbuf->b_p_bl)
	返回;

    curbuf->b_p_bl = on;
    如果 (on)
	apply_autocmds(EVENT_BUFADD, NULL, NULL, FALSE, curbuf);
    否则
	apply_autocmds(EVENT_BUFDELETE, NULL, NULL, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    整数
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    整数		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(NULL, NULL, (linenr_T)1, BLN_DUMMY);
    如果 (newbuf == NULL)
	返回 TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    如果 (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	返回 TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    如果 (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	返回 TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    如果 (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	如果 (buf->b_ml.ml_line_count == curbuf->b_ml.ml_line_count)
	{
	    differ = FALSE;
	    为 (lnum = 1; lnum <= curbuf->b_ml.ml_line_count; ++lnum)
		如果 (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    break;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    如果 (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    返回 differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    void
wipe_buffer(
    buf_T	*buf,
    整数		aucmd)	    // When TRUE trigger autocommands.
{
    如果 (buf->b_fnum == top_file_num - 1)
	--top_file_num;

    如果 (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(NULL, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    如果 (!aucmd)
	unblock_autocmds();
}
