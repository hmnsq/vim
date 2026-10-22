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

#include "vim.h"

#གལ་སྲིད defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    ཧྲིལ
set_ref_in_buffers(ཧྲིལ copyID)
{
    ཧྲིལ		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	དོན་ལུ (lnr = bp->b_listener; !abort && lnr != NULL; lnr = lnr->lr_next)
	    abort = abort || set_ref_in_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_JOB_CHANNEL
	གལ་སྲིད (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_callback, copyID);
	གལ་སྲིད (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_interrupt, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	གལ་སྲིད (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_cfu_cb, copyID);
	གལ་སྲིད (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ofu_cb, copyID);
	གལ་སྲིད (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tsrfu_cb, copyID);
	གལ་སྲིད (!abort && bp->b_p_cpt_cb != NULL)
	    abort = abort || set_ref_in_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_count, copyID);
# endif
	གལ་སྲིད (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tfu_cb, copyID);
	གལ་སྲིད (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ffu_cb, copyID);
	གལ་སྲིད (abort)
	    bha_nga;
    }
    ལོག abort;
}

    buf_T *
buflist_find_by_name(char_u *name, ཧྲིལ curtab_only)
{
    ཧྲིལ		save_magic;
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
    ལོག buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
find_buffer(typval_T *avar)
{
    buf_T	*buf = NULL;

    གལ་སྲིད (avar->v_type == VAR_NUMBER)
	buf = buflist_findnr((ཧྲིལ)avar->vval.v_number);
    དེ་མིན གལ་སྲིད (in_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	ལོག NULL;
    དེ་མིན གལ་སྲིད (avar->v_type == VAR_STRING && avar->vval.v_string != NULL)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	གལ་སྲིད (buf == NULL)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		གལ་སྲིད (buf->b_fname != NULL
			&& (path_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    bha_nga;
	}
    }
    ལོག buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    static void
find_win_for_curbuf(void)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	གལ་སྲིད (wip->wi_win != NULL && wip->wi_win->w_buffer == curbuf)
	{
	    curwin = wip->wi_win;
	    bha_nga;
	}
    }
}

typedef sa_nracanaa {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    ཧྲིལ		cob_using_aco;
    ཧྲིལ		cob_save_VIsual_active;
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
    static void
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

    གལ་སྲིད (curwin->w_buffer != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_buffer;
	aucmd_prepbuf(&cob->cob_aco, buf);
	གལ་སྲིད (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    static void
change_other_buffer_restore(cob_T *cob)
{
    གལ་སྲིད (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    དེ་མིན
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
    static void
set_buffer_lines(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	ཧྲིལ	    append,
	typval_T    *lines,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*line = NULL;
    list_T	*l = NULL;
    listitem_T	*li = NULL;
    long	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    ཧྲིལ is_curbuf = buf == curbuf;
    གལ་སྲིད (buf == NULL || (!is_curbuf && buf->b_ml.ml_mfp == NULL) || lnum < 1)
    {
	rettv->vval.v_number = 1;	// FAIL
	གལ་སྲིད (in_vim9script() && lnum < 1)
	    semsg(_(e_invalid_line_number_nr), lnum_arg);
	ལོག;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    གལ་སྲིད (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    གལ་སྲིད (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    དེ་མིན
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_line_count;

    གལ་སྲིད (lines->v_type == VAR_LIST)
    {
	l = lines->vval.v_list;
	གལ་སྲིད (l == NULL || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    goto done;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    དེ་མིན
	line = typval_tostring(lines, FALSE);

    // default result is zero == OK
    དོན་ལུ (;;)
    {
	གལ་སྲིད (l != NULL)
	{
	    // list argument, get next string
	    གལ་སྲིད (li == NULL)
		bha_nga;
	    vim_free(line);
	    line = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_next;
	}

	rettv->vval.v_number = 1;	// FAIL
	གལ་སྲིད (line == NULL || lnum > curbuf->b_ml.ml_line_count + 1)
	    bha_nga;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	གལ་སྲིད (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	གལ་སྲིད (!append && lnum <= curbuf->b_ml.ml_line_count)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    གལ་སྲིད (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, line, (colnr_T)STRLEN(line) + 1, TRUE, TRUE) == OK)
	    {
		changed_bytes(lnum, 0);
		གལ་སྲིད (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_col();
		rettv->vval.v_number = 0;	// OK
	    }
	}
	དེ་མིན གལ་སྲིད (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    གལ་སྲིད (ml_append(lnum - 1, line, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_number = 0;	// OK
	}

	གལ་སྲིད (l == NULL)			// only one string argument
	    bha_nga;
	++lnum;
    }
    vim_free(line);

    གལ་སྲིད (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_lines_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    གལ་སྲིད (wp->w_buffer == buf
		    && (wp->w_buffer != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_col();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	གལ་སྲིད (curwin->w_buffer == curbuf)
	    update_topline();
    }

done:
    གལ་སྲིད (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    void
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    ཧྲིལ		did_emsg_before = did_emsg;

    གལ་སྲིད (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ལོག;

    lnum = tv_get_lnum(&argvars[0]);
    གལ་སྲིད (did_emsg == did_emsg_before)
	set_buffer_lines(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    static void
buf_set_append_line(typval_T *argvars, typval_T *rettv, ཧྲིལ append)
{
    linenr_T	lnum;
    buf_T	*buf;
    ཧྲིལ		did_emsg_before = did_emsg;

    གལ་སྲིད (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_number_or_list_arg(argvars, 2) == FAIL))
	ལོག;

    buf = tv_get_buf(&argvars[0], FALSE);
    གལ་སྲིད (buf == NULL)
	rettv->vval.v_number = 1; // FAIL
    དེ་མིན
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	གལ་སྲིད (did_emsg == did_emsg_before)
	    set_buffer_lines(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    void
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_line(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    void
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    གལ་སྲིད (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ལོག;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_number = buflist_add(*name == NUL ? NULL : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    void
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    གལ་སྲིད (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ལོག;

    rettv->vval.v_number = (find_buffer(&argvars[0]) != NULL);
}

/*
 * "buflisted(expr)" function
 */
    void
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    གལ་སྲིད (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ལོག;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_number = (buf != NULL && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    void
f_bufload(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    གལ་སྲིད (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ལོག;

    buf = get_buf_arg(&argvars[0]);
    གལ་སྲིད (buf != NULL)
	buffer_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    void
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    གལ་སྲིད (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ལོག;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_number = (buf != NULL && buf->b_ml.ml_mfp != NULL);
}

/*
 * "bufname(expr)" function
 */
    void
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    གལ་སྲིད (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ལོག;

    གལ་སྲིད (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    དེ་མིན
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    གལ་སྲིད (buf != NULL && buf->b_fname != NULL)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    དེ་མིན
	rettv->vval.v_string = NULL;
}

/*
 * "bufnr(expr)" function
 */
    void
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ཧྲིལ		error = FALSE;
    char_u	*name;

    གལ་སྲིད (in_vim9script()
	    && (check_for_opt_buffer_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	ལོག;

    གལ་སྲིད (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    དེ་མིན
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    གལ་སྲིད (buf == NULL
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != NULL
	    && !error)
	buf = buflist_new(name, NULL, (linenr_T)1, 0);

    གལ་སྲིད (buf != NULL)
	rettv->vval.v_number = buf->b_fnum;
    དེ་མིན
	rettv->vval.v_number = -1;
}

    static void
buf_win_common(typval_T *argvars, typval_T *rettv, ཧྲིལ get_nr)
{
    win_T	*wp;
    ཧྲིལ		winnr = 0;
    buf_T	*buf;

    གལ་སྲིད (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ལོག;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	གལ་སྲིད (wp->w_buffer == buf)
	    bha_nga;
    }
    rettv->vval.v_number = (wp != NULL ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    void
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    void
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    void
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	first, last;
    linenr_T	lnum;
    long	count;
    ཧྲིལ		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    ཧྲིལ		did_emsg_before = did_emsg;

    rettv->vval.v_number = 1;	// FAIL by default

    གལ་སྲིད (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ལོག;

    buf = tv_get_buf(&argvars[0], FALSE);
    གལ་སྲིད (buf == NULL)
	ལོག;

    first = tv_get_lnum_buf(&argvars[1], buf);
    གལ་སྲིད (did_emsg > did_emsg_before)
	ལོག;
    གལ་སྲིད (argvars[2].v_type != VAR_UNKNOWN)
	last = tv_get_lnum_buf(&argvars[2], buf);
    དེ་མིན
	last = first;

    གལ་སྲིད (buf->b_ml.ml_mfp == NULL || first < 1
			   || first > buf->b_ml.ml_line_count || last < first)
	ལོག;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    གལ་སྲིད (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    གལ་སྲིད (last > curbuf->b_ml.ml_line_count)
	last = curbuf->b_ml.ml_line_count;
    count = last - first + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    གལ་སྲིད (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    གལ་སྲིད (u_save(first - 1, last + 1) == FAIL)
	goto cleanup;

    དོན་ལུ (lnum = first; lnum <= last; ++lnum)
	ml_delete_flags(first, ML_DEL_MESSAGE);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	གལ་སྲིད (wp->w_buffer == buf)
	{
	    གལ་སྲིད (wp->w_cursor.lnum > last)
		wp->w_cursor.lnum -= count;
	    དེ་མིན གལ་སྲིད (wp->w_cursor.lnum > first)
		wp->w_cursor.lnum = first;
	    གལ་སྲིད (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_line_count)
		wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_line_count;
	    wp->w_valid = 0;
	    གལ་སྲིད (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_col();
    deleted_lines_mark(first, count);
    rettv->vval.v_number = 0; // OK

cleanup:
    གལ་སྲིད (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    static dict_T *
get_buffer_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    གལ་སྲིད (dict == NULL)
	ལོག NULL;

    dict_add_number(dict, "bufnr", buf->b_fnum);
    dict_add_string(dict, "name", buf->b_ffname);
    dict_add_number(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_add_number(dict, "linecount", buf->b_ml.ml_line_count);
    dict_add_number(dict, "loaded", buf->b_ml.ml_mfp != NULL);
    dict_add_number(dict, "listed", buf->b_p_bl);
    dict_add_number(dict, "changed", bufIsChanged(buf));
    dict_add_number(dict, "changedtick", CHANGEDTICK(buf));
    dict_add_number(dict, "hidden",
			    buf->b_ml.ml_mfp != NULL && buf->b_nwindows == 0);
    dict_add_number(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_add_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    གལ་སྲིད (windows != NULL)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    གལ་སྲིད (wp->w_buffer == buf)
		list_append_number(windows, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    གལ་སྲིད (windows != NULL)
    {
	FOR_ALL_POPUPWINS(wp)
	    གལ་སྲིད (wp->w_buffer == buf)
		list_append_number(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		གལ་སྲིད (wp->w_buffer == buf)
		    list_append_number(windows, (varnumber_T)wp->w_id);

	dict_add_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    གལ་སྲིད (buf->b_signlist != NULL)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	གལ་སྲིད (signs != NULL)
	{
	    get_buffer_signs(buf, signs);
	    dict_add_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_add_number(dict, "lastused", buf->b_last_used);
# endif

    ལོག dict;
}

/*
 * "getbufinfo()" function
 */
    void
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = NULL;
    buf_T	*argbuf = NULL;
    dict_T	*d;
    ཧྲིལ		filtered = FALSE;
    ཧྲིལ		sel_buflisted = FALSE;
    ཧྲིལ		sel_bufloaded = FALSE;
    ཧྲིལ		sel_bufmodified = FALSE;

    གལ་སྲིད (rettv_list_alloc(rettv) == FAIL)
	ལོག;

    གལ་སྲིད (in_vim9script()
	    && check_for_opt_buffer_or_dict_arg(argvars, 0) == FAIL)
	ལོག;

    // List of all the buffers or selected buffers
    གལ་སྲིད (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	གལ་སྲིད (sel_d != NULL)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    དེ་མིན གལ་སྲིད (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	གལ་སྲིད (argbuf == NULL)
	    ལོག;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	གལ་སྲིད (argbuf != NULL && argbuf != buf)
	    continue;
	གལ་སྲིད (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == NULL)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_changed)))
	    continue;

	d = get_buffer_info(buf);
	གལ་སྲིད (d != NULL)
	    list_append_dict(rettv->vval.v_list, d);
	གལ་སྲིད (argbuf != NULL)
	    ལོག;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    static void
get_buffer_lines(
    buf_T	*buf,
    linenr_T	start,
    linenr_T	end,
    ཧྲིལ		retlist,
    typval_T	*rettv)
{
    གལ་སྲིད (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = NULL;
    }

    གལ་སྲིད (buf == NULL || buf->b_ml.ml_mfp == NULL || start < 0)
    {
	གལ་སྲིད (retlist)
	    (void)rettv_list_alloc(rettv);
	ལོག;
    }

    གལ་སྲིད (!retlist)
    {
	གལ་སྲིད (start >= 1 && start <= buf->b_ml.ml_line_count)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, start, FALSE),
		    ml_get_buf_len(buf, start));
	དེ་མིན
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    དེ་མིན
    {
	list_T		*l;
	listitem_T	*li;

	start = MAX(start, 1);
	end = MIN(end, buf->b_ml.ml_line_count);
	གལ་སྲིད (end < start)
	    goto nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_items((ཧྲིལ)(end - start + 1));
	གལ་སྲིད (l == NULL)
	    goto nolist;
	rettv_list_set(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	དོན་ལུ (li = l->lv_first; start <= end; ++start, li = li->li_next)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, start, FALSE),
						   ml_get_buf_len(buf, start));
	}
    }
    ལོག;

nolist:
    (void)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    static void
getbufline(typval_T *argvars, typval_T *rettv, ཧྲིལ retlist)
{
    linenr_T	lnum = 1;
    linenr_T	end = 1;
    buf_T	*buf;
    ཧྲིལ		did_emsg_before = did_emsg;

    གལ་སྲིད (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ལོག;

    buf = tv_get_buf_from_arg(&argvars[0]);
    གལ་སྲིད (buf != NULL)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	གལ་སྲིད (did_emsg > did_emsg_before)
	    ལོག;
	གལ་སྲིད (argvars[2].v_type == VAR_UNKNOWN)
	    end = lnum;
	དེ་མིན
	    end = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_buffer_lines(buf, lnum, end, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    void
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    void
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    void
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	end;
    ཧྲིལ		retlist;

    གལ་སྲིད (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	ལོག;

    lnum = tv_get_lnum(argvars);
    གལ་སྲིད (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = 0;
	retlist = FALSE;
    }
    དེ་མིན
    {
	end = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_buffer_lines(curbuf, lnum, end, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    void
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_line(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    void
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    ཧྲིལ		did_emsg_before = did_emsg;

    གལ་སྲིད (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ལོག;

    lnum = tv_get_lnum(&argvars[0]);
    གལ་སྲིད (did_emsg == did_emsg_before)
	set_buffer_lines(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#endif  // FEAT_EVAL

#གལ་སྲིད defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    void
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
    void
restore_buffer(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    གལ་སྲིད (bufref_valid(save_curbuf))
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
    static ཧྲིལ
find_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	གལ་སྲིད ((*wp)->w_buffer == buf)
	    ལོག OK;
    ལོག FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    void
switch_to_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    གལ་སྲིད (find_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_buffer(save_curbuf, buf);
    དེ་མིན གལ་སྲིད (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_buffer(save_curbuf, buf);
    }
}

    void
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    གལ་སྲིད (save_curbuf->br_buf == NULL)
	restore_win(switchwin, TRUE);
    དེ་མིན
	restore_buffer(save_curbuf);
}
#endif
