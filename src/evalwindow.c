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

#もし defined(FEAT_EVAL)

    static 整数
win_getid(typval_T *argvars)
{
    整数	    winnr;
    win_T   *wp;

    もし (argvars[0].v_type == VAR_UNKNOWN)
	戻る curwin->w_id;
    winnr = tv_get_number(&argvars[0]);
    もし (winnr <= 0)
	戻る 0;

    もし (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    否则
    {
	tabpage_T	*tp;
	整数		tabnr = tv_get_number(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    もし (--tabnr == 0)
		break;
	もし (tp == NULL)
	    戻る -1;
	もし (tp == curtab)
	    wp = firstwin;
	否则
	    wp = tp->tp_firstwin;
    }
    ために ( ; wp != NULL; wp = wp->w_next)
	もし (--winnr == 0)
	    戻る wp->w_id;
    戻る 0;
}

    static void
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    整数		winnr = 1;
    整数		tabnr = 1;
    整数		id = tv_get_number(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    もし (wp->w_id == id)
	    {
		list_append_number(list, tabnr);
		list_append_number(list, winnr);
		戻る;
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
win_id2wp(整数 id)
{
    戻る win_id2wp_tp(id, NULL);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(整数 id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	もし (wp->w_id == id)
	{
	    もし (tpp != NULL)
		*tpp = tp;
	    戻る wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     もし (wp->w_id == id)
	     {
		 もし (tpp != NULL)
		     *tpp = tp;
		 戻る wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	もし (wp->w_id == id)
	{
	    もし (tpp != NULL)
		*tpp = curtab;  // any tabpage would do
	    戻る wp;
	}
# endif

    戻る NULL;
}

    static 整数
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    整数	    nr = 1;
    整数	    id = tv_get_number(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	もし (wp->w_id == id)
	    戻る nr;
	++nr;
    }
    戻る 0;
}

    void
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    整数		bufnr = tv_get_number(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    もし (wp->w_buffer->b_fnum == bufnr)
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
    整数		nr = (整数)tv_get_number_chk(vp, NULL);

    もし (nr < 0)
	戻る NULL;
    もし (nr == 0)
	戻る curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	もし (nr >= LOWEST_WIN_ID)
	{
	    もし (wp->w_id == nr)
		戻る wp;
	}
	否则 もし (--nr <= 0)
	    break;
    }
    もし (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	ために (wp = (tp == NULL ? curtab : tp)->tp_first_popupwin;
						   wp != NULL; wp = wp->w_next)
	    もし (wp->w_id == nr)
		戻る wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    もし (wp->w_id == nr)
		戻る wp;
# endif
	戻る NULL;
    }
    戻る wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    整数	nr = (整数)tv_get_number_chk(vp, NULL);

    もし (nr >= LOWEST_WIN_ID)
	戻る win_id2wp(tv_get_number(vp));
    戻る find_win_by_nr(vp, NULL);
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

    もし (wvp->v_type != VAR_UNKNOWN)
    {
	もし (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_number(tvp);
	    もし (n >= 0)
		tp = find_tabpage(n);
	}
	否则
	    tp = curtab;

	もし (tp != NULL)
	{
	    wp = find_win_by_nr(wvp, tp);
	    もし (wp == NULL && wvp->v_type == VAR_NUMBER
						&& wvp->vval.v_number != -1)
		// A window with the specified number is not found
		tp = NULL;
	}
    }
    否则
    {
	wp = curwin;
	tp = curtab;
    }

    もし (ptp != NULL)
	*ptp = tp;

    戻る wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    static void
get_framelayout(frame_T *fr, list_T *l, 整数 outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    もし (fr == NULL)
	戻る;

    もし (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    否则
    {
	fr_list = list_alloc();
	もし (fr_list == NULL)
	    戻る;
	もし (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    戻る;
	}
    }

    もし (fr->fr_layout == FR_LEAF)
    {
	もし (fr->fr_win != NULL)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_number(fr_list, fr->fr_win->w_id);
	}
    }
    否则
    {
	もし (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	否则
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	もし (win_list == NULL)
	    戻る;
	もし (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    戻る;
	}

	child = fr->fr_child;
	ながら (child != NULL)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    static 整数
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    整数		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    もし (argvar->v_type != VAR_UNKNOWN)
    {
	整数	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	もし (arg == NULL)
	    nr = 0;		// type error; errmsg already given
	否则 もし (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	否则 もし (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	否则
	{
	    long	count;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    count = strtol((char *)arg, (char **)&endp, 10);
	    もし (count <= 0)
		count = 1;	// if count is not specified, default to 1
	    もし (endp != NULL && *endp != '\0')
	    {
		もし (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, count);
		否则 もし (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, count);
		否则 もし (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, count);
		否则 もし (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, count);
		否则
		    invalid_arg = TRUE;
	    }
	    否则
		invalid_arg = TRUE;
	}
	もし (twin == NULL)
	    nr = 0;

	もし (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    もし (nr <= 0)
	戻る 0;

    ために (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	もし (wp == NULL)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    break;
	}
	++nr;
    }
    戻る nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    static dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    もし (dict == NULL)
	戻る NULL;

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

    戻る dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    static dict_T *
get_tabpage_info(tabpage_T *tp, 整数 tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    もし (dict == NULL)
	戻る NULL;

    dict_add_number(dict, "tabnr", tp_idx);

    l = list_alloc();
    もし (l != NULL)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_number(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    戻る dict;
}

/*
 * "gettabinfo()" function
 */
    void
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = NULL;
    dict_T	*d;
    整数		tpnr = 0;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((整数)tv_get_number_chk(&argvars[0], NULL));
	もし (tparg == NULL)
	    戻る;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	もし (tparg != NULL && tp != tparg)
	    continue;
	d = get_tabpage_info(tp, tpnr);
	もし (d != NULL)
	    list_append_dict(rettv->vval.v_list, d);
	もし (tparg != NULL)
	    戻る;
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

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_number(&argvars[0]));
	もし (wparg == NULL)
	    戻る;
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
	    もし (wparg != NULL && wp != wparg)
		continue;
	    d = get_win_info(wp, tabnr, winnr);
	    もし (d != NULL)
		list_append_dict(rettv->vval.v_list, d);
	    もし (wparg != NULL)
		// found information about a specific window
		戻る;
	}
    }
# ifdef FEAT_PROP_POPUP
    もし (wparg != NULL)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		もし (wp == wparg)
		    goto found;
	}
found:
	d = get_win_info(wparg, tp == NULL ? 0 : tabnr, 0);
	もし (d != NULL)
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
    整数 x = -1;
    整数 y = -1;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

# もし defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	もし (argvars[0].v_type != VAR_UNKNOWN)
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
# もし defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	整数	    x, y;

	もし (ui_get_winpos(&x, &y, 100) == OK)
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
# もし defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	整数	    x, y;

	もし (ui_get_winpos(&x, &y, 100) == OK)
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
    整数		nr = 1;
    char_u	*arg;

    もし (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	もし (arg != NULL)
	{
	    もし (STRCMP(arg, "$") == 0)
		nr = tabpage_index(NULL) - 1;
	    否则 もし (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    否则
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    否则
	nr = tabpage_index(curtab);
    rettv->vval.v_number = nr;
}

/*
 * "tabpagewinnr()" function
 */
    void
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    整数		nr = 1;
    tabpage_T	*tp;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	戻る;

    tp = find_tabpage((整数)tv_get_number(&argvars[0]));
    もし (tp == NULL)
	nr = 0;
    否则
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_number = nr;
}

/*
 * "win_execute()" function
 */
    void
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    整数		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	戻る;

    id = (整数)tv_get_number(argvars);
    wp = win_id2wp_tp(id, &tp);
    もし (wp == NULL || tp == NULL)
	戻る;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    整数	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    整数	apply_acd = FALSE;
    char_u	*save_sfname = NULL;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    もし (curwin != wp
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
    もし (cwd_status == OK && p_acd)
    {
	もし (curbuf->b_sfname != NULL && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    もし (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    もし (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    否则
# endif
	もし (cwd_status == OK)
	{
	    mch_chdir((char *)cwd);
# ifdef FEAT_AUTOCHDIR
	    もし (save_sfname != NULL)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    もし (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    もし (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    void
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    void
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	戻る;

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
    整数		id;

    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    id = tv_get_number(&argvars[0]);
    もし (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_number = 1;
	戻る;
    }

    もし (text_or_buf_locked())
	戻る;
# もし defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    もし (popup_is_popup(curwin) && curbuf->b_term != NULL)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	戻る;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	もし (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    もし (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_number = 1;
	    戻る;
	}
}

/*
 * "win_id2tabwin()" function
 */
    void
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    void
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    rettv->vval.v_number = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    void
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    整数		offset;

    rettv->vval.v_number = FALSE;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    wp = find_win_by_nr_or_id(&argvars[0]);
    もし (wp == NULL || win_valid_popup(wp))
	戻る;
    もし (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	戻る;
    }

    offset = (整数)tv_get_number(&argvars[1]);
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
    整数		offset;

    rettv->vval.v_number = FALSE;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	戻る;

    wp = find_win_by_nr_or_id(&argvars[0]);
    もし (wp == NULL || win_valid_popup(wp))
	戻る;
    もし (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	戻る;
    }

    offset = (整数)tv_get_number(&argvars[1]);
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

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

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
    整数     flags = 0, size = 0;

    rettv->vval.v_number = -1;

    もし (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	戻る;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    もし (wp == NULL || targetwin == NULL || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_number));
	戻る;
    }

    もし (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	もし (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    戻る;

	d = argvars[2].vval.v_dict;
	もし (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	もし ((di = dict_find(d, (char_u *)"rightbelow", -1)) != NULL)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (整数)dict_get_number(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    もし (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	戻る;

    もし (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    もし (curwin == targetwin && win_valid(wp))
    {
	もし (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_number = 0;
    }
    否则
	emsg(_(e_autocommands_caused_command_to_abort));

    もし (oldwin != curwin && win_valid(oldwin))
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

    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	もし (wp == NULL)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    戻る;
	}
    }
    もし (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# もし defined(FEAT_QUICKFIX)
    否则 もし (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    否则 もし (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    否则 もし (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    否则 もし (bt_quickfix(wp->w_buffer))
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
    もし (rettv->vval.v_string == NULL)
	戻る;

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

    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    wp = find_win_by_nr_or_id(&argvars[0]);
    もし (wp == NULL)
	rettv->vval.v_number = -1;
    否则
	rettv->vval.v_number = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    void
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    整数 col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    もし (curwin->w_p_rl)
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

    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    wp = find_win_by_nr_or_id(&argvars[0]);
    もし (wp == NULL)
	rettv->vval.v_number = -1;
    否则
	rettv->vval.v_number = wp->w_height;
}

/*
 * "winlayout()" function
 */
    void
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	戻る;

    もし (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    否则
    {
	tp = find_tabpage((整数)tv_get_number(&argvars[0]));
	もし (tp == NULL)
	    戻る;
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
    整数		nr = 1;

    もし (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	戻る;

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
    整数		i;
    整数		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, sizeof(char), 70);

    // Do this twice to handle some window layouts properly.
    ために (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((char *)buf, sizeof(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((char *)buf, sizeof(buf),
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

    もし (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	戻る;

    dict = argvars[0].vval.v_dict;
    もし (dict_has_key(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_number(dict, "lnum");
    もし (dict_has_key(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_number(dict, "col");
    もし (dict_has_key(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_number(dict, "coladd");
    もし (dict_has_key(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_number(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    もし (dict_has_key(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_number(dict, "topline"));
# ifdef FEAT_DIFF
    もし (dict_has_key(dict, "topfill"))
	curwin->w_topfill = (整数)dict_get_number(dict, "topfill");
# endif
    もし (dict_has_key(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_number(dict, "leftcol");
    もし (dict_has_key(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_number(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    もし (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    もし (curwin->w_topline > curbuf->b_ml.ml_line_count)
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

    もし (rettv_dict_alloc(rettv) == FAIL)
	戻る;
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

    もし (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	戻る;

    wp = find_win_by_nr_or_id(&argvars[0]);
    もし (wp == NULL)
	rettv->vval.v_number = -1;
    否则
	rettv->vval.v_number = wp->w_width;
}
#endif // FEAT_EVAL

#もし defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    整数
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	整数	    no_display)
{
    block_autocmds();
    戻る switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    整数
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	整数	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    もし (win == curwin)
	switchwin->sw_same_win = TRUE;
    否则
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    もし (tp != NULL)
    {
	switchwin->sw_curtab = curtab;
	もし (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	否则
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    もし (!win_valid(win))
	戻る FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    戻る OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    void
restore_win(
	switchwin_T *switchwin,
	整数	    no_display)
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
	整数	    no_display)
{
    もし (switchwin->sw_curtab != NULL && valid_tabpage(switchwin->sw_curtab))
    {
	もし (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	否则
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    もし (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    もし (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    否则 もし (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
