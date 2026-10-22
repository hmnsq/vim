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

#অন্তর্ভাৱ "vim.h"


#ifdef FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_DEPTH 100
#endif

স্থির শূন্য	enter_buffer(buf_T *buf);
স্থির শূন্য	buflist_getfpos(শূন্য);
স্থির char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, পূর্ণাঙ্ক ignore_case);
স্থির char_u	*fname_match(regmatch_T *rmp, char_u *name, পূর্ণাঙ্ক ignore_case);
#ifdef UNIX
স্থির buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
স্থির পূর্ণাঙ্ক	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
স্থির পূর্ণাঙ্ক	buf_same_ino(buf_T *buf, stat_T *stp);
#অন্যথা
স্থির পূর্ণাঙ্ক	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
স্থির পূর্ণাঙ্ক	মান_changed(char_u *str, char_u **last);
#যদি defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
স্থির পূর্ণাঙ্ক build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, পূর্ণাঙ্ক opt_scope, পূর্ণাঙ্ক fillchar, পূর্ণাঙ্ক maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, পূর্ণাঙ্ক *lbreaks, পূর্ণাঙ্ক *carry_hl);
#endif
স্থির পূর্ণাঙ্ক	append_arg_সংখ্যা(win_T *wp, char_u *buf, size_t buflen, পূর্ণাঙ্ক add_file);
স্থির শূন্য	free_buffer(buf_T *);
স্থির শূন্য	free_buffer_stuff(buf_T *buf, পূর্ণাঙ্ক free_options);
স্থির পূর্ণাঙ্ক	bt_nofileread(buf_T *buf);
স্থির পূর্ণাঙ্ক	do_buffer_ext(পূর্ণাঙ্ক action, পূর্ণাঙ্ক start, পূর্ণাঙ্ক dir, পূর্ণাঙ্ক গুনতি, পূর্ণাঙ্ক flags);

#ifdef UNIX
# define dev_T dev_t
#অন্যথা
# define dev_T unsigned
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    বাবে ((buf) = lastbuf; (buf) != রিক্ত; (buf) = (buf)->b_prev)

#যদি defined(FEAT_QUICKFIX)
স্থির ৱর্ণ *বাৰ্তা_loclist = N_("[Location List]");
স্থির ৱর্ণ *বাৰ্তা_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
স্থির পূর্ণাঙ্ক	buf_free_গুনতি = 0;

স্থির পূর্ণাঙ্ক	শীৰ্ষ_file_অঙ্ক = 1;	// highest file number
স্থির garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    স্থির শূন্য
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"যদি exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    স্থির পূর্ণাঙ্ক
calc_percentage(long part, long whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    ঘূৰাই (part > 1000000L)
	? (পূর্ণাঙ্ক)(part / (whole / 100L))
	: (পূর্ণাঙ্ক)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    পূর্ণাঙ্ক
get_highest_fnum(শূন্য)
{
    ঘূৰাই শীৰ্ষ_file_অঙ্ক - 1;
}

/*
 * Read data from buffer for retrying.
 */
    স্থির পূর্ণাঙ্ক
read_buffer(
    পূর্ণাঙ্ক		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    পূর্ণাঙ্ক		flags)		    // extra flags for readfile()
{
    পূর্ণাঙ্ক		retval = OK;
    linenr_T	সাৰি_গুনতি;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    সাৰি_গুনতি = curbuf->b_ml.ml_সাৰি_গুনতি;
    retval = readfile(
	    read_stdin ? রিক্ত : curbuf->b_ffname,
	    read_stdin ? রিক্ত : curbuf->b_fname,
	    সাৰি_গুনতি, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    যদি (retval == OK)
    {
	// Delete the binary lines.
	যেতিয়ালৈ (--সাৰি_গুনতি >= 0)
	    ml_delete((linenr_T)1);
    }
    অন্যথা
    {
	// Delete the converted lines.
	যেতিয়ালৈ (curbuf->b_ml.ml_সাৰি_গুনতি > সাৰি_গুনতি)
	    ml_delete(সাৰি_গুনতি);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    যদি (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	যদি (!readonlymode && !BUFEMPTY())
	    changed();
	অন্যথা যদি (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	যদি (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, রিক্ত, রিক্ত, FALSE,
							      curbuf, &retval);
#অন্যথা
	    apply_autocmds(EVENT_STDINREADPOST, রিক্ত, রিক্ত, FALSE, curbuf);
#endif
	}
    }
    ঘূৰাই retval;
}

#যদি defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    শূন্য
buffer_ensure_loaded(buf_T *buf)
{
    যদি (buf->b_ml.ml_mfp != রিক্ত)
	ঘূৰাই;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    যদি (curbuf == buf)
    {
	যদি (swap_exists_action != SEA_READONLY)
	    swap_exists_action = SEA_NONE;
	open_buffer(FALSE, রিক্ত, 0);
	aucmd_restbuf(&aco);
    }
}
#endif

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    পূর্ণাঙ্ক
open_buffer(
    পূর্ণাঙ্ক		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    পূর্ণাঙ্ক		flags_arg)	    // extra flags for readfile()
{
    পূর্ণাঙ্ক		flags = flags_arg;
    পূর্ণাঙ্ক		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    পূর্ণাঙ্ক		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    যদি (readonlymode && curbuf->b_ffname != রিক্ত
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    যদি (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    যদি (curbuf->b_ml.ml_mfp != রিক্ত)
		ভঙ্গ;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	যদি (curbuf == রিক্ত)
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
	যদি (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(রিক্ত, curwin);
#endif
	ঘূৰাই FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    যদি (curbuf->b_ml.ml_mfp != রিক্ত)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    যদি (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    যদি (curbuf->b_ffname != রিক্ত
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	পূর্ণাঙ্ক old_বাৰ্তা_silent = বাৰ্তা_silent;
#ifdef UNIX
	পূর্ণাঙ্ক	save_bin = curbuf->b_p_bin;
	পূর্ণাঙ্ক perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	পূর্ণাঙ্ক oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	যদি (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	যদি (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	যদি (shortmess(SHM_FILEINFO))
	    বাৰ্তা_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	যদি (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    যদি (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_buffer(FALSE, eap, flags);
	}
#endif
	বাৰ্তা_silent = old_বাৰ্তা_silent;
#ifdef FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#endif
	// Help buffer is filtered.
	যদি (bt_help(curbuf))
	    fix_help_buffer();
    }
    অন্যথা যদি (read_stdin)
    {
	পূর্ণাঙ্ক	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(রিক্ত, রিক্ত, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, রিক্ত,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	যদি (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    যদি (curbuf->b_ml.ml_mfp != রিক্ত
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    যদি (curbuf->b_flags & BF_NEVERLOADED)
    {
	(শূন্য)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    যদি ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != রিক্ত)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != রিক্ত)
#endif
       )
	changed();
    অন্যথা যদি (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    যদি (aborting())
#অন্যথা
    যদি (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    যদি (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, রিক্ত, রিক্ত, FALSE, curbuf, &retval);
#অন্যথা
    apply_autocmds(EVENT_BUFENTER, রিক্ত, রিক্ত, FALSE, curbuf);
#endif

    যদি (retval != OK)
	ঘূৰাই retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    যদি (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != রিক্ত)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	যদি (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    যদি ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, রিক্ত, রিক্ত,
			FALSE, curbuf, &retval);
#অন্যথা
	    apply_autocmds(EVENT_BUFWINENTER, রিক্ত, রিক্ত,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    ঘূৰাই retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    শূন্য
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == রিক্ত ? 0 : buf->b_fnum;
    bufref->br_buf_free_গুনতি = buf_free_গুনতি;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    পূর্ণাঙ্ক
bufref_valid(bufref_T *bufref)
{
    ঘূৰাই bufref->br_buf_free_গুনতি == buf_free_গুনতি
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    পূর্ণাঙ্ক
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	যদি (bp == buf)
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
স্থির hashtab_T buf_hashtab;

    স্থির শূন্য
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((ৱর্ণ *)buf->b_চাবি, পরিমাণ(buf->b_চাবি), "%x", buf->b_fnum);
    যদি (hash_add(&buf_hashtab, buf->b_চাবি, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    স্থির শূন্য
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_চাবি);

    যদি (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    স্থির পূর্ণাঙ্ক
can_unload_buffer(buf_T *buf)
{
    পূর্ণাঙ্ক	    can_unload = !buf->b_locked;

    যদি (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    যদি (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		ভঙ্গ;
	    }
    }
    // Don't unload the buffer while it's still being saved
    যদি (can_unload && buf->b_saving)
	can_unload = FALSE;

    যদি (!can_unload)
    {
	char_u *fname = buf->b_fname != রিক্ত ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != রিক্ত ? fname : (char_u *)"[No Name]");
    }
    ঘূৰাই can_unload;
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
    পূর্ণাঙ্ক
close_buffer(
    win_T	*win,
    buf_T	*buf,
    পূর্ণাঙ্ক		action,
    পূর্ণাঙ্ক		abort_if_last,
    পূর্ণাঙ্ক		ignore_abort,
    পূর্ণাঙ্ক		set_context)
{
    পূর্ণাঙ্ক		hiding_buf;
    bufref_T	bufref;
    পূর্ণাঙ্ক		is_curwin = (curwin != রিক্ত && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    পূর্ণাঙ্ক		unload_buf = (action != 0);
    পূর্ণাঙ্ক		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    পূর্ণাঙ্ক		del_buf = (action == DOBUF_DEL || wipe_buf);
    পূর্ণাঙ্ক		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    পূর্ণাঙ্ক		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    যদি (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    অন্যথা যদি (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    অন্যথা যদি (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    যদি (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	যদি (term_job_running(buf->b_term))
	{
	    যদি (wipe_buf || unload_buf)
	    {
		যদি (!can_unload_buffer(buf))
		    ঘূৰাই FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    অন্যথা
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	অন্যথা যদি (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	অন্যথা
	{
	    যদি (del_buf || unload_buf)
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
    যদি ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	ঘূৰাই FALSE;

    যদি (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	যদি (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    যদি ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	যদি (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    ঘূৰাই FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	যদি (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    গচ্ছ aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	যদি (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    যদি (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		গচ্ছ aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    যদি (abort_if_last && one_window())
		// Autocommands made this the only window.
		গচ্ছ aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	যদি (!ignore_abort && aborting())
	    ঘূৰাই FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    যদি (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    যদি (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    যদি (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    যদি (!hiding_buf || !unload_buf)
	ঘূৰাই FALSE;

    // Always remove the buffer when there is no file name.
    যদি (buf->b_ffname == রিক্ত)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    যদি (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	ঘূৰাই FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    যদি ((win_valid || closed_popup) && win->w_buffer == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_buffer = রিক্ত;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    যদি (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != রিক্ত || buf->b_next != রিক্ত
#যদি defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	যদি (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    যদি (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, পরিমাণ(পূর্ণাঙ্ক), 50);
	    যদি (ga_grow(&buf_reuse, 1) == OK)
		((পূর্ণাঙ্ক *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	যদি (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	অন্যথা
	    buf->b_sfname = রিক্ত;
	VIM_CLEAR(buf->b_ffname);
	যদি (buf->b_prev == রিক্ত)
	    firstbuf = buf->b_next;
	অন্যথা
	    buf->b_prev->b_next = buf->b_next;
	যদি (buf->b_next == রিক্ত)
	    lastbuf = buf->b_prev;
	অন্যথা
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    অন্যথা
    {
	যদি (del_buf)
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
	যদি (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    প্রতিজ্ঞা(!win_valid || !is_curwin || win == curwin);
    ঘূৰাই TRUE;
}

/*
 * Make buffer not contain a file.
 */
    শূন্য
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_সাৰি_গুনতি = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_start_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_start_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_start_bomb = FALSE;
    buf->b_ml.ml_mfp = রিক্ত;
    buf->b_ml.ml_flags = ML_EMPTY;		// empty buffer
#ifdef FEAT_NETBEANS_INTG
    netbeans_deleted_all_সাৰিগুলি(buf);
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
    পূর্ণাঙ্ক
buf_freeall(buf_T *buf, পূর্ণাঙ্ক flags)
{
    পূর্ণাঙ্ক		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    পূর্ণাঙ্ক		is_curwin = (curwin != রিক্ত && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    যদি (buf->b_ml.ml_mfp != রিক্ত)
    {
	যদি (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ঘূৰাই FALSE;
    }
    যদি ((flags & BFA_DEL) && buf->b_p_bl)
    {
	যদি (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ঘূৰাই FALSE;
    }
    যদি (flags & BFA_WIPE)
    {
	যদি (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ঘূৰাই FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    যদি (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    যদি ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	ঘূৰাই FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    যদি (buf == curbuf && !is_curbuf)
	ঘূৰাই FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    যদি (buf == curbuf && VIsual_active
#যদি defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    যদি (curwin != রিক্ত && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    যদি (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_সাৰি_গুনতি = 0;    // no lines in buffer
    যদি ((flags & BFA_KEEP_UNDO) == 0)
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
    ঘূৰাই TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    স্থির শূন্য
free_buffer(buf_T *buf)
{
    ++buf_free_গুনতি;
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

    যদি (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    অন্যথা
    {
	vim_free(buf);
	যদি (curbuf == buf)
	    curbuf = রিক্ত;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    স্থির শূন্য
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_সংখ্যা;
    di->di_tv.v_lock = VAR_FIXED;
    di->di_tv.vval.v_সংখ্যা = 0;

#ifdef FEAT_EVAL
    STRCPY(buf->b_ct_di.di_চাবি, "changedtick");
    (শূন্য)dict_add(buf->b_vars, di);
#endif
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    স্থির শূন্য
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    যেতিয়ালৈ (buf->b_wininfo != রিক্ত)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_next;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    স্থির শূন্য
free_buffer_stuff(
    buf_T	*buf,
    পূর্ণাঙ্ক		free_options)		// free options as well
{
    যদি (free_options)
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
    শূন্য
free_wininfo(wininfo_T *wip)
{
    যদি (wip->wi_optset)
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
    শূন্য
goto_buffer(
    exarg_T	*eap,
    পূর্ণাঙ্ক		start,
    পূর্ণাঙ্ক		dir,
    পূর্ণাঙ্ক		গুনতি)
{
    bufref_T	old_curbuf;
    পূর্ণাঙ্ক		save_sea = swap_exists_action;
    পূর্ণাঙ্ক		skip_help_buf;

    ৱিকল্প (eap->cmdidx)
    {
	পক্ষ CMD_bnext:
	পক্ষ CMD_sbnext:
	পক্ষ CMD_bNext:
	পক্ষ CMD_bprevious:
	পক্ষ CMD_sbNext:
	পক্ষ CMD_sbprevious:
	    skip_help_buf = TRUE;
	    ভঙ্গ;
	স্ৱভাৱ:
	    skip_help_buf = FALSE;
	    ভঙ্গ;
    }

    set_bufref(&old_curbuf, curbuf);

    যদি (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (শূন্য)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, গুনতি,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    যদি (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#যদি defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#যদি defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    অন্যথা
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    শূন্য
handle_swap_exists(bufref_T *old_curbuf)
{
#যদি defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    যদি (swap_exists_action == SEA_QUIT)
    {
#যদি defined(FEAT_EVAL)
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
	যদি (old_curbuf == রিক্ত || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(রিক্ত, রিক্ত, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	অন্যথা
	    buf = old_curbuf->br_buf;
	যদি (buf != রিক্ত)
	{
	    পূর্ণাঙ্ক old_বাৰ্তা_silent = বাৰ্তা_silent;

	    যদি (shortmess(SHM_FILEINFO))
		বাৰ্তা_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    বাৰ্তা_silent = old_বাৰ্তা_silent;

#ifdef FEAT_SYN_HL
	    যদি (old_tw != curbuf->b_p_tw)
		check_colorcolumn(রিক্ত, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#যদি defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    অন্যথা যদি (swap_exists_action == SEA_RECOVER)
    {
#যদি defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Recover at ATTENTION prompt.
	বাৰ্তা_scroll = TRUE;
	ml_recover(FALSE);
	বাৰ্তা_puts("\n");	// don't overwrite the last message
	cmdline_row = বাৰ্তা_row;
	do_modelines(0);

#যদি defined(FEAT_EVAL)
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
    স্থির পূর্ণাঙ্ক
empty_curbuf(
    পূর্ণাঙ্ক close_others,
    পূর্ণাঙ্ক forceit,
    পূর্ণাঙ্ক action)
{
    পূর্ণাঙ্ক	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    যদি (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	ঘূৰাই FAIL;
    }

    set_bufref(&bufref, buf);
    যদি (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, রিক্ত, রিক্ত, রিক্ত, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    যদি (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(রিক্ত, buf, action, FALSE, FALSE, FALSE);
    যদি (!close_others)
	need_fileinfo = FALSE;
    অন্যথা যদি (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    ঘূৰাই retval;
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
    স্থির পূর্ণাঙ্ক
do_buffer_ext(
    পূর্ণাঙ্ক		action,
    পূর্ণাঙ্ক		start,
    পূর্ণাঙ্ক		dir,		// FORWARD or BACKWARD
    পূর্ণাঙ্ক		গুনতি,		// buffer number or number of buffers
    পূর্ণাঙ্ক		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    পূর্ণাঙ্ক		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    ৱিকল্প (start)
    {
	পক্ষ DOBUF_FIRST:   buf = firstbuf; ভঙ্গ;
	পক্ষ DOBUF_LAST:    buf = lastbuf;  ভঙ্গ;
	স্ৱভাৱ:	    buf = curbuf;   ভঙ্গ;
    }
    যদি (start == DOBUF_MOD)	    // find next modified buffer
    {
	যেতিয়ালৈ (গুনতি-- > 0)
	{
	    কৰা
	    {
		buf = buf->b_next;
		যদি (buf == রিক্ত)
		    buf = firstbuf;
	    }
	    যেতিয়ালৈ (buf != curbuf && !bufIsChanged(buf));
	}
	যদি (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    ঘূৰাই FAIL;
	}
    }
    অন্যথা যদি (start == DOBUF_FIRST && গুনতি) // find specified buffer number
    {
	যেতিয়ালৈ (buf != রিক্ত && buf->b_fnum != গুনতি)
	    buf = buf->b_next;
    }
    অন্যথা
    {
	পূর্ণাঙ্ক help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = রিক্ত;
	যেতিয়ালৈ (গুনতি > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    যদি (bp == রিক্ত)
		bp = buf;
	    যদি (dir == FORWARD)
	    {
		buf = buf->b_next;
		যদি (buf == রিক্ত)
		    buf = firstbuf;
	    }
	    অন্যথা
	    {
		buf = buf->b_prev;
		যদি (buf == রিক্ত)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    যদি (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--গুনতি;
		bp = রিক্ত;	// use this buffer as new starting point
	    }
	    যদি (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		ঘূৰাই FAIL;
	    }
	}
    }

    যদি (buf == রিক্ত)	    // could not find it
    {
	যদি (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    যদি (!unload)
		semsg(_(e_buffer_nr_does_not_exist), গুনতি);
	}
	অন্যথা যদি (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	অন্যথা
	    emsg(_(e_cannot_go_before_first_buffer));
	ঘূৰাই FAIL;
    }
#ifdef FEAT_PROP_POPUP
    যদি ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	ঘূৰাই OK;
#endif
    যদি (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	ঘূৰাই FAIL;

    যদি ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), গুনতি);
	ঘূৰাই FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    যদি (unload)
    {
	পূর্ণাঙ্ক	forward;
	bufref_T bufref;

	যদি (!can_unload_buffer(buf))
	    ঘূৰাই FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	যদি (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == রিক্ত && !buf->b_p_bl)
	    ঘূৰাই FAIL;

	যদি ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    যদি ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		যদি (term_job_running(buf->b_term))
		{
		    যদি (term_confirm_stop(buf) == FAIL)
			ঘূৰাই FAIL;
		}
		অন্যথা
# endif
		{
		    dialog_changed(buf, FALSE);
		    যদি (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			ঘূৰাই FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    যদি (bufIsChanged(buf))
			ঘূৰাই FAIL;
		}
	    }
	    অন্যথা
#endif
	    {
		no_write_message_buf(buf);
		ঘূৰাই FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	যদি (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    যদি (bp->b_p_bl && bp != buf)
		ভঙ্গ;
	যদি (bp == রিক্ত && buf == curbuf)
	    ঘূৰাই empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	যেতিয়ালৈ (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != রিক্ত))
	{
	    যদি (win_close(curwin, FALSE) == FAIL)
		ভঙ্গ;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	যদি (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    যদি (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(রিক্ত, buf, action, FALSE, FALSE, FALSE);
	    ঘূৰাই OK;
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
	buf = রিক্ত;	// selected buffer
	bp = রিক্ত;	// used when no loaded buffer found
	যদি (au_new_curbuf.br_buf != রিক্ত && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	অন্যথা যদি (curwin->w_jumplistlen > 0)
	{
	    পূর্ণাঙ্ক     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    যদি (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    যেতিয়ালৈ (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		যদি (buf != রিক্ত)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    যদি (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = রিক্ত;
		    অন্যথা যদি (buf->b_ml.ml_mfp == রিক্ত)
		    {
			// skip unloaded buf, but may keep it for later
			যদি (bp == রিক্ত)
			    bp = buf;
			buf = রিক্ত;
		    }
		}
		যদি (buf != রিক্ত)   // found a valid buffer: stop searching
		    ভঙ্গ;
		// advance to older entry in jump list
		যদি (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    ভঙ্গ;
		যদি (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		যদি (jumpidx == forward)		// List exhausted for sure
		    ভঙ্গ;
	    }
	}

	যদি (buf == রিক্ত)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    বাবে (;;)
	    {
		যদি (buf == রিক্ত)
		{
		    যদি (!forward)	// tried both directions
			ভঙ্গ;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    অনুৱর্ত;
		}
		// in non-help buffer, try to skip help buffers, and vv
		যদি (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    যদি (buf->b_ml.ml_mfp != রিক্ত)   // found loaded buffer
			ভঙ্গ;
		    যদি (bp == রিক্ত)	// remember unloaded buf for later
			bp = buf;
		}
		যদি (forward)
		    buf = buf->b_next;
		অন্যথা
		    buf = buf->b_prev;
	    }
	}
	যদি (buf == রিক্ত)	// No loaded buffer, use unloaded one
	    buf = bp;
	যদি (buf == রিক্ত)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		যদি (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    ভঙ্গ;
	}
	যদি (buf == রিক্ত)	// Still no buffer, just take one
	{
	    যদি (curbuf->b_next != রিক্ত)
		buf = curbuf->b_next;
	    অন্যথা
		buf = curbuf->b_prev;
	    যদি (bt_quickfix(buf)
		    || (buf != রিক্ত && buf != curbuf && buf->b_locked_split))
		buf = রিক্ত;
	}
    }

    যদি (buf == রিক্ত)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	ঘূৰাই empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    যদি (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != রিক্ত)
	ঘূৰাই OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    যদি (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	ঘূৰাই FAIL;
    }
    যদি (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	ঘূৰাই FAIL;

    // go to current buffer - nothing to do
    যদি (buf == curbuf)
	ঘূৰাই OK;

    // Check if the current buffer may be abandoned.
    যদি (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	যদি ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    যদি (term_job_running(curbuf->b_term))
	    {
		যদি (term_confirm_stop(curbuf) == FAIL)
		    ঘূৰাই FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    অন্যথা
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		যদি (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    ঘূৰাই FAIL;

		যদি (bufIsChanged(curbuf))
		{
		    no_write_message();
		    ঘূৰাই FAIL;
		}
	    }
	}
	অন্যথা
#endif
	{
	    no_write_message();
	    ঘূৰাই FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    যদি (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#যদি defined(FEAT_EVAL)
    যদি (aborting())	    // autocmds may abort script processing
	ঘূৰাই FAIL;
#endif

    ঘূৰাই OK;
}

    পূর্ণাঙ্ক
do_buffer(
    পূর্ণাঙ্ক		action,
    পূর্ণাঙ্ক		start,
    পূর্ণাঙ্ক		dir,		// FORWARD or BACKWARD
    পূর্ণাঙ্ক		গুনতি,		// buffer number or number of buffers
    পূর্ণাঙ্ক		forceit)	// TRUE when using !
{
    ঘূৰাই do_buffer_ext(action, start, dir, গুনতি,
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
    ৱর্ণ *
do_bufdel(
    পূর্ণাঙ্ক		command,
    char_u	*arg,		// pointer to extra arguments
    পূর্ণাঙ্ক		addr_গুনতি,
    পূর্ণাঙ্ক		start_bnr,	// first buffer number in a range
    পূর্ণাঙ্ক		end_bnr,	// buffer nr or last buffer nr in a range
    পূর্ণাঙ্ক		forceit)
{
    পূর্ণাঙ্ক		do_current = 0;	// delete current buffer?
    পূর্ণাঙ্ক		deleted = 0;	// number of buffers deleted
    ৱর্ণ	*errormsg = রিক্ত; // return value
    পূর্ণাঙ্ক		bnr;		// buffer number
    char_u	*p;

    যদি (addr_গুনতি == 0)
    {
	(শূন্য)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    অন্যথা
    {
	যদি (addr_গুনতি == 2)
	{
	    যদি (*arg)		// both range and argument is not allowed
		ঘূৰাই ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	অন্যথা	// addr_count == 1
	    bnr = end_bnr;

	বাবে ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    যদি (bnr == curbuf->b_fnum)
		do_current = bnr;
	    অন্যথা যদি (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    যদি (addr_গুনতি == 2)
	    {
		যদি (++bnr > end_bnr)
		    ভঙ্গ;
	    }
	    অন্যথা    // addr_count == 1
	    {
		arg = skipwhite(arg);
		যদি (*arg == NUL)
		    ভঙ্গ;
		যদি (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    যদি (bnr < 0)	    // failed
			ভঙ্গ;
		    arg = p;
		}
		অন্যথা
		    bnr = getdigits(&arg);
	    }
	}
	যদি (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	যদি (deleted == 0)
	{
	    যদি (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    অন্যথা যদি (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    অন্যথা
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (ৱর্ণ *)IObuff;
	}
	অন্যথা যদি (deleted >= p_report)
	{
	    যদি (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    অন্যথা যদি (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    অন্যথা
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    ঘূৰাই errormsg;
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
    শূন্য
set_curbuf(buf_T *buf, পূর্ণাঙ্ক action)
{
    buf_T	*prevbuf;
    পূর্ণাঙ্ক		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    পূর্ণাঙ্ক		valid;

    setpcmark();
    যদি ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    যদি (!apply_autocmds(EVENT_BUFLEAVE, রিক্ত, রিক্ত, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	যদি (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	যদি (unload)
	    close_windows(prevbuf, FALSE);
#যদি defined(FEAT_EVAL)
	যদি (bufref_valid(&prevbufref) && !aborting())
#অন্যথা
	যদি (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    যদি (prevbuf == curbuf
		    && ((State & MODE_ঢোকাও) == 0 || curbuf->b_nwindows <= 1))
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
    যদি ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == রিক্ত)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	যদি (!valid)
	    enter_buffer(lastbuf);
	অন্যথা
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	যদি (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(রিক্ত, curwin);
#endif
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    স্থির শূন্য
enter_buffer(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    যদি (VIsual_active
#যদি defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    যদি (curwin->w_buffer != রিক্ত)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    যদি (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    অন্যথা
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    যদি (curwin->w_p_diff)
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
    যদি (curbuf->b_ml.ml_mfp == রিক্ত)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	যদি (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, রিক্ত, 0);
    }
    অন্যথা
    {
	যদি (!বাৰ্তা_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(শূন্য)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
	apply_autocmds(EVENT_BUFENTER, রিক্ত, রিক্ত, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, রিক্ত, রিক্ত, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    যদি (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    যদি (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    যদি (curbuf->b_kmap_state & KEYMAP_INIT)
	(শূন্য)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    যদি (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(শূন্য)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#যদি defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    শূন্য
do_autochdir(শূন্য)
{
    যদি ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != রিক্ত
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	last_chdir_reason = "autochdir";
    }
}
#endif

    শূন্য
no_write_message_buf(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    যদি (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    অন্যথা
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    শূন্য
no_write_message(শূন্য)
{
#ifdef FEAT_TERMINAL
    যদি (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    অন্যথা
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    শূন্য
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    যদি (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    অন্যথা
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
    পূর্ণাঙ্ক
curbuf_reusable(শূন্য)
{
    ঘূৰাই (curbuf != রিক্ত
	&& curbuf->b_ffname == রিক্ত
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == রিক্ত || BUFEMPTY())
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
    পূর্ণাঙ্ক		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    যদি (শীৰ্ষ_file_অঙ্ক == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    যদি (sfname == রিক্ত || mch_stat((ৱর্ণ *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    যদি (ffname != রিক্ত && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#অন্যথা
		buflist_findname(ffname)
#endif
		) != রিক্ত)
    {
	vim_free(ffname);
	যদি (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? রিক্ত : curwin,
						      lnum, (colnr_T)0, FALSE);

	যদি ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	যদি ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    যদি (!(flags & BLN_DUMMY))
	    {
		যদি (apply_autocmds(EVENT_BUFADD, রিক্ত, রিক্ত, FALSE, buf)
			&& !bufref_valid(&bufref))
		    ঘূৰাই রিক্ত;
	    }
	}
	ঘূৰাই buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = রিক্ত;
    যদি ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	যদি (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    ঘূৰাই রিক্ত;
	}
#endif
	যদি (!bufref_valid(&bufref))
	    buf = রিক্ত;		// buf was deleted; allocate a new buffer
    }
    যদি (buf != curbuf || curbuf == রিক্ত)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	যদি (buf == রিক্ত)
	{
	    vim_free(ffname);
	    ঘূৰাই রিক্ত;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	যদি (buf->b_vars == রিক্ত)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    ঘূৰাই রিক্ত;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    যদি (ffname != রিক্ত)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    যদি ((ffname != রিক্ত && (buf->b_ffname == রিক্ত || buf->b_sfname == রিক্ত))
	    || buf->b_wininfo == রিক্ত)
    {
	যদি (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	অন্যথা
	    buf->b_sfname = রিক্ত;
	VIM_CLEAR(buf->b_ffname);
	যদি (buf != curbuf)
	    free_buffer(buf);
	ঘূৰাই রিক্ত;
    }

    যদি (buf == curbuf)
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
    অন্যথা
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = রিক্ত;
	যদি (firstbuf == রিক্ত)		// buffer list is empty
	{
	    buf->b_prev = রিক্ত;
	    firstbuf = buf;
	}
	অন্যথা				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	যদি ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((পূর্ণাঙ্ক *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    যেতিয়ালৈ (buf->b_prev != রিক্ত && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		যদি (prev->b_next != রিক্ত)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		যদি (buf->b_prev != রিক্ত)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		যদি (lastbuf == buf)
		    lastbuf = prev;
		যদি (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	অন্যথা
	    buf->b_fnum = শীৰ্ষ_file_অঙ্ক++;
	যদি (শীৰ্ষ_file_অঙ্ক < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    যদি (emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    শীৰ্ষ_file_অঙ্ক = 1;
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
    যদি (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    অন্যথা
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    যদি (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    যদি (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	যদি (apply_autocmds(EVENT_BUFNEW, রিক্ত, রিক্ত, FALSE, buf)
		&& !bufref_valid(&bufref))
	    ঘূৰাই রিক্ত;
	যদি (flags & BLN_LISTED)
	{
	    যদি (apply_autocmds(EVENT_BUFADD, রিক্ত, রিক্ত, FALSE, buf)
		    && !bufref_valid(&bufref))
		ঘূৰাই রিক্ত;
	}
#ifdef FEAT_EVAL
	যদি (aborting())		// autocmds may abort script processing
	    ঘূৰাই রিক্ত;
#endif
    }

    ঘূৰাই buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    শূন্য
free_buf_options(
    buf_T	*buf,
    পূর্ণাঙ্ক		free_p_ff)
{
    যদি (free_p_ff)
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
#যদি defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#যদি defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#যদি defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#যদি defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    যদি (buf->b_p_চাবি != রিক্ত && *buf->b_p_চাবি != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_চাবি, STRLEN(buf->b_p_চাবি));
# endif
    clear_string_option(&buf->b_p_চাবি);
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
    buf->b_s.b_cap_prog = রিক্ত;
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
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_গুনতি);
    buf->b_p_cpt_গুনতি = 0;
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
    পূর্ণাঙ্ক
buflist_getfile(
    পূর্ণাঙ্ক		n,
    linenr_T	lnum,
    পূর্ণাঙ্ক		options,
    পূর্ণাঙ্ক		forceit)
{
    buf_T	*buf;
    win_T	*wp = রিক্ত;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    যদি (buf == রিক্ত)
    {
	যদি ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	অন্যথা
	    semsg(_(e_buffer_nr_not_found), n);
	ঘূৰাই FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    যদি (buf == curbuf)
	ঘূৰাই OK;

    যদি (text_or_buf_locked())
	ঘূৰাই FAIL;

    // altfpos may be changed by getfile(), get it now
    যদি (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    অন্যথা
	col = 0;

    যদি (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	যদি (wp == রিক্ত && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    যদি (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    অন্যথা যদি (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		ঘূৰাই FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    পূর্ণাঙ্ক retval = FAIL;
    যদি (GETFILE_SUCCESS(getfile(buf->b_fnum, রিক্ত, রিক্ত,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	যদি (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    যদি (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ঘূৰাই retval;
}

/*
 * go to the last know line number for the current buffer
 */
    স্থির শূন্য
buflist_getfpos(শূন্য)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    যদি (p_sol)
	curwin->w_cursor.col = 0;
    অন্যথা
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
    buf_T	*buf = রিক্ত;

    // First make the name into a full path name
    ffname = FullName_save(fname,
#ifdef UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#অন্যথা
	    FALSE
#endif
	    );
    যদি (ffname != রিক্ত)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    ঘূৰাই buf;
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

    যদি (mch_stat((ৱর্ণ *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    ঘূৰাই buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    স্থির buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#endif
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_LAST(buf)
	যদি ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    ঘূৰাই buf;
    ঘূৰাই রিক্ত;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    পূর্ণাঙ্ক
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    পূর্ণাঙ্ক		unlisted,	// find unlisted buffers
    পূর্ণাঙ্ক		diffmode UNUSED, // find diff-mode buffers only
    পূর্ণাঙ্ক		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    পূর্ণাঙ্ক		match = -1;
    পূর্ণাঙ্ক		find_listed;
    char_u	*pat;
    char_u	*patend;
    পূর্ণাঙ্ক		attempt;
    char_u	*p;
    পূর্ণাঙ্ক		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    যদি ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	যদি (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	অন্যথা
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	যদি (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
    অন্যথা
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, রিক্ত, FALSE);
	যদি (pat == রিক্ত)
	    ঘূৰাই -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	বাবে (;;)
	{
	    বাবে (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		যদি (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		যদি (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    যদি (regmatch.regprog == রিক্ত)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			ঘূৰাই -1;
		    }
		    যদি (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != রিক্ত)
		    {
			যদি (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				যদি (wp->w_buffer == buf)
				    ভঙ্গ;
			    যদি (wp == রিক্ত)
				অনুৱর্ত;
			}
			যদি (match >= 0)		// already found a match
			{
			    match = -2;
			    ভঙ্গ;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		যদি (match >= 0)			// found one match
		    ভঙ্গ;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    যদি (!unlisted || !find_listed || match != -1)
		ভঙ্গ;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    যদি (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    অন্যথা যদি (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    ঘূৰাই match;
}

#ifdef FEAT_VIMINFO
প্রকারনির্দেশ সংরচনা {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    পূর্ণাঙ্ক
ExpandBufnames(
    char_u	*pat,
    পূর্ণাঙ্ক		*অঙ্ক_file,
    char_u	***file,
    পূর্ণাঙ্ক		options)
{
    পূর্ণাঙ্ক		গুনতি;
    buf_T	*buf;
    পূর্ণাঙ্ক		round;
    char_u	*p;
    char_u	*patc = রিক্ত;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = রিক্ত;
#endif
    পূর্ণাঙ্ক		fuzzy;
    fuzmatch_str_T  *fuzmatch = রিক্ত;
    regmatch_T	regmatch;
    পূর্ণাঙ্ক		score = 0;
    পূর্ণাঙ্ক		to_free = FALSE;

    *অঙ্ক_file = 0;		    // return values in case of FAIL
    *file = রিক্ত;

#ifdef FEAT_DIFF
    যদি ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	ঘূৰাই FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    যদি (!fuzzy)
    {
	যদি (*pat == '^' && pat[1] != NUL)
	{
	    পূর্ণাঙ্ক  len = (পূর্ণাঙ্ক)STRLEN(pat);
	    patc = alloc(len);
	    যদি (patc == রিক্ত)
		ঘূৰাই FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	অন্যথা যদি (*pat == '^')
	    patc = (char_u *)"";
	অন্যথা
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    বাবে (round = 1; round <= 2; ++round)
    {
	গুনতি = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    যদি (!buf->b_p_bl)	// skip unlisted buffers
		অনুৱর্ত;
#ifdef FEAT_DIFF
	    যদি (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		যদি (buf == curbuf || !diff_mode_buf(buf))
		    অনুৱর্ত;
#endif

	    যদি (!fuzzy)
	    {
		যদি (regmatch.regprog == রিক্ত)
		{
		    // invalid pattern, possibly after recompiling
		    যদি (to_free)
			vim_free(patc);
		    ঘূৰাই FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    অন্যথা
	    {
		p = রিক্ত;
		// first try matching with the short file name
		যদি ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		যদি (p == রিক্ত)
		{
		    // next try matching with the full path file name
		    যদি ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    যদি (p == রিক্ত)
		অনুৱর্ত;

	    যদি (round == 1)
	    {
		++গুনতি;
		অনুৱর্ত;
	    }

	    যদি (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    অন্যথা
		p = vim_strsave(p);
	    যদি (p == রিক্ত)
	    {
		যদি (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, গুনতি);
		ঘূৰাই FAIL;
	    }

	    যদি (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		যদি (matches != রিক্ত)
		{
		    matches[গুনতি].buf = buf;
		    matches[গুনতি].match = p;
		    গুনতি++;
		}
		অন্যথা
#endif
		    (*file)[গুনতি++] = p;
	    }
	    অন্যথা
	    {
		fuzmatch[গুনতি].idx = গুনতি;
		fuzmatch[গুনতি].str = p;
		fuzmatch[গুনতি].score = score;
		গুনতি++;
	    }
	}
	যদি (গুনতি == 0)	// no match found, break here
	    ভঙ্গ;
	যদি (round == 1)
	{
	    যদি (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, গুনতি);
		যদি (*file == রিক্ত)
		{
		    vim_regfree(regmatch.regprog);
		    যদি (to_free)
			vim_free(patc);
		    ঘূৰাই FAIL;
		}
#ifdef FEAT_VIMINFO
		যদি (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, গুনতি);
#endif
	    }
	    অন্যথা
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, গুনতি);
		যদি (fuzmatch == রিক্ত)
		{
		    *অঙ্ক_file = 0;
		    *file = রিক্ত;
		    ঘূৰাই FAIL;
		}
	    }
	}
    }

    যদি (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	যদি (to_free)
	    vim_free(patc);
    }

    যদি (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	যদি (matches != রিক্ত)
	{
	    পূর্ণাঙ্ক i;
	    যদি (গুনতি > 1)
		qsort(matches, গুনতি, পরিমাণ(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    যদি (matches[0].buf == curbuf)
	    {
		বাবে (i = 1; i < গুনতি; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[গুনতি-1] = matches[0].match;
	    }
	    অন্যথা
	    {
		বাবে (i = 0; i < গুনতি; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    অন্যথা
    {
	যদি (fuzzymatches_to_strmatches(fuzmatch, file, গুনতি, FALSE) == FAIL)
	    ঘূৰাই FAIL;
    }

    *অঙ্ক_file = গুনতি;
    ঘূৰাই (গুনতি == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    স্থির char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    পূর্ণাঙ্ক		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    যদি (match == রিক্ত && rmp->regprog != রিক্ত)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    ঘূৰাই match;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    স্থির char_u *
fname_match(
    regmatch_T	*rmp,
    char_u	*name,
    পূর্ণাঙ্ক		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = রিক্ত;
    char_u	*p;

    // extra check for valid arguments
    যদি (name == রিক্ত || rmp->regprog == রিক্ত)
	ঘূৰাই রিক্ত;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    যদি (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    অন্যথা যদি (rmp->regprog != রিক্ত)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(রিক্ত, name);
	যদি (p != রিক্ত && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    ঘূৰাই match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(পূর্ণাঙ্ক nr)
{
    char_u	চাবি[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    যদি (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((ৱর্ণ *)চাবি, পরিমাণ(চাবি), "%x", nr);
    hi = hash_find(&buf_hashtab, চাবি);

    যদি (!HASHITEM_EMPTY(hi))
	ঘূৰাই (buf_T *)(hi->hi_চাবি
			     - ((unsigned)(curbuf->b_চাবি - (char_u *)curbuf)));
    ঘূৰাই রিক্ত;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    পূর্ণাঙ্ক		n,
    পূর্ণাঙ্ক		fullname,
    পূর্ণাঙ্ক		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    যদি (buf == রিক্ত)
	ঘূৰাই রিক্ত;
    ঘূৰাই home_replace_save(helptail ? buf : রিক্ত,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    শূন্য
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	col,
    পূর্ণাঙ্ক		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	যদি (wip->wi_win == win)
	    ভঙ্গ;
    যদি (wip == রিক্ত)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	যদি (wip == রিক্ত)
	    ঘূৰাই;
	wip->wi_win = win;
	যদি (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    অন্যথা
    {
	// remove the entry from the list
	যদি (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	অন্যথা
	    buf->b_wininfo = wip->wi_next;
	যদি (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	যদি (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    যদি (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    যদি (win != রিক্ত)
	wip->wi_changelistidx = win->w_changelistidx;
    যদি (copy_options && win != রিক্ত)
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
    wip->wi_prev = রিক্ত;
    যদি (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    স্থির পূর্ণাঙ্ক
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    যদি (!wip->wi_opt.wo_diff)
	ঘূৰাই FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	যদি (wip->wi_win == wp)
	    ঘূৰাই FALSE;
    ঘূৰাই TRUE;
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
    স্থির wininfo_T *
find_wininfo(
    buf_T	*buf,
    পূর্ণাঙ্ক		need_options,
    পূর্ণাঙ্ক		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	যদি (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    ভঙ্গ;

    যদি (wip != রিক্ত)
	ঘূৰাই wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    যদি (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    যদি (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != রিক্ত
			    && wip->wi_win->w_buffer == buf)))
		ভঙ্গ;
    }
    অন্যথা
#endif
	wip = buf->b_wininfo;
    ঘূৰাই wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    শূন্য
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
    clearFolding(curwin);
#endif

    wip = find_wininfo(buf, TRUE, TRUE);
    যদি (wip != রিক্ত && wip->wi_win != রিক্ত
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
    অন্যথা যদি (wip != রিক্ত && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    অন্যথা
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    যদি (wip != রিক্ত)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    যদি (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    যদি (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    যদি (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	যদি (p_ea)
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
    স্থির pos_T no_position = {1, 0, 0};

    wip = find_wininfo(buf, FALSE, FALSE);
    যদি (wip != রিক্ত)
	ঘূৰাই &(wip->wi_fpos);
    অন্যথা
	ঘূৰাই &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    ঘূৰাই buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    শূন্য
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		ro_char;
    পূর্ণাঙ্ক		changed_char;
#ifdef FEAT_TERMINAL
    পূর্ণাঙ্ক		job_running;
    পূর্ণাঙ্ক		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = রিক্ত, **p;

    যদি (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, পরিমাণ(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    যদি (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_data, (size_t)buflist.ga_len,
		পরিমাণ(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    বাবে (; buf != রিক্ত && !got_int; buf = buflist_data != রিক্ত
	    ? (++p < buflist_data + buflist.ga_len ? *p : রিক্ত)
	    : buf->b_next)
#অন্যথা
    বাবে (buf = firstbuf; buf != রিক্ত && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	যদি ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !bufIsChanged(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == রিক্ত || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == রিক্ত || buf->b_nwindows != 0))
#ifdef FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!job_running || (job_running && job_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!job_running || (job_running && !job_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (job_running || buf->b_term == রিক্ত))
#endif
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    অনুৱর্ত;
	name = buf_spname(buf);
	যদি (name != রিক্ত)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	অন্যথা
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	যদি (message_filtered(NameBuff))
	    অনুৱর্ত;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	যদি (job_running)
	{
	    যদি (job_none_open)
		ro_char = '?';
	    অন্যথা
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	অন্যথা যদি (buf->b_term != রিক্ত)
	    ro_char = 'F';
	অন্যথা
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	বাৰ্তা_putchar('\n');
	len = (পূর্ণাঙ্ক)vim_snprintf_safelen((ৱর্ণ *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == রিক্ত ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		changed_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	কৰা
	    IObuff[len++] = ' ';
	যেতিয়ালৈ (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	যদি (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (size_t)(IOSIZE - len), buf->b_last_used);
	অন্যথা
#endif
	    vim_snprintf((ৱর্ণ *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (long)buflist_findlnum(buf));
	বাৰ্তা_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    যদি (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    পূর্ণাঙ্ক
buflist_name_nr(
    পূর্ণাঙ্ক		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    যদি (buf == রিক্ত || buf->b_fname == রিক্ত)
	ঘূৰাই FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    ঘূৰাই OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    পূর্ণাঙ্ক
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    পূর্ণাঙ্ক		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = রিক্ত;
#ifdef UNIX
    stat_T	st;
#endif

    যদি (ffname == রিক্ত || *ffname == NUL)
    {
	// Removing the name.
	যদি (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	অন্যথা
	    buf->b_sfname = রিক্ত;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    অন্যথা
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	যদি (ffname == রিক্ত)		    // out of memory
	    ঘূৰাই FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	যদি (mch_stat((ৱর্ণ *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	যদি (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#অন্যথা
	    obuf = buflist_findname(ffname);
#endif
	যদি (obuf != রিক্ত && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    পূর্ণাঙ্ক		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		যদি (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    যদি (obuf->b_ml.ml_mfp != রিক্ত || in_use)
	    {
		যদি (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		ঘূৰাই FAIL;
	    }
	    // delete from the list
	    close_buffer(রিক্ত, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	যদি (ffname == রিক্ত || sfname == রিক্ত)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    ঘূৰাই FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	যদি (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    যদি (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    অন্যথা
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    ঘূৰাই OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    শূন্য
buf_set_name(পূর্ণাঙ্ক fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    যদি (buf == রিক্ত)
	ঘূৰাই;

    যদি (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = রিক্ত;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    শূন্য
buf_name_changed(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    যদি (buf->b_ml.ml_mfp != রিক্ত)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    যদি (buf->b_term != রিক্ত)
	term_clear_status_text(buf->b_term);
#endif

    যদি (curwin->w_buffer == buf)
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
    যদি (buf != রিক্ত && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    ঘূৰাই buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    পূর্ণাঙ্ক		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    যদি (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	যদি (errmsg)
	    emsg(_(e_no_alternate_file));
	ঘূৰাই রিক্ত;
    }
    ঘূৰাই fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    পূর্ণাঙ্ক
buflist_add(char_u *fname, পূর্ণাঙ্ক flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, রিক্ত, (linenr_T)0, flags);
    যদি (buf != রিক্ত)
	ঘূৰাই buf->b_fnum;
    ঘূৰাই 0;
}

#যদি defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    শূন্য
buflist_slash_adjust(শূন্য)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	যদি (bp->b_ffname != রিক্ত)
	    slash_adjust(bp->b_ffname);
	যদি (bp->b_sfname != রিক্ত)
	    slash_adjust(bp->b_sfname);
    }
}
#endif

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    শূন্য
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.col, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    পূর্ণাঙ্ক
otherfile(char_u *ffname)
{
    ঘূৰাই otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , রিক্ত
#endif
	    );
}

    স্থির পূর্ণাঙ্ক
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    যদি (ffname == রিক্ত || *ffname == NUL || buf->b_ffname == রিক্ত)
	ঘূৰাই TRUE;
    যদি (fnamecmp(ffname, buf->b_ffname) == 0)
	ঘূৰাই FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	যদি (stp == রিক্ত)
	{
	    যদি (!buf->b_dev_valid || mch_stat((ৱর্ণ *)ffname, &st) < 0)
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
	যদি (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    যদি (buf_same_ino(buf, stp))
		ঘূৰাই FALSE;
	}
    }
#endif
    ঘূৰাই TRUE;
}

#যদি defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    শূন্য
buf_setino(buf_T *buf)
{
    stat_T	st;

    যদি (buf->b_fname != রিক্ত && mch_stat((ৱর্ণ *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    অন্যথা
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    স্থির পূর্ণাঙ্ক
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    ঘূৰাই (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    শূন্য
fileinfo(
    পূর্ণাঙ্ক fullname,	    // when non-zero print full path
    পূর্ণাঙ্ক shorthelp,
    পূর্ণাঙ্ক	dont_truncate)
{
    char_u	*name;
    ৱর্ণ	*buffer;
    size_t	bufferlen = 0;

    buffer = alloc(IOSIZE);
    যদি (buffer == রিক্ত)
	ঘূৰাই;

    যদি (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    যদি (name != রিক্ত)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    অন্যথা
    {
	যদি (!fullname && curbuf->b_fname != রিক্ত)
	    name = curbuf->b_fname;
	অন্যথা
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : রিক্ত, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (পূর্ণাঙ্ক)bufferlen, TRUE);
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

    যদি (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_সাৰিগুলি_বাৰ্তা));
    অন্যথা যদি (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_সাৰি_গুনতি),
	    (long)curbuf->b_ml.ml_সাৰি_গুনতি,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_সাৰি_গুনতি));
    অন্যথা
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (long)curwin->w_cursor.lnum,
	    (long)curbuf->b_ml.ml_সাৰি_গুনতি,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_সাৰি_গুনতি));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (পূর্ণাঙ্ক)curwin->w_cursor.col + 1, (পূর্ণাঙ্ক)curwin->w_virtcol + 1);
    }

    (শূন্য)append_arg_সংখ্যা(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    যদি (dont_truncate)
    {
	পূর্ণাঙ্ক	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	বাৰ্তা_start();
	n = বাৰ্তা_scroll;
	বাৰ্তা_scroll = TRUE;
	বাৰ্তা(buffer);
	বাৰ্তা_scroll = n;
    }
    অন্যথা
    {
	ৱর্ণ	*p = বাৰ্তা_trunc_attr(buffer, FALSE, 0);
	যদি (restart_edit != 0 || (বাৰ্তা_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_বাৰ্তা((char_u *)p, 0);
    }

    vim_free(buffer);
}

    পূর্ণাঙ্ক
col_print(
    char_u  *buf,
    size_t  buflen,
    পূর্ণাঙ্ক	    col,
    পূর্ণাঙ্ক	    vcol)
{
    যদি (col == vcol)
	ঘূৰাই (পূর্ণাঙ্ক)vim_snprintf_safelen((ৱর্ণ *)buf, buflen, "%d", col);

    ঘূৰাই (পূর্ণাঙ্ক)vim_snprintf_safelen((ৱর্ণ *)buf, buflen, "%d-%d", col, vcol);
}

স্থির char_u *lasttitle = রিক্ত;
স্থির char_u *lasticon = রিক্ত;

/*
 * Put the file name in the title bar and icon of the window.
 */
    শূন্য
maketitle(শূন্য)
{
    char_u	*title_str = রিক্ত;
    char_u	*icon_str = রিক্ত;
    পূর্ণাঙ্ক		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    যদি (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	ঘূৰাই;
    }

    need_maketitle = FALSE;
    যদি (!p_title && !p_icon && lasttitle == রিক্ত && lasticon == রিক্ত)
	ঘূৰাই;  // nothing to do

    যদি (p_title)
    {
	পূর্ণাঙ্ক maxlen = 0;

	যদি (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    যদি (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	যদি (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    যদি (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, পরিমাণ(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, রিক্ত, রিক্ত, রিক্ত);
	    অন্যথা
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	অন্যথা
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (পরিমাণ(buf) - 100)
#define SPACE_FOR_DIR   (পরিমাণ(buf) - 20)
#define SPACE_FOR_ARGNR (পরিমাণ(buf) - 10)  // at least room for " - VIM"

	    // file name
	    যদি (curbuf->b_fname == রিক্ত)
		buflen = vim_snprintf_safelen((ৱর্ণ *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    অন্যথা যদি (curbuf->b_term != রিক্ত)
		buflen = vim_snprintf_safelen((ৱর্ণ *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    অন্যথা
	    {
		buflen = vim_snprintf_safelen((ৱর্ণ *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != রিক্ত)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    যদি (curbuf->b_term == রিক্ত)
#endif
	    {
		ৱিকল্প (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    পক্ষ 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (ৱর্ণ *)buf + buflen,
			    পরিমাণ(buf) - buflen, " +");
			ভঙ্গ;
		    পক্ষ 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (ৱর্ণ *)buf + buflen,
			    পরিমাণ(buf) - buflen, " =");
			ভঙ্গ;
		    পক্ষ 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (ৱর্ণ *)buf + buflen,
			    পরিমাণ(buf) - buflen, " =+");
			ভঙ্গ;
		    পক্ষ 4:
		    পক্ষ 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (ৱর্ণ *)buf + buflen,
			    পরিমাণ(buf) - buflen, " -");
			ভঙ্গ;
		    পক্ষ 5:
		    পক্ষ 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (ৱর্ণ *)buf + buflen,
			    পরিমাণ(buf) - buflen, " -+");
			ভঙ্গ;
		    স্ৱভাৱ:
			ভঙ্গ;
		}
	    }

	    // path (surrounded by '()')
	    যদি (curbuf->b_fname != রিক্ত
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == রিক্ত
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((ৱর্ণ *)buf + buflen,
		    পরিমাণ(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (পূর্ণাঙ্ক)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		যদি (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		যদি (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((ৱর্ণ *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		অন্যথা
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    যদি (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((ৱর্ণ *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != রিক্ত)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    অন্যথা
			buflen += vim_snprintf_safelen((ৱর্ণ *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_সংখ্যা(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((ৱর্ণ *)buf + buflen,
		পরিমাণ(buf) - buflen, " - %s",
#যদি defined(FEAT_CLIENTSERVER)
		(serverName != রিক্ত)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    যদি (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		যদি (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, পরিমাণ(buf));
	    }
	}
    }
    mustset = মান_changed(title_str, &lasttitle);

    যদি (p_icon)
    {
	icon_str = buf;
	যদি (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    যদি (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, পরিমাণ(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, রিক্ত, রিক্ত,
				 রিক্ত);
	    অন্যথা
#endif
		icon_str = p_iconstring;
	}
	অন্যথা
	{
	    char_u  *name;
	    পূর্ণাঙ্ক	    namelen;

	    name = buf_spname(curbuf);
	    যদি (name == রিক্ত)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (পূর্ণাঙ্ক)STRLEN(name);
	    যদি (namelen > 100)
	    {
		namelen -= 100;
		যদি (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, পরিমাণ(buf));
	}
    }

    mustset |= মান_changed(icon_str, &lasticon);

    যদি (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    স্থির পূর্ণাঙ্ক
মান_changed(char_u *str, char_u **last)
{
    যদি ((str == রিক্ত) != (*last == রিক্ত)
	    || (str != রিক্ত && *last != রিক্ত && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	যদি (str == রিক্ত)
	{
	    *last = রিক্ত;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	অন্যথা
	{
	    *last = vim_strsave(str);
	    ঘূৰাই TRUE;
	}
    }
    ঘূৰাই FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    শূন্য
resettitle(শূন্য)
{
    mch_settitle(lasttitle, lasticon);
}

#যদি defined(EXITFREE)
    শূন্য
free_titles(শূন্য)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#যদি defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
প্রকারনির্দেশ সংরচনা
{
    char_u	*stl_start;
    পূর্ণাঙ্ক		stl_minwid;
    পূর্ণাঙ্ক		stl_maxwid;
    গণনা {
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

স্থির size_t		stl_বস্তুসমূহ_len = 20; // Initial value, grows as needed.
স্থির stl_item_T      *stl_বস্তুসমূহ = রিক্ত;
স্থির পূর্ণাঙ্ক	       *stl_groupitem = রিক্ত;
স্থির stl_hlrec_T     *stl_hltab = রিক্ত;
স্থির stl_hlrec_T     *stl_tabtab = রিক্ত;
স্থির stl_clickrec_T  *stl_clicktab = রিক্ত;
স্থির পূর্ণাঙ্ক		*stl_separator_locations = রিক্ত;

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
    পূর্ণাঙ্ক
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    পূর্ণাঙ্ক		opt_scope,	// scope for "opt_name"
    পূর্ণাঙ্ক		fillchar,
    পূর্ণাঙ্ক		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    ঘূৰাই build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    রিক্ত, রিক্ত);
}

    পূর্ণাঙ্ক
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    পূর্ণাঙ্ক		opt_scope,	// scope for "opt_name"
    পূর্ণাঙ্ক		fillchar,
    পূর্ণাঙ্ক		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    পূর্ণাঙ্ক		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ঘূৰাই build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    রিক্ত, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    পূর্ণাঙ্ক
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    পূর্ণাঙ্ক		opt_scope,	// scope for "opt_name"
    পূর্ণাঙ্ক		fillchar,
    পূর্ণাঙ্ক		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    পূর্ণাঙ্ক		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ঘূৰাই build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    রিক্ত, carry_hl);
}
# endif

    পূর্ণাঙ্ক
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    পূর্ণাঙ্ক		opt_scope)	// scope for "opt_name"
{
    পূর্ণাঙ্ক rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (শূন্য)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, পরিমাণ(buf), &fmt,
	    opt_name, opt_scope, 0, 0, রিক্ত, রিক্ত, রিক্ত, &rendered_height,
	    রিক্ত);
    --emsg_off;
    ঘূৰাই rendered_height;
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
স্থির পূর্ণাঙ্ক
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    পূর্ণাঙ্ক		opt_scope,	// scope for "opt_name"
    পূর্ণাঙ্ক		fillchar,
    পূর্ণাঙ্ক		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    পূর্ণাঙ্ক		*rendered_height,   // return: stl rendered height (can be NULL)
    পূর্ণাঙ্ক		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    পূর্ণাঙ্ক		byteval;
# ifdef FEAT_EVAL
    পূর্ণাঙ্ক		use_sandbox;
    পূর্ণাঙ্ক		save_VIsual_active;
# endif
    পূর্ণাঙ্ক		empty_সাৰি;
    long	l;
    long	n;
    পূর্ণাঙ্ক		prevchar_isflag;
    পূর্ণাঙ্ক		prevchar_isitem;
    পূর্ণাঙ্ক		itemisflag;
    পূর্ণাঙ্ক		fillable;
    char_u	*str;
    long	অঙ্ক;
    পূর্ণাঙ্ক		width;
    পূর্ণাঙ্ক		itemcnt;
    পূর্ণাঙ্ক		curitem;
    পূর্ণাঙ্ক		groupdepth;
# ifdef FEAT_EVAL
    পূর্ণাঙ্ক		evaldepth;
# endif
    পূর্ণাঙ্ক		minwid;
    পূর্ণাঙ্ক		maxwid;
    পূর্ণাঙ্ক		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    পূর্ণাঙ্ক		save_redraw_not_allowed = redraw_not_allowed;
    পূর্ণাঙ্ক		save_চাবি_Typed = চাবি_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;
    পূর্ণাঙ্ক		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    যদি (updating_screen)
	redraw_not_allowed = TRUE;

    যদি (stl_বস্তুসমূহ == রিক্ত)
    {
	stl_বস্তুসমূহ = ALLOC_MULT(stl_item_T, stl_বস্তুসমূহ_len);
	stl_groupitem = ALLOC_MULT(পূর্ণাঙ্ক, stl_বস্তুসমূহ_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_বস্তুসমূহ_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_বস্তুসমূহ_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_বস্তুসমূহ_len + 1);

	stl_separator_locations = ALLOC_MULT(পূর্ণাঙ্ক, stl_বস্তুসমূহ_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    যদি (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_সংখ্যা;
	tv.vval.v_সংখ্যা = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	যদি (usefmt == রিক্ত)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    যদি (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    যদি (lnum > wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
    {
	lnum = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_সাৰি = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    যদি (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    অন্যথা
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    যদি (carry_hl != রিক্ত && *carry_hl != 0)
    {
	stl_বস্তুসমূহ[curitem].stl_type = Highlight;
	stl_বস্তুসমূহ[curitem].stl_start = p;
	stl_বস্তুসমূহ[curitem].stl_minwid = *carry_hl;
	stl_বস্তুসমূহ[curitem].stl_clickfunc = রিক্ত;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    বাবে (s = usefmt; *s != NUL; )
    {
	যদি (curitem == (পূর্ণাঙ্ক)stl_বস্তুসমূহ_len)
	{
	    size_t	new_len = stl_বস্তুসমূহ_len * 3 / 2;

	    stl_item_T *new_বস্তুসমূহ =
			  vim_realloc(stl_বস্তুসমূহ, পরিমাণ(stl_item_T) * new_len);
	    যদি (new_বস্তুসমূহ == রিক্ত)
		ভঙ্গ;
	    stl_বস্তুসমূহ = new_বস্তুসমূহ;

	    পূর্ণাঙ্ক *new_groupitem =
			     vim_realloc(stl_groupitem, পরিমাণ(পূর্ণাঙ্ক) * new_len);
	    যদি (new_groupitem == রিক্ত)
		ভঙ্গ;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  পরিমাণ(stl_hlrec_T) * (new_len + 1));
	    যদি (new_hlrec == রিক্ত)
		ভঙ্গ;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  পরিমাণ(stl_hlrec_T) * (new_len + 1));
	    যদি (new_hlrec == রিক্ত)
		ভঙ্গ;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      পরিমাণ(stl_clickrec_T) * (new_len + 1));
	    যদি (new_clickrec == রিক্ত)
		ভঙ্গ;
	    stl_clicktab = new_clickrec;

	    পূর্ণাঙ্ক *new_separator_locs = vim_realloc(stl_separator_locations,
					    পরিমাণ(পূর্ণাঙ্ক) * new_len);
	    যদি (new_separator_locs == রিক্ত)
		ভঙ্গ;
	    stl_separator_locations = new_separator_locs;

	    stl_বস্তুসমূহ_len = new_len;
	}

	stl_বস্তুসমূহ[curitem].stl_clickfunc = রিক্ত;

	যদি (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	যেতিয়ালৈ (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    যদি (*s == '\n' || *s == '\r')
	    {
		যদি (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    গচ্ছ find_linebreak;
		}
		অন্যথা যদি (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	যদি (*s == NUL || p + 1 >= out + outlen)
	    ভঙ্গ;

	/*
	 * Handle one '%' item.
	 */
	s++;
	যদি (*s == NUL)  // ignore trailing %
	    ভঙ্গ;

	যদি (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    যদি (s[1] == ']')
	    {
		stl_বস্তুসমূহ[curitem].stl_type = ClickFunc;
		stl_বস্তুসমূহ[curitem].stl_start = p;
		stl_বস্তুসমূহ[curitem].stl_minwid = 0;
		stl_বস্তুসমূহ[curitem].stl_clickfunc = রিক্ত;
		s += 2;
		curitem++;
		অনুৱর্ত;
	    }
	    // %[FuncName] - start click region
	    যদি (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		যদি (rb != রিক্ত)
		{
		    stl_বস্তুসমূহ[curitem].stl_type = ClickFunc;
		    stl_বস্তুসমূহ[curitem].stl_start = p;
		    stl_বস্তুসমূহ[curitem].stl_minwid = 0;
		    stl_বস্তুসমূহ[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    অনুৱর্ত;
		}
	    }
	}
	যদি (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    যদি (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		ভঙ্গ;
	    }
	    অন্যথা যদি (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	যদি (*s == '%')
	{
	    যদি (p + 1 >= out + outlen)
		ভঙ্গ;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    অনুৱর্ত;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	যদি (*s == STL_SEPARATE)
	{
	    s++;
	    যদি (groupdepth > 0)
		অনুৱর্ত;
	    stl_বস্তুসমূহ[curitem].stl_type = Separate;
	    stl_বস্তুসমূহ[curitem++].stl_start = p;
	    অনুৱর্ত;
	}
	যদি (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_বস্তুসমূহ[curitem].stl_type = Trunc;
	    stl_বস্তুসমূহ[curitem++].stl_start = p;
	    অনুৱর্ত;
	}
	যদি (*s == ')')
	{
	    char_u  *t;

	    s++;
	    যদি (groupdepth < 1)
		অনুৱর্ত;
	    groupdepth--;

	    t = stl_বস্তুসমূহ[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    যদি (curitem > stl_groupitem[groupdepth] + 1
		    && stl_বস্তুসমূহ[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		পূর্ণাঙ্ক group_start_userhl = 0;
		পূর্ণাঙ্ক group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		বাবে (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    যদি (stl_বস্তুসমূহ[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_বস্তুসমূহ[n].stl_minwid;
			ভঙ্গ;
		    }
		}
		বাবে (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    যদি (stl_বস্তুসমূহ[n].stl_type == Normal)
			ভঙ্গ;
		    যদি (stl_বস্তুসমূহ[n].stl_type == Highlight)
			group_end_userhl = stl_বস্তুসমূহ[n].stl_minwid;
		}
		যদি (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    বাবে (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			যদি (stl_বস্তুসমূহ[n].stl_type == Highlight)
			    stl_বস্তুসমূহ[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			যদি (stl_বস্তুসমূহ[n].stl_type == TabPage)
			    stl_বস্তুসমূহ[n].stl_start = p;
		    }
		}
	    }
	    যদি (l > stl_বস্তুসমূহ[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		যদি (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    যেতিয়ালৈ (l >= stl_বস্তুসমূহ[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		অন্যথা
		    n = (long)(p - t) - stl_বস্তুসমূহ[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		পূর্ণাঙ্ক minwid_fixed = MIN(stl_বস্তুসমূহ[stl_groupitem[groupdepth]].stl_minwid,
				       stl_বস্তুসমূহ[stl_groupitem[groupdepth]].stl_maxwid);
		যেতিয়ালৈ (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		বাবে (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_বস্তুসমূহ[l].stl_start -= n - 1;
		    যদি (stl_বস্তুসমূহ[l].stl_start < t)
			stl_বস্তুসমূহ[l].stl_start = t;
		}
	    }
	    অন্যথা যদি (abs(stl_বস্তুসমূহ[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_বস্তুসমূহ[stl_groupitem[groupdepth]].stl_minwid;
		পূর্ণাঙ্ক fillchar_len = MB_CHAR2LEN(fillchar);
		যদি (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    যেতিয়ালৈ (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		অন্যথা
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    যদি (p + l >= out + outlen)
		    {
			n = (long)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    বাবে ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    বাবে (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_বস্তুসমূহ[n].stl_start += l;
		}
	    }
	    অনুৱর্ত;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	যদি (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	যদি (*s == '-')
	{
	    s++;
	    l = -1;
	}
	যদি (VIM_ISDIGIT(*s))
	{
	    minwid = (পূর্ণাঙ্ক)getdigits(&s);
	    যদি (minwid < 0)	// overflow
		minwid = 0;
	}
	যদি (*s == STL_USER_HL)
	{
	    stl_বস্তুসমূহ[curitem].stl_type = Highlight;
	    stl_বস্তুসমূহ[curitem].stl_start = p;
	    stl_বস্তুসমূহ[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    অনুৱর্ত;
	}
	যদি (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    যদি (*s == STL_TABCLOSENR)
	    {
		যদি (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    বাবে (n = curitem - 1; n >= 0; --n)
			যদি (stl_বস্তুসমূহ[n].stl_type == TabPage
					       && stl_বস্তুসমূহ[n].stl_minwid >= 0)
			{
			    minwid = stl_বস্তুসমূহ[n].stl_minwid;
			    ভঙ্গ;
			}
		}
		অন্যথা
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_বস্তুসমূহ[curitem].stl_type = TabPage;
	    stl_বস্তুসমূহ[curitem].stl_start = p;
	    stl_বস্তুসমূহ[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    অনুৱর্ত;
	}
	যদি (*s == '.')
	{
	    s++;
	    যদি (VIM_ISDIGIT(*s))
	    {
		maxwid = (পূর্ণাঙ্ক)getdigits(&s);
		যদি (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	পূর্ণাঙ্ক raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	যদি (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_বস্তুসমূহ[curitem].stl_type = Group;
	    stl_বস্তুসমূহ[curitem].stl_start = p;
	    stl_বস্তুসমূহ[curitem].stl_minwid = minwid;
	    stl_বস্তুসমূহ[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    অনুৱর্ত;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	যদি (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    অনুৱর্ত;
	}
# endif
	যদি (vim_strchr(STL_ALL, *s) == রিক্ত)
	{
	    যদি (*s == NUL)  // can happen with "%0"
		ভঙ্গ;
	    s++;
	    অনুৱর্ত;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	অঙ্ক = -1;
	str = রিক্ত;
	ৱিকল্প (opt)
	{
	পক্ষ STL_FILEPATH:
	পক্ষ STL_FULLPATH:
	পক্ষ STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    যদি (name != রিক্ত)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    অন্যথা
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    যদি (opt != STL_FILENAME)
		str = NameBuff;
	    অন্যথা
		str = gettail(NameBuff);
	    ভঙ্গ;
	}

	পক্ষ STL_VIM_ৰাশিমালা: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    পূর্ণাঙ্ক	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    যদি (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    যেতিয়ালৈ ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    যদি (*s != '}')	// missing '}' or out of space
		ভঙ্গ;
	    s++;
	    যদি (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    অন্যথা
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((ৱর্ণ *)buf_tmp, পরিমাণ(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((ৱর্ণ *)buf_tmp, পরিমাণ(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    যদি (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    যদি (!zeropad && str != রিক্ত && *str != NUL)
	    {
		যদি (*skipdigits(str) == NUL)
		{
		    অঙ্ক = atoi((ৱর্ণ *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    যদি (reevaluate && str != রিক্ত && *str != NUL
		    && strchr((ধ্রুৱ ৱর্ণ *)str, '%') != রিক্ত
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((ধ্রুৱ ৱর্ণ *)str);
		size_t fmt_length = strlen((ধ্রুৱ ৱর্ণ *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * পরিমাণ(char_u));

		যদি (new_fmt != রিক্ত)
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
		    new_fmt_p = রিক্ত;

		    যদি (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    অনুৱর্ত;
		}
	    }
# endif
	    ভঙ্গ;
	}
	পক্ষ STL_সাৰি:
	    অঙ্ক = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    ভঙ্গ;

	পক্ষ STL_NUMLINES:
	    অঙ্ক = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;
	    ভঙ্গ;

	পক্ষ STL_COLUMN:
	    অঙ্ক = (State & MODE_ঢোকাও) == 0 && empty_সাৰি
					       ? 0 : (পূর্ণাঙ্ক)wp->w_cursor.col + 1;
	    ভঙ্গ;

	পক্ষ STL_VIRTCOL:
	পক্ষ STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    যদি (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_ঢোকাও) == 0
			       && empty_সাৰি ? 0 : (পূর্ণাঙ্ক)wp->w_cursor.col + 1)))
		ভঙ্গ;
	    অঙ্ক = (long)virtcol;
	    ভঙ্গ;
	}

	পক্ষ STL_PERCENTAGE:
	    অঙ্ক = calc_percentage((long)wp->w_cursor.lnum, (long)wp->w_buffer->b_ml.ml_সাৰি_গুনতি);
	    ভঙ্গ;

	পক্ষ STL_ALTPERCENT:
	    str = buf_tmp;
	    (শূন্য)get_rel_pos(wp, str, TMPLEN);
	    ভঙ্গ;

	পক্ষ STL_SHOWCMD:
	    যদি (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    ভঙ্গ;

	পক্ষ STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    যদি (append_arg_সংখ্যা(wp, buf_tmp, পরিমাণ(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    ভঙ্গ;

	পক্ষ STL_KEYMAP:
	    fillable = FALSE;
	    যদি (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    ভঙ্গ;
	পক্ষ STL_PAGENUM:
# যদি defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    অঙ্ক = printer_page_অঙ্ক;
# অন্যথা
	    অঙ্ক = 0;
# endif
	    ভঙ্গ;

	পক্ষ STL_BUFNO:
	    অঙ্ক = wp->w_buffer->b_fnum;
	    ভঙ্গ;

	পক্ষ STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	পক্ষ STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_সাৰি_or_offset(wp->w_buffer, wp->w_cursor.lnum, রিক্ত);
	    অঙ্ক = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_ঢোকাও) == 0 && empty_সাৰি
				? 0 : (পূর্ণাঙ্ক)wp->w_cursor.col);
# endif
	    ভঙ্গ;

	পক্ষ STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	পক্ষ STL_BYTEVAL:
	    অঙ্ক = byteval;
	    যদি (অঙ্ক == NL)
		অঙ্ক = 0;
	    অন্যথা যদি (অঙ্ক == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		অঙ্ক = NL;
	    ভঙ্গ;

	পক্ষ STL_ROFLAG:
	পক্ষ STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    যদি (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    ভঙ্গ;

	পক্ষ STL_HELPFLAG:
	পক্ষ STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    যদি (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    ভঙ্গ;

	পক্ষ STL_FILETYPE:
	    যদি (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((ৱর্ণ *)buf_tmp, পরিমাণ(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    ভঙ্গ;

	পক্ষ STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    যদি (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((ৱর্ণ *)buf_tmp, পরিমাণ(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		বাবে (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    ভঙ্গ;

# যদি defined(FEAT_QUICKFIX)
	পক্ষ STL_PREVIEWFLAG:
	পক্ষ STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    যদি (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    ভঙ্গ;

	পক্ষ STL_QUICKFIX:
	    যদি (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(বাৰ্তা_loclist)
			    : _(বাৰ্তা_qflist));
	    ভঙ্গ;
# endif

	পক্ষ STL_MODIFIED:
	পক্ষ STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    ৱিকল্প ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		পক্ষ 2: str = (char_u *)"[+]"; ভঙ্গ;
		পক্ষ 3: str = (char_u *)",+"; ভঙ্গ;
		পক্ষ 4: str = (char_u *)"[-]"; ভঙ্গ;
		পক্ষ 5: str = (char_u *)",-"; ভঙ্গ;
		পক্ষ 6: str = (char_u *)"[+-]"; ভঙ্গ;
		পক্ষ 7: str = (char_u *)",+-"; ভঙ্গ;
	    }
	    ভঙ্গ;

	পক্ষ STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		যেতিয়ালৈ (*s != '#' && *s != NUL)
		    ++s;
		যদি (*s == '#')
		{
		    stl_বস্তুসমূহ[curitem].stl_type = Highlight;
		    stl_বস্তুসমূহ[curitem].stl_start = p;
		    stl_বস্তুসমূহ[curitem].stl_minwid = -syn_namen2id(t, (পূর্ণাঙ্ক)(s - t));
		    curitem++;
		}
		যদি (*s != NUL)
		    ++s;
		অনুৱর্ত;
	    }

	পক্ষ STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    যদি (*s == ']')
	    {
		stl_বস্তুসমূহ[curitem].stl_type = ClickFunc;
		stl_বস্তুসমূহ[curitem].stl_start = p;
		stl_বস্তুসমূহ[curitem].stl_minwid = 0;
		stl_বস্তুসমূহ[curitem].stl_clickfunc = রিক্ত;
		s++;
		curitem++;
		অনুৱর্ত;
	    }
	    // %N[FuncName] with minwid
	    যদি (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		যদি (rb != রিক্ত)
		{
		    stl_বস্তুসমূহ[curitem].stl_type = ClickFunc;
		    stl_বস্তুসমূহ[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_বস্তুসমূহ[curitem].stl_minwid = raw_minwid;
		    stl_বস্তুসমূহ[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    অনুৱর্ত;
		}
	    }
	    অনুৱর্ত;

	পক্ষ STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    অনুৱর্ত;
	}

	stl_বস্তুসমূহ[curitem].stl_start = p;
	stl_বস্তুসমূহ[curitem].stl_type = Normal;
	যদি (str != রিক্ত && *str)
	{
	    char_u  *t = str;

	    যদি (itemisflag)
	    {
		যদি ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    যদি (l > 0)
		prevchar_isitem = TRUE;
	    যদি (l > maxwid)
	    {
		যেতিয়ালৈ (l >= maxwid)
		    যদি (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    অন্যথা
			l -= byte2cells(*t++);
		যদি (p + 1 >= out + outlen)
		    ভঙ্গ;
		*p++ = '<';
	    }
	    যদি (minwid > 0)
	    {
		বাবে (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    যদি (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    অন্যথা
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    অন্যথা
		minwid *= -1;
	    বাবে (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		যদি (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		অন্যথা
		    *p++ = *t;
	    }
	    বাবে (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	অন্যথা যদি (অঙ্ক >= 0)
	{
	    পূর্ণাঙ্ক	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    যদি (p + 20 >= out + outlen)
		ভঙ্গ;		// not sufficient space
	    prevchar_isitem = TRUE;
	    যদি (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    যদি (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    বাবে (n = অঙ্ক, l = 1; n >= nbase; n /= nbase)
		l++;
	    যদি (opt == STL_VIRTCOL_ALT)
		l++;

	    যদি (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		যেতিয়ালৈ (l-- > maxwid)
		    অঙ্ক /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((ৱর্ণ *)p, outlen - (p - out),
		    (ৱর্ণ *)nstr, 0, অঙ্ক, n);
	    }
	    অন্যথা
		p += vim_snprintf_safelen((ৱর্ণ *)p, outlen - (p - out),
		    (ৱর্ণ *)nstr, minwid, অঙ্ক);
	}
	অন্যথা
	    stl_বস্তুসমূহ[curitem].stl_type = Empty;

	যদি (অঙ্ক >= 0 || (!itemisflag && str != রিক্ত && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	যদি (opt == STL_VIM_ৰাশিমালা)
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
    যদি (carry_hl != রিক্ত)
    {
	পূর্ণাঙ্ক last_hl = 0;
	বাবে (l = 0; l < itemcnt; l++)
	    যদি (stl_বস্তুসমূহ[l].stl_type == Highlight)
		last_hl = stl_বস্তুসমূহ[l].stl_minwid;
	*carry_hl = last_hl;
    }

    যদি (mode == STL_MODE_MULTI
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
	যদি (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = strlen((ৱর্ণ *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    অন্যথা
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	যদি (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    যদি (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	বাবে (l = 0; l < itemcnt; l++)
	    যদি (stl_বস্তুসমূহ[l].stl_type == ClickFunc)
		vim_free(stl_বস্তুসমূহ[l].stl_clickfunc);
	যদি (rendered_height != রিক্ত)
	    *rendered_height = rheight;
	ঘূৰাই 0;
    }

    width = vim_strsize(out);
    যদি (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	যদি (itemcnt == 0)
	    s = out;
	অন্যথা
	{
	    বাবে ( ; l < itemcnt; l++)
		যদি (stl_বস্তুসমূহ[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_বস্তুসমূহ[l].stl_start;
		    ভঙ্গ;
		}
	    যদি (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_বস্তুসমূহ[0].stl_start;
		l = 0;
	    }
	}

	যদি (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    যদি (has_mbyte)
	    {
		s = out;
		width = 0;
		বাবে (;;)
		{
		    width += ptr2cells(s);
		    যদি (width >= maxwidth)
			ভঙ্গ;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		যেতিয়ালৈ (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    অন্যথা
		s = out + maxwidth - 1;
	    বাবে (l = 0; l < itemcnt; l++)
		যদি (stl_বস্তুসমূহ[l].stl_start > s)
		    ভঙ্গ;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	অন্যথা
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    যেতিয়ালৈ (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(end - p) + 1);	// +1 for NUL
	    end -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    বাবে (; l < itemcnt; l++)
	    {
		যদি (stl_বস্তুসমূহ[l].stl_start - n >= s)
		    stl_বস্তুসমূহ[l].stl_start -= n;
		অন্যথা
		    stl_বস্তুসমূহ[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    যেতিয়ালৈ (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    অন্যথা যদি (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	পূর্ণাঙ্ক অঙ্ক_separators = 0;

	বাবে (l = 0; l < itemcnt; l++)
	{
	    যদি (stl_বস্তুসমূহ[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[অঙ্ক_separators] = l;
		অঙ্ক_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	যদি (অঙ্ক_separators)
	{
	    পূর্ণাঙ্ক standard_spaces;
	    পূর্ণাঙ্ক final_spaces;

	    standard_spaces = (maxwidth - width) / অঙ্ক_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (অঙ্ক_separators - 1);
	    বাবে (l = 0; l < অঙ্ক_separators; l++)
	    {
		পূর্ণাঙ্ক dislocation = (l == (অঙ্ক_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_বস্তুসমূহ[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		বাবে (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		বাবে (পূর্ণাঙ্ক i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_বস্তুসমূহ[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    যদি (hltab != রিক্ত)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	বাবে (l = 0; l < itemcnt; l++)
	{
	    যদি (stl_বস্তুসমূহ[l].stl_type == Highlight)
	    {
		sp->start = stl_বস্তুসমূহ[l].stl_start;
		sp->userhl = stl_বস্তুসমূহ[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = রিক্ত;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    যদি (tabtab != রিক্ত)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	বাবে (l = 0; l < itemcnt; l++)
	{
	    যদি (stl_বস্তুসমূহ[l].stl_type == TabPage)
	    {
		sp->start = stl_বস্তুসমূহ[l].stl_start;
		sp->userhl = stl_বস্তুসমূহ[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = রিক্ত;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    যদি (clicktab != রিক্ত)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	বাবে (l = 0; l < itemcnt; l++)
	{
	    যদি (stl_বস্তুসমূহ[l].stl_type == ClickFunc)
	    {
		cp->start = stl_বস্তুসমূহ[l].stl_start;
		cp->funcname = stl_বস্তুসমূহ[l].stl_clickfunc;
		cp->minwid = stl_বস্তুসমূহ[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = রিক্ত;
	cp->funcname = রিক্ত;
    }
    অন্যথা
    {
	// Free click function names when caller doesn't need them.
	বাবে (l = 0; l < itemcnt; l++)
	    যদি (stl_বস্তুসমূহ[l].stl_type == ClickFunc)
		vim_free(stl_বস্তুসমূহ[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    চাবি_Typed = save_চাবি_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    যদি (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    ঘূৰাই width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    পূর্ণাঙ্ক
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    পূর্ণাঙ্ক		buflen)
{
    long	above; // number of lines above window
    long	below; // number of lines below window

    যদি (buflen < 3) // need at least 3 chars for writing
	ঘূৰাই 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    যদি (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_সাৰি_গুনতি - wp->w_botline + 1;
    যদি (below <= 0)
	ঘূৰাই (পূর্ণাঙ্ক)vim_snprintf_safelen((ৱর্ণ *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    যদি (above <= 0)
	ঘূৰাই (পূর্ণাঙ্ক)vim_snprintf_safelen((ৱর্ণ *)buf, buflen,
	    "%s", _("Top"));

    পূর্ণাঙ্ক perc = calc_percentage(above, above + below);
    ৱর্ণ tmp[8];
    // localized percentage value
    vim_snprintf(tmp, পরিমাণ(tmp), _("%d%%"), perc);
    ঘূৰাই (পূর্ণাঙ্ক)vim_snprintf_safelen((ৱর্ণ *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    স্থির পূর্ণাঙ্ক
append_arg_সংখ্যা(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    পূর্ণাঙ্ক		add_file)	// Add "file" before the arg number
{
    যদি (ARGCOUNT <= 1)		// nothing to do
	ঘূৰাই 0;

    ৱর্ণ *বাৰ্তা;
    ৱিকল্প ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	পক্ষ 0: বাৰ্তা = _(" (%d of %d)"); ভঙ্গ;
	পক্ষ 1: বাৰ্তা = _(" ((%d) of %d)"); ভঙ্গ;
	পক্ষ 2: বাৰ্তা = _(" (file %d of %d)"); ভঙ্গ;
	পক্ষ 3: বাৰ্তা = _(" (file (%d) of %d)"); ভঙ্গ;
    }

    ঘূৰাই (পূর্ণাঙ্ক)vim_snprintf_safelen((ৱর্ণ *)buf, buflen, বাৰ্তা,
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
    ঘূৰাই FullName_save(fname, TRUE);
#অন্যথা
    যদি (!vim_isAbsName(fname)
	    || strstr((ৱর্ণ *)fname, "..") != রিক্ত
	    || strstr((ৱর্ণ *)fname, "//") != রিক্ত
# ifdef BACKSLASH_IN_FILENAME
	    || strstr((ৱর্ণ *)fname, "\\\\") != রিক্ত
# endif
# যদি defined(MSWIN)
	    || vim_strchr(fname, '~') != রিক্ত
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	ঘূৰাই FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    যদি (fname != রিক্ত)
	fname_case(fname, 0);	// set correct case for file name
# endif

    ঘূৰাই fname;
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
    শূন্য
fname_expand(
    buf_T	*buf UNUSED,
    char_u	**ffname,
    char_u	**sfname)
{
    যদি (*ffname == রিক্ত)	    // no file name given, nothing to do
	ঘূৰাই;
    যদি (*sfname == রিক্ত)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    যদি (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	যদি (rfname != রিক্ত)
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
    শূন্য
ex_buffer_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    পূর্ণাঙ্ক		split_ret = OK;
    পূর্ণাঙ্ক		p_ea_save;
    পূর্ণাঙ্ক		open_wins = 0;
    পূর্ণাঙ্ক		r;
    পূর্ণাঙ্ক		গুনতি;		// Maximum number of windows to open.
    পূর্ণাঙ্ক		all;		// When TRUE also load inactive buffers.
    পূর্ণাঙ্ক		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    যদি (eap->addr_গুনতি == 0)	// make as many windows as possible
	গুনতি = 9999;
    অন্যথা
	গুনতি = eap->line2;	// make as many windows as specified
    যদি (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    অন্যথা
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
    যদি (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    বাবে (;;)
    {
	tpnext = curtab->tp_next;
	বাবে (wp = firstwin; wp != রিক্ত; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    যদি ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		যদি (win_close(wp, FALSE) == FAIL)
		    ভঙ্গ;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    অন্যথা
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	যদি (had_tab == 0 || tpnext == রিক্ত)
	    ভঙ্গ;
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
    বাবে (buf = firstbuf; buf != রিক্ত && open_wins < গুনতি; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	যদি ((!all && buf->b_ml.ml_mfp == রিক্ত) || !buf->b_p_bl)
	    অনুৱর্ত;

	যদি (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    যদি (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    অন্যথা
		wp = রিক্ত;
	}
	অন্যথা
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		যদি (wp->w_buffer == buf)
		    ভঙ্গ;
	    // If the buffer already has a window, move it
	    যদি (wp != রিক্ত)
		win_move_after(wp, curwin);
	}

	যদি (wp == রিক্ত && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    যদি (split_ret == FAIL)
		অনুৱর্ত;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    যদি (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		ভঙ্গ;
	    }
	    যদি (swap_exists_action == SEA_QUIT)
	    {
#যদি defined(FEAT_EVAL)
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

#যদি defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    অন্যথা
		handle_swap_exists(রিক্ত);
	}

	ui_breakcheck();
	যদি (got_int)
	{
	    (শূন্য)vgetc();	// only break the file loading, not the rest
	    ভঙ্গ;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	যদি (aborting())
	    ভঙ্গ;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	যদি (had_tab > 0 && tabpage_index(রিক্ত) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    বাবে (wp = lastwin; open_wins > গুনতি; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	যদি (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	অন্যথা যদি (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	অন্যথা
	{
	    wp = wp->w_prev;
	    যদি (wp == রিক্ত)
		ভঙ্গ;
	}
    }
}


স্থির পূর্ণাঙ্ক  chk_modeline(linenr_T, পূর্ণাঙ্ক);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    শূন্য
do_modelines(পূর্ণাঙ্ক flags)
{
    linenr_T	lnum;
    পূর্ণাঙ্ক		nmlines;
    স্থির পূর্ণাঙ্ক	entered = 0;

    যদি (!curbuf->b_p_ml || (nmlines = (পূর্ণাঙ্ক)p_mls) == 0)
	ঘূৰাই;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    যদি (entered)
	ঘূৰাই;

    ++entered;
    বাবে (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_সাৰি_গুনতি && lnum <= nmlines;
								       ++lnum)
	যদি (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    বাবে (lnum = curbuf->b_ml.ml_সাৰি_গুনতি; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_সাৰি_গুনতি - nmlines; --lnum)
	যদি (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#অন্তর্ভাৱ "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    স্থির পূর্ণাঙ্ক
chk_modeline(
    linenr_T	lnum,
    পূর্ণাঙ্ক		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*সাৰি_end;		// point to the end of the line
    char_u	*e;
    পূর্ণাঙ্ক		prev;
    পূর্ণাঙ্ক		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    সাৰি_end = s + ml_get_len(lnum);
    বাবে (; *s != NUL; ++s)
    {
	যদি (prev == -1 || vim_isspace(prev))
	{
	    যদি ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		ভঙ্গ;
	    // Accept both "vim" and "Vim".
	    যদি ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		পূর্ণাঙ্ক vers;

		যদি (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		অন্যথা
		    e = s + 3;
		vers = getdigits(&e);
		যদি (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    ভঙ্গ;
	    }
	}
	prev = *s;
    }

    যদি (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	পূর্ণাঙ্ক	end;

	কৰা				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	যেতিয়ালৈ (s[-1] != ':');

	len = (size_t)(সাৰি_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	যদি (linecopy == রিক্ত)
	    ঘূৰাই FAIL;

	সাৰি_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_ঠেলো(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	যেতিয়ালৈ (end == FALSE)
	{
	    s = skipwhite(s);
	    যদি (*s == NUL)
		ভঙ্গ;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    বাবে (e = s; *e != ':' && *e != NUL; ++e)
		যদি (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(সাৰি_end - (e + 1)) + 1);	// +1 for NUL
		    --সাৰি_end;
		}
	    যদি (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    যদি (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		যদি (*e != ':')		// no terminating ':'?
		    ভঙ্গ;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    যদি (*s != NUL)		// skip over an empty "::"
	    {
		পূর্ণাঙ্ক secure_save = secure;
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
		যদি (retval == FAIL)		// stop if error found
		    ভঙ্গ;
	    }
	    s = (e == সাৰি_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_বেৰ_কৰো();
	vim_free(linecopy);
    }
    ঘূৰাই retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    পূর্ণাঙ্ক
bt_normal(buf_T *buf)
{
    ঘূৰাই buf != রিক্ত && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    পূর্ণাঙ্ক
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    ঘূৰাই buf != রিক্ত && buf->b_p_bt[0] == 'q';
#অন্যথা
    ঘূৰাই FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    পূর্ণাঙ্ক
bt_terminal(buf_T *buf UNUSED)
{
#যদি defined(FEAT_TERMINAL)
    ঘূৰাই buf != রিক্ত && buf->b_p_bt[0] == 't';
#অন্যথা
    ঘূৰাই FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    পূর্ণাঙ্ক
bt_help(buf_T *buf)
{
    ঘূৰাই buf != রিক্ত && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    পূর্ণাঙ্ক
bt_prompt(buf_T *buf)
{
    ঘূৰাই buf != রিক্ত && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#যদি defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    পূর্ণাঙ্ক
bt_popup(buf_T *buf)
{
    ঘূৰাই buf != রিক্ত && buf->b_p_bt != রিক্ত
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    পূর্ণাঙ্ক
bt_nofilename(buf_T *buf)
{
    ঘূৰাই buf != রিক্ত && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    স্থির পূর্ণাঙ্ক
bt_nofileread(buf_T *buf)
{
    ঘূৰাই buf != রিক্ত && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#যদি defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    পূর্ণাঙ্ক
bt_nofile(buf_T *buf)
{
    ঘূৰাই buf != রিক্ত && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    পূর্ণাঙ্ক
bt_dontwrite(buf_T *buf)
{
    ঘূৰাই buf != রিক্ত && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    পূর্ণাঙ্ক
bt_dontwrite_বাৰ্তা(buf_T *buf)
{
    যদি (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    পূর্ণাঙ্ক
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    ৱিকল্প (buf->b_p_bh[0])
    {
	পক্ষ 'u':		    // "unload"
	পক্ষ 'w':		    // "wipe"
	পক্ষ 'd': ঘূৰাই FALSE;	    // "delete"
	পক্ষ 'h': ঘূৰাই TRUE;	    // "hide"
    }
    ঘূৰাই (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#যদি defined(FEAT_QUICKFIX)
    যদি (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	যদি (buf->b_fnum == qf_স্তূপ_get_bufnr())
	    ঘূৰাই (char_u *)_(বাৰ্তা_qflist);
	অন্যথা
	    ঘূৰাই (char_u *)_(বাৰ্তা_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    যদি (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	যদি (buf->b_term != রিক্ত)
	    ঘূৰাই term_get_status_text(buf->b_term);
#endif
	যদি (buf->b_fname != রিক্ত)
	    ঘূৰাই buf->b_fname;
	যদি (buf == cmdwin_buf)
	    ঘূৰাই (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	যদি (bt_prompt(buf))
	    ঘূৰাই (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	যদি (bt_popup(buf))
	    ঘূৰাই (char_u *)_("[Popup]");
#endif
	ঘূৰাই (char_u *)_("[Scratch]");
    }

    যদি (buf->b_fname == রিক্ত)
	ঘূৰাই buf_get_fname(buf);
    ঘূৰাই রিক্ত;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    যদি (buf->b_fname == রিক্ত)
	ঘূৰাই (char_u *)_("[No Name]");
    ঘূৰাই buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    শূন্য
set_buflisted(পূর্ণাঙ্ক on)
{
    যদি (on == curbuf->b_p_bl)
	ঘূৰাই;

    curbuf->b_p_bl = on;
    যদি (on)
	apply_autocmds(EVENT_BUFADD, রিক্ত, রিক্ত, FALSE, curbuf);
    অন্যথা
	apply_autocmds(EVENT_BUFDELETE, রিক্ত, রিক্ত, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    পূর্ণাঙ্ক
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    পূর্ণাঙ্ক		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(রিক্ত, রিক্ত, (linenr_T)1, BLN_DUMMY);
    যদি (newbuf == রিক্ত)
	ঘূৰাই TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    যদি (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	ঘূৰাই TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    যদি (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	ঘূৰাই TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    যদি (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	যদি (buf->b_ml.ml_সাৰি_গুনতি == curbuf->b_ml.ml_সাৰি_গুনতি)
	{
	    differ = FALSE;
	    বাবে (lnum = 1; lnum <= curbuf->b_ml.ml_সাৰি_গুনতি; ++lnum)
		যদি (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    ভঙ্গ;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    যদি (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    ঘূৰাই differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    শূন্য
wipe_buffer(
    buf_T	*buf,
    পূর্ণাঙ্ক		aucmd)	    // When TRUE trigger autocommands.
{
    যদি (buf->b_fnum == শীৰ্ষ_file_অঙ্ক - 1)
	--শীৰ্ষ_file_অঙ্ক;

    যদি (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(রিক্ত, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    যদি (!aucmd)
	unblock_autocmds();
}
