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

#ضمن "vim.h"
#ضمن "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
autowrite(buf_T *buf, صحيح forceit)
{
    صحيح		r;
    bufref_T	bufref;

    𐡄𐡍 (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == عدم)
	أرجع FAIL;
    set_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    𐡄𐡍 (bufref_valid(&bufref) && bufIsChanged(buf))
	r = FAIL;
    أرجع r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    فراغ
autowrite_all(فراغ)
{
    buf_T	*buf;

    𐡄𐡍 (!(p_aw || p_awa) || !p_write)
	أرجع;
    FOR_ALL_BUFFERS(buf)
	𐡄𐡍 (bufIsChanged(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    (فراغ)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    𐡄𐡍 (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    صحيح
check_changed(buf_T *buf, صحيح flags)
{
    صحيح		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    𐡄𐡍 (       !forceit
	    && bufIsChanged(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#𐡄𐡍 defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	𐡄𐡍 ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    𐡄𐡍 (term_job_running(buf->b_term))
	    {
		أرجع term_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    صحيح		count = 0;

	    𐡄𐡍 (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    𐡄𐡍 (bufIsChanged(buf2)
				     && (buf2->b_ffname != عدم
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++count;
	    𐡄𐡍 (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		أرجع FALSE;

	    dialog_changed(buf, count > 1);

	    𐡄𐡍 (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		أرجع FALSE;
	    أرجع bufIsChanged(buf);
	}
#endif
	𐡄𐡍 (flags & CCGD_EXCMD)
	    no_write_message();
	وإلا
	    no_write_message_nobang(curbuf);
	أرجع TRUE;
    }
    أرجع FALSE;
}

#𐡄𐡍 defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# 𐡄𐡍 defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    فراغ
browse_save_fname(buf_T *buf)
{
    𐡄𐡍 (buf->b_fname != عدم)
	أرجع;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", عدم, عدم, عدم, buf);
    𐡄𐡍 (fname == عدم)
	أرجع;

    𐡄𐡍 (setfname(buf, fname, عدم, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    فراغ
dialog_changed(
    buf_T	*buf,
    صحيح		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_MSG_SIZE];
    صحيح		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_msg(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    𐡄𐡍 (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, عدم, buff, 1);
    وإلا
	ret = vim_dialog_yesnocancel(VIM_QUESTION, عدم, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_FIELD(ea);

    𐡄𐡍 (ret == VIM_YES)
    {
	صحيح	empty_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	𐡄𐡍 (buf->b_fname == عدم)
	    أرجع;
# endif
	empty_bufname = buf->b_fname == عدم ? TRUE : FALSE;
	𐡄𐡍 (empty_bufname)
	    buf_set_name(buf->b_fnum, (char_u *)"Untitled");

	𐡄𐡍 (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    𐡄𐡍 (buf_write_all(buf, FALSE) == OK)
		أرجع;
	}

	// restore to empty when write failed or was cancelled
	𐡄𐡍 (empty_bufname)
	{
	    buf->b_fname = عدم;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    وإلا 𐡄𐡍 (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    وإلا 𐡄𐡍 (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    𐡄𐡍 (bufIsChanged(buf2)
		    && (buf2->b_ffname != عدم
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
		𐡄𐡍 (buf2->b_fname != عدم && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (فراغ)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		𐡄𐡍 (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    وإلا 𐡄𐡍 (ret == VIM_DISCARDALL)
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
    صحيح
can_abandon(buf_T *buf, صحيح forceit)
{
    أرجع (	   buf_hide(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    ساكن فراغ
add_bufnum(صحيح *bufnrs, صحيح *bufnump, صحيح nr)
{
    صحيح i;

    لكل (i = 0; i < *bufnump; ++i)
	𐡄𐡍 (bufnrs[i] == nr)
	    أرجع;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    صحيح
check_changed_any(
    صحيح		hidden,		// Only check hidden buffers
    صحيح		unload)
{
    صحيح		ret = FALSE;
    buf_T	*buf;
    صحيح		save;
    صحيح		i;
    صحيح		bufnum = 0;
    صحيح		bufcount = 0;
    صحيح		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    𐡄𐡍 (bufcount == 0)
	أرجع FALSE;

    bufnrs = ALLOC_MULT(صحيح, bufcount);
    𐡄𐡍 (bufnrs == عدم)
	أرجع FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	𐡄𐡍 (wp->w_buffer != curbuf)
	    add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	𐡄𐡍 (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	add_bufnum(bufnrs, &bufnum, buf->b_fnum);

    لكل (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	𐡄𐡍 (buf == عدم)
	    استمر;
	𐡄𐡍 ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
	{
	    bufref_T bufref;

	    set_bufref(&bufref, buf);
#ifdef FEAT_TERMINAL
	    𐡄𐡍 (term_job_running(buf->b_term))
	    {
		𐡄𐡍 (term_try_stop_job(buf) == FAIL)
		    اكسر;
	    }
	    وإلا
#endif
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    𐡄𐡍 (check_changed(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		اكسر;	    // didn't save - still changes
	}
    }

    𐡄𐡍 (i >= bufnum)
	اذهب theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#𐡄𐡍 defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    𐡄𐡍 (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#endif
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	𐡄𐡍 (vgetc_busy > 0)
	{
	    msg_row = cmdline_row;
	    msg_col = 0;
	    msg_didout = FALSE;
	}
	𐡄𐡍 (
#ifdef FEAT_TERMINAL
		term_job_running(buf->b_term)
		    ? semsg(_(e_job_still_running_in_buffer_str), buf->b_fname)
		    :
#endif
		semsg(_(e_no_write_since_last_change_for_buffer_str),
		    buf_spname(buf) != عدم ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    𐡄𐡍 (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    𐡄𐡍 (wp->w_buffer == buf)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		𐡄𐡍 (!bufref_valid(&bufref))
		    اذهب theend;
		اذهب buf_found;
	    }
buf_found:

    // Open the changed buffer in the current window.
    𐡄𐡍 (buf != curbuf)
	set_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    أرجع ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    صحيح
check_fname(فراغ)
{
    𐡄𐡍 (curbuf->b_ffname == عدم)
    {
	emsg(_(e_no_file_name));
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
buf_write_all(buf_T *buf, صحيح forceit)
{
    صحيح	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_line_count, عدم,
						  FALSE, forceit, TRUE, FALSE));
    𐡄𐡍 (curbuf != old_curbuf)
    {
	msg_source(HL_ATTR(HLF_W));
	msg(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    أرجع retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    فراغ
ex_listdo(exarg_T *eap)
{
    صحيح		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    صحيح		next_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    𐡄𐡍 (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	𐡄𐡍 ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_to_buffer));
	    أرجع;
	}

	𐡄𐡍 (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	𐡄𐡍 (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (فراغ)win_split(0, 0);

	    𐡄𐡍 (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		أرجع;
	    }
	}
    }

#𐡄𐡍 defined(FEAT_SYN_HL)
    char_u	*save_ei = عدم;
#endif
#ifdef FEAT_QUICKFIX
    صحيح		qf_size = 0;
    صحيح		qf_idx;
#endif

#ifndef FEAT_QUICKFIX
    𐡄𐡍 (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	أرجع;
    }
#endif

#𐡄𐡍 defined(FEAT_SYN_HL)
    𐡄𐡍 (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
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

    𐡄𐡍 (eap->cmdidx == CMD_windo
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
	بدل (eap->cmdidx)
	{
	    حالة CMD_windo:
		لكل ( ; wp != عدم && i + 1 < eap->line1; wp = wp->w_next)
		    i++;
		اكسر;
	    حالة CMD_tabdo:
		لكل ( ; tp != عدم && i + 1 < eap->line1; tp = tp->tp_next)
		    i++;
		اكسر;
	    حالة CMD_argdo:
		i = eap->line1 - 1;
		اكسر;
	    افتراضي:
		اكسر;
	}
	// set pcmark now
	𐡄𐡍 (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    لكل (buf = firstbuf; buf != عدم && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_next)
		𐡄𐡍 (buf->b_fnum > eap->line2)
		{
		    buf = عدم;
		    اكسر;
		}
	    𐡄𐡍 (buf != عدم)
		goto_buffer(eap, DOBUF_FIRST, FORWARD, buf->b_fnum);
	}
#ifdef FEAT_QUICKFIX
	وإلا 𐡄𐡍 (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_size = qf_get_valid_size(eap);
	    𐡄𐡍 (qf_size <= 0 || eap->line1 > qf_size)
		buf = عدم;
	    وإلا
	    {
		save_clear_shm_value();
		ex_cc(eap);
		restore_shm_value();

		buf = curbuf;
		i = eap->line1 - 1;
		𐡄𐡍 (eap->addr_count <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_size;
	    }
	}
#endif
	وإلا
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	طالما (!got_int && buf != عدم)
	{
	    𐡄𐡍 (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		𐡄𐡍 (i == ARGCOUNT)
		    اكسر;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		𐡄𐡍 (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_value();
		    do_argfile(eap, i);
		    restore_shm_value();
		}
		𐡄𐡍 (curwin->w_arg_idx != i)
		    اكسر;
	    }
	    وإلا 𐡄𐡍 (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		𐡄𐡍 (!win_valid(wp))
		    اكسر;
		win_goto(wp);
		𐡄𐡍 (curwin != wp)
		    اكسر;  // something must be wrong
		wp = curwin->w_next;
	    }
	    وإلا 𐡄𐡍 (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		𐡄𐡍 (!valid_tabpage(tp))
		    اكسر;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_next;
	    }
	    وإلا 𐡄𐡍 (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		next_fnum = -1;
		لكل (buf = curbuf->b_next; buf != عدم; buf = buf->b_next)
		    𐡄𐡍 (buf->b_p_bl)
		    {
			next_fnum = buf->b_fnum;
			اكسر;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    𐡄𐡍 (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		𐡄𐡍 (next_fnum < 0 || next_fnum > eap->line2)
		    اكسر;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    𐡄𐡍 (buf->b_fnum == next_fnum)
			اكسر;
		𐡄𐡍 (buf == عدم)
		    اكسر;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_value();
		goto_buffer(eap, DOBUF_FIRST, FORWARD, next_fnum);
		restore_shm_value();

		// If autocommands took us elsewhere, quit here.
		𐡄𐡍 (curbuf->b_fnum != next_fnum)
		    اكسر;
	    }

#ifdef FEAT_QUICKFIX
	    𐡄𐡍 (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		𐡄𐡍 (i >= qf_size || i >= eap->line2)
		    اكسر;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_value();
		ex_cnext(eap);
		restore_shm_value();

		// If jumping to the next quickfix entry fails, quit here
		𐡄𐡍 (qf_get_cur_idx(eap) == qf_idx)
		    اكسر;
	    }
#endif

	    𐡄𐡍 (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		𐡄𐡍 (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    𐡄𐡍 (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		𐡄𐡍 (i+1 > eap->line2)
		    اكسر;
	    𐡄𐡍 (eap->cmdidx == CMD_argdo && i >= eap->line2)
		اكسر;
	}
	listcmd_busy = FALSE;
    }

#𐡄𐡍 defined(FEAT_SYN_HL)
    𐡄𐡍 (save_ei != عدم)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	لكل (buf = firstbuf; buf != عدم; buf = bnext)
	{
	    bnext = buf->b_next;
	    𐡄𐡍 (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_SET))
	    {
		buf->b_flags &= ~BF_SYN_SET;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		𐡄𐡍 (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		وإلا
		{
		    aucmd_prepbuf(&aco, buf);
		    𐡄𐡍 (curbuf == buf)
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
    فراغ
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = عدم;
    char_u	*p;

    𐡄𐡍 (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	أرجع;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    𐡄𐡍 (buf == عدم)
	أرجع;

    𐡄𐡍 (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    وإلا
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_value((char_u *)"g:current_compiler");
	𐡄𐡍 (old_cur_comp != عدم)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((حرف *)buf, "compiler/%s.vim", eap->arg);
    𐡄𐡍 (source_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_value((char_u *)"g:current_compiler");
    𐡄𐡍 (p != عدم)
	set_internal_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    𐡄𐡍 (!eap->forceit)
    {
	𐡄𐡍 (old_cur_comp != عدم)
	{
	    set_internal_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	وإلا
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#endif

#𐡄𐡍 defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# 𐡄𐡍 defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    فراغ
init_pyxversion(فراغ)
{
    𐡄𐡍 (p_pyx == 0)
    {
	𐡄𐡍 (python3_enabled(FALSE))
	    p_pyx = 3;
	وإلا 𐡄𐡍 (python_enabled(FALSE))
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
    ساكن صحيح
requires_py_version(char_u *filename)
{
    FILE    *file;
    صحيح	    requires_py_version = 0;
    صحيح	    i, lines;

    lines = (صحيح)p_mls;
    𐡄𐡍 (lines < 0)
	lines = 5;

    file = mch_fopen((حرف *)filename, "r");
    𐡄𐡍 (file == عدم)
	أرجع 0;

    لكل (i = 0; i < lines; i++)
    {
	𐡄𐡍 (vim_fgets(IObuff, IOSIZE, file))
	    اكسر;
	𐡄𐡍 (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    𐡄𐡍 (strstr((حرف *)IObuff + 2, "python2") != عدم)
	    {
		requires_py_version = 2;
		اكسر;
	    }
	    𐡄𐡍 (strstr((حرف *)IObuff + 2, "python3") != عدم)
	    {
		requires_py_version = 3;
		اكسر;
	    }
	}
	IObuff[21] = '\0';
	𐡄𐡍 (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_version = 2;
	    اكسر;
	}
	𐡄𐡍 (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_version = 3;
	    اكسر;
	}
    }
    fclose(file);
    أرجع requires_py_version;
}


/*
 * Source a python file using the requested python version.
 */
    ساكن فراغ
source_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    صحيح	    v = requires_py_version(fname);

# 𐡄𐡍 defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    𐡄𐡍 (v == 0)
    {
# 𐡄𐡍 defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    𐡄𐡍 (eap == عدم)
	CLEAR_FIELD(ex);
    وإلا
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    𐡄𐡍 (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# وإلا
	vim_snprintf((حرف *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	msg((حرف *)IObuff);
# endif
	أرجع;
    }
    وإلا
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# وإلا
	vim_snprintf((حرف *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	msg((حرف *)IObuff);
# endif
	أرجع;
    }
}

/*
 * ":pyxfile {fname}"
 */
    فراغ
ex_pyxfile(exarg_T *eap)
{
    source_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    فراغ
ex_pyx(exarg_T *eap)
{
# 𐡄𐡍 defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    𐡄𐡍 (p_pyx == 2)
	ex_python(eap);
    وإلا
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
    فراغ
ex_pyxdo(exarg_T *eap)
{
# 𐡄𐡍 defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    𐡄𐡍 (p_pyx == 2)
	ex_pydo(eap);
    وإلا
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
    فراغ
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    صحيح		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    𐡄𐡍 (eap->addr_count == 0)	// default is all buffers
	check_timestamps(FALSE);
    وإلا
    {
	buf = buflist_findnr((صحيح)eap->line2);
	𐡄𐡍 (buf != عدم)	// cannot happen?
	    (فراغ)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
