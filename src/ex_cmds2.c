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
    целое
autowrite(buf_T *buf, целое forceit)
{
    целое		r;
    bufref_T	bufref;

    если (!(p_aw || p_awa) || !p_write
	    // never autowrite a "nofile" or "nowrite" buffer
	    || bt_dontwrite(buf)
	    || (!forceit && buf->b_p_ro) || buf->b_ffname == NULL)
	вернуть FAIL;
    set_bufref(&bufref, buf);
    r = buf_write_all(buf, forceit);

    // Writing may succeed but the buffer still changed, e.g., when there is a
    // conversion error.  We do want to return FAIL then.
    если (bufref_valid(&bufref) && bufIsChanged(buf))
	r = FAIL;
    вернуть r;
}

/*
 * Flush all buffers, except the ones that are readonly or are never written.
 */
    void
autowrite_all(void)
{
    buf_T	*buf;

    если (!(p_aw || p_awa) || !p_write)
	вернуть;
    FOR_ALL_BUFFERS(buf)
	если (bufIsChanged(buf) && !buf->b_p_ro && !bt_dontwrite(buf))
	{
	    bufref_T	bufref;

	    set_bufref(&bufref, buf);

	    (void)buf_write_all(buf, FALSE);

	    // an autocommand may have deleted the buffer
	    если (!bufref_valid(&bufref))
		buf = firstbuf;
	}
}

/*
 * Return TRUE if buffer was changed and cannot be abandoned.
 * For flags use the CCGD_ values.
 */
    целое
check_changed(buf_T *buf, целое flags)
{
    целое		forceit = (flags & CCGD_FORCEIT);
    bufref_T	bufref;

    set_bufref(&bufref, buf);

    если (       !forceit
	    && bufIsChanged(buf)
	    && ((flags & CCGD_MULTWIN) || buf->b_nwindows <= 1)
	    && (!(flags & CCGD_AW) || autowrite(buf, forceit) == FAIL))
    {
#если defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
	если ((p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)) && p_write)
	{
# ifdef FEAT_TERMINAL
	    если (term_job_running(buf->b_term))
	    {
		вернуть term_confirm_stop(buf) == FAIL;
	    }
# endif

	    buf_T	*buf2;
	    целое		count = 0;

	    если (flags & CCGD_ALLBUF)
		FOR_ALL_BUFFERS(buf2)
		    если (bufIsChanged(buf2)
				     && (buf2->b_ffname != NULL
# ifdef FEAT_BROWSE
					 || (cmdmod.cmod_flags & CMOD_BROWSE)
# endif
					))
			++count;
	    если (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		вернуть FALSE;

	    dialog_changed(buf, count > 1);

	    если (!bufref_valid(&bufref))
		// Autocommand deleted buffer, oops!  It's not changed now.
		вернуть FALSE;
	    вернуть bufIsChanged(buf);
	}
#endif
	если (flags & CCGD_EXCMD)
	    no_write_message();
	иначе
	    no_write_message_nobang(curbuf);
	вернуть TRUE;
    }
    вернуть FALSE;
}

#если defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)

# если defined(FEAT_BROWSE)
/*
 * When wanting to write a file without a file name, ask the user for a name.
 */
    void
browse_save_fname(buf_T *buf)
{
    если (buf->b_fname != NULL)
	вернуть;

    char_u *fname;

    fname = do_browse(BROWSE_SAVE, (char_u *)_("Save As"),
	    (char_u *)"Untitled", NULL, NULL, NULL, buf);
    если (fname == NULL)
	вернуть;

    если (setfname(buf, fname, NULL, TRUE) == OK)
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
    целое		checkall)	// may abandon all changed buffers
{
    char_u	buff[DIALOG_MSG_SIZE];
    целое		ret;
    buf_T	*buf2;
    exarg_T	ea;

    dialog_msg(buff, _("Save changes to \"%s\"?"), buf->b_fname);
    если (checkall)
	ret = vim_dialog_yesnoallcancel(VIM_QUESTION, NULL, buff, 1);
    иначе
	ret = vim_dialog_yesnocancel(VIM_QUESTION, NULL, buff, 1);

    // Init ea pseudo-structure, this is needed for the check_overwrite()
    // function.
    CLEAR_FIELD(ea);

    если (ret == VIM_YES)
    {
	целое	empty_bufname;

# ifdef FEAT_BROWSE
	// May get file name, when there is none
	browse_save_fname(buf);

	// User cancelled the file dialog; keep the buffer modified.
	если (buf->b_fname == NULL)
	    вернуть;
# endif
	empty_bufname = buf->b_fname == NULL ? TRUE : FALSE;
	если (empty_bufname)
	    buf_set_name(buf->b_fnum, (char_u *)"Untitled");

	если (check_overwrite(&ea, buf, buf->b_fname, buf->b_ffname, FALSE) == OK)
	{
	    // didn't hit Cancel
	    если (buf_write_all(buf, FALSE) == OK)
		вернуть;
	}

	// restore to empty when write failed or was cancelled
	если (empty_bufname)
	{
	    buf->b_fname = NULL;
	    VIM_CLEAR(buf->b_ffname);
	    VIM_CLEAR(buf->b_sfname);
	}
    }
    иначе если (ret == VIM_NO)
    {
	unchanged(buf, TRUE, FALSE);
    }
    иначе если (ret == VIM_ALL)
    {
	/*
	 * Write all modified files that can be written.
	 * Skip readonly buffers, these need to be confirmed
	 * individually.
	 */
	FOR_ALL_BUFFERS(buf2)
	{
	    если (bufIsChanged(buf2)
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
		если (buf2->b_fname != NULL && check_overwrite(&ea, buf2,
				  buf2->b_fname, buf2->b_ffname, FALSE) == OK)
		    // didn't hit Cancel
		    (void)buf_write_all(buf2, FALSE);

		// an autocommand may have deleted the buffer
		если (!bufref_valid(&bufref))
		    buf2 = firstbuf;
	    }
	}
    }
    иначе если (ret == VIM_DISCARDALL)
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
    целое
can_abandon(buf_T *buf, целое forceit)
{
    вернуть (	   buf_hide(buf)
		|| !bufIsChanged(buf)
		|| buf->b_nwindows > 1
		|| autowrite(buf, forceit) == OK
		|| forceit);
}

/*
 * Add a buffer number to "bufnrs", unless it's already there.
 */
    static void
add_bufnum(целое *bufnrs, целое *bufnump, целое nr)
{
    целое i;

    для (i = 0; i < *bufnump; ++i)
	если (bufnrs[i] == nr)
	    вернуть;
    bufnrs[*bufnump] = nr;
    *bufnump = *bufnump + 1;
}

/*
 * Return TRUE if any buffer was changed and cannot be abandoned.
 * That changed buffer becomes the current buffer.
 * When "unload" is TRUE the current buffer is unloaded instead of making it
 * hidden.  This is used for ":q!".
 */
    целое
check_changed_any(
    целое		hidden,		// Only check hidden buffers
    целое		unload)
{
    целое		ret = FALSE;
    buf_T	*buf;
    целое		save;
    целое		i;
    целое		bufnum = 0;
    целое		bufcount = 0;
    целое		*bufnrs;
    tabpage_T   *tp;
    win_T	*wp;

    // Make a list of all buffers, with the most important ones first.
    FOR_ALL_BUFFERS(buf)
	++bufcount;

    если (bufcount == 0)
	вернуть FALSE;

    bufnrs = ALLOC_MULT(целое, bufcount);
    если (bufnrs == NULL)
	вернуть FALSE;

    // curbuf
    bufnrs[bufnum++] = curbuf->b_fnum;

    // buffers in current tab
    FOR_ALL_WINDOWS(wp)
	если (wp->w_buffer != curbuf)
	    add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // buffers in other tabs
    FOR_ALL_TABPAGES(tp)
	если (tp != curtab)
	    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
		add_bufnum(bufnrs, &bufnum, wp->w_buffer->b_fnum);

    // any other buffer
    FOR_ALL_BUFFERS(buf)
	add_bufnum(bufnrs, &bufnum, buf->b_fnum);

    для (i = 0; i < bufnum; ++i)
    {
	buf = buflist_findnr(bufnrs[i]);
	если (buf == NULL)
	    continue;
	если ((!hidden || buf->b_nwindows == 0) && bufIsChanged(buf))
	{
	    bufref_T bufref;

	    set_bufref(&bufref, buf);
#ifdef FEAT_TERMINAL
	    если (term_job_running(buf->b_term))
	    {
		если (term_try_stop_job(buf) == FAIL)
		    break;
	    }
	    иначе
#endif
	    // Try auto-writing the buffer.  If this fails but the buffer no
	    // longer exists it's not changed, that's OK.
	    если (check_changed(buf, (p_awa ? CCGD_AW : 0)
				 | CCGD_MULTWIN
				 | CCGD_ALLBUF) && bufref_valid(&bufref))
		break;	    // didn't save - still changes
	}
    }

    если (i >= bufnum)
	goto theend;

    // Get here if "buf" cannot be abandoned.
    ret = TRUE;
    exiting = FALSE;
#если defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    /*
     * When ":confirm" used, don't give an error message.
     */
    если (!(p_confirm || (cmdmod.cmod_flags & CMOD_CONFIRM)))
#endif
    {
	// There must be a wait_return() for this message, do_buffer()
	// may cause a redraw.  But wait_return() is a no-op when vgetc()
	// is busy (Quit used from window menu), then make sure we don't
	// cause a scroll up.
	если (vgetc_busy > 0)
	{
	    msg_row = cmdline_row;
	    msg_col = 0;
	    msg_didout = FALSE;
	}
	если (
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
    если (buf != curbuf)
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    если (wp->w_buffer == buf)
	    {
		bufref_T bufref;

		set_bufref(&bufref, buf);

		goto_tabpage_win(tp, wp);

		// Paranoia: did autocmd wipe out the buffer with changes?
		если (!bufref_valid(&bufref))
		    goto theend;
		goto buf_found;
	    }
buf_found:

    // Open the changed buffer in the current window.
    если (buf != curbuf)
	set_curbuf(buf, unload ? DOBUF_UNLOAD : DOBUF_GOTO);

theend:
    vim_free(bufnrs);
    вернуть ret;
}

/*
 * return FAIL if there is no file name, OK if there is one
 * give error message for FAIL
 */
    целое
check_fname(void)
{
    если (curbuf->b_ffname == NULL)
    {
	emsg(_(e_no_file_name));
	вернуть FAIL;
    }
    вернуть OK;
}

/*
 * flush the contents of a buffer, unless it has no file name
 *
 * return FAIL for failure, OK otherwise
 */
    целое
buf_write_all(buf_T *buf, целое forceit)
{
    целое	    retval;
    buf_T	*old_curbuf = curbuf;

    retval = (buf_write(buf, buf->b_ffname, buf->b_fname,
				   (linenr_T)1, buf->b_ml.ml_line_count, NULL,
						  FALSE, forceit, TRUE, FALSE));
    если (curbuf != old_curbuf)
    {
	msg_source(HL_ATTR(HLF_W));
	msg(_("Warning: Entered other buffer unexpectedly (check autocommands)"));
    }
    вернуть retval;
}

/*
 * ":argdo", ":windo", ":bufdo", ":tabdo", ":cdo", ":ldo", ":cfdo" and ":lfdo"
 */
    void
ex_listdo(exarg_T *eap)
{
    целое		i;
    win_T	*wp;
    tabpage_T	*tp;
    buf_T	*buf = curbuf;
    целое		next_fnum = 0;

    // ":windo" and ":tabdo" only visit existing windows/tabpages, they don't
    // change the current window's buffer, so they can't escape a 'winfixbuf'
    // window (which would create a split).
    если (curwin->w_p_wfb && eap->cmdidx != CMD_windo &&
	    eap->cmdidx != CMD_tabdo)
    {
	если ((eap->cmdidx == CMD_ldo || eap->cmdidx == CMD_lfdo) &&
		!eap->forceit)
	{
	    // Disallow :ldo if 'winfixbuf' is applied
	    emsg(_(e_winfixbuf_cannot_go_to_buffer));
	    вернуть;
	}

	если (win_valid(prevwin) && !prevwin->w_p_wfb)
	    // 'winfixbuf' is set; attempt to change to a window without it.
	    win_goto(prevwin);
	если (curwin->w_p_wfb)
	{
	    // Split the window, which will be 'nowinfixbuf', and set curwin to
	    // that
	    (void)win_split(0, 0);

	    если (curwin->w_p_wfb)
	    {
		// Autocommands set 'winfixbuf' or sent us to another window
		// with it set, or we failed to split the window.  Give up.
		emsg(_(e_winfixbuf_cannot_go_to_buffer));
		вернуть;
	    }
	}
    }

#если defined(FEAT_SYN_HL)
    char_u	*save_ei = NULL;
#endif
#ifdef FEAT_QUICKFIX
    целое		qf_size = 0;
    целое		qf_idx;
#endif

#ifndef FEAT_QUICKFIX
    если (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo ||
	    eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
    {
	ex_ni(eap);
	вернуть;
    }
#endif

#если defined(FEAT_SYN_HL)
    если (eap->cmdidx != CMD_windo && eap->cmdidx != CMD_tabdo)
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

    если (eap->cmdidx == CMD_windo
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
	    case CMD_windo:
		для ( ; wp != NULL && i + 1 < eap->line1; wp = wp->w_next)
		    i++;
		break;
	    case CMD_tabdo:
		для ( ; tp != NULL && i + 1 < eap->line1; tp = tp->tp_next)
		    i++;
		break;
	    case CMD_argdo:
		i = eap->line1 - 1;
		break;
	    default:
		break;
	}
	// set pcmark now
	если (eap->cmdidx == CMD_bufdo)
	{
	    // Advance to the first listed buffer after "eap->line1".
	    для (buf = firstbuf; buf != NULL && (buf->b_fnum < eap->line1
					  || !buf->b_p_bl); buf = buf->b_next)
		если (buf->b_fnum > eap->line2)
		{
		    buf = NULL;
		    break;
		}
	    если (buf != NULL)
		goto_buffer(eap, DOBUF_FIRST, FORWARD, buf->b_fnum);
	}
#ifdef FEAT_QUICKFIX
	иначе если (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		|| eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	{
	    qf_size = qf_get_valid_size(eap);
	    если (qf_size <= 0 || eap->line1 > qf_size)
		buf = NULL;
	    иначе
	    {
		save_clear_shm_value();
		ex_cc(eap);
		restore_shm_value();

		buf = curbuf;
		i = eap->line1 - 1;
		если (eap->addr_count <= 0)
		    // default is all the quickfix/location list entries
		    eap->line2 = qf_size;
	    }
	}
#endif
	иначе
	    setpcmark();
	listcmd_busy = TRUE;	    // avoids setting pcmark below

	пока (!got_int && buf != NULL)
	{
	    если (eap->cmdidx == CMD_argdo)
	    {
		// go to argument "i"
		если (i == ARGCOUNT)
		    break;
		// Don't call do_argfile() when already there, it will try
		// reloading the file.
		если (curwin->w_arg_idx != i || !editing_arg_idx(curwin))
		{
		    // Clear 'shm' to avoid that the file message overwrites
		    // any output from the command.
		    save_clear_shm_value();
		    do_argfile(eap, i);
		    restore_shm_value();
		}
		если (curwin->w_arg_idx != i)
		    break;
	    }
	    иначе если (eap->cmdidx == CMD_windo)
	    {
		// go to window "wp"
		если (!win_valid(wp))
		    break;
		win_goto(wp);
		если (curwin != wp)
		    break;  // something must be wrong
		wp = curwin->w_next;
	    }
	    иначе если (eap->cmdidx == CMD_tabdo)
	    {
		// go to window "tp"
		если (!valid_tabpage(tp))
		    break;
		goto_tabpage_tp(tp, TRUE, TRUE);
		tp = tp->tp_next;
	    }
	    иначе если (eap->cmdidx == CMD_bufdo)
	    {
		// Remember the number of the next listed buffer, in case
		// ":bwipe" is used or autocommands do something strange.
		next_fnum = -1;
		для (buf = curbuf->b_next; buf != NULL; buf = buf->b_next)
		    если (buf->b_p_bl)
		    {
			next_fnum = buf->b_fnum;
			break;
		    }
	    }

	    ++i;

	    // execute the command
	    do_cmdline(eap->arg, eap->ea_getline, eap->cookie,
						DOCMD_VERBOSE + DOCMD_NOWAIT);

	    если (eap->cmdidx == CMD_bufdo)
	    {
		// Done?
		если (next_fnum < 0 || next_fnum > eap->line2)
		    break;
		// Check if the buffer still exists.
		FOR_ALL_BUFFERS(buf)
		    если (buf->b_fnum == next_fnum)
			break;
		если (buf == NULL)
		    break;

		// Go to the next buffer.  Clear 'shm' to avoid that the file
		// message overwrites any output from the command.
		save_clear_shm_value();
		goto_buffer(eap, DOBUF_FIRST, FORWARD, next_fnum);
		restore_shm_value();

		// If autocommands took us elsewhere, quit here.
		если (curbuf->b_fnum != next_fnum)
		    break;
	    }

#ifdef FEAT_QUICKFIX
	    если (eap->cmdidx == CMD_cdo || eap->cmdidx == CMD_ldo
		    || eap->cmdidx == CMD_cfdo || eap->cmdidx == CMD_lfdo)
	    {
		если (i >= qf_size || i >= eap->line2)
		    break;

		qf_idx = qf_get_cur_idx(eap);

		save_clear_shm_value();
		ex_cnext(eap);
		restore_shm_value();

		// If jumping to the next quickfix entry fails, quit here
		если (qf_get_cur_idx(eap) == qf_idx)
		    break;
	    }
#endif

	    если (eap->cmdidx == CMD_windo)
	    {
		validate_cursor();	// cursor may have moved

		// required when 'scrollbind' has been set
		если (curwin->w_p_scb)
		    do_check_scrollbind(TRUE);
	    }

	    если (eap->cmdidx == CMD_windo || eap->cmdidx == CMD_tabdo)
		если (i+1 > eap->line2)
		    break;
	    если (eap->cmdidx == CMD_argdo && i >= eap->line2)
		break;
	}
	listcmd_busy = FALSE;
    }

#если defined(FEAT_SYN_HL)
    если (save_ei != NULL)
    {
	buf_T		*bnext;
	aco_save_T	aco;

	au_event_restore(save_ei);

	для (buf = firstbuf; buf != NULL; buf = bnext)
	{
	    bnext = buf->b_next;
	    если (buf->b_nwindows > 0 && (buf->b_flags & BF_SYN_SET))
	    {
		buf->b_flags &= ~BF_SYN_SET;

		// buffer was opened while Syntax autocommands were disabled,
		// need to trigger them now.
		если (buf == curbuf)
		    apply_autocmds(EVENT_SYNTAX, curbuf->b_p_syn,
					       curbuf->b_fname, TRUE, curbuf);
		иначе
		{
		    aucmd_prepbuf(&aco, buf);
		    если (curbuf == buf)
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

    если (*eap->arg == NUL)
    {
	// List all compiler scripts.
	do_cmdline_cmd((char_u *)"echo globpath(&rtp, 'compiler/*.vim')");
					// ) keep the indenter happy...
	вернуть;
    }

    buf = alloc(STRLEN(eap->arg) + 14);
    если (buf == NULL)
	вернуть;

    если (eap->forceit)
    {
	// ":compiler! {name}" sets global options
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet set <args>");
    }
    иначе
    {
	// ":compiler! {name}" sets local options.
	// To remain backwards compatible "current_compiler" is always
	// used.  A user's compiler plugin may set it, the distributed
	// plugin will then skip the settings.  Afterwards set
	// "b:current_compiler" and restore "current_compiler".
	// Explicitly prepend "g:" to make it work in a function.
	old_cur_comp = get_var_value((char_u *)"g:current_compiler");
	если (old_cur_comp != NULL)
	    old_cur_comp = vim_strsave(old_cur_comp);
	do_cmdline_cmd((char_u *)
		"command -nargs=* -keepscript CompilerSet setlocal <args>");
    }
    do_unlet((char_u *)"g:current_compiler", TRUE);
    do_unlet((char_u *)"b:current_compiler", TRUE);

    sprintf((char *)buf, "compiler/%s.vim", eap->arg);
    если (source_runtime(buf, DIP_ALL) == FAIL)
	semsg(_(e_compiler_not_supported_str), eap->arg);
    vim_free(buf);

    do_cmdline_cmd((char_u *)":delcommand CompilerSet");

    // Set "b:current_compiler" from "current_compiler".
    p = get_var_value((char_u *)"g:current_compiler");
    если (p != NULL)
	set_internal_string_var((char_u *)"b:current_compiler", p);

    // Restore "current_compiler" for ":compiler {name}".
    если (!eap->forceit)
    {
	если (old_cur_comp != NULL)
	{
	    set_internal_string_var((char_u *)"g:current_compiler",
		    old_cur_comp);
	    vim_free(old_cur_comp);
	}
	иначе
	    do_unlet((char_u *)"g:current_compiler", TRUE);
    }
}
#endif

#если defined(FEAT_PYTHON3) || defined(FEAT_PYTHON)

# если defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
/*
 * Detect Python 3 or 2, and initialize 'pyxversion'.
 */
    void
init_pyxversion(void)
{
    если (p_pyx == 0)
    {
	если (python3_enabled(FALSE))
	    p_pyx = 3;
	иначе если (python_enabled(FALSE))
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
    static целое
requires_py_version(char_u *filename)
{
    FILE    *file;
    целое	    requires_py_version = 0;
    целое	    i, lines;

    lines = (целое)p_mls;
    если (lines < 0)
	lines = 5;

    file = mch_fopen((char *)filename, "r");
    если (file == NULL)
	вернуть 0;

    для (i = 0; i < lines; i++)
    {
	если (vim_fgets(IObuff, IOSIZE, file))
	    break;
	если (i == 0 && IObuff[0] == '#' && IObuff[1] == '!')
	{
	    // Check shebang.
	    если (strstr((char *)IObuff + 2, "python2") != NULL)
	    {
		requires_py_version = 2;
		break;
	    }
	    если (strstr((char *)IObuff + 2, "python3") != NULL)
	    {
		requires_py_version = 3;
		break;
	    }
	}
	IObuff[21] = '\0';
	если (STRCMP("# requires python 2.x", IObuff) == 0)
	{
	    requires_py_version = 2;
	    break;
	}
	если (STRCMP("# requires python 3.x", IObuff) == 0)
	{
	    requires_py_version = 3;
	    break;
	}
    }
    fclose(file);
    вернуть requires_py_version;
}


/*
 * Source a python file using the requested python version.
 */
    static void
source_pyx_file(exarg_T *eap, char_u *fname)
{
    exarg_T ex;
    целое	    v = requires_py_version(fname);

# если defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
# endif
    если (v == 0)
    {
# если defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
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
    если (eap == NULL)
	CLEAR_FIELD(ex);
    иначе
	ex = *eap;
    ex.arg = fname;
    ex.cmd = (char_u *)(v == 2 ? "pyfile" : "pyfile3");

    если (v == 2)
    {
# ifdef FEAT_PYTHON
	ex_pyfile(&ex);
# иначе
	vim_snprintf((char *)IObuff, IOSIZE,
		_("W20: Required python version 2.x not supported, ignoring file: %s"),
		fname);
	msg((char *)IObuff);
# endif
	вернуть;
    }
    иначе
    {
# ifdef FEAT_PYTHON3
	ex_py3file(&ex);
# иначе
	vim_snprintf((char *)IObuff, IOSIZE,
		_("W21: Required python version 3.x not supported, ignoring file: %s"),
		fname);
	msg((char *)IObuff);
# endif
	вернуть;
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
# если defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    если (p_pyx == 2)
	ex_python(eap);
    иначе
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
# если defined(FEAT_PYTHON) && defined(FEAT_PYTHON3)
    init_pyxversion();
    если (p_pyx == 2)
	ex_pydo(eap);
    иначе
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
    целое		save_no_check_timestamps = no_check_timestamps;

    no_check_timestamps = 0;
    если (eap->addr_count == 0)	// default is all buffers
	check_timestamps(FALSE);
    иначе
    {
	buf = buflist_findnr((целое)eap->line2);
	если (buf != NULL)	// cannot happen?
	    (void)buf_check_timestamp(buf, FALSE);
    }
    no_check_timestamps = save_no_check_timestamps;
}
