/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * evalbuffer.c: Buffer related builtin functions
 */

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_buffers(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	വേണ്ടി (lnr = bp->b_listener; !abort && lnr != ശൂന്യ; lnr = lnr->lr_next)
	    abort = abort || set_ref_in_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_JOB_CHANNEL
	എങ്കിൽ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_callback, copyID);
	എങ്കിൽ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_interrupt, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	എങ്കിൽ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_cfu_cb, copyID);
	എങ്കിൽ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ofu_cb, copyID);
	എങ്കിൽ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tsrfu_cb, copyID);
	എങ്കിൽ (!abort && bp->b_p_cpt_cb != ശൂന്യ)
	    abort = abort || set_ref_in_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_എണ്ണം, copyID);
# endif
	എങ്കിൽ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tfu_cb, copyID);
	എങ്കിൽ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ffu_cb, copyID);
	എങ്കിൽ (abort)
	    നിർത്തുക;
    }
    തിരികെ abort;
}

    buf_T *
buflist_find_by_name(char_u *name, പൂർണ്ണസംഖ്യ curtab_only)
{
    പൂർണ്ണസംഖ്യ		save_magic;
    char_u	*save_cpo;
    buf_T	*buf;

    // Ignore 'magic' and 'cpoptions' here to make scripts portable
    save_magic = p_magic;
    p_magic = TRUE;
    save_cpo = p_cpo;
    p_cpo = empty_option;

    buf = buflist_findnr(buflist_findpat(name, name + STRLEN(name),
						    TRUE, FALSE, curtab_only));

    p_magic = save_magic;
    p_cpo = save_cpo;
    തിരികെ buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
find_buffer(typval_T *avar)
{
    buf_T	*buf = ശൂന്യ;

    എങ്കിൽ (avar->v_type == VAR_സംഖ്യ)
	buf = buflist_findnr((പൂർണ്ണസംഖ്യ)avar->vval.v_സംഖ്യ);
    അല്ലെങ്കിൽ എങ്കിൽ (in_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	തിരികെ ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ (avar->v_type == VAR_STRING && avar->vval.v_string != ശൂന്യ)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	എങ്കിൽ (buf == ശൂന്യ)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		എങ്കിൽ (buf->b_fname != ശൂന്യ
			&& (path_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    നിർത്തുക;
	}
    }
    തിരികെ buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    സ്ഥിതം ശൂന്യം
find_win_for_curbuf(ശൂന്യം)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	എങ്കിൽ (wip->wi_win != ശൂന്യ && wip->wi_win->w_buffer == curbuf)
	{
	    curwin = wip->wi_win;
	    നിർത്തുക;
	}
    }
}

തരനിർവചനം ഘടന {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    പൂർണ്ണസംഖ്യ		cob_using_aco;
    പൂർണ്ണസംഖ്യ		cob_save_VIsual_active;
} cob_T;

/*
 * Used before making a change in "buf", which is not the current one: Make
 * "buf" the current buffer and find a window for this buffer, so that side
 * effects are done correctly (e.g., adjusting marks).
 *
 * Information is saved in "cob" and MUST be restored by calling
 * change_other_buffer_restore().
 *
 * If this fails then "curbuf" will not be equal to "buf".
 */
    സ്ഥിതം ശൂന്യം
change_other_buffer_prepare(cob_T *cob, buf_T *buf)
{
    CLEAR_POINTER(cob);

    // Set "curbuf" to the buffer being changed.  Then make sure there is a
    // window for it to handle any side effects.
    cob->cob_save_VIsual_active = VIsual_active;
    VIsual_active = FALSE;
    cob->cob_curwin_save = curwin;
    curbuf = buf;
    find_win_for_curbuf();  // simplest: find existing window for "buf"

    എങ്കിൽ (curwin->w_buffer != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_buffer;
	aucmd_prepbuf(&cob->cob_aco, buf);
	എങ്കിൽ (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    സ്ഥിതം ശൂന്യം
change_other_buffer_restore(cob_T *cob)
{
    എങ്കിൽ (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    അല്ലെങ്കിൽ
    {
	curwin = cob->cob_curwin_save;
	curbuf = curwin->w_buffer;
    }
    VIsual_active = cob->cob_save_VIsual_active;
}

/*
 * Set line or list of lines in buffer "buf" to "lines".
 * Any type is allowed and converted to a string.
 */
    സ്ഥിതം ശൂന്യം
set_buffer_വരികൾ(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	പൂർണ്ണസംഖ്യ	    append,
	typval_T    *വരികൾ,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*വരി = ശൂന്യ;
    list_T	*l = ശൂന്യ;
    listitem_T	*li = ശൂന്യ;
    long	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    പൂർണ്ണസംഖ്യ is_curbuf = buf == curbuf;
    എങ്കിൽ (buf == ശൂന്യ || (!is_curbuf && buf->b_ml.ml_mfp == ശൂന്യ) || lnum < 1)
    {
	rettv->vval.v_സംഖ്യ = 1;	// FAIL
	എങ്കിൽ (in_vim9script() && lnum < 1)
	    semsg(_(e_invalid_വരി_സംഖ്യ_nr), lnum_arg);
	തിരികെ;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    എങ്കിൽ (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    എങ്കിൽ (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    അല്ലെങ്കിൽ
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_വരി_എണ്ണം;

    എങ്കിൽ (വരികൾ->v_type == VAR_LIST)
    {
	l = വരികൾ->vval.v_list;
	എങ്കിൽ (l == ശൂന്യ || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    പോകുക done;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    അല്ലെങ്കിൽ
	വരി = typval_tostring(വരികൾ, FALSE);

    // default result is zero == OK
    വേണ്ടി (;;)
    {
	എങ്കിൽ (l != ശൂന്യ)
	{
	    // list argument, get next string
	    എങ്കിൽ (li == ശൂന്യ)
		നിർത്തുക;
	    vim_free(വരി);
	    വരി = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_next;
	}

	rettv->vval.v_സംഖ്യ = 1;	// FAIL
	എങ്കിൽ (വരി == ശൂന്യ || lnum > curbuf->b_ml.ml_വരി_എണ്ണം + 1)
	    നിർത്തുക;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	എങ്കിൽ (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	എങ്കിൽ (!append && lnum <= curbuf->b_ml.ml_വരി_എണ്ണം)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    എങ്കിൽ (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, വരി, (colnr_T)STRLEN(വരി) + 1, TRUE, TRUE) == OK)
	    {
		changed_bytes(lnum, 0);
		എങ്കിൽ (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_col();
		rettv->vval.v_സംഖ്യ = 0;	// OK
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    എങ്കിൽ (ml_append(lnum - 1, വരി, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_സംഖ്യ = 0;	// OK
	}

	എങ്കിൽ (l == ശൂന്യ)			// only one string argument
	    നിർത്തുക;
	++lnum;
    }
    vim_free(വരി);

    എങ്കിൽ (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_വരികൾ_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    എങ്കിൽ (wp->w_buffer == buf
		    && (wp->w_buffer != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_col();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	എങ്കിൽ (curwin->w_buffer == curbuf)
	    update_topline();
    }

done:
    എങ്കിൽ (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    ശൂന്യം
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;

    എങ്കിൽ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	തിരികെ;

    lnum = tv_get_lnum(&argvars[0]);
    എങ്കിൽ (did_emsg == did_emsg_before)
	set_buffer_വരികൾ(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    സ്ഥിതം ശൂന്യം
buf_set_append_വരി(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ append)
{
    linenr_T	lnum;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_സംഖ്യ_or_list_arg(argvars, 2) == FAIL))
	തിരികെ;

    buf = tv_get_buf(&argvars[0], FALSE);
    എങ്കിൽ (buf == ശൂന്യ)
	rettv->vval.v_സംഖ്യ = 1; // FAIL
    അല്ലെങ്കിൽ
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	എങ്കിൽ (did_emsg == did_emsg_before)
	    set_buffer_വരികൾ(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    ശൂന്യം
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_വരി(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    ശൂന്യം
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    എങ്കിൽ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_സംഖ്യ = buflist_add(*name == NUL ? ശൂന്യ : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    ശൂന്യം
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = (find_buffer(&argvars[0]) != ശൂന്യ);
}

/*
 * "buflisted(expr)" function
 */
    ശൂന്യം
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_സംഖ്യ = (buf != ശൂന്യ && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    ശൂന്യം
f_bufload(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = get_buf_arg(&argvars[0]);
    എങ്കിൽ (buf != ശൂന്യ)
	buffer_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    ശൂന്യം
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_സംഖ്യ = (buf != ശൂന്യ && buf->b_ml.ml_mfp != ശൂന്യ);
}

/*
 * "bufname(expr)" function
 */
    ശൂന്യം
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    എങ്കിൽ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    അല്ലെങ്കിൽ
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    എങ്കിൽ (buf != ശൂന്യ && buf->b_fname != ശൂന്യ)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    അല്ലെങ്കിൽ
	rettv->vval.v_string = ശൂന്യ;
}

/*
 * "bufnr(expr)" function
 */
    ശൂന്യം
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		error = FALSE;
    char_u	*name;

    എങ്കിൽ (in_vim9script()
	    && (check_for_opt_buffer_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    അല്ലെങ്കിൽ
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    എങ്കിൽ (buf == ശൂന്യ
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != ശൂന്യ
	    && !error)
	buf = buflist_new(name, ശൂന്യ, (linenr_T)1, 0);

    എങ്കിൽ (buf != ശൂന്യ)
	rettv->vval.v_സംഖ്യ = buf->b_fnum;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = -1;
}

    സ്ഥിതം ശൂന്യം
buf_win_common(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ get_nr)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		winnr = 0;
    buf_T	*buf;

    എങ്കിൽ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	തിരികെ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	എങ്കിൽ (wp->w_buffer == buf)
	    നിർത്തുക;
    }
    rettv->vval.v_സംഖ്യ = (wp != ശൂന്യ ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    ശൂന്യം
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    ശൂന്യം
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    ശൂന്യം
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	first, last;
    linenr_T	lnum;
    long	എണ്ണം;
    പൂർണ്ണസംഖ്യ		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;

    rettv->vval.v_സംഖ്യ = 1;	// FAIL by default

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	തിരികെ;

    buf = tv_get_buf(&argvars[0], FALSE);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    first = tv_get_lnum_buf(&argvars[1], buf);
    എങ്കിൽ (did_emsg > did_emsg_before)
	തിരികെ;
    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
	last = tv_get_lnum_buf(&argvars[2], buf);
    അല്ലെങ്കിൽ
	last = first;

    എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ || first < 1
			   || first > buf->b_ml.ml_വരി_എണ്ണം || last < first)
	തിരികെ;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    എങ്കിൽ (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    എങ്കിൽ (last > curbuf->b_ml.ml_വരി_എണ്ണം)
	last = curbuf->b_ml.ml_വരി_എണ്ണം;
    എണ്ണം = last - first + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    എങ്കിൽ (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    എങ്കിൽ (u_save(first - 1, last + 1) == FAIL)
	പോകുക cleanup;

    വേണ്ടി (lnum = first; lnum <= last; ++lnum)
	ml_delete_flags(first, ML_DEL_MESSAGE);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	എങ്കിൽ (wp->w_buffer == buf)
	{
	    എങ്കിൽ (wp->w_cursor.lnum > last)
		wp->w_cursor.lnum -= എണ്ണം;
	    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_cursor.lnum > first)
		wp->w_cursor.lnum = first;
	    എങ്കിൽ (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_വരി_എണ്ണം)
		wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_വരി_എണ്ണം;
	    wp->w_valid = 0;
	    എങ്കിൽ (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_col();
    deleted_വരികൾ_mark(first, എണ്ണം);
    rettv->vval.v_സംഖ്യ = 0; // OK

cleanup:
    എങ്കിൽ (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    സ്ഥിതം dict_T *
get_buffer_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    എങ്കിൽ (dict == ശൂന്യ)
	തിരികെ ശൂന്യ;

    dict_add_സംഖ്യ(dict, "bufnr", buf->b_fnum);
    dict_add_string(dict, "name", buf->b_ffname);
    dict_add_സംഖ്യ(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_add_സംഖ്യ(dict, "linecount", buf->b_ml.ml_വരി_എണ്ണം);
    dict_add_സംഖ്യ(dict, "loaded", buf->b_ml.ml_mfp != ശൂന്യ);
    dict_add_സംഖ്യ(dict, "listed", buf->b_p_bl);
    dict_add_സംഖ്യ(dict, "changed", bufIsChanged(buf));
    dict_add_സംഖ്യ(dict, "changedtick", CHANGEDTICK(buf));
    dict_add_സംഖ്യ(dict, "hidden",
			    buf->b_ml.ml_mfp != ശൂന്യ && buf->b_nwindows == 0);
    dict_add_സംഖ്യ(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_add_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    എങ്കിൽ (windows != ശൂന്യ)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    എങ്കിൽ (wp->w_buffer == buf)
		list_append_സംഖ്യ(windows, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    എങ്കിൽ (windows != ശൂന്യ)
    {
	FOR_ALL_POPUPWINS(wp)
	    എങ്കിൽ (wp->w_buffer == buf)
		list_append_സംഖ്യ(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		എങ്കിൽ (wp->w_buffer == buf)
		    list_append_സംഖ്യ(windows, (varnumber_T)wp->w_id);

	dict_add_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    എങ്കിൽ (buf->b_signlist != ശൂന്യ)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	എങ്കിൽ (signs != ശൂന്യ)
	{
	    get_buffer_signs(buf, signs);
	    dict_add_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_add_സംഖ്യ(dict, "lastused", buf->b_last_used);
# endif

    തിരികെ dict;
}

/*
 * "getbufinfo()" function
 */
    ശൂന്യം
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ശൂന്യ;
    buf_T	*argbuf = ശൂന്യ;
    dict_T	*d;
    പൂർണ്ണസംഖ്യ		filtered = FALSE;
    പൂർണ്ണസംഖ്യ		sel_buflisted = FALSE;
    പൂർണ്ണസംഖ്യ		sel_bufloaded = FALSE;
    പൂർണ്ണസംഖ്യ		sel_bufmodified = FALSE;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && check_for_opt_buffer_or_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    // List of all the buffers or selected buffers
    എങ്കിൽ (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	എങ്കിൽ (sel_d != ശൂന്യ)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	എങ്കിൽ (argbuf == ശൂന്യ)
	    തിരികെ;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	എങ്കിൽ (argbuf != ശൂന്യ && argbuf != buf)
	    തുടരുക;
	എങ്കിൽ (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == ശൂന്യ)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_changed)))
	    തുടരുക;

	d = get_buffer_info(buf);
	എങ്കിൽ (d != ശൂന്യ)
	    list_append_dict(rettv->vval.v_list, d);
	എങ്കിൽ (argbuf != ശൂന്യ)
	    തിരികെ;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    സ്ഥിതം ശൂന്യം
get_buffer_വരികൾ(
    buf_T	*buf,
    linenr_T	start,
    linenr_T	end,
    പൂർണ്ണസംഖ്യ		retlist,
    typval_T	*rettv)
{
    എങ്കിൽ (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ശൂന്യ;
    }

    എങ്കിൽ (buf == ശൂന്യ || buf->b_ml.ml_mfp == ശൂന്യ || start < 0)
    {
	എങ്കിൽ (retlist)
	    (ശൂന്യം)rettv_list_alloc(rettv);
	തിരികെ;
    }

    എങ്കിൽ (!retlist)
    {
	എങ്കിൽ (start >= 1 && start <= buf->b_ml.ml_വരി_എണ്ണം)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, start, FALSE),
		    ml_get_buf_len(buf, start));
	അല്ലെങ്കിൽ
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    അല്ലെങ്കിൽ
    {
	list_T		*l;
	listitem_T	*li;

	start = MAX(start, 1);
	end = MIN(end, buf->b_ml.ml_വരി_എണ്ണം);
	എങ്കിൽ (end < start)
	    പോകുക nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_ഇനങ്ങൾ((പൂർണ്ണസംഖ്യ)(end - start + 1));
	എങ്കിൽ (l == ശൂന്യ)
	    പോകുക nolist;
	rettv_list_set(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	വേണ്ടി (li = l->lv_first; start <= end; ++start, li = li->li_next)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, start, FALSE),
						   ml_get_buf_len(buf, start));
	}
    }
    തിരികെ;

nolist:
    (ശൂന്യം)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    സ്ഥിതം ശൂന്യം
getbufline(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ retlist)
{
    linenr_T	lnum = 1;
    linenr_T	end = 1;
    buf_T	*buf;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;

    എങ്കിൽ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	തിരികെ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    എങ്കിൽ (buf != ശൂന്യ)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	എങ്കിൽ (did_emsg > did_emsg_before)
	    തിരികെ;
	എങ്കിൽ (argvars[2].v_type == VAR_UNKNOWN)
	    end = lnum;
	അല്ലെങ്കിൽ
	    end = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_buffer_വരികൾ(buf, lnum, end, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    ശൂന്യം
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    ശൂന്യം
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    ശൂന്യം
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	end;
    പൂർണ്ണസംഖ്യ		retlist;

    എങ്കിൽ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	തിരികെ;

    lnum = tv_get_lnum(argvars);
    എങ്കിൽ (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = 0;
	retlist = FALSE;
    }
    അല്ലെങ്കിൽ
    {
	end = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_buffer_വരികൾ(curbuf, lnum, end, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    ശൂന്യം
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_വരി(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    ശൂന്യം
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;

    എങ്കിൽ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	തിരികെ;

    lnum = tv_get_lnum(&argvars[0]);
    എങ്കിൽ (did_emsg == did_emsg_before)
	set_buffer_വരികൾ(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#endif  // FEAT_EVAL

#എങ്കിൽ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    ശൂന്യം
switch_buffer(bufref_T *save_curbuf, buf_T *buf)
{
    block_autocmds();
# ifdef FEAT_FOLDING
    ++disable_fold_update;
# endif
    set_bufref(save_curbuf, curbuf);
    --curbuf->b_nwindows;
    curbuf = buf;
    curwin->w_buffer = buf;
    ++curbuf->b_nwindows;
}

/*
 * Restore the current buffer after using switch_buffer().
 */
    ശൂന്യം
restore_buffer(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    എങ്കിൽ (bufref_valid(save_curbuf))
    {
	--curbuf->b_nwindows;
	curwin->w_buffer = save_curbuf->br_buf;
	curbuf = save_curbuf->br_buf;
	++curbuf->b_nwindows;
    }
}

/*
 * Find a window for buffer "buf".
 * If found OK is returned and "wp" and "tp" are set to the window and tabpage.
 * If not found FAIL is returned.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
find_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	എങ്കിൽ ((*wp)->w_buffer == buf)
	    തിരികെ OK;
    തിരികെ FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    ശൂന്യം
switch_to_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    എങ്കിൽ (find_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_buffer(save_curbuf, buf);
    അല്ലെങ്കിൽ എങ്കിൽ (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_buffer(save_curbuf, buf);
    }
}

    ശൂന്യം
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    എങ്കിൽ (save_curbuf->br_buf == ശൂന്യ)
	restore_win(switchwin, TRUE);
    അല്ലെങ്കിൽ
	restore_buffer(save_curbuf);
}
#endif
