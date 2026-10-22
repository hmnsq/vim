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

#include "vim.h"
#include "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    ကိန်းပြည့်
autowrite(buf_T *buf, ကိန်းပြည့် forceit)
{
    ကိန်းပြည့်		r;
    bufref_T	bufref;

    အကယ်၍ (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == NULL)
	ပြန်ပေး FAIL;
    set_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    အကယ်၍ (bufref_valid(&bufref) && bufIsChanged(buf))
	r = FAIL;
    ပြန်ပေး r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    void
autowrite_all(void)
{
    buf_T	*buf;

    အကယ်၍ (!(p_aw || p_awa) || !p_write)
	ပြန်ပေး;
    FOR_ALL_BUFFERS(buf)
	အကယ်၍ (bufIsChanged(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    (void)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    အကယ်၍ (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    ကိန်းပြည့်
check_changed(buf_T *buf, ကိန်းပြည့် flags)
{
    ကိန်းပြည့်		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    အကယ်၍ (       !forceit
	    && bufIsChanged(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#အကယ်၍ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	အကယ်၍ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    အကယ်၍ (term_job_running(buf->b_term))
	    {
		ပြန်ပေး term_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    ကိန်းပြည့်		count = 0;

	    အကယ်၍ (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    အကယ်၍ (bufIsChanged(buf2)
				     && (buf2->b_ffname != NULL
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++count;
	    အကယ်၍ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ပြန်ပေး FALSE;

	    dialog_changed(buf, count > 1);

	    အကယ်၍ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ပြန်ပေး FALSE;
	    ပြန်ပေး bufIsChanged(buf);
	}
#endif
	အကယ်၍ (flags & CCGD_EXCMD)
	    no_write_message();
	မဟုတ်ပါက
	    no_write_message_nobang(curbuf);
	ပြန်ပေး TRUE;
    }
    ပြန်ပေး FALSE;
}

#အကယ်၍ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# အကယ်၍ defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    void
browse_save_fname(buf_T *buf)
{
    အကယ်၍ (buf->b_fname != NULL)
	ပြန်ပေး;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", NULL, NULL, NULL, buf);
    အကယ်၍ (fname == NULL)
	ပြန်ပေး;

    အကယ်၍ (setfname(buf, fname, NULL, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    void
dialog_changed(
    buf_T	*buf,
    ကိန်းပြည့်		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_MSG_SIZE];
    ကိန်းပြည့်		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_msg(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    အကယ်၍ (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, NULL, buff, 1);
    မဟုတ်ပါက
	ret = vim_dialog_yesnocancel(VIM_QUESTION, NULL, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_FIELD(ea);

    အကယ်၍ (ret == VIM_YES)
    {
	ကိန်းပြည့်	empty_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	အကယ်၍ (buf->b_fname == NULL)
	    ပြန်ပေး;
# endif
	empty_bufname = buf->b_fname == NULL ? TRUE : FALSE;
	အကယ်၍ (empty_bufname)
	    buf_set_name(buf->b_fnum, (char_u *)"Untitled");

	အကယ်၍ (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    အကယ်၍ (buf_write_all(buf, FALSE) == OK)
		ပြန်ပေး;
	}

	// restore to empty when write failed or was cancelled
	အကယ်၍ (empty_bufname)
	{
	    buf->b_fname = NULL;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    မဟုတ်ပါက အကယ်၍ (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    မဟုတ်ပါက အကယ်၍ (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    အကယ်၍ (bufIsChanged(buf2)
		    && (buf2->b_ffname != NULL
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
		အကယ်၍ (buf2->b_fname != NULL && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (void)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		အကယ်၍ (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    မဟုတ်ပါက အကယ်၍ (ret == VIM_DISCARDALL)
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
    ကိန်းပြည့်
can_abandon(buf_T *buf, ကိန်းပြည့် forceit)
{
    ပြန်ပေး (	   buf_hide(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    static void
add_bufnum(ကိန်းပြည့် *bufnrs, ကိန်းပြည့် *bufnump, ကိန်းပြည့် nr)
{
    ကိန်းပြည့် i;

    အတွက် (i = 0; i < *bufnump; ++i)
	အကယ်၍ (bufnrs[i] == nr)
	    ပြန်ပေး;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    ကိန်းပြည့်
check_changed_any(
    ကိန်းပြည့်		hidden,		// Only check hidden buffers
    ကိန်းပြည့်		unload)
{
    ကိန်းပြည့်		ret = FALSE;
    buf_T	*buf;
    ကိန်းပြည့်		save;
    ကိန်းပြည့်		i;
    ကိန်းပြည့်		bufnum = 0;
    ကိန်းပြည့်		bufcount = 0;
    ကိန်းပြည့်		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    အကယ်၍ (bufcount == 0)
	ပြန်ပေး FALSE;

    bufnrs = ALLOC_MULT(ကိန်းပြည့်, bufcount);
    အကယ်၍ (bufnrs == NULL)
	ပြန်ပေး FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	အကယ်၍ (wp->w_buffer != curbuf)
	    add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	အကယ်၍ (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	add_bufnum(bufnrs, &bufnum, buf->b_fnum);

    အတွက် (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	အကယ်၍ (buf == NULL)
	    continue;
	အကယ်၍ ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
	{
	    bufref_T bufref;

	    set_bufref(&bufref, buf);
#ifdef FEAT_TERMINAL
	    အကယ်၍ (term_job_running(buf->b_term))
	    {
		အကယ်၍ (term_try_stop_job(buf) == FAIL)
		    bha_nga;
	    }
	    မဟုတ်ပါက
#endif
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    အကယ်၍ (check_changed(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		bha_nga;	    // didn't save - still changes
	}
    }

    အကယ်၍ (i >= bufnum)
	goto theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#အကယ်၍ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    အကယ်၍ (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#endif
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	အကယ်၍ (vgetc_busy > 0)
	{
	    msg_row = cmdline_row;
	    msg_col = 0;
	    msg_didout = FALSE;
	}
	အကယ်၍ (
#ifdef FEAT_TERMINAL
		term_job_running(buf->b_term)
		    ? semsg(_(e_job_still_running_in_buffer_str), buf->b_fname)
		    :
#endif
		semsg(_(e_no_write_since_last_change_for_buffer_str),
		    buf_spname(buf) != NULL ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    အကယ်၍ (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    အကယ်၍ (wp->w_buffer == buf)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		အကယ်၍ (!bufref_valid(&bufref))
		    goto theend;
		goto buf_found;
	    }
buf_found:

    // Open the changed buffer in the current window.
    အကယ်၍ (buf != curbuf)
	set_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    ပြန်ပေး ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    ကိန်းပြည့်
check_fname(void)
{
    အကယ်၍ (curbuf->b_ffname == NULL)
    {
	emsg(_(e_no_file_name));
	ပြန်ပေး FAIL;
    }
    ပြန်ပေး OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    ကိန်းပြည့်
buf_write_all(buf_T *buf, ကိန်းပြည့် forceit)
{
    ကိန်းပြည့်	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_line_count, NULL,
						  FALSE, forceit, TRUE, FALSE));
    အကယ်၍ (curbuf != old_curbuf)
    {
	msg_source(HL_ATTR(HLF_W));
	msg(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    ပြန်ပေး retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    void
ex_listdo(exarg_T *eap)
{
    ကိန်းပြည့်		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    ကိန်းပြည့်		next_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    အကယ်၍ (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	အကယ်၍ ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_to_buffer));
	    ပြန်ပေး;
	}

	အကယ်၍ (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	အကယ်၍ (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (void)win_split(0, 0);

	    အကယ်၍ (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		ပြန်ပေး;
	    }
	}
    }

#အကယ်၍ defined(FEAT_SYN_HL)
    char_u	*save_ei = NULL;
#endif
#ifdef FEAT_QUICKFIX
    ကိန်းပြည့်		qf_size = 0;
    ကိန်းပြည့်		qf_idx;
#endif

#ifndef FEAT_QUICKFIX
    အကယ်၍ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	ပြန်ပေး;
    }
#endif

#အကယ်၍ defined(FEAT_SYN_HL)
    အကယ်၍ (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
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

    အကယ်၍ (eap->cmdidx == CMD_windo
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
	switch (eap->cmdidx)
	{
	    pak_sa CMD_windo:
		အတွက် ( ; wp != NULL && i + 1 < eap->line1; wp = wp->w_next)
		    i++;
		bha_nga;
	    pak_sa CMD_tabdo:
		အတွက် ( ; tp != NULL && i + 1 < eap->line1; tp = tp->tp_next)
		    i++;
		bha_nga;
	    pak_sa CMD_argdo:
		i = eap->line1 - 1;
		bha_nga;
	    default:
		bha_nga;
	}
	// set pcmark now
	အကယ်၍ (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    အတွက် (buf = firstbuf; buf != NULL && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_next)
		အကယ်၍ (buf->b_fnum > eap->line2)
		{
		    buf = NULL;
		    bha_nga;
		}
	    အကယ်၍ (buf != NULL)
		goto_buffer(eap, DOBUF_FIRST, FORWARD, buf->b_fnum);
	}
#ifdef FEAT_QUICKFIX
	မဟုတ်ပါက အကယ်၍ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_size = qf_get_valid_size(eap);
	    အကယ်၍ (qf_size <= 0 || eap->line1 > qf_size)
		buf = NULL;
	    မဟုတ်ပါက
	    {
		save_clear_shm_value();
		ex_cc(eap);
		restore_shm_value();

		buf = curbuf;
		i = eap->line1 - 1;
		အကယ်၍ (eap->addr_count <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_size;
	    }
	}
#endif
	မဟုတ်ပါက
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	နေစဉ် (!got_int && buf != NULL)
	{
	    အကယ်၍ (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		အကယ်၍ (i == ARGCOUNT)
		    bha_nga;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		အကယ်၍ (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_value();
		    do_argfile(eap, i);
		    restore_shm_value();
		}
		အကယ်၍ (curwin->w_arg_idx != i)
		    bha_nga;
	    }
	    မဟုတ်ပါက အကယ်၍ (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		အကယ်၍ (!win_valid(wp))
		    bha_nga;
		win_goto(wp);
		အကယ်၍ (curwin != wp)
		    bha_nga;  // something must be wrong
		wp = curwin->w_next;
	    }
	    မဟုတ်ပါက အကယ်၍ (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		အကယ်၍ (!valid_tabpage(tp))
		    bha_nga;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_next;
	    }
	    မဟုတ်ပါက အကယ်၍ (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		next_fnum = -1;
		အတွက် (buf = curbuf->b_next; buf != NULL; buf = buf->b_next)
		    အကယ်၍ (buf->b_p_bl)
		    {
			next_fnum = buf->b_fnum;
			bha_nga;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    အကယ်၍ (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		အကယ်၍ (next_fnum < 0 || next_fnum > eap->line2)
		    bha_nga;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    အကယ်၍ (buf->b_fnum == next_fnum)
			bha_nga;
		အကယ်၍ (buf == NULL)
		    bha_nga;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_value();
		goto_buffer(eap, DOBUF_FIRST, FORWARD, next_fnum);
		restore_shm_value();

		// If autocommands took us elsewhere, quit here.
		အကယ်၍ (curbuf->b_fnum != next_fnum)
		    bha_nga;
	    }

#ifdef FEAT_QUICKFIX
	    အကယ်၍ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		အကယ်၍ (i >= qf_size || i >= eap->line2)
		    bha_nga;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_value();
		ex_cnext(eap);
		restore_shm_value();

		// If jumping to the next quickfix entry fails, quit here
		အကယ်၍ (qf_get_cur_idx(eap) == qf_idx)
		    bha_nga;
	    }
#endif

	    အကယ်၍ (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		အကယ်၍ (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    အကယ်၍ (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		အကယ်၍ (i+1 > eap->line2)
		    bha_nga;
	    အကယ်၍ (eap->cmdidx == CMD_argdo && i >= eap->line2)
		bha_nga;
	}
	listcmd_busy = FALSE;
    }

#အကယ်၍ defined(FEAT_SYN_HL)
    အကယ်၍ (save_ei != NULL)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	အတွက် (buf = firstbuf; buf != NULL; buf = bnext)
	{
	    bnext = buf->b_next;
	    အကယ်၍ (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_SET))
	    {
		buf->b_flags &= ~BF_SYN_SET;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		အကယ်၍ (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		မဟုတ်ပါက
		{
		    aucmd_prepbuf(&aco, buf);
		    အကယ်၍ (curbuf == buf)
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
    void
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = NULL;
    char_u	*p;

    အကယ်၍ (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	ပြန်ပေး;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    အကယ်၍ (buf == NULL)
	ပြန်ပေး;

    အကယ်၍ (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    မဟုတ်ပါက
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_value((char_u *)"g:current_compiler");
	အကယ်၍ (old_cur_comp != NULL)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((var_na *)buf, "compiler/%s.vim", eap->arg);
    အကယ်၍ (source_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_value((char_u *)"g:current_compiler");
    အကယ်၍ (p != NULL)
	set_internal_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    အကယ်၍ (!eap->forceit)
    {
	အကယ်၍ (old_cur_comp != NULL)
	{
	    set_internal_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	မဟုတ်ပါက
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#endif

#အကယ်၍ defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# အကယ်၍ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    void
init_pyxversion(void)
{
    အကယ်၍ (p_pyx == 0)
    {
	အကယ်၍ (python3_enabled(FALSE))
	    p_pyx = 3;
	မဟုတ်ပါက အကယ်၍ (python_enabled(FALSE))
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
    static ကိန်းပြည့်
requires_py_version(char_u *filename)
{
    FILE    *file;
    ကိန်းပြည့်	    requires_py_version = 0;
    ကိန်းပြည့်	    i, lines;

    lines = (ကိန်းပြည့်)p_mls;
    အကယ်၍ (lines < 0)
	lines = 5;

    file = mch_fopen((var_na *)filename, "r");
    အကယ်၍ (file == NULL)
	ပြန်ပေး 0;

    အတွက် (i = 0; i < lines; i++)
    {
	အကယ်၍ (vim_fgets(IObuff, IOSIZE, file))
	    bha_nga;
	အကယ်၍ (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    အကယ်၍ (strstr((var_na *)IObuff + 2, "python2") != NULL)
	    {
		requires_py_version = 2;
		bha_nga;
	    }
	    အကယ်၍ (strstr((var_na *)IObuff + 2, "python3") != NULL)
	    {
		requires_py_version = 3;
		bha_nga;
	    }
	}
	IObuff[21] = '\0';
	အကယ်၍ (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_version = 2;
	    bha_nga;
	}
	အကယ်၍ (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_version = 3;
	    bha_nga;
	}
    }
    fclose(file);
    ပြန်ပေး requires_py_version;
}


/*
 * Source a python file using the requested python version.
 */
    static void
source_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    ကိန်းပြည့်	    v = requires_py_version(fname);

# အကယ်၍ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    အကယ်၍ (v == 0)
    {
# အကယ်၍ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    အကယ်၍ (eap == NULL)
	CLEAR_FIELD(ex);
    မဟုတ်ပါက
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    အကယ်၍ (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# မဟုတ်ပါက
	vim_snprintf((var_na *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	msg((var_na *)IObuff);
# endif
	ပြန်ပေး;
    }
    မဟုတ်ပါက
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# မဟုတ်ပါက
	vim_snprintf((var_na *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	msg((var_na *)IObuff);
# endif
	ပြန်ပေး;
    }
}

/*
 * ":pyxfile {fname}"
 */
    void
ex_pyxfile(exarg_T *eap)
{
    source_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    void
ex_pyx(exarg_T *eap)
{
# အကယ်၍ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    အကယ်၍ (p_pyx == 2)
	ex_python(eap);
    မဟုတ်ပါက
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
    void
ex_pyxdo(exarg_T *eap)
{
# အကယ်၍ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    အကယ်၍ (p_pyx == 2)
	ex_pydo(eap);
    မဟုတ်ပါက
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
    void
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    ကိန်းပြည့်		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    အကယ်၍ (eap->addr_count == 0)	// default is all buffers
	check_timestamps(FALSE);
    မဟုတ်ပါက
    {
	buf = buflist_findnr((ကိန်းပြည့်)eap->line2);
	အကယ်၍ (buf != NULL)	// cannot happen?
	    (void)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
