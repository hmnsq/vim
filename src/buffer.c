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

#شامل "vim.h"


#ifdef FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_DEPTH 100
#endif

ایستا تهی	enter_buffer(buf_T *buf);
ایستا تهی	buflist_getfpos(تهی);
ایستا char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, صحیح ignore_case);
ایستا char_u	*fname_match(regmatch_T *rmp, char_u *name, صحیح ignore_case);
#ifdef UNIX
ایستا buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
ایستا صحیح	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
ایستا صحیح	buf_same_ino(buf_T *buf, stat_T *stp);
#وگرنه
ایستا صحیح	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
ایستا صحیح	مقدار_changed(char_u *str, char_u **last);
#اگر defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
ایستا صحیح build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, صحیح opt_scope, صحیح fillchar, صحیح maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, صحیح *lbreaks, صحیح *carry_hl);
#endif
ایستا صحیح	append_arg_عدد(win_T *wp, char_u *buf, size_t buflen, صحیح add_file);
ایستا تهی	free_buffer(buf_T *);
ایستا تهی	free_buffer_stuff(buf_T *buf, صحیح free_options);
ایستا صحیح	bt_nofileread(buf_T *buf);
ایستا صحیح	do_buffer_ext(صحیح action, صحیح start, صحیح dir, صحیح شمار, صحیح flags);

#ifdef UNIX
# define dev_T dev_t
#وگرنه
# define dev_T unsigned
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    برای ((buf) = lastbuf; (buf) != پوچ; (buf) = (buf)->b_prev)

#اگر defined(FEAT_QUICKFIX)
ایستا نویسه *پیام_loclist = N_("[Location List]");
ایستا نویسه *پیام_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
ایستا صحیح	buf_free_شمار = 0;

ایستا صحیح	بالاترین_file_رقم = 1;	// highest file number
ایستا garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    ایستا تهی
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"اگر exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    ایستا صحیح
calc_percentage(long part, long whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    بازگشت (part > 1000000L)
	? (صحیح)(part / (whole / 100L))
	: (صحیح)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    صحیح
get_highest_fnum(تهی)
{
    بازگشت بالاترین_file_رقم - 1;
}

/*
 * Read data from buffer for retrying.
 */
    ایستا صحیح
read_buffer(
    صحیح		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    صحیح		flags)		    // extra flags for readfile()
{
    صحیح		retval = OK;
    linenr_T	سطر_شمار;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    سطر_شمار = curbuf->b_ml.ml_سطر_شمار;
    retval = readfile(
	    read_stdin ? پوچ : curbuf->b_ffname,
	    read_stdin ? پوچ : curbuf->b_fname,
	    سطر_شمار, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    اگر (retval == OK)
    {
	// Delete the binary lines.
	وقتی (--سطر_شمار >= 0)
	    ml_delete((linenr_T)1);
    }
    وگرنه
    {
	// Delete the converted lines.
	وقتی (curbuf->b_ml.ml_سطر_شمار > سطر_شمار)
	    ml_delete(سطر_شمار);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    اگر (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	اگر (!readonlymode && !BUFEMPTY())
	    changed();
	وگرنه اگر (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	اگر (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, پوچ, پوچ, FALSE,
							      curbuf, &retval);
#وگرنه
	    apply_autocmds(EVENT_STDINREADPOST, پوچ, پوچ, FALSE, curbuf);
#endif
	}
    }
    بازگشت retval;
}

#اگر defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    تهی
buffer_ensure_loaded(buf_T *buf)
{
    اگر (buf->b_ml.ml_mfp != پوچ)
	بازگشت;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    اگر (curbuf == buf)
    {
	اگر (swap_exists_action != SEA_READONLY)
	    swap_exists_action = SEA_NONE;
	open_buffer(FALSE, پوچ, 0);
	aucmd_restbuf(&aco);
    }
}
#endif

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
open_buffer(
    صحیح		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    صحیح		flags_arg)	    // extra flags for readfile()
{
    صحیح		flags = flags_arg;
    صحیح		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    صحیح		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    اگر (readonlymode && curbuf->b_ffname != پوچ
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    اگر (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    اگر (curbuf->b_ml.ml_mfp != پوچ)
		بشکن;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	اگر (curbuf == پوچ)
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
	اگر (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(پوچ, curwin);
#endif
	بازگشت FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    اگر (curbuf->b_ml.ml_mfp != پوچ)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    اگر (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    اگر (curbuf->b_ffname != پوچ
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	صحیح old_پیام_silent = پیام_silent;
#ifdef UNIX
	صحیح	save_bin = curbuf->b_p_bin;
	صحیح perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	صحیح oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	اگر (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	اگر (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	اگر (shortmess(SHM_FILEINFO))
	    پیام_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	اگر (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    اگر (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_buffer(FALSE, eap, flags);
	}
#endif
	پیام_silent = old_پیام_silent;
#ifdef FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#endif
	// Help buffer is filtered.
	اگر (bt_help(curbuf))
	    fix_help_buffer();
    }
    وگرنه اگر (read_stdin)
    {
	صحیح	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(پوچ, پوچ, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, پوچ,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	اگر (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    اگر (curbuf->b_ml.ml_mfp != پوچ
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    اگر (curbuf->b_flags & BF_NEVERLOADED)
    {
	(تهی)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    اگر ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != پوچ)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != پوچ)
#endif
       )
	changed();
    وگرنه اگر (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    اگر (aborting())
#وگرنه
    اگر (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    اگر (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, پوچ, پوچ, FALSE, curbuf, &retval);
#وگرنه
    apply_autocmds(EVENT_BUFENTER, پوچ, پوچ, FALSE, curbuf);
#endif

    اگر (retval != OK)
	بازگشت retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    اگر (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != پوچ)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	اگر (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    اگر ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, پوچ, پوچ,
			FALSE, curbuf, &retval);
#وگرنه
	    apply_autocmds(EVENT_BUFWINENTER, پوچ, پوچ,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    بازگشت retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    تهی
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == پوچ ? 0 : buf->b_fnum;
    bufref->br_buf_free_شمار = buf_free_شمار;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    صحیح
bufref_valid(bufref_T *bufref)
{
    بازگشت bufref->br_buf_free_شمار == buf_free_شمار
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    صحیح
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	اگر (bp == buf)
	    بازگشت TRUE;
    بازگشت FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
ایستا hashtab_T buf_hashtab;

    ایستا تهی
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((نویسه *)buf->b_کلید, اندازه(buf->b_کلید), "%x", buf->b_fnum);
    اگر (hash_add(&buf_hashtab, buf->b_کلید, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    ایستا تهی
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_کلید);

    اگر (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    ایستا صحیح
can_unload_buffer(buf_T *buf)
{
    صحیح	    can_unload = !buf->b_locked;

    اگر (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		بشکن;
	    }
    }
    // Don't unload the buffer while it's still being saved
    اگر (can_unload && buf->b_saving)
	can_unload = FALSE;

    اگر (!can_unload)
    {
	char_u *fname = buf->b_fname != پوچ ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != پوچ ? fname : (char_u *)"[No Name]");
    }
    بازگشت can_unload;
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
    صحیح
close_buffer(
    win_T	*win,
    buf_T	*buf,
    صحیح		action,
    صحیح		abort_if_last,
    صحیح		ignore_abort,
    صحیح		set_context)
{
    صحیح		hiding_buf;
    bufref_T	bufref;
    صحیح		is_curwin = (curwin != پوچ && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    صحیح		unload_buf = (action != 0);
    صحیح		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    صحیح		del_buf = (action == DOBUF_DEL || wipe_buf);
    صحیح		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    صحیح		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    اگر (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    وگرنه اگر (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    وگرنه اگر (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    اگر (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	اگر (term_job_running(buf->b_term))
	{
	    اگر (wipe_buf || unload_buf)
	    {
		اگر (!can_unload_buffer(buf))
		    بازگشت FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    وگرنه
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	وگرنه اگر (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	وگرنه
	{
	    اگر (del_buf || unload_buf)
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
    اگر ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	بازگشت FALSE;

    اگر (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	اگر (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    اگر ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	اگر (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    بازگشت FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	اگر (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    برو aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	اگر (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    اگر (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		برو aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    اگر (abort_if_last && one_window())
		// Autocommands made this the only window.
		برو aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	اگر (!ignore_abort && aborting())
	    بازگشت FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    اگر (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    اگر (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    اگر (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    اگر (!hiding_buf || !unload_buf)
	بازگشت FALSE;

    // Always remove the buffer when there is no file name.
    اگر (buf->b_ffname == پوچ)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    اگر (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	بازگشت FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    اگر ((win_valid || closed_popup) && win->w_buffer == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_buffer = پوچ;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    اگر (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != پوچ || buf->b_next != پوچ
#اگر defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	اگر (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    اگر (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, اندازه(صحیح), 50);
	    اگر (ga_grow(&buf_reuse, 1) == OK)
		((صحیح *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	اگر (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	وگرنه
	    buf->b_sfname = پوچ;
	VIM_CLEAR(buf->b_ffname);
	اگر (buf->b_prev == پوچ)
	    firstbuf = buf->b_next;
	وگرنه
	    buf->b_prev->b_next = buf->b_next;
	اگر (buf->b_next == پوچ)
	    lastbuf = buf->b_prev;
	وگرنه
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    وگرنه
    {
	اگر (del_buf)
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
	اگر (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    أكد(!win_valid || !is_curwin || win == curwin);
    بازگشت TRUE;
}

/*
 * Make buffer not contain a file.
 */
    تهی
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_سطر_شمار = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_start_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_start_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_start_bomb = FALSE;
    buf->b_ml.ml_mfp = پوچ;
    buf->b_ml.ml_flags = ML_EMPTY;		// empty buffer
#ifdef FEAT_NETBEANS_INTG
    netbeans_deleted_all_سطرها(buf);
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
    صحیح
buf_freeall(buf_T *buf, صحیح flags)
{
    صحیح		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    صحیح		is_curwin = (curwin != پوچ && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    اگر (buf->b_ml.ml_mfp != پوچ)
    {
	اگر (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    بازگشت FALSE;
    }
    اگر ((flags & BFA_DEL) && buf->b_p_bl)
    {
	اگر (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    بازگشت FALSE;
    }
    اگر (flags & BFA_WIPE)
    {
	اگر (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    بازگشت FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    اگر (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    اگر ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	بازگشت FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    اگر (buf == curbuf && !is_curbuf)
	بازگشت FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    اگر (buf == curbuf && VIsual_active
#اگر defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    اگر (curwin != پوچ && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    اگر (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_سطر_شمار = 0;    // no lines in buffer
    اگر ((flags & BFA_KEEP_UNDO) == 0)
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
    بازگشت TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    ایستا تهی
free_buffer(buf_T *buf)
{
    ++buf_free_شمار;
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

    اگر (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    وگرنه
    {
	vim_free(buf);
	اگر (curbuf == buf)
	    curbuf = پوچ;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    ایستا تهی
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_عدد;
    di->di_tv.v_lock = VAR_FIXED;
    di->di_tv.vval.v_عدد = 0;

#ifdef FEAT_EVAL
    STRCPY(buf->b_ct_di.di_کلید, "changedtick");
    (تهی)dict_add(buf->b_vars, di);
#endif
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    ایستا تهی
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    وقتی (buf->b_wininfo != پوچ)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_next;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    ایستا تهی
free_buffer_stuff(
    buf_T	*buf,
    صحیح		free_options)		// free options as well
{
    اگر (free_options)
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
    تهی
free_wininfo(wininfo_T *wip)
{
    اگر (wip->wi_optset)
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
    تهی
goto_buffer(
    exarg_T	*eap,
    صحیح		start,
    صحیح		dir,
    صحیح		شمار)
{
    bufref_T	old_curbuf;
    صحیح		save_sea = swap_exists_action;
    صحیح		skip_help_buf;

    انتخاب (eap->cmdidx)
    {
	حالت CMD_bnext:
	حالت CMD_sbnext:
	حالت CMD_bNext:
	حالت CMD_bprevious:
	حالت CMD_sbNext:
	حالت CMD_sbprevious:
	    skip_help_buf = TRUE;
	    بشکن;
	پیشفرض:
	    skip_help_buf = FALSE;
	    بشکن;
    }

    set_bufref(&old_curbuf, curbuf);

    اگر (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (تهی)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, شمار,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    اگر (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#اگر defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#اگر defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    وگرنه
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    تهی
handle_swap_exists(bufref_T *old_curbuf)
{
#اگر defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    اگر (swap_exists_action == SEA_QUIT)
    {
#اگر defined(FEAT_EVAL)
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
	اگر (old_curbuf == پوچ || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(پوچ, پوچ, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	وگرنه
	    buf = old_curbuf->br_buf;
	اگر (buf != پوچ)
	{
	    صحیح old_پیام_silent = پیام_silent;

	    اگر (shortmess(SHM_FILEINFO))
		پیام_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    پیام_silent = old_پیام_silent;

#ifdef FEAT_SYN_HL
	    اگر (old_tw != curbuf->b_p_tw)
		check_colorcolumn(پوچ, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#اگر defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    وگرنه اگر (swap_exists_action == SEA_RECOVER)
    {
#اگر defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Recover at ATTENTION prompt.
	پیام_scroll = TRUE;
	ml_recover(FALSE);
	پیام_puts("\n");	// don't overwrite the last message
	cmdline_row = پیام_row;
	do_modelines(0);

#اگر defined(FEAT_EVAL)
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
    ایستا صحیح
empty_curbuf(
    صحیح close_others,
    صحیح forceit,
    صحیح action)
{
    صحیح	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    اگر (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	بازگشت FAIL;
    }

    set_bufref(&bufref, buf);
    اگر (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, پوچ, پوچ, پوچ, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    اگر (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(پوچ, buf, action, FALSE, FALSE, FALSE);
    اگر (!close_others)
	need_fileinfo = FALSE;
    وگرنه اگر (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    بازگشت retval;
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
    ایستا صحیح
do_buffer_ext(
    صحیح		action,
    صحیح		start,
    صحیح		dir,		// FORWARD or BACKWARD
    صحیح		شمار,		// buffer number or number of buffers
    صحیح		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    صحیح		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    انتخاب (start)
    {
	حالت DOBUF_FIRST:   buf = firstbuf; بشکن;
	حالت DOBUF_LAST:    buf = lastbuf;  بشکن;
	پیشفرض:	    buf = curbuf;   بشکن;
    }
    اگر (start == DOBUF_MOD)	    // find next modified buffer
    {
	وقتی (شمار-- > 0)
	{
	    انجامبده
	    {
		buf = buf->b_next;
		اگر (buf == پوچ)
		    buf = firstbuf;
	    }
	    وقتی (buf != curbuf && !bufIsChanged(buf));
	}
	اگر (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    بازگشت FAIL;
	}
    }
    وگرنه اگر (start == DOBUF_FIRST && شمار) // find specified buffer number
    {
	وقتی (buf != پوچ && buf->b_fnum != شمار)
	    buf = buf->b_next;
    }
    وگرنه
    {
	صحیح help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = پوچ;
	وقتی (شمار > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    اگر (bp == پوچ)
		bp = buf;
	    اگر (dir == FORWARD)
	    {
		buf = buf->b_next;
		اگر (buf == پوچ)
		    buf = firstbuf;
	    }
	    وگرنه
	    {
		buf = buf->b_prev;
		اگر (buf == پوچ)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    اگر (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--شمار;
		bp = پوچ;	// use this buffer as new starting point
	    }
	    اگر (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		بازگشت FAIL;
	    }
	}
    }

    اگر (buf == پوچ)	    // could not find it
    {
	اگر (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    اگر (!unload)
		semsg(_(e_buffer_nr_does_not_exist), شمار);
	}
	وگرنه اگر (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	وگرنه
	    emsg(_(e_cannot_go_before_first_buffer));
	بازگشت FAIL;
    }
#ifdef FEAT_PROP_POPUP
    اگر ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	بازگشت OK;
#endif
    اگر (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	بازگشت FAIL;

    اگر ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), شمار);
	بازگشت FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    اگر (unload)
    {
	صحیح	forward;
	bufref_T bufref;

	اگر (!can_unload_buffer(buf))
	    بازگشت FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	اگر (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == پوچ && !buf->b_p_bl)
	    بازگشت FAIL;

	اگر ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    اگر ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		اگر (term_job_running(buf->b_term))
		{
		    اگر (term_confirm_stop(buf) == FAIL)
			بازگشت FAIL;
		}
		وگرنه
# endif
		{
		    dialog_changed(buf, FALSE);
		    اگر (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			بازگشت FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    اگر (bufIsChanged(buf))
			بازگشت FAIL;
		}
	    }
	    وگرنه
#endif
	    {
		no_write_message_buf(buf);
		بازگشت FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	اگر (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    اگر (bp->b_p_bl && bp != buf)
		بشکن;
	اگر (bp == پوچ && buf == curbuf)
	    بازگشت empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	وقتی (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != پوچ))
	{
	    اگر (win_close(curwin, FALSE) == FAIL)
		بشکن;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	اگر (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    اگر (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(پوچ, buf, action, FALSE, FALSE, FALSE);
	    بازگشت OK;
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
	buf = پوچ;	// selected buffer
	bp = پوچ;	// used when no loaded buffer found
	اگر (au_new_curbuf.br_buf != پوچ && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	وگرنه اگر (curwin->w_jumplistlen > 0)
	{
	    صحیح     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    اگر (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    وقتی (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		اگر (buf != پوچ)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    اگر (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = پوچ;
		    وگرنه اگر (buf->b_ml.ml_mfp == پوچ)
		    {
			// skip unloaded buf, but may keep it for later
			اگر (bp == پوچ)
			    bp = buf;
			buf = پوچ;
		    }
		}
		اگر (buf != پوچ)   // found a valid buffer: stop searching
		    بشکن;
		// advance to older entry in jump list
		اگر (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    بشکن;
		اگر (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		اگر (jumpidx == forward)		// List exhausted for sure
		    بشکن;
	    }
	}

	اگر (buf == پوچ)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    برای (;;)
	    {
		اگر (buf == پوچ)
		{
		    اگر (!forward)	// tried both directions
			بشکن;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    ادامه;
		}
		// in non-help buffer, try to skip help buffers, and vv
		اگر (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    اگر (buf->b_ml.ml_mfp != پوچ)   // found loaded buffer
			بشکن;
		    اگر (bp == پوچ)	// remember unloaded buf for later
			bp = buf;
		}
		اگر (forward)
		    buf = buf->b_next;
		وگرنه
		    buf = buf->b_prev;
	    }
	}
	اگر (buf == پوچ)	// No loaded buffer, use unloaded one
	    buf = bp;
	اگر (buf == پوچ)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		اگر (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    بشکن;
	}
	اگر (buf == پوچ)	// Still no buffer, just take one
	{
	    اگر (curbuf->b_next != پوچ)
		buf = curbuf->b_next;
	    وگرنه
		buf = curbuf->b_prev;
	    اگر (bt_quickfix(buf)
		    || (buf != پوچ && buf != curbuf && buf->b_locked_split))
		buf = پوچ;
	}
    }

    اگر (buf == پوچ)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	بازگشت empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    اگر (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != پوچ)
	بازگشت OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    اگر (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	بازگشت FAIL;
    }
    اگر (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	بازگشت FAIL;

    // go to current buffer - nothing to do
    اگر (buf == curbuf)
	بازگشت OK;

    // Check if the current buffer may be abandoned.
    اگر (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	اگر ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    اگر (term_job_running(curbuf->b_term))
	    {
		اگر (term_confirm_stop(curbuf) == FAIL)
		    بازگشت FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    وگرنه
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		اگر (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    بازگشت FAIL;

		اگر (bufIsChanged(curbuf))
		{
		    no_write_message();
		    بازگشت FAIL;
		}
	    }
	}
	وگرنه
#endif
	{
	    no_write_message();
	    بازگشت FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    اگر (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#اگر defined(FEAT_EVAL)
    اگر (aborting())	    // autocmds may abort script processing
	بازگشت FAIL;
#endif

    بازگشت OK;
}

    صحیح
do_buffer(
    صحیح		action,
    صحیح		start,
    صحیح		dir,		// FORWARD or BACKWARD
    صحیح		شمار,		// buffer number or number of buffers
    صحیح		forceit)	// TRUE when using !
{
    بازگشت do_buffer_ext(action, start, dir, شمار,
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
    نویسه *
do_bufdel(
    صحیح		command,
    char_u	*arg,		// pointer to extra arguments
    صحیح		addr_شمار,
    صحیح		start_bnr,	// first buffer number in a range
    صحیح		end_bnr,	// buffer nr or last buffer nr in a range
    صحیح		forceit)
{
    صحیح		do_current = 0;	// delete current buffer?
    صحیح		deleted = 0;	// number of buffers deleted
    نویسه	*errormsg = پوچ; // return value
    صحیح		bnr;		// buffer number
    char_u	*p;

    اگر (addr_شمار == 0)
    {
	(تهی)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    وگرنه
    {
	اگر (addr_شمار == 2)
	{
	    اگر (*arg)		// both range and argument is not allowed
		بازگشت ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	وگرنه	// addr_count == 1
	    bnr = end_bnr;

	برای ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    اگر (bnr == curbuf->b_fnum)
		do_current = bnr;
	    وگرنه اگر (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    اگر (addr_شمار == 2)
	    {
		اگر (++bnr > end_bnr)
		    بشکن;
	    }
	    وگرنه    // addr_count == 1
	    {
		arg = skipwhite(arg);
		اگر (*arg == NUL)
		    بشکن;
		اگر (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    اگر (bnr < 0)	    // failed
			بشکن;
		    arg = p;
		}
		وگرنه
		    bnr = getdigits(&arg);
	    }
	}
	اگر (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	اگر (deleted == 0)
	{
	    اگر (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    وگرنه اگر (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    وگرنه
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (نویسه *)IObuff;
	}
	وگرنه اگر (deleted >= p_report)
	{
	    اگر (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    وگرنه اگر (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    وگرنه
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    بازگشت errormsg;
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
    تهی
set_curbuf(buf_T *buf, صحیح action)
{
    buf_T	*prevbuf;
    صحیح		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    صحیح		valid;

    setpcmark();
    اگر ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    اگر (!apply_autocmds(EVENT_BUFLEAVE, پوچ, پوچ, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	اگر (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	اگر (unload)
	    close_windows(prevbuf, FALSE);
#اگر defined(FEAT_EVAL)
	اگر (bufref_valid(&prevbufref) && !aborting())
#وگرنه
	اگر (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    اگر (prevbuf == curbuf
		    && ((State & MODE_درج) == 0 || curbuf->b_nwindows <= 1))
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
    اگر ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == پوچ)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	اگر (!valid)
	    enter_buffer(lastbuf);
	وگرنه
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	اگر (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(پوچ, curwin);
#endif
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    ایستا تهی
enter_buffer(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    اگر (VIsual_active
#اگر defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    اگر (curwin->w_buffer != پوچ)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    اگر (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    وگرنه
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    اگر (curwin->w_p_diff)
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
    اگر (curbuf->b_ml.ml_mfp == پوچ)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	اگر (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, پوچ, 0);
    }
    وگرنه
    {
	اگر (!پیام_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(تهی)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
	apply_autocmds(EVENT_BUFENTER, پوچ, پوچ, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, پوچ, پوچ, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    اگر (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    اگر (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    اگر (curbuf->b_kmap_state & KEYMAP_INIT)
	(تهی)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    اگر (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(تهی)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#اگر defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    تهی
do_autochdir(تهی)
{
    اگر ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != پوچ
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	last_chdir_reason = "autochdir";
    }
}
#endif

    تهی
no_write_message_buf(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    اگر (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    وگرنه
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    تهی
no_write_message(تهی)
{
#ifdef FEAT_TERMINAL
    اگر (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    وگرنه
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    تهی
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    اگر (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    وگرنه
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
    صحیح
curbuf_reusable(تهی)
{
    بازگشت (curbuf != پوچ
	&& curbuf->b_ffname == پوچ
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == پوچ || BUFEMPTY())
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
    صحیح		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    اگر (بالاترین_file_رقم == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    اگر (sfname == پوچ || mch_stat((نویسه *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    اگر (ffname != پوچ && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#وگرنه
		buflist_findname(ffname)
#endif
		) != پوچ)
    {
	vim_free(ffname);
	اگر (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? پوچ : curwin,
						      lnum, (colnr_T)0, FALSE);

	اگر ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	اگر ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    اگر (!(flags & BLN_DUMMY))
	    {
		اگر (apply_autocmds(EVENT_BUFADD, پوچ, پوچ, FALSE, buf)
			&& !bufref_valid(&bufref))
		    بازگشت پوچ;
	    }
	}
	بازگشت buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = پوچ;
    اگر ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	اگر (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    بازگشت پوچ;
	}
#endif
	اگر (!bufref_valid(&bufref))
	    buf = پوچ;		// buf was deleted; allocate a new buffer
    }
    اگر (buf != curbuf || curbuf == پوچ)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	اگر (buf == پوچ)
	{
	    vim_free(ffname);
	    بازگشت پوچ;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	اگر (buf->b_vars == پوچ)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    بازگشت پوچ;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    اگر (ffname != پوچ)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    اگر ((ffname != پوچ && (buf->b_ffname == پوچ || buf->b_sfname == پوچ))
	    || buf->b_wininfo == پوچ)
    {
	اگر (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	وگرنه
	    buf->b_sfname = پوچ;
	VIM_CLEAR(buf->b_ffname);
	اگر (buf != curbuf)
	    free_buffer(buf);
	بازگشت پوچ;
    }

    اگر (buf == curbuf)
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
    وگرنه
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = پوچ;
	اگر (firstbuf == پوچ)		// buffer list is empty
	{
	    buf->b_prev = پوچ;
	    firstbuf = buf;
	}
	وگرنه				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	اگر ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((صحیح *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    وقتی (buf->b_prev != پوچ && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		اگر (prev->b_next != پوچ)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		اگر (buf->b_prev != پوچ)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		اگر (lastbuf == buf)
		    lastbuf = prev;
		اگر (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	وگرنه
	    buf->b_fnum = بالاترین_file_رقم++;
	اگر (بالاترین_file_رقم < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    اگر (emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    بالاترین_file_رقم = 1;
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
    اگر (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    وگرنه
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    اگر (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    اگر (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	اگر (apply_autocmds(EVENT_BUFNEW, پوچ, پوچ, FALSE, buf)
		&& !bufref_valid(&bufref))
	    بازگشت پوچ;
	اگر (flags & BLN_LISTED)
	{
	    اگر (apply_autocmds(EVENT_BUFADD, پوچ, پوچ, FALSE, buf)
		    && !bufref_valid(&bufref))
		بازگشت پوچ;
	}
#ifdef FEAT_EVAL
	اگر (aborting())		// autocmds may abort script processing
	    بازگشت پوچ;
#endif
    }

    بازگشت buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    تهی
free_buf_options(
    buf_T	*buf,
    صحیح		free_p_ff)
{
    اگر (free_p_ff)
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
#اگر defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#اگر defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#اگر defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    اگر (buf->b_p_کلید != پوچ && *buf->b_p_کلید != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_کلید, STRLEN(buf->b_p_کلید));
# endif
    clear_string_option(&buf->b_p_کلید);
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
    buf->b_s.b_cap_prog = پوچ;
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
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_شمار);
    buf->b_p_cpt_شمار = 0;
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
    صحیح
buflist_getfile(
    صحیح		n,
    linenr_T	lnum,
    صحیح		options,
    صحیح		forceit)
{
    buf_T	*buf;
    win_T	*wp = پوچ;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    اگر (buf == پوچ)
    {
	اگر ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	وگرنه
	    semsg(_(e_buffer_nr_not_found), n);
	بازگشت FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    اگر (buf == curbuf)
	بازگشت OK;

    اگر (text_or_buf_locked())
	بازگشت FAIL;

    // altfpos may be changed by getfile(), get it now
    اگر (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    وگرنه
	col = 0;

    اگر (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	اگر (wp == پوچ && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    اگر (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    وگرنه اگر (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		بازگشت FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    صحیح retval = FAIL;
    اگر (GETFILE_SUCCESS(getfile(buf->b_fnum, پوچ, پوچ,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	اگر (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    بازگشت retval;
}

/*
 * go to the last know line number for the current buffer
 */
    ایستا تهی
buflist_getfpos(تهی)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    اگر (p_sol)
	curwin->w_cursor.col = 0;
    وگرنه
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
    buf_T	*buf = پوچ;

    // First make the name into a full path name
    ffname = FullName_save(fname,
#ifdef UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#وگرنه
	    FALSE
#endif
	    );
    اگر (ffname != پوچ)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    بازگشت buf;
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

    اگر (mch_stat((نویسه *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    بازگشت buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    ایستا buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#endif
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_LAST(buf)
	اگر ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    بازگشت buf;
    بازگشت پوچ;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    صحیح
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    صحیح		unlisted,	// find unlisted buffers
    صحیح		diffmode UNUSED, // find diff-mode buffers only
    صحیح		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    صحیح		match = -1;
    صحیح		find_listed;
    char_u	*pat;
    char_u	*patend;
    صحیح		attempt;
    char_u	*p;
    صحیح		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    اگر ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	اگر (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	وگرنه
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	اگر (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
    وگرنه
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, پوچ, FALSE);
	اگر (pat == پوچ)
	    بازگشت -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	برای (;;)
	{
	    برای (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		اگر (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		اگر (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    اگر (regmatch.regprog == پوچ)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			بازگشت -1;
		    }
		    اگر (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != پوچ)
		    {
			اگر (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				اگر (wp->w_buffer == buf)
				    بشکن;
			    اگر (wp == پوچ)
				ادامه;
			}
			اگر (match >= 0)		// already found a match
			{
			    match = -2;
			    بشکن;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		اگر (match >= 0)			// found one match
		    بشکن;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    اگر (!unlisted || !find_listed || match != -1)
		بشکن;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    اگر (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    وگرنه اگر (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    بازگشت match;
}

#ifdef FEAT_VIMINFO
تعریفنوع ساختار {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    صحیح
ExpandBufnames(
    char_u	*pat,
    صحیح		*رقم_file,
    char_u	***file,
    صحیح		options)
{
    صحیح		شمار;
    buf_T	*buf;
    صحیح		round;
    char_u	*p;
    char_u	*patc = پوچ;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = پوچ;
#endif
    صحیح		fuzzy;
    fuzmatch_str_T  *fuzmatch = پوچ;
    regmatch_T	regmatch;
    صحیح		score = 0;
    صحیح		to_free = FALSE;

    *رقم_file = 0;		    // return values in case of FAIL
    *file = پوچ;

#ifdef FEAT_DIFF
    اگر ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	بازگشت FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    اگر (!fuzzy)
    {
	اگر (*pat == '^' && pat[1] != NUL)
	{
	    صحیح  len = (صحیح)STRLEN(pat);
	    patc = alloc(len);
	    اگر (patc == پوچ)
		بازگشت FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	وگرنه اگر (*pat == '^')
	    patc = (char_u *)"";
	وگرنه
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    برای (round = 1; round <= 2; ++round)
    {
	شمار = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    اگر (!buf->b_p_bl)	// skip unlisted buffers
		ادامه;
#ifdef FEAT_DIFF
	    اگر (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		اگر (buf == curbuf || !diff_mode_buf(buf))
		    ادامه;
#endif

	    اگر (!fuzzy)
	    {
		اگر (regmatch.regprog == پوچ)
		{
		    // invalid pattern, possibly after recompiling
		    اگر (to_free)
			vim_free(patc);
		    بازگشت FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    وگرنه
	    {
		p = پوچ;
		// first try matching with the short file name
		اگر ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		اگر (p == پوچ)
		{
		    // next try matching with the full path file name
		    اگر ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    اگر (p == پوچ)
		ادامه;

	    اگر (round == 1)
	    {
		++شمار;
		ادامه;
	    }

	    اگر (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    وگرنه
		p = vim_strsave(p);
	    اگر (p == پوچ)
	    {
		اگر (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, شمار);
		بازگشت FAIL;
	    }

	    اگر (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		اگر (matches != پوچ)
		{
		    matches[شمار].buf = buf;
		    matches[شمار].match = p;
		    شمار++;
		}
		وگرنه
#endif
		    (*file)[شمار++] = p;
	    }
	    وگرنه
	    {
		fuzmatch[شمار].idx = شمار;
		fuzmatch[شمار].str = p;
		fuzmatch[شمار].score = score;
		شمار++;
	    }
	}
	اگر (شمار == 0)	// no match found, break here
	    بشکن;
	اگر (round == 1)
	{
	    اگر (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, شمار);
		اگر (*file == پوچ)
		{
		    vim_regfree(regmatch.regprog);
		    اگر (to_free)
			vim_free(patc);
		    بازگشت FAIL;
		}
#ifdef FEAT_VIMINFO
		اگر (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, شمار);
#endif
	    }
	    وگرنه
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, شمار);
		اگر (fuzmatch == پوچ)
		{
		    *رقم_file = 0;
		    *file = پوچ;
		    بازگشت FAIL;
		}
	    }
	}
    }

    اگر (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	اگر (to_free)
	    vim_free(patc);
    }

    اگر (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	اگر (matches != پوچ)
	{
	    صحیح i;
	    اگر (شمار > 1)
		qsort(matches, شمار, اندازه(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    اگر (matches[0].buf == curbuf)
	    {
		برای (i = 1; i < شمار; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[شمار-1] = matches[0].match;
	    }
	    وگرنه
	    {
		برای (i = 0; i < شمار; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    وگرنه
    {
	اگر (fuzzymatches_to_strmatches(fuzmatch, file, شمار, FALSE) == FAIL)
	    بازگشت FAIL;
    }

    *رقم_file = شمار;
    بازگشت (شمار == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    ایستا char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    صحیح		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    اگر (match == پوچ && rmp->regprog != پوچ)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    بازگشت match;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    ایستا char_u *
fname_match(
    regmatch_T	*rmp,
    char_u	*name,
    صحیح		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = پوچ;
    char_u	*p;

    // extra check for valid arguments
    اگر (name == پوچ || rmp->regprog == پوچ)
	بازگشت پوچ;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    اگر (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    وگرنه اگر (rmp->regprog != پوچ)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(پوچ, name);
	اگر (p != پوچ && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    بازگشت match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(صحیح nr)
{
    char_u	کلید[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    اگر (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((نویسه *)کلید, اندازه(کلید), "%x", nr);
    hi = hash_find(&buf_hashtab, کلید);

    اگر (!HASHITEM_EMPTY(hi))
	بازگشت (buf_T *)(hi->hi_کلید
			     - ((unsigned)(curbuf->b_کلید - (char_u *)curbuf)));
    بازگشت پوچ;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    صحیح		n,
    صحیح		fullname,
    صحیح		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    اگر (buf == پوچ)
	بازگشت پوچ;
    بازگشت home_replace_save(helptail ? buf : پوچ,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    تهی
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	col,
    صحیح		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	اگر (wip->wi_win == win)
	    بشکن;
    اگر (wip == پوچ)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	اگر (wip == پوچ)
	    بازگشت;
	wip->wi_win = win;
	اگر (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    وگرنه
    {
	// remove the entry from the list
	اگر (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	وگرنه
	    buf->b_wininfo = wip->wi_next;
	اگر (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	اگر (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    اگر (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    اگر (win != پوچ)
	wip->wi_changelistidx = win->w_changelistidx;
    اگر (copy_options && win != پوچ)
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
    wip->wi_prev = پوچ;
    اگر (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    ایستا صحیح
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    اگر (!wip->wi_opt.wo_diff)
	بازگشت FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	اگر (wip->wi_win == wp)
	    بازگشت FALSE;
    بازگشت TRUE;
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
    ایستا wininfo_T *
find_wininfo(
    buf_T	*buf,
    صحیح		need_options,
    صحیح		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	اگر (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    بشکن;

    اگر (wip != پوچ)
	بازگشت wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    اگر (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    اگر (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != پوچ
			    && wip->wi_win->w_buffer == buf)))
		بشکن;
    }
    وگرنه
#endif
	wip = buf->b_wininfo;
    بازگشت wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    تهی
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
    clearFolding(curwin);
#endif

    wip = find_wininfo(buf, TRUE, TRUE);
    اگر (wip != پوچ && wip->wi_win != پوچ
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
    وگرنه اگر (wip != پوچ && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    وگرنه
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    اگر (wip != پوچ)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    اگر (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    اگر (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    اگر (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	اگر (p_ea)
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
    ایستا pos_T no_position = {1, 0, 0};

    wip = find_wininfo(buf, FALSE, FALSE);
    اگر (wip != پوچ)
	بازگشت &(wip->wi_fpos);
    وگرنه
	بازگشت &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    بازگشت buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    تهی
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    صحیح		len;
    صحیح		i;
    صحیح		ro_char;
    صحیح		changed_char;
#ifdef FEAT_TERMINAL
    صحیح		job_running;
    صحیح		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = پوچ, **p;

    اگر (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, اندازه(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    اگر (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_data, (size_t)buflist.ga_len,
		اندازه(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    برای (; buf != پوچ && !got_int; buf = buflist_data != پوچ
	    ? (++p < buflist_data + buflist.ga_len ? *p : پوچ)
	    : buf->b_next)
#وگرنه
    برای (buf = firstbuf; buf != پوچ && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	اگر ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !bufIsChanged(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == پوچ || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == پوچ || buf->b_nwindows != 0))
#ifdef FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!job_running || (job_running && job_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!job_running || (job_running && !job_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (job_running || buf->b_term == پوچ))
#endif
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    ادامه;
	name = buf_spname(buf);
	اگر (name != پوچ)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	وگرنه
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	اگر (message_filtered(NameBuff))
	    ادامه;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	اگر (job_running)
	{
	    اگر (job_none_open)
		ro_char = '?';
	    وگرنه
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	وگرنه اگر (buf->b_term != پوچ)
	    ro_char = 'F';
	وگرنه
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	پیام_putchar('\n');
	len = (صحیح)vim_snprintf_safelen((نویسه *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == پوچ ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		changed_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	انجامبده
	    IObuff[len++] = ' ';
	وقتی (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	اگر (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (size_t)(IOSIZE - len), buf->b_last_used);
	وگرنه
#endif
	    vim_snprintf((نویسه *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (long)buflist_findlnum(buf));
	پیام_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    اگر (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    صحیح
buflist_name_nr(
    صحیح		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    اگر (buf == پوچ || buf->b_fname == پوچ)
	بازگشت FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    بازگشت OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    صحیح
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    صحیح		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = پوچ;
#ifdef UNIX
    stat_T	st;
#endif

    اگر (ffname == پوچ || *ffname == NUL)
    {
	// Removing the name.
	اگر (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	وگرنه
	    buf->b_sfname = پوچ;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    وگرنه
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	اگر (ffname == پوچ)		    // out of memory
	    بازگشت FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	اگر (mch_stat((نویسه *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	اگر (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#وگرنه
	    obuf = buflist_findname(ffname);
#endif
	اگر (obuf != پوچ && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    صحیح		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		اگر (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    اگر (obuf->b_ml.ml_mfp != پوچ || in_use)
	    {
		اگر (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		بازگشت FAIL;
	    }
	    // delete from the list
	    close_buffer(پوچ, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	اگر (ffname == پوچ || sfname == پوچ)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    بازگشت FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	اگر (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    اگر (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    وگرنه
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    بازگشت OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    تهی
buf_set_name(صحیح fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    اگر (buf == پوچ)
	بازگشت;

    اگر (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = پوچ;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    تهی
buf_name_changed(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    اگر (buf->b_ml.ml_mfp != پوچ)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    اگر (buf->b_term != پوچ)
	term_clear_status_text(buf->b_term);
#endif

    اگر (curwin->w_buffer == buf)
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
    اگر (buf != پوچ && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    بازگشت buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    صحیح		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    اگر (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	اگر (errmsg)
	    emsg(_(e_no_alternate_file));
	بازگشت پوچ;
    }
    بازگشت fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    صحیح
buflist_add(char_u *fname, صحیح flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, پوچ, (linenr_T)0, flags);
    اگر (buf != پوچ)
	بازگشت buf->b_fnum;
    بازگشت 0;
}

#اگر defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    تهی
buflist_slash_adjust(تهی)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	اگر (bp->b_ffname != پوچ)
	    slash_adjust(bp->b_ffname);
	اگر (bp->b_sfname != پوچ)
	    slash_adjust(bp->b_sfname);
    }
}
#endif

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    تهی
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.col, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    صحیح
otherfile(char_u *ffname)
{
    بازگشت otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , پوچ
#endif
	    );
}

    ایستا صحیح
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    اگر (ffname == پوچ || *ffname == NUL || buf->b_ffname == پوچ)
	بازگشت TRUE;
    اگر (fnamecmp(ffname, buf->b_ffname) == 0)
	بازگشت FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	اگر (stp == پوچ)
	{
	    اگر (!buf->b_dev_valid || mch_stat((نویسه *)ffname, &st) < 0)
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
	اگر (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    اگر (buf_same_ino(buf, stp))
		بازگشت FALSE;
	}
    }
#endif
    بازگشت TRUE;
}

#اگر defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    تهی
buf_setino(buf_T *buf)
{
    stat_T	st;

    اگر (buf->b_fname != پوچ && mch_stat((نویسه *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    وگرنه
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    ایستا صحیح
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    بازگشت (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    تهی
fileinfo(
    صحیح fullname,	    // when non-zero print full path
    صحیح shorthelp,
    صحیح	dont_truncate)
{
    char_u	*name;
    نویسه	*buffer;
    size_t	bufferlen = 0;

    buffer = alloc(IOSIZE);
    اگر (buffer == پوچ)
	بازگشت;

    اگر (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    اگر (name != پوچ)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    وگرنه
    {
	اگر (!fullname && curbuf->b_fname != پوچ)
	    name = curbuf->b_fname;
	وگرنه
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : پوچ, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (صحیح)bufferlen, TRUE);
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

    اگر (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_سطرها_پیام));
    وگرنه اگر (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_سطر_شمار),
	    (long)curbuf->b_ml.ml_سطر_شمار,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_سطر_شمار));
    وگرنه
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (long)curwin->w_cursor.lnum,
	    (long)curbuf->b_ml.ml_سطر_شمار,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_سطر_شمار));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (صحیح)curwin->w_cursor.col + 1, (صحیح)curwin->w_virtcol + 1);
    }

    (تهی)append_arg_عدد(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    اگر (dont_truncate)
    {
	صحیح	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	پیام_start();
	n = پیام_scroll;
	پیام_scroll = TRUE;
	پیام(buffer);
	پیام_scroll = n;
    }
    وگرنه
    {
	نویسه	*p = پیام_trunc_attr(buffer, FALSE, 0);
	اگر (restart_edit != 0 || (پیام_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_پیام((char_u *)p, 0);
    }

    vim_free(buffer);
}

    صحیح
col_print(
    char_u  *buf,
    size_t  buflen,
    صحیح	    col,
    صحیح	    vcol)
{
    اگر (col == vcol)
	بازگشت (صحیح)vim_snprintf_safelen((نویسه *)buf, buflen, "%d", col);

    بازگشت (صحیح)vim_snprintf_safelen((نویسه *)buf, buflen, "%d-%d", col, vcol);
}

ایستا char_u *lasttitle = پوچ;
ایستا char_u *lasticon = پوچ;

/*
 * Put the file name in the title bar and icon of the window.
 */
    تهی
maketitle(تهی)
{
    char_u	*title_str = پوچ;
    char_u	*icon_str = پوچ;
    صحیح		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    اگر (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	بازگشت;
    }

    need_maketitle = FALSE;
    اگر (!p_title && !p_icon && lasttitle == پوچ && lasticon == پوچ)
	بازگشت;  // nothing to do

    اگر (p_title)
    {
	صحیح maxlen = 0;

	اگر (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    اگر (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	اگر (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    اگر (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, اندازه(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, پوچ, پوچ, پوچ);
	    وگرنه
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	وگرنه
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (اندازه(buf) - 100)
#define SPACE_FOR_DIR   (اندازه(buf) - 20)
#define SPACE_FOR_ARGNR (اندازه(buf) - 10)  // at least room for " - VIM"

	    // file name
	    اگر (curbuf->b_fname == پوچ)
		buflen = vim_snprintf_safelen((نویسه *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    وگرنه اگر (curbuf->b_term != پوچ)
		buflen = vim_snprintf_safelen((نویسه *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    وگرنه
	    {
		buflen = vim_snprintf_safelen((نویسه *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != پوچ)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    اگر (curbuf->b_term == پوچ)
#endif
	    {
		انتخاب (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    حالت 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (نویسه *)buf + buflen,
			    اندازه(buf) - buflen, " +");
			بشکن;
		    حالت 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (نویسه *)buf + buflen,
			    اندازه(buf) - buflen, " =");
			بشکن;
		    حالت 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (نویسه *)buf + buflen,
			    اندازه(buf) - buflen, " =+");
			بشکن;
		    حالت 4:
		    حالت 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (نویسه *)buf + buflen,
			    اندازه(buf) - buflen, " -");
			بشکن;
		    حالت 5:
		    حالت 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (نویسه *)buf + buflen,
			    اندازه(buf) - buflen, " -+");
			بشکن;
		    پیشفرض:
			بشکن;
		}
	    }

	    // path (surrounded by '()')
	    اگر (curbuf->b_fname != پوچ
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == پوچ
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((نویسه *)buf + buflen,
		    اندازه(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (صحیح)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		اگر (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		اگر (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((نویسه *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		وگرنه
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    اگر (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((نویسه *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != پوچ)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    وگرنه
			buflen += vim_snprintf_safelen((نویسه *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_عدد(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((نویسه *)buf + buflen,
		اندازه(buf) - buflen, " - %s",
#اگر defined(FEAT_CLIENTSERVER)
		(serverName != پوچ)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    اگر (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		اگر (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, اندازه(buf));
	    }
	}
    }
    mustset = مقدار_changed(title_str, &lasttitle);

    اگر (p_icon)
    {
	icon_str = buf;
	اگر (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    اگر (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, اندازه(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, پوچ, پوچ,
				 پوچ);
	    وگرنه
#endif
		icon_str = p_iconstring;
	}
	وگرنه
	{
	    char_u  *name;
	    صحیح	    namelen;

	    name = buf_spname(curbuf);
	    اگر (name == پوچ)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (صحیح)STRLEN(name);
	    اگر (namelen > 100)
	    {
		namelen -= 100;
		اگر (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, اندازه(buf));
	}
    }

    mustset |= مقدار_changed(icon_str, &lasticon);

    اگر (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    ایستا صحیح
مقدار_changed(char_u *str, char_u **last)
{
    اگر ((str == پوچ) != (*last == پوچ)
	    || (str != پوچ && *last != پوچ && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	اگر (str == پوچ)
	{
	    *last = پوچ;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	وگرنه
	{
	    *last = vim_strsave(str);
	    بازگشت TRUE;
	}
    }
    بازگشت FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    تهی
resettitle(تهی)
{
    mch_settitle(lasttitle, lasticon);
}

#اگر defined(EXITFREE)
    تهی
free_titles(تهی)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#اگر defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
تعریفنوع ساختار
{
    char_u	*stl_start;
    صحیح		stl_minwid;
    صحیح		stl_maxwid;
    شمارش {
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

ایستا size_t		stl_اقلام_len = 20; // Initial value, grows as needed.
ایستا stl_item_T      *stl_اقلام = پوچ;
ایستا صحیح	       *stl_groupitem = پوچ;
ایستا stl_hlrec_T     *stl_hltab = پوچ;
ایستا stl_hlrec_T     *stl_tabtab = پوچ;
ایستا stl_clickrec_T  *stl_clicktab = پوچ;
ایستا صحیح		*stl_separator_locations = پوچ;

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
    صحیح
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحیح		opt_scope,	// scope for "opt_name"
    صحیح		fillchar,
    صحیح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    بازگشت build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    پوچ, پوچ);
}

    صحیح
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحیح		opt_scope,	// scope for "opt_name"
    صحیح		fillchar,
    صحیح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    صحیح		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    بازگشت build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    پوچ, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    صحیح
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحیح		opt_scope,	// scope for "opt_name"
    صحیح		fillchar,
    صحیح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    صحیح		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    بازگشت build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    پوچ, carry_hl);
}
# endif

    صحیح
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحیح		opt_scope)	// scope for "opt_name"
{
    صحیح rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (تهی)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, اندازه(buf), &fmt,
	    opt_name, opt_scope, 0, 0, پوچ, پوچ, پوچ, &rendered_height,
	    پوچ);
    --emsg_off;
    بازگشت rendered_height;
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
ایستا صحیح
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحیح		opt_scope,	// scope for "opt_name"
    صحیح		fillchar,
    صحیح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    صحیح		*rendered_height,   // return: stl rendered height (can be NULL)
    صحیح		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    صحیح		byteval;
# ifdef FEAT_EVAL
    صحیح		use_sandbox;
    صحیح		save_VIsual_active;
# endif
    صحیح		empty_سطر;
    long	l;
    long	n;
    صحیح		prevchar_isflag;
    صحیح		prevchar_isitem;
    صحیح		itemisflag;
    صحیح		fillable;
    char_u	*str;
    long	رقم;
    صحیح		width;
    صحیح		itemcnt;
    صحیح		curitem;
    صحیح		groupdepth;
# ifdef FEAT_EVAL
    صحیح		evaldepth;
# endif
    صحیح		minwid;
    صحیح		maxwid;
    صحیح		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    صحیح		save_redraw_not_allowed = redraw_not_allowed;
    صحیح		save_کلید_Typed = کلید_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    صحیح		did_emsg_before = did_emsg;
    صحیح		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    اگر (updating_screen)
	redraw_not_allowed = TRUE;

    اگر (stl_اقلام == پوچ)
    {
	stl_اقلام = ALLOC_MULT(stl_item_T, stl_اقلام_len);
	stl_groupitem = ALLOC_MULT(صحیح, stl_اقلام_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_اقلام_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_اقلام_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_اقلام_len + 1);

	stl_separator_locations = ALLOC_MULT(صحیح, stl_اقلام_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    اگر (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_عدد;
	tv.vval.v_عدد = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	اگر (usefmt == پوچ)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    اگر (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    اگر (lnum > wp->w_buffer->b_ml.ml_سطر_شمار)
    {
	lnum = wp->w_buffer->b_ml.ml_سطر_شمار;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_سطر = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    اگر (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    وگرنه
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    اگر (carry_hl != پوچ && *carry_hl != 0)
    {
	stl_اقلام[curitem].stl_type = Highlight;
	stl_اقلام[curitem].stl_start = p;
	stl_اقلام[curitem].stl_minwid = *carry_hl;
	stl_اقلام[curitem].stl_clickfunc = پوچ;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    برای (s = usefmt; *s != NUL; )
    {
	اگر (curitem == (صحیح)stl_اقلام_len)
	{
	    size_t	new_len = stl_اقلام_len * 3 / 2;

	    stl_item_T *new_اقلام =
			  vim_realloc(stl_اقلام, اندازه(stl_item_T) * new_len);
	    اگر (new_اقلام == پوچ)
		بشکن;
	    stl_اقلام = new_اقلام;

	    صحیح *new_groupitem =
			     vim_realloc(stl_groupitem, اندازه(صحیح) * new_len);
	    اگر (new_groupitem == پوچ)
		بشکن;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  اندازه(stl_hlrec_T) * (new_len + 1));
	    اگر (new_hlrec == پوچ)
		بشکن;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  اندازه(stl_hlrec_T) * (new_len + 1));
	    اگر (new_hlrec == پوچ)
		بشکن;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      اندازه(stl_clickrec_T) * (new_len + 1));
	    اگر (new_clickrec == پوچ)
		بشکن;
	    stl_clicktab = new_clickrec;

	    صحیح *new_separator_locs = vim_realloc(stl_separator_locations,
					    اندازه(صحیح) * new_len);
	    اگر (new_separator_locs == پوچ)
		بشکن;
	    stl_separator_locations = new_separator_locs;

	    stl_اقلام_len = new_len;
	}

	stl_اقلام[curitem].stl_clickfunc = پوچ;

	اگر (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	وقتی (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    اگر (*s == '\n' || *s == '\r')
	    {
		اگر (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    برو find_linebreak;
		}
		وگرنه اگر (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	اگر (*s == NUL || p + 1 >= out + outlen)
	    بشکن;

	/*
	 * Handle one '%' item.
	 */
	s++;
	اگر (*s == NUL)  // ignore trailing %
	    بشکن;

	اگر (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    اگر (s[1] == ']')
	    {
		stl_اقلام[curitem].stl_type = ClickFunc;
		stl_اقلام[curitem].stl_start = p;
		stl_اقلام[curitem].stl_minwid = 0;
		stl_اقلام[curitem].stl_clickfunc = پوچ;
		s += 2;
		curitem++;
		ادامه;
	    }
	    // %[FuncName] - start click region
	    اگر (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		اگر (rb != پوچ)
		{
		    stl_اقلام[curitem].stl_type = ClickFunc;
		    stl_اقلام[curitem].stl_start = p;
		    stl_اقلام[curitem].stl_minwid = 0;
		    stl_اقلام[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    ادامه;
		}
	    }
	}
	اگر (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    اگر (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		بشکن;
	    }
	    وگرنه اگر (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	اگر (*s == '%')
	{
	    اگر (p + 1 >= out + outlen)
		بشکن;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    ادامه;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	اگر (*s == STL_SEPARATE)
	{
	    s++;
	    اگر (groupdepth > 0)
		ادامه;
	    stl_اقلام[curitem].stl_type = Separate;
	    stl_اقلام[curitem++].stl_start = p;
	    ادامه;
	}
	اگر (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_اقلام[curitem].stl_type = Trunc;
	    stl_اقلام[curitem++].stl_start = p;
	    ادامه;
	}
	اگر (*s == ')')
	{
	    char_u  *t;

	    s++;
	    اگر (groupdepth < 1)
		ادامه;
	    groupdepth--;

	    t = stl_اقلام[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    اگر (curitem > stl_groupitem[groupdepth] + 1
		    && stl_اقلام[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		صحیح group_start_userhl = 0;
		صحیح group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		برای (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    اگر (stl_اقلام[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_اقلام[n].stl_minwid;
			بشکن;
		    }
		}
		برای (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    اگر (stl_اقلام[n].stl_type == Normal)
			بشکن;
		    اگر (stl_اقلام[n].stl_type == Highlight)
			group_end_userhl = stl_اقلام[n].stl_minwid;
		}
		اگر (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    برای (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			اگر (stl_اقلام[n].stl_type == Highlight)
			    stl_اقلام[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			اگر (stl_اقلام[n].stl_type == TabPage)
			    stl_اقلام[n].stl_start = p;
		    }
		}
	    }
	    اگر (l > stl_اقلام[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		اگر (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    وقتی (l >= stl_اقلام[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		وگرنه
		    n = (long)(p - t) - stl_اقلام[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		صحیح minwid_fixed = MIN(stl_اقلام[stl_groupitem[groupdepth]].stl_minwid,
				       stl_اقلام[stl_groupitem[groupdepth]].stl_maxwid);
		وقتی (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		برای (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_اقلام[l].stl_start -= n - 1;
		    اگر (stl_اقلام[l].stl_start < t)
			stl_اقلام[l].stl_start = t;
		}
	    }
	    وگرنه اگر (abs(stl_اقلام[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_اقلام[stl_groupitem[groupdepth]].stl_minwid;
		صحیح fillchar_len = MB_CHAR2LEN(fillchar);
		اگر (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    وقتی (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		وگرنه
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    اگر (p + l >= out + outlen)
		    {
			n = (long)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    برای ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    برای (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_اقلام[n].stl_start += l;
		}
	    }
	    ادامه;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	اگر (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	اگر (*s == '-')
	{
	    s++;
	    l = -1;
	}
	اگر (VIM_ISDIGIT(*s))
	{
	    minwid = (صحیح)getdigits(&s);
	    اگر (minwid < 0)	// overflow
		minwid = 0;
	}
	اگر (*s == STL_USER_HL)
	{
	    stl_اقلام[curitem].stl_type = Highlight;
	    stl_اقلام[curitem].stl_start = p;
	    stl_اقلام[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    ادامه;
	}
	اگر (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    اگر (*s == STL_TABCLOSENR)
	    {
		اگر (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    برای (n = curitem - 1; n >= 0; --n)
			اگر (stl_اقلام[n].stl_type == TabPage
					       && stl_اقلام[n].stl_minwid >= 0)
			{
			    minwid = stl_اقلام[n].stl_minwid;
			    بشکن;
			}
		}
		وگرنه
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_اقلام[curitem].stl_type = TabPage;
	    stl_اقلام[curitem].stl_start = p;
	    stl_اقلام[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    ادامه;
	}
	اگر (*s == '.')
	{
	    s++;
	    اگر (VIM_ISDIGIT(*s))
	    {
		maxwid = (صحیح)getdigits(&s);
		اگر (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	صحیح raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	اگر (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_اقلام[curitem].stl_type = Group;
	    stl_اقلام[curitem].stl_start = p;
	    stl_اقلام[curitem].stl_minwid = minwid;
	    stl_اقلام[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    ادامه;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	اگر (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    ادامه;
	}
# endif
	اگر (vim_strchr(STL_ALL, *s) == پوچ)
	{
	    اگر (*s == NUL)  // can happen with "%0"
		بشکن;
	    s++;
	    ادامه;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	رقم = -1;
	str = پوچ;
	انتخاب (opt)
	{
	حالت STL_FILEPATH:
	حالت STL_FULLPATH:
	حالت STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    اگر (name != پوچ)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    وگرنه
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    اگر (opt != STL_FILENAME)
		str = NameBuff;
	    وگرنه
		str = gettail(NameBuff);
	    بشکن;
	}

	حالت STL_VIM_عبارت: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    صحیح	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    اگر (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    وقتی ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    اگر (*s != '}')	// missing '}' or out of space
		بشکن;
	    s++;
	    اگر (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    وگرنه
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((نویسه *)buf_tmp, اندازه(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((نویسه *)buf_tmp, اندازه(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    اگر (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    اگر (!zeropad && str != پوچ && *str != NUL)
	    {
		اگر (*skipdigits(str) == NUL)
		{
		    رقم = atoi((نویسه *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    اگر (reevaluate && str != پوچ && *str != NUL
		    && strchr((ثابت نویسه *)str, '%') != پوچ
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((ثابت نویسه *)str);
		size_t fmt_length = strlen((ثابت نویسه *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * اندازه(char_u));

		اگر (new_fmt != پوچ)
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
		    new_fmt_p = پوچ;

		    اگر (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    ادامه;
		}
	    }
# endif
	    بشکن;
	}
	حالت STL_سطر:
	    رقم = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    بشکن;

	حالت STL_NUMLINES:
	    رقم = wp->w_buffer->b_ml.ml_سطر_شمار;
	    بشکن;

	حالت STL_COLUMN:
	    رقم = (State & MODE_درج) == 0 && empty_سطر
					       ? 0 : (صحیح)wp->w_cursor.col + 1;
	    بشکن;

	حالت STL_VIRTCOL:
	حالت STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    اگر (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_درج) == 0
			       && empty_سطر ? 0 : (صحیح)wp->w_cursor.col + 1)))
		بشکن;
	    رقم = (long)virtcol;
	    بشکن;
	}

	حالت STL_PERCENTAGE:
	    رقم = calc_percentage((long)wp->w_cursor.lnum, (long)wp->w_buffer->b_ml.ml_سطر_شمار);
	    بشکن;

	حالت STL_ALTPERCENT:
	    str = buf_tmp;
	    (تهی)get_rel_pos(wp, str, TMPLEN);
	    بشکن;

	حالت STL_SHOWCMD:
	    اگر (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    بشکن;

	حالت STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    اگر (append_arg_عدد(wp, buf_tmp, اندازه(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    بشکن;

	حالت STL_KEYMAP:
	    fillable = FALSE;
	    اگر (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    بشکن;
	حالت STL_PAGENUM:
# اگر defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    رقم = printer_page_رقم;
# وگرنه
	    رقم = 0;
# endif
	    بشکن;

	حالت STL_BUFNO:
	    رقم = wp->w_buffer->b_fnum;
	    بشکن;

	حالت STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	حالت STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_سطر_or_offset(wp->w_buffer, wp->w_cursor.lnum, پوچ);
	    رقم = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_درج) == 0 && empty_سطر
				? 0 : (صحیح)wp->w_cursor.col);
# endif
	    بشکن;

	حالت STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	حالت STL_BYTEVAL:
	    رقم = byteval;
	    اگر (رقم == NL)
		رقم = 0;
	    وگرنه اگر (رقم == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		رقم = NL;
	    بشکن;

	حالت STL_ROFLAG:
	حالت STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    اگر (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    بشکن;

	حالت STL_HELPFLAG:
	حالت STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    اگر (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    بشکن;

	حالت STL_FILETYPE:
	    اگر (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((نویسه *)buf_tmp, اندازه(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    بشکن;

	حالت STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    اگر (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((نویسه *)buf_tmp, اندازه(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		برای (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    بشکن;

# اگر defined(FEAT_QUICKFIX)
	حالت STL_PREVIEWFLAG:
	حالت STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    اگر (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    بشکن;

	حالت STL_QUICKFIX:
	    اگر (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(پیام_loclist)
			    : _(پیام_qflist));
	    بشکن;
# endif

	حالت STL_MODIFIED:
	حالت STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    انتخاب ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		حالت 2: str = (char_u *)"[+]"; بشکن;
		حالت 3: str = (char_u *)",+"; بشکن;
		حالت 4: str = (char_u *)"[-]"; بشکن;
		حالت 5: str = (char_u *)",-"; بشکن;
		حالت 6: str = (char_u *)"[+-]"; بشکن;
		حالت 7: str = (char_u *)",+-"; بشکن;
	    }
	    بشکن;

	حالت STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		وقتی (*s != '#' && *s != NUL)
		    ++s;
		اگر (*s == '#')
		{
		    stl_اقلام[curitem].stl_type = Highlight;
		    stl_اقلام[curitem].stl_start = p;
		    stl_اقلام[curitem].stl_minwid = -syn_namen2id(t, (صحیح)(s - t));
		    curitem++;
		}
		اگر (*s != NUL)
		    ++s;
		ادامه;
	    }

	حالت STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    اگر (*s == ']')
	    {
		stl_اقلام[curitem].stl_type = ClickFunc;
		stl_اقلام[curitem].stl_start = p;
		stl_اقلام[curitem].stl_minwid = 0;
		stl_اقلام[curitem].stl_clickfunc = پوچ;
		s++;
		curitem++;
		ادامه;
	    }
	    // %N[FuncName] with minwid
	    اگر (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		اگر (rb != پوچ)
		{
		    stl_اقلام[curitem].stl_type = ClickFunc;
		    stl_اقلام[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_اقلام[curitem].stl_minwid = raw_minwid;
		    stl_اقلام[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    ادامه;
		}
	    }
	    ادامه;

	حالت STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    ادامه;
	}

	stl_اقلام[curitem].stl_start = p;
	stl_اقلام[curitem].stl_type = Normal;
	اگر (str != پوچ && *str)
	{
	    char_u  *t = str;

	    اگر (itemisflag)
	    {
		اگر ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    اگر (l > 0)
		prevchar_isitem = TRUE;
	    اگر (l > maxwid)
	    {
		وقتی (l >= maxwid)
		    اگر (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    وگرنه
			l -= byte2cells(*t++);
		اگر (p + 1 >= out + outlen)
		    بشکن;
		*p++ = '<';
	    }
	    اگر (minwid > 0)
	    {
		برای (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    اگر (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    وگرنه
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    وگرنه
		minwid *= -1;
	    برای (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		اگر (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		وگرنه
		    *p++ = *t;
	    }
	    برای (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	وگرنه اگر (رقم >= 0)
	{
	    صحیح	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    اگر (p + 20 >= out + outlen)
		بشکن;		// not sufficient space
	    prevchar_isitem = TRUE;
	    اگر (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    اگر (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    برای (n = رقم, l = 1; n >= nbase; n /= nbase)
		l++;
	    اگر (opt == STL_VIRTCOL_ALT)
		l++;

	    اگر (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		وقتی (l-- > maxwid)
		    رقم /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((نویسه *)p, outlen - (p - out),
		    (نویسه *)nstr, 0, رقم, n);
	    }
	    وگرنه
		p += vim_snprintf_safelen((نویسه *)p, outlen - (p - out),
		    (نویسه *)nstr, minwid, رقم);
	}
	وگرنه
	    stl_اقلام[curitem].stl_type = Empty;

	اگر (رقم >= 0 || (!itemisflag && str != پوچ && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	اگر (opt == STL_VIM_عبارت)
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
    اگر (carry_hl != پوچ)
    {
	صحیح last_hl = 0;
	برای (l = 0; l < itemcnt; l++)
	    اگر (stl_اقلام[l].stl_type == Highlight)
		last_hl = stl_اقلام[l].stl_minwid;
	*carry_hl = last_hl;
    }

    اگر (mode == STL_MODE_MULTI
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
	اگر (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = strlen((نویسه *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    وگرنه
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	اگر (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    اگر (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	برای (l = 0; l < itemcnt; l++)
	    اگر (stl_اقلام[l].stl_type == ClickFunc)
		vim_free(stl_اقلام[l].stl_clickfunc);
	اگر (rendered_height != پوچ)
	    *rendered_height = rheight;
	بازگشت 0;
    }

    width = vim_strsize(out);
    اگر (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	اگر (itemcnt == 0)
	    s = out;
	وگرنه
	{
	    برای ( ; l < itemcnt; l++)
		اگر (stl_اقلام[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_اقلام[l].stl_start;
		    بشکن;
		}
	    اگر (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_اقلام[0].stl_start;
		l = 0;
	    }
	}

	اگر (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    اگر (has_mbyte)
	    {
		s = out;
		width = 0;
		برای (;;)
		{
		    width += ptr2cells(s);
		    اگر (width >= maxwidth)
			بشکن;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		وقتی (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    وگرنه
		s = out + maxwidth - 1;
	    برای (l = 0; l < itemcnt; l++)
		اگر (stl_اقلام[l].stl_start > s)
		    بشکن;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	وگرنه
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    وقتی (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(end - p) + 1);	// +1 for NUL
	    end -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    برای (; l < itemcnt; l++)
	    {
		اگر (stl_اقلام[l].stl_start - n >= s)
		    stl_اقلام[l].stl_start -= n;
		وگرنه
		    stl_اقلام[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    وقتی (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    وگرنه اگر (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	صحیح رقم_separators = 0;

	برای (l = 0; l < itemcnt; l++)
	{
	    اگر (stl_اقلام[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[رقم_separators] = l;
		رقم_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	اگر (رقم_separators)
	{
	    صحیح standard_spaces;
	    صحیح final_spaces;

	    standard_spaces = (maxwidth - width) / رقم_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (رقم_separators - 1);
	    برای (l = 0; l < رقم_separators; l++)
	    {
		صحیح dislocation = (l == (رقم_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_اقلام[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		برای (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		برای (صحیح i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_اقلام[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    اگر (hltab != پوچ)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	برای (l = 0; l < itemcnt; l++)
	{
	    اگر (stl_اقلام[l].stl_type == Highlight)
	    {
		sp->start = stl_اقلام[l].stl_start;
		sp->userhl = stl_اقلام[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = پوچ;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    اگر (tabtab != پوچ)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	برای (l = 0; l < itemcnt; l++)
	{
	    اگر (stl_اقلام[l].stl_type == TabPage)
	    {
		sp->start = stl_اقلام[l].stl_start;
		sp->userhl = stl_اقلام[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = پوچ;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    اگر (clicktab != پوچ)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	برای (l = 0; l < itemcnt; l++)
	{
	    اگر (stl_اقلام[l].stl_type == ClickFunc)
	    {
		cp->start = stl_اقلام[l].stl_start;
		cp->funcname = stl_اقلام[l].stl_clickfunc;
		cp->minwid = stl_اقلام[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = پوچ;
	cp->funcname = پوچ;
    }
    وگرنه
    {
	// Free click function names when caller doesn't need them.
	برای (l = 0; l < itemcnt; l++)
	    اگر (stl_اقلام[l].stl_type == ClickFunc)
		vim_free(stl_اقلام[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    کلید_Typed = save_کلید_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    اگر (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    بازگشت width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    صحیح
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    صحیح		buflen)
{
    long	above; // number of lines above window
    long	below; // number of lines below window

    اگر (buflen < 3) // need at least 3 chars for writing
	بازگشت 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    اگر (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_سطر_شمار - wp->w_botline + 1;
    اگر (below <= 0)
	بازگشت (صحیح)vim_snprintf_safelen((نویسه *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    اگر (above <= 0)
	بازگشت (صحیح)vim_snprintf_safelen((نویسه *)buf, buflen,
	    "%s", _("Top"));

    صحیح perc = calc_percentage(above, above + below);
    نویسه tmp[8];
    // localized percentage value
    vim_snprintf(tmp, اندازه(tmp), _("%d%%"), perc);
    بازگشت (صحیح)vim_snprintf_safelen((نویسه *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    ایستا صحیح
append_arg_عدد(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    صحیح		add_file)	// Add "file" before the arg number
{
    اگر (ARGCOUNT <= 1)		// nothing to do
	بازگشت 0;

    نویسه *پیام;
    انتخاب ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	حالت 0: پیام = _(" (%d of %d)"); بشکن;
	حالت 1: پیام = _(" ((%d) of %d)"); بشکن;
	حالت 2: پیام = _(" (file %d of %d)"); بشکن;
	حالت 3: پیام = _(" (file (%d) of %d)"); بشکن;
    }

    بازگشت (صحیح)vim_snprintf_safelen((نویسه *)buf, buflen, پیام,
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
    بازگشت FullName_save(fname, TRUE);
#وگرنه
    اگر (!vim_isAbsName(fname)
	    || strstr((نویسه *)fname, "..") != پوچ
	    || strstr((نویسه *)fname, "//") != پوچ
# ifdef BACKSLASH_IN_FILENAME
	    || strstr((نویسه *)fname, "\\\\") != پوچ
# endif
# اگر defined(MSWIN)
	    || vim_strchr(fname, '~') != پوچ
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	بازگشت FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    اگر (fname != پوچ)
	fname_case(fname, 0);	// set correct case for file name
# endif

    بازگشت fname;
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
    تهی
fname_expand(
    buf_T	*buf UNUSED,
    char_u	**ffname,
    char_u	**sfname)
{
    اگر (*ffname == پوچ)	    // no file name given, nothing to do
	بازگشت;
    اگر (*sfname == پوچ)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    اگر (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	اگر (rfname != پوچ)
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
    تهی
ex_buffer_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    صحیح		split_ret = OK;
    صحیح		p_ea_save;
    صحیح		open_wins = 0;
    صحیح		r;
    صحیح		شمار;		// Maximum number of windows to open.
    صحیح		all;		// When TRUE also load inactive buffers.
    صحیح		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    اگر (eap->addr_شمار == 0)	// make as many windows as possible
	شمار = 9999;
    وگرنه
	شمار = eap->line2;	// make as many windows as specified
    اگر (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    وگرنه
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
    اگر (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    برای (;;)
    {
	tpnext = curtab->tp_next;
	برای (wp = firstwin; wp != پوچ; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    اگر ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		اگر (win_close(wp, FALSE) == FAIL)
		    بشکن;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    وگرنه
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	اگر (had_tab == 0 || tpnext == پوچ)
	    بشکن;
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
    برای (buf = firstbuf; buf != پوچ && open_wins < شمار; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	اگر ((!all && buf->b_ml.ml_mfp == پوچ) || !buf->b_p_bl)
	    ادامه;

	اگر (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    اگر (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    وگرنه
		wp = پوچ;
	}
	وگرنه
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		اگر (wp->w_buffer == buf)
		    بشکن;
	    // If the buffer already has a window, move it
	    اگر (wp != پوچ)
		win_move_after(wp, curwin);
	}

	اگر (wp == پوچ && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    اگر (split_ret == FAIL)
		ادامه;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    اگر (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		بشکن;
	    }
	    اگر (swap_exists_action == SEA_QUIT)
	    {
#اگر defined(FEAT_EVAL)
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

#اگر defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    وگرنه
		handle_swap_exists(پوچ);
	}

	ui_breakcheck();
	اگر (got_int)
	{
	    (تهی)vgetc();	// only break the file loading, not the rest
	    بشکن;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	اگر (aborting())
	    بشکن;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	اگر (had_tab > 0 && tabpage_index(پوچ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    برای (wp = lastwin; open_wins > شمار; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	اگر (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	وگرنه اگر (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	وگرنه
	{
	    wp = wp->w_prev;
	    اگر (wp == پوچ)
		بشکن;
	}
    }
}


ایستا صحیح  chk_modeline(linenr_T, صحیح);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    تهی
do_modelines(صحیح flags)
{
    linenr_T	lnum;
    صحیح		nmlines;
    ایستا صحیح	entered = 0;

    اگر (!curbuf->b_p_ml || (nmlines = (صحیح)p_mls) == 0)
	بازگشت;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    اگر (entered)
	بازگشت;

    ++entered;
    برای (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_سطر_شمار && lnum <= nmlines;
								       ++lnum)
	اگر (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    برای (lnum = curbuf->b_ml.ml_سطر_شمار; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_سطر_شمار - nmlines; --lnum)
	اگر (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#شامل "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    ایستا صحیح
chk_modeline(
    linenr_T	lnum,
    صحیح		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*سطر_end;		// point to the end of the line
    char_u	*e;
    صحیح		prev;
    صحیح		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    سطر_end = s + ml_get_len(lnum);
    برای (; *s != NUL; ++s)
    {
	اگر (prev == -1 || vim_isspace(prev))
	{
	    اگر ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		بشکن;
	    // Accept both "vim" and "Vim".
	    اگر ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		صحیح vers;

		اگر (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		وگرنه
		    e = s + 3;
		vers = getdigits(&e);
		اگر (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    بشکن;
	    }
	}
	prev = *s;
    }

    اگر (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	صحیح	end;

	انجامبده				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	وقتی (s[-1] != ':');

	len = (size_t)(سطر_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	اگر (linecopy == پوچ)
	    بازگشت FAIL;

	سطر_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_افزودن(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	وقتی (end == FALSE)
	{
	    s = skipwhite(s);
	    اگر (*s == NUL)
		بشکن;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    برای (e = s; *e != ':' && *e != NUL; ++e)
		اگر (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(سطر_end - (e + 1)) + 1);	// +1 for NUL
		    --سطر_end;
		}
	    اگر (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    اگر (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		اگر (*e != ':')		// no terminating ':'?
		    بشکن;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    اگر (*s != NUL)		// skip over an empty "::"
	    {
		صحیح secure_save = secure;
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
		اگر (retval == FAIL)		// stop if error found
		    بشکن;
	    }
	    s = (e == سطر_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_برداشت();
	vim_free(linecopy);
    }
    بازگشت retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    صحیح
bt_normal(buf_T *buf)
{
    بازگشت buf != پوچ && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    صحیح
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    بازگشت buf != پوچ && buf->b_p_bt[0] == 'q';
#وگرنه
    بازگشت FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    صحیح
bt_terminal(buf_T *buf UNUSED)
{
#اگر defined(FEAT_TERMINAL)
    بازگشت buf != پوچ && buf->b_p_bt[0] == 't';
#وگرنه
    بازگشت FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    صحیح
bt_help(buf_T *buf)
{
    بازگشت buf != پوچ && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    صحیح
bt_prompt(buf_T *buf)
{
    بازگشت buf != پوچ && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    صحیح
bt_popup(buf_T *buf)
{
    بازگشت buf != پوچ && buf->b_p_bt != پوچ
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    صحیح
bt_nofilename(buf_T *buf)
{
    بازگشت buf != پوچ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    ایستا صحیح
bt_nofileread(buf_T *buf)
{
    بازگشت buf != پوچ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#اگر defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    صحیح
bt_nofile(buf_T *buf)
{
    بازگشت buf != پوچ && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    صحیح
bt_dontwrite(buf_T *buf)
{
    بازگشت buf != پوچ && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    صحیح
bt_dontwrite_پیام(buf_T *buf)
{
    اگر (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    صحیح
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    انتخاب (buf->b_p_bh[0])
    {
	حالت 'u':		    // "unload"
	حالت 'w':		    // "wipe"
	حالت 'd': بازگشت FALSE;	    // "delete"
	حالت 'h': بازگشت TRUE;	    // "hide"
    }
    بازگشت (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#اگر defined(FEAT_QUICKFIX)
    اگر (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	اگر (buf->b_fnum == qf_پشته_get_bufnr())
	    بازگشت (char_u *)_(پیام_qflist);
	وگرنه
	    بازگشت (char_u *)_(پیام_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    اگر (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	اگر (buf->b_term != پوچ)
	    بازگشت term_get_status_text(buf->b_term);
#endif
	اگر (buf->b_fname != پوچ)
	    بازگشت buf->b_fname;
	اگر (buf == cmdwin_buf)
	    بازگشت (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	اگر (bt_prompt(buf))
	    بازگشت (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	اگر (bt_popup(buf))
	    بازگشت (char_u *)_("[Popup]");
#endif
	بازگشت (char_u *)_("[Scratch]");
    }

    اگر (buf->b_fname == پوچ)
	بازگشت buf_get_fname(buf);
    بازگشت پوچ;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    اگر (buf->b_fname == پوچ)
	بازگشت (char_u *)_("[No Name]");
    بازگشت buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    تهی
set_buflisted(صحیح on)
{
    اگر (on == curbuf->b_p_bl)
	بازگشت;

    curbuf->b_p_bl = on;
    اگر (on)
	apply_autocmds(EVENT_BUFADD, پوچ, پوچ, FALSE, curbuf);
    وگرنه
	apply_autocmds(EVENT_BUFDELETE, پوچ, پوچ, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    صحیح
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    صحیح		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(پوچ, پوچ, (linenr_T)1, BLN_DUMMY);
    اگر (newbuf == پوچ)
	بازگشت TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    اگر (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	بازگشت TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    اگر (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	بازگشت TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    اگر (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	اگر (buf->b_ml.ml_سطر_شمار == curbuf->b_ml.ml_سطر_شمار)
	{
	    differ = FALSE;
	    برای (lnum = 1; lnum <= curbuf->b_ml.ml_سطر_شمار; ++lnum)
		اگر (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    بشکن;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    اگر (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    بازگشت differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    تهی
wipe_buffer(
    buf_T	*buf,
    صحیح		aucmd)	    // When TRUE trigger autocommands.
{
    اگر (buf->b_fnum == بالاترین_file_رقم - 1)
	--بالاترین_file_رقم;

    اگر (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(پوچ, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    اگر (!aucmd)
	unblock_autocmds();
}
