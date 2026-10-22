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


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_ಆಳ 100
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ	enter_ಬಫರ್(buf_T *buf);
ಸ್ಥಿರ ಶೂನ್ಯ	buflist_getfpos(ಶೂನ್ಯ);
ಸ್ಥಿರ char_u	*buflist_ಹೊಂದಿಕೆ(regmatch_T *rmp, buf_T *buf, ಪೂರ್ಣಾಂಕ ignore_case);
ಸ್ಥಿರ char_u	*fname_ಹೊಂದಿಕೆ(regmatch_T *rmp, char_u *name, ಪೂರ್ಣಾಂಕ ignore_case);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
ಸ್ಥಿರ buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	buf_same_ino(buf_T *buf, stat_T *stp);
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	otherfile_buf(buf_T *buf, char_u *ffname);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ಮೌಲ್ಯ_ಬದಲಾಯಿತು(char_u *str, char_u **ಕೊನೆಯ);
#ಆದರೆ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ build_stl_str_hl_local(stl_ಬಹುಲಕ_T ಬಹುಲಕ, win_T *wp,
		char_u *ಹೊರಹೋಗು, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, ಪೂರ್ಣಾಂಕ opt_scope, ಪೂರ್ಣಾಂಕ fillchar, ಪೂರ್ಣಾಂಕ maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, ಪೂರ್ಣಾಂಕ *lbreaks, ಪೂರ್ಣಾಂಕ *ವಾಹಕ_hl);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	append_arg_ಸಂಖ್ಯೆ(win_T *wp, char_u *buf, size_t buflen, ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು_file);
ಸ್ಥಿರ ಶೂನ್ಯ	free_ಬಫರ್(buf_T *);
ಸ್ಥಿರ ಶೂನ್ಯ	free_ಬಫರ್_stuff(buf_T *buf, ಪೂರ್ಣಾಂಕ free_options);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	bt_nofileread(buf_T *buf);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	do_ಬಫರ್_ext(ಪೂರ್ಣಾಂಕ action, ಪೂರ್ಣಾಂಕ ಆರಂಭ, ಪೂರ್ಣಾಂಕ dir, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ flags);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
# define dev_T dev_t
#ಇಲ್ಲದಿದ್ದರೆ
# define dev_T ಚಿಹ್ನೆರಹಿತ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_BUFS_FROM_ಕೊನೆಯ(buf) \
    ಗಾಗಿ ((buf) = lastbuf; (buf) != ಖಾಲಿ; (buf) = (buf)->b_prev)

#ಆದರೆ defined(FEAT_QUICKFIX)
ಸ್ಥಿರ ಅಕ್ಷರ *ಸಂದೇಶ_loclist = N_("[Location List]");
ಸ್ಥಿರ ಅಕ್ಷರ *ಸಂದೇಶ_qflist = N_("[Quickfix List]");
#ಆದರೆ_ಅಂತ್ಯ

// Number of times free_buffer() was called.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	buf_free_ಎಣಿಕೆ = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	ತುದಿ_file_ಅಂಕೆ = 1;	// highest file number
ಸ್ಥಿರ garray_T buf_reuse = GA_ಖಾಲಿಗಳು;	// file numbers to recycle

    ಸ್ಥಿರ ಶೂನ್ಯ
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    ಕಿಟಕಿ_layout_ಬೀಗ();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"ಆದರೆ exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    ಕಿಟಕಿ_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
calc_percentage(ದೀರ್ಘ part, ದೀರ್ಘ ಸಂಪೂರ್ಣ)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    ಹಿಂತಿರುಗಿ (part > 1000000L)
	? (ಪೂರ್ಣಾಂಕ)(part / (ಸಂಪೂರ್ಣ / 100L))
	: (ಪೂರ್ಣಾಂಕ)((part * 100L) / ಸಂಪೂರ್ಣ);
}

/*
 * Return the highest possible buffer number.
 */
    ಪೂರ್ಣಾಂಕ
get_highest_fnum(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ತುದಿ_file_ಅಂಕೆ - 1;
}

/*
 * Read data from buffer for retrying.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
read_ಬಫರ್(
    ಪೂರ್ಣಾಂಕ		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    ಪೂರ್ಣಾಂಕ		flags)		    // extra flags for readfile()
{
    ಪೂರ್ಣಾಂಕ		retval = OK;
    linenr_T	ಸಾಲು_ಎಣಿಕೆ;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    ಸಾಲು_ಎಣಿಕೆ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    retval = readfile(
	    read_stdin ? ಖಾಲಿ : curbuf->b_ffname,
	    read_stdin ? ಖಾಲಿ : curbuf->b_fname,
	    ಸಾಲು_ಎಣಿಕೆ, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_ಬಫರ್);
    ಆದರೆ (retval == OK)
    {
	// Delete the binary lines.
	ಆಗ (--ಸಾಲು_ಎಣಿಕೆ >= 0)
	    ml_delete((linenr_T)1);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Delete the converted lines.
	ಆಗ (curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ > ಸಾಲು_ಎಣಿಕೆ)
	    ml_delete(ಸಾಲು_ಎಣಿಕೆ);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;

    ಆದರೆ (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	ಆದರೆ (!readonlymode && !BUFEMPTY())
	    ಬದಲಾಯಿತು();
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	ಆದರೆ (retval == OK)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE,
							      curbuf, &retval);
#ಇಲ್ಲದಿದ್ದರೆ
	    apply_autocmds(EVENT_STDINREADPOST, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }
    ಹಿಂತಿರುಗಿ retval;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    ಶೂನ್ಯ
ಬಫರ್_ensure_loaded(buf_T *buf)
{
    ಆದರೆ (buf->b_ml.ml_mfp != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    ಆದರೆ (curbuf == buf)
    {
	ಆದರೆ (ಅದಲುಬದಲು_exists_action != SEA_READONLY)
	    ಅದಲುಬದಲು_exists_action = SEA_NONE;
	open_ಬಫರ್(FALSE, ಖಾಲಿ, 0);
	aucmd_restbuf(&aco);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
open_ಬಫರ್(
    ಪೂರ್ಣಾಂಕ		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    ಪೂರ್ಣಾಂಕ		flags_arg)	    // extra flags for readfile()
{
    ಪೂರ್ಣಾಂಕ		flags = flags_arg;
    ಪೂರ್ಣಾಂಕ		retval = OK;
    bufref_T	old_curbuf;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ದೀರ್ಘ	old_tw = curbuf->b_p_tw;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    ಆದರೆ (readonlymode && curbuf->b_ffname != ಖಾಲಿ
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    ಆದರೆ (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_ಬಫರ್(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    ಆದರೆ (curbuf->b_ml.ml_mfp != ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	ಆದರೆ (curbuf == ಖಾಲಿ)
	{
	    emsg(_(e_cannot_allocate_any_ಬಫರ್_exiting));

	    // Don't try to do any saving, with "curbuf" NULL almost nothing
	    // will work.
	    v_dying = 2;
	    getout(2);
	}

	emsg(_(e_cannot_allocate_ಬಫರ್_using_other_one));
	enter_ಬಫರ್(curbuf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಆದರೆ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ಖಾಲಿ, curwin);
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    ಆದರೆ (curbuf->b_ml.ml_mfp != ಖಾಲಿ)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    ಗಣ_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_ಗಣ = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    ಆದರೆ (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    ಆದರೆ (curbuf->b_ffname != ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	    && netbeansReadFile
#ಆದರೆ_ಅಂತ್ಯ
       )
    {
	ಪೂರ್ಣಾಂಕ old_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	ಪೂರ್ಣಾಂಕ	save_bin = curbuf->b_p_bin;
	ಪೂರ್ಣಾಂಕ perm;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	ಪೂರ್ಣಾಂಕ oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	perm = mch_getperm(curbuf->b_ffname);
	ಆದರೆ (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	ಆದರೆ (read_fifo)
	    curbuf->b_p_bin = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (shortmess(SHM_FILEINFO))
	    ಸಂದೇಶ_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	ಆದರೆ (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    ಆದರೆ (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_ಬಫರ್(FALSE, eap, flags);
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಸಂದೇಶ_silent = old_ಸಂದೇಶ_silent;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#ಆದರೆ_ಅಂತ್ಯ
	// Help buffer is filtered.
	ಆದರೆ (bt_help(curbuf))
	    fix_help_ಬಫರ್();
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (read_stdin)
    {
	ಪೂರ್ಣಾಂಕ	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(ಖಾಲಿ, ಖಾಲಿ, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, ಖಾಲಿ,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	ಆದರೆ (retval == OK)
	    retval = read_ಬಫರ್(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    ಆದರೆ (curbuf->b_ml.ml_mfp != ಖಾಲಿ
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    ಆದರೆ (curbuf->b_flags & BF_NEVERLOADED)
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
    ಆದರೆ ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != ಖಾಲಿ)
		|| curbuf->b_modified_was_ಗಣ	// autocmd did ":set modified"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != ಖಾಲಿ)
#ಆದರೆ_ಅಂತ್ಯ
       )
	ಬದಲಾಯಿತು();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_ಕೊನೆಯ_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_ಕೊನೆಯ_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_ಕೊನೆಯ_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಆದರೆ (aborting())
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (got_int)
#ಆದರೆ_ಅಂತ್ಯ
	curbuf->b_flags |= BF_READERR;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#ಆದರೆ_ಅಂತ್ಯ

    // need to set w_topline, unless some autocommand already did that.
    ಆದರೆ (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	curwin->w_topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf, &retval);
#ಇಲ್ಲದಿದ್ದರೆ
    apply_autocmds(EVENT_BUFENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (retval != OK)
	ಹಿಂತಿರುಗಿ retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    ಆದರೆ (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != ಖಾಲಿ)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	ಆದರೆ (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    ಆದರೆ ((flags & READ_NOWINENTER) == 0)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, ಖಾಲಿ, ಖಾಲಿ,
			FALSE, curbuf, &retval);
#ಇಲ್ಲದಿದ್ದರೆ
	    apply_autocmds(EVENT_BUFWINENTER, ಖಾಲಿ, ಖಾಲಿ,
		    FALSE, curbuf);
#ಆದರೆ_ಅಂತ್ಯ

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    ಶೂನ್ಯ
ಗಣ_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == ಖಾಲಿ ? 0 : buf->b_fnum;
    bufref->br_buf_free_ಎಣಿಕೆ = buf_free_ಎಣಿಕೆ;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    ಪೂರ್ಣಾಂಕ
bufref_valid(bufref_T *bufref)
{
    ಹಿಂತಿರುಗಿ bufref->br_buf_free_ಎಣಿಕೆ == buf_free_ಎಣಿಕೆ
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    ಪೂರ್ಣಾಂಕ
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_ಕೊನೆಯ(bp)
	ಆದರೆ (bp == buf)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
ಸ್ಥಿರ hashtab_T buf_hashtab;

    ಸ್ಥಿರ ಶೂನ್ಯ
buf_hashtab_ಕೂಡಿಸು(buf_T *buf)
{
    vim_snprintf((ಅಕ್ಷರ *)buf->b_ಕೀಲಿ, ಗಾತ್ರ(buf->b_ಕೀಲಿ), "%x", buf->b_fnum);
    ಆದರೆ (hash_ಕೂಡಿಸು(&buf_hashtab, buf->b_ಕೀಲಿ, "create buffer") == FAIL)
	emsg(_(e_ಬಫರ್_cannot_be_registered));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_ಕಂಡುಹಿಡಿ(&buf_hashtab, buf->b_ಕೀಲಿ);

    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
can_unload_ಬಫರ್(buf_T *buf)
{
    ಪೂರ್ಣಾಂಕ	    can_unload = !buf->b_locked;

    ಆದರೆ (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ಆದರೆ (wp->w_ಬಫರ್ == buf)
	    {
		can_unload = FALSE;
		ನಿಲ್ಲಿಸು;
	    }
    }
    // Don't unload the buffer while it's still being saved
    ಆದರೆ (can_unload && buf->b_saving)
	can_unload = FALSE;

    ಆದರೆ (!can_unload)
    {
	char_u *fname = buf->b_fname != ಖಾಲಿ ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_ಗಮ್ಯ_delete_ಬಫರ್_that_is_ಒಳ_use_str),
				fname != ಖಾಲಿ ? fname : (char_u *)"[No Name]");
    }
    ಹಿಂತಿರುಗಿ can_unload;
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
    ಪೂರ್ಣಾಂಕ
close_ಬಫರ್(
    win_T	*win,
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		action,
    ಪೂರ್ಣಾಂಕ		abort_if_ಕೊನೆಯ,
    ಪೂರ್ಣಾಂಕ		ignore_abort,
    ಪೂರ್ಣಾಂಕ		ಗಣ_context)
{
    ಪೂರ್ಣಾಂಕ		hiding_buf;
    bufref_T	bufref;
    ಪೂರ್ಣಾಂಕ		is_curwin = (curwin != ಖಾಲಿ && curwin == win
				&& curwin->w_ಬಫರ್ == buf);
    tabpage_T	*the_curtab = curtab;
    ಪೂರ್ಣಾಂಕ		unload_buf = (action != 0);
    ಪೂರ್ಣಾಂಕ		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    ಪೂರ್ಣಾಂಕ		del_buf = (action == DOBUF_DEL || wipe_buf);
    ಪೂರ್ಣಾಂಕ		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    ಪೂರ್ಣಾಂಕ		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    ಆದರೆ (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    ಆದರೆ (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	ಆದರೆ (ಪದ_ಕೆಲಸ_running(buf->b_ಪದ))
	{
	    ಆದರೆ (wipe_buf || unload_buf)
	    {
		ಆದರೆ (!can_unload_ಬಫರ್(buf))
		    ಹಿಂತಿರುಗಿ FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (del_buf || unload_buf)
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
#ಆದರೆ_ಅಂತ್ಯ

    // Disallow deleting the buffer when it is locked (already being closed or
    // halfway a command that relies on it). Unloading is allowed.
    ಆದರೆ ((del_buf || wipe_buf) && !can_unload_ಬಫರ್(buf))
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (ಗಣ_context && win_valid && win->w_ಬಫರ್ == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	ಆದರೆ (buf->b_nwindows == 1)
	    ಗಣ_ಕೊನೆಯ_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.ಲಂಬಸಾಲು, TRUE);
    }

    ಗಣ_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    ಆದರೆ ((win_valid || closed_popup) && win->w_ಬಫರ್ == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_ವಿಭಜಿಸು;
	ಆದರೆ (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_ಗಮ್ಯ_abort));
	    ಹಿಂತಿರುಗಿ FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_ವಿಭಜಿಸು;
	ಆದರೆ (abort_if_ಕೊನೆಯ && one_ಕಿಟಕಿ())
	    // Autocommands made this the only window.
	    ಹೋಗು aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	ಆದರೆ (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_ವಿಭಜಿಸು;
	    ಆದರೆ (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		ಹೋಗು aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_ವಿಭಜಿಸು;
	    ಆದರೆ (abort_if_ಕೊನೆಯ && one_ಕಿಟಕಿ())
		// Autocommands made this the only window.
		ಹೋಗು aucmd_abort;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// autocmds may abort script processing
	ಆದರೆ (!ignore_abort && aborting())
	    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ಆದರೆ (is_curwin && curwin != win && win_valid)
    {
	ಖಂಡ_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    ಆದರೆ (bt_quickfix(buf) && win_valid && win->w_ಬಫರ್ == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_ಬಫರ್ == buf && buf->b_nwindows == 1);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#ಆದರೆ_ಅಂತ್ಯ

    // Return when another window is displaying the buffer or when not unloaded.
    ಆದರೆ (!hiding_buf || !unload_buf)
	ಹಿಂತಿರುಗಿ FALSE;

    // Always remove the buffer when there is no file name.
    ಆದರೆ (buf->b_ffname == ಖಾಲಿ)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    ಆದರೆ (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	ಹಿಂತಿರುಗಿ FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    ಆದರೆ ((win_valid || closed_popup) && win->w_ಬಫರ್ == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_ಬಫರ್ = ಖಾಲಿ;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    ಆದರೆ (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != ಖಾಲಿ || buf->b_ಮುಂದಿನ != ಖಾಲಿ
#ಆದರೆ defined(EXITFREE)
				|| entered_free_all_mem
#ಆದರೆ_ಅಂತ್ಯ
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	ಆದರೆ (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    ಆದರೆ (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), 50);
	    ಆದರೆ (ga_grow(&buf_reuse, 1) == OK)
		((ಪೂರ್ಣಾಂಕ *)buf_reuse.ga_ದತ್ತಾಂಶ)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	ಆದರೆ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ಇಲ್ಲದಿದ್ದರೆ
	    buf->b_sfname = ಖಾಲಿ;
	VIM_CLEAR(buf->b_ffname);
	ಆದರೆ (buf->b_prev == ಖಾಲಿ)
	    firstbuf = buf->b_ಮುಂದಿನ;
	ಇಲ್ಲದಿದ್ದರೆ
	    buf->b_prev->b_ಮುಂದಿನ = buf->b_ಮುಂದಿನ;
	ಆದರೆ (buf->b_ಮುಂದಿನ == ಖಾಲಿ)
	    lastbuf = buf->b_prev;
	ಇಲ್ಲದಿದ್ದರೆ
	    buf->b_ಮುಂದಿನ->b_prev = buf->b_prev;
	free_ಬಫರ್(buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (del_buf)
	{
	    // Free all internal variables and reset option values, to make
	    // ":bdel" compatible with Vim 5.7.
	    free_ಬಫರ್_stuff(buf, TRUE);

	    // Make it look like a new buffer.
	    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;

	    // Init the options when loaded again.
	    buf->b_p_initialized = false;
	}
	buf_clear_file(buf);
	ಆದರೆ (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    ಪ್ರತಿಜ್ಞಾ(!win_valid || !is_curwin || win == curwin);
    ಹಿಂತಿರುಗಿ TRUE;
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
    buf->b_ಆರಂಭ_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_ಆರಂಭ_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_ಆರಂಭ_bomb = FALSE;
    buf->b_ml.ml_mfp = ಖಾಲಿ;
    buf->b_ml.ml_flags = ML_ಖಾಲಿಗಳು;		// empty buffer
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    netbeans_deleted_all_ಸಾಲುಗಳು(buf);
#ಆದರೆ_ಅಂತ್ಯ
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
    ಪೂರ್ಣಾಂಕ
buf_freeall(buf_T *buf, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    ಪೂರ್ಣಾಂಕ		is_curwin = (curwin != ಖಾಲಿ && curwin->w_ಬಫರ್ == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_ವಿಭಜಿಸು;
    ಗಣ_bufref(&bufref, buf);
    ಆದರೆ (buf->b_ml.ml_mfp != ಖಾಲಿ)
    {
	ಆದರೆ (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಆದರೆ ((flags & BFA_DEL) && buf->b_p_bl)
    {
	ಆದರೆ (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಆದರೆ (flags & BFA_WIPE)
    {
	ಆದರೆ (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_ವಿಭಜಿಸು;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ಆದರೆ (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	ಖಂಡ_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // autocmds may abort script processing
    ಆದರೆ ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    ಆದರೆ (buf == curbuf && !is_curbuf)
	ಹಿಂತಿರುಗಿ FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    ಆದರೆ (buf == curbuf && VIsual_ಸಕ್ರಿಯ
#ಆದರೆ defined(EXITFREE)
	    && !entered_free_all_mem
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಕೊನೆ_visual_ಬಹುಲಕ();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    ಆದರೆ (curwin != ಖಾಲಿ && curwin->w_ಬಫರ್ == buf)
	reset_synblock(curwin);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    ಆದರೆ (win->w_ಬಫರ್ == buf)
		clearFolding(win);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TCL
    tcl_ಬಫರ್_free(buf);
#ಆದರೆ_ಅಂತ್ಯ
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ = 0;    // no lines in buffer
    ಆದರೆ ((flags & BFA_KEEP_UNDO) == 0)
	// free the memory allocated for undo
	// and reset all undo information
	u_clearallandblockfree(buf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    syntax_clear(&buf->b_s);	    // reset syntax info
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    clear_buf_prop_types(buf);
#ಆದರೆ_ಅಂತ್ಯ
    buf->b_flags &= ~BF_READERR;    // a read error is no longer relevant
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_ಬಫರ್(buf_T *buf)
{
    ++buf_free_ಎಣಿಕೆ;
    free_ಬಫರ್_stuff(buf, TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // b:changedtick uses an item in buf_T, remove it now
    dictitem_remove(buf->b_vars, (dictitem_T *)&buf->b_ct_di, "free buffer");
    unref_var_dict(buf->b_vars);
    remove_listeners(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LUA
    lua_ಬಫರ್_free(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
    mzscheme_ಬಫರ್_free(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PERL
    perl_buf_free(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON
    python_ಬಫರ್_free(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PYTHON3
    python3_ಬಫರ್_free(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RUBY
    ruby_ಬಫರ್_free(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    channel_ಬಫರ್_free(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    free_terminal(buf);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    vim_free(buf->b_prompt_ಪಠ್ಯ);
    free_callback(&buf->b_prompt_callback);
    free_callback(&buf->b_prompt_ಅಡಚಣೆ);
#ಆದರೆ_ಅಂತ್ಯ

    buf_hashtab_remove(buf);

    aubuflocal_remove(buf);

    ಆದರೆ (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_ಮುಂದಿನ = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	vim_free(buf);
	ಆದರೆ (curbuf == buf)
	    curbuf = ಖಾಲಿ;  // make clear it's not to be used
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
    di->di_tv.v_ಬೀಗ = VAR_FIXED;
    di->di_tv.vval.v_ಸಂಖ್ಯೆ = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    STRCPY(buf->b_ct_di.di_ಕೀಲಿ, "changedtick");
    (ಶೂನ್ಯ)dict_ಕೂಡಿಸು(buf->b_vars, di);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    ಆಗ (buf->b_wininfo != ಖಾಲಿ)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_ಮುಂದಿನ;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
free_ಬಫರ್_stuff(
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		free_options)		// free options as well
{
    ಆದರೆ (free_options)
    {
	clear_wininfo(buf);		// including window-local options
	free_buf_options(buf, TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
	ga_clear(&buf->b_s.b_langp);
#ಆದರೆ_ಅಂತ್ಯ
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    {
	varnumber_T tick = CHANGEDTICK(buf);

	vars_clear(&buf->b_vars->dv_hashtab); // free all buffer variables
	hash_init(&buf->b_vars->dv_hashtab);
	init_changedtick(buf);
	CHANGEDTICK(buf) = tick;
	remove_listeners(buf);
    }
#ಆದರೆ_ಅಂತ್ಯ
    uc_clear(&buf->b_ucmds);		// clear local user commands
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SIGNS
    buf_delete_signs(buf, (char_u *)"*");	// delete any signs
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    netbeans_file_killed(buf);
#ಆದರೆ_ಅಂತ್ಯ
    map_clear_ಬಹುಲಕ(buf, MAP_ALL_MODES, TRUE, FALSE);  // clear local mappings
    map_clear_ಬಹುಲಕ(buf, MAP_ALL_MODES, TRUE, TRUE);   // clear local abbrevs
    VIM_CLEAR(buf->b_ಆರಂಭ_fenc);
}

/*
 * Free one wininfo_T.
 */
    ಶೂನ್ಯ
free_wininfo(wininfo_T *wip)
{
    ಆದರೆ (wip->wi_optset)
    {
	clear_winopt(&wip->wi_opt);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	deleteFoldRecurse(&wip->wi_folds);
#ಆದರೆ_ಅಂತ್ಯ
    }
    vim_free(wip);
}

/*
 * Go to another buffer.  Handles the result of the ATTENTION dialog.
 */
    ಶೂನ್ಯ
goto_ಬಫರ್(
    exarg_T	*eap,
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		dir,
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2)
{
    bufref_T	old_curbuf;
    ಪೂರ್ಣಾಂಕ		save_sea = ಅದಲುಬದಲು_exists_action;
    ಪೂರ್ಣಾಂಕ		skip_help_buf;

    ಆಯ್ಕೆ (eap->cmdidx)
    {
	ಸ್ಥಿತಿ CMD_bnext:
	ಸ್ಥಿತಿ CMD_sbnext:
	ಸ್ಥಿತಿ CMD_b_ಮುಂದಿನ:
	ಸ್ಥಿತಿ CMD_bprevious:
	ಸ್ಥಿತಿ CMD_sb_ಮುಂದಿನ:
	ಸ್ಥಿತಿ CMD_sbprevious:
	    skip_help_buf = TRUE;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    skip_help_buf = FALSE;
	    ನಿಲ್ಲಿಸು;
    }

    ಗಣ_bufref(&old_curbuf, curbuf);

    ಆದರೆ (ಅದಲುಬದಲು_exists_action == SEA_NONE)
	ಅದಲುಬದಲು_exists_action = SEA_DIALOG;
    (ಶೂನ್ಯ)do_ಬಫರ್_ext(*eap->cmd == 's' ? DOBUF_ವಿಭಜಿಸು : DOBUF_GOTO, ಆರಂಭ, dir, ಎಣಿಕೆ_2,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    ಆದರೆ (ಅದಲುಬದಲು_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#ಆದರೆ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#ಆದರೆ_ಅಂತ್ಯ

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	ಅದಲುಬದಲು_exists_action = save_sea;
	ಅದಲುಬದಲು_exists_did_quit = TRUE;

#ಆದರೆ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
	handle_ಅದಲುಬದಲು_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    ಶೂನ್ಯ
handle_ಅದಲುಬದಲು_exists(bufref_T *old_curbuf)
{
#ಆದರೆ defined(FEAT_EVAL)
    cleanup_T	cs;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ದೀರ್ಘ	old_tw = curbuf->b_p_tw;
#ಆದರೆ_ಅಂತ್ಯ
    buf_T	*buf;

    ಆದರೆ (ಅದಲುಬದಲು_exists_action == SEA_QUIT)
    {
#ಆದರೆ defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#ಆದರೆ_ಅಂತ್ಯ

	// User selected Quit at ATTENTION prompt.  Go back to previous
	// buffer.  If that buffer is gone or the same as the current one,
	// open a new, empty buffer.
	ಅದಲುಬದಲು_exists_action = SEA_NONE;	// don't want it again
	ಅದಲುಬದಲು_exists_did_quit = TRUE;
	close_ಬಫರ್(curwin, curbuf, DOBUF_UNLOAD, FALSE, FALSE, TRUE);
	ಆದರೆ (old_curbuf == ಖಾಲಿ || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    ಖಂಡ_autocmds();
	    buf = buflist_new(ಖಾಲಿ, ಖಾಲಿ, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    buf = old_curbuf->br_buf;
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ old_ಸಂದೇಶ_silent = ಸಂದೇಶ_silent;

	    ಆದರೆ (shortmess(SHM_FILEINFO))
		ಸಂದೇಶ_silent = 1;  // prevent fileinfo message
	    enter_ಬಫರ್(buf);
	    // restore msg_silent, so that the command line will be shown
	    ಸಂದೇಶ_silent = old_ಸಂದೇಶ_silent;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	    ಆದರೆ (old_tw != curbuf->b_p_tw)
		check_colorcolumn(ಖಾಲಿ, curwin);
#ಆದರೆ_ಅಂತ್ಯ
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#ಆದರೆ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅದಲುಬದಲು_exists_action == SEA_RECOVER)
    {
#ಆದರೆ defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#ಆದರೆ_ಅಂತ್ಯ

	// User selected Recover at ATTENTION prompt.
	ಸಂದೇಶ_scroll = TRUE;
	ml_recover(FALSE);
	ಸಂದೇಶ_puts("\n");	// don't overwrite the last message
	cmdline_ಅಡ್ಡಸಾಲು = ಸಂದೇಶ_ಅಡ್ಡಸಾಲು;
	do_modelines(0);

#ಆದರೆ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಅದಲುಬದಲು_exists_action = SEA_NONE;
}

/*
 * Make the current buffer empty.
 * Used when it is wiped out and it's the last buffer.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಖಾಲಿಗಳು_curbuf(
    ಪೂರ್ಣಾಂಕ close_others,
    ಪೂರ್ಣಾಂಕ forceit,
    ಪೂರ್ಣಾಂಕ action)
{
    ಪೂರ್ಣಾಂಕ	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    ಆದರೆ (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_ಕೊನೆಯ_ಬಫರ್));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಗಣ_bufref(&bufref, buf);
    ಆದರೆ (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    ಆದರೆ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_ಬಫರ್(ಖಾಲಿ, buf, action, FALSE, FALSE, FALSE);
    ಆದರೆ (!close_others)
	need_fileinfo = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    ಹಿಂತಿರುಗಿ retval;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_ಬಫರ್_ext(
    ಪೂರ್ಣಾಂಕ		action,
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		dir,		// FORWARD or BACKWARD
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2,		// buffer number or number of buffers
    ಪೂರ್ಣಾಂಕ		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    ಪೂರ್ಣಾಂಕ		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    ಆಯ್ಕೆ (ಆರಂಭ)
    {
	ಸ್ಥಿತಿ DOBUF_ಮೊದಲ:   buf = firstbuf; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DOBUF_ಕೊನೆಯ:    buf = lastbuf;  ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:	    buf = curbuf;   ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (ಆರಂಭ == DOBUF_MOD)	    // find next modified buffer
    {
	ಆಗ (ಎಣಿಕೆ_2-- > 0)
	{
	    ಮಾಡು
	    {
		buf = buf->b_ಮುಂದಿನ;
		ಆದರೆ (buf == ಖಾಲಿ)
		    buf = firstbuf;
	    }
	    ಆಗ (buf != curbuf && !buf_Is_ಬದಲಾಯಿತು(buf));
	}
	ಆದರೆ (!buf_Is_ಬದಲಾಯಿತು(buf))
	{
	    emsg(_(e_no_modified_ಬಫರ್_ಸಿಕ್ಕಿತು));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆರಂಭ == DOBUF_ಮೊದಲ && ಎಣಿಕೆ_2) // find specified buffer number
    {
	ಆಗ (buf != ಖಾಲಿ && buf->b_fnum != ಎಣಿಕೆ_2)
	    buf = buf->b_ಮುಂದಿನ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = ಖಾಲಿ;
	ಆಗ (ಎಣಿಕೆ_2 > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    ಆದರೆ (bp == ಖಾಲಿ)
		bp = buf;
	    ಆದರೆ (dir == FORWARD)
	    {
		buf = buf->b_ಮುಂದಿನ;
		ಆದರೆ (buf == ಖಾಲಿ)
		    buf = firstbuf;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		buf = buf->b_prev;
		ಆದರೆ (buf == ಖಾಲಿ)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    ಆದರೆ (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--ಎಣಿಕೆ_2;
		bp = ಖಾಲಿ;	// use this buffer as new starting point
	    }
	    ಆದರೆ (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_ಬಫರ್));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
    }

    ಆದರೆ (buf == ಖಾಲಿ)	    // could not find it
    {
	ಆದರೆ (ಆರಂಭ == DOBUF_ಮೊದಲ)
	{
	    // don't warn when deleting
	    ಆದರೆ (!unload)
		semsg(_(e_ಬಫರ್_nr_does_not_exist), ಎಣಿಕೆ_2);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_ಕೊನೆಯ_ಬಫರ್));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_cannot_go_before_ಮೊದಲ_ಬಫರ್));
	ಹಿಂತಿರುಗಿ FAIL;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	ಹಿಂತಿರುಗಿ OK;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_ಗಣ_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ ((action == DOBUF_GOTO || action == DOBUF_ವಿಭಜಿಸು)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_ಬಫರ್_nr_does_not_exist), ಎಣಿಕೆ_2);
	ಹಿಂತಿರುಗಿ FAIL;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * delete buffer "buf" from memory and/or the list
     */
    ಆದರೆ (unload)
    {
	ಪೂರ್ಣಾಂಕ	forward;
	bufref_T bufref;

	ಆದರೆ (!can_unload_ಬಫರ್(buf))
	    ಹಿಂತಿರುಗಿ FAIL;

	ಗಣ_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	ಆದರೆ (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == ಖಾಲಿ && !buf->b_p_bl)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ ((flags & DOBUF_FORCEIT) == 0 && buf_Is_ಬದಲಾಯಿತು(buf))
	{
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ಆದರೆ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		ಆದರೆ (ಪದ_ಕೆಲಸ_running(buf->b_ಪದ))
		{
		    ಆದರೆ (ಪದ_confirm_stop(buf) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
# endif
		{
		    dialog_ಬದಲಾಯಿತು(buf, FALSE);
		    ಆದರೆ (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			ಹಿಂತಿರುಗಿ FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(buf))
			ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		no_write_ಸಂದೇಶ_buf(buf);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	ಆದರೆ (buf == curbuf && VIsual_ಸಕ್ರಿಯ)
	    ಕೊನೆ_visual_ಬಹುಲಕ();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    ಆದರೆ (bp->b_p_bl && bp != buf)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (bp == ಖಾಲಿ && buf == curbuf)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	ಆಗ (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_ಬಫರ್->b_locked > 0)
		   && (!ONE_ಕಿಟಕಿ || ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ))
	{
	    ಆದರೆ (win_close(curwin, FALSE) == FAIL)
		ನಿಲ್ಲಿಸು;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	ಆದರೆ (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    ಆದರೆ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_ಬಫರ್(ಖಾಲಿ, buf, action, FALSE, FALSE, FALSE);
	    ಹಿಂತಿರುಗಿ OK;
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
	buf = ಖಾಲಿ;	// selected buffer
	bp = ಖಾಲಿ;	// used when no loaded buffer found
	ಆದರೆ (au_new_curbuf.br_buf != ಖಾಲಿ && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_ವಿಭಜಿಸು)
	    buf = au_new_curbuf.br_buf;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin->w_jumplistlen > 0)
	{
	    ಪೂರ್ಣಾಂಕ     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    ಆದರೆ (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    ಆಗ (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		ಆದರೆ (buf != ಖಾಲಿ)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    ಆದರೆ (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_ವಿಭಜಿಸು)
			buf = ಖಾಲಿ;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
		    {
			// skip unloaded buf, but may keep it for later
			ಆದರೆ (bp == ಖಾಲಿ)
			    bp = buf;
			buf = ಖಾಲಿ;
		    }
		}
		ಆದರೆ (buf != ಖಾಲಿ)   // found a valid buffer: stop searching
		    ನಿಲ್ಲಿಸು;
		// advance to older entry in jump list
		ಆದರೆ (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		ಆದರೆ (jumpidx == forward)		// List exhausted for sure
		    ನಿಲ್ಲಿಸು;
	    }
	}

	ಆದರೆ (buf == ಖಾಲಿ)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_ಮುಂದಿನ;
	    ಗಾಗಿ (;;)
	    {
		ಆದರೆ (buf == ಖಾಲಿ)
		{
		    ಆದರೆ (!forward)	// tried both directions
			ನಿಲ್ಲಿಸು;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    ಮುಂದುವರಿ;
		}
		// in non-help buffer, try to skip help buffers, and vv
		ಆದರೆ (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_ವಿಭಜಿಸು)
		{
		    ಆದರೆ (buf->b_ml.ml_mfp != ಖಾಲಿ)   // found loaded buffer
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (bp == ಖಾಲಿ)	// remember unloaded buf for later
			bp = buf;
		}
		ಆದರೆ (forward)
		    buf = buf->b_ಮುಂದಿನ;
		ಇಲ್ಲದಿದ್ದರೆ
		    buf = buf->b_prev;
	    }
	}
	ಆದರೆ (buf == ಖಾಲಿ)	// No loaded buffer, use unloaded one
	    buf = bp;
	ಆದರೆ (buf == ಖಾಲಿ)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		ಆದರೆ (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_ವಿಭಜಿಸು)
		    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (buf == ಖಾಲಿ)	// Still no buffer, just take one
	{
	    ಆದರೆ (curbuf->b_ಮುಂದಿನ != ಖಾಲಿ)
		buf = curbuf->b_ಮುಂದಿನ;
	    ಇಲ್ಲದಿದ್ದರೆ
		buf = curbuf->b_prev;
	    ಆದರೆ (bt_quickfix(buf)
		    || (buf != ಖಾಲಿ && buf != curbuf && buf->b_locked_ವಿಭಜಿಸು))
		buf = ಖಾಲಿ;
	}
    }

    ಆದರೆ (buf == ಖಾಲಿ)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    ಆದರೆ (action == DOBUF_ವಿಭಜಿಸು && swbuf_goto_win_with_buf(buf) != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    ಆದರೆ (buf != curbuf && buf->b_locked_ವಿಭಜಿಸು)
    {
	emsg(_(e_cannot_switch_ಗಮ್ಯ_a_closing_ಬಫರ್));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (action == DOBUF_ವಿಭಜಿಸು && win_ವಿಭಜಿಸು(0, 0) == FAIL) // split window first
	ಹಿಂತಿರುಗಿ FAIL;

    // go to current buffer - nothing to do
    ಆದರೆ (buf == curbuf)
	ಹಿಂತಿರುಗಿ OK;

    // Check if the current buffer may be abandoned.
    ಆದರೆ (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ಆದರೆ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ಆದರೆ (ಪದ_ಕೆಲಸ_running(curbuf->b_ಪದ))
	    {
		ಆದರೆ (ಪದ_confirm_stop(curbuf) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
	    {
		bufref_T bufref;

		ಗಣ_bufref(&bufref, buf);
		dialog_ಬದಲಾಯಿತು(curbuf, FALSE);
		ಆದರೆ (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    ಹಿಂತಿರುಗಿ FAIL;

		ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(curbuf))
		{
		    no_write_ಸಂದೇಶ();
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	{
	    no_write_ಸಂದೇಶ();
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    // Go to the other buffer.
    ಗಣ_curbuf(buf, action);

    ಆದರೆ (action == DOBUF_ವಿಭಜಿಸು)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#ಆದರೆ defined(FEAT_EVAL)
    ಆದರೆ (aborting())	    // autocmds may abort script processing
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
do_ಬಫರ್(
    ಪೂರ್ಣಾಂಕ		action,
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		dir,		// FORWARD or BACKWARD
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2,		// buffer number or number of buffers
    ಪೂರ್ಣಾಂಕ		forceit)	// TRUE when using !
{
    ಹಿಂತಿರುಗಿ do_ಬಫರ್_ext(action, ಆರಂಭ, dir, ಎಣಿಕೆ_2,
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
    ಅಕ್ಷರ *
do_bufdel(
    ಪೂರ್ಣಾಂಕ		command,
    char_u	*arg,		// pointer to extra arguments
    ಪೂರ್ಣಾಂಕ		addr_ಎಣಿಕೆ,
    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_bnr,	// first buffer number in a range
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_bnr,	// buffer nr or last buffer nr in a range
    ಪೂರ್ಣಾಂಕ		forceit)
{
    ಪೂರ್ಣಾಂಕ		do_current = 0;	// delete current buffer?
    ಪೂರ್ಣಾಂಕ		deleted = 0;	// number of buffers deleted
    ಅಕ್ಷರ	*errormsg = ಖಾಲಿ; // return value
    ಪೂರ್ಣಾಂಕ		bnr;		// buffer number
    char_u	*p;

    ಆದರೆ (addr_ಎಣಿಕೆ == 0)
    {
	(ಶೂನ್ಯ)do_ಬಫರ್(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (addr_ಎಣಿಕೆ == 2)
	{
	    ಆದರೆ (*arg)		// both range and argument is not allowed
		ಹಿಂತಿರುಗಿ ex_errmsg(e_trailing_characters_str, arg);
	    bnr = ಆರಂಭ_bnr;
	}
	ಇಲ್ಲದಿದ್ದರೆ	// addr_count == 1
	    bnr = ಕೊನೆ_bnr;

	ಗಾಗಿ ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    ಆದರೆ (bnr == curbuf->b_fnum)
		do_current = bnr;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (do_ಬಫರ್_ext(command, DOBUF_ಮೊದಲ, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    ಆದರೆ (addr_ಎಣಿಕೆ == 2)
	    {
		ಆದರೆ (++bnr > ಕೊನೆ_bnr)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ    // addr_count == 1
	    {
		arg = skipwhite(arg);
		ಆದರೆ (*arg == NUL)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    ಆದರೆ (bnr < 0)	    // failed
			ನಿಲ್ಲಿಸು;
		    arg = p;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    bnr = getdigits(&arg);
	    }
	}
	ಆದರೆ (!got_int && do_current && do_ಬಫರ್(command, DOBUF_ಮೊದಲ,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	ಆದರೆ (deleted == 0)
	{
	    ಆದರೆ (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    ಇಲ್ಲದಿದ್ದರೆ
		STRCPY(IObuff, _(e_no_buffers_were_wiped_ಹೊರಹೋಗು));
	    errormsg = (ಅಕ್ಷರ *)IObuff;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (deleted >= p_report)
	{
	    ಆದರೆ (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    ಇಲ್ಲದಿದ್ದರೆ
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    ಹಿಂತಿರುಗಿ errormsg;
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
ಗಣ_curbuf(buf_T *buf, ಪೂರ್ಣಾಂಕ action)
{
    buf_T	*prevbuf;
    ಪೂರ್ಣಾಂಕ		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    ದೀರ್ಘ	old_tw = curbuf->b_p_tw;
#ಆದರೆ_ಅಂತ್ಯ
    bufref_T	newbufref;
    bufref_T	prevbufref;
    ಪೂರ್ಣಾಂಕ		valid;

    setpcmark();
    ಆದರೆ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = curbuf->b_fnum; // remember alternate file
    buflist_altfpos(curwin);			 // remember curpos

    // Don't restart Select mode after switching to another buffer.
    VIsual_reselect = FALSE;

    // close_windows() or apply_autocmds() may change curbuf and wipe out "buf"
    prevbuf = curbuf;
    ಗಣ_bufref(&prevbufref, prevbuf);
    ಗಣ_bufref(&newbufref, buf);

    // Autocommands may delete the current buffer and/or the buffer we want to
    // go to.  In those cases don't close the buffer.
    ಆದರೆ (!apply_autocmds(EVENT_BUFLEAVE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		&& !aborting()
#ಆದರೆ_ಅಂತ್ಯ
	       ))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಆದರೆ (prevbuf == curwin->w_ಬಫರ್)
	    reset_synblock(curwin);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (unload)
	    close_windows(prevbuf, FALSE);
#ಆದರೆ defined(FEAT_EVAL)
	ಆದರೆ (bufref_valid(&prevbufref) && !aborting())
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (bufref_valid(&prevbufref))
#ಆದರೆ_ಅಂತ್ಯ
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    ಆದರೆ (prevbuf == curbuf
		    && ((State & ಬಹುಲಕ_ಸೇರಿಸು) == 0 || curbuf->b_nwindows <= 1))
		u_sync(FALSE);
	    close_ಬಫರ್(curwin, prevbuf,
		    unload ? action : (action == DOBUF_GOTO
			&& !buf_hide(prevbuf)
			&& !buf_Is_ಬದಲಾಯಿತು(prevbuf)) ? DOBUF_UNLOAD : 0,
		    FALSE, FALSE, TRUE);
	}
    }
    // An autocommand may have deleted "buf", already entered it (e.g., when
    // it did ":bunload") or aborted the script processing.
    // If curwin->w_buffer is null, enter_buffer() will make it valid again
    valid = buf_valid(buf);
    ಆದರೆ ((valid && buf != curbuf
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		&& !aborting()
#ಆದರೆ_ಅಂತ್ಯ
	) || curwin->w_ಬಫರ್ == ಖಾಲಿ)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	ಆದರೆ (!valid)
	    enter_ಬಫರ್(lastbuf);
	ಇಲ್ಲದಿದ್ದರೆ
	    enter_ಬಫರ್(buf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
	ಆದರೆ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ಖಾಲಿ, curwin);
#ಆದರೆ_ಅಂತ್ಯ
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
enter_ಬಫರ್(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ
#ಆದರೆ defined(EXITFREE)
	    && !entered_free_all_mem
#ಆದರೆ_ಅಂತ್ಯ
	    )
	ಕೊನೆ_visual_ಬಹುಲಕ();

    ಆದರೆ (curwin->w_ಬಫರ್ != ಖಾಲಿ)
	--curwin->w_ಬಫರ್->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_ಬಫರ್ = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    ಆದರೆ (!buf->b_help)
	get_winopts(buf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    ಇಲ್ಲದಿದ್ದರೆ
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (curwin->w_p_diff)
	diff_buf_ಕೂಡಿಸು(curbuf);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    curwin->w_s = &(curbuf->b_s);
#ಆದರೆ_ಅಂತ್ಯ

    // Cursor on first line by default.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_ಗಣ_curswant = true;
    curwin->w_topline_was_ಗಣ = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // Make sure the buffer is loaded.
    ಆದರೆ (curbuf->b_ml.ml_mfp == ಖಾಲಿ)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	ಆದರೆ (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_ಬಫರ್(FALSE, ಖಾಲಿ, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!ಸಂದೇಶ_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(ಶೂನ್ಯ)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	curwin->w_topfill = 0;
#ಆದರೆ_ಅಂತ್ಯ
	apply_autocmds(EVENT_BUFENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    ಆದರೆ (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    ಆದರೆ (curwin->w_topline == 1 && !curwin->w_topline_was_ಗಣ)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#ಆದರೆ_ಅಂತ್ಯ

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
    ಆದರೆ (curbuf->b_kmap_state & KEYMAP_INIT)
	(ಶೂನ್ಯ)keymap_init();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    ಆದರೆ (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(ಶೂನ್ಯ)parse_spelllang(curwin);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    curbuf->b_ಕೊನೆಯ_ಬಳಸಿದ = vim_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ

    redraw_later(UPD_NOT_VALID);
}

#ಆದರೆ defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    ಶೂನ್ಯ
do_autochdir(ಶೂನ್ಯ)
{
    ಆದರೆ ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != ಖಾಲಿ
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	ಕೊನೆಯ_chdir_reason = "autochdir";
    }
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
no_write_ಸಂದೇಶ_buf(buf_T *buf)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (ಪದ_ಕೆಲಸ_running(buf->b_ಪದ))
	emsg(_(e_ಕೆಲಸ_still_running_ಕೂಡಿಸು_bang_ಗಮ್ಯ_ಕೊನೆ_the_ಕೆಲಸ));
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	semsg(_(e_no_write_since_ಕೊನೆಯ_change_for_ಬಫರ್_nr_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override),
		buf->b_fnum);
}

    ಶೂನ್ಯ
no_write_ಸಂದೇಶ(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (ಪದ_ಕೆಲಸ_running(curbuf->b_ಪದ))
	emsg(_(e_ಕೆಲಸ_still_running_ಕೂಡಿಸು_bang_ಗಮ್ಯ_ಕೊನೆ_the_ಕೆಲಸ));
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	emsg(_(e_no_write_since_ಕೊನೆಯ_change_ಕೂಡಿಸು_bang_ಗಮ್ಯ_override));
}

    ಶೂನ್ಯ
no_write_ಸಂದೇಶ_nobang(buf_T *buf ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (ಪದ_ಕೆಲಸ_running(buf->b_ಪದ))
	emsg(_(e_ಕೆಲಸ_still_running));
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	emsg(_(e_no_write_since_ಕೊನೆಯ_change));
}

/*
 * functions for dealing with the buffer list
 */

/*
 * Return TRUE if the current buffer is empty, unnamed, unmodified and used in
 * only one window.  That means it can be re-used.
 */
    ಪೂರ್ಣಾಂಕ
curbuf_reusable(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (curbuf != ಖಾಲಿ
	&& curbuf->b_ffname == ಖಾಲಿ
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == ಖಾಲಿ || BUFEMPTY())
	&& !bt_quickfix(curbuf)
	&& !curbuf_Is_ಬದಲಾಯಿತು());
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
    ಪೂರ್ಣಾಂಕ		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    stat_T	st;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ತುದಿ_file_ಅಂಕೆ == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    ಆದರೆ (sfname == ಖಾಲಿ || mch_stat((ಅಕ್ಷರ *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (ffname != ಖಾಲಿ && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		buflist_findname_stat(ffname, &st)
#ಇಲ್ಲದಿದ್ದರೆ
		buflist_findname(ffname)
#ಆದರೆ_ಅಂತ್ಯ
		) != ಖಾಲಿ)
    {
	vim_free(ffname);
	ಆದರೆ (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? ಖಾಲಿ : curwin,
						      lnum, (colnr_T)0, FALSE);

	ಆದರೆ ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	ಆದರೆ ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    ಗಣ_bufref(&bufref, buf);
	    ಆದರೆ (!(flags & BLN_DUMMY))
	    {
		ಆದರೆ (apply_autocmds(EVENT_BUFADD, ಖಾಲಿ, ಖಾಲಿ, FALSE, buf)
			&& !bufref_valid(&bufref))
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
	ಹಿಂತಿರುಗಿ buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = ಖಾಲಿ;
    ಆದರೆ ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	ಗಣ_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (!bufref_valid(&bufref))
	    buf = ಖಾಲಿ;		// buf was deleted; allocate a new buffer
    }
    ಆದರೆ (buf != curbuf || curbuf == ಖಾಲಿ)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    vim_free(ffname);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	ಆದರೆ (buf->b_vars == ಖಾಲಿ)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#ಆದರೆ_ಅಂತ್ಯ
	init_changedtick(buf);
    }

    ಆದರೆ (ffname != ಖಾಲಿ)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    ಆದರೆ ((ffname != ಖಾಲಿ && (buf->b_ffname == ಖಾಲಿ || buf->b_sfname == ಖಾಲಿ))
	    || buf->b_wininfo == ಖಾಲಿ)
    {
	ಆದರೆ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ಇಲ್ಲದಿದ್ದರೆ
	    buf->b_sfname = ಖಾಲಿ;
	VIM_CLEAR(buf->b_ffname);
	ಆದರೆ (buf != curbuf)
	    free_ಬಫರ್(buf);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (buf == curbuf)
    {
	free_ಬಫರ್_stuff(buf, FALSE);	// delete local variables et al.

	// Init the options.
	buf->b_p_initialized = false;
	buf_copy_options(buf, BCO_ENTER);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
	// need to reload lmaps and set b:keymap_name
	curbuf->b_kmap_state |= KEYMAP_INIT;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// put the new buffer at the end of the buffer list
	buf->b_ಮುಂದಿನ = ಖಾಲಿ;
	ಆದರೆ (firstbuf == ಖಾಲಿ)		// buffer list is empty
	{
	    buf->b_prev = ಖಾಲಿ;
	    firstbuf = buf;
	}
	ಇಲ್ಲದಿದ್ದರೆ				// append new buffer at end of list
	{
	    lastbuf->b_ಮುಂದಿನ = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	ಆದರೆ ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((ಪೂರ್ಣಾಂಕ *)buf_reuse.ga_ದತ್ತಾಂಶ)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    ಆಗ (buf->b_prev != ಖಾಲಿ && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_ಮುಂದಿನ = buf->b_ಮುಂದಿನ;
		ಆದರೆ (prev->b_ಮುಂದಿನ != ಖಾಲಿ)
		    prev->b_ಮುಂದಿನ->b_prev = prev;
		buf->b_ಮುಂದಿನ = prev;
		buf->b_prev = prev->b_prev;
		ಆದರೆ (buf->b_prev != ಖಾಲಿ)
		    buf->b_prev->b_ಮುಂದಿನ = buf;
		prev->b_prev = buf;
		ಆದರೆ (lastbuf == buf)
		    lastbuf = prev;
		ಆದರೆ (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    buf->b_fnum = ತುದಿ_file_ಅಂಕೆ++;
	ಆದರೆ (ತುದಿ_file_ಅಂಕೆ < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    ಆದರೆ (emsg_silent == 0 && !ಒಳ_assert_fails)
	    {
		ಹೊರಹೋಗು_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    ತುದಿ_file_ಅಂಕೆ = 1;
	}
	buf_hashtab_ಕೂಡಿಸು(buf);

	// Always copy the options from the current buffer.
	buf_copy_options(buf, BCO_ALWAYS);
    }

    buf->b_wininfo->wi_fpos.lnum = lnum;
    buf->b_wininfo->wi_win = curwin;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    hash_init(&buf->b_s.b_keywtab);
    hash_init(&buf->b_s.b_keywtab_ic);
#ಆದರೆ_ಅಂತ್ಯ

    buf->b_fname = buf->b_sfname;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಆದರೆ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#ಆದರೆ_ಅಂತ್ಯ
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    ಆದರೆ (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_ಹೆಸರುಗಳು(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    ಆದರೆ (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	ಗಣ_bufref(&bufref, buf);
	ಆದರೆ (apply_autocmds(EVENT_BUFNEW, ಖಾಲಿ, ಖಾಲಿ, FALSE, buf)
		&& !bufref_valid(&bufref))
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (flags & BLN_LISTED)
	{
	    ಆದರೆ (apply_autocmds(EVENT_BUFADD, ಖಾಲಿ, ಖಾಲಿ, FALSE, buf)
		    && !bufref_valid(&bufref))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ಆದರೆ (aborting())		// autocmds may abort script processing
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    ಶೂನ್ಯ
free_buf_options(
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		free_p_ff)
{
    ಆದರೆ (free_p_ff)
    {
	clear_string_option(&buf->b_p_fenc);
	clear_string_option(&buf->b_p_ff);
	clear_string_option(&buf->b_p_bh);
	clear_string_option(&buf->b_p_bt);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕಂಡುಹಿಡಿ_ID
    clear_string_option(&buf->b_p_def);
    clear_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    clear_string_option(&buf->b_p_inex);
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&buf->b_p_fp);
#ಆದರೆ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CRYPT
# ifdef FEAT_SODIUM
    ಆದರೆ (buf->b_p_ಕೀಲಿ != ಖಾಲಿ && *buf->b_p_ಕೀಲಿ != NUL
			   && crypt_ವಿಧಾನ_is_sodium(crypt_get_ವಿಧಾನ_nr(buf)))
	crypt_sodium_munlock(buf->b_p_ಕೀಲಿ, STRLEN(buf->b_p_ಕೀಲಿ));
# endif
    clear_string_option(&buf->b_p_ಕೀಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&buf->b_p_kp);
    clear_string_option(&buf->b_p_mps);
    clear_string_option(&buf->b_p_fo);
    clear_string_option(&buf->b_p_flp);
    clear_string_option(&buf->b_p_isk);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VARTABS
    clear_string_option(&buf->b_p_vsts);
    VIM_CLEAR(buf->b_p_vsts_nopaste);
    VIM_CLEAR(buf->b_p_vsts_array);
    clear_string_option(&buf->b_p_vts);
    VIM_CLEAR(buf->b_p_vts_array);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_KEYMAP
    clear_string_option(&buf->b_p_keymap);
    keymap_clear(&buf->b_kmap_ga);
    ga_clear(&buf->b_kmap_ga);
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&buf->b_p_com);
    clear_string_option(&buf->b_p_cms);
    clear_string_option(&buf->b_p_nf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYN_HL
    clear_string_option(&buf->b_p_syn);
    clear_string_option(&buf->b_s.b_syn_isk);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SPELL
    clear_string_option(&buf->b_s.b_p_spc);
    clear_string_option(&buf->b_s.b_p_spf);
    vim_regfree(buf->b_s.b_cap_prog);
    buf->b_s.b_cap_prog = ಖಾಲಿ;
    clear_string_option(&buf->b_s.b_p_spl);
    clear_string_option(&buf->b_s.b_p_spo);
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&buf->b_p_sua);
    clear_string_option(&buf->b_p_ft);
    clear_string_option(&buf->b_p_cink);
    clear_string_option(&buf->b_p_cino);
    clear_string_option(&buf->b_p_lop);
    clear_string_option(&buf->b_p_cinsd);
    clear_string_option(&buf->b_p_cinw);
    clear_string_option(&buf->b_p_cot);
    clear_string_option(&buf->b_p_cpt);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
    clear_string_option(&buf->b_p_csl);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_COMPL_FUNC
    clear_string_option(&buf->b_p_cfu);
    free_callback(&buf->b_cfu_cb);
    clear_string_option(&buf->b_p_ofu);
    free_callback(&buf->b_ofu_cb);
    clear_string_option(&buf->b_p_tsrfu);
    free_callback(&buf->b_tsrfu_cb);
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_ಎಣಿಕೆ);
    buf->b_p_cpt_ಎಣಿಕೆ = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    clear_string_option(&buf->b_p_gefm);
    clear_string_option(&buf->b_p_gp);
    clear_string_option(&buf->b_p_mp);
    clear_string_option(&buf->b_p_efm);
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&buf->b_p_ep);
    clear_string_option(&buf->b_p_ಮಾರ್ಗ);
    clear_string_option(&buf->b_p_ಟ್ಯಾಗುಗಳು);
    clear_string_option(&buf->b_p_tc);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    clear_string_option(&buf->b_p_tfu);
    free_callback(&buf->b_tfu_cb);
    clear_string_option(&buf->b_p_ffu);
    free_callback(&buf->b_ffu_cb);
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&buf->b_p_dict);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    clear_string_option(&buf->b_p_dia);
#ಆದರೆ_ಅಂತ್ಯ
    clear_string_option(&buf->b_p_tsr);
    clear_string_option(&buf->b_p_qe);
    buf->b_p_ac = -1;
    buf->b_p_ar = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FSYNC
    buf->b_p_fs = -1;
#ಆದರೆ_ಅಂತ್ಯ
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
    ಪೂರ್ಣಾಂಕ
buflist_getfile(
    ಪೂರ್ಣಾಂಕ		n,
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		options,
    ಪೂರ್ಣಾಂಕ		forceit)
{
    buf_T	*buf;
    win_T	*wp = ಖಾಲಿ;
    pos_T	*fpos;
    colnr_T	ಲಂಬಸಾಲು;

    buf = buflist_findnr(n);
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	ಆದರೆ ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_ಬಫರ್_nr_not_ಸಿಕ್ಕಿತು), n);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    ಆದರೆ (buf == curbuf)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (ಪಠ್ಯ_or_buf_locked())
	ಹಿಂತಿರುಗಿ FAIL;

    // altfpos may be changed by getfile(), get it now
    ಆದರೆ (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	ಲಂಬಸಾಲು = fpos->ಲಂಬಸಾಲು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಲಂಬಸಾಲು = 0;

    ಆದರೆ (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	ಆದರೆ (wp == ಖಾಲಿ && (swb_flags & (SWB_VSPLIT | SWB_ವಿಭಜಿಸು | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    ಆದರೆ (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (win_ವಿಭಜಿಸು(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    ಪೂರ್ಣಾಂಕ retval = FAIL;
    ಆದರೆ (GETFILE_SUCCESS(getfile(buf->b_fnum, ಖಾಲಿ, ಖಾಲಿ,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	ಆದರೆ (!p_sol && ಲಂಬಸಾಲು != 0)
	{
	    curwin->w_cursor.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
	    check_cursor_ಲಂಬಸಾಲು();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_ಗಣ_curswant = true;
	}
	retval = OK;
    }

    ಆದರೆ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ಹಿಂತಿರುಗಿ retval;
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

    ಆದರೆ (p_sol)
	curwin->w_cursor.ಲಂಬಸಾಲು = 0;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	curwin->w_cursor.ಲಂಬಸಾಲು = fpos->ಲಂಬಸಾಲು;
	check_cursor_ಲಂಬಸಾಲು();
	curwin->w_cursor.coladd = 0;
	curwin->w_ಗಣ_curswant = true;
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
    buf_T	*buf = ಖಾಲಿ;

    // First make the name into a full path name
    ffname = ತುಂಬಿದ_Name_save(fname,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#ಇಲ್ಲದಿದ್ದರೆ
	    FALSE
#ಆದರೆ_ಅಂತ್ಯ
	    );
    ಆದರೆ (ffname != ಖಾಲಿ)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    ಹಿಂತಿರುಗಿ buf;
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    stat_T	st;

    ಆದರೆ (mch_stat((ಅಕ್ಷರ *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    ಹಿಂತಿರುಗಿ buflist_findname_stat(ffname, &st);
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
#ಆದರೆ_ಅಂತ್ಯ
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_ಕೊನೆಯ(buf)
	ಆದರೆ ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
		    , stp
#ಆದರೆ_ಅಂತ್ಯ
		    ))
	    ಹಿಂತಿರುಗಿ buf;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    ಪೂರ್ಣಾಂಕ
buflist_findpat(
    char_u	*ನಮೂನೆ,
    char_u	*ನಮೂನೆ_ಕೊನೆ,	// pointer to first char after pattern
    ಪೂರ್ಣಾಂಕ		unlisted,	// find unlisted buffers
    ಪೂರ್ಣಾಂಕ		diffmode ಬಳಸದ, // find diff-mode buffers only
    ಪೂರ್ಣಾಂಕ		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		ಹೊಂದಿಕೆ = -1;
    ಪೂರ್ಣಾಂಕ		ಕಂಡುಹಿಡಿ_listed;
    char_u	*pat;
    char_u	*patend;
    ಪೂರ್ಣಾಂಕ		attempt;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    ಆದರೆ ((ನಮೂನೆ_ಕೊನೆ == ನಮೂನೆ + 1 && (*ನಮೂನೆ == '%' || *ನಮೂನೆ == '#'))
	    || (ಒಳ_vim9script() && ನಮೂನೆ_ಕೊನೆ == ನಮೂನೆ + 2
				    && ನಮೂನೆ[0] == '%' && ನಮೂನೆ[1] == '%'))
    {
	ಆದರೆ (*ನಮೂನೆ == '#' || ನಮೂನೆ_ಕೊನೆ == ನಮೂನೆ + 2)
	    ಹೊಂದಿಕೆ = curwin->w_alt_fnum;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೊಂದಿಕೆ = curbuf->b_fnum;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	ಆದರೆ (diffmode && !diff_ಬಹುಲಕ_buf(buflist_findnr(ಹೊಂದಿಕೆ)))
	    ಹೊಂದಿಕೆ = -1;
#ಆದರೆ_ಅಂತ್ಯ
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
    ಇಲ್ಲದಿದ್ದರೆ
    {
	pat = file_pat_ಗಮ್ಯ_reg_pat(ನಮೂನೆ, ನಮೂನೆ_ಕೊನೆ, ಖಾಲಿ, FALSE);
	ಆದರೆ (pat == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	ಕಂಡುಹಿಡಿ_listed = TRUE;
	ಗಾಗಿ (;;)
	{
	    ಗಾಗಿ (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		ಆದರೆ (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		ಆದರೆ (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_ಕೊನೆಯ(buf)
		{
		    ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			ಹಿಂತಿರುಗಿ -1;
		    }
		    ಆದರೆ (buf->b_p_bl == ಕಂಡುಹಿಡಿ_listed
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			    && (!diffmode || diff_ಬಹುಲಕ_buf(buf))
#ಆದರೆ_ಅಂತ್ಯ
			    && buflist_ಹೊಂದಿಕೆ(&regmatch, buf, FALSE) != ಖಾಲಿ)
		    {
			ಆದರೆ (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				ಆದರೆ (wp->w_ಬಫರ್ == buf)
				    ನಿಲ್ಲಿಸು;
			    ಆದರೆ (wp == ಖಾಲಿ)
				ಮುಂದುವರಿ;
			}
			ಆದರೆ (ಹೊಂದಿಕೆ >= 0)		// already found a match
			{
			    ಹೊಂದಿಕೆ = -2;
			    ನಿಲ್ಲಿಸು;
			}
			ಹೊಂದಿಕೆ = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		ಆದರೆ (ಹೊಂದಿಕೆ >= 0)			// found one match
		    ನಿಲ್ಲಿಸು;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    ಆದರೆ (!unlisted || !ಕಂಡುಹಿಡಿ_listed || ಹೊಂದಿಕೆ != -1)
		ನಿಲ್ಲಿಸು;
	    ಕಂಡುಹಿಡಿ_listed = FALSE;
	}

	vim_free(pat);
    }

    ಆದರೆ (ಹೊಂದಿಕೆ == -2)
	semsg(_(e_ಇನ್ನಷ್ಟು_than_one_ಹೊಂದಿಕೆ_for_str), ನಮೂನೆ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಹೊಂದಿಕೆ < 0)
	semsg(_(e_no_matching_ಬಫರ್_for_str), ನಮೂನೆ);
    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    buf_T   *buf;
    char_u  *ಹೊಂದಿಕೆ;
} bufmatch_T;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
ExpandBufnames(
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_file,
    char_u	***file,
    ಪೂರ್ಣಾಂಕ		options)
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		ಸುತ್ತು;
    char_u	*p;
    char_u	*patc = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    bufmatch_T	*matches = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		fuzzy;
    fuzmatch_str_T  *fuzmatch = ಖಾಲಿ;
    regmatch_T	regmatch;
    ಪೂರ್ಣಾಂಕ		score = 0;
    ಪೂರ್ಣಾಂಕ		ಗಮ್ಯ_free = FALSE;

    *ಅಂಕೆ_file = 0;		    // return values in case of FAIL
    *file = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    ಆದರೆ (!fuzzy)
    {
	ಆದರೆ (*pat == '^' && pat[1] != NUL)
	{
	    ಪೂರ್ಣಾಂಕ  len = (ಪೂರ್ಣಾಂಕ)STRLEN(pat);
	    patc = alloc(len);
	    ಆದರೆ (patc == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    ಗಮ್ಯ_free = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*pat == '^')
	    patc = (char_u *)"";
	ಇಲ್ಲದಿದ್ದರೆ
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    ಗಾಗಿ (ಸುತ್ತು = 1; ಸುತ್ತು <= 2; ++ಸುತ್ತು)
    {
	ಎಣಿಕೆ_2 = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    ಆದರೆ (!buf->b_p_bl)	// skip unlisted buffers
		ಮುಂದುವರಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
	    ಆದರೆ (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		ಆದರೆ (buf == curbuf || !diff_ಬಹುಲಕ_buf(buf))
		    ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (!fuzzy)
	    {
		ಆದರೆ (regmatch.regprog == ಖಾಲಿ)
		{
		    // invalid pattern, possibly after recompiling
		    ಆದರೆ (ಗಮ್ಯ_free)
			vim_free(patc);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		p = buflist_ಹೊಂದಿಕೆ(&regmatch, buf, p_wic);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		p = ಖಾಲಿ;
		// first try matching with the short file name
		ಆದರೆ ((score = fuzzy_ಹೊಂದಿಕೆ_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		ಆದರೆ (p == ಖಾಲಿ)
		{
		    // next try matching with the full path file name
		    ಆದರೆ ((score = fuzzy_ಹೊಂದಿಕೆ_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    ಆದರೆ (p == ಖಾಲಿ)
		ಮುಂದುವರಿ;

	    ಆದರೆ (ಸುತ್ತು == 1)
	    {
		++ಎಣಿಕೆ_2;
		ಮುಂದುವರಿ;
	    }

	    ಆದರೆ (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    ಇಲ್ಲದಿದ್ದರೆ
		p = vim_strsave(p);
	    ಆದರೆ (p == ಖಾಲಿ)
	    {
		ಆದರೆ (fuzzy && ಸುತ್ತು == 2)
		    fuzmatch_str_free(fuzmatch, ಎಣಿಕೆ_2);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    ಆದರೆ (!fuzzy)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
		ಆದರೆ (matches != ಖಾಲಿ)
		{
		    matches[ಎಣಿಕೆ_2].buf = buf;
		    matches[ಎಣಿಕೆ_2].ಹೊಂದಿಕೆ = p;
		    ಎಣಿಕೆ_2++;
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    (*file)[ಎಣಿಕೆ_2++] = p;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		fuzmatch[ಎಣಿಕೆ_2].idx = ಎಣಿಕೆ_2;
		fuzmatch[ಎಣಿಕೆ_2].str = p;
		fuzmatch[ಎಣಿಕೆ_2].score = score;
		ಎಣಿಕೆ_2++;
	    }
	}
	ಆದರೆ (ಎಣಿಕೆ_2 == 0)	// no match found, break here
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (ಸುತ್ತು == 1)
	{
	    ಆದರೆ (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, ಎಣಿಕೆ_2);
		ಆದರೆ (*file == ಖಾಲಿ)
		{
		    vim_regfree(regmatch.regprog);
		    ಆದರೆ (ಗಮ್ಯ_free)
			vim_free(patc);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
		ಆದರೆ (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, ಎಣಿಕೆ_2);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, ಎಣಿಕೆ_2);
		ಆದರೆ (fuzmatch == ಖಾಲಿ)
		{
		    *ಅಂಕೆ_file = 0;
		    *file = ಖಾಲಿ;
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	}
    }

    ಆದರೆ (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	ಆದರೆ (ಗಮ್ಯ_free)
	    vim_free(patc);
    }

    ಆದರೆ (!fuzzy)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
	ಆದರೆ (matches != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ i;
	    ಆದರೆ (ಎಣಿಕೆ_2 > 1)
		qsort(matches, ಎಣಿಕೆ_2, ಗಾತ್ರ(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    ಆದರೆ (matches[0].buf == curbuf)
	    {
		ಗಾಗಿ (i = 1; i < ಎಣಿಕೆ_2; i++)
		    (*file)[i-1] = matches[i].ಹೊಂದಿಕೆ;
		(*file)[ಎಣಿಕೆ_2-1] = matches[0].ಹೊಂದಿಕೆ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; i++)
		    (*file)[i] = matches[i].ಹೊಂದಿಕೆ;
	    }
	    vim_free(matches);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (fuzzymatches_ಗಮ್ಯ_strmatches(fuzmatch, file, ಎಣಿಕೆ_2, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    *ಅಂಕೆ_file = ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ (ಎಣಿಕೆ_2 == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    ಸ್ಥಿರ char_u *
buflist_ಹೊಂದಿಕೆ(
    regmatch_T	*rmp,
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*ಹೊಂದಿಕೆ;

    // First try the short file name, then the long file name.
    ಹೊಂದಿಕೆ = fname_ಹೊಂದಿಕೆ(rmp, buf->b_sfname, ignore_case);
    ಆದರೆ (ಹೊಂದಿಕೆ == ಖಾಲಿ && rmp->regprog != ಖಾಲಿ)
	ಹೊಂದಿಕೆ = fname_ಹೊಂದಿಕೆ(rmp, buf->b_ffname, ignore_case);

    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    ಸ್ಥಿರ char_u *
fname_ಹೊಂದಿಕೆ(
    regmatch_T	*rmp,
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*ಹೊಂದಿಕೆ = ಖಾಲಿ;
    char_u	*p;

    // extra check for valid arguments
    ಆದರೆ (name == ಖಾಲಿ || rmp->regprog == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    ಆದರೆ (vim_regexec(rmp, name, (colnr_T)0))
	ಹೊಂದಿಕೆ = name;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rmp->regprog != ಖಾಲಿ)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(ಖಾಲಿ, name);
	ಆದರೆ (p != ಖಾಲಿ && vim_regexec(rmp, p, (colnr_T)0))
	    ಹೊಂದಿಕೆ = name;
	vim_free(p);
    }

    ಹಿಂತಿರುಗಿ ಹೊಂದಿಕೆ;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(ಪೂರ್ಣಾಂಕ nr)
{
    char_u	ಕೀಲಿ[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    ಆದರೆ (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((ಅಕ್ಷರ *)ಕೀಲಿ, ಗಾತ್ರ(ಕೀಲಿ), "%x", nr);
    hi = hash_ಕಂಡುಹಿಡಿ(&buf_hashtab, ಕೀಲಿ);

    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
	ಹಿಂತಿರುಗಿ (buf_T *)(hi->hi_ಕೀಲಿ
			     - ((ಚಿಹ್ನೆರಹಿತ)(curbuf->b_ಕೀಲಿ - (char_u *)curbuf)));
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    ಪೂರ್ಣಾಂಕ		n,
    ಪೂರ್ಣಾಂಕ		fullname,
    ಪೂರ್ಣಾಂಕ		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ home_replace_save(helptail ? buf : ಖಾಲಿ,
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
    colnr_T	ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ಆದರೆ (wip->wi_win == win)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (wip == ಖಾಲಿ)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	ಆದರೆ (wip == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	wip->wi_win = win;
	ಆದರೆ (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// remove the entry from the list
	ಆದರೆ (wip->wi_prev)
	    wip->wi_prev->wi_ಮುಂದಿನ = wip->wi_ಮುಂದಿನ;
	ಇಲ್ಲದಿದ್ದರೆ
	    buf->b_wininfo = wip->wi_ಮುಂದಿನ;
	ಆದರೆ (wip->wi_ಮುಂದಿನ)
	    wip->wi_ಮುಂದಿನ->wi_prev = wip->wi_prev;
	ಆದರೆ (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }
    ಆದರೆ (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    }
    ಆದರೆ (win != ಖಾಲಿ)
	wip->wi_changelistidx = win->w_changelistidx;
    ಆದರೆ (copy_options && win != ಖಾಲಿ)
    {
	// Save the window-specific option values.
	copy_winopt(&win->w_onebuf_opt, &wip->wi_opt);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	wip->wi_fold_manual = win->w_fold_manual;
	cloneFoldGrowArray(&win->w_folds, &wip->wi_folds);
#ಆದರೆ_ಅಂತ್ಯ
	wip->wi_optset = TRUE;
    }

    // insert the entry in front of the list
    wip->wi_ಮುಂದಿನ = buf->b_wininfo;
    buf->b_wininfo = wip;
    wip->wi_prev = ಖಾಲಿ;
    ಆದರೆ (wip->wi_ಮುಂದಿನ)
	wip->wi_ಮುಂದಿನ->wi_prev = wip;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    ಆದರೆ (!wip->wi_opt.wo_diff)
	ಹಿಂತಿರುಗಿ FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	ಆದರೆ (wip->wi_win == wp)
	    ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Find info for the current window in buffer "buf".
 * If not found, return the info for the most recently used window.
 * When "need_options" is TRUE skip entries where wi_optset is FALSE.
 * When "skip_diff_buffer" is TRUE avoid windows with 'diff' set that is in
 * another tab page.
 * Returns NULL when there isn't any info.
 */
    ಸ್ಥಿರ wininfo_T *
ಕಂಡುಹಿಡಿ_wininfo(
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		need_options,
    ಪೂರ್ಣಾಂಕ		skip_diff_ಬಫರ್ ಬಳಸದ)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ಆದರೆ (wip->wi_win == curwin
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		&& (!skip_diff_ಬಫರ್ || !wininfo_other_tab_diff(wip))
#ಆದರೆ_ಅಂತ್ಯ

		&& (!need_options || wip->wi_optset))
	    ನಿಲ್ಲಿಸು;

    ಆದರೆ (wip != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    ಆದರೆ (skip_diff_ಬಫರ್)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    ಆದರೆ (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != ಖಾಲಿ
			    && wip->wi_win->w_ಬಫರ್ == buf)))
		ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	wip = buf->b_wininfo;
    ಹಿಂತಿರುಗಿ wip;
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    clearFolding(curwin);
#ಆದರೆ_ಅಂತ್ಯ

    wip = ಕಂಡುಹಿಡಿ_wininfo(buf, TRUE, TRUE);
    ಆದರೆ (wip != ಖಾಲಿ && wip->wi_win != ಖಾಲಿ
	    && wip->wi_win != curwin && wip->wi_win->w_ಬಫರ್ == buf)
    {
	// The buffer is currently displayed in the window: use the actual
	// option values instead of the saved (possibly outdated) values.
	win_T *wp = wip->wi_win;

	copy_winopt(&wp->w_onebuf_opt, &curwin->w_onebuf_opt);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
	// w_stl_rendered_height is not in winvar_T; copy it explicitly.
	curwin->w_stl_rendered_ಎತ್ತರ = wp->w_stl_rendered_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	curwin->w_fold_manual = wp->w_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wp->w_folds, &curwin->w_folds);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wip != ಖಾಲಿ && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    ಆದರೆ (wip != ಖಾಲಿ)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    ಆದರೆ (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    ಆದರೆ (*curwin->w_p_stl != NUL)
	update_stl_rendered_ಎತ್ತರ(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    ಆದರೆ (curwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ > 0
	    && curwin->w_ಸ್ಥಿತಿ_ಎತ್ತರ != statusline_ಎತ್ತರ(curwin))
    {
	ಚೌಕಟ್ಟು_change_statusline_ಎತ್ತರ();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	ಆದರೆ (p_ea)
	    win_equal(curwin, TRUE, 'v');
    }
#ಆದರೆ_ಅಂತ್ಯ
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

    wip = ಕಂಡುಹಿಡಿ_wininfo(buf, FALSE, FALSE);
    ಆದರೆ (wip != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &(wip->wi_fpos);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    ಶೂನ್ಯ
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		ro_char;
    ಪೂರ್ಣಾಂಕ		ಬದಲಾಯಿತು_char;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪೂರ್ಣಾಂಕ		ಕೆಲಸ_running;
    ಪೂರ್ಣಾಂಕ		ಕೆಲಸ_none_open;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_ದತ್ತಾಂಶ = ಖಾಲಿ, **p;

    ಆದರೆ (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, ಗಾತ್ರ(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    ಆದರೆ (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_ದತ್ತಾಂಶ)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_ದತ್ತಾಂಶ, (size_t)buflist.ga_len,
		ಗಾತ್ರ(buf_T *), buf_compare);

	buflist_ದತ್ತಾಂಶ = (buf_T **)buflist.ga_ದತ್ತಾಂಶ;
	buf = *buflist_ದತ್ತಾಂಶ;
    }
    p = buflist_ದತ್ತಾಂಶ;

    ಗಾಗಿ (; buf != ಖಾಲಿ && !got_int; buf = buflist_ದತ್ತಾಂಶ != ಖಾಲಿ
	    ? (++p < buflist_ದತ್ತಾಂಶ + buflist.ga_len ? *p : ಖಾಲಿ)
	    : buf->b_ಮುಂದಿನ)
#ಇಲ್ಲದಿದ್ದರೆ
    ಗಾಗಿ (buf = firstbuf; buf != ಖಾಲಿ && !got_int; buf = buf->b_ಮುಂದಿನ)
#ಆದರೆ_ಅಂತ್ಯ
    {
	char_u	*name;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	ಕೆಲಸ_running = ಪದ_ಕೆಲಸ_running(buf->b_ಪದ);
	ಕೆಲಸ_none_open = ಪದ_none_open(buf->b_ಪದ);
#ಆದರೆ_ಅಂತ್ಯ
	// skip unlisted buffers, unless ! was used
	ಆದರೆ ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !buf_Is_ಬದಲಾಯಿತು(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == ಖಾಲಿ || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == ಖಾಲಿ || buf->b_nwindows != 0))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!ಕೆಲಸ_running || (ಕೆಲಸ_running && ಕೆಲಸ_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!ಕೆಲಸ_running || (ಕೆಲಸ_running && !ಕೆಲಸ_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (ಕೆಲಸ_running || buf->b_ಪದ == ಖಾಲಿ))
#ಆದರೆ_ಅಂತ್ಯ
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    ಮುಂದುವರಿ;
	name = buf_spname(buf);
	ಆದರೆ (name != ಖಾಲಿ)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	ಇಲ್ಲದಿದ್ದರೆ
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	ಆದರೆ (ಸಂದೇಶ_filtered(NameBuff))
	    ಮುಂದುವರಿ;

	ಬದಲಾಯಿತು_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (buf_Is_ಬದಲಾಯಿತು(buf) ? '+' : ' ');
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	ಆದರೆ (ಕೆಲಸ_running)
	{
	    ಆದರೆ (ಕೆಲಸ_none_open)
		ro_char = '?';
	    ಇಲ್ಲದಿದ್ದರೆ
		ro_char = 'R';
	    ಬದಲಾಯಿತು_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buf->b_ಪದ != ಖಾಲಿ)
	    ro_char = 'F';
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	ಸಂದೇಶ_putchar('\n');
	len = (ಪೂರ್ಣಾಂಕ)vim_snprintf_safelen((ಅಕ್ಷರ *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == ಖಾಲಿ ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		ಬದಲಾಯಿತು_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	ಮಾಡು
	    IObuff[len++] = ' ';
	ಆಗ (--i > 0 && len < IOSIZE - 18);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
	ಆದರೆ (vim_strchr(eap->arg, 't') && buf->b_ಕೊನೆಯ_ಬಳಸಿದ)
	    ಕೂಡಿಸು_ಸಮಯ(IObuff + len, (size_t)(IOSIZE - len), buf->b_ಕೊನೆಯ_ಬಳಸಿದ);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    vim_snprintf((ಅಕ್ಷರ *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (ದೀರ್ಘ)buflist_findlnum(buf));
	ಸಂದೇಶ_outtrans(IObuff);
	ಹೊರಹೋಗು_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
    ಆದರೆ (buflist_ದತ್ತಾಂಶ)
	ga_clear(&buflist);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    ಪೂರ್ಣಾಂಕ
buflist_name_nr(
    ಪೂರ್ಣಾಂಕ		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ಆದರೆ (buf == ಖಾಲಿ || buf->b_fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    ಪೂರ್ಣಾಂಕ		ಸಂದೇಶ_2)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    stat_T	st;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ffname == ಖಾಲಿ || *ffname == NUL)
    {
	// Removing the name.
	ಆದರೆ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ಇಲ್ಲದಿದ್ದರೆ
	    buf->b_sfname = ಖಾಲಿ;
	VIM_CLEAR(buf->b_ffname);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	st.st_dev = (dev_T)-1;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	ಆದರೆ (ffname == ಖಾಲಿ)		    // out of memory
	    ಹಿಂತಿರುಗಿ FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	ಆದರೆ (mch_stat((ಅಕ್ಷರ *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (!(buf->b_flags & BF_DUMMY))
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#ಇಲ್ಲದಿದ್ದರೆ
	    obuf = buflist_findname(ffname);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (obuf != ಖಾಲಿ && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    ಪೂರ್ಣಾಂಕ		ಒಳ_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		ಆದರೆ (win->w_ಬಫರ್ == obuf)
		    ಒಳ_use = TRUE;

	    // it's loaded or used in a window, fail
	    ಆದರೆ (obuf->b_ml.ml_mfp != ಖಾಲಿ || ಒಳ_use)
	    {
		ಆದರೆ (ಸಂದೇಶ_2)
		    emsg(_(e_ಬಫರ್_with_this_name_already_exists));
		vim_free(ffname);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    // delete from the list
	    close_ಬಫರ್(ಖಾಲಿ, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	ಆದರೆ (ffname == ಖಾಲಿ || sfname == ಖಾಲಿ)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಆದರೆ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#ಆದರೆ_ಅಂತ್ಯ

    buf->b_shortname = false;

    buf_name_ಬದಲಾಯಿತು(buf);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    ಶೂನ್ಯ
buf_ಗಣ_name(ಪೂರ್ಣಾಂಕ fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = ಖಾಲಿ;
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
buf_name_ಬದಲಾಯಿತು(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    ಆದರೆ (buf->b_ml.ml_mfp != ಖಾಲಿ)
	ml_setname(buf);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಆದರೆ (buf->b_ಪದ != ಖಾಲಿ)
	ಪದ_clear_ಸ್ಥಿತಿ_ಪಠ್ಯ(buf->b_ಪದ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (curwin->w_ಬಫರ್ == buf)
	check_arg_idx(curwin);	// check file name for arg list
    maketitle();		// set window title
    ಸ್ಥಿತಿ_redraw_all();	// status lines need to be redrawn
    fmarks_check_ಹೆಸರುಗಳು(buf);	// check named file marks
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
    ಆದರೆ (buf != ಖಾಲಿ && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    ಪೂರ್ಣಾಂಕ		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    ಆದರೆ (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	ಆದರೆ (errmsg)
	    emsg(_(e_no_alternate_file));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    ಪೂರ್ಣಾಂಕ
buflist_ಕೂಡಿಸು(char_u *fname, ಪೂರ್ಣಾಂಕ flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, ಖಾಲಿ, (linenr_T)0, flags);
    ಆದರೆ (buf != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ buf->b_fnum;
    ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ defined(BACKSLASH_ಒಳ_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    ಶೂನ್ಯ
buflist_slash_adjust(ಶೂನ್ಯ)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	ಆದರೆ (bp->b_ffname != ಖಾಲಿ)
	    slash_adjust(bp->b_ffname);
	ಆದರೆ (bp->b_sfname != ಖಾಲಿ)
	    slash_adjust(bp->b_sfname);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    ಶೂನ್ಯ
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.ಲಂಬಸಾಲು, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    ಪೂರ್ಣಾಂಕ
otherfile(char_u *ffname)
{
    ಹಿಂತಿರುಗಿ otherfile_buf(curbuf, ffname
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
	    , ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
	    );
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    , stat_T		*stp
#ಆದರೆ_ಅಂತ್ಯ
    )
{
    // no name is different
    ಆದರೆ (ffname == ಖಾಲಿ || *ffname == NUL || buf->b_ffname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (fnamecmp(ffname, buf->b_ffname) == 0)
	ಹಿಂತಿರುಗಿ FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	ಆದರೆ (stp == ಖಾಲಿ)
	{
	    ಆದರೆ (!buf->b_dev_valid || mch_stat((ಅಕ್ಷರ *)ffname, &st) < 0)
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
	ಆದರೆ (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    ಆದರೆ (buf_same_ino(buf, stp))
		ಹಿಂತಿರುಗಿ FALSE;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    ಶೂನ್ಯ
buf_setino(buf_T *buf)
{
    stat_T	st;

    ಆದರೆ (buf->b_fname != ಖಾಲಿ && mch_stat((ಅಕ್ಷರ *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    ಹಿಂತಿರುಗಿ (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Print info about the current buffer.
 */
    ಶೂನ್ಯ
fileinfo(
    ಪೂರ್ಣಾಂಕ fullname,	    // when non-zero print full path
    ಪೂರ್ಣಾಂಕ shorthelp,
    ಪೂರ್ಣಾಂಕ	dont_truncate)
{
    char_u	*name;
    ಅಕ್ಷರ	*ಬಫರ್;
    size_t	bufferlen = 0;

    ಬಫರ್ = alloc(IOSIZE);
    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(ಬಫರ್, IOSIZE, "buf %d: ", curbuf->b_fnum);

    ಬಫರ್[bufferlen++] = '"';

    name = buf_spname(curbuf);
    ಆದರೆ (name != ಖಾಲಿ)
	bufferlen += vim_snprintf_safelen(ಬಫರ್ + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!fullname && curbuf->b_fname != ಖಾಲಿ)
	    name = curbuf->b_fname;
	ಇಲ್ಲದಿದ್ದರೆ
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : ಖಾಲಿ, name,
	    (char_u *)ಬಫರ್ + bufferlen, IOSIZE - (ಪೂರ್ಣಾಂಕ)bufferlen, TRUE);
    }

    bufferlen += vim_snprintf_safelen(
	ಬಫರ್ + bufferlen,
	IOSIZE - bufferlen,
	"\"%s%s%s%s%s%s",
	curbuf_Is_ಬದಲಾಯಿತು() ? (shortmess(SHM_MOD)
	    ?  " [+]" : _(" [Modified]")) : " ",
	(curbuf->b_flags & BF_NOTEDITED) && !bt_dontwrite(curbuf)
	    ? _("[Not edited]") : "",
	(curbuf->b_flags & BF_NEW) && !bt_dontwrite(curbuf)
	    ? new_file_ಸಂದೇಶ() : "",
	(curbuf->b_flags & BF_READERR) ? _("[Read errors]") : "", curbuf->b_p_ro
	    ? (shortmess(SHM_RO) ? _("[RO]") : _("[readonly]")) : "",
	(curbuf_Is_ಬದಲಾಯಿತು() || (curbuf->b_flags & BF_WRITE_ಮುಖವಾಡ) || curbuf->b_p_ro)
	    ? " " : "");

    ಆದರೆ (curbuf->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
	bufferlen += vim_snprintf_safelen(ಬಫರ್ + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_ಸಾಲುಗಳು_ಸಂದೇಶ));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    ಬಫರ್ + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ),
	    (ದೀರ್ಘ)curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	bufferlen += vim_snprintf_safelen(
	    ಬಫರ್ + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (ದೀರ್ಘ)curwin->w_cursor.lnum,
	    (ದೀರ್ಘ)curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ));

	validate_virtcol();
	bufferlen += ಲಂಬಸಾಲು_print((char_u *)ಬಫರ್ + bufferlen, IOSIZE - bufferlen,
		   (ಪೂರ್ಣಾಂಕ)curwin->w_cursor.ಲಂಬಸಾಲು + 1, (ಪೂರ್ಣಾಂಕ)curwin->w_virtcol + 1);
    }

    (ಶೂನ್ಯ)append_arg_ಸಂಖ್ಯೆ(curwin, (char_u *)ಬಫರ್ + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    ಆದರೆ (dont_truncate)
    {
	ಪೂರ್ಣಾಂಕ	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	ಸಂದೇಶ_ಆರಂಭ();
	n = ಸಂದೇಶ_scroll;
	ಸಂದೇಶ_scroll = TRUE;
	ಸಂದೇಶ(ಬಫರ್);
	ಸಂದೇಶ_scroll = n;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಕ್ಷರ	*p = ಸಂದೇಶ_trunc_attr(ಬಫರ್, FALSE, 0);
	ಆದರೆ (restart_edit != 0 || (ಸಂದೇಶ_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    ಗಣ_keep_ಸಂದೇಶ((char_u *)p, 0);
    }

    vim_free(ಬಫರ್);
}

    ಪೂರ್ಣಾಂಕ
ಲಂಬಸಾಲು_print(
    char_u  *buf,
    size_t  buflen,
    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು,
    ಪೂರ್ಣಾಂಕ	    vcol)
{
    ಆದರೆ (ಲಂಬಸಾಲು == vcol)
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)vim_snprintf_safelen((ಅಕ್ಷರ *)buf, buflen, "%d", ಲಂಬಸಾಲು);

    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)vim_snprintf_safelen((ಅಕ್ಷರ *)buf, buflen, "%d-%d", ಲಂಬಸಾಲು, vcol);
}

ಸ್ಥಿರ char_u *lasttitle = ಖಾಲಿ;
ಸ್ಥಿರ char_u *lasticon = ಖಾಲಿ;

/*
 * Put the file name in the title bar and icon of the window.
 */
    ಶೂನ್ಯ
maketitle(ಶೂನ್ಯ)
{
    char_u	*title_str = ಖಾಲಿ;
    char_u	*icon_str = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    ಆದರೆ (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	ಹಿಂತಿರುಗಿ;
    }

    need_maketitle = FALSE;
    ಆದರೆ (!p_title && !p_icon && lasttitle == ಖಾಲಿ && lasticon == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // nothing to do

    ಆದರೆ (p_title)
    {
	ಪೂರ್ಣಾಂಕ maxlen = 0;

	ಆದರೆ (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    ಆದರೆ (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	ಆದರೆ (*p_titlestring != NUL)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
	    ಆದರೆ (stl_syntax & STL_ಒಳ_TITLE)
		build_stl_str_hl(curwin, title_str, ಗಾತ್ರ(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#ವ್ಯಾಖ್ಯಾನಿಸು SPACE_FOR_FNAME (ಗಾತ್ರ(buf) - 100)
#ವ್ಯಾಖ್ಯಾನಿಸು SPACE_FOR_DIR   (ಗಾತ್ರ(buf) - 20)
#ವ್ಯಾಖ್ಯಾನಿಸು SPACE_FOR_ARGNR (ಗಾತ್ರ(buf) - 10)  // at least room for " - VIM"

	    // file name
	    ಆದರೆ (curbuf->b_fname == ಖಾಲಿ)
		buflen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curbuf->b_ಪದ != ಖಾಲಿ)
		buflen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ಪದ_get_ಸ್ಥಿತಿ_ಪಠ್ಯ(curbuf->b_ಪದ));
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		buflen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != ಖಾಲಿ)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    ಆದರೆ (curbuf->b_ಪದ == ಖಾಲಿ)
#ಆದರೆ_ಅಂತ್ಯ
	    {
		ಆಯ್ಕೆ (buf_Is_ಬದಲಾಯಿತು(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    ಸ್ಥಿತಿ 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (ಅಕ್ಷರ *)buf + buflen,
			    ಗಾತ್ರ(buf) - buflen, " +");
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (ಅಕ್ಷರ *)buf + buflen,
			    ಗಾತ್ರ(buf) - buflen, " =");
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (ಅಕ್ಷರ *)buf + buflen,
			    ಗಾತ್ರ(buf) - buflen, " =+");
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 4:
		    ಸ್ಥಿತಿ 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (ಅಕ್ಷರ *)buf + buflen,
			    ಗಾತ್ರ(buf) - buflen, " -");
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ 5:
		    ಸ್ಥಿತಿ 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (ಅಕ್ಷರ *)buf + buflen,
			    ಗಾತ್ರ(buf) - buflen, " -+");
			ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
	    }

	    // path (surrounded by '()')
	    ಆದರೆ (curbuf->b_fname != ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		    && curbuf->b_ಪದ == ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((ಅಕ್ಷರ *)buf + buflen,
		    ಗಾತ್ರ(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (ಪೂರ್ಣಾಂಕ)(SPACE_FOR_DIR - buflen), TRUE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ BACKSLASH_ಒಳ_FILENAME
		// avoid "c:/name" to be reduced to "c"
		ಆದರೆ (ಸುರಕ್ಷಿತ_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#ಆದರೆ_ಅಂತ್ಯ

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		ಆದರೆ (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((ಅಕ್ಷರ *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    ಆದರೆ (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((ಅಕ್ಷರ *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != ಖಾಲಿ)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			buflen += vim_snprintf_safelen((ಅಕ್ಷರ *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_ಸಂಖ್ಯೆ(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((ಅಕ್ಷರ *)buf + buflen,
		ಗಾತ್ರ(buf) - buflen, " - %s",
#ಆದರೆ defined(FEAT_CLIENTSERVER)
		(serverName != ಖಾಲಿ)
		    ? serverName :
#ಆದರೆ_ಅಂತ್ಯ
		    (char_u *)"VIM");

	    ಆದರೆ (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		ಆದರೆ (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, ಗಾತ್ರ(buf));
	    }
	}
    }
    mustset = ಮೌಲ್ಯ_ಬದಲಾಯಿತು(title_str, &lasttitle);

    ಆದರೆ (p_icon)
    {
	icon_str = buf;
	ಆದರೆ (*p_iconstring != NUL)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_STL_OPT
	    ಆದರೆ (stl_syntax & STL_ಒಳ_ICON)
		build_stl_str_hl(curwin, icon_str, ಗಾತ್ರ(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, ಖಾಲಿ, ಖಾಲಿ,
				 ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		icon_str = p_iconstring;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u  *name;
	    ಪೂರ್ಣಾಂಕ	    namelen;

	    name = buf_spname(curbuf);
	    ಆದರೆ (name == ಖಾಲಿ)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (ಪೂರ್ಣಾಂಕ)STRLEN(name);
	    ಆದರೆ (namelen > 100)
	    {
		namelen -= 100;
		ಆದರೆ (has_mbyte)
		    namelen += (*mb_ಬಾಲ_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, ಗಾತ್ರ(buf));
	}
    }

    mustset |= ಮೌಲ್ಯ_ಬದಲಾಯಿತು(icon_str, &lasticon);

    ಆದರೆ (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಮೌಲ್ಯ_ಬದಲಾಯಿತು(char_u *str, char_u **ಕೊನೆಯ)
{
    ಆದರೆ ((str == ಖಾಲಿ) != (*ಕೊನೆಯ == ಖಾಲಿ)
	    || (str != ಖಾಲಿ && *ಕೊನೆಯ != ಖಾಲಿ && STRCMP(str, *ಕೊನೆಯ) != 0))
    {
	vim_free(*ಕೊನೆಯ);
	ಆದರೆ (str == ಖಾಲಿ)
	{
	    *ಕೊನೆಯ = ಖಾಲಿ;
	    mch_restore_title(
		  ಕೊನೆಯ == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    *ಕೊನೆಯ = vim_strsave(str);
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    ಶೂನ್ಯ
resettitle(ಶೂನ್ಯ)
{
    mch_settitle(lasttitle, lasticon);
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
free_titles(ಶೂನ್ಯ)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u	*stl_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		stl_minwid;
    ಪೂರ್ಣಾಂಕ		stl_maxwid;
    ಎಣಿಕೆ {
	ಸಾಮಾನ್ಯ,
	ಖಾಲಿಗಳು,
	ಗುಂಪು_2,
	Separate,
	Highlight,
	Tab_ಪುಟ,
	ClickFunc,
	Trunc
    }		stl_type;
    char_u	*stl_clickfunc;	// function name for ClickFunc items
} stl_ಅಂಶ_T;

ಸ್ಥಿರ size_t		stl_ವಸ್ತುಗಳು_len = 20; // Initial value, grows as needed.
ಸ್ಥಿರ stl_ಅಂಶ_T      *stl_ವಸ್ತುಗಳು = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	       *stl_groupitem = ಖಾಲಿ;
ಸ್ಥಿರ stl_hlrec_T     *stl_hltab = ಖಾಲಿ;
ಸ್ಥಿರ stl_hlrec_T     *stl_tabtab = ಖಾಲಿ;
ಸ್ಥಿರ stl_clickrec_T  *stl_clicktab = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		*stl_separator_locations = ಖಾಲಿ;

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
    ಪೂರ್ಣಾಂಕ
build_stl_str_hl(
    win_T	*wp,
    char_u	*ಹೊರಹೋಗು,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ಪೂರ್ಣಾಂಕ		opt_scope,	// scope for "opt_name"
    ಪೂರ್ಣಾಂಕ		fillchar,
    ಪೂರ್ಣಾಂಕ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    ಹಿಂತಿರುಗಿ build_stl_str_hl_local(STL_ಬಹುಲಕ_SINGLE, wp, ಹೊರಹೋಗು, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ಖಾಲಿ, ಖಾಲಿ);
}

    ಪೂರ್ಣಾಂಕ
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*ಹೊರಹೋಗು,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    ಪೂರ್ಣಾಂಕ		opt_scope,	// scope for "opt_name"
    ಪೂರ್ಣಾಂಕ		fillchar,
    ಪೂರ್ಣಾಂಕ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ಪೂರ್ಣಾಂಕ		*ವಾಹಕ_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ಹಿಂತಿರುಗಿ build_stl_str_hl_local(STL_ಬಹುಲಕ_MULTI, wp, ಹೊರಹೋಗು, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ಖಾಲಿ, ವಾಹಕ_hl);
}

# ifdef ENABLE_STL_ಬಹುಲಕ_MULTI_NL
    ಪೂರ್ಣಾಂಕ
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*ಹೊರಹೋಗು,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    ಪೂರ್ಣಾಂಕ		opt_scope,	// scope for "opt_name"
    ಪೂರ್ಣಾಂಕ		fillchar,
    ಪೂರ್ಣಾಂಕ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ಪೂರ್ಣಾಂಕ		*ವಾಹಕ_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ಹಿಂತಿರುಗಿ build_stl_str_hl_local(STL_ಬಹುಲಕ_MULTI_NL, wp, ಹೊರಹೋಗು, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ಖಾಲಿ, ವಾಹಕ_hl);
}
# endif

    ಪೂರ್ಣಾಂಕ
get_stl_rendered_ಎತ್ತರ(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ಪೂರ್ಣಾಂಕ		opt_scope)	// scope for "opt_name"
{
    ಪೂರ್ಣಾಂಕ rendered_ಎತ್ತರ = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (ಶೂನ್ಯ)build_stl_str_hl_local(STL_ಬಹುಲಕ_GET_RENDERED_ಎತ್ತರ,
	    wp, buf, ಗಾತ್ರ(buf), &fmt,
	    opt_name, opt_scope, 0, 0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, &rendered_ಎತ್ತರ,
	    ಖಾಲಿ);
    --emsg_off;
    ಹಿಂತಿರುಗಿ rendered_ಎತ್ತರ;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
build_stl_str_hl_local(
    stl_ಬಹುಲಕ_T	ಬಹುಲಕ,
    win_T	*wp,
    char_u	*ಹೊರಹೋಗು,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ಪೂರ್ಣಾಂಕ		opt_scope,	// scope for "opt_name"
    ಪೂರ್ಣಾಂಕ		fillchar,
    ಪೂರ್ಣಾಂಕ		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ಪೂರ್ಣಾಂಕ		*rendered_ಎತ್ತರ,   // return: stl rendered height (can be NULL)
    ಪೂರ್ಣಾಂಕ		*ವಾಹಕ_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    ಪೂರ್ಣಾಂಕ		byteval;
# ifdef FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		use_sandbox;
    ಪೂರ್ಣಾಂಕ		save_VIsual_ಸಕ್ರಿಯ;
# endif
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_ಸಾಲು;
    ದೀರ್ಘ	l;
    ದೀರ್ಘ	n;
    ಪೂರ್ಣಾಂಕ		prevchar_isflag;
    ಪೂರ್ಣಾಂಕ		prevchar_isitem;
    ಪೂರ್ಣಾಂಕ		itemisflag;
    ಪೂರ್ಣಾಂಕ		fillable;
    char_u	*str;
    ದೀರ್ಘ	ಅಂಕೆ;
    ಪೂರ್ಣಾಂಕ		ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		itemcnt;
    ಪೂರ್ಣಾಂಕ		curitem;
    ಪೂರ್ಣಾಂಕ		groupdepth;
# ifdef FEAT_EVAL
    ಪೂರ್ಣಾಂಕ		evaldepth;
# endif
    ಪೂರ್ಣಾಂಕ		minwid;
    ಪೂರ್ಣಾಂಕ		maxwid;
    ಪೂರ್ಣಾಂಕ		zeropad;
    char_u	ತಳ;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    ಪೂರ್ಣಾಂಕ		save_redraw_not_allowed = redraw_not_allowed;
    ಪೂರ್ಣಾಂಕ		save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
    ಪೂರ್ಣಾಂಕ		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    ಆದರೆ (updating_screen)
	redraw_not_allowed = TRUE;

    ಆದರೆ (stl_ವಸ್ತುಗಳು == ಖಾಲಿ)
    {
	stl_ವಸ್ತುಗಳು = ALLOC_MULT(stl_ಅಂಶ_T, stl_ವಸ್ತುಗಳು_len);
	stl_groupitem = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, stl_ವಸ್ತುಗಳು_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_ವಸ್ತುಗಳು_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_ವಸ್ತುಗಳು_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_ವಸ್ತುಗಳು_len + 1);

	stl_separator_locations = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, stl_ವಸ್ತುಗಳು_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_ಗಣ_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    ಆದರೆ (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_ಸಂಖ್ಯೆ;
	tv.vval.v_ಸಂಖ್ಯೆ = wp->w_id;
	ಗಣ_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_ಗಮ್ಯ_string_ಸುರಕ್ಷಿತ(fmt + 2, use_sandbox, FALSE, FALSE);
	ಆದರೆ (usefmt == ಖಾಲಿ)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    ಆದರೆ (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    ಆದರೆ (lnum > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
    {
	lnum = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_ಬಫರ್, lnum, FALSE);
    ಖಾಲಿಗಳು_ಸಾಲು = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_ಬಫರ್, lnum);
    ಆದರೆ (wp->w_cursor.ಲಂಬಸಾಲು > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.ಲಂಬಸಾಲು = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	byteval = (*mb_ptr2char)(p + wp->w_cursor.ಲಂಬಸಾಲು);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = ಹೊರಹೋಗು;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    ಆದರೆ (ವಾಹಕ_hl != ಖಾಲಿ && *ವಾಹಕ_hl != 0)
    {
	stl_ವಸ್ತುಗಳು[curitem].stl_type = Highlight;
	stl_ವಸ್ತುಗಳು[curitem].stl_ಆರಂಭ = p;
	stl_ವಸ್ತುಗಳು[curitem].stl_minwid = *ವಾಹಕ_hl;
	stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc = ಖಾಲಿ;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    ಗಾಗಿ (s = usefmt; *s != NUL; )
    {
	ಆದರೆ (curitem == (ಪೂರ್ಣಾಂಕ)stl_ವಸ್ತುಗಳು_len)
	{
	    size_t	new_len = stl_ವಸ್ತುಗಳು_len * 3 / 2;

	    stl_ಅಂಶ_T *new_ವಸ್ತುಗಳು =
			  vim_realloc(stl_ವಸ್ತುಗಳು, ಗಾತ್ರ(stl_ಅಂಶ_T) * new_len);
	    ಆದರೆ (new_ವಸ್ತುಗಳು == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    stl_ವಸ್ತುಗಳು = new_ವಸ್ತುಗಳು;

	    ಪೂರ್ಣಾಂಕ *new_groupitem =
			     vim_realloc(stl_groupitem, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * new_len);
	    ಆದರೆ (new_groupitem == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  ಗಾತ್ರ(stl_hlrec_T) * (new_len + 1));
	    ಆದರೆ (new_hlrec == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  ಗಾತ್ರ(stl_hlrec_T) * (new_len + 1));
	    ಆದರೆ (new_hlrec == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      ಗಾತ್ರ(stl_clickrec_T) * (new_len + 1));
	    ಆದರೆ (new_clickrec == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    stl_clicktab = new_clickrec;

	    ಪೂರ್ಣಾಂಕ *new_separator_locs = vim_realloc(stl_separator_locations,
					    ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * new_len);
	    ಆದರೆ (new_separator_locs == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    stl_separator_locations = new_separator_locs;

	    stl_ವಸ್ತುಗಳು_len = new_len;
	}

	stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc = ಖಾಲಿ;

	ಆದರೆ (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	ಆಗ (*s != NUL && *s != '%' && p + 1 < ಹೊರಹೋಗು + outlen)
# ifdef ENABLE_STL_ಬಹುಲಕ_MULTI_NL
	{
	    ಆದರೆ (*s == '\n' || *s == '\r')
	    {
		ಆದರೆ (ಬಹುಲಕ == STL_ಬಹುಲಕ_MULTI_NL)
		{
		    s++;
		    ಹೋಗು ಕಂಡುಹಿಡಿ_linebreak;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ == STL_ಬಹುಲಕ_GET_RENDERED_ಎತ್ತರ)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_ಬಹುಲಕ_MULTI_NL
	}
# endif
	ಆದರೆ (*s == NUL || p + 1 >= ಹೊರಹೋಗು + outlen)
	    ನಿಲ್ಲಿಸು;

	/*
	 * Handle one '%' item.
	 */
	s++;
	ಆದರೆ (*s == NUL)  // ignore trailing %
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    ಆದರೆ (s[1] == ']')
	    {
		stl_ವಸ್ತುಗಳು[curitem].stl_type = ClickFunc;
		stl_ವಸ್ತುಗಳು[curitem].stl_ಆರಂಭ = p;
		stl_ವಸ್ತುಗಳು[curitem].stl_minwid = 0;
		stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc = ಖಾಲಿ;
		s += 2;
		curitem++;
		ಮುಂದುವರಿ;
	    }
	    // %[FuncName] - start click region
	    ಆದರೆ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		ಆದರೆ (rb != ಖಾಲಿ)
		{
		    stl_ವಸ್ತುಗಳು[curitem].stl_type = ClickFunc;
		    stl_ವಸ್ತುಗಳು[curitem].stl_ಆರಂಭ = p;
		    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = 0;
		    stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    ಮುಂದುವರಿ;
		}
	    }
	}
	ಆದರೆ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    ಆದರೆ (ಬಹುಲಕ == STL_ಬಹುಲಕ_MULTI
# ifdef ENABLE_STL_ಬಹುಲಕ_MULTI_NL
		    || ಬಹುಲಕ == STL_ಬಹುಲಕ_MULTI_NL
# endif
	       )
	    {
		s++;
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಹುಲಕ == STL_ಬಹುಲಕ_GET_RENDERED_ಎತ್ತರ)
		rheight++;
	}
	ಆದರೆ (*s == '%')
	{
	    ಆದರೆ (p + 1 >= ಹೊರಹೋಗು + outlen)
		ನಿಲ್ಲಿಸು;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    ಮುಂದುವರಿ;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	ಆದರೆ (*s == STL_SEPARATE)
	{
	    s++;
	    ಆದರೆ (groupdepth > 0)
		ಮುಂದುವರಿ;
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = Separate;
	    stl_ವಸ್ತುಗಳು[curitem++].stl_ಆರಂಭ = p;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = Trunc;
	    stl_ವಸ್ತುಗಳು[curitem++].stl_ಆರಂಭ = p;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (*s == ')')
	{
	    char_u  *t;

	    s++;
	    ಆದರೆ (groupdepth < 1)
		ಮುಂದುವರಿ;
	    groupdepth--;

	    t = stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_ಆರಂಭ;
	    *p = NUL;
	    l = vim_strsize(t);
	    ಆದರೆ (curitem > stl_groupitem[groupdepth] + 1
		    && stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		ಪೂರ್ಣಾಂಕ ಗುಂಪು_ಆರಂಭ_userhl = 0;
		ಪೂರ್ಣಾಂಕ ಗುಂಪು_ಕೊನೆ_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		ಗಾಗಿ (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    ಆದರೆ (stl_ವಸ್ತುಗಳು[n].stl_type == Highlight)
		    {
			ಗುಂಪು_ಆರಂಭ_userhl = ಗುಂಪು_ಕೊನೆ_userhl =
						       stl_ವಸ್ತುಗಳು[n].stl_minwid;
			ನಿಲ್ಲಿಸು;
		    }
		}
		ಗಾಗಿ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    ಆದರೆ (stl_ವಸ್ತುಗಳು[n].stl_type == ಸಾಮಾನ್ಯ)
			ನಿಲ್ಲಿಸು;
		    ಆದರೆ (stl_ವಸ್ತುಗಳು[n].stl_type == Highlight)
			ಗುಂಪು_ಕೊನೆ_userhl = stl_ವಸ್ತುಗಳು[n].stl_minwid;
		}
		ಆದರೆ (n == curitem && ಗುಂಪು_ಆರಂಭ_userhl == ಗುಂಪು_ಕೊನೆ_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    ಗಾಗಿ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			ಆದರೆ (stl_ವಸ್ತುಗಳು[n].stl_type == Highlight)
			    stl_ವಸ್ತುಗಳು[n].stl_type = ಖಾಲಿಗಳು;
			// adjust the start position of TabPage to the next
			// item position
			ಆದರೆ (stl_ವಸ್ತುಗಳು[n].stl_type == Tab_ಪುಟ)
			    stl_ವಸ್ತುಗಳು[n].stl_ಆರಂಭ = p;
		    }
		}
	    }
	    ಆದರೆ (l > stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		ಆದರೆ (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    ಆಗ (l >= stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    n = (ದೀರ್ಘ)(p - t) - stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		ಪೂರ್ಣಾಂಕ minwid_fixed = MIN(stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_minwid,
				       stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_maxwid);
		ಆಗ (++l < minwid_fixed)
		    MB_CHAR2_ಬೈಟುಗಳು(fillchar, p);

		// correct the start of the items for the truncation
		ಗಾಗಿ (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ -= n - 1;
		    ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ < t)
			stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ = t;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ನಿರಪೇಕ್ಷ(stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_ವಸ್ತುಗಳು[stl_groupitem[groupdepth]].stl_minwid;
		ಪೂರ್ಣಾಂಕ fillchar_len = MB_CHAR2LEN(fillchar);
		ಆದರೆ (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    ಆಗ (l++ < n && p + fillchar_len < ಹೊರಹೋಗು + outlen)
			MB_CHAR2_ಬೈಟುಗಳು(fillchar, p);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    ಆದರೆ (p + l >= ಹೊರಹೋಗು + outlen)
		    {
			n = (ದೀರ್ಘ)((ಹೊರಹೋಗು + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    ಗಾಗಿ ( ; n > 0; n--)
			MB_CHAR2_ಬೈಟುಗಳು(fillchar, t);
		    ಗಾಗಿ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_ವಸ್ತುಗಳು[n].stl_ಆರಂಭ += l;
		}
	    }
	    ಮುಂದುವರಿ;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	ಆದರೆ (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	ಆದರೆ (*s == '-')
	{
	    s++;
	    l = -1;
	}
	ಆದರೆ (VIM_ISDIGIT(*s))
	{
	    minwid = (ಪೂರ್ಣಾಂಕ)getdigits(&s);
	    ಆದರೆ (minwid < 0)	//ಅತಿಪ್ರವಾಹ
		minwid = 0;
	}
	ಆದರೆ (*s == STL_USER_HL)
	{
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = Highlight;
	    stl_ವಸ್ತುಗಳು[curitem].stl_ಆರಂಭ = p;
	    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    ಆದರೆ (*s == STL_TABCLOSENR)
	    {
		ಆದರೆ (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    ಗಾಗಿ (n = curitem - 1; n >= 0; --n)
			ಆದರೆ (stl_ವಸ್ತುಗಳು[n].stl_type == Tab_ಪುಟ
					       && stl_ವಸ್ತುಗಳು[n].stl_minwid >= 0)
			{
			    minwid = stl_ವಸ್ತುಗಳು[n].stl_minwid;
			    ನಿಲ್ಲಿಸು;
			}
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = Tab_ಪುಟ;
	    stl_ವಸ್ತುಗಳು[curitem].stl_ಆರಂಭ = p;
	    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (*s == '.')
	{
	    s++;
	    ಆದರೆ (VIM_ISDIGIT(*s))
	    {
		maxwid = (ಪೂರ್ಣಾಂಕ)getdigits(&s);
		ಆದರೆ (maxwid <= 0)	//ಅತಿಪ್ರವಾಹ
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	ಪೂರ್ಣಾಂಕ raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	ಆದರೆ (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = ಗುಂಪು_2;
	    stl_ವಸ್ತುಗಳು[curitem].stl_ಆರಂಭ = p;
	    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = minwid;
	    stl_ವಸ್ತುಗಳು[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    ಮುಂದುವರಿ;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	ಆದರೆ (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    ಮುಂದುವರಿ;
	}
# endif
	ಆದರೆ (vim_strchr(STL_ALL, *s) == ಖಾಲಿ)
	{
	    ಆದರೆ (*s == NUL)  // can happen with "%0"
		ನಿಲ್ಲಿಸು;
	    s++;
	    ಮುಂದುವರಿ;
	}
	opt = *s++;

	// OK - now for the real work
	ತಳ = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	ಅಂಕೆ = -1;
	str = ಖಾಲಿ;
	ಆಯ್ಕೆ (opt)
	{
	ಸ್ಥಿತಿ STL_FILEPATH:
	ಸ್ಥಿತಿ STL_FULLPATH:
	ಸ್ಥಿತಿ STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_ಬಫರ್);
	    ಆದರೆ (name != ಖಾಲಿ)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_ಬಫರ್->b_ffname
					  : wp->w_ಬಫರ್->b_fname;
		home_replace(wp->w_ಬಫರ್, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    ಆದರೆ (opt != STL_FILENAME)
		str = NameBuff;
	    ಇಲ್ಲದಿದ್ದರೆ
		str = gettail(NameBuff);
	    ನಿಲ್ಲಿಸು;
	}

	ಸ್ಥಿತಿ STL_VIM_ಅಭಿವ್ಯಕ್ತಿ: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *ಖಂಡ_ಆರಂಭ = s - 1;
# endif
	    ಪೂರ್ಣಾಂಕ	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    ಆದರೆ (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    ಆಗ ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < ಹೊರಹೋಗು + outlen)
		*p++ = *s++;
	    ಆದರೆ (*s != '}')	// missing '}' or out of space
		ನಿಲ್ಲಿಸು;
	    s++;
	    ಆದರೆ (reevaluate && p > ಹೊರಹೋಗು)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    ಇಲ್ಲದಿದ್ದರೆ
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((ಅಕ್ಷರ *)buf_tmp, ಗಾತ್ರ(buf_tmp),
							 "%d", curbuf->b_fnum);
	    ಗಣ_ಆಂತರಿಕ_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((ಅಕ್ಷರ *)buf_tmp, ಗಾತ್ರ(buf_tmp), "%d", curwin->w_id);
	    ಗಣ_ಆಂತರಿಕ_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_ಸಕ್ರಿಯ = VIsual_ಸಕ್ರಿಯ;
	    curwin = wp;
	    curbuf = wp->w_ಬಫರ್;
	    // Visual mode is only valid in the current window.
	    ಆದರೆ (curwin != save_curwin)
		VIsual_ಸಕ್ರಿಯ = FALSE;

	    str = eval_ಗಮ್ಯ_string_ಸುರಕ್ಷಿತ(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_ಸಕ್ರಿಯ = save_VIsual_ಸಕ್ರಿಯ;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    ಆದರೆ (!zeropad && str != ಖಾಲಿ && *str != NUL)
	    {
		ಆದರೆ (*skipdigits(str) == NUL)
		{
		    ಅಂಕೆ = atoi((ಅಕ್ಷರ *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    ಆದರೆ (reevaluate && str != ಖಾಲಿ && *str != NUL
		    && strchr((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)str, '%') != ಖಾಲಿ
		    && evaldepth < MAX_STL_EVAL_ಆಳ)
	    {
		size_t parsed_usefmt = (size_t)(ಖಂಡ_ಆರಂಭ - usefmt);
		size_t str_ಉದ್ದಳತೆ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)str);
		size_t fmt_ಉದ್ದಳತೆ = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_ಉದ್ದಳತೆ + fmt_ಉದ್ದಳತೆ + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * ಗಾತ್ರ(char_u));

		ಆದರೆ (new_fmt != ಖಾಲಿ)
		{
		    char_u *new_fmt_p = new_fmt;

		    new_fmt_p = (char_u *)ಸ್ಮರಣೆ_ನಕಲು(new_fmt_p, usefmt, parsed_usefmt)
								   + parsed_usefmt;
		    new_fmt_p = (char_u *)ಸ್ಮರಣೆ_ನಕಲು(new_fmt_p , str, str_ಉದ್ದಳತೆ)
								      + str_ಉದ್ದಳತೆ;
		    new_fmt_p = (char_u *)ಸ್ಮರಣೆ_ನಕಲು(new_fmt_p, "%}", 2) + 2;
		    new_fmt_p = (char_u *)ಸ್ಮರಣೆ_ನಕಲು(new_fmt_p , s, fmt_ಉದ್ದಳತೆ)
								      + fmt_ಉದ್ದಳತೆ;
		    *new_fmt_p = 0;
		    new_fmt_p = ಖಾಲಿ;

		    ಆದರೆ (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    ಮುಂದುವರಿ;
		}
	    }
# endif
	    ನಿಲ್ಲಿಸು;
	}
	ಸ್ಥಿತಿ STL_ಸಾಲು:
	    ಅಂಕೆ = (wp->w_ಬಫರ್->b_ml.ml_flags & ML_ಖಾಲಿಗಳು)
		  ? 0L : (ದೀರ್ಘ)(wp->w_cursor.lnum);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_NUMLINES:
	    ಅಂಕೆ = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_ಸ್ತಂಭ:
	    ಅಂಕೆ = (State & ಬಹುಲಕ_ಸೇರಿಸು) == 0 && ಖಾಲಿಗಳು_ಸಾಲು
					       ? 0 : (ಪೂರ್ಣಾಂಕ)wp->w_cursor.ಲಂಬಸಾಲು + 1;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_VIRTCOL:
	ಸ್ಥಿತಿ STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    ಆದರೆ (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & ಬಹುಲಕ_ಸೇರಿಸು) == 0
			       && ಖಾಲಿಗಳು_ಸಾಲು ? 0 : (ಪೂರ್ಣಾಂಕ)wp->w_cursor.ಲಂಬಸಾಲು + 1)))
		ನಿಲ್ಲಿಸು;
	    ಅಂಕೆ = (ದೀರ್ಘ)virtcol;
	    ನಿಲ್ಲಿಸು;
	}

	ಸ್ಥಿತಿ STL_PERCENTAGE:
	    ಅಂಕೆ = calc_percentage((ದೀರ್ಘ)wp->w_cursor.lnum, (ದೀರ್ಘ)wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_ALTPERCENT:
	    str = buf_tmp;
	    (ಶೂನ್ಯ)get_rel_pos(wp, str, TMPLEN);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_SHOWCMD:
	    ಆದರೆ (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    ಆದರೆ (append_arg_ಸಂಖ್ಯೆ(wp, buf_tmp, ಗಾತ್ರ(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_KEYMAP:
	    fillable = FALSE;
	    ಆದರೆ (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ STL_PAGENUM:
# ಆದರೆ defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    ಅಂಕೆ = printer_ಪುಟ_ಅಂಕೆ;
# ಇಲ್ಲದಿದ್ದರೆ
	    ಅಂಕೆ = 0;
# endif
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_BUFNO:
	    ಅಂಕೆ = wp->w_ಬಫರ್->b_fnum;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_ಸ್ಥಾನಾಂತರ_X:
	    ತಳ = 'X';
	    // FALLTHROUGH
	ಸ್ಥಿತಿ STL_ಸ್ಥಾನಾಂತರ:
# ifdef FEAT_BYTEOFF
	    l = ml_ಕಂಡುಹಿಡಿ_ಸಾಲು_or_ಸ್ಥಾನಾಂತರ(wp->w_ಬಫರ್, wp->w_cursor.lnum, ಖಾಲಿ);
	    ಅಂಕೆ = (wp->w_ಬಫರ್->b_ml.ml_flags & ML_ಖಾಲಿಗಳು) || l < 0
		       ? 0L : l + 1 + ((State & ಬಹುಲಕ_ಸೇರಿಸು) == 0 && ಖಾಲಿಗಳು_ಸಾಲು
				? 0 : (ಪೂರ್ಣಾಂಕ)wp->w_cursor.ಲಂಬಸಾಲು);
# endif
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_BYTEVAL_X:
	    ತಳ = 'X';
	    // FALLTHROUGH
	ಸ್ಥಿತಿ STL_BYTEVAL:
	    ಅಂಕೆ = byteval;
	    ಆದರೆ (ಅಂಕೆ == NL)
		ಅಂಕೆ = 0;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಕೆ == CAR && get_fileformat(wp->w_ಬಫರ್) == EOL_MAC)
		ಅಂಕೆ = NL;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_ROFLAG:
	ಸ್ಥಿತಿ STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    ಆದರೆ (wp->w_ಬಫರ್->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_HELPFLAG:
	ಸ್ಥಿತಿ STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    ಆದರೆ (wp->w_ಬಫರ್->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_FILETYPE:
	    ಆದರೆ (*wp->w_ಬಫರ್->b_p_ft != NUL
		    && STRLEN(wp->w_ಬಫರ್->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((ಅಕ್ಷರ *)buf_tmp, ಗಾತ್ರ(buf_tmp), "[%s]",
							wp->w_ಬಫರ್->b_p_ft);
		str = buf_tmp;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    ಆದರೆ (*wp->w_ಬಫರ್->b_p_ft != NUL
		    && STRLEN(wp->w_ಬಫರ್->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((ಅಕ್ಷರ *)buf_tmp, ಗಾತ್ರ(buf_tmp), ",%s",
							wp->w_ಬಫರ್->b_p_ft);
		ಗಾಗಿ (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    ನಿಲ್ಲಿಸು;

# ಆದರೆ defined(FEAT_QUICKFIX)
	ಸ್ಥಿತಿ STL_PREVIEWFLAG:
	ಸ್ಥಿತಿ STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    ಆದರೆ (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_QUICKFIX:
	    ಆದರೆ (bt_quickfix(wp->w_ಬಫರ್))
		str = (char_u *)(wp->w_llist_ref
			    ? _(ಸಂದೇಶ_loclist)
			    : _(ಸಂದೇಶ_qflist));
	    ನಿಲ್ಲಿಸು;
# endif

	ಸ್ಥಿತಿ STL_MODIFIED:
	ಸ್ಥಿತಿ STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    ಆಯ್ಕೆ ((opt == STL_MODIFIED_ALT)
		    + buf_Is_ಬದಲಾಯಿತು(wp->w_ಬಫರ್) * 2
		    + (!wp->w_ಬಫರ್->b_p_ma) * 4)
	    {
		ಸ್ಥಿತಿ 2: str = (char_u *)"[+]"; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 3: str = (char_u *)",+"; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 4: str = (char_u *)"[-]"; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 5: str = (char_u *)",-"; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 6: str = (char_u *)"[+-]"; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 7: str = (char_u *)",+-"; ನಿಲ್ಲಿಸು;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		ಆಗ (*s != '#' && *s != NUL)
		    ++s;
		ಆದರೆ (*s == '#')
		{
		    stl_ವಸ್ತುಗಳು[curitem].stl_type = Highlight;
		    stl_ವಸ್ತುಗಳು[curitem].stl_ಆರಂಭ = p;
		    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = -syn_namen2id(t, (ಪೂರ್ಣಾಂಕ)(s - t));
		    curitem++;
		}
		ಆದರೆ (*s != NUL)
		    ++s;
		ಮುಂದುವರಿ;
	    }

	ಸ್ಥಿತಿ STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    ಆದರೆ (*s == ']')
	    {
		stl_ವಸ್ತುಗಳು[curitem].stl_type = ClickFunc;
		stl_ವಸ್ತುಗಳು[curitem].stl_ಆರಂಭ = p;
		stl_ವಸ್ತುಗಳು[curitem].stl_minwid = 0;
		stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc = ಖಾಲಿ;
		s++;
		curitem++;
		ಮುಂದುವರಿ;
	    }
	    // %N[FuncName] with minwid
	    ಆದರೆ (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		ಆದರೆ (rb != ಖಾಲಿ)
		{
		    stl_ವಸ್ತುಗಳು[curitem].stl_type = ClickFunc;
		    stl_ವಸ್ತುಗಳು[curitem].stl_ಆರಂಭ = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_ವಸ್ತುಗಳು[curitem].stl_minwid = raw_minwid;
		    stl_ವಸ್ತುಗಳು[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    ಮುಂದುವರಿ;
		}
	    }
	    ಮುಂದುವರಿ;

	ಸ್ಥಿತಿ STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    ಮುಂದುವರಿ;
	}

	stl_ವಸ್ತುಗಳು[curitem].stl_ಆರಂಭ = p;
	stl_ವಸ್ತುಗಳು[curitem].stl_type = ಸಾಮಾನ್ಯ;
	ಆದರೆ (str != ಖಾಲಿ && *str)
	{
	    char_u  *t = str;

	    ಆದರೆ (itemisflag)
	    {
		ಆದರೆ ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    ಆದರೆ (l > 0)
		prevchar_isitem = TRUE;
	    ಆದರೆ (l > maxwid)
	    {
		ಆಗ (l >= maxwid)
		    ಆದರೆ (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			l -= byte2cells(*t++);
		ಆದರೆ (p + 1 >= ಹೊರಹೋಗು + outlen)
		    ನಿಲ್ಲಿಸು;
		*p++ = '<';
	    }
	    ಆದರೆ (minwid > 0)
	    {
		ಗಾಗಿ (; l < minwid && p + 1 < ಹೊರಹೋಗು + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    ಆದರೆ (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    ಇಲ್ಲದಿದ್ದರೆ
			MB_CHAR2_ಬೈಟುಗಳು(fillchar, p);
		}
		minwid = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		minwid *= -1;
	    ಗಾಗಿ (; *t && p + 1 < ಹೊರಹೋಗು + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		ಆದರೆ (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2_ಬೈಟುಗಳು(fillchar, p);
		ಇಲ್ಲದಿದ್ದರೆ
		    *p++ = *t;
	    }
	    ಗಾಗಿ (; l < minwid && p + 1 < ಹೊರಹೋಗು + outlen; l++)
		MB_CHAR2_ಬೈಟುಗಳು(fillchar, p);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಕೆ >= 0)
	{
	    ಪೂರ್ಣಾಂಕ	    nbase = (ತಳ == 'D' ? 10 : (ತಳ == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    ಆದರೆ (p + 20 >= ಹೊರಹೋಗು + outlen)
		ನಿಲ್ಲಿಸು;		// not sufficient space
	    prevchar_isitem = TRUE;
	    ಆದರೆ (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    ಆದರೆ (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? ತಳ : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    ಗಾಗಿ (n = ಅಂಕೆ, l = 1; n >= nbase; n /= nbase)
		l++;
	    ಆದರೆ (opt == STL_VIRTCOL_ALT)
		l++;

	    ಆದರೆ (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		ಆಗ (l-- > maxwid)
		    ಅಂಕೆ /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((ಅಕ್ಷರ *)p, outlen - (p - ಹೊರಹೋಗು),
		    (ಅಕ್ಷರ *)nstr, 0, ಅಂಕೆ, n);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		p += vim_snprintf_safelen((ಅಕ್ಷರ *)p, outlen - (p - ಹೊರಹೋಗು),
		    (ಅಕ್ಷರ *)nstr, minwid, ಅಂಕೆ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    stl_ವಸ್ತುಗಳು[curitem].stl_type = ಖಾಲಿಗಳು;

	ಆದರೆ (ಅಂಕೆ >= 0 || (!itemisflag && str != ಖಾಲಿ && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	ಆದರೆ (opt == STL_VIM_ಅಭಿವ್ಯಕ್ತಿ)
	    vim_free(str);
	curitem++;
    }
# ifdef ENABLE_STL_ಬಹುಲಕ_MULTI_NL
ಕಂಡುಹಿಡಿ_linebreak:
# endif
    *p = NUL;
    outputlen = (size_t)(p - ಹೊರಹೋಗು);
    itemcnt = curitem;

    // Remember the most recent %# / %* highlight so the next row of a
    // multi-line statusline can resume it.
    ಆದರೆ (ವಾಹಕ_hl != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_hl = 0;
	ಗಾಗಿ (l = 0; l < itemcnt; l++)
	    ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_type == Highlight)
		ಕೊನೆಯ_hl = stl_ವಸ್ತುಗಳು[l].stl_minwid;
	*ವಾಹಕ_hl = ಕೊನೆಯ_hl;
    }

    ಆದರೆ (ಬಹುಲಕ == STL_ಬಹುಲಕ_MULTI
# ifdef ENABLE_STL_ಬಹುಲಕ_MULTI_NL
		    || ಬಹುಲಕ == STL_ಬಹುಲಕ_MULTI_NL
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
	ಆದರೆ (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಅಕ್ಷರ *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	ಆದರೆ (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    ಆದರೆ (ಬಹುಲಕ == STL_ಬಹುಲಕ_GET_RENDERED_ಎತ್ತರ)
    {
	// Free click function names that were allocated during parsing.
	ಗಾಗಿ (l = 0; l < itemcnt; l++)
	    ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_type == ClickFunc)
		vim_free(stl_ವಸ್ತುಗಳು[l].stl_clickfunc);
	ಆದರೆ (rendered_ಎತ್ತರ != ಖಾಲಿ)
	    *rendered_ಎತ್ತರ = rheight;
	ಹಿಂತಿರುಗಿ 0;
    }

    ಅಗಲ = vim_strsize(ಹೊರಹೋಗು);
    ಆದರೆ (maxwidth > 0 && ಅಗಲ > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	ಆದರೆ (itemcnt == 0)
	    s = ಹೊರಹೋಗು;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ ( ; l < itemcnt; l++)
		ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ;
		    ನಿಲ್ಲಿಸು;
		}
	    ಆದರೆ (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_ವಸ್ತುಗಳು[0].stl_ಆರಂಭ;
		l = 0;
	    }
	}

	ಆದರೆ (ಅಗಲ - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    ಆದರೆ (has_mbyte)
	    {
		s = ಹೊರಹೋಗು;
		ಅಗಲ = 0;
		ಗಾಗಿ (;;)
		{
		    ಅಗಲ += ptr2cells(s);
		    ಆದರೆ (ಅಗಲ >= maxwidth)
			ನಿಲ್ಲಿಸು;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		ಆಗ (++ಅಗಲ < maxwidth)
		    MB_CHAR2_ಬೈಟುಗಳು(fillchar, s);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		s = ಹೊರಹೋಗು + maxwidth - 1;
	    ಗಾಗಿ (l = 0; l < itemcnt; l++)
		ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ > s)
		    ನಿಲ್ಲಿಸು;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    char_u  *ಕೊನೆ = ಹೊರಹೋಗು + outputlen;

	    n = 0;
	    ಆಗ (ಅಗಲ >= maxwidth)
	    {
		ಅಗಲ -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(ಕೊನೆ - p) + 1);	// +1 for NUL
	    ಕೊನೆ -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    ಗಾಗಿ (; l < itemcnt; l++)
	    {
		ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ - n >= s)
		    stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ -= n;
		ಇಲ್ಲದಿದ್ದರೆ
		    stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ = s;
	    }

	    // Fill up for half a double-wide character.
	    ಆಗ (++ಅಗಲ < maxwidth)
	    {
		s = ಕೊನೆ;
		MB_CHAR2_ಬೈಟುಗಳು(fillchar, s);
		*s = NUL;
		ಕೊನೆ = s;
	    }
	}
	ಅಗಲ = maxwidth;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಗಲ < maxwidth &&
	    outputlen + (maxwidth - ಅಗಲ) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	ಪೂರ್ಣಾಂಕ ಅಂಕೆ_separators = 0;

	ಗಾಗಿ (l = 0; l < itemcnt; l++)
	{
	    ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[ಅಂಕೆ_separators] = l;
		ಅಂಕೆ_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	ಆದರೆ (ಅಂಕೆ_separators)
	{
	    ಪೂರ್ಣಾಂಕ standard_spaces;
	    ಪೂರ್ಣಾಂಕ final_spaces;

	    standard_spaces = (maxwidth - ಅಗಲ) / ಅಂಕೆ_separators;
	    final_spaces = (maxwidth - ಅಗಲ) -
					standard_spaces * (ಅಂಕೆ_separators - 1);
	    ಗಾಗಿ (l = 0; l < ಅಂಕೆ_separators; l++)
	    {
		ಪೂರ್ಣಾಂಕ dislocation = (l == (ಅಂಕೆ_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *ಆರಂಭ = stl_ವಸ್ತುಗಳು[stl_separator_locations[l]].stl_ಆರಂಭ;
		char_u *seploc = ಆರಂಭ + dislocation;
		STRMOVE(seploc, ಆರಂಭ);
		ಗಾಗಿ (s = ಆರಂಭ; s < seploc;)
		    MB_CHAR2_ಬೈಟುಗಳು(fillchar, s);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_ವಸ್ತುಗಳು[i].stl_ಆರಂಭ += dislocation;
	    }

	    ಅಗಲ = maxwidth;
	}
    }

    // Store the info about highlighting.
    ಆದರೆ (hltab != ಖಾಲಿ)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	ಗಾಗಿ (l = 0; l < itemcnt; l++)
	{
	    ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_type == Highlight)
	    {
		sp->ಆರಂಭ = stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ;
		sp->userhl = stl_ವಸ್ತುಗಳು[l].stl_minwid;
		sp++;
	    }
	}
	sp->ಆರಂಭ = ಖಾಲಿ;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    ಆದರೆ (tabtab != ಖಾಲಿ)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	ಗಾಗಿ (l = 0; l < itemcnt; l++)
	{
	    ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_type == Tab_ಪುಟ)
	    {
		sp->ಆರಂಭ = stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ;
		sp->userhl = stl_ವಸ್ತುಗಳು[l].stl_minwid;
		sp++;
	    }
	}
	sp->ಆರಂಭ = ಖಾಲಿ;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    ಆದರೆ (clicktab != ಖಾಲಿ)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	ಗಾಗಿ (l = 0; l < itemcnt; l++)
	{
	    ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_type == ClickFunc)
	    {
		cp->ಆರಂಭ = stl_ವಸ್ತುಗಳು[l].stl_ಆರಂಭ;
		cp->funcname = stl_ವಸ್ತುಗಳು[l].stl_clickfunc;
		cp->minwid = stl_ವಸ್ತುಗಳು[l].stl_minwid;
		cp++;
	    }
	}
	cp->ಆರಂಭ = ಖಾಲಿ;
	cp->funcname = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Free click function names when caller doesn't need them.
	ಗಾಗಿ (l = 0; l < itemcnt; l++)
	    ಆದರೆ (stl_ವಸ್ತುಗಳು[l].stl_type == ClickFunc)
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
    ಆದರೆ (did_emsg > did_emsg_before)
	ಗಣ_string_option_ನೇರ(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    ಹಿಂತಿರುಗಿ ಅಗಲ;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    ಪೂರ್ಣಾಂಕ
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		buflen)
{
    ದೀರ್ಘ	above; // number of lines above window
    ದೀರ್ಘ	ಕೆಳಗೆ; // number of lines below window

    ಆದರೆ (buflen < 3) // need at least 3 chars for writing
	ಹಿಂತಿರುಗಿ 0;
    above = wp->w_topline - 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    ಆದರೆ (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#ಆದರೆ_ಅಂತ್ಯ
    ಕೆಳಗೆ = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - wp->w_botline + 1;
    ಆದರೆ (ಕೆಳಗೆ <= 0)
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)vim_snprintf_safelen((ಅಕ್ಷರ *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    ಆದರೆ (above <= 0)
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)vim_snprintf_safelen((ಅಕ್ಷರ *)buf, buflen,
	    "%s", _("Top"));

    ಪೂರ್ಣಾಂಕ perc = calc_percentage(above, above + ಕೆಳಗೆ);
    ಅಕ್ಷರ tmp[8];
    // localized percentage value
    vim_snprintf(tmp, ಗಾತ್ರ(tmp), _("%d%%"), perc);
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)vim_snprintf_safelen((ಅಕ್ಷರ *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
append_arg_ಸಂಖ್ಯೆ(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    ಪೂರ್ಣಾಂಕ		ಕೂಡಿಸು_file)	// Add "file" before the arg number
{
    ಆದರೆ (ARGCOUNT <= 1)		// nothing to do
	ಹಿಂತಿರುಗಿ 0;

    ಅಕ್ಷರ *ಸಂದೇಶ;
    ಆಯ್ಕೆ ((wp->w_arg_idx_invalid ? 1 : 0) + (ಕೂಡಿಸು_file ? 2 : 0))
    {
	ಸ್ಥಿತಿ 0: ಸಂದೇಶ = _(" (%d of %d)"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 1: ಸಂದೇಶ = _(" ((%d) of %d)"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2: ಸಂದೇಶ = _(" (file %d of %d)"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 3: ಸಂದೇಶ = _(" (file (%d) of %d)"); ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)vim_snprintf_safelen((ಅಕ್ಷರ *)buf, buflen, ಸಂದೇಶ,
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIX
    ಹಿಂತಿರುಗಿ ತುಂಬಿದ_Name_save(fname, TRUE);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (!vim_isAbsName(fname)
	    || ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)fname, "..") != ಖಾಲಿ
	    || ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)fname, "//") != ಖಾಲಿ
# ifdef BACKSLASH_ಒಳ_FILENAME
	    || ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)fname, "\\\\") != ಖಾಲಿ
# endif
# ಆದರೆ defined(MSWIN)
	    || vim_strchr(fname, '~') != ಖಾಲಿ
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	ಹಿಂತಿರುಗಿ ತುಂಬಿದ_Name_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    ಆದರೆ (fname != ಖಾಲಿ)
	fname_case(fname, 0);	// set correct case for file name
# endif

    ಹಿಂತಿರುಗಿ fname;
#ಆದರೆ_ಅಂತ್ಯ
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
    buf_T	*buf ಬಳಸದ,
    char_u	**ffname,
    char_u	**sfname)
{
    ಆದರೆ (*ffname == ಖಾಲಿ)	    // no file name given, nothing to do
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (*sfname == ಖಾಲಿ)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SHORTCUT
    ಆದರೆ (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_ಮಾರ್ಗ(*ffname, FALSE);
	ಆದರೆ (rfname != ಖಾಲಿ)
	{
	    vim_free(*ffname);
	    *ffname = rfname;
	    *sfname = rfname;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Open a window for a number of buffers.
 */
    ಶೂನ್ಯ
ex_ಬಫರ್_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    ಪೂರ್ಣಾಂಕ		ವಿಭಜಿಸು_ret = OK;
    ಪೂರ್ಣಾಂಕ		p_ea_save;
    ಪೂರ್ಣಾಂಕ		open_wins = 0;
    ಪೂರ್ಣಾಂಕ		r;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;		// Maximum number of windows to open.
    ಪೂರ್ಣಾಂಕ		all;		// When TRUE also load inactive buffers.
    ಪೂರ್ಣಾಂಕ		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)	// make as many windows as possible
	ಎಣಿಕೆ_2 = 9999;
    ಇಲ್ಲದಿದ್ದರೆ
	ಎಣಿಕೆ_2 = eap->line2;	// make as many windows as specified
    ಆದರೆ (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	all = TRUE;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    setpcmark();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    need_mouse_correct = TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Close superfluous windows (two windows for the same buffer).
     * Also close windows that are not full-width.
     */
    ಆದರೆ (had_tab > 0)
	goto_tabpage_tp(ಮೊದಲ_tabpage, TRUE, TRUE);
    ಗಾಗಿ (;;)
    {
	tpnext = curtab->tp_ಮುಂದಿನ;
	ಗಾಗಿ (wp = firstwin; wp != ಖಾಲಿ; wp = wpnext)
	{
	    wpnext = wp->w_ಮುಂದಿನ;
	    ಆದರೆ ((wp->w_ಬಫರ್->b_nwindows > 1
			|| ((cmdmod.cmod_ವಿಭಜಿಸು & WSP_VERT)
			    ? wp->w_ಎತ್ತರ + wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ < ಅಡ್ಡಸಾಲುಗಳು - p_ch
							     - tabline_ಎತ್ತರ()
			    : wp->w_ಅಗಲ != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_ಕಿಟಕಿ
		    && !(win_locked(wp) || wp->w_ಬಫರ್->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		ಆದರೆ (win_close(wp, FALSE) == FAIL)
		    ನಿಲ್ಲಿಸು;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = ಮೊದಲ_tabpage;
		open_wins = 0;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	ಆದರೆ (had_tab == 0 || tpnext == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
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
    ಗಾಗಿ (buf = firstbuf; buf != ಖಾಲಿ && open_wins < ಎಣಿಕೆ_2; buf = buf->b_ಮುಂದಿನ)
    {
	// Check if this buffer needs a window
	ಆದರೆ ((!all && buf->b_ml.ml_mfp == ಖಾಲಿ) || !buf->b_p_bl)
	    ಮುಂದುವರಿ;

	ಆದರೆ (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    ಆದರೆ (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    ಇಲ್ಲದಿದ್ದರೆ
		wp = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		ಆದರೆ (wp->w_ಬಫರ್ == buf)
		    ನಿಲ್ಲಿಸು;
	    // If the buffer already has a window, move it
	    ಆದರೆ (wp != ಖಾಲಿ)
		win_move_after(wp, curwin);
	}

	ಆದರೆ (wp == ಖಾಲಿ && ವಿಭಜಿಸು_ret == OK)
	{
	    bufref_T	bufref;

	    ಗಣ_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    ವಿಭಜಿಸು_ret = win_ವಿಭಜಿಸು(0, WSP_ಸ್ಥಳ | WSP_ಕೆಳಗೆ);
	    ++open_wins;
	    p_ea = p_ea_save;
	    ಆದರೆ (ವಿಭಜಿಸು_ret == FAIL)
		ಮುಂದುವರಿ;

	    // Open the buffer in this window.
	    ಅದಲುಬದಲು_exists_action = SEA_DIALOG;
	    ಗಣ_curbuf(buf, DOBUF_GOTO);
	    ಆದರೆ (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		ಅದಲುಬದಲು_exists_action = SEA_NONE;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (ಅದಲುಬದಲು_exists_action == SEA_QUIT)
	    {
#ಆದರೆ defined(FEAT_EVAL)
		cleanup_T   cs;

		// Reset the error/interrupt/exception state here so that
		// aborting() returns FALSE when closing a window.
		enter_cleanup(&cs);
#ಆದರೆ_ಅಂತ್ಯ

		// User selected Quit at ATTENTION prompt; close this window.
		win_close(curwin, TRUE);
		--open_wins;
		ಅದಲುಬದಲು_exists_action = SEA_NONE;
		ಅದಲುಬದಲು_exists_did_quit = TRUE;

#ಆದರೆ defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		handle_ಅದಲುಬದಲು_exists(ಖಾಲಿ);
	}

	ui_breakcheck();
	ಆದರೆ (got_int)
	{
	    (ಶೂನ್ಯ)vgetc();	// only break the file loading, not the rest
	    ನಿಲ್ಲಿಸು;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	ಆದರೆ (aborting())
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	// When ":tab" was used open a new tab for a new window repeatedly.
	ಆದರೆ (had_tab > 0 && tabpage_ಸೂಚ್ಯಂಕ(ಖಾಲಿ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    ಗಾಗಿ (wp = lastwin; open_wins > ಎಣಿಕೆ_2; )
    {
	r = (buf_hide(wp->w_ಬಫರ್) || !buf_Is_ಬದಲಾಯಿತು(wp->w_ಬಫರ್)
				     || autowrite(wp->w_ಬಫರ್, FALSE) == OK);
	ಆದರೆ (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r)
	{
	    win_close(wp, !buf_hide(wp->w_ಬಫರ್));
	    --open_wins;
	    wp = lastwin;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wp = wp->w_prev;
	    ಆದರೆ (wp == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	}
    }
}


ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  chk_modeline(linenr_T, ಪೂರ್ಣಾಂಕ);

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
do_modelines(ಪೂರ್ಣಾಂಕ flags)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		nmlines;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	entered = 0;

    ಆದರೆ (!curbuf->b_p_ml || (nmlines = (ಪೂರ್ಣಾಂಕ)p_mls) == 0)
	ಹಿಂತಿರುಗಿ;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    ಆದರೆ (entered)
	ಹಿಂತಿರುಗಿ;

    ++entered;
    ಗಾಗಿ (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ && lnum <= nmlines;
								       ++lnum)
	ಆದರೆ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    ಗಾಗಿ (lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - nmlines; --lnum)
	ಆದರೆ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#ಅನ್ತರ್ಭಾವ "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
chk_modeline(
    linenr_T	lnum,
    ಪೂರ್ಣಾಂಕ		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*ಸಾಲು_ಕೊನೆ;		// point to the end of the line
    char_u	*e;
    ಪೂರ್ಣಾಂಕ		prev;
    ಪೂರ್ಣಾಂಕ		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    ಸಾಲು_ಕೊನೆ = s + ml_get_len(lnum);
    ಗಾಗಿ (; *s != NUL; ++s)
    {
	ಆದರೆ (prev == -1 || vim_isspace(prev))
	{
	    ಆದರೆ ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		ನಿಲ್ಲಿಸು;
	    // Accept both "vim" and "Vim".
	    ಆದರೆ ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		ಪೂರ್ಣಾಂಕ vers;

		ಆದರೆ (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		ಇಲ್ಲದಿದ್ದರೆ
		    e = s + 3;
		vers = getdigits(&e);
		ಆದರೆ (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_ಆವೃತ್ತಿ_100 >= vers && ಸುರಕ್ಷಿತ_isdigit(s[3]))
			    || (VIM_ಆವೃತ್ತಿ_100 < vers && s[3] == '<')
			    || (VIM_ಆವೃತ್ತಿ_100 > vers && s[3] == '>')
			    || (VIM_ಆವೃತ್ತಿ_100 == vers && s[3] == '=')))
		    ನಿಲ್ಲಿಸು;
	    }
	}
	prev = *s;
    }

    ಆದರೆ (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	ಪೂರ್ಣಾಂಕ	ಕೊನೆ;

	ಮಾಡು				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	ಆಗ (s[-1] != ':');

	len = (size_t)(ಸಾಲು_ಕೊನೆ - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	ಆದರೆ (linecopy == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಸಾಲು_ಕೊನೆ = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_ತಳ್ಳು(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_ಸಿದ್ಧತೆ;

	ಕೊನೆ = FALSE;
	ಆಗ (ಕೊನೆ == FALSE)
	{
	    s = skipwhite(s);
	    ಆದರೆ (*s == NUL)
		ನಿಲ್ಲಿಸು;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    ಗಾಗಿ (e = s; *e != ':' && *e != NUL; ++e)
		ಆದರೆ (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(ಸಾಲು_ಕೊನೆ - (e + 1)) + 1);	// +1 for NUL
		    --ಸಾಲು_ಕೊನೆ;
		}
	    ಆದರೆ (*e == NUL)
		ಕೊನೆ = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    ಆದರೆ (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		ಆದರೆ (*e != ':')		// no terminating ':'?
		    ನಿಲ್ಲಿಸು;
		ಕೊನೆ = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    ಆದರೆ (*s != NUL)		// skip over an empty "::"
	    {
		ಪೂರ್ಣಾಂಕ secure_save = secure;
		sctx_T	save_current_sctx = current_sctx;

		current_sctx.sc_ಆವೃತ್ತಿ = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
		current_sctx.sc_sid = SID_MODELINE;
		current_sctx.sc_seq = 0;
		current_sctx.sc_lnum = lnum;
#ಆದರೆ_ಅಂತ್ಯ

		// Make sure no risky things are executed as a side effect.
		secure = 1;

		retval = do_ಗಣ(s, OPT_MODELINE | OPT_LOCAL | flags);

		secure = secure_save;
		current_sctx = save_current_sctx;
		ಆದರೆ (retval == FAIL)		// stop if error found
		    ನಿಲ್ಲಿಸು;
	    }
	    s = (e == ಸಾಲು_ಕೊನೆ) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_ತೆಗೆ();
	vim_free(linecopy);
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    ಪೂರ್ಣಾಂಕ
bt_ಸಾಮಾನ್ಯ(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buf != ಖಾಲಿ && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    ಪೂರ್ಣಾಂಕ
bt_quickfix(buf_T *buf ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    ಹಿಂತಿರುಗಿ buf != ಖಾಲಿ && buf->b_p_bt[0] == 'q';
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    ಪೂರ್ಣಾಂಕ
bt_terminal(buf_T *buf ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_TERMINAL)
    ಹಿಂತಿರುಗಿ buf != ಖಾಲಿ && buf->b_p_bt[0] == 't';
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    ಪೂರ್ಣಾಂಕ
bt_help(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buf != ಖಾಲಿ && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    ಪೂರ್ಣಾಂಕ
bt_prompt(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buf != ಖಾಲಿ && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#ಆದರೆ defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    ಪೂರ್ಣಾಂಕ
bt_popup(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buf != ಖಾಲಿ && buf->b_p_bt != ಖಾಲಿ
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    ಪೂರ್ಣಾಂಕ
bt_nofilename(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buf != ಖಾಲಿ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
bt_nofileread(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buf != ಖಾಲಿ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#ಆದರೆ defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    ಪೂರ್ಣಾಂಕ
bt_nofile(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buf != ಖಾಲಿ && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    ಪೂರ್ಣಾಂಕ
bt_dontwrite(buf_T *buf)
{
    ಹಿಂತಿರುಗಿ buf != ಖಾಲಿ && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    ಪೂರ್ಣಾಂಕ
bt_dontwrite_ಸಂದೇಶ(buf_T *buf)
{
    ಆದರೆ (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_ಗಣ));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    ಪೂರ್ಣಾಂಕ
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    ಆಯ್ಕೆ (buf->b_p_bh[0])
    {
	ಸ್ಥಿತಿ 'u':		    // "unload"
	ಸ್ಥಿತಿ 'w':		    // "wipe"
	ಸ್ಥಿತಿ 'd': ಹಿಂತಿರುಗಿ FALSE;	    // "delete"
	ಸ್ಥಿತಿ 'h': ಹಿಂತಿರುಗಿ TRUE;	    // "hide"
    }
    ಹಿಂತಿರುಗಿ (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#ಆದರೆ defined(FEAT_QUICKFIX)
    ಆದರೆ (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	ಆದರೆ (buf->b_fnum == qf_ರಾಶಿ_get_bufnr())
	    ಹಿಂತಿರುಗಿ (char_u *)_(ಸಂದೇಶ_qflist);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ (char_u *)_(ಸಂದೇಶ_loclist);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    ಆದರೆ (bt_nofilename(buf))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	ಆದರೆ (buf->b_ಪದ != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಪದ_get_ಸ್ಥಿತಿ_ಪಠ್ಯ(buf->b_ಪದ);
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (buf->b_fname != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ buf->b_fname;
	ಆದರೆ (buf == cmdwin_buf)
	    ಹಿಂತಿರುಗಿ (char_u *)_("[Command Line]");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	ಆದರೆ (bt_prompt(buf))
	    ಹಿಂತಿರುಗಿ (char_u *)_("[Prompt]");
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
	ಆದರೆ (bt_popup(buf))
	    ಹಿಂತಿರುಗಿ (char_u *)_("[Popup]");
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ (char_u *)_("[Scratch]");
    }

    ಆದರೆ (buf->b_fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ buf_get_fname(buf);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    ಆದರೆ (buf->b_fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (char_u *)_("[No Name]");
    ಹಿಂತಿರುಗಿ buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    ಶೂನ್ಯ
ಗಣ_buflisted(ಪೂರ್ಣಾಂಕ on)
{
    ಆದರೆ (on == curbuf->b_p_bl)
	ಹಿಂತಿರುಗಿ;

    curbuf->b_p_bl = on;
    ಆದರೆ (on)
	apply_autocmds(EVENT_BUFADD, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
    ಇಲ್ಲದಿದ್ದರೆ
	apply_autocmds(EVENT_BUFDELETE, ಖಾಲಿ, ಖಾಲಿ, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    ಪೂರ್ಣಾಂಕ
buf_contents_ಬದಲಾಯಿತು(buf_T *buf)
{
    buf_T	*newbuf;
    ಪೂರ್ಣಾಂಕ		ವ್ಯತ್ಯಾಸ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ಖಾಲಿ, ಖಾಲಿ, (linenr_T)1, BLN_DUMMY);
    ಆದರೆ (newbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    ಆದರೆ (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_ಬಫರ್(newbuf, FALSE);
	ಹಿಂತಿರುಗಿ TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    ಆದರೆ (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_ಬಫರ್(newbuf, FALSE);
	ಹಿಂತಿರುಗಿ TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    ಖಂಡ_autocmds();
    ಆದರೆ (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	ಆದರೆ (buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ == curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    ವ್ಯತ್ಯಾಸ = FALSE;
	    ಗಾಗಿ (lnum = 1; lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
		ಆದರೆ (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    ವ್ಯತ್ಯಾಸ = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    ಆದರೆ (curbuf != newbuf)	// safety check
	wipe_ಬಫರ್(newbuf, FALSE);

    unblock_autocmds();

    ಹಿಂತಿರುಗಿ ವ್ಯತ್ಯಾಸ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    ಶೂನ್ಯ
wipe_ಬಫರ್(
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		aucmd)	    // When TRUE trigger autocommands.
{
    ಆದರೆ (buf->b_fnum == ತುದಿ_file_ಅಂಕೆ - 1)
	--ತುದಿ_file_ಅಂಕೆ;

    ಆದರೆ (!aucmd)		    // Don't trigger BufDelete autocommands here.
	ಖಂಡ_autocmds();

    close_ಬಫರ್(ಖಾಲಿ, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    ಆದರೆ (!aucmd)
	unblock_autocmds();
}
