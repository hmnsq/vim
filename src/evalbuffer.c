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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    পূর্ণাঙ্ক
set_ref_in_buffers(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	বাবে (lnr = bp->b_listener; !abort && lnr != রিক্ত; lnr = lnr->lr_next)
	    abort = abort || set_ref_in_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_JOB_CHANNEL
	যদি (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_callback, copyID);
	যদি (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_interrupt, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	যদি (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_cfu_cb, copyID);
	যদি (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ofu_cb, copyID);
	যদি (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tsrfu_cb, copyID);
	যদি (!abort && bp->b_p_cpt_cb != রিক্ত)
	    abort = abort || set_ref_in_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_গুনতি, copyID);
# endif
	যদি (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tfu_cb, copyID);
	যদি (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ffu_cb, copyID);
	যদি (abort)
	    ভঙ্গ;
    }
    ঘূৰাই abort;
}

    buf_T *
buflist_find_by_name(char_u *name, পূর্ণাঙ্ক curtab_only)
{
    পূর্ণাঙ্ক		save_magic;
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
    ঘূৰাই buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
find_buffer(typval_T *avar)
{
    buf_T	*buf = রিক্ত;

    যদি (avar->v_type == VAR_সংখ্যা)
	buf = buflist_findnr((পূর্ণাঙ্ক)avar->vval.v_সংখ্যা);
    অন্যথা যদি (in_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	ঘূৰাই রিক্ত;
    অন্যথা যদি (avar->v_type == VAR_STRING && avar->vval.v_string != রিক্ত)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	যদি (buf == রিক্ত)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		যদি (buf->b_fname != রিক্ত
			&& (path_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    ভঙ্গ;
	}
    }
    ঘূৰাই buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    স্থির শূন্য
find_win_for_curbuf(শূন্য)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	যদি (wip->wi_win != রিক্ত && wip->wi_win->w_buffer == curbuf)
	{
	    curwin = wip->wi_win;
	    ভঙ্গ;
	}
    }
}

প্রকারনির্দেশ সংরচনা {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    পূর্ণাঙ্ক		cob_using_aco;
    পূর্ণাঙ্ক		cob_save_VIsual_active;
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
    স্থির শূন্য
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

    যদি (curwin->w_buffer != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_buffer;
	aucmd_prepbuf(&cob->cob_aco, buf);
	যদি (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    স্থির শূন্য
change_other_buffer_restore(cob_T *cob)
{
    যদি (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    অন্যথা
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
    স্থির শূন্য
set_buffer_সাৰিগুলি(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	পূর্ণাঙ্ক	    append,
	typval_T    *সাৰিগুলি,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*সাৰি = রিক্ত;
    list_T	*l = রিক্ত;
    listitem_T	*li = রিক্ত;
    long	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    পূর্ণাঙ্ক is_curbuf = buf == curbuf;
    যদি (buf == রিক্ত || (!is_curbuf && buf->b_ml.ml_mfp == রিক্ত) || lnum < 1)
    {
	rettv->vval.v_সংখ্যা = 1;	// FAIL
	যদি (in_vim9script() && lnum < 1)
	    semsg(_(e_invalid_সাৰি_সংখ্যা_nr), lnum_arg);
	ঘূৰাই;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    যদি (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    যদি (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    অন্যথা
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_সাৰি_গুনতি;

    যদি (সাৰিগুলি->v_type == VAR_LIST)
    {
	l = সাৰিগুলি->vval.v_list;
	যদি (l == রিক্ত || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    গচ্ছ done;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    অন্যথা
	সাৰি = typval_tostring(সাৰিগুলি, FALSE);

    // default result is zero == OK
    বাবে (;;)
    {
	যদি (l != রিক্ত)
	{
	    // list argument, get next string
	    যদি (li == রিক্ত)
		ভঙ্গ;
	    vim_free(সাৰি);
	    সাৰি = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_next;
	}

	rettv->vval.v_সংখ্যা = 1;	// FAIL
	যদি (সাৰি == রিক্ত || lnum > curbuf->b_ml.ml_সাৰি_গুনতি + 1)
	    ভঙ্গ;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	যদি (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	যদি (!append && lnum <= curbuf->b_ml.ml_সাৰি_গুনতি)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    যদি (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, সাৰি, (colnr_T)STRLEN(সাৰি) + 1, TRUE, TRUE) == OK)
	    {
		changed_bytes(lnum, 0);
		যদি (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_col();
		rettv->vval.v_সংখ্যা = 0;	// OK
	    }
	}
	অন্যথা যদি (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    যদি (ml_append(lnum - 1, সাৰি, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_সংখ্যা = 0;	// OK
	}

	যদি (l == রিক্ত)			// only one string argument
	    ভঙ্গ;
	++lnum;
    }
    vim_free(সাৰি);

    যদি (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_সাৰিগুলি_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    যদি (wp->w_buffer == buf
		    && (wp->w_buffer != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_col();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	যদি (curwin->w_buffer == curbuf)
	    update_topline();
    }

done:
    যদি (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    শূন্য
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;

    যদি (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    lnum = tv_get_lnum(&argvars[0]);
    যদি (did_emsg == did_emsg_before)
	set_buffer_সাৰিগুলি(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    স্থির শূন্য
buf_set_append_সাৰি(typval_T *argvars, typval_T *rettv, পূর্ণাঙ্ক append)
{
    linenr_T	lnum;
    buf_T	*buf;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_সংখ্যা_or_list_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    buf = tv_get_buf(&argvars[0], FALSE);
    যদি (buf == রিক্ত)
	rettv->vval.v_সংখ্যা = 1; // FAIL
    অন্যথা
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	যদি (did_emsg == did_emsg_before)
	    set_buffer_সাৰিগুলি(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    শূন্য
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_সাৰি(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    শূন্য
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    যদি (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_সংখ্যা = buflist_add(*name == NUL ? রিক্ত : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    শূন্য
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    rettv->vval.v_সংখ্যা = (find_buffer(&argvars[0]) != রিক্ত);
}

/*
 * "buflisted(expr)" function
 */
    শূন্য
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_সংখ্যা = (buf != রিক্ত && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    শূন্য
f_bufload(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    buf = get_buf_arg(&argvars[0]);
    যদি (buf != রিক্ত)
	buffer_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    শূন্য
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_সংখ্যা = (buf != রিক্ত && buf->b_ml.ml_mfp != রিক্ত);
}

/*
 * "bufname(expr)" function
 */
    শূন্য
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    যদি (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    যদি (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    অন্যথা
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    যদি (buf != রিক্ত && buf->b_fname != রিক্ত)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    অন্যথা
	rettv->vval.v_string = রিক্ত;
}

/*
 * "bufnr(expr)" function
 */
    শূন্য
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    পূর্ণাঙ্ক		error = FALSE;
    char_u	*name;

    যদি (in_vim9script()
	    && (check_for_opt_buffer_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	ঘূৰাই;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    অন্যথা
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    যদি (buf == রিক্ত
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != রিক্ত
	    && !error)
	buf = buflist_new(name, রিক্ত, (linenr_T)1, 0);

    যদি (buf != রিক্ত)
	rettv->vval.v_সংখ্যা = buf->b_fnum;
    অন্যথা
	rettv->vval.v_সংখ্যা = -1;
}

    স্থির শূন্য
buf_win_common(typval_T *argvars, typval_T *rettv, পূর্ণাঙ্ক get_nr)
{
    win_T	*wp;
    পূর্ণাঙ্ক		winnr = 0;
    buf_T	*buf;

    যদি (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	যদি (wp->w_buffer == buf)
	    ভঙ্গ;
    }
    rettv->vval.v_সংখ্যা = (wp != রিক্ত ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    শূন্য
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    শূন্য
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    শূন্য
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	first, last;
    linenr_T	lnum;
    long	গুনতি;
    পূর্ণাঙ্ক		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;

    rettv->vval.v_সংখ্যা = 1;	// FAIL by default

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    buf = tv_get_buf(&argvars[0], FALSE);
    যদি (buf == রিক্ত)
	ঘূৰাই;

    first = tv_get_lnum_buf(&argvars[1], buf);
    যদি (did_emsg > did_emsg_before)
	ঘূৰাই;
    যদি (argvars[2].v_type != VAR_UNKNOWN)
	last = tv_get_lnum_buf(&argvars[2], buf);
    অন্যথা
	last = first;

    যদি (buf->b_ml.ml_mfp == রিক্ত || first < 1
			   || first > buf->b_ml.ml_সাৰি_গুনতি || last < first)
	ঘূৰাই;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    যদি (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    যদি (last > curbuf->b_ml.ml_সাৰি_গুনতি)
	last = curbuf->b_ml.ml_সাৰি_গুনতি;
    গুনতি = last - first + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    যদি (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    যদি (u_save(first - 1, last + 1) == FAIL)
	গচ্ছ cleanup;

    বাবে (lnum = first; lnum <= last; ++lnum)
	ml_delete_flags(first, ML_DEL_MESSAGE);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	যদি (wp->w_buffer == buf)
	{
	    যদি (wp->w_cursor.lnum > last)
		wp->w_cursor.lnum -= গুনতি;
	    অন্যথা যদি (wp->w_cursor.lnum > first)
		wp->w_cursor.lnum = first;
	    যদি (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_সাৰি_গুনতি)
		wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_সাৰি_গুনতি;
	    wp->w_valid = 0;
	    যদি (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_col();
    deleted_সাৰিগুলি_mark(first, গুনতি);
    rettv->vval.v_সংখ্যা = 0; // OK

cleanup:
    যদি (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    স্থির dict_T *
get_buffer_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    যদি (dict == রিক্ত)
	ঘূৰাই রিক্ত;

    dict_add_সংখ্যা(dict, "bufnr", buf->b_fnum);
    dict_add_string(dict, "name", buf->b_ffname);
    dict_add_সংখ্যা(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_add_সংখ্যা(dict, "linecount", buf->b_ml.ml_সাৰি_গুনতি);
    dict_add_সংখ্যা(dict, "loaded", buf->b_ml.ml_mfp != রিক্ত);
    dict_add_সংখ্যা(dict, "listed", buf->b_p_bl);
    dict_add_সংখ্যা(dict, "changed", bufIsChanged(buf));
    dict_add_সংখ্যা(dict, "changedtick", CHANGEDTICK(buf));
    dict_add_সংখ্যা(dict, "hidden",
			    buf->b_ml.ml_mfp != রিক্ত && buf->b_nwindows == 0);
    dict_add_সংখ্যা(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_add_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    যদি (windows != রিক্ত)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    যদি (wp->w_buffer == buf)
		list_append_সংখ্যা(windows, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    যদি (windows != রিক্ত)
    {
	FOR_ALL_POPUPWINS(wp)
	    যদি (wp->w_buffer == buf)
		list_append_সংখ্যা(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		যদি (wp->w_buffer == buf)
		    list_append_সংখ্যা(windows, (varnumber_T)wp->w_id);

	dict_add_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    যদি (buf->b_signlist != রিক্ত)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	যদি (signs != রিক্ত)
	{
	    get_buffer_signs(buf, signs);
	    dict_add_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_add_সংখ্যা(dict, "lastused", buf->b_last_used);
# endif

    ঘূৰাই dict;
}

/*
 * "getbufinfo()" function
 */
    শূন্য
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = রিক্ত;
    buf_T	*argbuf = রিক্ত;
    dict_T	*d;
    পূর্ণাঙ্ক		filtered = FALSE;
    পূর্ণাঙ্ক		sel_buflisted = FALSE;
    পূর্ণাঙ্ক		sel_bufloaded = FALSE;
    পূর্ণাঙ্ক		sel_bufmodified = FALSE;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ঘূৰাই;

    যদি (in_vim9script()
	    && check_for_opt_buffer_or_dict_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    // List of all the buffers or selected buffers
    যদি (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	যদি (sel_d != রিক্ত)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    অন্যথা যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	যদি (argbuf == রিক্ত)
	    ঘূৰাই;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	যদি (argbuf != রিক্ত && argbuf != buf)
	    অনুৱর্ত;
	যদি (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == রিক্ত)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_changed)))
	    অনুৱর্ত;

	d = get_buffer_info(buf);
	যদি (d != রিক্ত)
	    list_append_dict(rettv->vval.v_list, d);
	যদি (argbuf != রিক্ত)
	    ঘূৰাই;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    স্থির শূন্য
get_buffer_সাৰিগুলি(
    buf_T	*buf,
    linenr_T	start,
    linenr_T	end,
    পূর্ণাঙ্ক		retlist,
    typval_T	*rettv)
{
    যদি (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = রিক্ত;
    }

    যদি (buf == রিক্ত || buf->b_ml.ml_mfp == রিক্ত || start < 0)
    {
	যদি (retlist)
	    (শূন্য)rettv_list_alloc(rettv);
	ঘূৰাই;
    }

    যদি (!retlist)
    {
	যদি (start >= 1 && start <= buf->b_ml.ml_সাৰি_গুনতি)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, start, FALSE),
		    ml_get_buf_len(buf, start));
	অন্যথা
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    অন্যথা
    {
	list_T		*l;
	listitem_T	*li;

	start = MAX(start, 1);
	end = MIN(end, buf->b_ml.ml_সাৰি_গুনতি);
	যদি (end < start)
	    গচ্ছ nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_বস্তুসমূহ((পূর্ণাঙ্ক)(end - start + 1));
	যদি (l == রিক্ত)
	    গচ্ছ nolist;
	rettv_list_set(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	বাবে (li = l->lv_first; start <= end; ++start, li = li->li_next)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, start, FALSE),
						   ml_get_buf_len(buf, start));
	}
    }
    ঘূৰাই;

nolist:
    (শূন্য)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    স্থির শূন্য
getbufline(typval_T *argvars, typval_T *rettv, পূর্ণাঙ্ক retlist)
{
    linenr_T	lnum = 1;
    linenr_T	end = 1;
    buf_T	*buf;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;

    যদি (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    buf = tv_get_buf_from_arg(&argvars[0]);
    যদি (buf != রিক্ত)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	যদি (did_emsg > did_emsg_before)
	    ঘূৰাই;
	যদি (argvars[2].v_type == VAR_UNKNOWN)
	    end = lnum;
	অন্যথা
	    end = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_buffer_সাৰিগুলি(buf, lnum, end, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    শূন্য
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    শূন্য
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    শূন্য
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	end;
    পূর্ণাঙ্ক		retlist;

    যদি (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    lnum = tv_get_lnum(argvars);
    যদি (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = 0;
	retlist = FALSE;
    }
    অন্যথা
    {
	end = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_buffer_সাৰিগুলি(curbuf, lnum, end, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    শূন্য
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_সাৰি(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    শূন্য
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;

    যদি (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    lnum = tv_get_lnum(&argvars[0]);
    যদি (did_emsg == did_emsg_before)
	set_buffer_সাৰিগুলি(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#endif  // FEAT_EVAL

#যদি defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    শূন্য
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
    শূন্য
restore_buffer(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    যদি (bufref_valid(save_curbuf))
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
    স্থির পূর্ণাঙ্ক
find_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	যদি ((*wp)->w_buffer == buf)
	    ঘূৰাই OK;
    ঘূৰাই FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    শূন্য
switch_to_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    যদি (find_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_buffer(save_curbuf, buf);
    অন্যথা যদি (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_buffer(save_curbuf, buf);
    }
}

    শূন্য
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    যদি (save_curbuf->br_buf == রিক্ত)
	restore_win(switchwin, TRUE);
    অন্যথা
	restore_buffer(save_curbuf);
}
#endif
