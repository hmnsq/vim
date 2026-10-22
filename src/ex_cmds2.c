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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
autowrite(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    bufref_T	bufref;

    ଯଦି (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    set_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    ଯଦି (bufref_valid(&bufref) && bufIsChanged(buf))
	r = FAIL;
    ଫେରନ୍ତୁ r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    ଶୂନ୍ୟ
autowrite_all(ଶୂନ୍ୟ)
{
    buf_T	*buf;

    ଯଦି (!(p_aw || p_awa) || !p_write)
	ଫେରନ୍ତୁ;
    FOR_ALL_BUFFERS(buf)
	ଯଦି (bufIsChanged(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    (ଶୂନ୍ୟ)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    ଯଦି (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_changed(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    ଯଦି (       !forceit
	    && bufIsChanged(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ଯଦି ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ଯଦି (term_job_running(buf->b_term))
	    {
		ଫେରନ୍ତୁ term_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;

	    ଯଦି (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    ଯଦି (bufIsChanged(buf2)
				     && (buf2->b_ffname != ଖାଲି
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++ଗଣନା_2;
	    ଯଦି (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ଫେରନ୍ତୁ FALSE;

	    dialog_changed(buf, ଗଣନା_2 > 1);

	    ଯଦି (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ଫେରନ୍ତୁ FALSE;
	    ଫେରନ୍ତୁ bufIsChanged(buf);
	}
#endif
	ଯଦି (flags & CCGD_EXCMD)
	    no_write_message();
	ଅନ୍ୟଥା
	    no_write_message_nobang(curbuf);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# ଯଦି defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    ଶୂନ୍ୟ
browse_save_fname(buf_T *buf)
{
    ଯଦି (buf->b_fname != ଖାଲି)
	ଫେରନ୍ତୁ;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", ଖାଲି, ଖାଲି, ଖାଲି, buf);
    ଯଦି (fname == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (setfname(buf, fname, ଖାଲି, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    ଶୂନ୍ୟ
dialog_changed(
    buf_T	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_ସନ୍ଦେଶ_SIZE];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_ସନ୍ଦେଶ(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    ଯଦି (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, ଖାଲି, buff, 1);
    ଅନ୍ୟଥା
	ret = vim_dialog_yesnocancel(VIM_QUESTION, ଖାଲି, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_FIELD(ea);

    ଯଦି (ret == VIM_YES)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	empty_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	ଯଦି (buf->b_fname == ଖାଲି)
	    ଫେରନ୍ତୁ;
# endif
	empty_bufname = buf->b_fname == ଖାଲି ? TRUE : FALSE;
	ଯଦି (empty_bufname)
	    buf_set_name(buf->b_fnum, (char_u *)"Untitled");

	ଯଦି (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    ଯଦି (buf_write_all(buf, FALSE) == OK)
		ଫେରନ୍ତୁ;
	}

	// restore to empty when write failed or was cancelled
	ଯଦି (empty_bufname)
	{
	    buf->b_fname = ଖାଲି;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    ଅନ୍ୟଥା ଯଦି (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    ଅନ୍ୟଥା ଯଦି (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    ଯଦି (bufIsChanged(buf2)
		    && (buf2->b_ffname != ଖାଲି
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
		ଯଦି (buf2->b_fname != ଖାଲି && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (ଶୂନ୍ୟ)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		ଯଦି (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (ret == VIM_DISCARDALL)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
can_abandon(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    ଫେରନ୍ତୁ (	   buf_hide(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
add_bufnum(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *bufnrs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *bufnump, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < *bufnump; ++i)
	ଯଦି (bufnrs[i] == nr)
	    ଫେରନ୍ତୁ;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_changed_any(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hidden,		// Only check hidden buffers
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		unload)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FALSE;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufnum = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufcount = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    ଯଦି (bufcount == 0)
	ଫେରନ୍ତୁ FALSE;

    bufnrs = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, bufcount);
    ଯଦି (bufnrs == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	ଯଦି (wp->w_buffer != curbuf)
	    add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	ଯଦି (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	add_bufnum(bufnrs, &bufnum, buf->b_fnum);

    ପାଇଁ (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	ଯଦି (buf == ଖାଲି)
	    ଜାରି;
	ଯଦି ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
	{
	    bufref_T bufref;

	    set_bufref(&bufref, buf);
#ifdef FEAT_TERMINAL
	    ଯଦି (term_job_running(buf->b_term))
	    {
		ଯଦି (term_try_stop_job(buf) == FAIL)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା
#endif
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    ଯଦି (check_changed(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		ଭାଙ୍ଗ;	    // didn't save - still changes
	}
    }

    ଯଦି (i >= bufnum)
	ଯାଅ theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#ଯଦି defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    ଯଦି (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#endif
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	ଯଦି (vgetc_busy > 0)
	{
	    ସନ୍ଦେଶ_row = cmdline_row;
	    ସନ୍ଦେଶ_col = 0;
	    ସନ୍ଦେଶ_didout = FALSE;
	}
	ଯଦି (
#ifdef FEAT_TERMINAL
		term_job_running(buf->b_term)
		    ? semsg(_(e_job_still_running_in_buffer_str), buf->b_fname)
		    :
#endif
		semsg(_(e_no_write_since_last_change_for_buffer_str),
		    buf_spname(buf) != ଖାଲି ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    ଯଦି (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ଯଦି (wp->w_buffer == buf)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		ଯଦି (!bufref_valid(&bufref))
		    ଯାଅ theend;
		ଯାଅ buf_found;
	    }
buf_found:

    // Open the changed buffer in the current window.
    ଯଦି (buf != curbuf)
	set_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    ଫେରନ୍ତୁ ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_fname(ଶୂନ୍ୟ)
{
    ଯଦି (curbuf->b_ffname == ଖାଲି)
    {
	emsg(_(e_no_file_name));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
buf_write_all(buf_T *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା, ଖାଲି,
						  FALSE, forceit, TRUE, FALSE));
    ଯଦି (curbuf != old_curbuf)
    {
	ସନ୍ଦେଶ_source(HL_ATTR(HLF_W));
	ସନ୍ଦେଶ(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    ଫେରନ୍ତୁ retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    ଶୂନ୍ୟ
ex_listdo(exarg_T *eap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		next_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    ଯଦି (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	ଯଦି ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_to_buffer));
	    ଫେରନ୍ତୁ;
	}

	ଯଦି (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	ଯଦି (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (ଶୂନ୍ୟ)win_split(0, 0);

	    ଯଦି (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		ଫେରନ୍ତୁ;
	    }
	}
    }

#ଯଦି defined(FEAT_SYN_HL)
    char_u	*save_ei = ଖାଲି;
#endif
#ifdef FEAT_QUICKFIX
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_size = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		qf_idx;
#endif

#ifndef FEAT_QUICKFIX
    ଯଦି (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	ଫେରନ୍ତୁ;
    }
#endif

#ଯଦି defined(FEAT_SYN_HL)
    ଯଦି (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
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

    ଯଦି (eap->cmdidx == CMD_windo
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
	ବାଛ (eap->cmdidx)
	{
	    ସ୍ଥିତି CMD_windo:
		ପାଇଁ ( ; wp != ଖାଲି && i + 1 < eap->line1; wp = wp->w_next)
		    i++;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_tabdo:
		ପାଇଁ ( ; tp != ଖାଲି && i + 1 < eap->line1; tp = tp->tp_next)
		    i++;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି CMD_argdo:
		i = eap->line1 - 1;
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
	// set pcmark now
	ଯଦି (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    ପାଇଁ (buf = firstbuf; buf != ଖାଲି && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_next)
		ଯଦି (buf->b_fnum > eap->line2)
		{
		    buf = ଖାଲି;
		    ଭାଙ୍ଗ;
		}
	    ଯଦି (buf != ଖାଲି)
		goto_buffer(eap, DOBUF_FIRST, FORWARD, buf->b_fnum);
	}
#ifdef FEAT_QUICKFIX
	ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_size = qf_get_valid_size(eap);
	    ଯଦି (qf_size <= 0 || eap->line1 > qf_size)
		buf = ଖାଲି;
	    ଅନ୍ୟଥା
	    {
		save_clear_shm_ମୂଲ୍ୟ();
		ex_cc(eap);
		restore_shm_ମୂଲ୍ୟ();

		buf = curbuf;
		i = eap->line1 - 1;
		ଯଦି (eap->addr_ଗଣନା <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_size;
	    }
	}
#endif
	ଅନ୍ୟଥା
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	ଯେତେବେଳେ (!got_int && buf != ଖାଲି)
	{
	    ଯଦି (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		ଯଦି (i == ARGCOUNT)
		    ଭାଙ୍ଗ;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		ଯଦି (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_ମୂଲ୍ୟ();
		    do_argfile(eap, i);
		    restore_shm_ମୂଲ୍ୟ();
		}
		ଯଦି (curwin->w_arg_idx != i)
		    ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		ଯଦି (!win_valid(wp))
		    ଭାଙ୍ଗ;
		win_goto(wp);
		ଯଦି (curwin != wp)
		    ଭାଙ୍ଗ;  // something must be wrong
		wp = curwin->w_next;
	    }
	    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		ଯଦି (!valid_tabpage(tp))
		    ଭାଙ୍ଗ;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_next;
	    }
	    ଅନ୍ୟଥା ଯଦି (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		next_fnum = -1;
		ପାଇଁ (buf = curbuf->b_next; buf != ଖାଲି; buf = buf->b_next)
		    ଯଦି (buf->b_p_bl)
		    {
			next_fnum = buf->b_fnum;
			ଭାଙ୍ଗ;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    ଯଦି (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		ଯଦି (next_fnum < 0 || next_fnum > eap->line2)
		    ଭାଙ୍ଗ;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    ଯଦି (buf->b_fnum == next_fnum)
			ଭାଙ୍ଗ;
		ଯଦି (buf == ଖାଲି)
		    ଭାଙ୍ଗ;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_ମୂଲ୍ୟ();
		goto_buffer(eap, DOBUF_FIRST, FORWARD, next_fnum);
		restore_shm_ମୂଲ୍ୟ();

		// If autocommands took us elsewhere, quit here.
		ଯଦି (curbuf->b_fnum != next_fnum)
		    ଭାଙ୍ଗ;
	    }

#ifdef FEAT_QUICKFIX
	    ଯଦି (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		ଯଦି (i >= qf_size || i >= eap->line2)
		    ଭାଙ୍ଗ;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_ମୂଲ୍ୟ();
		ex_cnext(eap);
		restore_shm_ମୂଲ୍ୟ();

		// If jumping to the next quickfix entry fails, quit here
		ଯଦି (qf_get_cur_idx(eap) == qf_idx)
		    ଭାଙ୍ଗ;
	    }
#endif

	    ଯଦି (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		ଯଦି (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    ଯଦି (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		ଯଦି (i+1 > eap->line2)
		    ଭାଙ୍ଗ;
	    ଯଦି (eap->cmdidx == CMD_argdo && i >= eap->line2)
		ଭାଙ୍ଗ;
	}
	listcmd_busy = FALSE;
    }

#ଯଦି defined(FEAT_SYN_HL)
    ଯଦି (save_ei != ଖାଲି)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	ପାଇଁ (buf = firstbuf; buf != ଖାଲି; buf = bnext)
	{
	    bnext = buf->b_next;
	    ଯଦି (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_SET))
	    {
		buf->b_flags &= ~BF_SYN_SET;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		ଯଦି (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		ଅନ୍ୟଥା
		{
		    aucmd_prepbuf(&aco, buf);
		    ଯଦି (curbuf == buf)
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
    ଶୂନ୍ୟ
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = ଖାଲି;
    char_u	*p;

    ଯଦି (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	ଫେରନ୍ତୁ;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    ଅନ୍ୟଥା
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_ମୂଲ୍ୟ((char_u *)"g:current_compiler");
	ଯଦି (old_cur_comp != ଖାଲି)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((ଅକ୍ଷର *)buf, "compiler/%s.vim", eap->arg);
    ଯଦି (source_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_ମୂଲ୍ୟ((char_u *)"g:current_compiler");
    ଯଦି (p != ଖାଲି)
	set_internal_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    ଯଦି (!eap->forceit)
    {
	ଯଦି (old_cur_comp != ଖାଲି)
	{
	    set_internal_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	ଅନ୍ୟଥା
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#endif

#ଯଦି defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# ଯଦି defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    ଶୂନ୍ୟ
init_pyxversion(ଶୂନ୍ୟ)
{
    ଯଦି (p_pyx == 0)
    {
	ଯଦି (python3_enabled(FALSE))
	    p_pyx = 3;
	ଅନ୍ୟଥା ଯଦି (python_enabled(FALSE))
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
requires_py_version(char_u *filename)
{
    FILE    *file;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    requires_py_version = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i, ଧାଡ଼ିଗୁଡ଼ିକ;

    ଧାଡ଼ିଗୁଡ଼ିକ = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_mls;
    ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ < 0)
	ଧାଡ଼ିଗୁଡ଼ିକ = 5;

    file = mch_fopen((ଅକ୍ଷର *)filename, "r");
    ଯଦି (file == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    ପାଇଁ (i = 0; i < ଧାଡ଼ିଗୁଡ଼ିକ; i++)
    {
	ଯଦି (vim_fgets(IObuff, IOSIZE, file))
	    ଭାଙ୍ଗ;
	ଯଦି (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    ଯଦି (strstr((ଅକ୍ଷର *)IObuff + 2, "python2") != ଖାଲି)
	    {
		requires_py_version = 2;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (strstr((ଅକ୍ଷର *)IObuff + 2, "python3") != ଖାଲି)
	    {
		requires_py_version = 3;
		ଭାଙ୍ଗ;
	    }
	}
	IObuff[21] = '\0';
	ଯଦି (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_version = 2;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_version = 3;
	    ଭାଙ୍ଗ;
	}
    }
    fclose(file);
    ଫେରନ୍ତୁ requires_py_version;
}


/*
 * Source a python file using the requested python version.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
source_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    v = requires_py_version(fname);

# ଯଦି defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    ଯଦି (v == 0)
    {
# ଯଦି defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    ଯଦି (eap == ଖାଲି)
	CLEAR_FIELD(ex);
    ଅନ୍ୟଥା
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    ଯଦି (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# ଅନ୍ୟଥା
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	ସନ୍ଦେଶ((ଅକ୍ଷର *)IObuff);
# endif
	ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# ଅନ୍ୟଥା
	vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	ସନ୍ଦେଶ((ଅକ୍ଷର *)IObuff);
# endif
	ଫେରନ୍ତୁ;
    }
}

/*
 * ":pyxfile {fname}"
 */
    ଶୂନ୍ୟ
ex_pyxfile(exarg_T *eap)
{
    source_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    ଶୂନ୍ୟ
ex_pyx(exarg_T *eap)
{
# ଯଦି defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ଯଦି (p_pyx == 2)
	ex_python(eap);
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
ex_pyxdo(exarg_T *eap)
{
# ଯଦି defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ଯଦି (p_pyx == 2)
	ex_pydo(eap);
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    ଯଦି (eap->addr_ଗଣନା == 0)	// default is all buffers
	check_timestamps(FALSE);
    ଅନ୍ୟଥା
    {
	buf = buflist_findnr((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eap->line2);
	ଯଦି (buf != ଖାଲି)	// cannot happen?
	    (ଶୂନ୍ୟ)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
