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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    પૂર્ણાંક
set_ref_in_buffers(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	માટે (lnr = bp->b_listener; !abort && lnr != ખાલી; lnr = lnr->lr_next)
	    abort = abort || set_ref_in_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_JOB_CHANNEL
	જો (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_callback, copyID);
	જો (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_interrupt, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	જો (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_cfu_cb, copyID);
	જો (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ofu_cb, copyID);
	જો (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tsrfu_cb, copyID);
	જો (!abort && bp->b_p_cpt_cb != ખાલી)
	    abort = abort || set_ref_in_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_ગણતરી, copyID);
# endif
	જો (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tfu_cb, copyID);
	જો (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ffu_cb, copyID);
	જો (abort)
	    તોડો;
    }
    પાછા abort;
}

    buf_T *
buflist_find_by_name(char_u *name, પૂર્ણાંક curtab_only)
{
    પૂર્ણાંક		save_magic;
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
    પાછા buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
find_buffer(typval_T *avar)
{
    buf_T	*buf = ખાલી;

    જો (avar->v_type == VAR_સંખ્યા)
	buf = buflist_findnr((પૂર્ણાંક)avar->vval.v_સંખ્યા);
    નહીંતર જો (in_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	પાછા ખાલી;
    નહીંતર જો (avar->v_type == VAR_STRING && avar->vval.v_string != ખાલી)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	જો (buf == ખાલી)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		જો (buf->b_fname != ખાલી
			&& (path_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    તોડો;
	}
    }
    પાછા buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    સ્થિર શૂન્ય
find_win_for_curbuf(શૂન્ય)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	જો (wip->wi_win != ખાલી && wip->wi_win->w_buffer == curbuf)
	{
	    curwin = wip->wi_win;
	    તોડો;
	}
    }
}

પ્રકારવ્યાખ્યા સંરચના {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    પૂર્ણાંક		cob_using_aco;
    પૂર્ણાંક		cob_save_VIsual_active;
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
    સ્થિર શૂન્ય
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

    જો (curwin->w_buffer != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_buffer;
	aucmd_prepbuf(&cob->cob_aco, buf);
	જો (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    સ્થિર શૂન્ય
change_other_buffer_restore(cob_T *cob)
{
    જો (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    નહીંતર
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
    સ્થિર શૂન્ય
set_buffer_લીટીઓ(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	પૂર્ણાંક	    append,
	typval_T    *લીટીઓ,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*લીટી = ખાલી;
    list_T	*l = ખાલી;
    listitem_T	*li = ખાલી;
    long	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    પૂર્ણાંક is_curbuf = buf == curbuf;
    જો (buf == ખાલી || (!is_curbuf && buf->b_ml.ml_mfp == ખાલી) || lnum < 1)
    {
	rettv->vval.v_સંખ્યા = 1;	// FAIL
	જો (in_vim9script() && lnum < 1)
	    semsg(_(e_invalid_લીટી_સંખ્યા_nr), lnum_arg);
	પાછા;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    જો (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    જો (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    નહીંતર
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_લીટી_ગણતરી;

    જો (લીટીઓ->v_type == VAR_LIST)
    {
	l = લીટીઓ->vval.v_list;
	જો (l == ખાલી || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    જાઓ done;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    નહીંતર
	લીટી = typval_tostring(લીટીઓ, FALSE);

    // default result is zero == OK
    માટે (;;)
    {
	જો (l != ખાલી)
	{
	    // list argument, get next string
	    જો (li == ખાલી)
		તોડો;
	    vim_free(લીટી);
	    લીટી = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_next;
	}

	rettv->vval.v_સંખ્યા = 1;	// FAIL
	જો (લીટી == ખાલી || lnum > curbuf->b_ml.ml_લીટી_ગણતરી + 1)
	    તોડો;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	જો (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	જો (!append && lnum <= curbuf->b_ml.ml_લીટી_ગણતરી)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    જો (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, લીટી, (colnr_T)STRLEN(લીટી) + 1, TRUE, TRUE) == OK)
	    {
		changed_bytes(lnum, 0);
		જો (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_col();
		rettv->vval.v_સંખ્યા = 0;	// OK
	    }
	}
	નહીંતર જો (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    જો (ml_append(lnum - 1, લીટી, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_સંખ્યા = 0;	// OK
	}

	જો (l == ખાલી)			// only one string argument
	    તોડો;
	++lnum;
    }
    vim_free(લીટી);

    જો (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_લીટીઓ_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    જો (wp->w_buffer == buf
		    && (wp->w_buffer != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_col();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	જો (curwin->w_buffer == curbuf)
	    update_topline();
    }

done:
    જો (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    શૂન્ય
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    પૂર્ણાંક		did_emsg_before = did_emsg;

    જો (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	પાછા;

    lnum = tv_get_lnum(&argvars[0]);
    જો (did_emsg == did_emsg_before)
	set_buffer_લીટીઓ(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    સ્થિર શૂન્ય
buf_set_append_લીટી(typval_T *argvars, typval_T *rettv, પૂર્ણાંક append)
{
    linenr_T	lnum;
    buf_T	*buf;
    પૂર્ણાંક		did_emsg_before = did_emsg;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_સંખ્યા_or_list_arg(argvars, 2) == FAIL))
	પાછા;

    buf = tv_get_buf(&argvars[0], FALSE);
    જો (buf == ખાલી)
	rettv->vval.v_સંખ્યા = 1; // FAIL
    નહીંતર
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	જો (did_emsg == did_emsg_before)
	    set_buffer_લીટીઓ(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    શૂન્ય
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_લીટી(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    શૂન્ય
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    જો (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	પાછા;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_સંખ્યા = buflist_add(*name == NUL ? ખાલી : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    શૂન્ય
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = (find_buffer(&argvars[0]) != ખાલી);
}

/*
 * "buflisted(expr)" function
 */
    શૂન્ય
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_સંખ્યા = (buf != ખાલી && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    શૂન્ય
f_bufload(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = get_buf_arg(&argvars[0]);
    જો (buf != ખાલી)
	buffer_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    શૂન્ય
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_સંખ્યા = (buf != ખાલી && buf->b_ml.ml_mfp != ખાલી);
}

/*
 * "bufname(expr)" function
 */
    શૂન્ય
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    જો (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    જો (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    નહીંતર
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    જો (buf != ખાલી && buf->b_fname != ખાલી)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    નહીંતર
	rettv->vval.v_string = ખાલી;
}

/*
 * "bufnr(expr)" function
 */
    શૂન્ય
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    પૂર્ણાંક		error = FALSE;
    char_u	*name;

    જો (in_vim9script()
	    && (check_for_opt_buffer_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	પાછા;

    જો (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    નહીંતર
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    જો (buf == ખાલી
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != ખાલી
	    && !error)
	buf = buflist_new(name, ખાલી, (linenr_T)1, 0);

    જો (buf != ખાલી)
	rettv->vval.v_સંખ્યા = buf->b_fnum;
    નહીંતર
	rettv->vval.v_સંખ્યા = -1;
}

    સ્થિર શૂન્ય
buf_win_common(typval_T *argvars, typval_T *rettv, પૂર્ણાંક get_nr)
{
    win_T	*wp;
    પૂર્ણાંક		winnr = 0;
    buf_T	*buf;

    જો (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	પાછા;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	જો (wp->w_buffer == buf)
	    તોડો;
    }
    rettv->vval.v_સંખ્યા = (wp != ખાલી ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    શૂન્ય
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    શૂન્ય
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    શૂન્ય
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	first, last;
    linenr_T	lnum;
    long	ગણતરી;
    પૂર્ણાંક		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    પૂર્ણાંક		did_emsg_before = did_emsg;

    rettv->vval.v_સંખ્યા = 1;	// FAIL by default

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	પાછા;

    buf = tv_get_buf(&argvars[0], FALSE);
    જો (buf == ખાલી)
	પાછા;

    first = tv_get_lnum_buf(&argvars[1], buf);
    જો (did_emsg > did_emsg_before)
	પાછા;
    જો (argvars[2].v_type != VAR_UNKNOWN)
	last = tv_get_lnum_buf(&argvars[2], buf);
    નહીંતર
	last = first;

    જો (buf->b_ml.ml_mfp == ખાલી || first < 1
			   || first > buf->b_ml.ml_લીટી_ગણતરી || last < first)
	પાછા;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    જો (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    જો (last > curbuf->b_ml.ml_લીટી_ગણતરી)
	last = curbuf->b_ml.ml_લીટી_ગણતરી;
    ગણતરી = last - first + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    જો (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    જો (u_save(first - 1, last + 1) == FAIL)
	જાઓ cleanup;

    માટે (lnum = first; lnum <= last; ++lnum)
	ml_delete_flags(first, ML_DEL_MESSAGE);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	જો (wp->w_buffer == buf)
	{
	    જો (wp->w_cursor.lnum > last)
		wp->w_cursor.lnum -= ગણતરી;
	    નહીંતર જો (wp->w_cursor.lnum > first)
		wp->w_cursor.lnum = first;
	    જો (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_લીટી_ગણતરી)
		wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_લીટી_ગણતરી;
	    wp->w_valid = 0;
	    જો (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_col();
    deleted_લીટીઓ_mark(first, ગણતરી);
    rettv->vval.v_સંખ્યા = 0; // OK

cleanup:
    જો (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    સ્થિર dict_T *
get_buffer_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    જો (dict == ખાલી)
	પાછા ખાલી;

    dict_add_સંખ્યા(dict, "bufnr", buf->b_fnum);
    dict_add_string(dict, "name", buf->b_ffname);
    dict_add_સંખ્યા(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_add_સંખ્યા(dict, "linecount", buf->b_ml.ml_લીટી_ગણતરી);
    dict_add_સંખ્યા(dict, "loaded", buf->b_ml.ml_mfp != ખાલી);
    dict_add_સંખ્યા(dict, "listed", buf->b_p_bl);
    dict_add_સંખ્યા(dict, "changed", bufIsChanged(buf));
    dict_add_સંખ્યા(dict, "changedtick", CHANGEDTICK(buf));
    dict_add_સંખ્યા(dict, "hidden",
			    buf->b_ml.ml_mfp != ખાલી && buf->b_nwindows == 0);
    dict_add_સંખ્યા(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_add_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    જો (windows != ખાલી)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    જો (wp->w_buffer == buf)
		list_append_સંખ્યા(windows, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    જો (windows != ખાલી)
    {
	FOR_ALL_POPUPWINS(wp)
	    જો (wp->w_buffer == buf)
		list_append_સંખ્યા(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		જો (wp->w_buffer == buf)
		    list_append_સંખ્યા(windows, (varnumber_T)wp->w_id);

	dict_add_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    જો (buf->b_signlist != ખાલી)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	જો (signs != ખાલી)
	{
	    get_buffer_signs(buf, signs);
	    dict_add_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_add_સંખ્યા(dict, "lastused", buf->b_last_used);
# endif

    પાછા dict;
}

/*
 * "getbufinfo()" function
 */
    શૂન્ય
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ખાલી;
    buf_T	*argbuf = ખાલી;
    dict_T	*d;
    પૂર્ણાંક		filtered = FALSE;
    પૂર્ણાંક		sel_buflisted = FALSE;
    પૂર્ણાંક		sel_bufloaded = FALSE;
    પૂર્ણાંક		sel_bufmodified = FALSE;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script()
	    && check_for_opt_buffer_or_dict_arg(argvars, 0) == FAIL)
	પાછા;

    // List of all the buffers or selected buffers
    જો (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	જો (sel_d != ખાલી)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    નહીંતર જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	જો (argbuf == ખાલી)
	    પાછા;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	જો (argbuf != ખાલી && argbuf != buf)
	    ચાલુ;
	જો (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == ખાલી)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_changed)))
	    ચાલુ;

	d = get_buffer_info(buf);
	જો (d != ખાલી)
	    list_append_dict(rettv->vval.v_list, d);
	જો (argbuf != ખાલી)
	    પાછા;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    સ્થિર શૂન્ય
get_buffer_લીટીઓ(
    buf_T	*buf,
    linenr_T	start,
    linenr_T	end,
    પૂર્ણાંક		retlist,
    typval_T	*rettv)
{
    જો (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ખાલી;
    }

    જો (buf == ખાલી || buf->b_ml.ml_mfp == ખાલી || start < 0)
    {
	જો (retlist)
	    (શૂન્ય)rettv_list_alloc(rettv);
	પાછા;
    }

    જો (!retlist)
    {
	જો (start >= 1 && start <= buf->b_ml.ml_લીટી_ગણતરી)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, start, FALSE),
		    ml_get_buf_len(buf, start));
	નહીંતર
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    નહીંતર
    {
	list_T		*l;
	listitem_T	*li;

	start = MAX(start, 1);
	end = MIN(end, buf->b_ml.ml_લીટી_ગણતરી);
	જો (end < start)
	    જાઓ nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_વસ્તુઓ((પૂર્ણાંક)(end - start + 1));
	જો (l == ખાલી)
	    જાઓ nolist;
	rettv_list_set(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	માટે (li = l->lv_first; start <= end; ++start, li = li->li_next)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, start, FALSE),
						   ml_get_buf_len(buf, start));
	}
    }
    પાછા;

nolist:
    (શૂન્ય)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    સ્થિર શૂન્ય
getbufline(typval_T *argvars, typval_T *rettv, પૂર્ણાંક retlist)
{
    linenr_T	lnum = 1;
    linenr_T	end = 1;
    buf_T	*buf;
    પૂર્ણાંક		did_emsg_before = did_emsg;

    જો (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	પાછા;

    buf = tv_get_buf_from_arg(&argvars[0]);
    જો (buf != ખાલી)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	જો (did_emsg > did_emsg_before)
	    પાછા;
	જો (argvars[2].v_type == VAR_UNKNOWN)
	    end = lnum;
	નહીંતર
	    end = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_buffer_લીટીઓ(buf, lnum, end, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    શૂન્ય
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    શૂન્ય
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    શૂન્ય
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	end;
    પૂર્ણાંક		retlist;

    જો (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	પાછા;

    lnum = tv_get_lnum(argvars);
    જો (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = 0;
	retlist = FALSE;
    }
    નહીંતર
    {
	end = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_buffer_લીટીઓ(curbuf, lnum, end, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    શૂન્ય
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_લીટી(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    શૂન્ય
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    પૂર્ણાંક		did_emsg_before = did_emsg;

    જો (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	પાછા;

    lnum = tv_get_lnum(&argvars[0]);
    જો (did_emsg == did_emsg_before)
	set_buffer_લીટીઓ(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#endif  // FEAT_EVAL

#જો defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    શૂન્ય
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
    શૂન્ય
restore_buffer(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    જો (bufref_valid(save_curbuf))
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
    સ્થિર પૂર્ણાંક
find_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	જો ((*wp)->w_buffer == buf)
	    પાછા OK;
    પાછા FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    શૂન્ય
switch_to_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    જો (find_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_buffer(save_curbuf, buf);
    નહીંતર જો (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_buffer(save_curbuf, buf);
    }
}

    શૂન્ય
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    જો (save_curbuf->br_buf == ખાલી)
	restore_win(switchwin, TRUE);
    નહીંતર
	restore_buffer(save_curbuf);
}
#endif
