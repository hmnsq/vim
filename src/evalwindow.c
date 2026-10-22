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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

    நிலையான முழு
win_getid(typval_T *argvars)
{
    முழு	    winnr;
    win_T   *wp;

    எனில் (argvars[0].v_type == VAR_UNKNOWN)
	திரும்பு curwin->w_id;
    winnr = tv_get_எண்(&argvars[0]);
    எனில் (winnr <= 0)
	திரும்பு 0;

    எனில் (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    இல்லையெனில்
    {
	tabpage_T	*tp;
	முழு		tabnr = tv_get_எண்(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    எனில் (--tabnr == 0)
		நிறுத்து;
	எனில் (tp == வெற்று)
	    திரும்பு -1;
	எனில் (tp == curtab)
	    wp = firstwin;
	இல்லையெனில்
	    wp = tp->tp_firstwin;
    }
    க்கு ( ; wp != வெற்று; wp = wp->w_next)
	எனில் (--winnr == 0)
	    திரும்பு wp->w_id;
    திரும்பு 0;
}

    நிலையான வெறுமை
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    முழு		winnr = 1;
    முழு		tabnr = 1;
    முழு		id = tv_get_எண்(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    எனில் (wp->w_id == id)
	    {
		list_append_எண்(list, tabnr);
		list_append_எண்(list, winnr);
		திரும்பு;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_எண்(list, 0);
    list_append_எண்(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(முழு id)
{
    திரும்பு win_id2wp_tp(id, வெற்று);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(முழு id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	எனில் (wp->w_id == id)
	{
	    எனில் (tpp != வெற்று)
		*tpp = tp;
	    திரும்பு wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     எனில் (wp->w_id == id)
	     {
		 எனில் (tpp != வெற்று)
		     *tpp = tp;
		 திரும்பு wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	எனில் (wp->w_id == id)
	{
	    எனில் (tpp != வெற்று)
		*tpp = curtab;  // any tabpage would do
	    திரும்பு wp;
	}
# endif

    திரும்பு வெற்று;
}

    நிலையான முழு
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    முழு	    nr = 1;
    முழு	    id = tv_get_எண்(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	எனில் (wp->w_id == id)
	    திரும்பு nr;
	++nr;
    }
    திரும்பு 0;
}

    வெறுமை
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    முழு		bufnr = tv_get_எண்(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    எனில் (wp->w_buffer->b_fnum == bufnr)
		list_append_எண்(list, wp->w_id);
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
    முழு		nr = (முழு)tv_get_எண்_chk(vp, வெற்று);

    எனில் (nr < 0)
	திரும்பு வெற்று;
    எனில் (nr == 0)
	திரும்பு curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	எனில் (nr >= LOWEST_WIN_ID)
	{
	    எனில் (wp->w_id == nr)
		திரும்பு wp;
	}
	இல்லையெனில் எனில் (--nr <= 0)
	    நிறுத்து;
    }
    எனில் (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	க்கு (wp = (tp == வெற்று ? curtab : tp)->tp_first_popupwin;
						   wp != வெற்று; wp = wp->w_next)
	    எனில் (wp->w_id == nr)
		திரும்பு wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    எனில் (wp->w_id == nr)
		திரும்பு wp;
# endif
	திரும்பு வெற்று;
    }
    திரும்பு wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    முழு	nr = (முழு)tv_get_எண்_chk(vp, வெற்று);

    எனில் (nr >= LOWEST_WIN_ID)
	திரும்பு win_id2wp(tv_get_எண்(vp));
    திரும்பு find_win_by_nr(vp, வெற்று);
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
    win_T	*wp = வெற்று;
    tabpage_T	*tp = வெற்று;
    long	n;

    எனில் (wvp->v_type != VAR_UNKNOWN)
    {
	எனில் (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_எண்(tvp);
	    எனில் (n >= 0)
		tp = find_tabpage(n);
	}
	இல்லையெனில்
	    tp = curtab;

	எனில் (tp != வெற்று)
	{
	    wp = find_win_by_nr(wvp, tp);
	    எனில் (wp == வெற்று && wvp->v_type == VAR_எண்
						&& wvp->vval.v_எண் != -1)
		// A window with the specified number is not found
		tp = வெற்று;
	}
    }
    இல்லையெனில்
    {
	wp = curwin;
	tp = curtab;
    }

    எனில் (ptp != வெற்று)
	*ptp = tp;

    திரும்பு wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    நிலையான வெறுமை
get_framelayout(frame_T *fr, list_T *l, முழு outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    எனில் (fr == வெற்று)
	திரும்பு;

    எனில் (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    இல்லையெனில்
    {
	fr_list = list_alloc();
	எனில் (fr_list == வெற்று)
	    திரும்பு;
	எனில் (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    திரும்பு;
	}
    }

    எனில் (fr->fr_layout == FR_LEAF)
    {
	எனில் (fr->fr_win != வெற்று)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_எண்(fr_list, fr->fr_win->w_id);
	}
    }
    இல்லையெனில்
    {
	எனில் (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	இல்லையெனில்
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	எனில் (win_list == வெற்று)
	    திரும்பு;
	எனில் (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    திரும்பு;
	}

	child = fr->fr_child;
	போது (child != வெற்று)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    நிலையான முழு
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    முழு		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    எனில் (argvar->v_type != VAR_UNKNOWN)
    {
	முழு	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	எனில் (arg == வெற்று)
	    nr = 0;		// type error; errmsg already given
	இல்லையெனில் எனில் (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	இல்லையெனில் எனில் (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	இல்லையெனில்
	{
	    long	மொத்தம்;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    மொத்தம் = strtol((எழுத்து *)arg, (எழுத்து **)&endp, 10);
	    எனில் (மொத்தம் <= 0)
		மொத்தம் = 1;	// if count is not specified, default to 1
	    எனில் (endp != வெற்று && *endp != '\0')
	    {
		எனில் (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, மொத்தம்);
		இல்லையெனில் எனில் (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, மொத்தம்);
		இல்லையெனில் எனில் (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, மொத்தம்);
		இல்லையெனில் எனில் (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, மொத்தம்);
		இல்லையெனில்
		    invalid_arg = TRUE;
	    }
	    இல்லையெனில்
		invalid_arg = TRUE;
	}
	எனில் (twin == வெற்று)
	    nr = 0;

	எனில் (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    எனில் (nr <= 0)
	திரும்பு 0;

    க்கு (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	எனில் (wp == வெற்று)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    நிறுத்து;
	}
	++nr;
    }
    திரும்பு nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    நிலையான dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    எனில் (dict == வெற்று)
	திரும்பு வெற்று;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_add_எண்(dict, "tabnr", tpnr);
    dict_add_எண்(dict, "winnr", winnr);
    dict_add_எண்(dict, "winid", wp->w_id);
    dict_add_எண்(dict, "height", wp->w_height);
    dict_add_எண்(dict, "status_height", wp->w_status_height);
    dict_add_எண்(dict, "winrow", wp->w_winrow + 1);
    dict_add_எண்(dict, "topline", wp->w_topline);
    dict_add_எண்(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_add_எண்(dict, "winbar", wp->w_winbar_height);
# endif
    dict_add_எண்(dict, "width", wp->w_width);
    dict_add_எண்(dict, "wincol", wp->w_wincol + 1);
    dict_add_எண்(dict, "textoff", win_col_off(wp));
    dict_add_எண்(dict, "bufnr", wp->w_buffer->b_fnum);
    dict_add_எண்(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_add_எண்(dict, "terminal", bt_terminal(wp->w_buffer));
# endif
# ifdef FEAT_QUICKFIX
    dict_add_எண்(dict, "quickfix", bt_quickfix(wp->w_buffer));
    dict_add_எண்(dict, "loclist",
		      (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != வெற்று));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    திரும்பு dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    நிலையான dict_T *
get_tabpage_info(tabpage_T *tp, முழு tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    எனில் (dict == வெற்று)
	திரும்பு வெற்று;

    dict_add_எண்(dict, "tabnr", tp_idx);

    l = list_alloc();
    எனில் (l != வெற்று)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_எண்(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    திரும்பு dict;
}

/*
 * "gettabinfo()" function
 */
    வெறுமை
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = வெற்று;
    dict_T	*d;
    முழு		tpnr = 0;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((முழு)tv_get_எண்_chk(&argvars[0], வெற்று));
	எனில் (tparg == வெற்று)
	    திரும்பு;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	எனில் (tparg != வெற்று && tp != tparg)
	    தொடர்;
	d = get_tabpage_info(tp, tpnr);
	எனில் (d != வெற்று)
	    list_append_dict(rettv->vval.v_list, d);
	எனில் (tparg != வெற்று)
	    திரும்பு;
    }
}

/*
 * "getwininfo()" function
 */
    வெறுமை
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = வெற்று, *wparg = வெற்று;
    dict_T	*d;
    short	tabnr = 0, winnr;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_எண்(&argvars[0]));
	எனில் (wparg == வெற்று)
	    திரும்பு;
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
	    எனில் (wparg != வெற்று && wp != wparg)
		தொடர்;
	    d = get_win_info(wp, tabnr, winnr);
	    எனில் (d != வெற்று)
		list_append_dict(rettv->vval.v_list, d);
	    எனில் (wparg != வெற்று)
		// found information about a specific window
		திரும்பு;
	}
    }
# ifdef FEAT_PROP_POPUP
    எனில் (wparg != வெற்று)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		எனில் (wp == wparg)
		    செல் found;
	}
found:
	d = get_win_info(wparg, tp == வெற்று ? 0 : tabnr, 0);
	எனில் (d != வெற்று)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    வெறுமை
f_getwinpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    முழு x = -1;
    முழு y = -1;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

# எனில் defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	எனில் (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_எண்(&argvars[0]);

	(வெறுமை)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_எண்(rettv->vval.v_list, (varnumber_T)x);
    list_append_எண்(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    வெறுமை
f_getwinposx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_எண் = -1;
# எனில் defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	முழு	    x, y;

	எனில் (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_எண் = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    வெறுமை
f_getwinposy(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_எண் = -1;
# எனில் defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	முழு	    x, y;

	எனில் (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_எண் = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    வெறுமை
f_tabpagenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    முழு		nr = 1;
    char_u	*arg;

    எனில் (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	எனில் (arg != வெற்று)
	{
	    எனில் (STRCMP(arg, "$") == 0)
		nr = tabpage_index(வெற்று) - 1;
	    இல்லையெனில் எனில் (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    இல்லையெனில்
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    இல்லையெனில்
	nr = tabpage_index(curtab);
    rettv->vval.v_எண் = nr;
}

/*
 * "tabpagewinnr()" function
 */
    வெறுமை
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    முழு		nr = 1;
    tabpage_T	*tp;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    tp = find_tabpage((முழு)tv_get_எண்(&argvars[0]));
    எனில் (tp == வெற்று)
	nr = 0;
    இல்லையெனில்
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_எண் = nr;
}

/*
 * "win_execute()" function
 */
    வெறுமை
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    முழு		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	திரும்பு;

    id = (முழு)tv_get_எண்(argvars);
    wp = win_id2wp_tp(id, &tp);
    எனில் (wp == வெற்று || tp == வெற்று)
	திரும்பு;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    முழு	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    முழு	apply_acd = FALSE;
    char_u	*save_sfname = வெற்று;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    எனில் (curwin != wp
	    && (curwin->w_localdir != வெற்று
		|| wp->w_localdir != வெற்று
		|| (curtab != tp
		    && (curtab->tp_localdir != வெற்று
			|| tp->tp_localdir != வெற்று))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_status = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    எனில் (cwd_status == OK && p_acd)
    {
	எனில் (curbuf->b_sfname != வெற்று && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    எனில் (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    எனில் (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    இல்லையெனில்
# endif
	எனில் (cwd_status == OK)
	{
	    mch_chdir((எழுத்து *)cwd);
# ifdef FEAT_AUTOCHDIR
	    எனில் (save_sfname != வெற்று)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    எனில் (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    எனில் (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    வெறுமை
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    வெறுமை
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script()
	    && (check_for_opt_எண்_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_எண்_arg(argvars, 1) == FAIL)))
	திரும்பு;

    rettv->vval.v_எண் = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    வெறுமை
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    முழு		id;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    id = tv_get_எண்(&argvars[0]);
    எனில் (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_எண் = 1;
	திரும்பு;
    }

    எனில் (text_or_buf_locked())
	திரும்பு;
# எனில் defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    எனில் (popup_is_popup(curwin) && curbuf->b_term != வெற்று)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	திரும்பு;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	எனில் (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    எனில் (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_எண் = 1;
	    திரும்பு;
	}
}

/*
 * "win_id2tabwin()" function
 */
    வெறுமை
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    வெறுமை
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    rettv->vval.v_எண் = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    வெறுமை
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    முழு		offset;

    rettv->vval.v_எண் = FALSE;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    wp = find_win_by_nr_or_id(&argvars[0]);
    எனில் (wp == வெற்று || win_valid_popup(wp))
	திரும்பு;
    எனில் (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	திரும்பு;
    }

    offset = (முழு)tv_get_எண்(&argvars[1]);
    win_drag_vsep_வரி(wp, offset);
    rettv->vval.v_எண் = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    வெறுமை
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    முழு		offset;

    rettv->vval.v_எண் = FALSE;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL))
	திரும்பு;

    wp = find_win_by_nr_or_id(&argvars[0]);
    எனில் (wp == வெற்று || win_valid_popup(wp))
	திரும்பு;
    எனில் (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	திரும்பு;
    }

    offset = (முழு)tv_get_எண்(&argvars[1]);
    win_drag_status_வரி(wp, offset);
    rettv->vval.v_எண் = TRUE;
}

/*
 * "win_screenpos()" function
 */
    வெறுமை
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_எண்(rettv->vval.v_list, wp == வெற்று ? 0 : wp->w_winrow + 1);
    list_append_எண்(rettv->vval.v_list, wp == வெற்று ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    வெறுமை
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    முழு     flags = 0, size = 0;

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_எண்_arg(argvars, 0) == FAIL
		|| check_for_எண்_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	திரும்பு;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    எனில் (wp == வெற்று || targetwin == வெற்று || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_எண்));
	திரும்பு;
    }

    எனில் (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	எனில் (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    திரும்பு;

	d = argvars[2].vval.v_dict;
	எனில் (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	எனில் ((di = dict_find(d, (char_u *)"rightbelow", -1)) != வெற்று)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (முழு)dict_get_எண்(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    எனில் (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	திரும்பு;

    எனில் (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    எனில் (curwin == targetwin && win_valid(wp))
    {
	எனில் (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_எண் = 0;
    }
    இல்லையெனில்
	emsg(_(e_autocommands_caused_command_to_abort));

    எனில் (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    வெறுமை
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	எனில் (wp == வெற்று)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    திரும்பு;
	}
    }
    எனில் (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# எனில் defined(FEAT_QUICKFIX)
    இல்லையெனில் எனில் (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    இல்லையெனில் எனில் (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    இல்லையெனில் எனில் (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    இல்லையெனில் எனில் (bt_quickfix(wp->w_buffer))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != வெற்று ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    வெறுமை
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;
    rettv->vval.v_string = alloc(2);
    எனில் (rettv->vval.v_string == வெற்று)
	திரும்பு;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    வெறுமை
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    wp = find_win_by_nr_or_id(&argvars[0]);
    எனில் (wp == வெற்று)
	rettv->vval.v_எண் = -1;
    இல்லையெனில்
	rettv->vval.v_எண் = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    வெறுமை
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    முழு col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    எனில் (curwin->w_p_rl)
	col = curwin->w_width - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_எண் = col;
}

/*
 * "winheight(nr)" function
 */
    வெறுமை
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    wp = find_win_by_nr_or_id(&argvars[0]);
    எனில் (wp == வெற்று)
	rettv->vval.v_எண் = -1;
    இல்லையெனில்
	rettv->vval.v_எண் = wp->w_height;
}

/*
 * "winlayout()" function
 */
    வெறுமை
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (in_vim9script() && check_for_opt_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    எனில் (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    இல்லையெனில்
    {
	tp = find_tabpage((முழு)tv_get_எண்(&argvars[0]));
	எனில் (tp == வெற்று)
	    திரும்பு;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    வெறுமை
f_winline(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_எண் = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    வெறுமை
f_winnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    முழு		nr = 1;

    எனில் (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	திரும்பு;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_எண் = nr;
}

/*
 * "winrestcmd()" function
 */
    வெறுமை
f_winrestcmd(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    முழு		i;
    முழு		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, அளவு(எழுத்து), 70);

    // Do this twice to handle some window layouts properly.
    க்கு (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((எழுத்து *)buf, அளவு(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((எழுத்து *)buf, அளவு(buf),
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
    வெறுமை
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    எனில் (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	திரும்பு;

    dict = argvars[0].vval.v_dict;
    எனில் (dict_has_சாவி(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_எண்(dict, "lnum");
    எனில் (dict_has_சாவி(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_எண்(dict, "col");
    எனில் (dict_has_சாவி(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_எண்(dict, "coladd");
    எனில் (dict_has_சாவி(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_எண்(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    எனில் (dict_has_சாவி(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_எண்(dict, "topline"));
# ifdef FEAT_DIFF
    எனில் (dict_has_சாவி(dict, "topfill"))
	curwin->w_topfill = (முழு)dict_get_எண்(dict, "topfill");
# endif
    எனில் (dict_has_சாவி(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_எண்(dict, "leftcol");
    எனில் (dict_has_சாவி(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_எண்(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    எனில் (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    எனில் (curwin->w_topline > curbuf->b_ml.ml_வரி_மொத்தம்)
	curwin->w_topline = curbuf->b_ml.ml_வரி_மொத்தம்;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    வெறுமை
f_winsaveview(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;

    எனில் (rettv_dict_alloc(rettv) == FAIL)
	திரும்பு;
    dict = rettv->vval.v_dict;

    dict_add_எண்(dict, "lnum", (long)curwin->w_cursor.lnum);
    dict_add_எண்(dict, "col", (long)curwin->w_cursor.col);
    dict_add_எண்(dict, "coladd", (long)curwin->w_cursor.coladd);
    update_curswant();
    dict_add_எண்(dict, "curswant", (long)curwin->w_curswant);

    dict_add_எண்(dict, "topline", (long)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_add_எண்(dict, "topfill", (long)curwin->w_topfill);
# endif
    dict_add_எண்(dict, "leftcol", (long)curwin->w_leftcol);
    dict_add_எண்(dict, "skipcol", (long)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    வெறுமை
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    எனில் (in_vim9script() && check_for_எண்_arg(argvars, 0) == FAIL)
	திரும்பு;

    wp = find_win_by_nr_or_id(&argvars[0]);
    எனில் (wp == வெற்று)
	rettv->vval.v_எண் = -1;
    இல்லையெனில்
	rettv->vval.v_எண் = wp->w_width;
}
#endif // FEAT_EVAL

#எனில் defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    முழு
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	முழு	    no_display)
{
    block_autocmds();
    திரும்பு switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    முழு
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	முழு	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    எனில் (win == curwin)
	switchwin->sw_same_win = TRUE;
    இல்லையெனில்
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    எனில் (tp != வெற்று)
    {
	switchwin->sw_curtab = curtab;
	எனில் (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	இல்லையெனில்
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    எனில் (!win_valid(win))
	திரும்பு FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    திரும்பு OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    வெறுமை
restore_win(
	switchwin_T *switchwin,
	முழு	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    வெறுமை
restore_win_noblock(
	switchwin_T *switchwin,
	முழு	    no_display)
{
    எனில் (switchwin->sw_curtab != வெற்று && valid_tabpage(switchwin->sw_curtab))
    {
	எனில் (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	இல்லையெனில்
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    எனில் (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    எனில் (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    இல்லையெனில் எனில் (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
