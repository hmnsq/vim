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

#만약 defined(FEAT_EVAL)

    static 정수
win_getid(typval_T *argvars)
{
    정수	    winnr;
    win_T   *wp;

    만약 (argvars[0].v_type == VAR_UNKNOWN)
	반환 curwin->w_id;
    winnr = tv_get_number(&argvars[0]);
    만약 (winnr <= 0)
	반환 0;

    만약 (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    아니면
    {
	tabpage_T	*tp;
	정수		tabnr = tv_get_number(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    만약 (--tabnr == 0)
		break;
	만약 (tp == NULL)
	    반환 -1;
	만약 (tp == curtab)
	    wp = firstwin;
	아니면
	    wp = tp->tp_firstwin;
    }
    위해 ( ; wp != NULL; wp = wp->w_next)
	만약 (--winnr == 0)
	    반환 wp->w_id;
    반환 0;
}

    static void
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    정수		winnr = 1;
    정수		tabnr = 1;
    정수		id = tv_get_number(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    만약 (wp->w_id == id)
	    {
		list_append_number(list, tabnr);
		list_append_number(list, winnr);
		반환;
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
win_id2wp(정수 id)
{
    반환 win_id2wp_tp(id, NULL);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(정수 id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	만약 (wp->w_id == id)
	{
	    만약 (tpp != NULL)
		*tpp = tp;
	    반환 wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     만약 (wp->w_id == id)
	     {
		 만약 (tpp != NULL)
		     *tpp = tp;
		 반환 wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	만약 (wp->w_id == id)
	{
	    만약 (tpp != NULL)
		*tpp = curtab;  // any tabpage would do
	    반환 wp;
	}
# endif

    반환 NULL;
}

    static 정수
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    정수	    nr = 1;
    정수	    id = tv_get_number(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	만약 (wp->w_id == id)
	    반환 nr;
	++nr;
    }
    반환 0;
}

    void
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    정수		bufnr = tv_get_number(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    만약 (wp->w_buffer->b_fnum == bufnr)
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
    정수		nr = (정수)tv_get_number_chk(vp, NULL);

    만약 (nr < 0)
	반환 NULL;
    만약 (nr == 0)
	반환 curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	만약 (nr >= LOWEST_WIN_ID)
	{
	    만약 (wp->w_id == nr)
		반환 wp;
	}
	아니면 만약 (--nr <= 0)
	    break;
    }
    만약 (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	위해 (wp = (tp == NULL ? curtab : tp)->tp_first_popupwin;
						   wp != NULL; wp = wp->w_next)
	    만약 (wp->w_id == nr)
		반환 wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    만약 (wp->w_id == nr)
		반환 wp;
# endif
	반환 NULL;
    }
    반환 wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    정수	nr = (정수)tv_get_number_chk(vp, NULL);

    만약 (nr >= LOWEST_WIN_ID)
	반환 win_id2wp(tv_get_number(vp));
    반환 find_win_by_nr(vp, NULL);
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

    만약 (wvp->v_type != VAR_UNKNOWN)
    {
	만약 (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_number(tvp);
	    만약 (n >= 0)
		tp = find_tabpage(n);
	}
	아니면
	    tp = curtab;

	만약 (tp != NULL)
	{
	    wp = find_win_by_nr(wvp, tp);
	    만약 (wp == NULL && wvp->v_type == VAR_NUMBER
						&& wvp->vval.v_number != -1)
		// A window with the specified number is not found
		tp = NULL;
	}
    }
    아니면
    {
	wp = curwin;
	tp = curtab;
    }

    만약 (ptp != NULL)
	*ptp = tp;

    반환 wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    static void
get_framelayout(frame_T *fr, list_T *l, 정수 outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    만약 (fr == NULL)
	반환;

    만약 (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    아니면
    {
	fr_list = list_alloc();
	만약 (fr_list == NULL)
	    반환;
	만약 (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    반환;
	}
    }

    만약 (fr->fr_layout == FR_LEAF)
    {
	만약 (fr->fr_win != NULL)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_number(fr_list, fr->fr_win->w_id);
	}
    }
    아니면
    {
	만약 (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	아니면
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	만약 (win_list == NULL)
	    반환;
	만약 (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    반환;
	}

	child = fr->fr_child;
	동안 (child != NULL)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    static 정수
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    정수		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    만약 (argvar->v_type != VAR_UNKNOWN)
    {
	정수	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	만약 (arg == NULL)
	    nr = 0;		// type error; errmsg already given
	아니면 만약 (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	아니면 만약 (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	아니면
	{
	    long	count;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    count = strtol((char *)arg, (char **)&endp, 10);
	    만약 (count <= 0)
		count = 1;	// if count is not specified, default to 1
	    만약 (endp != NULL && *endp != '\0')
	    {
		만약 (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, count);
		아니면 만약 (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, count);
		아니면 만약 (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, count);
		아니면 만약 (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, count);
		아니면
		    invalid_arg = TRUE;
	    }
	    아니면
		invalid_arg = TRUE;
	}
	만약 (twin == NULL)
	    nr = 0;

	만약 (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    만약 (nr <= 0)
	반환 0;

    위해 (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	만약 (wp == NULL)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    break;
	}
	++nr;
    }
    반환 nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    static dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    만약 (dict == NULL)
	반환 NULL;

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

    반환 dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    static dict_T *
get_tabpage_info(tabpage_T *tp, 정수 tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    만약 (dict == NULL)
	반환 NULL;

    dict_add_number(dict, "tabnr", tp_idx);

    l = list_alloc();
    만약 (l != NULL)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_number(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    반환 dict;
}

/*
 * "gettabinfo()" function
 */
    void
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = NULL;
    dict_T	*d;
    정수		tpnr = 0;

    만약 (rettv_list_alloc(rettv) == FAIL)
	반환;

    만약 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((정수)tv_get_number_chk(&argvars[0], NULL));
	만약 (tparg == NULL)
	    반환;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	만약 (tparg != NULL && tp != tparg)
	    continue;
	d = get_tabpage_info(tp, tpnr);
	만약 (d != NULL)
	    list_append_dict(rettv->vval.v_list, d);
	만약 (tparg != NULL)
	    반환;
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

    만약 (rettv_list_alloc(rettv) == FAIL)
	반환;

    만약 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_number(&argvars[0]));
	만약 (wparg == NULL)
	    반환;
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
	    만약 (wparg != NULL && wp != wparg)
		continue;
	    d = get_win_info(wp, tabnr, winnr);
	    만약 (d != NULL)
		list_append_dict(rettv->vval.v_list, d);
	    만약 (wparg != NULL)
		// found information about a specific window
		반환;
	}
    }
# ifdef FEAT_PROP_POPUP
    만약 (wparg != NULL)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		만약 (wp == wparg)
		    goto found;
	}
found:
	d = get_win_info(wparg, tp == NULL ? 0 : tabnr, 0);
	만약 (d != NULL)
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
    정수 x = -1;
    정수 y = -1;

    만약 (rettv_list_alloc(rettv) == FAIL)
	반환;

    만약 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	반환;

# 만약 defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	만약 (argvars[0].v_type != VAR_UNKNOWN)
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
# 만약 defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	정수	    x, y;

	만약 (ui_get_winpos(&x, &y, 100) == OK)
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
# 만약 defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	정수	    x, y;

	만약 (ui_get_winpos(&x, &y, 100) == OK)
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
    정수		nr = 1;
    char_u	*arg;

    만약 (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	만약 (arg != NULL)
	{
	    만약 (STRCMP(arg, "$") == 0)
		nr = tabpage_index(NULL) - 1;
	    아니면 만약 (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    아니면
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    아니면
	nr = tabpage_index(curtab);
    rettv->vval.v_number = nr;
}

/*
 * "tabpagewinnr()" function
 */
    void
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    정수		nr = 1;
    tabpage_T	*tp;

    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	반환;

    tp = find_tabpage((정수)tv_get_number(&argvars[0]));
    만약 (tp == NULL)
	nr = 0;
    아니면
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_number = nr;
}

/*
 * "win_execute()" function
 */
    void
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    정수		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	반환;

    id = (정수)tv_get_number(argvars);
    wp = win_id2wp_tp(id, &tp);
    만약 (wp == NULL || tp == NULL)
	반환;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    정수	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    정수	apply_acd = FALSE;
    char_u	*save_sfname = NULL;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    만약 (curwin != wp
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
    만약 (cwd_status == OK && p_acd)
    {
	만약 (curbuf->b_sfname != NULL && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    만약 (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    만약 (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    아니면
# endif
	만약 (cwd_status == OK)
	{
	    mch_chdir((char *)cwd);
# ifdef FEAT_AUTOCHDIR
	    만약 (save_sfname != NULL)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    만약 (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    만약 (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    void
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	반환;

    만약 (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    void
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    만약 (in_vim9script()
	    && (check_for_opt_number_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 1) == FAIL)))
	반환;

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
    정수		id;

    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	반환;

    id = tv_get_number(&argvars[0]);
    만약 (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_number = 1;
	반환;
    }

    만약 (text_or_buf_locked())
	반환;
# 만약 defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    만약 (popup_is_popup(curwin) && curbuf->b_term != NULL)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	반환;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	만약 (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    만약 (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_number = 1;
	    반환;
	}
}

/*
 * "win_id2tabwin()" function
 */
    void
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	반환;

    만약 (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    void
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	반환;

    rettv->vval.v_number = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    void
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    정수		offset;

    rettv->vval.v_number = FALSE;

    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	반환;

    wp = find_win_by_nr_or_id(&argvars[0]);
    만약 (wp == NULL || win_valid_popup(wp))
	반환;
    만약 (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	반환;
    }

    offset = (정수)tv_get_number(&argvars[1]);
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
    정수		offset;

    rettv->vval.v_number = FALSE;

    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL))
	반환;

    wp = find_win_by_nr_or_id(&argvars[0]);
    만약 (wp == NULL || win_valid_popup(wp))
	반환;
    만약 (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	반환;
    }

    offset = (정수)tv_get_number(&argvars[1]);
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

    만약 (rettv_list_alloc(rettv) == FAIL)
	반환;

    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	반환;

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
    정수     flags = 0, size = 0;

    rettv->vval.v_number = -1;

    만약 (in_vim9script()
	    && (check_for_number_arg(argvars, 0) == FAIL
		|| check_for_number_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	반환;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    만약 (wp == NULL || targetwin == NULL || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_number));
	반환;
    }

    만약 (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	만약 (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    반환;

	d = argvars[2].vval.v_dict;
	만약 (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	만약 ((di = dict_find(d, (char_u *)"rightbelow", -1)) != NULL)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (정수)dict_get_number(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    만약 (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	반환;

    만약 (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    만약 (curwin == targetwin && win_valid(wp))
    {
	만약 (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_number = 0;
    }
    아니면
	emsg(_(e_autocommands_caused_command_to_abort));

    만약 (oldwin != curwin && win_valid(oldwin))
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

    만약 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	만약 (wp == NULL)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    반환;
	}
    }
    만약 (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# 만약 defined(FEAT_QUICKFIX)
    아니면 만약 (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    아니면 만약 (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    아니면 만약 (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    아니면 만약 (bt_quickfix(wp->w_buffer))
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
    만약 (rettv->vval.v_string == NULL)
	반환;

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

    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	반환;

    wp = find_win_by_nr_or_id(&argvars[0]);
    만약 (wp == NULL)
	rettv->vval.v_number = -1;
    아니면
	rettv->vval.v_number = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    void
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    정수 col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    만약 (curwin->w_p_rl)
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

    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	반환;

    wp = find_win_by_nr_or_id(&argvars[0]);
    만약 (wp == NULL)
	rettv->vval.v_number = -1;
    아니면
	rettv->vval.v_number = wp->w_height;
}

/*
 * "winlayout()" function
 */
    void
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    만약 (rettv_list_alloc(rettv) == FAIL)
	반환;

    만약 (in_vim9script() && check_for_opt_number_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    아니면
    {
	tp = find_tabpage((정수)tv_get_number(&argvars[0]));
	만약 (tp == NULL)
	    반환;
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
    정수		nr = 1;

    만약 (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	반환;

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
    정수		i;
    정수		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, sizeof(char), 70);

    // Do this twice to handle some window layouts properly.
    위해 (i = 0; i < 2; ++i)
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

    만약 (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	반환;

    dict = argvars[0].vval.v_dict;
    만약 (dict_has_key(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_number(dict, "lnum");
    만약 (dict_has_key(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_number(dict, "col");
    만약 (dict_has_key(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_number(dict, "coladd");
    만약 (dict_has_key(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_number(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    만약 (dict_has_key(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_number(dict, "topline"));
# ifdef FEAT_DIFF
    만약 (dict_has_key(dict, "topfill"))
	curwin->w_topfill = (정수)dict_get_number(dict, "topfill");
# endif
    만약 (dict_has_key(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_number(dict, "leftcol");
    만약 (dict_has_key(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_number(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    만약 (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    만약 (curwin->w_topline > curbuf->b_ml.ml_line_count)
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

    만약 (rettv_dict_alloc(rettv) == FAIL)
	반환;
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

    만약 (in_vim9script() && check_for_number_arg(argvars, 0) == FAIL)
	반환;

    wp = find_win_by_nr_or_id(&argvars[0]);
    만약 (wp == NULL)
	rettv->vval.v_number = -1;
    아니면
	rettv->vval.v_number = wp->w_width;
}
#endif // FEAT_EVAL

#만약 defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    정수
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	정수	    no_display)
{
    block_autocmds();
    반환 switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    정수
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	정수	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    만약 (win == curwin)
	switchwin->sw_same_win = TRUE;
    아니면
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    만약 (tp != NULL)
    {
	switchwin->sw_curtab = curtab;
	만약 (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	아니면
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    만약 (!win_valid(win))
	반환 FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    반환 OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    void
restore_win(
	switchwin_T *switchwin,
	정수	    no_display)
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
	정수	    no_display)
{
    만약 (switchwin->sw_curtab != NULL && valid_tabpage(switchwin->sw_curtab))
    {
	만약 (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	아니면
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    만약 (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    만약 (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    아니면 만약 (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
