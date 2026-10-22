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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
autowrite(buf_T *buf, ਪੂਰਨ forceit)
{
    ਪੂਰਨ		r;
    bufref_T	bufref;

    ਜੇ (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਸੈੱਟ_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    ਜੇ (bufref_valid(&bufref) && buf_Is_ਬਦਲਿਆ(buf))
	r = FAIL;
    ਵਾਪਸ r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    ਖਾਲੀ
autowrite_all(ਖਾਲੀ)
{
    buf_T	*buf;

    ਜੇ (!(p_aw || p_awa) || !p_write)
	ਵਾਪਸ;
    FOR_ALL_BUFFERS(buf)
	ਜੇ (buf_Is_ਬਦਲਿਆ(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    ਸੈੱਟ_bufref(&bufref, buf);

	    (ਖਾਲੀ)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    ਜੇ (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    ਪੂਰਨ
check_ਬਦਲਿਆ(buf_T *buf, ਪੂਰਨ flags)
{
    ਪੂਰਨ		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    ਸੈੱਟ_bufref(&bufref, buf);

    ਜੇ (       !forceit
	    && buf_Is_ਬਦਲਿਆ(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ਜੇ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ਜੇ (ਪਦ_ਕੰਮ_running(buf->b_ਪਦ))
	    {
		ਵਾਪਸ ਪਦ_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;

	    ਜੇ (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    ਜੇ (buf_Is_ਬਦਲਿਆ(buf2)
				     && (buf2->b_ffname != ਨਿਰਾ
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++ਗਿਣਤੀ_2;
	    ਜੇ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ਵਾਪਸ FALSE;

	    dialog_ਬਦਲਿਆ(buf, ਗਿਣਤੀ_2 > 1);

	    ਜੇ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ਵਾਪਸ FALSE;
	    ਵਾਪਸ buf_Is_ਬਦਲਿਆ(buf);
	}
#ਜੇ_ਅੰਤ
	ਜੇ (flags & CCGD_EXCMD)
	    no_write_ਸੁਨੇਹਾ();
	ਨਹੀਂਤਾਂ
	    no_write_ਸੁਨੇਹਾ_nobang(curbuf);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# ਜੇ defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    ਖਾਲੀ
browse_save_fname(buf_T *buf)
{
    ਜੇ (buf->b_fname != ਨਿਰਾ)
	ਵਾਪਸ;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, buf);
    ਜੇ (fname == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (setfname(buf, fname, ਨਿਰਾ, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    ਖਾਲੀ
dialog_ਬਦਲਿਆ(
    buf_T	*buf,
    ਪੂਰਨ		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_ਸੁਨੇਹਾ_ਮਾਪ];
    ਪੂਰਨ		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_ਸੁਨੇਹਾ(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    ਜੇ (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, ਨਿਰਾ, buff, 1);
    ਨਹੀਂਤਾਂ
	ret = vim_dialog_yesnocancel(VIM_QUESTION, ਨਿਰਾ, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_ਖੇਤਰ(ea);

    ਜੇ (ret == VIM_YES)
    {
	ਪੂਰਨ	ਖਾਲੀ_ਥਾਵਾਂ_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	ਜੇ (buf->b_fname == ਨਿਰਾ)
	    ਵਾਪਸ;
# endif
	ਖਾਲੀ_ਥਾਵਾਂ_bufname = buf->b_fname == ਨਿਰਾ ? TRUE : FALSE;
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_bufname)
	    buf_ਸੈੱਟ_name(buf->b_fnum, (char_u *)"Untitled");

	ਜੇ (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    ਜੇ (buf_write_all(buf, FALSE) == OK)
		ਵਾਪਸ;
	}

	// restore to empty when write failed or was cancelled
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_bufname)
	{
	    buf->b_fname = ਨਿਰਾ;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    ਜੇ (buf_Is_ਬਦਲਿਆ(buf2)
		    && (buf2->b_ffname != ਨਿਰਾ
# ifdef FEAT_BROWSE
			|| (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
			)
		    && !bt_dontwrite(buf2)
		    && !buf2->b_p_ro)
	    {
		bufref_T bufref;

		ਸੈੱਟ_bufref(&bufref, buf2);
# ifdef FEAT_BROWSE
		// May get file name, when there is none
		browse_save_fname(buf2);
# endif
		ਜੇ (buf2->b_fname != ਨਿਰਾ && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (ਖਾਲੀ)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		ਜੇ (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ret == VIM_DISCARDALL)
    {
	/*
	 * mark all buffers as unchanged
	 */
	FOR_ALL_BUFFERS(buf2)
	    unchanged(buf2, TRUE, FALSE);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE if the buffer "buf" can be abandoned, either by making it
 * hidden, autowriting it or unloading it.
 */
    ਪੂਰਨ
can_abandon(buf_T *buf, ਪੂਰਨ forceit)
{
    ਵਾਪਸ (	   buf_hide(buf)
		|| !buf_Is_ਬਦਲਿਆ(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_bufnum(ਪੂਰਨ *bufnrs, ਪੂਰਨ *bufnump, ਪੂਰਨ nr)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < *bufnump; ++i)
	ਜੇ (bufnrs[i] == nr)
	    ਵਾਪਸ;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    ਪੂਰਨ
check_ਬਦਲਿਆ_any(
    ਪੂਰਨ		hidden,		// Only check hidden buffers
    ਪੂਰਨ		unload)
{
    ਪੂਰਨ		ret = FALSE;
    buf_T	*buf;
    ਪੂਰਨ		save;
    ਪੂਰਨ		i;
    ਪੂਰਨ		bufnum = 0;
    ਪੂਰਨ		bufcount = 0;
    ਪੂਰਨ		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    ਜੇ (bufcount == 0)
	ਵਾਪਸ FALSE;

    bufnrs = ALLOC_MULT(ਪੂਰਨ, bufcount);
    ਜੇ (bufnrs == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	ਜੇ (wp->w_ਬਫਰ != curbuf)
	    ਜੋੜੋ_bufnum(bufnrs, &bufnum, wp->w_ਬਫਰ->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	ਜੇ (tp != curtab)
	    FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
		ਜੋੜੋ_bufnum(bufnrs, &bufnum, wp->w_ਬਫਰ->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	ਜੋੜੋ_bufnum(bufnrs, &bufnum, buf->b_fnum);

    ਲਈ (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	ਜੇ (buf == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਜੇ ((!hidden || buf->b_nwindows == 0) && buf_Is_ਬਦਲਿਆ(buf))
	{
	    bufref_T bufref;

	    ਸੈੱਟ_bufref(&bufref, buf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	    ਜੇ (ਪਦ_ਕੰਮ_running(buf->b_ਪਦ))
	    {
		ਜੇ (ਪਦ_try_stop_ਕੰਮ(buf) == FAIL)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    ਜੇ (check_ਬਦਲਿਆ(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		ਰੋਕੋ;	    // didn't save - still changes
	}
    }

    ਜੇ (i >= bufnum)
	ਜਾਓ theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    ਜੇ (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#ਜੇ_ਅੰਤ
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	ਜੇ (vgetc_busy > 0)
	{
	    ਸੁਨੇਹਾ_ਪੰਕਤੀ = cmdline_ਪੰਕਤੀ;
	    ਸੁਨੇਹਾ_ਖਾਨਾ = 0;
	    ਸੁਨੇਹਾ_didout = FALSE;
	}
	ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		ਪਦ_ਕੰਮ_running(buf->b_ਪਦ)
		    ? semsg(_(e_ਕੰਮ_still_running_ਵਿੱਚ_ਬਫਰ_str), buf->b_fname)
		    :
#ਜੇ_ਅੰਤ
		semsg(_(e_no_write_since_ਆਖਰੀ_change_for_ਬਫਰ_str),
		    buf_spname(buf) != ਨਿਰਾ ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    ਜੇ (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ਜੇ (wp->w_ਬਫਰ == buf)
	    {
		bufref_T bufref;

		ਸੈੱਟ_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		ਜੇ (!bufref_valid(&bufref))
		    ਜਾਓ theend;
		ਜਾਓ buf_ਮਿਲਿਆ;
	    }
buf_ਮਿਲਿਆ:

    // Open the changed buffer in the current window.
    ਜੇ (buf != curbuf)
	ਸੈੱਟ_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    ਵਾਪਸ ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    ਪੂਰਨ
check_fname(ਖਾਲੀ)
{
    ਜੇ (curbuf->b_ffname == ਨਿਰਾ)
    {
	emsg(_(e_no_file_name));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
buf_write_all(buf_T *buf, ਪੂਰਨ forceit)
{
    ਪੂਰਨ	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ, ਨਿਰਾ,
						  FALSE, forceit, TRUE, FALSE));
    ਜੇ (curbuf != old_curbuf)
    {
	ਸੁਨੇਹਾ_ਸਰੋਤ(HL_ATTR(HLF_W));
	ਸੁਨੇਹਾ(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    ਵਾਪਸ retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    ਖਾਲੀ
ex_listdo(exarg_T *eap)
{
    ਪੂਰਨ		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    ਪੂਰਨ		ਅਗਲਾ_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    ਜੇ (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	ਜੇ ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_ਮੰਜ਼ਿਲ_ਬਫਰ));
	    ਵਾਪਸ;
	}

	ਜੇ (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	ਜੇ (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (ਖਾਲੀ)win_ਵੰਡੋ(0, 0);

	    ਜੇ (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_ਮੰਜ਼ਿਲ_ਬਫਰ));
		ਵਾਪਸ;
	    }
	}
    }

#ਜੇ defined(FEAT_SYN_HL)
    char_u	*save_ei = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    ਪੂਰਨ		qf_ਮਾਪ = 0;
    ਪੂਰਨ		qf_idx;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    ਜੇ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SYN_HL)
    ਜੇ (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
    {
	// Don't do syntax HL autocommands.  Skipping the syntax file is a
	// great speed improvement.
	save_ei = au_event_disable(",Syntax");

	FOR_ALL_BUFFERS(buf)
	    buf->b_flags &= ~BF_SYN_ਸੈੱਟ;
	buf = curbuf;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਸ਼ੁਰੂ_global_changes();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#ਜੇ_ਅੰਤ

    ਜੇ (eap->cmdidx == CMD_windo
	    || eap->cmdidx == CMD_tabdo
	    || buf_hide(curbuf)
	    || !check_ਬਦਲਿਆ(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	i = 0;
	// start at the eap->line1 argument/window/buffer
	wp = firstwin;
	tp = ਪਹਿਲਾ_tabpage;
	ਚੋਣ (eap->cmdidx)
	{
	    ਹਾਲਤ CMD_windo:
		ਲਈ ( ; wp != ਨਿਰਾ && i + 1 < eap->line1; wp = wp->w_ਅਗਲਾ)
		    i++;
		ਰੋਕੋ;
	    ਹਾਲਤ CMD_tabdo:
		ਲਈ ( ; tp != ਨਿਰਾ && i + 1 < eap->line1; tp = tp->tp_ਅਗਲਾ)
		    i++;
		ਰੋਕੋ;
	    ਹਾਲਤ CMD_argdo:
		i = eap->line1 - 1;
		ਰੋਕੋ;
	    ਮੂਲ:
		ਰੋਕੋ;
	}
	// set pcmark now
	ਜੇ (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    ਲਈ (buf = firstbuf; buf != ਨਿਰਾ && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_ਅਗਲਾ)
		ਜੇ (buf->b_fnum > eap->line2)
		{
		    buf = ਨਿਰਾ;
		    ਰੋਕੋ;
		}
	    ਜੇ (buf != ਨਿਰਾ)
		goto_ਬਫਰ(eap, DOBUF_ਪਹਿਲਾ, FORWARD, buf->b_fnum);
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_ਮਾਪ = qf_get_valid_ਮਾਪ(eap);
	    ਜੇ (qf_ਮਾਪ <= 0 || eap->line1 > qf_ਮਾਪ)
		buf = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		save_clear_shm_ਮੁੱਲ();
		ex_cc(eap);
		restore_shm_ਮੁੱਲ();

		buf = curbuf;
		i = eap->line1 - 1;
		ਜੇ (eap->addr_ਗਿਣਤੀ <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_ਮਾਪ;
	    }
	}
#ਜੇ_ਅੰਤ
	ਨਹੀਂਤਾਂ
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	ਜਦੋਂ (!got_int && buf != ਨਿਰਾ)
	{
	    ਜੇ (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		ਜੇ (i == ARGCOUNT)
		    ਰੋਕੋ;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		ਜੇ (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_ਮੁੱਲ();
		    do_argfile(eap, i);
		    restore_shm_ਮੁੱਲ();
		}
		ਜੇ (curwin->w_arg_idx != i)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		ਜੇ (!win_valid(wp))
		    ਰੋਕੋ;
		win_goto(wp);
		ਜੇ (curwin != wp)
		    ਰੋਕੋ;  // something must be wrong
		wp = curwin->w_ਅਗਲਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		ਜੇ (!valid_tabpage(tp))
		    ਰੋਕੋ;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_ਅਗਲਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		ਅਗਲਾ_fnum = -1;
		ਲਈ (buf = curbuf->b_ਅਗਲਾ; buf != ਨਿਰਾ; buf = buf->b_ਅਗਲਾ)
		    ਜੇ (buf->b_p_bl)
		    {
			ਅਗਲਾ_fnum = buf->b_fnum;
			ਰੋਕੋ;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    ਜੇ (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		ਜੇ (ਅਗਲਾ_fnum < 0 || ਅਗਲਾ_fnum > eap->line2)
		    ਰੋਕੋ;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    ਜੇ (buf->b_fnum == ਅਗਲਾ_fnum)
			ਰੋਕੋ;
		ਜੇ (buf == ਨਿਰਾ)
		    ਰੋਕੋ;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_ਮੁੱਲ();
		goto_ਬਫਰ(eap, DOBUF_ਪਹਿਲਾ, FORWARD, ਅਗਲਾ_fnum);
		restore_shm_ਮੁੱਲ();

		// If autocommands took us elsewhere, quit here.
		ਜੇ (curbuf->b_fnum != ਅਗਲਾ_fnum)
		    ਰੋਕੋ;
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
	    ਜੇ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		ਜੇ (i >= qf_ਮਾਪ || i >= eap->line2)
		    ਰੋਕੋ;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_ਮੁੱਲ();
		ex_cnext(eap);
		restore_shm_ਮੁੱਲ();

		// If jumping to the next quickfix entry fails, quit here
		ਜੇ (qf_get_cur_idx(eap) == qf_idx)
		    ਰੋਕੋ;
	    }
#ਜੇ_ਅੰਤ

	    ਜੇ (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		ਜੇ (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    ਜੇ (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		ਜੇ (i+1 > eap->line2)
		    ਰੋਕੋ;
	    ਜੇ (eap->cmdidx == CMD_argdo && i >= eap->line2)
		ਰੋਕੋ;
	}
	listcmd_busy = FALSE;
    }

#ਜੇ defined(FEAT_SYN_HL)
    ਜੇ (save_ei != ਨਿਰਾ)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	ਲਈ (buf = firstbuf; buf != ਨਿਰਾ; buf = bnext)
	{
	    bnext = buf->b_ਅਗਲਾ;
	    ਜੇ (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_ਸੈੱਟ))
	    {
		buf->b_flags &= ~BF_SYN_ਸੈੱਟ;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		ਜੇ (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		ਨਹੀਂਤਾਂ
		{
		    aucmd_prepbuf(&aco, buf);
		    ਜੇ (curbuf == buf)
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
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD
    ਸਿਰਾ_global_changes();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
/*
 * ":compiler[!] {name}"
 */
    ਖਾਲੀ
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = ਨਿਰਾ;
    char_u	*p;

    ਜੇ (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	ਵਾਪਸ;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    ਨਹੀਂਤਾਂ
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_ਮੁੱਲ((char_u *)"g:current_compiler");
	ਜੇ (old_cur_comp != ਨਿਰਾ)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((ਅੱਖਰ *)buf, "compiler/%s.vim", eap->arg);
    ਜੇ (ਸਰੋਤ_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_ਮੁੱਲ((char_u *)"g:current_compiler");
    ਜੇ (p != ਨਿਰਾ)
	ਸੈੱਟ_ਅੰਦਰੂਨੀ_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    ਜੇ (!eap->forceit)
    {
	ਜੇ (old_cur_comp != ਨਿਰਾ)
	{
	    ਸੈੱਟ_ਅੰਦਰੂਨੀ_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	ਨਹੀਂਤਾਂ
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# ਜੇ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    ਖਾਲੀ
init_pyxversion(ਖਾਲੀ)
{
    ਜੇ (p_pyx == 0)
    {
	ਜੇ (python3_enabled(FALSE))
	    p_pyx = 3;
	ਨਹੀਂਤਾਂ ਜੇ (python_enabled(FALSE))
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
    ਸਥਿਰ ਪੂਰਨ
requires_py_ਸੰਸਕਰਣ(char_u *filename)
{
    FILE    *file;
    ਪੂਰਨ	    requires_py_ਸੰਸਕਰਣ = 0;
    ਪੂਰਨ	    i, ਸਤਰਾਂ;

    ਸਤਰਾਂ = (ਪੂਰਨ)p_mls;
    ਜੇ (ਸਤਰਾਂ < 0)
	ਸਤਰਾਂ = 5;

    file = mch_fopen((ਅੱਖਰ *)filename, "r");
    ਜੇ (file == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਲਈ (i = 0; i < ਸਤਰਾਂ; i++)
    {
	ਜੇ (vim_fgets(IObuff, IOSIZE, file))
	    ਰੋਕੋ;
	ਜੇ (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)IObuff + 2, "python2") != ਨਿਰਾ)
	    {
		requires_py_ਸੰਸਕਰਣ = 2;
		ਰੋਕੋ;
	    }
	    ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)IObuff + 2, "python3") != ਨਿਰਾ)
	    {
		requires_py_ਸੰਸਕਰਣ = 3;
		ਰੋਕੋ;
	    }
	}
	IObuff[21] = '\0';
	ਜੇ (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_ਸੰਸਕਰਣ = 2;
	    ਰੋਕੋ;
	}
	ਜੇ (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_ਸੰਸਕਰਣ = 3;
	    ਰੋਕੋ;
	}
    }
    fclose(file);
    ਵਾਪਸ requires_py_ਸੰਸਕਰਣ;
}


/*
 * Source a python file using the requested python version.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸਰੋਤ_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    ਪੂਰਨ	    v = requires_py_ਸੰਸਕਰਣ(fname);

# ਜੇ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    ਜੇ (v == 0)
    {
# ਜੇ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    ਜੇ (eap == ਨਿਰਾ)
	CLEAR_ਖੇਤਰ(ex);
    ਨਹੀਂਤਾਂ
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    ਜੇ (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# ਨਹੀਂਤਾਂ
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	ਸੁਨੇਹਾ((ਅੱਖਰ *)IObuff);
# endif
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# ਨਹੀਂਤਾਂ
	vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	ਸੁਨੇਹਾ((ਅੱਖਰ *)IObuff);
# endif
	ਵਾਪਸ;
    }
}

/*
 * ":pyxfile {fname}"
 */
    ਖਾਲੀ
ex_pyxfile(exarg_T *eap)
{
    ਸਰੋਤ_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    ਖਾਲੀ
ex_pyx(exarg_T *eap)
{
# ਜੇ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ਜੇ (p_pyx == 2)
	ex_python(eap);
    ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
ex_pyxdo(exarg_T *eap)
{
# ਜੇ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ਜੇ (p_pyx == 2)
	ex_pydo(eap);
    ਨਹੀਂਤਾਂ
	ex_py3do(eap);
# elif defined(FEAT_PYTHON)
    ex_pydo(eap);
# elif defined(FEAT_PYTHON3)
    ex_py3do(eap);
# endif
}

#ਜੇ_ਅੰਤ

/*
 * ":checktime [buffer]"
 */
    ਖਾਲੀ
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    ਪੂਰਨ		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    ਜੇ (eap->addr_ਗਿਣਤੀ == 0)	// default is all buffers
	check_timestamps(FALSE);
    ਨਹੀਂਤਾਂ
    {
	buf = buflist_findnr((ਪੂਰਨ)eap->line2);
	ਜੇ (buf != ਨਿਰਾ)	// cannot happen?
	    (ਖਾਲੀ)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
