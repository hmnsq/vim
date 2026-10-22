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
#अन्यथा
स्थिर पूर्णाङ्क	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
स्थिर पूर्णाङ्क	मूल्य_changed(char_u *str, char_u **last);
#यदि defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
स्थिर पूर्णाङ्क build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, माप_प्रकार outlen, char_u **fmt_arg,
		char_u *opt_name, पूर्णाङ्क opt_scope, पूर्णाङ्क fillchar, पूर्णाङ्क maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, पूर्णाङ्क *lbreaks, पूर्णाङ्क *carry_hl);
#endif
स्थिर पूर्णाङ्क	append_arg_संख्या(win_T *wp, char_u *buf, माप_प्रकार buflen, पूर्णाङ्क add_file);
स्थिर शून्य	free_buffer(buf_T *);
स्थिर शून्य	free_buffer_stuff(buf_T *buf, पूर्णाङ्क free_options);
स्थिर पूर्णाङ्क	bt_nofileread(buf_T *buf);
स्थिर पूर्णाङ्क	do_buffer_ext(पूर्णाङ्क action, पूर्णाङ्क start, पूर्णाङ्क dir, पूर्णाङ्क गिनती, पूर्णाङ्क flags);

#ifdef UNIX
# define dev_T dev_t
#अन्यथा
# define dev_T अचिन्हित
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    प्रति ((buf) = lastbuf; (buf) != रिक्त; (buf) = (buf)->b_prev)

#यदि defined(FEAT_QUICKFIX)
स्थिर वर्ण *संदेश_loclist = N_("[Location List]");
स्थिर वर्ण *संदेश_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
स्थिर पूर्णाङ्क	buf_free_गिनती = 0;

स्थिर पूर्णाङ्क	शीर्ष_file_अंक = 1;	// highest file number
स्थिर garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    स्थिर शून्य
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"यदि exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    स्थिर पूर्णाङ्क
calc_percentage(दीर्घ part, दीर्घ whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    प्रत्यागम (part > 1000000L)
	? (पूर्णाङ्क)(part / (whole / 100L))
	: (पूर्णाङ्क)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    पूर्णाङ्क
get_highest_fnum(शून्य)
{
    प्रत्यागम शीर्ष_file_अंक - 1;
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
    linenr_T	पंक्ति_गिनती;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    पंक्ति_गिनती = curbuf->b_ml.ml_पंक्ति_गिनती;
    retval = readfile(
	    read_stdin ? रिक्त : curbuf->b_ffname,
	    read_stdin ? रिक्त : curbuf->b_fname,
	    पंक्ति_गिनती, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    यदि (retval == OK)
    {
	// Delete the binary lines.
	यावत् (--पंक्ति_गिनती >= 0)
	    ml_delete((linenr_T)1);
    }
    अन्यथा
    {
	// Delete the converted lines.
	यावत् (curbuf->b_ml.ml_पंक्ति_गिनती > पंक्ति_गिनती)
	    ml_delete(पंक्ति_गिनती);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    यदि (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	यदि (!readonlymode && !BUFEMPTY())
	    changed();
	अन्यथा यदि (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	यदि (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, रिक्त, रिक्त, FALSE,
							      curbuf, &retval);
#अन्यथा
	    apply_autocmds(EVENT_STDINREADPOST, रिक्त, रिक्त, FALSE, curbuf);
#endif
	}
    }
    प्रत्यागम retval;
}

#यदि defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    शून्य
buffer_ensure_loaded(buf_T *buf)
{
    यदि (buf->b_ml.ml_mfp != रिक्त)
	प्रत्यागम;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    यदि (curbuf == buf)
    {
	यदि (swap_exists_action != SEA_READONLY)
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
    दीर्घ	old_tw = curbuf->b_p_tw;
#endif
    पूर्णाङ्क		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    यदि (readonlymode && curbuf->b_ffname != रिक्त
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    यदि (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    यदि (curbuf->b_ml.ml_mfp != रिक्त)
		भङ्ग;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	यदि (curbuf == रिक्त)
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
	यदि (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(रिक्त, curwin);
#endif
	प्रत्यागम FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    यदि (curbuf->b_ml.ml_mfp != रिक्त)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    यदि (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    यदि (curbuf->b_ffname != रिक्त
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	पूर्णाङ्क old_संदेश_silent = संदेश_silent;
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
	यदि (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	यदि (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	यदि (shortmess(SHM_FILEINFO))
	    संदेश_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	यदि (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    यदि (retval == OK)
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
	यदि (bt_help(curbuf))
	    fix_help_buffer();
    }
    अन्यथा यदि (read_stdin)
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
	यदि (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    यदि (curbuf->b_ml.ml_mfp != रिक्त
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    यदि (curbuf->b_flags & BF_NEVERLOADED)
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
    यदि ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != रिक्त)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != रिक्त)
#endif
       )
	changed();
    अन्यथा यदि (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    यदि (aborting())
#अन्यथा
    यदि (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    यदि (!(curwin->w_valid & VALID_TOPLINE))
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

    यदि (retval != OK)
	प्रत्यागम retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    यदि (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != रिक्त)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	यदि (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    यदि ((flags & READ_NOWINENTER) == 0)
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

    प्रत्यागम retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    शून्य
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == रिक्त ? 0 : buf->b_fnum;
    bufref->br_buf_free_गिनती = buf_free_गिनती;
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
    प्रत्यागम bufref->br_buf_free_गिनती == buf_free_गिनती
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
	यदि (bp == buf)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
स्थिर hashtab_T buf_hashtab;

    स्थिर शून्य
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((वर्ण *)buf->b_कुंजी, परिमाण(buf->b_कुंजी), "%x", buf->b_fnum);
    यदि (hash_add(&buf_hashtab, buf->b_कुंजी, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    स्थिर शून्य
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_कुंजी);

    यदि (!HASHITEM_EMPTY(hi))
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

    यदि (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    यदि (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		भङ्ग;
	    }
    }
    // Don't unload the buffer while it's still being saved
    यदि (can_unload && buf->b_saving)
	can_unload = FALSE;

    यदि (!can_unload)
    {
	char_u *fname = buf->b_fname != रिक्त ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != रिक्त ? fname : (char_u *)"[No Name]");
    }
    प्रत्यागम can_unload;
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
    यदि (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    अन्यथा यदि (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    अन्यथा यदि (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    यदि (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	यदि (term_job_running(buf->b_term))
	{
	    यदि (wipe_buf || unload_buf)
	    {
		यदि (!can_unload_buffer(buf))
		    प्रत्यागम FALSE;

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
	अन्यथा यदि (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	अन्यथा
	{
	    यदि (del_buf || unload_buf)
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
    यदि ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	प्रत्यागम FALSE;

    यदि (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	यदि (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    यदि ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	यदि (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    प्रत्यागम FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	यदि (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    गच्छ aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	यदि (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    यदि (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		गच्छ aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    यदि (abort_if_last && one_window())
		// Autocommands made this the only window.
		गच्छ aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	यदि (!ignore_abort && aborting())
	    प्रत्यागम FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    यदि (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    यदि (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    यदि (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    यदि (!hiding_buf || !unload_buf)
	प्रत्यागम FALSE;

    // Always remove the buffer when there is no file name.
    यदि (buf->b_ffname == रिक्त)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    यदि (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	प्रत्यागम FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    यदि ((win_valid || closed_popup) && win->w_buffer == buf)
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
    यदि (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != रिक्त || buf->b_next != रिक्त
#यदि defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	यदि (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    यदि (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, परिमाण(पूर्णाङ्क), 50);
	    यदि (ga_grow(&buf_reuse, 1) == OK)
		((पूर्णाङ्क *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	यदि (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	अन्यथा
	    buf->b_sfname = रिक्त;
	VIM_CLEAR(buf->b_ffname);
	यदि (buf->b_prev == रिक्त)
	    firstbuf = buf->b_next;
	अन्यथा
	    buf->b_prev->b_next = buf->b_next;
	यदि (buf->b_next == रिक्त)
	    lastbuf = buf->b_prev;
	अन्यथा
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    अन्यथा
    {
	यदि (del_buf)
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
	यदि (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    प्रतिज्ञा(!win_valid || !is_curwin || win == curwin);
    प्रत्यागम TRUE;
}

/*
 * Make buffer not contain a file.
 */
    शून्य
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_पंक्ति_गिनती = 1;
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
    netbeans_deleted_all_पंक्तियाँ(buf);
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
    यदि (buf->b_ml.ml_mfp != रिक्त)
    {
	यदि (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    प्रत्यागम FALSE;
    }
    यदि ((flags & BFA_DEL) && buf->b_p_bl)
    {
	यदि (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    प्रत्यागम FALSE;
    }
    यदि (flags & BFA_WIPE)
    {
	यदि (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    प्रत्यागम FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    यदि (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    यदि ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	प्रत्यागम FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    यदि (buf == curbuf && !is_curbuf)
	प्रत्यागम FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    यदि (buf == curbuf && VIsual_active
#यदि defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    यदि (curwin != रिक्त && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    यदि (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_पंक्ति_गिनती = 0;    // no lines in buffer
    यदि ((flags & BFA_KEEP_UNDO) == 0)
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
    प्रत्यागम TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    स्थिर शून्य
free_buffer(buf_T *buf)
{
    ++buf_free_गिनती;
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

    यदि (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    अन्यथा
    {
	vim_free(buf);
	यदि (curbuf == buf)
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
    STRCPY(buf->b_ct_di.di_कुंजी, "changedtick");
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

    यावत् (buf->b_wininfo != रिक्त)
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
    यदि (free_options)
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
    यदि (wip->wi_optset)
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
    पूर्णाङ्क		गिनती)
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

    यदि (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (शून्य)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, गिनती,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    यदि (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#यदि defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#यदि defined(FEAT_EVAL)
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
#यदि defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    दीर्घ	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    यदि (swap_exists_action == SEA_QUIT)
    {
#यदि defined(FEAT_EVAL)
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
	यदि (old_curbuf == रिक्त || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(रिक्त, रिक्त, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	अन्यथा
	    buf = old_curbuf->br_buf;
	यदि (buf != रिक्त)
	{
	    पूर्णाङ्क old_संदेश_silent = संदेश_silent;

	    यदि (shortmess(SHM_FILEINFO))
		संदेश_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    संदेश_silent = old_संदेश_silent;

#ifdef FEAT_SYN_HL
	    यदि (old_tw != curbuf->b_p_tw)
		check_colorcolumn(रिक्त, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#यदि defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    अन्यथा यदि (swap_exists_action == SEA_RECOVER)
    {
#यदि defined(FEAT_EVAL)
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

#यदि defined(FEAT_EVAL)
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

    यदि (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	प्रत्यागम FAIL;
    }

    set_bufref(&bufref, buf);
    यदि (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, रिक्त, रिक्त, रिक्त, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    यदि (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(रिक्त, buf, action, FALSE, FALSE, FALSE);
    यदि (!close_others)
	need_fileinfo = FALSE;
    अन्यथा यदि (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    प्रत्यागम retval;
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
    पूर्णाङ्क		गिनती,		// buffer number or number of buffers
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
    यदि (start == DOBUF_MOD)	    // find next modified buffer
    {
	यावत् (गिनती-- > 0)
	{
	    कुरु
	    {
		buf = buf->b_next;
		यदि (buf == रिक्त)
		    buf = firstbuf;
	    }
	    यावत् (buf != curbuf && !bufIsChanged(buf));
	}
	यदि (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    प्रत्यागम FAIL;
	}
    }
    अन्यथा यदि (start == DOBUF_FIRST && गिनती) // find specified buffer number
    {
	यावत् (buf != रिक्त && buf->b_fnum != गिनती)
	    buf = buf->b_next;
    }
    अन्यथा
    {
	पूर्णाङ्क help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = रिक्त;
	यावत् (गिनती > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    यदि (bp == रिक्त)
		bp = buf;
	    यदि (dir == FORWARD)
	    {
		buf = buf->b_next;
		यदि (buf == रिक्त)
		    buf = firstbuf;
	    }
	    अन्यथा
	    {
		buf = buf->b_prev;
		यदि (buf == रिक्त)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    यदि (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--गिनती;
		bp = रिक्त;	// use this buffer as new starting point
	    }
	    यदि (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		प्रत्यागम FAIL;
	    }
	}
    }

    यदि (buf == रिक्त)	    // could not find it
    {
	यदि (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    यदि (!unload)
		semsg(_(e_buffer_nr_does_not_exist), गिनती);
	}
	अन्यथा यदि (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	अन्यथा
	    emsg(_(e_cannot_go_before_first_buffer));
	प्रत्यागम FAIL;
    }
#ifdef FEAT_PROP_POPUP
    यदि ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	प्रत्यागम OK;
#endif
    यदि (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	प्रत्यागम FAIL;

    यदि ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), गिनती);
	प्रत्यागम FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    यदि (unload)
    {
	पूर्णाङ्क	forward;
	bufref_T bufref;

	यदि (!can_unload_buffer(buf))
	    प्रत्यागम FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	यदि (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == रिक्त && !buf->b_p_bl)
	    प्रत्यागम FAIL;

	यदि ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    यदि ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		यदि (term_job_running(buf->b_term))
		{
		    यदि (term_confirm_stop(buf) == FAIL)
			प्रत्यागम FAIL;
		}
		अन्यथा
# endif
		{
		    dialog_changed(buf, FALSE);
		    यदि (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			प्रत्यागम FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    यदि (bufIsChanged(buf))
			प्रत्यागम FAIL;
		}
	    }
	    अन्यथा
#endif
	    {
		no_write_message_buf(buf);
		प्रत्यागम FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	यदि (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    यदि (bp->b_p_bl && bp != buf)
		भङ्ग;
	यदि (bp == रिक्त && buf == curbuf)
	    प्रत्यागम empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	यावत् (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != रिक्त))
	{
	    यदि (win_close(curwin, FALSE) == FAIL)
		भङ्ग;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	यदि (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    यदि (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(रिक्त, buf, action, FALSE, FALSE, FALSE);
	    प्रत्यागम OK;
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
	यदि (au_new_curbuf.br_buf != रिक्त && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	अन्यथा यदि (curwin->w_jumplistlen > 0)
	{
	    पूर्णाङ्क     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    यदि (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    यावत् (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		यदि (buf != रिक्त)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    यदि (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = रिक्त;
		    अन्यथा यदि (buf->b_ml.ml_mfp == रिक्त)
		    {
			// skip unloaded buf, but may keep it for later
			यदि (bp == रिक्त)
			    bp = buf;
			buf = रिक्त;
		    }
		}
		यदि (buf != रिक्त)   // found a valid buffer: stop searching
		    भङ्ग;
		// advance to older entry in jump list
		यदि (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    भङ्ग;
		यदि (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		यदि (jumpidx == forward)		// List exhausted for sure
		    भङ्ग;
	    }
	}

	यदि (buf == रिक्त)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    प्रति (;;)
	    {
		यदि (buf == रिक्त)
		{
		    यदि (!forward)	// tried both directions
			भङ्ग;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    अनुवर्त;
		}
		// in non-help buffer, try to skip help buffers, and vv
		यदि (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    यदि (buf->b_ml.ml_mfp != रिक्त)   // found loaded buffer
			भङ्ग;
		    यदि (bp == रिक्त)	// remember unloaded buf for later
			bp = buf;
		}
		यदि (forward)
		    buf = buf->b_next;
		अन्यथा
		    buf = buf->b_prev;
	    }
	}
	यदि (buf == रिक्त)	// No loaded buffer, use unloaded one
	    buf = bp;
	यदि (buf == रिक्त)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		यदि (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    भङ्ग;
	}
	यदि (buf == रिक्त)	// Still no buffer, just take one
	{
	    यदि (curbuf->b_next != रिक्त)
		buf = curbuf->b_next;
	    अन्यथा
		buf = curbuf->b_prev;
	    यदि (bt_quickfix(buf)
		    || (buf != रिक्त && buf != curbuf && buf->b_locked_split))
		buf = रिक्त;
	}
    }

    यदि (buf == रिक्त)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	प्रत्यागम empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    यदि (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != रिक्त)
	प्रत्यागम OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    यदि (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	प्रत्यागम FAIL;
    }
    यदि (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	प्रत्यागम FAIL;

    // go to current buffer - nothing to do
    यदि (buf == curbuf)
	प्रत्यागम OK;

    // Check if the current buffer may be abandoned.
    यदि (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#यदि defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	यदि ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    यदि (term_job_running(curbuf->b_term))
	    {
		यदि (term_confirm_stop(curbuf) == FAIL)
		    प्रत्यागम FAIL;
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
		यदि (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    प्रत्यागम FAIL;

		यदि (bufIsChanged(curbuf))
		{
		    no_write_message();
		    प्रत्यागम FAIL;
		}
	    }
	}
	अन्यथा
#endif
	{
	    no_write_message();
	    प्रत्यागम FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    यदि (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#यदि defined(FEAT_EVAL)
    यदि (aborting())	    // autocmds may abort script processing
	प्रत्यागम FAIL;
#endif

    प्रत्यागम OK;
}

    पूर्णाङ्क
do_buffer(
    पूर्णाङ्क		action,
    पूर्णाङ्क		start,
    पूर्णाङ्क		dir,		// FORWARD or BACKWARD
    पूर्णाङ्क		गिनती,		// buffer number or number of buffers
    पूर्णाङ्क		forceit)	// TRUE when using !
{
    प्रत्यागम do_buffer_ext(action, start, dir, गिनती,
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
    पूर्णाङ्क		addr_गिनती,
    पूर्णाङ्क		start_bnr,	// first buffer number in a range
    पूर्णाङ्क		end_bnr,	// buffer nr or last buffer nr in a range
    पूर्णाङ्क		forceit)
{
    पूर्णाङ्क		do_current = 0;	// delete current buffer?
    पूर्णाङ्क		deleted = 0;	// number of buffers deleted
    वर्ण	*errormsg = रिक्त; // return value
    पूर्णाङ्क		bnr;		// buffer number
    char_u	*p;

    यदि (addr_गिनती == 0)
    {
	(शून्य)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    अन्यथा
    {
	यदि (addr_गिनती == 2)
	{
	    यदि (*arg)		// both range and argument is not allowed
		प्रत्यागम ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	अन्यथा	// addr_count == 1
	    bnr = end_bnr;

	प्रति ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    यदि (bnr == curbuf->b_fnum)
		do_current = bnr;
	    अन्यथा यदि (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    यदि (addr_गिनती == 2)
	    {
		यदि (++bnr > end_bnr)
		    भङ्ग;
	    }
	    अन्यथा    // addr_count == 1
	    {
		arg = skipwhite(arg);
		यदि (*arg == NUL)
		    भङ्ग;
		यदि (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    यदि (bnr < 0)	    // failed
			भङ्ग;
		    arg = p;
		}
		अन्यथा
		    bnr = getdigits(&arg);
	    }
	}
	यदि (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	यदि (deleted == 0)
	{
	    यदि (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    अन्यथा यदि (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    अन्यथा
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (वर्ण *)IObuff;
	}
	अन्यथा यदि (deleted >= p_report)
	{
	    यदि (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    अन्यथा यदि (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    अन्यथा
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    प्रत्यागम errormsg;
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
    दीर्घ	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    पूर्णाङ्क		valid;

    setpcmark();
    यदि ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    यदि (!apply_autocmds(EVENT_BUFLEAVE, रिक्त, रिक्त, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	यदि (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	यदि (unload)
	    close_windows(prevbuf, FALSE);
#यदि defined(FEAT_EVAL)
	यदि (bufref_valid(&prevbufref) && !aborting())
#अन्यथा
	यदि (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    यदि (prevbuf == curbuf
		    && ((State & MODE_जोड़ो) == 0 || curbuf->b_nwindows <= 1))
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
    यदि ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == रिक्त)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	यदि (!valid)
	    enter_buffer(lastbuf);
	अन्यथा
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	यदि (old_tw != curbuf->b_p_tw)
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
    यदि (VIsual_active
#यदि defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    यदि (curwin->w_buffer != रिक्त)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    यदि (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    अन्यथा
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    यदि (curwin->w_p_diff)
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
    यदि (curbuf->b_ml.ml_mfp == रिक्त)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	यदि (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, रिक्त, 0);
    }
    अन्यथा
    {
	यदि (!संदेश_silent && !shortmess(SHM_FILEINFO))
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
    यदि (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    यदि (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    यदि (curbuf->b_kmap_state & KEYMAP_INIT)
	(शून्य)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    यदि (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(शून्य)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#यदि defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    शून्य
do_autochdir(शून्य)
{
    यदि ((starting == 0 || test_autochdir)
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
    यदि (term_job_running(buf->b_term))
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
    यदि (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    अन्यथा
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    शून्य
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    यदि (term_job_running(buf->b_term))
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
    पूर्णाङ्क
curbuf_reusable(शून्य)
{
    प्रत्यागम (curbuf != रिक्त
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

    यदि (शीर्ष_file_अंक == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    यदि (sfname == रिक्त || mch_stat((वर्ण *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    यदि (ffname != रिक्त && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#अन्यथा
		buflist_findname(ffname)
#endif
		) != रिक्त)
    {
	vim_free(ffname);
	यदि (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? रिक्त : curwin,
						      lnum, (colnr_T)0, FALSE);

	यदि ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	यदि ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    यदि (!(flags & BLN_DUMMY))
	    {
		यदि (apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, buf)
			&& !bufref_valid(&bufref))
		    प्रत्यागम रिक्त;
	    }
	}
	प्रत्यागम buf;
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
    यदि ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	यदि (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    प्रत्यागम रिक्त;
	}
#endif
	यदि (!bufref_valid(&bufref))
	    buf = रिक्त;		// buf was deleted; allocate a new buffer
    }
    यदि (buf != curbuf || curbuf == रिक्त)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	यदि (buf == रिक्त)
	{
	    vim_free(ffname);
	    प्रत्यागम रिक्त;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	यदि (buf->b_vars == रिक्त)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    प्रत्यागम रिक्त;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    यदि (ffname != रिक्त)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    यदि ((ffname != रिक्त && (buf->b_ffname == रिक्त || buf->b_sfname == रिक्त))
	    || buf->b_wininfo == रिक्त)
    {
	यदि (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	अन्यथा
	    buf->b_sfname = रिक्त;
	VIM_CLEAR(buf->b_ffname);
	यदि (buf != curbuf)
	    free_buffer(buf);
	प्रत्यागम रिक्त;
    }

    यदि (buf == curbuf)
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
	यदि (firstbuf == रिक्त)		// buffer list is empty
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

	यदि ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((पूर्णाङ्क *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    यावत् (buf->b_prev != रिक्त && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		यदि (prev->b_next != रिक्त)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		यदि (buf->b_prev != रिक्त)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		यदि (lastbuf == buf)
		    lastbuf = prev;
		यदि (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	अन्यथा
	    buf->b_fnum = शीर्ष_file_अंक++;
	यदि (शीर्ष_file_अंक < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    यदि (emsg_silent == 0 && !in_assert_fails)
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
    यदि (st.st_dev == (dev_T)-1)
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
    यदि (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    यदि (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	यदि (apply_autocmds(EVENT_BUFNEW, रिक्त, रिक्त, FALSE, buf)
		&& !bufref_valid(&bufref))
	    प्रत्यागम रिक्त;
	यदि (flags & BLN_LISTED)
	{
	    यदि (apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, buf)
		    && !bufref_valid(&bufref))
		प्रत्यागम रिक्त;
	}
#ifdef FEAT_EVAL
	यदि (aborting())		// autocmds may abort script processing
	    प्रत्यागम रिक्त;
#endif
    }

    प्रत्यागम buf;
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
    यदि (free_p_ff)
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
#यदि defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#यदि defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#यदि defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#यदि defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    यदि (buf->b_p_कुंजी != रिक्त && *buf->b_p_कुंजी != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_कुंजी, STRLEN(buf->b_p_कुंजी));
# endif
    clear_string_option(&buf->b_p_कुंजी);
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
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_गिनती);
    buf->b_p_cpt_गिनती = 0;
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
    यदि (buf == रिक्त)
    {
	यदि ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	अन्यथा
	    semsg(_(e_buffer_nr_not_found), n);
	प्रत्यागम FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    यदि (buf == curbuf)
	प्रत्यागम OK;

    यदि (text_or_buf_locked())
	प्रत्यागम FAIL;

    // altfpos may be changed by getfile(), get it now
    यदि (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    अन्यथा
	col = 0;

    यदि (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	यदि (wp == रिक्त && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    यदि (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    अन्यथा यदि (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		प्रत्यागम FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    पूर्णाङ्क retval = FAIL;
    यदि (GETFILE_SUCCESS(getfile(buf->b_fnum, रिक्त, रिक्त,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	यदि (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    यदि (RedrawingDisabled > 0)
	--RedrawingDisabled;
    प्रत्यागम retval;
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

    यदि (p_sol)
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
    यदि (ffname != रिक्त)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    प्रत्यागम buf;
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

    यदि (mch_stat((वर्ण *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    प्रत्यागम buflist_findname_stat(ffname, &st);
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
	यदि ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    प्रत्यागम buf;
    प्रत्यागम रिक्त;
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
    यदि ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	यदि (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	अन्यथा
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	यदि (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
	यदि (pat == रिक्त)
	    प्रत्यागम -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	प्रति (;;)
	{
	    प्रति (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		यदि (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		यदि (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    यदि (regmatch.regprog == रिक्त)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			प्रत्यागम -1;
		    }
		    यदि (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != रिक्त)
		    {
			यदि (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				यदि (wp->w_buffer == buf)
				    भङ्ग;
			    यदि (wp == रिक्त)
				अनुवर्त;
			}
			यदि (match >= 0)		// already found a match
			{
			    match = -2;
			    भङ्ग;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		यदि (match >= 0)			// found one match
		    भङ्ग;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    यदि (!unlisted || !find_listed || match != -1)
		भङ्ग;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    यदि (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    अन्यथा यदि (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    प्रत्यागम match;
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
    पूर्णाङ्क		*अंक_file,
    char_u	***file,
    पूर्णाङ्क		options)
{
    पूर्णाङ्क		गिनती;
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

    *अंक_file = 0;		    // return values in case of FAIL
    *file = रिक्त;

#ifdef FEAT_DIFF
    यदि ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	प्रत्यागम FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    यदि (!fuzzy)
    {
	यदि (*pat == '^' && pat[1] != NUL)
	{
	    पूर्णाङ्क  len = (पूर्णाङ्क)STRLEN(pat);
	    patc = alloc(len);
	    यदि (patc == रिक्त)
		प्रत्यागम FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	अन्यथा यदि (*pat == '^')
	    patc = (char_u *)"";
	अन्यथा
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    प्रति (round = 1; round <= 2; ++round)
    {
	गिनती = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    यदि (!buf->b_p_bl)	// skip unlisted buffers
		अनुवर्त;
#ifdef FEAT_DIFF
	    यदि (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		यदि (buf == curbuf || !diff_mode_buf(buf))
		    अनुवर्त;
#endif

	    यदि (!fuzzy)
	    {
		यदि (regmatch.regprog == रिक्त)
		{
		    // invalid pattern, possibly after recompiling
		    यदि (to_free)
			vim_free(patc);
		    प्रत्यागम FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    अन्यथा
	    {
		p = रिक्त;
		// first try matching with the short file name
		यदि ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		यदि (p == रिक्त)
		{
		    // next try matching with the full path file name
		    यदि ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    यदि (p == रिक्त)
		अनुवर्त;

	    यदि (round == 1)
	    {
		++गिनती;
		अनुवर्त;
	    }

	    यदि (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    अन्यथा
		p = vim_strsave(p);
	    यदि (p == रिक्त)
	    {
		यदि (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, गिनती);
		प्रत्यागम FAIL;
	    }

	    यदि (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		यदि (matches != रिक्त)
		{
		    matches[गिनती].buf = buf;
		    matches[गिनती].match = p;
		    गिनती++;
		}
		अन्यथा
#endif
		    (*file)[गिनती++] = p;
	    }
	    अन्यथा
	    {
		fuzmatch[गिनती].idx = गिनती;
		fuzmatch[गिनती].str = p;
		fuzmatch[गिनती].score = score;
		गिनती++;
	    }
	}
	यदि (गिनती == 0)	// no match found, break here
	    भङ्ग;
	यदि (round == 1)
	{
	    यदि (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, गिनती);
		यदि (*file == रिक्त)
		{
		    vim_regfree(regmatch.regprog);
		    यदि (to_free)
			vim_free(patc);
		    प्रत्यागम FAIL;
		}
#ifdef FEAT_VIMINFO
		यदि (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, गिनती);
#endif
	    }
	    अन्यथा
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, गिनती);
		यदि (fuzmatch == रिक्त)
		{
		    *अंक_file = 0;
		    *file = रिक्त;
		    प्रत्यागम FAIL;
		}
	    }
	}
    }

    यदि (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	यदि (to_free)
	    vim_free(patc);
    }

    यदि (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	यदि (matches != रिक्त)
	{
	    पूर्णाङ्क i;
	    यदि (गिनती > 1)
		क्विक(matches, गिनती, परिमाण(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    यदि (matches[0].buf == curbuf)
	    {
		प्रति (i = 1; i < गिनती; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[गिनती-1] = matches[0].match;
	    }
	    अन्यथा
	    {
		प्रति (i = 0; i < गिनती; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    अन्यथा
    {
	यदि (fuzzymatches_to_strmatches(fuzmatch, file, गिनती, FALSE) == FAIL)
	    प्रत्यागम FAIL;
    }

    *अंक_file = गिनती;
    प्रत्यागम (गिनती == 0 ? FAIL : OK);
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
    यदि (match == रिक्त && rmp->regprog != रिक्त)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    प्रत्यागम match;
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
    यदि (name == रिक्त || rmp->regprog == रिक्त)
	प्रत्यागम रिक्त;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    यदि (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    अन्यथा यदि (rmp->regprog != रिक्त)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(रिक्त, name);
	यदि (p != रिक्त && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    प्रत्यागम match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(पूर्णाङ्क nr)
{
    char_u	कुंजी[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    यदि (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((वर्ण *)कुंजी, परिमाण(कुंजी), "%x", nr);
    hi = hash_find(&buf_hashtab, कुंजी);

    यदि (!HASHITEM_EMPTY(hi))
	प्रत्यागम (buf_T *)(hi->hi_कुंजी
			     - ((अचिन्हित)(curbuf->b_कुंजी - (char_u *)curbuf)));
    प्रत्यागम रिक्त;
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
    यदि (buf == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम home_replace_save(helptail ? buf : रिक्त,
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
	यदि (wip->wi_win == win)
	    भङ्ग;
    यदि (wip == रिक्त)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	यदि (wip == रिक्त)
	    प्रत्यागम;
	wip->wi_win = win;
	यदि (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    अन्यथा
    {
	// remove the entry from the list
	यदि (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	अन्यथा
	    buf->b_wininfo = wip->wi_next;
	यदि (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	यदि (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    यदि (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    यदि (win != रिक्त)
	wip->wi_changelistidx = win->w_changelistidx;
    यदि (copy_options && win != रिक्त)
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
    यदि (wip->wi_next)
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

    यदि (!wip->wi_opt.wo_diff)
	प्रत्यागम FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	यदि (wip->wi_win == wp)
	    प्रत्यागम FALSE;
    प्रत्यागम TRUE;
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
	यदि (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    भङ्ग;

    यदि (wip != रिक्त)
	प्रत्यागम wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    यदि (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    यदि (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != रिक्त
			    && wip->wi_win->w_buffer == buf)))
		भङ्ग;
    }
    अन्यथा
#endif
	wip = buf->b_wininfo;
    प्रत्यागम wip;
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
    यदि (wip != रिक्त && wip->wi_win != रिक्त
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
    अन्यथा यदि (wip != रिक्त && wip->wi_optset)
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
    यदि (wip != रिक्त)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    यदि (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    यदि (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    यदि (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	यदि (p_ea)
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
    यदि (wip != रिक्त)
	प्रत्यागम &(wip->wi_fpos);
    अन्यथा
	प्रत्यागम &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    प्रत्यागम buflist_findfpos(buf)->lnum;
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

    यदि (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, परिमाण(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    यदि (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	क्विक(buflist.ga_data, (माप_प्रकार)buflist.ga_len,
		परिमाण(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    प्रति (; buf != रिक्त && !got_int; buf = buflist_data != रिक्त
	    ? (++p < buflist_data + buflist.ga_len ? *p : रिक्त)
	    : buf->b_next)
#अन्यथा
    प्रति (buf = firstbuf; buf != रिक्त && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	यदि ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
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
	यदि (name != रिक्त)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	अन्यथा
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	यदि (message_filtered(NameBuff))
	    अनुवर्त;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	यदि (job_running)
	{
	    यदि (job_none_open)
		ro_char = '?';
	    अन्यथा
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	अन्यथा यदि (buf->b_term != रिक्त)
	    ro_char = 'F';
	अन्यथा
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	संदेश_putchar('\n');
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
	कुरु
	    IObuff[len++] = ' ';
	यावत् (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	यदि (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (माप_प्रकार)(IOSIZE - len), buf->b_last_used);
	अन्यथा
#endif
	    vim_snprintf((वर्ण *)IObuff + len, (माप_प्रकार)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (दीर्घ)buflist_findlnum(buf));
	संदेश_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    यदि (buflist_data)
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
    यदि (buf == रिक्त || buf->b_fname == रिक्त)
	प्रत्यागम FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    प्रत्यागम OK;
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

    यदि (ffname == रिक्त || *ffname == NUL)
    {
	// Removing the name.
	यदि (buf->b_sfname != buf->b_ffname)
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
	यदि (ffname == रिक्त)		    // out of memory
	    प्रत्यागम FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	यदि (mch_stat((वर्ण *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	यदि (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#अन्यथा
	    obuf = buflist_findname(ffname);
#endif
	यदि (obuf != रिक्त && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    पूर्णाङ्क		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		यदि (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    यदि (obuf->b_ml.ml_mfp != रिक्त || in_use)
	    {
		यदि (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		प्रत्यागम FAIL;
	    }
	    // delete from the list
	    close_buffer(रिक्त, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	यदि (ffname == रिक्त || sfname == रिक्त)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    प्रत्यागम FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	यदि (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    यदि (st.st_dev == (dev_T)-1)
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
    प्रत्यागम OK;
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
    यदि (buf == रिक्त)
	प्रत्यागम;

    यदि (buf->b_sfname != buf->b_ffname)
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
    यदि (buf->b_ml.ml_mfp != रिक्त)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    यदि (buf->b_term != रिक्त)
	term_clear_status_text(buf->b_term);
#endif

    यदि (curwin->w_buffer == buf)
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
    यदि (buf != रिक्त && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    प्रत्यागम buf;
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

    यदि (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	यदि (errmsg)
	    emsg(_(e_no_alternate_file));
	प्रत्यागम रिक्त;
    }
    प्रत्यागम fname;
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
    यदि (buf != रिक्त)
	प्रत्यागम buf->b_fnum;
    प्रत्यागम 0;
}

#यदि defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    शून्य
buflist_slash_adjust(शून्य)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	यदि (bp->b_ffname != रिक्त)
	    slash_adjust(bp->b_ffname);
	यदि (bp->b_sfname != रिक्त)
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
    प्रत्यागम otherfile_buf(curbuf, ffname
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
    यदि (ffname == रिक्त || *ffname == NUL || buf->b_ffname == रिक्त)
	प्रत्यागम TRUE;
    यदि (fnamecmp(ffname, buf->b_ffname) == 0)
	प्रत्यागम FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	यदि (stp == रिक्त)
	{
	    यदि (!buf->b_dev_valid || mch_stat((वर्ण *)ffname, &st) < 0)
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
	यदि (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    यदि (buf_same_ino(buf, stp))
		प्रत्यागम FALSE;
	}
    }
#endif
    प्रत्यागम TRUE;
}

#यदि defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    शून्य
buf_setino(buf_T *buf)
{
    stat_T	st;

    यदि (buf->b_fname != रिक्त && mch_stat((वर्ण *)buf->b_fname, &st) >= 0)
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
    स्थिर पूर्णाङ्क
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    प्रत्यागम (buf->b_dev_valid
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
    माप_प्रकार	bufferlen = 0;

    buffer = alloc(IOSIZE);
    यदि (buffer == रिक्त)
	प्रत्यागम;

    यदि (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    यदि (name != रिक्त)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    अन्यथा
    {
	यदि (!fullname && curbuf->b_fname != रिक्त)
	    name = curbuf->b_fname;
	अन्यथा
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

    यदि (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_पंक्तियाँ_संदेश));
    अन्यथा यदि (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_पंक्ति_गिनती),
	    (दीर्घ)curbuf->b_ml.ml_पंक्ति_गिनती,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_पंक्ति_गिनती));
    अन्यथा
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (दीर्घ)curwin->w_cursor.lnum,
	    (दीर्घ)curbuf->b_ml.ml_पंक्ति_गिनती,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_पंक्ति_गिनती));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (पूर्णाङ्क)curwin->w_cursor.col + 1, (पूर्णाङ्क)curwin->w_virtcol + 1);
    }

    (शून्य)append_arg_संख्या(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    यदि (dont_truncate)
    {
	पूर्णाङ्क	n;

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
	वर्ण	*p = संदेश_trunc_attr(buffer, FALSE, 0);
	यदि (restart_edit != 0 || (संदेश_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_संदेश((char_u *)p, 0);
    }

    vim_free(buffer);
}

    पूर्णाङ्क
col_print(
    char_u  *buf,
    माप_प्रकार  buflen,
    पूर्णाङ्क	    col,
    पूर्णाङ्क	    vcol)
{
    यदि (col == vcol)
	प्रत्यागम (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen, "%d", col);

    प्रत्यागम (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen, "%d-%d", col, vcol);
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
    माप_प्रकार	buflen = 0;

    यदि (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	प्रत्यागम;
    }

    need_maketitle = FALSE;
    यदि (!p_title && !p_icon && lasttitle == रिक्त && lasticon == रिक्त)
	प्रत्यागम;  // nothing to do

    यदि (p_title)
    {
	पूर्णाङ्क maxlen = 0;

	यदि (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    यदि (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	यदि (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    यदि (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, परिमाण(buf), p_titlestring,
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
#define SPACE_FOR_FNAME (परिमाण(buf) - 100)
#define SPACE_FOR_DIR   (परिमाण(buf) - 20)
#define SPACE_FOR_ARGNR (परिमाण(buf) - 10)  // at least room for " - VIM"

	    // file name
	    यदि (curbuf->b_fname == रिक्त)
		buflen = vim_snprintf_safelen((वर्ण *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    अन्यथा यदि (curbuf->b_term != रिक्त)
		buflen = vim_snprintf_safelen((वर्ण *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    अन्यथा
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
	    यदि (curbuf->b_term == रिक्त)
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
	    यदि (curbuf->b_fname != रिक्त
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
		यदि (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		यदि (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((वर्ण *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		अन्यथा
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    यदि (buflen < SPACE_FOR_DIR)
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
		    अन्यथा
			buflen += vim_snprintf_safelen((वर्ण *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_संख्या(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((वर्ण *)buf + buflen,
		परिमाण(buf) - buflen, " - %s",
#यदि defined(FEAT_CLIENTSERVER)
		(serverName != रिक्त)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    यदि (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		यदि (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, परिमाण(buf));
	    }
	}
    }
    mustset = मूल्य_changed(title_str, &lasttitle);

    यदि (p_icon)
    {
	icon_str = buf;
	यदि (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    यदि (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, परिमाण(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, रिक्त, रिक्त,
				 रिक्त);
	    अन्यथा
#endif
		icon_str = p_iconstring;
	}
	अन्यथा
	{
	    char_u  *name;
	    पूर्णाङ्क	    namelen;

	    name = buf_spname(curbuf);
	    यदि (name == रिक्त)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (पूर्णाङ्क)STRLEN(name);
	    यदि (namelen > 100)
	    {
		namelen -= 100;
		यदि (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, परिमाण(buf));
	}
    }

    mustset |= मूल्य_changed(icon_str, &lasticon);

    यदि (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    स्थिर पूर्णाङ्क
मूल्य_changed(char_u *str, char_u **last)
{
    यदि ((str == रिक्त) != (*last == रिक्त)
	    || (str != रिक्त && *last != रिक्त && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	यदि (str == रिक्त)
	{
	    *last = रिक्त;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	अन्यथा
	{
	    *last = vim_strsave(str);
	    प्रत्यागम TRUE;
	}
    }
    प्रत्यागम FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    शून्य
resettitle(शून्य)
{
    mch_settitle(lasttitle, lasticon);
}

#यदि defined(EXITFREE)
    शून्य
free_titles(शून्य)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#यदि defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

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

स्थिर माप_प्रकार		stl_वस्तुएँ_len = 20; // Initial value, grows as needed.
स्थिर stl_item_T      *stl_वस्तुएँ = रिक्त;
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
    माप_प्रकार	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    पूर्णाङ्क		opt_scope,	// scope for "opt_name"
    पूर्णाङ्क		fillchar,
    पूर्णाङ्क		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    प्रत्यागम build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    रिक्त, रिक्त);
}

    पूर्णाङ्क
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    माप_प्रकार	outlen,		// length of out[]
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
    प्रत्यागम build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    रिक्त, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    पूर्णाङ्क
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    माप_प्रकार	outlen,		// length of out[]
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
    प्रत्यागम build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
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
    प्रत्यागम rendered_height;
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
    माप_प्रकार	outlen,		// length of out[]
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
    माप_प्रकार	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    पूर्णाङ्क		byteval;
# ifdef FEAT_EVAL
    पूर्णाङ्क		use_sandbox;
    पूर्णाङ्क		save_VIsual_active;
# endif
    पूर्णाङ्क		empty_पंक्ति;
    दीर्घ	l;
    दीर्घ	n;
    पूर्णाङ्क		prevchar_isflag;
    पूर्णाङ्क		prevchar_isitem;
    पूर्णाङ्क		itemisflag;
    पूर्णाङ्क		fillable;
    char_u	*str;
    दीर्घ	अंक;
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
    पूर्णाङ्क		save_कुंजी_Typed = कुंजी_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    पूर्णाङ्क		did_emsg_before = did_emsg;
    पूर्णाङ्क		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    यदि (updating_screen)
	redraw_not_allowed = TRUE;

    यदि (stl_वस्तुएँ == रिक्त)
    {
	stl_वस्तुएँ = ALLOC_MULT(stl_item_T, stl_वस्तुएँ_len);
	stl_groupitem = ALLOC_MULT(पूर्णाङ्क, stl_वस्तुएँ_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_वस्तुएँ_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_वस्तुएँ_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_वस्तुएँ_len + 1);

	stl_separator_locations = ALLOC_MULT(पूर्णाङ्क, stl_वस्तुएँ_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    यदि (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_संख्या;
	tv.vval.v_संख्या = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	यदि (usefmt == रिक्त)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    यदि (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    यदि (lnum > wp->w_buffer->b_ml.ml_पंक्ति_गिनती)
    {
	lnum = wp->w_buffer->b_ml.ml_पंक्ति_गिनती;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_पंक्ति = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    यदि (wp->w_cursor.col > len)
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
    यदि (carry_hl != रिक्त && *carry_hl != 0)
    {
	stl_वस्तुएँ[curitem].stl_type = Highlight;
	stl_वस्तुएँ[curitem].stl_start = p;
	stl_वस्तुएँ[curitem].stl_minwid = *carry_hl;
	stl_वस्तुएँ[curitem].stl_clickfunc = रिक्त;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    प्रति (s = usefmt; *s != NUL; )
    {
	यदि (curitem == (पूर्णाङ्क)stl_वस्तुएँ_len)
	{
	    माप_प्रकार	new_len = stl_वस्तुएँ_len * 3 / 2;

	    stl_item_T *new_वस्तुएँ =
			  vim_realloc(stl_वस्तुएँ, परिमाण(stl_item_T) * new_len);
	    यदि (new_वस्तुएँ == रिक्त)
		भङ्ग;
	    stl_वस्तुएँ = new_वस्तुएँ;

	    पूर्णाङ्क *new_groupitem =
			     vim_realloc(stl_groupitem, परिमाण(पूर्णाङ्क) * new_len);
	    यदि (new_groupitem == रिक्त)
		भङ्ग;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  परिमाण(stl_hlrec_T) * (new_len + 1));
	    यदि (new_hlrec == रिक्त)
		भङ्ग;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  परिमाण(stl_hlrec_T) * (new_len + 1));
	    यदि (new_hlrec == रिक्त)
		भङ्ग;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      परिमाण(stl_clickrec_T) * (new_len + 1));
	    यदि (new_clickrec == रिक्त)
		भङ्ग;
	    stl_clicktab = new_clickrec;

	    पूर्णाङ्क *new_separator_locs = vim_realloc(stl_separator_locations,
					    परिमाण(पूर्णाङ्क) * new_len);
	    यदि (new_separator_locs == रिक्त)
		भङ्ग;
	    stl_separator_locations = new_separator_locs;

	    stl_वस्तुएँ_len = new_len;
	}

	stl_वस्तुएँ[curitem].stl_clickfunc = रिक्त;

	यदि (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	यावत् (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    यदि (*s == '\n' || *s == '\r')
	    {
		यदि (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    गच्छ find_linebreak;
		}
		अन्यथा यदि (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	यदि (*s == NUL || p + 1 >= out + outlen)
	    भङ्ग;

	/*
	 * Handle one '%' item.
	 */
	s++;
	यदि (*s == NUL)  // ignore trailing %
	    भङ्ग;

	यदि (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    यदि (s[1] == ']')
	    {
		stl_वस्तुएँ[curitem].stl_type = ClickFunc;
		stl_वस्तुएँ[curitem].stl_start = p;
		stl_वस्तुएँ[curitem].stl_minwid = 0;
		stl_वस्तुएँ[curitem].stl_clickfunc = रिक्त;
		s += 2;
		curitem++;
		अनुवर्त;
	    }
	    // %[FuncName] - start click region
	    यदि (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		यदि (rb != रिक्त)
		{
		    stl_वस्तुएँ[curitem].stl_type = ClickFunc;
		    stl_वस्तुएँ[curitem].stl_start = p;
		    stl_वस्तुएँ[curitem].stl_minwid = 0;
		    stl_वस्तुएँ[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    अनुवर्त;
		}
	    }
	}
	यदि (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    यदि (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		भङ्ग;
	    }
	    अन्यथा यदि (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	यदि (*s == '%')
	{
	    यदि (p + 1 >= out + outlen)
		भङ्ग;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    अनुवर्त;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	यदि (*s == STL_SEPARATE)
	{
	    s++;
	    यदि (groupdepth > 0)
		अनुवर्त;
	    stl_वस्तुएँ[curitem].stl_type = Separate;
	    stl_वस्तुएँ[curitem++].stl_start = p;
	    अनुवर्त;
	}
	यदि (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_वस्तुएँ[curitem].stl_type = Trunc;
	    stl_वस्तुएँ[curitem++].stl_start = p;
	    अनुवर्त;
	}
	यदि (*s == ')')
	{
	    char_u  *t;

	    s++;
	    यदि (groupdepth < 1)
		अनुवर्त;
	    groupdepth--;

	    t = stl_वस्तुएँ[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    यदि (curitem > stl_groupitem[groupdepth] + 1
		    && stl_वस्तुएँ[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		पूर्णाङ्क group_start_userhl = 0;
		पूर्णाङ्क group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		प्रति (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    यदि (stl_वस्तुएँ[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_वस्तुएँ[n].stl_minwid;
			भङ्ग;
		    }
		}
		प्रति (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    यदि (stl_वस्तुएँ[n].stl_type == Normal)
			भङ्ग;
		    यदि (stl_वस्तुएँ[n].stl_type == Highlight)
			group_end_userhl = stl_वस्तुएँ[n].stl_minwid;
		}
		यदि (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    प्रति (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			यदि (stl_वस्तुएँ[n].stl_type == Highlight)
			    stl_वस्तुएँ[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			यदि (stl_वस्तुएँ[n].stl_type == TabPage)
			    stl_वस्तुएँ[n].stl_start = p;
		    }
		}
	    }
	    यदि (l > stl_वस्तुएँ[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		यदि (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    यावत् (l >= stl_वस्तुएँ[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		अन्यथा
		    n = (दीर्घ)(p - t) - stl_वस्तुएँ[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (माप_प्रकार)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		पूर्णाङ्क minwid_fixed = MIN(stl_वस्तुएँ[stl_groupitem[groupdepth]].stl_minwid,
				       stl_वस्तुएँ[stl_groupitem[groupdepth]].stl_maxwid);
		यावत् (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		प्रति (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_वस्तुएँ[l].stl_start -= n - 1;
		    यदि (stl_वस्तुएँ[l].stl_start < t)
			stl_वस्तुएँ[l].stl_start = t;
		}
	    }
	    अन्यथा यदि (असल(stl_वस्तुएँ[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_वस्तुएँ[stl_groupitem[groupdepth]].stl_minwid;
		पूर्णाङ्क fillchar_len = MB_CHAR2LEN(fillchar);
		यदि (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    यावत् (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		अन्यथा
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    यदि (p + l >= out + outlen)
		    {
			n = (दीर्घ)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (माप_प्रकार)(p - t));
		    p += l;
		    प्रति ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    प्रति (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_वस्तुएँ[n].stl_start += l;
		}
	    }
	    अनुवर्त;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	यदि (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	यदि (*s == '-')
	{
	    s++;
	    l = -1;
	}
	यदि (VIM_ISDIGIT(*s))
	{
	    minwid = (पूर्णाङ्क)getdigits(&s);
	    यदि (minwid < 0)	// overflow
		minwid = 0;
	}
	यदि (*s == STL_USER_HL)
	{
	    stl_वस्तुएँ[curitem].stl_type = Highlight;
	    stl_वस्तुएँ[curitem].stl_start = p;
	    stl_वस्तुएँ[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    अनुवर्त;
	}
	यदि (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    यदि (*s == STL_TABCLOSENR)
	    {
		यदि (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    प्रति (n = curitem - 1; n >= 0; --n)
			यदि (stl_वस्तुएँ[n].stl_type == TabPage
					       && stl_वस्तुएँ[n].stl_minwid >= 0)
			{
			    minwid = stl_वस्तुएँ[n].stl_minwid;
			    भङ्ग;
			}
		}
		अन्यथा
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_वस्तुएँ[curitem].stl_type = TabPage;
	    stl_वस्तुएँ[curitem].stl_start = p;
	    stl_वस्तुएँ[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    अनुवर्त;
	}
	यदि (*s == '.')
	{
	    s++;
	    यदि (VIM_ISDIGIT(*s))
	    {
		maxwid = (पूर्णाङ्क)getdigits(&s);
		यदि (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	पूर्णाङ्क raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	यदि (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_वस्तुएँ[curitem].stl_type = Group;
	    stl_वस्तुएँ[curitem].stl_start = p;
	    stl_वस्तुएँ[curitem].stl_minwid = minwid;
	    stl_वस्तुएँ[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    अनुवर्त;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	यदि (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    अनुवर्त;
	}
# endif
	यदि (vim_strchr(STL_ALL, *s) == रिक्त)
	{
	    यदि (*s == NUL)  // can happen with "%0"
		भङ्ग;
	    s++;
	    अनुवर्त;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	अंक = -1;
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
	    यदि (name != रिक्त)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    अन्यथा
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    यदि (opt != STL_FILENAME)
		str = NameBuff;
	    अन्यथा
		str = gettail(NameBuff);
	    भङ्ग;
	}

	पक्ष STL_VIM_व्यंजक: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    पूर्णाङ्क	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    यदि (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    यावत् ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    यदि (*s != '}')	// missing '}' or out of space
		भङ्ग;
	    s++;
	    यदि (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    अन्यथा
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
	    यदि (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    यदि (!zeropad && str != रिक्त && *str != NUL)
	    {
		यदि (*skipdigits(str) == NUL)
		{
		    अंक = म_से_प((वर्ण *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    यदि (reevaluate && str != रिक्त && *str != NUL
		    && म_अक्षर((ध्रुव वर्ण *)str, '%') != रिक्त
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		माप_प्रकार parsed_usefmt = (माप_प्रकार)(block_start - usefmt);
		माप_प्रकार str_length = म_माप((ध्रुव वर्ण *)str);
		माप_प्रकार fmt_length = म_माप((ध्रुव वर्ण *)s);
		माप_प्रकार new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * परिमाण(char_u));

		यदि (new_fmt != रिक्त)
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

		    यदि (usefmt != fmt)
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
	पक्ष STL_पंक्ति:
	    अंक = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (दीर्घ)(wp->w_cursor.lnum);
	    भङ्ग;

	पक्ष STL_NUMLINES:
	    अंक = wp->w_buffer->b_ml.ml_पंक्ति_गिनती;
	    भङ्ग;

	पक्ष STL_COLUMN:
	    अंक = (State & MODE_जोड़ो) == 0 && empty_पंक्ति
					       ? 0 : (पूर्णाङ्क)wp->w_cursor.col + 1;
	    भङ्ग;

	पक्ष STL_VIRTCOL:
	पक्ष STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    यदि (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_जोड़ो) == 0
			       && empty_पंक्ति ? 0 : (पूर्णाङ्क)wp->w_cursor.col + 1)))
		भङ्ग;
	    अंक = (दीर्घ)virtcol;
	    भङ्ग;
	}

	पक्ष STL_PERCENTAGE:
	    अंक = calc_percentage((दीर्घ)wp->w_cursor.lnum, (दीर्घ)wp->w_buffer->b_ml.ml_पंक्ति_गिनती);
	    भङ्ग;

	पक्ष STL_ALTPERCENT:
	    str = buf_tmp;
	    (शून्य)get_rel_pos(wp, str, TMPLEN);
	    भङ्ग;

	पक्ष STL_SHOWCMD:
	    यदि (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    भङ्ग;

	पक्ष STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    यदि (append_arg_संख्या(wp, buf_tmp, परिमाण(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    भङ्ग;

	पक्ष STL_KEYMAP:
	    fillable = FALSE;
	    यदि (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    भङ्ग;
	पक्ष STL_PAGENUM:
# यदि defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    अंक = printer_page_अंक;
# अन्यथा
	    अंक = 0;
# endif
	    भङ्ग;

	पक्ष STL_BUFNO:
	    अंक = wp->w_buffer->b_fnum;
	    भङ्ग;

	पक्ष STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	पक्ष STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_पंक्ति_or_offset(wp->w_buffer, wp->w_cursor.lnum, रिक्त);
	    अंक = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_जोड़ो) == 0 && empty_पंक्ति
				? 0 : (पूर्णाङ्क)wp->w_cursor.col);
# endif
	    भङ्ग;

	पक्ष STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	पक्ष STL_BYTEVAL:
	    अंक = byteval;
	    यदि (अंक == NL)
		अंक = 0;
	    अन्यथा यदि (अंक == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		अंक = NL;
	    भङ्ग;

	पक्ष STL_ROFLAG:
	पक्ष STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    यदि (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    भङ्ग;

	पक्ष STL_HELPFLAG:
	पक्ष STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    यदि (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    भङ्ग;

	पक्ष STL_FILETYPE:
	    यदि (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((वर्ण *)buf_tmp, परिमाण(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    भङ्ग;

	पक्ष STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    यदि (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((वर्ण *)buf_tmp, परिमाण(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		प्रति (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    भङ्ग;

# यदि defined(FEAT_QUICKFIX)
	पक्ष STL_PREVIEWFLAG:
	पक्ष STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    यदि (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    भङ्ग;

	पक्ष STL_QUICKFIX:
	    यदि (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(संदेश_loclist)
			    : _(संदेश_qflist));
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

		यावत् (*s != '#' && *s != NUL)
		    ++s;
		यदि (*s == '#')
		{
		    stl_वस्तुएँ[curitem].stl_type = Highlight;
		    stl_वस्तुएँ[curitem].stl_start = p;
		    stl_वस्तुएँ[curitem].stl_minwid = -syn_namen2id(t, (पूर्णाङ्क)(s - t));
		    curitem++;
		}
		यदि (*s != NUL)
		    ++s;
		अनुवर्त;
	    }

	पक्ष STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    यदि (*s == ']')
	    {
		stl_वस्तुएँ[curitem].stl_type = ClickFunc;
		stl_वस्तुएँ[curitem].stl_start = p;
		stl_वस्तुएँ[curitem].stl_minwid = 0;
		stl_वस्तुएँ[curitem].stl_clickfunc = रिक्त;
		s++;
		curitem++;
		अनुवर्त;
	    }
	    // %N[FuncName] with minwid
	    यदि (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		यदि (rb != रिक्त)
		{
		    stl_वस्तुएँ[curitem].stl_type = ClickFunc;
		    stl_वस्तुएँ[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_वस्तुएँ[curitem].stl_minwid = raw_minwid;
		    stl_वस्तुएँ[curitem].stl_clickfunc =
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

	stl_वस्तुएँ[curitem].stl_start = p;
	stl_वस्तुएँ[curitem].stl_type = Normal;
	यदि (str != रिक्त && *str)
	{
	    char_u  *t = str;

	    यदि (itemisflag)
	    {
		यदि ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    यदि (l > 0)
		prevchar_isitem = TRUE;
	    यदि (l > maxwid)
	    {
		यावत् (l >= maxwid)
		    यदि (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    अन्यथा
			l -= byte2cells(*t++);
		यदि (p + 1 >= out + outlen)
		    भङ्ग;
		*p++ = '<';
	    }
	    यदि (minwid > 0)
	    {
		प्रति (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    यदि (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    अन्यथा
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    अन्यथा
		minwid *= -1;
	    प्रति (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		यदि (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		अन्यथा
		    *p++ = *t;
	    }
	    प्रति (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	अन्यथा यदि (अंक >= 0)
	{
	    पूर्णाङ्क	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    यदि (p + 20 >= out + outlen)
		भङ्ग;		// not sufficient space
	    prevchar_isitem = TRUE;
	    यदि (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    यदि (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    प्रति (n = अंक, l = 1; n >= nbase; n /= nbase)
		l++;
	    यदि (opt == STL_VIRTCOL_ALT)
		l++;

	    यदि (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		यावत् (l-- > maxwid)
		    अंक /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((वर्ण *)p, outlen - (p - out),
		    (वर्ण *)nstr, 0, अंक, n);
	    }
	    अन्यथा
		p += vim_snprintf_safelen((वर्ण *)p, outlen - (p - out),
		    (वर्ण *)nstr, minwid, अंक);
	}
	अन्यथा
	    stl_वस्तुएँ[curitem].stl_type = Empty;

	यदि (अंक >= 0 || (!itemisflag && str != रिक्त && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	यदि (opt == STL_VIM_व्यंजक)
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
    यदि (carry_hl != रिक्त)
    {
	पूर्णाङ्क last_hl = 0;
	प्रति (l = 0; l < itemcnt; l++)
	    यदि (stl_वस्तुएँ[l].stl_type == Highlight)
		last_hl = stl_वस्तुएँ[l].stl_minwid;
	*carry_hl = last_hl;
    }

    यदि (mode == STL_MODE_MULTI
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
	यदि (usefmt != fmt)
	    vim_free(fmt);
# endif
	माप_प्रकार fmt_remain_len = म_माप((वर्ण *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    अन्यथा
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	यदि (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    यदि (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	प्रति (l = 0; l < itemcnt; l++)
	    यदि (stl_वस्तुएँ[l].stl_type == ClickFunc)
		vim_free(stl_वस्तुएँ[l].stl_clickfunc);
	यदि (rendered_height != रिक्त)
	    *rendered_height = rheight;
	प्रत्यागम 0;
    }

    width = vim_strsize(out);
    यदि (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	यदि (itemcnt == 0)
	    s = out;
	अन्यथा
	{
	    प्रति ( ; l < itemcnt; l++)
		यदि (stl_वस्तुएँ[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_वस्तुएँ[l].stl_start;
		    भङ्ग;
		}
	    यदि (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_वस्तुएँ[0].stl_start;
		l = 0;
	    }
	}

	यदि (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    यदि (has_mbyte)
	    {
		s = out;
		width = 0;
		प्रति (;;)
		{
		    width += ptr2cells(s);
		    यदि (width >= maxwidth)
			भङ्ग;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		यावत् (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    अन्यथा
		s = out + maxwidth - 1;
	    प्रति (l = 0; l < itemcnt; l++)
		यदि (stl_वस्तुएँ[l].stl_start > s)
		    भङ्ग;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	अन्यथा
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    यावत् (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (माप_प्रकार)(end - p) + 1);	// +1 for NUL
	    end -= (माप_प्रकार)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    प्रति (; l < itemcnt; l++)
	    {
		यदि (stl_वस्तुएँ[l].stl_start - n >= s)
		    stl_वस्तुएँ[l].stl_start -= n;
		अन्यथा
		    stl_वस्तुएँ[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    यावत् (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    अन्यथा यदि (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	पूर्णाङ्क अंक_separators = 0;

	प्रति (l = 0; l < itemcnt; l++)
	{
	    यदि (stl_वस्तुएँ[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[अंक_separators] = l;
		अंक_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	यदि (अंक_separators)
	{
	    पूर्णाङ्क standard_spaces;
	    पूर्णाङ्क final_spaces;

	    standard_spaces = (maxwidth - width) / अंक_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (अंक_separators - 1);
	    प्रति (l = 0; l < अंक_separators; l++)
	    {
		पूर्णाङ्क dislocation = (l == (अंक_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_वस्तुएँ[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		प्रति (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		प्रति (पूर्णाङ्क i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_वस्तुएँ[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    यदि (hltab != रिक्त)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	प्रति (l = 0; l < itemcnt; l++)
	{
	    यदि (stl_वस्तुएँ[l].stl_type == Highlight)
	    {
		sp->start = stl_वस्तुएँ[l].stl_start;
		sp->userhl = stl_वस्तुएँ[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = रिक्त;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    यदि (tabtab != रिक्त)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	प्रति (l = 0; l < itemcnt; l++)
	{
	    यदि (stl_वस्तुएँ[l].stl_type == TabPage)
	    {
		sp->start = stl_वस्तुएँ[l].stl_start;
		sp->userhl = stl_वस्तुएँ[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = रिक्त;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    यदि (clicktab != रिक्त)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	प्रति (l = 0; l < itemcnt; l++)
	{
	    यदि (stl_वस्तुएँ[l].stl_type == ClickFunc)
	    {
		cp->start = stl_वस्तुएँ[l].stl_start;
		cp->funcname = stl_वस्तुएँ[l].stl_clickfunc;
		cp->minwid = stl_वस्तुएँ[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = रिक्त;
	cp->funcname = रिक्त;
    }
    अन्यथा
    {
	// Free click function names when caller doesn't need them.
	प्रति (l = 0; l < itemcnt; l++)
	    यदि (stl_वस्तुएँ[l].stl_type == ClickFunc)
		vim_free(stl_वस्तुएँ[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    कुंजी_Typed = save_कुंजी_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    यदि (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    प्रत्यागम width;
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
    दीर्घ	above; // number of lines above window
    दीर्घ	below; // number of lines below window

    यदि (buflen < 3) // need at least 3 chars for writing
	प्रत्यागम 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    यदि (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_पंक्ति_गिनती - wp->w_botline + 1;
    यदि (below <= 0)
	प्रत्यागम (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    यदि (above <= 0)
	प्रत्यागम (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen,
	    "%s", _("Top"));

    पूर्णाङ्क perc = calc_percentage(above, above + below);
    वर्ण tmp[8];
    // localized percentage value
    vim_snprintf(tmp, परिमाण(tmp), _("%d%%"), perc);
    प्रत्यागम (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    स्थिर पूर्णाङ्क
append_arg_संख्या(
    win_T	*wp,
    char_u	*buf,
    माप_प्रकार	buflen,
    पूर्णाङ्क		add_file)	// Add "file" before the arg number
{
    यदि (ARGCOUNT <= 1)		// nothing to do
	प्रत्यागम 0;

    वर्ण *संदेश;
    विकल्प ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	पक्ष 0: संदेश = _(" (%d of %d)"); भङ्ग;
	पक्ष 1: संदेश = _(" ((%d) of %d)"); भङ्ग;
	पक्ष 2: संदेश = _(" (file %d of %d)"); भङ्ग;
	पक्ष 3: संदेश = _(" (file (%d) of %d)"); भङ्ग;
    }

    प्रत्यागम (पूर्णाङ्क)vim_snprintf_safelen((वर्ण *)buf, buflen, संदेश,
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
    प्रत्यागम FullName_save(fname, TRUE);
#अन्यथा
    यदि (!vim_isAbsName(fname)
	    || म_माला((वर्ण *)fname, "..") != रिक्त
	    || म_माला((वर्ण *)fname, "//") != रिक्त
# ifdef BACKSLASH_IN_FILENAME
	    || म_माला((वर्ण *)fname, "\\\\") != रिक्त
# endif
# यदि defined(MSWIN)
	    || vim_strchr(fname, '~') != रिक्त
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	प्रत्यागम FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    यदि (fname != रिक्त)
	fname_case(fname, 0);	// set correct case for file name
# endif

    प्रत्यागम fname;
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
    यदि (*ffname == रिक्त)	    // no file name given, nothing to do
	प्रत्यागम;
    यदि (*sfname == रिक्त)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    यदि (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	यदि (rfname != रिक्त)
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
    पूर्णाङ्क		गिनती;		// Maximum number of windows to open.
    पूर्णाङ्क		all;		// When TRUE also load inactive buffers.
    पूर्णाङ्क		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    यदि (eap->addr_गिनती == 0)	// make as many windows as possible
	गिनती = 9999;
    अन्यथा
	गिनती = eap->line2;	// make as many windows as specified
    यदि (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
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
    यदि (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    प्रति (;;)
    {
	tpnext = curtab->tp_next;
	प्रति (wp = firstwin; wp != रिक्त; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    यदि ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		यदि (win_close(wp, FALSE) == FAIL)
		    भङ्ग;
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
	यदि (had_tab == 0 || tpnext == रिक्त)
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
    प्रति (buf = firstbuf; buf != रिक्त && open_wins < गिनती; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	यदि ((!all && buf->b_ml.ml_mfp == रिक्त) || !buf->b_p_bl)
	    अनुवर्त;

	यदि (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    यदि (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    अन्यथा
		wp = रिक्त;
	}
	अन्यथा
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		यदि (wp->w_buffer == buf)
		    भङ्ग;
	    // If the buffer already has a window, move it
	    यदि (wp != रिक्त)
		win_move_after(wp, curwin);
	}

	यदि (wp == रिक्त && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    यदि (split_ret == FAIL)
		अनुवर्त;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    यदि (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		भङ्ग;
	    }
	    यदि (swap_exists_action == SEA_QUIT)
	    {
#यदि defined(FEAT_EVAL)
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

#यदि defined(FEAT_EVAL)
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
	यदि (got_int)
	{
	    (शून्य)vgetc();	// only break the file loading, not the rest
	    भङ्ग;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	यदि (aborting())
	    भङ्ग;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	यदि (had_tab > 0 && tabpage_index(रिक्त) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    प्रति (wp = lastwin; open_wins > गिनती; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	यदि (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	अन्यथा यदि (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	अन्यथा
	{
	    wp = wp->w_prev;
	    यदि (wp == रिक्त)
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

    यदि (!curbuf->b_p_ml || (nmlines = (पूर्णाङ्क)p_mls) == 0)
	प्रत्यागम;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    यदि (entered)
	प्रत्यागम;

    ++entered;
    प्रति (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_पंक्ति_गिनती && lnum <= nmlines;
								       ++lnum)
	यदि (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    प्रति (lnum = curbuf->b_ml.ml_पंक्ति_गिनती; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_पंक्ति_गिनती - nmlines; --lnum)
	यदि (chk_modeline(lnum, flags) == FAIL)
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
    char_u	*पंक्ति_end;		// point to the end of the line
    char_u	*e;
    पूर्णाङ्क		prev;
    पूर्णाङ्क		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    पंक्ति_end = s + ml_get_len(lnum);
    प्रति (; *s != NUL; ++s)
    {
	यदि (prev == -1 || vim_isspace(prev))
	{
	    यदि ((prev != -1 && STRNCMP(s, "ex:", (माप_प्रकार)3) == 0)
		    || STRNCMP(s, "vi:", (माप_प्रकार)3) == 0)
		भङ्ग;
	    // Accept both "vim" and "Vim".
	    यदि ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		पूर्णाङ्क vers;

		यदि (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		अन्यथा
		    e = s + 3;
		vers = getdigits(&e);
		यदि (*e == ':'
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

    यदि (*s)
    {
	माप_प्रकार	len;
	char_u	*linecopy;		// local copy of any modeline found
	पूर्णाङ्क	end;

	कुरु				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	यावत् (s[-1] != ':');

	len = (माप_प्रकार)(पंक्ति_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	यदि (linecopy == रिक्त)
	    प्रत्यागम FAIL;

	पंक्ति_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_धकेलो(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	यावत् (end == FALSE)
	{
	    s = skipwhite(s);
	    यदि (*s == NUL)
		भङ्ग;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    प्रति (e = s; *e != ':' && *e != NUL; ++e)
		यदि (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (माप_प्रकार)(पंक्ति_end - (e + 1)) + 1);	// +1 for NUL
		    --पंक्ति_end;
		}
	    यदि (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    यदि (STRNCMP(s, "set ", (माप_प्रकार)4) == 0
		    || STRNCMP(s, "se ", (माप_प्रकार)3) == 0)
	    {
		यदि (*e != ':')		// no terminating ':'?
		    भङ्ग;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    यदि (*s != NUL)		// skip over an empty "::"
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
		यदि (retval == FAIL)		// stop if error found
		    भङ्ग;
	    }
	    s = (e == पंक्ति_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_निकालो();
	vim_free(linecopy);
    }
    प्रत्यागम retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    पूर्णाङ्क
bt_normal(buf_T *buf)
{
    प्रत्यागम buf != रिक्त && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    पूर्णाङ्क
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    प्रत्यागम buf != रिक्त && buf->b_p_bt[0] == 'q';
#अन्यथा
    प्रत्यागम FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    पूर्णाङ्क
bt_terminal(buf_T *buf UNUSED)
{
#यदि defined(FEAT_TERMINAL)
    प्रत्यागम buf != रिक्त && buf->b_p_bt[0] == 't';
#अन्यथा
    प्रत्यागम FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    पूर्णाङ्क
bt_help(buf_T *buf)
{
    प्रत्यागम buf != रिक्त && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    पूर्णाङ्क
bt_prompt(buf_T *buf)
{
    प्रत्यागम buf != रिक्त && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#यदि defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    पूर्णाङ्क
bt_popup(buf_T *buf)
{
    प्रत्यागम buf != रिक्त && buf->b_p_bt != रिक्त
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
    प्रत्यागम buf != रिक्त && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
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
    प्रत्यागम buf != रिक्त && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#यदि defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    पूर्णाङ्क
bt_nofile(buf_T *buf)
{
    प्रत्यागम buf != रिक्त && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    पूर्णाङ्क
bt_dontwrite(buf_T *buf)
{
    प्रत्यागम buf != रिक्त && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    पूर्णाङ्क
bt_dontwrite_संदेश(buf_T *buf)
{
    यदि (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
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
	पक्ष 'd': प्रत्यागम FALSE;	    // "delete"
	पक्ष 'h': प्रत्यागम TRUE;	    // "hide"
    }
    प्रत्यागम (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#यदि defined(FEAT_QUICKFIX)
    यदि (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	यदि (buf->b_fnum == qf_ढेर_get_bufnr())
	    प्रत्यागम (char_u *)_(संदेश_qflist);
	अन्यथा
	    प्रत्यागम (char_u *)_(संदेश_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    यदि (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	यदि (buf->b_term != रिक्त)
	    प्रत्यागम term_get_status_text(buf->b_term);
#endif
	यदि (buf->b_fname != रिक्त)
	    प्रत्यागम buf->b_fname;
	यदि (buf == cmdwin_buf)
	    प्रत्यागम (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	यदि (bt_prompt(buf))
	    प्रत्यागम (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	यदि (bt_popup(buf))
	    प्रत्यागम (char_u *)_("[Popup]");
#endif
	प्रत्यागम (char_u *)_("[Scratch]");
    }

    यदि (buf->b_fname == रिक्त)
	प्रत्यागम buf_get_fname(buf);
    प्रत्यागम रिक्त;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    यदि (buf->b_fname == रिक्त)
	प्रत्यागम (char_u *)_("[No Name]");
    प्रत्यागम buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    शून्य
set_buflisted(पूर्णाङ्क on)
{
    यदि (on == curbuf->b_p_bl)
	प्रत्यागम;

    curbuf->b_p_bl = on;
    यदि (on)
	apply_autocmds(EVENT_BUFADD, रिक्त, रिक्त, FALSE, curbuf);
    अन्यथा
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
    यदि (newbuf == रिक्त)
	प्रत्यागम TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    यदि (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	प्रत्यागम TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    यदि (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	प्रत्यागम TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    यदि (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	यदि (buf->b_ml.ml_पंक्ति_गिनती == curbuf->b_ml.ml_पंक्ति_गिनती)
	{
	    differ = FALSE;
	    प्रति (lnum = 1; lnum <= curbuf->b_ml.ml_पंक्ति_गिनती; ++lnum)
		यदि (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    भङ्ग;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    यदि (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    प्रत्यागम differ;
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
    यदि (buf->b_fnum == शीर्ष_file_अंक - 1)
	--शीर्ष_file_अंक;

    यदि (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(रिक्त, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    यदि (!aucmd)
	unblock_autocmds();
}
