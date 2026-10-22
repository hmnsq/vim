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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
autowrite(buf_T *buf, పూర్ణాంకం forceit)
{
    పూర్ణాంకం		r;
    bufref_T	bufref;

    ఒకవేళ (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == ఖాళీ)
	తిరిగి FAIL;
    సమితి_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    ఒకవేళ (bufref_valid(&bufref) && buf_Is_మారింది(buf))
	r = FAIL;
    తిరిగి r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    శూన్య
autowrite_all(శూన్య)
{
    buf_T	*buf;

    ఒకవేళ (!(p_aw || p_awa) || !p_write)
	తిరిగి;
    FOR_ALL_BUFFERS(buf)
	ఒకవేళ (buf_Is_మారింది(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    సమితి_bufref(&bufref, buf);

	    (శూన్య)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    ఒకవేళ (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    పూర్ణాంకం
check_మారింది(buf_T *buf, పూర్ణాంకం flags)
{
    పూర్ణాంకం		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    సమితి_bufref(&bufref, buf);

    ఒకవేళ (       !forceit
	    && buf_Is_మారింది(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ఒకవేళ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ఒకవేళ (పదం_పని_running(buf->b_పదం))
	    {
		తిరిగి పదం_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    పూర్ణాంకం		లెక్క = 0;

	    ఒకవేళ (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    ఒకవేళ (buf_Is_మారింది(buf2)
				     && (buf2->b_ffname != ఖాళీ
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++లెక్క;
	    ఒకవేళ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		తిరిగి FALSE;

	    dialog_మారింది(buf, లెక్క > 1);

	    ఒకవేళ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		తిరిగి FALSE;
	    తిరిగి buf_Is_మారింది(buf);
	}
#అయితే_ముగింపు
	ఒకవేళ (flags & CCGD_EXCMD)
	    no_write_సందేశం();
	లేకపోతే
	    no_write_సందేశం_nobang(curbuf);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# ఒకవేళ defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    శూన్య
browse_save_fname(buf_T *buf)
{
    ఒకవేళ (buf->b_fname != ఖాళీ)
	తిరిగి;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", ఖాళీ, ఖాళీ, ఖాళీ, buf);
    ఒకవేళ (fname == ఖాళీ)
	తిరిగి;

    ఒకవేళ (setfname(buf, fname, ఖాళీ, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    శూన్య
dialog_మారింది(
    buf_T	*buf,
    పూర్ణాంకం		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_సందేశం_కొలత];
    పూర్ణాంకం		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_సందేశం(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    ఒకవేళ (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, ఖాళీ, buff, 1);
    లేకపోతే
	ret = vim_dialog_yesnocancel(VIM_QUESTION, ఖాళీ, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_క్షేత్రం(ea);

    ఒకవేళ (ret == VIM_YES)
    {
	పూర్ణాంకం	ఖాళీలు_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	ఒకవేళ (buf->b_fname == ఖాళీ)
	    తిరిగి;
# endif
	ఖాళీలు_bufname = buf->b_fname == ఖాళీ ? TRUE : FALSE;
	ఒకవేళ (ఖాళీలు_bufname)
	    buf_సమితి_name(buf->b_fnum, (char_u *)"Untitled");

	ఒకవేళ (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    ఒకవేళ (buf_write_all(buf, FALSE) == OK)
		తిరిగి;
	}

	// restore to empty when write failed or was cancelled
	ఒకవేళ (ఖాళీలు_bufname)
	{
	    buf->b_fname = ఖాళీ;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    లేకపోతే ఒకవేళ (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    లేకపోతే ఒకవేళ (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    ఒకవేళ (buf_Is_మారింది(buf2)
		    && (buf2->b_ffname != ఖాళీ
# ifdef FEAT_BROWSE
			|| (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
			)
		    && !bt_dontwrite(buf2)
		    && !buf2->b_p_ro)
	    {
		bufref_T bufref;

		సమితి_bufref(&bufref, buf2);
# ifdef FEAT_BROWSE
		// May get file name, when there is none
		browse_save_fname(buf2);
# endif
		ఒకవేళ (buf2->b_fname != ఖాళీ && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (శూన్య)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		ఒకవేళ (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    లేకపోతే ఒకవేళ (ret == VIM_DISCARDALL)
    {
	/*
	 * mark all buffers as unchanged
	 */
	FOR_ALL_BUFFERS(buf2)
	    unchanged(buf2, TRUE, FALSE);
    }
}
#అయితే_ముగింపు

/*
 * Return TRUE if the buffer "buf" can be abandoned, either by making it
 * hidden, autowriting it or unloading it.
 */
    పూర్ణాంకం
can_abandon(buf_T *buf, పూర్ణాంకం forceit)
{
    తిరిగి (	   buf_hide(buf)
		|| !buf_Is_మారింది(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    స్థిర శూన్య
కూడు_bufnum(పూర్ణాంకం *bufnrs, పూర్ణాంకం *bufnump, పూర్ణాంకం nr)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < *bufnump; ++i)
	ఒకవేళ (bufnrs[i] == nr)
	    తిరిగి;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    పూర్ణాంకం
check_మారింది_any(
    పూర్ణాంకం		hidden,		// Only check hidden buffers
    పూర్ణాంకం		unload)
{
    పూర్ణాంకం		ret = FALSE;
    buf_T	*buf;
    పూర్ణాంకం		save;
    పూర్ణాంకం		i;
    పూర్ణాంకం		bufnum = 0;
    పూర్ణాంకం		bufcount = 0;
    పూర్ణాంకం		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    ఒకవేళ (bufcount == 0)
	తిరిగి FALSE;

    bufnrs = ALLOC_MULT(పూర్ణాంకం, bufcount);
    ఒకవేళ (bufnrs == ఖాళీ)
	తిరిగి FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	ఒకవేళ (wp->w_బఫరు != curbuf)
	    కూడు_bufnum(bufnrs, &bufnum, wp->w_బఫరు->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	ఒకవేళ (tp != curtab)
	    FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
		కూడు_bufnum(bufnrs, &bufnum, wp->w_బఫరు->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	కూడు_bufnum(bufnrs, &bufnum, buf->b_fnum);

    కొరకు (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	ఒకవేళ (buf == ఖాళీ)
	    కొనసాగు;
	ఒకవేళ ((!hidden || buf->b_nwindows == 0) && buf_Is_మారింది(buf))
	{
	    bufref_T bufref;

	    సమితి_bufref(&bufref, buf);
#నిర్వచితమైతే FEAT_TERMINAL
	    ఒకవేళ (పదం_పని_running(buf->b_పదం))
	    {
		ఒకవేళ (పదం_try_stop_పని(buf) == FAIL)
		    ఆపు;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    ఒకవేళ (check_మారింది(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		ఆపు;	    // didn't save - still changes
	}
    }

    ఒకవేళ (i >= bufnum)
	వెళ్ళు theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    ఒకవేళ (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#అయితే_ముగింపు
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	ఒకవేళ (vgetc_busy > 0)
	{
	    సందేశం_అడ్డువరుస = cmdline_అడ్డువరుస;
	    సందేశం_నిలువు = 0;
	    సందేశం_didout = FALSE;
	}
	ఒకవేళ (
#నిర్వచితమైతే FEAT_TERMINAL
		పదం_పని_running(buf->b_పదం)
		    ? semsg(_(e_పని_still_running_లోపల_బఫరు_str), buf->b_fname)
		    :
#అయితే_ముగింపు
		semsg(_(e_no_write_since_చివరి_change_for_బఫరు_str),
		    buf_spname(buf) != ఖాళీ ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    ఒకవేళ (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ఒకవేళ (wp->w_బఫరు == buf)
	    {
		bufref_T bufref;

		సమితి_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		ఒకవేళ (!bufref_valid(&bufref))
		    వెళ్ళు theend;
		వెళ్ళు buf_దొరికింది;
	    }
buf_దొరికింది:

    // Open the changed buffer in the current window.
    ఒకవేళ (buf != curbuf)
	సమితి_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    తిరిగి ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    పూర్ణాంకం
check_fname(శూన్య)
{
    ఒకవేళ (curbuf->b_ffname == ఖాళీ)
    {
	emsg(_(e_no_file_name));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
buf_write_all(buf_T *buf, పూర్ణాంకం forceit)
{
    పూర్ణాంకం	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_పంక్తి_లెక్క, ఖాళీ,
						  FALSE, forceit, TRUE, FALSE));
    ఒకవేళ (curbuf != old_curbuf)
    {
	సందేశం_ఆధారం(HL_ATTR(HLF_W));
	సందేశం(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    తిరిగి retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    శూన్య
ex_listdo(exarg_T *eap)
{
    పూర్ణాంకం		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    పూర్ణాంకం		తదుపరి_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    ఒకవేళ (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	ఒకవేళ ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_గమ్యం_బఫరు));
	    తిరిగి;
	}

	ఒకవేళ (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	ఒకవేళ (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (శూన్య)win_విభజించు(0, 0);

	    ఒకవేళ (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_గమ్యం_బఫరు));
		తిరిగి;
	    }
	}
    }

#ఒకవేళ defined(FEAT_SYN_HL)
    char_u	*save_ei = ఖాళీ;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_QUICKFIX
    పూర్ణాంకం		qf_కొలత = 0;
    పూర్ణాంకం		qf_idx;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే FEAT_QUICKFIX
    ఒకవేళ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	తిరిగి;
    }
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SYN_HL)
    ఒకవేళ (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
    {
	// Don't do syntax HL autocommands.  Skipping the syntax file is a
	// great speed improvement.
	save_ei = au_event_disable(",Syntax");

	FOR_ALL_BUFFERS(buf)
	    buf->b_flags &= ~BF_SYN_సమితి;
	buf = curbuf;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD
    ప్రారంభం_global_changes();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#అయితే_ముగింపు

    ఒకవేళ (eap->cmdidx == CMD_windo
	    || eap->cmdidx == CMD_tabdo
	    || buf_hide(curbuf)
	    || !check_మారింది(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	i = 0;
	// start at the eap->line1 argument/window/buffer
	wp = firstwin;
	tp = మొదటి_tabpage;
	ఎంపిక (eap->cmdidx)
	{
	    స్థితి CMD_windo:
		కొరకు ( ; wp != ఖాళీ && i + 1 < eap->line1; wp = wp->w_తదుపరి)
		    i++;
		ఆపు;
	    స్థితి CMD_tabdo:
		కొరకు ( ; tp != ఖాళీ && i + 1 < eap->line1; tp = tp->tp_తదుపరి)
		    i++;
		ఆపు;
	    స్థితి CMD_argdo:
		i = eap->line1 - 1;
		ఆపు;
	    అప్రమేయ:
		ఆపు;
	}
	// set pcmark now
	ఒకవేళ (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    కొరకు (buf = firstbuf; buf != ఖాళీ && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_తదుపరి)
		ఒకవేళ (buf->b_fnum > eap->line2)
		{
		    buf = ఖాళీ;
		    ఆపు;
		}
	    ఒకవేళ (buf != ఖాళీ)
		goto_బఫరు(eap, DOBUF_మొదటి, FORWARD, buf->b_fnum);
	}
#నిర్వచితమైతే FEAT_QUICKFIX
	లేకపోతే ఒకవేళ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_కొలత = qf_get_valid_కొలత(eap);
	    ఒకవేళ (qf_కొలత <= 0 || eap->line1 > qf_కొలత)
		buf = ఖాళీ;
	    లేకపోతే
	    {
		save_clear_shm_విలువ();
		ex_cc(eap);
		restore_shm_విలువ();

		buf = curbuf;
		i = eap->line1 - 1;
		ఒకవేళ (eap->addr_లెక్క <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_కొలత;
	    }
	}
#అయితే_ముగింపు
	లేకపోతే
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	అయితే (!got_int && buf != ఖాళీ)
	{
	    ఒకవేళ (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		ఒకవేళ (i == ARGCOUNT)
		    ఆపు;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		ఒకవేళ (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_విలువ();
		    do_argfile(eap, i);
		    restore_shm_విలువ();
		}
		ఒకవేళ (curwin->w_arg_idx != i)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		ఒకవేళ (!win_valid(wp))
		    ఆపు;
		win_goto(wp);
		ఒకవేళ (curwin != wp)
		    ఆపు;  // something must be wrong
		wp = curwin->w_తదుపరి;
	    }
	    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		ఒకవేళ (!valid_tabpage(tp))
		    ఆపు;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_తదుపరి;
	    }
	    లేకపోతే ఒకవేళ (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		తదుపరి_fnum = -1;
		కొరకు (buf = curbuf->b_తదుపరి; buf != ఖాళీ; buf = buf->b_తదుపరి)
		    ఒకవేళ (buf->b_p_bl)
		    {
			తదుపరి_fnum = buf->b_fnum;
			ఆపు;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    ఒకవేళ (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		ఒకవేళ (తదుపరి_fnum < 0 || తదుపరి_fnum > eap->line2)
		    ఆపు;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    ఒకవేళ (buf->b_fnum == తదుపరి_fnum)
			ఆపు;
		ఒకవేళ (buf == ఖాళీ)
		    ఆపు;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_విలువ();
		goto_బఫరు(eap, DOBUF_మొదటి, FORWARD, తదుపరి_fnum);
		restore_shm_విలువ();

		// If autocommands took us elsewhere, quit here.
		ఒకవేళ (curbuf->b_fnum != తదుపరి_fnum)
		    ఆపు;
	    }

#నిర్వచితమైతే FEAT_QUICKFIX
	    ఒకవేళ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		ఒకవేళ (i >= qf_కొలత || i >= eap->line2)
		    ఆపు;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_విలువ();
		ex_cnext(eap);
		restore_shm_విలువ();

		// If jumping to the next quickfix entry fails, quit here
		ఒకవేళ (qf_get_cur_idx(eap) == qf_idx)
		    ఆపు;
	    }
#అయితే_ముగింపు

	    ఒకవేళ (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		ఒకవేళ (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    ఒకవేళ (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		ఒకవేళ (i+1 > eap->line2)
		    ఆపు;
	    ఒకవేళ (eap->cmdidx == CMD_argdo && i >= eap->line2)
		ఆపు;
	}
	listcmd_busy = FALSE;
    }

#ఒకవేళ defined(FEAT_SYN_HL)
    ఒకవేళ (save_ei != ఖాళీ)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	కొరకు (buf = firstbuf; buf != ఖాళీ; buf = bnext)
	{
	    bnext = buf->b_తదుపరి;
	    ఒకవేళ (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_సమితి))
	    {
		buf->b_flags &= ~BF_SYN_సమితి;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		ఒకవేళ (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		లేకపోతే
		{
		    aucmd_prepbuf(&aco, buf);
		    ఒకవేళ (curbuf == buf)
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
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD
    చివర_global_changes();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#అయితే_ముగింపు
}

#నిర్వచితమైతే FEAT_EVAL
/*
 * ":compiler[!] {name}"
 */
    శూన్య
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = ఖాళీ;
    char_u	*p;

    ఒకవేళ (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	తిరిగి;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    ఒకవేళ (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    లేకపోతే
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_విలువ((char_u *)"g:current_compiler");
	ఒకవేళ (old_cur_comp != ఖాళీ)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((అక్షరం *)buf, "compiler/%s.vim", eap->arg);
    ఒకవేళ (ఆధారం_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_విలువ((char_u *)"g:current_compiler");
    ఒకవేళ (p != ఖాళీ)
	సమితి_అంతర్గత_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    ఒకవేళ (!eap->forceit)
    {
	ఒకవేళ (old_cur_comp != ఖాళీ)
	{
	    సమితి_అంతర్గత_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	లేకపోతే
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# ఒకవేళ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    శూన్య
init_pyxversion(శూన్య)
{
    ఒకవేళ (p_pyx == 0)
    {
	ఒకవేళ (python3_enabled(FALSE))
	    p_pyx = 3;
	లేకపోతే ఒకవేళ (python_enabled(FALSE))
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
    స్థిర పూర్ణాంకం
requires_py_సంస్కరణ(char_u *filename)
{
    FILE    *file;
    పూర్ణాంకం	    requires_py_సంస్కరణ = 0;
    పూర్ణాంకం	    i, పంక్తులు;

    పంక్తులు = (పూర్ణాంకం)p_mls;
    ఒకవేళ (పంక్తులు < 0)
	పంక్తులు = 5;

    file = mch_fopen((అక్షరం *)filename, "r");
    ఒకవేళ (file == ఖాళీ)
	తిరిగి 0;

    కొరకు (i = 0; i < పంక్తులు; i++)
    {
	ఒకవేళ (vim_fgets(IObuff, IOSIZE, file))
	    ఆపు;
	ఒకవేళ (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)IObuff + 2, "python2") != ఖాళీ)
	    {
		requires_py_సంస్కరణ = 2;
		ఆపు;
	    }
	    ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)IObuff + 2, "python3") != ఖాళీ)
	    {
		requires_py_సంస్కరణ = 3;
		ఆపు;
	    }
	}
	IObuff[21] = '\0';
	ఒకవేళ (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_సంస్కరణ = 2;
	    ఆపు;
	}
	ఒకవేళ (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_సంస్కరణ = 3;
	    ఆపు;
	}
    }
    fclose(file);
    తిరిగి requires_py_సంస్కరణ;
}


/*
 * Source a python file using the requested python version.
 */
    స్థిర శూన్య
ఆధారం_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    పూర్ణాంకం	    v = requires_py_సంస్కరణ(fname);

# ఒకవేళ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    ఒకవేళ (v == 0)
    {
# ఒకవేళ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    ఒకవేళ (eap == ఖాళీ)
	CLEAR_క్షేత్రం(ex);
    లేకపోతే
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    ఒకవేళ (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# లేకపోతే
	vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	సందేశం((అక్షరం *)IObuff);
# endif
	తిరిగి;
    }
    లేకపోతే
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# లేకపోతే
	vim_snprintf((అక్షరం *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	సందేశం((అక్షరం *)IObuff);
# endif
	తిరిగి;
    }
}

/*
 * ":pyxfile {fname}"
 */
    శూన్య
ex_pyxfile(exarg_T *eap)
{
    ఆధారం_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    శూన్య
ex_pyx(exarg_T *eap)
{
# ఒకవేళ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ఒకవేళ (p_pyx == 2)
	ex_python(eap);
    లేకపోతే
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
    శూన్య
ex_pyxdo(exarg_T *eap)
{
# ఒకవేళ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ఒకవేళ (p_pyx == 2)
	ex_pydo(eap);
    లేకపోతే
	ex_py3do(eap);
# elif defined(FEAT_PYTHON)
    ex_pydo(eap);
# elif defined(FEAT_PYTHON3)
    ex_py3do(eap);
# endif
}

#అయితే_ముగింపు

/*
 * ":checktime [buffer]"
 */
    శూన్య
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    పూర్ణాంకం		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    ఒకవేళ (eap->addr_లెక్క == 0)	// default is all buffers
	check_timestamps(FALSE);
    లేకపోతే
    {
	buf = buflist_findnr((పూర్ణాంకం)eap->line2);
	ఒకవేళ (buf != ఖాళీ)	// cannot happen?
	    (శూన్య)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
