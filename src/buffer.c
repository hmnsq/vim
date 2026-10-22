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

#અન્તર્ભાવ "vim.h"


#ifdef FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_DEPTH 100
#endif

સ્થિર શૂન્ય	enter_buffer(buf_T *buf);
સ્થિર શૂન્ય	buflist_getfpos(શૂન્ય);
સ્થિર char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, પૂર્ણાંક ignore_case);
સ્થિર char_u	*fname_match(regmatch_T *rmp, char_u *name, પૂર્ણાંક ignore_case);
#ifdef UNIX
સ્થિર buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
સ્થિર પૂર્ણાંક	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
સ્થિર પૂર્ણાંક	buf_same_ino(buf_T *buf, stat_T *stp);
#નહીંતર
સ્થિર પૂર્ણાંક	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
સ્થિર પૂર્ણાંક	કિંમત_changed(char_u *str, char_u **last);
#જો defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
સ્થિર પૂર્ણાંક build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, પૂર્ણાંક opt_scope, પૂર્ણાંક fillchar, પૂર્ણાંક maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, પૂર્ણાંક *lbreaks, પૂર્ણાંક *carry_hl);
#endif
સ્થિર પૂર્ણાંક	append_arg_સંખ્યા(win_T *wp, char_u *buf, size_t buflen, પૂર્ણાંક add_file);
સ્થિર શૂન્ય	free_buffer(buf_T *);
સ્થિર શૂન્ય	free_buffer_stuff(buf_T *buf, પૂર્ણાંક free_options);
સ્થિર પૂર્ણાંક	bt_nofileread(buf_T *buf);
સ્થિર પૂર્ણાંક	do_buffer_ext(પૂર્ણાંક action, પૂર્ણાંક start, પૂર્ણાંક dir, પૂર્ણાંક ગણતરી, પૂર્ણાંક flags);

#ifdef UNIX
# define dev_T dev_t
#નહીંતર
# define dev_T unsigned
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    માટે ((buf) = lastbuf; (buf) != ખાલી; (buf) = (buf)->b_prev)

#જો defined(FEAT_QUICKFIX)
સ્થિર અક્ષર *સંદેશ_loclist = N_("[Location List]");
સ્થિર અક્ષર *સંદેશ_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
સ્થિર પૂર્ણાંક	buf_free_ગણતરી = 0;

સ્થિર પૂર્ણાંક	ટોચ_file_અંક = 1;	// highest file number
સ્થિર garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    સ્થિર શૂન્ય
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"જો exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    સ્થિર પૂર્ણાંક
calc_percentage(long part, long whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    પાછા (part > 1000000L)
	? (પૂર્ણાંક)(part / (whole / 100L))
	: (પૂર્ણાંક)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    પૂર્ણાંક
get_highest_fnum(શૂન્ય)
{
    પાછા ટોચ_file_અંક - 1;
}

/*
 * Read data from buffer for retrying.
 */
    સ્થિર પૂર્ણાંક
read_buffer(
    પૂર્ણાંક		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    પૂર્ણાંક		flags)		    // extra flags for readfile()
{
    પૂર્ણાંક		retval = OK;
    linenr_T	લીટી_ગણતરી;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    લીટી_ગણતરી = curbuf->b_ml.ml_લીટી_ગણતરી;
    retval = readfile(
	    read_stdin ? ખાલી : curbuf->b_ffname,
	    read_stdin ? ખાલી : curbuf->b_fname,
	    લીટી_ગણતરી, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    જો (retval == OK)
    {
	// Delete the binary lines.
	જ્યારે (--લીટી_ગણતરી >= 0)
	    ml_delete((linenr_T)1);
    }
    નહીંતર
    {
	// Delete the converted lines.
	જ્યારે (curbuf->b_ml.ml_લીટી_ગણતરી > લીટી_ગણતરી)
	    ml_delete(લીટી_ગણતરી);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    જો (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	જો (!readonlymode && !BUFEMPTY())
	    changed();
	નહીંતર જો (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	જો (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, ખાલી, ખાલી, FALSE,
							      curbuf, &retval);
#નહીંતર
	    apply_autocmds(EVENT_STDINREADPOST, ખાલી, ખાલી, FALSE, curbuf);
#endif
	}
    }
    પાછા retval;
}

#જો defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    શૂન્ય
buffer_ensure_loaded(buf_T *buf)
{
    જો (buf->b_ml.ml_mfp != ખાલી)
	પાછા;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    જો (curbuf == buf)
    {
	જો (swap_exists_action != SEA_READONLY)
	    swap_exists_action = SEA_NONE;
	open_buffer(FALSE, ખાલી, 0);
	aucmd_restbuf(&aco);
    }
}
#endif

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
open_buffer(
    પૂર્ણાંક		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    પૂર્ણાંક		flags_arg)	    // extra flags for readfile()
{
    પૂર્ણાંક		flags = flags_arg;
    પૂર્ણાંક		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    પૂર્ણાંક		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    જો (readonlymode && curbuf->b_ffname != ખાલી
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    જો (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    જો (curbuf->b_ml.ml_mfp != ખાલી)
		તોડો;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	જો (curbuf == ખાલી)
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
	જો (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ખાલી, curwin);
#endif
	પાછા FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    જો (curbuf->b_ml.ml_mfp != ખાલી)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    જો (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    જો (curbuf->b_ffname != ખાલી
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	પૂર્ણાંક old_સંદેશ_silent = સંદેશ_silent;
#ifdef UNIX
	પૂર્ણાંક	save_bin = curbuf->b_p_bin;
	પૂર્ણાંક perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	પૂર્ણાંક oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	જો (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	જો (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	જો (shortmess(SHM_FILEINFO))
	    સંદેશ_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	જો (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    જો (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_buffer(FALSE, eap, flags);
	}
#endif
	સંદેશ_silent = old_સંદેશ_silent;
#ifdef FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#endif
	// Help buffer is filtered.
	જો (bt_help(curbuf))
	    fix_help_buffer();
    }
    નહીંતર જો (read_stdin)
    {
	પૂર્ણાંક	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(ખાલી, ખાલી, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, ખાલી,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	જો (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    જો (curbuf->b_ml.ml_mfp != ખાલી
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    જો (curbuf->b_flags & BF_NEVERLOADED)
    {
	(શૂન્ય)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    જો ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != ખાલી)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != ખાલી)
#endif
       )
	changed();
    નહીંતર જો (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    જો (aborting())
#નહીંતર
    જો (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    જો (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, ખાલી, ખાલી, FALSE, curbuf, &retval);
#નહીંતર
    apply_autocmds(EVENT_BUFENTER, ખાલી, ખાલી, FALSE, curbuf);
#endif

    જો (retval != OK)
	પાછા retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    જો (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != ખાલી)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	જો (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    જો ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, ખાલી, ખાલી,
			FALSE, curbuf, &retval);
#નહીંતર
	    apply_autocmds(EVENT_BUFWINENTER, ખાલી, ખાલી,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    પાછા retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    શૂન્ય
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == ખાલી ? 0 : buf->b_fnum;
    bufref->br_buf_free_ગણતરી = buf_free_ગણતરી;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    પૂર્ણાંક
bufref_valid(bufref_T *bufref)
{
    પાછા bufref->br_buf_free_ગણતરી == buf_free_ગણતરી
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    પૂર્ણાંક
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	જો (bp == buf)
	    પાછા TRUE;
    પાછા FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
સ્થિર hashtab_T buf_hashtab;

    સ્થિર શૂન્ય
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((અક્ષર *)buf->b_ચાવી, કદ(buf->b_ચાવી), "%x", buf->b_fnum);
    જો (hash_add(&buf_hashtab, buf->b_ચાવી, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    સ્થિર શૂન્ય
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_ચાવી);

    જો (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    સ્થિર પૂર્ણાંક
can_unload_buffer(buf_T *buf)
{
    પૂર્ણાંક	    can_unload = !buf->b_locked;

    જો (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    જો (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		તોડો;
	    }
    }
    // Don't unload the buffer while it's still being saved
    જો (can_unload && buf->b_saving)
	can_unload = FALSE;

    જો (!can_unload)
    {
	char_u *fname = buf->b_fname != ખાલી ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != ખાલી ? fname : (char_u *)"[No Name]");
    }
    પાછા can_unload;
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
    પૂર્ણાંક
close_buffer(
    win_T	*win,
    buf_T	*buf,
    પૂર્ણાંક		action,
    પૂર્ણાંક		abort_if_last,
    પૂર્ણાંક		ignore_abort,
    પૂર્ણાંક		set_context)
{
    પૂર્ણાંક		hiding_buf;
    bufref_T	bufref;
    પૂર્ણાંક		is_curwin = (curwin != ખાલી && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    પૂર્ણાંક		unload_buf = (action != 0);
    પૂર્ણાંક		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    પૂર્ણાંક		del_buf = (action == DOBUF_DEL || wipe_buf);
    પૂર્ણાંક		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    પૂર્ણાંક		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    જો (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    નહીંતર જો (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    નહીંતર જો (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    જો (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	જો (term_job_running(buf->b_term))
	{
	    જો (wipe_buf || unload_buf)
	    {
		જો (!can_unload_buffer(buf))
		    પાછા FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    નહીંતર
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	નહીંતર જો (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	નહીંતર
	{
	    જો (del_buf || unload_buf)
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
    જો ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	પાછા FALSE;

    જો (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	જો (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    જો ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	જો (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    પાછા FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	જો (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    જાઓ aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	જો (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    જો (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		જાઓ aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    જો (abort_if_last && one_window())
		// Autocommands made this the only window.
		જાઓ aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	જો (!ignore_abort && aborting())
	    પાછા FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    જો (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    જો (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    જો (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    જો (!hiding_buf || !unload_buf)
	પાછા FALSE;

    // Always remove the buffer when there is no file name.
    જો (buf->b_ffname == ખાલી)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    જો (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	પાછા FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    જો ((win_valid || closed_popup) && win->w_buffer == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_buffer = ખાલી;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    જો (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != ખાલી || buf->b_next != ખાલી
#જો defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	જો (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    જો (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, કદ(પૂર્ણાંક), 50);
	    જો (ga_grow(&buf_reuse, 1) == OK)
		((પૂર્ણાંક *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	જો (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	નહીંતર
	    buf->b_sfname = ખાલી;
	VIM_CLEAR(buf->b_ffname);
	જો (buf->b_prev == ખાલી)
	    firstbuf = buf->b_next;
	નહીંતર
	    buf->b_prev->b_next = buf->b_next;
	જો (buf->b_next == ખાલી)
	    lastbuf = buf->b_prev;
	નહીંતર
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    નહીંતર
    {
	જો (del_buf)
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
	જો (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    પ્રતિજ્ઞા(!win_valid || !is_curwin || win == curwin);
    પાછા TRUE;
}

/*
 * Make buffer not contain a file.
 */
    શૂન્ય
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_લીટી_ગણતરી = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_start_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_start_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_start_bomb = FALSE;
    buf->b_ml.ml_mfp = ખાલી;
    buf->b_ml.ml_flags = ML_EMPTY;		// empty buffer
#ifdef FEAT_NETBEANS_INTG
    netbeans_deleted_all_લીટીઓ(buf);
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
    પૂર્ણાંક
buf_freeall(buf_T *buf, પૂર્ણાંક flags)
{
    પૂર્ણાંક		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    પૂર્ણાંક		is_curwin = (curwin != ખાલી && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    જો (buf->b_ml.ml_mfp != ખાલી)
    {
	જો (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    પાછા FALSE;
    }
    જો ((flags & BFA_DEL) && buf->b_p_bl)
    {
	જો (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    પાછા FALSE;
    }
    જો (flags & BFA_WIPE)
    {
	જો (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    પાછા FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    જો (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    જો ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	પાછા FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    જો (buf == curbuf && !is_curbuf)
	પાછા FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    જો (buf == curbuf && VIsual_active
#જો defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    જો (curwin != ખાલી && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    જો (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_લીટી_ગણતરી = 0;    // no lines in buffer
    જો ((flags & BFA_KEEP_UNDO) == 0)
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
    પાછા TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    સ્થિર શૂન્ય
free_buffer(buf_T *buf)
{
    ++buf_free_ગણતરી;
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

    જો (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    નહીંતર
    {
	vim_free(buf);
	જો (curbuf == buf)
	    curbuf = ખાલી;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    સ્થિર શૂન્ય
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_સંખ્યા;
    di->di_tv.v_lock = VAR_FIXED;
    di->di_tv.vval.v_સંખ્યા = 0;

#ifdef FEAT_EVAL
    STRCPY(buf->b_ct_di.di_ચાવી, "changedtick");
    (શૂન્ય)dict_add(buf->b_vars, di);
#endif
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    સ્થિર શૂન્ય
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    જ્યારે (buf->b_wininfo != ખાલી)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_next;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    સ્થિર શૂન્ય
free_buffer_stuff(
    buf_T	*buf,
    પૂર્ણાંક		free_options)		// free options as well
{
    જો (free_options)
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
    શૂન્ય
free_wininfo(wininfo_T *wip)
{
    જો (wip->wi_optset)
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
    શૂન્ય
goto_buffer(
    exarg_T	*eap,
    પૂર્ણાંક		start,
    પૂર્ણાંક		dir,
    પૂર્ણાંક		ગણતરી)
{
    bufref_T	old_curbuf;
    પૂર્ણાંક		save_sea = swap_exists_action;
    પૂર્ણાંક		skip_help_buf;

    પસંદગી (eap->cmdidx)
    {
	સ્થિતિ CMD_bnext:
	સ્થિતિ CMD_sbnext:
	સ્થિતિ CMD_bNext:
	સ્થિતિ CMD_bprevious:
	સ્થિતિ CMD_sbNext:
	સ્થિતિ CMD_sbprevious:
	    skip_help_buf = TRUE;
	    તોડો;
	મૂળભૂત:
	    skip_help_buf = FALSE;
	    તોડો;
    }

    set_bufref(&old_curbuf, curbuf);

    જો (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (શૂન્ય)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, ગણતરી,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    જો (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#જો defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#જો defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    નહીંતર
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    શૂન્ય
handle_swap_exists(bufref_T *old_curbuf)
{
#જો defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    જો (swap_exists_action == SEA_QUIT)
    {
#જો defined(FEAT_EVAL)
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
	જો (old_curbuf == ખાલી || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(ખાલી, ખાલી, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	નહીંતર
	    buf = old_curbuf->br_buf;
	જો (buf != ખાલી)
	{
	    પૂર્ણાંક old_સંદેશ_silent = સંદેશ_silent;

	    જો (shortmess(SHM_FILEINFO))
		સંદેશ_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    સંદેશ_silent = old_સંદેશ_silent;

#ifdef FEAT_SYN_HL
	    જો (old_tw != curbuf->b_p_tw)
		check_colorcolumn(ખાલી, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#જો defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    નહીંતર જો (swap_exists_action == SEA_RECOVER)
    {
#જો defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Recover at ATTENTION prompt.
	સંદેશ_scroll = TRUE;
	ml_recover(FALSE);
	સંદેશ_puts("\n");	// don't overwrite the last message
	cmdline_row = સંદેશ_row;
	do_modelines(0);

#જો defined(FEAT_EVAL)
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
    સ્થિર પૂર્ણાંક
empty_curbuf(
    પૂર્ણાંક close_others,
    પૂર્ણાંક forceit,
    પૂર્ણાંક action)
{
    પૂર્ણાંક	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    જો (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	પાછા FAIL;
    }

    set_bufref(&bufref, buf);
    જો (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, ખાલી, ખાલી, ખાલી, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    જો (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(ખાલી, buf, action, FALSE, FALSE, FALSE);
    જો (!close_others)
	need_fileinfo = FALSE;
    નહીંતર જો (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    પાછા retval;
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
    સ્થિર પૂર્ણાંક
do_buffer_ext(
    પૂર્ણાંક		action,
    પૂર્ણાંક		start,
    પૂર્ણાંક		dir,		// FORWARD or BACKWARD
    પૂર્ણાંક		ગણતરી,		// buffer number or number of buffers
    પૂર્ણાંક		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    પૂર્ણાંક		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    પસંદગી (start)
    {
	સ્થિતિ DOBUF_FIRST:   buf = firstbuf; તોડો;
	સ્થિતિ DOBUF_LAST:    buf = lastbuf;  તોડો;
	મૂળભૂત:	    buf = curbuf;   તોડો;
    }
    જો (start == DOBUF_MOD)	    // find next modified buffer
    {
	જ્યારે (ગણતરી-- > 0)
	{
	    કરો
	    {
		buf = buf->b_next;
		જો (buf == ખાલી)
		    buf = firstbuf;
	    }
	    જ્યારે (buf != curbuf && !bufIsChanged(buf));
	}
	જો (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    પાછા FAIL;
	}
    }
    નહીંતર જો (start == DOBUF_FIRST && ગણતરી) // find specified buffer number
    {
	જ્યારે (buf != ખાલી && buf->b_fnum != ગણતરી)
	    buf = buf->b_next;
    }
    નહીંતર
    {
	પૂર્ણાંક help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = ખાલી;
	જ્યારે (ગણતરી > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    જો (bp == ખાલી)
		bp = buf;
	    જો (dir == FORWARD)
	    {
		buf = buf->b_next;
		જો (buf == ખાલી)
		    buf = firstbuf;
	    }
	    નહીંતર
	    {
		buf = buf->b_prev;
		જો (buf == ખાલી)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    જો (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--ગણતરી;
		bp = ખાલી;	// use this buffer as new starting point
	    }
	    જો (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		પાછા FAIL;
	    }
	}
    }

    જો (buf == ખાલી)	    // could not find it
    {
	જો (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    જો (!unload)
		semsg(_(e_buffer_nr_does_not_exist), ગણતરી);
	}
	નહીંતર જો (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	નહીંતર
	    emsg(_(e_cannot_go_before_first_buffer));
	પાછા FAIL;
    }
#ifdef FEAT_PROP_POPUP
    જો ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	પાછા OK;
#endif
    જો (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	પાછા FAIL;

    જો ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), ગણતરી);
	પાછા FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    જો (unload)
    {
	પૂર્ણાંક	forward;
	bufref_T bufref;

	જો (!can_unload_buffer(buf))
	    પાછા FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	જો (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == ખાલી && !buf->b_p_bl)
	    પાછા FAIL;

	જો ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    જો ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		જો (term_job_running(buf->b_term))
		{
		    જો (term_confirm_stop(buf) == FAIL)
			પાછા FAIL;
		}
		નહીંતર
# endif
		{
		    dialog_changed(buf, FALSE);
		    જો (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			પાછા FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    જો (bufIsChanged(buf))
			પાછા FAIL;
		}
	    }
	    નહીંતર
#endif
	    {
		no_write_message_buf(buf);
		પાછા FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	જો (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    જો (bp->b_p_bl && bp != buf)
		તોડો;
	જો (bp == ખાલી && buf == curbuf)
	    પાછા empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	જ્યારે (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != ખાલી))
	{
	    જો (win_close(curwin, FALSE) == FAIL)
		તોડો;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	જો (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    જો (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(ખાલી, buf, action, FALSE, FALSE, FALSE);
	    પાછા OK;
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
	buf = ખાલી;	// selected buffer
	bp = ખાલી;	// used when no loaded buffer found
	જો (au_new_curbuf.br_buf != ખાલી && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	નહીંતર જો (curwin->w_jumplistlen > 0)
	{
	    પૂર્ણાંક     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    જો (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    જ્યારે (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		જો (buf != ખાલી)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    જો (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = ખાલી;
		    નહીંતર જો (buf->b_ml.ml_mfp == ખાલી)
		    {
			// skip unloaded buf, but may keep it for later
			જો (bp == ખાલી)
			    bp = buf;
			buf = ખાલી;
		    }
		}
		જો (buf != ખાલી)   // found a valid buffer: stop searching
		    તોડો;
		// advance to older entry in jump list
		જો (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    તોડો;
		જો (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		જો (jumpidx == forward)		// List exhausted for sure
		    તોડો;
	    }
	}

	જો (buf == ખાલી)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    માટે (;;)
	    {
		જો (buf == ખાલી)
		{
		    જો (!forward)	// tried both directions
			તોડો;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    ચાલુ;
		}
		// in non-help buffer, try to skip help buffers, and vv
		જો (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    જો (buf->b_ml.ml_mfp != ખાલી)   // found loaded buffer
			તોડો;
		    જો (bp == ખાલી)	// remember unloaded buf for later
			bp = buf;
		}
		જો (forward)
		    buf = buf->b_next;
		નહીંતર
		    buf = buf->b_prev;
	    }
	}
	જો (buf == ખાલી)	// No loaded buffer, use unloaded one
	    buf = bp;
	જો (buf == ખાલી)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		જો (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    તોડો;
	}
	જો (buf == ખાલી)	// Still no buffer, just take one
	{
	    જો (curbuf->b_next != ખાલી)
		buf = curbuf->b_next;
	    નહીંતર
		buf = curbuf->b_prev;
	    જો (bt_quickfix(buf)
		    || (buf != ખાલી && buf != curbuf && buf->b_locked_split))
		buf = ખાલી;
	}
    }

    જો (buf == ખાલી)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	પાછા empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    જો (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != ખાલી)
	પાછા OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    જો (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	પાછા FAIL;
    }
    જો (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	પાછા FAIL;

    // go to current buffer - nothing to do
    જો (buf == curbuf)
	પાછા OK;

    // Check if the current buffer may be abandoned.
    જો (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	જો ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    જો (term_job_running(curbuf->b_term))
	    {
		જો (term_confirm_stop(curbuf) == FAIL)
		    પાછા FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    નહીંતર
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		જો (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    પાછા FAIL;

		જો (bufIsChanged(curbuf))
		{
		    no_write_message();
		    પાછા FAIL;
		}
	    }
	}
	નહીંતર
#endif
	{
	    no_write_message();
	    પાછા FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    જો (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#જો defined(FEAT_EVAL)
    જો (aborting())	    // autocmds may abort script processing
	પાછા FAIL;
#endif

    પાછા OK;
}

    પૂર્ણાંક
do_buffer(
    પૂર્ણાંક		action,
    પૂર્ણાંક		start,
    પૂર્ણાંક		dir,		// FORWARD or BACKWARD
    પૂર્ણાંક		ગણતરી,		// buffer number or number of buffers
    પૂર્ણાંક		forceit)	// TRUE when using !
{
    પાછા do_buffer_ext(action, start, dir, ગણતરી,
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
    અક્ષર *
do_bufdel(
    પૂર્ણાંક		command,
    char_u	*arg,		// pointer to extra arguments
    પૂર્ણાંક		addr_ગણતરી,
    પૂર્ણાંક		start_bnr,	// first buffer number in a range
    પૂર્ણાંક		end_bnr,	// buffer nr or last buffer nr in a range
    પૂર્ણાંક		forceit)
{
    પૂર્ણાંક		do_current = 0;	// delete current buffer?
    પૂર્ણાંક		deleted = 0;	// number of buffers deleted
    અક્ષર	*errormsg = ખાલી; // return value
    પૂર્ણાંક		bnr;		// buffer number
    char_u	*p;

    જો (addr_ગણતરી == 0)
    {
	(શૂન્ય)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    નહીંતર
    {
	જો (addr_ગણતરી == 2)
	{
	    જો (*arg)		// both range and argument is not allowed
		પાછા ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	નહીંતર	// addr_count == 1
	    bnr = end_bnr;

	માટે ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    જો (bnr == curbuf->b_fnum)
		do_current = bnr;
	    નહીંતર જો (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    જો (addr_ગણતરી == 2)
	    {
		જો (++bnr > end_bnr)
		    તોડો;
	    }
	    નહીંતર    // addr_count == 1
	    {
		arg = skipwhite(arg);
		જો (*arg == NUL)
		    તોડો;
		જો (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    જો (bnr < 0)	    // failed
			તોડો;
		    arg = p;
		}
		નહીંતર
		    bnr = getdigits(&arg);
	    }
	}
	જો (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	જો (deleted == 0)
	{
	    જો (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    નહીંતર જો (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    નહીંતર
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (અક્ષર *)IObuff;
	}
	નહીંતર જો (deleted >= p_report)
	{
	    જો (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    નહીંતર જો (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    નહીંતર
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    પાછા errormsg;
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
    શૂન્ય
set_curbuf(buf_T *buf, પૂર્ણાંક action)
{
    buf_T	*prevbuf;
    પૂર્ણાંક		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    પૂર્ણાંક		valid;

    setpcmark();
    જો ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    જો (!apply_autocmds(EVENT_BUFLEAVE, ખાલી, ખાલી, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	જો (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	જો (unload)
	    close_windows(prevbuf, FALSE);
#જો defined(FEAT_EVAL)
	જો (bufref_valid(&prevbufref) && !aborting())
#નહીંતર
	જો (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    જો (prevbuf == curbuf
		    && ((State & MODE_ઉમેરો) == 0 || curbuf->b_nwindows <= 1))
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
    જો ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == ખાલી)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	જો (!valid)
	    enter_buffer(lastbuf);
	નહીંતર
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	જો (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ખાલી, curwin);
#endif
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    સ્થિર શૂન્ય
enter_buffer(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    જો (VIsual_active
#જો defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    જો (curwin->w_buffer != ખાલી)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    જો (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    નહીંતર
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    જો (curwin->w_p_diff)
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
    જો (curbuf->b_ml.ml_mfp == ખાલી)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	જો (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, ખાલી, 0);
    }
    નહીંતર
    {
	જો (!સંદેશ_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(શૂન્ય)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
	apply_autocmds(EVENT_BUFENTER, ખાલી, ખાલી, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ખાલી, ખાલી, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    જો (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    જો (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    જો (curbuf->b_kmap_state & KEYMAP_INIT)
	(શૂન્ય)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    જો (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(શૂન્ય)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#જો defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    શૂન્ય
do_autochdir(શૂન્ય)
{
    જો ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != ખાલી
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	last_chdir_reason = "autochdir";
    }
}
#endif

    શૂન્ય
no_write_message_buf(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    જો (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    નહીંતર
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    શૂન્ય
no_write_message(શૂન્ય)
{
#ifdef FEAT_TERMINAL
    જો (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    નહીંતર
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    શૂન્ય
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    જો (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    નહીંતર
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
    પૂર્ણાંક
curbuf_reusable(શૂન્ય)
{
    પાછા (curbuf != ખાલી
	&& curbuf->b_ffname == ખાલી
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == ખાલી || BUFEMPTY())
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
    પૂર્ણાંક		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    જો (ટોચ_file_અંક == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    જો (sfname == ખાલી || mch_stat((અક્ષર *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    જો (ffname != ખાલી && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#નહીંતર
		buflist_findname(ffname)
#endif
		) != ખાલી)
    {
	vim_free(ffname);
	જો (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? ખાલી : curwin,
						      lnum, (colnr_T)0, FALSE);

	જો ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	જો ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    જો (!(flags & BLN_DUMMY))
	    {
		જો (apply_autocmds(EVENT_BUFADD, ખાલી, ખાલી, FALSE, buf)
			&& !bufref_valid(&bufref))
		    પાછા ખાલી;
	    }
	}
	પાછા buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = ખાલી;
    જો ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	જો (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    પાછા ખાલી;
	}
#endif
	જો (!bufref_valid(&bufref))
	    buf = ખાલી;		// buf was deleted; allocate a new buffer
    }
    જો (buf != curbuf || curbuf == ખાલી)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	જો (buf == ખાલી)
	{
	    vim_free(ffname);
	    પાછા ખાલી;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	જો (buf->b_vars == ખાલી)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    પાછા ખાલી;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    જો (ffname != ખાલી)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    જો ((ffname != ખાલી && (buf->b_ffname == ખાલી || buf->b_sfname == ખાલી))
	    || buf->b_wininfo == ખાલી)
    {
	જો (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	નહીંતર
	    buf->b_sfname = ખાલી;
	VIM_CLEAR(buf->b_ffname);
	જો (buf != curbuf)
	    free_buffer(buf);
	પાછા ખાલી;
    }

    જો (buf == curbuf)
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
    નહીંતર
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = ખાલી;
	જો (firstbuf == ખાલી)		// buffer list is empty
	{
	    buf->b_prev = ખાલી;
	    firstbuf = buf;
	}
	નહીંતર				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	જો ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((પૂર્ણાંક *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    જ્યારે (buf->b_prev != ખાલી && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		જો (prev->b_next != ખાલી)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		જો (buf->b_prev != ખાલી)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		જો (lastbuf == buf)
		    lastbuf = prev;
		જો (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	નહીંતર
	    buf->b_fnum = ટોચ_file_અંક++;
	જો (ટોચ_file_અંક < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    જો (emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    ટોચ_file_અંક = 1;
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
    જો (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    નહીંતર
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    જો (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    જો (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	જો (apply_autocmds(EVENT_BUFNEW, ખાલી, ખાલી, FALSE, buf)
		&& !bufref_valid(&bufref))
	    પાછા ખાલી;
	જો (flags & BLN_LISTED)
	{
	    જો (apply_autocmds(EVENT_BUFADD, ખાલી, ખાલી, FALSE, buf)
		    && !bufref_valid(&bufref))
		પાછા ખાલી;
	}
#ifdef FEAT_EVAL
	જો (aborting())		// autocmds may abort script processing
	    પાછા ખાલી;
#endif
    }

    પાછા buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    શૂન્ય
free_buf_options(
    buf_T	*buf,
    પૂર્ણાંક		free_p_ff)
{
    જો (free_p_ff)
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
#જો defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#જો defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#જો defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#જો defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    જો (buf->b_p_ચાવી != ખાલી && *buf->b_p_ચાવી != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_ચાવી, STRLEN(buf->b_p_ચાવી));
# endif
    clear_string_option(&buf->b_p_ચાવી);
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
    buf->b_s.b_cap_prog = ખાલી;
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
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_ગણતરી);
    buf->b_p_cpt_ગણતરી = 0;
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
    પૂર્ણાંક
buflist_getfile(
    પૂર્ણાંક		n,
    linenr_T	lnum,
    પૂર્ણાંક		options,
    પૂર્ણાંક		forceit)
{
    buf_T	*buf;
    win_T	*wp = ખાલી;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    જો (buf == ખાલી)
    {
	જો ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	નહીંતર
	    semsg(_(e_buffer_nr_not_found), n);
	પાછા FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    જો (buf == curbuf)
	પાછા OK;

    જો (text_or_buf_locked())
	પાછા FAIL;

    // altfpos may be changed by getfile(), get it now
    જો (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    નહીંતર
	col = 0;

    જો (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	જો (wp == ખાલી && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    જો (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    નહીંતર જો (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		પાછા FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    પૂર્ણાંક retval = FAIL;
    જો (GETFILE_SUCCESS(getfile(buf->b_fnum, ખાલી, ખાલી,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	જો (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    જો (RedrawingDisabled > 0)
	--RedrawingDisabled;
    પાછા retval;
}

/*
 * go to the last know line number for the current buffer
 */
    સ્થિર શૂન્ય
buflist_getfpos(શૂન્ય)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    જો (p_sol)
	curwin->w_cursor.col = 0;
    નહીંતર
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
    buf_T	*buf = ખાલી;

    // First make the name into a full path name
    ffname = FullName_save(fname,
#ifdef UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#નહીંતર
	    FALSE
#endif
	    );
    જો (ffname != ખાલી)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    પાછા buf;
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

    જો (mch_stat((અક્ષર *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    પાછા buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    સ્થિર buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#endif
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_LAST(buf)
	જો ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    પાછા buf;
    પાછા ખાલી;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    પૂર્ણાંક
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    પૂર્ણાંક		unlisted,	// find unlisted buffers
    પૂર્ણાંક		diffmode UNUSED, // find diff-mode buffers only
    પૂર્ણાંક		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    પૂર્ણાંક		match = -1;
    પૂર્ણાંક		find_listed;
    char_u	*pat;
    char_u	*patend;
    પૂર્ણાંક		attempt;
    char_u	*p;
    પૂર્ણાંક		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    જો ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	જો (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	નહીંતર
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	જો (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
    નહીંતર
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, ખાલી, FALSE);
	જો (pat == ખાલી)
	    પાછા -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	માટે (;;)
	{
	    માટે (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		જો (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		જો (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    જો (regmatch.regprog == ખાલી)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			પાછા -1;
		    }
		    જો (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != ખાલી)
		    {
			જો (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				જો (wp->w_buffer == buf)
				    તોડો;
			    જો (wp == ખાલી)
				ચાલુ;
			}
			જો (match >= 0)		// already found a match
			{
			    match = -2;
			    તોડો;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		જો (match >= 0)			// found one match
		    તોડો;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    જો (!unlisted || !find_listed || match != -1)
		તોડો;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    જો (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    નહીંતર જો (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    પાછા match;
}

#ifdef FEAT_VIMINFO
પ્રકારવ્યાખ્યા સંરચના {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    પૂર્ણાંક
ExpandBufnames(
    char_u	*pat,
    પૂર્ણાંક		*અંક_file,
    char_u	***file,
    પૂર્ણાંક		options)
{
    પૂર્ણાંક		ગણતરી;
    buf_T	*buf;
    પૂર્ણાંક		round;
    char_u	*p;
    char_u	*patc = ખાલી;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = ખાલી;
#endif
    પૂર્ણાંક		fuzzy;
    fuzmatch_str_T  *fuzmatch = ખાલી;
    regmatch_T	regmatch;
    પૂર્ણાંક		score = 0;
    પૂર્ણાંક		to_free = FALSE;

    *અંક_file = 0;		    // return values in case of FAIL
    *file = ખાલી;

#ifdef FEAT_DIFF
    જો ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	પાછા FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    જો (!fuzzy)
    {
	જો (*pat == '^' && pat[1] != NUL)
	{
	    પૂર્ણાંક  len = (પૂર્ણાંક)STRLEN(pat);
	    patc = alloc(len);
	    જો (patc == ખાલી)
		પાછા FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	નહીંતર જો (*pat == '^')
	    patc = (char_u *)"";
	નહીંતર
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    માટે (round = 1; round <= 2; ++round)
    {
	ગણતરી = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    જો (!buf->b_p_bl)	// skip unlisted buffers
		ચાલુ;
#ifdef FEAT_DIFF
	    જો (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		જો (buf == curbuf || !diff_mode_buf(buf))
		    ચાલુ;
#endif

	    જો (!fuzzy)
	    {
		જો (regmatch.regprog == ખાલી)
		{
		    // invalid pattern, possibly after recompiling
		    જો (to_free)
			vim_free(patc);
		    પાછા FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    નહીંતર
	    {
		p = ખાલી;
		// first try matching with the short file name
		જો ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		જો (p == ખાલી)
		{
		    // next try matching with the full path file name
		    જો ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    જો (p == ખાલી)
		ચાલુ;

	    જો (round == 1)
	    {
		++ગણતરી;
		ચાલુ;
	    }

	    જો (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    નહીંતર
		p = vim_strsave(p);
	    જો (p == ખાલી)
	    {
		જો (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, ગણતરી);
		પાછા FAIL;
	    }

	    જો (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		જો (matches != ખાલી)
		{
		    matches[ગણતરી].buf = buf;
		    matches[ગણતરી].match = p;
		    ગણતરી++;
		}
		નહીંતર
#endif
		    (*file)[ગણતરી++] = p;
	    }
	    નહીંતર
	    {
		fuzmatch[ગણતરી].idx = ગણતરી;
		fuzmatch[ગણતરી].str = p;
		fuzmatch[ગણતરી].score = score;
		ગણતરી++;
	    }
	}
	જો (ગણતરી == 0)	// no match found, break here
	    તોડો;
	જો (round == 1)
	{
	    જો (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, ગણતરી);
		જો (*file == ખાલી)
		{
		    vim_regfree(regmatch.regprog);
		    જો (to_free)
			vim_free(patc);
		    પાછા FAIL;
		}
#ifdef FEAT_VIMINFO
		જો (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, ગણતરી);
#endif
	    }
	    નહીંતર
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, ગણતરી);
		જો (fuzmatch == ખાલી)
		{
		    *અંક_file = 0;
		    *file = ખાલી;
		    પાછા FAIL;
		}
	    }
	}
    }

    જો (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	જો (to_free)
	    vim_free(patc);
    }

    જો (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	જો (matches != ખાલી)
	{
	    પૂર્ણાંક i;
	    જો (ગણતરી > 1)
		qsort(matches, ગણતરી, કદ(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    જો (matches[0].buf == curbuf)
	    {
		માટે (i = 1; i < ગણતરી; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[ગણતરી-1] = matches[0].match;
	    }
	    નહીંતર
	    {
		માટે (i = 0; i < ગણતરી; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    નહીંતર
    {
	જો (fuzzymatches_to_strmatches(fuzmatch, file, ગણતરી, FALSE) == FAIL)
	    પાછા FAIL;
    }

    *અંક_file = ગણતરી;
    પાછા (ગણતરી == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    સ્થિર char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    પૂર્ણાંક		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    જો (match == ખાલી && rmp->regprog != ખાલી)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    પાછા match;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    સ્થિર char_u *
fname_match(
    regmatch_T	*rmp,
    char_u	*name,
    પૂર્ણાંક		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = ખાલી;
    char_u	*p;

    // extra check for valid arguments
    જો (name == ખાલી || rmp->regprog == ખાલી)
	પાછા ખાલી;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    જો (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    નહીંતર જો (rmp->regprog != ખાલી)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(ખાલી, name);
	જો (p != ખાલી && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    પાછા match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(પૂર્ણાંક nr)
{
    char_u	ચાવી[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    જો (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((અક્ષર *)ચાવી, કદ(ચાવી), "%x", nr);
    hi = hash_find(&buf_hashtab, ચાવી);

    જો (!HASHITEM_EMPTY(hi))
	પાછા (buf_T *)(hi->hi_ચાવી
			     - ((unsigned)(curbuf->b_ચાવી - (char_u *)curbuf)));
    પાછા ખાલી;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    પૂર્ણાંક		n,
    પૂર્ણાંક		fullname,
    પૂર્ણાંક		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    જો (buf == ખાલી)
	પાછા ખાલી;
    પાછા home_replace_save(helptail ? buf : ખાલી,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    શૂન્ય
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	col,
    પૂર્ણાંક		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	જો (wip->wi_win == win)
	    તોડો;
    જો (wip == ખાલી)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	જો (wip == ખાલી)
	    પાછા;
	wip->wi_win = win;
	જો (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    નહીંતર
    {
	// remove the entry from the list
	જો (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	નહીંતર
	    buf->b_wininfo = wip->wi_next;
	જો (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	જો (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    જો (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    જો (win != ખાલી)
	wip->wi_changelistidx = win->w_changelistidx;
    જો (copy_options && win != ખાલી)
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
    wip->wi_prev = ખાલી;
    જો (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    સ્થિર પૂર્ણાંક
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    જો (!wip->wi_opt.wo_diff)
	પાછા FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	જો (wip->wi_win == wp)
	    પાછા FALSE;
    પાછા TRUE;
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
    સ્થિર wininfo_T *
find_wininfo(
    buf_T	*buf,
    પૂર્ણાંક		need_options,
    પૂર્ણાંક		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	જો (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    તોડો;

    જો (wip != ખાલી)
	પાછા wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    જો (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    જો (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != ખાલી
			    && wip->wi_win->w_buffer == buf)))
		તોડો;
    }
    નહીંતર
#endif
	wip = buf->b_wininfo;
    પાછા wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    શૂન્ય
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
    clearFolding(curwin);
#endif

    wip = find_wininfo(buf, TRUE, TRUE);
    જો (wip != ખાલી && wip->wi_win != ખાલી
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
    નહીંતર જો (wip != ખાલી && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    નહીંતર
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    જો (wip != ખાલી)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    જો (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    જો (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    જો (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	જો (p_ea)
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
    સ્થિર pos_T no_position = {1, 0, 0};

    wip = find_wininfo(buf, FALSE, FALSE);
    જો (wip != ખાલી)
	પાછા &(wip->wi_fpos);
    નહીંતર
	પાછા &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    પાછા buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    શૂન્ય
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;
    પૂર્ણાંક		ro_char;
    પૂર્ણાંક		changed_char;
#ifdef FEAT_TERMINAL
    પૂર્ણાંક		job_running;
    પૂર્ણાંક		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = ખાલી, **p;

    જો (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, કદ(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    જો (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_data, (size_t)buflist.ga_len,
		કદ(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    માટે (; buf != ખાલી && !got_int; buf = buflist_data != ખાલી
	    ? (++p < buflist_data + buflist.ga_len ? *p : ખાલી)
	    : buf->b_next)
#નહીંતર
    માટે (buf = firstbuf; buf != ખાલી && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	જો ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !bufIsChanged(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == ખાલી || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == ખાલી || buf->b_nwindows != 0))
#ifdef FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!job_running || (job_running && job_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!job_running || (job_running && !job_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (job_running || buf->b_term == ખાલી))
#endif
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    ચાલુ;
	name = buf_spname(buf);
	જો (name != ખાલી)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	નહીંતર
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	જો (message_filtered(NameBuff))
	    ચાલુ;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	જો (job_running)
	{
	    જો (job_none_open)
		ro_char = '?';
	    નહીંતર
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	નહીંતર જો (buf->b_term != ખાલી)
	    ro_char = 'F';
	નહીંતર
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	સંદેશ_putchar('\n');
	len = (પૂર્ણાંક)vim_snprintf_safelen((અક્ષર *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == ખાલી ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		changed_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	કરો
	    IObuff[len++] = ' ';
	જ્યારે (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	જો (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (size_t)(IOSIZE - len), buf->b_last_used);
	નહીંતર
#endif
	    vim_snprintf((અક્ષર *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (long)buflist_findlnum(buf));
	સંદેશ_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    જો (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    પૂર્ણાંક
buflist_name_nr(
    પૂર્ણાંક		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    જો (buf == ખાલી || buf->b_fname == ખાલી)
	પાછા FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    પાછા OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    પૂર્ણાંક
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    પૂર્ણાંક		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = ખાલી;
#ifdef UNIX
    stat_T	st;
#endif

    જો (ffname == ખાલી || *ffname == NUL)
    {
	// Removing the name.
	જો (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	નહીંતર
	    buf->b_sfname = ખાલી;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    નહીંતર
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	જો (ffname == ખાલી)		    // out of memory
	    પાછા FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	જો (mch_stat((અક્ષર *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	જો (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#નહીંતર
	    obuf = buflist_findname(ffname);
#endif
	જો (obuf != ખાલી && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    પૂર્ણાંક		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		જો (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    જો (obuf->b_ml.ml_mfp != ખાલી || in_use)
	    {
		જો (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		પાછા FAIL;
	    }
	    // delete from the list
	    close_buffer(ખાલી, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	જો (ffname == ખાલી || sfname == ખાલી)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    પાછા FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	જો (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    જો (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    નહીંતર
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    પાછા OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    શૂન્ય
buf_set_name(પૂર્ણાંક fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    જો (buf == ખાલી)
	પાછા;

    જો (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = ખાલી;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    શૂન્ય
buf_name_changed(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    જો (buf->b_ml.ml_mfp != ખાલી)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    જો (buf->b_term != ખાલી)
	term_clear_status_text(buf->b_term);
#endif

    જો (curwin->w_buffer == buf)
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
    જો (buf != ખાલી && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    પાછા buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    પૂર્ણાંક		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    જો (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	જો (errmsg)
	    emsg(_(e_no_alternate_file));
	પાછા ખાલી;
    }
    પાછા fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    પૂર્ણાંક
buflist_add(char_u *fname, પૂર્ણાંક flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, ખાલી, (linenr_T)0, flags);
    જો (buf != ખાલી)
	પાછા buf->b_fnum;
    પાછા 0;
}

#જો defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    શૂન્ય
buflist_slash_adjust(શૂન્ય)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	જો (bp->b_ffname != ખાલી)
	    slash_adjust(bp->b_ffname);
	જો (bp->b_sfname != ખાલી)
	    slash_adjust(bp->b_sfname);
    }
}
#endif

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    શૂન્ય
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.col, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    પૂર્ણાંક
otherfile(char_u *ffname)
{
    પાછા otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , ખાલી
#endif
	    );
}

    સ્થિર પૂર્ણાંક
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    જો (ffname == ખાલી || *ffname == NUL || buf->b_ffname == ખાલી)
	પાછા TRUE;
    જો (fnamecmp(ffname, buf->b_ffname) == 0)
	પાછા FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	જો (stp == ખાલી)
	{
	    જો (!buf->b_dev_valid || mch_stat((અક્ષર *)ffname, &st) < 0)
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
	જો (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    જો (buf_same_ino(buf, stp))
		પાછા FALSE;
	}
    }
#endif
    પાછા TRUE;
}

#જો defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    શૂન્ય
buf_setino(buf_T *buf)
{
    stat_T	st;

    જો (buf->b_fname != ખાલી && mch_stat((અક્ષર *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    નહીંતર
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    સ્થિર પૂર્ણાંક
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    પાછા (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    શૂન્ય
fileinfo(
    પૂર્ણાંક fullname,	    // when non-zero print full path
    પૂર્ણાંક shorthelp,
    પૂર્ણાંક	dont_truncate)
{
    char_u	*name;
    અક્ષર	*buffer;
    size_t	bufferlen = 0;

    buffer = alloc(IOSIZE);
    જો (buffer == ખાલી)
	પાછા;

    જો (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    જો (name != ખાલી)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    નહીંતર
    {
	જો (!fullname && curbuf->b_fname != ખાલી)
	    name = curbuf->b_fname;
	નહીંતર
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : ખાલી, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (પૂર્ણાંક)bufferlen, TRUE);
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

    જો (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_લીટીઓ_સંદેશ));
    નહીંતર જો (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_લીટી_ગણતરી),
	    (long)curbuf->b_ml.ml_લીટી_ગણતરી,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_લીટી_ગણતરી));
    નહીંતર
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (long)curwin->w_cursor.lnum,
	    (long)curbuf->b_ml.ml_લીટી_ગણતરી,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_લીટી_ગણતરી));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (પૂર્ણાંક)curwin->w_cursor.col + 1, (પૂર્ણાંક)curwin->w_virtcol + 1);
    }

    (શૂન્ય)append_arg_સંખ્યા(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    જો (dont_truncate)
    {
	પૂર્ણાંક	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	સંદેશ_start();
	n = સંદેશ_scroll;
	સંદેશ_scroll = TRUE;
	સંદેશ(buffer);
	સંદેશ_scroll = n;
    }
    નહીંતર
    {
	અક્ષર	*p = સંદેશ_trunc_attr(buffer, FALSE, 0);
	જો (restart_edit != 0 || (સંદેશ_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_સંદેશ((char_u *)p, 0);
    }

    vim_free(buffer);
}

    પૂર્ણાંક
col_print(
    char_u  *buf,
    size_t  buflen,
    પૂર્ણાંક	    col,
    પૂર્ણાંક	    vcol)
{
    જો (col == vcol)
	પાછા (પૂર્ણાંક)vim_snprintf_safelen((અક્ષર *)buf, buflen, "%d", col);

    પાછા (પૂર્ણાંક)vim_snprintf_safelen((અક્ષર *)buf, buflen, "%d-%d", col, vcol);
}

સ્થિર char_u *lasttitle = ખાલી;
સ્થિર char_u *lasticon = ખાલી;

/*
 * Put the file name in the title bar and icon of the window.
 */
    શૂન્ય
maketitle(શૂન્ય)
{
    char_u	*title_str = ખાલી;
    char_u	*icon_str = ખાલી;
    પૂર્ણાંક		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    જો (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	પાછા;
    }

    need_maketitle = FALSE;
    જો (!p_title && !p_icon && lasttitle == ખાલી && lasticon == ખાલી)
	પાછા;  // nothing to do

    જો (p_title)
    {
	પૂર્ણાંક maxlen = 0;

	જો (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    જો (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	જો (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    જો (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, કદ(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, ખાલી, ખાલી, ખાલી);
	    નહીંતર
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	નહીંતર
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (કદ(buf) - 100)
#define SPACE_FOR_DIR   (કદ(buf) - 20)
#define SPACE_FOR_ARGNR (કદ(buf) - 10)  // at least room for " - VIM"

	    // file name
	    જો (curbuf->b_fname == ખાલી)
		buflen = vim_snprintf_safelen((અક્ષર *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    નહીંતર જો (curbuf->b_term != ખાલી)
		buflen = vim_snprintf_safelen((અક્ષર *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    નહીંતર
	    {
		buflen = vim_snprintf_safelen((અક્ષર *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != ખાલી)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    જો (curbuf->b_term == ખાલી)
#endif
	    {
		પસંદગી (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    સ્થિતિ 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (અક્ષર *)buf + buflen,
			    કદ(buf) - buflen, " +");
			તોડો;
		    સ્થિતિ 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (અક્ષર *)buf + buflen,
			    કદ(buf) - buflen, " =");
			તોડો;
		    સ્થિતિ 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (અક્ષર *)buf + buflen,
			    કદ(buf) - buflen, " =+");
			તોડો;
		    સ્થિતિ 4:
		    સ્થિતિ 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (અક્ષર *)buf + buflen,
			    કદ(buf) - buflen, " -");
			તોડો;
		    સ્થિતિ 5:
		    સ્થિતિ 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (અક્ષર *)buf + buflen,
			    કદ(buf) - buflen, " -+");
			તોડો;
		    મૂળભૂત:
			તોડો;
		}
	    }

	    // path (surrounded by '()')
	    જો (curbuf->b_fname != ખાલી
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == ખાલી
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((અક્ષર *)buf + buflen,
		    કદ(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (પૂર્ણાંક)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		જો (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		જો (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((અક્ષર *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		નહીંતર
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    જો (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((અક્ષર *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != ખાલી)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    નહીંતર
			buflen += vim_snprintf_safelen((અક્ષર *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_સંખ્યા(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((અક્ષર *)buf + buflen,
		કદ(buf) - buflen, " - %s",
#જો defined(FEAT_CLIENTSERVER)
		(serverName != ખાલી)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    જો (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		જો (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, કદ(buf));
	    }
	}
    }
    mustset = કિંમત_changed(title_str, &lasttitle);

    જો (p_icon)
    {
	icon_str = buf;
	જો (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    જો (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, કદ(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, ખાલી, ખાલી,
				 ખાલી);
	    નહીંતર
#endif
		icon_str = p_iconstring;
	}
	નહીંતર
	{
	    char_u  *name;
	    પૂર્ણાંક	    namelen;

	    name = buf_spname(curbuf);
	    જો (name == ખાલી)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (પૂર્ણાંક)STRLEN(name);
	    જો (namelen > 100)
	    {
		namelen -= 100;
		જો (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, કદ(buf));
	}
    }

    mustset |= કિંમત_changed(icon_str, &lasticon);

    જો (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    સ્થિર પૂર્ણાંક
કિંમત_changed(char_u *str, char_u **last)
{
    જો ((str == ખાલી) != (*last == ખાલી)
	    || (str != ખાલી && *last != ખાલી && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	જો (str == ખાલી)
	{
	    *last = ખાલી;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	નહીંતર
	{
	    *last = vim_strsave(str);
	    પાછા TRUE;
	}
    }
    પાછા FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    શૂન્ય
resettitle(શૂન્ય)
{
    mch_settitle(lasttitle, lasticon);
}

#જો defined(EXITFREE)
    શૂન્ય
free_titles(શૂન્ય)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#જો defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
પ્રકારવ્યાખ્યા સંરચના
{
    char_u	*stl_start;
    પૂર્ણાંક		stl_minwid;
    પૂર્ણાંક		stl_maxwid;
    ગણના {
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

સ્થિર size_t		stl_વસ્તુઓ_len = 20; // Initial value, grows as needed.
સ્થિર stl_item_T      *stl_વસ્તુઓ = ખાલી;
સ્થિર પૂર્ણાંક	       *stl_groupitem = ખાલી;
સ્થિર stl_hlrec_T     *stl_hltab = ખાલી;
સ્થિર stl_hlrec_T     *stl_tabtab = ખાલી;
સ્થિર stl_clickrec_T  *stl_clicktab = ખાલી;
સ્થિર પૂર્ણાંક		*stl_separator_locations = ખાલી;

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
    પૂર્ણાંક
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    પૂર્ણાંક		opt_scope,	// scope for "opt_name"
    પૂર્ણાંક		fillchar,
    પૂર્ણાંક		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    પાછા build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ખાલી, ખાલી);
}

    પૂર્ણાંક
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    પૂર્ણાંક		opt_scope,	// scope for "opt_name"
    પૂર્ણાંક		fillchar,
    પૂર્ણાંક		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    પૂર્ણાંક		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    પાછા build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ખાલી, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    પૂર્ણાંક
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    પૂર્ણાંક		opt_scope,	// scope for "opt_name"
    પૂર્ણાંક		fillchar,
    પૂર્ણાંક		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    પૂર્ણાંક		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    પાછા build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ખાલી, carry_hl);
}
# endif

    પૂર્ણાંક
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    પૂર્ણાંક		opt_scope)	// scope for "opt_name"
{
    પૂર્ણાંક rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (શૂન્ય)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, કદ(buf), &fmt,
	    opt_name, opt_scope, 0, 0, ખાલી, ખાલી, ખાલી, &rendered_height,
	    ખાલી);
    --emsg_off;
    પાછા rendered_height;
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
સ્થિર પૂર્ણાંક
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    પૂર્ણાંક		opt_scope,	// scope for "opt_name"
    પૂર્ણાંક		fillchar,
    પૂર્ણાંક		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    પૂર્ણાંક		*rendered_height,   // return: stl rendered height (can be NULL)
    પૂર્ણાંક		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    પૂર્ણાંક		byteval;
# ifdef FEAT_EVAL
    પૂર્ણાંક		use_sandbox;
    પૂર્ણાંક		save_VIsual_active;
# endif
    પૂર્ણાંક		empty_લીટી;
    long	l;
    long	n;
    પૂર્ણાંક		prevchar_isflag;
    પૂર્ણાંક		prevchar_isitem;
    પૂર્ણાંક		itemisflag;
    પૂર્ણાંક		fillable;
    char_u	*str;
    long	અંક;
    પૂર્ણાંક		width;
    પૂર્ણાંક		itemcnt;
    પૂર્ણાંક		curitem;
    પૂર્ણાંક		groupdepth;
# ifdef FEAT_EVAL
    પૂર્ણાંક		evaldepth;
# endif
    પૂર્ણાંક		minwid;
    પૂર્ણાંક		maxwid;
    પૂર્ણાંક		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    પૂર્ણાંક		save_redraw_not_allowed = redraw_not_allowed;
    પૂર્ણાંક		save_ચાવી_Typed = ચાવી_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    પૂર્ણાંક		did_emsg_before = did_emsg;
    પૂર્ણાંક		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    જો (updating_screen)
	redraw_not_allowed = TRUE;

    જો (stl_વસ્તુઓ == ખાલી)
    {
	stl_વસ્તુઓ = ALLOC_MULT(stl_item_T, stl_વસ્તુઓ_len);
	stl_groupitem = ALLOC_MULT(પૂર્ણાંક, stl_વસ્તુઓ_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_વસ્તુઓ_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_વસ્તુઓ_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_વસ્તુઓ_len + 1);

	stl_separator_locations = ALLOC_MULT(પૂર્ણાંક, stl_વસ્તુઓ_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    જો (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_સંખ્યા;
	tv.vval.v_સંખ્યા = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	જો (usefmt == ખાલી)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    જો (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    જો (lnum > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
    {
	lnum = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_લીટી = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    જો (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    નહીંતર
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    જો (carry_hl != ખાલી && *carry_hl != 0)
    {
	stl_વસ્તુઓ[curitem].stl_type = Highlight;
	stl_વસ્તુઓ[curitem].stl_start = p;
	stl_વસ્તુઓ[curitem].stl_minwid = *carry_hl;
	stl_વસ્તુઓ[curitem].stl_clickfunc = ખાલી;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    માટે (s = usefmt; *s != NUL; )
    {
	જો (curitem == (પૂર્ણાંક)stl_વસ્તુઓ_len)
	{
	    size_t	new_len = stl_વસ્તુઓ_len * 3 / 2;

	    stl_item_T *new_વસ્તુઓ =
			  vim_realloc(stl_વસ્તુઓ, કદ(stl_item_T) * new_len);
	    જો (new_વસ્તુઓ == ખાલી)
		તોડો;
	    stl_વસ્તુઓ = new_વસ્તુઓ;

	    પૂર્ણાંક *new_groupitem =
			     vim_realloc(stl_groupitem, કદ(પૂર્ણાંક) * new_len);
	    જો (new_groupitem == ખાલી)
		તોડો;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  કદ(stl_hlrec_T) * (new_len + 1));
	    જો (new_hlrec == ખાલી)
		તોડો;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  કદ(stl_hlrec_T) * (new_len + 1));
	    જો (new_hlrec == ખાલી)
		તોડો;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      કદ(stl_clickrec_T) * (new_len + 1));
	    જો (new_clickrec == ખાલી)
		તોડો;
	    stl_clicktab = new_clickrec;

	    પૂર્ણાંક *new_separator_locs = vim_realloc(stl_separator_locations,
					    કદ(પૂર્ણાંક) * new_len);
	    જો (new_separator_locs == ખાલી)
		તોડો;
	    stl_separator_locations = new_separator_locs;

	    stl_વસ્તુઓ_len = new_len;
	}

	stl_વસ્તુઓ[curitem].stl_clickfunc = ખાલી;

	જો (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	જ્યારે (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    જો (*s == '\n' || *s == '\r')
	    {
		જો (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    જાઓ find_linebreak;
		}
		નહીંતર જો (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	જો (*s == NUL || p + 1 >= out + outlen)
	    તોડો;

	/*
	 * Handle one '%' item.
	 */
	s++;
	જો (*s == NUL)  // ignore trailing %
	    તોડો;

	જો (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    જો (s[1] == ']')
	    {
		stl_વસ્તુઓ[curitem].stl_type = ClickFunc;
		stl_વસ્તુઓ[curitem].stl_start = p;
		stl_વસ્તુઓ[curitem].stl_minwid = 0;
		stl_વસ્તુઓ[curitem].stl_clickfunc = ખાલી;
		s += 2;
		curitem++;
		ચાલુ;
	    }
	    // %[FuncName] - start click region
	    જો (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		જો (rb != ખાલી)
		{
		    stl_વસ્તુઓ[curitem].stl_type = ClickFunc;
		    stl_વસ્તુઓ[curitem].stl_start = p;
		    stl_વસ્તુઓ[curitem].stl_minwid = 0;
		    stl_વસ્તુઓ[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    ચાલુ;
		}
	    }
	}
	જો (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    જો (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		તોડો;
	    }
	    નહીંતર જો (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	જો (*s == '%')
	{
	    જો (p + 1 >= out + outlen)
		તોડો;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    ચાલુ;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	જો (*s == STL_SEPARATE)
	{
	    s++;
	    જો (groupdepth > 0)
		ચાલુ;
	    stl_વસ્તુઓ[curitem].stl_type = Separate;
	    stl_વસ્તુઓ[curitem++].stl_start = p;
	    ચાલુ;
	}
	જો (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_વસ્તુઓ[curitem].stl_type = Trunc;
	    stl_વસ્તુઓ[curitem++].stl_start = p;
	    ચાલુ;
	}
	જો (*s == ')')
	{
	    char_u  *t;

	    s++;
	    જો (groupdepth < 1)
		ચાલુ;
	    groupdepth--;

	    t = stl_વસ્તુઓ[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    જો (curitem > stl_groupitem[groupdepth] + 1
		    && stl_વસ્તુઓ[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		પૂર્ણાંક group_start_userhl = 0;
		પૂર્ણાંક group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		માટે (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    જો (stl_વસ્તુઓ[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_વસ્તુઓ[n].stl_minwid;
			તોડો;
		    }
		}
		માટે (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    જો (stl_વસ્તુઓ[n].stl_type == Normal)
			તોડો;
		    જો (stl_વસ્તુઓ[n].stl_type == Highlight)
			group_end_userhl = stl_વસ્તુઓ[n].stl_minwid;
		}
		જો (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    માટે (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			જો (stl_વસ્તુઓ[n].stl_type == Highlight)
			    stl_વસ્તુઓ[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			જો (stl_વસ્તુઓ[n].stl_type == TabPage)
			    stl_વસ્તુઓ[n].stl_start = p;
		    }
		}
	    }
	    જો (l > stl_વસ્તુઓ[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		જો (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    જ્યારે (l >= stl_વસ્તુઓ[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		નહીંતર
		    n = (long)(p - t) - stl_વસ્તુઓ[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		પૂર્ણાંક minwid_fixed = MIN(stl_વસ્તુઓ[stl_groupitem[groupdepth]].stl_minwid,
				       stl_વસ્તુઓ[stl_groupitem[groupdepth]].stl_maxwid);
		જ્યારે (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		માટે (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_વસ્તુઓ[l].stl_start -= n - 1;
		    જો (stl_વસ્તુઓ[l].stl_start < t)
			stl_વસ્તુઓ[l].stl_start = t;
		}
	    }
	    નહીંતર જો (abs(stl_વસ્તુઓ[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_વસ્તુઓ[stl_groupitem[groupdepth]].stl_minwid;
		પૂર્ણાંક fillchar_len = MB_CHAR2LEN(fillchar);
		જો (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    જ્યારે (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		નહીંતર
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    જો (p + l >= out + outlen)
		    {
			n = (long)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    માટે ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    માટે (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_વસ્તુઓ[n].stl_start += l;
		}
	    }
	    ચાલુ;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	જો (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	જો (*s == '-')
	{
	    s++;
	    l = -1;
	}
	જો (VIM_ISDIGIT(*s))
	{
	    minwid = (પૂર્ણાંક)getdigits(&s);
	    જો (minwid < 0)	// overflow
		minwid = 0;
	}
	જો (*s == STL_USER_HL)
	{
	    stl_વસ્તુઓ[curitem].stl_type = Highlight;
	    stl_વસ્તુઓ[curitem].stl_start = p;
	    stl_વસ્તુઓ[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    ચાલુ;
	}
	જો (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    જો (*s == STL_TABCLOSENR)
	    {
		જો (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    માટે (n = curitem - 1; n >= 0; --n)
			જો (stl_વસ્તુઓ[n].stl_type == TabPage
					       && stl_વસ્તુઓ[n].stl_minwid >= 0)
			{
			    minwid = stl_વસ્તુઓ[n].stl_minwid;
			    તોડો;
			}
		}
		નહીંતર
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_વસ્તુઓ[curitem].stl_type = TabPage;
	    stl_વસ્તુઓ[curitem].stl_start = p;
	    stl_વસ્તુઓ[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    ચાલુ;
	}
	જો (*s == '.')
	{
	    s++;
	    જો (VIM_ISDIGIT(*s))
	    {
		maxwid = (પૂર્ણાંક)getdigits(&s);
		જો (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	પૂર્ણાંક raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	જો (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_વસ્તુઓ[curitem].stl_type = Group;
	    stl_વસ્તુઓ[curitem].stl_start = p;
	    stl_વસ્તુઓ[curitem].stl_minwid = minwid;
	    stl_વસ્તુઓ[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    ચાલુ;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	જો (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    ચાલુ;
	}
# endif
	જો (vim_strchr(STL_ALL, *s) == ખાલી)
	{
	    જો (*s == NUL)  // can happen with "%0"
		તોડો;
	    s++;
	    ચાલુ;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	અંક = -1;
	str = ખાલી;
	પસંદગી (opt)
	{
	સ્થિતિ STL_FILEPATH:
	સ્થિતિ STL_FULLPATH:
	સ્થિતિ STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    જો (name != ખાલી)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    નહીંતર
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    જો (opt != STL_FILENAME)
		str = NameBuff;
	    નહીંતર
		str = gettail(NameBuff);
	    તોડો;
	}

	સ્થિતિ STL_VIM_પદાવલિ: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    પૂર્ણાંક	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    જો (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    જ્યારે ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    જો (*s != '}')	// missing '}' or out of space
		તોડો;
	    s++;
	    જો (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    નહીંતર
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((અક્ષર *)buf_tmp, કદ(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((અક્ષર *)buf_tmp, કદ(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    જો (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    જો (!zeropad && str != ખાલી && *str != NUL)
	    {
		જો (*skipdigits(str) == NUL)
		{
		    અંક = atoi((અક્ષર *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    જો (reevaluate && str != ખાલી && *str != NUL
		    && strchr((અચલ અક્ષર *)str, '%') != ખાલી
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((અચલ અક્ષર *)str);
		size_t fmt_length = strlen((અચલ અક્ષર *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * કદ(char_u));

		જો (new_fmt != ખાલી)
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
		    new_fmt_p = ખાલી;

		    જો (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    ચાલુ;
		}
	    }
# endif
	    તોડો;
	}
	સ્થિતિ STL_લીટી:
	    અંક = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    તોડો;

	સ્થિતિ STL_NUMLINES:
	    અંક = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
	    તોડો;

	સ્થિતિ STL_COLUMN:
	    અંક = (State & MODE_ઉમેરો) == 0 && empty_લીટી
					       ? 0 : (પૂર્ણાંક)wp->w_cursor.col + 1;
	    તોડો;

	સ્થિતિ STL_VIRTCOL:
	સ્થિતિ STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    જો (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_ઉમેરો) == 0
			       && empty_લીટી ? 0 : (પૂર્ણાંક)wp->w_cursor.col + 1)))
		તોડો;
	    અંક = (long)virtcol;
	    તોડો;
	}

	સ્થિતિ STL_PERCENTAGE:
	    અંક = calc_percentage((long)wp->w_cursor.lnum, (long)wp->w_buffer->b_ml.ml_લીટી_ગણતરી);
	    તોડો;

	સ્થિતિ STL_ALTPERCENT:
	    str = buf_tmp;
	    (શૂન્ય)get_rel_pos(wp, str, TMPLEN);
	    તોડો;

	સ્થિતિ STL_SHOWCMD:
	    જો (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    તોડો;

	સ્થિતિ STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    જો (append_arg_સંખ્યા(wp, buf_tmp, કદ(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    તોડો;

	સ્થિતિ STL_KEYMAP:
	    fillable = FALSE;
	    જો (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    તોડો;
	સ્થિતિ STL_PAGENUM:
# જો defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    અંક = printer_page_અંક;
# નહીંતર
	    અંક = 0;
# endif
	    તોડો;

	સ્થિતિ STL_BUFNO:
	    અંક = wp->w_buffer->b_fnum;
	    તોડો;

	સ્થિતિ STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	સ્થિતિ STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_લીટી_or_offset(wp->w_buffer, wp->w_cursor.lnum, ખાલી);
	    અંક = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_ઉમેરો) == 0 && empty_લીટી
				? 0 : (પૂર્ણાંક)wp->w_cursor.col);
# endif
	    તોડો;

	સ્થિતિ STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	સ્થિતિ STL_BYTEVAL:
	    અંક = byteval;
	    જો (અંક == NL)
		અંક = 0;
	    નહીંતર જો (અંક == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		અંક = NL;
	    તોડો;

	સ્થિતિ STL_ROFLAG:
	સ્થિતિ STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    જો (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    તોડો;

	સ્થિતિ STL_HELPFLAG:
	સ્થિતિ STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    જો (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    તોડો;

	સ્થિતિ STL_FILETYPE:
	    જો (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((અક્ષર *)buf_tmp, કદ(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    તોડો;

	સ્થિતિ STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    જો (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((અક્ષર *)buf_tmp, કદ(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		માટે (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    તોડો;

# જો defined(FEAT_QUICKFIX)
	સ્થિતિ STL_PREVIEWFLAG:
	સ્થિતિ STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    જો (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    તોડો;

	સ્થિતિ STL_QUICKFIX:
	    જો (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(સંદેશ_loclist)
			    : _(સંદેશ_qflist));
	    તોડો;
# endif

	સ્થિતિ STL_MODIFIED:
	સ્થિતિ STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    પસંદગી ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		સ્થિતિ 2: str = (char_u *)"[+]"; તોડો;
		સ્થિતિ 3: str = (char_u *)",+"; તોડો;
		સ્થિતિ 4: str = (char_u *)"[-]"; તોડો;
		સ્થિતિ 5: str = (char_u *)",-"; તોડો;
		સ્થિતિ 6: str = (char_u *)"[+-]"; તોડો;
		સ્થિતિ 7: str = (char_u *)",+-"; તોડો;
	    }
	    તોડો;

	સ્થિતિ STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		જ્યારે (*s != '#' && *s != NUL)
		    ++s;
		જો (*s == '#')
		{
		    stl_વસ્તુઓ[curitem].stl_type = Highlight;
		    stl_વસ્તુઓ[curitem].stl_start = p;
		    stl_વસ્તુઓ[curitem].stl_minwid = -syn_namen2id(t, (પૂર્ણાંક)(s - t));
		    curitem++;
		}
		જો (*s != NUL)
		    ++s;
		ચાલુ;
	    }

	સ્થિતિ STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    જો (*s == ']')
	    {
		stl_વસ્તુઓ[curitem].stl_type = ClickFunc;
		stl_વસ્તુઓ[curitem].stl_start = p;
		stl_વસ્તુઓ[curitem].stl_minwid = 0;
		stl_વસ્તુઓ[curitem].stl_clickfunc = ખાલી;
		s++;
		curitem++;
		ચાલુ;
	    }
	    // %N[FuncName] with minwid
	    જો (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		જો (rb != ખાલી)
		{
		    stl_વસ્તુઓ[curitem].stl_type = ClickFunc;
		    stl_વસ્તુઓ[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_વસ્તુઓ[curitem].stl_minwid = raw_minwid;
		    stl_વસ્તુઓ[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    ચાલુ;
		}
	    }
	    ચાલુ;

	સ્થિતિ STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    ચાલુ;
	}

	stl_વસ્તુઓ[curitem].stl_start = p;
	stl_વસ્તુઓ[curitem].stl_type = Normal;
	જો (str != ખાલી && *str)
	{
	    char_u  *t = str;

	    જો (itemisflag)
	    {
		જો ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    જો (l > 0)
		prevchar_isitem = TRUE;
	    જો (l > maxwid)
	    {
		જ્યારે (l >= maxwid)
		    જો (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    નહીંતર
			l -= byte2cells(*t++);
		જો (p + 1 >= out + outlen)
		    તોડો;
		*p++ = '<';
	    }
	    જો (minwid > 0)
	    {
		માટે (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    જો (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    નહીંતર
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    નહીંતર
		minwid *= -1;
	    માટે (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		જો (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		નહીંતર
		    *p++ = *t;
	    }
	    માટે (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	નહીંતર જો (અંક >= 0)
	{
	    પૂર્ણાંક	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    જો (p + 20 >= out + outlen)
		તોડો;		// not sufficient space
	    prevchar_isitem = TRUE;
	    જો (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    જો (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    માટે (n = અંક, l = 1; n >= nbase; n /= nbase)
		l++;
	    જો (opt == STL_VIRTCOL_ALT)
		l++;

	    જો (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		જ્યારે (l-- > maxwid)
		    અંક /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((અક્ષર *)p, outlen - (p - out),
		    (અક્ષર *)nstr, 0, અંક, n);
	    }
	    નહીંતર
		p += vim_snprintf_safelen((અક્ષર *)p, outlen - (p - out),
		    (અક્ષર *)nstr, minwid, અંક);
	}
	નહીંતર
	    stl_વસ્તુઓ[curitem].stl_type = Empty;

	જો (અંક >= 0 || (!itemisflag && str != ખાલી && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	જો (opt == STL_VIM_પદાવલિ)
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
    જો (carry_hl != ખાલી)
    {
	પૂર્ણાંક last_hl = 0;
	માટે (l = 0; l < itemcnt; l++)
	    જો (stl_વસ્તુઓ[l].stl_type == Highlight)
		last_hl = stl_વસ્તુઓ[l].stl_minwid;
	*carry_hl = last_hl;
    }

    જો (mode == STL_MODE_MULTI
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
	જો (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = strlen((અક્ષર *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    નહીંતર
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	જો (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    જો (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	માટે (l = 0; l < itemcnt; l++)
	    જો (stl_વસ્તુઓ[l].stl_type == ClickFunc)
		vim_free(stl_વસ્તુઓ[l].stl_clickfunc);
	જો (rendered_height != ખાલી)
	    *rendered_height = rheight;
	પાછા 0;
    }

    width = vim_strsize(out);
    જો (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	જો (itemcnt == 0)
	    s = out;
	નહીંતર
	{
	    માટે ( ; l < itemcnt; l++)
		જો (stl_વસ્તુઓ[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_વસ્તુઓ[l].stl_start;
		    તોડો;
		}
	    જો (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_વસ્તુઓ[0].stl_start;
		l = 0;
	    }
	}

	જો (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    જો (has_mbyte)
	    {
		s = out;
		width = 0;
		માટે (;;)
		{
		    width += ptr2cells(s);
		    જો (width >= maxwidth)
			તોડો;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		જ્યારે (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    નહીંતર
		s = out + maxwidth - 1;
	    માટે (l = 0; l < itemcnt; l++)
		જો (stl_વસ્તુઓ[l].stl_start > s)
		    તોડો;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	નહીંતર
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    જ્યારે (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(end - p) + 1);	// +1 for NUL
	    end -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    માટે (; l < itemcnt; l++)
	    {
		જો (stl_વસ્તુઓ[l].stl_start - n >= s)
		    stl_વસ્તુઓ[l].stl_start -= n;
		નહીંતર
		    stl_વસ્તુઓ[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    જ્યારે (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    નહીંતર જો (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	પૂર્ણાંક અંક_separators = 0;

	માટે (l = 0; l < itemcnt; l++)
	{
	    જો (stl_વસ્તુઓ[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[અંક_separators] = l;
		અંક_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	જો (અંક_separators)
	{
	    પૂર્ણાંક standard_spaces;
	    પૂર્ણાંક final_spaces;

	    standard_spaces = (maxwidth - width) / અંક_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (અંક_separators - 1);
	    માટે (l = 0; l < અંક_separators; l++)
	    {
		પૂર્ણાંક dislocation = (l == (અંક_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_વસ્તુઓ[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		માટે (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		માટે (પૂર્ણાંક i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_વસ્તુઓ[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    જો (hltab != ખાલી)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	માટે (l = 0; l < itemcnt; l++)
	{
	    જો (stl_વસ્તુઓ[l].stl_type == Highlight)
	    {
		sp->start = stl_વસ્તુઓ[l].stl_start;
		sp->userhl = stl_વસ્તુઓ[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = ખાલી;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    જો (tabtab != ખાલી)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	માટે (l = 0; l < itemcnt; l++)
	{
	    જો (stl_વસ્તુઓ[l].stl_type == TabPage)
	    {
		sp->start = stl_વસ્તુઓ[l].stl_start;
		sp->userhl = stl_વસ્તુઓ[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = ખાલી;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    જો (clicktab != ખાલી)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	માટે (l = 0; l < itemcnt; l++)
	{
	    જો (stl_વસ્તુઓ[l].stl_type == ClickFunc)
	    {
		cp->start = stl_વસ્તુઓ[l].stl_start;
		cp->funcname = stl_વસ્તુઓ[l].stl_clickfunc;
		cp->minwid = stl_વસ્તુઓ[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = ખાલી;
	cp->funcname = ખાલી;
    }
    નહીંતર
    {
	// Free click function names when caller doesn't need them.
	માટે (l = 0; l < itemcnt; l++)
	    જો (stl_વસ્તુઓ[l].stl_type == ClickFunc)
		vim_free(stl_વસ્તુઓ[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    ચાવી_Typed = save_ચાવી_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    જો (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    પાછા width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    પૂર્ણાંક
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    પૂર્ણાંક		buflen)
{
    long	above; // number of lines above window
    long	below; // number of lines below window

    જો (buflen < 3) // need at least 3 chars for writing
	પાછા 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    જો (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_લીટી_ગણતરી - wp->w_botline + 1;
    જો (below <= 0)
	પાછા (પૂર્ણાંક)vim_snprintf_safelen((અક્ષર *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    જો (above <= 0)
	પાછા (પૂર્ણાંક)vim_snprintf_safelen((અક્ષર *)buf, buflen,
	    "%s", _("Top"));

    પૂર્ણાંક perc = calc_percentage(above, above + below);
    અક્ષર tmp[8];
    // localized percentage value
    vim_snprintf(tmp, કદ(tmp), _("%d%%"), perc);
    પાછા (પૂર્ણાંક)vim_snprintf_safelen((અક્ષર *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    સ્થિર પૂર્ણાંક
append_arg_સંખ્યા(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    પૂર્ણાંક		add_file)	// Add "file" before the arg number
{
    જો (ARGCOUNT <= 1)		// nothing to do
	પાછા 0;

    અક્ષર *સંદેશ;
    પસંદગી ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	સ્થિતિ 0: સંદેશ = _(" (%d of %d)"); તોડો;
	સ્થિતિ 1: સંદેશ = _(" ((%d) of %d)"); તોડો;
	સ્થિતિ 2: સંદેશ = _(" (file %d of %d)"); તોડો;
	સ્થિતિ 3: સંદેશ = _(" (file (%d) of %d)"); તોડો;
    }

    પાછા (પૂર્ણાંક)vim_snprintf_safelen((અક્ષર *)buf, buflen, સંદેશ,
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
    પાછા FullName_save(fname, TRUE);
#નહીંતર
    જો (!vim_isAbsName(fname)
	    || strstr((અક્ષર *)fname, "..") != ખાલી
	    || strstr((અક્ષર *)fname, "//") != ખાલી
# ifdef BACKSLASH_IN_FILENAME
	    || strstr((અક્ષર *)fname, "\\\\") != ખાલી
# endif
# જો defined(MSWIN)
	    || vim_strchr(fname, '~') != ખાલી
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	પાછા FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    જો (fname != ખાલી)
	fname_case(fname, 0);	// set correct case for file name
# endif

    પાછા fname;
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
    શૂન્ય
fname_expand(
    buf_T	*buf UNUSED,
    char_u	**ffname,
    char_u	**sfname)
{
    જો (*ffname == ખાલી)	    // no file name given, nothing to do
	પાછા;
    જો (*sfname == ખાલી)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    જો (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	જો (rfname != ખાલી)
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
    શૂન્ય
ex_buffer_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    પૂર્ણાંક		split_ret = OK;
    પૂર્ણાંક		p_ea_save;
    પૂર્ણાંક		open_wins = 0;
    પૂર્ણાંક		r;
    પૂર્ણાંક		ગણતરી;		// Maximum number of windows to open.
    પૂર્ણાંક		all;		// When TRUE also load inactive buffers.
    પૂર્ણાંક		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    જો (eap->addr_ગણતરી == 0)	// make as many windows as possible
	ગણતરી = 9999;
    નહીંતર
	ગણતરી = eap->line2;	// make as many windows as specified
    જો (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    નહીંતર
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
    જો (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    માટે (;;)
    {
	tpnext = curtab->tp_next;
	માટે (wp = firstwin; wp != ખાલી; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    જો ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		જો (win_close(wp, FALSE) == FAIL)
		    તોડો;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    નહીંતર
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	જો (had_tab == 0 || tpnext == ખાલી)
	    તોડો;
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
    માટે (buf = firstbuf; buf != ખાલી && open_wins < ગણતરી; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	જો ((!all && buf->b_ml.ml_mfp == ખાલી) || !buf->b_p_bl)
	    ચાલુ;

	જો (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    જો (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    નહીંતર
		wp = ખાલી;
	}
	નહીંતર
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		જો (wp->w_buffer == buf)
		    તોડો;
	    // If the buffer already has a window, move it
	    જો (wp != ખાલી)
		win_move_after(wp, curwin);
	}

	જો (wp == ખાલી && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    જો (split_ret == FAIL)
		ચાલુ;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    જો (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		તોડો;
	    }
	    જો (swap_exists_action == SEA_QUIT)
	    {
#જો defined(FEAT_EVAL)
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

#જો defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    નહીંતર
		handle_swap_exists(ખાલી);
	}

	ui_breakcheck();
	જો (got_int)
	{
	    (શૂન્ય)vgetc();	// only break the file loading, not the rest
	    તોડો;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	જો (aborting())
	    તોડો;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	જો (had_tab > 0 && tabpage_index(ખાલી) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    માટે (wp = lastwin; open_wins > ગણતરી; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	જો (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	નહીંતર જો (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	નહીંતર
	{
	    wp = wp->w_prev;
	    જો (wp == ખાલી)
		તોડો;
	}
    }
}


સ્થિર પૂર્ણાંક  chk_modeline(linenr_T, પૂર્ણાંક);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    શૂન્ય
do_modelines(પૂર્ણાંક flags)
{
    linenr_T	lnum;
    પૂર્ણાંક		nmlines;
    સ્થિર પૂર્ણાંક	entered = 0;

    જો (!curbuf->b_p_ml || (nmlines = (પૂર્ણાંક)p_mls) == 0)
	પાછા;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    જો (entered)
	પાછા;

    ++entered;
    માટે (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_લીટી_ગણતરી && lnum <= nmlines;
								       ++lnum)
	જો (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    માટે (lnum = curbuf->b_ml.ml_લીટી_ગણતરી; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_લીટી_ગણતરી - nmlines; --lnum)
	જો (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#અન્તર્ભાવ "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    સ્થિર પૂર્ણાંક
chk_modeline(
    linenr_T	lnum,
    પૂર્ણાંક		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*લીટી_end;		// point to the end of the line
    char_u	*e;
    પૂર્ણાંક		prev;
    પૂર્ણાંક		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    લીટી_end = s + ml_get_len(lnum);
    માટે (; *s != NUL; ++s)
    {
	જો (prev == -1 || vim_isspace(prev))
	{
	    જો ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		તોડો;
	    // Accept both "vim" and "Vim".
	    જો ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		પૂર્ણાંક vers;

		જો (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		નહીંતર
		    e = s + 3;
		vers = getdigits(&e);
		જો (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    તોડો;
	    }
	}
	prev = *s;
    }

    જો (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	પૂર્ણાંક	end;

	કરો				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	જ્યારે (s[-1] != ':');

	len = (size_t)(લીટી_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	જો (linecopy == ખાલી)
	    પાછા FAIL;

	લીટી_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_ધકેલો(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	જ્યારે (end == FALSE)
	{
	    s = skipwhite(s);
	    જો (*s == NUL)
		તોડો;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    માટે (e = s; *e != ':' && *e != NUL; ++e)
		જો (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(લીટી_end - (e + 1)) + 1);	// +1 for NUL
		    --લીટી_end;
		}
	    જો (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    જો (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		જો (*e != ':')		// no terminating ':'?
		    તોડો;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    જો (*s != NUL)		// skip over an empty "::"
	    {
		પૂર્ણાંક secure_save = secure;
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
		જો (retval == FAIL)		// stop if error found
		    તોડો;
	    }
	    s = (e == લીટી_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_કાઢો();
	vim_free(linecopy);
    }
    પાછા retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    પૂર્ણાંક
bt_normal(buf_T *buf)
{
    પાછા buf != ખાલી && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    પૂર્ણાંક
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    પાછા buf != ખાલી && buf->b_p_bt[0] == 'q';
#નહીંતર
    પાછા FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    પૂર્ણાંક
bt_terminal(buf_T *buf UNUSED)
{
#જો defined(FEAT_TERMINAL)
    પાછા buf != ખાલી && buf->b_p_bt[0] == 't';
#નહીંતર
    પાછા FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    પૂર્ણાંક
bt_help(buf_T *buf)
{
    પાછા buf != ખાલી && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    પૂર્ણાંક
bt_prompt(buf_T *buf)
{
    પાછા buf != ખાલી && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#જો defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    પૂર્ણાંક
bt_popup(buf_T *buf)
{
    પાછા buf != ખાલી && buf->b_p_bt != ખાલી
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    પૂર્ણાંક
bt_nofilename(buf_T *buf)
{
    પાછા buf != ખાલી && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    સ્થિર પૂર્ણાંક
bt_nofileread(buf_T *buf)
{
    પાછા buf != ખાલી && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#જો defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    પૂર્ણાંક
bt_nofile(buf_T *buf)
{
    પાછા buf != ખાલી && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    પૂર્ણાંક
bt_dontwrite(buf_T *buf)
{
    પાછા buf != ખાલી && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    પૂર્ણાંક
bt_dontwrite_સંદેશ(buf_T *buf)
{
    જો (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    પૂર્ણાંક
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    પસંદગી (buf->b_p_bh[0])
    {
	સ્થિતિ 'u':		    // "unload"
	સ્થિતિ 'w':		    // "wipe"
	સ્થિતિ 'd': પાછા FALSE;	    // "delete"
	સ્થિતિ 'h': પાછા TRUE;	    // "hide"
    }
    પાછા (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#જો defined(FEAT_QUICKFIX)
    જો (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	જો (buf->b_fnum == qf_ઢગલો_get_bufnr())
	    પાછા (char_u *)_(સંદેશ_qflist);
	નહીંતર
	    પાછા (char_u *)_(સંદેશ_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    જો (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	જો (buf->b_term != ખાલી)
	    પાછા term_get_status_text(buf->b_term);
#endif
	જો (buf->b_fname != ખાલી)
	    પાછા buf->b_fname;
	જો (buf == cmdwin_buf)
	    પાછા (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	જો (bt_prompt(buf))
	    પાછા (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	જો (bt_popup(buf))
	    પાછા (char_u *)_("[Popup]");
#endif
	પાછા (char_u *)_("[Scratch]");
    }

    જો (buf->b_fname == ખાલી)
	પાછા buf_get_fname(buf);
    પાછા ખાલી;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    જો (buf->b_fname == ખાલી)
	પાછા (char_u *)_("[No Name]");
    પાછા buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    શૂન્ય
set_buflisted(પૂર્ણાંક on)
{
    જો (on == curbuf->b_p_bl)
	પાછા;

    curbuf->b_p_bl = on;
    જો (on)
	apply_autocmds(EVENT_BUFADD, ખાલી, ખાલી, FALSE, curbuf);
    નહીંતર
	apply_autocmds(EVENT_BUFDELETE, ખાલી, ખાલી, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    પૂર્ણાંક
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    પૂર્ણાંક		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ખાલી, ખાલી, (linenr_T)1, BLN_DUMMY);
    જો (newbuf == ખાલી)
	પાછા TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    જો (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	પાછા TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    જો (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	પાછા TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    જો (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	જો (buf->b_ml.ml_લીટી_ગણતરી == curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    differ = FALSE;
	    માટે (lnum = 1; lnum <= curbuf->b_ml.ml_લીટી_ગણતરી; ++lnum)
		જો (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    તોડો;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    જો (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    પાછા differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    શૂન્ય
wipe_buffer(
    buf_T	*buf,
    પૂર્ણાંક		aucmd)	    // When TRUE trigger autocommands.
{
    જો (buf->b_fnum == ટોચ_file_અંક - 1)
	--ટોચ_file_અંક;

    જો (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(ખાલી, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    જો (!aucmd)
	unblock_autocmds();
}
