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

#अन्तर्भाव "vim.h"

#जर defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    पूर्णांक
set_ref_in_buffers(पूर्णांक copyID)
{
    पूर्णांक		पात = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	साठी (lnr = bp->b_listener; !पात && lnr != रिक्त; lnr = lnr->lr_next)
	    पात = पात || set_ref_in_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_JOB_CHANNEL
	जर (!पात)
	    पात = पात || set_ref_in_callback(&bp->b_prompt_callback, copyID);
	जर (!पात)
	    पात = पात || set_ref_in_callback(&bp->b_prompt_interrupt, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	जर (!पात)
	    पात = पात || set_ref_in_callback(&bp->b_cfu_cb, copyID);
	जर (!पात)
	    पात = पात || set_ref_in_callback(&bp->b_ofu_cb, copyID);
	जर (!पात)
	    पात = पात || set_ref_in_callback(&bp->b_tsrfu_cb, copyID);
	जर (!पात && bp->b_p_cpt_cb != रिक्त)
	    पात = पात || set_ref_in_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_मोजणी, copyID);
# endif
	जर (!पात)
	    पात = पात || set_ref_in_callback(&bp->b_tfu_cb, copyID);
	जर (!पात)
	    पात = पात || set_ref_in_callback(&bp->b_ffu_cb, copyID);
	जर (पात)
	    थांबा;
    }
    परत पात;
}

    buf_T *
buflist_find_by_name(char_u *name, पूर्णांक curtab_only)
{
    पूर्णांक		save_magic;
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
    परत buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
find_buffer(typval_T *avar)
{
    buf_T	*buf = रिक्त;

    जर (avar->v_type == VAR_संख्या)
	buf = buflist_findnr((पूर्णांक)avar->vval.v_संख्या);
    अन्यथा जर (in_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	परत रिक्त;
    अन्यथा जर (avar->v_type == VAR_STRING && avar->vval.v_string != रिक्त)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	जर (buf == रिक्त)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		जर (buf->b_fname != रिक्त
			&& (path_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    थांबा;
	}
    }
    परत buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    स्थिर शून्य
find_win_for_curbuf(शून्य)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	जर (wip->wi_win != रिक्त && wip->wi_win->w_buffer == curbuf)
	{
	    curwin = wip->wi_win;
	    थांबा;
	}
    }
}

प्रकारव्याख्या संरचना {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    पूर्णांक		cob_using_aco;
    पूर्णांक		cob_save_VIsual_active;
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
    स्थिर शून्य
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

    जर (curwin->w_buffer != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_buffer;
	aucmd_prepbuf(&cob->cob_aco, buf);
	जर (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    स्थिर शून्य
change_other_buffer_restore(cob_T *cob)
{
    जर (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    अन्यथा
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
    स्थिर शून्य
set_buffer_ओळी(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	पूर्णांक	    append,
	typval_T    *ओळी,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*ओळ = रिक्त;
    list_T	*l = रिक्त;
    listitem_T	*li = रिक्त;
    दीर्घ	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    पूर्णांक is_curbuf = buf == curbuf;
    जर (buf == रिक्त || (!is_curbuf && buf->b_ml.ml_mfp == रिक्त) || lnum < 1)
    {
	rettv->vval.v_संख्या = 1;	// FAIL
	जर (in_vim9script() && lnum < 1)
	    semsg(_(e_invalid_ओळ_संख्या_nr), lnum_arg);
	परत;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    जर (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    जर (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    अन्यथा
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_ओळ_मोजणी;

    जर (ओळी->v_type == VAR_LIST)
    {
	l = ओळी->vval.v_list;
	जर (l == रिक्त || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    जा done;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    अन्यथा
	ओळ = typval_tostring(ओळी, FALSE);

    // default result is zero == OK
    साठी (;;)
    {
	जर (l != रिक्त)
	{
	    // list argument, get next string
	    जर (li == रिक्त)
		थांबा;
	    vim_free(ओळ);
	    ओळ = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_next;
	}

	rettv->vval.v_संख्या = 1;	// FAIL
	जर (ओळ == रिक्त || lnum > curbuf->b_ml.ml_ओळ_मोजणी + 1)
	    थांबा;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	जर (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	जर (!append && lnum <= curbuf->b_ml.ml_ओळ_मोजणी)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    जर (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, ओळ, (colnr_T)STRLEN(ओळ) + 1, TRUE, TRUE) == OK)
	    {
		changed_bytes(lnum, 0);
		जर (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_col();
		rettv->vval.v_संख्या = 0;	// OK
	    }
	}
	अन्यथा जर (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    जर (ml_append(lnum - 1, ओळ, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_संख्या = 0;	// OK
	}

	जर (l == रिक्त)			// only one string argument
	    थांबा;
	++lnum;
    }
    vim_free(ओळ);

    जर (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_ओळी_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    जर (wp->w_buffer == buf
		    && (wp->w_buffer != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_col();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	जर (curwin->w_buffer == curbuf)
	    update_topline();
    }

done:
    जर (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    शून्य
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    पूर्णांक		did_emsg_before = did_emsg;

    जर (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	परत;

    lnum = tv_get_lnum(&argvars[0]);
    जर (did_emsg == did_emsg_before)
	set_buffer_ओळी(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    स्थिर शून्य
buf_set_append_ओळ(typval_T *argvars, typval_T *rettv, पूर्णांक append)
{
    linenr_T	lnum;
    buf_T	*buf;
    पूर्णांक		did_emsg_before = did_emsg;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_संख्या_or_list_arg(argvars, 2) == FAIL))
	परत;

    buf = tv_get_buf(&argvars[0], FALSE);
    जर (buf == रिक्त)
	rettv->vval.v_संख्या = 1; // FAIL
    अन्यथा
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	जर (did_emsg == did_emsg_before)
	    set_buffer_ओळी(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    शून्य
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_ओळ(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    शून्य
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    जर (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	परत;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_संख्या = buflist_add(*name == NUL ? रिक्त : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    शून्य
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    rettv->vval.v_संख्या = (find_buffer(&argvars[0]) != रिक्त);
}

/*
 * "buflisted(expr)" function
 */
    शून्य
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_संख्या = (buf != रिक्त && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    शून्य
f_bufload(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = get_buf_arg(&argvars[0]);
    जर (buf != रिक्त)
	buffer_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    शून्य
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_संख्या = (buf != रिक्त && buf->b_ml.ml_mfp != रिक्त);
}

/*
 * "bufname(expr)" function
 */
    शून्य
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    जर (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	परत;

    जर (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    अन्यथा
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    जर (buf != रिक्त && buf->b_fname != रिक्त)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    अन्यथा
	rettv->vval.v_string = रिक्त;
}

/*
 * "bufnr(expr)" function
 */
    शून्य
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    पूर्णांक		error = FALSE;
    char_u	*name;

    जर (in_vim9script()
	    && (check_for_opt_buffer_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	परत;

    जर (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    अन्यथा
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    जर (buf == रिक्त
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != रिक्त
	    && !error)
	buf = buflist_new(name, रिक्त, (linenr_T)1, 0);

    जर (buf != रिक्त)
	rettv->vval.v_संख्या = buf->b_fnum;
    अन्यथा
	rettv->vval.v_संख्या = -1;
}

    स्थिर शून्य
buf_win_common(typval_T *argvars, typval_T *rettv, पूर्णांक get_nr)
{
    win_T	*wp;
    पूर्णांक		winnr = 0;
    buf_T	*buf;

    जर (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	परत;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	जर (wp->w_buffer == buf)
	    थांबा;
    }
    rettv->vval.v_संख्या = (wp != रिक्त ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    शून्य
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    शून्य
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    शून्य
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	first, last;
    linenr_T	lnum;
    दीर्घ	मोजणी;
    पूर्णांक		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    पूर्णांक		did_emsg_before = did_emsg;

    rettv->vval.v_संख्या = 1;	// FAIL by default

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	परत;

    buf = tv_get_buf(&argvars[0], FALSE);
    जर (buf == रिक्त)
	परत;

    first = tv_get_lnum_buf(&argvars[1], buf);
    जर (did_emsg > did_emsg_before)
	परत;
    जर (argvars[2].v_type != VAR_UNKNOWN)
	last = tv_get_lnum_buf(&argvars[2], buf);
    अन्यथा
	last = first;

    जर (buf->b_ml.ml_mfp == रिक्त || first < 1
			   || first > buf->b_ml.ml_ओळ_मोजणी || last < first)
	परत;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    जर (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    जर (last > curbuf->b_ml.ml_ओळ_मोजणी)
	last = curbuf->b_ml.ml_ओळ_मोजणी;
    मोजणी = last - first + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    जर (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    जर (u_save(first - 1, last + 1) == FAIL)
	जा cleanup;

    साठी (lnum = first; lnum <= last; ++lnum)
	ml_delete_flags(first, ML_DEL_MESSAGE);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	जर (wp->w_buffer == buf)
	{
	    जर (wp->w_cursor.lnum > last)
		wp->w_cursor.lnum -= मोजणी;
	    अन्यथा जर (wp->w_cursor.lnum > first)
		wp->w_cursor.lnum = first;
	    जर (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_ओळ_मोजणी)
		wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_ओळ_मोजणी;
	    wp->w_valid = 0;
	    जर (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_col();
    deleted_ओळी_mark(first, मोजणी);
    rettv->vval.v_संख्या = 0; // OK

cleanup:
    जर (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    स्थिर dict_T *
get_buffer_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    जर (dict == रिक्त)
	परत रिक्त;

    dict_add_संख्या(dict, "bufnr", buf->b_fnum);
    dict_add_string(dict, "name", buf->b_ffname);
    dict_add_संख्या(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_add_संख्या(dict, "linecount", buf->b_ml.ml_ओळ_मोजणी);
    dict_add_संख्या(dict, "loaded", buf->b_ml.ml_mfp != रिक्त);
    dict_add_संख्या(dict, "listed", buf->b_p_bl);
    dict_add_संख्या(dict, "changed", bufIsChanged(buf));
    dict_add_संख्या(dict, "changedtick", CHANGEDTICK(buf));
    dict_add_संख्या(dict, "hidden",
			    buf->b_ml.ml_mfp != रिक्त && buf->b_nwindows == 0);
    dict_add_संख्या(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_add_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    जर (windows != रिक्त)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    जर (wp->w_buffer == buf)
		list_append_संख्या(windows, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    जर (windows != रिक्त)
    {
	FOR_ALL_POPUPWINS(wp)
	    जर (wp->w_buffer == buf)
		list_append_संख्या(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		जर (wp->w_buffer == buf)
		    list_append_संख्या(windows, (varnumber_T)wp->w_id);

	dict_add_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    जर (buf->b_signlist != रिक्त)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	जर (signs != रिक्त)
	{
	    get_buffer_signs(buf, signs);
	    dict_add_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_add_संख्या(dict, "lastused", buf->b_last_used);
# endif

    परत dict;
}

/*
 * "getbufinfo()" function
 */
    शून्य
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = रिक्त;
    buf_T	*argbuf = रिक्त;
    dict_T	*d;
    पूर्णांक		filtered = FALSE;
    पूर्णांक		sel_buflisted = FALSE;
    पूर्णांक		sel_bufloaded = FALSE;
    पूर्णांक		sel_bufmodified = FALSE;

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (in_vim9script()
	    && check_for_opt_buffer_or_dict_arg(argvars, 0) == FAIL)
	परत;

    // List of all the buffers or selected buffers
    जर (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	जर (sel_d != रिक्त)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    अन्यथा जर (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	जर (argbuf == रिक्त)
	    परत;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	जर (argbuf != रिक्त && argbuf != buf)
	    सुरूठेवा;
	जर (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == रिक्त)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_changed)))
	    सुरूठेवा;

	d = get_buffer_info(buf);
	जर (d != रिक्त)
	    list_append_dict(rettv->vval.v_list, d);
	जर (argbuf != रिक्त)
	    परत;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    स्थिर शून्य
get_buffer_ओळी(
    buf_T	*buf,
    linenr_T	start,
    linenr_T	end,
    पूर्णांक		retlist,
    typval_T	*rettv)
{
    जर (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = रिक्त;
    }

    जर (buf == रिक्त || buf->b_ml.ml_mfp == रिक्त || start < 0)
    {
	जर (retlist)
	    (शून्य)rettv_list_alloc(rettv);
	परत;
    }

    जर (!retlist)
    {
	जर (start >= 1 && start <= buf->b_ml.ml_ओळ_मोजणी)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, start, FALSE),
		    ml_get_buf_len(buf, start));
	अन्यथा
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    अन्यथा
    {
	list_T		*l;
	listitem_T	*li;

	start = MAX(start, 1);
	end = MIN(end, buf->b_ml.ml_ओळ_मोजणी);
	जर (end < start)
	    जा nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_वस्तुसंच((पूर्णांक)(end - start + 1));
	जर (l == रिक्त)
	    जा nolist;
	rettv_list_set(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	साठी (li = l->lv_first; start <= end; ++start, li = li->li_next)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, start, FALSE),
						   ml_get_buf_len(buf, start));
	}
    }
    परत;

nolist:
    (शून्य)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    स्थिर शून्य
getbufline(typval_T *argvars, typval_T *rettv, पूर्णांक retlist)
{
    linenr_T	lnum = 1;
    linenr_T	end = 1;
    buf_T	*buf;
    पूर्णांक		did_emsg_before = did_emsg;

    जर (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	परत;

    buf = tv_get_buf_from_arg(&argvars[0]);
    जर (buf != रिक्त)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	जर (did_emsg > did_emsg_before)
	    परत;
	जर (argvars[2].v_type == VAR_UNKNOWN)
	    end = lnum;
	अन्यथा
	    end = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_buffer_ओळी(buf, lnum, end, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    शून्य
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    शून्य
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    शून्य
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	end;
    पूर्णांक		retlist;

    जर (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	परत;

    lnum = tv_get_lnum(argvars);
    जर (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = 0;
	retlist = FALSE;
    }
    अन्यथा
    {
	end = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_buffer_ओळी(curbuf, lnum, end, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    शून्य
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_ओळ(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    शून्य
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    पूर्णांक		did_emsg_before = did_emsg;

    जर (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	परत;

    lnum = tv_get_lnum(&argvars[0]);
    जर (did_emsg == did_emsg_before)
	set_buffer_ओळी(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#endif  // FEAT_EVAL

#जर defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    शून्य
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
    शून्य
restore_buffer(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    जर (bufref_valid(save_curbuf))
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
    स्थिर पूर्णांक
find_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	जर ((*wp)->w_buffer == buf)
	    परत OK;
    परत FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    शून्य
switch_to_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    जर (find_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_buffer(save_curbuf, buf);
    अन्यथा जर (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_buffer(save_curbuf, buf);
    }
}

    शून्य
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    जर (save_curbuf->br_buf == रिक्त)
	restore_win(switchwin, TRUE);
    अन्यथा
	restore_buffer(save_curbuf);
}
#endif
