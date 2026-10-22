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

#ಅನ್ತರ್ಭಾವ "vim.h"


#ifdef FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_DEPTH 100
#endif

ಸ್ಥಿರ ಶೂನ್ಯ	enter_buffer(buf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ	buflist_getfpos(ಶೂನ್ಯ);
ಸ್ಥಿರ char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, ಪೂರ್ಣಾಙ್ಕ ignore_case);
ಸ್ಥಿರ char_u	*fname_match(regmatch_T *rmp, char_u *name, ಪೂರ್ಣಾಙ್ಕ ignore_case);
#ifdef UNIX
ಸ್ಥಿರ buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	buf_same_ino(buf_T *buf, stat_T *stp);
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	ಮೌಲ್ಯ_changed(char_u *str, char_u **last);
#ಯದಿ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, ಪೂರ್ಣಾಙ್ಕ opt_scope, ಪೂರ್ಣಾಙ್ಕ fillchar, ಪೂರ್ಣಾಙ್ಕ maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, ಪೂರ್ಣಾಙ್ಕ *lbreaks, ಪೂರ್ಣಾಙ್ಕ *carry_hl);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	append_arg_ಸಂಖ್ಯೆ(win_T *wp, char_u *buf, size_t buflen, ಪೂರ್ಣಾಙ್ಕ add_file);
ಸ್ಥಿರ ಶೂನ್ಯ	free_buffer(buf_T *);
ಸ್ಥಿರ ಶೂನ್ಯ	free_buffer_stuff(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ free_options);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	bt_nofileread(buf_T *buf);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	do_buffer_ext(ಪೂರ್ಣಾಙ್ಕ action, ಪೂರ್ಣಾಙ್ಕ start, ಪೂರ್ಣಾಙ್ಕ dir, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ flags);

#ifdef UNIX
# define dev_T dev_t
#ಅನ್ಯಥಾ
# define dev_T unsigned
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    ಪ್ರತಿ ((buf) = lastbuf; (buf) != ರಿಕ್ತ; (buf) = (buf)->b_prev)

#ಯದಿ defined(FEAT_QUICKFIX)
ಸ್ಥಿರ ವರ್ಣ *ಸಂದೇಶ_loclist = N_("[Location List]");
ಸ್ಥಿರ ವರ್ಣ *ಸಂದೇಶ_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	buf_free_ಎಣಿಕೆ = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	ತುದಿ_file_ಅಂಕೆ = 1;	// highest file number
ಸ್ಥಿರ garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"ಯದಿ exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
calc_percentage(long part, long whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    ಪ್ರತ್ಯಾಗಮ (part > 1000000L)
	? (ಪೂರ್ಣಾಙ್ಕ)(part / (whole / 100L))
	: (ಪೂರ್ಣಾಙ್ಕ)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_highest_fnum(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ ತುದಿ_file_ಅಂಕೆ - 1;
}

/*
 * Read data from buffer for retrying.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
read_buffer(
    ಪೂರ್ಣಾಙ್ಕ		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    ಪೂರ್ಣಾಙ್ಕ		flags)		    // extra flags for readfile()
{
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    retval = readfile(
	    read_stdin ? ರಿಕ್ತ : curbuf->b_ffname,
	    read_stdin ? ರಿಕ್ತ : curbuf->b_fname,
	    ಸಾಲು_ಎಣಿಕೆ, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    ಯದಿ (retval == OK)
    {
	// Delete the binary lines.
	ಯಾವತ್ (--ಸಾಲು_ಎಣಿಕೆ >= 0)
	    ml_delete((linenr_T)1);
    }
    ಅನ್ಯಥಾ
    {
	// Delete the converted lines.
	ಯಾವತ್ (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ > ಸಾಲು_ಎಣಿಕೆ)
	    ml_delete(ಸಾಲು_ಎಣಿಕೆ);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    ಯದಿ (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	ಯದಿ (!readonlymode && !BUFEMPTY())
	    changed();
	ಅನ್ಯಥಾ ಯದಿ (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	ಯದಿ (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, ರಿಕ್ತ, ರಿಕ್ತ, FALSE,
							      curbuf, &retval);
#ಅನ್ಯಥಾ
	    apply_autocmds(EVENT_STDINREADPOST, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
#endif
	}
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    ಶೂನ್ಯ
buffer_ensure_loaded(buf_T *buf)
{
    ಯದಿ (buf->b_ml.ml_mfp != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    ಯದಿ (curbuf == buf)
    {
	ಯದಿ (swap_exists_action != SEA_READONLY)
	    swap_exists_action = SEA_NONE;
	open_buffer(FALSE, ರಿಕ್ತ, 0);
	aucmd_restbuf(&aco);
    }
}
#endif

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
open_buffer(
    ಪೂರ್ಣಾಙ್ಕ		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    ಪೂರ್ಣಾಙ್ಕ		flags_arg)	    // extra flags for readfile()
{
    ಪೂರ್ಣಾಙ್ಕ		flags = flags_arg;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    ಪೂರ್ಣಾಙ್ಕ		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    ಯದಿ (readonlymode && curbuf->b_ffname != ರಿಕ್ತ
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    ಯದಿ (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    ಯದಿ (curbuf->b_ml.ml_mfp != ರಿಕ್ತ)
		ಭಙ್ಗ;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	ಯದಿ (curbuf == ರಿಕ್ತ)
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
	ಯದಿ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ರಿಕ್ತ, curwin);
#endif
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    ಯದಿ (curbuf->b_ml.ml_mfp != ರಿಕ್ತ)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    ಯದಿ (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    ಯದಿ (curbuf->b_ffname != ರಿಕ್ತ
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	ಪೂರ್ಣಾಙ್ಕ old_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent;
#ifdef UNIX
	ಪೂರ್ಣಾಙ್ಕ	save_bin = curbuf->b_p_bin;
	ಪೂರ್ಣಾಙ್ಕ perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	ಪೂರ್ಣಾಙ್ಕ oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	ಯದಿ (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	ಯದಿ (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	ಯದಿ (shortmess(SHM_FILEINFO))
	    ಸಂದೇಶ_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	ಯದಿ (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    ಯದಿ (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_buffer(FALSE, eap, flags);
	}
#endif
	ಸಂದೇಶ_silent = old_ಸಂದೇಶ_silent;
#ifdef FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#endif
	// Help buffer is filtered.
	ಯದಿ (bt_help(curbuf))
	    fix_help_buffer();
    }
    ಅನ್ಯಥಾ ಯದಿ (read_stdin)
    {
	ಪೂರ್ಣಾಙ್ಕ	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(ರಿಕ್ತ, ರಿಕ್ತ, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, ರಿಕ್ತ,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	ಯದಿ (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    ಯದಿ (curbuf->b_ml.ml_mfp != ರಿಕ್ತ
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    ಯದಿ (curbuf->b_flags & BF_NEVERLOADED)
    {
	(ಶೂನ್ಯ)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    ಯದಿ ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != ರಿಕ್ತ)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != ರಿಕ್ತ)
#endif
       )
	changed();
    ಅನ್ಯಥಾ ಯದಿ (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    ಯದಿ (aborting())
#ಅನ್ಯಥಾ
    ಯದಿ (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    ಯದಿ (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf, &retval);
#ಅನ್ಯಥಾ
    apply_autocmds(EVENT_BUFENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
#endif

    ಯದಿ (retval != OK)
	ಪ್ರತ್ಯಾಗಮ retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    ಯದಿ (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != ರಿಕ್ತ)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	ಯದಿ (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    ಯದಿ ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, ರಿಕ್ತ, ರಿಕ್ತ,
			FALSE, curbuf, &retval);
#ಅನ್ಯಥಾ
	    apply_autocmds(EVENT_BUFWINENTER, ರಿಕ್ತ, ರಿಕ್ತ,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    ಶೂನ್ಯ
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == ರಿಕ್ತ ? 0 : buf->b_fnum;
    bufref->br_buf_free_ಎಣಿಕೆ = buf_free_ಎಣಿಕೆ;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    ಪೂರ್ಣಾಙ್ಕ
bufref_valid(bufref_T *bufref)
{
    ಪ್ರತ್ಯಾಗಮ bufref->br_buf_free_ಎಣಿಕೆ == buf_free_ಎಣಿಕೆ
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    ಪೂರ್ಣಾಙ್ಕ
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	ಯದಿ (bp == buf)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
ಸ್ಥಿರ hashtab_T buf_hashtab;

    ಸ್ಥಿರ ಶೂನ್ಯ
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((ವರ್ಣ *)buf->b_ಕೀಲಿ, ಪರಿಮಾಣ(buf->b_ಕೀಲಿ), "%x", buf->b_fnum);
    ಯದಿ (hash_add(&buf_hashtab, buf->b_ಕೀಲಿ, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_ಕೀಲಿ);

    ಯದಿ (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
can_unload_buffer(buf_T *buf)
{
    ಪೂರ್ಣಾಙ್ಕ	    can_unload = !buf->b_locked;

    ಯದಿ (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ಯದಿ (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		ಭಙ್ಗ;
	    }
    }
    // Don't unload the buffer while it's still being saved
    ಯದಿ (can_unload && buf->b_saving)
	can_unload = FALSE;

    ಯದಿ (!can_unload)
    {
	char_u *fname = buf->b_fname != ರಿಕ್ತ ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != ರಿಕ್ತ ? fname : (char_u *)"[No Name]");
    }
    ಪ್ರತ್ಯಾಗಮ can_unload;
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
    ಪೂರ್ಣಾಙ್ಕ
close_buffer(
    win_T	*win,
    buf_T	*buf,
    ಪೂರ್ಣಾಙ್ಕ		action,
    ಪೂರ್ಣಾಙ್ಕ		abort_if_last,
    ಪೂರ್ಣಾಙ್ಕ		ignore_abort,
    ಪೂರ್ಣಾಙ್ಕ		set_context)
{
    ಪೂರ್ಣಾಙ್ಕ		hiding_buf;
    bufref_T	bufref;
    ಪೂರ್ಣಾಙ್ಕ		is_curwin = (curwin != ರಿಕ್ತ && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    ಪೂರ್ಣಾಙ್ಕ		unload_buf = (action != 0);
    ಪೂರ್ಣಾಙ್ಕ		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    ಪೂರ್ಣಾಙ್ಕ		del_buf = (action == DOBUF_DEL || wipe_buf);
    ಪೂರ್ಣಾಙ್ಕ		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    ಪೂರ್ಣಾಙ್ಕ		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    ಯದಿ (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    ಯದಿ (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	ಯದಿ (term_job_running(buf->b_term))
	{
	    ಯದಿ (wipe_buf || unload_buf)
	    {
		ಯದಿ (!can_unload_buffer(buf))
		    ಪ್ರತ್ಯಾಗಮ FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    ಅನ್ಯಥಾ
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (del_buf || unload_buf)
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
    ಯದಿ ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	ಯದಿ (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    ಯದಿ ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	ಯದಿ (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	ಯದಿ (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    ಗಚ್ಛ aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	ಯದಿ (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    ಯದಿ (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		ಗಚ್ಛ aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    ಯದಿ (abort_if_last && one_window())
		// Autocommands made this the only window.
		ಗಚ್ಛ aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	ಯದಿ (!ignore_abort && aborting())
	    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ಯದಿ (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    ಯದಿ (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    ಯದಿ (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    ಯದಿ (!hiding_buf || !unload_buf)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Always remove the buffer when there is no file name.
    ಯದಿ (buf->b_ffname == ರಿಕ್ತ)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    ಯದಿ (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    ಯದಿ ((win_valid || closed_popup) && win->w_buffer == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_buffer = ರಿಕ್ತ;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    ಯದಿ (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != ರಿಕ್ತ || buf->b_next != ರಿಕ್ತ
#ಯದಿ defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	ಯದಿ (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    ಯದಿ (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), 50);
	    ಯದಿ (ga_grow(&buf_reuse, 1) == OK)
		((ಪೂರ್ಣಾಙ್ಕ *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	ಯದಿ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ಅನ್ಯಥಾ
	    buf->b_sfname = ರಿಕ್ತ;
	VIM_CLEAR(buf->b_ffname);
	ಯದಿ (buf->b_prev == ರಿಕ್ತ)
	    firstbuf = buf->b_next;
	ಅನ್ಯಥಾ
	    buf->b_prev->b_next = buf->b_next;
	ಯದಿ (buf->b_next == ರಿಕ್ತ)
	    lastbuf = buf->b_prev;
	ಅನ್ಯಥಾ
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (del_buf)
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
	ಯದಿ (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    ಪ್ರತಿಜ್ಞಾ(!win_valid || !is_curwin || win == curwin);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Make buffer not contain a file.
 */
    ಶೂನ್ಯ
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_start_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_start_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_start_bomb = FALSE;
    buf->b_ml.ml_mfp = ರಿಕ್ತ;
    buf->b_ml.ml_flags = ML_EMPTY;		// empty buffer
#ifdef FEAT_NETBEANS_INTG
    netbeans_deleted_all_ಸಾಲುಗಳು(buf);
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
    ಪೂರ್ಣಾಙ್ಕ
buf_freeall(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಪೂರ್ಣಾಙ್ಕ		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    ಪೂರ್ಣಾಙ್ಕ		is_curwin = (curwin != ರಿಕ್ತ && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    ಯದಿ (buf->b_ml.ml_mfp != ರಿಕ್ತ)
    {
	ಯದಿ (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಯದಿ ((flags & BFA_DEL) && buf->b_p_bl)
    {
	ಯದಿ (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಯದಿ (flags & BFA_WIPE)
    {
	ಯದಿ (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ಯದಿ (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    ಯದಿ ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	ಪ್ರತ್ಯಾಗಮ FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    ಯದಿ (buf == curbuf && !is_curbuf)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    ಯದಿ (buf == curbuf && VIsual_active
#ಯದಿ defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    ಯದಿ (curwin != ರಿಕ್ತ && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    ಯದಿ (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ = 0;    // no lines in buffer
    ಯದಿ ((flags & BFA_KEEP_UNDO) == 0)
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
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_buffer(buf_T *buf)
{
    ++buf_free_ಎಣಿಕೆ;
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

    ಯದಿ (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    ಅನ್ಯಥಾ
    {
	vim_free(buf);
	ಯದಿ (curbuf == buf)
	    curbuf = ರಿಕ್ತ;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_ಸಂಖ್ಯೆ;
    di->di_tv.v_lock = VAR_FIXED;
    di->di_tv.vval.v_ಸಂಖ್ಯೆ = 0;

#ifdef FEAT_EVAL
    STRCPY(buf->b_ct_di.di_ಕೀಲಿ, "changedtick");
    (ಶೂನ್ಯ)dict_add(buf->b_vars, di);
#endif
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    ಯಾವತ್ (buf->b_wininfo != ರಿಕ್ತ)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_next;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_buffer_stuff(
    buf_T	*buf,
    ಪೂರ್ಣಾಙ್ಕ		free_options)		// free options as well
{
    ಯದಿ (free_options)
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
    ಶೂನ್ಯ
free_wininfo(wininfo_T *wip)
{
    ಯದಿ (wip->wi_optset)
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
    ಶೂನ್ಯ
goto_buffer(
    exarg_T	*eap,
    ಪೂರ್ಣಾಙ್ಕ		start,
    ಪೂರ್ಣಾಙ್ಕ		dir,
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ)
{
    bufref_T	old_curbuf;
    ಪೂರ್ಣಾಙ್ಕ		save_sea = swap_exists_action;
    ಪೂರ್ಣಾಙ್ಕ		skip_help_buf;

    ವಿಕಲ್ಪ (eap->cmdidx)
    {
	ಪಕ್ಷ CMD_bnext:
	ಪಕ್ಷ CMD_sbnext:
	ಪಕ್ಷ CMD_bNext:
	ಪಕ್ಷ CMD_bprevious:
	ಪಕ್ಷ CMD_sbNext:
	ಪಕ್ಷ CMD_sbprevious:
	    skip_help_buf = TRUE;
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    skip_help_buf = FALSE;
	    ಭಙ್ಗ;
    }

    set_bufref(&old_curbuf, curbuf);

    ಯದಿ (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (ಶೂನ್ಯ)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, ಎಣಿಕೆ,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    ಯದಿ (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#ಯದಿ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#ಯದಿ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    ಅನ್ಯಥಾ
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    ಶೂನ್ಯ
handle_swap_exists(bufref_T *old_curbuf)
{
#ಯದಿ defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    ಯದಿ (swap_exists_action == SEA_QUIT)
    {
#ಯದಿ defined(FEAT_EVAL)
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
	ಯದಿ (old_curbuf == ರಿಕ್ತ || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(ರಿಕ್ತ, ರಿಕ್ತ, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	ಅನ್ಯಥಾ
	    buf = old_curbuf->br_buf;
	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ old_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent;

	    ಯದಿ (shortmess(SHM_FILEINFO))
		ಸಂದೇಶ_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    ಸಂದೇಶ_silent = old_ಸಂದೇಶ_silent;

#ifdef FEAT_SYN_HL
	    ಯದಿ (old_tw != curbuf->b_p_tw)
		check_colorcolumn(ರಿಕ್ತ, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#ಯದಿ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    ಅನ್ಯಥಾ ಯದಿ (swap_exists_action == SEA_RECOVER)
    {
#ಯದಿ defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Recover at ATTENTION prompt.
	ಸಂದೇಶ_scroll = TRUE;
	ml_recover(FALSE);
	ಸಂದೇಶ_puts("\n");	// don't overwrite the last message
	cmdline_row = ಸಂದೇಶ_row;
	do_modelines(0);

#ಯದಿ defined(FEAT_EVAL)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
empty_curbuf(
    ಪೂರ್ಣಾಙ್ಕ close_others,
    ಪೂರ್ಣಾಙ್ಕ forceit,
    ಪೂರ್ಣಾಙ್ಕ action)
{
    ಪೂರ್ಣಾಙ್ಕ	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    ಯದಿ (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    set_bufref(&bufref, buf);
    ಯದಿ (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    ಯದಿ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(ರಿಕ್ತ, buf, action, FALSE, FALSE, FALSE);
    ಯದಿ (!close_others)
	need_fileinfo = FALSE;
    ಅನ್ಯಥಾ ಯದಿ (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    ಪ್ರತ್ಯಾಗಮ retval;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_buffer_ext(
    ಪೂರ್ಣಾಙ್ಕ		action,
    ಪೂರ್ಣಾಙ್ಕ		start,
    ಪೂರ್ಣಾಙ್ಕ		dir,		// FORWARD or BACKWARD
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ,		// buffer number or number of buffers
    ಪೂರ್ಣಾಙ್ಕ		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    ಪೂರ್ಣಾಙ್ಕ		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    ವಿಕಲ್ಪ (start)
    {
	ಪಕ್ಷ DOBUF_FIRST:   buf = firstbuf; ಭಙ್ಗ;
	ಪಕ್ಷ DOBUF_LAST:    buf = lastbuf;  ಭಙ್ಗ;
	ಸ್ವಭಾವ:	    buf = curbuf;   ಭಙ್ಗ;
    }
    ಯದಿ (start == DOBUF_MOD)	    // find next modified buffer
    {
	ಯಾವತ್ (ಎಣಿಕೆ-- > 0)
	{
	    ಕುರು
	    {
		buf = buf->b_next;
		ಯದಿ (buf == ರಿಕ್ತ)
		    buf = firstbuf;
	    }
	    ಯಾವತ್ (buf != curbuf && !bufIsChanged(buf));
	}
	ಯದಿ (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (start == DOBUF_FIRST && ಎಣಿಕೆ) // find specified buffer number
    {
	ಯಾವತ್ (buf != ರಿಕ್ತ && buf->b_fnum != ಎಣಿಕೆ)
	    buf = buf->b_next;
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = ರಿಕ್ತ;
	ಯಾವತ್ (ಎಣಿಕೆ > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    ಯದಿ (bp == ರಿಕ್ತ)
		bp = buf;
	    ಯದಿ (dir == FORWARD)
	    {
		buf = buf->b_next;
		ಯದಿ (buf == ರಿಕ್ತ)
		    buf = firstbuf;
	    }
	    ಅನ್ಯಥಾ
	    {
		buf = buf->b_prev;
		ಯದಿ (buf == ರಿಕ್ತ)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    ಯದಿ (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--ಎಣಿಕೆ;
		bp = ರಿಕ್ತ;	// use this buffer as new starting point
	    }
	    ಯದಿ (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
    }

    ಯದಿ (buf == ರಿಕ್ತ)	    // could not find it
    {
	ಯದಿ (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    ಯದಿ (!unload)
		semsg(_(e_buffer_nr_does_not_exist), ಎಣಿಕೆ);
	}
	ಅನ್ಯಥಾ ಯದಿ (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	ಅನ್ಯಥಾ
	    emsg(_(e_cannot_go_before_first_buffer));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
#ifdef FEAT_PROP_POPUP
    ಯದಿ ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	ಪ್ರತ್ಯಾಗಮ OK;
#endif
    ಯದಿ (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), ಎಣಿಕೆ);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    ಯದಿ (unload)
    {
	ಪೂರ್ಣಾಙ್ಕ	forward;
	bufref_T bufref;

	ಯದಿ (!can_unload_buffer(buf))
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	ಯದಿ (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == ರಿಕ್ತ && !buf->b_p_bl)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ಯದಿ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		ಯದಿ (term_job_running(buf->b_term))
		{
		    ಯದಿ (term_confirm_stop(buf) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಅನ್ಯಥಾ
# endif
		{
		    dialog_changed(buf, FALSE);
		    ಯದಿ (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    ಯದಿ (bufIsChanged(buf))
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ
#endif
	    {
		no_write_message_buf(buf);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	ಯದಿ (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    ಯದಿ (bp->b_p_bl && bp != buf)
		ಭಙ್ಗ;
	ಯದಿ (bp == ರಿಕ್ತ && buf == curbuf)
	    ಪ್ರತ್ಯಾಗಮ empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	ಯಾವತ್ (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != ರಿಕ್ತ))
	{
	    ಯದಿ (win_close(curwin, FALSE) == FAIL)
		ಭಙ್ಗ;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	ಯದಿ (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    ಯದಿ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(ರಿಕ್ತ, buf, action, FALSE, FALSE, FALSE);
	    ಪ್ರತ್ಯಾಗಮ OK;
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
	buf = ರಿಕ್ತ;	// selected buffer
	bp = ರಿಕ್ತ;	// used when no loaded buffer found
	ಯದಿ (au_new_curbuf.br_buf != ರಿಕ್ತ && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	ಅನ್ಯಥಾ ಯದಿ (curwin->w_jumplistlen > 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    ಯದಿ (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    ಯಾವತ್ (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		ಯದಿ (buf != ರಿಕ್ತ)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    ಯದಿ (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = ರಿಕ್ತ;
		    ಅನ್ಯಥಾ ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
		    {
			// skip unloaded buf, but may keep it for later
			ಯದಿ (bp == ರಿಕ್ತ)
			    bp = buf;
			buf = ರಿಕ್ತ;
		    }
		}
		ಯದಿ (buf != ರಿಕ್ತ)   // found a valid buffer: stop searching
		    ಭಙ್ಗ;
		// advance to older entry in jump list
		ಯದಿ (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    ಭಙ್ಗ;
		ಯದಿ (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		ಯದಿ (jumpidx == forward)		// List exhausted for sure
		    ಭಙ್ಗ;
	    }
	}

	ಯದಿ (buf == ರಿಕ್ತ)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    ಪ್ರತಿ (;;)
	    {
		ಯದಿ (buf == ರಿಕ್ತ)
		{
		    ಯದಿ (!forward)	// tried both directions
			ಭಙ್ಗ;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    ಅನುವರ್ತ;
		}
		// in non-help buffer, try to skip help buffers, and vv
		ಯದಿ (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    ಯದಿ (buf->b_ml.ml_mfp != ರಿಕ್ತ)   // found loaded buffer
			ಭಙ್ಗ;
		    ಯದಿ (bp == ರಿಕ್ತ)	// remember unloaded buf for later
			bp = buf;
		}
		ಯದಿ (forward)
		    buf = buf->b_next;
		ಅನ್ಯಥಾ
		    buf = buf->b_prev;
	    }
	}
	ಯದಿ (buf == ರಿಕ್ತ)	// No loaded buffer, use unloaded one
	    buf = bp;
	ಯದಿ (buf == ರಿಕ್ತ)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		ಯದಿ (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    ಭಙ್ಗ;
	}
	ಯದಿ (buf == ರಿಕ್ತ)	// Still no buffer, just take one
	{
	    ಯದಿ (curbuf->b_next != ರಿಕ್ತ)
		buf = curbuf->b_next;
	    ಅನ್ಯಥಾ
		buf = curbuf->b_prev;
	    ಯದಿ (bt_quickfix(buf)
		    || (buf != ರಿಕ್ತ && buf != curbuf && buf->b_locked_split))
		buf = ರಿಕ್ತ;
	}
    }

    ಯದಿ (buf == ರಿಕ್ತ)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	ಪ್ರತ್ಯಾಗಮ empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    ಯದಿ (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    ಯದಿ (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // go to current buffer - nothing to do
    ಯದಿ (buf == curbuf)
	ಪ್ರತ್ಯಾಗಮ OK;

    // Check if the current buffer may be abandoned.
    ಯದಿ (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ಯದಿ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ಯದಿ (term_job_running(curbuf->b_term))
	    {
		ಯದಿ (term_confirm_stop(curbuf) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    ಅನ್ಯಥಾ
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		ಯದಿ (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		ಯದಿ (bufIsChanged(curbuf))
		{
		    no_write_message();
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	}
	ಅನ್ಯಥಾ
#endif
	{
	    no_write_message();
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    ಯದಿ (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#ಯದಿ defined(FEAT_EVAL)
    ಯದಿ (aborting())	    // autocmds may abort script processing
	ಪ್ರತ್ಯಾಗಮ FAIL;
#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
do_buffer(
    ಪೂರ್ಣಾಙ್ಕ		action,
    ಪೂರ್ಣಾಙ್ಕ		start,
    ಪೂರ್ಣಾಙ್ಕ		dir,		// FORWARD or BACKWARD
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ,		// buffer number or number of buffers
    ಪೂರ್ಣಾಙ್ಕ		forceit)	// TRUE when using !
{
    ಪ್ರತ್ಯಾಗಮ do_buffer_ext(action, start, dir, ಎಣಿಕೆ,
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
    ವರ್ಣ *
do_bufdel(
    ಪೂರ್ಣಾಙ್ಕ		command,
    char_u	*arg,		// pointer to extra arguments
    ಪೂರ್ಣಾಙ್ಕ		addr_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಙ್ಕ		start_bnr,	// first buffer number in a range
    ಪೂರ್ಣಾಙ್ಕ		end_bnr,	// buffer nr or last buffer nr in a range
    ಪೂರ್ಣಾಙ್ಕ		forceit)
{
    ಪೂರ್ಣಾಙ್ಕ		do_current = 0;	// delete current buffer?
    ಪೂರ್ಣಾಙ್ಕ		deleted = 0;	// number of buffers deleted
    ವರ್ಣ	*errormsg = ರಿಕ್ತ; // return value
    ಪೂರ್ಣಾಙ್ಕ		bnr;		// buffer number
    char_u	*p;

    ಯದಿ (addr_ಎಣಿಕೆ == 0)
    {
	(ಶೂನ್ಯ)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (addr_ಎಣಿಕೆ == 2)
	{
	    ಯದಿ (*arg)		// both range and argument is not allowed
		ಪ್ರತ್ಯಾಗಮ ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	ಅನ್ಯಥಾ	// addr_count == 1
	    bnr = end_bnr;

	ಪ್ರತಿ ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    ಯದಿ (bnr == curbuf->b_fnum)
		do_current = bnr;
	    ಅನ್ಯಥಾ ಯದಿ (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    ಯದಿ (addr_ಎಣಿಕೆ == 2)
	    {
		ಯದಿ (++bnr > end_bnr)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ    // addr_count == 1
	    {
		arg = skipwhite(arg);
		ಯದಿ (*arg == NUL)
		    ಭಙ್ಗ;
		ಯದಿ (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    ಯದಿ (bnr < 0)	    // failed
			ಭಙ್ಗ;
		    arg = p;
		}
		ಅನ್ಯಥಾ
		    bnr = getdigits(&arg);
	    }
	}
	ಯದಿ (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	ಯದಿ (deleted == 0)
	{
	    ಯದಿ (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    ಅನ್ಯಥಾ ಯದಿ (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    ಅನ್ಯಥಾ
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (ವರ್ಣ *)IObuff;
	}
	ಅನ್ಯಥಾ ಯದಿ (deleted >= p_report)
	{
	    ಯದಿ (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    ಅನ್ಯಥಾ ಯದಿ (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    ಅನ್ಯಥಾ
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    ಪ್ರತ್ಯಾಗಮ errormsg;
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
    ಶೂನ್ಯ
set_curbuf(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ action)
{
    buf_T	*prevbuf;
    ಪೂರ್ಣಾಙ್ಕ		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    ಪೂರ್ಣಾಙ್ಕ		valid;

    setpcmark();
    ಯದಿ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    ಯದಿ (!apply_autocmds(EVENT_BUFLEAVE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	ಯದಿ (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	ಯದಿ (unload)
	    close_windows(prevbuf, FALSE);
#ಯದಿ defined(FEAT_EVAL)
	ಯದಿ (bufref_valid(&prevbufref) && !aborting())
#ಅನ್ಯಥಾ
	ಯದಿ (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    ಯದಿ (prevbuf == curbuf
		    && ((State & MODE_ಸೇರಿಸು) == 0 || curbuf->b_nwindows <= 1))
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
    ಯದಿ ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == ರಿಕ್ತ)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	ಯದಿ (!valid)
	    enter_buffer(lastbuf);
	ಅನ್ಯಥಾ
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	ಯದಿ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ರಿಕ್ತ, curwin);
#endif
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
enter_buffer(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    ಯದಿ (VIsual_active
#ಯದಿ defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    ಯದಿ (curwin->w_buffer != ರಿಕ್ತ)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    ಯದಿ (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    ಅನ್ಯಥಾ
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    ಯದಿ (curwin->w_p_diff)
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
    ಯದಿ (curbuf->b_ml.ml_mfp == ರಿಕ್ತ)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	ಯದಿ (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, ರಿಕ್ತ, 0);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!ಸಂದೇಶ_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(ಶೂನ್ಯ)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
	apply_autocmds(EVENT_BUFENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    ಯದಿ (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    ಯದಿ (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    ಯದಿ (curbuf->b_kmap_state & KEYMAP_INIT)
	(ಶೂನ್ಯ)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    ಯದಿ (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(ಶೂನ್ಯ)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#ಯದಿ defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    ಶೂನ್ಯ
do_autochdir(ಶೂನ್ಯ)
{
    ಯದಿ ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != ರಿಕ್ತ
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	last_chdir_reason = "autochdir";
    }
}
#endif

    ಶೂನ್ಯ
no_write_message_buf(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    ಯದಿ (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    ಅನ್ಯಥಾ
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    ಶೂನ್ಯ
no_write_message(ಶೂನ್ಯ)
{
#ifdef FEAT_TERMINAL
    ಯದಿ (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    ಅನ್ಯಥಾ
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    ಶೂನ್ಯ
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    ಯದಿ (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    ಅನ್ಯಥಾ
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
    ಪೂರ್ಣಾಙ್ಕ
curbuf_reusable(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (curbuf != ರಿಕ್ತ
	&& curbuf->b_ffname == ರಿಕ್ತ
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == ರಿಕ್ತ || BUFEMPTY())
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
    ಪೂರ್ಣಾಙ್ಕ		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    ಯದಿ (ತುದಿ_file_ಅಂಕೆ == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    ಯದಿ (sfname == ರಿಕ್ತ || mch_stat((ವರ್ಣ *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    ಯದಿ (ffname != ರಿಕ್ತ && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#ಅನ್ಯಥಾ
		buflist_findname(ffname)
#endif
		) != ರಿಕ್ತ)
    {
	vim_free(ffname);
	ಯದಿ (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? ರಿಕ್ತ : curwin,
						      lnum, (colnr_T)0, FALSE);

	ಯದಿ ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	ಯದಿ ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    ಯದಿ (!(flags & BLN_DUMMY))
	    {
		ಯದಿ (apply_autocmds(EVENT_BUFADD, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, buf)
			&& !bufref_valid(&bufref))
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
	ಪ್ರತ್ಯಾಗಮ buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = ರಿಕ್ತ;
    ಯದಿ ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	ಯದಿ (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#endif
	ಯದಿ (!bufref_valid(&bufref))
	    buf = ರಿಕ್ತ;		// buf was deleted; allocate a new buffer
    }
    ಯದಿ (buf != curbuf || curbuf == ರಿಕ್ತ)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	ಯದಿ (buf == ರಿಕ್ತ)
	{
	    vim_free(ffname);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	ಯದಿ (buf->b_vars == ರಿಕ್ತ)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    ಯದಿ (ffname != ರಿಕ್ತ)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    ಯದಿ ((ffname != ರಿಕ್ತ && (buf->b_ffname == ರಿಕ್ತ || buf->b_sfname == ರಿಕ್ತ))
	    || buf->b_wininfo == ರಿಕ್ತ)
    {
	ಯದಿ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ಅನ್ಯಥಾ
	    buf->b_sfname = ರಿಕ್ತ;
	VIM_CLEAR(buf->b_ffname);
	ಯದಿ (buf != curbuf)
	    free_buffer(buf);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (buf == curbuf)
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
    ಅನ್ಯಥಾ
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = ರಿಕ್ತ;
	ಯದಿ (firstbuf == ರಿಕ್ತ)		// buffer list is empty
	{
	    buf->b_prev = ರಿಕ್ತ;
	    firstbuf = buf;
	}
	ಅನ್ಯಥಾ				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	ಯದಿ ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((ಪೂರ್ಣಾಙ್ಕ *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    ಯಾವತ್ (buf->b_prev != ರಿಕ್ತ && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		ಯದಿ (prev->b_next != ರಿಕ್ತ)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		ಯದಿ (buf->b_prev != ರಿಕ್ತ)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		ಯದಿ (lastbuf == buf)
		    lastbuf = prev;
		ಯದಿ (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	ಅನ್ಯಥಾ
	    buf->b_fnum = ತುದಿ_file_ಅಂಕೆ++;
	ಯದಿ (ತುದಿ_file_ಅಂಕೆ < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    ಯದಿ (emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    ತುದಿ_file_ಅಂಕೆ = 1;
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
    ಯದಿ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ಅನ್ಯಥಾ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    ಯದಿ (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    ಯದಿ (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	ಯದಿ (apply_autocmds(EVENT_BUFNEW, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, buf)
		&& !bufref_valid(&bufref))
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (flags & BLN_LISTED)
	{
	    ಯದಿ (apply_autocmds(EVENT_BUFADD, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, buf)
		    && !bufref_valid(&bufref))
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
#ifdef FEAT_EVAL
	ಯದಿ (aborting())		// autocmds may abort script processing
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#endif
    }

    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    ಶೂನ್ಯ
free_buf_options(
    buf_T	*buf,
    ಪೂರ್ಣಾಙ್ಕ		free_p_ff)
{
    ಯದಿ (free_p_ff)
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
#ಯದಿ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#ಯದಿ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#ಯದಿ defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#ಯದಿ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    ಯದಿ (buf->b_p_ಕೀಲಿ != ರಿಕ್ತ && *buf->b_p_ಕೀಲಿ != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_ಕೀಲಿ, STRLEN(buf->b_p_ಕೀಲಿ));
# endif
    clear_string_option(&buf->b_p_ಕೀಲಿ);
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
    buf->b_s.b_cap_prog = ರಿಕ್ತ;
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
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_ಎಣಿಕೆ);
    buf->b_p_cpt_ಎಣಿಕೆ = 0;
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
    ಪೂರ್ಣಾಙ್ಕ
buflist_getfile(
    ಪೂರ್ಣಾಙ್ಕ		n,
    linenr_T	lnum,
    ಪೂರ್ಣಾಙ್ಕ		options,
    ಪೂರ್ಣಾಙ್ಕ		forceit)
{
    buf_T	*buf;
    win_T	*wp = ರಿಕ್ತ;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	ಯದಿ ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	ಅನ್ಯಥಾ
	    semsg(_(e_buffer_nr_not_found), n);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    ಯದಿ (buf == curbuf)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (text_or_buf_locked())
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // altfpos may be changed by getfile(), get it now
    ಯದಿ (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    ಅನ್ಯಥಾ
	col = 0;

    ಯದಿ (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	ಯದಿ (wp == ರಿಕ್ತ && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    ಯದಿ (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    ಅನ್ಯಥಾ ಯದಿ (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    ಪೂರ್ಣಾಙ್ಕ retval = FAIL;
    ಯದಿ (GETFILE_SUCCESS(getfile(buf->b_fnum, ರಿಕ್ತ, ರಿಕ್ತ,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	ಯದಿ (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    ಯದಿ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * go to the last know line number for the current buffer
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
buflist_getfpos(ಶೂನ್ಯ)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    ಯದಿ (p_sol)
	curwin->w_cursor.col = 0;
    ಅನ್ಯಥಾ
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
    buf_T	*buf = ರಿಕ್ತ;

    // First make the name into a full path name
    ffname = FullName_save(fname,
#ifdef UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#ಅನ್ಯಥಾ
	    FALSE
#endif
	    );
    ಯದಿ (ffname != ರಿಕ್ತ)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    ಪ್ರತ್ಯಾಗಮ buf;
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

    ಯದಿ (mch_stat((ವರ್ಣ *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    ಪ್ರತ್ಯಾಗಮ buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    ಸ್ಥಿರ buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#endif
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_LAST(buf)
	ಯದಿ ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    ಪ್ರತ್ಯಾಗಮ buf;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    ಪೂರ್ಣಾಙ್ಕ
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    ಪೂರ್ಣಾಙ್ಕ		unlisted,	// find unlisted buffers
    ಪೂರ್ಣಾಙ್ಕ		diffmode UNUSED, // find diff-mode buffers only
    ಪೂರ್ಣಾಙ್ಕ		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		match = -1;
    ಪೂರ್ಣಾಙ್ಕ		find_listed;
    char_u	*pat;
    char_u	*patend;
    ಪೂರ್ಣಾಙ್ಕ		attempt;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    ಯದಿ ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	ಯದಿ (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	ಅನ್ಯಥಾ
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	ಯದಿ (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
    ಅನ್ಯಥಾ
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, ರಿಕ್ತ, FALSE);
	ಯದಿ (pat == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	ಪ್ರತಿ (;;)
	{
	    ಪ್ರತಿ (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		ಯದಿ (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		ಯದಿ (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			ಪ್ರತ್ಯಾಗಮ -1;
		    }
		    ಯದಿ (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != ರಿಕ್ತ)
		    {
			ಯದಿ (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				ಯದಿ (wp->w_buffer == buf)
				    ಭಙ್ಗ;
			    ಯದಿ (wp == ರಿಕ್ತ)
				ಅನುವರ್ತ;
			}
			ಯದಿ (match >= 0)		// already found a match
			{
			    match = -2;
			    ಭಙ್ಗ;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		ಯದಿ (match >= 0)			// found one match
		    ಭಙ್ಗ;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    ಯದಿ (!unlisted || !find_listed || match != -1)
		ಭಙ್ಗ;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    ಯದಿ (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    ಅನ್ಯಥಾ ಯದಿ (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    ಪ್ರತ್ಯಾಗಮ match;
}

#ifdef FEAT_VIMINFO
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
ExpandBufnames(
    char_u	*pat,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_file,
    char_u	***file,
    ಪೂರ್ಣಾಙ್ಕ		options)
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		round;
    char_u	*p;
    char_u	*patc = ರಿಕ್ತ;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = ರಿಕ್ತ;
#endif
    ಪೂರ್ಣಾಙ್ಕ		fuzzy;
    fuzmatch_str_T  *fuzmatch = ರಿಕ್ತ;
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಙ್ಕ		score = 0;
    ಪೂರ್ಣಾಙ್ಕ		to_free = FALSE;

    *ಅಂಕೆ_file = 0;		    // return values in case of FAIL
    *file = ರಿಕ್ತ;

#ifdef FEAT_DIFF
    ಯದಿ ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	ಪ್ರತ್ಯಾಗಮ FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    ಯದಿ (!fuzzy)
    {
	ಯದಿ (*pat == '^' && pat[1] != NUL)
	{
	    ಪೂರ್ಣಾಙ್ಕ  len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(pat);
	    patc = alloc(len);
	    ಯದಿ (patc == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (*pat == '^')
	    patc = (char_u *)"";
	ಅನ್ಯಥಾ
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    ಪ್ರತಿ (round = 1; round <= 2; ++round)
    {
	ಎಣಿಕೆ = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    ಯದಿ (!buf->b_p_bl)	// skip unlisted buffers
		ಅನುವರ್ತ;
#ifdef FEAT_DIFF
	    ಯದಿ (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		ಯದಿ (buf == curbuf || !diff_mode_buf(buf))
		    ಅನುವರ್ತ;
#endif

	    ಯದಿ (!fuzzy)
	    {
		ಯದಿ (regmatch.regprog == ರಿಕ್ತ)
		{
		    // invalid pattern, possibly after recompiling
		    ಯದಿ (to_free)
			vim_free(patc);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    ಅನ್ಯಥಾ
	    {
		p = ರಿಕ್ತ;
		// first try matching with the short file name
		ಯದಿ ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		ಯದಿ (p == ರಿಕ್ತ)
		{
		    // next try matching with the full path file name
		    ಯದಿ ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    ಯದಿ (p == ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    ಯದಿ (round == 1)
	    {
		++ಎಣಿಕೆ;
		ಅನುವರ್ತ;
	    }

	    ಯದಿ (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    ಅನ್ಯಥಾ
		p = vim_strsave(p);
	    ಯದಿ (p == ರಿಕ್ತ)
	    {
		ಯದಿ (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, ಎಣಿಕೆ);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    ಯದಿ (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		ಯದಿ (matches != ರಿಕ್ತ)
		{
		    matches[ಎಣಿಕೆ].buf = buf;
		    matches[ಎಣಿಕೆ].match = p;
		    ಎಣಿಕೆ++;
		}
		ಅನ್ಯಥಾ
#endif
		    (*file)[ಎಣಿಕೆ++] = p;
	    }
	    ಅನ್ಯಥಾ
	    {
		fuzmatch[ಎಣಿಕೆ].idx = ಎಣಿಕೆ;
		fuzmatch[ಎಣಿಕೆ].str = p;
		fuzmatch[ಎಣಿಕೆ].score = score;
		ಎಣಿಕೆ++;
	    }
	}
	ಯದಿ (ಎಣಿಕೆ == 0)	// no match found, break here
	    ಭಙ್ಗ;
	ಯದಿ (round == 1)
	{
	    ಯದಿ (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, ಎಣಿಕೆ);
		ಯದಿ (*file == ರಿಕ್ತ)
		{
		    vim_regfree(regmatch.regprog);
		    ಯದಿ (to_free)
			vim_free(patc);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
#ifdef FEAT_VIMINFO
		ಯದಿ (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, ಎಣಿಕೆ);
#endif
	    }
	    ಅನ್ಯಥಾ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, ಎಣಿಕೆ);
		ಯದಿ (fuzmatch == ರಿಕ್ತ)
		{
		    *ಅಂಕೆ_file = 0;
		    *file = ರಿಕ್ತ;
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	}
    }

    ಯದಿ (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	ಯದಿ (to_free)
	    vim_free(patc);
    }

    ಯದಿ (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	ಯದಿ (matches != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ i;
	    ಯದಿ (ಎಣಿಕೆ > 1)
		qsort(matches, ಎಣಿಕೆ, ಪರಿಮಾಣ(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    ಯದಿ (matches[0].buf == curbuf)
	    {
		ಪ್ರತಿ (i = 1; i < ಎಣಿಕೆ; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[ಎಣಿಕೆ-1] = matches[0].match;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (fuzzymatches_to_strmatches(fuzmatch, file, ಎಣಿಕೆ, FALSE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    *ಅಂಕೆ_file = ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ (ಎಣಿಕೆ == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    ಸ್ಥಿರ char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    ಪೂರ್ಣಾಙ್ಕ		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    ಯದಿ (match == ರಿಕ್ತ && rmp->regprog != ರಿಕ್ತ)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    ಪ್ರತ್ಯಾಗಮ match;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    ಸ್ಥಿರ char_u *
fname_match(
    regmatch_T	*rmp,
    char_u	*name,
    ಪೂರ್ಣಾಙ್ಕ		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = ರಿಕ್ತ;
    char_u	*p;

    // extra check for valid arguments
    ಯದಿ (name == ರಿಕ್ತ || rmp->regprog == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    ಯದಿ (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    ಅನ್ಯಥಾ ಯದಿ (rmp->regprog != ರಿಕ್ತ)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(ರಿಕ್ತ, name);
	ಯದಿ (p != ರಿಕ್ತ && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    ಪ್ರತ್ಯಾಗಮ match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(ಪೂರ್ಣಾಙ್ಕ nr)
{
    char_u	ಕೀಲಿ[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    ಯದಿ (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((ವರ್ಣ *)ಕೀಲಿ, ಪರಿಮಾಣ(ಕೀಲಿ), "%x", nr);
    hi = hash_find(&buf_hashtab, ಕೀಲಿ);

    ಯದಿ (!HASHITEM_EMPTY(hi))
	ಪ್ರತ್ಯಾಗಮ (buf_T *)(hi->hi_ಕೀಲಿ
			     - ((unsigned)(curbuf->b_ಕೀಲಿ - (char_u *)curbuf)));
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    ಪೂರ್ಣಾಙ್ಕ		n,
    ಪೂರ್ಣಾಙ್ಕ		fullname,
    ಪೂರ್ಣಾಙ್ಕ		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ home_replace_save(helptail ? buf : ರಿಕ್ತ,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    ಶೂನ್ಯ
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	col,
    ಪೂರ್ಣಾಙ್ಕ		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ಯದಿ (wip->wi_win == win)
	    ಭಙ್ಗ;
    ಯದಿ (wip == ರಿಕ್ತ)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	ಯದಿ (wip == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	wip->wi_win = win;
	ಯದಿ (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    ಅನ್ಯಥಾ
    {
	// remove the entry from the list
	ಯದಿ (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	ಅನ್ಯಥಾ
	    buf->b_wininfo = wip->wi_next;
	ಯದಿ (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	ಯದಿ (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    ಯದಿ (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    ಯದಿ (win != ರಿಕ್ತ)
	wip->wi_changelistidx = win->w_changelistidx;
    ಯದಿ (copy_options && win != ರಿಕ್ತ)
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
    wip->wi_prev = ರಿಕ್ತ;
    ಯದಿ (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    ಯದಿ (!wip->wi_opt.wo_diff)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	ಯದಿ (wip->wi_win == wp)
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಸ್ಥಿರ wininfo_T *
find_wininfo(
    buf_T	*buf,
    ಪೂರ್ಣಾಙ್ಕ		need_options,
    ಪೂರ್ಣಾಙ್ಕ		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ಯದಿ (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    ಭಙ್ಗ;

    ಯದಿ (wip != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    ಯದಿ (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    ಯದಿ (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != ರಿಕ್ತ
			    && wip->wi_win->w_buffer == buf)))
		ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ
#endif
	wip = buf->b_wininfo;
    ಪ್ರತ್ಯಾಗಮ wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    ಶೂನ್ಯ
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
    clearFolding(curwin);
#endif

    wip = find_wininfo(buf, TRUE, TRUE);
    ಯದಿ (wip != ರಿಕ್ತ && wip->wi_win != ರಿಕ್ತ
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
    ಅನ್ಯಥಾ ಯದಿ (wip != ರಿಕ್ತ && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    ಅನ್ಯಥಾ
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    ಯದಿ (wip != ರಿಕ್ತ)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    ಯದಿ (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    ಯದಿ (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    ಯದಿ (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	ಯದಿ (p_ea)
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
    ಸ್ಥಿರ pos_T no_position = {1, 0, 0};

    wip = find_wininfo(buf, FALSE, FALSE);
    ಯದಿ (wip != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ &(wip->wi_fpos);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    ಶೂನ್ಯ
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		ro_char;
    ಪೂರ್ಣಾಙ್ಕ		changed_char;
#ifdef FEAT_TERMINAL
    ಪೂರ್ಣಾಙ್ಕ		job_running;
    ಪೂರ್ಣಾಙ್ಕ		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = ರಿಕ್ತ, **p;

    ಯದಿ (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, ಪರಿಮಾಣ(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    ಯದಿ (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_data, (size_t)buflist.ga_len,
		ಪರಿಮಾಣ(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    ಪ್ರತಿ (; buf != ರಿಕ್ತ && !got_int; buf = buflist_data != ರಿಕ್ತ
	    ? (++p < buflist_data + buflist.ga_len ? *p : ರಿಕ್ತ)
	    : buf->b_next)
#ಅನ್ಯಥಾ
    ಪ್ರತಿ (buf = firstbuf; buf != ರಿಕ್ತ && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	ಯದಿ ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !bufIsChanged(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == ರಿಕ್ತ || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == ರಿಕ್ತ || buf->b_nwindows != 0))
#ifdef FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!job_running || (job_running && job_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!job_running || (job_running && !job_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (job_running || buf->b_term == ರಿಕ್ತ))
#endif
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    ಅನುವರ್ತ;
	name = buf_spname(buf);
	ಯದಿ (name != ರಿಕ್ತ)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	ಅನ್ಯಥಾ
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	ಯದಿ (message_filtered(NameBuff))
	    ಅನುವರ್ತ;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	ಯದಿ (job_running)
	{
	    ಯದಿ (job_none_open)
		ro_char = '?';
	    ಅನ್ಯಥಾ
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	ಅನ್ಯಥಾ ಯದಿ (buf->b_term != ರಿಕ್ತ)
	    ro_char = 'F';
	ಅನ್ಯಥಾ
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	ಸಂದೇಶ_putchar('\n');
	len = (ಪೂರ್ಣಾಙ್ಕ)vim_snprintf_safelen((ವರ್ಣ *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == ರಿಕ್ತ ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		changed_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	ಕುರು
	    IObuff[len++] = ' ';
	ಯಾವತ್ (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	ಯದಿ (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (size_t)(IOSIZE - len), buf->b_last_used);
	ಅನ್ಯಥಾ
#endif
	    vim_snprintf((ವರ್ಣ *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (long)buflist_findlnum(buf));
	ಸಂದೇಶ_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    ಯದಿ (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    ಪೂರ್ಣಾಙ್ಕ
buflist_name_nr(
    ಪೂರ್ಣಾಙ್ಕ		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ಯದಿ (buf == ರಿಕ್ತ || buf->b_fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    ಪೂರ್ಣಾಙ್ಕ		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = ರಿಕ್ತ;
#ifdef UNIX
    stat_T	st;
#endif

    ಯದಿ (ffname == ರಿಕ್ತ || *ffname == NUL)
    {
	// Removing the name.
	ಯದಿ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ಅನ್ಯಥಾ
	    buf->b_sfname = ರಿಕ್ತ;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    ಅನ್ಯಥಾ
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	ಯದಿ (ffname == ರಿಕ್ತ)		    // out of memory
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	ಯದಿ (mch_stat((ವರ್ಣ *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	ಯದಿ (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#ಅನ್ಯಥಾ
	    obuf = buflist_findname(ffname);
#endif
	ಯದಿ (obuf != ರಿಕ್ತ && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    ಪೂರ್ಣಾಙ್ಕ		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		ಯದಿ (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    ಯದಿ (obuf->b_ml.ml_mfp != ರಿಕ್ತ || in_use)
	    {
		ಯದಿ (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    // delete from the list
	    close_buffer(ರಿಕ್ತ, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	ಯದಿ (ffname == ರಿಕ್ತ || sfname == ರಿಕ್ತ)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	ಯದಿ (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    ಯದಿ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ಅನ್ಯಥಾ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    ಶೂನ್ಯ
buf_set_name(ಪೂರ್ಣಾಙ್ಕ fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = ರಿಕ್ತ;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    ಶೂನ್ಯ
buf_name_changed(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    ಯದಿ (buf->b_ml.ml_mfp != ರಿಕ್ತ)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    ಯದಿ (buf->b_term != ರಿಕ್ತ)
	term_clear_status_text(buf->b_term);
#endif

    ಯದಿ (curwin->w_buffer == buf)
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
    ಯದಿ (buf != ರಿಕ್ತ && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    ಪೂರ್ಣಾಙ್ಕ		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    ಯದಿ (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	ಯದಿ (errmsg)
	    emsg(_(e_no_alternate_file));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    ಪೂರ್ಣಾಙ್ಕ
buflist_add(char_u *fname, ಪೂರ್ಣಾಙ್ಕ flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, ರಿಕ್ತ, (linenr_T)0, flags);
    ಯದಿ (buf != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ buf->b_fnum;
    ಪ್ರತ್ಯಾಗಮ 0;
}

#ಯದಿ defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    ಶೂನ್ಯ
buflist_slash_adjust(ಶೂನ್ಯ)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	ಯದಿ (bp->b_ffname != ರಿಕ್ತ)
	    slash_adjust(bp->b_ffname);
	ಯದಿ (bp->b_sfname != ರಿಕ್ತ)
	    slash_adjust(bp->b_sfname);
    }
}
#endif

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    ಶೂನ್ಯ
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.col, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    ಪೂರ್ಣಾಙ್ಕ
otherfile(char_u *ffname)
{
    ಪ್ರತ್ಯಾಗಮ otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , ರಿಕ್ತ
#endif
	    );
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    ಯದಿ (ffname == ರಿಕ್ತ || *ffname == NUL || buf->b_ffname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (fnamecmp(ffname, buf->b_ffname) == 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	ಯದಿ (stp == ರಿಕ್ತ)
	{
	    ಯದಿ (!buf->b_dev_valid || mch_stat((ವರ್ಣ *)ffname, &st) < 0)
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
	ಯದಿ (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    ಯದಿ (buf_same_ino(buf, stp))
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
#endif
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ಯದಿ defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    ಶೂನ್ಯ
buf_setino(buf_T *buf)
{
    stat_T	st;

    ಯದಿ (buf->b_fname != ರಿಕ್ತ && mch_stat((ವರ್ಣ *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    ಅನ್ಯಥಾ
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    ಪ್ರತ್ಯಾಗಮ (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    ಶೂನ್ಯ
fileinfo(
    ಪೂರ್ಣಾಙ್ಕ fullname,	    // when non-zero print full path
    ಪೂರ್ಣಾಙ್ಕ shorthelp,
    ಪೂರ್ಣಾಙ್ಕ	dont_truncate)
{
    char_u	*name;
    ವರ್ಣ	*buffer;
    size_t	bufferlen = 0;

    buffer = alloc(IOSIZE);
    ಯದಿ (buffer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    ಯದಿ (name != ರಿಕ್ತ)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!fullname && curbuf->b_fname != ರಿಕ್ತ)
	    name = curbuf->b_fname;
	ಅನ್ಯಥಾ
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : ರಿಕ್ತ, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (ಪೂರ್ಣಾಙ್ಕ)bufferlen, TRUE);
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

    ಯದಿ (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_ಸಾಲುಗಳು_ಸಂದೇಶ));
    ಅನ್ಯಥಾ ಯದಿ (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ),
	    (long)curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ));
    ಅನ್ಯಥಾ
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (long)curwin->w_cursor.lnum,
	    (long)curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (ಪೂರ್ಣಾಙ್ಕ)curwin->w_cursor.col + 1, (ಪೂರ್ಣಾಙ್ಕ)curwin->w_virtcol + 1);
    }

    (ಶೂನ್ಯ)append_arg_ಸಂಖ್ಯೆ(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    ಯದಿ (dont_truncate)
    {
	ಪೂರ್ಣಾಙ್ಕ	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	ಸಂದೇಶ_start();
	n = ಸಂದೇಶ_scroll;
	ಸಂದೇಶ_scroll = TRUE;
	ಸಂದೇಶ(buffer);
	ಸಂದೇಶ_scroll = n;
    }
    ಅನ್ಯಥಾ
    {
	ವರ್ಣ	*p = ಸಂದೇಶ_trunc_attr(buffer, FALSE, 0);
	ಯದಿ (restart_edit != 0 || (ಸಂದೇಶ_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_ಸಂದೇಶ((char_u *)p, 0);
    }

    vim_free(buffer);
}

    ಪೂರ್ಣಾಙ್ಕ
col_print(
    char_u  *buf,
    size_t  buflen,
    ಪೂರ್ಣಾಙ್ಕ	    col,
    ಪೂರ್ಣಾಙ್ಕ	    vcol)
{
    ಯದಿ (col == vcol)
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)vim_snprintf_safelen((ವರ್ಣ *)buf, buflen, "%d", col);

    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)vim_snprintf_safelen((ವರ್ಣ *)buf, buflen, "%d-%d", col, vcol);
}

ಸ್ಥಿರ char_u *lasttitle = ರಿಕ್ತ;
ಸ್ಥಿರ char_u *lasticon = ರಿಕ್ತ;

/*
 * Put the file name in the title bar and icon of the window.
 */
    ಶೂನ್ಯ
maketitle(ಶೂನ್ಯ)
{
    char_u	*title_str = ರಿಕ್ತ;
    char_u	*icon_str = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    ಯದಿ (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	ಪ್ರತ್ಯಾಗಮ;
    }

    need_maketitle = FALSE;
    ಯದಿ (!p_title && !p_icon && lasttitle == ರಿಕ್ತ && lasticon == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // nothing to do

    ಯದಿ (p_title)
    {
	ಪೂರ್ಣಾಙ್ಕ maxlen = 0;

	ಯದಿ (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    ಯದಿ (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	ಯದಿ (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    ಯದಿ (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, ಪರಿಮಾಣ(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	ಅನ್ಯಥಾ
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (ಪರಿಮಾಣ(buf) - 100)
#define SPACE_FOR_DIR   (ಪರಿಮಾಣ(buf) - 20)
#define SPACE_FOR_ARGNR (ಪರಿಮಾಣ(buf) - 10)  // at least room for " - VIM"

	    // file name
	    ಯದಿ (curbuf->b_fname == ರಿಕ್ತ)
		buflen = vim_snprintf_safelen((ವರ್ಣ *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    ಅನ್ಯಥಾ ಯದಿ (curbuf->b_term != ರಿಕ್ತ)
		buflen = vim_snprintf_safelen((ವರ್ಣ *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    ಅನ್ಯಥಾ
	    {
		buflen = vim_snprintf_safelen((ವರ್ಣ *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != ರಿಕ್ತ)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    ಯದಿ (curbuf->b_term == ರಿಕ್ತ)
#endif
	    {
		ವಿಕಲ್ಪ (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    ಪಕ್ಷ 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (ವರ್ಣ *)buf + buflen,
			    ಪರಿಮಾಣ(buf) - buflen, " +");
			ಭಙ್ಗ;
		    ಪಕ್ಷ 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (ವರ್ಣ *)buf + buflen,
			    ಪರಿಮಾಣ(buf) - buflen, " =");
			ಭಙ್ಗ;
		    ಪಕ್ಷ 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (ವರ್ಣ *)buf + buflen,
			    ಪರಿಮಾಣ(buf) - buflen, " =+");
			ಭಙ್ಗ;
		    ಪಕ್ಷ 4:
		    ಪಕ್ಷ 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (ವರ್ಣ *)buf + buflen,
			    ಪರಿಮಾಣ(buf) - buflen, " -");
			ಭಙ್ಗ;
		    ಪಕ್ಷ 5:
		    ಪಕ್ಷ 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (ವರ್ಣ *)buf + buflen,
			    ಪರಿಮಾಣ(buf) - buflen, " -+");
			ಭಙ್ಗ;
		    ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}
	    }

	    // path (surrounded by '()')
	    ಯದಿ (curbuf->b_fname != ರಿಕ್ತ
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == ರಿಕ್ತ
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((ವರ್ಣ *)buf + buflen,
		    ಪರಿಮಾಣ(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (ಪೂರ್ಣಾಙ್ಕ)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		ಯದಿ (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		ಯದಿ (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((ವರ್ಣ *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		ಅನ್ಯಥಾ
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    ಯದಿ (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((ವರ್ಣ *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != ರಿಕ್ತ)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    ಅನ್ಯಥಾ
			buflen += vim_snprintf_safelen((ವರ್ಣ *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_ಸಂಖ್ಯೆ(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((ವರ್ಣ *)buf + buflen,
		ಪರಿಮಾಣ(buf) - buflen, " - %s",
#ಯದಿ defined(FEAT_CLIENTSERVER)
		(serverName != ರಿಕ್ತ)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    ಯದಿ (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		ಯದಿ (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, ಪರಿಮಾಣ(buf));
	    }
	}
    }
    mustset = ಮೌಲ್ಯ_changed(title_str, &lasttitle);

    ಯದಿ (p_icon)
    {
	icon_str = buf;
	ಯದಿ (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    ಯದಿ (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, ಪರಿಮಾಣ(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, ರಿಕ್ತ, ರಿಕ್ತ,
				 ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
#endif
		icon_str = p_iconstring;
	}
	ಅನ್ಯಥಾ
	{
	    char_u  *name;
	    ಪೂರ್ಣಾಙ್ಕ	    namelen;

	    name = buf_spname(curbuf);
	    ಯದಿ (name == ರಿಕ್ತ)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(name);
	    ಯದಿ (namelen > 100)
	    {
		namelen -= 100;
		ಯದಿ (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, ಪರಿಮಾಣ(buf));
	}
    }

    mustset |= ಮೌಲ್ಯ_changed(icon_str, &lasticon);

    ಯದಿ (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ಮೌಲ್ಯ_changed(char_u *str, char_u **last)
{
    ಯದಿ ((str == ರಿಕ್ತ) != (*last == ರಿಕ್ತ)
	    || (str != ರಿಕ್ತ && *last != ರಿಕ್ತ && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	ಯದಿ (str == ರಿಕ್ತ)
	{
	    *last = ರಿಕ್ತ;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	ಅನ್ಯಥಾ
	{
	    *last = vim_strsave(str);
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    ಶೂನ್ಯ
resettitle(ಶೂನ್ಯ)
{
    mch_settitle(lasttitle, lasticon);
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
free_titles(ಶೂನ್ಯ)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#ಯದಿ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    char_u	*stl_start;
    ಪೂರ್ಣಾಙ್ಕ		stl_minwid;
    ಪೂರ್ಣಾಙ್ಕ		stl_maxwid;
    ಗಣನಾ {
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

ಸ್ಥಿರ size_t		stl_ವಸ್ತುಗಳು_len = 20; // Initial value, grows as needed.
ಸ್ಥಿರ stl_item_T      *stl_ವಸ್ತುಗಳು = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	       *stl_groupitem = ರಿಕ್ತ;
ಸ್ಥಿರ stl_hlrec_T     *stl_hltab = ರಿಕ್ತ;
ಸ್ಥಿರ stl_hlrec_T     *stl_tabtab = ರಿಕ್ತ;
ಸ್ಥಿರ stl_clickrec_T  *stl_clicktab = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		*stl_separator_locations = ರಿಕ್ತ;

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
    ಪೂರ್ಣಾಙ್ಕ
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ಪೂರ್ಣಾಙ್ಕ		opt_scope,	// scope for "opt_name"
    ಪೂರ್ಣಾಙ್ಕ		fillchar,
    ಪೂರ್ಣಾಙ್ಕ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    ಪ್ರತ್ಯಾಗಮ build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ರಿಕ್ತ, ರಿಕ್ತ);
}

    ಪೂರ್ಣಾಙ್ಕ
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    ಪೂರ್ಣಾಙ್ಕ		opt_scope,	// scope for "opt_name"
    ಪೂರ್ಣಾಙ್ಕ		fillchar,
    ಪೂರ್ಣಾಙ್ಕ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ಪೂರ್ಣಾಙ್ಕ		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ಪ್ರತ್ಯಾಗಮ build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ರಿಕ್ತ, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    ಪೂರ್ಣಾಙ್ಕ
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    ಪೂರ್ಣಾಙ್ಕ		opt_scope,	// scope for "opt_name"
    ಪೂರ್ಣಾಙ್ಕ		fillchar,
    ಪೂರ್ಣಾಙ್ಕ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ಪೂರ್ಣಾಙ್ಕ		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ಪ್ರತ್ಯಾಗಮ build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ರಿಕ್ತ, carry_hl);
}
# endif

    ಪೂರ್ಣಾಙ್ಕ
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ಪೂರ್ಣಾಙ್ಕ		opt_scope)	// scope for "opt_name"
{
    ಪೂರ್ಣಾಙ್ಕ rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (ಶೂನ್ಯ)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, ಪರಿಮಾಣ(buf), &fmt,
	    opt_name, opt_scope, 0, 0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, &rendered_height,
	    ರಿಕ್ತ);
    --emsg_off;
    ಪ್ರತ್ಯಾಗಮ rendered_height;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ಪೂರ್ಣಾಙ್ಕ		opt_scope,	// scope for "opt_name"
    ಪೂರ್ಣಾಙ್ಕ		fillchar,
    ಪೂರ್ಣಾಙ್ಕ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ಪೂರ್ಣಾಙ್ಕ		*rendered_height,   // return: stl rendered height (can be NULL)
    ಪೂರ್ಣಾಙ್ಕ		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    ಪೂರ್ಣಾಙ್ಕ		byteval;
# ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ		use_sandbox;
    ಪೂರ್ಣಾಙ್ಕ		save_VIsual_active;
# endif
    ಪೂರ್ಣಾಙ್ಕ		empty_ಸಾಲು;
    long	l;
    long	n;
    ಪೂರ್ಣಾಙ್ಕ		prevchar_isflag;
    ಪೂರ್ಣಾಙ್ಕ		prevchar_isitem;
    ಪೂರ್ಣಾಙ್ಕ		itemisflag;
    ಪೂರ್ಣಾಙ್ಕ		fillable;
    char_u	*str;
    long	ಅಂಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		width;
    ಪೂರ್ಣಾಙ್ಕ		itemcnt;
    ಪೂರ್ಣಾಙ್ಕ		curitem;
    ಪೂರ್ಣಾಙ್ಕ		groupdepth;
# ifdef FEAT_EVAL
    ಪೂರ್ಣಾಙ್ಕ		evaldepth;
# endif
    ಪೂರ್ಣಾಙ್ಕ		minwid;
    ಪೂರ್ಣಾಙ್ಕ		maxwid;
    ಪೂರ್ಣಾಙ್ಕ		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    ಪೂರ್ಣಾಙ್ಕ		save_redraw_not_allowed = redraw_not_allowed;
    ಪೂರ್ಣಾಙ್ಕ		save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;
    ಪೂರ್ಣಾಙ್ಕ		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    ಯದಿ (updating_screen)
	redraw_not_allowed = TRUE;

    ಯದಿ (stl_ವಸ್ತುಗಳು == ರಿಕ್ತ)
    {
	stl_ವಸ್ತುಗಳು = ALLOC_MULT(stl_item_T, stl_ವಸ್ತುಗಳು_len);
	stl_groupitem = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, stl_ವಸ್ತುಗಳು_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_ವಸ್ತುಗಳು_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_ವಸ್ತುಗಳು_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_ವಸ್ತುಗಳು_len + 1);

	stl_separator_locations = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, stl_ವಸ್ತುಗಳು_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    ಯದಿ (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_ಸಂಖ್ಯೆ;
	tv.vval.v_ಸಂಖ್ಯೆ = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	ಯದಿ (usefmt == ರಿಕ್ತ)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    ಯದಿ (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    ಯದಿ (lnum > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	lnum = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_ಸಾಲು = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    ಯದಿ (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    ಅನ್ಯಥಾ
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    ಯದಿ (carry_hl != ರಿಕ್ತ && *carry_hl != 0)
    {
	stl_ವಸ್ತುಗಳು[curitem].stl_type = Highlight;
	stl_ವಸ್ತುಗಳು[curitem].stl_start = p;
	stl_ವಸ್ತುಗಳು[curitem].stl_minwid = *carry_hl;
	stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc = ರಿಕ್ತ;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    ಪ್ರತಿ (s = usefmt; *s != NUL; )
    {
	ಯದಿ (curitem == (ಪೂರ್ಣಾಙ್ಕ)stl_ವಸ್ತುಗಳು_len)
	{
	    size_t	new_len = stl_ವಸ್ತುಗಳು_len * 3 / 2;

	    stl_item_T *new_ವಸ್ತುಗಳು =
			  vim_realloc(stl_ವಸ್ತುಗಳು, ಪರಿಮಾಣ(stl_item_T) * new_len);
	    ಯದಿ (new_ವಸ್ತುಗಳು == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    stl_ವಸ್ತುಗಳು = new_ವಸ್ತುಗಳು;

	    ಪೂರ್ಣಾಙ್ಕ *new_groupitem =
			     vim_realloc(stl_groupitem, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * new_len);
	    ಯದಿ (new_groupitem == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  ಪರಿಮಾಣ(stl_hlrec_T) * (new_len + 1));
	    ಯದಿ (new_hlrec == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  ಪರಿಮಾಣ(stl_hlrec_T) * (new_len + 1));
	    ಯದಿ (new_hlrec == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      ಪರಿಮಾಣ(stl_clickrec_T) * (new_len + 1));
	    ಯದಿ (new_clickrec == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    stl_clicktab = new_clickrec;

	    ಪೂರ್ಣಾಙ್ಕ *new_separator_locs = vim_realloc(stl_separator_locations,
					    ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * new_len);
	    ಯದಿ (new_separator_locs == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    stl_separator_locations = new_separator_locs;

	    stl_ವಸ್ತುಗಳು_len = new_len;
	}

	stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc = ರಿಕ್ತ;

	ಯದಿ (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	ಯಾವತ್ (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    ಯದಿ (*s == '\n' || *s == '\r')
	    {
		ಯದಿ (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    ಗಚ್ಛ find_linebreak;
		}
		ಅನ್ಯಥಾ ಯದಿ (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	ಯದಿ (*s == NUL || p + 1 >= out + outlen)
	    ಭಙ್ಗ;

	/*
	 * Handle one '%' item.
	 */
	s++;
	ಯದಿ (*s == NUL)  // ignore trailing %
	    ಭಙ್ಗ;

	ಯದಿ (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    ಯದಿ (s[1] == ']')
	    {
		stl_ವಸ್ತುಗಳು[curitem].stl_type = ClickFunc;
		stl_ವಸ್ತುಗಳು[curitem].stl_start = p;
		stl_ವಸ್ತುಗಳು[curitem].stl_minwid = 0;
		stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc = ರಿಕ್ತ;
		s += 2;
		curitem++;
		ಅನುವರ್ತ;
	    }
	    // %[FuncName] - start click region
	    ಯದಿ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		ಯದಿ (rb != ರಿಕ್ತ)
		{
		    stl_ವಸ್ತುಗಳು[curitem].stl_type = ClickFunc;
		    stl_ವಸ್ತುಗಳು[curitem].stl_start = p;
		    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = 0;
		    stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    ಅನುವರ್ತ;
		}
	    }
	}
	ಯದಿ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    ಯದಿ (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	ಯದಿ (*s == '%')
	{
	    ಯದಿ (p + 1 >= out + outlen)
		ಭಙ್ಗ;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    ಅನುವರ್ತ;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	ಯದಿ (*s == STL_SEPARATE)
	{
	    s++;
	    ಯದಿ (groupdepth > 0)
		ಅನುವರ್ತ;
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = Separate;
	    stl_ವಸ್ತುಗಳು[curitem++].stl_start = p;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = Trunc;
	    stl_ವಸ್ತುಗಳು[curitem++].stl_start = p;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (*s == ')')
	{
	    char_u  *t;

	    s++;
	    ಯದಿ (groupdepth < 1)
		ಅನುವರ್ತ;
	    groupdepth--;

	    t = stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    ಯದಿ (curitem > stl_groupitem[groupdepth] + 1
		    && stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ group_start_userhl = 0;
		ಪೂರ್ಣಾಙ್ಕ group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		ಪ್ರತಿ (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    ಯದಿ (stl_ವಸ್ತುಗಳು[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_ವಸ್ತುಗಳು[n].stl_minwid;
			ಭಙ್ಗ;
		    }
		}
		ಪ್ರತಿ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    ಯದಿ (stl_ವಸ್ತುಗಳು[n].stl_type == Normal)
			ಭಙ್ಗ;
		    ಯದಿ (stl_ವಸ್ತುಗಳು[n].stl_type == Highlight)
			group_end_userhl = stl_ವಸ್ತುಗಳು[n].stl_minwid;
		}
		ಯದಿ (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    ಪ್ರತಿ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			ಯದಿ (stl_ವಸ್ತುಗಳು[n].stl_type == Highlight)
			    stl_ವಸ್ತುಗಳು[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			ಯದಿ (stl_ವಸ್ತುಗಳು[n].stl_type == TabPage)
			    stl_ವಸ್ತುಗಳು[n].stl_start = p;
		    }
		}
	    }
	    ಯದಿ (l > stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		ಯದಿ (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    ಯಾವತ್ (l >= stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		ಅನ್ಯಥಾ
		    n = (long)(p - t) - stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		ಪೂರ್ಣಾಙ್ಕ minwid_fixed = MIN(stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_minwid,
				       stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_maxwid);
		ಯಾವತ್ (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		ಪ್ರತಿ (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_ವಸ್ತುಗಳು[l].stl_start -= n - 1;
		    ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_start < t)
			stl_ವಸ್ತುಗಳು[l].stl_start = t;
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ (abs(stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_minwid;
		ಪೂರ್ಣಾಙ್ಕ fillchar_len = MB_CHAR2LEN(fillchar);
		ಯದಿ (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    ಯಾವತ್ (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		ಅನ್ಯಥಾ
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    ಯದಿ (p + l >= out + outlen)
		    {
			n = (long)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    ಪ್ರತಿ ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    ಪ್ರತಿ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_ವಸ್ತುಗಳು[n].stl_start += l;
		}
	    }
	    ಅನುವರ್ತ;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	ಯದಿ (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	ಯದಿ (*s == '-')
	{
	    s++;
	    l = -1;
	}
	ಯದಿ (VIM_ISDIGIT(*s))
	{
	    minwid = (ಪೂರ್ಣಾಙ್ಕ)getdigits(&s);
	    ಯದಿ (minwid < 0)	// overflow
		minwid = 0;
	}
	ಯದಿ (*s == STL_USER_HL)
	{
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = Highlight;
	    stl_ವಸ್ತುಗಳು[curitem].stl_start = p;
	    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    ಯದಿ (*s == STL_TABCLOSENR)
	    {
		ಯದಿ (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    ಪ್ರತಿ (n = curitem - 1; n >= 0; --n)
			ಯದಿ (stl_ವಸ್ತುಗಳು[n].stl_type == TabPage
					       && stl_ವಸ್ತುಗಳು[n].stl_minwid >= 0)
			{
			    minwid = stl_ವಸ್ತುಗಳು[n].stl_minwid;
			    ಭಙ್ಗ;
			}
		}
		ಅನ್ಯಥಾ
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = TabPage;
	    stl_ವಸ್ತುಗಳು[curitem].stl_start = p;
	    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (*s == '.')
	{
	    s++;
	    ಯದಿ (VIM_ISDIGIT(*s))
	    {
		maxwid = (ಪೂರ್ಣಾಙ್ಕ)getdigits(&s);
		ಯದಿ (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	ಪೂರ್ಣಾಙ್ಕ raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	ಯದಿ (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = Group;
	    stl_ವಸ್ತುಗಳು[curitem].stl_start = p;
	    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = minwid;
	    stl_ವಸ್ತುಗಳು[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    ಅನುವರ್ತ;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	ಯದಿ (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    ಅನುವರ್ತ;
	}
# endif
	ಯದಿ (vim_strchr(STL_ALL, *s) == ರಿಕ್ತ)
	{
	    ಯದಿ (*s == NUL)  // can happen with "%0"
		ಭಙ್ಗ;
	    s++;
	    ಅನುವರ್ತ;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	ಅಂಕೆ = -1;
	str = ರಿಕ್ತ;
	ವಿಕಲ್ಪ (opt)
	{
	ಪಕ್ಷ STL_FILEPATH:
	ಪಕ್ಷ STL_FULLPATH:
	ಪಕ್ಷ STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    ಯದಿ (name != ರಿಕ್ತ)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    ಅನ್ಯಥಾ
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    ಯದಿ (opt != STL_FILENAME)
		str = NameBuff;
	    ಅನ್ಯಥಾ
		str = gettail(NameBuff);
	    ಭಙ್ಗ;
	}

	ಪಕ್ಷ STL_VIM_ಅಭಿವ್ಯಕ್ತಿ: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    ಪೂರ್ಣಾಙ್ಕ	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    ಯದಿ (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    ಯಾವತ್ ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    ಯದಿ (*s != '}')	// missing '}' or out of space
		ಭಙ್ಗ;
	    s++;
	    ಯದಿ (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    ಅನ್ಯಥಾ
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((ವರ್ಣ *)buf_tmp, ಪರಿಮಾಣ(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((ವರ್ಣ *)buf_tmp, ಪರಿಮಾಣ(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    ಯದಿ (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    ಯದಿ (!zeropad && str != ರಿಕ್ತ && *str != NUL)
	    {
		ಯದಿ (*skipdigits(str) == NUL)
		{
		    ಅಂಕೆ = atoi((ವರ್ಣ *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    ಯದಿ (reevaluate && str != ರಿಕ್ತ && *str != NUL
		    && strchr((ಧ್ರುವ ವರ್ಣ *)str, '%') != ರಿಕ್ತ
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((ಧ್ರುವ ವರ್ಣ *)str);
		size_t fmt_length = strlen((ಧ್ರುವ ವರ್ಣ *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * ಪರಿಮಾಣ(char_u));

		ಯದಿ (new_fmt != ರಿಕ್ತ)
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
		    new_fmt_p = ರಿಕ್ತ;

		    ಯದಿ (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    ಅನುವರ್ತ;
		}
	    }
# endif
	    ಭಙ್ಗ;
	}
	ಪಕ್ಷ STL_ಸಾಲು:
	    ಅಂಕೆ = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_NUMLINES:
	    ಅಂಕೆ = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_COLUMN:
	    ಅಂಕೆ = (State & MODE_ಸೇರಿಸು) == 0 && empty_ಸಾಲು
					       ? 0 : (ಪೂರ್ಣಾಙ್ಕ)wp->w_cursor.col + 1;
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_VIRTCOL:
	ಪಕ್ಷ STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    ಯದಿ (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_ಸೇರಿಸು) == 0
			       && empty_ಸಾಲು ? 0 : (ಪೂರ್ಣಾಙ್ಕ)wp->w_cursor.col + 1)))
		ಭಙ್ಗ;
	    ಅಂಕೆ = (long)virtcol;
	    ಭಙ್ಗ;
	}

	ಪಕ್ಷ STL_PERCENTAGE:
	    ಅಂಕೆ = calc_percentage((long)wp->w_cursor.lnum, (long)wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_ALTPERCENT:
	    str = buf_tmp;
	    (ಶೂನ್ಯ)get_rel_pos(wp, str, TMPLEN);
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_SHOWCMD:
	    ಯದಿ (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    ಯದಿ (append_arg_ಸಂಖ್ಯೆ(wp, buf_tmp, ಪರಿಮಾಣ(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_KEYMAP:
	    fillable = FALSE;
	    ಯದಿ (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    ಭಙ್ಗ;
	ಪಕ್ಷ STL_PAGENUM:
# ಯದಿ defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    ಅಂಕೆ = printer_page_ಅಂಕೆ;
# ಅನ್ಯಥಾ
	    ಅಂಕೆ = 0;
# endif
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_BUFNO:
	    ಅಂಕೆ = wp->w_buffer->b_fnum;
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	ಪಕ್ಷ STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_ಸಾಲು_or_offset(wp->w_buffer, wp->w_cursor.lnum, ರಿಕ್ತ);
	    ಅಂಕೆ = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_ಸೇರಿಸು) == 0 && empty_ಸಾಲು
				? 0 : (ಪೂರ್ಣಾಙ್ಕ)wp->w_cursor.col);
# endif
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	ಪಕ್ಷ STL_BYTEVAL:
	    ಅಂಕೆ = byteval;
	    ಯದಿ (ಅಂಕೆ == NL)
		ಅಂಕೆ = 0;
	    ಅನ್ಯಥಾ ಯದಿ (ಅಂಕೆ == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		ಅಂಕೆ = NL;
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_ROFLAG:
	ಪಕ್ಷ STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    ಯದಿ (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_HELPFLAG:
	ಪಕ್ಷ STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    ಯದಿ (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_FILETYPE:
	    ಯದಿ (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((ವರ್ಣ *)buf_tmp, ಪರಿಮಾಣ(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    ಯದಿ (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((ವರ್ಣ *)buf_tmp, ಪರಿಮಾಣ(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		ಪ್ರತಿ (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    ಭಙ್ಗ;

# ಯದಿ defined(FEAT_QUICKFIX)
	ಪಕ್ಷ STL_PREVIEWFLAG:
	ಪಕ್ಷ STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    ಯದಿ (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_QUICKFIX:
	    ಯದಿ (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(ಸಂದೇಶ_loclist)
			    : _(ಸಂದೇಶ_qflist));
	    ಭಙ್ಗ;
# endif

	ಪಕ್ಷ STL_MODIFIED:
	ಪಕ್ಷ STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    ವಿಕಲ್ಪ ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		ಪಕ್ಷ 2: str = (char_u *)"[+]"; ಭಙ್ಗ;
		ಪಕ್ಷ 3: str = (char_u *)",+"; ಭಙ್ಗ;
		ಪಕ್ಷ 4: str = (char_u *)"[-]"; ಭಙ್ಗ;
		ಪಕ್ಷ 5: str = (char_u *)",-"; ಭಙ್ಗ;
		ಪಕ್ಷ 6: str = (char_u *)"[+-]"; ಭಙ್ಗ;
		ಪಕ್ಷ 7: str = (char_u *)",+-"; ಭಙ್ಗ;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		ಯಾವತ್ (*s != '#' && *s != NUL)
		    ++s;
		ಯದಿ (*s == '#')
		{
		    stl_ವಸ್ತುಗಳು[curitem].stl_type = Highlight;
		    stl_ವಸ್ತುಗಳು[curitem].stl_start = p;
		    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = -syn_namen2id(t, (ಪೂರ್ಣಾಙ್ಕ)(s - t));
		    curitem++;
		}
		ಯದಿ (*s != NUL)
		    ++s;
		ಅನುವರ್ತ;
	    }

	ಪಕ್ಷ STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    ಯದಿ (*s == ']')
	    {
		stl_ವಸ್ತುಗಳು[curitem].stl_type = ClickFunc;
		stl_ವಸ್ತುಗಳು[curitem].stl_start = p;
		stl_ವಸ್ತುಗಳು[curitem].stl_minwid = 0;
		stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc = ರಿಕ್ತ;
		s++;
		curitem++;
		ಅನುವರ್ತ;
	    }
	    // %N[FuncName] with minwid
	    ಯದಿ (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		ಯದಿ (rb != ರಿಕ್ತ)
		{
		    stl_ವಸ್ತುಗಳು[curitem].stl_type = ClickFunc;
		    stl_ವಸ್ತುಗಳು[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = raw_minwid;
		    stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    ಅನುವರ್ತ;
		}
	    }
	    ಅನುವರ್ತ;

	ಪಕ್ಷ STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    ಅನುವರ್ತ;
	}

	stl_ವಸ್ತುಗಳು[curitem].stl_start = p;
	stl_ವಸ್ತುಗಳು[curitem].stl_type = Normal;
	ಯದಿ (str != ರಿಕ್ತ && *str)
	{
	    char_u  *t = str;

	    ಯದಿ (itemisflag)
	    {
		ಯದಿ ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    ಯದಿ (l > 0)
		prevchar_isitem = TRUE;
	    ಯದಿ (l > maxwid)
	    {
		ಯಾವತ್ (l >= maxwid)
		    ಯದಿ (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    ಅನ್ಯಥಾ
			l -= byte2cells(*t++);
		ಯದಿ (p + 1 >= out + outlen)
		    ಭಙ್ಗ;
		*p++ = '<';
	    }
	    ಯದಿ (minwid > 0)
	    {
		ಪ್ರತಿ (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    ಯದಿ (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    ಅನ್ಯಥಾ
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    ಅನ್ಯಥಾ
		minwid *= -1;
	    ಪ್ರತಿ (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		ಯದಿ (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		ಅನ್ಯಥಾ
		    *p++ = *t;
	    }
	    ಪ್ರತಿ (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	ಅನ್ಯಥಾ ಯದಿ (ಅಂಕೆ >= 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    ಯದಿ (p + 20 >= out + outlen)
		ಭಙ್ಗ;		// not sufficient space
	    prevchar_isitem = TRUE;
	    ಯದಿ (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    ಯದಿ (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    ಪ್ರತಿ (n = ಅಂಕೆ, l = 1; n >= nbase; n /= nbase)
		l++;
	    ಯದಿ (opt == STL_VIRTCOL_ALT)
		l++;

	    ಯದಿ (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		ಯಾವತ್ (l-- > maxwid)
		    ಅಂಕೆ /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((ವರ್ಣ *)p, outlen - (p - out),
		    (ವರ್ಣ *)nstr, 0, ಅಂಕೆ, n);
	    }
	    ಅನ್ಯಥಾ
		p += vim_snprintf_safelen((ವರ್ಣ *)p, outlen - (p - out),
		    (ವರ್ಣ *)nstr, minwid, ಅಂಕೆ);
	}
	ಅನ್ಯಥಾ
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = Empty;

	ಯದಿ (ಅಂಕೆ >= 0 || (!itemisflag && str != ರಿಕ್ತ && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	ಯದಿ (opt == STL_VIM_ಅಭಿವ್ಯಕ್ತಿ)
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
    ಯದಿ (carry_hl != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ last_hl = 0;
	ಪ್ರತಿ (l = 0; l < itemcnt; l++)
	    ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_type == Highlight)
		last_hl = stl_ವಸ್ತುಗಳು[l].stl_minwid;
	*carry_hl = last_hl;
    }

    ಯದಿ (mode == STL_MODE_MULTI
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
	ಯದಿ (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = strlen((ವರ್ಣ *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    ಅನ್ಯಥಾ
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	ಯದಿ (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    ಯದಿ (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	ಪ್ರತಿ (l = 0; l < itemcnt; l++)
	    ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_type == ClickFunc)
		vim_free(stl_ವಸ್ತುಗಳು[l].stl_clickfunc);
	ಯದಿ (rendered_height != ರಿಕ್ತ)
	    *rendered_height = rheight;
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    width = vim_strsize(out);
    ಯದಿ (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	ಯದಿ (itemcnt == 0)
	    s = out;
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ ( ; l < itemcnt; l++)
		ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_ವಸ್ತುಗಳು[l].stl_start;
		    ಭಙ್ಗ;
		}
	    ಯದಿ (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_ವಸ್ತುಗಳು[0].stl_start;
		l = 0;
	    }
	}

	ಯದಿ (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    ಯದಿ (has_mbyte)
	    {
		s = out;
		width = 0;
		ಪ್ರತಿ (;;)
		{
		    width += ptr2cells(s);
		    ಯದಿ (width >= maxwidth)
			ಭಙ್ಗ;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		ಯಾವತ್ (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    ಅನ್ಯಥಾ
		s = out + maxwidth - 1;
	    ಪ್ರತಿ (l = 0; l < itemcnt; l++)
		ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_start > s)
		    ಭಙ್ಗ;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	ಅನ್ಯಥಾ
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    ಯಾವತ್ (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(end - p) + 1);	// +1 for NUL
	    end -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    ಪ್ರತಿ (; l < itemcnt; l++)
	    {
		ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_start - n >= s)
		    stl_ವಸ್ತುಗಳು[l].stl_start -= n;
		ಅನ್ಯಥಾ
		    stl_ವಸ್ತುಗಳು[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    ಯಾವತ್ (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    ಅನ್ಯಥಾ ಯದಿ (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_separators = 0;

	ಪ್ರತಿ (l = 0; l < itemcnt; l++)
	{
	    ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[ಅಂಕೆ_separators] = l;
		ಅಂಕೆ_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	ಯದಿ (ಅಂಕೆ_separators)
	{
	    ಪೂರ್ಣಾಙ್ಕ standard_spaces;
	    ಪೂರ್ಣಾಙ್ಕ final_spaces;

	    standard_spaces = (maxwidth - width) / ಅಂಕೆ_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (ಅಂಕೆ_separators - 1);
	    ಪ್ರತಿ (l = 0; l < ಅಂಕೆ_separators; l++)
	    {
		ಪೂರ್ಣಾಙ್ಕ dislocation = (l == (ಅಂಕೆ_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_ವಸ್ತುಗಳು[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		ಪ್ರತಿ (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_ವಸ್ತುಗಳು[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    ಯದಿ (hltab != ರಿಕ್ತ)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	ಪ್ರತಿ (l = 0; l < itemcnt; l++)
	{
	    ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_type == Highlight)
	    {
		sp->start = stl_ವಸ್ತುಗಳು[l].stl_start;
		sp->userhl = stl_ವಸ್ತುಗಳು[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = ರಿಕ್ತ;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    ಯದಿ (tabtab != ರಿಕ್ತ)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	ಪ್ರತಿ (l = 0; l < itemcnt; l++)
	{
	    ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_type == TabPage)
	    {
		sp->start = stl_ವಸ್ತುಗಳು[l].stl_start;
		sp->userhl = stl_ವಸ್ತುಗಳು[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = ರಿಕ್ತ;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    ಯದಿ (clicktab != ರಿಕ್ತ)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	ಪ್ರತಿ (l = 0; l < itemcnt; l++)
	{
	    ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_type == ClickFunc)
	    {
		cp->start = stl_ವಸ್ತುಗಳು[l].stl_start;
		cp->funcname = stl_ವಸ್ತುಗಳು[l].stl_clickfunc;
		cp->minwid = stl_ವಸ್ತುಗಳು[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = ರಿಕ್ತ;
	cp->funcname = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ
    {
	// Free click function names when caller doesn't need them.
	ಪ್ರತಿ (l = 0; l < itemcnt; l++)
	    ಯದಿ (stl_ವಸ್ತುಗಳು[l].stl_type == ClickFunc)
		vim_free(stl_ವಸ್ತುಗಳು[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    ಯದಿ (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    ಪ್ರತ್ಯಾಗಮ width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		buflen)
{
    long	above; // number of lines above window
    long	below; // number of lines below window

    ಯದಿ (buflen < 3) // need at least 3 chars for writing
	ಪ್ರತ್ಯಾಗಮ 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    ಯದಿ (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - wp->w_botline + 1;
    ಯದಿ (below <= 0)
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)vim_snprintf_safelen((ವರ್ಣ *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    ಯದಿ (above <= 0)
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)vim_snprintf_safelen((ವರ್ಣ *)buf, buflen,
	    "%s", _("Top"));

    ಪೂರ್ಣಾಙ್ಕ perc = calc_percentage(above, above + below);
    ವರ್ಣ tmp[8];
    // localized percentage value
    vim_snprintf(tmp, ಪರಿಮಾಣ(tmp), _("%d%%"), perc);
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)vim_snprintf_safelen((ವರ್ಣ *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
append_arg_ಸಂಖ್ಯೆ(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    ಪೂರ್ಣಾಙ್ಕ		add_file)	// Add "file" before the arg number
{
    ಯದಿ (ARGCOUNT <= 1)		// nothing to do
	ಪ್ರತ್ಯಾಗಮ 0;

    ವರ್ಣ *ಸಂದೇಶ;
    ವಿಕಲ್ಪ ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	ಪಕ್ಷ 0: ಸಂದೇಶ = _(" (%d of %d)"); ಭಙ್ಗ;
	ಪಕ್ಷ 1: ಸಂದೇಶ = _(" ((%d) of %d)"); ಭಙ್ಗ;
	ಪಕ್ಷ 2: ಸಂದೇಶ = _(" (file %d of %d)"); ಭಙ್ಗ;
	ಪಕ್ಷ 3: ಸಂದೇಶ = _(" (file (%d) of %d)"); ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)vim_snprintf_safelen((ವರ್ಣ *)buf, buflen, ಸಂದೇಶ,
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
    ಪ್ರತ್ಯಾಗಮ FullName_save(fname, TRUE);
#ಅನ್ಯಥಾ
    ಯದಿ (!vim_isAbsName(fname)
	    || strstr((ವರ್ಣ *)fname, "..") != ರಿಕ್ತ
	    || strstr((ವರ್ಣ *)fname, "//") != ರಿಕ್ತ
# ifdef BACKSLASH_IN_FILENAME
	    || strstr((ವರ್ಣ *)fname, "\\\\") != ರಿಕ್ತ
# endif
# ಯದಿ defined(MSWIN)
	    || vim_strchr(fname, '~') != ರಿಕ್ತ
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	ಪ್ರತ್ಯಾಗಮ FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    ಯದಿ (fname != ರಿಕ್ತ)
	fname_case(fname, 0);	// set correct case for file name
# endif

    ಪ್ರತ್ಯಾಗಮ fname;
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
    ಶೂನ್ಯ
fname_expand(
    buf_T	*buf UNUSED,
    char_u	**ffname,
    char_u	**sfname)
{
    ಯದಿ (*ffname == ರಿಕ್ತ)	    // no file name given, nothing to do
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (*sfname == ರಿಕ್ತ)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    ಯದಿ (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	ಯದಿ (rfname != ರಿಕ್ತ)
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
    ಶೂನ್ಯ
ex_buffer_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    ಪೂರ್ಣಾಙ್ಕ		split_ret = OK;
    ಪೂರ್ಣಾಙ್ಕ		p_ea_save;
    ಪೂರ್ಣಾಙ್ಕ		open_wins = 0;
    ಪೂರ್ಣಾಙ್ಕ		r;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;		// Maximum number of windows to open.
    ಪೂರ್ಣಾಙ್ಕ		all;		// When TRUE also load inactive buffers.
    ಪೂರ್ಣಾಙ್ಕ		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)	// make as many windows as possible
	ಎಣಿಕೆ = 9999;
    ಅನ್ಯಥಾ
	ಎಣಿಕೆ = eap->line2;	// make as many windows as specified
    ಯದಿ (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    ಅನ್ಯಥಾ
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
    ಯದಿ (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    ಪ್ರತಿ (;;)
    {
	tpnext = curtab->tp_next;
	ಪ್ರತಿ (wp = firstwin; wp != ರಿಕ್ತ; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    ಯದಿ ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		ಯದಿ (win_close(wp, FALSE) == FAIL)
		    ಭಙ್ಗ;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    ಅನ್ಯಥಾ
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	ಯದಿ (had_tab == 0 || tpnext == ರಿಕ್ತ)
	    ಭಙ್ಗ;
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
    ಪ್ರತಿ (buf = firstbuf; buf != ರಿಕ್ತ && open_wins < ಎಣಿಕೆ; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	ಯದಿ ((!all && buf->b_ml.ml_mfp == ರಿಕ್ತ) || !buf->b_p_bl)
	    ಅನುವರ್ತ;

	ಯದಿ (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    ಯದಿ (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    ಅನ್ಯಥಾ
		wp = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		ಯದಿ (wp->w_buffer == buf)
		    ಭಙ್ಗ;
	    // If the buffer already has a window, move it
	    ಯದಿ (wp != ರಿಕ್ತ)
		win_move_after(wp, curwin);
	}

	ಯದಿ (wp == ರಿಕ್ತ && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    ಯದಿ (split_ret == FAIL)
		ಅನುವರ್ತ;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    ಯದಿ (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (swap_exists_action == SEA_QUIT)
	    {
#ಯದಿ defined(FEAT_EVAL)
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

#ಯದಿ defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    ಅನ್ಯಥಾ
		handle_swap_exists(ರಿಕ್ತ);
	}

	ui_breakcheck();
	ಯದಿ (got_int)
	{
	    (ಶೂನ್ಯ)vgetc();	// only break the file loading, not the rest
	    ಭಙ್ಗ;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	ಯದಿ (aborting())
	    ಭಙ್ಗ;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	ಯದಿ (had_tab > 0 && tabpage_index(ರಿಕ್ತ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    ಪ್ರತಿ (wp = lastwin; open_wins > ಎಣಿಕೆ; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	ಯದಿ (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	ಅನ್ಯಥಾ ಯದಿ (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	ಅನ್ಯಥಾ
	{
	    wp = wp->w_prev;
	    ಯದಿ (wp == ರಿಕ್ತ)
		ಭಙ್ಗ;
	}
    }
}


ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  chk_modeline(linenr_T, ಪೂರ್ಣಾಙ್ಕ);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    ಶೂನ್ಯ
do_modelines(ಪೂರ್ಣಾಙ್ಕ flags)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		nmlines;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	entered = 0;

    ಯದಿ (!curbuf->b_p_ml || (nmlines = (ಪೂರ್ಣಾಙ್ಕ)p_mls) == 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    ಯದಿ (entered)
	ಪ್ರತ್ಯಾಗಮ;

    ++entered;
    ಪ್ರತಿ (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ && lnum <= nmlines;
								       ++lnum)
	ಯದಿ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    ಪ್ರತಿ (lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - nmlines; --lnum)
	ಯದಿ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#ಅನ್ತರ್ಭಾವ "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
chk_modeline(
    linenr_T	lnum,
    ಪೂರ್ಣಾಙ್ಕ		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*ಸಾಲು_end;		// point to the end of the line
    char_u	*e;
    ಪೂರ್ಣಾಙ್ಕ		prev;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    ಸಾಲು_end = s + ml_get_len(lnum);
    ಪ್ರತಿ (; *s != NUL; ++s)
    {
	ಯದಿ (prev == -1 || vim_isspace(prev))
	{
	    ಯದಿ ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		ಭಙ್ಗ;
	    // Accept both "vim" and "Vim".
	    ಯದಿ ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		ಪೂರ್ಣಾಙ್ಕ vers;

		ಯದಿ (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		ಅನ್ಯಥಾ
		    e = s + 3;
		vers = getdigits(&e);
		ಯದಿ (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    ಭಙ್ಗ;
	    }
	}
	prev = *s;
    }

    ಯದಿ (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	ಪೂರ್ಣಾಙ್ಕ	end;

	ಕುರು				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	ಯಾವತ್ (s[-1] != ':');

	len = (size_t)(ಸಾಲು_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	ಯದಿ (linecopy == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಸಾಲು_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_ತಳ್ಳು(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	ಯಾವತ್ (end == FALSE)
	{
	    s = skipwhite(s);
	    ಯದಿ (*s == NUL)
		ಭಙ್ಗ;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    ಪ್ರತಿ (e = s; *e != ':' && *e != NUL; ++e)
		ಯದಿ (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(ಸಾಲು_end - (e + 1)) + 1);	// +1 for NUL
		    --ಸಾಲು_end;
		}
	    ಯದಿ (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    ಯದಿ (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		ಯದಿ (*e != ':')		// no terminating ':'?
		    ಭಙ್ಗ;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    ಯದಿ (*s != NUL)		// skip over an empty "::"
	    {
		ಪೂರ್ಣಾಙ್ಕ secure_save = secure;
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
		ಯದಿ (retval == FAIL)		// stop if error found
		    ಭಙ್ಗ;
	    }
	    s = (e == ಸಾಲು_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_ತೆಗೆ();
	vim_free(linecopy);
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    ಪೂರ್ಣಾಙ್ಕ
bt_normal(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buf != ರಿಕ್ತ && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    ಪೂರ್ಣಾಙ್ಕ
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    ಪ್ರತ್ಯಾಗಮ buf != ರಿಕ್ತ && buf->b_p_bt[0] == 'q';
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    ಪೂರ್ಣಾಙ್ಕ
bt_terminal(buf_T *buf UNUSED)
{
#ಯದಿ defined(FEAT_TERMINAL)
    ಪ್ರತ್ಯಾಗಮ buf != ರಿಕ್ತ && buf->b_p_bt[0] == 't';
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    ಪೂರ್ಣಾಙ್ಕ
bt_help(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buf != ರಿಕ್ತ && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    ಪೂರ್ಣಾಙ್ಕ
bt_prompt(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buf != ರಿಕ್ತ && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#ಯದಿ defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    ಪೂರ್ಣಾಙ್ಕ
bt_popup(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buf != ರಿಕ್ತ && buf->b_p_bt != ರಿಕ್ತ
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    ಪೂರ್ಣಾಙ್ಕ
bt_nofilename(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buf != ರಿಕ್ತ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
bt_nofileread(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buf != ರಿಕ್ತ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#ಯದಿ defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    ಪೂರ್ಣಾಙ್ಕ
bt_nofile(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buf != ರಿಕ್ತ && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    ಪೂರ್ಣಾಙ್ಕ
bt_dontwrite(buf_T *buf)
{
    ಪ್ರತ್ಯಾಗಮ buf != ರಿಕ್ತ && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    ಪೂರ್ಣಾಙ್ಕ
bt_dontwrite_ಸಂದೇಶ(buf_T *buf)
{
    ಯದಿ (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    ಪೂರ್ಣಾಙ್ಕ
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    ವಿಕಲ್ಪ (buf->b_p_bh[0])
    {
	ಪಕ್ಷ 'u':		    // "unload"
	ಪಕ್ಷ 'w':		    // "wipe"
	ಪಕ್ಷ 'd': ಪ್ರತ್ಯಾಗಮ FALSE;	    // "delete"
	ಪಕ್ಷ 'h': ಪ್ರತ್ಯಾಗಮ TRUE;	    // "hide"
    }
    ಪ್ರತ್ಯಾಗಮ (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#ಯದಿ defined(FEAT_QUICKFIX)
    ಯದಿ (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	ಯದಿ (buf->b_fnum == qf_ರಾಶಿ_get_bufnr())
	    ಪ್ರತ್ಯಾಗಮ (char_u *)_(ಸಂದೇಶ_qflist);
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ (char_u *)_(ಸಂದೇಶ_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    ಯದಿ (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	ಯದಿ (buf->b_term != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ term_get_status_text(buf->b_term);
#endif
	ಯದಿ (buf->b_fname != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ buf->b_fname;
	ಯದಿ (buf == cmdwin_buf)
	    ಪ್ರತ್ಯಾಗಮ (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	ಯದಿ (bt_prompt(buf))
	    ಪ್ರತ್ಯಾಗಮ (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	ಯದಿ (bt_popup(buf))
	    ಪ್ರತ್ಯಾಗಮ (char_u *)_("[Popup]");
#endif
	ಪ್ರತ್ಯಾಗಮ (char_u *)_("[Scratch]");
    }

    ಯದಿ (buf->b_fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ buf_get_fname(buf);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    ಯದಿ (buf->b_fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (char_u *)_("[No Name]");
    ಪ್ರತ್ಯಾಗಮ buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    ಶೂನ್ಯ
set_buflisted(ಪೂರ್ಣಾಙ್ಕ on)
{
    ಯದಿ (on == curbuf->b_p_bl)
	ಪ್ರತ್ಯಾಗಮ;

    curbuf->b_p_bl = on;
    ಯದಿ (on)
	apply_autocmds(EVENT_BUFADD, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
    ಅನ್ಯಥಾ
	apply_autocmds(EVENT_BUFDELETE, ರಿಕ್ತ, ರಿಕ್ತ, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    ಪೂರ್ಣಾಙ್ಕ
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    ಪೂರ್ಣಾಙ್ಕ		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ರಿಕ್ತ, ರಿಕ್ತ, (linenr_T)1, BLN_DUMMY);
    ಯದಿ (newbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    ಯದಿ (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    ಯದಿ (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    ಯದಿ (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	ಯದಿ (buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    differ = FALSE;
	    ಪ್ರತಿ (lnum = 1; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
		ಯದಿ (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    ಭಙ್ಗ;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    ಯದಿ (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    ಪ್ರತ್ಯಾಗಮ differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    ಶೂನ್ಯ
wipe_buffer(
    buf_T	*buf,
    ಪೂರ್ಣಾಙ್ಕ		aucmd)	    // When TRUE trigger autocommands.
{
    ಯದಿ (buf->b_fnum == ತುದಿ_file_ಅಂಕೆ - 1)
	--ತುದಿ_file_ಅಂಕೆ;

    ಯದಿ (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(ರಿಕ್ತ, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    ಯದಿ (!aucmd)
	unblock_autocmds();
}
