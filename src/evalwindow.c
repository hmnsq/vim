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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win_getid(typval_T *argvars)
{
    ಪೂರ್ಣಾಂಕ	    winnr;
    win_T   *wp;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
	ಹಿಂತಿರುಗಿ curwin->w_id;
    winnr = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಆದರೆ (winnr <= 0)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	tabpage_T	*tp;
	ಪೂರ್ಣಾಂಕ		tabnr = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    ಆದರೆ (--tabnr == 0)
		ನಿಲ್ಲಿಸು;
	ಆದರೆ (tp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;
	ಆದರೆ (tp == curtab)
	    wp = firstwin;
	ಇಲ್ಲದಿದ್ದರೆ
	    wp = tp->tp_firstwin;
    }
    ಗಾಗಿ ( ; wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	ಆದರೆ (--winnr == 0)
	    ಹಿಂತಿರುಗಿ wp->w_id;
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    ಪೂರ್ಣಾಂಕ		winnr = 1;
    ಪೂರ್ಣಾಂಕ		tabnr = 1;
    ಪೂರ್ಣಾಂಕ		id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
	{
	    ಆದರೆ (wp->w_id == id)
	    {
		list_append_ಸಂಖ್ಯೆ(list, tabnr);
		list_append_ಸಂಖ್ಯೆ(list, winnr);
		ಹಿಂತಿರುಗಿ;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_ಸಂಖ್ಯೆ(list, 0);
    list_append_ಸಂಖ್ಯೆ(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(ಪೂರ್ಣಾಂಕ id)
{
    ಹಿಂತಿರುಗಿ win_id2wp_tp(id, ಖಾಲಿ);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(ಪೂರ್ಣಾಂಕ id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಆದರೆ (wp->w_id == id)
	{
	    ಆದರೆ (tpp != ಖಾಲಿ)
		*tpp = tp;
	    ಹಿಂತಿರುಗಿ wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, wp)
	     ಆದರೆ (wp->w_id == id)
	     {
		 ಆದರೆ (tpp != ಖಾಲಿ)
		     *tpp = tp;
		 ಹಿಂತಿರುಗಿ wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	ಆದರೆ (wp->w_id == id)
	{
	    ಆದರೆ (tpp != ಖಾಲಿ)
		*tpp = curtab;  // any tabpage would do
	    ಹಿಂತಿರುಗಿ wp;
	}
# endif

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    ಪೂರ್ಣಾಂಕ	    nr = 1;
    ಪೂರ್ಣಾಂಕ	    id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (wp->w_id == id)
	    ಹಿಂತಿರುಗಿ nr;
	++nr;
    }
    ಹಿಂತಿರುಗಿ 0;
}

    ಶೂನ್ಯ
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    ಪೂರ್ಣಾಂಕ		bufnr = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಆದರೆ (wp->w_ಬಫರ್->b_fnum == bufnr)
		list_append_ಸಂಖ್ಯೆ(list, wp->w_id);
}

/*
 * Find window specified by "vp" in tabpage "tp".
 * Returns current window if "vp" is number zero.
 * Returns NULL if not found.
 */
    win_T *
ಕಂಡುಹಿಡಿ_win_by_nr(
    typval_T	*vp,
    tabpage_T	*tp)	// NULL for current tab page
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		nr = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(vp, ಖಾಲಿ);

    ಆದರೆ (nr < 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (nr == 0)
	ಹಿಂತಿರುಗಿ curwin;

    FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
    {
	ಆದರೆ (nr >= ಅತಿಕಡಿಮೆ_WIN_ID)
	{
	    ಆದರೆ (wp->w_id == nr)
		ಹಿಂತಿರುಗಿ wp;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (--nr <= 0)
	    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (nr >= ಅತಿಕಡಿಮೆ_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	ಗಾಗಿ (wp = (tp == ಖಾಲಿ ? curtab : tp)->tp_ಮೊದಲ_popupwin;
						   wp != ಖಾಲಿ; wp = wp->w_ಮುಂದಿನ)
	    ಆದರೆ (wp->w_id == nr)
		ಹಿಂತಿರುಗಿ wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    ಆದರೆ (wp->w_id == nr)
		ಹಿಂತಿರುಗಿ wp;
# endif
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
ಕಂಡುಹಿಡಿ_win_by_nr_or_id(typval_T *vp)
{
    ಪೂರ್ಣಾಂಕ	nr = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(vp, ಖಾಲಿ);

    ಆದರೆ (nr >= ಅತಿಕಡಿಮೆ_WIN_ID)
	ಹಿಂತಿರುಗಿ win_id2wp(tv_get_ಸಂಖ್ಯೆ(vp));
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_win_by_nr(vp, ಖಾಲಿ);
}

/*
 * Find window specified by "wvp" in tabpage "tvp".
 * Returns the tab page in 'ptp'
 */
    win_T *
ಕಂಡುಹಿಡಿ_tabwin(
    typval_T	*wvp,	// VAR_UNKNOWN for current window
    typval_T	*tvp,	// VAR_UNKNOWN for current tab page
    tabpage_T	**ptp)
{
    win_T	*wp = ಖಾಲಿ;
    tabpage_T	*tp = ಖಾಲಿ;
    ದೀರ್ಘ	n;

    ಆದರೆ (wvp->v_type != VAR_UNKNOWN)
    {
	ಆದರೆ (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ(tvp);
	    ಆದರೆ (n >= 0)
		tp = ಕಂಡುಹಿಡಿ_tabpage(n);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    tp = curtab;

	ಆದರೆ (tp != ಖಾಲಿ)
	{
	    wp = ಕಂಡುಹಿಡಿ_win_by_nr(wvp, tp);
	    ಆದರೆ (wp == ಖಾಲಿ && wvp->v_type == VAR_ಸಂಖ್ಯೆ
						&& wvp->vval.v_ಸಂಖ್ಯೆ != -1)
		// A window with the specified number is not found
		tp = ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	wp = curwin;
	tp = curtab;
    }

    ಆದರೆ (ptp != ಖಾಲಿ)
	*ptp = tp;

    ಹಿಂತಿರುಗಿ wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_framelayout(ಚೌಕಟ್ಟು_T *fr, list_T *l, ಪೂರ್ಣಾಂಕ outer)
{
    ಚೌಕಟ್ಟು_T	*ಮಗು;
    list_T	*fr_list;
    list_T	*win_list;

    ಆದರೆ (fr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	fr_list = list_alloc();
	ಆದರೆ (fr_list == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    ಹಿಂತಿರುಗಿ;
	}
    }

    ಆದರೆ (fr->fr_layout == FR_ಎಲೆ)
    {
	ಆದರೆ (fr->fr_win != ಖಾಲಿ)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_ಸಂಖ್ಯೆ(fr_list, fr->fr_win->w_id);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (fr->fr_layout == FR_ಅಡ್ಡಸಾಲು)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	ಇಲ್ಲದಿದ್ದರೆ
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	ಆದರೆ (win_list == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
	ಆದರೆ (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    ಹಿಂತಿರುಗಿ;
	}

	ಮಗು = fr->fr_ಮಗು;
	ಆಗ (ಮಗು != ಖಾಲಿ)
	{
	    get_framelayout(ಮಗು, win_list, FALSE);
	    ಮಗು = ಮಗು->fr_ಮುಂದಿನ;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    ಪೂರ್ಣಾಂಕ		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    ಆದರೆ (argvar->v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಂಕ	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	ಆದರೆ (arg == ಖಾಲಿ)
	    nr = 0;		// type error; errmsg already given
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ದೀರ್ಘ	ಎಣಿಕೆ_2;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    ಎಣಿಕೆ_2 = strtol((ಅಕ್ಷರ *)arg, (ಅಕ್ಷರ **)&endp, 10);
	    ಆದರೆ (ಎಣಿಕೆ_2 <= 0)
		ಎಣಿಕೆ_2 = 1;	// if count is not specified, default to 1
	    ಆದರೆ (endp != ಖಾಲಿ && *endp != '\0')
	    {
		ಆದರೆ (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, ಎಣಿಕೆ_2);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, ಎಣಿಕೆ_2);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, ಎಣಿಕೆ_2);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, ಎಣಿಕೆ_2);
		ಇಲ್ಲದಿದ್ದರೆ
		    invalid_arg = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		invalid_arg = TRUE;
	}
	ಆದರೆ (twin == ಖಾಲಿ)
	    nr = 0;

	ಆದರೆ (invalid_arg)
	{
	    semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), arg);
	    nr = 0;
	}
    }

    ಆದರೆ (nr <= 0)
	ಹಿಂತಿರುಗಿ 0;

    ಗಾಗಿ (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_ಮುಂದಿನ)
    {
	ಆದರೆ (wp == ಖಾಲಿ)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    ನಿಲ್ಲಿಸು;
	}
	++nr;
    }
    ಹಿಂತಿರುಗಿ nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    ಸ್ಥಿರ dict_T *
get_win_info(win_T *wp, ಗಿಡ್ಡ tpnr, ಗಿಡ್ಡ winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "tabnr", tpnr);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "winnr", winnr);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "winid", wp->w_id);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "height", wp->w_ಎತ್ತರ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "status_height", wp->w_ಸ್ಥಿತಿ_ಎತ್ತರ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "winrow", wp->w_winrow + 1);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "topline", wp->w_topline);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "winbar", wp->w_winbar_ಎತ್ತರ);
# endif
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "width", wp->w_ಅಗಲ);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "wincol", wp->w_wincol + 1);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "textoff", win_ಲಂಬಸಾಲು_off(wp));
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "bufnr", wp->w_ಬಫರ್->b_fnum);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "terminal", bt_terminal(wp->w_ಬಫರ್));
# endif
# ifdef FEAT_QUICKFIX
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "quickfix", bt_quickfix(wp->w_ಬಫರ್));
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "loclist",
		      (bt_quickfix(wp->w_ಬಫರ್) && wp->w_llist_ref != ಖಾಲಿ));
# endif

    // Add a reference to window variables
    dict_ಕೂಡಿಸು_dict(dict, "variables", wp->w_vars);

    ಹಿಂತಿರುಗಿ dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    ಸ್ಥಿರ dict_T *
get_tabpage_info(tabpage_T *tp, ಪೂರ್ಣಾಂಕ tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "tabnr", tp_idx);

    l = list_alloc();
    ಆದರೆ (l != ಖಾಲಿ)
    {
	FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
	    list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)wp->w_id);
	dict_ಕೂಡಿಸು_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_ಕೂಡಿಸು_dict(dict, "variables", tp->tp_vars);

    ಹಿಂತಿರುಗಿ dict;
}

/*
 * "gettabinfo()" function
 */
    ಶೂನ್ಯ
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = ಖಾಲಿ;
    dict_T	*d;
    ಪೂರ್ಣಾಂಕ		tpnr = 0;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = ಕಂಡುಹಿಡಿ_tabpage((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ಖಾಲಿ));
	ಆದರೆ (tparg == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	ಆದರೆ (tparg != ಖಾಲಿ && tp != tparg)
	    ಮುಂದುವರಿ;
	d = get_tabpage_info(tp, tpnr);
	ಆದರೆ (d != ಖಾಲಿ)
	    list_append_dict(rettv->vval.v_list, d);
	ಆದರೆ (tparg != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }
}

/*
 * "getwininfo()" function
 */
    ಶೂನ್ಯ
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = ಖಾಲಿ, *wparg = ಖಾಲಿ;
    dict_T	*d;
    ಗಿಡ್ಡ	tabnr = 0, winnr;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
	ಆದರೆ (wparg == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }

    // Collect information about either all the windows across all the tab
    // pages or one particular window.
    FOR_ALL_TABPAGES(tp)
    {
	tabnr++;
	winnr = 0;
	FOR_ALL_WINDOWS_ಒಳ_TAB(tp, wp)
	{
	    winnr++;
	    ಆದರೆ (wparg != ಖಾಲಿ && wp != wparg)
		ಮುಂದುವರಿ;
	    d = get_win_info(wp, tabnr, winnr);
	    ಆದರೆ (d != ಖಾಲಿ)
		list_append_dict(rettv->vval.v_list, d);
	    ಆದರೆ (wparg != ಖಾಲಿ)
		// found information about a specific window
		ಹಿಂತಿರುಗಿ;
	}
    }
# ifdef FEAT_PROP_POPUP
    ಆದರೆ (wparg != ಖಾಲಿ)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_ಒಳ_TAB(tp, wp)
		ಆದರೆ (wp == wparg)
		    ಹೋಗು ಸಿಕ್ಕಿತು;
	}
ಸಿಕ್ಕಿತು:
	d = get_win_info(wparg, tp == ಖಾಲಿ ? 0 : tabnr, 0);
	ಆದರೆ (d != ಖಾಲಿ)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    ಶೂನ್ಯ
f_getwinpos(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ x = -1;
    ಪೂರ್ಣಾಂಕ y = -1;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

# ಆದರೆ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);

	(ಶೂನ್ಯ)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)x);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    ಶೂನ್ಯ
f_getwinposx(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
# ಆದರೆ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	ಪೂರ್ಣಾಂಕ	    x, y;

	ಆದರೆ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_ಸಂಖ್ಯೆ = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    ಶೂನ್ಯ
f_getwinposy(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
# ಆದರೆ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	ಪೂರ್ಣಾಂಕ	    x, y;

	ಆದರೆ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_ಸಂಖ್ಯೆ = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    ಶೂನ್ಯ
f_tabpagenr(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		nr = 1;
    char_u	*arg;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	ಆದರೆ (arg != ಖಾಲಿ)
	{
	    ಆದರೆ (STRCMP(arg, "$") == 0)
		nr = tabpage_ಸೂಚ್ಯಂಕ(ಖಾಲಿ) - 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_ಸೂಚ್ಯಂಕ(lastused_tabpage) : 0;
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), arg);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	nr = tabpage_ಸೂಚ್ಯಂಕ(curtab);
    rettv->vval.v_ಸಂಖ್ಯೆ = nr;
}

/*
 * "tabpagewinnr()" function
 */
    ಶೂನ್ಯ
f_tabpagewinnr(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		nr = 1;
    tabpage_T	*tp;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    tp = ಕಂಡುಹಿಡಿ_tabpage((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
    ಆದರೆ (tp == ಖಾಲಿ)
	nr = 0;
    ಇಲ್ಲದಿದ್ದರೆ
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_ಸಂಖ್ಯೆ = nr;
}

/*
 * "win_execute()" function
 */
    ಶೂನ್ಯ
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    id = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = win_id2wp_tp(id, &tp);
    ಆದರೆ (wp == ಖಾಲಿ || tp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    ಪೂರ್ಣಾಂಕ	cwd_ಸ್ಥಿತಿ = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    ಪೂರ್ಣಾಂಕ	apply_acd = FALSE;
    char_u	*save_sfname = ಖಾಲಿ;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    ಆದರೆ (curwin != wp
	    && (curwin->w_localdir != ಖಾಲಿ
		|| wp->w_localdir != ಖಾಲಿ
		|| (curtab != tp
		    && (curtab->tp_localdir != ಖಾಲಿ
			|| tp->tp_localdir != ಖಾಲಿ))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_ಸ್ಥಿತಿ = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    ಆದರೆ (cwd_ಸ್ಥಿತಿ == OK && p_acd)
    {
	ಆದರೆ (curbuf->b_sfname != ಖಾಲಿ && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    ಆದರೆ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    ಆದರೆ (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	ಆದರೆ (cwd_ಸ್ಥಿತಿ == OK)
	{
	    mch_chdir((ಅಕ್ಷರ *)cwd);
# ifdef FEAT_AUTOCHDIR
	    ಆದರೆ (save_sfname != ಖಾಲಿ)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    ಆದರೆ (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_ಸ್ಥಿತಿ = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    ಆದರೆ (VIsual_ಸಕ್ರಿಯ)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    ಶೂನ್ಯ
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    ಶೂನ್ಯ
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    ಶೂನ್ಯ
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    ಪೂರ್ಣಾಂಕ		id;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಆದರೆ (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ಪಠ್ಯ_or_buf_locked())
	ಹಿಂತಿರುಗಿ;
# ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ಆದರೆ (popup_is_popup(curwin) && curbuf->b_ಪದ != ಖಾಲಿ)
    {
	emsg(_(e_not_allowed_for_terminal_ಒಳ_popup_ಕಿಟಕಿ));
	ಹಿಂತಿರುಗಿ;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಆದರೆ (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    ಆದರೆ (VIsual_ಸಕ್ರಿಯ && wp->w_ಬಫರ್ != curbuf)
		ಕೊನೆ_visual_ಬಹುಲಕ();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_ಸಂಖ್ಯೆ = 1;
	    ಹಿಂತಿರುಗಿ;
	}
}

/*
 * "win_id2tabwin()" function
 */
    ಶೂನ್ಯ
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    ಶೂನ್ಯ
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_ಸಂಖ್ಯೆ = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    ಶೂನ್ಯ
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ;

    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    ಆದರೆ (wp == ಖಾಲಿ || win_valid_popup(wp))
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_ಕಿಟಕಿ_ಒಳ_another_tab_ಪುಟ));
	ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಾನಾಂತರ = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    win_drag_vsep_ಸಾಲು(wp, ಸ್ಥಾನಾಂತರ);
    rettv->vval.v_ಸಂಖ್ಯೆ = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    ಶೂನ್ಯ
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಾನಾಂತರ;

    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    ಆದರೆ (wp == ಖಾಲಿ || win_valid_popup(wp))
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_ಕಿಟಕಿ_ಒಳ_another_tab_ಪುಟ));
	ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಾನಾಂತರ = (ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    win_drag_ಸ್ಥಿತಿ_ಸಾಲು(wp, ಸ್ಥಾನಾಂತರ);
    rettv->vval.v_ಸಂಖ್ಯೆ = TRUE;
}

/*
 * "win_screenpos()" function
 */
    ಶೂನ್ಯ
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, wp == ಖಾಲಿ ? 0 : wp->w_winrow + 1);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, wp == ಖಾಲಿ ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    ಶೂನ್ಯ
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    ಪೂರ್ಣಾಂಕ     flags = 0, ಗಾತ್ರ_2 = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    targetwin = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[1]);

    ಆದರೆ (wp == ಖಾಲಿ || targetwin == ಖಾಲಿ || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_ಕಿಟಕಿ_ಸಂಖ್ಯೆ));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	ಆದರೆ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ಹಿಂತಿರುಗಿ;

	d = argvars[2].vval.v_dict;
	ಆದರೆ (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"rightbelow", -1)) != ಖಾಲಿ)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_ಕೆಳಗೆ : WSP_ABOVE;
	ಗಾತ್ರ_2 = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    ಆದರೆ (ಪಠ್ಯ_or_buf_locked() || check_ವಿಭಜಿಸು_disallowed(wp) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    ಆದರೆ (curwin == targetwin && win_valid(wp))
    {
	ಆದರೆ (win_splitmove(wp, ಗಾತ್ರ_2, flags) == OK)
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	emsg(_(e_autocommands_caused_command_ಗಮ್ಯ_abort));

    ಆದರೆ (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    ಶೂನ್ಯ
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
	ಆದರೆ (wp == ಖಾಲಿ)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಆದರೆ (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# ಆದರೆ defined(FEAT_QUICKFIX)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bt_quickfix(wp->w_ಬಫರ್))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != ಖಾಲಿ ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    ಶೂನ್ಯ
f_getcmdwintype(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;
    rettv->vval.v_string = alloc(2);
    ಆದರೆ (rettv->vval.v_string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    ಶೂನ್ಯ
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    ಆದರೆ (wp == ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_ಬಫರ್->b_fnum;
}

/*
 * "wincol()" function
 */
    ಶೂನ್ಯ
f_wincol(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    validate_cursor();
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    ಆದರೆ (curwin->w_p_rl)
	ಲಂಬಸಾಲು = curwin->w_ಅಗಲ - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು;
}

/*
 * "winheight(nr)" function
 */
    ಶೂನ್ಯ
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    ಆದರೆ (wp == ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_ಎತ್ತರ;
}

/*
 * "winlayout()" function
 */
    ಶೂನ್ಯ
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	tp = ಕಂಡುಹಿಡಿ_tabpage((ಪೂರ್ಣಾಂಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
	ಆದರೆ (tp == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    ಶೂನ್ಯ
f_winline(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_ಸಂಖ್ಯೆ = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    ಶೂನ್ಯ
f_winnr(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    ಪೂರ್ಣಾಂಕ		nr = 1;

    ಆದರೆ (ಒಳ_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = nr;
}

/*
 * "winrestcmd()" function
 */
    ಶೂನ್ಯ
f_winrestcmd(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 70);

    // Do this twice to handle some window layouts properly.
    ಗಾಗಿ (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf, ಗಾತ್ರ(buf),
		":%dresize %d|", winnr, wp->w_ಎತ್ತರ);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((ಅಕ್ಷರ *)buf, ಗಾತ್ರ(buf),
		"vert :%dresize %d|", winnr, wp->w_ಅಗಲ);
	    ga_concat_len(&ga, buf, buflen);
	    ++winnr;
	}
    }
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_ದತ್ತಾಂಶ;
    rettv->v_type = VAR_STRING;
}

/*
 * "winrestview()" function
 */
    ಶೂನ್ಯ
f_winrestview(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    dict_T	*dict;

    ಆದರೆ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    dict = argvars[0].vval.v_dict;
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_ಸಂಖ್ಯೆ(dict, "lnum");
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "col"))
	curwin->w_cursor.ಲಂಬಸಾಲು = (colnr_T)dict_get_ಸಂಖ್ಯೆ(dict, "col");
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_ಸಂಖ್ಯೆ(dict, "coladd");
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_ಸಂಖ್ಯೆ(dict, "curswant");
	curwin->w_ಗಣ_curswant = false;
    }

    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "topline"))
	ಗಣ_topline(curwin, (linenr_T)dict_get_ಸಂಖ್ಯೆ(dict, "topline"));
# ifdef FEAT_DIFF
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "topfill"))
	curwin->w_topfill = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(dict, "topfill");
# endif
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_ಸಂಖ್ಯೆ(dict, "leftcol");
    ಆದರೆ (dict_has_ಕೀಲಿ(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_ಸಂಖ್ಯೆ(dict, "skipcol");

    check_cursor();
    win_new_ಎತ್ತರ(curwin, curwin->w_ಎತ್ತರ);
    win_new_ಅಗಲ(curwin, curwin->w_ಅಗಲ);
    ಬದಲಾಯಿತು_ಕಿಟಕಿ_setting();

    ಆದರೆ (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    ಆದರೆ (curwin->w_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	curwin->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    ಶೂನ್ಯ
f_winsaveview(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    dict_T	*dict;

    ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;
    dict = rettv->vval.v_dict;

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "lnum", (ದೀರ್ಘ)curwin->w_cursor.lnum);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "col", (ದೀರ್ಘ)curwin->w_cursor.ಲಂಬಸಾಲು);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "coladd", (ದೀರ್ಘ)curwin->w_cursor.coladd);
    update_curswant();
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "curswant", (ದೀರ್ಘ)curwin->w_curswant);

    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "topline", (ದೀರ್ಘ)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "topfill", (ದೀರ್ಘ)curwin->w_topfill);
# endif
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "leftcol", (ದೀರ್ಘ)curwin->w_leftcol);
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "skipcol", (ದೀರ್ಘ)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    ಶೂನ್ಯ
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ಆದರೆ (ಒಳ_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    wp = ಕಂಡುಹಿಡಿ_win_by_nr_or_id(&argvars[0]);
    ಆದರೆ (wp == ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಇಲ್ಲದಿದ್ದರೆ
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_ಅಗಲ;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL

#ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    ಪೂರ್ಣಾಂಕ
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	ಪೂರ್ಣಾಂಕ	    no_display)
{
    ಖಂಡ_autocmds();
    ಹಿಂತಿರುಗಿ switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    ಪೂರ್ಣಾಂಕ
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	ಪೂರ್ಣಾಂಕ	    no_display)
{
    CLEAR_ಸೂಚಕ(switchwin);
    switchwin->sw_curwin = curwin;
    ಆದರೆ (win == curwin)
	switchwin->sw_same_win = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_ಸಕ್ರಿಯ = VIsual_ಸಕ್ರಿಯ;
	VIsual_ಸಕ್ರಿಯ = FALSE;
    }

    ಆದರೆ (tp != ಖಾಲಿ)
    {
	switchwin->sw_curtab = curtab;
	ಆದರೆ (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    ಆದರೆ (!win_valid(win))
	ಹಿಂತಿರುಗಿ FAIL;
    curwin = win;
    curbuf = curwin->w_ಬಫರ್;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    ಶೂನ್ಯ
restore_win(
	switchwin_T *switchwin,
	ಪೂರ್ಣಾಂಕ	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    ಶೂನ್ಯ
restore_win_noblock(
	switchwin_T *switchwin,
	ಪೂರ್ಣಾಂಕ	    no_display)
{
    ಆದರೆ (switchwin->sw_curtab != ಖಾಲಿ && valid_tabpage(switchwin->sw_curtab))
    {
	ಆದರೆ (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    ಆದರೆ (!switchwin->sw_same_win)
	VIsual_ಸಕ್ರಿಯ = switchwin->sw_visual_ಸಕ್ರಿಯ;

    ಆದರೆ (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_ಬಫರ್;
    }
# ifdef FEAT_PROP_POPUP
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ
