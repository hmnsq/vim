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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

    സ്ഥിതം പൂർണ്ണസംഖ്യ
win_getid(typval_T *argvars)
{
    പൂർണ്ണസംഖ്യ	    winnr;
    win_T   *wp;

    എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
	തിരികെ curwin->w_id;
    winnr = tv_get_സംഖ്യ(&argvars[0]);
    എങ്കിൽ (winnr <= 0)
	തിരികെ 0;

    എങ്കിൽ (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    അല്ലെങ്കിൽ
    {
	tabpage_T	*tp;
	പൂർണ്ണസംഖ്യ		tabnr = tv_get_സംഖ്യ(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    എങ്കിൽ (--tabnr == 0)
		നിർത്തുക;
	എങ്കിൽ (tp == ശൂന്യ)
	    തിരികെ -1;
	എങ്കിൽ (tp == curtab)
	    wp = firstwin;
	അല്ലെങ്കിൽ
	    wp = tp->tp_firstwin;
    }
    വേണ്ടി ( ; wp != ശൂന്യ; wp = wp->w_next)
	എങ്കിൽ (--winnr == 0)
	    തിരികെ wp->w_id;
    തിരികെ 0;
}

    സ്ഥിതം ശൂന്യം
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    പൂർണ്ണസംഖ്യ		winnr = 1;
    പൂർണ്ണസംഖ്യ		tabnr = 1;
    പൂർണ്ണസംഖ്യ		id = tv_get_സംഖ്യ(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    എങ്കിൽ (wp->w_id == id)
	    {
		list_append_സംഖ്യ(list, tabnr);
		list_append_സംഖ്യ(list, winnr);
		തിരികെ;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_സംഖ്യ(list, 0);
    list_append_സംഖ്യ(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(പൂർണ്ണസംഖ്യ id)
{
    തിരികെ win_id2wp_tp(id, ശൂന്യ);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(പൂർണ്ണസംഖ്യ id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	എങ്കിൽ (wp->w_id == id)
	{
	    എങ്കിൽ (tpp != ശൂന്യ)
		*tpp = tp;
	    തിരികെ wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     എങ്കിൽ (wp->w_id == id)
	     {
		 എങ്കിൽ (tpp != ശൂന്യ)
		     *tpp = tp;
		 തിരികെ wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	എങ്കിൽ (wp->w_id == id)
	{
	    എങ്കിൽ (tpp != ശൂന്യ)
		*tpp = curtab;  // any tabpage would do
	    തിരികെ wp;
	}
# endif

    തിരികെ ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    പൂർണ്ണസംഖ്യ	    nr = 1;
    പൂർണ്ണസംഖ്യ	    id = tv_get_സംഖ്യ(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	എങ്കിൽ (wp->w_id == id)
	    തിരികെ nr;
	++nr;
    }
    തിരികെ 0;
}

    ശൂന്യം
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    പൂർണ്ണസംഖ്യ		bufnr = tv_get_സംഖ്യ(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    എങ്കിൽ (wp->w_buffer->b_fnum == bufnr)
		list_append_സംഖ്യ(list, wp->w_id);
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
    പൂർണ്ണസംഖ്യ		nr = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(vp, ശൂന്യ);

    എങ്കിൽ (nr < 0)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (nr == 0)
	തിരികെ curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	എങ്കിൽ (nr >= LOWEST_WIN_ID)
	{
	    എങ്കിൽ (wp->w_id == nr)
		തിരികെ wp;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (--nr <= 0)
	    നിർത്തുക;
    }
    എങ്കിൽ (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	വേണ്ടി (wp = (tp == ശൂന്യ ? curtab : tp)->tp_first_popupwin;
						   wp != ശൂന്യ; wp = wp->w_next)
	    എങ്കിൽ (wp->w_id == nr)
		തിരികെ wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    എങ്കിൽ (wp->w_id == nr)
		തിരികെ wp;
# endif
	തിരികെ ശൂന്യ;
    }
    തിരികെ wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    പൂർണ്ണസംഖ്യ	nr = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(vp, ശൂന്യ);

    എങ്കിൽ (nr >= LOWEST_WIN_ID)
	തിരികെ win_id2wp(tv_get_സംഖ്യ(vp));
    തിരികെ find_win_by_nr(vp, ശൂന്യ);
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
    win_T	*wp = ശൂന്യ;
    tabpage_T	*tp = ശൂന്യ;
    long	n;

    എങ്കിൽ (wvp->v_type != VAR_UNKNOWN)
    {
	എങ്കിൽ (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_സംഖ്യ(tvp);
	    എങ്കിൽ (n >= 0)
		tp = find_tabpage(n);
	}
	അല്ലെങ്കിൽ
	    tp = curtab;

	എങ്കിൽ (tp != ശൂന്യ)
	{
	    wp = find_win_by_nr(wvp, tp);
	    എങ്കിൽ (wp == ശൂന്യ && wvp->v_type == VAR_സംഖ്യ
						&& wvp->vval.v_സംഖ്യ != -1)
		// A window with the specified number is not found
		tp = ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ
    {
	wp = curwin;
	tp = curtab;
    }

    എങ്കിൽ (ptp != ശൂന്യ)
	*ptp = tp;

    തിരികെ wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    സ്ഥിതം ശൂന്യം
get_framelayout(frame_T *fr, list_T *l, പൂർണ്ണസംഖ്യ outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    എങ്കിൽ (fr == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    അല്ലെങ്കിൽ
    {
	fr_list = list_alloc();
	എങ്കിൽ (fr_list == ശൂന്യ)
	    തിരികെ;
	എങ്കിൽ (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    തിരികെ;
	}
    }

    എങ്കിൽ (fr->fr_layout == FR_LEAF)
    {
	എങ്കിൽ (fr->fr_win != ശൂന്യ)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_സംഖ്യ(fr_list, fr->fr_win->w_id);
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	അല്ലെങ്കിൽ
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	എങ്കിൽ (win_list == ശൂന്യ)
	    തിരികെ;
	എങ്കിൽ (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    തിരികെ;
	}

	child = fr->fr_child;
	ആകുമ്പോൾ (child != ശൂന്യ)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    പൂർണ്ണസംഖ്യ		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    എങ്കിൽ (argvar->v_type != VAR_UNKNOWN)
    {
	പൂർണ്ണസംഖ്യ	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	എങ്കിൽ (arg == ശൂന്യ)
	    nr = 0;		// type error; errmsg already given
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	അല്ലെങ്കിൽ
	{
	    long	എണ്ണം;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    എണ്ണം = strtol((അക്ഷരം *)arg, (അക്ഷരം **)&endp, 10);
	    എങ്കിൽ (എണ്ണം <= 0)
		എണ്ണം = 1;	// if count is not specified, default to 1
	    എങ്കിൽ (endp != ശൂന്യ && *endp != '\0')
	    {
		എങ്കിൽ (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, എണ്ണം);
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, എണ്ണം);
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, എണ്ണം);
		അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, എണ്ണം);
		അല്ലെങ്കിൽ
		    invalid_arg = TRUE;
	    }
	    അല്ലെങ്കിൽ
		invalid_arg = TRUE;
	}
	എങ്കിൽ (twin == ശൂന്യ)
	    nr = 0;

	എങ്കിൽ (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    എങ്കിൽ (nr <= 0)
	തിരികെ 0;

    വേണ്ടി (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	എങ്കിൽ (wp == ശൂന്യ)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    നിർത്തുക;
	}
	++nr;
    }
    തിരികെ nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    സ്ഥിതം dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    എങ്കിൽ (dict == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_add_സംഖ്യ(dict, "tabnr", tpnr);
    dict_add_സംഖ്യ(dict, "winnr", winnr);
    dict_add_സംഖ്യ(dict, "winid", wp->w_id);
    dict_add_സംഖ്യ(dict, "height", wp->w_height);
    dict_add_സംഖ്യ(dict, "status_height", wp->w_status_height);
    dict_add_സംഖ്യ(dict, "winrow", wp->w_winrow + 1);
    dict_add_സംഖ്യ(dict, "topline", wp->w_topline);
    dict_add_സംഖ്യ(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_add_സംഖ്യ(dict, "winbar", wp->w_winbar_height);
# endif
    dict_add_സംഖ്യ(dict, "width", wp->w_width);
    dict_add_സംഖ്യ(dict, "wincol", wp->w_wincol + 1);
    dict_add_സംഖ്യ(dict, "textoff", win_col_off(wp));
    dict_add_സംഖ്യ(dict, "bufnr", wp->w_buffer->b_fnum);
    dict_add_സംഖ്യ(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_add_സംഖ്യ(dict, "terminal", bt_terminal(wp->w_buffer));
# endif
# ifdef FEAT_QUICKFIX
    dict_add_സംഖ്യ(dict, "quickfix", bt_quickfix(wp->w_buffer));
    dict_add_സംഖ്യ(dict, "loclist",
		      (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != ശൂന്യ));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    തിരികെ dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    സ്ഥിതം dict_T *
get_tabpage_info(tabpage_T *tp, പൂർണ്ണസംഖ്യ tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    എങ്കിൽ (dict == ശൂന്യ)
	തിരികെ ശൂന്യ;

    dict_add_സംഖ്യ(dict, "tabnr", tp_idx);

    l = list_alloc();
    എങ്കിൽ (l != ശൂന്യ)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_സംഖ്യ(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    തിരികെ dict;
}

/*
 * "gettabinfo()" function
 */
    ശൂന്യം
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = ശൂന്യ;
    dict_T	*d;
    പൂർണ്ണസംഖ്യ		tpnr = 0;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ_chk(&argvars[0], ശൂന്യ));
	എങ്കിൽ (tparg == ശൂന്യ)
	    തിരികെ;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	എങ്കിൽ (tparg != ശൂന്യ && tp != tparg)
	    തുടരുക;
	d = get_tabpage_info(tp, tpnr);
	എങ്കിൽ (d != ശൂന്യ)
	    list_append_dict(rettv->vval.v_list, d);
	എങ്കിൽ (tparg != ശൂന്യ)
	    തിരികെ;
    }
}

/*
 * "getwininfo()" function
 */
    ശൂന്യം
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = ശൂന്യ, *wparg = ശൂന്യ;
    dict_T	*d;
    short	tabnr = 0, winnr;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_സംഖ്യ(&argvars[0]));
	എങ്കിൽ (wparg == ശൂന്യ)
	    തിരികെ;
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
	    എങ്കിൽ (wparg != ശൂന്യ && wp != wparg)
		തുടരുക;
	    d = get_win_info(wp, tabnr, winnr);
	    എങ്കിൽ (d != ശൂന്യ)
		list_append_dict(rettv->vval.v_list, d);
	    എങ്കിൽ (wparg != ശൂന്യ)
		// found information about a specific window
		തിരികെ;
	}
    }
# ifdef FEAT_PROP_POPUP
    എങ്കിൽ (wparg != ശൂന്യ)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		എങ്കിൽ (wp == wparg)
		    പോകുക found;
	}
found:
	d = get_win_info(wparg, tp == ശൂന്യ ? 0 : tabnr, 0);
	എങ്കിൽ (d != ശൂന്യ)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    ശൂന്യം
f_getwinpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ x = -1;
    പൂർണ്ണസംഖ്യ y = -1;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

# എങ്കിൽ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_സംഖ്യ(&argvars[0]);

	(ശൂന്യം)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)x);
    list_append_സംഖ്യ(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    ശൂന്യം
f_getwinposx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = -1;
# എങ്കിൽ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	പൂർണ്ണസംഖ്യ	    x, y;

	എങ്കിൽ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_സംഖ്യ = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    ശൂന്യം
f_getwinposy(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_സംഖ്യ = -1;
# എങ്കിൽ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	പൂർണ്ണസംഖ്യ	    x, y;

	എങ്കിൽ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_സംഖ്യ = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    ശൂന്യം
f_tabpagenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		nr = 1;
    char_u	*arg;

    എങ്കിൽ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	എങ്കിൽ (arg != ശൂന്യ)
	{
	    എങ്കിൽ (STRCMP(arg, "$") == 0)
		nr = tabpage_index(ശൂന്യ) - 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    അല്ലെങ്കിൽ
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    അല്ലെങ്കിൽ
	nr = tabpage_index(curtab);
    rettv->vval.v_സംഖ്യ = nr;
}

/*
 * "tabpagewinnr()" function
 */
    ശൂന്യം
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		nr = 1;
    tabpage_T	*tp;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    tp = find_tabpage((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]));
    എങ്കിൽ (tp == ശൂന്യ)
	nr = 0;
    അല്ലെങ്കിൽ
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_സംഖ്യ = nr;
}

/*
 * "win_execute()" function
 */
    ശൂന്യം
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	തിരികെ;

    id = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(argvars);
    wp = win_id2wp_tp(id, &tp);
    എങ്കിൽ (wp == ശൂന്യ || tp == ശൂന്യ)
	തിരികെ;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    പൂർണ്ണസംഖ്യ	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    പൂർണ്ണസംഖ്യ	apply_acd = FALSE;
    char_u	*save_sfname = ശൂന്യ;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    എങ്കിൽ (curwin != wp
	    && (curwin->w_localdir != ശൂന്യ
		|| wp->w_localdir != ശൂന്യ
		|| (curtab != tp
		    && (curtab->tp_localdir != ശൂന്യ
			|| tp->tp_localdir != ശൂന്യ))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_status = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    എങ്കിൽ (cwd_status == OK && p_acd)
    {
	എങ്കിൽ (curbuf->b_sfname != ശൂന്യ && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    എങ്കിൽ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    എങ്കിൽ (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    അല്ലെങ്കിൽ
# endif
	എങ്കിൽ (cwd_status == OK)
	{
	    mch_chdir((അക്ഷരം *)cwd);
# ifdef FEAT_AUTOCHDIR
	    എങ്കിൽ (save_sfname != ശൂന്യ)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    എങ്കിൽ (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    എങ്കിൽ (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    ശൂന്യം
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    ശൂന്യം
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script()
	    && (check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_സംഖ്യ_arg(argvars, 1) == FAIL)))
	തിരികെ;

    rettv->vval.v_സംഖ്യ = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    ശൂന്യം
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    പൂർണ്ണസംഖ്യ		id;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    id = tv_get_സംഖ്യ(&argvars[0]);
    എങ്കിൽ (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_സംഖ്യ = 1;
	തിരികെ;
    }

    എങ്കിൽ (text_or_buf_locked())
	തിരികെ;
# എങ്കിൽ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    എങ്കിൽ (popup_is_popup(curwin) && curbuf->b_term != ശൂന്യ)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	തിരികെ;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	എങ്കിൽ (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    എങ്കിൽ (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_സംഖ്യ = 1;
	    തിരികെ;
	}
}

/*
 * "win_id2tabwin()" function
 */
    ശൂന്യം
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    ശൂന്യം
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    rettv->vval.v_സംഖ്യ = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    ശൂന്യം
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		offset;

    rettv->vval.v_സംഖ്യ = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    എങ്കിൽ (wp == ശൂന്യ || win_valid_popup(wp))
	തിരികെ;
    എങ്കിൽ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	തിരികെ;
    }

    offset = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[1]);
    win_drag_vsep_വരി(wp, offset);
    rettv->vval.v_സംഖ്യ = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    ശൂന്യം
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		offset;

    rettv->vval.v_സംഖ്യ = FALSE;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL))
	തിരികെ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    എങ്കിൽ (wp == ശൂന്യ || win_valid_popup(wp))
	തിരികെ;
    എങ്കിൽ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	തിരികെ;
    }

    offset = (പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[1]);
    win_drag_status_വരി(wp, offset);
    rettv->vval.v_സംഖ്യ = TRUE;
}

/*
 * "win_screenpos()" function
 */
    ശൂന്യം
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_സംഖ്യ(rettv->vval.v_list, wp == ശൂന്യ ? 0 : wp->w_winrow + 1);
    list_append_സംഖ്യ(rettv->vval.v_list, wp == ശൂന്യ ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    ശൂന്യം
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    പൂർണ്ണസംഖ്യ     flags = 0, size = 0;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_സംഖ്യ_arg(argvars, 0) == FAIL
		|| check_for_സംഖ്യ_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	തിരികെ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    എങ്കിൽ (wp == ശൂന്യ || targetwin == ശൂന്യ || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_സംഖ്യ));
	തിരികെ;
    }

    എങ്കിൽ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    തിരികെ;

	d = argvars[2].vval.v_dict;
	എങ്കിൽ (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	എങ്കിൽ ((di = dict_find(d, (char_u *)"rightbelow", -1)) != ശൂന്യ)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    എങ്കിൽ (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	തിരികെ;

    എങ്കിൽ (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    എങ്കിൽ (curwin == targetwin && win_valid(wp))
    {
	എങ്കിൽ (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_സംഖ്യ = 0;
    }
    അല്ലെങ്കിൽ
	emsg(_(e_autocommands_caused_command_to_abort));

    എങ്കിൽ (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    ശൂന്യം
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	എങ്കിൽ (wp == ശൂന്യ)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    തിരികെ;
	}
    }
    എങ്കിൽ (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# എങ്കിൽ defined(FEAT_QUICKFIX)
    അല്ലെങ്കിൽ എങ്കിൽ (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    അല്ലെങ്കിൽ എങ്കിൽ (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    അല്ലെങ്കിൽ എങ്കിൽ (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    അല്ലെങ്കിൽ എങ്കിൽ (bt_quickfix(wp->w_buffer))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != ശൂന്യ ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    ശൂന്യം
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;
    rettv->vval.v_string = alloc(2);
    എങ്കിൽ (rettv->vval.v_string == ശൂന്യ)
	തിരികെ;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    ശൂന്യം
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    എങ്കിൽ (wp == ശൂന്യ)
	rettv->vval.v_സംഖ്യ = -1;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    ശൂന്യം
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    പൂർണ്ണസംഖ്യ col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    എങ്കിൽ (curwin->w_p_rl)
	col = curwin->w_width - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_സംഖ്യ = col;
}

/*
 * "winheight(nr)" function
 */
    ശൂന്യം
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    എങ്കിൽ (wp == ശൂന്യ)
	rettv->vval.v_സംഖ്യ = -1;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = wp->w_height;
}

/*
 * "winlayout()" function
 */
    ശൂന്യം
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (in_vim9script() && check_for_opt_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    എങ്കിൽ (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    അല്ലെങ്കിൽ
    {
	tp = find_tabpage((പൂർണ്ണസംഖ്യ)tv_get_സംഖ്യ(&argvars[0]));
	എങ്കിൽ (tp == ശൂന്യ)
	    തിരികെ;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    ശൂന്യം
f_winline(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_സംഖ്യ = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    ശൂന്യം
f_winnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    പൂർണ്ണസംഖ്യ		nr = 1;

    എങ്കിൽ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	തിരികെ;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_സംഖ്യ = nr;
}

/*
 * "winrestcmd()" function
 */
    ശൂന്യം
f_winrestcmd(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, വലുപ്പം(അക്ഷരം), 70);

    // Do this twice to handle some window layouts properly.
    വേണ്ടി (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((അക്ഷരം *)buf, വലുപ്പം(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((അക്ഷരം *)buf, വലുപ്പം(buf),
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
    ശൂന്യം
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    എങ്കിൽ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	തിരികെ;

    dict = argvars[0].vval.v_dict;
    എങ്കിൽ (dict_has_താക്കോൽ(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_സംഖ്യ(dict, "lnum");
    എങ്കിൽ (dict_has_താക്കോൽ(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_സംഖ്യ(dict, "col");
    എങ്കിൽ (dict_has_താക്കോൽ(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_സംഖ്യ(dict, "coladd");
    എങ്കിൽ (dict_has_താക്കോൽ(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_സംഖ്യ(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    എങ്കിൽ (dict_has_താക്കോൽ(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_സംഖ്യ(dict, "topline"));
# ifdef FEAT_DIFF
    എങ്കിൽ (dict_has_താക്കോൽ(dict, "topfill"))
	curwin->w_topfill = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(dict, "topfill");
# endif
    എങ്കിൽ (dict_has_താക്കോൽ(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_സംഖ്യ(dict, "leftcol");
    എങ്കിൽ (dict_has_താക്കോൽ(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_സംഖ്യ(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    എങ്കിൽ (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    എങ്കിൽ (curwin->w_topline > curbuf->b_ml.ml_വരി_എണ്ണം)
	curwin->w_topline = curbuf->b_ml.ml_വരി_എണ്ണം;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    ശൂന്യം
f_winsaveview(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;

    എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	തിരികെ;
    dict = rettv->vval.v_dict;

    dict_add_സംഖ്യ(dict, "lnum", (long)curwin->w_cursor.lnum);
    dict_add_സംഖ്യ(dict, "col", (long)curwin->w_cursor.col);
    dict_add_സംഖ്യ(dict, "coladd", (long)curwin->w_cursor.coladd);
    update_curswant();
    dict_add_സംഖ്യ(dict, "curswant", (long)curwin->w_curswant);

    dict_add_സംഖ്യ(dict, "topline", (long)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_add_സംഖ്യ(dict, "topfill", (long)curwin->w_topfill);
# endif
    dict_add_സംഖ്യ(dict, "leftcol", (long)curwin->w_leftcol);
    dict_add_സംഖ്യ(dict, "skipcol", (long)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    ശൂന്യം
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    എങ്കിൽ (in_vim9script() && check_for_സംഖ്യ_arg(argvars, 0) == FAIL)
	തിരികെ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    എങ്കിൽ (wp == ശൂന്യ)
	rettv->vval.v_സംഖ്യ = -1;
    അല്ലെങ്കിൽ
	rettv->vval.v_സംഖ്യ = wp->w_width;
}
#endif // FEAT_EVAL

#എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    പൂർണ്ണസംഖ്യ
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	പൂർണ്ണസംഖ്യ	    no_display)
{
    block_autocmds();
    തിരികെ switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    പൂർണ്ണസംഖ്യ
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	പൂർണ്ണസംഖ്യ	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    എങ്കിൽ (win == curwin)
	switchwin->sw_same_win = TRUE;
    അല്ലെങ്കിൽ
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    എങ്കിൽ (tp != ശൂന്യ)
    {
	switchwin->sw_curtab = curtab;
	എങ്കിൽ (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	അല്ലെങ്കിൽ
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    എങ്കിൽ (!win_valid(win))
	തിരികെ FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    തിരികെ OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    ശൂന്യം
restore_win(
	switchwin_T *switchwin,
	പൂർണ്ണസംഖ്യ	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    ശൂന്യം
restore_win_noblock(
	switchwin_T *switchwin,
	പൂർണ്ണസംഖ്യ	    no_display)
{
    എങ്കിൽ (switchwin->sw_curtab != ശൂന്യ && valid_tabpage(switchwin->sw_curtab))
    {
	എങ്കിൽ (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	അല്ലെങ്കിൽ
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    എങ്കിൽ (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    എങ്കിൽ (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    അല്ലെങ്കിൽ എങ്കിൽ (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
