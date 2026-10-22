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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

    ਸਥਿਰ ਪੂਰਨ
win_getid(typval_T *argvars)
{
    ਪੂਰਨ	    winnr;
    win_T   *wp;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
	ਵਾਪਸ curwin->w_id;
    winnr = tv_get_ਸੰਖਿਆ(&argvars[0]);
    ਜੇ (winnr <= 0)
	ਵਾਪਸ 0;

    ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    ਨਹੀਂਤਾਂ
    {
	tabpage_T	*tp;
	ਪੂਰਨ		tabnr = tv_get_ਸੰਖਿਆ(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    ਜੇ (--tabnr == 0)
		ਰੋਕੋ;
	ਜੇ (tp == ਨਿਰਾ)
	    ਵਾਪਸ -1;
	ਜੇ (tp == curtab)
	    wp = firstwin;
	ਨਹੀਂਤਾਂ
	    wp = tp->tp_firstwin;
    }
    ਲਈ ( ; wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	ਜੇ (--winnr == 0)
	    ਵਾਪਸ wp->w_id;
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਖਾਲੀ
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    ਪੂਰਨ		winnr = 1;
    ਪੂਰਨ		tabnr = 1;
    ਪੂਰਨ		id = tv_get_ਸੰਖਿਆ(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
	{
	    ਜੇ (wp->w_id == id)
	    {
		list_append_ਸੰਖਿਆ(list, tabnr);
		list_append_ਸੰਖਿਆ(list, winnr);
		ਵਾਪਸ;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_ਸੰਖਿਆ(list, 0);
    list_append_ਸੰਖਿਆ(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(ਪੂਰਨ id)
{
    ਵਾਪਸ win_id2wp_tp(id, ਨਿਰਾ);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(ਪੂਰਨ id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ਜੇ (wp->w_id == id)
	{
	    ਜੇ (tpp != ਨਿਰਾ)
		*tpp = tp;
	    ਵਾਪਸ wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, wp)
	     ਜੇ (wp->w_id == id)
	     {
		 ਜੇ (tpp != ਨਿਰਾ)
		     *tpp = tp;
		 ਵਾਪਸ wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	ਜੇ (wp->w_id == id)
	{
	    ਜੇ (tpp != ਨਿਰਾ)
		*tpp = curtab;  // any tabpage would do
	    ਵਾਪਸ wp;
	}
# endif

    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    ਪੂਰਨ	    nr = 1;
    ਪੂਰਨ	    id = tv_get_ਸੰਖਿਆ(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (wp->w_id == id)
	    ਵਾਪਸ nr;
	++nr;
    }
    ਵਾਪਸ 0;
}

    ਖਾਲੀ
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    ਪੂਰਨ		bufnr = tv_get_ਸੰਖਿਆ(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    ਜੇ (wp->w_ਬਫਰ->b_fnum == bufnr)
		list_append_ਸੰਖਿਆ(list, wp->w_id);
}

/*
 * Find window specified by "vp" in tabpage "tp".
 * Returns current window if "vp" is number zero.
 * Returns NULL if not found.
 */
    win_T *
ਲੱਭੋ_win_by_nr(
    typval_T	*vp,
    tabpage_T	*tp)	// NULL for current tab page
{
    win_T	*wp;
    ਪੂਰਨ		nr = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(vp, ਨਿਰਾ);

    ਜੇ (nr < 0)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (nr == 0)
	ਵਾਪਸ curwin;

    FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
    {
	ਜੇ (nr >= ਸਭ_ਤੋਂ_ਨੀਵਾਂ_WIN_ID)
	{
	    ਜੇ (wp->w_id == nr)
		ਵਾਪਸ wp;
	}
	ਨਹੀਂਤਾਂ ਜੇ (--nr <= 0)
	    ਰੋਕੋ;
    }
    ਜੇ (nr >= ਸਭ_ਤੋਂ_ਨੀਵਾਂ_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	ਲਈ (wp = (tp == ਨਿਰਾ ? curtab : tp)->tp_ਪਹਿਲਾ_popupwin;
						   wp != ਨਿਰਾ; wp = wp->w_ਅਗਲਾ)
	    ਜੇ (wp->w_id == nr)
		ਵਾਪਸ wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    ਜੇ (wp->w_id == nr)
		ਵਾਪਸ wp;
# endif
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
ਲੱਭੋ_win_by_nr_or_id(typval_T *vp)
{
    ਪੂਰਨ	nr = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(vp, ਨਿਰਾ);

    ਜੇ (nr >= ਸਭ_ਤੋਂ_ਨੀਵਾਂ_WIN_ID)
	ਵਾਪਸ win_id2wp(tv_get_ਸੰਖਿਆ(vp));
    ਵਾਪਸ ਲੱਭੋ_win_by_nr(vp, ਨਿਰਾ);
}

/*
 * Find window specified by "wvp" in tabpage "tvp".
 * Returns the tab page in 'ptp'
 */
    win_T *
ਲੱਭੋ_tabwin(
    typval_T	*wvp,	// VAR_UNKNOWN for current window
    typval_T	*tvp,	// VAR_UNKNOWN for current tab page
    tabpage_T	**ptp)
{
    win_T	*wp = ਨਿਰਾ;
    tabpage_T	*tp = ਨਿਰਾ;
    ਲੰਮਾ	n;

    ਜੇ (wvp->v_type != VAR_UNKNOWN)
    {
	ਜੇ (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ(tvp);
	    ਜੇ (n >= 0)
		tp = ਲੱਭੋ_tabpage(n);
	}
	ਨਹੀਂਤਾਂ
	    tp = curtab;

	ਜੇ (tp != ਨਿਰਾ)
	{
	    wp = ਲੱਭੋ_win_by_nr(wvp, tp);
	    ਜੇ (wp == ਨਿਰਾ && wvp->v_type == VAR_ਸੰਖਿਆ
						&& wvp->vval.v_ਸੰਖਿਆ != -1)
		// A window with the specified number is not found
		tp = ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	wp = curwin;
	tp = curtab;
    }

    ਜੇ (ptp != ਨਿਰਾ)
	*ptp = tp;

    ਵਾਪਸ wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    ਸਥਿਰ ਖਾਲੀ
get_framelayout(ਚੌਖਟਾ_T *fr, list_T *l, ਪੂਰਨ outer)
{
    ਚੌਖਟਾ_T	*ਬੱਚਾ;
    list_T	*fr_list;
    list_T	*win_list;

    ਜੇ (fr == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    ਨਹੀਂਤਾਂ
    {
	fr_list = list_alloc();
	ਜੇ (fr_list == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਜੇ (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    ਵਾਪਸ;
	}
    }

    ਜੇ (fr->fr_layout == FR_ਪੱਤੀ)
    {
	ਜੇ (fr->fr_win != ਨਿਰਾ)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_ਸੰਖਿਆ(fr_list, fr->fr_win->w_id);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (fr->fr_layout == FR_ਪੰਕਤੀ)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	ਨਹੀਂਤਾਂ
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	ਜੇ (win_list == ਨਿਰਾ)
	    ਵਾਪਸ;
	ਜੇ (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    ਵਾਪਸ;
	}

	ਬੱਚਾ = fr->fr_ਬੱਚਾ;
	ਜਦੋਂ (ਬੱਚਾ != ਨਿਰਾ)
	{
	    get_framelayout(ਬੱਚਾ, win_list, FALSE);
	    ਬੱਚਾ = ਬੱਚਾ->fr_ਅਗਲਾ;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    ਸਥਿਰ ਪੂਰਨ
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    ਪੂਰਨ		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    ਜੇ (argvar->v_type != VAR_UNKNOWN)
    {
	ਪੂਰਨ	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	ਜੇ (arg == ਨਿਰਾ)
	    nr = 0;		// type error; errmsg already given
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਲੰਮਾ	ਗਿਣਤੀ_2;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    ਗਿਣਤੀ_2 = strtol((ਅੱਖਰ *)arg, (ਅੱਖਰ **)&endp, 10);
	    ਜੇ (ਗਿਣਤੀ_2 <= 0)
		ਗਿਣਤੀ_2 = 1;	// if count is not specified, default to 1
	    ਜੇ (endp != ਨਿਰਾ && *endp != '\0')
	    {
		ਜੇ (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, ਗਿਣਤੀ_2);
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, ਗਿਣਤੀ_2);
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, ਗਿਣਤੀ_2);
		ਨਹੀਂਤਾਂ ਜੇ (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, ਗਿਣਤੀ_2);
		ਨਹੀਂਤਾਂ
		    invalid_arg = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
		invalid_arg = TRUE;
	}
	ਜੇ (twin == ਨਿਰਾ)
	    nr = 0;

	ਜੇ (invalid_arg)
	{
	    semsg(_(e_invalid_ਵਿਅੰਜਕ_str), arg);
	    nr = 0;
	}
    }

    ਜੇ (nr <= 0)
	ਵਾਪਸ 0;

    ਲਈ (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_ਅਗਲਾ)
    {
	ਜੇ (wp == ਨਿਰਾ)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    ਰੋਕੋ;
	}
	++nr;
    }
    ਵਾਪਸ nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    ਸਥਿਰ dict_T *
get_win_info(win_T *wp, ਛੋਟਾ tpnr, ਛੋਟਾ winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "tabnr", tpnr);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "winnr", winnr);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "winid", wp->w_id);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "height", wp->w_ਉਚਾਈ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "status_height", wp->w_ਸਥਿਤੀ_ਉਚਾਈ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "winrow", wp->w_winrow + 1);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "topline", wp->w_topline);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "winbar", wp->w_winbar_ਉਚਾਈ);
# endif
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "width", wp->w_ਚੌੜਾਈ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "wincol", wp->w_wincol + 1);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "textoff", win_ਖਾਨਾ_off(wp));
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "bufnr", wp->w_ਬਫਰ->b_fnum);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "terminal", bt_terminal(wp->w_ਬਫਰ));
# endif
# ifdef FEAT_QUICKFIX
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "quickfix", bt_quickfix(wp->w_ਬਫਰ));
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "loclist",
		      (bt_quickfix(wp->w_ਬਫਰ) && wp->w_llist_ref != ਨਿਰਾ));
# endif

    // Add a reference to window variables
    dict_ਜੋੜੋ_dict(dict, "variables", wp->w_vars);

    ਵਾਪਸ dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    ਸਥਿਰ dict_T *
get_tabpage_info(tabpage_T *tp, ਪੂਰਨ tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "tabnr", tp_idx);

    l = list_alloc();
    ਜੇ (l != ਨਿਰਾ)
    {
	FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
	    list_append_ਸੰਖਿਆ(l, (varnumber_T)wp->w_id);
	dict_ਜੋੜੋ_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_ਜੋੜੋ_dict(dict, "variables", tp->tp_vars);

    ਵਾਪਸ dict;
}

/*
 * "gettabinfo()" function
 */
    ਖਾਲੀ
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = ਨਿਰਾ;
    dict_T	*d;
    ਪੂਰਨ		tpnr = 0;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = ਲੱਭੋ_tabpage((ਪੂਰਨ)tv_get_ਸੰਖਿਆ_chk(&argvars[0], ਨਿਰਾ));
	ਜੇ (tparg == ਨਿਰਾ)
	    ਵਾਪਸ;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	ਜੇ (tparg != ਨਿਰਾ && tp != tparg)
	    ਜਾਰੀ;
	d = get_tabpage_info(tp, tpnr);
	ਜੇ (d != ਨਿਰਾ)
	    list_append_dict(rettv->vval.v_list, d);
	ਜੇ (tparg != ਨਿਰਾ)
	    ਵਾਪਸ;
    }
}

/*
 * "getwininfo()" function
 */
    ਖਾਲੀ
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = ਨਿਰਾ, *wparg = ਨਿਰਾ;
    dict_T	*d;
    ਛੋਟਾ	tabnr = 0, winnr;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_ਸੰਖਿਆ(&argvars[0]));
	ਜੇ (wparg == ਨਿਰਾ)
	    ਵਾਪਸ;
    }

    // Collect information about either all the windows across all the tab
    // pages or one particular window.
    FOR_ALL_TABPAGES(tp)
    {
	tabnr++;
	winnr = 0;
	FOR_ALL_WINDOWS_ਵਿੱਚ_TAB(tp, wp)
	{
	    winnr++;
	    ਜੇ (wparg != ਨਿਰਾ && wp != wparg)
		ਜਾਰੀ;
	    d = get_win_info(wp, tabnr, winnr);
	    ਜੇ (d != ਨਿਰਾ)
		list_append_dict(rettv->vval.v_list, d);
	    ਜੇ (wparg != ਨਿਰਾ)
		// found information about a specific window
		ਵਾਪਸ;
	}
    }
# ifdef FEAT_PROP_POPUP
    ਜੇ (wparg != ਨਿਰਾ)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_ਵਿੱਚ_TAB(tp, wp)
		ਜੇ (wp == wparg)
		    ਜਾਓ ਮਿਲਿਆ;
	}
ਮਿਲਿਆ:
	d = get_win_info(wparg, tp == ਨਿਰਾ ? 0 : tabnr, 0);
	ਜੇ (d != ਨਿਰਾ)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    ਖਾਲੀ
f_getwinpos(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪੂਰਨ x = -1;
    ਪੂਰਨ y = -1;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

# ਜੇ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_ਸੰਖਿਆ(&argvars[0]);

	(ਖਾਲੀ)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)x);
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    ਖਾਲੀ
f_getwinposx(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = -1;
# ਜੇ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	ਪੂਰਨ	    x, y;

	ਜੇ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_ਸੰਖਿਆ = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    ਖਾਲੀ
f_getwinposy(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = -1;
# ਜੇ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	ਪੂਰਨ	    x, y;

	ਜੇ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_ਸੰਖਿਆ = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    ਖਾਲੀ
f_tabpagenr(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪੂਰਨ		nr = 1;
    char_u	*arg;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	ਜੇ (arg != ਨਿਰਾ)
	{
	    ਜੇ (STRCMP(arg, "$") == 0)
		nr = tabpage_ਸੂਚਕਾਂਕ(ਨਿਰਾ) - 1;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_ਸੂਚਕਾਂਕ(lastused_tabpage) : 0;
	    ਨਹੀਂਤਾਂ
		semsg(_(e_invalid_ਵਿਅੰਜਕ_str), arg);
	}
    }
    ਨਹੀਂਤਾਂ
	nr = tabpage_ਸੂਚਕਾਂਕ(curtab);
    rettv->vval.v_ਸੰਖਿਆ = nr;
}

/*
 * "tabpagewinnr()" function
 */
    ਖਾਲੀ
f_tabpagewinnr(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪੂਰਨ		nr = 1;
    tabpage_T	*tp;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    tp = ਲੱਭੋ_tabpage((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]));
    ਜੇ (tp == ਨਿਰਾ)
	nr = 0;
    ਨਹੀਂਤਾਂ
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_ਸੰਖਿਆ = nr;
}

/*
 * "win_execute()" function
 */
    ਖਾਲੀ
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    ਪੂਰਨ		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    id = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(argvars);
    wp = win_id2wp_tp(id, &tp);
    ਜੇ (wp == ਨਿਰਾ || tp == ਨਿਰਾ)
	ਵਾਪਸ;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    ਪੂਰਨ	cwd_ਸਥਿਤੀ = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    ਪੂਰਨ	apply_acd = FALSE;
    char_u	*save_sfname = ਨਿਰਾ;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    ਜੇ (curwin != wp
	    && (curwin->w_localdir != ਨਿਰਾ
		|| wp->w_localdir != ਨਿਰਾ
		|| (curtab != tp
		    && (curtab->tp_localdir != ਨਿਰਾ
			|| tp->tp_localdir != ਨਿਰਾ))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_ਸਥਿਤੀ = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    ਜੇ (cwd_ਸਥਿਤੀ == OK && p_acd)
    {
	ਜੇ (curbuf->b_sfname != ਨਿਰਾ && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    ਜੇ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    ਜੇ (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    ਨਹੀਂਤਾਂ
# endif
	ਜੇ (cwd_ਸਥਿਤੀ == OK)
	{
	    mch_chdir((ਅੱਖਰ *)cwd);
# ifdef FEAT_AUTOCHDIR
	    ਜੇ (save_sfname != ਨਿਰਾ)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    ਜੇ (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_ਸਥਿਤੀ = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    ਜੇ (VIsual_ਸਰਗਰਮ)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    ਖਾਲੀ
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    ਖਾਲੀ
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)))
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    ਖਾਲੀ
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    ਪੂਰਨ		id;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    id = tv_get_ਸੰਖਿਆ(&argvars[0]);
    ਜੇ (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_ਸੰਖਿਆ = 1;
	ਵਾਪਸ;
    }

    ਜੇ (ਪਾਠ_or_buf_locked())
	ਵਾਪਸ;
# ਜੇ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ਜੇ (popup_is_popup(curwin) && curbuf->b_ਪਦ != ਨਿਰਾ)
    {
	emsg(_(e_not_allowed_for_terminal_ਵਿੱਚ_popup_ਖਿੜਕੀ));
	ਵਾਪਸ;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	ਜੇ (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    ਜੇ (VIsual_ਸਰਗਰਮ && wp->w_ਬਫਰ != curbuf)
		ਸਿਰਾ_visual_ਬਹੁਲਕ();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_ਸੰਖਿਆ = 1;
	    ਵਾਪਸ;
	}
}

/*
 * "win_id2tabwin()" function
 */
    ਖਾਲੀ
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    ਖਾਲੀ
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    rettv->vval.v_ਸੰਖਿਆ = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    ਖਾਲੀ
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ਪੂਰਨ		ਵਿੱਥ;

    rettv->vval.v_ਸੰਖਿਆ = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    ਜੇ (wp == ਨਿਰਾ || win_valid_popup(wp))
	ਵਾਪਸ;
    ਜੇ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_ਖਿੜਕੀ_ਵਿੱਚ_another_tab_ਪੰਨਾ));
	ਵਾਪਸ;
    }

    ਵਿੱਥ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[1]);
    win_drag_vsep_ਸਤਰ(wp, ਵਿੱਥ);
    rettv->vval.v_ਸੰਖਿਆ = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    ਖਾਲੀ
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ਪੂਰਨ		ਵਿੱਥ;

    rettv->vval.v_ਸੰਖਿਆ = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    ਜੇ (wp == ਨਿਰਾ || win_valid_popup(wp))
	ਵਾਪਸ;
    ਜੇ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_ਖਿੜਕੀ_ਵਿੱਚ_another_tab_ਪੰਨਾ));
	ਵਾਪਸ;
    }

    ਵਿੱਥ = (ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[1]);
    win_drag_ਸਥਿਤੀ_ਸਤਰ(wp, ਵਿੱਥ);
    rettv->vval.v_ਸੰਖਿਆ = TRUE;
}

/*
 * "win_screenpos()" function
 */
    ਖਾਲੀ
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, wp == ਨਿਰਾ ? 0 : wp->w_winrow + 1);
    list_append_ਸੰਖਿਆ(rettv->vval.v_list, wp == ਨਿਰਾ ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    ਖਾਲੀ
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    ਪੂਰਨ     flags = 0, ਮਾਪ = 0;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    targetwin = ਲੱਭੋ_win_by_nr_or_id(&argvars[1]);

    ਜੇ (wp == ਨਿਰਾ || targetwin == ਨਿਰਾ || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_ਖਿੜਕੀ_ਸੰਖਿਆ));
	ਵਾਪਸ;
    }

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	ਜੇ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ਵਾਪਸ;

	d = argvars[2].vval.v_dict;
	ਜੇ (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	ਜੇ ((di = dict_ਲੱਭੋ(d, (char_u *)"rightbelow", -1)) != ਨਿਰਾ)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_ਹੇਠਾਂ : WSP_ABOVE;
	ਮਾਪ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    ਜੇ (ਪਾਠ_or_buf_locked() || check_ਵੰਡੋ_disallowed(wp) == FAIL)
	ਵਾਪਸ;

    ਜੇ (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    ਜੇ (curwin == targetwin && win_valid(wp))
    {
	ਜੇ (win_splitmove(wp, ਮਾਪ, flags) == OK)
	    rettv->vval.v_ਸੰਖਿਆ = 0;
    }
    ਨਹੀਂਤਾਂ
	emsg(_(e_autocommands_caused_command_ਮੰਜ਼ਿਲ_abort));

    ਜੇ (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    ਖਾਲੀ
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
	ਜੇ (wp == ਨਿਰਾ)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    ਵਾਪਸ;
	}
    }
    ਜੇ (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# ਜੇ defined(FEAT_QUICKFIX)
    ਨਹੀਂਤਾਂ ਜੇ (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    ਨਹੀਂਤਾਂ ਜੇ (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    ਨਹੀਂਤਾਂ ਜੇ (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    ਨਹੀਂਤਾਂ ਜੇ (bt_quickfix(wp->w_ਬਫਰ))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != ਨਿਰਾ ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    ਖਾਲੀ
f_getcmdwintype(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;
    rettv->vval.v_string = alloc(2);
    ਜੇ (rettv->vval.v_string == ਨਿਰਾ)
	ਵਾਪਸ;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    ਖਾਲੀ
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    ਜੇ (wp == ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = -1;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = wp->w_ਬਫਰ->b_fnum;
}

/*
 * "wincol()" function
 */
    ਖਾਲੀ
f_wincol(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    validate_cursor();
    ਪੂਰਨ ਖਾਨਾ = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    ਜੇ (curwin->w_p_rl)
	ਖਾਨਾ = curwin->w_ਚੌੜਾਈ - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_ਸੰਖਿਆ = ਖਾਨਾ;
}

/*
 * "winheight(nr)" function
 */
    ਖਾਲੀ
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    ਜੇ (wp == ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = -1;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = wp->w_ਉਚਾਈ;
}

/*
 * "winlayout()" function
 */
    ਖਾਲੀ
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    ਨਹੀਂਤਾਂ
    {
	tp = ਲੱਭੋ_tabpage((ਪੂਰਨ)tv_get_ਸੰਖਿਆ(&argvars[0]));
	ਜੇ (tp == ਨਿਰਾ)
	    ਵਾਪਸ;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    ਖਾਲੀ
f_winline(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_ਸੰਖਿਆ = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    ਖਾਲੀ
f_winnr(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    ਪੂਰਨ		nr = 1;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_ਸੰਖਿਆ = nr;
}

/*
 * "winrestcmd()" function
 */
    ਖਾਲੀ
f_winrestcmd(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    win_T	*wp;
    ਪੂਰਨ		i;
    ਪੂਰਨ		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 70);

    // Do this twice to handle some window layouts properly.
    ਲਈ (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((ਅੱਖਰ *)buf, ਆਕਾਰ(buf),
		":%dresize %d|", winnr, wp->w_ਉਚਾਈ);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((ਅੱਖਰ *)buf, ਆਕਾਰ(buf),
		"vert :%dresize %d|", winnr, wp->w_ਚੌੜਾਈ);
	    ga_concat_len(&ga, buf, buflen);
	    ++winnr;
	}
    }
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_ਅੰਕੜੇ;
    rettv->v_type = VAR_STRING;
}

/*
 * "winrestview()" function
 */
    ਖਾਲੀ
f_winrestview(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    dict_T	*dict;

    ਜੇ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    dict = argvars[0].vval.v_dict;
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_ਸੰਖਿਆ(dict, "lnum");
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "col"))
	curwin->w_cursor.ਖਾਨਾ = (colnr_T)dict_get_ਸੰਖਿਆ(dict, "col");
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_ਸੰਖਿਆ(dict, "coladd");
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_ਸੰਖਿਆ(dict, "curswant");
	curwin->w_ਸੈੱਟ_curswant = false;
    }

    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "topline"))
	ਸੈੱਟ_topline(curwin, (linenr_T)dict_get_ਸੰਖਿਆ(dict, "topline"));
# ifdef FEAT_DIFF
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "topfill"))
	curwin->w_topfill = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(dict, "topfill");
# endif
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_ਸੰਖਿਆ(dict, "leftcol");
    ਜੇ (dict_has_ਕੁੰਜੀ(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_ਸੰਖਿਆ(dict, "skipcol");

    check_cursor();
    win_new_ਉਚਾਈ(curwin, curwin->w_ਉਚਾਈ);
    win_new_ਚੌੜਾਈ(curwin, curwin->w_ਚੌੜਾਈ);
    ਬਦਲਿਆ_ਖਿੜਕੀ_setting();

    ਜੇ (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    ਜੇ (curwin->w_topline > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	curwin->w_topline = curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    ਖਾਲੀ
f_winsaveview(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    dict_T	*dict;

    ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    dict = rettv->vval.v_dict;

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "lnum", (ਲੰਮਾ)curwin->w_cursor.lnum);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "col", (ਲੰਮਾ)curwin->w_cursor.ਖਾਨਾ);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "coladd", (ਲੰਮਾ)curwin->w_cursor.coladd);
    update_curswant();
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "curswant", (ਲੰਮਾ)curwin->w_curswant);

    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "topline", (ਲੰਮਾ)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "topfill", (ਲੰਮਾ)curwin->w_topfill);
# endif
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "leftcol", (ਲੰਮਾ)curwin->w_leftcol);
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "skipcol", (ਲੰਮਾ)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    ਖਾਲੀ
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_ਸੰਖਿਆ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    wp = ਲੱਭੋ_win_by_nr_or_id(&argvars[0]);
    ਜੇ (wp == ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = -1;
    ਨਹੀਂਤਾਂ
	rettv->vval.v_ਸੰਖਿਆ = wp->w_ਚੌੜਾਈ;
}
#ਜੇ_ਅੰਤ // FEAT_EVAL

#ਜੇ defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    ਪੂਰਨ
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	ਪੂਰਨ	    no_display)
{
    ਖੰਡ_autocmds();
    ਵਾਪਸ switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    ਪੂਰਨ
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	ਪੂਰਨ	    no_display)
{
    CLEAR_ਸੂਚਕ(switchwin);
    switchwin->sw_curwin = curwin;
    ਜੇ (win == curwin)
	switchwin->sw_same_win = TRUE;
    ਨਹੀਂਤਾਂ
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_ਸਰਗਰਮ = VIsual_ਸਰਗਰਮ;
	VIsual_ਸਰਗਰਮ = FALSE;
    }

    ਜੇ (tp != ਨਿਰਾ)
    {
	switchwin->sw_curtab = curtab;
	ਜੇ (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	ਨਹੀਂਤਾਂ
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    ਜੇ (!win_valid(win))
	ਵਾਪਸ FAIL;
    curwin = win;
    curbuf = curwin->w_ਬਫਰ;
    ਵਾਪਸ OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    ਖਾਲੀ
restore_win(
	switchwin_T *switchwin,
	ਪੂਰਨ	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    ਖਾਲੀ
restore_win_noblock(
	switchwin_T *switchwin,
	ਪੂਰਨ	    no_display)
{
    ਜੇ (switchwin->sw_curtab != ਨਿਰਾ && valid_tabpage(switchwin->sw_curtab))
    {
	ਜੇ (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	ਨਹੀਂਤਾਂ
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    ਜੇ (!switchwin->sw_same_win)
	VIsual_ਸਰਗਰਮ = switchwin->sw_visual_ਸਰਗਰਮ;

    ਜੇ (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_ਬਫਰ;
    }
# ifdef FEAT_PROP_POPUP
    ਨਹੀਂਤਾਂ ਜੇ (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#ਜੇ_ਅੰਤ
