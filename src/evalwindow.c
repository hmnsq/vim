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

#ಯದಿ defined(FEAT_EVAL)

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
win_getid(typval_T *argvars)
{
    ಪೂರ್ಣಾಙ್ಕ	    winnr;
    win_T   *wp;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
	ಪ್ರತ್ಯಾಗಮ curwin->w_id;
    winnr = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಯದಿ (winnr <= 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    ಅನ್ಯಥಾ
    {
	tabpage_T	*tp;
	ಪೂರ್ಣಾಙ್ಕ		tabnr = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    ಯದಿ (--tabnr == 0)
		ಭಙ್ಗ;
	ಯದಿ (tp == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;
	ಯದಿ (tp == curtab)
	    wp = firstwin;
	ಅನ್ಯಥಾ
	    wp = tp->tp_firstwin;
    }
    ಪ್ರತಿ ( ; wp != ರಿಕ್ತ; wp = wp->w_next)
	ಯದಿ (--winnr == 0)
	    ಪ್ರತ್ಯಾಗಮ wp->w_id;
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    ಪೂರ್ಣಾಙ್ಕ		winnr = 1;
    ಪೂರ್ಣಾಙ್ಕ		tabnr = 1;
    ಪೂರ್ಣಾಙ್ಕ		id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ಯದಿ (wp->w_id == id)
	    {
		list_append_ಸಂಖ್ಯೆ(list, tabnr);
		list_append_ಸಂಖ್ಯೆ(list, winnr);
		ಪ್ರತ್ಯಾಗಮ;
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
win_id2wp(ಪೂರ್ಣಾಙ್ಕ id)
{
    ಪ್ರತ್ಯಾಗಮ win_id2wp_tp(id, ರಿಕ್ತ);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(ಪೂರ್ಣಾಙ್ಕ id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಯದಿ (wp->w_id == id)
	{
	    ಯದಿ (tpp != ರಿಕ್ತ)
		*tpp = tp;
	    ಪ್ರತ್ಯಾಗಮ wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     ಯದಿ (wp->w_id == id)
	     {
		 ಯದಿ (tpp != ರಿಕ್ತ)
		     *tpp = tp;
		 ಪ್ರತ್ಯಾಗಮ wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	ಯದಿ (wp->w_id == id)
	{
	    ಯದಿ (tpp != ರಿಕ್ತ)
		*tpp = curtab;  // any tabpage would do
	    ಪ್ರತ್ಯಾಗಮ wp;
	}
# endif

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    ಪೂರ್ಣಾಙ್ಕ	    nr = 1;
    ಪೂರ್ಣಾಙ್ಕ	    id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	ಯದಿ (wp->w_id == id)
	    ಪ್ರತ್ಯಾಗಮ nr;
	++nr;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಶೂನ್ಯ
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    ಪೂರ್ಣಾಙ್ಕ		bufnr = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    ಯದಿ (wp->w_buffer->b_fnum == bufnr)
		list_append_ಸಂಖ್ಯೆ(list, wp->w_id);
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
    ಪೂರ್ಣಾಙ್ಕ		nr = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(vp, ರಿಕ್ತ);

    ಯದಿ (nr < 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (nr == 0)
	ಪ್ರತ್ಯಾಗಮ curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	ಯದಿ (nr >= LOWEST_WIN_ID)
	{
	    ಯದಿ (wp->w_id == nr)
		ಪ್ರತ್ಯಾಗಮ wp;
	}
	ಅನ್ಯಥಾ ಯದಿ (--nr <= 0)
	    ಭಙ್ಗ;
    }
    ಯದಿ (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	ಪ್ರತಿ (wp = (tp == ರಿಕ್ತ ? curtab : tp)->tp_first_popupwin;
						   wp != ರಿಕ್ತ; wp = wp->w_next)
	    ಯದಿ (wp->w_id == nr)
		ಪ್ರತ್ಯಾಗಮ wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    ಯದಿ (wp->w_id == nr)
		ಪ್ರತ್ಯಾಗಮ wp;
# endif
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    ಪೂರ್ಣಾಙ್ಕ	nr = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(vp, ರಿಕ್ತ);

    ಯದಿ (nr >= LOWEST_WIN_ID)
	ಪ್ರತ್ಯಾಗಮ win_id2wp(tv_get_ಸಂಖ್ಯೆ(vp));
    ಪ್ರತ್ಯಾಗಮ find_win_by_nr(vp, ರಿಕ್ತ);
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
    win_T	*wp = ರಿಕ್ತ;
    tabpage_T	*tp = ರಿಕ್ತ;
    long	n;

    ಯದಿ (wvp->v_type != VAR_UNKNOWN)
    {
	ಯದಿ (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_ಸಂಖ್ಯೆ(tvp);
	    ಯದಿ (n >= 0)
		tp = find_tabpage(n);
	}
	ಅನ್ಯಥಾ
	    tp = curtab;

	ಯದಿ (tp != ರಿಕ್ತ)
	{
	    wp = find_win_by_nr(wvp, tp);
	    ಯದಿ (wp == ರಿಕ್ತ && wvp->v_type == VAR_ಸಂಖ್ಯೆ
						&& wvp->vval.v_ಸಂಖ್ಯೆ != -1)
		// A window with the specified number is not found
		tp = ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ
    {
	wp = curwin;
	tp = curtab;
    }

    ಯದಿ (ptp != ರಿಕ್ತ)
	*ptp = tp;

    ಪ್ರತ್ಯಾಗಮ wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_framelayout(frame_T *fr, list_T *l, ಪೂರ್ಣಾಙ್ಕ outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    ಯದಿ (fr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    ಅನ್ಯಥಾ
    {
	fr_list = list_alloc();
	ಯದಿ (fr_list == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    ಯದಿ (fr->fr_layout == FR_LEAF)
    {
	ಯದಿ (fr->fr_win != ರಿಕ್ತ)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_ಸಂಖ್ಯೆ(fr_list, fr->fr_win->w_id);
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	ಅನ್ಯಥಾ
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	ಯದಿ (win_list == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	child = fr->fr_child;
	ಯಾವತ್ (child != ರಿಕ್ತ)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    ಪೂರ್ಣಾಙ್ಕ		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    ಯದಿ (argvar->v_type != VAR_UNKNOWN)
    {
	ಪೂರ್ಣಾಙ್ಕ	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	ಯದಿ (arg == ರಿಕ್ತ)
	    nr = 0;		// type error; errmsg already given
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	ಅನ್ಯಥಾ
	{
	    long	ಎಣಿಕೆ;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    ಎಣಿಕೆ = strtol((ವರ್ಣ *)arg, (ವರ್ಣ **)&endp, 10);
	    ಯದಿ (ಎಣಿಕೆ <= 0)
		ಎಣಿಕೆ = 1;	// if count is not specified, default to 1
	    ಯದಿ (endp != ರಿಕ್ತ && *endp != '\0')
	    {
		ಯದಿ (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, ಎಣಿಕೆ);
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, ಎಣಿಕೆ);
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, ಎಣಿಕೆ);
		ಅನ್ಯಥಾ ಯದಿ (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, ಎಣಿಕೆ);
		ಅನ್ಯಥಾ
		    invalid_arg = TRUE;
	    }
	    ಅನ್ಯಥಾ
		invalid_arg = TRUE;
	}
	ಯದಿ (twin == ರಿಕ್ತ)
	    nr = 0;

	ಯದಿ (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    ಯದಿ (nr <= 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪ್ರತಿ (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	ಯದಿ (wp == ರಿಕ್ತ)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    ಭಙ್ಗ;
	}
	++nr;
    }
    ಪ್ರತ್ಯಾಗಮ nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    ಸ್ಥಿರ dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    ಯದಿ (dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_add_ಸಂಖ್ಯೆ(dict, "tabnr", tpnr);
    dict_add_ಸಂಖ್ಯೆ(dict, "winnr", winnr);
    dict_add_ಸಂಖ್ಯೆ(dict, "winid", wp->w_id);
    dict_add_ಸಂಖ್ಯೆ(dict, "height", wp->w_height);
    dict_add_ಸಂಖ್ಯೆ(dict, "status_height", wp->w_status_height);
    dict_add_ಸಂಖ್ಯೆ(dict, "winrow", wp->w_winrow + 1);
    dict_add_ಸಂಖ್ಯೆ(dict, "topline", wp->w_topline);
    dict_add_ಸಂಖ್ಯೆ(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_add_ಸಂಖ್ಯೆ(dict, "winbar", wp->w_winbar_height);
# endif
    dict_add_ಸಂಖ್ಯೆ(dict, "width", wp->w_width);
    dict_add_ಸಂಖ್ಯೆ(dict, "wincol", wp->w_wincol + 1);
    dict_add_ಸಂಖ್ಯೆ(dict, "textoff", win_col_off(wp));
    dict_add_ಸಂಖ್ಯೆ(dict, "bufnr", wp->w_buffer->b_fnum);
    dict_add_ಸಂಖ್ಯೆ(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_add_ಸಂಖ್ಯೆ(dict, "terminal", bt_terminal(wp->w_buffer));
# endif
# ifdef FEAT_QUICKFIX
    dict_add_ಸಂಖ್ಯೆ(dict, "quickfix", bt_quickfix(wp->w_buffer));
    dict_add_ಸಂಖ್ಯೆ(dict, "loclist",
		      (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != ರಿಕ್ತ));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    ಪ್ರತ್ಯಾಗಮ dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    ಸ್ಥಿರ dict_T *
get_tabpage_info(tabpage_T *tp, ಪೂರ್ಣಾಙ್ಕ tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    ಯದಿ (dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    dict_add_ಸಂಖ್ಯೆ(dict, "tabnr", tp_idx);

    l = list_alloc();
    ಯದಿ (l != ರಿಕ್ತ)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_ಸಂಖ್ಯೆ(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    ಪ್ರತ್ಯಾಗಮ dict;
}

/*
 * "gettabinfo()" function
 */
    ಶೂನ್ಯ
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = ರಿಕ್ತ;
    dict_T	*d;
    ಪೂರ್ಣಾಙ್ಕ		tpnr = 0;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ_chk(&argvars[0], ರಿಕ್ತ));
	ಯದಿ (tparg == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	ಯದಿ (tparg != ರಿಕ್ತ && tp != tparg)
	    ಅನುವರ್ತ;
	d = get_tabpage_info(tp, tpnr);
	ಯದಿ (d != ರಿಕ್ತ)
	    list_append_dict(rettv->vval.v_list, d);
	ಯದಿ (tparg != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }
}

/*
 * "getwininfo()" function
 */
    ಶೂನ್ಯ
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = ರಿಕ್ತ, *wparg = ರಿಕ್ತ;
    dict_T	*d;
    short	tabnr = 0, winnr;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
	ಯದಿ (wparg == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
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
	    ಯದಿ (wparg != ರಿಕ್ತ && wp != wparg)
		ಅನುವರ್ತ;
	    d = get_win_info(wp, tabnr, winnr);
	    ಯದಿ (d != ರಿಕ್ತ)
		list_append_dict(rettv->vval.v_list, d);
	    ಯದಿ (wparg != ರಿಕ್ತ)
		// found information about a specific window
		ಪ್ರತ್ಯಾಗಮ;
	}
    }
# ifdef FEAT_PROP_POPUP
    ಯದಿ (wparg != ರಿಕ್ತ)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		ಯದಿ (wp == wparg)
		    ಗಚ್ಛ found;
	}
found:
	d = get_win_info(wparg, tp == ರಿಕ್ತ ? 0 : tabnr, 0);
	ಯದಿ (d != ರಿಕ್ತ)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    ಶೂನ್ಯ
f_getwinpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ x = -1;
    ಪೂರ್ಣಾಙ್ಕ y = -1;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

# ಯದಿ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
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
f_getwinposx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
# ಯದಿ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	ಪೂರ್ಣಾಙ್ಕ	    x, y;

	ಯದಿ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_ಸಂಖ್ಯೆ = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    ಶೂನ್ಯ
f_getwinposy(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
# ಯದಿ defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	ಪೂರ್ಣಾಙ್ಕ	    x, y;

	ಯದಿ (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_ಸಂಖ್ಯೆ = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    ಶೂನ್ಯ
f_tabpagenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		nr = 1;
    char_u	*arg;

    ಯದಿ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	ಯದಿ (arg != ರಿಕ್ತ)
	{
	    ಯದಿ (STRCMP(arg, "$") == 0)
		nr = tabpage_index(ರಿಕ್ತ) - 1;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    ಅನ್ಯಥಾ
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    ಅನ್ಯಥಾ
	nr = tabpage_index(curtab);
    rettv->vval.v_ಸಂಖ್ಯೆ = nr;
}

/*
 * "tabpagewinnr()" function
 */
    ಶೂನ್ಯ
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		nr = 1;
    tabpage_T	*tp;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    tp = find_tabpage((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
    ಯದಿ (tp == ರಿಕ್ತ)
	nr = 0;
    ಅನ್ಯಥಾ
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_ಸಂಖ್ಯೆ = nr;
}

/*
 * "win_execute()" function
 */
    ಶೂನ್ಯ
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    id = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(argvars);
    wp = win_id2wp_tp(id, &tp);
    ಯದಿ (wp == ರಿಕ್ತ || tp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    ಪೂರ್ಣಾಙ್ಕ	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    ಪೂರ್ಣಾಙ್ಕ	apply_acd = FALSE;
    char_u	*save_sfname = ರಿಕ್ತ;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    ಯದಿ (curwin != wp
	    && (curwin->w_localdir != ರಿಕ್ತ
		|| wp->w_localdir != ರಿಕ್ತ
		|| (curtab != tp
		    && (curtab->tp_localdir != ರಿಕ್ತ
			|| tp->tp_localdir != ರಿಕ್ತ))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_status = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    ಯದಿ (cwd_status == OK && p_acd)
    {
	ಯದಿ (curbuf->b_sfname != ರಿಕ್ತ && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    ಯದಿ (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    ಯದಿ (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    ಅನ್ಯಥಾ
# endif
	ಯದಿ (cwd_status == OK)
	{
	    mch_chdir((ವರ್ಣ *)cwd);
# ifdef FEAT_AUTOCHDIR
	    ಯದಿ (save_sfname != ರಿಕ್ತ)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    ಯದಿ (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    ಯದಿ (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    ಶೂನ್ಯ
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    ಶೂನ್ಯ
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script()
	    && (check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL)))
	ಪ್ರತ್ಯಾಗಮ;

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
    ಪೂರ್ಣಾಙ್ಕ		id;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    id = tv_get_ಸಂಖ್ಯೆ(&argvars[0]);
    ಯದಿ (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_ಸಂಖ್ಯೆ = 1;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (text_or_buf_locked())
	ಪ್ರತ್ಯಾಗಮ;
# ಯದಿ defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ಯದಿ (popup_is_popup(curwin) && curbuf->b_term != ರಿಕ್ತ)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	ಯದಿ (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    ಯದಿ (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_ಸಂಖ್ಯೆ = 1;
	    ಪ್ರತ್ಯಾಗಮ;
	}
}

/*
 * "win_id2tabwin()" function
 */
    ಶೂನ್ಯ
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    ಶೂನ್ಯ
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->vval.v_ಸಂಖ್ಯೆ = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    ಶೂನ್ಯ
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		offset;

    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ಯದಿ (wp == ರಿಕ್ತ || win_valid_popup(wp))
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	ಪ್ರತ್ಯಾಗಮ;
    }

    offset = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    win_drag_vsep_ಸಾಲು(wp, offset);
    rettv->vval.v_ಸಂಖ್ಯೆ = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    ಶೂನ್ಯ
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		offset;

    rettv->vval.v_ಸಂಖ್ಯೆ = FALSE;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ಯದಿ (wp == ರಿಕ್ತ || win_valid_popup(wp))
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	ಪ್ರತ್ಯಾಗಮ;
    }

    offset = (ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[1]);
    win_drag_status_ಸಾಲು(wp, offset);
    rettv->vval.v_ಸಂಖ್ಯೆ = TRUE;
}

/*
 * "win_screenpos()" function
 */
    ಶೂನ್ಯ
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, wp == ರಿಕ್ತ ? 0 : wp->w_winrow + 1);
    list_append_ಸಂಖ್ಯೆ(rettv->vval.v_list, wp == ರಿಕ್ತ ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    ಶೂನ್ಯ
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    ಪೂರ್ಣಾಙ್ಕ     flags = 0, size = 0;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL
		|| check_for_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    ಯದಿ (wp == ರಿಕ್ತ || targetwin == ರಿಕ್ತ || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_ಸಂಖ್ಯೆ));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	ಯದಿ (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

	d = argvars[2].vval.v_dict;
	ಯದಿ (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	ಯದಿ ((di = dict_find(d, (char_u *)"rightbelow", -1)) != ರಿಕ್ತ)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    ಯದಿ (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    ಯದಿ (curwin == targetwin && win_valid(wp))
    {
	ಯದಿ (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ಅನ್ಯಥಾ
	emsg(_(e_autocommands_caused_command_to_abort));

    ಯದಿ (oldwin != curwin && win_valid(oldwin))
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
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	ಯದಿ (wp == ರಿಕ್ತ)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಯದಿ (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# ಯದಿ defined(FEAT_QUICKFIX)
    ಅನ್ಯಥಾ ಯದಿ (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    ಅನ್ಯಥಾ ಯದಿ (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    ಅನ್ಯಥಾ ಯದಿ (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    ಅನ್ಯಥಾ ಯದಿ (bt_quickfix(wp->w_buffer))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != ರಿಕ್ತ ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    ಶೂನ್ಯ
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;
    rettv->vval.v_string = alloc(2);
    ಯದಿ (rettv->vval.v_string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

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

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ಯದಿ (wp == ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    ಶೂನ್ಯ
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    ಪೂರ್ಣಾಙ್ಕ col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    ಯದಿ (curwin->w_p_rl)
	col = curwin->w_width - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_ಸಂಖ್ಯೆ = col;
}

/*
 * "winheight(nr)" function
 */
    ಶೂನ್ಯ
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ಯದಿ (wp == ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_height;
}

/*
 * "winlayout()" function
 */
    ಶೂನ್ಯ
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script() && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    ಅನ್ಯಥಾ
    {
	tp = find_tabpage((ಪೂರ್ಣಾಙ್ಕ)tv_get_ಸಂಖ್ಯೆ(&argvars[0]));
	ಯದಿ (tp == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    ಶೂನ್ಯ
f_winline(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_ಸಂಖ್ಯೆ = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    ಶೂನ್ಯ
f_winnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		nr = 1;

    ಯದಿ (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_ಸಂಖ್ಯೆ = nr;
}

/*
 * "winrestcmd()" function
 */
    ಶೂನ್ಯ
f_winrestcmd(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 70);

    // Do this twice to handle some window layouts properly.
    ಪ್ರತಿ (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((ವರ್ಣ *)buf, ಪರಿಮಾಣ(buf),
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
    ಶೂನ್ಯ
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    ಯದಿ (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    dict = argvars[0].vval.v_dict;
    ಯದಿ (dict_has_ಕೀಲಿ(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_ಸಂಖ್ಯೆ(dict, "lnum");
    ಯದಿ (dict_has_ಕೀಲಿ(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_ಸಂಖ್ಯೆ(dict, "col");
    ಯದಿ (dict_has_ಕೀಲಿ(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_ಸಂಖ್ಯೆ(dict, "coladd");
    ಯದಿ (dict_has_ಕೀಲಿ(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_ಸಂಖ್ಯೆ(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    ಯದಿ (dict_has_ಕೀಲಿ(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_ಸಂಖ್ಯೆ(dict, "topline"));
# ifdef FEAT_DIFF
    ಯದಿ (dict_has_ಕೀಲಿ(dict, "topfill"))
	curwin->w_topfill = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(dict, "topfill");
# endif
    ಯದಿ (dict_has_ಕೀಲಿ(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_ಸಂಖ್ಯೆ(dict, "leftcol");
    ಯದಿ (dict_has_ಕೀಲಿ(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_ಸಂಖ್ಯೆ(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    ಯದಿ (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    ಯದಿ (curwin->w_topline > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	curwin->w_topline = curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    ಶೂನ್ಯ
f_winsaveview(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;

    ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    dict = rettv->vval.v_dict;

    dict_add_ಸಂಖ್ಯೆ(dict, "lnum", (long)curwin->w_cursor.lnum);
    dict_add_ಸಂಖ್ಯೆ(dict, "col", (long)curwin->w_cursor.col);
    dict_add_ಸಂಖ್ಯೆ(dict, "coladd", (long)curwin->w_cursor.coladd);
    update_curswant();
    dict_add_ಸಂಖ್ಯೆ(dict, "curswant", (long)curwin->w_curswant);

    dict_add_ಸಂಖ್ಯೆ(dict, "topline", (long)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_add_ಸಂಖ್ಯೆ(dict, "topfill", (long)curwin->w_topfill);
# endif
    dict_add_ಸಂಖ್ಯೆ(dict, "leftcol", (long)curwin->w_leftcol);
    dict_add_ಸಂಖ್ಯೆ(dict, "skipcol", (long)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    ಶೂನ್ಯ
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ಯದಿ (in_vim9script() && check_for_ಸಂಖ್ಯೆ_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ಯದಿ (wp == ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಅನ್ಯಥಾ
	rettv->vval.v_ಸಂಖ್ಯೆ = wp->w_width;
}
#endif // FEAT_EVAL

#ಯದಿ defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    ಪೂರ್ಣಾಙ್ಕ
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	ಪೂರ್ಣಾಙ್ಕ	    no_display)
{
    block_autocmds();
    ಪ್ರತ್ಯಾಗಮ switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    ಪೂರ್ಣಾಙ್ಕ
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	ಪೂರ್ಣಾಙ್ಕ	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    ಯದಿ (win == curwin)
	switchwin->sw_same_win = TRUE;
    ಅನ್ಯಥಾ
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    ಯದಿ (tp != ರಿಕ್ತ)
    {
	switchwin->sw_curtab = curtab;
	ಯದಿ (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	ಅನ್ಯಥಾ
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    ಯದಿ (!win_valid(win))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    ಶೂನ್ಯ
restore_win(
	switchwin_T *switchwin,
	ಪೂರ್ಣಾಙ್ಕ	    no_display)
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
	ಪೂರ್ಣಾಙ್ಕ	    no_display)
{
    ಯದಿ (switchwin->sw_curtab != ರಿಕ್ತ && valid_tabpage(switchwin->sw_curtab))
    {
	ಯದಿ (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	ಅನ್ಯಥಾ
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    ಯದಿ (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    ಯದಿ (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    ಅನ್ಯಥಾ ಯದಿ (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
