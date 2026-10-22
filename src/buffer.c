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

#अन्तर्भाव "vim.h"


#ifdef FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_DEPTH 100
#endif

स्थिर शून्य	enter_buffer(buf_T *buf);
स्थिर शून्य	buflist_getfpos(शून्य);
स्थिर char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, पूर्णांक ignore_case);
स्थिर char_u	*fname_match(regmatch_T *rmp, char_u *name, पूर्णांक ignore_case);
#ifdef UNIX
स्थिर buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
स्थिर पूर्णांक	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
स्थिर पूर्णांक	buf_same_ino(buf_T *buf, stat_T *stp);
#अन्यथा
स्थिर पूर्णांक	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
स्थिर पूर्णांक	मूल्य_changed(char_u *str, char_u **last);
#जर defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
स्थिर पूर्णांक build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, माप_प्रकार outlen, char_u **fmt_arg,
		char_u *opt_name, पूर्णांक opt_scope, पूर्णांक fillchar, पूर्णांक maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, पूर्णांक *lbreaks, पूर्णांक *carry_hl);
#endif
स्थिर पूर्णांक	append_arg_संख्या(win_T *wp, char_u *buf, माप_प्रकार buflen, पूर्णांक add_file);
स्थिर शून्य	free_buffer(buf_T *);
स्थिर शून्य	free_buffer_stuff(buf_T *buf, पूर्णांक free_options);
स्थिर पूर्णांक	bt_nofileread(buf_T *buf);
स्थिर पूर्णांक	do_buffer_ext(पूर्णांक action, पूर्णांक start, पूर्णांक dir, पूर्णांक मोजणी, पूर्णांक flags);

#ifdef UNIX
# define dev_T dev_t
#अन्यथा
# define dev_T अचिन्हित
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    साठी ((buf) = lastbuf; (buf) != रिक्त; (buf) = (buf)->b_prev)

#जर defined(FEAT_QUICKFIX)
स्थिर अक्षर *संदेश_loclist = N_("[Location List]");
स्थिर अक्षर *संदेश_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
स्थिर पूर्णांक	buf_free_मोजणी = 0;

स्थिर पूर्णांक	शीर्ष_file_अंक = 1;	// highest file number
स्थिर garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    स्थिर शून्य
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"जर exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    स्थिर पूर्णांक
calc_percentage(दीर्घ part, दीर्घ whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    परत (part > 1000000L)
	? (पूर्णांक)(part / (whole / 100L))
	: (पूर्णांक)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    पूर्णांक
get_highest_fnum(शून्य)
{
    परत शीर्ष_file_अंक - 1;
}

/*
 * Read data from buffer for retrying.
 */
    स्थिर पूर्णांक
read_buffer(
    पूर्णांक		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    पूर्णांक		flags)		    // extra flags for readfile()
{
    पूर्णांक		retval = OK;
    linenr_T	ओळ_मोजणी;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    ओळ_मोजणी = curbuf->b_ml.ml_ओळ_मोजणी;
    retval = readfile(
	    read_stdin ? रिक्त : curbuf->b_ffname,
	    read_stdin ? रिक्त : curbuf->b_fname,
	    ओळ_मोजणी, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    जर (retval == OK)
    {
	// Delete the binary lines.
	पर्यंत (--ओळ_मोजणी >= 0)
	    ml_delete((linenr_T)1);
    }
    अन्यथा
    {
	// Delete the converted lines.
	पर्यंत (curbuf->b_ml.ml_ओळ_मोजणी > ओळ_मोजणी)
	    ml_delete(ओळ_मोजणी);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    जर (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	जर (!readonlymode && !BUFEMPTY())
	    changed();
	अन्यथा जर (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	जर (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, रिक्त, रिक्त, FALSE,
							      curbuf, &retval);
#अन्यथा
	    apply_autocmds(EVENT_STDINREADPOST, रिक्त, रिक्त, FALSE, curbuf);
#endif
	}
    }
    परत retval;
}

#जर defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    शून्य
buffer_ensure_loaded(buf_T *buf)
{
    जर (buf->b_ml.ml_mfp != रिक्त)
	परत;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    जर (curbuf == buf)
    {
	जर (swap_exists_action != SEA_READONLY)
	    swap_exists_action = SEA_NONE;
	open_buffer(FALSE, रिक्त, 0);
	aucmd_restbuf(&aco);
    }
}
#endif

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णांक
open_buffer(
    पूर्णांक		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    पूर्णांक		flags_arg)	    // extra flags for readfile()
{
    पूर्णांक		flags = flags_arg;
    पूर्णांक		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    दीर्घ	old_tw = curbuf->b_p_tw;
#endif
    पूर्णांक		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    जर (readonlymode && curbuf->b_ffname != रिक्त
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    जर (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    जर (curbuf->b_ml.ml_mfp != रिक्त)
		थांबा;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	जर (curbuf == रिक्त)
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
	जर (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(रिक्त, curwin);
#endif
	परत FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    जर (curbuf->b_ml.ml_mfp != रिक्त)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    जर (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    जर (curbuf->b_ffname != रिक्त
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	पूर्णांक old_संदेश_silent = संदेश_silent;
#ifdef UNIX
	पूर्णांक	save_bin = curbuf->b_p_bin;
	पूर्णांक perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	पूर्णांक oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	जर (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	जर (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	जर (shortmess(SHM_FILEINFO))
	    संदेश_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	जर (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    जर (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_buffer(FALSE, eap, flags);
	}
#endif
	संदेश_silent = old_संदेश_silent;
#ifdef FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#endif
	// Help buffer is filtered.
	जर (bt_help(curbuf))
	    fix_help_buffer();
    }
    अन्यथा जर (read_stdin)
    {
	पूर्णांक	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(रिक्त, रिक्त, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, रिक्त,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	जर (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    जर (curbuf->b_ml.ml_mfp != रिक्त
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    जर (curbuf->b_flags & BF_NEVERLOADED)
    {
	(शून्य)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    जर ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != रिक्त)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != रिक्त)
#endif
       )
	changed();
    अन्यथा जर (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    जर (aborting())
#अन्यथा
    जर (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    जर (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf, &retval);
#अन्यथा
    apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
#endif

    जर (retval != OK)
	परत retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    जर (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != रिक्त)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	जर (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    जर ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, रिक्त, रिक्त,
			FALSE, curbuf, &retval);
#अन्यथा
	    apply_autocmds(EVENT_BUFWINENTER, रिक्त, रिक्त,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    परत retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    शून्य
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == रिक्त ? 0 : buf->b_fnum;
    bufref->br_buf_free_मोजणी = buf_free_मोजणी;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    पूर्णांक
bufref_valid(bufref_T *bufref)
{
    परत bufref->br_buf_free_मोजणी == buf_free_मोजणी
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    पूर्णांक
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	जर (bp == buf)
	    परत TRUE;
    परत FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
स्थिर hashtab_T buf_hashtab;

    स्थिर शून्य
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((अक्षर *)buf->b_किल्ली, आकार(buf->b_किल्ली), "%x", buf->b_fnum);
    जर (hash_add(&buf_hashtab, buf->b_किल्ली, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    स्थिर शून्य
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_किल्ली);

    जर (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    स्थिर पूर्णांक
can_unload_buffer(buf_T *buf)
{
    पूर्णांक	    can_unload = !buf->b_locked;

    जर (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    जर (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		थांबा;
	    }
    }
    // Don't unload the buffer while it's still being saved
    जर (can_unload && buf->b_saving)
	can_unload = FALSE;

    जर (!can_unload)
    {
	char_u *fname = buf->b_fname != रिक्त ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != रिक्त ? fname : (char_u *)"[No Name]");
    }
    परत can_unload;
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
    पूर्णांक
close_buffer(
    win_T	*win,
    buf_T	*buf,
    पूर्णांक		action,
    पूर्णांक		abort_if_last,
    पूर्णांक		ignore_abort,
    पूर्णांक		set_context)
{
    पूर्णांक		hiding_buf;
    bufref_T	bufref;
    पूर्णांक		is_curwin = (curwin != रिक्त && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    पूर्णांक		unload_buf = (action != 0);
    पूर्णांक		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    पूर्णांक		del_buf = (action == DOBUF_DEL || wipe_buf);
    पूर्णांक		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    पूर्णांक		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    जर (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    अन्यथा जर (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    अन्यथा जर (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    जर (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	जर (term_job_running(buf->b_term))
	{
	    जर (wipe_buf || unload_buf)
	    {
		जर (!can_unload_buffer(buf))
		    परत FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    अन्यथा
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	अन्यथा जर (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	अन्यथा
	{
	    जर (del_buf || unload_buf)
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
    जर ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	परत FALSE;

    जर (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	जर (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    जर ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	जर (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    परत FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	जर (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    जा aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	जर (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    जर (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		जा aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    जर (abort_if_last && one_window())
		// Autocommands made this the only window.
		जा aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	जर (!ignore_abort && aborting())
	    परत FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    जर (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    जर (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    जर (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    जर (!hiding_buf || !unload_buf)
	परत FALSE;

    // Always remove the buffer when there is no file name.
    जर (buf->b_ffname == रिक्त)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    जर (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	परत FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    जर ((win_valid || closed_popup) && win->w_buffer == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_buffer = रिक्त;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    जर (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != रिक्त || buf->b_next != रिक्त
#जर defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	जर (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    जर (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, आकार(पूर्णांक), 50);
	    जर (ga_grow(&buf_reuse, 1) == OK)
		((पूर्णांक *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	जर (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	अन्यथा
	    buf->b_sfname = रिक्त;
	VIM_CLEAR(buf->b_ffname);
	जर (buf->b_prev == रिक्त)
	    firstbuf = buf->b_next;
	अन्यथा
	    buf->b_prev->b_next = buf->b_next;
	जर (buf->b_next == रिक्त)
	    lastbuf = buf->b_prev;
	अन्यथा
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    अन्यथा
    {
	जर (del_buf)
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
	जर (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    प्रतिज्ञा(!win_valid || !is_curwin || win == curwin);
    परत TRUE;
}

/*
 * Make buffer not contain a file.
 */
    शून्य
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_ओळ_मोजणी = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_start_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_start_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_start_bomb = FALSE;
    buf->b_ml.ml_mfp = रिक्त;
    buf->b_ml.ml_flags = ML_EMPTY;		// empty buffer
#ifdef FEAT_NETBEANS_INTG
    netbeans_deleted_all_ओळी(buf);
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
    पूर्णांक
buf_freeall(buf_T *buf, पूर्णांक flags)
{
    पूर्णांक		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    पूर्णांक		is_curwin = (curwin != रिक्त && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    जर (buf->b_ml.ml_mfp != रिक्त)
    {
	जर (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    परत FALSE;
    }
    जर ((flags & BFA_DEL) && buf->b_p_bl)
    {
	जर (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    परत FALSE;
    }
    जर (flags & BFA_WIPE)
    {
	जर (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    परत FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    जर (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    जर ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	परत FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    जर (buf == curbuf && !is_curbuf)
	परत FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    जर (buf == curbuf && VIsual_active
#जर defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    जर (curwin != रिक्त && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    जर (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_ओळ_मोजणी = 0;    // no lines in buffer
    जर ((flags & BFA_KEEP_UNDO) == 0)
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
    परत TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    स्थिर शून्य
free_buffer(buf_T *buf)
{
    ++buf_free_मोजणी;
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

    जर (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    अन्यथा
    {
	vim_free(buf);
	जर (curbuf == buf)
	    curbuf = रिक्त;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    स्थिर शून्य
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_संख्या;
    di->di_tv.v_lock = VAR_FIXED;
    di->di_tv.vval.v_संख्या = 0;

#ifdef FEAT_EVAL
    STRCPY(buf->b_ct_di.di_किल्ली, "changedtick");
    (शून्य)dict_add(buf->b_vars, di);
#endif
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    स्थिर शून्य
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    पर्यंत (buf->b_wininfo != रिक्त)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_next;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    स्थिर शून्य
free_buffer_stuff(
    buf_T	*buf,
    पूर्णांक		free_options)		// free options as well
{
    जर (free_options)
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
    शून्य
free_wininfo(wininfo_T *wip)
{
    जर (wip->wi_optset)
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
    शून्य
goto_buffer(
    exarg_T	*eap,
    पूर्णांक		start,
    पूर्णांक		dir,
    पूर्णांक		मोजणी)
{
    bufref_T	old_curbuf;
    पूर्णांक		save_sea = swap_exists_action;
    पूर्णांक		skip_help_buf;

    निवड (eap->cmdidx)
    {
	स्थिती CMD_bnext:
	स्थिती CMD_sbnext:
	स्थिती CMD_bNext:
	स्थिती CMD_bprevious:
	स्थिती CMD_sbNext:
	स्थिती CMD_sbprevious:
	    skip_help_buf = TRUE;
	    थांबा;
	पूर्वनिर्धारित:
	    skip_help_buf = FALSE;
	    थांबा;
    }

    set_bufref(&old_curbuf, curbuf);

    जर (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (शून्य)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, मोजणी,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    जर (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#जर defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#जर defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    अन्यथा
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    शून्य
handle_swap_exists(bufref_T *old_curbuf)
{
#जर defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    दीर्घ	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    जर (swap_exists_action == SEA_QUIT)
    {
#जर defined(FEAT_EVAL)
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
	जर (old_curbuf == रिक्त || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(रिक्त, रिक्त, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	अन्यथा
	    buf = old_curbuf->br_buf;
	जर (buf != रिक्त)
	{
	    पूर्णांक old_संदेश_silent = संदेश_silent;

	    जर (shortmess(SHM_FILEINFO))
		संदेश_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    संदेश_silent = old_संदेश_silent;

#ifdef FEAT_SYN_HL
	    जर (old_tw != curbuf->b_p_tw)
		check_colorcolumn(रिक्त, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#जर defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    अन्यथा जर (swap_exists_action == SEA_RECOVER)
    {
#जर defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Recover at ATTENTION prompt.
	संदेश_scroll = TRUE;
	ml_recover(FALSE);
	संदेश_puts("\n");	// don't overwrite the last message
	cmdline_row = संदेश_row;
	do_modelines(0);

#जर defined(FEAT_EVAL)
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
    स्थिर पूर्णांक
empty_curbuf(
    पूर्णांक close_others,
    पूर्णांक forceit,
    पूर्णांक action)
{
    पूर्णांक	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    जर (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	परत FAIL;
    }

    set_bufref(&bufref, buf);
    जर (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, रिक्त, रिक्त, रिक्त, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    जर (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(रिक्त, buf, action, FALSE, FALSE, FALSE);
    जर (!close_others)
	need_fileinfo = FALSE;
    अन्यथा जर (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    परत retval;
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
    स्थिर पूर्णांक
do_buffer_ext(
    पूर्णांक		action,
    पूर्णांक		start,
    पूर्णांक		dir,		// FORWARD or BACKWARD
    पूर्णांक		मोजणी,		// buffer number or number of buffers
    पूर्णांक		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    पूर्णांक		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    निवड (start)
    {
	स्थिती DOBUF_FIRST:   buf = firstbuf; थांबा;
	स्थिती DOBUF_LAST:    buf = lastbuf;  थांबा;
	पूर्वनिर्धारित:	    buf = curbuf;   थांबा;
    }
    जर (start == DOBUF_MOD)	    // find next modified buffer
    {
	पर्यंत (मोजणी-- > 0)
	{
	    करा
	    {
		buf = buf->b_next;
		जर (buf == रिक्त)
		    buf = firstbuf;
	    }
	    पर्यंत (buf != curbuf && !bufIsChanged(buf));
	}
	जर (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    परत FAIL;
	}
    }
    अन्यथा जर (start == DOBUF_FIRST && मोजणी) // find specified buffer number
    {
	पर्यंत (buf != रिक्त && buf->b_fnum != मोजणी)
	    buf = buf->b_next;
    }
    अन्यथा
    {
	पूर्णांक help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = रिक्त;
	पर्यंत (मोजणी > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    जर (bp == रिक्त)
		bp = buf;
	    जर (dir == FORWARD)
	    {
		buf = buf->b_next;
		जर (buf == रिक्त)
		    buf = firstbuf;
	    }
	    अन्यथा
	    {
		buf = buf->b_prev;
		जर (buf == रिक्त)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    जर (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--मोजणी;
		bp = रिक्त;	// use this buffer as new starting point
	    }
	    जर (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		परत FAIL;
	    }
	}
    }

    जर (buf == रिक्त)	    // could not find it
    {
	जर (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    जर (!unload)
		semsg(_(e_buffer_nr_does_not_exist), मोजणी);
	}
	अन्यथा जर (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	अन्यथा
	    emsg(_(e_cannot_go_before_first_buffer));
	परत FAIL;
    }
#ifdef FEAT_PROP_POPUP
    जर ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	परत OK;
#endif
    जर (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	परत FAIL;

    जर ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), मोजणी);
	परत FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    जर (unload)
    {
	पूर्णांक	forward;
	bufref_T bufref;

	जर (!can_unload_buffer(buf))
	    परत FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	जर (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == रिक्त && !buf->b_p_bl)
	    परत FAIL;

	जर ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    जर ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		जर (term_job_running(buf->b_term))
		{
		    जर (term_confirm_stop(buf) == FAIL)
			परत FAIL;
		}
		अन्यथा
# endif
		{
		    dialog_changed(buf, FALSE);
		    जर (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			परत FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    जर (bufIsChanged(buf))
			परत FAIL;
		}
	    }
	    अन्यथा
#endif
	    {
		no_write_message_buf(buf);
		परत FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	जर (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    जर (bp->b_p_bl && bp != buf)
		थांबा;
	जर (bp == रिक्त && buf == curbuf)
	    परत empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	पर्यंत (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != रिक्त))
	{
	    जर (win_close(curwin, FALSE) == FAIL)
		थांबा;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	जर (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    जर (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(रिक्त, buf, action, FALSE, FALSE, FALSE);
	    परत OK;
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
	buf = रिक्त;	// selected buffer
	bp = रिक्त;	// used when no loaded buffer found
	जर (au_new_curbuf.br_buf != रिक्त && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	अन्यथा जर (curwin->w_jumplistlen > 0)
	{
	    पूर्णांक     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    जर (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    पर्यंत (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		जर (buf != रिक्त)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    जर (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = रिक्त;
		    अन्यथा जर (buf->b_ml.ml_mfp == रिक्त)
		    {
			// skip unloaded buf, but may keep it for later
			जर (bp == रिक्त)
			    bp = buf;
			buf = रिक्त;
		    }
		}
		जर (buf != रिक्त)   // found a valid buffer: stop searching
		    थांबा;
		// advance to older entry in jump list
		जर (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    थांबा;
		जर (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		जर (jumpidx == forward)		// List exhausted for sure
		    थांबा;
	    }
	}

	जर (buf == रिक्त)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    साठी (;;)
	    {
		जर (buf == रिक्त)
		{
		    जर (!forward)	// tried both directions
			थांबा;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    सुरूठेवा;
		}
		// in non-help buffer, try to skip help buffers, and vv
		जर (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    जर (buf->b_ml.ml_mfp != रिक्त)   // found loaded buffer
			थांबा;
		    जर (bp == रिक्त)	// remember unloaded buf for later
			bp = buf;
		}
		जर (forward)
		    buf = buf->b_next;
		अन्यथा
		    buf = buf->b_prev;
	    }
	}
	जर (buf == रिक्त)	// No loaded buffer, use unloaded one
	    buf = bp;
	जर (buf == रिक्त)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		जर (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    थांबा;
	}
	जर (buf == रिक्त)	// Still no buffer, just take one
	{
	    जर (curbuf->b_next != रिक्त)
		buf = curbuf->b_next;
	    अन्यथा
		buf = curbuf->b_prev;
	    जर (bt_quickfix(buf)
		    || (buf != रिक्त && buf != curbuf && buf->b_locked_split))
		buf = रिक्त;
	}
    }

    जर (buf == रिक्त)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	परत empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    जर (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != रिक्त)
	परत OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    जर (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	परत FAIL;
    }
    जर (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	परत FAIL;

    // go to current buffer - nothing to do
    जर (buf == curbuf)
	परत OK;

    // Check if the current buffer may be abandoned.
    जर (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#जर defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	जर ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    जर (term_job_running(curbuf->b_term))
	    {
		जर (term_confirm_stop(curbuf) == FAIL)
		    परत FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    अन्यथा
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		जर (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    परत FAIL;

		जर (bufIsChanged(curbuf))
		{
		    no_write_message();
		    परत FAIL;
		}
	    }
	}
	अन्यथा
#endif
	{
	    no_write_message();
	    परत FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    जर (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#जर defined(FEAT_EVAL)
    जर (aborting())	    // autocmds may abort script processing
	परत FAIL;
#endif

    परत OK;
}

    पूर्णांक
do_buffer(
    पूर्णांक		action,
    पूर्णांक		start,
    पूर्णांक		dir,		// FORWARD or BACKWARD
    पूर्णांक		मोजणी,		// buffer number or number of buffers
    पूर्णांक		forceit)	// TRUE when using !
{
    परत do_buffer_ext(action, start, dir, मोजणी,
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
    अक्षर *
do_bufdel(
    पूर्णांक		command,
    char_u	*arg,		// pointer to extra arguments
    पूर्णांक		addr_मोजणी,
    पूर्णांक		start_bnr,	// first buffer number in a range
    पूर्णांक		end_bnr,	// buffer nr or last buffer nr in a range
    पूर्णांक		forceit)
{
    पूर्णांक		do_current = 0;	// delete current buffer?
    पूर्णांक		deleted = 0;	// number of buffers deleted
    अक्षर	*errormsg = रिक्त; // return value
    पूर्णांक		bnr;		// buffer number
    char_u	*p;

    जर (addr_मोजणी == 0)
    {
	(शून्य)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    अन्यथा
    {
	जर (addr_मोजणी == 2)
	{
	    जर (*arg)		// both range and argument is not allowed
		परत ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	अन्यथा	// addr_count == 1
	    bnr = end_bnr;

	साठी ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    जर (bnr == curbuf->b_fnum)
		do_current = bnr;
	    अन्यथा जर (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    जर (addr_मोजणी == 2)
	    {
		जर (++bnr > end_bnr)
		    थांबा;
	    }
	    अन्यथा    // addr_count == 1
	    {
		arg = skipwhite(arg);
		जर (*arg == NUL)
		    थांबा;
		जर (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    जर (bnr < 0)	    // failed
			थांबा;
		    arg = p;
		}
		अन्यथा
		    bnr = getdigits(&arg);
	    }
	}
	जर (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	जर (deleted == 0)
	{
	    जर (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    अन्यथा जर (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    अन्यथा
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (अक्षर *)IObuff;
	}
	अन्यथा जर (deleted >= p_report)
	{
	    जर (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    अन्यथा जर (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    अन्यथा
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    परत errormsg;
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
    शून्य
set_curbuf(buf_T *buf, पूर्णांक action)
{
    buf_T	*prevbuf;
    पूर्णांक		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    दीर्घ	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    पूर्णांक		valid;

    setpcmark();
    जर ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    जर (!apply_autocmds(EVENT_BUFLEAVE, रिक्त, रिक्त, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	जर (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	जर (unload)
	    close_windows(prevbuf, FALSE);
#जर defined(FEAT_EVAL)
	जर (bufref_valid(&prevbufref) && !aborting())
#अन्यथा
	जर (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    जर (prevbuf == curbuf
		    && ((State & MODE_घाला) == 0 || curbuf->b_nwindows <= 1))
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
    जर ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == रिक्त)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	जर (!valid)
	    enter_buffer(lastbuf);
	अन्यथा
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	जर (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(रिक्त, curwin);
#endif
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    स्थिर शून्य
enter_buffer(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    जर (VIsual_active
#जर defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    जर (curwin->w_buffer != रिक्त)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    जर (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    अन्यथा
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    जर (curwin->w_p_diff)
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
    जर (curbuf->b_ml.ml_mfp == रिक्त)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	जर (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, रिक्त, 0);
    }
    अन्यथा
    {
	जर (!संदेश_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(शून्य)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
	apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, रिक्त, रिक्त, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    जर (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    जर (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    जर (curbuf->b_kmap_state & KEYMAP_INIT)
	(शून्य)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    जर (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(शून्य)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#जर defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    शून्य
do_autochdir(शून्य)
{
    जर ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != रिक्त
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	last_chdir_reason = "autochdir";
    }
}
#endif

    शून्य
no_write_message_buf(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    जर (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    अन्यथा
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    शून्य
no_write_message(शून्य)
{
#ifdef FEAT_TERMINAL
    जर (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    अन्यथा
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    शून्य
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    जर (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    अन्यथा
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
    पूर्णांक
curbuf_reusable(शून्य)
{
    परत (curbuf != रिक्त
	&& curbuf->b_ffname == रिक्त
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == रिक्त || BUFEMPTY())
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
    पूर्णांक		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    जर (शीर्ष_file_अंक == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    जर (sfname == रिक्त || mch_stat((अक्षर *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    जर (ffname != रिक्त && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#अन्यथा
		buflist_findname(ffname)
#endif
		) != रिक्त)
    {
	vim_free(ffname);
	जर (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? रिक्त : curwin,
						      lnum, (colnr_T)0, FALSE);

	जर ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	जर ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    जर (!(flags & BLN_DUMMY))
	    {
		जर (apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, buf)
			&& !bufref_valid(&bufref))
		    परत रिक्त;
	    }
	}
	परत buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = रिक्त;
    जर ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	जर (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    परत रिक्त;
	}
#endif
	जर (!bufref_valid(&bufref))
	    buf = रिक्त;		// buf was deleted; allocate a new buffer
    }
    जर (buf != curbuf || curbuf == रिक्त)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	जर (buf == रिक्त)
	{
	    vim_free(ffname);
	    परत रिक्त;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	जर (buf->b_vars == रिक्त)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    परत रिक्त;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    जर (ffname != रिक्त)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    जर ((ffname != रिक्त && (buf->b_ffname == रिक्त || buf->b_sfname == रिक्त))
	    || buf->b_wininfo == रिक्त)
    {
	जर (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	अन्यथा
	    buf->b_sfname = रिक्त;
	VIM_CLEAR(buf->b_ffname);
	जर (buf != curbuf)
	    free_buffer(buf);
	परत रिक्त;
    }

    जर (buf == curbuf)
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
    अन्यथा
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = रिक्त;
	जर (firstbuf == रिक्त)		// buffer list is empty
	{
	    buf->b_prev = रिक्त;
	    firstbuf = buf;
	}
	अन्यथा				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	जर ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((पूर्णांक *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    पर्यंत (buf->b_prev != रिक्त && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		जर (prev->b_next != रिक्त)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		जर (buf->b_prev != रिक्त)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		जर (lastbuf == buf)
		    lastbuf = prev;
		जर (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	अन्यथा
	    buf->b_fnum = शीर्ष_file_अंक++;
	जर (शीर्ष_file_अंक < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    जर (emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    शीर्ष_file_अंक = 1;
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
    जर (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    अन्यथा
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    जर (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    जर (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	जर (apply_autocmds(EVENT_BUFNEW, रिक्त, रिक्त, FALSE, buf)
		&& !bufref_valid(&bufref))
	    परत रिक्त;
	जर (flags & BLN_LISTED)
	{
	    जर (apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, buf)
		    && !bufref_valid(&bufref))
		परत रिक्त;
	}
#ifdef FEAT_EVAL
	जर (aborting())		// autocmds may abort script processing
	    परत रिक्त;
#endif
    }

    परत buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    शून्य
free_buf_options(
    buf_T	*buf,
    पूर्णांक		free_p_ff)
{
    जर (free_p_ff)
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
#जर defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#जर defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#जर defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#जर defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    जर (buf->b_p_किल्ली != रिक्त && *buf->b_p_किल्ली != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_किल्ली, STRLEN(buf->b_p_किल्ली));
# endif
    clear_string_option(&buf->b_p_किल्ली);
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
    buf->b_s.b_cap_prog = रिक्त;
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
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_मोजणी);
    buf->b_p_cpt_मोजणी = 0;
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
    पूर्णांक
buflist_getfile(
    पूर्णांक		n,
    linenr_T	lnum,
    पूर्णांक		options,
    पूर्णांक		forceit)
{
    buf_T	*buf;
    win_T	*wp = रिक्त;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    जर (buf == रिक्त)
    {
	जर ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	अन्यथा
	    semsg(_(e_buffer_nr_not_found), n);
	परत FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    जर (buf == curbuf)
	परत OK;

    जर (text_or_buf_locked())
	परत FAIL;

    // altfpos may be changed by getfile(), get it now
    जर (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    अन्यथा
	col = 0;

    जर (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	जर (wp == रिक्त && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    जर (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    अन्यथा जर (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		परत FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    पूर्णांक retval = FAIL;
    जर (GETFILE_SUCCESS(getfile(buf->b_fnum, रिक्त, रिक्त,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	जर (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    जर (RedrawingDisabled > 0)
	--RedrawingDisabled;
    परत retval;
}

/*
 * go to the last know line number for the current buffer
 */
    स्थिर शून्य
buflist_getfpos(शून्य)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    जर (p_sol)
	curwin->w_cursor.col = 0;
    अन्यथा
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
    buf_T	*buf = रिक्त;

    // First make the name into a full path name
    ffname = FullName_save(fname,
#ifdef UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#अन्यथा
	    FALSE
#endif
	    );
    जर (ffname != रिक्त)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    परत buf;
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

    जर (mch_stat((अक्षर *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    परत buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    स्थिर buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#endif
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_LAST(buf)
	जर ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    परत buf;
    परत रिक्त;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    पूर्णांक
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    पूर्णांक		unlisted,	// find unlisted buffers
    पूर्णांक		diffmode UNUSED, // find diff-mode buffers only
    पूर्णांक		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    पूर्णांक		match = -1;
    पूर्णांक		find_listed;
    char_u	*pat;
    char_u	*patend;
    पूर्णांक		attempt;
    char_u	*p;
    पूर्णांक		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    जर ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	जर (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	अन्यथा
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	जर (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
    अन्यथा
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, रिक्त, FALSE);
	जर (pat == रिक्त)
	    परत -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	साठी (;;)
	{
	    साठी (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		जर (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		जर (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    जर (regmatch.regprog == रिक्त)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			परत -1;
		    }
		    जर (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != रिक्त)
		    {
			जर (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				जर (wp->w_buffer == buf)
				    थांबा;
			    जर (wp == रिक्त)
				सुरूठेवा;
			}
			जर (match >= 0)		// already found a match
			{
			    match = -2;
			    थांबा;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		जर (match >= 0)			// found one match
		    थांबा;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    जर (!unlisted || !find_listed || match != -1)
		थांबा;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    जर (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    अन्यथा जर (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    परत match;
}

#ifdef FEAT_VIMINFO
प्रकारव्याख्या संरचना {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    पूर्णांक
ExpandBufnames(
    char_u	*pat,
    पूर्णांक		*अंक_file,
    char_u	***file,
    पूर्णांक		options)
{
    पूर्णांक		मोजणी;
    buf_T	*buf;
    पूर्णांक		round;
    char_u	*p;
    char_u	*patc = रिक्त;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = रिक्त;
#endif
    पूर्णांक		fuzzy;
    fuzmatch_str_T  *fuzmatch = रिक्त;
    regmatch_T	regmatch;
    पूर्णांक		score = 0;
    पूर्णांक		to_free = FALSE;

    *अंक_file = 0;		    // return values in case of FAIL
    *file = रिक्त;

#ifdef FEAT_DIFF
    जर ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	परत FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    जर (!fuzzy)
    {
	जर (*pat == '^' && pat[1] != NUL)
	{
	    पूर्णांक  len = (पूर्णांक)STRLEN(pat);
	    patc = alloc(len);
	    जर (patc == रिक्त)
		परत FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	अन्यथा जर (*pat == '^')
	    patc = (char_u *)"";
	अन्यथा
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    साठी (round = 1; round <= 2; ++round)
    {
	मोजणी = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    जर (!buf->b_p_bl)	// skip unlisted buffers
		सुरूठेवा;
#ifdef FEAT_DIFF
	    जर (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		जर (buf == curbuf || !diff_mode_buf(buf))
		    सुरूठेवा;
#endif

	    जर (!fuzzy)
	    {
		जर (regmatch.regprog == रिक्त)
		{
		    // invalid pattern, possibly after recompiling
		    जर (to_free)
			vim_free(patc);
		    परत FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    अन्यथा
	    {
		p = रिक्त;
		// first try matching with the short file name
		जर ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		जर (p == रिक्त)
		{
		    // next try matching with the full path file name
		    जर ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    जर (p == रिक्त)
		सुरूठेवा;

	    जर (round == 1)
	    {
		++मोजणी;
		सुरूठेवा;
	    }

	    जर (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    अन्यथा
		p = vim_strsave(p);
	    जर (p == रिक्त)
	    {
		जर (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, मोजणी);
		परत FAIL;
	    }

	    जर (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		जर (matches != रिक्त)
		{
		    matches[मोजणी].buf = buf;
		    matches[मोजणी].match = p;
		    मोजणी++;
		}
		अन्यथा
#endif
		    (*file)[मोजणी++] = p;
	    }
	    अन्यथा
	    {
		fuzmatch[मोजणी].idx = मोजणी;
		fuzmatch[मोजणी].str = p;
		fuzmatch[मोजणी].score = score;
		मोजणी++;
	    }
	}
	जर (मोजणी == 0)	// no match found, break here
	    थांबा;
	जर (round == 1)
	{
	    जर (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, मोजणी);
		जर (*file == रिक्त)
		{
		    vim_regfree(regmatch.regprog);
		    जर (to_free)
			vim_free(patc);
		    परत FAIL;
		}
#ifdef FEAT_VIMINFO
		जर (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, मोजणी);
#endif
	    }
	    अन्यथा
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, मोजणी);
		जर (fuzmatch == रिक्त)
		{
		    *अंक_file = 0;
		    *file = रिक्त;
		    परत FAIL;
		}
	    }
	}
    }

    जर (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	जर (to_free)
	    vim_free(patc);
    }

    जर (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	जर (matches != रिक्त)
	{
	    पूर्णांक i;
	    जर (मोजणी > 1)
		क्विक(matches, मोजणी, आकार(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    जर (matches[0].buf == curbuf)
	    {
		साठी (i = 1; i < मोजणी; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[मोजणी-1] = matches[0].match;
	    }
	    अन्यथा
	    {
		साठी (i = 0; i < मोजणी; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    अन्यथा
    {
	जर (fuzzymatches_to_strmatches(fuzmatch, file, मोजणी, FALSE) == FAIL)
	    परत FAIL;
    }

    *अंक_file = मोजणी;
    परत (मोजणी == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    स्थिर char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    पूर्णांक		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    जर (match == रिक्त && rmp->regprog != रिक्त)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    परत match;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    स्थिर char_u *
fname_match(
    regmatch_T	*rmp,
    char_u	*name,
    पूर्णांक		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = रिक्त;
    char_u	*p;

    // extra check for valid arguments
    जर (name == रिक्त || rmp->regprog == रिक्त)
	परत रिक्त;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    जर (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    अन्यथा जर (rmp->regprog != रिक्त)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(रिक्त, name);
	जर (p != रिक्त && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    परत match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(पूर्णांक nr)
{
    char_u	किल्ली[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    जर (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((अक्षर *)किल्ली, आकार(किल्ली), "%x", nr);
    hi = hash_find(&buf_hashtab, किल्ली);

    जर (!HASHITEM_EMPTY(hi))
	परत (buf_T *)(hi->hi_किल्ली
			     - ((अचिन्हित)(curbuf->b_किल्ली - (char_u *)curbuf)));
    परत रिक्त;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    पूर्णांक		n,
    पूर्णांक		fullname,
    पूर्णांक		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    जर (buf == रिक्त)
	परत रिक्त;
    परत home_replace_save(helptail ? buf : रिक्त,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    शून्य
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	col,
    पूर्णांक		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	जर (wip->wi_win == win)
	    थांबा;
    जर (wip == रिक्त)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	जर (wip == रिक्त)
	    परत;
	wip->wi_win = win;
	जर (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    अन्यथा
    {
	// remove the entry from the list
	जर (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	अन्यथा
	    buf->b_wininfo = wip->wi_next;
	जर (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	जर (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    जर (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    जर (win != रिक्त)
	wip->wi_changelistidx = win->w_changelistidx;
    जर (copy_options && win != रिक्त)
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
    wip->wi_prev = रिक्त;
    जर (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    स्थिर पूर्णांक
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    जर (!wip->wi_opt.wo_diff)
	परत FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	जर (wip->wi_win == wp)
	    परत FALSE;
    परत TRUE;
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
    स्थिर wininfo_T *
find_wininfo(
    buf_T	*buf,
    पूर्णांक		need_options,
    पूर्णांक		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	जर (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    थांबा;

    जर (wip != रिक्त)
	परत wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    जर (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    जर (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != रिक्त
			    && wip->wi_win->w_buffer == buf)))
		थांबा;
    }
    अन्यथा
#endif
	wip = buf->b_wininfo;
    परत wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    शून्य
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
    clearFolding(curwin);
#endif

    wip = find_wininfo(buf, TRUE, TRUE);
    जर (wip != रिक्त && wip->wi_win != रिक्त
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
    अन्यथा जर (wip != रिक्त && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    अन्यथा
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    जर (wip != रिक्त)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    जर (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    जर (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    जर (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	जर (p_ea)
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
    स्थिर pos_T no_position = {1, 0, 0};

    wip = find_wininfo(buf, FALSE, FALSE);
    जर (wip != रिक्त)
	परत &(wip->wi_fpos);
    अन्यथा
	परत &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    परत buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    शून्य
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    पूर्णांक		len;
    पूर्णांक		i;
    पूर्णांक		ro_char;
    पूर्णांक		changed_char;
#ifdef FEAT_TERMINAL
    पूर्णांक		job_running;
    पूर्णांक		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = रिक्त, **p;

    जर (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, आकार(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    जर (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	क्विक(buflist.ga_data, (माप_प्रकार)buflist.ga_len,
		आकार(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    साठी (; buf != रिक्त && !got_int; buf = buflist_data != रिक्त
	    ? (++p < buflist_data + buflist.ga_len ? *p : रिक्त)
	    : buf->b_next)
#अन्यथा
    साठी (buf = firstbuf; buf != रिक्त && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	जर ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !bufIsChanged(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == रिक्त || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == रिक्त || buf->b_nwindows != 0))
#ifdef FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!job_running || (job_running && job_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!job_running || (job_running && !job_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (job_running || buf->b_term == रिक्त))
#endif
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    सुरूठेवा;
	name = buf_spname(buf);
	जर (name != रिक्त)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	अन्यथा
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	जर (message_filtered(NameBuff))
	    सुरूठेवा;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	जर (job_running)
	{
	    जर (job_none_open)
		ro_char = '?';
	    अन्यथा
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	अन्यथा जर (buf->b_term != रिक्त)
	    ro_char = 'F';
	अन्यथा
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	संदेश_putchar('\n');
	len = (पूर्णांक)vim_snprintf_safelen((अक्षर *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == रिक्त ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		changed_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	करा
	    IObuff[len++] = ' ';
	पर्यंत (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	जर (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (माप_प्रकार)(IOSIZE - len), buf->b_last_used);
	अन्यथा
#endif
	    vim_snprintf((अक्षर *)IObuff + len, (माप_प्रकार)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (दीर्घ)buflist_findlnum(buf));
	संदेश_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    जर (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    पूर्णांक
buflist_name_nr(
    पूर्णांक		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    जर (buf == रिक्त || buf->b_fname == रिक्त)
	परत FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    परत OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    पूर्णांक
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    पूर्णांक		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = रिक्त;
#ifdef UNIX
    stat_T	st;
#endif

    जर (ffname == रिक्त || *ffname == NUL)
    {
	// Removing the name.
	जर (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	अन्यथा
	    buf->b_sfname = रिक्त;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    अन्यथा
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	जर (ffname == रिक्त)		    // out of memory
	    परत FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	जर (mch_stat((अक्षर *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	जर (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#अन्यथा
	    obuf = buflist_findname(ffname);
#endif
	जर (obuf != रिक्त && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    पूर्णांक		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		जर (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    जर (obuf->b_ml.ml_mfp != रिक्त || in_use)
	    {
		जर (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		परत FAIL;
	    }
	    // delete from the list
	    close_buffer(रिक्त, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	जर (ffname == रिक्त || sfname == रिक्त)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    परत FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	जर (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    जर (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    अन्यथा
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    परत OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    शून्य
buf_set_name(पूर्णांक fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    जर (buf == रिक्त)
	परत;

    जर (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = रिक्त;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    शून्य
buf_name_changed(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    जर (buf->b_ml.ml_mfp != रिक्त)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    जर (buf->b_term != रिक्त)
	term_clear_status_text(buf->b_term);
#endif

    जर (curwin->w_buffer == buf)
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
    जर (buf != रिक्त && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    परत buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    पूर्णांक		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    जर (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	जर (errmsg)
	    emsg(_(e_no_alternate_file));
	परत रिक्त;
    }
    परत fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    पूर्णांक
buflist_add(char_u *fname, पूर्णांक flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, रिक्त, (linenr_T)0, flags);
    जर (buf != रिक्त)
	परत buf->b_fnum;
    परत 0;
}

#जर defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    शून्य
buflist_slash_adjust(शून्य)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	जर (bp->b_ffname != रिक्त)
	    slash_adjust(bp->b_ffname);
	जर (bp->b_sfname != रिक्त)
	    slash_adjust(bp->b_sfname);
    }
}
#endif

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    शून्य
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.col, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    पूर्णांक
otherfile(char_u *ffname)
{
    परत otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , रिक्त
#endif
	    );
}

    स्थिर पूर्णांक
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    जर (ffname == रिक्त || *ffname == NUL || buf->b_ffname == रिक्त)
	परत TRUE;
    जर (fnamecmp(ffname, buf->b_ffname) == 0)
	परत FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	जर (stp == रिक्त)
	{
	    जर (!buf->b_dev_valid || mch_stat((अक्षर *)ffname, &st) < 0)
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
	जर (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    जर (buf_same_ino(buf, stp))
		परत FALSE;
	}
    }
#endif
    परत TRUE;
}

#जर defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    शून्य
buf_setino(buf_T *buf)
{
    stat_T	st;

    जर (buf->b_fname != रिक्त && mch_stat((अक्षर *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    अन्यथा
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    स्थिर पूर्णांक
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    परत (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    शून्य
fileinfo(
    पूर्णांक fullname,	    // when non-zero print full path
    पूर्णांक shorthelp,
    पूर्णांक	dont_truncate)
{
    char_u	*name;
    अक्षर	*buffer;
    माप_प्रकार	bufferlen = 0;

    buffer = alloc(IOSIZE);
    जर (buffer == रिक्त)
	परत;

    जर (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    जर (name != रिक्त)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    अन्यथा
    {
	जर (!fullname && curbuf->b_fname != रिक्त)
	    name = curbuf->b_fname;
	अन्यथा
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : रिक्त, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (पूर्णांक)bufferlen, TRUE);
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

    जर (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_ओळी_संदेश));
    अन्यथा जर (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_ओळ_मोजणी),
	    (दीर्घ)curbuf->b_ml.ml_ओळ_मोजणी,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_ओळ_मोजणी));
    अन्यथा
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (दीर्घ)curwin->w_cursor.lnum,
	    (दीर्घ)curbuf->b_ml.ml_ओळ_मोजणी,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_ओळ_मोजणी));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (पूर्णांक)curwin->w_cursor.col + 1, (पूर्णांक)curwin->w_virtcol + 1);
    }

    (शून्य)append_arg_संख्या(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    जर (dont_truncate)
    {
	पूर्णांक	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	संदेश_start();
	n = संदेश_scroll;
	संदेश_scroll = TRUE;
	संदेश(buffer);
	संदेश_scroll = n;
    }
    अन्यथा
    {
	अक्षर	*p = संदेश_trunc_attr(buffer, FALSE, 0);
	जर (restart_edit != 0 || (संदेश_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_संदेश((char_u *)p, 0);
    }

    vim_free(buffer);
}

    पूर्णांक
col_print(
    char_u  *buf,
    माप_प्रकार  buflen,
    पूर्णांक	    col,
    पूर्णांक	    vcol)
{
    जर (col == vcol)
	परत (पूर्णांक)vim_snprintf_safelen((अक्षर *)buf, buflen, "%d", col);

    परत (पूर्णांक)vim_snprintf_safelen((अक्षर *)buf, buflen, "%d-%d", col, vcol);
}

स्थिर char_u *lasttitle = रिक्त;
स्थिर char_u *lasticon = रिक्त;

/*
 * Put the file name in the title bar and icon of the window.
 */
    शून्य
maketitle(शून्य)
{
    char_u	*title_str = रिक्त;
    char_u	*icon_str = रिक्त;
    पूर्णांक		mustset;
    char_u	buf[IOSIZE];
    माप_प्रकार	buflen = 0;

    जर (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	परत;
    }

    need_maketitle = FALSE;
    जर (!p_title && !p_icon && lasttitle == रिक्त && lasticon == रिक्त)
	परत;  // nothing to do

    जर (p_title)
    {
	पूर्णांक maxlen = 0;

	जर (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    जर (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	जर (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    जर (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, आकार(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, रिक्त, रिक्त, रिक्त);
	    अन्यथा
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	अन्यथा
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (आकार(buf) - 100)
#define SPACE_FOR_DIR   (आकार(buf) - 20)
#define SPACE_FOR_ARGNR (आकार(buf) - 10)  // at least room for " - VIM"

	    // file name
	    जर (curbuf->b_fname == रिक्त)
		buflen = vim_snprintf_safelen((अक्षर *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    अन्यथा जर (curbuf->b_term != रिक्त)
		buflen = vim_snprintf_safelen((अक्षर *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    अन्यथा
	    {
		buflen = vim_snprintf_safelen((अक्षर *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != रिक्त)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    जर (curbuf->b_term == रिक्त)
#endif
	    {
		निवड (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    स्थिती 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (अक्षर *)buf + buflen,
			    आकार(buf) - buflen, " +");
			थांबा;
		    स्थिती 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (अक्षर *)buf + buflen,
			    आकार(buf) - buflen, " =");
			थांबा;
		    स्थिती 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (अक्षर *)buf + buflen,
			    आकार(buf) - buflen, " =+");
			थांबा;
		    स्थिती 4:
		    स्थिती 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (अक्षर *)buf + buflen,
			    आकार(buf) - buflen, " -");
			थांबा;
		    स्थिती 5:
		    स्थिती 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (अक्षर *)buf + buflen,
			    आकार(buf) - buflen, " -+");
			थांबा;
		    पूर्वनिर्धारित:
			थांबा;
		}
	    }

	    // path (surrounded by '()')
	    जर (curbuf->b_fname != रिक्त
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == रिक्त
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((अक्षर *)buf + buflen,
		    आकार(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (पूर्णांक)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		जर (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		जर (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((अक्षर *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		अन्यथा
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    जर (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((अक्षर *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != रिक्त)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    अन्यथा
			buflen += vim_snprintf_safelen((अक्षर *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_संख्या(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((अक्षर *)buf + buflen,
		आकार(buf) - buflen, " - %s",
#जर defined(FEAT_CLIENTSERVER)
		(serverName != रिक्त)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    जर (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		जर (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, आकार(buf));
	    }
	}
    }
    mustset = मूल्य_changed(title_str, &lasttitle);

    जर (p_icon)
    {
	icon_str = buf;
	जर (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    जर (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, आकार(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, रिक्त, रिक्त,
				 रिक्त);
	    अन्यथा
#endif
		icon_str = p_iconstring;
	}
	अन्यथा
	{
	    char_u  *name;
	    पूर्णांक	    namelen;

	    name = buf_spname(curbuf);
	    जर (name == रिक्त)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (पूर्णांक)STRLEN(name);
	    जर (namelen > 100)
	    {
		namelen -= 100;
		जर (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, आकार(buf));
	}
    }

    mustset |= मूल्य_changed(icon_str, &lasticon);

    जर (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    स्थिर पूर्णांक
मूल्य_changed(char_u *str, char_u **last)
{
    जर ((str == रिक्त) != (*last == रिक्त)
	    || (str != रिक्त && *last != रिक्त && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	जर (str == रिक्त)
	{
	    *last = रिक्त;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	अन्यथा
	{
	    *last = vim_strsave(str);
	    परत TRUE;
	}
    }
    परत FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    शून्य
resettitle(शून्य)
{
    mch_settitle(lasttitle, lasticon);
}

#जर defined(EXITFREE)
    शून्य
free_titles(शून्य)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#जर defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
प्रकारव्याख्या संरचना
{
    char_u	*stl_start;
    पूर्णांक		stl_minwid;
    पूर्णांक		stl_maxwid;
    गणना {
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

स्थिर माप_प्रकार		stl_वस्तुसंच_len = 20; // Initial value, grows as needed.
स्थिर stl_item_T      *stl_वस्तुसंच = रिक्त;
स्थिर पूर्णांक	       *stl_groupitem = रिक्त;
स्थिर stl_hlrec_T     *stl_hltab = रिक्त;
स्थिर stl_hlrec_T     *stl_tabtab = रिक्त;
स्थिर stl_clickrec_T  *stl_clicktab = रिक्त;
स्थिर पूर्णांक		*stl_separator_locations = रिक्त;

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
    पूर्णांक
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    माप_प्रकार	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णांक		opt_scope,	// scope for "opt_name"
    पूर्णांक		fillchar,
    पूर्णांक		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    परत build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    रिक्त, रिक्त);
}

    पूर्णांक
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    माप_प्रकार	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णांक		opt_scope,	// scope for "opt_name"
    पूर्णांक		fillchar,
    पूर्णांक		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    पूर्णांक		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    परत build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    रिक्त, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    पूर्णांक
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    माप_प्रकार	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णांक		opt_scope,	// scope for "opt_name"
    पूर्णांक		fillchar,
    पूर्णांक		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    पूर्णांक		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    परत build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    रिक्त, carry_hl);
}
# endif

    पूर्णांक
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णांक		opt_scope)	// scope for "opt_name"
{
    पूर्णांक rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (शून्य)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, आकार(buf), &fmt,
	    opt_name, opt_scope, 0, 0, रिक्त, रिक्त, रिक्त, &rendered_height,
	    रिक्त);
    --emsg_off;
    परत rendered_height;
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
स्थिर पूर्णांक
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    माप_प्रकार	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णांक		opt_scope,	// scope for "opt_name"
    पूर्णांक		fillchar,
    पूर्णांक		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    पूर्णांक		*rendered_height,   // return: stl rendered height (can be NULL)
    पूर्णांक		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    माप_प्रकार	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    पूर्णांक		byteval;
# ifdef FEAT_EVAL
    पूर्णांक		use_sandbox;
    पूर्णांक		save_VIsual_active;
# endif
    पूर्णांक		empty_ओळ;
    दीर्घ	l;
    दीर्घ	n;
    पूर्णांक		prevchar_isflag;
    पूर्णांक		prevchar_isitem;
    पूर्णांक		itemisflag;
    पूर्णांक		fillable;
    char_u	*str;
    दीर्घ	अंक;
    पूर्णांक		width;
    पूर्णांक		itemcnt;
    पूर्णांक		curitem;
    पूर्णांक		groupdepth;
# ifdef FEAT_EVAL
    पूर्णांक		evaldepth;
# endif
    पूर्णांक		minwid;
    पूर्णांक		maxwid;
    पूर्णांक		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    पूर्णांक		save_redraw_not_allowed = redraw_not_allowed;
    पूर्णांक		save_किल्ली_Typed = किल्ली_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    पूर्णांक		did_emsg_before = did_emsg;
    पूर्णांक		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    जर (updating_screen)
	redraw_not_allowed = TRUE;

    जर (stl_वस्तुसंच == रिक्त)
    {
	stl_वस्तुसंच = ALLOC_MULT(stl_item_T, stl_वस्तुसंच_len);
	stl_groupitem = ALLOC_MULT(पूर्णांक, stl_वस्तुसंच_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_वस्तुसंच_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_वस्तुसंच_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_वस्तुसंच_len + 1);

	stl_separator_locations = ALLOC_MULT(पूर्णांक, stl_वस्तुसंच_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    जर (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_संख्या;
	tv.vval.v_संख्या = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	जर (usefmt == रिक्त)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    जर (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    जर (lnum > wp->w_buffer->b_ml.ml_ओळ_मोजणी)
    {
	lnum = wp->w_buffer->b_ml.ml_ओळ_मोजणी;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_ओळ = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    जर (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    अन्यथा
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    जर (carry_hl != रिक्त && *carry_hl != 0)
    {
	stl_वस्तुसंच[curitem].stl_type = Highlight;
	stl_वस्तुसंच[curitem].stl_start = p;
	stl_वस्तुसंच[curitem].stl_minwid = *carry_hl;
	stl_वस्तुसंच[curitem].stl_clickfunc = रिक्त;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    साठी (s = usefmt; *s != NUL; )
    {
	जर (curitem == (पूर्णांक)stl_वस्तुसंच_len)
	{
	    माप_प्रकार	new_len = stl_वस्तुसंच_len * 3 / 2;

	    stl_item_T *new_वस्तुसंच =
			  vim_realloc(stl_वस्तुसंच, आकार(stl_item_T) * new_len);
	    जर (new_वस्तुसंच == रिक्त)
		थांबा;
	    stl_वस्तुसंच = new_वस्तुसंच;

	    पूर्णांक *new_groupitem =
			     vim_realloc(stl_groupitem, आकार(पूर्णांक) * new_len);
	    जर (new_groupitem == रिक्त)
		थांबा;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  आकार(stl_hlrec_T) * (new_len + 1));
	    जर (new_hlrec == रिक्त)
		थांबा;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  आकार(stl_hlrec_T) * (new_len + 1));
	    जर (new_hlrec == रिक्त)
		थांबा;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      आकार(stl_clickrec_T) * (new_len + 1));
	    जर (new_clickrec == रिक्त)
		थांबा;
	    stl_clicktab = new_clickrec;

	    पूर्णांक *new_separator_locs = vim_realloc(stl_separator_locations,
					    आकार(पूर्णांक) * new_len);
	    जर (new_separator_locs == रिक्त)
		थांबा;
	    stl_separator_locations = new_separator_locs;

	    stl_वस्तुसंच_len = new_len;
	}

	stl_वस्तुसंच[curitem].stl_clickfunc = रिक्त;

	जर (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	पर्यंत (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    जर (*s == '\n' || *s == '\r')
	    {
		जर (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    जा find_linebreak;
		}
		अन्यथा जर (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	जर (*s == NUL || p + 1 >= out + outlen)
	    थांबा;

	/*
	 * Handle one '%' item.
	 */
	s++;
	जर (*s == NUL)  // ignore trailing %
	    थांबा;

	जर (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    जर (s[1] == ']')
	    {
		stl_वस्तुसंच[curitem].stl_type = ClickFunc;
		stl_वस्तुसंच[curitem].stl_start = p;
		stl_वस्तुसंच[curitem].stl_minwid = 0;
		stl_वस्तुसंच[curitem].stl_clickfunc = रिक्त;
		s += 2;
		curitem++;
		सुरूठेवा;
	    }
	    // %[FuncName] - start click region
	    जर (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		जर (rb != रिक्त)
		{
		    stl_वस्तुसंच[curitem].stl_type = ClickFunc;
		    stl_वस्तुसंच[curitem].stl_start = p;
		    stl_वस्तुसंच[curitem].stl_minwid = 0;
		    stl_वस्तुसंच[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    सुरूठेवा;
		}
	    }
	}
	जर (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    जर (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		थांबा;
	    }
	    अन्यथा जर (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	जर (*s == '%')
	{
	    जर (p + 1 >= out + outlen)
		थांबा;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    सुरूठेवा;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	जर (*s == STL_SEPARATE)
	{
	    s++;
	    जर (groupdepth > 0)
		सुरूठेवा;
	    stl_वस्तुसंच[curitem].stl_type = Separate;
	    stl_वस्तुसंच[curitem++].stl_start = p;
	    सुरूठेवा;
	}
	जर (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_वस्तुसंच[curitem].stl_type = Trunc;
	    stl_वस्तुसंच[curitem++].stl_start = p;
	    सुरूठेवा;
	}
	जर (*s == ')')
	{
	    char_u  *t;

	    s++;
	    जर (groupdepth < 1)
		सुरूठेवा;
	    groupdepth--;

	    t = stl_वस्तुसंच[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    जर (curitem > stl_groupitem[groupdepth] + 1
		    && stl_वस्तुसंच[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		पूर्णांक group_start_userhl = 0;
		पूर्णांक group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		साठी (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    जर (stl_वस्तुसंच[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_वस्तुसंच[n].stl_minwid;
			थांबा;
		    }
		}
		साठी (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    जर (stl_वस्तुसंच[n].stl_type == Normal)
			थांबा;
		    जर (stl_वस्तुसंच[n].stl_type == Highlight)
			group_end_userhl = stl_वस्तुसंच[n].stl_minwid;
		}
		जर (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    साठी (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			जर (stl_वस्तुसंच[n].stl_type == Highlight)
			    stl_वस्तुसंच[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			जर (stl_वस्तुसंच[n].stl_type == TabPage)
			    stl_वस्तुसंच[n].stl_start = p;
		    }
		}
	    }
	    जर (l > stl_वस्तुसंच[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		जर (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    पर्यंत (l >= stl_वस्तुसंच[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		अन्यथा
		    n = (दीर्घ)(p - t) - stl_वस्तुसंच[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (माप_प्रकार)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		पूर्णांक minwid_fixed = MIN(stl_वस्तुसंच[stl_groupitem[groupdepth]].stl_minwid,
				       stl_वस्तुसंच[stl_groupitem[groupdepth]].stl_maxwid);
		पर्यंत (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		साठी (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_वस्तुसंच[l].stl_start -= n - 1;
		    जर (stl_वस्तुसंच[l].stl_start < t)
			stl_वस्तुसंच[l].stl_start = t;
		}
	    }
	    अन्यथा जर (असल(stl_वस्तुसंच[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_वस्तुसंच[stl_groupitem[groupdepth]].stl_minwid;
		पूर्णांक fillchar_len = MB_CHAR2LEN(fillchar);
		जर (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    पर्यंत (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		अन्यथा
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    जर (p + l >= out + outlen)
		    {
			n = (दीर्घ)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (माप_प्रकार)(p - t));
		    p += l;
		    साठी ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    साठी (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_वस्तुसंच[n].stl_start += l;
		}
	    }
	    सुरूठेवा;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	जर (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	जर (*s == '-')
	{
	    s++;
	    l = -1;
	}
	जर (VIM_ISDIGIT(*s))
	{
	    minwid = (पूर्णांक)getdigits(&s);
	    जर (minwid < 0)	// overflow
		minwid = 0;
	}
	जर (*s == STL_USER_HL)
	{
	    stl_वस्तुसंच[curitem].stl_type = Highlight;
	    stl_वस्तुसंच[curitem].stl_start = p;
	    stl_वस्तुसंच[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    सुरूठेवा;
	}
	जर (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    जर (*s == STL_TABCLOSENR)
	    {
		जर (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    साठी (n = curitem - 1; n >= 0; --n)
			जर (stl_वस्तुसंच[n].stl_type == TabPage
					       && stl_वस्तुसंच[n].stl_minwid >= 0)
			{
			    minwid = stl_वस्तुसंच[n].stl_minwid;
			    थांबा;
			}
		}
		अन्यथा
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_वस्तुसंच[curitem].stl_type = TabPage;
	    stl_वस्तुसंच[curitem].stl_start = p;
	    stl_वस्तुसंच[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    सुरूठेवा;
	}
	जर (*s == '.')
	{
	    s++;
	    जर (VIM_ISDIGIT(*s))
	    {
		maxwid = (पूर्णांक)getdigits(&s);
		जर (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	पूर्णांक raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	जर (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_वस्तुसंच[curitem].stl_type = Group;
	    stl_वस्तुसंच[curitem].stl_start = p;
	    stl_वस्तुसंच[curitem].stl_minwid = minwid;
	    stl_वस्तुसंच[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    सुरूठेवा;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	जर (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    सुरूठेवा;
	}
# endif
	जर (vim_strchr(STL_ALL, *s) == रिक्त)
	{
	    जर (*s == NUL)  // can happen with "%0"
		थांबा;
	    s++;
	    सुरूठेवा;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	अंक = -1;
	str = रिक्त;
	निवड (opt)
	{
	स्थिती STL_FILEPATH:
	स्थिती STL_FULLPATH:
	स्थिती STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    जर (name != रिक्त)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    अन्यथा
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    जर (opt != STL_FILENAME)
		str = NameBuff;
	    अन्यथा
		str = gettail(NameBuff);
	    थांबा;
	}

	स्थिती STL_VIM_अभिव्यक्ती: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    पूर्णांक	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    जर (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    पर्यंत ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    जर (*s != '}')	// missing '}' or out of space
		थांबा;
	    s++;
	    जर (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    अन्यथा
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((अक्षर *)buf_tmp, आकार(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((अक्षर *)buf_tmp, आकार(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    जर (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    जर (!zeropad && str != रिक्त && *str != NUL)
	    {
		जर (*skipdigits(str) == NUL)
		{
		    अंक = म_से_प((अक्षर *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    जर (reevaluate && str != रिक्त && *str != NUL
		    && म_अक्षर((स्थिरांक अक्षर *)str, '%') != रिक्त
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		माप_प्रकार parsed_usefmt = (माप_प्रकार)(block_start - usefmt);
		माप_प्रकार str_length = म_माप((स्थिरांक अक्षर *)str);
		माप_प्रकार fmt_length = म_माप((स्थिरांक अक्षर *)s);
		माप_प्रकार new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * आकार(char_u));

		जर (new_fmt != रिक्त)
		{
		    char_u *new_fmt_p = new_fmt;

		    new_fmt_p = (char_u *)स_नकल(new_fmt_p, usefmt, parsed_usefmt)
								   + parsed_usefmt;
		    new_fmt_p = (char_u *)स_नकल(new_fmt_p , str, str_length)
								      + str_length;
		    new_fmt_p = (char_u *)स_नकल(new_fmt_p, "%}", 2) + 2;
		    new_fmt_p = (char_u *)स_नकल(new_fmt_p , s, fmt_length)
								      + fmt_length;
		    *new_fmt_p = 0;
		    new_fmt_p = रिक्त;

		    जर (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    सुरूठेवा;
		}
	    }
# endif
	    थांबा;
	}
	स्थिती STL_ओळ:
	    अंक = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (दीर्घ)(wp->w_cursor.lnum);
	    थांबा;

	स्थिती STL_NUMLINES:
	    अंक = wp->w_buffer->b_ml.ml_ओळ_मोजणी;
	    थांबा;

	स्थिती STL_COLUMN:
	    अंक = (State & MODE_घाला) == 0 && empty_ओळ
					       ? 0 : (पूर्णांक)wp->w_cursor.col + 1;
	    थांबा;

	स्थिती STL_VIRTCOL:
	स्थिती STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    जर (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_घाला) == 0
			       && empty_ओळ ? 0 : (पूर्णांक)wp->w_cursor.col + 1)))
		थांबा;
	    अंक = (दीर्घ)virtcol;
	    थांबा;
	}

	स्थिती STL_PERCENTAGE:
	    अंक = calc_percentage((दीर्घ)wp->w_cursor.lnum, (दीर्घ)wp->w_buffer->b_ml.ml_ओळ_मोजणी);
	    थांबा;

	स्थिती STL_ALTPERCENT:
	    str = buf_tmp;
	    (शून्य)get_rel_pos(wp, str, TMPLEN);
	    थांबा;

	स्थिती STL_SHOWCMD:
	    जर (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    थांबा;

	स्थिती STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    जर (append_arg_संख्या(wp, buf_tmp, आकार(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    थांबा;

	स्थिती STL_KEYMAP:
	    fillable = FALSE;
	    जर (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    थांबा;
	स्थिती STL_PAGENUM:
# जर defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    अंक = printer_page_अंक;
# अन्यथा
	    अंक = 0;
# endif
	    थांबा;

	स्थिती STL_BUFNO:
	    अंक = wp->w_buffer->b_fnum;
	    थांबा;

	स्थिती STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	स्थिती STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_ओळ_or_offset(wp->w_buffer, wp->w_cursor.lnum, रिक्त);
	    अंक = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_घाला) == 0 && empty_ओळ
				? 0 : (पूर्णांक)wp->w_cursor.col);
# endif
	    थांबा;

	स्थिती STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	स्थिती STL_BYTEVAL:
	    अंक = byteval;
	    जर (अंक == NL)
		अंक = 0;
	    अन्यथा जर (अंक == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		अंक = NL;
	    थांबा;

	स्थिती STL_ROFLAG:
	स्थिती STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    जर (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    थांबा;

	स्थिती STL_HELPFLAG:
	स्थिती STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    जर (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    थांबा;

	स्थिती STL_FILETYPE:
	    जर (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((अक्षर *)buf_tmp, आकार(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    थांबा;

	स्थिती STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    जर (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((अक्षर *)buf_tmp, आकार(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		साठी (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    थांबा;

# जर defined(FEAT_QUICKFIX)
	स्थिती STL_PREVIEWFLAG:
	स्थिती STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    जर (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    थांबा;

	स्थिती STL_QUICKFIX:
	    जर (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(संदेश_loclist)
			    : _(संदेश_qflist));
	    थांबा;
# endif

	स्थिती STL_MODIFIED:
	स्थिती STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    निवड ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		स्थिती 2: str = (char_u *)"[+]"; थांबा;
		स्थिती 3: str = (char_u *)",+"; थांबा;
		स्थिती 4: str = (char_u *)"[-]"; थांबा;
		स्थिती 5: str = (char_u *)",-"; थांबा;
		स्थिती 6: str = (char_u *)"[+-]"; थांबा;
		स्थिती 7: str = (char_u *)",+-"; थांबा;
	    }
	    थांबा;

	स्थिती STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		पर्यंत (*s != '#' && *s != NUL)
		    ++s;
		जर (*s == '#')
		{
		    stl_वस्तुसंच[curitem].stl_type = Highlight;
		    stl_वस्तुसंच[curitem].stl_start = p;
		    stl_वस्तुसंच[curitem].stl_minwid = -syn_namen2id(t, (पूर्णांक)(s - t));
		    curitem++;
		}
		जर (*s != NUL)
		    ++s;
		सुरूठेवा;
	    }

	स्थिती STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    जर (*s == ']')
	    {
		stl_वस्तुसंच[curitem].stl_type = ClickFunc;
		stl_वस्तुसंच[curitem].stl_start = p;
		stl_वस्तुसंच[curitem].stl_minwid = 0;
		stl_वस्तुसंच[curitem].stl_clickfunc = रिक्त;
		s++;
		curitem++;
		सुरूठेवा;
	    }
	    // %N[FuncName] with minwid
	    जर (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		जर (rb != रिक्त)
		{
		    stl_वस्तुसंच[curitem].stl_type = ClickFunc;
		    stl_वस्तुसंच[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_वस्तुसंच[curitem].stl_minwid = raw_minwid;
		    stl_वस्तुसंच[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    सुरूठेवा;
		}
	    }
	    सुरूठेवा;

	स्थिती STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    सुरूठेवा;
	}

	stl_वस्तुसंच[curitem].stl_start = p;
	stl_वस्तुसंच[curitem].stl_type = Normal;
	जर (str != रिक्त && *str)
	{
	    char_u  *t = str;

	    जर (itemisflag)
	    {
		जर ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    जर (l > 0)
		prevchar_isitem = TRUE;
	    जर (l > maxwid)
	    {
		पर्यंत (l >= maxwid)
		    जर (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    अन्यथा
			l -= byte2cells(*t++);
		जर (p + 1 >= out + outlen)
		    थांबा;
		*p++ = '<';
	    }
	    जर (minwid > 0)
	    {
		साठी (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    जर (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    अन्यथा
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    अन्यथा
		minwid *= -1;
	    साठी (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		जर (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		अन्यथा
		    *p++ = *t;
	    }
	    साठी (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	अन्यथा जर (अंक >= 0)
	{
	    पूर्णांक	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    जर (p + 20 >= out + outlen)
		थांबा;		// not sufficient space
	    prevchar_isitem = TRUE;
	    जर (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    जर (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    साठी (n = अंक, l = 1; n >= nbase; n /= nbase)
		l++;
	    जर (opt == STL_VIRTCOL_ALT)
		l++;

	    जर (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		पर्यंत (l-- > maxwid)
		    अंक /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((अक्षर *)p, outlen - (p - out),
		    (अक्षर *)nstr, 0, अंक, n);
	    }
	    अन्यथा
		p += vim_snprintf_safelen((अक्षर *)p, outlen - (p - out),
		    (अक्षर *)nstr, minwid, अंक);
	}
	अन्यथा
	    stl_वस्तुसंच[curitem].stl_type = Empty;

	जर (अंक >= 0 || (!itemisflag && str != रिक्त && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	जर (opt == STL_VIM_अभिव्यक्ती)
	    vim_free(str);
	curitem++;
    }
# ifdef ENABLE_STL_MODE_MULTI_NL
find_linebreak:
# endif
    *p = NUL;
    outputlen = (माप_प्रकार)(p - out);
    itemcnt = curitem;

    // Remember the most recent %# / %* highlight so the next row of a
    // multi-line statusline can resume it.
    जर (carry_hl != रिक्त)
    {
	पूर्णांक last_hl = 0;
	साठी (l = 0; l < itemcnt; l++)
	    जर (stl_वस्तुसंच[l].stl_type == Highlight)
		last_hl = stl_वस्तुसंच[l].stl_minwid;
	*carry_hl = last_hl;
    }

    जर (mode == STL_MODE_MULTI
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
	जर (usefmt != fmt)
	    vim_free(fmt);
# endif
	माप_प्रकार fmt_remain_len = म_माप((अक्षर *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    अन्यथा
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	जर (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    जर (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	साठी (l = 0; l < itemcnt; l++)
	    जर (stl_वस्तुसंच[l].stl_type == ClickFunc)
		vim_free(stl_वस्तुसंच[l].stl_clickfunc);
	जर (rendered_height != रिक्त)
	    *rendered_height = rheight;
	परत 0;
    }

    width = vim_strsize(out);
    जर (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	जर (itemcnt == 0)
	    s = out;
	अन्यथा
	{
	    साठी ( ; l < itemcnt; l++)
		जर (stl_वस्तुसंच[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_वस्तुसंच[l].stl_start;
		    थांबा;
		}
	    जर (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_वस्तुसंच[0].stl_start;
		l = 0;
	    }
	}

	जर (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    जर (has_mbyte)
	    {
		s = out;
		width = 0;
		साठी (;;)
		{
		    width += ptr2cells(s);
		    जर (width >= maxwidth)
			थांबा;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		पर्यंत (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    अन्यथा
		s = out + maxwidth - 1;
	    साठी (l = 0; l < itemcnt; l++)
		जर (stl_वस्तुसंच[l].stl_start > s)
		    थांबा;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	अन्यथा
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    पर्यंत (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (माप_प्रकार)(end - p) + 1);	// +1 for NUL
	    end -= (माप_प्रकार)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    साठी (; l < itemcnt; l++)
	    {
		जर (stl_वस्तुसंच[l].stl_start - n >= s)
		    stl_वस्तुसंच[l].stl_start -= n;
		अन्यथा
		    stl_वस्तुसंच[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    पर्यंत (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    अन्यथा जर (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	पूर्णांक अंक_separators = 0;

	साठी (l = 0; l < itemcnt; l++)
	{
	    जर (stl_वस्तुसंच[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[अंक_separators] = l;
		अंक_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	जर (अंक_separators)
	{
	    पूर्णांक standard_spaces;
	    पूर्णांक final_spaces;

	    standard_spaces = (maxwidth - width) / अंक_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (अंक_separators - 1);
	    साठी (l = 0; l < अंक_separators; l++)
	    {
		पूर्णांक dislocation = (l == (अंक_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_वस्तुसंच[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		साठी (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		साठी (पूर्णांक i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_वस्तुसंच[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    जर (hltab != रिक्त)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	साठी (l = 0; l < itemcnt; l++)
	{
	    जर (stl_वस्तुसंच[l].stl_type == Highlight)
	    {
		sp->start = stl_वस्तुसंच[l].stl_start;
		sp->userhl = stl_वस्तुसंच[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = रिक्त;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    जर (tabtab != रिक्त)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	साठी (l = 0; l < itemcnt; l++)
	{
	    जर (stl_वस्तुसंच[l].stl_type == TabPage)
	    {
		sp->start = stl_वस्तुसंच[l].stl_start;
		sp->userhl = stl_वस्तुसंच[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = रिक्त;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    जर (clicktab != रिक्त)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	साठी (l = 0; l < itemcnt; l++)
	{
	    जर (stl_वस्तुसंच[l].stl_type == ClickFunc)
	    {
		cp->start = stl_वस्तुसंच[l].stl_start;
		cp->funcname = stl_वस्तुसंच[l].stl_clickfunc;
		cp->minwid = stl_वस्तुसंच[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = रिक्त;
	cp->funcname = रिक्त;
    }
    अन्यथा
    {
	// Free click function names when caller doesn't need them.
	साठी (l = 0; l < itemcnt; l++)
	    जर (stl_वस्तुसंच[l].stl_type == ClickFunc)
		vim_free(stl_वस्तुसंच[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    किल्ली_Typed = save_किल्ली_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    जर (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    परत width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    पूर्णांक
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    पूर्णांक		buflen)
{
    दीर्घ	above; // number of lines above window
    दीर्घ	below; // number of lines below window

    जर (buflen < 3) // need at least 3 chars for writing
	परत 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    जर (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_ओळ_मोजणी - wp->w_botline + 1;
    जर (below <= 0)
	परत (पूर्णांक)vim_snprintf_safelen((अक्षर *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    जर (above <= 0)
	परत (पूर्णांक)vim_snprintf_safelen((अक्षर *)buf, buflen,
	    "%s", _("Top"));

    पूर्णांक perc = calc_percentage(above, above + below);
    अक्षर tmp[8];
    // localized percentage value
    vim_snprintf(tmp, आकार(tmp), _("%d%%"), perc);
    परत (पूर्णांक)vim_snprintf_safelen((अक्षर *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    स्थिर पूर्णांक
append_arg_संख्या(
    win_T	*wp,
    char_u	*buf,
    माप_प्रकार	buflen,
    पूर्णांक		add_file)	// Add "file" before the arg number
{
    जर (ARGCOUNT <= 1)		// nothing to do
	परत 0;

    अक्षर *संदेश;
    निवड ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	स्थिती 0: संदेश = _(" (%d of %d)"); थांबा;
	स्थिती 1: संदेश = _(" ((%d) of %d)"); थांबा;
	स्थिती 2: संदेश = _(" (file %d of %d)"); थांबा;
	स्थिती 3: संदेश = _(" (file (%d) of %d)"); थांबा;
    }

    परत (पूर्णांक)vim_snprintf_safelen((अक्षर *)buf, buflen, संदेश,
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
    परत FullName_save(fname, TRUE);
#अन्यथा
    जर (!vim_isAbsName(fname)
	    || म_माला((अक्षर *)fname, "..") != रिक्त
	    || म_माला((अक्षर *)fname, "//") != रिक्त
# ifdef BACKSLASH_IN_FILENAME
	    || म_माला((अक्षर *)fname, "\\\\") != रिक्त
# endif
# जर defined(MSWIN)
	    || vim_strchr(fname, '~') != रिक्त
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	परत FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    जर (fname != रिक्त)
	fname_case(fname, 0);	// set correct case for file name
# endif

    परत fname;
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
    शून्य
fname_expand(
    buf_T	*buf UNUSED,
    char_u	**ffname,
    char_u	**sfname)
{
    जर (*ffname == रिक्त)	    // no file name given, nothing to do
	परत;
    जर (*sfname == रिक्त)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    जर (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	जर (rfname != रिक्त)
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
    शून्य
ex_buffer_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    पूर्णांक		split_ret = OK;
    पूर्णांक		p_ea_save;
    पूर्णांक		open_wins = 0;
    पूर्णांक		r;
    पूर्णांक		मोजणी;		// Maximum number of windows to open.
    पूर्णांक		all;		// When TRUE also load inactive buffers.
    पूर्णांक		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    जर (eap->addr_मोजणी == 0)	// make as many windows as possible
	मोजणी = 9999;
    अन्यथा
	मोजणी = eap->line2;	// make as many windows as specified
    जर (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    अन्यथा
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
    जर (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    साठी (;;)
    {
	tpnext = curtab->tp_next;
	साठी (wp = firstwin; wp != रिक्त; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    जर ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		जर (win_close(wp, FALSE) == FAIL)
		    थांबा;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    अन्यथा
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	जर (had_tab == 0 || tpnext == रिक्त)
	    थांबा;
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
    साठी (buf = firstbuf; buf != रिक्त && open_wins < मोजणी; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	जर ((!all && buf->b_ml.ml_mfp == रिक्त) || !buf->b_p_bl)
	    सुरूठेवा;

	जर (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    जर (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    अन्यथा
		wp = रिक्त;
	}
	अन्यथा
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		जर (wp->w_buffer == buf)
		    थांबा;
	    // If the buffer already has a window, move it
	    जर (wp != रिक्त)
		win_move_after(wp, curwin);
	}

	जर (wp == रिक्त && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    जर (split_ret == FAIL)
		सुरूठेवा;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    जर (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		थांबा;
	    }
	    जर (swap_exists_action == SEA_QUIT)
	    {
#जर defined(FEAT_EVAL)
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

#जर defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    अन्यथा
		handle_swap_exists(रिक्त);
	}

	ui_breakcheck();
	जर (got_int)
	{
	    (शून्य)vgetc();	// only break the file loading, not the rest
	    थांबा;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	जर (aborting())
	    थांबा;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	जर (had_tab > 0 && tabpage_index(रिक्त) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    साठी (wp = lastwin; open_wins > मोजणी; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	जर (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	अन्यथा जर (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	अन्यथा
	{
	    wp = wp->w_prev;
	    जर (wp == रिक्त)
		थांबा;
	}
    }
}


स्थिर पूर्णांक  chk_modeline(linenr_T, पूर्णांक);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    शून्य
do_modelines(पूर्णांक flags)
{
    linenr_T	lnum;
    पूर्णांक		nmlines;
    स्थिर पूर्णांक	entered = 0;

    जर (!curbuf->b_p_ml || (nmlines = (पूर्णांक)p_mls) == 0)
	परत;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    जर (entered)
	परत;

    ++entered;
    साठी (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_ओळ_मोजणी && lnum <= nmlines;
								       ++lnum)
	जर (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    साठी (lnum = curbuf->b_ml.ml_ओळ_मोजणी; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_ओळ_मोजणी - nmlines; --lnum)
	जर (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#अन्तर्भाव "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    स्थिर पूर्णांक
chk_modeline(
    linenr_T	lnum,
    पूर्णांक		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*ओळ_end;		// point to the end of the line
    char_u	*e;
    पूर्णांक		prev;
    पूर्णांक		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    ओळ_end = s + ml_get_len(lnum);
    साठी (; *s != NUL; ++s)
    {
	जर (prev == -1 || vim_isspace(prev))
	{
	    जर ((prev != -1 && STRNCMP(s, "ex:", (माप_प्रकार)3) == 0)
		    || STRNCMP(s, "vi:", (माप_प्रकार)3) == 0)
		थांबा;
	    // Accept both "vim" and "Vim".
	    जर ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		पूर्णांक vers;

		जर (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		अन्यथा
		    e = s + 3;
		vers = getdigits(&e);
		जर (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    थांबा;
	    }
	}
	prev = *s;
    }

    जर (*s)
    {
	माप_प्रकार	len;
	char_u	*linecopy;		// local copy of any modeline found
	पूर्णांक	end;

	करा				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	पर्यंत (s[-1] != ':');

	len = (माप_प्रकार)(ओळ_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	जर (linecopy == रिक्त)
	    परत FAIL;

	ओळ_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_ढकला(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	पर्यंत (end == FALSE)
	{
	    s = skipwhite(s);
	    जर (*s == NUL)
		थांबा;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    साठी (e = s; *e != ':' && *e != NUL; ++e)
		जर (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (माप_प्रकार)(ओळ_end - (e + 1)) + 1);	// +1 for NUL
		    --ओळ_end;
		}
	    जर (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    जर (STRNCMP(s, "set ", (माप_प्रकार)4) == 0
		    || STRNCMP(s, "se ", (माप_प्रकार)3) == 0)
	    {
		जर (*e != ':')		// no terminating ':'?
		    थांबा;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    जर (*s != NUL)		// skip over an empty "::"
	    {
		पूर्णांक secure_save = secure;
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
		जर (retval == FAIL)		// stop if error found
		    थांबा;
	    }
	    s = (e == ओळ_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_काढा();
	vim_free(linecopy);
    }
    परत retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    पूर्णांक
bt_normal(buf_T *buf)
{
    परत buf != रिक्त && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    पूर्णांक
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    परत buf != रिक्त && buf->b_p_bt[0] == 'q';
#अन्यथा
    परत FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    पूर्णांक
bt_terminal(buf_T *buf UNUSED)
{
#जर defined(FEAT_TERMINAL)
    परत buf != रिक्त && buf->b_p_bt[0] == 't';
#अन्यथा
    परत FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    पूर्णांक
bt_help(buf_T *buf)
{
    परत buf != रिक्त && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    पूर्णांक
bt_prompt(buf_T *buf)
{
    परत buf != रिक्त && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#जर defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    पूर्णांक
bt_popup(buf_T *buf)
{
    परत buf != रिक्त && buf->b_p_bt != रिक्त
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    पूर्णांक
bt_nofilename(buf_T *buf)
{
    परत buf != रिक्त && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    स्थिर पूर्णांक
bt_nofileread(buf_T *buf)
{
    परत buf != रिक्त && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#जर defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    पूर्णांक
bt_nofile(buf_T *buf)
{
    परत buf != रिक्त && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    पूर्णांक
bt_dontwrite(buf_T *buf)
{
    परत buf != रिक्त && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    पूर्णांक
bt_dontwrite_संदेश(buf_T *buf)
{
    जर (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	परत TRUE;
    }
    परत FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    पूर्णांक
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    निवड (buf->b_p_bh[0])
    {
	स्थिती 'u':		    // "unload"
	स्थिती 'w':		    // "wipe"
	स्थिती 'd': परत FALSE;	    // "delete"
	स्थिती 'h': परत TRUE;	    // "hide"
    }
    परत (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#जर defined(FEAT_QUICKFIX)
    जर (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	जर (buf->b_fnum == qf_चळत_get_bufnr())
	    परत (char_u *)_(संदेश_qflist);
	अन्यथा
	    परत (char_u *)_(संदेश_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    जर (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	जर (buf->b_term != रिक्त)
	    परत term_get_status_text(buf->b_term);
#endif
	जर (buf->b_fname != रिक्त)
	    परत buf->b_fname;
	जर (buf == cmdwin_buf)
	    परत (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	जर (bt_prompt(buf))
	    परत (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	जर (bt_popup(buf))
	    परत (char_u *)_("[Popup]");
#endif
	परत (char_u *)_("[Scratch]");
    }

    जर (buf->b_fname == रिक्त)
	परत buf_get_fname(buf);
    परत रिक्त;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    जर (buf->b_fname == रिक्त)
	परत (char_u *)_("[No Name]");
    परत buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    शून्य
set_buflisted(पूर्णांक on)
{
    जर (on == curbuf->b_p_bl)
	परत;

    curbuf->b_p_bl = on;
    जर (on)
	apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, curbuf);
    अन्यथा
	apply_autocmds(EVENT_BUFDELETE, रिक्त, रिक्त, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    पूर्णांक
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    पूर्णांक		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(रिक्त, रिक्त, (linenr_T)1, BLN_DUMMY);
    जर (newbuf == रिक्त)
	परत TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    जर (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	परत TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    जर (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	परत TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    जर (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	जर (buf->b_ml.ml_ओळ_मोजणी == curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    differ = FALSE;
	    साठी (lnum = 1; lnum <= curbuf->b_ml.ml_ओळ_मोजणी; ++lnum)
		जर (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    थांबा;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    जर (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    परत differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    शून्य
wipe_buffer(
    buf_T	*buf,
    पूर्णांक		aucmd)	    // When TRUE trigger autocommands.
{
    जर (buf->b_fnum == शीर्ष_file_अंक - 1)
	--शीर्ष_file_अंक;

    जर (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(रिक्त, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    जर (!aucmd)
	unblock_autocmds();
}
