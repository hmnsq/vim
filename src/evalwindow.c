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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

    స్థిర పూర్ణాంకం
win_getid(typval_T *argvars)
{
    పూర్ణాంకం	    winnr;
    win_T   *wp;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
	తిరిగి curwin->w_id;
    winnr = tv_get_సంఖ్య(&argvars[0]);
    ఒకవేళ (winnr <= 0)
	తిరిగి 0;

    ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    లేకపోతే
    {
	tabpage_T	*tp;
	పూర్ణాంకం		tabnr = tv_get_సంఖ్య(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    ఒకవేళ (--tabnr == 0)
		ఆపు;
	ఒకవేళ (tp == ఖాళీ)
	    తిరిగి -1;
	ఒకవేళ (tp == curtab)
	    wp = firstwin;
	లేకపోతే
	    wp = tp->tp_firstwin;
    }
    కొరకు ( ; wp != ఖాళీ; wp = wp->w_తదుపరి)
	ఒకవేళ (--winnr == 0)
	    తిరిగి wp->w_id;
    తిరిగి 0;
}

    స్థిర శూన్య
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    పూర్ణాంకం		winnr = 1;
    పూర్ణాంకం		tabnr = 1;
    పూర్ణాంకం		id = tv_get_సంఖ్య(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
	{
	    ఒకవేళ (wp->w_id == id)
	    {
		list_append_సంఖ్య(list, tabnr);
		list_append_సంఖ్య(list, winnr);
		తిరిగి;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_సంఖ్య(list, 0);
    list_append_సంఖ్య(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(పూర్ణాంకం id)
{
    తిరిగి win_id2wp_tp(id, ఖాళీ);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(పూర్ణాంకం id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ఒకవేళ (wp->w_id == id)
	{
	    ఒకవేళ (tpp != ఖాళీ)
		*tpp = tp;
	    తిరిగి wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_లోపల_TAB(tp, wp)
	     ఒకవేళ (wp->w_id == id)
	     {
		 ఒకవేళ (tpp != ఖాళీ)
		     *tpp = tp;
		 తిరిగి wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	ఒకవేళ (wp->w_id == id)
	{
	    ఒకవేళ (tpp != ఖాళీ)
		*tpp = curtab;  // any tabpage would do
	    తిరిగి wp;
	}
# endif

    తిరిగి ఖాళీ;
}

    స్థిర పూర్ణాంకం
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    పూర్ణాంకం	    nr = 1;
    పూర్ణాంకం	    id = tv_get_సంఖ్య(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (wp->w_id == id)
	    తిరిగి nr;
	++nr;
    }
    తిరిగి 0;
}

    శూన్య
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    పూర్ణాంకం		bufnr = tv_get_సంఖ్య(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    ఒకవేళ (wp->w_బఫరు->b_fnum == bufnr)
		list_append_సంఖ్య(list, wp->w_id);
}

/*
 * Find window specified by "vp" in tabpage "tp".
 * Returns current window if "vp" is number zero.
 * Returns NULL if not found.
 */
    win_T *
కనుగొను_win_by_nr(
    typval_T	*vp,
    tabpage_T	*tp)	// NULL for current tab page
{
    win_T	*wp;
    పూర్ణాంకం		nr = (పూర్ణాంకం)tv_get_సంఖ్య_chk(vp, ఖాళీ);

    ఒకవేళ (nr < 0)
	తిరిగి ఖాళీ;
    ఒకవేళ (nr == 0)
	తిరిగి curwin;

    FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
    {
	ఒకవేళ (nr >= అత్యల్ప_WIN_ID)
	{
	    ఒకవేళ (wp->w_id == nr)
		తిరిగి wp;
	}
	లేకపోతే ఒకవేళ (--nr <= 0)
	    ఆపు;
    }
    ఒకవేళ (nr >= అత్యల్ప_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	కొరకు (wp = (tp == ఖాళీ ? curtab : tp)->tp_మొదటి_popupwin;
						   wp != ఖాళీ; wp = wp->w_తదుపరి)
	    ఒకవేళ (wp->w_id == nr)
		తిరిగి wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    ఒకవేళ (wp->w_id == nr)
		తిరిగి wp;
# endif
	తిరిగి ఖాళీ;
    }
    తిరిగి wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
కనుగొను_win_by_nr_or_id(typval_T *vp)
{
    పూర్ణాంకం	nr = (పూర్ణాంకం)tv_get_సంఖ్య_chk(vp, ఖాళీ);

    ఒకవేళ (nr >= అత్యల్ప_WIN_ID)
	తిరిగి win_id2wp(tv_get_సంఖ్య(vp));
    తిరిగి కనుగొను_win_by_nr(vp, ఖాళీ);
}

/*
 * Find window specified by "wvp" in tabpage "tvp".
 * Returns the tab page in 'ptp'
 */
    win_T *
కనుగొను_tabwin(
    typval_T	*wvp,	// VAR_UNKNOWN for current window
    typval_T	*tvp,	// VAR_UNKNOWN for current tab page
    tabpage_T	**ptp)
{
    win_T	*wp = ఖాళీ;
    tabpage_T	*tp = ఖాళీ;
    దీర్ఘ	n;

    ఒకవేళ (wvp->v_type != VAR_UNKNOWN)
    {
	ఒకవేళ (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (దీర్ఘ)tv_get_సంఖ్య(tvp);
	    ఒకవేళ (n >= 0)
		tp = కనుగొను_tabpage(n);
	}
	లేకపోతే
	    tp = curtab;

	ఒకవేళ (tp != ఖాళీ)
	{
	    wp = కనుగొను_win_by_nr(wvp, tp);
	    ఒకవేళ (wp == ఖాళీ && wvp->v_type == VAR_సంఖ్య
						&& wvp->vval.v_సంఖ్య != -1)
		// A window with the specified number is not found
		tp = ఖాళీ;
	}
    }
    లేకపోతే
    {
	wp = curwin;
	tp = curtab;
    }

    ఒకవేళ (ptp != ఖాళీ)
	*ptp = tp;

    తిరిగి wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    స్థిర శూన్య
get_framelayout(చట్రం_T *fr, list_T *l, పూర్ణాంకం outer)
{
    చట్రం_T	*బిడ్డ;
    list_T	*fr_list;
    list_T	*win_list;

    ఒకవేళ (fr == ఖాళీ)
	తిరిగి;

    ఒకవేళ (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    లేకపోతే
    {
	fr_list = list_alloc();
	ఒకవేళ (fr_list == ఖాళీ)
	    తిరిగి;
	ఒకవేళ (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    తిరిగి;
	}
    }

    ఒకవేళ (fr->fr_layout == FR_ఆకు)
    {
	ఒకవేళ (fr->fr_win != ఖాళీ)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_సంఖ్య(fr_list, fr->fr_win->w_id);
	}
    }
    లేకపోతే
    {
	ఒకవేళ (fr->fr_layout == FR_అడ్డువరుస)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	లేకపోతే
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	ఒకవేళ (win_list == ఖాళీ)
	    తిరిగి;
	ఒకవేళ (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    తిరిగి;
	}

	బిడ్డ = fr->fr_బిడ్డ;
	అయితే (బిడ్డ != ఖాళీ)
	{
	    get_framelayout(బిడ్డ, win_list, FALSE);
	    బిడ్డ = బిడ్డ->fr_తదుపరి;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    స్థిర పూర్ణాంకం
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    పూర్ణాంకం		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    ఒకవేళ (argvar->v_type != VAR_UNKNOWN)
    {
	పూర్ణాంకం	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	ఒకవేళ (arg == ఖాళీ)
	    nr = 0;		// type error; errmsg already given
	లేకపోతే ఒకవేళ (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	లేకపోతే ఒకవేళ (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	లేకపోతే
	{
	    దీర్ఘ	లెక్క;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    లెక్క = strtol((అక్షరం *)arg, (అక్షరం **)&endp, 10);
	    ఒకవేళ (లెక్క <= 0)
		లెక్క = 1;	// if count is not specified, default to 1
	    ఒకవేళ (endp != ఖాళీ && *endp != '\0')
	    {
		ఒకవేళ (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, లెక్క);
		లేకపోతే ఒకవేళ (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, లెక్క);
		లేకపోతే ఒకవేళ (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, లెక్క);
		లేకపోతే ఒకవేళ (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, లెక్క);
		లేకపోతే
		    invalid_arg = TRUE;
	    }
	    లేకపోతే
		invalid_arg = TRUE;
	}
	ఒకవేళ (twin == ఖాళీ)
	    nr = 0;

	ఒకవేళ (invalid_arg)
	{
	    semsg(_(e_invalid_వ్యక్తీకరణ_str), arg);
	    nr = 0;
	}
    }

    ఒకవేళ (nr <= 0)
	తిరిగి 0;

    కొరకు (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_తదుపరి)
    {
	ఒకవేళ (wp == ఖాళీ)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    ఆపు;
	}
	++nr;
    }
    తిరిగి nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    స్థిర dict_T *
get_win_info(win_T *wp, పొట్టి tpnr, పొట్టి winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    ఒకవేళ (dict == ఖాళీ)
	తిరిగి ఖాళీ;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_కూడు_సంఖ్య(dict, "tabnr", tpnr);
    dict_కూడు_సంఖ్య(dict, "winnr", winnr);
    dict_కూడు_సంఖ్య(dict, "winid", wp->w_id);
    dict_కూడు_సంఖ్య(dict, "height", wp->w_ఎత్తు);
    dict_కూడు_సంఖ్య(dict, "status_height", wp->w_స్థితి_ఎత్తు);
    dict_కూడు_సంఖ్య(dict, "winrow", wp->w_winrow + 1);
    dict_కూడు_సంఖ్య(dict, "topline", wp->w_topline);
    dict_కూడు_సంఖ్య(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_కూడు_సంఖ్య(dict, "winbar", wp->w_winbar_ఎత్తు);
# endif
    dict_కూడు_సంఖ్య(dict, "width", wp->w_వెడల్పు);
    dict_కూడు_సంఖ్య(dict, "wincol", wp->w_wincol + 1);
    dict_కూడు_సంఖ్య(dict, "textoff", win_నిలువు_off(wp));
    dict_కూడు_సంఖ్య(dict, "bufnr", wp->w_బఫరు->b_fnum);
    dict_కూడు_సంఖ్య(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_కూడు_సంఖ్య(dict, "terminal", bt_terminal(wp->w_బఫరు));
# endif
# ifdef FEAT_QUICKFIX
    dict_కూడు_సంఖ్య(dict, "quickfix", bt_quickfix(wp->w_బఫరు));
    dict_కూడు_సంఖ్య(dict, "loclist",
		      (bt_quickfix(wp->w_బఫరు) && wp->w_llist_ref != ఖాళీ));
# endif

    // Add a reference to window variables
    dict_కూడు_dict(dict, "variables", wp->w_vars);

    తిరిగి dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    స్థిర dict_T *
get_tabpage_info(tabpage_T *tp, పూర్ణాంకం tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    ఒకవేళ (dict == ఖాళీ)
	తిరిగి ఖాళీ;

    dict_కూడు_సంఖ్య(dict, "tabnr", tp_idx);

    l = list_alloc();
    ఒకవేళ (l != ఖాళీ)
    {
	FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
	    list_append_సంఖ్య(l, (varnumber_T)wp->w_id);
	dict_కూడు_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_కూడు_dict(dict, "variables", tp->tp_vars);

    తిరిగి dict;
}

/*
 * "gettabinfo()" function
 */
    శూన్య
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = ఖాళీ;
    dict_T	*d;
    పూర్ణాంకం		tpnr = 0;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = కనుగొను_tabpage((పూర్ణాంకం)tv_get_సంఖ్య_chk(&argvars[0], ఖాళీ));
	ఒకవేళ (tparg == ఖాళీ)
	    తిరిగి;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	ఒకవేళ (tparg != ఖాళీ && tp != tparg)
	    కొనసాగు;
	d = get_tabpage_info(tp, tpnr);
	ఒకవేళ (d != ఖాళీ)
	    list_append_dict(rettv->vval.v_list, d);
	ఒకవేళ (tparg != ఖాళీ)
	    తిరిగి;
    }
}

/*
 * "getwininfo()" function
 */
    శూన్య
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = ఖాళీ, *wparg = ఖాళీ;
    dict_T	*d;
    పొట్టి	tabnr = 0, winnr;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_సంఖ్య(&argvars[0]));
	ఒకవేళ (wparg == ఖాళీ)
	    తిరిగి;
    }

    // Collect information about either all the windows across all the tab
    // pages or one particular window.
    FOR_ALL_TABPAGES(tp)
    {
	tabnr++;
	winnr = 0;
	FOR_ALL_WINDOWS_లోపల_TAB(tp, wp)
	{
	    winnr++;
	    ఒకవేళ (wparg != ఖాళీ && wp != wparg)
		కొనసాగు;
	    d = get_win_info(wp, tabnr, winnr);
	    ఒకవేళ (d != ఖాళీ)
		list_append_dict(rettv->vval.v_list, d);
	    ఒకవేళ (wparg != ఖాళీ)
		// found information about a specific window
		తిరిగి;
	}
    }
# ifdef FEAT_PROP_POPUP
    ఒకవేళ (wparg != ఖాళీ)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_లోపల_TAB(tp, wp)
		ఒకవేళ (wp == wparg)
		    వెళ్ళు దొరికింది;
	}
దొరికింది:
	d = get_win_info(wparg, tp == ఖాళీ ? 0 : tabnr, 0);
	ఒకవేళ (d != ఖాళీ)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    శూన్య
f_getwinpos(typval_T *argvars వాడని, typval_T *rettv)
{
    పూర్ణాంకం x = -1;
    పూర్ణాంకం y = -1;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

# ఒకవేళ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_సంఖ్య(&argvars[0]);

	(శూన్య)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)x);
    list_append_సంఖ్య(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    శూన్య
f_getwinposx(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = -1;
# ఒకవేళ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	పూర్ణాంకం	    x, y;

	ఒకవేళ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_సంఖ్య = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    శూన్య
f_getwinposy(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = -1;
# ఒకవేళ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	పూర్ణాంకం	    x, y;

	ఒకవేళ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_సంఖ్య = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    శూన్య
f_tabpagenr(typval_T *argvars వాడని, typval_T *rettv)
{
    పూర్ణాంకం		nr = 1;
    char_u	*arg;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	ఒకవేళ (arg != ఖాళీ)
	{
	    ఒకవేళ (STRCMP(arg, "$") == 0)
		nr = tabpage_సూచీ(ఖాళీ) - 1;
	    లేకపోతే ఒకవేళ (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_సూచీ(lastused_tabpage) : 0;
	    లేకపోతే
		semsg(_(e_invalid_వ్యక్తీకరణ_str), arg);
	}
    }
    లేకపోతే
	nr = tabpage_సూచీ(curtab);
    rettv->vval.v_సంఖ్య = nr;
}

/*
 * "tabpagewinnr()" function
 */
    శూన్య
f_tabpagewinnr(typval_T *argvars వాడని, typval_T *rettv)
{
    పూర్ణాంకం		nr = 1;
    tabpage_T	*tp;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    tp = కనుగొను_tabpage((పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]));
    ఒకవేళ (tp == ఖాళీ)
	nr = 0;
    లేకపోతే
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_సంఖ్య = nr;
}

/*
 * "win_execute()" function
 */
    శూన్య
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    పూర్ణాంకం		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	తిరిగి;

    id = (పూర్ణాంకం)tv_get_సంఖ్య(argvars);
    wp = win_id2wp_tp(id, &tp);
    ఒకవేళ (wp == ఖాళీ || tp == ఖాళీ)
	తిరిగి;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    పూర్ణాంకం	cwd_స్థితి = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    పూర్ణాంకం	apply_acd = FALSE;
    char_u	*save_sfname = ఖాళీ;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    ఒకవేళ (curwin != wp
	    && (curwin->w_localdir != ఖాళీ
		|| wp->w_localdir != ఖాళీ
		|| (curtab != tp
		    && (curtab->tp_localdir != ఖాళీ
			|| tp->tp_localdir != ఖాళీ))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_స్థితి = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    ఒకవేళ (cwd_స్థితి == OK && p_acd)
    {
	ఒకవేళ (curbuf->b_sfname != ఖాళీ && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    ఒకవేళ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    ఒకవేళ (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    లేకపోతే
# endif
	ఒకవేళ (cwd_స్థితి == OK)
	{
	    mch_chdir((అక్షరం *)cwd);
# ifdef FEAT_AUTOCHDIR
	    ఒకవేళ (save_sfname != ఖాళీ)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    ఒకవేళ (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_స్థితి = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    ఒకవేళ (VIsual_చురుకైన)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    శూన్య
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    శూన్య
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL)))
	తిరిగి;

    rettv->vval.v_సంఖ్య = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    శూన్య
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    పూర్ణాంకం		id;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    id = tv_get_సంఖ్య(&argvars[0]);
    ఒకవేళ (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_సంఖ్య = 1;
	తిరిగి;
    }

    ఒకవేళ (పాఠ్యం_or_buf_locked())
	తిరిగి;
# ఒకవేళ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ఒకవేళ (popup_is_popup(curwin) && curbuf->b_పదం != ఖాళీ)
    {
	emsg(_(e_not_allowed_for_terminal_లోపల_popup_కిటికీ));
	తిరిగి;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	ఒకవేళ (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    ఒకవేళ (VIsual_చురుకైన && wp->w_బఫరు != curbuf)
		చివర_visual_బహుళకం();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_సంఖ్య = 1;
	    తిరిగి;
	}
}

/*
 * "win_id2tabwin()" function
 */
    శూన్య
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    శూన్య
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    rettv->vval.v_సంఖ్య = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    శూన్య
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    పూర్ణాంకం		స్థానభ్రంశం;

    rettv->vval.v_సంఖ్య = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    ఒకవేళ (wp == ఖాళీ || win_valid_popup(wp))
	తిరిగి;
    ఒకవేళ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_కిటికీ_లోపల_another_tab_పుట));
	తిరిగి;
    }

    స్థానభ్రంశం = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[1]);
    win_drag_vsep_పంక్తి(wp, స్థానభ్రంశం);
    rettv->vval.v_సంఖ్య = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    శూన్య
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    పూర్ణాంకం		స్థానభ్రంశం;

    rettv->vval.v_సంఖ్య = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    ఒకవేళ (wp == ఖాళీ || win_valid_popup(wp))
	తిరిగి;
    ఒకవేళ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_కిటికీ_లోపల_another_tab_పుట));
	తిరిగి;
    }

    స్థానభ్రంశం = (పూర్ణాంకం)tv_get_సంఖ్య(&argvars[1]);
    win_drag_స్థితి_పంక్తి(wp, స్థానభ్రంశం);
    rettv->vval.v_సంఖ్య = TRUE;
}

/*
 * "win_screenpos()" function
 */
    శూన్య
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    list_append_సంఖ్య(rettv->vval.v_list, wp == ఖాళీ ? 0 : wp->w_winrow + 1);
    list_append_సంఖ్య(rettv->vval.v_list, wp == ఖాళీ ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    శూన్య
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    పూర్ణాంకం     flags = 0, కొలత = 0;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_సంఖ్య_arg(argvars, 0) == FAIL
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	తిరిగి;

    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    targetwin = కనుగొను_win_by_nr_or_id(&argvars[1]);

    ఒకవేళ (wp == ఖాళీ || targetwin == ఖాళీ || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_కిటికీ_సంఖ్య));
	తిరిగి;
    }

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	ఒకవేళ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    తిరిగి;

	d = argvars[2].vval.v_dict;
	ఒకవేళ (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	ఒకవేళ ((di = dict_కనుగొను(d, (char_u *)"rightbelow", -1)) != ఖాళీ)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_క్రింద : WSP_ABOVE;
	కొలత = (పూర్ణాంకం)dict_get_సంఖ్య(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    ఒకవేళ (పాఠ్యం_or_buf_locked() || check_విభజించు_disallowed(wp) == FAIL)
	తిరిగి;

    ఒకవేళ (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    ఒకవేళ (curwin == targetwin && win_valid(wp))
    {
	ఒకవేళ (win_splitmove(wp, కొలత, flags) == OK)
	    rettv->vval.v_సంఖ్య = 0;
    }
    లేకపోతే
	emsg(_(e_autocommands_caused_command_గమ్యం_abort));

    ఒకవేళ (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    శూన్య
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
	ఒకవేళ (wp == ఖాళీ)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    తిరిగి;
	}
    }
    ఒకవేళ (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# ఒకవేళ defined(FEAT_QUICKFIX)
    లేకపోతే ఒకవేళ (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    లేకపోతే ఒకవేళ (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    లేకపోతే ఒకవేళ (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    లేకపోతే ఒకవేళ (bt_quickfix(wp->w_బఫరు))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != ఖాళీ ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    శూన్య
f_getcmdwintype(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;
    rettv->vval.v_string = alloc(2);
    ఒకవేళ (rettv->vval.v_string == ఖాళీ)
	తిరిగి;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    శూన్య
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    ఒకవేళ (wp == ఖాళీ)
	rettv->vval.v_సంఖ్య = -1;
    లేకపోతే
	rettv->vval.v_సంఖ్య = wp->w_బఫరు->b_fnum;
}

/*
 * "wincol()" function
 */
    శూన్య
f_wincol(typval_T *argvars వాడని, typval_T *rettv)
{
    validate_cursor();
    పూర్ణాంకం నిలువు = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    ఒకవేళ (curwin->w_p_rl)
	నిలువు = curwin->w_వెడల్పు - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_సంఖ్య = నిలువు;
}

/*
 * "winheight(nr)" function
 */
    శూన్య
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    ఒకవేళ (wp == ఖాళీ)
	rettv->vval.v_సంఖ్య = -1;
    లేకపోతే
	rettv->vval.v_సంఖ్య = wp->w_ఎత్తు;
}

/*
 * "winlayout()" function
 */
    శూన్య
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    లేకపోతే
    {
	tp = కనుగొను_tabpage((పూర్ణాంకం)tv_get_సంఖ్య(&argvars[0]));
	ఒకవేళ (tp == ఖాళీ)
	    తిరిగి;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    శూన్య
f_winline(typval_T *argvars వాడని, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_సంఖ్య = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    శూన్య
f_winnr(typval_T *argvars వాడని, typval_T *rettv)
{
    పూర్ణాంకం		nr = 1;

    ఒకవేళ (లోపల_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_సంఖ్య = nr;
}

/*
 * "winrestcmd()" function
 */
    శూన్య
f_winrestcmd(typval_T *argvars వాడని, typval_T *rettv)
{
    win_T	*wp;
    పూర్ణాంకం		i;
    పూర్ణాంకం		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, పరిమాణం(అక్షరం), 70);

    // Do this twice to handle some window layouts properly.
    కొరకు (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((అక్షరం *)buf, పరిమాణం(buf),
		":%dresize %d|", winnr, wp->w_ఎత్తు);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((అక్షరం *)buf, పరిమాణం(buf),
		"vert :%dresize %d|", winnr, wp->w_వెడల్పు);
	    ga_concat_len(&ga, buf, buflen);
	    ++winnr;
	}
    }
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_దత్తాంశం;
    rettv->v_type = VAR_STRING;
}

/*
 * "winrestview()" function
 */
    శూన్య
f_winrestview(typval_T *argvars, typval_T *rettv వాడని)
{
    dict_T	*dict;

    ఒకవేళ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    dict = argvars[0].vval.v_dict;
    ఒకవేళ (dict_has_కీలకం(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_సంఖ్య(dict, "lnum");
    ఒకవేళ (dict_has_కీలకం(dict, "col"))
	curwin->w_cursor.నిలువు = (colnr_T)dict_get_సంఖ్య(dict, "col");
    ఒకవేళ (dict_has_కీలకం(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_సంఖ్య(dict, "coladd");
    ఒకవేళ (dict_has_కీలకం(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_సంఖ్య(dict, "curswant");
	curwin->w_సమితి_curswant = false;
    }

    ఒకవేళ (dict_has_కీలకం(dict, "topline"))
	సమితి_topline(curwin, (linenr_T)dict_get_సంఖ్య(dict, "topline"));
# ifdef FEAT_DIFF
    ఒకవేళ (dict_has_కీలకం(dict, "topfill"))
	curwin->w_topfill = (పూర్ణాంకం)dict_get_సంఖ్య(dict, "topfill");
# endif
    ఒకవేళ (dict_has_కీలకం(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_సంఖ్య(dict, "leftcol");
    ఒకవేళ (dict_has_కీలకం(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_సంఖ్య(dict, "skipcol");

    check_cursor();
    win_new_ఎత్తు(curwin, curwin->w_ఎత్తు);
    win_new_వెడల్పు(curwin, curwin->w_వెడల్పు);
    మారింది_కిటికీ_setting();

    ఒకవేళ (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    ఒకవేళ (curwin->w_topline > curbuf->b_ml.ml_పంక్తి_లెక్క)
	curwin->w_topline = curbuf->b_ml.ml_పంక్తి_లెక్క;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    శూన్య
f_winsaveview(typval_T *argvars వాడని, typval_T *rettv)
{
    dict_T	*dict;

    ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	తిరిగి;
    dict = rettv->vval.v_dict;

    dict_కూడు_సంఖ్య(dict, "lnum", (దీర్ఘ)curwin->w_cursor.lnum);
    dict_కూడు_సంఖ్య(dict, "col", (దీర్ఘ)curwin->w_cursor.నిలువు);
    dict_కూడు_సంఖ్య(dict, "coladd", (దీర్ఘ)curwin->w_cursor.coladd);
    update_curswant();
    dict_కూడు_సంఖ్య(dict, "curswant", (దీర్ఘ)curwin->w_curswant);

    dict_కూడు_సంఖ్య(dict, "topline", (దీర్ఘ)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_కూడు_సంఖ్య(dict, "topfill", (దీర్ఘ)curwin->w_topfill);
# endif
    dict_కూడు_సంఖ్య(dict, "leftcol", (దీర్ఘ)curwin->w_leftcol);
    dict_కూడు_సంఖ్య(dict, "skipcol", (దీర్ఘ)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    శూన్య
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ఒకవేళ (లోపల_vim9script() && check_for_సంఖ్య_arg(argvars, 0) == FAIL)
	తిరిగి;

    wp = కనుగొను_win_by_nr_or_id(&argvars[0]);
    ఒకవేళ (wp == ఖాళీ)
	rettv->vval.v_సంఖ్య = -1;
    లేకపోతే
	rettv->vval.v_సంఖ్య = wp->w_వెడల్పు;
}
#అయితే_ముగింపు // FEAT_EVAL

#ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    పూర్ణాంకం
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	పూర్ణాంకం	    no_display)
{
    ఖండం_autocmds();
    తిరిగి switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    పూర్ణాంకం
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	పూర్ణాంకం	    no_display)
{
    CLEAR_సూచిక(switchwin);
    switchwin->sw_curwin = curwin;
    ఒకవేళ (win == curwin)
	switchwin->sw_same_win = TRUE;
    లేకపోతే
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_చురుకైన = VIsual_చురుకైన;
	VIsual_చురుకైన = FALSE;
    }

    ఒకవేళ (tp != ఖాళీ)
    {
	switchwin->sw_curtab = curtab;
	ఒకవేళ (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	లేకపోతే
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    ఒకవేళ (!win_valid(win))
	తిరిగి FAIL;
    curwin = win;
    curbuf = curwin->w_బఫరు;
    తిరిగి OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    శూన్య
restore_win(
	switchwin_T *switchwin,
	పూర్ణాంకం	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    శూన్య
restore_win_noblock(
	switchwin_T *switchwin,
	పూర్ణాంకం	    no_display)
{
    ఒకవేళ (switchwin->sw_curtab != ఖాళీ && valid_tabpage(switchwin->sw_curtab))
    {
	ఒకవేళ (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	లేకపోతే
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    ఒకవేళ (!switchwin->sw_same_win)
	VIsual_చురుకైన = switchwin->sw_visual_చురుకైన;

    ఒకవేళ (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_బఫరు;
    }
# ifdef FEAT_PROP_POPUP
    లేకపోతే ఒకవేళ (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#అయితే_ముగింపు
