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

#شامل "vim.h"
#شامل "version.h"

/*
 * If 'autowrite' option set, try to write the file.
 * Careful: autocommands may make "buf" invalid!
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
autowrite(buf_T *buf, صحیح forceit)
{
    صحیح		r;
    bufref_T	bufref;

    اگر (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == تہی)
	واپس FAIL;
    سیٹ_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    اگر (bufref_valid(&bufref) && buf_Is_بدلا(buf))
	r = FAIL;
    واپس r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    خالی
autowrite_all(خالی)
{
    buf_T	*buf;

    اگر (!(p_aw || p_awa) || !p_write)
	واپس;
    FOR_ALL_BUFFERS(buf)
	اگر (buf_Is_بدلا(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    سیٹ_bufref(&bufref, buf);

	    (خالی)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    اگر (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    صحیح
check_بدلا(buf_T *buf, صحیح flags)
{
    صحیح		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    سیٹ_bufref(&bufref, buf);

    اگر (       !forceit
	    && buf_Is_بدلا(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	اگر ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    اگر (رکن_کام_running(buf->b_رکن))
	    {
		واپس رکن_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    صحیح		گنتی = 0;

	    اگر (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    اگر (buf_Is_بدلا(buf2)
				     && (buf2->b_ffname != تہی
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++گنتی;
	    اگر (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		واپس FALSE;

	    dialog_بدلا(buf, گنتی > 1);

	    اگر (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		واپس FALSE;
	    واپس buf_Is_بدلا(buf);
	}
#اگر_ختم
	اگر (flags & CCGD_EXCMD)
	    no_write_پیغام();
	ورنہ
	    no_write_پیغام_nobang(curbuf);
	واپس TRUE;
    }
    واپس FALSE;
}

#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# اگر defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    خالی
browse_save_fname(buf_T *buf)
{
    اگر (buf->b_fname != تہی)
	واپس;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", تہی, تہی, تہی, buf);
    اگر (fname == تہی)
	واپس;

    اگر (setfname(buf, fname, تہی, TRUE) == OK)
	buf->b_flags |= BF_NOTEDITED;
    vim_free(fname);
}
# endif

/*
 * Ask the user what to do when abandoning a changed buffer.
 * Must check 'write' option first!
 */
    خالی
dialog_بدلا(
    buf_T	*buf,
    صحیح		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_پیغام_سائز];
    صحیح		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_پیغام(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    اگر (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, تہی, buff, 1);
    ورنہ
	ret = vim_dialog_yesnocancel(VIM_QUESTION, تہی, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_میدان(ea);

    اگر (ret == VIM_YES)
    {
	صحیح	خالی_خانے_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	اگر (buf->b_fname == تہی)
	    واپس;
# endif
	خالی_خانے_bufname = buf->b_fname == تہی ? TRUE : FALSE;
	اگر (خالی_خانے_bufname)
	    buf_سیٹ_name(buf->b_fnum, (char_u *)"Untitled");

	اگر (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    اگر (buf_write_all(buf, FALSE) == OK)
		واپس;
	}

	// restore to empty when write failed or was cancelled
	اگر (خالی_خانے_bufname)
	{
	    buf->b_fname = تہی;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    ورنہ اگر (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    ورنہ اگر (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    اگر (buf_Is_بدلا(buf2)
		    && (buf2->b_ffname != تہی
# ifdef FEAT_BROWSE
			|| (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
			)
		    && !bt_dontwrite(buf2)
		    && !buf2->b_p_ro)
	    {
		bufref_T bufref;

		سیٹ_bufref(&bufref, buf2);
# ifdef FEAT_BROWSE
		// May get file name, when there is none
		browse_save_fname(buf2);
# endif
		اگر (buf2->b_fname != تہی && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (خالی)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		اگر (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    ورنہ اگر (ret == VIM_DISCARDALL)
    {
	/*
	 * mark all buffers as unchanged
	 */
	FOR_ALL_BUFFERS(buf2)
	    unchanged(buf2, TRUE, FALSE);
    }
}
#اگر_ختم

/*
 * Return TRUE if the buffer "buf" can be abandoned, either by making it
 * hidden, autowriting it or unloading it.
 */
    صحیح
can_abandon(buf_T *buf, صحیح forceit)
{
    واپس (	   buf_hide(buf)
		|| !buf_Is_بدلا(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    ساکن خالی
جوڑو_bufnum(صحیح *bufnrs, صحیح *bufnump, صحیح nr)
{
    صحیح i;

    برائے (i = 0; i < *bufnump; ++i)
	اگر (bufnrs[i] == nr)
	    واپس;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    صحیح
check_بدلا_any(
    صحیح		hidden,		// Only check hidden buffers
    صحیح		unload)
{
    صحیح		ret = FALSE;
    buf_T	*buf;
    صحیح		save;
    صحیح		i;
    صحیح		bufnum = 0;
    صحیح		bufcount = 0;
    صحیح		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    اگر (bufcount == 0)
	واپس FALSE;

    bufnrs = ALLOC_MULT(صحیح, bufcount);
    اگر (bufnrs == تہی)
	واپس FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	اگر (wp->w_بفر != curbuf)
	    جوڑو_bufnum(bufnrs, &bufnum, wp->w_بفر->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	اگر (tp != curtab)
	    FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
		جوڑو_bufnum(bufnrs, &bufnum, wp->w_بفر->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	جوڑو_bufnum(bufnrs, &bufnum, buf->b_fnum);

    برائے (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	اگر (buf == تہی)
	    جاری;
	اگر ((!hidden || buf->b_nwindows == 0) && buf_Is_بدلا(buf))
	{
	    bufref_T bufref;

	    سیٹ_bufref(&bufref, buf);
#اگر_تعریف_شدہ FEAT_TERMINAL
	    اگر (رکن_کام_running(buf->b_رکن))
	    {
		اگر (رکن_try_stop_کام(buf) == FAIL)
		    توڑو;
	    }
	    ورنہ
#اگر_ختم
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    اگر (check_بدلا(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		توڑو;	    // didn't save - still changes
	}
    }

    اگر (i >= bufnum)
	جاؤ theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    اگر (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#اگر_ختم
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	اگر (vgetc_busy > 0)
	{
	    پیغام_صف = cmdline_صف;
	    پیغام_ستون = 0;
	    پیغام_didout = FALSE;
	}
	اگر (
#اگر_تعریف_شدہ FEAT_TERMINAL
		رکن_کام_running(buf->b_رکن)
		    ? semsg(_(e_کام_still_running_اندر_بفر_str), buf->b_fname)
		    :
#اگر_ختم
		semsg(_(e_no_write_since_آخری_change_for_بفر_str),
		    buf_spname(buf) != تہی ? buf_spname(buf) : buf->b_fname))
	{
	    save = no_wait_return;
	    no_wait_return = FALSE;
	    wait_return(FALSE);
	    no_wait_return = save;
	}
    }

    // Try to find a window that contains the buffer.
    اگر (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp->w_بفر == buf)
	    {
		bufref_T bufref;

		سیٹ_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		اگر (!bufref_valid(&bufref))
		    جاؤ theend;
		جاؤ buf_ملا;
	    }
buf_ملا:

    // Open the changed buffer in the current window.
    اگر (buf != curbuf)
	سیٹ_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    واپس ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    صحیح
check_fname(خالی)
{
    اگر (curbuf->b_ffname == تہی)
    {
	emsg(_(e_no_file_name));
	واپس FAIL;
    }
    واپس OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
buf_write_all(buf_T *buf, صحیح forceit)
{
    صحیح	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_سطر_گنتی, تہی,
						  FALSE, forceit, TRUE, FALSE));
    اگر (curbuf != old_curbuf)
    {
	پیغام_ماخذ(HL_ATTR(HLF_W));
	پیغام(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    واپس retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    خالی
ex_listdo(exarg_T *eap)
{
    صحیح		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    صحیح		اگلا_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    اگر (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	اگر ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_مقصد_بفر));
	    واپس;
	}

	اگر (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	اگر (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (خالی)win_بانٹو(0, 0);

	    اگر (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_مقصد_بفر));
		واپس;
	    }
	}
    }

#اگر defined(FEAT_SYN_HL)
    char_u	*save_ei = تہی;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_QUICKFIX
    صحیح		qf_سائز = 0;
    صحیح		qf_idx;
#اگر_ختم

#اگر_غیر_تعریف_شدہ FEAT_QUICKFIX
    اگر (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	واپس;
    }
#اگر_ختم

#اگر defined(FEAT_SYN_HL)
    اگر (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
    {
	// Don't do syntax HL autocommands.  Skipping the syntax file is a
	// great speed improvement.
	save_ei = au_event_disable(",Syntax");

	FOR_ALL_BUFFERS(buf)
	    buf->b_flags &= ~BF_SYN_سیٹ;
	buf = curbuf;
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    آغاز_global_changes();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    inc_clip_provider();
#اگر_ختم

    اگر (eap->cmdidx == CMD_windo
	    || eap->cmdidx == CMD_tabdo
	    || buf_hide(curbuf)
	    || !check_بدلا(curbuf, CCGD_AW
				    | (eap->forceit ? CCGD_FORCEIT : 0)
				    | CCGD_EXCMD))
    {
	i = 0;
	// start at the eap->line1 argument/window/buffer
	wp = firstwin;
	tp = پہلا_tabpage;
	انتخاب (eap->cmdidx)
	{
	    حالت CMD_windo:
		برائے ( ; wp != تہی && i + 1 < eap->line1; wp = wp->w_اگلا)
		    i++;
		توڑو;
	    حالت CMD_tabdo:
		برائے ( ; tp != تہی && i + 1 < eap->line1; tp = tp->tp_اگلا)
		    i++;
		توڑو;
	    حالت CMD_argdo:
		i = eap->line1 - 1;
		توڑو;
	    طےشدہ:
		توڑو;
	}
	// set pcmark now
	اگر (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    برائے (buf = firstbuf; buf != تہی && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_اگلا)
		اگر (buf->b_fnum > eap->line2)
		{
		    buf = تہی;
		    توڑو;
		}
	    اگر (buf != تہی)
		goto_بفر(eap, DOBUF_پہلا, FORWARD, buf->b_fnum);
	}
#اگر_تعریف_شدہ FEAT_QUICKFIX
	ورنہ اگر (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_سائز = qf_get_valid_سائز(eap);
	    اگر (qf_سائز <= 0 || eap->line1 > qf_سائز)
		buf = تہی;
	    ورنہ
	    {
		save_clear_shm_قدر();
		ex_cc(eap);
		restore_shm_قدر();

		buf = curbuf;
		i = eap->line1 - 1;
		اگر (eap->addr_گنتی <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_سائز;
	    }
	}
#اگر_ختم
	ورنہ
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	جب (!got_int && buf != تہی)
	{
	    اگر (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		اگر (i == ARGCOUNT)
		    توڑو;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		اگر (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_قدر();
		    do_argfile(eap, i);
		    restore_shm_قدر();
		}
		اگر (curwin->w_arg_idx != i)
		    توڑو;
	    }
	    ورنہ اگر (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		اگر (!win_valid(wp))
		    توڑو;
		win_goto(wp);
		اگر (curwin != wp)
		    توڑو;  // something must be wrong
		wp = curwin->w_اگلا;
	    }
	    ورنہ اگر (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		اگر (!valid_tabpage(tp))
		    توڑو;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_اگلا;
	    }
	    ورنہ اگر (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		اگلا_fnum = -1;
		برائے (buf = curbuf->b_اگلا; buf != تہی; buf = buf->b_اگلا)
		    اگر (buf->b_p_bl)
		    {
			اگلا_fnum = buf->b_fnum;
			توڑو;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    اگر (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		اگر (اگلا_fnum < 0 || اگلا_fnum > eap->line2)
		    توڑو;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    اگر (buf->b_fnum == اگلا_fnum)
			توڑو;
		اگر (buf == تہی)
		    توڑو;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_قدر();
		goto_بفر(eap, DOBUF_پہلا, FORWARD, اگلا_fnum);
		restore_shm_قدر();

		// If autocommands took us elsewhere, quit here.
		اگر (curbuf->b_fnum != اگلا_fnum)
		    توڑو;
	    }

#اگر_تعریف_شدہ FEAT_QUICKFIX
	    اگر (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		اگر (i >= qf_سائز || i >= eap->line2)
		    توڑو;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_قدر();
		ex_cnext(eap);
		restore_shm_قدر();

		// If jumping to the next quickfix entry fails, quit here
		اگر (qf_get_cur_idx(eap) == qf_idx)
		    توڑو;
	    }
#اگر_ختم

	    اگر (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		اگر (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    اگر (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		اگر (i+1 > eap->line2)
		    توڑو;
	    اگر (eap->cmdidx == CMD_argdo && i >= eap->line2)
		توڑو;
	}
	listcmd_busy = FALSE;
    }

#اگر defined(FEAT_SYN_HL)
    اگر (save_ei != تہی)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	برائے (buf = firstbuf; buf != تہی; buf = bnext)
	{
	    bnext = buf->b_اگلا;
	    اگر (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_سیٹ))
	    {
		buf->b_flags &= ~BF_SYN_سیٹ;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		اگر (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		ورنہ
		{
		    aucmd_prepbuf(&aco, buf);
		    اگر (curbuf == buf)
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
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD
    انتہا_global_changes();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CLIPBOARD_PROVIDER
    dec_clip_provider();
#اگر_ختم
}

#اگر_تعریف_شدہ FEAT_EVAL
/*
 * ":compiler[!] {name}"
 */
    خالی
ex_compiler(exarg_T *eap)
{
    char_u	*buf;
    char_u	*old_cur_comp = تہی;
    char_u	*p;

    اگر (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	واپس;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    اگر (buf == تہی)
	واپس;

    اگر (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    ورنہ
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_قدر((char_u *)"g:current_compiler");
	اگر (old_cur_comp != تہی)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((حرف *)buf, "compiler/%s.vim", eap->arg);
    اگر (ماخذ_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_قدر((char_u *)"g:current_compiler");
    اگر (p != تہی)
	سیٹ_اندرونی_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    اگر (!eap->forceit)
    {
	اگر (old_cur_comp != تہی)
	{
	    سیٹ_اندرونی_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	ورنہ
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#اگر_ختم

#اگر defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# اگر defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    خالی
init_pyxversion(خالی)
{
    اگر (p_pyx == 0)
    {
	اگر (python3_enabled(FALSE))
	    p_pyx = 3;
	ورنہ اگر (python_enabled(FALSE))
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
    ساکن صحیح
requires_py_نسخہ(char_u *filename)
{
    FILE    *file;
    صحیح	    requires_py_نسخہ = 0;
    صحیح	    i, سطریں;

    سطریں = (صحیح)p_mls;
    اگر (سطریں < 0)
	سطریں = 5;

    file = mch_fopen((حرف *)filename, "r");
    اگر (file == تہی)
	واپس 0;

    برائے (i = 0; i < سطریں; i++)
    {
	اگر (vim_fgets(IObuff, IOSIZE, file))
	    توڑو;
	اگر (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    اگر (لڑی_تلاش((حرف *)IObuff + 2, "python2") != تہی)
	    {
		requires_py_نسخہ = 2;
		توڑو;
	    }
	    اگر (لڑی_تلاش((حرف *)IObuff + 2, "python3") != تہی)
	    {
		requires_py_نسخہ = 3;
		توڑو;
	    }
	}
	IObuff[21] = '\0';
	اگر (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_نسخہ = 2;
	    توڑو;
	}
	اگر (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_نسخہ = 3;
	    توڑو;
	}
    }
    fclose(file);
    واپس requires_py_نسخہ;
}


/*
 * Source a python file using the requested python version.
 */
    ساکن خالی
ماخذ_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    صحیح	    v = requires_py_نسخہ(fname);

# اگر defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    اگر (v == 0)
    {
# اگر defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    اگر (eap == تہی)
	CLEAR_میدان(ex);
    ورنہ
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    اگر (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# ورنہ
	vim_snprintf((حرف *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	پیغام((حرف *)IObuff);
# endif
	واپس;
    }
    ورنہ
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# ورنہ
	vim_snprintf((حرف *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	پیغام((حرف *)IObuff);
# endif
	واپس;
    }
}

/*
 * ":pyxfile {fname}"
 */
    خالی
ex_pyxfile(exarg_T *eap)
{
    ماخذ_pyx_file(eap, eap->arg);
}

/*
 * ":pyx"
 */
    خالی
ex_pyx(exarg_T *eap)
{
# اگر defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    اگر (p_pyx == 2)
	ex_python(eap);
    ورنہ
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
    خالی
ex_pyxdo(exarg_T *eap)
{
# اگر defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    اگر (p_pyx == 2)
	ex_pydo(eap);
    ورنہ
	ex_py3do(eap);
# elif defined(FEAT_PYTHON)
    ex_pydo(eap);
# elif defined(FEAT_PYTHON3)
    ex_py3do(eap);
# endif
}

#اگر_ختم

/*
 * ":checktime [buffer]"
 */
    خالی
ex_checktime(exarg_T *eap)
{
    buf_T	*buf;
    صحیح		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    اگر (eap->addr_گنتی == 0)	// default is all buffers
	check_timestamps(FALSE);
    ورنہ
    {
	buf = buflist_findnr((صحیح)eap->line2);
	اگر (buf != تہی)	// cannot happen?
	    (خالی)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
