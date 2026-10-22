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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_buffers(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	ಪ್ರತಿ (lnr = bp->b_listener; !abort && lnr != ರಿಕ್ತ; lnr = lnr->lr_next)
	    abort = abort || set_ref_in_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_JOB_CHANNEL
	ಯದಿ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_callback, copyID);
	ಯದಿ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_interrupt, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	ಯದಿ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_cfu_cb, copyID);
	ಯದಿ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ofu_cb, copyID);
	ಯದಿ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tsrfu_cb, copyID);
	ಯದಿ (!abort && bp->b_p_cpt_cb != ರಿಕ್ತ)
	    abort = abort || set_ref_in_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_ಎಣಿಕೆ, copyID);
# endif
	ಯದಿ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tfu_cb, copyID);
	ಯದಿ (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ffu_cb, copyID);
	ಯದಿ (abort)
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ abort;
}

    buf_T *
buflist_find_by_name(char_u *name, ಪೂರ್ಣಾಙ್ಕ curtab_only)
{
    ಪೂರ್ಣಾಙ್ಕ		save_magic;
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
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
find_buffer(typval_T *avar)
{
    buf_T	*buf = ರಿಕ್ತ;

    ಯದಿ (avar->v_type == VAR_ಸಂಖ್ಯೆ)
	buf = buflist_findnr((ಪೂರ್ಣಾಙ್ಕ)avar->vval.v_ಸಂಖ್ಯೆ);
    ಅನ್ಯಥಾ ಯದಿ (in_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ (avar->v_type == VAR_STRING && avar->vval.v_string != ರಿಕ್ತ)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	ಯದಿ (buf == ರಿಕ್ತ)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		ಯದಿ (buf->b_fname != ರಿಕ್ತ
			&& (path_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    ಭಙ್ಗ;
	}
    }
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
find_win_for_curbuf(ಶೂನ್ಯ)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	ಯದಿ (wip->wi_win != ರಿಕ್ತ && wip->wi_win->w_buffer == curbuf)
	{
	    curwin = wip->wi_win;
	    ಭಙ್ಗ;
	}
    }
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    ಪೂರ್ಣಾಙ್ಕ		cob_using_aco;
    ಪೂರ್ಣಾಙ್ಕ		cob_save_VIsual_active;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
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

    ಯದಿ (curwin->w_buffer != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_buffer;
	aucmd_prepbuf(&cob->cob_aco, buf);
	ಯದಿ (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
change_other_buffer_restore(cob_T *cob)
{
    ಯದಿ (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
set_buffer_ಸಾಲುಗಳು(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	ಪೂರ್ಣಾಙ್ಕ	    append,
	typval_T    *ಸಾಲುಗಳು,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*ಸಾಲು = ರಿಕ್ತ;
    list_T	*l = ರಿಕ್ತ;
    listitem_T	*li = ರಿಕ್ತ;
    long	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    ಪೂರ್ಣಾಙ್ಕ is_curbuf = buf == curbuf;
    ಯದಿ (buf == ರಿಕ್ತ || (!is_curbuf && buf->b_ml.ml_mfp == ರಿಕ್ತ) || lnum < 1)
    {
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;	// FAIL
	ಯದಿ (in_vim9script() && lnum < 1)
	    semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), lnum_arg);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    ಯದಿ (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    ಯದಿ (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    ಅನ್ಯಥಾ
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಯದಿ (ಸಾಲುಗಳು->v_type == VAR_LIST)
    {
	l = ಸಾಲುಗಳು->vval.v_list;
	ಯದಿ (l == ರಿಕ್ತ || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    ಗಚ್ಛ done;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    ಅನ್ಯಥಾ
	ಸಾಲು = typval_tostring(ಸಾಲುಗಳು, FALSE);

    // default result is zero == OK
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (l != ರಿಕ್ತ)
	{
	    // list argument, get next string
	    ಯದಿ (li == ರಿಕ್ತ)
		ಭಙ್ಗ;
	    vim_free(ಸಾಲು);
	    ಸಾಲು = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_next;
	}

	rettv->vval.v_ಸಂಖ್ಯೆ = 1;	// FAIL
	ಯದಿ (ಸಾಲು == ರಿಕ್ತ || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	    ಭಙ್ಗ;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	ಯದಿ (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	ಯದಿ (!append && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    ಯದಿ (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, ಸಾಲು, (colnr_T)STRLEN(ಸಾಲು) + 1, TRUE, TRUE) == OK)
	    {
		changed_bytes(lnum, 0);
		ಯದಿ (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_col();
		rettv->vval.v_ಸಂಖ್ಯೆ = 0;	// OK
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    ಯದಿ (ml_append(lnum - 1, ಸಾಲು, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_ಸಂಖ್ಯೆ = 0;	// OK
	}

	ಯದಿ (l == ರಿಕ್ತ)			// only one string argument
	    ಭಙ್ಗ;
	++lnum;
    }
    vim_free(ಸಾಲು);

    ಯದಿ (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_ಸಾಲುಗಳು_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಯದಿ (wp->w_buffer == buf
		    && (wp->w_buffer != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_col();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	ಯದಿ (curwin->w_buffer == curbuf)
	    update_topline();
    }

done:
    ಯದಿ (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    ಶೂನ್ಯ
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;

    ಯದಿ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    lnum = tv_get_lnum(&argvars[0]);
    ಯದಿ (did_emsg == did_emsg_before)
	set_buffer_ಸಾಲುಗಳು(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
buf_set_append_ಸಾಲು(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ append)
{
    linenr_T	lnum;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_ಸಂಖ್ಯೆ_or_list_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ಯದಿ (buf == ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = 1; // FAIL
    ಅನ್ಯಥಾ
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	ಯದಿ (did_emsg == did_emsg_before)
	    set_buffer_ಸಾಲುಗಳು(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    ಶೂನ್ಯ
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_ಸಾಲು(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    ಶೂನ್ಯ
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ಯದಿ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = buflist_add(*name == NUL ? ರಿಕ್ತ : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    ಶೂನ್ಯ
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = (find_buffer(&argvars[0]) != ರಿಕ್ತ);
}

/*
 * "buflisted(expr)" function
 */
    ಶೂನ್ಯ
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = (buf != ರಿಕ್ತ && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    ಶೂನ್ಯ
f_bufload(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = get_buf_arg(&argvars[0]);
    ಯದಿ (buf != ರಿಕ್ತ)
	buffer_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    ಶೂನ್ಯ
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = (buf != ರಿಕ್ತ && buf->b_ml.ml_mfp != ರಿಕ್ತ);
}

/*
 * "bufname(expr)" function
 */
    ಶೂನ್ಯ
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    ಯದಿ (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    ಅನ್ಯಥಾ
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    ಯದಿ (buf != ರಿಕ್ತ && buf->b_fname != ರಿಕ್ತ)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    ಅನ್ಯಥಾ
	rettv->vval.v_string = ರಿಕ್ತ;
}

/*
 * "bufnr(expr)" function
 */
    ಶೂನ್ಯ
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
    char_u	*name;

    ಯದಿ (in_vim9script()
	    && (check_for_opt_buffer_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    ಅನ್ಯಥಾ
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    ಯದಿ (buf == ರಿಕ್ತ
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != ರಿಕ್ತ
	    && !error)
	buf = buflist_new(name, ರಿಕ್ತ, (linenr_T)1, 0);

    ಯದಿ (buf != ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = buf->b_fnum;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
buf_win_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ get_nr)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		winnr = 0;
    buf_T	*buf;

    ಯದಿ (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	ಯದಿ (wp->w_buffer == buf)
	    ಭಙ್ಗ;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = (wp != ರಿಕ್ತ ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    ಶೂನ್ಯ
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    ಶೂನ್ಯ
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    ಶೂನ್ಯ
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	first, last;
    linenr_T	lnum;
    long	ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;

    rettv->vval.v_ಸಂಖ್ಯೆ = 1;	// FAIL by default

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    first = tv_get_lnum_buf(&argvars[1], buf);
    ಯದಿ (did_emsg > did_emsg_before)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
	last = tv_get_lnum_buf(&argvars[2], buf);
    ಅನ್ಯಥಾ
	last = first;

    ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ || first < 1
			   || first > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || last < first)
	ಪ್ರತ್ಯಾಗಮ;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    ಯದಿ (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    ಯದಿ (last > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	last = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಎಣಿಕೆ = last - first + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    ಯದಿ (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    ಯದಿ (u_save(first - 1, last + 1) == FAIL)
	ಗಚ್ಛ cleanup;

    ಪ್ರತಿ (lnum = first; lnum <= last; ++lnum)
	ml_delete_flags(first, ML_DEL_MESSAGE);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಯದಿ (wp->w_buffer == buf)
	{
	    ಯದಿ (wp->w_cursor.lnum > last)
		wp->w_cursor.lnum -= ಎಣಿಕೆ;
	    ಅನ್ಯಥಾ ಯದಿ (wp->w_cursor.lnum > first)
		wp->w_cursor.lnum = first;
	    ಯದಿ (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    wp->w_valid = 0;
	    ಯದಿ (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_col();
    deleted_ಸಾಲುಗಳು_mark(first, ಎಣಿಕೆ);
    rettv->vval.v_ಸಂಖ್ಯೆ = 0; // OK

cleanup:
    ಯದಿ (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    ಸ್ಥಿರ dict_T *
get_buffer_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    ಯದಿ (dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    dict_add_ಸಂಖ್ಯೆ(dict, "bufnr", buf->b_fnum);
    dict_add_string(dict, "name", buf->b_ffname);
    dict_add_ಸಂಖ್ಯೆ(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_add_ಸಂಖ್ಯೆ(dict, "linecount", buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
    dict_add_ಸಂಖ್ಯೆ(dict, "loaded", buf->b_ml.ml_mfp != ರಿಕ್ತ);
    dict_add_ಸಂಖ್ಯೆ(dict, "listed", buf->b_p_bl);
    dict_add_ಸಂಖ್ಯೆ(dict, "changed", bufIsChanged(buf));
    dict_add_ಸಂಖ್ಯೆ(dict, "changedtick", CHANGEDTICK(buf));
    dict_add_ಸಂಖ್ಯೆ(dict, "hidden",
			    buf->b_ml.ml_mfp != ರಿಕ್ತ && buf->b_nwindows == 0);
    dict_add_ಸಂಖ್ಯೆ(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_add_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    ಯದಿ (windows != ರಿಕ್ತ)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಯದಿ (wp->w_buffer == buf)
		list_append_ಸಂಖ್ಯೆ(windows, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    ಯದಿ (windows != ರಿಕ್ತ)
    {
	FOR_ALL_POPUPWINS(wp)
	    ಯದಿ (wp->w_buffer == buf)
		list_append_ಸಂಖ್ಯೆ(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		ಯದಿ (wp->w_buffer == buf)
		    list_append_ಸಂಖ್ಯೆ(windows, (varnumber_T)wp->w_id);

	dict_add_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    ಯದಿ (buf->b_signlist != ರಿಕ್ತ)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	ಯದಿ (signs != ರಿಕ್ತ)
	{
	    get_buffer_signs(buf, signs);
	    dict_add_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_add_ಸಂಖ್ಯೆ(dict, "lastused", buf->b_last_used);
# endif

    ಪ್ರತ್ಯಾಗಮ dict;
}

/*
 * "getbufinfo()" function
 */
    ಶೂನ್ಯ
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ರಿಕ್ತ;
    buf_T	*argbuf = ರಿಕ್ತ;
    dict_T	*d;
    ಪೂರ್ಣಾಙ್ಕ		filtered = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		sel_buflisted = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		sel_bufloaded = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		sel_bufmodified = FALSE;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && check_for_opt_buffer_or_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    // List of all the buffers or selected buffers
    ಯದಿ (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	ಯದಿ (sel_d != ರಿಕ್ತ)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	ಯದಿ (argbuf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	ಯದಿ (argbuf != ರಿಕ್ತ && argbuf != buf)
	    ಅನುವರ್ತ;
	ಯದಿ (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == ರಿಕ್ತ)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_changed)))
	    ಅನುವರ್ತ;

	d = get_buffer_info(buf);
	ಯದಿ (d != ರಿಕ್ತ)
	    list_append_dict(rettv->vval.v_list, d);
	ಯದಿ (argbuf != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_buffer_ಸಾಲುಗಳು(
    buf_T	*buf,
    linenr_T	start,
    linenr_T	end,
    ಪೂರ್ಣಾಙ್ಕ		retlist,
    typval_T	*rettv)
{
    ಯದಿ (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ರಿಕ್ತ;
    }

    ಯದಿ (buf == ರಿಕ್ತ || buf->b_ml.ml_mfp == ರಿಕ್ತ || start < 0)
    {
	ಯದಿ (retlist)
	    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (!retlist)
    {
	ಯದಿ (start >= 1 && start <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, start, FALSE),
		    ml_get_buf_len(buf, start));
	ಅನ್ಯಥಾ
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    ಅನ್ಯಥಾ
    {
	list_T		*l;
	listitem_T	*li;

	start = MAX(start, 1);
	end = MIN(end, buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
	ಯದಿ (end < start)
	    ಗಚ್ಛ nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_ವಸ್ತುಗಳು((ಪೂರ್ಣಾಙ್ಕ)(end - start + 1));
	ಯದಿ (l == ರಿಕ್ತ)
	    ಗಚ್ಛ nolist;
	rettv_list_set(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	ಪ್ರತಿ (li = l->lv_first; start <= end; ++start, li = li->li_next)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, start, FALSE),
						   ml_get_buf_len(buf, start));
	}
    }
    ಪ್ರತ್ಯಾಗಮ;

nolist:
    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
getbufline(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ retlist)
{
    linenr_T	lnum = 1;
    linenr_T	end = 1;
    buf_T	*buf;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;

    ಯದಿ (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ಯದಿ (buf != ರಿಕ್ತ)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	ಯದಿ (did_emsg > did_emsg_before)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (argvars[2].v_type == VAR_UNKNOWN)
	    end = lnum;
	ಅನ್ಯಥಾ
	    end = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_buffer_ಸಾಲುಗಳು(buf, lnum, end, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    ಶೂನ್ಯ
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    ಶೂನ್ಯ
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    ಶೂನ್ಯ
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	end;
    ಪೂರ್ಣಾಙ್ಕ		retlist;

    ಯದಿ (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    lnum = tv_get_lnum(argvars);
    ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = 0;
	retlist = FALSE;
    }
    ಅನ್ಯಥಾ
    {
	end = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_buffer_ಸಾಲುಗಳು(curbuf, lnum, end, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    ಶೂನ್ಯ
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_ಸಾಲು(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    ಶೂನ್ಯ
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;

    ಯದಿ (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    lnum = tv_get_lnum(&argvars[0]);
    ಯದಿ (did_emsg == did_emsg_before)
	set_buffer_ಸಾಲುಗಳು(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#endif  // FEAT_EVAL

#ಯದಿ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    ಶೂನ್ಯ
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
    ಶೂನ್ಯ
restore_buffer(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    ಯದಿ (bufref_valid(save_curbuf))
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
find_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	ಯದಿ ((*wp)->w_buffer == buf)
	    ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    ಶೂನ್ಯ
switch_to_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    ಯದಿ (find_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_buffer(save_curbuf, buf);
    ಅನ್ಯಥಾ ಯದಿ (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_buffer(save_curbuf, buf);
    }
}

    ಶೂನ್ಯ
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    ಯದಿ (save_curbuf->br_buf == ರಿಕ್ತ)
	restore_win(switchwin, TRUE);
    ಅನ್ಯಥಾ
	restore_buffer(save_curbuf);
}
#endif
