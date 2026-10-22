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

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_buffers(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	ಗಾಗಿ (lnr = bp->b_listener; !abort && lnr != ಖಾಲಿ; lnr = lnr->lr_ಮುಂದಿನ)
	    abort = abort || ಗಣ_ref_ಒಳ_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_ಕೆಲಸ_CHANNEL
	ಆದರೆ (!abort)
	    abort = abort || ಗಣ_ref_ಒಳ_callback(&bp->b_prompt_callback, copyID);
	ಆದರೆ (!abort)
	    abort = abort || ಗಣ_ref_ಒಳ_callback(&bp->b_prompt_ಅಡಚಣೆ, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	ಆದರೆ (!abort)
	    abort = abort || ಗಣ_ref_ಒಳ_callback(&bp->b_cfu_cb, copyID);
	ಆದರೆ (!abort)
	    abort = abort || ಗಣ_ref_ಒಳ_callback(&bp->b_ofu_cb, copyID);
	ಆದರೆ (!abort)
	    abort = abort || ಗಣ_ref_ಒಳ_callback(&bp->b_tsrfu_cb, copyID);
	ಆದರೆ (!abort && bp->b_p_cpt_cb != ಖಾಲಿ)
	    abort = abort || ಗಣ_ref_ಒಳ_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_ಎಣಿಕೆ, copyID);
# endif
	ಆದರೆ (!abort)
	    abort = abort || ಗಣ_ref_ಒಳ_callback(&bp->b_tfu_cb, copyID);
	ಆದರೆ (!abort)
	    abort = abort || ಗಣ_ref_ಒಳ_callback(&bp->b_ffu_cb, copyID);
	ಆದರೆ (abort)
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ abort;
}

    buf_T *
buflist_ಕಂಡುಹಿಡಿ_by_name(char_u *name, ಪೂರ್ಣಾಂಕ curtab_only)
{
    ಪೂರ್ಣಾಂಕ		save_magic;
    char_u	*save_cpo;
    buf_T	*buf;

    // Ignore 'magic' and 'cpoptions' here to make scripts portable
    save_magic = p_magic;
    p_magic = TRUE;
    save_cpo = p_cpo;
    p_cpo = ಖಾಲಿಗಳು_option;

    buf = buflist_findnr(buflist_findpat(name, name + STRLEN(name),
						    TRUE, FALSE, curtab_only));

    p_magic = save_magic;
    p_cpo = save_cpo;
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
ಕಂಡುಹಿಡಿ_ಬಫರ್(typval_T *avar)
{
    buf_T	*buf = ಖಾಲಿ;

    ಆದರೆ (avar->v_type == VAR_ಸಂಖ್ಯೆ)
	buf = buflist_findnr((ಪೂರ್ಣಾಂಕ)avar->vval.v_ಸಂಖ್ಯೆ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (avar->v_type == VAR_STRING && avar->vval.v_string != ಖಾಲಿ)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	ಆದರೆ (buf == ಖಾಲಿ)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		ಆದರೆ (buf->b_fname != ಖಾಲಿ
			&& (ಮಾರ್ಗ_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    ನಿಲ್ಲಿಸು;
	}
    }
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_win_for_curbuf(ಶೂನ್ಯ)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	ಆದರೆ (wip->wi_win != ಖಾಲಿ && wip->wi_win->w_ಬಫರ್ == curbuf)
	{
	    curwin = wip->wi_win;
	    ನಿಲ್ಲಿಸು;
	}
    }
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    ಪೂರ್ಣಾಂಕ		cob_using_aco;
    ಪೂರ್ಣಾಂಕ		cob_save_VIsual_ಸಕ್ರಿಯ;
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
change_other_ಬಫರ್_prepare(cob_T *cob, buf_T *buf)
{
    CLEAR_ಸೂಚಕ(cob);

    // Set "curbuf" to the buffer being changed.  Then make sure there is a
    // window for it to handle any side effects.
    cob->cob_save_VIsual_ಸಕ್ರಿಯ = VIsual_ಸಕ್ರಿಯ;
    VIsual_ಸಕ್ರಿಯ = FALSE;
    cob->cob_curwin_save = curwin;
    curbuf = buf;
    ಕಂಡುಹಿಡಿ_win_for_curbuf();  // simplest: find existing window for "buf"

    ಆದರೆ (curwin->w_ಬಫರ್ != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_ಬಫರ್;
	aucmd_prepbuf(&cob->cob_aco, buf);
	ಆದರೆ (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
change_other_ಬಫರ್_restore(cob_T *cob)
{
    ಆದರೆ (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	curwin = cob->cob_curwin_save;
	curbuf = curwin->w_ಬಫರ್;
    }
    VIsual_ಸಕ್ರಿಯ = cob->cob_save_VIsual_ಸಕ್ರಿಯ;
}

/*
 * Set line or list of lines in buffer "buf" to "lines".
 * Any type is allowed and converted to a string.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_ಬಫರ್_ಸಾಲುಗಳು(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	ಪೂರ್ಣಾಂಕ	    append,
	typval_T    *ಸಾಲುಗಳು,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*ಸಾಲು = ಖಾಲಿ;
    list_T	*l = ಖಾಲಿ;
    listitem_T	*li = ಖಾಲಿ;
    ದೀರ್ಘ	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    ಪೂರ್ಣಾಂಕ is_curbuf = buf == curbuf;
    ಆದರೆ (buf == ಖಾಲಿ || (!is_curbuf && buf->b_ml.ml_mfp == ಖಾಲಿ) || lnum < 1)
    {
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;	// FAIL
	ಆದರೆ (ಒಳ_vim9script() && lnum < 1)
	    semsg(_(e_invalid_ಸಾಲು_ಸಂಖ್ಯೆ_nr), lnum_arg);
	ಹಿಂತಿರುಗಿ;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    ಆದರೆ (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_ಬಫರ್_prepare(&cob, buf);

    ಆದರೆ (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    ಇಲ್ಲದಿದ್ದರೆ
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;

    ಆದರೆ (ಸಾಲುಗಳು->v_type == VAR_LIST)
    {
	l = ಸಾಲುಗಳು->vval.v_list;
	ಆದರೆ (l == ಖಾಲಿ || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    ಹೋಗು ಮುಗಿದ;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_ಮೊದಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಾಲು = typval_tostring(ಸಾಲುಗಳು, FALSE);

    // default result is zero == OK
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (l != ಖಾಲಿ)
	{
	    // list argument, get next string
	    ಆದರೆ (li == ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	    vim_free(ಸಾಲು);
	    ಸಾಲು = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_ಮುಂದಿನ;
	}

	rettv->vval.v_ಸಂಖ್ಯೆ = 1;	// FAIL
	ಆದರೆ (ಸಾಲು == ಖಾಲಿ || lnum > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1)
	    ನಿಲ್ಲಿಸು;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	ಆದರೆ (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	ಆದರೆ (!append && lnum <= curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    ಆದರೆ (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, ಸಾಲು, (colnr_T)STRLEN(ಸಾಲು) + 1, TRUE, TRUE) == OK)
	    {
		ಬದಲಾಯಿತು_ಬೈಟುಗಳು(lnum, 0);
		ಆದರೆ (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_ಲಂಬಸಾಲು();
		rettv->vval.v_ಸಂಖ್ಯೆ = 0;	// OK
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    ಆದರೆ (ml_append(lnum - 1, ಸಾಲು, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_ಸಂಖ್ಯೆ = 0;	// OK
	}

	ಆದರೆ (l == ಖಾಲಿ)			// only one string argument
	    ನಿಲ್ಲಿಸು;
	++lnum;
    }
    vim_free(ಸಾಲು);

    ಆದರೆ (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_ಸಾಲುಗಳು_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಆದರೆ (wp->w_ಬಫರ್ == buf
		    && (wp->w_ಬಫರ್ != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_ಲಂಬಸಾಲು();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	ಆದರೆ (curwin->w_ಬಫರ್ == curbuf)
	    update_topline();
    }

ಮುಗಿದ:
    ಆದರೆ (!is_curbuf)
	change_other_ಬಫರ್_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    ಶೂನ್ಯ
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;

    ಆದರೆ (ಒಳ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    lnum = tv_get_lnum(&argvars[0]);
    ಆದರೆ (did_emsg == did_emsg_before)
	ಗಣ_ಬಫರ್_ಸಾಲುಗಳು(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
buf_ಗಣ_append_ಸಾಲು(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ append)
{
    linenr_T	lnum;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_ಸಂಖ್ಯೆ_or_list_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ಆದರೆ (buf == ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = 1; // FAIL
    ಇಲ್ಲದಿದ್ದರೆ
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	ಆದರೆ (did_emsg == did_emsg_before)
	    ಗಣ_ಬಫರ್_ಸಾಲುಗಳು(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    ಶೂನ್ಯ
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_ಗಣ_append_ಸಾಲು(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    ಶೂನ್ಯ
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = buflist_ಕೂಡಿಸು(*name == NUL ? ಖಾಲಿ : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    ಶೂನ್ಯ
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = (ಕಂಡುಹಿಡಿ_ಬಫರ್(&argvars[0]) != ಖಾಲಿ);
}

/*
 * "buflisted(expr)" function
 */
    ಶೂನ್ಯ
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಕಂಡುಹಿಡಿ_ಬಫರ್(&argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = (buf != ಖಾಲಿ && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    ಶೂನ್ಯ
f_bufload(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    buf_T	*buf;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = get_buf_arg(&argvars[0]);
    ಆದರೆ (buf != ಖಾಲಿ)
	ಬಫರ್_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    ಶೂನ್ಯ
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = ಕಂಡುಹಿಡಿ_ಬಫರ್(&argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = (buf != ಖಾಲಿ && buf->b_ml.ml_mfp != ಖಾಲಿ);
}

/*
 * "bufname(expr)" function
 */
    ಶೂನ್ಯ
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    ಇಲ್ಲದಿದ್ದರೆ
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    ಆದರೆ (buf != ಖಾಲಿ && buf->b_fname != ಖಾಲಿ)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_string = ಖಾಲಿ;
}

/*
 * "bufnr(expr)" function
 */
    ಶೂನ್ಯ
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		error = FALSE;
    char_u	*name;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_opt_ಬಫರ್_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    ಇಲ್ಲದಿದ್ದರೆ
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    ಆದರೆ (buf == ಖಾಲಿ
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != ಖಾಲಿ
	    && !error)
	buf = buflist_new(name, ಖಾಲಿ, (linenr_T)1, 0);

    ಆದರೆ (buf != ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = buf->b_fnum;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
buf_win_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ get_nr)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		winnr = 0;
    buf_T	*buf;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಬಫರ್_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	ಆದರೆ (wp->w_ಬಫರ್ == buf)
	    ನಿಲ್ಲಿಸು;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = (wp != ಖಾಲಿ ? (get_nr ? winnr : wp->w_id) : -1);
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
    linenr_T	ಮೊದಲ, ಕೊನೆಯ;
    linenr_T	lnum;
    ದೀರ್ಘ	ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;

    rettv->vval.v_ಸಂಖ್ಯೆ = 1;	// FAIL by default

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಮೊದಲ = tv_get_lnum_buf(&argvars[1], buf);
    ಆದರೆ (did_emsg > did_emsg_before)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
	ಕೊನೆಯ = tv_get_lnum_buf(&argvars[2], buf);
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೊನೆಯ = ಮೊದಲ;

    ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ || ಮೊದಲ < 1
			   || ಮೊದಲ > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || ಕೊನೆಯ < ಮೊದಲ)
	ಹಿಂತಿರುಗಿ;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    ಆದರೆ (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_ಬಫರ್_prepare(&cob, buf);

    ಆದರೆ (ಕೊನೆಯ > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	ಕೊನೆಯ = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    ಎಣಿಕೆ_2 = ಕೊನೆಯ - ಮೊದಲ + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    ಆದರೆ (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    ಆದರೆ (u_save(ಮೊದಲ - 1, ಕೊನೆಯ + 1) == FAIL)
	ಹೋಗು cleanup;

    ಗಾಗಿ (lnum = ಮೊದಲ; lnum <= ಕೊನೆಯ; ++lnum)
	ml_delete_flags(ಮೊದಲ, ML_DEL_ಸಂದೇಶ);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಆದರೆ (wp->w_ಬಫರ್ == buf)
	{
	    ಆದರೆ (wp->w_cursor.lnum > ಕೊನೆಯ)
		wp->w_cursor.lnum -= ಎಣಿಕೆ_2;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_cursor.lnum > ಮೊದಲ)
		wp->w_cursor.lnum = ಮೊದಲ;
	    ಆದರೆ (wp->w_cursor.lnum > wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		wp->w_cursor.lnum = wp->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	    wp->w_valid = 0;
	    ಆದರೆ (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_ಲಂಬಸಾಲು();
    deleted_ಸಾಲುಗಳು_mark(ಮೊದಲ, ಎಣಿಕೆ_2);
    rettv->vval.v_ಸಂಖ್ಯೆ = 0; // OK

cleanup:
    ಆದರೆ (!is_curbuf)
	change_other_ಬಫರ್_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    ಸ್ಥಿರ dict_T *
get_ಬಫರ್_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "bufnr", buf->b_fnum);
    dict_ಕೂಡಿಸು_string(dict, "name", buf->b_ffname);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "linecount", buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "loaded", buf->b_ml.ml_mfp != ಖಾಲಿ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "listed", buf->b_p_bl);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "changed", buf_Is_ಬದಲಾಯಿತು(buf));
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "changedtick", CHANGEDTICK(buf));
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "hidden",
			    buf->b_ml.ml_mfp != ಖಾಲಿ && buf->b_nwindows == 0);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_ಕೂಡಿಸು_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    ಆದರೆ (windows != ಖಾಲಿ)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಆದರೆ (wp->w_ಬಫರ್ == buf)
		list_append_ಸಂಖ್ಯೆ(windows, (varnumber_T)wp->w_id);
	dict_ಕೂಡಿಸು_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    ಆದರೆ (windows != ಖಾಲಿ)
    {
	FOR_ALL_POPUPWINS(wp)
	    ಆದರೆ (wp->w_ಬಫರ್ == buf)
		list_append_ಸಂಖ್ಯೆ(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, wp)
		ಆದರೆ (wp->w_ಬಫರ್ == buf)
		    list_append_ಸಂಖ್ಯೆ(windows, (varnumber_T)wp->w_id);

	dict_ಕೂಡಿಸು_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    ಆದರೆ (buf->b_signlist != ಖಾಲಿ)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	ಆದರೆ (signs != ಖಾಲಿ)
	{
	    get_ಬಫರ್_signs(buf, signs);
	    dict_ಕೂಡಿಸು_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "lastused", buf->b_ಕೊನೆಯ_ಬಳಸಿದ);
# endif

    ಹಿಂತಿರುಗಿ dict;
}

/*
 * "getbufinfo()" function
 */
    ಶೂನ್ಯ
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ಖಾಲಿ;
    buf_T	*argbuf = ಖಾಲಿ;
    dict_T	*d;
    ಪೂರ್ಣಾಂಕ		filtered = FALSE;
    ಪೂರ್ಣಾಂಕ		sel_buflisted = FALSE;
    ಪೂರ್ಣಾಂಕ		sel_bufloaded = FALSE;
    ಪೂರ್ಣಾಂಕ		sel_bufmodified = FALSE;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && check_for_opt_ಬಫರ್_or_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    // List of all the buffers or selected buffers
    ಆದರೆ (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	ಆದರೆ (sel_d != ಖಾಲಿ)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	ಆದರೆ (argbuf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	ಆದರೆ (argbuf != ಖಾಲಿ && argbuf != buf)
	    ಮುಂದುವರಿ;
	ಆದರೆ (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == ಖಾಲಿ)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_ಬದಲಾಯಿತು)))
	    ಮುಂದುವರಿ;

	d = get_ಬಫರ್_info(buf);
	ಆದರೆ (d != ಖಾಲಿ)
	    list_append_dict(rettv->vval.v_list, d);
	ಆದರೆ (argbuf != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ಬಫರ್_ಸಾಲುಗಳು(
    buf_T	*buf,
    linenr_T	ಆರಂಭ,
    linenr_T	ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		retlist,
    typval_T	*rettv)
{
    ಆದರೆ (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ಖಾಲಿ;
    }

    ಆದರೆ (buf == ಖಾಲಿ || buf->b_ml.ml_mfp == ಖಾಲಿ || ಆರಂಭ < 0)
    {
	ಆದರೆ (retlist)
	    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (!retlist)
    {
	ಆದರೆ (ಆರಂಭ >= 1 && ಆರಂಭ <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, ಆರಂಭ, FALSE),
		    ml_get_buf_len(buf, ಆರಂಭ));
	ಇಲ್ಲದಿದ್ದರೆ
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	list_T		*l;
	listitem_T	*li;

	ಆರಂಭ = MAX(ಆರಂಭ, 1);
	ಕೊನೆ = MIN(ಕೊನೆ, buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
	ಆದರೆ (ಕೊನೆ < ಆರಂಭ)
	    ಹೋಗು nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_ವಸ್ತುಗಳು((ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - ಆರಂಭ + 1));
	ಆದರೆ (l == ಖಾಲಿ)
	    ಹೋಗು nolist;
	rettv_list_ಗಣ(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	ಗಾಗಿ (li = l->lv_ಮೊದಲ; ಆರಂಭ <= ಕೊನೆ; ++ಆರಂಭ, li = li->li_ಮುಂದಿನ)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, ಆರಂಭ, FALSE),
						   ml_get_buf_len(buf, ಆರಂಭ));
	}
    }
    ಹಿಂತಿರುಗಿ;

nolist:
    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
getbufline(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ retlist)
{
    linenr_T	lnum = 1;
    linenr_T	ಕೊನೆ = 1;
    buf_T	*buf;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಬಫರ್_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ಆದರೆ (buf != ಖಾಲಿ)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	ಆದರೆ (did_emsg > did_emsg_before)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (argvars[2].v_type == VAR_UNKNOWN)
	    ಕೊನೆ = lnum;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೊನೆ = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_ಬಫರ್_ಸಾಲುಗಳು(buf, lnum, ಕೊನೆ, retlist, rettv);
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
    linenr_T	ಕೊನೆ;
    ಪೂರ್ಣಾಂಕ		retlist;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    lnum = tv_get_lnum(argvars);
    ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
    {
	ಕೊನೆ = 0;
	retlist = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಕೊನೆ = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_ಬಫರ್_ಸಾಲುಗಳು(curbuf, lnum, ಕೊನೆ, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    ಶೂನ್ಯ
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_ಗಣ_append_ಸಾಲು(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    ಶೂನ್ಯ
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;

    ಆದರೆ (ಒಳ_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    lnum = tv_get_lnum(&argvars[0]);
    ಆದರೆ (did_emsg == did_emsg_before)
	ಗಣ_ಬಫರ್_ಸಾಲುಗಳು(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#ಆದರೆ_ಅಂತ್ಯ  // FEAT_EVAL

#ಆದರೆ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    ಶೂನ್ಯ
switch_ಬಫರ್(bufref_T *save_curbuf, buf_T *buf)
{
    ಖಂಡ_autocmds();
# ifdef FEAT_FOLDING
    ++disable_fold_update;
# endif
    ಗಣ_bufref(save_curbuf, curbuf);
    --curbuf->b_nwindows;
    curbuf = buf;
    curwin->w_ಬಫರ್ = buf;
    ++curbuf->b_nwindows;
}

/*
 * Restore the current buffer after using switch_buffer().
 */
    ಶೂನ್ಯ
restore_ಬಫರ್(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    ಆದರೆ (bufref_valid(save_curbuf))
    {
	--curbuf->b_nwindows;
	curwin->w_ಬಫರ್ = save_curbuf->br_buf;
	curbuf = save_curbuf->br_buf;
	++curbuf->b_nwindows;
    }
}

/*
 * Find a window for buffer "buf".
 * If found OK is returned and "wp" and "tp" are set to the window and tabpage.
 * If not found FAIL is returned.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕಂಡುಹಿಡಿ_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	ಆದರೆ ((*wp)->w_ಬಫರ್ == buf)
	    ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    ಶೂನ್ಯ
switch_ಗಮ್ಯ_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    ಆದರೆ (ಕಂಡುಹಿಡಿ_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_ಬಫರ್(save_curbuf, buf);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_ಬಫರ್(save_curbuf, buf);
    }
}

    ಶೂನ್ಯ
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    ಆದರೆ (save_curbuf->br_buf == ಖಾಲಿ)
	restore_win(switchwin, TRUE);
    ಇಲ್ಲದಿದ್ದರೆ
	restore_ಬಫರ್(save_curbuf);
}
#ಆದರೆ_ಅಂತ್ಯ
