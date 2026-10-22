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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

    স্থির পূর্ণসংখ্যা
win_getid(typval_T *argvars)
{
    পূর্ণসংখ্যা	    winnr;
    win_T   *wp;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
	ফেরত curwin->w_id;
    winnr = tv_get_সংখ্যা(&argvars[0]);
    যদি (winnr <= 0)
	ফেরত 0;

    যদি (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    অন্যথা
    {
	tabpage_T	*tp;
	পূর্ণসংখ্যা		tabnr = tv_get_সংখ্যা(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    যদি (--tabnr == 0)
		বিরতি;
	যদি (tp == ফাঁকা)
	    ফেরত -1;
	যদি (tp == curtab)
	    wp = firstwin;
	অন্যথা
	    wp = tp->tp_firstwin;
    }
    জন্য ( ; wp != ফাঁকা; wp = wp->w_পরবর্তী)
	যদি (--winnr == 0)
	    ফেরত wp->w_id;
    ফেরত 0;
}

    স্থির শূন্য
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    পূর্ণসংখ্যা		winnr = 1;
    পূর্ণসংখ্যা		tabnr = 1;
    পূর্ণসংখ্যা		id = tv_get_সংখ্যা(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    যদি (wp->w_id == id)
	    {
		list_append_সংখ্যা(list, tabnr);
		list_append_সংখ্যা(list, winnr);
		ফেরত;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_সংখ্যা(list, 0);
    list_append_সংখ্যা(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(পূর্ণসংখ্যা id)
{
    ফেরত win_id2wp_tp(id, ফাঁকা);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(পূর্ণসংখ্যা id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	যদি (wp->w_id == id)
	{
	    যদি (tpp != ফাঁকা)
		*tpp = tp;
	    ফেরত wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     যদি (wp->w_id == id)
	     {
		 যদি (tpp != ফাঁকা)
		     *tpp = tp;
		 ফেরত wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	যদি (wp->w_id == id)
	{
	    যদি (tpp != ফাঁকা)
		*tpp = curtab;  // any tabpage would do
	    ফেরত wp;
	}
# endif

    ফেরত ফাঁকা;
}

    স্থির পূর্ণসংখ্যা
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    পূর্ণসংখ্যা	    nr = 1;
    পূর্ণসংখ্যা	    id = tv_get_সংখ্যা(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	যদি (wp->w_id == id)
	    ফেরত nr;
	++nr;
    }
    ফেরত 0;
}

    শূন্য
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    পূর্ণসংখ্যা		bufnr = tv_get_সংখ্যা(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    যদি (wp->w_বাফার->b_fnum == bufnr)
		list_append_সংখ্যা(list, wp->w_id);
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
    পূর্ণসংখ্যা		nr = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(vp, ফাঁকা);

    যদি (nr < 0)
	ফেরত ফাঁকা;
    যদি (nr == 0)
	ফেরত curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	যদি (nr >= LOWEST_WIN_ID)
	{
	    যদি (wp->w_id == nr)
		ফেরত wp;
	}
	অন্যথা যদি (--nr <= 0)
	    বিরতি;
    }
    যদি (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	জন্য (wp = (tp == ফাঁকা ? curtab : tp)->tp_first_popupwin;
						   wp != ফাঁকা; wp = wp->w_পরবর্তী)
	    যদি (wp->w_id == nr)
		ফেরত wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    যদি (wp->w_id == nr)
		ফেরত wp;
# endif
	ফেরত ফাঁকা;
    }
    ফেরত wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    পূর্ণসংখ্যা	nr = (পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(vp, ফাঁকা);

    যদি (nr >= LOWEST_WIN_ID)
	ফেরত win_id2wp(tv_get_সংখ্যা(vp));
    ফেরত find_win_by_nr(vp, ফাঁকা);
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
    win_T	*wp = ফাঁকা;
    tabpage_T	*tp = ফাঁকা;
    long	n;

    যদি (wvp->v_type != VAR_UNKNOWN)
    {
	যদি (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_সংখ্যা(tvp);
	    যদি (n >= 0)
		tp = find_tabpage(n);
	}
	অন্যথা
	    tp = curtab;

	যদি (tp != ফাঁকা)
	{
	    wp = find_win_by_nr(wvp, tp);
	    যদি (wp == ফাঁকা && wvp->v_type == VAR_সংখ্যা
						&& wvp->vval.v_সংখ্যা != -1)
		// A window with the specified number is not found
		tp = ফাঁকা;
	}
    }
    অন্যথা
    {
	wp = curwin;
	tp = curtab;
    }

    যদি (ptp != ফাঁকা)
	*ptp = tp;

    ফেরত wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    স্থির শূন্য
get_framelayout(frame_T *fr, list_T *l, পূর্ণসংখ্যা outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    যদি (fr == ফাঁকা)
	ফেরত;

    যদি (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    অন্যথা
    {
	fr_list = list_alloc();
	যদি (fr_list == ফাঁকা)
	    ফেরত;
	যদি (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    ফেরত;
	}
    }

    যদি (fr->fr_layout == FR_LEAF)
    {
	যদি (fr->fr_win != ফাঁকা)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_সংখ্যা(fr_list, fr->fr_win->w_id);
	}
    }
    অন্যথা
    {
	যদি (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	অন্যথা
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	যদি (win_list == ফাঁকা)
	    ফেরত;
	যদি (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    ফেরত;
	}

	child = fr->fr_child;
	যখন (child != ফাঁকা)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_পরবর্তী;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    স্থির পূর্ণসংখ্যা
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    পূর্ণসংখ্যা		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    যদি (argvar->v_type != VAR_UNKNOWN)
    {
	পূর্ণসংখ্যা	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	যদি (arg == ফাঁকা)
	    nr = 0;		// type error; errmsg already given
	অন্যথা যদি (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	অন্যথা যদি (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	অন্যথা
	{
	    long	গুনতি;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    গুনতি = strtol((অক্ষর *)arg, (অক্ষর **)&endp, 10);
	    যদি (গুনতি <= 0)
		গুনতি = 1;	// if count is not specified, default to 1
	    যদি (endp != ফাঁকা && *endp != '\0')
	    {
		যদি (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, গুনতি);
		অন্যথা যদি (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, গুনতি);
		অন্যথা যদি (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, গুনতি);
		অন্যথা যদি (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, গুনতি);
		অন্যথা
		    invalid_arg = TRUE;
	    }
	    অন্যথা
		invalid_arg = TRUE;
	}
	যদি (twin == ফাঁকা)
	    nr = 0;

	যদি (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    যদি (nr <= 0)
	ফেরত 0;

    জন্য (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_পরবর্তী)
    {
	যদি (wp == ফাঁকা)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    বিরতি;
	}
	++nr;
    }
    ফেরত nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    স্থির dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    যদি (dict == ফাঁকা)
	ফেরত ফাঁকা;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_add_সংখ্যা(dict, "tabnr", tpnr);
    dict_add_সংখ্যা(dict, "winnr", winnr);
    dict_add_সংখ্যা(dict, "winid", wp->w_id);
    dict_add_সংখ্যা(dict, "height", wp->w_height);
    dict_add_সংখ্যা(dict, "status_height", wp->w_status_height);
    dict_add_সংখ্যা(dict, "winrow", wp->w_winrow + 1);
    dict_add_সংখ্যা(dict, "topline", wp->w_topline);
    dict_add_সংখ্যা(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_add_সংখ্যা(dict, "winbar", wp->w_winbar_height);
# endif
    dict_add_সংখ্যা(dict, "width", wp->w_width);
    dict_add_সংখ্যা(dict, "wincol", wp->w_wincol + 1);
    dict_add_সংখ্যা(dict, "textoff", win_col_off(wp));
    dict_add_সংখ্যা(dict, "bufnr", wp->w_বাফার->b_fnum);
    dict_add_সংখ্যা(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_add_সংখ্যা(dict, "terminal", bt_terminal(wp->w_বাফার));
# endif
# ifdef FEAT_QUICKFIX
    dict_add_সংখ্যা(dict, "quickfix", bt_quickfix(wp->w_বাফার));
    dict_add_সংখ্যা(dict, "loclist",
		      (bt_quickfix(wp->w_বাফার) && wp->w_llist_ref != ফাঁকা));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    ফেরত dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    স্থির dict_T *
get_tabpage_info(tabpage_T *tp, পূর্ণসংখ্যা tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    যদি (dict == ফাঁকা)
	ফেরত ফাঁকা;

    dict_add_সংখ্যা(dict, "tabnr", tp_idx);

    l = list_alloc();
    যদি (l != ফাঁকা)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_সংখ্যা(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    ফেরত dict;
}

/*
 * "gettabinfo()" function
 */
    শূন্য
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = ফাঁকা;
    dict_T	*d;
    পূর্ণসংখ্যা		tpnr = 0;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((পূর্ণসংখ্যা)tv_get_সংখ্যা_chk(&argvars[0], ফাঁকা));
	যদি (tparg == ফাঁকা)
	    ফেরত;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	যদি (tparg != ফাঁকা && tp != tparg)
	    চালু;
	d = get_tabpage_info(tp, tpnr);
	যদি (d != ফাঁকা)
	    list_append_dict(rettv->vval.v_list, d);
	যদি (tparg != ফাঁকা)
	    ফেরত;
    }
}

/*
 * "getwininfo()" function
 */
    শূন্য
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = ফাঁকা, *wparg = ফাঁকা;
    dict_T	*d;
    short	tabnr = 0, winnr;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_সংখ্যা(&argvars[0]));
	যদি (wparg == ফাঁকা)
	    ফেরত;
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
	    যদি (wparg != ফাঁকা && wp != wparg)
		চালু;
	    d = get_win_info(wp, tabnr, winnr);
	    যদি (d != ফাঁকা)
		list_append_dict(rettv->vval.v_list, d);
	    যদি (wparg != ফাঁকা)
		// found information about a specific window
		ফেরত;
	}
    }
# ifdef FEAT_PROP_POPUP
    যদি (wparg != ফাঁকা)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		যদি (wp == wparg)
		    যাও found;
	}
found:
	d = get_win_info(wparg, tp == ফাঁকা ? 0 : tabnr, 0);
	যদি (d != ফাঁকা)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    শূন্য
f_getwinpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণসংখ্যা x = -1;
    পূর্ণসংখ্যা y = -1;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

# যদি defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	যদি (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_সংখ্যা(&argvars[0]);

	(শূন্য)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)x);
    list_append_সংখ্যা(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    শূন্য
f_getwinposx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = -1;
# যদি defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	পূর্ণসংখ্যা	    x, y;

	যদি (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_সংখ্যা = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    শূন্য
f_getwinposy(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_সংখ্যা = -1;
# যদি defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	পূর্ণসংখ্যা	    x, y;

	যদি (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_সংখ্যা = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    শূন্য
f_tabpagenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণসংখ্যা		nr = 1;
    char_u	*arg;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	যদি (arg != ফাঁকা)
	{
	    যদি (STRCMP(arg, "$") == 0)
		nr = tabpage_সূচক(ফাঁকা) - 1;
	    অন্যথা যদি (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_সূচক(lastused_tabpage) : 0;
	    অন্যথা
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    অন্যথা
	nr = tabpage_সূচক(curtab);
    rettv->vval.v_সংখ্যা = nr;
}

/*
 * "tabpagewinnr()" function
 */
    শূন্য
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণসংখ্যা		nr = 1;
    tabpage_T	*tp;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ফেরত;

    tp = find_tabpage((পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]));
    যদি (tp == ফাঁকা)
	nr = 0;
    অন্যথা
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_সংখ্যা = nr;
}

/*
 * "win_execute()" function
 */
    শূন্য
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    পূর্ণসংখ্যা		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ফেরত;

    id = (পূর্ণসংখ্যা)tv_get_সংখ্যা(argvars);
    wp = win_id2wp_tp(id, &tp);
    যদি (wp == ফাঁকা || tp == ফাঁকা)
	ফেরত;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    পূর্ণসংখ্যা	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    পূর্ণসংখ্যা	apply_acd = FALSE;
    char_u	*save_sfname = ফাঁকা;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    যদি (curwin != wp
	    && (curwin->w_localdir != ফাঁকা
		|| wp->w_localdir != ফাঁকা
		|| (curtab != tp
		    && (curtab->tp_localdir != ফাঁকা
			|| tp->tp_localdir != ফাঁকা))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_status = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    যদি (cwd_status == OK && p_acd)
    {
	যদি (curbuf->b_sfname != ফাঁকা && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    যদি (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    যদি (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    অন্যথা
# endif
	যদি (cwd_status == OK)
	{
	    mch_chdir((অক্ষর *)cwd);
# ifdef FEAT_AUTOCHDIR
	    যদি (save_sfname != ফাঁকা)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    যদি (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    যদি (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    শূন্য
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    শূন্য
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script()
	    && (check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_সংখ্যা_arg(argvars, 1) == FAIL)))
	ফেরত;

    rettv->vval.v_সংখ্যা = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    শূন্য
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    পূর্ণসংখ্যা		id;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    id = tv_get_সংখ্যা(&argvars[0]);
    যদি (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_সংখ্যা = 1;
	ফেরত;
    }

    যদি (text_or_buf_locked())
	ফেরত;
# যদি defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    যদি (popup_is_popup(curwin) && curbuf->b_term != ফাঁকা)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	ফেরত;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	যদি (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    যদি (VIsual_active && wp->w_বাফার != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_সংখ্যা = 1;
	    ফেরত;
	}
}

/*
 * "win_id2tabwin()" function
 */
    শূন্য
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    শূন্য
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    rettv->vval.v_সংখ্যা = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    শূন্য
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    পূর্ণসংখ্যা		offset;

    rettv->vval.v_সংখ্যা = FALSE;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == ফাঁকা || win_valid_popup(wp))
	ফেরত;
    যদি (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	ফেরত;
    }

    offset = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[1]);
    win_drag_vsep_সারি(wp, offset);
    rettv->vval.v_সংখ্যা = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    শূন্য
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    পূর্ণসংখ্যা		offset;

    rettv->vval.v_সংখ্যা = FALSE;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	ফেরত;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == ফাঁকা || win_valid_popup(wp))
	ফেরত;
    যদি (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	ফেরত;
    }

    offset = (পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[1]);
    win_drag_status_সারি(wp, offset);
    rettv->vval.v_সংখ্যা = TRUE;
}

/*
 * "win_screenpos()" function
 */
    শূন্য
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_সংখ্যা(rettv->vval.v_list, wp == ফাঁকা ? 0 : wp->w_winrow + 1);
    list_append_সংখ্যা(rettv->vval.v_list, wp == ফাঁকা ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    শূন্য
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    পূর্ণসংখ্যা     flags = 0, আকার_2 = 0;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ফেরত;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    যদি (wp == ফাঁকা || targetwin == ফাঁকা || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_সংখ্যা));
	ফেরত;
    }

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	যদি (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ফেরত;

	d = argvars[2].vval.v_dict;
	যদি (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	যদি ((di = dict_find(d, (char_u *)"rightbelow", -1)) != ফাঁকা)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	আকার_2 = (পূর্ণসংখ্যা)dict_get_সংখ্যা(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    যদি (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	ফেরত;

    যদি (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    যদি (curwin == targetwin && win_valid(wp))
    {
	যদি (win_splitmove(wp, আকার_2, flags) == OK)
	    rettv->vval.v_সংখ্যা = 0;
    }
    অন্যথা
	emsg(_(e_autocommands_caused_command_to_abort));

    যদি (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    শূন্য
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	যদি (wp == ফাঁকা)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    ফেরত;
	}
    }
    যদি (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# যদি defined(FEAT_QUICKFIX)
    অন্যথা যদি (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    অন্যথা যদি (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    অন্যথা যদি (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    অন্যথা যদি (bt_quickfix(wp->w_বাফার))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != ফাঁকা ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    শূন্য
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;
    rettv->vval.v_string = alloc(2);
    যদি (rettv->vval.v_string == ফাঁকা)
	ফেরত;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    শূন্য
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == ফাঁকা)
	rettv->vval.v_সংখ্যা = -1;
    অন্যথা
	rettv->vval.v_সংখ্যা = wp->w_বাফার->b_fnum;
}

/*
 * "wincol()" function
 */
    শূন্য
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    পূর্ণসংখ্যা col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    যদি (curwin->w_p_rl)
	col = curwin->w_width - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_সংখ্যা = col;
}

/*
 * "winheight(nr)" function
 */
    শূন্য
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == ফাঁকা)
	rettv->vval.v_সংখ্যা = -1;
    অন্যথা
	rettv->vval.v_সংখ্যা = wp->w_height;
}

/*
 * "winlayout()" function
 */
    শূন্য
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ফেরত;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    অন্যথা
    {
	tp = find_tabpage((পূর্ণসংখ্যা)tv_get_সংখ্যা(&argvars[0]));
	যদি (tp == ফাঁকা)
	    ফেরত;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    শূন্য
f_winline(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_সংখ্যা = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    শূন্য
f_winnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণসংখ্যা		nr = 1;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ফেরত;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_সংখ্যা = nr;
}

/*
 * "winrestcmd()" function
 */
    শূন্য
f_winrestcmd(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, আকার(অক্ষর), 70);

    // Do this twice to handle some window layouts properly.
    জন্য (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((অক্ষর *)buf, আকার(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((অক্ষর *)buf, আকার(buf),
		"vert :%dresize %d|", winnr, wp->w_width);
	    ga_concat_len(&ga, buf, buflen);
	    ++winnr;
	}
    }
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_তথ্য;
    rettv->v_type = VAR_STRING;
}

/*
 * "winrestview()" function
 */
    শূন্য
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    যদি (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ফেরত;

    dict = argvars[0].vval.v_dict;
    যদি (dict_has_চাবি(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_সংখ্যা(dict, "lnum");
    যদি (dict_has_চাবি(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_সংখ্যা(dict, "col");
    যদি (dict_has_চাবি(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_সংখ্যা(dict, "coladd");
    যদি (dict_has_চাবি(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_সংখ্যা(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    যদি (dict_has_চাবি(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_সংখ্যা(dict, "topline"));
# ifdef FEAT_DIFF
    যদি (dict_has_চাবি(dict, "topfill"))
	curwin->w_topfill = (পূর্ণসংখ্যা)dict_get_সংখ্যা(dict, "topfill");
# endif
    যদি (dict_has_চাবি(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_সংখ্যা(dict, "leftcol");
    যদি (dict_has_চাবি(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_সংখ্যা(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    যদি (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    যদি (curwin->w_topline > curbuf->b_ml.ml_সারি_গুনতি)
	curwin->w_topline = curbuf->b_ml.ml_সারি_গুনতি;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    শূন্য
f_winsaveview(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;

    যদি (rettv_dict_alloc(rettv) == FAIL)
	ফেরত;
    dict = rettv->vval.v_dict;

    dict_add_সংখ্যা(dict, "lnum", (long)curwin->w_cursor.lnum);
    dict_add_সংখ্যা(dict, "col", (long)curwin->w_cursor.col);
    dict_add_সংখ্যা(dict, "coladd", (long)curwin->w_cursor.coladd);
    update_curswant();
    dict_add_সংখ্যা(dict, "curswant", (long)curwin->w_curswant);

    dict_add_সংখ্যা(dict, "topline", (long)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_add_সংখ্যা(dict, "topfill", (long)curwin->w_topfill);
# endif
    dict_add_সংখ্যা(dict, "leftcol", (long)curwin->w_leftcol);
    dict_add_সংখ্যা(dict, "skipcol", (long)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    শূন্য
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	ফেরত;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == ফাঁকা)
	rettv->vval.v_সংখ্যা = -1;
    অন্যথা
	rettv->vval.v_সংখ্যা = wp->w_width;
}
#endif // FEAT_EVAL

#যদি defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    পূর্ণসংখ্যা
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	পূর্ণসংখ্যা	    no_display)
{
    block_autocmds();
    ফেরত switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    পূর্ণসংখ্যা
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	পূর্ণসংখ্যা	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    যদি (win == curwin)
	switchwin->sw_same_win = TRUE;
    অন্যথা
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    যদি (tp != ফাঁকা)
    {
	switchwin->sw_curtab = curtab;
	যদি (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	অন্যথা
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    যদি (!win_valid(win))
	ফেরত FAIL;
    curwin = win;
    curbuf = curwin->w_বাফার;
    ফেরত OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    শূন্য
restore_win(
	switchwin_T *switchwin,
	পূর্ণসংখ্যা	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    শূন্য
restore_win_noblock(
	switchwin_T *switchwin,
	পূর্ণসংখ্যা	    no_display)
{
    যদি (switchwin->sw_curtab != ফাঁকা && valid_tabpage(switchwin->sw_curtab))
    {
	যদি (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	অন্যথা
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    যদি (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    যদি (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_বাফার;
    }
# ifdef FEAT_PROP_POPUP
    অন্যথা যদি (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
