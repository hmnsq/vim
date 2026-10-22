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

#ଅନ୍ତର୍ଭାଵ "vim.h"


#ifdef FEAT_EVAL
// Determines how deeply nested %{} blocks will be evaluated in statusline.
# define MAX_STL_EVAL_DEPTH 100
#endif

ସ୍ଥିର ଶୂନ୍ୟ	enter_buffer(buf_T *buf);
ସ୍ଥିର ଶୂନ୍ୟ	buflist_getfpos(ଶୂନ୍ୟ);
ସ୍ଥିର char_u	*buflist_match(regmatch_T *rmp, buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_case);
ସ୍ଥିର char_u	*fname_match(regmatch_T *rmp, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_case);
#ifdef UNIX
ସ୍ଥିର buf_T	*buflist_findname_stat(char_u *ffname, stat_T *st);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	otherfile_buf(buf_T *buf, char_u *ffname, stat_T *stp);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	buf_same_ino(buf_T *buf, stat_T *stp);
#ଅନ୍ୟଥା
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	otherfile_buf(buf_T *buf, char_u *ffname);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ମୂଲ୍ୟ_changed(char_u *str, char_u **last);
#ଯଦି defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା build_stl_str_hl_local(stl_mode_T mode, win_T *wp,
		char_u *out, size_t outlen, char_u **fmt_arg,
		char_u *opt_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା opt_scope, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fillchar, ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxwidth,
		stl_hlrec_T **hltab, stl_hlrec_T **tabtab,
		stl_clickrec_T **clicktab, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lbreaks, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *carry_hl);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	append_arg_ସଂଖ୍ୟା(win_T *wp, char_u *buf, size_t buflen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା add_file);
ସ୍ଥିର ଶୂନ୍ୟ	free_buffer(buf_T *);
ସ୍ଥିର ଶୂନ୍ୟ	free_buffer_stuff(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_options);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	bt_nofileread(buf_T *buf);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_buffer_ext(ପୂର୍ଣ୍ଣସଂଖ୍ୟା action, ପୂର୍ଣ୍ଣସଂଖ୍ୟା start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);

#ifdef UNIX
# define dev_T dev_t
#ଅନ୍ୟଥା
# define dev_T unsigned
#endif

#define FOR_ALL_BUFS_FROM_LAST(buf) \
    ପାଇଁ ((buf) = lastbuf; (buf) != ଖାଲି; (buf) = (buf)->b_prev)

#ଯଦି defined(FEAT_QUICKFIX)
ସ୍ଥିର ଅକ୍ଷର *ସନ୍ଦେଶ_loclist = N_("[Location List]");
ସ୍ଥିର ଅକ୍ଷର *ସନ୍ଦେଶ_qflist = N_("[Quickfix List]");
#endif

// Number of times free_buffer() was called.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	buf_free_ଗଣନା = 0;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଶୀର୍ଷ_file_ଅଙ୍କ = 1;	// highest file number
ସ୍ଥିର garray_T buf_reuse = GA_EMPTY;	// file numbers to recycle

    ସ୍ଥିର ଶୂନ୍ୟ
trigger_undo_ftplugin(buf_T *buf, win_T *win)
{
    window_layout_lock();
    ++buf->b_locked;
    ++win->w_locked;
    // b:undo_ftplugin may be set, undo it
    do_cmdline_cmd((char_u*)"ଯଦି exists('b:undo_ftplugin') | :legacy :exe \
	    b:undo_ftplugin | endif");
    --buf->b_locked;
    --win->w_locked;
    window_layout_unlock();
}

/*
 * Calculate the percentage that `part` is of the `whole`.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
calc_percentage(long part, long whole)
{
    // With 32 bit longs and more than 21,474,836 lines multiplying by 100
    // causes an overflow, thus for large numbers divide instead.
    ଫେରନ୍ତୁ (part > 1000000L)
	? (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(part / (whole / 100L))
	: (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((part * 100L) / whole);
}

/*
 * Return the highest possible buffer number.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_highest_fnum(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ ଶୀର୍ଷ_file_ଅଙ୍କ - 1;
}

/*
 * Read data from buffer for retrying.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
read_buffer(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		read_stdin,	    // read file from stdin, otherwise fifo
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		    // extra flags for readfile()
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    linenr_T	ଧାଡ଼ି_ଗଣନା;

    // Read from the buffer which the text is already filled in and append at
    // the end.  This makes it possible to retry when 'fileformat' or
    // 'fileencoding' was guessed wrong.
    ଧାଡ଼ି_ଗଣନା = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    retval = readfile(
	    read_stdin ? ଖାଲି : curbuf->b_ffname,
	    read_stdin ? ଖାଲି : curbuf->b_fname,
	    ଧାଡ଼ି_ଗଣନା, (linenr_T)0, (linenr_T)MAXLNUM, eap,
	    flags | READ_BUFFER);
    ଯଦି (retval == OK)
    {
	// Delete the binary lines.
	ଯେତେବେଳେ (--ଧାଡ଼ି_ଗଣନା >= 0)
	    ml_delete((linenr_T)1);
    }
    ଅନ୍ୟଥା
    {
	// Delete the converted lines.
	ଯେତେବେଳେ (curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା > ଧାଡ଼ି_ଗଣନା)
	    ml_delete(ଧାଡ଼ି_ଗଣନା);
    }
    // Put the cursor on the first line.
    curwin->w_cursor.lnum = 1;
    curwin->w_cursor.col = 0;

    ଯଦି (read_stdin)
    {
	// Set or reset 'modified' before executing autocommands, so that
	// it can be changed there.
	ଯଦି (!readonlymode && !BUFEMPTY())
	    changed();
	ଅନ୍ୟଥା ଯଦି (retval == OK)
	    unchanged(curbuf, FALSE, TRUE);

	ଯଦି (retval == OK)
	{
#ifdef FEAT_EVAL
	    apply_autocmds_retval(EVENT_STDINREADPOST, ଖାଲି, ଖାଲି, FALSE,
							      curbuf, &retval);
#ଅନ୍ୟଥା
	    apply_autocmds(EVENT_STDINREADPOST, ଖାଲି, ଖାଲି, FALSE, curbuf);
#endif
	}
    }
    ଫେରନ୍ତୁ retval;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Ensure buffer "buf" is loaded.  Does not trigger the swap-exists action.
 */
    ଶୂନ୍ୟ
buffer_ensure_loaded(buf_T *buf)
{
    ଯଦି (buf->b_ml.ml_mfp != ଖାଲି)
	ଫେରନ୍ତୁ;

    aco_save_T	aco;

    // Make sure the buffer is in a window.  If not then skip it.
    aucmd_prepbuf(&aco, buf);
    ଯଦି (curbuf == buf)
    {
	ଯଦି (swap_exists_action != SEA_READONLY)
	    swap_exists_action = SEA_NONE;
	open_buffer(FALSE, ଖାଲି, 0);
	aucmd_restbuf(&aco);
    }
}
#endif

/*
 * Open current buffer, that is: open the memfile and read the file into
 * memory.
 * Return FAIL for failure, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
open_buffer(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		read_stdin,	    // read file from stdin
    exarg_T	*eap,		    // for forced 'ff' and 'fenc' or NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags_arg)	    // extra flags for readfile()
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags = flags_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    bufref_T	old_curbuf;
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		read_fifo = FALSE;

    // The 'readonly' flag is only set when BF_NEVERLOADED is being reset.
    // When re-entering the same buffer, it should not change, because the
    // user may have reset the flag by hand.
    ଯଦି (readonlymode && curbuf->b_ffname != ଖାଲି
					&& (curbuf->b_flags & BF_NEVERLOADED))
	curbuf->b_p_ro = TRUE;

    ଯଦି (ml_open(curbuf) == FAIL)
    {
	// There MUST be a memfile, otherwise we can't do anything
	// If we can't create one for the current buffer, take another buffer
	close_buffer(curwin, curbuf, 0, FALSE, FALSE, FALSE);
	FOR_ALL_BUFFERS(curbuf)
	    ଯଦି (curbuf->b_ml.ml_mfp != ଖାଲି)
		ଭାଙ୍ଗ;
	// If there is no memfile at all, exit.
	// This is OK, since there are no changes to lose.
	ଯଦି (curbuf == ଖାଲି)
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
	ଯଦି (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ଖାଲି, curwin);
#endif
	ଫେରନ୍ତୁ FAIL;
    }

    // Do not sync this buffer yet, may first want to read the file.
    ଯଦି (curbuf->b_ml.ml_mfp != ଖାଲି)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES_NOSYNC;

    // The autocommands in readfile() may change the buffer, but only AFTER
    // reading the file.
    set_bufref(&old_curbuf, curbuf);
    curbuf->b_modified_was_set = false;

    // mark cursor position as being invalid
    curwin->w_valid = 0;

    // A buffer without an actual file should not use the buffer name to read a
    // file.
    ଯଦି (bt_nofileread(curbuf))
	flags |= READ_NOFILE;

    // Read the file if there is one.
    ଯଦି (curbuf->b_ffname != ଖାଲି
#ifdef FEAT_NETBEANS_INTG
	    && netbeansReadFile
#endif
       )
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_ସନ୍ଦେଶ_silent = ସନ୍ଦେଶ_silent;
#ifdef UNIX
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_bin = curbuf->b_p_bin;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା perm;
#endif
#ifdef FEAT_NETBEANS_INTG
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldFire = netbeansFireChanges;

	netbeansFireChanges = 0;
#endif
#ifdef UNIX
	perm = mch_getperm(curbuf->b_ffname);
	ଯଦି (perm >= 0 && (S_ISFIFO(perm)
		      || S_ISSOCK(perm)
# ifdef OPEN_CHR_FILES
		      || (S_ISCHR(perm) && is_dev_fd_file(curbuf->b_ffname))
# endif
		    ))
		read_fifo = TRUE;
	ଯଦି (read_fifo)
	    curbuf->b_p_bin = TRUE;
#endif
	ଯଦି (shortmess(SHM_FILEINFO))
	    ସନ୍ଦେଶ_silent = 1;
	retval = readfile(curbuf->b_ffname, curbuf->b_fname,
		  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM, eap,
		  flags | READ_NEW | (read_fifo ? READ_FIFO : 0));
#ifdef UNIX
	ଯଦି (read_fifo)
	{
	    curbuf->b_p_bin = save_bin;
	    ଯଦି (retval == OK)
		// don't add READ_FIFO here, otherwise we won't be able to
		// detect the encoding
		retval = read_buffer(FALSE, eap, flags);
	}
#endif
	ସନ୍ଦେଶ_silent = old_ସନ୍ଦେଶ_silent;
#ifdef FEAT_NETBEANS_INTG
	netbeansFireChanges = oldFire;
#endif
	// Help buffer is filtered.
	ଯଦି (bt_help(curbuf))
	    fix_help_buffer();
    }
    ଅନ୍ୟଥା ଯଦି (read_stdin)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	save_bin = curbuf->b_p_bin;

	// First read the text in binary mode into the buffer.
	// Then read from that same buffer and append at the end.  This makes
	// it possible to retry when 'fileformat' or 'fileencoding' was
	// guessed wrong.
	curbuf->b_p_bin = TRUE;
	retval = readfile(ଖାଲି, ଖାଲି, (linenr_T)0,
		  (linenr_T)0, (linenr_T)MAXLNUM, ଖାଲି,
		  flags | (READ_NEW + READ_STDIN));
	curbuf->b_p_bin = save_bin;
	ଯଦି (retval == OK)
	    retval = read_buffer(TRUE, eap, flags);
    }

    // Can now sync this buffer in ml_sync_all().
    ଯଦି (curbuf->b_ml.ml_mfp != ଖାଲି
	    && curbuf->b_ml.ml_mfp->mf_dirty == MF_DIRTY_YES_NOSYNC)
	curbuf->b_ml.ml_mfp->mf_dirty = MF_DIRTY_YES;

    // if first time loading this buffer, init b_chartab[]
    ଯଦି (curbuf->b_flags & BF_NEVERLOADED)
    {
	(ଶୂନ୍ୟ)buf_init_chartab(curbuf, FALSE);
	parse_cino(curbuf);
    }

    // Set/reset the Changed flag first, autocmds may change the buffer.
    // Apply the automatic commands, before processing the modelines.
    // So the modelines have priority over autocommands.
    //
    // When reading stdin, the buffer contents always needs writing, so set
    // the changed flag.  Unless in readonly mode: "ls | gview -".
    // When interrupted and 'cpoptions' contains 'i' set changed flag.
    ଯଦି ((got_int && vim_strchr(p_cpo, CPO_INTMOD) != ଖାଲି)
		|| curbuf->b_modified_was_set	// autocmd did ":set modified"
#ifdef FEAT_EVAL
		|| (aborting() && vim_strchr(p_cpo, CPO_INTMOD) != ଖାଲି)
#endif
       )
	changed();
    ଅନ୍ୟଥା ଯଦି (retval == OK && !read_stdin && !read_fifo)
	unchanged(curbuf, FALSE, TRUE);
    save_file_ff(curbuf);		// keep this fileformat

    // Set last_changedtick to avoid triggering a TextChanged autocommand right
    // after it was added.
    curbuf->b_last_changedtick = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_i = CHANGEDTICK(curbuf);
    curbuf->b_last_changedtick_pum = CHANGEDTICK(curbuf);

    // require "!" to overwrite the file, because it wasn't read completely
#ifdef FEAT_EVAL
    ଯଦି (aborting())
#ଅନ୍ୟଥା
    ଯଦି (got_int)
#endif
	curbuf->b_flags |= BF_READERR;

#ifdef FEAT_FOLDING
    // Need to update automatic folding.  Do this before the autocommands,
    // they may use the fold info.
    foldUpdateAll(curwin);
#endif

    // need to set w_topline, unless some autocommand already did that.
    ଯଦି (!(curwin->w_valid & VALID_TOPLINE))
    {
	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
    }
#ifdef FEAT_EVAL
    apply_autocmds_retval(EVENT_BUFENTER, ଖାଲି, ଖାଲି, FALSE, curbuf, &retval);
#ଅନ୍ୟଥା
    apply_autocmds(EVENT_BUFENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
#endif

    ଯଦି (retval != OK)
	ଫେରନ୍ତୁ retval;

    // The autocommands may have changed the current buffer.  Apply the
    // modelines to the correct buffer, if it still exists and is loaded.
    ଯଦି (bufref_valid(&old_curbuf) && old_curbuf.br_buf->b_ml.ml_mfp != ଖାଲି)
    {
	aco_save_T	aco;

	// Go to the buffer that was opened, make sure it is in a window.
	// If not then skip it.
	aucmd_prepbuf(&aco, old_curbuf.br_buf);
	ଯଦି (curbuf == old_curbuf.br_buf)
	{
	    do_modelines(0);
	    curbuf->b_flags &= ~(BF_CHECK_RO | BF_NEVERLOADED);

	    ଯଦି ((flags & READ_NOWINENTER) == 0)
#ifdef FEAT_EVAL
		apply_autocmds_retval(EVENT_BUFWINENTER, ଖାଲି, ଖାଲି,
			FALSE, curbuf, &retval);
#ଅନ୍ୟଥା
	    apply_autocmds(EVENT_BUFWINENTER, ଖାଲି, ଖାଲି,
		    FALSE, curbuf);
#endif

	    // restore curwin/curbuf and a few other things
	    aucmd_restbuf(&aco);
	}
    }

    ଫେରନ୍ତୁ retval;
}

/*
 * Store "buf" in "bufref" and set the free count.
 */
    ଶୂନ୍ୟ
set_bufref(bufref_T *bufref, buf_T *buf)
{
    bufref->br_buf = buf;
    bufref->br_fnum = buf == ଖାଲି ? 0 : buf->b_fnum;
    bufref->br_buf_free_ଗଣନା = buf_free_ଗଣନା;
}

/*
 * Return TRUE if "bufref->br_buf" points to the same buffer as when
 * set_bufref() was called and it is a valid buffer.
 * Only goes through the buffer list if buf_free_count changed.
 * Also checks if b_fnum is still the same, a :bwipe followed by :new might get
 * the same allocated memory, but it's a different buffer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bufref_valid(bufref_T *bufref)
{
    ଫେରନ୍ତୁ bufref->br_buf_free_ଗଣନା == buf_free_ଗଣନା
	? TRUE : buf_valid(bufref->br_buf)
				  && bufref->br_fnum == bufref->br_buf->b_fnum;
}

/*
 * Return TRUE if "buf" points to a valid buffer (in the buffer list).
 * This can be slow if there are many buffers, prefer using bufref_valid().
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_valid(buf_T *buf)
{
    buf_T	*bp;

    // Assume that we more often have a recent buffer, start with the last
    // one.
    FOR_ALL_BUFS_FROM_LAST(bp)
	ଯଦି (bp == buf)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

/*
 * A hash table used to quickly lookup a buffer by its number.
 */
ସ୍ଥିର hashtab_T buf_hashtab;

    ସ୍ଥିର ଶୂନ୍ୟ
buf_hashtab_add(buf_T *buf)
{
    vim_snprintf((ଅକ୍ଷର *)buf->b_ଚାବି, ଆକାର(buf->b_ଚାବି), "%x", buf->b_fnum);
    ଯଦି (hash_add(&buf_hashtab, buf->b_ଚାବି, "create buffer") == FAIL)
	emsg(_(e_buffer_cannot_be_registered));
}

    ସ୍ଥିର ଶୂନ୍ୟ
buf_hashtab_remove(buf_T *buf)
{
    hashitem_T *hi = hash_find(&buf_hashtab, buf->b_ଚାବି);

    ଯଦି (!HASHITEM_EMPTY(hi))
	hash_remove(&buf_hashtab, hi, "close buffer");
}

/*
 * Return TRUE when buffer "buf" can be unloaded.
 * Give an error message and return FALSE when the buffer is locked or the
 * screen is being redrawn and the buffer is in a window.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
can_unload_buffer(buf_T *buf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    can_unload = !buf->b_locked;

    ଯଦି (can_unload && updating_screen)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	    ଯଦି (wp->w_buffer == buf)
	    {
		can_unload = FALSE;
		ଭାଙ୍ଗ;
	    }
    }
    // Don't unload the buffer while it's still being saved
    ଯଦି (can_unload && buf->b_saving)
	can_unload = FALSE;

    ଯଦି (!can_unload)
    {
	char_u *fname = buf->b_fname != ଖାଲି ? buf->b_fname : buf->b_ffname;

	semsg(_(e_attempt_to_delete_buffer_that_is_in_use_str),
				fname != ଖାଲି ? fname : (char_u *)"[No Name]");
    }
    ଫେରନ୍ତୁ can_unload;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
close_buffer(
    win_T	*win,
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		action,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort_if_last,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ignore_abort,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		set_context)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hiding_buf;
    bufref_T	bufref;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_curwin = (curwin != ଖାଲି && curwin == win
				&& curwin->w_buffer == buf);
    tabpage_T	*the_curtab = curtab;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unload_buf = (action != 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wipe_buf = (action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		del_buf = (action == DOBUF_DEL || wipe_buf);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		win_valid = win_valid_any_tab(win);
    // win_valid is FALSE for closing popups, as they're first removed from the
    // window list.  If so, skip some side-effects, but treat win as valid.
    // Shouldn't be possible for autocmds to prematurely free win then, as it's
    // not in the window list.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		closed_popup = win && !win_valid && WIN_IS_POPUP(win);

    CHECK_CURBUF;

    // Force unloading or deleting when 'bufhidden' says so.
    // The caller must take care of NOT deleting/freeing when 'bufhidden' is
    // "hide" (otherwise we could never free or delete a buffer).
    ଯଦି (buf->b_p_bh[0] == 'd')		// 'bufhidden' == "delete"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (buf->b_p_bh[0] == 'w')	// 'bufhidden' == "wipe"
    {
	del_buf = TRUE;
	unload_buf = TRUE;
	wipe_buf = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (buf->b_p_bh[0] == 'u')	// 'bufhidden' == "unload"
	unload_buf = TRUE;

#ifdef FEAT_TERMINAL
    // depending on how we get here b_nwindows may already be zero
    ଯଦି (bt_terminal(buf) && (buf->b_nwindows <= 1 || del_buf))
    {
	CHECK_CURBUF;
	ଯଦି (term_job_running(buf->b_term))
	{
	    ଯଦି (wipe_buf || unload_buf)
	    {
		ଯଦି (!can_unload_buffer(buf))
		    ଫେରନ୍ତୁ FALSE;

		// Wiping out or unloading a terminal buffer kills the job.
		free_terminal(buf);

		// A terminal buffer is wiped out when job has finished.
		del_buf = TRUE;
		unload_buf = TRUE;
		wipe_buf = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		// The job keeps running, hide the buffer.
		del_buf = FALSE;
		unload_buf = FALSE;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (buf->b_p_bh[0] == 'h' && !del_buf)
	{
	    // Hide a terminal buffer.
	    unload_buf = FALSE;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (del_buf || unload_buf)
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
    ଯଦି ((del_buf || wipe_buf) && !can_unload_buffer(buf))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (set_context && win_valid && win->w_buffer == buf)
    {
	// Set b_last_cursor when closing the last window for the buffer.
	// Remember the last cursor position and window options of the buffer.
	// This used to be only for the current window, but then options like
	// 'foldmethod' may be lost with a ":only" command.
	ଯଦି (buf->b_nwindows == 1)
	    set_last_cursor(win);
	buflist_setfpos(buf, win,
		    win->w_cursor.lnum == 1 ? 0 : win->w_cursor.lnum,
		    win->w_cursor.col, TRUE);
    }

    set_bufref(&bufref, buf);

    // When the buffer is no longer in a window, trigger BufWinLeave
    ଯଦି ((win_valid || closed_popup) && win->w_buffer == buf
	    && buf->b_nwindows == 1)
    {
	++buf->b_locked;
	++buf->b_locked_split;
	ଯଦି (apply_autocmds(EVENT_BUFWINLEAVE, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	{
	    // Autocommands deleted the buffer.
aucmd_abort:
	    emsg(_(e_autocommands_caused_command_to_abort));
	    ଫେରନ୍ତୁ FALSE;
	}
	--buf->b_locked;
	--buf->b_locked_split;
	ଯଦି (abort_if_last && one_window())
	    // Autocommands made this the only window.
	    ଯାଅ aucmd_abort;

	// When the buffer becomes hidden, but is not unloaded, trigger
	// BufHidden
	ଯଦି (!unload_buf)
	{
	    ++buf->b_locked;
	    ++buf->b_locked_split;
	    ଯଦି (apply_autocmds(EVENT_BUFHIDDEN, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		    && !bufref_valid(&bufref))
		// Autocommands deleted the buffer.
		ଯାଅ aucmd_abort;
	    --buf->b_locked;
	    --buf->b_locked_split;
	    ଯଦି (abort_if_last && one_window())
		// Autocommands made this the only window.
		ଯାଅ aucmd_abort;
	}
#ifdef FEAT_EVAL
	// autocmds may abort script processing
	ଯଦି (!ignore_abort && aborting())
	    ଫେରନ୍ତୁ FALSE;
#endif
	win_valid = win_valid && win_valid_any_tab(win);
    }

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ଯଦି (is_curwin && curwin != win && win_valid)
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, win);
	unblock_autocmds();
    }

    // When a quickfix buffer is deleted from a window, clear the
    // 'winfixheight' option.
    ଯଦି (bt_quickfix(buf) && win_valid && win->w_buffer == buf)
	win->w_p_wfh = FALSE;

    // Remember if the buffer may be hidden soon, or is already hidden.
    hiding_buf = buf->b_nwindows <= 0 || ((win_valid || closed_popup)
	    && win->w_buffer == buf && buf->b_nwindows == 1);

#ifdef FEAT_DIFF
    ଯଦି (diffopt_hiddenoff() && !unload_buf && hiding_buf)
	diff_buf_delete(buf);	// Clear 'diff' for hidden buffer.
#endif

    // Return when another window is displaying the buffer or when not unloaded.
    ଯଦି (!hiding_buf || !unload_buf)
	ଫେରନ୍ତୁ FALSE;

    // Always remove the buffer when there is no file name.
    ଯଦି (buf->b_ffname == ଖାଲି)
	del_buf = TRUE;

    // Free all things allocated for this buffer.
    // Also calls the "BufDelete" autocommands when del_buf is TRUE.
    // Abort if nothing was freed, or if autocommands delete the buffer.
    ଯଦି (!buf_freeall(buf, (del_buf ? BFA_DEL : 0)
		   + (wipe_buf ? BFA_WIPE : 0)
		   + (ignore_abort ? BFA_IGNORE_ABORT : 0)))
	ଫେରନ୍ତୁ FALSE;

    win_valid = win_valid && win_valid_any_tab(win);
    ଯଦି ((win_valid || closed_popup) && win->w_buffer == buf)
    {
	// Autocommands may have opened (despite b_locked_split) or closed
	// windows for this buffer.  Decrement for the close we do here.
	--buf->b_nwindows;
	win->w_buffer = ଖାଲି;  // make sure we don't use the buffer now
    }

    /*
     * Remove the buffer from the list.
     * Do not wipe out the buffer if it is used in a window, or if autocommands
     * wiped out all other buffers (unless when inside free_all_mem() where all
     * buffers need to be freed and autocommands are blocked).
     */
    ଯଦି (wipe_buf && buf->b_nwindows <= 0
			    && (buf->b_prev != ଖାଲି || buf->b_next != ଖାଲି
#ଯଦି defined(EXITFREE)
				|| entered_free_all_mem
#endif
				))
    {
	tabpage_T	*tp;
	win_T		*wp;

	FOR_ALL_TAB_WINDOWS(tp, wp)
	    mark_forget_file(wp, buf->b_fnum);

	ଯଦି (action == DOBUF_WIPE_REUSE)
	{
	    // we can re-use this buffer number, store it
	    ଯଦି (buf_reuse.ga_itemsize == 0)
		ga_init2(&buf_reuse, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), 50);
	    ଯଦି (ga_grow(&buf_reuse, 1) == OK)
		((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)buf_reuse.ga_data)[buf_reuse.ga_len++] = buf->b_fnum;
	}
	ଯଦି (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ଅନ୍ୟଥା
	    buf->b_sfname = ଖାଲି;
	VIM_CLEAR(buf->b_ffname);
	ଯଦି (buf->b_prev == ଖାଲି)
	    firstbuf = buf->b_next;
	ଅନ୍ୟଥା
	    buf->b_prev->b_next = buf->b_next;
	ଯଦି (buf->b_next == ଖାଲି)
	    lastbuf = buf->b_prev;
	ଅନ୍ୟଥା
	    buf->b_next->b_prev = buf->b_prev;
	free_buffer(buf);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (del_buf)
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
	ଯଦି (del_buf)
	    buf->b_p_bl = FALSE;
    }
    // NOTE: at this point "curbuf" may be invalid!

    // When closing curbuf for curwin, is_curwin checks should've ensured
    // autocmds don't switch windows, unless they closed curwin.  Otherwise
    // callers may leave the window open to a NULL buffer!
    ପ୍ରତିଜ୍ଞା(!win_valid || !is_curwin || win == curwin);
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Make buffer not contain a file.
 */
    ଶୂନ୍ୟ
buf_clear_file(buf_T *buf)
{
    buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା = 1;
    unchanged(buf, TRUE, TRUE);
    buf->b_shortname = false;
    buf->b_p_eof = FALSE;
    buf->b_start_eof = FALSE;
    buf->b_p_eol = TRUE;
    buf->b_start_eol = TRUE;
    buf->b_p_bomb = FALSE;
    buf->b_start_bomb = FALSE;
    buf->b_ml.ml_mfp = ଖାଲି;
    buf->b_ml.ml_flags = ML_EMPTY;		// empty buffer
#ifdef FEAT_NETBEANS_INTG
    netbeans_deleted_all_ଧାଡ଼ିଗୁଡ଼ିକ(buf);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_freeall(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_curbuf = (buf == curbuf);
    bufref_T	bufref;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_curwin = (curwin != ଖାଲି && curwin->w_buffer == buf);
    win_T	*the_curwin = curwin;
    tabpage_T	*the_curtab = curtab;

    // Make sure the buffer isn't closed by autocommands.
    ++buf->b_locked;
    ++buf->b_locked_split;
    set_bufref(&bufref, buf);
    ଯଦି (buf->b_ml.ml_mfp != ଖାଲି)
    {
	ଯଦି (apply_autocmds(EVENT_BUFUNLOAD, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ଫେରନ୍ତୁ FALSE;
    }
    ଯଦି ((flags & BFA_DEL) && buf->b_p_bl)
    {
	ଯଦି (apply_autocmds(EVENT_BUFDELETE, buf->b_fname, buf->b_fname,
								   FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ଫେରନ୍ତୁ FALSE;
    }
    ଯଦି (flags & BFA_WIPE)
    {
	ଯଦି (apply_autocmds(EVENT_BUFWIPEOUT, buf->b_fname, buf->b_fname,
								  FALSE, buf)
		&& !bufref_valid(&bufref))
	    // autocommands deleted the buffer
	    ଫେରନ୍ତୁ FALSE;
    }
    --buf->b_locked;
    --buf->b_locked_split;

    // If the buffer was in curwin and the window has changed, go back to that
    // window, if it still exists.  This avoids that ":edit x" triggering a
    // "tabnext" BufUnload autocmd leaves a window behind without a buffer.
    ଯଦି (is_curwin && curwin != the_curwin && win_valid_any_tab(the_curwin))
    {
	block_autocmds();
	goto_tabpage_win(the_curtab, the_curwin);
	unblock_autocmds();
    }

#ifdef FEAT_EVAL
    // autocmds may abort script processing
    ଯଦି ((flags & BFA_IGNORE_ABORT) == 0 && aborting())
	ଫେରନ୍ତୁ FALSE;
#endif

    // It's possible that autocommands change curbuf to the one being deleted.
    // This might cause curbuf to be deleted unexpectedly.  But in some cases
    // it's OK to delete the curbuf, because a new one is obtained anyway.
    // Therefore only return if curbuf changed to the deleted buffer.
    ଯଦି (buf == curbuf && !is_curbuf)
	ଫେରନ୍ତୁ FALSE;

    // If curbuf, stop Visual mode just before freeing, but after autocmds that
    // may restart it.  May trigger TextYankPost, but with textlock set.
    ଯଦି (buf == curbuf && VIsual_active
#ଯଦି defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

#ifdef FEAT_DIFF
    diff_buf_delete(buf);	    // Can't use 'diff' for unloaded buffer.
#endif
#ifdef FEAT_SYN_HL
    // Remove any ownsyntax, unless exiting.
    ଯଦି (curwin != ଖାଲି && curwin->w_buffer == buf)
	reset_synblock(curwin);
#endif

#ifdef FEAT_FOLDING
    // No folds in an empty buffer.
    {
	win_T		*win;
	tabpage_T	*tp;

	FOR_ALL_TAB_WINDOWS(tp, win)
	    ଯଦି (win->w_buffer == buf)
		clearFolding(win);
    }
#endif

#ifdef FEAT_TCL
    tcl_buffer_free(buf);
#endif
    ml_close(buf, TRUE);	    // close and delete the memline/memfile
    buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା = 0;    // no lines in buffer
    ଯଦି ((flags & BFA_KEEP_UNDO) == 0)
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
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Free a buffer structure and the things it contains related to the buffer
 * itself (not the file, that must have been done already).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_buffer(buf_T *buf)
{
    ++buf_free_ଗଣନା;
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

    ଯଦି (autocmd_busy)
    {
	// Do not free the buffer structure while autocommands are executing,
	// it's still needed. Free it when autocmd_busy is reset.
	buf->b_next = au_pending_free_buf;
	au_pending_free_buf = buf;
    }
    ଅନ୍ୟଥା
    {
	vim_free(buf);
	ଯଦି (curbuf == buf)
	    curbuf = ଖାଲି;  // make clear it's not to be used
    }
}

/*
 * Initializes b:changedtick.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
init_changedtick(buf_T *buf)
{
    dictitem_T *di = (dictitem_T *)&buf->b_ct_di;

    di->di_flags = DI_FLAGS_FIX | DI_FLAGS_RO;
    di->di_tv.v_type = VAR_ସଂଖ୍ୟା;
    di->di_tv.v_lock = VAR_FIXED;
    di->di_tv.vval.v_ସଂଖ୍ୟା = 0;

#ifdef FEAT_EVAL
    STRCPY(buf->b_ct_di.di_ଚାବି, "changedtick");
    (ଶୂନ୍ୟ)dict_add(buf->b_vars, di);
#endif
}

/*
 * Free the b_wininfo list for buffer "buf".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clear_wininfo(buf_T *buf)
{
    wininfo_T	*wip;

    ଯେତେବେଳେ (buf->b_wininfo != ଖାଲି)
    {
	wip = buf->b_wininfo;
	buf->b_wininfo = wip->wi_next;
	free_wininfo(wip);
    }
}

/*
 * Free stuff in the buffer for ":bdel" and when wiping out the buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
free_buffer_stuff(
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		free_options)		// free options as well
{
    ଯଦି (free_options)
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
    ଶୂନ୍ୟ
free_wininfo(wininfo_T *wip)
{
    ଯଦି (wip->wi_optset)
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
    ଶୂନ୍ୟ
goto_buffer(
    exarg_T	*eap,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2)
{
    bufref_T	old_curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_sea = swap_exists_action;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_help_buf;

    ବାଛ (eap->cmdidx)
    {
	ସ୍ଥିତି CMD_bnext:
	ସ୍ଥିତି CMD_sbnext:
	ସ୍ଥିତି CMD_bNext:
	ସ୍ଥିତି CMD_bprevious:
	ସ୍ଥିତି CMD_sbNext:
	ସ୍ଥିତି CMD_sbprevious:
	    skip_help_buf = TRUE;
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    skip_help_buf = FALSE;
	    ଭାଙ୍ଗ;
    }

    set_bufref(&old_curbuf, curbuf);

    ଯଦି (swap_exists_action == SEA_NONE)
	swap_exists_action = SEA_DIALOG;
    (ଶୂନ୍ୟ)do_buffer_ext(*eap->cmd == 's' ? DOBUF_SPLIT : DOBUF_GOTO, start, dir, ଗଣନା_2,
	    (eap->forceit ? DOBUF_FORCEIT : 0) |
	    (skip_help_buf ? DOBUF_SKIPHELP : 0));
    ଯଦି (swap_exists_action == SEA_QUIT && *eap->cmd == 's')
    {
#ଯଦି defined(FEAT_EVAL)
	cleanup_T   cs;

	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a window.
	enter_cleanup(&cs);
#endif

	// Quitting means closing the split window, nothing else.
	win_close(curwin, TRUE);
	swap_exists_action = save_sea;
	swap_exists_did_quit = TRUE;

#ଯଦି defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    ଅନ୍ୟଥା
	handle_swap_exists(&old_curbuf);
}

/*
 * Handle the situation of swap_exists_action being set.
 * It is allowed for "old_curbuf" to be NULL or invalid.
 */
    ଶୂନ୍ୟ
handle_swap_exists(bufref_T *old_curbuf)
{
#ଯଦି defined(FEAT_EVAL)
    cleanup_T	cs;
#endif
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    buf_T	*buf;

    ଯଦି (swap_exists_action == SEA_QUIT)
    {
#ଯଦି defined(FEAT_EVAL)
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
	ଯଦି (old_curbuf == ଖାଲି || !bufref_valid(old_curbuf)
					      || old_curbuf->br_buf == curbuf)
	{
	    // Block autocommands here because curwin->w_buffer may be NULL.
	    block_autocmds();
	    buf = buflist_new(ଖାଲି, ଖାଲି, 1L, BLN_CURBUF | BLN_LISTED);
	    unblock_autocmds();
	}
	ଅନ୍ୟଥା
	    buf = old_curbuf->br_buf;
	ଯଦି (buf != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_ସନ୍ଦେଶ_silent = ସନ୍ଦେଶ_silent;

	    ଯଦି (shortmess(SHM_FILEINFO))
		ସନ୍ଦେଶ_silent = 1;  // prevent fileinfo message
	    enter_buffer(buf);
	    // restore msg_silent, so that the command line will be shown
	    ସନ୍ଦେଶ_silent = old_ସନ୍ଦେଶ_silent;

#ifdef FEAT_SYN_HL
	    ଯଦି (old_tw != curbuf->b_p_tw)
		check_colorcolumn(ଖାଲି, curwin);
#endif
	}
	// If "old_curbuf" is NULL we are in big trouble here...

#ଯଦି defined(FEAT_EVAL)
	// Restore the error/interrupt/exception state if not discarded by a
	// new aborting error, interrupt, or uncaught exception.
	leave_cleanup(&cs);
#endif
    }
    ଅନ୍ୟଥା ଯଦି (swap_exists_action == SEA_RECOVER)
    {
#ଯଦି defined(FEAT_EVAL)
	// Reset the error/interrupt/exception state here so that
	// aborting() returns FALSE when closing a buffer.
	enter_cleanup(&cs);
#endif

	// User selected Recover at ATTENTION prompt.
	ସନ୍ଦେଶ_scroll = TRUE;
	ml_recover(FALSE);
	ସନ୍ଦେଶ_puts("\n");	// don't overwrite the last message
	cmdline_row = ସନ୍ଦେଶ_row;
	do_modelines(0);

#ଯଦି defined(FEAT_EVAL)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
empty_curbuf(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା close_others,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା action)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval;
    buf_T   *buf = curbuf;
    bufref_T bufref;

    ଯଦି (action == DOBUF_UNLOAD)
    {
	emsg(_(e_cannot_unload_last_buffer));
	ଫେରନ୍ତୁ FAIL;
    }

    set_bufref(&bufref, buf);
    ଯଦି (close_others)
	// Close any other windows on this buffer, then make it empty.
	close_windows(buf, TRUE);

    setpcmark();
    retval = do_ecmd(0, ଖାଲି, ଖାଲି, ଖାଲି, ECMD_ONE,
					  forceit ? ECMD_FORCEIT : 0, curwin);

    // do_ecmd() may create a new buffer, then we have to delete
    // the old one.  But do_ecmd() may have done that already, check
    // if the buffer still exists.
    ଯଦି (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows == 0)
	close_buffer(ଖାଲି, buf, action, FALSE, FALSE, FALSE);
    ଯଦି (!close_others)
	need_fileinfo = FALSE;
    ଅନ୍ୟଥା ଯଦି (retval == OK && !shortmess(SHM_FILEINFO))
	// do_ecmd() does not display file info for a new empty buffer.
	need_fileinfo = TRUE;
    ଫେରନ୍ତୁ retval;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_buffer_ext(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		action,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,		// FORWARD or BACKWARD
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2,		// buffer number or number of buffers
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		// DOBUF_FORCEIT etc.
{
    buf_T	*buf;
    buf_T	*bp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);

    ବାଛ (start)
    {
	ସ୍ଥିତି DOBUF_FIRST:   buf = firstbuf; ଭାଙ୍ଗ;
	ସ୍ଥିତି DOBUF_LAST:    buf = lastbuf;  ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	    buf = curbuf;   ଭାଙ୍ଗ;
    }
    ଯଦି (start == DOBUF_MOD)	    // find next modified buffer
    {
	ଯେତେବେଳେ (ଗଣନା_2-- > 0)
	{
	    କର
	    {
		buf = buf->b_next;
		ଯଦି (buf == ଖାଲି)
		    buf = firstbuf;
	    }
	    ଯେତେବେଳେ (buf != curbuf && !bufIsChanged(buf));
	}
	ଯଦି (!bufIsChanged(buf))
	{
	    emsg(_(e_no_modified_buffer_found));
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଅନ୍ୟଥା ଯଦି (start == DOBUF_FIRST && ଗଣନା_2) // find specified buffer number
    {
	ଯେତେବେଳେ (buf != ଖାଲି && buf->b_fnum != ଗଣନା_2)
	    buf = buf->b_next;
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା help_only = (flags & DOBUF_SKIPHELP) != 0 && buf->b_help;

	bp = ଖାଲି;
	ଯେତେବେଳେ (ଗଣନା_2 > 0 || (bp != buf && !unload
				&& !(help_only ? buf->b_help : buf->b_p_bl)))
	{
	    // remember the buffer where we start, we come back there when all
	    // buffers are unlisted.
	    ଯଦି (bp == ଖାଲି)
		bp = buf;
	    ଯଦି (dir == FORWARD)
	    {
		buf = buf->b_next;
		ଯଦି (buf == ଖାଲି)
		    buf = firstbuf;
	    }
	    ଅନ୍ୟଥା
	    {
		buf = buf->b_prev;
		ଯଦି (buf == ଖାଲି)
		    buf = lastbuf;
	    }
	    // Avoid non-help buffers if the starting point was a help buffer
	    // and vice-versa.
	    // Don't count unlisted buffers.
	    ଯଦି (unload
		    || (help_only
			? buf->b_help
			: (buf->b_p_bl && ((flags & DOBUF_SKIPHELP) == 0
							    || !buf->b_help))))
	    {
		--ଗଣନା_2;
		bp = ଖାଲି;	// use this buffer as new starting point
	    }
	    ଯଦି (bp == buf)
	    {
		// back where we started, didn't find anything.
		emsg(_(e_there_is_no_listed_buffer));
		ଫେରନ୍ତୁ FAIL;
	    }
	}
    }

    ଯଦି (buf == ଖାଲି)	    // could not find it
    {
	ଯଦି (start == DOBUF_FIRST)
	{
	    // don't warn when deleting
	    ଯଦି (!unload)
		semsg(_(e_buffer_nr_does_not_exist), ଗଣନା_2);
	}
	ଅନ୍ୟଥା ଯଦି (dir == FORWARD)
	    emsg(_(e_cannot_go_beyond_last_buffer));
	ଅନ୍ୟଥା
	    emsg(_(e_cannot_go_before_first_buffer));
	ଫେରନ୍ତୁ FAIL;
    }
#ifdef FEAT_PROP_POPUP
    ଯଦି ((flags & DOBUF_NOPOPUP) && bt_popup(buf) && !bt_terminal(buf))
	ଫେରନ୍ତୁ OK;
#endif
    ଯଦି (action == DOBUF_GOTO && buf != curbuf
	    && !check_can_set_curbuf_forceit((flags & DOBUF_FORCEIT) != 0))
	// disallow navigating to another buffer when 'winfixbuf' is applied
	ଫେରନ୍ତୁ FAIL;

    ଯଦି ((action == DOBUF_GOTO || action == DOBUF_SPLIT)
						  && (buf->b_flags & BF_DUMMY))
    {
	// disallow navigating to the dummy buffer
	semsg(_(e_buffer_nr_does_not_exist), ଗଣନା_2);
	ଫେରନ୍ତୁ FAIL;
    }

#ifdef FEAT_GUI
    need_mouse_correct = TRUE;
#endif

    /*
     * delete buffer "buf" from memory and/or the list
     */
    ଯଦି (unload)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	forward;
	bufref_T bufref;

	ଯଦି (!can_unload_buffer(buf))
	    ଫେରନ୍ତୁ FAIL;

	set_bufref(&bufref, buf);

	// When unloading or deleting a buffer that's already unloaded and
	// unlisted: fail silently.
	ଯଦି (action != DOBUF_WIPE && action != DOBUF_WIPE_REUSE
				   && buf->b_ml.ml_mfp == ଖାଲି && !buf->b_p_bl)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି ((flags & DOBUF_FORCEIT) == 0 && bufIsChanged(buf))
	{
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	    ଯଦି ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	    {
# ifdef FEAT_TERMINAL
		ଯଦି (term_job_running(buf->b_term))
		{
		    ଯଦି (term_confirm_stop(buf) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		}
		ଅନ୍ୟଥା
# endif
		{
		    dialog_changed(buf, FALSE);
		    ଯଦି (!bufref_valid(&bufref))
			// Autocommand deleted buffer, oops!  It's not changed
			// now.
			ଫେରନ୍ତୁ FAIL;
		    // If it's still changed fail silently, the dialog already
		    // mentioned why it fails.
		    ଯଦି (bufIsChanged(buf))
			ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		no_write_message_buf(buf);
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	// When closing the current buffer stop Visual mode.
	ଯଦି (buf == curbuf && VIsual_active)
	    end_visual_mode();

	// If deleting the last (listed) buffer, make it empty.
	// The last (listed) buffer cannot be unloaded.
	FOR_ALL_BUFFERS(bp)
	    ଯଦି (bp->b_p_bl && bp != buf)
		ଭାଙ୍ଗ;
	ଯଦି (bp == ଖାଲି && buf == curbuf)
	    ଫେରନ୍ତୁ empty_curbuf(TRUE, (flags & DOBUF_FORCEIT), action);

	// If the deleted buffer is the current one, close the current window
	// (unless it's the only window).  Repeat this so long as we end up in
	// a window with this buffer.
	ଯେତେବେଳେ (buf == curbuf
		   && !(win_locked(curwin) || curwin->w_buffer->b_locked > 0)
		   && (!ONE_WINDOW || first_tabpage->tp_next != ଖାଲି))
	{
	    ଯଦି (win_close(curwin, FALSE) == FAIL)
		ଭାଙ୍ଗ;
	}

	// If the buffer to be deleted is not the current one, delete it here.
	ଯଦି (buf != curbuf)
	{
	    close_windows(buf, FALSE);
	    ଯଦି (buf != curbuf && bufref_valid(&bufref) && buf->b_nwindows <= 0)
		    close_buffer(ଖାଲି, buf, action, FALSE, FALSE, FALSE);
	    ଫେରନ୍ତୁ OK;
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
	buf = ଖାଲି;	// selected buffer
	bp = ଖାଲି;	// used when no loaded buffer found
	ଯଦି (au_new_curbuf.br_buf != ଖାଲି && bufref_valid(&au_new_curbuf)
		&& !au_new_curbuf.br_buf->b_locked_split)
	    buf = au_new_curbuf.br_buf;
	ଅନ୍ୟଥା ଯଦି (curwin->w_jumplistlen > 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     jumpidx;

	    jumpidx = curwin->w_jumplistidx - 1;
	    ଯଦି (jumpidx < 0)
		jumpidx = curwin->w_jumplistlen - 1;

	    forward = jumpidx;
	    ଯେତେବେଳେ (jumpidx != curwin->w_jumplistidx)
	    {
		buf = buflist_findnr(curwin->w_jumplist[jumpidx].fmark.fnum);
		ଯଦି (buf != ଖାଲି)
		{
		    // Skip current and unlisted bufs.  Also skip a quickfix
		    // or closing buffer, it might be deleted soon.
		    ଯଦି (buf == curbuf || !buf->b_p_bl || bt_quickfix(buf)
			    || buf->b_locked_split)
			buf = ଖାଲି;
		    ଅନ୍ୟଥା ଯଦି (buf->b_ml.ml_mfp == ଖାଲି)
		    {
			// skip unloaded buf, but may keep it for later
			ଯଦି (bp == ଖାଲି)
			    bp = buf;
			buf = ଖାଲି;
		    }
		}
		ଯଦି (buf != ଖାଲି)   // found a valid buffer: stop searching
		    ଭାଙ୍ଗ;
		// advance to older entry in jump list
		ଯଦି (!jumpidx && curwin->w_jumplistidx == curwin->w_jumplistlen)
		    ଭାଙ୍ଗ;
		ଯଦି (--jumpidx < 0)
		    jumpidx = curwin->w_jumplistlen - 1;
		ଯଦି (jumpidx == forward)		// List exhausted for sure
		    ଭାଙ୍ଗ;
	    }
	}

	ଯଦି (buf == ଖାଲି)	// No previous buffer, Try 2'nd approach
	{
	    forward = TRUE;
	    buf = curbuf->b_next;
	    ପାଇଁ (;;)
	    {
		ଯଦି (buf == ଖାଲି)
		{
		    ଯଦି (!forward)	// tried both directions
			ଭାଙ୍ଗ;
		    buf = curbuf->b_prev;
		    forward = FALSE;
		    ଜାରି;
		}
		// in non-help buffer, try to skip help buffers, and vv
		ଯଦି (buf->b_help == curbuf->b_help && buf->b_p_bl
			    && !bt_quickfix(buf) && !buf->b_locked_split)
		{
		    ଯଦି (buf->b_ml.ml_mfp != ଖାଲି)   // found loaded buffer
			ଭାଙ୍ଗ;
		    ଯଦି (bp == ଖାଲି)	// remember unloaded buf for later
			bp = buf;
		}
		ଯଦି (forward)
		    buf = buf->b_next;
		ଅନ୍ୟଥା
		    buf = buf->b_prev;
	    }
	}
	ଯଦି (buf == ଖାଲି)	// No loaded buffer, use unloaded one
	    buf = bp;
	ଯଦି (buf == ଖାଲି)	// No loaded buffer, find listed one
	{
	    FOR_ALL_BUFFERS(buf)
		ଯଦି (buf->b_p_bl && buf != curbuf && !bt_quickfix(buf)
			&& !buf->b_locked_split)
		    ଭାଙ୍ଗ;
	}
	ଯଦି (buf == ଖାଲି)	// Still no buffer, just take one
	{
	    ଯଦି (curbuf->b_next != ଖାଲି)
		buf = curbuf->b_next;
	    ଅନ୍ୟଥା
		buf = curbuf->b_prev;
	    ଯଦି (bt_quickfix(buf)
		    || (buf != ଖାଲି && buf != curbuf && buf->b_locked_split))
		buf = ଖାଲି;
	}
    }

    ଯଦି (buf == ଖାଲି)
    {
	// Autocommands must have wiped out all other buffers.  Only option
	// now is to make the current buffer empty.
	ଫେରନ୍ତୁ empty_curbuf(FALSE, (flags & DOBUF_FORCEIT), action);
    }

    /*
     * make "buf" the current buffer
     */
    // If 'switchbuf' is set jump to the window containing "buf".
    ଯଦି (action == DOBUF_SPLIT && swbuf_goto_win_with_buf(buf) != ଖାଲି)
	ଫେରନ୍ତୁ OK;
    // Whether splitting or not, don't open a closing buffer in more windows.
    ଯଦି (buf != curbuf && buf->b_locked_split)
    {
	emsg(_(e_cannot_switch_to_a_closing_buffer));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (action == DOBUF_SPLIT && win_split(0, 0) == FAIL) // split window first
	ଫେରନ୍ତୁ FAIL;

    // go to current buffer - nothing to do
    ଯଦି (buf == curbuf)
	ଫେରନ୍ତୁ OK;

    // Check if the current buffer may be abandoned.
    ଯଦି (action == DOBUF_GOTO && !can_abandon(curbuf, (flags & DOBUF_FORCEIT)))
    {
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ଯଦି ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ଯଦି (term_job_running(curbuf->b_term))
	    {
		ଯଦି (term_confirm_stop(curbuf) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		// Manually kill the terminal here because this command will
		// hide it otherwise.
		free_terminal(curbuf);
	    }
	    ଅନ୍ୟଥା
# endif
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);
		dialog_changed(curbuf, FALSE);
		ଯଦି (!bufref_valid(&bufref))
		    // Autocommand deleted buffer, oops!
		    ଫେରନ୍ତୁ FAIL;

		ଯଦି (bufIsChanged(curbuf))
		{
		    no_write_message();
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	}
	ଅନ୍ୟଥା
#endif
	{
	    no_write_message();
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    // Go to the other buffer.
    set_curbuf(buf, action);

    ଯଦି (action == DOBUF_SPLIT)
	RESET_BINDING(curwin);	// reset 'scrollbind' and 'cursorbind'

#ଯଦି defined(FEAT_EVAL)
    ଯଦି (aborting())	    // autocmds may abort script processing
	ଫେରନ୍ତୁ FAIL;
#endif

    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_buffer(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		action,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir,		// FORWARD or BACKWARD
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2,		// buffer number or number of buffers
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit)	// TRUE when using !
{
    ଫେରନ୍ତୁ do_buffer_ext(action, start, dir, ଗଣନା_2,
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
    ଅକ୍ଷର *
do_bufdel(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		command,
    char_u	*arg,		// pointer to extra arguments
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		addr_ଗଣନା,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_bnr,	// first buffer number in a range
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_bnr,	// buffer nr or last buffer nr in a range
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_current = 0;	// delete current buffer?
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		deleted = 0;	// number of buffers deleted
    ଅକ୍ଷର	*errormsg = ଖାଲି; // return value
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bnr;		// buffer number
    char_u	*p;

    ଯଦି (addr_ଗଣନା == 0)
    {
	(ଶୂନ୍ୟ)do_buffer(command, DOBUF_CURRENT, FORWARD, 0, forceit);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (addr_ଗଣନା == 2)
	{
	    ଯଦି (*arg)		// both range and argument is not allowed
		ଫେରନ୍ତୁ ex_errmsg(e_trailing_characters_str, arg);
	    bnr = start_bnr;
	}
	ଅନ୍ୟଥା	// addr_count == 1
	    bnr = end_bnr;

	ପାଇଁ ( ;!got_int; ui_breakcheck())
	{
	    // Delete the current buffer last, otherwise when the
	    // current buffer is deleted, the next buffer becomes
	    // the current one and will be loaded, which may then
	    // also be deleted, etc.
	    ଯଦି (bnr == curbuf->b_fnum)
		do_current = bnr;
	    ଅନ୍ୟଥା ଯଦି (do_buffer_ext(command, DOBUF_FIRST, FORWARD, bnr,
			  DOBUF_NOPOPUP | (forceit ? DOBUF_FORCEIT : 0)) == OK)
		++deleted;

	    // find next buffer number to delete/unload
	    ଯଦି (addr_ଗଣନା == 2)
	    {
		ଯଦି (++bnr > end_bnr)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା    // addr_count == 1
	    {
		arg = skipwhite(arg);
		ଯଦି (*arg == NUL)
		    ଭାଙ୍ଗ;
		ଯଦି (!VIM_ISDIGIT(*arg))
		{
		    p = skiptowhite_esc(arg);
		    bnr = buflist_findpat(arg, p,
			  command == DOBUF_WIPE || command == DOBUF_WIPE_REUSE,
								FALSE, FALSE);
		    ଯଦି (bnr < 0)	    // failed
			ଭାଙ୍ଗ;
		    arg = p;
		}
		ଅନ୍ୟଥା
		    bnr = getdigits(&arg);
	    }
	}
	ଯଦି (!got_int && do_current && do_buffer(command, DOBUF_FIRST,
					  FORWARD, do_current, forceit) == OK)
	    ++deleted;

	ଯଦି (deleted == 0)
	{
	    ଯଦି (command == DOBUF_UNLOAD)
		STRCPY(IObuff, _(e_no_buffers_were_unloaded));
	    ଅନ୍ୟଥା ଯଦି (command == DOBUF_DEL)
		STRCPY(IObuff, _(e_no_buffers_were_deleted));
	    ଅନ୍ୟଥା
		STRCPY(IObuff, _(e_no_buffers_were_wiped_out));
	    errormsg = (ଅକ୍ଷର *)IObuff;
	}
	ଅନ୍ୟଥା ଯଦି (deleted >= p_report)
	{
	    ଯଦି (command == DOBUF_UNLOAD)
		smsg(NGETTEXT("%d buffer unloaded",
			    "%d buffers unloaded", deleted), deleted);
	    ଅନ୍ୟଥା ଯଦି (command == DOBUF_DEL)
		smsg(NGETTEXT("%d buffer deleted",
			    "%d buffers deleted", deleted), deleted);
	    ଅନ୍ୟଥା
		smsg(NGETTEXT("%d buffer wiped out",
			    "%d buffers wiped out", deleted), deleted);
	}
    }

    ଫେରନ୍ତୁ errormsg;
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
    ଶୂନ୍ୟ
set_curbuf(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା action)
{
    buf_T	*prevbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unload = (action == DOBUF_UNLOAD || action == DOBUF_DEL
			|| action == DOBUF_WIPE || action == DOBUF_WIPE_REUSE);
#ifdef FEAT_SYN_HL
    long	old_tw = curbuf->b_p_tw;
#endif
    bufref_T	newbufref;
    bufref_T	prevbufref;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		valid;

    setpcmark();
    ଯଦି ((cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
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
    ଯଦି (!apply_autocmds(EVENT_BUFLEAVE, ଖାଲି, ଖାଲି, FALSE, curbuf)
	    || (bufref_valid(&prevbufref)
		&& bufref_valid(&newbufref)
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	       ))
    {
#ifdef FEAT_SYN_HL
	ଯଦି (prevbuf == curwin->w_buffer)
	    reset_synblock(curwin);
#endif
	ଯଦି (unload)
	    close_windows(prevbuf, FALSE);
#ଯଦି defined(FEAT_EVAL)
	ଯଦି (bufref_valid(&prevbufref) && !aborting())
#ଅନ୍ୟଥା
	ଯଦି (bufref_valid(&prevbufref))
#endif
	{
	    // Do not sync when in Insert mode and the buffer is open in
	    // another window, might be a timer doing something in another
	    // window.
	    ଯଦି (prevbuf == curbuf
		    && ((State & MODE_ଭର୍ତ୍ତି) == 0 || curbuf->b_nwindows <= 1))
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
    ଯଦି ((valid && buf != curbuf
#ifdef FEAT_EVAL
		&& !aborting()
#endif
	) || curwin->w_buffer == ଖାଲି)
    {
	// If the buffer is not valid but curwin->w_buffer is NULL we must
	// enter some buffer.  Using the last one is hopefully OK.
	ଯଦି (!valid)
	    enter_buffer(lastbuf);
	ଅନ୍ୟଥା
	    enter_buffer(buf);
#ifdef FEAT_SYN_HL
	ଯଦି (old_tw != curbuf->b_p_tw)
	    check_colorcolumn(ଖାଲି, curwin);
#endif
    }
}

/*
 * Enter a new current buffer.
 * Old curbuf must have been abandoned already!  This also means "curbuf" may
 * be pointing to freed memory.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
enter_buffer(buf_T *buf)
{
    // Stop Visual mode before changing curbuf.  May trigger TextYankPost, but
    // with textlock set.  Assumes curbuf and curwin->w_buffer is valid; if not,
    // buf_freeall() should've done this already!
    ଯଦି (VIsual_active
#ଯଦି defined(EXITFREE)
	    && !entered_free_all_mem
#endif
	    )
	end_visual_mode();

    ଯଦି (curwin->w_buffer != ଖାଲି)
	--curwin->w_buffer->b_nwindows;

    // Get the buffer in the current window.
    curwin->w_buffer = buf;
    curbuf = buf;
    ++curbuf->b_nwindows;

    // Copy buffer and window local option values.  Not for a help buffer.
    buf_copy_options(buf, BCO_ENTER | BCO_NOHELP);
    ଯଦି (!buf->b_help)
	get_winopts(buf);
#ifdef FEAT_FOLDING
    ଅନ୍ୟଥା
	// Remove all folds in the window.
	clearFolding(curwin);
    foldUpdateAll(curwin);	// update folds (later).
#endif

#ifdef FEAT_DIFF
    ଯଦି (curwin->w_p_diff)
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
    ଯଦି (curbuf->b_ml.ml_mfp == ଖାଲି)	// need to load the file
    {
	// If there is no filetype, allow for detecting one.  Esp. useful for
	// ":ball" used in an autocommand.  If there already is a filetype we
	// might prefer to keep it.
	ଯଦି (*curbuf->b_p_ft == NUL)
	    curbuf->b_did_filetype = false;

	open_buffer(FALSE, ଖାଲି, 0);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (!ସନ୍ଦେଶ_silent && !shortmess(SHM_FILEINFO))
	    need_fileinfo = TRUE;	// display file info after redraw

	// check if file changed
	(ଶୂନ୍ୟ)buf_check_timestamp(curbuf, FALSE);

	curwin->w_topline = 1;
#ifdef FEAT_DIFF
	curwin->w_topfill = 0;
#endif
	apply_autocmds(EVENT_BUFENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
	apply_autocmds(EVENT_BUFWINENTER, ଖାଲି, ଖାଲି, FALSE, curbuf);
    }

    // If autocommands did not change the cursor position, restore cursor lnum
    // and possibly cursor col.
    ଯଦି (curwin->w_cursor.lnum == 1 && inindent(0))
	buflist_getfpos();

    check_arg_idx(curwin);		// check for valid arg_idx
    maketitle();
	// when autocmds didn't change it
    ଯଦି (curwin->w_topline == 1 && !curwin->w_topline_was_set)
	scroll_cursor_halfway(FALSE, FALSE);	// redisplay at correct position

#ifdef FEAT_NETBEANS_INTG
    // Send fileOpened event because we've changed buffers.
    netbeans_file_activated(curbuf);
#endif

    // Change directories when the 'acd' option is set.
    DO_AUTOCHDIR;

#ifdef FEAT_KEYMAP
    ଯଦି (curbuf->b_kmap_state & KEYMAP_INIT)
	(ଶୂନ୍ୟ)keymap_init();
#endif
#ifdef FEAT_SPELL
    // May need to set the spell language.  Can only do this after the buffer
    // has been properly setup.
    ଯଦି (!curbuf->b_help && curwin->w_p_spell && *curwin->w_s->b_p_spl != NUL)
	(ଶୂନ୍ୟ)parse_spelllang(curwin);
#endif
#ifdef FEAT_VIMINFO
    curbuf->b_last_used = vim_time();
#endif

    redraw_later(UPD_NOT_VALID);
}

#ଯଦି defined(FEAT_AUTOCHDIR)
/*
 * Change to the directory of the current buffer.
 * Don't do this while still starting up.
 */
    ଶୂନ୍ୟ
do_autochdir(ଶୂନ୍ୟ)
{
    ଯଦି ((starting == 0 || test_autochdir)
	    && curbuf->b_ffname != ଖାଲି
	    && vim_chdirfile(curbuf->b_ffname, "auto") == OK)
    {
	shorten_fnames(TRUE);
	last_chdir_reason = "autochdir";
    }
}
#endif

    ଶୂନ୍ୟ
no_write_message_buf(buf_T *buf)
{
#ifdef FEAT_TERMINAL
    ଯଦି (term_job_running(buf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    ଅନ୍ୟଥା
#endif
	semsg(_(e_no_write_since_last_change_for_buffer_nr_add_bang_to_override),
		buf->b_fnum);
}

    ଶୂନ୍ୟ
no_write_message(ଶୂନ୍ୟ)
{
#ifdef FEAT_TERMINAL
    ଯଦି (term_job_running(curbuf->b_term))
	emsg(_(e_job_still_running_add_bang_to_end_the_job));
    ଅନ୍ୟଥା
#endif
	emsg(_(e_no_write_since_last_change_add_bang_to_override));
}

    ଶୂନ୍ୟ
no_write_message_nobang(buf_T *buf UNUSED)
{
#ifdef FEAT_TERMINAL
    ଯଦି (term_job_running(buf->b_term))
	emsg(_(e_job_still_running));
    ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
curbuf_reusable(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (curbuf != ଖାଲି
	&& curbuf->b_ffname == ଖାଲି
	&& curbuf->b_nwindows <= 1
	&& (curbuf->b_ml.ml_mfp == ଖାଲି || BUFEMPTY())
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		// BLN_ defines
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*buf;
#ifdef UNIX
    stat_T	st;
#endif

    ଯଦି (ଶୀର୍ଷ_file_ଅଙ୍କ == 1)
	hash_init(&buf_hashtab);

    fname_expand(curbuf, &ffname, &sfname);	// will allocate ffname

    /*
     * If the file name already exists in the list, update the entry.
     */
#ifdef UNIX
    // On Unix we can use inode numbers when the file exists.  Works better
    // for hard links.
    ଯଦି (sfname == ଖାଲି || mch_stat((ଅକ୍ଷର *)sfname, &st) < 0)
	st.st_dev = (dev_T)-1;
#endif
    ଯଦି (ffname != ଖାଲି && !(flags & (BLN_DUMMY | BLN_NEW)) && (buf =
#ifdef UNIX
		buflist_findname_stat(ffname, &st)
#ଅନ୍ୟଥା
		buflist_findname(ffname)
#endif
		) != ଖାଲି)
    {
	vim_free(ffname);
	ଯଦି (lnum != 0)
	    buflist_setfpos(buf, (flags & BLN_NOCURWIN) ? ଖାଲି : curwin,
						      lnum, (colnr_T)0, FALSE);

	ଯଦି ((flags & BLN_NOOPT) == 0)
	    // copy the options now, if 'cpo' doesn't have 's' and not done
	    // already
	    buf_copy_options(buf, 0);

	ଯଦି ((flags & BLN_LISTED) && !buf->b_p_bl)
	{
	    bufref_T bufref;

	    buf->b_p_bl = TRUE;
	    set_bufref(&bufref, buf);
	    ଯଦି (!(flags & BLN_DUMMY))
	    {
		ଯଦି (apply_autocmds(EVENT_BUFADD, ଖାଲି, ଖାଲି, FALSE, buf)
			&& !bufref_valid(&bufref))
		    ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
	ଫେରନ୍ତୁ buf;
    }

    /*
     * If the current buffer has no name and no contents, use the current
     * buffer.	Otherwise: Need to allocate a new buffer structure.
     *
     * This is the ONLY place where a new buffer structure is allocated!
     * (A spell file buffer is allocated in spell.c, but that's not a normal
     * buffer.)
     */
    buf = ଖାଲି;
    ଯଦି ((flags & BLN_CURBUF) && curbuf_reusable())
    {
	bufref_T bufref;

	buf = curbuf;
	set_bufref(&bufref, buf);
	trigger_undo_ftplugin(buf, curwin);
	// It's like this buffer is deleted.  Watch out for autocommands that
	// change curbuf!  If that happens, allocate a new buffer anyway.
	buf_freeall(buf, BFA_WIPE | BFA_DEL);
#ifdef FEAT_EVAL
	ଯଦି (aborting())		// autocmds may abort script processing
	{
	    vim_free(ffname);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
#endif
	ଯଦି (!bufref_valid(&bufref))
	    buf = ଖାଲି;		// buf was deleted; allocate a new buffer
    }
    ଯଦି (buf != curbuf || curbuf == ଖାଲି)
    {
	buf = ALLOC_CLEAR_ONE(buf_T);
	ଯଦି (buf == ଖାଲି)
	{
	    vim_free(ffname);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
#ifdef FEAT_EVAL
	// init b: variables
	buf->b_vars = dict_alloc_id(aid_newbuf_bvars);
	ଯଦି (buf->b_vars == ଖାଲି)
	{
	    vim_free(ffname);
	    vim_free(buf);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	init_var_dict(buf->b_vars, &buf->b_bufvar, VAR_SCOPE);
#endif
	init_changedtick(buf);
    }

    ଯଦି (ffname != ଖାଲି)
    {
	buf->b_ffname = ffname;
	buf->b_sfname = vim_strsave(sfname);
    }

    clear_wininfo(buf);
    buf->b_wininfo = ALLOC_CLEAR_ONE(wininfo_T);

    ଯଦି ((ffname != ଖାଲି && (buf->b_ffname == ଖାଲି || buf->b_sfname == ଖାଲି))
	    || buf->b_wininfo == ଖାଲି)
    {
	ଯଦି (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ଅନ୍ୟଥା
	    buf->b_sfname = ଖାଲି;
	VIM_CLEAR(buf->b_ffname);
	ଯଦି (buf != curbuf)
	    free_buffer(buf);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (buf == curbuf)
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
    ଅନ୍ୟଥା
    {
	// put the new buffer at the end of the buffer list
	buf->b_next = ଖାଲି;
	ଯଦି (firstbuf == ଖାଲି)		// buffer list is empty
	{
	    buf->b_prev = ଖାଲି;
	    firstbuf = buf;
	}
	ଅନ୍ୟଥା				// append new buffer at end of list
	{
	    lastbuf->b_next = buf;
	    buf->b_prev = lastbuf;
	}
	lastbuf = buf;

	ଯଦି ((flags & BLN_REUSE) && buf_reuse.ga_len > 0)
	{
	    // Recycle a previously used buffer number.  Used for buffers which
	    // are normally hidden, e.g. in a popup window.  Avoids that the
	    // buffer number grows rapidly.
	    --buf_reuse.ga_len;
	    buf->b_fnum = ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)buf_reuse.ga_data)[buf_reuse.ga_len];

	    // Move buffer to the right place in the buffer list.
	    ଯେତେବେଳେ (buf->b_prev != ଖାଲି && buf->b_fnum < buf->b_prev->b_fnum)
	    {
		buf_T	*prev = buf->b_prev;

		prev->b_next = buf->b_next;
		ଯଦି (prev->b_next != ଖାଲି)
		    prev->b_next->b_prev = prev;
		buf->b_next = prev;
		buf->b_prev = prev->b_prev;
		ଯଦି (buf->b_prev != ଖାଲି)
		    buf->b_prev->b_next = buf;
		prev->b_prev = buf;
		ଯଦି (lastbuf == buf)
		    lastbuf = prev;
		ଯଦି (firstbuf == prev)
		    firstbuf = buf;
	    }
	}
	ଅନ୍ୟଥା
	    buf->b_fnum = ଶୀର୍ଷ_file_ଅଙ୍କ++;
	ଯଦି (ଶୀର୍ଷ_file_ଅଙ୍କ < 0)		// wrap around (may cause duplicates)
	{
	    emsg(_("W14: Warning: List of file names overflow"));
	    ଯଦି (emsg_silent == 0 && !in_assert_fails)
	    {
		out_flush();
		ui_delay(3001L, TRUE);	// make sure it is noticed
	    }
	    ଶୀର୍ଷ_file_ଅଙ୍କ = 1;
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
    ଯଦି (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ଅନ୍ୟଥା
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif
    buf->b_u_synced = true;
    buf->b_flags = BF_CHECK_RO | BF_NEVERLOADED;
    ଯଦି (flags & BLN_DUMMY)
	buf->b_flags |= BF_DUMMY;
    buf_clear_file(buf);
    clrallmarks(buf);			// clear marks
    fmarks_check_names(buf);		// check file marks for this file
    buf->b_p_bl = (flags & BLN_LISTED) ? TRUE : FALSE;	// init 'buflisted'
    ଯଦି (!(flags & BLN_DUMMY))
    {
	bufref_T bufref;

	// Tricky: these autocommands may change the buffer list.  They could
	// also split the window with re-using the one empty buffer. This may
	// result in unexpectedly losing the empty buffer.
	set_bufref(&bufref, buf);
	ଯଦି (apply_autocmds(EVENT_BUFNEW, ଖାଲି, ଖାଲି, FALSE, buf)
		&& !bufref_valid(&bufref))
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (flags & BLN_LISTED)
	{
	    ଯଦି (apply_autocmds(EVENT_BUFADD, ଖାଲି, ଖାଲି, FALSE, buf)
		    && !bufref_valid(&bufref))
		ଫେରନ୍ତୁ ଖାଲି;
	}
#ifdef FEAT_EVAL
	ଯଦି (aborting())		// autocmds may abort script processing
	    ଫେରନ୍ତୁ ଖାଲି;
#endif
    }

    ଫେରନ୍ତୁ buf;
}

/*
 * Free the memory for the options of a buffer.
 * If "free_p_ff" is TRUE also free 'fileformat', 'buftype' and
 * 'fileencoding'.
 */
    ଶୂନ୍ୟ
free_buf_options(
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		free_p_ff)
{
    ଯଦି (free_p_ff)
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
#ଯଦି defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_inde);
    clear_string_option(&buf->b_p_indk);
#endif
#ଯଦି defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_bexpr);
#endif
#ଯଦି defined(FEAT_CRYPT)
    clear_string_option(&buf->b_p_cm);
#endif
    clear_string_option(&buf->b_p_fp);
#ଯଦି defined(FEAT_EVAL)
    clear_string_option(&buf->b_p_fex);
#endif
#ifdef FEAT_CRYPT
# ifdef FEAT_SODIUM
    ଯଦି (buf->b_p_ଚାବି != ଖାଲି && *buf->b_p_ଚାବି != NUL
			   && crypt_method_is_sodium(crypt_get_method_nr(buf)))
	crypt_sodium_munlock(buf->b_p_ଚାବି, STRLEN(buf->b_p_ଚାବି));
# endif
    clear_string_option(&buf->b_p_ଚାବି);
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
    buf->b_s.b_cap_prog = ଖାଲି;
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
    clear_cpt_callbacks(&buf->b_p_cpt_cb, buf->b_p_cpt_ଗଣନା);
    buf->b_p_cpt_ଗଣନା = 0;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buflist_getfile(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n,
    linenr_T	lnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit)
{
    buf_T	*buf;
    win_T	*wp = ଖାଲି;
    pos_T	*fpos;
    colnr_T	col;

    buf = buflist_findnr(n);
    ଯଦି (buf == ଖାଲି)
    {
	ଯଦି ((options & GETF_ALT) && n == 0)
	    emsg(_(e_no_alternate_file));
	ଅନ୍ୟଥା
	    semsg(_(e_buffer_nr_not_found), n);
	ଫେରନ୍ତୁ FAIL;
    }

    // if alternate file is the current buffer, nothing to do
    ଯଦି (buf == curbuf)
	ଫେରନ୍ତୁ OK;

    ଯଦି (text_or_buf_locked())
	ଫେରନ୍ତୁ FAIL;

    // altfpos may be changed by getfile(), get it now
    ଯଦି (lnum == 0)
    {
	fpos = buflist_findfpos(buf);
	lnum = fpos->lnum;
	col = fpos->col;
    }
    ଅନ୍ୟଥା
	col = 0;

    ଯଦି (options & GETF_SWITCH)
    {
	// If 'switchbuf' is set jump to the window containing "buf".
	wp = swbuf_goto_win_with_buf(buf);

	// If 'switchbuf' contains "split", "vsplit" or "newtab" and the
	// current buffer isn't empty: open new tab or window
	ଯଦି (wp == ଖାଲି && (swb_flags & (SWB_VSPLIT | SWB_SPLIT | SWB_NEWTAB))
							       && !BUFEMPTY())
	{
	    ଯଦି (swb_flags & SWB_NEWTAB)
		tabpage_new();
	    ଅନ୍ୟଥା ଯଦି (win_split(0, (swb_flags & SWB_VSPLIT) ? WSP_VERT : 0)
								      == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    RESET_BINDING(curwin);
	}
    }

    ++RedrawingDisabled;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval = FAIL;
    ଯଦି (GETFILE_SUCCESS(getfile(buf->b_fnum, ଖାଲି, ଖାଲି,
				     (options & GETF_SETMARK), lnum, forceit)))
    {
	// cursor is at to BOL and w_cursor.lnum is checked due to getfile()
	ଯଦି (!p_sol && col != 0)
	{
	    curwin->w_cursor.col = col;
	    check_cursor_col();
	    curwin->w_cursor.coladd = 0;
	    curwin->w_set_curswant = true;
	}
	retval = OK;
    }

    ଯଦି (RedrawingDisabled > 0)
	--RedrawingDisabled;
    ଫେରନ୍ତୁ retval;
}

/*
 * go to the last know line number for the current buffer
 */
    ସ୍ଥିର ଶୂନ୍ୟ
buflist_getfpos(ଶୂନ୍ୟ)
{
    pos_T	*fpos;

    fpos = buflist_findfpos(curbuf);

    curwin->w_cursor.lnum = fpos->lnum;
    check_cursor_lnum();

    ଯଦି (p_sol)
	curwin->w_cursor.col = 0;
    ଅନ୍ୟଥା
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
    buf_T	*buf = ଖାଲି;

    // First make the name into a full path name
    ffname = FullName_save(fname,
#ifdef UNIX
	    TRUE	    // force expansion, get rid of symbolic links
#ଅନ୍ୟଥା
	    FALSE
#endif
	    );
    ଯଦି (ffname != ଖାଲି)
    {
	buf = buflist_findname(ffname);
	vim_free(ffname);
    }
    ଫେରନ୍ତୁ buf;
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

    ଯଦି (mch_stat((ଅକ୍ଷର *)ffname, &st) < 0)
	st.st_dev = (dev_T)-1;
    ଫେରନ୍ତୁ buflist_findname_stat(ffname, &st);
}

/*
 * Same as buflist_findname(), but pass the stat structure to avoid getting it
 * twice for the same file.
 * Returns NULL if not found.
 */
    ସ୍ଥିର buf_T *
buflist_findname_stat(
    char_u	*ffname,
    stat_T	*stp)
{
#endif
    buf_T	*buf;

    // Start at the last buffer, expect to find a match sooner.
    FOR_ALL_BUFS_FROM_LAST(buf)
	ଯଦି ((buf->b_flags & BF_DUMMY) == 0 && !otherfile_buf(buf, ffname
#ifdef UNIX
		    , stp
#endif
		    ))
	    ଫେରନ୍ତୁ buf;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Find file in buffer list by a regexp pattern.
 * Return fnum of the found buffer.
 * Return < 0 for error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buflist_findpat(
    char_u	*pattern,
    char_u	*pattern_end,	// pointer to first char after pattern
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unlisted,	// find unlisted buffers
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		diffmode UNUSED, // find diff-mode buffers only
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curtab_only)	// find buffers in current tab only
{
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		match = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		find_listed;
    char_u	*pat;
    char_u	*patend;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		attempt;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		toggledollar;

    // "%" is current file, "%%" or "#" is alternate file
    ଯଦି ((pattern_end == pattern + 1 && (*pattern == '%' || *pattern == '#'))
	    || (in_vim9script() && pattern_end == pattern + 2
				    && pattern[0] == '%' && pattern[1] == '%'))
    {
	ଯଦି (*pattern == '#' || pattern_end == pattern + 2)
	    match = curwin->w_alt_fnum;
	ଅନ୍ୟଥା
	    match = curbuf->b_fnum;
#ifdef FEAT_DIFF
	ଯଦି (diffmode && !diff_mode_buf(buflist_findnr(match)))
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
    ଅନ୍ୟଥା
    {
	pat = file_pat_to_reg_pat(pattern, pattern_end, ଖାଲି, FALSE);
	ଯଦି (pat == ଖାଲି)
	    ଫେରନ୍ତୁ -1;
	patend = pat + STRLEN(pat) - 1;
	toggledollar = (patend > pat && *patend == '$');

	// First try finding a listed buffer.  If not found and "unlisted"
	// is TRUE, try finding an unlisted buffer.
	find_listed = TRUE;
	ପାଇଁ (;;)
	{
	    ପାଇଁ (attempt = 0; attempt <= 3; ++attempt)
	    {
		regmatch_T	regmatch;

		// may add '^' and '$'
		ଯଦି (toggledollar)
		    *patend = (attempt < 2) ? NUL : '$'; // add/remove '$'
		p = pat;
		ଯଦି (*p == '^' && !(attempt & 1))	 // add/remove '^'
		    ++p;
		regmatch.regprog = vim_regcomp(p, magic_isset() ? RE_MAGIC : 0);

		FOR_ALL_BUFS_FROM_LAST(buf)
		{
		    ଯଦି (regmatch.regprog == ଖାଲି)
		    {
			// invalid pattern, possibly after switching engine
			vim_free(pat);
			ଫେରନ୍ତୁ -1;
		    }
		    ଯଦି (buf->b_p_bl == find_listed
#ifdef FEAT_DIFF
			    && (!diffmode || diff_mode_buf(buf))
#endif
			    && buflist_match(&regmatch, buf, FALSE) != ଖାଲି)
		    {
			ଯଦି (curtab_only)
			{
			    // Ignore the match if the buffer is not open in
			    // the current tab.
			    win_T	*wp;

			    FOR_ALL_WINDOWS(wp)
				ଯଦି (wp->w_buffer == buf)
				    ଭାଙ୍ଗ;
			    ଯଦି (wp == ଖାଲି)
				ଜାରି;
			}
			ଯଦି (match >= 0)		// already found a match
			{
			    match = -2;
			    ଭାଙ୍ଗ;
			}
			match = buf->b_fnum;	// remember first match
		    }
		}

		vim_regfree(regmatch.regprog);
		ଯଦି (match >= 0)			// found one match
		    ଭାଙ୍ଗ;
	    }

	    // Only search for unlisted buffers if there was no match with
	    // a listed buffer.
	    ଯଦି (!unlisted || !find_listed || match != -1)
		ଭାଙ୍ଗ;
	    find_listed = FALSE;
	}

	vim_free(pat);
    }

    ଯଦି (match == -2)
	semsg(_(e_more_than_one_match_for_str), pattern);
    ଅନ୍ୟଥା ଯଦି (match < 0)
	semsg(_(e_no_matching_buffer_for_str), pattern);
    ଫେରନ୍ତୁ match;
}

#ifdef FEAT_VIMINFO
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    buf_T   *buf;
    char_u  *match;
} bufmatch_T;
#endif

/*
 * Find all buffer names that match.
 * For command line expansion of ":buf" and ":sbuf".
 * Return OK if matches found, FAIL otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ExpandBufnames(
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_file,
    char_u	***file,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		round;
    char_u	*p;
    char_u	*patc = ଖାଲି;
#ifdef FEAT_VIMINFO
    bufmatch_T	*matches = ଖାଲି;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fuzzy;
    fuzmatch_str_T  *fuzmatch = ଖାଲି;
    regmatch_T	regmatch;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		score = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		to_free = FALSE;

    *ଅଙ୍କ_file = 0;		    // return values in case of FAIL
    *file = ଖାଲି;

#ifdef FEAT_DIFF
    ଯଦି ((options & BUF_DIFF_FILTER) && !curwin->w_p_diff)
	ଫେରନ୍ତୁ FAIL;
#endif

    fuzzy = cmdline_fuzzy_complete(pat);

    // Make a copy of "pat" and change "^" to "\(^\|[\/]\)" (if doing regular
    // expression matching)
    ଯଦି (!fuzzy)
    {
	ଯଦି (*pat == '^' && pat[1] != NUL)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା  len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(pat);
	    patc = alloc(len);
	    ଯଦି (patc == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    STRNCPY(patc, pat + 1, len - 1);
	    patc[len - 1] = NUL;
	    to_free = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (*pat == '^')
	    patc = (char_u *)"";
	ଅନ୍ୟଥା
	    patc = pat;
	regmatch.regprog = vim_regcomp(patc, RE_MAGIC);
    }

    // round == 1: Count the matches.
    // round == 2: Build the array to keep the matches.
    ପାଇଁ (round = 1; round <= 2; ++round)
    {
	ଗଣନା_2 = 0;
	FOR_ALL_BUFFERS(buf)
	{
	    ଯଦି (!buf->b_p_bl)	// skip unlisted buffers
		ଜାରି;
#ifdef FEAT_DIFF
	    ଯଦି (options & BUF_DIFF_FILTER)
		// Skip buffers not suitable for
		// :diffget or :diffput completion.
		ଯଦି (buf == curbuf || !diff_mode_buf(buf))
		    ଜାରି;
#endif

	    ଯଦି (!fuzzy)
	    {
		ଯଦି (regmatch.regprog == ଖାଲି)
		{
		    // invalid pattern, possibly after recompiling
		    ଯଦି (to_free)
			vim_free(patc);
		    ଫେରନ୍ତୁ FAIL;
		}
		p = buflist_match(&regmatch, buf, p_wic);
	    }
	    ଅନ୍ୟଥା
	    {
		p = ଖାଲି;
		// first try matching with the short file name
		ଯଦି ((score = fuzzy_match_str(buf->b_sfname, pat))
			!= FUZZY_SCORE_NONE)
		    p = buf->b_sfname;
		ଯଦି (p == ଖାଲି)
		{
		    // next try matching with the full path file name
		    ଯଦି ((score = fuzzy_match_str(buf->b_ffname, pat))
			    != FUZZY_SCORE_NONE)
			p = buf->b_ffname;
		}
	    }

	    ଯଦି (p == ଖାଲି)
		ଜାରି;

	    ଯଦି (round == 1)
	    {
		++ଗଣନା_2;
		ଜାରି;
	    }

	    ଯଦି (options & WILD_HOME_REPLACE)
		p = home_replace_save(buf, p);
	    ଅନ୍ୟଥା
		p = vim_strsave(p);
	    ଯଦି (p == ଖାଲି)
	    {
		ଯଦି (fuzzy && round == 2)
		    fuzmatch_str_free(fuzmatch, ଗଣନା_2);
		ଫେରନ୍ତୁ FAIL;
	    }

	    ଯଦି (!fuzzy)
	    {
#ifdef FEAT_VIMINFO
		ଯଦି (matches != ଖାଲି)
		{
		    matches[ଗଣନା_2].buf = buf;
		    matches[ଗଣନା_2].match = p;
		    ଗଣନା_2++;
		}
		ଅନ୍ୟଥା
#endif
		    (*file)[ଗଣନା_2++] = p;
	    }
	    ଅନ୍ୟଥା
	    {
		fuzmatch[ଗଣନା_2].idx = ଗଣନା_2;
		fuzmatch[ଗଣନା_2].str = p;
		fuzmatch[ଗଣନା_2].score = score;
		ଗଣନା_2++;
	    }
	}
	ଯଦି (ଗଣନା_2 == 0)	// no match found, break here
	    ଭାଙ୍ଗ;
	ଯଦି (round == 1)
	{
	    ଯଦି (!fuzzy)
	    {
		*file = ALLOC_MULT(char_u *, ଗଣନା_2);
		ଯଦି (*file == ଖାଲି)
		{
		    vim_regfree(regmatch.regprog);
		    ଯଦି (to_free)
			vim_free(patc);
		    ଫେରନ୍ତୁ FAIL;
		}
#ifdef FEAT_VIMINFO
		ଯଦି (options & WILD_BUFLASTUSED)
		    matches = ALLOC_MULT(bufmatch_T, ଗଣନା_2);
#endif
	    }
	    ଅନ୍ୟଥା
	    {
		fuzmatch = ALLOC_MULT(fuzmatch_str_T, ଗଣନା_2);
		ଯଦି (fuzmatch == ଖାଲି)
		{
		    *ଅଙ୍କ_file = 0;
		    *file = ଖାଲି;
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	}
    }

    ଯଦି (!fuzzy)
    {
	vim_regfree(regmatch.regprog);
	ଯଦି (to_free)
	    vim_free(patc);
    }

    ଯଦି (!fuzzy)
    {
#ifdef FEAT_VIMINFO
	ଯଦି (matches != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	    ଯଦି (ଗଣନା_2 > 1)
		qsort(matches, ଗଣନା_2, ଆକାର(bufmatch_T), buf_compare);
	    // if the current buffer is first in the list, place it at the end
	    ଯଦି (matches[0].buf == curbuf)
	    {
		ପାଇଁ (i = 1; i < ଗଣନା_2; i++)
		    (*file)[i-1] = matches[i].match;
		(*file)[ଗଣନା_2-1] = matches[0].match;
	    }
	    ଅନ୍ୟଥା
	    {
		ପାଇଁ (i = 0; i < ଗଣନା_2; i++)
		    (*file)[i] = matches[i].match;
	    }
	    vim_free(matches);
	}
#endif
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (fuzzymatches_to_strmatches(fuzmatch, file, ଗଣନା_2, FALSE) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    *ଅଙ୍କ_file = ଗଣନା_2;
    ଫେରନ୍ତୁ (ଗଣନା_2 == 0 ? FAIL : OK);
}

/*
 * Check for a match on the file name for buffer "buf" with regprog "prog".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 */
    ସ୍ଥିର char_u *
buflist_match(
    regmatch_T	*rmp,
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match;

    // First try the short file name, then the long file name.
    match = fname_match(rmp, buf->b_sfname, ignore_case);
    ଯଦି (match == ଖାଲି && rmp->regprog != ଖାଲି)
	match = fname_match(rmp, buf->b_ffname, ignore_case);

    ଫେରନ୍ତୁ match;
}

/*
 * Try matching the regexp in "rmp->regprog" with file name "name".
 * Note that rmp->regprog may become NULL when switching regexp engine.
 * Return "name" when there is a match, NULL when not.
 */
    ସ୍ଥିର char_u *
fname_match(
    regmatch_T	*rmp,
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ignore_case)  // when TRUE ignore case, when FALSE use 'fic'
{
    char_u	*match = ଖାଲି;
    char_u	*p;

    // extra check for valid arguments
    ଯଦି (name == ଖାଲି || rmp->regprog == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Ignore case when 'fileignorecase' or the argument is set.
    rmp->rm_ic = p_fic || ignore_case;
    ଯଦି (vim_regexec(rmp, name, (colnr_T)0))
	match = name;
    ଅନ୍ୟଥା ଯଦି (rmp->regprog != ଖାଲି)
    {
	// Replace $(HOME) with '~' and try matching again.
	p = home_replace_save(ଖାଲି, name);
	ଯଦି (p != ଖାଲି && vim_regexec(rmp, p, (colnr_T)0))
	    match = name;
	vim_free(p);
    }

    ଫେରନ୍ତୁ match;
}

/*
 * Find a file in the buffer list by buffer number.
 */
    buf_T *
buflist_findnr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr)
{
    char_u	ଚାବି[VIM_SIZEOF_INT * 2 + 1];
    hashitem_T	*hi;

    ଯଦି (nr == 0)
	nr = curwin->w_alt_fnum;
    vim_snprintf((ଅକ୍ଷର *)ଚାବି, ଆକାର(ଚାବି), "%x", nr);
    hi = hash_find(&buf_hashtab, ଚାବି);

    ଯଦି (!HASHITEM_EMPTY(hi))
	ଫେରନ୍ତୁ (buf_T *)(hi->hi_ଚାବି
			     - ((unsigned)(curbuf->b_ଚାବି - (char_u *)curbuf)));
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Get name of file 'n' in the buffer list.
 * When the file has no name an empty string is returned.
 * home_replace() is used to shorten the file name (used for marks).
 * Returns a pointer to allocated memory, of NULL when failed.
 */
    char_u *
buflist_nr2name(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fullname,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		helptail)	// for help buffers return tail only
{
    buf_T	*buf;

    buf = buflist_findnr(n);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ home_replace_save(helptail ? buf : ଖାଲି,
				     fullname ? buf->b_ffname : buf->b_fname);
}

/*
 * Set the "lnum" and "col" for the buffer "buf" and the current window.
 * When "copy_options" is TRUE save the local window option values.
 * When "lnum" is 0 only do the options.
 */
    ଶୂନ୍ୟ
buflist_setfpos(
    buf_T	*buf,
    win_T	*win,		// may be NULL when using :badd
    linenr_T	lnum,
    colnr_T	col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copy_options)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ଯଦି (wip->wi_win == win)
	    ଭାଙ୍ଗ;
    ଯଦି (wip == ଖାଲି)
    {
	// allocate a new entry
	wip = ALLOC_CLEAR_ONE(wininfo_T);
	ଯଦି (wip == ଖାଲି)
	    ଫେରନ୍ତୁ;
	wip->wi_win = win;
	ଯଦି (lnum == 0)		// set lnum even when it's 0
	    lnum = 1;
    }
    ଅନ୍ୟଥା
    {
	// remove the entry from the list
	ଯଦି (wip->wi_prev)
	    wip->wi_prev->wi_next = wip->wi_next;
	ଅନ୍ୟଥା
	    buf->b_wininfo = wip->wi_next;
	ଯଦି (wip->wi_next)
	    wip->wi_next->wi_prev = wip->wi_prev;
	ଯଦି (copy_options && wip->wi_optset)
	{
	    clear_winopt(&wip->wi_opt);
#ifdef FEAT_FOLDING
	    deleteFoldRecurse(&wip->wi_folds);
#endif
	}
    }
    ଯଦି (lnum != 0)
    {
	wip->wi_fpos.lnum = lnum;
	wip->wi_fpos.col = col;
    }
    ଯଦି (win != ଖାଲି)
	wip->wi_changelistidx = win->w_changelistidx;
    ଯଦି (copy_options && win != ଖାଲି)
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
    wip->wi_prev = ଖାଲି;
    ଯଦି (wip->wi_next)
	wip->wi_next->wi_prev = wip;
}

#ifdef FEAT_DIFF
/*
 * Return TRUE when "wip" has 'diff' set and the diff is only for another tab
 * page.  That's because a diff is local to a tab page.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
wininfo_other_tab_diff(wininfo_T *wip)
{
    win_T	*wp;

    ଯଦି (!wip->wi_opt.wo_diff)
	ଫେରନ୍ତୁ FALSE;

    FOR_ALL_WINDOWS(wp)
	// return FALSE when it's a window in the current tab page, thus
	// the buffer was in diff mode here
	ଯଦି (wip->wi_win == wp)
	    ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
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
    ସ୍ଥିର wininfo_T *
find_wininfo(
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		need_options,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		skip_diff_buffer UNUSED)
{
    wininfo_T	*wip;

    FOR_ALL_BUF_WININFO(buf, wip)
	ଯଦି (wip->wi_win == curwin
#ifdef FEAT_DIFF
		&& (!skip_diff_buffer || !wininfo_other_tab_diff(wip))
#endif

		&& (!need_options || wip->wi_optset))
	    ଭାଙ୍ଗ;

    ଯଦି (wip != ଖାଲି)
	ଫେରନ୍ତୁ wip;

    // If no wininfo for curwin, use the first in the list (that doesn't have
    // 'diff' set and is in another tab page).
    // If "need_options" is TRUE skip entries that don't have options set,
    // unless the window is editing "buf", so we can copy from the window
    // itself.
#ifdef FEAT_DIFF
    ଯଦି (skip_diff_buffer)
    {
	FOR_ALL_BUF_WININFO(buf, wip)
	    ଯଦି (!wininfo_other_tab_diff(wip)
		    && (!need_options || wip->wi_optset
			|| (wip->wi_win != ଖାଲି
			    && wip->wi_win->w_buffer == buf)))
		ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା
#endif
	wip = buf->b_wininfo;
    ଫେରନ୍ତୁ wip;
}

/*
 * Reset the local window options to the values last used in this window.
 * If the buffer wasn't used in this window before, use the values from
 * the most recently used window.  If the values were never set, use the
 * global values for the window.
 */
    ଶୂନ୍ୟ
get_winopts(buf_T *buf)
{
    wininfo_T	*wip;

    clear_winopt(&curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
    clearFolding(curwin);
#endif

    wip = find_wininfo(buf, TRUE, TRUE);
    ଯଦି (wip != ଖାଲି && wip->wi_win != ଖାଲି
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
    ଅନ୍ୟଥା ଯଦି (wip != ଖାଲି && wip->wi_optset)
    {
	// the buffer was displayed in the current window earlier
	copy_winopt(&wip->wi_opt, &curwin->w_onebuf_opt);
#ifdef FEAT_FOLDING
	curwin->w_fold_manual = wip->wi_fold_manual;
	curwin->w_foldinvalid = true;
	cloneFoldGrowArray(&wip->wi_folds, &curwin->w_folds);
#endif
    }
    ଅନ୍ୟଥା
	copy_winopt(&curwin->w_allbuf_opt, &curwin->w_onebuf_opt);
    ଯଦି (wip != ଖାଲି)
	curwin->w_changelistidx = wip->wi_changelistidx;

#ifdef FEAT_FOLDING
    // Set 'foldlevel' to 'foldlevelstart' if it's not negative.
    ଯଦି (p_fdls >= 0)
	curwin->w_p_fdl = p_fdls;
#endif
#ifdef FEAT_STL_OPT
    // Update rendered height for window-local 'statusline'.
    ଯଦି (*curwin->w_p_stl != NUL)
	update_stl_rendered_height(curwin);
    // If the effective statuslineopt changed, re-adjust all window heights.
    ଯଦି (curwin->w_status_height > 0
	    && curwin->w_status_height != statusline_height(curwin))
    {
	frame_change_statusline_height();
	// win_split_ins() estimated the new window's stlh from the splitting
	// window, which may differ from the actual stlh after option
	// inheritance is resolved here.  frame_change_statusline_height()
	// redistributes space within the stale fr_height values set during the
	// split, so the content rows may be unequal.  Re-equalize if needed.
	ଯଦି (p_ea)
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
    ସ୍ଥିର pos_T no_position = {1, 0, 0};

    wip = find_wininfo(buf, FALSE, FALSE);
    ଯଦି (wip != ଖାଲି)
	ଫେରନ୍ତୁ &(wip->wi_fpos);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ &no_position;
}

/*
 * Find the lnum for the buffer 'buf' for the current window.
 */
    linenr_T
buflist_findlnum(buf_T *buf)
{
    ଫେରନ୍ତୁ buflist_findfpos(buf)->lnum;
}

/*
 * List all known file names (for :files and :buffers command).
 */
    ଶୂନ୍ୟ
buflist_list(exarg_T *eap)
{
    buf_T	*buf = firstbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ro_char;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		changed_char;
#ifdef FEAT_TERMINAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		job_running;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		job_none_open;
#endif

#ifdef FEAT_VIMINFO
    garray_T	buflist;
    buf_T	**buflist_data = ଖାଲି, **p;

    ଯଦି (vim_strchr(eap->arg, 't'))
    {
	ga_init2(&buflist, ଆକାର(buf_T *), 50);
	FOR_ALL_BUFFERS(buf)
	{
	    ଯଦି (ga_grow(&buflist, 1) == OK)
		((buf_T **)buflist.ga_data)[buflist.ga_len++] = buf;
	}

	qsort(buflist.ga_data, (size_t)buflist.ga_len,
		ଆକାର(buf_T *), buf_compare);

	buflist_data = (buf_T **)buflist.ga_data;
	buf = *buflist_data;
    }
    p = buflist_data;

    ପାଇଁ (; buf != ଖାଲି && !got_int; buf = buflist_data != ଖାଲି
	    ? (++p < buflist_data + buflist.ga_len ? *p : ଖାଲି)
	    : buf->b_next)
#ଅନ୍ୟଥା
    ପାଇଁ (buf = firstbuf; buf != ଖାଲି && !got_int; buf = buf->b_next)
#endif
    {
	char_u	*name;

#ifdef FEAT_TERMINAL
	job_running = term_job_running(buf->b_term);
	job_none_open = term_none_open(buf->b_term);
#endif
	// skip unlisted buffers, unless ! was used
	ଯଦି ((!buf->b_p_bl && !eap->forceit && !vim_strchr(eap->arg, 'u'))
		|| (vim_strchr(eap->arg, 'u') && buf->b_p_bl)
		|| (vim_strchr(eap->arg, '+')
			&& ((buf->b_flags & BF_READERR) || !bufIsChanged(buf)))
		|| (vim_strchr(eap->arg, 'a')
			&& (buf->b_ml.ml_mfp == ଖାଲି || buf->b_nwindows == 0))
		|| (vim_strchr(eap->arg, 'h')
			&& (buf->b_ml.ml_mfp == ଖାଲି || buf->b_nwindows != 0))
#ifdef FEAT_TERMINAL
		|| (vim_strchr(eap->arg, 'R')
			&& (!job_running || (job_running && job_none_open)))
		|| (vim_strchr(eap->arg, '?')
			&& (!job_running || (job_running && !job_none_open)))
		|| (vim_strchr(eap->arg, 'F')
			&& (job_running || buf->b_term == ଖାଲି))
#endif
		|| (vim_strchr(eap->arg, '-') && buf->b_p_ma)
		|| (vim_strchr(eap->arg, '=') && !buf->b_p_ro)
		|| (vim_strchr(eap->arg, 'x') && !(buf->b_flags & BF_READERR))
		|| (vim_strchr(eap->arg, '%') && buf != curbuf)
		|| (vim_strchr(eap->arg, '#')
		      && (buf == curbuf || curwin->w_alt_fnum != buf->b_fnum)))
	    ଜାରି;
	name = buf_spname(buf);
	ଯଦି (name != ଖାଲି)
	    vim_strncpy(NameBuff, name, MAXPATHL - 1);
	ଅନ୍ୟଥା
	    home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
	ଯଦି (message_filtered(NameBuff))
	    ଜାରି;

	changed_char = (buf->b_flags & BF_READERR) ? 'x'
					     : (bufIsChanged(buf) ? '+' : ' ');
#ifdef FEAT_TERMINAL
	ଯଦି (job_running)
	{
	    ଯଦି (job_none_open)
		ro_char = '?';
	    ଅନ୍ୟଥା
		ro_char = 'R';
	    changed_char = ' ';  // bufIsChanged() returns TRUE to avoid
				 // closing, but it's not actually changed.
	}
	ଅନ୍ୟଥା ଯଦି (buf->b_term != ଖାଲି)
	    ro_char = 'F';
	ଅନ୍ୟଥା
#endif
	    ro_char = !buf->b_p_ma ? '-' : (buf->b_p_ro ? '=' : ' ');

	ସନ୍ଦେଶ_putchar('\n');
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vim_snprintf_safelen((ଅକ୍ଷର *)IObuff, IOSIZE - 20, "%3d%c%c%c%c%c \"%s\"",
		buf->b_fnum,
		buf->b_p_bl ? ' ' : 'u',
		buf == curbuf ? '%' :
			(curwin->w_alt_fnum == buf->b_fnum ? '#' : ' '),
		buf->b_ml.ml_mfp == ଖାଲି ? ' ' :
			(buf->b_nwindows == 0 ? 'h' : 'a'),
		ro_char,
		changed_char,
		NameBuff);

	// put "line 999" in column 40 or after the file name
	i = 40 - vim_strsize(IObuff);
	କର
	    IObuff[len++] = ' ';
	ଯେତେବେଳେ (--i > 0 && len < IOSIZE - 18);
#ifdef FEAT_VIMINFO
	ଯଦି (vim_strchr(eap->arg, 't') && buf->b_last_used)
	    add_time(IObuff + len, (size_t)(IOSIZE - len), buf->b_last_used);
	ଅନ୍ୟଥା
#endif
	    vim_snprintf((ଅକ୍ଷର *)IObuff + len, (size_t)(IOSIZE - len),
		    _("line %ld"), buf == curbuf ? curwin->w_cursor.lnum
					       : (long)buflist_findlnum(buf));
	ସନ୍ଦେଶ_outtrans(IObuff);
	out_flush();	    // output one line at a time
	ui_breakcheck();
    }

#ifdef FEAT_VIMINFO
    ଯଦି (buflist_data)
	ga_clear(&buflist);
#endif
}

/*
 * Get file name and line number for file 'fnum'.
 * Used by DoOneCmd() for translating '%' and '#'.
 * Used by insert_reg() and cmdline_paste() for '#' register.
 * Return FAIL if not found, OK for success.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buflist_name_nr(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fnum,
    char_u	**fname,
    linenr_T	*lnum)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ଯଦି (buf == ଖାଲି || buf->b_fname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    *fname = buf->b_fname;
    *lnum = buflist_findlnum(buf);

    ଫେରନ୍ତୁ OK;
}

/*
 * Set the file name for "buf"' to "ffname_arg", short file name to
 * "sfname_arg".
 * The file name with the full path is also remembered, for when :cd is used.
 * Returns FAIL for failure (file name already in use by other buffer)
 *	OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
setfname(
    buf_T	*buf,
    char_u	*ffname_arg,
    char_u	*sfname_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		message)	// give message when buffer already exists
{
    char_u	*ffname = ffname_arg;
    char_u	*sfname = sfname_arg;
    buf_T	*obuf = ଖାଲି;
#ifdef UNIX
    stat_T	st;
#endif

    ଯଦି (ffname == ଖାଲି || *ffname == NUL)
    {
	// Removing the name.
	ଯଦି (buf->b_sfname != buf->b_ffname)
	    VIM_CLEAR(buf->b_sfname);
	ଅନ୍ୟଥା
	    buf->b_sfname = ଖାଲି;
	VIM_CLEAR(buf->b_ffname);
#ifdef UNIX
	st.st_dev = (dev_T)-1;
#endif
    }
    ଅନ୍ୟଥା
    {
	fname_expand(buf, &ffname, &sfname); // will allocate ffname
	ଯଦି (ffname == ଖାଲି)		    // out of memory
	    ଫେରନ୍ତୁ FAIL;

	/*
	 * If the file name is already used in another buffer:
	 * - if the buffer is loaded, fail
	 * - if the buffer is not loaded, delete it from the list
	 */
#ifdef UNIX
	ଯଦି (mch_stat((ଅକ୍ଷର *)ffname, &st) < 0)
	    st.st_dev = (dev_T)-1;
#endif
	ଯଦି (!(buf->b_flags & BF_DUMMY))
#ifdef UNIX
	    obuf = buflist_findname_stat(ffname, &st);
#ଅନ୍ୟଥା
	    obuf = buflist_findname(ffname);
#endif
	ଯଦି (obuf != ଖାଲି && obuf != buf)
	{
	    win_T	*win;
	    tabpage_T   *tab;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		in_use = FALSE;

	    // during startup a window may use a buffer that is not loaded yet
	    FOR_ALL_TAB_WINDOWS(tab, win)
		ଯଦି (win->w_buffer == obuf)
		    in_use = TRUE;

	    // it's loaded or used in a window, fail
	    ଯଦି (obuf->b_ml.ml_mfp != ଖାଲି || in_use)
	    {
		ଯଦି (message)
		    emsg(_(e_buffer_with_this_name_already_exists));
		vim_free(ffname);
		ଫେରନ୍ତୁ FAIL;
	    }
	    // delete from the list
	    close_buffer(ଖାଲି, obuf, DOBUF_WIPE, FALSE, FALSE, FALSE);
	}
	sfname = vim_strsave(sfname);
	ଯଦି (ffname == ଖାଲି || sfname == ଖାଲି)
	{
	    vim_free(sfname);
	    vim_free(ffname);
	    ଫେରନ୍ତୁ FAIL;
	}
#ifdef USE_FNAME_CASE
	fname_case(sfname, 0);    // set correct case for short file name
#endif
	ଯଦି (buf->b_sfname != buf->b_ffname)
	    vim_free(buf->b_sfname);
	vim_free(buf->b_ffname);
	buf->b_ffname = ffname;
	buf->b_sfname = sfname;
    }
    buf->b_fname = buf->b_sfname;
#ifdef UNIX
    ଯଦି (st.st_dev == (dev_T)-1)
	buf->b_dev_valid = false;
    ଅନ୍ୟଥା
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
#endif

    buf->b_shortname = false;

    buf_name_changed(buf);
    ଫେରନ୍ତୁ OK;
}

/*
 * Crude way of changing the name of a buffer.  Use with care!
 * The name should be relative to the current directory.
 */
    ଶୂନ୍ୟ
buf_set_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fnum, char_u *name)
{
    buf_T	*buf;

    buf = buflist_findnr(fnum);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (buf->b_sfname != buf->b_ffname)
	vim_free(buf->b_sfname);
    vim_free(buf->b_ffname);
    buf->b_ffname = vim_strsave(name);
    buf->b_sfname = ଖାଲି;
    // Allocate ffname and expand into full path.  Also resolves .lnk
    // files on Win32.
    fname_expand(buf, &buf->b_ffname, &buf->b_sfname);
    buf->b_fname = buf->b_sfname;
}

/*
 * Take care of what needs to be done when the name of buffer "buf" has
 * changed.
 */
    ଶୂନ୍ୟ
buf_name_changed(buf_T *buf)
{
    /*
     * If the file name changed, also change the name of the swapfile
     */
    ଯଦି (buf->b_ml.ml_mfp != ଖାଲି)
	ml_setname(buf);

#ifdef FEAT_TERMINAL
    ଯଦି (buf->b_term != ଖାଲି)
	term_clear_status_text(buf->b_term);
#endif

    ଯଦି (curwin->w_buffer == buf)
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
    ଯଦି (buf != ଖାଲି && (cmdmod.cmod_flags & CMOD_KEEPALT) == 0)
	curwin->w_alt_fnum = buf->b_fnum;
    ଫେରନ୍ତୁ buf;
}

/*
 * Get alternate file name for current window.
 * Return NULL if there isn't any, and give error message if requested.
 */
    char_u  *
getaltfname(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		errmsg)		// give error message
{
    char_u	*fname;
    linenr_T	dummy;

    ଯଦି (buflist_name_nr(0, &fname, &dummy) == FAIL)
    {
	ଯଦି (errmsg)
	    emsg(_(e_no_alternate_file));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ fname;
}

/*
 * Add a file name to the buflist and return its number.
 * Uses same flags as buflist_new(), except BLN_DUMMY.
 *
 * used by qf_init(), main() and doarglist()
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buflist_add(char_u *fname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    buf_T	*buf;

    buf = buflist_new(fname, ଖାଲି, (linenr_T)0, flags);
    ଯଦି (buf != ଖାଲି)
	ଫେରନ୍ତୁ buf->b_fnum;
    ଫେରନ୍ତୁ 0;
}

#ଯଦି defined(BACKSLASH_IN_FILENAME)
/*
 * Adjust slashes in file names.  Called after 'shellslash' was set.
 */
    ଶୂନ୍ୟ
buflist_slash_adjust(ଶୂନ୍ୟ)
{
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	ଯଦି (bp->b_ffname != ଖାଲି)
	    slash_adjust(bp->b_ffname);
	ଯଦି (bp->b_sfname != ଖାଲି)
	    slash_adjust(bp->b_sfname);
    }
}
#endif

/*
 * Set alternate cursor position for the current buffer and window "win".
 * Also save the local window option values.
 */
    ଶୂନ୍ୟ
buflist_altfpos(win_T *win)
{
    buflist_setfpos(curbuf, win, win->w_cursor.lnum, win->w_cursor.col, TRUE);
}

/*
 * Return TRUE if 'ffname' is not the same file as current file.
 * Fname must have a full path (expanded by mch_FullName()).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
otherfile(char_u *ffname)
{
    ଫେରନ୍ତୁ otherfile_buf(curbuf, ffname
#ifdef UNIX
	    , ଖାଲି
#endif
	    );
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
otherfile_buf(
    buf_T		*buf,
    char_u		*ffname
#ifdef UNIX
    , stat_T		*stp
#endif
    )
{
    // no name is different
    ଯଦି (ffname == ଖାଲି || *ffname == NUL || buf->b_ffname == ଖାଲି)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (fnamecmp(ffname, buf->b_ffname) == 0)
	ଫେରନ୍ତୁ FALSE;
#ifdef UNIX
    {
	stat_T	    st;

	// If no stat_T given, get it now
	ଯଦି (stp == ଖାଲି)
	{
	    ଯଦି (!buf->b_dev_valid || mch_stat((ଅକ୍ଷର *)ffname, &st) < 0)
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
	ଯଦି (buf_same_ino(buf, stp))
	{
	    buf_setino(buf);
	    ଯଦି (buf_same_ino(buf, stp))
		ଫେରନ୍ତୁ FALSE;
	}
    }
#endif
    ଫେରନ୍ତୁ TRUE;
}

#ଯଦି defined(UNIX)
/*
 * Set inode and device number for a buffer.
 * Must always be called when b_fname is changed!.
 */
    ଶୂନ୍ୟ
buf_setino(buf_T *buf)
{
    stat_T	st;

    ଯଦି (buf->b_fname != ଖାଲି && mch_stat((ଅକ୍ଷର *)buf->b_fname, &st) >= 0)
    {
	buf->b_dev_valid = true;
	buf->b_dev = st.st_dev;
	buf->b_ino = st.st_ino;
    }
    ଅନ୍ୟଥା
	buf->b_dev_valid = false;
}

/*
 * Return TRUE if dev/ino in buffer "buf" matches with "stp".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_same_ino(
    buf_T	*buf,
    stat_T	*stp)
{
    ଫେରନ୍ତୁ (buf->b_dev_valid
	    && stp->st_dev == buf->b_dev
	    && stp->st_ino == buf->b_ino);
}
#endif

/*
 * Print info about the current buffer.
 */
    ଶୂନ୍ୟ
fileinfo(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fullname,	    // when non-zero print full path
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା shorthelp,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dont_truncate)
{
    char_u	*name;
    ଅକ୍ଷର	*buffer;
    size_t	bufferlen = 0;

    buffer = alloc(IOSIZE);
    ଯଦି (buffer == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (fullname > 1)	    // 2 CTRL-G: include buffer number
	bufferlen = vim_snprintf_safelen(buffer, IOSIZE, "buf %d: ", curbuf->b_fnum);

    buffer[bufferlen++] = '"';

    name = buf_spname(curbuf);
    ଯଦି (name != ଖାଲି)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", name);
    ଅନ୍ୟଥା
    {
	ଯଦି (!fullname && curbuf->b_fname != ଖାଲି)
	    name = curbuf->b_fname;
	ଅନ୍ୟଥା
	    name = curbuf->b_ffname;
	bufferlen += home_replace(shorthelp ? curbuf : ଖାଲି, name,
	    (char_u *)buffer + bufferlen, IOSIZE - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)bufferlen, TRUE);
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

    ଯଦି (curbuf->b_ml.ml_flags & ML_EMPTY)
	bufferlen += vim_snprintf_safelen(buffer + bufferlen,
	    IOSIZE - bufferlen, "%s", _(no_ଧାଡ଼ିଗୁଡ଼ିକ_ସନ୍ଦେଶ));
    ଅନ୍ୟଥା ଯଦି (p_ru)
	// Current line and column are already on the screen -- webb
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    NGETTEXT("%ld line --%d%%--", "%ld lines --%d%%--", curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା),
	    (long)curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା));
    ଅନ୍ୟଥା
    {
	bufferlen += vim_snprintf_safelen(
	    buffer + bufferlen,
	    IOSIZE - bufferlen,
	    _("line %ld of %ld --%d%%-- col "),
	    (long)curwin->w_cursor.lnum,
	    (long)curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା,
	    calc_percentage(curwin->w_cursor.lnum, curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା));

	validate_virtcol();
	bufferlen += col_print((char_u *)buffer + bufferlen, IOSIZE - bufferlen,
		   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_cursor.col + 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curwin->w_virtcol + 1);
    }

    (ଶୂନ୍ୟ)append_arg_ସଂଖ୍ୟା(curwin, (char_u *)buffer + bufferlen,
	IOSIZE - bufferlen, !shortmess(SHM_FILE));

    ଯଦି (dont_truncate)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n;

	// Temporarily set msg_scroll to avoid the message being truncated.
	// First call msg_start() to get the message in the right place.
	ସନ୍ଦେଶ_start();
	n = ସନ୍ଦେଶ_scroll;
	ସନ୍ଦେଶ_scroll = TRUE;
	ସନ୍ଦେଶ(buffer);
	ସନ୍ଦେଶ_scroll = n;
    }
    ଅନ୍ୟଥା
    {
	ଅକ୍ଷର	*p = ସନ୍ଦେଶ_trunc_attr(buffer, FALSE, 0);
	ଯଦି (restart_edit != 0 || (ସନ୍ଦେଶ_scrolled && !need_wait_return))
	    // Need to repeat the message after redrawing when:
	    // - When restart_edit is set (otherwise there will be a delay
	    //   before redrawing).
	    // - When the screen was scrolled but there is no wait-return
	    //   prompt.
	    set_keep_ସନ୍ଦେଶ((char_u *)p, 0);
    }

    vim_free(buffer);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
col_print(
    char_u  *buf,
    size_t  buflen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    col,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    vcol)
{
    ଯଦି (col == vcol)
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vim_snprintf_safelen((ଅକ୍ଷର *)buf, buflen, "%d", col);

    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vim_snprintf_safelen((ଅକ୍ଷର *)buf, buflen, "%d-%d", col, vcol);
}

ସ୍ଥିର char_u *lasttitle = ଖାଲି;
ସ୍ଥିର char_u *lasticon = ଖାଲି;

/*
 * Put the file name in the title bar and icon of the window.
 */
    ଶୂନ୍ୟ
maketitle(ଶୂନ୍ୟ)
{
    char_u	*title_str = ଖାଲି;
    char_u	*icon_str = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mustset;
    char_u	buf[IOSIZE];
    size_t	buflen = 0;

    ଯଦି (!redrawing())
    {
	// Postpone updating the title when 'lazyredraw' is set.
	need_maketitle = TRUE;
	ଫେରନ୍ତୁ;
    }

    need_maketitle = FALSE;
    ଯଦି (!p_title && !p_icon && lasttitle == ଖାଲି && lasticon == ଖାଲି)
	ଫେରନ୍ତୁ;  // nothing to do

    ଯଦି (p_title)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxlen = 0;

	ଯଦି (p_titlelen > 0)
	{
	    maxlen = p_titlelen * Columns / 100;
	    ଯଦି (maxlen < 10)
		maxlen = 10;
	}

	title_str = buf;
	ଯଦି (*p_titlestring != NUL)
	{
#ifdef FEAT_STL_OPT
	    ଯଦି (stl_syntax & STL_IN_TITLE)
		build_stl_str_hl(curwin, title_str, ଆକାର(buf), p_titlestring,
				    (char_u *)"titlestring", 0,
				    0, maxlen, ଖାଲି, ଖାଲି, ଖାଲି);
	    ଅନ୍ୟଥା
#endif
		title_str = p_titlestring;
	    buflen = STRLEN(title_str);
	}
	ଅନ୍ୟଥା
	{
	    char_u  *p;

	    // format: "<filename> [flags] <(path)> [argument info] <- servername>"
	    // example:
	    //	    buffer.c + (/home/vim/src) (1 of 2) - VIM

	    // reserve some space for different parts of the title.
	    // use sizeof() to introduce 'size_t' so we don't have to
	    // cast sizes to it.
#define SPACE_FOR_FNAME (ଆକାର(buf) - 100)
#define SPACE_FOR_DIR   (ଆକାର(buf) - 20)
#define SPACE_FOR_ARGNR (ଆକାର(buf) - 10)  // at least room for " - VIM"

	    // file name
	    ଯଦି (curbuf->b_fname == ଖାଲି)
		buflen = vim_snprintf_safelen((ଅକ୍ଷର *)buf,
		    SPACE_FOR_FNAME, "%s", _("[No Name]"));
#ifdef FEAT_TERMINAL
	    ଅନ୍ୟଥା ଯଦି (curbuf->b_term != ଖାଲି)
		buflen = vim_snprintf_safelen((ଅକ୍ଷର *)buf,
		    SPACE_FOR_FNAME, "%s",
		    term_get_status_text(curbuf->b_term));
#endif
	    ଅନ୍ୟଥା
	    {
		buflen = vim_snprintf_safelen((ଅକ୍ଷର *)buf,
		    SPACE_FOR_FNAME, "%s",
		    ((p = transstr(gettail(curbuf->b_fname))) != ଖାଲି)
			? p
			: (char_u *)"");
		vim_free(p);
	    }

	    // flags
#ifdef FEAT_TERMINAL
	    ଯଦି (curbuf->b_term == ଖାଲି)
#endif
	    {
		ବାଛ (bufIsChanged(curbuf)
			+ (curbuf->b_p_ro * 2)
			+ (!curbuf->b_p_ma * 4))
		{
		    ସ୍ଥିତି 1:
			// file was modified
			buflen += vim_snprintf_safelen(
			    (ଅକ୍ଷର *)buf + buflen,
			    ଆକାର(buf) - buflen, " +");
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 2:
			// file is readonly
			buflen += vim_snprintf_safelen(
			    (ଅକ୍ଷର *)buf + buflen,
			    ଆକାର(buf) - buflen, " =");
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 3:
			// file was modified and is readonly
			buflen += vim_snprintf_safelen(
			    (ଅକ୍ଷର *)buf + buflen,
			    ଆକାର(buf) - buflen, " =+");
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 4:
		    ସ୍ଥିତି 6:
			// file cannot be modified
			buflen += vim_snprintf_safelen(
			    (ଅକ୍ଷର *)buf + buflen,
			    ଆକାର(buf) - buflen, " -");
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି 5:
		    ସ୍ଥିତି 7:
			// file cannot be modified but was modified
			buflen += vim_snprintf_safelen(
			    (ଅକ୍ଷର *)buf + buflen,
			    ଆକାର(buf) - buflen, " -+");
			ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}
	    }

	    // path (surrounded by '()')
	    ଯଦି (curbuf->b_fname != ଖାଲି
#ifdef FEAT_TERMINAL
		    && curbuf->b_term == ଖାଲି
#endif
		    )
	    {
		// Get path of file, replace home dir with ~
		buflen += vim_snprintf_safelen((ଅକ୍ଷର *)buf + buflen,
		    ଆକାର(buf) - buflen, " (");

		home_replace(curbuf, curbuf->b_ffname,
		    buf + buflen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(SPACE_FOR_DIR - buflen), TRUE);

#ifdef BACKSLASH_IN_FILENAME
		// avoid "c:/name" to be reduced to "c"
		ଯଦି (SAFE_isalpha(buf[buflen]) && buf[buflen + 1] == ':')
		    buflen += 2;			// step over "c:"
#endif

		// determine if we have a help or normal buffer
		p = gettail_sep(buf + buflen);
		ଯଦି (p == buf + buflen)
		{
		    // help buffer
		    buflen += vim_snprintf_safelen((ଅକ୍ଷର *)buf + buflen,
			SPACE_FOR_DIR - buflen, "%s)", _("help"));
		}
		ଅନ୍ୟଥା
		{
		    // normal buffer

		    // Translate unprintable chars and concatenate.  Keep some
		    // room for the server name.  When there is no room (very long
		    // file name) use (...).
		    ଯଦି (buflen < SPACE_FOR_DIR)
		    {
			// remove the file name
			*p = NUL;

			buflen += vim_snprintf_safelen((ଅକ୍ଷର *)buf + buflen,
			    SPACE_FOR_DIR - buflen, "%s)",
			    ((p = transstr(buf + buflen)) != ଖାଲି)
				? p
				: (char_u *)"");
			vim_free(p);
		    }
		    ଅନ୍ୟଥା
			buflen += vim_snprintf_safelen((ଅକ୍ଷର *)buf + buflen,
			    SPACE_FOR_ARGNR - buflen, "...)");
		}
	    }

	    // argument info
	    buflen += append_arg_ସଂଖ୍ୟା(curwin, buf + buflen,
		SPACE_FOR_ARGNR - buflen, FALSE);

	    // servername
	    buflen += vim_snprintf_safelen((ଅକ୍ଷର *)buf + buflen,
		ଆକାର(buf) - buflen, " - %s",
#ଯଦି defined(FEAT_CLIENTSERVER)
		(serverName != ଖାଲି)
		    ? serverName :
#endif
		    (char_u *)"VIM");

	    ଯଦି (maxlen > 0)
	    {
		// make it shorter by removing a bit in the middle
		ଯଦି (vim_strsize(buf) > maxlen)
		    trunc_string(buf, buf, maxlen, ଆକାର(buf));
	    }
	}
    }
    mustset = ମୂଲ୍ୟ_changed(title_str, &lasttitle);

    ଯଦି (p_icon)
    {
	icon_str = buf;
	ଯଦି (*p_iconstring != NUL)
	{
#ifdef FEAT_STL_OPT
	    ଯଦି (stl_syntax & STL_IN_ICON)
		build_stl_str_hl(curwin, icon_str, ଆକାର(buf), p_iconstring,
				 (char_u *)"iconstring", 0, 0, 0, ଖାଲି, ଖାଲି,
				 ଖାଲି);
	    ଅନ୍ୟଥା
#endif
		icon_str = p_iconstring;
	}
	ଅନ୍ୟଥା
	{
	    char_u  *name;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    namelen;

	    name = buf_spname(curbuf);
	    ଯଦି (name == ଖାଲି)
		name = gettail(curbuf->b_ffname);
	    // Truncate name at 100 bytes.
	    namelen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name);
	    ଯଦି (namelen > 100)
	    {
		namelen -= 100;
		ଯଦି (has_mbyte)
		    namelen += (*mb_tail_off)(name, name + namelen) + 1;
		name += namelen;
	    }
	    STRCPY(buf, name);
	    trans_characters(buf, ଆକାର(buf));
	}
    }

    mustset |= ମୂଲ୍ୟ_changed(icon_str, &lasticon);

    ଯଦି (mustset)
	resettitle();
}

/*
 * Used for title and icon: Check if "str" differs from "*last".  Set "*last"
 * from "str" if it does.
 * Return TRUE if resettitle() is to be called.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୂଲ୍ୟ_changed(char_u *str, char_u **last)
{
    ଯଦି ((str == ଖାଲି) != (*last == ଖାଲି)
	    || (str != ଖାଲି && *last != ଖାଲି && STRCMP(str, *last) != 0))
    {
	vim_free(*last);
	ଯଦି (str == ଖାଲି)
	{
	    *last = ଖାଲି;
	    mch_restore_title(
		  last == &lasttitle ? SAVE_RESTORE_TITLE : SAVE_RESTORE_ICON);
	}
	ଅନ୍ୟଥା
	{
	    *last = vim_strsave(str);
	    ଫେରନ୍ତୁ TRUE;
	}
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Put current window title back (used after calling a shell)
 */
    ଶୂନ୍ୟ
resettitle(ଶୂନ୍ୟ)
{
    mch_settitle(lasttitle, lasticon);
}

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
free_titles(ଶୂନ୍ୟ)
{
    vim_free(lasttitle);
    vim_free(lasticon);
}
#endif


#ଯଦି defined(FEAT_STL_OPT) || defined(FEAT_GUI_TABLINE)

/*
 * Used for building in the status line.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    char_u	*stl_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stl_minwid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stl_maxwid;
    ଗଣନା {
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

ସ୍ଥିର size_t		stl_ବସ୍ତୁଗୁଡ଼ିକ_len = 20; // Initial value, grows as needed.
ସ୍ଥିର stl_item_T      *stl_ବସ୍ତୁଗୁଡ଼ିକ = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	       *stl_groupitem = ଖାଲି;
ସ୍ଥିର stl_hlrec_T     *stl_hltab = ଖାଲି;
ସ୍ଥିର stl_hlrec_T     *stl_tabtab = ଖାଲି;
ସ୍ଥିର stl_clickrec_T  *stl_clicktab = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*stl_separator_locations = ଖାଲି;

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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
build_stl_str_hl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_scope,	// scope for "opt_name"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fillchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab)	// return: click func regions (can be NULL)
{
    ଫେରନ୍ତୁ build_stl_str_hl_local(STL_MODE_SINGLE, wp, out, outlen, &fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ଖାଲି, ଖାଲି);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
build_stl_str_hl_mline(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_scope,	// scope for "opt_name"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fillchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ଫେରନ୍ତୁ build_stl_str_hl_local(STL_MODE_MULTI, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ଖାଲି, carry_hl);
}

# ifdef ENABLE_STL_MODE_MULTI_NL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
build_stl_str_hl_mline_nl(
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt,		// (in/out)
    char_u	*opt_name,      // option name corresponding to "fmt"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_scope,	// scope for "opt_name"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fillchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    ଫେରନ୍ତୁ build_stl_str_hl_local(STL_MODE_MULTI_NL, wp, out, outlen, fmt,
	    opt_name, opt_scope, fillchar, maxwidth, hltab, tabtab, clicktab,
	    ଖାଲି, carry_hl);
}
# endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_stl_rendered_height(
    win_T	*wp,
    char_u	*fmt,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_scope)	// scope for "opt_name"
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rendered_height = 0;
    char_u	buf[MAXPATHL] = {0};

    // Suppress errors while computing height: expressions (%!expr, %{expr})
    // are evaluated here only to count rendered lines, not for their side
    // effects.  Any errors (e.g. E48 in sandbox) will surface properly when
    // the statusline is actually drawn.
    ++emsg_off;
    (ଶୂନ୍ୟ)build_stl_str_hl_local(STL_MODE_GET_RENDERED_HEIGHT,
	    wp, buf, ଆକାର(buf), &fmt,
	    opt_name, opt_scope, 0, 0, ଖାଲି, ଖାଲି, ଖାଲି, &rendered_height,
	    ଖାଲି);
    --emsg_off;
    ଫେରନ୍ତୁ rendered_height;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
build_stl_str_hl_local(
    stl_mode_T	mode,
    win_T	*wp,
    char_u	*out,		// buffer to write into != NameBuff
    size_t	outlen,		// length of out[]
    char_u	**fmt_arg,
    char_u	*opt_name,      // option name corresponding to "fmt"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_scope,	// scope for "opt_name"
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fillchar,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxwidth,
    stl_hlrec_T **hltab,	// return: HL attributes (can be NULL)
    stl_hlrec_T **tabtab,	// return: tab page nrs (can be NULL)
    stl_clickrec_T **clicktab,	// return: click func regions (can be NULL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*rendered_height,   // return: stl rendered height (can be NULL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*carry_hl)	// (in/out) %# / %* highlight carried across
				// line breaks (can be NULL)
{
    linenr_T	lnum;
    colnr_T	len;
    size_t	outputlen;	// length of out[] used (excluding the NUL)
    char_u	*p;
    char_u	*s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		byteval;
# ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_sandbox;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_VIsual_active;
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty_ଧାଡ଼ି;
    long	l;
    long	n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prevchar_isflag;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prevchar_isitem;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		itemisflag;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fillable;
    char_u	*str;
    long	ଅଙ୍କ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		itemcnt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curitem;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		groupdepth;
# ifdef FEAT_EVAL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaldepth;
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		minwid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxwid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		zeropad;
    char_u	base;
    char_u	opt;
# define TMPLEN 70
    char_u	buf_tmp[TMPLEN];
    char_u	*fmt = *fmt_arg;
    char_u	*usefmt = fmt;
    stl_hlrec_T *sp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_redraw_not_allowed = redraw_not_allowed;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ଚାବି_Typed = ଚାବି_Typed;
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // int	called_emsg_before = called_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rheight = 1;	// stl rendered height

    // When inside update_screen() we do not want redrawing a statusline,
    // ruler, title, etc. to trigger another redraw, it may cause an endless
    // loop.
    ଯଦି (updating_screen)
	redraw_not_allowed = TRUE;

    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ == ଖାଲି)
    {
	stl_ବସ୍ତୁଗୁଡ଼ିକ = ALLOC_MULT(stl_item_T, stl_ବସ୍ତୁଗୁଡ଼ିକ_len);
	stl_groupitem = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, stl_ବସ୍ତୁଗୁଡ଼ିକ_len);

	// Allocate one more, because the last element is used to indicate the
	// end of the list.
	stl_hltab  = ALLOC_MULT(stl_hlrec_T, stl_ବସ୍ତୁଗୁଡ଼ିକ_len + 1);
	stl_tabtab = ALLOC_MULT(stl_hlrec_T, stl_ବସ୍ତୁଗୁଡ଼ିକ_len + 1);
	stl_clicktab = ALLOC_MULT(stl_clickrec_T, stl_ବସ୍ତୁଗୁଡ଼ିକ_len + 1);

	stl_separator_locations = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, stl_ବସ୍ତୁଗୁଡ଼ିକ_len);
    }

# ifdef FEAT_EVAL
    // if "fmt" was set insecurely it needs to be evaluated in the sandbox
    use_sandbox = was_set_insecurely(wp, opt_name, opt_scope);

    // When the format starts with "%!" then evaluate it as an expression and
    // use the result as the actual format string.
    ଯଦି (fmt[0] == '%' && fmt[1] == '!')
    {
	typval_T	tv;

	tv.v_type = VAR_ସଂଖ୍ୟା;
	tv.vval.v_ସଂଖ୍ୟା = wp->w_id;
	set_var((char_u *)"g:statusline_winid", &tv, FALSE);

	usefmt = eval_to_string_safe(fmt + 2, use_sandbox, FALSE, FALSE);
	ଯଦି (usefmt == ଖାଲି)
	    usefmt = fmt;

	do_unlet((char_u *)"g:statusline_winid", TRUE);
    }
# endif

    ଯଦି (fillchar == 0)
	fillchar = ' ';

    // The cursor in windows other than the current one isn't always
    // up-to-date, esp. because of autocommands and timers.
    lnum = wp->w_cursor.lnum;
    ଯଦି (lnum > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
    {
	lnum = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	wp->w_cursor.lnum = lnum;
    }

    // Get line & check if empty (cursorpos will show "0-1").  Note that
    // p will become invalid when getting another buffer line.
    p = ml_get_buf(wp->w_buffer, lnum, FALSE);
    empty_ଧାଡ଼ି = (*p == NUL);

    // Get the byte value now, in case we need it below. This is more efficient
    // than making a copy of the line.
    len = ml_get_buf_len(wp->w_buffer, lnum);
    ଯଦି (wp->w_cursor.col > len)
    {
	// Line may have changed since checking the cursor column, or the lnum
	// was adjusted above.
	wp->w_cursor.col = len;
	wp->w_cursor.coladd = 0;
	byteval = 0;
    }
    ଅନ୍ୟଥା
	byteval = (*mb_ptr2char)(p + wp->w_cursor.col);

    groupdepth = 0;
# ifdef FEAT_EVAL
    evaldepth = 0;
# endif
    p = out;
    curitem = 0;

    // Pre-insert a Highlight item from carry_hl so that %# / %* set on a
    // previous multi-line statusline row continues to apply on this row.
    ଯଦି (carry_hl != ଖାଲି && *carry_hl != 0)
    {
	stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = Highlight;
	stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_start = p;
	stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_minwid = *carry_hl;
	stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_clickfunc = ଖାଲି;
	curitem++;
    }

    prevchar_isflag = TRUE;
    prevchar_isitem = FALSE;
    ପାଇଁ (s = usefmt; *s != NUL; )
    {
	ଯଦି (curitem == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)stl_ବସ୍ତୁଗୁଡ଼ିକ_len)
	{
	    size_t	new_len = stl_ବସ୍ତୁଗୁଡ଼ିକ_len * 3 / 2;

	    stl_item_T *new_ବସ୍ତୁଗୁଡ଼ିକ =
			  vim_realloc(stl_ବସ୍ତୁଗୁଡ଼ିକ, ଆକାର(stl_item_T) * new_len);
	    ଯଦି (new_ବସ୍ତୁଗୁଡ଼ିକ == ଖାଲି)
		ଭାଙ୍ଗ;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ = new_ବସ୍ତୁଗୁଡ଼ିକ;

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *new_groupitem =
			     vim_realloc(stl_groupitem, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * new_len);
	    ଯଦି (new_groupitem == ଖାଲି)
		ଭାଙ୍ଗ;
	    stl_groupitem = new_groupitem;

	    stl_hlrec_T	*new_hlrec = vim_realloc(stl_hltab,
					  ଆକାର(stl_hlrec_T) * (new_len + 1));
	    ଯଦି (new_hlrec == ଖାଲି)
		ଭାଙ୍ଗ;
	    stl_hltab = new_hlrec;
	    new_hlrec = vim_realloc(stl_tabtab,
					  ଆକାର(stl_hlrec_T) * (new_len + 1));
	    ଯଦି (new_hlrec == ଖାଲି)
		ଭାଙ୍ଗ;
	    stl_tabtab = new_hlrec;

	    stl_clickrec_T *new_clickrec = vim_realloc(stl_clicktab,
				      ଆକାର(stl_clickrec_T) * (new_len + 1));
	    ଯଦି (new_clickrec == ଖାଲି)
		ଭାଙ୍ଗ;
	    stl_clicktab = new_clickrec;

	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *new_separator_locs = vim_realloc(stl_separator_locations,
					    ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * new_len);
	    ଯଦି (new_separator_locs == ଖାଲି)
		ଭାଙ୍ଗ;
	    stl_separator_locations = new_separator_locs;

	    stl_ବସ୍ତୁଗୁଡ଼ିକ_len = new_len;
	}

	stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_clickfunc = ଖାଲି;

	ଯଦି (*s != '%')
	    prevchar_isflag = prevchar_isitem = FALSE;

	/*
	 * Handle up to the next '%' or the end.
	 */
	ଯେତେବେଳେ (*s != NUL && *s != '%' && p + 1 < out + outlen)
# ifdef ENABLE_STL_MODE_MULTI_NL
	{
	    ଯଦି (*s == '\n' || *s == '\r')
	    {
		ଯଦି (mode == STL_MODE_MULTI_NL)
		{
		    s++;
		    ଯାଅ find_linebreak;
		}
		ଅନ୍ୟଥା ଯଦି (mode == STL_MODE_GET_RENDERED_HEIGHT)
		    rheight++;
	    }
# endif
	    *p++ = *s++;
# ifdef ENABLE_STL_MODE_MULTI_NL
	}
# endif
	ଯଦି (*s == NUL || p + 1 >= out + outlen)
	    ଭାଙ୍ଗ;

	/*
	 * Handle one '%' item.
	 */
	s++;
	ଯଦି (*s == NUL)  // ignore trailing %
	    ଭାଙ୍ଗ;

	ଯଦି (*s == STL_CLICKFUNC)
	{
	    // %[] - end click region
	    ଯଦି (s[1] == ']')
	    {
		stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = ClickFunc;
		stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_start = p;
		stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_minwid = 0;
		stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_clickfunc = ଖାଲି;
		s += 2;
		curitem++;
		ଜାରି;
	    }
	    // %[FuncName] - start click region
	    ଯଦି (ASCII_ISALPHA(s[1]) || s[1] == '_')
	    {
		char_u *rb = vim_strchr(s + 1, ']');
		ଯଦି (rb != ଖାଲି)
		{
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = ClickFunc;
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_start = p;
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_minwid = 0;
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_clickfunc =
					  vim_strnsave(s + 1, rb - s - 1);
		    s = rb + 1;
		    curitem++;
		    ଜାରି;
		}
	    }
	}
	ଯଦି (*s == STL_LINEBREAK)
	{
	    // Plain %@ - line break
	    ଯଦି (mode == STL_MODE_MULTI
# ifdef ENABLE_STL_MODE_MULTI_NL
		    || mode == STL_MODE_MULTI_NL
# endif
	       )
	    {
		s++;
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (mode == STL_MODE_GET_RENDERED_HEIGHT)
		rheight++;
	}
	ଯଦି (*s == '%')
	{
	    ଯଦି (p + 1 >= out + outlen)
		ଭାଙ୍ଗ;
	    *p++ = *s++;
	    prevchar_isflag = prevchar_isitem = FALSE;
	    ଜାରି;
	}
	// STL_SEPARATE: Separation between items, filled with white space.
	ଯଦି (*s == STL_SEPARATE)
	{
	    s++;
	    ଯଦି (groupdepth > 0)
		ଜାରି;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = Separate;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem++].stl_start = p;
	    ଜାରି;
	}
	ଯଦି (*s == STL_TRUNCMARK)
	{
	    s++;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = Trunc;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem++].stl_start = p;
	    ଜାରି;
	}
	ଯଦି (*s == ')')
	{
	    char_u  *t;

	    s++;
	    ଯଦି (groupdepth < 1)
		ଜାରି;
	    groupdepth--;

	    t = stl_ବସ୍ତୁଗୁଡ଼ିକ[stl_groupitem[groupdepth]].stl_start;
	    *p = NUL;
	    l = vim_strsize(t);
	    ଯଦି (curitem > stl_groupitem[groupdepth] + 1
		    && stl_ବସ୍ତୁଗୁଡ଼ିକ[stl_groupitem[groupdepth]].stl_minwid == 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା group_start_userhl = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା group_end_userhl = 0;

		// remove group if all items are empty and highlight group
		// doesn't change
		ପାଇଁ (n = stl_groupitem[groupdepth] - 1; n >= 0; n--)
		{
		    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_type == Highlight)
		    {
			group_start_userhl = group_end_userhl =
						       stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_minwid;
			ଭାଙ୍ଗ;
		    }
		}
		ପାଇଁ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		{
		    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_type == Normal)
			ଭାଙ୍ଗ;
		    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_type == Highlight)
			group_end_userhl = stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_minwid;
		}
		ଯଦି (n == curitem && group_start_userhl == group_end_userhl)
		{
		    // empty group
		    p = t;
		    l = 0;
		    ପାଇଁ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
		    {
			// do not use the highlighting from the removed group
			ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_type == Highlight)
			    stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_type = Empty;
			// adjust the start position of TabPage to the next
			// item position
			ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_type == TabPage)
			    stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_start = p;
		    }
		}
	    }
	    ଯଦି (l > stl_ବସ୍ତୁଗୁଡ଼ିକ[stl_groupitem[groupdepth]].stl_maxwid)
	    {
		// truncate, remove n bytes of text at the start
		ଯଦି (has_mbyte)
		{
		    // Find the first character that should be included.
		    n = 0;
		    ଯେତେବେଳେ (l >= stl_ବସ୍ତୁଗୁଡ଼ିକ[stl_groupitem[groupdepth]].stl_maxwid)
		    {
			l -= ptr2cells(t + n);
			n += (*mb_ptr2len)(t + n);
		    }
		}
		ଅନ୍ୟଥା
		    n = (long)(p - t) - stl_ବସ୍ତୁଗୁଡ଼ିକ[stl_groupitem[groupdepth]]
							       .stl_maxwid + 1;

		*t = '<';
		mch_memmove(t + 1, t + n, (size_t)(p - (t + n)));
		p = p - n + 1;

		// Fill up space left over by half a double-wide char.
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା minwid_fixed = MIN(stl_ବସ୍ତୁଗୁଡ଼ିକ[stl_groupitem[groupdepth]].stl_minwid,
				       stl_ବସ୍ତୁଗୁଡ଼ିକ[stl_groupitem[groupdepth]].stl_maxwid);
		ଯେତେବେଳେ (++l < minwid_fixed)
		    MB_CHAR2BYTES(fillchar, p);

		// correct the start of the items for the truncation
		ପାଇଁ (l = stl_groupitem[groupdepth] + 1; l < curitem; l++)
		{
		    // Minus one for the leading '<' added above.
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start -= n - 1;
		    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start < t)
			stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start = t;
		}
	    }
	    ଅନ୍ୟଥା ଯଦି (abs(stl_ବସ୍ତୁଗୁଡ଼ିକ[stl_groupitem[groupdepth]].stl_minwid) > l)
	    {
		// fill
		n = stl_ବସ୍ତୁଗୁଡ଼ିକ[stl_groupitem[groupdepth]].stl_minwid;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା fillchar_len = MB_CHAR2LEN(fillchar);
		ଯଦି (n < 0)
		{
		    // fill by appending characters
		    n = 0 - n;
		    ଯେତେବେଳେ (l++ < n && p + fillchar_len < out + outlen)
			MB_CHAR2BYTES(fillchar, p);
		}
		ଅନ୍ୟଥା
		{
		    // fill by inserting characters
		    n = n - l;
		    l = n * fillchar_len;
		    ଯଦି (p + l >= out + outlen)
		    {
			n = (long)((out + outlen) - p - 1) / fillchar_len;
			l = n * fillchar_len;
		    }
		    mch_memmove(t + l, t, (size_t)(p - t));
		    p += l;
		    ପାଇଁ ( ; n > 0; n--)
			MB_CHAR2BYTES(fillchar, t);
		    ପାଇଁ (n = stl_groupitem[groupdepth] + 1; n < curitem; n++)
			stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_start += l;
		}
	    }
	    ଜାରି;
	}
	minwid = 0;
	maxwid = 9999;
	zeropad = FALSE;
	l = 1;
	ଯଦି (*s == '0')
	{
	    s++;
	    zeropad = TRUE;
	}
	ଯଦି (*s == '-')
	{
	    s++;
	    l = -1;
	}
	ଯଦି (VIM_ISDIGIT(*s))
	{
	    minwid = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)getdigits(&s);
	    ଯଦି (minwid < 0)	// overflow
		minwid = 0;
	}
	ଯଦି (*s == STL_USER_HL)
	{
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = Highlight;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_start = p;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_minwid = minwid > 9 ? 1 : minwid;
	    s++;
	    curitem++;
	    ଜାରି;
	}
	ଯଦି (*s == STL_TABPAGENR || *s == STL_TABCLOSENR)
	{
	    ଯଦି (*s == STL_TABCLOSENR)
	    {
		ଯଦି (minwid == 0)
		{
		    // %X ends the close label, go back to the previously
		    // define tab label nr.
		    ପାଇଁ (n = curitem - 1; n >= 0; --n)
			ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_type == TabPage
					       && stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_minwid >= 0)
			{
			    minwid = stl_ବସ୍ତୁଗୁଡ଼ିକ[n].stl_minwid;
			    ଭାଙ୍ଗ;
			}
		}
		ଅନ୍ୟଥା
		    // close nrs are stored as negative values
		    minwid = - minwid;
	    }
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = TabPage;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_start = p;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_minwid = minwid;
	    s++;
	    curitem++;
	    ଜାରି;
	}
	ଯଦି (*s == '.')
	{
	    s++;
	    ଯଦି (VIM_ISDIGIT(*s))
	    {
		maxwid = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)getdigits(&s);
		ଯଦି (maxwid <= 0)	// overflow
		    maxwid = 50;
	    }
	}
	// Keep the uncapped value for %N[FuncName] click-region IDs; the 50
	// cap below applies only when minwid is used as a padding width.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା raw_minwid = minwid * l;
	minwid = (minwid > 50 ? 50 : minwid) * l;
	ଯଦି (*s == '(')
	{
	    stl_groupitem[groupdepth++] = curitem;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = Group;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_start = p;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_minwid = minwid;
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_maxwid = maxwid;
	    s++;
	    curitem++;
	    ଜାରି;
	}
# ifdef FEAT_EVAL
	// Denotes end of expanded %{} block
	ଯଦି (*s == '}' && evaldepth > 0)
	{
	    s++;
	    evaldepth--;
	    ଜାରି;
	}
# endif
	ଯଦି (vim_strchr(STL_ALL, *s) == ଖାଲି)
	{
	    ଯଦି (*s == NUL)  // can happen with "%0"
		ଭାଙ୍ଗ;
	    s++;
	    ଜାରି;
	}
	opt = *s++;

	// OK - now for the real work
	base = 'D';
	itemisflag = FALSE;
	fillable = TRUE;
	ଅଙ୍କ = -1;
	str = ଖାଲି;
	ବାଛ (opt)
	{
	ସ୍ଥିତି STL_FILEPATH:
	ସ୍ଥିତି STL_FULLPATH:
	ସ୍ଥିତି STL_FILENAME:
	{
	    char_u  *name;

	    fillable = FALSE;	// don't change ' ' to fillchar
	    name = buf_spname(wp->w_buffer);
	    ଯଦି (name != ଖାଲି)
		vim_strncpy(NameBuff, name, MAXPATHL - 1);
	    ଅନ୍ୟଥା
	    {
		char_u	*t = (opt == STL_FULLPATH) ? wp->w_buffer->b_ffname
					  : wp->w_buffer->b_fname;
		home_replace(wp->w_buffer, t, NameBuff, MAXPATHL, TRUE);
	    }
	    trans_characters(NameBuff, MAXPATHL);
	    ଯଦି (opt != STL_FILENAME)
		str = NameBuff;
	    ଅନ୍ୟଥା
		str = gettail(NameBuff);
	    ଭାଙ୍ଗ;
	}

	ସ୍ଥିତି STL_VIM_ପଦସମଷ୍ଟି: // '{'
	{
# ifdef FEAT_EVAL
	    char_u  *block_start = s - 1;
# endif
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    reevaluate = (*s == '%');
	    char_u  *t;
	    buf_T   *save_curbuf;
	    win_T   *save_curwin;

	    ଯଦି (reevaluate)
		s++;
	    // %0{} keeps the result verbatim
	    itemisflag = zeropad ? FALSE : TRUE;
	    t = p;
	    ଯେତେବେଳେ ((*s != '}' || (reevaluate && s[-1] != '%'))
					  && *s != NUL && p + 1 < out + outlen)
		*p++ = *s++;
	    ଯଦି (*s != '}')	// missing '}' or out of space
		ଭାଙ୍ଗ;
	    s++;
	    ଯଦି (reevaluate && p > out)
		p[-1] = NUL; // remove the % at the end of %{% expr %}
	    ଅନ୍ୟଥା
		*p = NUL;
	    p = t;
# ifdef FEAT_EVAL
	    vim_snprintf((ଅକ୍ଷର *)buf_tmp, ଆକାର(buf_tmp),
							 "%d", curbuf->b_fnum);
	    set_internal_string_var((char_u *)"g:actual_curbuf", buf_tmp);
	    vim_snprintf((ଅକ୍ଷର *)buf_tmp, ଆକାର(buf_tmp), "%d", curwin->w_id);
	    set_internal_string_var((char_u *)"g:actual_curwin", buf_tmp);

	    save_curbuf = curbuf;
	    save_curwin = curwin;
	    save_VIsual_active = VIsual_active;
	    curwin = wp;
	    curbuf = wp->w_buffer;
	    // Visual mode is only valid in the current window.
	    ଯଦି (curwin != save_curwin)
		VIsual_active = FALSE;

	    str = eval_to_string_safe(p, use_sandbox, FALSE, FALSE);

	    curwin = save_curwin;
	    curbuf = save_curbuf;
	    VIsual_active = save_VIsual_active;
	    do_unlet((char_u *)"g:actual_curbuf", TRUE);
	    do_unlet((char_u *)"g:actual_curwin", TRUE);

	    ଯଦି (!zeropad && str != ଖାଲି && *str != NUL)
	    {
		ଯଦି (*skipdigits(str) == NUL)
		{
		    ଅଙ୍କ = atoi((ଅକ୍ଷର *)str);
		    VIM_CLEAR(str);
		    itemisflag = FALSE;
		}
	    }

	    // If the output of the expression needs to be evaluated
	    // replace the %{} block with the result of evaluation
	    ଯଦି (reevaluate && str != ଖାଲି && *str != NUL
		    && strchr((ଅଚଳ ଅକ୍ଷର *)str, '%') != ଖାଲି
		    && evaldepth < MAX_STL_EVAL_DEPTH)
	    {
		size_t parsed_usefmt = (size_t)(block_start - usefmt);
		size_t str_length = strlen((ଅଚଳ ଅକ୍ଷର *)str);
		size_t fmt_length = strlen((ଅଚଳ ଅକ୍ଷର *)s);
		size_t new_fmt_len = parsed_usefmt
						 + str_length + fmt_length + 3;
		char_u *new_fmt = (char_u *)alloc(new_fmt_len * ଆକାର(char_u));

		ଯଦି (new_fmt != ଖାଲି)
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
		    new_fmt_p = ଖାଲି;

		    ଯଦି (usefmt != fmt)
			vim_free(usefmt);
		    VIM_CLEAR(str);
		    usefmt = new_fmt;
		    s = usefmt + parsed_usefmt;
		    evaldepth++;
		    ଜାରି;
		}
	    }
# endif
	    ଭାଙ୍ଗ;
	}
	ସ୍ଥିତି STL_ଧାଡ଼ି:
	    ଅଙ୍କ = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		  ? 0L : (long)(wp->w_cursor.lnum);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_NUMLINES:
	    ଅଙ୍କ = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_COLUMN:
	    ଅଙ୍କ = (State & MODE_ଭର୍ତ୍ତି) == 0 && empty_ଧାଡ଼ି
					       ? 0 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_cursor.col + 1;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_VIRTCOL:
	ସ୍ଥିତି STL_VIRTCOL_ALT:
	{
	    colnr_T virtcol = wp->w_virtcol + 1;

	    // Don't display %V if it's the same as %c.
	    ଯଦି (opt == STL_VIRTCOL_ALT
		    && (virtcol == (colnr_T)((State & MODE_ଭର୍ତ୍ତି) == 0
			       && empty_ଧାଡ଼ି ? 0 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_cursor.col + 1)))
		ଭାଙ୍ଗ;
	    ଅଙ୍କ = (long)virtcol;
	    ଭାଙ୍ଗ;
	}

	ସ୍ଥିତି STL_PERCENTAGE:
	    ଅଙ୍କ = calc_percentage((long)wp->w_cursor.lnum, (long)wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_ALTPERCENT:
	    str = buf_tmp;
	    (ଶୂନ୍ୟ)get_rel_pos(wp, str, TMPLEN);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_SHOWCMD:
	    ଯଦି (p_sc && STRCMP(opt_name, p_sloc) == 0)
		str = showcmd_buf;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_ARGLISTSTAT:
	    fillable = FALSE;
	    buf_tmp[0] = NUL;
	    ଯଦି (append_arg_ସଂଖ୍ୟା(wp, buf_tmp, ଆକାର(buf_tmp), FALSE) > 0)
		str = buf_tmp;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_KEYMAP:
	    fillable = FALSE;
	    ଯଦି (get_keymap_str(wp, (char_u *)"<%s>", buf_tmp, TMPLEN) > 0)
		str = buf_tmp;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି STL_PAGENUM:
# ଯଦି defined(FEAT_PRINTER) || defined(FEAT_GUI_TABLINE)
	    ଅଙ୍କ = printer_page_ଅଙ୍କ;
# ଅନ୍ୟଥା
	    ଅଙ୍କ = 0;
# endif
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_BUFNO:
	    ଅଙ୍କ = wp->w_buffer->b_fnum;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_OFFSET_X:
	    base = 'X';
	    // FALLTHROUGH
	ସ୍ଥିତି STL_OFFSET:
# ifdef FEAT_BYTEOFF
	    l = ml_find_ଧାଡ଼ି_or_offset(wp->w_buffer, wp->w_cursor.lnum, ଖାଲି);
	    ଅଙ୍କ = (wp->w_buffer->b_ml.ml_flags & ML_EMPTY) || l < 0
		       ? 0L : l + 1 + ((State & MODE_ଭର୍ତ୍ତି) == 0 && empty_ଧାଡ଼ି
				? 0 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wp->w_cursor.col);
# endif
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_BYTEVAL_X:
	    base = 'X';
	    // FALLTHROUGH
	ସ୍ଥିତି STL_BYTEVAL:
	    ଅଙ୍କ = byteval;
	    ଯଦି (ଅଙ୍କ == NL)
		ଅଙ୍କ = 0;
	    ଅନ୍ୟଥା ଯଦି (ଅଙ୍କ == CAR && get_fileformat(wp->w_buffer) == EOL_MAC)
		ଅଙ୍କ = NL;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_ROFLAG:
	ସ୍ଥିତି STL_ROFLAG_ALT:
	    itemisflag = TRUE;
	    ଯଦି (wp->w_buffer->b_p_ro)
		str = (char_u *)((opt == STL_ROFLAG_ALT) ? ",RO" : _("[RO]"));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_HELPFLAG:
	ସ୍ଥିତି STL_HELPFLAG_ALT:
	    itemisflag = TRUE;
	    ଯଦି (wp->w_buffer->b_help)
		str = (char_u *)((opt == STL_HELPFLAG_ALT) ? ",HLP"
							       : _("[Help]"));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_FILETYPE:
	    ଯଦି (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 3)
	    {
		vim_snprintf((ଅକ୍ଷର *)buf_tmp, ଆକାର(buf_tmp), "[%s]",
							wp->w_buffer->b_p_ft);
		str = buf_tmp;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_FILETYPE_ALT:
	    itemisflag = TRUE;
	    ଯଦି (*wp->w_buffer->b_p_ft != NUL
		    && STRLEN(wp->w_buffer->b_p_ft) < TMPLEN - 2)
	    {
		char_u	*t;

		vim_snprintf((ଅକ୍ଷର *)buf_tmp, ଆକାର(buf_tmp), ",%s",
							wp->w_buffer->b_p_ft);
		ପାଇଁ (t = buf_tmp; *t != 0; t++)
		    *t = TOUPPER_LOC(*t);
		str = buf_tmp;
	    }
	    ଭାଙ୍ଗ;

# ଯଦି defined(FEAT_QUICKFIX)
	ସ୍ଥିତି STL_PREVIEWFLAG:
	ସ୍ଥିତି STL_PREVIEWFLAG_ALT:
	    itemisflag = TRUE;
	    ଯଦି (wp->w_p_pvw)
		str = (char_u *)((opt == STL_PREVIEWFLAG_ALT) ? ",PRV"
							    : _("[Preview]"));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_QUICKFIX:
	    ଯଦି (bt_quickfix(wp->w_buffer))
		str = (char_u *)(wp->w_llist_ref
			    ? _(ସନ୍ଦେଶ_loclist)
			    : _(ସନ୍ଦେଶ_qflist));
	    ଭାଙ୍ଗ;
# endif

	ସ୍ଥିତି STL_MODIFIED:
	ସ୍ଥିତି STL_MODIFIED_ALT:
	    itemisflag = TRUE;
	    ବାଛ ((opt == STL_MODIFIED_ALT)
		    + bufIsChanged(wp->w_buffer) * 2
		    + (!wp->w_buffer->b_p_ma) * 4)
	    {
		ସ୍ଥିତି 2: str = (char_u *)"[+]"; ଭାଙ୍ଗ;
		ସ୍ଥିତି 3: str = (char_u *)",+"; ଭାଙ୍ଗ;
		ସ୍ଥିତି 4: str = (char_u *)"[-]"; ଭାଙ୍ଗ;
		ସ୍ଥିତି 5: str = (char_u *)",-"; ଭାଙ୍ଗ;
		ସ୍ଥିତି 6: str = (char_u *)"[+-]"; ଭାଙ୍ଗ;
		ସ୍ଥିତି 7: str = (char_u *)",+-"; ଭାଙ୍ଗ;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି STL_HIGHLIGHT:
	    {
		char_u  *t = s;

		ଯେତେବେଳେ (*s != '#' && *s != NUL)
		    ++s;
		ଯଦି (*s == '#')
		{
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = Highlight;
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_start = p;
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_minwid = -syn_namen2id(t, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(s - t));
		    curitem++;
		}
		ଯଦି (*s != NUL)
		    ++s;
		ଜାରି;
	    }

	ସ୍ଥିତି STL_CLICKFUNC:
	    // %N[] - end click region (with minwid, minwid is ignored)
	    ଯଦି (*s == ']')
	    {
		stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = ClickFunc;
		stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_start = p;
		stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_minwid = 0;
		stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_clickfunc = ଖାଲି;
		s++;
		curitem++;
		ଜାରି;
	    }
	    // %N[FuncName] with minwid
	    ଯଦି (ASCII_ISALPHA(*s) || *s == '_')
	    {
		char_u *rb = vim_strchr(s, ']');
		ଯଦି (rb != ଖାଲି)
		{
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = ClickFunc;
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_start = p;
		    // The stl_minwid field is overloaded: it may be the
		    // "min" part of %<min>.<max> used for padding, or an
		    // identifier passed to the %N[FuncName] callback.  Store
		    // the uncapped value so IDs above 50 are preserved.
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_minwid = raw_minwid;
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_clickfunc =
					      vim_strnsave(s, rb - s);
		    s = rb + 1;
		    curitem++;
		    ଜାରି;
		}
	    }
	    ଜାରି;

	ସ୍ଥିତି STL_LINEBREAK:
	    // %N@ - line break (already handled above, fallback)
	    ଜାରି;
	}

	stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_start = p;
	stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = Normal;
	ଯଦି (str != ଖାଲି && *str)
	{
	    char_u  *t = str;

	    ଯଦି (itemisflag)
	    {
		ଯଦି ((t[0] && t[1])
			&& ((!prevchar_isitem && *t == ',')
			      || (prevchar_isflag && *t == ' ')))
		    t++;
		prevchar_isflag = TRUE;
	    }
	    l = vim_strsize(t);
	    ଯଦି (l > 0)
		prevchar_isitem = TRUE;
	    ଯଦି (l > maxwid)
	    {
		ଯେତେବେଳେ (l >= maxwid)
		    ଯଦି (has_mbyte)
		    {
			l -= ptr2cells(t);
			t += (*mb_ptr2len)(t);
		    }
		    ଅନ୍ୟଥା
			l -= byte2cells(*t++);
		ଯଦି (p + 1 >= out + outlen)
		    ଭାଙ୍ଗ;
		*p++ = '<';
	    }
	    ଯଦି (minwid > 0)
	    {
		ପାଇଁ (; l < minwid && p + 1 < out + outlen; l++)
		{
		    // Don't put a "-" in front of a digit.
		    ଯଦି (l + 1 == minwid && fillchar == '-' && VIM_ISDIGIT(*t))
			*p++ = ' ';
		    ଅନ୍ୟଥା
			MB_CHAR2BYTES(fillchar, p);
		}
		minwid = 0;
	    }
	    ଅନ୍ୟଥା
		minwid *= -1;
	    ପାଇଁ (; *t && p + 1 < out + outlen; t++)
	    {
		// Change a space by fillchar, unless fillchar is '-' and a
		// digit follows.
		ଯଦି (fillable && *t == ' '
				&& (!VIM_ISDIGIT(*(t + 1)) || fillchar != '-'))
		    MB_CHAR2BYTES(fillchar, p);
		ଅନ୍ୟଥା
		    *p++ = *t;
	    }
	    ପାଇଁ (; l < minwid && p + 1 < out + outlen; l++)
		MB_CHAR2BYTES(fillchar, p);
	}
	ଅନ୍ୟଥା ଯଦି (ଅଙ୍କ >= 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nbase = (base == 'D' ? 10 : (base == 'O' ? 8 : 16));
	    char_u  nstr[20];
	    char_u  *t = nstr;

	    ଯଦି (p + 20 >= out + outlen)
		ଭାଙ୍ଗ;		// not sufficient space
	    prevchar_isitem = TRUE;
	    ଯଦି (opt == STL_VIRTCOL_ALT)
	    {
		*t++ = '-';
		minwid--;
	    }
	    *t++ = '%';
	    ଯଦି (zeropad)
		*t++ = '0';
	    *t++ = '*';
	    *t++ = nbase == 16 ? base : (char_u)(nbase == 8 ? 'o' : 'd');
	    *t = NUL;

	    ପାଇଁ (n = ଅଙ୍କ, l = 1; n >= nbase; n /= nbase)
		l++;
	    ଯଦି (opt == STL_VIRTCOL_ALT)
		l++;

	    ଯଦି (l > maxwid)
	    {
		l += 2;
		n = l - maxwid;
		ଯେତେବେଳେ (l-- > maxwid)
		    ଅଙ୍କ /= nbase;
		*t++ = '>';
		*t++ = '%';
		*t = t[-3];
		*++t = NUL;
		p += vim_snprintf_safelen((ଅକ୍ଷର *)p, outlen - (p - out),
		    (ଅକ୍ଷର *)nstr, 0, ଅଙ୍କ, n);
	    }
	    ଅନ୍ୟଥା
		p += vim_snprintf_safelen((ଅକ୍ଷର *)p, outlen - (p - out),
		    (ଅକ୍ଷର *)nstr, minwid, ଅଙ୍କ);
	}
	ଅନ୍ୟଥା
	    stl_ବସ୍ତୁଗୁଡ଼ିକ[curitem].stl_type = Empty;

	ଯଦି (ଅଙ୍କ >= 0 || (!itemisflag && str != ଖାଲି && *str != NUL))
	    prevchar_isflag = FALSE;	    // Item not NULL, but not a flag
					    //
	ଯଦି (opt == STL_VIM_ପଦସମଷ୍ଟି)
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
    ଯଦି (carry_hl != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_hl = 0;
	ପାଇଁ (l = 0; l < itemcnt; l++)
	    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_type == Highlight)
		last_hl = stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_minwid;
	*carry_hl = last_hl;
    }

    ଯଦି (mode == STL_MODE_MULTI
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
	ଯଦି (usefmt != fmt)
	    vim_free(fmt);
# endif
	size_t fmt_remain_len = strlen((ଅକ୍ଷର *)s);

	mch_memmove(usefmt, s, fmt_remain_len);
	usefmt[fmt_remain_len] = NUL;
	*fmt_arg = usefmt;
    }
    ଅନ୍ୟଥା
    {
	// In single-line mode, the caller retains ownership of its buffer.
	// Free the eval result if "%!" was used.
# ifdef FEAT_EVAL
	ଯଦି (usefmt != fmt)
	    vim_free(usefmt);
# endif
    }

    ଯଦି (mode == STL_MODE_GET_RENDERED_HEIGHT)
    {
	// Free click function names that were allocated during parsing.
	ପାଇଁ (l = 0; l < itemcnt; l++)
	    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_type == ClickFunc)
		vim_free(stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_clickfunc);
	ଯଦି (rendered_height != ଖାଲି)
	    *rendered_height = rheight;
	ଫେରନ୍ତୁ 0;
    }

    width = vim_strsize(out);
    ଯଦି (maxwidth > 0 && width > maxwidth)
    {
	// Result is too long, must truncate somewhere.
	l = 0;
	ଯଦି (itemcnt == 0)
	    s = out;
	ଅନ୍ୟଥା
	{
	    ପାଇଁ ( ; l < itemcnt; l++)
		ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_type == Trunc)
		{
		    // Truncate at %< item.
		    s = stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start;
		    ଭାଙ୍ଗ;
		}
	    ଯଦି (l == itemcnt)
	    {
		// No %< item, truncate first item.
		s = stl_ବସ୍ତୁଗୁଡ଼ିକ[0].stl_start;
		l = 0;
	    }
	}

	ଯଦି (width - vim_strsize(s) >= maxwidth)
	{
	    // Truncation mark is beyond max length
	    ଯଦି (has_mbyte)
	    {
		s = out;
		width = 0;
		ପାଇଁ (;;)
		{
		    width += ptr2cells(s);
		    ଯଦି (width >= maxwidth)
			ଭାଙ୍ଗ;
		    s += (*mb_ptr2len)(s);
		}
		// Fill up for half a double-wide character.
		ଯେତେବେଳେ (++width < maxwidth)
		    MB_CHAR2BYTES(fillchar, s);
	    }
	    ଅନ୍ୟଥା
		s = out + maxwidth - 1;
	    ପାଇଁ (l = 0; l < itemcnt; l++)
		ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start > s)
		    ଭାଙ୍ଗ;
	    itemcnt = l;
	    *s++ = '>';
	    *s = NUL;
	}
	ଅନ୍ୟଥା
	{
	    char_u  *end = out + outputlen;

	    n = 0;
	    ଯେତେବେଳେ (width >= maxwidth)
	    {
		width -= ptr2cells(s + n);
		n += (*mb_ptr2len)(s + n);
	    }
	    p = s + n;
	    mch_memmove(s + 1, p, (size_t)(end - p) + 1);	// +1 for NUL
	    end -= (size_t)(p - (s + 1));
	    *s = '<';

	    --n;	// count the '<'
	    ପାଇଁ (; l < itemcnt; l++)
	    {
		ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start - n >= s)
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start -= n;
		ଅନ୍ୟଥା
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start = s;
	    }

	    // Fill up for half a double-wide character.
	    ଯେତେବେଳେ (++width < maxwidth)
	    {
		s = end;
		MB_CHAR2BYTES(fillchar, s);
		*s = NUL;
		end = s;
	    }
	}
	width = maxwidth;
    }
    ଅନ୍ୟଥା ଯଦି (width < maxwidth &&
	    outputlen + (maxwidth - width) * MB_CHAR2LEN(fillchar) + 1 < outlen)
    {
	// Find how many separators there are, which we will use when
	// figuring out how many groups there are.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_separators = 0;

	ପାଇଁ (l = 0; l < itemcnt; l++)
	{
	    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_type == Separate)
	    {
		// Create an array of the start location for each separator
		// mark.
		stl_separator_locations[ଅଙ୍କ_separators] = l;
		ଅଙ୍କ_separators++;
	    }
	}

	// If we have separated groups, then we deal with it now
	ଯଦି (ଅଙ୍କ_separators)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା standard_spaces;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା final_spaces;

	    standard_spaces = (maxwidth - width) / ଅଙ୍କ_separators;
	    final_spaces = (maxwidth - width) -
					standard_spaces * (ଅଙ୍କ_separators - 1);
	    ପାଇଁ (l = 0; l < ଅଙ୍କ_separators; l++)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା dislocation = (l == (ଅଙ୍କ_separators - 1)) ?
					final_spaces : standard_spaces;
		dislocation *= MB_CHAR2LEN(fillchar);
		char_u *start = stl_ବସ୍ତୁଗୁଡ଼ିକ[stl_separator_locations[l]].stl_start;
		char_u *seploc = start + dislocation;
		STRMOVE(seploc, start);
		ପାଇଁ (s = start; s < seploc;)
		    MB_CHAR2BYTES(fillchar, s);

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = stl_separator_locations[l] + 1; i < itemcnt; i++)
		    stl_ବସ୍ତୁଗୁଡ଼ିକ[i].stl_start += dislocation;
	    }

	    width = maxwidth;
	}
    }

    // Store the info about highlighting.
    ଯଦି (hltab != ଖାଲି)
    {
	*hltab = stl_hltab;
	sp = stl_hltab;
	ପାଇଁ (l = 0; l < itemcnt; l++)
	{
	    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_type == Highlight)
	    {
		sp->start = stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start;
		sp->userhl = stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = ଖାଲି;
	sp->userhl = 0;
    }

    // Store the info about tab pages labels.
    ଯଦି (tabtab != ଖାଲି)
    {
	*tabtab = stl_tabtab;
	sp = stl_tabtab;
	ପାଇଁ (l = 0; l < itemcnt; l++)
	{
	    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_type == TabPage)
	    {
		sp->start = stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start;
		sp->userhl = stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_minwid;
		sp++;
	    }
	}
	sp->start = ଖାଲି;
	sp->userhl = 0;
    }

    // Store the info about click function regions.
    ଯଦି (clicktab != ଖାଲି)
    {
	stl_clickrec_T *cp;

	*clicktab = stl_clicktab;
	cp = stl_clicktab;
	ପାଇଁ (l = 0; l < itemcnt; l++)
	{
	    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_type == ClickFunc)
	    {
		cp->start = stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_start;
		cp->funcname = stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_clickfunc;
		cp->minwid = stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_minwid;
		cp++;
	    }
	}
	cp->start = ଖାଲି;
	cp->funcname = ଖାଲି;
    }
    ଅନ୍ୟଥା
    {
	// Free click function names when caller doesn't need them.
	ପାଇଁ (l = 0; l < itemcnt; l++)
	    ଯଦି (stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_type == ClickFunc)
		vim_free(stl_ବସ୍ତୁଗୁଡ଼ିକ[l].stl_clickfunc);
    }

    redraw_not_allowed = save_redraw_not_allowed;

    // A user function may reset KeyTyped, restore it.
    ଚାବି_Typed = save_ଚାବି_Typed;

    // Check for an error.  If there is one the display will be messed up and
    // might loop redrawing.  Avoid that by making the corresponding option
    // empty.
    // TODO: find out why using called_emsg_before makes tests fail, does it
    // matter?
    // if (called_emsg > called_emsg_before)
    ଯଦି (did_emsg > did_emsg_before)
	set_string_option_direct(opt_name, -1, (char_u *)"",
					      OPT_FREE | opt_scope, SID_ERROR);

    ଫେରନ୍ତୁ width;
}
#endif // FEAT_STL_OPT

/*
 * Get relative cursor position in window into "buf[]", in the localized
 * percentage form like %99, 99%; using "Top", "Bot" or "All" when appropriate.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_rel_pos(
    win_T	*wp,
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflen)
{
    long	above; // number of lines above window
    long	below; // number of lines below window

    ଯଦି (buflen < 3) // need at least 3 chars for writing
	ଫେରନ୍ତୁ 0;
    above = wp->w_topline - 1;
#ifdef FEAT_DIFF
    above += diff_check_fill(wp, wp->w_topline) - wp->w_topfill;
    ଯଦି (wp->w_topline == 1 && wp->w_topfill >= 1)
	above = 0;  // All buffer lines are displayed and there is an
		    // indication of filler lines, that can be considered
		    // seeing all lines.
#endif
    below = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା - wp->w_botline + 1;
    ଯଦି (below <= 0)
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vim_snprintf_safelen((ଅକ୍ଷର *)buf, buflen,
	    "%s", (above == 0) ? _("All") : _("Bot"));

    ଯଦି (above <= 0)
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vim_snprintf_safelen((ଅକ୍ଷର *)buf, buflen,
	    "%s", _("Top"));

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା perc = calc_percentage(above, above + below);
    ଅକ୍ଷର tmp[8];
    // localized percentage value
    vim_snprintf(tmp, ଆକାର(tmp), _("%d%%"), perc);
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vim_snprintf_safelen((ଅକ୍ଷର *)buf, buflen, _("%3s"), tmp);
}

/*
 * Append (file 2 of 8) to "buf[]", if editing more than one file.
 * Return the number of characters appended.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
append_arg_ସଂଖ୍ୟା(
    win_T	*wp,
    char_u	*buf,
    size_t	buflen,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		add_file)	// Add "file" before the arg number
{
    ଯଦି (ARGCOUNT <= 1)		// nothing to do
	ଫେରନ୍ତୁ 0;

    ଅକ୍ଷର *ସନ୍ଦେଶ;
    ବାଛ ((wp->w_arg_idx_invalid ? 1 : 0) + (add_file ? 2 : 0))
    {
	ସ୍ଥିତି 0: ସନ୍ଦେଶ = _(" (%d of %d)"); ଭାଙ୍ଗ;
	ସ୍ଥିତି 1: ସନ୍ଦେଶ = _(" ((%d) of %d)"); ଭାଙ୍ଗ;
	ସ୍ଥିତି 2: ସନ୍ଦେଶ = _(" (file %d of %d)"); ଭାଙ୍ଗ;
	ସ୍ଥିତି 3: ସନ୍ଦେଶ = _(" (file (%d) of %d)"); ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vim_snprintf_safelen((ଅକ୍ଷର *)buf, buflen, ସନ୍ଦେଶ,
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
    ଫେରନ୍ତୁ FullName_save(fname, TRUE);
#ଅନ୍ୟଥା
    ଯଦି (!vim_isAbsName(fname)
	    || strstr((ଅକ୍ଷର *)fname, "..") != ଖାଲି
	    || strstr((ଅକ୍ଷର *)fname, "//") != ଖାଲି
# ifdef BACKSLASH_IN_FILENAME
	    || strstr((ଅକ୍ଷର *)fname, "\\\\") != ଖାଲି
# endif
# ଯଦି defined(MSWIN)
	    || vim_strchr(fname, '~') != ଖାଲି
	    || fname[0] == '/'
	    || fname[0] == '\\'
# endif
	    )
	ଫେରନ୍ତୁ FullName_save(fname, FALSE);

    fname = vim_strsave(fname);

# ifdef USE_FNAME_CASE
    ଯଦି (fname != ଖାଲି)
	fname_case(fname, 0);	// set correct case for file name
# endif

    ଫେରନ୍ତୁ fname;
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
    ଶୂନ୍ୟ
fname_expand(
    buf_T	*buf UNUSED,
    char_u	**ffname,
    char_u	**sfname)
{
    ଯଦି (*ffname == ଖାଲି)	    // no file name given, nothing to do
	ଫେରନ୍ତୁ;
    ଯଦି (*sfname == ଖାଲି)	    // no short file name given, use ffname
	*sfname = *ffname;
    *ffname = fix_fname(*ffname);   // expand to full path

#ifdef FEAT_SHORTCUT
    ଯଦି (!buf->b_p_bin)
    {
	char_u  *rfname;

	// If the file name is a shortcut file, use the file it links to.
	rfname = mch_resolve_path(*ffname, FALSE);
	ଯଦି (rfname != ଖାଲି)
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
    ଶୂନ୍ୟ
ex_buffer_all(exarg_T *eap)
{
    buf_T	*buf;
    win_T	*wp, *wpnext;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		split_ret = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		p_ea_save;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		open_wins = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;		// Maximum number of windows to open.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		all;		// When TRUE also load inactive buffers.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_tab = cmdmod.cmod_tab;
    tabpage_T	*tpnext;

    ଯଦି (eap->addr_ଗଣନା == 0)	// make as many windows as possible
	ଗଣନା_2 = 9999;
    ଅନ୍ୟଥା
	ଗଣନା_2 = eap->line2;	// make as many windows as specified
    ଯଦି (eap->cmdidx == CMD_unhide || eap->cmdidx == CMD_sunhide)
	all = FALSE;
    ଅନ୍ୟଥା
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
    ଯଦି (had_tab > 0)
	goto_tabpage_tp(first_tabpage, TRUE, TRUE);
    ପାଇଁ (;;)
    {
	tpnext = curtab->tp_next;
	ପାଇଁ (wp = firstwin; wp != ଖାଲି; wp = wpnext)
	{
	    wpnext = wp->w_next;
	    ଯଦି ((wp->w_buffer->b_nwindows > 1
			|| ((cmdmod.cmod_split & WSP_VERT)
			    ? wp->w_height + wp->w_status_height < Rows - p_ch
							     - tabline_height()
			    : wp->w_width != Columns)
			|| (had_tab > 0 && wp != firstwin))
		    && !ONE_WINDOW
		    && !(win_locked(wp) || wp->w_buffer->b_locked > 0)
		    && !win_unlisted(wp))
	    {
		ଯଦି (win_close(wp, FALSE) == FAIL)
		    ଭାଙ୍ଗ;
		// Just in case an autocommand does something strange with
		// windows: start all over...
		wpnext = firstwin;
		tpnext = first_tabpage;
		open_wins = 0;
	    }
	    ଅନ୍ୟଥା
		++open_wins;
	}

	// Without the ":tab" modifier only do the current tab page.
	ଯଦି (had_tab == 0 || tpnext == ଖାଲି)
	    ଭାଙ୍ଗ;
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
    ପାଇଁ (buf = firstbuf; buf != ଖାଲି && open_wins < ଗଣନା_2; buf = buf->b_next)
    {
	// Check if this buffer needs a window
	ଯଦି ((!all && buf->b_ml.ml_mfp == ଖାଲି) || !buf->b_p_bl)
	    ଜାରି;

	ଯଦି (had_tab != 0)
	{
	    // With the ":tab" modifier don't move the window.
	    ଯଦି (buf->b_nwindows > 0)
		wp = lastwin;	    // buffer has a window, skip it
	    ଅନ୍ୟଥା
		wp = ଖାଲି;
	}
	ଅନ୍ୟଥା
	{
	    // Check if this buffer already has a window
	    FOR_ALL_WINDOWS(wp)
		ଯଦି (wp->w_buffer == buf)
		    ଭାଙ୍ଗ;
	    // If the buffer already has a window, move it
	    ଯଦି (wp != ଖାଲି)
		win_move_after(wp, curwin);
	}

	ଯଦି (wp == ଖାଲି && split_ret == OK)
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    // Split the window and put the buffer in it
	    p_ea_save = p_ea;
	    p_ea = TRUE;		// use space from all windows
	    split_ret = win_split(0, WSP_ROOM | WSP_BELOW);
	    ++open_wins;
	    p_ea = p_ea_save;
	    ଯଦି (split_ret == FAIL)
		ଜାରି;

	    // Open the buffer in this window.
	    swap_exists_action = SEA_DIALOG;
	    set_curbuf(buf, DOBUF_GOTO);
	    ଯଦି (!bufref_valid(&bufref))
	    {
		// autocommands deleted the buffer!!!
		swap_exists_action = SEA_NONE;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (swap_exists_action == SEA_QUIT)
	    {
#ଯଦି defined(FEAT_EVAL)
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

#ଯଦି defined(FEAT_EVAL)
		// Restore the error/interrupt/exception state if not
		// discarded by a new aborting error, interrupt, or uncaught
		// exception.
		leave_cleanup(&cs);
#endif
	    }
	    ଅନ୍ୟଥା
		handle_swap_exists(ଖାଲି);
	}

	ui_breakcheck();
	ଯଦି (got_int)
	{
	    (ଶୂନ୍ୟ)vgetc();	// only break the file loading, not the rest
	    ଭାଙ୍ଗ;
	}
#ifdef FEAT_EVAL
	// Autocommands deleted the buffer or aborted script processing!!!
	ଯଦି (aborting())
	    ଭାଙ୍ଗ;
#endif
	// When ":tab" was used open a new tab for a new window repeatedly.
	ଯଦି (had_tab > 0 && tabpage_index(ଖାଲି) <= p_tpm)
	    cmdmod.cmod_tab = 9999;
    }
    --autocmd_no_enter;
    win_enter(firstwin, FALSE);		// back to first window
    --autocmd_no_leave;

    /*
     * Close superfluous windows.
     */
    ପାଇଁ (wp = lastwin; open_wins > ଗଣନା_2; )
    {
	r = (buf_hide(wp->w_buffer) || !bufIsChanged(wp->w_buffer)
				     || autowrite(wp->w_buffer, FALSE) == OK);
	ଯଦି (!win_valid(wp))
	{
	    // BufWrite Autocommands made the window invalid, start over
	    wp = lastwin;
	}
	ଅନ୍ୟଥା ଯଦି (r)
	{
	    win_close(wp, !buf_hide(wp->w_buffer));
	    --open_wins;
	    wp = lastwin;
	}
	ଅନ୍ୟଥା
	{
	    wp = wp->w_prev;
	    ଯଦି (wp == ଖାଲି)
		ଭାଙ୍ଗ;
	}
    }
}


ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  chk_modeline(linenr_T, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

/*
 * do_modelines() - process mode lines for the current file
 *
 * "flags" can be:
 * OPT_WINONLY	    only set options local to window
 * OPT_NOWIN	    don't set options local to window
 *
 * Returns immediately if the "ml" option isn't set.
 */
    ଶୂନ୍ୟ
do_modelines(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nmlines;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	entered = 0;

    ଯଦି (!curbuf->b_p_ml || (nmlines = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_mls) == 0)
	ଫେରନ୍ତୁ;

    // Disallow recursive entry here.  Can happen when executing a modeline
    // triggers an autocommand, which reloads modelines with a ":do".
    ଯଦି (entered)
	ଫେରନ୍ତୁ;

    ++entered;
    ପାଇଁ (lnum = 1; curbuf->b_p_ml && lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା && lnum <= nmlines;
								       ++lnum)
	ଯଦି (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;

    ପାଇଁ (lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା; curbuf->b_p_ml && lnum > 0 && lnum > nmlines
		       && lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - nmlines; --lnum)
	ଯଦି (chk_modeline(lnum, flags) == FAIL)
	    nmlines = 0;
    --entered;
}

#ଅନ୍ତର୍ଭାଵ "version.h"		// for version number

/*
 * chk_modeline() - check a single line for a mode string
 * Return FAIL if an error encountered.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
chk_modeline(
    linenr_T	lnum,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		// Same as for do_modelines().
{
    char_u	*s;
    char_u	*ଧାଡ଼ି_end;		// point to the end of the line
    char_u	*e;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
    ESTACK_CHECK_DECLARATION;

    prev = -1;
    s = ml_get(lnum);
    ଧାଡ଼ି_end = s + ml_get_len(lnum);
    ପାଇଁ (; *s != NUL; ++s)
    {
	ଯଦି (prev == -1 || vim_isspace(prev))
	{
	    ଯଦି ((prev != -1 && STRNCMP(s, "ex:", (size_t)3) == 0)
		    || STRNCMP(s, "vi:", (size_t)3) == 0)
		ଭାଙ୍ଗ;
	    // Accept both "vim" and "Vim".
	    ଯଦି ((s[0] == 'v' || s[0] == 'V') && s[1] == 'i' && s[2] == 'm')
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା vers;

		ଯଦି (s[3] == '<' || s[3] == '=' || s[3] == '>')
		    e = s + 4;
		ଅନ୍ୟଥା
		    e = s + 3;
		vers = getdigits(&e);
		ଯଦି (*e == ':'
			&& (s[0] != 'V'
				  || STRNCMP(skipwhite(e + 1), "set", 3) == 0)
			&& (s[3] == ':'
			    || (VIM_VERSION_100 >= vers && SAFE_isdigit(s[3]))
			    || (VIM_VERSION_100 < vers && s[3] == '<')
			    || (VIM_VERSION_100 > vers && s[3] == '>')
			    || (VIM_VERSION_100 == vers && s[3] == '=')))
		    ଭାଙ୍ଗ;
	    }
	}
	prev = *s;
    }

    ଯଦି (*s)
    {
	size_t	len;
	char_u	*linecopy;		// local copy of any modeline found
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	end;

	କର				// skip over "ex:", "vi:" or "vim:"
	    ++s;
	ଯେତେବେଳେ (s[-1] != ':');

	len = (size_t)(ଧାଡ଼ି_end - s);		// remember the line length
						// so we can restore 'line_end'
						// after the copy
	s = linecopy = vim_strnsave(s, len);	// copy the line, it will change
	ଯଦି (linecopy == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ଧାଡ଼ି_end = s + len;			// restore 'line_end'

	// prepare for emsg()
	estack_ଠେଲ(ETYPE_MODELINE, (char_u *)"modelines", lnum);
	ESTACK_CHECK_SETUP;

	end = FALSE;
	ଯେତେବେଳେ (end == FALSE)
	{
	    s = skipwhite(s);
	    ଯଦି (*s == NUL)
		ଭାଙ୍ଗ;

	    /*
	     * Find end of set command: ':' or end of line.
	     * Skip over "\:", replacing it with ":".
	     */
	    ପାଇଁ (e = s; *e != ':' && *e != NUL; ++e)
		ଯଦି (e[0] == '\\' && e[1] == ':')
		{
		    mch_memmove(e, e + 1, (size_t)(ଧାଡ଼ି_end - (e + 1)) + 1);	// +1 for NUL
		    --ଧାଡ଼ି_end;
		}
	    ଯଦି (*e == NUL)
		end = TRUE;

	    /*
	     * If there is a "set" command, require a terminating ':' and
	     * ignore the stuff after the ':'.
	     * "vi:set opt opt opt: foo" -- foo not interpreted
	     * "vi:opt opt opt: foo" -- foo interpreted
	     * Accept "se" for compatibility with Elvis.
	     */
	    ଯଦି (STRNCMP(s, "set ", (size_t)4) == 0
		    || STRNCMP(s, "se ", (size_t)3) == 0)
	    {
		ଯଦି (*e != ':')		// no terminating ':'?
		    ଭାଙ୍ଗ;
		end = TRUE;
		s += (*(s + 2) == ' ') ? 3 : 4;
	    }
	    *e = NUL;			// truncate the set command

	    ଯଦି (*s != NUL)		// skip over an empty "::"
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା secure_save = secure;
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
		ଯଦି (retval == FAIL)		// stop if error found
		    ଭାଙ୍ଗ;
	    }
	    s = (e == ଧାଡ଼ି_end) ? e : e + 1;	// advance to next part
						// careful not to go off the end
	}

	ESTACK_CHECK_NOW;
	estack_କାଢ଼();
	vim_free(linecopy);
    }
    ଫେରନ୍ତୁ retval;
}

/*
 * Return TRUE if "buf" is a normal buffer, 'buftype' is empty.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_normal(buf_T *buf)
{
    ଫେରନ୍ତୁ buf != ଖାଲି && buf->b_p_bt[0] == NUL;
}

/*
 * Return TRUE if "buf" is the quickfix buffer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_quickfix(buf_T *buf UNUSED)
{
#ifdef FEAT_QUICKFIX
    ଫେରନ୍ତୁ buf != ଖାଲି && buf->b_p_bt[0] == 'q';
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a terminal buffer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_terminal(buf_T *buf UNUSED)
{
#ଯଦି defined(FEAT_TERMINAL)
    ଫେରନ୍ତୁ buf != ଖାଲି && buf->b_p_bt[0] == 't';
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FALSE;
#endif
}

/*
 * Return TRUE if "buf" is a help buffer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_help(buf_T *buf)
{
    ଫେରନ୍ତୁ buf != ଖାଲି && buf->b_help;
}

/*
 * Return TRUE if "buf" is a prompt buffer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_prompt(buf_T *buf)
{
    ଫେରନ୍ତୁ buf != ଖାଲି && buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'r';
}

#ଯଦି defined(FEAT_PROP_POPUP)
/*
 * Return TRUE if "buf" is a buffer for a popup window.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_popup(buf_T *buf)
{
    ଫେରନ୍ତୁ buf != ଖାଲି && buf->b_p_bt != ଖାଲି
	&& buf->b_p_bt[0] == 'p' && buf->b_p_bt[1] == 'o';
}
#endif

/*
 * Return TRUE if "buf" is a "nofile", "acwrite", "terminal" or "prompt"
 * buffer.  This means the buffer name may not be a file name, at least not for
 * writing the buffer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_nofilename(buf_T *buf)
{
    ଫେରନ୍ତୁ buf != ଖାଲି && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 'a'
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'p');
}

/*
 * Return TRUE if "buf" is a "nofile", "quickfix", "terminal" or "prompt"
 * buffer.  This means the buffer is not to be read from a file.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_nofileread(buf_T *buf)
{
    ଫେରନ୍ତୁ buf != ଖାଲି && ((buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f')
	    || buf->b_p_bt[0] == 't'
	    || buf->b_p_bt[0] == 'q'
	    || buf->b_p_bt[0] == 'p');
}

#ଯଦି defined(FEAT_QUICKFIX)
/*
 * Return TRUE if "buf" has 'buftype' set to "nofile".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_nofile(buf_T *buf)
{
    ଫେରନ୍ତୁ buf != ଖାଲି && buf->b_p_bt[0] == 'n' && buf->b_p_bt[2] == 'f';
}
#endif

/*
 * Return TRUE if "buf" is a "nowrite", "nofile", "terminal", "prompt", or
 * "popup" buffer.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_dontwrite(buf_T *buf)
{
    ଫେରନ୍ତୁ buf != ଖାଲି && (buf->b_p_bt[0] == 'n'
		 || buf->b_p_bt[0] == 't'
		 || buf->b_p_bt[0] == 'p');
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bt_dontwrite_ସନ୍ଦେଶ(buf_T *buf)
{
    ଯଦି (bt_dontwrite(buf))
    {
	emsg(_(e_cannot_write_buftype_option_is_set));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if the buffer should be hidden, according to 'hidden', ":hide"
 * and 'bufhidden'.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_hide(buf_T *buf)
{
    // 'bufhidden' overrules 'hidden' and ":hide", check it first
    ବାଛ (buf->b_p_bh[0])
    {
	ସ୍ଥିତି 'u':		    // "unload"
	ସ୍ଥିତି 'w':		    // "wipe"
	ସ୍ଥିତି 'd': ଫେରନ୍ତୁ FALSE;	    // "delete"
	ସ୍ଥିତି 'h': ଫେରନ୍ତୁ TRUE;	    // "hide"
    }
    ଫେରନ୍ତୁ (p_hid || (cmdmod.cmod_flags & CMOD_HIDE));
}

/*
 * Return special buffer name.
 * Returns NULL when the buffer has a normal file name.
 */
    char_u *
buf_spname(buf_T *buf)
{
#ଯଦି defined(FEAT_QUICKFIX)
    ଯଦି (bt_quickfix(buf))
    {
	/*
	 * Differentiate between the quickfix and location list buffers using
	 * the buffer number stored in the global quickfix stack.
	 */
	ଯଦି (buf->b_fnum == qf_ଗଦା_get_bufnr())
	    ଫେରନ୍ତୁ (char_u *)_(ସନ୍ଦେଶ_qflist);
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ (char_u *)_(ସନ୍ଦେଶ_loclist);
    }
#endif

    // There is no _file_ when 'buftype' is "nofile", b_sfname
    // contains the name as specified by the user.
    ଯଦି (bt_nofilename(buf))
    {
#ifdef FEAT_TERMINAL
	ଯଦି (buf->b_term != ଖାଲି)
	    ଫେରନ୍ତୁ term_get_status_text(buf->b_term);
#endif
	ଯଦି (buf->b_fname != ଖାଲି)
	    ଫେରନ୍ତୁ buf->b_fname;
	ଯଦି (buf == cmdwin_buf)
	    ଫେରନ୍ତୁ (char_u *)_("[Command Line]");
#ifdef FEAT_JOB_CHANNEL
	ଯଦି (bt_prompt(buf))
	    ଫେରନ୍ତୁ (char_u *)_("[Prompt]");
#endif
#ifdef FEAT_PROP_POPUP
	ଯଦି (bt_popup(buf))
	    ଫେରନ୍ତୁ (char_u *)_("[Popup]");
#endif
	ଫେରନ୍ତୁ (char_u *)_("[Scratch]");
    }

    ଯଦି (buf->b_fname == ଖାଲି)
	ଫେରନ୍ତୁ buf_get_fname(buf);
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Get "buf->b_fname", use "[No Name]" if it is NULL.
 */
    char_u *
buf_get_fname(buf_T *buf)
{
    ଯଦି (buf->b_fname == ଖାଲି)
	ଫେରନ୍ତୁ (char_u *)_("[No Name]");
    ଫେରନ୍ତୁ buf->b_fname;
}

/*
 * Set 'buflisted' for curbuf to "on" and trigger autocommands if it changed.
 */
    ଶୂନ୍ୟ
set_buflisted(ପୂର୍ଣ୍ଣସଂଖ୍ୟା on)
{
    ଯଦି (on == curbuf->b_p_bl)
	ଫେରନ୍ତୁ;

    curbuf->b_p_bl = on;
    ଯଦି (on)
	apply_autocmds(EVENT_BUFADD, ଖାଲି, ଖାଲି, FALSE, curbuf);
    ଅନ୍ୟଥା
	apply_autocmds(EVENT_BUFDELETE, ଖାଲି, ଖାଲି, FALSE, curbuf);
}

/*
 * Read the file for "buf" again and check if the contents changed.
 * Return TRUE if it changed or this could not be checked.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_contents_changed(buf_T *buf)
{
    buf_T	*newbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		differ = TRUE;
    linenr_T	lnum;
    aco_save_T	aco;
    exarg_T	ea;

    // Allocate a buffer without putting it in the buffer list.
    newbuf = buflist_new(ଖାଲି, ଖାଲି, (linenr_T)1, BLN_DUMMY);
    ଯଦି (newbuf == ଖାଲି)
	ଫେରନ୍ତୁ TRUE;

    // Force the 'fileencoding' and 'fileformat' to be equal.
    ଯଦି (prep_exarg(&ea, buf) == FAIL)
    {
	wipe_buffer(newbuf, FALSE);
	ଫେରନ୍ତୁ TRUE;
    }

    // Set curwin/curbuf to buf and save a few things.
    aucmd_prepbuf(&aco, newbuf);
    ଯଦି (curbuf != newbuf)
    {
	// Failed to find a window for "newbuf".
	wipe_buffer(newbuf, FALSE);
	ଫେରନ୍ତୁ TRUE;
    }

    // We don't want to trigger autocommands now, they may have nasty
    // side-effects like wiping buffers
    block_autocmds();
    ଯଦି (ml_open(curbuf) == OK
	    && readfile(buf->b_ffname, buf->b_fname,
				  (linenr_T)0, (linenr_T)0, (linenr_T)MAXLNUM,
					    &ea, READ_NEW | READ_DUMMY) == OK)
    {
	// compare the two files line by line
	ଯଦି (buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା == curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    differ = FALSE;
	    ପାଇଁ (lnum = 1; lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା; ++lnum)
		ଯଦି (STRCMP(ml_get_buf(buf, lnum, FALSE), ml_get(lnum)) != 0)
		{
		    differ = TRUE;
		    ଭାଙ୍ଗ;
		}
	}
    }
    vim_free(ea.cmd);

    // restore curwin/curbuf and a few other things
    aucmd_restbuf(&aco);

    ଯଦି (curbuf != newbuf)	// safety check
	wipe_buffer(newbuf, FALSE);

    unblock_autocmds();

    ଫେରନ୍ତୁ differ;
}

/*
 * Wipe out a buffer and decrement the last buffer number if it was used for
 * this buffer.  Call this to wipe out a temp buffer that does not contain any
 * marks.
 */
    ଶୂନ୍ୟ
wipe_buffer(
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		aucmd)	    // When TRUE trigger autocommands.
{
    ଯଦି (buf->b_fnum == ଶୀର୍ଷ_file_ଅଙ୍କ - 1)
	--ଶୀର୍ଷ_file_ଅଙ୍କ;

    ଯଦି (!aucmd)		    // Don't trigger BufDelete autocommands here.
	block_autocmds();

    close_buffer(ଖାଲି, buf, DOBUF_WIPE, FALSE, TRUE, FALSE);

    ଯଦି (!aucmd)
	unblock_autocmds();
}
