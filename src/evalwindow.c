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

#अन्तर्भाव "vim.h"

#सचे defined(FEAT_EVAL)

    स्थिर पूर्णाङ्क
win_getid(typval_T *argvars)
{
    पूर्णाङ्क	    winnr;
    win_T   *wp;

    सचे (argvars[0].v_type == VAR_UNKNOWN)
	पटिनिवत्त curwin->w_id;
    winnr = tv_get_सङ्ख्या(&argvars[0]);
    सचे (winnr <= 0)
	पटिनिवत्त 0;

    सचे (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    अञ्ञथा
    {
	tabpage_T	*tp;
	पूर्णाङ्क		tabnr = tv_get_सङ्ख्या(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    सचे (--tabnr == 0)
		भङ्ग;
	सचे (tp == रिक्त)
	    पटिनिवत्त -1;
	सचे (tp == curtab)
	    wp = firstwin;
	अञ्ञथा
	    wp = tp->tp_firstwin;
    }
    अत्थाय ( ; wp != रिक्त; wp = wp->w_next)
	सचे (--winnr == 0)
	    पटिनिवत्त wp->w_id;
    पटिनिवत्त 0;
}

    स्थिर शून्य
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    पूर्णाङ्क		winnr = 1;
    पूर्णाङ्क		tabnr = 1;
    पूर्णाङ्क		id = tv_get_सङ्ख्या(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    सचे (wp->w_id == id)
	    {
		list_append_सङ्ख्या(list, tabnr);
		list_append_सङ्ख्या(list, winnr);
		पटिनिवत्त;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_सङ्ख्या(list, 0);
    list_append_सङ्ख्या(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(पूर्णाङ्क id)
{
    पटिनिवत्त win_id2wp_tp(id, रिक्त);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(पूर्णाङ्क id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	सचे (wp->w_id == id)
	{
	    सचे (tpp != रिक्त)
		*tpp = tp;
	    पटिनिवत्त wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     सचे (wp->w_id == id)
	     {
		 सचे (tpp != रिक्त)
		     *tpp = tp;
		 पटिनिवत्त wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	सचे (wp->w_id == id)
	{
	    सचे (tpp != रिक्त)
		*tpp = curtab;  // any tabpage would do
	    पटिनिवत्त wp;
	}
# endif

    पटिनिवत्त रिक्त;
}

    स्थिर पूर्णाङ्क
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    पूर्णाङ्क	    nr = 1;
    पूर्णाङ्क	    id = tv_get_सङ्ख्या(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	सचे (wp->w_id == id)
	    पटिनिवत्त nr;
	++nr;
    }
    पटिनिवत्त 0;
}

    शून्य
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    पूर्णाङ्क		bufnr = tv_get_सङ्ख्या(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    सचे (wp->w_buffer->b_fnum == bufnr)
		list_append_सङ्ख्या(list, wp->w_id);
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
    पूर्णाङ्क		nr = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(vp, रिक्त);

    सचे (nr < 0)
	पटिनिवत्त रिक्त;
    सचे (nr == 0)
	पटिनिवत्त curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	सचे (nr >= LOWEST_WIN_ID)
	{
	    सचे (wp->w_id == nr)
		पटिनिवत्त wp;
	}
	अञ्ञथा सचे (--nr <= 0)
	    भङ्ग;
    }
    सचे (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	अत्थाय (wp = (tp == रिक्त ? curtab : tp)->tp_first_popupwin;
						   wp != रिक्त; wp = wp->w_next)
	    सचे (wp->w_id == nr)
		पटिनिवत्त wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    सचे (wp->w_id == nr)
		पटिनिवत्त wp;
# endif
	पटिनिवत्त रिक्त;
    }
    पटिनिवत्त wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    पूर्णाङ्क	nr = (पूर्णाङ्क)tv_get_सङ्ख्या_chk(vp, रिक्त);

    सचे (nr >= LOWEST_WIN_ID)
	पटिनिवत्त win_id2wp(tv_get_सङ्ख्या(vp));
    पटिनिवत्त find_win_by_nr(vp, रिक्त);
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
    win_T	*wp = रिक्त;
    tabpage_T	*tp = रिक्त;
    long	n;

    सचे (wvp->v_type != VAR_UNKNOWN)
    {
	सचे (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_सङ्ख्या(tvp);
	    सचे (n >= 0)
		tp = find_tabpage(n);
	}
	अञ्ञथा
	    tp = curtab;

	सचे (tp != रिक्त)
	{
	    wp = find_win_by_nr(wvp, tp);
	    सचे (wp == रिक्त && wvp->v_type == VAR_सङ्ख्या
						&& wvp->vval.v_सङ्ख्या != -1)
		// A window with the specified number is not found
		tp = रिक्त;
	}
    }
    अञ्ञथा
    {
	wp = curwin;
	tp = curtab;
    }

    सचे (ptp != रिक्त)
	*ptp = tp;

    पटिनिवत्त wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    स्थिर शून्य
get_framelayout(frame_T *fr, list_T *l, पूर्णाङ्क outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    सचे (fr == रिक्त)
	पटिनिवत्त;

    सचे (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    अञ्ञथा
    {
	fr_list = list_alloc();
	सचे (fr_list == रिक्त)
	    पटिनिवत्त;
	सचे (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    पटिनिवत्त;
	}
    }

    सचे (fr->fr_layout == FR_LEAF)
    {
	सचे (fr->fr_win != रिक्त)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_सङ्ख्या(fr_list, fr->fr_win->w_id);
	}
    }
    अञ्ञथा
    {
	सचे (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	अञ्ञथा
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	सचे (win_list == रिक्त)
	    पटिनिवत्त;
	सचे (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    पटिनिवत्त;
	}

	child = fr->fr_child;
	याव (child != रिक्त)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    स्थिर पूर्णाङ्क
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    पूर्णाङ्क		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    सचे (argvar->v_type != VAR_UNKNOWN)
    {
	पूर्णाङ्क	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	सचे (arg == रिक्त)
	    nr = 0;		// type error; errmsg already given
	अञ्ञथा सचे (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	अञ्ञथा सचे (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	अञ्ञथा
	{
	    long	गणनम्;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    गणनम् = strtol((वर्ण *)arg, (वर्ण **)&endp, 10);
	    सचे (गणनम् <= 0)
		गणनम् = 1;	// if count is not specified, default to 1
	    सचे (endp != रिक्त && *endp != '\0')
	    {
		सचे (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, गणनम्);
		अञ्ञथा सचे (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, गणनम्);
		अञ्ञथा सचे (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, गणनम्);
		अञ्ञथा सचे (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, गणनम्);
		अञ्ञथा
		    invalid_arg = TRUE;
	    }
	    अञ्ञथा
		invalid_arg = TRUE;
	}
	सचे (twin == रिक्त)
	    nr = 0;

	सचे (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    सचे (nr <= 0)
	पटिनिवत्त 0;

    अत्थाय (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	सचे (wp == रिक्त)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    भङ्ग;
	}
	++nr;
    }
    पटिनिवत्त nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    स्थिर dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    सचे (dict == रिक्त)
	पटिनिवत्त रिक्त;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_add_सङ्ख्या(dict, "tabnr", tpnr);
    dict_add_सङ्ख्या(dict, "winnr", winnr);
    dict_add_सङ्ख्या(dict, "winid", wp->w_id);
    dict_add_सङ्ख्या(dict, "height", wp->w_height);
    dict_add_सङ्ख्या(dict, "status_height", wp->w_status_height);
    dict_add_सङ्ख्या(dict, "winrow", wp->w_winrow + 1);
    dict_add_सङ्ख्या(dict, "topline", wp->w_topline);
    dict_add_सङ्ख्या(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_add_सङ्ख्या(dict, "winbar", wp->w_winbar_height);
# endif
    dict_add_सङ्ख्या(dict, "width", wp->w_width);
    dict_add_सङ्ख्या(dict, "wincol", wp->w_wincol + 1);
    dict_add_सङ्ख्या(dict, "textoff", win_col_off(wp));
    dict_add_सङ्ख्या(dict, "bufnr", wp->w_buffer->b_fnum);
    dict_add_सङ्ख्या(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_add_सङ्ख्या(dict, "terminal", bt_terminal(wp->w_buffer));
# endif
# ifdef FEAT_QUICKFIX
    dict_add_सङ्ख्या(dict, "quickfix", bt_quickfix(wp->w_buffer));
    dict_add_सङ्ख्या(dict, "loclist",
		      (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != रिक्त));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    पटिनिवत्त dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    स्थिर dict_T *
get_tabpage_info(tabpage_T *tp, पूर्णाङ्क tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    सचे (dict == रिक्त)
	पटिनिवत्त रिक्त;

    dict_add_सङ्ख्या(dict, "tabnr", tp_idx);

    l = list_alloc();
    सचे (l != रिक्त)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_सङ्ख्या(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    पटिनिवत्त dict;
}

/*
 * "gettabinfo()" function
 */
    शून्य
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = रिक्त;
    dict_T	*d;
    पूर्णाङ्क		tpnr = 0;

    सचे (rettv_list_alloc(rettv) == FAIL)
	पटिनिवत्त;

    सचे (in_vim9script() && check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((पूर्णाङ्क)tv_get_सङ्ख्या_chk(&argvars[0], रिक्त));
	सचे (tparg == रिक्त)
	    पटिनिवत्त;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	सचे (tparg != रिक्त && tp != tparg)
	    अनुवर्त;
	d = get_tabpage_info(tp, tpnr);
	सचे (d != रिक्त)
	    list_append_dict(rettv->vval.v_list, d);
	सचे (tparg != रिक्त)
	    पटिनिवत्त;
    }
}

/*
 * "getwininfo()" function
 */
    शून्य
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = रिक्त, *wparg = रिक्त;
    dict_T	*d;
    short	tabnr = 0, winnr;

    सचे (rettv_list_alloc(rettv) == FAIL)
	पटिनिवत्त;

    सचे (in_vim9script() && check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_सङ्ख्या(&argvars[0]));
	सचे (wparg == रिक्त)
	    पटिनिवत्त;
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
	    सचे (wparg != रिक्त && wp != wparg)
		अनुवर्त;
	    d = get_win_info(wp, tabnr, winnr);
	    सचे (d != रिक्त)
		list_append_dict(rettv->vval.v_list, d);
	    सचे (wparg != रिक्त)
		// found information about a specific window
		पटिनिवत्त;
	}
    }
# ifdef FEAT_PROP_POPUP
    सचे (wparg != रिक्त)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		सचे (wp == wparg)
		    गच्छ found;
	}
found:
	d = get_win_info(wparg, tp == रिक्त ? 0 : tabnr, 0);
	सचे (d != रिक्त)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    शून्य
f_getwinpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    पूर्णाङ्क x = -1;
    पूर्णाङ्क y = -1;

    सचे (rettv_list_alloc(rettv) == FAIL)
	पटिनिवत्त;

    सचे (in_vim9script() && check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

# सचे defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	सचे (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_सङ्ख्या(&argvars[0]);

	(शून्य)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_सङ्ख्या(rettv->vval.v_list, (varnumber_T)x);
    list_append_सङ्ख्या(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    शून्य
f_getwinposx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = -1;
# सचे defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	पूर्णाङ्क	    x, y;

	सचे (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_सङ्ख्या = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    शून्य
f_getwinposy(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_सङ्ख्या = -1;
# सचे defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	पूर्णाङ्क	    x, y;

	सचे (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_सङ्ख्या = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    शून्य
f_tabpagenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    पूर्णाङ्क		nr = 1;
    char_u	*arg;

    सचे (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	सचे (arg != रिक्त)
	{
	    सचे (STRCMP(arg, "$") == 0)
		nr = tabpage_index(रिक्त) - 1;
	    अञ्ञथा सचे (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    अञ्ञथा
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    अञ्ञथा
	nr = tabpage_index(curtab);
    rettv->vval.v_सङ्ख्या = nr;
}

/*
 * "tabpagewinnr()" function
 */
    शून्य
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    पूर्णाङ्क		nr = 1;
    tabpage_T	*tp;

    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    tp = find_tabpage((पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[0]));
    सचे (tp == रिक्त)
	nr = 0;
    अञ्ञथा
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_सङ्ख्या = nr;
}

/*
 * "win_execute()" function
 */
    शून्य
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    पूर्णाङ्क		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	पटिनिवत्त;

    id = (पूर्णाङ्क)tv_get_सङ्ख्या(argvars);
    wp = win_id2wp_tp(id, &tp);
    सचे (wp == रिक्त || tp == रिक्त)
	पटिनिवत्त;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    पूर्णाङ्क	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    पूर्णाङ्क	apply_acd = FALSE;
    char_u	*save_sfname = रिक्त;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    सचे (curwin != wp
	    && (curwin->w_localdir != रिक्त
		|| wp->w_localdir != रिक्त
		|| (curtab != tp
		    && (curtab->tp_localdir != रिक्त
			|| tp->tp_localdir != रिक्त))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_status = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    सचे (cwd_status == OK && p_acd)
    {
	सचे (curbuf->b_sfname != रिक्त && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    सचे (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    सचे (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    अञ्ञथा
# endif
	सचे (cwd_status == OK)
	{
	    mch_chdir((वर्ण *)cwd);
# ifdef FEAT_AUTOCHDIR
	    सचे (save_sfname != रिक्त)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    सचे (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    सचे (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    शून्य
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    सचे (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    शून्य
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    सचे (in_vim9script()
	    && (check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_सङ्ख्या_arg(argvars, 1) == FAIL)))
	पटिनिवत्त;

    rettv->vval.v_सङ्ख्या = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    शून्य
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    पूर्णाङ्क		id;

    सचे (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    id = tv_get_सङ्ख्या(&argvars[0]);
    सचे (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_सङ्ख्या = 1;
	पटिनिवत्त;
    }

    सचे (text_or_buf_locked())
	पटिनिवत्त;
# सचे defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    सचे (popup_is_popup(curwin) && curbuf->b_term != रिक्त)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	पटिनिवत्त;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	सचे (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    सचे (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_सङ्ख्या = 1;
	    पटिनिवत्त;
	}
}

/*
 * "win_id2tabwin()" function
 */
    शून्य
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    सचे (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    शून्य
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    सचे (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    rettv->vval.v_सङ्ख्या = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    शून्य
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    पूर्णाङ्क		offset;

    rettv->vval.v_सङ्ख्या = FALSE;

    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    wp = find_win_by_nr_or_id(&argvars[0]);
    सचे (wp == रिक्त || win_valid_popup(wp))
	पटिनिवत्त;
    सचे (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	पटिनिवत्त;
    }

    offset = (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[1]);
    win_drag_vsep_पङ्क्तिः(wp, offset);
    rettv->vval.v_सङ्ख्या = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    शून्य
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    पूर्णाङ्क		offset;

    rettv->vval.v_सङ्ख्या = FALSE;

    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL))
	पटिनिवत्त;

    wp = find_win_by_nr_or_id(&argvars[0]);
    सचे (wp == रिक्त || win_valid_popup(wp))
	पटिनिवत्त;
    सचे (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	पटिनिवत्त;
    }

    offset = (पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[1]);
    win_drag_status_पङ्क्तिः(wp, offset);
    rettv->vval.v_सङ्ख्या = TRUE;
}

/*
 * "win_screenpos()" function
 */
    शून्य
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    सचे (rettv_list_alloc(rettv) == FAIL)
	पटिनिवत्त;

    सचे (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_सङ्ख्या(rettv->vval.v_list, wp == रिक्त ? 0 : wp->w_winrow + 1);
    list_append_सङ्ख्या(rettv->vval.v_list, wp == रिक्त ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    शून्य
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    पूर्णाङ्क     flags = 0, size = 0;

    rettv->vval.v_सङ्ख्या = -1;

    सचे (in_vim9script()
	    && (check_for_सङ्ख्या_arg(argvars, 0) == FAIL
		|| check_for_सङ्ख्या_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	पटिनिवत्त;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    सचे (wp == रिक्त || targetwin == रिक्त || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_सङ्ख्या));
	पटिनिवत्त;
    }

    सचे (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	सचे (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    पटिनिवत्त;

	d = argvars[2].vval.v_dict;
	सचे (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	सचे ((di = dict_find(d, (char_u *)"rightbelow", -1)) != रिक्त)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (पूर्णाङ्क)dict_get_सङ्ख्या(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    सचे (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	पटिनिवत्त;

    सचे (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    सचे (curwin == targetwin && win_valid(wp))
    {
	सचे (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_सङ्ख्या = 0;
    }
    अञ्ञथा
	emsg(_(e_autocommands_caused_command_to_abort));

    सचे (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    शून्य
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    सचे (in_vim9script() && check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	सचे (wp == रिक्त)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    पटिनिवत्त;
	}
    }
    सचे (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# सचे defined(FEAT_QUICKFIX)
    अञ्ञथा सचे (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    अञ्ञथा सचे (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    अञ्ञथा सचे (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    अञ्ञथा सचे (bt_quickfix(wp->w_buffer))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != रिक्त ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    शून्य
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;
    rettv->vval.v_string = alloc(2);
    सचे (rettv->vval.v_string == रिक्त)
	पटिनिवत्त;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    शून्य
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    सचे (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    wp = find_win_by_nr_or_id(&argvars[0]);
    सचे (wp == रिक्त)
	rettv->vval.v_सङ्ख्या = -1;
    अञ्ञथा
	rettv->vval.v_सङ्ख्या = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    शून्य
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    पूर्णाङ्क col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    सचे (curwin->w_p_rl)
	col = curwin->w_width - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_सङ्ख्या = col;
}

/*
 * "winheight(nr)" function
 */
    शून्य
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    सचे (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    wp = find_win_by_nr_or_id(&argvars[0]);
    सचे (wp == रिक्त)
	rettv->vval.v_सङ्ख्या = -1;
    अञ्ञथा
	rettv->vval.v_सङ्ख्या = wp->w_height;
}

/*
 * "winlayout()" function
 */
    शून्य
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    सचे (rettv_list_alloc(rettv) == FAIL)
	पटिनिवत्त;

    सचे (in_vim9script() && check_for_opt_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    सचे (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    अञ्ञथा
    {
	tp = find_tabpage((पूर्णाङ्क)tv_get_सङ्ख्या(&argvars[0]));
	सचे (tp == रिक्त)
	    पटिनिवत्त;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    शून्य
f_winline(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_सङ्ख्या = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    शून्य
f_winnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    पूर्णाङ्क		nr = 1;

    सचे (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_सङ्ख्या = nr;
}

/*
 * "winrestcmd()" function
 */
    शून्य
f_winrestcmd(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    पूर्णाङ्क		i;
    पूर्णाङ्क		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, परिमाण(वर्ण), 70);

    // Do this twice to handle some window layouts properly.
    अत्थाय (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((वर्ण *)buf, परिमाण(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((वर्ण *)buf, परिमाण(buf),
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
    शून्य
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    सचे (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    dict = argvars[0].vval.v_dict;
    सचे (dict_has_कुञ्चिका(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_सङ्ख्या(dict, "lnum");
    सचे (dict_has_कुञ्चिका(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_सङ्ख्या(dict, "col");
    सचे (dict_has_कुञ्चिका(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_सङ्ख्या(dict, "coladd");
    सचे (dict_has_कुञ्चिका(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_सङ्ख्या(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    सचे (dict_has_कुञ्चिका(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_सङ्ख्या(dict, "topline"));
# ifdef FEAT_DIFF
    सचे (dict_has_कुञ्चिका(dict, "topfill"))
	curwin->w_topfill = (पूर्णाङ्क)dict_get_सङ्ख्या(dict, "topfill");
# endif
    सचे (dict_has_कुञ्चिका(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_सङ्ख्या(dict, "leftcol");
    सचे (dict_has_कुञ्चिका(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_सङ्ख्या(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    सचे (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    सचे (curwin->w_topline > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	curwin->w_topline = curbuf->b_ml.ml_पङ्क्तिः_गणनम्;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    शून्य
f_winsaveview(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;

    सचे (rettv_dict_alloc(rettv) == FAIL)
	पटिनिवत्त;
    dict = rettv->vval.v_dict;

    dict_add_सङ्ख्या(dict, "lnum", (long)curwin->w_cursor.lnum);
    dict_add_सङ्ख्या(dict, "col", (long)curwin->w_cursor.col);
    dict_add_सङ्ख्या(dict, "coladd", (long)curwin->w_cursor.coladd);
    update_curswant();
    dict_add_सङ्ख्या(dict, "curswant", (long)curwin->w_curswant);

    dict_add_सङ्ख्या(dict, "topline", (long)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_add_सङ्ख्या(dict, "topfill", (long)curwin->w_topfill);
# endif
    dict_add_सङ्ख्या(dict, "leftcol", (long)curwin->w_leftcol);
    dict_add_सङ्ख्या(dict, "skipcol", (long)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    शून्य
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    सचे (in_vim9script() && check_for_सङ्ख्या_arg(argvars, 0) == FAIL)
	पटिनिवत्त;

    wp = find_win_by_nr_or_id(&argvars[0]);
    सचे (wp == रिक्त)
	rettv->vval.v_सङ्ख्या = -1;
    अञ्ञथा
	rettv->vval.v_सङ्ख्या = wp->w_width;
}
#endif // FEAT_EVAL

#सचे defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    पूर्णाङ्क
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	पूर्णाङ्क	    no_display)
{
    block_autocmds();
    पटिनिवत्त switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    पूर्णाङ्क
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	पूर्णाङ्क	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    सचे (win == curwin)
	switchwin->sw_same_win = TRUE;
    अञ्ञथा
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    सचे (tp != रिक्त)
    {
	switchwin->sw_curtab = curtab;
	सचे (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	अञ्ञथा
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    सचे (!win_valid(win))
	पटिनिवत्त FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    पटिनिवत्त OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    शून्य
restore_win(
	switchwin_T *switchwin,
	पूर्णाङ्क	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    शून्य
restore_win_noblock(
	switchwin_T *switchwin,
	पूर्णाङ्क	    no_display)
{
    सचे (switchwin->sw_curtab != रिक्त && valid_tabpage(switchwin->sw_curtab))
    {
	सचे (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	अञ्ञथा
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    सचे (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    सचे (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    अञ्ञथा सचे (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
