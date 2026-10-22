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

#അന്തര്ഭാവ "vim.h"


#ifdef FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_DEPTH 100
#endif

സ്ഥിതം ശൂന്യം	enter_buffer(buf_T *buf);
സ്ഥിതം ശൂന്യം	buflist_getfpos(ശൂന്യം);
സ്ഥിതം char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, പൂർണ്ണസംഖ്യ ignore_case);
സ്ഥിതം char_u	*fname_match(regmatch_T *rmp, char_u *name, പൂർണ്ണസംഖ്യ ignore_case);
#ifdef UNIX
സ്ഥിതം buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
സ്ഥിതം പൂർണ്ണസംഖ്യ	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
സ്ഥിതം പൂർണ്ണസംഖ്യ	buf_same_ino(buf_T *buf, stat_T *stp);
#അല്ലെങ്കിൽ
സ്ഥിതം പൂർണ്ണസംഖ്യ	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ	മൂല്യം_changed(char_u *str, char_u **last);
#എങ്കിൽ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
സ്ഥിതം പൂർണ്ണസംഖ്യ build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, പൂർണ്ണസംഖ്യ opt_scope, പൂർണ്ണസംഖ്യ fillchar, പൂർണ്ണസംഖ്യ maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, പൂർണ്ണസംഖ്യ *lbreaks, പൂർണ്ണസംഖ്യ *carry_hl);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ	append_arg_സംഖ്യ(win_T *wp, char_u *buf, size_t buflen, പൂർണ്ണസംഖ്യ add_file);
സ്ഥിതം ശൂന്യം	free_buffer(buf_T *);
സ്ഥിതം ശൂന്യം	free_buffer_stuff(buf_T *buf, പൂർണ്ണസംഖ്യ free_options);
സ്ഥിതം പൂർണ്ണസംഖ്യ	bt_nofileread(buf_T *buf);
സ്ഥിതം പൂർണ്ണസംഖ്യ	do_buffer_ext(പൂർണ്ണസംഖ്യ action, പൂർണ്ണസംഖ്യ start, പൂർണ്ണസംഖ്യ dir, പൂർണ്ണസംഖ്യ എണ്ണം, പൂർണ്ണസംഖ്യ flags);

#ifdef UNIX
# define dev_T dev_t
#അല്ലെങ്കിൽ
# define dev_T unsigned
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    വേണ്ടി ((buf) = lastbuf; (buf) != ശൂന്യ; (buf) = (buf)->b_prev)

#എങ്കിൽ defined(FEAT_QUICKFIX)
സ്ഥിതം അക്ഷരം *സന്ദേശം_loclist = N_("[Location List]");
സ്ഥിതം അക്ഷരം *സന്ദേശം_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
സ്ഥിതം പൂർണ്ണസംഖ്യ	buf_free_എണ്ണം = 0;

സ്ഥിതം പൂർണ്ണസംഖ്യ	മുകൾ_file_അക്കം = 1;	// highest file number
സ്ഥിതം garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    സ്ഥിതം ശൂന്യം
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"എങ്കിൽ exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
calc_percentage(long part, long whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    തിരികെ (part > 1000000L)
	? (പൂർണ്ണസംഖ്യ)(part / (whole / 100L))
	: (പൂർണ്ണസംഖ്യ)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    പൂർണ്ണസംഖ്യ
get_highest_fnum(ശൂന്യം)
{
    തിരികെ മുകൾ_file_അക്കം - 1;
}

/*
 * Read data from buffer for retrying.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
read_buffer(
    പൂർണ്ണസംഖ്യ		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    പൂർണ്ണസംഖ്യ		flags)		    // extra flags for readfile()
{
    പൂർണ്ണസംഖ്യ		retval = OK;
    linenr_T	വരി_എണ്ണം;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    വരി_എണ്ണം = curbuf->b_ml.ml_വരി_എണ്ണം;
    retval = readfile(
	    read_stdin ? ശൂന്യ : curbuf->b_ffname,
	    read_stdin ? ശൂന്യ : curbuf->b_fname,
	    വരി_എണ്ണം, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    എങ്കിൽ (retval == OK)
    {
	// Delete the binary lines.
	ആകുമ്പോൾ (--വരി_എണ്ണം >= 0)
	    ml_delete((linenr_T)1);
    }
    അല്ലെങ്കിൽ
    {
	// Delete the converted lines.
	ആകുമ്പോൾ (curbuf->b_ml.ml_വരി_എണ്ണം > വരി_എണ്ണം)
	    ml_delete(വരി_എണ്ണം);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    എങ്കിൽ (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	എങ്കിൽ (!readonlymode && !BUFEMPTY())
	    changed();
	അല്ലെങ്കിൽ എങ്കിൽ (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	എങ്കിൽ (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, ശൂന്യ, ശൂന്യ, FALSE,
							      curbuf, &retval);
#അല്ലെങ്കിൽ
	    apply_autocmds(EVENT_STDINREADPOST, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
#endif
	}
    }
    തിരികെ retval;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    ശൂന്യം
buffer_ensure_loaded(buf_T *buf)
{
    എങ്കിൽ (buf->b_ml.ml_mfp != ശൂന്യ)
	തിരികെ;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    എങ്കിൽ (curbuf == buf)
    {
	എങ്കിൽ (swap_exists_action != SEA_READONLY)
	    swap_exists_action = SEA_NONE;
	open_buffer(FALSE, ശൂന്യ, 0);
	aucmd_restbuf(&aco);
    }
}
#endif

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
open_buffer(
    പൂർണ്ണസംഖ്യ		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    പൂർണ്ണസംഖ്യ		flags_arg)	    // extra flags for readfile()
{
    പൂർണ്ണസംഖ്യ		flags = flags_arg;
    പൂർണ്ണസംഖ്യ		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    പൂർണ്ണസംഖ്യ		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    എങ്കിൽ (readonlymode && curbuf->b_ffname != ശൂന്യ
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    എങ്കിൽ (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    എങ്കിൽ (curbuf->b_ml.ml_mfp != ശൂന്യ)
		നിർത്തുക;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	എങ്കിൽ (curbuf == ശൂന്യ)
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
	എങ്കിൽ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ശൂന്യ, curwin);
#endif
	തിരികെ FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    എങ്കിൽ (curbuf->b_ml.ml_mfp != ശൂന്യ)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    എങ്കിൽ (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    എങ്കിൽ (curbuf->b_ffname != ശൂന്യ
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	പൂർണ്ണസംഖ്യ old_സന്ദേശം_silent = സന്ദേശം_silent;
#ifdef UNIX
	പൂർണ്ണസംഖ്യ	save_bin = curbuf->b_p_bin;
	പൂർണ്ണസംഖ്യ perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	പൂർണ്ണസംഖ്യ oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	എങ്കിൽ (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	എങ്കിൽ (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	എങ്കിൽ (shortmess(SHM_FILEINFO))
	    സന്ദേശം_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	എങ്കിൽ (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    എങ്കിൽ (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_buffer(FALSE, eap, flags);
	}
#endif
	സന്ദേശം_silent = old_സന്ദേശം_silent;
#ifdef FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#endif
	// Help buffer is filtered.
	എങ്കിൽ (bt_help(curbuf))
	    fix_help_buffer();
    }
    അല്ലെങ്കിൽ എങ്കിൽ (read_stdin)
    {
	പൂർണ്ണസംഖ്യ	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(ശൂന്യ, ശൂന്യ, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, ശൂന്യ,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	എങ്കിൽ (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    എങ്കിൽ (curbuf->b_ml.ml_mfp != ശൂന്യ
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    എങ്കിൽ (curbuf->b_flags & BF_NEVERLOADED)
    {
	(ശൂന്യം)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    എങ്കിൽ ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != ശൂന്യ)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != ശൂന്യ)
#endif
       )
	changed();
    അല്ലെങ്കിൽ എങ്കിൽ (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    എങ്കിൽ (aborting())
#അല്ലെങ്കിൽ
    എങ്കിൽ (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    എങ്കിൽ (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf, &retval);
#അല്ലെങ്കിൽ
    apply_autocmds(EVENT_BUFENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
#endif

    എങ്കിൽ (retval != OK)
	തിരികെ retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    എങ്കിൽ (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != ശൂന്യ)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	എങ്കിൽ (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    എങ്കിൽ ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, ശൂന്യ, ശൂന്യ,
			FALSE, curbuf, &retval);
#അല്ലെങ്കിൽ
	    apply_autocmds(EVENT_BUFWINENTER, ശൂന്യ, ശൂന്യ,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    തിരികെ retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    ശൂന്യം
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == ശൂന്യ ? 0 : buf->b_fnum;
    bufref->br_buf_free_എണ്ണം = buf_free_എണ്ണം;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    പൂർണ്ണസംഖ്യ
bufref_valid(bufref_T *bufref)
{
    തിരികെ bufref->br_buf_free_എണ്ണം == buf_free_എണ്ണം
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    പൂർണ്ണസംഖ്യ
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	എങ്കിൽ (bp == buf)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
സ്ഥിതം hashtab_T buf_hashtab;

    സ്ഥിതം ശൂന്യം
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((അക്ഷരം *)buf->b_താക്കോൽ, വലുപ്പം(buf->b_താക്കോൽ), "%x", buf->b_fnum);
    എങ്കിൽ (hash_add(&buf_hashtab, buf->b_താക്കോൽ, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    സ്ഥിതം ശൂന്യം
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_താക്കോൽ);

    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
can_unload_buffer(buf_T *buf)
{
    പൂർണ്ണസംഖ്യ	    can_unload = !buf->b_locked;

    എങ്കിൽ (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    എങ്കിൽ (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		നിർത്തുക;
	    }
    }
    // Don't unload the buffer while it's still being saved
    എങ്കിൽ (can_unload && buf->b_saving)
	can_unload = FALSE;

    എങ്കിൽ (!can_unload)
    {
	char_u *fname = buf->b_fname != ശൂന്യ ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != ശൂന്യ ? fname : (char_u *)"[No Name]");
    }
    തിരികെ can_unload;
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
    പൂർണ്ണസംഖ്യ
close_buffer(
    win_T	*win,
    buf_T	*buf,
    പൂർണ്ണസംഖ്യ		action,
    പൂർണ്ണസംഖ്യ		abort_if_last,
    പൂർണ്ണസംഖ്യ		ignore_abort,
    പൂർണ്ണസംഖ്യ		set_context)
{
    പൂർണ്ണസംഖ്യ		hiding_buf;
    bufref_T	bufref;
    പൂർണ്ണസംഖ്യ		is_curwin = (curwin != ശൂന്യ && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    പൂർണ്ണസംഖ്യ		unload_buf = (action != 0);
    പൂർണ്ണസംഖ്യ		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    പൂർണ്ണസംഖ്യ		del_buf = (action == DOBUF_DEL || wipe_buf);
    പൂർണ്ണസംഖ്യ		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    പൂർണ്ണസംഖ്യ		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    എങ്കിൽ (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    എങ്കിൽ (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	എങ്കിൽ (term_job_running(buf->b_term))
	{
	    എങ്കിൽ (wipe_buf || unload_buf)
	    {
		എങ്കിൽ (!can_unload_buffer(buf))
		    തിരികെ FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (del_buf || unload_buf)
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
    എങ്കിൽ ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	തിരികെ FALSE;

    എങ്കിൽ (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	എങ്കിൽ (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    എങ്കിൽ ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	എങ്കിൽ (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    തിരികെ FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	എങ്കിൽ (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    പോകുക aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	എങ്കിൽ (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    എങ്കിൽ (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		പോകുക aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    എങ്കിൽ (abort_if_last && one_window())
		// Autocommands made this the only window.
		പോകുക aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	എങ്കിൽ (!ignore_abort && aborting())
	    തിരികെ FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    എങ്കിൽ (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    എങ്കിൽ (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    എങ്കിൽ (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    എങ്കിൽ (!hiding_buf || !unload_buf)
	തിരികെ FALSE;

    // Always remove the buffer when there is no file name.
    എങ്കിൽ (buf->b_ffname == ശൂന്യ)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    എങ്കിൽ (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	തിരികെ FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    എങ്കിൽ ((win_valid || closed_popup) && win->w_buffer == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_buffer = ശൂന്യ;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    എങ്കിൽ (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != ശൂന്യ || buf->b_next != ശൂന്യ
#എങ്കിൽ defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	എങ്കിൽ (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    എങ്കിൽ (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, വലുപ്പം(പൂർണ്ണസംഖ്യ), 50);
	    എങ്കിൽ (ga_grow(&buf_reuse, 1) == OK)
		((പൂർണ്ണസംഖ്യ *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	എങ്കിൽ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	അല്ലെങ്കിൽ
	    buf->b_sfname = ശൂന്യ;
	VIM_CLEAR(buf->b_ffname);
	എങ്കിൽ (buf->b_prev == ശൂന്യ)
	    firstbuf = buf->b_next;
	അല്ലെങ്കിൽ
	    buf->b_prev->b_next = buf->b_next;
	എങ്കിൽ (buf->b_next == ശൂന്യ)
	    lastbuf = buf->b_prev;
	അല്ലെങ്കിൽ
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (del_buf)
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
	എങ്കിൽ (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    പ്രതിജ്ഞാ(!win_valid || !is_curwin || win == curwin);
    തിരികെ TRUE;
}

/*
 * Make buffer not contain a file.
 */
    ശൂന്യം
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_വരി_എണ്ണം = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_start_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_start_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_start_bomb = FALSE;
    buf->b_ml.ml_mfp = ശൂന്യ;
    buf->b_ml.ml_flags = ML_EMPTY;		// empty buffer
#ifdef FEAT_NETBEANS_INTG
    netbeans_deleted_all_വരികൾ(buf);
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
    പൂർണ്ണസംഖ്യ
buf_freeall(buf_T *buf, പൂർണ്ണസംഖ്യ flags)
{
    പൂർണ്ണസംഖ്യ		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    പൂർണ്ണസംഖ്യ		is_curwin = (curwin != ശൂന്യ && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    എങ്കിൽ (buf->b_ml.ml_mfp != ശൂന്യ)
    {
	എങ്കിൽ (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    തിരികെ FALSE;
    }
    എങ്കിൽ ((flags & BFA_DEL) && buf->b_p_bl)
    {
	എങ്കിൽ (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    തിരികെ FALSE;
    }
    എങ്കിൽ (flags & BFA_WIPE)
    {
	എങ്കിൽ (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    തിരികെ FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    എങ്കിൽ (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    എങ്കിൽ ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	തിരികെ FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    എങ്കിൽ (buf == curbuf && !is_curbuf)
	തിരികെ FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    എങ്കിൽ (buf == curbuf && VIsual_active
#എങ്കിൽ defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    എങ്കിൽ (curwin != ശൂന്യ && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    എങ്കിൽ (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_വരി_എണ്ണം = 0;    // no lines in buffer
    എങ്കിൽ ((flags & BFA_KEEP_UNDO) == 0)
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
    തിരികെ TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    സ്ഥിതം ശൂന്യം
free_buffer(buf_T *buf)
{
    ++buf_free_എണ്ണം;
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

    എങ്കിൽ (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    അല്ലെങ്കിൽ
    {
	vim_free(buf);
	എങ്കിൽ (curbuf == buf)
	    curbuf = ശൂന്യ;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    സ്ഥിതം ശൂന്യം
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_സംഖ്യ;
    di->di_tv.v_lock = VAR_FIXED;
    di->di_tv.vval.v_സംഖ്യ = 0;

#ifdef FEAT_EVAL
    STRCPY(buf->b_ct_di.di_താക്കോൽ, "changedtick");
    (ശൂന്യം)dict_add(buf->b_vars, di);
#endif
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    സ്ഥിതം ശൂന്യം
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    ആകുമ്പോൾ (buf->b_wininfo != ശൂന്യ)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_next;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    സ്ഥിതം ശൂന്യം
free_buffer_stuff(
    buf_T	*buf,
    പൂർണ്ണസംഖ്യ		free_options)		// free options as well
{
    എങ്കിൽ (free_options)
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
    ശൂന്യം
free_wininfo(wininfo_T *wip)
{
    എങ്കിൽ (wip->wi_optset)
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
    ശൂന്യം
goto_buffer(
    exarg_T	*eap,
    പൂർണ്ണസംഖ്യ		start,
    പൂർണ്ണസംഖ്യ		dir,
    പൂർണ്ണസംഖ്യ		എണ്ണം)
{
    bufref_T	old_curbuf;
    പൂർണ്ണസംഖ്യ		save_sea = swap_exists_action;
    പൂർണ്ണസംഖ്യ		skip_help_buf;

    തിരഞ്ഞെടുപ്പ് (eap->cmdidx)
    {
	അവസ്ഥ CMD_bnext:
	അവസ്ഥ CMD_sbnext:
	അവസ്ഥ CMD_bNext:
	അവസ്ഥ CMD_bprevious:
	അവസ്ഥ CMD_sbNext:
	അവസ്ഥ CMD_sbprevious:
	    skip_help_buf = TRUE;
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    skip_help_buf = FALSE;
	    നിർത്തുക;
    }

    set_bufref(&old_curbuf, curbuf);

    എങ്കിൽ (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (ശൂന്യം)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, എണ്ണം,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    എങ്കിൽ (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#എങ്കിൽ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#എങ്കിൽ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    അല്ലെങ്കിൽ
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    ശൂന്യം
handle_swap_exists(bufref_T *old_curbuf)
{
#എങ്കിൽ defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    എങ്കിൽ (swap_exists_action == SEA_QUIT)
    {
#എങ്കിൽ defined(FEAT_EVAL)
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
	എങ്കിൽ (old_curbuf == ശൂന്യ || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(ശൂന്യ, ശൂന്യ, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	അല്ലെങ്കിൽ
	    buf = old_curbuf->br_buf;
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ old_സന്ദേശം_silent = സന്ദേശം_silent;

	    എങ്കിൽ (shortmess(SHM_FILEINFO))
		സന്ദേശം_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    സന്ദേശം_silent = old_സന്ദേശം_silent;

#ifdef FEAT_SYN_HL
	    എങ്കിൽ (old_tw != curbuf->b_p_tw)
		check_colorcolumn(ശൂന്യ, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#എങ്കിൽ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (swap_exists_action == SEA_RECOVER)
    {
#എങ്കിൽ defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Recover at ATTENTION prompt.
	സന്ദേശം_scroll = TRUE;
	ml_recover(FALSE);
	സന്ദേശം_puts("\n");	// don't overwrite the last message
	cmdline_row = സന്ദേശം_row;
	do_modelines(0);

#എങ്കിൽ defined(FEAT_EVAL)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
empty_curbuf(
    പൂർണ്ണസംഖ്യ close_others,
    പൂർണ്ണസംഖ്യ forceit,
    പൂർണ്ണസംഖ്യ action)
{
    പൂർണ്ണസംഖ്യ	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    എങ്കിൽ (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	തിരികെ FAIL;
    }

    set_bufref(&bufref, buf);
    എങ്കിൽ (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, ശൂന്യ, ശൂന്യ, ശൂന്യ, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    എങ്കിൽ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(ശൂന്യ, buf, action, FALSE, FALSE, FALSE);
    എങ്കിൽ (!close_others)
	need_fileinfo = FALSE;
    അല്ലെങ്കിൽ എങ്കിൽ (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    തിരികെ retval;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_buffer_ext(
    പൂർണ്ണസംഖ്യ		action,
    പൂർണ്ണസംഖ്യ		start,
    പൂർണ്ണസംഖ്യ		dir,		// FORWARD or BACKWARD
    പൂർണ്ണസംഖ്യ		എണ്ണം,		// buffer number or number of buffers
    പൂർണ്ണസംഖ്യ		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    പൂർണ്ണസംഖ്യ		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    തിരഞ്ഞെടുപ്പ് (start)
    {
	അവസ്ഥ DOBUF_FIRST:   buf = firstbuf; നിർത്തുക;
	അവസ്ഥ DOBUF_LAST:    buf = lastbuf;  നിർത്തുക;
	സ്ഥിരസ്ഥിതി:	    buf = curbuf;   നിർത്തുക;
    }
    എങ്കിൽ (start == DOBUF_MOD)	    // find next modified buffer
    {
	ആകുമ്പോൾ (എണ്ണം-- > 0)
	{
	    ചെയ്യുക
	    {
		buf = buf->b_next;
		എങ്കിൽ (buf == ശൂന്യ)
		    buf = firstbuf;
	    }
	    ആകുമ്പോൾ (buf != curbuf && !bufIsChanged(buf));
	}
	എങ്കിൽ (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    തിരികെ FAIL;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (start == DOBUF_FIRST && എണ്ണം) // find specified buffer number
    {
	ആകുമ്പോൾ (buf != ശൂന്യ && buf->b_fnum != എണ്ണം)
	    buf = buf->b_next;
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = ശൂന്യ;
	ആകുമ്പോൾ (എണ്ണം > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    എങ്കിൽ (bp == ശൂന്യ)
		bp = buf;
	    എങ്കിൽ (dir == FORWARD)
	    {
		buf = buf->b_next;
		എങ്കിൽ (buf == ശൂന്യ)
		    buf = firstbuf;
	    }
	    അല്ലെങ്കിൽ
	    {
		buf = buf->b_prev;
		എങ്കിൽ (buf == ശൂന്യ)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    എങ്കിൽ (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--എണ്ണം;
		bp = ശൂന്യ;	// use this buffer as new starting point
	    }
	    എങ്കിൽ (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		തിരികെ FAIL;
	    }
	}
    }

    എങ്കിൽ (buf == ശൂന്യ)	    // could not find it
    {
	എങ്കിൽ (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    എങ്കിൽ (!unload)
		semsg(_(e_buffer_nr_does_not_exist), എണ്ണം);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	അല്ലെങ്കിൽ
	    emsg(_(e_cannot_go_before_first_buffer));
	തിരികെ FAIL;
    }
#ifdef FEAT_PROP_POPUP
    എങ്കിൽ ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	തിരികെ OK;
#endif
    എങ്കിൽ (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	തിരികെ FAIL;

    എങ്കിൽ ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), എണ്ണം);
	തിരികെ FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    എങ്കിൽ (unload)
    {
	പൂർണ്ണസംഖ്യ	forward;
	bufref_T bufref;

	എങ്കിൽ (!can_unload_buffer(buf))
	    തിരികെ FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	എങ്കിൽ (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == ശൂന്യ && !buf->b_p_bl)
	    തിരികെ FAIL;

	എങ്കിൽ ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    എങ്കിൽ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		എങ്കിൽ (term_job_running(buf->b_term))
		{
		    എങ്കിൽ (term_confirm_stop(buf) == FAIL)
			തിരികെ FAIL;
		}
		അല്ലെങ്കിൽ
# endif
		{
		    dialog_changed(buf, FALSE);
		    എങ്കിൽ (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			തിരികെ FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    എങ്കിൽ (bufIsChanged(buf))
			തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
		no_write_message_buf(buf);
		തിരികെ FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	എങ്കിൽ (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    എങ്കിൽ (bp->b_p_bl && bp != buf)
		നിർത്തുക;
	എങ്കിൽ (bp == ശൂന്യ && buf == curbuf)
	    തിരികെ empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	ആകുമ്പോൾ (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != ശൂന്യ))
	{
	    എങ്കിൽ (win_close(curwin, FALSE) == FAIL)
		നിർത്തുക;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	എങ്കിൽ (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    എങ്കിൽ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(ശൂന്യ, buf, action, FALSE, FALSE, FALSE);
	    തിരികെ OK;
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
	buf = ശൂന്യ;	// selected buffer
	bp = ശൂന്യ;	// used when no loaded buffer found
	എങ്കിൽ (au_new_curbuf.br_buf != ശൂന്യ && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	അല്ലെങ്കിൽ എങ്കിൽ (curwin->w_jumplistlen > 0)
	{
	    പൂർണ്ണസംഖ്യ     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    എങ്കിൽ (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    ആകുമ്പോൾ (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		എങ്കിൽ (buf != ശൂന്യ)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    എങ്കിൽ (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = ശൂന്യ;
		    അല്ലെങ്കിൽ എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ)
		    {
			// skip unloaded buf, but may keep it for later
			എങ്കിൽ (bp == ശൂന്യ)
			    bp = buf;
			buf = ശൂന്യ;
		    }
		}
		എങ്കിൽ (buf != ശൂന്യ)   // found a valid buffer: stop searching
		    നിർത്തുക;
		// advance to older entry in jump list
		എങ്കിൽ (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    നിർത്തുക;
		എങ്കിൽ (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		എങ്കിൽ (jumpidx == forward)		// List exhausted for sure
		    നിർത്തുക;
	    }
	}

	എങ്കിൽ (buf == ശൂന്യ)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    വേണ്ടി (;;)
	    {
		എങ്കിൽ (buf == ശൂന്യ)
		{
		    എങ്കിൽ (!forward)	// tried both directions
			നിർത്തുക;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    തുടരുക;
		}
		// in non-help buffer, try to skip help buffers, and vv
		എങ്കിൽ (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    എങ്കിൽ (buf->b_ml.ml_mfp != ശൂന്യ)   // found loaded buffer
			നിർത്തുക;
		    എങ്കിൽ (bp == ശൂന്യ)	// remember unloaded buf for later
			bp = buf;
		}
		എങ്കിൽ (forward)
		    buf = buf->b_next;
		അല്ലെങ്കിൽ
		    buf = buf->b_prev;
	    }
	}
	എങ്കിൽ (buf == ശൂന്യ)	// No loaded buffer, use unloaded one
	    buf = bp;
	എങ്കിൽ (buf == ശൂന്യ)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		എങ്കിൽ (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    നിർത്തുക;
	}
	എങ്കിൽ (buf == ശൂന്യ)	// Still no buffer, just take one
	{
	    എങ്കിൽ (curbuf->b_next != ശൂന്യ)
		buf = curbuf->b_next;
	    അല്ലെങ്കിൽ
		buf = curbuf->b_prev;
	    എങ്കിൽ (bt_quickfix(buf)
		    || (buf != ശൂന്യ && buf != curbuf && buf->b_locked_split))
		buf = ശൂന്യ;
	}
    }

    എങ്കിൽ (buf == ശൂന്യ)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	തിരികെ empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    എങ്കിൽ (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != ശൂന്യ)
	തിരികെ OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    എങ്കിൽ (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	തിരികെ FAIL;
    }
    എങ്കിൽ (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	തിരികെ FAIL;

    // go to current buffer - nothing to do
    എങ്കിൽ (buf == curbuf)
	തിരികെ OK;

    // Check if the current buffer may be abandoned.
    എങ്കിൽ (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	എങ്കിൽ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    എങ്കിൽ (term_job_running(curbuf->b_term))
	    {
		എങ്കിൽ (term_confirm_stop(curbuf) == FAIL)
		    തിരികെ FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    അല്ലെങ്കിൽ
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		എങ്കിൽ (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    തിരികെ FAIL;

		എങ്കിൽ (bufIsChanged(curbuf))
		{
		    no_write_message();
		    തിരികെ FAIL;
		}
	    }
	}
	അല്ലെങ്കിൽ
#endif
	{
	    no_write_message();
	    തിരികെ FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    എങ്കിൽ (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#എങ്കിൽ defined(FEAT_EVAL)
    എങ്കിൽ (aborting())	    // autocmds may abort script processing
	തിരികെ FAIL;
#endif

    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
do_buffer(
    പൂർണ്ണസംഖ്യ		action,
    പൂർണ്ണസംഖ്യ		start,
    പൂർണ്ണസംഖ്യ		dir,		// FORWARD or BACKWARD
    പൂർണ്ണസംഖ്യ		എണ്ണം,		// buffer number or number of buffers
    പൂർണ്ണസംഖ്യ		forceit)	// TRUE when using !
{
    തിരികെ do_buffer_ext(action, start, dir, എണ്ണം,
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
    അക്ഷരം *
do_bufdel(
    പൂർണ്ണസംഖ്യ		command,
    char_u	*arg,		// pointer to extra arguments
    പൂർണ്ണസംഖ്യ		addr_എണ്ണം,
    പൂർണ്ണസംഖ്യ		start_bnr,	// first buffer number in a range
    പൂർണ്ണസംഖ്യ		end_bnr,	// buffer nr or last buffer nr in a range
    പൂർണ്ണസംഖ്യ		forceit)
{
    പൂർണ്ണസംഖ്യ		do_current = 0;	// delete current buffer?
    പൂർണ്ണസംഖ്യ		deleted = 0;	// number of buffers deleted
    അക്ഷരം	*errormsg = ശൂന്യ; // return value
    പൂർണ്ണസംഖ്യ		bnr;		// buffer number
    char_u	*p;

    എങ്കിൽ (addr_എണ്ണം == 0)
    {
	(ശൂന്യം)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (addr_എണ്ണം == 2)
	{
	    എങ്കിൽ (*arg)		// both range and argument is not allowed
		തിരികെ ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	അല്ലെങ്കിൽ	// addr_count == 1
	    bnr = end_bnr;

	വേണ്ടി ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    എങ്കിൽ (bnr == curbuf->b_fnum)
		do_current = bnr;
	    അല്ലെങ്കിൽ എങ്കിൽ (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    എങ്കിൽ (addr_എണ്ണം == 2)
	    {
		എങ്കിൽ (++bnr > end_bnr)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ    // addr_count == 1
	    {
		arg = skipwhite(arg);
		എങ്കിൽ (*arg == NUL)
		    നിർത്തുക;
		എങ്കിൽ (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    എങ്കിൽ (bnr < 0)	    // failed
			നിർത്തുക;
		    arg = p;
		}
		അല്ലെങ്കിൽ
		    bnr = getdigits(&arg);
	    }
	}
	എങ്കിൽ (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	എങ്കിൽ (deleted == 0)
	{
	    എങ്കിൽ (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    അല്ലെങ്കിൽ എങ്കിൽ (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    അല്ലെങ്കിൽ
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (അക്ഷരം *)IObuff;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (deleted >= p_report)
	{
	    എങ്കിൽ (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    അല്ലെങ്കിൽ എങ്കിൽ (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    അല്ലെങ്കിൽ
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    തിരികെ errormsg;
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
    ശൂന്യം
set_curbuf(buf_T *buf, പൂർണ്ണസംഖ്യ action)
{
    buf_T	*prevbuf;
    പൂർണ്ണസംഖ്യ		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    പൂർണ്ണസംഖ്യ		valid;

    setpcmark();
    എങ്കിൽ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    എങ്കിൽ (!apply_autocmds(EVENT_BUFLEAVE, ശൂന്യ, ശൂന്യ, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	എങ്കിൽ (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	എങ്കിൽ (unload)
	    close_windows(prevbuf, FALSE);
#എങ്കിൽ defined(FEAT_EVAL)
	എങ്കിൽ (bufref_valid(&prevbufref) && !aborting())
#അല്ലെങ്കിൽ
	എങ്കിൽ (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    എങ്കിൽ (prevbuf == curbuf
		    && ((State & MODE_തിരുകുക) == 0 || curbuf->b_nwindows <= 1))
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
    എങ്കിൽ ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == ശൂന്യ)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	എങ്കിൽ (!valid)
	    enter_buffer(lastbuf);
	അല്ലെങ്കിൽ
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	എങ്കിൽ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ശൂന്യ, curwin);
#endif
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    സ്ഥിതം ശൂന്യം
enter_buffer(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    എങ്കിൽ (VIsual_active
#എങ്കിൽ defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    എങ്കിൽ (curwin->w_buffer != ശൂന്യ)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    എങ്കിൽ (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    അല്ലെങ്കിൽ
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    എങ്കിൽ (curwin->w_p_diff)
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
    എങ്കിൽ (curbuf->b_ml.ml_mfp == ശൂന്യ)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	എങ്കിൽ (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, ശൂന്യ, 0);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!സന്ദേശം_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(ശൂന്യം)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
	apply_autocmds(EVENT_BUFENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    എങ്കിൽ (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    എങ്കിൽ (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    എങ്കിൽ (curbuf->b_kmap_state & KEYMAP_INIT)
	(ശൂന്യം)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    എങ്കിൽ (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(ശൂന്യം)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#എങ്കിൽ defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    ശൂന്യം
do_autochdir(ശൂന്യം)
{
    എങ്കിൽ ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != ശൂന്യ
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	last_chdir_reason = "autochdir";
    }
}
#endif

    ശൂന്യം
no_write_message_buf(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    എങ്കിൽ (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    അല്ലെങ്കിൽ
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    ശൂന്യം
no_write_message(ശൂന്യം)
{
#ifdef FEAT_TERMINAL
    എങ്കിൽ (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    അല്ലെങ്കിൽ
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    ശൂന്യം
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    എങ്കിൽ (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    അല്ലെങ്കിൽ
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
    പൂർണ്ണസംഖ്യ
curbuf_reusable(ശൂന്യം)
{
    തിരികെ (curbuf != ശൂന്യ
	&& curbuf->b_ffname == ശൂന്യ
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == ശൂന്യ || BUFEMPTY())
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
    പൂർണ്ണസംഖ്യ		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    എങ്കിൽ (മുകൾ_file_അക്കം == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    എങ്കിൽ (sfname == ശൂന്യ || mch_stat((അക്ഷരം *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    എങ്കിൽ (ffname != ശൂന്യ && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#അല്ലെങ്കിൽ
		buflist_findname(ffname)
#endif
		) != ശൂന്യ)
    {
	vim_free(ffname);
	എങ്കിൽ (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? ശൂന്യ : curwin,
						      lnum, (colnr_T)0, FALSE);

	എങ്കിൽ ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	എങ്കിൽ ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    എങ്കിൽ (!(flags & BLN_DUMMY))
	    {
		എങ്കിൽ (apply_autocmds(EVENT_BUFADD, ശൂന്യ, ശൂന്യ, FALSE, buf)
			&& !bufref_valid(&bufref))
		    തിരികെ ശൂന്യ;
	    }
	}
	തിരികെ buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = ശൂന്യ;
    എങ്കിൽ ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	എങ്കിൽ (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    തിരികെ ശൂന്യ;
	}
#endif
	എങ്കിൽ (!bufref_valid(&bufref))
	    buf = ശൂന്യ;		// buf was deleted; allocate a new buffer
    }
    എങ്കിൽ (buf != curbuf || curbuf == ശൂന്യ)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	എങ്കിൽ (buf == ശൂന്യ)
	{
	    vim_free(ffname);
	    തിരികെ ശൂന്യ;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	എങ്കിൽ (buf->b_vars == ശൂന്യ)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    തിരികെ ശൂന്യ;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    എങ്കിൽ (ffname != ശൂന്യ)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    എങ്കിൽ ((ffname != ശൂന്യ && (buf->b_ffname == ശൂന്യ || buf->b_sfname == ശൂന്യ))
	    || buf->b_wininfo == ശൂന്യ)
    {
	എങ്കിൽ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	അല്ലെങ്കിൽ
	    buf->b_sfname = ശൂന്യ;
	VIM_CLEAR(buf->b_ffname);
	എങ്കിൽ (buf != curbuf)
	    free_buffer(buf);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (buf == curbuf)
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
    അല്ലെങ്കിൽ
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = ശൂന്യ;
	എങ്കിൽ (firstbuf == ശൂന്യ)		// buffer list is empty
	{
	    buf->b_prev = ശൂന്യ;
	    firstbuf = buf;
	}
	അല്ലെങ്കിൽ				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	എങ്കിൽ ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((പൂർണ്ണസംഖ്യ *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    ആകുമ്പോൾ (buf->b_prev != ശൂന്യ && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		എങ്കിൽ (prev->b_next != ശൂന്യ)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		എങ്കിൽ (buf->b_prev != ശൂന്യ)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		എങ്കിൽ (lastbuf == buf)
		    lastbuf = prev;
		എങ്കിൽ (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	അല്ലെങ്കിൽ
	    buf->b_fnum = മുകൾ_file_അക്കം++;
	എങ്കിൽ (മുകൾ_file_അക്കം < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    എങ്കിൽ (emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    മുകൾ_file_അക്കം = 1;
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
    എങ്കിൽ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    അല്ലെങ്കിൽ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    എങ്കിൽ (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    എങ്കിൽ (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	എങ്കിൽ (apply_autocmds(EVENT_BUFNEW, ശൂന്യ, ശൂന്യ, FALSE, buf)
		&& !bufref_valid(&bufref))
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (flags & BLN_LISTED)
	{
	    എങ്കിൽ (apply_autocmds(EVENT_BUFADD, ശൂന്യ, ശൂന്യ, FALSE, buf)
		    && !bufref_valid(&bufref))
		തിരികെ ശൂന്യ;
	}
#ifdef FEAT_EVAL
	എങ്കിൽ (aborting())		// autocmds may abort script processing
	    തിരികെ ശൂന്യ;
#endif
    }

    തിരികെ buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    ശൂന്യം
free_buf_options(
    buf_T	*buf,
    പൂർണ്ണസംഖ്യ		free_p_ff)
{
    എങ്കിൽ (free_p_ff)
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
#എങ്കിൽ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#എങ്കിൽ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#എങ്കിൽ defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#എങ്കിൽ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    എങ്കിൽ (buf->b_p_താക്കോൽ != ശൂന്യ && *buf->b_p_താക്കോൽ != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_താക്കോൽ, STRLEN(buf->b_p_താക്കോൽ));
# endif
    clear_string_option(&buf->b_p_താക്കോൽ);
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
    buf->b_s.b_cap_prog = ശൂന്യ;
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
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_എണ്ണം);
    buf->b_p_cpt_എണ്ണം = 0;
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
    പൂർണ്ണസംഖ്യ
buflist_getfile(
    പൂർണ്ണസംഖ്യ		n,
    linenr_T	lnum,
    പൂർണ്ണസംഖ്യ		options,
    പൂർണ്ണസംഖ്യ		forceit)
{
    buf_T	*buf;
    win_T	*wp = ശൂന്യ;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    എങ്കിൽ (buf == ശൂന്യ)
    {
	എങ്കിൽ ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	അല്ലെങ്കിൽ
	    semsg(_(e_buffer_nr_not_found), n);
	തിരികെ FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    എങ്കിൽ (buf == curbuf)
	തിരികെ OK;

    എങ്കിൽ (text_or_buf_locked())
	തിരികെ FAIL;

    // altfpos may be changed by getfile(), get it now
    എങ്കിൽ (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    അല്ലെങ്കിൽ
	col = 0;

    എങ്കിൽ (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	എങ്കിൽ (wp == ശൂന്യ && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    എങ്കിൽ (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    അല്ലെങ്കിൽ എങ്കിൽ (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		തിരികെ FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    പൂർണ്ണസംഖ്യ retval = FAIL;
    എങ്കിൽ (GETFILE_SUCCESS(getfile(buf->b_fnum, ശൂന്യ, ശൂന്യ,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	എങ്കിൽ (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    എങ്കിൽ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    തിരികെ retval;
}

/*
 * go to the last know line number for the current buffer
 */
    സ്ഥിതം ശൂന്യം
buflist_getfpos(ശൂന്യം)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    എങ്കിൽ (p_sol)
	curwin->w_cursor.col = 0;
    അല്ലെങ്കിൽ
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
    buf_T	*buf = ശൂന്യ;

    // First make the name into a full path name
    ffname = FullName_save(fname,
#ifdef UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#അല്ലെങ്കിൽ
	    FALSE
#endif
	    );
    എങ്കിൽ (ffname != ശൂന്യ)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    തിരികെ buf;
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

    എങ്കിൽ (mch_stat((അക്ഷരം *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    തിരികെ buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    സ്ഥിതം buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#endif
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_LAST(buf)
	എങ്കിൽ ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    തിരികെ buf;
    തിരികെ ശൂന്യ;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    പൂർണ്ണസംഖ്യ
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    പൂർണ്ണസംഖ്യ		unlisted,	// find unlisted buffers
    പൂർണ്ണസംഖ്യ		diffmode UNUSED, // find diff-mode buffers only
    പൂർണ്ണസംഖ്യ		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		match = -1;
    പൂർണ്ണസംഖ്യ		find_listed;
    char_u	*pat;
    char_u	*patend;
    പൂർണ്ണസംഖ്യ		attempt;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    എങ്കിൽ ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	എങ്കിൽ (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	അല്ലെങ്കിൽ
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	എങ്കിൽ (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
    അല്ലെങ്കിൽ
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, ശൂന്യ, FALSE);
	എങ്കിൽ (pat == ശൂന്യ)
	    തിരികെ -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	വേണ്ടി (;;)
	{
	    വേണ്ടി (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		എങ്കിൽ (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		എങ്കിൽ (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    എങ്കിൽ (regmatch.regprog == ശൂന്യ)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			തിരികെ -1;
		    }
		    എങ്കിൽ (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != ശൂന്യ)
		    {
			എങ്കിൽ (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				എങ്കിൽ (wp->w_buffer == buf)
				    നിർത്തുക;
			    എങ്കിൽ (wp == ശൂന്യ)
				തുടരുക;
			}
			എങ്കിൽ (match >= 0)		// already found a match
			{
			    match = -2;
			    നിർത്തുക;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		എങ്കിൽ (match >= 0)			// found one match
		    നിർത്തുക;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    എങ്കിൽ (!unlisted || !find_listed || match != -1)
		നിർത്തുക;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    എങ്കിൽ (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    അല്ലെങ്കിൽ എങ്കിൽ (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    തിരികെ match;
}

#ifdef FEAT_VIMINFO
തരനിർവചനം ഘടന {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    പൂർണ്ണസംഖ്യ
ExpandBufnames(
    char_u	*pat,
    പൂർണ്ണസംഖ്യ		*അക്കം_file,
    char_u	***file,
    പൂർണ്ണസംഖ്യ		options)
{
    പൂർണ്ണസംഖ്യ		എണ്ണം;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		round;
    char_u	*p;
    char_u	*patc = ശൂന്യ;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = ശൂന്യ;
#endif
    പൂർണ്ണസംഖ്യ		fuzzy;
    fuzmatch_str_T  *fuzmatch = ശൂന്യ;
    regmatch_T	regmatch;
    പൂർണ്ണസംഖ്യ		score = 0;
    പൂർണ്ണസംഖ്യ		to_free = FALSE;

    *അക്കം_file = 0;		    // return values in case of FAIL
    *file = ശൂന്യ;

#ifdef FEAT_DIFF
    എങ്കിൽ ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	തിരികെ FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    എങ്കിൽ (!fuzzy)
    {
	എങ്കിൽ (*pat == '^' && pat[1] != NUL)
	{
	    പൂർണ്ണസംഖ്യ  len = (പൂർണ്ണസംഖ്യ)STRLEN(pat);
	    patc = alloc(len);
	    എങ്കിൽ (patc == ശൂന്യ)
		തിരികെ FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*pat == '^')
	    patc = (char_u *)"";
	അല്ലെങ്കിൽ
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    വേണ്ടി (round = 1; round <= 2; ++round)
    {
	എണ്ണം = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    എങ്കിൽ (!buf->b_p_bl)	// skip unlisted buffers
		തുടരുക;
#ifdef FEAT_DIFF
	    എങ്കിൽ (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		എങ്കിൽ (buf == curbuf || !diff_mode_buf(buf))
		    തുടരുക;
#endif

	    എങ്കിൽ (!fuzzy)
	    {
		എങ്കിൽ (regmatch.regprog == ശൂന്യ)
		{
		    // invalid pattern, possibly after recompiling
		    എങ്കിൽ (to_free)
			vim_free(patc);
		    തിരികെ FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    അല്ലെങ്കിൽ
	    {
		p = ശൂന്യ;
		// first try matching with the short file name
		എങ്കിൽ ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		എങ്കിൽ (p == ശൂന്യ)
		{
		    // next try matching with the full path file name
		    എങ്കിൽ ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    എങ്കിൽ (p == ശൂന്യ)
		തുടരുക;

	    എങ്കിൽ (round == 1)
	    {
		++എണ്ണം;
		തുടരുക;
	    }

	    എങ്കിൽ (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    അല്ലെങ്കിൽ
		p = vim_strsave(p);
	    എങ്കിൽ (p == ശൂന്യ)
	    {
		എങ്കിൽ (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, എണ്ണം);
		തിരികെ FAIL;
	    }

	    എങ്കിൽ (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		എങ്കിൽ (matches != ശൂന്യ)
		{
		    matches[എണ്ണം].buf = buf;
		    matches[എണ്ണം].match = p;
		    എണ്ണം++;
		}
		അല്ലെങ്കിൽ
#endif
		    (*file)[എണ്ണം++] = p;
	    }
	    അല്ലെങ്കിൽ
	    {
		fuzmatch[എണ്ണം].idx = എണ്ണം;
		fuzmatch[എണ്ണം].str = p;
		fuzmatch[എണ്ണം].score = score;
		എണ്ണം++;
	    }
	}
	എങ്കിൽ (എണ്ണം == 0)	// no match found, break here
	    നിർത്തുക;
	എങ്കിൽ (round == 1)
	{
	    എങ്കിൽ (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, എണ്ണം);
		എങ്കിൽ (*file == ശൂന്യ)
		{
		    vim_regfree(regmatch.regprog);
		    എങ്കിൽ (to_free)
			vim_free(patc);
		    തിരികെ FAIL;
		}
#ifdef FEAT_VIMINFO
		എങ്കിൽ (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, എണ്ണം);
#endif
	    }
	    അല്ലെങ്കിൽ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, എണ്ണം);
		എങ്കിൽ (fuzmatch == ശൂന്യ)
		{
		    *അക്കം_file = 0;
		    *file = ശൂന്യ;
		    തിരികെ FAIL;
		}
	    }
	}
    }

    എങ്കിൽ (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	എങ്കിൽ (to_free)
	    vim_free(patc);
    }

    എങ്കിൽ (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	എങ്കിൽ (matches != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ i;
	    എങ്കിൽ (എണ്ണം > 1)
		qsort(matches, എണ്ണം, വലുപ്പം(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    എങ്കിൽ (matches[0].buf == curbuf)
	    {
		വേണ്ടി (i = 1; i < എണ്ണം; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[എണ്ണം-1] = matches[0].match;
	    }
	    അല്ലെങ്കിൽ
	    {
		വേണ്ടി (i = 0; i < എണ്ണം; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (fuzzymatches_to_strmatches(fuzmatch, file, എണ്ണം, FALSE) == FAIL)
	    തിരികെ FAIL;
    }

    *അക്കം_file = എണ്ണം;
    തിരികെ (എണ്ണം == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    സ്ഥിതം char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    പൂർണ്ണസംഖ്യ		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    എങ്കിൽ (match == ശൂന്യ && rmp->regprog != ശൂന്യ)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    തിരികെ match;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    സ്ഥിതം char_u *
fname_match(
    regmatch_T	*rmp,
    char_u	*name,
    പൂർണ്ണസംഖ്യ		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = ശൂന്യ;
    char_u	*p;

    // extra check for valid arguments
    എങ്കിൽ (name == ശൂന്യ || rmp->regprog == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    എങ്കിൽ (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    അല്ലെങ്കിൽ എങ്കിൽ (rmp->regprog != ശൂന്യ)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(ശൂന്യ, name);
	എങ്കിൽ (p != ശൂന്യ && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    തിരികെ match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(പൂർണ്ണസംഖ്യ nr)
{
    char_u	താക്കോൽ[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    എങ്കിൽ (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((അക്ഷരം *)താക്കോൽ, വലുപ്പം(താക്കോൽ), "%x", nr);
    hi = hash_find(&buf_hashtab, താക്കോൽ);

    എങ്കിൽ (!HASHITEM_EMPTY(hi))
	തിരികെ (buf_T *)(hi->hi_താക്കോൽ
			     - ((unsigned)(curbuf->b_താക്കോൽ - (char_u *)curbuf)));
    തിരികെ ശൂന്യ;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    പൂർണ്ണസംഖ്യ		n,
    പൂർണ്ണസംഖ്യ		fullname,
    പൂർണ്ണസംഖ്യ		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ home_replace_save(helptail ? buf : ശൂന്യ,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    ശൂന്യം
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	col,
    പൂർണ്ണസംഖ്യ		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	എങ്കിൽ (wip->wi_win == win)
	    നിർത്തുക;
    എങ്കിൽ (wip == ശൂന്യ)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	എങ്കിൽ (wip == ശൂന്യ)
	    തിരികെ;
	wip->wi_win = win;
	എങ്കിൽ (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    അല്ലെങ്കിൽ
    {
	// remove the entry from the list
	എങ്കിൽ (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	അല്ലെങ്കിൽ
	    buf->b_wininfo = wip->wi_next;
	എങ്കിൽ (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	എങ്കിൽ (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    എങ്കിൽ (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    എങ്കിൽ (win != ശൂന്യ)
	wip->wi_changelistidx = win->w_changelistidx;
    എങ്കിൽ (copy_options && win != ശൂന്യ)
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
    wip->wi_prev = ശൂന്യ;
    എങ്കിൽ (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    എങ്കിൽ (!wip->wi_opt.wo_diff)
	തിരികെ FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	എങ്കിൽ (wip->wi_win == wp)
	    തിരികെ FALSE;
    തിരികെ TRUE;
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
    സ്ഥിതം wininfo_T *
find_wininfo(
    buf_T	*buf,
    പൂർണ്ണസംഖ്യ		need_options,
    പൂർണ്ണസംഖ്യ		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	എങ്കിൽ (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    നിർത്തുക;

    എങ്കിൽ (wip != ശൂന്യ)
	തിരികെ wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    എങ്കിൽ (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    എങ്കിൽ (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != ശൂന്യ
			    && wip->wi_win->w_buffer == buf)))
		നിർത്തുക;
    }
    അല്ലെങ്കിൽ
#endif
	wip = buf->b_wininfo;
    തിരികെ wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    ശൂന്യം
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
    clearFolding(curwin);
#endif

    wip = find_wininfo(buf, TRUE, TRUE);
    എങ്കിൽ (wip != ശൂന്യ && wip->wi_win != ശൂന്യ
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
    അല്ലെങ്കിൽ എങ്കിൽ (wip != ശൂന്യ && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    അല്ലെങ്കിൽ
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    എങ്കിൽ (wip != ശൂന്യ)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    എങ്കിൽ (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    എങ്കിൽ (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    എങ്കിൽ (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	എങ്കിൽ (p_ea)
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
    സ്ഥിതം pos_T no_position = {1, 0, 0};

    wip = find_wininfo(buf, FALSE, FALSE);
    എങ്കിൽ (wip != ശൂന്യ)
	തിരികെ &(wip->wi_fpos);
    അല്ലെങ്കിൽ
	തിരികെ &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    തിരികെ buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    ശൂന്യം
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		ro_char;
    പൂർണ്ണസംഖ്യ		changed_char;
#ifdef FEAT_TERMINAL
    പൂർണ്ണസംഖ്യ		job_running;
    പൂർണ്ണസംഖ്യ		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = ശൂന്യ, **p;

    എങ്കിൽ (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, വലുപ്പം(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    എങ്കിൽ (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_data, (size_t)buflist.ga_len,
		വലുപ്പം(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    വേണ്ടി (; buf != ശൂന്യ && !got_int; buf = buflist_data != ശൂന്യ
	    ? (++p < buflist_data + buflist.ga_len ? *p : ശൂന്യ)
	    : buf->b_next)
#അല്ലെങ്കിൽ
    വേണ്ടി (buf = firstbuf; buf != ശൂന്യ && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	എങ്കിൽ ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !bufIsChanged(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == ശൂന്യ || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == ശൂന്യ || buf->b_nwindows != 0))
#ifdef FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!job_running || (job_running && job_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!job_running || (job_running && !job_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (job_running || buf->b_term == ശൂന്യ))
#endif
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    തുടരുക;
	name = buf_spname(buf);
	എങ്കിൽ (name != ശൂന്യ)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	അല്ലെങ്കിൽ
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	എങ്കിൽ (message_filtered(NameBuff))
	    തുടരുക;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	എങ്കിൽ (job_running)
	{
	    എങ്കിൽ (job_none_open)
		ro_char = '?';
	    അല്ലെങ്കിൽ
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	അല്ലെങ്കിൽ എങ്കിൽ (buf->b_term != ശൂന്യ)
	    ro_char = 'F';
	അല്ലെങ്കിൽ
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	സന്ദേശം_putchar('\n');
	len = (പൂർണ്ണസംഖ്യ)vim_snprintf_safelen((അക്ഷരം *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == ശൂന്യ ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		changed_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	ചെയ്യുക
	    IObuff[len++] = ' ';
	ആകുമ്പോൾ (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	എങ്കിൽ (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (size_t)(IOSIZE - len), buf->b_last_used);
	അല്ലെങ്കിൽ
#endif
	    vim_snprintf((അക്ഷരം *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (long)buflist_findlnum(buf));
	സന്ദേശം_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    എങ്കിൽ (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    പൂർണ്ണസംഖ്യ
buflist_name_nr(
    പൂർണ്ണസംഖ്യ		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    എങ്കിൽ (buf == ശൂന്യ || buf->b_fname == ശൂന്യ)
	തിരികെ FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    തിരികെ OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    പൂർണ്ണസംഖ്യ		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = ശൂന്യ;
#ifdef UNIX
    stat_T	st;
#endif

    എങ്കിൽ (ffname == ശൂന്യ || *ffname == NUL)
    {
	// Removing the name.
	എങ്കിൽ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	അല്ലെങ്കിൽ
	    buf->b_sfname = ശൂന്യ;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    അല്ലെങ്കിൽ
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	എങ്കിൽ (ffname == ശൂന്യ)		    // out of memory
	    തിരികെ FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	എങ്കിൽ (mch_stat((അക്ഷരം *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	എങ്കിൽ (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#അല്ലെങ്കിൽ
	    obuf = buflist_findname(ffname);
#endif
	എങ്കിൽ (obuf != ശൂന്യ && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    പൂർണ്ണസംഖ്യ		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		എങ്കിൽ (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    എങ്കിൽ (obuf->b_ml.ml_mfp != ശൂന്യ || in_use)
	    {
		എങ്കിൽ (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		തിരികെ FAIL;
	    }
	    // delete from the list
	    close_buffer(ശൂന്യ, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	എങ്കിൽ (ffname == ശൂന്യ || sfname == ശൂന്യ)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    തിരികെ FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	എങ്കിൽ (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    എങ്കിൽ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    അല്ലെങ്കിൽ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    തിരികെ OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    ശൂന്യം
buf_set_name(പൂർണ്ണസംഖ്യ fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = ശൂന്യ;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    ശൂന്യം
buf_name_changed(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    എങ്കിൽ (buf->b_ml.ml_mfp != ശൂന്യ)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    എങ്കിൽ (buf->b_term != ശൂന്യ)
	term_clear_status_text(buf->b_term);
#endif

    എങ്കിൽ (curwin->w_buffer == buf)
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
    എങ്കിൽ (buf != ശൂന്യ && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    തിരികെ buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    പൂർണ്ണസംഖ്യ		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    എങ്കിൽ (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	എങ്കിൽ (errmsg)
	    emsg(_(e_no_alternate_file));
	തിരികെ ശൂന്യ;
    }
    തിരികെ fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    പൂർണ്ണസംഖ്യ
buflist_add(char_u *fname, പൂർണ്ണസംഖ്യ flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, ശൂന്യ, (linenr_T)0, flags);
    എങ്കിൽ (buf != ശൂന്യ)
	തിരികെ buf->b_fnum;
    തിരികെ 0;
}

#എങ്കിൽ defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    ശൂന്യം
buflist_slash_adjust(ശൂന്യം)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	എങ്കിൽ (bp->b_ffname != ശൂന്യ)
	    slash_adjust(bp->b_ffname);
	എങ്കിൽ (bp->b_sfname != ശൂന്യ)
	    slash_adjust(bp->b_sfname);
    }
}
#endif

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    ശൂന്യം
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.col, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    പൂർണ്ണസംഖ്യ
otherfile(char_u *ffname)
{
    തിരികെ otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , ശൂന്യ
#endif
	    );
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    എങ്കിൽ (ffname == ശൂന്യ || *ffname == NUL || buf->b_ffname == ശൂന്യ)
	തിരികെ TRUE;
    എങ്കിൽ (fnamecmp(ffname, buf->b_ffname) == 0)
	തിരികെ FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	എങ്കിൽ (stp == ശൂന്യ)
	{
	    എങ്കിൽ (!buf->b_dev_valid || mch_stat((അക്ഷരം *)ffname, &st) < 0)
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
	എങ്കിൽ (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    എങ്കിൽ (buf_same_ino(buf, stp))
		തിരികെ FALSE;
	}
    }
#endif
    തിരികെ TRUE;
}

#എങ്കിൽ defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    ശൂന്യം
buf_setino(buf_T *buf)
{
    stat_T	st;

    എങ്കിൽ (buf->b_fname != ശൂന്യ && mch_stat((അക്ഷരം *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    അല്ലെങ്കിൽ
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    തിരികെ (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    ശൂന്യം
fileinfo(
    പൂർണ്ണസംഖ്യ fullname,	    // when non-zero print full path
    പൂർണ്ണസംഖ്യ shorthelp,
    പൂർണ്ണസംഖ്യ	dont_truncate)
{
    char_u	*name;
    അക്ഷരം	*buffer;
    size_t	bufferlen = 0;

    buffer = alloc(IOSIZE);
    എങ്കിൽ (buffer == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    എങ്കിൽ (name != ശൂന്യ)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!fullname && curbuf->b_fname != ശൂന്യ)
	    name = curbuf->b_fname;
	അല്ലെങ്കിൽ
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : ശൂന്യ, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (പൂർണ്ണസംഖ്യ)bufferlen, TRUE);
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

    എങ്കിൽ (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_വരികൾ_സന്ദേശം));
    അല്ലെങ്കിൽ എങ്കിൽ (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_വരി_എണ്ണം),
	    (long)curbuf->b_ml.ml_വരി_എണ്ണം,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_വരി_എണ്ണം));
    അല്ലെങ്കിൽ
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (long)curwin->w_cursor.lnum,
	    (long)curbuf->b_ml.ml_വരി_എണ്ണം,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_വരി_എണ്ണം));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (പൂർണ്ണസംഖ്യ)curwin->w_cursor.col + 1, (പൂർണ്ണസംഖ്യ)curwin->w_virtcol + 1);
    }

    (ശൂന്യം)append_arg_സംഖ്യ(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    എങ്കിൽ (dont_truncate)
    {
	പൂർണ്ണസംഖ്യ	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	സന്ദേശം_start();
	n = സന്ദേശം_scroll;
	സന്ദേശം_scroll = TRUE;
	സന്ദേശം(buffer);
	സന്ദേശം_scroll = n;
    }
    അല്ലെങ്കിൽ
    {
	അക്ഷരം	*p = സന്ദേശം_trunc_attr(buffer, FALSE, 0);
	എങ്കിൽ (restart_edit != 0 || (സന്ദേശം_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_സന്ദേശം((char_u *)p, 0);
    }

    vim_free(buffer);
}

    പൂർണ്ണസംഖ്യ
col_print(
    char_u  *buf,
    size_t  buflen,
    പൂർണ്ണസംഖ്യ	    col,
    പൂർണ്ണസംഖ്യ	    vcol)
{
    എങ്കിൽ (col == vcol)
	തിരികെ (പൂർണ്ണസംഖ്യ)vim_snprintf_safelen((അക്ഷരം *)buf, buflen, "%d", col);

    തിരികെ (പൂർണ്ണസംഖ്യ)vim_snprintf_safelen((അക്ഷരം *)buf, buflen, "%d-%d", col, vcol);
}

സ്ഥിതം char_u *lasttitle = ശൂന്യ;
സ്ഥിതം char_u *lasticon = ശൂന്യ;

/*
 * Put the file name in the title bar and icon of the window.
 */
    ശൂന്യം
maketitle(ശൂന്യം)
{
    char_u	*title_str = ശൂന്യ;
    char_u	*icon_str = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    എങ്കിൽ (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	തിരികെ;
    }

    need_maketitle = FALSE;
    എങ്കിൽ (!p_title && !p_icon && lasttitle == ശൂന്യ && lasticon == ശൂന്യ)
	തിരികെ;  // nothing to do

    എങ്കിൽ (p_title)
    {
	പൂർണ്ണസംഖ്യ maxlen = 0;

	എങ്കിൽ (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    എങ്കിൽ (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	എങ്കിൽ (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    എങ്കിൽ (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, വലുപ്പം(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, ശൂന്യ, ശൂന്യ, ശൂന്യ);
	    അല്ലെങ്കിൽ
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	അല്ലെങ്കിൽ
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (വലുപ്പം(buf) - 100)
#define SPACE_FOR_DIR   (വലുപ്പം(buf) - 20)
#define SPACE_FOR_ARGNR (വലുപ്പം(buf) - 10)  // at least room for " - VIM"

	    // file name
	    എങ്കിൽ (curbuf->b_fname == ശൂന്യ)
		buflen = vim_snprintf_safelen((അക്ഷരം *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    അല്ലെങ്കിൽ എങ്കിൽ (curbuf->b_term != ശൂന്യ)
		buflen = vim_snprintf_safelen((അക്ഷരം *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    അല്ലെങ്കിൽ
	    {
		buflen = vim_snprintf_safelen((അക്ഷരം *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != ശൂന്യ)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    എങ്കിൽ (curbuf->b_term == ശൂന്യ)
#endif
	    {
		തിരഞ്ഞെടുപ്പ് (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    അവസ്ഥ 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (അക്ഷരം *)buf + buflen,
			    വലുപ്പം(buf) - buflen, " +");
			നിർത്തുക;
		    അവസ്ഥ 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (അക്ഷരം *)buf + buflen,
			    വലുപ്പം(buf) - buflen, " =");
			നിർത്തുക;
		    അവസ്ഥ 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (അക്ഷരം *)buf + buflen,
			    വലുപ്പം(buf) - buflen, " =+");
			നിർത്തുക;
		    അവസ്ഥ 4:
		    അവസ്ഥ 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (അക്ഷരം *)buf + buflen,
			    വലുപ്പം(buf) - buflen, " -");
			നിർത്തുക;
		    അവസ്ഥ 5:
		    അവസ്ഥ 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (അക്ഷരം *)buf + buflen,
			    വലുപ്പം(buf) - buflen, " -+");
			നിർത്തുക;
		    സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}
	    }

	    // path (surrounded by '()')
	    എങ്കിൽ (curbuf->b_fname != ശൂന്യ
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == ശൂന്യ
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((അക്ഷരം *)buf + buflen,
		    വലുപ്പം(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (പൂർണ്ണസംഖ്യ)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		എങ്കിൽ (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		എങ്കിൽ (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((അക്ഷരം *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		അല്ലെങ്കിൽ
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    എങ്കിൽ (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((അക്ഷരം *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != ശൂന്യ)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    അല്ലെങ്കിൽ
			buflen += vim_snprintf_safelen((അക്ഷരം *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_സംഖ്യ(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((അക്ഷരം *)buf + buflen,
		വലുപ്പം(buf) - buflen, " - %s",
#എങ്കിൽ defined(FEAT_CLIENTSERVER)
		(serverName != ശൂന്യ)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    എങ്കിൽ (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		എങ്കിൽ (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, വലുപ്പം(buf));
	    }
	}
    }
    mustset = മൂല്യം_changed(title_str, &lasttitle);

    എങ്കിൽ (p_icon)
    {
	icon_str = buf;
	എങ്കിൽ (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    എങ്കിൽ (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, വലുപ്പം(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, ശൂന്യ, ശൂന്യ,
				 ശൂന്യ);
	    അല്ലെങ്കിൽ
#endif
		icon_str = p_iconstring;
	}
	അല്ലെങ്കിൽ
	{
	    char_u  *name;
	    പൂർണ്ണസംഖ്യ	    namelen;

	    name = buf_spname(curbuf);
	    എങ്കിൽ (name == ശൂന്യ)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (പൂർണ്ണസംഖ്യ)STRLEN(name);
	    എങ്കിൽ (namelen > 100)
	    {
		namelen -= 100;
		എങ്കിൽ (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, വലുപ്പം(buf));
	}
    }

    mustset |= മൂല്യം_changed(icon_str, &lasticon);

    എങ്കിൽ (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
മൂല്യം_changed(char_u *str, char_u **last)
{
    എങ്കിൽ ((str == ശൂന്യ) != (*last == ശൂന്യ)
	    || (str != ശൂന്യ && *last != ശൂന്യ && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	എങ്കിൽ (str == ശൂന്യ)
	{
	    *last = ശൂന്യ;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	അല്ലെങ്കിൽ
	{
	    *last = vim_strsave(str);
	    തിരികെ TRUE;
	}
    }
    തിരികെ FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    ശൂന്യം
resettitle(ശൂന്യം)
{
    mch_settitle(lasttitle, lasticon);
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
free_titles(ശൂന്യം)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#എങ്കിൽ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
തരനിർവചനം ഘടന
{
    char_u	*stl_start;
    പൂർണ്ണസംഖ്യ		stl_minwid;
    പൂർണ്ണസംഖ്യ		stl_maxwid;
    എണ്ണൽ {
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

സ്ഥിതം size_t		stl_ഇനങ്ങൾ_len = 20; // Initial value, grows as needed.
സ്ഥിതം stl_item_T      *stl_ഇനങ്ങൾ = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	       *stl_groupitem = ശൂന്യ;
സ്ഥിതം stl_hlrec_T     *stl_hltab = ശൂന്യ;
സ്ഥിതം stl_hlrec_T     *stl_tabtab = ശൂന്യ;
സ്ഥിതം stl_clickrec_T  *stl_clicktab = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ		*stl_separator_locations = ശൂന്യ;

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
    പൂർണ്ണസംഖ്യ
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    പൂർണ്ണസംഖ്യ		opt_scope,	// scope for "opt_name"
    പൂർണ്ണസംഖ്യ		fillchar,
    പൂർണ്ണസംഖ്യ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    തിരികെ build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ശൂന്യ, ശൂന്യ);
}

    പൂർണ്ണസംഖ്യ
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    പൂർണ്ണസംഖ്യ		opt_scope,	// scope for "opt_name"
    പൂർണ്ണസംഖ്യ		fillchar,
    പൂർണ്ണസംഖ്യ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    പൂർണ്ണസംഖ്യ		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    തിരികെ build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ശൂന്യ, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    പൂർണ്ണസംഖ്യ
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    പൂർണ്ണസംഖ്യ		opt_scope,	// scope for "opt_name"
    പൂർണ്ണസംഖ്യ		fillchar,
    പൂർണ്ണസംഖ്യ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    പൂർണ്ണസംഖ്യ		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    തിരികെ build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ശൂന്യ, carry_hl);
}
# endif

    പൂർണ്ണസംഖ്യ
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    പൂർണ്ണസംഖ്യ		opt_scope)	// scope for "opt_name"
{
    പൂർണ്ണസംഖ്യ rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (ശൂന്യം)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, വലുപ്പം(buf), &fmt,
	    opt_name, opt_scope, 0, 0, ശൂന്യ, ശൂന്യ, ശൂന്യ, &rendered_height,
	    ശൂന്യ);
    --emsg_off;
    തിരികെ rendered_height;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    പൂർണ്ണസംഖ്യ		opt_scope,	// scope for "opt_name"
    പൂർണ്ണസംഖ്യ		fillchar,
    പൂർണ്ണസംഖ്യ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    പൂർണ്ണസംഖ്യ		*rendered_height,   // return: stl rendered height (can be NULL)
    പൂർണ്ണസംഖ്യ		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    പൂർണ്ണസംഖ്യ		byteval;
# ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		use_sandbox;
    പൂർണ്ണസംഖ്യ		save_VIsual_active;
# endif
    പൂർണ്ണസംഖ്യ		empty_വരി;
    long	l;
    long	n;
    പൂർണ്ണസംഖ്യ		prevchar_isflag;
    പൂർണ്ണസംഖ്യ		prevchar_isitem;
    പൂർണ്ണസംഖ്യ		itemisflag;
    പൂർണ്ണസംഖ്യ		fillable;
    char_u	*str;
    long	അക്കം;
    പൂർണ്ണസംഖ്യ		width;
    പൂർണ്ണസംഖ്യ		itemcnt;
    പൂർണ്ണസംഖ്യ		curitem;
    പൂർണ്ണസംഖ്യ		groupdepth;
# ifdef FEAT_EVAL
    പൂർണ്ണസംഖ്യ		evaldepth;
# endif
    പൂർണ്ണസംഖ്യ		minwid;
    പൂർണ്ണസംഖ്യ		maxwid;
    പൂർണ്ണസംഖ്യ		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    പൂർണ്ണസംഖ്യ		save_redraw_not_allowed = redraw_not_allowed;
    പൂർണ്ണസംഖ്യ		save_താക്കോൽ_Typed = താക്കോൽ_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
    പൂർണ്ണസംഖ്യ		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    എങ്കിൽ (updating_screen)
	redraw_not_allowed = TRUE;

    എങ്കിൽ (stl_ഇനങ്ങൾ == ശൂന്യ)
    {
	stl_ഇനങ്ങൾ = ALLOC_MULT(stl_item_T, stl_ഇനങ്ങൾ_len);
	stl_groupitem = ALLOC_MULT(പൂർണ്ണസംഖ്യ, stl_ഇനങ്ങൾ_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_ഇനങ്ങൾ_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_ഇനങ്ങൾ_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_ഇനങ്ങൾ_len + 1);

	stl_separator_locations = ALLOC_MULT(പൂർണ്ണസംഖ്യ, stl_ഇനങ്ങൾ_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    എങ്കിൽ (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_സംഖ്യ;
	tv.vval.v_സംഖ്യ = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	എങ്കിൽ (usefmt == ശൂന്യ)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    എങ്കിൽ (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    എങ്കിൽ (lnum > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
    {
	lnum = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_വരി = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    എങ്കിൽ (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    അല്ലെങ്കിൽ
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    എങ്കിൽ (carry_hl != ശൂന്യ && *carry_hl != 0)
    {
	stl_ഇനങ്ങൾ[curitem].stl_type = Highlight;
	stl_ഇനങ്ങൾ[curitem].stl_start = p;
	stl_ഇനങ്ങൾ[curitem].stl_minwid = *carry_hl;
	stl_ഇനങ്ങൾ[curitem].stl_clickfunc = ശൂന്യ;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    വേണ്ടി (s = usefmt; *s != NUL; )
    {
	എങ്കിൽ (curitem == (പൂർണ്ണസംഖ്യ)stl_ഇനങ്ങൾ_len)
	{
	    size_t	new_len = stl_ഇനങ്ങൾ_len * 3 / 2;

	    stl_item_T *new_ഇനങ്ങൾ =
			  vim_realloc(stl_ഇനങ്ങൾ, വലുപ്പം(stl_item_T) * new_len);
	    എങ്കിൽ (new_ഇനങ്ങൾ == ശൂന്യ)
		നിർത്തുക;
	    stl_ഇനങ്ങൾ = new_ഇനങ്ങൾ;

	    പൂർണ്ണസംഖ്യ *new_groupitem =
			     vim_realloc(stl_groupitem, വലുപ്പം(പൂർണ്ണസംഖ്യ) * new_len);
	    എങ്കിൽ (new_groupitem == ശൂന്യ)
		നിർത്തുക;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  വലുപ്പം(stl_hlrec_T) * (new_len + 1));
	    എങ്കിൽ (new_hlrec == ശൂന്യ)
		നിർത്തുക;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  വലുപ്പം(stl_hlrec_T) * (new_len + 1));
	    എങ്കിൽ (new_hlrec == ശൂന്യ)
		നിർത്തുക;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      വലുപ്പം(stl_clickrec_T) * (new_len + 1));
	    എങ്കിൽ (new_clickrec == ശൂന്യ)
		നിർത്തുക;
	    stl_clicktab = new_clickrec;

	    പൂർണ്ണസംഖ്യ *new_separator_locs = vim_realloc(stl_separator_locations,
					    വലുപ്പം(പൂർണ്ണസംഖ്യ) * new_len);
	    എങ്കിൽ (new_separator_locs == ശൂന്യ)
		നിർത്തുക;
	    stl_separator_locations = new_separator_locs;

	    stl_ഇനങ്ങൾ_len = new_len;
	}

	stl_ഇനങ്ങൾ[curitem].stl_clickfunc = ശൂന്യ;

	എങ്കിൽ (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	ആകുമ്പോൾ (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    എങ്കിൽ (*s == '\n' || *s == '\r')
	    {
		എങ്കിൽ (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    പോകുക find_linebreak;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	എങ്കിൽ (*s == NUL || p + 1 >= out + outlen)
	    നിർത്തുക;

	/*
	 * Handle one '%' item.
	 */
	s++;
	എങ്കിൽ (*s == NUL)  // ignore trailing %
	    നിർത്തുക;

	എങ്കിൽ (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    എങ്കിൽ (s[1] == ']')
	    {
		stl_ഇനങ്ങൾ[curitem].stl_type = ClickFunc;
		stl_ഇനങ്ങൾ[curitem].stl_start = p;
		stl_ഇനങ്ങൾ[curitem].stl_minwid = 0;
		stl_ഇനങ്ങൾ[curitem].stl_clickfunc = ശൂന്യ;
		s += 2;
		curitem++;
		തുടരുക;
	    }
	    // %[FuncName] - start click region
	    എങ്കിൽ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		എങ്കിൽ (rb != ശൂന്യ)
		{
		    stl_ഇനങ്ങൾ[curitem].stl_type = ClickFunc;
		    stl_ഇനങ്ങൾ[curitem].stl_start = p;
		    stl_ഇനങ്ങൾ[curitem].stl_minwid = 0;
		    stl_ഇനങ്ങൾ[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    തുടരുക;
		}
	    }
	}
	എങ്കിൽ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    എങ്കിൽ (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	എങ്കിൽ (*s == '%')
	{
	    എങ്കിൽ (p + 1 >= out + outlen)
		നിർത്തുക;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    തുടരുക;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	എങ്കിൽ (*s == STL_SEPARATE)
	{
	    s++;
	    എങ്കിൽ (groupdepth > 0)
		തുടരുക;
	    stl_ഇനങ്ങൾ[curitem].stl_type = Separate;
	    stl_ഇനങ്ങൾ[curitem++].stl_start = p;
	    തുടരുക;
	}
	എങ്കിൽ (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_ഇനങ്ങൾ[curitem].stl_type = Trunc;
	    stl_ഇനങ്ങൾ[curitem++].stl_start = p;
	    തുടരുക;
	}
	എങ്കിൽ (*s == ')')
	{
	    char_u  *t;

	    s++;
	    എങ്കിൽ (groupdepth < 1)
		തുടരുക;
	    groupdepth--;

	    t = stl_ഇനങ്ങൾ[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    എങ്കിൽ (curitem > stl_groupitem[groupdepth] + 1
		    && stl_ഇനങ്ങൾ[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		പൂർണ്ണസംഖ്യ group_start_userhl = 0;
		പൂർണ്ണസംഖ്യ group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		വേണ്ടി (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    എങ്കിൽ (stl_ഇനങ്ങൾ[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_ഇനങ്ങൾ[n].stl_minwid;
			നിർത്തുക;
		    }
		}
		വേണ്ടി (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    എങ്കിൽ (stl_ഇനങ്ങൾ[n].stl_type == Normal)
			നിർത്തുക;
		    എങ്കിൽ (stl_ഇനങ്ങൾ[n].stl_type == Highlight)
			group_end_userhl = stl_ഇനങ്ങൾ[n].stl_minwid;
		}
		എങ്കിൽ (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    വേണ്ടി (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			എങ്കിൽ (stl_ഇനങ്ങൾ[n].stl_type == Highlight)
			    stl_ഇനങ്ങൾ[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			എങ്കിൽ (stl_ഇനങ്ങൾ[n].stl_type == TabPage)
			    stl_ഇനങ്ങൾ[n].stl_start = p;
		    }
		}
	    }
	    എങ്കിൽ (l > stl_ഇനങ്ങൾ[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		എങ്കിൽ (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    ആകുമ്പോൾ (l >= stl_ഇനങ്ങൾ[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		അല്ലെങ്കിൽ
		    n = (long)(p - t) - stl_ഇനങ്ങൾ[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		പൂർണ്ണസംഖ്യ minwid_fixed = MIN(stl_ഇനങ്ങൾ[stl_groupitem[groupdepth]].stl_minwid,
				       stl_ഇനങ്ങൾ[stl_groupitem[groupdepth]].stl_maxwid);
		ആകുമ്പോൾ (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		വേണ്ടി (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_ഇനങ്ങൾ[l].stl_start -= n - 1;
		    എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_start < t)
			stl_ഇനങ്ങൾ[l].stl_start = t;
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (abs(stl_ഇനങ്ങൾ[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_ഇനങ്ങൾ[stl_groupitem[groupdepth]].stl_minwid;
		പൂർണ്ണസംഖ്യ fillchar_len = MB_CHAR2LEN(fillchar);
		എങ്കിൽ (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    ആകുമ്പോൾ (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		അല്ലെങ്കിൽ
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    എങ്കിൽ (p + l >= out + outlen)
		    {
			n = (long)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    വേണ്ടി ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    വേണ്ടി (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_ഇനങ്ങൾ[n].stl_start += l;
		}
	    }
	    തുടരുക;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	എങ്കിൽ (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	എങ്കിൽ (*s == '-')
	{
	    s++;
	    l = -1;
	}
	എങ്കിൽ (VIM_ISDIGIT(*s))
	{
	    minwid = (പൂർണ്ണസംഖ്യ)getdigits(&s);
	    എങ്കിൽ (minwid < 0)	// overflow
		minwid = 0;
	}
	എങ്കിൽ (*s == STL_USER_HL)
	{
	    stl_ഇനങ്ങൾ[curitem].stl_type = Highlight;
	    stl_ഇനങ്ങൾ[curitem].stl_start = p;
	    stl_ഇനങ്ങൾ[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    തുടരുക;
	}
	എങ്കിൽ (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    എങ്കിൽ (*s == STL_TABCLOSENR)
	    {
		എങ്കിൽ (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    വേണ്ടി (n = curitem - 1; n >= 0; --n)
			എങ്കിൽ (stl_ഇനങ്ങൾ[n].stl_type == TabPage
					       && stl_ഇനങ്ങൾ[n].stl_minwid >= 0)
			{
			    minwid = stl_ഇനങ്ങൾ[n].stl_minwid;
			    നിർത്തുക;
			}
		}
		അല്ലെങ്കിൽ
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_ഇനങ്ങൾ[curitem].stl_type = TabPage;
	    stl_ഇനങ്ങൾ[curitem].stl_start = p;
	    stl_ഇനങ്ങൾ[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    തുടരുക;
	}
	എങ്കിൽ (*s == '.')
	{
	    s++;
	    എങ്കിൽ (VIM_ISDIGIT(*s))
	    {
		maxwid = (പൂർണ്ണസംഖ്യ)getdigits(&s);
		എങ്കിൽ (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	പൂർണ്ണസംഖ്യ raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	എങ്കിൽ (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_ഇനങ്ങൾ[curitem].stl_type = Group;
	    stl_ഇനങ്ങൾ[curitem].stl_start = p;
	    stl_ഇനങ്ങൾ[curitem].stl_minwid = minwid;
	    stl_ഇനങ്ങൾ[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    തുടരുക;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	എങ്കിൽ (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    തുടരുക;
	}
# endif
	എങ്കിൽ (vim_strchr(STL_ALL, *s) == ശൂന്യ)
	{
	    എങ്കിൽ (*s == NUL)  // can happen with "%0"
		നിർത്തുക;
	    s++;
	    തുടരുക;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	അക്കം = -1;
	str = ശൂന്യ;
	തിരഞ്ഞെടുപ്പ് (opt)
	{
	അവസ്ഥ STL_FILEPATH:
	അവസ്ഥ STL_FULLPATH:
	അവസ്ഥ STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    എങ്കിൽ (name != ശൂന്യ)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    അല്ലെങ്കിൽ
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    എങ്കിൽ (opt != STL_FILENAME)
		str = NameBuff;
	    അല്ലെങ്കിൽ
		str = gettail(NameBuff);
	    നിർത്തുക;
	}

	അവസ്ഥ STL_VIM_വ്യഞ്ജകം: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    പൂർണ്ണസംഖ്യ	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    എങ്കിൽ (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    ആകുമ്പോൾ ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    എങ്കിൽ (*s != '}')	// missing '}' or out of space
		നിർത്തുക;
	    s++;
	    എങ്കിൽ (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    അല്ലെങ്കിൽ
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((അക്ഷരം *)buf_tmp, വലുപ്പം(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((അക്ഷരം *)buf_tmp, വലുപ്പം(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    എങ്കിൽ (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    എങ്കിൽ (!zeropad && str != ശൂന്യ && *str != NUL)
	    {
		എങ്കിൽ (*skipdigits(str) == NUL)
		{
		    അക്കം = atoi((അക്ഷരം *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    എങ്കിൽ (reevaluate && str != ശൂന്യ && *str != NUL
		    && strchr((സ്ഥിരം അക്ഷരം *)str, '%') != ശൂന്യ
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((സ്ഥിരം അക്ഷരം *)str);
		size_t fmt_length = strlen((സ്ഥിരം അക്ഷരം *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * വലുപ്പം(char_u));

		എങ്കിൽ (new_fmt != ശൂന്യ)
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
		    new_fmt_p = ശൂന്യ;

		    എങ്കിൽ (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    തുടരുക;
		}
	    }
# endif
	    നിർത്തുക;
	}
	അവസ്ഥ STL_വരി:
	    അക്കം = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    നിർത്തുക;

	അവസ്ഥ STL_NUMLINES:
	    അക്കം = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
	    നിർത്തുക;

	അവസ്ഥ STL_COLUMN:
	    അക്കം = (State & MODE_തിരുകുക) == 0 && empty_വരി
					       ? 0 : (പൂർണ്ണസംഖ്യ)wp->w_cursor.col + 1;
	    നിർത്തുക;

	അവസ്ഥ STL_VIRTCOL:
	അവസ്ഥ STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    എങ്കിൽ (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_തിരുകുക) == 0
			       && empty_വരി ? 0 : (പൂർണ്ണസംഖ്യ)wp->w_cursor.col + 1)))
		നിർത്തുക;
	    അക്കം = (long)virtcol;
	    നിർത്തുക;
	}

	അവസ്ഥ STL_PERCENTAGE:
	    അക്കം = calc_percentage((long)wp->w_cursor.lnum, (long)wp->w_buffer->b_ml.ml_വരി_എണ്ണം);
	    നിർത്തുക;

	അവസ്ഥ STL_ALTPERCENT:
	    str = buf_tmp;
	    (ശൂന്യം)get_rel_pos(wp, str, TMPLEN);
	    നിർത്തുക;

	അവസ്ഥ STL_SHOWCMD:
	    എങ്കിൽ (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    നിർത്തുക;

	അവസ്ഥ STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    എങ്കിൽ (append_arg_സംഖ്യ(wp, buf_tmp, വലുപ്പം(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    നിർത്തുക;

	അവസ്ഥ STL_KEYMAP:
	    fillable = FALSE;
	    എങ്കിൽ (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    നിർത്തുക;
	അവസ്ഥ STL_PAGENUM:
# എങ്കിൽ defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    അക്കം = printer_page_അക്കം;
# അല്ലെങ്കിൽ
	    അക്കം = 0;
# endif
	    നിർത്തുക;

	അവസ്ഥ STL_BUFNO:
	    അക്കം = wp->w_buffer->b_fnum;
	    നിർത്തുക;

	അവസ്ഥ STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	അവസ്ഥ STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_വരി_or_offset(wp->w_buffer, wp->w_cursor.lnum, ശൂന്യ);
	    അക്കം = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_തിരുകുക) == 0 && empty_വരി
				? 0 : (പൂർണ്ണസംഖ്യ)wp->w_cursor.col);
# endif
	    നിർത്തുക;

	അവസ്ഥ STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	അവസ്ഥ STL_BYTEVAL:
	    അക്കം = byteval;
	    എങ്കിൽ (അക്കം == NL)
		അക്കം = 0;
	    അല്ലെങ്കിൽ എങ്കിൽ (അക്കം == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		അക്കം = NL;
	    നിർത്തുക;

	അവസ്ഥ STL_ROFLAG:
	അവസ്ഥ STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    എങ്കിൽ (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    നിർത്തുക;

	അവസ്ഥ STL_HELPFLAG:
	അവസ്ഥ STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    എങ്കിൽ (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    നിർത്തുക;

	അവസ്ഥ STL_FILETYPE:
	    എങ്കിൽ (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((അക്ഷരം *)buf_tmp, വലുപ്പം(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    നിർത്തുക;

	അവസ്ഥ STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    എങ്കിൽ (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((അക്ഷരം *)buf_tmp, വലുപ്പം(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		വേണ്ടി (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    നിർത്തുക;

# എങ്കിൽ defined(FEAT_QUICKFIX)
	അവസ്ഥ STL_PREVIEWFLAG:
	അവസ്ഥ STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    എങ്കിൽ (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    നിർത്തുക;

	അവസ്ഥ STL_QUICKFIX:
	    എങ്കിൽ (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(സന്ദേശം_loclist)
			    : _(സന്ദേശം_qflist));
	    നിർത്തുക;
# endif

	അവസ്ഥ STL_MODIFIED:
	അവസ്ഥ STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    തിരഞ്ഞെടുപ്പ് ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		അവസ്ഥ 2: str = (char_u *)"[+]"; നിർത്തുക;
		അവസ്ഥ 3: str = (char_u *)",+"; നിർത്തുക;
		അവസ്ഥ 4: str = (char_u *)"[-]"; നിർത്തുക;
		അവസ്ഥ 5: str = (char_u *)",-"; നിർത്തുക;
		അവസ്ഥ 6: str = (char_u *)"[+-]"; നിർത്തുക;
		അവസ്ഥ 7: str = (char_u *)",+-"; നിർത്തുക;
	    }
	    നിർത്തുക;

	അവസ്ഥ STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		ആകുമ്പോൾ (*s != '#' && *s != NUL)
		    ++s;
		എങ്കിൽ (*s == '#')
		{
		    stl_ഇനങ്ങൾ[curitem].stl_type = Highlight;
		    stl_ഇനങ്ങൾ[curitem].stl_start = p;
		    stl_ഇനങ്ങൾ[curitem].stl_minwid = -syn_namen2id(t, (പൂർണ്ണസംഖ്യ)(s - t));
		    curitem++;
		}
		എങ്കിൽ (*s != NUL)
		    ++s;
		തുടരുക;
	    }

	അവസ്ഥ STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    എങ്കിൽ (*s == ']')
	    {
		stl_ഇനങ്ങൾ[curitem].stl_type = ClickFunc;
		stl_ഇനങ്ങൾ[curitem].stl_start = p;
		stl_ഇനങ്ങൾ[curitem].stl_minwid = 0;
		stl_ഇനങ്ങൾ[curitem].stl_clickfunc = ശൂന്യ;
		s++;
		curitem++;
		തുടരുക;
	    }
	    // %N[FuncName] with minwid
	    എങ്കിൽ (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		എങ്കിൽ (rb != ശൂന്യ)
		{
		    stl_ഇനങ്ങൾ[curitem].stl_type = ClickFunc;
		    stl_ഇനങ്ങൾ[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_ഇനങ്ങൾ[curitem].stl_minwid = raw_minwid;
		    stl_ഇനങ്ങൾ[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    തുടരുക;
		}
	    }
	    തുടരുക;

	അവസ്ഥ STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    തുടരുക;
	}

	stl_ഇനങ്ങൾ[curitem].stl_start = p;
	stl_ഇനങ്ങൾ[curitem].stl_type = Normal;
	എങ്കിൽ (str != ശൂന്യ && *str)
	{
	    char_u  *t = str;

	    എങ്കിൽ (itemisflag)
	    {
		എങ്കിൽ ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    എങ്കിൽ (l > 0)
		prevchar_isitem = TRUE;
	    എങ്കിൽ (l > maxwid)
	    {
		ആകുമ്പോൾ (l >= maxwid)
		    എങ്കിൽ (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    അല്ലെങ്കിൽ
			l -= byte2cells(*t++);
		എങ്കിൽ (p + 1 >= out + outlen)
		    നിർത്തുക;
		*p++ = '<';
	    }
	    എങ്കിൽ (minwid > 0)
	    {
		വേണ്ടി (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    എങ്കിൽ (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    അല്ലെങ്കിൽ
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    അല്ലെങ്കിൽ
		minwid *= -1;
	    വേണ്ടി (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		എങ്കിൽ (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		അല്ലെങ്കിൽ
		    *p++ = *t;
	    }
	    വേണ്ടി (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (അക്കം >= 0)
	{
	    പൂർണ്ണസംഖ്യ	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    എങ്കിൽ (p + 20 >= out + outlen)
		നിർത്തുക;		// not sufficient space
	    prevchar_isitem = TRUE;
	    എങ്കിൽ (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    എങ്കിൽ (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    വേണ്ടി (n = അക്കം, l = 1; n >= nbase; n /= nbase)
		l++;
	    എങ്കിൽ (opt == STL_VIRTCOL_ALT)
		l++;

	    എങ്കിൽ (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		ആകുമ്പോൾ (l-- > maxwid)
		    അക്കം /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((അക്ഷരം *)p, outlen - (p - out),
		    (അക്ഷരം *)nstr, 0, അക്കം, n);
	    }
	    അല്ലെങ്കിൽ
		p += vim_snprintf_safelen((അക്ഷരം *)p, outlen - (p - out),
		    (അക്ഷരം *)nstr, minwid, അക്കം);
	}
	അല്ലെങ്കിൽ
	    stl_ഇനങ്ങൾ[curitem].stl_type = Empty;

	എങ്കിൽ (അക്കം >= 0 || (!itemisflag && str != ശൂന്യ && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	എങ്കിൽ (opt == STL_VIM_വ്യഞ്ജകം)
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
    എങ്കിൽ (carry_hl != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ last_hl = 0;
	വേണ്ടി (l = 0; l < itemcnt; l++)
	    എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_type == Highlight)
		last_hl = stl_ഇനങ്ങൾ[l].stl_minwid;
	*carry_hl = last_hl;
    }

    എങ്കിൽ (mode == STL_MODE_MULTI
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
	എങ്കിൽ (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = strlen((അക്ഷരം *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    അല്ലെങ്കിൽ
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	എങ്കിൽ (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    എങ്കിൽ (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	വേണ്ടി (l = 0; l < itemcnt; l++)
	    എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_type == ClickFunc)
		vim_free(stl_ഇനങ്ങൾ[l].stl_clickfunc);
	എങ്കിൽ (rendered_height != ശൂന്യ)
	    *rendered_height = rheight;
	തിരികെ 0;
    }

    width = vim_strsize(out);
    എങ്കിൽ (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	എങ്കിൽ (itemcnt == 0)
	    s = out;
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി ( ; l < itemcnt; l++)
		എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_ഇനങ്ങൾ[l].stl_start;
		    നിർത്തുക;
		}
	    എങ്കിൽ (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_ഇനങ്ങൾ[0].stl_start;
		l = 0;
	    }
	}

	എങ്കിൽ (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    എങ്കിൽ (has_mbyte)
	    {
		s = out;
		width = 0;
		വേണ്ടി (;;)
		{
		    width += ptr2cells(s);
		    എങ്കിൽ (width >= maxwidth)
			നിർത്തുക;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		ആകുമ്പോൾ (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    അല്ലെങ്കിൽ
		s = out + maxwidth - 1;
	    വേണ്ടി (l = 0; l < itemcnt; l++)
		എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_start > s)
		    നിർത്തുക;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	അല്ലെങ്കിൽ
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    ആകുമ്പോൾ (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(end - p) + 1);	// +1 for NUL
	    end -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    വേണ്ടി (; l < itemcnt; l++)
	    {
		എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_start - n >= s)
		    stl_ഇനങ്ങൾ[l].stl_start -= n;
		അല്ലെങ്കിൽ
		    stl_ഇനങ്ങൾ[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    ആകുമ്പോൾ (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	പൂർണ്ണസംഖ്യ അക്കം_separators = 0;

	വേണ്ടി (l = 0; l < itemcnt; l++)
	{
	    എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[അക്കം_separators] = l;
		അക്കം_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	എങ്കിൽ (അക്കം_separators)
	{
	    പൂർണ്ണസംഖ്യ standard_spaces;
	    പൂർണ്ണസംഖ്യ final_spaces;

	    standard_spaces = (maxwidth - width) / അക്കം_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (അക്കം_separators - 1);
	    വേണ്ടി (l = 0; l < അക്കം_separators; l++)
	    {
		പൂർണ്ണസംഖ്യ dislocation = (l == (അക്കം_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_ഇനങ്ങൾ[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		വേണ്ടി (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_ഇനങ്ങൾ[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    എങ്കിൽ (hltab != ശൂന്യ)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	വേണ്ടി (l = 0; l < itemcnt; l++)
	{
	    എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_type == Highlight)
	    {
		sp->start = stl_ഇനങ്ങൾ[l].stl_start;
		sp->userhl = stl_ഇനങ്ങൾ[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = ശൂന്യ;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    എങ്കിൽ (tabtab != ശൂന്യ)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	വേണ്ടി (l = 0; l < itemcnt; l++)
	{
	    എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_type == TabPage)
	    {
		sp->start = stl_ഇനങ്ങൾ[l].stl_start;
		sp->userhl = stl_ഇനങ്ങൾ[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = ശൂന്യ;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    എങ്കിൽ (clicktab != ശൂന്യ)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	വേണ്ടി (l = 0; l < itemcnt; l++)
	{
	    എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_type == ClickFunc)
	    {
		cp->start = stl_ഇനങ്ങൾ[l].stl_start;
		cp->funcname = stl_ഇനങ്ങൾ[l].stl_clickfunc;
		cp->minwid = stl_ഇനങ്ങൾ[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = ശൂന്യ;
	cp->funcname = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
    {
	// Free click function names when caller doesn't need them.
	വേണ്ടി (l = 0; l < itemcnt; l++)
	    എങ്കിൽ (stl_ഇനങ്ങൾ[l].stl_type == ClickFunc)
		vim_free(stl_ഇനങ്ങൾ[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    താക്കോൽ_Typed = save_താക്കോൽ_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    എങ്കിൽ (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    തിരികെ width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    പൂർണ്ണസംഖ്യ
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		buflen)
{
    long	above; // number of lines above window
    long	below; // number of lines below window

    എങ്കിൽ (buflen < 3) // need at least 3 chars for writing
	തിരികെ 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    എങ്കിൽ (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_വരി_എണ്ണം - wp->w_botline + 1;
    എങ്കിൽ (below <= 0)
	തിരികെ (പൂർണ്ണസംഖ്യ)vim_snprintf_safelen((അക്ഷരം *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    എങ്കിൽ (above <= 0)
	തിരികെ (പൂർണ്ണസംഖ്യ)vim_snprintf_safelen((അക്ഷരം *)buf, buflen,
	    "%s", _("Top"));

    പൂർണ്ണസംഖ്യ perc = calc_percentage(above, above + below);
    അക്ഷരം tmp[8];
    // localized percentage value
    vim_snprintf(tmp, വലുപ്പം(tmp), _("%d%%"), perc);
    തിരികെ (പൂർണ്ണസംഖ്യ)vim_snprintf_safelen((അക്ഷരം *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
append_arg_സംഖ്യ(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    പൂർണ്ണസംഖ്യ		add_file)	// Add "file" before the arg number
{
    എങ്കിൽ (ARGCOUNT <= 1)		// nothing to do
	തിരികെ 0;

    അക്ഷരം *സന്ദേശം;
    തിരഞ്ഞെടുപ്പ് ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	അവസ്ഥ 0: സന്ദേശം = _(" (%d of %d)"); നിർത്തുക;
	അവസ്ഥ 1: സന്ദേശം = _(" ((%d) of %d)"); നിർത്തുക;
	അവസ്ഥ 2: സന്ദേശം = _(" (file %d of %d)"); നിർത്തുക;
	അവസ്ഥ 3: സന്ദേശം = _(" (file (%d) of %d)"); നിർത്തുക;
    }

    തിരികെ (പൂർണ്ണസംഖ്യ)vim_snprintf_safelen((അക്ഷരം *)buf, buflen, സന്ദേശം,
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
    തിരികെ FullName_save(fname, TRUE);
#അല്ലെങ്കിൽ
    എങ്കിൽ (!vim_isAbsName(fname)
	    || strstr((അക്ഷരം *)fname, "..") != ശൂന്യ
	    || strstr((അക്ഷരം *)fname, "//") != ശൂന്യ
# ifdef BACKSLASH_IN_FILENAME
	    || strstr((അക്ഷരം *)fname, "\\\\") != ശൂന്യ
# endif
# എങ്കിൽ defined(MSWIN)
	    || vim_strchr(fname, '~') != ശൂന്യ
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	തിരികെ FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    എങ്കിൽ (fname != ശൂന്യ)
	fname_case(fname, 0);	// set correct case for file name
# endif

    തിരികെ fname;
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
    ശൂന്യം
fname_expand(
    buf_T	*buf UNUSED,
    char_u	**ffname,
    char_u	**sfname)
{
    എങ്കിൽ (*ffname == ശൂന്യ)	    // no file name given, nothing to do
	തിരികെ;
    എങ്കിൽ (*sfname == ശൂന്യ)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    എങ്കിൽ (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	എങ്കിൽ (rfname != ശൂന്യ)
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
    ശൂന്യം
ex_buffer_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    പൂർണ്ണസംഖ്യ		split_ret = OK;
    പൂർണ്ണസംഖ്യ		p_ea_save;
    പൂർണ്ണസംഖ്യ		open_wins = 0;
    പൂർണ്ണസംഖ്യ		r;
    പൂർണ്ണസംഖ്യ		എണ്ണം;		// Maximum number of windows to open.
    പൂർണ്ണസംഖ്യ		all;		// When TRUE also load inactive buffers.
    പൂർണ്ണസംഖ്യ		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    എങ്കിൽ (eap->addr_എണ്ണം == 0)	// make as many windows as possible
	എണ്ണം = 9999;
    അല്ലെങ്കിൽ
	എണ്ണം = eap->line2;	// make as many windows as specified
    എങ്കിൽ (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    വേണ്ടി (;;)
    {
	tpnext = curtab->tp_next;
	വേണ്ടി (wp = firstwin; wp != ശൂന്യ; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    എങ്കിൽ ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		എങ്കിൽ (win_close(wp, FALSE) == FAIL)
		    നിർത്തുക;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    അല്ലെങ്കിൽ
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	എങ്കിൽ (had_tab == 0 || tpnext == ശൂന്യ)
	    നിർത്തുക;
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
    വേണ്ടി (buf = firstbuf; buf != ശൂന്യ && open_wins < എണ്ണം; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	എങ്കിൽ ((!all && buf->b_ml.ml_mfp == ശൂന്യ) || !buf->b_p_bl)
	    തുടരുക;

	എങ്കിൽ (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    എങ്കിൽ (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    അല്ലെങ്കിൽ
		wp = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		എങ്കിൽ (wp->w_buffer == buf)
		    നിർത്തുക;
	    // If the buffer already has a window, move it
	    എങ്കിൽ (wp != ശൂന്യ)
		win_move_after(wp, curwin);
	}

	എങ്കിൽ (wp == ശൂന്യ && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    എങ്കിൽ (split_ret == FAIL)
		തുടരുക;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    എങ്കിൽ (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		നിർത്തുക;
	    }
	    എങ്കിൽ (swap_exists_action == SEA_QUIT)
	    {
#എങ്കിൽ defined(FEAT_EVAL)
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

#എങ്കിൽ defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    അല്ലെങ്കിൽ
		handle_swap_exists(ശൂന്യ);
	}

	ui_breakcheck();
	എങ്കിൽ (got_int)
	{
	    (ശൂന്യം)vgetc();	// only break the file loading, not the rest
	    നിർത്തുക;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	എങ്കിൽ (aborting())
	    നിർത്തുക;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	എങ്കിൽ (had_tab > 0 && tabpage_index(ശൂന്യ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    വേണ്ടി (wp = lastwin; open_wins > എണ്ണം; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	എങ്കിൽ (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	അല്ലെങ്കിൽ
	{
	    wp = wp->w_prev;
	    എങ്കിൽ (wp == ശൂന്യ)
		നിർത്തുക;
	}
    }
}


സ്ഥിതം പൂർണ്ണസംഖ്യ  chk_modeline(linenr_T, പൂർണ്ണസംഖ്യ);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    ശൂന്യം
do_modelines(പൂർണ്ണസംഖ്യ flags)
{
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		nmlines;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	entered = 0;

    എങ്കിൽ (!curbuf->b_p_ml || (nmlines = (പൂർണ്ണസംഖ്യ)p_mls) == 0)
	തിരികെ;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    എങ്കിൽ (entered)
	തിരികെ;

    ++entered;
    വേണ്ടി (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_വരി_എണ്ണം && lnum <= nmlines;
								       ++lnum)
	എങ്കിൽ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    വേണ്ടി (lnum = curbuf->b_ml.ml_വരി_എണ്ണം; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_വരി_എണ്ണം - nmlines; --lnum)
	എങ്കിൽ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#അന്തര്ഭാവ "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
chk_modeline(
    linenr_T	lnum,
    പൂർണ്ണസംഖ്യ		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*വരി_end;		// point to the end of the line
    char_u	*e;
    പൂർണ്ണസംഖ്യ		prev;
    പൂർണ്ണസംഖ്യ		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    വരി_end = s + ml_get_len(lnum);
    വേണ്ടി (; *s != NUL; ++s)
    {
	എങ്കിൽ (prev == -1 || vim_isspace(prev))
	{
	    എങ്കിൽ ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		നിർത്തുക;
	    // Accept both "vim" and "Vim".
	    എങ്കിൽ ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		പൂർണ്ണസംഖ്യ vers;

		എങ്കിൽ (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		അല്ലെങ്കിൽ
		    e = s + 3;
		vers = getdigits(&e);
		എങ്കിൽ (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    നിർത്തുക;
	    }
	}
	prev = *s;
    }

    എങ്കിൽ (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	പൂർണ്ണസംഖ്യ	end;

	ചെയ്യുക				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	ആകുമ്പോൾ (s[-1] != ':');

	len = (size_t)(വരി_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	എങ്കിൽ (linecopy == ശൂന്യ)
	    തിരികെ FAIL;

	വരി_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_തള്ളുക(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	ആകുമ്പോൾ (end == FALSE)
	{
	    s = skipwhite(s);
	    എങ്കിൽ (*s == NUL)
		നിർത്തുക;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    വേണ്ടി (e = s; *e != ':' && *e != NUL; ++e)
		എങ്കിൽ (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(വരി_end - (e + 1)) + 1);	// +1 for NUL
		    --വരി_end;
		}
	    എങ്കിൽ (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    എങ്കിൽ (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		എങ്കിൽ (*e != ':')		// no terminating ':'?
		    നിർത്തുക;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    എങ്കിൽ (*s != NUL)		// skip over an empty "::"
	    {
		പൂർണ്ണസംഖ്യ secure_save = secure;
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
		എങ്കിൽ (retval == FAIL)		// stop if error found
		    നിർത്തുക;
	    }
	    s = (e == വരി_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_എടുക്കുക();
	vim_free(linecopy);
    }
    തിരികെ retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    പൂർണ്ണസംഖ്യ
bt_normal(buf_T *buf)
{
    തിരികെ buf != ശൂന്യ && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    പൂർണ്ണസംഖ്യ
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    തിരികെ buf != ശൂന്യ && buf->b_p_bt[0] == 'q';
#അല്ലെങ്കിൽ
    തിരികെ FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    പൂർണ്ണസംഖ്യ
bt_terminal(buf_T *buf UNUSED)
{
#എങ്കിൽ defined(FEAT_TERMINAL)
    തിരികെ buf != ശൂന്യ && buf->b_p_bt[0] == 't';
#അല്ലെങ്കിൽ
    തിരികെ FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    പൂർണ്ണസംഖ്യ
bt_help(buf_T *buf)
{
    തിരികെ buf != ശൂന്യ && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    പൂർണ്ണസംഖ്യ
bt_prompt(buf_T *buf)
{
    തിരികെ buf != ശൂന്യ && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#എങ്കിൽ defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    പൂർണ്ണസംഖ്യ
bt_popup(buf_T *buf)
{
    തിരികെ buf != ശൂന്യ && buf->b_p_bt != ശൂന്യ
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    പൂർണ്ണസംഖ്യ
bt_nofilename(buf_T *buf)
{
    തിരികെ buf != ശൂന്യ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
bt_nofileread(buf_T *buf)
{
    തിരികെ buf != ശൂന്യ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#എങ്കിൽ defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    പൂർണ്ണസംഖ്യ
bt_nofile(buf_T *buf)
{
    തിരികെ buf != ശൂന്യ && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    പൂർണ്ണസംഖ്യ
bt_dontwrite(buf_T *buf)
{
    തിരികെ buf != ശൂന്യ && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    പൂർണ്ണസംഖ്യ
bt_dontwrite_സന്ദേശം(buf_T *buf)
{
    എങ്കിൽ (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    പൂർണ്ണസംഖ്യ
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    തിരഞ്ഞെടുപ്പ് (buf->b_p_bh[0])
    {
	അവസ്ഥ 'u':		    // "unload"
	അവസ്ഥ 'w':		    // "wipe"
	അവസ്ഥ 'd': തിരികെ FALSE;	    // "delete"
	അവസ്ഥ 'h': തിരികെ TRUE;	    // "hide"
    }
    തിരികെ (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#എങ്കിൽ defined(FEAT_QUICKFIX)
    എങ്കിൽ (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	എങ്കിൽ (buf->b_fnum == qf_അട്ടി_get_bufnr())
	    തിരികെ (char_u *)_(സന്ദേശം_qflist);
	അല്ലെങ്കിൽ
	    തിരികെ (char_u *)_(സന്ദേശം_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    എങ്കിൽ (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	എങ്കിൽ (buf->b_term != ശൂന്യ)
	    തിരികെ term_get_status_text(buf->b_term);
#endif
	എങ്കിൽ (buf->b_fname != ശൂന്യ)
	    തിരികെ buf->b_fname;
	എങ്കിൽ (buf == cmdwin_buf)
	    തിരികെ (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	എങ്കിൽ (bt_prompt(buf))
	    തിരികെ (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	എങ്കിൽ (bt_popup(buf))
	    തിരികെ (char_u *)_("[Popup]");
#endif
	തിരികെ (char_u *)_("[Scratch]");
    }

    എങ്കിൽ (buf->b_fname == ശൂന്യ)
	തിരികെ buf_get_fname(buf);
    തിരികെ ശൂന്യ;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    എങ്കിൽ (buf->b_fname == ശൂന്യ)
	തിരികെ (char_u *)_("[No Name]");
    തിരികെ buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    ശൂന്യം
set_buflisted(പൂർണ്ണസംഖ്യ on)
{
    എങ്കിൽ (on == curbuf->b_p_bl)
	തിരികെ;

    curbuf->b_p_bl = on;
    എങ്കിൽ (on)
	apply_autocmds(EVENT_BUFADD, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
    അല്ലെങ്കിൽ
	apply_autocmds(EVENT_BUFDELETE, ശൂന്യ, ശൂന്യ, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    പൂർണ്ണസംഖ്യ
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    പൂർണ്ണസംഖ്യ		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ശൂന്യ, ശൂന്യ, (linenr_T)1, BLN_DUMMY);
    എങ്കിൽ (newbuf == ശൂന്യ)
	തിരികെ TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    എങ്കിൽ (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	തിരികെ TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    എങ്കിൽ (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	തിരികെ TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    എങ്കിൽ (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	എങ്കിൽ (buf->b_ml.ml_വരി_എണ്ണം == curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    differ = FALSE;
	    വേണ്ടി (lnum = 1; lnum <= curbuf->b_ml.ml_വരി_എണ്ണം; ++lnum)
		എങ്കിൽ (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    നിർത്തുക;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    എങ്കിൽ (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    തിരികെ differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    ശൂന്യം
wipe_buffer(
    buf_T	*buf,
    പൂർണ്ണസംഖ്യ		aucmd)	    // When TRUE trigger autocommands.
{
    എങ്കിൽ (buf->b_fnum == മുകൾ_file_അക്കം - 1)
	--മുകൾ_file_അക്കം;

    എങ്കിൽ (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(ശൂന്യ, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    എങ്കിൽ (!aucmd)
	unblock_autocmds();
}
