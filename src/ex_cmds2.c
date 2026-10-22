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
    ಪೂರ್ಣಾಂಕ
autowrite(buf_T *buf, ಪೂರ್ಣಾಂಕ forceit)
{
    ಪೂರ್ಣಾಂಕ		r;
    bufref_T	bufref;

    ಆದರೆ (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಗಣ_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    ಆದರೆ (bufref_valid(&bufref) && buf_Is_ಬದಲಾಯಿತು(buf))
	r = FAIL;
    ಹಿಂತಿರುಗಿ r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    ಶೂನ್ಯ
autowrite_all(ಶೂನ್ಯ)
{
    buf_T	*buf;

    ಆದರೆ (!(p_aw || p_awa) || !p_write)
	ಹಿಂತಿರುಗಿ;
    FOR_ALL_BUFFERS(buf)
	ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    ಗಣ_bufref(&bufref, buf);

	    (ಶೂನ್ಯ)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    ಆದರೆ (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    ಪೂರ್ಣಾಂಕ
check_ಬದಲಾಯಿತು(buf_T *buf, ಪೂರ್ಣಾಂಕ flags)
{
    ಪೂರ್ಣಾಂಕ		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    ಗಣ_bufref(&bufref, buf);

    ಆದರೆ (       !forceit
	    && buf_Is_ಬದಲಾಯಿತು(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	ಆದರೆ ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    ಆದರೆ (ಪದ_ಕೆಲಸ_running(buf->b_ಪದ))
	    {
		ಹಿಂತಿರುಗಿ ಪದ_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;

	    ಆದರೆ (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(buf2)
				     && (buf2->b_ffname != ಖಾಲಿ
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++ಎಣಿಕೆ_2;
	    ಆದರೆ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ಹಿಂತಿರುಗಿ FALSE;

	    dialog_ಬದಲಾಯಿತು(buf, ಎಣಿಕೆ_2 > 1);

	    ಆದರೆ (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		ಹಿಂತಿರುಗಿ FALSE;
	    ಹಿಂತಿರುಗಿ buf_Is_ಬದಲಾಯಿತು(buf);
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (flags & CCGD_EXCMD)
	    no_write_ಸಂದೇಶ();
	ಇಲ್ಲದಿದ್ದರೆ
	    no_write_ಸಂದೇಶ_nobang(curbuf);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# ಆದರೆ defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    ಶೂನ್ಯ
browse_save_fname(buf_T *buf)
{
    ಆದರೆ (buf->b_fname != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, buf);
    ಆದರೆ (fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (setfname(buf, fname, ಖಾಲಿ, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    ಶೂನ್ಯ
dialog_ಬದಲಾಯಿತು(
    buf_T	*buf,
    ಪೂರ್ಣಾಂಕ		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_ಸಂದೇಶ_ಗಾತ್ರ];
    ಪೂರ್ಣಾಂಕ		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_ಸಂದೇಶ(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    ಆದರೆ (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, ಖಾಲಿ, buff, 1);
    ಇಲ್ಲದಿದ್ದರೆ
	ret = vim_dialog_yesnocancel(VIM_QUESTION, ಖಾಲಿ, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_ಕ್ಷೇತ್ರ(ea);

    ಆದರೆ (ret == VIM_YES)
    {
	ಪೂರ್ಣಾಂಕ	ಖಾಲಿಗಳು_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	ಆದರೆ (buf->b_fname == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
# endif
	ಖಾಲಿಗಳು_bufname = buf->b_fname == ಖಾಲಿ ? TRUE : FALSE;
	ಆದರೆ (ಖಾಲಿಗಳು_bufname)
	    buf_ಗಣ_name(buf->b_fnum, (char_u *)"Untitled");

	ಆದರೆ (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    ಆದರೆ (buf_write_all(buf, FALSE) == OK)
		ಹಿಂತಿರುಗಿ;
	}

	// restore to empty when write failed or was cancelled
	ಆದರೆ (ಖಾಲಿಗಳು_bufname)
	{
	    buf->b_fname = ಖಾಲಿ;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    ಆದರೆ (buf_Is_ಬದಲಾಯಿತು(buf2)
		    && (buf2->b_ffname != ಖಾಲಿ
# ifdef FEAT_BROWSE
			|| (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
			)
		    && !bt_dontwrite(buf2)
		    && !buf2->b_p_ro)
	    {
		bufref_T bufref;

		ಗಣ_bufref(&bufref, buf2);
# ifdef FEAT_BROWSE
		// May get file name, when there is none
		browse_save_fname(buf2);
# endif
		ಆದರೆ (buf2->b_fname != ಖಾಲಿ && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (ಶೂನ್ಯ)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		ಆದರೆ (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret == VIM_DISCARDALL)
    {
	/*
	 * mark all buffers as unchanged
	 */
	FOR_ALL_BUFFERS(buf2)
	    unchanged(buf2, TRUE, FALSE);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE if the buffer "buf" can be abandoned, either by making it
 * hidden, autowriting it or unloading it.
 */
    ಪೂರ್ಣಾಂಕ
can_abandon(buf_T *buf, ಪೂರ್ಣಾಂಕ forceit)
{
    ಹಿಂತಿರುಗಿ (	   buf_hide(buf)
		|| !buf_Is_ಬದಲಾಯಿತು(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_bufnum(ಪೂರ್ಣಾಂಕ *bufnrs, ಪೂರ್ಣಾಂಕ *bufnump, ಪೂರ್ಣಾಂಕ nr)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < *bufnump; ++i)
	ಆದರೆ (bufnrs[i] == nr)
	    ಹಿಂತಿರುಗಿ;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    ಪೂರ್ಣಾಂಕ
check_ಬದಲಾಯಿತು_any(
    ಪೂರ್ಣಾಂಕ		hidden,		// Only check hidden buffers
    ಪೂರ್ಣಾಂಕ		unload)
{
    ಪೂರ್ಣಾಂಕ		ret = FALSE;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		save;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		bufnum = 0;
    ಪೂರ್ಣಾಂಕ		bufcount = 0;
    ಪೂರ್ಣಾಂಕ		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    ಆದರೆ (bufcount == 0)
	ಹಿಂತಿರುಗಿ FALSE;

    bufnrs = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, bufcount);
    ಆದರೆ (bufnrs == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	ಆದರೆ (wp->w_ಬಫರ್ != curbuf)
	    ಕೂಡಿಸು_bufnum(bufnrs, &bufnum, wp->w_ಬಫರ್->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	ಆದರೆ (tp != curtab)
	    FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
		ಕೂಡಿಸು_bufnum(bufnrs, &bufnum, wp->w_ಬಫರ್->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	ಕೂಡಿಸು_bufnum(bufnrs, &bufnum, buf->b_fnum);

    ಗಾಗಿ (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಆದರೆ ((!hidden || buf->b_nwindows == 0) && buf_Is_ಬದಲಾಯಿತು(buf))
	{
	    bufref_T bufref;

	    ಗಣ_bufref(&bufref, buf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	    ಆದರೆ (ಪದ_ಕೆಲಸ_running(buf->b_ಪದ))
	    {
		ಆದರೆ (ಪದ_try_stop_ಕೆಲಸ(buf) == FAIL)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    ಆದರೆ (check_ಬದಲಾಯಿತು(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		ನಿಲ್ಲಿಸು;	    // didn't save - still changes
	}
    }

    ಆದರೆ (i >= bufnum)
	ಹೋಗು theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    ಆದರೆ (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#ಆದರೆ_ಅಂತ್ಯ
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	ಆದರೆ (vgetc_busy > 0)
	{
	    ಸಂದೇಶ_ಅಡ್ಡಸಾಲು = cmdline_ಅಡ್ಡಸಾಲು;
	    ಸಂದೇಶ_ಲಂಬಸಾಲು = 0;
	    ಸಂದೇಶ_didout = FALSE;
	}
	ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		ಪದ_ಕೆಲಸ_running(buf->b_ಪದ)
		    ? semsg(_(e_ಕೆಲಸ_still_running_ಒಳ_ಬಫರ್_str), buf->b_fname)
		    :
#ಆದರೆ_ಅಂತ್ಯ
		semsg(_(e_no_write_since_ಕೊನೆಯ_change_for_ಬಫರ್_str),
		    buf_spname(buf) != ಖಾಲಿ ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    ಆದರೆ (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಆದರೆ (wp->w_ಬಫರ್ == buf)
	    {
		bufref_T bufref;

		ಗಣ_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		ಆದರೆ (!bufref_valid(&bufref))
		    ಹೋಗು theend;
		ಹೋಗು buf_ಸಿಕ್ಕಿತು;
	    }
buf_ಸಿಕ್ಕಿತು:

    // Open the changed buffer in the current window.
    ಆದರೆ (buf != curbuf)
	ಗಣ_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    ಪೂರ್ಣಾಂಕ
check_fname(ಶೂನ್ಯ)
{
    ಆದರೆ (curbuf->b_ffname == ಖಾಲಿ)
    {
	emsg(_(e_no_file_name));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
buf_write_all(buf_T *buf, ಪೂರ್ಣಾಂಕ forceit)
{
    ಪೂರ್ಣಾಂಕ	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ, ಖಾಲಿ,
						  FALSE, forceit, TRUE, FALSE));
    ಆದರೆ (curbuf != old_curbuf)
    {
	ಸಂದೇಶ_ಮೂಲ(HL_ATTR(HLF_W));
	ಸಂದೇಶ(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    ಶೂನ್ಯ
ex_listdo(exarg_T *eap)
{
    ಪೂರ್ಣಾಂಕ		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    ಪೂರ್ಣಾಂಕ		ಮುಂದಿನ_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    ಆದರೆ (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	ಆದರೆ ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_ಗಮ್ಯ_ಬಫರ್));
	    ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	ಆದರೆ (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (ಶೂನ್ಯ)win_ವಿಭಜಿಸು(0, 0);

	    ಆದರೆ (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_ಗಮ್ಯ_ಬಫರ್));
		ಹಿಂತಿರುಗಿ;
	    }
	}
    }

#ಆದರೆ defined(FEAT_SYN_HL)
    char_u	*save_ei = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    ಪೂರ್ಣಾಂಕ		qf_ಗಾತ್ರ = 0;
    ಪೂರ್ಣಾಂಕ		qf_idx;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_QUICKFIX
    ಆದರೆ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SYN_HL)
    ಆದರೆ (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
    {
	// Don't do syntax HL autocommands.  Skipping the syntax file is a
	// great speed improvement.
	save_ei = au_event_disable(",Syntax");

	FOR_ALL_BUFFERS(buf)
	    buf->b_flags &= ~BF_SYN_ಗಣ;
	buf = curbuf;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಆರಂಭ_global_changes();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (eap->cmdidx == CMD_windo
	    || eap->cmdidx == CMD_tabdo
	    || buf_hide(curbuf)
	    || !check_ಬದಲಾಯಿತು(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	i = 0;
	// start at the eap->line1 argument/window/buffer
	wp = firstwin;
	tp = ಮೊದಲ_tabpage;
	ಆಯ್ಕೆ (eap->cmdidx)
	{
	    ಸ್ಥಿತಿ CMD_windo:
		ಗಾಗಿ ( ; wp != ಖಾಲಿ && i + 1 < eap->line1; wp = wp->w_ಮುಂದಿನ)
		    i++;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_tabdo:
		ಗಾಗಿ ( ; tp != ಖಾಲಿ && i + 1 < eap->line1; tp = tp->tp_ಮುಂದಿನ)
		    i++;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_argdo:
		i = eap->line1 - 1;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
	// set pcmark now
	ಆದರೆ (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    ಗಾಗಿ (buf = firstbuf; buf != ಖಾಲಿ && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_ಮುಂದಿನ)
		ಆದರೆ (buf->b_fnum > eap->line2)
		{
		    buf = ಖಾಲಿ;
		    ನಿಲ್ಲಿಸು;
		}
	    ಆದರೆ (buf != ಖಾಲಿ)
		goto_ಬಫರ್(eap, DOBUF_ಮೊದಲ, FORWARD, buf->b_fnum);
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_ಗಾತ್ರ = qf_get_valid_ಗಾತ್ರ(eap);
	    ಆದರೆ (qf_ಗಾತ್ರ <= 0 || eap->line1 > qf_ಗಾತ್ರ)
		buf = ಖಾಲಿ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		save_clear_shm_ಮೌಲ್ಯ();
		ex_cc(eap);
		restore_shm_ಮೌಲ್ಯ();

		buf = curbuf;
		i = eap->line1 - 1;
		ಆದರೆ (eap->addr_ಎಣಿಕೆ <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_ಗಾತ್ರ;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಇಲ್ಲದಿದ್ದರೆ
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	ಆಗ (!got_int && buf != ಖಾಲಿ)
	{
	    ಆದರೆ (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		ಆದರೆ (i == ARGCOUNT)
		    ನಿಲ್ಲಿಸು;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		ಆದರೆ (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_ಮೌಲ್ಯ();
		    do_argfile(eap, i);
		    restore_shm_ಮೌಲ್ಯ();
		}
		ಆದರೆ (curwin->w_arg_idx != i)
		    ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		ಆದರೆ (!win_valid(wp))
		    ನಿಲ್ಲಿಸು;
		win_goto(wp);
		ಆದರೆ (curwin != wp)
		    ನಿಲ್ಲಿಸು;  // something must be wrong
		wp = curwin->w_ಮುಂದಿನ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		ಆದರೆ (!valid_tabpage(tp))
		    ನಿಲ್ಲಿಸು;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_ಮುಂದಿನ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		ಮುಂದಿನ_fnum = -1;
		ಗಾಗಿ (buf = curbuf->b_ಮುಂದಿನ; buf != ಖಾಲಿ; buf = buf->b_ಮುಂದಿನ)
		    ಆದರೆ (buf->b_p_bl)
		    {
			ಮುಂದಿನ_fnum = buf->b_fnum;
			ನಿಲ್ಲಿಸು;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    ಆದರೆ (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		ಆದರೆ (ಮುಂದಿನ_fnum < 0 || ಮುಂದಿನ_fnum > eap->line2)
		    ನಿಲ್ಲಿಸು;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    ಆದರೆ (buf->b_fnum == ಮುಂದಿನ_fnum)
			ನಿಲ್ಲಿಸು;
		ಆದರೆ (buf == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_ಮೌಲ್ಯ();
		goto_ಬಫರ್(eap, DOBUF_ಮೊದಲ, FORWARD, ಮುಂದಿನ_fnum);
		restore_shm_ಮೌಲ್ಯ();

		// If autocommands took us elsewhere, quit here.
		ಆದರೆ (curbuf->b_fnum != ಮುಂದಿನ_fnum)
		    ನಿಲ್ಲಿಸು;
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
	    ಆದರೆ (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		ಆದರೆ (i >= qf_ಗಾತ್ರ || i >= eap->line2)
		    ನಿಲ್ಲಿಸು;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_ಮೌಲ್ಯ();
		ex_cnext(eap);
		restore_shm_ಮೌಲ್ಯ();

		// If jumping to the next quickfix entry fails, quit here
		ಆದರೆ (qf_get_cur_idx(eap) == qf_idx)
		    ನಿಲ್ಲಿಸು;
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		ಆದರೆ (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    ಆದರೆ (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		ಆದರೆ (i+1 > eap->line2)
		    ನಿಲ್ಲಿಸು;
	    ಆದರೆ (eap->cmdidx == CMD_argdo && i >= eap->line2)
		ನಿಲ್ಲಿಸು;
	}
	listcmd_busy = FALSE;
    }

#ಆದರೆ defined(FEAT_SYN_HL)
    ಆದರೆ (save_ei != ಖಾಲಿ)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	ಗಾಗಿ (buf = firstbuf; buf != ಖಾಲಿ; buf = bnext)
	{
	    bnext = buf->b_ಮುಂದಿನ;
	    ಆದರೆ (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_ಗಣ))
	    {
		buf->b_flags &= ~BF_SYN_ಗಣ;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		ಆದರೆ (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    aucmd_prepbuf(&aco, buf);
		    ಆದರೆ (curbuf == buf)
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
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD
    ಕೊನೆ_global_changes();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
/*
 * ":compiler[!] {name}"
 */
    ಶೂನ್ಯ
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = ಖಾಲಿ;
    char_u	*p;

    ಆದರೆ (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	ಹಿಂತಿರುಗಿ;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_ಮೌಲ್ಯ((char_u *)"g:current_compiler");
	ಆದರೆ (old_cur_comp != ಖಾಲಿ)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((ಅಕ್ಷರ *)buf, "compiler/%s.vim", eap->arg);
    ಆದರೆ (ಮೂಲ_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_ಮೌಲ್ಯ((char_u *)"g:current_compiler");
    ಆದರೆ (p != ಖಾಲಿ)
	ಗಣ_ಆಂತರಿಕ_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    ಆದರೆ (!eap->forceit)
    {
	ಆದರೆ (old_cur_comp != ಖಾಲಿ)
	{
	    ಗಣ_ಆಂತರಿಕ_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# ಆದರೆ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    ಶೂನ್ಯ
init_pyxversion(ಶೂನ್ಯ)
{
    ಆದರೆ (p_pyx == 0)
    {
	ಆದರೆ (python3_enabled(FALSE))
	    p_pyx = 3;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (python_enabled(FALSE))
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
requires_py_ಆವೃತ್ತಿ(char_u *filename)
{
    FILE    *file;
    ಪೂರ್ಣಾಂಕ	    requires_py_ಆವೃತ್ತಿ = 0;
    ಪೂರ್ಣಾಂಕ	    i, ಸಾಲುಗಳು;

    ಸಾಲುಗಳು = (ಪೂರ್ಣಾಂಕ)p_mls;
    ಆದರೆ (ಸಾಲುಗಳು < 0)
	ಸಾಲುಗಳು = 5;

    file = mch_fopen((ಅಕ್ಷರ *)filename, "r");
    ಆದರೆ (file == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಗಾಗಿ (i = 0; i < ಸಾಲುಗಳು; i++)
    {
	ಆದರೆ (vim_fgets(IObuff, IOSIZE, file))
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)IObuff + 2, "python2") != ಖಾಲಿ)
	    {
		requires_py_ಆವೃತ್ತಿ = 2;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)IObuff + 2, "python3") != ಖಾಲಿ)
	    {
		requires_py_ಆವೃತ್ತಿ = 3;
		ನಿಲ್ಲಿಸು;
	    }
	}
	IObuff[21] = '\0';
	ಆದರೆ (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_ಆವೃತ್ತಿ = 2;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_ಆವೃತ್ತಿ = 3;
	    ನಿಲ್ಲಿಸು;
	}
    }
    fclose(file);
    ಹಿಂತಿರುಗಿ requires_py_ಆವೃತ್ತಿ;
}


/*
 * Source a python file using the requested python version.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಮೂಲ_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    ಪೂರ್ಣಾಂಕ	    v = requires_py_ಆವೃತ್ತಿ(fname);

# ಆದರೆ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    ಆದರೆ (v == 0)
    {
# ಆದರೆ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    ಆದರೆ (eap == ಖಾಲಿ)
	CLEAR_ಕ್ಷೇತ್ರ(ex);
    ಇಲ್ಲದಿದ್ದರೆ
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    ಆದರೆ (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# ಇಲ್ಲದಿದ್ದರೆ
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	ಸಂದೇಶ((ಅಕ್ಷರ *)IObuff);
# endif
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# ಇಲ್ಲದಿದ್ದರೆ
	vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	ಸಂದೇಶ((ಅಕ್ಷರ *)IObuff);
# endif
	ಹಿಂತಿರುಗಿ;
    }
}

/*
 * ":pyxfile {fname}"
 */
    ಶೂನ್ಯ
ex_pyxfile(exarg_T *eap)
{
    ಮೂಲ_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    ಶೂನ್ಯ
ex_pyx(exarg_T *eap)
{
# ಆದರೆ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ಆದರೆ (p_pyx == 2)
	ex_python(eap);
    ಇಲ್ಲದಿದ್ದರೆ
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
# ಆದರೆ defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    ಆದರೆ (p_pyx == 2)
	ex_pydo(eap);
    ಇಲ್ಲದಿದ್ದರೆ
	ex_py3do(eap);
# elif defined(FEAT_PYTHON)
    ex_pydo(eap);
# elif defined(FEAT_PYTHON3)
    ex_py3do(eap);
# endif
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * ":checktime [buffer]"
 */
    ಶೂನ್ಯ
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    ಆದರೆ (eap->addr_ಎಣಿಕೆ == 0)	// default is all buffers
	check_timestamps(FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf = buflist_findnr((ಪೂರ್ಣಾಂಕ)eap->line2);
	ಆದರೆ (buf != ಖಾಲಿ)	// cannot happen?
	    (ಶೂನ್ಯ)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
