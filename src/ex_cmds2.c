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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಙ್ಕ
autowrite(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    ಪೂರ್ಣಾಙ್ಕ		r;
    bufref_T	bufref;

    ಯದಿ (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    set_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    ಯದಿ (bufref_valid(&bufref) && bufIsChanged(buf))
	r = FAIL;
    ಪ್ರತ್ಯಾಗಮ r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    ಶೂನ್ಯ
autowrite_all(ಶೂನ್ಯ)
{
    buf_T	*buf;

    ಯದಿ (!(p_aw || p_awa) || !p_write)
	ಪ್ರತ್ಯಾಗಮ;
    FOR_ALL_BUFFERS(buf)
	ಯದಿ (bufIsChanged(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    (ಶೂನ್ಯ)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    ಯದಿ (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_changed(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಪೂರ್ಣಾಙ್ಕ		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    ಯದಿ (       !forceit
	    && bufIsChanged(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ಯದಿ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ಯದಿ (term_job_running(buf->b_term))
	    {
		ಪ್ರತ್ಯಾಗಮ term_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;

	    ಯದಿ (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    ಯದಿ (bufIsChanged(buf2)
				     && (buf2->b_ffname != ರಿಕ್ತ
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++ಎಣಿಕೆ;
	    ಯದಿ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ಪ್ರತ್ಯಾಗಮ FALSE;

	    dialog_changed(buf, ಎಣಿಕೆ > 1);

	    ಯದಿ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    ಪ್ರತ್ಯಾಗಮ bufIsChanged(buf);
	}
#endif
	ಯದಿ (flags & CCGD_EXCMD)
	    no_write_message();
	ಅನ್ಯಥಾ
	    no_write_message_nobang(curbuf);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# ಯದಿ defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    ಶೂನ್ಯ
browse_save_fname(buf_T *buf)
{
    ಯದಿ (buf->b_fname != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, buf);
    ಯದಿ (fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (setfname(buf, fname, ರಿಕ್ತ, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    ಶೂನ್ಯ
dialog_changed(
    buf_T	*buf,
    ಪೂರ್ಣಾಙ್ಕ		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_ಸಂದೇಶ_SIZE];
    ಪೂರ್ಣಾಙ್ಕ		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_ಸಂದೇಶ(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    ಯದಿ (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, ರಿಕ್ತ, buff, 1);
    ಅನ್ಯಥಾ
	ret = vim_dialog_yesnocancel(VIM_QUESTION, ರಿಕ್ತ, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_FIELD(ea);

    ಯದಿ (ret == VIM_YES)
    {
	ಪೂರ್ಣಾಙ್ಕ	empty_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	ಯದಿ (buf->b_fname == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
# endif
	empty_bufname = buf->b_fname == ರಿಕ್ತ ? TRUE : FALSE;
	ಯದಿ (empty_bufname)
	    buf_set_name(buf->b_fnum, (char_u *)"Untitled");

	ಯದಿ (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    ಯದಿ (buf_write_all(buf, FALSE) == OK)
		ಪ್ರತ್ಯಾಗಮ;
	}

	// restore to empty when write failed or was cancelled
	ಯದಿ (empty_bufname)
	{
	    buf->b_fname = ರಿಕ್ತ;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    ಅನ್ಯಥಾ ಯದಿ (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    ಯದಿ (bufIsChanged(buf2)
		    && (buf2->b_ffname != ರಿಕ್ತ
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
		ಯದಿ (buf2->b_fname != ರಿಕ್ತ && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (ಶೂನ್ಯ)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		ಯದಿ (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (ret == VIM_DISCARDALL)
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
    ಪೂರ್ಣಾಙ್ಕ
can_abandon(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    ಪ್ರತ್ಯಾಗಮ (	   buf_hide(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
add_bufnum(ಪೂರ್ಣಾಙ್ಕ *bufnrs, ಪೂರ್ಣಾಙ್ಕ *bufnump, ಪೂರ್ಣಾಙ್ಕ nr)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; i < *bufnump; ++i)
	ಯದಿ (bufnrs[i] == nr)
	    ಪ್ರತ್ಯಾಗಮ;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    ಪೂರ್ಣಾಙ್ಕ
check_changed_any(
    ಪೂರ್ಣಾಙ್ಕ		hidden,		// Only check hidden buffers
    ಪೂರ್ಣಾಙ್ಕ		unload)
{
    ಪೂರ್ಣಾಙ್ಕ		ret = FALSE;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		save;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		bufnum = 0;
    ಪೂರ್ಣಾಙ್ಕ		bufcount = 0;
    ಪೂರ್ಣಾಙ್ಕ		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    ಯದಿ (bufcount == 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    bufnrs = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, bufcount);
    ಯದಿ (bufnrs == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	ಯದಿ (wp->w_buffer != curbuf)
	    add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	ಯದಿ (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	add_bufnum(bufnrs, &bufnum, buf->b_fnum);

    ಪ್ರತಿ (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	ಯದಿ (buf == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	ಯದಿ ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
	{
	    bufref_T bufref;

	    set_bufref(&bufref, buf);
#ifdef FEAT_TERMINAL
	    ಯದಿ (term_job_running(buf->b_term))
	    {
		ಯದಿ (term_try_stop_job(buf) == FAIL)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ
#endif
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    ಯದಿ (check_changed(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		ಭಙ್ಗ;	    // didn't save - still changes
	}
    }

    ಯದಿ (i >= bufnum)
	ಗಚ್ಛ theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#ಯದಿ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    ಯದಿ (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#endif
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	ಯದಿ (vgetc_busy > 0)
	{
	    ಸಂದೇಶ_row = cmdline_row;
	    ಸಂದೇಶ_col = 0;
	    ಸಂದೇಶ_didout = FALSE;
	}
	ಯದಿ (
#ifdef FEAT_TERMINAL
		term_job_running(buf->b_term)
		    ? semsg(_(e_job_still_running_in_buffer_str), buf->b_fname)
		    :
#endif
		semsg(_(e_no_write_since_last_change_for_buffer_str),
		    buf_spname(buf) != ರಿಕ್ತ ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    ಯದಿ (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಯದಿ (wp->w_buffer == buf)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		ಯದಿ (!bufref_valid(&bufref))
		    ಗಚ್ಛ theend;
		ಗಚ್ಛ buf_found;
	    }
buf_found:

    // Open the changed buffer in the current window.
    ಯದಿ (buf != curbuf)
	set_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    ಪೂರ್ಣಾಙ್ಕ
check_fname(ಶೂನ್ಯ)
{
    ಯದಿ (curbuf->b_ffname == ರಿಕ್ತ)
    {
	emsg(_(e_no_file_name));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಙ್ಕ
buf_write_all(buf_T *buf, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    ಪೂರ್ಣಾಙ್ಕ	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, ರಿಕ್ತ,
						  FALSE, forceit, TRUE, FALSE));
    ಯದಿ (curbuf != old_curbuf)
    {
	ಸಂದೇಶ_source(HL_ATTR(HLF_W));
	ಸಂದೇಶ(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    ಶೂನ್ಯ
ex_listdo(exarg_T *eap)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    ಪೂರ್ಣಾಙ್ಕ		next_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    ಯದಿ (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	ಯದಿ ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_to_buffer));
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	ಯದಿ (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (ಶೂನ್ಯ)win_split(0, 0);

	    ಯದಿ (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}
    }

#ಯದಿ defined(FEAT_SYN_HL)
    char_u	*save_ei = ರಿಕ್ತ;
#endif
#ifdef FEAT_QUICKFIX
    ಪೂರ್ಣಾಙ್ಕ		qf_size = 0;
    ಪೂರ್ಣಾಙ್ಕ		qf_idx;
#endif

#ifndef FEAT_QUICKFIX
    ಯದಿ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

#ಯದಿ defined(FEAT_SYN_HL)
    ಯದಿ (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
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

    ಯದಿ (eap->cmdidx == CMD_windo
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
	ವಿಕಲ್ಪ (eap->cmdidx)
	{
	    ಪಕ್ಷ CMD_windo:
		ಪ್ರತಿ ( ; wp != ರಿಕ್ತ && i + 1 < eap->line1; wp = wp->w_next)
		    i++;
		ಭಙ್ಗ;
	    ಪಕ್ಷ CMD_tabdo:
		ಪ್ರತಿ ( ; tp != ರಿಕ್ತ && i + 1 < eap->line1; tp = tp->tp_next)
		    i++;
		ಭಙ್ಗ;
	    ಪಕ್ಷ CMD_argdo:
		i = eap->line1 - 1;
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
	// set pcmark now
	ಯದಿ (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    ಪ್ರತಿ (buf = firstbuf; buf != ರಿಕ್ತ && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_next)
		ಯದಿ (buf->b_fnum > eap->line2)
		{
		    buf = ರಿಕ್ತ;
		    ಭಙ್ಗ;
		}
	    ಯದಿ (buf != ರಿಕ್ತ)
		goto_buffer(eap, DOBUF_FIRST, FORWARD, buf->b_fnum);
	}
#ifdef FEAT_QUICKFIX
	ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_size = qf_get_valid_size(eap);
	    ಯದಿ (qf_size <= 0 || eap->line1 > qf_size)
		buf = ರಿಕ್ತ;
	    ಅನ್ಯಥಾ
	    {
		save_clear_shm_ಮೌಲ್ಯ();
		ex_cc(eap);
		restore_shm_ಮೌಲ್ಯ();

		buf = curbuf;
		i = eap->line1 - 1;
		ಯದಿ (eap->addr_ಎಣಿಕೆ <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_size;
	    }
	}
#endif
	ಅನ್ಯಥಾ
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	ಯಾವತ್ (!got_int && buf != ರಿಕ್ತ)
	{
	    ಯದಿ (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		ಯದಿ (i == ARGCOUNT)
		    ಭಙ್ಗ;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		ಯದಿ (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_ಮೌಲ್ಯ();
		    do_argfile(eap, i);
		    restore_shm_ಮೌಲ್ಯ();
		}
		ಯದಿ (curwin->w_arg_idx != i)
		    ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		ಯದಿ (!win_valid(wp))
		    ಭಙ್ಗ;
		win_goto(wp);
		ಯದಿ (curwin != wp)
		    ಭಙ್ಗ;  // something must be wrong
		wp = curwin->w_next;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		ಯದಿ (!valid_tabpage(tp))
		    ಭಙ್ಗ;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_next;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		next_fnum = -1;
		ಪ್ರತಿ (buf = curbuf->b_next; buf != ರಿಕ್ತ; buf = buf->b_next)
		    ಯದಿ (buf->b_p_bl)
		    {
			next_fnum = buf->b_fnum;
			ಭಙ್ಗ;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    ಯದಿ (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		ಯದಿ (next_fnum < 0 || next_fnum > eap->line2)
		    ಭಙ್ಗ;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    ಯದಿ (buf->b_fnum == next_fnum)
			ಭಙ್ಗ;
		ಯದಿ (buf == ರಿಕ್ತ)
		    ಭಙ್ಗ;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_ಮೌಲ್ಯ();
		goto_buffer(eap, DOBUF_FIRST, FORWARD, next_fnum);
		restore_shm_ಮೌಲ್ಯ();

		// If autocommands took us elsewhere, quit here.
		ಯದಿ (curbuf->b_fnum != next_fnum)
		    ಭಙ್ಗ;
	    }

#ifdef FEAT_QUICKFIX
	    ಯದಿ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		ಯದಿ (i >= qf_size || i >= eap->line2)
		    ಭಙ್ಗ;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_ಮೌಲ್ಯ();
		ex_cnext(eap);
		restore_shm_ಮೌಲ್ಯ();

		// If jumping to the next quickfix entry fails, quit here
		ಯದಿ (qf_get_cur_idx(eap) == qf_idx)
		    ಭಙ್ಗ;
	    }
#endif

	    ಯದಿ (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		ಯದಿ (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    ಯದಿ (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		ಯದಿ (i+1 > eap->line2)
		    ಭಙ್ಗ;
	    ಯದಿ (eap->cmdidx == CMD_argdo && i >= eap->line2)
		ಭಙ್ಗ;
	}
	listcmd_busy = FALSE;
    }

#ಯದಿ defined(FEAT_SYN_HL)
    ಯದಿ (save_ei != ರಿಕ್ತ)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	ಪ್ರತಿ (buf = firstbuf; buf != ರಿಕ್ತ; buf = bnext)
	{
	    bnext = buf->b_next;
	    ಯದಿ (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_SET))
	    {
		buf->b_flags &= ~BF_SYN_SET;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		ಯದಿ (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		ಅನ್ಯಥಾ
		{
		    aucmd_prepbuf(&aco, buf);
		    ಯದಿ (curbuf == buf)
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
    ಶೂನ್ಯ
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = ರಿಕ್ತ;
    char_u	*p;

    ಯದಿ (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	ಪ್ರತ್ಯಾಗಮ;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    ಅನ್ಯಥಾ
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_ಮೌಲ್ಯ((char_u *)"g:current_compiler");
	ಯದಿ (old_cur_comp != ರಿಕ್ತ)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((ವರ್ಣ *)buf, "compiler/%s.vim", eap->arg);
    ಯದಿ (source_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_ಮೌಲ್ಯ((char_u *)"g:current_compiler");
    ಯದಿ (p != ರಿಕ್ತ)
	set_internal_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    ಯದಿ (!eap->forceit)
    {
	ಯದಿ (old_cur_comp != ರಿಕ್ತ)
	{
	    set_internal_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	ಅನ್ಯಥಾ
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#endif

#ಯದಿ defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# ಯದಿ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    ಶೂನ್ಯ
init_pyxversion(ಶೂನ್ಯ)
{
    ಯದಿ (p_pyx == 0)
    {
	ಯದಿ (python3_enabled(FALSE))
	    p_pyx = 3;
	ಅನ್ಯಥಾ ಯದಿ (python_enabled(FALSE))
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
requires_py_version(char_u *filename)
{
    FILE    *file;
    ಪೂರ್ಣಾಙ್ಕ	    requires_py_version = 0;
    ಪೂರ್ಣಾಙ್ಕ	    i, ಸಾಲುಗಳು;

    ಸಾಲುಗಳು = (ಪೂರ್ಣಾಙ್ಕ)p_mls;
    ಯದಿ (ಸಾಲುಗಳು < 0)
	ಸಾಲುಗಳು = 5;

    file = mch_fopen((ವರ್ಣ *)filename, "r");
    ಯದಿ (file == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತಿ (i = 0; i < ಸಾಲುಗಳು; i++)
    {
	ಯದಿ (vim_fgets(IObuff, IOSIZE, file))
	    ಭಙ್ಗ;
	ಯದಿ (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    ಯದಿ (strstr((ವರ್ಣ *)IObuff + 2, "python2") != ರಿಕ್ತ)
	    {
		requires_py_version = 2;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (strstr((ವರ್ಣ *)IObuff + 2, "python3") != ರಿಕ್ತ)
	    {
		requires_py_version = 3;
		ಭಙ್ಗ;
	    }
	}
	IObuff[21] = '\0';
	ಯದಿ (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_version = 2;
	    ಭಙ್ಗ;
	}
	ಯದಿ (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_version = 3;
	    ಭಙ್ಗ;
	}
    }
    fclose(file);
    ಪ್ರತ್ಯಾಗಮ requires_py_version;
}


/*
 * Source a python file using the requested python version.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
source_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    ಪೂರ್ಣಾಙ್ಕ	    v = requires_py_version(fname);

# ಯದಿ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    ಯದಿ (v == 0)
    {
# ಯದಿ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    ಯದಿ (eap == ರಿಕ್ತ)
	CLEAR_FIELD(ex);
    ಅನ್ಯಥಾ
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    ಯದಿ (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# ಅನ್ಯಥಾ
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	ಸಂದೇಶ((ವರ್ಣ *)IObuff);
# endif
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಅನ್ಯಥಾ
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# ಅನ್ಯಥಾ
	vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	ಸಂದೇಶ((ವರ್ಣ *)IObuff);
# endif
	ಪ್ರತ್ಯಾಗಮ;
    }
}

/*
 * ":pyxfile {fname}"
 */
    ಶೂನ್ಯ
ex_pyxfile(exarg_T *eap)
{
    source_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    ಶೂನ್ಯ
ex_pyx(exarg_T *eap)
{
# ಯದಿ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ಯದಿ (p_pyx == 2)
	ex_python(eap);
    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
ex_pyxdo(exarg_T *eap)
{
# ಯದಿ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ಯದಿ (p_pyx == 2)
	ex_pydo(eap);
    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    ಯದಿ (eap->addr_ಎಣಿಕೆ == 0)	// default is all buffers
	check_timestamps(FALSE);
    ಅನ್ಯಥಾ
    {
	buf = buflist_findnr((ಪೂರ್ಣಾಙ್ಕ)eap->line2);
	ಯದಿ (buf != ರಿಕ್ತ)	// cannot happen?
	    (ಶೂನ್ಯ)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
