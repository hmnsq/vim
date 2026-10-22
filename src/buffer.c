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
स्थिर char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, पूर्णाङ्क ignore_case);
स्थिर char_u	*fname_match(regmatch_T *rmp, char_u *name, पूर्णाङ्क ignore_case);
#ifdef UNIX
स्थिर buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
स्थिर पूर्णाङ्क	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
स्थिर पूर्णाङ्क	buf_same_ino(buf_T *buf, stat_T *stp);
#अञ्ञथा
स्थिर पूर्णाङ्क	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
स्थिर पूर्णाङ्क	मानम्_changed(char_u *str, char_u **last);
#सचे defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
स्थिर पूर्णाङ्क build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, पूर्णाङ्क opt_scope, पूर्णाङ्क fillchar, पूर्णाङ्क maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, पूर्णाङ्क *lbreaks, पूर्णाङ्क *carry_hl);
#endif
स्थिर पूर्णाङ्क	append_arg_सङ्ख्या(win_T *wp, char_u *buf, size_t buflen, पूर्णाङ्क add_file);
स्थिर शून्य	free_buffer(buf_T *);
स्थिर शून्य	free_buffer_stuff(buf_T *buf, पूर्णाङ्क free_options);
स्थिर पूर्णाङ्क	bt_nofileread(buf_T *buf);
स्थिर पूर्णाङ्क	do_buffer_ext(पूर्णाङ्क action, पूर्णाङ्क start, पूर्णाङ्क dir, पूर्णाङ्क गणनम्, पूर्णाङ्क flags);

#ifdef UNIX
# define dev_T dev_t
#अञ्ञथा
# define dev_T unsigned
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    अत्थाय ((buf) = lastbuf; (buf) != रिक्त; (buf) = (buf)->b_prev)

#सचे defined(FEAT_QUICKFIX)
स्थिर वर्ण *सन्देशः_loclist = N_("[Location List]");
स्थिर वर्ण *सन्देशः_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
स्थिर पूर्णाङ्क	buf_free_गणनम् = 0;

स्थिर पूर्णाङ्क	शिखरम्_file_अङ्कः = 1;	// highest file number
स्थिर garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    स्थिर शून्य
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"सचे exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    स्थिर पूर्णाङ्क
calc_percentage(long part, long whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    पटिनिवत्त (part > 1000000L)
	? (पूर्णाङ्क)(part / (whole / 100L))
	: (पूर्णाङ्क)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    पूर्णाङ्क
get_highest_fnum(शून्य)
{
    पटिनिवत्त शिखरम्_file_अङ्कः - 1;
}

/*
 * Read data from buffer for retrying.
 */
    स्थिर पूर्णाङ्क
read_buffer(
    पूर्णाङ्क		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    पूर्णाङ्क		flags)		    // extra flags for readfile()
{
    पूर्णाङ्क		retval = OK;
    linenr_T	पङ्क्तिः_गणनम्;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    पङ्क्तिः_गणनम् = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
    retval = readfile(
	    read_stdin ? रिक्त : curbuf->b_ffname,
	    read_stdin ? रिक्त : curbuf->b_fname,
	    पङ्क्तिः_गणनम्, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    सचे (retval == OK)
    {
	// Delete the binary lines.
	याव (--पङ्क्तिः_गणनम् >= 0)
	    ml_delete((linenr_T)1);
    }
    अञ्ञथा
    {
	// Delete the converted lines.
	याव (curbuf->b_ml.ml_पङ्क्तिः_गणनम् > पङ्क्तिः_गणनम्)
	    ml_delete(पङ्क्तिः_गणनम्);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    सचे (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	सचे (!readonlymode && !BUFEMPTY())
	    changed();
	अञ्ञथा सचे (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	सचे (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, रिक्त, रिक्त, FALSE,
							      curbuf, &retval);
#अञ्ञथा
	    apply_autocmds(EVENT_STDINREADPOST, रिक्त, रिक्त, FALSE, curbuf);
#endif
	}
    }
    पटिनिवत्त retval;
}

#सचे defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    शून्य
buffer_ensure_loaded(buf_T *buf)
{
    सचे (buf->b_ml.ml_mfp != रिक्त)
	पटिनिवत्त;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    सचे (curbuf == buf)
    {
	सचे (swap_exists_action != SEA_READONLY)
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
    पूर्णाङ्क
open_buffer(
    पूर्णाङ्क		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    पूर्णाङ्क		flags_arg)	    // extra flags for readfile()
{
    पूर्णाङ्क		flags = flags_arg;
    पूर्णाङ्क		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    पूर्णाङ्क		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    सचे (readonlymode && curbuf->b_ffname != रिक्त
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    सचे (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    सचे (curbuf->b_ml.ml_mfp != रिक्त)
		भङ्ग;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	सचे (curbuf == रिक्त)
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
	सचे (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(रिक्त, curwin);
#endif
	पटिनिवत्त FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    सचे (curbuf->b_ml.ml_mfp != रिक्त)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    सचे (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    सचे (curbuf->b_ffname != रिक्त
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	पूर्णाङ्क old_सन्देशः_silent = सन्देशः_silent;
#ifdef UNIX
	पूर्णाङ्क	save_bin = curbuf->b_p_bin;
	पूर्णाङ्क perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	पूर्णाङ्क oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	सचे (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	सचे (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	सचे (shortmess(SHM_FILEINFO))
	    सन्देशः_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	सचे (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    सचे (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_buffer(FALSE, eap, flags);
	}
#endif
	सन्देशः_silent = old_सन्देशः_silent;
#ifdef FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#endif
	// Help buffer is filtered.
	सचे (bt_help(curbuf))
	    fix_help_buffer();
    }
    अञ्ञथा सचे (read_stdin)
    {
	पूर्णाङ्क	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(रिक्त, रिक्त, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, रिक्त,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	सचे (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    सचे (curbuf->b_ml.ml_mfp != रिक्त
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    सचे (curbuf->b_flags & BF_NEVERLOADED)
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
    सचे ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != रिक्त)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != रिक्त)
#endif
       )
	changed();
    अञ्ञथा सचे (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    सचे (aborting())
#अञ्ञथा
    सचे (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    सचे (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf, &retval);
#अञ्ञथा
    apply_autocmds(EVENT_BUFENTER, रिक्त, रिक्त, FALSE, curbuf);
#endif

    सचे (retval != OK)
	पटिनिवत्त retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    सचे (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != रिक्त)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	सचे (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    सचे ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, रिक्त, रिक्त,
			FALSE, curbuf, &retval);
#अञ्ञथा
	    apply_autocmds(EVENT_BUFWINENTER, रिक्त, रिक्त,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    पटिनिवत्त retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    शून्य
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == रिक्त ? 0 : buf->b_fnum;
    bufref->br_buf_free_गणनम् = buf_free_गणनम्;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    पूर्णाङ्क
bufref_valid(bufref_T *bufref)
{
    पटिनिवत्त bufref->br_buf_free_गणनम् == buf_free_गणनम्
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    पूर्णाङ्क
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	सचे (bp == buf)
	    पटिनिवत्त TRUE;
    पटिनिवत्त FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
स्थिर hashtab_T buf_hashtab;

    स्थिर शून्य
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((वर्ण *)buf->b_कुञ्चिका, परिमाण(buf->b_कुञ्चिका), "%x", buf->b_fnum);
    सचे (hash_add(&buf_hashtab, buf->b_कुञ्चिका, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    स्थिर शून्य
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_कुञ्चिका);

    सचे (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    स्थिर पूर्णाङ्क
can_unload_buffer(buf_T *buf)
{
    पूर्णाङ्क	    can_unload = !buf->b_locked;

    सचे (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    सचे (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		भङ्ग;
	    }
    }
    // Don't unload the buffer while it's still being saved
    सचे (can_unload && buf->b_saving)
	can_unload = FALSE;

    सचे (!can_unload)
    {
	char_u *fname = buf->b_fname != रिक्त ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != रिक्त ? fname : (char_u *)"[No Name]");
    }
    पटिनिवत्त can_unload;
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
    पूर्णाङ्क
close_buffer(
    win_T	*win,
    buf_T	*buf,
    पूर्णाङ्क		action,
    पूर्णाङ्क		abort_if_last,
    पूर्णाङ्क		ignore_abort,
    पूर्णाङ्क		set_context)
{
    पूर्णाङ्क		hiding_buf;
    bufref_T	bufref;
    पूर्णाङ्क		is_curwin = (curwin != रिक्त && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    पूर्णाङ्क		unload_buf = (action != 0);
    पूर्णाङ्क		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    पूर्णाङ्क		del_buf = (action == DOBUF_DEL || wipe_buf);
    पूर्णाङ्क		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    पूर्णाङ्क		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    सचे (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    अञ्ञथा सचे (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    अञ्ञथा सचे (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    सचे (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	सचे (term_job_running(buf->b_term))
	{
	    सचे (wipe_buf || unload_buf)
	    {
		सचे (!can_unload_buffer(buf))
		    पटिनिवत्त FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    अञ्ञथा
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	अञ्ञथा सचे (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	अञ्ञथा
	{
	    सचे (del_buf || unload_buf)
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
    सचे ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	पटिनिवत्त FALSE;

    सचे (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	सचे (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    सचे ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	सचे (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    पटिनिवत्त FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	सचे (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    गच्छ aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	सचे (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    सचे (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		गच्छ aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    सचे (abort_if_last && one_window())
		// Autocommands made this the only window.
		गच्छ aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	सचे (!ignore_abort && aborting())
	    पटिनिवत्त FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    सचे (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    सचे (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    सचे (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    सचे (!hiding_buf || !unload_buf)
	पटिनिवत्त FALSE;

    // Always remove the buffer when there is no file name.
    सचे (buf->b_ffname == रिक्त)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    सचे (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	पटिनिवत्त FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    सचे ((win_valid || closed_popup) && win->w_buffer == buf)
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
    सचे (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != रिक्त || buf->b_next != रिक्त
#सचे defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	सचे (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    सचे (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, परिमाण(पूर्णाङ्क), 50);
	    सचे (ga_grow(&buf_reuse, 1) == OK)
		((पूर्णाङ्क *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	सचे (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	अञ्ञथा
	    buf->b_sfname = रिक्त;
	VIM_CLEAR(buf->b_ffname);
	सचे (buf->b_prev == रिक्त)
	    firstbuf = buf->b_next;
	अञ्ञथा
	    buf->b_prev->b_next = buf->b_next;
	सचे (buf->b_next == रिक्त)
	    lastbuf = buf->b_prev;
	अञ्ञथा
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    अञ्ञथा
    {
	सचे (del_buf)
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
	सचे (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    प्रतिज्ञा(!win_valid || !is_curwin || win == curwin);
    पटिनिवत्त TRUE;
}

/*
 * Make buffer not contain a file.
 */
    शून्य
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_पङ्क्तिः_गणनम् = 1;
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
    netbeans_deleted_all_पङ्क्तयः(buf);
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
    पूर्णाङ्क
buf_freeall(buf_T *buf, पूर्णाङ्क flags)
{
    पूर्णाङ्क		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    पूर्णाङ्क		is_curwin = (curwin != रिक्त && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    सचे (buf->b_ml.ml_mfp != रिक्त)
    {
	सचे (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    पटिनिवत्त FALSE;
    }
    सचे ((flags & BFA_DEL) && buf->b_p_bl)
    {
	सचे (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    पटिनिवत्त FALSE;
    }
    सचे (flags & BFA_WIPE)
    {
	सचे (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    पटिनिवत्त FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    सचे (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    सचे ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	पटिनिवत्त FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    सचे (buf == curbuf && !is_curbuf)
	पटिनिवत्त FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    सचे (buf == curbuf && VIsual_active
#सचे defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    सचे (curwin != रिक्त && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    सचे (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_पङ्क्तिः_गणनम् = 0;    // no lines in buffer
    सचे ((flags & BFA_KEEP_UNDO) == 0)
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
    पटिनिवत्त TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    स्थिर शून्य
free_buffer(buf_T *buf)
{
    ++buf_free_गणनम्;
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

    सचे (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    अञ्ञथा
    {
	vim_free(buf);
	सचे (curbuf == buf)
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
    di->di_tv.v_type = VAR_सङ्ख्या;
    di->di_tv.v_lock = VAR_FIXED;
    di->di_tv.vval.v_सङ्ख्या = 0;

#ifdef FEAT_EVAL
    STRCPY(buf->b_ct_di.di_कुञ्चिका, "changedtick");
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

    याव (buf->b_wininfo != रिक्त)
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
    पूर्णाङ्क		free_options)		// free options as well
{
    सचे (free_options)
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
    सचे (wip->wi_optset)
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
    पूर्णाङ्क		start,
    पूर्णाङ्क		dir,
    पूर्णाङ्क		गणनम्)
{
    bufref_T	old_curbuf;
    पूर्णाङ्क		save_sea = swap_exists_action;
    पूर्णाङ्क		skip_help_buf;

    विकल्प (eap->cmdidx)
    {
	पक्ष CMD_bnext:
	पक्ष CMD_sbnext:
	पक्ष CMD_bNext:
	पक्ष CMD_bprevious:
	पक्ष CMD_sbNext:
	पक्ष CMD_sbprevious:
	    skip_help_buf = TRUE;
	    भङ्ग;
	स्वभाव:
	    skip_help_buf = FALSE;
	    भङ्ग;
    }

    set_bufref(&old_curbuf, curbuf);

    सचे (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (शून्य)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, गणनम्,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    सचे (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#सचे defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#सचे defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    अञ्ञथा
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    शून्य
handle_swap_exists(bufref_T *old_curbuf)
{
#सचे defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    सचे (swap_exists_action == SEA_QUIT)
    {
#सचे defined(FEAT_EVAL)
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
	सचे (old_curbuf == रिक्त || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(रिक्त, रिक्त, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	अञ्ञथा
	    buf = old_curbuf->br_buf;
	सचे (buf != रिक्त)
	{
	    पूर्णाङ्क old_सन्देशः_silent = सन्देशः_silent;

	    सचे (shortmess(SHM_FILEINFO))
		सन्देशः_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    सन्देशः_silent = old_सन्देशः_silent;

#ifdef FEAT_SYN_HL
	    सचे (old_tw != curbuf->b_p_tw)
		check_colorcolumn(रिक्त, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#सचे defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    अञ्ञथा सचे (swap_exists_action == SEA_RECOVER)
    {
#सचे defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Recover at ATTENTION prompt.
	सन्देशः_scroll = TRUE;
	ml_recover(FALSE);
	सन्देशः_puts("\n");	// don't overwrite the last message
	cmdline_row = सन्देशः_row;
	do_modelines(0);

#सचे defined(FEAT_EVAL)
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
    स्थिर पूर्णाङ्क
empty_curbuf(
    पूर्णाङ्क close_others,
    पूर्णाङ्क forceit,
    पूर्णाङ्क action)
{
    पूर्णाङ्क	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    सचे (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	पटिनिवत्त FAIL;
    }

    set_bufref(&bufref, buf);
    सचे (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, रिक्त, रिक्त, रिक्त, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    सचे (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(रिक्त, buf, action, FALSE, FALSE, FALSE);
    सचे (!close_others)
	need_fileinfo = FALSE;
    अञ्ञथा सचे (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    पटिनिवत्त retval;
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
    स्थिर पूर्णाङ्क
do_buffer_ext(
    पूर्णाङ्क		action,
    पूर्णाङ्क		start,
    पूर्णाङ्क		dir,		// FORWARD or BACKWARD
    पूर्णाङ्क		गणनम्,		// buffer number or number of buffers
    पूर्णाङ्क		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    पूर्णाङ्क		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    विकल्प (start)
    {
	पक्ष DOBUF_FIRST:   buf = firstbuf; भङ्ग;
	पक्ष DOBUF_LAST:    buf = lastbuf;  भङ्ग;
	स्वभाव:	    buf = curbuf;   भङ्ग;
    }
    सचे (start == DOBUF_MOD)	    // find next modified buffer
    {
	याव (गणनम्-- > 0)
	{
	    करोहि
	    {
		buf = buf->b_next;
		सचे (buf == रिक्त)
		    buf = firstbuf;
	    }
	    याव (buf != curbuf && !bufIsChanged(buf));
	}
	सचे (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    पटिनिवत्त FAIL;
	}
    }
    अञ्ञथा सचे (start == DOBUF_FIRST && गणनम्) // find specified buffer number
    {
	याव (buf != रिक्त && buf->b_fnum != गणनम्)
	    buf = buf->b_next;
    }
    अञ्ञथा
    {
	पूर्णाङ्क help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = रिक्त;
	याव (गणनम् > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    सचे (bp == रिक्त)
		bp = buf;
	    सचे (dir == FORWARD)
	    {
		buf = buf->b_next;
		सचे (buf == रिक्त)
		    buf = firstbuf;
	    }
	    अञ्ञथा
	    {
		buf = buf->b_prev;
		सचे (buf == रिक्त)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    सचे (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--गणनम्;
		bp = रिक्त;	// use this buffer as new starting point
	    }
	    सचे (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		पटिनिवत्त FAIL;
	    }
	}
    }

    सचे (buf == रिक्त)	    // could not find it
    {
	सचे (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    सचे (!unload)
		semsg(_(e_buffer_nr_does_not_exist), गणनम्);
	}
	अञ्ञथा सचे (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	अञ्ञथा
	    emsg(_(e_cannot_go_before_first_buffer));
	पटिनिवत्त FAIL;
    }
#ifdef FEAT_PROP_POPUP
    सचे ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	पटिनिवत्त OK;
#endif
    सचे (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	पटिनिवत्त FAIL;

    सचे ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), गणनम्);
	पटिनिवत्त FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    सचे (unload)
    {
	पूर्णाङ्क	forward;
	bufref_T bufref;

	सचे (!can_unload_buffer(buf))
	    पटिनिवत्त FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	सचे (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == रिक्त && !buf->b_p_bl)
	    पटिनिवत्त FAIL;

	सचे ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#सचे defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    सचे ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		सचे (term_job_running(buf->b_term))
		{
		    सचे (term_confirm_stop(buf) == FAIL)
			पटिनिवत्त FAIL;
		}
		अञ्ञथा
# endif
		{
		    dialog_changed(buf, FALSE);
		    सचे (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			पटिनिवत्त FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    सचे (bufIsChanged(buf))
			पटिनिवत्त FAIL;
		}
	    }
	    अञ्ञथा
#endif
	    {
		no_write_message_buf(buf);
		पटिनिवत्त FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	सचे (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    सचे (bp->b_p_bl && bp != buf)
		भङ्ग;
	सचे (bp == रिक्त && buf == curbuf)
	    पटिनिवत्त empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	याव (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != रिक्त))
	{
	    सचे (win_close(curwin, FALSE) == FAIL)
		भङ्ग;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	सचे (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    सचे (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(रिक्त, buf, action, FALSE, FALSE, FALSE);
	    पटिनिवत्त OK;
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
	सचे (au_new_curbuf.br_buf != रिक्त && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	अञ्ञथा सचे (curwin->w_jumplistlen > 0)
	{
	    पूर्णाङ्क     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    सचे (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    याव (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		सचे (buf != रिक्त)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    सचे (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = रिक्त;
		    अञ्ञथा सचे (buf->b_ml.ml_mfp == रिक्त)
		    {
			// skip unloaded buf, but may keep it for later
			सचे (bp == रिक्त)
			    bp = buf;
			buf = रिक्त;
		    }
		}
		सचे (buf != रिक्त)   // found a valid buffer: stop searching
		    भङ्ग;
		// advance to older entry in jump list
		सचे (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    भङ्ग;
		सचे (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		सचे (jumpidx == forward)		// List exhausted for sure
		    भङ्ग;
	    }
	}

	सचे (buf == रिक्त)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    अत्थाय (;;)
	    {
		सचे (buf == रिक्त)
		{
		    सचे (!forward)	// tried both directions
			भङ्ग;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    अनुवर्त;
		}
		// in non-help buffer, try to skip help buffers, and vv
		सचे (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    सचे (buf->b_ml.ml_mfp != रिक्त)   // found loaded buffer
			भङ्ग;
		    सचे (bp == रिक्त)	// remember unloaded buf for later
			bp = buf;
		}
		सचे (forward)
		    buf = buf->b_next;
		अञ्ञथा
		    buf = buf->b_prev;
	    }
	}
	सचे (buf == रिक्त)	// No loaded buffer, use unloaded one
	    buf = bp;
	सचे (buf == रिक्त)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		सचे (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    भङ्ग;
	}
	सचे (buf == रिक्त)	// Still no buffer, just take one
	{
	    सचे (curbuf->b_next != रिक्त)
		buf = curbuf->b_next;
	    अञ्ञथा
		buf = curbuf->b_prev;
	    सचे (bt_quickfix(buf)
		    || (buf != रिक्त && buf != curbuf && buf->b_locked_split))
		buf = रिक्त;
	}
    }

    सचे (buf == रिक्त)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	पटिनिवत्त empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    सचे (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != रिक्त)
	पटिनिवत्त OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    सचे (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	पटिनिवत्त FAIL;
    }
    सचे (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	पटिनिवत्त FAIL;

    // go to current buffer - nothing to do
    सचे (buf == curbuf)
	पटिनिवत्त OK;

    // Check if the current buffer may be abandoned.
    सचे (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#सचे defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	सचे ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    सचे (term_job_running(curbuf->b_term))
	    {
		सचे (term_confirm_stop(curbuf) == FAIL)
		    पटिनिवत्त FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    अञ्ञथा
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		सचे (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    पटिनिवत्त FAIL;

		सचे (bufIsChanged(curbuf))
		{
		    no_write_message();
		    पटिनिवत्त FAIL;
		}
	    }
	}
	अञ्ञथा
#endif
	{
	    no_write_message();
	    पटिनिवत्त FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    सचे (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#सचे defined(FEAT_EVAL)
    सचे (aborting())	    // autocmds may abort script processing
	पटिनिवत्त FAIL;
#endif

    पटिनिवत्त OK;
}

    पूर्णाङ्क
do_buffer(
    पूर्णाङ्क		action,
    पूर्णाङ्क		start,
    पूर्णाङ्क		dir,		// FORWARD or BACKWARD
    पूर्णाङ्क		गणनम्,		// buffer number or number of buffers
    पूर्णाङ्क		forceit)	// TRUE when using !
{
    पटिनिवत्त do_buffer_ext(action, start, dir, गणनम्,
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
    वर्ण *
do_bufdel(
    पूर्णाङ्क		command,
    char_u	*arg,		// pointer to extra arguments
    पूर्णाङ्क		addr_गणनम्,
    पूर्णाङ्क		start_bnr,	// first buffer number in a range
    पूर्णाङ्क		end_bnr,	// buffer nr or last buffer nr in a range
    पूर्णाङ्क		forceit)
{
    पूर्णाङ्क		do_current = 0;	// delete current buffer?
    पूर्णाङ्क		deleted = 0;	// number of buffers deleted
    वर्ण	*errormsg = रिक्त; // return value
    पूर्णाङ्क		bnr;		// buffer number
    char_u	*p;

    सचे (addr_गणनम् == 0)
    {
	(शून्य)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    अञ्ञथा
    {
	सचे (addr_गणनम् == 2)
	{
	    सचे (*arg)		// both range and argument is not allowed
		पटिनिवत्त ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	अञ्ञथा	// addr_count == 1
	    bnr = end_bnr;

	अत्थाय ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    सचे (bnr == curbuf->b_fnum)
		do_current = bnr;
	    अञ्ञथा सचे (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    सचे (addr_गणनम् == 2)
	    {
		सचे (++bnr > end_bnr)
		    भङ्ग;
	    }
	    अञ्ञथा    // addr_count == 1
	    {
		arg = skipwhite(arg);
		सचे (*arg == NUL)
		    भङ्ग;
		सचे (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    सचे (bnr < 0)	    // failed
			भङ्ग;
		    arg = p;
		}
		अञ्ञथा
		    bnr = getdigits(&arg);
	    }
	}
	सचे (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	सचे (deleted == 0)
	{
	    सचे (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    अञ्ञथा सचे (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    अञ्ञथा
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (वर्ण *)IObuff;
	}
	अञ्ञथा सचे (deleted >= p_report)
	{
	    सचे (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    अञ्ञथा सचे (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    अञ्ञथा
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    पटिनिवत्त errormsg;
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
set_curbuf(buf_T *buf, पूर्णाङ्क action)
{
    buf_T	*prevbuf;
    पूर्णाङ्क		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    पूर्णाङ्क		valid;

    setpcmark();
    सचे ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    सचे (!apply_autocmds(EVENT_BUFLEAVE, रिक्त, रिक्त, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	सचे (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	सचे (unload)
	    close_windows(prevbuf, FALSE);
#सचे defined(FEAT_EVAL)
	सचे (bufref_valid(&prevbufref) && !aborting())
#अञ्ञथा
	सचे (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    सचे (prevbuf == curbuf
		    && ((State & MODE_निवेशय) == 0 || curbuf->b_nwindows <= 1))
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
    सचे ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == रिक्त)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	सचे (!valid)
	    enter_buffer(lastbuf);
	अञ्ञथा
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	सचे (old_tw != curbuf->b_p_tw)
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
    सचे (VIsual_active
#सचे defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    सचे (curwin->w_buffer != रिक्त)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    सचे (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    अञ्ञथा
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    सचे (curwin->w_p_diff)
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
    सचे (curbuf->b_ml.ml_mfp == रिक्त)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	सचे (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, रिक्त, 0);
    }
    अञ्ञथा
    {
	सचे (!सन्देशः_silent && !shortmess(SHM_FILEINFO))
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
    सचे (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    सचे (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    सचे (curbuf->b_kmap_state & KEYMAP_INIT)
	(शून्य)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    सचे (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(शून्य)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#सचे defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    शून्य
do_autochdir(शून्य)
{
    सचे ((starting == 0 || test_autochdir)
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
    सचे (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    अञ्ञथा
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    शून्य
no_write_message(शून्य)
{
#ifdef FEAT_TERMINAL
    सचे (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    अञ्ञथा
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    शून्य
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    सचे (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    अञ्ञथा
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
    पूर्णाङ्क
curbuf_reusable(शून्य)
{
    पटिनिवत्त (curbuf != रिक्त
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
    पूर्णाङ्क		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    सचे (शिखरम्_file_अङ्कः == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    सचे (sfname == रिक्त || mch_stat((वर्ण *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    सचे (ffname != रिक्त && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#अञ्ञथा
		buflist_findname(ffname)
#endif
		) != रिक्त)
    {
	vim_free(ffname);
	सचे (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? रिक्त : curwin,
						      lnum, (colnr_T)0, FALSE);

	सचे ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	सचे ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    सचे (!(flags & BLN_DUMMY))
	    {
		सचे (apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, buf)
			&& !bufref_valid(&bufref))
		    पटिनिवत्त रिक्त;
	    }
	}
	पटिनिवत्त buf;
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
    सचे ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	सचे (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    पटिनिवत्त रिक्त;
	}
#endif
	सचे (!bufref_valid(&bufref))
	    buf = रिक्त;		// buf was deleted; allocate a new buffer
    }
    सचे (buf != curbuf || curbuf == रिक्त)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	सचे (buf == रिक्त)
	{
	    vim_free(ffname);
	    पटिनिवत्त रिक्त;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	सचे (buf->b_vars == रिक्त)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    पटिनिवत्त रिक्त;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    सचे (ffname != रिक्त)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    सचे ((ffname != रिक्त && (buf->b_ffname == रिक्त || buf->b_sfname == रिक्त))
	    || buf->b_wininfo == रिक्त)
    {
	सचे (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	अञ्ञथा
	    buf->b_sfname = रिक्त;
	VIM_CLEAR(buf->b_ffname);
	सचे (buf != curbuf)
	    free_buffer(buf);
	पटिनिवत्त रिक्त;
    }

    सचे (buf == curbuf)
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
    अञ्ञथा
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = रिक्त;
	सचे (firstbuf == रिक्त)		// buffer list is empty
	{
	    buf->b_prev = रिक्त;
	    firstbuf = buf;
	}
	अञ्ञथा				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	सचे ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((पूर्णाङ्क *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    याव (buf->b_prev != रिक्त && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		सचे (prev->b_next != रिक्त)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		सचे (buf->b_prev != रिक्त)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		सचे (lastbuf == buf)
		    lastbuf = prev;
		सचे (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	अञ्ञथा
	    buf->b_fnum = शिखरम्_file_अङ्कः++;
	सचे (शिखरम्_file_अङ्कः < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    सचे (emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    शिखरम्_file_अङ्कः = 1;
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
    सचे (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    अञ्ञथा
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    सचे (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    सचे (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	सचे (apply_autocmds(EVENT_BUFNEW, रिक्त, रिक्त, FALSE, buf)
		&& !bufref_valid(&bufref))
	    पटिनिवत्त रिक्त;
	सचे (flags & BLN_LISTED)
	{
	    सचे (apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, buf)
		    && !bufref_valid(&bufref))
		पटिनिवत्त रिक्त;
	}
#ifdef FEAT_EVAL
	सचे (aborting())		// autocmds may abort script processing
	    पटिनिवत्त रिक्त;
#endif
    }

    पटिनिवत्त buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    शून्य
free_buf_options(
    buf_T	*buf,
    पूर्णाङ्क		free_p_ff)
{
    सचे (free_p_ff)
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
#सचे defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#सचे defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#सचे defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#सचे defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    सचे (buf->b_p_कुञ्चिका != रिक्त && *buf->b_p_कुञ्चिका != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_कुञ्चिका, STRLEN(buf->b_p_कुञ्चिका));
# endif
    clear_string_option(&buf->b_p_कुञ्चिका);
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
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_गणनम्);
    buf->b_p_cpt_गणनम् = 0;
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
    पूर्णाङ्क
buflist_getfile(
    पूर्णाङ्क		n,
    linenr_T	lnum,
    पूर्णाङ्क		options,
    पूर्णाङ्क		forceit)
{
    buf_T	*buf;
    win_T	*wp = रिक्त;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    सचे (buf == रिक्त)
    {
	सचे ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	अञ्ञथा
	    semsg(_(e_buffer_nr_not_found), n);
	पटिनिवत्त FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    सचे (buf == curbuf)
	पटिनिवत्त OK;

    सचे (text_or_buf_locked())
	पटिनिवत्त FAIL;

    // altfpos may be changed by getfile(), get it now
    सचे (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    अञ्ञथा
	col = 0;

    सचे (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	सचे (wp == रिक्त && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    सचे (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    अञ्ञथा सचे (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		पटिनिवत्त FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    पूर्णाङ्क retval = FAIL;
    सचे (GETFILE_SUCCESS(getfile(buf->b_fnum, रिक्त, रिक्त,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	सचे (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    सचे (RedrawingDisabled > 0)
	--RedrawingDisabled;
    पटिनिवत्त retval;
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

    सचे (p_sol)
	curwin->w_cursor.col = 0;
    अञ्ञथा
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
#अञ्ञथा
	    FALSE
#endif
	    );
    सचे (ffname != रिक्त)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    पटिनिवत्त buf;
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

    सचे (mch_stat((वर्ण *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    पटिनिवत्त buflist_findname_stat(ffname, &st);
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
	सचे ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    पटिनिवत्त buf;
    पटिनिवत्त रिक्त;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    पूर्णाङ्क
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    पूर्णाङ्क		unlisted,	// find unlisted buffers
    पूर्णाङ्क		diffmode UNUSED, // find diff-mode buffers only
    पूर्णाङ्क		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    पूर्णाङ्क		match = -1;
    पूर्णाङ्क		find_listed;
    char_u	*pat;
    char_u	*patend;
    पूर्णाङ्क		attempt;
    char_u	*p;
    पूर्णाङ्क		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    सचे ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	सचे (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	अञ्ञथा
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	सचे (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
    अञ्ञथा
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, रिक्त, FALSE);
	सचे (pat == रिक्त)
	    पटिनिवत्त -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	अत्थाय (;;)
	{
	    अत्थाय (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		सचे (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		सचे (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    सचे (regmatch.regprog == रिक्त)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			पटिनिवत्त -1;
		    }
		    सचे (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != रिक्त)
		    {
			सचे (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				सचे (wp->w_buffer == buf)
				    भङ्ग;
			    सचे (wp == रिक्त)
				अनुवर्त;
			}
			सचे (match >= 0)		// already found a match
			{
			    match = -2;
			    भङ्ग;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		सचे (match >= 0)			// found one match
		    भङ्ग;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    सचे (!unlisted || !find_listed || match != -1)
		भङ्ग;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    सचे (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    अञ्ञथा सचे (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    पटिनिवत्त match;
}

#ifdef FEAT_VIMINFO
प्रकारनिर्देश संरचना {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    पूर्णाङ्क
ExpandBufnames(
    char_u	*pat,
    पूर्णाङ्क		*अङ्कः_file,
    char_u	***file,
    पूर्णाङ्क		options)
{
    पूर्णाङ्क		गणनम्;
    buf_T	*buf;
    पूर्णाङ्क		round;
    char_u	*p;
    char_u	*patc = रिक्त;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = रिक्त;
#endif
    पूर्णाङ्क		fuzzy;
    fuzmatch_str_T  *fuzmatch = रिक्त;
    regmatch_T	regmatch;
    पूर्णाङ्क		score = 0;
    पूर्णाङ्क		to_free = FALSE;

    *अङ्कः_file = 0;		    // return values in case of FAIL
    *file = रिक्त;

#ifdef FEAT_DIFF
    सचे ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	पटिनिवत्त FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    सचे (!fuzzy)
    {
	सचे (*pat == '^' && pat[1] != NUL)
	{
	    पूर्णाङ्क  len = (पूर्णाङ्क)STRLEN(pat);
	    patc = alloc(len);
	    सचे (patc == रिक्त)
		पटिनिवत्त FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	अञ्ञथा सचे (*pat == '^')
	    patc = (char_u *)"";
	अञ्ञथा
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    अत्थाय (round = 1; round <= 2; ++round)
    {
	गणनम् = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    सचे (!buf->b_p_bl)	// skip unlisted buffers
		अनुवर्त;
#ifdef FEAT_DIFF
	    सचे (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		सचे (buf == curbuf || !diff_mode_buf(buf))
		    अनुवर्त;
#endif

	    सचे (!fuzzy)
	    {
		सचे (regmatch.regprog == रिक्त)
		{
		    // invalid pattern, possibly after recompiling
		    सचे (to_free)
			vim_free(patc);
		    पटिनिवत्त FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    अञ्ञथा
	    {
		p = रिक्त;
		// first try matching with the short file name
		सचे ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		सचे (p == रिक्त)
		{
		    // next try matching with the full path file name
		    सचे ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    सचे (p == रिक्त)
		अनुवर्त;

	    सचे (round == 1)
	    {
		++गणनम्;
		अनुवर्त;
	    }

	    सचे (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    अञ्ञथा
		p = vim_strsave(p);
	    सचे (p == रिक्त)
	    {
		सचे (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, गणनम्);
		पटिनिवत्त FAIL;
	    }

	    सचे (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		सचे (matches != रिक्त)
		{
		    matches[गणनम्].buf = buf;
		    matches[गणनम्].match = p;
		    गणनम्++;
		}
		अञ्ञथा
#endif
		    (*file)[गणनम्++] = p;
	    }
	    अञ्ञथा
	    {
		fuzmatch[गणनम्].idx = गणनम्;
		fuzmatch[गणनम्].str = p;
		fuzmatch[गणनम्].score = score;
		गणनम्++;
	    }
	}
	सचे (गणनम् == 0)	// no match found, break here
	    भङ्ग;
	सचे (round == 1)
	{
	    सचे (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, गणनम्);
		सचे (*file == रिक्त)
		{
		    vim_regfree(regmatch.regprog);
		    सचे (to_free)
			vim_free(patc);
		    पटिनिवत्त FAIL;
		}
#ifdef FEAT_VIMINFO
		सचे (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, गणनम्);
#endif
	    }
	    अञ्ञथा
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, गणनम्);
		सचे (fuzmatch == रिक्त)
		{
		    *अङ्कः_file = 0;
		    *file = रिक्त;
		    पटिनिवत्त FAIL;
		}
	    }
	}
    }

    सचे (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	सचे (to_free)
	    vim_free(patc);
    }

    सचे (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	सचे (matches != रिक्त)
	{
	    पूर्णाङ्क i;
	    सचे (गणनम् > 1)
		qsort(matches, गणनम्, परिमाण(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    सचे (matches[0].buf == curbuf)
	    {
		अत्थाय (i = 1; i < गणनम्; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[गणनम्-1] = matches[0].match;
	    }
	    अञ्ञथा
	    {
		अत्थाय (i = 0; i < गणनम्; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    अञ्ञथा
    {
	सचे (fuzzymatches_to_strmatches(fuzmatch, file, गणनम्, FALSE) == FAIL)
	    पटिनिवत्त FAIL;
    }

    *अङ्कः_file = गणनम्;
    पटिनिवत्त (गणनम् == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    स्थिर char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    पूर्णाङ्क		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    सचे (match == रिक्त && rmp->regprog != रिक्त)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    पटिनिवत्त match;
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
    पूर्णाङ्क		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = रिक्त;
    char_u	*p;

    // extra check for valid arguments
    सचे (name == रिक्त || rmp->regprog == रिक्त)
	पटिनिवत्त रिक्त;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    सचे (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    अञ्ञथा सचे (rmp->regprog != रिक्त)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(रिक्त, name);
	सचे (p != रिक्त && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    पटिनिवत्त match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(पूर्णाङ्क nr)
{
    char_u	कुञ्चिका[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    सचे (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((वर्ण *)कुञ्चिका, परिमाण(कुञ्चिका), "%x", nr);
    hi = hash_find(&buf_hashtab, कुञ्चिका);

    सचे (!HASHITEM_EMPTY(hi))
	पटिनिवत्त (buf_T *)(hi->hi_कुञ्चिका
			     - ((unsigned)(curbuf->b_कुञ्चिका - (char_u *)curbuf)));
    पटिनिवत्त रिक्त;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    पूर्णाङ्क		n,
    पूर्णाङ्क		fullname,
    पूर्णाङ्क		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    सचे (buf == रिक्त)
	पटिनिवत्त रिक्त;
    पटिनिवत्त home_replace_save(helptail ? buf : रिक्त,
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
    पूर्णाङ्क		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	सचे (wip->wi_win == win)
	    भङ्ग;
    सचे (wip == रिक्त)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	सचे (wip == रिक्त)
	    पटिनिवत्त;
	wip->wi_win = win;
	सचे (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    अञ्ञथा
    {
	// remove the entry from the list
	सचे (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	अञ्ञथा
	    buf->b_wininfo = wip->wi_next;
	सचे (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	सचे (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    सचे (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    सचे (win != रिक्त)
	wip->wi_changelistidx = win->w_changelistidx;
    सचे (copy_options && win != रिक्त)
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
    सचे (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    स्थिर पूर्णाङ्क
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    सचे (!wip->wi_opt.wo_diff)
	पटिनिवत्त FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	सचे (wip->wi_win == wp)
	    पटिनिवत्त FALSE;
    पटिनिवत्त TRUE;
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
    पूर्णाङ्क		need_options,
    पूर्णाङ्क		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	सचे (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    भङ्ग;

    सचे (wip != रिक्त)
	पटिनिवत्त wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    सचे (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    सचे (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != रिक्त
			    && wip->wi_win->w_buffer == buf)))
		भङ्ग;
    }
    अञ्ञथा
#endif
	wip = buf->b_wininfo;
    पटिनिवत्त wip;
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
    सचे (wip != रिक्त && wip->wi_win != रिक्त
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
    अञ्ञथा सचे (wip != रिक्त && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    अञ्ञथा
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    सचे (wip != रिक्त)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    सचे (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    सचे (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    सचे (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	सचे (p_ea)
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
    सचे (wip != रिक्त)
	पटिनिवत्त &(wip->wi_fpos);
    अञ्ञथा
	पटिनिवत्त &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    पटिनिवत्त buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    शून्य
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    पूर्णाङ्क		len;
    पूर्णाङ्क		i;
    पूर्णाङ्क		ro_char;
    पूर्णाङ्क		changed_char;
#ifdef FEAT_TERMINAL
    पूर्णाङ्क		job_running;
    पूर्णाङ्क		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = रिक्त, **p;

    सचे (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, परिमाण(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    सचे (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_data, (size_t)buflist.ga_len,
		परिमाण(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    अत्थाय (; buf != रिक्त && !got_int; buf = buflist_data != रिक्त
	    ? (++p < buflist_data + buflist.ga_len ? *p : रिक्त)
	    : buf->b_next)
#अञ्ञथा
    अत्थाय (buf = firstbuf; buf != रिक्त && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	सचे ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
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
	    अनुवर्त;
	name = buf_spname(buf);
	सचे (name != रिक्त)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	अञ्ञथा
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	सचे (message_filtered(NameBuff))
	    अनुवर्त;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	सचे (job_running)
	{
	    सचे (job_none_open)
		ro_char = '?';
	    अञ्ञथा
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	अञ्ञथा सचे (buf->b_term != रिक्त)
	    ro_char = 'F';
	अञ्ञथा
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	सन्देशः_putchar('\n');
	len = (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
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
	करोहि
	    IObuff[len++] = ' ';
	याव (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	सचे (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (size_t)(IOSIZE - len), buf->b_last_used);
	अञ्ञथा
#endif
	    vim_snprintf((वर्ण *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (long)buflist_findlnum(buf));
	सन्देशः_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    सचे (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    पूर्णाङ्क
buflist_name_nr(
    पूर्णाङ्क		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    सचे (buf == रिक्त || buf->b_fname == रिक्त)
	पटिनिवत्त FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    पटिनिवत्त OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    पूर्णाङ्क
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    पूर्णाङ्क		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = रिक्त;
#ifdef UNIX
    stat_T	st;
#endif

    सचे (ffname == रिक्त || *ffname == NUL)
    {
	// Removing the name.
	सचे (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	अञ्ञथा
	    buf->b_sfname = रिक्त;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    अञ्ञथा
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	सचे (ffname == रिक्त)		    // out of memory
	    पटिनिवत्त FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	सचे (mch_stat((वर्ण *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	सचे (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#अञ्ञथा
	    obuf = buflist_findname(ffname);
#endif
	सचे (obuf != रिक्त && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    पूर्णाङ्क		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		सचे (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    सचे (obuf->b_ml.ml_mfp != रिक्त || in_use)
	    {
		सचे (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		पटिनिवत्त FAIL;
	    }
	    // delete from the list
	    close_buffer(रिक्त, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	सचे (ffname == रिक्त || sfname == रिक्त)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    पटिनिवत्त FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	सचे (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    सचे (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    अञ्ञथा
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    पटिनिवत्त OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    शून्य
buf_set_name(पूर्णाङ्क fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    सचे (buf == रिक्त)
	पटिनिवत्त;

    सचे (buf->b_sfname != buf->b_ffname)
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
    सचे (buf->b_ml.ml_mfp != रिक्त)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    सचे (buf->b_term != रिक्त)
	term_clear_status_text(buf->b_term);
#endif

    सचे (curwin->w_buffer == buf)
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
    सचे (buf != रिक्त && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    पटिनिवत्त buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    पूर्णाङ्क		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    सचे (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	सचे (errmsg)
	    emsg(_(e_no_alternate_file));
	पटिनिवत्त रिक्त;
    }
    पटिनिवत्त fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    पूर्णाङ्क
buflist_add(char_u *fname, पूर्णाङ्क flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, रिक्त, (linenr_T)0, flags);
    सचे (buf != रिक्त)
	पटिनिवत्त buf->b_fnum;
    पटिनिवत्त 0;
}

#सचे defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    शून्य
buflist_slash_adjust(शून्य)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	सचे (bp->b_ffname != रिक्त)
	    slash_adjust(bp->b_ffname);
	सचे (bp->b_sfname != रिक्त)
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
    पूर्णाङ्क
otherfile(char_u *ffname)
{
    पटिनिवत्त otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , रिक्त
#endif
	    );
}

    स्थिर पूर्णाङ्क
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    सचे (ffname == रिक्त || *ffname == NUL || buf->b_ffname == रिक्त)
	पटिनिवत्त TRUE;
    सचे (fnamecmp(ffname, buf->b_ffname) == 0)
	पटिनिवत्त FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	सचे (stp == रिक्त)
	{
	    सचे (!buf->b_dev_valid || mch_stat((वर्ण *)ffname, &st) < 0)
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
	सचे (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    सचे (buf_same_ino(buf, stp))
		पटिनिवत्त FALSE;
	}
    }
#endif
    पटिनिवत्त TRUE;
}

#सचे defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    शून्य
buf_setino(buf_T *buf)
{
    stat_T	st;

    सचे (buf->b_fname != रिक्त && mch_stat((वर्ण *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    अञ्ञथा
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    स्थिर पूर्णाङ्क
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    पटिनिवत्त (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    शून्य
fileinfo(
    पूर्णाङ्क fullname,	    // when non-zero print full path
    पूर्णाङ्क shorthelp,
    पूर्णाङ्क	dont_truncate)
{
    char_u	*name;
    वर्ण	*buffer;
    size_t	bufferlen = 0;

    buffer = alloc(IOSIZE);
    सचे (buffer == रिक्त)
	पटिनिवत्त;

    सचे (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    सचे (name != रिक्त)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    अञ्ञथा
    {
	सचे (!fullname && curbuf->b_fname != रिक्त)
	    name = curbuf->b_fname;
	अञ्ञथा
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : रिक्त, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (पूर्णाङ्क)bufferlen, TRUE);
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

    सचे (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_पङ्क्तयः_सन्देशः));
    अञ्ञथा सचे (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_पङ्क्तिः_गणनम्),
	    (long)curbuf->b_ml.ml_पङ्क्तिः_गणनम्,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_पङ्क्तिः_गणनम्));
    अञ्ञथा
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (long)curwin->w_cursor.lnum,
	    (long)curbuf->b_ml.ml_पङ्क्तिः_गणनम्,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_पङ्क्तिः_गणनम्));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (पूर्णाङ्क)curwin->w_cursor.col + 1, (पूर्णाङ्क)curwin->w_virtcol + 1);
    }

    (शून्य)append_arg_सङ्ख्या(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    सचे (dont_truncate)
    {
	पूर्णाङ्क	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	सन्देशः_start();
	n = सन्देशः_scroll;
	सन्देशः_scroll = TRUE;
	सन्देशः(buffer);
	सन्देशः_scroll = n;
    }
    अञ्ञथा
    {
	वर्ण	*p = सन्देशः_trunc_attr(buffer, FALSE, 0);
	सचे (restart_edit != 0 || (सन्देशः_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_सन्देशः((char_u *)p, 0);
    }

    vim_free(buffer);
}

    पूर्णाङ्क
col_print(
    char_u  *buf,
    size_t  buflen,
    पूर्णाङ्क	    col,
    पूर्णाङ्क	    vcol)
{
    सचे (col == vcol)
	पटिनिवत्त (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen, "%d", col);

    पटिनिवत्त (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen, "%d-%d", col, vcol);
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
    पूर्णाङ्क		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    सचे (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	पटिनिवत्त;
    }

    need_maketitle = FALSE;
    सचे (!p_title && !p_icon && lasttitle == रिक्त && lasticon == रिक्त)
	पटिनिवत्त;  // nothing to do

    सचे (p_title)
    {
	पूर्णाङ्क maxlen = 0;

	सचे (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    सचे (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	सचे (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    सचे (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, परिमाण(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, रिक्त, रिक्त, रिक्त);
	    अञ्ञथा
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	अञ्ञथा
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (परिमाण(buf) - 100)
#define SPACE_FOR_DIR   (परिमाण(buf) - 20)
#define SPACE_FOR_ARGNR (परिमाण(buf) - 10)  // at least room for " - VIM"

	    // file name
	    सचे (curbuf->b_fname == रिक्त)
		buflen = vim_snprintf_safelen((वर्ण *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    अञ्ञथा सचे (curbuf->b_term != रिक्त)
		buflen = vim_snprintf_safelen((वर्ण *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    अञ्ञथा
	    {
		buflen = vim_snprintf_safelen((वर्ण *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != रिक्त)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    सचे (curbuf->b_term == रिक्त)
#endif
	    {
		विकल्प (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    पक्ष 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (वर्ण *)buf + buflen,
			    परिमाण(buf) - buflen, " +");
			भङ्ग;
		    पक्ष 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (वर्ण *)buf + buflen,
			    परिमाण(buf) - buflen, " =");
			भङ्ग;
		    पक्ष 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (वर्ण *)buf + buflen,
			    परिमाण(buf) - buflen, " =+");
			भङ्ग;
		    पक्ष 4:
		    पक्ष 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (वर्ण *)buf + buflen,
			    परिमाण(buf) - buflen, " -");
			भङ्ग;
		    पक्ष 5:
		    पक्ष 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (वर्ण *)buf + buflen,
			    परिमाण(buf) - buflen, " -+");
			भङ्ग;
		    स्वभाव:
			भङ्ग;
		}
	    }

	    // path (surrounded by '()')
	    सचे (curbuf->b_fname != रिक्त
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == रिक्त
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((वर्ण *)buf + buflen,
		    परिमाण(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (पूर्णाङ्क)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		सचे (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		सचे (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((वर्ण *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		अञ्ञथा
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    सचे (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((वर्ण *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != रिक्त)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    अञ्ञथा
			buflen += vim_snprintf_safelen((वर्ण *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_सङ्ख्या(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((वर्ण *)buf + buflen,
		परिमाण(buf) - buflen, " - %s",
#सचे defined(FEAT_CLIENTSERVER)
		(serverName != रिक्त)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    सचे (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		सचे (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, परिमाण(buf));
	    }
	}
    }
    mustset = मानम्_changed(title_str, &lasttitle);

    सचे (p_icon)
    {
	icon_str = buf;
	सचे (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    सचे (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, परिमाण(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, रिक्त, रिक्त,
				 रिक्त);
	    अञ्ञथा
#endif
		icon_str = p_iconstring;
	}
	अञ्ञथा
	{
	    char_u  *name;
	    पूर्णाङ्क	    namelen;

	    name = buf_spname(curbuf);
	    सचे (name == रिक्त)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (पूर्णाङ्क)STRLEN(name);
	    सचे (namelen > 100)
	    {
		namelen -= 100;
		सचे (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, परिमाण(buf));
	}
    }

    mustset |= मानम्_changed(icon_str, &lasticon);

    सचे (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    स्थिर पूर्णाङ्क
मानम्_changed(char_u *str, char_u **last)
{
    सचे ((str == रिक्त) != (*last == रिक्त)
	    || (str != रिक्त && *last != रिक्त && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	सचे (str == रिक्त)
	{
	    *last = रिक्त;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	अञ्ञथा
	{
	    *last = vim_strsave(str);
	    पटिनिवत्त TRUE;
	}
    }
    पटिनिवत्त FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    शून्य
resettitle(शून्य)
{
    mch_settitle(lasttitle, lasticon);
}

#सचे defined(EXITFREE)
    शून्य
free_titles(शून्य)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#सचे defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
प्रकारनिर्देश संरचना
{
    char_u	*stl_start;
    पूर्णाङ्क		stl_minwid;
    पूर्णाङ्क		stl_maxwid;
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

स्थिर size_t		stl_वस्तूनि_len = 20; // Initial value, grows as needed.
स्थिर stl_item_T      *stl_वस्तूनि = रिक्त;
स्थिर पूर्णाङ्क	       *stl_groupitem = रिक्त;
स्थिर stl_hlrec_T     *stl_hltab = रिक्त;
स्थिर stl_hlrec_T     *stl_tabtab = रिक्त;
स्थिर stl_clickrec_T  *stl_clicktab = रिक्त;
स्थिर पूर्णाङ्क		*stl_separator_locations = रिक्त;

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
    पूर्णाङ्क
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णाङ्क		opt_scope,	// scope for "opt_name"
    पूर्णाङ्क		fillchar,
    पूर्णाङ्क		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    पटिनिवत्त build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    रिक्त, रिक्त);
}

    पूर्णाङ्क
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णाङ्क		opt_scope,	// scope for "opt_name"
    पूर्णाङ्क		fillchar,
    पूर्णाङ्क		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    पूर्णाङ्क		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    पटिनिवत्त build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    रिक्त, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    पूर्णाङ्क
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णाङ्क		opt_scope,	// scope for "opt_name"
    पूर्णाङ्क		fillchar,
    पूर्णाङ्क		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    पूर्णाङ्क		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    पटिनिवत्त build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    रिक्त, carry_hl);
}
# endif

    पूर्णाङ्क
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णाङ्क		opt_scope)	// scope for "opt_name"
{
    पूर्णाङ्क rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (शून्य)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, परिमाण(buf), &fmt,
	    opt_name, opt_scope, 0, 0, रिक्त, रिक्त, रिक्त, &rendered_height,
	    रिक्त);
    --emsg_off;
    पटिनिवत्त rendered_height;
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
स्थिर पूर्णाङ्क
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णाङ्क		opt_scope,	// scope for "opt_name"
    पूर्णाङ्क		fillchar,
    पूर्णाङ्क		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    पूर्णाङ्क		*rendered_height,   // return: stl rendered height (can be NULL)
    पूर्णाङ्क		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    पूर्णाङ्क		byteval;
# ifdef FEAT_EVAL
    पूर्णाङ्क		use_sandbox;
    पूर्णाङ्क		save_VIsual_active;
# endif
    पूर्णाङ्क		empty_पङ्क्तिः;
    long	l;
    long	n;
    पूर्णाङ्क		prevchar_isflag;
    पूर्णाङ्क		prevchar_isitem;
    पूर्णाङ्क		itemisflag;
    पूर्णाङ्क		fillable;
    char_u	*str;
    long	अङ्कः;
    पूर्णाङ्क		width;
    पूर्णाङ्क		itemcnt;
    पूर्णाङ्क		curitem;
    पूर्णाङ्क		groupdepth;
# ifdef FEAT_EVAL
    पूर्णाङ्क		evaldepth;
# endif
    पूर्णाङ्क		minwid;
    पूर्णाङ्क		maxwid;
    पूर्णाङ्क		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    पूर्णाङ्क		save_redraw_not_allowed = redraw_not_allowed;
    पूर्णाङ्क		save_कुञ्चिका_Typed = कुञ्चिका_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    पूर्णाङ्क		did_emsg_before = did_emsg;
    पूर्णाङ्क		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    सचे (updating_screen)
	redraw_not_allowed = TRUE;

    सचे (stl_वस्तूनि == रिक्त)
    {
	stl_वस्तूनि = ALLOC_MULT(stl_item_T, stl_वस्तूनि_len);
	stl_groupitem = ALLOC_MULT(पूर्णाङ्क, stl_वस्तूनि_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_वस्तूनि_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_वस्तूनि_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_वस्तूनि_len + 1);

	stl_separator_locations = ALLOC_MULT(पूर्णाङ्क, stl_वस्तूनि_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    सचे (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_सङ्ख्या;
	tv.vval.v_सङ्ख्या = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	सचे (usefmt == रिक्त)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    सचे (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    सचे (lnum > wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
    {
	lnum = wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_पङ्क्तिः = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    सचे (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    अञ्ञथा
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    सचे (carry_hl != रिक्त && *carry_hl != 0)
    {
	stl_वस्तूनि[curitem].stl_type = Highlight;
	stl_वस्तूनि[curitem].stl_start = p;
	stl_वस्तूनि[curitem].stl_minwid = *carry_hl;
	stl_वस्तूनि[curitem].stl_clickfunc = रिक्त;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    अत्थाय (s = usefmt; *s != NUL; )
    {
	सचे (curitem == (पूर्णाङ्क)stl_वस्तूनि_len)
	{
	    size_t	new_len = stl_वस्तूनि_len * 3 / 2;

	    stl_item_T *new_वस्तूनि =
			  vim_realloc(stl_वस्तूनि, परिमाण(stl_item_T) * new_len);
	    सचे (new_वस्तूनि == रिक्त)
		भङ्ग;
	    stl_वस्तूनि = new_वस्तूनि;

	    पूर्णाङ्क *new_groupitem =
			     vim_realloc(stl_groupitem, परिमाण(पूर्णाङ्क) * new_len);
	    सचे (new_groupitem == रिक्त)
		भङ्ग;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  परिमाण(stl_hlrec_T) * (new_len + 1));
	    सचे (new_hlrec == रिक्त)
		भङ्ग;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  परिमाण(stl_hlrec_T) * (new_len + 1));
	    सचे (new_hlrec == रिक्त)
		भङ्ग;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      परिमाण(stl_clickrec_T) * (new_len + 1));
	    सचे (new_clickrec == रिक्त)
		भङ्ग;
	    stl_clicktab = new_clickrec;

	    पूर्णाङ्क *new_separator_locs = vim_realloc(stl_separator_locations,
					    परिमाण(पूर्णाङ्क) * new_len);
	    सचे (new_separator_locs == रिक्त)
		भङ्ग;
	    stl_separator_locations = new_separator_locs;

	    stl_वस्तूनि_len = new_len;
	}

	stl_वस्तूनि[curitem].stl_clickfunc = रिक्त;

	सचे (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	याव (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    सचे (*s == '\n' || *s == '\r')
	    {
		सचे (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    गच्छ find_linebreak;
		}
		अञ्ञथा सचे (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	सचे (*s == NUL || p + 1 >= out + outlen)
	    भङ्ग;

	/*
	 * Handle one '%' item.
	 */
	s++;
	सचे (*s == NUL)  // ignore trailing %
	    भङ्ग;

	सचे (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    सचे (s[1] == ']')
	    {
		stl_वस्तूनि[curitem].stl_type = ClickFunc;
		stl_वस्तूनि[curitem].stl_start = p;
		stl_वस्तूनि[curitem].stl_minwid = 0;
		stl_वस्तूनि[curitem].stl_clickfunc = रिक्त;
		s += 2;
		curitem++;
		अनुवर्त;
	    }
	    // %[FuncName] - start click region
	    सचे (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		सचे (rb != रिक्त)
		{
		    stl_वस्तूनि[curitem].stl_type = ClickFunc;
		    stl_वस्तूनि[curitem].stl_start = p;
		    stl_वस्तूनि[curitem].stl_minwid = 0;
		    stl_वस्तूनि[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    अनुवर्त;
		}
	    }
	}
	सचे (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    सचे (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		भङ्ग;
	    }
	    अञ्ञथा सचे (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	सचे (*s == '%')
	{
	    सचे (p + 1 >= out + outlen)
		भङ्ग;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    अनुवर्त;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	सचे (*s == STL_SEPARATE)
	{
	    s++;
	    सचे (groupdepth > 0)
		अनुवर्त;
	    stl_वस्तूनि[curitem].stl_type = Separate;
	    stl_वस्तूनि[curitem++].stl_start = p;
	    अनुवर्त;
	}
	सचे (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_वस्तूनि[curitem].stl_type = Trunc;
	    stl_वस्तूनि[curitem++].stl_start = p;
	    अनुवर्त;
	}
	सचे (*s == ')')
	{
	    char_u  *t;

	    s++;
	    सचे (groupdepth < 1)
		अनुवर्त;
	    groupdepth--;

	    t = stl_वस्तूनि[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    सचे (curitem > stl_groupitem[groupdepth] + 1
		    && stl_वस्तूनि[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		पूर्णाङ्क group_start_userhl = 0;
		पूर्णाङ्क group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		अत्थाय (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    सचे (stl_वस्तूनि[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_वस्तूनि[n].stl_minwid;
			भङ्ग;
		    }
		}
		अत्थाय (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    सचे (stl_वस्तूनि[n].stl_type == Normal)
			भङ्ग;
		    सचे (stl_वस्तूनि[n].stl_type == Highlight)
			group_end_userhl = stl_वस्तूनि[n].stl_minwid;
		}
		सचे (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    अत्थाय (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			सचे (stl_वस्तूनि[n].stl_type == Highlight)
			    stl_वस्तूनि[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			सचे (stl_वस्तूनि[n].stl_type == TabPage)
			    stl_वस्तूनि[n].stl_start = p;
		    }
		}
	    }
	    सचे (l > stl_वस्तूनि[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		सचे (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    याव (l >= stl_वस्तूनि[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		अञ्ञथा
		    n = (long)(p - t) - stl_वस्तूनि[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		पूर्णाङ्क minwid_fixed = MIN(stl_वस्तूनि[stl_groupitem[groupdepth]].stl_minwid,
				       stl_वस्तूनि[stl_groupitem[groupdepth]].stl_maxwid);
		याव (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		अत्थाय (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_वस्तूनि[l].stl_start -= n - 1;
		    सचे (stl_वस्तूनि[l].stl_start < t)
			stl_वस्तूनि[l].stl_start = t;
		}
	    }
	    अञ्ञथा सचे (abs(stl_वस्तूनि[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_वस्तूनि[stl_groupitem[groupdepth]].stl_minwid;
		पूर्णाङ्क fillchar_len = MB_CHAR2LEN(fillchar);
		सचे (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    याव (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		अञ्ञथा
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    सचे (p + l >= out + outlen)
		    {
			n = (long)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    अत्थाय ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    अत्थाय (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_वस्तूनि[n].stl_start += l;
		}
	    }
	    अनुवर्त;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	सचे (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	सचे (*s == '-')
	{
	    s++;
	    l = -1;
	}
	सचे (VIM_ISDIGIT(*s))
	{
	    minwid = (पूर्णाङ्क)getdigits(&s);
	    सचे (minwid < 0)	// overflow
		minwid = 0;
	}
	सचे (*s == STL_USER_HL)
	{
	    stl_वस्तूनि[curitem].stl_type = Highlight;
	    stl_वस्तूनि[curitem].stl_start = p;
	    stl_वस्तूनि[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    अनुवर्त;
	}
	सचे (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    सचे (*s == STL_TABCLOSENR)
	    {
		सचे (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    अत्थाय (n = curitem - 1; n >= 0; --n)
			सचे (stl_वस्तूनि[n].stl_type == TabPage
					       && stl_वस्तूनि[n].stl_minwid >= 0)
			{
			    minwid = stl_वस्तूनि[n].stl_minwid;
			    भङ्ग;
			}
		}
		अञ्ञथा
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_वस्तूनि[curitem].stl_type = TabPage;
	    stl_वस्तूनि[curitem].stl_start = p;
	    stl_वस्तूनि[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    अनुवर्त;
	}
	सचे (*s == '.')
	{
	    s++;
	    सचे (VIM_ISDIGIT(*s))
	    {
		maxwid = (पूर्णाङ्क)getdigits(&s);
		सचे (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	पूर्णाङ्क raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	सचे (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_वस्तूनि[curitem].stl_type = Group;
	    stl_वस्तूनि[curitem].stl_start = p;
	    stl_वस्तूनि[curitem].stl_minwid = minwid;
	    stl_वस्तूनि[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    अनुवर्त;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	सचे (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    अनुवर्त;
	}
# endif
	सचे (vim_strchr(STL_ALL, *s) == रिक्त)
	{
	    सचे (*s == NUL)  // can happen with "%0"
		भङ्ग;
	    s++;
	    अनुवर्त;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	अङ्कः = -1;
	str = रिक्त;
	विकल्प (opt)
	{
	पक्ष STL_FILEPATH:
	पक्ष STL_FULLPATH:
	पक्ष STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    सचे (name != रिक्त)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    अञ्ञथा
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    सचे (opt != STL_FILENAME)
		str = NameBuff;
	    अञ्ञथा
		str = gettail(NameBuff);
	    भङ्ग;
	}

	पक्ष STL_VIM_व्यञ्जकम्: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    पूर्णाङ्क	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    सचे (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    याव ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    सचे (*s != '}')	// missing '}' or out of space
		भङ्ग;
	    s++;
	    सचे (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    अञ्ञथा
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((वर्ण *)buf_tmp, परिमाण(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((वर्ण *)buf_tmp, परिमाण(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    सचे (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    सचे (!zeropad && str != रिक्त && *str != NUL)
	    {
		सचे (*skipdigits(str) == NUL)
		{
		    अङ्कः = atoi((वर्ण *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    सचे (reevaluate && str != रिक्त && *str != NUL
		    && strchr((ध्रुव वर्ण *)str, '%') != रिक्त
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((ध्रुव वर्ण *)str);
		size_t fmt_length = strlen((ध्रुव वर्ण *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * परिमाण(char_u));

		सचे (new_fmt != रिक्त)
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
		    new_fmt_p = रिक्त;

		    सचे (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    अनुवर्त;
		}
	    }
# endif
	    भङ्ग;
	}
	पक्ष STL_पङ्क्तिः:
	    अङ्कः = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    भङ्ग;

	पक्ष STL_NUMLINES:
	    अङ्कः = wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्;
	    भङ्ग;

	पक्ष STL_COLUMN:
	    अङ्कः = (State & MODE_निवेशय) == 0 && empty_पङ्क्तिः
					       ? 0 : (पूर्णाङ्क)wp->w_cursor.col + 1;
	    भङ्ग;

	पक्ष STL_VIRTCOL:
	पक्ष STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    सचे (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_निवेशय) == 0
			       && empty_पङ्क्तिः ? 0 : (पूर्णाङ्क)wp->w_cursor.col + 1)))
		भङ्ग;
	    अङ्कः = (long)virtcol;
	    भङ्ग;
	}

	पक्ष STL_PERCENTAGE:
	    अङ्कः = calc_percentage((long)wp->w_cursor.lnum, (long)wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्);
	    भङ्ग;

	पक्ष STL_ALTPERCENT:
	    str = buf_tmp;
	    (शून्य)get_rel_pos(wp, str, TMPLEN);
	    भङ्ग;

	पक्ष STL_SHOWCMD:
	    सचे (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    भङ्ग;

	पक्ष STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    सचे (append_arg_सङ्ख्या(wp, buf_tmp, परिमाण(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    भङ्ग;

	पक्ष STL_KEYMAP:
	    fillable = FALSE;
	    सचे (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    भङ्ग;
	पक्ष STL_PAGENUM:
# सचे defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    अङ्कः = printer_page_अङ्कः;
# अञ्ञथा
	    अङ्कः = 0;
# endif
	    भङ्ग;

	पक्ष STL_BUFNO:
	    अङ्कः = wp->w_buffer->b_fnum;
	    भङ्ग;

	पक्ष STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	पक्ष STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_पङ्क्तिः_or_offset(wp->w_buffer, wp->w_cursor.lnum, रिक्त);
	    अङ्कः = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_निवेशय) == 0 && empty_पङ्क्तिः
				? 0 : (पूर्णाङ्क)wp->w_cursor.col);
# endif
	    भङ्ग;

	पक्ष STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	पक्ष STL_BYTEVAL:
	    अङ्कः = byteval;
	    सचे (अङ्कः == NL)
		अङ्कः = 0;
	    अञ्ञथा सचे (अङ्कः == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		अङ्कः = NL;
	    भङ्ग;

	पक्ष STL_ROFLAG:
	पक्ष STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    सचे (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    भङ्ग;

	पक्ष STL_HELPFLAG:
	पक्ष STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    सचे (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    भङ्ग;

	पक्ष STL_FILETYPE:
	    सचे (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((वर्ण *)buf_tmp, परिमाण(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    भङ्ग;

	पक्ष STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    सचे (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((वर्ण *)buf_tmp, परिमाण(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		अत्थाय (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    भङ्ग;

# सचे defined(FEAT_QUICKFIX)
	पक्ष STL_PREVIEWFLAG:
	पक्ष STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    सचे (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    भङ्ग;

	पक्ष STL_QUICKFIX:
	    सचे (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(सन्देशः_loclist)
			    : _(सन्देशः_qflist));
	    भङ्ग;
# endif

	पक्ष STL_MODIFIED:
	पक्ष STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    विकल्प ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		पक्ष 2: str = (char_u *)"[+]"; भङ्ग;
		पक्ष 3: str = (char_u *)",+"; भङ्ग;
		पक्ष 4: str = (char_u *)"[-]"; भङ्ग;
		पक्ष 5: str = (char_u *)",-"; भङ्ग;
		पक्ष 6: str = (char_u *)"[+-]"; भङ्ग;
		पक्ष 7: str = (char_u *)",+-"; भङ्ग;
	    }
	    भङ्ग;

	पक्ष STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		याव (*s != '#' && *s != NUL)
		    ++s;
		सचे (*s == '#')
		{
		    stl_वस्तूनि[curitem].stl_type = Highlight;
		    stl_वस्तूनि[curitem].stl_start = p;
		    stl_वस्तूनि[curitem].stl_minwid = -syn_namen2id(t, (पूर्णाङ्क)(s - t));
		    curitem++;
		}
		सचे (*s != NUL)
		    ++s;
		अनुवर्त;
	    }

	पक्ष STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    सचे (*s == ']')
	    {
		stl_वस्तूनि[curitem].stl_type = ClickFunc;
		stl_वस्तूनि[curitem].stl_start = p;
		stl_वस्तूनि[curitem].stl_minwid = 0;
		stl_वस्तूनि[curitem].stl_clickfunc = रिक्त;
		s++;
		curitem++;
		अनुवर्त;
	    }
	    // %N[FuncName] with minwid
	    सचे (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		सचे (rb != रिक्त)
		{
		    stl_वस्तूनि[curitem].stl_type = ClickFunc;
		    stl_वस्तूनि[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_वस्तूनि[curitem].stl_minwid = raw_minwid;
		    stl_वस्तूनि[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    अनुवर्त;
		}
	    }
	    अनुवर्त;

	पक्ष STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    अनुवर्त;
	}

	stl_वस्तूनि[curitem].stl_start = p;
	stl_वस्तूनि[curitem].stl_type = Normal;
	सचे (str != रिक्त && *str)
	{
	    char_u  *t = str;

	    सचे (itemisflag)
	    {
		सचे ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    सचे (l > 0)
		prevchar_isitem = TRUE;
	    सचे (l > maxwid)
	    {
		याव (l >= maxwid)
		    सचे (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    अञ्ञथा
			l -= byte2cells(*t++);
		सचे (p + 1 >= out + outlen)
		    भङ्ग;
		*p++ = '<';
	    }
	    सचे (minwid > 0)
	    {
		अत्थाय (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    सचे (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    अञ्ञथा
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    अञ्ञथा
		minwid *= -1;
	    अत्थाय (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		सचे (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		अञ्ञथा
		    *p++ = *t;
	    }
	    अत्थाय (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	अञ्ञथा सचे (अङ्कः >= 0)
	{
	    पूर्णाङ्क	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    सचे (p + 20 >= out + outlen)
		भङ्ग;		// not sufficient space
	    prevchar_isitem = TRUE;
	    सचे (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    सचे (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    अत्थाय (n = अङ्कः, l = 1; n >= nbase; n /= nbase)
		l++;
	    सचे (opt == STL_VIRTCOL_ALT)
		l++;

	    सचे (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		याव (l-- > maxwid)
		    अङ्कः /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((वर्ण *)p, outlen - (p - out),
		    (वर्ण *)nstr, 0, अङ्कः, n);
	    }
	    अञ्ञथा
		p += vim_snprintf_safelen((वर्ण *)p, outlen - (p - out),
		    (वर्ण *)nstr, minwid, अङ्कः);
	}
	अञ्ञथा
	    stl_वस्तूनि[curitem].stl_type = Empty;

	सचे (अङ्कः >= 0 || (!itemisflag && str != रिक्त && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	सचे (opt == STL_VIM_व्यञ्जकम्)
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
    सचे (carry_hl != रिक्त)
    {
	पूर्णाङ्क last_hl = 0;
	अत्थाय (l = 0; l < itemcnt; l++)
	    सचे (stl_वस्तूनि[l].stl_type == Highlight)
		last_hl = stl_वस्तूनि[l].stl_minwid;
	*carry_hl = last_hl;
    }

    सचे (mode == STL_MODE_MULTI
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
	सचे (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = strlen((वर्ण *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    अञ्ञथा
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	सचे (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    सचे (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	अत्थाय (l = 0; l < itemcnt; l++)
	    सचे (stl_वस्तूनि[l].stl_type == ClickFunc)
		vim_free(stl_वस्तूनि[l].stl_clickfunc);
	सचे (rendered_height != रिक्त)
	    *rendered_height = rheight;
	पटिनिवत्त 0;
    }

    width = vim_strsize(out);
    सचे (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	सचे (itemcnt == 0)
	    s = out;
	अञ्ञथा
	{
	    अत्थाय ( ; l < itemcnt; l++)
		सचे (stl_वस्तूनि[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_वस्तूनि[l].stl_start;
		    भङ्ग;
		}
	    सचे (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_वस्तूनि[0].stl_start;
		l = 0;
	    }
	}

	सचे (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    सचे (has_mbyte)
	    {
		s = out;
		width = 0;
		अत्थाय (;;)
		{
		    width += ptr2cells(s);
		    सचे (width >= maxwidth)
			भङ्ग;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		याव (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    अञ्ञथा
		s = out + maxwidth - 1;
	    अत्थाय (l = 0; l < itemcnt; l++)
		सचे (stl_वस्तूनि[l].stl_start > s)
		    भङ्ग;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	अञ्ञथा
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    याव (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(end - p) + 1);	// +1 for NUL
	    end -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    अत्थाय (; l < itemcnt; l++)
	    {
		सचे (stl_वस्तूनि[l].stl_start - n >= s)
		    stl_वस्तूनि[l].stl_start -= n;
		अञ्ञथा
		    stl_वस्तूनि[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    याव (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    अञ्ञथा सचे (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	पूर्णाङ्क अङ्कः_separators = 0;

	अत्थाय (l = 0; l < itemcnt; l++)
	{
	    सचे (stl_वस्तूनि[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[अङ्कः_separators] = l;
		अङ्कः_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	सचे (अङ्कः_separators)
	{
	    पूर्णाङ्क standard_spaces;
	    पूर्णाङ्क final_spaces;

	    standard_spaces = (maxwidth - width) / अङ्कः_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (अङ्कः_separators - 1);
	    अत्थाय (l = 0; l < अङ्कः_separators; l++)
	    {
		पूर्णाङ्क dislocation = (l == (अङ्कः_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_वस्तूनि[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		अत्थाय (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		अत्थाय (पूर्णाङ्क i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_वस्तूनि[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    सचे (hltab != रिक्त)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	अत्थाय (l = 0; l < itemcnt; l++)
	{
	    सचे (stl_वस्तूनि[l].stl_type == Highlight)
	    {
		sp->start = stl_वस्तूनि[l].stl_start;
		sp->userhl = stl_वस्तूनि[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = रिक्त;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    सचे (tabtab != रिक्त)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	अत्थाय (l = 0; l < itemcnt; l++)
	{
	    सचे (stl_वस्तूनि[l].stl_type == TabPage)
	    {
		sp->start = stl_वस्तूनि[l].stl_start;
		sp->userhl = stl_वस्तूनि[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = रिक्त;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    सचे (clicktab != रिक्त)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	अत्थाय (l = 0; l < itemcnt; l++)
	{
	    सचे (stl_वस्तूनि[l].stl_type == ClickFunc)
	    {
		cp->start = stl_वस्तूनि[l].stl_start;
		cp->funcname = stl_वस्तूनि[l].stl_clickfunc;
		cp->minwid = stl_वस्तूनि[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = रिक्त;
	cp->funcname = रिक्त;
    }
    अञ्ञथा
    {
	// Free click function names when caller doesn't need them.
	अत्थाय (l = 0; l < itemcnt; l++)
	    सचे (stl_वस्तूनि[l].stl_type == ClickFunc)
		vim_free(stl_वस्तूनि[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    कुञ्चिका_Typed = save_कुञ्चिका_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    सचे (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    पटिनिवत्त width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    पूर्णाङ्क
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    पूर्णाङ्क		buflen)
{
    long	above; // number of lines above window
    long	below; // number of lines below window

    सचे (buflen < 3) // need at least 3 chars for writing
	पटिनिवत्त 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    सचे (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_पङ्क्तिः_गणनम् - wp->w_botline + 1;
    सचे (below <= 0)
	पटिनिवत्त (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    सचे (above <= 0)
	पटिनिवत्त (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen,
	    "%s", _("Top"));

    पूर्णाङ्क perc = calc_percentage(above, above + below);
    वर्ण tmp[8];
    // localized percentage value
    vim_snprintf(tmp, परिमाण(tmp), _("%d%%"), perc);
    पटिनिवत्त (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    स्थिर पूर्णाङ्क
append_arg_सङ्ख्या(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    पूर्णाङ्क		add_file)	// Add "file" before the arg number
{
    सचे (ARGCOUNT <= 1)		// nothing to do
	पटिनिवत्त 0;

    वर्ण *सन्देशः;
    विकल्प ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	पक्ष 0: सन्देशः = _(" (%d of %d)"); भङ्ग;
	पक्ष 1: सन्देशः = _(" ((%d) of %d)"); भङ्ग;
	पक्ष 2: सन्देशः = _(" (file %d of %d)"); भङ्ग;
	पक्ष 3: सन्देशः = _(" (file (%d) of %d)"); भङ्ग;
    }

    पटिनिवत्त (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen, सन्देशः,
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
    पटिनिवत्त FullName_save(fname, TRUE);
#अञ्ञथा
    सचे (!vim_isAbsName(fname)
	    || strstr((वर्ण *)fname, "..") != रिक्त
	    || strstr((वर्ण *)fname, "//") != रिक्त
# ifdef BACKSLASH_IN_FILENAME
	    || strstr((वर्ण *)fname, "\\\\") != रिक्त
# endif
# सचे defined(MSWIN)
	    || vim_strchr(fname, '~') != रिक्त
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	पटिनिवत्त FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    सचे (fname != रिक्त)
	fname_case(fname, 0);	// set correct case for file name
# endif

    पटिनिवत्त fname;
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
    सचे (*ffname == रिक्त)	    // no file name given, nothing to do
	पटिनिवत्त;
    सचे (*sfname == रिक्त)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    सचे (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	सचे (rfname != रिक्त)
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
    पूर्णाङ्क		split_ret = OK;
    पूर्णाङ्क		p_ea_save;
    पूर्णाङ्क		open_wins = 0;
    पूर्णाङ्क		r;
    पूर्णाङ्क		गणनम्;		// Maximum number of windows to open.
    पूर्णाङ्क		all;		// When TRUE also load inactive buffers.
    पूर्णाङ्क		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    सचे (eap->addr_गणनम् == 0)	// make as many windows as possible
	गणनम् = 9999;
    अञ्ञथा
	गणनम् = eap->line2;	// make as many windows as specified
    सचे (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    अञ्ञथा
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
    सचे (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    अत्थाय (;;)
    {
	tpnext = curtab->tp_next;
	अत्थाय (wp = firstwin; wp != रिक्त; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    सचे ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		सचे (win_close(wp, FALSE) == FAIL)
		    भङ्ग;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    अञ्ञथा
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	सचे (had_tab == 0 || tpnext == रिक्त)
	    भङ्ग;
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
    अत्थाय (buf = firstbuf; buf != रिक्त && open_wins < गणनम्; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	सचे ((!all && buf->b_ml.ml_mfp == रिक्त) || !buf->b_p_bl)
	    अनुवर्त;

	सचे (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    सचे (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    अञ्ञथा
		wp = रिक्त;
	}
	अञ्ञथा
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		सचे (wp->w_buffer == buf)
		    भङ्ग;
	    // If the buffer already has a window, move it
	    सचे (wp != रिक्त)
		win_move_after(wp, curwin);
	}

	सचे (wp == रिक्त && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    सचे (split_ret == FAIL)
		अनुवर्त;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    सचे (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		भङ्ग;
	    }
	    सचे (swap_exists_action == SEA_QUIT)
	    {
#सचे defined(FEAT_EVAL)
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

#सचे defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    अञ्ञथा
		handle_swap_exists(रिक्त);
	}

	ui_breakcheck();
	सचे (got_int)
	{
	    (शून्य)vgetc();	// only break the file loading, not the rest
	    भङ्ग;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	सचे (aborting())
	    भङ्ग;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	सचे (had_tab > 0 && tabpage_index(रिक्त) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    अत्थाय (wp = lastwin; open_wins > गणनम्; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	सचे (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	अञ्ञथा सचे (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	अञ्ञथा
	{
	    wp = wp->w_prev;
	    सचे (wp == रिक्त)
		भङ्ग;
	}
    }
}


स्थिर पूर्णाङ्क  chk_modeline(linenr_T, पूर्णाङ्क);

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
do_modelines(पूर्णाङ्क flags)
{
    linenr_T	lnum;
    पूर्णाङ्क		nmlines;
    स्थिर पूर्णाङ्क	entered = 0;

    सचे (!curbuf->b_p_ml || (nmlines = (पूर्णाङ्क)p_mls) == 0)
	पटिनिवत्त;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    सचे (entered)
	पटिनिवत्त;

    ++entered;
    अत्थाय (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_पङ्क्तिः_गणनम् && lnum <= nmlines;
								       ++lnum)
	सचे (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    अत्थाय (lnum = curbuf->b_ml.ml_पङ्क्तिः_गणनम्; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_पङ्क्तिः_गणनम् - nmlines; --lnum)
	सचे (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#अन्तर्भाव "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    स्थिर पूर्णाङ्क
chk_modeline(
    linenr_T	lnum,
    पूर्णाङ्क		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*पङ्क्तिः_end;		// point to the end of the line
    char_u	*e;
    पूर्णाङ्क		prev;
    पूर्णाङ्क		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    पङ्क्तिः_end = s + ml_get_len(lnum);
    अत्थाय (; *s != NUL; ++s)
    {
	सचे (prev == -1 || vim_isspace(prev))
	{
	    सचे ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		भङ्ग;
	    // Accept both "vim" and "Vim".
	    सचे ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		पूर्णाङ्क vers;

		सचे (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		अञ्ञथा
		    e = s + 3;
		vers = getdigits(&e);
		सचे (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    भङ्ग;
	    }
	}
	prev = *s;
    }

    सचे (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	पूर्णाङ्क	end;

	करोहि				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	याव (s[-1] != ':');

	len = (size_t)(पङ्क्तिः_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	सचे (linecopy == रिक्त)
	    पटिनिवत्त FAIL;

	पङ्क्तिः_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_प्रक्षिप(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	याव (end == FALSE)
	{
	    s = skipwhite(s);
	    सचे (*s == NUL)
		भङ्ग;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    अत्थाय (e = s; *e != ':' && *e != NUL; ++e)
		सचे (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(पङ्क्तिः_end - (e + 1)) + 1);	// +1 for NUL
		    --पङ्क्तिः_end;
		}
	    सचे (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    सचे (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		सचे (*e != ':')		// no terminating ':'?
		    भङ्ग;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    सचे (*s != NUL)		// skip over an empty "::"
	    {
		पूर्णाङ्क secure_save = secure;
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
		सचे (retval == FAIL)		// stop if error found
		    भङ्ग;
	    }
	    s = (e == पङ्क्तिः_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_निष्कासय();
	vim_free(linecopy);
    }
    पटिनिवत्त retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    पूर्णाङ्क
bt_normal(buf_T *buf)
{
    पटिनिवत्त buf != रिक्त && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    पूर्णाङ्क
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    पटिनिवत्त buf != रिक्त && buf->b_p_bt[0] == 'q';
#अञ्ञथा
    पटिनिवत्त FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    पूर्णाङ्क
bt_terminal(buf_T *buf UNUSED)
{
#सचे defined(FEAT_TERMINAL)
    पटिनिवत्त buf != रिक्त && buf->b_p_bt[0] == 't';
#अञ्ञथा
    पटिनिवत्त FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    पूर्णाङ्क
bt_help(buf_T *buf)
{
    पटिनिवत्त buf != रिक्त && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    पूर्णाङ्क
bt_prompt(buf_T *buf)
{
    पटिनिवत्त buf != रिक्त && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#सचे defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    पूर्णाङ्क
bt_popup(buf_T *buf)
{
    पटिनिवत्त buf != रिक्त && buf->b_p_bt != रिक्त
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    पूर्णाङ्क
bt_nofilename(buf_T *buf)
{
    पटिनिवत्त buf != रिक्त && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    स्थिर पूर्णाङ्क
bt_nofileread(buf_T *buf)
{
    पटिनिवत्त buf != रिक्त && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#सचे defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    पूर्णाङ्क
bt_nofile(buf_T *buf)
{
    पटिनिवत्त buf != रिक्त && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    पूर्णाङ्क
bt_dontwrite(buf_T *buf)
{
    पटिनिवत्त buf != रिक्त && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    पूर्णाङ्क
bt_dontwrite_सन्देशः(buf_T *buf)
{
    सचे (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    पूर्णाङ्क
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    विकल्प (buf->b_p_bh[0])
    {
	पक्ष 'u':		    // "unload"
	पक्ष 'w':		    // "wipe"
	पक्ष 'd': पटिनिवत्त FALSE;	    // "delete"
	पक्ष 'h': पटिनिवत्त TRUE;	    // "hide"
    }
    पटिनिवत्त (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#सचे defined(FEAT_QUICKFIX)
    सचे (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	सचे (buf->b_fnum == qf_राशिः_get_bufnr())
	    पटिनिवत्त (char_u *)_(सन्देशः_qflist);
	अञ्ञथा
	    पटिनिवत्त (char_u *)_(सन्देशः_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    सचे (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	सचे (buf->b_term != रिक्त)
	    पटिनिवत्त term_get_status_text(buf->b_term);
#endif
	सचे (buf->b_fname != रिक्त)
	    पटिनिवत्त buf->b_fname;
	सचे (buf == cmdwin_buf)
	    पटिनिवत्त (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	सचे (bt_prompt(buf))
	    पटिनिवत्त (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	सचे (bt_popup(buf))
	    पटिनिवत्त (char_u *)_("[Popup]");
#endif
	पटिनिवत्त (char_u *)_("[Scratch]");
    }

    सचे (buf->b_fname == रिक्त)
	पटिनिवत्त buf_get_fname(buf);
    पटिनिवत्त रिक्त;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    सचे (buf->b_fname == रिक्त)
	पटिनिवत्त (char_u *)_("[No Name]");
    पटिनिवत्त buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    शून्य
set_buflisted(पूर्णाङ्क on)
{
    सचे (on == curbuf->b_p_bl)
	पटिनिवत्त;

    curbuf->b_p_bl = on;
    सचे (on)
	apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, curbuf);
    अञ्ञथा
	apply_autocmds(EVENT_BUFDELETE, रिक्त, रिक्त, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    पूर्णाङ्क
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    पूर्णाङ्क		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(रिक्त, रिक्त, (linenr_T)1, BLN_DUMMY);
    सचे (newbuf == रिक्त)
	पटिनिवत्त TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    सचे (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	पटिनिवत्त TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    सचे (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	पटिनिवत्त TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    सचे (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	सचे (buf->b_ml.ml_पङ्क्तिः_गणनम् == curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	{
	    differ = FALSE;
	    अत्थाय (lnum = 1; lnum <= curbuf->b_ml.ml_पङ्क्तिः_गणनम्; ++lnum)
		सचे (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    भङ्ग;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    सचे (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    पटिनिवत्त differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    शून्य
wipe_buffer(
    buf_T	*buf,
    पूर्णाङ्क		aucmd)	    // When TRUE trigger autocommands.
{
    सचे (buf->b_fnum == शिखरम्_file_अङ्कः - 1)
	--शिखरम्_file_अङ्कः;

    सचे (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(रिक्त, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    सचे (!aucmd)
	unblock_autocmds();
}
