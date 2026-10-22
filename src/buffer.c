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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_ਡੂੰਘਾਈ 100
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ	enter_ਬਫਰ(buf_T *buf);
ਸਥਿਰ ਖਾਲੀ	buflist_getfpos(ਖਾਲੀ);
ਸਥਿਰ char_u	*buflist_ਮੇਲ(regmatch_T *rmp, buf_T *buf, ਪੂਰਨ ignore_case);
ਸਥਿਰ char_u	*fname_ਮੇਲ(regmatch_T *rmp, char_u *name, ਪੂਰਨ ignore_case);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
ਸਥਿਰ buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
ਸਥਿਰ ਪੂਰਨ	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
ਸਥਿਰ ਪੂਰਨ	buf_same_ino(buf_T *buf, stat_T *stp);
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਪੂਰਨ	otherfile_buf(buf_T *buf, char_u *ffname);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ	ਮੁੱਲ_ਬਦਲਿਆ(char_u *str, char_u **ਆਖਰੀ);
#ਜੇ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
ਸਥਿਰ ਪੂਰਨ build_stl_str_hl_local(stl_ਬਹੁਲਕ_T ਬਹੁਲਕ, win_T *wp,
		char_u *ਬਾਹਰਲਾ, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, ਪੂਰਨ opt_scope, ਪੂਰਨ fillchar, ਪੂਰਨ maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, ਪੂਰਨ *lbreaks, ਪੂਰਨ *ਹਾਸਲ_hl);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ	append_arg_ਸੰਖਿਆ(win_T *wp, char_u *buf, size_t buflen, ਪੂਰਨ ਜੋੜੋ_file);
ਸਥਿਰ ਖਾਲੀ	free_ਬਫਰ(buf_T *);
ਸਥਿਰ ਖਾਲੀ	free_ਬਫਰ_stuff(buf_T *buf, ਪੂਰਨ free_options);
ਸਥਿਰ ਪੂਰਨ	bt_nofileread(buf_T *buf);
ਸਥਿਰ ਪੂਰਨ	do_ਬਫਰ_ext(ਪੂਰਨ action, ਪੂਰਨ ਸ਼ੁਰੂ, ਪੂਰਨ dir, ਪੂਰਨ ਗਿਣਤੀ_2, ਪੂਰਨ flags);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
# define dev_T dev_t
#ਨਹੀਂਤਾਂ
# define dev_T ਬਿਨਾਨਿਸ਼ਾਨੀ
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_BUFS_FROM_ਆਖਰੀ(buf) \
    ਲਈ ((buf) = lastbuf; (buf) != ਨਿਰਾ; (buf) = (buf)->b_prev)

#ਜੇ defined(FEAT_QUICKFIX)
ਸਥਿਰ ਅੱਖਰ *ਸੁਨੇਹਾ_loclist = N_("[Location List]");
ਸਥਿਰ ਅੱਖਰ *ਸੁਨੇਹਾ_qflist = N_("[Quickfix List]");
#ਜੇ_ਅੰਤ

// Number of times free_buffer() was called.
ਸਥਿਰ ਪੂਰਨ	buf_free_ਗਿਣਤੀ = 0;

ਸਥਿਰ ਪੂਰਨ	ਸਿਖਰ_file_ਅੰਕ = 1;	// highest file number
ਸਥਿਰ garray_T buf_reuse = GA_ਖਾਲੀ_ਥਾਵਾਂ;	// file numbers to recycle

    ਸਥਿਰ ਖਾਲੀ
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    ਖਿੜਕੀ_layout_ਤਾਲਾ();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"ਜੇ exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    ਖਿੜਕੀ_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    ਸਥਿਰ ਪੂਰਨ
calc_percentage(ਲੰਮਾ part, ਲੰਮਾ ਸਾਰਾ)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    ਵਾਪਸ (part > 1000000L)
	? (ਪੂਰਨ)(part / (ਸਾਰਾ / 100L))
	: (ਪੂਰਨ)((part * 100L) / ਸਾਰਾ);
}

/*
 * Return the highest possible buffer number.
 */
    ਪੂਰਨ
get_highest_fnum(ਖਾਲੀ)
{
    ਵਾਪਸ ਸਿਖਰ_file_ਅੰਕ - 1;
}

/*
 * Read data from buffer for retrying.
 */
    ਸਥਿਰ ਪੂਰਨ
read_ਬਫਰ(
    ਪੂਰਨ		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    ਪੂਰਨ		flags)		    // extra flags for readfile()
{
    ਪੂਰਨ		retval = OK;
    linenr_T	ਸਤਰ_ਗਿਣਤੀ;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    ਸਤਰ_ਗਿਣਤੀ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    retval = readfile(
	    read_stdin ? ਨਿਰਾ : curbuf->b_ffname,
	    read_stdin ? ਨਿਰਾ : curbuf->b_fname,
	    ਸਤਰ_ਗਿਣਤੀ, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_ਬਫਰ);
    ਜੇ (retval == OK)
    {
	// Delete the binary lines.
	ਜਦੋਂ (--ਸਤਰ_ਗਿਣਤੀ >= 0)
	    ml_delete((linenr_T)1);
    }
    ਨਹੀਂਤਾਂ
    {
	// Delete the converted lines.
	ਜਦੋਂ (curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ > ਸਤਰ_ਗਿਣਤੀ)
	    ml_delete(ਸਤਰ_ਗਿਣਤੀ);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ਖਾਨਾ = 0;

    ਜੇ (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	ਜੇ (!readonlymode && !BUFEMPTY())
	    ਬਦਲਿਆ();
	ਨਹੀਂਤਾਂ ਜੇ (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	ਜੇ (retval == OK)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE,
							      curbuf, &retval);
#ਨਹੀਂਤਾਂ
	    apply_autocmds(EVENT_STDINREADPOST, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
#ਜੇ_ਅੰਤ
	}
    }
    ਵਾਪਸ retval;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    ਖਾਲੀ
ਬਫਰ_ensure_loaded(buf_T *buf)
{
    ਜੇ (buf->b_ml.ml_mfp != ਨਿਰਾ)
	ਵਾਪਸ;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    ਜੇ (curbuf == buf)
    {
	ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action != SEA_READONLY)
	    ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_NONE;
	open_ਬਫਰ(FALSE, ਨਿਰਾ, 0);
	aucmd_restbuf(&aco);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
open_ਬਫਰ(
    ਪੂਰਨ		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    ਪੂਰਨ		flags_arg)	    // extra flags for readfile()
{
    ਪੂਰਨ		flags = flags_arg;
    ਪੂਰਨ		retval = OK;
    bufref_T	old_curbuf;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਲੰਮਾ	old_tw = curbuf->b_p_tw;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    ਜੇ (readonlymode && curbuf->b_ffname != ਨਿਰਾ
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    ਜੇ (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_ਬਫਰ(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    ਜੇ (curbuf->b_ml.ml_mfp != ਨਿਰਾ)
		ਰੋਕੋ;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	ਜੇ (curbuf == ਨਿਰਾ)
	{
	    emsg(_(e_cannot_allocate_any_ਬਫਰ_exiting));

	    // Don't try to do any saving, with "curbuf" NULL almost nothing
	    // will work.
	    v_dying = 2;
	    getout(2);
	}

	emsg(_(e_cannot_allocate_ਬਫਰ_using_other_one));
	enter_ਬਫਰ(curbuf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਜੇ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ਨਿਰਾ, curwin);
#ਜੇ_ਅੰਤ
	ਵਾਪਸ FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    ਜੇ (curbuf->b_ml.ml_mfp != ਨਿਰਾ)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    ਸੈੱਟ_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_ਸੈੱਟ = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    ਜੇ (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    ਜੇ (curbuf->b_ffname != ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	    && netbeansReadFile
#ਜੇ_ਅੰਤ
       )
    {
	ਪੂਰਨ old_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	ਪੂਰਨ	save_bin = curbuf->b_p_bin;
	ਪੂਰਨ perm;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	ਪੂਰਨ oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	perm = mch_getperm(curbuf->b_ffname);
	ਜੇ (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	ਜੇ (read_fifo)
	    curbuf->b_p_bin = TRUE;
#ਜੇ_ਅੰਤ
	ਜੇ (shortmess(SHM_FILEINFO))
	    ਸੁਨੇਹਾ_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	ਜੇ (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    ਜੇ (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_ਬਫਰ(FALSE, eap, flags);
	}
#ਜੇ_ਅੰਤ
	ਸੁਨੇਹਾ_silent = old_ਸੁਨੇਹਾ_silent;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#ਜੇ_ਅੰਤ
	// Help buffer is filtered.
	ਜੇ (bt_help(curbuf))
	    fix_help_ਬਫਰ();
    }
    ਨਹੀਂਤਾਂ ਜੇ (read_stdin)
    {
	ਪੂਰਨ	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(ਨਿਰਾ, ਨਿਰਾ, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, ਨਿਰਾ,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	ਜੇ (retval == OK)
	    retval = read_ਬਫਰ(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    ਜੇ (curbuf->b_ml.ml_mfp != ਨਿਰਾ
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    ਜੇ (curbuf->b_flags & BF_NEVERLOADED)
    {
	(ਖਾਲੀ)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    ਜੇ ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != ਨਿਰਾ)
		|| curbuf->b_modified_was_ਸੈੱਟ	// autocmd did ":set modified"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != ਨਿਰਾ)
#ਜੇ_ਅੰਤ
       )
	ਬਦਲਿਆ();
    ਨਹੀਂਤਾਂ ਜੇ (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_ਆਖਰੀ_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_ਆਖਰੀ_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_ਆਖਰੀ_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (aborting())
#ਨਹੀਂਤਾਂ
    ਜੇ (got_int)
#ਜੇ_ਅੰਤ
	curbuf->b_flags |= BF_READERR;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#ਜੇ_ਅੰਤ

    // need to set w_topline, unless some autocommand already did that.
    ਜੇ (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	curwin->w_topfill = 0;
#ਜੇ_ਅੰਤ
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf, &retval);
#ਨਹੀਂਤਾਂ
    apply_autocmds(EVENT_BUFENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
#ਜੇ_ਅੰਤ

    ਜੇ (retval != OK)
	ਵਾਪਸ retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    ਜੇ (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != ਨਿਰਾ)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	ਜੇ (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    ਜੇ ((flags & READ_NOWINENTER) == 0)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, ਨਿਰਾ, ਨਿਰਾ,
			FALSE, curbuf, &retval);
#ਨਹੀਂਤਾਂ
	    apply_autocmds(EVENT_BUFWINENTER, ਨਿਰਾ, ਨਿਰਾ,
		    FALSE, curbuf);
#ਜੇ_ਅੰਤ

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    ਵਾਪਸ retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    ਖਾਲੀ
ਸੈੱਟ_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == ਨਿਰਾ ? 0 : buf->b_fnum;
    bufref->br_buf_free_ਗਿਣਤੀ = buf_free_ਗਿਣਤੀ;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    ਪੂਰਨ
bufref_valid(bufref_T *bufref)
{
    ਵਾਪਸ bufref->br_buf_free_ਗਿਣਤੀ == buf_free_ਗਿਣਤੀ
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    ਪੂਰਨ
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_ਆਖਰੀ(bp)
	ਜੇ (bp == buf)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
ਸਥਿਰ hashtab_T buf_hashtab;

    ਸਥਿਰ ਖਾਲੀ
buf_hashtab_ਜੋੜੋ(buf_T *buf)
{
    vim_snprintf((ਅੱਖਰ *)buf->b_ਕੁੰਜੀ, ਆਕਾਰ(buf->b_ਕੁੰਜੀ), "%x", buf->b_fnum);
    ਜੇ (hash_ਜੋੜੋ(&buf_hashtab, buf->b_ਕੁੰਜੀ, "create buffer") == FAIL)
	emsg(_(e_ਬਫਰ_cannot_be_registered));
}

    ਸਥਿਰ ਖਾਲੀ
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_ਲੱਭੋ(&buf_hashtab, buf->b_ਕੁੰਜੀ);

    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    ਸਥਿਰ ਪੂਰਨ
can_unload_ਬਫਰ(buf_T *buf)
{
    ਪੂਰਨ	    can_unload = !buf->b_locked;

    ਜੇ (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ਜੇ (wp->w_ਬਫਰ == buf)
	    {
		can_unload = FALSE;
		ਰੋਕੋ;
	    }
    }
    // Don't unload the buffer while it's still being saved
    ਜੇ (can_unload && buf->b_saving)
	can_unload = FALSE;

    ਜੇ (!can_unload)
    {
	char_u *fname = buf->b_fname != ਨਿਰਾ ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_ਮੰਜ਼ਿਲ_delete_ਬਫਰ_that_is_ਵਿੱਚ_use_str),
				fname != ਨਿਰਾ ? fname : (char_u *)"[No Name]");
    }
    ਵਾਪਸ can_unload;
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
    ਪੂਰਨ
close_ਬਫਰ(
    win_T	*win,
    buf_T	*buf,
    ਪੂਰਨ		action,
    ਪੂਰਨ		abort_if_ਆਖਰੀ,
    ਪੂਰਨ		ignore_abort,
    ਪੂਰਨ		ਸੈੱਟ_context)
{
    ਪੂਰਨ		hiding_buf;
    bufref_T	bufref;
    ਪੂਰਨ		is_curwin = (curwin != ਨਿਰਾ && curwin == win
				&& curwin->w_ਬਫਰ == buf);
    tabpage_T	*the_curtab = curtab;
    ਪੂਰਨ		unload_buf = (action != 0);
    ਪੂਰਨ		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    ਪੂਰਨ		del_buf = (action == DOBUF_DEL || wipe_buf);
    ਪੂਰਨ		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    ਪੂਰਨ		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    ਜੇ (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    ਜੇ (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	ਜੇ (ਪਦ_ਕੰਮ_running(buf->b_ਪਦ))
	{
	    ਜੇ (wipe_buf || unload_buf)
	    {
		ਜੇ (!can_unload_ਬਫਰ(buf))
		    ਵਾਪਸ FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (del_buf || unload_buf)
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
#ਜੇ_ਅੰਤ

    // Disallow deleting the buffer when it is locked (already being closed or
    // halfway a command that relies on it). Unloading is allowed.
    ਜੇ ((del_buf || wipe_buf) && !can_unload_ਬਫਰ(buf))
	ਵਾਪਸ FALSE;

    ਜੇ (ਸੈੱਟ_context && win_valid && win->w_ਬਫਰ == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	ਜੇ (buf->b_nwindows == 1)
	    ਸੈੱਟ_ਆਖਰੀ_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.ਖਾਨਾ, TRUE);
    }

    ਸੈੱਟ_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    ਜੇ ((win_valid || closed_popup) && win->w_ਬਫਰ == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_ਵੰਡੋ;
	ਜੇ (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_ਮੰਜ਼ਿਲ_abort));
	    ਵਾਪਸ FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_ਵੰਡੋ;
	ਜੇ (abort_if_ਆਖਰੀ && one_ਖਿੜਕੀ())
	    // Autocommands made this the only window.
	    ਜਾਓ aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	ਜੇ (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_ਵੰਡੋ;
	    ਜੇ (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		ਜਾਓ aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_ਵੰਡੋ;
	    ਜੇ (abort_if_ਆਖਰੀ && one_ਖਿੜਕੀ())
		// Autocommands made this the only window.
		ਜਾਓ aucmd_abort;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// autocmds may abort script processing
	ਜੇ (!ignore_abort && aborting())
	    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ਜੇ (is_curwin && curwin != win && win_valid)
    {
	ਖੰਡ_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    ਜੇ (bt_quickfix(buf) && win_valid && win->w_ਬਫਰ == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_ਬਫਰ == buf && buf->b_nwindows == 1);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#ਜੇ_ਅੰਤ

    // Return when another window is displaying the buffer or when not unloaded.
    ਜੇ (!hiding_buf || !unload_buf)
	ਵਾਪਸ FALSE;

    // Always remove the buffer when there is no file name.
    ਜੇ (buf->b_ffname == ਨਿਰਾ)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    ਜੇ (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	ਵਾਪਸ FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    ਜੇ ((win_valid || closed_popup) && win->w_ਬਫਰ == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_ਬਫਰ = ਨਿਰਾ;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    ਜੇ (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != ਨਿਰਾ || buf->b_ਅਗਲਾ != ਨਿਰਾ
#ਜੇ defined(EXITFREE)
				|| entered_free_all_mem
#ਜੇ_ਅੰਤ
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	ਜੇ (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    ਜੇ (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, ਆਕਾਰ(ਪੂਰਨ), 50);
	    ਜੇ (ga_grow(&buf_reuse, 1) == OK)
		((ਪੂਰਨ *)buf_reuse.ga_ਅੰਕੜੇ)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	ਜੇ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ਨਹੀਂਤਾਂ
	    buf->b_sfname = ਨਿਰਾ;
	VIM_CLEAR(buf->b_ffname);
	ਜੇ (buf->b_prev == ਨਿਰਾ)
	    firstbuf = buf->b_ਅਗਲਾ;
	ਨਹੀਂਤਾਂ
	    buf->b_prev->b_ਅਗਲਾ = buf->b_ਅਗਲਾ;
	ਜੇ (buf->b_ਅਗਲਾ == ਨਿਰਾ)
	    lastbuf = buf->b_prev;
	ਨਹੀਂਤਾਂ
	    buf->b_ਅਗਲਾ->b_prev = buf->b_prev;
	free_ਬਫਰ(buf);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (del_buf)
	{
	    // Free all internal variables and reset option values, to make
	    // ":bdel" compatible with Vim 5.7.
	    free_ਬਫਰ_stuff(buf, TRUE);

	    // Make it look like a new buffer.
	    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;

	    // Init the options when loaded again.
	    buf->b_p_initialized = false;
	}
	buf_clear_file(buf);
	ਜੇ (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    ਪ੍ਰਤਿਜ੍ਞਾ(!win_valid || !is_curwin || win == curwin);
    ਵਾਪਸ TRUE;
}

/*
 * Make buffer not contain a file.
 */
    ਖਾਲੀ
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_ਸ਼ੁਰੂ_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_ਸ਼ੁਰੂ_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_ਸ਼ੁਰੂ_bomb = FALSE;
    buf->b_ml.ml_mfp = ਨਿਰਾ;
    buf->b_ml.ml_flags = ML_ਖਾਲੀ_ਥਾਵਾਂ;		// empty buffer
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    netbeans_deleted_all_ਸਤਰਾਂ(buf);
#ਜੇ_ਅੰਤ
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
    ਪੂਰਨ
buf_freeall(buf_T *buf, ਪੂਰਨ flags)
{
    ਪੂਰਨ		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    ਪੂਰਨ		is_curwin = (curwin != ਨਿਰਾ && curwin->w_ਬਫਰ == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_ਵੰਡੋ;
    ਸੈੱਟ_bufref(&bufref, buf);
    ਜੇ (buf->b_ml.ml_mfp != ਨਿਰਾ)
    {
	ਜੇ (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ਵਾਪਸ FALSE;
    }
    ਜੇ ((flags & BFA_DEL) && buf->b_p_bl)
    {
	ਜੇ (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ਵਾਪਸ FALSE;
    }
    ਜੇ (flags & BFA_WIPE)
    {
	ਜੇ (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ਵਾਪਸ FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_ਵੰਡੋ;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ਜੇ (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	ਖੰਡ_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // autocmds may abort script processing
    ਜੇ ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    ਜੇ (buf == curbuf && !is_curbuf)
	ਵਾਪਸ FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    ਜੇ (buf == curbuf && VIsual_ਸਰਗਰਮ
#ਜੇ defined(EXITFREE)
	    && !entered_free_all_mem
#ਜੇ_ਅੰਤ
	    )
	ਸਿਰਾ_visual_ਬਹੁਲਕ();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    ਜੇ (curwin != ਨਿਰਾ && curwin->w_ਬਫਰ == buf)
	reset_synblock(curwin);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    ਜੇ (win->w_ਬਫਰ == buf)
		clearFolding(win);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TCL
    tcl_ਬਫਰ_free(buf);
#ਜੇ_ਅੰਤ
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ = 0;    // no lines in buffer
    ਜੇ ((flags & BFA_KEEP_UNDO) == 0)
	// free the memory allocated for undo
	// and reset all undo information
	u_clearallandblockfree(buf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    syntax_clear(&buf->b_s);	    // reset syntax info
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    clear_buf_prop_types(buf);
#ਜੇ_ਅੰਤ
    buf->b_flags &= ~BF_READERR;    // a read error is no longer relevant
    ਵਾਪਸ TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    ਸਥਿਰ ਖਾਲੀ
free_ਬਫਰ(buf_T *buf)
{
    ++buf_free_ਗਿਣਤੀ;
    free_ਬਫਰ_stuff(buf, TRUE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // b:changedtick uses an item in buf_T, remove it now
    dictitem_remove(buf->b_vars, (dictitem_T *)&buf->b_ct_di, "free buffer");
    unref_var_dict(buf->b_vars);
    remove_listeners(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LUA
    lua_ਬਫਰ_free(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
    mzscheme_ਬਫਰ_free(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PERL
    perl_buf_free(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON
    python_ਬਫਰ_free(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PYTHON3
    python3_ਬਫਰ_free(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RUBY
    ruby_ਬਫਰ_free(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    channel_ਬਫਰ_free(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    free_terminal(buf);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    vim_free(buf->b_prompt_ਪਾਠ);
    free_callback(&buf->b_prompt_callback);
    free_callback(&buf->b_prompt_ਰੁਕਾਵਟ);
#ਜੇ_ਅੰਤ

    buf_hashtab_remove(buf);

    aubuflocal_remove(buf);

    ਜੇ (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_ਅਗਲਾ = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    ਨਹੀਂਤਾਂ
    {
	vim_free(buf);
	ਜੇ (curbuf == buf)
	    curbuf = ਨਿਰਾ;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    ਸਥਿਰ ਖਾਲੀ
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_ਸੰਖਿਆ;
    di->di_tv.v_ਤਾਲਾ = VAR_FIXED;
    di->di_tv.vval.v_ਸੰਖਿਆ = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    STRCPY(buf->b_ct_di.di_ਕੁੰਜੀ, "changedtick");
    (ਖਾਲੀ)dict_ਜੋੜੋ(buf->b_vars, di);
#ਜੇ_ਅੰਤ
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    ਸਥਿਰ ਖਾਲੀ
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    ਜਦੋਂ (buf->b_wininfo != ਨਿਰਾ)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_ਅਗਲਾ;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
free_ਬਫਰ_stuff(
    buf_T	*buf,
    ਪੂਰਨ		free_options)		// free options as well
{
    ਜੇ (free_options)
    {
	clear_wininfo(buf);		// including window-local options
	free_buf_options(buf, TRUE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
	ga_clear(&buf->b_s.b_langp);
#ਜੇ_ਅੰਤ
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    {
	varnumber_T tick = CHANGEDTICK(buf);

	vars_clear(&buf->b_vars->dv_hashtab); // free all buffer variables
	hash_init(&buf->b_vars->dv_hashtab);
	init_changedtick(buf);
	CHANGEDTICK(buf) = tick;
	remove_listeners(buf);
    }
#ਜੇ_ਅੰਤ
    uc_clear(&buf->b_ucmds);		// clear local user commands
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SIGNS
    buf_delete_signs(buf, (char_u *)"*");	// delete any signs
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    netbeans_file_killed(buf);
#ਜੇ_ਅੰਤ
    map_clear_ਬਹੁਲਕ(buf, MAP_ALL_MODES, TRUE, FALSE);  // clear local mappings
    map_clear_ਬਹੁਲਕ(buf, MAP_ALL_MODES, TRUE, TRUE);   // clear local abbrevs
    VIM_CLEAR(buf->b_ਸ਼ੁਰੂ_fenc);
}

/*
 * Free one wininfo_T.
 */
    ਖਾਲੀ
free_wininfo(wininfo_T *wip)
{
    ਜੇ (wip->wi_optset)
    {
	clear_winopt(&wip->wi_opt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	deleteFoldRecurse(&wip->wi_folds);
#ਜੇ_ਅੰਤ
    }
    vim_free(wip);
}

/*
 * Go to another buffer.  Handles the result of the ATTENTION dialog.
 */
    ਖਾਲੀ
goto_ਬਫਰ(
    exarg_T	*eap,
    ਪੂਰਨ		ਸ਼ੁਰੂ,
    ਪੂਰਨ		dir,
    ਪੂਰਨ		ਗਿਣਤੀ_2)
{
    bufref_T	old_curbuf;
    ਪੂਰਨ		save_sea = ਅਦਲਾ_ਬਦਲੀ_exists_action;
    ਪੂਰਨ		skip_help_buf;

    ਚੋਣ (eap->cmdidx)
    {
	ਹਾਲਤ CMD_bnext:
	ਹਾਲਤ CMD_sbnext:
	ਹਾਲਤ CMD_b_ਅਗਲਾ:
	ਹਾਲਤ CMD_bprevious:
	ਹਾਲਤ CMD_sb_ਅਗਲਾ:
	ਹਾਲਤ CMD_sbprevious:
	    skip_help_buf = TRUE;
	    ਰੋਕੋ;
	ਮੂਲ:
	    skip_help_buf = FALSE;
	    ਰੋਕੋ;
    }

    ਸੈੱਟ_bufref(&old_curbuf, curbuf);

    ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action == SEA_NONE)
	ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_DIALOG;
    (ਖਾਲੀ)do_ਬਫਰ_ext(*eap->cmd == 's' ? DOBUF_ਵੰਡੋ : DOBUF_GOTO, ਸ਼ੁਰੂ, dir, ਗਿਣਤੀ_2,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#ਜੇ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#ਜੇ_ਅੰਤ

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	ਅਦਲਾ_ਬਦਲੀ_exists_action = save_sea;
	ਅਦਲਾ_ਬਦਲੀ_exists_did_quit = TRUE;

#ਜੇ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
	handle_ਅਦਲਾ_ਬਦਲੀ_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    ਖਾਲੀ
handle_ਅਦਲਾ_ਬਦਲੀ_exists(bufref_T *old_curbuf)
{
#ਜੇ defined(FEAT_EVAL)
    cleanup_T	cs;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਲੰਮਾ	old_tw = curbuf->b_p_tw;
#ਜੇ_ਅੰਤ
    buf_T	*buf;

    ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action == SEA_QUIT)
    {
#ਜੇ defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#ਜੇ_ਅੰਤ

	// User selected Quit at ATTENTION prompt.  Go back to previous
	// buffer.  If that buffer is gone or the same as the current one,
	// open a new, empty buffer.
	ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_NONE;	// don't want it again
	ਅਦਲਾ_ਬਦਲੀ_exists_did_quit = TRUE;
	close_ਬਫਰ(curwin, curbuf, DOBUF_UNLOAD, FALSE, FALSE, TRUE);
	ਜੇ (old_curbuf == ਨਿਰਾ || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    ਖੰਡ_autocmds();
	    buf = buflist_new(ਨਿਰਾ, ਨਿਰਾ, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	ਨਹੀਂਤਾਂ
	    buf = old_curbuf->br_buf;
	ਜੇ (buf != ਨਿਰਾ)
	{
	    ਪੂਰਨ old_ਸੁਨੇਹਾ_silent = ਸੁਨੇਹਾ_silent;

	    ਜੇ (shortmess(SHM_FILEINFO))
		ਸੁਨੇਹਾ_silent = 1;  // prevent fileinfo message
	    enter_ਬਫਰ(buf);
	    // restore msg_silent, so that the command line will be shown
	    ਸੁਨੇਹਾ_silent = old_ਸੁਨੇਹਾ_silent;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	    ਜੇ (old_tw != curbuf->b_p_tw)
		check_colorcolumn(ਨਿਰਾ, curwin);
#ਜੇ_ਅੰਤ
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#ਜੇ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action == SEA_RECOVER)
    {
#ਜੇ defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#ਜੇ_ਅੰਤ

	// User selected Recover at ATTENTION prompt.
	ਸੁਨੇਹਾ_scroll = TRUE;
	ml_recover(FALSE);
	ਸੁਨੇਹਾ_puts("\n");	// don't overwrite the last message
	cmdline_ਪੰਕਤੀ = ਸੁਨੇਹਾ_ਪੰਕਤੀ;
	do_modelines(0);

#ਜੇ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#ਜੇ_ਅੰਤ
    }
    ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_NONE;
}

/*
 * Make the current buffer empty.
 * Used when it is wiped out and it's the last buffer.
 */
    ਸਥਿਰ ਪੂਰਨ
ਖਾਲੀ_ਥਾਵਾਂ_curbuf(
    ਪੂਰਨ close_others,
    ਪੂਰਨ forceit,
    ਪੂਰਨ action)
{
    ਪੂਰਨ	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    ਜੇ (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_ਆਖਰੀ_ਬਫਰ));
	ਵਾਪਸ FAIL;
    }

    ਸੈੱਟ_bufref(&bufref, buf);
    ਜੇ (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    ਜੇ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_ਬਫਰ(ਨਿਰਾ, buf, action, FALSE, FALSE, FALSE);
    ਜੇ (!close_others)
	need_fileinfo = FALSE;
    ਨਹੀਂਤਾਂ ਜੇ (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    ਵਾਪਸ retval;
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
    ਸਥਿਰ ਪੂਰਨ
do_ਬਫਰ_ext(
    ਪੂਰਨ		action,
    ਪੂਰਨ		ਸ਼ੁਰੂ,
    ਪੂਰਨ		dir,		// FORWARD or BACKWARD
    ਪੂਰਨ		ਗਿਣਤੀ_2,		// buffer number or number of buffers
    ਪੂਰਨ		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    ਪੂਰਨ		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    ਚੋਣ (ਸ਼ੁਰੂ)
    {
	ਹਾਲਤ DOBUF_ਪਹਿਲਾ:   buf = firstbuf; ਰੋਕੋ;
	ਹਾਲਤ DOBUF_ਆਖਰੀ:    buf = lastbuf;  ਰੋਕੋ;
	ਮੂਲ:	    buf = curbuf;   ਰੋਕੋ;
    }
    ਜੇ (ਸ਼ੁਰੂ == DOBUF_MOD)	    // find next modified buffer
    {
	ਜਦੋਂ (ਗਿਣਤੀ_2-- > 0)
	{
	    ਕਰੋ
	    {
		buf = buf->b_ਅਗਲਾ;
		ਜੇ (buf == ਨਿਰਾ)
		    buf = firstbuf;
	    }
	    ਜਦੋਂ (buf != curbuf && !buf_Is_ਬਦਲਿਆ(buf));
	}
	ਜੇ (!buf_Is_ਬਦਲਿਆ(buf))
	{
	    emsg(_(e_no_modified_ਬਫਰ_ਮਿਲਿਆ));
	    ਵਾਪਸ FAIL;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਸ਼ੁਰੂ == DOBUF_ਪਹਿਲਾ && ਗਿਣਤੀ_2) // find specified buffer number
    {
	ਜਦੋਂ (buf != ਨਿਰਾ && buf->b_fnum != ਗਿਣਤੀ_2)
	    buf = buf->b_ਅਗਲਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = ਨਿਰਾ;
	ਜਦੋਂ (ਗਿਣਤੀ_2 > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    ਜੇ (bp == ਨਿਰਾ)
		bp = buf;
	    ਜੇ (dir == FORWARD)
	    {
		buf = buf->b_ਅਗਲਾ;
		ਜੇ (buf == ਨਿਰਾ)
		    buf = firstbuf;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		buf = buf->b_prev;
		ਜੇ (buf == ਨਿਰਾ)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    ਜੇ (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--ਗਿਣਤੀ_2;
		bp = ਨਿਰਾ;	// use this buffer as new starting point
	    }
	    ਜੇ (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_ਬਫਰ));
		ਵਾਪਸ FAIL;
	    }
	}
    }

    ਜੇ (buf == ਨਿਰਾ)	    // could not find it
    {
	ਜੇ (ਸ਼ੁਰੂ == DOBUF_ਪਹਿਲਾ)
	{
	    // don't warn when deleting
	    ਜੇ (!unload)
		semsg(_(e_ਬਫਰ_nr_does_not_exist), ਗਿਣਤੀ_2);
	}
	ਨਹੀਂਤਾਂ ਜੇ (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_ਆਖਰੀ_ਬਫਰ));
	ਨਹੀਂਤਾਂ
	    emsg(_(e_cannot_go_before_ਪਹਿਲਾ_ਬਫਰ));
	ਵਾਪਸ FAIL;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	ਵਾਪਸ OK;
#ਜੇ_ਅੰਤ
    ਜੇ (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_ਸੈੱਟ_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	ਵਾਪਸ FAIL;

    ਜੇ ((action == DOBUF_GOTO || action == DOBUF_ਵੰਡੋ)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_ਬਫਰ_nr_does_not_exist), ਗਿਣਤੀ_2);
	ਵਾਪਸ FAIL;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    /*
     * delete buffer "buf" from memory and/or the list
     */
    ਜੇ (unload)
    {
	ਪੂਰਨ	forward;
	bufref_T bufref;

	ਜੇ (!can_unload_ਬਫਰ(buf))
	    ਵਾਪਸ FAIL;

	ਸੈੱਟ_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	ਜੇ (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == ਨਿਰਾ && !buf->b_p_bl)
	    ਵਾਪਸ FAIL;

	ਜੇ ((flags & DOBUF_FORCEIT) == 0 && buf_Is_ਬਦਲਿਆ(buf))
	{
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ਜੇ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		ਜੇ (ਪਦ_ਕੰਮ_running(buf->b_ਪਦ))
		{
		    ਜੇ (ਪਦ_confirm_stop(buf) == FAIL)
			ਵਾਪਸ FAIL;
		}
		ਨਹੀਂਤਾਂ
# endif
		{
		    dialog_ਬਦਲਿਆ(buf, FALSE);
		    ਜੇ (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			ਵਾਪਸ FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    ਜੇ (buf_Is_ਬਦਲਿਆ(buf))
			ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		no_write_ਸੁਨੇਹਾ_buf(buf);
		ਵਾਪਸ FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	ਜੇ (buf == curbuf && VIsual_ਸਰਗਰਮ)
	    ਸਿਰਾ_visual_ਬਹੁਲਕ();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    ਜੇ (bp->b_p_bl && bp != buf)
		ਰੋਕੋ;
	ਜੇ (bp == ਨਿਰਾ && buf == curbuf)
	    ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	ਜਦੋਂ (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_ਬਫਰ->b_locked > 0)
		   && (!ONE_ਖਿੜਕੀ || ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ))
	{
	    ਜੇ (win_close(curwin, FALSE) == FAIL)
		ਰੋਕੋ;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	ਜੇ (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    ਜੇ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_ਬਫਰ(ਨਿਰਾ, buf, action, FALSE, FALSE, FALSE);
	    ਵਾਪਸ OK;
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
	buf = ਨਿਰਾ;	// selected buffer
	bp = ਨਿਰਾ;	// used when no loaded buffer found
	ਜੇ (au_new_curbuf.br_buf != ਨਿਰਾ && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_ਵੰਡੋ)
	    buf = au_new_curbuf.br_buf;
	ਨਹੀਂਤਾਂ ਜੇ (curwin->w_jumplistlen > 0)
	{
	    ਪੂਰਨ     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    ਜੇ (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    ਜਦੋਂ (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		ਜੇ (buf != ਨਿਰਾ)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    ਜੇ (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_ਵੰਡੋ)
			buf = ਨਿਰਾ;
		    ਨਹੀਂਤਾਂ ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
		    {
			// skip unloaded buf, but may keep it for later
			ਜੇ (bp == ਨਿਰਾ)
			    bp = buf;
			buf = ਨਿਰਾ;
		    }
		}
		ਜੇ (buf != ਨਿਰਾ)   // found a valid buffer: stop searching
		    ਰੋਕੋ;
		// advance to older entry in jump list
		ਜੇ (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    ਰੋਕੋ;
		ਜੇ (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		ਜੇ (jumpidx == forward)		// List exhausted for sure
		    ਰੋਕੋ;
	    }
	}

	ਜੇ (buf == ਨਿਰਾ)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_ਅਗਲਾ;
	    ਲਈ (;;)
	    {
		ਜੇ (buf == ਨਿਰਾ)
		{
		    ਜੇ (!forward)	// tried both directions
			ਰੋਕੋ;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    ਜਾਰੀ;
		}
		// in non-help buffer, try to skip help buffers, and vv
		ਜੇ (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_ਵੰਡੋ)
		{
		    ਜੇ (buf->b_ml.ml_mfp != ਨਿਰਾ)   // found loaded buffer
			ਰੋਕੋ;
		    ਜੇ (bp == ਨਿਰਾ)	// remember unloaded buf for later
			bp = buf;
		}
		ਜੇ (forward)
		    buf = buf->b_ਅਗਲਾ;
		ਨਹੀਂਤਾਂ
		    buf = buf->b_prev;
	    }
	}
	ਜੇ (buf == ਨਿਰਾ)	// No loaded buffer, use unloaded one
	    buf = bp;
	ਜੇ (buf == ਨਿਰਾ)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		ਜੇ (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_ਵੰਡੋ)
		    ਰੋਕੋ;
	}
	ਜੇ (buf == ਨਿਰਾ)	// Still no buffer, just take one
	{
	    ਜੇ (curbuf->b_ਅਗਲਾ != ਨਿਰਾ)
		buf = curbuf->b_ਅਗਲਾ;
	    ਨਹੀਂਤਾਂ
		buf = curbuf->b_prev;
	    ਜੇ (bt_quickfix(buf)
		    || (buf != ਨਿਰਾ && buf != curbuf && buf->b_locked_ਵੰਡੋ))
		buf = ਨਿਰਾ;
	}
    }

    ਜੇ (buf == ਨਿਰਾ)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    ਜੇ (action == DOBUF_ਵੰਡੋ && swbuf_goto_win_with_buf(buf) != ਨਿਰਾ)
	ਵਾਪਸ OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    ਜੇ (buf != curbuf && buf->b_locked_ਵੰਡੋ)
    {
	emsg(_(e_cannot_switch_ਮੰਜ਼ਿਲ_a_closing_ਬਫਰ));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (action == DOBUF_ਵੰਡੋ && win_ਵੰਡੋ(0, 0) == FAIL) // split window first
	ਵਾਪਸ FAIL;

    // go to current buffer - nothing to do
    ਜੇ (buf == curbuf)
	ਵਾਪਸ OK;

    // Check if the current buffer may be abandoned.
    ਜੇ (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ਜੇ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ਜੇ (ਪਦ_ਕੰਮ_running(curbuf->b_ਪਦ))
	    {
		ਜੇ (ਪਦ_confirm_stop(curbuf) == FAIL)
		    ਵਾਪਸ FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    ਨਹੀਂਤਾਂ
# endif
	    {
		bufref_T bufref;

		ਸੈੱਟ_bufref(&bufref, buf);
		dialog_ਬਦਲਿਆ(curbuf, FALSE);
		ਜੇ (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    ਵਾਪਸ FAIL;

		ਜੇ (buf_Is_ਬਦਲਿਆ(curbuf))
		{
		    no_write_ਸੁਨੇਹਾ();
		    ਵਾਪਸ FAIL;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	{
	    no_write_ਸੁਨੇਹਾ();
	    ਵਾਪਸ FAIL;
	}
    }

    // Go to the other buffer.
    ਸੈੱਟ_curbuf(buf, action);

    ਜੇ (action == DOBUF_ਵੰਡੋ)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#ਜੇ defined(FEAT_EVAL)
    ਜੇ (aborting())	    // autocmds may abort script processing
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

    ਪੂਰਨ
do_ਬਫਰ(
    ਪੂਰਨ		action,
    ਪੂਰਨ		ਸ਼ੁਰੂ,
    ਪੂਰਨ		dir,		// FORWARD or BACKWARD
    ਪੂਰਨ		ਗਿਣਤੀ_2,		// buffer number or number of buffers
    ਪੂਰਨ		forceit)	// TRUE when using !
{
    ਵਾਪਸ do_ਬਫਰ_ext(action, ਸ਼ੁਰੂ, dir, ਗਿਣਤੀ_2,
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
    ਅੱਖਰ *
do_bufdel(
    ਪੂਰਨ		command,
    char_u	*arg,		// pointer to extra arguments
    ਪੂਰਨ		addr_ਗਿਣਤੀ,
    ਪੂਰਨ		ਸ਼ੁਰੂ_bnr,	// first buffer number in a range
    ਪੂਰਨ		ਸਿਰਾ_bnr,	// buffer nr or last buffer nr in a range
    ਪੂਰਨ		forceit)
{
    ਪੂਰਨ		do_current = 0;	// delete current buffer?
    ਪੂਰਨ		deleted = 0;	// number of buffers deleted
    ਅੱਖਰ	*errormsg = ਨਿਰਾ; // return value
    ਪੂਰਨ		bnr;		// buffer number
    char_u	*p;

    ਜੇ (addr_ਗਿਣਤੀ == 0)
    {
	(ਖਾਲੀ)do_ਬਫਰ(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (addr_ਗਿਣਤੀ == 2)
	{
	    ਜੇ (*arg)		// both range and argument is not allowed
		ਵਾਪਸ ex_errmsg(e_trailing_characters_str, arg);
	    bnr = ਸ਼ੁਰੂ_bnr;
	}
	ਨਹੀਂਤਾਂ	// addr_count == 1
	    bnr = ਸਿਰਾ_bnr;

	ਲਈ ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    ਜੇ (bnr == curbuf->b_fnum)
		do_current = bnr;
	    ਨਹੀਂਤਾਂ ਜੇ (do_ਬਫਰ_ext(command, DOBUF_ਪਹਿਲਾ, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    ਜੇ (addr_ਗਿਣਤੀ == 2)
	    {
		ਜੇ (++bnr > ਸਿਰਾ_bnr)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ    // addr_count == 1
	    {
		arg = skipwhite(arg);
		ਜੇ (*arg == NUL)
		    ਰੋਕੋ;
		ਜੇ (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    ਜੇ (bnr < 0)	    // failed
			ਰੋਕੋ;
		    arg = p;
		}
		ਨਹੀਂਤਾਂ
		    bnr = getdigits(&arg);
	    }
	}
	ਜੇ (!got_int && do_current && do_ਬਫਰ(command, DOBUF_ਪਹਿਲਾ,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	ਜੇ (deleted == 0)
	{
	    ਜੇ (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    ਨਹੀਂਤਾਂ ਜੇ (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    ਨਹੀਂਤਾਂ
		STRCPY(IObuff, _(e_no_buffers_were_wiped_ਬਾਹਰਲਾ));
	    errormsg = (ਅੱਖਰ *)IObuff;
	}
	ਨਹੀਂਤਾਂ ਜੇ (deleted >= p_report)
	{
	    ਜੇ (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    ਨਹੀਂਤਾਂ ਜੇ (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    ਨਹੀਂਤਾਂ
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    ਵਾਪਸ errormsg;
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
    ਖਾਲੀ
ਸੈੱਟ_curbuf(buf_T *buf, ਪੂਰਨ action)
{
    buf_T	*prevbuf;
    ਪੂਰਨ		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    ਲੰਮਾ	old_tw = curbuf->b_p_tw;
#ਜੇ_ਅੰਤ
    bufref_T	newbufref;
    bufref_T	prevbufref;
    ਪੂਰਨ		valid;

    setpcmark();
    ਜੇ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = curbuf->b_fnum; // remember alternate file
    buflist_altfpos(curwin);			 // remember curpos

    // Don't restart Select mode after switching to another buffer.
    VIsual_reselect = FALSE;

    // close_windows() or apply_autocmds() may change curbuf and wipe out "buf"
    prevbuf = curbuf;
    ਸੈੱਟ_bufref(&prevbufref, prevbuf);
    ਸੈੱਟ_bufref(&newbufref, buf);

    // Autocommands may delete the current buffer and/or the buffer we want to
    // go to.  In those cases don't close the buffer.
    ਜੇ (!apply_autocmds(EVENT_BUFLEAVE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		&& !aborting()
#ਜੇ_ਅੰਤ
	       ))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਜੇ (prevbuf == curwin->w_ਬਫਰ)
	    reset_synblock(curwin);
#ਜੇ_ਅੰਤ
	ਜੇ (unload)
	    close_windows(prevbuf, FALSE);
#ਜੇ defined(FEAT_EVAL)
	ਜੇ (bufref_valid(&prevbufref) && !aborting())
#ਨਹੀਂਤਾਂ
	ਜੇ (bufref_valid(&prevbufref))
#ਜੇ_ਅੰਤ
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    ਜੇ (prevbuf == curbuf
		    && ((State & ਬਹੁਲਕ_ਪਾਓ) == 0 || curbuf->b_nwindows <= 1))
		u_sync(FALSE);
	    close_ਬਫਰ(curwin, prevbuf,
		    unload ? action : (action == DOBUF_GOTO
			&& !buf_hide(prevbuf)
			&& !buf_Is_ਬਦਲਿਆ(prevbuf)) ? DOBUF_UNLOAD : 0,
		    FALSE, FALSE, TRUE);
	}
    }
    // An autocommand may have deleted "buf", already entered it (e.g., when
    // it did ":bunload") or aborted the script processing.
    // If curwin->w_buffer is null, enter_buffer() will make it valid again
    valid = buf_valid(buf);
    ਜੇ ((valid && buf != curbuf
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		&& !aborting()
#ਜੇ_ਅੰਤ
	) || curwin->w_ਬਫਰ == ਨਿਰਾ)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	ਜੇ (!valid)
	    enter_ਬਫਰ(lastbuf);
	ਨਹੀਂਤਾਂ
	    enter_ਬਫਰ(buf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
	ਜੇ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ਨਿਰਾ, curwin);
#ਜੇ_ਅੰਤ
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    ਸਥਿਰ ਖਾਲੀ
enter_ਬਫਰ(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    ਜੇ (VIsual_ਸਰਗਰਮ
#ਜੇ defined(EXITFREE)
	    && !entered_free_all_mem
#ਜੇ_ਅੰਤ
	    )
	ਸਿਰਾ_visual_ਬਹੁਲਕ();

    ਜੇ (curwin->w_ਬਫਰ != ਨਿਰਾ)
	--curwin->w_ਬਫਰ->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_ਬਫਰ = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    ਜੇ (!buf->b_help)
	get_winopts(buf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    ਨਹੀਂਤਾਂ
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (curwin->w_p_diff)
	diff_buf_ਜੋੜੋ(curbuf);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    curwin->w_s = &(curbuf->b_s);
#ਜੇ_ਅੰਤ

    // Cursor on first line by default.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ਖਾਨਾ = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_ਸੈੱਟ_curswant = true;
    curwin->w_topline_was_ਸੈੱਟ = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // Make sure the buffer is loaded.
    ਜੇ (curbuf->b_ml.ml_mfp == ਨਿਰਾ)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	ਜੇ (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_ਬਫਰ(FALSE, ਨਿਰਾ, 0);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!ਸੁਨੇਹਾ_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(ਖਾਲੀ)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	curwin->w_topfill = 0;
#ਜੇ_ਅੰਤ
	apply_autocmds(EVENT_BUFENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    ਜੇ (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    ਜੇ (curwin->w_topline == 1 && !curwin->w_topline_was_ਸੈੱਟ)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#ਜੇ_ਅੰਤ

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
    ਜੇ (curbuf->b_kmap_state & KEYMAP_INIT)
	(ਖਾਲੀ)keymap_init();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    ਜੇ (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(ਖਾਲੀ)parse_spelllang(curwin);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    curbuf->b_ਆਖਰੀ_ਵਰਤਿਆ = vim_ਸਮਾਂ();
#ਜੇ_ਅੰਤ

    redraw_later(UPD_NOT_VALID);
}

#ਜੇ defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    ਖਾਲੀ
do_autochdir(ਖਾਲੀ)
{
    ਜੇ ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != ਨਿਰਾ
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	ਆਖਰੀ_chdir_reason = "autochdir";
    }
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
no_write_ਸੁਨੇਹਾ_buf(buf_T *buf)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (ਪਦ_ਕੰਮ_running(buf->b_ਪਦ))
	emsg(_(e_ਕੰਮ_still_running_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_ਸਿਰਾ_the_ਕੰਮ));
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	semsg(_(e_no_write_since_ਆਖਰੀ_change_for_ਬਫਰ_nr_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_override),
		buf->b_fnum);
}

    ਖਾਲੀ
no_write_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (ਪਦ_ਕੰਮ_running(curbuf->b_ਪਦ))
	emsg(_(e_ਕੰਮ_still_running_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_ਸਿਰਾ_the_ਕੰਮ));
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	emsg(_(e_no_write_since_ਆਖਰੀ_change_ਜੋੜੋ_bang_ਮੰਜ਼ਿਲ_override));
}

    ਖਾਲੀ
no_write_ਸੁਨੇਹਾ_nobang(buf_T *buf ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (ਪਦ_ਕੰਮ_running(buf->b_ਪਦ))
	emsg(_(e_ਕੰਮ_still_running));
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	emsg(_(e_no_write_since_ਆਖਰੀ_change));
}

/*
 * functions for dealing with the buffer list
 */

/*
 * Return TRUE if the current buffer is empty, unnamed, unmodified and used in
 * only one window.  That means it can be re-used.
 */
    ਪੂਰਨ
curbuf_reusable(ਖਾਲੀ)
{
    ਵਾਪਸ (curbuf != ਨਿਰਾ
	&& curbuf->b_ffname == ਨਿਰਾ
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == ਨਿਰਾ || BUFEMPTY())
	&& !bt_quickfix(curbuf)
	&& !curbuf_Is_ਬਦਲਿਆ());
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
    ਪੂਰਨ		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    stat_T	st;
#ਜੇ_ਅੰਤ

    ਜੇ (ਸਿਖਰ_file_ਅੰਕ == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    ਜੇ (sfname == ਨਿਰਾ || mch_stat((ਅੱਖਰ *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#ਜੇ_ਅੰਤ
    ਜੇ (ffname != ਨਿਰਾ && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		buflist_findname_stat(ffname, &st)
#ਨਹੀਂਤਾਂ
		buflist_findname(ffname)
#ਜੇ_ਅੰਤ
		) != ਨਿਰਾ)
    {
	vim_free(ffname);
	ਜੇ (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? ਨਿਰਾ : curwin,
						      lnum, (colnr_T)0, FALSE);

	ਜੇ ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	ਜੇ ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    ਸੈੱਟ_bufref(&bufref, buf);
	    ਜੇ (!(flags & BLN_DUMMY))
	    {
		ਜੇ (apply_autocmds(EVENT_BUFADD, ਨਿਰਾ, ਨਿਰਾ, FALSE, buf)
			&& !bufref_valid(&bufref))
		    ਵਾਪਸ ਨਿਰਾ;
	    }
	}
	ਵਾਪਸ buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = ਨਿਰਾ;
    ਜੇ ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	ਸੈੱਟ_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    ਵਾਪਸ ਨਿਰਾ;
	}
#ਜੇ_ਅੰਤ
	ਜੇ (!bufref_valid(&bufref))
	    buf = ਨਿਰਾ;		// buf was deleted; allocate a new buffer
    }
    ਜੇ (buf != curbuf || curbuf == ਨਿਰਾ)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	ਜੇ (buf == ਨਿਰਾ)
	{
	    vim_free(ffname);
	    ਵਾਪਸ ਨਿਰਾ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	ਜੇ (buf->b_vars == ਨਿਰਾ)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#ਜੇ_ਅੰਤ
	init_changedtick(buf);
    }

    ਜੇ (ffname != ਨਿਰਾ)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    ਜੇ ((ffname != ਨਿਰਾ && (buf->b_ffname == ਨਿਰਾ || buf->b_sfname == ਨਿਰਾ))
	    || buf->b_wininfo == ਨਿਰਾ)
    {
	ਜੇ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ਨਹੀਂਤਾਂ
	    buf->b_sfname = ਨਿਰਾ;
	VIM_CLEAR(buf->b_ffname);
	ਜੇ (buf != curbuf)
	    free_ਬਫਰ(buf);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (buf == curbuf)
    {
	free_ਬਫਰ_stuff(buf, FALSE);	// delete local variables et al.

	// Init the options.
	buf->b_p_initialized = false;
	buf_copy_options(buf, BCO_ENTER);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
	// need to reload lmaps and set b:keymap_name
	curbuf->b_kmap_state |= KEYMAP_INIT;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	// put the new buffer at the end of the buffer list
	buf->b_ਅਗਲਾ = ਨਿਰਾ;
	ਜੇ (firstbuf == ਨਿਰਾ)		// buffer list is empty
	{
	    buf->b_prev = ਨਿਰਾ;
	    firstbuf = buf;
	}
	ਨਹੀਂਤਾਂ				// append new buffer at end of list
	{
	    lastbuf->b_ਅਗਲਾ = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	ਜੇ ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((ਪੂਰਨ *)buf_reuse.ga_ਅੰਕੜੇ)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    ਜਦੋਂ (buf->b_prev != ਨਿਰਾ && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_ਅਗਲਾ = buf->b_ਅਗਲਾ;
		ਜੇ (prev->b_ਅਗਲਾ != ਨਿਰਾ)
		    prev->b_ਅਗਲਾ->b_prev = prev;
		buf->b_ਅਗਲਾ = prev;
		buf->b_prev = prev->b_prev;
		ਜੇ (buf->b_prev != ਨਿਰਾ)
		    buf->b_prev->b_ਅਗਲਾ = buf;
		prev->b_prev = buf;
		ਜੇ (lastbuf == buf)
		    lastbuf = prev;
		ਜੇ (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	ਨਹੀਂਤਾਂ
	    buf->b_fnum = ਸਿਖਰ_file_ਅੰਕ++;
	ਜੇ (ਸਿਖਰ_file_ਅੰਕ < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    ਜੇ (emsg_silent == 0 && !ਵਿੱਚ_assert_fails)
	    {
		ਬਾਹਰਲਾ_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    ਸਿਖਰ_file_ਅੰਕ = 1;
	}
	buf_hashtab_ਜੋੜੋ(buf);

	// Always copy the options from the current buffer.
	buf_copy_options(buf, BCO_ALWAYS);
    }

    buf->b_wininfo->wi_fpos.lnum = lnum;
    buf->b_wininfo->wi_win = curwin;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    hash_init(&buf->b_s.b_keywtab);
    hash_init(&buf->b_s.b_keywtab_ic);
#ਜੇ_ਅੰਤ

    buf->b_fname = buf->b_sfname;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਜੇ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ਨਹੀਂਤਾਂ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#ਜੇ_ਅੰਤ
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    ਜੇ (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_ਨਾਂ(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    ਜੇ (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	ਸੈੱਟ_bufref(&bufref, buf);
	ਜੇ (apply_autocmds(EVENT_BUFNEW, ਨਿਰਾ, ਨਿਰਾ, FALSE, buf)
		&& !bufref_valid(&bufref))
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (flags & BLN_LISTED)
	{
	    ਜੇ (apply_autocmds(EVENT_BUFADD, ਨਿਰਾ, ਨਿਰਾ, FALSE, buf)
		    && !bufref_valid(&bufref))
		ਵਾਪਸ ਨਿਰਾ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ਜੇ (aborting())		// autocmds may abort script processing
	    ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    }

    ਵਾਪਸ buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    ਖਾਲੀ
free_buf_options(
    buf_T	*buf,
    ਪੂਰਨ		free_p_ff)
{
    ਜੇ (free_p_ff)
    {
	clear_string_option(&buf->b_p_fenc);
	clear_string_option(&buf->b_p_ff);
	clear_string_option(&buf->b_p_bh);
	clear_string_option(&buf->b_p_bt);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਲੱਭੋ_ID
    clear_string_option(&buf->b_p_def);
    clear_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    clear_string_option(&buf->b_p_inex);
# endif
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#ਜੇ_ਅੰਤ
    clear_string_option(&buf->b_p_fp);
#ਜੇ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CRYPT
# ifdef FEAT_SODIUM
    ਜੇ (buf->b_p_ਕੁੰਜੀ != ਨਿਰਾ && *buf->b_p_ਕੁੰਜੀ != NUL
			   && crypt_ਵਿਧੀ_is_sodium(crypt_get_ਵਿਧੀ_nr(buf)))
	crypt_sodium_munlock(buf->b_p_ਕੁੰਜੀ, STRLEN(buf->b_p_ਕੁੰਜੀ));
# endif
    clear_string_option(&buf->b_p_ਕੁੰਜੀ);
#ਜੇ_ਅੰਤ
    clear_string_option(&buf->b_p_kp);
    clear_string_option(&buf->b_p_mps);
    clear_string_option(&buf->b_p_fo);
    clear_string_option(&buf->b_p_flp);
    clear_string_option(&buf->b_p_isk);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VARTABS
    clear_string_option(&buf->b_p_vsts);
    VIM_CLEAR(buf->b_p_vsts_nopaste);
    VIM_CLEAR(buf->b_p_vsts_array);
    clear_string_option(&buf->b_p_vts);
    VIM_CLEAR(buf->b_p_vts_array);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_KEYMAP
    clear_string_option(&buf->b_p_keymap);
    keymap_clear(&buf->b_kmap_ga);
    ga_clear(&buf->b_kmap_ga);
#ਜੇ_ਅੰਤ
    clear_string_option(&buf->b_p_com);
    clear_string_option(&buf->b_p_cms);
    clear_string_option(&buf->b_p_nf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYN_HL
    clear_string_option(&buf->b_p_syn);
    clear_string_option(&buf->b_s.b_syn_isk);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SPELL
    clear_string_option(&buf->b_s.b_p_spc);
    clear_string_option(&buf->b_s.b_p_spf);
    vim_regfree(buf->b_s.b_cap_prog);
    buf->b_s.b_cap_prog = ਨਿਰਾ;
    clear_string_option(&buf->b_s.b_p_spl);
    clear_string_option(&buf->b_s.b_p_spo);
#ਜੇ_ਅੰਤ
    clear_string_option(&buf->b_p_sua);
    clear_string_option(&buf->b_p_ft);
    clear_string_option(&buf->b_p_cink);
    clear_string_option(&buf->b_p_cino);
    clear_string_option(&buf->b_p_lop);
    clear_string_option(&buf->b_p_cinsd);
    clear_string_option(&buf->b_p_cinw);
    clear_string_option(&buf->b_p_cot);
    clear_string_option(&buf->b_p_cpt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
    clear_string_option(&buf->b_p_csl);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_COMPL_FUNC
    clear_string_option(&buf->b_p_cfu);
    free_callback(&buf->b_cfu_cb);
    clear_string_option(&buf->b_p_ofu);
    free_callback(&buf->b_ofu_cb);
    clear_string_option(&buf->b_p_tsrfu);
    free_callback(&buf->b_tsrfu_cb);
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_ਗਿਣਤੀ);
    buf->b_p_cpt_ਗਿਣਤੀ = 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    clear_string_option(&buf->b_p_gefm);
    clear_string_option(&buf->b_p_gp);
    clear_string_option(&buf->b_p_mp);
    clear_string_option(&buf->b_p_efm);
#ਜੇ_ਅੰਤ
    clear_string_option(&buf->b_p_ep);
    clear_string_option(&buf->b_p_ਰਸਤਾ);
    clear_string_option(&buf->b_p_ਟੈਗ_ਸੂਚੀ);
    clear_string_option(&buf->b_p_tc);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    clear_string_option(&buf->b_p_tfu);
    free_callback(&buf->b_tfu_cb);
    clear_string_option(&buf->b_p_ffu);
    free_callback(&buf->b_ffu_cb);
#ਜੇ_ਅੰਤ
    clear_string_option(&buf->b_p_dict);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    clear_string_option(&buf->b_p_dia);
#ਜੇ_ਅੰਤ
    clear_string_option(&buf->b_p_tsr);
    clear_string_option(&buf->b_p_qe);
    buf->b_p_ac = -1;
    buf->b_p_ar = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FSYNC
    buf->b_p_fs = -1;
#ਜੇ_ਅੰਤ
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
    ਪੂਰਨ
buflist_getfile(
    ਪੂਰਨ		n,
    linenr_T	lnum,
    ਪੂਰਨ		options,
    ਪੂਰਨ		forceit)
{
    buf_T	*buf;
    win_T	*wp = ਨਿਰਾ;
    pos_T	*fpos;
    colnr_T	ਖਾਨਾ;

    buf = buflist_findnr(n);
    ਜੇ (buf == ਨਿਰਾ)
    {
	ਜੇ ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_ਬਫਰ_nr_not_ਮਿਲਿਆ), n);
	ਵਾਪਸ FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    ਜੇ (buf == curbuf)
	ਵਾਪਸ OK;

    ਜੇ (ਪਾਠ_or_buf_locked())
	ਵਾਪਸ FAIL;

    // altfpos may be changed by getfile(), get it now
    ਜੇ (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	ਖਾਨਾ = fpos->ਖਾਨਾ;
    }
    ਨਹੀਂਤਾਂ
	ਖਾਨਾ = 0;

    ਜੇ (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	ਜੇ (wp == ਨਿਰਾ && (swb_flags & (SWB_VSPLIT | SWB_ਵੰਡੋ | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    ਜੇ (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    ਨਹੀਂਤਾਂ ਜੇ (win_ਵੰਡੋ(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		ਵਾਪਸ FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    ਪੂਰਨ retval = FAIL;
    ਜੇ (GETFILE_SUCCESS(getfile(buf->b_fnum, ਨਿਰਾ, ਨਿਰਾ,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	ਜੇ (!p_sol && ਖਾਨਾ != 0)
	{
	    curwin->w_cursor.ਖਾਨਾ = ਖਾਨਾ;
	    check_cursor_ਖਾਨਾ();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_ਸੈੱਟ_curswant = true;
	}
	retval = OK;
    }

    ਜੇ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ਵਾਪਸ retval;
}

/*
 * go to the last know line number for the current buffer
 */
    ਸਥਿਰ ਖਾਲੀ
buflist_getfpos(ਖਾਲੀ)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    ਜੇ (p_sol)
	curwin->w_cursor.ਖਾਨਾ = 0;
    ਨਹੀਂਤਾਂ
    {
	curwin->w_cursor.ਖਾਨਾ = fpos->ਖਾਨਾ;
	check_cursor_ਖਾਨਾ();
	curwin->w_cursor.coladd = 0;
	curwin->w_ਸੈੱਟ_curswant = true;
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
    buf_T	*buf = ਨਿਰਾ;

    // First make the name into a full path name
    ffname = ਭਰਿਆ_Name_save(fname,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#ਨਹੀਂਤਾਂ
	    FALSE
#ਜੇ_ਅੰਤ
	    );
    ਜੇ (ffname != ਨਿਰਾ)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    ਵਾਪਸ buf;
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    stat_T	st;

    ਜੇ (mch_stat((ਅੱਖਰ *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    ਵਾਪਸ buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    ਸਥਿਰ buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#ਜੇ_ਅੰਤ
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_ਆਖਰੀ(buf)
	ਜੇ ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
		    , stp
#ਜੇ_ਅੰਤ
		    ))
	    ਵਾਪਸ buf;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    ਪੂਰਨ
buflist_findpat(
    char_u	*ਨਮੂਨਾ,
    char_u	*ਨਮੂਨਾ_ਸਿਰਾ,	// pointer to first char after pattern
    ਪੂਰਨ		unlisted,	// find unlisted buffers
    ਪੂਰਨ		diffmode ਅਣਵਰਤਿਆ, // find diff-mode buffers only
    ਪੂਰਨ		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    ਪੂਰਨ		ਮੇਲ = -1;
    ਪੂਰਨ		ਲੱਭੋ_listed;
    char_u	*pat;
    char_u	*patend;
    ਪੂਰਨ		attempt;
    char_u	*p;
    ਪੂਰਨ		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    ਜੇ ((ਨਮੂਨਾ_ਸਿਰਾ == ਨਮੂਨਾ + 1 && (*ਨਮੂਨਾ == '%' || *ਨਮੂਨਾ == '#'))
	    || (ਵਿੱਚ_vim9script() && ਨਮੂਨਾ_ਸਿਰਾ == ਨਮੂਨਾ + 2
				    && ਨਮੂਨਾ[0] == '%' && ਨਮੂਨਾ[1] == '%'))
    {
	ਜੇ (*ਨਮੂਨਾ == '#' || ਨਮੂਨਾ_ਸਿਰਾ == ਨਮੂਨਾ + 2)
	    ਮੇਲ = curwin->w_alt_fnum;
	ਨਹੀਂਤਾਂ
	    ਮੇਲ = curbuf->b_fnum;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	ਜੇ (diffmode && !diff_ਬਹੁਲਕ_buf(buflist_findnr(ਮੇਲ)))
	    ਮੇਲ = -1;
#ਜੇ_ਅੰਤ
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
    ਨਹੀਂਤਾਂ
    {
	pat = file_pat_ਮੰਜ਼ਿਲ_reg_pat(ਨਮੂਨਾ, ਨਮੂਨਾ_ਸਿਰਾ, ਨਿਰਾ, FALSE);
	ਜੇ (pat == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	ਲੱਭੋ_listed = TRUE;
	ਲਈ (;;)
	{
	    ਲਈ (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		ਜੇ (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		ਜੇ (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_ਆਖਰੀ(buf)
		{
		    ਜੇ (regmatch.regprog == ਨਿਰਾ)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			ਵਾਪਸ -1;
		    }
		    ਜੇ (buf->b_p_bl == ਲੱਭੋ_listed
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			    && (!diffmode || diff_ਬਹੁਲਕ_buf(buf))
#ਜੇ_ਅੰਤ
			    && buflist_ਮੇਲ(&regmatch, buf, FALSE) != ਨਿਰਾ)
		    {
			ਜੇ (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				ਜੇ (wp->w_ਬਫਰ == buf)
				    ਰੋਕੋ;
			    ਜੇ (wp == ਨਿਰਾ)
				ਜਾਰੀ;
			}
			ਜੇ (ਮੇਲ >= 0)		// already found a match
			{
			    ਮੇਲ = -2;
			    ਰੋਕੋ;
			}
			ਮੇਲ = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		ਜੇ (ਮੇਲ >= 0)			// found one match
		    ਰੋਕੋ;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    ਜੇ (!unlisted || !ਲੱਭੋ_listed || ਮੇਲ != -1)
		ਰੋਕੋ;
	    ਲੱਭੋ_listed = FALSE;
	}

	vim_free(pat);
    }

    ਜੇ (ਮੇਲ == -2)
	semsg(_(e_ਹੋਰ_than_one_ਮੇਲ_for_str), ਨਮੂਨਾ);
    ਨਹੀਂਤਾਂ ਜੇ (ਮੇਲ < 0)
	semsg(_(e_no_matching_ਬਫਰ_for_str), ਨਮੂਨਾ);
    ਵਾਪਸ ਮੇਲ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    buf_T   *buf;
    char_u  *ਮੇਲ;
} bufmatch_T;
#ਜੇ_ਅੰਤ

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    ਪੂਰਨ
ExpandBufnames(
    char_u	*pat,
    ਪੂਰਨ		*ਅੰਕ_file,
    char_u	***file,
    ਪੂਰਨ		options)
{
    ਪੂਰਨ		ਗਿਣਤੀ_2;
    buf_T	*buf;
    ਪੂਰਨ		ਗੇੜਾ;
    char_u	*p;
    char_u	*patc = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    bufmatch_T	*matches = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		fuzzy;
    fuzmatch_str_T  *fuzmatch = ਨਿਰਾ;
    regmatch_T	regmatch;
    ਪੂਰਨ		score = 0;
    ਪੂਰਨ		ਮੰਜ਼ਿਲ_free = FALSE;

    *ਅੰਕ_file = 0;		    // return values in case of FAIL
    *file = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    ਜੇ (!fuzzy)
    {
	ਜੇ (*pat == '^' && pat[1] != NUL)
	{
	    ਪੂਰਨ  len = (ਪੂਰਨ)STRLEN(pat);
	    patc = alloc(len);
	    ਜੇ (patc == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    ਮੰਜ਼ਿਲ_free = TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*pat == '^')
	    patc = (char_u *)"";
	ਨਹੀਂਤਾਂ
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    ਲਈ (ਗੇੜਾ = 1; ਗੇੜਾ <= 2; ++ਗੇੜਾ)
    {
	ਗਿਣਤੀ_2 = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    ਜੇ (!buf->b_p_bl)	// skip unlisted buffers
		ਜਾਰੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
	    ਜੇ (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		ਜੇ (buf == curbuf || !diff_ਬਹੁਲਕ_buf(buf))
		    ਜਾਰੀ;
#ਜੇ_ਅੰਤ

	    ਜੇ (!fuzzy)
	    {
		ਜੇ (regmatch.regprog == ਨਿਰਾ)
		{
		    // invalid pattern, possibly after recompiling
		    ਜੇ (ਮੰਜ਼ਿਲ_free)
			vim_free(patc);
		    ਵਾਪਸ FAIL;
		}
		p = buflist_ਮੇਲ(&regmatch, buf, p_wic);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		p = ਨਿਰਾ;
		// first try matching with the short file name
		ਜੇ ((score = fuzzy_ਮੇਲ_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		ਜੇ (p == ਨਿਰਾ)
		{
		    // next try matching with the full path file name
		    ਜੇ ((score = fuzzy_ਮੇਲ_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    ਜੇ (p == ਨਿਰਾ)
		ਜਾਰੀ;

	    ਜੇ (ਗੇੜਾ == 1)
	    {
		++ਗਿਣਤੀ_2;
		ਜਾਰੀ;
	    }

	    ਜੇ (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    ਨਹੀਂਤਾਂ
		p = vim_strsave(p);
	    ਜੇ (p == ਨਿਰਾ)
	    {
		ਜੇ (fuzzy && ਗੇੜਾ == 2)
		    fuzmatch_str_free(fuzmatch, ਗਿਣਤੀ_2);
		ਵਾਪਸ FAIL;
	    }

	    ਜੇ (!fuzzy)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
		ਜੇ (matches != ਨਿਰਾ)
		{
		    matches[ਗਿਣਤੀ_2].buf = buf;
		    matches[ਗਿਣਤੀ_2].ਮੇਲ = p;
		    ਗਿਣਤੀ_2++;
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    (*file)[ਗਿਣਤੀ_2++] = p;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		fuzmatch[ਗਿਣਤੀ_2].idx = ਗਿਣਤੀ_2;
		fuzmatch[ਗਿਣਤੀ_2].str = p;
		fuzmatch[ਗਿਣਤੀ_2].score = score;
		ਗਿਣਤੀ_2++;
	    }
	}
	ਜੇ (ਗਿਣਤੀ_2 == 0)	// no match found, break here
	    ਰੋਕੋ;
	ਜੇ (ਗੇੜਾ == 1)
	{
	    ਜੇ (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, ਗਿਣਤੀ_2);
		ਜੇ (*file == ਨਿਰਾ)
		{
		    vim_regfree(regmatch.regprog);
		    ਜੇ (ਮੰਜ਼ਿਲ_free)
			vim_free(patc);
		    ਵਾਪਸ FAIL;
		}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
		ਜੇ (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, ਗਿਣਤੀ_2);
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, ਗਿਣਤੀ_2);
		ਜੇ (fuzmatch == ਨਿਰਾ)
		{
		    *ਅੰਕ_file = 0;
		    *file = ਨਿਰਾ;
		    ਵਾਪਸ FAIL;
		}
	    }
	}
    }

    ਜੇ (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	ਜੇ (ਮੰਜ਼ਿਲ_free)
	    vim_free(patc);
    }

    ਜੇ (!fuzzy)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
	ਜੇ (matches != ਨਿਰਾ)
	{
	    ਪੂਰਨ i;
	    ਜੇ (ਗਿਣਤੀ_2 > 1)
		qsort(matches, ਗਿਣਤੀ_2, ਆਕਾਰ(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    ਜੇ (matches[0].buf == curbuf)
	    {
		ਲਈ (i = 1; i < ਗਿਣਤੀ_2; i++)
		    (*file)[i-1] = matches[i].ਮੇਲ;
		(*file)[ਗਿਣਤੀ_2-1] = matches[0].ਮੇਲ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਲਈ (i = 0; i < ਗਿਣਤੀ_2; i++)
		    (*file)[i] = matches[i].ਮੇਲ;
	    }
	    vim_free(matches);
	}
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (fuzzymatches_ਮੰਜ਼ਿਲ_strmatches(fuzmatch, file, ਗਿਣਤੀ_2, FALSE) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    *ਅੰਕ_file = ਗਿਣਤੀ_2;
    ਵਾਪਸ (ਗਿਣਤੀ_2 == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    ਸਥਿਰ char_u *
buflist_ਮੇਲ(
    regmatch_T	*rmp,
    buf_T	*buf,
    ਪੂਰਨ		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*ਮੇਲ;

    // First try the short file name, then the long file name.
    ਮੇਲ = fname_ਮੇਲ(rmp, buf->b_sfname, ignore_case);
    ਜੇ (ਮੇਲ == ਨਿਰਾ && rmp->regprog != ਨਿਰਾ)
	ਮੇਲ = fname_ਮੇਲ(rmp, buf->b_ffname, ignore_case);

    ਵਾਪਸ ਮੇਲ;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    ਸਥਿਰ char_u *
fname_ਮੇਲ(
    regmatch_T	*rmp,
    char_u	*name,
    ਪੂਰਨ		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*ਮੇਲ = ਨਿਰਾ;
    char_u	*p;

    // extra check for valid arguments
    ਜੇ (name == ਨਿਰਾ || rmp->regprog == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    ਜੇ (vim_regexec(rmp, name, (colnr_T)0))
	ਮੇਲ = name;
    ਨਹੀਂਤਾਂ ਜੇ (rmp->regprog != ਨਿਰਾ)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(ਨਿਰਾ, name);
	ਜੇ (p != ਨਿਰਾ && vim_regexec(rmp, p, (colnr_T)0))
	    ਮੇਲ = name;
	vim_free(p);
    }

    ਵਾਪਸ ਮੇਲ;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(ਪੂਰਨ nr)
{
    char_u	ਕੁੰਜੀ[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    ਜੇ (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((ਅੱਖਰ *)ਕੁੰਜੀ, ਆਕਾਰ(ਕੁੰਜੀ), "%x", nr);
    hi = hash_ਲੱਭੋ(&buf_hashtab, ਕੁੰਜੀ);

    ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
	ਵਾਪਸ (buf_T *)(hi->hi_ਕੁੰਜੀ
			     - ((ਬਿਨਾਨਿਸ਼ਾਨੀ)(curbuf->b_ਕੁੰਜੀ - (char_u *)curbuf)));
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    ਪੂਰਨ		n,
    ਪੂਰਨ		fullname,
    ਪੂਰਨ		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ home_replace_save(helptail ? buf : ਨਿਰਾ,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    ਖਾਲੀ
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	ਖਾਨਾ,
    ਪੂਰਨ		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ਜੇ (wip->wi_win == win)
	    ਰੋਕੋ;
    ਜੇ (wip == ਨਿਰਾ)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	ਜੇ (wip == ਨਿਰਾ)
	    ਵਾਪਸ;
	wip->wi_win = win;
	ਜੇ (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    ਨਹੀਂਤਾਂ
    {
	// remove the entry from the list
	ਜੇ (wip->wi_prev)
	    wip->wi_prev->wi_ਅਗਲਾ = wip->wi_ਅਗਲਾ;
	ਨਹੀਂਤਾਂ
	    buf->b_wininfo = wip->wi_ਅਗਲਾ;
	ਜੇ (wip->wi_ਅਗਲਾ)
	    wip->wi_ਅਗਲਾ->wi_prev = wip->wi_prev;
	ਜੇ (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#ਜੇ_ਅੰਤ
	}
    }
    ਜੇ (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.ਖਾਨਾ = ਖਾਨਾ;
    }
    ਜੇ (win != ਨਿਰਾ)
	wip->wi_changelistidx = win->w_changelistidx;
    ਜੇ (copy_options && win != ਨਿਰਾ)
    {
	// Save the window-specific option values.
	copy_winopt(&win->w_onebuf_opt, &wip->wi_opt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	wip->wi_fold_manual = win->w_fold_manual;
	cloneFoldGrowArray(&win->w_folds, &wip->wi_folds);
#ਜੇ_ਅੰਤ
	wip->wi_optset = TRUE;
    }

    // insert the entry in front of the list
    wip->wi_ਅਗਲਾ = buf->b_wininfo;
    buf->b_wininfo = wip;
    wip->wi_prev = ਨਿਰਾ;
    ਜੇ (wip->wi_ਅਗਲਾ)
	wip->wi_ਅਗਲਾ->wi_prev = wip;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    ਸਥਿਰ ਪੂਰਨ
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    ਜੇ (!wip->wi_opt.wo_diff)
	ਵਾਪਸ FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	ਜੇ (wip->wi_win == wp)
	    ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ

/*
 * Find info for the current window in buffer "buf".
 * If not found, return the info for the most recently used window.
 * When "need_options" is TRUE skip entries where wi_optset is FALSE.
 * When "skip_diff_buffer" is TRUE avoid windows with 'diff' set that is in
 * another tab page.
 * Returns NULL when there isn't any info.
 */
    ਸਥਿਰ wininfo_T *
ਲੱਭੋ_wininfo(
    buf_T	*buf,
    ਪੂਰਨ		need_options,
    ਪੂਰਨ		skip_diff_ਬਫਰ ਅਣਵਰਤਿਆ)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ਜੇ (wip->wi_win == curwin
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		&& (!skip_diff_ਬਫਰ || !wininfo_other_tab_diff(wip))
#ਜੇ_ਅੰਤ

		&& (!need_options || wip->wi_optset))
	    ਰੋਕੋ;

    ਜੇ (wip != ਨਿਰਾ)
	ਵਾਪਸ wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    ਜੇ (skip_diff_ਬਫਰ)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    ਜੇ (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != ਨਿਰਾ
			    && wip->wi_win->w_ਬਫਰ == buf)))
		ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	wip = buf->b_wininfo;
    ਵਾਪਸ wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    ਖਾਲੀ
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    clearFolding(curwin);
#ਜੇ_ਅੰਤ

    wip = ਲੱਭੋ_wininfo(buf, TRUE, TRUE);
    ਜੇ (wip != ਨਿਰਾ && wip->wi_win != ਨਿਰਾ
	    && wip->wi_win != curwin && wip->wi_win->w_ਬਫਰ == buf)
    {
	// The buffer is currently displayed in the window: use the actual
	// option values instead of the saved (possibly outdated) values.
	win_T *wp = wip->wi_win;

	copy_winopt(&wp->w_onebuf_opt, &curwin->w_onebuf_opt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
	// w_stl_rendered_height is not in winvar_T; copy it explicitly.
	curwin->w_stl_rendered_ਉਚਾਈ = wp->w_stl_rendered_ਉਚਾਈ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	curwin->w_fold_manual = wp->w_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wp->w_folds, &curwin->w_folds);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ ਜੇ (wip != ਨਿਰਾ && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    ਜੇ (wip != ਨਿਰਾ)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    ਜੇ (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    ਜੇ (*curwin->w_p_stl != NUL)
	update_stl_rendered_ਉਚਾਈ(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    ਜੇ (curwin->w_ਸਥਿਤੀ_ਉਚਾਈ > 0
	    && curwin->w_ਸਥਿਤੀ_ਉਚਾਈ != statusline_ਉਚਾਈ(curwin))
    {
	ਚੌਖਟਾ_change_statusline_ਉਚਾਈ();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	ਜੇ (p_ea)
	    win_equal(curwin, TRUE, 'v');
    }
#ਜੇ_ਅੰਤ
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
    ਸਥਿਰ pos_T no_position = {1, 0, 0};

    wip = ਲੱਭੋ_wininfo(buf, FALSE, FALSE);
    ਜੇ (wip != ਨਿਰਾ)
	ਵਾਪਸ &(wip->wi_fpos);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    ਵਾਪਸ buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    ਖਾਲੀ
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;
    ਪੂਰਨ		ro_char;
    ਪੂਰਨ		ਬਦਲਿਆ_char;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪੂਰਨ		ਕੰਮ_running;
    ਪੂਰਨ		ਕੰਮ_none_open;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_ਅੰਕੜੇ = ਨਿਰਾ, **p;

    ਜੇ (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, ਆਕਾਰ(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    ਜੇ (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_ਅੰਕੜੇ)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_ਅੰਕੜੇ, (size_t)buflist.ga_len,
		ਆਕਾਰ(buf_T *), buf_compare);

	buflist_ਅੰਕੜੇ = (buf_T **)buflist.ga_ਅੰਕੜੇ;
	buf = *buflist_ਅੰਕੜੇ;
    }
    p = buflist_ਅੰਕੜੇ;

    ਲਈ (; buf != ਨਿਰਾ && !got_int; buf = buflist_ਅੰਕੜੇ != ਨਿਰਾ
	    ? (++p < buflist_ਅੰਕੜੇ + buflist.ga_len ? *p : ਨਿਰਾ)
	    : buf->b_ਅਗਲਾ)
#ਨਹੀਂਤਾਂ
    ਲਈ (buf = firstbuf; buf != ਨਿਰਾ && !got_int; buf = buf->b_ਅਗਲਾ)
#ਜੇ_ਅੰਤ
    {
	char_u	*name;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	ਕੰਮ_running = ਪਦ_ਕੰਮ_running(buf->b_ਪਦ);
	ਕੰਮ_none_open = ਪਦ_none_open(buf->b_ਪਦ);
#ਜੇ_ਅੰਤ
	// skip unlisted buffers, unless ! was used
	ਜੇ ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !buf_Is_ਬਦਲਿਆ(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == ਨਿਰਾ || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == ਨਿਰਾ || buf->b_nwindows != 0))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!ਕੰਮ_running || (ਕੰਮ_running && ਕੰਮ_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!ਕੰਮ_running || (ਕੰਮ_running && !ਕੰਮ_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (ਕੰਮ_running || buf->b_ਪਦ == ਨਿਰਾ))
#ਜੇ_ਅੰਤ
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    ਜਾਰੀ;
	name = buf_spname(buf);
	ਜੇ (name != ਨਿਰਾ)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	ਨਹੀਂਤਾਂ
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	ਜੇ (ਸੁਨੇਹਾ_filtered(NameBuff))
	    ਜਾਰੀ;

	ਬਦਲਿਆ_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (buf_Is_ਬਦਲਿਆ(buf) ? '+' : ' ');
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	ਜੇ (ਕੰਮ_running)
	{
	    ਜੇ (ਕੰਮ_none_open)
		ro_char = '?';
	    ਨਹੀਂਤਾਂ
		ro_char = 'R';
	    ਬਦਲਿਆ_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	ਨਹੀਂਤਾਂ ਜੇ (buf->b_ਪਦ != ਨਿਰਾ)
	    ro_char = 'F';
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	ਸੁਨੇਹਾ_putchar('\n');
	len = (ਪੂਰਨ)vim_snprintf_safelen((ਅੱਖਰ *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == ਨਿਰਾ ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		ਬਦਲਿਆ_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	ਕਰੋ
	    IObuff[len++] = ' ';
	ਜਦੋਂ (--i > 0 && len < IOSIZE - 18);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
	ਜੇ (vim_strchr(eap->arg, 't') && buf->b_ਆਖਰੀ_ਵਰਤਿਆ)
	    ਜੋੜੋ_ਸਮਾਂ(IObuff + len, (size_t)(IOSIZE - len), buf->b_ਆਖਰੀ_ਵਰਤਿਆ);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    vim_snprintf((ਅੱਖਰ *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (ਲੰਮਾ)buflist_findlnum(buf));
	ਸੁਨੇਹਾ_outtrans(IObuff);
	ਬਾਹਰਲਾ_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
    ਜੇ (buflist_ਅੰਕੜੇ)
	ga_clear(&buflist);
#ਜੇ_ਅੰਤ
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    ਪੂਰਨ
buflist_name_nr(
    ਪੂਰਨ		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ਜੇ (buf == ਨਿਰਾ || buf->b_fname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    ਵਾਪਸ OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    ਪੂਰਨ
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    ਪੂਰਨ		ਸੁਨੇਹਾ_2)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    stat_T	st;
#ਜੇ_ਅੰਤ

    ਜੇ (ffname == ਨਿਰਾ || *ffname == NUL)
    {
	// Removing the name.
	ਜੇ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ਨਹੀਂਤਾਂ
	    buf->b_sfname = ਨਿਰਾ;
	VIM_CLEAR(buf->b_ffname);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	st.st_dev = (dev_T)-1;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	ਜੇ (ffname == ਨਿਰਾ)		    // out of memory
	    ਵਾਪਸ FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	ਜੇ (mch_stat((ਅੱਖਰ *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#ਜੇ_ਅੰਤ
	ਜੇ (!(buf->b_flags & BF_DUMMY))
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#ਨਹੀਂਤਾਂ
	    obuf = buflist_findname(ffname);
#ਜੇ_ਅੰਤ
	ਜੇ (obuf != ਨਿਰਾ && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    ਪੂਰਨ		ਵਿੱਚ_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		ਜੇ (win->w_ਬਫਰ == obuf)
		    ਵਿੱਚ_use = TRUE;

	    // it's loaded or used in a window, fail
	    ਜੇ (obuf->b_ml.ml_mfp != ਨਿਰਾ || ਵਿੱਚ_use)
	    {
		ਜੇ (ਸੁਨੇਹਾ_2)
		    emsg(_(e_ਬਫਰ_with_this_name_already_exists));
		vim_free(ffname);
		ਵਾਪਸ FAIL;
	    }
	    // delete from the list
	    close_ਬਫਰ(ਨਿਰਾ, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	ਜੇ (ffname == ਨਿਰਾ || sfname == ਨਿਰਾ)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    ਵਾਪਸ FAIL;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#ਜੇ_ਅੰਤ
	ਜੇ (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਜੇ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ਨਹੀਂਤਾਂ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#ਜੇ_ਅੰਤ

    buf->b_shortname = false;

    buf_name_ਬਦਲਿਆ(buf);
    ਵਾਪਸ OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    ਖਾਲੀ
buf_ਸੈੱਟ_name(ਪੂਰਨ fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = ਨਿਰਾ;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    ਖਾਲੀ
buf_name_ਬਦਲਿਆ(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    ਜੇ (buf->b_ml.ml_mfp != ਨਿਰਾ)
	ml_setname(buf);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਜੇ (buf->b_ਪਦ != ਨਿਰਾ)
	ਪਦ_clear_ਸਥਿਤੀ_ਪਾਠ(buf->b_ਪਦ);
#ਜੇ_ਅੰਤ

    ਜੇ (curwin->w_ਬਫਰ == buf)
	check_arg_idx(curwin);	// check file name for arg list
    maketitle();		// set window title
    ਸਥਿਤੀ_redraw_all();	// status lines need to be redrawn
    fmarks_check_ਨਾਂ(buf);	// check named file marks
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
    ਜੇ (buf != ਨਿਰਾ && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    ਵਾਪਸ buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    ਪੂਰਨ		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    ਜੇ (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	ਜੇ (errmsg)
	    emsg(_(e_no_alternate_file));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    ਪੂਰਨ
buflist_ਜੋੜੋ(char_u *fname, ਪੂਰਨ flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, ਨਿਰਾ, (linenr_T)0, flags);
    ਜੇ (buf != ਨਿਰਾ)
	ਵਾਪਸ buf->b_fnum;
    ਵਾਪਸ 0;
}

#ਜੇ defined(BACKSLASH_ਵਿੱਚ_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    ਖਾਲੀ
buflist_slash_adjust(ਖਾਲੀ)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	ਜੇ (bp->b_ffname != ਨਿਰਾ)
	    slash_adjust(bp->b_ffname);
	ਜੇ (bp->b_sfname != ਨਿਰਾ)
	    slash_adjust(bp->b_sfname);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    ਖਾਲੀ
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.ਖਾਨਾ, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    ਪੂਰਨ
otherfile(char_u *ffname)
{
    ਵਾਪਸ otherfile_buf(curbuf, ffname
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
	    , ਨਿਰਾ
#ਜੇ_ਅੰਤ
	    );
}

    ਸਥਿਰ ਪੂਰਨ
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    , stat_T		*stp
#ਜੇ_ਅੰਤ
    )
{
    // no name is different
    ਜੇ (ffname == ਨਿਰਾ || *ffname == NUL || buf->b_ffname == ਨਿਰਾ)
	ਵਾਪਸ TRUE;
    ਜੇ (fnamecmp(ffname, buf->b_ffname) == 0)
	ਵਾਪਸ FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	ਜੇ (stp == ਨਿਰਾ)
	{
	    ਜੇ (!buf->b_dev_valid || mch_stat((ਅੱਖਰ *)ffname, &st) < 0)
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
	ਜੇ (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    ਜੇ (buf_same_ino(buf, stp))
		ਵਾਪਸ FALSE;
	}
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ TRUE;
}

#ਜੇ defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    ਖਾਲੀ
buf_setino(buf_T *buf)
{
    stat_T	st;

    ਜੇ (buf->b_fname != ਨਿਰਾ && mch_stat((ਅੱਖਰ *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    ਨਹੀਂਤਾਂ
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    ਸਥਿਰ ਪੂਰਨ
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    ਵਾਪਸ (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#ਜੇ_ਅੰਤ

/*
 * Print info about the current buffer.
 */
    ਖਾਲੀ
fileinfo(
    ਪੂਰਨ fullname,	    // when non-zero print full path
    ਪੂਰਨ shorthelp,
    ਪੂਰਨ	dont_truncate)
{
    char_u	*name;
    ਅੱਖਰ	*ਬਫਰ;
    size_t	bufferlen = 0;

    ਬਫਰ = alloc(IOSIZE);
    ਜੇ (ਬਫਰ == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(ਬਫਰ, IOSIZE, "buf %d: ", curbuf->b_fnum);

    ਬਫਰ[bufferlen++] = '"';

    name = buf_spname(curbuf);
    ਜੇ (name != ਨਿਰਾ)
	bufferlen += vim_snprintf_safelen(ਬਫਰ + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!fullname && curbuf->b_fname != ਨਿਰਾ)
	    name = curbuf->b_fname;
	ਨਹੀਂਤਾਂ
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : ਨਿਰਾ, name,
	    (char_u *)ਬਫਰ + bufferlen, IOSIZE - (ਪੂਰਨ)bufferlen, TRUE);
    }

    bufferlen += vim_snprintf_safelen(
	ਬਫਰ + bufferlen,
	IOSIZE - bufferlen,
	"\"%s%s%s%s%s%s",
	curbuf_Is_ਬਦਲਿਆ() ? (shortmess(SHM_MOD)
	    ?  " [+]" : _(" [Modified]")) : " ",
	(curbuf->b_flags & BF_NOTEDITED) && !bt_dontwrite(curbuf)
	    ? _("[Not edited]") : "",
	(curbuf->b_flags & BF_NEW) && !bt_dontwrite(curbuf)
	    ? new_file_ਸੁਨੇਹਾ() : "",
	(curbuf->b_flags & BF_READERR) ? _("[Read errors]") : "", curbuf->b_p_ro
	    ? (shortmess(SHM_RO) ? _("[RO]") : _("[readonly]")) : "",
	(curbuf_Is_ਬਦਲਿਆ() || (curbuf->b_flags & BF_WRITE_ਨਕਾਬ) || curbuf->b_p_ro)
	    ? " " : "");

    ਜੇ (curbuf->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
	bufferlen += vim_snprintf_safelen(ਬਫਰ + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_ਸਤਰਾਂ_ਸੁਨੇਹਾ));
    ਨਹੀਂਤਾਂ ਜੇ (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    ਬਫਰ + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ),
	    (ਲੰਮਾ)curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ));
    ਨਹੀਂਤਾਂ
    {
	bufferlen += vim_snprintf_safelen(
	    ਬਫਰ + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (ਲੰਮਾ)curwin->w_cursor.lnum,
	    (ਲੰਮਾ)curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ));

	validate_virtcol();
	bufferlen += ਖਾਨਾ_print((char_u *)ਬਫਰ + bufferlen, IOSIZE - bufferlen,
		   (ਪੂਰਨ)curwin->w_cursor.ਖਾਨਾ + 1, (ਪੂਰਨ)curwin->w_virtcol + 1);
    }

    (ਖਾਲੀ)append_arg_ਸੰਖਿਆ(curwin, (char_u *)ਬਫਰ + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    ਜੇ (dont_truncate)
    {
	ਪੂਰਨ	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();
	n = ਸੁਨੇਹਾ_scroll;
	ਸੁਨੇਹਾ_scroll = TRUE;
	ਸੁਨੇਹਾ(ਬਫਰ);
	ਸੁਨੇਹਾ_scroll = n;
    }
    ਨਹੀਂਤਾਂ
    {
	ਅੱਖਰ	*p = ਸੁਨੇਹਾ_trunc_attr(ਬਫਰ, FALSE, 0);
	ਜੇ (restart_edit != 0 || (ਸੁਨੇਹਾ_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    ਸੈੱਟ_keep_ਸੁਨੇਹਾ((char_u *)p, 0);
    }

    vim_free(ਬਫਰ);
}

    ਪੂਰਨ
ਖਾਨਾ_print(
    char_u  *buf,
    size_t  buflen,
    ਪੂਰਨ	    ਖਾਨਾ,
    ਪੂਰਨ	    vcol)
{
    ਜੇ (ਖਾਨਾ == vcol)
	ਵਾਪਸ (ਪੂਰਨ)vim_snprintf_safelen((ਅੱਖਰ *)buf, buflen, "%d", ਖਾਨਾ);

    ਵਾਪਸ (ਪੂਰਨ)vim_snprintf_safelen((ਅੱਖਰ *)buf, buflen, "%d-%d", ਖਾਨਾ, vcol);
}

ਸਥਿਰ char_u *lasttitle = ਨਿਰਾ;
ਸਥਿਰ char_u *lasticon = ਨਿਰਾ;

/*
 * Put the file name in the title bar and icon of the window.
 */
    ਖਾਲੀ
maketitle(ਖਾਲੀ)
{
    char_u	*title_str = ਨਿਰਾ;
    char_u	*icon_str = ਨਿਰਾ;
    ਪੂਰਨ		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    ਜੇ (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	ਵਾਪਸ;
    }

    need_maketitle = FALSE;
    ਜੇ (!p_title && !p_icon && lasttitle == ਨਿਰਾ && lasticon == ਨਿਰਾ)
	ਵਾਪਸ;  // nothing to do

    ਜੇ (p_title)
    {
	ਪੂਰਨ maxlen = 0;

	ਜੇ (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    ਜੇ (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	ਜੇ (*p_titlestring != NUL)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
	    ਜੇ (stl_syntax & STL_ਵਿੱਚ_TITLE)
		build_stl_str_hl(curwin, title_str, ਆਕਾਰ(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPACE_FOR_FNAME (ਆਕਾਰ(buf) - 100)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPACE_FOR_DIR   (ਆਕਾਰ(buf) - 20)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPACE_FOR_ARGNR (ਆਕਾਰ(buf) - 10)  // at least room for " - VIM"

	    // file name
	    ਜੇ (curbuf->b_fname == ਨਿਰਾ)
		buflen = vim_snprintf_safelen((ਅੱਖਰ *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    ਨਹੀਂਤਾਂ ਜੇ (curbuf->b_ਪਦ != ਨਿਰਾ)
		buflen = vim_snprintf_safelen((ਅੱਖਰ *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ਪਦ_get_ਸਥਿਤੀ_ਪਾਠ(curbuf->b_ਪਦ));
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ
	    {
		buflen = vim_snprintf_safelen((ਅੱਖਰ *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != ਨਿਰਾ)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    ਜੇ (curbuf->b_ਪਦ == ਨਿਰਾ)
#ਜੇ_ਅੰਤ
	    {
		ਚੋਣ (buf_Is_ਬਦਲਿਆ(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    ਹਾਲਤ 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (ਅੱਖਰ *)buf + buflen,
			    ਆਕਾਰ(buf) - buflen, " +");
			ਰੋਕੋ;
		    ਹਾਲਤ 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (ਅੱਖਰ *)buf + buflen,
			    ਆਕਾਰ(buf) - buflen, " =");
			ਰੋਕੋ;
		    ਹਾਲਤ 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (ਅੱਖਰ *)buf + buflen,
			    ਆਕਾਰ(buf) - buflen, " =+");
			ਰੋਕੋ;
		    ਹਾਲਤ 4:
		    ਹਾਲਤ 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (ਅੱਖਰ *)buf + buflen,
			    ਆਕਾਰ(buf) - buflen, " -");
			ਰੋਕੋ;
		    ਹਾਲਤ 5:
		    ਹਾਲਤ 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (ਅੱਖਰ *)buf + buflen,
			    ਆਕਾਰ(buf) - buflen, " -+");
			ਰੋਕੋ;
		    ਮੂਲ:
			ਰੋਕੋ;
		}
	    }

	    // path (surrounded by '()')
	    ਜੇ (curbuf->b_fname != ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		    && curbuf->b_ਪਦ == ਨਿਰਾ
#ਜੇ_ਅੰਤ
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((ਅੱਖਰ *)buf + buflen,
		    ਆਕਾਰ(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (ਪੂਰਨ)(SPACE_FOR_DIR - buflen), TRUE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ BACKSLASH_ਵਿੱਚ_FILENAME
		// avoid "c:/name" to be reduced to "c"
		ਜੇ (ਸੁਰੱਖਿਅਤ_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#ਜੇ_ਅੰਤ

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		ਜੇ (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((ਅੱਖਰ *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		ਨਹੀਂਤਾਂ
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    ਜੇ (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((ਅੱਖਰ *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != ਨਿਰਾ)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    ਨਹੀਂਤਾਂ
			buflen += vim_snprintf_safelen((ਅੱਖਰ *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_ਸੰਖਿਆ(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((ਅੱਖਰ *)buf + buflen,
		ਆਕਾਰ(buf) - buflen, " - %s",
#ਜੇ defined(FEAT_CLIENTSERVER)
		(serverName != ਨਿਰਾ)
		    ? serverName :
#ਜੇ_ਅੰਤ
		    (char_u *)"VIM");

	    ਜੇ (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		ਜੇ (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, ਆਕਾਰ(buf));
	    }
	}
    }
    mustset = ਮੁੱਲ_ਬਦਲਿਆ(title_str, &lasttitle);

    ਜੇ (p_icon)
    {
	icon_str = buf;
	ਜੇ (*p_iconstring != NUL)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_STL_OPT
	    ਜੇ (stl_syntax & STL_ਵਿੱਚ_ICON)
		build_stl_str_hl(curwin, icon_str, ਆਕਾਰ(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, ਨਿਰਾ, ਨਿਰਾ,
				 ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		icon_str = p_iconstring;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u  *name;
	    ਪੂਰਨ	    namelen;

	    name = buf_spname(curbuf);
	    ਜੇ (name == ਨਿਰਾ)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (ਪੂਰਨ)STRLEN(name);
	    ਜੇ (namelen > 100)
	    {
		namelen -= 100;
		ਜੇ (has_mbyte)
		    namelen += (*mb_ਪੂਛ_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, ਆਕਾਰ(buf));
	}
    }

    mustset |= ਮੁੱਲ_ਬਦਲਿਆ(icon_str, &lasticon);

    ਜੇ (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੁੱਲ_ਬਦਲਿਆ(char_u *str, char_u **ਆਖਰੀ)
{
    ਜੇ ((str == ਨਿਰਾ) != (*ਆਖਰੀ == ਨਿਰਾ)
	    || (str != ਨਿਰਾ && *ਆਖਰੀ != ਨਿਰਾ && STRCMP(str, *ਆਖਰੀ) != 0))
    {
	vim_free(*ਆਖਰੀ);
	ਜੇ (str == ਨਿਰਾ)
	{
	    *ਆਖਰੀ = ਨਿਰਾ;
	    mch_restore_title(
		  ਆਖਰੀ == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	ਨਹੀਂਤਾਂ
	{
	    *ਆਖਰੀ = vim_strsave(str);
	    ਵਾਪਸ TRUE;
	}
    }
    ਵਾਪਸ FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    ਖਾਲੀ
resettitle(ਖਾਲੀ)
{
    mch_settitle(lasttitle, lasticon);
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
free_titles(ਖਾਲੀ)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#ਜੇ_ਅੰਤ


#ਜੇ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u	*stl_ਸ਼ੁਰੂ;
    ਪੂਰਨ		stl_minwid;
    ਪੂਰਨ		stl_maxwid;
    ਗਿਣਤੀ {
	ਸਾਧਾਰਨ,
	ਖਾਲੀ_ਥਾਵਾਂ,
	ਸਮੂਹ_2,
	Separate,
	Highlight,
	Tab_ਪੰਨਾ,
	ClickFunc,
	Trunc
    }		stl_type;
    char_u	*stl_clickfunc;	// function name for ClickFunc items
} stl_ਵਸਤੂ_T;

ਸਥਿਰ size_t		stl_ਚੀਜ਼ਾਂ_len = 20; // Initial value, grows as needed.
ਸਥਿਰ stl_ਵਸਤੂ_T      *stl_ਚੀਜ਼ਾਂ = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	       *stl_groupitem = ਨਿਰਾ;
ਸਥਿਰ stl_hlrec_T     *stl_hltab = ਨਿਰਾ;
ਸਥਿਰ stl_hlrec_T     *stl_tabtab = ਨਿਰਾ;
ਸਥਿਰ stl_clickrec_T  *stl_clicktab = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ		*stl_separator_locations = ਨਿਰਾ;

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
    ਪੂਰਨ
build_stl_str_hl(
    win_T	*wp,
    char_u	*ਬਾਹਰਲਾ,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ਪੂਰਨ		opt_scope,	// scope for "opt_name"
    ਪੂਰਨ		fillchar,
    ਪੂਰਨ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    ਵਾਪਸ build_stl_str_hl_local(STL_ਬਹੁਲਕ_SINGLE, wp, ਬਾਹਰਲਾ, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ਨਿਰਾ, ਨਿਰਾ);
}

    ਪੂਰਨ
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*ਬਾਹਰਲਾ,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    ਪੂਰਨ		opt_scope,	// scope for "opt_name"
    ਪੂਰਨ		fillchar,
    ਪੂਰਨ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ਪੂਰਨ		*ਹਾਸਲ_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ਵਾਪਸ build_stl_str_hl_local(STL_ਬਹੁਲਕ_MULTI, wp, ਬਾਹਰਲਾ, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ਨਿਰਾ, ਹਾਸਲ_hl);
}

# ifdef ENABLE_STL_ਬਹੁਲਕ_MULTI_NL
    ਪੂਰਨ
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*ਬਾਹਰਲਾ,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    ਪੂਰਨ		opt_scope,	// scope for "opt_name"
    ਪੂਰਨ		fillchar,
    ਪੂਰਨ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ਪੂਰਨ		*ਹਾਸਲ_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ਵਾਪਸ build_stl_str_hl_local(STL_ਬਹੁਲਕ_MULTI_NL, wp, ਬਾਹਰਲਾ, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ਨਿਰਾ, ਹਾਸਲ_hl);
}
# endif

    ਪੂਰਨ
get_stl_rendered_ਉਚਾਈ(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ਪੂਰਨ		opt_scope)	// scope for "opt_name"
{
    ਪੂਰਨ rendered_ਉਚਾਈ = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (ਖਾਲੀ)build_stl_str_hl_local(STL_ਬਹੁਲਕ_GET_RENDERED_ਉਚਾਈ,
	    wp, buf, ਆਕਾਰ(buf), &fmt,
	    opt_name, opt_scope, 0, 0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, &rendered_ਉਚਾਈ,
	    ਨਿਰਾ);
    --emsg_off;
    ਵਾਪਸ rendered_ਉਚਾਈ;
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
ਸਥਿਰ ਪੂਰਨ
build_stl_str_hl_local(
    stl_ਬਹੁਲਕ_T	ਬਹੁਲਕ,
    win_T	*wp,
    char_u	*ਬਾਹਰਲਾ,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ਪੂਰਨ		opt_scope,	// scope for "opt_name"
    ਪੂਰਨ		fillchar,
    ਪੂਰਨ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ਪੂਰਨ		*rendered_ਉਚਾਈ,   // return: stl rendered height (can be NULL)
    ਪੂਰਨ		*ਹਾਸਲ_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    ਪੂਰਨ		byteval;
# ifdef FEAT_EVAL
    ਪੂਰਨ		use_sandbox;
    ਪੂਰਨ		save_VIsual_ਸਰਗਰਮ;
# endif
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ;
    ਲੰਮਾ	l;
    ਲੰਮਾ	n;
    ਪੂਰਨ		prevchar_isflag;
    ਪੂਰਨ		prevchar_isitem;
    ਪੂਰਨ		itemisflag;
    ਪੂਰਨ		fillable;
    char_u	*str;
    ਲੰਮਾ	ਅੰਕ;
    ਪੂਰਨ		ਚੌੜਾਈ;
    ਪੂਰਨ		itemcnt;
    ਪੂਰਨ		curitem;
    ਪੂਰਨ		groupdepth;
# ifdef FEAT_EVAL
    ਪੂਰਨ		evaldepth;
# endif
    ਪੂਰਨ		minwid;
    ਪੂਰਨ		maxwid;
    ਪੂਰਨ		zeropad;
    char_u	ਅਧਾਰ;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    ਪੂਰਨ		save_redraw_not_allowed = redraw_not_allowed;
    ਪੂਰਨ		save_ਕੁੰਜੀ_Typed = ਕੁੰਜੀ_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    ਪੂਰਨ		did_emsg_before = did_emsg;
    ਪੂਰਨ		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    ਜੇ (updating_screen)
	redraw_not_allowed = TRUE;

    ਜੇ (stl_ਚੀਜ਼ਾਂ == ਨਿਰਾ)
    {
	stl_ਚੀਜ਼ਾਂ = ALLOC_MULT(stl_ਵਸਤੂ_T, stl_ਚੀਜ਼ਾਂ_len);
	stl_groupitem = ALLOC_MULT(ਪੂਰਨ, stl_ਚੀਜ਼ਾਂ_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_ਚੀਜ਼ਾਂ_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_ਚੀਜ਼ਾਂ_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_ਚੀਜ਼ਾਂ_len + 1);

	stl_separator_locations = ALLOC_MULT(ਪੂਰਨ, stl_ਚੀਜ਼ਾਂ_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_ਸੈੱਟ_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    ਜੇ (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_ਸੰਖਿਆ;
	tv.vval.v_ਸੰਖਿਆ = wp->w_id;
	ਸੈੱਟ_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_ਮੰਜ਼ਿਲ_string_ਸੁਰੱਖਿਅਤ(fmt + 2, use_sandbox, FALSE, FALSE);
	ਜੇ (usefmt == ਨਿਰਾ)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    ਜੇ (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    ਜੇ (lnum > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
    {
	lnum = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_ਬਫਰ, lnum, FALSE);
    ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_ਬਫਰ, lnum);
    ਜੇ (wp->w_cursor.ਖਾਨਾ > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.ਖਾਨਾ = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    ਨਹੀਂਤਾਂ
	byteval = (*mb_ptr2char)(p + wp->w_cursor.ਖਾਨਾ);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = ਬਾਹਰਲਾ;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    ਜੇ (ਹਾਸਲ_hl != ਨਿਰਾ && *ਹਾਸਲ_hl != 0)
    {
	stl_ਚੀਜ਼ਾਂ[curitem].stl_type = Highlight;
	stl_ਚੀਜ਼ਾਂ[curitem].stl_ਸ਼ੁਰੂ = p;
	stl_ਚੀਜ਼ਾਂ[curitem].stl_minwid = *ਹਾਸਲ_hl;
	stl_ਚੀਜ਼ਾਂ[curitem].stl_clickfunc = ਨਿਰਾ;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    ਲਈ (s = usefmt; *s != NUL; )
    {
	ਜੇ (curitem == (ਪੂਰਨ)stl_ਚੀਜ਼ਾਂ_len)
	{
	    size_t	new_len = stl_ਚੀਜ਼ਾਂ_len * 3 / 2;

	    stl_ਵਸਤੂ_T *new_ਚੀਜ਼ਾਂ =
			  vim_realloc(stl_ਚੀਜ਼ਾਂ, ਆਕਾਰ(stl_ਵਸਤੂ_T) * new_len);
	    ਜੇ (new_ਚੀਜ਼ਾਂ == ਨਿਰਾ)
		ਰੋਕੋ;
	    stl_ਚੀਜ਼ਾਂ = new_ਚੀਜ਼ਾਂ;

	    ਪੂਰਨ *new_groupitem =
			     vim_realloc(stl_groupitem, ਆਕਾਰ(ਪੂਰਨ) * new_len);
	    ਜੇ (new_groupitem == ਨਿਰਾ)
		ਰੋਕੋ;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  ਆਕਾਰ(stl_hlrec_T) * (new_len + 1));
	    ਜੇ (new_hlrec == ਨਿਰਾ)
		ਰੋਕੋ;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  ਆਕਾਰ(stl_hlrec_T) * (new_len + 1));
	    ਜੇ (new_hlrec == ਨਿਰਾ)
		ਰੋਕੋ;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      ਆਕਾਰ(stl_clickrec_T) * (new_len + 1));
	    ਜੇ (new_clickrec == ਨਿਰਾ)
		ਰੋਕੋ;
	    stl_clicktab = new_clickrec;

	    ਪੂਰਨ *new_separator_locs = vim_realloc(stl_separator_locations,
					    ਆਕਾਰ(ਪੂਰਨ) * new_len);
	    ਜੇ (new_separator_locs == ਨਿਰਾ)
		ਰੋਕੋ;
	    stl_separator_locations = new_separator_locs;

	    stl_ਚੀਜ਼ਾਂ_len = new_len;
	}

	stl_ਚੀਜ਼ਾਂ[curitem].stl_clickfunc = ਨਿਰਾ;

	ਜੇ (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	ਜਦੋਂ (*s != NUL && *s != '%' && p + 1 < ਬਾਹਰਲਾ + outlen)
# ifdef ENABLE_STL_ਬਹੁਲਕ_MULTI_NL
	{
	    ਜੇ (*s == '\n' || *s == '\r')
	    {
		ਜੇ (ਬਹੁਲਕ == STL_ਬਹੁਲਕ_MULTI_NL)
		{
		    s++;
		    ਜਾਓ ਲੱਭੋ_linebreak;
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ == STL_ਬਹੁਲਕ_GET_RENDERED_ਉਚਾਈ)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_ਬਹੁਲਕ_MULTI_NL
	}
# endif
	ਜੇ (*s == NUL || p + 1 >= ਬਾਹਰਲਾ + outlen)
	    ਰੋਕੋ;

	/*
	 * Handle one '%' item.
	 */
	s++;
	ਜੇ (*s == NUL)  // ignore trailing %
	    ਰੋਕੋ;

	ਜੇ (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    ਜੇ (s[1] == ']')
	    {
		stl_ਚੀਜ਼ਾਂ[curitem].stl_type = ClickFunc;
		stl_ਚੀਜ਼ਾਂ[curitem].stl_ਸ਼ੁਰੂ = p;
		stl_ਚੀਜ਼ਾਂ[curitem].stl_minwid = 0;
		stl_ਚੀਜ਼ਾਂ[curitem].stl_clickfunc = ਨਿਰਾ;
		s += 2;
		curitem++;
		ਜਾਰੀ;
	    }
	    // %[FuncName] - start click region
	    ਜੇ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		ਜੇ (rb != ਨਿਰਾ)
		{
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_type = ClickFunc;
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_ਸ਼ੁਰੂ = p;
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_minwid = 0;
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    ਜਾਰੀ;
		}
	    }
	}
	ਜੇ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    ਜੇ (ਬਹੁਲਕ == STL_ਬਹੁਲਕ_MULTI
# ifdef ENABLE_STL_ਬਹੁਲਕ_MULTI_NL
		    || ਬਹੁਲਕ == STL_ਬਹੁਲਕ_MULTI_NL
# endif
	       )
	    {
		s++;
		ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਬਹੁਲਕ == STL_ਬਹੁਲਕ_GET_RENDERED_ਉਚਾਈ)
		rheight++;
	}
	ਜੇ (*s == '%')
	{
	    ਜੇ (p + 1 >= ਬਾਹਰਲਾ + outlen)
		ਰੋਕੋ;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    ਜਾਰੀ;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	ਜੇ (*s == STL_SEPARATE)
	{
	    s++;
	    ਜੇ (groupdepth > 0)
		ਜਾਰੀ;
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_type = Separate;
	    stl_ਚੀਜ਼ਾਂ[curitem++].stl_ਸ਼ੁਰੂ = p;
	    ਜਾਰੀ;
	}
	ਜੇ (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_type = Trunc;
	    stl_ਚੀਜ਼ਾਂ[curitem++].stl_ਸ਼ੁਰੂ = p;
	    ਜਾਰੀ;
	}
	ਜੇ (*s == ')')
	{
	    char_u  *t;

	    s++;
	    ਜੇ (groupdepth < 1)
		ਜਾਰੀ;
	    groupdepth--;

	    t = stl_ਚੀਜ਼ਾਂ[stl_groupitem[groupdepth]].stl_ਸ਼ੁਰੂ;
	    *p = NUL;
	    l = vim_strsize(t);
	    ਜੇ (curitem > stl_groupitem[groupdepth] + 1
		    && stl_ਚੀਜ਼ਾਂ[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		ਪੂਰਨ ਸਮੂਹ_ਸ਼ੁਰੂ_userhl = 0;
		ਪੂਰਨ ਸਮੂਹ_ਸਿਰਾ_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		ਲਈ (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    ਜੇ (stl_ਚੀਜ਼ਾਂ[n].stl_type == Highlight)
		    {
			ਸਮੂਹ_ਸ਼ੁਰੂ_userhl = ਸਮੂਹ_ਸਿਰਾ_userhl =
						       stl_ਚੀਜ਼ਾਂ[n].stl_minwid;
			ਰੋਕੋ;
		    }
		}
		ਲਈ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    ਜੇ (stl_ਚੀਜ਼ਾਂ[n].stl_type == ਸਾਧਾਰਨ)
			ਰੋਕੋ;
		    ਜੇ (stl_ਚੀਜ਼ਾਂ[n].stl_type == Highlight)
			ਸਮੂਹ_ਸਿਰਾ_userhl = stl_ਚੀਜ਼ਾਂ[n].stl_minwid;
		}
		ਜੇ (n == curitem && ਸਮੂਹ_ਸ਼ੁਰੂ_userhl == ਸਮੂਹ_ਸਿਰਾ_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    ਲਈ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			ਜੇ (stl_ਚੀਜ਼ਾਂ[n].stl_type == Highlight)
			    stl_ਚੀਜ਼ਾਂ[n].stl_type = ਖਾਲੀ_ਥਾਵਾਂ;
			// adjust the start position of TabPage to the next
			// item position
			ਜੇ (stl_ਚੀਜ਼ਾਂ[n].stl_type == Tab_ਪੰਨਾ)
			    stl_ਚੀਜ਼ਾਂ[n].stl_ਸ਼ੁਰੂ = p;
		    }
		}
	    }
	    ਜੇ (l > stl_ਚੀਜ਼ਾਂ[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		ਜੇ (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    ਜਦੋਂ (l >= stl_ਚੀਜ਼ਾਂ[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		ਨਹੀਂਤਾਂ
		    n = (ਲੰਮਾ)(p - t) - stl_ਚੀਜ਼ਾਂ[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		ਪੂਰਨ minwid_fixed = MIN(stl_ਚੀਜ਼ਾਂ[stl_groupitem[groupdepth]].stl_minwid,
				       stl_ਚੀਜ਼ਾਂ[stl_groupitem[groupdepth]].stl_maxwid);
		ਜਦੋਂ (++l < minwid_fixed)
		    MB_CHAR2_ਬਾਈਟਾਂ(fillchar, p);

		// correct the start of the items for the truncation
		ਲਈ (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ -= n - 1;
		    ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ < t)
			stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ = t;
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਨਿਰਪੇਖ(stl_ਚੀਜ਼ਾਂ[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_ਚੀਜ਼ਾਂ[stl_groupitem[groupdepth]].stl_minwid;
		ਪੂਰਨ fillchar_len = MB_CHAR2LEN(fillchar);
		ਜੇ (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    ਜਦੋਂ (l++ < n && p + fillchar_len < ਬਾਹਰਲਾ + outlen)
			MB_CHAR2_ਬਾਈਟਾਂ(fillchar, p);
		}
		ਨਹੀਂਤਾਂ
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    ਜੇ (p + l >= ਬਾਹਰਲਾ + outlen)
		    {
			n = (ਲੰਮਾ)((ਬਾਹਰਲਾ + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    ਲਈ ( ; n > 0; n--)
			MB_CHAR2_ਬਾਈਟਾਂ(fillchar, t);
		    ਲਈ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_ਚੀਜ਼ਾਂ[n].stl_ਸ਼ੁਰੂ += l;
		}
	    }
	    ਜਾਰੀ;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	ਜੇ (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	ਜੇ (*s == '-')
	{
	    s++;
	    l = -1;
	}
	ਜੇ (VIM_ISDIGIT(*s))
	{
	    minwid = (ਪੂਰਨ)getdigits(&s);
	    ਜੇ (minwid < 0)	//ਉਛਾਲ
		minwid = 0;
	}
	ਜੇ (*s == STL_USER_HL)
	{
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_type = Highlight;
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_ਸ਼ੁਰੂ = p;
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    ਜਾਰੀ;
	}
	ਜੇ (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    ਜੇ (*s == STL_TABCLOSENR)
	    {
		ਜੇ (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    ਲਈ (n = curitem - 1; n >= 0; --n)
			ਜੇ (stl_ਚੀਜ਼ਾਂ[n].stl_type == Tab_ਪੰਨਾ
					       && stl_ਚੀਜ਼ਾਂ[n].stl_minwid >= 0)
			{
			    minwid = stl_ਚੀਜ਼ਾਂ[n].stl_minwid;
			    ਰੋਕੋ;
			}
		}
		ਨਹੀਂਤਾਂ
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_type = Tab_ਪੰਨਾ;
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_ਸ਼ੁਰੂ = p;
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    ਜਾਰੀ;
	}
	ਜੇ (*s == '.')
	{
	    s++;
	    ਜੇ (VIM_ISDIGIT(*s))
	    {
		maxwid = (ਪੂਰਨ)getdigits(&s);
		ਜੇ (maxwid <= 0)	//ਉਛਾਲ
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	ਪੂਰਨ raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	ਜੇ (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_type = ਸਮੂਹ_2;
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_ਸ਼ੁਰੂ = p;
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_minwid = minwid;
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    ਜਾਰੀ;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	ਜੇ (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    ਜਾਰੀ;
	}
# endif
	ਜੇ (vim_strchr(STL_ALL, *s) == ਨਿਰਾ)
	{
	    ਜੇ (*s == NUL)  // can happen with "%0"
		ਰੋਕੋ;
	    s++;
	    ਜਾਰੀ;
	}
	opt = *s++;

	// OK - now for the real work
	ਅਧਾਰ = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	ਅੰਕ = -1;
	str = ਨਿਰਾ;
	ਚੋਣ (opt)
	{
	ਹਾਲਤ STL_FILEPATH:
	ਹਾਲਤ STL_FULLPATH:
	ਹਾਲਤ STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_ਬਫਰ);
	    ਜੇ (name != ਨਿਰਾ)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    ਨਹੀਂਤਾਂ
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_ਬਫਰ->b_ffname
					  : wp->w_ਬਫਰ->b_fname;
		home_replace(wp->w_ਬਫਰ, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    ਜੇ (opt != STL_FILENAME)
		str = NameBuff;
	    ਨਹੀਂਤਾਂ
		str = gettail(NameBuff);
	    ਰੋਕੋ;
	}

	ਹਾਲਤ STL_VIM_ਵਿਅੰਜਕ: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *ਖੰਡ_ਸ਼ੁਰੂ = s - 1;
# endif
	    ਪੂਰਨ	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    ਜੇ (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    ਜਦੋਂ ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < ਬਾਹਰਲਾ + outlen)
		*p++ = *s++;
	    ਜੇ (*s != '}')	// missing '}' or out of space
		ਰੋਕੋ;
	    s++;
	    ਜੇ (reevaluate && p > ਬਾਹਰਲਾ)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    ਨਹੀਂਤਾਂ
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((ਅੱਖਰ *)buf_tmp, ਆਕਾਰ(buf_tmp),
							 "%d", curbuf->b_fnum);
	    ਸੈੱਟ_ਅੰਦਰੂਨੀ_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((ਅੱਖਰ *)buf_tmp, ਆਕਾਰ(buf_tmp), "%d", curwin->w_id);
	    ਸੈੱਟ_ਅੰਦਰੂਨੀ_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_ਸਰਗਰਮ = VIsual_ਸਰਗਰਮ;
	    curwin = wp;
	    curbuf = wp->w_ਬਫਰ;
	    // Visual mode is only valid in the current window.
	    ਜੇ (curwin != save_curwin)
		VIsual_ਸਰਗਰਮ = FALSE;

	    str = eval_ਮੰਜ਼ਿਲ_string_ਸੁਰੱਖਿਅਤ(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_ਸਰਗਰਮ = save_VIsual_ਸਰਗਰਮ;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    ਜੇ (!zeropad && str != ਨਿਰਾ && *str != NUL)
	    {
		ਜੇ (*skipdigits(str) == NUL)
		{
		    ਅੰਕ = atoi((ਅੱਖਰ *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    ਜੇ (reevaluate && str != ਨਿਰਾ && *str != NUL
		    && strchr((ਅਚਲ ਅੱਖਰ *)str, '%') != ਨਿਰਾ
		    && evaldepth < MAX_STL_EVAL_ਡੂੰਘਾਈ)
	    {
		size_t parsed_usefmt = (size_t)(ਖੰਡ_ਸ਼ੁਰੂ - usefmt);
		size_t str_ਲੰਮਾਈ = ਲੜੀ_ਲੰਬਾਈ((ਅਚਲ ਅੱਖਰ *)str);
		size_t fmt_ਲੰਮਾਈ = ਲੜੀ_ਲੰਬਾਈ((ਅਚਲ ਅੱਖਰ *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_ਲੰਮਾਈ + fmt_ਲੰਮਾਈ + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * ਆਕਾਰ(char_u));

		ਜੇ (new_fmt != ਨਿਰਾ)
		{
		    char_u *new_fmt_p = new_fmt;

		    new_fmt_p = (char_u *)ਸਮ੍ਰਿਤੀ_ਨਕਲ(new_fmt_p, usefmt, parsed_usefmt)
								   + parsed_usefmt;
		    new_fmt_p = (char_u *)ਸਮ੍ਰਿਤੀ_ਨਕਲ(new_fmt_p , str, str_ਲੰਮਾਈ)
								      + str_ਲੰਮਾਈ;
		    new_fmt_p = (char_u *)ਸਮ੍ਰਿਤੀ_ਨਕਲ(new_fmt_p, "%}", 2) + 2;
		    new_fmt_p = (char_u *)ਸਮ੍ਰਿਤੀ_ਨਕਲ(new_fmt_p , s, fmt_ਲੰਮਾਈ)
								      + fmt_ਲੰਮਾਈ;
		    *new_fmt_p = 0;
		    new_fmt_p = ਨਿਰਾ;

		    ਜੇ (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    ਜਾਰੀ;
		}
	    }
# endif
	    ਰੋਕੋ;
	}
	ਹਾਲਤ STL_ਸਤਰ:
	    ਅੰਕ = (wp->w_ਬਫਰ->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ)
		  ? 0L : (ਲੰਮਾ)(wp->w_cursor.lnum);
	    ਰੋਕੋ;

	ਹਾਲਤ STL_NUMLINES:
	    ਅੰਕ = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    ਰੋਕੋ;

	ਹਾਲਤ STL_ਕਾਲਮ:
	    ਅੰਕ = (State & ਬਹੁਲਕ_ਪਾਓ) == 0 && ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ
					       ? 0 : (ਪੂਰਨ)wp->w_cursor.ਖਾਨਾ + 1;
	    ਰੋਕੋ;

	ਹਾਲਤ STL_VIRTCOL:
	ਹਾਲਤ STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    ਜੇ (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & ਬਹੁਲਕ_ਪਾਓ) == 0
			       && ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ ? 0 : (ਪੂਰਨ)wp->w_cursor.ਖਾਨਾ + 1)))
		ਰੋਕੋ;
	    ਅੰਕ = (ਲੰਮਾ)virtcol;
	    ਰੋਕੋ;
	}

	ਹਾਲਤ STL_PERCENTAGE:
	    ਅੰਕ = calc_percentage((ਲੰਮਾ)wp->w_cursor.lnum, (ਲੰਮਾ)wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
	    ਰੋਕੋ;

	ਹਾਲਤ STL_ALTPERCENT:
	    str = buf_tmp;
	    (ਖਾਲੀ)get_rel_pos(wp, str, TMPLEN);
	    ਰੋਕੋ;

	ਹਾਲਤ STL_SHOWCMD:
	    ਜੇ (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    ਰੋਕੋ;

	ਹਾਲਤ STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    ਜੇ (append_arg_ਸੰਖਿਆ(wp, buf_tmp, ਆਕਾਰ(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    ਰੋਕੋ;

	ਹਾਲਤ STL_KEYMAP:
	    fillable = FALSE;
	    ਜੇ (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    ਰੋਕੋ;
	ਹਾਲਤ STL_PAGENUM:
# ਜੇ defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    ਅੰਕ = printer_ਪੰਨਾ_ਅੰਕ;
# ਨਹੀਂਤਾਂ
	    ਅੰਕ = 0;
# endif
	    ਰੋਕੋ;

	ਹਾਲਤ STL_BUFNO:
	    ਅੰਕ = wp->w_ਬਫਰ->b_fnum;
	    ਰੋਕੋ;

	ਹਾਲਤ STL_ਵਿੱਥ_X:
	    ਅਧਾਰ = 'X';
	    // FALLTHROUGH
	ਹਾਲਤ STL_ਵਿੱਥ:
# ifdef FEAT_BYTEOFF
	    l = ml_ਲੱਭੋ_ਸਤਰ_or_ਵਿੱਥ(wp->w_ਬਫਰ, wp->w_cursor.lnum, ਨਿਰਾ);
	    ਅੰਕ = (wp->w_ਬਫਰ->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ) || l < 0
		       ? 0L : l + 1 + ((State & ਬਹੁਲਕ_ਪਾਓ) == 0 && ਖਾਲੀ_ਥਾਵਾਂ_ਸਤਰ
				? 0 : (ਪੂਰਨ)wp->w_cursor.ਖਾਨਾ);
# endif
	    ਰੋਕੋ;

	ਹਾਲਤ STL_BYTEVAL_X:
	    ਅਧਾਰ = 'X';
	    // FALLTHROUGH
	ਹਾਲਤ STL_BYTEVAL:
	    ਅੰਕ = byteval;
	    ਜੇ (ਅੰਕ == NL)
		ਅੰਕ = 0;
	    ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕ == CAR && get_fileformat(wp->w_ਬਫਰ) == EOL_MAC)
		ਅੰਕ = NL;
	    ਰੋਕੋ;

	ਹਾਲਤ STL_ROFLAG:
	ਹਾਲਤ STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    ਜੇ (wp->w_ਬਫਰ->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    ਰੋਕੋ;

	ਹਾਲਤ STL_HELPFLAG:
	ਹਾਲਤ STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    ਜੇ (wp->w_ਬਫਰ->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    ਰੋਕੋ;

	ਹਾਲਤ STL_FILETYPE:
	    ਜੇ (*wp->w_ਬਫਰ->b_p_ft != NUL
		    && STRLEN(wp->w_ਬਫਰ->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((ਅੱਖਰ *)buf_tmp, ਆਕਾਰ(buf_tmp), "[%s]",
							wp->w_ਬਫਰ->b_p_ft);
		str = buf_tmp;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    ਜੇ (*wp->w_ਬਫਰ->b_p_ft != NUL
		    && STRLEN(wp->w_ਬਫਰ->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((ਅੱਖਰ *)buf_tmp, ਆਕਾਰ(buf_tmp), ",%s",
							wp->w_ਬਫਰ->b_p_ft);
		ਲਈ (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    ਰੋਕੋ;

# ਜੇ defined(FEAT_QUICKFIX)
	ਹਾਲਤ STL_PREVIEWFLAG:
	ਹਾਲਤ STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    ਜੇ (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    ਰੋਕੋ;

	ਹਾਲਤ STL_QUICKFIX:
	    ਜੇ (bt_quickfix(wp->w_ਬਫਰ))
		str = (char_u *)(wp->w_llist_ref
			    ? _(ਸੁਨੇਹਾ_loclist)
			    : _(ਸੁਨੇਹਾ_qflist));
	    ਰੋਕੋ;
# endif

	ਹਾਲਤ STL_MODIFIED:
	ਹਾਲਤ STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    ਚੋਣ ((opt == STL_MODIFIED_ALT)
		    + buf_Is_ਬਦਲਿਆ(wp->w_ਬਫਰ) * 2
		    + (!wp->w_ਬਫਰ->b_p_ma) * 4)
	    {
		ਹਾਲਤ 2: str = (char_u *)"[+]"; ਰੋਕੋ;
		ਹਾਲਤ 3: str = (char_u *)",+"; ਰੋਕੋ;
		ਹਾਲਤ 4: str = (char_u *)"[-]"; ਰੋਕੋ;
		ਹਾਲਤ 5: str = (char_u *)",-"; ਰੋਕੋ;
		ਹਾਲਤ 6: str = (char_u *)"[+-]"; ਰੋਕੋ;
		ਹਾਲਤ 7: str = (char_u *)",+-"; ਰੋਕੋ;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		ਜਦੋਂ (*s != '#' && *s != NUL)
		    ++s;
		ਜੇ (*s == '#')
		{
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_type = Highlight;
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_ਸ਼ੁਰੂ = p;
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_minwid = -syn_namen2id(t, (ਪੂਰਨ)(s - t));
		    curitem++;
		}
		ਜੇ (*s != NUL)
		    ++s;
		ਜਾਰੀ;
	    }

	ਹਾਲਤ STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    ਜੇ (*s == ']')
	    {
		stl_ਚੀਜ਼ਾਂ[curitem].stl_type = ClickFunc;
		stl_ਚੀਜ਼ਾਂ[curitem].stl_ਸ਼ੁਰੂ = p;
		stl_ਚੀਜ਼ਾਂ[curitem].stl_minwid = 0;
		stl_ਚੀਜ਼ਾਂ[curitem].stl_clickfunc = ਨਿਰਾ;
		s++;
		curitem++;
		ਜਾਰੀ;
	    }
	    // %N[FuncName] with minwid
	    ਜੇ (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		ਜੇ (rb != ਨਿਰਾ)
		{
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_type = ClickFunc;
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_ਸ਼ੁਰੂ = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_minwid = raw_minwid;
		    stl_ਚੀਜ਼ਾਂ[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    ਜਾਰੀ;
		}
	    }
	    ਜਾਰੀ;

	ਹਾਲਤ STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    ਜਾਰੀ;
	}

	stl_ਚੀਜ਼ਾਂ[curitem].stl_ਸ਼ੁਰੂ = p;
	stl_ਚੀਜ਼ਾਂ[curitem].stl_type = ਸਾਧਾਰਨ;
	ਜੇ (str != ਨਿਰਾ && *str)
	{
	    char_u  *t = str;

	    ਜੇ (itemisflag)
	    {
		ਜੇ ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    ਜੇ (l > 0)
		prevchar_isitem = TRUE;
	    ਜੇ (l > maxwid)
	    {
		ਜਦੋਂ (l >= maxwid)
		    ਜੇ (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    ਨਹੀਂਤਾਂ
			l -= byte2cells(*t++);
		ਜੇ (p + 1 >= ਬਾਹਰਲਾ + outlen)
		    ਰੋਕੋ;
		*p++ = '<';
	    }
	    ਜੇ (minwid > 0)
	    {
		ਲਈ (; l < minwid && p + 1 < ਬਾਹਰਲਾ + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    ਜੇ (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    ਨਹੀਂਤਾਂ
			MB_CHAR2_ਬਾਈਟਾਂ(fillchar, p);
		}
		minwid = 0;
	    }
	    ਨਹੀਂਤਾਂ
		minwid *= -1;
	    ਲਈ (; *t && p + 1 < ਬਾਹਰਲਾ + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		ਜੇ (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2_ਬਾਈਟਾਂ(fillchar, p);
		ਨਹੀਂਤਾਂ
		    *p++ = *t;
	    }
	    ਲਈ (; l < minwid && p + 1 < ਬਾਹਰਲਾ + outlen; l++)
		MB_CHAR2_ਬਾਈਟਾਂ(fillchar, p);
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਅੰਕ >= 0)
	{
	    ਪੂਰਨ	    nbase = (ਅਧਾਰ == 'D' ? 10 : (ਅਧਾਰ == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    ਜੇ (p + 20 >= ਬਾਹਰਲਾ + outlen)
		ਰੋਕੋ;		// not sufficient space
	    prevchar_isitem = TRUE;
	    ਜੇ (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    ਜੇ (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? ਅਧਾਰ : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    ਲਈ (n = ਅੰਕ, l = 1; n >= nbase; n /= nbase)
		l++;
	    ਜੇ (opt == STL_VIRTCOL_ALT)
		l++;

	    ਜੇ (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		ਜਦੋਂ (l-- > maxwid)
		    ਅੰਕ /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((ਅੱਖਰ *)p, outlen - (p - ਬਾਹਰਲਾ),
		    (ਅੱਖਰ *)nstr, 0, ਅੰਕ, n);
	    }
	    ਨਹੀਂਤਾਂ
		p += vim_snprintf_safelen((ਅੱਖਰ *)p, outlen - (p - ਬਾਹਰਲਾ),
		    (ਅੱਖਰ *)nstr, minwid, ਅੰਕ);
	}
	ਨਹੀਂਤਾਂ
	    stl_ਚੀਜ਼ਾਂ[curitem].stl_type = ਖਾਲੀ_ਥਾਵਾਂ;

	ਜੇ (ਅੰਕ >= 0 || (!itemisflag && str != ਨਿਰਾ && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	ਜੇ (opt == STL_VIM_ਵਿਅੰਜਕ)
	    vim_free(str);
	curitem++;
    }
# ifdef ENABLE_STL_ਬਹੁਲਕ_MULTI_NL
ਲੱਭੋ_linebreak:
# endif
    *p = NUL;
    outputlen = (size_t)(p - ਬਾਹਰਲਾ);
    itemcnt = curitem;

    // Remember the most recent %# / %* highlight so the next row of a
    // multi-line statusline can resume it.
    ਜੇ (ਹਾਸਲ_hl != ਨਿਰਾ)
    {
	ਪੂਰਨ ਆਖਰੀ_hl = 0;
	ਲਈ (l = 0; l < itemcnt; l++)
	    ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_type == Highlight)
		ਆਖਰੀ_hl = stl_ਚੀਜ਼ਾਂ[l].stl_minwid;
	*ਹਾਸਲ_hl = ਆਖਰੀ_hl;
    }

    ਜੇ (ਬਹੁਲਕ == STL_ਬਹੁਲਕ_MULTI
# ifdef ENABLE_STL_ਬਹੁਲਕ_MULTI_NL
		    || ਬਹੁਲਕ == STL_ਬਹੁਲਕ_MULTI_NL
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
	ਜੇ (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = ਲੜੀ_ਲੰਬਾਈ((ਅੱਖਰ *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    ਨਹੀਂਤਾਂ
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	ਜੇ (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    ਜੇ (ਬਹੁਲਕ == STL_ਬਹੁਲਕ_GET_RENDERED_ਉਚਾਈ)
    {
	// Free click function names that were allocated during parsing.
	ਲਈ (l = 0; l < itemcnt; l++)
	    ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_type == ClickFunc)
		vim_free(stl_ਚੀਜ਼ਾਂ[l].stl_clickfunc);
	ਜੇ (rendered_ਉਚਾਈ != ਨਿਰਾ)
	    *rendered_ਉਚਾਈ = rheight;
	ਵਾਪਸ 0;
    }

    ਚੌੜਾਈ = vim_strsize(ਬਾਹਰਲਾ);
    ਜੇ (maxwidth > 0 && ਚੌੜਾਈ > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	ਜੇ (itemcnt == 0)
	    s = ਬਾਹਰਲਾ;
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ ( ; l < itemcnt; l++)
		ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ;
		    ਰੋਕੋ;
		}
	    ਜੇ (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_ਚੀਜ਼ਾਂ[0].stl_ਸ਼ੁਰੂ;
		l = 0;
	    }
	}

	ਜੇ (ਚੌੜਾਈ - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    ਜੇ (has_mbyte)
	    {
		s = ਬਾਹਰਲਾ;
		ਚੌੜਾਈ = 0;
		ਲਈ (;;)
		{
		    ਚੌੜਾਈ += ptr2cells(s);
		    ਜੇ (ਚੌੜਾਈ >= maxwidth)
			ਰੋਕੋ;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		ਜਦੋਂ (++ਚੌੜਾਈ < maxwidth)
		    MB_CHAR2_ਬਾਈਟਾਂ(fillchar, s);
	    }
	    ਨਹੀਂਤਾਂ
		s = ਬਾਹਰਲਾ + maxwidth - 1;
	    ਲਈ (l = 0; l < itemcnt; l++)
		ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ > s)
		    ਰੋਕੋ;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	ਨਹੀਂਤਾਂ
	{
	    char_u  *ਸਿਰਾ = ਬਾਹਰਲਾ + outputlen;

	    n = 0;
	    ਜਦੋਂ (ਚੌੜਾਈ >= maxwidth)
	    {
		ਚੌੜਾਈ -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(ਸਿਰਾ - p) + 1);	// +1 for NUL
	    ਸਿਰਾ -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    ਲਈ (; l < itemcnt; l++)
	    {
		ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ - n >= s)
		    stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ -= n;
		ਨਹੀਂਤਾਂ
		    stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ = s;
	    }

	    // Fill up for half a double-wide character.
	    ਜਦੋਂ (++ਚੌੜਾਈ < maxwidth)
	    {
		s = ਸਿਰਾ;
		MB_CHAR2_ਬਾਈਟਾਂ(fillchar, s);
		*s = NUL;
		ਸਿਰਾ = s;
	    }
	}
	ਚੌੜਾਈ = maxwidth;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਚੌੜਾਈ < maxwidth &&
	    outputlen + (maxwidth - ਚੌੜਾਈ) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	ਪੂਰਨ ਅੰਕ_separators = 0;

	ਲਈ (l = 0; l < itemcnt; l++)
	{
	    ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[ਅੰਕ_separators] = l;
		ਅੰਕ_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	ਜੇ (ਅੰਕ_separators)
	{
	    ਪੂਰਨ standard_spaces;
	    ਪੂਰਨ final_spaces;

	    standard_spaces = (maxwidth - ਚੌੜਾਈ) / ਅੰਕ_separators;
	    final_spaces = (maxwidth - ਚੌੜਾਈ) -
					standard_spaces * (ਅੰਕ_separators - 1);
	    ਲਈ (l = 0; l < ਅੰਕ_separators; l++)
	    {
		ਪੂਰਨ dislocation = (l == (ਅੰਕ_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *ਸ਼ੁਰੂ = stl_ਚੀਜ਼ਾਂ[stl_separator_locations[l]].stl_ਸ਼ੁਰੂ;
		char_u *seploc = ਸ਼ੁਰੂ + dislocation;
		STRMOVE(seploc, ਸ਼ੁਰੂ);
		ਲਈ (s = ਸ਼ੁਰੂ; s < seploc;)
		    MB_CHAR2_ਬਾਈਟਾਂ(fillchar, s);

		ਲਈ (ਪੂਰਨ i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_ਚੀਜ਼ਾਂ[i].stl_ਸ਼ੁਰੂ += dislocation;
	    }

	    ਚੌੜਾਈ = maxwidth;
	}
    }

    // Store the info about highlighting.
    ਜੇ (hltab != ਨਿਰਾ)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	ਲਈ (l = 0; l < itemcnt; l++)
	{
	    ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_type == Highlight)
	    {
		sp->ਸ਼ੁਰੂ = stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ;
		sp->userhl = stl_ਚੀਜ਼ਾਂ[l].stl_minwid;
		sp++;
	    }
	}
	sp->ਸ਼ੁਰੂ = ਨਿਰਾ;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    ਜੇ (tabtab != ਨਿਰਾ)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	ਲਈ (l = 0; l < itemcnt; l++)
	{
	    ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_type == Tab_ਪੰਨਾ)
	    {
		sp->ਸ਼ੁਰੂ = stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ;
		sp->userhl = stl_ਚੀਜ਼ਾਂ[l].stl_minwid;
		sp++;
	    }
	}
	sp->ਸ਼ੁਰੂ = ਨਿਰਾ;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    ਜੇ (clicktab != ਨਿਰਾ)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	ਲਈ (l = 0; l < itemcnt; l++)
	{
	    ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_type == ClickFunc)
	    {
		cp->ਸ਼ੁਰੂ = stl_ਚੀਜ਼ਾਂ[l].stl_ਸ਼ੁਰੂ;
		cp->funcname = stl_ਚੀਜ਼ਾਂ[l].stl_clickfunc;
		cp->minwid = stl_ਚੀਜ਼ਾਂ[l].stl_minwid;
		cp++;
	    }
	}
	cp->ਸ਼ੁਰੂ = ਨਿਰਾ;
	cp->funcname = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	// Free click function names when caller doesn't need them.
	ਲਈ (l = 0; l < itemcnt; l++)
	    ਜੇ (stl_ਚੀਜ਼ਾਂ[l].stl_type == ClickFunc)
		vim_free(stl_ਚੀਜ਼ਾਂ[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    ਕੁੰਜੀ_Typed = save_ਕੁੰਜੀ_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    ਜੇ (did_emsg > did_emsg_before)
	ਸੈੱਟ_string_option_ਸਿੱਧਾ(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    ਵਾਪਸ ਚੌੜਾਈ;
}
#ਜੇ_ਅੰਤ // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    ਪੂਰਨ
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    ਪੂਰਨ		buflen)
{
    ਲੰਮਾ	above; // number of lines above window
    ਲੰਮਾ	ਹੇਠਾਂ; // number of lines below window

    ਜੇ (buflen < 3) // need at least 3 chars for writing
	ਵਾਪਸ 0;
    above = wp->w_topline - 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    ਜੇ (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#ਜੇ_ਅੰਤ
    ਹੇਠਾਂ = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - wp->w_botline + 1;
    ਜੇ (ਹੇਠਾਂ <= 0)
	ਵਾਪਸ (ਪੂਰਨ)vim_snprintf_safelen((ਅੱਖਰ *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    ਜੇ (above <= 0)
	ਵਾਪਸ (ਪੂਰਨ)vim_snprintf_safelen((ਅੱਖਰ *)buf, buflen,
	    "%s", _("Top"));

    ਪੂਰਨ perc = calc_percentage(above, above + ਹੇਠਾਂ);
    ਅੱਖਰ tmp[8];
    // localized percentage value
    vim_snprintf(tmp, ਆਕਾਰ(tmp), _("%d%%"), perc);
    ਵਾਪਸ (ਪੂਰਨ)vim_snprintf_safelen((ਅੱਖਰ *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    ਸਥਿਰ ਪੂਰਨ
append_arg_ਸੰਖਿਆ(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    ਪੂਰਨ		ਜੋੜੋ_file)	// Add "file" before the arg number
{
    ਜੇ (ARGCOUNT <= 1)		// nothing to do
	ਵਾਪਸ 0;

    ਅੱਖਰ *ਸੁਨੇਹਾ;
    ਚੋਣ ((wp->w_arg_idx_invalid ? 1 : 0) + (ਜੋੜੋ_file ? 2 : 0))
    {
	ਹਾਲਤ 0: ਸੁਨੇਹਾ = _(" (%d of %d)"); ਰੋਕੋ;
	ਹਾਲਤ 1: ਸੁਨੇਹਾ = _(" ((%d) of %d)"); ਰੋਕੋ;
	ਹਾਲਤ 2: ਸੁਨੇਹਾ = _(" (file %d of %d)"); ਰੋਕੋ;
	ਹਾਲਤ 3: ਸੁਨੇਹਾ = _(" (file (%d) of %d)"); ਰੋਕੋ;
    }

    ਵਾਪਸ (ਪੂਰਨ)vim_snprintf_safelen((ਅੱਖਰ *)buf, buflen, ਸੁਨੇਹਾ,
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIX
    ਵਾਪਸ ਭਰਿਆ_Name_save(fname, TRUE);
#ਨਹੀਂਤਾਂ
    ਜੇ (!vim_isAbsName(fname)
	    || ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)fname, "..") != ਨਿਰਾ
	    || ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)fname, "//") != ਨਿਰਾ
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
	    || ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)fname, "\\\\") != ਨਿਰਾ
# endif
# ਜੇ defined(MSWIN)
	    || vim_strchr(fname, '~') != ਨਿਰਾ
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	ਵਾਪਸ ਭਰਿਆ_Name_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    ਜੇ (fname != ਨਿਰਾ)
	fname_case(fname, 0);	// set correct case for file name
# endif

    ਵਾਪਸ fname;
#ਜੇ_ਅੰਤ
}

/*
 * Make "*ffname" a full file name, set "*sfname" to "*ffname" if not NULL.
 * "*ffname" becomes a pointer to allocated memory (or NULL).
 * When resolving a link both "*sfname" and "*ffname" will point to the same
 * allocated memory.
 * The "*ffname" and "*sfname" pointer values on call will not be freed.
 * Note that the resulting "*ffname" pointer should be considered not allocated.
 */
    ਖਾਲੀ
fname_expand(
    buf_T	*buf ਅਣਵਰਤਿਆ,
    char_u	**ffname,
    char_u	**sfname)
{
    ਜੇ (*ffname == ਨਿਰਾ)	    // no file name given, nothing to do
	ਵਾਪਸ;
    ਜੇ (*sfname == ਨਿਰਾ)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SHORTCUT
    ਜੇ (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_ਰਸਤਾ(*ffname, FALSE);
	ਜੇ (rfname != ਨਿਰਾ)
	{
	    vim_free(*ffname);
	    *ffname = rfname;
	    *sfname = rfname;
	}
    }
#ਜੇ_ਅੰਤ
}

/*
 * Open a window for a number of buffers.
 */
    ਖਾਲੀ
ex_ਬਫਰ_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    ਪੂਰਨ		ਵੰਡੋ_ret = OK;
    ਪੂਰਨ		p_ea_save;
    ਪੂਰਨ		open_wins = 0;
    ਪੂਰਨ		r;
    ਪੂਰਨ		ਗਿਣਤੀ_2;		// Maximum number of windows to open.
    ਪੂਰਨ		all;		// When TRUE also load inactive buffers.
    ਪੂਰਨ		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    ਜੇ (eap->addr_ਗਿਣਤੀ == 0)	// make as many windows as possible
	ਗਿਣਤੀ_2 = 9999;
    ਨਹੀਂਤਾਂ
	ਗਿਣਤੀ_2 = eap->line2;	// make as many windows as specified
    ਜੇ (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    ਨਹੀਂਤਾਂ
	all = TRUE;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    setpcmark();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    need_mouse_correct = TRUE;
#ਜੇ_ਅੰਤ

    /*
     * Close superfluous windows (two windows for the same buffer).
     * Also close windows that are not full-width.
     */
    ਜੇ (had_tab > 0)
	goto_tabpage_tp(ਪਹਿਲਾ_tabpage, TRUE, TRUE);
    ਲਈ (;;)
    {
	tpnext = curtab->tp_ਅਗਲਾ;
	ਲਈ (wp = firstwin; wp != ਨਿਰਾ; wp = wpnext)
	{
	    wpnext = wp->w_ਅਗਲਾ;
	    ਜੇ ((wp->w_ਬਫਰ->b_nwindows > 1
			|| ((cmdmod.cmod_ਵੰਡੋ & WSP_VERT)
			    ? wp->w_ਉਚਾਈ + wp->w_ਸਥਿਤੀ_ਉਚਾਈ < ਪੰਕਤੀਆਂ - p_ch
							     - tabline_ਉਚਾਈ()
			    : wp->w_ਚੌੜਾਈ != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_ਖਿੜਕੀ
		    && !(win_locked(wp) || wp->w_ਬਫਰ->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		ਜੇ (win_close(wp, FALSE) == FAIL)
		    ਰੋਕੋ;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = ਪਹਿਲਾ_tabpage;
		open_wins = 0;
	    }
	    ਨਹੀਂਤਾਂ
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	ਜੇ (had_tab == 0 || tpnext == ਨਿਰਾ)
	    ਰੋਕੋ;
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
    ਲਈ (buf = firstbuf; buf != ਨਿਰਾ && open_wins < ਗਿਣਤੀ_2; buf = buf->b_ਅਗਲਾ)
    {
	// Check if this buffer needs a window
	ਜੇ ((!all && buf->b_ml.ml_mfp == ਨਿਰਾ) || !buf->b_p_bl)
	    ਜਾਰੀ;

	ਜੇ (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    ਜੇ (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    ਨਹੀਂਤਾਂ
		wp = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		ਜੇ (wp->w_ਬਫਰ == buf)
		    ਰੋਕੋ;
	    // If the buffer already has a window, move it
	    ਜੇ (wp != ਨਿਰਾ)
		win_move_after(wp, curwin);
	}

	ਜੇ (wp == ਨਿਰਾ && ਵੰਡੋ_ret == OK)
	{
	    bufref_T	bufref;

	    ਸੈੱਟ_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    ਵੰਡੋ_ret = win_ਵੰਡੋ(0, WSP_ਗੁੰਜਾਇਸ਼ | WSP_ਹੇਠਾਂ);
	    ++open_wins;
	    p_ea = p_ea_save;
	    ਜੇ (ਵੰਡੋ_ret == FAIL)
		ਜਾਰੀ;

	    // Open the buffer in this window.
	    ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_DIALOG;
	    ਸੈੱਟ_curbuf(buf, DOBUF_GOTO);
	    ਜੇ (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_NONE;
		ਰੋਕੋ;
	    }
	    ਜੇ (ਅਦਲਾ_ਬਦਲੀ_exists_action == SEA_QUIT)
	    {
#ਜੇ defined(FEAT_EVAL)
		cleanup_T   cs;

		// Reset the error/interrupt/exception state here so that
		// aborting() returns FALSE when closing a window.
		enter_cleanup(&cs);
#ਜੇ_ਅੰਤ

		// User selected Quit at ATTENTION prompt; close this window.
		win_close(curwin, TRUE);
		--open_wins;
		ਅਦਲਾ_ਬਦਲੀ_exists_action = SEA_NONE;
		ਅਦਲਾ_ਬਦਲੀ_exists_did_quit = TRUE;

#ਜੇ defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ
		handle_ਅਦਲਾ_ਬਦਲੀ_exists(ਨਿਰਾ);
	}

	ui_breakcheck();
	ਜੇ (got_int)
	{
	    (ਖਾਲੀ)vgetc();	// only break the file loading, not the rest
	    ਰੋਕੋ;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	ਜੇ (aborting())
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	// When ":tab" was used open a new tab for a new window repeatedly.
	ਜੇ (had_tab > 0 && tabpage_ਸੂਚਕਾਂਕ(ਨਿਰਾ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    ਲਈ (wp = lastwin; open_wins > ਗਿਣਤੀ_2; )
    {
	r = (buf_hide(wp->w_ਬਫਰ) || !buf_Is_ਬਦਲਿਆ(wp->w_ਬਫਰ)
				     || autowrite(wp->w_ਬਫਰ, FALSE) == OK);
	ਜੇ (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	ਨਹੀਂਤਾਂ ਜੇ (r)
	{
	    win_close(wp, !buf_hide(wp->w_ਬਫਰ));
	    --open_wins;
	    wp = lastwin;
	}
	ਨਹੀਂਤਾਂ
	{
	    wp = wp->w_prev;
	    ਜੇ (wp == ਨਿਰਾ)
		ਰੋਕੋ;
	}
    }
}


ਸਥਿਰ ਪੂਰਨ  chk_modeline(linenr_T, ਪੂਰਨ);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    ਖਾਲੀ
do_modelines(ਪੂਰਨ flags)
{
    linenr_T	lnum;
    ਪੂਰਨ		nmlines;
    ਸਥਿਰ ਪੂਰਨ	entered = 0;

    ਜੇ (!curbuf->b_p_ml || (nmlines = (ਪੂਰਨ)p_mls) == 0)
	ਵਾਪਸ;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    ਜੇ (entered)
	ਵਾਪਸ;

    ++entered;
    ਲਈ (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ && lnum <= nmlines;
								       ++lnum)
	ਜੇ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    ਲਈ (lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - nmlines; --lnum)
	ਜੇ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    ਸਥਿਰ ਪੂਰਨ
chk_modeline(
    linenr_T	lnum,
    ਪੂਰਨ		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*ਸਤਰ_ਸਿਰਾ;		// point to the end of the line
    char_u	*e;
    ਪੂਰਨ		prev;
    ਪੂਰਨ		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    ਸਤਰ_ਸਿਰਾ = s + ml_get_len(lnum);
    ਲਈ (; *s != NUL; ++s)
    {
	ਜੇ (prev == -1 || vim_isspace(prev))
	{
	    ਜੇ ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		ਰੋਕੋ;
	    // Accept both "vim" and "Vim".
	    ਜੇ ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		ਪੂਰਨ vers;

		ਜੇ (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		ਨਹੀਂਤਾਂ
		    e = s + 3;
		vers = getdigits(&e);
		ਜੇ (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_ਸੰਸਕਰਣ_100 >= vers && ਸੁਰੱਖਿਅਤ_isdigit(s[3]))
			    || (VIM_ਸੰਸਕਰਣ_100 < vers && s[3] == '<')
			    || (VIM_ਸੰਸਕਰਣ_100 > vers && s[3] == '>')
			    || (VIM_ਸੰਸਕਰਣ_100 == vers && s[3] == '=')))
		    ਰੋਕੋ;
	    }
	}
	prev = *s;
    }

    ਜੇ (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	ਪੂਰਨ	ਸਿਰਾ;

	ਕਰੋ				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	ਜਦੋਂ (s[-1] != ':');

	len = (size_t)(ਸਤਰ_ਸਿਰਾ - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	ਜੇ (linecopy == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਸਤਰ_ਸਿਰਾ = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_ਧੱਕੋ(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_ਤਿਆਰੀ;

	ਸਿਰਾ = FALSE;
	ਜਦੋਂ (ਸਿਰਾ == FALSE)
	{
	    s = skipwhite(s);
	    ਜੇ (*s == NUL)
		ਰੋਕੋ;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    ਲਈ (e = s; *e != ':' && *e != NUL; ++e)
		ਜੇ (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(ਸਤਰ_ਸਿਰਾ - (e + 1)) + 1);	// +1 for NUL
		    --ਸਤਰ_ਸਿਰਾ;
		}
	    ਜੇ (*e == NUL)
		ਸਿਰਾ = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    ਜੇ (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		ਜੇ (*e != ':')		// no terminating ':'?
		    ਰੋਕੋ;
		ਸਿਰਾ = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    ਜੇ (*s != NUL)		// skip over an empty "::"
	    {
		ਪੂਰਨ secure_save = secure;
		sctx_T	save_current_sctx = current_sctx;

		current_sctx.sc_ਸੰਸਕਰਣ = 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
		current_sctx.sc_sid = SID_MODELINE;
		current_sctx.sc_seq = 0;
		current_sctx.sc_lnum = lnum;
#ਜੇ_ਅੰਤ

		// Make sure no risky things are executed as a side effect.
		secure = 1;

		retval = do_ਸੈੱਟ(s, OPT_MODELINE | OPT_LOCAL | flags);

		secure = secure_save;
		current_sctx = save_current_sctx;
		ਜੇ (retval == FAIL)		// stop if error found
		    ਰੋਕੋ;
	    }
	    s = (e == ਸਤਰ_ਸਿਰਾ) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_ਕੱਢੋ();
	vim_free(linecopy);
    }
    ਵਾਪਸ retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    ਪੂਰਨ
bt_ਸਾਧਾਰਨ(buf_T *buf)
{
    ਵਾਪਸ buf != ਨਿਰਾ && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    ਪੂਰਨ
bt_quickfix(buf_T *buf ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    ਵਾਪਸ buf != ਨਿਰਾ && buf->b_p_bt[0] == 'q';
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    ਪੂਰਨ
bt_terminal(buf_T *buf ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_TERMINAL)
    ਵਾਪਸ buf != ਨਿਰਾ && buf->b_p_bt[0] == 't';
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    ਪੂਰਨ
bt_help(buf_T *buf)
{
    ਵਾਪਸ buf != ਨਿਰਾ && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    ਪੂਰਨ
bt_prompt(buf_T *buf)
{
    ਵਾਪਸ buf != ਨਿਰਾ && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#ਜੇ defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    ਪੂਰਨ
bt_popup(buf_T *buf)
{
    ਵਾਪਸ buf != ਨਿਰਾ && buf->b_p_bt != ਨਿਰਾ
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    ਪੂਰਨ
bt_nofilename(buf_T *buf)
{
    ਵਾਪਸ buf != ਨਿਰਾ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    ਸਥਿਰ ਪੂਰਨ
bt_nofileread(buf_T *buf)
{
    ਵਾਪਸ buf != ਨਿਰਾ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#ਜੇ defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    ਪੂਰਨ
bt_nofile(buf_T *buf)
{
    ਵਾਪਸ buf != ਨਿਰਾ && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    ਪੂਰਨ
bt_dontwrite(buf_T *buf)
{
    ਵਾਪਸ buf != ਨਿਰਾ && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    ਪੂਰਨ
bt_dontwrite_ਸੁਨੇਹਾ(buf_T *buf)
{
    ਜੇ (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_ਸੈੱਟ));
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    ਪੂਰਨ
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    ਚੋਣ (buf->b_p_bh[0])
    {
	ਹਾਲਤ 'u':		    // "unload"
	ਹਾਲਤ 'w':		    // "wipe"
	ਹਾਲਤ 'd': ਵਾਪਸ FALSE;	    // "delete"
	ਹਾਲਤ 'h': ਵਾਪਸ TRUE;	    // "hide"
    }
    ਵਾਪਸ (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#ਜੇ defined(FEAT_QUICKFIX)
    ਜੇ (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	ਜੇ (buf->b_fnum == qf_ਢੇਰ_get_bufnr())
	    ਵਾਪਸ (char_u *)_(ਸੁਨੇਹਾ_qflist);
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ (char_u *)_(ਸੁਨੇਹਾ_loclist);
    }
#ਜੇ_ਅੰਤ

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    ਜੇ (bt_nofilename(buf))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	ਜੇ (buf->b_ਪਦ != ਨਿਰਾ)
	    ਵਾਪਸ ਪਦ_get_ਸਥਿਤੀ_ਪਾਠ(buf->b_ਪਦ);
#ਜੇ_ਅੰਤ
	ਜੇ (buf->b_fname != ਨਿਰਾ)
	    ਵਾਪਸ buf->b_fname;
	ਜੇ (buf == cmdwin_buf)
	    ਵਾਪਸ (char_u *)_("[Command Line]");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	ਜੇ (bt_prompt(buf))
	    ਵਾਪਸ (char_u *)_("[Prompt]");
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
	ਜੇ (bt_popup(buf))
	    ਵਾਪਸ (char_u *)_("[Popup]");
#ਜੇ_ਅੰਤ
	ਵਾਪਸ (char_u *)_("[Scratch]");
    }

    ਜੇ (buf->b_fname == ਨਿਰਾ)
	ਵਾਪਸ buf_get_fname(buf);
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    ਜੇ (buf->b_fname == ਨਿਰਾ)
	ਵਾਪਸ (char_u *)_("[No Name]");
    ਵਾਪਸ buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    ਖਾਲੀ
ਸੈੱਟ_buflisted(ਪੂਰਨ on)
{
    ਜੇ (on == curbuf->b_p_bl)
	ਵਾਪਸ;

    curbuf->b_p_bl = on;
    ਜੇ (on)
	apply_autocmds(EVENT_BUFADD, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
    ਨਹੀਂਤਾਂ
	apply_autocmds(EVENT_BUFDELETE, ਨਿਰਾ, ਨਿਰਾ, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    ਪੂਰਨ
buf_contents_ਬਦਲਿਆ(buf_T *buf)
{
    buf_T	*newbuf;
    ਪੂਰਨ		ਫਰਕ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ਨਿਰਾ, ਨਿਰਾ, (linenr_T)1, BLN_DUMMY);
    ਜੇ (newbuf == ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    ਜੇ (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_ਬਫਰ(newbuf, FALSE);
	ਵਾਪਸ TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    ਜੇ (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_ਬਫਰ(newbuf, FALSE);
	ਵਾਪਸ TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    ਖੰਡ_autocmds();
    ਜੇ (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	ਜੇ (buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ == curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    ਫਰਕ = FALSE;
	    ਲਈ (lnum = 1; lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++lnum)
		ਜੇ (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    ਫਰਕ = TRUE;
		    ਰੋਕੋ;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    ਜੇ (curbuf != newbuf)	// safety check
	wipe_ਬਫਰ(newbuf, FALSE);

    unblock_autocmds();

    ਵਾਪਸ ਫਰਕ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    ਖਾਲੀ
wipe_ਬਫਰ(
    buf_T	*buf,
    ਪੂਰਨ		aucmd)	    // When TRUE trigger autocommands.
{
    ਜੇ (buf->b_fnum == ਸਿਖਰ_file_ਅੰਕ - 1)
	--ਸਿਖਰ_file_ਅੰਕ;

    ਜੇ (!aucmd)		    // Don't trigger BufDelete autocommands here.
	ਖੰਡ_autocmds();

    close_ਬਫਰ(ਨਿਰਾ, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    ਜੇ (!aucmd)
	unblock_autocmds();
}
