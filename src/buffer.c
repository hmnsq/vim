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

#ضمن "vim.h"


#که_تعریف_شوی FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_ژوروالی 100
#د_که_پای

ساكن فراغ	enter_بفر(buf_T *buf);
ساكن فراغ	buflist_getfpos(فراغ);
ساكن char_u	*buflist_سمون(regmatch_T *rmp, buf_T *buf, صحيح ignore_case);
ساكن char_u	*fname_سمون(regmatch_T *rmp, char_u *name, صحيح ignore_case);
#که_تعریف_شوی UNIX
ساكن buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
ساكن صحيح	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
ساكن صحيح	buf_same_ino(buf_T *buf, stat_T *stp);
#وإلا
ساكن صحيح	otherfile_buf(buf_T *buf, char_u *ffname);
#د_که_پای
ساكن صحيح	ارزښت_بدل_شوی(char_u *str, char_u **وروستی);
#إذا defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
ساكن صحيح build_stl_str_hl_local(stl_ډېرپېښ_T ډېرپېښ, win_T *wp,
		char_u *بهر, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, صحيح opt_scope, صحيح fillchar, صحيح maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, صحيح *lbreaks, صحيح *لېږد_hl);
#د_که_پای
ساكن صحيح	append_arg_شمېره(win_T *wp, char_u *buf, size_t buflen, صحيح جمع_کړه_file);
ساكن فراغ	free_بفر(buf_T *);
ساكن فراغ	free_بفر_stuff(buf_T *buf, صحيح free_options);
ساكن صحيح	bt_nofileread(buf_T *buf);
ساكن صحيح	do_بفر_ext(صحيح action, صحيح پیل, صحيح dir, صحيح شمېر, صحيح flags);

#که_تعریف_شوی UNIX
# define dev_T dev_t
#وإلا
# define dev_T بینښه
#د_که_پای

#تعریف_کړه FOR_ALL_BUFS_FROM_وروستی(buf) \
    لكل ((buf) = lastbuf; (buf) != عدم; (buf) = (buf)->b_prev)

#إذا defined(FEAT_QUICKFIX)
ساكن حرف *پیغام_loclist = N_("[Location List]");
ساكن حرف *پیغام_qflist = N_("[Quickfix List]");
#د_که_پای

// Number of times free_buffer() was called.
ساكن صحيح	buf_free_شمېر = 0;

ساكن صحيح	سر_file_عدد = 1;	// highest file number
ساكن garray_T buf_reuse = GA_تشه;	// file numbers to recycle

    ساكن فراغ
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    کړکۍ_layout_قلف();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"إذا exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    کړکۍ_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    ساكن صحيح
calc_percentage(اوږد part, اوږد ټولیز)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    أرجع (part > 1000000L)
	? (صحيح)(part / (ټولیز / 100L))
	: (صحيح)((part * 100L) / ټولیز);
}

/*
 * Return the highest possible buffer number.
 */
    صحيح
get_highest_fnum(فراغ)
{
    أرجع سر_file_عدد - 1;
}

/*
 * Read data from buffer for retrying.
 */
    ساكن صحيح
read_بفر(
    صحيح		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    صحيح		flags)		    // extra flags for readfile()
{
    صحيح		retval = OK;
    linenr_T	کرښه_شمېر;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    کرښه_شمېر = curbuf->b_ml.ml_کرښه_شمېر;
    retval = readfile(
	    read_stdin ? عدم : curbuf->b_ffname,
	    read_stdin ? عدم : curbuf->b_fname,
	    کرښه_شمېر, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_بفر);
    إذا (retval == OK)
    {
	// Delete the binary lines.
	طالما (--کرښه_شمېر >= 0)
	    ml_delete((linenr_T)1);
    }
    وإلا
    {
	// Delete the converted lines.
	طالما (curbuf->b_ml.ml_کرښه_شمېر > کرښه_شمېر)
	    ml_delete(کرښه_شمېر);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ستون = 0;

    إذا (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	إذا (!readonlymode && !BUFEMPTY())
	    بدل_شوی();
	وإلا إذا (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	إذا (retval == OK)
	{
#که_تعریف_شوی FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, عدم, عدم, FALSE,
							      curbuf, &retval);
#وإلا
	    apply_autocmds(EVENT_STDINREADPOST, عدم, عدم, FALSE, curbuf);
#د_که_پای
	}
    }
    أرجع retval;
}

#إذا defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    فراغ
بفر_ensure_loaded(buf_T *buf)
{
    إذا (buf->b_ml.ml_mfp != عدم)
	أرجع;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    إذا (curbuf == buf)
    {
	إذا (بدلون_exists_action != SEA_READONLY)
	    بدلون_exists_action = SEA_NONE;
	open_بفر(FALSE, عدم, 0);
	aucmd_restbuf(&aco);
    }
}
#د_که_پای

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
open_بفر(
    صحيح		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    صحيح		flags_arg)	    // extra flags for readfile()
{
    صحيح		flags = flags_arg;
    صحيح		retval = OK;
    bufref_T	old_curbuf;
#که_تعریف_شوی FEAT_SYN_HL
    اوږد	old_tw = curbuf->b_p_tw;
#د_که_پای
    صحيح		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    إذا (readonlymode && curbuf->b_ffname != عدم
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    إذا (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_بفر(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    إذا (curbuf->b_ml.ml_mfp != عدم)
		اكسر;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	إذا (curbuf == عدم)
	{
	    emsg(_(e_cannot_allocate_any_بفر_exiting));

	    // Don't try to do any saving, with "curbuf" NULL almost nothing
	    // will work.
	    v_dying = 2;
	    getout(2);
	}

	emsg(_(e_cannot_allocate_بفر_using_other_one));
	enter_بفر(curbuf);
#که_تعریف_شوی FEAT_SYN_HL
	إذا (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(عدم, curwin);
#د_که_پای
	أرجع FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    إذا (curbuf->b_ml.ml_mfp != عدم)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    سېټ_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_سېټ = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    إذا (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    إذا (curbuf->b_ffname != عدم
#که_تعریف_شوی FEAT_NETBEANS_INTG
	    && netbeansReadFile
#د_که_پای
       )
    {
	صحيح old_پیغام_silent = پیغام_silent;
#که_تعریف_شوی UNIX
	صحيح	save_bin = curbuf->b_p_bin;
	صحيح perm;
#د_که_پای
#که_تعریف_شوی FEAT_NETBEANS_INTG
	صحيح oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#د_که_پای
#که_تعریف_شوی UNIX
	perm = mch_getperm(curbuf->b_ffname);
	إذا (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	إذا (read_fifo)
	    curbuf->b_p_bin = TRUE;
#د_که_پای
	إذا (shortmess(SHM_FILEINFO))
	    پیغام_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#که_تعریف_شوی UNIX
	إذا (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    إذا (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_بفر(FALSE, eap, flags);
	}
#د_که_پای
	پیغام_silent = old_پیغام_silent;
#که_تعریف_شوی FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#د_که_پای
	// Help buffer is filtered.
	إذا (bt_help(curbuf))
	    fix_help_بفر();
    }
    وإلا إذا (read_stdin)
    {
	صحيح	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(عدم, عدم, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, عدم,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	إذا (retval == OK)
	    retval = read_بفر(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    إذا (curbuf->b_ml.ml_mfp != عدم
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    إذا (curbuf->b_flags & BF_NEVERLOADED)
    {
	(فراغ)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    إذا ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != عدم)
		|| curbuf->b_modified_was_سېټ	// autocmd did ":set modified"
#که_تعریف_شوی FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != عدم)
#د_که_پای
       )
	بدل_شوی();
    وإلا إذا (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_وروستی_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_وروستی_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_وروستی_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#که_تعریف_شوی FEAT_EVAL
    إذا (aborting())
#وإلا
    إذا (got_int)
#د_که_پای
	curbuf->b_flags |= BF_READERR;

#که_تعریف_شوی FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#د_که_پای

    // need to set w_topline, unless some autocommand already did that.
    إذا (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#که_تعریف_شوی FEAT_DIFF
	curwin->w_topfill = 0;
#د_که_پای
    }
#که_تعریف_شوی FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, عدم, عدم, FALSE, curbuf, &retval);
#وإلا
    apply_autocmds(EVENT_BUFENTER, عدم, عدم, FALSE, curbuf);
#د_که_پای

    إذا (retval != OK)
	أرجع retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    إذا (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != عدم)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	إذا (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    إذا ((flags & READ_NOWINENTER) == 0)
#که_تعریف_شوی FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, عدم, عدم,
			FALSE, curbuf, &retval);
#وإلا
	    apply_autocmds(EVENT_BUFWINENTER, عدم, عدم,
		    FALSE, curbuf);
#د_که_پای

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    أرجع retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    فراغ
سېټ_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == عدم ? 0 : buf->b_fnum;
    bufref->br_buf_free_شمېر = buf_free_شمېر;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    صحيح
bufref_valid(bufref_T *bufref)
{
    أرجع bufref->br_buf_free_شمېر == buf_free_شمېر
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    صحيح
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_وروستی(bp)
	إذا (bp == buf)
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
ساكن hashtab_T buf_hashtab;

    ساكن فراغ
buf_hashtab_جمع_کړه(buf_T *buf)
{
    vim_snprintf((حرف *)buf->b_کلی, حجم(buf->b_کلی), "%x", buf->b_fnum);
    إذا (hash_جمع_کړه(&buf_hashtab, buf->b_کلی, "create buffer") == FAIL)
	emsg(_(e_بفر_cannot_be_registered));
}

    ساكن فراغ
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_پیدا_کړه(&buf_hashtab, buf->b_کلی);

    إذا (!HASHITEM_تشه(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    ساكن صحيح
can_unload_بفر(buf_T *buf)
{
    صحيح	    can_unload = !buf->b_locked;

    إذا (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    إذا (wp->w_بفر == buf)
	    {
		can_unload = FALSE;
		اكسر;
	    }
    }
    // Don't unload the buffer while it's still being saved
    إذا (can_unload && buf->b_saving)
	can_unload = FALSE;

    إذا (!can_unload)
    {
	char_u *fname = buf->b_fname != عدم ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_منزل_delete_بفر_that_is_دننه_use_str),
				fname != عدم ? fname : (char_u *)"[No Name]");
    }
    أرجع can_unload;
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
    صحيح
close_بفر(
    win_T	*win,
    buf_T	*buf,
    صحيح		action,
    صحيح		abort_if_وروستی,
    صحيح		ignore_abort,
    صحيح		سېټ_context)
{
    صحيح		hiding_buf;
    bufref_T	bufref;
    صحيح		is_curwin = (curwin != عدم && curwin == win
				&& curwin->w_بفر == buf);
    tabpage_T	*the_curtab = curtab;
    صحيح		unload_buf = (action != 0);
    صحيح		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    صحيح		del_buf = (action == DOBUF_DEL || wipe_buf);
    صحيح		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    صحيح		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    إذا (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    وإلا إذا (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    وإلا إذا (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#که_تعریف_شوی FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    إذا (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	إذا (غړی_دنده_running(buf->b_غړی))
	{
	    إذا (wipe_buf || unload_buf)
	    {
		إذا (!can_unload_بفر(buf))
		    أرجع FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    وإلا
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	وإلا إذا (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	وإلا
	{
	    إذا (del_buf || unload_buf)
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
#د_که_پای

    // Disallow deleting the buffer when it is locked (already being closed or
    // halfway a command that relies on it). Unloading is allowed.
    إذا ((del_buf || wipe_buf) && !can_unload_بفر(buf))
	أرجع FALSE;

    إذا (سېټ_context && win_valid && win->w_بفر == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	إذا (buf->b_nwindows == 1)
	    سېټ_وروستی_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.ستون, TRUE);
    }

    سېټ_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    إذا ((win_valid || closed_popup) && win->w_بفر == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_وېشل;
	إذا (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_منزل_abort));
	    أرجع FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_وېشل;
	إذا (abort_if_وروستی && one_کړکۍ())
	    // Autocommands made this the only window.
	    اذهب aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	إذا (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_وېشل;
	    إذا (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		اذهب aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_وېشل;
	    إذا (abort_if_وروستی && one_کړکۍ())
		// Autocommands made this the only window.
		اذهب aucmd_abort;
	}
#که_تعریف_شوی FEAT_EVAL
	// autocmds may abort script processing
	إذا (!ignore_abort && aborting())
	    أرجع FALSE;
#د_که_پای
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    إذا (is_curwin && curwin != win && win_valid)
    {
	بلاک_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    إذا (bt_quickfix(buf) && win_valid && win->w_بفر == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_بفر == buf && buf->b_nwindows == 1);

#که_تعریف_شوی FEAT_DIFF
    إذا (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#د_که_پای

    // Return when another window is displaying the buffer or when not unloaded.
    إذا (!hiding_buf || !unload_buf)
	أرجع FALSE;

    // Always remove the buffer when there is no file name.
    إذا (buf->b_ffname == عدم)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    إذا (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	أرجع FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    إذا ((win_valid || closed_popup) && win->w_بفر == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_بفر = عدم;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    إذا (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != عدم || buf->b_بل != عدم
#إذا defined(EXITFREE)
				|| entered_free_all_mem
#د_که_پای
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	إذا (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    إذا (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, حجم(صحيح), 50);
	    إذا (ga_grow(&buf_reuse, 1) == OK)
		((صحيح *)buf_reuse.ga_معلومات)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	إذا (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	وإلا
	    buf->b_sfname = عدم;
	VIM_CLEAR(buf->b_ffname);
	إذا (buf->b_prev == عدم)
	    firstbuf = buf->b_بل;
	وإلا
	    buf->b_prev->b_بل = buf->b_بل;
	إذا (buf->b_بل == عدم)
	    lastbuf = buf->b_prev;
	وإلا
	    buf->b_بل->b_prev = buf->b_prev;
	free_بفر(buf);
    }
    وإلا
    {
	إذا (del_buf)
	{
	    // Free all internal variables and reset option values, to make
	    // ":bdel" compatible with Vim 5.7.
	    free_بفر_stuff(buf, TRUE);

	    // Make it look like a new buffer.
	    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;

	    // Init the options when loaded again.
	    buf->b_p_initialized = false;
	}
	buf_clear_file(buf);
	إذا (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    أكد(!win_valid || !is_curwin || win == curwin);
    أرجع TRUE;
}

/*
 * Make buffer not contain a file.
 */
    فراغ
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_کرښه_شمېر = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_پیل_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_پیل_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_پیل_bomb = FALSE;
    buf->b_ml.ml_mfp = عدم;
    buf->b_ml.ml_flags = ML_تشه;		// empty buffer
#که_تعریف_شوی FEAT_NETBEANS_INTG
    netbeans_deleted_all_کرښې(buf);
#د_که_پای
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
    صحيح
buf_freeall(buf_T *buf, صحيح flags)
{
    صحيح		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    صحيح		is_curwin = (curwin != عدم && curwin->w_بفر == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_وېشل;
    سېټ_bufref(&bufref, buf);
    إذا (buf->b_ml.ml_mfp != عدم)
    {
	إذا (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    أرجع FALSE;
    }
    إذا ((flags & BFA_DEL) && buf->b_p_bl)
    {
	إذا (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    أرجع FALSE;
    }
    إذا (flags & BFA_WIPE)
    {
	إذا (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    أرجع FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_وېشل;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    إذا (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	بلاک_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#که_تعریف_شوی FEAT_EVAL
    // autocmds may abort script processing
    إذا ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	أرجع FALSE;
#د_که_پای

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    إذا (buf == curbuf && !is_curbuf)
	أرجع FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    إذا (buf == curbuf && VIsual_فعال
#إذا defined(EXITFREE)
	    && !entered_free_all_mem
#د_که_پای
	    )
	پای_ټکی_visual_ډېرپېښ();

#که_تعریف_شوی FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#د_که_پای
#که_تعریف_شوی FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    إذا (curwin != عدم && curwin->w_بفر == buf)
	reset_synblock(curwin);
#د_که_پای

#که_تعریف_شوی FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    إذا (win->w_بفر == buf)
		clearFolding(win);
    }
#د_که_پای

#که_تعریف_شوی FEAT_TCL
    tcl_بفر_free(buf);
#د_که_پای
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_کرښه_شمېر = 0;    // no lines in buffer
    إذا ((flags & BFA_KEEP_UNDO) == 0)
	// free the memory allocated for undo
	// and reset all undo information
	u_clearallandblockfree(buf);
#که_تعریف_شوی FEAT_SYN_HL
    syntax_clear(&buf->b_s);	    // reset syntax info
#د_که_پای
#که_تعریف_شوی FEAT_PROP_POPUP
    clear_buf_prop_types(buf);
#د_که_پای
    buf->b_flags &= ~BF_READERR;    // a read error is no longer relevant
    أرجع TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    ساكن فراغ
free_بفر(buf_T *buf)
{
    ++buf_free_شمېر;
    free_بفر_stuff(buf, TRUE);
#که_تعریف_شوی FEAT_EVAL
    // b:changedtick uses an item in buf_T, remove it now
    dictitem_remove(buf->b_vars, (dictitem_T *)&buf->b_ct_di, "free buffer");
    unref_var_dict(buf->b_vars);
    remove_listeners(buf);
#د_که_پای
#که_تعریف_شوی FEAT_LUA
    lua_بفر_free(buf);
#د_که_پای
#که_تعریف_شوی FEAT_MZSCHEME
    mzscheme_بفر_free(buf);
#د_که_پای
#که_تعریف_شوی FEAT_PERL
    perl_buf_free(buf);
#د_که_پای
#که_تعریف_شوی FEAT_PYTHON
    python_بفر_free(buf);
#د_که_پای
#که_تعریف_شوی FEAT_PYTHON3
    python3_بفر_free(buf);
#د_که_پای
#که_تعریف_شوی FEAT_RUBY
    ruby_بفر_free(buf);
#د_که_پای
#که_تعریف_شوی FEAT_دنده_CHANNEL
    channel_بفر_free(buf);
#د_که_پای
#که_تعریف_شوی FEAT_TERMINAL
    free_terminal(buf);
#د_که_پای
#که_تعریف_شوی FEAT_دنده_CHANNEL
    vim_free(buf->b_prompt_متن);
    free_callback(&buf->b_prompt_callback);
    free_callback(&buf->b_prompt_مداخله);
#د_که_پای

    buf_hashtab_remove(buf);

    aubuflocal_remove(buf);

    إذا (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_بل = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    وإلا
    {
	vim_free(buf);
	إذا (curbuf == buf)
	    curbuf = عدم;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    ساكن فراغ
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_شمېره;
    di->di_tv.v_قلف = VAR_FIXED;
    di->di_tv.vval.v_شمېره = 0;

#که_تعریف_شوی FEAT_EVAL
    STRCPY(buf->b_ct_di.di_کلی, "changedtick");
    (فراغ)dict_جمع_کړه(buf->b_vars, di);
#د_که_پای
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    ساكن فراغ
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    طالما (buf->b_wininfo != عدم)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_بل;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    ساكن فراغ
free_بفر_stuff(
    buf_T	*buf,
    صحيح		free_options)		// free options as well
{
    إذا (free_options)
    {
	clear_wininfo(buf);		// including window-local options
	free_buf_options(buf, TRUE);
#که_تعریف_شوی FEAT_SPELL
	ga_clear(&buf->b_s.b_langp);
#د_که_پای
    }
#که_تعریف_شوی FEAT_EVAL
    {
	varnumber_T tick = CHANGEDTICK(buf);

	vars_clear(&buf->b_vars->dv_hashtab); // free all buffer variables
	hash_init(&buf->b_vars->dv_hashtab);
	init_changedtick(buf);
	CHANGEDTICK(buf) = tick;
	remove_listeners(buf);
    }
#د_که_پای
    uc_clear(&buf->b_ucmds);		// clear local user commands
#که_تعریف_شوی FEAT_SIGNS
    buf_delete_signs(buf, (char_u *)"*");	// delete any signs
#د_که_پای
#که_تعریف_شوی FEAT_NETBEANS_INTG
    netbeans_file_killed(buf);
#د_که_پای
    map_clear_ډېرپېښ(buf, MAP_ALL_MODES, TRUE, FALSE);  // clear local mappings
    map_clear_ډېرپېښ(buf, MAP_ALL_MODES, TRUE, TRUE);   // clear local abbrevs
    VIM_CLEAR(buf->b_پیل_fenc);
}

/*
 * Free one wininfo_T.
 */
    فراغ
free_wininfo(wininfo_T *wip)
{
    إذا (wip->wi_optset)
    {
	clear_winopt(&wip->wi_opt);
#که_تعریف_شوی FEAT_FOLDING
	deleteFoldRecurse(&wip->wi_folds);
#د_که_پای
    }
    vim_free(wip);
}

/*
 * Go to another buffer.  Handles the result of the ATTENTION dialog.
 */
    فراغ
goto_بفر(
    exarg_T	*eap,
    صحيح		پیل,
    صحيح		dir,
    صحيح		شمېر)
{
    bufref_T	old_curbuf;
    صحيح		save_sea = بدلون_exists_action;
    صحيح		skip_help_buf;

    بدل (eap->cmdidx)
    {
	حالة CMD_bnext:
	حالة CMD_sbnext:
	حالة CMD_b_بل:
	حالة CMD_bprevious:
	حالة CMD_sb_بل:
	حالة CMD_sbprevious:
	    skip_help_buf = TRUE;
	    اكسر;
	افتراضي:
	    skip_help_buf = FALSE;
	    اكسر;
    }

    سېټ_bufref(&old_curbuf, curbuf);

    إذا (بدلون_exists_action == SEA_NONE)
	بدلون_exists_action = SEA_DIALOG;
    (فراغ)do_بفر_ext(*eap->cmd == 's' ? DOBUF_وېشل : DOBUF_GOTO, پیل, dir, شمېر,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    إذا (بدلون_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#إذا defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#د_که_پای

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	بدلون_exists_action = save_sea;
	بدلون_exists_did_quit = TRUE;

#إذا defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#د_که_پای
    }
    وإلا
	handle_بدلون_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    فراغ
handle_بدلون_exists(bufref_T *old_curbuf)
{
#إذا defined(FEAT_EVAL)
    cleanup_T	cs;
#د_که_پای
#که_تعریف_شوی FEAT_SYN_HL
    اوږد	old_tw = curbuf->b_p_tw;
#د_که_پای
    buf_T	*buf;

    إذا (بدلون_exists_action == SEA_QUIT)
    {
#إذا defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#د_که_پای

	// User selected Quit at ATTENTION prompt.  Go back to previous
	// buffer.  If that buffer is gone or the same as the current one,
	// open a new, empty buffer.
	بدلون_exists_action = SEA_NONE;	// don't want it again
	بدلون_exists_did_quit = TRUE;
	close_بفر(curwin, curbuf, DOBUF_UNLOAD, FALSE, FALSE, TRUE);
	إذا (old_curbuf == عدم || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    بلاک_autocmds();
	    buf = buflist_new(عدم, عدم, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	وإلا
	    buf = old_curbuf->br_buf;
	إذا (buf != عدم)
	{
	    صحيح old_پیغام_silent = پیغام_silent;

	    إذا (shortmess(SHM_FILEINFO))
		پیغام_silent = 1;  // prevent fileinfo message
	    enter_بفر(buf);
	    // restore msg_silent, so that the command line will be shown
	    پیغام_silent = old_پیغام_silent;

#که_تعریف_شوی FEAT_SYN_HL
	    إذا (old_tw != curbuf->b_p_tw)
		check_colorcolumn(عدم, curwin);
#د_که_پای
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#إذا defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#د_که_پای
    }
    وإلا إذا (بدلون_exists_action == SEA_RECOVER)
    {
#إذا defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#د_که_پای

	// User selected Recover at ATTENTION prompt.
	پیغام_scroll = TRUE;
	ml_recover(FALSE);
	پیغام_puts("\n");	// don't overwrite the last message
	cmdline_لیکه = پیغام_لیکه;
	do_modelines(0);

#إذا defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#د_که_پای
    }
    بدلون_exists_action = SEA_NONE;
}

/*
 * Make the current buffer empty.
 * Used when it is wiped out and it's the last buffer.
 */
    ساكن صحيح
تشه_curbuf(
    صحيح close_others,
    صحيح forceit,
    صحيح action)
{
    صحيح	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    إذا (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_وروستی_بفر));
	أرجع FAIL;
    }

    سېټ_bufref(&bufref, buf);
    إذا (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, عدم, عدم, عدم, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    إذا (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_بفر(عدم, buf, action, FALSE, FALSE, FALSE);
    إذا (!close_others)
	need_fileinfo = FALSE;
    وإلا إذا (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    أرجع retval;
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
    ساكن صحيح
do_بفر_ext(
    صحيح		action,
    صحيح		پیل,
    صحيح		dir,		// FORWARD or BACKWARD
    صحيح		شمېر,		// buffer number or number of buffers
    صحيح		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    صحيح		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    بدل (پیل)
    {
	حالة DOBUF_لومړی:   buf = firstbuf; اكسر;
	حالة DOBUF_وروستی:    buf = lastbuf;  اكسر;
	افتراضي:	    buf = curbuf;   اكسر;
    }
    إذا (پیل == DOBUF_MOD)	    // find next modified buffer
    {
	طالما (شمېر-- > 0)
	{
	    افعل
	    {
		buf = buf->b_بل;
		إذا (buf == عدم)
		    buf = firstbuf;
	    }
	    طالما (buf != curbuf && !buf_Is_بدل_شوی(buf));
	}
	إذا (!buf_Is_بدل_شوی(buf))
	{
	    emsg(_(e_no_modified_بفر_وموندل));
	    أرجع FAIL;
	}
    }
    وإلا إذا (پیل == DOBUF_لومړی && شمېر) // find specified buffer number
    {
	طالما (buf != عدم && buf->b_fnum != شمېر)
	    buf = buf->b_بل;
    }
    وإلا
    {
	صحيح help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = عدم;
	طالما (شمېر > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    إذا (bp == عدم)
		bp = buf;
	    إذا (dir == FORWARD)
	    {
		buf = buf->b_بل;
		إذا (buf == عدم)
		    buf = firstbuf;
	    }
	    وإلا
	    {
		buf = buf->b_prev;
		إذا (buf == عدم)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    إذا (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--شمېر;
		bp = عدم;	// use this buffer as new starting point
	    }
	    إذا (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_بفر));
		أرجع FAIL;
	    }
	}
    }

    إذا (buf == عدم)	    // could not find it
    {
	إذا (پیل == DOBUF_لومړی)
	{
	    // don't warn when deleting
	    إذا (!unload)
		semsg(_(e_بفر_nr_does_not_exist), شمېر);
	}
	وإلا إذا (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_وروستی_بفر));
	وإلا
	    emsg(_(e_cannot_go_before_لومړی_بفر));
	أرجع FAIL;
    }
#که_تعریف_شوی FEAT_PROP_POPUP
    إذا ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	أرجع OK;
#د_که_پای
    إذا (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_سېټ_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	أرجع FAIL;

    إذا ((action == DOBUF_GOTO || action == DOBUF_وېشل)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_بفر_nr_does_not_exist), شمېر);
	أرجع FAIL;
    }

#که_تعریف_شوی FEAT_GUI
    need_mouse_correct = TRUE;
#د_که_پای

    /*
     * delete buffer "buf" from memory and/or the list
     */
    إذا (unload)
    {
	صحيح	forward;
	bufref_T bufref;

	إذا (!can_unload_بفر(buf))
	    أرجع FAIL;

	سېټ_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	إذا (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == عدم && !buf->b_p_bl)
	    أرجع FAIL;

	إذا ((flags & DOBUF_FORCEIT) == 0 && buf_Is_بدل_شوی(buf))
	{
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    إذا ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		إذا (غړی_دنده_running(buf->b_غړی))
		{
		    إذا (غړی_confirm_stop(buf) == FAIL)
			أرجع FAIL;
		}
		وإلا
# endif
		{
		    dialog_بدل_شوی(buf, FALSE);
		    إذا (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			أرجع FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    إذا (buf_Is_بدل_شوی(buf))
			أرجع FAIL;
		}
	    }
	    وإلا
#د_که_پای
	    {
		no_write_پیغام_buf(buf);
		أرجع FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	إذا (buf == curbuf && VIsual_فعال)
	    پای_ټکی_visual_ډېرپېښ();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    إذا (bp->b_p_bl && bp != buf)
		اكسر;
	إذا (bp == عدم && buf == curbuf)
	    أرجع تشه_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	طالما (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_بفر->b_locked > 0)
		   && (!ONE_کړکۍ || لومړی_tabpage->tp_بل != عدم))
	{
	    إذا (win_close(curwin, FALSE) == FAIL)
		اكسر;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	إذا (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    إذا (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_بفر(عدم, buf, action, FALSE, FALSE, FALSE);
	    أرجع OK;
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
	buf = عدم;	// selected buffer
	bp = عدم;	// used when no loaded buffer found
	إذا (au_new_curbuf.br_buf != عدم && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_وېشل)
	    buf = au_new_curbuf.br_buf;
	وإلا إذا (curwin->w_jumplistlen > 0)
	{
	    صحيح     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    إذا (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    طالما (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		إذا (buf != عدم)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    إذا (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_وېشل)
			buf = عدم;
		    وإلا إذا (buf->b_ml.ml_mfp == عدم)
		    {
			// skip unloaded buf, but may keep it for later
			إذا (bp == عدم)
			    bp = buf;
			buf = عدم;
		    }
		}
		إذا (buf != عدم)   // found a valid buffer: stop searching
		    اكسر;
		// advance to older entry in jump list
		إذا (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    اكسر;
		إذا (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		إذا (jumpidx == forward)		// List exhausted for sure
		    اكسر;
	    }
	}

	إذا (buf == عدم)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_بل;
	    لكل (;;)
	    {
		إذا (buf == عدم)
		{
		    إذا (!forward)	// tried both directions
			اكسر;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    استمر;
		}
		// in non-help buffer, try to skip help buffers, and vv
		إذا (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_وېشل)
		{
		    إذا (buf->b_ml.ml_mfp != عدم)   // found loaded buffer
			اكسر;
		    إذا (bp == عدم)	// remember unloaded buf for later
			bp = buf;
		}
		إذا (forward)
		    buf = buf->b_بل;
		وإلا
		    buf = buf->b_prev;
	    }
	}
	إذا (buf == عدم)	// No loaded buffer, use unloaded one
	    buf = bp;
	إذا (buf == عدم)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		إذا (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_وېشل)
		    اكسر;
	}
	إذا (buf == عدم)	// Still no buffer, just take one
	{
	    إذا (curbuf->b_بل != عدم)
		buf = curbuf->b_بل;
	    وإلا
		buf = curbuf->b_prev;
	    إذا (bt_quickfix(buf)
		    || (buf != عدم && buf != curbuf && buf->b_locked_وېشل))
		buf = عدم;
	}
    }

    إذا (buf == عدم)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	أرجع تشه_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    إذا (action == DOBUF_وېشل && swbuf_goto_win_with_buf(buf) != عدم)
	أرجع OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    إذا (buf != curbuf && buf->b_locked_وېشل)
    {
	emsg(_(e_cannot_switch_منزل_a_closing_بفر));
	أرجع FAIL;
    }
    إذا (action == DOBUF_وېشل && win_وېشل(0, 0) == FAIL) // split window first
	أرجع FAIL;

    // go to current buffer - nothing to do
    إذا (buf == curbuf)
	أرجع OK;

    // Check if the current buffer may be abandoned.
    إذا (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	إذا ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    إذا (غړی_دنده_running(curbuf->b_غړی))
	    {
		إذا (غړی_confirm_stop(curbuf) == FAIL)
		    أرجع FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    وإلا
# endif
	    {
		bufref_T bufref;

		سېټ_bufref(&bufref, buf);
		dialog_بدل_شوی(curbuf, FALSE);
		إذا (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    أرجع FAIL;

		إذا (buf_Is_بدل_شوی(curbuf))
		{
		    no_write_پیغام();
		    أرجع FAIL;
		}
	    }
	}
	وإلا
#د_که_پای
	{
	    no_write_پیغام();
	    أرجع FAIL;
	}
    }

    // Go to the other buffer.
    سېټ_curbuf(buf, action);

    إذا (action == DOBUF_وېشل)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#إذا defined(FEAT_EVAL)
    إذا (aborting())	    // autocmds may abort script processing
	أرجع FAIL;
#د_که_پای

    أرجع OK;
}

    صحيح
do_بفر(
    صحيح		action,
    صحيح		پیل,
    صحيح		dir,		// FORWARD or BACKWARD
    صحيح		شمېر,		// buffer number or number of buffers
    صحيح		forceit)	// TRUE when using !
{
    أرجع do_بفر_ext(action, پیل, dir, شمېر,
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
    حرف *
do_bufdel(
    صحيح		command,
    char_u	*arg,		// pointer to extra arguments
    صحيح		addr_شمېر,
    صحيح		پیل_bnr,	// first buffer number in a range
    صحيح		پای_ټکی_bnr,	// buffer nr or last buffer nr in a range
    صحيح		forceit)
{
    صحيح		do_current = 0;	// delete current buffer?
    صحيح		deleted = 0;	// number of buffers deleted
    حرف	*errormsg = عدم; // return value
    صحيح		bnr;		// buffer number
    char_u	*p;

    إذا (addr_شمېر == 0)
    {
	(فراغ)do_بفر(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    وإلا
    {
	إذا (addr_شمېر == 2)
	{
	    إذا (*arg)		// both range and argument is not allowed
		أرجع ex_errmsg(e_trailing_characters_str, arg);
	    bnr = پیل_bnr;
	}
	وإلا	// addr_count == 1
	    bnr = پای_ټکی_bnr;

	لكل ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    إذا (bnr == curbuf->b_fnum)
		do_current = bnr;
	    وإلا إذا (do_بفر_ext(command, DOBUF_لومړی, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    إذا (addr_شمېر == 2)
	    {
		إذا (++bnr > پای_ټکی_bnr)
		    اكسر;
	    }
	    وإلا    // addr_count == 1
	    {
		arg = skipwhite(arg);
		إذا (*arg == NUL)
		    اكسر;
		إذا (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    إذا (bnr < 0)	    // failed
			اكسر;
		    arg = p;
		}
		وإلا
		    bnr = getdigits(&arg);
	    }
	}
	إذا (!got_int && do_current && do_بفر(command, DOBUF_لومړی,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	إذا (deleted == 0)
	{
	    إذا (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    وإلا إذا (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    وإلا
		STRCPY(IObuff, _(e_no_buffers_were_wiped_بهر));
	    errormsg = (حرف *)IObuff;
	}
	وإلا إذا (deleted >= p_report)
	{
	    إذا (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    وإلا إذا (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    وإلا
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    أرجع errormsg;
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
    فراغ
سېټ_curbuf(buf_T *buf, صحيح action)
{
    buf_T	*prevbuf;
    صحيح		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#که_تعریف_شوی FEAT_SYN_HL
    اوږد	old_tw = curbuf->b_p_tw;
#د_که_پای
    bufref_T	newbufref;
    bufref_T	prevbufref;
    صحيح		valid;

    setpcmark();
    إذا ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = curbuf->b_fnum; // remember alternate file
    buflist_altfpos(curwin);			 // remember curpos

    // Don't restart Select mode after switching to another buffer.
    VIsual_reselect = FALSE;

    // close_windows() or apply_autocmds() may change curbuf and wipe out "buf"
    prevbuf = curbuf;
    سېټ_bufref(&prevbufref, prevbuf);
    سېټ_bufref(&newbufref, buf);

    // Autocommands may delete the current buffer and/or the buffer we want to
    // go to.  In those cases don't close the buffer.
    إذا (!apply_autocmds(EVENT_BUFLEAVE, عدم, عدم, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#که_تعریف_شوی FEAT_EVAL
		&& !aborting()
#د_که_پای
	       ))
    {
#که_تعریف_شوی FEAT_SYN_HL
	إذا (prevbuf == curwin->w_بفر)
	    reset_synblock(curwin);
#د_که_پای
	إذا (unload)
	    close_windows(prevbuf, FALSE);
#إذا defined(FEAT_EVAL)
	إذا (bufref_valid(&prevbufref) && !aborting())
#وإلا
	إذا (bufref_valid(&prevbufref))
#د_که_پای
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    إذا (prevbuf == curbuf
		    && ((State & ډېرپېښ_ورننباسه) == 0 || curbuf->b_nwindows <= 1))
		u_sync(FALSE);
	    close_بفر(curwin, prevbuf,
		    unload ? action : (action == DOBUF_GOTO
			&& !buf_hide(prevbuf)
			&& !buf_Is_بدل_شوی(prevbuf)) ? DOBUF_UNLOAD : 0,
		    FALSE, FALSE, TRUE);
	}
    }
    // An autocommand may have deleted "buf", already entered it (e.g., when
    // it did ":bunload") or aborted the script processing.
    // If curwin->w_buffer is null, enter_buffer() will make it valid again
    valid = buf_valid(buf);
    إذا ((valid && buf != curbuf
#که_تعریف_شوی FEAT_EVAL
		&& !aborting()
#د_که_پای
	) || curwin->w_بفر == عدم)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	إذا (!valid)
	    enter_بفر(lastbuf);
	وإلا
	    enter_بفر(buf);
#که_تعریف_شوی FEAT_SYN_HL
	إذا (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(عدم, curwin);
#د_که_پای
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    ساكن فراغ
enter_بفر(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    إذا (VIsual_فعال
#إذا defined(EXITFREE)
	    && !entered_free_all_mem
#د_که_پای
	    )
	پای_ټکی_visual_ډېرپېښ();

    إذا (curwin->w_بفر != عدم)
	--curwin->w_بفر->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_بفر = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    إذا (!buf->b_help)
	get_winopts(buf);
#که_تعریف_شوی FEAT_FOLDING
    وإلا
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#د_که_پای

#که_تعریف_شوی FEAT_DIFF
    إذا (curwin->w_p_diff)
	diff_buf_جمع_کړه(curbuf);
#د_که_پای

#که_تعریف_شوی FEAT_SYN_HL
    curwin->w_s = &(curbuf->b_s);
#د_که_پای

    // Cursor on first line by default.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ستون = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_سېټ_curswant = true;
    curwin->w_topline_was_سېټ = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // Make sure the buffer is loaded.
    إذا (curbuf->b_ml.ml_mfp == عدم)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	إذا (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_بفر(FALSE, عدم, 0);
    }
    وإلا
    {
	إذا (!پیغام_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(فراغ)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#که_تعریف_شوی FEAT_DIFF
	curwin->w_topfill = 0;
#د_که_پای
	apply_autocmds(EVENT_BUFENTER, عدم, عدم, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, عدم, عدم, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    إذا (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    إذا (curwin->w_topline == 1 && !curwin->w_topline_was_سېټ)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#که_تعریف_شوی FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#د_که_پای

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#که_تعریف_شوی FEAT_KEYMAP
    إذا (curbuf->b_kmap_state & KEYMAP_INIT)
	(فراغ)keymap_init();
#د_که_پای
#که_تعریف_شوی FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    إذا (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(فراغ)parse_spelllang(curwin);
#د_که_پای
#که_تعریف_شوی FEAT_VIMINFO
    curbuf->b_وروستی_کارول_شوی = vim_وخت();
#د_که_پای

    redraw_later(UPD_NOT_VALID);
}

#إذا defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    فراغ
do_autochdir(فراغ)
{
    إذا ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != عدم
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	وروستی_chdir_reason = "autochdir";
    }
}
#د_که_پای

    فراغ
no_write_پیغام_buf(buf_T *buf)
{
#که_تعریف_شوی FEAT_TERMINAL
    إذا (غړی_دنده_running(buf->b_غړی))
	emsg(_(e_دنده_still_running_جمع_کړه_bang_منزل_پای_ټکی_the_دنده));
    وإلا
#د_که_پای
	semsg(_(e_no_write_since_وروستی_change_for_بفر_nr_جمع_کړه_bang_منزل_override),
		buf->b_fnum);
}

    فراغ
no_write_پیغام(فراغ)
{
#که_تعریف_شوی FEAT_TERMINAL
    إذا (غړی_دنده_running(curbuf->b_غړی))
	emsg(_(e_دنده_still_running_جمع_کړه_bang_منزل_پای_ټکی_the_دنده));
    وإلا
#د_که_پای
	emsg(_(e_no_write_since_وروستی_change_جمع_کړه_bang_منزل_override));
}

    فراغ
no_write_پیغام_nobang(buf_T *buf ناکارول_شوی)
{
#که_تعریف_شوی FEAT_TERMINAL
    إذا (غړی_دنده_running(buf->b_غړی))
	emsg(_(e_دنده_still_running));
    وإلا
#د_که_پای
	emsg(_(e_no_write_since_وروستی_change));
}

/*
 * functions for dealing with the buffer list
 */

/*
 * Return TRUE if the current buffer is empty, unnamed, unmodified and used in
 * only one window.  That means it can be re-used.
 */
    صحيح
curbuf_reusable(فراغ)
{
    أرجع (curbuf != عدم
	&& curbuf->b_ffname == عدم
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == عدم || BUFEMPTY())
	&& !bt_quickfix(curbuf)
	&& !curbuf_Is_بدل_شوی());
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
    صحيح		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#که_تعریف_شوی UNIX
    stat_T	st;
#د_که_پای

    إذا (سر_file_عدد == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#که_تعریف_شوی UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    إذا (sfname == عدم || mch_stat((حرف *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#د_که_پای
    إذا (ffname != عدم && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#که_تعریف_شوی UNIX
		buflist_findname_stat(ffname, &st)
#وإلا
		buflist_findname(ffname)
#د_که_پای
		) != عدم)
    {
	vim_free(ffname);
	إذا (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? عدم : curwin,
						      lnum, (colnr_T)0, FALSE);

	إذا ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	إذا ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    سېټ_bufref(&bufref, buf);
	    إذا (!(flags & BLN_DUMMY))
	    {
		إذا (apply_autocmds(EVENT_BUFADD, عدم, عدم, FALSE, buf)
			&& !bufref_valid(&bufref))
		    أرجع عدم;
	    }
	}
	أرجع buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = عدم;
    إذا ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	سېټ_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#که_تعریف_شوی FEAT_EVAL
	إذا (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    أرجع عدم;
	}
#د_که_پای
	إذا (!bufref_valid(&bufref))
	    buf = عدم;		// buf was deleted; allocate a new buffer
    }
    إذا (buf != curbuf || curbuf == عدم)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	إذا (buf == عدم)
	{
	    vim_free(ffname);
	    أرجع عدم;
	}
#که_تعریف_شوی FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	إذا (buf->b_vars == عدم)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    أرجع عدم;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#د_که_پای
	init_changedtick(buf);
    }

    إذا (ffname != عدم)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    إذا ((ffname != عدم && (buf->b_ffname == عدم || buf->b_sfname == عدم))
	    || buf->b_wininfo == عدم)
    {
	إذا (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	وإلا
	    buf->b_sfname = عدم;
	VIM_CLEAR(buf->b_ffname);
	إذا (buf != curbuf)
	    free_بفر(buf);
	أرجع عدم;
    }

    إذا (buf == curbuf)
    {
	free_بفر_stuff(buf, FALSE);	// delete local variables et al.

	// Init the options.
	buf->b_p_initialized = false;
	buf_copy_options(buf, BCO_ENTER);

#که_تعریف_شوی FEAT_KEYMAP
	// need to reload lmaps and set b:keymap_name
	curbuf->b_kmap_state |= KEYMAP_INIT;
#د_که_پای
    }
    وإلا
    {
	// put the new buffer at the end of the buffer list
	buf->b_بل = عدم;
	إذا (firstbuf == عدم)		// buffer list is empty
	{
	    buf->b_prev = عدم;
	    firstbuf = buf;
	}
	وإلا				// append new buffer at end of list
	{
	    lastbuf->b_بل = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	إذا ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((صحيح *)buf_reuse.ga_معلومات)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    طالما (buf->b_prev != عدم && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_بل = buf->b_بل;
		إذا (prev->b_بل != عدم)
		    prev->b_بل->b_prev = prev;
		buf->b_بل = prev;
		buf->b_prev = prev->b_prev;
		إذا (buf->b_prev != عدم)
		    buf->b_prev->b_بل = buf;
		prev->b_prev = buf;
		إذا (lastbuf == buf)
		    lastbuf = prev;
		إذا (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	وإلا
	    buf->b_fnum = سر_file_عدد++;
	إذا (سر_file_عدد < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    إذا (emsg_silent == 0 && !دننه_assert_fails)
	    {
		بهر_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    سر_file_عدد = 1;
	}
	buf_hashtab_جمع_کړه(buf);

	// Always copy the options from the current buffer.
	buf_copy_options(buf, BCO_ALWAYS);
    }

    buf->b_wininfo->wi_fpos.lnum = lnum;
    buf->b_wininfo->wi_win = curwin;

#که_تعریف_شوی FEAT_SYN_HL
    hash_init(&buf->b_s.b_keywtab);
    hash_init(&buf->b_s.b_keywtab_ic);
#د_که_پای

    buf->b_fname = buf->b_sfname;
#که_تعریف_شوی UNIX
    إذا (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    وإلا
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#د_که_پای
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    إذا (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_نومونه(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    إذا (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	سېټ_bufref(&bufref, buf);
	إذا (apply_autocmds(EVENT_BUFNEW, عدم, عدم, FALSE, buf)
		&& !bufref_valid(&bufref))
	    أرجع عدم;
	إذا (flags & BLN_LISTED)
	{
	    إذا (apply_autocmds(EVENT_BUFADD, عدم, عدم, FALSE, buf)
		    && !bufref_valid(&bufref))
		أرجع عدم;
	}
#که_تعریف_شوی FEAT_EVAL
	إذا (aborting())		// autocmds may abort script processing
	    أرجع عدم;
#د_که_پای
    }

    أرجع buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    فراغ
free_buf_options(
    buf_T	*buf,
    صحيح		free_p_ff)
{
    إذا (free_p_ff)
    {
	clear_string_option(&buf->b_p_fenc);
	clear_string_option(&buf->b_p_ff);
	clear_string_option(&buf->b_p_bh);
	clear_string_option(&buf->b_p_bt);
    }
#که_تعریف_شوی FEAT_پیدا_کړه_ID
    clear_string_option(&buf->b_p_def);
    clear_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    clear_string_option(&buf->b_p_inex);
# endif
#د_که_پای
#إذا defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#د_که_پای
#إذا defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#د_که_پای
#إذا defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#د_که_پای
    clear_string_option(&buf->b_p_fp);
#إذا defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#د_که_پای
#که_تعریف_شوی FEAT_CRYPT
# ifdef FEAT_SODIUM
    إذا (buf->b_p_کلی != عدم && *buf->b_p_کلی != NUL
			   && crypt_لاره_is_sodium(crypt_get_لاره_nr(buf)))
	crypt_sodium_munlock(buf->b_p_کلی, STRLEN(buf->b_p_کلی));
# endif
    clear_string_option(&buf->b_p_کلی);
#د_که_پای
    clear_string_option(&buf->b_p_kp);
    clear_string_option(&buf->b_p_mps);
    clear_string_option(&buf->b_p_fo);
    clear_string_option(&buf->b_p_flp);
    clear_string_option(&buf->b_p_isk);
#که_تعریف_شوی FEAT_VARTABS
    clear_string_option(&buf->b_p_vsts);
    VIM_CLEAR(buf->b_p_vsts_nopaste);
    VIM_CLEAR(buf->b_p_vsts_array);
    clear_string_option(&buf->b_p_vts);
    VIM_CLEAR(buf->b_p_vts_array);
#د_که_پای
#که_تعریف_شوی FEAT_KEYMAP
    clear_string_option(&buf->b_p_keymap);
    keymap_clear(&buf->b_kmap_ga);
    ga_clear(&buf->b_kmap_ga);
#د_که_پای
    clear_string_option(&buf->b_p_com);
    clear_string_option(&buf->b_p_cms);
    clear_string_option(&buf->b_p_nf);
#که_تعریف_شوی FEAT_SYN_HL
    clear_string_option(&buf->b_p_syn);
    clear_string_option(&buf->b_s.b_syn_isk);
#د_که_پای
#که_تعریف_شوی FEAT_SPELL
    clear_string_option(&buf->b_s.b_p_spc);
    clear_string_option(&buf->b_s.b_p_spf);
    vim_regfree(buf->b_s.b_cap_prog);
    buf->b_s.b_cap_prog = عدم;
    clear_string_option(&buf->b_s.b_p_spl);
    clear_string_option(&buf->b_s.b_p_spo);
#د_که_پای
    clear_string_option(&buf->b_p_sua);
    clear_string_option(&buf->b_p_ft);
    clear_string_option(&buf->b_p_cink);
    clear_string_option(&buf->b_p_cino);
    clear_string_option(&buf->b_p_lop);
    clear_string_option(&buf->b_p_cinsd);
    clear_string_option(&buf->b_p_cinw);
    clear_string_option(&buf->b_p_cot);
    clear_string_option(&buf->b_p_cpt);
#که_تعریف_شوی BACKSLASH_دننه_FILENAME
    clear_string_option(&buf->b_p_csl);
#د_که_پای
#که_تعریف_شوی FEAT_COMPL_FUNC
    clear_string_option(&buf->b_p_cfu);
    free_callback(&buf->b_cfu_cb);
    clear_string_option(&buf->b_p_ofu);
    free_callback(&buf->b_ofu_cb);
    clear_string_option(&buf->b_p_tsrfu);
    free_callback(&buf->b_tsrfu_cb);
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_شمېر);
    buf->b_p_cpt_شمېر = 0;
#د_که_پای
#که_تعریف_شوی FEAT_QUICKFIX
    clear_string_option(&buf->b_p_gefm);
    clear_string_option(&buf->b_p_gp);
    clear_string_option(&buf->b_p_mp);
    clear_string_option(&buf->b_p_efm);
#د_که_پای
    clear_string_option(&buf->b_p_ep);
    clear_string_option(&buf->b_p_لار);
    clear_string_option(&buf->b_p_نښانونه);
    clear_string_option(&buf->b_p_tc);
#که_تعریف_شوی FEAT_EVAL
    clear_string_option(&buf->b_p_tfu);
    free_callback(&buf->b_tfu_cb);
    clear_string_option(&buf->b_p_ffu);
    free_callback(&buf->b_ffu_cb);
#د_که_پای
    clear_string_option(&buf->b_p_dict);
#که_تعریف_شوی FEAT_DIFF
    clear_string_option(&buf->b_p_dia);
#د_که_پای
    clear_string_option(&buf->b_p_tsr);
    clear_string_option(&buf->b_p_qe);
    buf->b_p_ac = -1;
    buf->b_p_ar = -1;
#که_تعریف_شوی HAVE_FSYNC
    buf->b_p_fs = -1;
#د_که_پای
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
    صحيح
buflist_getfile(
    صحيح		n,
    linenr_T	lnum,
    صحيح		options,
    صحيح		forceit)
{
    buf_T	*buf;
    win_T	*wp = عدم;
    pos_T	*fpos;
    colnr_T	ستون;

    buf = buflist_findnr(n);
    إذا (buf == عدم)
    {
	إذا ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	وإلا
	    semsg(_(e_بفر_nr_not_وموندل), n);
	أرجع FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    إذا (buf == curbuf)
	أرجع OK;

    إذا (متن_or_buf_locked())
	أرجع FAIL;

    // altfpos may be changed by getfile(), get it now
    إذا (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	ستون = fpos->ستون;
    }
    وإلا
	ستون = 0;

    إذا (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	إذا (wp == عدم && (swb_flags & (SWB_VSPLIT | SWB_وېشل | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    إذا (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    وإلا إذا (win_وېشل(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		أرجع FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    صحيح retval = FAIL;
    إذا (GETFILE_SUCCESS(getfile(buf->b_fnum, عدم, عدم,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	إذا (!p_sol && ستون != 0)
	{
	    curwin->w_cursor.ستون = ستون;
	    check_cursor_ستون();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_سېټ_curswant = true;
	}
	retval = OK;
    }

    إذا (RedrawingDisabled > 0)
	--RedrawingDisabled;
    أرجع retval;
}

/*
 * go to the last know line number for the current buffer
 */
    ساكن فراغ
buflist_getfpos(فراغ)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    إذا (p_sol)
	curwin->w_cursor.ستون = 0;
    وإلا
    {
	curwin->w_cursor.ستون = fpos->ستون;
	check_cursor_ستون();
	curwin->w_cursor.coladd = 0;
	curwin->w_سېټ_curswant = true;
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
    buf_T	*buf = عدم;

    // First make the name into a full path name
    ffname = ډک_Name_save(fname,
#که_تعریف_شوی UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#وإلا
	    FALSE
#د_که_پای
	    );
    إذا (ffname != عدم)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    أرجع buf;
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
#که_تعریف_شوی UNIX
    stat_T	st;

    إذا (mch_stat((حرف *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    أرجع buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    ساكن buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#د_که_پای
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_وروستی(buf)
	إذا ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#که_تعریف_شوی UNIX
		    , stp
#د_که_پای
		    ))
	    أرجع buf;
    أرجع عدم;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    صحيح
buflist_findpat(
    char_u	*نمونه,
    char_u	*نمونه_پای_ټکی,	// pointer to first char after pattern
    صحيح		unlisted,	// find unlisted buffers
    صحيح		diffmode ناکارول_شوی, // find diff-mode buffers only
    صحيح		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    صحيح		سمون = -1;
    صحيح		پیدا_کړه_listed;
    char_u	*pat;
    char_u	*patend;
    صحيح		attempt;
    char_u	*p;
    صحيح		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    إذا ((نمونه_پای_ټکی == نمونه + 1 && (*نمونه == '%' || *نمونه == '#'))
	    || (دننه_vim9script() && نمونه_پای_ټکی == نمونه + 2
				    && نمونه[0] == '%' && نمونه[1] == '%'))
    {
	إذا (*نمونه == '#' || نمونه_پای_ټکی == نمونه + 2)
	    سمون = curwin->w_alt_fnum;
	وإلا
	    سمون = curbuf->b_fnum;
#که_تعریف_شوی FEAT_DIFF
	إذا (diffmode && !diff_ډېرپېښ_buf(buflist_findnr(سمون)))
	    سمون = -1;
#د_که_پای
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
    وإلا
    {
	pat = file_pat_منزل_reg_pat(نمونه, نمونه_پای_ټکی, عدم, FALSE);
	إذا (pat == عدم)
	    أرجع -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	پیدا_کړه_listed = TRUE;
	لكل (;;)
	{
	    لكل (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		إذا (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		إذا (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_وروستی(buf)
		{
		    إذا (regmatch.regprog == عدم)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			أرجع -1;
		    }
		    إذا (buf->b_p_bl == پیدا_کړه_listed
#که_تعریف_شوی FEAT_DIFF
			    && (!diffmode || diff_ډېرپېښ_buf(buf))
#د_که_پای
			    && buflist_سمون(&regmatch, buf, FALSE) != عدم)
		    {
			إذا (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				إذا (wp->w_بفر == buf)
				    اكسر;
			    إذا (wp == عدم)
				استمر;
			}
			إذا (سمون >= 0)		// already found a match
			{
			    سمون = -2;
			    اكسر;
			}
			سمون = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		إذا (سمون >= 0)			// found one match
		    اكسر;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    إذا (!unlisted || !پیدا_کړه_listed || سمون != -1)
		اكسر;
	    پیدا_کړه_listed = FALSE;
	}

	vim_free(pat);
    }

    إذا (سمون == -2)
	semsg(_(e_نور_than_one_سمون_for_str), نمونه);
    وإلا إذا (سمون < 0)
	semsg(_(e_no_matching_بفر_for_str), نمونه);
    أرجع سمون;
}

#که_تعریف_شوی FEAT_VIMINFO
تعريفنوع بنية {
    buf_T   *buf;
    char_u  *سمون;
} bufmatch_T;
#د_که_پای

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    صحيح
ExpandBufnames(
    char_u	*pat,
    صحيح		*عدد_file,
    char_u	***file,
    صحيح		options)
{
    صحيح		شمېر;
    buf_T	*buf;
    صحيح		پړاو;
    char_u	*p;
    char_u	*patc = عدم;
#که_تعریف_شوی FEAT_VIMINFO
    bufmatch_T	*matches = عدم;
#د_که_پای
    صحيح		fuzzy;
    fuzmatch_str_T  *fuzmatch = عدم;
    regmatch_T	regmatch;
    صحيح		score = 0;
    صحيح		منزل_free = FALSE;

    *عدد_file = 0;		    // return values in case of FAIL
    *file = عدم;

#که_تعریف_شوی FEAT_DIFF
    إذا ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	أرجع FAIL;
#د_که_پای

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    إذا (!fuzzy)
    {
	إذا (*pat == '^' && pat[1] != NUL)
	{
	    صحيح  len = (صحيح)STRLEN(pat);
	    patc = alloc(len);
	    إذا (patc == عدم)
		أرجع FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    منزل_free = TRUE;
	}
	وإلا إذا (*pat == '^')
	    patc = (char_u *)"";
	وإلا
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    لكل (پړاو = 1; پړاو <= 2; ++پړاو)
    {
	شمېر = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    إذا (!buf->b_p_bl)	// skip unlisted buffers
		استمر;
#که_تعریف_شوی FEAT_DIFF
	    إذا (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		إذا (buf == curbuf || !diff_ډېرپېښ_buf(buf))
		    استمر;
#د_که_پای

	    إذا (!fuzzy)
	    {
		إذا (regmatch.regprog == عدم)
		{
		    // invalid pattern, possibly after recompiling
		    إذا (منزل_free)
			vim_free(patc);
		    أرجع FAIL;
		}
		p = buflist_سمون(&regmatch, buf, p_wic);
	    }
	    وإلا
	    {
		p = عدم;
		// first try matching with the short file name
		إذا ((score = fuzzy_سمون_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		إذا (p == عدم)
		{
		    // next try matching with the full path file name
		    إذا ((score = fuzzy_سمون_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    إذا (p == عدم)
		استمر;

	    إذا (پړاو == 1)
	    {
		++شمېر;
		استمر;
	    }

	    إذا (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    وإلا
		p = vim_strsave(p);
	    إذا (p == عدم)
	    {
		إذا (fuzzy && پړاو == 2)
		    fuzmatch_str_free(fuzmatch, شمېر);
		أرجع FAIL;
	    }

	    إذا (!fuzzy)
	    {
#که_تعریف_شوی FEAT_VIMINFO
		إذا (matches != عدم)
		{
		    matches[شمېر].buf = buf;
		    matches[شمېر].سمون = p;
		    شمېر++;
		}
		وإلا
#د_که_پای
		    (*file)[شمېر++] = p;
	    }
	    وإلا
	    {
		fuzmatch[شمېر].idx = شمېر;
		fuzmatch[شمېر].str = p;
		fuzmatch[شمېر].score = score;
		شمېر++;
	    }
	}
	إذا (شمېر == 0)	// no match found, break here
	    اكسر;
	إذا (پړاو == 1)
	{
	    إذا (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, شمېر);
		إذا (*file == عدم)
		{
		    vim_regfree(regmatch.regprog);
		    إذا (منزل_free)
			vim_free(patc);
		    أرجع FAIL;
		}
#که_تعریف_شوی FEAT_VIMINFO
		إذا (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, شمېر);
#د_که_پای
	    }
	    وإلا
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, شمېر);
		إذا (fuzmatch == عدم)
		{
		    *عدد_file = 0;
		    *file = عدم;
		    أرجع FAIL;
		}
	    }
	}
    }

    إذا (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	إذا (منزل_free)
	    vim_free(patc);
    }

    إذا (!fuzzy)
    {
#که_تعریف_شوی FEAT_VIMINFO
	إذا (matches != عدم)
	{
	    صحيح i;
	    إذا (شمېر > 1)
		qsort(matches, شمېر, حجم(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    إذا (matches[0].buf == curbuf)
	    {
		لكل (i = 1; i < شمېر; i++)
		    (*file)[i-1] = matches[i].سمون;
		(*file)[شمېر-1] = matches[0].سمون;
	    }
	    وإلا
	    {
		لكل (i = 0; i < شمېر; i++)
		    (*file)[i] = matches[i].سمون;
	    }
	    vim_free(matches);
	}
#د_که_پای
    }
    وإلا
    {
	إذا (fuzzymatches_منزل_strmatches(fuzmatch, file, شمېر, FALSE) == FAIL)
	    أرجع FAIL;
    }

    *عدد_file = شمېر;
    أرجع (شمېر == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    ساكن char_u *
buflist_سمون(
    regmatch_T	*rmp,
    buf_T	*buf,
    صحيح		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*سمون;

    // First try the short file name, then the long file name.
    سمون = fname_سمون(rmp, buf->b_sfname, ignore_case);
    إذا (سمون == عدم && rmp->regprog != عدم)
	سمون = fname_سمون(rmp, buf->b_ffname, ignore_case);

    أرجع سمون;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    ساكن char_u *
fname_سمون(
    regmatch_T	*rmp,
    char_u	*name,
    صحيح		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*سمون = عدم;
    char_u	*p;

    // extra check for valid arguments
    إذا (name == عدم || rmp->regprog == عدم)
	أرجع عدم;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    إذا (vim_regexec(rmp, name, (colnr_T)0))
	سمون = name;
    وإلا إذا (rmp->regprog != عدم)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(عدم, name);
	إذا (p != عدم && vim_regexec(rmp, p, (colnr_T)0))
	    سمون = name;
	vim_free(p);
    }

    أرجع سمون;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(صحيح nr)
{
    char_u	کلی[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    إذا (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((حرف *)کلی, حجم(کلی), "%x", nr);
    hi = hash_پیدا_کړه(&buf_hashtab, کلی);

    إذا (!HASHITEM_تشه(hi))
	أرجع (buf_T *)(hi->hi_کلی
			     - ((بینښه)(curbuf->b_کلی - (char_u *)curbuf)));
    أرجع عدم;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    صحيح		n,
    صحيح		fullname,
    صحيح		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    إذا (buf == عدم)
	أرجع عدم;
    أرجع home_replace_save(helptail ? buf : عدم,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    فراغ
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	ستون,
    صحيح		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	إذا (wip->wi_win == win)
	    اكسر;
    إذا (wip == عدم)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	إذا (wip == عدم)
	    أرجع;
	wip->wi_win = win;
	إذا (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    وإلا
    {
	// remove the entry from the list
	إذا (wip->wi_prev)
	    wip->wi_prev->wi_بل = wip->wi_بل;
	وإلا
	    buf->b_wininfo = wip->wi_بل;
	إذا (wip->wi_بل)
	    wip->wi_بل->wi_prev = wip->wi_prev;
	إذا (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#که_تعریف_شوی FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#د_که_پای
	}
    }
    إذا (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.ستون = ستون;
    }
    إذا (win != عدم)
	wip->wi_changelistidx = win->w_changelistidx;
    إذا (copy_options && win != عدم)
    {
	// Save the window-specific option values.
	copy_winopt(&win->w_onebuf_opt, &wip->wi_opt);
#که_تعریف_شوی FEAT_FOLDING
	wip->wi_fold_manual = win->w_fold_manual;
	cloneFoldGrowArray(&win->w_folds, &wip->wi_folds);
#د_که_پای
	wip->wi_optset = TRUE;
    }

    // insert the entry in front of the list
    wip->wi_بل = buf->b_wininfo;
    buf->b_wininfo = wip;
    wip->wi_prev = عدم;
    إذا (wip->wi_بل)
	wip->wi_بل->wi_prev = wip;
}

#که_تعریف_شوی FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    ساكن صحيح
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    إذا (!wip->wi_opt.wo_diff)
	أرجع FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	إذا (wip->wi_win == wp)
	    أرجع FALSE;
    أرجع TRUE;
}
#د_که_پای

/*
 * Find info for the current window in buffer "buf".
 * If not found, return the info for the most recently used window.
 * When "need_options" is TRUE skip entries where wi_optset is FALSE.
 * When "skip_diff_buffer" is TRUE avoid windows with 'diff' set that is in
 * another tab page.
 * Returns NULL when there isn't any info.
 */
    ساكن wininfo_T *
پیدا_کړه_wininfo(
    buf_T	*buf,
    صحيح		need_options,
    صحيح		skip_diff_بفر ناکارول_شوی)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	إذا (wip->wi_win == curwin
#که_تعریف_شوی FEAT_DIFF
		&& (!skip_diff_بفر || !wininfo_other_tab_diff(wip))
#د_که_پای

		&& (!need_options || wip->wi_optset))
	    اكسر;

    إذا (wip != عدم)
	أرجع wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#که_تعریف_شوی FEAT_DIFF
    إذا (skip_diff_بفر)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    إذا (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != عدم
			    && wip->wi_win->w_بفر == buf)))
		اكسر;
    }
    وإلا
#د_که_پای
	wip = buf->b_wininfo;
    أرجع wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    فراغ
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#که_تعریف_شوی FEAT_FOLDING
    clearFolding(curwin);
#د_که_پای

    wip = پیدا_کړه_wininfo(buf, TRUE, TRUE);
    إذا (wip != عدم && wip->wi_win != عدم
	    && wip->wi_win != curwin && wip->wi_win->w_بفر == buf)
    {
	// The buffer is currently displayed in the window: use the actual
	// option values instead of the saved (possibly outdated) values.
	win_T *wp = wip->wi_win;

	copy_winopt(&wp->w_onebuf_opt, &curwin->w_onebuf_opt);
#که_تعریف_شوی FEAT_STL_OPT
	// w_stl_rendered_height is not in winvar_T; copy it explicitly.
	curwin->w_stl_rendered_لوړوالی = wp->w_stl_rendered_لوړوالی;
#د_که_پای
#که_تعریف_شوی FEAT_FOLDING
	curwin->w_fold_manual = wp->w_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wp->w_folds, &curwin->w_folds);
#د_که_پای
    }
    وإلا إذا (wip != عدم && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#که_تعریف_شوی FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#د_که_پای
    }
    وإلا
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    إذا (wip != عدم)
	curwin->w_changelistidx = wip->wi_changelistidx;

#که_تعریف_شوی FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    إذا (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#د_که_پای
#که_تعریف_شوی FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    إذا (*curwin->w_p_stl != NUL)
	update_stl_rendered_لوړوالی(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    إذا (curwin->w_حالت_لوړوالی > 0
	    && curwin->w_حالت_لوړوالی != statusline_لوړوالی(curwin))
    {
	چوکاټ_change_statusline_لوړوالی();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	إذا (p_ea)
	    win_equal(curwin, TRUE, 'v');
    }
#د_که_پای
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
    ساكن pos_T no_position = {1, 0, 0};

    wip = پیدا_کړه_wininfo(buf, FALSE, FALSE);
    إذا (wip != عدم)
	أرجع &(wip->wi_fpos);
    وإلا
	أرجع &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    أرجع buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    فراغ
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    صحيح		len;
    صحيح		i;
    صحيح		ro_char;
    صحيح		بدل_شوی_char;
#که_تعریف_شوی FEAT_TERMINAL
    صحيح		دنده_running;
    صحيح		دنده_none_open;
#د_که_پای

#که_تعریف_شوی FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_معلومات = عدم, **p;

    إذا (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, حجم(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    إذا (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_معلومات)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_معلومات, (size_t)buflist.ga_len,
		حجم(buf_T *), buf_compare);

	buflist_معلومات = (buf_T **)buflist.ga_معلومات;
	buf = *buflist_معلومات;
    }
    p = buflist_معلومات;

    لكل (; buf != عدم && !got_int; buf = buflist_معلومات != عدم
	    ? (++p < buflist_معلومات + buflist.ga_len ? *p : عدم)
	    : buf->b_بل)
#وإلا
    لكل (buf = firstbuf; buf != عدم && !got_int; buf = buf->b_بل)
#د_که_پای
    {
	char_u	*name;

#که_تعریف_شوی FEAT_TERMINAL
	دنده_running = غړی_دنده_running(buf->b_غړی);
	دنده_none_open = غړی_none_open(buf->b_غړی);
#د_که_پای
	// skip unlisted buffers, unless ! was used
	إذا ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !buf_Is_بدل_شوی(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == عدم || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == عدم || buf->b_nwindows != 0))
#که_تعریف_شوی FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!دنده_running || (دنده_running && دنده_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!دنده_running || (دنده_running && !دنده_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (دنده_running || buf->b_غړی == عدم))
#د_که_پای
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    استمر;
	name = buf_spname(buf);
	إذا (name != عدم)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	وإلا
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	إذا (پیغام_filtered(NameBuff))
	    استمر;

	بدل_شوی_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (buf_Is_بدل_شوی(buf) ? '+' : ' ');
#که_تعریف_شوی FEAT_TERMINAL
	إذا (دنده_running)
	{
	    إذا (دنده_none_open)
		ro_char = '?';
	    وإلا
		ro_char = 'R';
	    بدل_شوی_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	وإلا إذا (buf->b_غړی != عدم)
	    ro_char = 'F';
	وإلا
#د_که_پای
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	پیغام_putchar('\n');
	len = (صحيح)vim_snprintf_safelen((حرف *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == عدم ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		بدل_شوی_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	افعل
	    IObuff[len++] = ' ';
	طالما (--i > 0 && len < IOSIZE - 18);
#که_تعریف_شوی FEAT_VIMINFO
	إذا (vim_strchr(eap->arg, 't') && buf->b_وروستی_کارول_شوی)
	    جمع_کړه_وخت(IObuff + len, (size_t)(IOSIZE - len), buf->b_وروستی_کارول_شوی);
	وإلا
#د_که_پای
	    vim_snprintf((حرف *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (اوږد)buflist_findlnum(buf));
	پیغام_outtrans(IObuff);
	بهر_flush();	    // output one line at a time
	ui_breakcheck();
    }

#که_تعریف_شوی FEAT_VIMINFO
    إذا (buflist_معلومات)
	ga_clear(&buflist);
#د_که_پای
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    صحيح
buflist_name_nr(
    صحيح		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    إذا (buf == عدم || buf->b_fname == عدم)
	أرجع FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    أرجع OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    صحيح
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    صحيح		پیغام_2)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = عدم;
#که_تعریف_شوی UNIX
    stat_T	st;
#د_که_پای

    إذا (ffname == عدم || *ffname == NUL)
    {
	// Removing the name.
	إذا (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	وإلا
	    buf->b_sfname = عدم;
	VIM_CLEAR(buf->b_ffname);
#که_تعریف_شوی UNIX
	st.st_dev = (dev_T)-1;
#د_که_پای
    }
    وإلا
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	إذا (ffname == عدم)		    // out of memory
	    أرجع FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#که_تعریف_شوی UNIX
	إذا (mch_stat((حرف *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#د_که_پای
	إذا (!(buf->b_flags & BF_DUMMY))
#که_تعریف_شوی UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#وإلا
	    obuf = buflist_findname(ffname);
#د_که_پای
	إذا (obuf != عدم && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    صحيح		دننه_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		إذا (win->w_بفر == obuf)
		    دننه_use = TRUE;

	    // it's loaded or used in a window, fail
	    إذا (obuf->b_ml.ml_mfp != عدم || دننه_use)
	    {
		إذا (پیغام_2)
		    emsg(_(e_بفر_with_this_name_already_exists));
		vim_free(ffname);
		أرجع FAIL;
	    }
	    // delete from the list
	    close_بفر(عدم, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	إذا (ffname == عدم || sfname == عدم)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    أرجع FAIL;
	}
#که_تعریف_شوی USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#د_که_پای
	إذا (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#که_تعریف_شوی UNIX
    إذا (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    وإلا
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#د_که_پای

    buf->b_shortname = false;

    buf_name_بدل_شوی(buf);
    أرجع OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    فراغ
buf_سېټ_name(صحيح fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    إذا (buf == عدم)
	أرجع;

    إذا (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = عدم;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    فراغ
buf_name_بدل_شوی(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    إذا (buf->b_ml.ml_mfp != عدم)
	ml_setname(buf);

#که_تعریف_شوی FEAT_TERMINAL
    إذا (buf->b_غړی != عدم)
	غړی_clear_حالت_متن(buf->b_غړی);
#د_که_پای

    إذا (curwin->w_بفر == buf)
	check_arg_idx(curwin);	// check file name for arg list
    maketitle();		// set window title
    حالت_redraw_all();	// status lines need to be redrawn
    fmarks_check_نومونه(buf);	// check named file marks
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
    إذا (buf != عدم && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    أرجع buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    صحيح		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    إذا (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	إذا (errmsg)
	    emsg(_(e_no_alternate_file));
	أرجع عدم;
    }
    أرجع fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    صحيح
buflist_جمع_کړه(char_u *fname, صحيح flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, عدم, (linenr_T)0, flags);
    إذا (buf != عدم)
	أرجع buf->b_fnum;
    أرجع 0;
}

#إذا defined(BACKSLASH_دننه_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    فراغ
buflist_slash_adjust(فراغ)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	إذا (bp->b_ffname != عدم)
	    slash_adjust(bp->b_ffname);
	إذا (bp->b_sfname != عدم)
	    slash_adjust(bp->b_sfname);
    }
}
#د_که_پای

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    فراغ
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.ستون, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    صحيح
otherfile(char_u *ffname)
{
    أرجع otherfile_buf(curbuf, ffname
#که_تعریف_شوی UNIX
	    , عدم
#د_که_پای
	    );
}

    ساكن صحيح
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#که_تعریف_شوی UNIX
    , stat_T		*stp
#د_که_پای
    )
{
    // no name is different
    إذا (ffname == عدم || *ffname == NUL || buf->b_ffname == عدم)
	أرجع TRUE;
    إذا (fnamecmp(ffname, buf->b_ffname) == 0)
	أرجع FALSE;
#که_تعریف_شوی UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	إذا (stp == عدم)
	{
	    إذا (!buf->b_dev_valid || mch_stat((حرف *)ffname, &st) < 0)
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
	إذا (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    إذا (buf_same_ino(buf, stp))
		أرجع FALSE;
	}
    }
#د_که_پای
    أرجع TRUE;
}

#إذا defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    فراغ
buf_setino(buf_T *buf)
{
    stat_T	st;

    إذا (buf->b_fname != عدم && mch_stat((حرف *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    وإلا
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    ساكن صحيح
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    أرجع (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#د_که_پای

/*
 * Print info about the current buffer.
 */
    فراغ
fileinfo(
    صحيح fullname,	    // when non-zero print full path
    صحيح shorthelp,
    صحيح	dont_truncate)
{
    char_u	*name;
    حرف	*بفر;
    size_t	bufferlen = 0;

    بفر = alloc(IOSIZE);
    إذا (بفر == عدم)
	أرجع;

    إذا (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(بفر, IOSIZE, "buf %d: ", curbuf->b_fnum);

    بفر[bufferlen++] = '"';

    name = buf_spname(curbuf);
    إذا (name != عدم)
	bufferlen += vim_snprintf_safelen(بفر + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    وإلا
    {
	إذا (!fullname && curbuf->b_fname != عدم)
	    name = curbuf->b_fname;
	وإلا
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : عدم, name,
	    (char_u *)بفر + bufferlen, IOSIZE - (صحيح)bufferlen, TRUE);
    }

    bufferlen += vim_snprintf_safelen(
	بفر + bufferlen,
	IOSIZE - bufferlen,
	"\"%s%s%s%s%s%s",
	curbuf_Is_بدل_شوی() ? (shortmess(SHM_MOD)
	    ?  " [+]" : _(" [Modified]")) : " ",
	(curbuf->b_flags & BF_NOTEDITED) && !bt_dontwrite(curbuf)
	    ? _("[Not edited]") : "",
	(curbuf->b_flags & BF_NEW) && !bt_dontwrite(curbuf)
	    ? new_file_پیغام() : "",
	(curbuf->b_flags & BF_READERR) ? _("[Read errors]") : "", curbuf->b_p_ro
	    ? (shortmess(SHM_RO) ? _("[RO]") : _("[readonly]")) : "",
	(curbuf_Is_بدل_شوی() || (curbuf->b_flags & BF_WRITE_ماسک) || curbuf->b_p_ro)
	    ? " " : "");

    إذا (curbuf->b_ml.ml_flags & ML_تشه)
	bufferlen += vim_snprintf_safelen(بفر + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_کرښې_پیغام));
    وإلا إذا (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    بفر + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_کرښه_شمېر),
	    (اوږد)curbuf->b_ml.ml_کرښه_شمېر,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_کرښه_شمېر));
    وإلا
    {
	bufferlen += vim_snprintf_safelen(
	    بفر + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (اوږد)curwin->w_cursor.lnum,
	    (اوږد)curbuf->b_ml.ml_کرښه_شمېر,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_کرښه_شمېر));

	validate_virtcol();
	bufferlen += ستون_print((char_u *)بفر + bufferlen, IOSIZE - bufferlen,
		   (صحيح)curwin->w_cursor.ستون + 1, (صحيح)curwin->w_virtcol + 1);
    }

    (فراغ)append_arg_شمېره(curwin, (char_u *)بفر + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    إذا (dont_truncate)
    {
	صحيح	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	پیغام_پیل();
	n = پیغام_scroll;
	پیغام_scroll = TRUE;
	پیغام(بفر);
	پیغام_scroll = n;
    }
    وإلا
    {
	حرف	*p = پیغام_trunc_attr(بفر, FALSE, 0);
	إذا (restart_edit != 0 || (پیغام_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    سېټ_keep_پیغام((char_u *)p, 0);
    }

    vim_free(بفر);
}

    صحيح
ستون_print(
    char_u  *buf,
    size_t  buflen,
    صحيح	    ستون,
    صحيح	    vcol)
{
    إذا (ستون == vcol)
	أرجع (صحيح)vim_snprintf_safelen((حرف *)buf, buflen, "%d", ستون);

    أرجع (صحيح)vim_snprintf_safelen((حرف *)buf, buflen, "%d-%d", ستون, vcol);
}

ساكن char_u *lasttitle = عدم;
ساكن char_u *lasticon = عدم;

/*
 * Put the file name in the title bar and icon of the window.
 */
    فراغ
maketitle(فراغ)
{
    char_u	*title_str = عدم;
    char_u	*icon_str = عدم;
    صحيح		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    إذا (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	أرجع;
    }

    need_maketitle = FALSE;
    إذا (!p_title && !p_icon && lasttitle == عدم && lasticon == عدم)
	أرجع;  // nothing to do

    إذا (p_title)
    {
	صحيح maxlen = 0;

	إذا (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    إذا (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	إذا (*p_titlestring != NUL)
	{
#که_تعریف_شوی FEAT_STL_OPT
	    إذا (stl_syntax & STL_دننه_TITLE)
		build_stl_str_hl(curwin, title_str, حجم(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, عدم, عدم, عدم);
	    وإلا
#د_که_پای
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	وإلا
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#تعریف_کړه SPACE_FOR_FNAME (حجم(buf) - 100)
#تعریف_کړه SPACE_FOR_DIR   (حجم(buf) - 20)
#تعریف_کړه SPACE_FOR_ARGNR (حجم(buf) - 10)  // at least room for " - VIM"

	    // file name
	    إذا (curbuf->b_fname == عدم)
		buflen = vim_snprintf_safelen((حرف *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#که_تعریف_شوی FEAT_TERMINAL
	    وإلا إذا (curbuf->b_غړی != عدم)
		buflen = vim_snprintf_safelen((حرف *)buf,
		    SPACE_FOR_FNAME, "%s",
		    غړی_get_حالت_متن(curbuf->b_غړی));
#د_که_پای
	    وإلا
	    {
		buflen = vim_snprintf_safelen((حرف *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != عدم)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#که_تعریف_شوی FEAT_TERMINAL
	    إذا (curbuf->b_غړی == عدم)
#د_که_پای
	    {
		بدل (buf_Is_بدل_شوی(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    حالة 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (حرف *)buf + buflen,
			    حجم(buf) - buflen, " +");
			اكسر;
		    حالة 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (حرف *)buf + buflen,
			    حجم(buf) - buflen, " =");
			اكسر;
		    حالة 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (حرف *)buf + buflen,
			    حجم(buf) - buflen, " =+");
			اكسر;
		    حالة 4:
		    حالة 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (حرف *)buf + buflen,
			    حجم(buf) - buflen, " -");
			اكسر;
		    حالة 5:
		    حالة 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (حرف *)buf + buflen,
			    حجم(buf) - buflen, " -+");
			اكسر;
		    افتراضي:
			اكسر;
		}
	    }

	    // path (surrounded by '()')
	    إذا (curbuf->b_fname != عدم
#که_تعریف_شوی FEAT_TERMINAL
		    && curbuf->b_غړی == عدم
#د_که_پای
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((حرف *)buf + buflen,
		    حجم(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (صحيح)(SPACE_FOR_DIR - buflen), TRUE);

#که_تعریف_شوی BACKSLASH_دننه_FILENAME
		// avoid "c:/name" to be reduced to "c"
		إذا (خوندي_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#د_که_پای

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		إذا (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((حرف *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		وإلا
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    إذا (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((حرف *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != عدم)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    وإلا
			buflen += vim_snprintf_safelen((حرف *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_شمېره(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((حرف *)buf + buflen,
		حجم(buf) - buflen, " - %s",
#إذا defined(FEAT_CLIENTSERVER)
		(serverName != عدم)
		    ? serverName :
#د_که_پای
		    (char_u *)"VIM");

	    إذا (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		إذا (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, حجم(buf));
	    }
	}
    }
    mustset = ارزښت_بدل_شوی(title_str, &lasttitle);

    إذا (p_icon)
    {
	icon_str = buf;
	إذا (*p_iconstring != NUL)
	{
#که_تعریف_شوی FEAT_STL_OPT
	    إذا (stl_syntax & STL_دننه_ICON)
		build_stl_str_hl(curwin, icon_str, حجم(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, عدم, عدم,
				 عدم);
	    وإلا
#د_که_پای
		icon_str = p_iconstring;
	}
	وإلا
	{
	    char_u  *name;
	    صحيح	    namelen;

	    name = buf_spname(curbuf);
	    إذا (name == عدم)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (صحيح)STRLEN(name);
	    إذا (namelen > 100)
	    {
		namelen -= 100;
		إذا (has_mbyte)
		    namelen += (*mb_لکۍ_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, حجم(buf));
	}
    }

    mustset |= ارزښت_بدل_شوی(icon_str, &lasticon);

    إذا (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    ساكن صحيح
ارزښت_بدل_شوی(char_u *str, char_u **وروستی)
{
    إذا ((str == عدم) != (*وروستی == عدم)
	    || (str != عدم && *وروستی != عدم && STRCMP(str, *وروستی) != 0))
    {
	vim_free(*وروستی);
	إذا (str == عدم)
	{
	    *وروستی = عدم;
	    mch_restore_title(
		  وروستی == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	وإلا
	{
	    *وروستی = vim_strsave(str);
	    أرجع TRUE;
	}
    }
    أرجع FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    فراغ
resettitle(فراغ)
{
    mch_settitle(lasttitle, lasticon);
}

#إذا defined(EXITFREE)
    فراغ
free_titles(فراغ)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#د_که_پای


#إذا defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
تعريفنوع بنية
{
    char_u	*stl_پیل;
    صحيح		stl_minwid;
    صحيح		stl_maxwid;
    تعداد {
	نورمال,
	تشه,
	ډله_2,
	Separate,
	Highlight,
	Tab_پاڼه,
	ClickFunc,
	Trunc
    }		stl_type;
    char_u	*stl_clickfunc;	// function name for ClickFunc items
} stl_توکی_T;

ساكن size_t		stl_توکي_len = 20; // Initial value, grows as needed.
ساكن stl_توکی_T      *stl_توکي = عدم;
ساكن صحيح	       *stl_groupitem = عدم;
ساكن stl_hlrec_T     *stl_hltab = عدم;
ساكن stl_hlrec_T     *stl_tabtab = عدم;
ساكن stl_clickrec_T  *stl_clicktab = عدم;
ساكن صحيح		*stl_separator_locations = عدم;

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
    صحيح
build_stl_str_hl(
    win_T	*wp,
    char_u	*بهر,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحيح		opt_scope,	// scope for "opt_name"
    صحيح		fillchar,
    صحيح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    أرجع build_stl_str_hl_local(STL_ډېرپېښ_SINGLE, wp, بهر, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    عدم, عدم);
}

    صحيح
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*بهر,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحيح		opt_scope,	// scope for "opt_name"
    صحيح		fillchar,
    صحيح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    صحيح		*لېږد_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    أرجع build_stl_str_hl_local(STL_ډېرپېښ_MULTI, wp, بهر, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    عدم, لېږد_hl);
}

# ifdef ENABLE_STL_ډېرپېښ_MULTI_NL
    صحيح
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*بهر,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحيح		opt_scope,	// scope for "opt_name"
    صحيح		fillchar,
    صحيح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    صحيح		*لېږد_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    أرجع build_stl_str_hl_local(STL_ډېرپېښ_MULTI_NL, wp, بهر, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    عدم, لېږد_hl);
}
# endif

    صحيح
get_stl_rendered_لوړوالی(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحيح		opt_scope)	// scope for "opt_name"
{
    صحيح rendered_لوړوالی = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (فراغ)build_stl_str_hl_local(STL_ډېرپېښ_GET_RENDERED_لوړوالی,
	    wp, buf, حجم(buf), &fmt,
	    opt_name, opt_scope, 0, 0, عدم, عدم, عدم, &rendered_لوړوالی,
	    عدم);
    --emsg_off;
    أرجع rendered_لوړوالی;
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
ساكن صحيح
build_stl_str_hl_local(
    stl_ډېرپېښ_T	ډېرپېښ,
    win_T	*wp,
    char_u	*بهر,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحيح		opt_scope,	// scope for "opt_name"
    صحيح		fillchar,
    صحيح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    صحيح		*rendered_لوړوالی,   // return: stl rendered height (can be NULL)
    صحيح		*لېږد_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    صحيح		byteval;
# ifdef FEAT_EVAL
    صحيح		use_sandbox;
    صحيح		save_VIsual_فعال;
# endif
    صحيح		تشه_کرښه;
    اوږد	l;
    اوږد	n;
    صحيح		prevchar_isflag;
    صحيح		prevchar_isitem;
    صحيح		itemisflag;
    صحيح		fillable;
    char_u	*str;
    اوږد	عدد;
    صحيح		پلنوالی;
    صحيح		itemcnt;
    صحيح		curitem;
    صحيح		groupdepth;
# ifdef FEAT_EVAL
    صحيح		evaldepth;
# endif
    صحيح		minwid;
    صحيح		maxwid;
    صحيح		zeropad;
    char_u	بنسټ;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    صحيح		save_redraw_not_allowed = redraw_not_allowed;
    صحيح		save_کلی_Typed = کلی_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    صحيح		did_emsg_before = did_emsg;
    صحيح		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    إذا (updating_screen)
	redraw_not_allowed = TRUE;

    إذا (stl_توکي == عدم)
    {
	stl_توکي = ALLOC_MULT(stl_توکی_T, stl_توکي_len);
	stl_groupitem = ALLOC_MULT(صحيح, stl_توکي_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_توکي_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_توکي_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_توکي_len + 1);

	stl_separator_locations = ALLOC_MULT(صحيح, stl_توکي_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_سېټ_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    إذا (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_شمېره;
	tv.vval.v_شمېره = wp->w_id;
	سېټ_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_منزل_string_خوندي(fmt + 2, use_sandbox, FALSE, FALSE);
	إذا (usefmt == عدم)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    إذا (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    إذا (lnum > wp->w_بفر->b_ml.ml_کرښه_شمېر)
    {
	lnum = wp->w_بفر->b_ml.ml_کرښه_شمېر;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_بفر, lnum, FALSE);
    تشه_کرښه = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_بفر, lnum);
    إذا (wp->w_cursor.ستون > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.ستون = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    وإلا
	byteval = (*mb_ptr2char)(p + wp->w_cursor.ستون);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = بهر;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    إذا (لېږد_hl != عدم && *لېږد_hl != 0)
    {
	stl_توکي[curitem].stl_type = Highlight;
	stl_توکي[curitem].stl_پیل = p;
	stl_توکي[curitem].stl_minwid = *لېږد_hl;
	stl_توکي[curitem].stl_clickfunc = عدم;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    لكل (s = usefmt; *s != NUL; )
    {
	إذا (curitem == (صحيح)stl_توکي_len)
	{
	    size_t	new_len = stl_توکي_len * 3 / 2;

	    stl_توکی_T *new_توکي =
			  vim_realloc(stl_توکي, حجم(stl_توکی_T) * new_len);
	    إذا (new_توکي == عدم)
		اكسر;
	    stl_توکي = new_توکي;

	    صحيح *new_groupitem =
			     vim_realloc(stl_groupitem, حجم(صحيح) * new_len);
	    إذا (new_groupitem == عدم)
		اكسر;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  حجم(stl_hlrec_T) * (new_len + 1));
	    إذا (new_hlrec == عدم)
		اكسر;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  حجم(stl_hlrec_T) * (new_len + 1));
	    إذا (new_hlrec == عدم)
		اكسر;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      حجم(stl_clickrec_T) * (new_len + 1));
	    إذا (new_clickrec == عدم)
		اكسر;
	    stl_clicktab = new_clickrec;

	    صحيح *new_separator_locs = vim_realloc(stl_separator_locations,
					    حجم(صحيح) * new_len);
	    إذا (new_separator_locs == عدم)
		اكسر;
	    stl_separator_locations = new_separator_locs;

	    stl_توکي_len = new_len;
	}

	stl_توکي[curitem].stl_clickfunc = عدم;

	إذا (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	طالما (*s != NUL && *s != '%' && p + 1 < بهر + outlen)
# ifdef ENABLE_STL_ډېرپېښ_MULTI_NL
	{
	    إذا (*s == '\n' || *s == '\r')
	    {
		إذا (ډېرپېښ == STL_ډېرپېښ_MULTI_NL)
		{
		    s++;
		    اذهب پیدا_کړه_linebreak;
		}
		وإلا إذا (ډېرپېښ == STL_ډېرپېښ_GET_RENDERED_لوړوالی)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_ډېرپېښ_MULTI_NL
	}
# endif
	إذا (*s == NUL || p + 1 >= بهر + outlen)
	    اكسر;

	/*
	 * Handle one '%' item.
	 */
	s++;
	إذا (*s == NUL)  // ignore trailing %
	    اكسر;

	إذا (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    إذا (s[1] == ']')
	    {
		stl_توکي[curitem].stl_type = ClickFunc;
		stl_توکي[curitem].stl_پیل = p;
		stl_توکي[curitem].stl_minwid = 0;
		stl_توکي[curitem].stl_clickfunc = عدم;
		s += 2;
		curitem++;
		استمر;
	    }
	    // %[FuncName] - start click region
	    إذا (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		إذا (rb != عدم)
		{
		    stl_توکي[curitem].stl_type = ClickFunc;
		    stl_توکي[curitem].stl_پیل = p;
		    stl_توکي[curitem].stl_minwid = 0;
		    stl_توکي[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    استمر;
		}
	    }
	}
	إذا (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    إذا (ډېرپېښ == STL_ډېرپېښ_MULTI
# ifdef ENABLE_STL_ډېرپېښ_MULTI_NL
		    || ډېرپېښ == STL_ډېرپېښ_MULTI_NL
# endif
	       )
	    {
		s++;
		اكسر;
	    }
	    وإلا إذا (ډېرپېښ == STL_ډېرپېښ_GET_RENDERED_لوړوالی)
		rheight++;
	}
	إذا (*s == '%')
	{
	    إذا (p + 1 >= بهر + outlen)
		اكسر;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    استمر;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	إذا (*s == STL_SEPARATE)
	{
	    s++;
	    إذا (groupdepth > 0)
		استمر;
	    stl_توکي[curitem].stl_type = Separate;
	    stl_توکي[curitem++].stl_پیل = p;
	    استمر;
	}
	إذا (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_توکي[curitem].stl_type = Trunc;
	    stl_توکي[curitem++].stl_پیل = p;
	    استمر;
	}
	إذا (*s == ')')
	{
	    char_u  *t;

	    s++;
	    إذا (groupdepth < 1)
		استمر;
	    groupdepth--;

	    t = stl_توکي[stl_groupitem[groupdepth]].stl_پیل;
	    *p = NUL;
	    l = vim_strsize(t);
	    إذا (curitem > stl_groupitem[groupdepth] + 1
		    && stl_توکي[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		صحيح ډله_پیل_userhl = 0;
		صحيح ډله_پای_ټکی_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		لكل (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    إذا (stl_توکي[n].stl_type == Highlight)
		    {
			ډله_پیل_userhl = ډله_پای_ټکی_userhl =
						       stl_توکي[n].stl_minwid;
			اكسر;
		    }
		}
		لكل (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    إذا (stl_توکي[n].stl_type == نورمال)
			اكسر;
		    إذا (stl_توکي[n].stl_type == Highlight)
			ډله_پای_ټکی_userhl = stl_توکي[n].stl_minwid;
		}
		إذا (n == curitem && ډله_پیل_userhl == ډله_پای_ټکی_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    لكل (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			إذا (stl_توکي[n].stl_type == Highlight)
			    stl_توکي[n].stl_type = تشه;
			// adjust the start position of TabPage to the next
			// item position
			إذا (stl_توکي[n].stl_type == Tab_پاڼه)
			    stl_توکي[n].stl_پیل = p;
		    }
		}
	    }
	    إذا (l > stl_توکي[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		إذا (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    طالما (l >= stl_توکي[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		وإلا
		    n = (اوږد)(p - t) - stl_توکي[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		صحيح minwid_fixed = MIN(stl_توکي[stl_groupitem[groupdepth]].stl_minwid,
				       stl_توکي[stl_groupitem[groupdepth]].stl_maxwid);
		طالما (++l < minwid_fixed)
		    MB_CHAR2_بایټونه(fillchar, p);

		// correct the start of the items for the truncation
		لكل (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_توکي[l].stl_پیل -= n - 1;
		    إذا (stl_توکي[l].stl_پیل < t)
			stl_توکي[l].stl_پیل = t;
		}
	    }
	    وإلا إذا (مطلق_قیمت(stl_توکي[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_توکي[stl_groupitem[groupdepth]].stl_minwid;
		صحيح fillchar_len = MB_CHAR2LEN(fillchar);
		إذا (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    طالما (l++ < n && p + fillchar_len < بهر + outlen)
			MB_CHAR2_بایټونه(fillchar, p);
		}
		وإلا
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    إذا (p + l >= بهر + outlen)
		    {
			n = (اوږد)((بهر + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    لكل ( ; n > 0; n--)
			MB_CHAR2_بایټونه(fillchar, t);
		    لكل (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_توکي[n].stl_پیل += l;
		}
	    }
	    استمر;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	إذا (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	إذا (*s == '-')
	{
	    s++;
	    l = -1;
	}
	إذا (VIM_ISDIGIT(*s))
	{
	    minwid = (صحيح)getdigits(&s);
	    إذا (minwid < 0)	//ډکېدنه
		minwid = 0;
	}
	إذا (*s == STL_USER_HL)
	{
	    stl_توکي[curitem].stl_type = Highlight;
	    stl_توکي[curitem].stl_پیل = p;
	    stl_توکي[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    استمر;
	}
	إذا (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    إذا (*s == STL_TABCLOSENR)
	    {
		إذا (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    لكل (n = curitem - 1; n >= 0; --n)
			إذا (stl_توکي[n].stl_type == Tab_پاڼه
					       && stl_توکي[n].stl_minwid >= 0)
			{
			    minwid = stl_توکي[n].stl_minwid;
			    اكسر;
			}
		}
		وإلا
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_توکي[curitem].stl_type = Tab_پاڼه;
	    stl_توکي[curitem].stl_پیل = p;
	    stl_توکي[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    استمر;
	}
	إذا (*s == '.')
	{
	    s++;
	    إذا (VIM_ISDIGIT(*s))
	    {
		maxwid = (صحيح)getdigits(&s);
		إذا (maxwid <= 0)	//ډکېدنه
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	صحيح raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	إذا (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_توکي[curitem].stl_type = ډله_2;
	    stl_توکي[curitem].stl_پیل = p;
	    stl_توکي[curitem].stl_minwid = minwid;
	    stl_توکي[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    استمر;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	إذا (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    استمر;
	}
# endif
	إذا (vim_strchr(STL_ALL, *s) == عدم)
	{
	    إذا (*s == NUL)  // can happen with "%0"
		اكسر;
	    s++;
	    استمر;
	}
	opt = *s++;

	// OK - now for the real work
	بنسټ = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	عدد = -1;
	str = عدم;
	بدل (opt)
	{
	حالة STL_FILEPATH:
	حالة STL_FULLPATH:
	حالة STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_بفر);
	    إذا (name != عدم)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    وإلا
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_بفر->b_ffname
					  : wp->w_بفر->b_fname;
		home_replace(wp->w_بفر, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    إذا (opt != STL_FILENAME)
		str = NameBuff;
	    وإلا
		str = gettail(NameBuff);
	    اكسر;
	}

	حالة STL_VIM_بیان: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *بلاک_پیل = s - 1;
# endif
	    صحيح	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    إذا (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    طالما ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < بهر + outlen)
		*p++ = *s++;
	    إذا (*s != '}')	// missing '}' or out of space
		اكسر;
	    s++;
	    إذا (reevaluate && p > بهر)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    وإلا
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((حرف *)buf_tmp, حجم(buf_tmp),
							 "%d", curbuf->b_fnum);
	    سېټ_دننی_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((حرف *)buf_tmp, حجم(buf_tmp), "%d", curwin->w_id);
	    سېټ_دننی_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_فعال = VIsual_فعال;
	    curwin = wp;
	    curbuf = wp->w_بفر;
	    // Visual mode is only valid in the current window.
	    إذا (curwin != save_curwin)
		VIsual_فعال = FALSE;

	    str = eval_منزل_string_خوندي(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_فعال = save_VIsual_فعال;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    إذا (!zeropad && str != عدم && *str != NUL)
	    {
		إذا (*skipdigits(str) == NUL)
		{
		    عدد = atoi((حرف *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    إذا (reevaluate && str != عدم && *str != NUL
		    && strchr((ثابت حرف *)str, '%') != عدم
		    && evaldepth < MAX_STL_EVAL_ژوروالی)
	    {
		size_t parsed_usefmt = (size_t)(بلاک_پیل - usefmt);
		size_t str_اوږدتیا = مزي_اوږدوالی((ثابت حرف *)str);
		size_t fmt_اوږدتیا = مزي_اوږدوالی((ثابت حرف *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_اوږدتیا + fmt_اوږدتیا + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * حجم(char_u));

		إذا (new_fmt != عدم)
		{
		    char_u *new_fmt_p = new_fmt;

		    new_fmt_p = (char_u *)حافظه_نقل(new_fmt_p, usefmt, parsed_usefmt)
								   + parsed_usefmt;
		    new_fmt_p = (char_u *)حافظه_نقل(new_fmt_p , str, str_اوږدتیا)
								      + str_اوږدتیا;
		    new_fmt_p = (char_u *)حافظه_نقل(new_fmt_p, "%}", 2) + 2;
		    new_fmt_p = (char_u *)حافظه_نقل(new_fmt_p , s, fmt_اوږدتیا)
								      + fmt_اوږدتیا;
		    *new_fmt_p = 0;
		    new_fmt_p = عدم;

		    إذا (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    استمر;
		}
	    }
# endif
	    اكسر;
	}
	حالة STL_کرښه:
	    عدد = (wp->w_بفر->b_ml.ml_flags & ML_تشه)
		  ? 0L : (اوږد)(wp->w_cursor.lnum);
	    اكسر;

	حالة STL_NUMLINES:
	    عدد = wp->w_بفر->b_ml.ml_کرښه_شمېر;
	    اكسر;

	حالة STL_ستنه:
	    عدد = (State & ډېرپېښ_ورننباسه) == 0 && تشه_کرښه
					       ? 0 : (صحيح)wp->w_cursor.ستون + 1;
	    اكسر;

	حالة STL_VIRTCOL:
	حالة STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    إذا (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & ډېرپېښ_ورننباسه) == 0
			       && تشه_کرښه ? 0 : (صحيح)wp->w_cursor.ستون + 1)))
		اكسر;
	    عدد = (اوږد)virtcol;
	    اكسر;
	}

	حالة STL_PERCENTAGE:
	    عدد = calc_percentage((اوږد)wp->w_cursor.lnum, (اوږد)wp->w_بفر->b_ml.ml_کرښه_شمېر);
	    اكسر;

	حالة STL_ALTPERCENT:
	    str = buf_tmp;
	    (فراغ)get_rel_pos(wp, str, TMPLEN);
	    اكسر;

	حالة STL_SHOWCMD:
	    إذا (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    اكسر;

	حالة STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    إذا (append_arg_شمېره(wp, buf_tmp, حجم(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    اكسر;

	حالة STL_KEYMAP:
	    fillable = FALSE;
	    إذا (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    اكسر;
	حالة STL_PAGENUM:
# إذا defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    عدد = printer_پاڼه_عدد;
# وإلا
	    عدد = 0;
# endif
	    اكسر;

	حالة STL_BUFNO:
	    عدد = wp->w_بفر->b_fnum;
	    اكسر;

	حالة STL_دننه_واټن_X:
	    بنسټ = 'X';
	    // FALLTHROUGH
	حالة STL_دننه_واټن:
# ifdef FEAT_BYTEOFF
	    l = ml_پیدا_کړه_کرښه_or_دننه_واټن(wp->w_بفر, wp->w_cursor.lnum, عدم);
	    عدد = (wp->w_بفر->b_ml.ml_flags & ML_تشه) || l < 0
		       ? 0L : l + 1 + ((State & ډېرپېښ_ورننباسه) == 0 && تشه_کرښه
				? 0 : (صحيح)wp->w_cursor.ستون);
# endif
	    اكسر;

	حالة STL_BYTEVAL_X:
	    بنسټ = 'X';
	    // FALLTHROUGH
	حالة STL_BYTEVAL:
	    عدد = byteval;
	    إذا (عدد == NL)
		عدد = 0;
	    وإلا إذا (عدد == CAR && get_fileformat(wp->w_بفر) == EOL_MAC)
		عدد = NL;
	    اكسر;

	حالة STL_ROFLAG:
	حالة STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    إذا (wp->w_بفر->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    اكسر;

	حالة STL_HELPFLAG:
	حالة STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    إذا (wp->w_بفر->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    اكسر;

	حالة STL_FILETYPE:
	    إذا (*wp->w_بفر->b_p_ft != NUL
		    && STRLEN(wp->w_بفر->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((حرف *)buf_tmp, حجم(buf_tmp), "[%s]",
							wp->w_بفر->b_p_ft);
		str = buf_tmp;
	    }
	    اكسر;

	حالة STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    إذا (*wp->w_بفر->b_p_ft != NUL
		    && STRLEN(wp->w_بفر->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((حرف *)buf_tmp, حجم(buf_tmp), ",%s",
							wp->w_بفر->b_p_ft);
		لكل (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    اكسر;

# إذا defined(FEAT_QUICKFIX)
	حالة STL_PREVIEWFLAG:
	حالة STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    إذا (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    اكسر;

	حالة STL_QUICKFIX:
	    إذا (bt_quickfix(wp->w_بفر))
		str = (char_u *)(wp->w_llist_ref
			    ? _(پیغام_loclist)
			    : _(پیغام_qflist));
	    اكسر;
# endif

	حالة STL_MODIFIED:
	حالة STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    بدل ((opt == STL_MODIFIED_ALT)
		    + buf_Is_بدل_شوی(wp->w_بفر) * 2
		    + (!wp->w_بفر->b_p_ma) * 4)
	    {
		حالة 2: str = (char_u *)"[+]"; اكسر;
		حالة 3: str = (char_u *)",+"; اكسر;
		حالة 4: str = (char_u *)"[-]"; اكسر;
		حالة 5: str = (char_u *)",-"; اكسر;
		حالة 6: str = (char_u *)"[+-]"; اكسر;
		حالة 7: str = (char_u *)",+-"; اكسر;
	    }
	    اكسر;

	حالة STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		طالما (*s != '#' && *s != NUL)
		    ++s;
		إذا (*s == '#')
		{
		    stl_توکي[curitem].stl_type = Highlight;
		    stl_توکي[curitem].stl_پیل = p;
		    stl_توکي[curitem].stl_minwid = -syn_namen2id(t, (صحيح)(s - t));
		    curitem++;
		}
		إذا (*s != NUL)
		    ++s;
		استمر;
	    }

	حالة STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    إذا (*s == ']')
	    {
		stl_توکي[curitem].stl_type = ClickFunc;
		stl_توکي[curitem].stl_پیل = p;
		stl_توکي[curitem].stl_minwid = 0;
		stl_توکي[curitem].stl_clickfunc = عدم;
		s++;
		curitem++;
		استمر;
	    }
	    // %N[FuncName] with minwid
	    إذا (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		إذا (rb != عدم)
		{
		    stl_توکي[curitem].stl_type = ClickFunc;
		    stl_توکي[curitem].stl_پیل = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_توکي[curitem].stl_minwid = raw_minwid;
		    stl_توکي[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    استمر;
		}
	    }
	    استمر;

	حالة STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    استمر;
	}

	stl_توکي[curitem].stl_پیل = p;
	stl_توکي[curitem].stl_type = نورمال;
	إذا (str != عدم && *str)
	{
	    char_u  *t = str;

	    إذا (itemisflag)
	    {
		إذا ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    إذا (l > 0)
		prevchar_isitem = TRUE;
	    إذا (l > maxwid)
	    {
		طالما (l >= maxwid)
		    إذا (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    وإلا
			l -= byte2cells(*t++);
		إذا (p + 1 >= بهر + outlen)
		    اكسر;
		*p++ = '<';
	    }
	    إذا (minwid > 0)
	    {
		لكل (; l < minwid && p + 1 < بهر + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    إذا (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    وإلا
			MB_CHAR2_بایټونه(fillchar, p);
		}
		minwid = 0;
	    }
	    وإلا
		minwid *= -1;
	    لكل (; *t && p + 1 < بهر + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		إذا (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2_بایټونه(fillchar, p);
		وإلا
		    *p++ = *t;
	    }
	    لكل (; l < minwid && p + 1 < بهر + outlen; l++)
		MB_CHAR2_بایټونه(fillchar, p);
	}
	وإلا إذا (عدد >= 0)
	{
	    صحيح	    nbase = (بنسټ == 'D' ? 10 : (بنسټ == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    إذا (p + 20 >= بهر + outlen)
		اكسر;		// not sufficient space
	    prevchar_isitem = TRUE;
	    إذا (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    إذا (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? بنسټ : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    لكل (n = عدد, l = 1; n >= nbase; n /= nbase)
		l++;
	    إذا (opt == STL_VIRTCOL_ALT)
		l++;

	    إذا (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		طالما (l-- > maxwid)
		    عدد /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((حرف *)p, outlen - (p - بهر),
		    (حرف *)nstr, 0, عدد, n);
	    }
	    وإلا
		p += vim_snprintf_safelen((حرف *)p, outlen - (p - بهر),
		    (حرف *)nstr, minwid, عدد);
	}
	وإلا
	    stl_توکي[curitem].stl_type = تشه;

	إذا (عدد >= 0 || (!itemisflag && str != عدم && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	إذا (opt == STL_VIM_بیان)
	    vim_free(str);
	curitem++;
    }
# ifdef ENABLE_STL_ډېرپېښ_MULTI_NL
پیدا_کړه_linebreak:
# endif
    *p = NUL;
    outputlen = (size_t)(p - بهر);
    itemcnt = curitem;

    // Remember the most recent %# / %* highlight so the next row of a
    // multi-line statusline can resume it.
    إذا (لېږد_hl != عدم)
    {
	صحيح وروستی_hl = 0;
	لكل (l = 0; l < itemcnt; l++)
	    إذا (stl_توکي[l].stl_type == Highlight)
		وروستی_hl = stl_توکي[l].stl_minwid;
	*لېږد_hl = وروستی_hl;
    }

    إذا (ډېرپېښ == STL_ډېرپېښ_MULTI
# ifdef ENABLE_STL_ډېرپېښ_MULTI_NL
		    || ډېرپېښ == STL_ډېرپېښ_MULTI_NL
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
	إذا (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = مزي_اوږدوالی((حرف *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    وإلا
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	إذا (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    إذا (ډېرپېښ == STL_ډېرپېښ_GET_RENDERED_لوړوالی)
    {
	// Free click function names that were allocated during parsing.
	لكل (l = 0; l < itemcnt; l++)
	    إذا (stl_توکي[l].stl_type == ClickFunc)
		vim_free(stl_توکي[l].stl_clickfunc);
	إذا (rendered_لوړوالی != عدم)
	    *rendered_لوړوالی = rheight;
	أرجع 0;
    }

    پلنوالی = vim_strsize(بهر);
    إذا (maxwidth > 0 && پلنوالی > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	إذا (itemcnt == 0)
	    s = بهر;
	وإلا
	{
	    لكل ( ; l < itemcnt; l++)
		إذا (stl_توکي[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_توکي[l].stl_پیل;
		    اكسر;
		}
	    إذا (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_توکي[0].stl_پیل;
		l = 0;
	    }
	}

	إذا (پلنوالی - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    إذا (has_mbyte)
	    {
		s = بهر;
		پلنوالی = 0;
		لكل (;;)
		{
		    پلنوالی += ptr2cells(s);
		    إذا (پلنوالی >= maxwidth)
			اكسر;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		طالما (++پلنوالی < maxwidth)
		    MB_CHAR2_بایټونه(fillchar, s);
	    }
	    وإلا
		s = بهر + maxwidth - 1;
	    لكل (l = 0; l < itemcnt; l++)
		إذا (stl_توکي[l].stl_پیل > s)
		    اكسر;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	وإلا
	{
	    char_u  *پای_ټکی = بهر + outputlen;

	    n = 0;
	    طالما (پلنوالی >= maxwidth)
	    {
		پلنوالی -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(پای_ټکی - p) + 1);	// +1 for NUL
	    پای_ټکی -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    لكل (; l < itemcnt; l++)
	    {
		إذا (stl_توکي[l].stl_پیل - n >= s)
		    stl_توکي[l].stl_پیل -= n;
		وإلا
		    stl_توکي[l].stl_پیل = s;
	    }

	    // Fill up for half a double-wide character.
	    طالما (++پلنوالی < maxwidth)
	    {
		s = پای_ټکی;
		MB_CHAR2_بایټونه(fillchar, s);
		*s = NUL;
		پای_ټکی = s;
	    }
	}
	پلنوالی = maxwidth;
    }
    وإلا إذا (پلنوالی < maxwidth &&
	    outputlen + (maxwidth - پلنوالی) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	صحيح عدد_separators = 0;

	لكل (l = 0; l < itemcnt; l++)
	{
	    إذا (stl_توکي[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[عدد_separators] = l;
		عدد_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	إذا (عدد_separators)
	{
	    صحيح standard_spaces;
	    صحيح final_spaces;

	    standard_spaces = (maxwidth - پلنوالی) / عدد_separators;
	    final_spaces = (maxwidth - پلنوالی) -
					standard_spaces * (عدد_separators - 1);
	    لكل (l = 0; l < عدد_separators; l++)
	    {
		صحيح dislocation = (l == (عدد_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *پیل = stl_توکي[stl_separator_locations[l]].stl_پیل;
		char_u *seploc = پیل + dislocation;
		STRMOVE(seploc, پیل);
		لكل (s = پیل; s < seploc;)
		    MB_CHAR2_بایټونه(fillchar, s);

		لكل (صحيح i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_توکي[i].stl_پیل += dislocation;
	    }

	    پلنوالی = maxwidth;
	}
    }

    // Store the info about highlighting.
    إذا (hltab != عدم)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	لكل (l = 0; l < itemcnt; l++)
	{
	    إذا (stl_توکي[l].stl_type == Highlight)
	    {
		sp->پیل = stl_توکي[l].stl_پیل;
		sp->userhl = stl_توکي[l].stl_minwid;
		sp++;
	    }
	}
	sp->پیل = عدم;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    إذا (tabtab != عدم)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	لكل (l = 0; l < itemcnt; l++)
	{
	    إذا (stl_توکي[l].stl_type == Tab_پاڼه)
	    {
		sp->پیل = stl_توکي[l].stl_پیل;
		sp->userhl = stl_توکي[l].stl_minwid;
		sp++;
	    }
	}
	sp->پیل = عدم;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    إذا (clicktab != عدم)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	لكل (l = 0; l < itemcnt; l++)
	{
	    إذا (stl_توکي[l].stl_type == ClickFunc)
	    {
		cp->پیل = stl_توکي[l].stl_پیل;
		cp->funcname = stl_توکي[l].stl_clickfunc;
		cp->minwid = stl_توکي[l].stl_minwid;
		cp++;
	    }
	}
	cp->پیل = عدم;
	cp->funcname = عدم;
    }
    وإلا
    {
	// Free click function names when caller doesn't need them.
	لكل (l = 0; l < itemcnt; l++)
	    إذا (stl_توکي[l].stl_type == ClickFunc)
		vim_free(stl_توکي[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    کلی_Typed = save_کلی_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    إذا (did_emsg > did_emsg_before)
	سېټ_string_option_مستقیم(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    أرجع پلنوالی;
}
#د_که_پای // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    صحيح
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    صحيح		buflen)
{
    اوږد	above; // number of lines above window
    اوږد	لاندې; // number of lines below window

    إذا (buflen < 3) // need at least 3 chars for writing
	أرجع 0;
    above = wp->w_topline - 1;
#که_تعریف_شوی FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    إذا (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#د_که_پای
    لاندې = wp->w_بفر->b_ml.ml_کرښه_شمېر - wp->w_botline + 1;
    إذا (لاندې <= 0)
	أرجع (صحيح)vim_snprintf_safelen((حرف *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    إذا (above <= 0)
	أرجع (صحيح)vim_snprintf_safelen((حرف *)buf, buflen,
	    "%s", _("Top"));

    صحيح perc = calc_percentage(above, above + لاندې);
    حرف tmp[8];
    // localized percentage value
    vim_snprintf(tmp, حجم(tmp), _("%d%%"), perc);
    أرجع (صحيح)vim_snprintf_safelen((حرف *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    ساكن صحيح
append_arg_شمېره(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    صحيح		جمع_کړه_file)	// Add "file" before the arg number
{
    إذا (ARGCOUNT <= 1)		// nothing to do
	أرجع 0;

    حرف *پیغام;
    بدل ((wp->w_arg_idx_invalid ? 1 : 0) + (جمع_کړه_file ? 2 : 0))
    {
	حالة 0: پیغام = _(" (%d of %d)"); اكسر;
	حالة 1: پیغام = _(" ((%d) of %d)"); اكسر;
	حالة 2: پیغام = _(" (file %d of %d)"); اكسر;
	حالة 3: پیغام = _(" (file (%d) of %d)"); اكسر;
    }

    أرجع (صحيح)vim_snprintf_safelen((حرف *)buf, buflen, پیغام,
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
#که_تعریف_شوی UNIX
    أرجع ډک_Name_save(fname, TRUE);
#وإلا
    إذا (!vim_isAbsName(fname)
	    || مزي_پلټنه((حرف *)fname, "..") != عدم
	    || مزي_پلټنه((حرف *)fname, "//") != عدم
# ifdef BACKSLASH_دننه_FILENAME
	    || مزي_پلټنه((حرف *)fname, "\\\\") != عدم
# endif
# إذا defined(MSWIN)
	    || vim_strchr(fname, '~') != عدم
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	أرجع ډک_Name_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    إذا (fname != عدم)
	fname_case(fname, 0);	// set correct case for file name
# endif

    أرجع fname;
#د_که_پای
}

/*
 * Make "*ffname" a full file name, set "*sfname" to "*ffname" if not NULL.
 * "*ffname" becomes a pointer to allocated memory (or NULL).
 * When resolving a link both "*sfname" and "*ffname" will point to the same
 * allocated memory.
 * The "*ffname" and "*sfname" pointer values on call will not be freed.
 * Note that the resulting "*ffname" pointer should be considered not allocated.
 */
    فراغ
fname_expand(
    buf_T	*buf ناکارول_شوی,
    char_u	**ffname,
    char_u	**sfname)
{
    إذا (*ffname == عدم)	    // no file name given, nothing to do
	أرجع;
    إذا (*sfname == عدم)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#که_تعریف_شوی FEAT_SHORTCUT
    إذا (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_لار(*ffname, FALSE);
	إذا (rfname != عدم)
	{
	    vim_free(*ffname);
	    *ffname = rfname;
	    *sfname = rfname;
	}
    }
#د_که_پای
}

/*
 * Open a window for a number of buffers.
 */
    فراغ
ex_بفر_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    صحيح		وېشل_ret = OK;
    صحيح		p_ea_save;
    صحيح		open_wins = 0;
    صحيح		r;
    صحيح		شمېر;		// Maximum number of windows to open.
    صحيح		all;		// When TRUE also load inactive buffers.
    صحيح		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    إذا (eap->addr_شمېر == 0)	// make as many windows as possible
	شمېر = 9999;
    وإلا
	شمېر = eap->line2;	// make as many windows as specified
    إذا (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    وإلا
	all = TRUE;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    setpcmark();

#که_تعریف_شوی FEAT_GUI
    need_mouse_correct = TRUE;
#د_که_پای

    /*
     * Close superfluous windows (two windows for the same buffer).
     * Also close windows that are not full-width.
     */
    إذا (had_tab > 0)
	goto_tabpage_tp(لومړی_tabpage, TRUE, TRUE);
    لكل (;;)
    {
	tpnext = curtab->tp_بل;
	لكل (wp = firstwin; wp != عدم; wp = wpnext)
	{
	    wpnext = wp->w_بل;
	    إذا ((wp->w_بفر->b_nwindows > 1
			|| ((cmdmod.cmod_وېشل & WSP_VERT)
			    ? wp->w_لوړوالی + wp->w_حالت_لوړوالی < لیکې - p_ch
							     - tabline_لوړوالی()
			    : wp->w_پلنوالی != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_کړکۍ
		    && !(win_locked(wp) || wp->w_بفر->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		إذا (win_close(wp, FALSE) == FAIL)
		    اكسر;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = لومړی_tabpage;
		open_wins = 0;
	    }
	    وإلا
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	إذا (had_tab == 0 || tpnext == عدم)
	    اكسر;
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
    لكل (buf = firstbuf; buf != عدم && open_wins < شمېر; buf = buf->b_بل)
    {
	// Check if this buffer needs a window
	إذا ((!all && buf->b_ml.ml_mfp == عدم) || !buf->b_p_bl)
	    استمر;

	إذا (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    إذا (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    وإلا
		wp = عدم;
	}
	وإلا
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		إذا (wp->w_بفر == buf)
		    اكسر;
	    // If the buffer already has a window, move it
	    إذا (wp != عدم)
		win_move_after(wp, curwin);
	}

	إذا (wp == عدم && وېشل_ret == OK)
	{
	    bufref_T	bufref;

	    سېټ_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    وېشل_ret = win_وېشل(0, WSP_ځای | WSP_لاندې);
	    ++open_wins;
	    p_ea = p_ea_save;
	    إذا (وېشل_ret == FAIL)
		استمر;

	    // Open the buffer in this window.
	    بدلون_exists_action = SEA_DIALOG;
	    سېټ_curbuf(buf, DOBUF_GOTO);
	    إذا (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		بدلون_exists_action = SEA_NONE;
		اكسر;
	    }
	    إذا (بدلون_exists_action == SEA_QUIT)
	    {
#إذا defined(FEAT_EVAL)
		cleanup_T   cs;

		// Reset the error/interrupt/exception state here so that
		// aborting() returns FALSE when closing a window.
		enter_cleanup(&cs);
#د_که_پای

		// User selected Quit at ATTENTION prompt; close this window.
		win_close(curwin, TRUE);
		--open_wins;
		بدلون_exists_action = SEA_NONE;
		بدلون_exists_did_quit = TRUE;

#إذا defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#د_که_پای
	    }
	    وإلا
		handle_بدلون_exists(عدم);
	}

	ui_breakcheck();
	إذا (got_int)
	{
	    (فراغ)vgetc();	// only break the file loading, not the rest
	    اكسر;
	}
#که_تعریف_شوی FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	إذا (aborting())
	    اكسر;
#د_که_پای
	// When ":tab" was used open a new tab for a new window repeatedly.
	إذا (had_tab > 0 && tabpage_شاخص(عدم) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    لكل (wp = lastwin; open_wins > شمېر; )
    {
	r = (buf_hide(wp->w_بفر) || !buf_Is_بدل_شوی(wp->w_بفر)
				     || autowrite(wp->w_بفر, FALSE) == OK);
	إذا (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	وإلا إذا (r)
	{
	    win_close(wp, !buf_hide(wp->w_بفر));
	    --open_wins;
	    wp = lastwin;
	}
	وإلا
	{
	    wp = wp->w_prev;
	    إذا (wp == عدم)
		اكسر;
	}
    }
}


ساكن صحيح  chk_modeline(linenr_T, صحيح);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    فراغ
do_modelines(صحيح flags)
{
    linenr_T	lnum;
    صحيح		nmlines;
    ساكن صحيح	entered = 0;

    إذا (!curbuf->b_p_ml || (nmlines = (صحيح)p_mls) == 0)
	أرجع;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    إذا (entered)
	أرجع;

    ++entered;
    لكل (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_کرښه_شمېر && lnum <= nmlines;
								       ++lnum)
	إذا (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    لكل (lnum = curbuf->b_ml.ml_کرښه_شمېر; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_کرښه_شمېر - nmlines; --lnum)
	إذا (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#ضمن "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    ساكن صحيح
chk_modeline(
    linenr_T	lnum,
    صحيح		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*کرښه_پای_ټکی;		// point to the end of the line
    char_u	*e;
    صحيح		prev;
    صحيح		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    کرښه_پای_ټکی = s + ml_get_len(lnum);
    لكل (; *s != NUL; ++s)
    {
	إذا (prev == -1 || vim_isspace(prev))
	{
	    إذا ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		اكسر;
	    // Accept both "vim" and "Vim".
	    إذا ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		صحيح vers;

		إذا (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		وإلا
		    e = s + 3;
		vers = getdigits(&e);
		إذا (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_نسخه_100 >= vers && خوندي_isdigit(s[3]))
			    || (VIM_نسخه_100 < vers && s[3] == '<')
			    || (VIM_نسخه_100 > vers && s[3] == '>')
			    || (VIM_نسخه_100 == vers && s[3] == '=')))
		    اكسر;
	    }
	}
	prev = *s;
    }

    إذا (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	صحيح	پای_ټکی;

	افعل				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	طالما (s[-1] != ':');

	len = (size_t)(کرښه_پای_ټکی - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	إذا (linecopy == عدم)
	    أرجع FAIL;

	کرښه_پای_ټکی = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_ټېل(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_تیاری;

	پای_ټکی = FALSE;
	طالما (پای_ټکی == FALSE)
	{
	    s = skipwhite(s);
	    إذا (*s == NUL)
		اكسر;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    لكل (e = s; *e != ':' && *e != NUL; ++e)
		إذا (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(کرښه_پای_ټکی - (e + 1)) + 1);	// +1 for NUL
		    --کرښه_پای_ټکی;
		}
	    إذا (*e == NUL)
		پای_ټکی = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    إذا (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		إذا (*e != ':')		// no terminating ':'?
		    اكسر;
		پای_ټکی = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    إذا (*s != NUL)		// skip over an empty "::"
	    {
		صحيح secure_save = secure;
		sctx_T	save_current_sctx = current_sctx;

		current_sctx.sc_نسخه = 1;
#که_تعریف_شوی FEAT_EVAL
		current_sctx.sc_sid = SID_MODELINE;
		current_sctx.sc_seq = 0;
		current_sctx.sc_lnum = lnum;
#د_که_پای

		// Make sure no risky things are executed as a side effect.
		secure = 1;

		retval = do_سېټ(s, OPT_MODELINE | OPT_LOCAL | flags);

		secure = secure_save;
		current_sctx = save_current_sctx;
		إذا (retval == FAIL)		// stop if error found
		    اكسر;
	    }
	    s = (e == کرښه_پای_ټکی) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_راوباسه();
	vim_free(linecopy);
    }
    أرجع retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    صحيح
bt_نورمال(buf_T *buf)
{
    أرجع buf != عدم && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    صحيح
bt_quickfix(buf_T *buf ناکارول_شوی)
{
#که_تعریف_شوی FEAT_QUICKFIX
    أرجع buf != عدم && buf->b_p_bt[0] == 'q';
#وإلا
    أرجع FALSE;
#د_که_پای
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    صحيح
bt_terminal(buf_T *buf ناکارول_شوی)
{
#إذا defined(FEAT_TERMINAL)
    أرجع buf != عدم && buf->b_p_bt[0] == 't';
#وإلا
    أرجع FALSE;
#د_که_پای
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    صحيح
bt_help(buf_T *buf)
{
    أرجع buf != عدم && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    صحيح
bt_prompt(buf_T *buf)
{
    أرجع buf != عدم && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#إذا defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    صحيح
bt_popup(buf_T *buf)
{
    أرجع buf != عدم && buf->b_p_bt != عدم
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#د_که_پای

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    صحيح
bt_nofilename(buf_T *buf)
{
    أرجع buf != عدم && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    ساكن صحيح
bt_nofileread(buf_T *buf)
{
    أرجع buf != عدم && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#إذا defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    صحيح
bt_nofile(buf_T *buf)
{
    أرجع buf != عدم && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#د_که_پای

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    صحيح
bt_dontwrite(buf_T *buf)
{
    أرجع buf != عدم && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    صحيح
bt_dontwrite_پیغام(buf_T *buf)
{
    إذا (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_سېټ));
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    صحيح
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    بدل (buf->b_p_bh[0])
    {
	حالة 'u':		    // "unload"
	حالة 'w':		    // "wipe"
	حالة 'd': أرجع FALSE;	    // "delete"
	حالة 'h': أرجع TRUE;	    // "hide"
    }
    أرجع (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#إذا defined(FEAT_QUICKFIX)
    إذا (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	إذا (buf->b_fnum == qf_ډېری_get_bufnr())
	    أرجع (char_u *)_(پیغام_qflist);
	وإلا
	    أرجع (char_u *)_(پیغام_loclist);
    }
#د_که_پای

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    إذا (bt_nofilename(buf))
    {
#که_تعریف_شوی FEAT_TERMINAL
	إذا (buf->b_غړی != عدم)
	    أرجع غړی_get_حالت_متن(buf->b_غړی);
#د_که_پای
	إذا (buf->b_fname != عدم)
	    أرجع buf->b_fname;
	إذا (buf == cmdwin_buf)
	    أرجع (char_u *)_("[Command Line]");
#که_تعریف_شوی FEAT_دنده_CHANNEL
	إذا (bt_prompt(buf))
	    أرجع (char_u *)_("[Prompt]");
#د_که_پای
#که_تعریف_شوی FEAT_PROP_POPUP
	إذا (bt_popup(buf))
	    أرجع (char_u *)_("[Popup]");
#د_که_پای
	أرجع (char_u *)_("[Scratch]");
    }

    إذا (buf->b_fname == عدم)
	أرجع buf_get_fname(buf);
    أرجع عدم;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    إذا (buf->b_fname == عدم)
	أرجع (char_u *)_("[No Name]");
    أرجع buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    فراغ
سېټ_buflisted(صحيح on)
{
    إذا (on == curbuf->b_p_bl)
	أرجع;

    curbuf->b_p_bl = on;
    إذا (on)
	apply_autocmds(EVENT_BUFADD, عدم, عدم, FALSE, curbuf);
    وإلا
	apply_autocmds(EVENT_BUFDELETE, عدم, عدم, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    صحيح
buf_contents_بدل_شوی(buf_T *buf)
{
    buf_T	*newbuf;
    صحيح		توپیر = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(عدم, عدم, (linenr_T)1, BLN_DUMMY);
    إذا (newbuf == عدم)
	أرجع TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    إذا (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_بفر(newbuf, FALSE);
	أرجع TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    إذا (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_بفر(newbuf, FALSE);
	أرجع TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    بلاک_autocmds();
    إذا (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	إذا (buf->b_ml.ml_کرښه_شمېر == curbuf->b_ml.ml_کرښه_شمېر)
	{
	    توپیر = FALSE;
	    لكل (lnum = 1; lnum <= curbuf->b_ml.ml_کرښه_شمېر; ++lnum)
		إذا (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    توپیر = TRUE;
		    اكسر;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    إذا (curbuf != newbuf)	// safety check
	wipe_بفر(newbuf, FALSE);

    unblock_autocmds();

    أرجع توپیر;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    فراغ
wipe_بفر(
    buf_T	*buf,
    صحيح		aucmd)	    // When TRUE trigger autocommands.
{
    إذا (buf->b_fnum == سر_file_عدد - 1)
	--سر_file_عدد;

    إذا (!aucmd)		    // Don't trigger BufDelete autocommands here.
	بلاک_autocmds();

    close_بفر(عدم, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    إذا (!aucmd)
	unblock_autocmds();
}
