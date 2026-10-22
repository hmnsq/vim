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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    முழு
set_ref_in_buffers(முழு copyID)
{
    முழு		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	க்கு (lnr = bp->b_listener; !abort && lnr != வெற்று; lnr = lnr->lr_next)
	    abort = abort || set_ref_in_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_JOB_CHANNEL
	எனில் (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_callback, copyID);
	எனில் (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_interrupt, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	எனில் (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_cfu_cb, copyID);
	எனில் (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ofu_cb, copyID);
	எனில் (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tsrfu_cb, copyID);
	எனில் (!abort && bp->b_p_cpt_cb != வெற்று)
	    abort = abort || set_ref_in_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_மொத்தம், copyID);
# endif
	எனில் (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tfu_cb, copyID);
	எனில் (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ffu_cb, copyID);
	எனில் (abort)
	    நிறுத்து;
    }
    திரும்பு abort;
}

    buf_T *
buflist_find_by_name(char_u *name, முழு curtab_only)
{
    முழு		save_magic;
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
    திரும்பு buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
find_buffer(typval_T *avar)
{
    buf_T	*buf = வெற்று;

    எனில் (avar->v_type == VAR_எண்)
	buf = buflist_findnr((முழு)avar->vval.v_எண்);
    இல்லையெனில் எனில் (in_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	திரும்பு வெற்று;
    இல்லையெனில் எனில் (avar->v_type == VAR_STRING && avar->vval.v_string != வெற்று)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	எனில் (buf == வெற்று)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		எனில் (buf->b_fname != வெற்று
			&& (path_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    நிறுத்து;
	}
    }
    திரும்பு buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    நிலையான வெறுமை
find_win_for_curbuf(வெறுமை)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	எனில் (wip->wi_win != வெற்று && wip->wi_win->w_buffer == curbuf)
	{
	    curwin = wip->wi_win;
	    நிறுத்து;
	}
    }
}

வகைவரை கட்டமைப்பு {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    முழு		cob_using_aco;
    முழு		cob_save_VIsual_active;
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
    நிலையான வெறுமை
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

    எனில் (curwin->w_buffer != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_buffer;
	aucmd_prepbuf(&cob->cob_aco, buf);
	எனில் (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    நிலையான வெறுமை
change_other_buffer_restore(cob_T *cob)
{
    எனில் (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    இல்லையெனில்
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
    நிலையான வெறுமை
set_buffer_வரிகள்(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	முழு	    append,
	typval_T    *வரிகள்,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*வரி = வெற்று;
    list_T	*l = வெற்று;
    listitem_T	*li = வெற்று;
    long	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    முழு is_curbuf = buf == curbuf;
    எனில் (buf == வெற்று || (!is_curbuf && buf->b_ml.ml_mfp == வெற்று) || lnum < 1)
    {
	rettv->vval.v_எண் = 1;	// FAIL
	எனில் (in_vim9script() && lnum < 1)
	    semsg(_(e_invalid_வரி_எண்_nr), lnum_arg);
	திரும்பு;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    எனில் (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    எனில் (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    இல்லையெனில்
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_வரி_மொத்தம்;

    எனில் (வரிகள்->v_type == VAR_LIST)
    {
	l = வரிகள்->vval.v_list;
	எனில் (l == வெற்று || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    செல் done;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    இல்லையெனில்
	வரி = typval_tostring(வரிகள், FALSE);

    // default result is zero == OK
    க்கு (;;)
    {
	எனில் (l != வெற்று)
	{
	    // list argument, get next string
	    எனில் (li == வெற்று)
		நிறுத்து;
	    vim_free(வரி);
	    வரி = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_next;
	}

	rettv->vval.v_எண் = 1;	// FAIL
	எனில் (வரி == வெற்று || lnum > curbuf->b_ml.ml_வரி_மொத்தம் + 1)
	    நிறுத்து;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	எனில் (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	எனில் (!append && lnum <= curbuf->b_ml.ml_வரி_மொத்தம்)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    எனில் (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, வரி, (colnr_T)STRLEN(வரி) + 1, TRUE, TRUE) == OK)
	    {
		changed_bytes(lnum, 0);
		எனில் (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_col();
		rettv->vval.v_எண் = 0;	// OK
	    }
	}
	இல்லையெனில் எனில் (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    எனில் (ml_append(lnum - 1, வரி, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_எண் = 0;	// OK
	}

	எனில் (l == வெற்று)			// only one string argument
	    நிறுத்து;
	++lnum;
    }
    vim_free(வரி);

    எனில் (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_வரிகள்_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    எனில் (wp->w_buffer == buf
		    && (wp->w_buffer != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_col();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	எனில் (curwin->w_buffer == curbuf)
	    update_topline();
    }

done:
    எனில் (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    வெறுமை
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    முழு		did_emsg_before = did_emsg;

    எனில் (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	திரும்பு;

    lnum = tv_get_lnum(&argvars[0]);
    எனில் (did_emsg == did_emsg_before)
	set_buffer_வரிகள்(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    நிலையான வெறுமை
buf_set_append_வரி(typval_T *argvars, typval_T *rettv, முழு append)
{
    linenr_T	lnum;
    buf_T	*buf;
    முழு		did_emsg_before = did_emsg;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_எண்_or_list_arg(argvars, 2) == FAIL))
	திரும்பு;

    buf = tv_get_buf(&argvars[0], FALSE);
    எனில் (buf == வெற்று)
	rettv->vval.v_எண் = 1; // FAIL
    இல்லையெனில்
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	எனில் (did_emsg == did_emsg_before)
	    set_buffer_வரிகள்(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    வெறுமை
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_வரி(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    வெறுமை
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    எனில் (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_எண் = buflist_add(*name == NUL ? வெற்று : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    வெறுமை
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = (find_buffer(&argvars[0]) != வெற்று);
}

/*
 * "buflisted(expr)" function
 */
    வெறுமை
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_எண் = (buf != வெற்று && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    வெறுமை
f_bufload(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = get_buf_arg(&argvars[0]);
    எனில் (buf != வெற்று)
	buffer_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    வெறுமை
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_எண் = (buf != வெற்று && buf->b_ml.ml_mfp != வெற்று);
}

/*
 * "bufname(expr)" function
 */
    வெறுமை
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    எனில் (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    இல்லையெனில்
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    எனில் (buf != வெற்று && buf->b_fname != வெற்று)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    இல்லையெனில்
	rettv->vval.v_string = வெற்று;
}

/*
 * "bufnr(expr)" function
 */
    வெறுமை
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    முழு		error = FALSE;
    char_u	*name;

    எனில் (in_vim9script()
	    && (check_for_opt_buffer_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    இல்லையெனில்
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    எனில் (buf == வெற்று
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != வெற்று
	    && !error)
	buf = buflist_new(name, வெற்று, (linenr_T)1, 0);

    எனில் (buf != வெற்று)
	rettv->vval.v_எண் = buf->b_fnum;
    இல்லையெனில்
	rettv->vval.v_எண் = -1;
}

    நிலையான வெறுமை
buf_win_common(typval_T *argvars, typval_T *rettv, முழு get_nr)
{
    win_T	*wp;
    முழு		winnr = 0;
    buf_T	*buf;

    எனில் (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	திரும்பு;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	எனில் (wp->w_buffer == buf)
	    நிறுத்து;
    }
    rettv->vval.v_எண் = (wp != வெற்று ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    வெறுமை
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    வெறுமை
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    வெறுமை
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	first, last;
    linenr_T	lnum;
    long	மொத்தம்;
    முழு		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    முழு		did_emsg_before = did_emsg;

    rettv->vval.v_எண் = 1;	// FAIL by default

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	திரும்பு;

    buf = tv_get_buf(&argvars[0], FALSE);
    எனில் (buf == வெற்று)
	திரும்பு;

    first = tv_get_lnum_buf(&argvars[1], buf);
    எனில் (did_emsg > did_emsg_before)
	திரும்பு;
    எனில் (argvars[2].v_type != VAR_UNKNOWN)
	last = tv_get_lnum_buf(&argvars[2], buf);
    இல்லையெனில்
	last = first;

    எனில் (buf->b_ml.ml_mfp == வெற்று || first < 1
			   || first > buf->b_ml.ml_வரி_மொத்தம் || last < first)
	திரும்பு;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    எனில் (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    எனில் (last > curbuf->b_ml.ml_வரி_மொத்தம்)
	last = curbuf->b_ml.ml_வரி_மொத்தம்;
    மொத்தம் = last - first + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    எனில் (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    எனில் (u_save(first - 1, last + 1) == FAIL)
	செல் cleanup;

    க்கு (lnum = first; lnum <= last; ++lnum)
	ml_delete_flags(first, ML_DEL_MESSAGE);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	எனில் (wp->w_buffer == buf)
	{
	    எனில் (wp->w_cursor.lnum > last)
		wp->w_cursor.lnum -= மொத்தம்;
	    இல்லையெனில் எனில் (wp->w_cursor.lnum > first)
		wp->w_cursor.lnum = first;
	    எனில் (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_வரி_மொத்தம்)
		wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_வரி_மொத்தம்;
	    wp->w_valid = 0;
	    எனில் (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_col();
    deleted_வரிகள்_mark(first, மொத்தம்);
    rettv->vval.v_எண் = 0; // OK

cleanup:
    எனில் (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    நிலையான dict_T *
get_buffer_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    எனில் (dict == வெற்று)
	திரும்பு வெற்று;

    dict_add_எண்(dict, "bufnr", buf->b_fnum);
    dict_add_string(dict, "name", buf->b_ffname);
    dict_add_எண்(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_add_எண்(dict, "linecount", buf->b_ml.ml_வரி_மொத்தம்);
    dict_add_எண்(dict, "loaded", buf->b_ml.ml_mfp != வெற்று);
    dict_add_எண்(dict, "listed", buf->b_p_bl);
    dict_add_எண்(dict, "changed", bufIsChanged(buf));
    dict_add_எண்(dict, "changedtick", CHANGEDTICK(buf));
    dict_add_எண்(dict, "hidden",
			    buf->b_ml.ml_mfp != வெற்று && buf->b_nwindows == 0);
    dict_add_எண்(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_add_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    எனில் (windows != வெற்று)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    எனில் (wp->w_buffer == buf)
		list_append_எண்(windows, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    எனில் (windows != வெற்று)
    {
	FOR_ALL_POPUPWINS(wp)
	    எனில் (wp->w_buffer == buf)
		list_append_எண்(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		எனில் (wp->w_buffer == buf)
		    list_append_எண்(windows, (varnumber_T)wp->w_id);

	dict_add_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    எனில் (buf->b_signlist != வெற்று)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	எனில் (signs != வெற்று)
	{
	    get_buffer_signs(buf, signs);
	    dict_add_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_add_எண்(dict, "lastused", buf->b_last_used);
# endif

    திரும்பு dict;
}

/*
 * "getbufinfo()" function
 */
    வெறுமை
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = வெற்று;
    buf_T	*argbuf = வெற்று;
    dict_T	*d;
    முழு		filtered = FALSE;
    முழு		sel_buflisted = FALSE;
    முழு		sel_bufloaded = FALSE;
    முழு		sel_bufmodified = FALSE;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script()
	    && check_for_opt_buffer_or_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    // List of all the buffers or selected buffers
    எனில் (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	எனில் (sel_d != வெற்று)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    இல்லையெனில் எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	எனில் (argbuf == வெற்று)
	    திரும்பு;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	எனில் (argbuf != வெற்று && argbuf != buf)
	    தொடர்;
	எனில் (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == வெற்று)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_changed)))
	    தொடர்;

	d = get_buffer_info(buf);
	எனில் (d != வெற்று)
	    list_append_dict(rettv->vval.v_list, d);
	எனில் (argbuf != வெற்று)
	    திரும்பு;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    நிலையான வெறுமை
get_buffer_வரிகள்(
    buf_T	*buf,
    linenr_T	start,
    linenr_T	end,
    முழு		retlist,
    typval_T	*rettv)
{
    எனில் (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = வெற்று;
    }

    எனில் (buf == வெற்று || buf->b_ml.ml_mfp == வெற்று || start < 0)
    {
	எனில் (retlist)
	    (வெறுமை)rettv_list_alloc(rettv);
	திரும்பு;
    }

    எனில் (!retlist)
    {
	எனில் (start >= 1 && start <= buf->b_ml.ml_வரி_மொத்தம்)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, start, FALSE),
		    ml_get_buf_len(buf, start));
	இல்லையெனில்
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    இல்லையெனில்
    {
	list_T		*l;
	listitem_T	*li;

	start = MAX(start, 1);
	end = MIN(end, buf->b_ml.ml_வரி_மொத்தம்);
	எனில் (end < start)
	    செல் nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_பொருட்கள்((முழு)(end - start + 1));
	எனில் (l == வெற்று)
	    செல் nolist;
	rettv_list_set(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	க்கு (li = l->lv_first; start <= end; ++start, li = li->li_next)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, start, FALSE),
						   ml_get_buf_len(buf, start));
	}
    }
    திரும்பு;

nolist:
    (வெறுமை)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    நிலையான வெறுமை
getbufline(typval_T *argvars, typval_T *rettv, முழு retlist)
{
    linenr_T	lnum = 1;
    linenr_T	end = 1;
    buf_T	*buf;
    முழு		did_emsg_before = did_emsg;

    எனில் (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	திரும்பு;

    buf = tv_get_buf_from_arg(&argvars[0]);
    எனில் (buf != வெற்று)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	எனில் (did_emsg > did_emsg_before)
	    திரும்பு;
	எனில் (argvars[2].v_type == VAR_UNKNOWN)
	    end = lnum;
	இல்லையெனில்
	    end = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_buffer_வரிகள்(buf, lnum, end, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    வெறுமை
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    வெறுமை
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    வெறுமை
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	end;
    முழு		retlist;

    எனில் (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	திரும்பு;

    lnum = tv_get_lnum(argvars);
    எனில் (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = 0;
	retlist = FALSE;
    }
    இல்லையெனில்
    {
	end = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_buffer_வரிகள்(curbuf, lnum, end, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    வெறுமை
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_வரி(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    வெறுமை
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    முழு		did_emsg_before = did_emsg;

    எனில் (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	திரும்பு;

    lnum = tv_get_lnum(&argvars[0]);
    எனில் (did_emsg == did_emsg_before)
	set_buffer_வரிகள்(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#endif  // FEAT_EVAL

#எனில் defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    வெறுமை
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
    வெறுமை
restore_buffer(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    எனில் (bufref_valid(save_curbuf))
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
    நிலையான முழு
find_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	எனில் ((*wp)->w_buffer == buf)
	    திரும்பு OK;
    திரும்பு FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    வெறுமை
switch_to_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    எனில் (find_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_buffer(save_curbuf, buf);
    இல்லையெனில் எனில் (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_buffer(save_curbuf, buf);
    }
}

    வெறுமை
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    எனில் (save_curbuf->br_buf == வெற்று)
	restore_win(switchwin, TRUE);
    இல்லையெனில்
	restore_buffer(save_curbuf);
}
#endif
