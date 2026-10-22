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

#include "vim.h"

#གལ་ཏེ defined(FEAT_EVAL)

    static ཧྲིལ
win_getid(typval_T *argvars)
{
    ཧྲིལ	    winnr;
    win_T   *wp;

    གལ་ཏེ (argvars[0].v_type == VAR_UNKNOWN)
	ཕྱིར curwin->w_id;
    winnr = tv_get_number(&argvars[0]);
    གལ་ཏེ (winnr <= 0)
	ཕྱིར 0;

    གལ་ཏེ (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    དེ་མིན
    {
	tabpage_T	*tp;
	ཧྲིལ		tabnr = tv_get_number(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    གལ་ཏེ (--tabnr == 0)
		bha_nga;
	གལ་ཏེ (tp == NULL)
	    ཕྱིར -1;
	གལ་ཏེ (tp == curtab)
	    wp = firstwin;
	དེ་མིན
	    wp = tp->tp_firstwin;
    }
    ཆེད་དུ ( ; wp != NULL; wp = wp->w_next)
	གལ་ཏེ (--winnr == 0)
	    ཕྱིར wp->w_id;
    ཕྱིར 0;
}

    static void
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    ཧྲིལ		winnr = 1;
    ཧྲིལ		tabnr = 1;
    ཧྲིལ		id = tv_get_number(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    གལ་ཏེ (wp->w_id == id)
	    {
		list_append_number(list, tabnr);
		list_append_number(list, winnr);
		ཕྱིར;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_number(list, 0);
    list_append_number(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(ཧྲིལ id)
{
    ཕྱིར win_id2wp_tp(id, NULL);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(ཧྲིལ id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	གལ་ཏེ (wp->w_id == id)
	{
	    གལ་ཏེ (tpp != NULL)
		*tpp = tp;
	    ཕྱིར wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     གལ་ཏེ (wp->w_id == id)
	     {
		 གལ་ཏེ (tpp != NULL)
		     *tpp = tp;
		 ཕྱིར wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	གལ་ཏེ (wp->w_id == id)
	{
	    གལ་ཏེ (tpp != NULL)
		*tpp = curtab;  // any tabpage would do
	    ཕྱིར wp;
	}
# endif

    ཕྱིར NULL;
}

    static ཧྲིལ
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    ཧྲིལ	    nr = 1;
    ཧྲིལ	    id = tv_get_number(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	གལ་ཏེ (wp->w_id == id)
	    ཕྱིར nr;
	++nr;
    }
    ཕྱིར 0;
}

    void
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    ཧྲིལ		bufnr = tv_get_number(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    གལ་ཏེ (wp->w_buffer->b_fnum == bufnr)
		list_append_number(list, wp->w_id);
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
    ཧྲིལ		nr = (ཧྲིལ)tv_get_number_chk(vp, NULL);

    གལ་ཏེ (nr < 0)
	ཕྱིར NULL;
    གལ་ཏེ (nr == 0)
	ཕྱིར curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	གལ་ཏེ (nr >= LOWEST_WIN_ID)
	{
	    གལ་ཏེ (wp->w_id == nr)
		ཕྱིར wp;
	}
	དེ་མིན གལ་ཏེ (--nr <= 0)
	    bha_nga;
    }
    གལ་ཏེ (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	ཆེད་དུ (wp = (tp == NULL ? curtab : tp)->tp_first_popupwin;
						   wp != NULL; wp = wp->w_next)
	    གལ་ཏེ (wp->w_id == nr)
		ཕྱིར wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    གལ་ཏེ (wp->w_id == nr)
		ཕྱིར wp;
# endif
	ཕྱིར NULL;
    }
    ཕྱིར wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    ཧྲིལ	nr = (ཧྲིལ)tv_get_number_chk(vp, NULL);

    གལ་ཏེ (nr >= LOWEST_WIN_ID)
	ཕྱིར win_id2wp(tv_get_number(vp));
    ཕྱིར find_win_by_nr(vp, NULL);
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
    win_T	*wp = NULL;
    tabpage_T	*tp = NULL;
    long	n;

    གལ་ཏེ (wvp->v_type != VAR_UNKNOWN)
    {
	གལ་ཏེ (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_number(tvp);
	    གལ་ཏེ (n >= 0)
		tp = find_tabpage(n);
	}
	དེ་མིན
	    tp = curtab;

	གལ་ཏེ (tp != NULL)
	{
	    wp = find_win_by_nr(wvp, tp);
	    གལ་ཏེ (wp == NULL && wvp->v_type == VAR_NUMBER
						&& wvp->vval.v_number != -1)
		// A window with the specified number is not found
		tp = NULL;
	}
    }
    དེ་མིན
    {
	wp = curwin;
	tp = curtab;
    }

    གལ་ཏེ (ptp != NULL)
	*ptp = tp;

    ཕྱིར wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    static void
get_framelayout(frame_T *fr, list_T *l, ཧྲིལ outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    གལ་ཏེ (fr == NULL)
	ཕྱིར;

    གལ་ཏེ (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    དེ་མིན
    {
	fr_list = list_alloc();
	གལ་ཏེ (fr_list == NULL)
	    ཕྱིར;
	གལ་ཏེ (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    ཕྱིར;
	}
    }

    གལ་ཏེ (fr->fr_layout == FR_LEAF)
    {
	གལ་ཏེ (fr->fr_win != NULL)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_number(fr_list, fr->fr_win->w_id);
	}
    }
    དེ་མིན
    {
	གལ་ཏེ (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	དེ་མིན
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	གལ་ཏེ (win_list == NULL)
	    ཕྱིར;
	གལ་ཏེ (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    ཕྱིར;
	}

	child = fr->fr_child;
	སྐབས (child != NULL)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    static ཧྲིལ
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    ཧྲིལ		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    གལ་ཏེ (argvar->v_type != VAR_UNKNOWN)
    {
	ཧྲིལ	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	གལ་ཏེ (arg == NULL)
	    nr = 0;		// type error; errmsg already given
	དེ་མིན གལ་ཏེ (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	དེ་མིན གལ་ཏེ (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	དེ་མིན
	{
	    long	count;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    count = strtol((var_na *)arg, (var_na **)&endp, 10);
	    གལ་ཏེ (count <= 0)
		count = 1;	// if count is not specified, default to 1
	    གལ་ཏེ (endp != NULL && *endp != '\0')
	    {
		གལ་ཏེ (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, count);
		དེ་མིན གལ་ཏེ (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, count);
		དེ་མིན གལ་ཏེ (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, count);
		དེ་མིན གལ་ཏེ (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, count);
		དེ་མིན
		    invalid_arg = TRUE;
	    }
	    དེ་མིན
		invalid_arg = TRUE;
	}
	གལ་ཏེ (twin == NULL)
	    nr = 0;

	གལ་ཏེ (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    གལ་ཏེ (nr <= 0)
	ཕྱིར 0;

    ཆེད་དུ (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	གལ་ཏེ (wp == NULL)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    bha_nga;
	}
	++nr;
    }
    ཕྱིར nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    static dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    གལ་ཏེ (dict == NULL)
	ཕྱིར NULL;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_add_number(dict, "tabnr", tpnr);
    dict_add_number(dict, "winnr", winnr);
    dict_add_number(dict, "winid", wp->w_id);
    dict_add_number(dict, "height", wp->w_height);
    dict_add_number(dict, "status_height", wp->w_status_height);
    dict_add_number(dict, "winrow", wp->w_winrow + 1);
    dict_add_number(dict, "topline", wp->w_topline);
    dict_add_number(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_add_number(dict, "winbar", wp->w_winbar_height);
# endif
    dict_add_number(dict, "width", wp->w_width);
    dict_add_number(dict, "wincol", wp->w_wincol + 1);
    dict_add_number(dict, "textoff", win_col_off(wp));
    dict_add_number(dict, "bufnr", wp->w_buffer->b_fnum);
    dict_add_number(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_add_number(dict, "terminal", bt_terminal(wp->w_buffer));
# endif
# ifdef FEAT_QUICKFIX
    dict_add_number(dict, "quickfix", bt_quickfix(wp->w_buffer));
    dict_add_number(dict, "loclist",
		      (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != NULL));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    ཕྱིར dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    static dict_T *
get_tabpage_info(tabpage_T *tp, ཧྲིལ tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    གལ་ཏེ (dict == NULL)
	ཕྱིར NULL;

    dict_add_number(dict, "tabnr", tp_idx);

    l = list_alloc();
    གལ་ཏེ (l != NULL)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_number(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    ཕྱིར dict;
}

/*
 * "gettabinfo()" function
 */
    void
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = NULL;
    dict_T	*d;
    ཧྲིལ		tpnr = 0;

    གལ་ཏེ (rettv_list_alloc(rettv) == FAIL)
	ཕྱིར;

    གལ་ཏེ (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    གལ་ཏེ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((ཧྲིལ)tv_get_number_chk(&argvars[0], NULL));
	གལ་ཏེ (tparg == NULL)
	    ཕྱིར;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	གལ་ཏེ (tparg != NULL && tp != tparg)
	    continue;
	d = get_tabpage_info(tp, tpnr);
	གལ་ཏེ (d != NULL)
	    list_append_dict(rettv->vval.v_list, d);
	གལ་ཏེ (tparg != NULL)
	    ཕྱིར;
    }
}

/*
 * "getwininfo()" function
 */
    void
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = NULL, *wparg = NULL;
    dict_T	*d;
    short	tabnr = 0, winnr;

    གལ་ཏེ (rettv_list_alloc(rettv) == FAIL)
	ཕྱིར;

    གལ་ཏེ (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    གལ་ཏེ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_number(&argvars[0]));
	གལ་ཏེ (wparg == NULL)
	    ཕྱིར;
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
	    གལ་ཏེ (wparg != NULL && wp != wparg)
		continue;
	    d = get_win_info(wp, tabnr, winnr);
	    གལ་ཏེ (d != NULL)
		list_append_dict(rettv->vval.v_list, d);
	    གལ་ཏེ (wparg != NULL)
		// found information about a specific window
		ཕྱིར;
	}
    }
# ifdef FEAT_PROP_POPUP
    གལ་ཏེ (wparg != NULL)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		གལ་ཏེ (wp == wparg)
		    goto found;
	}
found:
	d = get_win_info(wparg, tp == NULL ? 0 : tabnr, 0);
	གལ་ཏེ (d != NULL)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    void
f_getwinpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    ཧྲིལ x = -1;
    ཧྲིལ y = -1;

    གལ་ཏེ (rettv_list_alloc(rettv) == FAIL)
	ཕྱིར;

    གལ་ཏེ (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

# གལ་ཏེ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	གལ་ཏེ (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_number(&argvars[0]);

	(void)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_number(rettv->vval.v_list, (varnumber_T)x);
    list_append_number(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    void
f_getwinposx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = -1;
# གལ་ཏེ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	ཧྲིལ	    x, y;

	གལ་ཏེ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_number = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    void
f_getwinposy(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = -1;
# གལ་ཏེ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	ཧྲིལ	    x, y;

	གལ་ཏེ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_number = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    void
f_tabpagenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ཧྲིལ		nr = 1;
    char_u	*arg;

    གལ་ཏེ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ཕྱིར;

    གལ་ཏེ (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	གལ་ཏེ (arg != NULL)
	{
	    གལ་ཏེ (STRCMP(arg, "$") == 0)
		nr = tabpage_index(NULL) - 1;
	    དེ་མིན གལ་ཏེ (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    དེ་མིན
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    དེ་མིན
	nr = tabpage_index(curtab);
    rettv->vval.v_number = nr;
}

/*
 * "tabpagewinnr()" function
 */
    void
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ཧྲིལ		nr = 1;
    tabpage_T	*tp;

    གལ་ཏེ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ཕྱིར;

    tp = find_tabpage((ཧྲིལ)tv_get_number(&argvars[0]));
    གལ་ཏེ (tp == NULL)
	nr = 0;
    དེ་མིན
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_number = nr;
}

/*
 * "win_execute()" function
 */
    void
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    ཧྲིལ		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    གལ་ཏེ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ཕྱིར;

    id = (ཧྲིལ)tv_get_number(argvars);
    wp = win_id2wp_tp(id, &tp);
    གལ་ཏེ (wp == NULL || tp == NULL)
	ཕྱིར;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    ཧྲིལ	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    ཧྲིལ	apply_acd = FALSE;
    char_u	*save_sfname = NULL;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    གལ་ཏེ (curwin != wp
	    && (curwin->w_localdir != NULL
		|| wp->w_localdir != NULL
		|| (curtab != tp
		    && (curtab->tp_localdir != NULL
			|| tp->tp_localdir != NULL))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_status = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    གལ་ཏེ (cwd_status == OK && p_acd)
    {
	གལ་ཏེ (curbuf->b_sfname != NULL && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    གལ་ཏེ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    གལ་ཏེ (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    དེ་མིན
# endif
	གལ་ཏེ (cwd_status == OK)
	{
	    mch_chdir((var_na *)cwd);
# ifdef FEAT_AUTOCHDIR
	    གལ་ཏེ (save_sfname != NULL)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    གལ་ཏེ (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    གལ་ཏེ (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    void
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    གལ་ཏེ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    གལ་ཏེ (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    void
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    གལ་ཏེ (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	ཕྱིར;

    rettv->vval.v_number = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    void
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    ཧྲིལ		id;

    གལ་ཏེ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    id = tv_get_number(&argvars[0]);
    གལ་ཏེ (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_number = 1;
	ཕྱིར;
    }

    གལ་ཏེ (text_or_buf_locked())
	ཕྱིར;
# གལ་ཏེ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    གལ་ཏེ (popup_is_popup(curwin) && curbuf->b_term != NULL)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	ཕྱིར;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	གལ་ཏེ (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    གལ་ཏེ (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_number = 1;
	    ཕྱིར;
	}
}

/*
 * "win_id2tabwin()" function
 */
    void
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    གལ་ཏེ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    གལ་ཏེ (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    void
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    གལ་ཏེ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    rettv->vval.v_number = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    void
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ཧྲིལ		offset;

    rettv->vval.v_number = FALSE;

    གལ་ཏེ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	ཕྱིར;

    wp = find_win_by_nr_or_id(&argvars[0]);
    གལ་ཏེ (wp == NULL || win_valid_popup(wp))
	ཕྱིར;
    གལ་ཏེ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	ཕྱིར;
    }

    offset = (ཧྲིལ)tv_get_number(&argvars[1]);
    win_drag_vsep_line(wp, offset);
    rettv->vval.v_number = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    void
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ཧྲིལ		offset;

    rettv->vval.v_number = FALSE;

    གལ་ཏེ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	ཕྱིར;

    wp = find_win_by_nr_or_id(&argvars[0]);
    གལ་ཏེ (wp == NULL || win_valid_popup(wp))
	ཕྱིར;
    གལ་ཏེ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	ཕྱིར;
    }

    offset = (ཧྲིལ)tv_get_number(&argvars[1]);
    win_drag_status_line(wp, offset);
    rettv->vval.v_number = TRUE;
}

/*
 * "win_screenpos()" function
 */
    void
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    གལ་ཏེ (rettv_list_alloc(rettv) == FAIL)
	ཕྱིར;

    གལ་ཏེ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_number(rettv->vval.v_list, wp == NULL ? 0 : wp->w_winrow + 1);
    list_append_number(rettv->vval.v_list, wp == NULL ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    void
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    ཧྲིལ     flags = 0, size = 0;

    rettv->vval.v_number = -1;

    གལ་ཏེ (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ཕྱིར;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    གལ་ཏེ (wp == NULL || targetwin == NULL || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_number));
	ཕྱིར;
    }

    གལ་ཏེ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	གལ་ཏེ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ཕྱིར;

	d = argvars[2].vval.v_dict;
	གལ་ཏེ (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	གལ་ཏེ ((di = dict_find(d, (char_u *)"rightbelow", -1)) != NULL)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (ཧྲིལ)dict_get_number(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    གལ་ཏེ (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	ཕྱིར;

    གལ་ཏེ (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    གལ་ཏེ (curwin == targetwin && win_valid(wp))
    {
	གལ་ཏེ (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_number = 0;
    }
    དེ་མིན
	emsg(_(e_autocommands_caused_command_to_abort));

    གལ་ཏེ (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    void
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    གལ་ཏེ (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    གལ་ཏེ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	གལ་ཏེ (wp == NULL)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    ཕྱིར;
	}
    }
    གལ་ཏེ (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# གལ་ཏེ defined(FEAT_QUICKFIX)
    དེ་མིན གལ་ཏེ (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    དེ་མིན གལ་ཏེ (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    དེ་མིན གལ་ཏེ (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    དེ་མིན གལ་ཏེ (bt_quickfix(wp->w_buffer))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != NULL ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    void
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    rettv->vval.v_string = alloc(2);
    གལ་ཏེ (rettv->vval.v_string == NULL)
	ཕྱིར;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    void
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    གལ་ཏེ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    wp = find_win_by_nr_or_id(&argvars[0]);
    གལ་ཏེ (wp == NULL)
	rettv->vval.v_number = -1;
    དེ་མིན
	rettv->vval.v_number = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    void
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    ཧྲིལ col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    གལ་ཏེ (curwin->w_p_rl)
	col = curwin->w_width - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_number = col;
}

/*
 * "winheight(nr)" function
 */
    void
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    གལ་ཏེ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    wp = find_win_by_nr_or_id(&argvars[0]);
    གལ་ཏེ (wp == NULL)
	rettv->vval.v_number = -1;
    དེ་མིན
	rettv->vval.v_number = wp->w_height;
}

/*
 * "winlayout()" function
 */
    void
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    གལ་ཏེ (rettv_list_alloc(rettv) == FAIL)
	ཕྱིར;

    གལ་ཏེ (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    གལ་ཏེ (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    དེ་མིན
    {
	tp = find_tabpage((ཧྲིལ)tv_get_number(&argvars[0]));
	གལ་ཏེ (tp == NULL)
	    ཕྱིར;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    void
f_winline(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_number = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    void
f_winnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ཧྲིལ		nr = 1;

    གལ་ཏེ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ཕྱིར;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_number = nr;
}

/*
 * "winrestcmd()" function
 */
    void
f_winrestcmd(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    ཧྲིལ		i;
    ཧྲིལ		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, parimaa_na(var_na), 70);

    // Do this twice to handle some window layouts properly.
    ཆེད་དུ (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((var_na *)buf, parimaa_na(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((var_na *)buf, parimaa_na(buf),
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
    void
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    གལ་ཏེ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ཕྱིར;

    dict = argvars[0].vval.v_dict;
    གལ་ཏེ (dict_has_key(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_number(dict, "lnum");
    གལ་ཏེ (dict_has_key(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_number(dict, "col");
    གལ་ཏེ (dict_has_key(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_number(dict, "coladd");
    གལ་ཏེ (dict_has_key(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_number(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    གལ་ཏེ (dict_has_key(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_number(dict, "topline"));
# ifdef FEAT_DIFF
    གལ་ཏེ (dict_has_key(dict, "topfill"))
	curwin->w_topfill = (ཧྲིལ)dict_get_number(dict, "topfill");
# endif
    གལ་ཏེ (dict_has_key(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_number(dict, "leftcol");
    གལ་ཏེ (dict_has_key(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_number(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    གལ་ཏེ (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    གལ་ཏེ (curwin->w_topline > curbuf->b_ml.ml_line_count)
	curwin->w_topline = curbuf->b_ml.ml_line_count;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    void
f_winsaveview(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;

    གལ་ཏེ (rettv_dict_alloc(rettv) == FAIL)
	ཕྱིར;
    dict = rettv->vval.v_dict;

    dict_add_number(dict, "lnum", (long)curwin->w_cursor.lnum);
    dict_add_number(dict, "col", (long)curwin->w_cursor.col);
    dict_add_number(dict, "coladd", (long)curwin->w_cursor.coladd);
    update_curswant();
    dict_add_number(dict, "curswant", (long)curwin->w_curswant);

    dict_add_number(dict, "topline", (long)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_add_number(dict, "topfill", (long)curwin->w_topfill);
# endif
    dict_add_number(dict, "leftcol", (long)curwin->w_leftcol);
    dict_add_number(dict, "skipcol", (long)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    void
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    གལ་ཏེ (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	ཕྱིར;

    wp = find_win_by_nr_or_id(&argvars[0]);
    གལ་ཏེ (wp == NULL)
	rettv->vval.v_number = -1;
    དེ་མིན
	rettv->vval.v_number = wp->w_width;
}
#endif // FEAT_EVAL

#གལ་ཏེ defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    ཧྲིལ
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	ཧྲིལ	    no_display)
{
    block_autocmds();
    ཕྱིར switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    ཧྲིལ
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	ཧྲིལ	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    གལ་ཏེ (win == curwin)
	switchwin->sw_same_win = TRUE;
    དེ་མིན
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    གལ་ཏེ (tp != NULL)
    {
	switchwin->sw_curtab = curtab;
	གལ་ཏེ (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	དེ་མིན
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    གལ་ཏེ (!win_valid(win))
	ཕྱིར FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    ཕྱིར OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    void
restore_win(
	switchwin_T *switchwin,
	ཧྲིལ	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    void
restore_win_noblock(
	switchwin_T *switchwin,
	ཧྲིལ	    no_display)
{
    གལ་ཏེ (switchwin->sw_curtab != NULL && valid_tabpage(switchwin->sw_curtab))
    {
	གལ་ཏེ (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	དེ་མིན
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    གལ་ཏེ (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    གལ་ཏེ (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    དེ་མིན གལ་ཏེ (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
