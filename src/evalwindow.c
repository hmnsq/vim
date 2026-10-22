/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * evalwindow.c: Window related builtin functions
 */

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

    ایستا صحیح
win_getid(typval_T *argvars)
{
    صحیح	    winnr;
    win_T   *wp;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	بازگشت curwin->w_id;
    winnr = tv_get_عدد(&argvars[0]);
    اگر (winnr <= 0)
	بازگشت 0;

    اگر (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    وگرنه
    {
	tabpage_T	*tp;
	صحیح		tabnr = tv_get_عدد(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    اگر (--tabnr == 0)
		بشکن;
	اگر (tp == پوچ)
	    بازگشت -1;
	اگر (tp == curtab)
	    wp = firstwin;
	وگرنه
	    wp = tp->tp_firstwin;
    }
    برای ( ; wp != پوچ; wp = wp->w_next)
	اگر (--winnr == 0)
	    بازگشت wp->w_id;
    بازگشت 0;
}

    ایستا تهی
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    صحیح		winnr = 1;
    صحیح		tabnr = 1;
    صحیح		id = tv_get_عدد(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    اگر (wp->w_id == id)
	    {
		list_append_عدد(list, tabnr);
		list_append_عدد(list, winnr);
		بازگشت;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_عدد(list, 0);
    list_append_عدد(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(صحیح id)
{
    بازگشت win_id2wp_tp(id, پوچ);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(صحیح id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_id == id)
	{
	    اگر (tpp != پوچ)
		*tpp = tp;
	    بازگشت wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     اگر (wp->w_id == id)
	     {
		 اگر (tpp != پوچ)
		     *tpp = tp;
		 بازگشت wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_id == id)
	{
	    اگر (tpp != پوچ)
		*tpp = curtab;  // any tabpage would do
	    بازگشت wp;
	}
# endif

    بازگشت پوچ;
}

    ایستا صحیح
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    صحیح	    nr = 1;
    صحیح	    id = tv_get_عدد(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_id == id)
	    بازگشت nr;
	++nr;
    }
    بازگشت 0;
}

    تهی
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    صحیح		bufnr = tv_get_عدد(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp->w_buffer->b_fnum == bufnr)
		list_append_عدد(list, wp->w_id);
}

/*
 * Find window specified by "vp" in tabpage "tp".
 * Returns current window if "vp" is number zero.
 * Returns NULL if not found.
 */
    win_T *
find_win_by_nr(
    typval_T	*vp,
    tabpage_T	*tp)	// NULL for current tab page
{
    win_T	*wp;
    صحیح		nr = (صحیح)tv_get_عدد_chk(vp, پوچ);

    اگر (nr < 0)
	بازگشت پوچ;
    اگر (nr == 0)
	بازگشت curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	اگر (nr >= LOWEST_WIN_ID)
	{
	    اگر (wp->w_id == nr)
		بازگشت wp;
	}
	وگرنه اگر (--nr <= 0)
	    بشکن;
    }
    اگر (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	برای (wp = (tp == پوچ ? curtab : tp)->tp_first_popupwin;
						   wp != پوچ; wp = wp->w_next)
	    اگر (wp->w_id == nr)
		بازگشت wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    اگر (wp->w_id == nr)
		بازگشت wp;
# endif
	بازگشت پوچ;
    }
    بازگشت wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    صحیح	nr = (صحیح)tv_get_عدد_chk(vp, پوچ);

    اگر (nr >= LOWEST_WIN_ID)
	بازگشت win_id2wp(tv_get_عدد(vp));
    بازگشت find_win_by_nr(vp, پوچ);
}

/*
 * Find window specified by "wvp" in tabpage "tvp".
 * Returns the tab page in 'ptp'
 */
    win_T *
find_tabwin(
    typval_T	*wvp,	// VAR_UNKNOWN for current window
    typval_T	*tvp,	// VAR_UNKNOWN for current tab page
    tabpage_T	**ptp)
{
    win_T	*wp = پوچ;
    tabpage_T	*tp = پوچ;
    long	n;

    اگر (wvp->v_type != VAR_UNKNOWN)
    {
	اگر (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_عدد(tvp);
	    اگر (n >= 0)
		tp = find_tabpage(n);
	}
	وگرنه
	    tp = curtab;

	اگر (tp != پوچ)
	{
	    wp = find_win_by_nr(wvp, tp);
	    اگر (wp == پوچ && wvp->v_type == VAR_عدد
						&& wvp->vval.v_عدد != -1)
		// A window with the specified number is not found
		tp = پوچ;
	}
    }
    وگرنه
    {
	wp = curwin;
	tp = curtab;
    }

    اگر (ptp != پوچ)
	*ptp = tp;

    بازگشت wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    ایستا تهی
get_framelayout(frame_T *fr, list_T *l, صحیح outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    اگر (fr == پوچ)
	بازگشت;

    اگر (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    وگرنه
    {
	fr_list = list_alloc();
	اگر (fr_list == پوچ)
	    بازگشت;
	اگر (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    بازگشت;
	}
    }

    اگر (fr->fr_layout == FR_LEAF)
    {
	اگر (fr->fr_win != پوچ)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_عدد(fr_list, fr->fr_win->w_id);
	}
    }
    وگرنه
    {
	اگر (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	وگرنه
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	اگر (win_list == پوچ)
	    بازگشت;
	اگر (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    بازگشت;
	}

	child = fr->fr_child;
	وقتی (child != پوچ)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    ایستا صحیح
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    صحیح		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    اگر (argvar->v_type != VAR_UNKNOWN)
    {
	صحیح	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	اگر (arg == پوچ)
	    nr = 0;		// type error; errmsg already given
	وگرنه اگر (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	وگرنه اگر (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	وگرنه
	{
	    long	شمار;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    شمار = strtol((نویسه *)arg, (نویسه **)&endp, 10);
	    اگر (شمار <= 0)
		شمار = 1;	// if count is not specified, default to 1
	    اگر (endp != پوچ && *endp != '\0')
	    {
		اگر (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, شمار);
		وگرنه اگر (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, شمار);
		وگرنه اگر (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, شمار);
		وگرنه اگر (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, شمار);
		وگرنه
		    invalid_arg = TRUE;
	    }
	    وگرنه
		invalid_arg = TRUE;
	}
	اگر (twin == پوچ)
	    nr = 0;

	اگر (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    اگر (nr <= 0)
	بازگشت 0;

    برای (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	اگر (wp == پوچ)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    بشکن;
	}
	++nr;
    }
    بازگشت nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    ایستا dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    اگر (dict == پوچ)
	بازگشت پوچ;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_add_عدد(dict, "tabnr", tpnr);
    dict_add_عدد(dict, "winnr", winnr);
    dict_add_عدد(dict, "winid", wp->w_id);
    dict_add_عدد(dict, "height", wp->w_height);
    dict_add_عدد(dict, "status_height", wp->w_status_height);
    dict_add_عدد(dict, "winrow", wp->w_winrow + 1);
    dict_add_عدد(dict, "topline", wp->w_topline);
    dict_add_عدد(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_add_عدد(dict, "winbar", wp->w_winbar_height);
# endif
    dict_add_عدد(dict, "width", wp->w_width);
    dict_add_عدد(dict, "wincol", wp->w_wincol + 1);
    dict_add_عدد(dict, "textoff", win_col_off(wp));
    dict_add_عدد(dict, "bufnr", wp->w_buffer->b_fnum);
    dict_add_عدد(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_add_عدد(dict, "terminal", bt_terminal(wp->w_buffer));
# endif
# ifdef FEAT_QUICKFIX
    dict_add_عدد(dict, "quickfix", bt_quickfix(wp->w_buffer));
    dict_add_عدد(dict, "loclist",
		      (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != پوچ));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    بازگشت dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    ایستا dict_T *
get_tabpage_info(tabpage_T *tp, صحیح tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    اگر (dict == پوچ)
	بازگشت پوچ;

    dict_add_عدد(dict, "tabnr", tp_idx);

    l = list_alloc();
    اگر (l != پوچ)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_عدد(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    بازگشت dict;
}

/*
 * "gettabinfo()" function
 */
    تهی
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = پوچ;
    dict_T	*d;
    صحیح		tpnr = 0;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((صحیح)tv_get_عدد_chk(&argvars[0], پوچ));
	اگر (tparg == پوچ)
	    بازگشت;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	اگر (tparg != پوچ && tp != tparg)
	    ادامه;
	d = get_tabpage_info(tp, tpnr);
	اگر (d != پوچ)
	    list_append_dict(rettv->vval.v_list, d);
	اگر (tparg != پوچ)
	    بازگشت;
    }
}

/*
 * "getwininfo()" function
 */
    تهی
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = پوچ, *wparg = پوچ;
    dict_T	*d;
    short	tabnr = 0, winnr;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_عدد(&argvars[0]));
	اگر (wparg == پوچ)
	    بازگشت;
    }

    // Collect information about either all the windows across all the tab
    // pages or one particular window.
    FOR_ALL_TABPAGES(tp)
    {
	tabnr++;
	winnr = 0;
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    winnr++;
	    اگر (wparg != پوچ && wp != wparg)
		ادامه;
	    d = get_win_info(wp, tabnr, winnr);
	    اگر (d != پوچ)
		list_append_dict(rettv->vval.v_list, d);
	    اگر (wparg != پوچ)
		// found information about a specific window
		بازگشت;
	}
    }
# ifdef FEAT_PROP_POPUP
    اگر (wparg != پوچ)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		اگر (wp == wparg)
		    برو found;
	}
found:
	d = get_win_info(wparg, tp == پوچ ? 0 : tabnr, 0);
	اگر (d != پوچ)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    تهی
f_getwinpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحیح x = -1;
    صحیح y = -1;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

# اگر defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	اگر (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_عدد(&argvars[0]);

	(تهی)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_عدد(rettv->vval.v_list, (varnumber_T)x);
    list_append_عدد(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    تهی
f_getwinposx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;
# اگر defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	صحیح	    x, y;

	اگر (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_عدد = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    تهی
f_getwinposy(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;
# اگر defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	صحیح	    x, y;

	اگر (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_عدد = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    تهی
f_tabpagenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحیح		nr = 1;
    char_u	*arg;

    اگر (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	اگر (arg != پوچ)
	{
	    اگر (STRCMP(arg, "$") == 0)
		nr = tabpage_index(پوچ) - 1;
	    وگرنه اگر (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    وگرنه
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    وگرنه
	nr = tabpage_index(curtab);
    rettv->vval.v_عدد = nr;
}

/*
 * "tabpagewinnr()" function
 */
    تهی
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحیح		nr = 1;
    tabpage_T	*tp;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	بازگشت;

    tp = find_tabpage((صحیح)tv_get_عدد(&argvars[0]));
    اگر (tp == پوچ)
	nr = 0;
    وگرنه
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_عدد = nr;
}

/*
 * "win_execute()" function
 */
    تهی
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    صحیح		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	بازگشت;

    id = (صحیح)tv_get_عدد(argvars);
    wp = win_id2wp_tp(id, &tp);
    اگر (wp == پوچ || tp == پوچ)
	بازگشت;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    صحیح	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    صحیح	apply_acd = FALSE;
    char_u	*save_sfname = پوچ;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    اگر (curwin != wp
	    && (curwin->w_localdir != پوچ
		|| wp->w_localdir != پوچ
		|| (curtab != tp
		    && (curtab->tp_localdir != پوچ
			|| tp->tp_localdir != پوچ))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_status = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    اگر (cwd_status == OK && p_acd)
    {
	اگر (curbuf->b_sfname != پوچ && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    اگر (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    اگر (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    وگرنه
# endif
	اگر (cwd_status == OK)
	{
	    mch_chdir((نویسه *)cwd);
# ifdef FEAT_AUTOCHDIR
	    اگر (save_sfname != پوچ)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    اگر (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    اگر (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    تهی
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    تهی
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script()
	    && (check_for_opt_عدد_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 1) == FAIL)))
	بازگشت;

    rettv->vval.v_عدد = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    تهی
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    صحیح		id;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    id = tv_get_عدد(&argvars[0]);
    اگر (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_عدد = 1;
	بازگشت;
    }

    اگر (text_or_buf_locked())
	بازگشت;
# اگر defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    اگر (popup_is_popup(curwin) && curbuf->b_term != پوچ)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	بازگشت;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    اگر (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_عدد = 1;
	    بازگشت;
	}
}

/*
 * "win_id2tabwin()" function
 */
    تهی
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    تهی
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    rettv->vval.v_عدد = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    تهی
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحیح		offset;

    rettv->vval.v_عدد = FALSE;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    wp = find_win_by_nr_or_id(&argvars[0]);
    اگر (wp == پوچ || win_valid_popup(wp))
	بازگشت;
    اگر (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	بازگشت;
    }

    offset = (صحیح)tv_get_عدد(&argvars[1]);
    win_drag_vsep_سطر(wp, offset);
    rettv->vval.v_عدد = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    تهی
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحیح		offset;

    rettv->vval.v_عدد = FALSE;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	بازگشت;

    wp = find_win_by_nr_or_id(&argvars[0]);
    اگر (wp == پوچ || win_valid_popup(wp))
	بازگشت;
    اگر (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	بازگشت;
    }

    offset = (صحیح)tv_get_عدد(&argvars[1]);
    win_drag_status_سطر(wp, offset);
    rettv->vval.v_عدد = TRUE;
}

/*
 * "win_screenpos()" function
 */
    تهی
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_عدد(rettv->vval.v_list, wp == پوچ ? 0 : wp->w_winrow + 1);
    list_append_عدد(rettv->vval.v_list, wp == پوچ ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    تهی
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    صحیح     flags = 0, size = 0;

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	بازگشت;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    اگر (wp == پوچ || targetwin == پوچ || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_عدد));
	بازگشت;
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	اگر (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    بازگشت;

	d = argvars[2].vval.v_dict;
	اگر (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	اگر ((di = dict_find(d, (char_u *)"rightbelow", -1)) != پوچ)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (صحیح)dict_get_عدد(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    اگر (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	بازگشت;

    اگر (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    اگر (curwin == targetwin && win_valid(wp))
    {
	اگر (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_عدد = 0;
    }
    وگرنه
	emsg(_(e_autocommands_caused_command_to_abort));

    اگر (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    تهی
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	اگر (wp == پوچ)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    بازگشت;
	}
    }
    اگر (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# اگر defined(FEAT_QUICKFIX)
    وگرنه اگر (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    وگرنه اگر (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    وگرنه اگر (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    وگرنه اگر (bt_quickfix(wp->w_buffer))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != پوچ ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    تهی
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;
    rettv->vval.v_string = alloc(2);
    اگر (rettv->vval.v_string == پوچ)
	بازگشت;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    تهی
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    wp = find_win_by_nr_or_id(&argvars[0]);
    اگر (wp == پوچ)
	rettv->vval.v_عدد = -1;
    وگرنه
	rettv->vval.v_عدد = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    تهی
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    صحیح col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl)
	col = curwin->w_width - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_عدد = col;
}

/*
 * "winheight(nr)" function
 */
    تهی
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    wp = find_win_by_nr_or_id(&argvars[0]);
    اگر (wp == پوچ)
	rettv->vval.v_عدد = -1;
    وگرنه
	rettv->vval.v_عدد = wp->w_height;
}

/*
 * "winlayout()" function
 */
    تهی
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (in_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    وگرنه
    {
	tp = find_tabpage((صحیح)tv_get_عدد(&argvars[0]));
	اگر (tp == پوچ)
	    بازگشت;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    تهی
f_winline(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_عدد = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    تهی
f_winnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحیح		nr = 1;

    اگر (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	بازگشت;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_عدد = nr;
}

/*
 * "winrestcmd()" function
 */
    تهی
f_winrestcmd(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    صحیح		i;
    صحیح		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, اندازه(نویسه), 70);

    // Do this twice to handle some window layouts properly.
    برای (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((نویسه *)buf, اندازه(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((نویسه *)buf, اندازه(buf),
		"vert :%dresize %d|", winnr, wp->w_width);
	    ga_concat_len(&ga, buf, buflen);
	    ++winnr;
	}
    }
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_data;
    rettv->v_type = VAR_STRING;
}

/*
 * "winrestview()" function
 */
    تهی
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    اگر (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	بازگشت;

    dict = argvars[0].vval.v_dict;
    اگر (dict_has_کلید(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_عدد(dict, "lnum");
    اگر (dict_has_کلید(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_عدد(dict, "col");
    اگر (dict_has_کلید(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_عدد(dict, "coladd");
    اگر (dict_has_کلید(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_عدد(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    اگر (dict_has_کلید(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_عدد(dict, "topline"));
# ifdef FEAT_DIFF
    اگر (dict_has_کلید(dict, "topfill"))
	curwin->w_topfill = (صحیح)dict_get_عدد(dict, "topfill");
# endif
    اگر (dict_has_کلید(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_عدد(dict, "leftcol");
    اگر (dict_has_کلید(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_عدد(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    اگر (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    اگر (curwin->w_topline > curbuf->b_ml.ml_سطر_شمار)
	curwin->w_topline = curbuf->b_ml.ml_سطر_شمار;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    تهی
f_winsaveview(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	بازگشت;
    dict = rettv->vval.v_dict;

    dict_add_عدد(dict, "lnum", (long)curwin->w_cursor.lnum);
    dict_add_عدد(dict, "col", (long)curwin->w_cursor.col);
    dict_add_عدد(dict, "coladd", (long)curwin->w_cursor.coladd);
    update_curswant();
    dict_add_عدد(dict, "curswant", (long)curwin->w_curswant);

    dict_add_عدد(dict, "topline", (long)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_add_عدد(dict, "topfill", (long)curwin->w_topfill);
# endif
    dict_add_عدد(dict, "leftcol", (long)curwin->w_leftcol);
    dict_add_عدد(dict, "skipcol", (long)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    تهی
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (in_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	بازگشت;

    wp = find_win_by_nr_or_id(&argvars[0]);
    اگر (wp == پوچ)
	rettv->vval.v_عدد = -1;
    وگرنه
	rettv->vval.v_عدد = wp->w_width;
}
#endif // FEAT_EVAL

#اگر defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    صحیح
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	صحیح	    no_display)
{
    block_autocmds();
    بازگشت switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    صحیح
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	صحیح	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    اگر (win == curwin)
	switchwin->sw_same_win = TRUE;
    وگرنه
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    اگر (tp != پوچ)
    {
	switchwin->sw_curtab = curtab;
	اگر (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	وگرنه
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    اگر (!win_valid(win))
	بازگشت FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    بازگشت OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    تهی
restore_win(
	switchwin_T *switchwin,
	صحیح	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    تهی
restore_win_noblock(
	switchwin_T *switchwin,
	صحیح	    no_display)
{
    اگر (switchwin->sw_curtab != پوچ && valid_tabpage(switchwin->sw_curtab))
    {
	اگر (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	وگرنه
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    اگر (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    اگر (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    وگرنه اگر (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
