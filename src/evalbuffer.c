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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_buffers(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	ਲਈ (lnr = bp->b_listener; !abort && lnr != ਨਿਰਾ; lnr = lnr->lr_ਅਗਲਾ)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_ਕੰਮ_CHANNEL
	ਜੇ (!abort)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&bp->b_prompt_callback, copyID);
	ਜੇ (!abort)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&bp->b_prompt_ਰੁਕਾਵਟ, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	ਜੇ (!abort)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&bp->b_cfu_cb, copyID);
	ਜੇ (!abort)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&bp->b_ofu_cb, copyID);
	ਜੇ (!abort)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&bp->b_tsrfu_cb, copyID);
	ਜੇ (!abort && bp->b_p_cpt_cb != ਨਿਰਾ)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_ਗਿਣਤੀ, copyID);
# endif
	ਜੇ (!abort)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&bp->b_tfu_cb, copyID);
	ਜੇ (!abort)
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_callback(&bp->b_ffu_cb, copyID);
	ਜੇ (abort)
	    ਰੋਕੋ;
    }
    ਵਾਪਸ abort;
}

    buf_T *
buflist_ਲੱਭੋ_by_name(char_u *name, ਪੂਰਨ curtab_only)
{
    ਪੂਰਨ		save_magic;
    char_u	*save_cpo;
    buf_T	*buf;

    // Ignore 'magic' and 'cpoptions' here to make scripts portable
    save_magic = p_magic;
    p_magic = TRUE;
    save_cpo = p_cpo;
    p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;

    buf = buflist_findnr(buflist_findpat(name, name + STRLEN(name),
						    TRUE, FALSE, curtab_only));

    p_magic = save_magic;
    p_cpo = save_cpo;
    ਵਾਪਸ buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
ਲੱਭੋ_ਬਫਰ(typval_T *avar)
{
    buf_T	*buf = ਨਿਰਾ;

    ਜੇ (avar->v_type == VAR_ਸੰਖਿਆ)
	buf = buflist_findnr((ਪੂਰਨ)avar->vval.v_ਸੰਖਿਆ);
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ (avar->v_type == VAR_STRING && avar->vval.v_string != ਨਿਰਾ)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	ਜੇ (buf == ਨਿਰਾ)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		ਜੇ (buf->b_fname != ਨਿਰਾ
			&& (ਰਸਤਾ_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    ਰੋਕੋ;
	}
    }
    ਵਾਪਸ buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_win_for_curbuf(ਖਾਲੀ)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	ਜੇ (wip->wi_win != ਨਿਰਾ && wip->wi_win->w_ਬਫਰ == curbuf)
	{
	    curwin = wip->wi_win;
	    ਰੋਕੋ;
	}
    }
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    ਪੂਰਨ		cob_using_aco;
    ਪੂਰਨ		cob_save_VIsual_ਸਰਗਰਮ;
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
    ਸਥਿਰ ਖਾਲੀ
change_other_ਬਫਰ_prepare(cob_T *cob, buf_T *buf)
{
    CLEAR_ਸੂਚਕ(cob);

    // Set "curbuf" to the buffer being changed.  Then make sure there is a
    // window for it to handle any side effects.
    cob->cob_save_VIsual_ਸਰਗਰਮ = VIsual_ਸਰਗਰਮ;
    VIsual_ਸਰਗਰਮ = FALSE;
    cob->cob_curwin_save = curwin;
    curbuf = buf;
    ਲੱਭੋ_win_for_curbuf();  // simplest: find existing window for "buf"

    ਜੇ (curwin->w_ਬਫਰ != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_ਬਫਰ;
	aucmd_prepbuf(&cob->cob_aco, buf);
	ਜੇ (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    ਸਥਿਰ ਖਾਲੀ
change_other_ਬਫਰ_restore(cob_T *cob)
{
    ਜੇ (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    ਨਹੀਂਤਾਂ
    {
	curwin = cob->cob_curwin_save;
	curbuf = curwin->w_ਬਫਰ;
    }
    VIsual_ਸਰਗਰਮ = cob->cob_save_VIsual_ਸਰਗਰਮ;
}

/*
 * Set line or list of lines in buffer "buf" to "lines".
 * Any type is allowed and converted to a string.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_ਬਫਰ_ਸਤਰਾਂ(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	ਪੂਰਨ	    append,
	typval_T    *ਸਤਰਾਂ,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*ਸਤਰ = ਨਿਰਾ;
    list_T	*l = ਨਿਰਾ;
    listitem_T	*li = ਨਿਰਾ;
    ਲੰਮਾ	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    ਪੂਰਨ is_curbuf = buf == curbuf;
    ਜੇ (buf == ਨਿਰਾ || (!is_curbuf && buf->b_ml.ml_mfp == ਨਿਰਾ) || lnum < 1)
    {
	rettv->vval.v_ਸੰਖਿਆ = 1;	// FAIL
	ਜੇ (ਵਿੱਚ_vim9script() && lnum < 1)
	    semsg(_(e_invalid_ਸਤਰ_ਸੰਖਿਆ_nr), lnum_arg);
	ਵਾਪਸ;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    ਜੇ (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_ਬਫਰ_prepare(&cob, buf);

    ਜੇ (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    ਨਹੀਂਤਾਂ
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;

    ਜੇ (ਸਤਰਾਂ->v_type == VAR_LIST)
    {
	l = ਸਤਰਾਂ->vval.v_list;
	ਜੇ (l == ਨਿਰਾ || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    ਜਾਓ ਪੂਰਾ;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_ਪਹਿਲਾ;
    }
    ਨਹੀਂਤਾਂ
	ਸਤਰ = typval_tostring(ਸਤਰਾਂ, FALSE);

    // default result is zero == OK
    ਲਈ (;;)
    {
	ਜੇ (l != ਨਿਰਾ)
	{
	    // list argument, get next string
	    ਜੇ (li == ਨਿਰਾ)
		ਰੋਕੋ;
	    vim_free(ਸਤਰ);
	    ਸਤਰ = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_ਅਗਲਾ;
	}

	rettv->vval.v_ਸੰਖਿਆ = 1;	// FAIL
	ਜੇ (ਸਤਰ == ਨਿਰਾ || lnum > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1)
	    ਰੋਕੋ;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	ਜੇ (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	ਜੇ (!append && lnum <= curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    ਜੇ (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, ਸਤਰ, (colnr_T)STRLEN(ਸਤਰ) + 1, TRUE, TRUE) == OK)
	    {
		ਬਦਲਿਆ_ਬਾਈਟਾਂ(lnum, 0);
		ਜੇ (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_ਖਾਨਾ();
		rettv->vval.v_ਸੰਖਿਆ = 0;	// OK
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    ਜੇ (ml_append(lnum - 1, ਸਤਰ, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_ਸੰਖਿਆ = 0;	// OK
	}

	ਜੇ (l == ਨਿਰਾ)			// only one string argument
	    ਰੋਕੋ;
	++lnum;
    }
    vim_free(ਸਤਰ);

    ਜੇ (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_ਸਤਰਾਂ_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ਜੇ (wp->w_ਬਫਰ == buf
		    && (wp->w_ਬਫਰ != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_ਖਾਨਾ();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	ਜੇ (curwin->w_ਬਫਰ == curbuf)
	    update_topline();
    }

ਪੂਰਾ:
    ਜੇ (!is_curbuf)
	change_other_ਬਫਰ_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    ਖਾਲੀ
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    ਪੂਰਨ		did_emsg_before = did_emsg;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    lnum = tv_get_lnum(&argvars[0]);
    ਜੇ (did_emsg == did_emsg_before)
	ਸੈੱਟ_ਬਫਰ_ਸਤਰਾਂ(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
buf_ਸੈੱਟ_append_ਸਤਰ(typval_T *argvars, typval_T *rettv, ਪੂਰਨ append)
{
    linenr_T	lnum;
    buf_T	*buf;
    ਪੂਰਨ		did_emsg_before = did_emsg;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_ਸੰਖਿਆ_or_list_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ਜੇ (buf == ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = 1; // FAIL
    ਨਹੀਂਤਾਂ
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	ਜੇ (did_emsg == did_emsg_before)
	    ਸੈੱਟ_ਬਫਰ_ਸਤਰਾਂ(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    ਖਾਲੀ
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_ਸੈੱਟ_append_ਸਤਰ(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    ਖਾਲੀ
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_ਸੰਖਿਆ = buflist_ਜੋੜੋ(*name == NUL ? ਨਿਰਾ : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    ਖਾਲੀ
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = (ਲੱਭੋ_ਬਫਰ(&argvars[0]) != ਨਿਰਾ);
}

/*
 * "buflisted(expr)" function
 */
    ਖਾਲੀ
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = ਲੱਭੋ_ਬਫਰ(&argvars[0]);
    rettv->vval.v_ਸੰਖਿਆ = (buf != ਨਿਰਾ && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    ਖਾਲੀ
f_bufload(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    buf_T	*buf;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = get_buf_arg(&argvars[0]);
    ਜੇ (buf != ਨਿਰਾ)
	ਬਫਰ_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    ਖਾਲੀ
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = ਲੱਭੋ_ਬਫਰ(&argvars[0]);
    rettv->vval.v_ਸੰਖਿਆ = (buf != ਨਿਰਾ && buf->b_ml.ml_mfp != ਨਿਰਾ);
}

/*
 * "bufname(expr)" function
 */
    ਖਾਲੀ
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    ਨਹੀਂਤਾਂ
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    ਜੇ (buf != ਨਿਰਾ && buf->b_fname != ਨਿਰਾ)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = ਨਿਰਾ;
}

/*
 * "bufnr(expr)" function
 */
    ਖਾਲੀ
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ਪੂਰਨ		error = FALSE;
    char_u	*name;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_opt_ਬਫਰ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    ਨਹੀਂਤਾਂ
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    ਜੇ (buf == ਨਿਰਾ
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != ਨਿਰਾ
	    && !error)
	buf = buflist_new(name, ਨਿਰਾ, (linenr_T)1, 0);

    ਜੇ (buf != ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = buf->b_fnum;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = -1;
}

    ਸਥਿਰ ਖਾਲੀ
buf_win_common(typval_T *argvars, typval_T *rettv, ਪੂਰਨ get_nr)
{
    win_T	*wp;
    ਪੂਰਨ		winnr = 0;
    buf_T	*buf;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਬਫਰ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	ਜੇ (wp->w_ਬਫਰ == buf)
	    ਰੋਕੋ;
    }
    rettv->vval.v_ਸੰਖਿਆ = (wp != ਨਿਰਾ ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    ਖਾਲੀ
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    ਖਾਲੀ
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    ਖਾਲੀ
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	ਪਹਿਲਾ, ਆਖਰੀ;
    linenr_T	lnum;
    ਲੰਮਾ	ਗਿਣਤੀ_2;
    ਪੂਰਨ		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    ਪੂਰਨ		did_emsg_before = did_emsg;

    rettv->vval.v_ਸੰਖਿਆ = 1;	// FAIL by default

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    ਪਹਿਲਾ = tv_get_lnum_buf(&argvars[1], buf);
    ਜੇ (did_emsg > did_emsg_before)
	ਵਾਪਸ;
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	ਆਖਰੀ = tv_get_lnum_buf(&argvars[2], buf);
    ਨਹੀਂਤਾਂ
	ਆਖਰੀ = ਪਹਿਲਾ;

    ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ || ਪਹਿਲਾ < 1
			   || ਪਹਿਲਾ > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ || ਆਖਰੀ < ਪਹਿਲਾ)
	ਵਾਪਸ;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    ਜੇ (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_ਬਫਰ_prepare(&cob, buf);

    ਜੇ (ਆਖਰੀ > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	ਆਖਰੀ = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    ਗਿਣਤੀ_2 = ਆਖਰੀ - ਪਹਿਲਾ + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    ਜੇ (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    ਜੇ (u_save(ਪਹਿਲਾ - 1, ਆਖਰੀ + 1) == FAIL)
	ਜਾਓ cleanup;

    ਲਈ (lnum = ਪਹਿਲਾ; lnum <= ਆਖਰੀ; ++lnum)
	ml_delete_flags(ਪਹਿਲਾ, ML_DEL_ਸੁਨੇਹਾ);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ਜੇ (wp->w_ਬਫਰ == buf)
	{
	    ਜੇ (wp->w_cursor.lnum > ਆਖਰੀ)
		wp->w_cursor.lnum -= ਗਿਣਤੀ_2;
	    ਨਹੀਂਤਾਂ ਜੇ (wp->w_cursor.lnum > ਪਹਿਲਾ)
		wp->w_cursor.lnum = ਪਹਿਲਾ;
	    ਜੇ (wp->w_cursor.lnum > wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		wp->w_cursor.lnum = wp->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	    wp->w_valid = 0;
	    ਜੇ (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_ਖਾਨਾ();
    deleted_ਸਤਰਾਂ_mark(ਪਹਿਲਾ, ਗਿਣਤੀ_2);
    rettv->vval.v_ਸੰਖਿਆ = 0; // OK

cleanup:
    ਜੇ (!is_curbuf)
	change_other_ਬਫਰ_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    ਸਥਿਰ dict_T *
get_ਬਫਰ_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "bufnr", buf->b_fnum);
    dict_ਜੋੜੋ_string(dict, "name", buf->b_ffname);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "linecount", buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "loaded", buf->b_ml.ml_mfp != ਨਿਰਾ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "listed", buf->b_p_bl);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "changed", buf_Is_ਬਦਲਿਆ(buf));
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "changedtick", CHANGEDTICK(buf));
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "hidden",
			    buf->b_ml.ml_mfp != ਨਿਰਾ && buf->b_nwindows == 0);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_ਜੋੜੋ_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    ਜੇ (windows != ਨਿਰਾ)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ਜੇ (wp->w_ਬਫਰ == buf)
		list_append_ਸੰਖਿਆ(windows, (varnumber_T)wp->w_id);
	dict_ਜੋੜੋ_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    ਜੇ (windows != ਨਿਰਾ)
    {
	FOR_ALL_POPUPWINS(wp)
	    ਜੇ (wp->w_ਬਫਰ == buf)
		list_append_ਸੰਖਿਆ(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, wp)
		ਜੇ (wp->w_ਬਫਰ == buf)
		    list_append_ਸੰਖਿਆ(windows, (varnumber_T)wp->w_id);

	dict_ਜੋੜੋ_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    ਜੇ (buf->b_signlist != ਨਿਰਾ)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	ਜੇ (signs != ਨਿਰਾ)
	{
	    get_ਬਫਰ_signs(buf, signs);
	    dict_ਜੋੜੋ_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "lastused", buf->b_ਆਖਰੀ_ਵਰਤਿਆ);
# endif

    ਵਾਪਸ dict;
}

/*
 * "getbufinfo()" function
 */
    ਖਾਲੀ
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ਨਿਰਾ;
    buf_T	*argbuf = ਨਿਰਾ;
    dict_T	*d;
    ਪੂਰਨ		filtered = FALSE;
    ਪੂਰਨ		sel_buflisted = FALSE;
    ਪੂਰਨ		sel_bufloaded = FALSE;
    ਪੂਰਨ		sel_bufmodified = FALSE;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && check_for_opt_ਬਫਰ_or_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    // List of all the buffers or selected buffers
    ਜੇ (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	ਜੇ (sel_d != ਨਿਰਾ)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	ਜੇ (argbuf == ਨਿਰਾ)
	    ਵਾਪਸ;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	ਜੇ (argbuf != ਨਿਰਾ && argbuf != buf)
	    ਜਾਰੀ;
	ਜੇ (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == ਨਿਰਾ)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_ਬਦਲਿਆ)))
	    ਜਾਰੀ;

	d = get_ਬਫਰ_info(buf);
	ਜੇ (d != ਨਿਰਾ)
	    list_append_dict(rettv->vval.v_list, d);
	ਜੇ (argbuf != ਨਿਰਾ)
	    ਵਾਪਸ;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਬਫਰ_ਸਤਰਾਂ(
    buf_T	*buf,
    linenr_T	ਸ਼ੁਰੂ,
    linenr_T	ਸਿਰਾ,
    ਪੂਰਨ		retlist,
    typval_T	*rettv)
{
    ਜੇ (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ਨਿਰਾ;
    }

    ਜੇ (buf == ਨਿਰਾ || buf->b_ml.ml_mfp == ਨਿਰਾ || ਸ਼ੁਰੂ < 0)
    {
	ਜੇ (retlist)
	    (ਖਾਲੀ)rettv_list_alloc(rettv);
	ਵਾਪਸ;
    }

    ਜੇ (!retlist)
    {
	ਜੇ (ਸ਼ੁਰੂ >= 1 && ਸ਼ੁਰੂ <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, ਸ਼ੁਰੂ, FALSE),
		    ml_get_buf_len(buf, ਸ਼ੁਰੂ));
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    ਨਹੀਂਤਾਂ
    {
	list_T		*l;
	listitem_T	*li;

	ਸ਼ੁਰੂ = MAX(ਸ਼ੁਰੂ, 1);
	ਸਿਰਾ = MIN(ਸਿਰਾ, buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
	ਜੇ (ਸਿਰਾ < ਸ਼ੁਰੂ)
	    ਜਾਓ nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_ਚੀਜ਼ਾਂ((ਪੂਰਨ)(ਸਿਰਾ - ਸ਼ੁਰੂ + 1));
	ਜੇ (l == ਨਿਰਾ)
	    ਜਾਓ nolist;
	rettv_list_ਸੈੱਟ(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	ਲਈ (li = l->lv_ਪਹਿਲਾ; ਸ਼ੁਰੂ <= ਸਿਰਾ; ++ਸ਼ੁਰੂ, li = li->li_ਅਗਲਾ)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, ਸ਼ੁਰੂ, FALSE),
						   ml_get_buf_len(buf, ਸ਼ੁਰੂ));
	}
    }
    ਵਾਪਸ;

nolist:
    (ਖਾਲੀ)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    ਸਥਿਰ ਖਾਲੀ
getbufline(typval_T *argvars, typval_T *rettv, ਪੂਰਨ retlist)
{
    linenr_T	lnum = 1;
    linenr_T	ਸਿਰਾ = 1;
    buf_T	*buf;
    ਪੂਰਨ		did_emsg_before = did_emsg;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਬਫਰ_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ਜੇ (buf != ਨਿਰਾ)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	ਜੇ (did_emsg > did_emsg_before)
	    ਵਾਪਸ;
	ਜੇ (argvars[2].v_type == VAR_UNKNOWN)
	    ਸਿਰਾ = lnum;
	ਨਹੀਂਤਾਂ
	    ਸਿਰਾ = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_ਬਫਰ_ਸਤਰਾਂ(buf, lnum, ਸਿਰਾ, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    ਖਾਲੀ
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    ਖਾਲੀ
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    ਖਾਲੀ
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	ਸਿਰਾ;
    ਪੂਰਨ		retlist;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    lnum = tv_get_lnum(argvars);
    ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
    {
	ਸਿਰਾ = 0;
	retlist = FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਿਰਾ = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_ਬਫਰ_ਸਤਰਾਂ(curbuf, lnum, ਸਿਰਾ, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    ਖਾਲੀ
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_ਸੈੱਟ_append_ਸਤਰ(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    ਖਾਲੀ
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    ਪੂਰਨ		did_emsg_before = did_emsg;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    lnum = tv_get_lnum(&argvars[0]);
    ਜੇ (did_emsg == did_emsg_before)
	ਸੈੱਟ_ਬਫਰ_ਸਤਰਾਂ(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#ਜੇ_ਅੰਤ  // FEAT_EVAL

#ਜੇ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    ਖਾਲੀ
switch_ਬਫਰ(bufref_T *save_curbuf, buf_T *buf)
{
    ਖੰਡ_autocmds();
# ifdef FEAT_FOLDING
    ++disable_fold_update;
# endif
    ਸੈੱਟ_bufref(save_curbuf, curbuf);
    --curbuf->b_nwindows;
    curbuf = buf;
    curwin->w_ਬਫਰ = buf;
    ++curbuf->b_nwindows;
}

/*
 * Restore the current buffer after using switch_buffer().
 */
    ਖਾਲੀ
restore_ਬਫਰ(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    ਜੇ (bufref_valid(save_curbuf))
    {
	--curbuf->b_nwindows;
	curwin->w_ਬਫਰ = save_curbuf->br_buf;
	curbuf = save_curbuf->br_buf;
	++curbuf->b_nwindows;
    }
}

/*
 * Find a window for buffer "buf".
 * If found OK is returned and "wp" and "tp" are set to the window and tabpage.
 * If not found FAIL is returned.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਭੋ_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	ਜੇ ((*wp)->w_ਬਫਰ == buf)
	    ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    ਖਾਲੀ
switch_ਮੰਜ਼ਿਲ_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    ਜੇ (ਲੱਭੋ_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_ਬਫਰ(save_curbuf, buf);
    ਨਹੀਂਤਾਂ ਜੇ (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_ਬਫਰ(save_curbuf, buf);
    }
}

    ਖਾਲੀ
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    ਜੇ (save_curbuf->br_buf == ਨਿਰਾ)
	restore_win(switchwin, TRUE);
    ਨਹੀਂਤਾਂ
	restore_ਬਫਰ(save_curbuf);
}
#ਜੇ_ਅੰਤ
