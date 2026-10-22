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

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
autowrite(buf_T *buf, പൂർണ്ണസംഖ്യ forceit)
{
    പൂർണ്ണസംഖ്യ		r;
    bufref_T	bufref;

    എങ്കിൽ (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == ശൂന്യ)
	തിരികെ FAIL;
    set_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    എങ്കിൽ (bufref_valid(&bufref) && bufIsChanged(buf))
	r = FAIL;
    തിരികെ r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    ശൂന്യം
autowrite_all(ശൂന്യം)
{
    buf_T	*buf;

    എങ്കിൽ (!(p_aw || p_awa) || !p_write)
	തിരികെ;
    FOR_ALL_BUFFERS(buf)
	എങ്കിൽ (bufIsChanged(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    (ശൂന്യം)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    എങ്കിൽ (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    പൂർണ്ണസംഖ്യ
check_changed(buf_T *buf, പൂർണ്ണസംഖ്യ flags)
{
    പൂർണ്ണസംഖ്യ		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    എങ്കിൽ (       !forceit
	    && bufIsChanged(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	എങ്കിൽ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    എങ്കിൽ (term_job_running(buf->b_term))
	    {
		തിരികെ term_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;

	    എങ്കിൽ (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    എങ്കിൽ (bufIsChanged(buf2)
				     && (buf2->b_ffname != ശൂന്യ
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++എണ്ണം;
	    എങ്കിൽ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		തിരികെ FALSE;

	    dialog_changed(buf, എണ്ണം > 1);

	    എങ്കിൽ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		തിരികെ FALSE;
	    തിരികെ bufIsChanged(buf);
	}
#endif
	എങ്കിൽ (flags & CCGD_EXCMD)
	    no_write_message();
	അല്ലെങ്കിൽ
	    no_write_message_nobang(curbuf);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# എങ്കിൽ defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    ശൂന്യം
browse_save_fname(buf_T *buf)
{
    എങ്കിൽ (buf->b_fname != ശൂന്യ)
	തിരികെ;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", ശൂന്യ, ശൂന്യ, ശൂന്യ, buf);
    എങ്കിൽ (fname == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (setfname(buf, fname, ശൂന്യ, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    ശൂന്യം
dialog_changed(
    buf_T	*buf,
    പൂർണ്ണസംഖ്യ		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_സന്ദേശം_SIZE];
    പൂർണ്ണസംഖ്യ		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_സന്ദേശം(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    എങ്കിൽ (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, ശൂന്യ, buff, 1);
    അല്ലെങ്കിൽ
	ret = vim_dialog_yesnocancel(VIM_QUESTION, ശൂന്യ, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_FIELD(ea);

    എങ്കിൽ (ret == VIM_YES)
    {
	പൂർണ്ണസംഖ്യ	empty_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	എങ്കിൽ (buf->b_fname == ശൂന്യ)
	    തിരികെ;
# endif
	empty_bufname = buf->b_fname == ശൂന്യ ? TRUE : FALSE;
	എങ്കിൽ (empty_bufname)
	    buf_set_name(buf->b_fnum, (char_u *)"Untitled");

	എങ്കിൽ (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    എങ്കിൽ (buf_write_all(buf, FALSE) == OK)
		തിരികെ;
	}

	// restore to empty when write failed or was cancelled
	എങ്കിൽ (empty_bufname)
	{
	    buf->b_fname = ശൂന്യ;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    എങ്കിൽ (bufIsChanged(buf2)
		    && (buf2->b_ffname != ശൂന്യ
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
		എങ്കിൽ (buf2->b_fname != ശൂന്യ && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (ശൂന്യം)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		എങ്കിൽ (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ret == VIM_DISCARDALL)
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
    പൂർണ്ണസംഖ്യ
can_abandon(buf_T *buf, പൂർണ്ണസംഖ്യ forceit)
{
    തിരികെ (	   buf_hide(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    സ്ഥിതം ശൂന്യം
add_bufnum(പൂർണ്ണസംഖ്യ *bufnrs, പൂർണ്ണസംഖ്യ *bufnump, പൂർണ്ണസംഖ്യ nr)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; i < *bufnump; ++i)
	എങ്കിൽ (bufnrs[i] == nr)
	    തിരികെ;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    പൂർണ്ണസംഖ്യ
check_changed_any(
    പൂർണ്ണസംഖ്യ		hidden,		// Only check hidden buffers
    പൂർണ്ണസംഖ്യ		unload)
{
    പൂർണ്ണസംഖ്യ		ret = FALSE;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		save;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		bufnum = 0;
    പൂർണ്ണസംഖ്യ		bufcount = 0;
    പൂർണ്ണസംഖ്യ		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    എങ്കിൽ (bufcount == 0)
	തിരികെ FALSE;

    bufnrs = ALLOC_MULT(പൂർണ്ണസംഖ്യ, bufcount);
    എങ്കിൽ (bufnrs == ശൂന്യ)
	തിരികെ FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	എങ്കിൽ (wp->w_buffer != curbuf)
	    add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	എങ്കിൽ (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	add_bufnum(bufnrs, &bufnum, buf->b_fnum);

    വേണ്ടി (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	എങ്കിൽ (buf == ശൂന്യ)
	    തുടരുക;
	എങ്കിൽ ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
	{
	    bufref_T bufref;

	    set_bufref(&bufref, buf);
#ifdef FEAT_TERMINAL
	    എങ്കിൽ (term_job_running(buf->b_term))
	    {
		എങ്കിൽ (term_try_stop_job(buf) == FAIL)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ
#endif
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    എങ്കിൽ (check_changed(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		നിർത്തുക;	    // didn't save - still changes
	}
    }

    എങ്കിൽ (i >= bufnum)
	പോകുക theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    എങ്കിൽ (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#endif
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	എങ്കിൽ (vgetc_busy > 0)
	{
	    സന്ദേശം_row = cmdline_row;
	    സന്ദേശം_col = 0;
	    സന്ദേശം_didout = FALSE;
	}
	എങ്കിൽ (
#ifdef FEAT_TERMINAL
		term_job_running(buf->b_term)
		    ? semsg(_(e_job_still_running_in_buffer_str), buf->b_fname)
		    :
#endif
		semsg(_(e_no_write_since_last_change_for_buffer_str),
		    buf_spname(buf) != ശൂന്യ ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    എങ്കിൽ (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    എങ്കിൽ (wp->w_buffer == buf)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		എങ്കിൽ (!bufref_valid(&bufref))
		    പോകുക theend;
		പോകുക buf_found;
	    }
buf_found:

    // Open the changed buffer in the current window.
    എങ്കിൽ (buf != curbuf)
	set_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    തിരികെ ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    പൂർണ്ണസംഖ്യ
check_fname(ശൂന്യം)
{
    എങ്കിൽ (curbuf->b_ffname == ശൂന്യ)
    {
	emsg(_(e_no_file_name));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
buf_write_all(buf_T *buf, പൂർണ്ണസംഖ്യ forceit)
{
    പൂർണ്ണസംഖ്യ	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_വരി_എണ്ണം, ശൂന്യ,
						  FALSE, forceit, TRUE, FALSE));
    എങ്കിൽ (curbuf != old_curbuf)
    {
	സന്ദേശം_source(HL_ATTR(HLF_W));
	സന്ദേശം(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    തിരികെ retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    ശൂന്യം
ex_listdo(exarg_T *eap)
{
    പൂർണ്ണസംഖ്യ		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    പൂർണ്ണസംഖ്യ		next_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    എങ്കിൽ (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	എങ്കിൽ ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_to_buffer));
	    തിരികെ;
	}

	എങ്കിൽ (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	എങ്കിൽ (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (ശൂന്യം)win_split(0, 0);

	    എങ്കിൽ (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		തിരികെ;
	    }
	}
    }

#എങ്കിൽ defined(FEAT_SYN_HL)
    char_u	*save_ei = ശൂന്യ;
#endif
#ifdef FEAT_QUICKFIX
    പൂർണ്ണസംഖ്യ		qf_size = 0;
    പൂർണ്ണസംഖ്യ		qf_idx;
#endif

#ifndef FEAT_QUICKFIX
    എങ്കിൽ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	തിരികെ;
    }
#endif

#എങ്കിൽ defined(FEAT_SYN_HL)
    എങ്കിൽ (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
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

    എങ്കിൽ (eap->cmdidx == CMD_windo
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
	തിരഞ്ഞെടുപ്പ് (eap->cmdidx)
	{
	    അവസ്ഥ CMD_windo:
		വേണ്ടി ( ; wp != ശൂന്യ && i + 1 < eap->line1; wp = wp->w_next)
		    i++;
		നിർത്തുക;
	    അവസ്ഥ CMD_tabdo:
		വേണ്ടി ( ; tp != ശൂന്യ && i + 1 < eap->line1; tp = tp->tp_next)
		    i++;
		നിർത്തുക;
	    അവസ്ഥ CMD_argdo:
		i = eap->line1 - 1;
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
	// set pcmark now
	എങ്കിൽ (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    വേണ്ടി (buf = firstbuf; buf != ശൂന്യ && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_next)
		എങ്കിൽ (buf->b_fnum > eap->line2)
		{
		    buf = ശൂന്യ;
		    നിർത്തുക;
		}
	    എങ്കിൽ (buf != ശൂന്യ)
		goto_buffer(eap, DOBUF_FIRST, FORWARD, buf->b_fnum);
	}
#ifdef FEAT_QUICKFIX
	അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_size = qf_get_valid_size(eap);
	    എങ്കിൽ (qf_size <= 0 || eap->line1 > qf_size)
		buf = ശൂന്യ;
	    അല്ലെങ്കിൽ
	    {
		save_clear_shm_മൂല്യം();
		ex_cc(eap);
		restore_shm_മൂല്യം();

		buf = curbuf;
		i = eap->line1 - 1;
		എങ്കിൽ (eap->addr_എണ്ണം <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_size;
	    }
	}
#endif
	അല്ലെങ്കിൽ
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	ആകുമ്പോൾ (!got_int && buf != ശൂന്യ)
	{
	    എങ്കിൽ (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		എങ്കിൽ (i == ARGCOUNT)
		    നിർത്തുക;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		എങ്കിൽ (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_മൂല്യം();
		    do_argfile(eap, i);
		    restore_shm_മൂല്യം();
		}
		എങ്കിൽ (curwin->w_arg_idx != i)
		    നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		എങ്കിൽ (!win_valid(wp))
		    നിർത്തുക;
		win_goto(wp);
		എങ്കിൽ (curwin != wp)
		    നിർത്തുക;  // something must be wrong
		wp = curwin->w_next;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		എങ്കിൽ (!valid_tabpage(tp))
		    നിർത്തുക;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_next;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		next_fnum = -1;
		വേണ്ടി (buf = curbuf->b_next; buf != ശൂന്യ; buf = buf->b_next)
		    എങ്കിൽ (buf->b_p_bl)
		    {
			next_fnum = buf->b_fnum;
			നിർത്തുക;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    എങ്കിൽ (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		എങ്കിൽ (next_fnum < 0 || next_fnum > eap->line2)
		    നിർത്തുക;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    എങ്കിൽ (buf->b_fnum == next_fnum)
			നിർത്തുക;
		എങ്കിൽ (buf == ശൂന്യ)
		    നിർത്തുക;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_മൂല്യം();
		goto_buffer(eap, DOBUF_FIRST, FORWARD, next_fnum);
		restore_shm_മൂല്യം();

		// If autocommands took us elsewhere, quit here.
		എങ്കിൽ (curbuf->b_fnum != next_fnum)
		    നിർത്തുക;
	    }

#ifdef FEAT_QUICKFIX
	    എങ്കിൽ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		എങ്കിൽ (i >= qf_size || i >= eap->line2)
		    നിർത്തുക;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_മൂല്യം();
		ex_cnext(eap);
		restore_shm_മൂല്യം();

		// If jumping to the next quickfix entry fails, quit here
		എങ്കിൽ (qf_get_cur_idx(eap) == qf_idx)
		    നിർത്തുക;
	    }
#endif

	    എങ്കിൽ (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		എങ്കിൽ (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    എങ്കിൽ (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		എങ്കിൽ (i+1 > eap->line2)
		    നിർത്തുക;
	    എങ്കിൽ (eap->cmdidx == CMD_argdo && i >= eap->line2)
		നിർത്തുക;
	}
	listcmd_busy = FALSE;
    }

#എങ്കിൽ defined(FEAT_SYN_HL)
    എങ്കിൽ (save_ei != ശൂന്യ)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	വേണ്ടി (buf = firstbuf; buf != ശൂന്യ; buf = bnext)
	{
	    bnext = buf->b_next;
	    എങ്കിൽ (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_SET))
	    {
		buf->b_flags &= ~BF_SYN_SET;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		എങ്കിൽ (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		അല്ലെങ്കിൽ
		{
		    aucmd_prepbuf(&aco, buf);
		    എങ്കിൽ (curbuf == buf)
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
    ശൂന്യം
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = ശൂന്യ;
    char_u	*p;

    എങ്കിൽ (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	തിരികെ;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    അല്ലെങ്കിൽ
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_മൂല്യം((char_u *)"g:current_compiler");
	എങ്കിൽ (old_cur_comp != ശൂന്യ)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((അക്ഷരം *)buf, "compiler/%s.vim", eap->arg);
    എങ്കിൽ (source_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_മൂല്യം((char_u *)"g:current_compiler");
    എങ്കിൽ (p != ശൂന്യ)
	set_internal_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    എങ്കിൽ (!eap->forceit)
    {
	എങ്കിൽ (old_cur_comp != ശൂന്യ)
	{
	    set_internal_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	അല്ലെങ്കിൽ
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#endif

#എങ്കിൽ defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# എങ്കിൽ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    ശൂന്യം
init_pyxversion(ശൂന്യം)
{
    എങ്കിൽ (p_pyx == 0)
    {
	എങ്കിൽ (python3_enabled(FALSE))
	    p_pyx = 3;
	അല്ലെങ്കിൽ എങ്കിൽ (python_enabled(FALSE))
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
requires_py_version(char_u *filename)
{
    FILE    *file;
    പൂർണ്ണസംഖ്യ	    requires_py_version = 0;
    പൂർണ്ണസംഖ്യ	    i, വരികൾ;

    വരികൾ = (പൂർണ്ണസംഖ്യ)p_mls;
    എങ്കിൽ (വരികൾ < 0)
	വരികൾ = 5;

    file = mch_fopen((അക്ഷരം *)filename, "r");
    എങ്കിൽ (file == ശൂന്യ)
	തിരികെ 0;

    വേണ്ടി (i = 0; i < വരികൾ; i++)
    {
	എങ്കിൽ (vim_fgets(IObuff, IOSIZE, file))
	    നിർത്തുക;
	എങ്കിൽ (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    എങ്കിൽ (strstr((അക്ഷരം *)IObuff + 2, "python2") != ശൂന്യ)
	    {
		requires_py_version = 2;
		നിർത്തുക;
	    }
	    എങ്കിൽ (strstr((അക്ഷരം *)IObuff + 2, "python3") != ശൂന്യ)
	    {
		requires_py_version = 3;
		നിർത്തുക;
	    }
	}
	IObuff[21] = '\0';
	എങ്കിൽ (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_version = 2;
	    നിർത്തുക;
	}
	എങ്കിൽ (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_version = 3;
	    നിർത്തുക;
	}
    }
    fclose(file);
    തിരികെ requires_py_version;
}


/*
 * Source a python file using the requested python version.
 */
    സ്ഥിതം ശൂന്യം
source_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    പൂർണ്ണസംഖ്യ	    v = requires_py_version(fname);

# എങ്കിൽ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    എങ്കിൽ (v == 0)
    {
# എങ്കിൽ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    എങ്കിൽ (eap == ശൂന്യ)
	CLEAR_FIELD(ex);
    അല്ലെങ്കിൽ
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    എങ്കിൽ (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# അല്ലെങ്കിൽ
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	സന്ദേശം((അക്ഷരം *)IObuff);
# endif
	തിരികെ;
    }
    അല്ലെങ്കിൽ
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# അല്ലെങ്കിൽ
	vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	സന്ദേശം((അക്ഷരം *)IObuff);
# endif
	തിരികെ;
    }
}

/*
 * ":pyxfile {fname}"
 */
    ശൂന്യം
ex_pyxfile(exarg_T *eap)
{
    source_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    ശൂന്യം
ex_pyx(exarg_T *eap)
{
# എങ്കിൽ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    എങ്കിൽ (p_pyx == 2)
	ex_python(eap);
    അല്ലെങ്കിൽ
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
    ശൂന്യം
ex_pyxdo(exarg_T *eap)
{
# എങ്കിൽ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    എങ്കിൽ (p_pyx == 2)
	ex_pydo(eap);
    അല്ലെങ്കിൽ
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
    ശൂന്യം
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    എങ്കിൽ (eap->addr_എണ്ണം == 0)	// default is all buffers
	check_timestamps(FALSE);
    അല്ലെങ്കിൽ
    {
	buf = buflist_findnr((പൂർണ്ണസംഖ്യ)eap->line2);
	എങ്കിൽ (buf != ശൂന്യ)	// cannot happen?
	    (ശൂന്യം)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
