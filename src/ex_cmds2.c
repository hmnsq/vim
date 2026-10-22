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

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণসংখ্যা
autowrite(buf_T *buf, পূর্ণসংখ্যা forceit)
{
    পূর্ণসংখ্যা		r;
    bufref_T	bufref;

    যদি (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == ফাঁকা)
	ফেরত FAIL;
    set_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    যদি (bufref_valid(&bufref) && bufIsChanged(buf))
	r = FAIL;
    ফেরত r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    শূন্য
autowrite_all(শূন্য)
{
    buf_T	*buf;

    যদি (!(p_aw || p_awa) || !p_write)
	ফেরত;
    FOR_ALL_BUFFERS(buf)
	যদি (bufIsChanged(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    (শূন্য)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    যদি (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    পূর্ণসংখ্যা
check_changed(buf_T *buf, পূর্ণসংখ্যা flags)
{
    পূর্ণসংখ্যা		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    যদি (       !forceit
	    && bufIsChanged(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	যদি ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    যদি (term_job_running(buf->b_term))
	    {
		ফেরত term_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    পূর্ণসংখ্যা		গুনতি = 0;

	    যদি (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    যদি (bufIsChanged(buf2)
				     && (buf2->b_ffname != ফাঁকা
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++গুনতি;
	    যদি (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ফেরত FALSE;

	    dialog_changed(buf, গুনতি > 1);

	    যদি (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ফেরত FALSE;
	    ফেরত bufIsChanged(buf);
	}
#endif
	যদি (flags & CCGD_EXCMD)
	    no_write_message();
	অন্যথা
	    no_write_message_nobang(curbuf);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# যদি defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    শূন্য
browse_save_fname(buf_T *buf)
{
    যদি (buf->b_fname != ফাঁকা)
	ফেরত;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", ফাঁকা, ফাঁকা, ফাঁকা, buf);
    যদি (fname == ফাঁকা)
	ফেরত;

    যদি (setfname(buf, fname, ফাঁকা, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    শূন্য
dialog_changed(
    buf_T	*buf,
    পূর্ণসংখ্যা		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_বার্তা_আকার];
    পূর্ণসংখ্যা		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_বার্তা(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    যদি (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, ফাঁকা, buff, 1);
    অন্যথা
	ret = vim_dialog_yesnocancel(VIM_QUESTION, ফাঁকা, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_FIELD(ea);

    যদি (ret == VIM_YES)
    {
	পূর্ণসংখ্যা	empty_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	যদি (buf->b_fname == ফাঁকা)
	    ফেরত;
# endif
	empty_bufname = buf->b_fname == ফাঁকা ? TRUE : FALSE;
	যদি (empty_bufname)
	    buf_set_name(buf->b_fnum, (char_u *)"Untitled");

	যদি (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    যদি (buf_write_all(buf, FALSE) == OK)
		ফেরত;
	}

	// restore to empty when write failed or was cancelled
	যদি (empty_bufname)
	{
	    buf->b_fname = ফাঁকা;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    অন্যথা যদি (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    অন্যথা যদি (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    যদি (bufIsChanged(buf2)
		    && (buf2->b_ffname != ফাঁকা
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
		যদি (buf2->b_fname != ফাঁকা && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (শূন্য)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		যদি (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    অন্যথা যদি (ret == VIM_DISCARDALL)
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
    পূর্ণসংখ্যা
can_abandon(buf_T *buf, পূর্ণসংখ্যা forceit)
{
    ফেরত (	   buf_hide(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    স্থির শূন্য
add_bufnum(পূর্ণসংখ্যা *bufnrs, পূর্ণসংখ্যা *bufnump, পূর্ণসংখ্যা nr)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; i < *bufnump; ++i)
	যদি (bufnrs[i] == nr)
	    ফেরত;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    পূর্ণসংখ্যা
check_changed_any(
    পূর্ণসংখ্যা		hidden,		// Only check hidden buffers
    পূর্ণসংখ্যা		unload)
{
    পূর্ণসংখ্যা		ret = FALSE;
    buf_T	*buf;
    পূর্ণসংখ্যা		save;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		bufnum = 0;
    পূর্ণসংখ্যা		bufcount = 0;
    পূর্ণসংখ্যা		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    যদি (bufcount == 0)
	ফেরত FALSE;

    bufnrs = ALLOC_MULT(পূর্ণসংখ্যা, bufcount);
    যদি (bufnrs == ফাঁকা)
	ফেরত FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	যদি (wp->w_বাফার != curbuf)
	    add_bufnum(bufnrs, &bufnum, wp->w_বাফার->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	যদি (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		add_bufnum(bufnrs, &bufnum, wp->w_বাফার->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	add_bufnum(bufnrs, &bufnum, buf->b_fnum);

    জন্য (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	যদি (buf == ফাঁকা)
	    চালু;
	যদি ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
	{
	    bufref_T bufref;

	    set_bufref(&bufref, buf);
#ifdef FEAT_TERMINAL
	    যদি (term_job_running(buf->b_term))
	    {
		যদি (term_try_stop_job(buf) == FAIL)
		    বিরতি;
	    }
	    অন্যথা
#endif
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    যদি (check_changed(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		বিরতি;	    // didn't save - still changes
	}
    }

    যদি (i >= bufnum)
	যাও theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#যদি defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    যদি (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#endif
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	যদি (vgetc_busy > 0)
	{
	    বার্তা_row = cmdline_row;
	    বার্তা_col = 0;
	    বার্তা_didout = FALSE;
	}
	যদি (
#ifdef FEAT_TERMINAL
		term_job_running(buf->b_term)
		    ? semsg(_(e_job_still_running_in_বাফার_str), buf->b_fname)
		    :
#endif
		semsg(_(e_no_write_since_last_change_for_বাফার_str),
		    buf_spname(buf) != ফাঁকা ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    যদি (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    যদি (wp->w_বাফার == buf)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		যদি (!bufref_valid(&bufref))
		    যাও theend;
		যাও buf_found;
	    }
buf_found:

    // Open the changed buffer in the current window.
    যদি (buf != curbuf)
	set_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    ফেরত ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    পূর্ণসংখ্যা
check_fname(শূন্য)
{
    যদি (curbuf->b_ffname == ফাঁকা)
    {
	emsg(_(e_no_file_name));
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণসংখ্যা
buf_write_all(buf_T *buf, পূর্ণসংখ্যা forceit)
{
    পূর্ণসংখ্যা	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_সারি_গুনতি, ফাঁকা,
						  FALSE, forceit, TRUE, FALSE));
    যদি (curbuf != old_curbuf)
    {
	বার্তা_source(HL_ATTR(HLF_W));
	বার্তা(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    ফেরত retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    শূন্য
ex_listdo(exarg_T *eap)
{
    পূর্ণসংখ্যা		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    পূর্ণসংখ্যা		পরবর্তী_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    যদি (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	যদি ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_to_বাফার));
	    ফেরত;
	}

	যদি (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	যদি (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (শূন্য)win_split(0, 0);

	    যদি (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_to_বাফার));
		ফেরত;
	    }
	}
    }

#যদি defined(FEAT_SYN_HL)
    char_u	*save_ei = ফাঁকা;
#endif
#ifdef FEAT_QUICKFIX
    পূর্ণসংখ্যা		qf_আকার = 0;
    পূর্ণসংখ্যা		qf_idx;
#endif

#ifndef FEAT_QUICKFIX
    যদি (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	ফেরত;
    }
#endif

#যদি defined(FEAT_SYN_HL)
    যদি (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
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

    যদি (eap->cmdidx == CMD_windo
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
	নির্বাচন (eap->cmdidx)
	{
	    ক্ষেত্র CMD_windo:
		জন্য ( ; wp != ফাঁকা && i + 1 < eap->line1; wp = wp->w_পরবর্তী)
		    i++;
		বিরতি;
	    ক্ষেত্র CMD_tabdo:
		জন্য ( ; tp != ফাঁকা && i + 1 < eap->line1; tp = tp->tp_পরবর্তী)
		    i++;
		বিরতি;
	    ক্ষেত্র CMD_argdo:
		i = eap->line1 - 1;
		বিরতি;
	    পূর্বনির্ধারিত:
		বিরতি;
	}
	// set pcmark now
	যদি (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    জন্য (buf = firstbuf; buf != ফাঁকা && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_পরবর্তী)
		যদি (buf->b_fnum > eap->line2)
		{
		    buf = ফাঁকা;
		    বিরতি;
		}
	    যদি (buf != ফাঁকা)
		goto_বাফার(eap, DOBUF_FIRST, FORWARD, buf->b_fnum);
	}
#ifdef FEAT_QUICKFIX
	অন্যথা যদি (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_আকার = qf_get_valid_আকার(eap);
	    যদি (qf_আকার <= 0 || eap->line1 > qf_আকার)
		buf = ফাঁকা;
	    অন্যথা
	    {
		save_clear_shm_মান();
		ex_cc(eap);
		restore_shm_মান();

		buf = curbuf;
		i = eap->line1 - 1;
		যদি (eap->addr_গুনতি <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_আকার;
	    }
	}
#endif
	অন্যথা
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	যখন (!got_int && buf != ফাঁকা)
	{
	    যদি (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		যদি (i == ARGCOUNT)
		    বিরতি;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		যদি (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_মান();
		    do_argfile(eap, i);
		    restore_shm_মান();
		}
		যদি (curwin->w_arg_idx != i)
		    বিরতি;
	    }
	    অন্যথা যদি (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		যদি (!win_valid(wp))
		    বিরতি;
		win_goto(wp);
		যদি (curwin != wp)
		    বিরতি;  // something must be wrong
		wp = curwin->w_পরবর্তী;
	    }
	    অন্যথা যদি (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		যদি (!valid_tabpage(tp))
		    বিরতি;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_পরবর্তী;
	    }
	    অন্যথা যদি (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		পরবর্তী_fnum = -1;
		জন্য (buf = curbuf->b_পরবর্তী; buf != ফাঁকা; buf = buf->b_পরবর্তী)
		    যদি (buf->b_p_bl)
		    {
			পরবর্তী_fnum = buf->b_fnum;
			বিরতি;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    যদি (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		যদি (পরবর্তী_fnum < 0 || পরবর্তী_fnum > eap->line2)
		    বিরতি;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    যদি (buf->b_fnum == পরবর্তী_fnum)
			বিরতি;
		যদি (buf == ফাঁকা)
		    বিরতি;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_মান();
		goto_বাফার(eap, DOBUF_FIRST, FORWARD, পরবর্তী_fnum);
		restore_shm_মান();

		// If autocommands took us elsewhere, quit here.
		যদি (curbuf->b_fnum != পরবর্তী_fnum)
		    বিরতি;
	    }

#ifdef FEAT_QUICKFIX
	    যদি (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		যদি (i >= qf_আকার || i >= eap->line2)
		    বিরতি;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_মান();
		ex_cnext(eap);
		restore_shm_মান();

		// If jumping to the next quickfix entry fails, quit here
		যদি (qf_get_cur_idx(eap) == qf_idx)
		    বিরতি;
	    }
#endif

	    যদি (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		যদি (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    যদি (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		যদি (i+1 > eap->line2)
		    বিরতি;
	    যদি (eap->cmdidx == CMD_argdo && i >= eap->line2)
		বিরতি;
	}
	listcmd_busy = FALSE;
    }

#যদি defined(FEAT_SYN_HL)
    যদি (save_ei != ফাঁকা)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	জন্য (buf = firstbuf; buf != ফাঁকা; buf = bnext)
	{
	    bnext = buf->b_পরবর্তী;
	    যদি (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_SET))
	    {
		buf->b_flags &= ~BF_SYN_SET;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		যদি (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		অন্যথা
		{
		    aucmd_prepbuf(&aco, buf);
		    যদি (curbuf == buf)
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
    শূন্য
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = ফাঁকা;
    char_u	*p;

    যদি (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	ফেরত;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    যদি (buf == ফাঁকা)
	ফেরত;

    যদি (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    অন্যথা
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_মান((char_u *)"g:current_compiler");
	যদি (old_cur_comp != ফাঁকা)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((অক্ষর *)buf, "compiler/%s.vim", eap->arg);
    যদি (source_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_মান((char_u *)"g:current_compiler");
    যদি (p != ফাঁকা)
	set_internal_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    যদি (!eap->forceit)
    {
	যদি (old_cur_comp != ফাঁকা)
	{
	    set_internal_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	অন্যথা
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#endif

#যদি defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# যদি defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    শূন্য
init_pyxversion(শূন্য)
{
    যদি (p_pyx == 0)
    {
	যদি (python3_enabled(FALSE))
	    p_pyx = 3;
	অন্যথা যদি (python_enabled(FALSE))
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
    স্থির পূর্ণসংখ্যা
requires_py_version(char_u *filename)
{
    FILE    *file;
    পূর্ণসংখ্যা	    requires_py_version = 0;
    পূর্ণসংখ্যা	    i, সারিগুলি;

    সারিগুলি = (পূর্ণসংখ্যা)p_mls;
    যদি (সারিগুলি < 0)
	সারিগুলি = 5;

    file = mch_fopen((অক্ষর *)filename, "r");
    যদি (file == ফাঁকা)
	ফেরত 0;

    জন্য (i = 0; i < সারিগুলি; i++)
    {
	যদি (vim_fgets(IObuff, IOSIZE, file))
	    বিরতি;
	যদি (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    যদি (strstr((অক্ষর *)IObuff + 2, "python2") != ফাঁকা)
	    {
		requires_py_version = 2;
		বিরতি;
	    }
	    যদি (strstr((অক্ষর *)IObuff + 2, "python3") != ফাঁকা)
	    {
		requires_py_version = 3;
		বিরতি;
	    }
	}
	IObuff[21] = '\0';
	যদি (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_version = 2;
	    বিরতি;
	}
	যদি (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_version = 3;
	    বিরতি;
	}
    }
    fclose(file);
    ফেরত requires_py_version;
}


/*
 * Source a python file using the requested python version.
 */
    স্থির শূন্য
source_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    পূর্ণসংখ্যা	    v = requires_py_version(fname);

# যদি defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    যদি (v == 0)
    {
# যদি defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    যদি (eap == ফাঁকা)
	CLEAR_FIELD(ex);
    অন্যথা
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    যদি (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# অন্যথা
	vim_snprintf((অক্ষর *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	বার্তা((অক্ষর *)IObuff);
# endif
	ফেরত;
    }
    অন্যথা
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# অন্যথা
	vim_snprintf((অক্ষর *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	বার্তা((অক্ষর *)IObuff);
# endif
	ফেরত;
    }
}

/*
 * ":pyxfile {fname}"
 */
    শূন্য
ex_pyxfile(exarg_T *eap)
{
    source_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    শূন্য
ex_pyx(exarg_T *eap)
{
# যদি defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    যদি (p_pyx == 2)
	ex_python(eap);
    অন্যথা
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
    শূন্য
ex_pyxdo(exarg_T *eap)
{
# যদি defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    যদি (p_pyx == 2)
	ex_pydo(eap);
    অন্যথা
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
    শূন্য
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    পূর্ণসংখ্যা		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    যদি (eap->addr_গুনতি == 0)	// default is all buffers
	check_timestamps(FALSE);
    অন্যথা
    {
	buf = buflist_findnr((পূর্ণসংখ্যা)eap->line2);
	যদি (buf != ফাঁকা)	// cannot happen?
	    (শূন্য)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
