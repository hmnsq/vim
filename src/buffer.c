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

#అన్తర్భావ "vim.h"


#నిర్వచితమైతే FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_లోతు 100
#అయితే_ముగింపు

స్థిర శూన్య	enter_బఫరు(buf_T *buf);
స్థిర శూన్య	buflist_getfpos(శూన్య);
స్థిర char_u	*buflist_సరిపోలిక(regmatch_T *rmp, buf_T *buf, పూర్ణాంకం ignore_case);
స్థిర char_u	*fname_సరిపోలిక(regmatch_T *rmp, char_u *name, పూర్ణాంకం ignore_case);
#నిర్వచితమైతే UNIX
స్థిర buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
స్థిర పూర్ణాంకం	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
స్థిర పూర్ణాంకం	buf_same_ino(buf_T *buf, stat_T *stp);
#లేకపోతే
స్థిర పూర్ణాంకం	otherfile_buf(buf_T *buf, char_u *ffname);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం	విలువ_మారింది(char_u *str, char_u **చివరి);
#ఒకవేళ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
స్థిర పూర్ణాంకం build_stl_str_hl_local(stl_బహుళకం_T బహుళకం, win_T *wp,
		char_u *బహిర్గతం, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, పూర్ణాంకం opt_scope, పూర్ణాంకం fillchar, పూర్ణాంకం maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, పూర్ణాంకం *lbreaks, పూర్ణాంకం *వాహకం_hl);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం	append_arg_సంఖ్య(win_T *wp, char_u *buf, size_t buflen, పూర్ణాంకం కూడు_file);
స్థిర శూన్య	free_బఫరు(buf_T *);
స్థిర శూన్య	free_బఫరు_stuff(buf_T *buf, పూర్ణాంకం free_options);
స్థిర పూర్ణాంకం	bt_nofileread(buf_T *buf);
స్థిర పూర్ణాంకం	do_బఫరు_ext(పూర్ణాంకం action, పూర్ణాంకం ప్రారంభం, పూర్ణాంకం dir, పూర్ణాంకం లెక్క, పూర్ణాంకం flags);

#నిర్వచితమైతే UNIX
# define dev_T dev_t
#లేకపోతే
# define dev_T గుర్తులేని
#అయితే_ముగింపు

#నిర్వచించు FOR_ALL_BUFS_FROM_చివరి(buf) \
    కొరకు ((buf) = lastbuf; (buf) != ఖాళీ; (buf) = (buf)->b_prev)

#ఒకవేళ defined(FEAT_QUICKFIX)
స్థిర అక్షరం *సందేశం_loclist = N_("[Location List]");
స్థిర అక్షరం *సందేశం_qflist = N_("[Quickfix List]");
#అయితే_ముగింపు

// Number of times free_buffer() was called.
స్థిర పూర్ణాంకం	buf_free_లెక్క = 0;

స్థిర పూర్ణాంకం	శిఖరం_file_అంకె = 1;	// highest file number
స్థిర garray_T buf_reuse = GA_ఖాళీలు;	// file numbers to recycle

    స్థిర శూన్య
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    కిటికీ_layout_తాళం();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"ఒకవేళ exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    కిటికీ_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    స్థిర పూర్ణాంకం
calc_percentage(దీర్ఘ part, దీర్ఘ సంపూర్ణ)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    తిరిగి (part > 1000000L)
	? (పూర్ణాంకం)(part / (సంపూర్ణ / 100L))
	: (పూర్ణాంకం)((part * 100L) / సంపూర్ణ);
}

/*
 * Return the highest possible buffer number.
 */
    పూర్ణాంకం
get_highest_fnum(శూన్య)
{
    తిరిగి శిఖరం_file_అంకె - 1;
}

/*
 * Read data from buffer for retrying.
 */
    స్థిర పూర్ణాంకం
read_బఫరు(
    పూర్ణాంకం		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    పూర్ణాంకం		flags)		    // extra flags for readfile()
{
    పూర్ణాంకం		retval = OK;
    linenr_T	పంక్తి_లెక్క;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    పంక్తి_లెక్క = curbuf->b_ml.ml_పంక్తి_లెక్క;
    retval = readfile(
	    read_stdin ? ఖాళీ : curbuf->b_ffname,
	    read_stdin ? ఖాళీ : curbuf->b_fname,
	    పంక్తి_లెక్క, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_బఫరు);
    ఒకవేళ (retval == OK)
    {
	// Delete the binary lines.
	అయితే (--పంక్తి_లెక్క >= 0)
	    ml_delete((linenr_T)1);
    }
    లేకపోతే
    {
	// Delete the converted lines.
	అయితే (curbuf->b_ml.ml_పంక్తి_లెక్క > పంక్తి_లెక్క)
	    ml_delete(పంక్తి_లెక్క);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.నిలువు = 0;

    ఒకవేళ (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	ఒకవేళ (!readonlymode && !BUFEMPTY())
	    మారింది();
	లేకపోతే ఒకవేళ (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	ఒకవేళ (retval == OK)
	{
#నిర్వచితమైతే FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, ఖాళీ, ఖాళీ, FALSE,
							      curbuf, &retval);
#లేకపోతే
	    apply_autocmds(EVENT_STDINREADPOST, ఖాళీ, ఖాళీ, FALSE, curbuf);
#అయితే_ముగింపు
	}
    }
    తిరిగి retval;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    శూన్య
బఫరు_ensure_loaded(buf_T *buf)
{
    ఒకవేళ (buf->b_ml.ml_mfp != ఖాళీ)
	తిరిగి;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    ఒకవేళ (curbuf == buf)
    {
	ఒకవేళ (మార్పిడి_exists_action != SEA_READONLY)
	    మార్పిడి_exists_action = SEA_NONE;
	open_బఫరు(FALSE, ఖాళీ, 0);
	aucmd_restbuf(&aco);
    }
}
#అయితే_ముగింపు

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
open_బఫరు(
    పూర్ణాంకం		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    పూర్ణాంకం		flags_arg)	    // extra flags for readfile()
{
    పూర్ణాంకం		flags = flags_arg;
    పూర్ణాంకం		retval = OK;
    bufref_T	old_curbuf;
#నిర్వచితమైతే FEAT_SYN_HL
    దీర్ఘ	old_tw = curbuf->b_p_tw;
#అయితే_ముగింపు
    పూర్ణాంకం		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    ఒకవేళ (readonlymode && curbuf->b_ffname != ఖాళీ
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    ఒకవేళ (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_బఫరు(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    ఒకవేళ (curbuf->b_ml.ml_mfp != ఖాళీ)
		ఆపు;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	ఒకవేళ (curbuf == ఖాళీ)
	{
	    emsg(_(e_cannot_allocate_any_బఫరు_exiting));

	    // Don't try to do any saving, with "curbuf" NULL almost nothing
	    // will work.
	    v_dying = 2;
	    getout(2);
	}

	emsg(_(e_cannot_allocate_బఫరు_using_other_one));
	enter_బఫరు(curbuf);
#నిర్వచితమైతే FEAT_SYN_HL
	ఒకవేళ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ఖాళీ, curwin);
#అయితే_ముగింపు
	తిరిగి FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    ఒకవేళ (curbuf->b_ml.ml_mfp != ఖాళీ)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    సమితి_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_సమితి = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    ఒకవేళ (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    ఒకవేళ (curbuf->b_ffname != ఖాళీ
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	    && netbeansReadFile
#అయితే_ముగింపు
       )
    {
	పూర్ణాంకం old_సందేశం_silent = సందేశం_silent;
#నిర్వచితమైతే UNIX
	పూర్ణాంకం	save_bin = curbuf->b_p_bin;
	పూర్ణాంకం perm;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	పూర్ణాంకం oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#అయితే_ముగింపు
#నిర్వచితమైతే UNIX
	perm = mch_getperm(curbuf->b_ffname);
	ఒకవేళ (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	ఒకవేళ (read_fifo)
	    curbuf->b_p_bin = TRUE;
#అయితే_ముగింపు
	ఒకవేళ (shortmess(SHM_FILEINFO))
	    సందేశం_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#నిర్వచితమైతే UNIX
	ఒకవేళ (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    ఒకవేళ (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_బఫరు(FALSE, eap, flags);
	}
#అయితే_ముగింపు
	సందేశం_silent = old_సందేశం_silent;
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#అయితే_ముగింపు
	// Help buffer is filtered.
	ఒకవేళ (bt_help(curbuf))
	    fix_help_బఫరు();
    }
    లేకపోతే ఒకవేళ (read_stdin)
    {
	పూర్ణాంకం	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(ఖాళీ, ఖాళీ, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, ఖాళీ,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	ఒకవేళ (retval == OK)
	    retval = read_బఫరు(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    ఒకవేళ (curbuf->b_ml.ml_mfp != ఖాళీ
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    ఒకవేళ (curbuf->b_flags & BF_NEVERLOADED)
    {
	(శూన్య)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    ఒకవేళ ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != ఖాళీ)
		|| curbuf->b_modified_was_సమితి	// autocmd did ":set modified"
#నిర్వచితమైతే FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != ఖాళీ)
#అయితే_ముగింపు
       )
	మారింది();
    లేకపోతే ఒకవేళ (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_చివరి_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_చివరి_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_చివరి_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (aborting())
#లేకపోతే
    ఒకవేళ (got_int)
#అయితే_ముగింపు
	curbuf->b_flags |= BF_READERR;

#నిర్వచితమైతే FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#అయితే_ముగింపు

    // need to set w_topline, unless some autocommand already did that.
    ఒకవేళ (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#నిర్వచితమైతే FEAT_DIFF
	curwin->w_topfill = 0;
#అయితే_ముగింపు
    }
#నిర్వచితమైతే FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, ఖాళీ, ఖాళీ, FALSE, curbuf, &retval);
#లేకపోతే
    apply_autocmds(EVENT_BUFENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
#అయితే_ముగింపు

    ఒకవేళ (retval != OK)
	తిరిగి retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    ఒకవేళ (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != ఖాళీ)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	ఒకవేళ (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    ఒకవేళ ((flags & READ_NOWINENTER) == 0)
#నిర్వచితమైతే FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, ఖాళీ, ఖాళీ,
			FALSE, curbuf, &retval);
#లేకపోతే
	    apply_autocmds(EVENT_BUFWINENTER, ఖాళీ, ఖాళీ,
		    FALSE, curbuf);
#అయితే_ముగింపు

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    తిరిగి retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    శూన్య
సమితి_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == ఖాళీ ? 0 : buf->b_fnum;
    bufref->br_buf_free_లెక్క = buf_free_లెక్క;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    పూర్ణాంకం
bufref_valid(bufref_T *bufref)
{
    తిరిగి bufref->br_buf_free_లెక్క == buf_free_లెక్క
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    పూర్ణాంకం
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_చివరి(bp)
	ఒకవేళ (bp == buf)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
స్థిర hashtab_T buf_hashtab;

    స్థిర శూన్య
buf_hashtab_కూడు(buf_T *buf)
{
    vim_snprintf((అక్షరం *)buf->b_కీలకం, పరిమాణం(buf->b_కీలకం), "%x", buf->b_fnum);
    ఒకవేళ (hash_కూడు(&buf_hashtab, buf->b_కీలకం, "create buffer") == FAIL)
	emsg(_(e_బఫరు_cannot_be_registered));
}

    స్థిర శూన్య
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_కనుగొను(&buf_hashtab, buf->b_కీలకం);

    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    స్థిర పూర్ణాంకం
can_unload_బఫరు(buf_T *buf)
{
    పూర్ణాంకం	    can_unload = !buf->b_locked;

    ఒకవేళ (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ఒకవేళ (wp->w_బఫరు == buf)
	    {
		can_unload = FALSE;
		ఆపు;
	    }
    }
    // Don't unload the buffer while it's still being saved
    ఒకవేళ (can_unload && buf->b_saving)
	can_unload = FALSE;

    ఒకవేళ (!can_unload)
    {
	char_u *fname = buf->b_fname != ఖాళీ ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_గమ్యం_delete_బఫరు_that_is_లోపల_use_str),
				fname != ఖాళీ ? fname : (char_u *)"[No Name]");
    }
    తిరిగి can_unload;
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
    పూర్ణాంకం
close_బఫరు(
    win_T	*win,
    buf_T	*buf,
    పూర్ణాంకం		action,
    పూర్ణాంకం		abort_if_చివరి,
    పూర్ణాంకం		ignore_abort,
    పూర్ణాంకం		సమితి_context)
{
    పూర్ణాంకం		hiding_buf;
    bufref_T	bufref;
    పూర్ణాంకం		is_curwin = (curwin != ఖాళీ && curwin == win
				&& curwin->w_బఫరు == buf);
    tabpage_T	*the_curtab = curtab;
    పూర్ణాంకం		unload_buf = (action != 0);
    పూర్ణాంకం		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    పూర్ణాంకం		del_buf = (action == DOBUF_DEL || wipe_buf);
    పూర్ణాంకం		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    పూర్ణాంకం		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    ఒకవేళ (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    లేకపోతే ఒకవేళ (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    లేకపోతే ఒకవేళ (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#నిర్వచితమైతే FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    ఒకవేళ (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	ఒకవేళ (పదం_పని_running(buf->b_పదం))
	{
	    ఒకవేళ (wipe_buf || unload_buf)
	    {
		ఒకవేళ (!can_unload_బఫరు(buf))
		    తిరిగి FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    లేకపోతే
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	లేకపోతే ఒకవేళ (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	లేకపోతే
	{
	    ఒకవేళ (del_buf || unload_buf)
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
#అయితే_ముగింపు

    // Disallow deleting the buffer when it is locked (already being closed or
    // halfway a command that relies on it). Unloading is allowed.
    ఒకవేళ ((del_buf || wipe_buf) && !can_unload_బఫరు(buf))
	తిరిగి FALSE;

    ఒకవేళ (సమితి_context && win_valid && win->w_బఫరు == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	ఒకవేళ (buf->b_nwindows == 1)
	    సమితి_చివరి_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.నిలువు, TRUE);
    }

    సమితి_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    ఒకవేళ ((win_valid || closed_popup) && win->w_బఫరు == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_విభజించు;
	ఒకవేళ (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_గమ్యం_abort));
	    తిరిగి FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_విభజించు;
	ఒకవేళ (abort_if_చివరి && one_కిటికీ())
	    // Autocommands made this the only window.
	    వెళ్ళు aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	ఒకవేళ (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_విభజించు;
	    ఒకవేళ (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		వెళ్ళు aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_విభజించు;
	    ఒకవేళ (abort_if_చివరి && one_కిటికీ())
		// Autocommands made this the only window.
		వెళ్ళు aucmd_abort;
	}
#నిర్వచితమైతే FEAT_EVAL
	// autocmds may abort script processing
	ఒకవేళ (!ignore_abort && aborting())
	    తిరిగి FALSE;
#అయితే_ముగింపు
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ఒకవేళ (is_curwin && curwin != win && win_valid)
    {
	ఖండం_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    ఒకవేళ (bt_quickfix(buf) && win_valid && win->w_బఫరు == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_బఫరు == buf && buf->b_nwindows == 1);

#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#అయితే_ముగింపు

    // Return when another window is displaying the buffer or when not unloaded.
    ఒకవేళ (!hiding_buf || !unload_buf)
	తిరిగి FALSE;

    // Always remove the buffer when there is no file name.
    ఒకవేళ (buf->b_ffname == ఖాళీ)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    ఒకవేళ (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	తిరిగి FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    ఒకవేళ ((win_valid || closed_popup) && win->w_బఫరు == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_బఫరు = ఖాళీ;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    ఒకవేళ (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != ఖాళీ || buf->b_తదుపరి != ఖాళీ
#ఒకవేళ defined(EXITFREE)
				|| entered_free_all_mem
#అయితే_ముగింపు
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	ఒకవేళ (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    ఒకవేళ (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, పరిమాణం(పూర్ణాంకం), 50);
	    ఒకవేళ (ga_grow(&buf_reuse, 1) == OK)
		((పూర్ణాంకం *)buf_reuse.ga_దత్తాంశం)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	ఒకవేళ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	లేకపోతే
	    buf->b_sfname = ఖాళీ;
	VIM_CLEAR(buf->b_ffname);
	ఒకవేళ (buf->b_prev == ఖాళీ)
	    firstbuf = buf->b_తదుపరి;
	లేకపోతే
	    buf->b_prev->b_తదుపరి = buf->b_తదుపరి;
	ఒకవేళ (buf->b_తదుపరి == ఖాళీ)
	    lastbuf = buf->b_prev;
	లేకపోతే
	    buf->b_తదుపరి->b_prev = buf->b_prev;
	free_బఫరు(buf);
    }
    లేకపోతే
    {
	ఒకవేళ (del_buf)
	{
	    // Free all internal variables and reset option values, to make
	    // ":bdel" compatible with Vim 5.7.
	    free_బఫరు_stuff(buf, TRUE);

	    // Make it look like a new buffer.
	    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;

	    // Init the options when loaded again.
	    buf->b_p_initialized = false;
	}
	buf_clear_file(buf);
	ఒకవేళ (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    ప్రతిజ్ఞా(!win_valid || !is_curwin || win == curwin);
    తిరిగి TRUE;
}

/*
 * Make buffer not contain a file.
 */
    శూన్య
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_పంక్తి_లెక్క = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_ప్రారంభం_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_ప్రారంభం_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_ప్రారంభం_bomb = FALSE;
    buf->b_ml.ml_mfp = ఖాళీ;
    buf->b_ml.ml_flags = ML_ఖాళీలు;		// empty buffer
#నిర్వచితమైతే FEAT_NETBEANS_INTG
    netbeans_deleted_all_పంక్తులు(buf);
#అయితే_ముగింపు
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
    పూర్ణాంకం
buf_freeall(buf_T *buf, పూర్ణాంకం flags)
{
    పూర్ణాంకం		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    పూర్ణాంకం		is_curwin = (curwin != ఖాళీ && curwin->w_బఫరు == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_విభజించు;
    సమితి_bufref(&bufref, buf);
    ఒకవేళ (buf->b_ml.ml_mfp != ఖాళీ)
    {
	ఒకవేళ (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    తిరిగి FALSE;
    }
    ఒకవేళ ((flags & BFA_DEL) && buf->b_p_bl)
    {
	ఒకవేళ (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    తిరిగి FALSE;
    }
    ఒకవేళ (flags & BFA_WIPE)
    {
	ఒకవేళ (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    తిరిగి FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_విభజించు;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ఒకవేళ (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	ఖండం_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#నిర్వచితమైతే FEAT_EVAL
    // autocmds may abort script processing
    ఒకవేళ ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	తిరిగి FALSE;
#అయితే_ముగింపు

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    ఒకవేళ (buf == curbuf && !is_curbuf)
	తిరిగి FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    ఒకవేళ (buf == curbuf && VIsual_చురుకైన
#ఒకవేళ defined(EXITFREE)
	    && !entered_free_all_mem
#అయితే_ముగింపు
	    )
	చివర_visual_బహుళకం();

#నిర్వచితమైతే FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    ఒకవేళ (curwin != ఖాళీ && curwin->w_బఫరు == buf)
	reset_synblock(curwin);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    ఒకవేళ (win->w_బఫరు == buf)
		clearFolding(win);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TCL
    tcl_బఫరు_free(buf);
#అయితే_ముగింపు
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_పంక్తి_లెక్క = 0;    // no lines in buffer
    ఒకవేళ ((flags & BFA_KEEP_UNDO) == 0)
	// free the memory allocated for undo
	// and reset all undo information
	u_clearallandblockfree(buf);
#నిర్వచితమైతే FEAT_SYN_HL
    syntax_clear(&buf->b_s);	    // reset syntax info
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
    clear_buf_prop_types(buf);
#అయితే_ముగింపు
    buf->b_flags &= ~BF_READERR;    // a read error is no longer relevant
    తిరిగి TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    స్థిర శూన్య
free_బఫరు(buf_T *buf)
{
    ++buf_free_లెక్క;
    free_బఫరు_stuff(buf, TRUE);
#నిర్వచితమైతే FEAT_EVAL
    // b:changedtick uses an item in buf_T, remove it now
    dictitem_remove(buf->b_vars, (dictitem_T *)&buf->b_ct_di, "free buffer");
    unref_var_dict(buf->b_vars);
    remove_listeners(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_LUA
    lua_బఫరు_free(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MZSCHEME
    mzscheme_బఫరు_free(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PERL
    perl_buf_free(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PYTHON
    python_బఫరు_free(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PYTHON3
    python3_బఫరు_free(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_RUBY
    ruby_బఫరు_free(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
    channel_బఫరు_free(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TERMINAL
    free_terminal(buf);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_పని_CHANNEL
    vim_free(buf->b_prompt_పాఠ్యం);
    free_callback(&buf->b_prompt_callback);
    free_callback(&buf->b_prompt_అంతరాయం);
#అయితే_ముగింపు

    buf_hashtab_remove(buf);

    aubuflocal_remove(buf);

    ఒకవేళ (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_తదుపరి = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    లేకపోతే
    {
	vim_free(buf);
	ఒకవేళ (curbuf == buf)
	    curbuf = ఖాళీ;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    స్థిర శూన్య
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_సంఖ్య;
    di->di_tv.v_తాళం = VAR_FIXED;
    di->di_tv.vval.v_సంఖ్య = 0;

#నిర్వచితమైతే FEAT_EVAL
    STRCPY(buf->b_ct_di.di_కీలకం, "changedtick");
    (శూన్య)dict_కూడు(buf->b_vars, di);
#అయితే_ముగింపు
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    స్థిర శూన్య
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    అయితే (buf->b_wininfo != ఖాళీ)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_తదుపరి;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    స్థిర శూన్య
free_బఫరు_stuff(
    buf_T	*buf,
    పూర్ణాంకం		free_options)		// free options as well
{
    ఒకవేళ (free_options)
    {
	clear_wininfo(buf);		// including window-local options
	free_buf_options(buf, TRUE);
#నిర్వచితమైతే FEAT_SPELL
	ga_clear(&buf->b_s.b_langp);
#అయితే_ముగింపు
    }
#నిర్వచితమైతే FEAT_EVAL
    {
	varnumber_T tick = CHANGEDTICK(buf);

	vars_clear(&buf->b_vars->dv_hashtab); // free all buffer variables
	hash_init(&buf->b_vars->dv_hashtab);
	init_changedtick(buf);
	CHANGEDTICK(buf) = tick;
	remove_listeners(buf);
    }
#అయితే_ముగింపు
    uc_clear(&buf->b_ucmds);		// clear local user commands
#నిర్వచితమైతే FEAT_SIGNS
    buf_delete_signs(buf, (char_u *)"*");	// delete any signs
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_NETBEANS_INTG
    netbeans_file_killed(buf);
#అయితే_ముగింపు
    map_clear_బహుళకం(buf, MAP_ALL_MODES, TRUE, FALSE);  // clear local mappings
    map_clear_బహుళకం(buf, MAP_ALL_MODES, TRUE, TRUE);   // clear local abbrevs
    VIM_CLEAR(buf->b_ప్రారంభం_fenc);
}

/*
 * Free one wininfo_T.
 */
    శూన్య
free_wininfo(wininfo_T *wip)
{
    ఒకవేళ (wip->wi_optset)
    {
	clear_winopt(&wip->wi_opt);
#నిర్వచితమైతే FEAT_FOLDING
	deleteFoldRecurse(&wip->wi_folds);
#అయితే_ముగింపు
    }
    vim_free(wip);
}

/*
 * Go to another buffer.  Handles the result of the ATTENTION dialog.
 */
    శూన్య
goto_బఫరు(
    exarg_T	*eap,
    పూర్ణాంకం		ప్రారంభం,
    పూర్ణాంకం		dir,
    పూర్ణాంకం		లెక్క)
{
    bufref_T	old_curbuf;
    పూర్ణాంకం		save_sea = మార్పిడి_exists_action;
    పూర్ణాంకం		skip_help_buf;

    ఎంపిక (eap->cmdidx)
    {
	స్థితి CMD_bnext:
	స్థితి CMD_sbnext:
	స్థితి CMD_b_తదుపరి:
	స్థితి CMD_bprevious:
	స్థితి CMD_sb_తదుపరి:
	స్థితి CMD_sbprevious:
	    skip_help_buf = TRUE;
	    ఆపు;
	అప్రమేయ:
	    skip_help_buf = FALSE;
	    ఆపు;
    }

    సమితి_bufref(&old_curbuf, curbuf);

    ఒకవేళ (మార్పిడి_exists_action == SEA_NONE)
	మార్పిడి_exists_action = SEA_DIALOG;
    (శూన్య)do_బఫరు_ext(*eap->cmd == 's' ? DOBUF_విభజించు : DOBUF_GOTO, ప్రారంభం, dir, లెక్క,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    ఒకవేళ (మార్పిడి_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#ఒకవేళ defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#అయితే_ముగింపు

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	మార్పిడి_exists_action = save_sea;
	మార్పిడి_exists_did_quit = TRUE;

#ఒకవేళ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#అయితే_ముగింపు
    }
    లేకపోతే
	handle_మార్పిడి_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    శూన్య
handle_మార్పిడి_exists(bufref_T *old_curbuf)
{
#ఒకవేళ defined(FEAT_EVAL)
    cleanup_T	cs;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SYN_HL
    దీర్ఘ	old_tw = curbuf->b_p_tw;
#అయితే_ముగింపు
    buf_T	*buf;

    ఒకవేళ (మార్పిడి_exists_action == SEA_QUIT)
    {
#ఒకవేళ defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#అయితే_ముగింపు

	// User selected Quit at ATTENTION prompt.  Go back to previous
	// buffer.  If that buffer is gone or the same as the current one,
	// open a new, empty buffer.
	మార్పిడి_exists_action = SEA_NONE;	// don't want it again
	మార్పిడి_exists_did_quit = TRUE;
	close_బఫరు(curwin, curbuf, DOBUF_UNLOAD, FALSE, FALSE, TRUE);
	ఒకవేళ (old_curbuf == ఖాళీ || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    ఖండం_autocmds();
	    buf = buflist_new(ఖాళీ, ఖాళీ, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	లేకపోతే
	    buf = old_curbuf->br_buf;
	ఒకవేళ (buf != ఖాళీ)
	{
	    పూర్ణాంకం old_సందేశం_silent = సందేశం_silent;

	    ఒకవేళ (shortmess(SHM_FILEINFO))
		సందేశం_silent = 1;  // prevent fileinfo message
	    enter_బఫరు(buf);
	    // restore msg_silent, so that the command line will be shown
	    సందేశం_silent = old_సందేశం_silent;

#నిర్వచితమైతే FEAT_SYN_HL
	    ఒకవేళ (old_tw != curbuf->b_p_tw)
		check_colorcolumn(ఖాళీ, curwin);
#అయితే_ముగింపు
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#ఒకవేళ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (మార్పిడి_exists_action == SEA_RECOVER)
    {
#ఒకవేళ defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#అయితే_ముగింపు

	// User selected Recover at ATTENTION prompt.
	సందేశం_scroll = TRUE;
	ml_recover(FALSE);
	సందేశం_puts("\n");	// don't overwrite the last message
	cmdline_అడ్డువరుస = సందేశం_అడ్డువరుస;
	do_modelines(0);

#ఒకవేళ defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#అయితే_ముగింపు
    }
    మార్పిడి_exists_action = SEA_NONE;
}

/*
 * Make the current buffer empty.
 * Used when it is wiped out and it's the last buffer.
 */
    స్థిర పూర్ణాంకం
ఖాళీలు_curbuf(
    పూర్ణాంకం close_others,
    పూర్ణాంకం forceit,
    పూర్ణాంకం action)
{
    పూర్ణాంకం	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    ఒకవేళ (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_చివరి_బఫరు));
	తిరిగి FAIL;
    }

    సమితి_bufref(&bufref, buf);
    ఒకవేళ (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, ఖాళీ, ఖాళీ, ఖాళీ, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    ఒకవేళ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_బఫరు(ఖాళీ, buf, action, FALSE, FALSE, FALSE);
    ఒకవేళ (!close_others)
	need_fileinfo = FALSE;
    లేకపోతే ఒకవేళ (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    తిరిగి retval;
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
    స్థిర పూర్ణాంకం
do_బఫరు_ext(
    పూర్ణాంకం		action,
    పూర్ణాంకం		ప్రారంభం,
    పూర్ణాంకం		dir,		// FORWARD or BACKWARD
    పూర్ణాంకం		లెక్క,		// buffer number or number of buffers
    పూర్ణాంకం		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    పూర్ణాంకం		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    ఎంపిక (ప్రారంభం)
    {
	స్థితి DOBUF_మొదటి:   buf = firstbuf; ఆపు;
	స్థితి DOBUF_చివరి:    buf = lastbuf;  ఆపు;
	అప్రమేయ:	    buf = curbuf;   ఆపు;
    }
    ఒకవేళ (ప్రారంభం == DOBUF_MOD)	    // find next modified buffer
    {
	అయితే (లెక్క-- > 0)
	{
	    చేయి
	    {
		buf = buf->b_తదుపరి;
		ఒకవేళ (buf == ఖాళీ)
		    buf = firstbuf;
	    }
	    అయితే (buf != curbuf && !buf_Is_మారింది(buf));
	}
	ఒకవేళ (!buf_Is_మారింది(buf))
	{
	    emsg(_(e_no_modified_బఫరు_దొరికింది));
	    తిరిగి FAIL;
	}
    }
    లేకపోతే ఒకవేళ (ప్రారంభం == DOBUF_మొదటి && లెక్క) // find specified buffer number
    {
	అయితే (buf != ఖాళీ && buf->b_fnum != లెక్క)
	    buf = buf->b_తదుపరి;
    }
    లేకపోతే
    {
	పూర్ణాంకం help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = ఖాళీ;
	అయితే (లెక్క > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    ఒకవేళ (bp == ఖాళీ)
		bp = buf;
	    ఒకవేళ (dir == FORWARD)
	    {
		buf = buf->b_తదుపరి;
		ఒకవేళ (buf == ఖాళీ)
		    buf = firstbuf;
	    }
	    లేకపోతే
	    {
		buf = buf->b_prev;
		ఒకవేళ (buf == ఖాళీ)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    ఒకవేళ (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--లెక్క;
		bp = ఖాళీ;	// use this buffer as new starting point
	    }
	    ఒకవేళ (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_బఫరు));
		తిరిగి FAIL;
	    }
	}
    }

    ఒకవేళ (buf == ఖాళీ)	    // could not find it
    {
	ఒకవేళ (ప్రారంభం == DOBUF_మొదటి)
	{
	    // don't warn when deleting
	    ఒకవేళ (!unload)
		semsg(_(e_బఫరు_nr_does_not_exist), లెక్క);
	}
	లేకపోతే ఒకవేళ (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_చివరి_బఫరు));
	లేకపోతే
	    emsg(_(e_cannot_go_before_మొదటి_బఫరు));
	తిరిగి FAIL;
    }
#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	తిరిగి OK;
#అయితే_ముగింపు
    ఒకవేళ (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_సమితి_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	తిరిగి FAIL;

    ఒకవేళ ((action == DOBUF_GOTO || action == DOBUF_విభజించు)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_బఫరు_nr_does_not_exist), లెక్క);
	తిరిగి FAIL;
    }

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    /*
     * delete buffer "buf" from memory and/or the list
     */
    ఒకవేళ (unload)
    {
	పూర్ణాంకం	forward;
	bufref_T bufref;

	ఒకవేళ (!can_unload_బఫరు(buf))
	    తిరిగి FAIL;

	సమితి_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	ఒకవేళ (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == ఖాళీ && !buf->b_p_bl)
	    తిరిగి FAIL;

	ఒకవేళ ((flags & DOBUF_FORCEIT) == 0 && buf_Is_మారింది(buf))
	{
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ఒకవేళ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		ఒకవేళ (పదం_పని_running(buf->b_పదం))
		{
		    ఒకవేళ (పదం_confirm_stop(buf) == FAIL)
			తిరిగి FAIL;
		}
		లేకపోతే
# endif
		{
		    dialog_మారింది(buf, FALSE);
		    ఒకవేళ (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			తిరిగి FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    ఒకవేళ (buf_Is_మారింది(buf))
			తిరిగి FAIL;
		}
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		no_write_సందేశం_buf(buf);
		తిరిగి FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	ఒకవేళ (buf == curbuf && VIsual_చురుకైన)
	    చివర_visual_బహుళకం();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    ఒకవేళ (bp->b_p_bl && bp != buf)
		ఆపు;
	ఒకవేళ (bp == ఖాళీ && buf == curbuf)
	    తిరిగి ఖాళీలు_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	అయితే (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_బఫరు->b_locked > 0)
		   && (!ONE_కిటికీ || మొదటి_tabpage->tp_తదుపరి != ఖాళీ))
	{
	    ఒకవేళ (win_close(curwin, FALSE) == FAIL)
		ఆపు;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	ఒకవేళ (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    ఒకవేళ (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_బఫరు(ఖాళీ, buf, action, FALSE, FALSE, FALSE);
	    తిరిగి OK;
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
	buf = ఖాళీ;	// selected buffer
	bp = ఖాళీ;	// used when no loaded buffer found
	ఒకవేళ (au_new_curbuf.br_buf != ఖాళీ && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_విభజించు)
	    buf = au_new_curbuf.br_buf;
	లేకపోతే ఒకవేళ (curwin->w_jumplistlen > 0)
	{
	    పూర్ణాంకం     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    ఒకవేళ (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    అయితే (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		ఒకవేళ (buf != ఖాళీ)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    ఒకవేళ (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_విభజించు)
			buf = ఖాళీ;
		    లేకపోతే ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)
		    {
			// skip unloaded buf, but may keep it for later
			ఒకవేళ (bp == ఖాళీ)
			    bp = buf;
			buf = ఖాళీ;
		    }
		}
		ఒకవేళ (buf != ఖాళీ)   // found a valid buffer: stop searching
		    ఆపు;
		// advance to older entry in jump list
		ఒకవేళ (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    ఆపు;
		ఒకవేళ (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		ఒకవేళ (jumpidx == forward)		// List exhausted for sure
		    ఆపు;
	    }
	}

	ఒకవేళ (buf == ఖాళీ)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_తదుపరి;
	    కొరకు (;;)
	    {
		ఒకవేళ (buf == ఖాళీ)
		{
		    ఒకవేళ (!forward)	// tried both directions
			ఆపు;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    కొనసాగు;
		}
		// in non-help buffer, try to skip help buffers, and vv
		ఒకవేళ (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_విభజించు)
		{
		    ఒకవేళ (buf->b_ml.ml_mfp != ఖాళీ)   // found loaded buffer
			ఆపు;
		    ఒకవేళ (bp == ఖాళీ)	// remember unloaded buf for later
			bp = buf;
		}
		ఒకవేళ (forward)
		    buf = buf->b_తదుపరి;
		లేకపోతే
		    buf = buf->b_prev;
	    }
	}
	ఒకవేళ (buf == ఖాళీ)	// No loaded buffer, use unloaded one
	    buf = bp;
	ఒకవేళ (buf == ఖాళీ)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		ఒకవేళ (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_విభజించు)
		    ఆపు;
	}
	ఒకవేళ (buf == ఖాళీ)	// Still no buffer, just take one
	{
	    ఒకవేళ (curbuf->b_తదుపరి != ఖాళీ)
		buf = curbuf->b_తదుపరి;
	    లేకపోతే
		buf = curbuf->b_prev;
	    ఒకవేళ (bt_quickfix(buf)
		    || (buf != ఖాళీ && buf != curbuf && buf->b_locked_విభజించు))
		buf = ఖాళీ;
	}
    }

    ఒకవేళ (buf == ఖాళీ)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	తిరిగి ఖాళీలు_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    ఒకవేళ (action == DOBUF_విభజించు && swbuf_goto_win_with_buf(buf) != ఖాళీ)
	తిరిగి OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    ఒకవేళ (buf != curbuf && buf->b_locked_విభజించు)
    {
	emsg(_(e_cannot_switch_గమ్యం_a_closing_బఫరు));
	తిరిగి FAIL;
    }
    ఒకవేళ (action == DOBUF_విభజించు && win_విభజించు(0, 0) == FAIL) // split window first
	తిరిగి FAIL;

    // go to current buffer - nothing to do
    ఒకవేళ (buf == curbuf)
	తిరిగి OK;

    // Check if the current buffer may be abandoned.
    ఒకవేళ (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ఒకవేళ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ఒకవేళ (పదం_పని_running(curbuf->b_పదం))
	    {
		ఒకవేళ (పదం_confirm_stop(curbuf) == FAIL)
		    తిరిగి FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    లేకపోతే
# endif
	    {
		bufref_T bufref;

		సమితి_bufref(&bufref, buf);
		dialog_మారింది(curbuf, FALSE);
		ఒకవేళ (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    తిరిగి FAIL;

		ఒకవేళ (buf_Is_మారింది(curbuf))
		{
		    no_write_సందేశం();
		    తిరిగి FAIL;
		}
	    }
	}
	లేకపోతే
#అయితే_ముగింపు
	{
	    no_write_సందేశం();
	    తిరిగి FAIL;
	}
    }

    // Go to the other buffer.
    సమితి_curbuf(buf, action);

    ఒకవేళ (action == DOBUF_విభజించు)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#ఒకవేళ defined(FEAT_EVAL)
    ఒకవేళ (aborting())	    // autocmds may abort script processing
	తిరిగి FAIL;
#అయితే_ముగింపు

    తిరిగి OK;
}

    పూర్ణాంకం
do_బఫరు(
    పూర్ణాంకం		action,
    పూర్ణాంకం		ప్రారంభం,
    పూర్ణాంకం		dir,		// FORWARD or BACKWARD
    పూర్ణాంకం		లెక్క,		// buffer number or number of buffers
    పూర్ణాంకం		forceit)	// TRUE when using !
{
    తిరిగి do_బఫరు_ext(action, ప్రారంభం, dir, లెక్క,
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
    అక్షరం *
do_bufdel(
    పూర్ణాంకం		command,
    char_u	*arg,		// pointer to extra arguments
    పూర్ణాంకం		addr_లెక్క,
    పూర్ణాంకం		ప్రారంభం_bnr,	// first buffer number in a range
    పూర్ణాంకం		చివర_bnr,	// buffer nr or last buffer nr in a range
    పూర్ణాంకం		forceit)
{
    పూర్ణాంకం		do_current = 0;	// delete current buffer?
    పూర్ణాంకం		deleted = 0;	// number of buffers deleted
    అక్షరం	*errormsg = ఖాళీ; // return value
    పూర్ణాంకం		bnr;		// buffer number
    char_u	*p;

    ఒకవేళ (addr_లెక్క == 0)
    {
	(శూన్య)do_బఫరు(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    లేకపోతే
    {
	ఒకవేళ (addr_లెక్క == 2)
	{
	    ఒకవేళ (*arg)		// both range and argument is not allowed
		తిరిగి ex_errmsg(e_trailing_characters_str, arg);
	    bnr = ప్రారంభం_bnr;
	}
	లేకపోతే	// addr_count == 1
	    bnr = చివర_bnr;

	కొరకు ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    ఒకవేళ (bnr == curbuf->b_fnum)
		do_current = bnr;
	    లేకపోతే ఒకవేళ (do_బఫరు_ext(command, DOBUF_మొదటి, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    ఒకవేళ (addr_లెక్క == 2)
	    {
		ఒకవేళ (++bnr > చివర_bnr)
		    ఆపు;
	    }
	    లేకపోతే    // addr_count == 1
	    {
		arg = skipwhite(arg);
		ఒకవేళ (*arg == NUL)
		    ఆపు;
		ఒకవేళ (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    ఒకవేళ (bnr < 0)	    // failed
			ఆపు;
		    arg = p;
		}
		లేకపోతే
		    bnr = getdigits(&arg);
	    }
	}
	ఒకవేళ (!got_int && do_current && do_బఫరు(command, DOBUF_మొదటి,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	ఒకవేళ (deleted == 0)
	{
	    ఒకవేళ (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    లేకపోతే ఒకవేళ (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    లేకపోతే
		STRCPY(IObuff, _(e_no_buffers_were_wiped_బహిర్గతం));
	    errormsg = (అక్షరం *)IObuff;
	}
	లేకపోతే ఒకవేళ (deleted >= p_report)
	{
	    ఒకవేళ (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    లేకపోతే ఒకవేళ (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    లేకపోతే
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    తిరిగి errormsg;
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
    శూన్య
సమితి_curbuf(buf_T *buf, పూర్ణాంకం action)
{
    buf_T	*prevbuf;
    పూర్ణాంకం		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#నిర్వచితమైతే FEAT_SYN_HL
    దీర్ఘ	old_tw = curbuf->b_p_tw;
#అయితే_ముగింపు
    bufref_T	newbufref;
    bufref_T	prevbufref;
    పూర్ణాంకం		valid;

    setpcmark();
    ఒకవేళ ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = curbuf->b_fnum; // remember alternate file
    buflist_altfpos(curwin);			 // remember curpos

    // Don't restart Select mode after switching to another buffer.
    VIsual_reselect = FALSE;

    // close_windows() or apply_autocmds() may change curbuf and wipe out "buf"
    prevbuf = curbuf;
    సమితి_bufref(&prevbufref, prevbuf);
    సమితి_bufref(&newbufref, buf);

    // Autocommands may delete the current buffer and/or the buffer we want to
    // go to.  In those cases don't close the buffer.
    ఒకవేళ (!apply_autocmds(EVENT_BUFLEAVE, ఖాళీ, ఖాళీ, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#నిర్వచితమైతే FEAT_EVAL
		&& !aborting()
#అయితే_ముగింపు
	       ))
    {
#నిర్వచితమైతే FEAT_SYN_HL
	ఒకవేళ (prevbuf == curwin->w_బఫరు)
	    reset_synblock(curwin);
#అయితే_ముగింపు
	ఒకవేళ (unload)
	    close_windows(prevbuf, FALSE);
#ఒకవేళ defined(FEAT_EVAL)
	ఒకవేళ (bufref_valid(&prevbufref) && !aborting())
#లేకపోతే
	ఒకవేళ (bufref_valid(&prevbufref))
#అయితే_ముగింపు
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    ఒకవేళ (prevbuf == curbuf
		    && ((State & బహుళకం_చొప్పించు) == 0 || curbuf->b_nwindows <= 1))
		u_sync(FALSE);
	    close_బఫరు(curwin, prevbuf,
		    unload ? action : (action == DOBUF_GOTO
			&& !buf_hide(prevbuf)
			&& !buf_Is_మారింది(prevbuf)) ? DOBUF_UNLOAD : 0,
		    FALSE, FALSE, TRUE);
	}
    }
    // An autocommand may have deleted "buf", already entered it (e.g., when
    // it did ":bunload") or aborted the script processing.
    // If curwin->w_buffer is null, enter_buffer() will make it valid again
    valid = buf_valid(buf);
    ఒకవేళ ((valid && buf != curbuf
#నిర్వచితమైతే FEAT_EVAL
		&& !aborting()
#అయితే_ముగింపు
	) || curwin->w_బఫరు == ఖాళీ)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	ఒకవేళ (!valid)
	    enter_బఫరు(lastbuf);
	లేకపోతే
	    enter_బఫరు(buf);
#నిర్వచితమైతే FEAT_SYN_HL
	ఒకవేళ (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ఖాళీ, curwin);
#అయితే_ముగింపు
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    స్థిర శూన్య
enter_బఫరు(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    ఒకవేళ (VIsual_చురుకైన
#ఒకవేళ defined(EXITFREE)
	    && !entered_free_all_mem
#అయితే_ముగింపు
	    )
	చివర_visual_బహుళకం();

    ఒకవేళ (curwin->w_బఫరు != ఖాళీ)
	--curwin->w_బఫరు->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_బఫరు = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    ఒకవేళ (!buf->b_help)
	get_winopts(buf);
#నిర్వచితమైతే FEAT_FOLDING
    లేకపోతే
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (curwin->w_p_diff)
	diff_buf_కూడు(curbuf);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYN_HL
    curwin->w_s = &(curbuf->b_s);
#అయితే_ముగింపు

    // Cursor on first line by default.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.నిలువు = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_సమితి_curswant = true;
    curwin->w_topline_was_సమితి = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // Make sure the buffer is loaded.
    ఒకవేళ (curbuf->b_ml.ml_mfp == ఖాళీ)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	ఒకవేళ (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_బఫరు(FALSE, ఖాళీ, 0);
    }
    లేకపోతే
    {
	ఒకవేళ (!సందేశం_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(శూన్య)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#నిర్వచితమైతే FEAT_DIFF
	curwin->w_topfill = 0;
#అయితే_ముగింపు
	apply_autocmds(EVENT_BUFENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ఖాళీ, ఖాళీ, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    ఒకవేళ (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    ఒకవేళ (curwin->w_topline == 1 && !curwin->w_topline_was_సమితి)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#నిర్వచితమైతే FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#అయితే_ముగింపు

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#నిర్వచితమైతే FEAT_KEYMAP
    ఒకవేళ (curbuf->b_kmap_state & KEYMAP_INIT)
	(శూన్య)keymap_init();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    ఒకవేళ (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(శూన్య)parse_spelllang(curwin);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_VIMINFO
    curbuf->b_చివరి_వాడిన = vim_సమయం();
#అయితే_ముగింపు

    redraw_later(UPD_NOT_VALID);
}

#ఒకవేళ defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    శూన్య
do_autochdir(శూన్య)
{
    ఒకవేళ ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != ఖాళీ
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	చివరి_chdir_reason = "autochdir";
    }
}
#అయితే_ముగింపు

    శూన్య
no_write_సందేశం_buf(buf_T *buf)
{
#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (పదం_పని_running(buf->b_పదం))
	emsg(_(e_పని_still_running_కూడు_bang_గమ్యం_చివర_the_పని));
    లేకపోతే
#అయితే_ముగింపు
	semsg(_(e_no_write_since_చివరి_change_for_బఫరు_nr_కూడు_bang_గమ్యం_override),
		buf->b_fnum);
}

    శూన్య
no_write_సందేశం(శూన్య)
{
#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (పదం_పని_running(curbuf->b_పదం))
	emsg(_(e_పని_still_running_కూడు_bang_గమ్యం_చివర_the_పని));
    లేకపోతే
#అయితే_ముగింపు
	emsg(_(e_no_write_since_చివరి_change_కూడు_bang_గమ్యం_override));
}

    శూన్య
no_write_సందేశం_nobang(buf_T *buf వాడని)
{
#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (పదం_పని_running(buf->b_పదం))
	emsg(_(e_పని_still_running));
    లేకపోతే
#అయితే_ముగింపు
	emsg(_(e_no_write_since_చివరి_change));
}

/*
 * functions for dealing with the buffer list
 */

/*
 * Return TRUE if the current buffer is empty, unnamed, unmodified and used in
 * only one window.  That means it can be re-used.
 */
    పూర్ణాంకం
curbuf_reusable(శూన్య)
{
    తిరిగి (curbuf != ఖాళీ
	&& curbuf->b_ffname == ఖాళీ
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == ఖాళీ || BUFEMPTY())
	&& !bt_quickfix(curbuf)
	&& !curbuf_Is_మారింది());
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
    పూర్ణాంకం		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#నిర్వచితమైతే UNIX
    stat_T	st;
#అయితే_ముగింపు

    ఒకవేళ (శిఖరం_file_అంకె == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#నిర్వచితమైతే UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    ఒకవేళ (sfname == ఖాళీ || mch_stat((అక్షరం *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#అయితే_ముగింపు
    ఒకవేళ (ffname != ఖాళీ && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#నిర్వచితమైతే UNIX
		buflist_findname_stat(ffname, &st)
#లేకపోతే
		buflist_findname(ffname)
#అయితే_ముగింపు
		) != ఖాళీ)
    {
	vim_free(ffname);
	ఒకవేళ (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? ఖాళీ : curwin,
						      lnum, (colnr_T)0, FALSE);

	ఒకవేళ ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	ఒకవేళ ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    సమితి_bufref(&bufref, buf);
	    ఒకవేళ (!(flags & BLN_DUMMY))
	    {
		ఒకవేళ (apply_autocmds(EVENT_BUFADD, ఖాళీ, ఖాళీ, FALSE, buf)
			&& !bufref_valid(&bufref))
		    తిరిగి ఖాళీ;
	    }
	}
	తిరిగి buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = ఖాళీ;
    ఒకవేళ ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	సమితి_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    తిరిగి ఖాళీ;
	}
#అయితే_ముగింపు
	ఒకవేళ (!bufref_valid(&bufref))
	    buf = ఖాళీ;		// buf was deleted; allocate a new buffer
    }
    ఒకవేళ (buf != curbuf || curbuf == ఖాళీ)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	ఒకవేళ (buf == ఖాళీ)
	{
	    vim_free(ffname);
	    తిరిగి ఖాళీ;
	}
#నిర్వచితమైతే FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	ఒకవేళ (buf->b_vars == ఖాళీ)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    తిరిగి ఖాళీ;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#అయితే_ముగింపు
	init_changedtick(buf);
    }

    ఒకవేళ (ffname != ఖాళీ)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    ఒకవేళ ((ffname != ఖాళీ && (buf->b_ffname == ఖాళీ || buf->b_sfname == ఖాళీ))
	    || buf->b_wininfo == ఖాళీ)
    {
	ఒకవేళ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	లేకపోతే
	    buf->b_sfname = ఖాళీ;
	VIM_CLEAR(buf->b_ffname);
	ఒకవేళ (buf != curbuf)
	    free_బఫరు(buf);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (buf == curbuf)
    {
	free_బఫరు_stuff(buf, FALSE);	// delete local variables et al.

	// Init the options.
	buf->b_p_initialized = false;
	buf_copy_options(buf, BCO_ENTER);

#నిర్వచితమైతే FEAT_KEYMAP
	// need to reload lmaps and set b:keymap_name
	curbuf->b_kmap_state |= KEYMAP_INIT;
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	// put the new buffer at the end of the buffer list
	buf->b_తదుపరి = ఖాళీ;
	ఒకవేళ (firstbuf == ఖాళీ)		// buffer list is empty
	{
	    buf->b_prev = ఖాళీ;
	    firstbuf = buf;
	}
	లేకపోతే				// append new buffer at end of list
	{
	    lastbuf->b_తదుపరి = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	ఒకవేళ ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((పూర్ణాంకం *)buf_reuse.ga_దత్తాంశం)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    అయితే (buf->b_prev != ఖాళీ && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_తదుపరి = buf->b_తదుపరి;
		ఒకవేళ (prev->b_తదుపరి != ఖాళీ)
		    prev->b_తదుపరి->b_prev = prev;
		buf->b_తదుపరి = prev;
		buf->b_prev = prev->b_prev;
		ఒకవేళ (buf->b_prev != ఖాళీ)
		    buf->b_prev->b_తదుపరి = buf;
		prev->b_prev = buf;
		ఒకవేళ (lastbuf == buf)
		    lastbuf = prev;
		ఒకవేళ (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	లేకపోతే
	    buf->b_fnum = శిఖరం_file_అంకె++;
	ఒకవేళ (శిఖరం_file_అంకె < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    ఒకవేళ (emsg_silent == 0 && !లోపల_assert_fails)
	    {
		బహిర్గతం_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    శిఖరం_file_అంకె = 1;
	}
	buf_hashtab_కూడు(buf);

	// Always copy the options from the current buffer.
	buf_copy_options(buf, BCO_ALWAYS);
    }

    buf->b_wininfo->wi_fpos.lnum = lnum;
    buf->b_wininfo->wi_win = curwin;

#నిర్వచితమైతే FEAT_SYN_HL
    hash_init(&buf->b_s.b_keywtab);
    hash_init(&buf->b_s.b_keywtab_ic);
#అయితే_ముగింపు

    buf->b_fname = buf->b_sfname;
#నిర్వచితమైతే UNIX
    ఒకవేళ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    లేకపోతే
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#అయితే_ముగింపు
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    ఒకవేళ (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_పేర్లు(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    ఒకవేళ (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	సమితి_bufref(&bufref, buf);
	ఒకవేళ (apply_autocmds(EVENT_BUFNEW, ఖాళీ, ఖాళీ, FALSE, buf)
		&& !bufref_valid(&bufref))
	    తిరిగి ఖాళీ;
	ఒకవేళ (flags & BLN_LISTED)
	{
	    ఒకవేళ (apply_autocmds(EVENT_BUFADD, ఖాళీ, ఖాళీ, FALSE, buf)
		    && !bufref_valid(&bufref))
		తిరిగి ఖాళీ;
	}
#నిర్వచితమైతే FEAT_EVAL
	ఒకవేళ (aborting())		// autocmds may abort script processing
	    తిరిగి ఖాళీ;
#అయితే_ముగింపు
    }

    తిరిగి buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    శూన్య
free_buf_options(
    buf_T	*buf,
    పూర్ణాంకం		free_p_ff)
{
    ఒకవేళ (free_p_ff)
    {
	clear_string_option(&buf->b_p_fenc);
	clear_string_option(&buf->b_p_ff);
	clear_string_option(&buf->b_p_bh);
	clear_string_option(&buf->b_p_bt);
    }
#నిర్వచితమైతే FEAT_కనుగొను_ID
    clear_string_option(&buf->b_p_def);
    clear_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    clear_string_option(&buf->b_p_inex);
# endif
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#అయితే_ముగింపు
    clear_string_option(&buf->b_p_fp);
#ఒకవేళ defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CRYPT
# ifdef FEAT_SODIUM
    ఒకవేళ (buf->b_p_కీలకం != ఖాళీ && *buf->b_p_కీలకం != NUL
			   && crypt_పద్ధతి_is_sodium(crypt_get_పద్ధతి_nr(buf)))
	crypt_sodium_munlock(buf->b_p_కీలకం, STRLEN(buf->b_p_కీలకం));
# endif
    clear_string_option(&buf->b_p_కీలకం);
#అయితే_ముగింపు
    clear_string_option(&buf->b_p_kp);
    clear_string_option(&buf->b_p_mps);
    clear_string_option(&buf->b_p_fo);
    clear_string_option(&buf->b_p_flp);
    clear_string_option(&buf->b_p_isk);
#నిర్వచితమైతే FEAT_VARTABS
    clear_string_option(&buf->b_p_vsts);
    VIM_CLEAR(buf->b_p_vsts_nopaste);
    VIM_CLEAR(buf->b_p_vsts_array);
    clear_string_option(&buf->b_p_vts);
    VIM_CLEAR(buf->b_p_vts_array);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_KEYMAP
    clear_string_option(&buf->b_p_keymap);
    keymap_clear(&buf->b_kmap_ga);
    ga_clear(&buf->b_kmap_ga);
#అయితే_ముగింపు
    clear_string_option(&buf->b_p_com);
    clear_string_option(&buf->b_p_cms);
    clear_string_option(&buf->b_p_nf);
#నిర్వచితమైతే FEAT_SYN_HL
    clear_string_option(&buf->b_p_syn);
    clear_string_option(&buf->b_s.b_syn_isk);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_SPELL
    clear_string_option(&buf->b_s.b_p_spc);
    clear_string_option(&buf->b_s.b_p_spf);
    vim_regfree(buf->b_s.b_cap_prog);
    buf->b_s.b_cap_prog = ఖాళీ;
    clear_string_option(&buf->b_s.b_p_spl);
    clear_string_option(&buf->b_s.b_p_spo);
#అయితే_ముగింపు
    clear_string_option(&buf->b_p_sua);
    clear_string_option(&buf->b_p_ft);
    clear_string_option(&buf->b_p_cink);
    clear_string_option(&buf->b_p_cino);
    clear_string_option(&buf->b_p_lop);
    clear_string_option(&buf->b_p_cinsd);
    clear_string_option(&buf->b_p_cinw);
    clear_string_option(&buf->b_p_cot);
    clear_string_option(&buf->b_p_cpt);
#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
    clear_string_option(&buf->b_p_csl);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_COMPL_FUNC
    clear_string_option(&buf->b_p_cfu);
    free_callback(&buf->b_cfu_cb);
    clear_string_option(&buf->b_p_ofu);
    free_callback(&buf->b_ofu_cb);
    clear_string_option(&buf->b_p_tsrfu);
    free_callback(&buf->b_tsrfu_cb);
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_లెక్క);
    buf->b_p_cpt_లెక్క = 0;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
    clear_string_option(&buf->b_p_gefm);
    clear_string_option(&buf->b_p_gp);
    clear_string_option(&buf->b_p_mp);
    clear_string_option(&buf->b_p_efm);
#అయితే_ముగింపు
    clear_string_option(&buf->b_p_ep);
    clear_string_option(&buf->b_p_మార్గం);
    clear_string_option(&buf->b_p_ట్యాగులు);
    clear_string_option(&buf->b_p_tc);
#నిర్వచితమైతే FEAT_EVAL
    clear_string_option(&buf->b_p_tfu);
    free_callback(&buf->b_tfu_cb);
    clear_string_option(&buf->b_p_ffu);
    free_callback(&buf->b_ffu_cb);
#అయితే_ముగింపు
    clear_string_option(&buf->b_p_dict);
#నిర్వచితమైతే FEAT_DIFF
    clear_string_option(&buf->b_p_dia);
#అయితే_ముగింపు
    clear_string_option(&buf->b_p_tsr);
    clear_string_option(&buf->b_p_qe);
    buf->b_p_ac = -1;
    buf->b_p_ar = -1;
#నిర్వచితమైతే HAVE_FSYNC
    buf->b_p_fs = -1;
#అయితే_ముగింపు
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
    పూర్ణాంకం
buflist_getfile(
    పూర్ణాంకం		n,
    linenr_T	lnum,
    పూర్ణాంకం		options,
    పూర్ణాంకం		forceit)
{
    buf_T	*buf;
    win_T	*wp = ఖాళీ;
    pos_T	*fpos;
    colnr_T	నిలువు;

    buf = buflist_findnr(n);
    ఒకవేళ (buf == ఖాళీ)
    {
	ఒకవేళ ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	లేకపోతే
	    semsg(_(e_బఫరు_nr_not_దొరికింది), n);
	తిరిగి FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    ఒకవేళ (buf == curbuf)
	తిరిగి OK;

    ఒకవేళ (పాఠ్యం_or_buf_locked())
	తిరిగి FAIL;

    // altfpos may be changed by getfile(), get it now
    ఒకవేళ (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	నిలువు = fpos->నిలువు;
    }
    లేకపోతే
	నిలువు = 0;

    ఒకవేళ (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	ఒకవేళ (wp == ఖాళీ && (swb_flags & (SWB_VSPLIT | SWB_విభజించు | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    ఒకవేళ (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    లేకపోతే ఒకవేళ (win_విభజించు(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		తిరిగి FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    పూర్ణాంకం retval = FAIL;
    ఒకవేళ (GETFILE_SUCCESS(getfile(buf->b_fnum, ఖాళీ, ఖాళీ,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	ఒకవేళ (!p_sol && నిలువు != 0)
	{
	    curwin->w_cursor.నిలువు = నిలువు;
	    check_cursor_నిలువు();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_సమితి_curswant = true;
	}
	retval = OK;
    }

    ఒకవేళ (RedrawingDisabled > 0)
	--RedrawingDisabled;
    తిరిగి retval;
}

/*
 * go to the last know line number for the current buffer
 */
    స్థిర శూన్య
buflist_getfpos(శూన్య)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    ఒకవేళ (p_sol)
	curwin->w_cursor.నిలువు = 0;
    లేకపోతే
    {
	curwin->w_cursor.నిలువు = fpos->నిలువు;
	check_cursor_నిలువు();
	curwin->w_cursor.coladd = 0;
	curwin->w_సమితి_curswant = true;
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
    buf_T	*buf = ఖాళీ;

    // First make the name into a full path name
    ffname = నిండిన_Name_save(fname,
#నిర్వచితమైతే UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#లేకపోతే
	    FALSE
#అయితే_ముగింపు
	    );
    ఒకవేళ (ffname != ఖాళీ)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    తిరిగి buf;
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
#నిర్వచితమైతే UNIX
    stat_T	st;

    ఒకవేళ (mch_stat((అక్షరం *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    తిరిగి buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    స్థిర buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#అయితే_ముగింపు
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_చివరి(buf)
	ఒకవేళ ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#నిర్వచితమైతే UNIX
		    , stp
#అయితే_ముగింపు
		    ))
	    తిరిగి buf;
    తిరిగి ఖాళీ;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    పూర్ణాంకం
buflist_findpat(
    char_u	*నమూనా,
    char_u	*నమూనా_చివర,	// pointer to first char after pattern
    పూర్ణాంకం		unlisted,	// find unlisted buffers
    పూర్ణాంకం		diffmode వాడని, // find diff-mode buffers only
    పూర్ణాంకం		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    పూర్ణాంకం		సరిపోలిక = -1;
    పూర్ణాంకం		కనుగొను_listed;
    char_u	*pat;
    char_u	*patend;
    పూర్ణాంకం		attempt;
    char_u	*p;
    పూర్ణాంకం		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    ఒకవేళ ((నమూనా_చివర == నమూనా + 1 && (*నమూనా == '%' || *నమూనా == '#'))
	    || (లోపల_vim9script() && నమూనా_చివర == నమూనా + 2
				    && నమూనా[0] == '%' && నమూనా[1] == '%'))
    {
	ఒకవేళ (*నమూనా == '#' || నమూనా_చివర == నమూనా + 2)
	    సరిపోలిక = curwin->w_alt_fnum;
	లేకపోతే
	    సరిపోలిక = curbuf->b_fnum;
#నిర్వచితమైతే FEAT_DIFF
	ఒకవేళ (diffmode && !diff_బహుళకం_buf(buflist_findnr(సరిపోలిక)))
	    సరిపోలిక = -1;
#అయితే_ముగింపు
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
    లేకపోతే
    {
	pat = file_pat_గమ్యం_reg_pat(నమూనా, నమూనా_చివర, ఖాళీ, FALSE);
	ఒకవేళ (pat == ఖాళీ)
	    తిరిగి -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	కనుగొను_listed = TRUE;
	కొరకు (;;)
	{
	    కొరకు (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		ఒకవేళ (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		ఒకవేళ (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_చివరి(buf)
		{
		    ఒకవేళ (regmatch.regprog == ఖాళీ)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			తిరిగి -1;
		    }
		    ఒకవేళ (buf->b_p_bl == కనుగొను_listed
#నిర్వచితమైతే FEAT_DIFF
			    && (!diffmode || diff_బహుళకం_buf(buf))
#అయితే_ముగింపు
			    && buflist_సరిపోలిక(&regmatch, buf, FALSE) != ఖాళీ)
		    {
			ఒకవేళ (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				ఒకవేళ (wp->w_బఫరు == buf)
				    ఆపు;
			    ఒకవేళ (wp == ఖాళీ)
				కొనసాగు;
			}
			ఒకవేళ (సరిపోలిక >= 0)		// already found a match
			{
			    సరిపోలిక = -2;
			    ఆపు;
			}
			సరిపోలిక = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		ఒకవేళ (సరిపోలిక >= 0)			// found one match
		    ఆపు;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    ఒకవేళ (!unlisted || !కనుగొను_listed || సరిపోలిక != -1)
		ఆపు;
	    కనుగొను_listed = FALSE;
	}

	vim_free(pat);
    }

    ఒకవేళ (సరిపోలిక == -2)
	semsg(_(e_మరిన్ని_than_one_సరిపోలిక_for_str), నమూనా);
    లేకపోతే ఒకవేళ (సరిపోలిక < 0)
	semsg(_(e_no_matching_బఫరు_for_str), నమూనా);
    తిరిగి సరిపోలిక;
}

#నిర్వచితమైతే FEAT_VIMINFO
రకనిర్వచనం నిర్మాణం {
    buf_T   *buf;
    char_u  *సరిపోలిక;
} bufmatch_T;
#అయితే_ముగింపు

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    పూర్ణాంకం
ExpandBufnames(
    char_u	*pat,
    పూర్ణాంకం		*అంకె_file,
    char_u	***file,
    పూర్ణాంకం		options)
{
    పూర్ణాంకం		లెక్క;
    buf_T	*buf;
    పూర్ణాంకం		విడత;
    char_u	*p;
    char_u	*patc = ఖాళీ;
#నిర్వచితమైతే FEAT_VIMINFO
    bufmatch_T	*matches = ఖాళీ;
#అయితే_ముగింపు
    పూర్ణాంకం		fuzzy;
    fuzmatch_str_T  *fuzmatch = ఖాళీ;
    regmatch_T	regmatch;
    పూర్ణాంకం		score = 0;
    పూర్ణాంకం		గమ్యం_free = FALSE;

    *అంకె_file = 0;		    // return values in case of FAIL
    *file = ఖాళీ;

#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	తిరిగి FAIL;
#అయితే_ముగింపు

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    ఒకవేళ (!fuzzy)
    {
	ఒకవేళ (*pat == '^' && pat[1] != NUL)
	{
	    పూర్ణాంకం  len = (పూర్ణాంకం)STRLEN(pat);
	    patc = alloc(len);
	    ఒకవేళ (patc == ఖాళీ)
		తిరిగి FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    గమ్యం_free = TRUE;
	}
	లేకపోతే ఒకవేళ (*pat == '^')
	    patc = (char_u *)"";
	లేకపోతే
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    కొరకు (విడత = 1; విడత <= 2; ++విడత)
    {
	లెక్క = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    ఒకవేళ (!buf->b_p_bl)	// skip unlisted buffers
		కొనసాగు;
#నిర్వచితమైతే FEAT_DIFF
	    ఒకవేళ (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		ఒకవేళ (buf == curbuf || !diff_బహుళకం_buf(buf))
		    కొనసాగు;
#అయితే_ముగింపు

	    ఒకవేళ (!fuzzy)
	    {
		ఒకవేళ (regmatch.regprog == ఖాళీ)
		{
		    // invalid pattern, possibly after recompiling
		    ఒకవేళ (గమ్యం_free)
			vim_free(patc);
		    తిరిగి FAIL;
		}
		p = buflist_సరిపోలిక(&regmatch, buf, p_wic);
	    }
	    లేకపోతే
	    {
		p = ఖాళీ;
		// first try matching with the short file name
		ఒకవేళ ((score = fuzzy_సరిపోలిక_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		ఒకవేళ (p == ఖాళీ)
		{
		    // next try matching with the full path file name
		    ఒకవేళ ((score = fuzzy_సరిపోలిక_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    ఒకవేళ (p == ఖాళీ)
		కొనసాగు;

	    ఒకవేళ (విడత == 1)
	    {
		++లెక్క;
		కొనసాగు;
	    }

	    ఒకవేళ (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    లేకపోతే
		p = vim_strsave(p);
	    ఒకవేళ (p == ఖాళీ)
	    {
		ఒకవేళ (fuzzy && విడత == 2)
		    fuzmatch_str_free(fuzmatch, లెక్క);
		తిరిగి FAIL;
	    }

	    ఒకవేళ (!fuzzy)
	    {
#నిర్వచితమైతే FEAT_VIMINFO
		ఒకవేళ (matches != ఖాళీ)
		{
		    matches[లెక్క].buf = buf;
		    matches[లెక్క].సరిపోలిక = p;
		    లెక్క++;
		}
		లేకపోతే
#అయితే_ముగింపు
		    (*file)[లెక్క++] = p;
	    }
	    లేకపోతే
	    {
		fuzmatch[లెక్క].idx = లెక్క;
		fuzmatch[లెక్క].str = p;
		fuzmatch[లెక్క].score = score;
		లెక్క++;
	    }
	}
	ఒకవేళ (లెక్క == 0)	// no match found, break here
	    ఆపు;
	ఒకవేళ (విడత == 1)
	{
	    ఒకవేళ (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, లెక్క);
		ఒకవేళ (*file == ఖాళీ)
		{
		    vim_regfree(regmatch.regprog);
		    ఒకవేళ (గమ్యం_free)
			vim_free(patc);
		    తిరిగి FAIL;
		}
#నిర్వచితమైతే FEAT_VIMINFO
		ఒకవేళ (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, లెక్క);
#అయితే_ముగింపు
	    }
	    లేకపోతే
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, లెక్క);
		ఒకవేళ (fuzmatch == ఖాళీ)
		{
		    *అంకె_file = 0;
		    *file = ఖాళీ;
		    తిరిగి FAIL;
		}
	    }
	}
    }

    ఒకవేళ (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	ఒకవేళ (గమ్యం_free)
	    vim_free(patc);
    }

    ఒకవేళ (!fuzzy)
    {
#నిర్వచితమైతే FEAT_VIMINFO
	ఒకవేళ (matches != ఖాళీ)
	{
	    పూర్ణాంకం i;
	    ఒకవేళ (లెక్క > 1)
		qsort(matches, లెక్క, పరిమాణం(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    ఒకవేళ (matches[0].buf == curbuf)
	    {
		కొరకు (i = 1; i < లెక్క; i++)
		    (*file)[i-1] = matches[i].సరిపోలిక;
		(*file)[లెక్క-1] = matches[0].సరిపోలిక;
	    }
	    లేకపోతే
	    {
		కొరకు (i = 0; i < లెక్క; i++)
		    (*file)[i] = matches[i].సరిపోలిక;
	    }
	    vim_free(matches);
	}
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	ఒకవేళ (fuzzymatches_గమ్యం_strmatches(fuzmatch, file, లెక్క, FALSE) == FAIL)
	    తిరిగి FAIL;
    }

    *అంకె_file = లెక్క;
    తిరిగి (లెక్క == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    స్థిర char_u *
buflist_సరిపోలిక(
    regmatch_T	*rmp,
    buf_T	*buf,
    పూర్ణాంకం		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*సరిపోలిక;

    // First try the short file name, then the long file name.
    సరిపోలిక = fname_సరిపోలిక(rmp, buf->b_sfname, ignore_case);
    ఒకవేళ (సరిపోలిక == ఖాళీ && rmp->regprog != ఖాళీ)
	సరిపోలిక = fname_సరిపోలిక(rmp, buf->b_ffname, ignore_case);

    తిరిగి సరిపోలిక;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    స్థిర char_u *
fname_సరిపోలిక(
    regmatch_T	*rmp,
    char_u	*name,
    పూర్ణాంకం		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*సరిపోలిక = ఖాళీ;
    char_u	*p;

    // extra check for valid arguments
    ఒకవేళ (name == ఖాళీ || rmp->regprog == ఖాళీ)
	తిరిగి ఖాళీ;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    ఒకవేళ (vim_regexec(rmp, name, (colnr_T)0))
	సరిపోలిక = name;
    లేకపోతే ఒకవేళ (rmp->regprog != ఖాళీ)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(ఖాళీ, name);
	ఒకవేళ (p != ఖాళీ && vim_regexec(rmp, p, (colnr_T)0))
	    సరిపోలిక = name;
	vim_free(p);
    }

    తిరిగి సరిపోలిక;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(పూర్ణాంకం nr)
{
    char_u	కీలకం[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    ఒకవేళ (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((అక్షరం *)కీలకం, పరిమాణం(కీలకం), "%x", nr);
    hi = hash_కనుగొను(&buf_hashtab, కీలకం);

    ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
	తిరిగి (buf_T *)(hi->hi_కీలకం
			     - ((గుర్తులేని)(curbuf->b_కీలకం - (char_u *)curbuf)));
    తిరిగి ఖాళీ;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    పూర్ణాంకం		n,
    పూర్ణాంకం		fullname,
    పూర్ణాంకం		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి home_replace_save(helptail ? buf : ఖాళీ,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    శూన్య
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	నిలువు,
    పూర్ణాంకం		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ఒకవేళ (wip->wi_win == win)
	    ఆపు;
    ఒకవేళ (wip == ఖాళీ)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	ఒకవేళ (wip == ఖాళీ)
	    తిరిగి;
	wip->wi_win = win;
	ఒకవేళ (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    లేకపోతే
    {
	// remove the entry from the list
	ఒకవేళ (wip->wi_prev)
	    wip->wi_prev->wi_తదుపరి = wip->wi_తదుపరి;
	లేకపోతే
	    buf->b_wininfo = wip->wi_తదుపరి;
	ఒకవేళ (wip->wi_తదుపరి)
	    wip->wi_తదుపరి->wi_prev = wip->wi_prev;
	ఒకవేళ (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#నిర్వచితమైతే FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#అయితే_ముగింపు
	}
    }
    ఒకవేళ (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.నిలువు = నిలువు;
    }
    ఒకవేళ (win != ఖాళీ)
	wip->wi_changelistidx = win->w_changelistidx;
    ఒకవేళ (copy_options && win != ఖాళీ)
    {
	// Save the window-specific option values.
	copy_winopt(&win->w_onebuf_opt, &wip->wi_opt);
#నిర్వచితమైతే FEAT_FOLDING
	wip->wi_fold_manual = win->w_fold_manual;
	cloneFoldGrowArray(&win->w_folds, &wip->wi_folds);
#అయితే_ముగింపు
	wip->wi_optset = TRUE;
    }

    // insert the entry in front of the list
    wip->wi_తదుపరి = buf->b_wininfo;
    buf->b_wininfo = wip;
    wip->wi_prev = ఖాళీ;
    ఒకవేళ (wip->wi_తదుపరి)
	wip->wi_తదుపరి->wi_prev = wip;
}

#నిర్వచితమైతే FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    స్థిర పూర్ణాంకం
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    ఒకవేళ (!wip->wi_opt.wo_diff)
	తిరిగి FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	ఒకవేళ (wip->wi_win == wp)
	    తిరిగి FALSE;
    తిరిగి TRUE;
}
#అయితే_ముగింపు

/*
 * Find info for the current window in buffer "buf".
 * If not found, return the info for the most recently used window.
 * When "need_options" is TRUE skip entries where wi_optset is FALSE.
 * When "skip_diff_buffer" is TRUE avoid windows with 'diff' set that is in
 * another tab page.
 * Returns NULL when there isn't any info.
 */
    స్థిర wininfo_T *
కనుగొను_wininfo(
    buf_T	*buf,
    పూర్ణాంకం		need_options,
    పూర్ణాంకం		skip_diff_బఫరు వాడని)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ఒకవేళ (wip->wi_win == curwin
#నిర్వచితమైతే FEAT_DIFF
		&& (!skip_diff_బఫరు || !wininfo_other_tab_diff(wip))
#అయితే_ముగింపు

		&& (!need_options || wip->wi_optset))
	    ఆపు;

    ఒకవేళ (wip != ఖాళీ)
	తిరిగి wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#నిర్వచితమైతే FEAT_DIFF
    ఒకవేళ (skip_diff_బఫరు)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    ఒకవేళ (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != ఖాళీ
			    && wip->wi_win->w_బఫరు == buf)))
		ఆపు;
    }
    లేకపోతే
#అయితే_ముగింపు
	wip = buf->b_wininfo;
    తిరిగి wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    శూన్య
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#నిర్వచితమైతే FEAT_FOLDING
    clearFolding(curwin);
#అయితే_ముగింపు

    wip = కనుగొను_wininfo(buf, TRUE, TRUE);
    ఒకవేళ (wip != ఖాళీ && wip->wi_win != ఖాళీ
	    && wip->wi_win != curwin && wip->wi_win->w_బఫరు == buf)
    {
	// The buffer is currently displayed in the window: use the actual
	// option values instead of the saved (possibly outdated) values.
	win_T *wp = wip->wi_win;

	copy_winopt(&wp->w_onebuf_opt, &curwin->w_onebuf_opt);
#నిర్వచితమైతే FEAT_STL_OPT
	// w_stl_rendered_height is not in winvar_T; copy it explicitly.
	curwin->w_stl_rendered_ఎత్తు = wp->w_stl_rendered_ఎత్తు;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_FOLDING
	curwin->w_fold_manual = wp->w_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wp->w_folds, &curwin->w_folds);
#అయితే_ముగింపు
    }
    లేకపోతే ఒకవేళ (wip != ఖాళీ && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#నిర్వచితమైతే FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#అయితే_ముగింపు
    }
    లేకపోతే
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    ఒకవేళ (wip != ఖాళీ)
	curwin->w_changelistidx = wip->wi_changelistidx;

#నిర్వచితమైతే FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    ఒకవేళ (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    ఒకవేళ (*curwin->w_p_stl != NUL)
	update_stl_rendered_ఎత్తు(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    ఒకవేళ (curwin->w_స్థితి_ఎత్తు > 0
	    && curwin->w_స్థితి_ఎత్తు != statusline_ఎత్తు(curwin))
    {
	చట్రం_change_statusline_ఎత్తు();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	ఒకవేళ (p_ea)
	    win_equal(curwin, TRUE, 'v');
    }
#అయితే_ముగింపు
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
    స్థిర pos_T no_position = {1, 0, 0};

    wip = కనుగొను_wininfo(buf, FALSE, FALSE);
    ఒకవేళ (wip != ఖాళీ)
	తిరిగి &(wip->wi_fpos);
    లేకపోతే
	తిరిగి &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    తిరిగి buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    శూన్య
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;
    పూర్ణాంకం		ro_char;
    పూర్ణాంకం		మారింది_char;
#నిర్వచితమైతే FEAT_TERMINAL
    పూర్ణాంకం		పని_running;
    పూర్ణాంకం		పని_none_open;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_దత్తాంశం = ఖాళీ, **p;

    ఒకవేళ (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, పరిమాణం(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    ఒకవేళ (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_దత్తాంశం)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_దత్తాంశం, (size_t)buflist.ga_len,
		పరిమాణం(buf_T *), buf_compare);

	buflist_దత్తాంశం = (buf_T **)buflist.ga_దత్తాంశం;
	buf = *buflist_దత్తాంశం;
    }
    p = buflist_దత్తాంశం;

    కొరకు (; buf != ఖాళీ && !got_int; buf = buflist_దత్తాంశం != ఖాళీ
	    ? (++p < buflist_దత్తాంశం + buflist.ga_len ? *p : ఖాళీ)
	    : buf->b_తదుపరి)
#లేకపోతే
    కొరకు (buf = firstbuf; buf != ఖాళీ && !got_int; buf = buf->b_తదుపరి)
#అయితే_ముగింపు
    {
	char_u	*name;

#నిర్వచితమైతే FEAT_TERMINAL
	పని_running = పదం_పని_running(buf->b_పదం);
	పని_none_open = పదం_none_open(buf->b_పదం);
#అయితే_ముగింపు
	// skip unlisted buffers, unless ! was used
	ఒకవేళ ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !buf_Is_మారింది(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == ఖాళీ || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == ఖాళీ || buf->b_nwindows != 0))
#నిర్వచితమైతే FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!పని_running || (పని_running && పని_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!పని_running || (పని_running && !పని_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (పని_running || buf->b_పదం == ఖాళీ))
#అయితే_ముగింపు
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    కొనసాగు;
	name = buf_spname(buf);
	ఒకవేళ (name != ఖాళీ)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	లేకపోతే
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	ఒకవేళ (సందేశం_filtered(NameBuff))
	    కొనసాగు;

	మారింది_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (buf_Is_మారింది(buf) ? '+' : ' ');
#నిర్వచితమైతే FEAT_TERMINAL
	ఒకవేళ (పని_running)
	{
	    ఒకవేళ (పని_none_open)
		ro_char = '?';
	    లేకపోతే
		ro_char = 'R';
	    మారింది_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	లేకపోతే ఒకవేళ (buf->b_పదం != ఖాళీ)
	    ro_char = 'F';
	లేకపోతే
#అయితే_ముగింపు
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	సందేశం_putchar('\n');
	len = (పూర్ణాంకం)vim_snprintf_safelen((అక్షరం *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == ఖాళీ ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		మారింది_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	చేయి
	    IObuff[len++] = ' ';
	అయితే (--i > 0 && len < IOSIZE - 18);
#నిర్వచితమైతే FEAT_VIMINFO
	ఒకవేళ (vim_strchr(eap->arg, 't') && buf->b_చివరి_వాడిన)
	    కూడు_సమయం(IObuff + len, (size_t)(IOSIZE - len), buf->b_చివరి_వాడిన);
	లేకపోతే
#అయితే_ముగింపు
	    vim_snprintf((అక్షరం *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (దీర్ఘ)buflist_findlnum(buf));
	సందేశం_outtrans(IObuff);
	బహిర్గతం_flush();	    // output one line at a time
	ui_breakcheck();
    }

#నిర్వచితమైతే FEAT_VIMINFO
    ఒకవేళ (buflist_దత్తాంశం)
	ga_clear(&buflist);
#అయితే_ముగింపు
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    పూర్ణాంకం
buflist_name_nr(
    పూర్ణాంకం		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ఒకవేళ (buf == ఖాళీ || buf->b_fname == ఖాళీ)
	తిరిగి FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    తిరిగి OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    పూర్ణాంకం
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    పూర్ణాంకం		సందేశం_2)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = ఖాళీ;
#నిర్వచితమైతే UNIX
    stat_T	st;
#అయితే_ముగింపు

    ఒకవేళ (ffname == ఖాళీ || *ffname == NUL)
    {
	// Removing the name.
	ఒకవేళ (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	లేకపోతే
	    buf->b_sfname = ఖాళీ;
	VIM_CLEAR(buf->b_ffname);
#నిర్వచితమైతే UNIX
	st.st_dev = (dev_T)-1;
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	ఒకవేళ (ffname == ఖాళీ)		    // out of memory
	    తిరిగి FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#నిర్వచితమైతే UNIX
	ఒకవేళ (mch_stat((అక్షరం *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#అయితే_ముగింపు
	ఒకవేళ (!(buf->b_flags & BF_DUMMY))
#నిర్వచితమైతే UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#లేకపోతే
	    obuf = buflist_findname(ffname);
#అయితే_ముగింపు
	ఒకవేళ (obuf != ఖాళీ && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    పూర్ణాంకం		లోపల_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		ఒకవేళ (win->w_బఫరు == obuf)
		    లోపల_use = TRUE;

	    // it's loaded or used in a window, fail
	    ఒకవేళ (obuf->b_ml.ml_mfp != ఖాళీ || లోపల_use)
	    {
		ఒకవేళ (సందేశం_2)
		    emsg(_(e_బఫరు_with_this_name_already_exists));
		vim_free(ffname);
		తిరిగి FAIL;
	    }
	    // delete from the list
	    close_బఫరు(ఖాళీ, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	ఒకవేళ (ffname == ఖాళీ || sfname == ఖాళీ)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    తిరిగి FAIL;
	}
#నిర్వచితమైతే USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#అయితే_ముగింపు
	ఒకవేళ (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#నిర్వచితమైతే UNIX
    ఒకవేళ (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    లేకపోతే
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#అయితే_ముగింపు

    buf->b_shortname = false;

    buf_name_మారింది(buf);
    తిరిగి OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    శూన్య
buf_సమితి_name(పూర్ణాంకం fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    ఒకవేళ (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = ఖాళీ;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    శూన్య
buf_name_మారింది(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    ఒకవేళ (buf->b_ml.ml_mfp != ఖాళీ)
	ml_setname(buf);

#నిర్వచితమైతే FEAT_TERMINAL
    ఒకవేళ (buf->b_పదం != ఖాళీ)
	పదం_clear_స్థితి_పాఠ్యం(buf->b_పదం);
#అయితే_ముగింపు

    ఒకవేళ (curwin->w_బఫరు == buf)
	check_arg_idx(curwin);	// check file name for arg list
    maketitle();		// set window title
    స్థితి_redraw_all();	// status lines need to be redrawn
    fmarks_check_పేర్లు(buf);	// check named file marks
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
    ఒకవేళ (buf != ఖాళీ && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    తిరిగి buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    పూర్ణాంకం		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    ఒకవేళ (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	ఒకవేళ (errmsg)
	    emsg(_(e_no_alternate_file));
	తిరిగి ఖాళీ;
    }
    తిరిగి fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    పూర్ణాంకం
buflist_కూడు(char_u *fname, పూర్ణాంకం flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, ఖాళీ, (linenr_T)0, flags);
    ఒకవేళ (buf != ఖాళీ)
	తిరిగి buf->b_fnum;
    తిరిగి 0;
}

#ఒకవేళ defined(BACKSLASH_లోపల_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    శూన్య
buflist_slash_adjust(శూన్య)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	ఒకవేళ (bp->b_ffname != ఖాళీ)
	    slash_adjust(bp->b_ffname);
	ఒకవేళ (bp->b_sfname != ఖాళీ)
	    slash_adjust(bp->b_sfname);
    }
}
#అయితే_ముగింపు

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    శూన్య
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.నిలువు, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    పూర్ణాంకం
otherfile(char_u *ffname)
{
    తిరిగి otherfile_buf(curbuf, ffname
#నిర్వచితమైతే UNIX
	    , ఖాళీ
#అయితే_ముగింపు
	    );
}

    స్థిర పూర్ణాంకం
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#నిర్వచితమైతే UNIX
    , stat_T		*stp
#అయితే_ముగింపు
    )
{
    // no name is different
    ఒకవేళ (ffname == ఖాళీ || *ffname == NUL || buf->b_ffname == ఖాళీ)
	తిరిగి TRUE;
    ఒకవేళ (fnamecmp(ffname, buf->b_ffname) == 0)
	తిరిగి FALSE;
#నిర్వచితమైతే UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	ఒకవేళ (stp == ఖాళీ)
	{
	    ఒకవేళ (!buf->b_dev_valid || mch_stat((అక్షరం *)ffname, &st) < 0)
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
	ఒకవేళ (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    ఒకవేళ (buf_same_ino(buf, stp))
		తిరిగి FALSE;
	}
    }
#అయితే_ముగింపు
    తిరిగి TRUE;
}

#ఒకవేళ defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    శూన్య
buf_setino(buf_T *buf)
{
    stat_T	st;

    ఒకవేళ (buf->b_fname != ఖాళీ && mch_stat((అక్షరం *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    లేకపోతే
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    స్థిర పూర్ణాంకం
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    తిరిగి (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#అయితే_ముగింపు

/*
 * Print info about the current buffer.
 */
    శూన్య
fileinfo(
    పూర్ణాంకం fullname,	    // when non-zero print full path
    పూర్ణాంకం shorthelp,
    పూర్ణాంకం	dont_truncate)
{
    char_u	*name;
    అక్షరం	*బఫరు;
    size_t	bufferlen = 0;

    బఫరు = alloc(IOSIZE);
    ఒకవేళ (బఫరు == ఖాళీ)
	తిరిగి;

    ఒకవేళ (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(బఫరు, IOSIZE, "buf %d: ", curbuf->b_fnum);

    బఫరు[bufferlen++] = '"';

    name = buf_spname(curbuf);
    ఒకవేళ (name != ఖాళీ)
	bufferlen += vim_snprintf_safelen(బఫరు + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    లేకపోతే
    {
	ఒకవేళ (!fullname && curbuf->b_fname != ఖాళీ)
	    name = curbuf->b_fname;
	లేకపోతే
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : ఖాళీ, name,
	    (char_u *)బఫరు + bufferlen, IOSIZE - (పూర్ణాంకం)bufferlen, TRUE);
    }

    bufferlen += vim_snprintf_safelen(
	బఫరు + bufferlen,
	IOSIZE - bufferlen,
	"\"%s%s%s%s%s%s",
	curbuf_Is_మారింది() ? (shortmess(SHM_MOD)
	    ?  " [+]" : _(" [Modified]")) : " ",
	(curbuf->b_flags & BF_NOTEDITED) && !bt_dontwrite(curbuf)
	    ? _("[Not edited]") : "",
	(curbuf->b_flags & BF_NEW) && !bt_dontwrite(curbuf)
	    ? new_file_సందేశం() : "",
	(curbuf->b_flags & BF_READERR) ? _("[Read errors]") : "", curbuf->b_p_ro
	    ? (shortmess(SHM_RO) ? _("[RO]") : _("[readonly]")) : "",
	(curbuf_Is_మారింది() || (curbuf->b_flags & BF_WRITE_ముసుగు) || curbuf->b_p_ro)
	    ? " " : "");

    ఒకవేళ (curbuf->b_ml.ml_flags & ML_ఖాళీలు)
	bufferlen += vim_snprintf_safelen(బఫరు + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_పంక్తులు_సందేశం));
    లేకపోతే ఒకవేళ (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    బఫరు + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_పంక్తి_లెక్క),
	    (దీర్ఘ)curbuf->b_ml.ml_పంక్తి_లెక్క,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_పంక్తి_లెక్క));
    లేకపోతే
    {
	bufferlen += vim_snprintf_safelen(
	    బఫరు + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (దీర్ఘ)curwin->w_cursor.lnum,
	    (దీర్ఘ)curbuf->b_ml.ml_పంక్తి_లెక్క,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_పంక్తి_లెక్క));

	validate_virtcol();
	bufferlen += నిలువు_print((char_u *)బఫరు + bufferlen, IOSIZE - bufferlen,
		   (పూర్ణాంకం)curwin->w_cursor.నిలువు + 1, (పూర్ణాంకం)curwin->w_virtcol + 1);
    }

    (శూన్య)append_arg_సంఖ్య(curwin, (char_u *)బఫరు + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    ఒకవేళ (dont_truncate)
    {
	పూర్ణాంకం	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	సందేశం_ప్రారంభం();
	n = సందేశం_scroll;
	సందేశం_scroll = TRUE;
	సందేశం(బఫరు);
	సందేశం_scroll = n;
    }
    లేకపోతే
    {
	అక్షరం	*p = సందేశం_trunc_attr(బఫరు, FALSE, 0);
	ఒకవేళ (restart_edit != 0 || (సందేశం_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    సమితి_keep_సందేశం((char_u *)p, 0);
    }

    vim_free(బఫరు);
}

    పూర్ణాంకం
నిలువు_print(
    char_u  *buf,
    size_t  buflen,
    పూర్ణాంకం	    నిలువు,
    పూర్ణాంకం	    vcol)
{
    ఒకవేళ (నిలువు == vcol)
	తిరిగి (పూర్ణాంకం)vim_snprintf_safelen((అక్షరం *)buf, buflen, "%d", నిలువు);

    తిరిగి (పూర్ణాంకం)vim_snprintf_safelen((అక్షరం *)buf, buflen, "%d-%d", నిలువు, vcol);
}

స్థిర char_u *lasttitle = ఖాళీ;
స్థిర char_u *lasticon = ఖాళీ;

/*
 * Put the file name in the title bar and icon of the window.
 */
    శూన్య
maketitle(శూన్య)
{
    char_u	*title_str = ఖాళీ;
    char_u	*icon_str = ఖాళీ;
    పూర్ణాంకం		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    ఒకవేళ (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	తిరిగి;
    }

    need_maketitle = FALSE;
    ఒకవేళ (!p_title && !p_icon && lasttitle == ఖాళీ && lasticon == ఖాళీ)
	తిరిగి;  // nothing to do

    ఒకవేళ (p_title)
    {
	పూర్ణాంకం maxlen = 0;

	ఒకవేళ (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    ఒకవేళ (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	ఒకవేళ (*p_titlestring != NUL)
	{
#నిర్వచితమైతే FEAT_STL_OPT
	    ఒకవేళ (stl_syntax & STL_లోపల_TITLE)
		build_stl_str_hl(curwin, title_str, పరిమాణం(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, ఖాళీ, ఖాళీ, ఖాళీ);
	    లేకపోతే
#అయితే_ముగింపు
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	లేకపోతే
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#నిర్వచించు SPACE_FOR_FNAME (పరిమాణం(buf) - 100)
#నిర్వచించు SPACE_FOR_DIR   (పరిమాణం(buf) - 20)
#నిర్వచించు SPACE_FOR_ARGNR (పరిమాణం(buf) - 10)  // at least room for " - VIM"

	    // file name
	    ఒకవేళ (curbuf->b_fname == ఖాళీ)
		buflen = vim_snprintf_safelen((అక్షరం *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#నిర్వచితమైతే FEAT_TERMINAL
	    లేకపోతే ఒకవేళ (curbuf->b_పదం != ఖాళీ)
		buflen = vim_snprintf_safelen((అక్షరం *)buf,
		    SPACE_FOR_FNAME, "%s",
		    పదం_get_స్థితి_పాఠ్యం(curbuf->b_పదం));
#అయితే_ముగింపు
	    లేకపోతే
	    {
		buflen = vim_snprintf_safelen((అక్షరం *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != ఖాళీ)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#నిర్వచితమైతే FEAT_TERMINAL
	    ఒకవేళ (curbuf->b_పదం == ఖాళీ)
#అయితే_ముగింపు
	    {
		ఎంపిక (buf_Is_మారింది(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    స్థితి 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (అక్షరం *)buf + buflen,
			    పరిమాణం(buf) - buflen, " +");
			ఆపు;
		    స్థితి 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (అక్షరం *)buf + buflen,
			    పరిమాణం(buf) - buflen, " =");
			ఆపు;
		    స్థితి 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (అక్షరం *)buf + buflen,
			    పరిమాణం(buf) - buflen, " =+");
			ఆపు;
		    స్థితి 4:
		    స్థితి 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (అక్షరం *)buf + buflen,
			    పరిమాణం(buf) - buflen, " -");
			ఆపు;
		    స్థితి 5:
		    స్థితి 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (అక్షరం *)buf + buflen,
			    పరిమాణం(buf) - buflen, " -+");
			ఆపు;
		    అప్రమేయ:
			ఆపు;
		}
	    }

	    // path (surrounded by '()')
	    ఒకవేళ (curbuf->b_fname != ఖాళీ
#నిర్వచితమైతే FEAT_TERMINAL
		    && curbuf->b_పదం == ఖాళీ
#అయితే_ముగింపు
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((అక్షరం *)buf + buflen,
		    పరిమాణం(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (పూర్ణాంకం)(SPACE_FOR_DIR - buflen), TRUE);

#నిర్వచితమైతే BACKSLASH_లోపల_FILENAME
		// avoid "c:/name" to be reduced to "c"
		ఒకవేళ (సురక్షిత_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#అయితే_ముగింపు

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		ఒకవేళ (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((అక్షరం *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		లేకపోతే
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    ఒకవేళ (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((అక్షరం *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != ఖాళీ)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    లేకపోతే
			buflen += vim_snprintf_safelen((అక్షరం *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_సంఖ్య(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((అక్షరం *)buf + buflen,
		పరిమాణం(buf) - buflen, " - %s",
#ఒకవేళ defined(FEAT_CLIENTSERVER)
		(serverName != ఖాళీ)
		    ? serverName :
#అయితే_ముగింపు
		    (char_u *)"VIM");

	    ఒకవేళ (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		ఒకవేళ (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, పరిమాణం(buf));
	    }
	}
    }
    mustset = విలువ_మారింది(title_str, &lasttitle);

    ఒకవేళ (p_icon)
    {
	icon_str = buf;
	ఒకవేళ (*p_iconstring != NUL)
	{
#నిర్వచితమైతే FEAT_STL_OPT
	    ఒకవేళ (stl_syntax & STL_లోపల_ICON)
		build_stl_str_hl(curwin, icon_str, పరిమాణం(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, ఖాళీ, ఖాళీ,
				 ఖాళీ);
	    లేకపోతే
#అయితే_ముగింపు
		icon_str = p_iconstring;
	}
	లేకపోతే
	{
	    char_u  *name;
	    పూర్ణాంకం	    namelen;

	    name = buf_spname(curbuf);
	    ఒకవేళ (name == ఖాళీ)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (పూర్ణాంకం)STRLEN(name);
	    ఒకవేళ (namelen > 100)
	    {
		namelen -= 100;
		ఒకవేళ (has_mbyte)
		    namelen += (*mb_తోక_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, పరిమాణం(buf));
	}
    }

    mustset |= విలువ_మారింది(icon_str, &lasticon);

    ఒకవేళ (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    స్థిర పూర్ణాంకం
విలువ_మారింది(char_u *str, char_u **చివరి)
{
    ఒకవేళ ((str == ఖాళీ) != (*చివరి == ఖాళీ)
	    || (str != ఖాళీ && *చివరి != ఖాళీ && STRCMP(str, *చివరి) != 0))
    {
	vim_free(*చివరి);
	ఒకవేళ (str == ఖాళీ)
	{
	    *చివరి = ఖాళీ;
	    mch_restore_title(
		  చివరి == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	లేకపోతే
	{
	    *చివరి = vim_strsave(str);
	    తిరిగి TRUE;
	}
    }
    తిరిగి FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    శూన్య
resettitle(శూన్య)
{
    mch_settitle(lasttitle, lasticon);
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
free_titles(శూన్య)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#అయితే_ముగింపు


#ఒకవేళ defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
రకనిర్వచనం నిర్మాణం
{
    char_u	*stl_ప్రారంభం;
    పూర్ణాంకం		stl_minwid;
    పూర్ణాంకం		stl_maxwid;
    గణన {
	సాధారణ,
	ఖాళీలు,
	సమూహం_2,
	Separate,
	Highlight,
	Tab_పుట,
	ClickFunc,
	Trunc
    }		stl_type;
    char_u	*stl_clickfunc;	// function name for ClickFunc items
} stl_అంశం_T;

స్థిర size_t		stl_వస్తువులు_len = 20; // Initial value, grows as needed.
స్థిర stl_అంశం_T      *stl_వస్తువులు = ఖాళీ;
స్థిర పూర్ణాంకం	       *stl_groupitem = ఖాళీ;
స్థిర stl_hlrec_T     *stl_hltab = ఖాళీ;
స్థిర stl_hlrec_T     *stl_tabtab = ఖాళీ;
స్థిర stl_clickrec_T  *stl_clicktab = ఖాళీ;
స్థిర పూర్ణాంకం		*stl_separator_locations = ఖాళీ;

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
    పూర్ణాంకం
build_stl_str_hl(
    win_T	*wp,
    char_u	*బహిర్గతం,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    పూర్ణాంకం		opt_scope,	// scope for "opt_name"
    పూర్ణాంకం		fillchar,
    పూర్ణాంకం		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    తిరిగి build_stl_str_hl_local(STL_బహుళకం_SINGLE, wp, బహిర్గతం, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ఖాళీ, ఖాళీ);
}

    పూర్ణాంకం
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*బహిర్గతం,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    పూర్ణాంకం		opt_scope,	// scope for "opt_name"
    పూర్ణాంకం		fillchar,
    పూర్ణాంకం		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    పూర్ణాంకం		*వాహకం_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    తిరిగి build_stl_str_hl_local(STL_బహుళకం_MULTI, wp, బహిర్గతం, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ఖాళీ, వాహకం_hl);
}

# ifdef ENABLE_STL_బహుళకం_MULTI_NL
    పూర్ణాంకం
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*బహిర్గతం,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    పూర్ణాంకం		opt_scope,	// scope for "opt_name"
    పూర్ణాంకం		fillchar,
    పూర్ణాంకం		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    పూర్ణాంకం		*వాహకం_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    తిరిగి build_stl_str_hl_local(STL_బహుళకం_MULTI_NL, wp, బహిర్గతం, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ఖాళీ, వాహకం_hl);
}
# endif

    పూర్ణాంకం
get_stl_rendered_ఎత్తు(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    పూర్ణాంకం		opt_scope)	// scope for "opt_name"
{
    పూర్ణాంకం rendered_ఎత్తు = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (శూన్య)build_stl_str_hl_local(STL_బహుళకం_GET_RENDERED_ఎత్తు,
	    wp, buf, పరిమాణం(buf), &fmt,
	    opt_name, opt_scope, 0, 0, ఖాళీ, ఖాళీ, ఖాళీ, &rendered_ఎత్తు,
	    ఖాళీ);
    --emsg_off;
    తిరిగి rendered_ఎత్తు;
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
స్థిర పూర్ణాంకం
build_stl_str_hl_local(
    stl_బహుళకం_T	బహుళకం,
    win_T	*wp,
    char_u	*బహిర్గతం,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    పూర్ణాంకం		opt_scope,	// scope for "opt_name"
    పూర్ణాంకం		fillchar,
    పూర్ణాంకం		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    పూర్ణాంకం		*rendered_ఎత్తు,   // return: stl rendered height (can be NULL)
    పూర్ణాంకం		*వాహకం_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    పూర్ణాంకం		byteval;
# ifdef FEAT_EVAL
    పూర్ణాంకం		use_sandbox;
    పూర్ణాంకం		save_VIsual_చురుకైన;
# endif
    పూర్ణాంకం		ఖాళీలు_పంక్తి;
    దీర్ఘ	l;
    దీర్ఘ	n;
    పూర్ణాంకం		prevchar_isflag;
    పూర్ణాంకం		prevchar_isitem;
    పూర్ణాంకం		itemisflag;
    పూర్ణాంకం		fillable;
    char_u	*str;
    దీర్ఘ	అంకె;
    పూర్ణాంకం		వెడల్పు;
    పూర్ణాంకం		itemcnt;
    పూర్ణాంకం		curitem;
    పూర్ణాంకం		groupdepth;
# ifdef FEAT_EVAL
    పూర్ణాంకం		evaldepth;
# endif
    పూర్ణాంకం		minwid;
    పూర్ణాంకం		maxwid;
    పూర్ణాంకం		zeropad;
    char_u	పునాది;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    పూర్ణాంకం		save_redraw_not_allowed = redraw_not_allowed;
    పూర్ణాంకం		save_కీలకం_Typed = కీలకం_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    పూర్ణాంకం		did_emsg_before = did_emsg;
    పూర్ణాంకం		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    ఒకవేళ (updating_screen)
	redraw_not_allowed = TRUE;

    ఒకవేళ (stl_వస్తువులు == ఖాళీ)
    {
	stl_వస్తువులు = ALLOC_MULT(stl_అంశం_T, stl_వస్తువులు_len);
	stl_groupitem = ALLOC_MULT(పూర్ణాంకం, stl_వస్తువులు_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_వస్తువులు_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_వస్తువులు_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_వస్తువులు_len + 1);

	stl_separator_locations = ALLOC_MULT(పూర్ణాంకం, stl_వస్తువులు_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_సమితి_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    ఒకవేళ (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_సంఖ్య;
	tv.vval.v_సంఖ్య = wp->w_id;
	సమితి_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_గమ్యం_string_సురక్షిత(fmt + 2, use_sandbox, FALSE, FALSE);
	ఒకవేళ (usefmt == ఖాళీ)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    ఒకవేళ (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    ఒకవేళ (lnum > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
    {
	lnum = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_బఫరు, lnum, FALSE);
    ఖాళీలు_పంక్తి = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_బఫరు, lnum);
    ఒకవేళ (wp->w_cursor.నిలువు > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.నిలువు = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    లేకపోతే
	byteval = (*mb_ptr2char)(p + wp->w_cursor.నిలువు);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = బహిర్గతం;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    ఒకవేళ (వాహకం_hl != ఖాళీ && *వాహకం_hl != 0)
    {
	stl_వస్తువులు[curitem].stl_type = Highlight;
	stl_వస్తువులు[curitem].stl_ప్రారంభం = p;
	stl_వస్తువులు[curitem].stl_minwid = *వాహకం_hl;
	stl_వస్తువులు[curitem].stl_clickfunc = ఖాళీ;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    కొరకు (s = usefmt; *s != NUL; )
    {
	ఒకవేళ (curitem == (పూర్ణాంకం)stl_వస్తువులు_len)
	{
	    size_t	new_len = stl_వస్తువులు_len * 3 / 2;

	    stl_అంశం_T *new_వస్తువులు =
			  vim_realloc(stl_వస్తువులు, పరిమాణం(stl_అంశం_T) * new_len);
	    ఒకవేళ (new_వస్తువులు == ఖాళీ)
		ఆపు;
	    stl_వస్తువులు = new_వస్తువులు;

	    పూర్ణాంకం *new_groupitem =
			     vim_realloc(stl_groupitem, పరిమాణం(పూర్ణాంకం) * new_len);
	    ఒకవేళ (new_groupitem == ఖాళీ)
		ఆపు;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  పరిమాణం(stl_hlrec_T) * (new_len + 1));
	    ఒకవేళ (new_hlrec == ఖాళీ)
		ఆపు;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  పరిమాణం(stl_hlrec_T) * (new_len + 1));
	    ఒకవేళ (new_hlrec == ఖాళీ)
		ఆపు;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      పరిమాణం(stl_clickrec_T) * (new_len + 1));
	    ఒకవేళ (new_clickrec == ఖాళీ)
		ఆపు;
	    stl_clicktab = new_clickrec;

	    పూర్ణాంకం *new_separator_locs = vim_realloc(stl_separator_locations,
					    పరిమాణం(పూర్ణాంకం) * new_len);
	    ఒకవేళ (new_separator_locs == ఖాళీ)
		ఆపు;
	    stl_separator_locations = new_separator_locs;

	    stl_వస్తువులు_len = new_len;
	}

	stl_వస్తువులు[curitem].stl_clickfunc = ఖాళీ;

	ఒకవేళ (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	అయితే (*s != NUL && *s != '%' && p + 1 < బహిర్గతం + outlen)
# ifdef ENABLE_STL_బహుళకం_MULTI_NL
	{
	    ఒకవేళ (*s == '\n' || *s == '\r')
	    {
		ఒకవేళ (బహుళకం == STL_బహుళకం_MULTI_NL)
		{
		    s++;
		    వెళ్ళు కనుగొను_linebreak;
		}
		లేకపోతే ఒకవేళ (బహుళకం == STL_బహుళకం_GET_RENDERED_ఎత్తు)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_బహుళకం_MULTI_NL
	}
# endif
	ఒకవేళ (*s == NUL || p + 1 >= బహిర్గతం + outlen)
	    ఆపు;

	/*
	 * Handle one '%' item.
	 */
	s++;
	ఒకవేళ (*s == NUL)  // ignore trailing %
	    ఆపు;

	ఒకవేళ (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    ఒకవేళ (s[1] == ']')
	    {
		stl_వస్తువులు[curitem].stl_type = ClickFunc;
		stl_వస్తువులు[curitem].stl_ప్రారంభం = p;
		stl_వస్తువులు[curitem].stl_minwid = 0;
		stl_వస్తువులు[curitem].stl_clickfunc = ఖాళీ;
		s += 2;
		curitem++;
		కొనసాగు;
	    }
	    // %[FuncName] - start click region
	    ఒకవేళ (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		ఒకవేళ (rb != ఖాళీ)
		{
		    stl_వస్తువులు[curitem].stl_type = ClickFunc;
		    stl_వస్తువులు[curitem].stl_ప్రారంభం = p;
		    stl_వస్తువులు[curitem].stl_minwid = 0;
		    stl_వస్తువులు[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    కొనసాగు;
		}
	    }
	}
	ఒకవేళ (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    ఒకవేళ (బహుళకం == STL_బహుళకం_MULTI
# ifdef ENABLE_STL_బహుళకం_MULTI_NL
		    || బహుళకం == STL_బహుళకం_MULTI_NL
# endif
	       )
	    {
		s++;
		ఆపు;
	    }
	    లేకపోతే ఒకవేళ (బహుళకం == STL_బహుళకం_GET_RENDERED_ఎత్తు)
		rheight++;
	}
	ఒకవేళ (*s == '%')
	{
	    ఒకవేళ (p + 1 >= బహిర్గతం + outlen)
		ఆపు;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    కొనసాగు;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	ఒకవేళ (*s == STL_SEPARATE)
	{
	    s++;
	    ఒకవేళ (groupdepth > 0)
		కొనసాగు;
	    stl_వస్తువులు[curitem].stl_type = Separate;
	    stl_వస్తువులు[curitem++].stl_ప్రారంభం = p;
	    కొనసాగు;
	}
	ఒకవేళ (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_వస్తువులు[curitem].stl_type = Trunc;
	    stl_వస్తువులు[curitem++].stl_ప్రారంభం = p;
	    కొనసాగు;
	}
	ఒకవేళ (*s == ')')
	{
	    char_u  *t;

	    s++;
	    ఒకవేళ (groupdepth < 1)
		కొనసాగు;
	    groupdepth--;

	    t = stl_వస్తువులు[stl_groupitem[groupdepth]].stl_ప్రారంభం;
	    *p = NUL;
	    l = vim_strsize(t);
	    ఒకవేళ (curitem > stl_groupitem[groupdepth] + 1
		    && stl_వస్తువులు[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		పూర్ణాంకం సమూహం_ప్రారంభం_userhl = 0;
		పూర్ణాంకం సమూహం_చివర_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		కొరకు (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    ఒకవేళ (stl_వస్తువులు[n].stl_type == Highlight)
		    {
			సమూహం_ప్రారంభం_userhl = సమూహం_చివర_userhl =
						       stl_వస్తువులు[n].stl_minwid;
			ఆపు;
		    }
		}
		కొరకు (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    ఒకవేళ (stl_వస్తువులు[n].stl_type == సాధారణ)
			ఆపు;
		    ఒకవేళ (stl_వస్తువులు[n].stl_type == Highlight)
			సమూహం_చివర_userhl = stl_వస్తువులు[n].stl_minwid;
		}
		ఒకవేళ (n == curitem && సమూహం_ప్రారంభం_userhl == సమూహం_చివర_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    కొరకు (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			ఒకవేళ (stl_వస్తువులు[n].stl_type == Highlight)
			    stl_వస్తువులు[n].stl_type = ఖాళీలు;
			// adjust the start position of TabPage to the next
			// item position
			ఒకవేళ (stl_వస్తువులు[n].stl_type == Tab_పుట)
			    stl_వస్తువులు[n].stl_ప్రారంభం = p;
		    }
		}
	    }
	    ఒకవేళ (l > stl_వస్తువులు[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		ఒకవేళ (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    అయితే (l >= stl_వస్తువులు[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		లేకపోతే
		    n = (దీర్ఘ)(p - t) - stl_వస్తువులు[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		పూర్ణాంకం minwid_fixed = MIN(stl_వస్తువులు[stl_groupitem[groupdepth]].stl_minwid,
				       stl_వస్తువులు[stl_groupitem[groupdepth]].stl_maxwid);
		అయితే (++l < minwid_fixed)
		    MB_CHAR2_బైట్లు(fillchar, p);

		// correct the start of the items for the truncation
		కొరకు (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_వస్తువులు[l].stl_ప్రారంభం -= n - 1;
		    ఒకవేళ (stl_వస్తువులు[l].stl_ప్రారంభం < t)
			stl_వస్తువులు[l].stl_ప్రారంభం = t;
		}
	    }
	    లేకపోతే ఒకవేళ (నిరపేక్ష(stl_వస్తువులు[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_వస్తువులు[stl_groupitem[groupdepth]].stl_minwid;
		పూర్ణాంకం fillchar_len = MB_CHAR2LEN(fillchar);
		ఒకవేళ (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    అయితే (l++ < n && p + fillchar_len < బహిర్గతం + outlen)
			MB_CHAR2_బైట్లు(fillchar, p);
		}
		లేకపోతే
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    ఒకవేళ (p + l >= బహిర్గతం + outlen)
		    {
			n = (దీర్ఘ)((బహిర్గతం + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    కొరకు ( ; n > 0; n--)
			MB_CHAR2_బైట్లు(fillchar, t);
		    కొరకు (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_వస్తువులు[n].stl_ప్రారంభం += l;
		}
	    }
	    కొనసాగు;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	ఒకవేళ (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	ఒకవేళ (*s == '-')
	{
	    s++;
	    l = -1;
	}
	ఒకవేళ (VIM_ISDIGIT(*s))
	{
	    minwid = (పూర్ణాంకం)getdigits(&s);
	    ఒకవేళ (minwid < 0)	//అధికప్రవాహం
		minwid = 0;
	}
	ఒకవేళ (*s == STL_USER_HL)
	{
	    stl_వస్తువులు[curitem].stl_type = Highlight;
	    stl_వస్తువులు[curitem].stl_ప్రారంభం = p;
	    stl_వస్తువులు[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    కొనసాగు;
	}
	ఒకవేళ (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    ఒకవేళ (*s == STL_TABCLOSENR)
	    {
		ఒకవేళ (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    కొరకు (n = curitem - 1; n >= 0; --n)
			ఒకవేళ (stl_వస్తువులు[n].stl_type == Tab_పుట
					       && stl_వస్తువులు[n].stl_minwid >= 0)
			{
			    minwid = stl_వస్తువులు[n].stl_minwid;
			    ఆపు;
			}
		}
		లేకపోతే
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_వస్తువులు[curitem].stl_type = Tab_పుట;
	    stl_వస్తువులు[curitem].stl_ప్రారంభం = p;
	    stl_వస్తువులు[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    కొనసాగు;
	}
	ఒకవేళ (*s == '.')
	{
	    s++;
	    ఒకవేళ (VIM_ISDIGIT(*s))
	    {
		maxwid = (పూర్ణాంకం)getdigits(&s);
		ఒకవేళ (maxwid <= 0)	//అధికప్రవాహం
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	పూర్ణాంకం raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	ఒకవేళ (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_వస్తువులు[curitem].stl_type = సమూహం_2;
	    stl_వస్తువులు[curitem].stl_ప్రారంభం = p;
	    stl_వస్తువులు[curitem].stl_minwid = minwid;
	    stl_వస్తువులు[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    కొనసాగు;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	ఒకవేళ (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    కొనసాగు;
	}
# endif
	ఒకవేళ (vim_strchr(STL_ALL, *s) == ఖాళీ)
	{
	    ఒకవేళ (*s == NUL)  // can happen with "%0"
		ఆపు;
	    s++;
	    కొనసాగు;
	}
	opt = *s++;

	// OK - now for the real work
	పునాది = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	అంకె = -1;
	str = ఖాళీ;
	ఎంపిక (opt)
	{
	స్థితి STL_FILEPATH:
	స్థితి STL_FULLPATH:
	స్థితి STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_బఫరు);
	    ఒకవేళ (name != ఖాళీ)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    లేకపోతే
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_బఫరు->b_ffname
					  : wp->w_బఫరు->b_fname;
		home_replace(wp->w_బఫరు, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    ఒకవేళ (opt != STL_FILENAME)
		str = NameBuff;
	    లేకపోతే
		str = gettail(NameBuff);
	    ఆపు;
	}

	స్థితి STL_VIM_సమాసం: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *ఖండం_ప్రారంభం = s - 1;
# endif
	    పూర్ణాంకం	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    ఒకవేళ (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    అయితే ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < బహిర్గతం + outlen)
		*p++ = *s++;
	    ఒకవేళ (*s != '}')	// missing '}' or out of space
		ఆపు;
	    s++;
	    ఒకవేళ (reevaluate && p > బహిర్గతం)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    లేకపోతే
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((అక్షరం *)buf_tmp, పరిమాణం(buf_tmp),
							 "%d", curbuf->b_fnum);
	    సమితి_అంతర్గత_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((అక్షరం *)buf_tmp, పరిమాణం(buf_tmp), "%d", curwin->w_id);
	    సమితి_అంతర్గత_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_చురుకైన = VIsual_చురుకైన;
	    curwin = wp;
	    curbuf = wp->w_బఫరు;
	    // Visual mode is only valid in the current window.
	    ఒకవేళ (curwin != save_curwin)
		VIsual_చురుకైన = FALSE;

	    str = eval_గమ్యం_string_సురక్షిత(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_చురుకైన = save_VIsual_చురుకైన;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    ఒకవేళ (!zeropad && str != ఖాళీ && *str != NUL)
	    {
		ఒకవేళ (*skipdigits(str) == NUL)
		{
		    అంకె = atoi((అక్షరం *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    ఒకవేళ (reevaluate && str != ఖాళీ && *str != NUL
		    && strchr((స్థిరరాశి అక్షరం *)str, '%') != ఖాళీ
		    && evaldepth < MAX_STL_EVAL_లోతు)
	    {
		size_t parsed_usefmt = (size_t)(ఖండం_ప్రారంభం - usefmt);
		size_t str_నిడివి = అక్షరశ్రేణి_పొడవు((స్థిరరాశి అక్షరం *)str);
		size_t fmt_నిడివి = అక్షరశ్రేణి_పొడవు((స్థిరరాశి అక్షరం *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_నిడివి + fmt_నిడివి + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * పరిమాణం(char_u));

		ఒకవేళ (new_fmt != ఖాళీ)
		{
		    char_u *new_fmt_p = new_fmt;

		    new_fmt_p = (char_u *)జ్ఞాపకం_నకలు(new_fmt_p, usefmt, parsed_usefmt)
								   + parsed_usefmt;
		    new_fmt_p = (char_u *)జ్ఞాపకం_నకలు(new_fmt_p , str, str_నిడివి)
								      + str_నిడివి;
		    new_fmt_p = (char_u *)జ్ఞాపకం_నకలు(new_fmt_p, "%}", 2) + 2;
		    new_fmt_p = (char_u *)జ్ఞాపకం_నకలు(new_fmt_p , s, fmt_నిడివి)
								      + fmt_నిడివి;
		    *new_fmt_p = 0;
		    new_fmt_p = ఖాళీ;

		    ఒకవేళ (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    కొనసాగు;
		}
	    }
# endif
	    ఆపు;
	}
	స్థితి STL_పంక్తి:
	    అంకె = (wp->w_బఫరు->b_ml.ml_flags & ML_ఖాళీలు)
		  ? 0L : (దీర్ఘ)(wp->w_cursor.lnum);
	    ఆపు;

	స్థితి STL_NUMLINES:
	    అంకె = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
	    ఆపు;

	స్థితి STL_స్తంభం:
	    అంకె = (State & బహుళకం_చొప్పించు) == 0 && ఖాళీలు_పంక్తి
					       ? 0 : (పూర్ణాంకం)wp->w_cursor.నిలువు + 1;
	    ఆపు;

	స్థితి STL_VIRTCOL:
	స్థితి STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    ఒకవేళ (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & బహుళకం_చొప్పించు) == 0
			       && ఖాళీలు_పంక్తి ? 0 : (పూర్ణాంకం)wp->w_cursor.నిలువు + 1)))
		ఆపు;
	    అంకె = (దీర్ఘ)virtcol;
	    ఆపు;
	}

	స్థితి STL_PERCENTAGE:
	    అంకె = calc_percentage((దీర్ఘ)wp->w_cursor.lnum, (దీర్ఘ)wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క);
	    ఆపు;

	స్థితి STL_ALTPERCENT:
	    str = buf_tmp;
	    (శూన్య)get_rel_pos(wp, str, TMPLEN);
	    ఆపు;

	స్థితి STL_SHOWCMD:
	    ఒకవేళ (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    ఆపు;

	స్థితి STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    ఒకవేళ (append_arg_సంఖ్య(wp, buf_tmp, పరిమాణం(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    ఆపు;

	స్థితి STL_KEYMAP:
	    fillable = FALSE;
	    ఒకవేళ (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    ఆపు;
	స్థితి STL_PAGENUM:
# ఒకవేళ defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    అంకె = printer_పుట_అంకె;
# లేకపోతే
	    అంకె = 0;
# endif
	    ఆపు;

	స్థితి STL_BUFNO:
	    అంకె = wp->w_బఫరు->b_fnum;
	    ఆపు;

	స్థితి STL_స్థానభ్రంశం_X:
	    పునాది = 'X';
	    // FALLTHROUGH
	స్థితి STL_స్థానభ్రంశం:
# ifdef FEAT_BYTEOFF
	    l = ml_కనుగొను_పంక్తి_or_స్థానభ్రంశం(wp->w_బఫరు, wp->w_cursor.lnum, ఖాళీ);
	    అంకె = (wp->w_బఫరు->b_ml.ml_flags & ML_ఖాళీలు) || l < 0
		       ? 0L : l + 1 + ((State & బహుళకం_చొప్పించు) == 0 && ఖాళీలు_పంక్తి
				? 0 : (పూర్ణాంకం)wp->w_cursor.నిలువు);
# endif
	    ఆపు;

	స్థితి STL_BYTEVAL_X:
	    పునాది = 'X';
	    // FALLTHROUGH
	స్థితి STL_BYTEVAL:
	    అంకె = byteval;
	    ఒకవేళ (అంకె == NL)
		అంకె = 0;
	    లేకపోతే ఒకవేళ (అంకె == CAR && get_fileformat(wp->w_బఫరు) == EOL_MAC)
		అంకె = NL;
	    ఆపు;

	స్థితి STL_ROFLAG:
	స్థితి STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    ఒకవేళ (wp->w_బఫరు->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    ఆపు;

	స్థితి STL_HELPFLAG:
	స్థితి STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    ఒకవేళ (wp->w_బఫరు->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    ఆపు;

	స్థితి STL_FILETYPE:
	    ఒకవేళ (*wp->w_బఫరు->b_p_ft != NUL
		    && STRLEN(wp->w_బఫరు->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((అక్షరం *)buf_tmp, పరిమాణం(buf_tmp), "[%s]",
							wp->w_బఫరు->b_p_ft);
		str = buf_tmp;
	    }
	    ఆపు;

	స్థితి STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    ఒకవేళ (*wp->w_బఫరు->b_p_ft != NUL
		    && STRLEN(wp->w_బఫరు->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((అక్షరం *)buf_tmp, పరిమాణం(buf_tmp), ",%s",
							wp->w_బఫరు->b_p_ft);
		కొరకు (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    ఆపు;

# ఒకవేళ defined(FEAT_QUICKFIX)
	స్థితి STL_PREVIEWFLAG:
	స్థితి STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    ఒకవేళ (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    ఆపు;

	స్థితి STL_QUICKFIX:
	    ఒకవేళ (bt_quickfix(wp->w_బఫరు))
		str = (char_u *)(wp->w_llist_ref
			    ? _(సందేశం_loclist)
			    : _(సందేశం_qflist));
	    ఆపు;
# endif

	స్థితి STL_MODIFIED:
	స్థితి STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    ఎంపిక ((opt == STL_MODIFIED_ALT)
		    + buf_Is_మారింది(wp->w_బఫరు) * 2
		    + (!wp->w_బఫరు->b_p_ma) * 4)
	    {
		స్థితి 2: str = (char_u *)"[+]"; ఆపు;
		స్థితి 3: str = (char_u *)",+"; ఆపు;
		స్థితి 4: str = (char_u *)"[-]"; ఆపు;
		స్థితి 5: str = (char_u *)",-"; ఆపు;
		స్థితి 6: str = (char_u *)"[+-]"; ఆపు;
		స్థితి 7: str = (char_u *)",+-"; ఆపు;
	    }
	    ఆపు;

	స్థితి STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		అయితే (*s != '#' && *s != NUL)
		    ++s;
		ఒకవేళ (*s == '#')
		{
		    stl_వస్తువులు[curitem].stl_type = Highlight;
		    stl_వస్తువులు[curitem].stl_ప్రారంభం = p;
		    stl_వస్తువులు[curitem].stl_minwid = -syn_namen2id(t, (పూర్ణాంకం)(s - t));
		    curitem++;
		}
		ఒకవేళ (*s != NUL)
		    ++s;
		కొనసాగు;
	    }

	స్థితి STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    ఒకవేళ (*s == ']')
	    {
		stl_వస్తువులు[curitem].stl_type = ClickFunc;
		stl_వస్తువులు[curitem].stl_ప్రారంభం = p;
		stl_వస్తువులు[curitem].stl_minwid = 0;
		stl_వస్తువులు[curitem].stl_clickfunc = ఖాళీ;
		s++;
		curitem++;
		కొనసాగు;
	    }
	    // %N[FuncName] with minwid
	    ఒకవేళ (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		ఒకవేళ (rb != ఖాళీ)
		{
		    stl_వస్తువులు[curitem].stl_type = ClickFunc;
		    stl_వస్తువులు[curitem].stl_ప్రారంభం = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_వస్తువులు[curitem].stl_minwid = raw_minwid;
		    stl_వస్తువులు[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    కొనసాగు;
		}
	    }
	    కొనసాగు;

	స్థితి STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    కొనసాగు;
	}

	stl_వస్తువులు[curitem].stl_ప్రారంభం = p;
	stl_వస్తువులు[curitem].stl_type = సాధారణ;
	ఒకవేళ (str != ఖాళీ && *str)
	{
	    char_u  *t = str;

	    ఒకవేళ (itemisflag)
	    {
		ఒకవేళ ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    ఒకవేళ (l > 0)
		prevchar_isitem = TRUE;
	    ఒకవేళ (l > maxwid)
	    {
		అయితే (l >= maxwid)
		    ఒకవేళ (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    లేకపోతే
			l -= byte2cells(*t++);
		ఒకవేళ (p + 1 >= బహిర్గతం + outlen)
		    ఆపు;
		*p++ = '<';
	    }
	    ఒకవేళ (minwid > 0)
	    {
		కొరకు (; l < minwid && p + 1 < బహిర్గతం + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    ఒకవేళ (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    లేకపోతే
			MB_CHAR2_బైట్లు(fillchar, p);
		}
		minwid = 0;
	    }
	    లేకపోతే
		minwid *= -1;
	    కొరకు (; *t && p + 1 < బహిర్గతం + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		ఒకవేళ (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2_బైట్లు(fillchar, p);
		లేకపోతే
		    *p++ = *t;
	    }
	    కొరకు (; l < minwid && p + 1 < బహిర్గతం + outlen; l++)
		MB_CHAR2_బైట్లు(fillchar, p);
	}
	లేకపోతే ఒకవేళ (అంకె >= 0)
	{
	    పూర్ణాంకం	    nbase = (పునాది == 'D' ? 10 : (పునాది == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    ఒకవేళ (p + 20 >= బహిర్గతం + outlen)
		ఆపు;		// not sufficient space
	    prevchar_isitem = TRUE;
	    ఒకవేళ (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    ఒకవేళ (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? పునాది : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    కొరకు (n = అంకె, l = 1; n >= nbase; n /= nbase)
		l++;
	    ఒకవేళ (opt == STL_VIRTCOL_ALT)
		l++;

	    ఒకవేళ (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		అయితే (l-- > maxwid)
		    అంకె /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((అక్షరం *)p, outlen - (p - బహిర్గతం),
		    (అక్షరం *)nstr, 0, అంకె, n);
	    }
	    లేకపోతే
		p += vim_snprintf_safelen((అక్షరం *)p, outlen - (p - బహిర్గతం),
		    (అక్షరం *)nstr, minwid, అంకె);
	}
	లేకపోతే
	    stl_వస్తువులు[curitem].stl_type = ఖాళీలు;

	ఒకవేళ (అంకె >= 0 || (!itemisflag && str != ఖాళీ && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	ఒకవేళ (opt == STL_VIM_సమాసం)
	    vim_free(str);
	curitem++;
    }
# ifdef ENABLE_STL_బహుళకం_MULTI_NL
కనుగొను_linebreak:
# endif
    *p = NUL;
    outputlen = (size_t)(p - బహిర్గతం);
    itemcnt = curitem;

    // Remember the most recent %# / %* highlight so the next row of a
    // multi-line statusline can resume it.
    ఒకవేళ (వాహకం_hl != ఖాళీ)
    {
	పూర్ణాంకం చివరి_hl = 0;
	కొరకు (l = 0; l < itemcnt; l++)
	    ఒకవేళ (stl_వస్తువులు[l].stl_type == Highlight)
		చివరి_hl = stl_వస్తువులు[l].stl_minwid;
	*వాహకం_hl = చివరి_hl;
    }

    ఒకవేళ (బహుళకం == STL_బహుళకం_MULTI
# ifdef ENABLE_STL_బహుళకం_MULTI_NL
		    || బహుళకం == STL_బహుళకం_MULTI_NL
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
	ఒకవేళ (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = అక్షరశ్రేణి_పొడవు((అక్షరం *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    లేకపోతే
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	ఒకవేళ (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    ఒకవేళ (బహుళకం == STL_బహుళకం_GET_RENDERED_ఎత్తు)
    {
	// Free click function names that were allocated during parsing.
	కొరకు (l = 0; l < itemcnt; l++)
	    ఒకవేళ (stl_వస్తువులు[l].stl_type == ClickFunc)
		vim_free(stl_వస్తువులు[l].stl_clickfunc);
	ఒకవేళ (rendered_ఎత్తు != ఖాళీ)
	    *rendered_ఎత్తు = rheight;
	తిరిగి 0;
    }

    వెడల్పు = vim_strsize(బహిర్గతం);
    ఒకవేళ (maxwidth > 0 && వెడల్పు > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	ఒకవేళ (itemcnt == 0)
	    s = బహిర్గతం;
	లేకపోతే
	{
	    కొరకు ( ; l < itemcnt; l++)
		ఒకవేళ (stl_వస్తువులు[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_వస్తువులు[l].stl_ప్రారంభం;
		    ఆపు;
		}
	    ఒకవేళ (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_వస్తువులు[0].stl_ప్రారంభం;
		l = 0;
	    }
	}

	ఒకవేళ (వెడల్పు - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    ఒకవేళ (has_mbyte)
	    {
		s = బహిర్గతం;
		వెడల్పు = 0;
		కొరకు (;;)
		{
		    వెడల్పు += ptr2cells(s);
		    ఒకవేళ (వెడల్పు >= maxwidth)
			ఆపు;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		అయితే (++వెడల్పు < maxwidth)
		    MB_CHAR2_బైట్లు(fillchar, s);
	    }
	    లేకపోతే
		s = బహిర్గతం + maxwidth - 1;
	    కొరకు (l = 0; l < itemcnt; l++)
		ఒకవేళ (stl_వస్తువులు[l].stl_ప్రారంభం > s)
		    ఆపు;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	లేకపోతే
	{
	    char_u  *చివర = బహిర్గతం + outputlen;

	    n = 0;
	    అయితే (వెడల్పు >= maxwidth)
	    {
		వెడల్పు -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(చివర - p) + 1);	// +1 for NUL
	    చివర -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    కొరకు (; l < itemcnt; l++)
	    {
		ఒకవేళ (stl_వస్తువులు[l].stl_ప్రారంభం - n >= s)
		    stl_వస్తువులు[l].stl_ప్రారంభం -= n;
		లేకపోతే
		    stl_వస్తువులు[l].stl_ప్రారంభం = s;
	    }

	    // Fill up for half a double-wide character.
	    అయితే (++వెడల్పు < maxwidth)
	    {
		s = చివర;
		MB_CHAR2_బైట్లు(fillchar, s);
		*s = NUL;
		చివర = s;
	    }
	}
	వెడల్పు = maxwidth;
    }
    లేకపోతే ఒకవేళ (వెడల్పు < maxwidth &&
	    outputlen + (maxwidth - వెడల్పు) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	పూర్ణాంకం అంకె_separators = 0;

	కొరకు (l = 0; l < itemcnt; l++)
	{
	    ఒకవేళ (stl_వస్తువులు[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[అంకె_separators] = l;
		అంకె_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	ఒకవేళ (అంకె_separators)
	{
	    పూర్ణాంకం standard_spaces;
	    పూర్ణాంకం final_spaces;

	    standard_spaces = (maxwidth - వెడల్పు) / అంకె_separators;
	    final_spaces = (maxwidth - వెడల్పు) -
					standard_spaces * (అంకె_separators - 1);
	    కొరకు (l = 0; l < అంకె_separators; l++)
	    {
		పూర్ణాంకం dislocation = (l == (అంకె_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *ప్రారంభం = stl_వస్తువులు[stl_separator_locations[l]].stl_ప్రారంభం;
		char_u *seploc = ప్రారంభం + dislocation;
		STRMOVE(seploc, ప్రారంభం);
		కొరకు (s = ప్రారంభం; s < seploc;)
		    MB_CHAR2_బైట్లు(fillchar, s);

		కొరకు (పూర్ణాంకం i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_వస్తువులు[i].stl_ప్రారంభం += dislocation;
	    }

	    వెడల్పు = maxwidth;
	}
    }

    // Store the info about highlighting.
    ఒకవేళ (hltab != ఖాళీ)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	కొరకు (l = 0; l < itemcnt; l++)
	{
	    ఒకవేళ (stl_వస్తువులు[l].stl_type == Highlight)
	    {
		sp->ప్రారంభం = stl_వస్తువులు[l].stl_ప్రారంభం;
		sp->userhl = stl_వస్తువులు[l].stl_minwid;
		sp++;
	    }
	}
	sp->ప్రారంభం = ఖాళీ;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    ఒకవేళ (tabtab != ఖాళీ)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	కొరకు (l = 0; l < itemcnt; l++)
	{
	    ఒకవేళ (stl_వస్తువులు[l].stl_type == Tab_పుట)
	    {
		sp->ప్రారంభం = stl_వస్తువులు[l].stl_ప్రారంభం;
		sp->userhl = stl_వస్తువులు[l].stl_minwid;
		sp++;
	    }
	}
	sp->ప్రారంభం = ఖాళీ;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    ఒకవేళ (clicktab != ఖాళీ)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	కొరకు (l = 0; l < itemcnt; l++)
	{
	    ఒకవేళ (stl_వస్తువులు[l].stl_type == ClickFunc)
	    {
		cp->ప్రారంభం = stl_వస్తువులు[l].stl_ప్రారంభం;
		cp->funcname = stl_వస్తువులు[l].stl_clickfunc;
		cp->minwid = stl_వస్తువులు[l].stl_minwid;
		cp++;
	    }
	}
	cp->ప్రారంభం = ఖాళీ;
	cp->funcname = ఖాళీ;
    }
    లేకపోతే
    {
	// Free click function names when caller doesn't need them.
	కొరకు (l = 0; l < itemcnt; l++)
	    ఒకవేళ (stl_వస్తువులు[l].stl_type == ClickFunc)
		vim_free(stl_వస్తువులు[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    కీలకం_Typed = save_కీలకం_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    ఒకవేళ (did_emsg > did_emsg_before)
	సమితి_string_option_ప్రత్యక్ష(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    తిరిగి వెడల్పు;
}
#అయితే_ముగింపు // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    పూర్ణాంకం
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    పూర్ణాంకం		buflen)
{
    దీర్ఘ	above; // number of lines above window
    దీర్ఘ	క్రింద; // number of lines below window

    ఒకవేళ (buflen < 3) // need at least 3 chars for writing
	తిరిగి 0;
    above = wp->w_topline - 1;
#నిర్వచితమైతే FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    ఒకవేళ (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#అయితే_ముగింపు
    క్రింద = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క - wp->w_botline + 1;
    ఒకవేళ (క్రింద <= 0)
	తిరిగి (పూర్ణాంకం)vim_snprintf_safelen((అక్షరం *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    ఒకవేళ (above <= 0)
	తిరిగి (పూర్ణాంకం)vim_snprintf_safelen((అక్షరం *)buf, buflen,
	    "%s", _("Top"));

    పూర్ణాంకం perc = calc_percentage(above, above + క్రింద);
    అక్షరం tmp[8];
    // localized percentage value
    vim_snprintf(tmp, పరిమాణం(tmp), _("%d%%"), perc);
    తిరిగి (పూర్ణాంకం)vim_snprintf_safelen((అక్షరం *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    స్థిర పూర్ణాంకం
append_arg_సంఖ్య(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    పూర్ణాంకం		కూడు_file)	// Add "file" before the arg number
{
    ఒకవేళ (ARGCOUNT <= 1)		// nothing to do
	తిరిగి 0;

    అక్షరం *సందేశం;
    ఎంపిక ((wp->w_arg_idx_invalid ? 1 : 0) + (కూడు_file ? 2 : 0))
    {
	స్థితి 0: సందేశం = _(" (%d of %d)"); ఆపు;
	స్థితి 1: సందేశం = _(" ((%d) of %d)"); ఆపు;
	స్థితి 2: సందేశం = _(" (file %d of %d)"); ఆపు;
	స్థితి 3: సందేశం = _(" (file (%d) of %d)"); ఆపు;
    }

    తిరిగి (పూర్ణాంకం)vim_snprintf_safelen((అక్షరం *)buf, buflen, సందేశం,
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
#నిర్వచితమైతే UNIX
    తిరిగి నిండిన_Name_save(fname, TRUE);
#లేకపోతే
    ఒకవేళ (!vim_isAbsName(fname)
	    || అక్షరశ్రేణి_శోధన((అక్షరం *)fname, "..") != ఖాళీ
	    || అక్షరశ్రేణి_శోధన((అక్షరం *)fname, "//") != ఖాళీ
# ifdef BACKSLASH_లోపల_FILENAME
	    || అక్షరశ్రేణి_శోధన((అక్షరం *)fname, "\\\\") != ఖాళీ
# endif
# ఒకవేళ defined(MSWIN)
	    || vim_strchr(fname, '~') != ఖాళీ
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	తిరిగి నిండిన_Name_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    ఒకవేళ (fname != ఖాళీ)
	fname_case(fname, 0);	// set correct case for file name
# endif

    తిరిగి fname;
#అయితే_ముగింపు
}

/*
 * Make "*ffname" a full file name, set "*sfname" to "*ffname" if not NULL.
 * "*ffname" becomes a pointer to allocated memory (or NULL).
 * When resolving a link both "*sfname" and "*ffname" will point to the same
 * allocated memory.
 * The "*ffname" and "*sfname" pointer values on call will not be freed.
 * Note that the resulting "*ffname" pointer should be considered not allocated.
 */
    శూన్య
fname_expand(
    buf_T	*buf వాడని,
    char_u	**ffname,
    char_u	**sfname)
{
    ఒకవేళ (*ffname == ఖాళీ)	    // no file name given, nothing to do
	తిరిగి;
    ఒకవేళ (*sfname == ఖాళీ)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#నిర్వచితమైతే FEAT_SHORTCUT
    ఒకవేళ (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_మార్గం(*ffname, FALSE);
	ఒకవేళ (rfname != ఖాళీ)
	{
	    vim_free(*ffname);
	    *ffname = rfname;
	    *sfname = rfname;
	}
    }
#అయితే_ముగింపు
}

/*
 * Open a window for a number of buffers.
 */
    శూన్య
ex_బఫరు_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    పూర్ణాంకం		విభజించు_ret = OK;
    పూర్ణాంకం		p_ea_save;
    పూర్ణాంకం		open_wins = 0;
    పూర్ణాంకం		r;
    పూర్ణాంకం		లెక్క;		// Maximum number of windows to open.
    పూర్ణాంకం		all;		// When TRUE also load inactive buffers.
    పూర్ణాంకం		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    ఒకవేళ (eap->addr_లెక్క == 0)	// make as many windows as possible
	లెక్క = 9999;
    లేకపోతే
	లెక్క = eap->line2;	// make as many windows as specified
    ఒకవేళ (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    లేకపోతే
	all = TRUE;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    setpcmark();

#నిర్వచితమైతే FEAT_GUI
    need_mouse_correct = TRUE;
#అయితే_ముగింపు

    /*
     * Close superfluous windows (two windows for the same buffer).
     * Also close windows that are not full-width.
     */
    ఒకవేళ (had_tab > 0)
	goto_tabpage_tp(మొదటి_tabpage, TRUE, TRUE);
    కొరకు (;;)
    {
	tpnext = curtab->tp_తదుపరి;
	కొరకు (wp = firstwin; wp != ఖాళీ; wp = wpnext)
	{
	    wpnext = wp->w_తదుపరి;
	    ఒకవేళ ((wp->w_బఫరు->b_nwindows > 1
			|| ((cmdmod.cmod_విభజించు & WSP_VERT)
			    ? wp->w_ఎత్తు + wp->w_స్థితి_ఎత్తు < అడ్డువరుసలు - p_ch
							     - tabline_ఎత్తు()
			    : wp->w_వెడల్పు != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_కిటికీ
		    && !(win_locked(wp) || wp->w_బఫరు->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		ఒకవేళ (win_close(wp, FALSE) == FAIL)
		    ఆపు;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = మొదటి_tabpage;
		open_wins = 0;
	    }
	    లేకపోతే
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	ఒకవేళ (had_tab == 0 || tpnext == ఖాళీ)
	    ఆపు;
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
    కొరకు (buf = firstbuf; buf != ఖాళీ && open_wins < లెక్క; buf = buf->b_తదుపరి)
    {
	// Check if this buffer needs a window
	ఒకవేళ ((!all && buf->b_ml.ml_mfp == ఖాళీ) || !buf->b_p_bl)
	    కొనసాగు;

	ఒకవేళ (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    ఒకవేళ (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    లేకపోతే
		wp = ఖాళీ;
	}
	లేకపోతే
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		ఒకవేళ (wp->w_బఫరు == buf)
		    ఆపు;
	    // If the buffer already has a window, move it
	    ఒకవేళ (wp != ఖాళీ)
		win_move_after(wp, curwin);
	}

	ఒకవేళ (wp == ఖాళీ && విభజించు_ret == OK)
	{
	    bufref_T	bufref;

	    సమితి_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    విభజించు_ret = win_విభజించు(0, WSP_స్థలం | WSP_క్రింద);
	    ++open_wins;
	    p_ea = p_ea_save;
	    ఒకవేళ (విభజించు_ret == FAIL)
		కొనసాగు;

	    // Open the buffer in this window.
	    మార్పిడి_exists_action = SEA_DIALOG;
	    సమితి_curbuf(buf, DOBUF_GOTO);
	    ఒకవేళ (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		మార్పిడి_exists_action = SEA_NONE;
		ఆపు;
	    }
	    ఒకవేళ (మార్పిడి_exists_action == SEA_QUIT)
	    {
#ఒకవేళ defined(FEAT_EVAL)
		cleanup_T   cs;

		// Reset the error/interrupt/exception state here so that
		// aborting() returns FALSE when closing a window.
		enter_cleanup(&cs);
#అయితే_ముగింపు

		// User selected Quit at ATTENTION prompt; close this window.
		win_close(curwin, TRUE);
		--open_wins;
		మార్పిడి_exists_action = SEA_NONE;
		మార్పిడి_exists_did_quit = TRUE;

#ఒకవేళ defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#అయితే_ముగింపు
	    }
	    లేకపోతే
		handle_మార్పిడి_exists(ఖాళీ);
	}

	ui_breakcheck();
	ఒకవేళ (got_int)
	{
	    (శూన్య)vgetc();	// only break the file loading, not the rest
	    ఆపు;
	}
#నిర్వచితమైతే FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	ఒకవేళ (aborting())
	    ఆపు;
#అయితే_ముగింపు
	// When ":tab" was used open a new tab for a new window repeatedly.
	ఒకవేళ (had_tab > 0 && tabpage_సూచీ(ఖాళీ) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    కొరకు (wp = lastwin; open_wins > లెక్క; )
    {
	r = (buf_hide(wp->w_బఫరు) || !buf_Is_మారింది(wp->w_బఫరు)
				     || autowrite(wp->w_బఫరు, FALSE) == OK);
	ఒకవేళ (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	లేకపోతే ఒకవేళ (r)
	{
	    win_close(wp, !buf_hide(wp->w_బఫరు));
	    --open_wins;
	    wp = lastwin;
	}
	లేకపోతే
	{
	    wp = wp->w_prev;
	    ఒకవేళ (wp == ఖాళీ)
		ఆపు;
	}
    }
}


స్థిర పూర్ణాంకం  chk_modeline(linenr_T, పూర్ణాంకం);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    శూన్య
do_modelines(పూర్ణాంకం flags)
{
    linenr_T	lnum;
    పూర్ణాంకం		nmlines;
    స్థిర పూర్ణాంకం	entered = 0;

    ఒకవేళ (!curbuf->b_p_ml || (nmlines = (పూర్ణాంకం)p_mls) == 0)
	తిరిగి;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    ఒకవేళ (entered)
	తిరిగి;

    ++entered;
    కొరకు (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క && lnum <= nmlines;
								       ++lnum)
	ఒకవేళ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    కొరకు (lnum = curbuf->b_ml.ml_పంక్తి_లెక్క; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_పంక్తి_లెక్క - nmlines; --lnum)
	ఒకవేళ (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#అన్తర్భావ "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    స్థిర పూర్ణాంకం
chk_modeline(
    linenr_T	lnum,
    పూర్ణాంకం		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*పంక్తి_చివర;		// point to the end of the line
    char_u	*e;
    పూర్ణాంకం		prev;
    పూర్ణాంకం		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    పంక్తి_చివర = s + ml_get_len(lnum);
    కొరకు (; *s != NUL; ++s)
    {
	ఒకవేళ (prev == -1 || vim_isspace(prev))
	{
	    ఒకవేళ ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		ఆపు;
	    // Accept both "vim" and "Vim".
	    ఒకవేళ ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		పూర్ణాంకం vers;

		ఒకవేళ (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		లేకపోతే
		    e = s + 3;
		vers = getdigits(&e);
		ఒకవేళ (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_సంస్కరణ_100 >= vers && సురక్షిత_isdigit(s[3]))
			    || (VIM_సంస్కరణ_100 < vers && s[3] == '<')
			    || (VIM_సంస్కరణ_100 > vers && s[3] == '>')
			    || (VIM_సంస్కరణ_100 == vers && s[3] == '=')))
		    ఆపు;
	    }
	}
	prev = *s;
    }

    ఒకవేళ (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	పూర్ణాంకం	చివర;

	చేయి				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	అయితే (s[-1] != ':');

	len = (size_t)(పంక్తి_చివర - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	ఒకవేళ (linecopy == ఖాళీ)
	    తిరిగి FAIL;

	పంక్తి_చివర = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_నెట్టు(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_అమరిక;

	చివర = FALSE;
	అయితే (చివర == FALSE)
	{
	    s = skipwhite(s);
	    ఒకవేళ (*s == NUL)
		ఆపు;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    కొరకు (e = s; *e != ':' && *e != NUL; ++e)
		ఒకవేళ (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(పంక్తి_చివర - (e + 1)) + 1);	// +1 for NUL
		    --పంక్తి_చివర;
		}
	    ఒకవేళ (*e == NUL)
		చివర = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    ఒకవేళ (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		ఒకవేళ (*e != ':')		// no terminating ':'?
		    ఆపు;
		చివర = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    ఒకవేళ (*s != NUL)		// skip over an empty "::"
	    {
		పూర్ణాంకం secure_save = secure;
		sctx_T	save_current_sctx = current_sctx;

		current_sctx.sc_సంస్కరణ = 1;
#నిర్వచితమైతే FEAT_EVAL
		current_sctx.sc_sid = SID_MODELINE;
		current_sctx.sc_seq = 0;
		current_sctx.sc_lnum = lnum;
#అయితే_ముగింపు

		// Make sure no risky things are executed as a side effect.
		secure = 1;

		retval = do_సమితి(s, OPT_MODELINE | OPT_LOCAL | flags);

		secure = secure_save;
		current_sctx = save_current_sctx;
		ఒకవేళ (retval == FAIL)		// stop if error found
		    ఆపు;
	    }
	    s = (e == పంక్తి_చివర) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_తీయి();
	vim_free(linecopy);
    }
    తిరిగి retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    పూర్ణాంకం
bt_సాధారణ(buf_T *buf)
{
    తిరిగి buf != ఖాళీ && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    పూర్ణాంకం
bt_quickfix(buf_T *buf వాడని)
{
#నిర్వచితమైతే FEAT_QUICKFIX
    తిరిగి buf != ఖాళీ && buf->b_p_bt[0] == 'q';
#లేకపోతే
    తిరిగి FALSE;
#అయితే_ముగింపు
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    పూర్ణాంకం
bt_terminal(buf_T *buf వాడని)
{
#ఒకవేళ defined(FEAT_TERMINAL)
    తిరిగి buf != ఖాళీ && buf->b_p_bt[0] == 't';
#లేకపోతే
    తిరిగి FALSE;
#అయితే_ముగింపు
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    పూర్ణాంకం
bt_help(buf_T *buf)
{
    తిరిగి buf != ఖాళీ && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    పూర్ణాంకం
bt_prompt(buf_T *buf)
{
    తిరిగి buf != ఖాళీ && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#ఒకవేళ defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    పూర్ణాంకం
bt_popup(buf_T *buf)
{
    తిరిగి buf != ఖాళీ && buf->b_p_bt != ఖాళీ
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#అయితే_ముగింపు

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    పూర్ణాంకం
bt_nofilename(buf_T *buf)
{
    తిరిగి buf != ఖాళీ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    స్థిర పూర్ణాంకం
bt_nofileread(buf_T *buf)
{
    తిరిగి buf != ఖాళీ && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#ఒకవేళ defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    పూర్ణాంకం
bt_nofile(buf_T *buf)
{
    తిరిగి buf != ఖాళీ && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#అయితే_ముగింపు

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    పూర్ణాంకం
bt_dontwrite(buf_T *buf)
{
    తిరిగి buf != ఖాళీ && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    పూర్ణాంకం
bt_dontwrite_సందేశం(buf_T *buf)
{
    ఒకవేళ (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_సమితి));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    పూర్ణాంకం
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    ఎంపిక (buf->b_p_bh[0])
    {
	స్థితి 'u':		    // "unload"
	స్థితి 'w':		    // "wipe"
	స్థితి 'd': తిరిగి FALSE;	    // "delete"
	స్థితి 'h': తిరిగి TRUE;	    // "hide"
    }
    తిరిగి (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#ఒకవేళ defined(FEAT_QUICKFIX)
    ఒకవేళ (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	ఒకవేళ (buf->b_fnum == qf_దొంతర_get_bufnr())
	    తిరిగి (char_u *)_(సందేశం_qflist);
	లేకపోతే
	    తిరిగి (char_u *)_(సందేశం_loclist);
    }
#అయితే_ముగింపు

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    ఒకవేళ (bt_nofilename(buf))
    {
#నిర్వచితమైతే FEAT_TERMINAL
	ఒకవేళ (buf->b_పదం != ఖాళీ)
	    తిరిగి పదం_get_స్థితి_పాఠ్యం(buf->b_పదం);
#అయితే_ముగింపు
	ఒకవేళ (buf->b_fname != ఖాళీ)
	    తిరిగి buf->b_fname;
	ఒకవేళ (buf == cmdwin_buf)
	    తిరిగి (char_u *)_("[Command Line]");
#నిర్వచితమైతే FEAT_పని_CHANNEL
	ఒకవేళ (bt_prompt(buf))
	    తిరిగి (char_u *)_("[Prompt]");
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_PROP_POPUP
	ఒకవేళ (bt_popup(buf))
	    తిరిగి (char_u *)_("[Popup]");
#అయితే_ముగింపు
	తిరిగి (char_u *)_("[Scratch]");
    }

    ఒకవేళ (buf->b_fname == ఖాళీ)
	తిరిగి buf_get_fname(buf);
    తిరిగి ఖాళీ;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    ఒకవేళ (buf->b_fname == ఖాళీ)
	తిరిగి (char_u *)_("[No Name]");
    తిరిగి buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    శూన్య
సమితి_buflisted(పూర్ణాంకం on)
{
    ఒకవేళ (on == curbuf->b_p_bl)
	తిరిగి;

    curbuf->b_p_bl = on;
    ఒకవేళ (on)
	apply_autocmds(EVENT_BUFADD, ఖాళీ, ఖాళీ, FALSE, curbuf);
    లేకపోతే
	apply_autocmds(EVENT_BUFDELETE, ఖాళీ, ఖాళీ, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    పూర్ణాంకం
buf_contents_మారింది(buf_T *buf)
{
    buf_T	*newbuf;
    పూర్ణాంకం		తేడా = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ఖాళీ, ఖాళీ, (linenr_T)1, BLN_DUMMY);
    ఒకవేళ (newbuf == ఖాళీ)
	తిరిగి TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    ఒకవేళ (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_బఫరు(newbuf, FALSE);
	తిరిగి TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    ఒకవేళ (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_బఫరు(newbuf, FALSE);
	తిరిగి TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    ఖండం_autocmds();
    ఒకవేళ (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	ఒకవేళ (buf->b_ml.ml_పంక్తి_లెక్క == curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    తేడా = FALSE;
	    కొరకు (lnum = 1; lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క; ++lnum)
		ఒకవేళ (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    తేడా = TRUE;
		    ఆపు;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    ఒకవేళ (curbuf != newbuf)	// safety check
	wipe_బఫరు(newbuf, FALSE);

    unblock_autocmds();

    తిరిగి తేడా;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    శూన్య
wipe_బఫరు(
    buf_T	*buf,
    పూర్ణాంకం		aucmd)	    // When TRUE trigger autocommands.
{
    ఒకవేళ (buf->b_fnum == శిఖరం_file_అంకె - 1)
	--శిఖరం_file_అంకె;

    ఒకవేళ (!aucmd)		    // Don't trigger BufDelete autocommands here.
	ఖండం_autocmds();

    close_బఫరు(ఖాళీ, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    ఒకవేళ (!aucmd)
	unblock_autocmds();
}
