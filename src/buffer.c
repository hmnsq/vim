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

#شامل "vim.h"


#اگر_تعریف_شدہ FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_گہرائی 100
#اگر_ختم

ساکن خالی	enter_بفر(buf_T *buf);
ساکن خالی	buflist_getfpos(خالی);
ساکن char_u	*buflist_مطابقت(regmatch_T *rmp, buf_T *buf, صحیح ignore_case);
ساکن char_u	*fname_مطابقت(regmatch_T *rmp, char_u *name, صحیح ignore_case);
#اگر_تعریف_شدہ UNIX
ساکن buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
ساکن صحیح	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
ساکن صحیح	buf_same_ino(buf_T *buf, stat_T *stp);
#ورنہ
ساکن صحیح	otherfile_buf(buf_T *buf, char_u *ffname);
#اگر_ختم
ساکن صحیح	قدر_بدلا(char_u *str, char_u **آخری);
#اگر defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
ساکن صحیح build_stl_str_hl_local(stl_منوال_T منوال, win_T *wp,
		char_u *برآمد, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, صحیح opt_scope, صحیح fillchar, صحیح maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, صحیح *lbreaks, صحیح *حاصل_hl);
#اگر_ختم
ساکن صحیح	append_arg_نمبر(win_T *wp, char_u *buf, size_t buflen, صحیح جوڑو_file);
ساکن خالی	free_بفر(buf_T *);
ساکن خالی	free_بفر_stuff(buf_T *buf, صحیح free_options);
ساکن صحیح	bt_nofileread(buf_T *buf);
ساکن صحیح	do_بفر_ext(صحیح action, صحیح آغاز, صحیح dir, صحیح گنتی, صحیح flags);

#اگر_تعریف_شدہ UNIX
# define dev_T dev_t
#ورنہ
# define dev_T غیرعلامتی
#اگر_ختم

#تعریف_کرو FOR_ALL_BUFS_FROM_آخری(buf) \
    برائے ((buf) = lastbuf; (buf) != تہی; (buf) = (buf)->b_پچھلا)

#اگر defined(FEAT_QUICKFIX)
ساکن حرف *پیغام_loclist = N_("[Location List]");
ساکن حرف *پیغام_qflist = N_("[Quickfix List]");
#اگر_ختم

// Number of times free_buffer() was called.
ساکن صحیح	buf_free_گنتی = 0;

ساکن صحیح	چوٹی_file_عدد = 1;	// highest file number
ساکن garray_T buf_reuse = GA_خالی_خانے;	// file numbers to recycle

    ساکن خالی
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    کھڑکی_layout_تالا();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"اگر exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    کھڑکی_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    ساکن صحیح
calc_percentage(طویل part, طویل پورا)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    واپس (part > 1000000L)
	? (صحیح)(part / (پورا / 100L))
	: (صحیح)((part * 100L) / پورا);
}

/*
 * Return the highest possible buffer number.
 */
    صحیح
get_highest_fnum(خالی)
{
    واپس چوٹی_file_عدد - 1;
}

/*
 * Read data from buffer for retrying.
 */
    ساکن صحیح
read_بفر(
    صحیح		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    صحیح		flags)		    // extra flags for readfile()
{
    صحیح		retval = OK;
    linenr_T	سطر_گنتی;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    سطر_گنتی = curbuf->b_ml.ml_سطر_گنتی;
    retval = readfile(
	    read_stdin ? تہی : curbuf->b_ffname,
	    read_stdin ? تہی : curbuf->b_fname,
	    سطر_گنتی, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_بفر);
    اگر (retval == OK)
    {
	// Delete the binary lines.
	جب (--سطر_گنتی >= 0)
	    ml_delete((linenr_T)1);
    }
    ورنہ
    {
	// Delete the converted lines.
	جب (curbuf->b_ml.ml_سطر_گنتی > سطر_گنتی)
	    ml_delete(سطر_گنتی);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ستون = 0;

    اگر (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	اگر (!readonlymode && !BUFEMPTY())
	    بدلا();
	ورنہ اگر (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	اگر (retval == OK)
	{
#اگر_تعریف_شدہ FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, تہی, تہی, FALSE,
							      curbuf, &retval);
#ورنہ
	    apply_autocmds(EVENT_STDINREADPOST, تہی, تہی, FALSE, curbuf);
#اگر_ختم
	}
    }
    واپس retval;
}

#اگر defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    خالی
بفر_ensure_loaded(buf_T *buf)
{
    اگر (buf->b_ml.ml_mfp != تہی)
	واپس;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    اگر (curbuf == buf)
    {
	اگر (تبادلہ_exists_action != SEA_READONLY)
	    تبادلہ_exists_action = SEA_NONE;
	open_بفر(FALSE, تہی, 0);
	aucmd_restbuf(&aco);
    }
}
#اگر_ختم

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
open_بفر(
    صحیح		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    صحیح		flags_arg)	    // extra flags for readfile()
{
    صحیح		flags = flags_arg;
    صحیح		retval = OK;
    bufref_T	old_curbuf;
#اگر_تعریف_شدہ FEAT_SYN_HL
    طویل	old_tw = curbuf->b_p_tw;
#اگر_ختم
    صحیح		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    اگر (readonlymode && curbuf->b_ffname != تہی
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    اگر (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_بفر(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    اگر (curbuf->b_ml.ml_mfp != تہی)
		توڑو;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	اگر (curbuf == تہی)
	{
	    emsg(_(e_cannot_allocate_any_بفر_exiting));

	    // Don't try to do any saving, with "curbuf" NULL almost nothing
	    // will work.
	    v_dying = 2;
	    getout(2);
	}

	emsg(_(e_cannot_allocate_بفر_using_other_one));
	enter_بفر(curbuf);
#اگر_تعریف_شدہ FEAT_SYN_HL
	اگر (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(تہی, curwin);
#اگر_ختم
	واپس FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    اگر (curbuf->b_ml.ml_mfp != تہی)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    سیٹ_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_سیٹ = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    اگر (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    اگر (curbuf->b_ffname != تہی
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	    && netbeansReadFile
#اگر_ختم
       )
    {
	صحیح old_پیغام_silent = پیغام_silent;
#اگر_تعریف_شدہ UNIX
	صحیح	save_bin = curbuf->b_p_bin;
	صحیح perm;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	صحیح oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#اگر_ختم
#اگر_تعریف_شدہ UNIX
	perm = mch_getperm(curbuf->b_ffname);
	اگر (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	اگر (read_fifo)
	    curbuf->b_p_bin = TRUE;
#اگر_ختم
	اگر (shortmess(SHM_FILEINFO))
	    پیغام_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#اگر_تعریف_شدہ UNIX
	اگر (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    اگر (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_بفر(FALSE, eap, flags);
	}
#اگر_ختم
	پیغام_silent = old_پیغام_silent;
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#اگر_ختم
	// Help buffer is filtered.
	اگر (bt_help(curbuf))
	    fix_help_بفر();
    }
    ورنہ اگر (read_stdin)
    {
	صحیح	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(تہی, تہی, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, تہی,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	اگر (retval == OK)
	    retval = read_بفر(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    اگر (curbuf->b_ml.ml_mfp != تہی
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    اگر (curbuf->b_flags & BF_NEVERLOADED)
    {
	(خالی)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    اگر ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != تہی)
		|| curbuf->b_modified_was_سیٹ	// autocmd did ":set modified"
#اگر_تعریف_شدہ FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != تہی)
#اگر_ختم
       )
	بدلا();
    ورنہ اگر (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_آخری_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_آخری_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_آخری_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#اگر_تعریف_شدہ FEAT_EVAL
    اگر (aborting())
#ورنہ
    اگر (got_int)
#اگر_ختم
	curbuf->b_flags |= BF_READERR;

#اگر_تعریف_شدہ FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#اگر_ختم

    // need to set w_topline, unless some autocommand already did that.
    اگر (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#اگر_تعریف_شدہ FEAT_DIFF
	curwin->w_topfill = 0;
#اگر_ختم
    }
#اگر_تعریف_شدہ FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, تہی, تہی, FALSE, curbuf, &retval);
#ورنہ
    apply_autocmds(EVENT_BUFENTER, تہی, تہی, FALSE, curbuf);
#اگر_ختم

    اگر (retval != OK)
	واپس retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    اگر (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != تہی)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	اگر (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    اگر ((flags & READ_NOWINENTER) == 0)
#اگر_تعریف_شدہ FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, تہی, تہی,
			FALSE, curbuf, &retval);
#ورنہ
	    apply_autocmds(EVENT_BUFWINENTER, تہی, تہی,
		    FALSE, curbuf);
#اگر_ختم

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    واپس retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    خالی
سیٹ_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == تہی ? 0 : buf->b_fnum;
    bufref->br_buf_free_گنتی = buf_free_گنتی;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    صحیح
bufref_valid(bufref_T *bufref)
{
    واپس bufref->br_buf_free_گنتی == buf_free_گنتی
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    صحیح
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_آخری(bp)
	اگر (bp == buf)
	    واپس TRUE;
    واپس FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
ساکن hashtab_T buf_hashtab;

    ساکن خالی
buf_hashtab_جوڑو(buf_T *buf)
{
    vim_snprintf((حرف *)buf->b_کلید, حجم(buf->b_کلید), "%x", buf->b_fnum);
    اگر (hash_جوڑو(&buf_hashtab, buf->b_کلید, "create buffer") == FAIL)
	emsg(_(e_بفر_cannot_be_registered));
}

    ساکن خالی
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_ڈھونڈو(&buf_hashtab, buf->b_کلید);

    اگر (!HASHITEM_خالی_خانے(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    ساکن صحیح
can_unload_بفر(buf_T *buf)
{
    صحیح	    can_unload = !buf->b_locked;

    اگر (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    اگر (wp->w_بفر == buf)
	    {
		can_unload = FALSE;
		توڑو;
	    }
    }
    // Don't unload the buffer while it's still being saved
    اگر (can_unload && buf->b_saving)
	can_unload = FALSE;

    اگر (!can_unload)
    {
	char_u *fname = buf->b_fname != تہی ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_مقصد_delete_بفر_that_is_اندر_use_str),
				fname != تہی ? fname : (char_u *)"[No Name]");
    }
    واپس can_unload;
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
    صحیح
close_بفر(
    win_T	*win,
    buf_T	*buf,
    صحیح		action,
    صحیح		abort_if_آخری,
    صحیح		ignore_abort,
    صحیح		سیٹ_context)
{
    صحیح		hiding_buf;
    bufref_T	bufref;
    صحیح		is_curwin = (curwin != تہی && curwin == win
				&& curwin->w_بفر == buf);
    tabpage_T	*the_curtab = curtab;
    صحیح		unload_buf = (action != 0);
    صحیح		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    صحیح		del_buf = (action == DOBUF_DEL || wipe_buf);
    صحیح		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    صحیح		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    اگر (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    ورنہ اگر (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    ورنہ اگر (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#اگر_تعریف_شدہ FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    اگر (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	اگر (رکن_کام_running(buf->b_رکن))
	{
	    اگر (wipe_buf || unload_buf)
	    {
		اگر (!can_unload_بفر(buf))
		    واپس FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    ورنہ
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	ورنہ اگر (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	ورنہ
	{
	    اگر (del_buf || unload_buf)
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
#اگر_ختم

    // Disallow deleting the buffer when it is locked (already being closed or
    // halfway a command that relies on it). Unloading is allowed.
    اگر ((del_buf || wipe_buf) && !can_unload_بفر(buf))
	واپس FALSE;

    اگر (سیٹ_context && win_valid && win->w_بفر == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	اگر (buf->b_nwindows == 1)
	    سیٹ_آخری_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.ستون, TRUE);
    }

    سیٹ_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    اگر ((win_valid || closed_popup) && win->w_بفر == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_بانٹو;
	اگر (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_مقصد_abort));
	    واپس FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_بانٹو;
	اگر (abort_if_آخری && one_کھڑکی())
	    // Autocommands made this the only window.
	    جاؤ aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	اگر (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_بانٹو;
	    اگر (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		جاؤ aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_بانٹو;
	    اگر (abort_if_آخری && one_کھڑکی())
		// Autocommands made this the only window.
		جاؤ aucmd_abort;
	}
#اگر_تعریف_شدہ FEAT_EVAL
	// autocmds may abort script processing
	اگر (!ignore_abort && aborting())
	    واپس FALSE;
#اگر_ختم
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    اگر (is_curwin && curwin != win && win_valid)
    {
	بلاک_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    اگر (bt_quickfix(buf) && win_valid && win->w_بفر == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_بفر == buf && buf->b_nwindows == 1);

#اگر_تعریف_شدہ FEAT_DIFF
    اگر (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#اگر_ختم

    // Return when another window is displaying the buffer or when not unloaded.
    اگر (!hiding_buf || !unload_buf)
	واپس FALSE;

    // Always remove the buffer when there is no file name.
    اگر (buf->b_ffname == تہی)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    اگر (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	واپس FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    اگر ((win_valid || closed_popup) && win->w_بفر == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_بفر = تہی;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    اگر (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_پچھلا != تہی || buf->b_اگلا != تہی
#اگر defined(EXITFREE)
				|| entered_free_all_mem
#اگر_ختم
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	اگر (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    اگر (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, حجم(صحیح), 50);
	    اگر (ga_grow(&buf_reuse, 1) == OK)
		((صحیح *)buf_reuse.ga_ڈیٹا)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	اگر (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ورنہ
	    buf->b_sfname = تہی;
	VIM_CLEAR(buf->b_ffname);
	اگر (buf->b_پچھلا == تہی)
	    firstbuf = buf->b_اگلا;
	ورنہ
	    buf->b_پچھلا->b_اگلا = buf->b_اگلا;
	اگر (buf->b_اگلا == تہی)
	    lastbuf = buf->b_پچھلا;
	ورنہ
	    buf->b_اگلا->b_پچھلا = buf->b_پچھلا;
	free_بفر(buf);
    }
    ورنہ
    {
	اگر (del_buf)
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
	اگر (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    أكد(!win_valid || !is_curwin || win == curwin);
    واپس TRUE;
}

/*
 * Make buffer not contain a file.
 */
    خالی
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_سطر_گنتی = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_آغاز_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_آغاز_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_آغاز_bomb = FALSE;
    buf->b_ml.ml_mfp = تہی;
    buf->b_ml.ml_flags = ML_خالی_خانے;		// empty buffer
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    netbeans_deleted_all_سطریں(buf);
#اگر_ختم
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
    صحیح
buf_freeall(buf_T *buf, صحیح flags)
{
    صحیح		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    صحیح		is_curwin = (curwin != تہی && curwin->w_بفر == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_بانٹو;
    سیٹ_bufref(&bufref, buf);
    اگر (buf->b_ml.ml_mfp != تہی)
    {
	اگر (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    واپس FALSE;
    }
    اگر ((flags & BFA_DEL) && buf->b_p_bl)
    {
	اگر (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    واپس FALSE;
    }
    اگر (flags & BFA_WIPE)
    {
	اگر (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    واپس FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_بانٹو;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    اگر (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	بلاک_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#اگر_تعریف_شدہ FEAT_EVAL
    // autocmds may abort script processing
    اگر ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	واپس FALSE;
#اگر_ختم

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    اگر (buf == curbuf && !is_curbuf)
	واپس FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    اگر (buf == curbuf && VIsual_فعال
#اگر defined(EXITFREE)
	    && !entered_free_all_mem
#اگر_ختم
	    )
	انتہا_visual_منوال();

#اگر_تعریف_شدہ FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    اگر (curwin != تہی && curwin->w_بفر == buf)
	reset_synblock(curwin);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    اگر (win->w_بفر == buf)
		clearFolding(win);
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TCL
    tcl_بفر_free(buf);
#اگر_ختم
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_سطر_گنتی = 0;    // no lines in buffer
    اگر ((flags & BFA_KEEP_UNDO) == 0)
	// free the memory allocated for undo
	// and reset all undo information
	u_clearallandblockfree(buf);
#اگر_تعریف_شدہ FEAT_SYN_HL
    syntax_clear(&buf->b_s);	    // reset syntax info
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    clear_buf_prop_types(buf);
#اگر_ختم
    buf->b_flags &= ~BF_READERR;    // a read error is no longer relevant
    واپس TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    ساکن خالی
free_بفر(buf_T *buf)
{
    ++buf_free_گنتی;
    free_بفر_stuff(buf, TRUE);
#اگر_تعریف_شدہ FEAT_EVAL
    // b:changedtick uses an item in buf_T, remove it now
    dictitem_remove(buf->b_vars, (dictitem_T *)&buf->b_ct_di, "free buffer");
    unref_var_dict(buf->b_vars);
    remove_listeners(buf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_LUA
    lua_بفر_free(buf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MZSCHEME
    mzscheme_بفر_free(buf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PERL
    perl_buf_free(buf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PYTHON
    python_بفر_free(buf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PYTHON3
    python3_بفر_free(buf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_RUBY
    ruby_بفر_free(buf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    channel_بفر_free(buf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TERMINAL
    free_terminal(buf);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    vim_free(buf->b_prompt_متن);
    free_callback(&buf->b_prompt_callback);
    free_callback(&buf->b_prompt_مداخلت);
#اگر_ختم

    buf_hashtab_remove(buf);

    aubuflocal_remove(buf);

    اگر (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_اگلا = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    ورنہ
    {
	vim_free(buf);
	اگر (curbuf == buf)
	    curbuf = تہی;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    ساکن خالی
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_نمبر;
    di->di_tv.v_تالا = VAR_FIXED;
    di->di_tv.vval.v_نمبر = 0;

#اگر_تعریف_شدہ FEAT_EVAL
    STRCPY(buf->b_ct_di.di_کلید, "changedtick");
    (خالی)dict_جوڑو(buf->b_vars, di);
#اگر_ختم
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    ساکن خالی
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    جب (buf->b_wininfo != تہی)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_اگلا;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    ساکن خالی
free_بفر_stuff(
    buf_T	*buf,
    صحیح		free_options)		// free options as well
{
    اگر (free_options)
    {
	clear_wininfo(buf);		// including window-local options
	free_buf_options(buf, TRUE);
#اگر_تعریف_شدہ FEAT_SPELL
	ga_clear(&buf->b_s.b_langp);
#اگر_ختم
    }
#اگر_تعریف_شدہ FEAT_EVAL
    {
	varnumber_T tick = CHANGEDTICK(buf);

	vars_clear(&buf->b_vars->dv_hashtab); // free all buffer variables
	hash_init(&buf->b_vars->dv_hashtab);
	init_changedtick(buf);
	CHANGEDTICK(buf) = tick;
	remove_listeners(buf);
    }
#اگر_ختم
    uc_clear(&buf->b_ucmds);		// clear local user commands
#اگر_تعریف_شدہ FEAT_SIGNS
    buf_delete_signs(buf, (char_u *)"*");	// delete any signs
#اگر_ختم
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    netbeans_file_killed(buf);
#اگر_ختم
    map_clear_منوال(buf, MAP_ALL_MODES, TRUE, FALSE);  // clear local mappings
    map_clear_منوال(buf, MAP_ALL_MODES, TRUE, TRUE);   // clear local abbrevs
    VIM_CLEAR(buf->b_آغاز_fenc);
}

/*
 * Free one wininfo_T.
 */
    خالی
free_wininfo(wininfo_T *wip)
{
    اگر (wip->wi_optset)
    {
	clear_winopt(&wip->wi_opt);
#اگر_تعریف_شدہ FEAT_FOLDING
	deleteFoldRecurse(&wip->wi_folds);
#اگر_ختم
    }
    vim_free(wip);
}

/*
 * Go to another buffer.  Handles the result of the ATTENTION dialog.
 */
    خالی
goto_بفر(
    exarg_T	*eap,
    صحیح		آغاز,
    صحیح		dir,
    صحیح		گنتی)
{
    bufref_T	old_curbuf;
    صحیح		save_sea = تبادلہ_exists_action;
    صحیح		skip_help_buf;

    انتخاب (eap->cmdidx)
    {
	حالت CMD_bnext:
	حالت CMD_sbnext:
	حالت CMD_b_اگلا:
	حالت CMD_bprevious:
	حالت CMD_sb_اگلا:
	حالت CMD_sbprevious:
	    skip_help_buf = TRUE;
	    توڑو;
	طےشدہ:
	    skip_help_buf = FALSE;
	    توڑو;
    }

    سیٹ_bufref(&old_curbuf, curbuf);

    اگر (تبادلہ_exists_action == SEA_NONE)
	تبادلہ_exists_action = SEA_DIALOG;
    (خالی)do_بفر_ext(*eap->cmd == 's' ? DOBUF_بانٹو : DOBUF_GOTO, آغاز, dir, گنتی,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    اگر (تبادلہ_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#اگر defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#اگر_ختم

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	تبادلہ_exists_action = save_sea;
	تبادلہ_exists_did_quit = TRUE;

#اگر defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#اگر_ختم
    }
    ورنہ
	handle_تبادلہ_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    خالی
handle_تبادلہ_exists(bufref_T *old_curbuf)
{
#اگر defined(FEAT_EVAL)
    cleanup_T	cs;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SYN_HL
    طویل	old_tw = curbuf->b_p_tw;
#اگر_ختم
    buf_T	*buf;

    اگر (تبادلہ_exists_action == SEA_QUIT)
    {
#اگر defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#اگر_ختم

	// User selected Quit at ATTENTION prompt.  Go back to previous
	// buffer.  If that buffer is gone or the same as the current one,
	// open a new, empty buffer.
	تبادلہ_exists_action = SEA_NONE;	// don't want it again
	تبادلہ_exists_did_quit = TRUE;
	close_بفر(curwin, curbuf, DOBUF_UNLOAD, FALSE, FALSE, TRUE);
	اگر (old_curbuf == تہی || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    بلاک_autocmds();
	    buf = buflist_new(تہی, تہی, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	ورنہ
	    buf = old_curbuf->br_buf;
	اگر (buf != تہی)
	{
	    صحیح old_پیغام_silent = پیغام_silent;

	    اگر (shortmess(SHM_FILEINFO))
		پیغام_silent = 1;  // prevent fileinfo message
	    enter_بفر(buf);
	    // restore msg_silent, so that the command line will be shown
	    پیغام_silent = old_پیغام_silent;

#اگر_تعریف_شدہ FEAT_SYN_HL
	    اگر (old_tw != curbuf->b_p_tw)
		check_colorcolumn(تہی, curwin);
#اگر_ختم
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#اگر defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#اگر_ختم
    }
    ورنہ اگر (تبادلہ_exists_action == SEA_RECOVER)
    {
#اگر defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#اگر_ختم

	// User selected Recover at ATTENTION prompt.
	پیغام_scroll = TRUE;
	ml_recover(FALSE);
	پیغام_puts("\n");	// don't overwrite the last message
	cmdline_صف = پیغام_صف;
	do_modelines(0);

#اگر defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#اگر_ختم
    }
    تبادلہ_exists_action = SEA_NONE;
}

/*
 * Make the current buffer empty.
 * Used when it is wiped out and it's the last buffer.
 */
    ساکن صحیح
خالی_خانے_curbuf(
    صحیح close_others,
    صحیح forceit,
    صحیح action)
{
    صحیح	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    اگر (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_آخری_بفر));
	واپس FAIL;
    }

    سیٹ_bufref(&bufref, buf);
    اگر (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, تہی, تہی, تہی, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    اگر (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_بفر(تہی, buf, action, FALSE, FALSE, FALSE);
    اگر (!close_others)
	need_fileinfo = FALSE;
    ورنہ اگر (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    واپس retval;
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
    ساکن صحیح
do_بفر_ext(
    صحیح		action,
    صحیح		آغاز,
    صحیح		dir,		// FORWARD or BACKWARD
    صحیح		گنتی,		// buffer number or number of buffers
    صحیح		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    صحیح		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    انتخاب (آغاز)
    {
	حالت DOBUF_پہلا:   buf = firstbuf; توڑو;
	حالت DOBUF_آخری:    buf = lastbuf;  توڑو;
	طےشدہ:	    buf = curbuf;   توڑو;
    }
    اگر (آغاز == DOBUF_MOD)	    // find next modified buffer
    {
	جب (گنتی-- > 0)
	{
	    کرو
	    {
		buf = buf->b_اگلا;
		اگر (buf == تہی)
		    buf = firstbuf;
	    }
	    جب (buf != curbuf && !buf_Is_بدلا(buf));
	}
	اگر (!buf_Is_بدلا(buf))
	{
	    emsg(_(e_no_modified_بفر_ملا));
	    واپس FAIL;
	}
    }
    ورنہ اگر (آغاز == DOBUF_پہلا && گنتی) // find specified buffer number
    {
	جب (buf != تہی && buf->b_fnum != گنتی)
	    buf = buf->b_اگلا;
    }
    ورنہ
    {
	صحیح help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = تہی;
	جب (گنتی > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    اگر (bp == تہی)
		bp = buf;
	    اگر (dir == FORWARD)
	    {
		buf = buf->b_اگلا;
		اگر (buf == تہی)
		    buf = firstbuf;
	    }
	    ورنہ
	    {
		buf = buf->b_پچھلا;
		اگر (buf == تہی)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    اگر (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--گنتی;
		bp = تہی;	// use this buffer as new starting point
	    }
	    اگر (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_بفر));
		واپس FAIL;
	    }
	}
    }

    اگر (buf == تہی)	    // could not find it
    {
	اگر (آغاز == DOBUF_پہلا)
	{
	    // don't warn when deleting
	    اگر (!unload)
		semsg(_(e_بفر_nr_does_not_exist), گنتی);
	}
	ورنہ اگر (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_آخری_بفر));
	ورنہ
	    emsg(_(e_cannot_go_before_پہلا_بفر));
	واپس FAIL;
    }
#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	واپس OK;
#اگر_ختم
    اگر (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_سیٹ_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	واپس FAIL;

    اگر ((action == DOBUF_GOTO || action == DOBUF_بانٹو)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_بفر_nr_does_not_exist), گنتی);
	واپس FAIL;
    }

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    /*
     * delete buffer "buf" from memory and/or the list
     */
    اگر (unload)
    {
	صحیح	forward;
	bufref_T bufref;

	اگر (!can_unload_بفر(buf))
	    واپس FAIL;

	سیٹ_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	اگر (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == تہی && !buf->b_p_bl)
	    واپس FAIL;

	اگر ((flags & DOBUF_FORCEIT) == 0 && buf_Is_بدلا(buf))
	{
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    اگر ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		اگر (رکن_کام_running(buf->b_رکن))
		{
		    اگر (رکن_confirm_stop(buf) == FAIL)
			واپس FAIL;
		}
		ورنہ
# endif
		{
		    dialog_بدلا(buf, FALSE);
		    اگر (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			واپس FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    اگر (buf_Is_بدلا(buf))
			واپس FAIL;
		}
	    }
	    ورنہ
#اگر_ختم
	    {
		no_write_پیغام_buf(buf);
		واپس FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	اگر (buf == curbuf && VIsual_فعال)
	    انتہا_visual_منوال();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    اگر (bp->b_p_bl && bp != buf)
		توڑو;
	اگر (bp == تہی && buf == curbuf)
	    واپس خالی_خانے_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	جب (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_بفر->b_locked > 0)
		   && (!ONE_کھڑکی || پہلا_tabpage->tp_اگلا != تہی))
	{
	    اگر (win_close(curwin, FALSE) == FAIL)
		توڑو;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	اگر (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    اگر (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_بفر(تہی, buf, action, FALSE, FALSE, FALSE);
	    واپس OK;
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
	buf = تہی;	// selected buffer
	bp = تہی;	// used when no loaded buffer found
	اگر (au_new_curbuf.br_buf != تہی && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_بانٹو)
	    buf = au_new_curbuf.br_buf;
	ورنہ اگر (curwin->w_jumplistlen > 0)
	{
	    صحیح     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    اگر (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    جب (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		اگر (buf != تہی)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    اگر (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_بانٹو)
			buf = تہی;
		    ورنہ اگر (buf->b_ml.ml_mfp == تہی)
		    {
			// skip unloaded buf, but may keep it for later
			اگر (bp == تہی)
			    bp = buf;
			buf = تہی;
		    }
		}
		اگر (buf != تہی)   // found a valid buffer: stop searching
		    توڑو;
		// advance to older entry in jump list
		اگر (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    توڑو;
		اگر (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		اگر (jumpidx == forward)		// List exhausted for sure
		    توڑو;
	    }
	}

	اگر (buf == تہی)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_اگلا;
	    برائے (;;)
	    {
		اگر (buf == تہی)
		{
		    اگر (!forward)	// tried both directions
			توڑو;
		    buf = curbuf->b_پچھلا;
		    forward = FALSE;
		    جاری;
		}
		// in non-help buffer, try to skip help buffers, and vv
		اگر (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_بانٹو)
		{
		    اگر (buf->b_ml.ml_mfp != تہی)   // found loaded buffer
			توڑو;
		    اگر (bp == تہی)	// remember unloaded buf for later
			bp = buf;
		}
		اگر (forward)
		    buf = buf->b_اگلا;
		ورنہ
		    buf = buf->b_پچھلا;
	    }
	}
	اگر (buf == تہی)	// No loaded buffer, use unloaded one
	    buf = bp;
	اگر (buf == تہی)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		اگر (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_بانٹو)
		    توڑو;
	}
	اگر (buf == تہی)	// Still no buffer, just take one
	{
	    اگر (curbuf->b_اگلا != تہی)
		buf = curbuf->b_اگلا;
	    ورنہ
		buf = curbuf->b_پچھلا;
	    اگر (bt_quickfix(buf)
		    || (buf != تہی && buf != curbuf && buf->b_locked_بانٹو))
		buf = تہی;
	}
    }

    اگر (buf == تہی)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	واپس خالی_خانے_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    اگر (action == DOBUF_بانٹو && swbuf_goto_win_with_buf(buf) != تہی)
	واپس OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    اگر (buf != curbuf && buf->b_locked_بانٹو)
    {
	emsg(_(e_cannot_switch_مقصد_a_closing_بفر));
	واپس FAIL;
    }
    اگر (action == DOBUF_بانٹو && win_بانٹو(0, 0) == FAIL) // split window first
	واپس FAIL;

    // go to current buffer - nothing to do
    اگر (buf == curbuf)
	واپس OK;

    // Check if the current buffer may be abandoned.
    اگر (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	اگر ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    اگر (رکن_کام_running(curbuf->b_رکن))
	    {
		اگر (رکن_confirm_stop(curbuf) == FAIL)
		    واپس FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    ورنہ
# endif
	    {
		bufref_T bufref;

		سیٹ_bufref(&bufref, buf);
		dialog_بدلا(curbuf, FALSE);
		اگر (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    واپس FAIL;

		اگر (buf_Is_بدلا(curbuf))
		{
		    no_write_پیغام();
		    واپس FAIL;
		}
	    }
	}
	ورنہ
#اگر_ختم
	{
	    no_write_پیغام();
	    واپس FAIL;
	}
    }

    // Go to the other buffer.
    سیٹ_curbuf(buf, action);

    اگر (action == DOBUF_بانٹو)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#اگر defined(FEAT_EVAL)
    اگر (aborting())	    // autocmds may abort script processing
	واپس FAIL;
#اگر_ختم

    واپس OK;
}

    صحیح
do_بفر(
    صحیح		action,
    صحیح		آغاز,
    صحیح		dir,		// FORWARD or BACKWARD
    صحیح		گنتی,		// buffer number or number of buffers
    صحیح		forceit)	// TRUE when using !
{
    واپس do_بفر_ext(action, آغاز, dir, گنتی,
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
    صحیح		command,
    char_u	*arg,		// pointer to extra arguments
    صحیح		addr_گنتی,
    صحیح		آغاز_bnr,	// first buffer number in a range
    صحیح		انتہا_bnr,	// buffer nr or last buffer nr in a range
    صحیح		forceit)
{
    صحیح		do_current = 0;	// delete current buffer?
    صحیح		deleted = 0;	// number of buffers deleted
    حرف	*errormsg = تہی; // return value
    صحیح		bnr;		// buffer number
    char_u	*p;

    اگر (addr_گنتی == 0)
    {
	(خالی)do_بفر(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    ورنہ
    {
	اگر (addr_گنتی == 2)
	{
	    اگر (*arg)		// both range and argument is not allowed
		واپس ex_errmsg(e_trailing_characters_str, arg);
	    bnr = آغاز_bnr;
	}
	ورنہ	// addr_count == 1
	    bnr = انتہا_bnr;

	برائے ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    اگر (bnr == curbuf->b_fnum)
		do_current = bnr;
	    ورنہ اگر (do_بفر_ext(command, DOBUF_پہلا, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    اگر (addr_گنتی == 2)
	    {
		اگر (++bnr > انتہا_bnr)
		    توڑو;
	    }
	    ورنہ    // addr_count == 1
	    {
		arg = skipwhite(arg);
		اگر (*arg == NUL)
		    توڑو;
		اگر (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    اگر (bnr < 0)	    // failed
			توڑو;
		    arg = p;
		}
		ورنہ
		    bnr = getdigits(&arg);
	    }
	}
	اگر (!got_int && do_current && do_بفر(command, DOBUF_پہلا,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	اگر (deleted == 0)
	{
	    اگر (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    ورنہ اگر (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    ورنہ
		STRCPY(IObuff, _(e_no_buffers_were_wiped_برآمد));
	    errormsg = (حرف *)IObuff;
	}
	ورنہ اگر (deleted >= p_report)
	{
	    اگر (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    ورنہ اگر (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    ورنہ
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    واپس errormsg;
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
    خالی
سیٹ_curbuf(buf_T *buf, صحیح action)
{
    buf_T	*prevbuf;
    صحیح		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#اگر_تعریف_شدہ FEAT_SYN_HL
    طویل	old_tw = curbuf->b_p_tw;
#اگر_ختم
    bufref_T	newbufref;
    bufref_T	prevbufref;
    صحیح		valid;

    setpcmark();
    اگر ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = curbuf->b_fnum; // remember alternate file
    buflist_altfpos(curwin);			 // remember curpos

    // Don't restart Select mode after switching to another buffer.
    VIsual_reselect = FALSE;

    // close_windows() or apply_autocmds() may change curbuf and wipe out "buf"
    prevbuf = curbuf;
    سیٹ_bufref(&prevbufref, prevbuf);
    سیٹ_bufref(&newbufref, buf);

    // Autocommands may delete the current buffer and/or the buffer we want to
    // go to.  In those cases don't close the buffer.
    اگر (!apply_autocmds(EVENT_BUFLEAVE, تہی, تہی, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#اگر_تعریف_شدہ FEAT_EVAL
		&& !aborting()
#اگر_ختم
	       ))
    {
#اگر_تعریف_شدہ FEAT_SYN_HL
	اگر (prevbuf == curwin->w_بفر)
	    reset_synblock(curwin);
#اگر_ختم
	اگر (unload)
	    close_windows(prevbuf, FALSE);
#اگر defined(FEAT_EVAL)
	اگر (bufref_valid(&prevbufref) && !aborting())
#ورنہ
	اگر (bufref_valid(&prevbufref))
#اگر_ختم
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    اگر (prevbuf == curbuf
		    && ((State & منوال_داخل_کرو) == 0 || curbuf->b_nwindows <= 1))
		u_sync(FALSE);
	    close_بفر(curwin, prevbuf,
		    unload ? action : (action == DOBUF_GOTO
			&& !buf_hide(prevbuf)
			&& !buf_Is_بدلا(prevbuf)) ? DOBUF_UNLOAD : 0,
		    FALSE, FALSE, TRUE);
	}
    }
    // An autocommand may have deleted "buf", already entered it (e.g., when
    // it did ":bunload") or aborted the script processing.
    // If curwin->w_buffer is null, enter_buffer() will make it valid again
    valid = buf_valid(buf);
    اگر ((valid && buf != curbuf
#اگر_تعریف_شدہ FEAT_EVAL
		&& !aborting()
#اگر_ختم
	) || curwin->w_بفر == تہی)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	اگر (!valid)
	    enter_بفر(lastbuf);
	ورنہ
	    enter_بفر(buf);
#اگر_تعریف_شدہ FEAT_SYN_HL
	اگر (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(تہی, curwin);
#اگر_ختم
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    ساکن خالی
enter_بفر(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    اگر (VIsual_فعال
#اگر defined(EXITFREE)
	    && !entered_free_all_mem
#اگر_ختم
	    )
	انتہا_visual_منوال();

    اگر (curwin->w_بفر != تہی)
	--curwin->w_بفر->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_بفر = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    اگر (!buf->b_help)
	get_winopts(buf);
#اگر_تعریف_شدہ FEAT_FOLDING
    ورنہ
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#اگر_ختم

#اگر_تعریف_شدہ FEAT_DIFF
    اگر (curwin->w_p_diff)
	diff_buf_جوڑو(curbuf);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SYN_HL
    curwin->w_s = &(curbuf->b_s);
#اگر_ختم

    // Cursor on first line by default.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.ستون = 0;
    curwin->w_cursor.coladd = 0;
    curwin->w_سیٹ_curswant = true;
    curwin->w_topline_was_سیٹ = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // Make sure the buffer is loaded.
    اگر (curbuf->b_ml.ml_mfp == تہی)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	اگر (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_بفر(FALSE, تہی, 0);
    }
    ورنہ
    {
	اگر (!پیغام_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(خالی)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#اگر_تعریف_شدہ FEAT_DIFF
	curwin->w_topfill = 0;
#اگر_ختم
	apply_autocmds(EVENT_BUFENTER, تہی, تہی, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, تہی, تہی, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    اگر (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    اگر (curwin->w_topline == 1 && !curwin->w_topline_was_سیٹ)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#اگر_ختم

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#اگر_تعریف_شدہ FEAT_KEYMAP
    اگر (curbuf->b_kmap_state & KEYMAP_INIT)
	(خالی)keymap_init();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    اگر (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(خالی)parse_spelllang(curwin);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_VIMINFO
    curbuf->b_آخری_مستعمل = vim_وقت();
#اگر_ختم

    redraw_later(UPD_NOT_VALID);
}

#اگر defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    خالی
do_autochdir(خالی)
{
    اگر ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != تہی
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	آخری_chdir_reason = "autochdir";
    }
}
#اگر_ختم

    خالی
no_write_پیغام_buf(buf_T *buf)
{
#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (رکن_کام_running(buf->b_رکن))
	emsg(_(e_کام_still_running_جوڑو_bang_مقصد_انتہا_the_کام));
    ورنہ
#اگر_ختم
	semsg(_(e_no_write_since_آخری_change_for_بفر_nr_جوڑو_bang_مقصد_override),
		buf->b_fnum);
}

    خالی
no_write_پیغام(خالی)
{
#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (رکن_کام_running(curbuf->b_رکن))
	emsg(_(e_کام_still_running_جوڑو_bang_مقصد_انتہا_the_کام));
    ورنہ
#اگر_ختم
	emsg(_(e_no_write_since_آخری_change_جوڑو_bang_مقصد_override));
}

    خالی
no_write_پیغام_nobang(buf_T *buf غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (رکن_کام_running(buf->b_رکن))
	emsg(_(e_کام_still_running));
    ورنہ
#اگر_ختم
	emsg(_(e_no_write_since_آخری_change));
}

/*
 * functions for dealing with the buffer list
 */

/*
 * Return TRUE if the current buffer is empty, unnamed, unmodified and used in
 * only one window.  That means it can be re-used.
 */
    صحیح
curbuf_reusable(خالی)
{
    واپس (curbuf != تہی
	&& curbuf->b_ffname == تہی
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == تہی || BUFEMPTY())
	&& !bt_quickfix(curbuf)
	&& !curbuf_Is_بدلا());
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
    صحیح		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#اگر_تعریف_شدہ UNIX
    stat_T	st;
#اگر_ختم

    اگر (چوٹی_file_عدد == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#اگر_تعریف_شدہ UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    اگر (sfname == تہی || mch_stat((حرف *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#اگر_ختم
    اگر (ffname != تہی && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#اگر_تعریف_شدہ UNIX
		buflist_findname_stat(ffname, &st)
#ورنہ
		buflist_findname(ffname)
#اگر_ختم
		) != تہی)
    {
	vim_free(ffname);
	اگر (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? تہی : curwin,
						      lnum, (colnr_T)0, FALSE);

	اگر ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	اگر ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    سیٹ_bufref(&bufref, buf);
	    اگر (!(flags & BLN_DUMMY))
	    {
		اگر (apply_autocmds(EVENT_BUFADD, تہی, تہی, FALSE, buf)
			&& !bufref_valid(&bufref))
		    واپس تہی;
	    }
	}
	واپس buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = تہی;
    اگر ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	سیٹ_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    واپس تہی;
	}
#اگر_ختم
	اگر (!bufref_valid(&bufref))
	    buf = تہی;		// buf was deleted; allocate a new buffer
    }
    اگر (buf != curbuf || curbuf == تہی)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	اگر (buf == تہی)
	{
	    vim_free(ffname);
	    واپس تہی;
	}
#اگر_تعریف_شدہ FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	اگر (buf->b_vars == تہی)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    واپس تہی;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#اگر_ختم
	init_changedtick(buf);
    }

    اگر (ffname != تہی)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    اگر ((ffname != تہی && (buf->b_ffname == تہی || buf->b_sfname == تہی))
	    || buf->b_wininfo == تہی)
    {
	اگر (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ورنہ
	    buf->b_sfname = تہی;
	VIM_CLEAR(buf->b_ffname);
	اگر (buf != curbuf)
	    free_بفر(buf);
	واپس تہی;
    }

    اگر (buf == curbuf)
    {
	free_بفر_stuff(buf, FALSE);	// delete local variables et al.

	// Init the options.
	buf->b_p_initialized = false;
	buf_copy_options(buf, BCO_ENTER);

#اگر_تعریف_شدہ FEAT_KEYMAP
	// need to reload lmaps and set b:keymap_name
	curbuf->b_kmap_state |= KEYMAP_INIT;
#اگر_ختم
    }
    ورنہ
    {
	// put the new buffer at the end of the buffer list
	buf->b_اگلا = تہی;
	اگر (firstbuf == تہی)		// buffer list is empty
	{
	    buf->b_پچھلا = تہی;
	    firstbuf = buf;
	}
	ورنہ				// append new buffer at end of list
	{
	    lastbuf->b_اگلا = buf;
	    buf->b_پچھلا = lastbuf;
	}
	lastbuf = buf;

	اگر ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((صحیح *)buf_reuse.ga_ڈیٹا)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    جب (buf->b_پچھلا != تہی && buf->b_fnum < buf->b_پچھلا->b_fnum)
	    {
		buf_T	*پچھلا = buf->b_پچھلا;

		پچھلا->b_اگلا = buf->b_اگلا;
		اگر (پچھلا->b_اگلا != تہی)
		    پچھلا->b_اگلا->b_پچھلا = پچھلا;
		buf->b_اگلا = پچھلا;
		buf->b_پچھلا = پچھلا->b_پچھلا;
		اگر (buf->b_پچھلا != تہی)
		    buf->b_پچھلا->b_اگلا = buf;
		پچھلا->b_پچھلا = buf;
		اگر (lastbuf == buf)
		    lastbuf = پچھلا;
		اگر (firstbuf == پچھلا)
		    firstbuf = buf;
	    }
	}
	ورنہ
	    buf->b_fnum = چوٹی_file_عدد++;
	اگر (چوٹی_file_عدد < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    اگر (emsg_silent == 0 && !اندر_assert_fails)
	    {
		برآمد_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    چوٹی_file_عدد = 1;
	}
	buf_hashtab_جوڑو(buf);

	// Always copy the options from the current buffer.
	buf_copy_options(buf, BCO_ALWAYS);
    }

    buf->b_wininfo->wi_fpos.lnum = lnum;
    buf->b_wininfo->wi_win = curwin;

#اگر_تعریف_شدہ FEAT_SYN_HL
    hash_init(&buf->b_s.b_keywtab);
    hash_init(&buf->b_s.b_keywtab_ic);
#اگر_ختم

    buf->b_fname = buf->b_sfname;
#اگر_تعریف_شدہ UNIX
    اگر (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ورنہ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#اگر_ختم
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    اگر (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_نام(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    اگر (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	سیٹ_bufref(&bufref, buf);
	اگر (apply_autocmds(EVENT_BUFNEW, تہی, تہی, FALSE, buf)
		&& !bufref_valid(&bufref))
	    واپس تہی;
	اگر (flags & BLN_LISTED)
	{
	    اگر (apply_autocmds(EVENT_BUFADD, تہی, تہی, FALSE, buf)
		    && !bufref_valid(&bufref))
		واپس تہی;
	}
#اگر_تعریف_شدہ FEAT_EVAL
	اگر (aborting())		// autocmds may abort script processing
	    واپس تہی;
#اگر_ختم
    }

    واپس buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    خالی
free_buf_options(
    buf_T	*buf,
    صحیح		free_p_ff)
{
    اگر (free_p_ff)
    {
	clear_string_option(&buf->b_p_fenc);
	clear_string_option(&buf->b_p_ff);
	clear_string_option(&buf->b_p_bh);
	clear_string_option(&buf->b_p_bt);
    }
#اگر_تعریف_شدہ FEAT_ڈھونڈو_ID
    clear_string_option(&buf->b_p_def);
    clear_string_option(&buf->b_p_inc);
# ifdef FEAT_EVAL
    clear_string_option(&buf->b_p_inex);
# endif
#اگر_ختم
#اگر defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#اگر_ختم
#اگر defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#اگر_ختم
#اگر defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#اگر_ختم
    clear_string_option(&buf->b_p_fp);
#اگر defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CRYPT
# ifdef FEAT_SODIUM
    اگر (buf->b_p_کلید != تہی && *buf->b_p_کلید != NUL
			   && crypt_طریقہ_is_sodium(crypt_get_طریقہ_nr(buf)))
	crypt_sodium_munlock(buf->b_p_کلید, STRLEN(buf->b_p_کلید));
# endif
    clear_string_option(&buf->b_p_کلید);
#اگر_ختم
    clear_string_option(&buf->b_p_kp);
    clear_string_option(&buf->b_p_mps);
    clear_string_option(&buf->b_p_fo);
    clear_string_option(&buf->b_p_flp);
    clear_string_option(&buf->b_p_isk);
#اگر_تعریف_شدہ FEAT_VARTABS
    clear_string_option(&buf->b_p_vsts);
    VIM_CLEAR(buf->b_p_vsts_nopaste);
    VIM_CLEAR(buf->b_p_vsts_array);
    clear_string_option(&buf->b_p_vts);
    VIM_CLEAR(buf->b_p_vts_array);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_KEYMAP
    clear_string_option(&buf->b_p_keymap);
    keymap_clear(&buf->b_kmap_ga);
    ga_clear(&buf->b_kmap_ga);
#اگر_ختم
    clear_string_option(&buf->b_p_com);
    clear_string_option(&buf->b_p_cms);
    clear_string_option(&buf->b_p_nf);
#اگر_تعریف_شدہ FEAT_SYN_HL
    clear_string_option(&buf->b_p_syn);
    clear_string_option(&buf->b_s.b_syn_isk);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_SPELL
    clear_string_option(&buf->b_s.b_p_spc);
    clear_string_option(&buf->b_s.b_p_spf);
    vim_regfree(buf->b_s.b_cap_prog);
    buf->b_s.b_cap_prog = تہی;
    clear_string_option(&buf->b_s.b_p_spl);
    clear_string_option(&buf->b_s.b_p_spo);
#اگر_ختم
    clear_string_option(&buf->b_p_sua);
    clear_string_option(&buf->b_p_ft);
    clear_string_option(&buf->b_p_cink);
    clear_string_option(&buf->b_p_cino);
    clear_string_option(&buf->b_p_lop);
    clear_string_option(&buf->b_p_cinsd);
    clear_string_option(&buf->b_p_cinw);
    clear_string_option(&buf->b_p_cot);
    clear_string_option(&buf->b_p_cpt);
#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
    clear_string_option(&buf->b_p_csl);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_COMPL_FUNC
    clear_string_option(&buf->b_p_cfu);
    free_callback(&buf->b_cfu_cb);
    clear_string_option(&buf->b_p_ofu);
    free_callback(&buf->b_ofu_cb);
    clear_string_option(&buf->b_p_tsrfu);
    free_callback(&buf->b_tsrfu_cb);
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_گنتی);
    buf->b_p_cpt_گنتی = 0;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_QUICKFIX
    clear_string_option(&buf->b_p_gefm);
    clear_string_option(&buf->b_p_gp);
    clear_string_option(&buf->b_p_mp);
    clear_string_option(&buf->b_p_efm);
#اگر_ختم
    clear_string_option(&buf->b_p_ep);
    clear_string_option(&buf->b_p_راستہ);
    clear_string_option(&buf->b_p_لیبل_فہرست);
    clear_string_option(&buf->b_p_tc);
#اگر_تعریف_شدہ FEAT_EVAL
    clear_string_option(&buf->b_p_tfu);
    free_callback(&buf->b_tfu_cb);
    clear_string_option(&buf->b_p_ffu);
    free_callback(&buf->b_ffu_cb);
#اگر_ختم
    clear_string_option(&buf->b_p_dict);
#اگر_تعریف_شدہ FEAT_DIFF
    clear_string_option(&buf->b_p_dia);
#اگر_ختم
    clear_string_option(&buf->b_p_tsr);
    clear_string_option(&buf->b_p_qe);
    buf->b_p_ac = -1;
    buf->b_p_ar = -1;
#اگر_تعریف_شدہ HAVE_FSYNC
    buf->b_p_fs = -1;
#اگر_ختم
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
    صحیح
buflist_getfile(
    صحیح		n,
    linenr_T	lnum,
    صحیح		options,
    صحیح		forceit)
{
    buf_T	*buf;
    win_T	*wp = تہی;
    pos_T	*fpos;
    colnr_T	ستون;

    buf = buflist_findnr(n);
    اگر (buf == تہی)
    {
	اگر ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	ورنہ
	    semsg(_(e_بفر_nr_not_ملا), n);
	واپس FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    اگر (buf == curbuf)
	واپس OK;

    اگر (متن_or_buf_locked())
	واپس FAIL;

    // altfpos may be changed by getfile(), get it now
    اگر (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	ستون = fpos->ستون;
    }
    ورنہ
	ستون = 0;

    اگر (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	اگر (wp == تہی && (swb_flags & (SWB_VSPLIT | SWB_بانٹو | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    اگر (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    ورنہ اگر (win_بانٹو(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		واپس FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    صحیح retval = FAIL;
    اگر (GETFILE_SUCCESS(getfile(buf->b_fnum, تہی, تہی,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	اگر (!p_sol && ستون != 0)
	{
	    curwin->w_cursor.ستون = ستون;
	    check_cursor_ستون();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_سیٹ_curswant = true;
	}
	retval = OK;
    }

    اگر (RedrawingDisabled > 0)
	--RedrawingDisabled;
    واپس retval;
}

/*
 * go to the last know line number for the current buffer
 */
    ساکن خالی
buflist_getfpos(خالی)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    اگر (p_sol)
	curwin->w_cursor.ستون = 0;
    ورنہ
    {
	curwin->w_cursor.ستون = fpos->ستون;
	check_cursor_ستون();
	curwin->w_cursor.coladd = 0;
	curwin->w_سیٹ_curswant = true;
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
    buf_T	*buf = تہی;

    // First make the name into a full path name
    ffname = بھرا_Name_save(fname,
#اگر_تعریف_شدہ UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#ورنہ
	    FALSE
#اگر_ختم
	    );
    اگر (ffname != تہی)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    واپس buf;
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
#اگر_تعریف_شدہ UNIX
    stat_T	st;

    اگر (mch_stat((حرف *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    واپس buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    ساکن buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#اگر_ختم
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_آخری(buf)
	اگر ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#اگر_تعریف_شدہ UNIX
		    , stp
#اگر_ختم
		    ))
	    واپس buf;
    واپس تہی;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    صحیح
buflist_findpat(
    char_u	*نمونہ,
    char_u	*نمونہ_انتہا,	// pointer to first char after pattern
    صحیح		unlisted,	// find unlisted buffers
    صحیح		diffmode غیر_مستعمل, // find diff-mode buffers only
    صحیح		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    صحیح		مطابقت = -1;
    صحیح		ڈھونڈو_listed;
    char_u	*pat;
    char_u	*patend;
    صحیح		attempt;
    char_u	*p;
    صحیح		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    اگر ((نمونہ_انتہا == نمونہ + 1 && (*نمونہ == '%' || *نمونہ == '#'))
	    || (اندر_vim9script() && نمونہ_انتہا == نمونہ + 2
				    && نمونہ[0] == '%' && نمونہ[1] == '%'))
    {
	اگر (*نمونہ == '#' || نمونہ_انتہا == نمونہ + 2)
	    مطابقت = curwin->w_alt_fnum;
	ورنہ
	    مطابقت = curbuf->b_fnum;
#اگر_تعریف_شدہ FEAT_DIFF
	اگر (diffmode && !diff_منوال_buf(buflist_findnr(مطابقت)))
	    مطابقت = -1;
#اگر_ختم
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
    ورنہ
    {
	pat = file_pat_مقصد_reg_pat(نمونہ, نمونہ_انتہا, تہی, FALSE);
	اگر (pat == تہی)
	    واپس -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	ڈھونڈو_listed = TRUE;
	برائے (;;)
	{
	    برائے (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		اگر (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		اگر (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_آخری(buf)
		{
		    اگر (regmatch.regprog == تہی)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			واپس -1;
		    }
		    اگر (buf->b_p_bl == ڈھونڈو_listed
#اگر_تعریف_شدہ FEAT_DIFF
			    && (!diffmode || diff_منوال_buf(buf))
#اگر_ختم
			    && buflist_مطابقت(&regmatch, buf, FALSE) != تہی)
		    {
			اگر (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				اگر (wp->w_بفر == buf)
				    توڑو;
			    اگر (wp == تہی)
				جاری;
			}
			اگر (مطابقت >= 0)		// already found a match
			{
			    مطابقت = -2;
			    توڑو;
			}
			مطابقت = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		اگر (مطابقت >= 0)			// found one match
		    توڑو;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    اگر (!unlisted || !ڈھونڈو_listed || مطابقت != -1)
		توڑو;
	    ڈھونڈو_listed = FALSE;
	}

	vim_free(pat);
    }

    اگر (مطابقت == -2)
	semsg(_(e_مزید_than_one_مطابقت_for_str), نمونہ);
    ورنہ اگر (مطابقت < 0)
	semsg(_(e_no_matching_بفر_for_str), نمونہ);
    واپس مطابقت;
}

#اگر_تعریف_شدہ FEAT_VIMINFO
قسمتعریف ساخت {
    buf_T   *buf;
    char_u  *مطابقت;
} bufmatch_T;
#اگر_ختم

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    صحیح
ExpandBufnames(
    char_u	*pat,
    صحیح		*عدد_file,
    char_u	***file,
    صحیح		options)
{
    صحیح		گنتی;
    buf_T	*buf;
    صحیح		باری;
    char_u	*p;
    char_u	*patc = تہی;
#اگر_تعریف_شدہ FEAT_VIMINFO
    bufmatch_T	*matches = تہی;
#اگر_ختم
    صحیح		fuzzy;
    fuzmatch_str_T  *fuzmatch = تہی;
    regmatch_T	regmatch;
    صحیح		score = 0;
    صحیح		مقصد_free = FALSE;

    *عدد_file = 0;		    // return values in case of FAIL
    *file = تہی;

#اگر_تعریف_شدہ FEAT_DIFF
    اگر ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	واپس FAIL;
#اگر_ختم

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    اگر (!fuzzy)
    {
	اگر (*pat == '^' && pat[1] != NUL)
	{
	    صحیح  len = (صحیح)STRLEN(pat);
	    patc = alloc(len);
	    اگر (patc == تہی)
		واپس FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    مقصد_free = TRUE;
	}
	ورنہ اگر (*pat == '^')
	    patc = (char_u *)"";
	ورنہ
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    برائے (باری = 1; باری <= 2; ++باری)
    {
	گنتی = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    اگر (!buf->b_p_bl)	// skip unlisted buffers
		جاری;
#اگر_تعریف_شدہ FEAT_DIFF
	    اگر (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		اگر (buf == curbuf || !diff_منوال_buf(buf))
		    جاری;
#اگر_ختم

	    اگر (!fuzzy)
	    {
		اگر (regmatch.regprog == تہی)
		{
		    // invalid pattern, possibly after recompiling
		    اگر (مقصد_free)
			vim_free(patc);
		    واپس FAIL;
		}
		p = buflist_مطابقت(&regmatch, buf, p_wic);
	    }
	    ورنہ
	    {
		p = تہی;
		// first try matching with the short file name
		اگر ((score = fuzzy_مطابقت_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		اگر (p == تہی)
		{
		    // next try matching with the full path file name
		    اگر ((score = fuzzy_مطابقت_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    اگر (p == تہی)
		جاری;

	    اگر (باری == 1)
	    {
		++گنتی;
		جاری;
	    }

	    اگر (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    ورنہ
		p = vim_strsave(p);
	    اگر (p == تہی)
	    {
		اگر (fuzzy && باری == 2)
		    fuzmatch_str_free(fuzmatch, گنتی);
		واپس FAIL;
	    }

	    اگر (!fuzzy)
	    {
#اگر_تعریف_شدہ FEAT_VIMINFO
		اگر (matches != تہی)
		{
		    matches[گنتی].buf = buf;
		    matches[گنتی].مطابقت = p;
		    گنتی++;
		}
		ورنہ
#اگر_ختم
		    (*file)[گنتی++] = p;
	    }
	    ورنہ
	    {
		fuzmatch[گنتی].idx = گنتی;
		fuzmatch[گنتی].str = p;
		fuzmatch[گنتی].score = score;
		گنتی++;
	    }
	}
	اگر (گنتی == 0)	// no match found, break here
	    توڑو;
	اگر (باری == 1)
	{
	    اگر (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, گنتی);
		اگر (*file == تہی)
		{
		    vim_regfree(regmatch.regprog);
		    اگر (مقصد_free)
			vim_free(patc);
		    واپس FAIL;
		}
#اگر_تعریف_شدہ FEAT_VIMINFO
		اگر (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, گنتی);
#اگر_ختم
	    }
	    ورنہ
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, گنتی);
		اگر (fuzmatch == تہی)
		{
		    *عدد_file = 0;
		    *file = تہی;
		    واپس FAIL;
		}
	    }
	}
    }

    اگر (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	اگر (مقصد_free)
	    vim_free(patc);
    }

    اگر (!fuzzy)
    {
#اگر_تعریف_شدہ FEAT_VIMINFO
	اگر (matches != تہی)
	{
	    صحیح i;
	    اگر (گنتی > 1)
		qsort(matches, گنتی, حجم(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    اگر (matches[0].buf == curbuf)
	    {
		برائے (i = 1; i < گنتی; i++)
		    (*file)[i-1] = matches[i].مطابقت;
		(*file)[گنتی-1] = matches[0].مطابقت;
	    }
	    ورنہ
	    {
		برائے (i = 0; i < گنتی; i++)
		    (*file)[i] = matches[i].مطابقت;
	    }
	    vim_free(matches);
	}
#اگر_ختم
    }
    ورنہ
    {
	اگر (fuzzymatches_مقصد_strmatches(fuzmatch, file, گنتی, FALSE) == FAIL)
	    واپس FAIL;
    }

    *عدد_file = گنتی;
    واپس (گنتی == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    ساکن char_u *
buflist_مطابقت(
    regmatch_T	*rmp,
    buf_T	*buf,
    صحیح		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*مطابقت;

    // First try the short file name, then the long file name.
    مطابقت = fname_مطابقت(rmp, buf->b_sfname, ignore_case);
    اگر (مطابقت == تہی && rmp->regprog != تہی)
	مطابقت = fname_مطابقت(rmp, buf->b_ffname, ignore_case);

    واپس مطابقت;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    ساکن char_u *
fname_مطابقت(
    regmatch_T	*rmp,
    char_u	*name,
    صحیح		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*مطابقت = تہی;
    char_u	*p;

    // extra check for valid arguments
    اگر (name == تہی || rmp->regprog == تہی)
	واپس تہی;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    اگر (vim_regexec(rmp, name, (colnr_T)0))
	مطابقت = name;
    ورنہ اگر (rmp->regprog != تہی)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(تہی, name);
	اگر (p != تہی && vim_regexec(rmp, p, (colnr_T)0))
	    مطابقت = name;
	vim_free(p);
    }

    واپس مطابقت;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(صحیح nr)
{
    char_u	کلید[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    اگر (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((حرف *)کلید, حجم(کلید), "%x", nr);
    hi = hash_ڈھونڈو(&buf_hashtab, کلید);

    اگر (!HASHITEM_خالی_خانے(hi))
	واپس (buf_T *)(hi->hi_کلید
			     - ((غیرعلامتی)(curbuf->b_کلید - (char_u *)curbuf)));
    واپس تہی;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    صحیح		n,
    صحیح		fullname,
    صحیح		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    اگر (buf == تہی)
	واپس تہی;
    واپس home_replace_save(helptail ? buf : تہی,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    خالی
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	ستون,
    صحیح		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	اگر (wip->wi_win == win)
	    توڑو;
    اگر (wip == تہی)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	اگر (wip == تہی)
	    واپس;
	wip->wi_win = win;
	اگر (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    ورنہ
    {
	// remove the entry from the list
	اگر (wip->wi_پچھلا)
	    wip->wi_پچھلا->wi_اگلا = wip->wi_اگلا;
	ورنہ
	    buf->b_wininfo = wip->wi_اگلا;
	اگر (wip->wi_اگلا)
	    wip->wi_اگلا->wi_پچھلا = wip->wi_پچھلا;
	اگر (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#اگر_تعریف_شدہ FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#اگر_ختم
	}
    }
    اگر (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.ستون = ستون;
    }
    اگر (win != تہی)
	wip->wi_changelistidx = win->w_changelistidx;
    اگر (copy_options && win != تہی)
    {
	// Save the window-specific option values.
	copy_winopt(&win->w_onebuf_opt, &wip->wi_opt);
#اگر_تعریف_شدہ FEAT_FOLDING
	wip->wi_fold_manual = win->w_fold_manual;
	cloneFoldGrowArray(&win->w_folds, &wip->wi_folds);
#اگر_ختم
	wip->wi_optset = TRUE;
    }

    // insert the entry in front of the list
    wip->wi_اگلا = buf->b_wininfo;
    buf->b_wininfo = wip;
    wip->wi_پچھلا = تہی;
    اگر (wip->wi_اگلا)
	wip->wi_اگلا->wi_پچھلا = wip;
}

#اگر_تعریف_شدہ FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    ساکن صحیح
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    اگر (!wip->wi_opt.wo_diff)
	واپس FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	اگر (wip->wi_win == wp)
	    واپس FALSE;
    واپس TRUE;
}
#اگر_ختم

/*
 * Find info for the current window in buffer "buf".
 * If not found, return the info for the most recently used window.
 * When "need_options" is TRUE skip entries where wi_optset is FALSE.
 * When "skip_diff_buffer" is TRUE avoid windows with 'diff' set that is in
 * another tab page.
 * Returns NULL when there isn't any info.
 */
    ساکن wininfo_T *
ڈھونڈو_wininfo(
    buf_T	*buf,
    صحیح		need_options,
    صحیح		skip_diff_بفر غیر_مستعمل)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	اگر (wip->wi_win == curwin
#اگر_تعریف_شدہ FEAT_DIFF
		&& (!skip_diff_بفر || !wininfo_other_tab_diff(wip))
#اگر_ختم

		&& (!need_options || wip->wi_optset))
	    توڑو;

    اگر (wip != تہی)
	واپس wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#اگر_تعریف_شدہ FEAT_DIFF
    اگر (skip_diff_بفر)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    اگر (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != تہی
			    && wip->wi_win->w_بفر == buf)))
		توڑو;
    }
    ورنہ
#اگر_ختم
	wip = buf->b_wininfo;
    واپس wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    خالی
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#اگر_تعریف_شدہ FEAT_FOLDING
    clearFolding(curwin);
#اگر_ختم

    wip = ڈھونڈو_wininfo(buf, TRUE, TRUE);
    اگر (wip != تہی && wip->wi_win != تہی
	    && wip->wi_win != curwin && wip->wi_win->w_بفر == buf)
    {
	// The buffer is currently displayed in the window: use the actual
	// option values instead of the saved (possibly outdated) values.
	win_T *wp = wip->wi_win;

	copy_winopt(&wp->w_onebuf_opt, &curwin->w_onebuf_opt);
#اگر_تعریف_شدہ FEAT_STL_OPT
	// w_stl_rendered_height is not in winvar_T; copy it explicitly.
	curwin->w_stl_rendered_اونچائی = wp->w_stl_rendered_اونچائی;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_FOLDING
	curwin->w_fold_manual = wp->w_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wp->w_folds, &curwin->w_folds);
#اگر_ختم
    }
    ورنہ اگر (wip != تہی && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#اگر_تعریف_شدہ FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#اگر_ختم
    }
    ورنہ
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    اگر (wip != تہی)
	curwin->w_changelistidx = wip->wi_changelistidx;

#اگر_تعریف_شدہ FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    اگر (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    اگر (*curwin->w_p_stl != NUL)
	update_stl_rendered_اونچائی(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    اگر (curwin->w_حالت_اونچائی > 0
	    && curwin->w_حالت_اونچائی != statusline_اونچائی(curwin))
    {
	چوکھٹا_change_statusline_اونچائی();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	اگر (p_ea)
	    win_equal(curwin, TRUE, 'v');
    }
#اگر_ختم
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
    ساکن pos_T no_position = {1, 0, 0};

    wip = ڈھونڈو_wininfo(buf, FALSE, FALSE);
    اگر (wip != تہی)
	واپس &(wip->wi_fpos);
    ورنہ
	واپس &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    واپس buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    خالی
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    صحیح		len;
    صحیح		i;
    صحیح		ro_char;
    صحیح		بدلا_char;
#اگر_تعریف_شدہ FEAT_TERMINAL
    صحیح		کام_running;
    صحیح		کام_none_open;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_ڈیٹا = تہی, **p;

    اگر (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, حجم(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    اگر (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_ڈیٹا)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_ڈیٹا, (size_t)buflist.ga_len,
		حجم(buf_T *), buf_compare);

	buflist_ڈیٹا = (buf_T **)buflist.ga_ڈیٹا;
	buf = *buflist_ڈیٹا;
    }
    p = buflist_ڈیٹا;

    برائے (; buf != تہی && !got_int; buf = buflist_ڈیٹا != تہی
	    ? (++p < buflist_ڈیٹا + buflist.ga_len ? *p : تہی)
	    : buf->b_اگلا)
#ورنہ
    برائے (buf = firstbuf; buf != تہی && !got_int; buf = buf->b_اگلا)
#اگر_ختم
    {
	char_u	*name;

#اگر_تعریف_شدہ FEAT_TERMINAL
	کام_running = رکن_کام_running(buf->b_رکن);
	کام_none_open = رکن_none_open(buf->b_رکن);
#اگر_ختم
	// skip unlisted buffers, unless ! was used
	اگر ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !buf_Is_بدلا(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == تہی || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == تہی || buf->b_nwindows != 0))
#اگر_تعریف_شدہ FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!کام_running || (کام_running && کام_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!کام_running || (کام_running && !کام_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (کام_running || buf->b_رکن == تہی))
#اگر_ختم
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    جاری;
	name = buf_spname(buf);
	اگر (name != تہی)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	ورنہ
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	اگر (پیغام_filtered(NameBuff))
	    جاری;

	بدلا_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (buf_Is_بدلا(buf) ? '+' : ' ');
#اگر_تعریف_شدہ FEAT_TERMINAL
	اگر (کام_running)
	{
	    اگر (کام_none_open)
		ro_char = '?';
	    ورنہ
		ro_char = 'R';
	    بدلا_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	ورنہ اگر (buf->b_رکن != تہی)
	    ro_char = 'F';
	ورنہ
#اگر_ختم
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	پیغام_putchar('\n');
	len = (صحیح)vim_snprintf_safelen((حرف *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == تہی ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		بدلا_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	کرو
	    IObuff[len++] = ' ';
	جب (--i > 0 && len < IOSIZE - 18);
#اگر_تعریف_شدہ FEAT_VIMINFO
	اگر (vim_strchr(eap->arg, 't') && buf->b_آخری_مستعمل)
	    جوڑو_وقت(IObuff + len, (size_t)(IOSIZE - len), buf->b_آخری_مستعمل);
	ورنہ
#اگر_ختم
	    vim_snprintf((حرف *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (طویل)buflist_findlnum(buf));
	پیغام_outtrans(IObuff);
	برآمد_flush();	    // output one line at a time
	ui_breakcheck();
    }

#اگر_تعریف_شدہ FEAT_VIMINFO
    اگر (buflist_ڈیٹا)
	ga_clear(&buflist);
#اگر_ختم
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    صحیح
buflist_name_nr(
    صحیح		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    اگر (buf == تہی || buf->b_fname == تہی)
	واپس FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    واپس OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    صحیح
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    صحیح		پیغام_2)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = تہی;
#اگر_تعریف_شدہ UNIX
    stat_T	st;
#اگر_ختم

    اگر (ffname == تہی || *ffname == NUL)
    {
	// Removing the name.
	اگر (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ورنہ
	    buf->b_sfname = تہی;
	VIM_CLEAR(buf->b_ffname);
#اگر_تعریف_شدہ UNIX
	st.st_dev = (dev_T)-1;
#اگر_ختم
    }
    ورنہ
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	اگر (ffname == تہی)		    // out of memory
	    واپس FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#اگر_تعریف_شدہ UNIX
	اگر (mch_stat((حرف *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#اگر_ختم
	اگر (!(buf->b_flags & BF_DUMMY))
#اگر_تعریف_شدہ UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#ورنہ
	    obuf = buflist_findname(ffname);
#اگر_ختم
	اگر (obuf != تہی && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    صحیح		اندر_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		اگر (win->w_بفر == obuf)
		    اندر_use = TRUE;

	    // it's loaded or used in a window, fail
	    اگر (obuf->b_ml.ml_mfp != تہی || اندر_use)
	    {
		اگر (پیغام_2)
		    emsg(_(e_بفر_with_this_name_already_exists));
		vim_free(ffname);
		واپس FAIL;
	    }
	    // delete from the list
	    close_بفر(تہی, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	اگر (ffname == تہی || sfname == تہی)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    واپس FAIL;
	}
#اگر_تعریف_شدہ USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#اگر_ختم
	اگر (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#اگر_تعریف_شدہ UNIX
    اگر (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ورنہ
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#اگر_ختم

    buf->b_shortname = false;

    buf_name_بدلا(buf);
    واپس OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    خالی
buf_سیٹ_name(صحیح fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    اگر (buf == تہی)
	واپس;

    اگر (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = تہی;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    خالی
buf_name_بدلا(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    اگر (buf->b_ml.ml_mfp != تہی)
	ml_setname(buf);

#اگر_تعریف_شدہ FEAT_TERMINAL
    اگر (buf->b_رکن != تہی)
	رکن_clear_حالت_متن(buf->b_رکن);
#اگر_ختم

    اگر (curwin->w_بفر == buf)
	check_arg_idx(curwin);	// check file name for arg list
    maketitle();		// set window title
    حالت_redraw_all();	// status lines need to be redrawn
    fmarks_check_نام(buf);	// check named file marks
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
    اگر (buf != تہی && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    واپس buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    صحیح		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    اگر (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	اگر (errmsg)
	    emsg(_(e_no_alternate_file));
	واپس تہی;
    }
    واپس fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    صحیح
buflist_جوڑو(char_u *fname, صحیح flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, تہی, (linenr_T)0, flags);
    اگر (buf != تہی)
	واپس buf->b_fnum;
    واپس 0;
}

#اگر defined(BACKSLASH_اندر_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    خالی
buflist_slash_adjust(خالی)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	اگر (bp->b_ffname != تہی)
	    slash_adjust(bp->b_ffname);
	اگر (bp->b_sfname != تہی)
	    slash_adjust(bp->b_sfname);
    }
}
#اگر_ختم

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    خالی
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.ستون, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    صحیح
otherfile(char_u *ffname)
{
    واپس otherfile_buf(curbuf, ffname
#اگر_تعریف_شدہ UNIX
	    , تہی
#اگر_ختم
	    );
}

    ساکن صحیح
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#اگر_تعریف_شدہ UNIX
    , stat_T		*stp
#اگر_ختم
    )
{
    // no name is different
    اگر (ffname == تہی || *ffname == NUL || buf->b_ffname == تہی)
	واپس TRUE;
    اگر (fnamecmp(ffname, buf->b_ffname) == 0)
	واپس FALSE;
#اگر_تعریف_شدہ UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	اگر (stp == تہی)
	{
	    اگر (!buf->b_dev_valid || mch_stat((حرف *)ffname, &st) < 0)
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
	اگر (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    اگر (buf_same_ino(buf, stp))
		واپس FALSE;
	}
    }
#اگر_ختم
    واپس TRUE;
}

#اگر defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    خالی
buf_setino(buf_T *buf)
{
    stat_T	st;

    اگر (buf->b_fname != تہی && mch_stat((حرف *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    ورنہ
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    ساکن صحیح
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    واپس (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#اگر_ختم

/*
 * Print info about the current buffer.
 */
    خالی
fileinfo(
    صحیح fullname,	    // when non-zero print full path
    صحیح shorthelp,
    صحیح	dont_truncate)
{
    char_u	*name;
    حرف	*بفر;
    size_t	bufferlen = 0;

    بفر = alloc(IOSIZE);
    اگر (بفر == تہی)
	واپس;

    اگر (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(بفر, IOSIZE, "buf %d: ", curbuf->b_fnum);

    بفر[bufferlen++] = '"';

    name = buf_spname(curbuf);
    اگر (name != تہی)
	bufferlen += vim_snprintf_safelen(بفر + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    ورنہ
    {
	اگر (!fullname && curbuf->b_fname != تہی)
	    name = curbuf->b_fname;
	ورنہ
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : تہی, name,
	    (char_u *)بفر + bufferlen, IOSIZE - (صحیح)bufferlen, TRUE);
    }

    bufferlen += vim_snprintf_safelen(
	بفر + bufferlen,
	IOSIZE - bufferlen,
	"\"%s%s%s%s%s%s",
	curbuf_Is_بدلا() ? (shortmess(SHM_MOD)
	    ?  " [+]" : _(" [Modified]")) : " ",
	(curbuf->b_flags & BF_NOTEDITED) && !bt_dontwrite(curbuf)
	    ? _("[Not edited]") : "",
	(curbuf->b_flags & BF_NEW) && !bt_dontwrite(curbuf)
	    ? new_file_پیغام() : "",
	(curbuf->b_flags & BF_READERR) ? _("[Read errors]") : "", curbuf->b_p_ro
	    ? (shortmess(SHM_RO) ? _("[RO]") : _("[readonly]")) : "",
	(curbuf_Is_بدلا() || (curbuf->b_flags & BF_WRITE_نقاب) || curbuf->b_p_ro)
	    ? " " : "");

    اگر (curbuf->b_ml.ml_flags & ML_خالی_خانے)
	bufferlen += vim_snprintf_safelen(بفر + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_سطریں_پیغام));
    ورنہ اگر (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    بفر + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_سطر_گنتی),
	    (طویل)curbuf->b_ml.ml_سطر_گنتی,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_سطر_گنتی));
    ورنہ
    {
	bufferlen += vim_snprintf_safelen(
	    بفر + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (طویل)curwin->w_cursor.lnum,
	    (طویل)curbuf->b_ml.ml_سطر_گنتی,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_سطر_گنتی));

	validate_virtcol();
	bufferlen += ستون_print((char_u *)بفر + bufferlen, IOSIZE - bufferlen,
		   (صحیح)curwin->w_cursor.ستون + 1, (صحیح)curwin->w_virtcol + 1);
    }

    (خالی)append_arg_نمبر(curwin, (char_u *)بفر + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    اگر (dont_truncate)
    {
	صحیح	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	پیغام_آغاز();
	n = پیغام_scroll;
	پیغام_scroll = TRUE;
	پیغام(بفر);
	پیغام_scroll = n;
    }
    ورنہ
    {
	حرف	*p = پیغام_trunc_attr(بفر, FALSE, 0);
	اگر (restart_edit != 0 || (پیغام_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    سیٹ_keep_پیغام((char_u *)p, 0);
    }

    vim_free(بفر);
}

    صحیح
ستون_print(
    char_u  *buf,
    size_t  buflen,
    صحیح	    ستون,
    صحیح	    vcol)
{
    اگر (ستون == vcol)
	واپس (صحیح)vim_snprintf_safelen((حرف *)buf, buflen, "%d", ستون);

    واپس (صحیح)vim_snprintf_safelen((حرف *)buf, buflen, "%d-%d", ستون, vcol);
}

ساکن char_u *lasttitle = تہی;
ساکن char_u *lasticon = تہی;

/*
 * Put the file name in the title bar and icon of the window.
 */
    خالی
maketitle(خالی)
{
    char_u	*title_str = تہی;
    char_u	*icon_str = تہی;
    صحیح		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    اگر (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	واپس;
    }

    need_maketitle = FALSE;
    اگر (!p_title && !p_icon && lasttitle == تہی && lasticon == تہی)
	واپس;  // nothing to do

    اگر (p_title)
    {
	صحیح maxlen = 0;

	اگر (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    اگر (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	اگر (*p_titlestring != NUL)
	{
#اگر_تعریف_شدہ FEAT_STL_OPT
	    اگر (stl_syntax & STL_اندر_TITLE)
		build_stl_str_hl(curwin, title_str, حجم(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, تہی, تہی, تہی);
	    ورنہ
#اگر_ختم
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	ورنہ
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#تعریف_کرو SPACE_FOR_FNAME (حجم(buf) - 100)
#تعریف_کرو SPACE_FOR_DIR   (حجم(buf) - 20)
#تعریف_کرو SPACE_FOR_ARGNR (حجم(buf) - 10)  // at least room for " - VIM"

	    // file name
	    اگر (curbuf->b_fname == تہی)
		buflen = vim_snprintf_safelen((حرف *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#اگر_تعریف_شدہ FEAT_TERMINAL
	    ورنہ اگر (curbuf->b_رکن != تہی)
		buflen = vim_snprintf_safelen((حرف *)buf,
		    SPACE_FOR_FNAME, "%s",
		    رکن_get_حالت_متن(curbuf->b_رکن));
#اگر_ختم
	    ورنہ
	    {
		buflen = vim_snprintf_safelen((حرف *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != تہی)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#اگر_تعریف_شدہ FEAT_TERMINAL
	    اگر (curbuf->b_رکن == تہی)
#اگر_ختم
	    {
		انتخاب (buf_Is_بدلا(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    حالت 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (حرف *)buf + buflen,
			    حجم(buf) - buflen, " +");
			توڑو;
		    حالت 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (حرف *)buf + buflen,
			    حجم(buf) - buflen, " =");
			توڑو;
		    حالت 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (حرف *)buf + buflen,
			    حجم(buf) - buflen, " =+");
			توڑو;
		    حالت 4:
		    حالت 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (حرف *)buf + buflen,
			    حجم(buf) - buflen, " -");
			توڑو;
		    حالت 5:
		    حالت 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (حرف *)buf + buflen,
			    حجم(buf) - buflen, " -+");
			توڑو;
		    طےشدہ:
			توڑو;
		}
	    }

	    // path (surrounded by '()')
	    اگر (curbuf->b_fname != تہی
#اگر_تعریف_شدہ FEAT_TERMINAL
		    && curbuf->b_رکن == تہی
#اگر_ختم
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((حرف *)buf + buflen,
		    حجم(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (صحیح)(SPACE_FOR_DIR - buflen), TRUE);

#اگر_تعریف_شدہ BACKSLASH_اندر_FILENAME
		// avoid "c:/name" to be reduced to "c"
		اگر (محفوظ_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#اگر_ختم

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		اگر (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((حرف *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		ورنہ
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    اگر (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((حرف *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != تہی)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    ورنہ
			buflen += vim_snprintf_safelen((حرف *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_نمبر(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((حرف *)buf + buflen,
		حجم(buf) - buflen, " - %s",
#اگر defined(FEAT_CLIENTSERVER)
		(serverName != تہی)
		    ? serverName :
#اگر_ختم
		    (char_u *)"VIM");

	    اگر (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		اگر (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, حجم(buf));
	    }
	}
    }
    mustset = قدر_بدلا(title_str, &lasttitle);

    اگر (p_icon)
    {
	icon_str = buf;
	اگر (*p_iconstring != NUL)
	{
#اگر_تعریف_شدہ FEAT_STL_OPT
	    اگر (stl_syntax & STL_اندر_ICON)
		build_stl_str_hl(curwin, icon_str, حجم(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, تہی, تہی,
				 تہی);
	    ورنہ
#اگر_ختم
		icon_str = p_iconstring;
	}
	ورنہ
	{
	    char_u  *name;
	    صحیح	    namelen;

	    name = buf_spname(curbuf);
	    اگر (name == تہی)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (صحیح)STRLEN(name);
	    اگر (namelen > 100)
	    {
		namelen -= 100;
		اگر (has_mbyte)
		    namelen += (*mb_دم_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, حجم(buf));
	}
    }

    mustset |= قدر_بدلا(icon_str, &lasticon);

    اگر (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    ساکن صحیح
قدر_بدلا(char_u *str, char_u **آخری)
{
    اگر ((str == تہی) != (*آخری == تہی)
	    || (str != تہی && *آخری != تہی && STRCMP(str, *آخری) != 0))
    {
	vim_free(*آخری);
	اگر (str == تہی)
	{
	    *آخری = تہی;
	    mch_restore_title(
		  آخری == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	ورنہ
	{
	    *آخری = vim_strsave(str);
	    واپس TRUE;
	}
    }
    واپس FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    خالی
resettitle(خالی)
{
    mch_settitle(lasttitle, lasticon);
}

#اگر defined(EXITFREE)
    خالی
free_titles(خالی)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#اگر_ختم


#اگر defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
قسمتعریف ساخت
{
    char_u	*stl_آغاز;
    صحیح		stl_minwid;
    صحیح		stl_maxwid;
    شمار {
	معمول,
	خالی_خانے,
	گروہ_2,
	Separate,
	Highlight,
	Tab_صفحہ,
	ClickFunc,
	Trunc
    }		stl_type;
    char_u	*stl_clickfunc;	// function name for ClickFunc items
} stl_شے_T;

ساکن size_t		stl_اشیاء_len = 20; // Initial value, grows as needed.
ساکن stl_شے_T      *stl_اشیاء = تہی;
ساکن صحیح	       *stl_groupitem = تہی;
ساکن stl_hlrec_T     *stl_hltab = تہی;
ساکن stl_hlrec_T     *stl_tabtab = تہی;
ساکن stl_clickrec_T  *stl_clicktab = تہی;
ساکن صحیح		*stl_separator_locations = تہی;

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
    صحیح
build_stl_str_hl(
    win_T	*wp,
    char_u	*برآمد,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحیح		opt_scope,	// scope for "opt_name"
    صحیح		fillchar,
    صحیح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    واپس build_stl_str_hl_local(STL_منوال_SINGLE, wp, برآمد, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    تہی, تہی);
}

    صحیح
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*برآمد,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحیح		opt_scope,	// scope for "opt_name"
    صحیح		fillchar,
    صحیح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    صحیح		*حاصل_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    واپس build_stl_str_hl_local(STL_منوال_MULTI, wp, برآمد, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    تہی, حاصل_hl);
}

# ifdef ENABLE_STL_منوال_MULTI_NL
    صحیح
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*برآمد,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحیح		opt_scope,	// scope for "opt_name"
    صحیح		fillchar,
    صحیح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    صحیح		*حاصل_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    واپس build_stl_str_hl_local(STL_منوال_MULTI_NL, wp, برآمد, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    تہی, حاصل_hl);
}
# endif

    صحیح
get_stl_rendered_اونچائی(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحیح		opt_scope)	// scope for "opt_name"
{
    صحیح rendered_اونچائی = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (خالی)build_stl_str_hl_local(STL_منوال_GET_RENDERED_اونچائی,
	    wp, buf, حجم(buf), &fmt,
	    opt_name, opt_scope, 0, 0, تہی, تہی, تہی, &rendered_اونچائی,
	    تہی);
    --emsg_off;
    واپس rendered_اونچائی;
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
ساکن صحیح
build_stl_str_hl_local(
    stl_منوال_T	منوال,
    win_T	*wp,
    char_u	*برآمد,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    صحیح		opt_scope,	// scope for "opt_name"
    صحیح		fillchar,
    صحیح		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    صحیح		*rendered_اونچائی,   // return: stl rendered height (can be NULL)
    صحیح		*حاصل_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    صحیح		byteval;
# ifdef FEAT_EVAL
    صحیح		use_sandbox;
    صحیح		save_VIsual_فعال;
# endif
    صحیح		خالی_خانے_سطر;
    طویل	l;
    طویل	n;
    صحیح		prevchar_isflag;
    صحیح		prevchar_isitem;
    صحیح		itemisflag;
    صحیح		fillable;
    char_u	*str;
    طویل	عدد;
    صحیح		چوڑائی;
    صحیح		itemcnt;
    صحیح		curitem;
    صحیح		groupdepth;
# ifdef FEAT_EVAL
    صحیح		evaldepth;
# endif
    صحیح		minwid;
    صحیح		maxwid;
    صحیح		zeropad;
    char_u	بنیاد;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    صحیح		save_redraw_not_allowed = redraw_not_allowed;
    صحیح		save_کلید_Typed = کلید_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    صحیح		did_emsg_before = did_emsg;
    صحیح		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    اگر (updating_screen)
	redraw_not_allowed = TRUE;

    اگر (stl_اشیاء == تہی)
    {
	stl_اشیاء = ALLOC_MULT(stl_شے_T, stl_اشیاء_len);
	stl_groupitem = ALLOC_MULT(صحیح, stl_اشیاء_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_اشیاء_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_اشیاء_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_اشیاء_len + 1);

	stl_separator_locations = ALLOC_MULT(صحیح, stl_اشیاء_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_سیٹ_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    اگر (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_نمبر;
	tv.vval.v_نمبر = wp->w_id;
	سیٹ_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_مقصد_string_محفوظ(fmt + 2, use_sandbox, FALSE, FALSE);
	اگر (usefmt == تہی)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    اگر (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    اگر (lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
    {
	lnum = wp->w_بفر->b_ml.ml_سطر_گنتی;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_بفر, lnum, FALSE);
    خالی_خانے_سطر = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_بفر, lnum);
    اگر (wp->w_cursor.ستون > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.ستون = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    ورنہ
	byteval = (*mb_ptr2char)(p + wp->w_cursor.ستون);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = برآمد;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    اگر (حاصل_hl != تہی && *حاصل_hl != 0)
    {
	stl_اشیاء[curitem].stl_type = Highlight;
	stl_اشیاء[curitem].stl_آغاز = p;
	stl_اشیاء[curitem].stl_minwid = *حاصل_hl;
	stl_اشیاء[curitem].stl_clickfunc = تہی;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    برائے (s = usefmt; *s != NUL; )
    {
	اگر (curitem == (صحیح)stl_اشیاء_len)
	{
	    size_t	new_len = stl_اشیاء_len * 3 / 2;

	    stl_شے_T *new_اشیاء =
			  vim_realloc(stl_اشیاء, حجم(stl_شے_T) * new_len);
	    اگر (new_اشیاء == تہی)
		توڑو;
	    stl_اشیاء = new_اشیاء;

	    صحیح *new_groupitem =
			     vim_realloc(stl_groupitem, حجم(صحیح) * new_len);
	    اگر (new_groupitem == تہی)
		توڑو;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  حجم(stl_hlrec_T) * (new_len + 1));
	    اگر (new_hlrec == تہی)
		توڑو;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  حجم(stl_hlrec_T) * (new_len + 1));
	    اگر (new_hlrec == تہی)
		توڑو;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      حجم(stl_clickrec_T) * (new_len + 1));
	    اگر (new_clickrec == تہی)
		توڑو;
	    stl_clicktab = new_clickrec;

	    صحیح *new_separator_locs = vim_realloc(stl_separator_locations,
					    حجم(صحیح) * new_len);
	    اگر (new_separator_locs == تہی)
		توڑو;
	    stl_separator_locations = new_separator_locs;

	    stl_اشیاء_len = new_len;
	}

	stl_اشیاء[curitem].stl_clickfunc = تہی;

	اگر (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	جب (*s != NUL && *s != '%' && p + 1 < برآمد + outlen)
# ifdef ENABLE_STL_منوال_MULTI_NL
	{
	    اگر (*s == '\n' || *s == '\r')
	    {
		اگر (منوال == STL_منوال_MULTI_NL)
		{
		    s++;
		    جاؤ ڈھونڈو_linebreak;
		}
		ورنہ اگر (منوال == STL_منوال_GET_RENDERED_اونچائی)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_منوال_MULTI_NL
	}
# endif
	اگر (*s == NUL || p + 1 >= برآمد + outlen)
	    توڑو;

	/*
	 * Handle one '%' item.
	 */
	s++;
	اگر (*s == NUL)  // ignore trailing %
	    توڑو;

	اگر (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    اگر (s[1] == ']')
	    {
		stl_اشیاء[curitem].stl_type = ClickFunc;
		stl_اشیاء[curitem].stl_آغاز = p;
		stl_اشیاء[curitem].stl_minwid = 0;
		stl_اشیاء[curitem].stl_clickfunc = تہی;
		s += 2;
		curitem++;
		جاری;
	    }
	    // %[FuncName] - start click region
	    اگر (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		اگر (rb != تہی)
		{
		    stl_اشیاء[curitem].stl_type = ClickFunc;
		    stl_اشیاء[curitem].stl_آغاز = p;
		    stl_اشیاء[curitem].stl_minwid = 0;
		    stl_اشیاء[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    جاری;
		}
	    }
	}
	اگر (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    اگر (منوال == STL_منوال_MULTI
# ifdef ENABLE_STL_منوال_MULTI_NL
		    || منوال == STL_منوال_MULTI_NL
# endif
	       )
	    {
		s++;
		توڑو;
	    }
	    ورنہ اگر (منوال == STL_منوال_GET_RENDERED_اونچائی)
		rheight++;
	}
	اگر (*s == '%')
	{
	    اگر (p + 1 >= برآمد + outlen)
		توڑو;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    جاری;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	اگر (*s == STL_SEPARATE)
	{
	    s++;
	    اگر (groupdepth > 0)
		جاری;
	    stl_اشیاء[curitem].stl_type = Separate;
	    stl_اشیاء[curitem++].stl_آغاز = p;
	    جاری;
	}
	اگر (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_اشیاء[curitem].stl_type = Trunc;
	    stl_اشیاء[curitem++].stl_آغاز = p;
	    جاری;
	}
	اگر (*s == ')')
	{
	    char_u  *t;

	    s++;
	    اگر (groupdepth < 1)
		جاری;
	    groupdepth--;

	    t = stl_اشیاء[stl_groupitem[groupdepth]].stl_آغاز;
	    *p = NUL;
	    l = vim_strsize(t);
	    اگر (curitem > stl_groupitem[groupdepth] + 1
		    && stl_اشیاء[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		صحیح گروہ_آغاز_userhl = 0;
		صحیح گروہ_انتہا_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		برائے (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    اگر (stl_اشیاء[n].stl_type == Highlight)
		    {
			گروہ_آغاز_userhl = گروہ_انتہا_userhl =
						       stl_اشیاء[n].stl_minwid;
			توڑو;
		    }
		}
		برائے (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    اگر (stl_اشیاء[n].stl_type == معمول)
			توڑو;
		    اگر (stl_اشیاء[n].stl_type == Highlight)
			گروہ_انتہا_userhl = stl_اشیاء[n].stl_minwid;
		}
		اگر (n == curitem && گروہ_آغاز_userhl == گروہ_انتہا_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    برائے (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			اگر (stl_اشیاء[n].stl_type == Highlight)
			    stl_اشیاء[n].stl_type = خالی_خانے;
			// adjust the start position of TabPage to the next
			// item position
			اگر (stl_اشیاء[n].stl_type == Tab_صفحہ)
			    stl_اشیاء[n].stl_آغاز = p;
		    }
		}
	    }
	    اگر (l > stl_اشیاء[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		اگر (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    جب (l >= stl_اشیاء[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		ورنہ
		    n = (طویل)(p - t) - stl_اشیاء[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		صحیح minwid_fixed = MIN(stl_اشیاء[stl_groupitem[groupdepth]].stl_minwid,
				       stl_اشیاء[stl_groupitem[groupdepth]].stl_maxwid);
		جب (++l < minwid_fixed)
		    MB_CHAR2_بائٹیں(fillchar, p);

		// correct the start of the items for the truncation
		برائے (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_اشیاء[l].stl_آغاز -= n - 1;
		    اگر (stl_اشیاء[l].stl_آغاز < t)
			stl_اشیاء[l].stl_آغاز = t;
		}
	    }
	    ورنہ اگر (مطلق_قدر(stl_اشیاء[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_اشیاء[stl_groupitem[groupdepth]].stl_minwid;
		صحیح fillchar_len = MB_CHAR2LEN(fillchar);
		اگر (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    جب (l++ < n && p + fillchar_len < برآمد + outlen)
			MB_CHAR2_بائٹیں(fillchar, p);
		}
		ورنہ
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    اگر (p + l >= برآمد + outlen)
		    {
			n = (طویل)((برآمد + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    برائے ( ; n > 0; n--)
			MB_CHAR2_بائٹیں(fillchar, t);
		    برائے (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_اشیاء[n].stl_آغاز += l;
		}
	    }
	    جاری;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	اگر (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	اگر (*s == '-')
	{
	    s++;
	    l = -1;
	}
	اگر (VIM_ISDIGIT(*s))
	{
	    minwid = (صحیح)getdigits(&s);
	    اگر (minwid < 0)	//حد_سے_تجاوز
		minwid = 0;
	}
	اگر (*s == STL_USER_HL)
	{
	    stl_اشیاء[curitem].stl_type = Highlight;
	    stl_اشیاء[curitem].stl_آغاز = p;
	    stl_اشیاء[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    جاری;
	}
	اگر (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    اگر (*s == STL_TABCLOSENR)
	    {
		اگر (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    برائے (n = curitem - 1; n >= 0; --n)
			اگر (stl_اشیاء[n].stl_type == Tab_صفحہ
					       && stl_اشیاء[n].stl_minwid >= 0)
			{
			    minwid = stl_اشیاء[n].stl_minwid;
			    توڑو;
			}
		}
		ورنہ
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_اشیاء[curitem].stl_type = Tab_صفحہ;
	    stl_اشیاء[curitem].stl_آغاز = p;
	    stl_اشیاء[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    جاری;
	}
	اگر (*s == '.')
	{
	    s++;
	    اگر (VIM_ISDIGIT(*s))
	    {
		maxwid = (صحیح)getdigits(&s);
		اگر (maxwid <= 0)	//حد_سے_تجاوز
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	صحیح raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	اگر (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_اشیاء[curitem].stl_type = گروہ_2;
	    stl_اشیاء[curitem].stl_آغاز = p;
	    stl_اشیاء[curitem].stl_minwid = minwid;
	    stl_اشیاء[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    جاری;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	اگر (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    جاری;
	}
# endif
	اگر (vim_strchr(STL_ALL, *s) == تہی)
	{
	    اگر (*s == NUL)  // can happen with "%0"
		توڑو;
	    s++;
	    جاری;
	}
	opt = *s++;

	// OK - now for the real work
	بنیاد = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	عدد = -1;
	str = تہی;
	انتخاب (opt)
	{
	حالت STL_FILEPATH:
	حالت STL_FULLPATH:
	حالت STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_بفر);
	    اگر (name != تہی)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    ورنہ
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_بفر->b_ffname
					  : wp->w_بفر->b_fname;
		home_replace(wp->w_بفر, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    اگر (opt != STL_FILENAME)
		str = NameBuff;
	    ورنہ
		str = gettail(NameBuff);
	    توڑو;
	}

	حالت STL_VIM_اظہاریہ: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *بلاک_آغاز = s - 1;
# endif
	    صحیح	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    اگر (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    جب ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < برآمد + outlen)
		*p++ = *s++;
	    اگر (*s != '}')	// missing '}' or out of space
		توڑو;
	    s++;
	    اگر (reevaluate && p > برآمد)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    ورنہ
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((حرف *)buf_tmp, حجم(buf_tmp),
							 "%d", curbuf->b_fnum);
	    سیٹ_اندرونی_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((حرف *)buf_tmp, حجم(buf_tmp), "%d", curwin->w_id);
	    سیٹ_اندرونی_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_فعال = VIsual_فعال;
	    curwin = wp;
	    curbuf = wp->w_بفر;
	    // Visual mode is only valid in the current window.
	    اگر (curwin != save_curwin)
		VIsual_فعال = FALSE;

	    str = eval_مقصد_string_محفوظ(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_فعال = save_VIsual_فعال;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    اگر (!zeropad && str != تہی && *str != NUL)
	    {
		اگر (*skipdigits(str) == NUL)
		{
		    عدد = atoi((حرف *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    اگر (reevaluate && str != تہی && *str != NUL
		    && strchr((ثابت حرف *)str, '%') != تہی
		    && evaldepth < MAX_STL_EVAL_گہرائی)
	    {
		size_t parsed_usefmt = (size_t)(بلاک_آغاز - usefmt);
		size_t str_لمبائی = لڑی_لمبائی((ثابت حرف *)str);
		size_t fmt_لمبائی = لڑی_لمبائی((ثابت حرف *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_لمبائی + fmt_لمبائی + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * حجم(char_u));

		اگر (new_fmt != تہی)
		{
		    char_u *new_fmt_p = new_fmt;

		    new_fmt_p = (char_u *)حافظہ_نقل(new_fmt_p, usefmt, parsed_usefmt)
								   + parsed_usefmt;
		    new_fmt_p = (char_u *)حافظہ_نقل(new_fmt_p , str, str_لمبائی)
								      + str_لمبائی;
		    new_fmt_p = (char_u *)حافظہ_نقل(new_fmt_p, "%}", 2) + 2;
		    new_fmt_p = (char_u *)حافظہ_نقل(new_fmt_p , s, fmt_لمبائی)
								      + fmt_لمبائی;
		    *new_fmt_p = 0;
		    new_fmt_p = تہی;

		    اگر (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    جاری;
		}
	    }
# endif
	    توڑو;
	}
	حالت STL_سطر:
	    عدد = (wp->w_بفر->b_ml.ml_flags & ML_خالی_خانے)
		  ? 0L : (طویل)(wp->w_cursor.lnum);
	    توڑو;

	حالت STL_NUMLINES:
	    عدد = wp->w_بفر->b_ml.ml_سطر_گنتی;
	    توڑو;

	حالت STL_عمود:
	    عدد = (State & منوال_داخل_کرو) == 0 && خالی_خانے_سطر
					       ? 0 : (صحیح)wp->w_cursor.ستون + 1;
	    توڑو;

	حالت STL_VIRTCOL:
	حالت STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    اگر (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & منوال_داخل_کرو) == 0
			       && خالی_خانے_سطر ? 0 : (صحیح)wp->w_cursor.ستون + 1)))
		توڑو;
	    عدد = (طویل)virtcol;
	    توڑو;
	}

	حالت STL_PERCENTAGE:
	    عدد = calc_percentage((طویل)wp->w_cursor.lnum, (طویل)wp->w_بفر->b_ml.ml_سطر_گنتی);
	    توڑو;

	حالت STL_ALTPERCENT:
	    str = buf_tmp;
	    (خالی)get_rel_pos(wp, str, TMPLEN);
	    توڑو;

	حالت STL_SHOWCMD:
	    اگر (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    توڑو;

	حالت STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    اگر (append_arg_نمبر(wp, buf_tmp, حجم(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    توڑو;

	حالت STL_KEYMAP:
	    fillable = FALSE;
	    اگر (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    توڑو;
	حالت STL_PAGENUM:
# اگر defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    عدد = printer_صفحہ_عدد;
# ورنہ
	    عدد = 0;
# endif
	    توڑو;

	حالت STL_BUFNO:
	    عدد = wp->w_بفر->b_fnum;
	    توڑو;

	حالت STL_انحراف_X:
	    بنیاد = 'X';
	    // FALLTHROUGH
	حالت STL_انحراف:
# ifdef FEAT_BYTEOFF
	    l = ml_ڈھونڈو_سطر_or_انحراف(wp->w_بفر, wp->w_cursor.lnum, تہی);
	    عدد = (wp->w_بفر->b_ml.ml_flags & ML_خالی_خانے) || l < 0
		       ? 0L : l + 1 + ((State & منوال_داخل_کرو) == 0 && خالی_خانے_سطر
				? 0 : (صحیح)wp->w_cursor.ستون);
# endif
	    توڑو;

	حالت STL_BYTEVAL_X:
	    بنیاد = 'X';
	    // FALLTHROUGH
	حالت STL_BYTEVAL:
	    عدد = byteval;
	    اگر (عدد == NL)
		عدد = 0;
	    ورنہ اگر (عدد == CAR && get_fileformat(wp->w_بفر) == EOL_MAC)
		عدد = NL;
	    توڑو;

	حالت STL_ROFLAG:
	حالت STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    اگر (wp->w_بفر->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    توڑو;

	حالت STL_HELPFLAG:
	حالت STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    اگر (wp->w_بفر->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    توڑو;

	حالت STL_FILETYPE:
	    اگر (*wp->w_بفر->b_p_ft != NUL
		    && STRLEN(wp->w_بفر->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((حرف *)buf_tmp, حجم(buf_tmp), "[%s]",
							wp->w_بفر->b_p_ft);
		str = buf_tmp;
	    }
	    توڑو;

	حالت STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    اگر (*wp->w_بفر->b_p_ft != NUL
		    && STRLEN(wp->w_بفر->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((حرف *)buf_tmp, حجم(buf_tmp), ",%s",
							wp->w_بفر->b_p_ft);
		برائے (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    توڑو;

# اگر defined(FEAT_QUICKFIX)
	حالت STL_PREVIEWFLAG:
	حالت STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    اگر (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    توڑو;

	حالت STL_QUICKFIX:
	    اگر (bt_quickfix(wp->w_بفر))
		str = (char_u *)(wp->w_llist_ref
			    ? _(پیغام_loclist)
			    : _(پیغام_qflist));
	    توڑو;
# endif

	حالت STL_MODIFIED:
	حالت STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    انتخاب ((opt == STL_MODIFIED_ALT)
		    + buf_Is_بدلا(wp->w_بفر) * 2
		    + (!wp->w_بفر->b_p_ma) * 4)
	    {
		حالت 2: str = (char_u *)"[+]"; توڑو;
		حالت 3: str = (char_u *)",+"; توڑو;
		حالت 4: str = (char_u *)"[-]"; توڑو;
		حالت 5: str = (char_u *)",-"; توڑو;
		حالت 6: str = (char_u *)"[+-]"; توڑو;
		حالت 7: str = (char_u *)",+-"; توڑو;
	    }
	    توڑو;

	حالت STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		جب (*s != '#' && *s != NUL)
		    ++s;
		اگر (*s == '#')
		{
		    stl_اشیاء[curitem].stl_type = Highlight;
		    stl_اشیاء[curitem].stl_آغاز = p;
		    stl_اشیاء[curitem].stl_minwid = -syn_namen2id(t, (صحیح)(s - t));
		    curitem++;
		}
		اگر (*s != NUL)
		    ++s;
		جاری;
	    }

	حالت STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    اگر (*s == ']')
	    {
		stl_اشیاء[curitem].stl_type = ClickFunc;
		stl_اشیاء[curitem].stl_آغاز = p;
		stl_اشیاء[curitem].stl_minwid = 0;
		stl_اشیاء[curitem].stl_clickfunc = تہی;
		s++;
		curitem++;
		جاری;
	    }
	    // %N[FuncName] with minwid
	    اگر (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		اگر (rb != تہی)
		{
		    stl_اشیاء[curitem].stl_type = ClickFunc;
		    stl_اشیاء[curitem].stl_آغاز = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_اشیاء[curitem].stl_minwid = raw_minwid;
		    stl_اشیاء[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    جاری;
		}
	    }
	    جاری;

	حالت STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    جاری;
	}

	stl_اشیاء[curitem].stl_آغاز = p;
	stl_اشیاء[curitem].stl_type = معمول;
	اگر (str != تہی && *str)
	{
	    char_u  *t = str;

	    اگر (itemisflag)
	    {
		اگر ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    اگر (l > 0)
		prevchar_isitem = TRUE;
	    اگر (l > maxwid)
	    {
		جب (l >= maxwid)
		    اگر (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    ورنہ
			l -= byte2cells(*t++);
		اگر (p + 1 >= برآمد + outlen)
		    توڑو;
		*p++ = '<';
	    }
	    اگر (minwid > 0)
	    {
		برائے (; l < minwid && p + 1 < برآمد + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    اگر (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    ورنہ
			MB_CHAR2_بائٹیں(fillchar, p);
		}
		minwid = 0;
	    }
	    ورنہ
		minwid *= -1;
	    برائے (; *t && p + 1 < برآمد + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		اگر (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2_بائٹیں(fillchar, p);
		ورنہ
		    *p++ = *t;
	    }
	    برائے (; l < minwid && p + 1 < برآمد + outlen; l++)
		MB_CHAR2_بائٹیں(fillchar, p);
	}
	ورنہ اگر (عدد >= 0)
	{
	    صحیح	    nbase = (بنیاد == 'D' ? 10 : (بنیاد == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    اگر (p + 20 >= برآمد + outlen)
		توڑو;		// not sufficient space
	    prevchar_isitem = TRUE;
	    اگر (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    اگر (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? بنیاد : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    برائے (n = عدد, l = 1; n >= nbase; n /= nbase)
		l++;
	    اگر (opt == STL_VIRTCOL_ALT)
		l++;

	    اگر (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		جب (l-- > maxwid)
		    عدد /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((حرف *)p, outlen - (p - برآمد),
		    (حرف *)nstr, 0, عدد, n);
	    }
	    ورنہ
		p += vim_snprintf_safelen((حرف *)p, outlen - (p - برآمد),
		    (حرف *)nstr, minwid, عدد);
	}
	ورنہ
	    stl_اشیاء[curitem].stl_type = خالی_خانے;

	اگر (عدد >= 0 || (!itemisflag && str != تہی && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	اگر (opt == STL_VIM_اظہاریہ)
	    vim_free(str);
	curitem++;
    }
# ifdef ENABLE_STL_منوال_MULTI_NL
ڈھونڈو_linebreak:
# endif
    *p = NUL;
    outputlen = (size_t)(p - برآمد);
    itemcnt = curitem;

    // Remember the most recent %# / %* highlight so the next row of a
    // multi-line statusline can resume it.
    اگر (حاصل_hl != تہی)
    {
	صحیح آخری_hl = 0;
	برائے (l = 0; l < itemcnt; l++)
	    اگر (stl_اشیاء[l].stl_type == Highlight)
		آخری_hl = stl_اشیاء[l].stl_minwid;
	*حاصل_hl = آخری_hl;
    }

    اگر (منوال == STL_منوال_MULTI
# ifdef ENABLE_STL_منوال_MULTI_NL
		    || منوال == STL_منوال_MULTI_NL
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
	اگر (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = لڑی_لمبائی((حرف *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    ورنہ
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	اگر (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    اگر (منوال == STL_منوال_GET_RENDERED_اونچائی)
    {
	// Free click function names that were allocated during parsing.
	برائے (l = 0; l < itemcnt; l++)
	    اگر (stl_اشیاء[l].stl_type == ClickFunc)
		vim_free(stl_اشیاء[l].stl_clickfunc);
	اگر (rendered_اونچائی != تہی)
	    *rendered_اونچائی = rheight;
	واپس 0;
    }

    چوڑائی = vim_strsize(برآمد);
    اگر (maxwidth > 0 && چوڑائی > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	اگر (itemcnt == 0)
	    s = برآمد;
	ورنہ
	{
	    برائے ( ; l < itemcnt; l++)
		اگر (stl_اشیاء[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_اشیاء[l].stl_آغاز;
		    توڑو;
		}
	    اگر (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_اشیاء[0].stl_آغاز;
		l = 0;
	    }
	}

	اگر (چوڑائی - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    اگر (has_mbyte)
	    {
		s = برآمد;
		چوڑائی = 0;
		برائے (;;)
		{
		    چوڑائی += ptr2cells(s);
		    اگر (چوڑائی >= maxwidth)
			توڑو;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		جب (++چوڑائی < maxwidth)
		    MB_CHAR2_بائٹیں(fillchar, s);
	    }
	    ورنہ
		s = برآمد + maxwidth - 1;
	    برائے (l = 0; l < itemcnt; l++)
		اگر (stl_اشیاء[l].stl_آغاز > s)
		    توڑو;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	ورنہ
	{
	    char_u  *انتہا = برآمد + outputlen;

	    n = 0;
	    جب (چوڑائی >= maxwidth)
	    {
		چوڑائی -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(انتہا - p) + 1);	// +1 for NUL
	    انتہا -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    برائے (; l < itemcnt; l++)
	    {
		اگر (stl_اشیاء[l].stl_آغاز - n >= s)
		    stl_اشیاء[l].stl_آغاز -= n;
		ورنہ
		    stl_اشیاء[l].stl_آغاز = s;
	    }

	    // Fill up for half a double-wide character.
	    جب (++چوڑائی < maxwidth)
	    {
		s = انتہا;
		MB_CHAR2_بائٹیں(fillchar, s);
		*s = NUL;
		انتہا = s;
	    }
	}
	چوڑائی = maxwidth;
    }
    ورنہ اگر (چوڑائی < maxwidth &&
	    outputlen + (maxwidth - چوڑائی) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	صحیح عدد_separators = 0;

	برائے (l = 0; l < itemcnt; l++)
	{
	    اگر (stl_اشیاء[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[عدد_separators] = l;
		عدد_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	اگر (عدد_separators)
	{
	    صحیح standard_spaces;
	    صحیح final_spaces;

	    standard_spaces = (maxwidth - چوڑائی) / عدد_separators;
	    final_spaces = (maxwidth - چوڑائی) -
					standard_spaces * (عدد_separators - 1);
	    برائے (l = 0; l < عدد_separators; l++)
	    {
		صحیح dislocation = (l == (عدد_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *آغاز = stl_اشیاء[stl_separator_locations[l]].stl_آغاز;
		char_u *seploc = آغاز + dislocation;
		STRMOVE(seploc, آغاز);
		برائے (s = آغاز; s < seploc;)
		    MB_CHAR2_بائٹیں(fillchar, s);

		برائے (صحیح i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_اشیاء[i].stl_آغاز += dislocation;
	    }

	    چوڑائی = maxwidth;
	}
    }

    // Store the info about highlighting.
    اگر (hltab != تہی)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	برائے (l = 0; l < itemcnt; l++)
	{
	    اگر (stl_اشیاء[l].stl_type == Highlight)
	    {
		sp->آغاز = stl_اشیاء[l].stl_آغاز;
		sp->userhl = stl_اشیاء[l].stl_minwid;
		sp++;
	    }
	}
	sp->آغاز = تہی;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    اگر (tabtab != تہی)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	برائے (l = 0; l < itemcnt; l++)
	{
	    اگر (stl_اشیاء[l].stl_type == Tab_صفحہ)
	    {
		sp->آغاز = stl_اشیاء[l].stl_آغاز;
		sp->userhl = stl_اشیاء[l].stl_minwid;
		sp++;
	    }
	}
	sp->آغاز = تہی;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    اگر (clicktab != تہی)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	برائے (l = 0; l < itemcnt; l++)
	{
	    اگر (stl_اشیاء[l].stl_type == ClickFunc)
	    {
		cp->آغاز = stl_اشیاء[l].stl_آغاز;
		cp->funcname = stl_اشیاء[l].stl_clickfunc;
		cp->minwid = stl_اشیاء[l].stl_minwid;
		cp++;
	    }
	}
	cp->آغاز = تہی;
	cp->funcname = تہی;
    }
    ورنہ
    {
	// Free click function names when caller doesn't need them.
	برائے (l = 0; l < itemcnt; l++)
	    اگر (stl_اشیاء[l].stl_type == ClickFunc)
		vim_free(stl_اشیاء[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    کلید_Typed = save_کلید_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    اگر (did_emsg > did_emsg_before)
	سیٹ_string_option_براہ_راست(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    واپس چوڑائی;
}
#اگر_ختم // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    صحیح
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    صحیح		buflen)
{
    طویل	above; // number of lines above window
    طویل	نیچے; // number of lines below window

    اگر (buflen < 3) // need at least 3 chars for writing
	واپس 0;
    above = wp->w_topline - 1;
#اگر_تعریف_شدہ FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    اگر (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#اگر_ختم
    نیچے = wp->w_بفر->b_ml.ml_سطر_گنتی - wp->w_botline + 1;
    اگر (نیچے <= 0)
	واپس (صحیح)vim_snprintf_safelen((حرف *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    اگر (above <= 0)
	واپس (صحیح)vim_snprintf_safelen((حرف *)buf, buflen,
	    "%s", _("Top"));

    صحیح perc = calc_percentage(above, above + نیچے);
    حرف tmp[8];
    // localized percentage value
    vim_snprintf(tmp, حجم(tmp), _("%d%%"), perc);
    واپس (صحیح)vim_snprintf_safelen((حرف *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    ساکن صحیح
append_arg_نمبر(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    صحیح		جوڑو_file)	// Add "file" before the arg number
{
    اگر (ARGCOUNT <= 1)		// nothing to do
	واپس 0;

    حرف *پیغام;
    انتخاب ((wp->w_arg_idx_invalid ? 1 : 0) + (جوڑو_file ? 2 : 0))
    {
	حالت 0: پیغام = _(" (%d of %d)"); توڑو;
	حالت 1: پیغام = _(" ((%d) of %d)"); توڑو;
	حالت 2: پیغام = _(" (file %d of %d)"); توڑو;
	حالت 3: پیغام = _(" (file (%d) of %d)"); توڑو;
    }

    واپس (صحیح)vim_snprintf_safelen((حرف *)buf, buflen, پیغام,
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
#اگر_تعریف_شدہ UNIX
    واپس بھرا_Name_save(fname, TRUE);
#ورنہ
    اگر (!vim_isAbsName(fname)
	    || لڑی_تلاش((حرف *)fname, "..") != تہی
	    || لڑی_تلاش((حرف *)fname, "//") != تہی
# ifdef BACKSLASH_اندر_FILENAME
	    || لڑی_تلاش((حرف *)fname, "\\\\") != تہی
# endif
# اگر defined(MSWIN)
	    || vim_strchr(fname, '~') != تہی
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	واپس بھرا_Name_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    اگر (fname != تہی)
	fname_case(fname, 0);	// set correct case for file name
# endif

    واپس fname;
#اگر_ختم
}

/*
 * Make "*ffname" a full file name, set "*sfname" to "*ffname" if not NULL.
 * "*ffname" becomes a pointer to allocated memory (or NULL).
 * When resolving a link both "*sfname" and "*ffname" will point to the same
 * allocated memory.
 * The "*ffname" and "*sfname" pointer values on call will not be freed.
 * Note that the resulting "*ffname" pointer should be considered not allocated.
 */
    خالی
fname_expand(
    buf_T	*buf غیر_مستعمل,
    char_u	**ffname,
    char_u	**sfname)
{
    اگر (*ffname == تہی)	    // no file name given, nothing to do
	واپس;
    اگر (*sfname == تہی)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#اگر_تعریف_شدہ FEAT_SHORTCUT
    اگر (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_راستہ(*ffname, FALSE);
	اگر (rfname != تہی)
	{
	    vim_free(*ffname);
	    *ffname = rfname;
	    *sfname = rfname;
	}
    }
#اگر_ختم
}

/*
 * Open a window for a number of buffers.
 */
    خالی
ex_بفر_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    صحیح		بانٹو_ret = OK;
    صحیح		p_ea_save;
    صحیح		open_wins = 0;
    صحیح		r;
    صحیح		گنتی;		// Maximum number of windows to open.
    صحیح		all;		// When TRUE also load inactive buffers.
    صحیح		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    اگر (eap->addr_گنتی == 0)	// make as many windows as possible
	گنتی = 9999;
    ورنہ
	گنتی = eap->line2;	// make as many windows as specified
    اگر (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    ورنہ
	all = TRUE;

    // Stop Visual mode, the cursor and "VIsual" may very well be invalid after
    // switching to another buffer.
    reset_VIsual_and_resel();

    setpcmark();

#اگر_تعریف_شدہ FEAT_GUI
    need_mouse_correct = TRUE;
#اگر_ختم

    /*
     * Close superfluous windows (two windows for the same buffer).
     * Also close windows that are not full-width.
     */
    اگر (had_tab > 0)
	goto_tabpage_tp(پہلا_tabpage, TRUE, TRUE);
    برائے (;;)
    {
	tpnext = curtab->tp_اگلا;
	برائے (wp = firstwin; wp != تہی; wp = wpnext)
	{
	    wpnext = wp->w_اگلا;
	    اگر ((wp->w_بفر->b_nwindows > 1
			|| ((cmdmod.cmod_بانٹو & WSP_VERT)
			    ? wp->w_اونچائی + wp->w_حالت_اونچائی < صفیں - p_ch
							     - tabline_اونچائی()
			    : wp->w_چوڑائی != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_کھڑکی
		    && !(win_locked(wp) || wp->w_بفر->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		اگر (win_close(wp, FALSE) == FAIL)
		    توڑو;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = پہلا_tabpage;
		open_wins = 0;
	    }
	    ورنہ
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	اگر (had_tab == 0 || tpnext == تہی)
	    توڑو;
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
    برائے (buf = firstbuf; buf != تہی && open_wins < گنتی; buf = buf->b_اگلا)
    {
	// Check if this buffer needs a window
	اگر ((!all && buf->b_ml.ml_mfp == تہی) || !buf->b_p_bl)
	    جاری;

	اگر (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    اگر (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    ورنہ
		wp = تہی;
	}
	ورنہ
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		اگر (wp->w_بفر == buf)
		    توڑو;
	    // If the buffer already has a window, move it
	    اگر (wp != تہی)
		win_move_after(wp, curwin);
	}

	اگر (wp == تہی && بانٹو_ret == OK)
	{
	    bufref_T	bufref;

	    سیٹ_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    بانٹو_ret = win_بانٹو(0, WSP_گنجائش | WSP_نیچے);
	    ++open_wins;
	    p_ea = p_ea_save;
	    اگر (بانٹو_ret == FAIL)
		جاری;

	    // Open the buffer in this window.
	    تبادلہ_exists_action = SEA_DIALOG;
	    سیٹ_curbuf(buf, DOBUF_GOTO);
	    اگر (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		تبادلہ_exists_action = SEA_NONE;
		توڑو;
	    }
	    اگر (تبادلہ_exists_action == SEA_QUIT)
	    {
#اگر defined(FEAT_EVAL)
		cleanup_T   cs;

		// Reset the error/interrupt/exception state here so that
		// aborting() returns FALSE when closing a window.
		enter_cleanup(&cs);
#اگر_ختم

		// User selected Quit at ATTENTION prompt; close this window.
		win_close(curwin, TRUE);
		--open_wins;
		تبادلہ_exists_action = SEA_NONE;
		تبادلہ_exists_did_quit = TRUE;

#اگر defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#اگر_ختم
	    }
	    ورنہ
		handle_تبادلہ_exists(تہی);
	}

	ui_breakcheck();
	اگر (got_int)
	{
	    (خالی)vgetc();	// only break the file loading, not the rest
	    توڑو;
	}
#اگر_تعریف_شدہ FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	اگر (aborting())
	    توڑو;
#اگر_ختم
	// When ":tab" was used open a new tab for a new window repeatedly.
	اگر (had_tab > 0 && tabpage_اشاریہ(تہی) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    برائے (wp = lastwin; open_wins > گنتی; )
    {
	r = (buf_hide(wp->w_بفر) || !buf_Is_بدلا(wp->w_بفر)
				     || autowrite(wp->w_بفر, FALSE) == OK);
	اگر (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	ورنہ اگر (r)
	{
	    win_close(wp, !buf_hide(wp->w_بفر));
	    --open_wins;
	    wp = lastwin;
	}
	ورنہ
	{
	    wp = wp->w_پچھلا;
	    اگر (wp == تہی)
		توڑو;
	}
    }
}


ساکن صحیح  chk_modeline(linenr_T, صحیح);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    خالی
do_modelines(صحیح flags)
{
    linenr_T	lnum;
    صحیح		nmlines;
    ساکن صحیح	entered = 0;

    اگر (!curbuf->b_p_ml || (nmlines = (صحیح)p_mls) == 0)
	واپس;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    اگر (entered)
	واپس;

    ++entered;
    برائے (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_سطر_گنتی && lnum <= nmlines;
								       ++lnum)
	اگر (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    برائے (lnum = curbuf->b_ml.ml_سطر_گنتی; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_سطر_گنتی - nmlines; --lnum)
	اگر (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#شامل "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    ساکن صحیح
chk_modeline(
    linenr_T	lnum,
    صحیح		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*سطر_انتہا;		// point to the end of the line
    char_u	*e;
    صحیح		پچھلا;
    صحیح		retval = OK;
    ESTACK_CHECK_DECLARATION;

    پچھلا = -1;
    s = ml_get(lnum);
    سطر_انتہا = s + ml_get_len(lnum);
    برائے (; *s != NUL; ++s)
    {
	اگر (پچھلا == -1 || vim_isspace(پچھلا))
	{
	    اگر ((پچھلا != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		توڑو;
	    // Accept both "vim" and "Vim".
	    اگر ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		صحیح vers;

		اگر (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		ورنہ
		    e = s + 3;
		vers = getdigits(&e);
		اگر (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_نسخہ_100 >= vers && محفوظ_isdigit(s[3]))
			    || (VIM_نسخہ_100 < vers && s[3] == '<')
			    || (VIM_نسخہ_100 > vers && s[3] == '>')
			    || (VIM_نسخہ_100 == vers && s[3] == '=')))
		    توڑو;
	    }
	}
	پچھلا = *s;
    }

    اگر (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	صحیح	انتہا;

	کرو				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	جب (s[-1] != ':');

	len = (size_t)(سطر_انتہا - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	اگر (linecopy == تہی)
	    واپس FAIL;

	سطر_انتہا = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_ڈالو(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_تیاری;

	انتہا = FALSE;
	جب (انتہا == FALSE)
	{
	    s = skipwhite(s);
	    اگر (*s == NUL)
		توڑو;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    برائے (e = s; *e != ':' && *e != NUL; ++e)
		اگر (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(سطر_انتہا - (e + 1)) + 1);	// +1 for NUL
		    --سطر_انتہا;
		}
	    اگر (*e == NUL)
		انتہا = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    اگر (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		اگر (*e != ':')		// no terminating ':'?
		    توڑو;
		انتہا = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    اگر (*s != NUL)		// skip over an empty "::"
	    {
		صحیح secure_save = secure;
		sctx_T	save_current_sctx = current_sctx;

		current_sctx.sc_نسخہ = 1;
#اگر_تعریف_شدہ FEAT_EVAL
		current_sctx.sc_sid = SID_MODELINE;
		current_sctx.sc_seq = 0;
		current_sctx.sc_lnum = lnum;
#اگر_ختم

		// Make sure no risky things are executed as a side effect.
		secure = 1;

		retval = do_سیٹ(s, OPT_MODELINE | OPT_LOCAL | flags);

		secure = secure_save;
		current_sctx = save_current_sctx;
		اگر (retval == FAIL)		// stop if error found
		    توڑو;
	    }
	    s = (e == سطر_انتہا) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_نکالو();
	vim_free(linecopy);
    }
    واپس retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    صحیح
bt_معمول(buf_T *buf)
{
    واپس buf != تہی && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    صحیح
bt_quickfix(buf_T *buf غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_QUICKFIX
    واپس buf != تہی && buf->b_p_bt[0] == 'q';
#ورنہ
    واپس FALSE;
#اگر_ختم
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    صحیح
bt_terminal(buf_T *buf غیر_مستعمل)
{
#اگر defined(FEAT_TERMINAL)
    واپس buf != تہی && buf->b_p_bt[0] == 't';
#ورنہ
    واپس FALSE;
#اگر_ختم
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    صحیح
bt_help(buf_T *buf)
{
    واپس buf != تہی && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    صحیح
bt_prompt(buf_T *buf)
{
    واپس buf != تہی && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#اگر defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    صحیح
bt_popup(buf_T *buf)
{
    واپس buf != تہی && buf->b_p_bt != تہی
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#اگر_ختم

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    صحیح
bt_nofilename(buf_T *buf)
{
    واپس buf != تہی && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    ساکن صحیح
bt_nofileread(buf_T *buf)
{
    واپس buf != تہی && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#اگر defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    صحیح
bt_nofile(buf_T *buf)
{
    واپس buf != تہی && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#اگر_ختم

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    صحیح
bt_dontwrite(buf_T *buf)
{
    واپس buf != تہی && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    صحیح
bt_dontwrite_پیغام(buf_T *buf)
{
    اگر (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_سیٹ));
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    صحیح
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    انتخاب (buf->b_p_bh[0])
    {
	حالت 'u':		    // "unload"
	حالت 'w':		    // "wipe"
	حالت 'd': واپس FALSE;	    // "delete"
	حالت 'h': واپس TRUE;	    // "hide"
    }
    واپس (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#اگر defined(FEAT_QUICKFIX)
    اگر (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	اگر (buf->b_fnum == qf_ڈھیر_get_bufnr())
	    واپس (char_u *)_(پیغام_qflist);
	ورنہ
	    واپس (char_u *)_(پیغام_loclist);
    }
#اگر_ختم

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    اگر (bt_nofilename(buf))
    {
#اگر_تعریف_شدہ FEAT_TERMINAL
	اگر (buf->b_رکن != تہی)
	    واپس رکن_get_حالت_متن(buf->b_رکن);
#اگر_ختم
	اگر (buf->b_fname != تہی)
	    واپس buf->b_fname;
	اگر (buf == cmdwin_buf)
	    واپس (char_u *)_("[Command Line]");
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	اگر (bt_prompt(buf))
	    واپس (char_u *)_("[Prompt]");
#اگر_ختم
#اگر_تعریف_شدہ FEAT_PROP_POPUP
	اگر (bt_popup(buf))
	    واپس (char_u *)_("[Popup]");
#اگر_ختم
	واپس (char_u *)_("[Scratch]");
    }

    اگر (buf->b_fname == تہی)
	واپس buf_get_fname(buf);
    واپس تہی;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    اگر (buf->b_fname == تہی)
	واپس (char_u *)_("[No Name]");
    واپس buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    خالی
سیٹ_buflisted(صحیح on)
{
    اگر (on == curbuf->b_p_bl)
	واپس;

    curbuf->b_p_bl = on;
    اگر (on)
	apply_autocmds(EVENT_BUFADD, تہی, تہی, FALSE, curbuf);
    ورنہ
	apply_autocmds(EVENT_BUFDELETE, تہی, تہی, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    صحیح
buf_contents_بدلا(buf_T *buf)
{
    buf_T	*newbuf;
    صحیح		فرق = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(تہی, تہی, (linenr_T)1, BLN_DUMMY);
    اگر (newbuf == تہی)
	واپس TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    اگر (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_بفر(newbuf, FALSE);
	واپس TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    اگر (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_بفر(newbuf, FALSE);
	واپس TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    بلاک_autocmds();
    اگر (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	اگر (buf->b_ml.ml_سطر_گنتی == curbuf->b_ml.ml_سطر_گنتی)
	{
	    فرق = FALSE;
	    برائے (lnum = 1; lnum <= curbuf->b_ml.ml_سطر_گنتی; ++lnum)
		اگر (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    فرق = TRUE;
		    توڑو;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    اگر (curbuf != newbuf)	// safety check
	wipe_بفر(newbuf, FALSE);

    unblock_autocmds();

    واپس فرق;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    خالی
wipe_بفر(
    buf_T	*buf,
    صحیح		aucmd)	    // When TRUE trigger autocommands.
{
    اگر (buf->b_fnum == چوٹی_file_عدد - 1)
	--چوٹی_file_عدد;

    اگر (!aucmd)		    // Don't trigger BufDelete autocommands here.
	بلاک_autocmds();

    close_بفر(تہی, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    اگر (!aucmd)
	unblock_autocmds();
}
