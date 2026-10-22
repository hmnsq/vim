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

#அந்தர்பாவ "vim.h"


#ifdef FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_DEPTH 100
#endif

நிலையான வெறுமை	enter_buffer(buf_T *buf);
நிலையான வெறுமை	buflist_getfpos(வெறுமை);
நிலையான char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, முழு ignore_case);
நிலையான char_u	*fname_match(regmatch_T *rmp, char_u *name, முழு ignore_case);
#ifdef UNIX
நிலையான buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
நிலையான முழு	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
நிலையான முழு	buf_same_ino(buf_T *buf, stat_T *stp);
#இல்லையெனில்
நிலையான முழு	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
நிலையான முழு	மதிப்பு_changed(char_u *str, char_u **last);
#எனில் defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
நிலையான முழு build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, முழு opt_scope, முழு fillchar, முழு maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, முழு *lbreaks, முழு *carry_hl);
#endif
நிலையான முழு	append_arg_எண்(win_T *wp, char_u *buf, size_t buflen, முழு add_file);
நிலையான வெறுமை	free_buffer(buf_T *);
நிலையான வெறுமை	free_buffer_stuff(buf_T *buf, முழு free_options);
நிலையான முழு	bt_nofileread(buf_T *buf);
நிலையான முழு	do_buffer_ext(முழு action, முழு start, முழு dir, முழு மொத்தம், முழு flags);

#ifdef UNIX
# define dev_T dev_t
#இல்லையெனில்
# define dev_T unsigned
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    க்கு ((buf) = lastbuf; (buf) != வெற்று; (buf) = (buf)->b_prev)

#எனில் defined(FEAT_QUICKFIX)
நிலையான எழுத்து *செய்தி_loclist = N_("[Location List]");
நிலையான எழுத்து *செய்தி_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
நிலையான முழு	buf_free_மொத்தம் = 0;

நிலையான முழு	உச்சி_file_இலக்கம் = 1;	// highest file number
நிலையான garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    நிலையான வெறுமை
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"எனில் exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    நிலையான முழு
calc_percentage(long part, long whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    திரும்பு (part > 1000000L)
	? (முழு)(part / (whole / 100L))
	: (முழு)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    முழு
get_highest_fnum(வெறுமை)
{
    திரும்பு உச்சி_file_இலக்கம் - 1;
}

/*
 * Read data from buffer for retrying.
 */
    நிலையான முழு
read_buffer(
    முழு		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    முழு		flags)		    // extra flags for readfile()
{
    முழு		retval = OK;
    linenr_T	வரி_மொத்தம்;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    வரி_மொத்தம் = curbuf->b_ml.ml_வரி_மொத்தம்;
    retval = readfile(
	    read_stdin ? வெற்று : curbuf->b_ffname,
	    read_stdin ? வெற்று : curbuf->b_fname,
	    வரி_மொத்தம், (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    எனில் (retval == OK)
    {
	// Delete the binary lines.
	போது (--வரி_மொத்தம் >= 0)
	    ml_delete((linenr_T)1);
    }
    இல்லையெனில்
    {
	// Delete the converted lines.
	போது (curbuf->b_ml.ml_வரி_மொத்தம் > வரி_மொத்தம்)
	    ml_delete(வரி_மொத்தம்);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    எனில் (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	எனில் (!readonlymode && !BUFEMPTY())
	    changed();
	இல்லையெனில் எனில் (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	எனில் (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, வெற்று, வெற்று, FALSE,
							      curbuf, &retval);
#இல்லையெனில்
	    apply_autocmds(EVENT_STDINREADPOST, வெற்று, வெற்று, FALSE, curbuf);
#endif
	}
    }
    திரும்பு retval;
}

#எனில் defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    வெறுமை
buffer_ensure_loaded(buf_T *buf)
{
    எனில் (buf->b_ml.ml_mfp != வெற்று)
	திரும்பு;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    எனில் (curbuf == buf)
    {
	எனில் (swap_exists_action != SEA_READONLY)
	    swap_exists_action = SEA_NONE;
	open_buffer(FALSE, வெற்று, 0);
	aucmd_restbuf(&aco);
    }
}
#endif

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    முழு
open_buffer(
    முழு		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    முழு		flags_arg)	    // extra flags for readfile()
{
    முழு		flags = flags_arg;
    முழு		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    முழு		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    எனில் (readonlymode && curbuf->b_ffname != வெற்று
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    எனில் (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    எனில் (curbuf->b_ml.ml_mfp != வெற்று)
		நிறுத்து;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	எனில் (curbuf == வெற்று)
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
	எனில் (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(வெற்று, curwin);
#endif
	திரும்பு FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    எனில் (curbuf->b_ml.ml_mfp != வெற்று)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    எனில் (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    எனில் (curbuf->b_ffname != வெற்று
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	முழு old_செய்தி_silent = செய்தி_silent;
#ifdef UNIX
	முழு	save_bin = curbuf->b_p_bin;
	முழு perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	முழு oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	எனில் (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	எனில் (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	எனில் (shortmess(SHM_FILEINFO))
	    செய்தி_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	எனில் (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    எனில் (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_buffer(FALSE, eap, flags);
	}
#endif
	செய்தி_silent = old_செய்தி_silent;
#ifdef FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#endif
	// Help buffer is filtered.
	எனில் (bt_help(curbuf))
	    fix_help_buffer();
    }
    இல்லையெனில் எனில் (read_stdin)
    {
	முழு	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(வெற்று, வெற்று, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, வெற்று,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	எனில் (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    எனில் (curbuf->b_ml.ml_mfp != வெற்று
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    எனில் (curbuf->b_flags & BF_NEVERLOADED)
    {
	(வெறுமை)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    எனில் ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != வெற்று)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != வெற்று)
#endif
       )
	changed();
    இல்லையெனில் எனில் (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    எனில் (aborting())
#இல்லையெனில்
    எனில் (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    எனில் (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, வெற்று, வெற்று, FALSE, curbuf, &retval);
#இல்லையெனில்
    apply_autocmds(EVENT_BUFENTER, வெற்று, வெற்று, FALSE, curbuf);
#endif

    எனில் (retval != OK)
	திரும்பு retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    எனில் (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != வெற்று)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	எனில் (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    எனில் ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, வெற்று, வெற்று,
			FALSE, curbuf, &retval);
#இல்லையெனில்
	    apply_autocmds(EVENT_BUFWINENTER, வெற்று, வெற்று,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    திரும்பு retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    வெறுமை
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == வெற்று ? 0 : buf->b_fnum;
    bufref->br_buf_free_மொத்தம் = buf_free_மொத்தம்;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    முழு
bufref_valid(bufref_T *bufref)
{
    திரும்பு bufref->br_buf_free_மொத்தம் == buf_free_மொத்தம்
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    முழு
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	எனில் (bp == buf)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
நிலையான hashtab_T buf_hashtab;

    நிலையான வெறுமை
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((எழுத்து *)buf->b_சாவி, அளவு(buf->b_சாவி), "%x", buf->b_fnum);
    எனில் (hash_add(&buf_hashtab, buf->b_சாவி, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    நிலையான வெறுமை
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_சாவி);

    எனில் (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    நிலையான முழு
can_unload_buffer(buf_T *buf)
{
    முழு	    can_unload = !buf->b_locked;

    எனில் (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    எனில் (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		நிறுத்து;
	    }
    }
    // Don't unload the buffer while it's still being saved
    எனில் (can_unload && buf->b_saving)
	can_unload = FALSE;

    எனில் (!can_unload)
    {
	char_u *fname = buf->b_fname != வெற்று ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != வெற்று ? fname : (char_u *)"[No Name]");
    }
    திரும்பு can_unload;
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
    முழு
close_buffer(
    win_T	*win,
    buf_T	*buf,
    முழு		action,
    முழு		abort_if_last,
    முழு		ignore_abort,
    முழு		set_context)
{
    முழு		hiding_buf;
    bufref_T	bufref;
    முழு		is_curwin = (curwin != வெற்று && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    முழு		unload_buf = (action != 0);
    முழு		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    முழு		del_buf = (action == DOBUF_DEL || wipe_buf);
    முழு		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    முழு		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    எனில் (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    இல்லையெனில் எனில் (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    இல்லையெனில் எனில் (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    எனில் (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	எனில் (term_job_running(buf->b_term))
	{
	    எனில் (wipe_buf || unload_buf)
	    {
		எனில் (!can_unload_buffer(buf))
		    திரும்பு FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    இல்லையெனில்
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	இல்லையெனில் எனில் (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	இல்லையெனில்
	{
	    எனில் (del_buf || unload_buf)
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
    எனில் ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	திரும்பு FALSE;

    எனில் (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	எனில் (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    எனில் ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	எனில் (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    திரும்பு FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	எனில் (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    செல் aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	எனில் (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    எனில் (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		செல் aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    எனில் (abort_if_last && one_window())
		// Autocommands made this the only window.
		செல் aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	எனில் (!ignore_abort && aborting())
	    திரும்பு FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    எனில் (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    எனில் (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    எனில் (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    எனில் (!hiding_buf || !unload_buf)
	திரும்பு FALSE;

    // Always remove the buffer when there is no file name.
    எனில் (buf->b_ffname == வெற்று)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    எனில் (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	திரும்பு FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    எனில் ((win_valid || closed_popup) && win->w_buffer == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_buffer = வெற்று;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    எனில் (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != வெற்று || buf->b_next != வெற்று
#எனில் defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	எனில் (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    எனில் (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, அளவு(முழு), 50);
	    எனில் (ga_grow(&buf_reuse, 1) == OK)
		((முழு *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	எனில் (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	இல்லையெனில்
	    buf->b_sfname = வெற்று;
	VIM_CLEAR(buf->b_ffname);
	எனில் (buf->b_prev == வெற்று)
	    firstbuf = buf->b_next;
	இல்லையெனில்
	    buf->b_prev->b_next = buf->b_next;
	எனில் (buf->b_next == வெற்று)
	    lastbuf = buf->b_prev;
	இல்லையெனில்
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    இல்லையெனில்
    {
	எனில் (del_buf)
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
	எனில் (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    ப்ரதிஜ்ஞா(!win_valid || !is_curwin || win == curwin);
    திரும்பு TRUE;
}

/*
 * Make buffer not contain a file.
 */
    வெறுமை
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_வரி_மொத்தம் = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_start_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_start_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_start_bomb = FALSE;
    buf->b_ml.ml_mfp = வெற்று;
    buf->b_ml.ml_flags = ML_EMPTY;		// empty buffer
#ifdef FEAT_NETBEANS_INTG
    netbeans_deleted_all_வரிகள்(buf);
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
    முழு
buf_freeall(buf_T *buf, முழு flags)
{
    முழு		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    முழு		is_curwin = (curwin != வெற்று && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    எனில் (buf->b_ml.ml_mfp != வெற்று)
    {
	எனில் (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    திரும்பு FALSE;
    }
    எனில் ((flags & BFA_DEL) && buf->b_p_bl)
    {
	எனில் (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    திரும்பு FALSE;
    }
    எனில் (flags & BFA_WIPE)
    {
	எனில் (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    திரும்பு FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    எனில் (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    எனில் ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	திரும்பு FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    எனில் (buf == curbuf && !is_curbuf)
	திரும்பு FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    எனில் (buf == curbuf && VIsual_active
#எனில் defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    எனில் (curwin != வெற்று && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    எனில் (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_வரி_மொத்தம் = 0;    // no lines in buffer
    எனில் ((flags & BFA_KEEP_UNDO) == 0)
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
    திரும்பு TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    நிலையான வெறுமை
free_buffer(buf_T *buf)
{
    ++buf_free_மொத்தம்;
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

    எனில் (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    இல்லையெனில்
    {
	vim_free(buf);
	எனில் (curbuf == buf)
	    curbuf = வெற்று;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    நிலையான வெறுமை
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_எண்;
    di->di_tv.v_lock = VAR_FIXED;
    di->di_tv.vval.v_எண் = 0;

#ifdef FEAT_EVAL
    STRCPY(buf->b_ct_di.di_சாவி, "changedtick");
    (வெறுமை)dict_add(buf->b_vars, di);
#endif
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    நிலையான வெறுமை
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    போது (buf->b_wininfo != வெற்று)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_next;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    நிலையான வெறுமை
free_buffer_stuff(
    buf_T	*buf,
    முழு		free_options)		// free options as well
{
    எனில் (free_options)
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
    வெறுமை
free_wininfo(wininfo_T *wip)
{
    எனில் (wip->wi_optset)
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
    வெறுமை
goto_buffer(
    exarg_T	*eap,
    முழு		start,
    முழு		dir,
    முழு		மொத்தம்)
{
    bufref_T	old_curbuf;
    முழு		save_sea = swap_exists_action;
    முழு		skip_help_buf;

    தேர்வு (eap->cmdidx)
    {
	நிலை CMD_bnext:
	நிலை CMD_sbnext:
	நிலை CMD_bNext:
	நிலை CMD_bprevious:
	நிலை CMD_sbNext:
	நிலை CMD_sbprevious:
	    skip_help_buf = TRUE;
	    நிறுத்து;
	இயல்பு:
	    skip_help_buf = FALSE;
	    நிறுத்து;
    }

    set_bufref(&old_curbuf, curbuf);

    எனில் (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (வெறுமை)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, மொத்தம்,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    எனில் (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#எனில் defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#எனில் defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    இல்லையெனில்
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    வெறுமை
handle_swap_exists(bufref_T *old_curbuf)
{
#எனில் defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    எனில் (swap_exists_action == SEA_QUIT)
    {
#எனில் defined(FEAT_EVAL)
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
	எனில் (old_curbuf == வெற்று || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(வெற்று, வெற்று, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	இல்லையெனில்
	    buf = old_curbuf->br_buf;
	எனில் (buf != வெற்று)
	{
	    முழு old_செய்தி_silent = செய்தி_silent;

	    எனில் (shortmess(SHM_FILEINFO))
		செய்தி_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    செய்தி_silent = old_செய்தி_silent;

#ifdef FEAT_SYN_HL
	    எனில் (old_tw != curbuf->b_p_tw)
		check_colorcolumn(வெற்று, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#எனில் defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    இல்லையெனில் எனில் (swap_exists_action == SEA_RECOVER)
    {
#எனில் defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Recover at ATTENTION prompt.
	செய்தி_scroll = TRUE;
	ml_recover(FALSE);
	செய்தி_puts("\n");	// don't overwrite the last message
	cmdline_row = செய்தி_row;
	do_modelines(0);

#எனில் defined(FEAT_EVAL)
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
    நிலையான முழு
empty_curbuf(
    முழு close_others,
    முழு forceit,
    முழு action)
{
    முழு	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    எனில் (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	திரும்பு FAIL;
    }

    set_bufref(&bufref, buf);
    எனில் (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, வெற்று, வெற்று, வெற்று, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    எனில் (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(வெற்று, buf, action, FALSE, FALSE, FALSE);
    எனில் (!close_others)
	need_fileinfo = FALSE;
    இல்லையெனில் எனில் (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    திரும்பு retval;
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
    நிலையான முழு
do_buffer_ext(
    முழு		action,
    முழு		start,
    முழு		dir,		// FORWARD or BACKWARD
    முழு		மொத்தம்,		// buffer number or number of buffers
    முழு		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    முழு		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    தேர்வு (start)
    {
	நிலை DOBUF_FIRST:   buf = firstbuf; நிறுத்து;
	நிலை DOBUF_LAST:    buf = lastbuf;  நிறுத்து;
	இயல்பு:	    buf = curbuf;   நிறுத்து;
    }
    எனில் (start == DOBUF_MOD)	    // find next modified buffer
    {
	போது (மொத்தம்-- > 0)
	{
	    செய்
	    {
		buf = buf->b_next;
		எனில் (buf == வெற்று)
		    buf = firstbuf;
	    }
	    போது (buf != curbuf && !bufIsChanged(buf));
	}
	எனில் (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    திரும்பு FAIL;
	}
    }
    இல்லையெனில் எனில் (start == DOBUF_FIRST && மொத்தம்) // find specified buffer number
    {
	போது (buf != வெற்று && buf->b_fnum != மொத்தம்)
	    buf = buf->b_next;
    }
    இல்லையெனில்
    {
	முழு help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = வெற்று;
	போது (மொத்தம் > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    எனில் (bp == வெற்று)
		bp = buf;
	    எனில் (dir == FORWARD)
	    {
		buf = buf->b_next;
		எனில் (buf == வெற்று)
		    buf = firstbuf;
	    }
	    இல்லையெனில்
	    {
		buf = buf->b_prev;
		எனில் (buf == வெற்று)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    எனில் (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--மொத்தம்;
		bp = வெற்று;	// use this buffer as new starting point
	    }
	    எனில் (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		திரும்பு FAIL;
	    }
	}
    }

    எனில் (buf == வெற்று)	    // could not find it
    {
	எனில் (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    எனில் (!unload)
		semsg(_(e_buffer_nr_does_not_exist), மொத்தம்);
	}
	இல்லையெனில் எனில் (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	இல்லையெனில்
	    emsg(_(e_cannot_go_before_first_buffer));
	திரும்பு FAIL;
    }
#ifdef FEAT_PROP_POPUP
    எனில் ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	திரும்பு OK;
#endif
    எனில் (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	திரும்பு FAIL;

    எனில் ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), மொத்தம்);
	திரும்பு FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    எனில் (unload)
    {
	முழு	forward;
	bufref_T bufref;

	எனில் (!can_unload_buffer(buf))
	    திரும்பு FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	எனில் (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == வெற்று && !buf->b_p_bl)
	    திரும்பு FAIL;

	எனில் ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    எனில் ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		எனில் (term_job_running(buf->b_term))
		{
		    எனில் (term_confirm_stop(buf) == FAIL)
			திரும்பு FAIL;
		}
		இல்லையெனில்
# endif
		{
		    dialog_changed(buf, FALSE);
		    எனில் (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			திரும்பு FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    எனில் (bufIsChanged(buf))
			திரும்பு FAIL;
		}
	    }
	    இல்லையெனில்
#endif
	    {
		no_write_message_buf(buf);
		திரும்பு FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	எனில் (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    எனில் (bp->b_p_bl && bp != buf)
		நிறுத்து;
	எனில் (bp == வெற்று && buf == curbuf)
	    திரும்பு empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	போது (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != வெற்று))
	{
	    எனில் (win_close(curwin, FALSE) == FAIL)
		நிறுத்து;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	எனில் (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    எனில் (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(வெற்று, buf, action, FALSE, FALSE, FALSE);
	    திரும்பு OK;
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
	buf = வெற்று;	// selected buffer
	bp = வெற்று;	// used when no loaded buffer found
	எனில் (au_new_curbuf.br_buf != வெற்று && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	இல்லையெனில் எனில் (curwin->w_jumplistlen > 0)
	{
	    முழு     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    எனில் (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    போது (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		எனில் (buf != வெற்று)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    எனில் (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = வெற்று;
		    இல்லையெனில் எனில் (buf->b_ml.ml_mfp == வெற்று)
		    {
			// skip unloaded buf, but may keep it for later
			எனில் (bp == வெற்று)
			    bp = buf;
			buf = வெற்று;
		    }
		}
		எனில் (buf != வெற்று)   // found a valid buffer: stop searching
		    நிறுத்து;
		// advance to older entry in jump list
		எனில் (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    நிறுத்து;
		எனில் (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		எனில் (jumpidx == forward)		// List exhausted for sure
		    நிறுத்து;
	    }
	}

	எனில் (buf == வெற்று)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    க்கு (;;)
	    {
		எனில் (buf == வெற்று)
		{
		    எனில் (!forward)	// tried both directions
			நிறுத்து;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    தொடர்;
		}
		// in non-help buffer, try to skip help buffers, and vv
		எனில் (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    எனில் (buf->b_ml.ml_mfp != வெற்று)   // found loaded buffer
			நிறுத்து;
		    எனில் (bp == வெற்று)	// remember unloaded buf for later
			bp = buf;
		}
		எனில் (forward)
		    buf = buf->b_next;
		இல்லையெனில்
		    buf = buf->b_prev;
	    }
	}
	எனில் (buf == வெற்று)	// No loaded buffer, use unloaded one
	    buf = bp;
	எனில் (buf == வெற்று)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		எனில் (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    நிறுத்து;
	}
	எனில் (buf == வெற்று)	// Still no buffer, just take one
	{
	    எனில் (curbuf->b_next != வெற்று)
		buf = curbuf->b_next;
	    இல்லையெனில்
		buf = curbuf->b_prev;
	    எனில் (bt_quickfix(buf)
		    || (buf != வெற்று && buf != curbuf && buf->b_locked_split))
		buf = வெற்று;
	}
    }

    எனில் (buf == வெற்று)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	திரும்பு empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    எனில் (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != வெற்று)
	திரும்பு OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    எனில் (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	திரும்பு FAIL;
    }
    எனில் (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	திரும்பு FAIL;

    // go to current buffer - nothing to do
    எனில் (buf == curbuf)
	திரும்பு OK;

    // Check if the current buffer may be abandoned.
    எனில் (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	எனில் ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    எனில் (term_job_running(curbuf->b_term))
	    {
		எனில் (term_confirm_stop(curbuf) == FAIL)
		    திரும்பு FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    இல்லையெனில்
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		எனில் (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    திரும்பு FAIL;

		எனில் (bufIsChanged(curbuf))
		{
		    no_write_message();
		    திரும்பு FAIL;
		}
	    }
	}
	இல்லையெனில்
#endif
	{
	    no_write_message();
	    திரும்பு FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    எனில் (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#எனில் defined(FEAT_EVAL)
    எனில் (aborting())	    // autocmds may abort script processing
	திரும்பு FAIL;
#endif

    திரும்பு OK;
}

    முழு
do_buffer(
    முழு		action,
    முழு		start,
    முழு		dir,		// FORWARD or BACKWARD
    முழு		மொத்தம்,		// buffer number or number of buffers
    முழு		forceit)	// TRUE when using !
{
    திரும்பு do_buffer_ext(action, start, dir, மொத்தம்,
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
    எழுத்து *
do_bufdel(
    முழு		command,
    char_u	*arg,		// pointer to extra arguments
    முழு		addr_மொத்தம்,
    முழு		start_bnr,	// first buffer number in a range
    முழு		end_bnr,	// buffer nr or last buffer nr in a range
    முழு		forceit)
{
    முழு		do_current = 0;	// delete current buffer?
    முழு		deleted = 0;	// number of buffers deleted
    எழுத்து	*errormsg = வெற்று; // return value
    முழு		bnr;		// buffer number
    char_u	*p;

    எனில் (addr_மொத்தம் == 0)
    {
	(வெறுமை)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    இல்லையெனில்
    {
	எனில் (addr_மொத்தம் == 2)
	{
	    எனில் (*arg)		// both range and argument is not allowed
		திரும்பு ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	இல்லையெனில்	// addr_count == 1
	    bnr = end_bnr;

	க்கு ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    எனில் (bnr == curbuf->b_fnum)
		do_current = bnr;
	    இல்லையெனில் எனில் (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    எனில் (addr_மொத்தம் == 2)
	    {
		எனில் (++bnr > end_bnr)
		    நிறுத்து;
	    }
	    இல்லையெனில்    // addr_count == 1
	    {
		arg = skipwhite(arg);
		எனில் (*arg == NUL)
		    நிறுத்து;
		எனில் (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    எனில் (bnr < 0)	    // failed
			நிறுத்து;
		    arg = p;
		}
		இல்லையெனில்
		    bnr = getdigits(&arg);
	    }
	}
	எனில் (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	எனில் (deleted == 0)
	{
	    எனில் (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    இல்லையெனில் எனில் (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    இல்லையெனில்
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (எழுத்து *)IObuff;
	}
	இல்லையெனில் எனில் (deleted >= p_report)
	{
	    எனில் (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    இல்லையெனில் எனில் (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    இல்லையெனில்
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    திரும்பு errormsg;
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
    வெறுமை
set_curbuf(buf_T *buf, முழு action)
{
    buf_T	*prevbuf;
    முழு		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    முழு		valid;

    setpcmark();
    எனில் ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    எனில் (!apply_autocmds(EVENT_BUFLEAVE, வெற்று, வெற்று, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	எனில் (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	எனில் (unload)
	    close_windows(prevbuf, FALSE);
#எனில் defined(FEAT_EVAL)
	எனில் (bufref_valid(&prevbufref) && !aborting())
#இல்லையெனில்
	எனில் (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    எனில் (prevbuf == curbuf
		    && ((State & MODE_செருகு) == 0 || curbuf->b_nwindows <= 1))
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
    எனில் ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == வெற்று)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	எனில் (!valid)
	    enter_buffer(lastbuf);
	இல்லையெனில்
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	எனில் (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(வெற்று, curwin);
#endif
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    நிலையான வெறுமை
enter_buffer(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    எனில் (VIsual_active
#எனில் defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    எனில் (curwin->w_buffer != வெற்று)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    எனில் (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    இல்லையெனில்
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    எனில் (curwin->w_p_diff)
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
    எனில் (curbuf->b_ml.ml_mfp == வெற்று)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	எனில் (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, வெற்று, 0);
    }
    இல்லையெனில்
    {
	எனில் (!செய்தி_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(வெறுமை)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
	apply_autocmds(EVENT_BUFENTER, வெற்று, வெற்று, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, வெற்று, வெற்று, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    எனில் (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    எனில் (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    எனில் (curbuf->b_kmap_state & KEYMAP_INIT)
	(வெறுமை)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    எனில் (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(வெறுமை)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#எனில் defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    வெறுமை
do_autochdir(வெறுமை)
{
    எனில் ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != வெற்று
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	last_chdir_reason = "autochdir";
    }
}
#endif

    வெறுமை
no_write_message_buf(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    எனில் (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    இல்லையெனில்
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    வெறுமை
no_write_message(வெறுமை)
{
#ifdef FEAT_TERMINAL
    எனில் (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    இல்லையெனில்
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    வெறுமை
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    எனில் (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    இல்லையெனில்
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
    முழு
curbuf_reusable(வெறுமை)
{
    திரும்பு (curbuf != வெற்று
	&& curbuf->b_ffname == வெற்று
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == வெற்று || BUFEMPTY())
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
    முழு		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    எனில் (உச்சி_file_இலக்கம் == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    எனில் (sfname == வெற்று || mch_stat((எழுத்து *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    எனில் (ffname != வெற்று && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#இல்லையெனில்
		buflist_findname(ffname)
#endif
		) != வெற்று)
    {
	vim_free(ffname);
	எனில் (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? வெற்று : curwin,
						      lnum, (colnr_T)0, FALSE);

	எனில் ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	எனில் ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    எனில் (!(flags & BLN_DUMMY))
	    {
		எனில் (apply_autocmds(EVENT_BUFADD, வெற்று, வெற்று, FALSE, buf)
			&& !bufref_valid(&bufref))
		    திரும்பு வெற்று;
	    }
	}
	திரும்பு buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = வெற்று;
    எனில் ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	எனில் (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    திரும்பு வெற்று;
	}
#endif
	எனில் (!bufref_valid(&bufref))
	    buf = வெற்று;		// buf was deleted; allocate a new buffer
    }
    எனில் (buf != curbuf || curbuf == வெற்று)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	எனில் (buf == வெற்று)
	{
	    vim_free(ffname);
	    திரும்பு வெற்று;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	எனில் (buf->b_vars == வெற்று)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    திரும்பு வெற்று;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    எனில் (ffname != வெற்று)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    எனில் ((ffname != வெற்று && (buf->b_ffname == வெற்று || buf->b_sfname == வெற்று))
	    || buf->b_wininfo == வெற்று)
    {
	எனில் (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	இல்லையெனில்
	    buf->b_sfname = வெற்று;
	VIM_CLEAR(buf->b_ffname);
	எனில் (buf != curbuf)
	    free_buffer(buf);
	திரும்பு வெற்று;
    }

    எனில் (buf == curbuf)
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
    இல்லையெனில்
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = வெற்று;
	எனில் (firstbuf == வெற்று)		// buffer list is empty
	{
	    buf->b_prev = வெற்று;
	    firstbuf = buf;
	}
	இல்லையெனில்				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	எனில் ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((முழு *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    போது (buf->b_prev != வெற்று && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		எனில் (prev->b_next != வெற்று)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		எனில் (buf->b_prev != வெற்று)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		எனில் (lastbuf == buf)
		    lastbuf = prev;
		எனில் (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	இல்லையெனில்
	    buf->b_fnum = உச்சி_file_இலக்கம்++;
	எனில் (உச்சி_file_இலக்கம் < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    எனில் (emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    உச்சி_file_இலக்கம் = 1;
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
    எனில் (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    இல்லையெனில்
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    எனில் (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    எனில் (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	எனில் (apply_autocmds(EVENT_BUFNEW, வெற்று, வெற்று, FALSE, buf)
		&& !bufref_valid(&bufref))
	    திரும்பு வெற்று;
	எனில் (flags & BLN_LISTED)
	{
	    எனில் (apply_autocmds(EVENT_BUFADD, வெற்று, வெற்று, FALSE, buf)
		    && !bufref_valid(&bufref))
		திரும்பு வெற்று;
	}
#ifdef FEAT_EVAL
	எனில் (aborting())		// autocmds may abort script processing
	    திரும்பு வெற்று;
#endif
    }

    திரும்பு buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    வெறுமை
free_buf_options(
    buf_T	*buf,
    முழு		free_p_ff)
{
    எனில் (free_p_ff)
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
#எனில் defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#எனில் defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#எனில் defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#எனில் defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    எனில் (buf->b_p_சாவி != வெற்று && *buf->b_p_சாவி != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_சாவி, STRLEN(buf->b_p_சாவி));
# endif
    clear_string_option(&buf->b_p_சாவி);
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
    buf->b_s.b_cap_prog = வெற்று;
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
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_மொத்தம்);
    buf->b_p_cpt_மொத்தம் = 0;
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
    முழு
buflist_getfile(
    முழு		n,
    linenr_T	lnum,
    முழு		options,
    முழு		forceit)
{
    buf_T	*buf;
    win_T	*wp = வெற்று;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    எனில் (buf == வெற்று)
    {
	எனில் ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	இல்லையெனில்
	    semsg(_(e_buffer_nr_not_found), n);
	திரும்பு FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    எனில் (buf == curbuf)
	திரும்பு OK;

    எனில் (text_or_buf_locked())
	திரும்பு FAIL;

    // altfpos may be changed by getfile(), get it now
    எனில் (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    இல்லையெனில்
	col = 0;

    எனில் (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	எனில் (wp == வெற்று && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    எனில் (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    இல்லையெனில் எனில் (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		திரும்பு FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    முழு retval = FAIL;
    எனில் (GETFILE_SUCCESS(getfile(buf->b_fnum, வெற்று, வெற்று,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	எனில் (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    எனில் (RedrawingDisabled > 0)
	--RedrawingDisabled;
    திரும்பு retval;
}

/*
 * go to the last know line number for the current buffer
 */
    நிலையான வெறுமை
buflist_getfpos(வெறுமை)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    எனில் (p_sol)
	curwin->w_cursor.col = 0;
    இல்லையெனில்
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
    buf_T	*buf = வெற்று;

    // First make the name into a full path name
    ffname = FullName_save(fname,
#ifdef UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#இல்லையெனில்
	    FALSE
#endif
	    );
    எனில் (ffname != வெற்று)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    திரும்பு buf;
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

    எனில் (mch_stat((எழுத்து *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    திரும்பு buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    நிலையான buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#endif
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_LAST(buf)
	எனில் ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    திரும்பு buf;
    திரும்பு வெற்று;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    முழு
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    முழு		unlisted,	// find unlisted buffers
    முழு		diffmode UNUSED, // find diff-mode buffers only
    முழு		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    முழு		match = -1;
    முழு		find_listed;
    char_u	*pat;
    char_u	*patend;
    முழு		attempt;
    char_u	*p;
    முழு		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    எனில் ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	எனில் (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	இல்லையெனில்
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	எனில் (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
    இல்லையெனில்
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, வெற்று, FALSE);
	எனில் (pat == வெற்று)
	    திரும்பு -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	க்கு (;;)
	{
	    க்கு (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		எனில் (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		எனில் (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    எனில் (regmatch.regprog == வெற்று)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			திரும்பு -1;
		    }
		    எனில் (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != வெற்று)
		    {
			எனில் (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				எனில் (wp->w_buffer == buf)
				    நிறுத்து;
			    எனில் (wp == வெற்று)
				தொடர்;
			}
			எனில் (match >= 0)		// already found a match
			{
			    match = -2;
			    நிறுத்து;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		எனில் (match >= 0)			// found one match
		    நிறுத்து;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    எனில் (!unlisted || !find_listed || match != -1)
		நிறுத்து;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    எனில் (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    இல்லையெனில் எனில் (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    திரும்பு match;
}

#ifdef FEAT_VIMINFO
வகைவரை கட்டமைப்பு {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    முழு
ExpandBufnames(
    char_u	*pat,
    முழு		*இலக்கம்_file,
    char_u	***file,
    முழு		options)
{
    முழு		மொத்தம்;
    buf_T	*buf;
    முழு		round;
    char_u	*p;
    char_u	*patc = வெற்று;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = வெற்று;
#endif
    முழு		fuzzy;
    fuzmatch_str_T  *fuzmatch = வெற்று;
    regmatch_T	regmatch;
    முழு		score = 0;
    முழு		to_free = FALSE;

    *இலக்கம்_file = 0;		    // return values in case of FAIL
    *file = வெற்று;

#ifdef FEAT_DIFF
    எனில் ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	திரும்பு FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    எனில் (!fuzzy)
    {
	எனில் (*pat == '^' && pat[1] != NUL)
	{
	    முழு  len = (முழு)STRLEN(pat);
	    patc = alloc(len);
	    எனில் (patc == வெற்று)
		திரும்பு FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	இல்லையெனில் எனில் (*pat == '^')
	    patc = (char_u *)"";
	இல்லையெனில்
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    க்கு (round = 1; round <= 2; ++round)
    {
	மொத்தம் = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    எனில் (!buf->b_p_bl)	// skip unlisted buffers
		தொடர்;
#ifdef FEAT_DIFF
	    எனில் (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		எனில் (buf == curbuf || !diff_mode_buf(buf))
		    தொடர்;
#endif

	    எனில் (!fuzzy)
	    {
		எனில் (regmatch.regprog == வெற்று)
		{
		    // invalid pattern, possibly after recompiling
		    எனில் (to_free)
			vim_free(patc);
		    திரும்பு FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    இல்லையெனில்
	    {
		p = வெற்று;
		// first try matching with the short file name
		எனில் ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		எனில் (p == வெற்று)
		{
		    // next try matching with the full path file name
		    எனில் ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    எனில் (p == வெற்று)
		தொடர்;

	    எனில் (round == 1)
	    {
		++மொத்தம்;
		தொடர்;
	    }

	    எனில் (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    இல்லையெனில்
		p = vim_strsave(p);
	    எனில் (p == வெற்று)
	    {
		எனில் (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, மொத்தம்);
		திரும்பு FAIL;
	    }

	    எனில் (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		எனில் (matches != வெற்று)
		{
		    matches[மொத்தம்].buf = buf;
		    matches[மொத்தம்].match = p;
		    மொத்தம்++;
		}
		இல்லையெனில்
#endif
		    (*file)[மொத்தம்++] = p;
	    }
	    இல்லையெனில்
	    {
		fuzmatch[மொத்தம்].idx = மொத்தம்;
		fuzmatch[மொத்தம்].str = p;
		fuzmatch[மொத்தம்].score = score;
		மொத்தம்++;
	    }
	}
	எனில் (மொத்தம் == 0)	// no match found, break here
	    நிறுத்து;
	எனில் (round == 1)
	{
	    எனில் (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, மொத்தம்);
		எனில் (*file == வெற்று)
		{
		    vim_regfree(regmatch.regprog);
		    எனில் (to_free)
			vim_free(patc);
		    திரும்பு FAIL;
		}
#ifdef FEAT_VIMINFO
		எனில் (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, மொத்தம்);
#endif
	    }
	    இல்லையெனில்
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, மொத்தம்);
		எனில் (fuzmatch == வெற்று)
		{
		    *இலக்கம்_file = 0;
		    *file = வெற்று;
		    திரும்பு FAIL;
		}
	    }
	}
    }

    எனில் (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	எனில் (to_free)
	    vim_free(patc);
    }

    எனில் (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	எனில் (matches != வெற்று)
	{
	    முழு i;
	    எனில் (மொத்தம் > 1)
		qsort(matches, மொத்தம், அளவு(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    எனில் (matches[0].buf == curbuf)
	    {
		க்கு (i = 1; i < மொத்தம்; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[மொத்தம்-1] = matches[0].match;
	    }
	    இல்லையெனில்
	    {
		க்கு (i = 0; i < மொத்தம்; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    இல்லையெனில்
    {
	எனில் (fuzzymatches_to_strmatches(fuzmatch, file, மொத்தம், FALSE) == FAIL)
	    திரும்பு FAIL;
    }

    *இலக்கம்_file = மொத்தம்;
    திரும்பு (மொத்தம் == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    நிலையான char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    முழு		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    எனில் (match == வெற்று && rmp->regprog != வெற்று)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    திரும்பு match;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    நிலையான char_u *
fname_match(
    regmatch_T	*rmp,
    char_u	*name,
    முழு		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = வெற்று;
    char_u	*p;

    // extra check for valid arguments
    எனில் (name == வெற்று || rmp->regprog == வெற்று)
	திரும்பு வெற்று;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    எனில் (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    இல்லையெனில் எனில் (rmp->regprog != வெற்று)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(வெற்று, name);
	எனில் (p != வெற்று && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    திரும்பு match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(முழு nr)
{
    char_u	சாவி[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    எனில் (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((எழுத்து *)சாவி, அளவு(சாவி), "%x", nr);
    hi = hash_find(&buf_hashtab, சாவி);

    எனில் (!HASHITEM_EMPTY(hi))
	திரும்பு (buf_T *)(hi->hi_சாவி
			     - ((unsigned)(curbuf->b_சாவி - (char_u *)curbuf)));
    திரும்பு வெற்று;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    முழு		n,
    முழு		fullname,
    முழு		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    எனில் (buf == வெற்று)
	திரும்பு வெற்று;
    திரும்பு home_replace_save(helptail ? buf : வெற்று,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    வெறுமை
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	col,
    முழு		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	எனில் (wip->wi_win == win)
	    நிறுத்து;
    எனில் (wip == வெற்று)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	எனில் (wip == வெற்று)
	    திரும்பு;
	wip->wi_win = win;
	எனில் (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    இல்லையெனில்
    {
	// remove the entry from the list
	எனில் (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	இல்லையெனில்
	    buf->b_wininfo = wip->wi_next;
	எனில் (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	எனில் (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    எனில் (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    எனில் (win != வெற்று)
	wip->wi_changelistidx = win->w_changelistidx;
    எனில் (copy_options && win != வெற்று)
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
    wip->wi_prev = வெற்று;
    எனில் (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    நிலையான முழு
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    எனில் (!wip->wi_opt.wo_diff)
	திரும்பு FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	எனில் (wip->wi_win == wp)
	    திரும்பு FALSE;
    திரும்பு TRUE;
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
    நிலையான wininfo_T *
find_wininfo(
    buf_T	*buf,
    முழு		need_options,
    முழு		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	எனில் (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    நிறுத்து;

    எனில் (wip != வெற்று)
	திரும்பு wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    எனில் (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    எனில் (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != வெற்று
			    && wip->wi_win->w_buffer == buf)))
		நிறுத்து;
    }
    இல்லையெனில்
#endif
	wip = buf->b_wininfo;
    திரும்பு wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    வெறுமை
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
    clearFolding(curwin);
#endif

    wip = find_wininfo(buf, TRUE, TRUE);
    எனில் (wip != வெற்று && wip->wi_win != வெற்று
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
    இல்லையெனில் எனில் (wip != வெற்று && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    இல்லையெனில்
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    எனில் (wip != வெற்று)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    எனில் (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    எனில் (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    எனில் (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	எனில் (p_ea)
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
    நிலையான pos_T no_position = {1, 0, 0};

    wip = find_wininfo(buf, FALSE, FALSE);
    எனில் (wip != வெற்று)
	திரும்பு &(wip->wi_fpos);
    இல்லையெனில்
	திரும்பு &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    திரும்பு buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    வெறுமை
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    முழு		len;
    முழு		i;
    முழு		ro_char;
    முழு		changed_char;
#ifdef FEAT_TERMINAL
    முழு		job_running;
    முழு		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = வெற்று, **p;

    எனில் (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, அளவு(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    எனில் (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_data, (size_t)buflist.ga_len,
		அளவு(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    க்கு (; buf != வெற்று && !got_int; buf = buflist_data != வெற்று
	    ? (++p < buflist_data + buflist.ga_len ? *p : வெற்று)
	    : buf->b_next)
#இல்லையெனில்
    க்கு (buf = firstbuf; buf != வெற்று && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	எனில் ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !bufIsChanged(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == வெற்று || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == வெற்று || buf->b_nwindows != 0))
#ifdef FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!job_running || (job_running && job_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!job_running || (job_running && !job_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (job_running || buf->b_term == வெற்று))
#endif
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    தொடர்;
	name = buf_spname(buf);
	எனில் (name != வெற்று)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	இல்லையெனில்
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	எனில் (message_filtered(NameBuff))
	    தொடர்;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	எனில் (job_running)
	{
	    எனில் (job_none_open)
		ro_char = '?';
	    இல்லையெனில்
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	இல்லையெனில் எனில் (buf->b_term != வெற்று)
	    ro_char = 'F';
	இல்லையெனில்
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	செய்தி_putchar('\n');
	len = (முழு)vim_snprintf_safelen((எழுத்து *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == வெற்று ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		changed_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	செய்
	    IObuff[len++] = ' ';
	போது (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	எனில் (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (size_t)(IOSIZE - len), buf->b_last_used);
	இல்லையெனில்
#endif
	    vim_snprintf((எழுத்து *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (long)buflist_findlnum(buf));
	செய்தி_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    எனில் (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    முழு
buflist_name_nr(
    முழு		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    எனில் (buf == வெற்று || buf->b_fname == வெற்று)
	திரும்பு FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    திரும்பு OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    முழு
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    முழு		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = வெற்று;
#ifdef UNIX
    stat_T	st;
#endif

    எனில் (ffname == வெற்று || *ffname == NUL)
    {
	// Removing the name.
	எனில் (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	இல்லையெனில்
	    buf->b_sfname = வெற்று;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    இல்லையெனில்
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	எனில் (ffname == வெற்று)		    // out of memory
	    திரும்பு FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	எனில் (mch_stat((எழுத்து *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	எனில் (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#இல்லையெனில்
	    obuf = buflist_findname(ffname);
#endif
	எனில் (obuf != வெற்று && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    முழு		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		எனில் (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    எனில் (obuf->b_ml.ml_mfp != வெற்று || in_use)
	    {
		எனில் (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		திரும்பு FAIL;
	    }
	    // delete from the list
	    close_buffer(வெற்று, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	எனில் (ffname == வெற்று || sfname == வெற்று)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    திரும்பு FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	எனில் (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    எனில் (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    இல்லையெனில்
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    திரும்பு OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    வெறுமை
buf_set_name(முழு fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    எனில் (buf == வெற்று)
	திரும்பு;

    எனில் (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = வெற்று;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    வெறுமை
buf_name_changed(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    எனில் (buf->b_ml.ml_mfp != வெற்று)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    எனில் (buf->b_term != வெற்று)
	term_clear_status_text(buf->b_term);
#endif

    எனில் (curwin->w_buffer == buf)
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
    எனில் (buf != வெற்று && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    திரும்பு buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    முழு		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    எனில் (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	எனில் (errmsg)
	    emsg(_(e_no_alternate_file));
	திரும்பு வெற்று;
    }
    திரும்பு fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    முழு
buflist_add(char_u *fname, முழு flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, வெற்று, (linenr_T)0, flags);
    எனில் (buf != வெற்று)
	திரும்பு buf->b_fnum;
    திரும்பு 0;
}

#எனில் defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    வெறுமை
buflist_slash_adjust(வெறுமை)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	எனில் (bp->b_ffname != வெற்று)
	    slash_adjust(bp->b_ffname);
	எனில் (bp->b_sfname != வெற்று)
	    slash_adjust(bp->b_sfname);
    }
}
#endif

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    வெறுமை
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.col, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    முழு
otherfile(char_u *ffname)
{
    திரும்பு otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , வெற்று
#endif
	    );
}

    நிலையான முழு
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    எனில் (ffname == வெற்று || *ffname == NUL || buf->b_ffname == வெற்று)
	திரும்பு TRUE;
    எனில் (fnamecmp(ffname, buf->b_ffname) == 0)
	திரும்பு FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	எனில் (stp == வெற்று)
	{
	    எனில் (!buf->b_dev_valid || mch_stat((எழுத்து *)ffname, &st) < 0)
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
	எனில் (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    எனில் (buf_same_ino(buf, stp))
		திரும்பு FALSE;
	}
    }
#endif
    திரும்பு TRUE;
}

#எனில் defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    வெறுமை
buf_setino(buf_T *buf)
{
    stat_T	st;

    எனில் (buf->b_fname != வெற்று && mch_stat((எழுத்து *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    இல்லையெனில்
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    நிலையான முழு
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    திரும்பு (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    வெறுமை
fileinfo(
    முழு fullname,	    // when non-zero print full path
    முழு shorthelp,
    முழு	dont_truncate)
{
    char_u	*name;
    எழுத்து	*buffer;
    size_t	bufferlen = 0;

    buffer = alloc(IOSIZE);
    எனில் (buffer == வெற்று)
	திரும்பு;

    எனில் (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    எனில் (name != வெற்று)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    இல்லையெனில்
    {
	எனில் (!fullname && curbuf->b_fname != வெற்று)
	    name = curbuf->b_fname;
	இல்லையெனில்
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : வெற்று, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (முழு)bufferlen, TRUE);
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

    எனில் (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_வரிகள்_செய்தி));
    இல்லையெனில் எனில் (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_வரி_மொத்தம்),
	    (long)curbuf->b_ml.ml_வரி_மொத்தம்,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_வரி_மொத்தம்));
    இல்லையெனில்
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (long)curwin->w_cursor.lnum,
	    (long)curbuf->b_ml.ml_வரி_மொத்தம்,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_வரி_மொத்தம்));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (முழு)curwin->w_cursor.col + 1, (முழு)curwin->w_virtcol + 1);
    }

    (வெறுமை)append_arg_எண்(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    எனில் (dont_truncate)
    {
	முழு	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	செய்தி_start();
	n = செய்தி_scroll;
	செய்தி_scroll = TRUE;
	செய்தி(buffer);
	செய்தி_scroll = n;
    }
    இல்லையெனில்
    {
	எழுத்து	*p = செய்தி_trunc_attr(buffer, FALSE, 0);
	எனில் (restart_edit != 0 || (செய்தி_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_செய்தி((char_u *)p, 0);
    }

    vim_free(buffer);
}

    முழு
col_print(
    char_u  *buf,
    size_t  buflen,
    முழு	    col,
    முழு	    vcol)
{
    எனில் (col == vcol)
	திரும்பு (முழு)vim_snprintf_safelen((எழுத்து *)buf, buflen, "%d", col);

    திரும்பு (முழு)vim_snprintf_safelen((எழுத்து *)buf, buflen, "%d-%d", col, vcol);
}

நிலையான char_u *lasttitle = வெற்று;
நிலையான char_u *lasticon = வெற்று;

/*
 * Put the file name in the title bar and icon of the window.
 */
    வெறுமை
maketitle(வெறுமை)
{
    char_u	*title_str = வெற்று;
    char_u	*icon_str = வெற்று;
    முழு		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    எனில் (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	திரும்பு;
    }

    need_maketitle = FALSE;
    எனில் (!p_title && !p_icon && lasttitle == வெற்று && lasticon == வெற்று)
	திரும்பு;  // nothing to do

    எனில் (p_title)
    {
	முழு maxlen = 0;

	எனில் (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    எனில் (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	எனில் (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    எனில் (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, அளவு(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, வெற்று, வெற்று, வெற்று);
	    இல்லையெனில்
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	இல்லையெனில்
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (அளவு(buf) - 100)
#define SPACE_FOR_DIR   (அளவு(buf) - 20)
#define SPACE_FOR_ARGNR (அளவு(buf) - 10)  // at least room for " - VIM"

	    // file name
	    எனில் (curbuf->b_fname == வெற்று)
		buflen = vim_snprintf_safelen((எழுத்து *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    இல்லையெனில் எனில் (curbuf->b_term != வெற்று)
		buflen = vim_snprintf_safelen((எழுத்து *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    இல்லையெனில்
	    {
		buflen = vim_snprintf_safelen((எழுத்து *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != வெற்று)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    எனில் (curbuf->b_term == வெற்று)
#endif
	    {
		தேர்வு (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    நிலை 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (எழுத்து *)buf + buflen,
			    அளவு(buf) - buflen, " +");
			நிறுத்து;
		    நிலை 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (எழுத்து *)buf + buflen,
			    அளவு(buf) - buflen, " =");
			நிறுத்து;
		    நிலை 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (எழுத்து *)buf + buflen,
			    அளவு(buf) - buflen, " =+");
			நிறுத்து;
		    நிலை 4:
		    நிலை 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (எழுத்து *)buf + buflen,
			    அளவு(buf) - buflen, " -");
			நிறுத்து;
		    நிலை 5:
		    நிலை 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (எழுத்து *)buf + buflen,
			    அளவு(buf) - buflen, " -+");
			நிறுத்து;
		    இயல்பு:
			நிறுத்து;
		}
	    }

	    // path (surrounded by '()')
	    எனில் (curbuf->b_fname != வெற்று
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == வெற்று
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((எழுத்து *)buf + buflen,
		    அளவு(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (முழு)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		எனில் (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		எனில் (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((எழுத்து *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		இல்லையெனில்
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    எனில் (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((எழுத்து *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != வெற்று)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    இல்லையெனில்
			buflen += vim_snprintf_safelen((எழுத்து *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_எண்(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((எழுத்து *)buf + buflen,
		அளவு(buf) - buflen, " - %s",
#எனில் defined(FEAT_CLIENTSERVER)
		(serverName != வெற்று)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    எனில் (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		எனில் (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, அளவு(buf));
	    }
	}
    }
    mustset = மதிப்பு_changed(title_str, &lasttitle);

    எனில் (p_icon)
    {
	icon_str = buf;
	எனில் (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    எனில் (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, அளவு(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, வெற்று, வெற்று,
				 வெற்று);
	    இல்லையெனில்
#endif
		icon_str = p_iconstring;
	}
	இல்லையெனில்
	{
	    char_u  *name;
	    முழு	    namelen;

	    name = buf_spname(curbuf);
	    எனில் (name == வெற்று)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (முழு)STRLEN(name);
	    எனில் (namelen > 100)
	    {
		namelen -= 100;
		எனில் (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, அளவு(buf));
	}
    }

    mustset |= மதிப்பு_changed(icon_str, &lasticon);

    எனில் (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    நிலையான முழு
மதிப்பு_changed(char_u *str, char_u **last)
{
    எனில் ((str == வெற்று) != (*last == வெற்று)
	    || (str != வெற்று && *last != வெற்று && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	எனில் (str == வெற்று)
	{
	    *last = வெற்று;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	இல்லையெனில்
	{
	    *last = vim_strsave(str);
	    திரும்பு TRUE;
	}
    }
    திரும்பு FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    வெறுமை
resettitle(வெறுமை)
{
    mch_settitle(lasttitle, lasticon);
}

#எனில் defined(EXITFREE)
    வெறுமை
free_titles(வெறுமை)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#எனில் defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
வகைவரை கட்டமைப்பு
{
    char_u	*stl_start;
    முழு		stl_minwid;
    முழு		stl_maxwid;
    எண்ணிக்கை {
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

நிலையான size_t		stl_பொருட்கள்_len = 20; // Initial value, grows as needed.
நிலையான stl_item_T      *stl_பொருட்கள் = வெற்று;
நிலையான முழு	       *stl_groupitem = வெற்று;
நிலையான stl_hlrec_T     *stl_hltab = வெற்று;
நிலையான stl_hlrec_T     *stl_tabtab = வெற்று;
நிலையான stl_clickrec_T  *stl_clicktab = வெற்று;
நிலையான முழு		*stl_separator_locations = வெற்று;

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
    முழு
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    முழு		opt_scope,	// scope for "opt_name"
    முழு		fillchar,
    முழு		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    திரும்பு build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    வெற்று, வெற்று);
}

    முழு
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    முழு		opt_scope,	// scope for "opt_name"
    முழு		fillchar,
    முழு		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    முழு		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    திரும்பு build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    வெற்று, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    முழு
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    முழு		opt_scope,	// scope for "opt_name"
    முழு		fillchar,
    முழு		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    முழு		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    திரும்பு build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    வெற்று, carry_hl);
}
# endif

    முழு
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    முழு		opt_scope)	// scope for "opt_name"
{
    முழு rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (வெறுமை)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, அளவு(buf), &fmt,
	    opt_name, opt_scope, 0, 0, வெற்று, வெற்று, வெற்று, &rendered_height,
	    வெற்று);
    --emsg_off;
    திரும்பு rendered_height;
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
நிலையான முழு
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    முழு		opt_scope,	// scope for "opt_name"
    முழு		fillchar,
    முழு		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    முழு		*rendered_height,   // return: stl rendered height (can be NULL)
    முழு		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    முழு		byteval;
# ifdef FEAT_EVAL
    முழு		use_sandbox;
    முழு		save_VIsual_active;
# endif
    முழு		empty_வரி;
    long	l;
    long	n;
    முழு		prevchar_isflag;
    முழு		prevchar_isitem;
    முழு		itemisflag;
    முழு		fillable;
    char_u	*str;
    long	இலக்கம்;
    முழு		width;
    முழு		itemcnt;
    முழு		curitem;
    முழு		groupdepth;
# ifdef FEAT_EVAL
    முழு		evaldepth;
# endif
    முழு		minwid;
    முழு		maxwid;
    முழு		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    முழு		save_redraw_not_allowed = redraw_not_allowed;
    முழு		save_சாவி_Typed = சாவி_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    முழு		did_emsg_before = did_emsg;
    முழு		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    எனில் (updating_screen)
	redraw_not_allowed = TRUE;

    எனில் (stl_பொருட்கள் == வெற்று)
    {
	stl_பொருட்கள் = ALLOC_MULT(stl_item_T, stl_பொருட்கள்_len);
	stl_groupitem = ALLOC_MULT(முழு, stl_பொருட்கள்_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_பொருட்கள்_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_பொருட்கள்_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_பொருட்கள்_len + 1);

	stl_separator_locations = ALLOC_MULT(முழு, stl_பொருட்கள்_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    எனில் (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_எண்;
	tv.vval.v_எண் = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	எனில் (usefmt == வெற்று)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    எனில் (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    எனில் (lnum > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
    {
	lnum = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_வரி = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    எனில் (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    இல்லையெனில்
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    எனில் (carry_hl != வெற்று && *carry_hl != 0)
    {
	stl_பொருட்கள்[curitem].stl_type = Highlight;
	stl_பொருட்கள்[curitem].stl_start = p;
	stl_பொருட்கள்[curitem].stl_minwid = *carry_hl;
	stl_பொருட்கள்[curitem].stl_clickfunc = வெற்று;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    க்கு (s = usefmt; *s != NUL; )
    {
	எனில் (curitem == (முழு)stl_பொருட்கள்_len)
	{
	    size_t	new_len = stl_பொருட்கள்_len * 3 / 2;

	    stl_item_T *new_பொருட்கள் =
			  vim_realloc(stl_பொருட்கள், அளவு(stl_item_T) * new_len);
	    எனில் (new_பொருட்கள் == வெற்று)
		நிறுத்து;
	    stl_பொருட்கள் = new_பொருட்கள்;

	    முழு *new_groupitem =
			     vim_realloc(stl_groupitem, அளவு(முழு) * new_len);
	    எனில் (new_groupitem == வெற்று)
		நிறுத்து;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  அளவு(stl_hlrec_T) * (new_len + 1));
	    எனில் (new_hlrec == வெற்று)
		நிறுத்து;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  அளவு(stl_hlrec_T) * (new_len + 1));
	    எனில் (new_hlrec == வெற்று)
		நிறுத்து;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      அளவு(stl_clickrec_T) * (new_len + 1));
	    எனில் (new_clickrec == வெற்று)
		நிறுத்து;
	    stl_clicktab = new_clickrec;

	    முழு *new_separator_locs = vim_realloc(stl_separator_locations,
					    அளவு(முழு) * new_len);
	    எனில் (new_separator_locs == வெற்று)
		நிறுத்து;
	    stl_separator_locations = new_separator_locs;

	    stl_பொருட்கள்_len = new_len;
	}

	stl_பொருட்கள்[curitem].stl_clickfunc = வெற்று;

	எனில் (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	போது (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    எனில் (*s == '\n' || *s == '\r')
	    {
		எனில் (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    செல் find_linebreak;
		}
		இல்லையெனில் எனில் (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	எனில் (*s == NUL || p + 1 >= out + outlen)
	    நிறுத்து;

	/*
	 * Handle one '%' item.
	 */
	s++;
	எனில் (*s == NUL)  // ignore trailing %
	    நிறுத்து;

	எனில் (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    எனில் (s[1] == ']')
	    {
		stl_பொருட்கள்[curitem].stl_type = ClickFunc;
		stl_பொருட்கள்[curitem].stl_start = p;
		stl_பொருட்கள்[curitem].stl_minwid = 0;
		stl_பொருட்கள்[curitem].stl_clickfunc = வெற்று;
		s += 2;
		curitem++;
		தொடர்;
	    }
	    // %[FuncName] - start click region
	    எனில் (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		எனில் (rb != வெற்று)
		{
		    stl_பொருட்கள்[curitem].stl_type = ClickFunc;
		    stl_பொருட்கள்[curitem].stl_start = p;
		    stl_பொருட்கள்[curitem].stl_minwid = 0;
		    stl_பொருட்கள்[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    தொடர்;
		}
	    }
	}
	எனில் (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    எனில் (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		நிறுத்து;
	    }
	    இல்லையெனில் எனில் (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	எனில் (*s == '%')
	{
	    எனில் (p + 1 >= out + outlen)
		நிறுத்து;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    தொடர்;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	எனில் (*s == STL_SEPARATE)
	{
	    s++;
	    எனில் (groupdepth > 0)
		தொடர்;
	    stl_பொருட்கள்[curitem].stl_type = Separate;
	    stl_பொருட்கள்[curitem++].stl_start = p;
	    தொடர்;
	}
	எனில் (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_பொருட்கள்[curitem].stl_type = Trunc;
	    stl_பொருட்கள்[curitem++].stl_start = p;
	    தொடர்;
	}
	எனில் (*s == ')')
	{
	    char_u  *t;

	    s++;
	    எனில் (groupdepth < 1)
		தொடர்;
	    groupdepth--;

	    t = stl_பொருட்கள்[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    எனில் (curitem > stl_groupitem[groupdepth] + 1
		    && stl_பொருட்கள்[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		முழு group_start_userhl = 0;
		முழு group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		க்கு (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    எனில் (stl_பொருட்கள்[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_பொருட்கள்[n].stl_minwid;
			நிறுத்து;
		    }
		}
		க்கு (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    எனில் (stl_பொருட்கள்[n].stl_type == Normal)
			நிறுத்து;
		    எனில் (stl_பொருட்கள்[n].stl_type == Highlight)
			group_end_userhl = stl_பொருட்கள்[n].stl_minwid;
		}
		எனில் (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    க்கு (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			எனில் (stl_பொருட்கள்[n].stl_type == Highlight)
			    stl_பொருட்கள்[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			எனில் (stl_பொருட்கள்[n].stl_type == TabPage)
			    stl_பொருட்கள்[n].stl_start = p;
		    }
		}
	    }
	    எனில் (l > stl_பொருட்கள்[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		எனில் (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    போது (l >= stl_பொருட்கள்[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		இல்லையெனில்
		    n = (long)(p - t) - stl_பொருட்கள்[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		முழு minwid_fixed = MIN(stl_பொருட்கள்[stl_groupitem[groupdepth]].stl_minwid,
				       stl_பொருட்கள்[stl_groupitem[groupdepth]].stl_maxwid);
		போது (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		க்கு (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_பொருட்கள்[l].stl_start -= n - 1;
		    எனில் (stl_பொருட்கள்[l].stl_start < t)
			stl_பொருட்கள்[l].stl_start = t;
		}
	    }
	    இல்லையெனில் எனில் (abs(stl_பொருட்கள்[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_பொருட்கள்[stl_groupitem[groupdepth]].stl_minwid;
		முழு fillchar_len = MB_CHAR2LEN(fillchar);
		எனில் (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    போது (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		இல்லையெனில்
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    எனில் (p + l >= out + outlen)
		    {
			n = (long)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    க்கு ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    க்கு (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_பொருட்கள்[n].stl_start += l;
		}
	    }
	    தொடர்;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	எனில் (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	எனில் (*s == '-')
	{
	    s++;
	    l = -1;
	}
	எனில் (VIM_ISDIGIT(*s))
	{
	    minwid = (முழு)getdigits(&s);
	    எனில் (minwid < 0)	// overflow
		minwid = 0;
	}
	எனில் (*s == STL_USER_HL)
	{
	    stl_பொருட்கள்[curitem].stl_type = Highlight;
	    stl_பொருட்கள்[curitem].stl_start = p;
	    stl_பொருட்கள்[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    தொடர்;
	}
	எனில் (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    எனில் (*s == STL_TABCLOSENR)
	    {
		எனில் (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    க்கு (n = curitem - 1; n >= 0; --n)
			எனில் (stl_பொருட்கள்[n].stl_type == TabPage
					       && stl_பொருட்கள்[n].stl_minwid >= 0)
			{
			    minwid = stl_பொருட்கள்[n].stl_minwid;
			    நிறுத்து;
			}
		}
		இல்லையெனில்
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_பொருட்கள்[curitem].stl_type = TabPage;
	    stl_பொருட்கள்[curitem].stl_start = p;
	    stl_பொருட்கள்[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    தொடர்;
	}
	எனில் (*s == '.')
	{
	    s++;
	    எனில் (VIM_ISDIGIT(*s))
	    {
		maxwid = (முழு)getdigits(&s);
		எனில் (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	முழு raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	எனில் (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_பொருட்கள்[curitem].stl_type = Group;
	    stl_பொருட்கள்[curitem].stl_start = p;
	    stl_பொருட்கள்[curitem].stl_minwid = minwid;
	    stl_பொருட்கள்[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    தொடர்;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	எனில் (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    தொடர்;
	}
# endif
	எனில் (vim_strchr(STL_ALL, *s) == வெற்று)
	{
	    எனில் (*s == NUL)  // can happen with "%0"
		நிறுத்து;
	    s++;
	    தொடர்;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	இலக்கம் = -1;
	str = வெற்று;
	தேர்வு (opt)
	{
	நிலை STL_FILEPATH:
	நிலை STL_FULLPATH:
	நிலை STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    எனில் (name != வெற்று)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    இல்லையெனில்
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    எனில் (opt != STL_FILENAME)
		str = NameBuff;
	    இல்லையெனில்
		str = gettail(NameBuff);
	    நிறுத்து;
	}

	நிலை STL_VIM_கோவை: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    முழு	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    எனில் (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    போது ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    எனில் (*s != '}')	// missing '}' or out of space
		நிறுத்து;
	    s++;
	    எனில் (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    இல்லையெனில்
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((எழுத்து *)buf_tmp, அளவு(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((எழுத்து *)buf_tmp, அளவு(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    எனில் (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    எனில் (!zeropad && str != வெற்று && *str != NUL)
	    {
		எனில் (*skipdigits(str) == NUL)
		{
		    இலக்கம் = atoi((எழுத்து *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    எனில் (reevaluate && str != வெற்று && *str != NUL
		    && strchr((மாறிலி எழுத்து *)str, '%') != வெற்று
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((மாறிலி எழுத்து *)str);
		size_t fmt_length = strlen((மாறிலி எழுத்து *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * அளவு(char_u));

		எனில் (new_fmt != வெற்று)
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
		    new_fmt_p = வெற்று;

		    எனில் (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    தொடர்;
		}
	    }
# endif
	    நிறுத்து;
	}
	நிலை STL_வரி:
	    இலக்கம் = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    நிறுத்து;

	நிலை STL_NUMLINES:
	    இலக்கம் = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
	    நிறுத்து;

	நிலை STL_COLUMN:
	    இலக்கம் = (State & MODE_செருகு) == 0 && empty_வரி
					       ? 0 : (முழு)wp->w_cursor.col + 1;
	    நிறுத்து;

	நிலை STL_VIRTCOL:
	நிலை STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    எனில் (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_செருகு) == 0
			       && empty_வரி ? 0 : (முழு)wp->w_cursor.col + 1)))
		நிறுத்து;
	    இலக்கம் = (long)virtcol;
	    நிறுத்து;
	}

	நிலை STL_PERCENTAGE:
	    இலக்கம் = calc_percentage((long)wp->w_cursor.lnum, (long)wp->w_buffer->b_ml.ml_வரி_மொத்தம்);
	    நிறுத்து;

	நிலை STL_ALTPERCENT:
	    str = buf_tmp;
	    (வெறுமை)get_rel_pos(wp, str, TMPLEN);
	    நிறுத்து;

	நிலை STL_SHOWCMD:
	    எனில் (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    நிறுத்து;

	நிலை STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    எனில் (append_arg_எண்(wp, buf_tmp, அளவு(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    நிறுத்து;

	நிலை STL_KEYMAP:
	    fillable = FALSE;
	    எனில் (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    நிறுத்து;
	நிலை STL_PAGENUM:
# எனில் defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    இலக்கம் = printer_page_இலக்கம்;
# இல்லையெனில்
	    இலக்கம் = 0;
# endif
	    நிறுத்து;

	நிலை STL_BUFNO:
	    இலக்கம் = wp->w_buffer->b_fnum;
	    நிறுத்து;

	நிலை STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	நிலை STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_வரி_or_offset(wp->w_buffer, wp->w_cursor.lnum, வெற்று);
	    இலக்கம் = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_செருகு) == 0 && empty_வரி
				? 0 : (முழு)wp->w_cursor.col);
# endif
	    நிறுத்து;

	நிலை STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	நிலை STL_BYTEVAL:
	    இலக்கம் = byteval;
	    எனில் (இலக்கம் == NL)
		இலக்கம் = 0;
	    இல்லையெனில் எனில் (இலக்கம் == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		இலக்கம் = NL;
	    நிறுத்து;

	நிலை STL_ROFLAG:
	நிலை STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    எனில் (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    நிறுத்து;

	நிலை STL_HELPFLAG:
	நிலை STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    எனில் (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    நிறுத்து;

	நிலை STL_FILETYPE:
	    எனில் (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((எழுத்து *)buf_tmp, அளவு(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    நிறுத்து;

	நிலை STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    எனில் (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((எழுத்து *)buf_tmp, அளவு(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		க்கு (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    நிறுத்து;

# எனில் defined(FEAT_QUICKFIX)
	நிலை STL_PREVIEWFLAG:
	நிலை STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    எனில் (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    நிறுத்து;

	நிலை STL_QUICKFIX:
	    எனில் (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(செய்தி_loclist)
			    : _(செய்தி_qflist));
	    நிறுத்து;
# endif

	நிலை STL_MODIFIED:
	நிலை STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    தேர்வு ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		நிலை 2: str = (char_u *)"[+]"; நிறுத்து;
		நிலை 3: str = (char_u *)",+"; நிறுத்து;
		நிலை 4: str = (char_u *)"[-]"; நிறுத்து;
		நிலை 5: str = (char_u *)",-"; நிறுத்து;
		நிலை 6: str = (char_u *)"[+-]"; நிறுத்து;
		நிலை 7: str = (char_u *)",+-"; நிறுத்து;
	    }
	    நிறுத்து;

	நிலை STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		போது (*s != '#' && *s != NUL)
		    ++s;
		எனில் (*s == '#')
		{
		    stl_பொருட்கள்[curitem].stl_type = Highlight;
		    stl_பொருட்கள்[curitem].stl_start = p;
		    stl_பொருட்கள்[curitem].stl_minwid = -syn_namen2id(t, (முழு)(s - t));
		    curitem++;
		}
		எனில் (*s != NUL)
		    ++s;
		தொடர்;
	    }

	நிலை STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    எனில் (*s == ']')
	    {
		stl_பொருட்கள்[curitem].stl_type = ClickFunc;
		stl_பொருட்கள்[curitem].stl_start = p;
		stl_பொருட்கள்[curitem].stl_minwid = 0;
		stl_பொருட்கள்[curitem].stl_clickfunc = வெற்று;
		s++;
		curitem++;
		தொடர்;
	    }
	    // %N[FuncName] with minwid
	    எனில் (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		எனில் (rb != வெற்று)
		{
		    stl_பொருட்கள்[curitem].stl_type = ClickFunc;
		    stl_பொருட்கள்[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_பொருட்கள்[curitem].stl_minwid = raw_minwid;
		    stl_பொருட்கள்[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    தொடர்;
		}
	    }
	    தொடர்;

	நிலை STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    தொடர்;
	}

	stl_பொருட்கள்[curitem].stl_start = p;
	stl_பொருட்கள்[curitem].stl_type = Normal;
	எனில் (str != வெற்று && *str)
	{
	    char_u  *t = str;

	    எனில் (itemisflag)
	    {
		எனில் ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    எனில் (l > 0)
		prevchar_isitem = TRUE;
	    எனில் (l > maxwid)
	    {
		போது (l >= maxwid)
		    எனில் (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    இல்லையெனில்
			l -= byte2cells(*t++);
		எனில் (p + 1 >= out + outlen)
		    நிறுத்து;
		*p++ = '<';
	    }
	    எனில் (minwid > 0)
	    {
		க்கு (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    எனில் (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    இல்லையெனில்
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    இல்லையெனில்
		minwid *= -1;
	    க்கு (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		எனில் (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		இல்லையெனில்
		    *p++ = *t;
	    }
	    க்கு (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	இல்லையெனில் எனில் (இலக்கம் >= 0)
	{
	    முழு	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    எனில் (p + 20 >= out + outlen)
		நிறுத்து;		// not sufficient space
	    prevchar_isitem = TRUE;
	    எனில் (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    எனில் (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    க்கு (n = இலக்கம், l = 1; n >= nbase; n /= nbase)
		l++;
	    எனில் (opt == STL_VIRTCOL_ALT)
		l++;

	    எனில் (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		போது (l-- > maxwid)
		    இலக்கம் /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((எழுத்து *)p, outlen - (p - out),
		    (எழுத்து *)nstr, 0, இலக்கம், n);
	    }
	    இல்லையெனில்
		p += vim_snprintf_safelen((எழுத்து *)p, outlen - (p - out),
		    (எழுத்து *)nstr, minwid, இலக்கம்);
	}
	இல்லையெனில்
	    stl_பொருட்கள்[curitem].stl_type = Empty;

	எனில் (இலக்கம் >= 0 || (!itemisflag && str != வெற்று && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	எனில் (opt == STL_VIM_கோவை)
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
    எனில் (carry_hl != வெற்று)
    {
	முழு last_hl = 0;
	க்கு (l = 0; l < itemcnt; l++)
	    எனில் (stl_பொருட்கள்[l].stl_type == Highlight)
		last_hl = stl_பொருட்கள்[l].stl_minwid;
	*carry_hl = last_hl;
    }

    எனில் (mode == STL_MODE_MULTI
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
	எனில் (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = strlen((எழுத்து *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    இல்லையெனில்
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	எனில் (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    எனில் (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	க்கு (l = 0; l < itemcnt; l++)
	    எனில் (stl_பொருட்கள்[l].stl_type == ClickFunc)
		vim_free(stl_பொருட்கள்[l].stl_clickfunc);
	எனில் (rendered_height != வெற்று)
	    *rendered_height = rheight;
	திரும்பு 0;
    }

    width = vim_strsize(out);
    எனில் (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	எனில் (itemcnt == 0)
	    s = out;
	இல்லையெனில்
	{
	    க்கு ( ; l < itemcnt; l++)
		எனில் (stl_பொருட்கள்[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_பொருட்கள்[l].stl_start;
		    நிறுத்து;
		}
	    எனில் (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_பொருட்கள்[0].stl_start;
		l = 0;
	    }
	}

	எனில் (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    எனில் (has_mbyte)
	    {
		s = out;
		width = 0;
		க்கு (;;)
		{
		    width += ptr2cells(s);
		    எனில் (width >= maxwidth)
			நிறுத்து;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		போது (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    இல்லையெனில்
		s = out + maxwidth - 1;
	    க்கு (l = 0; l < itemcnt; l++)
		எனில் (stl_பொருட்கள்[l].stl_start > s)
		    நிறுத்து;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	இல்லையெனில்
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    போது (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(end - p) + 1);	// +1 for NUL
	    end -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    க்கு (; l < itemcnt; l++)
	    {
		எனில் (stl_பொருட்கள்[l].stl_start - n >= s)
		    stl_பொருட்கள்[l].stl_start -= n;
		இல்லையெனில்
		    stl_பொருட்கள்[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    போது (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    இல்லையெனில் எனில் (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	முழு இலக்கம்_separators = 0;

	க்கு (l = 0; l < itemcnt; l++)
	{
	    எனில் (stl_பொருட்கள்[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[இலக்கம்_separators] = l;
		இலக்கம்_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	எனில் (இலக்கம்_separators)
	{
	    முழு standard_spaces;
	    முழு final_spaces;

	    standard_spaces = (maxwidth - width) / இலக்கம்_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (இலக்கம்_separators - 1);
	    க்கு (l = 0; l < இலக்கம்_separators; l++)
	    {
		முழு dislocation = (l == (இலக்கம்_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_பொருட்கள்[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		க்கு (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		க்கு (முழு i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_பொருட்கள்[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    எனில் (hltab != வெற்று)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	க்கு (l = 0; l < itemcnt; l++)
	{
	    எனில் (stl_பொருட்கள்[l].stl_type == Highlight)
	    {
		sp->start = stl_பொருட்கள்[l].stl_start;
		sp->userhl = stl_பொருட்கள்[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = வெற்று;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    எனில் (tabtab != வெற்று)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	க்கு (l = 0; l < itemcnt; l++)
	{
	    எனில் (stl_பொருட்கள்[l].stl_type == TabPage)
	    {
		sp->start = stl_பொருட்கள்[l].stl_start;
		sp->userhl = stl_பொருட்கள்[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = வெற்று;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    எனில் (clicktab != வெற்று)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	க்கு (l = 0; l < itemcnt; l++)
	{
	    எனில் (stl_பொருட்கள்[l].stl_type == ClickFunc)
	    {
		cp->start = stl_பொருட்கள்[l].stl_start;
		cp->funcname = stl_பொருட்கள்[l].stl_clickfunc;
		cp->minwid = stl_பொருட்கள்[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = வெற்று;
	cp->funcname = வெற்று;
    }
    இல்லையெனில்
    {
	// Free click function names when caller doesn't need them.
	க்கு (l = 0; l < itemcnt; l++)
	    எனில் (stl_பொருட்கள்[l].stl_type == ClickFunc)
		vim_free(stl_பொருட்கள்[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    சாவி_Typed = save_சாவி_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    எனில் (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    திரும்பு width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    முழு
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    முழு		buflen)
{
    long	above; // number of lines above window
    long	below; // number of lines below window

    எனில் (buflen < 3) // need at least 3 chars for writing
	திரும்பு 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    எனில் (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_வரி_மொத்தம் - wp->w_botline + 1;
    எனில் (below <= 0)
	திரும்பு (முழு)vim_snprintf_safelen((எழுத்து *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    எனில் (above <= 0)
	திரும்பு (முழு)vim_snprintf_safelen((எழுத்து *)buf, buflen,
	    "%s", _("Top"));

    முழு perc = calc_percentage(above, above + below);
    எழுத்து tmp[8];
    // localized percentage value
    vim_snprintf(tmp, அளவு(tmp), _("%d%%"), perc);
    திரும்பு (முழு)vim_snprintf_safelen((எழுத்து *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    நிலையான முழு
append_arg_எண்(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    முழு		add_file)	// Add "file" before the arg number
{
    எனில் (ARGCOUNT <= 1)		// nothing to do
	திரும்பு 0;

    எழுத்து *செய்தி;
    தேர்வு ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	நிலை 0: செய்தி = _(" (%d of %d)"); நிறுத்து;
	நிலை 1: செய்தி = _(" ((%d) of %d)"); நிறுத்து;
	நிலை 2: செய்தி = _(" (file %d of %d)"); நிறுத்து;
	நிலை 3: செய்தி = _(" (file (%d) of %d)"); நிறுத்து;
    }

    திரும்பு (முழு)vim_snprintf_safelen((எழுத்து *)buf, buflen, செய்தி,
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
    திரும்பு FullName_save(fname, TRUE);
#இல்லையெனில்
    எனில் (!vim_isAbsName(fname)
	    || strstr((எழுத்து *)fname, "..") != வெற்று
	    || strstr((எழுத்து *)fname, "//") != வெற்று
# ifdef BACKSLASH_IN_FILENAME
	    || strstr((எழுத்து *)fname, "\\\\") != வெற்று
# endif
# எனில் defined(MSWIN)
	    || vim_strchr(fname, '~') != வெற்று
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	திரும்பு FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    எனில் (fname != வெற்று)
	fname_case(fname, 0);	// set correct case for file name
# endif

    திரும்பு fname;
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
    வெறுமை
fname_expand(
    buf_T	*buf UNUSED,
    char_u	**ffname,
    char_u	**sfname)
{
    எனில் (*ffname == வெற்று)	    // no file name given, nothing to do
	திரும்பு;
    எனில் (*sfname == வெற்று)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    எனில் (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	எனில் (rfname != வெற்று)
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
    வெறுமை
ex_buffer_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    முழு		split_ret = OK;
    முழு		p_ea_save;
    முழு		open_wins = 0;
    முழு		r;
    முழு		மொத்தம்;		// Maximum number of windows to open.
    முழு		all;		// When TRUE also load inactive buffers.
    முழு		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    எனில் (eap->addr_மொத்தம் == 0)	// make as many windows as possible
	மொத்தம் = 9999;
    இல்லையெனில்
	மொத்தம் = eap->line2;	// make as many windows as specified
    எனில் (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    இல்லையெனில்
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
    எனில் (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    க்கு (;;)
    {
	tpnext = curtab->tp_next;
	க்கு (wp = firstwin; wp != வெற்று; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    எனில் ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		எனில் (win_close(wp, FALSE) == FAIL)
		    நிறுத்து;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    இல்லையெனில்
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	எனில் (had_tab == 0 || tpnext == வெற்று)
	    நிறுத்து;
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
    க்கு (buf = firstbuf; buf != வெற்று && open_wins < மொத்தம்; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	எனில் ((!all && buf->b_ml.ml_mfp == வெற்று) || !buf->b_p_bl)
	    தொடர்;

	எனில் (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    எனில் (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    இல்லையெனில்
		wp = வெற்று;
	}
	இல்லையெனில்
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		எனில் (wp->w_buffer == buf)
		    நிறுத்து;
	    // If the buffer already has a window, move it
	    எனில் (wp != வெற்று)
		win_move_after(wp, curwin);
	}

	எனில் (wp == வெற்று && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    எனில் (split_ret == FAIL)
		தொடர்;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    எனில் (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		நிறுத்து;
	    }
	    எனில் (swap_exists_action == SEA_QUIT)
	    {
#எனில் defined(FEAT_EVAL)
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

#எனில் defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    இல்லையெனில்
		handle_swap_exists(வெற்று);
	}

	ui_breakcheck();
	எனில் (got_int)
	{
	    (வெறுமை)vgetc();	// only break the file loading, not the rest
	    நிறுத்து;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	எனில் (aborting())
	    நிறுத்து;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	எனில் (had_tab > 0 && tabpage_index(வெற்று) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    க்கு (wp = lastwin; open_wins > மொத்தம்; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	எனில் (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	இல்லையெனில் எனில் (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	இல்லையெனில்
	{
	    wp = wp->w_prev;
	    எனில் (wp == வெற்று)
		நிறுத்து;
	}
    }
}


நிலையான முழு  chk_modeline(linenr_T, முழு);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    வெறுமை
do_modelines(முழு flags)
{
    linenr_T	lnum;
    முழு		nmlines;
    நிலையான முழு	entered = 0;

    எனில் (!curbuf->b_p_ml || (nmlines = (முழு)p_mls) == 0)
	திரும்பு;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    எனில் (entered)
	திரும்பு;

    ++entered;
    க்கு (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_வரி_மொத்தம் && lnum <= nmlines;
								       ++lnum)
	எனில் (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    க்கு (lnum = curbuf->b_ml.ml_வரி_மொத்தம்; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_வரி_மொத்தம் - nmlines; --lnum)
	எனில் (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#அந்தர்பாவ "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    நிலையான முழு
chk_modeline(
    linenr_T	lnum,
    முழு		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*வரி_end;		// point to the end of the line
    char_u	*e;
    முழு		prev;
    முழு		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    வரி_end = s + ml_get_len(lnum);
    க்கு (; *s != NUL; ++s)
    {
	எனில் (prev == -1 || vim_isspace(prev))
	{
	    எனில் ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		நிறுத்து;
	    // Accept both "vim" and "Vim".
	    எனில் ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		முழு vers;

		எனில் (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		இல்லையெனில்
		    e = s + 3;
		vers = getdigits(&e);
		எனில் (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    நிறுத்து;
	    }
	}
	prev = *s;
    }

    எனில் (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	முழு	end;

	செய்				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	போது (s[-1] != ':');

	len = (size_t)(வரி_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	எனில் (linecopy == வெற்று)
	    திரும்பு FAIL;

	வரி_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_தள்ளு(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	போது (end == FALSE)
	{
	    s = skipwhite(s);
	    எனில் (*s == NUL)
		நிறுத்து;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    க்கு (e = s; *e != ':' && *e != NUL; ++e)
		எனில் (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(வரி_end - (e + 1)) + 1);	// +1 for NUL
		    --வரி_end;
		}
	    எனில் (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    எனில் (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		எனில் (*e != ':')		// no terminating ':'?
		    நிறுத்து;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    எனில் (*s != NUL)		// skip over an empty "::"
	    {
		முழு secure_save = secure;
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
		எனில் (retval == FAIL)		// stop if error found
		    நிறுத்து;
	    }
	    s = (e == வரி_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_எடு();
	vim_free(linecopy);
    }
    திரும்பு retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    முழு
bt_normal(buf_T *buf)
{
    திரும்பு buf != வெற்று && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    முழு
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    திரும்பு buf != வெற்று && buf->b_p_bt[0] == 'q';
#இல்லையெனில்
    திரும்பு FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    முழு
bt_terminal(buf_T *buf UNUSED)
{
#எனில் defined(FEAT_TERMINAL)
    திரும்பு buf != வெற்று && buf->b_p_bt[0] == 't';
#இல்லையெனில்
    திரும்பு FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    முழு
bt_help(buf_T *buf)
{
    திரும்பு buf != வெற்று && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    முழு
bt_prompt(buf_T *buf)
{
    திரும்பு buf != வெற்று && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#எனில் defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    முழு
bt_popup(buf_T *buf)
{
    திரும்பு buf != வெற்று && buf->b_p_bt != வெற்று
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    முழு
bt_nofilename(buf_T *buf)
{
    திரும்பு buf != வெற்று && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    நிலையான முழு
bt_nofileread(buf_T *buf)
{
    திரும்பு buf != வெற்று && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#எனில் defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    முழு
bt_nofile(buf_T *buf)
{
    திரும்பு buf != வெற்று && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    முழு
bt_dontwrite(buf_T *buf)
{
    திரும்பு buf != வெற்று && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    முழு
bt_dontwrite_செய்தி(buf_T *buf)
{
    எனில் (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    முழு
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    தேர்வு (buf->b_p_bh[0])
    {
	நிலை 'u':		    // "unload"
	நிலை 'w':		    // "wipe"
	நிலை 'd': திரும்பு FALSE;	    // "delete"
	நிலை 'h': திரும்பு TRUE;	    // "hide"
    }
    திரும்பு (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#எனில் defined(FEAT_QUICKFIX)
    எனில் (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	எனில் (buf->b_fnum == qf_அடுக்கு_get_bufnr())
	    திரும்பு (char_u *)_(செய்தி_qflist);
	இல்லையெனில்
	    திரும்பு (char_u *)_(செய்தி_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    எனில் (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	எனில் (buf->b_term != வெற்று)
	    திரும்பு term_get_status_text(buf->b_term);
#endif
	எனில் (buf->b_fname != வெற்று)
	    திரும்பு buf->b_fname;
	எனில் (buf == cmdwin_buf)
	    திரும்பு (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	எனில் (bt_prompt(buf))
	    திரும்பு (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	எனில் (bt_popup(buf))
	    திரும்பு (char_u *)_("[Popup]");
#endif
	திரும்பு (char_u *)_("[Scratch]");
    }

    எனில் (buf->b_fname == வெற்று)
	திரும்பு buf_get_fname(buf);
    திரும்பு வெற்று;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    எனில் (buf->b_fname == வெற்று)
	திரும்பு (char_u *)_("[No Name]");
    திரும்பு buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    வெறுமை
set_buflisted(முழு on)
{
    எனில் (on == curbuf->b_p_bl)
	திரும்பு;

    curbuf->b_p_bl = on;
    எனில் (on)
	apply_autocmds(EVENT_BUFADD, வெற்று, வெற்று, FALSE, curbuf);
    இல்லையெனில்
	apply_autocmds(EVENT_BUFDELETE, வெற்று, வெற்று, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    முழு
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    முழு		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(வெற்று, வெற்று, (linenr_T)1, BLN_DUMMY);
    எனில் (newbuf == வெற்று)
	திரும்பு TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    எனில் (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	திரும்பு TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    எனில் (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	திரும்பு TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    எனில் (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	எனில் (buf->b_ml.ml_வரி_மொத்தம் == curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    differ = FALSE;
	    க்கு (lnum = 1; lnum <= curbuf->b_ml.ml_வரி_மொத்தம்; ++lnum)
		எனில் (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    நிறுத்து;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    எனில் (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    திரும்பு differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    வெறுமை
wipe_buffer(
    buf_T	*buf,
    முழு		aucmd)	    // When TRUE trigger autocommands.
{
    எனில் (buf->b_fnum == உச்சி_file_இலக்கம் - 1)
	--உச்சி_file_இலக்கம்;

    எனில் (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(வெற்று, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    எனில் (!aucmd)
	unblock_autocmds();
}
