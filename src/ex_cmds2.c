/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * ex_cmds2.c: some more functions for command line commands
 */

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
autowrite(buf_T *buf, முழு forceit)
{
    முழு		r;
    bufref_T	bufref;

    எனில் (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == வெற்று)
	திரும்பு FAIL;
    set_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    எனில் (bufref_valid(&bufref) && bufIsChanged(buf))
	r = FAIL;
    திரும்பு r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    வெறுமை
autowrite_all(வெறுமை)
{
    buf_T	*buf;

    எனில் (!(p_aw || p_awa) || !p_write)
	திரும்பு;
    FOR_ALL_BUFFERS(buf)
	எனில் (bufIsChanged(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    (வெறுமை)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    எனில் (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    முழு
check_changed(buf_T *buf, முழு flags)
{
    முழு		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    எனில் (       !forceit
	    && bufIsChanged(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	எனில் ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    எனில் (term_job_running(buf->b_term))
	    {
		திரும்பு term_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    முழு		மொத்தம் = 0;

	    எனில் (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    எனில் (bufIsChanged(buf2)
				     && (buf2->b_ffname != வெற்று
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++மொத்தம்;
	    எனில் (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		திரும்பு FALSE;

	    dialog_changed(buf, மொத்தம் > 1);

	    எனில் (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		திரும்பு FALSE;
	    திரும்பு bufIsChanged(buf);
	}
#endif
	எனில் (flags & CCGD_EXCMD)
	    no_write_message();
	இல்லையெனில்
	    no_write_message_nobang(curbuf);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# எனில் defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    வெறுமை
browse_save_fname(buf_T *buf)
{
    எனில் (buf->b_fname != வெற்று)
	திரும்பு;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", வெற்று, வெற்று, வெற்று, buf);
    எனில் (fname == வெற்று)
	திரும்பு;

    எனில் (setfname(buf, fname, வெற்று, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    வெறுமை
dialog_changed(
    buf_T	*buf,
    முழு		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_செய்தி_SIZE];
    முழு		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_செய்தி(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    எனில் (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, வெற்று, buff, 1);
    இல்லையெனில்
	ret = vim_dialog_yesnocancel(VIM_QUESTION, வெற்று, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_FIELD(ea);

    எனில் (ret == VIM_YES)
    {
	முழு	empty_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	எனில் (buf->b_fname == வெற்று)
	    திரும்பு;
# endif
	empty_bufname = buf->b_fname == வெற்று ? TRUE : FALSE;
	எனில் (empty_bufname)
	    buf_set_name(buf->b_fnum, (char_u *)"Untitled");

	எனில் (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    எனில் (buf_write_all(buf, FALSE) == OK)
		திரும்பு;
	}

	// restore to empty when write failed or was cancelled
	எனில் (empty_bufname)
	{
	    buf->b_fname = வெற்று;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    இல்லையெனில் எனில் (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    இல்லையெனில் எனில் (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    எனில் (bufIsChanged(buf2)
		    && (buf2->b_ffname != வெற்று
# ifdef FEAT_BROWSE
			|| (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
			)
		    && !bt_dontwrite(buf2)
		    && !buf2->b_p_ro)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf2);
# ifdef FEAT_BROWSE
		// May get file name, when there is none
		browse_save_fname(buf2);
# endif
		எனில் (buf2->b_fname != வெற்று && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (வெறுமை)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		எனில் (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    இல்லையெனில் எனில் (ret == VIM_DISCARDALL)
    {
	/*
	 * mark all buffers as unchanged
	 */
	FOR_ALL_BUFFERS(buf2)
	    unchanged(buf2, TRUE, FALSE);
    }
}
#endif

/*
 * Return TRUE if the buffer "buf" can be abandoned, either by making it
 * hidden, autowriting it or unloading it.
 */
    முழு
can_abandon(buf_T *buf, முழு forceit)
{
    திரும்பு (	   buf_hide(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    நிலையான வெறுமை
add_bufnum(முழு *bufnrs, முழு *bufnump, முழு nr)
{
    முழு i;

    க்கு (i = 0; i < *bufnump; ++i)
	எனில் (bufnrs[i] == nr)
	    திரும்பு;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    முழு
check_changed_any(
    முழு		hidden,		// Only check hidden buffers
    முழு		unload)
{
    முழு		ret = FALSE;
    buf_T	*buf;
    முழு		save;
    முழு		i;
    முழு		bufnum = 0;
    முழு		bufcount = 0;
    முழு		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    எனில் (bufcount == 0)
	திரும்பு FALSE;

    bufnrs = ALLOC_MULT(முழு, bufcount);
    எனில் (bufnrs == வெற்று)
	திரும்பு FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	எனில் (wp->w_buffer != curbuf)
	    add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	எனில் (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	add_bufnum(bufnrs, &bufnum, buf->b_fnum);

    க்கு (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	எனில் (buf == வெற்று)
	    தொடர்;
	எனில் ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
	{
	    bufref_T bufref;

	    set_bufref(&bufref, buf);
#ifdef FEAT_TERMINAL
	    எனில் (term_job_running(buf->b_term))
	    {
		எனில் (term_try_stop_job(buf) == FAIL)
		    நிறுத்து;
	    }
	    இல்லையெனில்
#endif
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    எனில் (check_changed(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		நிறுத்து;	    // didn't save - still changes
	}
    }

    எனில் (i >= bufnum)
	செல் theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#எனில் defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    எனில் (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#endif
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	எனில் (vgetc_busy > 0)
	{
	    செய்தி_row = cmdline_row;
	    செய்தி_col = 0;
	    செய்தி_didout = FALSE;
	}
	எனில் (
#ifdef FEAT_TERMINAL
		term_job_running(buf->b_term)
		    ? semsg(_(e_job_still_running_in_buffer_str), buf->b_fname)
		    :
#endif
		semsg(_(e_no_write_since_last_change_for_buffer_str),
		    buf_spname(buf) != வெற்று ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    எனில் (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    எனில் (wp->w_buffer == buf)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		எனில் (!bufref_valid(&bufref))
		    செல் theend;
		செல் buf_found;
	    }
buf_found:

    // Open the changed buffer in the current window.
    எனில் (buf != curbuf)
	set_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    திரும்பு ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    முழு
check_fname(வெறுமை)
{
    எனில் (curbuf->b_ffname == வெற்று)
    {
	emsg(_(e_no_file_name));
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
buf_write_all(buf_T *buf, முழு forceit)
{
    முழு	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_வரி_மொத்தம், வெற்று,
						  FALSE, forceit, TRUE, FALSE));
    எனில் (curbuf != old_curbuf)
    {
	செய்தி_source(HL_ATTR(HLF_W));
	செய்தி(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    திரும்பு retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    வெறுமை
ex_listdo(exarg_T *eap)
{
    முழு		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    முழு		next_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    எனில் (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	எனில் ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_to_buffer));
	    திரும்பு;
	}

	எனில் (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	எனில் (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (வெறுமை)win_split(0, 0);

	    எனில் (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		திரும்பு;
	    }
	}
    }

#எனில் defined(FEAT_SYN_HL)
    char_u	*save_ei = வெற்று;
#endif
#ifdef FEAT_QUICKFIX
    முழு		qf_size = 0;
    முழு		qf_idx;
#endif

#ifndef FEAT_QUICKFIX
    எனில் (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	திரும்பு;
    }
#endif

#எனில் defined(FEAT_SYN_HL)
    எனில் (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
    {
	// Don't do syntax HL autocommands.  Skipping the syntax file is a
	// great speed improvement.
	save_ei = au_event_disable(",Syntax");

	FOR_ALL_BUFFERS(buf)
	    buf->b_flags &= ~BF_SYN_SET;
	buf = curbuf;
    }
#endif
#ifdef FEAT_CLIPBOARD
    start_global_changes();
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#endif

    எனில் (eap->cmdidx == CMD_windo
	    || eap->cmdidx == CMD_tabdo
	    || buf_hide(curbuf)
	    || !check_changed(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	i = 0;
	// start at the eap->line1 argument/window/buffer
	wp = firstwin;
	tp = first_tabpage;
	தேர்வு (eap->cmdidx)
	{
	    நிலை CMD_windo:
		க்கு ( ; wp != வெற்று && i + 1 < eap->line1; wp = wp->w_next)
		    i++;
		நிறுத்து;
	    நிலை CMD_tabdo:
		க்கு ( ; tp != வெற்று && i + 1 < eap->line1; tp = tp->tp_next)
		    i++;
		நிறுத்து;
	    நிலை CMD_argdo:
		i = eap->line1 - 1;
		நிறுத்து;
	    இயல்பு:
		நிறுத்து;
	}
	// set pcmark now
	எனில் (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    க்கு (buf = firstbuf; buf != வெற்று && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_next)
		எனில் (buf->b_fnum > eap->line2)
		{
		    buf = வெற்று;
		    நிறுத்து;
		}
	    எனில் (buf != வெற்று)
		goto_buffer(eap, DOBUF_FIRST, FORWARD, buf->b_fnum);
	}
#ifdef FEAT_QUICKFIX
	இல்லையெனில் எனில் (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_size = qf_get_valid_size(eap);
	    எனில் (qf_size <= 0 || eap->line1 > qf_size)
		buf = வெற்று;
	    இல்லையெனில்
	    {
		save_clear_shm_மதிப்பு();
		ex_cc(eap);
		restore_shm_மதிப்பு();

		buf = curbuf;
		i = eap->line1 - 1;
		எனில் (eap->addr_மொத்தம் <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_size;
	    }
	}
#endif
	இல்லையெனில்
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	போது (!got_int && buf != வெற்று)
	{
	    எனில் (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		எனில் (i == ARGCOUNT)
		    நிறுத்து;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		எனில் (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_மதிப்பு();
		    do_argfile(eap, i);
		    restore_shm_மதிப்பு();
		}
		எனில் (curwin->w_arg_idx != i)
		    நிறுத்து;
	    }
	    இல்லையெனில் எனில் (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		எனில் (!win_valid(wp))
		    நிறுத்து;
		win_goto(wp);
		எனில் (curwin != wp)
		    நிறுத்து;  // something must be wrong
		wp = curwin->w_next;
	    }
	    இல்லையெனில் எனில் (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		எனில் (!valid_tabpage(tp))
		    நிறுத்து;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_next;
	    }
	    இல்லையெனில் எனில் (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		next_fnum = -1;
		க்கு (buf = curbuf->b_next; buf != வெற்று; buf = buf->b_next)
		    எனில் (buf->b_p_bl)
		    {
			next_fnum = buf->b_fnum;
			நிறுத்து;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    எனில் (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		எனில் (next_fnum < 0 || next_fnum > eap->line2)
		    நிறுத்து;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    எனில் (buf->b_fnum == next_fnum)
			நிறுத்து;
		எனில் (buf == வெற்று)
		    நிறுத்து;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_மதிப்பு();
		goto_buffer(eap, DOBUF_FIRST, FORWARD, next_fnum);
		restore_shm_மதிப்பு();

		// If autocommands took us elsewhere, quit here.
		எனில் (curbuf->b_fnum != next_fnum)
		    நிறுத்து;
	    }

#ifdef FEAT_QUICKFIX
	    எனில் (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		எனில் (i >= qf_size || i >= eap->line2)
		    நிறுத்து;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_மதிப்பு();
		ex_cnext(eap);
		restore_shm_மதிப்பு();

		// If jumping to the next quickfix entry fails, quit here
		எனில் (qf_get_cur_idx(eap) == qf_idx)
		    நிறுத்து;
	    }
#endif

	    எனில் (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		எனில் (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    எனில் (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		எனில் (i+1 > eap->line2)
		    நிறுத்து;
	    எனில் (eap->cmdidx == CMD_argdo && i >= eap->line2)
		நிறுத்து;
	}
	listcmd_busy = FALSE;
    }

#எனில் defined(FEAT_SYN_HL)
    எனில் (save_ei != வெற்று)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	க்கு (buf = firstbuf; buf != வெற்று; buf = bnext)
	{
	    bnext = buf->b_next;
	    எனில் (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_SET))
	    {
		buf->b_flags &= ~BF_SYN_SET;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		எனில் (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		இல்லையெனில்
		{
		    aucmd_prepbuf(&aco, buf);
		    எனில் (curbuf == buf)
		    {
			apply_autocmds(EVENT_SYNTAX, buf->b_p_syn,
						      buf->b_fname, TRUE, buf);
			aucmd_restbuf(&aco);
		    }
		}

		// start over, in case autocommands messed things up.
		bnext = firstbuf;
	    }
	}
    }
#endif
#ifdef FEAT_CLIPBOARD
    end_global_changes();
#endif
#ifdef FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#endif
}

#ifdef FEAT_EVAL
/*
 * ":compiler[!] {name}"
 */
    வெறுமை
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = வெற்று;
    char_u	*p;

    எனில் (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	திரும்பு;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    எனில் (buf == வெற்று)
	திரும்பு;

    எனில் (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    இல்லையெனில்
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_மதிப்பு((char_u *)"g:current_compiler");
	எனில் (old_cur_comp != வெற்று)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((எழுத்து *)buf, "compiler/%s.vim", eap->arg);
    எனில் (source_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_மதிப்பு((char_u *)"g:current_compiler");
    எனில் (p != வெற்று)
	set_internal_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    எனில் (!eap->forceit)
    {
	எனில் (old_cur_comp != வெற்று)
	{
	    set_internal_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	இல்லையெனில்
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#endif

#எனில் defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# எனில் defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    வெறுமை
init_pyxversion(வெறுமை)
{
    எனில் (p_pyx == 0)
    {
	எனில் (python3_enabled(FALSE))
	    p_pyx = 3;
	இல்லையெனில் எனில் (python_enabled(FALSE))
	    p_pyx = 2;
    }
}
# endif

/*
 * Does a file contain one of the following strings at the beginning of any
 * line?
 * "#!(any string)python2"  => returns 2
 * "#!(any string)python3"  => returns 3
 * "# requires python 2.x"  => returns 2
 * "# requires python 3.x"  => returns 3
 * otherwise return 0.
 */
    நிலையான முழு
requires_py_version(char_u *filename)
{
    FILE    *file;
    முழு	    requires_py_version = 0;
    முழு	    i, வரிகள்;

    வரிகள் = (முழு)p_mls;
    எனில் (வரிகள் < 0)
	வரிகள் = 5;

    file = mch_fopen((எழுத்து *)filename, "r");
    எனில் (file == வெற்று)
	திரும்பு 0;

    க்கு (i = 0; i < வரிகள்; i++)
    {
	எனில் (vim_fgets(IObuff, IOSIZE, file))
	    நிறுத்து;
	எனில் (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    எனில் (strstr((எழுத்து *)IObuff + 2, "python2") != வெற்று)
	    {
		requires_py_version = 2;
		நிறுத்து;
	    }
	    எனில் (strstr((எழுத்து *)IObuff + 2, "python3") != வெற்று)
	    {
		requires_py_version = 3;
		நிறுத்து;
	    }
	}
	IObuff[21] = '\0';
	எனில் (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_version = 2;
	    நிறுத்து;
	}
	எனில் (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_version = 3;
	    நிறுத்து;
	}
    }
    fclose(file);
    திரும்பு requires_py_version;
}


/*
 * Source a python file using the requested python version.
 */
    நிலையான வெறுமை
source_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    முழு	    v = requires_py_version(fname);

# எனில் defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    எனில் (v == 0)
    {
# எனில் defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
	// user didn't choose a preference, 'pyx' is used
	v = p_pyx;
# elif defined(FEAT_PYTHON)
	v = 2;
# elif defined(FEAT_PYTHON3)
	v = 3;
# endif
    }

    /*
     * now source, if required python version is not supported show
     * unobtrusive message.
     */
    எனில் (eap == வெற்று)
	CLEAR_FIELD(ex);
    இல்லையெனில்
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    எனில் (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# இல்லையெனில்
	vim_snprintf((எழுத்து *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	செய்தி((எழுத்து *)IObuff);
# endif
	திரும்பு;
    }
    இல்லையெனில்
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# இல்லையெனில்
	vim_snprintf((எழுத்து *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	செய்தி((எழுத்து *)IObuff);
# endif
	திரும்பு;
    }
}

/*
 * ":pyxfile {fname}"
 */
    வெறுமை
ex_pyxfile(exarg_T *eap)
{
    source_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    வெறுமை
ex_pyx(exarg_T *eap)
{
# எனில் defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    எனில் (p_pyx == 2)
	ex_python(eap);
    இல்லையெனில்
	ex_py3(eap);
# elif defined(FEAT_PYTHON)
    ex_python(eap);
# elif defined(FEAT_PYTHON3)
    ex_py3(eap);
# endif
}

/*
 * ":pyxdo"
 */
    வெறுமை
ex_pyxdo(exarg_T *eap)
{
# எனில் defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    எனில் (p_pyx == 2)
	ex_pydo(eap);
    இல்லையெனில்
	ex_py3do(eap);
# elif defined(FEAT_PYTHON)
    ex_pydo(eap);
# elif defined(FEAT_PYTHON3)
    ex_py3do(eap);
# endif
}

#endif

/*
 * ":checktime [buffer]"
 */
    வெறுமை
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    முழு		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    எனில் (eap->addr_மொத்தம் == 0)	// default is all buffers
	check_timestamps(FALSE);
    இல்லையெனில்
    {
	buf = buflist_findnr((முழு)eap->line2);
	எனில் (buf != வெற்று)	// cannot happen?
	    (வெறுமை)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
