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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_buffers(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	ପାଇଁ (lnr = bp->b_listener; !abort && lnr != ଖାଲି; lnr = lnr->lr_next)
	    abort = abort || set_ref_in_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_JOB_CHANNEL
	ଯଦି (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_callback, copyID);
	ଯଦି (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_prompt_interrupt, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	ଯଦି (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_cfu_cb, copyID);
	ଯଦି (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ofu_cb, copyID);
	ଯଦି (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tsrfu_cb, copyID);
	ଯଦି (!abort && bp->b_p_cpt_cb != ଖାଲି)
	    abort = abort || set_ref_in_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_ଗଣନା, copyID);
# endif
	ଯଦି (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_tfu_cb, copyID);
	ଯଦି (!abort)
	    abort = abort || set_ref_in_callback(&bp->b_ffu_cb, copyID);
	ଯଦି (abort)
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ abort;
}

    buf_T *
buflist_find_by_name(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା curtab_only)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_magic;
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
    ଫେରନ୍ତୁ buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
find_buffer(typval_T *avar)
{
    buf_T	*buf = ଖାଲି;

    ଯଦି (avar->v_type == VAR_ସଂଖ୍ୟା)
	buf = buflist_findnr((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)avar->vval.v_ସଂଖ୍ୟା);
    ଅନ୍ୟଥା ଯଦି (in_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;
    ଅନ୍ୟଥା ଯଦି (avar->v_type == VAR_STRING && avar->vval.v_string != ଖାଲି)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	ଯଦି (buf == ଖାଲି)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		ଯଦି (buf->b_fname != ଖାଲି
			&& (path_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    ଭାଙ୍ଗ;
	}
    }
    ଫେରନ୍ତୁ buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
find_win_for_curbuf(ଶୂନ୍ୟ)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	ଯଦି (wip->wi_win != ଖାଲି && wip->wi_win->w_buffer == curbuf)
	{
	    curwin = wip->wi_win;
	    ଭାଙ୍ଗ;
	}
    }
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cob_using_aco;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cob_save_VIsual_active;
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
    ସ୍ଥିର ଶୂନ୍ୟ
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

    ଯଦି (curwin->w_buffer != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_buffer;
	aucmd_prepbuf(&cob->cob_aco, buf);
	ଯଦି (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
change_other_buffer_restore(cob_T *cob)
{
    ଯଦି (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
set_buffer_ଧାଡ଼ିଗୁଡ଼ିକ(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    append,
	typval_T    *ଧାଡ଼ିଗୁଡ଼ିକ,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*ଧାଡ଼ି = ଖାଲି;
    list_T	*l = ଖାଲି;
    listitem_T	*li = ଖାଲି;
    long	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_curbuf = buf == curbuf;
    ଯଦି (buf == ଖାଲି || (!is_curbuf && buf->b_ml.ml_mfp == ଖାଲି) || lnum < 1)
    {
	rettv->vval.v_ସଂଖ୍ୟା = 1;	// FAIL
	ଯଦି (in_vim9script() && lnum < 1)
	    semsg(_(e_invalid_ଧାଡ଼ି_ସଂଖ୍ୟା_nr), lnum_arg);
	ଫେରନ୍ତୁ;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    ଯଦି (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    ଯଦି (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    ଅନ୍ୟଥା
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;

    ଯଦି (ଧାଡ଼ିଗୁଡ଼ିକ->v_type == VAR_LIST)
    {
	l = ଧାଡ଼ିଗୁଡ଼ିକ->vval.v_list;
	ଯଦି (l == ଖାଲି || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    ଯାଅ done;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_first;
    }
    ଅନ୍ୟଥା
	ଧାଡ଼ି = typval_tostring(ଧାଡ଼ିଗୁଡ଼ିକ, FALSE);

    // default result is zero == OK
    ପାଇଁ (;;)
    {
	ଯଦି (l != ଖାଲି)
	{
	    // list argument, get next string
	    ଯଦି (li == ଖାଲି)
		ଭାଙ୍ଗ;
	    vim_free(ଧାଡ଼ି);
	    ଧାଡ଼ି = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_next;
	}

	rettv->vval.v_ସଂଖ୍ୟା = 1;	// FAIL
	ଯଦି (ଧାଡ଼ି == ଖାଲି || lnum > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1)
	    ଭାଙ୍ଗ;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	ଯଦି (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	ଯଦି (!append && lnum <= curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    ଯଦି (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, ଧାଡ଼ି, (colnr_T)STRLEN(ଧାଡ଼ି) + 1, TRUE, TRUE) == OK)
	    {
		changed_bytes(lnum, 0);
		ଯଦି (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_col();
		rettv->vval.v_ସଂଖ୍ୟା = 0;	// OK
	    }
	}
	ଅନ୍ୟଥା ଯଦି (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    ଯଦି (ml_append(lnum - 1, ଧାଡ଼ି, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_ସଂଖ୍ୟା = 0;	// OK
	}

	ଯଦି (l == ଖାଲି)			// only one string argument
	    ଭାଙ୍ଗ;
	++lnum;
    }
    vim_free(ଧାଡ଼ି);

    ଯଦି (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ଯଦି (wp->w_buffer == buf
		    && (wp->w_buffer != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_col();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	ଯଦି (curwin->w_buffer == curbuf)
	    update_topline();
    }

done:
    ଯଦି (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    ଶୂନ୍ୟ
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;

    ଯଦି (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    lnum = tv_get_lnum(&argvars[0]);
    ଯଦି (did_emsg == did_emsg_before)
	set_buffer_ଧାଡ଼ିଗୁଡ଼ିକ(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
buf_set_append_ଧାଡ଼ି(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା append)
{
    linenr_T	lnum;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_ସଂଖ୍ୟା_or_list_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ଯଦି (buf == ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = 1; // FAIL
    ଅନ୍ୟଥା
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	ଯଦି (did_emsg == did_emsg_before)
	    set_buffer_ଧାଡ଼ିଗୁଡ଼ିକ(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    ଶୂନ୍ୟ
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_ଧାଡ଼ି(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    ଶୂନ୍ୟ
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_ସଂଖ୍ୟା = buflist_add(*name == NUL ? ଖାଲି : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    ଶୂନ୍ୟ
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = (find_buffer(&argvars[0]) != ଖାଲି);
}

/*
 * "buflisted(expr)" function
 */
    ଶୂନ୍ୟ
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_ସଂଖ୍ୟା = (buf != ଖାଲି && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    ଶୂନ୍ୟ
f_bufload(typval_T *argvars, typval_T *rettv UNUSED)
{
    buf_T	*buf;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = get_buf_arg(&argvars[0]);
    ଯଦି (buf != ଖାଲି)
	buffer_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    ଶୂନ୍ୟ
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = find_buffer(&argvars[0]);
    rettv->vval.v_ସଂଖ୍ୟା = (buf != ଖାଲି && buf->b_ml.ml_mfp != ଖାଲି);
}

/*
 * "bufname(expr)" function
 */
    ଶୂନ୍ୟ
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    ଯଦି (in_vim9script() && check_for_opt_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    ଅନ୍ୟଥା
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    ଯଦି (buf != ଖାଲି && buf->b_fname != ଖାଲି)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    ଅନ୍ୟଥା
	rettv->vval.v_string = ଖାଲି;
}

/*
 * "bufnr(expr)" function
 */
    ଶୂନ୍ୟ
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
    char_u	*name;

    ଯଦି (in_vim9script()
	    && (check_for_opt_buffer_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    ଅନ୍ୟଥା
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    ଯଦି (buf == ଖାଲି
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != ଖାଲି
	    && !error)
	buf = buflist_new(name, ଖାଲି, (linenr_T)1, 0);

    ଯଦି (buf != ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = buf->b_fnum;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = -1;
}

    ସ୍ଥିର ଶୂନ୍ୟ
buf_win_common(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_nr)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winnr = 0;
    buf_T	*buf;

    ଯଦି (in_vim9script() && check_for_buffer_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	ଯଦି (wp->w_buffer == buf)
	    ଭାଙ୍ଗ;
    }
    rettv->vval.v_ସଂଖ୍ୟା = (wp != ଖାଲି ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    ଶୂନ୍ୟ
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    ଶୂନ୍ୟ
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    ଶୂନ୍ୟ
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	first, last;
    linenr_T	lnum;
    long	ଗଣନା_2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;

    rettv->vval.v_ସଂଖ୍ୟା = 1;	// FAIL by default

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    buf = tv_get_buf(&argvars[0], FALSE);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    first = tv_get_lnum_buf(&argvars[1], buf);
    ଯଦି (did_emsg > did_emsg_before)
	ଫେରନ୍ତୁ;
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
	last = tv_get_lnum_buf(&argvars[2], buf);
    ଅନ୍ୟଥା
	last = first;

    ଯଦି (buf->b_ml.ml_mfp == ଖାଲି || first < 1
			   || first > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା || last < first)
	ଫେରନ୍ତୁ;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    ଯଦି (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_buffer_prepare(&cob, buf);

    ଯଦି (last > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	last = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    ଗଣନା_2 = last - first + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    ଯଦି (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    ଯଦି (u_save(first - 1, last + 1) == FAIL)
	ଯାଅ cleanup;

    ପାଇଁ (lnum = first; lnum <= last; ++lnum)
	ml_delete_flags(first, ML_DEL_MESSAGE);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ଯଦି (wp->w_buffer == buf)
	{
	    ଯଦି (wp->w_cursor.lnum > last)
		wp->w_cursor.lnum -= ଗଣନା_2;
	    ଅନ୍ୟଥା ଯଦି (wp->w_cursor.lnum > first)
		wp->w_cursor.lnum = first;
	    ଯଦି (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		wp->w_cursor.lnum = wp->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	    wp->w_valid = 0;
	    ଯଦି (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_col();
    deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark(first, ଗଣନା_2);
    rettv->vval.v_ସଂଖ୍ୟା = 0; // OK

cleanup:
    ଯଦି (!is_curbuf)
	change_other_buffer_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    ସ୍ଥିର dict_T *
get_buffer_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    ଯଦି (dict == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    dict_add_ସଂଖ୍ୟା(dict, "bufnr", buf->b_fnum);
    dict_add_string(dict, "name", buf->b_ffname);
    dict_add_ସଂଖ୍ୟା(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_add_ସଂଖ୍ୟା(dict, "linecount", buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
    dict_add_ସଂଖ୍ୟା(dict, "loaded", buf->b_ml.ml_mfp != ଖାଲି);
    dict_add_ସଂଖ୍ୟା(dict, "listed", buf->b_p_bl);
    dict_add_ସଂଖ୍ୟା(dict, "changed", bufIsChanged(buf));
    dict_add_ସଂଖ୍ୟା(dict, "changedtick", CHANGEDTICK(buf));
    dict_add_ସଂଖ୍ୟା(dict, "hidden",
			    buf->b_ml.ml_mfp != ଖାଲି && buf->b_nwindows == 0);
    dict_add_ସଂଖ୍ୟା(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_add_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    ଯଦି (windows != ଖାଲି)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    ଯଦି (wp->w_buffer == buf)
		list_append_ସଂଖ୍ୟା(windows, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    ଯଦି (windows != ଖାଲି)
    {
	FOR_ALL_POPUPWINS(wp)
	    ଯଦି (wp->w_buffer == buf)
		list_append_ସଂଖ୍ୟା(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		ଯଦି (wp->w_buffer == buf)
		    list_append_ସଂଖ୍ୟା(windows, (varnumber_T)wp->w_id);

	dict_add_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    ଯଦି (buf->b_signlist != ଖାଲି)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	ଯଦି (signs != ଖାଲି)
	{
	    get_buffer_signs(buf, signs);
	    dict_add_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_add_ସଂଖ୍ୟା(dict, "lastused", buf->b_last_used);
# endif

    ଫେରନ୍ତୁ dict;
}

/*
 * "getbufinfo()" function
 */
    ଶୂନ୍ୟ
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = ଖାଲି;
    buf_T	*argbuf = ଖାଲି;
    dict_T	*d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filtered = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sel_buflisted = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sel_bufloaded = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sel_bufmodified = FALSE;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && check_for_opt_buffer_or_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    // List of all the buffers or selected buffers
    ଯଦି (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	ଯଦି (sel_d != ଖାଲି)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    ଅନ୍ୟଥା ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	ଯଦି (argbuf == ଖାଲି)
	    ଫେରନ୍ତୁ;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	ଯଦି (argbuf != ଖାଲି && argbuf != buf)
	    ଜାରି;
	ଯଦି (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == ଖାଲି)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_changed)))
	    ଜାରି;

	d = get_buffer_info(buf);
	ଯଦି (d != ଖାଲି)
	    list_append_dict(rettv->vval.v_list, d);
	ଯଦି (argbuf != ଖାଲି)
	    ଫେରନ୍ତୁ;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_buffer_ଧାଡ଼ିଗୁଡ଼ିକ(
    buf_T	*buf,
    linenr_T	start,
    linenr_T	end,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retlist,
    typval_T	*rettv)
{
    ଯଦି (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = ଖାଲି;
    }

    ଯଦି (buf == ଖାଲି || buf->b_ml.ml_mfp == ଖାଲି || start < 0)
    {
	ଯଦି (retlist)
	    (ଶୂନ୍ୟ)rettv_list_alloc(rettv);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (!retlist)
    {
	ଯଦି (start >= 1 && start <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, start, FALSE),
		    ml_get_buf_len(buf, start));
	ଅନ୍ୟଥା
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    ଅନ୍ୟଥା
    {
	list_T		*l;
	listitem_T	*li;

	start = MAX(start, 1);
	end = MIN(end, buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
	ଯଦି (end < start)
	    ଯାଅ nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - start + 1));
	ଯଦି (l == ଖାଲି)
	    ଯାଅ nolist;
	rettv_list_set(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	ପାଇଁ (li = l->lv_first; start <= end; ++start, li = li->li_next)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, start, FALSE),
						   ml_get_buf_len(buf, start));
	}
    }
    ଫେରନ୍ତୁ;

nolist:
    (ଶୂନ୍ୟ)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    ସ୍ଥିର ଶୂନ୍ୟ
getbufline(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା retlist)
{
    linenr_T	lnum = 1;
    linenr_T	end = 1;
    buf_T	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;

    ଯଦି (in_vim9script()
	    && (check_for_buffer_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    buf = tv_get_buf_from_arg(&argvars[0]);
    ଯଦି (buf != ଖାଲି)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	ଯଦି (did_emsg > did_emsg_before)
	    ଫେରନ୍ତୁ;
	ଯଦି (argvars[2].v_type == VAR_UNKNOWN)
	    end = lnum;
	ଅନ୍ୟଥା
	    end = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_buffer_ଧାଡ଼ିଗୁଡ଼ିକ(buf, lnum, end, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    ଶୂନ୍ୟ
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    ଶୂନ୍ୟ
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    ଶୂନ୍ୟ
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	end;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retlist;

    ଯଦି (in_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    lnum = tv_get_lnum(argvars);
    ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
    {
	end = 0;
	retlist = FALSE;
    }
    ଅନ୍ୟଥା
    {
	end = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_buffer_ଧାଡ଼ିଗୁଡ଼ିକ(curbuf, lnum, end, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    ଶୂନ୍ୟ
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_set_append_ଧାଡ଼ି(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    ଶୂନ୍ୟ
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;

    ଯଦି (in_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    lnum = tv_get_lnum(&argvars[0]);
    ଯଦି (did_emsg == did_emsg_before)
	set_buffer_ଧାଡ଼ିଗୁଡ଼ିକ(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#endif  // FEAT_EVAL

#ଯଦି defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
restore_buffer(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    ଯଦି (bufref_valid(save_curbuf))
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
find_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	ଯଦି ((*wp)->w_buffer == buf)
	    ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    ଶୂନ୍ୟ
switch_to_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    ଯଦି (find_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_buffer(save_curbuf, buf);
    ଅନ୍ୟଥା ଯଦି (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_buffer(save_curbuf, buf);
    }
}

    ଶୂନ୍ୟ
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    ଯଦି (save_curbuf->br_buf == ଖାଲି)
	restore_win(switchwin, TRUE);
    ଅନ୍ୟଥା
	restore_buffer(save_curbuf);
}
#endif
