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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णाङ्क
autowrite(buf_T *buf, पूर्णाङ्क forceit)
{
    पूर्णाङ्क		r;
    bufref_T	bufref;

    सचे (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == रिक्त)
	पटिनिवत्त FAIL;
    set_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    सचे (bufref_valid(&bufref) && bufIsChanged(buf))
	r = FAIL;
    पटिनिवत्त r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    शून्य
autowrite_all(शून्य)
{
    buf_T	*buf;

    सचे (!(p_aw || p_awa) || !p_write)
	पटिनिवत्त;
    FOR_ALL_BUFFERS(buf)
	सचे (bufIsChanged(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    (शून्य)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    सचे (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    पूर्णाङ्क
check_changed(buf_T *buf, पूर्णाङ्क flags)
{
    पूर्णाङ्क		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    सचे (       !forceit
	    && bufIsChanged(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#सचे defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	सचे ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    सचे (term_job_running(buf->b_term))
	    {
		पटिनिवत्त term_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    पूर्णाङ्क		गणनम् = 0;

	    सचे (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    सचे (bufIsChanged(buf2)
				     && (buf2->b_ffname != रिक्त
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++गणनम्;
	    सचे (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		पटिनिवत्त FALSE;

	    dialog_changed(buf, गणनम् > 1);

	    सचे (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		पटिनिवत्त FALSE;
	    पटिनिवत्त bufIsChanged(buf);
	}
#endif
	सचे (flags & CCGD_EXCMD)
	    no_write_message();
	अञ्ञथा
	    no_write_message_nobang(curbuf);
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

#सचे defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# सचे defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    शून्य
browse_save_fname(buf_T *buf)
{
    सचे (buf->b_fname != रिक्त)
	पटिनिवत्त;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", रिक्त, रिक्त, रिक्त, buf);
    सचे (fname == रिक्त)
	पटिनिवत्त;

    सचे (setfname(buf, fname, रिक्त, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    शून्य
dialog_changed(
    buf_T	*buf,
    पूर्णाङ्क		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_सन्देशः_SIZE];
    पूर्णाङ्क		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_सन्देशः(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    सचे (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, रिक्त, buff, 1);
    अञ्ञथा
	ret = vim_dialog_yesnocancel(VIM_QUESTION, रिक्त, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_FIELD(ea);

    सचे (ret == VIM_YES)
    {
	पूर्णाङ्क	empty_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	सचे (buf->b_fname == रिक्त)
	    पटिनिवत्त;
# endif
	empty_bufname = buf->b_fname == रिक्त ? TRUE : FALSE;
	सचे (empty_bufname)
	    buf_set_name(buf->b_fnum, (char_u *)"Untitled");

	सचे (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    सचे (buf_write_all(buf, FALSE) == OK)
		पटिनिवत्त;
	}

	// restore to empty when write failed or was cancelled
	सचे (empty_bufname)
	{
	    buf->b_fname = रिक्त;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    अञ्ञथा सचे (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    अञ्ञथा सचे (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    सचे (bufIsChanged(buf2)
		    && (buf2->b_ffname != रिक्त
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
		सचे (buf2->b_fname != रिक्त && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (शून्य)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		सचे (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    अञ्ञथा सचे (ret == VIM_DISCARDALL)
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
    पूर्णाङ्क
can_abandon(buf_T *buf, पूर्णाङ्क forceit)
{
    पटिनिवत्त (	   buf_hide(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    स्थिर शून्य
add_bufnum(पूर्णाङ्क *bufnrs, पूर्णाङ्क *bufnump, पूर्णाङ्क nr)
{
    पूर्णाङ्क i;

    अत्थाय (i = 0; i < *bufnump; ++i)
	सचे (bufnrs[i] == nr)
	    पटिनिवत्त;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    पूर्णाङ्क
check_changed_any(
    पूर्णाङ्क		hidden,		// Only check hidden buffers
    पूर्णाङ्क		unload)
{
    पूर्णाङ्क		ret = FALSE;
    buf_T	*buf;
    पूर्णाङ्क		save;
    पूर्णाङ्क		i;
    पूर्णाङ्क		bufnum = 0;
    पूर्णाङ्क		bufcount = 0;
    पूर्णाङ्क		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    सचे (bufcount == 0)
	पटिनिवत्त FALSE;

    bufnrs = ALLOC_MULT(पूर्णाङ्क, bufcount);
    सचे (bufnrs == रिक्त)
	पटिनिवत्त FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	सचे (wp->w_buffer != curbuf)
	    add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	सचे (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	add_bufnum(bufnrs, &bufnum, buf->b_fnum);

    अत्थाय (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	सचे (buf == रिक्त)
	    अनुवर्त;
	सचे ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
	{
	    bufref_T bufref;

	    set_bufref(&bufref, buf);
#ifdef FEAT_TERMINAL
	    सचे (term_job_running(buf->b_term))
	    {
		सचे (term_try_stop_job(buf) == FAIL)
		    भङ्ग;
	    }
	    अञ्ञथा
#endif
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    सचे (check_changed(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		भङ्ग;	    // didn't save - still changes
	}
    }

    सचे (i >= bufnum)
	गच्छ theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#सचे defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    सचे (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#endif
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	सचे (vgetc_busy > 0)
	{
	    सन्देशः_row = cmdline_row;
	    सन्देशः_col = 0;
	    सन्देशः_didout = FALSE;
	}
	सचे (
#ifdef FEAT_TERMINAL
		term_job_running(buf->b_term)
		    ? semsg(_(e_job_still_running_in_buffer_str), buf->b_fname)
		    :
#endif
		semsg(_(e_no_write_since_last_change_for_buffer_str),
		    buf_spname(buf) != रिक्त ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    सचे (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    सचे (wp->w_buffer == buf)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		सचे (!bufref_valid(&bufref))
		    गच्छ theend;
		गच्छ buf_found;
	    }
buf_found:

    // Open the changed buffer in the current window.
    सचे (buf != curbuf)
	set_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    पटिनिवत्त ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    पूर्णाङ्क
check_fname(शून्य)
{
    सचे (curbuf->b_ffname == रिक्त)
    {
	emsg(_(e_no_file_name));
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णाङ्क
buf_write_all(buf_T *buf, पूर्णाङ्क forceit)
{
    पूर्णाङ्क	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_पङ्क्तिः_गणनम्, रिक्त,
						  FALSE, forceit, TRUE, FALSE));
    सचे (curbuf != old_curbuf)
    {
	सन्देशः_source(HL_ATTR(HLF_W));
	सन्देशः(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    पटिनिवत्त retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    शून्य
ex_listdo(exarg_T *eap)
{
    पूर्णाङ्क		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    पूर्णाङ्क		next_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    सचे (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	सचे ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_to_buffer));
	    पटिनिवत्त;
	}

	सचे (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	सचे (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (शून्य)win_split(0, 0);

	    सचे (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		पटिनिवत्त;
	    }
	}
    }

#सचे defined(FEAT_SYN_HL)
    char_u	*save_ei = रिक्त;
#endif
#ifdef FEAT_QUICKFIX
    पूर्णाङ्क		qf_size = 0;
    पूर्णाङ्क		qf_idx;
#endif

#ifndef FEAT_QUICKFIX
    सचे (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	पटिनिवत्त;
    }
#endif

#सचे defined(FEAT_SYN_HL)
    सचे (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
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

    सचे (eap->cmdidx == CMD_windo
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
	विकल्प (eap->cmdidx)
	{
	    पक्ष CMD_windo:
		अत्थाय ( ; wp != रिक्त && i + 1 < eap->line1; wp = wp->w_next)
		    i++;
		भङ्ग;
	    पक्ष CMD_tabdo:
		अत्थाय ( ; tp != रिक्त && i + 1 < eap->line1; tp = tp->tp_next)
		    i++;
		भङ्ग;
	    पक्ष CMD_argdo:
		i = eap->line1 - 1;
		भङ्ग;
	    स्वभाव:
		भङ्ग;
	}
	// set pcmark now
	सचे (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    अत्थाय (buf = firstbuf; buf != रिक्त && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_next)
		सचे (buf->b_fnum > eap->line2)
		{
		    buf = रिक्त;
		    भङ्ग;
		}
	    सचे (buf != रिक्त)
		goto_buffer(eap, DOBUF_FIRST, FORWARD, buf->b_fnum);
	}
#ifdef FEAT_QUICKFIX
	अञ्ञथा सचे (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_size = qf_get_valid_size(eap);
	    सचे (qf_size <= 0 || eap->line1 > qf_size)
		buf = रिक्त;
	    अञ्ञथा
	    {
		save_clear_shm_मानम्();
		ex_cc(eap);
		restore_shm_मानम्();

		buf = curbuf;
		i = eap->line1 - 1;
		सचे (eap->addr_गणनम् <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_size;
	    }
	}
#endif
	अञ्ञथा
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	याव (!got_int && buf != रिक्त)
	{
	    सचे (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		सचे (i == ARGCOUNT)
		    भङ्ग;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		सचे (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_मानम्();
		    do_argfile(eap, i);
		    restore_shm_मानम्();
		}
		सचे (curwin->w_arg_idx != i)
		    भङ्ग;
	    }
	    अञ्ञथा सचे (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		सचे (!win_valid(wp))
		    भङ्ग;
		win_goto(wp);
		सचे (curwin != wp)
		    भङ्ग;  // something must be wrong
		wp = curwin->w_next;
	    }
	    अञ्ञथा सचे (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		सचे (!valid_tabpage(tp))
		    भङ्ग;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_next;
	    }
	    अञ्ञथा सचे (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		next_fnum = -1;
		अत्थाय (buf = curbuf->b_next; buf != रिक्त; buf = buf->b_next)
		    सचे (buf->b_p_bl)
		    {
			next_fnum = buf->b_fnum;
			भङ्ग;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    सचे (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		सचे (next_fnum < 0 || next_fnum > eap->line2)
		    भङ्ग;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    सचे (buf->b_fnum == next_fnum)
			भङ्ग;
		सचे (buf == रिक्त)
		    भङ्ग;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_मानम्();
		goto_buffer(eap, DOBUF_FIRST, FORWARD, next_fnum);
		restore_shm_मानम्();

		// If autocommands took us elsewhere, quit here.
		सचे (curbuf->b_fnum != next_fnum)
		    भङ्ग;
	    }

#ifdef FEAT_QUICKFIX
	    सचे (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		सचे (i >= qf_size || i >= eap->line2)
		    भङ्ग;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_मानम्();
		ex_cnext(eap);
		restore_shm_मानम्();

		// If jumping to the next quickfix entry fails, quit here
		सचे (qf_get_cur_idx(eap) == qf_idx)
		    भङ्ग;
	    }
#endif

	    सचे (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		सचे (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    सचे (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		सचे (i+1 > eap->line2)
		    भङ्ग;
	    सचे (eap->cmdidx == CMD_argdo && i >= eap->line2)
		भङ्ग;
	}
	listcmd_busy = FALSE;
    }

#सचे defined(FEAT_SYN_HL)
    सचे (save_ei != रिक्त)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	अत्थाय (buf = firstbuf; buf != रिक्त; buf = bnext)
	{
	    bnext = buf->b_next;
	    सचे (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_SET))
	    {
		buf->b_flags &= ~BF_SYN_SET;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		सचे (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		अञ्ञथा
		{
		    aucmd_prepbuf(&aco, buf);
		    सचे (curbuf == buf)
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
    शून्य
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = रिक्त;
    char_u	*p;

    सचे (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	पटिनिवत्त;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    सचे (buf == रिक्त)
	पटिनिवत्त;

    सचे (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    अञ्ञथा
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_मानम्((char_u *)"g:current_compiler");
	सचे (old_cur_comp != रिक्त)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((वर्ण *)buf, "compiler/%s.vim", eap->arg);
    सचे (source_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_मानम्((char_u *)"g:current_compiler");
    सचे (p != रिक्त)
	set_internal_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    सचे (!eap->forceit)
    {
	सचे (old_cur_comp != रिक्त)
	{
	    set_internal_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	अञ्ञथा
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#endif

#सचे defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# सचे defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    शून्य
init_pyxversion(शून्य)
{
    सचे (p_pyx == 0)
    {
	सचे (python3_enabled(FALSE))
	    p_pyx = 3;
	अञ्ञथा सचे (python_enabled(FALSE))
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
    स्थिर पूर्णाङ्क
requires_py_version(char_u *filename)
{
    FILE    *file;
    पूर्णाङ्क	    requires_py_version = 0;
    पूर्णाङ्क	    i, पङ्क्तयः;

    पङ्क्तयः = (पूर्णाङ्क)p_mls;
    सचे (पङ्क्तयः < 0)
	पङ्क्तयः = 5;

    file = mch_fopen((वर्ण *)filename, "r");
    सचे (file == रिक्त)
	पटिनिवत्त 0;

    अत्थाय (i = 0; i < पङ्क्तयः; i++)
    {
	सचे (vim_fgets(IObuff, IOSIZE, file))
	    भङ्ग;
	सचे (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    सचे (strstr((वर्ण *)IObuff + 2, "python2") != रिक्त)
	    {
		requires_py_version = 2;
		भङ्ग;
	    }
	    सचे (strstr((वर्ण *)IObuff + 2, "python3") != रिक्त)
	    {
		requires_py_version = 3;
		भङ्ग;
	    }
	}
	IObuff[21] = '\0';
	सचे (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_version = 2;
	    भङ्ग;
	}
	सचे (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_version = 3;
	    भङ्ग;
	}
    }
    fclose(file);
    पटिनिवत्त requires_py_version;
}


/*
 * Source a python file using the requested python version.
 */
    स्थिर शून्य
source_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    पूर्णाङ्क	    v = requires_py_version(fname);

# सचे defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    सचे (v == 0)
    {
# सचे defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    सचे (eap == रिक्त)
	CLEAR_FIELD(ex);
    अञ्ञथा
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    सचे (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# अञ्ञथा
	vim_snprintf((वर्ण *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	सन्देशः((वर्ण *)IObuff);
# endif
	पटिनिवत्त;
    }
    अञ्ञथा
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# अञ्ञथा
	vim_snprintf((वर्ण *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	सन्देशः((वर्ण *)IObuff);
# endif
	पटिनिवत्त;
    }
}

/*
 * ":pyxfile {fname}"
 */
    शून्य
ex_pyxfile(exarg_T *eap)
{
    source_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    शून्य
ex_pyx(exarg_T *eap)
{
# सचे defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    सचे (p_pyx == 2)
	ex_python(eap);
    अञ्ञथा
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
    शून्य
ex_pyxdo(exarg_T *eap)
{
# सचे defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    सचे (p_pyx == 2)
	ex_pydo(eap);
    अञ्ञथा
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
    शून्य
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    पूर्णाङ्क		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    सचे (eap->addr_गणनम् == 0)	// default is all buffers
	check_timestamps(FALSE);
    अञ्ञथा
    {
	buf = buflist_findnr((पूर्णाङ्क)eap->line2);
	सचे (buf != रिक्त)	// cannot happen?
	    (शून्य)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
