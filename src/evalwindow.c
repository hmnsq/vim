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

#ضمن "vim.h"

#𐡄𐡍 defined(FEAT_EVAL)

    ساكن صحيح
win_getid(typval_T *argvars)
{
    صحيح	    winnr;
    win_T   *wp;

    𐡄𐡍 (argvars[0].v_type == VAR_UNKNOWN)
	أرجع curwin->w_id;
    winnr = tv_get_number(&argvars[0]);
    𐡄𐡍 (winnr <= 0)
	أرجع 0;

    𐡄𐡍 (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    وإلا
    {
	tabpage_T	*tp;
	صحيح		tabnr = tv_get_number(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    𐡄𐡍 (--tabnr == 0)
		اكسر;
	𐡄𐡍 (tp == عدم)
	    أرجع -1;
	𐡄𐡍 (tp == curtab)
	    wp = firstwin;
	وإلا
	    wp = tp->tp_firstwin;
    }
    لكل ( ; wp != عدم; wp = wp->w_next)
	𐡄𐡍 (--winnr == 0)
	    أرجع wp->w_id;
    أرجع 0;
}

    ساكن فراغ
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    صحيح		winnr = 1;
    صحيح		tabnr = 1;
    صحيح		id = tv_get_number(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    𐡄𐡍 (wp->w_id == id)
	    {
		list_append_number(list, tabnr);
		list_append_number(list, winnr);
		أرجع;
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
win_id2wp(صحيح id)
{
    أرجع win_id2wp_tp(id, عدم);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(صحيح id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	𐡄𐡍 (wp->w_id == id)
	{
	    𐡄𐡍 (tpp != عدم)
		*tpp = tp;
	    أرجع wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     𐡄𐡍 (wp->w_id == id)
	     {
		 𐡄𐡍 (tpp != عدم)
		     *tpp = tp;
		 أرجع wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	𐡄𐡍 (wp->w_id == id)
	{
	    𐡄𐡍 (tpp != عدم)
		*tpp = curtab;  // any tabpage would do
	    أرجع wp;
	}
# endif

    أرجع عدم;
}

    ساكن صحيح
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    صحيح	    nr = 1;
    صحيح	    id = tv_get_number(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	𐡄𐡍 (wp->w_id == id)
	    أرجع nr;
	++nr;
    }
    أرجع 0;
}

    فراغ
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    صحيح		bufnr = tv_get_number(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    𐡄𐡍 (wp->w_buffer->b_fnum == bufnr)
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
    صحيح		nr = (صحيح)tv_get_number_chk(vp, عدم);

    𐡄𐡍 (nr < 0)
	أرجع عدم;
    𐡄𐡍 (nr == 0)
	أرجع curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	𐡄𐡍 (nr >= LOWEST_WIN_ID)
	{
	    𐡄𐡍 (wp->w_id == nr)
		أرجع wp;
	}
	وإلا 𐡄𐡍 (--nr <= 0)
	    اكسر;
    }
    𐡄𐡍 (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	لكل (wp = (tp == عدم ? curtab : tp)->tp_first_popupwin;
						   wp != عدم; wp = wp->w_next)
	    𐡄𐡍 (wp->w_id == nr)
		أرجع wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    𐡄𐡍 (wp->w_id == nr)
		أرجع wp;
# endif
	أرجع عدم;
    }
    أرجع wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    صحيح	nr = (صحيح)tv_get_number_chk(vp, عدم);

    𐡄𐡍 (nr >= LOWEST_WIN_ID)
	أرجع win_id2wp(tv_get_number(vp));
    أرجع find_win_by_nr(vp, عدم);
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
    win_T	*wp = عدم;
    tabpage_T	*tp = عدم;
    long	n;

    𐡄𐡍 (wvp->v_type != VAR_UNKNOWN)
    {
	𐡄𐡍 (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_number(tvp);
	    𐡄𐡍 (n >= 0)
		tp = find_tabpage(n);
	}
	وإلا
	    tp = curtab;

	𐡄𐡍 (tp != عدم)
	{
	    wp = find_win_by_nr(wvp, tp);
	    𐡄𐡍 (wp == عدم && wvp->v_type == VAR_NUMBER
						&& wvp->vval.v_number != -1)
		// A window with the specified number is not found
		tp = عدم;
	}
    }
    وإلا
    {
	wp = curwin;
	tp = curtab;
    }

    𐡄𐡍 (ptp != عدم)
	*ptp = tp;

    أرجع wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    ساكن فراغ
get_framelayout(frame_T *fr, list_T *l, صحيح outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    𐡄𐡍 (fr == عدم)
	أرجع;

    𐡄𐡍 (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    وإلا
    {
	fr_list = list_alloc();
	𐡄𐡍 (fr_list == عدم)
	    أرجع;
	𐡄𐡍 (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    أرجع;
	}
    }

    𐡄𐡍 (fr->fr_layout == FR_LEAF)
    {
	𐡄𐡍 (fr->fr_win != عدم)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_number(fr_list, fr->fr_win->w_id);
	}
    }
    وإلا
    {
	𐡄𐡍 (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	وإلا
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	𐡄𐡍 (win_list == عدم)
	    أرجع;
	𐡄𐡍 (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    أرجع;
	}

	child = fr->fr_child;
	طالما (child != عدم)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    ساكن صحيح
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    صحيح		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    𐡄𐡍 (argvar->v_type != VAR_UNKNOWN)
    {
	صحيح	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	𐡄𐡍 (arg == عدم)
	    nr = 0;		// type error; errmsg already given
	وإلا 𐡄𐡍 (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	وإلا 𐡄𐡍 (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	وإلا
	{
	    long	count;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    count = strtol((حرف *)arg, (حرف **)&endp, 10);
	    𐡄𐡍 (count <= 0)
		count = 1;	// if count is not specified, default to 1
	    𐡄𐡍 (endp != عدم && *endp != '\0')
	    {
		𐡄𐡍 (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, count);
		وإلا 𐡄𐡍 (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, count);
		وإلا 𐡄𐡍 (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, count);
		وإلا 𐡄𐡍 (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, count);
		وإلا
		    invalid_arg = TRUE;
	    }
	    وإلا
		invalid_arg = TRUE;
	}
	𐡄𐡍 (twin == عدم)
	    nr = 0;

	𐡄𐡍 (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    𐡄𐡍 (nr <= 0)
	أرجع 0;

    لكل (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	𐡄𐡍 (wp == عدم)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    اكسر;
	}
	++nr;
    }
    أرجع nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    ساكن dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    𐡄𐡍 (dict == عدم)
	أرجع عدم;

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
		      (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != عدم));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    أرجع dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    ساكن dict_T *
get_tabpage_info(tabpage_T *tp, صحيح tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    𐡄𐡍 (dict == عدم)
	أرجع عدم;

    dict_add_number(dict, "tabnr", tp_idx);

    l = list_alloc();
    𐡄𐡍 (l != عدم)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_number(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    أرجع dict;
}

/*
 * "gettabinfo()" function
 */
    فراغ
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = عدم;
    dict_T	*d;
    صحيح		tpnr = 0;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    𐡄𐡍 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	أرجع;

    𐡄𐡍 (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((صحيح)tv_get_number_chk(&argvars[0], عدم));
	𐡄𐡍 (tparg == عدم)
	    أرجع;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	𐡄𐡍 (tparg != عدم && tp != tparg)
	    استمر;
	d = get_tabpage_info(tp, tpnr);
	𐡄𐡍 (d != عدم)
	    list_append_dict(rettv->vval.v_list, d);
	𐡄𐡍 (tparg != عدم)
	    أرجع;
    }
}

/*
 * "getwininfo()" function
 */
    فراغ
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = عدم, *wparg = عدم;
    dict_T	*d;
    short	tabnr = 0, winnr;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    𐡄𐡍 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	أرجع;

    𐡄𐡍 (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_number(&argvars[0]));
	𐡄𐡍 (wparg == عدم)
	    أرجع;
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
	    𐡄𐡍 (wparg != عدم && wp != wparg)
		استمر;
	    d = get_win_info(wp, tabnr, winnr);
	    𐡄𐡍 (d != عدم)
		list_append_dict(rettv->vval.v_list, d);
	    𐡄𐡍 (wparg != عدم)
		// found information about a specific window
		أرجع;
	}
    }
# ifdef FEAT_PROP_POPUP
    𐡄𐡍 (wparg != عدم)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		𐡄𐡍 (wp == wparg)
		    اذهب found;
	}
found:
	d = get_win_info(wparg, tp == عدم ? 0 : tabnr, 0);
	𐡄𐡍 (d != عدم)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    فراغ
f_getwinpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحيح x = -1;
    صحيح y = -1;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    𐡄𐡍 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	أرجع;

# 𐡄𐡍 defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	𐡄𐡍 (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_number(&argvars[0]);

	(فراغ)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_number(rettv->vval.v_list, (varnumber_T)x);
    list_append_number(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    فراغ
f_getwinposx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = -1;
# 𐡄𐡍 defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	صحيح	    x, y;

	𐡄𐡍 (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_number = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    فراغ
f_getwinposy(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_number = -1;
# 𐡄𐡍 defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	صحيح	    x, y;

	𐡄𐡍 (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_number = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    فراغ
f_tabpagenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحيح		nr = 1;
    char_u	*arg;

    𐡄𐡍 (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	أرجع;

    𐡄𐡍 (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	𐡄𐡍 (arg != عدم)
	{
	    𐡄𐡍 (STRCMP(arg, "$") == 0)
		nr = tabpage_index(عدم) - 1;
	    وإلا 𐡄𐡍 (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    وإلا
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    وإلا
	nr = tabpage_index(curtab);
    rettv->vval.v_number = nr;
}

/*
 * "tabpagewinnr()" function
 */
    فراغ
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحيح		nr = 1;
    tabpage_T	*tp;

    𐡄𐡍 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    tp = find_tabpage((صحيح)tv_get_number(&argvars[0]));
    𐡄𐡍 (tp == عدم)
	nr = 0;
    وإلا
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_number = nr;
}

/*
 * "win_execute()" function
 */
    فراغ
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    صحيح		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    𐡄𐡍 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	أرجع;

    id = (صحيح)tv_get_number(argvars);
    wp = win_id2wp_tp(id, &tp);
    𐡄𐡍 (wp == عدم || tp == عدم)
	أرجع;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    صحيح	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    صحيح	apply_acd = FALSE;
    char_u	*save_sfname = عدم;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    𐡄𐡍 (curwin != wp
	    && (curwin->w_localdir != عدم
		|| wp->w_localdir != عدم
		|| (curtab != tp
		    && (curtab->tp_localdir != عدم
			|| tp->tp_localdir != عدم))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_status = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    𐡄𐡍 (cwd_status == OK && p_acd)
    {
	𐡄𐡍 (curbuf->b_sfname != عدم && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    𐡄𐡍 (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    𐡄𐡍 (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    وإلا
# endif
	𐡄𐡍 (cwd_status == OK)
	{
	    mch_chdir((حرف *)cwd);
# ifdef FEAT_AUTOCHDIR
	    𐡄𐡍 (save_sfname != عدم)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    𐡄𐡍 (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    𐡄𐡍 (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    فراغ
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	أرجع;

    𐡄𐡍 (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    فراغ
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	أرجع;

    rettv->vval.v_number = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    فراغ
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    صحيح		id;

    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	أرجع;

    id = tv_get_number(&argvars[0]);
    𐡄𐡍 (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_number = 1;
	أرجع;
    }

    𐡄𐡍 (text_or_buf_locked())
	أرجع;
# 𐡄𐡍 defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    𐡄𐡍 (popup_is_popup(curwin) && curbuf->b_term != عدم)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	أرجع;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	𐡄𐡍 (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    𐡄𐡍 (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_number = 1;
	    أرجع;
	}
}

/*
 * "win_id2tabwin()" function
 */
    فراغ
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	أرجع;

    𐡄𐡍 (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    فراغ
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_number = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    فراغ
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحيح		offset;

    rettv->vval.v_number = FALSE;

    𐡄𐡍 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	أرجع;

    wp = find_win_by_nr_or_id(&argvars[0]);
    𐡄𐡍 (wp == عدم || win_valid_popup(wp))
	أرجع;
    𐡄𐡍 (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	أرجع;
    }

    offset = (صحيح)tv_get_number(&argvars[1]);
    win_drag_vsep_line(wp, offset);
    rettv->vval.v_number = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    فراغ
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحيح		offset;

    rettv->vval.v_number = FALSE;

    𐡄𐡍 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	أرجع;

    wp = find_win_by_nr_or_id(&argvars[0]);
    𐡄𐡍 (wp == عدم || win_valid_popup(wp))
	أرجع;
    𐡄𐡍 (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	أرجع;
    }

    offset = (صحيح)tv_get_number(&argvars[1]);
    win_drag_status_line(wp, offset);
    rettv->vval.v_number = TRUE;
}

/*
 * "win_screenpos()" function
 */
    فراغ
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	أرجع;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_number(rettv->vval.v_list, wp == عدم ? 0 : wp->w_winrow + 1);
    list_append_number(rettv->vval.v_list, wp == عدم ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    فراغ
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    صحيح     flags = 0, size = 0;

    rettv->vval.v_number = -1;

    𐡄𐡍 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    𐡄𐡍 (wp == عدم || targetwin == عدم || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_number));
	أرجع;
    }

    𐡄𐡍 (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	𐡄𐡍 (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    أرجع;

	d = argvars[2].vval.v_dict;
	𐡄𐡍 (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	𐡄𐡍 ((di = dict_find(d, (char_u *)"rightbelow", -1)) != عدم)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (صحيح)dict_get_number(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    𐡄𐡍 (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	أرجع;

    𐡄𐡍 (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    𐡄𐡍 (curwin == targetwin && win_valid(wp))
    {
	𐡄𐡍 (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_number = 0;
    }
    وإلا
	emsg(_(e_autocommands_caused_command_to_abort));

    𐡄𐡍 (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    فراغ
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    𐡄𐡍 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	أرجع;

    𐡄𐡍 (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	𐡄𐡍 (wp == عدم)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    أرجع;
	}
    }
    𐡄𐡍 (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# 𐡄𐡍 defined(FEAT_QUICKFIX)
    وإلا 𐡄𐡍 (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    وإلا 𐡄𐡍 (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    وإلا 𐡄𐡍 (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    وإلا 𐡄𐡍 (bt_quickfix(wp->w_buffer))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != عدم ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    فراغ
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
    rettv->vval.v_string = alloc(2);
    𐡄𐡍 (rettv->vval.v_string == عدم)
	أرجع;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    فراغ
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	أرجع;

    wp = find_win_by_nr_or_id(&argvars[0]);
    𐡄𐡍 (wp == عدم)
	rettv->vval.v_number = -1;
    وإلا
	rettv->vval.v_number = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    فراغ
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    صحيح col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    𐡄𐡍 (curwin->w_p_rl)
	col = curwin->w_width - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_number = col;
}

/*
 * "winheight(nr)" function
 */
    فراغ
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	أرجع;

    wp = find_win_by_nr_or_id(&argvars[0]);
    𐡄𐡍 (wp == عدم)
	rettv->vval.v_number = -1;
    وإلا
	rettv->vval.v_number = wp->w_height;
}

/*
 * "winlayout()" function
 */
    فراغ
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    𐡄𐡍 (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    𐡄𐡍 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	أرجع;

    𐡄𐡍 (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    وإلا
    {
	tp = find_tabpage((صحيح)tv_get_number(&argvars[0]));
	𐡄𐡍 (tp == عدم)
	    أرجع;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    فراغ
f_winline(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_number = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    فراغ
f_winnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    صحيح		nr = 1;

    𐡄𐡍 (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	أرجع;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_number = nr;
}

/*
 * "winrestcmd()" function
 */
    فراغ
f_winrestcmd(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    صحيح		i;
    صحيح		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, حجم(حرف), 70);

    // Do this twice to handle some window layouts properly.
    لكل (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
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
    فراغ
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    𐡄𐡍 (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	أرجع;

    dict = argvars[0].vval.v_dict;
    𐡄𐡍 (dict_has_key(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_number(dict, "lnum");
    𐡄𐡍 (dict_has_key(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_number(dict, "col");
    𐡄𐡍 (dict_has_key(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_number(dict, "coladd");
    𐡄𐡍 (dict_has_key(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_number(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    𐡄𐡍 (dict_has_key(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_number(dict, "topline"));
# ifdef FEAT_DIFF
    𐡄𐡍 (dict_has_key(dict, "topfill"))
	curwin->w_topfill = (صحيح)dict_get_number(dict, "topfill");
# endif
    𐡄𐡍 (dict_has_key(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_number(dict, "leftcol");
    𐡄𐡍 (dict_has_key(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_number(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    𐡄𐡍 (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    𐡄𐡍 (curwin->w_topline > curbuf->b_ml.ml_line_count)
	curwin->w_topline = curbuf->b_ml.ml_line_count;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    فراغ
f_winsaveview(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;

    𐡄𐡍 (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
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
    فراغ
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    𐡄𐡍 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	أرجع;

    wp = find_win_by_nr_or_id(&argvars[0]);
    𐡄𐡍 (wp == عدم)
	rettv->vval.v_number = -1;
    وإلا
	rettv->vval.v_number = wp->w_width;
}
#endif // FEAT_EVAL

#𐡄𐡍 defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    صحيح
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	صحيح	    no_display)
{
    block_autocmds();
    أرجع switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    صحيح
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	صحيح	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    𐡄𐡍 (win == curwin)
	switchwin->sw_same_win = TRUE;
    وإلا
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    𐡄𐡍 (tp != عدم)
    {
	switchwin->sw_curtab = curtab;
	𐡄𐡍 (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	وإلا
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    𐡄𐡍 (!win_valid(win))
	أرجع FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    أرجع OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    فراغ
restore_win(
	switchwin_T *switchwin,
	صحيح	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    فراغ
restore_win_noblock(
	switchwin_T *switchwin,
	صحيح	    no_display)
{
    𐡄𐡍 (switchwin->sw_curtab != عدم && valid_tabpage(switchwin->sw_curtab))
    {
	𐡄𐡍 (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	وإلا
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    𐡄𐡍 (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    𐡄𐡍 (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    وإلا 𐡄𐡍 (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
