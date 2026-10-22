/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Implementation of popup windows.  See ":help popup".
 */

#ضمن "vim.h"

#إذا defined(FEAT_PROP_POPUP)

تعريفنوع بنية {
    string_T	pp_name;
    poppos_T	pp_val;
} poppos_مدخل_T;

// Snapshot of the popup's drawn rectangle.  Used to redraw what becomes
// exposed when the popup moves, resizes, hides or closes.  "active" is TRUE
// when the popup is an opacity popup that contributes to the blended
// background; non-opacity popups rely on popup_mask instead.
تعريفنوع بنية {
    صحيح		فعال;
    صحيح		winrow;
    صحيح		wincol;
    صحيح		ارتفاع;
    صحيح		عرض;
    صحيح		leftoff;
    صحيح		zindex;
} popup_area_T;

// Snapshot of the screen cells under an opacity popup's padding, captured
// before win_update() overwrites them so the padding can be re-blended
// against the original background.  "lines" is NULL when no snapshot was
// taken; popup_free_saved_screen() releases all owned buffers.
تعريفنوع بنية {
    schar_T	*سطرها;
    صحيح		*attrs;
    u8char_T	*linesuc;
    صحيح		شروع_ردیف;
    صحيح		شروع_ستون;
    صحيح		ردیفها_2;
    صحيح		تعداد_ستون;
} popup_saved_screen_T;

// Snapshot of popup style/option fields used by popup_setoptions() to detect
// which option changes require a redraw or reposition.
تعريفنوع بنية {
    linenr_T	firstline;
    صحيح		blend;
    صحيح		flags;
    صحيح		zindex;
    char_u	*title;
    char_u	*scrollbar_highlight;
    char_u	*thumb_highlight;
    char_u	*border_highlight[4];
} popup_style_snapshot_T;

// Snapshot of popup layout fields used by popup_adjust_position() to detect
// whether the position or size changed and the popup mask must be refreshed.
تعريفنوع بنية {
    صحيح		winrow;
    صحيح		wincol;
    صحيح		عرض;
    صحيح		ارتفاع;
    صحيح		leftcol;
    صحيح		leftoff;
    صحيح		has_scrollbar;
    صحيح		topoff;
    صحيح		bottomoff;
    صحيح		leftclip;
    صحيح		rightclip;
} popup_layout_T;

ساكن poppos_مدخل_T poppos_entries[] = {
    {STR_LITERAL_INIT("botleft"), POPPOS_BOTLEFT},
    {STR_LITERAL_INIT("topleft"), POPPOS_TOPLEFT},
    {STR_LITERAL_INIT("botright"), POPPOS_BOTRIGHT},
    {STR_LITERAL_INIT("topright"), POPPOS_TOPRIGHT},
    {STR_LITERAL_INIT("center"), POPPOS_CENTER}
};

#اگر_تعریف_شده HAS_پیام_پنجره
// Window used for ":echowindow"
ساكن win_T *پیام_win_2 = عدم;

// Time used for the next ":echowindow" message in msec.
ساكن صحيح  پیام_win_زمان = 3000;

// Flag set when a message is added to the message window, timer is started
// when the message window is drawn.  This might be after pressing Enter at the
// hit-enter prompt.
ساكن صحيح    شروع_پیام_win_timer = FALSE;

ساكن فراغ may_شروع_پیام_win_timer(win_T *wp);
#پایان_اگر

ساكن صحيح popup_on_cmdline = FALSE;

ساكن فراغ popup_adjust_position(win_T *wp);
ساكن bool popup_area_تغییریافته(win_T *wp, popup_area_T *area);
ساكن فراغ popup_redraw_exposed_area(popup_area_T *area);
ساكن فراغ popup_save_area(win_T *wp, popup_area_T *area);
ساكن فراغ popup_free_saved_screen(popup_saved_screen_T *saved_screen);
ساكن فراغ popup_save_padding_screen(win_T *wp,
	popup_saved_screen_T *saved_screen);
ساكن bool popup_layout_تغییریافته(win_T *wp, popup_layout_T *layout);
ساكن bool popup_style_تغییریافته(win_T *wp, popup_style_snapshot_T *style);
ساكن فراغ popup_save_style(win_T *wp, popup_style_snapshot_T *style);
ساكن فراغ popup_save_layout(win_T *wp, popup_layout_T *layout);
ساكن فراغ redraw_under_popup_area(صحيح winrow, صحيح wincol, صحيح ارتفاع,
	صحيح عرض, صحيح leftoff);
ساكن فراغ redraw_overlapped_opacity_popups(صحيح winrow, صحيح wincol,
	صحيح ارتفاع, صحيح عرض, صحيح leftoff, صحيح zindex);
ساكن فراغ redraw_win_under_opacity_popup(win_T *wp);

/*
 * Get option value for "key", which is "line" or "col".
 * Handles "cursor+N" and "cursor-N".
 * Returns MAXCOL if the entry is not present.
 */
    ساكن صحيح
popup_options_one(dict_T *dict, char_u *کلید)
{
    dictitem_T	*di;
    char_u	*val;
    char_u	*s;
    char_u	*endp;
    صحيح		n = 0;

    di = dict_پیدا_کن(dict, کلید, -1);
    إذا (di == عدم)
	أرجع MAXCOL;

    val = tv_get_string(&di->di_tv);
    إذا (STRNCMP(val, "cursor", 6) != 0)
	أرجع dict_get_عدد_check(dict, کلید);

    setcursor_mayforce(TRUE);
    s = val + 6;
    إذا (*s != NUL)
    {
	endp = s;
	إذا (*skipwhite(s) == '+' || *skipwhite(s) == '-')
	    n = strtol((حرف *)s, (حرف **)&endp, 10);
	إذا (endp != عدم && *skipwhite(endp) != NUL)
	{
	    semsg(_(e_invalid_عبارت_str), val);
	    أرجع 0;
	}
    }

    إذا (STRCMP(کلید, "line") == 0)
	n = screen_screenrow() + 1 + n;
    وإلا // "col"
	n = screen_screencol() + 1 + n;

    // Zero means "not set", use -1 instead.
    إذا (n == 0)
	n = -1;
    أرجع n;
}

    ساكن صحيح
مجموعه_padding_border(dict_T *dict, صحيح *array, حرف *name, صحيح max_val)
{
    dictitem_T	*di;

    di = dict_پیدا_کن(dict, (char_u *)name, -1);
    إذا (di == عدم)
	أرجع OK;

    إذا (di->di_tv.v_type != VAR_LIST)
    {
	emsg(_(e_list_required));
	أرجع FAIL;
    }

    list_T	*list = di->di_tv.vval.v_list;
    listitem_T	*li;
    صحيح		i;
    صحيح		nr;

    لكل (i = 0; i < 4; ++i)
	array[i] = 1;
    إذا (list == عدم)
	أرجع OK;

    CHECK_LIST_MATERIALIZE(list);
    لكل (i = 0, li = list->lv_اولی; i < 4 && i < list->lv_len;
	    ++i, li = li->li_بعدی)
    {
	nr = (صحيح)tv_get_عدد(&li->li_tv);
	إذا (nr >= 0)
	    array[i] = nr > max_val ? max_val : nr;
    }

    أرجع OK;
}

/*
 * Used when popup options contain "moved": set default moved values.
 */
    ساكن فراغ
مجموعه_moved_مقادیر(win_T *wp)
{
    wp->w_popup_curwin = curwin;
    wp->w_popup_lnum = curwin->w_cursor.lnum;
    wp->w_popup_mincol = curwin->w_cursor.ستون;
    wp->w_popup_maxcol = curwin->w_cursor.ستون;
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ساكن فراغ
مجموعه_moved_columns(win_T *wp, صحيح flags)
{
    char_u	*ptr;
    صحيح		len = پیدا_کن_ident_under_cursor(&ptr, flags | پیدا_کن_NOERROR);
    صحيح		mincol;

    إذا (len <= 0)
	أرجع;

    // When the cursor is on white space find_ident_under_cursor() skips
    // forward to the next word, whose range does not cover the cursor column.
    // Keep the cursor column then, otherwise popup_check_cursor_pos() would
    // close the popup right away.
    mincol = (صحيح)(ptr - ml_get_curline());
    إذا (curwin->w_cursor.ستون < mincol)
    {
	wp->w_popup_mincol = curwin->w_cursor.ستون;
	wp->w_popup_maxcol = curwin->w_cursor.ستون;
	أرجع;
    }

    wp->w_popup_mincol = mincol;
    wp->w_popup_maxcol = mincol + len - 1;
}

/*
 * Used when popup options contain "mousemoved": set default moved values.
 */
    ساكن فراغ
مجموعه_mousemoved_مقادیر(win_T *wp)
{
    wp->w_popup_mouse_ردیف = mouse_ردیف;
    wp->w_popup_mouse_mincol = mouse_ستون;
    wp->w_popup_mouse_maxcol = mouse_ستون;
}

    ساكن فراغ
update_popup_استفاده_میکند_mouse_move(فراغ)
{
    popup_استفاده_میکند_mouse_move = FALSE;
    إذا (!popup_visible)
	أرجع;

    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_popup_mouse_ردیف != 0)
	{
	    popup_استفاده_میکند_mouse_move = TRUE;
	    أرجع;
	}
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	إذا (wp->w_popup_mouse_ردیف != 0)
	{
	    popup_استفاده_میکند_mouse_move = TRUE;
	    أرجع;
	}
}

/*
 * Used when popup options contain "moved" with "word" or "WORD".
 */
    ساكن فراغ
مجموعه_mousemoved_columns(win_T *wp, صحيح flags)
{
    win_T	*textwp;
    char_u	*متن;
    صحيح		ستون;
    pos_T	pos;
    colnr_T	mcol;

    إذا (پیدا_کن_کلمه_under_cursor(mouse_ردیف, mouse_ستون, TRUE, flags,
				  &textwp, &pos.lnum, &متن, عدم, &ستون) != OK)
	أرجع;

    // convert text column to mouse column
    pos.ستون = ستون;
    pos.coladd = 0;
    getvcol(textwp, &pos, &mcol, عدم, عدم, 0);
    wp->w_popup_mouse_mincol = mcol;

    pos.ستون = ستون + (colnr_T)STRLEN(متن) - 1;
    getvcol(textwp, &pos, عدم, عدم, &mcol, 0);
    wp->w_popup_mouse_maxcol = mcol;
    vim_free(متن);
}

/*
 * Return TRUE if "row"/"col" is on the border of the popup.
 * The values are relative to the top-left corner.
 */
    صحيح
popup_on_border(win_T *wp, صحيح ردیف, صحيح ستون)
{
    أرجع (ردیف == 0 && wp->w_popup_border[0] > 0)
	    || (ردیف == popup_ارتفاع(wp) - 1 && wp->w_popup_border[2] > 0)
	    || (ستون == 0 && wp->w_popup_border[3] > 0)
	    || (ستون == popup_عرض(wp) - 1 && wp->w_popup_border[1] > 0);
}

/*
 * Return TRUE and close the popup if "row"/"col" is on the "X" button of the
 * popup and w_popup_close is POPCLOSE_BUTTON.
 * The values are relative to the top-left corner.
 * Caller should check the left mouse button was clicked.
 * Return TRUE if the popup was closed.
 */
    صحيح
popup_close_if_on_X(win_T *wp, صحيح ردیف, صحيح ستون)
{
    إذا (wp->w_popup_close == POPCLOSE_BUTTON
	    && ردیف == 0 && ستون == popup_عرض(wp) - 1)
    {
	popup_close_for_mouse_click(wp);
	أرجع TRUE;
    }
    أرجع FALSE;
}

// Values set when dragging a popup window starts.
ساكن صحيح drag_شروع_ردیف;
ساكن صحيح drag_شروع_ستون;
ساكن صحيح drag_شروع_wantline;
ساكن صحيح drag_شروع_wantcol;
ساكن صحيح drag_on_resize_handle;

/*
 * Mouse down on border of popup window: start dragging it.
 * Uses mouse_col and mouse_row.
 */
    فراغ
popup_شروع_drag(win_T *wp, صحيح ردیف, صحيح ستون)
{
    drag_شروع_ردیف = mouse_ردیف;
    drag_شروع_ستون = mouse_ستون;
    إذا (wp->w_wantline <= 0)
	drag_شروع_wantline = wp->w_winrow + 1;
    وإلا
	drag_شروع_wantline = wp->w_wantline;
    إذا (wp->w_wantcol == 0)
	drag_شروع_wantcol = wp->w_wincol + 1;
    وإلا
	drag_شروع_wantcol = wp->w_wantcol;

    // Stop centering the popup
    إذا (wp->w_popup_pos == POPPOS_CENTER)
	wp->w_popup_pos = POPPOS_TOPLEFT;

    drag_on_resize_handle = wp->w_popup_border[1] > 0
			    && wp->w_popup_border[2] > 0
			    && ردیف == popup_ارتفاع(wp) - 1
			    && ستون == popup_عرض(wp) - 1;

    إذا (wp->w_popup_pos != POPPOS_TOPLEFT && drag_on_resize_handle)
    {
	إذا (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_BOTRIGHT)
	    wp->w_wantcol = wp->w_wincol + 1;
	إذا (wp->w_popup_pos == POPPOS_BOTLEFT)
	    wp->w_wantline = wp->w_winrow + 1;
	wp->w_popup_pos = POPPOS_TOPLEFT;
    }
}

/*
 * Mouse moved while dragging a popup window: adjust the window popup position
 * or resize.
 */
    فراغ
popup_drag(win_T *wp)
{
    // The popup may be closed before dragging stops.
    إذا (!win_valid_popup(wp))
	أرجع;

    إذا ((wp->w_popup_flags & POPF_RESIZE) && drag_on_resize_handle)
    {
	صحيح عرض_inc = mouse_ستون - drag_شروع_ستون;
	صحيح ارتفاع_inc = mouse_ردیف - drag_شروع_ردیف;

	إذا (عرض_inc != 0)
	{
	    صحيح عرض = wp->w_عرض + عرض_inc;

	    إذا (عرض < 1)
		عرض = 1;
	    wp->w_minwidth = عرض;
	    wp->w_maxwidth = عرض;
	    drag_شروع_ستون = mouse_ستون;
	}

	إذا (ارتفاع_inc != 0)
	{
	    صحيح ارتفاع = wp->w_ارتفاع + ارتفاع_inc;

	    إذا (ارتفاع < 1)
		ارتفاع = 1;
	    wp->w_minheight = ارتفاع;
	    wp->w_maxheight = ارتفاع;
	    drag_شروع_ردیف = mouse_ردیف;
	}

	popup_adjust_position(wp);
	أرجع;
    }

    إذا (!(wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL)))
	أرجع;
    wp->w_wantline = drag_شروع_wantline + (mouse_ردیف - drag_شروع_ردیف);
    إذا (wp->w_wantline < 1)
	wp->w_wantline = 1;
    إذا (wp->w_wantline > ردیفها)
	wp->w_wantline = ردیفها;
    wp->w_wantcol = drag_شروع_wantcol + (mouse_ستون - drag_شروع_ستون);
    إذا (wp->w_wantcol < 1)
	wp->w_wantcol = 1;
    إذا (wp->w_wantcol > Columns)
	wp->w_wantcol = Columns;

    popup_adjust_position(wp);
}

/*
 * Set w_firstline to match the current "wp->w_topline".
 */
    فراغ
popup_مجموعه_firstline(win_T *wp)
{
    صحيح	    ارتفاع = wp->w_ارتفاع;

    wp->w_firstline = wp->w_topline;
    popup_adjust_position(wp);

    // we don't want the popup to get smaller, decrement the first line
    // until it doesn't
    طالما (wp->w_firstline > 1 && wp->w_ارتفاع < ارتفاع)
    {
	--wp->w_firstline;
	popup_adjust_position(wp);
    }
}

/*
 * Return TRUE if the position is in the popup window scrollbar.
 */
    صحيح
popup_is_درون_scrollbar(win_T *wp, صحيح ردیف, صحيح ستون)
{
    أرجع wp->w_has_scrollbar
	&& ردیف >= wp->w_popup_border[0]
	&& ردیف < popup_ارتفاع(wp) - wp->w_popup_border[2]
	&& ستون == popup_عرض(wp) - wp->w_popup_border[1] - 1;
}


/*
 * Handle a click in a popup window, if it is in the scrollbar.
 */
    فراغ
popup_handle_scrollbar_click(win_T *wp, صحيح ردیف, صحيح ستون)
{
    إذا (!popup_is_درون_scrollbar(wp, ردیف, ستون))
	أرجع;

    صحيح	    ارتفاع = popup_ارتفاع(wp);
    صحيح	    new_topline = wp->w_topline;

    إذا (ردیف >= ارتفاع / 2)
    {
	// Click in lower half, scroll down.
	إذا (wp->w_topline < wp->w_میانگیر->b_ml.ml_سطر_شمار)
	    ++new_topline;
    }
    وإلا إذا (wp->w_topline > 1)
	// click on upper half, scroll up.
	--new_topline;

    إذا (new_topline == wp->w_topline)
	أرجع;

    مجموعه_topline(wp, new_topline);
    إذا (wp == curwin)
    {
	إذا (wp->w_cursor.lnum < wp->w_topline)
	{
	    wp->w_cursor.lnum = wp->w_topline;
	    check_cursor();
	}
	وإلا إذا (wp->w_cursor.lnum >= wp->w_botline)
	{
	    wp->w_cursor.lnum = wp->w_botline - 1;
	    check_cursor();
	}
    }
    popup_مجموعه_firstline(wp);
    redraw_win_later(wp, UPD_NOT_VALID);
}

#إذا defined(FEAT_TIMERS)
/*
 * Add a timer to "wp" with "time".
 * If "close" is true use popup_close(), otherwise popup_hide().
 */
    ساكن فراغ
popup_جمع_کن_timeout(win_T *wp, صحيح زمان, صحيح close)
{
    char_u	    cbbuf[50];
    char_u	    *ptr = cbbuf;
    typval_T	    tv;

    vim_snprintf((حرف *)cbbuf, حجم(cbbuf),
		close ? "(_) => popup_close(%d)" : "(_) => popup_hide(%d)",
		wp->w_id);
    إذا (get_lambda_tv_and_compile(&ptr, &tv, FALSE, &EVALARG_EVALUATE) != OK)
	أرجع;

    wp->w_popup_timer = create_timer(زمان, 0);
    callback_T cb = get_callback(&tv);
    إذا (cb.cb_name != عدم && !cb.cb_free_name)
    {
	cb.cb_name = vim_strsave(cb.cb_name);
	cb.cb_free_name = TRUE;
    }
    wp->w_popup_timer->tr_callback = cb;
    clear_tv(&tv);
}
#پایان_اگر

    ساكن poppos_T
get_pos_مدخل(dict_T *d, صحيح give_error)
{
    char_u  *str = dict_get_string(d, "pos", FALSE);
    صحيح	    nr;

    إذا (str == عدم)
	أرجع POPPOS_NONE;

    لكل (nr = 0; nr < (صحيح)ARRAY_درازی(poppos_entries); ++nr)
	إذا (STRCMP(str, poppos_entries[nr].pp_name.string) == 0)
	    أرجع poppos_entries[nr].pp_val;

    إذا (give_error)
	semsg(_(e_invalid_آرگومان_str), str);
    أرجع POPPOS_NONE;
}

/*
 * Shared between popup_create() and f_popup_move().
 */
    ساكن فراغ
apply_move_options(win_T *wp, dict_T *d)
{
    صحيح		nr;
    char_u	*str;
    dictitem_T	*di;

    إذا ((nr = dict_get_عدد_def(d, "minwidth", -1)) >= 0)
	wp->w_minwidth = nr;
    إذا ((nr = dict_get_عدد_def(d, "minheight", -1)) >= 0)
	wp->w_minheight = nr;
    إذا ((nr = dict_get_عدد_def(d, "maxwidth", -1)) >= 0)
	wp->w_maxwidth = nr;
    إذا ((nr = dict_get_عدد_def(d, "maxheight", -1)) >= 0)
	wp->w_maxheight = nr;

    nr = popup_options_one(d, (char_u *)"line");
    إذا (nr != MAXCOL)
	wp->w_wantline = nr;
    nr = popup_options_one(d, (char_u *)"col");
    إذا (nr != MAXCOL)
	wp->w_wantcol = nr;


    nr = dict_get_bool(d, "fixed", -1);
    إذا (nr != -1)
	wp->w_popup_fixed = nr != 0;

    {
	poppos_T ppt = get_pos_مدخل(d, TRUE);

	إذا (ppt != POPPOS_NONE)
	    wp->w_popup_pos = ppt;
    }

    str = dict_get_string(d, "textprop", FALSE);
    إذا (str != عدم)
    {
	wp->w_popup_prop_type = 0;
	إذا (*str != NUL)
	{
	    wp->w_popup_prop_win = curwin;
	    di = dict_پیدا_کن(d, (char_u *)"textpropwin", -1);
	    إذا (di != عدم)
	    {
		wp->w_popup_prop_win = پیدا_کن_win_by_nr_or_id(&di->di_tv);
		إذا (!win_valid_any_tab(wp->w_popup_prop_win))
		    wp->w_popup_prop_win = curwin;
	    }

	    nr = پیدا_کن_prop_type_id(str, wp->w_popup_prop_win->w_میانگیر);
	    إذا (nr <= 0)
		nr = پیدا_کن_prop_type_id(str, عدم);
	    إذا (nr <= 0)
		semsg(_(e_invalid_آرگومان_str), str);
	    وإلا
		wp->w_popup_prop_type = nr;
	}
    }

    di = dict_پیدا_کن(d, (char_u *)"textpropid", -1);
    إذا (di != عدم)
	wp->w_popup_prop_id = dict_get_عدد(d, "textpropid");
}

/*
 * Handle "moved" and "mousemoved" arguments.
 */
    ساكن فراغ
handle_moved_آرگومان(win_T *wp, dictitem_T *di, صحيح mousemoved)
{
    إذا (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != عدم)
    {
	char_u  *s = di->di_tv.vval.v_string;
	صحيح	flags = 0;

	إذا (STRCMP(s, "word") == 0)
	    flags = پیدا_کن_IDENT | پیدا_کن_STRING;
	وإلا إذا (STRCMP(s, "WORD") == 0)
	    flags = پیدا_کن_STRING;
	وإلا إذا (STRCMP(s, "expr") == 0)
	    flags = پیدا_کن_IDENT | پیدا_کن_STRING | پیدا_کن_EVAL;
	وإلا إذا (STRCMP(s, "any") != 0)
	    semsg(_(e_invalid_آرگومان_str), s);
	إذا (flags != 0)
	{
	    إذا (mousemoved)
		مجموعه_mousemoved_columns(wp, flags);
	    وإلا
		مجموعه_moved_columns(wp, flags);
	}
    }
    وإلا إذا (di->di_tv.v_type == VAR_LIST
	    && di->di_tv.vval.v_list != عدم
	    && (di->di_tv.vval.v_list->lv_len == 2
	     || di->di_tv.vval.v_list->lv_len == 3))
    {
	list_T	    *l = di->di_tv.vval.v_list;
	listitem_T  *li;
	صحيح	    mincol;
	صحيح	    maxcol;

	CHECK_LIST_MATERIALIZE(l);
	li = l->lv_اولی;
	إذا (l->lv_len == 3)
	{
	    varnumber_T nr = tv_get_عدد(&l->lv_اولی->li_tv);

	    // Three numbers, might be from popup_getoptions().
	    إذا (mousemoved)
		wp->w_popup_mouse_ردیف = nr;
	    وإلا
		wp->w_popup_lnum = nr;
	    li = li->li_بعدی;
	    إذا (nr == 0)
		wp->w_popup_curwin = عدم;
	}

	mincol = tv_get_عدد(&li->li_tv);
	maxcol = tv_get_عدد(&li->li_بعدی->li_tv);
	إذا (mousemoved)
	{
	    wp->w_popup_mouse_mincol = mincol;
	    wp->w_popup_mouse_maxcol = maxcol;
	}
	وإلا
	{
	    wp->w_popup_mincol = mincol;
	    wp->w_popup_maxcol = maxcol;
	}
    }
    وإلا
	semsg(_(e_invalid_آرگومان_str), tv_get_string(&di->di_tv));
}

    ساكن فراغ
check_highlight(dict_T *dict, حرف *name, char_u **pval)
{
    dictitem_T  *di;
    char_u	*str;

    di = dict_پیدا_کن(dict, (char_u *)name, -1);
    إذا (di == عدم)
	أرجع;

    إذا (di->di_tv.v_type != VAR_STRING)
	semsg(_(e_invalid_مقدار_for_آرگومان_str), name);
    وإلا
    {
	str = tv_get_string(&di->di_tv);
	إذا (*str != NUL)
	    *pval = vim_strsave(str);
    }
}

/*
 * Scroll to show the line with the cursor.
 */
    ساكن فراغ
popup_show_curline(win_T *wp)
{
    إذا (wp->w_cursor.lnum < wp->w_topline)
	wp->w_topline = wp->w_cursor.lnum;
    وإلا إذا (wp->w_cursor.lnum >= wp->w_botline
					      && (wp->w_valid & VALID_BOTLINE))
    {
	wp->w_topline = wp->w_cursor.lnum - wp->w_ارتفاع + 1;
	إذا (wp->w_topline < 1)
	    wp->w_topline = 1;
	وإلا إذا (wp->w_topline > wp->w_میانگیر->b_ml.ml_سطر_شمار)
	    wp->w_topline = wp->w_میانگیر->b_ml.ml_سطر_شمار;
	طالما (wp->w_topline < wp->w_cursor.lnum
		&& wp->w_topline < wp->w_میانگیر->b_ml.ml_سطر_شمار
		&& plines_m_win(wp, wp->w_topline, wp->w_cursor.lnum,
					    wp->w_ارتفاع + 1) > wp->w_ارتفاع)
	    ++wp->w_topline;
    }

    // Don't let "firstline" cause a scroll.
    إذا (wp->w_firstline > 0)
	wp->w_firstline = wp->w_topline;
}

/*
 * Get the sign group name for window "wp".
 * Returns a pointer to a static buffer, overwritten on the next call.
 */
    ساكن char_u *
popup_get_sign_name(win_T *wp)
{
    ساكن حرف    buf[30];

    vim_snprintf(buf, حجم(buf), "popup-%d", wp->w_id);
    أرجع (char_u *)buf;
}

/*
 * Highlight the line with the cursor.
 * Also scrolls the text to put the cursor line in view.
 */
    ساكن فراغ
popup_highlight_curline(win_T *wp)
{
    صحيح	    sign_id = 0;
    char_u  *sign_name = popup_get_sign_name(wp);

    buf_delete_signs(wp->w_میانگیر, (char_u *)"PopUpMenu");

    إذا ((wp->w_popup_flags & POPF_CURSORLINE) != 0)
    {
	popup_show_curline(wp);

	إذا (!sign_exists_by_name(sign_name))
	{
	    حرف *linehl = "PopupSelected";
	    sign_define_by_name(sign_name, عدم, (char_u *)linehl, عدم, عدم, عدم,
		    عدم, SIGN_DEF_PRIO);
	}

	sign_قراربده(&sign_id, (char_u *)"PopUpMenu", sign_name,
			       wp->w_میانگیر, wp->w_cursor.lnum, SIGN_DEF_PRIO);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    وإلا
	sign_undefine_by_name(sign_name, FALSE);
    wp->w_popup_آخرین_curline = wp->w_cursor.lnum;
}

/*
 * Shared between popup_create() and f_popup_setoptions().
 */
    ساكن صحيح
apply_general_options(win_T *wp, dict_T *dict)
{
    dictitem_T	*di;
    صحيح		nr;
    char_u	*str;

#اگر_تعریف_شده FEAT_IMAGE_POPUP
    image_placement_use_T iplace_use;
#پایان_اگر

    // TODO: flip

    di = dict_پیدا_کن(dict, (char_u *)"firstline", -1);
    إذا (di != عدم)
    {
	wp->w_firstline = dict_get_عدد(dict, "firstline");
	إذا (wp->w_firstline < 0)
	    wp->w_firstline = -1;
    }

    nr = dict_get_bool(dict, "scrollbar", -1);
    إذا (nr != -1)
	wp->w_want_scrollbar = nr;

    str = dict_get_string(dict, "title", FALSE);
    إذا (str != عدم)
    {
	char_u	*title = vim_strsave(str);

	إذا (title != عدم)
	{
	    vim_free(wp->w_popup_title);
	    wp->w_popup_title = title;
	}
    }

    nr = dict_get_bool(dict, "wrap", -1);
    إذا (nr != -1)
	wp->w_p_wrap = nr != 0;

    nr = dict_get_bool(dict, "drag", -1);
    إذا (nr != -1)
    {
	إذا (nr)
	    wp->w_popup_flags |= POPF_DRAG;
	وإلا
	    wp->w_popup_flags &= ~POPF_DRAG;
    }
    nr = dict_get_bool(dict, "dragall", -1);
    إذا (nr != -1)
    {
	إذا (nr)
	    wp->w_popup_flags |= POPF_DRAGALL;
	وإلا
	    wp->w_popup_flags &= ~POPF_DRAGALL;
    }

    nr = dict_get_bool(dict, "posinvert", -1);
    إذا (nr != -1)
    {
	إذا (nr)
	    wp->w_popup_flags |= POPF_POSINVERT;
	وإلا
	    wp->w_popup_flags &= ~POPF_POSINVERT;
    }

    nr = dict_get_bool(dict, "clipwindow", -1);
    إذا (nr != -1)
    {
	إذا (nr)
	    wp->w_popup_flags |= POPF_CLIPWINDOW;
	وإلا
	    wp->w_popup_flags &= ~POPF_CLIPWINDOW;
    }

    nr = dict_get_bool(dict, "resize", -1);
    إذا (nr != -1)
    {
	إذا (nr)
	    wp->w_popup_flags |= POPF_RESIZE;
	وإلا
	    wp->w_popup_flags &= ~POPF_RESIZE;
    }

    di = dict_پیدا_کن(dict, (char_u *)"opacity", -1);
    إذا (di != عدم)
    {
	nr = dict_get_عدد(dict, "opacity");
	إذا (nr == 0)
	{
	    // opacity: 0, fully transparent
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100;
	}
	وإلا إذا (nr > 0 && nr < 100)
	{
	    // opacity: 1-99, partially transparent
	    // Convert to blend (0=opaque, 100=transparent)
	    wp->w_popup_flags |= POPF_OPACITY;
	    wp->w_popup_blend = 100 - nr;
	}
	وإلا إذا (nr == 100)
	{
	    // Fully opaque, same as no opacity set.
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
	وإلا
	{
	    wp->w_popup_flags &= ~POPF_OPACITY;
	    wp->w_popup_blend = 0;
	}
    }

#اگر_تعریف_شده FEAT_IMAGE_POPUP
    إذا (wp->w_popup_flags & POPF_GLOBAL)
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_GLOBAL;
    وإلا
	iplace_use = IMAGE_PLACEMENT_USE_POPUP_LOCAL;

    di = dict_پیدا_کن(dict, (char_u *)"image", -1);

    // Note: we do not enable the image placement here. This is because
    // draw_image_placements() may be drawn before the popup window text is
    // drawn (e.g. inside a hit-enter prompt). Instead only enable once we draw
    // the popup window.
    إذا (di != عدم && di->di_tv.v_type == VAR_DICT
	    && di->di_tv.vval.v_dict != عدم)
    {
	image_T		    *img;
	image_T		    *cur_img = عدم;
	image_placement_T   *قراربده;
	صحيح		    cw, ch;

	إذا (dict_len(di->di_tv.vval.v_dict) == 0)
	{
	    إذا (wp->w_popup_imagep != عدم)
	    {
		image_placement_free(wp->w_popup_imagep);
		wp->w_popup_imagep = عدم;
	    }
	    اذهب image_انجامشده;
	}

	// Check if image size and format is the same, if so, then just update
	// the existing data.
	إذا (wp->w_popup_imagep != عدم && wp->w_popup_imagep->img != عدم)
	    cur_img = wp->w_popup_imagep->img;

	img = جمع_کن_image(di->di_tv.vval.v_dict, cur_img, true);
	إذا (img == عدم)
	    أرجع FAIL;

	إذا (img == cur_img)
	    اذهب image_انجامشده;

	// Create image view for this popup. Use window id as generation number,
	// so that newer image placements are always placed below older image
	// placements with same zindex.
	قراربده = image_placement_new(img, wp->w_id, iplace_use, false);

	إذا (قراربده == عدم)
	{
	    image_unref(img);
	    أرجع FAIL;
	}

	إذا (wp->w_popup_imagep != عدم)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = قراربده;

	image_get_cell_dimensions(img, &cw, &ch);

	// Set the dimensions of the popup that were not specified by the user.
	إذا (dict_پیدا_کن(dict, (char_u *)"minwidth", -1) == عدم)
	    wp->w_minwidth = cw;
	إذا (dict_پیدا_کن(dict, (char_u *)"maxwidth", -1) == عدم)
	    wp->w_maxwidth = cw;
	إذا (dict_پیدا_کن(dict, (char_u *)"minheight", -1) == عدم)
	    wp->w_minheight = ch;
	إذا (dict_پیدا_کن(dict, (char_u *)"maxheight", -1) == عدم)
	    wp->w_maxheight = ch;
image_انجامشده:
	;
    }
    وإلا
    {
	// Create a image placement with no backing images. This is so that it
	// will affect the calculations of other popup images.
	إذا (wp->w_popup_imagep != عدم)
	    image_placement_free(wp->w_popup_imagep);
	wp->w_popup_imagep = image_placement_new(عدم, wp->w_id,
		iplace_use, true);

	// Not really needed because we always update the position before
	// drawing, but do it anyways.
	إذا (wp->w_popup_imagep != عدم)
	    image_placement_مجموعه_bounding_box(wp->w_popup_imagep,
		    wp->w_winrow, wp->w_wincol, wp->w_ارتفاع, wp->w_عرض);
    }
#پایان_اگر

    di = dict_پیدا_کن(dict, (char_u *)"close", -1);
    إذا (di != عدم)
    {
	صحيح ok = TRUE;

	إذا (di->di_tv.v_type == VAR_STRING && di->di_tv.vval.v_string != عدم)
	{
	    char_u  *s = di->di_tv.vval.v_string;

	    إذا (STRCMP(s, "none") == 0)
		wp->w_popup_close = POPCLOSE_NONE;
	    وإلا إذا (STRCMP(s, "button") == 0)
		wp->w_popup_close = POPCLOSE_BUTTON;
	    وإلا إذا (STRCMP(s, "click") == 0)
		wp->w_popup_close = POPCLOSE_CLICK;
	    وإلا
		ok = FALSE;
	}
	وإلا
	    ok = FALSE;
	إذا (!ok)
	    semsg(_(e_invalid_مقدار_for_آرگومان_str_str), "close", tv_get_string(&di->di_tv));
    }

    str = dict_get_string(dict, "highlight", FALSE);
    إذا (str != عدم)
    {
	حرف *errmsg = update_wincolor(wp, str);

	إذا (errmsg == عدم)
	    مجموعه_string_option_مستقیم_درون_win(wp, (char_u *)"wincolor", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	وإلا
	    emsg(_(errmsg));
    }

    str = dict_get_string(dict, "highlights", FALSE);
    إذا (str != عدم)
    {
	حرف *errmsg = update_winhighlight(wp, str);

	إذا (errmsg == عدم)
	    مجموعه_string_option_مستقیم_درون_win(wp, (char_u *)"winhighlight", -1,
		    str, OPT_FREE|OPT_LOCAL, 0);
	وإلا
	    emsg(_(errmsg));
    }

    إذا (مجموعه_padding_border(dict, wp->w_popup_padding, "padding", 999) == FAIL ||
	مجموعه_padding_border(dict, wp->w_popup_border, "border", 1) == FAIL)
	أرجع FAIL;

    di = dict_پیدا_کن(dict, (char_u *)"borderhighlight", -1);
    إذا (di != عدم)
    {
	إذا (di->di_tv.v_type != VAR_LIST || di->di_tv.vval.v_list == عدم)
	{
	    emsg(_(e_list_required));
	    أرجع FAIL;
	}
	وإلا
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    صحيح		i;

	    CHECK_LIST_MATERIALIZE(list);
	    wp->w_border_highlight_isset = true;
	    // Clear all highlights if list is empty
	    إذا (list->lv_len == 0)
	    {
		لكل (i = 0; i < 4; ++i)
		{
		    vim_free(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = عدم;
		}
	    }
	    وإلا
	    {
		لكل (i = 0, li = list->lv_اولی; i < 4 && i < list->lv_len;
						     ++i, li = li->li_بعدی)
		{
		    str = tv_get_string(&li->li_tv);
		    إذا (*str != NUL)
		    {
			char_u	*hl = vim_strsave(str);

			إذا (hl != عدم)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
		}
		إذا (list->lv_len == 1 && wp->w_border_highlight[0] != عدم)
		    لكل (i = 1; i < 4; ++i)
		    {
			char_u	*hl = vim_strsave(wp->w_border_highlight[0]);

			إذا (hl != عدم)
			{
			    vim_free(wp->w_border_highlight[i]);
			    wp->w_border_highlight[i] = hl;
			}
		    }
	    }
	}
    }

    di = dict_پیدا_کن(dict, (char_u *)"borderchars", -1);
    إذا (di != عدم)
    {
	إذا (di->di_tv.v_type != VAR_LIST)
	{
	    emsg(_(e_list_required));
	    أرجع FAIL;
	}
	وإلا
	{
	    list_T	*list = di->di_tv.vval.v_list;
	    listitem_T	*li;
	    صحيح		i;

	    إذا (list != عدم)
	    {
		CHECK_LIST_MATERIALIZE(list);
		لكل (i = 0, li = list->lv_اولی; i < 8 && i < list->lv_len;
							 ++i, li = li->li_بعدی)
		{
		    str = tv_get_string(&li->li_tv);
		    إذا (*str != NUL)
			wp->w_border_char[i] = mb_ptr2char(str);
		}
		إذا (list->lv_len == 1)
		    لكل (i = 1; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		إذا (list->lv_len == 2)
		{
		    لكل (i = 4; i < 8; ++i)
			wp->w_border_char[i] = wp->w_border_char[1];
		    لكل (i = 1; i < 4; ++i)
			wp->w_border_char[i] = wp->w_border_char[0];
		}
	    }
	}
    }

    check_highlight(dict, "scrollbarhighlight", &wp->w_scrollbar_highlight);
    check_highlight(dict, "thumbhighlight", &wp->w_thumb_highlight);

    di = dict_پیدا_کن(dict, (char_u *)"zindex", -1);
    إذا (di != عدم)
    {
	wp->w_zindex = dict_get_عدد(dict, "zindex");
	إذا (wp->w_zindex < 1)
	    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
	إذا (wp->w_zindex > 32000)
	    wp->w_zindex = 32000;
    }

    di = dict_پیدا_کن(dict, (char_u *)"mask", -1);
    إذا (di != عدم)
    {
	صحيح ok = FALSE;

	إذا (di->di_tv.v_type == VAR_LIST && di->di_tv.vval.v_list != عدم)
	{
	    listitem_T *li;

	    ok = TRUE;
	    FOR_ALL_LIST_اقلام(di->di_tv.vval.v_list, li)
	    {
		إذا (li->li_tv.v_type != VAR_LIST
			|| li->li_tv.vval.v_list == عدم
			|| li->li_tv.vval.v_list->lv_len != 4)
		{
		    ok = FALSE;
		    اكسر;
		}
		وإلا
		    CHECK_LIST_MATERIALIZE(li->li_tv.vval.v_list);
	    }
	}
	إذا (ok)
	{
	    wp->w_popup_ماسک = di->di_tv.vval.v_list;
	    ++wp->w_popup_ماسک->lv_refcount;
	    VIM_CLEAR(wp->w_popup_ماسک_cells);
	}
	وإلا
	{
	    semsg(_(e_invalid_مقدار_for_آرگومان_str), "mask");
	    أرجع FAIL;
	}
    }

#إذا defined(FEAT_TIMERS)
    // Add timer to close the popup after some time.
    nr = dict_get_عدد(dict, "time");
    إذا (nr > 0)
	popup_جمع_کن_timeout(wp, nr, TRUE);
#پایان_اگر

    di = dict_پیدا_کن(dict, (char_u *)"moved", -1);
    إذا (di != عدم)
    {
	مجموعه_moved_مقادیر(wp);
	handle_moved_آرگومان(wp, di, FALSE);
    }

    di = dict_پیدا_کن(dict, (char_u *)"mousemoved", -1);
    إذا (di != عدم)
    {
	مجموعه_mousemoved_مقادیر(wp);
	handle_moved_آرگومان(wp, di, TRUE);
    }

    nr = dict_get_bool(dict, "cursorline", -1);
    إذا (nr != -1)
    {
	إذا (nr != 0)
	    wp->w_popup_flags |= POPF_CURSORLINE;
	وإلا
	    wp->w_popup_flags &= ~POPF_CURSORLINE;
    }

    di = dict_پیدا_کن(dict, (char_u *)"filter", -1);
    إذا (di != عدم)
    {
	callback_T	callback = get_callback(&di->di_tv);

	إذا (callback.cb_name != عدم)
	{
	    free_callback(&wp->w_filter_cb);
	    مجموعه_callback(&wp->w_filter_cb, &callback);
	}
    }
    nr = dict_get_bool(dict, "mapping", -1);
    إذا (nr != -1)
    {
	إذا (nr)
	    wp->w_popup_flags |= POPF_MAPPING;
	وإلا
	    wp->w_popup_flags &= ~POPF_MAPPING;
    }

    str = dict_get_string(dict, "filtermode", FALSE);
    إذا (str != عدم)
    {
	إذا (STRCMP(str, "a") == 0)
	    wp->w_filter_نما = نما_ALL;
	وإلا
	    wp->w_filter_نما = نما_str2flags(str);
    }

    di = dict_پیدا_کن(dict, (char_u *)"callback", -1);
    إذا (di == عدم)
	أرجع OK;

    callback_T	callback = get_callback(&di->di_tv);
    إذا (callback.cb_name == عدم)
	أرجع OK;

    free_callback(&wp->w_close_cb);
    مجموعه_callback(&wp->w_close_cb, &callback);
    أرجع OK;
}

/*
 * Go through the options in "dict" and apply them to popup window "wp".
 * "create" is TRUE when creating a new popup window.
 */
    ساكن صحيح
apply_options(win_T *wp, dict_T *dict, صحيح create)
{
    صحيح		nr;

    apply_move_options(wp, dict);

    إذا (create)
	مجموعه_string_option_مستقیم_درون_win(wp, (char_u *)"signcolumn", -1,
					(char_u *)"no", OPT_FREE|OPT_LOCAL, 0);

    إذا (apply_general_options(wp, dict) == FAIL)
	أرجع FAIL;

    nr = dict_get_bool(dict, "hidden", FALSE);
    إذا (nr > 0)
	wp->w_popup_flags |= POPF_HIDDEN | POPF_HIDDEN_FORCE;

    // when "firstline" and "cursorline" are both set and the cursor would be
    // above or below the displayed lines, move the cursor to "firstline".
    إذا (wp->w_firstline > 0 && (wp->w_popup_flags & POPF_CURSORLINE))
    {
	إذا (wp->w_firstline > wp->w_میانگیر->b_ml.ml_سطر_شمار)
	    wp->w_cursor.lnum = wp->w_میانگیر->b_ml.ml_سطر_شمار;
	وإلا إذا (wp->w_cursor.lnum < wp->w_firstline
		|| wp->w_cursor.lnum >= wp->w_firstline + wp->w_ارتفاع)
	    wp->w_cursor.lnum = wp->w_firstline;
	wp->w_topline = wp->w_firstline;
	wp->w_valid &= ~VALID_BOTLINE;
    }

    إذا (create)
	popup_ماسک_refresh = TRUE;
    popup_highlight_curline(wp);

    أرجع OK;
}

/*
 * Add lines to the popup from a list of strings.
 */
    ساكن فراغ
جمع_کن_popup_رشتهها(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    linenr_T    lnum = 0;
    char_u	*p;

    FOR_ALL_LIST_اقلام(l, li)
	إذا (li->li_tv.v_type == VAR_STRING)
	{
	    p = li->li_tv.vval.v_string;
	    ml_append_buf(buf, lnum++,
			       p == عدم ? (char_u *)"" : p, (colnr_T)0, TRUE);
	}
}

/*
 * Add lines to the popup from a list of dictionaries.
 */
    ساكن فراغ
جمع_کن_popup_dicts(buf_T *buf, list_T *l)
{
    listitem_T  *li;
    listitem_T  *pli;
    linenr_T    lnum = 0;
    char_u	*p;
    dict_T	*dict;

    // first add the text lines
    FOR_ALL_LIST_اقلام(l, li)
    {
	إذا (li->li_tv.v_type != VAR_DICT)
	{
	    semsg(_(e_آرگومان_1_list_قلم_nr_dictionary_required), lnum + 1);
	    أرجع;
	}
	dict = li->li_tv.vval.v_dict;
	p = dict == عدم ? عدم : dict_get_string(dict, "text", FALSE);
	ml_append_buf(buf, lnum++,
			       p == عدم ? (char_u *)"" : p, (colnr_T)0, TRUE);
    }

    // add the text properties
    lnum = 1;
    لكل (li = l->lv_اولی; li != عدم; li = li->li_بعدی, ++lnum)
    {
	dictitem_T	*di;
	list_T		*plist;

	dict = li->li_tv.vval.v_dict;
	di = dict_پیدا_کن(dict, (char_u *)"props", -1);
	إذا (di != عدم)
	{
	    إذا (di->di_tv.v_type != VAR_LIST)
	    {
		emsg(_(e_list_required));
		أرجع;
	    }
	    plist = di->di_tv.vval.v_list;
	    إذا (plist != عدم)
	    {
		FOR_ALL_LIST_اقلام(plist, pli)
		{
		    إذا (pli->li_tv.v_type != VAR_DICT)
		    {
			emsg(_(e_dictionary_required));
			أرجع;
		    }
		    dict = pli->li_tv.vval.v_dict;
		    إذا (dict != عدم)
		    {
			صحيح ستون = dict_get_عدد(dict, "col");

			prop_جمع_کن_common(lnum, ستون, dict, buf, عدم);
		    }
		}
	    }
	}
    }
}

/*
 * Get the padding plus border at the top, adjusted to 1 if there is a title.
 */
    صحيح
popup_بالاترین_extra(win_T *wp)
{
    صحيح	extra = wp->w_popup_border[0] + wp->w_popup_padding[0];

    إذا (extra == 0 && wp->w_popup_title != عدم && *wp->w_popup_title != NUL)
	أرجع 1;
    أرجع extra;
}

/*
 * Get the padding plus border at the left.
 */
    صحيح
popup_چپ_extra(win_T *wp)
{
    أرجع wp->w_popup_border[3] + wp->w_popup_padding[3];
}

/*
 * Return the height of popup window "wp", including border and padding.
 */
    صحيح
popup_ارتفاع(win_T *wp)
{
    أرجع wp->w_ارتفاع
	    + popup_بالاترین_extra(wp)
	    + wp->w_popup_padding[2] + wp->w_popup_border[2];
}

/*
 * Return the width of popup window "wp", including border, padding and
 * scrollbar.
 */
    صحيح
popup_عرض(win_T *wp)
{
    // w_popup_rightoff is how many columns of the core are right of the screen
    أرجع wp->w_عرض
	    + popup_extra_عرض(wp)
	    + wp->w_popup_rightoff;
}

/*
 * Return the extra width of popup window "wp": border, padding and scrollbar.
 */
    صحيح
popup_extra_عرض(win_T *wp)
{
    أرجع wp->w_popup_padding[3] + wp->w_popup_border[3]
	    + wp->w_popup_padding[1] + wp->w_popup_border[1]
	    + wp->w_has_scrollbar;
}

/*
 * Return the host window used to clip popup "wp" when POPF_CLIPWINDOW is set,
 * or NULL when no clipping should be applied (option off, or the host window
 * is no longer valid).  The textprop window is used as the host; popups not
 * anchored to a textprop are not clipped.
 */
    ساكن win_T *
popup_get_clipwin(win_T *wp)
{
    إذا (!(wp->w_popup_flags & POPF_CLIPWINDOW))
	أرجع عدم;
    إذا (win_valid(wp->w_popup_prop_win))
	أرجع wp->w_popup_prop_win;
    أرجع عدم;
}

// Per-popup clip geometry derived from w_popup_{top,bottom}off and
// w_popup_{left,right}clip.  Filled by popup_compute_clip().
//
//   *_extra        : original border+padding at each edge.
//   clip_*_content : how many *content* rows/cols are clipped at each edge
//                    (border/padding is consumed first; the rest comes off
//                    w_height/w_width).  >= 0.
//   eff_border[],
//   eff_padding[]  : per-edge border/padding sizes (indexed [top,right,bot,left]
//                    matching wp->w_popup_border / wp->w_popup_padding).  The
//                    clip consumes the border first, then the padding, so when
//                    only the border is clipped the padding still survives.
//                    Drawing code can replace
//                    `wp->w_popup_border[N] > 0 && wp->w_popup_*clip == 0`
//                    with a single `cl.eff_border[N] > 0` test.
//   eff_*_extra    : eff_border + eff_padding at that edge (visible decoration).
//   eff_height     : drawn extent = eff_top_extra + visible content + eff_bot_extra.
//   eff_width      : drawn extent = eff_left_extra + visible content + eff_right_extra
//                    (does NOT include w_leftcol or scrollbar; see callers).
تعريفنوع بنية {
    صحيح بالاترین_extra;
    صحيح bot_extra;
    صحيح چپ_extra;
    صحيح راست_extra;

    صحيح clip_بالاترین_content;
    صحيح clip_bot_content;
    صحيح clip_چپ_content;
    صحيح clip_راست_content;

    صحيح eff_بالاترین_extra;
    صحيح eff_bot_extra;
    صحيح eff_چپ_extra;
    صحيح eff_راست_extra;

    صحيح eff_border[4];
    صحيح eff_padding[4];

    صحيح eff_ارتفاع;
    صحيح eff_عرض;
} popup_clip_T;

    ساكن فراغ
popup_compute_clip(win_T *wp, popup_clip_T *cl)
{
    صحيح h, w;

    cl->بالاترین_extra = popup_بالاترین_extra(wp);
    cl->bot_extra = wp->w_popup_padding[2] + wp->w_popup_border[2];
    cl->چپ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    cl->راست_extra = wp->w_popup_border[1] + wp->w_popup_padding[1];

    cl->clip_بالاترین_content = wp->w_popup_topoff - cl->بالاترین_extra;
    إذا (cl->clip_بالاترین_content < 0)
	cl->clip_بالاترین_content = 0;
    cl->clip_bot_content = wp->w_popup_bottomoff - cl->bot_extra;
    إذا (cl->clip_bot_content < 0)
	cl->clip_bot_content = 0;
    cl->clip_چپ_content = wp->w_popup_leftclip - cl->چپ_extra;
    إذا (cl->clip_چپ_content < 0)
	cl->clip_چپ_content = 0;
    cl->clip_راست_content = wp->w_popup_rightclip - cl->راست_extra;
    إذا (cl->clip_راست_content < 0)
	cl->clip_راست_content = 0;

    // Border is consumed before padding: when only the border row/column is
    // clipped, the adjacent padding row/column is still visible.  Horizontal
    // edges keep the previous all-or-nothing behaviour for now; the drawing
    // code there still uses original w_popup_border / w_popup_padding offsets.
    {
	صحيح clip = wp->w_popup_topoff;
	صحيح b = wp->w_popup_border[0];
	صحيح p = wp->w_popup_padding[0];
	صحيح rem;

	إذا (clip >= b)
	{
	    cl->eff_border[0] = 0;
	    rem = clip - b;
	    cl->eff_padding[0] = (rem >= p) ? 0 : p - rem;
	}
	وإلا
	{
	    cl->eff_border[0] = b;
	    cl->eff_padding[0] = p;
	}
    }
    {
	صحيح clip = wp->w_popup_bottomoff;
	صحيح b = wp->w_popup_border[2];
	صحيح p = wp->w_popup_padding[2];
	صحيح rem;

	إذا (clip >= b)
	{
	    cl->eff_border[2] = 0;
	    rem = clip - b;
	    cl->eff_padding[2] = (rem >= p) ? 0 : p - rem;
	}
	وإلا
	{
	    cl->eff_border[2] = b;
	    cl->eff_padding[2] = p;
	}
    }
    cl->eff_border[1] = wp->w_popup_rightclip > 0 ? 0 : wp->w_popup_border[1];
    cl->eff_border[3] = wp->w_popup_leftclip > 0 ? 0 : wp->w_popup_border[3];
    cl->eff_padding[1] = wp->w_popup_rightclip > 0 ? 0 : wp->w_popup_padding[1];
    cl->eff_padding[3] = wp->w_popup_leftclip > 0 ? 0 : wp->w_popup_padding[3];

    // When a clip on one edge runs past the content rows, the excess must
    // eat into the OPPOSITE edge's decorations.  Otherwise the surviving
    // padding/border can land outside the host (e.g. a popup whose body is
    // wholly below the host still drew its top padding onto the status row).
    {
	صحيح excess = wp->w_popup_bottomoff - cl->bot_extra - wp->w_ارتفاع;
	إذا (excess > 0)
	{
	    إذا (excess >= cl->eff_padding[0])
	    {
		excess -= cl->eff_padding[0];
		cl->eff_padding[0] = 0;
		إذا (excess >= cl->eff_border[0])
		    cl->eff_border[0] = 0;
		وإلا
		    cl->eff_border[0] -= excess;
	    }
	    وإلا
		cl->eff_padding[0] -= excess;
	}
    }
    {
	صحيح excess = wp->w_popup_topoff - cl->بالاترین_extra - wp->w_ارتفاع;
	إذا (excess > 0)
	{
	    إذا (excess >= cl->eff_padding[2])
	    {
		excess -= cl->eff_padding[2];
		cl->eff_padding[2] = 0;
		إذا (excess >= cl->eff_border[2])
		    cl->eff_border[2] = 0;
		وإلا
		    cl->eff_border[2] -= excess;
	    }
	    وإلا
		cl->eff_padding[2] -= excess;
	}
    }

    cl->eff_بالاترین_extra = cl->eff_border[0] + cl->eff_padding[0];
    cl->eff_bot_extra = cl->eff_border[2] + cl->eff_padding[2];
    cl->eff_چپ_extra = wp->w_popup_leftclip > 0 ? 0 : cl->چپ_extra;
    cl->eff_راست_extra = wp->w_popup_rightclip > 0 ? 0 : cl->راست_extra;

    h = wp->w_ارتفاع - cl->clip_بالاترین_content - cl->clip_bot_content;
    إذا (h < 0)
	h = 0;
    cl->eff_ارتفاع = cl->eff_بالاترین_extra + h + cl->eff_bot_extra;

    w = wp->w_عرض - cl->clip_چپ_content - cl->clip_راست_content;
    إذا (w < 0)
	w = 0;
    cl->eff_عرض = cl->eff_چپ_extra + w + cl->eff_راست_extra;
}

// Snapshot of the popup window geometry that update_popups() temporarily
// mutates so that win_update() draws within the host-window clip rectangle.
// Saved before the clip is applied, restored after win_update() returns so
// callers continue to see the popup's logical geometry.
// Field names omit the "w_" prefix to avoid clashing with struct-field
// macros like w_p_wrap (= w_onebuf_opt.wo_wrap).
تعريفنوع بنية {
    صحيح		ارتفاع;
    صحيح		عرض;
    صحيح		winrow;
    صحيح		wincol;
    صحيح		leftcol;
    صحيح		p_wrap;
    linenr_T	topline;
} popup_geom_save_T;

    ساكن فراغ
popup_geom_save(win_T *wp, popup_geom_save_T *sv)
{
    sv->ارتفاع  = wp->w_ارتفاع;
    sv->عرض   = wp->w_عرض;
    sv->winrow  = wp->w_winrow;
    sv->wincol  = wp->w_wincol;
    sv->leftcol = wp->w_leftcol;
    sv->p_wrap  = wp->w_p_wrap;
    sv->topline = wp->w_topline;
}

    ساكن فراغ
popup_geom_restore(win_T *wp, popup_geom_save_T *sv)
{
    wp->w_p_wrap  = sv->p_wrap;
    wp->w_leftcol = sv->leftcol;
    wp->w_wincol  = sv->wincol;
    wp->w_winrow  = sv->winrow;
    wp->w_topline = sv->topline;
    wp->w_عرض   = sv->عرض;
    wp->w_ارتفاع  = sv->ارتفاع;
}

/*
 * Compute a screen row for a textprop that has scrolled above the host
 * window's top.  textpos2screenpos() cannot return a row above topline, so
 * compute the virtual column directly from the prop's *own* line and then
 * extrapolate a (possibly-negative) row by counting how many buffer lines
 * lie between the prop and topline.  The popup_topoff clip path turns the
 * negative row into a top-clip animation as the prop rolls off the top edge.
 *
 * Probing at topline with the prop's tp_col would inherit topline's tab
 * stops / multi-byte widths, so the popup's wincol would jitter every time
 * a wider/narrower line scrolled into the topmost position.
 */
    ساكن فراغ
popup_screenpos_above_بالاترین(
	win_T	    *prop_win,
	pos_T	    *pos,
	linenr_T    prop_lnum,
	صحيح	    *screen_ردیف,
	صحيح	    *screen_scol,
	صحيح	    *screen_ccol,
	صحيح	    *screen_ecol)
{
    pos_T   probe = *pos;
    colnr_T scol = 0, ccol = 0, ecol = 0;
    صحيح	    coloff;

    probe.lnum = prop_lnum;
    getvcol(prop_win, &probe, &scol, &ccol, &ecol, 0);
    coloff = (صحيح)win_ستون_off(prop_win) - (صحيح)prop_win->w_leftcol
					+ prop_win->w_wincol + 1;
    *screen_scol = (صحيح)scol + coloff;
    *screen_ccol = (صحيح)ccol + coloff;
    *screen_ecol = (صحيح)ecol + coloff;
    *screen_ردیف = prop_win->w_winrow + 1
				 - (صحيح)(prop_win->w_topline - prop_lnum);
}

/*
 * Hide popup "wp" because its anchoring textprop is no longer reachable.
 * Marks the popup as POPF_HIDDEN (no-op when already hidden) and schedules a
 * redraw of the host window so any leftover decorations are cleared.
 */
    ساكن فراغ
popup_hide_for_textprop(win_T *wp)
{
    إذا ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	أرجع;
    wp->w_popup_flags |= POPF_HIDDEN;
    إذا (win_valid(wp->w_popup_prop_win))
	redraw_win_later(wp->w_popup_prop_win, UPD_SOME_VALID);
}

/*
 * For "clipwindow" popups: search the lines above prop_win->w_topline for the
 * popup's anchoring textprop and report whether one was found.  When
 * "max_reach" is > 0, only the last "max_reach" lines before topline are
 * scanned; pass 0 to scan all lines from line 1.  Returns false when the
 * popup is not "clipwindow", topline is already at line 1, or no prop matches.
 */
    ساكن bool
popup_پیدا_کن_prop_above_بالاترین(
	win_T	    *wp,
	win_T	    *prop_win,
	صحيح	    max_دسترسی,
	textprop_T  *prop,
	linenr_T    *یافت_lnum)
{
    linenr_T	اولی;

    إذا (!(wp->w_popup_flags & POPF_CLIPWINDOW) || prop_win->w_topline <= 1)
	أرجع false;

    اولی = max_دسترسی > 0 ? prop_win->w_topline - max_دسترسی : 1;
    إذا (اولی < 1)
	اولی = 1;
    أرجع پیدا_کن_prop_درون_سطرها(prop_win,
		    wp->w_popup_prop_type, wp->w_popup_prop_id,
		    prop, یافت_lnum, اولی, prop_win->w_topline - 1);
}

/*
 * Compute and assign w_popup_topoff/bottomoff/leftclip/rightclip from the
 * host (textprop) window's content rectangle when POPF_CLIPWINDOW is set.
 * The popup's logical geometry (w_winrow, w_height, w_width) is preserved;
 * only the *off/clip fields record how much of each edge falls outside.
 * Returns true when the popup has scrolled completely past one of the host
 * edges, in which case the caller must hide it.
 */
    ساكن bool
popup_compute_clipwindow_offsets(win_T *wp)
{
    win_T   *cw = popup_get_clipwin(wp);
    صحيح	    extra_h, extra_w;
    صحيح	    popup_بالاترین, popup_bottom, popup_چپ, popup_راست;
    صحيح	    مجموع_h, مجموع_w;

    إذا (cw == عدم)
	أرجع false;

    extra_h = popup_بالاترین_extra(wp)
		    + wp->w_popup_padding[2] + wp->w_popup_border[2];
    extra_w = popup_extra_عرض(wp);

    popup_بالاترین = wp->w_winrow;
    popup_bottom = wp->w_winrow + wp->w_ارتفاع + extra_h;
    popup_چپ = wp->w_wincol;
    popup_راست = wp->w_wincol + wp->w_عرض + extra_w;
    مجموع_h = wp->w_ارتفاع + extra_h;
    مجموع_w = wp->w_عرض + extra_w;

    إذا (popup_بالاترین < cw->w_winrow)
	wp->w_popup_topoff = cw->w_winrow - popup_بالاترین;
    إذا (popup_bottom > cw->w_winrow + cw->w_ارتفاع)
	wp->w_popup_bottomoff = popup_bottom - (cw->w_winrow + cw->w_ارتفاع);
    إذا (popup_چپ < cw->w_wincol)
	wp->w_popup_leftclip = cw->w_wincol - popup_چپ;
    إذا (popup_راست > cw->w_wincol + cw->w_عرض)
	wp->w_popup_rightclip = popup_راست - (cw->w_wincol + cw->w_عرض);

    أرجع wp->w_popup_topoff >= مجموع_h
	|| wp->w_popup_bottomoff >= مجموع_h
	|| wp->w_popup_leftclip >= مجموع_w
	|| wp->w_popup_rightclip >= مجموع_w;
}

/*
 * Mutate "wp"'s window geometry so win_update() draws only the rows/columns
 * that fit within the host-window clip rectangle for "clipwindow" popups.
 * The caller must save the original geometry with popup_geom_save() before
 * this call and restore it with popup_geom_restore() after win_update().
 *
 * Vertical clip: shrink w_height by the clipped content rows; advance
 * w_topline and w_winrow when rows are cut off the top so the first visible
 * content row lands on the host's top edge.
 *
 * Horizontal clip: when the right side is clipped, just shrink w_width.
 * When the left side is clipped, advance w_leftcol so the hidden buffer
 * columns scroll off and shift w_wincol so the first visible column lands on
 * the host's left edge.  Disable wrap so the transient w_width reduction does
 * not reflow wrapped lines: the popup's logical width is unchanged, we just
 * want to truncate cells that fall outside the host at draw time.
 */
    ساكن فراغ
popup_apply_winupdate_clip(win_T *wp, popup_clip_T *cl)
{
    إذا (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
    {
	wp->w_ارتفاع -= cl->clip_بالاترین_content + cl->clip_bot_content;
	إذا (wp->w_ارتفاع < 0)
	    wp->w_ارتفاع = 0;
	إذا (cl->clip_بالاترین_content > 0)
	    // w_winrow already points at the first visible row.
	    wp->w_topline += cl->clip_بالاترین_content;
    }
    إذا (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
    {
	إذا (cl->clip_چپ_content > 0 || cl->clip_راست_content > 0)
	    wp->w_p_wrap = FALSE;
	إذا (cl->clip_راست_content > 0)
	{
	    wp->w_عرض -= cl->clip_راست_content;
	    إذا (wp->w_عرض < 0)
		wp->w_عرض = 0;
	}
	إذا (cl->clip_چپ_content > 0)
	{
	    wp->w_leftcol += cl->clip_چپ_content;
	    wp->w_wincol += cl->clip_چپ_content;
	    wp->w_عرض -= cl->clip_چپ_content;
	    إذا (wp->w_عرض < 0)
		wp->w_عرض = 0;
	}
    }
}

#اگر_تعریف_شده FEAT_IMAGE_POPUP
/*
 * Position the image associated with this popup window (if any).
 */
    ساكن فراغ
popup_position_image(win_T *wp)
{
    linenr_T	    ردیف;
    colnr_T	    ستون;

    صحيح iw, ih;

    // Visible cell width and height of popup itself (not image!)
    صحيح	visible_عرض;
    صحيح	visible_ارتفاع;

    // Crop dimensions of image (in pixels).
    صحيح	crop_x, crop_y, crop_عرض, crop_ارتفاع;

    popup_clip_T clip;

    إذا (wp->w_popup_imagep == عدم || wp->w_popup_flags & POPF_HIDDEN)
	أرجع;

    image_placement_مجموعه_enable(wp->w_popup_imagep, true);

    ردیف = wp->w_winrow + wp->w_popup_border[0] + wp->w_popup_padding[0];
    ستون = wp->w_wincol + wp->w_popup_border[3] + wp->w_popup_padding[3];

    popup_compute_clip(wp, &clip);

    إذا (wp->w_popup_imagep->img == عدم)
	// Just set the bounding box
	اذهب خروج;

    visible_عرض =
	wp->w_عرض - clip.clip_چپ_content - clip.clip_راست_content;
    visible_ارتفاع =
	wp->w_ارتفاع - clip.clip_بالاترین_content - clip.clip_bot_content;

    إذا (visible_عرض < 0 || visible_ارتفاع < 0)
	أرجع;

    image_get_cell_dimensions(wp->w_popup_imagep->img, &iw, &ih);

    crop_x = clip.clip_چپ_content;
    crop_y = clip.clip_بالاترین_content;
    crop_عرض = iw - crop_x - clip.clip_راست_content;
    crop_ارتفاع = ih - crop_y - clip.clip_bot_content;

    // Must account for border and padding (only cells that have been clipped).
    ردیف -= wp->w_popup_border[0] - clip.eff_border[0];
    ردیف -= wp->w_popup_padding[0] - clip.eff_padding[0];

    ستون -= wp->w_popup_border[3] - clip.eff_border[3];
    ستون -= wp->w_popup_padding[3] - clip.eff_padding[3];

    // Clamp to the popup's actual visible cell box in pixels, so the
    // crop can never claim more cells than the popup has
    إذا (crop_عرض > visible_عرض)
	crop_عرض = visible_عرض;
    إذا (crop_ارتفاع > visible_ارتفاع)
	crop_ارتفاع = visible_ارتفاع;

    image_placement_مجموعه_crop(wp->w_popup_imagep, crop_x, crop_y,
	    crop_عرض, crop_ارتفاع);
خروج:
    // Add after the last placement with the same zindex (if any), to match
    // popup window behaviour.
    image_placement_مجموعه_zindex(wp->w_popup_imagep, wp->w_zindex);
    image_placement_مجموعه_position(wp->w_popup_imagep, ردیف, ستون);
    image_placement_مجموعه_bounding_box(wp->w_popup_imagep,
	    wp->w_winrow, wp->w_wincol, clip.eff_ارتفاع, clip.eff_عرض);
}
#پایان_اگر


/*
 * Adjust the position and size of the popup to fit on the screen.
 */
    ساكن فراغ
popup_adjust_position(win_T *wp)
{
    linenr_T	lnum;
    صحيح		wrapped = 0;
    صحيح		maxwidth;
    صحيح		maxwidth_no_scrollbar;
    صحيح		عرض_with_scrollbar = 0;
    صحيح		استفاده_شده_maxwidth = FALSE;
    صحيح		margin_عرض = 0;
    صحيح		maxspace;
    صحيح		center_vert = FALSE;
    صحيح		center_hor = FALSE;
    صحيح		allow_adjust_چپ = !wp->w_popup_fixed;
    صحيح		بالاترین_extra = popup_بالاترین_extra(wp);
    صحيح		راست_extra = wp->w_popup_border[1] + wp->w_popup_padding[1]
		    + (wp->w_popup_shadow ? 2 : 0);
    صحيح		bot_extra = wp->w_popup_border[2] + wp->w_popup_padding[2]
		    + wp->w_popup_shadow;
    صحيح		چپ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];
    صحيح		extra_ارتفاع = بالاترین_extra + bot_extra;
    صحيح		extra_عرض = چپ_extra + راست_extra;
    صحيح		w_ارتفاع_before_حد;
    popup_layout_T org_layout;
    صحيح		minwidth, minheight;
    صحيح		maxheight = ردیفها;
    صحيح		wantline = wp->w_wantline;  // adjusted for textprop
    صحيح		wantcol = wp->w_wantcol;    // adjusted for textprop
    صحيح		use_wantcol = wantcol != 0;
    صحيح		adjust_ارتفاع_for_بالاترین_aligned = FALSE;

    popup_save_layout(wp, &org_layout);

    wp->w_winrow = 0;
    wp->w_wincol = 0;
    wp->w_leftcol = 0;
    wp->w_popup_leftoff = 0;
    wp->w_popup_rightoff = 0;
    wp->w_popup_topoff = 0;
    wp->w_popup_bottomoff = 0;
    wp->w_popup_leftclip = 0;
    wp->w_popup_rightclip = 0;

    // May need to update the "cursorline" highlighting, which may also change
    // "topline"
    إذا (wp->w_popup_آخرین_curline != wp->w_cursor.lnum)
	popup_highlight_curline(wp);

    إذا (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	win_T	    *prop_win = wp->w_popup_prop_win;
	textprop_T  prop;
	linenr_T    prop_lnum;
	pos_T	    pos;
	صحيح	    screen_ردیف;
	صحيح	    screen_scol;
	صحيح	    screen_ccol;
	صحيح	    screen_ecol;

	// Popup window is positioned relative to a text property.  With
	// "clipwindow", keep the popup visible while the textprop has just
	// scrolled above the host's top: extrapolate a negative screen_row
	// from a prop above topline so the top-clip path can roll the popup
	// off the top edge.  Unhiding is done in check_popup_unhidden().
	bool prop_above_بالاترین = false;
	إذا (!پیدا_کن_visible_prop(prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
				&prop, &prop_lnum))
	{
	    إذا (popup_پیدا_کن_prop_above_بالاترین(wp, prop_win, 0,
							    &prop, &prop_lnum))
		prop_above_بالاترین = true;
	    وإلا
	    {
		إذا ((wp->w_popup_flags & POPF_HIDDEN) == 0)
		{
#اگر_تعریف_شده FEAT_IMAGE_POPUP
		    إذا (wp->w_popup_imagep != عدم)
			image_placement_clear(wp->w_popup_imagep);
#پایان_اگر
		    popup_hide_for_textprop(wp);
		    إذا (wp->w_winrow + popup_ارتفاع(wp) >= cmdline_ردیف)
			clear_cmdline = TRUE;
		}
		أرجع;
	    }
	}

	// Compute the desired position from the position of the text
	// property.  Use "wantline" and "wantcol" as offsets.
	pos.lnum = prop_lnum;
	pos.ستون = prop.tp_ستون;
	إذا (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    pos.ستون += prop.tp_len - 1;
	إذا (prop_above_بالاترین)
	    popup_screenpos_above_بالاترین(prop_win, &pos, prop_lnum, &screen_ردیف,
				     &screen_scol, &screen_ccol, &screen_ecol);
	وإلا
	    textpos2screenpos(prop_win, &pos, &screen_ردیف,
				     &screen_scol, &screen_ccol, &screen_ecol);

	إذا (screen_scol == 0)
	{
	    // position is off screen, make the width zero to hide it.
	    wp->w_عرض = 0;
	    أرجع;
	}
	إذا (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_TOPRIGHT)
	    // below the text
	    wantline = screen_ردیف + wantline + 1;
	وإلا
	    // above the text
	    wantline = screen_ردیف + wantline - 1;
	center_vert = FALSE;
	إذا (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
	    // right of the text
	    wantcol = screen_ecol + wantcol;
	وإلا
	    // left of the text
	    wantcol = screen_scol + wantcol - 2;
	use_wantcol = TRUE;
    }
    وإلا
    {
	// If no line was specified default to vertical centering.
	إذا (wantline == 0)
	    center_vert = TRUE;
	وإلا إذا (wantline < 0)
	    // If "wantline" is negative it actually means zero.
	    wantline = 0;
	إذا (wantcol < 0)
	    // If "wantcol" is negative it actually means zero.
	    wantcol = 0;
    }

    إذا (wp->w_popup_pos == POPPOS_CENTER)
    {
	// center after computing the size
	center_vert = TRUE;
	center_hor = TRUE;
    }
    وإلا
    {
	إذا (wantline > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
					|| wp->w_popup_pos == POPPOS_TOPRIGHT))
	{
	    wp->w_winrow = wantline - 1;
	    إذا (wp->w_winrow >= ردیفها)
		wp->w_winrow = ردیفها - 1;
	}
	إذا (wp->w_popup_pos == POPPOS_BOTTOM)
	{
	    // Assume that each buffer line takes one screen line, and one line
	    // for the top border.  First make sure cmdline_row is valid,
	    // calling update_screen() will set it only later.
	    compute_cmdrow();
	    wp->w_winrow = MAX(cmdline_ردیف
				    - wp->w_میانگیر->b_ml.ml_سطر_شمار - 1, 0);
	}

	إذا (!use_wantcol)
	    center_hor = TRUE;
	وإلا إذا (wantcol > 0 && (wp->w_popup_pos == POPPOS_TOPLEFT
		|| wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    wp->w_wincol = wantcol - 1;
	    // Need to see at least one character of content plus the right
	    // border/padding/shadow after the decoration.
	    إذا (wp->w_wincol > firstwin->w_wincol + topframe->fr_عرض
						- چپ_extra - راست_extra - 1)
		wp->w_wincol = firstwin->w_wincol + topframe->fr_عرض
						- چپ_extra - راست_extra - 1;
	}
    }

    // Keep the popup out of the tabpanel area so the available width is
    // computed correctly below.
    إذا (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;

    // When centering or right aligned, use maximum width.
    // When left aligned use the space available, but shift to the left when we
    // hit the right of the screen.
    // Reserve room for the right border/padding/shadow so the popup fits.
    maxspace = firstwin->w_wincol + topframe->fr_عرض
					- wp->w_wincol - چپ_extra - راست_extra;
    maxwidth = maxspace;
    إذا (wp->w_maxwidth > 0 && maxwidth > wp->w_maxwidth)
    {
	allow_adjust_چپ = FALSE;
	maxwidth = wp->w_maxwidth;
    }

    إذا (wp->w_p_nu || wp->w_p_rnu)
	margin_عرض = عدد_عرض(wp) + 1;
#اگر_تعریف_شده FEAT_FOLDING
    margin_عرض += wp->w_p_fdc;
#پایان_اگر
#اگر_تعریف_شده FEAT_SIGNS
    إذا (signcolumn_on(wp))
	margin_عرض += 2;
#پایان_اگر
    إذا (margin_عرض >= maxwidth)
	margin_عرض = maxwidth - 1;

    minwidth = wp->w_minwidth;
    minheight = wp->w_minheight;
#اگر_تعریف_شده FEAT_TERMINAL
    // A terminal popup initially does not have content, use a default minimal
    // width of 20 characters and height of 5 lines.
    إذا (wp->w_میانگیر->b_جمله != عدم)
    {
	إذا (minwidth == 0)
	    minwidth = 20;
	إذا (minheight == 0)
	    minheight = 5;
    }
#پایان_اگر

    إذا (wp->w_maxheight > 0)
	maxheight = wp->w_maxheight;
    وإلا إذا (wp->w_popup_pos == POPPOS_BOTTOM)
	maxheight = cmdline_ردیف - 1;

    // start at the desired first line
    إذا (wp->w_firstline > 0)
    {
	// If firstline is beyond the buffer content, reset it to auto-position.
	// This can happen when the popup was scrolled and then the buffer
	// content was changed to have fewer lines.
	إذا (wp->w_firstline > wp->w_میانگیر->b_ml.ml_سطر_شمار)
	    wp->w_firstline = 0;
	وإلا
	    wp->w_topline = wp->w_firstline;
    }
    إذا (wp->w_topline < 1)
	wp->w_topline = 1;
    وإلا إذا (wp->w_topline > wp->w_میانگیر->b_ml.ml_سطر_شمار)
	wp->w_topline = wp->w_میانگیر->b_ml.ml_سطر_شمار;

    // Compute width based on longest text line and the 'wrap' option.
    // Use a minimum width of one, so that something shows when there is no
    // text.
    // When "firstline" is -1 then start with the last buffer line and go
    // backwards.
    // TODO: more accurate wrapping
    wp->w_عرض = 1;
    // Pre-scan every buffer line to find the widest one, so the popup width
    // stays stable when scrolling changes which lines are visible.
    {
	linenr_T ln;
	صحيح saved_w_عرض = wp->w_عرض;

	إذا (wp->w_عرض < maxwidth)
	    wp->w_عرض = maxwidth;
	لكل (ln = 1; ln <= wp->w_میانگیر->b_ml.ml_سطر_شمار; ++ln)
	{
	    صحيح len = linetabsize(wp, ln) + margin_عرض;

	    إذا (wp->w_maxwidth > 0 && len > wp->w_maxwidth)
		len = wp->w_maxwidth;
	    إذا (saved_w_عرض < len)
		saved_w_عرض = len;
	    إذا (wp->w_maxwidth > 0 && saved_w_عرض >= wp->w_maxwidth)
		اكسر;
	}
	wp->w_عرض = saved_w_عرض;
    }
    إذا (wp->w_firstline < 0)
	lnum = wp->w_میانگیر->b_ml.ml_سطر_شمار;
    وإلا
	lnum = wp->w_topline;
    طالما (lnum >= 1 && lnum <= wp->w_میانگیر->b_ml.ml_سطر_شمار)
    {
	صحيح len;
	صحيح w_عرض = wp->w_عرض;

	// Count Tabs for what they are worth and compute the length based on
	// the maximum width (matters when 'showbreak' is set).
	// "margin_width" is added to "len" where it matters.
	إذا (wp->w_عرض < maxwidth)
	    wp->w_عرض = maxwidth;
	len = linetabsize(wp, lnum);
	wp->w_عرض = w_عرض;

	إذا (len + margin_عرض > maxwidth
		&& allow_adjust_چپ
		&& (wp->w_popup_pos == POPPOS_TOPLEFT
		    || wp->w_popup_pos == POPPOS_BOTLEFT))
	{
	    // adjust leftwise to fit text on screen
	    صحيح لغزش_by = len + margin_عرض - maxwidth;

	    إذا (لغزش_by > wp->w_wincol)
	    {
		صحيح truncate_لغزش = لغزش_by - wp->w_wincol;

		لغزش_by -= truncate_لغزش;
	    }

	    // When maxwidth is explicitly set, don't shift beyond it, the text
	    // is wrapped or truncated instead.
	    إذا (wp->w_maxwidth > 0 && maxwidth + لغزش_by > wp->w_maxwidth)
		لغزش_by = wp->w_maxwidth - maxwidth;

	    إذا (لغزش_by > 0)
	    {
		wp->w_wincol -= لغزش_by;
		maxwidth += لغزش_by;
	    }
	    wp->w_عرض = maxwidth;
	}
	إذا (wp->w_p_wrap)
	{
	    طالما (len + margin_عرض > maxwidth)
	    {
		++wrapped;
		len -= maxwidth - margin_عرض;
		wp->w_عرض = maxwidth;
		استفاده_شده_maxwidth = TRUE;
	    }
	}
	إذا (wp->w_عرض < len + margin_عرض)
	{
	    wp->w_عرض = len + margin_عرض;
	    إذا (wp->w_maxwidth > 0 && wp->w_عرض > wp->w_maxwidth)
		wp->w_عرض = wp->w_maxwidth;
	}

	إذا (wp->w_firstline < 0)
	    --lnum;
	وإلا
	    ++lnum;

	// stop at the last line that is going to be shown; the width was
	// already computed above from every buffer line
	إذا (maxheight > 0
		   && (wp->w_firstline >= 0
			       ? lnum - wp->w_topline
			       : wp->w_میانگیر->b_ml.ml_سطر_شمار - lnum)
		       + wrapped >= maxheight)
	    اكسر;
    }

    إذا (wp->w_firstline < 0)
	wp->w_topline = lnum + 1;

    wp->w_has_scrollbar = wp->w_want_scrollbar
	   && (wp->w_topline > 1 || lnum <= wp->w_میانگیر->b_ml.ml_سطر_شمار);
#اگر_تعریف_شده FEAT_TERMINAL
    إذا (wp->w_میانگیر->b_جمله != عدم && !جمله_is_پایانیافته(wp->w_میانگیر))
	// Terminal window with running job never has a scrollbar, adjusts to
	// window height.
	wp->w_has_scrollbar = false;
#پایان_اگر
    maxwidth_no_scrollbar = maxwidth;
    إذا (wp->w_has_scrollbar)
    {
	++راست_extra;
	++extra_عرض;
	// give up the right padding when the popup would not fit, when lines
	// wrap and when applying minwidth
	إذا (maxwidth + راست_extra >= maxspace
		&& (استفاده_شده_maxwidth || (minwidth > 0 && wp->w_عرض < minwidth)))
	    maxwidth -= wp->w_popup_padding[1];
    }

    إذا (wp->w_popup_title != عدم && *wp->w_popup_title != NUL)
    {
	صحيح title_len = vim_strsize(wp->w_popup_title) + 2 - extra_عرض;

	إذا (minwidth < title_len)
	    minwidth = title_len;
    }

    إذا (minwidth > 0 && wp->w_عرض < minwidth)
	wp->w_عرض = minwidth;
    إذا (wp->w_عرض > maxwidth)
    {
	إذا (wp->w_عرض > maxspace && !wp->w_p_wrap)
	    // some columns cut off on the right
	    wp->w_popup_rightoff = wp->w_عرض - maxspace;

	// If the window doesn't fit because 'minwidth' is set then the
	// scrollbar is at the far right of the screen, use the size without
	// the scrollbar.
	إذا (wp->w_has_scrollbar && wp->w_minwidth > 0)
	{
	    صحيح off = wp->w_عرض - maxwidth;
	    extra_عرض -= MIN(off, راست_extra);
	    wp->w_عرض = maxwidth_no_scrollbar;
	}
	وإلا
	{
	    wp->w_عرض = maxwidth;

	    // when adding a scrollbar below need to adjust the width
	    عرض_with_scrollbar = maxwidth_no_scrollbar - راست_extra;
	}
    }
    إذا (center_hor)
    {
	wp->w_wincol = firstwin->w_wincol
		    + (topframe->fr_عرض - wp->w_عرض - extra_عرض) / 2;
	إذا (wp->w_wincol < firstwin->w_wincol)
	    wp->w_wincol = firstwin->w_wincol;
    }
    وإلا إذا (wp->w_popup_pos == POPPOS_BOTRIGHT
	    || wp->w_popup_pos == POPPOS_TOPRIGHT)
    {
	صحيح leftoff = wantcol - (wp->w_عرض + extra_عرض);

	// Right aligned: move to the right if needed.
	// No truncation, because that would change the height.
	إذا (leftoff >= 0)
	    wp->w_wincol = leftoff;
	وإلا إذا (wp->w_popup_fixed)
	{
	    // "col" specifies the right edge, but the popup doesn't fit.
	    // Scroll the text instead of moving the border off the screen.
	    wp->w_leftcol = -leftoff;
	    wp->w_عرض -= wp->w_leftcol;
	    إذا (wp->w_عرض < 0)
		wp->w_عرض = 0;
	}
    }

    إذا (wp->w_p_wrap || (!wp->w_popup_fixed
			    && (wp->w_popup_pos == POPPOS_TOPLEFT
				|| wp->w_popup_pos == POPPOS_BOTLEFT)))
    {
	صحيح want_ستون = 0;

	// try to show the right border and any scrollbar
	want_ستون = چپ_extra + wp->w_عرض + راست_extra;
	إذا (want_ستون > 0 && wp->w_wincol > 0
					 && wp->w_wincol + want_ستون >= firstwin->w_wincol + topframe->fr_عرض)
	{
	    wp->w_wincol = firstwin->w_wincol + topframe->fr_عرض - want_ستون;
	    إذا (wp->w_wincol < 0)
		wp->w_wincol = 0;
	}
    }

    wp->w_ارتفاع = wp->w_میانگیر->b_ml.ml_سطر_شمار - wp->w_topline
								 + 1 + wrapped;
    إذا (minheight > 0 && wp->w_ارتفاع < minheight)
	wp->w_ارتفاع = minheight;
    إذا (maxheight > 0 && wp->w_ارتفاع > maxheight)
	wp->w_ارتفاع = maxheight;
    w_ارتفاع_before_حد = wp->w_ارتفاع;
    إذا (wp->w_ارتفاع > ردیفها - wp->w_winrow)
	wp->w_ارتفاع = ردیفها - wp->w_winrow;

    إذا (center_vert)
    {
	wp->w_winrow = (ردیفها - wp->w_ارتفاع - extra_ارتفاع) / 2;
	إذا (wp->w_winrow < 0)
	    wp->w_winrow = 0;
    }
    وإلا إذا (wp->w_popup_pos == POPPOS_BOTRIGHT
		|| wp->w_popup_pos == POPPOS_BOTLEFT)
    {
	إذا ((wp->w_ارتفاع + extra_ارتفاع) <= wantline
		|| (wp->w_popup_flags & POPF_CLIPWINDOW))
	    // bottom aligned: may move down.  With "clipwindow" the popup
	    // keeps its natural position even if it overflows the screen,
	    // because the clip logic handles the overflow.
	    wp->w_winrow = wantline - (wp->w_ارتفاع + extra_ارتفاع);
	وإلا إذا (wantline * 2 >= ردیفها || !(wp->w_popup_flags & POPF_POSINVERT))
	{
	    // Bottom aligned but does not fit, and less space on the other
	    // side or "posinvert" is off: reduce height.
	    wp->w_winrow = 0;
	    wp->w_ارتفاع = wantline - extra_ارتفاع;
	}
	وإلا
	{
	    // Not enough space and more space on the other side: make top
	    // aligned.
	    wp->w_winrow = (wantline < 0 ? 0 : wantline) + 1;
	    adjust_ارتفاع_for_بالاترین_aligned = TRUE;
	}
    }
    وإلا إذا (wp->w_popup_pos == POPPOS_TOPRIGHT
		|| wp->w_popup_pos == POPPOS_TOPLEFT)
    {

	صحيح check_ارتفاع = (wp->w_popup_flags & POPF_INFO) ? wp->w_ارتفاع
						    : w_ارتفاع_before_حد;
	إذا (wp != popup_dragwin
		&& wantline + (check_ارتفاع + extra_ارتفاع) - 1 > ردیفها
		&& wantline * 2 > ردیفها
		&& (wp->w_popup_flags & POPF_POSINVERT))
	{
	    // top aligned and not enough space below but there is space above:
	    // make bottom aligned and recompute the height
	    wp->w_ارتفاع = w_ارتفاع_before_حد;
	    wp->w_winrow = wantline - 2 - wp->w_ارتفاع - extra_ارتفاع;
	    إذا (wp->w_popup_flags & POPF_INFO)
		wp->w_winrow += extra_ارتفاع + 2;
	    إذا (wp->w_winrow < 0)
	    {
		wp->w_ارتفاع += wp->w_winrow;
		wp->w_winrow = 0;
	    }
	}
	وإلا
	{
	    wp->w_winrow = wantline - 1;
	    adjust_ارتفاع_for_بالاترین_aligned = TRUE;
	}
    }

    إذا (adjust_ارتفاع_for_بالاترین_aligned && wp->w_want_scrollbar
			  && !(wp->w_popup_flags & POPF_CLIPWINDOW)
			  && wp->w_winrow + wp->w_ارتفاع + extra_ارتفاع > ردیفها)
    {
	// Bottom of the popup goes below the last line, reduce the height and
	// add a scrollbar.  For "clipwindow" popups the host-window clip
	// already truncates the popup to fit inside the host, so we must not
	// also force a scrollbar here -- that would widen the popup by one
	// column the moment its decoration crossed the screen edge.
	wp->w_ارتفاع = ردیفها - wp->w_winrow - extra_ارتفاع;
#اگر_تعریف_شده FEAT_TERMINAL
	إذا (wp->w_میانگیر->b_جمله == عدم || جمله_is_پایانیافته(wp->w_میانگیر))
#پایان_اگر
	{
	    // The width already accounts for a scrollbar that was there.
	    إذا (!wp->w_has_scrollbar)
	    {
		wp->w_has_scrollbar = true;
		إذا (عرض_with_scrollbar > 0)
		    wp->w_عرض = عرض_with_scrollbar;
		وإلا
		    // The width and the position were computed without the
		    // scrollbar, reserve a column for it.
		    ++extra_عرض;
	    }
	}
    }

    // make sure w_winrow is valid
    إذا (wp->w_winrow >= ردیفها)
	wp->w_winrow = ردیفها - 1;
    وإلا إذا (wp->w_winrow < 0 && !(wp->w_popup_flags & POPF_CLIPWINDOW))
	wp->w_winrow = 0;

    إذا (wp->w_wincol + wp->w_عرض + extra_عرض
				    > firstwin->w_wincol + topframe->fr_عرض)
	wp->w_wincol = firstwin->w_wincol + topframe->fr_عرض
						- wp->w_عرض - extra_عرض;
    إذا (wp->w_wincol < firstwin->w_wincol)
	wp->w_wincol = firstwin->w_wincol;
    إذا (wp->w_wincol < 0)
	wp->w_wincol = 0;
    // If the popup is wider than the available area (e.g. minwidth larger than
    // the work area between tabpanels), clip the content width so the right
    // border/padding/shadow stays visible instead of being pushed off the
    // screen or into the tabpanel.
    إذا (wp->w_wincol + wp->w_عرض + extra_عرض
				    > firstwin->w_wincol + topframe->fr_عرض)
    {
	صحيح avail = firstwin->w_wincol + topframe->fr_عرض
						- wp->w_wincol - extra_عرض;
	wp->w_عرض = avail > 0 ? avail : 0;
    }

    // Same for the bottom edge: shift up so the border/padding/shadow stays
    // on screen, and clip the height if the popup is taller than the screen.
    // For "clipwindow" popups the host-window clip below handles overflow, so
    // skip these screen-edge clamps -- otherwise a synthesised negative
    // w_winrow (popup partially above the host's top edge) would be snapped
    // back to 0 and defeat the top-clip animation.
    إذا (!(wp->w_popup_flags & POPF_CLIPWINDOW))
    {
	إذا (wp->w_winrow + wp->w_ارتفاع + extra_ارتفاع > ردیفها)
	    wp->w_winrow = ردیفها - wp->w_ارتفاع - extra_ارتفاع;
	إذا (wp->w_winrow < 0)
	    wp->w_winrow = 0;
	إذا (wp->w_winrow + wp->w_ارتفاع + extra_ارتفاع > ردیفها)
	{
	    صحيح avail = ردیفها - wp->w_winrow - extra_ارتفاع;
	    wp->w_ارتفاع = avail > 0 ? avail : 0;
	}
    }

    إذا (wp->w_ارتفاع != org_layout.ارتفاع)
	win_comp_scroll(wp);

    // Confine the popup to its host window for "clipwindow".  The popup's
    // logical geometry stays untouched; only w_popup_topoff/bottomoff/
    // leftclip/rightclip record how many rows/columns of each edge fall
    // outside the host so the drawing code can skip them.  When the popup
    // has fully scrolled past one of the host edges, hide it instead of
    // leaving stray decorations behind.
    إذا (popup_compute_clipwindow_offsets(wp))
    {
#اگر_تعریف_شده FEAT_IMAGE_POPUP
	إذا (!(wp->w_popup_flags & POPF_HIDDEN))
	    // Clear placement before hiding, like popup_hide()
	    إذا (wp->w_popup_imagep != عدم)
		image_placement_clear(wp->w_popup_imagep);
#پایان_اگر

	popup_hide_for_textprop(wp);
	أرجع;
    }

    // Make w_winrow the first visible screen row (>= 0); the clipped-off top
    // rows are recorded in w_popup_topoff.
    wp->w_winrow += wp->w_popup_topoff;

    wp->w_popup_آخرین_changedtick = CHANGEDTICK(wp->w_میانگیر);
    إذا (win_valid(wp->w_popup_prop_win))
    {
	wp->w_popup_prop_changedtick =
				   CHANGEDTICK(wp->w_popup_prop_win->w_میانگیر);
	wp->w_popup_prop_topline = wp->w_popup_prop_win->w_topline;
	wp->w_popup_prop_winrow = wp->w_popup_prop_win->w_winrow;
	wp->w_popup_prop_wincol = wp->w_popup_prop_win->w_wincol;
	wp->w_popup_prop_عرض = wp->w_popup_prop_win->w_عرض;
	wp->w_popup_prop_winheight = wp->w_popup_prop_win->w_ارتفاع;
    }

    // Need to update popup_mask if the position or size changed.
    // And redraw windows and statuslines that were behind the popup.
    إذا (popup_layout_تغییریافته(wp, &org_layout))
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	إذا (wp->w_popup_flags & POPF_ON_CMDLINE)
	    clear_cmdline = TRUE;
	popup_ماسک_refresh = TRUE;
    }

#اگر_تعریف_شده FEAT_IMAGE_POPUP
    popup_position_image(wp);
#پایان_اگر
}

تعريفنوع تعداد
{
    TYPE_نرمال,
    TYPE_ATCURSOR,
    TYPE_BEVAL,
    TYPE_NOTIFICATION,
    TYPE_پیام_WIN,	// similar to TYPE_NOTIFICATION
    TYPE_DIALOG,
    TYPE_MENU,
    TYPE_PREVIEW,	// preview window
    TYPE_INFO		// popup menu info
} create_type_T;

/*
 * Return TRUE if "type" is TYPE_NOTIFICATION or TYPE_MESSAGE_WIN.
 */
    ساكن صحيح
popup_is_notification(create_type_T type)
{
    أرجع type == TYPE_NOTIFICATION || type == TYPE_پیام_WIN;
}

/*
 * Make "buf" empty and set the contents to "text".
 * Used by popup_create() and popup_settext().
 */
    ساكن فراغ
popup_مجموعه_میانگیر_متن(buf_T *buf, typval_T متن)
{
    صحيح	    lnum;

    // Clear the buffer, then replace the lines.
    curbuf = buf;
    لكل (lnum = buf->b_ml.ml_سطر_شمار; lnum > 0; --lnum)
	ml_delete(lnum);
    curbuf = curwin->w_میانگیر;

    // Add text to the buffer.
    إذا (متن.v_type == VAR_STRING)
    {
	char_u *s = متن.vval.v_string;

	// just a string
	ml_append_buf(buf, 0, s == عدم ? (char_u *)"" : s, (colnr_T)0, TRUE);
    }
    وإلا
    {
	list_T *l = متن.vval.v_list;

	إذا (l != عدم && l->lv_len > 0)
	{
	    إذا (l->lv_اولی == &range_list_قلم)
		emsg(_(e_using_عدد_as_string));
	    وإلا إذا (l->lv_اولی->li_tv.v_type == VAR_STRING)
		// list of strings
		جمع_کن_popup_رشتهها(buf, l);
	    وإلا
		// list of dictionaries
		جمع_کن_popup_dicts(buf, l);
	}
    }

    // delete the line that was in the empty buffer
    curbuf = buf;
    ml_delete(buf->b_ml.ml_سطر_شمار);
    curbuf = curwin->w_میانگیر;
}

#تعریف_کن مجموعه_BORDER_CHARS(a0, a1, a2, a3, a4, a5, a6, a7)    \
    افعل {						    \
	إذا (wp != عدم)					    \
	{						    \
	    wp->w_border_char[0] = (a0);		    \
	    wp->w_border_char[1] = (a1);		    \
	    wp->w_border_char[2] = (a2);		    \
	    wp->w_border_char[3] = (a3);		    \
	    wp->w_border_char[4] = (a4);		    \
	    wp->w_border_char[5] = (a5);		    \
	    wp->w_border_char[6] = (a6);		    \
	    wp->w_border_char[7] = (a7);		    \
	}						    \
    } طالما (0)

/*
 * Parse the 'previewpopup' or 'completepopup' option and apply the values to
 * window "wp" if it is not NULL.
 * Return FAIL if the parsing fails.
 */
    ساكن صحيح
parse_popup_option(win_T *wp, صحيح is_preview)
{
    char_u *p =
#اگر_تعریف_شده FEAT_QUICKFIX
	!is_preview ? p_cpp :
#پایان_اگر
	p_pvp;
    صحيح	    border_enabled = FALSE;

    إذا (wp != عدم)
	wp->w_popup_flags &= ~POPF_INFO_MENU;

    لكل ( ; *p != NUL; p += (*p == ',' ? 1 : 0))
    {
	char_u	*e, *dig;
	char_u	*s = p;
	صحيح	x;

	e = vim_strchr(p, ':');
	إذا (e == عدم || e[1] == NUL)
	    أرجع FAIL;

	p = vim_strchr(e, ',');
	إذا (p == عدم)
	    p = e + STRLEN(e);
	dig = e + 1;
	x = getdigits(&dig);

	// Note: Keep this in sync with p_popup_option_values.
	إذا (STRNCMP(s, "height:", 7) == 0)
	{
	    إذا (dig != p)
		أرجع FAIL;
	    إذا (wp != عدم)
	    {
		إذا (is_preview)
		    wp->w_minheight = x;
		wp->w_maxheight = x;
	    }
	}
	وإلا إذا (STRNCMP(s, "width:", 6) == 0)
	{
	    إذا (dig != p)
		أرجع FAIL;
	    إذا (wp != عدم)
	    {
		إذا (is_preview)
		    wp->w_minwidth = x;
		wp->w_maxwidth = x;
		wp->w_maxwidth_opt = x;
	    }
	}
	وإلا إذا (STRNCMP(s, "highlight:", 10) == 0)
	{
	    إذا (wp != عدم)
	    {
		حرف *errmsg;
		صحيح c = *p;

		*p = NUL;

		errmsg = update_wincolor(wp, s + 10);
		إذا (errmsg == عدم)
		    مجموعه_string_option_مستقیم_درون_win(wp, (char_u *)"wincolor",
			    -1, s + 10, OPT_FREE|OPT_LOCAL, 0);
		وإلا
		    emsg(_(errmsg));

		*p = c;
	    }
	}
	وإلا إذا (STRNCMP(s, "borderhighlight:", 16) == 0)
	{
	    char_u	*arg = s + 16;

	    إذا (*arg == NUL || *arg == ',')
		أرجع FAIL;
	    إذا (wp != عدم)
	    {
		لكل (صحيح i = 0; i < 4; ++i)
		{
		    VIM_CLEAR(wp->w_border_highlight[i]);
		    wp->w_border_highlight[i] = vim_strnsave(arg, p - arg);
		}
	    }
	}
	وإلا إذا (STRNCMP(s, "border:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    صحيح		i;
	    صحيح		token_len = p - arg;
	    char_u	*token;
	    // Use box-drawing characters only when 'encoding' is "utf-8" and
	    // 'ambiwidth' is "single".
	    صحيح		can_use_box_chars = (enc_utf8 && *p_ambw == 's');

	    إذا (token_len == 0
			|| (STRNCMP(arg, "off", 3) == 0 && arg + 3 == p))
	    {
		إذا (wp != عدم)
		{
		    لكل (i = 0; i < 4; ++i)
			wp->w_popup_border[i] = 0;
		    مجموعه_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		    // only show the X for close when there is a border
		    wp->w_popup_close = POPCLOSE_NONE;
		}
		استمر;
	    }

	    token = vim_strnsave(arg, token_len);
	    إذا (token == عدم)
		أرجع FAIL;

	    إذا ((can_use_box_chars && (STRCMP(token, "single") == 0
			    || STRCMP(token, "double") == 0
			    || STRCMP(token, "on") == 0
			    || STRCMP(token, "round") == 0))
		    || STRCMP(token, "ascii") == 0
		    || (STRNCMP(token, "custom:", 7) == 0))
	    {
		إذا (STRCMP(token, "single") == 0)
		    مجموعه_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x250c, 0x2510, 0x2518, 0x2514); // ┌ ┐ ┘ └
		وإلا إذا (STRCMP(token, "double") == 0)
		    مجموعه_BORDER_CHARS(0x2550, 0x2551, 0x2550, 0x2551, // ═ ║ ═ ║
			    0x2554, 0x2557, 0x255D, 0x255A); // ╔ ╗ ╝  ╚
		وإلا إذا (STRCMP(token, "round") == 0)
		    مجموعه_BORDER_CHARS(0x2500, 0x2502, 0x2500, 0x2502, // ─ │ ─ │
			    0x256d, 0x256e, 0x256f, 0x2570); // ╭ ╮ ╯ ╰
		وإلا إذا (STRCMP(token, "on") == 0)
		    مجموعه_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);
		وإلا إذا (STRCMP(token, "ascii") == 0)
		    مجموعه_BORDER_CHARS('-', '|', '-', '|', '+', '+', '+', '+');
		وإلا إذا (STRNCMP(token, "custom:", 7) == 0)
		{
		    char_u	*q = token + 7;
		    صحيح		بیرون[8];
		    صحيح		failed = FALSE;

		    مجموعه_BORDER_CHARS(0, 0, 0, 0, 0, 0, 0, 0);

		    لكل (i = 0; i < 8 && !failed; i++)
		    {
			إذا (*q == NUL)
			    failed = TRUE;
			وإلا
			{
			    بیرون[i] = mb_ptr2char(q);
			    mb_ptr2char_adv(&q);
			    إذا (i < 7)
			    {
				إذا (*q != ';')
				    failed = TRUE; // must be semicolon
				q++;
			    }
			}
		    }
		    إذا (failed || *q != NUL) // must end exactly after the 8th char
		    {
			vim_free(token);
			أرجع FAIL;
		    }
		    مجموعه_BORDER_CHARS(بیرون[0], بیرون[1], بیرون[2], بیرون[3], بیرون[4],
			    بیرون[5], بیرون[6], بیرون[7]);
		}
	    }
	    وإلا
	    {
		vim_free(token);
		أرجع FAIL;
	    }

	    إذا (wp != عدم)
	    {
		لكل (i = 0; i < 4; ++i)
		    wp->w_popup_border[i] = 1;
	    }
	    border_enabled = TRUE;

	    vim_free(token);
	}
	وإلا إذا (STRNCMP(s, "close:", 6) == 0)
	{
	    char_u	*arg = s + 6;
	    صحيح		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    صحيح		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    إذا (!on && !off)
		أرجع FAIL;
	    on = on && mouse_has(MOUSE_درج) && (border_enabled || is_preview);
	    إذا (wp != عدم)
		wp->w_popup_close = on ? POPCLOSE_BUTTON : POPCLOSE_NONE;
	}
	وإلا إذا (STRNCMP(s, "resize:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    صحيح		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    صحيح		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    إذا (!on && !off)
		أرجع FAIL;
	    إذا (wp != عدم)
	    {
		إذا (on && mouse_has(MOUSE_درج))
		    wp->w_popup_flags |= POPF_RESIZE;
		وإلا
		    wp->w_popup_flags &= ~POPF_RESIZE;
	    }
	}
	وإلا إذا (STRNCMP(s, "shadow:", 7) == 0)
	{
	    char_u	*arg = s + 7;
	    صحيح		on = STRNCMP(arg, "on", 2) == 0 && arg + 2 == p;
	    صحيح		off = STRNCMP(arg, "off", 3) == 0 && arg + 3 == p;

	    إذا (!on && !off)
		أرجع FAIL;
	    إذا (wp != عدم)
		wp->w_popup_shadow = on ? 1 : 0;
	}
	وإلا إذا (STRNCMP(s, "align:", 6) == 0)
	{
	    // Note: Keep this in sync with p_popup_option_align_values.
	    char_u	*arg = s + 6;
	    صحيح		قلم = STRNCMP(arg, "item", 4) == 0 && arg + 4 == p;
	    صحيح		menu = STRNCMP(arg, "menu", 4) == 0 && arg + 4 == p;

	    إذا (!menu && !قلم)
		أرجع FAIL;
	    إذا (wp != عدم && menu)
		wp->w_popup_flags |= POPF_INFO_MENU;
	}
	وإلا إذا (STRNCMP(s, "opacity:", 8) == 0)
	{
	    إذا (dig != p || x < 0 || x > 100)
		أرجع FAIL;
	    إذا (wp != عدم)
	    {
		إذا (x < 100)
		    wp->w_popup_flags |= POPF_OPACITY;
		وإلا
		    wp->w_popup_flags &= ~POPF_OPACITY;
		wp->w_popup_blend = 100 - x;
	    }
	}
	وإلا
	    أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Parse the 'previewpopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    صحيح
parse_previewpopup(win_T *wp)
{
    أرجع parse_popup_option(wp, TRUE);
}

/*
 * Parse the 'completepopup' option and apply the values to window "wp" if it
 * is not NULL.
 * Return FAIL if the parsing fails.
 */
    صحيح
parse_completepopup(win_T *wp)
{
    أرجع parse_popup_option(wp, FALSE);
}

/*
 * Set w_wantline and w_wantcol for the cursor position in the current window.
 * Keep at least "width" columns from the right of the screen.
 */
    فراغ
popup_مجموعه_wantpos_cursor(win_T *wp, صحيح عرض, dict_T *d)
{
    poppos_T ppt = POPPOS_NONE;

    إذا (d != عدم)
	ppt = get_pos_مدخل(d, FALSE);

    setcursor_mayforce(TRUE);
    إذا (ppt == POPPOS_TOPRIGHT || ppt == POPPOS_TOPLEFT)
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow + 2;
    }
    وإلا
    {
	wp->w_wantline = curwin->w_winrow + curwin->w_wrow;
	إذا (wp->w_wantline == 0)  // cursor in first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = ppt == POPPOS_BOTRIGHT
					    ? POPPOS_TOPRIGHT : POPPOS_TOPLEFT;
	}
    }

    wp->w_wantcol = curwin->w_wincol + curwin->w_wcol + 1;
    إذا (wp->w_wantcol > Columns - عرض)
    {
	wp->w_wantcol = Columns - عرض;
	إذا (wp->w_wantcol < 1)
	    wp->w_wantcol = 1;
    }

    popup_adjust_position(wp);
}

/*
 * Set w_wantline and w_wantcol for the a given screen position.
 * Caller must take care of running into the window border.
 */
    فراغ
popup_مجموعه_wantpos_rowcol(win_T *wp, صحيح ردیف, صحيح ستون)
{
    wp->w_wantline = ردیف;
    wp->w_wantcol = ستون;
    popup_adjust_position(wp);
}

/*
 * Add a border and lef&right padding.
 */
    ساكن فراغ
جمع_کن_border_چپ_راست_padding(win_T *wp)
{
    صحيح i;

    لكل (i = 0; i < 4; ++i)
    {
	wp->w_popup_border[i] = 1;
	wp->w_popup_padding[i] = (i & 1) ? 1 : 0;
    }
}

#اگر_تعریف_شده FEAT_TERMINAL
/*
 * Return TRUE if there is any popup window with a terminal buffer.
 */
    ساكن صحيح
popup_terminal_exists(فراغ)
{
    win_T	*wp;
    tabpage_T	*tp;

    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_میانگیر->b_جمله != عدم)
	    أرجع TRUE;
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_درون_TAB(tp, wp)
	    إذا (wp->w_میانگیر->b_جمله != عدم)
		أرجع TRUE;
    أرجع FALSE;
}
#پایان_اگر

/*
 * Mark all popup windows in the current tab and global for redrawing.
 */
    فراغ
popup_redraw_all(فراغ)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_redr_type = UPD_NOT_VALID;
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	wp->w_redr_type = UPD_NOT_VALID;
}

/*
 * Return TRUE if any visible popup window needs a redraw.
 */
    صحيح
popup_need_redraw(فراغ)
{
    win_T	*wp;

    FOR_ALL_POPUPWINS(wp)
	إذا ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    أرجع TRUE;
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	إذا ((wp->w_popup_flags & POPF_HIDDEN) == 0 && wp->w_redr_type != 0)
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Set the color for a notification window.
 */
    ساكن فراغ
popup_update_color(win_T *wp, create_type_T type)
{
    حرف    *hiname = type == TYPE_پیام_WIN
				       ? "MessageWindow" : "PopupNotification";
    حرف    *errmsg;

    errmsg = update_wincolor(wp, (char_u *)hiname);
    إذا (errmsg == عدم)
	مجموعه_string_option_مستقیم_درون_win(wp, (char_u *)"wincolor", -1,
		(char_u *)hiname, OPT_FREE|OPT_LOCAL, 0);
    وإلا
	emsg(_(errmsg));
}

/*
 * popup_create({text}, {options})
 * popup_atcursor({text}, {options})
 * etc.
 * When creating a preview or info popup "argvars" and "rettv" are NULL.
 */
    ساكن win_T *
popup_create(typval_T *argvars, typval_T *rettv, create_type_T type)
{
    win_T	*wp;
    tabpage_T	*tp = عدم;
    صحيح		tabnr = 0;
    صحيح		new_میانگیر;
    buf_T	*buf = عدم;
    dict_T	*d = عدم;
    صحيح		i;

    إذا (check_secure())
	أرجع عدم;

    إذا (argvars != عدم)
    {
	إذا (درون_vim9script()
		&& (check_for_string_or_عدد_or_list_arg(argvars, 0) == FAIL
		    || check_for_dict_arg(argvars, 1) == FAIL))
	    أرجع عدم;

	// Check that arguments look OK.
	إذا (argvars[0].v_type == VAR_عدد)
	{
	    buf = buflist_findnr(argvars[0].vval.v_عدد);
	    إذا (buf == عدم)
	    {
		semsg(_(e_میانگیر_nr_does_not_exist), argvars[0].vval.v_عدد);
		أرجع عدم;
	    }
#اگر_تعریف_شده FEAT_TERMINAL
	    إذا (buf->b_جمله != عدم && popup_terminal_exists())
	    {
		emsg(_(e_cannot_open_دومی_popup_with_terminal));
		أرجع عدم;
	    }
#پایان_اگر
	}
	وإلا إذا (!(argvars[0].v_type == VAR_STRING
			&& argvars[0].vval.v_string != عدم)
		    && !(argvars[0].v_type == VAR_LIST
			&& argvars[0].vval.v_list != عدم))
	{
	    emsg(_(e_میانگیر_عدد_متن_or_list_required));
	    أرجع عدم;
	}
	إذا (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	    أرجع عدم;
	d = argvars[1].vval.v_dict;
    }

    إذا (d != عدم)
    {
	إذا (dict_has_کلید(d, "tabpage"))
	    tabnr = (صحيح)dict_get_عدد(d, "tabpage");
	وإلا إذا (popup_is_notification(type))
	    tabnr = -1;  // notifications are global by default
	وإلا
	    tabnr = 0;
	إذا (tabnr > 0)
	{
	    tp = پیدا_کن_tabpage(tabnr);
	    إذا (tp == عدم)
	    {
		semsg(_(e_tabpage_not_یافت_nr), tabnr);
		أرجع عدم;
	    }
	}
    }
    وإلا إذا (popup_is_notification(type))
	tabnr = -1;  // show on all tabs

    إذا (buf != عدم && buf->b_locked_تقسیم)
    {
	// disallow opening a popup to a closing buffer, which like splitting,
	// can result in more windows displaying it
	emsg(_(e_cannot_open_a_popup_پنجره_مقصد_a_closing_میانگیر));
	أرجع عدم;
    }

    // Create the window and buffer.
    wp = win_alloc_popup_win();
    إذا (wp == عدم)
	أرجع عدم;
    إذا (rettv != عدم)
	rettv->vval.v_عدد = wp->w_id;
    wp->w_popup_pos = POPPOS_TOPLEFT;
    wp->w_popup_flags = POPF_IS_POPUP | POPF_MAPPING | POPF_POSINVERT;

    إذا (buf != عدم)
    {
	// use existing buffer
	new_میانگیر = FALSE;
	win_init_popup_win(wp, buf);
	مجموعه_local_options_default(wp, FALSE);
	جابجا_exists_action = SEA_READONLY;
	میانگیر_ensure_loaded(buf);
	جابجا_exists_action = SEA_NONE;
    }
    وإلا
    {
	// create a new buffer associated with the popup
	new_میانگیر = TRUE;
	buf = buflist_new(عدم, عدم, (linenr_T)0, BLN_NEW|BLN_DUMMY|BLN_REUSE);
	إذا (buf == عدم)
	{
	    win_free_popup(wp);
	    أرجع عدم;
	}
	ml_open(buf);

	win_init_popup_win(wp, buf);

	مجموعه_local_options_default(wp, TRUE);
	مجموعه_string_option_مستقیم_درون_buf(buf, (char_u *)"buftype", -1,
				     (char_u *)"popup", OPT_FREE|OPT_LOCAL, 0);
	مجموعه_string_option_مستقیم_درون_buf(buf, (char_u *)"bufhidden", -1,
				      (char_u *)"wipe", OPT_FREE|OPT_LOCAL, 0);
	buf->b_p_ul = -1;	// no undo
	buf->b_p_swf = FALSE;   // no swap file
	buf->b_p_bl = FALSE;    // unlisted buffer
	buf->b_locked = TRUE;	// prevent deleting the buffer

	// Avoid that 'buftype' is reset when this buffer is entered.
	buf->b_p_initialized = true;
    }
    wp->w_p_wrap = TRUE;	// 'wrap' is default on
    wp->w_p_so = 0;		// 'scrolloff' zero
    wp->w_p_sop = 0;		// 'scrolloffpad' zero

    إذا (tp != عدم)
    {
	// popup on specified tab page
	wp->w_بعدی = tp->tp_اولی_popupwin;
	tp->tp_اولی_popupwin = wp;
    }
    وإلا إذا (tabnr == 0)
    {
	// popup on current tab page
	wp->w_بعدی = curtab->tp_اولی_popupwin;
	curtab->tp_اولی_popupwin = wp;
    }
    وإلا // (tabnr < 0)
    {
	win_T *prev = اولی_popupwin;

	// Global popup: add at the end, so that it gets displayed on top of
	// older ones with the same zindex. Matters for notifications.
	إذا (اولی_popupwin == عدم)
	    اولی_popupwin = wp;
	وإلا
	{
	    طالما (prev->w_بعدی != عدم)
		prev = prev->w_بعدی;
	    prev->w_بعدی = wp;
	}
#اگر_تعریف_شده FEAT_IMAGE_POPUP
	wp->w_popup_flags |= POPF_GLOBAL;
#پایان_اگر
    }

    إذا (new_میانگیر && argvars != عدم)
	popup_مجموعه_میانگیر_متن(buf, argvars[0]);

    إذا (type == TYPE_ATCURSOR || type == TYPE_PREVIEW)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;
    }
    إذا (type == TYPE_ATCURSOR)
    {
	popup_مجموعه_wantpos_cursor(wp, 0, d);
	مجموعه_moved_مقادیر(wp);
	مجموعه_moved_columns(wp, پیدا_کن_STRING);
    }

    إذا (type == TYPE_BEVAL)
    {
	wp->w_popup_pos = POPPOS_BOTLEFT;

	// by default use the mouse position
	wp->w_wantline = mouse_ردیف;
	إذا (wp->w_wantline <= 0)  // mouse on first line
	{
	    wp->w_wantline = 2;
	    wp->w_popup_pos = POPPOS_TOPLEFT;
	}
	wp->w_wantcol = mouse_ستون + 1;
	مجموعه_mousemoved_مقادیر(wp);
	مجموعه_mousemoved_columns(wp, پیدا_کن_IDENT + پیدا_کن_STRING + پیدا_کن_EVAL);
    }

    // set default values
    wp->w_zindex = POPUPWIN_DEFAULT_ZINDEX;
    wp->w_popup_close = POPCLOSE_NONE;

    إذا (popup_is_notification(type))
    {
	win_T  *twp, *nextwin;
	صحيح	ارتفاع = buf->b_ml.ml_سطر_شمار + 3;

	// Try to not overlap with another global popup.  Guess we need 3
	// more screen lines than buffer lines.
	wp->w_wantline = 1;
	لكل (twp = اولی_popupwin; twp != عدم; twp = nextwin)
	{
	    nextwin = twp->w_بعدی;
	    إذا (twp != wp
		    && twp->w_zindex == POPUPWIN_NOTIFICATION_ZINDEX
		    && twp->w_winrow <= wp->w_wantline - 1 + ارتفاع
		    && twp->w_winrow + popup_ارتفاع(twp) > wp->w_wantline - 1)
	    {
		// move to below this popup and restart the loop to check for
		// overlap with other popups
		wp->w_wantline = twp->w_winrow + popup_ارتفاع(twp) + 1;
		nextwin = اولی_popupwin;
	    }
	}
	إذا (wp->w_wantline + ارتفاع > ردیفها)
	{
	    // can't avoid overlap, put on top in the hope that message goes
	    // away soon.
	    wp->w_wantline = 1;
	}

	wp->w_wantcol = 10;
	wp->w_zindex = POPUPWIN_NOTIFICATION_ZINDEX;
	wp->w_minwidth = 20;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_close = POPCLOSE_CLICK;
	لكل (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	wp->w_popup_padding[1] = 1;
	wp->w_popup_padding[3] = 1;

	popup_update_color(wp, type);
    }

    إذا (type == TYPE_DIALOG || type == TYPE_MENU)
    {
	wp->w_popup_pos = POPPOS_CENTER;
	wp->w_zindex = POPUPWIN_DIALOG_ZINDEX;
	wp->w_popup_flags |= POPF_DRAG;
	wp->w_popup_flags &= ~POPF_MAPPING;
	جمع_کن_border_چپ_راست_padding(wp);
    }

    إذا (type == TYPE_MENU)
    {
	typval_T	tv;
	callback_T	callback;

	tv.v_type = VAR_STRING;
	tv.vval.v_string = (char_u *)"popup_filter_menu";
	callback = get_callback(&tv);
	إذا (callback.cb_name != عدم)
	{
	    مجموعه_callback(&wp->w_filter_cb, &callback);
	}

	wp->w_p_wrap = 0;
	wp->w_popup_flags |= POPF_CURSORLINE;
    }

    لكل (i = 0; i < 4; ++i)
	VIM_CLEAR(wp->w_border_highlight[i]);
    لكل (i = 0; i < 8; ++i)
	wp->w_border_char[i] = 0;

    إذا (type == TYPE_PREVIEW)
    {
	إذا (mouse_has(MOUSE_درج))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	لكل (i = 0; i < 4; ++i)
	    wp->w_popup_border[i] = 1;
	parse_previewpopup(wp);
	popup_مجموعه_wantpos_cursor(wp, wp->w_minwidth, d);
    }

#اگر_تعریف_شده FEAT_QUICKFIX
    إذا (type == TYPE_INFO)
    {
	wp->w_popup_pos = POPPOS_TOPLEFT;
	إذا (mouse_has(MOUSE_درج))
	{
	    wp->w_popup_flags |= POPF_DRAG | POPF_RESIZE;
	    wp->w_popup_close = POPCLOSE_BUTTON;
	}
	جمع_کن_border_چپ_راست_padding(wp);
	parse_completepopup(wp);
    }
#پایان_اگر

    wp->w_want_scrollbar = 1;
    wp->w_popup_fixed = 0;
    wp->w_filter_نما = نما_ALL;

    إذا (d != عدم)
    {
	// Deal with options.
	إذا (apply_options(wp, d, TRUE) == FAIL)
	{
	    (فراغ)popup_close(wp->w_id, FALSE);
	    أرجع عدم;
	}
    }

#اگر_تعریف_شده FEAT_TIMERS
    إذا (popup_is_notification(type) && wp->w_popup_timer == عدم)
	popup_جمع_کن_timeout(wp, 3000, type == TYPE_NOTIFICATION);
#پایان_اگر

    popup_adjust_position(wp);

    wp->w_vsep_عرض = 0;

    redraw_all_later(UPD_NOT_VALID);
    popup_ماسک_refresh = TRUE;

#اگر_تعریف_شده FEAT_TERMINAL
    // When running a terminal in the popup it becomes the current window.
    إذا (buf->b_جمله != عدم)
	win_enter(wp, FALSE);
#پایان_اگر

    أرجع wp;
}

/*
 * popup_clear()
 */
    فراغ
f_popup_clear(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    صحيح force = FALSE;

    إذا (درون_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
	force = (صحيح)tv_get_bool(&argvars[0]);
    close_all_popups(force);
}

/*
 * popup_create({text}, {options})
 */
    فراغ
f_popup_create(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_نرمال);
}

/*
 * popup_atcursor({text}, {options})
 */
    فراغ
f_popup_atcursor(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_ATCURSOR);
}

/*
 * popup_beval({text}, {options})
 */
    فراغ
f_popup_beval(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_BEVAL);
}

/*
 * Invoke the close callback for window "wp" with value "result".
 * Careful: The callback may make "wp" invalid!
 */
    ساكن فراغ
invoke_popup_callback(win_T *wp, typval_T *نتیجه)
{
    typval_T	rettv;
    typval_T	argv[3];

    rettv.v_type = VAR_UNKNOWN;

    argv[0].v_type = VAR_عدد;
    argv[0].vval.v_عدد = (varnumber_T)wp->w_id;

    إذا (نتیجه != عدم && نتیجه->v_type != VAR_UNKNOWN)
	copy_tv(نتیجه, &argv[1]);
    وإلا
    {
	argv[1].v_type = VAR_عدد;
	argv[1].vval.v_عدد = 0;
    }

    argv[2].v_type = VAR_UNKNOWN;

    call_callback(&wp->w_close_cb, -1, &rettv, 2, argv);
    إذا (نتیجه != عدم)
	clear_tv(&argv[1]);
    clear_tv(&rettv);
}

/*
 * Make "prevwin" the current window, unless it's equal to "wp".
 * Otherwise make "firstwin" the current window.
 */
    ساكن فراغ
پشت_مقصد_prevwin(win_T *wp)
{
    إذا (win_valid(prevwin) && wp != prevwin)
	win_enter(prevwin, FALSE);
    وإلا
	win_enter(firstwin, FALSE);
}

/*
 * Close popup "wp" and invoke any close callback for it.
 * Careful: callback function might have freed the popup window already
 */
    ساكن فراغ
popup_close_and_callback(win_T *wp, typval_T *arg)
{
    صحيح id;

    إذا (!win_valid(wp))
       أرجع;

    id = wp->w_id;

#اگر_تعریف_شده FEAT_TERMINAL
    إذا (wp == curwin && curbuf->b_جمله != عدم)
    {
	win_T *owp;

	// Closing popup window with a terminal: put focus back on the first
	// that works:
	// - another popup window with a terminal
	// - the previous window
	// - the first one.
	FOR_ALL_POPUPWINS(owp)
	    إذا (owp != curwin && owp->w_میانگیر->b_جمله != عدم)
		اكسر;
	إذا (owp != عدم)
	    win_enter(owp, FALSE);
	وإلا
	{
	    FOR_ALL_POPUPWINS_درون_TAB(curtab, owp)
		إذا (owp != curwin && owp->w_میانگیر->b_جمله != عدم)
		    اكسر;
	    إذا (owp != عدم)
		win_enter(owp, FALSE);
	    وإلا
		پشت_مقصد_prevwin(wp);
	}
    }
#پایان_اگر

    // Just in case a check higher up is missing.
    إذا (wp == curwin && ERROR_IF_POPUP_پنجره)
    {
	// To avoid getting stuck when win_execute() does something that causes
	// an error, stop calling the filter callback.
	free_callback(&wp->w_filter_cb);

	أرجع;
    }

    CHECK_CURBUF;
    إذا (wp->w_close_cb.cb_name != عدم)
	// Careful: This may make "wp" invalid.
	invoke_popup_callback(wp, arg);

    popup_close(id, FALSE);
    CHECK_CURBUF;
}

    فراغ
popup_close_with_retval(win_T *wp, صحيح retval)
{
    typval_T res;

    res.v_type = VAR_عدد;
    res.vval.v_عدد = retval;
    popup_close_and_callback(wp, &res);
}

/*
 * Close popup "wp" because of a mouse click.
 */
    فراغ
popup_close_for_mouse_click(win_T *wp)
{
    popup_close_with_retval(wp, -2);
}

    ساكن فراغ
check_mouse_moved(win_T *wp, win_T *mouse_wp)
{
    // Close the popup when all if these are true:
    // - the mouse is not on this popup
    // - "mousemoved" was used
    // - the mouse is no longer on the same screen row or the mouse column is
    //   outside of the relevant text
    إذا (wp != mouse_wp
	    && wp->w_popup_mouse_ردیف != 0
	    && (wp->w_popup_mouse_ردیف != mouse_ردیف
		|| mouse_ستون < wp->w_popup_mouse_mincol
		|| mouse_ستون > wp->w_popup_mouse_maxcol))
    {
	// Careful: this makes "wp" invalid.
	popup_close_with_retval(wp, -2);
    }
}

/*
 * Called when the mouse moved: may close a popup with "mousemoved".
 */
    فراغ
popup_handle_mouse_moved(فراغ)
{
    win_T   *wp, *nextwp;
    win_T   *mouse_wp;
    صحيح	    ردیف = mouse_ردیف;
    صحيح	    ستون = mouse_ستون;

    // find the window where the mouse is in
    mouse_wp = mouse_پیدا_کن_win(&ردیف, &ستون, پیدا_کن_POPUP);

    لكل (wp = اولی_popupwin; wp != عدم; wp = nextwp)
    {
	nextwp = wp->w_بعدی;
	check_mouse_moved(wp, mouse_wp);
    }
    لكل (wp = curtab->tp_اولی_popupwin; wp != عدم; wp = nextwp)
    {
	nextwp = wp->w_بعدی;
	check_mouse_moved(wp, mouse_wp);
    }
}

/*
 * In a filter: check if the typed key is a mouse event that is used for
 * dragging the popup.
 */
    ساكن فراغ
filter_handle_drag(win_T *wp, صحيح c, typval_T *rettv)
{
    صحيح	ردیف = mouse_ردیف;
    صحيح	ستون = mouse_ستون;

    إذا ((wp->w_popup_flags & (POPF_DRAG | POPF_DRAGALL))
	    && is_mouse_کلید(c)
	    && (wp == popup_dragwin
			  || wp == mouse_پیدا_کن_win(&ردیف, &ستون, پیدا_کن_POPUP)))
	// do not consume the key, allow for dragging the popup
	rettv->vval.v_عدد = 0;
}

/*
 * popup_filter_menu({id}, {key})
 */
    فراغ
f_popup_filter_menu(typval_T *argvars, typval_T *rettv)
{
    صحيح		id;
    win_T	*wp;
    char_u	*کلید;
    typval_T	res;
    صحيح		c;
    linenr_T	old_lnum;

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    id = tv_get_عدد(&argvars[0]);
    wp = win_id2wp(id);
    کلید = tv_get_string(&argvars[1]);
    // If the popup has been closed do not consume the key.
    إذا (wp == عدم)
	أرجع;

    c = *کلید;
    إذا (c == K_SPECIAL && کلید[1] != NUL)
	c = مقصد_SPECIAL(کلید[1], کلید[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_TRUE;
    res.v_type = VAR_عدد;

    old_lnum = wp->w_cursor.lnum;
    إذا (c == 'k' || c == 'K' || c == K_به_بالا || c == Ctrl_P)
    {
	إذا (wp->w_cursor.lnum > 1)
	    --wp->w_cursor.lnum;
	وإلا
	    wp->w_cursor.lnum = wp->w_میانگیر->b_ml.ml_سطر_شمار;
    }
    إذا (c == 'j' || c == 'J' || c == K_پایین || c == Ctrl_N)
    {
	إذا (wp->w_cursor.lnum < wp->w_میانگیر->b_ml.ml_سطر_شمار)
	    ++wp->w_cursor.lnum;
	وإلا
	    wp->w_cursor.lnum = 1;
    }
    إذا (old_lnum != wp->w_cursor.lnum)
    {
	// caller will call popup_highlight_curline()
	أرجع;
    }

    إذا (c == 'x' || c == 'X' || c == ESC || c == Ctrl_C)
    {
	// Cancelled, invoke callback with -1
	res.vval.v_عدد = -1;
	popup_close_and_callback(wp, &res);
	أرجع;
    }
    إذا (c == ' ' || c == K_KENTER || c == CAR || c == NL)
    {
	// Invoke callback with current index.
	res.vval.v_عدد = wp->w_cursor.lnum;
	popup_close_and_callback(wp, &res);
	أرجع;
    }

    filter_handle_drag(wp, c, rettv);
}

/*
 * popup_filter_yesno({id}, {key})
 */
    فراغ
f_popup_filter_yesno(typval_T *argvars, typval_T *rettv)
{
    صحيح		id;
    win_T	*wp;
    char_u	*کلید;
    typval_T	res;
    صحيح		c;

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    id = tv_get_عدد(&argvars[0]);
    wp = win_id2wp(id);
    کلید = tv_get_string(&argvars[1]);
    // If the popup has been closed don't consume the key.
    إذا (wp == عدم)
	أرجع;

    c = *کلید;
    إذا (c == CAR && need_wait_return)
	أرجع;
    إذا (c == K_SPECIAL && کلید[1] != NUL)
	c = مقصد_SPECIAL(کلید[1], کلید[2]);

    // consume all keys until done
    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_TRUE;

    إذا (c == 'y' || c == 'Y')
	res.vval.v_عدد = 1;
    وإلا إذا (c == 'n' || c == 'N' || c == 'x' || c == 'X' || c == ESC)
	res.vval.v_عدد = 0;
    وإلا
    {
	filter_handle_drag(wp, c, rettv);
	أرجع;
    }

    // Invoke callback
    res.v_type = VAR_عدد;
    popup_close_and_callback(wp, &res);
}

/*
 * popup_dialog({text}, {options})
 */
    فراغ
f_popup_dialog(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_DIALOG);
}

/*
 * popup_menu({text}, {options})
 */
    فراغ
f_popup_menu(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_MENU);
}

/*
 * popup_notification({text}, {options})
 */
    فراغ
f_popup_notification(typval_T *argvars, typval_T *rettv)
{
    popup_create(argvars, rettv, TYPE_NOTIFICATION);
}

/*
 * Find the popup window with window-ID "id".
 * If the popup window does not exist NULL is returned.
 * If the window is not a popup window, and error message is given.
 */
    ساكن win_T *
پیدا_کن_popup_win(صحيح id)
{
    win_T *wp = win_id2wp(id);

    إذا (wp != عدم && !WIN_IS_POPUP(wp))
    {
	semsg(_(e_پنجره_nr_is_not_popup_پنجره), id);
	أرجع عدم;
    }
    أرجع wp;
}

/*
 * popup_close({id})
 */
    فراغ
f_popup_close(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    صحيح		id;
    win_T	*wp;

    إذا (درون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_عدد(argvars);
    إذا (
#اگر_تعریف_شده FEAT_TERMINAL
	// if the popup contains a terminal it will become hidden
	curbuf->b_جمله == عدم &&
#پایان_اگر
	    ERROR_IF_ANY_POPUP_پنجره)
	أرجع;

    wp = پیدا_کن_popup_win(id);
    إذا (wp != عدم)
	popup_close_and_callback(wp, &argvars[1]);
}

/*
 * Clear popup_mask entries for the cells covered by "wp" so that
 * screen_fill / screen_puts calls made before the next update_screen()
 * (e.g. msg_clr_eos triggered by a status message) are not silently
 * dropped by skip_for_popup().  Without this the popup's chars survive
 * on screen until may_update_popup_mask() runs and the affected cells
 * happen to be redrawn.
 */
    ساكن فراغ
popup_clear_ماسک_for(win_T *wp)
{
    صحيح r, c;
    صحيح ردیف_شروع, ستون_شروع, ردیف_انتها, ستون_انتها;

    إذا (popup_ماسک == عدم || !popup_visible)
	أرجع;

    ردیف_شروع = MAX(wp->w_winrow, 0);
    ستون_شروع = MAX(wp->w_wincol, 0);
    ردیف_انتها = MIN(wp->w_winrow + popup_ارتفاع(wp), (صحيح)screen_ردیفها);
    ستون_انتها = MIN(wp->w_wincol + popup_عرض(wp), (صحيح)screen_Columns);

    لكل (r = ردیف_شروع; r < ردیف_انتها; ++r)
	لكل (c = ستون_شروع; c < ستون_انتها; ++c)
	    popup_ماسک[r * screen_Columns + c] = 0;
}

    فراغ
popup_hide(win_T *wp)
{
    popup_area_T	old_area;
    صحيح			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

#اگر_تعریف_شده FEAT_TERMINAL
    إذا (error_if_جمله_popup_پنجره())
	أرجع;
#پایان_اگر
    إذا ((wp->w_popup_flags & POPF_HIDDEN) != 0)
	أرجع;

    popup_save_area(wp, &old_area);

#اگر_تعریف_شده FEAT_IMAGE_POPUP
    إذا (wp->w_popup_imagep != عدم)
	image_placement_clear(wp->w_popup_imagep);
#پایان_اگر

    wp->w_popup_flags |= POPF_HIDDEN;

    // Do not decrement b_nwindows, we still reference the buffer.
    إذا (wp->w_winrow + popup_ارتفاع(wp) >= cmdline_ردیف)
	clear_cmdline = TRUE;

    إذا (was_visible)
	popup_clear_ماسک_for(wp);

    إذا (old_area.فعال)
	popup_redraw_exposed_area(&old_area);
    وإلا
	redraw_all_later(UPD_NOT_VALID);

    وضعیت_redraw_all();
    إذا (!old_area.فعال)
	popup_ماسک_refresh = TRUE;
}

/*
 * popup_hide({id})
 */
    فراغ
f_popup_hide(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    صحيح		id;
    win_T	*wp;

    إذا (درون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_عدد(argvars);
    wp = پیدا_کن_popup_win(id);
    إذا (wp == عدم)
	أرجع;

    popup_hide(wp);
    wp->w_popup_flags |= POPF_HIDDEN_FORCE;
}

    فراغ
popup_show(win_T *wp)
{
    bool popup_فعال;

    إذا ((wp->w_popup_flags & POPF_HIDDEN) == 0)
	أرجع;

    popup_فعال = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    wp->w_popup_flags &= ~POPF_HIDDEN;
    إذا (popup_فعال)
    {
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_سطرها_valid = 0;
	إذا (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }
    وإلا
    {
	redraw_all_later(UPD_NOT_VALID);
	popup_ماسک_refresh = TRUE;
    }
}

/*
 * popup_show({id})
 */
    فراغ
f_popup_show(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    صحيح		id;
    win_T	*wp;

    rettv->v_type = VAR_عدد;
    rettv->vval.v_عدد = -1;

    إذا (درون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_عدد(argvars);
    wp = پیدا_کن_popup_win(id);
    إذا (wp == عدم)
	أرجع;

    wp->w_popup_flags &= ~POPF_HIDDEN_FORCE;
    popup_show(wp);
#اگر_تعریف_شده FEAT_QUICKFIX
    إذا (wp->w_popup_flags & POPF_INFO)
	pum_position_info_popup(wp);
#پایان_اگر

    rettv->vval.v_عدد = 0;
}

/*
 * popup_settext({id}, {text})
 */
    فراغ
f_popup_settext(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    صحيح		id;
    win_T	*wp;
    popup_area_T	old_area;

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_string_or_list_arg(argvars, 1) == FAIL))
	أرجع;

    id = (صحيح)tv_get_عدد(&argvars[0]);
    wp = پیدا_کن_popup_win(id);
    إذا (wp == عدم)
	أرجع;

    popup_save_area(wp, &old_area);

    إذا (check_for_string_or_list_arg(argvars, 1) == FAIL)
	أرجع;

    popup_مجموعه_میانگیر_متن(wp->w_میانگیر, argvars[1]);

    // Redraw the popup window without triggering a full screen redraw.
    // Using redraw_win_later() with UPD_NOT_VALID would set the global
    // must_redraw, causing may_update_popup_mask() to refresh the mask and
    // redraw windows behind the popup, resulting in flickering.
    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_سطرها_valid = 0;
    إذا (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
    popup_adjust_position(wp);

    إذا (popup_area_تغییریافته(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_setbuf({id}, {bufnr})
 */
    فراغ
f_popup_setbuf(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    صحيح			id;
    win_T		*wp;
    buf_T		*buf;
    popup_area_T	old_area;

    rettv->v_type = VAR_BOOL;
    rettv->vval.v_عدد = VVAL_FALSE;

    إذا (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_میانگیر_arg(argvars, 1) == FAIL)
	أرجع;

    id = (صحيح)tv_get_عدد(&argvars[0]);
    wp = پیدا_کن_popup_win(id);
    إذا (wp == عدم)
	أرجع;

    buf = tv_get_buf_from_arg(&argvars[1]);

    إذا (buf == عدم)
	أرجع;
#اگر_تعریف_شده FEAT_TERMINAL
    إذا (buf->b_جمله != عدم && popup_terminal_exists())
    {
	emsg(_(e_cannot_open_دومی_popup_with_terminal));
	أرجع;
    }
#پایان_اگر

    إذا (wp->w_میانگیر != buf)
    {
	popup_save_area(wp, &old_area);

	wp->w_میانگیر->b_nwindows--;
	win_init_popup_win(wp, buf);
	مجموعه_local_options_default(wp, FALSE);
	جابجا_exists_action = SEA_READONLY;
	میانگیر_ensure_loaded(buf);
	جابجا_exists_action = SEA_NONE;
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_adjust_position(wp);

	إذا (popup_area_تغییریافته(wp, &old_area))
	    popup_redraw_exposed_area(&old_area);
    }
    rettv->vval.v_عدد = VVAL_TRUE;
}

    ساكن فراغ
popup_free(win_T *wp)
{
    popup_area_T	old_area;
    صحيح			was_visible = (wp->w_popup_flags & POPF_HIDDEN) == 0;

    popup_save_area(wp, &old_area);

#اگر_تعریف_شده FEAT_IMAGE_POPUP
    إذا (wp->w_popup_imagep != عدم)
	image_placement_free(wp->w_popup_imagep);
#پایان_اگر

    sign_undefine_by_name(popup_get_sign_name(wp), FALSE);
    wp->w_میانگیر->b_locked = FALSE;
    إذا (wp->w_winrow + popup_ارتفاع(wp) >= cmdline_ردیف)
	clear_cmdline = TRUE;

    إذا (was_visible)
	popup_clear_ماسک_for(wp);

    popup_redraw_exposed_area(&old_area);

    win_free_popup(wp);

#اگر_تعریف_شده HAS_پیام_پنجره
    إذا (wp == پیام_win_2)
	پیام_win_2 = عدم;
#پایان_اگر

    إذا (!old_area.فعال)
	redraw_all_later(UPD_NOT_VALID);
    وضعیت_redraw_all();
    إذا (!old_area.فعال)
	popup_ماسک_refresh = TRUE;
}

    ساكن فراغ
error_for_popup_پنجره(فراغ)
{
    emsg(_(e_not_allowed_درون_popup_پنجره));
}

    صحيح
error_if_popup_پنجره(صحيح also_with_جمله استفادهنشده)
{
    // win_execute() may set "curwin" to a popup window temporarily, but many
    // commands are disallowed then.  When a terminal runs in the popup most
    // things are allowed.  When a terminal is finished it can be closed.
    إذا (WIN_IS_POPUP(curwin)
#اگر_تعریف_شده FEAT_TERMINAL
	    && (also_with_جمله || curbuf->b_جمله == عدم)
#پایان_اگر
	    )
    {
	error_for_popup_پنجره();
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Close a popup window by Window-id.
 * Does not invoke the callback.
 * Return OK if the popup was closed, FAIL otherwise.
 */
    صحيح
popup_close(صحيح id, صحيح force)
{
    win_T	*wp;
    tabpage_T	*tp;
    win_T	*prev = عدم;

    // go through global popups
    لكل (wp = اولی_popupwin; wp != عدم; prev = wp, wp = wp->w_بعدی)
	إذا (wp->w_id == id)
	{
	    إذا (wp == curwin)
	    {
		إذا (!force)
		{
		    error_for_popup_پنجره();
		    أرجع FAIL;
		}
		پشت_مقصد_prevwin(wp);
	    }
	    إذا (prev == عدم)
		اولی_popupwin = wp->w_بعدی;
	    وإلا
		prev->w_بعدی = wp->w_بعدی;
#اگر_تعریف_شده FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#پایان_اگر
	    popup_free(wp);
	    أرجع OK;
	}

    // go through tab-local popups
    FOR_ALL_TABPAGES(tp)
	إذا (popup_close_tabpage(tp, id, force) == OK)
	    أرجع OK;
    أرجع FAIL;
}

/*
 * Close a popup window with Window-id "id" in tabpage "tp".
 */
    صحيح
popup_close_tabpage(tabpage_T *tp, صحيح id, صحيح force)
{
    win_T	*wp;
    win_T	**ریشه = &tp->tp_اولی_popupwin;
    win_T	*prev = عدم;

    لكل (wp = *ریشه; wp != عدم; prev = wp, wp = wp->w_بعدی)
	إذا (wp->w_id == id)
	{
	    إذا (wp == curwin)
	    {
		إذا (!force)
		{
		    error_for_popup_پنجره();
		    أرجع FAIL;
		}
		پشت_مقصد_prevwin(wp);
	    }

	    // Set curwin for tabpage to a valid window, in case we try
	    // accessing it later.
	    إذا (tp->tp_curwin == wp)
		tp->tp_curwin = tp->tp_firstwin;

	    إذا (prev == عدم)
		*ریشه = wp->w_بعدی;
	    وإلا
		prev->w_بعدی = wp->w_بعدی;
#اگر_تعریف_شده FEAT_TERMINAL
	    // If the popup to be closed is opaque, terminal windows under
	    // the popup should trigger a force repaint of their windows.
	    redraw_win_under_opacity_popup(wp);
#پایان_اگر
	    popup_free(wp);
	    أرجع OK;
	}
    أرجع FAIL;
}

    فراغ
close_all_popups(صحيح force)
{
    إذا (!force && ERROR_IF_ANY_POPUP_پنجره)
	أرجع;
    طالما (اولی_popupwin != عدم)
	إذا (popup_close(اولی_popupwin->w_id, force) == FAIL)
	    أرجع;
    طالما (curtab->tp_اولی_popupwin != عدم)
	إذا (popup_close(curtab->tp_اولی_popupwin->w_id, force) == FAIL)
	    أرجع;
}

/*
 * Save the current popup area that may need to be restored later.
 */
    ساكن فراغ
popup_save_area(win_T *wp, popup_area_T *area)
{
    area->فعال = (wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0;
    area->winrow = wp->w_winrow;
    area->wincol = wp->w_wincol;
    area->ارتفاع = popup_ارتفاع(wp);
    area->عرض = popup_عرض(wp);
    area->leftoff = wp->w_popup_leftoff;
    area->zindex = wp->w_zindex;
}

/*
 * Save popup style-related fields that affect redraw/reposition decisions.
 */
    ساكن فراغ
popup_save_style(win_T *wp, popup_style_snapshot_T *style)
{
    صحيح i;

    style->firstline = wp->w_firstline;
    style->blend = wp->w_popup_blend;
    style->flags = wp->w_popup_flags;
    style->zindex = wp->w_zindex;
    style->title = wp->w_popup_title;
    style->scrollbar_highlight = wp->w_scrollbar_highlight;
    style->thumb_highlight = wp->w_thumb_highlight;
    لكل (i = 0; i < 4; i++)
	style->border_highlight[i] = wp->w_border_highlight[i];
}

/*
 * Return true if style changes require at least a popup redraw.
 */
    ساكن bool
popup_style_تغییریافته(win_T *wp, popup_style_snapshot_T *style)
{
    صحيح i;

    إذا (style->firstline != wp->w_firstline
	    || style->flags != wp->w_popup_flags
	    || style->title != wp->w_popup_title
	    || style->scrollbar_highlight != wp->w_scrollbar_highlight
	    || style->thumb_highlight != wp->w_thumb_highlight)
	أرجع true;
    لكل (i = 0; i < 4; i++)
	إذا (style->border_highlight[i] != wp->w_border_highlight[i])
	    أرجع true;
    أرجع false;
}

/*
 * Save popup layout fields that affect mask refresh and local redraw.
 */
    ساكن فراغ
popup_save_layout(win_T *wp, popup_layout_T *layout)
{
    layout->winrow = wp->w_winrow;
    layout->wincol = wp->w_wincol;
    layout->عرض = wp->w_عرض;
    layout->ارتفاع = wp->w_ارتفاع;
    layout->leftcol = wp->w_leftcol;
    layout->leftoff = wp->w_popup_leftoff;
    layout->has_scrollbar = wp->w_has_scrollbar;
    layout->topoff = wp->w_popup_topoff;
    layout->bottomoff = wp->w_popup_bottomoff;
    layout->leftclip = wp->w_popup_leftclip;
    layout->rightclip = wp->w_popup_rightclip;
}

/*
 * Return true when the popup layout changed.
 */
    ساكن bool
popup_layout_تغییریافته(win_T *wp, popup_layout_T *layout)
{
    أرجع layout->winrow != wp->w_winrow
	|| layout->wincol != wp->w_wincol
	|| layout->leftcol != wp->w_leftcol
	|| layout->leftoff != wp->w_popup_leftoff
	|| layout->عرض != wp->w_عرض
	|| layout->ارتفاع != wp->w_ارتفاع
	|| layout->has_scrollbar != wp->w_has_scrollbar
	|| layout->topoff != wp->w_popup_topoff
	|| layout->bottomoff != wp->w_popup_bottomoff
	|| layout->leftclip != wp->w_popup_leftclip
	|| layout->rightclip != wp->w_popup_rightclip;
}

/*
 * Return true when the popup no longer covers the saved area.
 */
    ساكن bool
popup_area_تغییریافته(win_T *wp, popup_area_T *area)
{
    أرجع area->winrow != wp->w_winrow
	|| area->wincol != wp->w_wincol
	|| area->ارتفاع != popup_ارتفاع(wp)
	|| area->عرض != popup_عرض(wp)
	|| area->leftoff != wp->w_popup_leftoff;
}

/*
 * If "wp" is a visible opacity popup at or below "zindex" whose drawn area
 * overlaps the rectangle, mark it for full redraw so its blended background
 * is recomputed.
 */
    ساكن فراغ
mark_overlapped_opacity_popup(win_T *wp, صحيح area_بالاترین, صحيح area_bot,
	صحيح area_چپ, صحيح area_راست, صحيح zindex)
{
    إذا ((wp->w_popup_flags & POPF_HIDDEN)
	    || (wp->w_popup_flags & POPF_OPACITY) == 0
	    || wp->w_popup_blend == 0
	    || wp->w_zindex > zindex
	    || wp->w_winrow >= area_bot
	    || wp->w_winrow + popup_ارتفاع(wp) <= area_بالاترین
	    || wp->w_wincol >= area_راست
	    || wp->w_wincol + popup_عرض(wp) - wp->w_popup_leftoff
							      <= area_چپ)
	أرجع;

    wp->w_redr_type = UPD_NOT_VALID;
    wp->w_سطرها_valid = 0;
}

/*
 * Mark lower or equal zindex opacity popups that overlap with a popup area
 * for redraw.  Their blended background may have included the old popup.
 */
    ساكن فراغ
redraw_overlapped_opacity_popups(صحيح winrow, صحيح wincol, صحيح ارتفاع, صحيح عرض,
	صحيح leftoff, صحيح zindex)
{
    win_T	*wp;
    صحيح		area_بالاترین = winrow;
    صحيح		area_bot = winrow + ارتفاع;
    صحيح		area_چپ = wincol;
    صحيح		area_راست = wincol + عرض - leftoff;

    FOR_ALL_POPUPWINS(wp)
	mark_overlapped_opacity_popup(wp, area_بالاترین, area_bot, area_چپ,
		area_راست, zindex);
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	mark_overlapped_opacity_popup(wp, area_بالاترین, area_bot, area_چپ,
		area_راست, zindex);

    إذا (must_redraw < UPD_VALID)
	must_redraw = UPD_VALID;
}

/*
 * Replace pum_bg_* cells [left, right) of screen row "r" with the underlying
 * buffer text, so the pum's opacity padding shows the buffer through.  Cells
 * outside any window (or past the end of the buffer line) become spaces.
 *
 * Used after an opacity popup that overlapped pum_bg_* is dismissed: the
 * stale popup content held in pum_bg_* would otherwise leak through the
 * pum's opacity blend as a ghost.  This walks the buffer line and writes
 * the displayed character at each visual column directly into pum_bg_*.
 *
 * Limitations: handles plain text and tabs; folds, conceal, virtual text and
 * other rendering features fall back to a space, which still beats showing a
 * stale popup char.
 */
    ساكن فراغ
refill_pum_bg_ردیف_from_میانگیر(صحيح r, صحيح چپ_2, صحيح راست_2)
{
    صحيح		سطر_cp = r;
    صحيح		ستون_cp = چپ_2;
    win_T	*wp;
    linenr_T	lnum;
    صحيح		ردیف_for_lnum;
    صحيح		ستون_for_lnum = 0;
    char_u	*سطر;
    char_u	*p;
    صحيح		win_متن_ستون;
    صحيح		screen_ستون;
    صحيح		soff_پایه = (r - pum_bg_بالاترین) * pum_bg_تعداد_ستون;
    صحيح		c;

    // Default: fill the range with spaces so trailing/empty cells render as
    // plain pum bg through opacity.
    لكل (c = چپ_2; c < راست_2; ++c)
    {
	pum_bg_سطرها[soff_پایه + c] = ' ';
	إذا (pum_bg_attrs != عدم)
	    pum_bg_attrs[soff_پایه + c] = 0;
	إذا (enc_utf8 && pum_bg_سطرها_UC != عدم)
	    pum_bg_سطرها_UC[soff_پایه + c] = 0;
	إذا (enc_utf8)
	{
	    صحيح k;
	    لكل (k = 0; k < MAX_MCO; ++k)
		إذا (pum_bg_سطرها_C[k] != عدم)
		    pum_bg_سطرها_C[k][soff_پایه + c] = 0;
	}
    }

    wp = mouse_پیدا_کن_win(&سطر_cp, &ستون_cp, IGNORE_POPUP);
    إذا (wp == عدم || سطر_cp < 0 || سطر_cp >= wp->w_ارتفاع)
	أرجع;
    إذا (wp->w_میانگیر == عدم || wp->w_میانگیر->b_ml.ml_mfp == عدم)
	أرجع;

    // Compute the buffer line for this screen row.
    ردیف_for_lnum = سطر_cp;
    إذا (mouse_comp_pos(wp, &ردیف_for_lnum, &ستون_for_lnum, &lnum, عدم))
	أرجع;	// past end of buffer
    إذا (lnum < 1 || lnum > wp->w_میانگیر->b_ml.ml_سطر_شمار)
	أرجع;

    سطر = ml_get_buf(wp->w_میانگیر, lnum, FALSE);
    إذا (سطر == عدم)
	أرجع;

    // Walk the buffer line and write each displayed cell into pum_bg_*.
    // win_text_col is the screen column where the buffer text starts inside
    // the window (after sign/number/fold columns and horizontal scroll).
    win_متن_ستون = wp->w_wincol + win_ستون_off(wp);
    إذا (!wp->w_p_wrap)
	win_متن_ستون -= wp->w_leftcol;
    screen_ستون = win_متن_ستون;
    p = سطر;
    طالما (*p != NUL && screen_ستون < راست_2)
    {
	صحيح char_cells;
	صحيح byte_شمار;
	صحيح soff = soff_پایه + screen_ستون;
	صحيح درون_range = (screen_ستون >= چپ_2 && screen_ستون < راست_2);

	إذا (*p == '\t')
	{
	    صحيح ts = (صحيح)wp->w_میانگیر->b_p_ts;
	    char_cells = ts > 0 ? ts - ((screen_ستون - win_متن_ستون) % ts) : 1;
	    byte_شمار = 1;
	    إذا (درون_range)
	    {
		pum_bg_سطرها[soff] = ' ';
		إذا (enc_utf8 && pum_bg_سطرها_UC != عدم)
		    pum_bg_سطرها_UC[soff] = 0;
	    }
	}
	وإلا إذا (has_mbyte)
	{
	    char_cells = mb_ptr2cells(p);
	    byte_شمار = mb_ptr2len(p);
	    إذا (درون_range)
	    {
		pum_bg_سطرها[soff] = *p;
		إذا (enc_utf8 && pum_bg_سطرها_UC != عدم)
		    pum_bg_سطرها_UC[soff] = (*p < 0x80) ? 0 : mb_ptr2char(p);
	    }
	}
	وإلا
	{
	    char_cells = 1;
	    byte_شمار = 1;
	    إذا (درون_range)
	    {
		pum_bg_سطرها[soff] = (*p < 0x20) ? ' ' : *p;
		إذا (enc_utf8 && pum_bg_سطرها_UC != عدم)
		    pum_bg_سطرها_UC[soff] = 0;
	    }
	}
	إذا (درون_range && pum_bg_attrs != عدم)
	    pum_bg_attrs[soff] = 0;

	// For wide chars / tabs the trailing cells are zeroed already (by the
	// initial space fill we did above).  Just skip past them.
	p += byte_شمار;
	screen_ستون += char_cells;
    }
}

/*
 * Redraw what becomes exposed when an opacity popup moves, resizes or closes.
 */
    ساكن فراغ
popup_redraw_exposed_area(popup_area_T *area)
{
    إذا (!area->فعال)
	أرجع;

    redraw_under_popup_area(area->winrow, area->wincol, area->ارتفاع,
	    area->عرض, area->leftoff);
    redraw_overlapped_opacity_popups(area->winrow, area->wincol,
	    area->ارتفاع, area->عرض, area->leftoff, area->zindex);

    // If the closing/moving popup overlapped the pum's saved background,
    // pum_bg_* still holds the dismissed popup's content.  When the pum
    // next blends opacity it would restore those stale chars at padding
    // cells, leaving a ghost.
    //
    // We can't re-snapshot via update_screen from here: the surrounding
    // update_screen has updating_screen set, so a nested call would no-op.
    // Instead, replace the overlapping pum_bg_* cells with the actual
    // underlying buffer text so the pum's opacity padding shows the buffer
    // through, just as it would if the popup had never been there.
    إذا (pum_bg_سطرها != عدم
	    && area->winrow < pum_bg_bot
	    && area->winrow + area->ارتفاع > pum_bg_بالاترین)
    {
	صحيح بالاترین = MAX(area->winrow, pum_bg_بالاترین);
	صحيح bot = MIN(area->winrow + area->ارتفاع, pum_bg_bot);
	صحيح چپ_2 = MAX(area->wincol, 0);
	صحيح راست_2 = MIN(area->wincol + area->عرض, pum_bg_تعداد_ستون);
	صحيح r;

	لكل (r = بالاترین; r < bot; ++r)
	    refill_pum_bg_ردیف_from_میانگیر(r, چپ_2, راست_2);
    }
}

/*
 * Release saved screen data used for opacity padding redraw.
 */
    ساكن فراغ
popup_free_saved_screen(popup_saved_screen_T *saved_screen)
{
    vim_free(saved_screen->سطرها);
    vim_free(saved_screen->attrs);
    vim_free(saved_screen->linesuc);
    CLEAR_اشاره_گر(saved_screen);
}

/*
 * Save the screen area that opacity padding may need to blend against.
 * On entry "saved_screen" must be zero-initialised.  When the snapshot is
 * unavailable (no opacity popup, no padding, allocation failure) "lines" is
 * left NULL and callers should fall back to plain screen_fill().  The caller
 * must always release the snapshot with popup_free_saved_screen().
 */
    ساكن فراغ
popup_save_padding_screen(win_T *wp, popup_saved_screen_T *saved_screen)
{
    إذا (screen_opacity_popup == عدم
	    || (wp->w_popup_padding[0] == 0 && wp->w_popup_padding[1] == 0
		&& wp->w_popup_padding[2] == 0 && wp->w_popup_padding[3] == 0))
	أرجع;

    saved_screen->شروع_ردیف = wp->w_winrow + wp->w_popup_border[0];
    saved_screen->شروع_ستون = wp->w_wincol + wp->w_popup_border[3];
    saved_screen->ردیفها_2 = wp->w_popup_padding[0] + wp->w_ارتفاع
						    + wp->w_popup_padding[2];
    saved_screen->تعداد_ستون = wp->w_popup_padding[3] + wp->w_عرض
						    + wp->w_popup_padding[1];

    // Include one column to the left to handle wide chars that overlap the
    // padding boundary.
    إذا (saved_screen->شروع_ستون > 0)
    {
	--saved_screen->شروع_ستون;
	++saved_screen->تعداد_ستون;
    }

    saved_screen->سطرها = ALLOC_CLEAR_MULT(schar_T,
				  saved_screen->ردیفها_2 * saved_screen->تعداد_ستون);
    saved_screen->attrs = ALLOC_CLEAR_MULT(صحيح,
				  saved_screen->ردیفها_2 * saved_screen->تعداد_ستون);
    إذا (enc_utf8)
	saved_screen->linesuc = ALLOC_CLEAR_MULT(u8char_T,
				  saved_screen->ردیفها_2 * saved_screen->تعداد_ستون);

    إذا (saved_screen->سطرها == عدم || saved_screen->attrs == عدم)
	أرجع;

    لكل (صحيح r = 0; r < saved_screen->ردیفها_2; r++)
    {
	صحيح screen_ردیف = saved_screen->شروع_ردیف + r;

	إذا (screen_ردیف >= 0 && screen_ردیف < screen_ردیفها)
	    لكل (صحيح c = 0; c < saved_screen->تعداد_ستون; c++)
	    {
		صحيح screen_ستون = saved_screen->شروع_ستون + c;

		إذا (screen_ستون >= 0 && screen_ستون < screen_Columns)
		{
		    صحيح off = سطر_جابه_جایی[screen_ردیف] + screen_ستون;
		    صحيح save_off = r * saved_screen->تعداد_ستون + c;

		    saved_screen->سطرها[save_off] = Screen_سطرها[off];
		    saved_screen->attrs[save_off] = ScreenAttrs[off];
		    إذا (enc_utf8 && saved_screen->linesuc != عدم)
			saved_screen->linesuc[save_off] = Screen_سطرها_UC[off];
		}
	    }
    }
}

/*
 * Force windows under a popup area to redraw.
 */
    ساكن فراغ
redraw_under_popup_area(صحيح winrow, صحيح wincol, صحيح ارتفاع, صحيح عرض, صحيح leftoff)
{
    صحيح	    r;

    لكل (r = winrow; r < winrow + ارتفاع && r < screen_ردیفها; ++r)
    {
	صحيح	    c;
	win_T	    *prev_twp = عدم;

	إذا (r >= cmdline_ردیف)
	{
	    clear_cmdline = TRUE;
	    استمر;
	}

	لكل (c = wincol; c < wincol + عرض - leftoff && c < screen_Columns; ++c)
	{
	    صحيح	    سطر_cp = r;
	    صحيح	    ستون_cp = c;
	    win_T   *twp;

	    twp = mouse_پیدا_کن_win(&سطر_cp, &ستون_cp, IGNORE_POPUP);
	    إذا (twp != عدم && twp != prev_twp)
	    {
		prev_twp = twp;
		إذا (سطر_cp < twp->w_ارتفاع)
		{
		    linenr_T lnum;

		    (فراغ)mouse_comp_pos(twp, &سطر_cp, &ستون_cp, &lnum, عدم);
		    redrawWinline(twp, lnum);
		}
		وإلا إذا (سطر_cp == twp->w_ارتفاع)
		    twp->w_redr_وضعیت = true;
	    }
	}
    }
}

/*
 * popup_move({id}, {options})
 */
    فراغ
f_popup_move(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    dict_T	*dict;
    صحيح		id;
    win_T	*wp;
    popup_area_T	old_area;

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    id = (صحيح)tv_get_عدد(argvars);
    wp = پیدا_کن_popup_win(id);
    إذا (wp == عدم)
	أرجع;  // invalid {id}

    إذا (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	أرجع;
    dict = argvars[1].vval.v_dict;

    popup_save_area(wp, &old_area);

    apply_move_options(wp, dict);

    إذا (wp->w_winrow + wp->w_ارتفاع >= cmdline_ردیف)
	clear_cmdline = TRUE;
    popup_adjust_position(wp);

    // Redraw the popup at the new position; for opaque popups, the
    // diff-based popup mask update in may_update_popup_mask() will handle
    // redrawing the affected lines in regular windows to clear the old
    // position.  Transparent popups don't participate in popup_mask, so
    // we need to manually mark the old area's lines for redraw.
    إذا (popup_area_تغییریافته(wp, &old_area))
    {
	redraw_win_later(wp, UPD_NOT_VALID);

	popup_redraw_exposed_area(&old_area);
    }
}

/*
 * popup_setoptions({id}, {options})
 */
    فراغ
f_popup_setoptions(typval_T *argvars, typval_T *rettv استفادهنشده)
{
    dict_T	*dict;
    صحيح		id;
    win_T	*wp;
    popup_area_T	old_area;
    popup_style_snapshot_T old_style;
    صحيح		need_redraw = FALSE;
    صحيح		need_reposition = FALSE;

    إذا (check_secure())
	أرجع;

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    id = (صحيح)tv_get_عدد(argvars);
    wp = پیدا_کن_popup_win(id);
    إذا (wp == عدم)
	أرجع;  // invalid {id}

    إذا (check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	أرجع;
    dict = argvars[1].vval.v_dict;
    popup_save_area(wp, &old_area);
    popup_save_style(wp, &old_style);

    (فراغ)apply_options(wp, dict, FALSE);

    // Keep "firstline" sticky across popup_setoptions(): when it is set, any
    // property update should reapply it and restore the displayed top line.
    إذا (wp->w_firstline > 0
	    && wp->w_firstline <= wp->w_میانگیر->b_ml.ml_سطر_شمار)
	wp->w_topline = wp->w_firstline;

    // Check if visual options changed and redraw if needed
    إذا (old_style.zindex != wp->w_zindex)
    {
	need_redraw = TRUE;
	need_reposition = TRUE;
    }
    وإلا إذا (popup_style_تغییریافته(wp, &old_style))
	need_redraw = TRUE;

    إذا (old_style.flags != wp->w_popup_flags)
	need_reposition = TRUE;

    إذا (need_reposition)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	popup_ماسک_refresh = TRUE;
    }
    وإلا إذا (need_redraw)
    {
	// Only content changed (e.g. firstline, highlight): redraw the
	// popup window without updating the popup mask or triggering a
	// full screen redraw.  This avoids flickering of windows behind
	// the popup.
	wp->w_redr_type = UPD_NOT_VALID;
	wp->w_سطرها_valid = 0;
	إذا (must_redraw < UPD_VALID)
	    must_redraw = UPD_VALID;
    }

    // Force redraw if opacity value changed
    إذا (old_style.blend != wp->w_popup_blend)
    {
	redraw_win_later(wp, UPD_NOT_VALID);
	// Also redraw windows below the popup
	redraw_all_later(UPD_NOT_VALID);
	popup_ماسک_refresh = TRUE;
    }

    // Always recalculate popup position/size: other options like border,
    // close, padding may have changed without affecting w_popup_flags.
    // popup_adjust_position() only sets popup_mask_refresh when the
    // position or size actually changed.
    popup_adjust_position(wp);

    إذا (popup_area_تغییریافته(wp, &old_area))
	popup_redraw_exposed_area(&old_area);
}

/*
 * popup_getpos({id})
 */
    فراغ
f_popup_getpos(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    صحيح		id;
    win_T	*wp;
    صحيح		بالاترین_extra;
    صحيح		چپ_extra;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

    إذا (درون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_عدد(argvars);
    wp = پیدا_کن_popup_win(id);
    إذا (wp == عدم)
	أرجع;  // invalid {id}
    بالاترین_extra = popup_بالاترین_extra(wp);
    چپ_extra = wp->w_popup_border[3] + wp->w_popup_padding[3];

    // we know how much space we need, avoid resizing halfway
    dict = rettv->vval.v_dict;
    hash_قفل_اندازه(&dict->dv_hashtab, 11);

    dict_جمع_کن_عدد(dict, "line", wp->w_winrow + 1);
    dict_جمع_کن_عدد(dict, "col", wp->w_wincol + 1);
    dict_جمع_کن_عدد(dict, "width", wp->w_عرض + چپ_extra
	    + wp->w_popup_border[1] + wp->w_popup_padding[1]);
    dict_جمع_کن_عدد(dict, "height", wp->w_ارتفاع + بالاترین_extra
	    + wp->w_popup_border[2] + wp->w_popup_padding[2]);

    dict_جمع_کن_عدد(dict, "core_line", wp->w_winrow + 1 + بالاترین_extra);
    dict_جمع_کن_عدد(dict, "core_col", wp->w_wincol + 1 + چپ_extra);
    dict_جمع_کن_عدد(dict, "core_width", wp->w_عرض);
    dict_جمع_کن_عدد(dict, "core_height", wp->w_ارتفاع);

    dict_جمع_کن_عدد(dict, "scrollbar", wp->w_has_scrollbar);
    dict_جمع_کن_عدد(dict, "firstline", wp->w_topline);
    dict_جمع_کن_عدد(dict, "lastline", wp->w_botline - 1);
    dict_جمع_کن_عدد(dict, "visible",
	    win_valid(wp) && (wp->w_popup_flags & POPF_HIDDEN) == 0);

    hash_unlock(&dict->dv_hashtab);
}

/*
 * popup_list()
 */
    فراغ
f_popup_list(typval_T *argvars استفادهنشده, typval_T *rettv)
{
    win_T	*wp;
    tabpage_T	*tp;

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;
    FOR_ALL_POPUPWINS(wp)
	list_append_عدد(rettv->vval.v_list, wp->w_id);
    FOR_ALL_TABPAGES(tp)
	FOR_ALL_POPUPWINS_درون_TAB(tp, wp)
	    list_append_عدد(rettv->vval.v_list, wp->w_id);
}

/*
 * popup_locate({row}, {col})
 */
    فراغ
f_popup_locate(typval_T *argvars, typval_T *rettv)
{
    صحيح		ردیف;
    صحيح		ستون;
    win_T	*wp;

    إذا (درون_vim9script()
	    && (check_for_عدد_arg(argvars, 0) == FAIL
		|| check_for_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    ردیف = tv_get_عدد(&argvars[0]) - 1;
    ستون = tv_get_عدد(&argvars[1]) - 1;
    wp = mouse_پیدا_کن_win(&ردیف, &ستون, پیدا_کن_POPUP);
    إذا (wp != عدم && WIN_IS_POPUP(wp))
	rettv->vval.v_عدد = wp->w_id;
}

/*
 * For popup_getoptions(): add a "border" or "padding" entry to "dict".
 */
    ساكن فراغ
get_padding_border(dict_T *dict, صحيح *array, حرف *name)
{
    list_T  *list;
    صحيح	    i;

    إذا (array[0] == 0 && array[1] == 0 && array[2] == 0 && array[3] == 0)
	أرجع;

    list = list_alloc();
    إذا (list == عدم)
	أرجع;

    إذا (dict_جمع_کن_list(dict, name, list) == FAIL)
    {
	list_unref(list);
	أرجع;
    }
    إذا (array[0] != 1 || array[1] != 1 || array[2] != 1 || array[3] != 1)
	لكل (i = 0; i < 4; ++i)
	    list_append_عدد(list, array[i]);
}

/*
 * For popup_getoptions(): add a "borderhighlight" entry to "dict".
 */
    ساكن فراغ
get_borderhighlight(dict_T *dict, win_T *wp)
{
    list_T  *list;
    صحيح	    i;

    لكل (i = 0; i < 4; ++i)
	إذا (wp->w_border_highlight[i] != عدم)
	    اكسر;
    // Only include "borderhighlight" if it was explicitly set (even if empty)
    // or if at least one highlight is set.
    إذا (i == 4 && !wp->w_border_highlight_isset)
	أرجع;

    list = list_alloc();
    إذا (list == عدم)
	أرجع;

    إذا (dict_جمع_کن_list(dict, "borderhighlight", list) == FAIL)
    {
	list_unref(list);
	أرجع;
    }
    // When all highlights are NULL (cleared to empty list), return empty list.
    إذا (i == 4)
	أرجع;
    لكل (i = 0; i < 4; ++i)
	list_append_string(list, wp->w_border_highlight[i], -1);
}

/*
 * For popup_getoptions(): add a "borderchars" entry to "dict".
 */
    ساكن فراغ
get_borderchars(dict_T *dict, win_T *wp)
{
    list_T  *list;
    صحيح	    i;
    char_u  buf[NUMBUFLEN];
    صحيح	    len;

    لكل (i = 0; i < 8; ++i)
	إذا (wp->w_border_char[i] != 0)
	    اكسر;
    إذا (i == 8)
	أرجع;

    list = list_alloc();
    إذا (list == عدم)
	أرجع;

    إذا (dict_جمع_کن_list(dict, "borderchars", list) == FAIL)
    {
	list_unref(list);
	أرجع;
    }
    لكل (i = 0; i < 8; ++i)
    {
	len = mb_char2bytes(wp->w_border_char[i], buf);
	list_append_string(list, buf, len);
    }
}

/*
 * For popup_getoptions(): add a "moved" and "mousemoved" entry to "dict".
 */
    ساكن فراغ
get_moved_list(dict_T *dict, win_T *wp)
{
    list_T  *list;

    list = list_alloc();
    إذا (list != عدم)
    {
	إذا (dict_جمع_کن_list(dict, "moved", list) == FAIL)
	    list_unref(list);
	وإلا
	{
	    list_append_عدد(list, wp->w_popup_lnum);
	    list_append_عدد(list, wp->w_popup_mincol);
	    list_append_عدد(list, wp->w_popup_maxcol);
	}
    }
    list = list_alloc();
    إذا (list == عدم)
	أرجع;

    إذا (dict_جمع_کن_list(dict, "mousemoved", list) == FAIL)
    {
	list_unref(list);
	أرجع;
    }
    list_append_عدد(list, wp->w_popup_mouse_ردیف);
    list_append_عدد(list, wp->w_popup_mouse_mincol);
    list_append_عدد(list, wp->w_popup_mouse_maxcol);
}

/*
 * popup_getoptions({id})
 */
    فراغ
f_popup_getoptions(typval_T *argvars, typval_T *rettv)
{
    dict_T	*dict;
    صحيح		id;
    win_T	*wp;
    tabpage_T	*tp;
    صحيح		i;

    إذا (rettv_dict_alloc(rettv) == FAIL)
	أرجع;

    إذا (درون_vim9script() && check_for_عدد_arg(argvars, 0) == FAIL)
	أرجع;

    id = (صحيح)tv_get_عدد(argvars);
    wp = پیدا_کن_popup_win(id);
    إذا (wp == عدم)
	أرجع;

    dict = rettv->vval.v_dict;
    dict_جمع_کن_عدد(dict, "line", wp->w_wantline);
    dict_جمع_کن_عدد(dict, "col", wp->w_wantcol);
    dict_جمع_کن_عدد(dict, "minwidth", wp->w_minwidth);
    dict_جمع_کن_عدد(dict, "minheight", wp->w_minheight);
    dict_جمع_کن_عدد(dict, "maxheight", wp->w_maxheight);
    dict_جمع_کن_عدد(dict, "maxwidth", wp->w_maxwidth);
    dict_جمع_کن_عدد(dict, "firstline", wp->w_firstline);
    dict_جمع_کن_عدد(dict, "scrollbar", wp->w_want_scrollbar);
    dict_جمع_کن_عدد(dict, "zindex", wp->w_zindex);
    dict_جمع_کن_عدد(dict, "fixed", wp->w_popup_fixed);
    إذا (wp->w_popup_prop_type && win_valid_any_tab(wp->w_popup_prop_win))
    {
	proptype_T *pt = متن_prop_type_by_id(
		wp->w_popup_prop_win->w_میانگیر,
		wp->w_popup_prop_type);

	إذا (pt != عدم)
	    dict_جمع_کن_string(dict, "textprop", pt->pt_name);
	dict_جمع_کن_عدد(dict, "textpropwin", wp->w_popup_prop_win->w_id);
	dict_جمع_کن_عدد(dict, "textpropid", wp->w_popup_prop_id);
    }

#اگر_تعریف_شده FEAT_IMAGE_POPUP
    إذا (wp->w_popup_imagep != عدم && wp->w_popup_imagep->img != عدم)
    {
	dict_T *img_dict = get_image_info(wp->w_popup_imagep->img);

	إذا (dict_جمع_کن_dict(dict, "image", img_dict) == FAIL)
	    dict_unref(img_dict);
    }
#پایان_اگر

    dict_جمع_کن_string(dict, "title", wp->w_popup_title);
    dict_جمع_کن_عدد(dict, "wrap", wp->w_p_wrap);
    dict_جمع_کن_عدد(dict, "drag", (wp->w_popup_flags & POPF_DRAG) != 0);
    dict_جمع_کن_عدد(dict, "dragall",
	    (wp->w_popup_flags & POPF_DRAGALL) != 0);
    dict_جمع_کن_عدد(dict, "mapping",
	    (wp->w_popup_flags & POPF_MAPPING) != 0);
    dict_جمع_کن_عدد(dict, "resize", (wp->w_popup_flags & POPF_RESIZE) != 0);
    dict_جمع_کن_عدد(dict, "posinvert",
	    (wp->w_popup_flags & POPF_POSINVERT) != 0);
    dict_جمع_کن_عدد(dict, "clipwindow",
	    (wp->w_popup_flags & POPF_CLIPWINDOW) != 0);
    // Return opacity (0-100) by converting from internal blend value
    dict_جمع_کن_عدد(dict, "opacity",
	    (wp->w_popup_flags & POPF_OPACITY) ? 100 - wp->w_popup_blend : 100);
    dict_جمع_کن_عدد(dict, "cursorline",
	    (wp->w_popup_flags & POPF_CURSORLINE) != 0);
    dict_جمع_کن_string(dict, "highlight", syn_id2name(hlf_get_id(wp, HLF_WIN)));
    dict_جمع_کن_string(dict, "highlights", wp->w_p_whl);
    إذا (wp->w_scrollbar_highlight != عدم)
	dict_جمع_کن_string(dict, "scrollbarhighlight",
		wp->w_scrollbar_highlight);
    إذا (wp->w_thumb_highlight != عدم)
	dict_جمع_کن_string(dict, "thumbhighlight", wp->w_thumb_highlight);

    // find the tabpage that holds this popup
    i = 1;
    FOR_ALL_TABPAGES(tp)
    {
	win_T *twp;

	FOR_ALL_POPUPWINS_درون_TAB(tp, twp)
	    إذا (twp->w_id == id)
		اكسر;
	إذا (twp != عدم)
	    اكسر;
	++i;
    }
    إذا (tp == عدم)
	i = -1;  // must be global
    وإلا إذا (tp == curtab)
	i = 0;
    dict_جمع_کن_عدد(dict, "tabpage", i);

    get_padding_border(dict, wp->w_popup_padding, "padding");
    get_padding_border(dict, wp->w_popup_border, "border");
    get_borderhighlight(dict, wp);
    get_borderchars(dict, wp);
    get_moved_list(dict, wp);

    إذا (wp->w_filter_cb.cb_name != عدم)
	dict_جمع_کن_callback(dict, "filter", &wp->w_filter_cb);
    إذا (wp->w_close_cb.cb_name != عدم)
	dict_جمع_کن_callback(dict, "callback", &wp->w_close_cb);

    لكل (i = 0; i < (صحيح)ARRAY_درازی(poppos_entries); ++i)
	إذا (wp->w_popup_pos == poppos_entries[i].pp_val)
	{
	    dict_جمع_کن_string_len(dict, "pos",
		poppos_entries[i].pp_name.string,
		(صحيح)poppos_entries[i].pp_name.درازی);
	    اكسر;
	}

    إذا (wp->w_popup_close == POPCLOSE_BUTTON)
	dict_جمع_کن_string_len(dict, "close", (char_u *)"button", STRLEN_LITERAL("button"));
    وإلا إذا (wp->w_popup_close == POPCLOSE_CLICK)
	dict_جمع_کن_string_len(dict, "close", (char_u *)"click", STRLEN_LITERAL("click"));
    وإلا
	dict_جمع_کن_string_len(dict, "close", (char_u *)"none", STRLEN_LITERAL("none"));

#إذا defined(FEAT_TIMERS)
    dict_جمع_کن_عدد(dict, "time", wp->w_popup_timer != عدم
	    ?  (دراز)wp->w_popup_timer->tr_interval : 0L);
#پایان_اگر
}

#إذا defined(FEAT_TERMINAL)
/*
 * Return TRUE if the current window is running a terminal in a popup window.
 * Return FALSE when the job has ended.
 */
    صحيح
error_if_جمله_popup_پنجره(فراغ)
{
    إذا (WIN_IS_POPUP(curwin) && curbuf->b_جمله != عدم
					   && جمله_کار_running(curbuf->b_جمله))
    {
	emsg(_(e_not_allowed_for_terminal_درون_popup_پنجره));
	أرجع TRUE;
    }
    أرجع FALSE;
}
#پایان_اگر

/*
 * Reset all the "handled_flag" flags in global popup windows and popup windows
 * in the current tab page.
 * Each calling function should use a different flag, see the list at
 * POPUP_HANDLED_1.  This won't work with recursive calls though.
 */
    فراغ
popup_reset_handled(صحيح handled_flag)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	wp->w_popup_handled &= ~handled_flag;
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	wp->w_popup_handled &= ~handled_flag;
}

/*
 * Find the next visible popup where "handled_flag" is not set.
 * Must have called popup_reset_handled() first.
 * When "lowest" is TRUE find the popup with the lowest zindex, otherwise the
 * popup with the highest zindex.
 */
    win_T *
پیدا_کن_بعدی_popup(صحيح پایینترین, صحيح handled_flag)
{
    win_T   *wp;
    win_T   *یافت_wp;
    صحيح	    یافت_zindex;

    یافت_zindex = پایینترین ? بیشترین_صحیح : 0;
    یافت_wp = عدم;
    FOR_ALL_POPUPWINS(wp)
	إذا ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (پایینترین ? wp->w_zindex < یافت_zindex
		    : wp->w_zindex > یافت_zindex))
	{
	    یافت_zindex = wp->w_zindex;
	    یافت_wp = wp;
	}
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	إذا ((wp->w_popup_handled & handled_flag) == 0
		&& (wp->w_popup_flags & POPF_HIDDEN) == 0
		&& (پایینترین ? wp->w_zindex < یافت_zindex
		    : wp->w_zindex > یافت_zindex))
	{
	    یافت_zindex = wp->w_zindex;
	    یافت_wp = wp;
	}

    إذا (یافت_wp != عدم)
	یافت_wp->w_popup_handled |= handled_flag;
    أرجع یافت_wp;
}

/*
 * Invoke the filter callback for window "wp" with typed character "c".
 * Uses the global "mod_mask" for modifiers.
 * Returns the return value of the filter or -1 for CTRL-C in the current
 * window.
 * Careful: The filter may make "wp" invalid!
 */
    ساكن صحيح
invoke_popup_filter(win_T *wp, صحيح c)
{
    صحيح		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	buf[NUMBUFLEN];
    linenr_T	old_lnum = wp->w_cursor.lnum;
    صحيح		prev_did_emsg = did_emsg;

    // Emergency exit: CTRL-C closes the popup.
    إذا (c == Ctrl_C)
    {
	صحيح save_got_int = got_int;
	صحيح was_curwin = wp == curwin;

	// Reset got_int to avoid the callback isn't called.
	got_int = FALSE;
	popup_close_with_retval(wp, -1);
	got_int |= save_got_int;

	// If the popup is the current window it probably fails to close.  Then
	// do not consume the key.
	إذا (was_curwin && wp == curwin)
	    أرجع -1;
	أرجع TRUE;
    }

    argv[0].v_type = VAR_عدد;
    argv[0].vval.v_عدد = (varnumber_T)wp->w_id;

    // Convert the number to a string, so that the function can use:
    //	    if a:c == "\<F2>"
    buf[special_مقصد_buf(c, mod_ماسک, FALSE, buf)] = NUL;
    argv[1].v_type = VAR_STRING;
    argv[1].vval.v_string = vim_strsave(buf);

    argv[2].v_type = VAR_UNKNOWN;

    // NOTE: The callback might close the popup and make "wp" invalid.
    إذا (call_callback(&wp->w_filter_cb, -1, &rettv, 2, argv) == FAIL)
    {
	// Cannot call the function, close the popup to avoid that the filter
	// eats keys and the user is stuck.  Might as well eat the key.
	popup_close_with_retval(wp, -1);
	res = TRUE;
    }
    وإلا
    {
	إذا (win_valid_popup(wp) && old_lnum != wp->w_cursor.lnum)
	    popup_highlight_curline(wp);

	// If an error message was given always return FALSE, so that keys are
	// not consumed and the user can type something.
	// If we get three errors in a row then close the popup.  Decrement the
	// error count by 1/10 if there are no errors, thus allowing up to 1 in
	// 10 calls to cause an error.
	إذا (win_valid_popup(wp) && did_emsg > prev_did_emsg)
	{
	    wp->w_filter_errors += 10;
	    إذا (wp->w_filter_errors >= 30)
		popup_close_with_retval(wp, -1);
	    res = FALSE;
	}
	وإلا
	{
	    إذا (win_valid_popup(wp) && wp->w_filter_errors > 0)
		--wp->w_filter_errors;
	    res = tv_get_bool(&rettv);
	}
    }

    vim_free(argv[1].vval.v_string);
    clear_tv(&rettv);
    أرجع res;
}

/*
 * Called when "c" was typed: invoke popup filter callbacks.
 * Returns TRUE when the character was consumed,
 */
    صحيح
popup_do_filter(صحيح c)
{
    ساكن صحيح	recursive = FALSE;
    صحيح		res = FALSE;
    win_T	*wp;
    صحيح		save_کلید_Typed = کلید_Typed;
    صحيح		state;
    صحيح		was_must_redraw = must_redraw;

#اگر_تعریف_شده FEAT_TERMINAL
    // Popup window with terminal always gets focus.
    إذا (popup_is_popup(curwin) && curbuf->b_جمله != عدم)
	أرجع FALSE;
#پایان_اگر

    // Vim is waiting for a key at a prompt of its own, the key is not meant
    // for a popup filter. MODE_HITRETURN includes MODE_NORMAL, thus the
    // filter mode check below would match.
    إذا (State == نما_HITRETURN || State == نما_ASKMORE
	    || State == نما_CONFIRM || State == نما_EXTERNCMD)
	أرجع FALSE;

    إذا (recursive)
	أرجع FALSE;
    recursive = TRUE;

    إذا (c == K_LEFTMOUSE)
    {
	صحيح ردیف = mouse_ردیف;
	صحيح ستون = mouse_ستون;

	wp = mouse_پیدا_کن_win(&ردیف, &ستون, پیدا_کن_POPUP);
	إذا (wp != عدم && popup_close_if_on_X(wp, ردیف, ستون))
	    res = TRUE;
    }

    popup_reset_handled(POPUP_HANDLED_2);
    state = get_real_state();
    طالما (res == FALSE
		     && (wp = پیدا_کن_بعدی_popup(FALSE, POPUP_HANDLED_2)) != عدم)
	إذا (wp->w_filter_cb.cb_name != عدم
		&& (wp->w_filter_نما & state) != 0)
	    res = invoke_popup_filter(wp, c);

    إذا (must_redraw > was_must_redraw)
    {
	صحيح save_got_int = got_int;

	// Reset got_int to avoid a function used in the statusline aborts.
	got_int = FALSE;
	redraw_after_callback(FALSE, FALSE);
	got_int |= save_got_int;
    }
    recursive = FALSE;
    کلید_Typed = save_کلید_Typed;

    // When interrupted return FALSE to avoid looping.
    أرجع res == -1 ? FALSE : res;
}

/*
 * Return TRUE if there is a popup visible with a filter callback and the
 * "mapping" property off.
 */
    صحيح
popup_no_mapping(فراغ)
{
    صحيح	    دور;
    win_T   *wp;

    لكل (دور = 1; دور <= 2; ++دور)
	لكل (wp = دور == 1 ? اولی_popupwin : curtab->tp_اولی_popupwin;
						   wp != عدم; wp = wp->w_بعدی)
	    إذا (wp->w_filter_cb.cb_name != عدم
		    && (wp->w_popup_flags & (POPF_HIDDEN | POPF_MAPPING)) == 0)
		أرجع TRUE;
    أرجع FALSE;
}

/*
 * Called when the cursor moved: check if any popup needs to be closed if the
 * cursor moved far enough.
 */
    فراغ
popup_check_cursor_pos(فراغ)
{
    win_T *wp;

    popup_reset_handled(POPUP_HANDLED_3);
    طالما ((wp = پیدا_کن_بعدی_popup(TRUE, POPUP_HANDLED_3)) != عدم)
	إذا (wp->w_popup_curwin != عدم
		&& (curwin != wp->w_popup_curwin
		    || curwin->w_cursor.lnum != wp->w_popup_lnum
		    || curwin->w_cursor.ستون < wp->w_popup_mincol
		    || curwin->w_cursor.ستون > wp->w_popup_maxcol))
	    popup_close_with_retval(wp, -1);
}

/*
 * Update "w_popup_mask_cells".
 */
    ساكن فراغ
popup_update_ماسک(win_T *wp, صحيح عرض, صحيح ارتفاع)
{
    listitem_T	*lio, *li;
    char_u	*cells;
    صحيح		ردیف, ستون;

    إذا (wp->w_popup_ماسک == عدم || عرض == 0 || ارتفاع == 0)
    {
	VIM_CLEAR(wp->w_popup_ماسک_cells);
	أرجع;
    }
    إذا (wp->w_popup_ماسک_cells != عدم
	    && wp->w_popup_ماسک_ارتفاع == ارتفاع
	    && wp->w_popup_ماسک_عرض == عرض)
	أرجع;  // cache is still valid

    vim_free(wp->w_popup_ماسک_cells);
    إذا (عرض > 0 && (size_t)ارتفاع > اندازه_MAX / (size_t)عرض)
    {
	wp->w_popup_ماسک_cells = عدم;
	أرجع;
    }
    wp->w_popup_ماسک_cells = alloc_clear((size_t)عرض * ارتفاع);
    إذا (wp->w_popup_ماسک_cells == عدم)
	أرجع;
    cells = wp->w_popup_ماسک_cells;

    FOR_ALL_LIST_اقلام(wp->w_popup_ماسک, lio)
    {
	صحيح تعداد_ستون, cole;
	صحيح سطرها, linee;

	li = lio->li_tv.vval.v_list->lv_اولی;
	تعداد_ستون = tv_get_عدد(&li->li_tv);
	إذا (تعداد_ستون < 0)
	    تعداد_ستون = عرض + تعداد_ستون + 1;
	إذا (تعداد_ستون <= 0)
	    تعداد_ستون = 1;
	li = li->li_بعدی;
	cole = tv_get_عدد(&li->li_tv);
	إذا (cole < 0)
	    cole = عرض + cole + 1;
	إذا (cole > عرض)
	    cole = عرض;
	li = li->li_بعدی;
	سطرها = tv_get_عدد(&li->li_tv);
	إذا (سطرها < 0)
	    سطرها = ارتفاع + سطرها + 1;
	إذا (سطرها <= 0)
	    سطرها = 1;
	li = li->li_بعدی;
	linee = tv_get_عدد(&li->li_tv);
	إذا (linee < 0)
	    linee = ارتفاع + linee + 1;
	إذا (linee > ارتفاع)
	    linee = ارتفاع;

	لكل (ردیف = سطرها - 1; ردیف < linee; ++ردیف)
	    لكل (ستون = تعداد_ستون - 1; ستون < cole; ++ستون)
		cells[ردیف * عرض + ستون] = 1;
    }
}

/*
 * Return TRUE if "col" / "line" matches with an entry in w_popup_mask.
 * "col" and "line" are screen coordinates.
 */
    ساكن صحيح
popup_masked(win_T *wp, صحيح عرض, صحيح ارتفاع, صحيح screencol, صحيح screenline)
{
    صحيح ستون = screencol - wp->w_wincol + wp->w_popup_leftoff;
    صحيح سطر = screenline - wp->w_winrow;

    أرجع ستون >= 0 && ستون < عرض
	    && سطر >= 0 && سطر < ارتفاع
	    && wp->w_popup_ماسک_cells[سطر * عرض + ستون];
}

/*
 * Set flags in popup_transparent[] for window "wp" to "val".
 */
    ساكن فراغ
update_popup_transparent(win_T *wp, صحيح val)
{
    إذا (wp->w_popup_ماسک == عدم)
	أرجع;

    صحيح		عرض = popup_عرض(wp);
    صحيح		ارتفاع = popup_ارتفاع(wp);
    listitem_T	*lio, *li;
    صحيح		تعداد_ستون, cole;
    صحيح		سطرها, linee;
    صحيح		ستون, سطر;

    FOR_ALL_LIST_اقلام(wp->w_popup_ماسک, lio)
    {
	li = lio->li_tv.vval.v_list->lv_اولی;
	تعداد_ستون = tv_get_عدد(&li->li_tv);
	إذا (تعداد_ستون < 0)
	    تعداد_ستون = عرض + تعداد_ستون + 1;
	li = li->li_بعدی;
	cole = tv_get_عدد(&li->li_tv);
	إذا (cole < 0)
	    cole = عرض + cole + 1;
	li = li->li_بعدی;
	سطرها = tv_get_عدد(&li->li_tv);
	إذا (سطرها < 0)
	    سطرها = ارتفاع + سطرها + 1;
	li = li->li_بعدی;
	linee = tv_get_عدد(&li->li_tv);
	إذا (linee < 0)
	    linee = ارتفاع + linee + 1;

	--تعداد_ستون;
	تعداد_ستون -= wp->w_popup_leftoff;
	إذا (تعداد_ستون < 0)
	    تعداد_ستون = 0;
	cole -= wp->w_popup_leftoff;
	--سطرها;
	إذا (سطرها < 0)
	    سطرها = 0;
	لكل (سطر = سطرها; سطر < linee
		&& سطر + wp->w_winrow < screen_ردیفها; ++سطر)
	    لكل (ستون = تعداد_ستون; ستون < cole
		    && ستون + wp->w_wincol < screen_Columns; ++ستون)
		popup_transparent[(سطر + wp->w_winrow) * screen_Columns
		    + ستون + wp->w_wincol] = val;
    }
}

/*
 * Only called when popup window "wp" is hidden: If the window is positioned
 * next to a text property, and it is now visible, then  unhide the popup.
 * We don't check if visible popups become hidden, that is done in
 * popup_adjust_position().
 * Return TRUE if the popup became unhidden.
 */
    ساكن صحيح
check_popup_unhidden(win_T *wp)
{
    إذا (wp->w_popup_prop_type > 0 && win_valid(wp->w_popup_prop_win))
    {
	textprop_T  prop;
	linenr_T    lnum;
	bool	    یافت = false;

	إذا ((wp->w_popup_flags & POPF_HIDDEN_FORCE) != 0)
	    أرجع FALSE;
	إذا (پیدا_کن_visible_prop(wp->w_popup_prop_win,
				wp->w_popup_prop_type, wp->w_popup_prop_id,
						       &prop, &lnum))
	    یافت = true;
	// The textprop may have scrolled just above the host window's top.
	// Unhide the popup so popup_adjust_position() can roll it partially
	// onto the host's top edge via the top-clip path.  Limit the search
	// to the popup's own height so we do not resurrect a popup whose
	// prop is already further off-screen than the popup can extend.
	وإلا إذا (popup_پیدا_کن_prop_above_بالاترین(wp, wp->w_popup_prop_win,
					    popup_ارتفاع(wp), &prop, &lnum))
	    یافت = true;
	إذا (یافت)
	{
	    wp->w_popup_flags &= ~POPF_HIDDEN;
	    wp->w_popup_prop_topline = 0; // force repositioning
	    أرجع TRUE;
	}
    }
    أرجع FALSE;
}

/*
 * Return TRUE if popup_adjust_position() needs to be called for "wp".
 * That is when the buffer in the popup was changed, or the popup is following
 * a textprop and the referenced buffer was changed.
 * Or when the cursor line changed and "cursorline" is set.
 */
    ساكن صحيح
popup_need_position_adjust(win_T *wp)
{
    إذا (wp->w_popup_آخرین_changedtick != CHANGEDTICK(wp->w_میانگیر))
	أرجع TRUE;
    إذا (win_valid(wp->w_popup_prop_win)
	    && (wp->w_popup_prop_changedtick
				 != CHANGEDTICK(wp->w_popup_prop_win->w_میانگیر)
	       || wp->w_popup_prop_topline != wp->w_popup_prop_win->w_topline
	       || wp->w_popup_prop_winrow != wp->w_popup_prop_win->w_winrow
	       || wp->w_popup_prop_wincol != wp->w_popup_prop_win->w_wincol
	       || wp->w_popup_prop_عرض != wp->w_popup_prop_win->w_عرض
	       || wp->w_popup_prop_winheight != wp->w_popup_prop_win->w_ارتفاع))
	أرجع TRUE;

    // May need to adjust the width if the cursor moved.
    أرجع wp->w_cursor.lnum != wp->w_popup_آخرین_curline;
}

// Cached array with max zindex of opacity popups covering each cell.
// Allocated in may_update_popup_mask() when opacity popups exist.
ساكن کوتاه *opacity_zindex = عدم;
ساكن صحيح    opacity_zindex_ردیفها = 0;
ساكن صحيح    opacity_zindex_تعداد_ستون = 0;

/*
 * Mark cells covered by opacity popup "wp" in opacity_zindex[].
 * Stores the maximum zindex so that lower popups can be suppressed too.
 */
    ساكن فراغ
popup_mark_opacity_zindex(win_T *wp)
{
    صحيح	    عرض;
    صحيح	    ارتفاع;
    صحيح	    r, c;

    إذا (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	أرجع;

    عرض = popup_عرض(wp);
    ارتفاع = popup_ارتفاع(wp);
    // w_winrow/w_wincol are visible cells (>= 0); w_popup_topoff rows are
    // clipped off the top.
    لكل (r = wp->w_winrow;
		       r < wp->w_winrow + ارتفاع - wp->w_popup_topoff
						&& r < screen_ردیفها; ++r)
	لكل (c = wp->w_wincol;
		 c < wp->w_wincol + عرض - wp->w_popup_leftoff
						&& c < screen_Columns; ++c)
	{
	    صحيح off = r * screen_Columns + c;
	    إذا (wp->w_zindex > opacity_zindex[off])
		opacity_zindex[off] = wp->w_zindex;
	}
}

/*
 * Force background windows to redraw rows under an opacity popup.
 */
    ساكن فراغ
redraw_win_under_opacity_popup(win_T *wp)
{
    صحيح	    ارتفاع;
    صحيح	    عرض;
    صحيح	    r;

    إذا (!(wp->w_popup_flags & POPF_OPACITY) || wp->w_popup_blend <= 0
	    || (wp->w_popup_flags & POPF_HIDDEN))
	أرجع;

    ارتفاع = popup_ارتفاع(wp);
    عرض = popup_عرض(wp);
    لكل (r = wp->w_winrow;
		       r < wp->w_winrow + ارتفاع && r < screen_ردیفها; ++r)
    {
	صحيح	    ستون;
	win_T	    *prev_twp = عدم;

	// Check across the full width of the popup to find all underlying
	// windows (e.g., when the popup spans a vertical split).
	لكل (ستون = wp->w_wincol;
		ستون < wp->w_wincol + عرض && ستون < screen_Columns; ++ستون)
	{
	    صحيح	    سطر_cp = r;
	    صحيح	    ستون_cp = ستون;
	    win_T   *twp;

	    twp = mouse_پیدا_کن_win(&سطر_cp, &ستون_cp, IGNORE_POPUP);
	    إذا (twp != عدم && twp != prev_twp)
	    {
		prev_twp = twp;
		إذا (سطر_cp < twp->w_ارتفاع)
		{
		    linenr_T lnum;

#اگر_تعریف_شده FEAT_TERMINAL
		    // A terminal only repaints vterm-damaged rows; force a
		    // full repaint so the opacity blend sees the true
		    // background, not stale blended cells.
		    إذا (جمله_do_update_پنجره(twp))
			twp->w_redr_type = UPD_NOT_VALID;
		    وإلا
#پایان_اگر
		    {
			(فراغ)mouse_comp_pos(twp, &سطر_cp, &ستون_cp, &lnum,
									عدم);
			// Called from inside update_screen(); raising
			// must_redraw would loop the outer redraw indefinitely.
			redraw_win_range_now(twp, lnum, lnum);
		    }
		}
		وإلا إذا (سطر_cp == twp->w_ارتفاع)
		    // Status bar line: mark for redraw to prevent
		    // opacity blend accumulation.
		    twp->w_redr_وضعیت = true;
	    }
	}
    }
}


/*
 * Return TRUE if cell (row, col) is covered by a higher-zindex opacity popup.
 */
    صحيح
popup_is_under_opacity(صحيح ردیف, صحيح ستون)
{
    إذا (opacity_zindex == عدم
	    || ردیف < 0 || ردیف >= opacity_zindex_ردیفها
	    || ستون < 0 || ستون >= opacity_zindex_تعداد_ستون)
	أرجع FALSE;
    أرجع opacity_zindex[ردیف * opacity_zindex_تعداد_ستون + ستون] > screen_zindex;
}

/*
 * Return TRUE if cell (row, col) is covered by a lower-zindex opacity popup.
 */
    صحيح
popup_is_over_opacity(صحيح ردیف, صحيح ستون)
{
    win_T *wp;

    FOR_ALL_POPUPWINS(wp)
	إذا ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& ردیف >= wp->w_winrow
		&& ردیف < wp->w_winrow + popup_ارتفاع(wp)
		&& ستون >= wp->w_wincol
		&& ستون < wp->w_wincol + popup_عرض(wp))
	    أرجع TRUE;
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	إذا ((wp->w_popup_flags & POPF_OPACITY)
		&& wp->w_popup_blend > 0
		&& !(wp->w_popup_flags & POPF_HIDDEN)
		&& wp->w_zindex < screen_zindex
		&& ردیف >= wp->w_winrow
		&& ردیف < wp->w_winrow + popup_ارتفاع(wp)
		&& ستون >= wp->w_wincol
		&& ستون < wp->w_wincol + popup_عرض(wp))
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Return TRUE if any cell in row "row" from "start_col" to "end_col"
 * (exclusive) is covered by a higher-zindex opacity popup.
 */
    صحيح
popup_is_under_opacity_range(صحيح ردیف, صحيح شروع_ستون, صحيح انتها_ستون)
{
    صحيح ستون;

    إذا (opacity_zindex == عدم
	    || ردیف < 0 || ردیف >= opacity_zindex_ردیفها)
	أرجع FALSE;
    لكل (ستون = شروع_ستون; ستون < انتها_ستون && ستون < opacity_zindex_تعداد_ستون; ++ستون)
	إذا (opacity_zindex[ردیف * opacity_zindex_تعداد_ستون + ستون] > screen_zindex)
	    أرجع TRUE;
    أرجع FALSE;
}

/*
 * Update "popup_mask" if needed.
 * Also recomputes the popup size and positions.
 * Also updates "popup_visible" and "popup_uses_mouse_move".
 * Also marks window lines for redrawing.
 */
    فراغ
may_update_popup_ماسک(صحيح type)
{
    win_T	*wp;
    کوتاه	*ماسک;
    صحيح		سطر, ستون;
    صحيح		redraw_all_popups = FALSE;
    صحيح		redrawing_all_win;

    // Need to recompute when switching tabs.
    // Also recompute when the type is UPD_CLEAR or UPD_NOT_VALID, something
    // basic (such as the screen size) must have changed.
    إذا (popup_ماسک_tab != curtab || type >= UPD_NOT_VALID)
    {
	popup_ماسک_refresh = TRUE;
	redraw_all_popups = TRUE;
    }

    // Check if any popup window buffer has changed and if any popup connected
    // to a text property has become visible.
    FOR_ALL_POPUPWINS(wp)
	إذا (wp->w_popup_flags & POPF_HIDDEN)
	    popup_ماسک_refresh |= check_popup_unhidden(wp);
	وإلا إذا (popup_need_position_adjust(wp))
	    popup_ماسک_refresh = TRUE;
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	إذا (wp->w_popup_flags & POPF_HIDDEN)
	    popup_ماسک_refresh |= check_popup_unhidden(wp);
	وإلا إذا (popup_need_position_adjust(wp))
	    popup_ماسک_refresh = TRUE;

    // Force background windows to redraw rows under opacity popups.
    // Opacity popups don't participate in popup_mask, so their area
    // wouldn't normally be redrawn.  Without this, ScreenAttrs retains
    // blended values from the previous cycle, causing blend accumulation.
    // This must run every cycle, not just when popup_mask_refresh is set.
    //
    // Also build the opacity_zindex array used by screen_char() to suppress
    // output for cells under opacity popups during background draw.
    {
	صحيح has_opacity = FALSE;

	FOR_ALL_POPUPWINS(wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    إذا ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}
	FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	{
	    redraw_win_under_opacity_popup(wp);
	    إذا ((wp->w_popup_flags & POPF_OPACITY)
		    && wp->w_popup_blend > 0
		    && !(wp->w_popup_flags & POPF_HIDDEN))
		has_opacity = TRUE;
	}

	إذا (!has_opacity)
	{
	    VIM_CLEAR(opacity_zindex);
	    opacity_zindex_ردیفها = 0;
	    opacity_zindex_تعداد_ستون = 0;
	}
	وإلا
	{
	    إذا (opacity_zindex_ردیفها != screen_ردیفها
		    || opacity_zindex_تعداد_ستون != screen_Columns)
	    {
		vim_free(opacity_zindex);
		opacity_zindex = LALLOC_MULT(کوتاه,
					screen_ردیفها * screen_Columns);
		opacity_zindex_ردیفها = screen_ردیفها;
		opacity_zindex_تعداد_ستون = screen_Columns;
	    }
	    إذا (opacity_zindex != عدم)
	    {
		vim_memset(opacity_zindex, 0,
		    (size_t)screen_ردیفها * screen_Columns * حجم(کوتاه));

		FOR_ALL_POPUPWINS(wp)
		    popup_mark_opacity_zindex(wp);
		FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
		    popup_mark_opacity_zindex(wp);
	    }
	}
    }

    إذا (!popup_ماسک_refresh)
	أرجع;

    // Need to update the mask, something has changed.
    popup_ماسک_refresh = FALSE;
    popup_ماسک_tab = curtab;
    popup_visible = FALSE;

    // If redrawing all windows, just update "popup_mask".
    // If redrawing only what is needed, update "popup_mask_next" and then
    // compare with "popup_mask" to see what changed.
    redrawing_all_win = TRUE;
    FOR_ALL_WINDOWS(wp)
	إذا (wp->w_redr_type < UPD_SOME_VALID)
	    redrawing_all_win = FALSE;
    إذا (redrawing_all_win)
	ماسک = popup_ماسک;
    وإلا
	ماسک = popup_ماسک_بعدی;
    vim_memset(ماسک, 0, (size_t)screen_ردیفها * screen_Columns * حجم(کوتاه));

    // Find the window with the lowest zindex that hasn't been handled yet,
    // so that the window with a higher zindex overwrites the value in
    // popup_mask.
    popup_reset_handled(POPUP_HANDLED_4);
    طالما ((wp = پیدا_کن_بعدی_popup(TRUE, POPUP_HANDLED_4)) != عدم)
    {
	صحيح عرض;
	صحيح ارتفاع;

	popup_visible = TRUE;

	// Recompute the position if the text changed.  It may make the popup
	// hidden if it's attach to a text property that is no longer visible.
	إذا (redraw_all_popups || popup_need_position_adjust(wp))
	{
	    popup_adjust_position(wp);
	    إذا (wp->w_popup_flags & POPF_HIDDEN)
		استمر;
	}

	عرض = popup_عرض(wp);
	// Match the drawn extent computed by update_popups so that cells
	// outside the clipped popup are not marked as popup-owned and the
	// background window can draw through them.
	إذا (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	{
	    popup_clip_T cl;

	    popup_compute_clip(wp, &cl);
	    ارتفاع = cl.eff_ارتفاع;
	}
	وإلا
	    ارتفاع = popup_ارتفاع(wp);
	popup_update_ماسک(wp, عرض, ارتفاع);

	// Popup with partial transparency do not block lower layers from
	// drawing, so they don't participate in the popup_mask.
	// Fully opaque popups (blend == 0) still block lower layers.
	إذا ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    استمر;

	{
	    صحيح ماسک_شروع = wp->w_winrow;
	    صحيح ماسک_انتها = ماسک_شروع + ارتفاع;
	    صحيح ماسک_ستون_شروع = wp->w_wincol + wp->w_popup_leftclip;
	    صحيح ماسک_ستون_انتها = wp->w_wincol + عرض - wp->w_popup_leftoff
						    - wp->w_popup_rightclip;

	    لكل (سطر = ماسک_شروع;
			    سطر < ماسک_انتها && سطر < screen_ردیفها; ++سطر)
		لكل (ستون = ماسک_ستون_شروع;
		     ستون < ماسک_ستون_انتها && ستون < screen_Columns; ++ستون)
		    إذا (wp->w_zindex < POPUPMENU_ZINDEX
			    && pum_visible()
			    && pum_under_menu(سطر, ستون, FALSE))
			ماسک[سطر * screen_Columns + ستون] = POPUPMENU_ZINDEX;
		    وإلا إذا (wp->w_popup_ماسک_cells == عدم
				    || !popup_masked(wp, عرض, ارتفاع, ستون, سطر))
			ماسک[سطر * screen_Columns + ستون] = wp->w_zindex;
	}
    }

    // Only check which lines are to be updated if not already
    // updating all lines.
    إذا (ماسک == popup_ماسک_بعدی)
    {
	صحيح	    *plines_cache = ALLOC_CLEAR_MULT(صحيح, ردیفها);
	win_T	    *prev_wp = عدم;

	لكل (سطر = 0; سطر < screen_ردیفها; ++سطر)
	{
	    صحيح	    ستون_انجامشده = 0;

	    لكل (ستون = 0; ستون < screen_Columns; ++ستون)
	    {
		صحيح off = سطر * screen_Columns + ستون;

		إذا (popup_ماسک[off] != popup_ماسک_بعدی[off])
		{
		    popup_ماسک[off] = popup_ماسک_بعدی[off];

		    إذا (سطر >= cmdline_ردیف)
		    {
			// the command line needs to be cleared if text below
			// the popup is now visible.
			إذا (!پیام_scrolled && popup_ماسک_بعدی[off] == 0)
			    clear_cmdline = TRUE;
		    }
		    وإلا إذا (ستون >= ستون_انجامشده)
		    {
			linenr_T	lnum;
			صحيح		سطر_cp = سطر;
			صحيح		ستون_cp = ستون;

			// The screen position "line" / "col" needs to be
			// redrawn.  Figure out what window that is and update
			// w_redraw_top and w_redraw_bot.  Only needs to be
			// done once for each window line.
			wp = mouse_پیدا_کن_win(&سطر_cp, &ستون_cp, IGNORE_POPUP);
			إذا (wp != عدم)
			{
#إذا defined(FEAT_TERMINAL)
			    // A terminal window needs to be redrawn.
			    إذا (bt_terminal(wp->w_میانگیر))
				redraw_win_later(wp, UPD_NOT_VALID);
			    وإلا
#پایان_اگر
			    {
				إذا (wp != prev_wp)
				{
				    vim_memset(plines_cache, 0,
							   حجم(صحيح) * ردیفها);
				    prev_wp = wp;
				}

				إذا (سطر_cp >= wp->w_ارتفاع)
				    // In (or below) status line
				    wp->w_redr_وضعیت = true;
				وإلا
				{
				    // compute the position in the buffer line
				    // from the position in the window
				    (فراغ)mouse_comp_pos(wp, &سطر_cp, &ستون_cp,
							  &lnum, plines_cache);
				    redrawWinline(wp, lnum);
				}
			    }

			    // This line is going to be redrawn, no need to
			    // check until the right side of the window.
			    ستون_انجامشده = wp->w_wincol + wp->w_عرض - 1;
			}
		    }
		}
	    }
	}

	vim_free(plines_cache);
    }

    update_popup_استفاده_میکند_mouse_move();
}

/*
 * If the current window is a popup and something relevant changed, recompute
 * the position and size.
 */
    فراغ
may_update_popup_position(فراغ)
{
    إذا (popup_is_popup(curwin) && popup_need_position_adjust(curwin))
	popup_adjust_position(curwin);
}

#اگر_تعریف_شده FEAT_PROP_POPUP
ساكن schar_T *پایه_screenlines = عدم;
ساكن صحيح *پایه_screenattrs = عدم;
ساكن u8char_T *پایه_screenlinesuc = عدم;
ساكن صحيح پایه_screen_ردیفها = 0;
ساكن صحيح پایه_screen_تعداد_ستون = 0;

/*
 * Get the base screen cell saved before drawing opacity popups.
 * Returns TRUE if the cell is available.
 */
    صحيح
popup_get_پایه_screen_cell(صحيح ردیف, صحيح ستون, schar_T *linep, صحيح *attrp,
							  u8char_T *ucp)
{
    إذا (پایه_screenlines == عدم || پایه_screenattrs == عدم)
	أرجع FALSE;
    إذا (ردیف < 0 || ستون < 0 || ردیف >= پایه_screen_ردیفها
						     || ستون >= پایه_screen_تعداد_ستون)
	أرجع FALSE;

    صحيح off = ردیف * پایه_screen_تعداد_ستون + ستون;
    إذا (linep != عدم)
	*linep = پایه_screenlines[off];
    إذا (attrp != عدم)
	*attrp = پایه_screenattrs[off];
    إذا (ucp != عدم)
    {
	إذا (enc_utf8 && پایه_screenlinesuc != عدم)
	    *ucp = پایه_screenlinesuc[off];
	وإلا
	    *ucp = 0;
    }
    أرجع TRUE;
}

/*
 * Set the base screen cell saved before drawing opacity popups.
 * Used to update the snapshot after blending a layer.
 */
    فراغ
popup_مجموعه_پایه_screen_cell(صحيح ردیف, صحيح ستون, schar_T سطر, صحيح attr, u8char_T uc)
{
    إذا (پایه_screenlines == عدم || پایه_screenattrs == عدم)
	أرجع;
    إذا (ردیف < 0 || ستون < 0 || ردیف >= پایه_screen_ردیفها
						     || ستون >= پایه_screen_تعداد_ستون)
	أرجع;

    صحيح off = ردیف * پایه_screen_تعداد_ستون + ستون;
    پایه_screenlines[off] = سطر;
    پایه_screenattrs[off] = attr;
    إذا (enc_utf8 && پایه_screenlinesuc != عدم)
	پایه_screenlinesuc[off] = uc;
}
#پایان_اگر

/*
 * Draw a single padding cell with opacity blending.
 * Restores background from saved data and blends with popup attribute.
 */
ساكن فراغ
draw_opacity_padding_cell(
	صحيح		ردیف,
	صحيح		ستون,
	popup_saved_screen_T *saved_screen,
	صحيح		pad_شروع_ستون,
	صحيح		pad_انتها_ستون)
{
    إذا (سطر_جابه_جایی == عدم || Screen_سطرها == عدم || ScreenAttrs == عدم
	    || (enc_utf8 && Screen_سطرها_UC == عدم))
	أرجع;

    صحيح off = سطر_جابه_جایی[ردیف] + ستون;
    صحيح r = ردیف - saved_screen->شروع_ردیف;
    صحيح c = ستون - saved_screen->شروع_ستون;

    إذا (r >= 0 && r < saved_screen->ردیفها_2 && c >= 0 && c < saved_screen->تعداد_ستون)
    {
	صحيح save_off = r * saved_screen->تعداد_ستون + c;
	// If this is the second cell of a wide background character, blend
	// the wide character instead of overwriting it.
	إذا (enc_utf8 && saved_screen->linesuc != عدم)
	{
	    صحيح پایه_ستون = ستون - 1;
	    صحيح پایه_off = off - 1;
	    صحيح پایه_save_off = save_off - 1;
	    صحيح wide_prev = FALSE;

	    // Prefer current screen state for detecting a wide char, since the
	    // saved data may not contain a reliable right-half marker.
	    إذا (پایه_off >= 0)
	    {
		إذا (Screen_سطرها_UC[پایه_off] != 0
			&& utf_char2cells(Screen_سطرها_UC[پایه_off]) == 2
			&& Screen_سطرها[off] == 0)
		    wide_prev = TRUE;
	    }
	    إذا (!wide_prev && save_off > 0)
	    {
		إذا (saved_screen->linesuc[save_off - 1] != 0
			&& utf_char2cells(saved_screen->linesuc[save_off - 1]) == 2
			&& saved_screen->سطرها[save_off] == 0)
		    wide_prev = TRUE;
	    }

	    إذا (wide_prev && پایه_ستون >= 0)
	    {
		// If the wide character starts outside the padding area, do not
		// overwrite it. Use the base screen cell if available.
		إذا (پایه_ستون < pad_شروع_ستون)
		{
		    إذا (Screen_سطرها_UC[پایه_off] != 0
			    && utf_char2cells(Screen_سطرها_UC[پایه_off]) == 2)
		    {
			// The left half still has the wide char on screen.
			// Clear it to an unblended space: only the right half is
			// covered by the popup background.
			Screen_سطرها[پایه_off] = ' ';
			Screen_سطرها_UC[پایه_off] = 0;
			ScreenAttrs[پایه_off] = saved_screen->attrs[پایه_save_off];
			popup_مجموعه_پایه_screen_cell(ردیف, پایه_ستون,
				Screen_سطرها[پایه_off],
				ScreenAttrs[پایه_off],
				Screen_سطرها_UC[پایه_off]);
			screen_char(پایه_off, ردیف, پایه_ستون);

			// Draw padding in the right half.
			// Use left half's attr since the right half of a
			// wide char may have an unreliable attr value.
			Screen_سطرها[off] = ' ';
			ScreenAttrs[off] = saved_screen->attrs[پایه_save_off];
			إذا (enc_utf8)
			    Screen_سطرها_UC[off] = 0;
			صحيح popup_attr_val =
					get_win_attr(screen_opacity_popup);
			صحيح blend = screen_opacity_popup->w_popup_blend;
			ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
					popup_attr_val, blend, TRUE);
			popup_مجموعه_پایه_screen_cell(ردیف, ستون,
				Screen_سطرها[off], ScreenAttrs[off],
				Screen_سطرها_UC[off]);
			screen_char(off, ردیف, ستون);
			أرجع;
		    }
		    // The content drawing cleared the left half to a
		    // space (wide char didn't fit at content edge),
		    // but the saved data has a wide char.  Restore it
		    // spanning both the content cell and padding cell.
		    إذا (پایه_save_off >= 0
			    && saved_screen->linesuc[پایه_save_off] != 0
			    && utf_char2cells(
				saved_screen->linesuc[پایه_save_off]) == 2
			    && Screen_سطرها[پایه_off] == ' '
			    && Screen_سطرها_UC[پایه_off] == 0)
		    {
			صحيح popup_attr_val =
				    get_win_attr(screen_opacity_popup);
			صحيح blend =
				    screen_opacity_popup->w_popup_blend;

			Screen_سطرها[پایه_off] =
				    saved_screen->سطرها[پایه_save_off];
			Screen_سطرها_UC[پایه_off] =
				    saved_screen->linesuc[پایه_save_off];
			ScreenAttrs[پایه_off] =
				    saved_screen->attrs[پایه_save_off];
			ScreenAttrs[پایه_off] = hl_blend_attr(
				    ScreenAttrs[پایه_off],
				    popup_attr_val, blend, TRUE);

			Screen_سطرها[off] = 0;
			Screen_سطرها_UC[off] = 0;
			ScreenAttrs[off] = ScreenAttrs[پایه_off];

			popup_مجموعه_پایه_screen_cell(ردیف, پایه_ستون,
				    Screen_سطرها[پایه_off],
				    ScreenAttrs[پایه_off],
				    Screen_سطرها_UC[پایه_off]);
			popup_مجموعه_پایه_screen_cell(ردیف, ستون,
				    Screen_سطرها[off],
				    ScreenAttrs[off],
				    Screen_سطرها_UC[off]);
			screen_char(پایه_off, ردیف, پایه_ستون);
			أرجع;
		    }

		    // Draw padding in the right half.
		    // Use left half's attr since the right half of a
		    // wide char may have an unreliable attr value.
		    Screen_سطرها[off] = ' ';
		    ScreenAttrs[off] = saved_screen->attrs[پایه_save_off];
		    إذا (enc_utf8)
			Screen_سطرها_UC[off] = 0;
		    صحيح popup_attr_val = get_win_attr(screen_opacity_popup);
		    صحيح blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				    popup_attr_val, blend, TRUE);
		    popup_مجموعه_پایه_screen_cell(ردیف, ستون, Screen_سطرها[off],
					       ScreenAttrs[off], Screen_سطرها_UC[off]);
		    screen_char(off, ردیف, ستون);
		    أرجع;
		}

		// Base cell is inside the saved area, redraw the wide char.
		إذا (save_off > 0)
		{
		    Screen_سطرها[پایه_off] = saved_screen->سطرها[پایه_save_off];
		    ScreenAttrs[پایه_off] = saved_screen->attrs[پایه_save_off];
		    Screen_سطرها[off] = saved_screen->سطرها[save_off];
		    ScreenAttrs[off] = saved_screen->attrs[save_off];
		    Screen_سطرها_UC[پایه_off] = saved_screen->linesuc[پایه_save_off];
		    Screen_سطرها_UC[off] = saved_screen->linesuc[save_off];

		    صحيح popup_attr_val = get_win_attr(screen_opacity_popup);
		    صحيح blend = screen_opacity_popup->w_popup_blend;
		    ScreenAttrs[پایه_off] = hl_blend_attr(ScreenAttrs[پایه_off],
				    popup_attr_val, blend, TRUE);
		    ScreenAttrs[off] = ScreenAttrs[پایه_off];
		    popup_مجموعه_پایه_screen_cell(ردیف, پایه_ستون, Screen_سطرها[پایه_off],
					       ScreenAttrs[پایه_off], Screen_سطرها_UC[پایه_off]);
		    popup_مجموعه_پایه_screen_cell(ردیف, ستون, Screen_سطرها[off],
					       ScreenAttrs[off], Screen_سطرها_UC[off]);
		    screen_char(پایه_off, ردیف, پایه_ستون);
		}
		أرجع;
	    }
	}
	Screen_سطرها[off] = saved_screen->سطرها[save_off];
	ScreenAttrs[off] = saved_screen->attrs[save_off];
	إذا (enc_utf8)
	    Screen_سطرها_UC[off] = saved_screen->linesuc != عدم
					? saved_screen->linesuc[save_off] : 0;

	// If the saved character is wide and would extend past the padding
	// area into the content area, replace with a space to avoid
	// corrupting the content.
	إذا (enc_utf8 && Screen_سطرها_UC[off] != 0
		&& utf_char2cells(Screen_سطرها_UC[off]) == 2
		&& ستون + 1 >= pad_انتها_ستون)
	{
	    Screen_سطرها[off] = ' ';
	    Screen_سطرها_UC[off] = 0;
	}

	صحيح popup_attr_val = get_win_attr(screen_opacity_popup);
	صحيح blend = screen_opacity_popup->w_popup_blend;
	ScreenAttrs[off] = hl_blend_attr(ScreenAttrs[off],
				popup_attr_val, blend, TRUE);
	popup_مجموعه_پایه_screen_cell(ردیف, ستون, Screen_سطرها[off],
				   ScreenAttrs[off], Screen_سطرها_UC[off]);
	screen_char(off, ردیف, ستون);
    }
}

/*
 * Fill a rectangular padding area with opacity blending.
 */
ساكن فراغ
fill_opacity_padding(
	صحيح		شروع_ردیف,
	صحيح		انتها_ردیف,
	صحيح		شروع_ستون,
	صحيح		انتها_ستون,
	popup_saved_screen_T *saved_screen)
{
    لكل (صحيح pad_ردیف = شروع_ردیف; pad_ردیف < انتها_ردیف; pad_ردیف++)
	لكل (صحيح pad_ستون = شروع_ستون; pad_ستون < انتها_ستون; pad_ستون++)
	    draw_opacity_padding_cell(pad_ردیف, pad_ستون, saved_screen,
		    شروع_ستون, انتها_ستون);
}

/*
 * Update popup windows.  They are drawn on top of normal windows.
 * "win_update" is called for each popup window, lowest zindex first.
 */
    فراغ
update_popups(فراغ (*win_update)(win_T *wp))
{
    win_T   *wp;
    صحيح	    بالاترین_off;
    صحيح	    چپ_extra;
    صحيح	    مجموع_عرض;
    صحيح	    مجموع_ارتفاع_2;
    صحيح	    بالاترین_padding;
    صحيح	    popup_attr;
    صحيح	    border_attr[4];
    صحيح	    border_char[8];
    char_u  buf[MB_MAXBYTES];
    صحيح	    ردیف;
    صحيح	    wincol;
    صحيح	    padcol = 0;
    صحيح	    padendcol = 0;
    صحيح	    i;
    صحيح	    sb_thumb_بالاترین = 0;
    صحيح	    sb_thumb_ارتفاع = 0;
    صحيح	    attr_scroll = 0;
    صحيح	    attr_thumb = 0;
    bool    override_success;

    // hide the cursor until redrawing is done.
    cursor_off();

    // Find the window with the lowest zindex that hasn't been updated yet,
    // so that the window with a higher zindex is drawn later, thus goes on
    // top.
    popup_reset_handled(POPUP_HANDLED_5);
#اگر_تعریف_شده FEAT_PROP_POPUP
    إذا (پایه_screenlines != عدم)
    {
	vim_free(پایه_screenlines);
	پایه_screenlines = عدم;
    }
    إذا (پایه_screenattrs != عدم)
    {
	vim_free(پایه_screenattrs);
	پایه_screenattrs = عدم;
    }
    إذا (پایه_screenlinesuc != عدم)
    {
	vim_free(پایه_screenlinesuc);
	پایه_screenlinesuc = عدم;
    }
    پایه_screen_ردیفها = 0;
    پایه_screen_تعداد_ستون = 0;
#پایان_اگر

    طالما ((wp = پیدا_کن_بعدی_popup(TRUE, POPUP_HANDLED_5)) != عدم)
    {
	صحيح	    title_len = 0;
	صحيح	    title_wincol;
	popup_clip_T cl;

	// Compute the clip geometry once per iteration; w_popup_*off/clip,
	// w_height, w_width, w_popup_border and w_popup_padding are stable
	// for the duration of this iteration (popup_apply_winupdate_clip()
	// mutates w_height/w_width temporarily but the result is restored
	// before any code below reads cl again).
	popup_compute_clip(wp, &cl);

	override_success = افزودن_highlight_overrides(wp->w_hl, wp->w_hl_len);

	// This drawing uses the zindex of the popup window, so that it's on
	// top of the text but doesn't draw when another popup with higher
	// zindex is on top of the character.
	screen_zindex = wp->w_zindex;

	// Set popup with opacity context for screen drawing.
	// Only enable transparency rendering when blend > 0 (not fully opaque).
	إذا ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    screen_opacity_popup = wp;
	وإلا
	    screen_opacity_popup = عدم;

#اگر_تعریف_شده FEAT_PROP_POPUP
	إذا (screen_opacity_popup != عدم)
	{
	    إذا (پایه_screenlines != عدم)
	    {
		vim_free(پایه_screenlines);
		پایه_screenlines = عدم;
	    }
	    إذا (پایه_screenattrs != عدم)
	    {
		vim_free(پایه_screenattrs);
		پایه_screenattrs = عدم;
	    }
	    إذا (پایه_screenlinesuc != عدم)
	    {
		vim_free(پایه_screenlinesuc);
		پایه_screenlinesuc = عدم;
	    }

	    پایه_screen_ردیفها = screen_ردیفها;
	    پایه_screen_تعداد_ستون = screen_Columns;
	    پایه_screenlines = ALLOC_MULT(schar_T,
				    پایه_screen_ردیفها * پایه_screen_تعداد_ستون);
	    پایه_screenattrs = ALLOC_MULT(صحيح,
				    پایه_screen_ردیفها * پایه_screen_تعداد_ستون);
	    إذا (enc_utf8)
		پایه_screenlinesuc = ALLOC_MULT(u8char_T,
				    پایه_screen_ردیفها * پایه_screen_تعداد_ستون);

	    إذا (پایه_screenlines != عدم && پایه_screenattrs != عدم)
	    {
		لكل (صحيح r = 0; r < پایه_screen_ردیفها; r++)
		{
		    صحيح off = سطر_جابه_جایی[r];
		    صحيح پایه_off = r * پایه_screen_تعداد_ستون;
		    لكل (صحيح c = 0; c < پایه_screen_تعداد_ستون; c++)
		    {
			پایه_screenlines[پایه_off + c] = Screen_سطرها[off + c];
			پایه_screenattrs[پایه_off + c] = ScreenAttrs[off + c];
			إذا (enc_utf8 && پایه_screenlinesuc != عدم)
			    پایه_screenlinesuc[پایه_off + c] =
					Screen_سطرها_UC[off + c];
		    }
		}
	    }
	}
#پایان_اگر

	// Save background ScreenLines for padding opacity before win_update()
	// overwrites them.
	popup_saved_screen_T saved_screen = { 0 };

	popup_save_padding_screen(wp, &saved_screen);

	// Set flags in popup_transparent[] for masked cells.
	update_popup_transparent(wp, 1);

	// adjust w_winrow and w_wincol for border and padding, since
	// win_update() doesn't handle them.
	بالاترین_off = popup_بالاترین_extra(wp);
	چپ_extra = wp->w_popup_padding[3] + wp->w_popup_border[3]
							- wp->w_popup_leftoff;
	إذا (wp->w_wincol + چپ_extra < 0)
	    چپ_extra = -wp->w_wincol;
	// Move to the content top, skipping any clipped top border/padding.
	wp->w_winrow += MAX(بالاترین_off - wp->w_popup_topoff, 0);
	wp->w_wincol += چپ_extra;

	// Draw the popup text, unless it's off screen.
	إذا (wp->w_winrow < screen_ردیفها && wp->w_wincol < screen_Columns)
	{
	    popup_geom_save_T saved;

	    popup_geom_save(wp, &saved);

	    // May need to update the "cursorline" highlighting, which may also
	    // change "topline"
	    إذا (wp->w_popup_آخرین_curline != wp->w_cursor.lnum)
		popup_highlight_curline(wp);

	    // Clip the buffer's drawn extent to the host window when
	    // "clipwindow" is set.  The transient mutations are reverted by
	    // popup_geom_restore() so callers continue to see the popup's
	    // logical geometry via popup_getoptions/popup_getpos.
	    popup_apply_winupdate_clip(wp, &cl);

	    win_update(wp);

	    popup_geom_restore(wp, &saved);

	    // move the cursor into the visible lines, otherwise executing
	    // commands with win_execute() may cause the text to jump.
	    إذا (wp->w_cursor.lnum < wp->w_topline)
		wp->w_cursor.lnum = wp->w_topline;
	    وإلا إذا (wp->w_cursor.lnum >= wp->w_botline)
		wp->w_cursor.lnum = wp->w_botline - 1;
	}

	wp->w_winrow -= MAX(بالاترین_off - wp->w_popup_topoff, 0);
	wp->w_wincol -= چپ_extra;

	// Add offset for border and padding if not done already.
	إذا ((wp->w_flags & WFLAG_WCOL_OFF_ADDED) == 0)
	{
	    wp->w_wcol += چپ_extra;
	    wp->w_flags |= WFLAG_WCOL_OFF_ADDED;
	}
	إذا ((wp->w_flags & WFLAG_WROW_OFF_ADDED) == 0)
	{
	    wp->w_wrow += بالاترین_off;
	    wp->w_flags |= WFLAG_WROW_OFF_ADDED;
	}

	// When clipped by "clipwindow", drop the border/padding slot at the
	// clipped edge that we will not render, so the popup ends exactly on
	// the last visible content row (no empty trailing side-border row)
	// and starts on the first visible row when top-clipped.  When
	// unclipped, fall back to the full popup geometry (cl.eff_width
	// excludes w_leftcol and the scrollbar, which popup_width() folds in).
	إذا (wp->w_popup_leftclip > 0 || wp->w_popup_rightclip > 0)
	    مجموع_عرض = cl.eff_عرض;
	وإلا
	    مجموع_عرض = popup_عرض(wp) - wp->w_popup_rightoff;
	إذا (مجموع_عرض < 0)
	    مجموع_عرض = 0;
	إذا (wp->w_popup_topoff > 0 || wp->w_popup_bottomoff > 0)
	    مجموع_ارتفاع_2 = cl.eff_ارتفاع;
	وإلا
	    مجموع_ارتفاع_2 = popup_ارتفاع(wp);
	popup_attr = get_win_attr(wp);

	إذا (wp->w_winrow + مجموع_ارتفاع_2 > cmdline_ردیف)
	    wp->w_popup_flags |= POPF_ON_CMDLINE;
	وإلا
	    wp->w_popup_flags &= ~POPF_ON_CMDLINE;

	// We can only use these line drawing characters when 'encoding' is
	// "utf-8" and 'ambiwidth' is "single".
	إذا (enc_utf8 && *p_ambw == 's')
	{
	    border_char[0] = border_char[2] = 0x2550;
	    border_char[1] = border_char[3] = 0x2551;
	    border_char[4] = 0x2554;
	    border_char[5] = 0x2557;
	    border_char[6] = (wp->w_popup_flags & POPF_RESIZE)
							     ? 0x21f2 : 0x255d;
	    border_char[7] = 0x255a;
	}
	وإلا
	{
	    border_char[0] = border_char[2] = '-';
	    border_char[1] = border_char[3] = '|';
	    لكل (i = 4; i < 8; ++i)
		border_char[i] = '+';
	    إذا (wp->w_popup_flags & POPF_RESIZE)
		border_char[6] = '@';
	}
	لكل (i = 0; i < 8; ++i)
	    إذا (wp->w_border_char[i] != 0)
		border_char[i] = wp->w_border_char[i];

	لكل (i = 0; i < 4; ++i)
	{
	    إذا (wp->w_border_highlight[i] != عدم)
		border_attr[i] = syn_name2attr(wp->w_border_highlight[i]);
	    وإلا إذا (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		border_attr[i] = popup_attr;
	    وإلا
		border_attr[i] = HL_ATTR(HLF_POPB);

	    // Apply blend to border attributes for popup with opacitys
	    إذا ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
		border_attr[i] = hl_blend_attr(0, border_attr[i],
					       wp->w_popup_blend, FALSE);
	}

	// Apply blend to popup_attr for padding areas
	إذا ((wp->w_popup_flags & POPF_OPACITY) && wp->w_popup_blend > 0)
	    popup_attr = hl_blend_attr(0, popup_attr, wp->w_popup_blend, FALSE);


	// Title goes on top of border or padding.
	title_wincol = wp->w_wincol + 1;
	إذا (wp->w_popup_title != عدم)
	{
	    صحيح	    title_attr;

	    إذا (wp->w_popup_border[0] > 0 && wp->w_border_highlight[0] != عدم)
		title_attr = border_attr[0];
	    وإلا إذا (wp->w_hlfwin_id != 0
				       || (wp->w_popup_flags & POPF_INFO))
		title_attr = popup_attr;
	    وإلا
		title_attr = HL_ATTR(HLF_POPT);

	    title_len = vim_strsize(wp->w_popup_title);

	    // truncate the title if too long
	    إذا (title_len > مجموع_عرض - 2)
	    {
		صحيح	title_byte_len = (صحيح)STRLEN(wp->w_popup_title);
		char_u  *title_متن = alloc(title_byte_len + 1);

		إذا (title_متن != عدم)
		{
		    trunc_string(wp->w_popup_title, title_متن,
					  مجموع_عرض - 2, title_byte_len + 1);
		    screen_puts(title_متن, wp->w_winrow, title_wincol,
								   title_attr);
		    vim_free(title_متن);
		}

		title_len = مجموع_عرض - 2;
	    }
	    وإلا
		screen_puts(wp->w_popup_title, wp->w_winrow, title_wincol,
								   title_attr);
	}

	wincol = wp->w_wincol - wp->w_popup_leftoff + wp->w_popup_leftclip;
	بالاترین_padding = cl.eff_padding[0];
	إذا (cl.eff_border[0] > 0)
	{
	    // top border; do not draw over the title
	    إذا (title_len > 0)
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, title_wincol,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			title_wincol + title_len, wincol + مجموع_عرض,
			border_char[0], border_char[0], border_attr[0]);
	    }
	    وإلا
	    {
		screen_fill(wp->w_winrow, wp->w_winrow + 1,
			wincol < 0 ? 0 : wincol, wincol + مجموع_عرض,
			cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[4] : border_char[0],
			border_char[0], border_attr[0]);
	    }
	    إذا (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[5], buf)] = NUL;
		screen_puts(buf, wp->w_winrow,
			       wincol + مجموع_عرض - 1, border_attr[1]);
	    }
	}
	وإلا إذا (cl.eff_padding[0] == 0 && popup_بالاترین_extra(wp) > 0
						    && wp->w_popup_topoff == 0)
	    بالاترین_padding = 1;

	إذا (بالاترین_padding > 0 || wp->w_popup_padding[2] > 0)
	{
	    padcol = wincol + wp->w_popup_border[3];
	    padendcol = wp->w_wincol + مجموع_عرض - wp->w_popup_border[1]
							- wp->w_has_scrollbar;
	    إذا (padcol < 0)
	    {
		padendcol += padcol;
		padcol = 0;
	    }
	}
	إذا (بالاترین_padding > 0)
	{
	    ردیف = wp->w_winrow + cl.eff_border[0];
	    إذا (title_len > 0 && ردیف == wp->w_winrow)
	    {
		// top padding and no border; do not draw over the title
		إذا (screen_opacity_popup != عدم && saved_screen.سطرها != عدم)
		{
		    // Left of title
		    fill_opacity_padding(ردیف, ردیف + 1, padcol, title_wincol,
			    &saved_screen);
		    // Right of title
		    fill_opacity_padding(ردیف, ردیف + 1,
			    title_wincol + title_len, padendcol, &saved_screen);
		}
		وإلا
		{
		    screen_fill(ردیف, ردیف + 1, padcol, title_wincol,
							     ' ', ' ', popup_attr);
		    screen_fill(ردیف, ردیف + 1, title_wincol + title_len,
						  padendcol, ' ', ' ', popup_attr);
		}
		ردیف += 1;
		بالاترین_padding -= 1;
	    }
	    // Draw remaining top padding rows
	    إذا (screen_opacity_popup != عدم && saved_screen.سطرها != عدم)
	    {
		fill_opacity_padding(ردیف, ردیف + بالاترین_padding, padcol, padendcol,
			&saved_screen);
	    }
	    وإلا
	    {
		screen_fill(ردیف, ردیف + بالاترین_padding, padcol, padendcol,
							     ' ', ' ', popup_attr);
	    }
	}

	// Compute scrollbar thumb position and size.
	إذا (wp->w_has_scrollbar)
	{
	    linenr_T	linecount = wp->w_میانگیر->b_ml.ml_سطر_شمار;
	    صحيح		ارتفاع = wp->w_ارتفاع;
	    صحيح		آخرین;

	    sb_thumb_ارتفاع = ((linenr_T)ارتفاع * ارتفاع + linecount / 2)
								   / linecount;
	    إذا (wp->w_topline > 1 && sb_thumb_ارتفاع == ارتفاع)
		--sb_thumb_ارتفاع;  // scrolled, no full thumb
	    إذا (sb_thumb_ارتفاع == 0)
		sb_thumb_ارتفاع = 1;
	    إذا (linecount <= wp->w_ارتفاع || wp->w_ارتفاع == 0)
		// it just fits, avoid divide by zero
		sb_thumb_بالاترین = 0;
	    وإلا
		sb_thumb_بالاترین = (wp->w_topline - 1
				+ (linecount / wp->w_ارتفاع) / 2)
				* (wp->w_ارتفاع - sb_thumb_ارتفاع)
						  / (linecount - wp->w_ارتفاع);
	    إذا (wp->w_topline > 1 && sb_thumb_بالاترین == 0 && ارتفاع > 1)
		sb_thumb_بالاترین = 1;  // show it's scrolled
	    آخرین = مجموع_ارتفاع_2 - بالاترین_off - wp->w_popup_border[2];
	    إذا (sb_thumb_بالاترین >= آخرین)
		// show at least one character
		sb_thumb_بالاترین = آخرین - 1;

	    إذا (wp->w_scrollbar_highlight != عدم)
		attr_scroll = syn_name2attr(wp->w_scrollbar_highlight);
	    وإلا
		attr_scroll = highlight_attr[HLF_PSB];
	    إذا (wp->w_thumb_highlight != عدم)
		attr_thumb = syn_name2attr(wp->w_thumb_highlight);
	    وإلا
		attr_thumb = highlight_attr[HLF_PST];
	}

	// The side-border loop spans the popup's drawn extent.  cl.eff_border
	// and cl.eff_padding collapse the clipped edges to 0 so the loop
	// covers the full visible area without leaving an empty trailing row.
	لكل (i = cl.eff_border[0]; i < مجموع_ارتفاع_2 - cl.eff_border[2]; ++i)
	{
	    صحيح	pad_چپ;
	    // left and right padding only needed next to the body
	    صحيح do_padding =
		    i >= cl.eff_border[0] + cl.eff_padding[0]
		    && i < مجموع_ارتفاع_2 - cl.eff_border[2] - cl.eff_padding[2];

	    ردیف = wp->w_winrow + i;

	    // left border
	    إذا (cl.eff_border[3] > 0 && wincol >= 0)
	    {
		buf[mb_char2bytes(border_char[3], buf)] = NUL;
		screen_puts(buf, ردیف, wincol, border_attr[3]);
	    }
	    إذا (do_padding && cl.eff_padding[3] > 0)
	    {
		صحيح ستون = wincol + wp->w_popup_border[3];

		// left padding
		pad_چپ = wp->w_popup_padding[3];
		إذا (ستون < 0)
		{
		    pad_چپ += ستون;
		    ستون = 0;
		}
		إذا (pad_چپ > 0)
		{
		    إذا (screen_opacity_popup != عدم && saved_screen.سطرها != عدم)
			fill_opacity_padding(ردیف, ردیف + 1, ستون, ستون + pad_چپ,
				&saved_screen);
		    وإلا
			screen_fill(ردیف, ردیف + 1, ستون, ستون + pad_چپ,
							 ' ', ' ', popup_attr);
		}
	    }
	    // scrollbar
	    إذا (wp->w_has_scrollbar)
	    {
		صحيح سطر = i - بالاترین_off;
		صحيح scroll_ستون = wp->w_wincol + مجموع_عرض - 1
						       - wp->w_popup_border[1];

		إذا (سطر >= 0 && سطر < wp->w_ارتفاع)
		    screen_putchar(' ', ردیف, scroll_ستون,
			    سطر >= sb_thumb_بالاترین
				       && سطر < sb_thumb_بالاترین + sb_thumb_ارتفاع
						  ? attr_thumb : attr_scroll);
		وإلا
		    screen_putchar(' ', ردیف, scroll_ستون, popup_attr);
	    }
	    // right border
	    إذا (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[1], buf)] = NUL;
		screen_puts(buf, ردیف, wincol + مجموع_عرض - 1, border_attr[1]);
	    }
	    // right padding
	    إذا (do_padding && cl.eff_padding[1] > 0)
	    {
		صحيح pad_ستون_شروع = wincol + wp->w_popup_border[3]
			+ wp->w_popup_padding[3] + wp->w_عرض;
		صحيح pad_ستون_انتها = pad_ستون_شروع + wp->w_popup_padding[1];

		إذا (screen_opacity_popup != عدم && saved_screen.سطرها != عدم)
		    fill_opacity_padding(ردیف, ردیف + 1, pad_ستون_شروع, pad_ستون_انتها,
			    &saved_screen);
		وإلا
		    screen_fill(ردیف, ردیف + 1, pad_ستون_شروع, pad_ستون_انتها,
							     ' ', ' ', popup_attr);
	    }
	}

	// right shadow
	إذا (wp->w_popup_shadow)
	{
	    صحيح ستون = wincol + مجموع_عرض;
	    لكل (i = 0; i < مجموع_ارتفاع_2; ++i)
	    {
		ردیف = wp->w_winrow + i + 1;
		put_shadow_char(ردیف, ستون);
		put_shadow_char(ردیف, ستون + 1);
	    }
	}

	إذا (cl.eff_padding[2] > 0)
	{
	    // bottom padding -- sits right after the visible content rows.
	    // Derive the row from total_height so it always lands inside the
	    // popup's drawn extent, including the corner case where the top
	    // clip consumes more rows than the content itself (so the visible
	    // content height is zero).  A formula based on
	    // w_height - clip_top_content - clip_bot_content can go negative
	    // there and would draw the padding above w_winrow.
	    ردیف = wp->w_winrow + مجموع_ارتفاع_2
		    - cl.eff_padding[2] - cl.eff_border[2];
	    إذا (screen_opacity_popup != عدم && saved_screen.سطرها != عدم)
		fill_opacity_padding(ردیف, ردیف + cl.eff_padding[2],
			padcol, padendcol, &saved_screen);
	    وإلا
		screen_fill(ردیف, ردیف + cl.eff_padding[2],
					   padcol, padendcol, ' ', ' ', popup_attr);
	}

	إذا (cl.eff_border[2] > 0)
	{
	    // bottom border
	    ردیف = wp->w_winrow + مجموع_ارتفاع_2 - 1;
	    screen_fill(ردیف, ردیف + 1,
		    wincol < 0 ? 0 : wincol,
		    wincol + مجموع_عرض,
		    cl.eff_border[3] != 0 && wp->w_popup_leftoff == 0
					     ? border_char[7] : border_char[2],
		    border_char[2], border_attr[2]);
	    إذا (cl.eff_border[1] > 0)
	    {
		buf[mb_char2bytes(border_char[6], buf)] = NUL;
		screen_puts(buf, ردیف, wincol + مجموع_عرض - 1, border_attr[2]);
	    }
	}

	إذا (wp->w_popup_shadow && wp->w_popup_bottomoff == 0)
	{
	    // bottom shadow
	    ردیف = wp->w_winrow + مجموع_ارتفاع_2;
	    لكل (صحيح ستون = 2 + (wincol < 0 ? 0 : wincol);
		    ستون < wincol + مجموع_عرض; ستون++)
		put_shadow_char(ردیف, ستون);
	}

	إذا (wp->w_popup_close == POPCLOSE_BUTTON)
	{
	    // close button goes on top of anything at the top-right corner
	    buf[mb_char2bytes('X', buf)] = NUL;
	    screen_puts(buf, wp->w_winrow, wincol + مجموع_عرض - 1,
		      wp->w_popup_border[0] > 0 ? border_attr[0] : popup_attr);
	}

	update_popup_transparent(wp, 0);

	popup_free_saved_screen(&saved_screen);

	// Clear popup with opacity context.
	screen_opacity_popup = عدم;

	// Back to the normal zindex.
	screen_zindex = 0;

#اگر_تعریف_شده HAS_پیام_پنجره
	// if this was the message window popup may start the timer now
	may_شروع_پیام_win_timer(wp);
#پایان_اگر

	إذا (override_success)
	    برداشت_highlight_overrides();

#اگر_تعریف_شده FEAT_IMAGE_POPUP
	popup_position_image(wp);
#پایان_اگر
    }

#اگر_تعریف_شده FEAT_PROP_POPUP
    VIM_CLEAR(پایه_screenlines);
    VIM_CLEAR(پایه_screenattrs);
    VIM_CLEAR(پایه_screenlinesuc);
    پایه_screen_ردیفها = 0;
    پایه_screen_تعداد_ستون = 0;
#پایان_اگر

#إذا defined(FEAT_جستجو_EXTRA)
    // In case win_update() called start_search_hl().
    انتها_جستجو_hl();
#پایان_اگر
}

/*
 * Mark references in callbacks of one popup window.
 */
    ساكن صحيح
مجموعه_ref_درون_one_popup(win_T *wp, صحيح copyID)
{
    صحيح		abort = FALSE;
    typval_T	tv;

    إذا (wp->w_close_cb.cb_partial != عدم)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_close_cb.cb_partial;
	abort = abort || مجموعه_ref_درون_قلم(&tv, copyID, عدم, عدم, عدم);
    }
    إذا (wp->w_filter_cb.cb_partial != عدم)
    {
	tv.v_type = VAR_PARTIAL;
	tv.vval.v_partial = wp->w_filter_cb.cb_partial;
	abort = abort || مجموعه_ref_درون_قلم(&tv, copyID, عدم, عدم, عدم);
    }
    abort = abort || مجموعه_ref_درون_list(wp->w_popup_ماسک, copyID);
    أرجع abort;
}

/*
 * Set reference in callbacks of popup windows.
 */
    صحيح
مجموعه_ref_درون_popups(صحيح copyID)
{
    صحيح		abort = FALSE;
    win_T	*wp;
    tabpage_T	*tp;

    لكل (wp = اولی_popupwin; !abort && wp != عدم; wp = wp->w_بعدی)
	abort = abort || مجموعه_ref_درون_one_popup(wp, copyID);

    FOR_ALL_TABPAGES(tp)
    {
	لكل (wp = tp->tp_اولی_popupwin; !abort && wp != عدم; wp = wp->w_بعدی)
	    abort = abort || مجموعه_ref_درون_one_popup(wp, copyID);
	إذا (abort)
	    اكسر;
    }
    أرجع abort;
}

    صحيح
popup_is_popup(win_T *wp)
{
    أرجع wp->w_popup_flags != 0;
}

#إذا defined(FEAT_QUICKFIX)
/*
 * Find an existing popup used as the preview window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_پیدا_کن_preview_پنجره(فراغ)
{
    win_T *wp;

    // Preview window popup is always local to tab page.
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	إذا (wp->w_p_pvw)
	    أرجع wp;
    أرجع عدم;
}

/*
 * Find an existing popup used as the info window, in the current tab page.
 * Return NULL if not found.
 */
    win_T *
popup_پیدا_کن_info_پنجره(فراغ)
{
    win_T *wp;

    // info window popup is always local to tab page.
    FOR_ALL_POPUPWINS_درون_TAB(curtab, wp)
	إذا (wp->w_popup_flags & POPF_INFO)
	    أرجع wp;
    أرجع عدم;
}
#پایان_اگر

/*
 * Scroll the completion info popup one line (by_page false) or one page
 * (by_page true); "dir" negative scrolls up, positive down.
 * Returns true when an info popup was found.
 */
    bool
popup_scroll_info(صحيح dir, bool by_صفحه)
{
#اگر_تعریف_شده FEAT_QUICKFIX
    win_T	*wp = popup_پیدا_کن_info_پنجره();
    صحيح		by;
    linenr_T	new_topline;

    إذا (wp == عدم)
	أرجع false;

    by = by_صفحه ? (wp->w_ارتفاع > 2 ? wp->w_ارتفاع - 1 : 1) : 1;
    new_topline = wp->w_topline + (dir < 0 ? -by : by);
    إذا (new_topline < 1)
	new_topline = 1;
    إذا (new_topline > wp->w_میانگیر->b_ml.ml_سطر_شمار)
	new_topline = wp->w_میانگیر->b_ml.ml_سطر_شمار;
    إذا (new_topline != wp->w_topline)
    {
	مجموعه_topline(wp, new_topline);
	popup_مجموعه_firstline(wp);
	redraw_win_later(wp, UPD_NOT_VALID);
    }
    أرجع true;
#وإلا
    أرجع false;
#پایان_اگر
}

    فراغ
f_popup_findecho(typval_T *argvars استفادهنشده, typval_T *rettv)
{
#اگر_تعریف_شده HAS_پیام_پنجره
    rettv->vval.v_عدد = پیام_win_2 == عدم ? 0 : پیام_win_2->w_id;
#وإلا
    rettv->vval.v_عدد = 0;
#پایان_اگر
}

    فراغ
f_popup_findinfo(typval_T *argvars استفادهنشده, typval_T *rettv)
{
#اگر_تعریف_شده FEAT_QUICKFIX
    win_T   *wp = popup_پیدا_کن_info_پنجره();

    rettv->vval.v_عدد = wp == عدم ? 0 : wp->w_id;
#وإلا
    rettv->vval.v_عدد = 0;
#پایان_اگر
}

    فراغ
f_popup_findpreview(typval_T *argvars استفادهنشده, typval_T *rettv)
{
#اگر_تعریف_شده FEAT_QUICKFIX
    win_T   *wp = popup_پیدا_کن_preview_پنجره();

    rettv->vval.v_عدد = wp == عدم ? 0 : wp->w_id;
#وإلا
    rettv->vval.v_عدد = 0;
#پایان_اگر
}

#إذا defined(FEAT_QUICKFIX)
/*
 * Create a popup to be used as the preview or info window.
 * NOTE: this makes the popup the current window, so that the file can be
 * edited.  However, it must not remain to be the current window, the caller
 * must make sure of that.
 */
    صحيح
popup_create_preview_پنجره(صحيح info)
{
    win_T *wp = popup_create(عدم, عدم, info ? TYPE_INFO : TYPE_PREVIEW);

    إذا (wp == عدم)
	أرجع FAIL;
    إذا (info)
	wp->w_popup_flags |= POPF_INFO;
    وإلا
	wp->w_p_pvw = TRUE;

    // Set the width to a reasonable value, so that w_topline can be computed.
    إذا (wp->w_minwidth > 0)
	wp->w_عرض = wp->w_minwidth;
    وإلا إذا (wp->w_maxwidth > 0)
	wp->w_عرض = wp->w_maxwidth;
    وإلا
	wp->w_عرض = curwin->w_عرض;

    // Will switch to another buffer soon, dummy one can be wiped.
    wp->w_میانگیر->b_locked = FALSE;

    win_enter(wp, FALSE);
    أرجع OK;
}

/*
 * Close any preview popup.
 */
    فراغ
popup_close_preview(فراغ)
{
    win_T *wp = popup_پیدا_کن_preview_پنجره();

    إذا (wp != عدم)
	popup_close_with_retval(wp, -1);
}

/*
 * Hide the info popup.
 */
    فراغ
popup_hide_info(فراغ)
{
    win_T *wp = popup_پیدا_کن_info_پنجره();

    إذا (wp != عدم)
    {
	popup_on_cmdline = wp->w_popup_flags & POPF_ON_CMDLINE;
	popup_hide(wp);
	إذا (State & نما_CMDLINE)
	    // Cmdline mode doesn't normally call update_screen(), so it's
	    // necessary to use pum_call_update_screen() here.
	    pum_call_update_screen();
    }
}

/*
 * Close any info popup.
 */
    فراغ
popup_close_info(فراغ)
{
    win_T *wp = popup_پیدا_کن_info_پنجره();

    إذا (wp != عدم)
	popup_close_with_retval(wp, -1);
}
#پایان_اگر

/*
 * Returns TRUE if a popup extends into the cmdline area.
 */
    صحيح
popup_overlaps_cmdline(فراغ)
{
    أرجع popup_on_cmdline;
}

#إذا defined(HAS_پیام_پنجره)

/*
 * Get the message window.
 * Returns NULL if something failed.
 */
    win_T *
popup_get_پیام_win(فراغ)
{
    إذا (پیام_win_2 != عدم)
	أرجع پیام_win_2;

    صحيح i;

    پیام_win_2 = popup_create(عدم, عدم, TYPE_پیام_WIN);

    إذا (پیام_win_2 == عدم)
	أرجع عدم;

    // use the full screen width
    پیام_win_2->w_عرض = Columns;

    // position at bottom of screen
    پیام_win_2->w_popup_pos = POPPOS_BOTTOM;
    پیام_win_2->w_wantcol = 1;
    پیام_win_2->w_minwidth = 9999;
    پیام_win_2->w_firstline = -1;

    // no padding, border at the top
    لكل (i = 0; i < 4; ++i)
	پیام_win_2->w_popup_padding[i] = 0;
    لكل (i = 1; i < 4; ++i)
	پیام_win_2->w_popup_border[i] = 0;

    إذا (پیام_win_2->w_popup_timer != عدم)
	پیام_win_2->w_popup_timer->tr_keep = TRUE;
    أرجع پیام_win_2;
}

/*
 * If the message window is not visible: show it
 * If the message window is visible: reset the timeout
 */
    فراغ
popup_show_پیام_win(فراغ)
{
    إذا (پیام_win_2 == عدم)
	أرجع;

    إذا ((پیام_win_2->w_popup_flags & POPF_HIDDEN) != 0)
    {
	// the highlight may have changed.
	popup_update_color(پیام_win_2, TYPE_پیام_WIN);
	popup_show(پیام_win_2);
    }
    شروع_پیام_win_timer = TRUE;
}

    ساكن فراغ
may_شروع_پیام_win_timer(win_T *wp)
{
    إذا (wp == پیام_win_2 && شروع_پیام_win_timer)
    {
	إذا (پیام_win_2->w_popup_timer != عدم)
	{
	    پیام_win_2->w_popup_timer->tr_interval = پیام_win_زمان;
	    timer_شروع(پیام_win_2->w_popup_timer);
	    پیام_win_زمان = 3000;
	}
	شروع_پیام_win_timer = FALSE;
    }
}

    صحيح
popup_پیام_win_visible(فراغ)
{
    أرجع پیام_win_2 != عدم
	&& (پیام_win_2->w_popup_flags & POPF_HIDDEN) == 0;
}

/*
 * If the message window is visible: hide it.
 */
    فراغ
popup_hide_پیام_win(فراغ)
{
    إذا (پیام_win_2 != عدم)
	popup_hide(پیام_win_2);
}

// Values saved in start_echowindow() and restored in end_echowindow()
ساكن صحيح save_پیام_didout = FALSE;
ساكن صحيح save_پیام_ستون = 0;
// Values saved in end_echowindow() and restored in start_echowindow()
ساكن صحيح ew_پیام_didout = FALSE;
ساكن صحيح ew_پیام_ستون = 0;

/*
 * Invoked before outputting a message for ":echowindow".
 * "time_sec" is the display time, zero means using the default 3 sec.
 */
    فراغ
شروع_echowindow(صحيح زمان_sec)
{
    درون_echowindow = TRUE;
    save_پیام_didout = پیام_didout;
    save_پیام_ستون = پیام_ستون;
    پیام_didout = ew_پیام_didout;
    پیام_ستون = ew_پیام_ستون;
    إذا (زمان_sec != 0)
	پیام_win_زمان = زمان_sec * 1000;
}

/*
 * Invoked after outputting a message for ":echowindow".
 */
    فراغ
انتها_echowindow(فراغ)
{
    درون_echowindow = FALSE;

    إذا ((State & نما_HITRETURN) == 0)
	// show the message window now
	redraw_cmd(FALSE);

    // do not overwrite messages
    ew_پیام_didout = TRUE;
    ew_پیام_ستون = پیام_ستون == 0 ? 1 : پیام_ستون;
    پیام_didout = save_پیام_didout;
    پیام_ستون = save_پیام_ستون;
}
#پایان_اگر

/*
 * Close any popup for a text property associated with "win".
 * Return TRUE if a popup was closed.
 */
    صحيح
popup_win_closed(win_T *win)
{
    صحيح	    دور;
    win_T   *wp;
    win_T   *بعدی;
    صحيح	    ret = FALSE;

    لكل (دور = 1; دور <= 2; ++دور)
	لكل (wp = دور == 1 ? اولی_popupwin : curtab->tp_اولی_popupwin;
							 wp != عدم; wp = بعدی)
	{
	    بعدی = wp->w_بعدی;
	    إذا (wp->w_popup_prop_win == win)
	    {
		popup_close_with_retval(wp, -1);
		ret = TRUE;
	    }
	}
    أرجع ret;
}

/*
 * Set the title of the popup window to the file name.
 */
    فراغ
popup_مجموعه_title(win_T *wp)
{
    إذا (wp->w_میانگیر->b_fname == عدم)
	أرجع;

    char_u	dirname[MAXPATHL];
    size_t	len;

    mch_dirname(dirname, MAXPATHL);
    shorten_buf_fname(wp->w_میانگیر, dirname, FALSE);

    vim_free(wp->w_popup_title);
    len = STRLEN(wp->w_میانگیر->b_fname) + 3;
    wp->w_popup_title = alloc(len);
    إذا (wp->w_popup_title != عدم)
	vim_snprintf((حرف *)wp->w_popup_title, len, " %s ",
		wp->w_میانگیر->b_fname);
    redraw_win_later(wp, UPD_VALID);
}

#إذا defined(FEAT_QUICKFIX)
/*
 * If there is a preview window, update the title.
 * Used after changing directory.
 */
    فراغ
popup_update_preview_title(فراغ)
{
    win_T *wp = popup_پیدا_کن_preview_پنجره();

    إذا (wp != عدم)
	popup_مجموعه_title(wp);
}
#پایان_اگر

#پایان_اگر // FEAT_PROP_POPUP
