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

    স্থির পূর্ণাঙ্ক
win_getid(typval_T *argvars)
{
    পূর্ণাঙ্ক	    winnr;
    win_T   *wp;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
	প্রত্যাগম curwin->w_id;
    winnr = tv_get_সংখ্যা(&argvars[0]);
    যদি (winnr <= 0)
	প্রত্যাগম 0;

    যদি (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    অন্যথা
    {
	tabpage_T	*tp;
	পূর্ণাঙ্ক		tabnr = tv_get_সংখ্যা(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    যদি (--tabnr == 0)
		ভঙ্গ;
	যদি (tp == রিক্ত)
	    প্রত্যাগম -1;
	যদি (tp == curtab)
	    wp = firstwin;
	অন্যথা
	    wp = tp->tp_firstwin;
    }
    প্রতি ( ; wp != রিক্ত; wp = wp->w_next)
	যদি (--winnr == 0)
	    প্রত্যাগম wp->w_id;
    প্রত্যাগম 0;
}

    স্থির শূন্য
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    পূর্ণাঙ্ক		winnr = 1;
    পূর্ণাঙ্ক		tabnr = 1;
    পূর্ণাঙ্ক		id = tv_get_সংখ্যা(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    যদি (wp->w_id == id)
	    {
		list_append_সংখ্যা(list, tabnr);
		list_append_সংখ্যা(list, winnr);
		প্রত্যাগম;
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
win_id2wp(পূর্ণাঙ্ক id)
{
    প্রত্যাগম win_id2wp_tp(id, রিক্ত);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(পূর্ণাঙ্ক id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	যদি (wp->w_id == id)
	{
	    যদি (tpp != রিক্ত)
		*tpp = tp;
	    প্রত্যাগম wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     যদি (wp->w_id == id)
	     {
		 যদি (tpp != রিক্ত)
		     *tpp = tp;
		 প্রত্যাগম wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	যদি (wp->w_id == id)
	{
	    যদি (tpp != রিক্ত)
		*tpp = curtab;  // any tabpage would do
	    প্রত্যাগম wp;
	}
# endif

    প্রত্যাগম রিক্ত;
}

    স্থির পূর্ণাঙ্ক
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    পূর্ণাঙ্ক	    nr = 1;
    পূর্ণাঙ্ক	    id = tv_get_সংখ্যা(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	যদি (wp->w_id == id)
	    প্রত্যাগম nr;
	++nr;
    }
    প্রত্যাগম 0;
}

    শূন্য
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    পূর্ণাঙ্ক		bufnr = tv_get_সংখ্যা(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    যদি (wp->w_buffer->b_fnum == bufnr)
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
    পূর্ণাঙ্ক		nr = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(vp, রিক্ত);

    যদি (nr < 0)
	প্রত্যাগম রিক্ত;
    যদি (nr == 0)
	প্রত্যাগম curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	যদি (nr >= LOWEST_WIN_ID)
	{
	    যদি (wp->w_id == nr)
		প্রত্যাগম wp;
	}
	অন্যথা যদি (--nr <= 0)
	    ভঙ্গ;
    }
    যদি (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	প্রতি (wp = (tp == রিক্ত ? curtab : tp)->tp_first_popupwin;
						   wp != রিক্ত; wp = wp->w_next)
	    যদি (wp->w_id == nr)
		প্রত্যাগম wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    যদি (wp->w_id == nr)
		প্রত্যাগম wp;
# endif
	প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    পূর্ণাঙ্ক	nr = (পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(vp, রিক্ত);

    যদি (nr >= LOWEST_WIN_ID)
	প্রত্যাগম win_id2wp(tv_get_সংখ্যা(vp));
    প্রত্যাগম find_win_by_nr(vp, রিক্ত);
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
    win_T	*wp = রিক্ত;
    tabpage_T	*tp = রিক্ত;
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

	যদি (tp != রিক্ত)
	{
	    wp = find_win_by_nr(wvp, tp);
	    যদি (wp == রিক্ত && wvp->v_type == VAR_সংখ্যা
						&& wvp->vval.v_সংখ্যা != -1)
		// A window with the specified number is not found
		tp = রিক্ত;
	}
    }
    অন্যথা
    {
	wp = curwin;
	tp = curtab;
    }

    যদি (ptp != রিক্ত)
	*ptp = tp;

    প্রত্যাগম wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    স্থির শূন্য
get_framelayout(frame_T *fr, list_T *l, পূর্ণাঙ্ক outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    যদি (fr == রিক্ত)
	প্রত্যাগম;

    যদি (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    অন্যথা
    {
	fr_list = list_alloc();
	যদি (fr_list == রিক্ত)
	    প্রত্যাগম;
	যদি (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    প্রত্যাগম;
	}
    }

    যদি (fr->fr_layout == FR_LEAF)
    {
	যদি (fr->fr_win != রিক্ত)
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
	যদি (win_list == রিক্ত)
	    প্রত্যাগম;
	যদি (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    প্রত্যাগম;
	}

	child = fr->fr_child;
	যাবত্ (child != রিক্ত)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    স্থির পূর্ণাঙ্ক
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    পূর্ণাঙ্ক		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    যদি (argvar->v_type != VAR_UNKNOWN)
    {
	পূর্ণাঙ্ক	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	যদি (arg == রিক্ত)
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
	    গুনতি = strtol((বর্ণ *)arg, (বর্ণ **)&endp, 10);
	    যদি (গুনতি <= 0)
		গুনতি = 1;	// if count is not specified, default to 1
	    যদি (endp != রিক্ত && *endp != '\0')
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
	যদি (twin == রিক্ত)
	    nr = 0;

	যদি (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    যদি (nr <= 0)
	প্রত্যাগম 0;

    প্রতি (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	যদি (wp == রিক্ত)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    ভঙ্গ;
	}
	++nr;
    }
    প্রত্যাগম nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    স্থির dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    যদি (dict == রিক্ত)
	প্রত্যাগম রিক্ত;

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
    dict_add_সংখ্যা(dict, "bufnr", wp->w_buffer->b_fnum);
    dict_add_সংখ্যা(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_add_সংখ্যা(dict, "terminal", bt_terminal(wp->w_buffer));
# endif
# ifdef FEAT_QUICKFIX
    dict_add_সংখ্যা(dict, "quickfix", bt_quickfix(wp->w_buffer));
    dict_add_সংখ্যা(dict, "loclist",
		      (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != রিক্ত));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    প্রত্যাগম dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    স্থির dict_T *
get_tabpage_info(tabpage_T *tp, পূর্ণাঙ্ক tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    যদি (dict == রিক্ত)
	প্রত্যাগম রিক্ত;

    dict_add_সংখ্যা(dict, "tabnr", tp_idx);

    l = list_alloc();
    যদি (l != রিক্ত)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_সংখ্যা(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    প্রত্যাগম dict;
}

/*
 * "gettabinfo()" function
 */
    শূন্য
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = রিক্ত;
    dict_T	*d;
    পূর্ণাঙ্ক		tpnr = 0;

    যদি (rettv_list_alloc(rettv) == FAIL)
	প্রত্যাগম;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((পূর্ণাঙ্ক)tv_get_সংখ্যা_chk(&argvars[0], রিক্ত));
	যদি (tparg == রিক্ত)
	    প্রত্যাগম;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	যদি (tparg != রিক্ত && tp != tparg)
	    অনুবর্ত;
	d = get_tabpage_info(tp, tpnr);
	যদি (d != রিক্ত)
	    list_append_dict(rettv->vval.v_list, d);
	যদি (tparg != রিক্ত)
	    প্রত্যাগম;
    }
}

/*
 * "getwininfo()" function
 */
    শূন্য
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = রিক্ত, *wparg = রিক্ত;
    dict_T	*d;
    short	tabnr = 0, winnr;

    যদি (rettv_list_alloc(rettv) == FAIL)
	প্রত্যাগম;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_সংখ্যা(&argvars[0]));
	যদি (wparg == রিক্ত)
	    প্রত্যাগম;
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
	    যদি (wparg != রিক্ত && wp != wparg)
		অনুবর্ত;
	    d = get_win_info(wp, tabnr, winnr);
	    যদি (d != রিক্ত)
		list_append_dict(rettv->vval.v_list, d);
	    যদি (wparg != রিক্ত)
		// found information about a specific window
		প্রত্যাগম;
	}
    }
# ifdef FEAT_PROP_POPUP
    যদি (wparg != রিক্ত)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		যদি (wp == wparg)
		    গচ্ছ found;
	}
found:
	d = get_win_info(wparg, tp == রিক্ত ? 0 : tabnr, 0);
	যদি (d != রিক্ত)
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
    পূর্ণাঙ্ক x = -1;
    পূর্ণাঙ্ক y = -1;

    যদি (rettv_list_alloc(rettv) == FAIL)
	প্রত্যাগম;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

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
	পূর্ণাঙ্ক	    x, y;

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
	পূর্ণাঙ্ক	    x, y;

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
    পূর্ণাঙ্ক		nr = 1;
    char_u	*arg;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	যদি (arg != রিক্ত)
	{
	    যদি (STRCMP(arg, "$") == 0)
		nr = tabpage_index(রিক্ত) - 1;
	    অন্যথা যদি (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    অন্যথা
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    অন্যথা
	nr = tabpage_index(curtab);
    rettv->vval.v_সংখ্যা = nr;
}

/*
 * "tabpagewinnr()" function
 */
    শূন্য
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    পূর্ণাঙ্ক		nr = 1;
    tabpage_T	*tp;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    tp = find_tabpage((পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[0]));
    যদি (tp == রিক্ত)
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
    পূর্ণাঙ্ক		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	প্রত্যাগম;

    id = (পূর্ণাঙ্ক)tv_get_সংখ্যা(argvars);
    wp = win_id2wp_tp(id, &tp);
    যদি (wp == রিক্ত || tp == রিক্ত)
	প্রত্যাগম;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    পূর্ণাঙ্ক	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    পূর্ণাঙ্ক	apply_acd = FALSE;
    char_u	*save_sfname = রিক্ত;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    যদি (curwin != wp
	    && (curwin->w_localdir != রিক্ত
		|| wp->w_localdir != রিক্ত
		|| (curtab != tp
		    && (curtab->tp_localdir != রিক্ত
			|| tp->tp_localdir != রিক্ত))
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
	যদি (curbuf->b_sfname != রিক্ত && curbuf->b_fname == curbuf->b_sfname)
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
	    mch_chdir((বর্ণ *)cwd);
# ifdef FEAT_AUTOCHDIR
	    যদি (save_sfname != রিক্ত)
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
	প্রত্যাগম;

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
	প্রত্যাগম;

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
    পূর্ণাঙ্ক		id;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    id = tv_get_সংখ্যা(&argvars[0]);
    যদি (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_সংখ্যা = 1;
	প্রত্যাগম;
    }

    যদি (text_or_buf_locked())
	প্রত্যাগম;
# যদি defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    যদি (popup_is_popup(curwin) && curbuf->b_term != রিক্ত)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	প্রত্যাগম;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	যদি (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    যদি (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_সংখ্যা = 1;
	    প্রত্যাগম;
	}
}

/*
 * "win_id2tabwin()" function
 */
    শূন্য
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

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
	প্রত্যাগম;

    rettv->vval.v_সংখ্যা = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    শূন্য
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    পূর্ণাঙ্ক		offset;

    rettv->vval.v_সংখ্যা = FALSE;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == রিক্ত || win_valid_popup(wp))
	প্রত্যাগম;
    যদি (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	প্রত্যাগম;
    }

    offset = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[1]);
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
    পূর্ণাঙ্ক		offset;

    rettv->vval.v_সংখ্যা = FALSE;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL))
	প্রত্যাগম;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == রিক্ত || win_valid_popup(wp))
	প্রত্যাগম;
    যদি (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	প্রত্যাগম;
    }

    offset = (পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[1]);
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
	প্রত্যাগম;

    যদি (in_vim9script() && check_for_সংখ্যা_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_সংখ্যা(rettv->vval.v_list, wp == রিক্ত ? 0 : wp->w_winrow + 1);
    list_append_সংখ্যা(rettv->vval.v_list, wp == রিক্ত ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    শূন্য
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    পূর্ণাঙ্ক     flags = 0, size = 0;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_সংখ্যা_arg(argvars, 0) == FAIL
		|| check_for_সংখ্যা_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	প্রত্যাগম;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    যদি (wp == রিক্ত || targetwin == রিক্ত || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_সংখ্যা));
	প্রত্যাগম;
    }

    যদি (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	যদি (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    প্রত্যাগম;

	d = argvars[2].vval.v_dict;
	যদি (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	যদি ((di = dict_find(d, (char_u *)"rightbelow", -1)) != রিক্ত)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (পূর্ণাঙ্ক)dict_get_সংখ্যা(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    যদি (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	প্রত্যাগম;

    যদি (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    যদি (curwin == targetwin && win_valid(wp))
    {
	যদি (win_splitmove(wp, size, flags) == OK)
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
    rettv->vval.v_string = রিক্ত;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    যদি (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	যদি (wp == রিক্ত)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    প্রত্যাগম;
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
    অন্যথা যদি (bt_quickfix(wp->w_buffer))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != রিক্ত ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    শূন্য
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;
    rettv->vval.v_string = alloc(2);
    যদি (rettv->vval.v_string == রিক্ত)
	প্রত্যাগম;

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
	প্রত্যাগম;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == রিক্ত)
	rettv->vval.v_সংখ্যা = -1;
    অন্যথা
	rettv->vval.v_সংখ্যা = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    শূন্য
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    পূর্ণাঙ্ক col = curwin->w_wcol + 1;
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
	প্রত্যাগম;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == রিক্ত)
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
	প্রত্যাগম;

    যদি (in_vim9script() && check_for_opt_সংখ্যা_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

    যদি (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    অন্যথা
    {
	tp = find_tabpage((পূর্ণাঙ্ক)tv_get_সংখ্যা(&argvars[0]));
	যদি (tp == রিক্ত)
	    প্রত্যাগম;
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
    পূর্ণাঙ্ক		nr = 1;

    যদি (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

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
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, পরিমাণ(বর্ণ), 70);

    // Do this twice to handle some window layouts properly.
    প্রতি (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((বর্ণ *)buf, পরিমাণ(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((বর্ণ *)buf, পরিমাণ(buf),
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
    শূন্য
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    যদি (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	প্রত্যাগম;

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
	curwin->w_topfill = (পূর্ণাঙ্ক)dict_get_সংখ্যা(dict, "topfill");
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
	প্রত্যাগম;
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
	প্রত্যাগম;

    wp = find_win_by_nr_or_id(&argvars[0]);
    যদি (wp == রিক্ত)
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
    পূর্ণাঙ্ক
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	পূর্ণাঙ্ক	    no_display)
{
    block_autocmds();
    প্রত্যাগম switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    পূর্ণাঙ্ক
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	পূর্ণাঙ্ক	    no_display)
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

    যদি (tp != রিক্ত)
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
	প্রত্যাগম FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    প্রত্যাগম OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    শূন্য
restore_win(
	switchwin_T *switchwin,
	পূর্ণাঙ্ক	    no_display)
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
	পূর্ণাঙ্ক	    no_display)
{
    যদি (switchwin->sw_curtab != রিক্ত && valid_tabpage(switchwin->sw_curtab))
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
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    অন্যথা যদি (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
