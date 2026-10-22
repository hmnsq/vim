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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_getid(typval_T *argvars)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    winnr;
    win_T   *wp;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
	ଫେରନ୍ତୁ curwin->w_id;
    winnr = tv_get_ସଂଖ୍ୟା(&argvars[0]);
    ଯଦି (winnr <= 0)
	ଫେରନ୍ତୁ 0;

    ଯଦି (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    ଅନ୍ୟଥା
    {
	tabpage_T	*tp;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabnr = tv_get_ସଂଖ୍ୟା(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    ଯଦି (--tabnr == 0)
		ଭାଙ୍ଗ;
	ଯଦି (tp == ଖାଲି)
	    ଫେରନ୍ତୁ -1;
	ଯଦି (tp == curtab)
	    wp = firstwin;
	ଅନ୍ୟଥା
	    wp = tp->tp_firstwin;
    }
    ପାଇଁ ( ; wp != ଖାଲି; wp = wp->w_next)
	ଯଦି (--winnr == 0)
	    ଫେରନ୍ତୁ wp->w_id;
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ଶୂନ୍ୟ
win_id2tabwin(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winnr = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabnr = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id = tv_get_ସଂଖ୍ୟା(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	{
	    ଯଦି (wp->w_id == id)
	    {
		list_append_ସଂଖ୍ୟା(list, tabnr);
		list_append_ସଂଖ୍ୟା(list, winnr);
		ଫେରନ୍ତୁ;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_ସଂଖ୍ୟା(list, 0);
    list_append_ସଂଖ୍ୟା(list, 0);
}

/*
 * Return the window pointer of window "id".
 */
    win_T *
win_id2wp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    ଫେରନ୍ତୁ win_id2wp_tp(id, ଖାଲି);
}

/*
 * Return the window and tab pointer of window "id".
 * Returns NULL when not found.
 */
    win_T *
win_id2wp_tp(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, tabpage_T **tpp)
{
    win_T	*wp;
    tabpage_T   *tp;

    FOR_ALL_TAB_WINDOWS(tp, wp)
	ଯଦି (wp->w_id == id)
	{
	    ଯଦି (tpp != ଖାଲି)
		*tpp = tp;
	    ଫେରନ୍ତୁ wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
	     ଯଦି (wp->w_id == id)
	     {
		 ଯଦି (tpp != ଖାଲି)
		     *tpp = tp;
		 ଫେରନ୍ତୁ wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	ଯଦି (wp->w_id == id)
	{
	    ଯଦି (tpp != ଖାଲି)
		*tpp = curtab;  // any tabpage would do
	    ଫେରନ୍ତୁ wp;
	}
# endif

    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
win_id2win(typval_T *argvars)
{
    win_T   *wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nr = 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id = tv_get_ସଂଖ୍ୟା(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	ଯଦି (wp->w_id == id)
	    ଫେରନ୍ତୁ nr;
	++nr;
    }
    ଫେରନ୍ତୁ 0;
}

    ଶୂନ୍ୟ
win_findbuf(typval_T *argvars, list_T *list)
{
    win_T	*wp;
    tabpage_T   *tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		bufnr = tv_get_ସଂଖ୍ୟା(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    ଯଦି (wp->w_buffer->b_fnum == bufnr)
		list_append_ସଂଖ୍ୟା(list, wp->w_id);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(vp, ଖାଲି);

    ଯଦି (nr < 0)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (nr == 0)
	ଫେରନ୍ତୁ curwin;

    FOR_ALL_WINDOWS_IN_TAB(tp, wp)
    {
	ଯଦି (nr >= LOWEST_WIN_ID)
	{
	    ଯଦି (wp->w_id == nr)
		ଫେରନ୍ତୁ wp;
	}
	ଅନ୍ୟଥା ଯଦି (--nr <= 0)
	    ଭାଙ୍ଗ;
    }
    ଯଦି (nr >= LOWEST_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	ପାଇଁ (wp = (tp == ଖାଲି ? curtab : tp)->tp_first_popupwin;
						   wp != ଖାଲି; wp = wp->w_next)
	    ଯଦି (wp->w_id == nr)
		ଫେରନ୍ତୁ wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    ଯଦି (wp->w_id == nr)
		ଫେରନ୍ତୁ wp;
# endif
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ wp;
}

/*
 * Find a window: When using a Window ID in any tab page, when using a number
 * in the current tab page.
 * Returns NULL when not found.
 */
    win_T *
find_win_by_nr_or_id(typval_T *vp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nr = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(vp, ଖାଲି);

    ଯଦି (nr >= LOWEST_WIN_ID)
	ଫେରନ୍ତୁ win_id2wp(tv_get_ସଂଖ୍ୟା(vp));
    ଫେରନ୍ତୁ find_win_by_nr(vp, ଖାଲି);
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
    win_T	*wp = ଖାଲି;
    tabpage_T	*tp = ଖାଲି;
    long	n;

    ଯଦି (wvp->v_type != VAR_UNKNOWN)
    {
	ଯଦି (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (long)tv_get_ସଂଖ୍ୟା(tvp);
	    ଯଦି (n >= 0)
		tp = find_tabpage(n);
	}
	ଅନ୍ୟଥା
	    tp = curtab;

	ଯଦି (tp != ଖାଲି)
	{
	    wp = find_win_by_nr(wvp, tp);
	    ଯଦି (wp == ଖାଲି && wvp->v_type == VAR_ସଂଖ୍ୟା
						&& wvp->vval.v_ସଂଖ୍ୟା != -1)
		// A window with the specified number is not found
		tp = ଖାଲି;
	}
    }
    ଅନ୍ୟଥା
    {
	wp = curwin;
	tp = curtab;
    }

    ଯଦି (ptp != ଖାଲି)
	*ptp = tp;

    ଫେରନ୍ତୁ wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_framelayout(frame_T *fr, list_T *l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା outer)
{
    frame_T	*child;
    list_T	*fr_list;
    list_T	*win_list;

    ଯଦି (fr == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    ଅନ୍ୟଥା
    {
	fr_list = list_alloc();
	ଯଦି (fr_list == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ଯଦି (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    ଫେରନ୍ତୁ;
	}
    }

    ଯଦି (fr->fr_layout == FR_LEAF)
    {
	ଯଦି (fr->fr_win != ଖାଲି)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_ସଂଖ୍ୟା(fr_list, fr->fr_win->w_id);
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (fr->fr_layout == FR_ROW)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	ଅନ୍ୟଥା
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	ଯଦି (win_list == ଖାଲି)
	    ଫେରନ୍ତୁ;
	ଯଦି (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    ଫେରନ୍ତୁ;
	}

	child = fr->fr_child;
	ଯେତେବେଳେ (child != ଖାଲି)
	{
	    get_framelayout(child, win_list, FALSE);
	    child = child->fr_next;
	}
    }
}

/*
 * Common code for tabpagewinnr() and winnr().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_winnr(tabpage_T *tp, typval_T *argvar)
{
    win_T	*twin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = 1;
    win_T	*wp;
    char_u	*arg;

    twin = (tp == curtab) ? curwin : tp->tp_curwin;
    ଯଦି (argvar->v_type != VAR_UNKNOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	ଯଦି (arg == ଖାଲି)
	    nr = 0;		// type error; errmsg already given
	ଅନ୍ୟଥା ଯଦି (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	ଅନ୍ୟଥା ଯଦି (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	ଅନ୍ୟଥା
	{
	    long	ଗଣନା_2;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    ଗଣନା_2 = strtol((ଅକ୍ଷର *)arg, (ଅକ୍ଷର **)&endp, 10);
	    ଯଦି (ଗଣନା_2 <= 0)
		ଗଣନା_2 = 1;	// if count is not specified, default to 1
	    ଯଦି (endp != ଖାଲି && *endp != '\0')
	    {
		ଯଦି (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, ଗଣନା_2);
		ଅନ୍ୟଥା ଯଦି (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, ଗଣନା_2);
		ଅନ୍ୟଥା ଯଦି (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, ଗଣନା_2);
		ଅନ୍ୟଥା ଯଦି (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, ଗଣନା_2);
		ଅନ୍ୟଥା
		    invalid_arg = TRUE;
	    }
	    ଅନ୍ୟଥା
		invalid_arg = TRUE;
	}
	ଯଦି (twin == ଖାଲି)
	    nr = 0;

	ଯଦି (invalid_arg)
	{
	    semsg(_(e_invalid_expression_str), arg);
	    nr = 0;
	}
    }

    ଯଦି (nr <= 0)
	ଫେରନ୍ତୁ 0;

    ପାଇଁ (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_next)
    {
	ଯଦି (wp == ଖାଲି)
	{
	    // didn't find it in this tabpage
	    nr = 0;
	    ଭାଙ୍ଗ;
	}
	++nr;
    }
    ଫେରନ୍ତୁ nr;
}

/*
 * Returns information about a window as a dictionary.
 */
    ସ୍ଥିର dict_T *
get_win_info(win_T *wp, short tpnr, short winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    ଯଦି (dict == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_add_ସଂଖ୍ୟା(dict, "tabnr", tpnr);
    dict_add_ସଂଖ୍ୟା(dict, "winnr", winnr);
    dict_add_ସଂଖ୍ୟା(dict, "winid", wp->w_id);
    dict_add_ସଂଖ୍ୟା(dict, "height", wp->w_height);
    dict_add_ସଂଖ୍ୟା(dict, "status_height", wp->w_status_height);
    dict_add_ସଂଖ୍ୟା(dict, "winrow", wp->w_winrow + 1);
    dict_add_ସଂଖ୍ୟା(dict, "topline", wp->w_topline);
    dict_add_ସଂଖ୍ୟା(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_add_ସଂଖ୍ୟା(dict, "winbar", wp->w_winbar_height);
# endif
    dict_add_ସଂଖ୍ୟା(dict, "width", wp->w_width);
    dict_add_ସଂଖ୍ୟା(dict, "wincol", wp->w_wincol + 1);
    dict_add_ସଂଖ୍ୟା(dict, "textoff", win_col_off(wp));
    dict_add_ସଂଖ୍ୟା(dict, "bufnr", wp->w_buffer->b_fnum);
    dict_add_ସଂଖ୍ୟା(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_add_ସଂଖ୍ୟା(dict, "terminal", bt_terminal(wp->w_buffer));
# endif
# ifdef FEAT_QUICKFIX
    dict_add_ସଂଖ୍ୟା(dict, "quickfix", bt_quickfix(wp->w_buffer));
    dict_add_ସଂଖ୍ୟା(dict, "loclist",
		      (bt_quickfix(wp->w_buffer) && wp->w_llist_ref != ଖାଲି));
# endif

    // Add a reference to window variables
    dict_add_dict(dict, "variables", wp->w_vars);

    ଫେରନ୍ତୁ dict;
}

/*
 * Returns information (variables, options, etc.) about a tab page
 * as a dictionary.
 */
    ସ୍ଥିର dict_T *
get_tabpage_info(tabpage_T *tp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tp_idx)
{
    win_T	*wp;
    dict_T	*dict;
    list_T	*l;

    dict = dict_alloc();
    ଯଦି (dict == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    dict_add_ସଂଖ୍ୟା(dict, "tabnr", tp_idx);

    l = list_alloc();
    ଯଦି (l != ଖାଲି)
    {
	FOR_ALL_WINDOWS_IN_TAB(tp, wp)
	    list_append_ସଂଖ୍ୟା(l, (varnumber_T)wp->w_id);
	dict_add_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_add_dict(dict, "variables", tp->tp_vars);

    ଫେରନ୍ତୁ dict;
}

/*
 * "gettabinfo()" function
 */
    ଶୂନ୍ୟ
f_gettabinfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp, *tparg = ଖାଲି;
    dict_T	*d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tpnr = 0;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = find_tabpage((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା_chk(&argvars[0], ଖାଲି));
	ଯଦି (tparg == ଖାଲି)
	    ଫେରନ୍ତୁ;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	ଯଦି (tparg != ଖାଲି && tp != tparg)
	    ଜାରି;
	d = get_tabpage_info(tp, tpnr);
	ଯଦି (d != ଖାଲି)
	    list_append_dict(rettv->vval.v_list, d);
	ଯଦି (tparg != ଖାଲି)
	    ଫେରନ୍ତୁ;
    }
}

/*
 * "getwininfo()" function
 */
    ଶୂନ୍ୟ
f_getwininfo(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;
    win_T	*wp = ଖାଲି, *wparg = ଖାଲି;
    dict_T	*d;
    short	tabnr = 0, winnr;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_ସଂଖ୍ୟା(&argvars[0]));
	ଯଦି (wparg == ଖାଲି)
	    ଫେରନ୍ତୁ;
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
	    ଯଦି (wparg != ଖାଲି && wp != wparg)
		ଜାରି;
	    d = get_win_info(wp, tabnr, winnr);
	    ଯଦି (d != ଖାଲି)
		list_append_dict(rettv->vval.v_list, d);
	    ଯଦି (wparg != ଖାଲି)
		// found information about a specific window
		ଫେରନ୍ତୁ;
	}
    }
# ifdef FEAT_PROP_POPUP
    ଯଦି (wparg != ଖାଲି)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_IN_TAB(tp, wp)
		ଯଦି (wp == wparg)
		    ଯାଅ found;
	}
found:
	d = get_win_info(wparg, tp == ଖାଲି ? 0 : tabnr, 0);
	ଯଦି (d != ଖାଲି)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    ଶୂନ୍ୟ
f_getwinpos(typval_T *argvars UNUSED, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା y = -1;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

# ଯଦି defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_ସଂଖ୍ୟା(&argvars[0]);

	(ଶୂନ୍ୟ)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)x);
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    ଶୂନ୍ୟ
f_getwinposx(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = -1;
# ଯଦି defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    x, y;

	ଯଦି (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_ସଂଖ୍ୟା = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    ଶୂନ୍ୟ
f_getwinposy(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->vval.v_ସଂଖ୍ୟା = -1;
# ଯଦି defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    x, y;

	ଯଦି (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_ସଂଖ୍ୟା = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    ଶୂନ୍ୟ
f_tabpagenr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = 1;
    char_u	*arg;

    ଯଦି (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	ଯଦି (arg != ଖାଲି)
	{
	    ଯଦି (STRCMP(arg, "$") == 0)
		nr = tabpage_index(ଖାଲି) - 1;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_index(lastused_tabpage) : 0;
	    ଅନ୍ୟଥା
		semsg(_(e_invalid_expression_str), arg);
	}
    }
    ଅନ୍ୟଥା
	nr = tabpage_index(curtab);
    rettv->vval.v_ସଂଖ୍ୟା = nr;
}

/*
 * "tabpagewinnr()" function
 */
    ଶୂନ୍ୟ
f_tabpagewinnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = 1;
    tabpage_T	*tp;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    tp = find_tabpage((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]));
    ଯଦି (tp == ଖାଲି)
	nr = 0;
    ଅନ୍ୟଥା
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_ସଂଖ୍ୟା = nr;
}

/*
 * "win_execute()" function
 */
    ଶୂନ୍ୟ
f_win_execute(typval_T *argvars, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    tabpage_T	*tp;
    win_T	*wp;
    switchwin_T	switchwin;

    // Return an empty string if something fails.
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    id = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(argvars);
    wp = win_id2wp_tp(id, &tp);
    ଯଦି (wp == ଖାଲି || tp == ଖାଲି)
	ଫେରନ୍ତୁ;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cwd_status = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	apply_acd = FALSE;
    char_u	*save_sfname = ଖାଲି;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    ଯଦି (curwin != wp
	    && (curwin->w_localdir != ଖାଲି
		|| wp->w_localdir != ଖାଲି
		|| (curtab != tp
		    && (curtab->tp_localdir != ଖାଲି
			|| tp->tp_localdir != ଖାଲି))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_status = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    ଯଦି (cwd_status == OK && p_acd)
    {
	ଯଦି (curbuf->b_sfname != ଖାଲି && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    ଯଦି (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    ଯଦି (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    ଅନ୍ୟଥା
# endif
	ଯଦି (cwd_status == OK)
	{
	    mch_chdir((ଅକ୍ଷର *)cwd);
# ifdef FEAT_AUTOCHDIR
	    ଯଦି (save_sfname != ଖାଲି)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    ଯଦି (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_status = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    ଯଦି (VIsual_active)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    ଶୂନ୍ୟ
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    ଶୂନ୍ୟ
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script()
	    && (check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL)))
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = win_getid(argvars);
}

/*
 * "win_gotoid()" function
 */
    ଶୂନ୍ୟ
f_win_gotoid(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T   *tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    id = tv_get_ସଂଖ୍ୟା(&argvars[0]);
    ଯଦି (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_ସଂଖ୍ୟା = 1;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (text_or_buf_locked())
	ଫେରନ୍ତୁ;
# ଯଦି defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    ଯଦି (popup_is_popup(curwin) && curbuf->b_term != ଖାଲି)
    {
	emsg(_(e_not_allowed_for_terminal_in_popup_window));
	ଫେରନ୍ତୁ;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	ଯଦି (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    ଯଦି (VIsual_active && wp->w_buffer != curbuf)
		end_visual_mode();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_ସଂଖ୍ୟା = 1;
	    ଫେରନ୍ତୁ;
	}
}

/*
 * "win_id2tabwin()" function
 */
    ଶୂନ୍ୟ
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    ଶୂନ୍ୟ
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    rettv->vval.v_ସଂଖ୍ୟା = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    ଶୂନ୍ୟ
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		offset;

    rettv->vval.v_ସଂଖ୍ୟା = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ଯଦି (wp == ଖାଲି || win_valid_popup(wp))
	ଫେରନ୍ତୁ;
    ଯଦି (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	ଫେରନ୍ତୁ;
    }

    offset = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[1]);
    win_drag_vsep_ଧାଡ଼ି(wp, offset);
    rettv->vval.v_ସଂଖ୍ୟା = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    ଶୂନ୍ୟ
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		offset;

    rettv->vval.v_ସଂଖ୍ୟା = FALSE;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ଯଦି (wp == ଖାଲି || win_valid_popup(wp))
	ଫେରନ୍ତୁ;
    ଯଦି (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_window_in_another_tab_page));
	ଫେରନ୍ତୁ;
    }

    offset = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[1]);
    win_drag_status_ଧାଡ଼ି(wp, offset);
    rettv->vval.v_ସଂଖ୍ୟା = TRUE;
}

/*
 * "win_screenpos()" function
 */
    ଶୂନ୍ୟ
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, wp == ଖାଲି ? 0 : wp->w_winrow + 1);
    list_append_ସଂଖ୍ୟା(rettv->vval.v_list, wp == ଖାଲି ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    ଶୂନ୍ୟ
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା     flags = 0, size = 0;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL
		|| check_for_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    targetwin = find_win_by_nr_or_id(&argvars[1]);

    ଯଦି (wp == ଖାଲି || targetwin == ଖାଲି || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_window_ସଂଖ୍ୟା));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	ଯଦି (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    ଫେରନ୍ତୁ;

	d = argvars[2].vval.v_dict;
	ଯଦି (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	ଯଦି ((di = dict_find(d, (char_u *)"rightbelow", -1)) != ଖାଲି)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_BELOW : WSP_ABOVE;
	size = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    ଯଦି (text_or_buf_locked() || check_split_disallowed(wp) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    ଯଦି (curwin == targetwin && win_valid(wp))
    {
	ଯଦି (win_splitmove(wp, size, flags) == OK)
	    rettv->vval.v_ସଂଖ୍ୟା = 0;
    }
    ଅନ୍ୟଥା
	emsg(_(e_autocommands_caused_command_to_abort));

    ଯଦି (oldwin != curwin && win_valid(oldwin))
	win_goto(oldwin);
}

/*
 * "win_gettype(nr)" function
 */
    ଶୂନ୍ୟ
f_win_gettype(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp = curwin;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = find_win_by_nr_or_id(&argvars[0]);
	ଯଦି (wp == ଖାଲି)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    ଫେରନ୍ତୁ;
	}
    }
    ଯଦି (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# ଯଦି defined(FEAT_QUICKFIX)
    ଅନ୍ୟଥା ଯଦି (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    ଅନ୍ୟଥା ଯଦି (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    ଅନ୍ୟଥା ଯଦି (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    ଅନ୍ୟଥା ଯଦି (bt_quickfix(wp->w_buffer))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != ଖାଲି ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    ଶୂନ୍ୟ
f_getcmdwintype(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;
    rettv->vval.v_string = alloc(2);
    ଯଦି (rettv->vval.v_string == ଖାଲି)
	ଫେରନ୍ତୁ;

    rettv->vval.v_string[0] = cmdwin_type;
    rettv->vval.v_string[1] = NUL;
}

/*
 * "winbufnr(nr)" function
 */
    ଶୂନ୍ୟ
f_winbufnr(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ଯଦି (wp == ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = wp->w_buffer->b_fnum;
}

/*
 * "wincol()" function
 */
    ଶୂନ୍ୟ
f_wincol(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    ଯଦି (curwin->w_p_rl)
	col = curwin->w_width - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_ସଂଖ୍ୟା = col;
}

/*
 * "winheight(nr)" function
 */
    ଶୂନ୍ୟ
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ଯଦି (wp == ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = wp->w_height;
}

/*
 * "winlayout()" function
 */
    ଶୂନ୍ୟ
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script() && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    ଅନ୍ୟଥା
    {
	tp = find_tabpage((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tv_get_ସଂଖ୍ୟା(&argvars[0]));
	ଯଦି (tp == ଖାଲି)
	    ଫେରନ୍ତୁ;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    ଶୂନ୍ୟ
f_winline(typval_T *argvars UNUSED, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_ସଂଖ୍ୟା = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    ଶୂନ୍ୟ
f_winnr(typval_T *argvars UNUSED, typval_T *rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = 1;

    ଯଦି (in_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_ସଂଖ୍ୟା = nr;
}

/*
 * "winrestcmd()" function
 */
    ଶୂନ୍ୟ
f_winrestcmd(typval_T *argvars UNUSED, typval_T *rettv)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		winnr;
    garray_T	ga;
    char_u	buf[50];

    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 70);

    // Do this twice to handle some window layouts properly.
    ପାଇଁ (i = 0; i < 2; ++i)
    {
	winnr = 1;
	FOR_ALL_WINDOWS(wp)
	{
	    size_t  buflen;

	    buflen = vim_snprintf_safelen((ଅକ୍ଷର *)buf, ଆକାର(buf),
		":%dresize %d|", winnr, wp->w_height);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((ଅକ୍ଷର *)buf, ଆକାର(buf),
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
    ଶୂନ୍ୟ
f_winrestview(typval_T *argvars, typval_T *rettv UNUSED)
{
    dict_T	*dict;

    ଯଦି (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    dict = argvars[0].vval.v_dict;
    ଯଦି (dict_has_ଚାବି(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_ସଂଖ୍ୟା(dict, "lnum");
    ଯଦି (dict_has_ଚାବି(dict, "col"))
	curwin->w_cursor.col = (colnr_T)dict_get_ସଂଖ୍ୟା(dict, "col");
    ଯଦି (dict_has_ଚାବି(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_ସଂଖ୍ୟା(dict, "coladd");
    ଯଦି (dict_has_ଚାବି(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_ସଂଖ୍ୟା(dict, "curswant");
	curwin->w_set_curswant = false;
    }

    ଯଦି (dict_has_ଚାବି(dict, "topline"))
	set_topline(curwin, (linenr_T)dict_get_ସଂଖ୍ୟା(dict, "topline"));
# ifdef FEAT_DIFF
    ଯଦି (dict_has_ଚାବି(dict, "topfill"))
	curwin->w_topfill = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(dict, "topfill");
# endif
    ଯଦି (dict_has_ଚାବି(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_ସଂଖ୍ୟା(dict, "leftcol");
    ଯଦି (dict_has_ଚାବି(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_ସଂଖ୍ୟା(dict, "skipcol");

    check_cursor();
    win_new_height(curwin, curwin->w_height);
    win_new_width(curwin, curwin->w_width);
    changed_window_setting();

    ଯଦି (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    ଯଦି (curwin->w_topline > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	curwin->w_topline = curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    ଶୂନ୍ୟ
f_winsaveview(typval_T *argvars UNUSED, typval_T *rettv)
{
    dict_T	*dict;

    ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;
    dict = rettv->vval.v_dict;

    dict_add_ସଂଖ୍ୟା(dict, "lnum", (long)curwin->w_cursor.lnum);
    dict_add_ସଂଖ୍ୟା(dict, "col", (long)curwin->w_cursor.col);
    dict_add_ସଂଖ୍ୟା(dict, "coladd", (long)curwin->w_cursor.coladd);
    update_curswant();
    dict_add_ସଂଖ୍ୟା(dict, "curswant", (long)curwin->w_curswant);

    dict_add_ସଂଖ୍ୟା(dict, "topline", (long)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_add_ସଂଖ୍ୟା(dict, "topfill", (long)curwin->w_topfill);
# endif
    dict_add_ସଂଖ୍ୟା(dict, "leftcol", (long)curwin->w_leftcol);
    dict_add_ସଂଖ୍ୟା(dict, "skipcol", (long)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    ଶୂନ୍ୟ
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    ଯଦି (in_vim9script() && check_for_ସଂଖ୍ୟା_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    wp = find_win_by_nr_or_id(&argvars[0]);
    ଯଦି (wp == ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଅନ୍ୟଥା
	rettv->vval.v_ସଂଖ୍ୟା = wp->w_width;
}
#endif // FEAT_EVAL

#ଯଦି defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
/*
 * Set "win" to be the curwin and "tp" to be the current tab page.
 * restore_win() MUST be called to undo, also when FAIL is returned.
 * No autocommands will be executed until restore_win() is called.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered, another tabpage access is limited.
 * Returns FAIL if switching to "win" failed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
switch_win(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    no_display)
{
    block_autocmds();
    ଫେରନ୍ତୁ switch_win_noblock(switchwin, win, tp, no_display);
}

/*
 * As switch_win() but without blocking autocommands.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
switch_win_noblock(
	switchwin_T *switchwin,
	win_T	    *win,
	tabpage_T   *tp,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    no_display)
{
    CLEAR_POINTER(switchwin);
    switchwin->sw_curwin = curwin;
    ଯଦି (win == curwin)
	switchwin->sw_same_win = TRUE;
    ଅନ୍ୟଥା
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_active = VIsual_active;
	VIsual_active = FALSE;
    }

    ଯଦି (tp != ଖାଲି)
    {
	switchwin->sw_curtab = curtab;
	ଯଦି (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	ଅନ୍ୟଥା
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    ଯଦି (!win_valid(win))
	ଫେରନ୍ତୁ FAIL;
    curwin = win;
    curbuf = curwin->w_buffer;
    ଫେରନ୍ତୁ OK;
}

/*
 * Restore current tabpage and window saved by switch_win(), if still valid.
 * When "no_display" is TRUE the display won't be affected, no redraw is
 * triggered.
 */
    ଶୂନ୍ୟ
restore_win(
	switchwin_T *switchwin,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    no_display)
{
    restore_win_noblock(switchwin, no_display);
    unblock_autocmds();
}

/*
 * As restore_win() but without unblocking autocommands.
 */
    ଶୂନ୍ୟ
restore_win_noblock(
	switchwin_T *switchwin,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    no_display)
{
    ଯଦି (switchwin->sw_curtab != ଖାଲି && valid_tabpage(switchwin->sw_curtab))
    {
	ଯଦି (no_display)
	{
	    win_T	*old_tp_curwin = curtab->tp_curwin;

	    unuse_tabpage(curtab);
	    // Don't change the curwin of the tabpage we temporarily visited.
	    curtab->tp_curwin = old_tp_curwin;
	    use_tabpage(switchwin->sw_curtab);
	}
	ଅନ୍ୟଥା
	    goto_tabpage_tp(switchwin->sw_curtab, FALSE, FALSE);
    }

    ଯଦି (!switchwin->sw_same_win)
	VIsual_active = switchwin->sw_visual_active;

    ଯଦି (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_buffer;
    }
# ifdef FEAT_PROP_POPUP
    ଅନ୍ୟଥା ଯଦି (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#endif
