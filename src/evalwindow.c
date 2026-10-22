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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

    સ્થિર પૂર્ણાંક
win_getid(typval_T *argvars)
{
    પૂર્ણાંક	    winnr;
    win_T   *wp;

    જો (argvars[0].v_type == VAR_UNKNOWN)
	પાછા curwin->w_id;
    winnr = tv_get_સંખ્યા(&argvars[0]);
    જો (winnr <= 0)
	પાછા 0;

    જો (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    નહીંતર
    {
	tabpage_T	*tp;
	પૂર્ણાંક		tabnr = tv_get_સંખ્યા(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    જો (--tabnr == 0)
		તોડો;
	જો (tp == ખાલી)
	    પાછા -1;
	જો (tp == curtab)
	    wp = firstwin;
	નહીંતર
	    wp = tp->tp_firstwin;
    }
    માટે ( ; wp != ખાલી; wp = wp->w_next)
	જો (--winnr == 0)
	    પાછા wp->w_id;
    પાછા 0;
}

    સ્થિર શૂન્ય
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    પૂર્ણાંક		winnr = 1;
    પૂર્ણાંક		tabnr = 1;
    પૂર્ણાંક		id = tv_get_સંખ્યા(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    જો (wp->w_id == id)
	    {
		list_append_સંખ્યા(list, tabnr);
		list_append_સંખ્યા(list, winnr);
		પાછા;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_સંખ્યા(list, 0);
    list_append_સંખ્યા(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(પૂર્ણાંક id)
{
    પાછા win_id2wp_tp(id, ખાલી);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(પૂર્ણાંક id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	જો (wp->w_id == id)
	{
	    જો (tpp != ખાલી)
		*tpp = tp;
	    પાછા wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     જો (wp->w_id == id)
	     {
		 જો (tpp != ખાલી)
		     *tpp = tp;
		 પાછા wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	જો (wp->w_id == id)
	{
	    જો (tpp != ખાલી)
		*tpp = curtab;  // any tabpage would do
	    પાછા wp;
	}
# endif

    પાછા ખાલી;
}

    સ્થિર પૂર્ણાંક
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    પૂર્ણાંક	    nr = 1;
    પૂર્ણાંક	    id = tv_get_સંખ્યા(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	જો (wp->w_id == id)
	    પાછા nr;
	++nr;
    }
    પાછા 0;
}

    શૂન્ય
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    પૂર્ણાંક		bufnr = tv_get_સંખ્યા(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    જો (wp->w_buffer->b_fnum == bufnr)
		list_append_સંખ્યા(list, wp->w_id);
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
    પૂર્ણાંક		nr = (પૂર્ણાંક)tv_get_સંખ્યા_chk(vp, ખાલી);

    જો (nr < 0)
	પાછા ખાલી;
    જો (nr == 0)
	પાછા curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	જો (nr >= LOWEST_WIN_ID)
	{
	    જો (wp->w_id == nr)
		પાછા wp;
	}
	નહીંતર જો (--nr <= 0)
	    તોડો;
    }
    જો (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	માટે (wp = (tp == ખાલી ? curtab : tp)->tp_first_popupwin;
						   wp != ખાલી; wp = wp->w_next)
	    જો (wp->w_id == nr)
		પાછા wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    જો (wp->w_id == nr)
		પાછા wp;
# endif
	પાછા ખાલી;
    }
    પાછા wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    પૂર્ણાંક	nr = (પૂર્ણાંક)tv_get_સંખ્યા_chk(vp, ખાલી);

    જો (nr >= LOWEST_WIN_ID)
	પાછા win_id2wp(tv_get_સંખ્યા(vp));
    પાછા find_win_by_nr(vp, ખાલી);
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
    win_T	*wp = ખાલી;
    tabpage_T	*tp = ખાલી;
    long	n;

    જો (wvp->v_type != VAR_UNKNOWN)
    {
	જો (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_સંખ્યા(tvp);
	    જો (n >= 0)
		tp = find_tabpage(n);
	}
	નહીંતર
	    tp = curtab;

	જો (tp != ખાલી)
	{
	    wp = find_win_by_nr(wvp, tp);
	    જો (wp == ખાલી && wvp->v_type == VAR_સંખ્યા
						&& wvp->vval.v_સંખ્યા != -1)
		// A window with the specified number is not found
		tp = ખાલી;
	}
    }
    નહીંતર
    {
	wp = curwin;
	tp = curtab;
    }

    જો (ptp != ખાલી)
	*ptp = tp;

    પાછા wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    સ્થિર શૂન્ય
get_framelayout(frame_T *fr, list_T *l, પૂર્ણાંક outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    જો (fr == ખાલી)
	પાછા;

    જો (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    નહીંતર
    {
	fr_list = list_alloc();
	જો (fr_list == ખાલી)
	    પાછા;
	જો (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    પાછા;
	}
    }

    જો (fr->fr_layout == FR_LEAF)
    {
	જો (fr->fr_win != ખાલી)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_સંખ્યા(fr_list, fr->fr_win->w_id);
	}
    }
    નહીંતર
    {
	જો (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	નહીંતર
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	જો (win_list == ખાલી)
	    પાછા;
	જો (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    પાછા;
	}

	child = fr->fr_child;
	જ્યારે (child != ખાલી)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    સ્થિર પૂર્ણાંક
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    પૂર્ણાંક		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    જો (argvar->v_type != VAR_UNKNOWN)
    {
	પૂર્ણાંક	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	જો (arg == ખાલી)
	    nr = 0;		// type error; errmsg already given
	નહીંતર જો (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	નહીંતર જો (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	નહીંતર
	{
	    long	ગણતરી;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    ગણતરી = strtol((અક્ષર *)arg, (અક્ષર **)&endp, 10);
	    જો (ગણતરી <= 0)
		ગણતરી = 1;	// if count is not specified, default to 1
	    જો (endp != ખાલી && *endp != '\0')
	    {
		જો (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, ગણતરી);
		નહીંતર જો (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, ગણતરી);
		નહીંતર જો (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, ગણતરી);
		નહીંતર જો (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, ગણતરી);
		નહીંતર
		    invalid_arg = TRUE;
	    }
	    નહીંતર
		invalid_arg = TRUE;
	}
	જો (twin == ખાલી)
	    nr = 0;

	જો (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    જો (nr <= 0)
	પાછા 0;

    માટે (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	જો (wp == ખાલી)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    તોડો;
	}
	++nr;
    }
    પાછા nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    સ્થિર dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    જો (dict == ખાલી)
	પાછા ખાલી;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_add_સંખ્યા(dict, "tabnr", tpnr);
    dict_add_સંખ્યા(dict, "winnr", winnr);
    dict_add_સંખ્યા(dict, "winid", wp->w_id);
    dict_add_સંખ્યા(dict, "height", wp->w_height);
    dict_add_સંખ્યા(dict, "status_height", wp->w_status_height);
    dict_add_સંખ્યા(dict, "winrow", wp->w_winrow + 1);
    dict_add_સંખ્યા(dict, "topline", wp->w_topline);
    dict_add_સંખ્યા(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_add_સંખ્યા(dict, "winbar", wp->w_winbar_height);
# endif
    dict_add_સંખ્યા(dict, "width", wp->w_width);
    dict_add_સંખ્યા(dict, "wincol", wp->w_wincol + 1);
    dict_add_સંખ્યા(dict, "textoff", win_col_off(wp));
    dict_add_સંખ્યા(dict, "bufnr", wp->w_buffer->b_fnum);
    dict_add_સંખ્યા(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_add_સંખ્યા(dict, "terminal", bt_terminal(wp->w_buffer));
# endif
# ifdef FEAT_QUICKFIX
    dict_add_સંખ્યા(dict, "quickfix", bt_quickfix(wp->w_buffer));
    dict_add_સંખ્યા(dict, "loclist",
		      (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != ખાલી));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    પાછા dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    સ્થિર dict_T *
get_tabpage_info(tabpage_T *tp, પૂર્ણાંક tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    જો (dict == ખાલી)
	પાછા ખાલી;

    dict_add_સંખ્યા(dict, "tabnr", tp_idx);

    l = list_alloc();
    જો (l != ખાલી)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_સંખ્યા(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    પાછા dict;
}

/*
 * "gettabinfo()" function
 */
    શૂન્ય
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = ખાલી;
    dict_T	*d;
    પૂર્ણાંક		tpnr = 0;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((પૂર્ણાંક)tv_get_સંખ્યા_chk(&argvars[0], ખાલી));
	જો (tparg == ખાલી)
	    પાછા;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	જો (tparg != ખાલી && tp != tparg)
	    ચાલુ;
	d = get_tabpage_info(tp, tpnr);
	જો (d != ખાલી)
	    list_append_dict(rettv->vval.v_list, d);
	જો (tparg != ખાલી)
	    પાછા;
    }
}

/*
 * "getwininfo()" function
 */
    શૂન્ય
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = ખાલી, *wparg = ખાલી;
    dict_T	*d;
    short	tabnr = 0, winnr;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_સંખ્યા(&argvars[0]));
	જો (wparg == ખાલી)
	    પાછા;
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
	    જો (wparg != ખાલી && wp != wparg)
		ચાલુ;
	    d = get_win_info(wp, tabnr, winnr);
	    જો (d != ખાલી)
		list_append_dict(rettv->vval.v_list, d);
	    જો (wparg != ખાલી)
		// found information about a specific window
		પાછા;
	}
    }
# ifdef FEAT_PROP_POPUP
    જો (wparg != ખાલી)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		જો (wp == wparg)
		    જાઓ found;
	}
found:
	d = get_win_info(wparg, tp == ખાલી ? 0 : tabnr, 0);
	જો (d != ખાલી)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    શૂન્ય
f_getwinpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    પૂર્ણાંક x = -1;
    પૂર્ણાંક y = -1;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

# જો defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	જો (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_સંખ્યા(&argvars[0]);

	(શૂન્ય)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)x);
    list_append_સંખ્યા(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    શૂન્ય
f_getwinposx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = -1;
# જો defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	પૂર્ણાંક	    x, y;

	જો (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_સંખ્યા = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    શૂન્ય
f_getwinposy(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_સંખ્યા = -1;
# જો defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	પૂર્ણાંક	    x, y;

	જો (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_સંખ્યા = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    શૂન્ય
f_tabpagenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    પૂર્ણાંક		nr = 1;
    char_u	*arg;

    જો (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	જો (arg != ખાલી)
	{
	    જો (STRCMP(arg, "$") == 0)
		nr = tabpage_index(ખાલી) - 1;
	    નહીંતર જો (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    નહીંતર
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    નહીંતર
	nr = tabpage_index(curtab);
    rettv->vval.v_સંખ્યા = nr;
}

/*
 * "tabpagewinnr()" function
 */
    શૂન્ય
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    પૂર્ણાંક		nr = 1;
    tabpage_T	*tp;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	પાછા;

    tp = find_tabpage((પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]));
    જો (tp == ખાલી)
	nr = 0;
    નહીંતર
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_સંખ્યા = nr;
}

/*
 * "win_execute()" function
 */
    શૂન્ય
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    પૂર્ણાંક		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	પાછા;

    id = (પૂર્ણાંક)tv_get_સંખ્યા(argvars);
    wp = win_id2wp_tp(id, &tp);
    જો (wp == ખાલી || tp == ખાલી)
	પાછા;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    પૂર્ણાંક	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    પૂર્ણાંક	apply_acd = FALSE;
    char_u	*save_sfname = ખાલી;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    જો (curwin != wp
	    && (curwin->w_localdir != ખાલી
		|| wp->w_localdir != ખાલી
		|| (curtab != tp
		    && (curtab->tp_localdir != ખાલી
			|| tp->tp_localdir != ખાલી))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_status = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    જો (cwd_status == OK && p_acd)
    {
	જો (curbuf->b_sfname != ખાલી && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    જો (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    જો (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    નહીંતર
# endif
	જો (cwd_status == OK)
	{
	    mch_chdir((અક્ષર *)cwd);
# ifdef FEAT_AUTOCHDIR
	    જો (save_sfname != ખાલી)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    જો (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    જો (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    શૂન્ય
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    શૂન્ય
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script()
	    && (check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_સંખ્યા_arg(argvars, 1) == FAIL)))
	પાછા;

    rettv->vval.v_સંખ્યા = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    શૂન્ય
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    પૂર્ણાંક		id;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    id = tv_get_સંખ્યા(&argvars[0]);
    જો (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_સંખ્યા = 1;
	પાછા;
    }

    જો (text_or_buf_locked())
	પાછા;
# જો defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    જો (popup_is_popup(curwin) && curbuf->b_term != ખાલી)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	પાછા;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	જો (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    જો (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_સંખ્યા = 1;
	    પાછા;
	}
}

/*
 * "win_id2tabwin()" function
 */
    શૂન્ય
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    શૂન્ય
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    rettv->vval.v_સંખ્યા = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    શૂન્ય
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    પૂર્ણાંક		offset;

    rettv->vval.v_સંખ્યા = FALSE;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    wp = find_win_by_nr_or_id(&argvars[0]);
    જો (wp == ખાલી || win_valid_popup(wp))
	પાછા;
    જો (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	પાછા;
    }

    offset = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[1]);
    win_drag_vsep_લીટી(wp, offset);
    rettv->vval.v_સંખ્યા = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    શૂન્ય
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    પૂર્ણાંક		offset;

    rettv->vval.v_સંખ્યા = FALSE;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL))
	પાછા;

    wp = find_win_by_nr_or_id(&argvars[0]);
    જો (wp == ખાલી || win_valid_popup(wp))
	પાછા;
    જો (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	પાછા;
    }

    offset = (પૂર્ણાંક)tv_get_સંખ્યા(&argvars[1]);
    win_drag_status_લીટી(wp, offset);
    rettv->vval.v_સંખ્યા = TRUE;
}

/*
 * "win_screenpos()" function
 */
    શૂન્ય
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_સંખ્યા(rettv->vval.v_list, wp == ખાલી ? 0 : wp->w_winrow + 1);
    list_append_સંખ્યા(rettv->vval.v_list, wp == ખાલી ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    શૂન્ય
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    પૂર્ણાંક     flags = 0, size = 0;

    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_સંખ્યા_arg(argvars, 0) == FAIL
		|| check_for_સંખ્યા_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	પાછા;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    જો (wp == ખાલી || targetwin == ખાલી || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_સંખ્યા));
	પાછા;
    }

    જો (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	જો (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    પાછા;

	d = argvars[2].vval.v_dict;
	જો (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	જો ((di = dict_find(d, (char_u *)"rightbelow", -1)) != ખાલી)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (પૂર્ણાંક)dict_get_સંખ્યા(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    જો (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	પાછા;

    જો (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    જો (curwin == targetwin && win_valid(wp))
    {
	જો (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_સંખ્યા = 0;
    }
    નહીંતર
	emsg(_(e_autocommands_caused_command_to_abort));

    જો (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    શૂન્ય
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	જો (wp == ખાલી)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    પાછા;
	}
    }
    જો (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# જો defined(FEAT_QUICKFIX)
    નહીંતર જો (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    નહીંતર જો (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    નહીંતર જો (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    નહીંતર જો (bt_quickfix(wp->w_buffer))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != ખાલી ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    શૂન્ય
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;
    rettv->vval.v_string = alloc(2);
    જો (rettv->vval.v_string == ખાલી)
	પાછા;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    શૂન્ય
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    wp = find_win_by_nr_or_id(&argvars[0]);
    જો (wp == ખાલી)
	rettv->vval.v_સંખ્યા = -1;
    નહીંતર
	rettv->vval.v_સંખ્યા = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    શૂન્ય
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    પૂર્ણાંક col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    જો (curwin->w_p_rl)
	col = curwin->w_width - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_સંખ્યા = col;
}

/*
 * "winheight(nr)" function
 */
    શૂન્ય
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    wp = find_win_by_nr_or_id(&argvars[0]);
    જો (wp == ખાલી)
	rettv->vval.v_સંખ્યા = -1;
    નહીંતર
	rettv->vval.v_સંખ્યા = wp->w_height;
}

/*
 * "winlayout()" function
 */
    શૂન્ય
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (in_vim9script() && check_for_opt_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    જો (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    નહીંતર
    {
	tp = find_tabpage((પૂર્ણાંક)tv_get_સંખ્યા(&argvars[0]));
	જો (tp == ખાલી)
	    પાછા;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    શૂન્ય
f_winline(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_સંખ્યા = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    શૂન્ય
f_winnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    પૂર્ણાંક		nr = 1;

    જો (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	પાછા;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_સંખ્યા = nr;
}

/*
 * "winrestcmd()" function
 */
    શૂન્ય
f_winrestcmd(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    પૂર્ણાંક		i;
    પૂર્ણાંક		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, કદ(અક્ષર), 70);

    // Do this twice to handle some window layouts properly.
    માટે (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((અક્ષર *)buf, કદ(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((અક્ષર *)buf, કદ(buf),
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
    શૂન્ય
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    જો (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	પાછા;

    dict = argvars[0].vval.v_dict;
    જો (dict_has_ચાવી(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_સંખ્યા(dict, "lnum");
    જો (dict_has_ચાવી(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_સંખ્યા(dict, "col");
    જો (dict_has_ચાવી(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_સંખ્યા(dict, "coladd");
    જો (dict_has_ચાવી(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_સંખ્યા(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    જો (dict_has_ચાવી(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_સંખ્યા(dict, "topline"));
# ifdef FEAT_DIFF
    જો (dict_has_ચાવી(dict, "topfill"))
	curwin->w_topfill = (પૂર્ણાંક)dict_get_સંખ્યા(dict, "topfill");
# endif
    જો (dict_has_ચાવી(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_સંખ્યા(dict, "leftcol");
    જો (dict_has_ચાવી(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_સંખ્યા(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    જો (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    જો (curwin->w_topline > curbuf->b_ml.ml_લીટી_ગણતરી)
	curwin->w_topline = curbuf->b_ml.ml_લીટી_ગણતરી;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    શૂન્ય
f_winsaveview(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;

    જો (rettv_dict_alloc(rettv) == FAIL)
	પાછા;
    dict = rettv->vval.v_dict;

    dict_add_સંખ્યા(dict, "lnum", (long)curwin->w_cursor.lnum);
    dict_add_સંખ્યા(dict, "col", (long)curwin->w_cursor.col);
    dict_add_સંખ્યા(dict, "coladd", (long)curwin->w_cursor.coladd);
    update_curswant();
    dict_add_સંખ્યા(dict, "curswant", (long)curwin->w_curswant);

    dict_add_સંખ્યા(dict, "topline", (long)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_add_સંખ્યા(dict, "topfill", (long)curwin->w_topfill);
# endif
    dict_add_સંખ્યા(dict, "leftcol", (long)curwin->w_leftcol);
    dict_add_સંખ્યા(dict, "skipcol", (long)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    શૂન્ય
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    જો (in_vim9script() && check_for_સંખ્યા_arg(argvars, 0) == FAIL)
	પાછા;

    wp = find_win_by_nr_or_id(&argvars[0]);
    જો (wp == ખાલી)
	rettv->vval.v_સંખ્યા = -1;
    નહીંતર
	rettv->vval.v_સંખ્યા = wp->w_width;
}
#endif // FEAT_EVAL

#જો defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    પૂર્ણાંક
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	પૂર્ણાંક	    no_display)
{
    block_autocmds();
    પાછા switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    પૂર્ણાંક
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	પૂર્ણાંક	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    જો (win == curwin)
	switchwin->sw_same_win = TRUE;
    નહીંતર
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    જો (tp != ખાલી)
    {
	switchwin->sw_curtab = curtab;
	જો (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	નહીંતર
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    જો (!win_valid(win))
	પાછા FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    પાછા OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    શૂન્ય
restore_win(
	switchwin_T *switchwin,
	પૂર્ણાંક	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    શૂન્ય
restore_win_noblock(
	switchwin_T *switchwin,
	પૂર્ણાંક	    no_display)
{
    જો (switchwin->sw_curtab != ખાલી && valid_tabpage(switchwin->sw_curtab))
    {
	જો (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	નહીંતર
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    જો (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    જો (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    નહીંતર જો (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
