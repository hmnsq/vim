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

#ضمن "vim.h"

#إذا defined(FEAT_EVAL)
/*
 * Mark references in functions of buffers.
 */
    صحيح
مجموعه_ref_درون_buffers(صحيح copyID)
{
    صحيح		abort = FALSE;
    buf_T	*bp;

    FOR_ALL_BUFFERS(bp)
    {
	listener_T *lnr;

	لكل (lnr = bp->b_listener; !abort && lnr != عدم; lnr = lnr->lr_بعدی)
	    abort = abort || مجموعه_ref_درون_callback(&lnr->lr_callback, copyID);
# ifdef FEAT_کار_CHANNEL
	إذا (!abort)
	    abort = abort || مجموعه_ref_درون_callback(&bp->b_prompt_callback, copyID);
	إذا (!abort)
	    abort = abort || مجموعه_ref_درون_callback(&bp->b_prompt_وقفه, copyID);
# endif
# ifdef FEAT_COMPL_FUNC
	إذا (!abort)
	    abort = abort || مجموعه_ref_درون_callback(&bp->b_cfu_cb, copyID);
	إذا (!abort)
	    abort = abort || مجموعه_ref_درون_callback(&bp->b_ofu_cb, copyID);
	إذا (!abort)
	    abort = abort || مجموعه_ref_درون_callback(&bp->b_tsrfu_cb, copyID);
	إذا (!abort && bp->b_p_cpt_cb != عدم)
	    abort = abort || مجموعه_ref_درون_cpt_callbacks(bp->b_p_cpt_cb,
		    bp->b_p_cpt_شمار, copyID);
# endif
	إذا (!abort)
	    abort = abort || مجموعه_ref_درون_callback(&bp->b_tfu_cb, copyID);
	إذا (!abort)
	    abort = abort || مجموعه_ref_درون_callback(&bp->b_ffu_cb, copyID);
	إذا (abort)
	    اكسر;
    }
    أرجع abort;
}

    buf_T *
buflist_پیدا_کن_by_name(char_u *name, صحيح curtab_only)
{
    صحيح		save_magic;
    char_u	*save_cpo;
    buf_T	*buf;

    // Ignore 'magic' and 'cpoptions' here to make scripts portable
    save_magic = p_magic;
    p_magic = TRUE;
    save_cpo = p_cpo;
    p_cpo = خالی_option;

    buf = buflist_findnr(buflist_findpat(name, name + STRLEN(name),
						    TRUE, FALSE, curtab_only));

    p_magic = save_magic;
    p_cpo = save_cpo;
    أرجع buf;
}

/*
 * Find a buffer by number or exact name.
 */
    buf_T *
پیدا_کن_میانگیر(typval_T *avar)
{
    buf_T	*buf = عدم;

    إذا (avar->v_type == VAR_عدد)
	buf = buflist_findnr((صحيح)avar->vval.v_عدد);
    وإلا إذا (درون_vim9script() && check_for_string_arg(avar, 0) == FAIL)
	أرجع عدم;
    وإلا إذا (avar->v_type == VAR_STRING && avar->vval.v_string != عدم)
    {
	buf = buflist_findname_exp(avar->vval.v_string);
	إذا (buf == عدم)
	{
	    // No full path name match, try a match with a URL or a "nofile"
	    // buffer, these don't use the full path.
	    FOR_ALL_BUFFERS(buf)
		إذا (buf->b_fname != عدم
			&& (مسیر_with_url(buf->b_fname) || bt_nofilename(buf))
			&& STRCMP(buf->b_fname, avar->vval.v_string) == 0)
		    اكسر;
	}
    }
    أرجع buf;
}

/*
 * If there is a window for "curbuf", make it the current window.
 */
    ساكن فراغ
پیدا_کن_win_for_curbuf(فراغ)
{
    wininfo_T *wip;

    // The b_wininfo list should have the windows that recently contained the
    // buffer, going over this is faster than going over all the windows.
    // Do check the buffer is still there.
    FOR_ALL_BUF_WININFO(curbuf, wip)
    {
	إذا (wip->wi_win != عدم && wip->wi_win->w_میانگیر == curbuf)
	{
	    curwin = wip->wi_win;
	    اكسر;
	}
    }
}

تعريفنوع بنية {
    win_T	*cob_curwin_save;
    aco_save_T	cob_aco;
    صحيح		cob_using_aco;
    صحيح		cob_save_VIsual_فعال;
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
    ساكن فراغ
change_other_میانگیر_prepare(cob_T *cob, buf_T *buf)
{
    CLEAR_اشاره_گر(cob);

    // Set "curbuf" to the buffer being changed.  Then make sure there is a
    // window for it to handle any side effects.
    cob->cob_save_VIsual_فعال = VIsual_فعال;
    VIsual_فعال = FALSE;
    cob->cob_curwin_save = curwin;
    curbuf = buf;
    پیدا_کن_win_for_curbuf();  // simplest: find existing window for "buf"

    إذا (curwin->w_میانگیر != buf)
    {
	// No existing window for this buffer.  It is dangerous to have
	// curwin->w_buffer differ from "curbuf", use the autocmd window.
	curbuf = curwin->w_میانگیر;
	aucmd_prepbuf(&cob->cob_aco, buf);
	إذا (curbuf == buf)
	    cob->cob_using_aco = TRUE;
    }
}

    ساكن فراغ
change_other_میانگیر_restore(cob_T *cob)
{
    إذا (cob->cob_using_aco)
    {
	aucmd_restbuf(&cob->cob_aco);
    }
    وإلا
    {
	curwin = cob->cob_curwin_save;
	curbuf = curwin->w_میانگیر;
    }
    VIsual_فعال = cob->cob_save_VIsual_فعال;
}

/*
 * Set line or list of lines in buffer "buf" to "lines".
 * Any type is allowed and converted to a string.
 */
    ساكن فراغ
مجموعه_میانگیر_سطرها(
	buf_T	    *buf,
	linenr_T    lnum_arg,
	صحيح	    append,
	typval_T    *سطرها,
	typval_T    *rettv)
{
    linenr_T    lnum = lnum_arg + (append ? 1 : 0);
    char_u	*سطر = عدم;
    list_T	*l = عدم;
    listitem_T	*li = عدم;
    دراز	added = 0;
    linenr_T	append_lnum;

    // When using the current buffer ml_mfp will be set if needed.  Useful when
    // setline() is used on startup.  For other buffers the buffer must be
    // loaded.
    صحيح is_curbuf = buf == curbuf;
    إذا (buf == عدم || (!is_curbuf && buf->b_ml.ml_mfp == عدم) || lnum < 1)
    {
	rettv->vval.v_عدد = 1;	// FAIL
	إذا (درون_vim9script() && lnum < 1)
	    semsg(_(e_invalid_سطر_عدد_nr), lnum_arg);
	أرجع;
    }

    // After this don't use "return", goto "cleanup"!
    cob_T cob;
    إذا (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_میانگیر_prepare(&cob, buf);

    إذا (append)
	// appendbufline() uses the line number below which we insert
	append_lnum = lnum - 1;
    وإلا
	// setbufline() uses the line number above which we insert, we only
	// append if it's below the last line
	append_lnum = curbuf->b_ml.ml_سطر_شمار;

    إذا (سطرها->v_type == VAR_LIST)
    {
	l = سطرها->vval.v_list;
	إذا (l == عدم || list_len(l) == 0)
	{
	    // not appending anything always succeeds
	    اذهب انجامشده;
	}
	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_اولی;
    }
    وإلا
	سطر = typval_tostring(سطرها, FALSE);

    // default result is zero == OK
    لكل (;;)
    {
	إذا (l != عدم)
	{
	    // list argument, get next string
	    إذا (li == عدم)
		اكسر;
	    vim_free(سطر);
	    سطر = typval_tostring(&li->li_tv, FALSE);
	    li = li->li_بعدی;
	}

	rettv->vval.v_عدد = 1;	// FAIL
	إذا (سطر == عدم || lnum > curbuf->b_ml.ml_سطر_شمار + 1)
	    اكسر;

	// When coming here from Insert mode, sync undo, so that this can be
	// undone separately from what was previously inserted.
	إذا (u_sync_once == 2)
	{
	    u_sync_once = 1; // notify that u_sync() was called
	    u_sync(TRUE);
	}

	إذا (!append && lnum <= curbuf->b_ml.ml_سطر_شمار)
	{
	    // Existing line, replace it.
	    // Removes any existing text properties.
	    إذا (u_savesub(lnum) == OK && ml_replace_len(
		      lnum, سطر, (colnr_T)STRLEN(سطر) + 1, TRUE, TRUE) == OK)
	    {
		تغییریافته_بایتها(lnum, 0);
		إذا (is_curbuf && lnum == curwin->w_cursor.lnum)
		    check_cursor_ستون();
		rettv->vval.v_عدد = 0;	// OK
	    }
	}
	وإلا إذا (added > 0 || u_save(lnum - 1, lnum) == OK)
	{
	    // append the line
	    ++added;
	    إذا (ml_append(lnum - 1, سطر, (colnr_T)0, FALSE) == OK)
		rettv->vval.v_عدد = 0;	// OK
	}

	إذا (l == عدم)			// only one string argument
	    اكسر;
	++lnum;
    }
    vim_free(سطر);

    إذا (added > 0)
    {
	win_T	    *wp;
	tabpage_T   *tp;

	appended_سطرها_mark(append_lnum, added);

	// Only adjust the cursor for buffers other than the current, unless it
	// is the current window.  For curbuf and other windows it has been
	// done in mark_adjust_internal().
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    إذا (wp->w_میانگیر == buf
		    && (wp->w_میانگیر != curbuf || wp == curwin)
		    && wp->w_cursor.lnum > append_lnum)
		wp->w_cursor.lnum += added;
	check_cursor_ستون();

	// Only update the window view if w_buffer matches curbuf, otherwise
	// the computations will be wrong.
	إذا (curwin->w_میانگیر == curbuf)
	    update_topline();
    }

انجامشده:
    إذا (!is_curbuf)
	change_other_میانگیر_restore(&cob);
}

/*
 * "append(lnum, string/list)" function
 */
    فراغ
f_append(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    صحيح		did_emsg_before = did_emsg;

    إذا (درون_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	أرجع;

    lnum = tv_get_lnum(&argvars[0]);
    إذا (did_emsg == did_emsg_before)
	مجموعه_میانگیر_سطرها(curbuf, lnum, TRUE, &argvars[1], rettv);
}

/*
 * Set or append lines to a buffer.
 */
    ساكن فراغ
buf_مجموعه_append_سطر(typval_T *argvars, typval_T *rettv, صحيح append)
{
    linenr_T	lnum;
    buf_T	*buf;
    صحيح		did_emsg_before = did_emsg;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_string_or_عدد_or_list_arg(argvars, 2) == FAIL))
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	rettv->vval.v_عدد = 1; // FAIL
    وإلا
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	إذا (did_emsg == did_emsg_before)
	    مجموعه_میانگیر_سطرها(buf, lnum, append, &argvars[2], rettv);
    }
}

/*
 * "appendbufline(buf, lnum, string/list)" function
 */
    فراغ
f_appendbufline(typval_T *argvars, typval_T *rettv)
{
    buf_مجموعه_append_سطر(argvars, rettv, TRUE);
}

/*
 * "bufadd(expr)" function
 */
    فراغ
f_bufadd(typval_T *argvars, typval_T *rettv)
{
    char_u *name;

    إذا (درون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

    name = tv_get_string(&argvars[0]);
    rettv->vval.v_عدد = buflist_جمع_کن(*name == NUL ? عدم : name, 0);
}

/*
 * "bufexists(expr)" function
 */
    فراغ
f_bufexists(typval_T *argvars, typval_T *rettv)
{
    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_عدد = (پیدا_کن_میانگیر(&argvars[0]) != عدم);
}

/*
 * "buflisted(expr)" function
 */
    فراغ
f_buflisted(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = پیدا_کن_میانگیر(&argvars[0]);
    rettv->vval.v_عدد = (buf != عدم && buf->b_p_bl);
}

/*
 * "bufload(expr)" function
 */
    فراغ
f_bufload(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    buf_T	*buf;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = get_buf_arg(&argvars[0]);
    إذا (buf != عدم)
	میانگیر_ensure_loaded(buf);
}

/*
 * "bufloaded(expr)" function
 */
    فراغ
f_bufloaded(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = پیدا_کن_میانگیر(&argvars[0]);
    rettv->vval.v_عدد = (buf != عدم && buf->b_ml.ml_mfp != عدم);
}

/*
 * "bufname(expr)" function
 */
    فراغ
f_bufname(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    typval_T	*tv = &argvars[0];

    إذا (درون_vim9script() && check_for_opt_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (tv->v_type == VAR_UNKNOWN)
	buf = curbuf;
    وإلا
	buf = tv_get_buf_from_arg(tv);
    rettv->v_type = VAR_STRING;
    إذا (buf != عدم && buf->b_fname != عدم)
	rettv->vval.v_string = vim_strsave(buf->b_fname);
    وإلا
	rettv->vval.v_string = عدم;
}

/*
 * "bufnr(expr)" function
 */
    فراغ
f_bufnr(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    صحيح		error = FALSE;
    char_u	*name;

    إذا (درون_vim9script()
	    && (check_for_opt_میانگیر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_bool_arg(argvars, 1) == FAIL)))
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
	buf = curbuf;
    وإلا
	buf = tv_get_buf_from_arg(&argvars[0]);

    // If the buffer isn't found and the second argument is not zero create a
    // new buffer.
    إذا (buf == عدم
	    && argvars[1].v_type != VAR_UNKNOWN
	    && tv_get_bool_chk(&argvars[1], &error) != 0
	    && !error
	    && (name = tv_get_string_chk(&argvars[0])) != عدم
	    && !error)
	buf = buflist_new(name, عدم, (linenr_T)1, 0);

    إذا (buf != عدم)
	rettv->vval.v_عدد = buf->b_fnum;
    وإلا
	rettv->vval.v_عدد = -1;
}

    ساكن فراغ
buf_win_common(typval_T *argvars, typval_T *rettv, صحيح get_nr)
{
    win_T	*wp;
    صحيح		winnr = 0;
    buf_T	*buf;

    إذا (درون_vim9script() && check_for_میانگیر_arg(argvars, 0) == FAIL)
	أرجع;

    buf = tv_get_buf_from_arg(&argvars[0]);
    FOR_ALL_WINDOWS(wp)
    {
	++winnr;
	إذا (wp->w_میانگیر == buf)
	    اكسر;
    }
    rettv->vval.v_عدد = (wp != عدم ? (get_nr ? winnr : wp->w_id) : -1);
}

/*
 * "bufwinid(nr)" function
 */
    فراغ
f_bufwinid(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, FALSE);
}

/*
 * "bufwinnr(nr)" function
 */
    فراغ
f_bufwinnr(typval_T *argvars, typval_T *rettv)
{
    buf_win_common(argvars, rettv, TRUE);
}

/*
 * "deletebufline()" function
 */
    فراغ
f_deletebufline(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf;
    linenr_T	اولی, آخرین;
    linenr_T	lnum;
    دراز	شمار;
    صحيح		is_curbuf;
    tabpage_T	*tp;
    win_T	*wp;
    صحيح		did_emsg_before = did_emsg;

    rettv->vval.v_عدد = 1;	// FAIL by default

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	أرجع;

    buf = tv_get_buf(&argvars[0], FALSE);
    إذا (buf == عدم)
	أرجع;

    اولی = tv_get_lnum_buf(&argvars[1], buf);
    إذا (did_emsg > did_emsg_before)
	أرجع;
    إذا (argvars[2].v_type != VAR_UNKNOWN)
	آخرین = tv_get_lnum_buf(&argvars[2], buf);
    وإلا
	آخرین = اولی;

    إذا (buf->b_ml.ml_mfp == عدم || اولی < 1
			   || اولی > buf->b_ml.ml_سطر_شمار || آخرین < اولی)
	أرجع;

    // After this don't use "return", goto "cleanup"!
    is_curbuf = buf == curbuf;
    cob_T cob;
    إذا (!is_curbuf)
	// set "curbuf" to "buf" and find a window for this buffer
	change_other_میانگیر_prepare(&cob, buf);

    إذا (آخرین > curbuf->b_ml.ml_سطر_شمار)
	آخرین = curbuf->b_ml.ml_سطر_شمار;
    شمار = آخرین - اولی + 1;

    // When coming here from Insert mode, sync undo, so that this can be
    // undone separately from what was previously inserted.
    إذا (u_sync_once == 2)
    {
	u_sync_once = 1; // notify that u_sync() was called
	u_sync(TRUE);
    }

    إذا (u_save(اولی - 1, آخرین + 1) == FAIL)
	اذهب cleanup;

    لكل (lnum = اولی; lnum <= آخرین; ++lnum)
	ml_delete_flags(اولی, ML_DEL_پیام);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	إذا (wp->w_میانگیر == buf)
	{
	    إذا (wp->w_cursor.lnum > آخرین)
		wp->w_cursor.lnum -= شمار;
	    وإلا إذا (wp->w_cursor.lnum > اولی)
		wp->w_cursor.lnum = اولی;
	    إذا (wp->w_cursor.lnum > wp->w_میانگیر->b_ml.ml_سطر_شمار)
		wp->w_cursor.lnum = wp->w_میانگیر->b_ml.ml_سطر_شمار;
	    wp->w_valid = 0;
	    إذا (wp->w_cursor.lnum <= wp->w_topline)
		wp->w_topline = 1;
	}
    check_cursor_ستون();
    deleted_سطرها_mark(اولی, شمار);
    rettv->vval.v_عدد = 0; // OK

cleanup:
    إذا (!is_curbuf)
	change_other_میانگیر_restore(&cob);
}

/*
 * Returns buffer options, variables and other attributes in a dictionary.
 */
    ساكن dict_T *
get_میانگیر_info(buf_T *buf)
{
    dict_T	*dict;
    tabpage_T	*tp;
    win_T	*wp;
    list_T	*windows;

    dict = dict_alloc();
    إذا (dict == عدم)
	أرجع عدم;

    dict_جمع_کن_عدد(dict, "bufnr", buf->b_fnum);
    dict_جمع_کن_string(dict, "name", buf->b_ffname);
    dict_جمع_کن_عدد(dict, "lnum", buf == curbuf ? curwin->w_cursor.lnum
						     : buflist_findlnum(buf));
    dict_جمع_کن_عدد(dict, "linecount", buf->b_ml.ml_سطر_شمار);
    dict_جمع_کن_عدد(dict, "loaded", buf->b_ml.ml_mfp != عدم);
    dict_جمع_کن_عدد(dict, "listed", buf->b_p_bl);
    dict_جمع_کن_عدد(dict, "changed", buf_Is_تغییریافته(buf));
    dict_جمع_کن_عدد(dict, "changedtick", CHANGEDTICK(buf));
    dict_جمع_کن_عدد(dict, "hidden",
			    buf->b_ml.ml_mfp != عدم && buf->b_nwindows == 0);
    dict_جمع_کن_عدد(dict, "command", buf == cmdwin_buf);

    // Get a reference to buffer variables
    dict_جمع_کن_dict(dict, "variables", buf->b_vars);

    // List of windows displaying this buffer
    windows = list_alloc();
    إذا (windows != عدم)
    {
	FOR_ALL_TAB_WINDOWS(tp, wp)
	    إذا (wp->w_میانگیر == buf)
		list_append_عدد(windows, (varnumber_T)wp->w_id);
	dict_جمع_کن_list(dict, "windows", windows);
    }

# ifdef FEAT_PROP_POPUP
    // List of popup windows displaying this buffer
    windows = list_alloc();
    إذا (windows != عدم)
    {
	FOR_ALL_POPUPWINS(wp)
	    إذا (wp->w_میانگیر == buf)
		list_append_عدد(windows, (varnumber_T)wp->w_id);
	FOR_ALL_TABPAGES(tp)
	    FOR_ALL_POPUPWINS_درون_TAB(tp, wp)
		إذا (wp->w_میانگیر == buf)
		    list_append_عدد(windows, (varnumber_T)wp->w_id);

	dict_جمع_کن_list(dict, "popups", windows);
    }
# endif

# ifdef FEAT_SIGNS
    إذا (buf->b_signlist != عدم)
    {
	// List of signs placed in this buffer
	list_T	*signs = list_alloc();
	إذا (signs != عدم)
	{
	    get_میانگیر_signs(buf, signs);
	    dict_جمع_کن_list(dict, "signs", signs);
	}
    }
# endif

# ifdef FEAT_VIMINFO
    dict_جمع_کن_عدد(dict, "lastused", buf->b_آخرین_استفاده_شده);
# endif

    أرجع dict;
}

/*
 * "getbufinfo()" function
 */
    فراغ
f_getbufinfo(typval_T *argvars, typval_T *rettv)
{
    buf_T	*buf = عدم;
    buf_T	*argbuf = عدم;
    dict_T	*d;
    صحيح		filtered = FALSE;
    صحيح		sel_buflisted = FALSE;
    صحيح		sel_bufloaded = FALSE;
    صحيح		sel_bufmodified = FALSE;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (درون_vim9script()
	    && check_for_opt_میانگیر_or_dict_arg(argvars, 0) == FAIL)
	أرجع;

    // List of all the buffers or selected buffers
    إذا (argvars[0].v_type == VAR_DICT)
    {
	dict_T	*sel_d = argvars[0].vval.v_dict;

	إذا (sel_d != عدم)
	{
	    filtered = TRUE;
	    sel_buflisted = dict_get_bool(sel_d, "buflisted", FALSE);
	    sel_bufloaded = dict_get_bool(sel_d, "bufloaded", FALSE);
	    sel_bufmodified = dict_get_bool(sel_d, "bufmodified",
									FALSE);
	}
    }
    وإلا إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one buffer.  Argument specifies the buffer
	argbuf = tv_get_buf_from_arg(&argvars[0]);
	إذا (argbuf == عدم)
	    أرجع;
    }

    // Return information about all the buffers or a specified buffer
    FOR_ALL_BUFFERS(buf)
    {
	إذا (argbuf != عدم && argbuf != buf)
	    استمر;
	إذا (filtered && ((sel_bufloaded && buf->b_ml.ml_mfp == عدم)
			|| (sel_buflisted && !buf->b_p_bl)
			|| (sel_bufmodified && !buf->b_تغییریافته)))
	    استمر;

	d = get_میانگیر_info(buf);
	إذا (d != عدم)
	    list_append_dict(rettv->vval.v_list, d);
	إذا (argbuf != عدم)
	    أرجع;
    }
}

/*
 * Get line or list of lines from buffer "buf" into "rettv".
 * Return a range (from start to end) of lines in rettv from the specified
 * buffer.
 * If 'retlist' is TRUE, then the lines are returned as a Vim List.
 */
    ساكن فراغ
get_میانگیر_سطرها(
    buf_T	*buf,
    linenr_T	شروع,
    linenr_T	انتها,
    صحيح		retlist,
    typval_T	*rettv)
{
    إذا (!retlist)
    {
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = عدم;
    }

    إذا (buf == عدم || buf->b_ml.ml_mfp == عدم || شروع < 0)
    {
	إذا (retlist)
	    (فراغ)rettv_list_alloc(rettv);
	أرجع;
    }

    إذا (!retlist)
    {
	إذا (شروع >= 1 && شروع <= buf->b_ml.ml_سطر_شمار)
	    rettv->vval.v_string =
		vim_strnsave(ml_get_buf(buf, شروع, FALSE),
		    ml_get_buf_len(buf, شروع));
	وإلا
	    rettv->vval.v_string = vim_strnsave((char_u *)"", 0);
    }
    وإلا
    {
	list_T		*l;
	listitem_T	*li;

	شروع = MAX(شروع, 1);
	انتها = MIN(انتها, buf->b_ml.ml_سطر_شمار);
	إذا (انتها < شروع)
	    اذهب nolist;

	// The number of lines is known, so allocate the list and its items
	// in one allocation.
	l = list_alloc_with_اقلام((صحيح)(انتها - شروع + 1));
	إذا (l == عدم)
	    اذهب nolist;
	rettv_list_مجموعه(rettv, l);

	// The items are already linked and zeroed, so only the string needs
	// to be filled in.
	لكل (li = l->lv_اولی; شروع <= انتها; ++شروع, li = li->li_بعدی)
	{
	    li->li_tv.v_type = VAR_STRING;
	    li->li_tv.vval.v_string = vim_strnsave(ml_get_buf(buf, شروع, FALSE),
						   ml_get_buf_len(buf, شروع));
	}
    }
    أرجع;

nolist:
    (فراغ)rettv_list_alloc(rettv);
}

/*
 * "retlist" TRUE: "getbufline()" function
 * "retlist" FALSE: "getbufoneline()" function
 */
    ساكن فراغ
getbufline(typval_T *argvars, typval_T *rettv, صحيح retlist)
{
    linenr_T	lnum = 1;
    linenr_T	انتها = 1;
    buf_T	*buf;
    صحيح		did_emsg_before = did_emsg;

    إذا (درون_vim9script()
	    && (check_for_میانگیر_arg(argvars, 0) == FAIL
		|| check_for_lnum_arg(argvars, 1) == FAIL
		|| check_for_opt_lnum_arg(argvars, 2) == FAIL))
	أرجع;

    buf = tv_get_buf_from_arg(&argvars[0]);
    إذا (buf != عدم)
    {
	lnum = tv_get_lnum_buf(&argvars[1], buf);
	إذا (did_emsg > did_emsg_before)
	    أرجع;
	إذا (argvars[2].v_type == VAR_UNKNOWN)
	    انتها = lnum;
	وإلا
	    انتها = tv_get_lnum_buf(&argvars[2], buf);
    }

    get_میانگیر_سطرها(buf, lnum, انتها, retlist, rettv);
}

/*
 * "getbufline()" function
 */
    فراغ
f_getbufline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, TRUE);
}

/*
 * "getbufoneline()" function
 */
    فراغ
f_getbufoneline(typval_T *argvars, typval_T *rettv)
{
    getbufline(argvars, rettv, FALSE);
}

/*
 * "getline(lnum, [end])" function
 */
    فراغ
f_getline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    linenr_T	انتها;
    صحيح		retlist;

    إذا (درون_vim9script()
	    && (check_for_lnum_arg(argvars, 0) == FAIL
		|| check_for_opt_lnum_arg(argvars, 1) == FAIL))
	أرجع;

    lnum = tv_get_lnum(argvars);
    إذا (argvars[1].v_type == VAR_UNKNOWN)
    {
	انتها = 0;
	retlist = FALSE;
    }
    وإلا
    {
	انتها = tv_get_lnum(&argvars[1]);
	retlist = TRUE;
    }

    get_میانگیر_سطرها(curbuf, lnum, انتها, retlist, rettv);
}

/*
 * "setbufline()" function
 */
    فراغ
f_setbufline(typval_T *argvars, typval_T *rettv)
{
    buf_مجموعه_append_سطر(argvars, rettv, FALSE);
}

/*
 * "setline()" function
 */
    فراغ
f_setline(typval_T *argvars, typval_T *rettv)
{
    linenr_T	lnum;
    صحيح		did_emsg_before = did_emsg;

    إذا (درون_vim9script() && check_for_lnum_arg(argvars, 0) == FAIL)
	أرجع;

    lnum = tv_get_lnum(&argvars[0]);
    إذا (did_emsg == did_emsg_before)
	مجموعه_میانگیر_سطرها(curbuf, lnum, FALSE, &argvars[1], rettv);
}
#پایان_اگر  // FEAT_EVAL

#إذا defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Make "buf" the current buffer.  restore_buffer() MUST be called to undo.
 * No autocommands will be executed.  Use aucmd_prepbuf() if there are any.
 */
    فراغ
switch_میانگیر(bufref_T *save_curbuf, buf_T *buf)
{
    بلوک_autocmds();
# ifdef FEAT_FOLDING
    ++disable_fold_update;
# endif
    مجموعه_bufref(save_curbuf, curbuf);
    --curbuf->b_nwindows;
    curbuf = buf;
    curwin->w_میانگیر = buf;
    ++curbuf->b_nwindows;
}

/*
 * Restore the current buffer after using switch_buffer().
 */
    فراغ
restore_میانگیر(bufref_T *save_curbuf)
{
    unblock_autocmds();
# ifdef FEAT_FOLDING
    --disable_fold_update;
# endif
    // Check for valid buffer, just in case.
    إذا (bufref_valid(save_curbuf))
    {
	--curbuf->b_nwindows;
	curwin->w_میانگیر = save_curbuf->br_buf;
	curbuf = save_curbuf->br_buf;
	++curbuf->b_nwindows;
    }
}

/*
 * Find a window for buffer "buf".
 * If found OK is returned and "wp" and "tp" are set to the window and tabpage.
 * If not found FAIL is returned.
 */
    ساكن صحيح
پیدا_کن_win_for_buf(
    buf_T     *buf,
    win_T     **wp,
    tabpage_T **tp)
{
    FOR_ALL_TAB_WINDOWS(*tp, *wp)
	إذا ((*wp)->w_میانگیر == buf)
	    أرجع OK;
    أرجع FAIL;
}

/*
 * Find a window that contains "buf" and switch to it.
 * If there is no such window, use the current window and change "curbuf".
 * Caller must initialize save_curbuf to NULL.
 * restore_win_for_buf() MUST be called later!
 */
    فراغ
switch_مقصد_win_for_buf(
	buf_T	    *buf,
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    win_T	*wp;
    tabpage_T	*tp;

    إذا (پیدا_کن_win_for_buf(buf, &wp, &tp) == FAIL)
	switch_میانگیر(save_curbuf, buf);
    وإلا إذا (switch_win(switchwin, wp, tp, TRUE) == FAIL)
    {
	restore_win(switchwin, TRUE);
	switch_میانگیر(save_curbuf, buf);
    }
}

    فراغ
restore_win_for_buf(
	switchwin_T *switchwin,
	bufref_T    *save_curbuf)
{
    إذا (save_curbuf->br_buf == عدم)
	restore_win(switchwin, TRUE);
    وإلا
	restore_میانگیر(save_curbuf);
}
#پایان_اگر
