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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_buffers(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	కొరకు (lnr = bp->b_listener; !abort && lnr != ఖాళీ; lnr = lnr->lr_తదుపరి)
	    abort = abort || సమితి_ref_లోపల_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_పని_CHANNEL
	ఒకవేళ (!abort)
	    abort = abort || సమితి_ref_లోపల_callback(&bp->b_prompt_callback, copyID);
	ఒకవేళ (!abort)
	    abort = abort || సమితి_ref_లోపల_callback(&bp->b_prompt_అంతరాయం, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	ఒకవేళ (!abort)
	    abort = abort || సమితి_ref_లోపల_callback(&bp->b_cfu_cb, copyID);
	ఒకవేళ (!abort)
	    abort = abort || సమితి_ref_లోపల_callback(&bp->b_ofu_cb, copyID);
	ఒకవేళ (!abort)
	    abort = abort || సమితి_ref_లోపల_callback(&bp->b_tsrfu_cb, copyID);
	ఒకవేళ (!abort && bp->b_p_cpt_cb != ఖాళీ)
	    abort = abort || సమితి_ref_లోపల_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_లెక్క, copyID);
# endif
	ఒకవేళ (!abort)
	    abort = abort || సమితి_ref_లోపల_callback(&bp->b_tfu_cb, copyID);
	ఒకవేళ (!abort)
	    abort = abort || సమితి_ref_లోపల_callback(&bp->b_ffu_cb, copyID);
	ఒకవేళ (abort)
	    ఆపు;
    }
    తిరిగి abort;
}

    buf_T *
buflist_కనుగొను_by_name(char_u *name, పూర్ణాంకం curtab_only)
{
    పూర్ణాంకం		save_magic;
    char_u	*save_cpo;
    buf_T	*buf;

    // Ignore 'magic' and 'cpoptions' here to make scripts portable
    save_magic = p_magic;
    p_magic = TRUE;
    save_cpo = p_cpo;
    p_cpo = ఖాళీలు_option;

    buf = buflist_findnr(buflist_findpat(name, name + STRLEN(name),
						    TRUE, FALSE, curtab_only));

    p_magic = save_magic;
    p_cpo = save_cpo;
    తిరిగి buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
కనుగొను_బఫరు(typval_T *avar)
{
    buf_T	*buf = ఖాళీ;

    ఒకవేళ (avar->v_type == VAR_సంఖ్య)
	buf = buflist_findnr((పూర్ణాంకం)avar->vval.v_సంఖ్య);
    లేకపోతే ఒకవేళ (లోపల_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	తిరిగి ఖాళీ;
    లేకపోతే ఒకవేళ (avar->v_type == VAR_STRING && avar->vval.v_string != ఖాళీ)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	ఒకవేళ (buf == ఖాళీ)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		ఒకవేళ (buf->b_fname != ఖాళీ
			&& (మార్గం_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    ఆపు;
	}
    }
    తిరిగి buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    స్థిర శూన్య
కనుగొను_win_for_curbuf(శూన్య)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	ఒకవేళ (wip->wi_win != ఖాళీ && wip->wi_win->w_బఫరు == curbuf)
	{
	    curwin = wip->wi_win;
	    ఆపు;
	}
    }
}

రకనిర్వచనం నిర్మాణం {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    పూర్ణాంకం		cob_using_aco;
    పూర్ణాంకం		cob_save_VIsual_చురుకైన;
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
    స్థిర శూన్య
change_other_బఫరు_prepare(cob_T *cob, buf_T *buf)
{
    CLEAR_సూచిక(cob);

    // Set "curbuf" to the buffer being changed.  Then make sure there is a
    // window for it to handle any side effects.
    cob->cob_save_VIsual_చురుకైన = VIsual_చురుకైన;
    VIsual_చురుకైన = FALSE;
    cob->cob_curwin_save = curwin;
    curbuf = buf;
    కనుగొను_win_for_curbuf();  // simplest: find existing window for "buf"

    ఒకవేళ (curwin->w_బఫరు != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_బఫరు;
	aucmd_prepbuf(&cob->cob_aco, buf);
	ఒకవేళ (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    స్థిర శూన్య
change_other_బఫరు_restore(cob_T *cob)
{
    ఒకవేళ (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    లేకపోతే
    {
	curwin = cob->cob_curwin_save;
	curbuf = curwin->w_బఫరు;
    }
    VIsual_చురుకైన = cob->cob_save_VIsual_చురుకైన;
}

/*
 * Set line or list of lines in buffer "buf" to "lines".
 * Any type is allowed and converted to a string.
 */
    స్థిర శూన్య
సమితి_బఫరు_పంక్తులు(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	పూర్ణాంకం	    append,
	typval_T    *పంక్తులు,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*పంక్తి = ఖాళీ;
    list_T	*l = ఖాళీ;
    listitem_T	*li = ఖాళీ;
    దీర్ఘ	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    పూర్ణాంకం is_curbuf = buf == curbuf;
    ఒకవేళ (buf == ఖాళీ || (!is_curbuf && buf->b_ml.ml_mfp == ఖాళీ) || lnum < 1)
    {
	rettv->vval.v_సంఖ్య = 1;	// FAIL
	ఒకవేళ (లోపల_vim9script() && lnum < 1)
	    semsg(_(e_invalid_పంక్తి_సంఖ్య_nr), lnum_arg);
	తిరిగి;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    ఒకవేళ (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_బఫరు_prepare(&cob, buf);

    ఒకవేళ (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    లేకపోతే
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_పంక్తి_లెక్క;

    ఒకవేళ (పంక్తులు->v_type == VAR_LIST)
    {
	l = పంక్తులు->vval.v_list;
	ఒకవేళ (l == ఖాళీ || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    వెళ్ళు పూర్తి;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_మొదటి;
    }
    లేకపోతే
	పంక్తి = typval_tostring(పంక్తులు, FALSE);

    // default result is zero == OK
    కొరకు (;;)
    {
	ఒకవేళ (l != ఖాళీ)
	{
	    // list argument, get next string
	    ఒకవేళ (li == ఖాళీ)
		ఆపు;
	    vim_free(పంక్తి);
	    పంక్తి = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_తదుపరి;
	}

	rettv->vval.v_సంఖ్య = 1;	// FAIL
	ఒకవేళ (పంక్తి == ఖాళీ || lnum > curbuf->b_ml.ml_పంక్తి_లెక్క + 1)
	    ఆపు;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	ఒకవేళ (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	ఒకవేళ (!append && lnum <= curbuf->b_ml.ml_పంక్తి_లెక్క)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    ఒకవేళ (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, పంక్తి, (colnr_T)STRLEN(పంక్తి) + 1, TRUE, TRUE) == OK)
	    {
		మారింది_బైట్లు(lnum, 0);
		ఒకవేళ (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_నిలువు();
		rettv->vval.v_సంఖ్య = 0;	// OK
	    }
	}
	లేకపోతే ఒకవేళ (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    ఒకవేళ (ml_append(lnum - 1, పంక్తి, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_సంఖ్య = 0;	// OK
	}

	ఒకవేళ (l == ఖాళీ)			// only one string argument
	    ఆపు;
	++lnum;
    }
    vim_free(పంక్తి);

    ఒకవేళ (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_పంక్తులు_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ఒకవేళ (wp->w_బఫరు == buf
		    && (wp->w_బఫరు != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_నిలువు();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	ఒకవేళ (curwin->w_బఫరు == curbuf)
	    update_topline();
    }

పూర్తి:
    ఒకవేళ (!is_curbuf)
	change_other_బఫరు_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    శూన్య
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    పూర్ణాంకం		did_emsg_before = did_emsg;

    ఒకవేళ (లోపల_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	తిరిగి;

    lnum = tv_get_lnum(&argvars[0]);
    ఒకవేళ (did_emsg == did_emsg_before)
	సమితి_బఫరు_పంక్తులు(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    స్థిర శూన్య
buf_సమితి_append_పంక్తి(typval_T *argvars, typval_T *rettv, పూర్ణాంకం append)
{
    linenr_T	lnum;
    buf_T	*buf;
    పూర్ణాంకం		did_emsg_before = did_emsg;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_సంఖ్య_or_list_arg(argvars, 2) == FAIL))
	తిరిగి;

    buf = tv_get_buf(&argvars[0], FALSE);
    ఒకవేళ (buf == ఖాళీ)
	rettv->vval.v_సంఖ్య = 1; // FAIL
    లేకపోతే
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	ఒకవేళ (did_emsg == did_emsg_before)
	    సమితి_బఫరు_పంక్తులు(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    శూన్య
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_సమితి_append_పంక్తి(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    శూన్య
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_సంఖ్య = buflist_కూడు(*name == NUL ? ఖాళీ : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    శూన్య
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = (కనుగొను_బఫరు(&argvars[0]) != ఖాళీ);
}

/*
 * "buflisted(expr)" function
 */
    శూన్య
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = కనుగొను_బఫరు(&argvars[0]);
    rettv->vval.v_సంఖ్య = (buf != ఖాళీ && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    శూన్య
f_bufload(typval_T *argvars, typval_T *rettv వాడని)
{
    buf_T	*buf;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = get_buf_arg(&argvars[0]);
    ఒకవేళ (buf != ఖాళీ)
	బఫరు_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    శూన్య
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = కనుగొను_బఫరు(&argvars[0]);
    rettv->vval.v_సంఖ్య = (buf != ఖాళీ && buf->b_ml.ml_mfp != ఖాళీ);
}

/*
 * "bufname(expr)" function
 */
    శూన్య
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    ఒకవేళ (లోపల_vim9script() && check_for_opt_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    లేకపోతే
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    ఒకవేళ (buf != ఖాళీ && buf->b_fname != ఖాళీ)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    లేకపోతే
	rettv->vval.v_string = ఖాళీ;
}

/*
 * "bufnr(expr)" function
 */
    శూన్య
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    పూర్ణాంకం		error = FALSE;
    char_u	*name;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_opt_బఫరు_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    లేకపోతే
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    ఒకవేళ (buf == ఖాళీ
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != ఖాళీ
	    && !error)
	buf = buflist_new(name, ఖాళీ, (linenr_T)1, 0);

    ఒకవేళ (buf != ఖాళీ)
	rettv->vval.v_సంఖ్య = buf->b_fnum;
    లేకపోతే
	rettv->vval.v_సంఖ్య = -1;
}

    స్థిర శూన్య
buf_win_common(typval_T *argvars, typval_T *rettv, పూర్ణాంకం get_nr)
{
    win_T	*wp;
    పూర్ణాంకం		winnr = 0;
    buf_T	*buf;

    ఒకవేళ (లోపల_vim9script() && check_for_బఫరు_arg(argvars, 0) == FAIL)
	తిరిగి;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	ఒకవేళ (wp->w_బఫరు == buf)
	    ఆపు;
    }
    rettv->vval.v_సంఖ్య = (wp != ఖాళీ ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    శూన్య
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    శూన్య
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    శూన్య
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	మొదటి, చివరి;
    linenr_T	lnum;
    దీర్ఘ	లెక్క;
    పూర్ణాంకం		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    పూర్ణాంకం		did_emsg_before = did_emsg;

    rettv->vval.v_సంఖ్య = 1;	// FAIL by default

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	తిరిగి;

    buf = tv_get_buf(&argvars[0], FALSE);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    మొదటి = tv_get_lnum_buf(&argvars[1], buf);
    ఒకవేళ (did_emsg > did_emsg_before)
	తిరిగి;
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	చివరి = tv_get_lnum_buf(&argvars[2], buf);
    లేకపోతే
	చివరి = మొదటి;

    ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ || మొదటి < 1
			   || మొదటి > buf->b_ml.ml_పంక్తి_లెక్క || చివరి < మొదటి)
	తిరిగి;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    ఒకవేళ (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_బఫరు_prepare(&cob, buf);

    ఒకవేళ (చివరి > curbuf->b_ml.ml_పంక్తి_లెక్క)
	చివరి = curbuf->b_ml.ml_పంక్తి_లెక్క;
    లెక్క = చివరి - మొదటి + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    ఒకవేళ (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    ఒకవేళ (u_save(మొదటి - 1, చివరి + 1) == FAIL)
	వెళ్ళు cleanup;

    కొరకు (lnum = మొదటి; lnum <= చివరి; ++lnum)
	ml_delete_flags(మొదటి, ML_DEL_సందేశం);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ఒకవేళ (wp->w_బఫరు == buf)
	{
	    ఒకవేళ (wp->w_cursor.lnum > చివరి)
		wp->w_cursor.lnum -= లెక్క;
	    లేకపోతే ఒకవేళ (wp->w_cursor.lnum > మొదటి)
		wp->w_cursor.lnum = మొదటి;
	    ఒకవేళ (wp->w_cursor.lnum > wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
		wp->w_cursor.lnum = wp->w_బఫరు->b_ml.ml_పంక్తి_లెక్క;
	    wp->w_valid = 0;
	    ఒకవేళ (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_నిలువు();
    deleted_పంక్తులు_mark(మొదటి, లెక్క);
    rettv->vval.v_సంఖ్య = 0; // OK

cleanup:
    ఒకవేళ (!is_curbuf)
	change_other_బఫరు_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    స్థిర dict_T *
get_బఫరు_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    ఒకవేళ (dict == ఖాళీ)
	తిరిగి ఖాళీ;

    dict_కూడు_సంఖ్య(dict, "bufnr", buf->b_fnum);
    dict_కూడు_string(dict, "name", buf->b_ffname);
    dict_కూడు_సంఖ్య(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_కూడు_సంఖ్య(dict, "linecount", buf->b_ml.ml_పంక్తి_లెక్క);
    dict_కూడు_సంఖ్య(dict, "loaded", buf->b_ml.ml_mfp != ఖాళీ);
    dict_కూడు_సంఖ్య(dict, "listed", buf->b_p_bl);
    dict_కూడు_సంఖ్య(dict, "changed", buf_Is_మారింది(buf));
    dict_కూడు_సంఖ్య(dict, "changedtick", CHANGEDTICK(buf));
    dict_కూడు_సంఖ్య(dict, "hidden",
			    buf->b_ml.ml_mfp != ఖాళీ && buf->b_nwindows == 0);
    dict_కూడు_సంఖ్య(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_కూడు_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    ఒకవేళ (windows != ఖాళీ)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ఒకవేళ (wp->w_బఫరు == buf)
		list_append_సంఖ్య(windows, (varnumber_T)wp->w_id);
	dict_కూడు_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    ఒకవేళ (windows != ఖాళీ)
    {
	FOR_ALL_POPUPWINS(wp)
	    ఒకవేళ (wp->w_బఫరు == buf)
		list_append_సంఖ్య(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_లోపల_TAB(tp, wp)
		ఒకవేళ (wp->w_బఫరు == buf)
		    list_append_సంఖ్య(windows, (varnumber_T)wp->w_id);

	dict_కూడు_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    ఒకవేళ (buf->b_signlist != ఖాళీ)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	ఒకవేళ (signs != ఖాళీ)
	{
	    get_బఫరు_signs(buf, signs);
	    dict_కూడు_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_కూడు_సంఖ్య(dict, "lastused", buf->b_చివరి_వాడిన);
# endif

    తిరిగి dict;
}

/*
 * "getbufinfo()" function
 */
    శూన్య
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ఖాళీ;
    buf_T	*argbuf = ఖాళీ;
    dict_T	*d;
    పూర్ణాంకం		filtered = FALSE;
    పూర్ణాంకం		sel_buflisted = FALSE;
    పూర్ణాంకం		sel_bufloaded = FALSE;
    పూర్ణాంకం		sel_bufmodified = FALSE;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && check_for_opt_బఫరు_or_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    // List of all the buffers or selected buffers
    ఒకవేళ (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	ఒకవేళ (sel_d != ఖాళీ)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	ఒకవేళ (argbuf == ఖాళీ)
	    తిరిగి;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	ఒకవేళ (argbuf != ఖాళీ && argbuf != buf)
	    కొనసాగు;
	ఒకవేళ (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == ఖాళీ)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_మారింది)))
	    కొనసాగు;

	d = get_బఫరు_info(buf);
	ఒకవేళ (d != ఖాళీ)
	    list_append_dict(rettv->vval.v_list, d);
	ఒకవేళ (argbuf != ఖాళీ)
	    తిరిగి;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    స్థిర శూన్య
get_బఫరు_పంక్తులు(
    buf_T	*buf,
    linenr_T	ప్రారంభం,
    linenr_T	చివర,
    పూర్ణాంకం		retlist,
    typval_T	*rettv)
{
    ఒకవేళ (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ఖాళీ;
    }

    ఒకవేళ (buf == ఖాళీ || buf->b_ml.ml_mfp == ఖాళీ || ప్రారంభం < 0)
    {
	ఒకవేళ (retlist)
	    (శూన్య)rettv_list_alloc(rettv);
	తిరిగి;
    }

    ఒకవేళ (!retlist)
    {
	ఒకవేళ (ప్రారంభం >= 1 && ప్రారంభం <= buf->b_ml.ml_పంక్తి_లెక్క)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, ప్రారంభం, FALSE),
		    ml_get_buf_len(buf, ప్రారంభం));
	లేకపోతే
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    లేకపోతే
    {
	list_T		*l;
	listitem_T	*li;

	ప్రారంభం = MAX(ప్రారంభం, 1);
	చివర = MIN(చివర, buf->b_ml.ml_పంక్తి_లెక్క);
	ఒకవేళ (చివర < ప్రారంభం)
	    వెళ్ళు nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_వస్తువులు((పూర్ణాంకం)(చివర - ప్రారంభం + 1));
	ఒకవేళ (l == ఖాళీ)
	    వెళ్ళు nolist;
	rettv_list_సమితి(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	కొరకు (li = l->lv_మొదటి; ప్రారంభం <= చివర; ++ప్రారంభం, li = li->li_తదుపరి)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, ప్రారంభం, FALSE),
						   ml_get_buf_len(buf, ప్రారంభం));
	}
    }
    తిరిగి;

nolist:
    (శూన్య)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    స్థిర శూన్య
getbufline(typval_T *argvars, typval_T *rettv, పూర్ణాంకం retlist)
{
    linenr_T	lnum = 1;
    linenr_T	చివర = 1;
    buf_T	*buf;
    పూర్ణాంకం		did_emsg_before = did_emsg;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_బఫరు_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	తిరిగి;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ఒకవేళ (buf != ఖాళీ)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	ఒకవేళ (did_emsg > did_emsg_before)
	    తిరిగి;
	ఒకవేళ (argvars[2].v_type == VAR_UNKNOWN)
	    చివర = lnum;
	లేకపోతే
	    చివర = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_బఫరు_పంక్తులు(buf, lnum, చివర, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    శూన్య
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    శూన్య
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    శూన్య
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	చివర;
    పూర్ణాంకం		retlist;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	తిరిగి;

    lnum = tv_get_lnum(argvars);
    ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
    {
	చివర = 0;
	retlist = FALSE;
    }
    లేకపోతే
    {
	చివర = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_బఫరు_పంక్తులు(curbuf, lnum, చివర, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    శూన్య
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_సమితి_append_పంక్తి(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    శూన్య
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    పూర్ణాంకం		did_emsg_before = did_emsg;

    ఒకవేళ (లోపల_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	తిరిగి;

    lnum = tv_get_lnum(&argvars[0]);
    ఒకవేళ (did_emsg == did_emsg_before)
	సమితి_బఫరు_పంక్తులు(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#అయితే_ముగింపు  // FEAT_EVAL

#ఒకవేళ defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    శూన్య
switch_బఫరు(bufref_T *save_curbuf, buf_T *buf)
{
    ఖండం_autocmds();
# ifdef FEAT_FOLDING
    ++disable_fold_update;
# endif
    సమితి_bufref(save_curbuf, curbuf);
    --curbuf->b_nwindows;
    curbuf = buf;
    curwin->w_బఫరు = buf;
    ++curbuf->b_nwindows;
}

/*
 * Restore the current buffer after using switch_buffer().
 */
    శూన్య
restore_బఫరు(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    ఒకవేళ (bufref_valid(save_curbuf))
    {
	--curbuf->b_nwindows;
	curwin->w_బఫరు = save_curbuf->br_buf;
	curbuf = save_curbuf->br_buf;
	++curbuf->b_nwindows;
    }
}

/*
 * Find a window for buffer "buf".
 * If found OK is returned and "wp" and "tp" are set to the window and tabpage.
 * If not found FAIL is returned.
 */
    స్థిర పూర్ణాంకం
కనుగొను_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	ఒకవేళ ((*wp)->w_బఫరు == buf)
	    తిరిగి OK;
    తిరిగి FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    శూన్య
switch_గమ్యం_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    ఒకవేళ (కనుగొను_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_బఫరు(save_curbuf, buf);
    లేకపోతే ఒకవేళ (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_బఫరు(save_curbuf, buf);
    }
}

    శూన్య
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    ఒకవేళ (save_curbuf->br_buf == ఖాళీ)
	restore_win(switchwin, TRUE);
    లేకపోతే
	restore_బఫరు(save_curbuf);
}
#అయితే_ముగింపు
