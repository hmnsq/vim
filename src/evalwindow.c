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

#إذا defined(FEAT_EVAL)

    ساكن صحيح
win_getid(typval_T *argvars)
{
    صحيح	    winnr;
    win_T   *wp;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
	أرجع curwin->w_id;
    winnr = tv_get_عدد(&argvars[0]);
    إذا (winnr <= 0)
	أرجع 0;

    إذا (argvars[1].v_type == VAR_UNKNOWN)
	wp = firstwin;
    وإلا
    {
	tabpage_T	*tp;
	صحيح		tabnr = tv_get_عدد(&argvars[1]);

	FOR_ALL_TABPAGES(tp)
	    إذا (--tabnr == 0)
		اكسر;
	إذا (tp == عدم)
	    أرجع -1;
	إذا (tp == curtab)
	    wp = firstwin;
	وإلا
	    wp = tp->tp_firstwin;
    }
    لكل ( ; wp != عدم; wp = wp->w_ايندڙ)
	إذا (--winnr == 0)
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
    صحيح		id = tv_get_عدد(&argvars[0]);

    FOR_ALL_TABPAGES(tp)
    {
	FOR_ALL_WINDOWS_اندريون_TAB(tp, wp)
	{
	    إذا (wp->w_id == id)
	    {
		list_append_عدد(list, tabnr);
		list_append_عدد(list, winnr);
		أرجع;
	    }
	    ++winnr;
	}
	++tabnr;
	winnr = 1;
    }
    list_append_عدد(list, 0);
    list_append_عدد(list, 0);
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
	إذا (wp->w_id == id)
	{
	    إذا (tpp != عدم)
		*tpp = tp;
	    أرجع wp;
	}
# ifdef FEAT_PROP_POPUP
    // popup windows are in separate lists
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_اندريون_TAB(tp, wp)
	     إذا (wp->w_id == id)
	     {
		 إذا (tpp != عدم)
		     *tpp = tp;
		 أرجع wp;
	     }
    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_id == id)
	{
	    إذا (tpp != عدم)
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
    صحيح	    id = tv_get_عدد(&argvars[0]);

    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_id == id)
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
    صحيح		bufnr = tv_get_عدد(&argvars[0]);

    FOR_ALL_TAB_WINDOWS(tp, wp)
	    إذا (wp->w_بفر->b_fnum == bufnr)
		list_append_عدد(list, wp->w_id);
}

/*
 * Find window specified by "vp" in tabpage "tp".
 * Returns current window if "vp" is number zero.
 * Returns NULL if not found.
 */
    win_T *
ڳوليو_win_by_nr(
    typval_T	*vp,
    tabpage_T	*tp)	// NULL for current tab page
{
    win_T	*wp;
    صحيح		nr = (صحيح)tv_get_عدد_chk(vp, عدم);

    إذا (nr < 0)
	أرجع عدم;
    إذا (nr == 0)
	أرجع curwin;

    FOR_ALL_WINDOWS_اندريون_TAB(tp, wp)
    {
	إذا (nr >= سڀ_کان_هيٺ_WIN_ID)
	{
	    إذا (wp->w_id == nr)
		أرجع wp;
	}
	وإلا إذا (--nr <= 0)
	    اكسر;
    }
    إذا (nr >= سڀ_کان_هيٺ_WIN_ID)
    {
# ifdef FEAT_PROP_POPUP
	// check tab-local popup windows
	لكل (wp = (tp == عدم ? curtab : tp)->tp_پهريون_popupwin;
						   wp != عدم; wp = wp->w_ايندڙ)
	    إذا (wp->w_id == nr)
		أرجع wp;
	// check global popup windows
	FOR_ALL_POPUPWINS(wp)
	    إذا (wp->w_id == nr)
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
ڳوليو_win_by_nr_or_id(typval_T *vp)
{
    صحيح	nr = (صحيح)tv_get_عدد_chk(vp, عدم);

    إذا (nr >= سڀ_کان_هيٺ_WIN_ID)
	أرجع win_id2wp(tv_get_عدد(vp));
    أرجع ڳوليو_win_by_nr(vp, عدم);
}

/*
 * Find window specified by "wvp" in tabpage "tvp".
 * Returns the tab page in 'ptp'
 */
    win_T *
ڳوليو_tabwin(
    typval_T	*wvp,	// VAR_UNKNOWN for current window
    typval_T	*tvp,	// VAR_UNKNOWN for current tab page
    tabpage_T	**ptp)
{
    win_T	*wp = عدم;
    tabpage_T	*tp = عدم;
    ڊگهو	n;

    إذا (wvp->v_type != VAR_UNKNOWN)
    {
	إذا (tvp->v_type != VAR_UNKNOWN)
	{
	    n = (ڊگهو)tv_get_عدد(tvp);
	    إذا (n >= 0)
		tp = ڳوليو_tabpage(n);
	}
	وإلا
	    tp = curtab;

	إذا (tp != عدم)
	{
	    wp = ڳوليو_win_by_nr(wvp, tp);
	    إذا (wp == عدم && wvp->v_type == VAR_عدد
						&& wvp->vval.v_عدد != -1)
		// A window with the specified number is not found
		tp = عدم;
	}
    }
    وإلا
    {
	wp = curwin;
	tp = curtab;
    }

    إذا (ptp != عدم)
	*ptp = tp;

    أرجع wp;
}

/*
 * Get the layout of the given tab page for winlayout() and add it to "l".
 */
    ساكن فراغ
get_framelayout(چوکٺو_T *fr, list_T *l, صحيح outer)
{
    چوکٺو_T	*ٻار;
    list_T	*fr_list;
    list_T	*win_list;

    إذا (fr == عدم)
	أرجع;

    إذا (outer)
	// outermost call from f_winlayout()
	fr_list = l;
    وإلا
    {
	fr_list = list_alloc();
	إذا (fr_list == عدم)
	    أرجع;
	إذا (list_append_list(l, fr_list) == FAIL)
	{
	    list_free(fr_list);
	    أرجع;
	}
    }

    إذا (fr->fr_layout == FR_پن)
    {
	إذا (fr->fr_win != عدم)
	{
	    list_append_string(fr_list, (char_u *)"leaf", STRLEN_LITERAL("leaf"));
	    list_append_عدد(fr_list, fr->fr_win->w_id);
	}
    }
    وإلا
    {
	إذا (fr->fr_layout == FR_صف)
	    list_append_string(fr_list, (char_u *)"row", STRLEN_LITERAL("row"));
	وإلا
	    list_append_string(fr_list, (char_u *)"col", STRLEN_LITERAL("col"));

	win_list = list_alloc();
	إذا (win_list == عدم)
	    أرجع;
	إذا (list_append_list(fr_list, win_list) == FAIL)
	{
	    list_free(win_list);
	    أرجع;
	}

	ٻار = fr->fr_ٻار;
	طالما (ٻار != عدم)
	{
	    get_framelayout(ٻار, win_list, FALSE);
	    ٻار = ٻار->fr_ايندڙ;
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
    إذا (argvar->v_type != VAR_UNKNOWN)
    {
	صحيح	invalid_arg = FALSE;

	arg = tv_get_string_chk(argvar);
	إذا (arg == عدم)
	    nr = 0;		// type error; errmsg already given
	وإلا إذا (STRCMP(arg, "$") == 0)
	    twin = (tp == curtab) ? lastwin : tp->tp_lastwin;
	وإلا إذا (STRCMP(arg, "#") == 0)
	{
	    twin = (tp == curtab) ? prevwin : tp->tp_prevwin;
	}
	وإلا
	{
	    ڊگهو	ڳڻپ;
	    char_u	*endp;

	    // Extract the window count (if specified). e.g. winnr('3j')
	    ڳڻپ = strtol((حرف *)arg, (حرف **)&endp, 10);
	    إذا (ڳڻپ <= 0)
		ڳڻپ = 1;	// if count is not specified, default to 1
	    إذا (endp != عدم && *endp != '\0')
	    {
		إذا (STRCMP(endp, "j") == 0)
		    twin = win_vert_neighbor(tp, twin, FALSE, ڳڻپ);
		وإلا إذا (STRCMP(endp, "k") == 0)
		    twin = win_vert_neighbor(tp, twin, TRUE, ڳڻپ);
		وإلا إذا (STRCMP(endp, "h") == 0)
		    twin = win_horz_neighbor(tp, twin, TRUE, ڳڻپ);
		وإلا إذا (STRCMP(endp, "l") == 0)
		    twin = win_horz_neighbor(tp, twin, FALSE, ڳڻپ);
		وإلا
		    invalid_arg = TRUE;
	    }
	    وإلا
		invalid_arg = TRUE;
	}
	إذا (twin == عدم)
	    nr = 0;

	إذا (invalid_arg)
	{
	    semsg(_(e_invalid_اظهار_str), arg);
	    nr = 0;
	}
    }

    إذا (nr <= 0)
	أرجع 0;

    لكل (wp = (tp == curtab) ? firstwin : tp->tp_firstwin;
	    wp != twin; wp = wp->w_ايندڙ)
    {
	إذا (wp == عدم)
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
get_win_info(win_T *wp, ننڍو tpnr, ننڍو winnr)
{
    dict_T	*dict;

    dict = dict_alloc();
    إذا (dict == عدم)
	أرجع عدم;

    // make sure w_botline is valid
    validate_botline_win(wp);

    dict_جوڙيو_عدد(dict, "tabnr", tpnr);
    dict_جوڙيو_عدد(dict, "winnr", winnr);
    dict_جوڙيو_عدد(dict, "winid", wp->w_id);
    dict_جوڙيو_عدد(dict, "height", wp->w_اوچائي);
    dict_جوڙيو_عدد(dict, "status_height", wp->w_حالت_اوچائي);
    dict_جوڙيو_عدد(dict, "winrow", wp->w_winrow + 1);
    dict_جوڙيو_عدد(dict, "topline", wp->w_topline);
    dict_جوڙيو_عدد(dict, "botline", wp->w_botline - 1);
# ifdef FEAT_MENU
    dict_جوڙيو_عدد(dict, "winbar", wp->w_winbar_اوچائي);
# endif
    dict_جوڙيو_عدد(dict, "width", wp->w_ويڪر);
    dict_جوڙيو_عدد(dict, "wincol", wp->w_wincol + 1);
    dict_جوڙيو_عدد(dict, "textoff", win_کانو_off(wp));
    dict_جوڙيو_عدد(dict, "bufnr", wp->w_بفر->b_fnum);
    dict_جوڙيو_عدد(dict, "leftcol", wp->w_leftcol);

# ifdef FEAT_TERMINAL
    dict_جوڙيو_عدد(dict, "terminal", bt_terminal(wp->w_بفر));
# endif
# ifdef FEAT_QUICKFIX
    dict_جوڙيو_عدد(dict, "quickfix", bt_quickfix(wp->w_بفر));
    dict_جوڙيو_عدد(dict, "loclist",
		      (bt_quickfix(wp->w_بفر) && wp->w_llist_ref != عدم));
# endif

    // Add a reference to window variables
    dict_جوڙيو_dict(dict, "variables", wp->w_vars);

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
    إذا (dict == عدم)
	أرجع عدم;

    dict_جوڙيو_عدد(dict, "tabnr", tp_idx);

    l = list_alloc();
    إذا (l != عدم)
    {
	FOR_ALL_WINDOWS_اندريون_TAB(tp, wp)
	    list_append_عدد(l, (varnumber_T)wp->w_id);
	dict_جوڙيو_list(dict, "windows", l);
    }

    // Make a reference to tabpage variables
    dict_جوڙيو_dict(dict, "variables", tp->tp_vars);

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

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (اندريون_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	// Information about one tab page
	tparg = ڳوليو_tabpage((صحيح)tv_get_عدد_chk(&argvars[0], عدم));
	إذا (tparg == عدم)
	    أرجع;
    }

    // Get information about a specific tab page or all tab pages
    FOR_ALL_TABPAGES(tp)
    {
	tpnr++;
	إذا (tparg != عدم && tp != tparg)
	    استمر;
	d = get_tabpage_info(tp, tpnr);
	إذا (d != عدم)
	    list_append_dict(rettv->vval.v_list, d);
	إذا (tparg != عدم)
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
    ننڍو	tabnr = 0, winnr;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (اندريون_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	wparg = win_id2wp(tv_get_عدد(&argvars[0]));
	إذا (wparg == عدم)
	    أرجع;
    }

    // Collect information about either all the windows across all the tab
    // pages or one particular window.
    FOR_ALL_TABPAGES(tp)
    {
	tabnr++;
	winnr = 0;
	FOR_ALL_WINDOWS_اندريون_TAB(tp, wp)
	{
	    winnr++;
	    إذا (wparg != عدم && wp != wparg)
		استمر;
	    d = get_win_info(wp, tabnr, winnr);
	    إذا (d != عدم)
		list_append_dict(rettv->vval.v_list, d);
	    إذا (wparg != عدم)
		// found information about a specific window
		أرجع;
	}
    }
# ifdef FEAT_PROP_POPUP
    إذا (wparg != عدم)
    {
	tabnr = 0;
	FOR_ALL_TABPAGES(tp)
	{
	    tabnr++;
	    FOR_ALL_POPUPWINS_اندريون_TAB(tp, wp)
		إذا (wp == wparg)
		    اذهب مليو;
	}
مليو:
	d = get_win_info(wparg, tp == عدم ? 0 : tabnr, 0);
	إذا (d != عدم)
	    list_append_dict(rettv->vval.v_list, d);
    }
# endif
}

/*
 * "getwinpos({timeout})" function
 */
    فراغ
f_getwinpos(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    صحيح x = -1;
    صحيح y = -1;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (اندريون_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	أرجع;

# إذا defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	varnumber_T timeout = 100;

	إذا (argvars[0].v_type != VAR_UNKNOWN)
	    timeout = tv_get_عدد(&argvars[0]);

	(فراغ)ui_get_winpos(&x, &y, timeout);
    }
# endif
    list_append_عدد(rettv->vval.v_list, (varnumber_T)x);
    list_append_عدد(rettv->vval.v_list, (varnumber_T)y);
}


/*
 * "getwinposx()" function
 */
    فراغ
f_getwinposx(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;
# إذا defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)

    {
	صحيح	    x, y;

	إذا (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_عدد = x;
    }
# endif
}

/*
 * "getwinposy()" function
 */
    فراغ
f_getwinposy(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->vval.v_عدد = -1;
# إذا defined(FEAT_GUI) \
	|| (defined(HAVE_TGETENT) && defined(FEAT_TERMRESPONSE)) \
	|| defined(MSWIN)
    {
	صحيح	    x, y;

	إذا (ui_get_winpos(&x, &y, 100) == OK)
	    rettv->vval.v_عدد = y;
    }
# endif
}

/*
 * "tabpagenr()" function
 */
    فراغ
f_tabpagenr(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    صحيح		nr = 1;
    char_u	*arg;

    إذا (اندريون_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	arg = tv_get_string_chk(&argvars[0]);
	nr = 0;
	إذا (arg != عدم)
	{
	    إذا (STRCMP(arg, "$") == 0)
		nr = tabpage_اشاريو(عدم) - 1;
	    وإلا إذا (STRCMP(arg, "#") == 0)
		nr = valid_tabpage(lastused_tabpage) ?
					tabpage_اشاريو(lastused_tabpage) : 0;
	    وإلا
		semsg(_(e_invalid_اظهار_str), arg);
	}
    }
    وإلا
	nr = tabpage_اشاريو(curtab);
    rettv->vval.v_عدد = nr;
}

/*
 * "tabpagewinnr()" function
 */
    فراغ
f_tabpagewinnr(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    صحيح		nr = 1;
    tabpage_T	*tp;

    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    tp = ڳوليو_tabpage((صحيح)tv_get_عدد(&argvars[0]));
    إذا (tp == عدم)
	nr = 0;
    وإلا
	nr = get_winnr(tp, &argvars[1]);
    rettv->vval.v_عدد = nr;
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

    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	أرجع;

    id = (صحيح)tv_get_عدد(argvars);
    wp = win_id2wp_tp(id, &tp);
    إذا (wp == عدم || tp == عدم)
	أرجع;

    pos_T	curpos = wp->w_cursor;
    char_u	cwd[MAXPATHL];
    صحيح	cwd_حالت = FAIL;
# ifdef FEAT_AUTOCHDIR
    char_u	autocwd[MAXPATHL];
    صحيح	apply_acd = FALSE;
    char_u	*save_sfname = عدم;
# endif

    // Getting and setting directory can be slow on some systems, only do
    // this when the current or target window/tab have a local directory or
    // 'acd' is set.
    إذا (curwin != wp
	    && (curwin->w_localdir != عدم
		|| wp->w_localdir != عدم
		|| (curtab != tp
		    && (curtab->tp_localdir != عدم
			|| tp->tp_localdir != عدم))
# ifdef FEAT_AUTOCHDIR
		|| p_acd
# endif
	       ))
	cwd_حالت = mch_dirname(cwd, MAXPATHL);

# ifdef FEAT_AUTOCHDIR
    // If 'acd' is set, check we are using that directory.  If yes, then
    // apply 'acd' afterwards, otherwise restore the current directory.
    إذا (cwd_حالت == OK && p_acd)
    {
	إذا (curbuf->b_sfname != عدم && curbuf->b_fname == curbuf->b_sfname)
	    save_sfname = vim_strsave(curbuf->b_sfname);
	do_autochdir();
	apply_acd = mch_dirname(autocwd, MAXPATHL) == OK
	    && STRCMP(cwd, autocwd) == 0;
    }
# endif

    إذا (switch_win_noblock(&switchwin, wp, tp, TRUE) == OK)
    {
	check_cursor();
	execute_common(argvars, rettv, 1);
    }
    restore_win_noblock(&switchwin, TRUE);
# ifdef FEAT_AUTOCHDIR
    إذا (apply_acd)
    {
	vim_free(save_sfname);
	do_autochdir();
    }
    وإلا
# endif
	إذا (cwd_حالت == OK)
	{
	    mch_chdir((حرف *)cwd);
# ifdef FEAT_AUTOCHDIR
	    إذا (save_sfname != عدم)
	    {
		vim_free(curbuf->b_sfname);
		curbuf->b_sfname = save_sfname;
		curbuf->b_fname = curbuf->b_sfname;
	    }
# endif
	}

    // Update the status line if the cursor moved.
    إذا (win_valid(wp) && !EQUAL_POS(curpos, wp->w_cursor))
	wp->w_redr_حالت = true;

    // In case the command moved the cursor or changed the Visual area,
    // check it is valid.
    check_cursor();
    إذا (VIsual_سرگرم)
	check_pos(curbuf, &VIsual);
}

/*
 * "win_findbuf()" function
 */
    فراغ
f_win_findbuf(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (rettv_list_alloc(rettv) == OK)
	win_findbuf(argvars, rettv->vval.v_list);
}

/*
 * "win_getid()" function
 */
    فراغ
f_win_getid(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script()
	    && (check_for_opt_عدد_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 1) == FAIL)))
	أرجع;

    rettv->vval.v_عدد = win_getid(argvars);
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

    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    id = tv_get_عدد(&argvars[0]);
    إذا (curwin->w_id == id)
    {
	// Nothing to do.
	rettv->vval.v_عدد = 1;
	أرجع;
    }

    إذا (متن_or_buf_locked())
	أرجع;
# إذا defined(FEAT_PROP_POPUP) && defined(FEAT_TERMINAL)
    إذا (popup_is_popup(curwin) && curbuf->b_رڪن != عدم)
    {
	emsg(_(e_not_allowed_for_terminal_اندريون_popup_دري));
	أرجع;
    }
# endif
    FOR_ALL_TAB_WINDOWS(tp, wp)
	إذا (wp->w_id == id)
	{
	    // When jumping to another buffer stop Visual mode.
	    إذا (VIsual_سرگرم && wp->w_بفر != curbuf)
		پڇاڙي_visual_گهڻائي();
	    goto_tabpage_win(tp, wp);
	    rettv->vval.v_عدد = 1;
	    أرجع;
	}
}

/*
 * "win_id2tabwin()" function
 */
    فراغ
f_win_id2tabwin(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (rettv_list_alloc(rettv) == OK)
	win_id2tabwin(argvars, rettv->vval.v_list);
}

/*
 * "win_id2win()" function
 */
    فراغ
f_win_id2win(typval_T *argvars, typval_T *rettv)
{
    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    rettv->vval.v_عدد = win_id2win(argvars);
}

/*
 * "win_move_separator()" function
 */
    فراغ
f_win_move_separator(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحيح		وٿي;

    rettv->vval.v_عدد = FALSE;

    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    wp = ڳوليو_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم || win_valid_popup(wp))
	أرجع;
    إذا (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_دري_اندريون_another_tab_صفحو));
	أرجع;
    }

    وٿي = (صحيح)tv_get_عدد(&argvars[1]);
    win_drag_vsep_سٽ(wp, وٿي);
    rettv->vval.v_عدد = TRUE;
}

/*
 * "win_move_statusline()" function
 */
    فراغ
f_win_move_statusline(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;
    صحيح		وٿي;

    rettv->vval.v_عدد = FALSE;

    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    wp = ڳوليو_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم || win_valid_popup(wp))
	أرجع;
    إذا (!win_valid(wp))
    {
	emsg(_(e_cannot_resize_دري_اندريون_another_tab_صفحو));
	أرجع;
    }

    وٿي = (صحيح)tv_get_عدد(&argvars[1]);
    win_drag_حالت_سٽ(wp, وٿي);
    rettv->vval.v_عدد = TRUE;
}

/*
 * "win_screenpos()" function
 */
    فراغ
f_win_screenpos(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    wp = ڳوليو_win_by_nr_or_id(&argvars[0]);
    list_append_عدد(rettv->vval.v_list, wp == عدم ? 0 : wp->w_winrow + 1);
    list_append_عدد(rettv->vval.v_list, wp == عدم ? 0 : wp->w_wincol + 1);
}

/*
 * "win_splitmove()" function
 */
    فراغ
f_win_splitmove(typval_T *argvars, typval_T *rettv)
{
    win_T   *wp, *targetwin;
    win_T   *oldwin = curwin;
    صحيح     flags = 0, سائيز = 0;

    rettv->vval.v_عدد = -1;

    إذا (اندريون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    wp = ڳوليو_win_by_nr_or_id(&argvars[0]);
    targetwin = ڳوليو_win_by_nr_or_id(&argvars[1]);

    إذا (wp == عدم || targetwin == عدم || wp == targetwin
	    || !win_valid(wp) || !win_valid(targetwin)
	    || win_valid_popup(wp) || win_valid_popup(targetwin))
    {
	emsg(_(e_invalid_دري_عدد));
	أرجع;
    }

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	dict_T      *d;
	dictitem_T  *di;

	إذا (check_for_nonnull_dict_arg(argvars, 2) == FAIL)
	    أرجع;

	d = argvars[2].vval.v_dict;
	إذا (dict_get_bool(d, "vertical", FALSE))
	    flags |= WSP_VERT;
	إذا ((di = dict_ڳوليو(d, (char_u *)"rightbelow", -1)) != عدم)
	    flags |= tv_get_bool(&di->di_tv) ? WSP_هيٺ : WSP_ABOVE;
	سائيز = (صحيح)dict_get_عدد(d, "size");
    }

    // Check if we're allowed to continue before we bother switching windows.
    إذا (متن_or_buf_locked() || check_ورهايو_disallowed(wp) == FAIL)
	أرجع;

    إذا (curwin != targetwin)
	win_goto(targetwin);

    // Autocommands may have sent us elsewhere or closed "wp" or "oldwin".
    إذا (curwin == targetwin && win_valid(wp))
    {
	إذا (win_splitmove(wp, سائيز, flags) == OK)
	    rettv->vval.v_عدد = 0;
    }
    وإلا
	emsg(_(e_autocommands_caused_command_منزل_abort));

    إذا (oldwin != curwin && win_valid(oldwin))
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

    إذا (اندريون_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	wp = ڳوليو_win_by_nr_or_id(&argvars[0]);
	إذا (wp == عدم)
	{
	    rettv->vval.v_string = vim_strsave((char_u *)"unknown");
	    أرجع;
	}
    }
    إذا (is_aucmd_win(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"autocmd");
# إذا defined(FEAT_QUICKFIX)
    وإلا إذا (wp->w_p_pvw)
	rettv->vval.v_string = vim_strsave((char_u *)"preview");
# endif
# ifdef FEAT_PROP_POPUP
    وإلا إذا (WIN_IS_POPUP(wp))
	rettv->vval.v_string = vim_strsave((char_u *)"popup");
# endif
    وإلا إذا (wp == cmdwin_win)
	rettv->vval.v_string = vim_strsave((char_u *)"command");
# ifdef FEAT_QUICKFIX
    وإلا إذا (bt_quickfix(wp->w_بفر))
	rettv->vval.v_string = vim_strsave((char_u *)
		(wp->w_llist_ref != عدم ? "loclist" : "quickfix"));
# endif

}

/*
 * "getcmdwintype()" function
 */
    فراغ
f_getcmdwintype(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;
    rettv->vval.v_string = alloc(2);
    إذا (rettv->vval.v_string == عدم)
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

    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    wp = ڳوليو_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم)
	rettv->vval.v_عدد = -1;
    وإلا
	rettv->vval.v_عدد = wp->w_بفر->b_fnum;
}

/*
 * "wincol()" function
 */
    فراغ
f_wincol(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    validate_cursor();
    صحيح کانو = curwin->w_wcol + 1;
# ifdef FEAT_RIGHTLEFT
    إذا (curwin->w_p_rl)
	کانو = curwin->w_ويڪر - curwin->w_wcol - cursor_screen_cells() + 1;
# endif
    rettv->vval.v_عدد = کانو;
}

/*
 * "winheight(nr)" function
 */
    فراغ
f_winheight(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    wp = ڳوليو_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم)
	rettv->vval.v_عدد = -1;
    وإلا
	rettv->vval.v_عدد = wp->w_اوچائي;
}

/*
 * "winlayout()" function
 */
    فراغ
f_winlayout(typval_T *argvars, typval_T *rettv)
{
    tabpage_T	*tp;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (اندريون_vim9script() && check_for_opt_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type == VAR_UNKNOWN)
	tp = curtab;
    وإلا
    {
	tp = ڳوليو_tabpage((صحيح)tv_get_عدد(&argvars[0]));
	إذا (tp == عدم)
	    أرجع;
    }

    get_framelayout(tp->tp_topframe, rettv->vval.v_list, TRUE);
}

/*
 * "winline()" function
 */
    فراغ
f_winline(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    validate_cursor();
    rettv->vval.v_عدد = curwin->w_wrow + 1;
}

/*
 * "winnr()" function
 */
    فراغ
f_winnr(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    صحيح		nr = 1;

    إذا (اندريون_vim9script() && check_for_opt_string_arg(argvars, 0) == FAIL)
	أرجع;

    nr = get_winnr(curtab, &argvars[0]);
    rettv->vval.v_عدد = nr;
}

/*
 * "winrestcmd()" function
 */
    فراغ
f_winrestcmd(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
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
		":%dresize %d|", winnr, wp->w_اوچائي);
	    ga_concat_len(&ga, buf, buflen);
	    buflen = vim_snprintf_safelen((حرف *)buf, حجم(buf),
		"vert :%dresize %d|", winnr, wp->w_ويڪر);
	    ga_concat_len(&ga, buf, buflen);
	    ++winnr;
	}
    }
    ga_append(&ga, NUL);

    rettv->vval.v_string = ga.ga_معلومات;
    rettv->v_type = VAR_STRING;
}

/*
 * "winrestview()" function
 */
    فراغ
f_winrestview(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    dict_T	*dict;

    إذا (check_for_nonnull_dict_arg(argvars, 0) == FAIL)
	أرجع;

    dict = argvars[0].vval.v_dict;
    إذا (dict_has_ڪنجي(dict, "lnum"))
	curwin->w_cursor.lnum = (linenr_T)dict_get_عدد(dict, "lnum");
    إذا (dict_has_ڪنجي(dict, "col"))
	curwin->w_cursor.کانو = (colnr_T)dict_get_عدد(dict, "col");
    إذا (dict_has_ڪنجي(dict, "coladd"))
	curwin->w_cursor.coladd = (colnr_T)dict_get_عدد(dict, "coladd");
    إذا (dict_has_ڪنجي(dict, "curswant"))
    {
	curwin->w_curswant = (colnr_T)dict_get_عدد(dict, "curswant");
	curwin->w_سيٽ_curswant = false;
    }

    إذا (dict_has_ڪنجي(dict, "topline"))
	سيٽ_topline(curwin, (linenr_T)dict_get_عدد(dict, "topline"));
# ifdef FEAT_DIFF
    إذا (dict_has_ڪنجي(dict, "topfill"))
	curwin->w_topfill = (صحيح)dict_get_عدد(dict, "topfill");
# endif
    إذا (dict_has_ڪنجي(dict, "leftcol"))
	curwin->w_leftcol = (colnr_T)dict_get_عدد(dict, "leftcol");
    إذا (dict_has_ڪنجي(dict, "skipcol"))
	curwin->w_skipcol = (colnr_T)dict_get_عدد(dict, "skipcol");

    check_cursor();
    win_new_اوچائي(curwin, curwin->w_اوچائي);
    win_new_ويڪر(curwin, curwin->w_ويڪر);
    بدليو_دري_setting();

    إذا (curwin->w_topline <= 0)
	curwin->w_topline = 1;
    إذا (curwin->w_topline > curbuf->b_ml.ml_سٽ_ڳڻپ)
	curwin->w_topline = curbuf->b_ml.ml_سٽ_ڳڻپ;
# ifdef FEAT_DIFF
    check_topfill(curwin, TRUE);
# endif
}

/*
 * "winsaveview()" function
 */
    فراغ
f_winsaveview(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    dict_T	*dict;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;
    dict = rettv->vval.v_dict;

    dict_جوڙيو_عدد(dict, "lnum", (ڊگهو)curwin->w_cursor.lnum);
    dict_جوڙيو_عدد(dict, "col", (ڊگهو)curwin->w_cursor.کانو);
    dict_جوڙيو_عدد(dict, "coladd", (ڊگهو)curwin->w_cursor.coladd);
    update_curswant();
    dict_جوڙيو_عدد(dict, "curswant", (ڊگهو)curwin->w_curswant);

    dict_جوڙيو_عدد(dict, "topline", (ڊگهو)curwin->w_topline);
# ifdef FEAT_DIFF
    dict_جوڙيو_عدد(dict, "topfill", (ڊگهو)curwin->w_topfill);
# endif
    dict_جوڙيو_عدد(dict, "leftcol", (ڊگهو)curwin->w_leftcol);
    dict_جوڙيو_عدد(dict, "skipcol", (ڊگهو)curwin->w_skipcol);
}

/*
 * "winwidth(nr)" function
 */
    فراغ
f_winwidth(typval_T *argvars, typval_T *rettv)
{
    win_T	*wp;

    إذا (اندريون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    wp = ڳوليو_win_by_nr_or_id(&argvars[0]);
    إذا (wp == عدم)
	rettv->vval.v_عدد = -1;
    وإلا
	rettv->vval.v_عدد = wp->w_ويڪر;
}
#جيڪڏهن_پورو // FEAT_EVAL

#إذا defined(FEAT_EVAL) || defined(FEAT_PYTHON) || defined(FEAT_PYTHON3)
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
    بلاڪ_autocmds();
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
    CLEAR_اشارو(switchwin);
    switchwin->sw_curwin = curwin;
    إذا (win == curwin)
	switchwin->sw_same_win = TRUE;
    وإلا
    {
	// Disable Visual selection, because redrawing may fail.
	switchwin->sw_visual_سرگرم = VIsual_سرگرم;
	VIsual_سرگرم = FALSE;
    }

    إذا (tp != عدم)
    {
	switchwin->sw_curtab = curtab;
	إذا (no_display)
	{
	    unuse_tabpage(curtab);
	    use_tabpage(tp);
	}
	وإلا
	    goto_tabpage_tp(tp, FALSE, FALSE);
    }
    إذا (!win_valid(win))
	أرجع FAIL;
    curwin = win;
    curbuf = curwin->w_بفر;
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
    إذا (switchwin->sw_curtab != عدم && valid_tabpage(switchwin->sw_curtab))
    {
	إذا (no_display)
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

    إذا (!switchwin->sw_same_win)
	VIsual_سرگرم = switchwin->sw_visual_سرگرم;

    إذا (win_valid(switchwin->sw_curwin))
    {
	curwin = switchwin->sw_curwin;
	curbuf = curwin->w_بفر;
    }
# ifdef FEAT_PROP_POPUP
    وإلا إذا (WIN_IS_POPUP(curwin))
	// original window was closed and now we're in a popup window: Go
	// to the first valid window.
	win_goto(firstwin);
# endif
}
#جيڪڏهن_پورو
