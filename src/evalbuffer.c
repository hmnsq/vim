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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    صحیح
سیٹ_ref_اندر_buffers(صحیح copyID)
{
    صحیح		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	برائے (lnr = bp->b_listener; !abort && lnr != تہی; lnr = lnr->lr_اگلا)
	    abort = abort || سیٹ_ref_اندر_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_کام_CHANNEL
	اگر (!abort)
	    abort = abort || سیٹ_ref_اندر_callback(&bp->b_prompt_callback, copyID);
	اگر (!abort)
	    abort = abort || سیٹ_ref_اندر_callback(&bp->b_prompt_مداخلت, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	اگر (!abort)
	    abort = abort || سیٹ_ref_اندر_callback(&bp->b_cfu_cb, copyID);
	اگر (!abort)
	    abort = abort || سیٹ_ref_اندر_callback(&bp->b_ofu_cb, copyID);
	اگر (!abort)
	    abort = abort || سیٹ_ref_اندر_callback(&bp->b_tsrfu_cb, copyID);
	اگر (!abort && bp->b_p_cpt_cb != تہی)
	    abort = abort || سیٹ_ref_اندر_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_گنتی, copyID);
# endif
	اگر (!abort)
	    abort = abort || سیٹ_ref_اندر_callback(&bp->b_tfu_cb, copyID);
	اگر (!abort)
	    abort = abort || سیٹ_ref_اندر_callback(&bp->b_ffu_cb, copyID);
	اگر (abort)
	    توڑو;
    }
    واپس abort;
}

    buf_T *
buflist_ڈھونڈو_by_name(char_u *name, صحیح curtab_only)
{
    صحیح		save_magic;
    char_u	*save_cpo;
    buf_T	*buf;

    // Ignore 'magic' and 'cpoptions' here to make scripts portable
    save_magic = p_magic;
    p_magic = TRUE;
    save_cpo = p_cpo;
    p_cpo = خالی_خانے_option;

    buf = buflist_findnr(buflist_findpat(name, name + STRLEN(name),
						    TRUE, FALSE, curtab_only));

    p_magic = save_magic;
    p_cpo = save_cpo;
    واپس buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
ڈھونڈو_بفر(typval_T *avar)
{
    buf_T	*buf = تہی;

    اگر (avar->v_type == VAR_نمبر)
	buf = buflist_findnr((صحیح)avar->vval.v_نمبر);
    ورنہ اگر (اندر_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	واپس تہی;
    ورنہ اگر (avar->v_type == VAR_STRING && avar->vval.v_string != تہی)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	اگر (buf == تہی)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		اگر (buf->b_fname != تہی
			&& (راستہ_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    توڑو;
	}
    }
    واپس buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    ساکن خالی
ڈھونڈو_win_for_curbuf(خالی)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	اگر (wip->wi_win != تہی && wip->wi_win->w_بفر == curbuf)
	{
	    curwin = wip->wi_win;
	    توڑو;
	}
    }
}

قسمتعریف ساخت {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    صحیح		cob_using_aco;
    صحیح		cob_save_VIsual_فعال;
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
    ساکن خالی
change_other_بفر_prepare(cob_T *cob, buf_T *buf)
{
    CLEAR_اشارہ_گر(cob);

    // Set "curbuf" to the buffer being changed.  Then make sure there is a
    // window for it to handle any side effects.
    cob->cob_save_VIsual_فعال = VIsual_فعال;
    VIsual_فعال = FALSE;
    cob->cob_curwin_save = curwin;
    curbuf = buf;
    ڈھونڈو_win_for_curbuf();  // simplest: find existing window for "buf"

    اگر (curwin->w_بفر != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_بفر;
	aucmd_prepbuf(&cob->cob_aco, buf);
	اگر (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    ساکن خالی
change_other_بفر_restore(cob_T *cob)
{
    اگر (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    ورنہ
    {
	curwin = cob->cob_curwin_save;
	curbuf = curwin->w_بفر;
    }
    VIsual_فعال = cob->cob_save_VIsual_فعال;
}

/*
 * Set line or list of lines in buffer "buf" to "lines".
 * Any type is allowed and converted to a string.
 */
    ساکن خالی
سیٹ_بفر_سطریں(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	صحیح	    append,
	typval_T    *سطریں,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*سطر = تہی;
    list_T	*l = تہی;
    listitem_T	*li = تہی;
    طویل	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    صحیح is_curbuf = buf == curbuf;
    اگر (buf == تہی || (!is_curbuf && buf->b_ml.ml_mfp == تہی) || lnum < 1)
    {
	rettv->vval.v_نمبر = 1;	// FAIL
	اگر (اندر_vim9script() && lnum < 1)
	    semsg(_(e_invalid_سطر_نمبر_nr), lnum_arg);
	واپس;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    اگر (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_بفر_prepare(&cob, buf);

    اگر (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    ورنہ
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_سطر_گنتی;

    اگر (سطریں->v_type == VAR_LIST)
    {
	l = سطریں->vval.v_list;
	اگر (l == تہی || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    جاؤ مکمل;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_پہلا;
    }
    ورنہ
	سطر = typval_tostring(سطریں, FALSE);

    // default result is zero == OK
    برائے (;;)
    {
	اگر (l != تہی)
	{
	    // list argument, get next string
	    اگر (li == تہی)
		توڑو;
	    vim_free(سطر);
	    سطر = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_اگلا;
	}

	rettv->vval.v_نمبر = 1;	// FAIL
	اگر (سطر == تہی || lnum > curbuf->b_ml.ml_سطر_گنتی + 1)
	    توڑو;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	اگر (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	اگر (!append && lnum <= curbuf->b_ml.ml_سطر_گنتی)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    اگر (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, سطر, (colnr_T)STRLEN(سطر) + 1, TRUE, TRUE) == OK)
	    {
		بدلا_بائٹیں(lnum, 0);
		اگر (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_ستون();
		rettv->vval.v_نمبر = 0;	// OK
	    }
	}
	ورنہ اگر (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    اگر (ml_append(lnum - 1, سطر, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_نمبر = 0;	// OK
	}

	اگر (l == تہی)			// only one string argument
	    توڑو;
	++lnum;
    }
    vim_free(سطر);

    اگر (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_سطریں_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp->w_بفر == buf
		    && (wp->w_بفر != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_ستون();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	اگر (curwin->w_بفر == curbuf)
	    update_topline();
    }

مکمل:
    اگر (!is_curbuf)
	change_other_بفر_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    خالی
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    صحیح		did_emsg_before = did_emsg;

    اگر (اندر_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	واپس;

    lnum = tv_get_lnum(&argvars[0]);
    اگر (did_emsg == did_emsg_before)
	سیٹ_بفر_سطریں(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    ساکن خالی
buf_سیٹ_append_سطر(typval_T *argvars, typval_T *rettv, صحیح append)
{
    linenr_T	lnum;
    buf_T	*buf;
    صحیح		did_emsg_before = did_emsg;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_نمبر_or_list_arg(argvars, 2) == FAIL))
	واپس;

    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == تہی)
	rettv->vval.v_نمبر = 1; // FAIL
    ورنہ
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	اگر (did_emsg == did_emsg_before)
	    سیٹ_بفر_سطریں(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    خالی
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_سیٹ_append_سطر(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    خالی
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_نمبر = buflist_جوڑو(*name == NUL ? تہی : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    خالی
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = (ڈھونڈو_بفر(&argvars[0]) != تہی);
}

/*
 * "buflisted(expr)" function
 */
    خالی
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = ڈھونڈو_بفر(&argvars[0]);
    rettv->vval.v_نمبر = (buf != تہی && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    خالی
f_bufload(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    buf_T	*buf;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = get_buf_arg(&argvars[0]);
    اگر (buf != تہی)
	بفر_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    خالی
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = ڈھونڈو_بفر(&argvars[0]);
    rettv->vval.v_نمبر = (buf != تہی && buf->b_ml.ml_mfp != تہی);
}

/*
 * "bufname(expr)" function
 */
    خالی
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    اگر (اندر_vim9script() && check_for_opt_بفر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    ورنہ
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    اگر (buf != تہی && buf->b_fname != تہی)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    ورنہ
	rettv->vval.v_string = تہی;
}

/*
 * "bufnr(expr)" function
 */
    خالی
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    صحیح		error = FALSE;
    char_u	*name;

    اگر (اندر_vim9script()
	    && (check_for_opt_بفر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	واپس;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    ورنہ
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    اگر (buf == تہی
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != تہی
	    && !error)
	buf = buflist_new(name, تہی, (linenr_T)1, 0);

    اگر (buf != تہی)
	rettv->vval.v_نمبر = buf->b_fnum;
    ورنہ
	rettv->vval.v_نمبر = -1;
}

    ساکن خالی
buf_win_common(typval_T *argvars, typval_T *rettv, صحیح get_nr)
{
    win_T	*wp;
    صحیح		winnr = 0;
    buf_T	*buf;

    اگر (اندر_vim9script() && check_for_بفر_arg(argvars, 0) == FAIL)
	واپس;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	اگر (wp->w_بفر == buf)
	    توڑو;
    }
    rettv->vval.v_نمبر = (wp != تہی ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    خالی
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    خالی
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    خالی
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	پہلا, آخری;
    linenr_T	lnum;
    طویل	گنتی;
    صحیح		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    صحیح		did_emsg_before = did_emsg;

    rettv->vval.v_نمبر = 1;	// FAIL by default

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	واپس;

    buf = tv_get_buf(&argvars[0], FALSE);
    اگر (buf == تہی)
	واپس;

    پہلا = tv_get_lnum_buf(&argvars[1], buf);
    اگر (did_emsg > did_emsg_before)
	واپس;
    اگر (argvars[2].v_type != VAR_UNKNOWN)
	آخری = tv_get_lnum_buf(&argvars[2], buf);
    ورنہ
	آخری = پہلا;

    اگر (buf->b_ml.ml_mfp == تہی || پہلا < 1
			   || پہلا > buf->b_ml.ml_سطر_گنتی || آخری < پہلا)
	واپس;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    اگر (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_بفر_prepare(&cob, buf);

    اگر (آخری > curbuf->b_ml.ml_سطر_گنتی)
	آخری = curbuf->b_ml.ml_سطر_گنتی;
    گنتی = آخری - پہلا + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    اگر (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    اگر (u_save(پہلا - 1, آخری + 1) == FAIL)
	جاؤ cleanup;

    برائے (lnum = پہلا; lnum <= آخری; ++lnum)
	ml_delete_flags(پہلا, ML_DEL_پیغام);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_بفر == buf)
	{
	    اگر (wp->w_cursor.lnum > آخری)
		wp->w_cursor.lnum -= گنتی;
	    ورنہ اگر (wp->w_cursor.lnum > پہلا)
		wp->w_cursor.lnum = پہلا;
	    اگر (wp->w_cursor.lnum > wp->w_بفر->b_ml.ml_سطر_گنتی)
		wp->w_cursor.lnum = wp->w_بفر->b_ml.ml_سطر_گنتی;
	    wp->w_valid = 0;
	    اگر (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_ستون();
    deleted_سطریں_mark(پہلا, گنتی);
    rettv->vval.v_نمبر = 0; // OK

cleanup:
    اگر (!is_curbuf)
	change_other_بفر_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    ساکن dict_T *
get_بفر_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    اگر (dict == تہی)
	واپس تہی;

    dict_جوڑو_نمبر(dict, "bufnr", buf->b_fnum);
    dict_جوڑو_string(dict, "name", buf->b_ffname);
    dict_جوڑو_نمبر(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_جوڑو_نمبر(dict, "linecount", buf->b_ml.ml_سطر_گنتی);
    dict_جوڑو_نمبر(dict, "loaded", buf->b_ml.ml_mfp != تہی);
    dict_جوڑو_نمبر(dict, "listed", buf->b_p_bl);
    dict_جوڑو_نمبر(dict, "changed", buf_Is_بدلا(buf));
    dict_جوڑو_نمبر(dict, "changedtick", CHANGEDTICK(buf));
    dict_جوڑو_نمبر(dict, "hidden",
			    buf->b_ml.ml_mfp != تہی && buf->b_nwindows == 0);
    dict_جوڑو_نمبر(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_جوڑو_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    اگر (windows != تہی)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp->w_بفر == buf)
		list_append_نمبر(windows, (varnumber_T)wp->w_id);
	dict_جوڑو_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    اگر (windows != تہی)
    {
	FOR_ALL_POPUPWINS(wp)
	    اگر (wp->w_بفر == buf)
		list_append_نمبر(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_اندر_TAB(tp, wp)
		اگر (wp->w_بفر == buf)
		    list_append_نمبر(windows, (varnumber_T)wp->w_id);

	dict_جوڑو_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    اگر (buf->b_signlist != تہی)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	اگر (signs != تہی)
	{
	    get_بفر_signs(buf, signs);
	    dict_جوڑو_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_جوڑو_نمبر(dict, "lastused", buf->b_آخری_مستعمل);
# endif

    واپس dict;
}

/*
 * "getbufinfo()" function
 */
    خالی
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = تہی;
    buf_T	*argbuf = تہی;
    dict_T	*d;
    صحیح		filtered = FALSE;
    صحیح		sel_buflisted = FALSE;
    صحیح		sel_bufloaded = FALSE;
    صحیح		sel_bufmodified = FALSE;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script()
	    && check_for_opt_بفر_or_dict_arg(argvars, 0) == FAIL)
	واپس;

    // List of all the buffers or selected buffers
    اگر (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	اگر (sel_d != تہی)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    ورنہ اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	اگر (argbuf == تہی)
	    واپس;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	اگر (argbuf != تہی && argbuf != buf)
	    جاری;
	اگر (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == تہی)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_بدلا)))
	    جاری;

	d = get_بفر_info(buf);
	اگر (d != تہی)
	    list_append_dict(rettv->vval.v_list, d);
	اگر (argbuf != تہی)
	    واپس;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    ساکن خالی
get_بفر_سطریں(
    buf_T	*buf,
    linenr_T	آغاز,
    linenr_T	انتہا,
    صحیح		retlist,
    typval_T	*rettv)
{
    اگر (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = تہی;
    }

    اگر (buf == تہی || buf->b_ml.ml_mfp == تہی || آغاز < 0)
    {
	اگر (retlist)
	    (خالی)rettv_list_alloc(rettv);
	واپس;
    }

    اگر (!retlist)
    {
	اگر (آغاز >= 1 && آغاز <= buf->b_ml.ml_سطر_گنتی)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, آغاز, FALSE),
		    ml_get_buf_len(buf, آغاز));
	ورنہ
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    ورنہ
    {
	list_T		*l;
	listitem_T	*li;

	آغاز = MAX(آغاز, 1);
	انتہا = MIN(انتہا, buf->b_ml.ml_سطر_گنتی);
	اگر (انتہا < آغاز)
	    جاؤ nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_اشیاء((صحیح)(انتہا - آغاز + 1));
	اگر (l == تہی)
	    جاؤ nolist;
	rettv_list_سیٹ(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	برائے (li = l->lv_پہلا; آغاز <= انتہا; ++آغاز, li = li->li_اگلا)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, آغاز, FALSE),
						   ml_get_buf_len(buf, آغاز));
	}
    }
    واپس;

nolist:
    (خالی)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    ساکن خالی
getbufline(typval_T *argvars, typval_T *rettv, صحیح retlist)
{
    linenr_T	lnum = 1;
    linenr_T	انتہا = 1;
    buf_T	*buf;
    صحیح		did_emsg_before = did_emsg;

    اگر (اندر_vim9script()
	    && (check_for_بفر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	واپس;

    buf = tv_get_buf_from_arg(&argvars[0]);
    اگر (buf != تہی)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	اگر (did_emsg > did_emsg_before)
	    واپس;
	اگر (argvars[2].v_type == VAR_UNKNOWN)
	    انتہا = lnum;
	ورنہ
	    انتہا = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_بفر_سطریں(buf, lnum, انتہا, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    خالی
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    خالی
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    خالی
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	انتہا;
    صحیح		retlist;

    اگر (اندر_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	واپس;

    lnum = tv_get_lnum(argvars);
    اگر (argvars[1].v_type == VAR_UNKNOWN)
    {
	انتہا = 0;
	retlist = FALSE;
    }
    ورنہ
    {
	انتہا = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_بفر_سطریں(curbuf, lnum, انتہا, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    خالی
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_سیٹ_append_سطر(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    خالی
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    صحیح		did_emsg_before = did_emsg;

    اگر (اندر_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	واپس;

    lnum = tv_get_lnum(&argvars[0]);
    اگر (did_emsg == did_emsg_before)
	سیٹ_بفر_سطریں(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#اگر_ختم  // FEAT_EVAL

#اگر defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    خالی
switch_بفر(bufref_T *save_curbuf, buf_T *buf)
{
    بلاک_autocmds();
# ifdef FEAT_FOLDING
    ++disable_fold_update;
# endif
    سیٹ_bufref(save_curbuf, curbuf);
    --curbuf->b_nwindows;
    curbuf = buf;
    curwin->w_بفر = buf;
    ++curbuf->b_nwindows;
}

/*
 * Restore the current buffer after using switch_buffer().
 */
    خالی
restore_بفر(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    اگر (bufref_valid(save_curbuf))
    {
	--curbuf->b_nwindows;
	curwin->w_بفر = save_curbuf->br_buf;
	curbuf = save_curbuf->br_buf;
	++curbuf->b_nwindows;
    }
}

/*
 * Find a window for buffer "buf".
 * If found OK is returned and "wp" and "tp" are set to the window and tabpage.
 * If not found FAIL is returned.
 */
    ساکن صحیح
ڈھونڈو_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	اگر ((*wp)->w_بفر == buf)
	    واپس OK;
    واپس FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    خالی
switch_مقصد_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    اگر (ڈھونڈو_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_بفر(save_curbuf, buf);
    ورنہ اگر (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_بفر(save_curbuf, buf);
    }
}

    خالی
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    اگر (save_curbuf->br_buf == تہی)
	restore_win(switchwin, TRUE);
    ورنہ
	restore_بفر(save_curbuf);
}
#اگر_ختم
