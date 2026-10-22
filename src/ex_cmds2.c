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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
autowrite(buf_T *buf, પૂર્ણાંક forceit)
{
    પૂર્ણાંક		r;
    bufref_T	bufref;

    જો (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == ખાલી)
	પાછા FAIL;
    set_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    જો (bufref_valid(&bufref) && bufIsChanged(buf))
	r = FAIL;
    પાછા r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    શૂન્ય
autowrite_all(શૂન્ય)
{
    buf_T	*buf;

    જો (!(p_aw || p_awa) || !p_write)
	પાછા;
    FOR_ALL_BUFFERS(buf)
	જો (bufIsChanged(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    (શૂન્ય)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    જો (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    પૂર્ણાંક
check_changed(buf_T *buf, પૂર્ણાંક flags)
{
    પૂર્ણાંક		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    જો (       !forceit
	    && bufIsChanged(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	જો ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    જો (term_job_running(buf->b_term))
	    {
		પાછા term_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    પૂર્ણાંક		ગણતરી = 0;

	    જો (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    જો (bufIsChanged(buf2)
				     && (buf2->b_ffname != ખાલી
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++ગણતરી;
	    જો (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		પાછા FALSE;

	    dialog_changed(buf, ગણતરી > 1);

	    જો (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		પાછા FALSE;
	    પાછા bufIsChanged(buf);
	}
#endif
	જો (flags & CCGD_EXCMD)
	    no_write_message();
	નહીંતર
	    no_write_message_nobang(curbuf);
	પાછા TRUE;
    }
    પાછા FALSE;
}

#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# જો defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    શૂન્ય
browse_save_fname(buf_T *buf)
{
    જો (buf->b_fname != ખાલી)
	પાછા;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", ખાલી, ખાલી, ખાલી, buf);
    જો (fname == ખાલી)
	પાછા;

    જો (setfname(buf, fname, ખાલી, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    શૂન્ય
dialog_changed(
    buf_T	*buf,
    પૂર્ણાંક		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_સંદેશ_SIZE];
    પૂર્ણાંક		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_સંદેશ(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    જો (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, ખાલી, buff, 1);
    નહીંતર
	ret = vim_dialog_yesnocancel(VIM_QUESTION, ખાલી, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_FIELD(ea);

    જો (ret == VIM_YES)
    {
	પૂર્ણાંક	empty_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	જો (buf->b_fname == ખાલી)
	    પાછા;
# endif
	empty_bufname = buf->b_fname == ખાલી ? TRUE : FALSE;
	જો (empty_bufname)
	    buf_set_name(buf->b_fnum, (char_u *)"Untitled");

	જો (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    જો (buf_write_all(buf, FALSE) == OK)
		પાછા;
	}

	// restore to empty when write failed or was cancelled
	જો (empty_bufname)
	{
	    buf->b_fname = ખાલી;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    નહીંતર જો (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    નહીંતર જો (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    જો (bufIsChanged(buf2)
		    && (buf2->b_ffname != ખાલી
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
		જો (buf2->b_fname != ખાલી && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (શૂન્ય)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		જો (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    નહીંતર જો (ret == VIM_DISCARDALL)
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
    પૂર્ણાંક
can_abandon(buf_T *buf, પૂર્ણાંક forceit)
{
    પાછા (	   buf_hide(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    સ્થિર શૂન્ય
add_bufnum(પૂર્ણાંક *bufnrs, પૂર્ણાંક *bufnump, પૂર્ણાંક nr)
{
    પૂર્ણાંક i;

    માટે (i = 0; i < *bufnump; ++i)
	જો (bufnrs[i] == nr)
	    પાછા;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    પૂર્ણાંક
check_changed_any(
    પૂર્ણાંક		hidden,		// Only check hidden buffers
    પૂર્ણાંક		unload)
{
    પૂર્ણાંક		ret = FALSE;
    buf_T	*buf;
    પૂર્ણાંક		save;
    પૂર્ણાંક		i;
    પૂર્ણાંક		bufnum = 0;
    પૂર્ણાંક		bufcount = 0;
    પૂર્ણાંક		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    જો (bufcount == 0)
	પાછા FALSE;

    bufnrs = ALLOC_MULT(પૂર્ણાંક, bufcount);
    જો (bufnrs == ખાલી)
	પાછા FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	જો (wp->w_buffer != curbuf)
	    add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	જો (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	add_bufnum(bufnrs, &bufnum, buf->b_fnum);

    માટે (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	જો (buf == ખાલી)
	    ચાલુ;
	જો ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
	{
	    bufref_T bufref;

	    set_bufref(&bufref, buf);
#ifdef FEAT_TERMINAL
	    જો (term_job_running(buf->b_term))
	    {
		જો (term_try_stop_job(buf) == FAIL)
		    તોડો;
	    }
	    નહીંતર
#endif
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    જો (check_changed(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		તોડો;	    // didn't save - still changes
	}
    }

    જો (i >= bufnum)
	જાઓ theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    જો (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#endif
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	જો (vgetc_busy > 0)
	{
	    સંદેશ_row = cmdline_row;
	    સંદેશ_col = 0;
	    સંદેશ_didout = FALSE;
	}
	જો (
#ifdef FEAT_TERMINAL
		term_job_running(buf->b_term)
		    ? semsg(_(e_job_still_running_in_buffer_str), buf->b_fname)
		    :
#endif
		semsg(_(e_no_write_since_last_change_for_buffer_str),
		    buf_spname(buf) != ખાલી ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    જો (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    જો (wp->w_buffer == buf)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		જો (!bufref_valid(&bufref))
		    જાઓ theend;
		જાઓ buf_found;
	    }
buf_found:

    // Open the changed buffer in the current window.
    જો (buf != curbuf)
	set_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    પાછા ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    પૂર્ણાંક
check_fname(શૂન્ય)
{
    જો (curbuf->b_ffname == ખાલી)
    {
	emsg(_(e_no_file_name));
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
buf_write_all(buf_T *buf, પૂર્ણાંક forceit)
{
    પૂર્ણાંક	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_લીટી_ગણતરી, ખાલી,
						  FALSE, forceit, TRUE, FALSE));
    જો (curbuf != old_curbuf)
    {
	સંદેશ_source(HL_ATTR(HLF_W));
	સંદેશ(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    પાછા retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    શૂન્ય
ex_listdo(exarg_T *eap)
{
    પૂર્ણાંક		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    પૂર્ણાંક		next_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    જો (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	જો ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_to_buffer));
	    પાછા;
	}

	જો (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	જો (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (શૂન્ય)win_split(0, 0);

	    જો (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		પાછા;
	    }
	}
    }

#જો defined(FEAT_SYN_HL)
    char_u	*save_ei = ખાલી;
#endif
#ifdef FEAT_QUICKFIX
    પૂર્ણાંક		qf_size = 0;
    પૂર્ણાંક		qf_idx;
#endif

#ifndef FEAT_QUICKFIX
    જો (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	પાછા;
    }
#endif

#જો defined(FEAT_SYN_HL)
    જો (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
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

    જો (eap->cmdidx == CMD_windo
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
	પસંદગી (eap->cmdidx)
	{
	    સ્થિતિ CMD_windo:
		માટે ( ; wp != ખાલી && i + 1 < eap->line1; wp = wp->w_next)
		    i++;
		તોડો;
	    સ્થિતિ CMD_tabdo:
		માટે ( ; tp != ખાલી && i + 1 < eap->line1; tp = tp->tp_next)
		    i++;
		તોડો;
	    સ્થિતિ CMD_argdo:
		i = eap->line1 - 1;
		તોડો;
	    મૂળભૂત:
		તોડો;
	}
	// set pcmark now
	જો (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    માટે (buf = firstbuf; buf != ખાલી && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_next)
		જો (buf->b_fnum > eap->line2)
		{
		    buf = ખાલી;
		    તોડો;
		}
	    જો (buf != ખાલી)
		goto_buffer(eap, DOBUF_FIRST, FORWARD, buf->b_fnum);
	}
#ifdef FEAT_QUICKFIX
	નહીંતર જો (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_size = qf_get_valid_size(eap);
	    જો (qf_size <= 0 || eap->line1 > qf_size)
		buf = ખાલી;
	    નહીંતર
	    {
		save_clear_shm_કિંમત();
		ex_cc(eap);
		restore_shm_કિંમત();

		buf = curbuf;
		i = eap->line1 - 1;
		જો (eap->addr_ગણતરી <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_size;
	    }
	}
#endif
	નહીંતર
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	જ્યારે (!got_int && buf != ખાલી)
	{
	    જો (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		જો (i == ARGCOUNT)
		    તોડો;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		જો (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_કિંમત();
		    do_argfile(eap, i);
		    restore_shm_કિંમત();
		}
		જો (curwin->w_arg_idx != i)
		    તોડો;
	    }
	    નહીંતર જો (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		જો (!win_valid(wp))
		    તોડો;
		win_goto(wp);
		જો (curwin != wp)
		    તોડો;  // something must be wrong
		wp = curwin->w_next;
	    }
	    નહીંતર જો (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		જો (!valid_tabpage(tp))
		    તોડો;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_next;
	    }
	    નહીંતર જો (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		next_fnum = -1;
		માટે (buf = curbuf->b_next; buf != ખાલી; buf = buf->b_next)
		    જો (buf->b_p_bl)
		    {
			next_fnum = buf->b_fnum;
			તોડો;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    જો (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		જો (next_fnum < 0 || next_fnum > eap->line2)
		    તોડો;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    જો (buf->b_fnum == next_fnum)
			તોડો;
		જો (buf == ખાલી)
		    તોડો;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_કિંમત();
		goto_buffer(eap, DOBUF_FIRST, FORWARD, next_fnum);
		restore_shm_કિંમત();

		// If autocommands took us elsewhere, quit here.
		જો (curbuf->b_fnum != next_fnum)
		    તોડો;
	    }

#ifdef FEAT_QUICKFIX
	    જો (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		જો (i >= qf_size || i >= eap->line2)
		    તોડો;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_કિંમત();
		ex_cnext(eap);
		restore_shm_કિંમત();

		// If jumping to the next quickfix entry fails, quit here
		જો (qf_get_cur_idx(eap) == qf_idx)
		    તોડો;
	    }
#endif

	    જો (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		જો (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    જો (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		જો (i+1 > eap->line2)
		    તોડો;
	    જો (eap->cmdidx == CMD_argdo && i >= eap->line2)
		તોડો;
	}
	listcmd_busy = FALSE;
    }

#જો defined(FEAT_SYN_HL)
    જો (save_ei != ખાલી)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	માટે (buf = firstbuf; buf != ખાલી; buf = bnext)
	{
	    bnext = buf->b_next;
	    જો (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_SET))
	    {
		buf->b_flags &= ~BF_SYN_SET;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		જો (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		નહીંતર
		{
		    aucmd_prepbuf(&aco, buf);
		    જો (curbuf == buf)
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
    શૂન્ય
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = ખાલી;
    char_u	*p;

    જો (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	પાછા;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    જો (buf == ખાલી)
	પાછા;

    જો (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    નહીંતર
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_કિંમત((char_u *)"g:current_compiler");
	જો (old_cur_comp != ખાલી)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((અક્ષર *)buf, "compiler/%s.vim", eap->arg);
    જો (source_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_કિંમત((char_u *)"g:current_compiler");
    જો (p != ખાલી)
	set_internal_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    જો (!eap->forceit)
    {
	જો (old_cur_comp != ખાલી)
	{
	    set_internal_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	નહીંતર
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#endif

#જો defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# જો defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    શૂન્ય
init_pyxversion(શૂન્ય)
{
    જો (p_pyx == 0)
    {
	જો (python3_enabled(FALSE))
	    p_pyx = 3;
	નહીંતર જો (python_enabled(FALSE))
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
    સ્થિર પૂર્ણાંક
requires_py_version(char_u *filename)
{
    FILE    *file;
    પૂર્ણાંક	    requires_py_version = 0;
    પૂર્ણાંક	    i, લીટીઓ;

    લીટીઓ = (પૂર્ણાંક)p_mls;
    જો (લીટીઓ < 0)
	લીટીઓ = 5;

    file = mch_fopen((અક્ષર *)filename, "r");
    જો (file == ખાલી)
	પાછા 0;

    માટે (i = 0; i < લીટીઓ; i++)
    {
	જો (vim_fgets(IObuff, IOSIZE, file))
	    તોડો;
	જો (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    જો (strstr((અક્ષર *)IObuff + 2, "python2") != ખાલી)
	    {
		requires_py_version = 2;
		તોડો;
	    }
	    જો (strstr((અક્ષર *)IObuff + 2, "python3") != ખાલી)
	    {
		requires_py_version = 3;
		તોડો;
	    }
	}
	IObuff[21] = '\0';
	જો (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_version = 2;
	    તોડો;
	}
	જો (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_version = 3;
	    તોડો;
	}
    }
    fclose(file);
    પાછા requires_py_version;
}


/*
 * Source a python file using the requested python version.
 */
    સ્થિર શૂન્ય
source_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    પૂર્ણાંક	    v = requires_py_version(fname);

# જો defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    જો (v == 0)
    {
# જો defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    જો (eap == ખાલી)
	CLEAR_FIELD(ex);
    નહીંતર
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    જો (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# નહીંતર
	vim_snprintf((અક્ષર *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	સંદેશ((અક્ષર *)IObuff);
# endif
	પાછા;
    }
    નહીંતર
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# નહીંતર
	vim_snprintf((અક્ષર *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	સંદેશ((અક્ષર *)IObuff);
# endif
	પાછા;
    }
}

/*
 * ":pyxfile {fname}"
 */
    શૂન્ય
ex_pyxfile(exarg_T *eap)
{
    source_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    શૂન્ય
ex_pyx(exarg_T *eap)
{
# જો defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    જો (p_pyx == 2)
	ex_python(eap);
    નહીંતર
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
    શૂન્ય
ex_pyxdo(exarg_T *eap)
{
# જો defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    જો (p_pyx == 2)
	ex_pydo(eap);
    નહીંતર
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
    શૂન્ય
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    પૂર્ણાંક		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    જો (eap->addr_ગણતરી == 0)	// default is all buffers
	check_timestamps(FALSE);
    નહીંતર
    {
	buf = buflist_findnr((પૂર્ણાંક)eap->line2);
	જો (buf != ખાલી)	// cannot happen?
	    (શૂન્ય)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
