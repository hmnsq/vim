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

    ساکن صحیح
win_getid(typval_T *argvars)
{
    صحیح	    winnr;
    win_T   *wp;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	واپس curwin->w_id;
    winnr = tv_get_نمبر(&argvars[0]);
    اگر (winnr <= 0)
	واپس 0;

    اگر (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    ورنہ
    {
	tabpage_T	*tp;
	صحیح		tabnr = tv_get_نمبر(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    اگر (--tabnr == 0)
		توڑو;
	اگر (tp == تہی)
	    واپس -1;
	اگر (tp == curtab)
	    wp = firstwin;
	ورنہ
	    wp = tp->tp_firstwin;
    }
    برائے ( ; wp != تہی; wp = wp->w_اگلا)
	اگر (--winnr == 0)
	    واپس wp->w_id;
    واپس 0;
}

    ساکن خالی
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    صحیح		winnr = 1;
    صحیح		tabnr = 1;
    صحیح		id = tv_get_نمبر(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
	{
	    اگر (wp->w_id == id)
	    {
		list_append_نمبر(list, tabnr);
		list_append_نمبر(list, winnr);
		واپس;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_نمبر(list, 0);
    list_append_نمبر(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(صحیح id)
{
    واپس win_id2wp_tp(id, تہی);
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
	    اگر (tpp != تہی)
		*tpp = tp;
	    واپس wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_اندر_TAB(tp, wp)
	     اگر (wp->w_id == id)
	     {
		 اگر (tpp != تہی)
		     *tpp = tp;
		 واپس wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	اگر (wp->w_id == id)
	{
	    اگر (tpp != تہی)
		*tpp = curtab;  // any tabpage would do
	    واپس wp;
	}
# endif

    واپس تہی;
}

    ساکن صحیح
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    صحیح	    nr = 1;
    صحیح	    id = tv_get_نمبر(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_id == id)
	    واپس nr;
	++nr;
    }
    واپس 0;
}

    خالی
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    صحیح		bufnr = tv_get_نمبر(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    اگر (wp->w_بفر->b_fnum == bufnr)
		list_append_نمبر(list, wp->w_id);
}

/*
 * Find window specified by "vp" in tabpage "tp".
 * Returns current window if "vp" is number zero.
 * Returns NULL if not found.
 */
    win_T *
ڈھونڈو_win_by_nr(
    typval_T	*vp,
    tabpage_T	*tp)	// NULL for current tab page
{
    win_T	*wp;
    صحیح		nr = (صحیح)tv_get_نمبر_chk(vp, تہی);

    اگر (nr < 0)
	واپس تہی;
    اگر (nr == 0)
	واپس curwin;

    FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
    {
	اگر (nr >= سب_سے_نیچا_WIN_ID)
	{
	    اگر (wp->w_id == nr)
		واپس wp;
	}
	ورنہ اگر (--nr <= 0)
	    توڑو;
    }
    اگر (nr >= سب_سے_نیچا_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	برائے (wp = (tp == تہی ? curtab : tp)->tp_پہلا_popupwin;
						   wp != تہی; wp = wp->w_اگلا)
	    اگر (wp->w_id == nr)
		واپس wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    اگر (wp->w_id == nr)
		واپس wp;
# endif
	واپس تہی;
    }
    واپس wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
ڈھونڈو_win_by_nr_or_id(typval_T *vp)
{
    صحیح	nr = (صحیح)tv_get_نمبر_chk(vp, تہی);

    اگر (nr >= سب_سے_نیچا_WIN_ID)
	واپس win_id2wp(tv_get_نمبر(vp));
    واپس ڈھونڈو_win_by_nr(vp, تہی);
}

/*
 * Find window specified by "wvp" in tabpage "tvp".
 * Returns the tab page in 'ptp'
 */
    win_T *
ڈھونڈو_tabwin(
    typval_T	*wvp,	// VAR_UNKNOWN for current window
    typval_T	*tvp,	// VAR_UNKNOWN for current tab page
    tabpage_T	**ptp)
{
    win_T	*wp = تہی;
    tabpage_T	*tp = تہی;
    طویل	n;

    اگر (wvp->v_type != VAR_UNKNOWN)
    {
	اگر (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (طویل)tv_get_نمبر(tvp);
	    اگر (n >= 0)
		tp = ڈھونڈو_tabpage(n);
	}
	ورنہ
	    tp = curtab;

	اگر (tp != تہی)
	{
	    wp = ڈھونڈو_win_by_nr(wvp, tp);
	    اگر (wp == تہی && wvp->v_type == VAR_نمبر
						&& wvp->vval.v_نمبر != -1)
		// A window with the specified number is not found
		tp = تہی;
	}
    }
    ورنہ
    {
	wp = curwin;
	tp = curtab;
    }

    اگر (ptp != تہی)
	*ptp = tp;

    واپس wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    ساکن خالی
get_framelayout(چوکھٹا_T *fr, list_T *l, صحیح outer)
{
    چوکھٹا_T	*بچہ;
    list_T	*fr_list;
    list_T	*win_list;

    اگر (fr == تہی)
	واپس;

    اگر (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    ورنہ
    {
	fr_list = list_alloc();
	اگر (fr_list == تہی)
	    واپس;
	اگر (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    واپس;
	}
    }

    اگر (fr->fr_layout == FR_برگ)
    {
	اگر (fr->fr_win != تہی)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_نمبر(fr_list, fr->fr_win->w_id);
	}
    }
    ورنہ
    {
	اگر (fr->fr_layout == FR_صف)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	ورنہ
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	اگر (win_list == تہی)
	    واپس;
	اگر (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    واپس;
	}

	بچہ = fr->fr_بچہ;
	جب (بچہ != تہی)
	{
	    get_framelayout(بچہ, win_list, FALSE);
	    بچہ = بچہ->fr_اگلا;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    ساکن صحیح
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
	اگر (arg == تہی)
	    nr = 0;		// type error; errmsg already given
	ورنہ اگر (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	ورنہ اگر (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	ورنہ
	{
	    طویل	گنتی;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    گنتی = strtol((حرف *)arg, (حرف **)&endp, 10);
	    اگر (گنتی <= 0)
		گنتی = 1;	// if count is not specified, default to 1
	    اگر (endp != تہی && *endp != '\0')
	    {
		اگر (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, گنتی);
		ورنہ اگر (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, گنتی);
		ورنہ اگر (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, گنتی);
		ورنہ اگر (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, گنتی);
		ورنہ
		    invalid_arg = TRUE;
	    }
	    ورنہ
		invalid_arg = TRUE;
	}
	اگر (twin == تہی)
	    nr = 0;

	اگر (invalid_arg)
	{
	    semsg(_(e_invalid_عبارت_str), arg);
	    nr = 0;
	}
    }

    اگر (nr <= 0)
	واپس 0;

    برائے (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_اگلا)
    {
	اگر (wp == تہی)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    توڑو;
	}
	++nr;
    }
    واپس nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    ساکن dict_T *
get_win_info(win_T *wp, مختصر tpnr, مختصر winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    اگر (dict == تہی)
	واپس تہی;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_جوڑو_نمبر(dict, "tabnr", tpnr);
    dict_جوڑو_نمبر(dict, "winnr", winnr);
    dict_جوڑو_نمبر(dict, "winid", wp->w_id);
    dict_جوڑو_نمبر(dict, "height", wp->w_اونچائی);
    dict_جوڑو_نمبر(dict, "status_height", wp->w_حالت_اونچائی);
    dict_جوڑو_نمبر(dict, "winrow", wp->w_winrow + 1);
    dict_جوڑو_نمبر(dict, "topline", wp->w_topline);
    dict_جوڑو_نمبر(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_جوڑو_نمبر(dict, "winbar", wp->w_winbar_اونچائی);
# endif
    dict_جوڑو_نمبر(dict, "width", wp->w_چوڑائی);
    dict_جوڑو_نمبر(dict, "wincol", wp->w_wincol + 1);
    dict_جوڑو_نمبر(dict, "textoff", win_ستون_off(wp));
    dict_جوڑو_نمبر(dict, "bufnr", wp->w_بفر->b_fnum);
    dict_جوڑو_نمبر(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_جوڑو_نمبر(dict, "terminal", bt_terminal(wp->w_بفر));
# endif
# ifdef FEAT_QUICKFIX
    dict_جوڑو_نمبر(dict, "quickfix", bt_quickfix(wp->w_بفر));
    dict_جوڑو_نمبر(dict, "loclist",
		      (bt_quickfix(wp->w_بفر) && wp->w_llist_ref != تہی));
# endif

    // Add a reference to window variables
    dict_جوڑو_dict(dict, "variables", wp->w_vars);

    واپس dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    ساکن dict_T *
get_tabpage_info(tabpage_T *tp, صحیح tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    اگر (dict == تہی)
	واپس تہی;

    dict_جوڑو_نمبر(dict, "tabnr", tp_idx);

    l = list_alloc();
    اگر (l != تہی)
    {
	FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
	    list_append_نمبر(l, (varnumber_T)wp->w_id);
	dict_جوڑو_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_جوڑو_dict(dict, "variables", tp->tp_vars);

    واپس dict;
}

/*
 * "gettabinfo()" function
 */
    خالی
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = تہی;
    dict_T	*d;
    صحیح		tpnr = 0;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = ڈھونڈو_tabpage((صحیح)tv_get_نمبر_chk(&argvars[0], تہی));
	اگر (tparg == تہی)
	    واپس;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	اگر (tparg != تہی && tp != tparg)
	    جاری;
	d = get_tabpage_info(tp, tpnr);
	اگر (d != تہی)
	    list_append_dict(rettv->vval.v_list, d);
	اگر (tparg != تہی)
	    واپس;
    }
}

/*
 * "getwininfo()" function
 */
    خالی
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = تہی, *wparg = تہی;
    dict_T	*d;
    مختصر	tabnr = 0, winnr;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_نمبر(&argvars[0]));
	اگر (wparg == تہی)
	    واپس;
    }

    // Collect information about either all the windows across all the tab
    // pages or one particular window.
    FOR_ALL_TABPAGES(tp)
    {
	tabnr++;
	winnr = 0;
	FOR_ALL_WINDOWS_اندر_TAB(tp, wp)
	{
	    winnr++;
	    اگر (wparg != تہی && wp != wparg)
		جاری;
	    d = get_win_info(wp, tabnr, winnr);
	    اگر (d != تہی)
		list_append_dict(rettv->vval.v_list, d);
	    اگر (wparg != تہی)
		// found information about a specific window
		واپس;
	}
    }
# ifdef FEAT_PROP_POPUP
    اگر (wparg != تہی)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_اندر_TAB(tp, wp)
		اگر (wp == wparg)
		    جاؤ ملا;
	}
ملا:
	d = get_win_info(wparg, tp == تہی ? 0 : tabnr, 0);
	اگر (d != تہی)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    خالی
f_getwinpos(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    صحیح x = -1;
    صحیح y = -1;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

# اگر defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	اگر (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_نمبر(&argvars[0]);

	(خالی)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)x);
    list_append_نمبر(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    خالی
f_getwinposx(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = -1;
# اگر defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	صحیح	    x, y;

	اگر (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_نمبر = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    خالی
f_getwinposy(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->vval.v_نمبر = -1;
# اگر defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	صحیح	    x, y;

	اگر (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_نمبر = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    خالی
f_tabpagenr(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    صحیح		nr = 1;
    char_u	*arg;

    اگر (اندر_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	اگر (arg != تہی)
	{
	    اگر (STRCMP(arg, "$") == 0)
		nr = tabpage_اشاریہ(تہی) - 1;
	    ورنہ اگر (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_اشاریہ(lastused_tabpage) : 0;
	    ورنہ
		semsg(_(e_invalid_عبارت_str), arg);
	}
    }
    ورنہ
	nr = tabpage_اشاریہ(curtab);
    rettv->vval.v_نمبر = nr;
}

/*
 * "tabpagewinnr()" function
 */
    خالی
f_tabpagewinnr(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    صحیح		nr = 1;
    tabpage_T	*tp;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    tp = ڈھونڈو_tabpage((صحیح)tv_get_نمبر(&argvars[0]));
    اگر (tp == تہی)
	nr = 0;
    ورنہ
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_نمبر = nr;
}

/*
 * "win_execute()" function
 */
    خالی
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    صحیح		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	واپس;

    id = (صحیح)tv_get_نمبر(argvars);
    wp = win_id2wp_tp(id, &tp);
    اگر (wp == تہی || tp == تہی)
	واپس;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    صحیح	cwd_حالت = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    صحیح	apply_acd = FALSE;
    char_u	*save_sfname = تہی;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    اگر (curwin != wp
	    && (curwin->w_localdir != تہی
		|| wp->w_localdir != تہی
		|| (curtab != tp
		    && (curtab->tp_localdir != تہی
			|| tp->tp_localdir != تہی))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_حالت = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    اگر (cwd_حالت == OK && p_acd)
    {
	اگر (curbuf->b_sfname != تہی && curbuf->b_fname == curbuf->b_sfname)
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
    ورنہ
# endif
	اگر (cwd_حالت == OK)
	{
	    mch_chdir((حرف *)cwd);
# ifdef FEAT_AUTOCHDIR
	    اگر (save_sfname != تہی)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    اگر (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_حالت = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    اگر (VIsual_فعال)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    خالی
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    خالی
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script()
	    && (check_for_opt_نمبر_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 1) == FAIL)))
	واپس;

    rettv->vval.v_نمبر = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    خالی
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    صحیح		id;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    id = tv_get_نمبر(&argvars[0]);
    اگر (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_نمبر = 1;
	واپس;
    }

    اگر (متن_or_buf_locked())
	واپس;
# اگر defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    اگر (popup_is_popup(curwin) && curbuf->b_رکن != تہی)
    {
	emsg(_(e_not_allowed_for_terminal_اندر_popup_کھڑکی));
	واپس;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	اگر (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    اگر (VIsual_فعال && wp->w_بفر != curbuf)
		انتہا_visual_منوال();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_نمبر = 1;
	    واپس;
	}
}

/*
 * "win_id2tabwin()" function
 */
    خالی
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    خالی
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    rettv->vval.v_نمبر = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    خالی
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحیح		انحراف;

    rettv->vval.v_نمبر = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    اگر (wp == تہی || win_valid_popup(wp))
	واپس;
    اگر (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_کھڑکی_اندر_another_tab_صفحہ));
	واپس;
    }

    انحراف = (صحیح)tv_get_نمبر(&argvars[1]);
    win_drag_vsep_سطر(wp, انحراف);
    rettv->vval.v_نمبر = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    خالی
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحیح		انحراف;

    rettv->vval.v_نمبر = FALSE;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    اگر (wp == تہی || win_valid_popup(wp))
	واپس;
    اگر (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_کھڑکی_اندر_another_tab_صفحہ));
	واپس;
    }

    انحراف = (صحیح)tv_get_نمبر(&argvars[1]);
    win_drag_حالت_سطر(wp, انحراف);
    rettv->vval.v_نمبر = TRUE;
}

/*
 * "win_screenpos()" function
 */
    خالی
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    list_append_نمبر(rettv->vval.v_list, wp == تہی ? 0 : wp->w_winrow + 1);
    list_append_نمبر(rettv->vval.v_list, wp == تہی ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    خالی
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    صحیح     flags = 0, سائز = 0;

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_نمبر_arg(argvars, 0) == FAIL
		|| check_for_نمبر_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	واپس;

    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    targetwin = ڈھونڈو_win_by_nr_or_id(&argvars[1]);

    اگر (wp == تہی || targetwin == تہی || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_کھڑکی_نمبر));
	واپس;
    }

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	اگر (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    واپس;

	d = argvars[2].vval.v_dict;
	اگر (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	اگر ((di = dict_ڈھونڈو(d, (char_u *)"rightbelow", -1)) != تہی)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_نیچے : WSP_ABOVE;
	سائز = (صحیح)dict_get_نمبر(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    اگر (متن_or_buf_locked() || check_بانٹو_disallowed(wp) == FAIL)
	واپس;

    اگر (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    اگر (curwin == targetwin && win_valid(wp))
    {
	اگر (win_splitmove(wp, سائز, flags) == OK)
	    rettv->vval.v_نمبر = 0;
    }
    ورنہ
	emsg(_(e_autocommands_caused_command_مقصد_abort));

    اگر (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    خالی
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
	اگر (wp == تہی)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    واپس;
	}
    }
    اگر (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# اگر defined(FEAT_QUICKFIX)
    ورنہ اگر (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    ورنہ اگر (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    ورنہ اگر (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    ورنہ اگر (bt_quickfix(wp->w_بفر))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != تہی ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    خالی
f_getcmdwintype(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;
    rettv->vval.v_string = alloc(2);
    اگر (rettv->vval.v_string == تہی)
	واپس;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    خالی
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    اگر (wp == تہی)
	rettv->vval.v_نمبر = -1;
    ورنہ
	rettv->vval.v_نمبر = wp->w_بفر->b_fnum;
}

/*
 * "wincol()" function
 */
    خالی
f_wincol(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    validate_cursor();
    صحیح ستون = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    اگر (curwin->w_p_rl)
	ستون = curwin->w_چوڑائی - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_نمبر = ستون;
}

/*
 * "winheight(nr)" function
 */
    خالی
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    اگر (wp == تہی)
	rettv->vval.v_نمبر = -1;
    ورنہ
	rettv->vval.v_نمبر = wp->w_اونچائی;
}

/*
 * "winlayout()" function
 */
    خالی
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (اندر_vim9script() && check_for_opt_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    ورنہ
    {
	tp = ڈھونڈو_tabpage((صحیح)tv_get_نمبر(&argvars[0]));
	اگر (tp == تہی)
	    واپس;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    خالی
f_winline(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_نمبر = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    خالی
f_winnr(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    صحیح		nr = 1;

    اگر (اندر_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	واپس;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_نمبر = nr;
}

/*
 * "winrestcmd()" function
 */
    خالی
f_winrestcmd(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    win_T	*wp;
    صحیح		i;
    صحیح		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, حجم(حرف), 70);

    // Do this twice to handle some window layouts properly.
    برائے (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
		":%dresize %d|", winnr, wp->w_اونچائی);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
		"vert :%dresize %d|", winnr, wp->w_چوڑائی);
	    ga_concat_len(&ga, buf, buflen);
	    ++winnr;
	}
    }
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_ڈیٹا;
    rettv->v_type = VAR_STRING;
}

/*
 * "winrestview()" function
 */
    خالی
f_winrestview(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    dict_T	*dict;

    اگر (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	واپس;

    dict = argvars[0].vval.v_dict;
    اگر (dict_has_کلید(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_نمبر(dict, "lnum");
    اگر (dict_has_کلید(dict, "col"))
	curwin->w_cursor.ستون = (colnr_T)dict_get_نمبر(dict, "col");
    اگر (dict_has_کلید(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_نمبر(dict, "coladd");
    اگر (dict_has_کلید(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_نمبر(dict, "curswant");
	curwin->w_سیٹ_curswant = false;
    }

    اگر (dict_has_کلید(dict, "topline"))
	سیٹ_topline(curwin, (linenr_T)dict_get_نمبر(dict, "topline"));
# ifdef FEAT_DIFF
    اگر (dict_has_کلید(dict, "topfill"))
	curwin->w_topfill = (صحیح)dict_get_نمبر(dict, "topfill");
# endif
    اگر (dict_has_کلید(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_نمبر(dict, "leftcol");
    اگر (dict_has_کلید(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_نمبر(dict, "skipcol");

    check_cursor();
    win_new_اونچائی(curwin, curwin->w_اونچائی);
    win_new_چوڑائی(curwin, curwin->w_چوڑائی);
    بدلا_کھڑکی_setting();

    اگر (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    اگر (curwin->w_topline > curbuf->b_ml.ml_سطر_گنتی)
	curwin->w_topline = curbuf->b_ml.ml_سطر_گنتی;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    خالی
f_winsaveview(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    dict_T	*dict;

    اگر (rettv_dict_alloc(rettv) == FAIL)
	واپس;
    dict = rettv->vval.v_dict;

    dict_جوڑو_نمبر(dict, "lnum", (طویل)curwin->w_cursor.lnum);
    dict_جوڑو_نمبر(dict, "col", (طویل)curwin->w_cursor.ستون);
    dict_جوڑو_نمبر(dict, "coladd", (طویل)curwin->w_cursor.coladd);
    update_curswant();
    dict_جوڑو_نمبر(dict, "curswant", (طویل)curwin->w_curswant);

    dict_جوڑو_نمبر(dict, "topline", (طویل)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_جوڑو_نمبر(dict, "topfill", (طویل)curwin->w_topfill);
# endif
    dict_جوڑو_نمبر(dict, "leftcol", (طویل)curwin->w_leftcol);
    dict_جوڑو_نمبر(dict, "skipcol", (طویل)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    خالی
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    اگر (اندر_vim9script() && check_for_نمبر_arg(argvars, 0) == FAIL)
	واپس;

    wp = ڈھونڈو_win_by_nr_or_id(&argvars[0]);
    اگر (wp == تہی)
	rettv->vval.v_نمبر = -1;
    ورنہ
	rettv->vval.v_نمبر = wp->w_چوڑائی;
}
#اگر_ختم // FEAT_EVAL

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
    بلاک_autocmds();
    واپس switch_win_noblock(switchwin, win, tp, no_display);
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
    CLEAR_اشارہ_گر(switchwin);
    switchwin->sw_curwin = curwin;
    اگر (win == curwin)
	switchwin->sw_same_win = TRUE;
    ورنہ
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_فعال = VIsual_فعال;
	VIsual_فعال = FALSE;
    }

    اگر (tp != تہی)
    {
	switchwin->sw_curtab = curtab;
	اگر (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	ورنہ
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    اگر (!win_valid(win))
	واپس FAIL;
    curwin = win;
    curbuf = curwin->w_بفر;
    واپس OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    خالی
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
    خالی
restore_win_noblock(
	switchwin_T *switchwin,
	صحیح	    no_display)
{
    اگر (switchwin->sw_curtab != تہی && valid_tabpage(switchwin->sw_curtab))
    {
	اگر (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	ورنہ
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    اگر (!switchwin->sw_same_win)
	VIsual_فعال = switchwin->sw_visual_فعال;

    اگر (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_بفر;
    }
# ifdef FEAT_PROP_POPUP
    ورنہ اگر (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#اگر_ختم
